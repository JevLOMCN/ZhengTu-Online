#pragma once

class CGuiRecvMailDialog : public CGuiDialog
{
public:
	CGuiRecvMailDialog(void);
	~CGuiRecvMailDialog(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	void OnSetCloseLocation();

	bool OnOpenMail();
	bool OnSendBack();
	bool OnWriteBack();
	bool OnDeleteMail();
	bool OnRecvItem();
	bool OnPreBtn();
	bool OnNextBtn();
	bool OnSwitchToSendMailDlg();

	DWORD GetMailIDFromList(int index);
	DWORD GetCurSelectMailId() const { return m_dwCurMailId; }
	void UpdateMailList();
	

	bool ParseMailContent(stContentMail* cmd);
	void OnConfirmReceiveItem(DWORD dwMailId,bool bOk = true);
	void OnRemoveMail(DWORD dwMailId);


	CGuiTable*  m_pItemTable;	//放物品的格子

private:
	void UpdateMailListText();
	void UpdateCtrlState();
	void UpdatePageButtonState();

	void UpdateMailListItem(DWORD dwMailID);

	bool IsSystemMail();
private:
	DWORD		m_dwCurMailId;			//当前邮件
	IBitmaps*	m_pMoneyBmp;			//银子显示
	IBitmaps*	m_pGoldCoinBmp;			//金子显示
	IBitmaps*	m_pNoMoneyBmp;			//银子显示(没有银子)
	IBitmaps*	m_pNoGoldCoinBmp;		//金子显示(没有金子)

	IBitmaps*	m_pNotReadMailBmp;		//没有读过的信
	IBitmaps*	m_pReadedMailBmp;		//读过的信

	
	CToolTips	m_toolTips;		//银子、物品的tooltip

	CGuiMLTextBox *	m_pMailMLTextBox;	//邮件正文
	CGuiListBox	 *	m_pMailList;		//邮件列表

	int			m_iSendToMeMoney;		//送给我的银子
	int			m_iSendToMeGoldCoin;	//送给我的金子

	int			m_iPayBackMoney;		//返还的银子
	int			m_iPayBackGoldCoin;		//返还的金子

	int			m_iCurPage;			//当前页
	int			m_iTotalPage;		//总共页数

	stRectI		m_rcSendMoney;		//发送银子的方框
	stRectI		m_rcSendGoldCoin;	//发送金子的方框

	CGuiMessageBox*	m_pConfirmRecvItem;
};
