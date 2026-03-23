/**
 * \file      IBitmap.cpp
 * \version   $Id$
 * \author    $$$03π´Àæ
 *            $$$03π´Àæ@###.com
 * \date      2006-7-19  21:11:00
 * \brief     Õº∆¨≤Ÿ◊˜Ω”ø⁄
 * 
 *	      Õº∆¨≤Ÿ◊˜Ω”ø⁄
 */

#include "../include/engine.h"
#include "../include/IDevice.h"
#include "../include/ibitmap.h"

#include "../include/iBitmapList.h"
#include "../include/TBitmapList.h"

const int c_GrpVersion = 12;
const int c_GrpVersionOld = 11;
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
IBitmap::IBitmap(void)
: m_pDataBase(NULL)
//, m_pal(NULL)
, m_pData(NULL)
, m_dwLastRenderTime(0)
, eLoad(Load_NotLoad)
, m_pBitmaps(NULL)
, m_pMapFileData(NULL)
{
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
IBitmap::~IBitmap(void)
{
	ReleaseMemory();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
size_t IBitmap::ReleaseMemory()
{
	//m_pal = NULL;
	m_pData = NULL;
	delete [] m_pDataBase;
	m_pDataBase = NULL;
	m_pMapFileData = NULL;
	eLoad = Load_NotLoad;
	return 0;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool IBitmap::_MapData()
{
	if(m_block.byGrpType == eGrpTypeRle)
	{
		//m_pal = (stRgb*)m_pDataBase;
		//m_pData = m_pDataBase + sizeof(stRgb)*m_block.wdPalCount;
		AssertFatal(0,"∑œ∆˙µƒÕº∆¨∏Ò Ω!");
		return false;
	}
	else
	{
		m_pData = m_pDataBase;
	}
	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param blockX : √Ë ˆ
 * \param blockY : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
BYTE* IBitmap::GetRleBlock(int blockX,int blockY)
{
	Assert(m_block.byGrpType == eGrpTypeRle || m_block.byGrpType == eGrpTypeTexRle || m_block.byGrpType == eGrpTypeTexColorRle || m_block.byGrpType == eGrpTypeTexA4Rle);
	DWORD* blkOffset = (DWORD*)m_pData;
	
	{
		/*BYTE* pData = m_pData + sizeof(stGrpImageBlock) * (DWORD)GetWidthBlockNum() * (DWORD)GetHeightBlockNum();

		stGrpImageBlock* p = (stGrpImageBlock*)m_pData;
		return pData + p[blockY * (DWORD)GetWidthBlockNum() + blockX].offset;*/
		DWORD colNum = (DWORD)GetWidthBlockNum();
		DWORD rowNum = (DWORD)GetHeightBlockNum();
		BYTE* pData = m_pData + sizeof(stGrpImageBlock) * colNum * rowNum;

		stGrpImageBlock* p = (stGrpImageBlock*)m_pData;
		stGrpImageBlock & st = p[blockY * colNum + blockX];

		bool bError = false;

		if(st.offset >= GetDataSize()){
			bError = true;
		}
		if(st.width > 256 || st.height > 256 || st.left > 256 || st.top > 256){
			bError = true;
		}

		if(colNum == 1 && rowNum == 1 && (st.width != m_block.wdClipWidth || st.height != m_block.wdClipHeight)){
			bError = true;
		}

		if(bError){
#if defined _DEBUG || defined RELEASE_TEST
			Engine_WriteLogF("===> ¥ÌŒÛµƒÕº∆¨ ˝æ› %s (%d:%d:%d)\n",GetBitmapList()->GetFileName(),GetGroup(),GetFrame(),GetFrameFrame());
#endif 
			return NULL;
		}

		return pData + p[blockY * colNum + blockX].offset;
	}
}

//SIZE IBitmap::GetBlockSize(int blockX,int blockY)
//{
//	SIZE size;
//	
//	if(blockX == GetWidthBlockNum() - 1)
//	{
//		size.cx = m_iTailWidth;
//	}
//	else
//	{
//		size.cx = MAX_TEXTUREWIDTH;
//	}
//
//	if(blockY == GetHeightBlockNum() - 1)
//	{
//		size.cy = m_iTailHeight;
//	}
//	else
//	{
//		size.cy = MAX_TEXTUREWIDTH;
//	}
//
//	return size;
//}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool IBitmap::LoadBitmap()
{
	IBitmapList* pBmpList = GetBitmapList();
	if(!pBmpList) return false;
	IBitmap* p = pBmpList->LoadFrame(GetGroup(),GetFrame(),m_dwFrameFrame);
	//Assert(p == this);
	//TRACE("IBitmap::LoadBitmap :%p\n",p);
	return p == this && m_pData != NULL;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
const char* IBitmap::GetPackName()
{
	return GetBitmapList()->GetFileName();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
IBitmapList* IBitmap::GetBitmapList()
{ 
	if(!m_pBitmaps)
	{
		Engine_WriteLogF("%s bitmaps = NULL\n",__FUNCTION__);
		return NULL;
	}
	return m_pBitmaps->GetBitmapList();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int IBitmap::GetGroup()
{ 
	return m_pBitmaps->GetGroup();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int IBitmap::GetFrame()
{ 
	return m_pBitmaps->GetFrame();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
DWORD IBitmap::GetDataOffset()
{
	return m_block.dwDataOffset;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
DWORD IBitmap::GetDataSize()
{
	return m_pBitmaps->GetFrameDataSize(m_dwFrameFrame);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param blockX : √Ë ˆ
 * \param blockY : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
SIZE IBitmap::CalculateBlockClipSize(int blockX,int blockY)
{
	//Assert(m_block.byGrpType == eGrpTypeRle || m_block.byGrpType == eGrpTypeTexRle || m_block.byGrpType == eGrpTypeTexColorRle || m_block.byGrpType == eGrpTypeTexA4Rle);
	Assert(m_block.byGrpType != eGrpTypeRle && m_block.byGrpType != eGrpTypeTexColorRle);
	if(!IsLoadData()) 
	{
		bool b = LoadBitmap();
		Assert(b);
	}

	DWORD* blkOffset = (DWORD*)m_pData;

	SIZE size;
	if(m_block.byGrpType == eGrpTypeTexA4Rle)
	{
		if(m_block.byAlphaType == AlphaType_AllBlack )
		{
			size.cx = 0;
			size.cy = 0;
			return size;
		}
		if(m_block.byAlphaType == AlphaType_AllWhite)
		{
			size.cx = min(GetWidth(),MAX_TEXTUREWIDTH);
			size.cy = min(GetHeight(),MAX_TEXTUREWIDTH);
			return size;
		}
	}

	if(m_block.byGrpType == eGrpTypeTexRle || m_block.byGrpType == eGrpTypeTexA4Rle)
	{
		BYTE* pData = m_pData + sizeof(stGrpImageBlock) * (DWORD)GetWidthBlockNum() * (DWORD)GetHeightBlockNum();

		stGrpImageBlock* p = (stGrpImageBlock*)m_pData;
		stGrpImageBlock & st = p[blockY * (DWORD)GetWidthBlockNum()+ blockX];

		size.cx = st.width;
		size.cy = st.height;
	}
	else
	{
		if(blockX == GetWidthBlockNum() - 1)
		{
			size.cx = ::GetTailWidth(m_block.wdClipWidth);
		}
		else
		{
			size.cx = MAX_TEXTUREWIDTH;
		}

		if(blockY == GetHeightBlockNum() - 1)
		{
			size.cy = ::GetTailWidth(m_block.wdClipHeight);
		}
		else
		{
			size.cy = MAX_TEXTUREWIDTH;
		}
	}
	return size;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param blockX : √Ë ˆ
 * \param blockY : √Ë ˆ
 * \param pRect : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void IBitmap::CalculateBlockClipRect(int blockX,int blockY,RECT* pRect)
{
	//Assert(m_block.byGrpType == eGrpTypeRle || m_block.byGrpType == eGrpTypeTexRle || m_block.byGrpType == eGrpTypeTexColorRle || m_block.byGrpType == eGrpTypeTexA4Rle);
	Assert(m_block.byGrpType != eGrpTypeRle && m_block.byGrpType != eGrpTypeTexColorRle);
	
	int wBlockNum = GetWidthBlockNum();
	int hBlockNum = GetHeightBlockNum();
	
	if(wBlockNum <= 0 || hBlockNum <= 0)
	{
		Assert(0);
		pRect->left = 0;
		pRect->top = 0;
		pRect->right = 0;
		pRect->bottom = 0;
		return;
	}

	if((size_t)blockX >= (size_t)wBlockNum || (size_t)blockY >= (size_t)hBlockNum)
	{
		Assert(0);
		pRect->left = 0;
		pRect->top = 0;
		pRect->right = 0;
		pRect->bottom = 0;
		return;
	}

	if( !IsLoadData() )
	{
		bool bLoad = LoadBitmap();
		if(!bLoad)
		{
			Assert(bLoad);
			pRect->left = 0;
			pRect->top = 0;
			pRect->right = 0;
			pRect->bottom = 0;
			return;
		}
	}

	DWORD* blkOffset = (DWORD*)m_pData;
	
	if(m_block.byGrpType == eGrpTypeTexA4Rle)
	{
		if(m_block.byAlphaType == AlphaType_AllBlack )
		{
			pRect->left = 0;
			pRect->top = 0;
			pRect->right = 0;
			pRect->bottom = 0;
			return;
		}
		if(m_block.byAlphaType == AlphaType_AllWhite)
		{
			pRect->left = 0;
			pRect->top = 0;
			pRect->right = min(GetWidth(),MAX_TEXTUREWIDTH);
			pRect->bottom = min(GetHeight(),MAX_TEXTUREWIDTH);
			return;
		}
	}

	if(m_block.byGrpType == eGrpTypeTexRle || m_block.byGrpType == eGrpTypeTexA4Rle)
	{
		BYTE* pData = m_pData + sizeof(stGrpImageBlock) * (DWORD)wBlockNum * (DWORD)hBlockNum;

		stGrpImageBlock* p = (stGrpImageBlock*)m_pData;
		stGrpImageBlock & st = p[blockY * (DWORD)wBlockNum + blockX];

		pRect->left = st.left;
		pRect->top = st.top;
		pRect->right = (size_t)st.width + st.left;
		pRect->bottom = (size_t)st.height + st.top;
	}
	else
	{
		pRect->left = 0;
		pRect->top = 0;

		if(blockX == wBlockNum - 1)
		{
			pRect->right = ::GetTailWidth(m_block.wdClipWidth);
		}
		else
		{
			pRect->right = MAX_TEXTUREWIDTH;
		}

		if(blockY == hBlockNum - 1)
		{
			pRect->bottom = ::GetTailWidth(m_block.wdClipHeight);
		}
		else
		{
			pRect->bottom = MAX_TEXTUREWIDTH;
		}
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param dwTime : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void  IBitmap::SetLastRenderTime(DWORD dwTime)
{ 
	m_dwLastRenderTime = dwTime;
	if(m_pMapFileData) m_pMapFileData->dwMarkTime = dwTime;
	//GetBitmapList()->MarkGroupTime(GetGroup(),dwTime);
}

void IBitmap::SetMapFileData(stMapFileData* p)
{
	m_pMapFileData = p;
	if(m_pMapFileData)
	{
		m_pMapFileData->bitmapSet.insert(this);
	}
}