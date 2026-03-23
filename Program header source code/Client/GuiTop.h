#pragma once

#include "..\gui\include\guidialog.h"

class CGuiListBox;
class CGuiTopDialog :
	public CGuiDialog
{
public:
	CGuiTopDialog();
	
	void RefreshLevelupList(); //刷新世界等级榜

	void RefreshZhanLiList(); //刷新战力榜

	void RefreshReroListT(); //刷英雄排行榜

	void RefreshReroListY(); //刷英雄排行榜

	void RefreshTicketupList(); //刷新个人积分榜

	void OnClose(void);
	void OnResetScreenSize();

	void OnCreate();
	void OnSetCloseLocation();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);

	CGuiListBox* m_pListBoxRankList; //排行榜列表

	void ShowLevelupRank(bool bShow);

	void ShowZhanLiRank(bool bShow);

	void ShowTodayHero(bool bShow);

	void ShowYesterdayHero(bool bShow);
	
	void ShowTicketupRank(bool bShow);

private:

	void Update();

private:
	CGuiListBox* m_pListBoxLevelup; //世界等级排行列表

	CGuiListBox* m_pListBoxZhanLi; //战力榜列表

	CGuiListBox* m_pListBoxTodayHero; //今日英雄排行榜

	CGuiListBox* m_pListBoxYesterdayHero; //昨日英雄排行榜

	CGuiListBox* m_pListBoxTicketup; //个人积分排行列表

};
