#include "../engine/include/engine.h"
#include ".\sceneobject.h"
#include ".\MapScene.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CSceneObject::CSceneObject(void)
: m_dwSceneObjectType(OBJECT_UNKNOW)
, m_ptGrid(-1,-1)
, m_ptPixelOffset(0,0)
, m_dwHP(0)
, m_dwMaxHP(1)
, m_dwMP(0)
, m_dwMaxMP(0)
{
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CSceneObject::~CSceneObject(void)
{
	if(GetMapScene()->m_pMouseOverObject == this)
		GetMapScene()->m_pMouseOverObject = NULL;
	if(GetMapScene()->m_pMouseOverTextObject == this)
	{
		GetMapScene()->m_pMouseOverTextObject = NULL;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptGrid : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CSceneObject::UpdateGridPosition(POINT ptGrid)
{
	if(ptGrid != m_ptGrid)
	{
		m_ptGrid = ptGrid;
		m_ptPixelOffset = GetMapScene()->GridCenterToPixelPos(ptGrid);
		return true;
	}
	return false;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dwScreenID : ÃèÊö
 * \param dwOffsetX : ÃèÊö
 * \param dwOffsetY : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CSceneObject::UpdatePosition(DWORD dwScreenID,DWORD dwOffsetX,DWORD dwOffsetY)
{
	return UpdateGridPosition(GetMapScene()->ScreenIDToGridPos(dwScreenID,dwOffsetX,dwOffsetY));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \param bJustInRect : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CSceneObject::IsAtScreenPixelPos(POINT pt, bool bJustInRect)
{
	return IsAtPixelPos(GetMapScene()->ScreenPixelPosToPixelPos(pt),bJustInRect);
}

//-------------------------------------------------------
//
//Add By David For 2006-3-9
bool CSceneObject::IsGridCenter( )
{
	//
	if( m_ptPixelOffset.x ==   m_ptGrid.x * GRID_WIDTH &&
		m_ptPixelOffset.x ==  m_ptGrid.y * GRID_HEIGHT )
	{
		return true;
	}
	
	//
	return false;
}

//-------------------------------------------------------
//
//Add By David For 2006-3-9
int	 CSceneObject::GetGridOffsetX( )
{
	return m_ptPixelOffset.x - m_ptGrid.x * GRID_WIDTH;
}

//-------------------------------------------------------
//
//Add By David For 2006-3-9
int	 CSceneObject::GetGridOffsetY( )
{
	return m_ptPixelOffset.x -  m_ptGrid.y * GRID_HEIGHT;
}

//-------------------------------------------------------
//
//Add By David For 2006-3-9
int	 CSceneObject::GetScreenGrid( )
{

	return ( m_ptGrid.x % 13 ) * ( m_ptGrid.y % 19 ) ;

}