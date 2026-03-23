/**
 * \file      GuiTab.h
 * \version   $Id$
 * \author    $$$03公司@###.com
 *           $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#pragma once
#include "guicontrol.h"
#include "GuiPanel.h"
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiTab :
	public CGuiControl
{
public:
	enum enumHeaderAlign{
		HeaderAlign_Top,
		HeaderAlign_Bottom,
		HeaderAlign_Left,
		HeaderAlign_Right,
	};
private:
	struct stTabItem{
		char	szText[64];
		char	szShowText[64];
		DWORD	data;
		stRectI rect;
		stRectI rcRender[9];
		std::vector<CGuiControl* > aControls;
	};


	typedef std::vector<stTabItem> t_Items;
	t_Items m_items;
	enumHeaderAlign m_eHeaderAlign;
	stPointI m_ptItemSpacing;
	int m_nSelected;	// 选择的选项
	int m_nMouseOver;	// 鼠标移动到的选项
	void UpdateRects();
	void UpdateItemShowText();

	CGuiElement* GetAElement(int part,int index){
		int i = index * 9 + part;
		if(m_Elements.size() > (size_t)i) return m_Elements[i];
		return NULL;
	}
	CGuiPanel m_Panel;
	virtual HRESULT OnInit();
	bool m_bPressed;
	int  m_nPressItem;
	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
protected:
	int	m_nSoundIndex;
	
public:
	void    SetSoundIndex(int soundindex)
	{
		m_nSoundIndex = soundindex;
	}

	static stPointI s_borderTopLeft;
	static stPointI s_borderRightBottom;


	CGuiTab(CGuiDialog* pDialog);
	~CGuiTab(void);
	void SetHeaderAlign(enumHeaderAlign a) { m_eHeaderAlign = a;}
	void RenderItem(int index,float fElapsedTime);
	void Render( float fElapsedTime );
	CGuiElement* GetItemElement(int index);
	int GetItemElementCount(int index);
	DWORD GetItemData(int index){ if((size_t)index >= m_items.size()) return 0; return m_items[index].data; }
	RECT GetItemRect(int index);
	void AddItem(const char* pszCaption,void* pData);
	const char* GetItemText(int index);
	void SetItemText(int index,const char* pszText);
	void AddControl(int index,CGuiControl* pControl);
	void RemoveControl(int index,CGuiControl* pControl);

	void RemoveItem(int index);
	void SetCurItem(int index);
	int  GetItemCount(){ return m_items.size(); }
	int  GetCurItem(){ return m_nSelected;}


	bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	int GetItemAtPos(POINT ptOffset);

	stRectI GetPanelRect();


	//-------------------------------------------
	// 注册UI脚本函数
	//-------------------------------------------
	static void bind_lua(lua_State* L);

};

