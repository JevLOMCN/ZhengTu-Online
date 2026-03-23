/**
 * \file      GuiControl.h
 * \version   $Id$
 * \author    $$$03公司@###.com
 *           $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI控件基类
 * 
 *	      GUI控件基类
 */

#pragma once

#include "AccelKey.h"
#include "ToolTips.h"
#include "guiTypes.h"

class CGuiDialog;
class CGuiElement;
struct lua_State;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiControl
{
	friend class CGuiDialog;
	bool m_bDeleted;
protected:
	CToolTips m_ToolTips;
	GUI_TOOLTIPS_TYPE m_TipsType;
	int m_iFont;
public:

	int GetFont(){ return m_iFont;}
	void SetFont(int iFont);

	POINT ScreenToClient(POINT pt);
	CToolTips* GetToolTips(){ return &m_ToolTips; }
	const char* GetToopTips();

	void RequestFocus();
	void SetToolTips(const char* pszText);
	void SetTipsShowType(GUI_TOOLTIPS_TYPE type){ m_TipsType = type; }
    CGuiControl( CGuiDialog *pDialog = NULL );
    virtual ~CGuiControl();

	/*DWORD GetTextFromat() { return m_dwTextFormat;}
	void SetTextFormat(DWORD dw) { m_dwTextFormat = dw;}*/
	virtual void AddItem(const char* pszCaption,void* pData = NULL){};
    virtual HRESULT OnInit() { return S_OK; }
    virtual void Refresh();
    virtual void Render( float fElapsedTime );
	virtual void RenderBoundingBox( float fElapsedTime );
    // Windows message handler
    virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );

    virtual bool CanHaveFocus() { return false; }
	virtual bool CanHaveInputFocus() { return false; }
    virtual void OnFocusIn() { m_bHasFocus = true; }
    virtual void OnFocusOut() { m_bHasFocus = false; }
    virtual void OnMouseEnter();
    virtual void OnMouseLeave();
    virtual void OnHotkey() {}

	virtual bool ContainsPoint( POINT pt );
	stRectI GetBoundingBox(){ return m_rcBoundingBox;}
	virtual void SetEnabled( bool bEnabled ) 
	{
		m_bEnabled = bEnabled; 
		if( !m_bEnabled )
		{
			m_dwBeginEnableTime = 0;
		}
	}

    virtual void SetEnableDelay( bool bEnabled,DWORD dwDelayTime )
	{
		if( bEnabled )
		{
			m_dwBeginEnableTime = xtimeGetTime();
			m_dwDelayEnableTime = dwDelayTime;
		}
		else
		{
			m_bEnabled = bEnabled;
		}			
	}

	virtual void HideDelay( DWORD dwDelayTime)
	{
		if( m_dwBeginHideTime == 0 &&
			m_dwDelayHideTime == 0 )
		{
			m_dwBeginHideTime = xtimeGetTime();
			m_dwDelayHideTime = dwDelayTime;
		}
	}

    virtual bool IsEnabled() { return m_bEnabled; }
    virtual void SetVisible( bool bVisible )
	{
		m_bVisible = bVisible; 
		if( !m_bVisible )
		{
			m_dwBeginHideTime = 0;
		}
	}
    virtual bool IsVisible() { return m_bVisible; }
	virtual bool IsMouseOver() { return m_bMouseOver; }

	virtual const char* GetText(){ return NULL;}
	virtual void SetText(const char* text){}

	virtual int  GetCurItem(){ return -1;}
	virtual int  GetItemCount(){ return 0;}
	virtual void SetCurItem(int index){}

	virtual void SetValue( int nValue ) {  }
	virtual int  GetValue() { return 0; };

	virtual void SetRange( int nMin, int nMax ){}
	virtual int GetRangeMin(){ return 0;}
	virtual int GetRangeMax(){ return 0;}

	virtual int GetPageSize() { return 0; }
	virtual void SetPageSize( int nPageSize ) { }

	virtual DWORD GetTextColor();
	virtual DWORD GetColor() { return (DWORD)-1; }
	//virtual void SaveConfigXML(TiXmlElement* item);
	//virtual void LoadConfigXML(TiXmlElement* item);

	bool IsFocus();
	
	virtual DWORD OnHiTest(POINT pt);
	
    UINT GetType() const { return m_Type; }

    int  GetID() const { return m_ID; }
    void SetID( int ID ) { m_ID = ID; }

    void SetLocation( int x, int y );
    void SetSize( int width, int height );
	
	POINT GetLocation() { POINT pt = {m_x,m_y}; return pt;}
	int	  GetX(){ return m_x;}
	int   GetY(){ return m_y;}
	int	  GetWidth(){ return m_width; }
	int	  GetHeight(){ return m_height; }

	
    void SetHotkey( const char* pszHotKey ) { m_HotKey.SetAccelKey(pszHotKey); }
    CAccelKey & GetHotkey() { return m_HotKey; }

    void SetUserData( void *pUserData ) { m_pUserData = pUserData; }
    void *GetUserData() const { return m_pUserData; }

    virtual void SetTextColor( DWORD Color );
	int GetElementCount(){ return m_Elements.size(); }
    CGuiElement* GetElement( UINT iElement ) { return m_Elements[ iElement ]; }
    HRESULT SetElement( UINT iElement, CGuiElement* pElement);

	DWORD GetStyle() const { return m_dwStyle; }
    void SetStyle( DWORD dwStyle ,bool bReinit = false) ;

    bool m_bVisible;                // Shown/hidden flag
    bool m_bMouseOver;              // Mouse pointer is above control
    bool m_bHasFocus;               // Control has input focus
    bool m_bIsDefault;              // Is the default control

    // Size, scale, and positioning members
    int m_x, m_y;
    int m_width, m_height;

	stRectI	m_rectClip;
    // These members are set by the container
    CGuiDialog* m_pDialog;    // Parent container
    
    std::vector< CGuiElement* > m_Elements;  // All display elements
	
	void SetZOrder(CGuiControl* pInsertAfter);
protected:
    virtual void UpdateRects();
	
	DWORD m_TextColor;
	CAccelKey m_HotKey;
	//DWORD	m_dwTextFormat;
#ifdef _DEBUG
	bool m_bDrag;
	bool m_bSizing;
	DWORD m_dwHiTest;
#endif
    int  m_ID;                 // ID number
    GUI_CONTROL_TYPE m_Type;  // Control type, set once in constructor  
    void *m_pUserData;         // Data associated with this control that is set by user.
    
    bool m_bEnabled;           // Enabled/disabled flag
    
	DWORD m_dwStyle;

    stRectI m_rcBoundingBox;      // Rectangle defining the active region of the control

	DWORD	m_dwBeginEnableTime;
	DWORD	m_dwDelayEnableTime;

	DWORD	m_dwBeginHideTime;
	DWORD	m_dwDelayHideTime;
public:
	//-------------------------------------------
	// 注册UI脚本函数
	//-------------------------------------------

	void SetClipRect(const RECT * prect);
	//int GetValueInScript() { return GetValue(); }
	void AddItemInScript(const char* pText) { AddItem(pText,NULL);}
	
	static void bind_lua(lua_State* L);

	//-------------------------------------------
	//-------------------------------------------
	static void RegisterScript(lua_State* lua);
};
