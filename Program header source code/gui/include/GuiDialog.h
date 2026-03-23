/**
* \file      GuiDialog.h
* \version   $Id$
* \author    $$$03公司@###.com
*           $$$03公司@###.com
* \date      2006-7-19  21:11:00
* \brief     GUI窗口基类
* 
*	      GUI窗口基类
*/

#pragma once

#include "guiImage.h"
#include "guiMLEditBox.h"

//#include "../../LuaWrapper/lua_State.h"
//#include "../../LuaWrapper/lua_ex.h"
//extern stPointI s_ctrlBorderTopLeft;
//extern stPointI s_ctrlBorderRightBottom;
extern DWORD s_PatchAlignFormat[];
//extern int s_nSBWidth;
/**
* \brief 简短描述
* 
* 详细描述
* 
*/
class CGuiManager;


struct stGuiCtrlBorderInfo{
	stGuiCtrlBorderInfo();
	void CuttRect9(RECT* pRect);
	CGuiElement element[9];
	stPointI ptLeftTop;
	stPointI ptRightBottom;
	stPointI ptBorderLeftTop;
	stPointI ptBorderRightBottom;
	stRectI  rcTexture;
};

typedef std::map< int, stGuiCtrlBorderInfo* > t_GuiMapCtrlBorder;

enum enumGuiAnimationEvent{
	GuiAniEvent_None,
	GuiAniEvent_Close,
	GuiAniEvent_Hide,
	GuiAniEvent_Anchor,
	GuiAniEvent_StopAnchor,
	GuiAniEvent_RunFun,
};

struct stGuiAnimationInfo{
	stPointI ptDst;
	DWORD	 dwColorDst;
	float	 fTime;	// 时间（秒）
	enumGuiAnimationEvent eAniComplete;// 动画完成的事件
};

struct stGuiAnimationInfoEx : public stGuiAnimationInfo{
	float	 fRunTime;
	stPointF pos;
	float	 a,r,g,b;
	float	 dx,dy;
	float	 da,dr,dg,db;
};
struct lua_State;
class CLuaState;
/**
* \brief 简短描述
* 
* 详细描述
* 
*/
class CGuiDialog
{
	friend class CGuiManager;
protected:
	stGuiAnimationInfoEx *m_pAniInfo;
	CGuiButton * m_pBtnClose;
	GUI_ALIGN_TYPE m_eAlign;
	CLuaState* m_lua;
	IBitmaps* m_pIcon;
	stPointI  m_ptIcon;
	bool	 m_bSmDialog;
	bool	 m_bAnchor;
	bool	 m_bPlayOpenSound;
	bool	 m_bPlayCloseSound;
	// 界面显示和隐藏音效
	int      m_nShowSoundIndex;
	int      m_nHideSoundIndex;
	CGuiControl* m_pLastFocus;
public:
	virtual ~CGuiDialog();
	static CGuiControl* GetControlFocus()
	{ 
		return s_pControlFocus;
	}
	bool SetIcon(stResourceLocation * rl);
	bool	 m_bCloseIsHide;
	static stPointI s_borderTopLeft;
	static stPointI s_borderRightBottom;
	static stPointI s_clientOffsetTopLeft;
	static stPointI s_clientOffsetRightBottom;
	static stRectI  s_borderRect;
	static stPointI s_ptLeftTopOffset;

	static stPointI s_smBorderTopLeft;
	static stPointI s_smBorderRightBottom;
	static stPointI s_smClientOffsetTopLeft;
	static stPointI s_smClientOffsetRightBottom;
	static stRectI  s_smBorderRect;

	stPointI m_ptCaptionOffset;
	stPointI m_ptLeftTopOffset;

	stPointI	m_ptBackGroundOffsetTopLeft;
	stPointI	m_ptBackGroundOffsetRightBottom;

	int m_nSBWidth;
	DWORD		m_dwTimeAnchor;
#ifdef _DEBUG
	std::string m_strScript;
#endif
	const char* GetScript()
	{

#ifdef _DEBUG
		return m_strScript.c_str();
#else
		return NULL;
#endif

	}

	lua_State* GetScriptEngine();

	void SetScript(const char* script);

	virtual void OnInitScript();

	void SetAlign(GUI_ALIGN_TYPE type)
	{
		m_eAlign = type;
		UpdateLocation();
	}
	CGuiDialog(DWORD dwStyle = 0);

	int ControlToIndex(CGuiControl* pControl);
	// Windows message handler
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	virtual void OnFocusIn() {}
	virtual void OnFocusOut() {}

	virtual BOOL ContainsPoint(POINT ptScreen);

	virtual BOOL IsInRect(POINT ptScreen);

	virtual DWORD OnHiTest(POINT ptScreen);

	virtual void OnCreate();

	virtual void OnHide();

	virtual void OnCompleteAniEvent();

	virtual bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	bool BeginAnimation(stGuiAnimationInfo* pInfo);
	void RunAnimation(float fElaspedTime);
	// Control creation
	HRESULT AddTab( int ID,int x,int y,int width,int height,DWORD style,CGuiTab** ppCreated);
	HRESULT AddTable(int ID,int x,int y,int cellWidth,int cellHeight,int colCount,int rowCount,int showRowCount,bool bVirtual ,CGuiTable** ppCreated = NULL);
	HRESULT AddItemCell(int ID,int x,int y,CGuiItemCell** ppCreated);
	HRESULT AddImage(int ID,int x,int y,const stResourceLocation & rl,const RECT * imgRect = NULL,DWORD dwColor = -1,int width = -1,int height = -1,DWORD dwStyle = CGuiImage::FULLSIZE_IMAGE,CGuiImage ** ppCreated = NULL);
	HRESULT AddStatic( int ID, const char* strText, int x, int y, int width, int height, bool bIsDefault=false, CGuiStatic** ppCreated=NULL );
	HRESULT AddButton( int ID, const char* strText, int x, int y, int width, int height, const char* nHotkey=0, bool bIsDefault=false, CGuiButton** ppCreated=NULL );
	HRESULT AddCheckBox( int ID, const char* strText, int x, int y, int width, int height, bool bChecked=false, const char* nHotkey=0, bool bIsDefault=false, CGuiCheckBox** ppCreated=NULL );
	HRESULT AddRadioButton( int ID, UINT nButtonGroup, const char* strText, int x, int y, int width, int height, bool bChecked=false, const char* nHotkey=0, bool bIsDefault=false, CGuiRadioButton** ppCreated=NULL );
	HRESULT AddComboBox( int ID, int x, int y, int width, int height, const char* nHotKey=0, bool bIsDefault=false, CGuiComboBox** ppCreated=NULL );
	HRESULT AddScrollBar( int ID, int x, int y, int width, int height, int min, int max, int pagesize, bool bIsDefault=false, CGuiScrollBar** ppCreated=NULL ,int nStyle = 0);
	HRESULT AddSlider( int ID, int x, int y, int width, int height, int min=0, int max=100, int value=50, bool bIsDefault=false, CGuiSlider** ppCreated=NULL );
	HRESULT AddEditBox( int ID, const char* strText, int x, int y, int width, int height, bool bIsDefault=false, CGuiEditBox** ppCreated=NULL );
	HRESULT AddMLEditBox(int ID,const char* strText,int x,int y,int width,int height,DWORD dwStyle = CGuiMLEditBox::AUTONEWLINE,bool isDefault=false,CGuiMLEditBox** ppCreated = NULL);
	HRESULT AddProcess(int ID,int x,int y,int width,int height,int nMin=0,int nMax=100,int nPos=0,bool isDefault=false,CGuiProcess** ppCreated = NULL);
	HRESULT AddScrollTab(int ID,int x,int y,int width,int height,bool isDefault=false,CGuiScrollTab** ppCreated = NULL);
	HRESULT AddMLTextBox(int ID,const char* strText,int x,int y,int width,int height,DWORD dwStyle,bool isDefault,CGuiMLTextBox** ppCreated = NULL);

	HRESULT AddListBox( int ID, int x, int y, int width, int height, DWORD dwStyle=0, CGuiListBox** ppCreated=NULL );
	HRESULT AddTree( int ID, int x, int y, int width, int height, DWORD dwStyle=0, CGuiTree** ppCreated=NULL );
	HRESULT AddControl( CGuiControl* pControl );
	HRESULT InitControl( CGuiControl* pControl );
	HRESULT InitControl( CGuiControl* pControl ,int iControlType);

	// Control retrieval
	CGuiStatic*      GetStatic( int ID ) { return (CGuiStatic*) GetControl( ID, GUI_CONTROL_STATIC ); }
	CGuiButton*      GetButton( int ID ) { return (CGuiButton*) GetControl( ID, GUI_CONTROL_BUTTON ); }
	CGuiCheckBox*    GetCheckBox( int ID ) { return (CGuiCheckBox*) GetControl( ID, GUI_CONTROL_CHECKBOX ); }
	CGuiRadioButton* GetRadioButton( int ID ) { return (CGuiRadioButton*) GetControl( ID, GUI_CONTROL_RADIOBUTTON ); }
	CGuiComboBox*    GetComboBox( int ID ) { return (CGuiComboBox*) GetControl( ID, GUI_CONTROL_COMBOBOX ); }
	CGuiSlider*      GetSlider( int ID ) { return (CGuiSlider*) GetControl( ID, GUI_CONTROL_SLIDER ); }
	CGuiEditBox*     GetEditBox( int ID ) { return (CGuiEditBox*) GetControl( ID, GUI_CONTROL_EDITBOX ); }
	//CGuiImeEditBox*  GetIMEEditBox( int ID ) { return (CGuiImeEditBox*) GetControl( ID, GUI_CONTROL_IMEEDITBOX ); }
	CGuiListBox*     GetListBox( int ID ) { return (CGuiListBox*) GetControl( ID, GUI_CONTROL_LISTBOX ); }
	CGuiImage*	 GetImage( int ID ) { return (CGuiImage*) GetControl( ID, GUI_CONTROL_IMAGE ); }
	CGuiMLEditBox*   GetMLEditBox( int ID ) { return (CGuiMLEditBox*) GetControl( ID, GUI_CONTROL_MLEDITBOX ); }
	CGuiItemCell*    GetItemCell( int ID ) { return (CGuiItemCell*) GetControl( ID, GUI_CONTROL_ITEMCELL ); }
	CGuiTable*	 GetTable( int ID ) { return (CGuiTable*) GetControl( ID, GUI_CONTROL_TABLE ); }
	CGuiProcess*     GetProcess( int ID ) { return (CGuiProcess*) GetControl( ID, GUI_CONTROL_PROCESS ); }
	CGuiTab*	 GetTab( int ID ) { return (CGuiTab*) GetControl( ID, GUI_CONTROL_TAB ); }
	CGuiScrollTab*   GetScrollTab( int ID ) { return (CGuiScrollTab*) GetControl( ID, GUI_CONTROL_SCROLLTAB ); }
	CGuiMLTextBox*   GetMLTextBox(int ID) { return (CGuiMLTextBox*) GetControl(ID,GUI_CONTROL_MLTEXTBOX);}

	CGuiControl* GetControl( int ID );
	CGuiControl* GetControl( int ID, UINT nControlType );
	virtual CGuiControl* GetControlAtPoint( POINT pt );

	bool AddControl2Tab(CGuiTab *pTab,int frame,int ctrlID);
	bool AddControl2TabByID(int tabID,int frame,int ctrlID);

	void SetButtonGroup(int button,int group);

	void CheckRadioButton(int nIDFirstButton,int nIDLastButton,int nIDCheckButton );
	int GetCheckedRadioButton(int nIDFirstButton,int nIDLastButton );

	bool GetControlEnabled( int ID );
	void SetControlEnabled( int ID, bool bEnabled );
	void SetControlEnabled(GUI_CONTROL_TYPE nControlType, bool bEnabled);

	void ClearRadioButtonGroup( UINT nGroup );
	void ClearButtonGroup( UINT nGroup );
	void ClearComboBox( int ID );

	int  FindControl(CGuiControl * pControl);
	void SetControlZOrder(CGuiControl* pControl,int index);
	void SetControlZOrder(CGuiControl* pControl,CGuiControl * pInsertAfter);

	void SetZOrder(CGuiDialog * pInsertAfter);

	CGuiElement* GetDefaultElement( UINT nControlType, UINT iElement );

	// Methods called by controls
	bool SendEvent( UINT nEvent, bool bTriggeredByUser, CGuiControl* pControl ,DWORD dwID = -1);
	void RequestFocus( CGuiControl* pControl );
	virtual bool ResetFocus();

	// Render helpers
	HRESULT FillRect(const RECT *pRect,DWORD color);
	HRESULT DrawRectangle(const RECT* pRect, DWORD color);
	HRESULT DrawRect( const RECT* pRect, DWORD color );
	HRESULT DrawPolyLine( POINT* apPoints, UINT nNumPoints, DWORD color );
	HRESULT DrawSprite( CGuiElement* pElement, RECT* prcDest );
	HRESULT CalcTextRect( const char* strText, CGuiElement* pElement, RECT* prcDest, int nCount = -1 );
	HRESULT DrawText( const char* strText, CGuiElement* pElement, RECT* prcDest, enumFontEffect fontEffect = FontEffect_None, int nCount = -1 );
	HRESULT DrawText( const WCHAR* strText, CGuiElement* pElement, RECT* prcDest, enumFontEffect fontEffect = FontEffect_None, int nCount = -1 ,DWORD* pData = NULL);
	// Attributes
	bool GetMinimized() { return m_bMinimized; }
	void SetMinimized( bool bMinimized ) { m_bMinimized = bMinimized; }
	void SetBackgroundColors( DWORD colorAllCorners ) { SetBackgroundColors( colorAllCorners, colorAllCorners, colorAllCorners, colorAllCorners ); }
	void SetBackgroundColors( DWORD colorTopLeft, DWORD colorTopRight, DWORD colorBottomLeft, DWORD colorBottomRight );
	void EnableCaption( bool bEnable ) { m_bCaption = bEnable; }
	int GetCaptionHeight() const { return m_nCaptionHeight; }
	void SetCaptionHeight( int nHeight ) { m_nCaptionHeight = nHeight; }
	void SetCaptionText( const char *pszText ) { strncpy( m_szCaption, pszText, sizeof(m_szCaption)/sizeof(m_szCaption[0]) ); m_szCaption[sizeof(m_szCaption)/sizeof(m_szCaption[0])-1] = 0; }
	//void GetLocation( POINT &Pt ) const { Pt.x = m_x; Pt.y = m_y; }
	POINT GetLocation() { POINT pt = {m_x,m_y}; return pt;}
	void SetLocation( int x, int y );
	void SetSize( int width, int height );
	int GetX(){ return m_x;}
	int GetY(){ return m_y;}

	int GetClientX(){ return m_x + (m_clientOffsetTopLeft.x);}
	int GetClientY(){ return m_y + (m_clientOffsetTopLeft.y);}
	int GetClientWidth(){ return m_width - ( m_clientOffsetTopLeft.x + m_clientOffsetRightBottom.x);}
	int GetClientHeight(){ return m_height - (m_clientOffsetTopLeft.y + m_clientOffsetRightBottom.y);}
	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }

	RECT GetClipRect();

	void BerthShow();
	void BerthHide();
	bool IsAnchor(){ return m_bAnchor;}

	DWORD GetStyle(){ return m_dwStyle; }
	void SetStyle(const DWORD& style) { m_dwStyle = style; }
	int  GetID() const { return m_ID; }
	void SetID( int ID ) { m_ID = ID; }

	//fancui
	virtual	void SetShowSoundIndex(int soundindex)
	{
		m_nShowSoundIndex = soundindex;
	}
	int  GetShowSoundIndex() 
	{ 
		return m_nShowSoundIndex;
	}
	virtual void SetHideSoundIndex(int  soundindex)
	{
		m_nHideSoundIndex = soundindex;
	}
	int  GetHideSoundIndex()
	{
		return m_nHideSoundIndex;
	}
	static void SetRefreshTime( float fTime ){ s_fTimeRefresh = fTime; }

	CGuiControl* GetNextControl( CGuiControl* pControl );
	CGuiControl* GetPrevControl( CGuiControl* pControl );

	void RemoveControl(CGuiControl* pControl);
	void RemoveControl( int ID );
	void RemoveAllControls();

	// Sets the callback used to notify the app of control events
	//void SetCallback( PCALLBACKGUIEVENT pCallback ) { m_pCallbackEvent = pCallback; }
	void EnableNonUserEvents( bool bEnable ) { m_bNonUserEvents = bEnable; }
	void EnableKeyboardInput( bool bEnable ) { m_bKeyboardInput = bEnable; }
	void EnableMouseInput( bool bEnable ) { m_bMouseInput = bEnable; }

	bool IsClosed(){ return m_bClosed;}
	bool IsEnabled() { return m_bEnabled;}
	bool IsVisible(){ return m_bVisible;}

	void SetEnabled(bool b);
	void SetVisible(bool b,bool bResetZOrder = true);
	void Hide(){ SetVisible(false); }

	bool IsCaption(){ return m_bCaption; }

	static bool IsFocus(CGuiControl* pControl) { return s_pControlFocus == pControl; }
	bool IsFocus();
	// Device state notification

	void Refresh();
	virtual HRESULT OnRender( float fElapsedTime );    
	virtual void RenderBackground(float fElapsedTime);
	virtual void RefreshFrame();
	// Shared resource access. Indexed fonts and textures are shared among
	// all the controls.
	HRESULT       SetFont( UINT index, const char* strFaceName, LONG height, LONG weight );

	static void ClearFocus();
	bool FocusDefaultControl();

	virtual void UpdateLocation(bool bReAlign = true);
	virtual void OnSetCloseLocation();
	void SetBackImage(const stResourceLocation& rl,const RECT * imgRect = NULL,DWORD color = -1,int x= 0,int y = 0,bool bReAlgin = false,int width = -1,int height = -1);
	bool m_bNonUserEvents;
	bool m_bKeyboardInput;
	bool m_bMouseInput;
protected:

	enum enumSizingType{
		eSizingType_None = 0,
		eSizingType_Top = BIT(0),
		eSizingType_Bottom = BIT(1),
		eSizingType_Left = BIT(2),
		eSizingType_Right = BIT(3),

		eSizingType_TopLeft = eSizingType_Top | eSizingType_Left,
		eSizingType_TopRight = eSizingType_Top | eSizingType_Right,
		eSizingType_BottomLeft = eSizingType_Bottom | eSizingType_Left,
		eSizingType_BottomRight = eSizingType_Bottom | eSizingType_Right,
	};

	enumSizingType HiTestCodeToSizingType(DWORD dwHiTest)
	{
		switch(dwHiTest)
		{
		case HTTOP:
			return eSizingType_Top;
		case HTBOTTOM:
			return eSizingType_Bottom;
		case HTLEFT:
			return eSizingType_Left;
		case HTRIGHT:
			return eSizingType_Right;
		case HTTOPLEFT:
			return eSizingType_TopLeft;
		case HTTOPRIGHT:
			return eSizingType_TopRight;
		case HTBOTTOMLEFT:
			return eSizingType_BottomLeft;
		case HTBOTTOMRIGHT:
			return eSizingType_BottomRight;
		}
		return eSizingType_None;
	}

	bool m_bClosed;
	bool m_bBottomMost;
	bool m_bTopMost;
	bool m_bEnabled;
	bool m_bVisible;

	stPointI m_minSize;
	stPointI m_maxSize;
	int m_nDefaultControlID;

	static double s_fTimeRefresh;
	double m_fTimeLastRefresh;

	// Windows message handlers
	void OnMouseMove( POINT pt );
	void OnMouseUp( POINT pt );

	// Control events
	bool OnCycleFocus( bool bForward );
	void OnMouseEnter( CGuiControl* pControl );
	void OnMouseLeave( CGuiControl* pControl );

	static CGuiControl* s_pControlFocus;        // The control which has focus
	static CGuiControl* s_pControlPressed;      // The control currently pressed

	CGuiControl* m_pControlMouseOver;           // The control which is hovered over
	CGuiImage*	 m_pBackImage;

	bool m_bDrag;
	bool m_bSizing;
	bool m_bCaption;
	bool m_bMinimized;
	char m_szCaption[256];
	char m_szName[64];

	int m_x;
	int m_y;
	int m_width;
	int m_height;
	int m_nCaptionHeight;

	DWORD m_dwColor;
	DWORD m_colorTopLeft;
	DWORD m_colorTopRight;
	DWORD m_colorBottomLeft;
	DWORD m_colorBottomRight;
	DWORD m_colorCaption;
	DWORD m_colorBorder;
	//PCALLBACKGUIEVENT m_pCallbackEvent;

	//std::vector< int > m_Textures;   // Index into m_TextureCache;
	//std::vector< int > m_Fonts;      // Index into m_FontCache;

	std::vector< CGuiControl* > m_Controls;
	std::vector< GuiElementHolder* > m_DefaultElements;

	static std::vector< GuiElementHolder* > s_DefaultElements;
	static CGuiElement s_CapElement;
	static std::vector< CGuiElement* > s_BackElements;
	static t_GuiMapCtrlBorder s_mapCtrlBorder;
	static CGuiElement		  s_BackGroundElement;
	static CGuiElement		  s_LeftTopElement;

	std::vector< CGuiElement* > m_BackElements;
	t_GuiMapCtrlBorder m_mapCtrlBorder;

	CGuiElement * m_pBackGroundElement;
	CGuiElement * m_pCapElement;

	CGuiElement * m_pLeftTopElement;

	stPointI m_borderTopLeft;
	stPointI m_borderRightBottom;

	stPointI m_clientOffsetTopLeft;
	stPointI m_clientOffsetRightBottom;

	stRectI					 m_BackRects[9];

	int			m_ID;
	DWORD		m_dwStyle;
	int						m_nCapOffsetY;
	CGuiMenu* m_pPopupMenu;
public:
	stGuiCtrlBorderInfo * GetCtrlBorderInfo(int iCtrlType);
	static stGuiCtrlBorderInfo * GetDefaultCtrlBorderInfo(int iCtrlType);
	stGuiCtrlBorderInfo * GetCustomCtrlBorderInfo(int iCtrlType);

	void SetColor(DWORD color);
	DWORD GetColor() { return m_dwColor;}
	DWORD GenerateControlID();

	stPointI m_ctrlBorderTopLeft[GUI_CONTROL_NUMBER];
	stPointI m_ctrlBorderRightBottom[GUI_CONTROL_NUMBER];

	// 设置不同的皮肤
	virtual void InitCustomElements();

	// 设置缺省的皮肤
	static void InitDefaultElements();
	static void UninitDefaultElement();
	// Access the default display Elements used when adding new controls
	static HRESULT  SetDefaultElement( UINT nControlType, UINT iElement, CGuiElement* pElement );
	static void SaveDefaultElementToXML(const char* pszFileName);
	static bool LoadDefaultElementFromXML(const char* pszZipPackName,const char* pszFileName);
	static bool LoadDefaultElementFromXML(Stream* pStream);

	CGuiElement* GetCaptionElement();
	void ClientToScreen(POINT* pt);
	void ClientToScreen(RECT* rc);

	void ScreenToClient(POINT* pt);
	void ScreenToClient(RECT* rc);

	CGuiDialog* GetNextDialog();
	CGuiDialog* GetPrevDialog();
	void Close(void);
	virtual void OnClose();
	// 保存窗口配置为XML文件
	HRESULT SaveGuiToXML(const char* pszFileName);

	const char* GetName() { return m_szName;}
	void SetName(const char* pszName) {if(!strncpy(m_szName,pszName,sizeof(m_szName))) m_szName[sizeof(m_szName)-1] = 0;}

	void SetFocus(CGuiControl* pControl);
	HRESULT PatchDrawSprite(CGuiElement* pElement, RECT* prcDest,DWORD dwAlignFormat = DT_TOP | DT_LEFT);

	void DrawControlBorder(const RECT & rect,DWORD dwControlType,DWORD color = -1,bool bDrawCenter = true);
	HRESULT SetCustomElement( UINT nControlType, UINT iElement, CGuiElement* pElement );
	CGuiButton* GetCloseButton(){ return m_pBtnClose;}
	CGuiImage* GetBackImage(){ return m_pBackImage;}

	virtual void OnResetScreenSize();
	virtual void BeginPopup(void);
	virtual void EndPopup(void);

	virtual int GetActionType() { return -1; }

	virtual void UpdateClientRect();
	bool IsSystemDialog(){ return (GetStyle() & GUI_WS_SYSTEM) != 0;}

	CGuiMenu* GetPopupMenu();
	bool SetPopupMenu(CGuiMenu* pMenu);
	virtual bool OnContextMenu();
	//-------------------------------------------
	// 注册UI脚本函数
	//-------------------------------------------
	CGuiControl* GetControlInScript(int nID){ return GetControl(nID);}
	bool IsFocusInScript() { return IsFocus();}
	void SetVisibleInScript(bool b) { SetVisible(b);}

	static void _bind_lua(lua_State* L);
	virtual void bind_lua(lua_State * L);
};

extern const char* GetGuiGraphicPackName();