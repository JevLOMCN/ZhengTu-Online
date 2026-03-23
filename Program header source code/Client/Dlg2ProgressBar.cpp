#include "public.h"

#include "command.h"
#include "..\gui\include\GuiProcess.h"

#include "Dlg2ProgressBar.h"

namespace
{
	// 倒计时用浮点数的最小精度（单位：秒）
	const float MIN_FLOAT_PRECISION = 0.001f;
	const int PROGRESSBAR_ID = 1;
}

CDlg2ProgressBar::CDlg2ProgressBar(PROGRESSBAR2_TYPE eType)
{
	FUNCTION_BEGIN;

	m_eType = eType;
	m_fLeftTime = 0.f;
	m_fTotalTime = 0.f;
	m_bStartCountDown = false;
	m_pProcess = NULL;

	FUNCTION_END;
}

CDlg2ProgressBar::~CDlg2ProgressBar()
{

}

void CDlg2ProgressBar::StartCountDown(float fSecs)
{
	FUNCTION_BEGIN;

	m_fLeftTime = fSecs;
	m_fTotalTime = fSecs;
	if (m_fLeftTime > MIN_FLOAT_PRECISION)
	{
		m_bStartCountDown = true;
	}
	else
	{
		m_bStartCountDown = false;
		this->Close();
	}

	FUNCTION_END;
}

void CDlg2ProgressBar::ShutDown()
{
	FUNCTION_BEGIN;

	m_bStartCountDown = false;
	this->Close();

	FUNCTION_END;
}

void CDlg2ProgressBar::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_pDlg2ProgressBar = NULL;

	FUNCTION_END;
}

void CDlg2ProgressBar::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	m_pProcess = GetProcess(PROGRESSBAR_ID);

	FUNCTION_END;
}

HRESULT CDlg2ProgressBar::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	if (m_bStartCountDown && (m_fLeftTime > MIN_FLOAT_PRECISION))
	{
		float fTemp = m_fLeftTime - fElapsedTime;
		if (fTemp > MIN_FLOAT_PRECISION)
		{
			m_fLeftTime = fTemp;
		}
		else
		{
			m_bStartCountDown = false;
			m_fLeftTime = 0.f;
			this->Close();

			this->OnTimeUp();
		}
	}

	if (m_pProcess)
	{
		float fProgress = (m_fTotalTime - m_fLeftTime) / m_fTotalTime;
		int nPos = static_cast<int>(fProgress * m_pProcess->Get2Range());

		m_pProcess->SetPos(nPos);
	}

	return hr;

	FUNCTION_END;
}

void CDlg2ProgressBar::OnTimeUp()
{
	switch (m_eType)
	{
	case PROGRESSBAR2_TYPE_LOONGPEARL:
		{
			ReqGenghuanLoongPearlLoongUserCmd cmd;
			SEND_USER_CMD(cmd);
		}
		break;
	}
}
