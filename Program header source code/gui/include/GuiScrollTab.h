#pragma once

#include "guicontrol.h"
#include "GuiPanel.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiScrollTab :
	public CGuiControl
{

	struct stScrollTabItem{
		char	szText[64];
		DWORD	data;
		bool    bTop;
		stRectI rectTop;
		stRectI rectBottom;
		stRectI rcRender[9];
		std::vector<CGuiControl* > aControls;
	};


	enum enumHeaderAlign{
		HeaderAlign_Top,
		HeaderAlign_Bottom,
		HeaderAlign_Left,
		HeaderAlign_Right,
	};

	typedef std::vector<stScrollTabItem> t_Items;
	t_Items m_items;
	enumHeaderAlign m_eHeaderAlign;
	
	int m_nSelected;	// 选择的选项
	int m_nMouseOver;	// 鼠标移动到的选项
	//CGuiPanel m_Panel;
	bool m_bPressed;
	int  m_nPressItem;
	stRectI   m_rcRender[9];
	stRectI   m_rcCenter;

	
	void UpdateRects();
	void UpdateScrollTabRect();

	
	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	

public:
	static stPointI s_borderTopLeft;
	static stPointI s_borderRightBottom;
	static int     s_nScrollItemHeight;

	
	CGuiScrollTab(CGuiDialog* pDialog);

	void AddItem(const char* pszCaption,void* pData);
	void SetCurItem(int index);


	
	void RenderItem(int index,float fElapsedTime);
	void Render( float fElapsedTime );
	int GetItemAtPos(POINT ptOffset);
	int	 GetItemCount(){ return m_items.size(); }
	int  GetCurItem(){ return m_nSelected;}
	RECT GetItemRect(int index);
	bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );

	void AddControl(int index,CGuiControl* pControl);
	void RemoveControl(int index,CGuiControl* pControl);

	const char* GetItemText(int index);

	
	/*
	
	CGuiElement* GetItemElement(int index);
	int GetItemElementCount(int index);
	DWORD GetItemData(int index){ if((size_t)index >= m_items.size()) return 0; return m_items[index].data; }
	
	
	void SetItemText(int index,const char* pszText);
	

	void RemoveItem(int index);

	stRectI GetPanelRect();

	*/

	//-------------------------------------------
	// 注册UI脚本函数
	//-------------------------------------------
	static void bind_lua(lua_State * L);
};
