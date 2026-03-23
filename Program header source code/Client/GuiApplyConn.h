#pragma once
#include "..\gui\include\guidialog.h"

#define  FONTCOLOR COLOR_ARGB(255,246,201,3)

struct stApplyConn {
	char szApplyConn[MAX_NAMESIZE];
	char szName[MAX_NAMESIZE];
	
	stApplyConn(){
		szApplyConn[0]=0;
		szName[0]=0;
	}
};

class CGuiApplyConn :
	public CGuiDialog
{
	///投票列表
	CGuiListBox*          m_pListBox;

public:
	CGuiApplyConn(void);

	void OnSetCloseLocation();

	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	void RefreshListBox();
	

	///家族列表结构
	std::vector<stApplyConn>  m_SeptList;
	///帮主列表结构
	std::vector<stApplyConn>  m_TongList;
};
