#include "StdAfx.h"
#include "ViewGL.h"
#include ".\savehelper.h"
#include ".\LoadThread.h"

//////////////////////////////////////////////////////////////////////////////////////////
// UI Thread 
CLoadThread		*g_LoadThread = NULL;
//////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////
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

#pragma pack(1)
typedef struct _TAGFILEHEADER{
	BYTE IDLength;
	BYTE ColormapType;
	BYTE ImageType;
	WORD ColormapOffset;		//颜色表首址 
	WORD ColormapLength;		//颜色表的长度
	BYTE ColormapBitDepth;		//颜色表表项的位（bit）数
	WORD XOrigin;
	WORD YOrigin;
	WORD Width;
	WORD Height;
	BYTE PixelDepth;
	BYTE ImageDescriptor;
	_TAGFILEHEADER()
	{
		memset(this,0,sizeof(*this));
		ImageType=2;
		PixelDepth=32;
	}
}TGAFILEHEADER, FAR * LPTGAFILEHEADER;
#pragma pack()

void SaveRGBAToTGA(const char* pszFName,DWORD* pData,int width,int height,bool bRLE = true)
{
	TGAFILEHEADER fh;
	fh.Width=width;
	fh.Height=height;
	fh.PixelDepth= 32 ;
	if(bRLE)fh.ImageType = 10;
	FILE* fp = fopen(pszFName,"wb+");
	fwrite(&fh,sizeof(fh),1,fp);

	DWORD* pstData1=pData + (height - 1) * width;
	DWORD* pstData;

	if(bRLE)
	{
		BYTE *rleData = new BYTE[width * 4 * 2];
		for(int i=height-1;i>=0;i--)
		{
			UINT rleLen;
			RLE_ProcessALine((DWORD*)pstData1,width,rleData, rleLen);
			fwrite(rleData,rleLen,1,fp);
			pstData1-=(width);
		}
		delete [] rleData;
	}
	else
	{
		for(int i=height-1;i>=0;i--)
		{
			fwrite(pstData1,4 * width,1,fp);
			pstData1-=(width);
		}
	}
	fclose(fp);

}

DWORD RGBAToARGB(DWORD data)
{
	int a,r,g,b;
	a = data & 0xff;
	return (data >> 8) | (a << 24);
}

DWORD ARGBToRGBA(DWORD data)
{
	int a,r,g,b;
	a = (data & 0xff000000) >> 24;
	return (data << 8) | a ;
}

void RGBAToARGB(void* pData,int width,int height,int pitch)
{
	BYTE* p1 = (BYTE*)pData;
	for(long i=0;i<height;++i)
	{
		DWORD* p2 = (DWORD*)p1;
		for(long j=0;j<width;++j)
			p2[j] = RGBAToARGB(p2[j]);
		p1 += pitch;
	}
}

void ARGBToRGBA(void* pData,int width,int height,int pitch)
{
	BYTE* p1 = (BYTE*)pData;
	for(long i=0;i<height;++i)
	{
		DWORD* p2 = (DWORD*)p1;
		for(long j=0;j<width;++j)
			p2[j] = ARGBToRGBA(p2[j]);
		p1 += pitch;
	}
}



//////////////////////////////////////////////////////////////////////////////////////////
//      Constructor / Destructor
//////////////////////////////////////////////////////////////////////////////////////////
CSaveHelper::CSaveHelper(void)
{
	m_strFileName[0] = 0;
	m_szSavePath[0] = 0;
}

CSaveHelper::~CSaveHelper(void)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
#define  TRACEOUT()\
	CString strCaption;\
	strCaption.Format("导出%s",szFileName);\
	strCaption += _T("\n");\
	TRACE(strCaption);\
	g_LoadThread->SetCaption(strCaption);
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
//   功能:
//        保存.gl图包文件中指定块成tga
//   参数:
//        pszPath: 保存路径
//        saveInfo: gl图包文件中指定块信息
//        bObject: 是否地图图包(地图图包编号不同)
//////////////////////////////////////////////////////////////////////////////////////////////
BOOL CSaveHelper::SaveToTga(const char* pszPath,const SAVE_INFO & saveInfo,bool bObject)
{
	
	//########################################################################################
	// UI Thread 
	g_LoadThread = (CLoadThread*)::AfxBeginThread(RUNTIME_CLASS(CLoadThread));
	while(!g_LoadThread->m_bInit)
		Sleep(1);

	g_LoadThread->HideProcessWnd();

	g_LoadThread->SetCaption(_T("保存物件中...."));
	//########################################################################################


        BOOL bSaveOK = FALSE;
	sprintf(m_szSavePath,"%s\\%s",pszPath,GetFileNameTitle(m_strFileName).c_str());
	CreateDirectory(m_szSavePath,NULL);

	IBitmapList * pBmpList = saveInfo.pBitmapList;
	if (!pBmpList) 
	{
		bSaveOK = FALSE;
		goto _exit;
	}
        /////////////////////////////////////////////////////////////////////////////////////////////////////
	// Save All Bimtap in .GL
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	if(saveInfo.itemdata == ITEM_ROOT)
	{
		//for(size_t iGroup= 0; iGroup < pBmpList->GetGroupCount(); ++iGroup)
		//{
		//	char szGroupPath[MAX_PATH];
		//	sprintf(szGroupPath,"%s\\%03u",m_szSavePath,iGroup);
		//	CreateDirectory(szGroupPath,NULL);
		//	for(size_t iFrame =0; iFrame < pBmpList->GetFrameCount(iGroup); ++iFrame)
		//	{
		//		char szFramePath[MAX_PATH];

		//		if(bObject)
		//			sprintf(szFramePath,"%s\\%05u",szGroupPath,iGroup * 100 + iFrame);
		//		else
		//			sprintf(szFramePath,"%s\\%05u",szGroupPath, iFrame );

		//		size_t frameframe = pBmpList->GetFrameFrameCount(iGroup,iFrame);
		//		if( frameframe == 1 )
		//		{
		//			char szFileName[MAX_PATH];
		//			sprintf(szFileName,"%s.tga",szFramePath);
		//			IBitmap * pBitmap = pBmpList->GetBitmap(iGroup,iFrame);
		//			if( !pBitmap || !SaveIBitmap( pBitmap,szFileName) ) 
		//			{
		//				bSaveOK = FALSE;
		//				goto _exit;
		//			}
  //                                      TRACEOUT();
		//		}
		//		else
		//		{
		//			CreateDirectory(szFramePath,NULL);
		//			for(size_t iFrameFrame = 0; iFrameFrame < frameframe; ++iFrameFrame )
		//			{
		//				char szFileName[MAX_PATH];
		//				sprintf(szFileName,"%s\\%05u.tga",szFramePath,iFrameFrame);
  //                                              IBitmap * pBitmap = pBmpList->GetBitmap(iGroup,iFrame,iFrameFrame);
		//				if( !pBitmap || !SaveIBitmap(pBitmap,szFileName))
		//				{
		//					bSaveOK = FALSE;
		//					goto _exit;
		//				}
		//				TRACEOUT();
  //            				}
		//		}
		//	}
		//}
		for(size_t i= 0; i < pBmpList->GetGroupCount(); ++i)
		{
			size_t iGroup = pBmpList->GetGroupID( i );

			char szGroupPath[MAX_PATH];
			sprintf(szGroupPath,"%s\\%03u",m_szSavePath,iGroup);
			CreateDirectory(szGroupPath,NULL);
			for(size_t iFrame =0; iFrame < pBmpList->GetFrameCount(iGroup); ++iFrame)
			{
				char szFramePath[MAX_PATH];

				if(bObject)
					sprintf(szFramePath,"%s\\%05u",szGroupPath,iGroup * 100 + iFrame);
				else
					sprintf(szFramePath,"%s\\%05u",szGroupPath, iFrame );

				size_t frameframe = pBmpList->GetFrameFrameCount(iGroup,iFrame);
				if( frameframe == 1 )
				{
					char szFileName[MAX_PATH];
					sprintf(szFileName,"%s.tga",szFramePath);
					IBitmap * pBitmap = pBmpList->GetBitmap(iGroup,iFrame);
					if( !pBitmap || !SaveIBitmap( pBitmap,szFileName) ) 
					{
						bSaveOK = FALSE;
						goto _exit;
					}
                                        TRACEOUT();
				}
				else
				{
					CreateDirectory(szFramePath,NULL);
					for(size_t iFrameFrame = 0; iFrameFrame < frameframe; ++iFrameFrame )
					{
						char szFileName[MAX_PATH];
						sprintf(szFileName,"%s\\%05u.tga",szFramePath,iFrameFrame);
                                                IBitmap * pBitmap = pBmpList->GetBitmap(iGroup,iFrame,iFrameFrame);
						if( !pBitmap || !SaveIBitmap(pBitmap,szFileName))
						{
							bSaveOK = FALSE;
							goto _exit;
						}
						TRACEOUT();
              				}
				}
			}
		}
		bSaveOK = TRUE;
		goto _exit;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Save A group in .GL
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	else if (saveInfo.itemdata == ITEM_GROUP)
	{
		size_t iGroup = saveInfo.group;

		char szGroupPath[MAX_PATH];
		sprintf(szGroupPath,"%s\\%03u",m_szSavePath,iGroup);
		CreateDirectory(szGroupPath,NULL);

		size_t  nGroups = pBmpList->GetFrameCount(iGroup);
		for( int iFrame = 0  ; iFrame< nGroups ; iFrame++)
		{
			char szFramePath[MAX_PATH];

			if(bObject)
				sprintf(szFramePath,"%s\\%05u",szGroupPath,iGroup * 100 + iFrame);
			else
				sprintf(szFramePath,"%s\\%05u",szGroupPath, iFrame );

			char szFileName[MAX_PATH];
			sprintf(szFileName,"%s.tga",szFramePath);

			size_t frameframe = pBmpList->GetFrameFrameCount(iGroup,iFrame);
			if( frameframe == 1 )
			{
				char szFileName[MAX_PATH];
				sprintf(szFileName,"%s.tga",szFramePath);
				IBitmap * pBitmap = pBmpList->GetBitmap(iGroup,iFrame);	
				if( !pBitmap ||  !SaveIBitmap(pBitmap,szFileName))
				{
					bSaveOK = FALSE;
					goto _exit;
				}
				TRACEOUT();
			}
			else
			{
				CreateDirectory(szFramePath,NULL);
				for(size_t iFrameFrame = 0; iFrameFrame < frameframe; ++iFrameFrame )
				{
					char szFileName[MAX_PATH];
					sprintf(szFileName,"%s\\%05u.tga",szFramePath,iFrameFrame);
					IBitmap * pBitmap = pBmpList->GetBitmap(iGroup,iFrame,iFrameFrame);	
					if( !pBitmap ||  !SaveIBitmap(pBitmap,szFileName))
					{
						bSaveOK = FALSE;
						goto _exit;
					}
					TRACEOUT();
				}
			}
		}
		bSaveOK = TRUE;
		goto _exit;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Save A Frame(if there are many subframes,it is a animation) in A Group in .GL
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	else if( saveInfo.itemdata == ITEM_FRAME)
	{
		size_t iGroup = saveInfo.group;
		size_t iFrame = saveInfo.frame;

		char szGroupPath[MAX_PATH];
		sprintf(szGroupPath,"%s\\%03u",m_szSavePath,iGroup);
		CreateDirectory(szGroupPath,NULL);

		char szFramePath[MAX_PATH];

		if(bObject)
			sprintf(szFramePath,"%s\\%05u",szGroupPath,iGroup * 100 + iFrame);
		else
			sprintf(szFramePath,"%s\\%05u",szGroupPath, iFrame );

		size_t frameframe = pBmpList->GetFrameFrameCount(iGroup,iFrame);
		if( frameframe == 1 )
		{
			char szFileName[MAX_PATH];
			sprintf(szFileName,"%s.tga",szFramePath);
			IBitmap * pBitmap = pBmpList->GetBitmap(iGroup,iFrame);	
			if( !pBitmap ||  !SaveIBitmap(pBitmap,szFileName))
			{
				bSaveOK = FALSE;
				goto _exit;
			}
			TRACEOUT();
		}
		else
		{
			CreateDirectory(szFramePath,NULL);
			for(size_t iFrameFrame = 0; iFrameFrame < frameframe; ++iFrameFrame )
			{
				char szFileName[MAX_PATH];
				sprintf(szFileName,"%s\\%05u.tga",szFramePath,iFrameFrame);
				IBitmap * pBitmap = pBmpList->GetBitmap(iGroup,iFrame,iFrameFrame);	
				if( !pBitmap ||  !SaveIBitmap(pBitmap,szFileName))
				{
					bSaveOK = FALSE;
					goto _exit;
				}
				TRACEOUT();
			}
		}
                
		bSaveOK = TRUE;
		goto _exit;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// SAVE A FRAME in a Ani (Frame in Frame )in A Group in .GL
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	else if( saveInfo.itemdata == ITEM_FRAME_FRAME)
	{

		size_t iGroup = saveInfo.group;
		size_t iFrame = saveInfo.frame;
		size_t iFrameFrame = saveInfo.frameframe;

		char szGroupPath[MAX_PATH];
		sprintf(szGroupPath,"%s\\%03u",m_szSavePath,iGroup);
		CreateDirectory(szGroupPath,NULL);

		char szFramePath[MAX_PATH];

		if(bObject)
			sprintf(szFramePath,"%s\\%05u",szGroupPath,iGroup * 100 + iFrame);
		else
			sprintf(szFramePath,"%s\\%05u",szGroupPath, iFrame );

		CreateDirectory(szFramePath,NULL);

		char szFileName[MAX_PATH];
		sprintf(szFileName,"%s\\%05u.tga",szFramePath,iFrameFrame);
		IBitmap * pBitmap = pBmpList->GetBitmap(iGroup,iFrame);		
		if( !pBitmap ||  !SaveIBitmap(pBitmap,szFileName))
		{
			bSaveOK = FALSE;
			goto _exit;
		}
		TRACEOUT();

		bSaveOK = TRUE;
		goto _exit;
	}
_exit:
	g_LoadThread->m_wndProgress.SendMessage(WM_CLOSE,0,0);
	
	// 2010 6
	//g_LoadThread->PostThreadMessage(WM_QUIT,0,0);

	g_LoadThread = NULL;

	return bSaveOK;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// 功能:
//	保存一个IBitmap * 指针所指向的图片成tga,支持RLE可选
//////////////////////////////////////////////////////////////////////////////////////////////
BOOL  CSaveHelper::SaveIBitmap(IBitmap * pBmp,const char * pszName)
{
	CRect _rcClip = pBmp->GetClipRect();
	int _nClipWidth = _rcClip.Width();
	int _nClipHeight = _rcClip.Height();

	int _nBitmapWidth  = pBmp->GetWidth();
	int _nBitmapHeight = pBmp->GetHeight();
 
	bool bUseFrameAlloctor = false;

        unsigned long total_need = _nBitmapWidth * _nBitmapHeight * sizeof(DWORD) + _nClipWidth * _nClipHeight * sizeof(DWORD) ;
	total_need = ((total_need + 0xF) & ~0xF); //16 bit aligned 
 
	if(FrameAllocator::getWaterMark() + total_need <= FrameAllocator::getHighWaterMark())
	        bUseFrameAlloctor = true;

	if( bUseFrameAlloctor )
	{
		SetFrameAllocator fa;
		unsigned long nSize = _nBitmapWidth * _nBitmapHeight * sizeof(DWORD);
		DWORD* pBitmapData = (DWORD*)FrameAllocator::alloc(nSize); 
		if( !pBitmapData)  
			return FALSE;
		memset(pBitmapData,0,nSize);

		DWORD dwXDiff =  _rcClip.left;
		DWORD dwYDiff = _rcClip.top;

		int nClipPitch = _nClipWidth * 4;  // 4 byte per pixel  and nClipWidth pixels 
		nSize = _nClipWidth * _nClipHeight * sizeof(DWORD);
		DWORD* pClipData = (DWORD*)FrameAllocator::alloc(nSize); 
		if( !pClipData ) return FALSE;
		memset(pClipData,0, nSize);
		if(!pBmp->GetBitmapData(pClipData,nClipPitch)) // 裁剪区数据
		{
			return FALSE;
		}

		// 注意: DWORD 和  BYTE 的 + 不同
		BYTE * pDst1 = (BYTE *)pBitmapData;
		BYTE * pSrc = (BYTE *)pClipData;
		pDst1+= dwYDiff * _nBitmapWidth * 4;
		for(size_t nRow  = 0 ; nRow < _nClipHeight; nRow++ )
		{
			BYTE * pDst2 =  pDst1;
			pDst2 += dwXDiff * 4;
			memcpy(pDst2,pSrc,_nClipWidth * 4);
			pSrc += _nClipWidth * 4;
			pDst1 += _nBitmapWidth * 4;
		}
		// Save to File 
		SaveRGBAToTGA(pszName,pBitmapData,_nBitmapWidth,_nBitmapHeight,true);
	}
	else
	{
		DWORD * pBitmapData  = new DWORD[_nBitmapWidth*_nBitmapHeight]; //
		if(!pBitmapData)
		{
			return FALSE;
		}
		memset(pBitmapData,0,sizeof(DWORD) * _nBitmapWidth *_nBitmapHeight);

		DWORD dwXDiff =  _rcClip.left;
		DWORD dwYDiff = _rcClip.top;


		int nClipPitch = _nClipWidth * 4;  // 4 byte per pixel  and nClipWidth pixels 
		DWORD * pClipData = new DWORD[_nClipWidth * _nClipHeight]; // pixel * pixel 
		memset(pClipData,0,_nClipWidth * _nClipHeight * sizeof(DWORD));
		if( !pClipData ) 
		{
			delete [] pBitmapData;
			return FALSE;
		}

		if(!pBmp->GetBitmapData(pClipData,nClipPitch)) // 裁剪区数据
		{
			delete [] pBitmapData;
			delete [] pClipData;
			return FALSE;
		}

		// 注意: DWORD 和  BYTE 的 + 不同
		BYTE * pDst1 = (BYTE *)pBitmapData;
		BYTE * pSrc = (BYTE *)pClipData;
		pDst1+= dwYDiff * _nBitmapWidth * 4;
		for(size_t nRow  = 0 ; nRow < _nClipHeight; nRow++ )
		{
			BYTE * pDst2 =  pDst1;
			pDst2 += dwXDiff * 4;
			memcpy(pDst2,pSrc,_nClipWidth * 4);
			pSrc += _nClipWidth * 4;
			pDst1 += _nBitmapWidth * 4;
		}
		////ARGB---->RGBA
		//int nBitmapPitch = _nBitmapWidth * 4;
		//ARGBToRGBA(pBitmapData,_nBitmapWidth,_nBitmapHeight,nBitmapPitch);


		// Save to File 
		SaveRGBAToTGA(pszName,pBitmapData,_nBitmapWidth,_nBitmapHeight,true);

		//for(int wpixel = 0 ; wpixel<_nClipWidth; wpixel ++)
		//{
		//	for(int hpixel = 0; hpixel<_nClipHeight; hpixel++)
		//	{									
		//		DWORD a = pBitmapData[wpixel + hpixel * _nClipWidth];
		//		a = (a&0xff000000) >> 24;
		//	}
		//}

		delete [] pBitmapData;
		delete [] pClipData;
	}

	return TRUE;
}