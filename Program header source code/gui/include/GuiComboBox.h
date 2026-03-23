/**
* \file      GuiComboBox.h
* \version   $Id$
* \author    $$$03公司@###.com
*           $$$03公司@###.com
* \date      2006-7-19  21:11:00
* \brief     GUI组合框
* 
*	      GUI组合框
*/

#pragma once
#include "guibutton.h"
#include "guiScrollBar.h"
//-----------------------------------------------------------------------------
// ComboBox control
//-----------------------------------------------------------------------------
struct GuiComboBoxItem
{
	char strText[256];
	void*  pData;

	stRectI  rcActive;
	bool  bVisible;
};

class CGuiEditBox;
/**
* \brief 简短描述
* 
* 详细描述
* 
*/
class CGuiComboBox :
	public CGuiButton
{
public:

	enum STYLE{
		GUI_CS_NOSKIN	= 0x8,
		GUI_CS_SCROLLBAR_NOSKIN = 0x100,
		GUI_CS_EDITTEXT = 0x200,
		GUI_CS_DROPUP = 0x400,
	};

	CGuiComboBox( CGuiDialog *pDialog = NULL );
	virtual ~CGuiComboBox();

	virtual void SetTextColor( DWORD Color );
	virtual HRESULT OnInit();

	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual void OnHotkey();

	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual void OnFocusOut();
	virtual void Render(float fElapsedTime );

	virtual void UpdateRects(); 

	void AddItem( const char* strText, void* pData );
	void    RemoveAllItems();
	void    RemoveItem( UINT index );
	bool    ContainsItem( const char* strText, UINT iStart=0 );
	int     FindItem( const char* strText, UINT iStart=0 );
	void*   GetItemData( const char* strText );
	void    SetDropHeight( UINT nHeight ) { m_nDropHeight = nHeight; UpdateRects(); }
	int		GetDropHeight() { return m_nDropHeight; }
	void    SetScrollBarWidth( int nWidth ) { m_nSBWidth = nWidth; UpdateRects(); }

	const char* GetItemText(int nIndex)
	{
		GuiComboBoxItem* pItem = GetItem(nIndex);
		if(pItem) return pItem->strText;
		return NULL;
	}

	void* GetItemData(int nIndex)
	{
		GuiComboBoxItem* pItem = GetItem(nIndex);
		if(pItem) return pItem->pData;
		return NULL;
	}

	void*   GetSelectedData();
	GuiComboBoxItem* GetSelectedItem();

	void SetCurItem(int index){ SetSelectedByIndex(index);}
	int  GetCurItem(){ return m_iSelected;}

	int    GetItemCount() { return m_Items.size(); }
	GuiComboBoxItem* GetItem( UINT index )
	{
		if ( index < m_Items.size() )
		{
			return m_Items[index]; 
		}
		return NULL;
	}

	HRESULT SetSelectedByIndex( UINT index );
	HRESULT SetSelectedByText( const char* strText );
	HRESULT SetSelectedByData( void* pData );  

	const char* GetText(){ return m_strText.c_str();}
	void SetText(const char* pszText);
	void SetWindowText(const char* pszText);

	bool PtInCombox(POINT ptScreen); 
	void FinishEdit();

protected:
	int     m_iSelected;
	int     m_iFocused;
	int     m_nDropHeight;
	CGuiScrollBar m_ScrollBar;
	int     m_nSBWidth;

	bool    m_bOpened;
	bool	m_bSBInit;
	stRectI m_rcText;
	stRectI m_rcButton;
	stRectI m_rcDropdown;
	stRectI m_rcDropdownText;
	std::string m_strText;


	CGuiEditBox * m_pEditBox;
	bool          m_bInEditing;

	std::vector< GuiComboBoxItem* > m_Items;

	void OnSelectChange(bool bSendMsg = true);
};
