#pragma once
#include "..\gui\include\guidialog.h"

enum PROGRESSBAR_TYPE
{
	PROGRESSBAR_TYPE_LOONGPEARL, // 神龙坛进度条
};

class CGuiProcess;
class CDlgProgressBar :
	public CGuiDialog
{
public:
	CDlgProgressBar(PROGRESSBAR_TYPE eType);
	~CDlgProgressBar();

	void StartCountDown(float fSecs);
	void ShutDown();

	void OnClose(void);
	void OnCreate();
	HRESULT OnRender(float fElapsedTime);

private:
	void OnTimeUp(); // 进度条时间到

private:
	PROGRESSBAR_TYPE m_eType; // 进度条类型
	float m_fLeftTime;
	float m_fTotalTime;
	bool m_bStartCountDown; // 是否开始进度条倒计时
	CGuiProcess* m_pProcess;
};
