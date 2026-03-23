/**
 * \file      D3DBitmap.cpp
 * \version   $Id$
 * \author    $$$03π´Àæ
 *            $$$03π´Àæ@###.com
 * \date      2005-07-05 12:29:19
 * \brief     Õº∆¨¿‡£¨”√D3D…Ë±∏ªÊ÷∆
 * 
 *	      Õº∆¨¿‡£¨”√D3D…Ë±∏ªÊ÷∆
 */

#include "../../engine/include/engine.h"
#include "../../engine/include/IBitmap.h"
#include "../include/d3dbitmap.h"
#include "../include/D3DDevice.h"
#include "../include/TextureCell.h"
#include "../include/D3DBitmapList.h"
#include "../include/TextureGroup.h"
#include "../include/Texturemanager.h"
#include "../../zlib/zlib.h"
#include "../Include/RenderD3D.h"

#include "../../engine/include/ijl.h"

extern size_t g_nTotalTexSize;
extern size_t g_nUselessTexSize;
extern size_t g_nUselessTexSize2;

static ULONGLONG s_texAlphaMax = 0xffffffffffffffff;
static ULONGLONG s_texColorMax = 0x00000000ffffffff;

//static HRESULT uncompress_jpeg_file(CAtlFile& file,unsigned long nDataLen,bool bAlpha,unsigned char* pOut,unsigned long pitch)
//{
//	HRESULT hr = S_OK;
//	BYTE* imageData = NULL;
//	BYTE* buf = NULL;
//	bool  bResult = false;
//	JPEG_CORE_PROPERTIES image;
//	DWORD dwJpgLen;
//	BYTE* pJpgData;
//
//	static std::vector<BYTE> s_buf;
//	static std::vector<BYTE> s_imgData;
//
//	s_buf.reserve(nDataLen);
//	buf = &s_buf[0];
//
//	hr = file.Read(buf,nDataLen);
//	if(FAILED(hr))
//	{
//		return S_FALSE;
//	}
//
//	if(bAlpha)
//	{
//		dwJpgLen = *(DWORD*)buf;
//		pJpgData = buf + 4;
//	}
//	else
//	{
//		dwJpgLen = nDataLen;
//		pJpgData = buf;
//	}
//
//	_IJLERR jpgErr;
//	memset( &image,0, sizeof( JPEG_CORE_PROPERTIES ) );
//	if( (jpgErr = ijlInit( &image )) != IJL_OK )
//	{
//		Assert(0);
//		return S_FALSE;
//	}
//
//	image.JPGBytes = pJpgData;
//	image.JPGSizeBytes = dwJpgLen;
//	if( (jpgErr = ijlRead( &image, IJL_JBUFF_READPARAMS )) != IJL_OK )
//	{
//		Assert(0);
//		ijlFree(&image);
//		return S_FALSE;
//	}
//
//	switch(image.JPGChannels)
//	{
//	case 1:
//		image.JPGColor    = IJL_G;
//		image.DIBChannels = 4;
//		image.DIBColor    = IJL_BGR;
//		break;
//
//	case 3:
//		image.JPGColor    = IJL_YCBCR;
//		image.DIBChannels = 4;
//		image.DIBColor    = IJL_BGR;
//		break;
//
//	case 4:
//		image.JPGColor    = IJL_YCBCRA_FPX;
//		image.DIBChannels = 4;
//		image.DIBColor    = IJL_RGBA_FPX;
//		break;
//
//	default:
//		 This catches everything else, but no
//		 color twist will be performed by the IJL.
//		image.DIBColor = (IJL_COLOR)IJL_OTHER;
//		image.JPGColor = (IJL_COLOR)IJL_OTHER;
//		image.DIBChannels = image.JPGChannels;
//		break;
//	}
//
//	image.DIBWidth    = image.JPGWidth;
//	image.DIBHeight   = image.JPGHeight;
//	image.DIBPadBytes = 0;
//	int imageSize = (image.DIBWidth * image.DIBChannels ) * image.DIBHeight;
//
//	s_imgData.reserve(imageSize);
//	imageData = &s_imgData[0];
//
//	image.DIBBytes = imageData;
//
//	if( (jpgErr = ijlRead( &image, /*IJL_JFILE_READWHOLEIMAGE*/IJL_JBUFF_READWHOLEIMAGE )) != IJL_OK )
//	{
//		Assert(0);
//		ijlFree( &image );
//		return S_FALSE;
//	}
//
//	if( (jpgErr = ijlFree( &image )) != IJL_OK )
//	{
//		Assert(0);
//		return S_FALSE;
//	}
//
//	int nSrcPitch = image.DIBWidth*image.DIBChannels;
//	if(bAlpha)
//	{
//		WORD* p = (WORD*)pOut;
//		BYTE* p1 = imageData;
//		BYTE* rleAlpha = pJpgData + dwJpgLen;
//		for(size_t i=0;i<image.DIBHeight;i++)
//		{
//			for(size_t j=0;j<image.DIBWidth;j++)
//			{
//				p[j] = COLOR_A4R4G4B4(255,p1[j*image.DIBChannels+2],p1[j*image.DIBChannels+1],p1[j*image.DIBChannels]);
//			}
//			RLE_DecodeALine(rleAlpha,image.DIBWidth,p,NULL,stRleDecodeAlphaTo4444());
//			p = (WORD*)((BYTE*)p + pitch);
//			p1 += nSrcPitch;
//		}
//	}
//	else
//	{
//		WORD* p = (WORD*)pOut;
//		BYTE* p1 = imageData;
//		for(size_t i=0;i<image.DIBHeight;i++)
//		{
//			for(size_t j=0;j<image.DIBWidth;j++)
//			{
//				p[j] = COLOR_R5G6B5(p1[j*image.DIBChannels+2],p1[j*image.DIBChannels+1],p1[j*image.DIBChannels]);
//			}
//			p = (WORD*)((BYTE*)p + pitch);
//			p1 += nSrcPitch;
//		}
//	}
//	return S_OK;
//}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param file : √Ë ˆ
 * \param long : √Ë ˆ
 * \param bAlpha : √Ë ˆ
 * \param char* : √Ë ˆ
 * \param long : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
static bool uncompress_jpeg_file( File& file,unsigned long nDataLen,bool bAlpha,unsigned char* pOut,unsigned long pitch )
{
	//Assert(0);
	//return false;
#if 1
	BYTE* imageData = NULL;
	BYTE* buf = NULL;
	bool  bResult = false;
	JPEG_CORE_PROPERTIES image;
	DWORD dwJpgLen;
	BYTE* pJpgData;

	static std::vector<BYTE> s_buf;
	static std::vector<BYTE> s_imgData;

	s_buf.reserve(nDataLen);
	buf = &s_buf[0];

	if(File::Ok != file.read(buf,nDataLen))
	{
		return false;
	}

	if(bAlpha)
	{
		dwJpgLen = *(DWORD*)buf;
		pJpgData = buf + 4;
	}
	else
	{
		dwJpgLen = nDataLen;
		pJpgData = buf;
	}

	memset( &image,0, sizeof( JPEG_CORE_PROPERTIES ) );
	if( ijlInit( &image ) != IJL_OK )
	{
		Assert(0);
		return false;
	}

	image.JPGBytes = pJpgData;
	image.JPGSizeBytes = dwJpgLen;
	IJLERR jerr = ijlRead( &image, IJL_JBUFF_READPARAMS );
	if( jerr != IJL_OK )
	{
		TRACE("ijlRead(IJL_JBUFF_READPARAMS) failed -- %s\n", ijlErrorStr(jerr));
		Assert(0);
		ijlFree(&image);
		return false;
	}

	switch(image.JPGChannels)
	{
	case 1:
		image.JPGColor    = IJL_G;
		image.DIBChannels = 3;
		image.DIBColor    = IJL_BGR;
		break;

	case 3:
		image.JPGColor    = IJL_YCBCR;
		image.DIBChannels = 3;
		image.DIBColor    = IJL_BGR;
		break;

	case 4:
		image.JPGColor    = IJL_YCBCRA_FPX;
		image.DIBChannels = 4;
		image.DIBColor    = IJL_RGBA_FPX;
		break;

	default:
		// This catches everything else, but no
		// color twist will be performed by the IJL.
		image.DIBColor = (IJL_COLOR)IJL_OTHER;
		image.JPGColor = (IJL_COLOR)IJL_OTHER;
		image.DIBChannels = image.JPGChannels;
		break;
	}

	image.DIBWidth    = image.JPGWidth;
	image.DIBHeight   = image.JPGHeight;
	image.DIBPadBytes = 0;
	int imageSize = (image.DIBWidth * image.DIBChannels ) * image.DIBHeight;

	s_imgData.reserve(imageSize);
	imageData = &s_imgData[0];

	image.DIBBytes = imageData;

	if( ijlRead( &image, /*IJL_JFILE_READWHOLEIMAGE*/IJL_JBUFF_READWHOLEIMAGE ) != IJL_OK )
	{
		Assert(0);
		ijlFree( &image );
		return false;
	}

	if( ijlFree( &image ) != IJL_OK )
	{
		Assert(0);
		return false;
	}

	if( bAlpha )
	{
		WORD* p = (WORD*)pOut;
		BYTE* p1 = imageData;
		BYTE* rleAlpha = pJpgData + dwJpgLen;
		for(size_t i=0;i<image.DIBHeight;i++)
		{
			for(size_t j=0;j<image.DIBWidth;j++)
			{
				p[j] = COLOR_A4R4G4B4(255,p1[j*3+2],p1[j*3+1],p1[j*3]);
			}
			RLE_DecodeALine(rleAlpha,image.DIBWidth,p,NULL,stRleDecodeAlphaTo4444());
			p = (WORD*)((BYTE*)p + pitch);
			p1 += image.DIBWidth*3;
		}
	}
	else
	{
		WORD* p = (WORD*)pOut;
		BYTE* p1 = imageData;
		for(size_t i=0;i<image.DIBHeight;i++)
		{
			for(size_t j=0;j<image.DIBWidth;j++)
			{
				p[j] = COLOR_R5G6B5(p1[j*3+2],p1[j*3+1],p1[j*3]);
			}
			p = (WORD*)((BYTE*)p + pitch);
			p1 += image.DIBWidth*3;
		}
	}
	return true;
#endif
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param file : √Ë ˆ
 * \param long : √Ë ˆ
 * \param char* : √Ë ˆ
 * \param long : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
static bool uncompress_file(File& file,unsigned long nInLen,unsigned char* pOut,unsigned long nOutLen)
{
	const unsigned long c_bufLen = 4096;
	z_stream_s zipStream;
	static unsigned char dataZip [c_bufLen];

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
	if (err != Z_OK) return false;
	while(nInLen)
	{
		unsigned long readLen;
		if(nInLen > c_bufLen)   readLen = c_bufLen;
		else                    readLen = nInLen;
		nInLen -= readLen;

		if( File::Ok != file.read(dataZip,readLen))
		{
			inflateEnd(&zipStream);
			return false;
		}

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

	return (err == Z_OK);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
CD3DBitmap::CD3DBitmap()
: m_bMapFile(false)
, m_textures(NULL)
{
	//m_textures = (stBlockInfo*)pAddinData;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
CD3DBitmap::~CD3DBitmap(void)
{
	//ReleaseTexture();
	ReleaseMemory();
	delete [] m_textures;
	m_textures = NULL;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param format : √Ë ˆ
 * \param width : √Ë ˆ
 * \param height : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
static size_t GetTexSize(D3DFORMAT format,int width,int height)
{
	switch(format)
	{
	case D3DFMT_A4R4G4B4:
	case D3DFMT_R5G6B5:
	case D3DFMT_A1R5G5B5:
		return width * height * 2;
		break;
	case D3DFMT_DXT1:
		return (width/4) * (height / 4) * 8;
		break;
	case D3DFMT_DXT3:
		return (width/4) * (height / 4) * 16;
		break;
	}
	return 0;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pBitmaps : √Ë ˆ
 * \param dwFrameFrame : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DBitmap::Init(IBitmaps* pBitmaps,DWORD dwFrameFrame)
{
	//m_iTailWidth = GetTailWidth(m_block.wdClipWidth);
	//m_iTailHeight = GetTailWidth(m_block.wdClipHeight);
	m_pBitmaps = pBitmaps;
	m_dwFrameFrame = (WORD)dwFrameFrame;
	//m_textures = new stBlockInfo[ (size_t)GetWidthBlockNum() * (size_t)GetHeightBlockNum() ];
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DBitmap::InitTexture()
{
	if(m_textures == NULL)
	{
		size_t count = (size_t)GetWidthBlockNum() * (size_t)GetHeightBlockNum();
		m_textures = new stBlockInfo[count];
		stBlockInfo* p = &m_textures[0];
		for(size_t i=0;i< count;++i)
		{
			p[i].pTex = NULL;
		}
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
size_t CD3DBitmap::ReleaseMemory()
{
	size_t size = GetDataSize();
	if(m_bMapFile)
	{
		//m_pal = NULL;
		m_pData = NULL;
		m_pDataBase = NULL;
	}

	IBitmap::ReleaseMemory();
	return size;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pMapData : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
size_t CD3DBitmap::_MapData(BYTE* pMapData)
{
	Assert(m_block.byGrpType != eGrpTypeJpeg);

	if(m_block.byGrpType == eGrpTypeJpeg) return false;

	m_pDataBase = pMapData;// + m_block.dwFileOffset + sizeof(stGrpFileHeader);
	m_bMapFile = true;
	if(!IBitmap::_MapData()) return 0;
	return GetDataSize();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pMapData : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */

//#pragma optimize(  off )
size_t CD3DBitmap::_MapDataLoad(BYTE* pMapData)
{
	size_t result = _MapData(pMapData);
	if(!result) return errorSize;
	size_t size = GetDataSize();
	DWORD dwPageSize = CD3DBitmapList::g_dwMemoryPageSize;

	/*

	fnVirtualLock fnLock = CD3DBitmapList::g_fnVirtualLock;
	fnVirtualUnlock fnUnlock = CD3DBitmapList::g_fnVirtualUnlock;

	if(fnLock && fnUnlock)
	{
		if(!fnLock(m_pDataBase,size))
		{
			DWORD code = GetLastError();
			if(code != ERROR_WORKING_SET_QUOTA)
			{
				TRACE("VirtualLock failed! [0x%p:%u] ,error:%u\n",m_pDataBase,size,code);
				CD3DBitmapList::g_fnVirtualLock = NULL;
				CD3DBitmapList::g_fnVirtualUnlock = NULL;
			}
			return result;
		}

		if(!fnUnlock(m_pDataBase,size))
		{
			DWORD code = GetLastError();
			if(code != ERROR_WORKING_SET_QUOTA)
			{
				TRACE("VirtualUnlock failed! [0x%p:%u] , error:%u\n",m_pDataBase,size,code);
				CD3DBitmapList::g_fnVirtualLock = NULL;
				CD3DBitmapList::g_fnVirtualUnlock = NULL;
			}
		}
	}
	else//*/
	{
		BYTE* end = m_pDataBase + size;
		BYTE* p = m_pDataBase;
		for(;p<end;p+=dwPageSize)
		{
			_asm
			{
				mov esi,p
				mov eax,[esi]
			}
		}
	}

	return result;
}
//#pragma optimize(  on )

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param file : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
size_t CD3DBitmap::_LoadBitmap(File& file)
{
	File::Status status;
	status = file.setPosition(GetDataOffset());
	if(status != File::Ok) return errorSize;
	m_bMapFile = false;
	size_t loadSize = 0;

	if(m_block.byGrpType == eGrpTypeJpeg)
	{
		size_t pitch = ROUNDNUM2(m_block.wdClipWidth*2,4);
		m_pDataBase = new BYTE[m_block.wdClipHeight * pitch];
		if(!uncompress_jpeg_file(file,GetDataSize(),m_block.byPixelFormat == ePixJpegARGB,m_pDataBase,pitch))
		{
			SAFE_DELETE(m_pDataBase);
			return errorSize;
		}
	}
	else
	{
		m_pDataBase = new BYTE[GetDataSize()];
		status = file.read(m_pDataBase,GetDataSize());
		if(status != File::Ok)
		{
			SAFE_DELETE(m_pDataBase);
			return errorSize;
		}
	}
	if(!IBitmap::_MapData())
	{
		SAFE_DELETE(m_pDataBase);
		return errorSize;
	}
	return GetDataSize();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pt : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int CD3DBitmap::GetAlpha(POINT pt)
{
	if(IsNull()) return 0;
	if(!IsInClipRect(pt))return 0;
	if(!IsLoadData() && !LoadBitmap()) return 0;
	int x = pt.x;
	int y = pt.y;
	HRESULT hr;
	switch(m_block.byGrpType)
	{
	case eGrpTypeRle:
		{
			/*
			POINT ptBlock,ptBlockPos;
			if(! GetBlockPos(x,y,ptBlock,ptBlockPos) ) return 0;
			SIZE sizeBlock = GetBlockSize(ptBlock.x,ptBlock.y);
			BYTE* pData = GetRleBlock(ptBlock.x,ptBlock.y);
			switch(m_block.byPixelFormat)
			{
			case ePixPal8NotAlpha:
			case ePix565:
			return 255;
			case ePixPal8:
			{
			DWORD dwColorLen = *(DWORD*)pData;
			BYTE* rleAlpha = pData + dwColorLen + 4;
			return RLE_GetValue(rleAlpha,sizeBlock.cx,ptBlockPos.x,ptBlockPos.y);
			}
			case ePix4444:
			{
			WORD* rleData = (WORD*)pData;
			WORD a = (RLE_GetValue(rleData,sizeBlock.cx,ptBlockPos.x,ptBlockPos.y) & 0xf000) >> 8;
			if(a >= 0xf0) a = 255;
			return a;
			}
			}
			*/
			return 0;
		}
		break;
	case eGrpTypeTexRle:
		{
			if(m_block.byPixelFormat == ePixDXT1) 
				return 255;
			POINT ptBlock,ptBlockPos;
			if(! GetBlockPos(x,y,ptBlock,ptBlockPos) ) return 0;
			SIZE sizeBlock = GetBlockSize(ptBlock.x,ptBlock.y);
			SIZE sizeTexBlock = { GetWidth4(sizeBlock.cx),GetWidth4(sizeBlock.cy)};

			BYTE* pData = GetRleBlock(ptBlock.x,ptBlock.y);
			if(pData == NULL){
				return 0;
			}

			D3DFORMAT format = GrpGetD3DFormat((enumPixelFormat)m_block.byPixelFormat);
			DWORD dwTexBlkSize = GetTexBlkSize(format);
			DWORD dwTexRowCount = sizeTexBlock.cy/4;
			DWORD dwTexRowSize = sizeTexBlock.cx/4 * dwTexBlkSize;

			SetFrameAllocator fa;
			BYTE *tmpData = (BYTE*)FrameAllocator::alloc(dwTexRowSize);

			for(size_t i=0 ; i< ptBlockPos.y/4 ; ++i)
				RLE_ForwardALine(pData,dwTexRowSize);

			RLE_DecodeALine(pData,dwTexRowSize,tmpData,NULL,stRleDecodeByteToByte());

			/*IDirect3DTexture8* pTex = GetD3DDevice()->GetTmpTexture(D3DFMT_A4R4G4B4);
			if(!pTex) return 0;

			IDirect3DSurface8* sur;
			HRESULT hr = pTex->GetSurfaceLevel(0,&sur);
			if(FAILED(hr)) return 0;

			RECT rcSrc = { ptBlockPos.x & (~3),0,(ptBlockPos.x & (~3)) + 4,4};
			RECT rcDst = { 0,0,4,4};
			hr = D3DXLoadSurfaceFromMemory(sur,NULL,&rcDst,tmpData,format,dwTexRowSize,NULL,&rcSrc,D3DX_DEFAULT,0);
			sur->Release();

			if(FAILED(hr)) return 0;

			D3DLOCKED_RECT lockRect;
			hr = pTex->LockRect(0,&lockRect,&rcDst,0);
			if(FAILED(hr)) return 0;

			WORD a = ((WORD*)lockRect.pBits)[(ptBlockPos.x & 3) + (ptBlockPos.y & 3) * lockRect.Pitch/2];
			a = (a & 0xf000) >> 8;
			if(a >= 0xf0) a = 255;

			pTex->UnlockRect(0);*/
			
			IDirect3DSurface8* pSur = GetD3DDevice()->GetTmpSurface(D3DFMT_A8R8G8B8);
			Assert(pSur);
			if(!pSur) return 0;

			RECT rcSrc = { ptBlockPos.x & (~3),0,(ptBlockPos.x & (~3)) + 4,4};
			RECT rcDst = { 0,0,4,4};
			hr = D3DXLoadSurfaceFromMemory(pSur,NULL,&rcDst,tmpData,format,dwTexRowSize,NULL,&rcSrc,D3DX_DEFAULT,0);

			if(FAILED(hr)) return 0;

			D3DLOCKED_RECT lockRect;
			hr = pSur->LockRect(&lockRect,&rcDst,D3DLOCK_NO_DIRTY_UPDATE);
			if(FAILED(hr)) return 0;

			DWORD a = ((DWORD*)lockRect.pBits)[(ptBlockPos.x & 3) + (ptBlockPos.y & 3) * lockRect.Pitch/4];
			a = (a&0xff000000) >> 24;
			/*a = (a & 0xf000) >> 8;
			if(a >= 0xf0) a = 255;*/

			pSur->UnlockRect();


			return a;
		}
		break;
	case eGrpTypeJpeg:
		{
			POINT ptOffset = GetOffsetPos(x,y);
			if(ptOffset.x == -1 || ptOffset.y == -1) return 0;
			switch(m_block.byPixelFormat)
			{
			case ePixJpegRGB:
				return 255;
			case ePixJpegARGB:
				{
					int pitch = GetPitch();
					WORD a = *(WORD*)(m_pData + y * pitch + x * 2);
					a = (a & 0xf000) >> 8;
					if(a >= 0xf0) a = 255;
					return a;
				}
			}
		}
		break;
	case eGrpTypeTex:
		{
			POINT ptOffset = GetOffsetPos(x,y);
			if(ptOffset.x == -1 || ptOffset.y == -1) return 0;
			IDirect3DSurface8* pSur = GetD3DDevice()->GetTmpSurface(D3DFMT_A8R8G8B8);
			Assert(pSur);
			if(!pSur) return 0;
			RECT rcSrc = { ptOffset.x & (~3),ptOffset.y & (~3),(ptOffset.x & (~3)) +4,(ptOffset.y & (~3)) +4};
			RECT rcDst = { 0,0,4,4};
			D3DFORMAT format = GrpGetD3DFormat((enumPixelFormat)m_block.byPixelFormat);
			hr = D3DXLoadSurfaceFromMemory(pSur,NULL,&rcDst,m_pData,format,GetPitch(),NULL,&rcSrc,D3DX_FILTER_NONE,0);

			if(FAILED(hr)) return 0;
			D3DLOCKED_RECT lockRect;
			hr = pSur->LockRect(&lockRect,&rcDst,D3DLOCK_NO_DIRTY_UPDATE);
			if(FAILED(hr)) return 0;

			WORD a = ((WORD*)lockRect.pBits)[(ptOffset.x & 3) + (ptOffset.y & 3) * lockRect.Pitch/4];
			a = (a & 0xff000000) >> 24;

			pSur->UnlockRect();

			return a;
		}
		break;
	}
	return 0;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pTex : √Ë ˆ
 * \param dstFormat : √Ë ˆ
 * \param blockX : √Ë ˆ
 * \param blockY : √Ë ˆ
 * \param ptTex : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
inline bool CD3DBitmap::UpdateTexture(IDirect3DTexture8* pTex,D3DFORMAT dstFormat,int blockX,int blockY,POINT ptTex)
{
	//return true;
	HRESULT hr;

#ifdef _DEBUG
	::D3DSURFACE_DESC dest_desc;
	pTex->GetLevelDesc(0,&dest_desc);
	Assert(dest_desc.Format == dstFormat);
#endif
	//if(srcFormat == D3DFMT_DXT1 && dstFormat == D3DFMT_DXT3) return true;
	D3DFORMAT srcFormat = dstFormat;
	Assert(0 == (ptTex.x & 0x3));
	Assert(0 == (ptTex.y & 0x3));

	if(m_block.byGrpType == eGrpTypeRle)
	{
		switch(m_block.byPixelFormat)
		{
		case ePixPal8:
		case ePix4444:
			if(dstFormat != D3DFMT_A4R4G4B4)
				srcFormat = D3DFMT_A8R8G8B8;
			break;
		case ePixPal8NotAlpha:
		case ePix565:
			if(dstFormat != D3DFMT_R5G6B5)
				srcFormat = D3DFMT_X8R8G8B8;
			break;
		}
	}

	// ªÒµ√ƒ⁄¥Êµƒƒø±ÍŒ∆¿Ì
	IDirect3DTexture8* pUpdate = GetD3DDevice()->GetTmpTexture(srcFormat);// ƒ⁄¥Ê÷–µƒƒø±ÍŒ∆¿Ì
	Assert(pUpdate);
	if(!pUpdate) return false;

	IDirect3DSurface8* pUpdateSurface;

	stRectI rcDst;
	stRectI rcSrc;
	//stRectI rect;

	switch(m_block.byGrpType)
	{
	case eGrpTypeRle:
		{
			Assert(0);
			/*
			BYTE* pBlockData = GetRleBlock(blockX,blockY);
			SIZE sizeBlock = GetBlockSize(blockX,blockY);
			D3DLOCKED_RECT lockRect;

			rcDst.left = ptTex.x;
			rcDst.top = ptTex.y;
			rcDst.right = ptTex.x + sizeBlock.cx;
			rcDst.bottom = ptTex.y + sizeBlock.cy;

			pUpdate->LockRect(0,&lockRect,&rcDst,0);

			BYTE* rleColor,*rleAlpha;
			DWORD rleColorLen;
			if(m_block.byPixelFormat == ePixPal8)
			{
				rleColorLen = *(DWORD*)pBlockData;
				rleColor = pBlockData + 4;
				rleAlpha = rleColor + rleColorLen; 
			}
			else
			{
				rleColor = pBlockData;
			}

			switch(m_block.byPixelFormat)
			{
			case ePixPal8:
				if(srcFormat == D3DFMT_A4R4G4B4)
				{
					for(size_t i=0;i<sizeBlock.cy;++i)
					{
						::RLE_DecodeALine(rleColor,sizeBlock.cx,(WORD*)lockRect.pBits,m_pal,stRleDecodeIndexTo4444());
						::RLE_DecodeALine(rleAlpha,sizeBlock.cx,(WORD*)lockRect.pBits,m_pal,stRleDecodeAlphaTo4444());
						lockRect.pBits = (BYTE*)lockRect.pBits + lockRect.Pitch;
					}
				}
				else
				{
					for(size_t i=0;i<sizeBlock.cy;++i)
					{
						::RLE_DecodeALine(rleColor,sizeBlock.cx,(DWORD*)lockRect.pBits,m_pal,stRleDecodeIndexTo8888());
						::RLE_DecodeALine(rleAlpha,sizeBlock.cx,(DWORD*)lockRect.pBits,m_pal,stRleDecodeAlphaTo8888());
						lockRect.pBits = (BYTE*)lockRect.pBits + lockRect.Pitch;
					}
				}
				break;
			case ePixPal8NotAlpha:
				if(srcFormat == D3DFMT_R5G6B5)
				{
					for(size_t i=0;i<sizeBlock.cy;++i)
					{
						::RLE_DecodeALine(rleColor,sizeBlock.cx,(WORD*)lockRect.pBits,m_pal,stRleDecodeIndexTo565());
						lockRect.pBits = (BYTE*)lockRect.pBits + lockRect.Pitch;
					}
				}
				else
				{
					for(size_t i=0;i<sizeBlock.cy;++i)
					{
						::RLE_DecodeALine(rleColor,sizeBlock.cx,(DWORD*)lockRect.pBits,m_pal,stRleDecodeIndexTo8888());
						lockRect.pBits = (BYTE*)lockRect.pBits + lockRect.Pitch;
					}
				}
				break;
			case ePix4444:
				if(srcFormat == D3DFMT_A4R4G4B4)
				{
					for(size_t i=0;i<sizeBlock.cy;++i)
					{
						::RLE_DecodeALine((WORD*&)rleColor,sizeBlock.cx,(WORD*)lockRect.pBits,m_pal,stRleDecodeWordToWord());
						lockRect.pBits = (BYTE*)lockRect.pBits + lockRect.Pitch;
					}
				}
				else
				{
					for(size_t i=0;i<sizeBlock.cy;++i)
					{
						::RLE_DecodeALine((WORD*&)rleColor,sizeBlock.cx,(DWORD*)lockRect.pBits,m_pal,stRleDecode4444To8888());
						lockRect.pBits = (BYTE*)lockRect.pBits + lockRect.Pitch;
					}
				}
				break;
			case ePix565:
				if(srcFormat == D3DFMT_R5G6B5)
				{
					for(size_t i=0;i<sizeBlock.cy;++i)
					{
						::RLE_DecodeALine((WORD*&)rleColor,sizeBlock.cx,(WORD*)lockRect.pBits,m_pal,stRleDecodeWordToWord());
						lockRect.pBits = (BYTE*)lockRect.pBits + lockRect.Pitch;
					}
				}
				else
				{
					for(size_t i=0;i<sizeBlock.cy;++i)
					{
						::RLE_DecodeALine((WORD*&)rleColor,sizeBlock.cx,(DWORD*)lockRect.pBits,m_pal,stRleDecode565To888());
						lockRect.pBits = (BYTE*)lockRect.pBits + lockRect.Pitch;
					}
				}
				break;
			}

			pUpdate->UnlockRect(0);
			//*/
		}
		break;
	case eGrpTypeJpeg:
		{
			rcDst.left = ptTex.x;
			rcDst.top  = ptTex.y;
			if(blockX == GetWidthBlockNum() - 1)
				//rcDst.right = ptTex.x + ( (srcFormat == D3DFMT_DXT1 || srcFormat == D3DFMT_DXT3) ? GetWidth4(m_iTailWidth) : m_iTailWidth );
				rcDst.right = ptTex.x + GetTailWidth();
			else
				rcDst.right = ptTex.x + MAX_TEXTUREWIDTH;

			if(blockY == GetHeightBlockNum() - 1)
				//rcDst.bottom = ptTex.y + ( (srcFormat == D3DFMT_DXT1 || srcFormat == D3DFMT_DXT3) ? GetWidth4(m_iTailHeight) : m_iTailHeight ) ;
				rcDst.bottom = ptTex.y + GetTailHeight() ;
			else
				rcDst.bottom = ptTex.y + MAX_TEXTUREWIDTH;

			rcSrc.left = blockX * MAX_TEXTUREWIDTH;
			rcSrc.top = blockY * MAX_TEXTUREWIDTH;
			rcSrc.right = rcSrc.left + (rcDst.right - rcDst.left);
			rcSrc.bottom = rcSrc.top + (rcDst.bottom - rcDst.top);

			D3DFORMAT format = GrpGetD3DFormat( (enumPixelFormat)m_block.byPixelFormat );
			size_t pitch = ROUNDNUM2(m_block.wdClipWidth * 2,4);
			IDirect3DSurface8 * sur;
			hr = pUpdate->GetSurfaceLevel(0,&sur);
			if(FAILED(hr)) return false;
			hr = D3DXLoadSurfaceFromMemory(sur,NULL,&rcDst,m_pData,format,pitch,NULL,&rcSrc,D3DX_DEFAULT,0);

			sur->Release();
			if(FAILED(hr))
			{
				Assert(0);
				return false;
			}
		}
		break;
	case eGrpTypeTex:
		{
			rcDst.left = ptTex.x;
			rcDst.top  = ptTex.y;

			if(blockX == GetWidthBlockNum() - 1)
			{
				if(srcFormat == D3DFMT_DXT1 || srcFormat == D3DFMT_DXT3)
					rcDst.right = ptTex.x + GetWidth4(GetTailWidth());
				else
					rcDst.right = ptTex.x + GetTailWidth();
			}
			else
			{
				rcDst.right = ptTex.x + MAX_TEXTUREWIDTH;
			}

			if(blockY == GetHeightBlockNum() - 1)
			{
				if(srcFormat == D3DFMT_DXT1 || srcFormat == D3DFMT_DXT3)
					rcDst.bottom = ptTex.y + GetWidth4(GetTailHeight());
				else
					rcDst.bottom = ptTex.y + GetTailHeight();
			}
			else
			{
				rcDst.bottom = ptTex.y + MAX_TEXTUREWIDTH;
			}

			rcSrc.left = blockX * MAX_TEXTUREWIDTH;
			rcSrc.top = blockY * MAX_TEXTUREWIDTH;
			rcSrc.right = rcSrc.left + (rcDst.right - rcDst.left);
			rcSrc.bottom = rcSrc.top + (rcDst.bottom - rcDst.top);

			D3DFORMAT format = GrpGetD3DFormat((enumPixelFormat)m_block.byPixelFormat);
			size_t pitch = GetWidth4(m_block.wdClipWidth) * GetTexBlkSize(format) / 4;
			IDirect3DSurface8 * sur;
			hr = pUpdate->GetSurfaceLevel(0,&sur);
			if(FAILED(hr)) 
				return false;

			hr = D3DXLoadSurfaceFromMemory(sur,NULL,&rcDst,m_pData,format,pitch,NULL,&rcSrc,D3DX_DEFAULT,0);

			sur->Release();

			if(FAILED(hr))
			{
				::TRACE_DXERROR(hr);
				Assert(0);
				return false;
			}
			//return true;
		}
		break;
	case eGrpTypeTexRle:
		{
			BYTE* pBlockData = GetRleBlock(blockX,blockY);
			if(pBlockData == NULL)
				return false;

			SIZE sizeBlock = GetBlockSize(blockX,blockY);
			
			if(sizeBlock.cx == 0 || sizeBlock.cy == 0) 
			{
				Assert(0);
				return false;
			}

			SIZE sizeTexBlock = { GetWidth4(sizeBlock.cx),GetWidth4(sizeBlock.cy) };

			rcDst = stRectI(ptTex.x,ptTex.y,sizeTexBlock.cx + ptTex.x,sizeTexBlock.cy + ptTex.y);

			stRectI rcBlockClip;
			GetBlockClipRect(blockX,blockY,&rcBlockClip);

			D3DLOCKED_RECT lockRect;
			D3DFORMAT tmpFmt = GrpGetD3DFormat((enumPixelFormat)m_block.byPixelFormat); // Õº∆¨µƒœÒÀÿ∏Ò Ω
			DWORD dwTexBlkSize = GetTexBlkSize(tmpFmt);
			DWORD dwTexRowCount = sizeTexBlock.cy/4;
			DWORD dwTexRowSize = sizeTexBlock.cx/4 * dwTexBlkSize;
			DWORD dwDataPitch;
			BYTE* pDstData;

			SetFrameAllocator fa;
			// ªÒµ√Ω‚—πÀıµƒƒø±ÍŒ∆¿Ì
			if(dstFormat == tmpFmt)
			{
				hr = pUpdate->LockRect(0,&lockRect,&rcDst,0);
				Assert(SUCCEEDED(hr));
				pDstData = (BYTE*)lockRect.pBits;
				dwDataPitch = lockRect.Pitch;
			}
			else
			{
				pDstData = (BYTE*)FrameAllocator::alloc(dwTexRowCount * dwTexRowSize);
				dwDataPitch = dwTexRowSize;
			}

			BYTE* pDstData0 = pDstData;
			for(size_t i=0;i < dwTexRowCount;++i)
			{
				RLE_DecodeALine(pBlockData,dwTexRowSize,pDstData0,NULL,stRleDecodeByteToByte());
				pDstData0 += dwDataPitch;
			}

			if(dstFormat == tmpFmt)
			{
				pUpdate->UnlockRect(0);
			}
			else
			{
				rcSrc = stRectI(0,0,sizeTexBlock.cx,sizeTexBlock.cy);
				hr = pUpdate->GetSurfaceLevel(0,&pUpdateSurface);
				Assert(SUCCEEDED(hr));
				hr = D3DXLoadSurfaceFromMemory(pUpdateSurface,NULL,&rcDst,pDstData,tmpFmt,dwDataPitch,NULL,&rcSrc,D3DX_DEFAULT,0);
				pUpdateSurface->Release();
				Assert(SUCCEEDED(hr));
			}
		}
		break;
	case eGrpTypeTexColorRle:
		{
			Assert(0);
			BYTE* pBlockData = GetRleBlock(blockX,blockY);
			if(pBlockData == NULL)
				return false;

			SIZE sizeBlock = GetBlockSize(blockX,blockY);

			if(sizeBlock.cx == 0 || sizeBlock.cy == 0) 
			{
				Assert(0);
				return false;
			}

			SIZE sizeTexBlock = { GetWidth4(sizeBlock.cx),GetWidth4(sizeBlock.cy) };

			rcDst = stRectI(ptTex.x,ptTex.y,sizeTexBlock.cx + ptTex.x,sizeTexBlock.cy + ptTex.y);

			D3DLOCKED_RECT lockRect;
			D3DFORMAT tmpFmt = D3DFMT_DXT3;//GrpGetD3DFormat((enumPixelFormat)m_block.byPixelFormat);
			DWORD dwTexBlkSize = GetTexBlkSize(tmpFmt);
			DWORD dwTexRowCount = sizeTexBlock.cy/4;
			DWORD dwTexRowSize = sizeTexBlock.cx/4 * dwTexBlkSize;
			DWORD dwTexRleRowSize = sizeTexBlock.cx/4 * dwTexBlkSize/2;
			DWORD dwTexColCount = sizeTexBlock.cx/4;
			DWORD dwDataPitch;
			BYTE* pDstData;

			SetFrameAllocator fa;

			// ªÒµ√Ω‚—πÀıµƒƒø±ÍŒ∆¿Ì
			if(dstFormat == tmpFmt)
			{
				hr = pUpdate->LockRect(0,&lockRect,&rcDst,0);
				Assert(SUCCEEDED(hr));
				pDstData = (BYTE*)lockRect.pBits;
				dwDataPitch = lockRect.Pitch;
			}
			else
			{
				pDstData = (BYTE*)FrameAllocator::alloc(dwTexRowCount * dwTexRowSize);
				dwDataPitch = dwTexRowSize;
			}

			BYTE* pDstData1 = (BYTE*)FrameAllocator::alloc(dwTexRowSize);
			BYTE* pDstData0 = pDstData;
			for(size_t i=0;i<dwTexRowCount;++i)
			{
				RLE_DecodeALine( pBlockData,dwTexRleRowSize,pDstData1,NULL,stRleDecodeByteToByte() );
				for(size_t j = 0;j < dwTexColCount;++j)
				{
					*(ULONGLONG*)&pDstData0[j * 16] = s_texAlphaMax;
					*(ULONGLONG*)&pDstData0[j * 16 + 8] = *(ULONGLONG*)&pDstData1[j * 8];
				}
				pDstData0 += dwDataPitch;
			}

			if(dstFormat == tmpFmt)
			{
				pUpdate->UnlockRect(0);
			}
			else
			{
				rcSrc = stRectI(0,0,sizeTexBlock.cx,sizeTexBlock.cy);
				hr = pUpdate->GetSurfaceLevel(0,&pUpdateSurface);
				Assert(SUCCEEDED(hr));
				hr = D3DXLoadSurfaceFromMemory(pUpdateSurface,NULL,&rcDst,pDstData,tmpFmt,dwDataPitch,NULL,&rcSrc,D3DX_DEFAULT,0);
				pUpdateSurface->Release();
				Assert(SUCCEEDED(hr));
			}

		}
		break;
	case eGrpTypeTexA4Rle:
		{
			BYTE* pBlockData = GetRleBlock(blockX,blockY);
			if(pBlockData == NULL)
				return false;

			SIZE sizeBlock = GetBlockSize(blockX,blockY);

			if(sizeBlock.cx == 0 || sizeBlock.cy == 0) 
			{
				Assert(0);
				return false;
			}

			SIZE sizeTexBlock = { GetWidth4(sizeBlock.cx),GetWidth4(sizeBlock.cy) };

			rcDst = stRectI(ptTex.x,ptTex.y,sizeTexBlock.cx + ptTex.x,sizeTexBlock.cy + ptTex.y);

			D3DLOCKED_RECT lockRect;
			D3DFORMAT tmpFmt = D3DFMT_DXT3;//GrpGetD3DFormat((enumPixelFormat)m_block.byPixelFormat);
			DWORD dwTexBlkSize = GetTexBlkSize(tmpFmt);
			DWORD dwTexRowCount = sizeTexBlock.cy/4;
			DWORD dwTexRowSize = sizeTexBlock.cx/4 * dwTexBlkSize;
			DWORD dwTexRleRowSize = sizeTexBlock.cx/4 * dwTexBlkSize/2;
			DWORD dwTexColCount = sizeTexBlock.cx/4;
			DWORD dwDataPitch;
			BYTE* pDstData;

			SetFrameAllocator fa;

			// ªÒµ√Ω‚—πÀıµƒƒø±ÍŒ∆¿Ì
			if(dstFormat == tmpFmt)
			{
				hr = pUpdate->LockRect(0,&lockRect,&rcDst,0);
				Assert(SUCCEEDED(hr));
				pDstData = (BYTE*)lockRect.pBits;
				dwDataPitch = lockRect.Pitch;
			}
			else
			{
				pDstData = (BYTE*)FrameAllocator::alloc(dwTexRowCount * dwTexRowSize);
				dwDataPitch = dwTexRowSize;
			}

			BYTE* pDstData0 = pDstData;
			switch(m_block.byAlphaType)
			{
			case AlphaType_AllBlack:
				{
					for(size_t i=0;i<dwTexRowCount;++i)
					{
						memset(pDstData0,0,dwTexColCount * 16);
						pDstData0 += dwDataPitch;
					}
				}
				break;
			case AlphaType_AllWhite:
				{
					for(size_t i=0;i<dwTexRowCount;++i)
					{
						for(size_t j=0;j<dwTexColCount;++j)
						{
							*(ULONGLONG*)&pDstData0[j * 16] = s_texAlphaMax;
							*(ULONGLONG*)&pDstData0[j * 16 + 8] = s_texColorMax;
						}
						pDstData0 += dwDataPitch;
					}
				}
				break;
			default:
				{
					BYTE* pDstData1 = (BYTE*)FrameAllocator::alloc(dwTexRowSize);
					for(size_t i=0;i<dwTexRowCount;++i)
					{
						RLE_DecodeALine(pBlockData,dwTexRleRowSize,pDstData1,NULL,stRleDecodeByteToByte());
						for(size_t j=0;j<dwTexColCount;++j)
						{
							*(ULONGLONG*)&pDstData0[j * 16] = *(ULONGLONG*)&pDstData1[j * 8];
							*(ULONGLONG*)&pDstData0[j * 16 + 8] = s_texColorMax;
						}
						pDstData0 += dwDataPitch;
					}
				}
				break;
			}

			if(dstFormat == tmpFmt)
			{
				pUpdate->UnlockRect(0);
			}
			else
			{
				rcSrc = stRectI(0,0,sizeTexBlock.cx,sizeTexBlock.cy);
				hr = pUpdate->GetSurfaceLevel(0,&pUpdateSurface);
				Assert(SUCCEEDED(hr));
				hr = D3DXLoadSurfaceFromMemory(pUpdateSurface,NULL,&rcDst,pDstData,tmpFmt,dwDataPitch,NULL,&rcSrc,D3DX_DEFAULT,0);
				pUpdateSurface->Release();
				Assert(SUCCEEDED(hr));
			}
		}
		break;
	}

	/*if( m_block.byGrpType == eGrpTypeTexA4Rle)
	{
		char szFileName[MAX_PATH];
		static int i=0;
		sprintf(szFileName,"c:\\%05d.bmp",i++);

		D3DXSaveTextureToFile(szFileName,D3DXIFF_BMP,pUpdate,NULL);
	}*/

	rcDst.right = GetWidth4(rcDst.right);
	rcDst.bottom = GetWidth4(rcDst.bottom);
	pUpdate->AddDirtyRect(&rcDst);
	hr = GetD3DDevice()->__UpdateTexture(pUpdate,pTex);
	if(FAILED(hr)){
		TRACE_DXERROR(hr);
		return false;
	}
	return true;

}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param ix : √Ë ˆ
 * \param iy : √Ë ˆ
 * \param RECT* : √Ë ˆ
 * \param stPointF : √Ë ˆ
 * \param color : √Ë ˆ
 * \param blend : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DBitmap::Render(int ix,int iy,const RECT* rect,const stPointF * scale,DWORD color,BlendType blend)
{
	if(IsNull()) return true;
	if(m_block.byGrpType == eGrpTypeTexA4Rle || m_block.byGrpType == eGrpTypeTexA4){
		DWORD color;
		bool bFill = false;
		if(m_block.byAlphaType == AlphaType_AllBlack ){
			color = 0xff000000;
			bFill = true;
		}else if(m_block.byAlphaType == AlphaType_AllWhite){
			color = 0xffffffff;
			bFill = true;
		}else if(m_block.byAlphaType == AlphaType_OneOnly){
			color = COLOR_ARGB(255,m_block.byAlpha,m_block.byAlpha,m_block.byAlpha);
			bFill = true;
		}
		if(bFill){
			stRectI rcClip = GetClipRect();
			if(rect)
				rcClip &= (*rect);
			if(scale){
				rcClip.left *= (long)scale->x;
				rcClip.top *= (long)scale->y;
				rcClip.right *= (long)scale->x;
				rcClip.bottom *= (long)scale->y;
			}
			stRectI rc(ix + rcClip.left,iy + rcClip.top,ix + rcClip.right,iy + rcClip.bottom);
			return GetD3DDevice()->FillRect(&rc,color);
		}
	}

	float x,y;
	SetLastRenderTime(xtimeGetTime());
	
	float blockW ,blockH;

	if(scale)
	{
		blockW = MAX_TEXTUREWIDTH * scale->x;
		blockH = MAX_TEXTUREWIDTH * scale->y;

		if(rect)
		{
			x = ix + (-rect->left +  m_block.wdClipLeft) * scale->x;
			y = iy + (-rect->top + m_block.wdClipTop) * scale->y;
		}
		else
		{
			x = ix + m_block.wdClipLeft * scale->x;
			y = iy + m_block.wdClipTop * scale->y;
		}
	}
	else
	{
		blockW = MAX_TEXTUREWIDTH;
		blockH = MAX_TEXTUREWIDTH;

		if(rect)
		{
			x = (float)(ix + (-rect->left +  m_block.wdClipLeft));
			y = (float)(iy + (-rect->top + m_block.wdClipTop));
		}
		else
		{
			x = (float)(ix + m_block.wdClipLeft);
			y = (float)(iy + m_block.wdClipTop);
		}
	}

	stPointI block;
	stPointF pt;

	int iHeightBlockNum = GetHeightBlockNum();
	int iWidthBlockNum = GetWidthBlockNum();
	if(rect)
	{
		stRectI clip;
		stRectI rcBlock;

		pt.y = y;
		rcBlock.top = m_block.wdClipTop;
		for(block.y =0;block.y < iHeightBlockNum;++block.y)
		{
			pt.x = x;
			rcBlock.left = m_block.wdClipLeft;
			if(block.y == iHeightBlockNum-1)
				rcBlock.bottom = rcBlock.top + GetTailHeight();
			else
				rcBlock.bottom = rcBlock.top + MAX_TEXTUREWIDTH;

			for(block.x=0; block.x< iWidthBlockNum; ++block.x)
			{
				if(block.x == iWidthBlockNum-1)
					rcBlock.right = rcBlock.left + GetTailWidth();
				else
					rcBlock.right = rcBlock.left + MAX_TEXTUREWIDTH;

				clip = stRectI(*rect) & rcBlock;
				if(!clip.IsRectEmpty())
				{
					clip.left -= rcBlock.left;
					clip.right -= rcBlock.left;
					clip.top -= rcBlock.top;
					clip.bottom -= rcBlock.top;
					if(!RenderBlock(block, pt,&clip,scale,color,blend))
					{
						Assert(0);
						return false;
					}
				}
				pt.x += blockW;
				rcBlock.left += MAX_TEXTUREWIDTH;
			}
			pt.y += blockH;
			rcBlock.top += MAX_TEXTUREWIDTH;
		}
	}
	else
	{
		pt.y = y;
		for(block.y =0;block.y < iHeightBlockNum;++block.y)
		{
			pt.x = x;
			for(block.x=0; block.x< iWidthBlockNum; ++block.x)
			{
				if(!RenderBlock(block, pt,NULL,scale,color,blend))
				{
					Assert(0);
					return false;
				}
				pt.x += blockW;
			}
			pt.y += blockH;
		}
	}


	if(GetBitmapList()->GetTextureGroup())
		((CTextureGroup*)GetBitmapList()->GetTextureGroup())->OnRender();

	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param x : √Ë ˆ
 * \param y : √Ë ˆ
 * \param color : √Ë ˆ
 * \param blend : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DBitmap::Render(int x,int y,DWORD color,BlendType blend)
{
	if(IsNull()) return true;
	
	if(m_block.byGrpType == eGrpTypeTexA4Rle || m_block.byGrpType == eGrpTypeTexA4){
		DWORD color;
		bool bFill = false;
		if(m_block.byAlphaType == AlphaType_AllBlack ){
			color = 0xff000000;
			bFill = true;
		}else if(m_block.byAlphaType == AlphaType_AllWhite){
			color = 0xffffffff;
			bFill = true;
		}else if(m_block.byAlphaType == AlphaType_OneOnly){
			color = COLOR_ARGB(255,m_block.byAlpha,m_block.byAlpha,m_block.byAlpha);
			bFill = true;
		}
		if(bFill){
			stRectI rcClip = GetClipRect();
			stRectI rc(x + rcClip.left,y + rcClip.top,x + rcClip.right,y + rcClip.bottom);
			return GetD3DDevice()->FillRect(&rc,color);
		}
	}

	SetLastRenderTime(xtimeGetTime());

	x += m_block.wdClipLeft;
	y += m_block.wdClipTop;
	stPointI block;
	stPointF pt;
	pt.y = (float)y;

	for(block.y =0;block.y < GetHeightBlockNum();++block.y)
	{
		pt.x = (float)x;
		for(block.x=0; block.x< GetWidthBlockNum(); ++block.x)
		{
			if(!RenderBlock(block, pt,NULL,NULL,color,blend))
			{
				Assert(0);
				return false;
			}
			pt.x += MAX_TEXTUREWIDTH;
		}
		pt.y += MAX_TEXTUREWIDTH;
	}

	if(GetBitmapList()->GetTextureGroup())
		((CTextureGroup*)GetBitmapList()->GetTextureGroup())->OnRender();

	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param block : √Ë ˆ
 * \param pt : øÈ∆µ„£®≤ª≤√ºÙµƒ£©µƒªÊ÷∆Œª÷√
 * \param RECT : øÈµƒ≤√ºÙ«¯”Ú
 * \param stPointF : √Ë ˆ
 * \param color : √Ë ˆ
 * \param blend : √Ë ˆ
 * \param & : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
inline enumBlockRenderInfo CD3DBitmap::GetBlockRenderInfo(POINT  block,stPointF  pt,const RECT * rect,const stPointF * scale,DWORD color,BlendType blend,stRenderInfo & ri)
{
	int blockW,blockH;
	stRectI rcBlockClip;
	GetBlockClipRect(block.x,block.y,&rcBlockClip);

	if(rcBlockClip.Width() == 0 || rcBlockClip.Height() == 0) return BlockRenderInfo_Null;

	blockW = ( (block.x == GetWidthBlockNum() - 1) ? GetTailWidth() : MAX_TEXTUREWIDTH );
	blockH = ( (block.y == GetHeightBlockNum() - 1) ? GetTailHeight() : MAX_TEXTUREWIDTH );

	stRectI rcClip;
	if(rect) rcClip = *rect;
	else rcClip = stRectI(0,0,blockW,blockH);

	rcClip &= rcBlockClip;

	if(scale)
	{
		ri.rect.left = pt.x + rcClip.left * scale->x;
		ri.rect.top = pt.y + rcClip.top * scale->y;
		ri.rect.right = pt.x + rcClip.right * scale->x;
		ri.rect.bottom = pt.y + rcClip.bottom * scale->y;

		if( !GetD3DDevice()->IsHasTransform() && (ri.rect & (*GetD3DDevice()->GetRenderRect())).IsRectEmpty() )
			return BlockRenderInfo_Null;

		if(GetD3DDevice()->GetImageScaleMode() == ImageScaleMode_Half)
		{
			/*ri.uv.left = rcClip.left - rcBlockClip.left + (ri.rect.left - (int)ri.rect.left) / scale->x;
			ri.uv.top = rcClip.top - rcBlockClip.top + (ri.rect.top - (int)ri.rect.top) / scale->y;
			ri.uv.right = rcClip.right - rcBlockClip.left  - (ri.rect.right - (int)ri.rect.right) / scale->x;
			ri.uv.bottom = rcClip.bottom - rcBlockClip.top   - (ri.rect.bottom - (int)ri.rect.bottom) / scale->y;*/

			ri.uv.left = (float)(rcClip.left - rcBlockClip.left);
			ri.uv.top = (float)(rcClip.top - rcBlockClip.top);
			ri.uv.right = (float)(rcClip.right - rcBlockClip.left);
			ri.uv.bottom = (float)(rcClip.bottom - rcBlockClip.top);
			
			if(scale->x != 1.0f){
				ri.uv.left+= 1.0f;
				ri.uv.right-= 1.0f;
			}
			if(scale->y != 1.0f){
				ri.uv.top+= 1.0f;
				ri.uv.bottom-= 1.0f;
			}

			/*
			ri.rect.left  = (int)(ri.rect.left + 0.5f);
			ri.rect.top = (int)(ri.rect.top + 0.5f);
			ri.rect.right = (int)(ri.rect.right + 0.5f);
			ri.rect.bottom = (int)(ri.rect.bottom + 0.5f);*/


			/*ri.rect.left = (int)ri.rect.left;
			ri.rect.top = (int)ri.rect.top;

			ri.rect.right = (int)(ri.rect.right + 0.9f);
			ri.rect.bottom = (int)(ri.rect.bottom + 0.9f);*/
		}
		else
		{
			ri.uv.left = (float)(rcClip.left - rcBlockClip.left);
			ri.uv.top = (float)(rcClip.top - rcBlockClip.top);
			ri.uv.right = (float)(rcClip.right - rcBlockClip.left);
			ri.uv.bottom = (float)(rcClip.bottom - rcBlockClip.top);
		}
	}
	else
	{
		stRectI rcRender((long)pt.x + rcClip.left , (long)pt.y + rcClip.top , (long)pt.x + rcClip.right , (long)pt.y + rcClip.bottom);
		if(!GetD3DDevice()->IsHasTransform())
			rcRender &= *GetDevice()->GetRenderRect();
		if(rcRender.IsRectEmpty())
			return BlockRenderInfo_Null;

		ri.rect = rcRender;
		ri.uv.left = rcRender.left - pt.x - rcBlockClip.left;
		ri.uv.top = rcRender.top - pt.y - rcBlockClip.top;
		ri.uv.right = rcRender.right - pt.x - rcBlockClip.left;
		ri.uv.bottom = rcRender.bottom - pt.y - rcBlockClip.top;

	}

	InitTexture();
	CBmpTextureCell* pTex = m_textures[block.y * GetWidthBlockNum() + block.x].pTex;

	if(!pTex)
	{
		if(!LoadBlockTexture(block.x,block.y))
		{
			return BlockRenderInfo_Error;
		}
	}else
	{
		Assert(GetBitmapList()->GetTextureGroup());
	}

	pTex = m_textures[block.y * GetWidthBlockNum() + block.x].pTex;

	Assert(pTex);

	GetBitmapList()->GetTextureGroup()->OnRender(pTex);

	//GetBitmapList()->GetTextureGroup()->CheckUsage(GetBitmapList()->GetUsage());

	ri.color = color;
	ri.blend = blend;

	float fTexW,fTexH;

	long nTexL = pTex->GetLeft();
	long nTexT = pTex->GetTop();

	fTexW = (float)pTex->GetTexCache()->GetWidth();
	fTexH = (float)pTex->GetTexCache()->GetHeight();

	ri.uv.left = (float)(ri.uv.left + nTexL) / fTexW;
	ri.uv.top = (float)(ri.uv.top + nTexT) / fTexH;

	ri.uv.right = (float)(ri.uv.right + nTexL ) / fTexW ;
	ri.uv.bottom = (float)(ri.uv.bottom + nTexT ) / fTexH ;

	

	ri.tex = pTex->GetTexCache()->GetTex();

	return BlockRenderInfo_Render;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param block : √Ë ˆ
 * \param pt : √Ë ˆ
 * \param RECT : √Ë ˆ
 * \param stPointF : √Ë ˆ
 * \param color : √Ë ˆ
 * \param blend : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
inline bool CD3DBitmap::RenderBlock(POINT  block,stPointF  pt,const RECT * rect,const stPointF * scale,DWORD color,BlendType blend)
{
	stRenderInfo ri;
	enumBlockRenderInfo e = GetBlockRenderInfo(block,pt,rect,scale,color,blend,ri);
	if( e == BlockRenderInfo_Error ) return false;
	if( e == BlockRenderInfo_Render )
		return SUCCEEDED(GetD3DDevice()->Render(ri));
	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param ptBlock : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
CBmpTextureCell* CD3DBitmap::GetBlockTexture(POINT ptBlock)
{
	return m_textures[ptBlock.y * GetWidthBlockNum() + ptBlock.x].pTex;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param xBlock : √Ë ˆ
 * \param yBlock : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DBitmap::LoadBlockTexture(size_t xBlock, size_t yBlock)
{
	if(!IsLoadData()) 
	{
		if(!LoadBitmap())
		{
			Assert(0);
			return false;
		}
	}
	InitTexture();
	Assert(m_textures[yBlock * GetWidthBlockNum() + xBlock].pTex == NULL);

	SIZE size = GetBlockSize(xBlock,yBlock);
	if(size.cx == 0 || size.cy == 0) return false;
	
	CTextureGroup* pGroup = GetBitmapList()->GetTextureGroup();

	if(!pGroup)
	{
		GetBitmapList()->CreateTextureGroup();
		pGroup = GetBitmapList()->GetTextureGroup();
		Assert(pGroup);
	}

	CBmpTextureCell* pCell = (CBmpTextureCell*)pGroup->CreateTexture(size.cx,size.cy);
	if(pCell)
	{
		int index = yBlock * GetWidthBlockNum() + xBlock;
		m_textures[index].pTex = pCell;
		pCell->SetBitmap(this , index);
		POINT ptTex = {pCell->GetLeft(),pCell->GetTop()};
		bool b = UpdateTexture(pCell->GetTexCache()->GetTex(),pGroup->GetFormat(),xBlock,yBlock,ptTex);
		Assert(b);
		return true;
	}
	return false;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param xBlock : √Ë ˆ
* \param yBlock : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//bool CD3DBitmap::TryLoadBlockTexture(size_t xBlock, size_t yBlock)
//{
//	InitTexture();
//	if(m_textures[yBlock * GetWidthBlockNum() + xBlock].pTex) return true;
//
//	if(!IsLoadData()) 
//	{
//		if(!LoadBitmap())
//		{
//			Assert(0);
//			return false;
//		}
//	}
//
//	SIZE size = GetBlockSize(xBlock,yBlock);
//	if(size.cx == 0 || size.cy == 0) return false;
//
//	CTextureGroup* pGroup = GetBitmapList()->GetTextureGroup();
//
//	if(!pGroup)
//	{
//		GetBitmapList()->CreateTextureGroup();
//		pGroup = GetBitmapList()->GetTextureGroup();
//		Assert(pGroup);
//	}
//
//	CBmpTextureCell* pCell = (CBmpTextureCell*)pGroup->TryCreateTexture(size.cx,size.cy);
//	if(pCell)
//	{
//		m_textures[yBlock * GetWidthBlockNum() + xBlock].pTex = pCell;
//		pCell->SetBitmap(this , yBlock * GetWidthBlockNum() + xBlock);
//		POINT ptTex = {pCell->GetLeft(),pCell->GetTop()};
//
//		Assert(pCell->GetTexCache());
//		Assert(pCell->GetTexCache()->GetTex());
//		//Engine_WriteLogF("CD3DBitmap::TryLoadBlockTexture 1111111111111111111");
//		bool b = UpdateTexture(pCell->GetTexCache()->GetTex(),pGroup->GetFormat(),xBlock,yBlock,ptTex);
//		//Engine_WriteLog("CD3DBitmap::TryLoadBlockTexture 2222222222222222222");
//		Assert(b);
//		return true;
//	}
//	return false;
//}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DBitmap::ReleaseTexture()
{
	size_t count = (size_t)GetWidthBlockNum() * (size_t)GetHeightBlockNum();
	for(size_t i=0; i< count;++i)
	{
		if(m_textures[i].pTex)
		{
			m_textures[i].pTex->ReleaseTexture();
			m_textures[i].pTex = NULL;
		}
	}
}

bool CD3DBitmap::GetBlockBitmapData(int blockX,int blockY,void* pData,int dataPatch)
{
	if(IsNull()) return false;

	if(!IsLoadData() && !LoadBitmap()) return false;

	switch(m_block.byGrpType)
	{
	case eGrpTypeRle:
		{
			Assert(0);
			return false;
		}
		break;
	case eGrpTypeTexRle:
		{
//			HRESULT hr;

			BYTE* pBlockData = GetRleBlock(blockX,blockY);
			if(pBlockData == NULL)
				return false;

			SIZE sizeBlock = GetBlockSize(blockX,blockY);//Clip
			
			if(sizeBlock.cx == 0 || sizeBlock.cy == 0) 
			{
				return true;
			}

			SIZE sizeTexBlock = { GetWidth4(sizeBlock.cx),GetWidth4(sizeBlock.cy) };

			stRectI rect(0,0,sizeTexBlock.cx,sizeTexBlock.cy);


			D3DLOCKED_RECT lockRect;
			D3DFORMAT tmpFmt = GrpGetD3DFormat((enumPixelFormat)m_block.byPixelFormat);
			DWORD dwTexBlkSize = GetTexBlkSize(tmpFmt);//8/16
			DWORD dwTexRowCount = sizeTexBlock.cy/4;
			DWORD dwTexRowSize = sizeTexBlock.cx/4 * dwTexBlkSize;
			DWORD dwDataPitch;

			SetFrameAllocator fa;
			BYTE* pTmpData = (BYTE*)FrameAllocator::alloc(MAX_TEXTUREWIDTH * MAX_TEXTUREWIDTH * 4);
			BYTE* pDstData = pTmpData;

			dwDataPitch = MAX_TEXTUREWIDTH * 4;
			BYTE* pDstData0 = pDstData;

			for(size_t i=0;i < dwTexRowCount;++i)
			{
				RLE_DecodeALine(pBlockData,dwTexRowSize,pDstData0,NULL,stRleDecodeByteToByte());
				pDstData0 += dwDataPitch;
			}

			IDirect3DSurface8* pSur = GetD3DDevice()->GetTmpSurface(D3DFMT_A8R8G8B8);
//			IDirect3DSurface8* surDst;

			D3DXLoadSurfaceFromMemory(pSur,NULL,&rect,pTmpData,tmpFmt,dwDataPitch,NULL,&rect,D3DX_DEFAULT,0);
			

                        //////////////////////////////////////////////////////		
			stRectI rcBlockClip;
			GetBlockClipRect(blockX,blockY,&rcBlockClip);
			DWORD dwXDiff = rcBlockClip.left; //+ blockX * MAX_TEXTUREWIDTH;
			DWORD dwYDiff = rcBlockClip.top;// + blockY * MAX_TEXTUREWIDTH;
			//////////////////////////////////////////////////////

                        
			pSur->LockRect(&lockRect,&rect,0);
			
			BYTE* pSrc,*pDst;
			pSrc = (BYTE*)lockRect.pBits;
			pDst = (BYTE*)pData;
			pDst += dwYDiff * dataPatch;
			for(size_t i = 0; i< sizeBlock.cy;++i)
			{
				memcpy(pDst+dwXDiff * 4,pSrc,sizeBlock.cx * 4);
				pDst += dataPatch;
				pSrc += lockRect.Pitch;
			}
			pSur->UnlockRect();

			return true;
		}
		break;
	case eGrpTypeJpeg:
		{
			Assert(0);
			return false;
		}
		break;
	case eGrpTypeTex:
		{
			Assert(0);
			return false;
		}
		break;
	}
	return false;
}

bool CD3DBitmap::GetBitmapData(void* pData,int dataPatch)
{
	int w = GetWidthBlockNum();
	int h = GetHeightBlockNum();

	for( size_t i=0;i<h;++i )
	{
		for( size_t j=0;j<w;++j )
		{
			BYTE* p = &((BYTE*)pData)[i * MAX_TEXTUREWIDTH * dataPatch + j * MAX_TEXTUREWIDTH * 4];
			if(!GetBlockBitmapData(j,i,p,dataPatch)) return false;
		}
	}
	return true;
}

//bool CD3DBitmap::TryLoadTexture()
//{
//	int iHeightBlockNum = GetHeightBlockNum();
//	int iWidthBlockNum = GetWidthBlockNum();
//
//	for(int i=0;i<iHeightBlockNum;++i)
//	{
//		for(int j=0;j<iWidthBlockNum;++j)
//		{
//			int blockW,blockH;
//			stRectI rcBlockClip;
//			GetBlockClipRect(j,i,&rcBlockClip);
//
//			if(rcBlockClip.Width() == 0 || rcBlockClip.Height() == 0) 
//				continue;
//
//			//Engine_WriteLog("CD3DBitmap::TryLoadTexture 111111111111111111111\n");
//			if(!TryLoadBlockTexture(j,i)) 
//			{
//				//Engine_WriteLog("CD3DBitmap::TryLoadTexture 222222222222222222222222\n");
//				return false;
//			}
//			//Engine_WriteLog("CD3DBitmap::TryLoadTexture 222222222222222222222222\n");
//		}
//	}
//	return true;
//}