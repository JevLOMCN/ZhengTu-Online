#pragma once 


//--------------------------------------------------------------------------------
// 结果查询显示
//--------------------------------------------------------------------------------
class CGuiLSScoreDlg : public CGuiDialog
{	
public:
	CGuiLSScoreDlg();

	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl * pControl);

	void UpdataList(DWORD nUserID,const MiniGameScore & SingleScore,const MiniGameScore & TotalScore);
	void UpdataList(std::string strText,int nSingleScore,int nSingleMoney,int nTotalScore,int nTotalMoeny);
};


class CGuiLordStrikeSortDlg : public CGuiDialog
{	
public:
	CGuiLordStrikeSortDlg();

	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl * pControl);

	void UpdateList();
};

