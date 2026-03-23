#pragma once
#include "guidialog.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CGuiFloatItem :
	public CGuiDialog
{
protected:
public:
	CGuiTable*	  m_pTable;
	int m_iTableType;
	CGuiFloatItem(void);
	~CGuiFloatItem(void);
	HRESULT OnRender( float fElapsedTime );
	CGuiItemCell* GetItem();
	void OnCreate();
	void DestroyCell();
	CGuiControl* GetControlAtPoint( POINT pt );
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline CGuiFloatItem* GetGuiFloatItem()
{
	extern CGuiFloatItem* g_pCurrentFloatItem;
	return g_pCurrentFloatItem;
}