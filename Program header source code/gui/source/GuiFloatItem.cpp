#include "../../engine/include/engine.h"
#include "../include/guitypes.h"
#include "../include/guifloatitem.h"

CGuiFloatItem* g_pCurrentFloatItem = NULL;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiFloatItem::CGuiFloatItem(void)
: CGuiDialog(0)
, m_pTable(NULL)
{
	g_pCurrentFloatItem = this;
	m_clientOffsetTopLeft = stPointI(0,0);
	m_clientOffsetRightBottom = stPointI(0,0);
	m_x = 0;
	m_y = 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiFloatItem::~CGuiFloatItem(void)
{
	g_pCurrentFloatItem = NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiItemCell* CGuiFloatItem::GetItem()
{
	return m_pTable->GetItemCell(0,0);
}

CGuiControl* CGuiFloatItem::GetControlAtPoint( POINT pt )
{
	return m_pTable;
}
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiFloatItem::OnRender( float fElapsedTime )
{
	Engine_GetCursor()->UpdateCursorPosition();
	CGuiItemCell* pItemCell = GetItem();
	if(!pItemCell) return S_OK;
	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	pItemCell->SetLocation(ptCursor.x - pItemCell->GetWidth()/2,ptCursor.y - pItemCell->GetHeight()/2);
	pItemCell->Render(fElapsedTime);
	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiFloatItem::OnCreate()
{
	CGuiDialog::OnCreate();
	AddTable(1,1,1,32,32,8,8,8,false,&m_pTable);
}

void CGuiFloatItem::DestroyCell()
{
	m_pTable->RemoveItem(0,0,true);
}