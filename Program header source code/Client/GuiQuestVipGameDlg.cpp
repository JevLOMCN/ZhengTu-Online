
#include "public.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include "./GuiQuestVipGameDlg.h"
#include "../gui/include/GuiManager.h"
#include "./Game.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "SafetyUser.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./RoleItem.h"
///////////////////////////////////////////////////////////////////////////////
 #define ID_BUTTON_ROLE_SELECT2  20    // 领取vip奖励

#define ID_BUTTON_CLOSE         0
///////////////////////////////////////////////////////////////////////////////

CGuiQuestVipGameDlg::CGuiQuestVipGameDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}
 
CGuiQuestVipGameDlg::~CGuiQuestVipGameDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuiQuestVipGameDlg::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
 
	FUNCTION_END;
}

bool CGuiQuestVipGameDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
		    case ID_BUTTON_ROLE_SELECT2:  // 领取vip奖励
			{
				stAddCountryVipCmd Cmd;
				SEND_USER_CMD(Cmd);
				Close();
			}
			break;
		case ID_BUTTON_CLOSE:
			{
			   SetVisible(false);
			   return true;
			}
			break;
		}
		
		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

HRESULT CGuiQuestVipGameDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	CMainCharacter *pMainChar = GetScene()->GetMainCharacter();

	//soke 角色vip点数
	char levelvip[256];
	DWORD nLevelvip = GetScene()->GetMainCharacter()->GetProperty()->charvip;	
	sprintf( levelvip, "%d点", nLevelvip );			
	GetStatic(4)->SetText(levelvip);

	//soke 角色vip等级
	char viplv[10];
	int nViplv = GetScene()->GetMainCharacter()->GetProperty()->charvip;	

	if(nLevelvip <= 0)
	{
		sprintf( viplv, "vip0" );
	}
	else if(nLevelvip > 0 && nLevelvip <= 10)
	{
		sprintf( viplv, "vip1" );
	}
	else if(nLevelvip > 10 && nLevelvip <= 20)
	{
		sprintf( viplv, "vip2" );
	}
	else if(nLevelvip > 20 && nLevelvip <= 30)
	{
		sprintf( viplv, "vip3" );
	}
	else if(nLevelvip > 30 && nLevelvip <= 40)
	{
		sprintf( viplv, "vip4" );
	}
	else if(nLevelvip > 40 && nLevelvip <= 50)
	{
		sprintf( viplv, "vip5" );
	}
	else if(nLevelvip > 50 && nLevelvip <= 60)
	{
		sprintf( viplv, "vip6" );
	}
	else if(nLevelvip > 60 && nLevelvip <= 70)
	{
		sprintf( viplv, "vip7" );
	}
	else if(nLevelvip > 70 && nLevelvip <= 80)
	{
		sprintf( viplv, "vip8" );
	}
	else if(nLevelvip > 80 && nLevelvip <= 90)
	{
		sprintf( viplv, "vip9" );
	}
	else if(nLevelvip > 90 && nLevelvip <= 110)
	{
		sprintf( viplv, "vip10" );
	}
	else if(nLevelvip > 110 && nLevelvip <= 130)
	{
		sprintf( viplv, "vip11" );
	}
	else if(nLevelvip > 130 && nLevelvip <= 150)
	{
		sprintf( viplv, "vip12" );
	}
	else if(nLevelvip > 150 && nLevelvip <= 170)
	{
		sprintf( viplv, "vip13" );
	}
	else if(nLevelvip > 170 && nLevelvip <= 190)
	{
		sprintf( viplv, "vip14" );
	}
	
	else if(nLevelvip > 190 && nLevelvip <= 690)
	{
		sprintf( viplv, "vip15" );
	}
	else if(nLevelvip > 690 && nLevelvip <= 1190)
	{
		sprintf( viplv, "vip16" );
	}
	else if(nLevelvip > 1190 && nLevelvip <= 1690)
	{
		sprintf( viplv, "vip17" );
	}
	else if(nLevelvip > 1690 && nLevelvip <= 2190)
	{
		sprintf( viplv, "vip18" );
	}
	else if(nLevelvip > 2190 && nLevelvip <= 2690)
	{
		sprintf( viplv, "vip19" );
	}
	else if(nLevelvip > 2690 )
	{
		sprintf( viplv, "vip20" );
	}
	GetStatic(6)->SetText(viplv);

	//soke 角色vip等级
	char lvip[256];
	int nLvip = GetScene()->GetMainCharacter()->GetProperty()->charvip;	

	if(nLevelvip <= 0)
	{
		sprintf( lvip, "距离升级vip1还差： 1点" );
	}
	else if(nLevelvip > 0 && nLevelvip <= 10)
	{
		sprintf( lvip, "距离升级vip2还差： %d点", 11-nLvip );
	}
	else if(nLevelvip > 10 && nLevelvip <= 20)
	{
		sprintf( lvip, "距离升级vip3还差： %d点", 21-nLvip );
	}
	else if(nLevelvip > 20 && nLevelvip <= 30)
	{
		sprintf( lvip, "距离升级vip4还差： %d点", 31-nLvip );
	}
	else if(nLevelvip > 30 && nLevelvip <= 40)
	{
		sprintf( lvip, "距离升级vip5还差： %d点", 41-nLvip );
	}
	else if(nLevelvip > 40 && nLevelvip <= 50)
	{
		sprintf( lvip, "距离升级vip6还差： %d点", 51-nLvip );
	}
	else if(nLevelvip > 50 && nLevelvip <= 60)
	{
		sprintf( lvip, "距离升级vip7还差： %d点", 61-nLvip );
	}
	else if(nLevelvip > 60 && nLevelvip <= 70)
	{
		sprintf( lvip, "距离升级vip8还差： %d点", 71-nLvip );
	}
	else if(nLevelvip > 70 && nLevelvip <= 80)
	{
		sprintf( lvip, "距离升级vip9还差： %d点", 81-nLvip );
	}
	else if(nLevelvip > 80 && nLevelvip <= 90)
	{
		sprintf( lvip, "距离升级vip10还差： %d点", 91-nLvip );
	}
	else if(nLevelvip > 90 && nLevelvip <= 110)
	{
		sprintf( lvip, "距离升级vip11还差： %d点", 111-nLvip );
	}
	else if(nLevelvip > 110 && nLevelvip <= 130)
	{
		sprintf( lvip, "距离升级vip12还差： %d点", 131-nLvip );
	}
	else if(nLevelvip > 130 && nLevelvip <= 150)
	{
		sprintf( lvip, "距离升级vip13还差： %d点", 151-nLvip );
	}
	else if(nLevelvip > 150 && nLevelvip <= 170)
	{
		sprintf( lvip, "距离升级vip14还差： %d点", 171-nLvip );
	}
	else if(nLevelvip > 170 && nLevelvip <= 190)
	{
		sprintf( lvip, "距离升级vip15还差： %d点", 191-nLvip );
	}
	
	else if(nLevelvip > 190 && nLevelvip <= 690)
	{
		sprintf( lvip, "距离升级vip16还差： %d点", 691-nLvip );
	}
	else if(nLevelvip > 690 && nLevelvip <= 1190)
	{
		sprintf( lvip, "距离升级vip17还差： %d点", 1191-nLvip );
	}

	else if(nLevelvip > 1190 && nLevelvip <= 1690)
	{
		sprintf( lvip, "距离升级vip18还差： %d点", 1691-nLvip );
	}

	else if(nLevelvip > 1690 && nLevelvip <= 2190)
	{
		sprintf( lvip, "距离升级vip19还差： %d点", 2191-nLvip );
	}
	else if(nLevelvip > 2190 && nLevelvip <= 2690)
	{
		sprintf( lvip, "距离升级vip20还差： %d点", 2691-nLvip );
	}
	else if(nLevelvip > 2690)
	{
		sprintf( lvip, "您的级别已经是超级vip20" );
	}

	GetStatic(8)->SetText( lvip);

	return hr;

	FUNCTION_END;
}


void CGuiQuestVipGameDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiQuestVipGame = NULL;

	FUNCTION_END;
}
