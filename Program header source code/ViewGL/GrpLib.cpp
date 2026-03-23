#include "stdafx.h"
#include <windows.h>
#include "./grplib.h"
#include "../zlib/zlib.h"
#include <assert.h>
#include <D3d8.h>
#include <D3dx8tex.h>
#include <Ole2.h>
#include ".\grplib.h"
#include "../MakeGL/TgaCore.h"
#include <Mmsystem.h>
#include <string>
#pragma comment (lib, "Winmm.lib")
#pragma comment (lib, "D3dx8.lib")
#pragma comment (lib, "D3d8.lib")
#pragma  comment(lib,"gdiplus.lib")

static IDirect3DDevice8* g_pD3DD = NULL;
static IDirect3D8* g_pD3D = NULL;

DWORD GetDesktopBitsPixel()
{
	HDC hDC = GetDC(NULL);
	DWORD dwBitsPixel = GetDeviceCaps(hDC,BITSPIXEL);
	ReleaseDC(NULL,hDC);
	return dwBitsPixel;
}

IDirect3DDevice8* GetD3DDevice()
{
	return g_pD3DD;
}

BOOL InitD3DDevice(HWND hWnd)
{
	HRESULT hr;
	g_pD3D = Direct3DCreate8(D3D_SDK_VERSION);

	if(!g_pD3D) return FALSE;

	D3DPRESENT_PARAMETERS d3dparam;

	memset(&d3dparam,0,sizeof(d3dparam));
	d3dparam.BackBufferCount = 1;
	d3dparam.BackBufferFormat = (GetDesktopBitsPixel() == 32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5);
	d3dparam.BackBufferHeight = 480;
	d3dparam.BackBufferWidth = 640;
	d3dparam.hDeviceWindow = hWnd;
	d3dparam.Windowed = TRUE;
	d3dparam.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dparam.AutoDepthStencilFormat	= D3DFMT_D16;
	hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_REF,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dparam,&g_pD3DD);
	if(FAILED(hr)) 
	{
		g_pD3D->Release();
		g_pD3D = NULL;
		return FALSE;
	}
	return TRUE;
}

BOOL DestroyD3DDevice()
{
	if(g_pD3DD) {
		g_pD3DD->Release();
		g_pD3DD = NULL;
	}
	if(g_pD3D)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}
	return TRUE;
}

static int uncompress_file(FILE* file,unsigned long nInLen,unsigned char* pOut,unsigned long nOutLen)
{
	const unsigned long c_bufLen = (1<<14);
	static unsigned char dataZip[c_bufLen];
	static z_stream_s zipStream;

	zipStream.zalloc = Z_NULL;
	zipStream.zfree  = Z_NULL;
	zipStream.opaque = Z_NULL;

	zipStream.next_out = pOut;
	zipStream.avail_out = nOutLen;
	zipStream.total_out = 0;

	zipStream.next_in  = NULL;
	zipStream.avail_in = 0;
	zipStream.total_in = 0;

	int err;
	err = inflateInit2(&zipStream, -MAX_WBITS);
	if (err != Z_OK) return err;
	while(nInLen)
	{
		unsigned long readLen;
		if(nInLen > c_bufLen)   readLen = c_bufLen;
		else                    readLen = nInLen;
		nInLen -= readLen;

		fread((char*)dataZip,readLen,1,file);

		zipStream.next_in  = dataZip;
		zipStream.total_in = 0;
		zipStream.avail_in = readLen;
		err = inflate(&zipStream,Z_SYNC_FLUSH);

		assert(err != Z_BUF_ERROR);
		assert(err == Z_OK || err == Z_STREAM_END);
		if(err == Z_STREAM_END)
			break;
	}

	err = inflateEnd(&zipStream);
	return err;
}

CGrpLib::CGrpLib(void)
: m_pCurBlock(NULL)
{
}

CGrpLib::~CGrpLib(void)
{
}

struct stGrpBlockEx2 : public stGrpBlock
{
	DWORD	dwFileOffset;			// 文件位置，相对数据区的位置
	DWORD	dwFileCompressSize;		// 未压缩后大小
	DWORD	dwFileSize;				// 文件大小
	BOOL	bCompress;				// 是否ZIP压缩
};

bool CGrpLib::CheckGroupID()
{
	for(size_t i=0;i<m_aGroup.size();++i)
	{
		stGrpGroup & grp = m_aGroup[i];
		for(int j = i-1;j>=0;--j)
		{
			if(grp.dwGroupID <= m_aGroup[j].dwGroupID)
			{
				ASSERT(0);
				return false;
			}
		}
	}
	return true;
}

BOOL CGrpLib::Open(const char* pszFileName)
{
	m_strFileName = pszFileName;

	m_pCurBlock = NULL;

	FILE* fp = fopen(pszFileName,"rb");
	fread(&m_hdr,sizeof(m_hdr),1,fp);

	if(strncmp((char*)&m_hdr.dwMagic,GRP_FILE_MAGIC,4))
	{
		fclose(fp);
		return FALSE;
	}

	if(m_hdr.wdVersion != c_GrpVersion)
	{
		fclose(fp);
		return FALSE;
	}

	m_aGroup.clear();
	m_aGroup.resize(m_hdr.wdDirCount);

	fseek(fp,(long)m_hdr.dwHeaderDataOffset,SEEK_SET);
	for(size_t i=0;i<m_hdr.wdDirCount;++i)
	{
		DWORD dwFileNum;
		DWORD dwGroupID;

		fread(&dwGroupID,sizeof(DWORD),1,fp);
		fread(&dwFileNum,sizeof(DWORD),1,fp);
		
		BOOL bMultiFrame = (dwFileNum & 0x80000000);
		dwFileNum &= 0x7fffffff;

		stGrpGroup & aAni = m_aGroup[i];

		aAni.dwGroupID = dwGroupID;
		aAni.Anis.resize(dwFileNum);

		for(size_t j=0;j<dwFileNum;++j)
		{
			stPackAniInfo aniInfo;

			if(bMultiFrame)
				fread(&aniInfo,sizeof(aniInfo),1,fp);
			else
				InitPackAniInfo(&aniInfo);

			stAnimation & ani = aAni.Anis[j];
			ani.aniInfo = aniInfo;
			ani.bmps.resize(aniInfo.wdBitmapCount);

			for(size_t k=0;k<aniInfo.wdBitmapCount;++k)
			{
				stGrpBlockEx & bst = ani.bmps[k].m_st;
				/*
				if(m_hdr.wdVersion == 2)
				{
					stGrpBlockEx2 st2;
					fread(&st2,sizeof(st2),1,fp);
					memcpy(&bst,&st2,sizeof(stGrpBlock));
					bst.bCompress = st2.bCompress;
					//bst.bCompress = 0;
					bst.dwFileSize = st2.dwFileSize;
					bst.dwFileOffset = st2.dwFileOffset;
					bst.dwFileCompressSize = st2.dwFileCompressSize;
					bst.dwCRC = 0xffffffff;
				}
				else
				//*/
				{
					fread(&ani.bmps[k].m_st,sizeof(stGrpBlockEx),1,fp);
				}
			}
		}		
	}
	fclose(fp);
#ifdef _DEBUG
	if(!CheckGroupID())
	{
		AfxMessageBox(_T("GroupID 错误！"));
	}
#endif
	return TRUE;
}

CGrpBlock::CGrpBlock()
: m_bmp(NULL)
{
}

CGrpBlock::~CGrpBlock()
{
	Release();
}

void CGrpBlock::Release()
{
	delete m_bmp;
	m_bmp = NULL;
}

void IndexToColor(stRgb* pal,BYTE* pData,size_t width,DWORD* pRetData)
{
	BYTE* end = pData + width;
	for(;pData < end;++pData)
	{
		*pRetData ++ = pal[*pData].color;
		//*pRetData ++ = 0xffffffff;
		ASSERT(pal[*pData].a == 255);
	}
}

void AlphaToColor(BYTE* pAlpha,size_t width,DWORD* pRetData)
{
	BYTE* end = pAlpha + width;
	for(;pAlpha < end;++pAlpha)
	{
		*pRetData ++ &= (((*pAlpha) << 24) | 0xffffff);
	}
}

void ColorToColor565(WORD* pData,size_t width,DWORD* pRetData)
{
	WORD* end = pData + width;
	for(;pData < end;++pData)
	{
		*pRetData ++ = Color565To888(*pData);
	}
}

void ColorToColor4444(WORD* pData,size_t width,DWORD* pRetData)
{
	WORD* end = pData + width;
	for(;pData < end;++pData)
	{
		*pRetData ++ = Color4444To8888(*pData);
	}
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

class CGlobalAlloc{
public:
	HGLOBAL m_hGlobal;

	void* Lock(){ return ::GlobalLock(m_hGlobal);}

	BOOL Unlock(){ return ::GlobalUnlock(m_hGlobal);}

	CGlobalAlloc() : m_hGlobal(NULL){}

	CGlobalAlloc(DWORD dwFlags,size_t nBytes)
	{
		Create(dwFlags,nBytes);
	}

	~CGlobalAlloc()
	{
		if(m_hGlobal)
			::GlobalFree(m_hGlobal);
	}

	BOOL Create(DWORD dwFlags,size_t nBytes)
	{
		m_hGlobal = ::GlobalAlloc(dwFlags,nBytes);
		return m_hGlobal != NULL;
	}

	HGLOBAL GetHandle(){ return m_hGlobal;}
};
#define COLOR_ARGB( a, r, g, b )	( (DWORD)( ( ((a) & 0xff) << 24 ) | ( ((r) & 0xff) << 16 ) | ( ((g) & 0xff)<< 8  ) | ( ((b) & 0xff)  ) ) )

BOOL CGrpBlock::LoadFromFile(const char* pszFileName,bool bTestLoad)
{
	Release();

	FILE* fp = fopen(pszFileName,"rb");
	fseek(fp,m_st.dwFileOffset + sizeof(stGrpFileHeader),SEEK_SET);

	CGlobalAlloc cga(GMEM_FIXED,m_st.dwFileSize);
	BYTE* buf = (BYTE*) cga.GetHandle();

	//*
	if(m_st.dwCompressType == eGrpCompress_Zip)
	{
		if(Z_OK != uncompress_file(fp,m_st.dwFileCompressSize,buf,m_st.dwFileSize))
		{
			assert(0);
			fclose(fp);
			return FALSE;
		}
	}
	else
	//*/
	{
		fread(buf,m_st.dwFileCompressSize,1,fp);
	}
	
	fclose(fp);

	if(bTestLoad) return TRUE;

	BYTE* pData = buf;
	
	size_t imgX = 0,imgY =0;

	switch(m_st.byGrpType)
	{
	case eGrpTypeRle:
		{
			m_bmp = new Gdiplus::Bitmap(GetWidth(),GetHeight(),PixelFormat32bppARGB);
			Gdiplus::Rect rcImg(0,0,GetWidth(),GetHeight());
			Gdiplus::BitmapData bmpData;
			Gdiplus::Status satus = m_bmp->LockBits(&rcImg,Gdiplus::ImageLockModeWrite,PixelFormat32bppARGB,&bmpData);
			assert(satus == Gdiplus::Ok);
			if(satus != Gdiplus::Ok) return FALSE;

			DWORD* pImgData = ((DWORD*)bmpData.Scan0) + m_st.wdWidth * m_st.wdClipTop + m_st.wdClipLeft;

			DWORD* blkDataOffset;

			stRgb *pal = (stRgb*)pData;
			pData += sizeof(stRgb) * m_st.wdPalCount;
			blkDataOffset = (DWORD*)pData;

			pData += sizeof(DWORD) * (size_t)m_st.byTotalHeightBlock * (size_t)m_st.byTotalWidthBlock;

			if(m_st.byPixelFormat == ePixPal8 || m_st.byPixelFormat == ePixPal8NotAlpha)
			{
				size_t blockHeight = MAX_TEXTUREWIDTH;
				for(size_t i=0;i<m_st.byTotalHeightBlock;++i)
				{
					imgY = i * MAX_TEXTUREWIDTH;
					if(i == m_st.byTotalHeightBlock - 1)
					{
						blockHeight = GetTailWidth(m_st.wdClipHeight);
					}
					size_t blockWidth = MAX_TEXTUREWIDTH;
					for(size_t j=0;j<m_st.byTotalWidthBlock;++j)
					{
						imgX = j * MAX_TEXTUREWIDTH;
						if(j == m_st.byTotalWidthBlock - 1)
						{
							blockWidth = GetTailWidth(m_st.wdClipWidth);
						}
						
						BYTE* colorData = pData + blkDataOffset[i * (size_t)m_st.byTotalWidthBlock + j];

						size_t alphaLen;
						BYTE* alphaData;
						if(m_st.byPixelFormat == ePixPal8)
						{
							size_t colorLen =  *(DWORD*)colorData;
							colorData += 4;
							alphaData = colorData + colorLen;
						}
						
						DWORD * pImg = pImgData + imgY * m_st.wdWidth + imgX;
						for(size_t k = 0;k < blockHeight;++k)
						{
							//static BYTE tmpBuf[MAX_TEXTUREWIDTH * 2];
							RLE_DecodeALine(colorData,blockWidth,pImg,pal,stRleDecodeIndexTo8888());
							//IndexToColor(pal,tmpBuf,blockWidth,pImg);
							if(m_st.byPixelFormat == ePixPal8)
							{
								RLE_DecodeALine(alphaData,blockWidth,pImg,NULL,stRleDecodeAlphaTo8888());
								//AlphaToColor(tmpBuf,blockWidth,pImg);
							}
							pImg += m_st.wdWidth;
						}
					}
				}
			}
			else
			{
				WORD* rleData;
				size_t blockHeight = MAX_TEXTUREWIDTH;
				for(size_t i=0;i<m_st.byTotalHeightBlock;++i)
				{
					imgY = i * MAX_TEXTUREWIDTH;
					if(i == m_st.byTotalHeightBlock - 1)
					{
						blockHeight = GetTailWidth(m_st.wdClipHeight);
					}
					size_t blockWidth = MAX_TEXTUREWIDTH;
					for(size_t j=0;j<m_st.byTotalWidthBlock;++j)
					{
						imgX = j * MAX_TEXTUREWIDTH;
						if(j == m_st.byTotalWidthBlock - 1)
						{
							blockWidth = GetTailWidth(m_st.wdClipWidth);
						}

						rleData = (WORD*)(pData + blkDataOffset[i * (size_t)m_st.byTotalWidthBlock + j]);

						DWORD * pImg = pImgData + imgY * m_st.wdWidth + imgX;
						if(m_st.byPixelFormat == ePix4444)
						{
							for(size_t k = 0;k < blockHeight;++k)
							{
								//TRACE("k=%u\n",k);
								//WORD tmpBuf[MAX_TEXTUREWIDTH];
								//RLE_DecodeALine(rleData,blockWidth,tmpBuf);
								//ColorToColor4444(tmpBuf,blockWidth,pImg);
								RLE_DecodeALine(rleData,blockWidth,pImg,NULL,stRleDecode4444To8888());
								pImg += m_st.wdWidth;
							}
						}
						else
						{
							for(size_t k = 0;k < blockHeight;++k)
							{
								//WORD tmpBuf[MAX_TEXTUREWIDTH];
								//RLE_DecodeALine(rleData,blockWidth,tmpBuf);
								//ColorToColor565(tmpBuf,blockWidth,pImg);
								RLE_DecodeALine(rleData,blockWidth,pImg,NULL,stRleDecode565To888());
								pImg += m_st.wdWidth;
							}
						}
					}
				}
			}
			m_bmp->UnlockBits(&bmpData);
		}
		break;
	case eGrpTypeTexRle:
		{
			LPDIRECT3DSURFACE8 sur;
			HRESULT hr;
			DWORD blkWidth,blkHeight;
			DWORD blkTrueWidth,blkTrueHeight;
			DWORD	pitch;
			DWORD	dwTexBlkSize;
			dwTexBlkSize = ( (m_st.byPixelFormat == ePixDXT1 || m_st.byPixelFormat == ePixDXT1_5551) ? 8 : 16);
			pitch = dwTexBlkSize * (MAX_TEXTUREWIDTH/4);
			m_bmp = new Gdiplus::Bitmap(GetWidth(),GetHeight(),PixelFormat32bppARGB);
			if(m_st.wdClipHeight == 0 || m_st.wdClipWidth == 0)
				return TRUE;

			static BYTE s_UnRleData[16 * (MAX_TEXTUREWIDTH/4) * (MAX_TEXTUREWIDTH/4)];
			hr = g_pD3DD->CreateImageSurface(MAX_TEXTUREWIDTH,MAX_TEXTUREWIDTH,D3DFMT_A8R8G8B8,&sur);
			if(FAILED(hr))
			{
				ATLASSERT(0);
				return FALSE;
			}
			
			Gdiplus::Rect rcImg(0,0,GetWidth(),GetHeight());
			Gdiplus::BitmapData bmpData;
			Gdiplus::Status satus = m_bmp->LockBits(&rcImg,Gdiplus::ImageLockModeWrite,PixelFormat32bppARGB,&bmpData);
			assert(satus == Gdiplus::Ok);
			if(satus != Gdiplus::Ok) 
			{
				sur->Release();
				ATLASSERT(0);
				return FALSE;
			}
			
			DWORD* blkDataOffset;

			blkDataOffset = (DWORD*)pData;
			pData += sizeof(DWORD) * (size_t)m_st.byTotalHeightBlock * (size_t)m_st.byTotalWidthBlock;

			DWORD* pImgData = ((DWORD*)bmpData.Scan0) + m_st.wdWidth * m_st.wdClipTop + m_st.wdClipLeft;
			for(size_t i=0;i<m_st.byTotalHeightBlock;++i)
			{
				if( i == m_st.byTotalHeightBlock - 1)
				{
					blkTrueHeight = GetTailWidth(m_st.wdClipHeight);
					blkHeight = GetWidth4(blkTrueHeight);
				}
				else
				{
					blkTrueHeight = MAX_TEXTUREWIDTH;
					blkHeight = MAX_TEXTUREWIDTH;
				}
				
				for(size_t j=0;j<m_st.byTotalWidthBlock;++j)
				{
					if( j == m_st.byTotalWidthBlock - 1)
					{
						blkTrueWidth = GetTailWidth(m_st.wdClipWidth);
						blkWidth = GetWidth4(blkTrueWidth);
					}
					else
					{
						blkTrueWidth = MAX_TEXTUREWIDTH;
						blkWidth = MAX_TEXTUREWIDTH;
					}

					D3DLOCKED_RECT lock_rect;

					BYTE* pRleData0 = pData + blkDataOffset[i * m_st.byTotalWidthBlock + j];

					DWORD dwTexRowCount = blkHeight/4;
					DWORD dwTexRowSize = blkWidth/4 * dwTexBlkSize;
					BYTE* pUnRleData = s_UnRleData;
					for(size_t i1 = 0;i1 < dwTexRowCount;++i1)
					{
						RLE_DecodeALine(pRleData0,dwTexRowSize,pUnRleData,NULL,stRleDecodeByteToByte());
						//memcpy(pUnRleData,pRleData0,dwTexRowSize);
						//pRleData0 += dwTexRowSize;

						pUnRleData += pitch;
					}

					RECT rect = {0,0,blkWidth,blkHeight};
					hr = D3DXLoadSurfaceFromMemory(sur,NULL,&rect,s_UnRleData,GrpGetD3DFormat((enumPixelFormat)m_st.byPixelFormat),pitch,NULL,&rect,D3DX_FILTER_NONE,0);
					if(FAILED(hr))
					{
						sur->Release();
						ATLASSERT(0);
						return FALSE;
					}

					hr = sur->LockRect(&lock_rect,NULL,0);
					if(FAILED(hr))
					{
						sur->Release();
						ATLASSERT(0);
						return FALSE;
					}
					
					int x,y;
					y = i * MAX_TEXTUREWIDTH ;
					x = j * MAX_TEXTUREWIDTH ;
					DWORD* pDstData = &pImgData[y * m_st.wdWidth + x];
					for(size_t i=0;i<blkTrueHeight;++i)
					{
						memcpy(pDstData,lock_rect.pBits,blkTrueWidth * 4);
						pDstData += m_st.wdWidth;
						lock_rect.pBits = (BYTE*)lock_rect.pBits + lock_rect.Pitch;
					}
					sur->UnlockRect();
				}
			}
			m_bmp->UnlockBits(&bmpData);
			sur->Release();
			
		}
		break;
	case eGrpTypeTexA4Rle:
		{
			HRESULT hr;
			DWORD blkWidth,blkHeight;
			DWORD blkTrueWidth,blkTrueHeight;
			DWORD	pitch;
			DWORD	dwTexBlkSize;
			dwTexBlkSize = ( (m_st.byPixelFormat == ePixDXT1 || m_st.byPixelFormat == ePixDXT1_5551) ? 8 : 16);
			pitch = dwTexBlkSize * (MAX_TEXTUREWIDTH/4);
			m_bmp = new Gdiplus::Bitmap(GetWidth(),GetHeight(),PixelFormat32bppARGB);
			if(m_st.wdClipHeight == 0 || m_st.wdClipWidth == 0)
				return TRUE;

			Gdiplus::Rect rcImg(0,0,GetWidth(),GetHeight());
			Gdiplus::BitmapData bmpData;
			Gdiplus::Status satus = m_bmp->LockBits(&rcImg,Gdiplus::ImageLockModeWrite,PixelFormat32bppARGB,&bmpData);
			assert(satus == Gdiplus::Ok);
			if(satus != Gdiplus::Ok) 
			{
				return FALSE;
			}

			DWORD* blkDataOffset;

			blkDataOffset = (DWORD*)pData;
			pData += sizeof(DWORD) * (size_t)m_st.byTotalHeightBlock * (size_t)m_st.byTotalWidthBlock;

			DWORD* pImgData = ((DWORD*)bmpData.Scan0) + m_st.wdWidth * m_st.wdClipTop + m_st.wdClipLeft;
			for(size_t i=0;i<m_st.byTotalHeightBlock;++i)
			{
				if( i == m_st.byTotalHeightBlock - 1)
				{
					blkTrueHeight = GetTailWidth(m_st.wdClipHeight);
					blkHeight = GetWidth4(blkTrueHeight);
				}
				else
				{
					blkTrueHeight = MAX_TEXTUREWIDTH;
					blkHeight = MAX_TEXTUREWIDTH;
				}

				for(size_t j=0;j<m_st.byTotalWidthBlock;++j)
				{
					if( j == m_st.byTotalWidthBlock - 1)
					{
						blkTrueWidth = GetTailWidth(m_st.wdClipWidth);
						blkWidth = GetWidth4(blkTrueWidth);
					}
					else
					{
						blkTrueWidth = MAX_TEXTUREWIDTH;
						blkWidth = MAX_TEXTUREWIDTH;
					}

					D3DLOCKED_RECT lock_rect;

					BYTE* pRleData0 = pData + blkDataOffset[i * m_st.byTotalWidthBlock + j];


					DWORD dwTexRowCount = blkHeight/4;
					DWORD dwTexRowSize = blkWidth/4 * dwTexBlkSize;
					DWORD dwTexColCount = blkWidth/4;
					for(size_t i1 = 0;i1 < dwTexRowCount;++i1)
					{
						static BYTE s_buf[MAX_TEXTUREWIDTH/4 * 16];

						RLE_DecodeALine(pRleData0,dwTexColCount*8,s_buf,NULL,stRleDecodeByteToByte());

						BYTE* pData0 = s_buf;
						for(size_t j1=0;j1<dwTexColCount;++j1)
						{
							for(size_t k=0;k<4;++k)
							{
								int x,y;
								y = i * MAX_TEXTUREWIDTH + i1 * 4 + k;
								x = j * MAX_TEXTUREWIDTH + j1 * 4;

								DWORD * pDst = &pImgData[ y * m_st.wdWidth + x];

								pDst[0] = ((pData0[0] & 0xf) << 28) | 0xffffff;
								pDst[1] = ((pData0[0] & 0xf0) << 24) | 0xffffff;
								pDst[2] = ((pData0[1] & 0xf) << 28) | 0xffffff;
								pDst[3] = ((pData0[1] & 0xf0) << 24) | 0xffffff;

								pData0 += 2;
							}
						}
					}
				}
			}
			m_bmp->UnlockBits(&bmpData);

		}
		break;
	case eGrpTypeTex:
		{
			LPDIRECT3DSURFACE8 sur;
			HRESULT hr;
			DWORD dwDataWidth,dwDataHeight;
			DWORD	dwTexBlkSize;
			DWORD	pitch;
			D3DLOCKED_RECT lock_rect;
			dwDataWidth = GetWidth4(m_st.wdClipWidth);
			dwDataHeight = GetWidth4(m_st.wdClipHeight);

			if(dwDataWidth || dwDataHeight)
			{
				dwTexBlkSize = ( (m_st.byPixelFormat == ePixDXT1 || m_st.byPixelFormat == ePixDXT1_5551) ? 8 : 16);
				pitch = dwTexBlkSize * dwDataWidth/4;
				hr = g_pD3DD->CreateImageSurface(dwDataWidth,dwDataHeight,D3DFMT_A8R8G8B8,&sur);
				if(FAILED(hr))
				{
					ATLASSERT(0);
					return FALSE;
				}
				RECT rect = {0,0,dwDataWidth,dwDataHeight};
				hr = D3DXLoadSurfaceFromMemory(sur,NULL,&rect,pData,GrpGetD3DFormat((enumPixelFormat)m_st.byPixelFormat),pitch,NULL,&rect,D3DX_FILTER_NONE,0);
				if(FAILED(hr))
				{
					sur->Release();
					ATLASSERT(0);
					return FALSE;
				}

				hr = sur->LockRect(&lock_rect,NULL,0);
				if(FAILED(hr))
				{
					sur->Release();
					ATLASSERT(0);
					return FALSE;
				}
			}

			m_bmp = new Gdiplus::Bitmap(GetWidth(),GetHeight(),PixelFormat32bppARGB);
			if(dwDataWidth || dwDataHeight)
			{
				Gdiplus::Rect rcImg(0,0,GetWidth(),GetHeight());
				Gdiplus::BitmapData bmpData;
				Gdiplus::Status satus = m_bmp->LockBits(&rcImg,Gdiplus::ImageLockModeWrite,PixelFormat32bppARGB,&bmpData);
				assert(satus == Gdiplus::Ok);
				if(satus != Gdiplus::Ok) 
				{
					sur->Release();
					ATLASSERT(0);
					return FALSE;
				}

				DWORD* pImgData = ((DWORD*)bmpData.Scan0) + m_st.wdWidth * m_st.wdClipTop + m_st.wdClipLeft;

				for(size_t i=0;i<m_st.wdClipHeight;++i)
				{
					memcpy(pImgData,lock_rect.pBits,m_st.wdClipWidth * 4);
					pImgData += m_st.wdWidth;
					lock_rect.pBits = (BYTE*)lock_rect.pBits + lock_rect.Pitch;
				}

				m_bmp->UnlockBits(&bmpData);
				sur->Release();
			}
			
		}
		break;
	case eGrpTypeTexA4:
		{
			m_bmp = new Gdiplus::Bitmap(GetWidth(),GetHeight(),PixelFormat32bppARGB);
			DWORD dwDataWidth = GetWidth4(m_st.wdClipWidth);
			DWORD dwDataHeight = GetWidth4(m_st.wdClipHeight);

			if(dwDataWidth || dwDataHeight)
			{
				Gdiplus::Rect rcImg(0,0,GetWidth(),GetHeight());
				Gdiplus::BitmapData bmpData;
				Gdiplus::Status satus = m_bmp->LockBits(&rcImg,Gdiplus::ImageLockModeWrite,PixelFormat32bppARGB,&bmpData);
				assert(satus == Gdiplus::Ok);
				if(satus != Gdiplus::Ok) 
				{
					ATLASSERT(0);
					return FALSE;
				}

				DWORD* pImgData = ((DWORD*)bmpData.Scan0) + m_st.wdWidth * m_st.wdClipTop + m_st.wdClipLeft;
				
				DWORD dwBlkRowCount = dwDataHeight/4;
				DWORD dwBlkColCount = dwDataWidth/4;
				BYTE* pData0 = pData;
				for(size_t i=0;i<dwBlkRowCount;++i)
				{
					for(size_t j=0;j<dwBlkColCount;++j)
					{
						for(size_t k=0;k<4;++k)
						{
							int x = j * 4;
							int y = i * 4 + k;

							DWORD * pDst = &pImgData[ y * m_st.wdWidth + x];

							pDst[0] = ((pData0[0] & 0xf) << 28) | 0xffffff;
							pDst[1] = ((pData0[0] & 0xf0) << 24) | 0xffffff;
							pDst[2] = ((pData0[1] & 0xf) << 28) | 0xffffff;
							pDst[3] = ((pData0[1] & 0xf0) << 24) | 0xffffff;

							pData0 += 2;
						}
					}
				}

				m_bmp->UnlockBits(&bmpData);
			}
		}
		break;
	case eGrpTypeJpeg:
		{
			IStream * pStr;
			HRESULT hr;
			CGlobalAlloc cga1;

			HGLOBAL h1 = NULL;
			if(m_st.byPixelFormat == ePixJpegARGB)
			{
				DWORD jpegLen = *(DWORD*)buf;
				cga1.Create(GMEM_FIXED,jpegLen);
				BYTE* jpegBuf = (BYTE*)cga1.GetHandle();
				memcpy(jpegBuf,buf + 4,jpegLen);
			}
			else
			{
				DWORD jpegLen = m_st.dwFileSize;
				cga1.Create(GMEM_FIXED,jpegLen);
				BYTE* jpegBuf = (BYTE*)cga1.GetHandle();
				memcpy(jpegBuf,buf,jpegLen);
			}

			hr = CreateStreamOnHGlobal(cga1.GetHandle(),FALSE,&pStr);
			if(FAILED(hr))
			{
				return FALSE;
			}

			LARGE_INTEGER pos;
			ULARGE_INTEGER newPos;
			//if(m_st.byPixelFormat == ePixJpegARGB)
			//{
			//	pos.QuadPart = 4;
			//	hr = pStr->Seek(pos,STREAM_SEEK_SET,&newPos);
			//}

			Gdiplus::Image * img = Gdiplus::Image::FromStream(pStr);
			pStr->Release();
			if(!img) 
			{
				return FALSE;
			}
			
			m_bmp = new Gdiplus::Bitmap(GetWidth(),GetHeight(),PixelFormat32bppARGB);
			Gdiplus::Graphics grp(m_bmp);
			Gdiplus::Status status = grp.DrawImage(img,m_st.wdClipLeft,m_st.wdClipTop,0,0,m_st.wdClipWidth,m_st.wdClipHeight,Gdiplus::UnitPixel);
			delete img;
			if(status != Gdiplus::Ok) return FALSE;

			if(m_st.byPixelFormat == ePixJpegARGB)
			{
				Gdiplus::BitmapData bmpData;
				Gdiplus::Rect rcImg(0,0,GetWidth(),GetHeight());
				Gdiplus::Status satus = m_bmp->LockBits(&rcImg,Gdiplus::ImageLockModeWrite,PixelFormat32bppARGB,&bmpData);
				assert(satus == Gdiplus::Ok);
				if(satus != Gdiplus::Ok) return FALSE;
				DWORD* pImgData = ((DWORD*)bmpData.Scan0) + m_st.wdWidth * m_st.wdClipTop + m_st.wdClipLeft;

				DWORD jpegLen = *(DWORD*)buf;
				BYTE* rleAlpha = (BYTE*)buf + 4 + jpegLen;
				//BYTE * tmpBuf = new BYTE[m_st.wdClipWidth];
				for(size_t i=0;i<m_st.wdClipHeight;++i)
				{
					RLE_DecodeALine(rleAlpha,m_st.wdClipWidth,pImgData,NULL,stRleDecodeAlphaTo8888());
					//AlphaToColor(tmpBuf,m_st.wdClipWidth,pImgData);
					pImgData += m_st.wdWidth;
				}
				//delete [] tmpBuf;
				m_bmp->UnlockBits(&bmpData);
			}
		}
		break;
	}
	

	CLSID pngClsid;
	//GetEncoderClsid(L"image/png", &pngClsid);
	//m_bmp->Save(L"c:\\1.png", &pngClsid, NULL);

	return TRUE;
}

Gdiplus::Bitmap* CGrpBlock::GetBitmap()
{
	return m_bmp;
}

/*
HBITMAP CGrpBlock::GetBitmap(COLORREF bkColor)
{
	HBITMAP ret=NULL;
	if(m_pData==NULL)
		return ret;

	HDC hdc=GetDC(NULL);

	int cx=GetWidth();
	int cy=GetHeight();

	int bitWidth=cx * 3;

	if(bitWidth % 4)
	{
		bitWidth +=( 4 - (bitWidth % 4));
	}

	BYTE* pBmp=new BYTE[bitWidth * cy];

	int x,y;
	int index=0;
	int ix=0;
	for(y=0;y<cy;y++)
	{
		int ix1=ix;
		for(x=0;x<cx;x++)
		{
			UINT f1,f2;
			f1= ((m_pData[index] & 0xff000000) >> 24);
			f2=255-f1;
			pBmp[ ix1 + 0] = ((m_pData[index] & 0xff) >> 0) * f1/255 + GetBValue(bkColor) *f2/255;
			pBmp[ ix1 + 1] = ((m_pData[index] & 0xff00) >> 8) * f1/255 + GetGValue(bkColor) *f2/255;
			pBmp[ ix1 + 2] = ((m_pData[index] & 0xff0000) >> 16) * f1/255 + GetRValue(bkColor) *f2/255;
			index++;
			ix1+=3;
		}
		ix+=bitWidth;
	}

	BITMAPINFO bmi;
	memset(&bmi,0,sizeof(bmi));
	bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       = cx;
	bmi.bmiHeader.biHeight      = -cy; // top-down image
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biBitCount    = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage   = 0;

	ret=CreateDIBitmap(hdc,&bmi.bmiHeader,CBM_INIT,(void*)pBmp,&bmi,DIB_RGB_COLORS);

	delete [] pBmp;

	::ReleaseDC(NULL,hdc);

	return ret;
}
//*/

void CGrpLib::SetCurBlock(size_t groupID,size_t frame,size_t frame0)
{
	if(m_pCurBlock)
	{
		m_pCurBlock->Release();
		m_pCurBlock = NULL;
	}

	long index = GetGroupIndex(groupID);

	if(index == -1) return;

	stGrpGroup & aBlock = m_aGroup[index];
	if(frame >= aBlock.Anis.size()) return;
	
	stAnimation& st = aBlock.Anis[frame];

	if(frame0 >= st.bmps.size()) return;
	m_pCurBlock = &st.bmps[frame0];

	if(!m_pCurBlock->LoadFromFile(m_strFileName.c_str()))
	{
		m_pCurBlock->Release();
		m_pCurBlock = NULL;
		//ATLASSERT(0);
	}
}

// RLE 压缩一行像素
void RLE_ProcessALine(DWORD* pData,size_t width, BYTE* pRetData, size_t& nRetLen )
{
	size_t flag;
	int i1,i2;

	const size_t max_line = (128);
	const size_t mask_rle = (128);

	i1=0;
	i2=-1;
	nRetLen=0;

	for(size_t i = 0 ; i < width ; i++)
	{
		int rl=1;
		while(i+1 < width && rl < max_line && pData[i] == pData[i+1] )
		{
			rl++;
			i++;
		}

		if(rl>1)
		{
			if(i1 <= i2)
			{
				flag=i2 - i1;

				pRetData[nRetLen++]=flag;
				memcpy(&pRetData[nRetLen],&pData[i1],(flag+1) * sizeof(DWORD));
				nRetLen+=(flag + 1) * sizeof(DWORD);
			}

			flag= mask_rle | (rl-1);

			pRetData[nRetLen++]=flag;
			*(DWORD*)(pRetData+nRetLen)=pData[i];
            nRetLen += 4;
			i1=i+1;
		}
		else
		{
			i2=i;
			if(i2-i1 + 1 == (int)max_line)
			{
				flag=i2-i1;
				pRetData[nRetLen++]=flag;
				memcpy( &pRetData[nRetLen],&pData[i1],(flag+1) * sizeof(DWORD) );
				nRetLen += (flag+1) * sizeof(DWORD);
				i1=i+1;
			}
		}
	}
	if(i2>=i1)
	{
		flag=i2-i1;
		pRetData[nRetLen++]=flag;
		memcpy(&pRetData[nRetLen],&pData[i1],(flag+1) * sizeof(DWORD) );
		nRetLen+=(flag+1)* sizeof(DWORD);
	}
}

bool CGrpBlock::SaveToTga(const char* szFileName , bool bRLE)
{
	if(m_bmp == NULL) return false;

	Gdiplus::BitmapData bmpData;
	Gdiplus::Status satus = m_bmp->LockBits(NULL,Gdiplus::ImageLockModeWrite,PixelFormat32bppARGB,&bmpData);
	assert(satus == Gdiplus::Ok);
	if(satus != Gdiplus::Ok) return FALSE;

	DWORD* pImgData = (DWORD*)bmpData.Scan0;

	TGAFILEHEADER fh;
    fh.Width	=	m_st.wdWidth;
    fh.Height	=	m_st.wdHeight;
    fh.PixelDepth= 32 ;
    if(bRLE)fh.ImageType = 10;
    FILE* fp = fopen(szFileName,"wb+");
    fwrite(&fh,sizeof(fh),1,fp);

    DWORD* pstData1=pImgData + (m_st.wdWidth*(m_st.wdHeight-1));
    if(bRLE)
    {
        BYTE *rleData = new BYTE[m_st.wdWidth * 4 * 5/4];
        for(int i=0;i<m_st.wdHeight;i++)
        {
            UINT rleLen;
            RLE_ProcessALine((DWORD*)pstData1,m_st.wdWidth,rleData, rleLen);
            fwrite(rleData,rleLen,1,fp);
            pstData1-=(m_st.wdWidth);
        }
        delete [] rleData;
    }
    else
    {
        for(int i=0;i<m_st.wdHeight;i++)
        {
            fwrite(pstData1,4 * m_st.wdWidth,1,fp);
            pstData1-=(m_st.wdWidth);
        }
    }
    fclose(fp);
	m_bmp->UnlockBits(&bmpData);
	return true;
}
inline std::string GetFileNameTitle(const char* pszFileName)
{
	const char* p2 = strrchr(pszFileName,'.');
	const char* p1 = strrchr(pszFileName,'\\');
	if(!p1) p1 = pszFileName;
	else p1++;
	if(!p2) p2 = pszFileName + strlen(pszFileName);
	int len = p2 - p1;
	std::string str;
	str.resize(len);
	memcpy(&str[0],p1,len);
	str[len] = 0;
	return str;
}

bool CGrpLib::SaveToTga(const char* pszPath,bool bObject)
{
	char szPath[MAX_PATH];
	sprintf(szPath,"%s\\%s",pszPath,GetFileNameTitle(m_strFileName.c_str()).c_str());
	CreateDirectory(szPath,NULL);
	for(size_t i= 0; i < GetGroupCount(); ++i)
	{
		char szGroupPath[MAX_PATH];
		sprintf(szGroupPath,"%s\\%03u",szPath,i);
		CreateDirectory(szGroupPath,NULL);
		for(size_t j=0; j < GetFrameCount(i);++j)
		{
			char szFramePath[MAX_PATH];
			
			if(bObject)
				sprintf(szFramePath,"%s\\%05u",szGroupPath,i * 100 + j);
			else
				sprintf(szFramePath,"%s\\%05u",szGroupPath,j);

			stAnimation& ani = GetAnimation(i,j);
			if(ani.bmps.size() == 1)
			{
				char szFileName[MAX_PATH];
				sprintf(szFileName,"%s.tga",szFramePath);
				const stGrpBlockEx* st = GetBlkStruct(i,j,0);
				if(st->wdWidth == 0 || st->wdHeight == 0)
				{
					TGAFILEHEADER hdr;
					FILE* fp = fopen(szFileName,"wb");
					fwrite(&hdr,sizeof(hdr),1,fp);
					fclose(fp);
				}
				else
				{
					SetCurBlock(i,j,0);
					if(!GetCurBlock()) return false;
					if(!GetCurBlock()->SaveToTga(szFileName,true))
						return false;
				}
			}
			else
			{
				CreateDirectory(szFramePath,NULL);
				for(size_t k=0;k < ani.bmps.size();++k)
				{
					char szFileName[MAX_PATH];
					sprintf(szFileName,"%s\\%05u.tga",szFramePath,k);

					const stGrpBlockEx* st = GetBlkStruct(i,j,k);
					if(st->wdWidth == 0 || st->wdHeight == 0)
					{
						TGAFILEHEADER hdr;
						FILE* fp = fopen(szFileName,"wb");
						fwrite(&hdr,sizeof(hdr),1,fp);
						fclose(fp);
					}
					else
					{
						SetCurBlock(i,j,k);
						if(!GetCurBlock()) return false;
						if(!GetCurBlock()->SaveToTga(szFileName,true))
							return false;
					}
				}
			}
		}
	}
	return true;
}

DWORD CGrpLib::TestTime(void)
{
	DWORD dw1 = timeGetTime();
	
	CGrpBlock* curBlk = GetCurBlock();
	for(size_t i=0;i < m_aGroup.size();++i)
	{
		stGrpGroup & group = m_aGroup[i];
		for(size_t j=0;j<group.Anis.size();++j)
		{
			stAnimation& ani = group.Anis[j];
			for(size_t k=0;k<ani.bmps.size();++k)
			{
				CGrpBlock & blk = ani.bmps[k];
				blk.Release();
				blk.LoadFromFile(m_strFileName.c_str(),true);
				if(curBlk != & blk)
					blk.Release();
			}
		}
	}

	return timeGetTime() - dw1;
}

static int __cdecl compareGrpGroupID ( const void * p1, const void *p2 ) 
{
	long groupID = *(long*)p1;
	stGrpGroup* grpGroup = (stGrpGroup*)p2;
	return groupID - (long)grpGroup->dwGroupID;
}

int CGrpLib::GetGroupIndex(DWORD nGroupID)
{
	stGrpGroup* p = (stGrpGroup*)bsearch(&nGroupID,&m_aGroup[0],m_aGroup.size(),sizeof(stGrpGroup),compareGrpGroupID);
	if(p) return p - &m_aGroup[0];
	return -1;
}