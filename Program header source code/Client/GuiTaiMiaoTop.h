#pragma once

#include "..\gui\include\guidialog.h"

class CGuiListBox;
class CGuiTaiMiaoTopDialog :
	public CGuiDialog
{
public:
	CGuiTaiMiaoTopDialog();

	void RefreshLoongPearlList(); // 刷新太庙资源榜

	void OnClose(void);
	void OnResetScreenSize();

	void OnCreate();
	void OnSetCloseLocation();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);
	CGuiListBox* m_pListBoxRankList; // 排行榜列表


	void ShowLoongPearlRank(bool bShow);
private:
	void Update();

private:
	CGuiListBox* m_pListBoxLoongPearl; // 太庙资源榜列表

};
