//#pragma once

#ifndef GUI_PASSWORD_DLG
#define GUI_PASSWORD_DLG

class CGuiPasswordDlg :
	public CGuiDialog
{
public:
	CGuiPasswordDlg(enumPasswordType type,const char* pszLabel,void * pData=NULL);
	~CGuiPasswordDlg(void);
	
	void OnClose(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	bool OnIDOK();
	bool OnIDCancel();
	HRESULT OnRender( float fElapsedTime );
private:
	enumPasswordType	m_type;
	void*				m_pData;
	std::string			m_Title;	//标题，放在该对话框的顶部
};

#endif

#ifndef GUI_MODIFY_PASSWORD_DLG
#define GUI_MODIFY_PASSWORD_DLG


class CGuiModifyPasswordDlg :
	public CGuiDialog
{
public:
	CGuiModifyPasswordDlg(enumPasswordType type,const char* pszLabel1,const char* pszLabel2,void * pData=NULL);
	~CGuiModifyPasswordDlg(void);

	void OnClose(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	bool OnUpIDOK();
	bool OnUpIDCancel();
	bool OnBottomIDOK();
	bool OnBottomIDCancel();

	HRESULT OnRender( float fElapsedTime );
private:
	enumPasswordType	m_type;
	void*				m_pData;
	std::string			m_Title1;	//标题，放在该对话框的顶部
	std::string			m_Title2;	//标题，放在该对话框的顶部
};

#endif
