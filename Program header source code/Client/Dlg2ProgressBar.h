#pragma once
#include "..\gui\include\guidialog.h"

enum PROGRESSBAR2_TYPE
{
	PROGRESSBAR2_TYPE_LOONGPEARL, // 太庙进度条
};

class CGuiProcess;
class CDlg2ProgressBar :
	public CGuiDialog
{
public:
	CDlg2ProgressBar(PROGRESSBAR2_TYPE eType);
	~CDlg2ProgressBar();

	void StartCountDown(float fSecs);
	void ShutDown();

	void OnClose(void);
	void OnCreate();
	HRESULT OnRender(float fElapsedTime);

private:
	void OnTimeUp(); // 进度条时间到

private:
	PROGRESSBAR2_TYPE m_eType; // 进度条类型
	float m_fLeftTime;
	float m_fTotalTime;
	bool m_bStartCountDown; // 是否开始进度条倒计时
	CGuiProcess* m_pProcess;
};
