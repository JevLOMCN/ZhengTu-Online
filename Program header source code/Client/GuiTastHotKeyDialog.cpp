#include "public.h"
#include"GuiTastHotKeyDialog.h"

CGuiTastHotKeyDialog::CGuiTastHotKeyDialog():m_Ttime(0),m_Ltime(0),IsHotKey(0)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
};

void CGuiTastHotKeyDialog::SetVisibleTime(float t)
{
	FUNCTION_BEGIN;
    m_Ltime = t;
	 this->SetVisible(true);
	//this->SetVisible(true);
	//m_Ltime = timeGetTime();
	//if(m_Ltime>=4000)
	//{
 //     
	//}
	FUNCTION_END;
}

void CGuiTastHotKeyDialog::SetVisibleHotKey(int b)
{
	FUNCTION_BEGIN;
    IsHotKey = b;

	FUNCTION_END;
}

void CGuiTastHotKeyDialog::OnCreate()
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();

	FUNCTION_END;
}

HRESULT CGuiTastHotKeyDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr =  CGuiDialog::OnRender(fElapsedTime);

	if(m_Ltime>0)
	{
		m_Ttime += fElapsedTime;

		if(m_Ttime>=m_Ltime)
		{
          m_Ltime=0;
          m_Ttime=0;
          this->SetVisible(false);

		}
      
	}
	return hr;
	FUNCTION_END;
}
