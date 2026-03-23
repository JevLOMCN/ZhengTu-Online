#pragma once

#include "..\gui\include\guidialog.h"

class CGuiListBox;
class CGuiHalofFameTopDialog :
	public CGuiDialog
{
public:
	CGuiHalofFameTopDialog();

	void RefreshFlowerList(); // 刷新鲜花排行榜

	void OnClose(void);
	void OnResetScreenSize();

	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);

	CGuiListBox* m_pListBoxRankList; // 排行榜列表


	void ShowFlowerRank(bool bShow);
private:
	void Update();

private:
	CGuiListBox* m_pListBoxFlower; // 鲜花榜列表

};
