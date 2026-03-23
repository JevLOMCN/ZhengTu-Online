#include "public.h"

#include "command.h"
#include "..\gui\include\GuiProcess.h"

#include "DlgProgressBar.h"

namespace
{
	// 倒计时用浮点数的最小精度（单位：秒）
	const float MIN_FLOAT_PRECISION = 0.001f;
	const int PROGRESSBAR_ID = 1;
}

CDlgProgressBar::CDlgProgressBar(PROGRESSBAR_TYPE eType)
{
	FUNCTION_BEGIN;

	m_eType = eType;
	m_fLeftTime = 0.f;
	m_fTotalTime = 0.f;
	m_bStartCountDown = false;
	m_pProcess = NULL;

	FUNCTION_END;
}

CDlgProgressBar::~CDlgProgressBar()
{

}

void CDlgProgressBar::StartCountDown(float fSecs)
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

void CDlgProgressBar::ShutDown()
{
	FUNCTION_BEGIN;

	m_bStartCountDown = false;
	this->Close();

	FUNCTION_END;
}

void CDlgProgressBar::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_pDlgProgressBar = NULL;

	FUNCTION_END;
}

void CDlgProgressBar::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	m_pProcess = GetProcess(PROGRESSBAR_ID);

	FUNCTION_END;
}

HRESULT CDlgProgressBar::OnRender(float fElapsedTime)
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
		int nPos = static_cast<int>(fProgress * m_pProcess->GetRange());

		m_pProcess->SetPos(nPos);
	}

	return hr;

	FUNCTION_END;
}

void CDlgProgressBar::OnTimeUp()
{
	switch (m_eType)
	{
	case PROGRESSBAR_TYPE_LOONGPEARL:
		{
			ReqFetchLoongPearlLoongUserCmd	cmd;
			SEND_USER_CMD(cmd);
		}
		break;
	}
}
