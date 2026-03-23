#pragma once

/////////common control id////////////////////////////
#define ID_BUTTON_FACE 10 
#define ID_EDIT_CHAR 11
#define ID_BUTTON_SEND 12 

#define ID_PLAYER_SCROLLBAR    2 
#define ID_LISTBOX_PLAYER      8 
//////////////////////////////////////////////////////


class CUserInfo;
class CGuiLordStrikeBaseDlg;
class CListBoxHelper
{
public:
	CListBoxHelper()
	{
		pListBox= NULL;
		pScrollBar = NULL;
		pBmpHeading = NULL;
		rcClip.left = 0 ;
		rcClip.right = 0 ;
		rcClip.top = 0 ; 
		rcClip.bottom = 0;
		pDialog = NULL;
	}
	void Init(CGuiListBox * pListBox,CGuiScrollBar * pScrollBar,CGuiLordStrikeBaseDlg * pDlg);
	void Adjust();
	void Render();

private:
	CGuiListBox * pListBox;
	CGuiScrollBar * pScrollBar;
	IBitmaps * pBmpHeading;
	stRectI rcClip;
	stRectI rcHeading;

	stPointI ptRenderBmp;

	CGuiDialog * pDialog;

	stRectI rcClipListBox;
};

class CGuiLordStrikeBaseDlg : public CGuiDialog
{
public:
	CGuiLordStrikeBaseDlg();
	~CGuiLordStrikeBaseDlg();

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	bool MsgProc(HWND hWnd,UINT uMsg,WPARAM wParam, LPARAM lParam);


	//因为要用水平滚动条,所以overload InitCustomElements()
	void InitCustomElements();
	void InitScrollBarValue();

	CGuiImage * __AddImage(int nImageID);
	CGuiStatic * __AddStatic(int nStaticID);
	CGuiButton * __AddButton(int nBtnID);
	void __SetImage(CGuiImage * pImage,stResourceLocation & rl,int x,int y );
	void __SetStatic(CGuiStatic * pStatic,std::string strText,int x,int y );
	void __SetButtonBitmap(CGuiButton * pButton,stResourceLocation & rl,int x,int y,stPointF & scale);

	//scroll bar 
	void    InitScrollBar();
	void	UpdateSrollRects();
	void OnMouseWheel();
	void InitListBox();

	void UpdatePlayerRoleNameAndLevel(std::string strRole,int nLevel);
	void UpdateMainPlayerScore(int nScore);
	void UpdateMainPlayerScore(const MiniGameScore & score);

	virtual void SendChat(std::string strChat){}
	void SendEditBox();

	void AddChat(const char * pstr,DWORD dwClr = COLOR_ARGB(255,255,255,255));



	CGuiMLTextBox * pMLTExtBox;
private:
	CGuiScrollBar*	    m_pRightHorzScrollBar;
	CListBoxHelper	    m_ListBoxHelper;

	int m_nRightHorzDeltaPos;
	int m_nOldRightHorzScrollPos;

	void __UpdateFaceDlgLocation();
	
};

