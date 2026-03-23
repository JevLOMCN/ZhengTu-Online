
#include "public.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include "./GuiMagicBoxDlg.h"
#include "../gui/include/GuiManager.h"
#include "./Game.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "SafetyUser.h"
#include "./GameScene.h"
#include "./character.h"
#include "./MainCharacter.h"
///////////////////////////////////////////////////////////////////////////////
#define ID_BUTTON_ROLE_SELECT2 3 // 领取登陆奖励

#define ID_BUTTON_CLOSE 0

#define MAX_PAGE 32	 // 时装最大页数
#define MAX_PAGE2 12 // 披风最大页数
#define MAX_PAGE3 4 // 披风最大页数
#define MAX_PAGE4 12 // 坐骑最大页数
//这里有个最大页数 改一下就行了 比如现在是24个坐骑 你加了6个 那这里改成5就行了
///////////////////////////////////////////////////////////////////////////////

CGuiMagicBoxDlg::CGuiMagicBoxDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiMagicBoxDlg::~CGuiMagicBoxDlg()
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}

// 创建对话框
void CGuiMagicBoxDlg::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	type = 0; // 默认时装页
	page = 1;

	mohelevel = 0;
	moheexp = 0;
	mohemaxexp = 50000;
	shizhuangshu = 0;
	pifengshu = 0;
	chibangshu = 0;
	zuoqishu = 0;
	shuaxin();
	FUNCTION_END;
}

bool CGuiMagicBoxDlg::OnGuiEvent(UINT nEvent, UINT nID, CGuiControl *pControl)
{
	FUNCTION_BEGIN;

	if (nEvent == EVENT_CHECKBOX_CHANGED)
	{
		if (type == 0)
		{
			if (!GetCheckBox(nID)->GetChecked())
			{
				// 请求取消佩戴称号
				stSetNotShizhuangUserCmd cmd1;
				CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
				if (pMainRole)
					cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd1.dwOldTempID = 0;
				cmd1.shizhuangid = (page - 1) * 6 + (nID / 10) - 1;
				SEND_USER_CMD(cmd1);

				stGetShizhuangPropertyUserCmd cmd2;
				if (pMainRole)
					cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd2.dwOldTempID = 0;
				SEND_USER_CMD(cmd2);
				return false;
			}
			if (GetCheckBox(15)->GetChecked() && nID == 15)
			{
				if (Shizhuang[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此时装无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetShizhuangUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.shizhuangid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(25)->GetChecked() && nID == 25)
			{
				if (Shizhuang[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此时装无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetShizhuangUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.shizhuangid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(35)->GetChecked() && nID == 35)
			{
				if (Shizhuang[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此时装无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetShizhuangUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.shizhuangid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(45)->GetChecked() && nID == 45)
			{
				if (Shizhuang[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此时装无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetShizhuangUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.shizhuangid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(55)->GetChecked() && nID == 55)
			{
				if (Shizhuang[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此时装无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetShizhuangUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.shizhuangid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(65)->GetChecked() && nID == 65)
			{
				if (Shizhuang[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此时装无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetShizhuangUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.shizhuangid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
			}
		}
		else if (type == 1) // 披风
		{

			if (!GetCheckBox(nID)->GetChecked())
			{
				// 请求取消佩戴披风
				stSetNotPifengUserCmd cmd1;
				CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
				if (pMainRole)
					cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd1.dwOldTempID = 0;
				cmd1.pifengid = (page - 1) * 6 + (nID / 10) - 1;
				SEND_USER_CMD(cmd1);

				stGetShizhuangPropertyUserCmd cmd2;
				if (pMainRole)
					cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd2.dwOldTempID = 0;
				SEND_USER_CMD(cmd2);
				return false;
			}
			if (GetCheckBox(15)->GetChecked() && nID == 15)
			{
				if (Pifeng[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此披风无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetPifengUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.pifengid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(25)->GetChecked() && nID == 25)
			{
				if (Pifeng[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此披风无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetPifengUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.pifengid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(35)->GetChecked() && nID == 35)
			{
				if (Pifeng[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此披风无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetPifengUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.pifengid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(45)->GetChecked() && nID == 45)
			{
				if (Pifeng[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此披风无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetPifengUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.pifengid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(55)->GetChecked() && nID == 55)
			{
				if (Pifeng[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此披风无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetPifengUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.pifengid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(65)->GetChecked() && nID == 65)
			{
				if (Pifeng[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此披风无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetPifengUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.pifengid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
			}
		}
		else if (type == 2) // 翅膀
		{

			if (!GetCheckBox(nID)->GetChecked())
			{
				// 请求取消佩戴翅膀
				stSetNotChibangUserCmd cmd1;
				CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
				if (pMainRole)
					cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd1.dwOldTempID = 0;
				cmd1.chibangid = (page - 1) * 6 + (nID / 10) - 1;
				SEND_USER_CMD(cmd1);

				stGetShizhuangPropertyUserCmd cmd2;
				if (pMainRole)
					cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd2.dwOldTempID = 0;
				SEND_USER_CMD(cmd2);
				return false;
			}
			if (GetCheckBox(15)->GetChecked() && nID == 15)
			{
				if (Chibang[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此翅膀无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetChibangUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.chibangid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(25)->GetChecked() && nID == 25)
			{
				if (Chibang[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此翅膀无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetChibangUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.chibangid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(35)->GetChecked() && nID == 35)
			{
				if (Chibang[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此翅膀无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetChibangUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.chibangid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(45)->GetChecked() && nID == 45)
			{
				if (Chibang[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此翅膀无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetChibangUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.chibangid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(55)->GetChecked() && nID == 55)
			{
				if (Chibang[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此翅膀无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetChibangUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.chibangid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(65)->GetChecked() && nID == 65)
			{
				if (Chibang[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此翅膀无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetChibangUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.chibangid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
			}
		}
		else if (type == 3) // 坐骑
		{

			if (!GetCheckBox(nID)->GetChecked())
			{
				// 请求取消佩戴坐骑
				stSetNotZuoqiUserCmd cmd1;
				CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
				if (pMainRole)
					cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd1.dwOldTempID = 0;
				cmd1.zuoqiid = (page - 1) * 6 + (nID / 10) - 1;
				SEND_USER_CMD(cmd1);

				stGetShizhuangPropertyUserCmd cmd2;
				if (pMainRole)
					cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd2.dwOldTempID = 0;
				SEND_USER_CMD(cmd2);
				return false;
			}
			if (GetCheckBox(15)->GetChecked() && nID == 15)
			{
				if (Zuoqi[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此坐骑无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetZuoqiUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.zuoqiid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(25)->GetChecked() && nID == 25)
			{
				if (Zuoqi[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此坐骑无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetZuoqiUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.zuoqiid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(35)->GetChecked() && nID == 35)
			{
				if (Zuoqi[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此坐骑无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetZuoqiUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.zuoqiid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(45)->GetChecked() && nID == 45)
			{
				if (Zuoqi[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此坐骑无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetZuoqiUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.zuoqiid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(55)->GetChecked() && nID == 55)
			{
				if (Zuoqi[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此坐骑无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetZuoqiUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.zuoqiid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(65)->SetChecked(false);
			}
			if (GetCheckBox(65)->GetChecked() && nID == 65)
			{
				if (Zuoqi[(page - 1) * 6 + (nID / 10) - 1].state != 1)
				{
					GameMessageBox("您未激活此坐骑无法穿戴！");
					GetCheckBox(nID)->SetChecked(false);
					return false;
				}
				else
				{
					// 请求佩戴称号
					stSetZuoqiUserCmd cmd1;
					CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;
					cmd1.zuoqiid = (page - 1) * 6 + (nID / 10) - 1;
					SEND_USER_CMD(cmd1);

					stGetShizhuangPropertyUserCmd cmd2;
					if (pMainRole)
						cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd2.dwOldTempID = 0;
					SEND_USER_CMD(cmd2);
				}
				GetCheckBox(15)->SetChecked(false);
				GetCheckBox(25)->SetChecked(false);
				GetCheckBox(35)->SetChecked(false);
				GetCheckBox(45)->SetChecked(false);
				GetCheckBox(55)->SetChecked(false);
			}
		}
	}

	if (nEvent == EVENT_BUTTON_CLICKED)
	{
		switch (nID)
		{
		case 1002:
		{
			if (GetGameGuiManager()->m_guiChenghao && ((CGuiDialog *)GetGameGuiManager()->m_guiChenghao)->IsVisible())
			{
				((CGuiDialog *)GetGameGuiManager()->m_guiChenghao)->Close();
			}
			else
			{
				GetGameGuiManager()->AddChenghao();
			}
		}
		break;
		case 1001: // 时装
		{
			page = 1;
			type = 0;
			shuaxin();
		}
		break;
		case 1003: // 披风
		{
			page = 1;
			type = 1;
			shuaxin();
		}
		break;
		case 1004: // 翅膀
		{
			page = 1;
			type = 2;
			shuaxin();
		}
		break;
		case 1005: // 坐骑
		{
			page = 1;
			type = 3;
			shuaxin();
		}
		break;
		case 1:
		{
			// Close();
			SetVisible(false);
		}
		break;
		case 3: // 上一页
		{
			if (page <= 1)
			{
				page = 1;
			}
			else
			{
				page -= 1;
			}

			shuaxin();
		}
		break;
		case 4: // 下一页
		{
			if (type == 0)
			{
				if (page >= MAX_PAGE)
				{
					page = MAX_PAGE;
				}
				else
				{
					page += 1;
				}
			}
			else if (type == 1)
			{
				if (page >= MAX_PAGE2)
				{
					page = MAX_PAGE2;
				}
				else
				{
					page += 1;
				}
			}
			else if (type == 2)
			{
				if (page >= MAX_PAGE3)
				{
					page = MAX_PAGE3;
				}
				else
				{
					page += 1;
				}
			}
			else if (type == 3)
			{
				if (page >= MAX_PAGE4)
				{
					page = MAX_PAGE4;
				}
				else
				{
					page += 1;
				}
			}

			shuaxin();
		}
		break;
		case 17: // 进阶
		case 27: // 进阶
		case 37: // 进阶
		case 47: // 进阶
		case 57: // 进阶
		case 67: // 进阶
		{
			if (type == 0)
			{
				CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
				stSetShizhuangJihuoUserCmd cmd1;
				if (pMainRole)
					cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd1.dwOldTempID = 0;
				cmd1.shizhuangid = (page - 1) * 6 + (nID / 10) - 1;
				SEND_USER_CMD(cmd1);

				stGetShizhuangPropertyUserCmd cmd2;
				if (pMainRole)
					cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd2.dwOldTempID = 0;
				SEND_USER_CMD(cmd2);
			}
			else if (type == 1) // 披风
			{
				CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
				stSetPifengJihuoUserCmd cmd1;
				if (pMainRole)
					cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd1.dwOldTempID = 0;
				cmd1.pifengid = (page - 1) * 6 + (nID / 10) - 1;
				SEND_USER_CMD(cmd1);

				stGetShizhuangPropertyUserCmd cmd2;
				if (pMainRole)
					cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd2.dwOldTempID = 0;
				SEND_USER_CMD(cmd2);
			}
			else if (type == 2) // 翅膀
			{
				CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
				stSetChibangJihuoUserCmd cmd1;
				if (pMainRole)
					cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd1.dwOldTempID = 0;
				cmd1.chibangid = (page - 1) * 6 + (nID / 10) - 1;
				SEND_USER_CMD(cmd1);

				stGetShizhuangPropertyUserCmd cmd2;
				if (pMainRole)
					cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd2.dwOldTempID = 0;
				SEND_USER_CMD(cmd2);
			}
			else if (type == 3) // 坐骑
			{
				CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
				stSetZuoqiJihuoUserCmd cmd1;
				if (pMainRole)
					cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd1.dwOldTempID = 0;
				cmd1.zuoqiid = (page - 1) * 6 + (nID / 10) - 1;
				SEND_USER_CMD(cmd1);

				stGetShizhuangPropertyUserCmd cmd2;
				if (pMainRole)
					cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd2.dwOldTempID = 0;
				SEND_USER_CMD(cmd2);
			}
		}
		break;

			break;
		}

		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent, nID, pControl);

	FUNCTION_END;
}

HRESULT CGuiMagicBoxDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	POINT pt;
	POINT pt2 = GetLocation();
	if (type == 0)
	{
		for (int i = 0; i < 6; i++)
		{
			if (i < 3)
			{
				pt.x = -150 + 205 * i + pt2.x;
				pt.y = 68 + pt2.y;
			}
			else
			{
				pt.x = -150 + 205 * (i - 3) + pt2.x;
				pt.y = 248 + pt2.y;
			}
			m_AniBody[i].Render(pt.x, pt.y);
			// todo 披风这里缩小
		}
	}
	else if (type == 1)
	{
		for (int i = 0; i < 6; i++)
		{
			if (i < 3)
			{
				pt.x = 20 + 205 * i + pt2.x;
				pt.y = 170 + pt2.y;
			}
			else
			{
				pt.x = 20 + 205 * (i - 3) + pt2.x;
				pt.y = 350 + pt2.y;
			}
			stPointF scale(0.4f, 0.4f);
			m_AniBody[i].Render(pt.x, pt.y, NULL, &scale);
			// todo 披风这里缩小
		}
	}
	else if (type == 2)
	{
		for (int i = 0; i < 6; i++)
		{
			if (i < 3)
			{
				pt.x = -5 + 205 * i + pt2.x;
				pt.y = 180 + pt2.y;
			}
			else
			{
				pt.x = -5 + 205 * (i - 3) + pt2.x;
				pt.y = 360 + pt2.y;
			}
			stPointF scale(0.5f, 0.5f);
			m_AniBody[i].Render(pt.x, pt.y, NULL, &scale);
		}
	}
	else if (type == 3)
	{
		for (int i = 0; i < 6; i++)
		{

			int w = m_AniBody[i].GetWidth();
			int h = m_AniBody[i].GetHeight();
			if (i < 3)
			{
				
				if(w ==1024 && h==1024)
				{
					pt.x = -100 + 205 * i + pt2.x;
					pt.y = 0 + pt2.y;
				}
				else{
					pt.x = -15 + 205 * i + pt2.x;
					pt.y = 180 + pt2.y;
				}
				
			}
			else
			{
				if(w ==1024 && h==1024)
				{
					pt.x = -100 + 205 * (i - 3) + pt2.x;
					pt.y = 210 + pt2.y;
				}
				else{
					pt.x = -15 + 205 * (i - 3) + pt2.x;
					pt.y = 360 + pt2.y;
				}
			}
			stPointF scale(0.5f, 0.5f);
			m_AniBody[i].Render(pt.x, pt.y, NULL, &scale);
		}
	}

	return hr;

	FUNCTION_END;
}

void CGuiMagicBoxDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiMagicBox = NULL;

	FUNCTION_END;
}

void CGuiMagicBoxDlg::shuaxin(void)
{
	shizhuangshu = 0;
	for (int i = 0; i < 300; i++)
	{
		if (Shizhuang[i].state == 1)
		{
			shizhuangshu++;
		}
	}
	pifengshu = 0;
	for (int i = 0; i < 100; i++)
	{
		if (Pifeng[i].state == 1)
		{
			pifengshu++;
		}
	}

	chibangshu = 0;
	for (int i = 0; i < 100; i++)
	{
		if (Chibang[i].state == 1)
		{
			chibangshu++;
		}
	}
	zuoqishu = 0;
	for (int i = 0; i < 100; i++)
	{
		if (Zuoqi[i].state == 1)
		{
			zuoqishu++;
		}
	}
	if (type == 0) // 时装
	{
		GetCheckBox(15)->SetChecked(false);
		GetCheckBox(25)->SetChecked(false);
		GetCheckBox(35)->SetChecked(false);
		GetCheckBox(45)->SetChecked(false);
		GetCheckBox(55)->SetChecked(false);
		GetCheckBox(65)->SetChecked(false);

		char msg[256];
		sprintf(msg, "%d/%d", page, MAX_PAGE);
		GetStatic(6)->SetText(msg);
		
		GetProcess(9)->SetRange(0, mohemaxexp);
		GetProcess(9)->SetPos(moheexp);

		sprintf(msg, "等级：%d级", mohelevel);
		GetStatic(7)->SetText(msg);

		sprintf(msg, "%d/%d", moheexp, mohemaxexp);
		GetStatic(112)->SetText(msg);

		
		sprintf(msg, "当前拥有：[%d] 套时装", shizhuangshu);
		GetStatic(111)->SetText(msg);
		stResourceLocation rlAniBody;
		rlAniBody.SetFileName("data\\body5.gl");
		POINT pt;
		for (int i = 0; i < 6; i++)
		{
			if (i < 3)
			{
				pt.x = 10 + 205 * i;
				pt.y = 200;
			}
			else
			{
				pt.x = 10 + 205 * (i - 3);
				pt.y = 380;
			}

			rlAniBody.group = Shizhuang[(page - 1) * 6 + i].bodyNum;
			rlAniBody.frame = 0;
			m_AniBody[i].Create(&rlAniBody, 16, 19);
			m_AniBody[i].SetLoopPlay(true);
			m_AniBody[i].SetSpeed(200);

			if (Shizhuang[(page - 1) * 6 + i].state == 1)
			{
				m_AniBody[i].SetColor(COLOR_ARGB(255, 255, 255, 255));
			}
			else
			{
				m_AniBody[i].SetColor(COLOR_ARGB(50, 255, 255, 255));
			}

			GetStatic((i + 1) * 10 + 3)->SetText(Shizhuang[(page - 1) * 6 + i].name);
			if (Shizhuang[(page - 1) * 6 + i].level >= 1)
			{
				GetImage((i + 1) * 10 + 6)->SetImage(stResourceLocation("data\\interfaces6.gl", 130, Shizhuang[(page - 1) * 6 + i].level));
			}
			else
			{
				GetImage((i + 1) * 10 + 6)->SetImage(stResourceLocation("data\\interfaces6.gl", 130, 9999));
			}

			if (shizhuang_select == (page - 1) * 6 + i)
			{
				GetCheckBox(((i + 1) * 10) + 5)->SetChecked(true);
			}
		}
	}
	else if (type == 1) // 披风
	{
		GetCheckBox(15)->SetChecked(false);
		GetCheckBox(25)->SetChecked(false);
		GetCheckBox(35)->SetChecked(false);
		GetCheckBox(45)->SetChecked(false);
		GetCheckBox(55)->SetChecked(false);
		GetCheckBox(65)->SetChecked(false);

		char msg[256];
		sprintf(msg, "%d/%d", page, MAX_PAGE2);
		GetStatic(6)->SetText(msg);

		GetProcess(9)->SetRange(0, mohemaxexp);
		GetProcess(9)->SetPos(moheexp);

		sprintf(msg, "等级：%d级", mohelevel);
		GetStatic(7)->SetText(msg);

		sprintf(msg, "%d/%d", moheexp, mohemaxexp);
		GetStatic(112)->SetText(msg);

		pifengshu = 0;
		for (int i = 0; i < 100; i++)
		{
			if (Pifeng[i].state == 1)
			{
				pifengshu++;
			}
		}
		sprintf(msg, "当前拥有：[%d] 套披风", pifengshu);
		GetStatic(111)->SetText(msg);
		stResourceLocation rlAniBody;
		rlAniBody.SetFileName("data\\items.gl");
		POINT pt;
		for (int i = 0; i < 6; i++)
		{
			if (i < 3)
			{
				pt.x = 10 + 205 * i;
				pt.y = 200;
			}
			else
			{
				pt.x = 10 + 205 * (i - 3);
				pt.y = 380;
			}

			rlAniBody.group = 3;
			rlAniBody.frame = Pifeng[(page - 1) * 6 + i].itemNum;
			m_AniBody[i].Create(&rlAniBody);
			m_AniBody[i].SetLoopPlay(true);
			m_AniBody[i].SetSpeed(200);

			if (Pifeng[(page - 1) * 6 + i].state == 1)
			{
				m_AniBody[i].SetColor(COLOR_ARGB(255, 255, 255, 255));
			}
			else
			{
				m_AniBody[i].SetColor(COLOR_ARGB(50, 255, 255, 255));
			}

			GetStatic((i + 1) * 10 + 3)->SetText(Pifeng[(page - 1) * 6 + i].name);
			if (Pifeng[(page - 1) * 6 + i].level >= 1)
			{
				GetImage((i + 1) * 10 + 6)->SetImage(stResourceLocation("data\\interfaces6.gl", 130, Pifeng[(page - 1) * 6 + i].level));
			}
			else
			{
				GetImage((i + 1) * 10 + 6)->SetImage(stResourceLocation("data\\interfaces6.gl", 130, 9999));
			}

			if (pifeng_select == (page - 1) * 6 + i)
			{
				GetCheckBox(((i + 1) * 10) + 5)->SetChecked(true);
			}
		}
	}
	else if (type == 2) // 翅膀
	{
		GetCheckBox(15)->SetChecked(false);
		GetCheckBox(25)->SetChecked(false);
		GetCheckBox(35)->SetChecked(false);
		GetCheckBox(45)->SetChecked(false);
		GetCheckBox(55)->SetChecked(false);
		GetCheckBox(65)->SetChecked(false);

		char msg[256];
		sprintf(msg, "%d/%d", page, MAX_PAGE3);
		GetStatic(6)->SetText(msg);

		GetProcess(9)->SetRange(0, mohemaxexp);
		GetProcess(9)->SetPos(moheexp);

		sprintf(msg, "等级：%d级", mohelevel);
		GetStatic(7)->SetText(msg);

		sprintf(msg, "%d/%d", moheexp, mohemaxexp);
		GetStatic(112)->SetText(msg);

		chibangshu = 0;
		for (int i = 0; i < 100; i++)
		{
			if (Chibang[i].state == 1)
			{
				chibangshu++;
			}
		}
		sprintf(msg, "当前拥有：[%d] 套翅膀", chibangshu);
		GetStatic(111)->SetText(msg);
		stResourceLocation rlAniBody;
		rlAniBody.SetFileName("data\\items.gl");
		POINT pt;
		for (int i = 0; i < 6; i++)
		{
			if (i < 3)
			{
				pt.x = 10 + 205 * i;
				pt.y = 200;
			}
			else
			{
				pt.x = 10 + 205 * (i - 3);
				pt.y = 380;
			}

			rlAniBody.group = 3;
			rlAniBody.frame = Chibang[(page - 1) * 6 + i].itemNum;
			m_AniBody[i].Create(&rlAniBody);
			m_AniBody[i].SetLoopPlay(true);
			m_AniBody[i].SetSpeed(200);

			if (Chibang[(page - 1) * 6 + i].state == 1)
			{
				m_AniBody[i].SetColor(COLOR_ARGB(255, 255, 255, 255));
			}
			else
			{
				m_AniBody[i].SetColor(COLOR_ARGB(50, 255, 255, 255));
			}

			GetStatic((i + 1) * 10 + 3)->SetText(Chibang[(page - 1) * 6 + i].name);
			if (Chibang[(page - 1) * 6 + i].level >= 1)
			{
				GetImage((i + 1) * 10 + 6)->SetImage(stResourceLocation("data\\interfaces6.gl", 130, Chibang[(page - 1) * 6 + i].level));
			}
			else
			{
				GetImage((i + 1) * 10 + 6)->SetImage(stResourceLocation("data\\interfaces6.gl", 130, 9999));
			}

			if (chibang_select == (page - 1) * 6 + i)
			{
				GetCheckBox(((i + 1) * 10) + 5)->SetChecked(true);
			}
		}
	}
	else if (type == 3) // 坐骑
	{
		GetCheckBox(15)->SetChecked(false);
		GetCheckBox(25)->SetChecked(false);
		GetCheckBox(35)->SetChecked(false);
		GetCheckBox(45)->SetChecked(false);
		GetCheckBox(55)->SetChecked(false);
		GetCheckBox(65)->SetChecked(false);

		char msg[256];
		sprintf(msg, "%d/%d", page, MAX_PAGE4);
		GetStatic(6)->SetText(msg);

		GetProcess(9)->SetRange(0, mohemaxexp);
		GetProcess(9)->SetPos(moheexp);

		sprintf(msg, "等级：%d级", mohelevel);
		GetStatic(7)->SetText(msg);

		sprintf(msg, "%d/%d", moheexp, mohemaxexp);
		GetStatic(112)->SetText(msg);

		zuoqishu = 0;
		for (int i = 0; i < 100; i++)
		{
			if (Zuoqi[i].state == 1)
			{
				zuoqishu++;
			}
		}
		sprintf(msg, "当前拥有：[%d] 套坐骑", zuoqishu);
		GetStatic(111)->SetText(msg);
		stResourceLocation rlAniBody;
		rlAniBody.SetFileName("data\\npc11.gl");
		POINT pt;
		for (int i = 0; i < 6; i++)
		{
			if (i < 3)
			{
				pt.x = 10 + 205 * i;
				pt.y = 200;
			}
			else
			{
				pt.x = 10 + 205 * (i - 3);
				pt.y = 380;
			}

			rlAniBody.group = Zuoqi[(page - 1) * 6 + i].npcNum;
			rlAniBody.frame = 14;
			m_AniBody[i].Create(&rlAniBody, 10, 14);
			m_AniBody[i].SetLoopPlay(true);
			m_AniBody[i].SetSpeed(200);

			if (Zuoqi[(page - 1) * 6 + i].state == 1)
			{
				m_AniBody[i].SetColor(COLOR_ARGB(255, 255, 255, 255));
			}
			else
			{
				m_AniBody[i].SetColor(COLOR_ARGB(50, 255, 255, 255));
			}

			GetStatic((i + 1) * 10 + 3)->SetText(Zuoqi[(page - 1) * 6 + i].name);
			if (Zuoqi[(page - 1) * 6 + i].level >= 1)
			{
				GetImage((i + 1) * 10 + 6)->SetImage(stResourceLocation("data\\interfaces6.gl", 130, Zuoqi[(page - 1) * 6 + i].level));
			}
			else
			{
				GetImage((i + 1) * 10 + 6)->SetImage(stResourceLocation("data\\interfaces6.gl", 130, 9999));
			}

			if (zuoqi_select == (page - 1) * 6 + i)
			{
				GetCheckBox(((i + 1) * 10) + 5)->SetChecked(true);
			}
		}
	}
}
