#pragma once

class CGuiSendMailDialog : public CGuiDialog
{
public:
	CGuiSendMailDialog(void);
	~CGuiSendMailDialog(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	void OnSetCloseLocation();

	bool OnSendMail();
	bool OnClearMail();
	bool OnClickItem();
	bool OnClickGold();
	bool OnClickMoney();
	bool OnSwitchToRecvMailDlg();
	bool OnClickCommCheck();
	bool OnClickPaybackCheck();

	void SetSendMoney(int iMoney);
	void SetSendGoldCoin(int iGold);

	void SetSendToName(const char* toName);

	CGuiTable*  m_pItemTable;	//放物品的格子
	CGuiMoneyInputBox*	m_pMoneyInputDlg;		//送出银子输入对话框
	CGuiMoneyInputBox*	m_pGoldCoinInputDlg;	//送出金碧输入对话框
private:
	int			m_iSendMoney;		//送出的银子
	int			m_iSendGoldCoin;	//送出的金子
	CGuiEditBox* m_pToNameEditBox;		//收件人
	CGuiEditBox* m_pDingEditBox;		//锭
	CGuiEditBox* m_pLiangEditBox;		//两
	CGuiEditBox* m_pWenEditBox;			//文
	CGuiEditBox* m_pGoldCoinEditBox;	//金子
	CGuiMLEditBox*	m_pMailTextMLEditBox;	//邮件正文

	
	IBitmaps*	m_pMoneyBmp;			//银子显示
	IBitmaps*	m_pGoldCoinBmp;			//金子显示

	IBitmaps*	m_pNoMoneyBmp;			//银子显示(没有银子)
	IBitmaps*	m_pNoGoldCoinBmp;		//金子显示(没有金子)    
	CToolTips	m_toolTips;		//银子、物品的tooltip

	stRectI		m_rcSendMoney;		//发送银子的方框
	stRectI		m_rcSendGoldCoin;	//发送金子的方框
};
