
#include "public.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include "./GuiAddSalaryDlg.h"
#include "../gui/include/GuiManager.h"
#include "./Game.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./RoleItem.h"
///////////////////////////////////////////////////////////////////////////////
#define ID_BUTTON_ROLE_SELECT  20    // 领取工资
#define ID_BUTTON_LOGIN        21    // 临时取消
 
#define ID_BUTTON_CLOSE        4   
///////////////////////////////////////////////////////////////////////////////

CGuiAddSalaryDlg::CGuiAddSalaryDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}
 
CGuiAddSalaryDlg::~CGuiAddSalaryDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuiAddSalaryDlg::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

    CMainCharacter* pMainChar = GetScene()->GetMainCharacter();

	//soke 角色等级
	char level[10];
	int nLevel = pMainChar->GetLevel();	
	sprintf( level, "%d级", nLevel );			
	GetStatic(10)->SetText(level);

	//soke 角色荣誉值
	char honor[10];
	int nHonor = pMainChar->GetHonor();	
	sprintf( honor, "%d点", nHonor );			
	GetStatic(11)->SetText(honor);

	//soke 角色在线时间
//	char onlinetime[32];
//	DWORD nOnlinetime = pMainChar->GetOnlinetime();	
//	sprintf( onlinetime, "%d小时%d分", (nOnlinetime)/3600,(nOnlinetime%3600)/60 );			
//	GetStatic(12)->SetText(onlinetime);

	//soke 角色领取工资数量 算法 等级*等级 + 在线时间
	char salary[32];
	int nSalary = pMainChar->GetSalary();
	int nLevel2 = (nLevel*nLevel);
//	DWORD nOnlinetime2 = (nOnlinetime)/3600;	

	if(nLevel2/10000 > 0)
	{
	   sprintf( salary, "%d锭%d两", nLevel2/10000, (nLevel2%10000)/100);	
//	   sprintf( salary, "%d锭%d两", (nLevel2 + nOnlinetime2)/100, (nLevel2 + nOnlinetime2)%100);
	   GetStatic(13)->SetText(salary);
	}
	else
	{
	   sprintf( salary, "%d两", (nLevel2%10000)/100);	
	   GetStatic(13)->SetText(salary);
	}
 
	FUNCTION_END;
}

bool CGuiAddSalaryDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
		case ID_BUTTON_ROLE_SELECT:  // 领取工资
			{

				stAddCountrySalaryCmd Cmd;

				SEND_USER_CMD(Cmd);
			}
			break;
		case ID_BUTTON_LOGIN:  // 临时取消
			{
				SetVisible(false);
			    return true;
			}
			break;
		case ID_BUTTON_CLOSE:  // 关闭
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


void CGuiAddSalaryDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiAddSalary = NULL;

	FUNCTION_END;
}

