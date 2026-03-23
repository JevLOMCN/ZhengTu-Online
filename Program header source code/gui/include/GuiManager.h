/**
 * \file      GuiManager.h
 * \version   $Id$
 * \author    $$$03公司@###.com
 *           $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI管理类
 * 
 *	      GUI管理类
 */

#pragma once
class CGuiDialog;
class CGuiMenu;
class Stream;
class CGuiFloatItem;
class CToolTips;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiManager
{
private:
	
	//CGuiDialog * m_Dialogs;
	std::vector< CGuiDialog* > m_Dialogs;
	CGuiDialog * m_pFocusDlg;
	CGuiDialog * m_pMouseDlg;		// 在鼠标上的GUI
	CGuiDialog * m_pPopupDlg;		
	CToolTips*	 m_pToolTips;
	stPointI	 m_ptToolTips;
	CGuiMenu*	 m_pEditMenu;
	bool		 m_bEnableIme;
private:
	CGuiDialog* m_pMouseOverDlg;
	CGuiDialog* IndexToDialog(int i)
	{
		if( i>= 0 && (size_t)i < m_Dialogs.size()) return m_Dialogs[i];
		return NULL;
	}

	int DialogToIndex(CGuiDialog* pDialog)
	{
		for(int i=0;(size_t)i<m_Dialogs.size();++i)
		{
			if(m_Dialogs[i] == pDialog) return i;
		}
		return -1;
	}

	int GetLastTopMostDlgIndex();
	int GetFirstTopMostDlgIndex();
	int GetLastBottomMostDlgIndex();
	int GetFirstBottomMostDlgIndex();

	void RemoveDlg(CGuiDialog * pDlg);
	void InsertDialog(CGuiDialog* pDlg,int index);

	void AppendDialog(CGuiDialog* pDlg);
public:

	CGuiDialog* GetPopupDlg(){ return m_pPopupDlg;}
	void SetPopupDlg(CGuiDialog* pDlg);
	enum { GUI_USER_ID = 0x1000};
	int GenerateDialogID();
	
	void SetToolTips(CToolTips* p,POINT pt){ m_pToolTips = p; m_ptToolTips = pt;}
	CToolTips* GetToolTips(){ return m_pToolTips; }
	POINT GetToolTipsPoint(){ return m_ptToolTips;}

	CGuiManager(void);
	~CGuiManager(void);
	
	HRESULT AddDialog(int ID,int x,int y,int width,int height,DWORD dwStyle,const char* pszCaption,const char* pszName,CGuiDialog* pDialog);
	HRESULT AddDialog(int ID,int x,int y,int width,int height,DWORD dwStyle = 0,const char* pszCaption = NULL,const char* pszName = NULL,CGuiDialog** ppCteated = NULL);

	void Init();
	void SetMouseDlg(CGuiDialog* pMouseDlg);
	CGuiDialog* GetMouseDlg(){ return m_pMouseDlg; }
	CGuiFloatItem* GetFloatItem();
	void CloseAllDialog();
	void OnCloseDlg(CGuiDialog* pDlg);
	void ResetFocus( );
	bool IsFocus(CGuiDialog* pDlg) { return m_pFocusDlg == pDlg; }
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void Render(float fElapsedTime);
	bool CanHaveFocus(CGuiDialog * pDlg);
	void RequestFocus(CGuiDialog * pDlg);
	void SetDialogZOrder(CGuiDialog * pDlg,CGuiDialog * pInsertAfter);
	
	//void Run(float fElapsedTime);
	HRESULT LoadGuiFromXML(const char* pszZipPackName,const char* pszFileName,CGuiDialog** pCreated = NULL);
	HRESULT LoadGuiFromXML(const char* pszFileName,CGuiDialog** pCreated = NULL);
	HRESULT LoadGuiFromXML(Stream* pStream,CGuiDialog** pCreated = NULL);
	HRESULT LoadGuiFromXML(Stream* pStream,CGuiDialog* pDialog);
	HRESULT LoadGuiFromXML(const char* pszFileName,CGuiDialog* pDialog);
	HRESULT LoadGuiFromXML(const char* pszZipPackName,const char* pszFileName,CGuiDialog* pDialog);

	CGuiDialog* GetNextDialog(CGuiDialog* pDialog);
	CGuiDialog* GetPrevDialog(CGuiDialog* pDialog);
	CGuiDialog* GetFocusDialog() { return m_pFocusDlg; }

	CGuiDialog * GetLastTopMostDlg()	{ return IndexToDialog(GetLastTopMostDlgIndex()); }
	CGuiDialog * GetFirstTopMostDlg()	{ return IndexToDialog(GetFirstTopMostDlgIndex()); }
	CGuiDialog * GetLastBottomMostDlg()	{ return IndexToDialog(GetLastBottomMostDlgIndex()); }
	CGuiDialog * GetFirstBottomMostDlg(){ return IndexToDialog(GetFirstBottomMostDlgIndex()); }

	CGuiDialog * GetTopDlg();
	CGuiDialog * GetBottomDlg();

	CGuiDialog* GetDialogByID(UINT nID);
	CGuiDialog* GetDialogByName(const char* szName);
	
	CGuiDialog* GetDialogByActionType(int type);

	void OnScreenSize();
	CGuiDialog* GetDialogByPosition(POINT ptScreen);
	void UpdateMouseOverDlg();
	CGuiDialog* GetMouseOverDlg() { return m_pMouseOverDlg; }
	CGuiMenu* CreateMenu(CGuiDialog* pOwner);
	void PopupEditMenu(CGuiDialog* pDlg);
	void InitCreate();
	bool IsShowCursor();
};

extern CGuiManager* GetGuiManager();