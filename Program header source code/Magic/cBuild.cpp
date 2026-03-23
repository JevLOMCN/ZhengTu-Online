#include "../engine/engine.h"
#include ".\cbuild.h"
#include ".\cground.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
cBuild::cBuild(void)
{
	m_ulCount = 1;
	m_pTexture = NULL;
	m_nPicLib = 0;
	m_nPic = 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
cBuild::~cBuild(void)
{
	FinalRelease();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cBuild::FinalRelease()
{
	m_pTexture = NULL;
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptMapOffset : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//ÔÚµØÍ¼Æ«ÒÆÎªptMapOffsetµÄµØ·½»æÖÆ
bool cBuild::Draw(POINT ptMapOffset)
{
	if(!m_pTexture)
		RestoreDeviceObjects();
	if (!m_pTexture)
		return false;
	
	int w,h;
	w = m_pTexture->GetWidth();
	h = m_pTexture->GetHeight();

	int m_iScreenOffsetX = m_dwX * GRID_WIDTH;
	int m_iScreenOffsetY = m_dwY * GRID_HEIGHT - h + GRID_HEIGHT;

	POINT pt;
	pt.x	=	m_iScreenOffsetX - ptMapOffset.x;
	pt.y	=	m_iScreenOffsetY - ptMapOffset.y;
	m_pTexture->Render(pt.x,pt.y);
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptScreen : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//ÔÚÆÁÄ»Ä³´¦Ö±½Ó»æÖÆ
bool cBuild::DrawAtScreen(POINT ptScreen)
{
	return false;
}

#define OBJ_GROUP 128
#define GetTileGroupFrame(tile,group,frame) group = (tile)/OBJ_GROUP;frame = (tile)%OBJ_GROUP;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param rcBuild : ÃèÊö
 * \param pRes : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cBuild::GetBuildRect(stRectI& rcBuild,IBitmapList* pRes)
{
	ATLASSERT(pRes);
	if (pRes == NULL)
		return false;
	if (m_nPicLib==0 || m_nPic==0)
		return false;
	int w,h;
	int nGroup,nFrame;

	GetTileGroupFrame(m_nPic,nGroup,nFrame);

	IBitmap* p = pRes->GetBitmap(nGroup,nFrame,0);
	if(!p) return false;

	w = p->GetWidth();
	h = p->GetHeight();

	rcBuild.left = m_dwX * GRID_WIDTH;
	rcBuild.right = rcBuild.left + w;
	rcBuild.top = m_dwY * GRID_HEIGHT - h + GRID_HEIGHT;
	rcBuild.bottom = rcBuild.top + h;
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT cBuild::RestoreDeviceObjects() 
{
	if (m_nPicLib && m_nPic)
	{
		
		IBitmapList* pRes = GetDevice()->FindResource(cGround::GetObjName(m_nPicLib),c_nMagicResourceFlags,Usage_Building);
		if(pRes)
		{
			int nGroup,nFrame;
			GetTileGroupFrame(m_nPic,nGroup,nFrame);
			m_pTexture = pRes->GetBitmap(nGroup,nFrame);
		}
	}
	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dwObject : ÃèÊö
 * \param xGrid : ÃèÊö
 * \param yGrid : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cBuild::SetPic(DWORD dwObject,int xGrid,int yGrid)
{
	//DWORD dwObject = pTile->uObject;
	m_nPicLib = (dwObject & 0xffff0000)>>16;
	m_nPic = (dwObject & 0xffff);
	m_dwX = xGrid;
	m_dwY = yGrid;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param p : ÃèÊö
 * \param nPic : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cBuild::PreLoad(IBitmapList* p, int nPic)
{
}
