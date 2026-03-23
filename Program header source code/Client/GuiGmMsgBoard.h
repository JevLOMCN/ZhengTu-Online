
#pragma once
#include "..\gui\include\guidialog.h"


class CGuiGmMsgBoxDialog :
	public CGuiDialog
{
public:
	CGuiGmMsgBoxDialog(void);
	~CGuiGmMsgBoxDialog(void);

	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
 
	DWORD OnHiTest(POINT ptScreen);

	void InitCustomElements();
	void SendMsg();
private:
	bool _SendMsgToServer();

	// ccuicontrol pointer 
	CGuiMLEditBox * m_guiDetailMEBox;
	CGuiEditBox   * m_guiTelNumEditBox;

	CGuiComboBox  * m_pMsgTypeComboBox;
	CGuiComboBox  * m_pTelTypeComboBox;

	CGuiTab       * m_pTab;

	int GetCurMsgType();
	int GetCurTelType();

	bool CheckNotNull();
	
	void SetTab(int nTab);

	stRectI  m_rcMLineEdit[5];

	void LimitDetaiInfo();
};

extern bool GMSendMsgToServer(const char * pInner,int nMsgType = 0 ,int nTelType = 0,const char * pszTelephone = NULL);
