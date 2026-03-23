
#include "public.h"
#include "./GuiCongratulateInfo.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include ".\guiitem.h"
#include "./table.h"
#include "./RoleItem.h"
CGuiCongratulateInfo::CGuiCongratulateInfo()
{
	
}

CGuiCongratulateInfo::~CGuiCongratulateInfo()
{
}

void CGuiCongratulateInfo::OnCreate()
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();
	FUNCTION_END;
}


void CGuiCongratulateInfo::OnClose(void)
{
	FUNCTION_BEGIN;
	
	GetGameGuiManager()->m_guiCongratulateInfo = NULL;

	FUNCTION_END;
}



bool CGuiCongratulateInfo::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent) {
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 100:
				{
				
				}break;
			}
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiCongratulateInfo::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x+6 ,pt.y-74);
	}

	FUNCTION_END;
}

HRESULT CGuiCongratulateInfo::OnRender(float fElapsedTime)
{
	HRESULT  hr = CGuiDialog::OnRender(fElapsedTime);

	

	return hr;
}