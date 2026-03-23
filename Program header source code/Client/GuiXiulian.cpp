/**
*\file		GuiTop.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    经脉对话框
*
* 
*
*/
#include "public.h"
#include "../gui/include/GuiManager.h"	
#include "./GameGuiManager.h"
#include "./Game.h"
#include "./table.h"
#include "./RoleItem.h"
#include "./GuiXiulian.h"
#include <sys/timeb.h>
#include "./GameScene.h"
#include "Maincharacter.h"
///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiXiulianDlg::CGuiXiulianDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiXiulianDlg::~CGuiXiulianDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}
void CGuiXiulianDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();

	



	FUNCTION_END;
}


bool CGuiXiulianDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
			case 1:
			{
				SetVisible(false);
			}	
			break;
			case 370:
			case 371:
			case 372:
			case 373:
			case 374:
			{
				stXiulianStartCmd cmd; //sky 请求指令
				cmd.type=(nID%370)+1; 
				SEND_USER_CMD(cmd);
			}
			break;
			case 999:
			{
				GetGameGuiManager()->AddMessageBox("你确定要重置能力修炼吗？",eAction_Xiulian,NULL, "确定|1|Enter|取消|2|Esc");
			}
			break;
		}		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiXiulianDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiXiulian = NULL;

	FUNCTION_END;
}
void CGuiXiulianDlg::shuaxin(void)
{
	char pszMoney[64] = {0};
	_snprintf(pszMoney,64,"%s",Money2String(GetScene()->GetMainCharacter()->GetMoney()));
	pszMoney[63] = 0;
	GetStatic(151)->SetText(pszMoney);

	char msg[256];
	sprintf(msg,"%d",xiulianexp);
	GetStatic(153)->SetText(msg);

	sprintf(msg,"Lv.%d",level1);
	GetStatic(340)->SetText(msg);
	sprintf(msg,"Lv.%d",level2);
	GetStatic(341)->SetText(msg);
	sprintf(msg,"Lv.%d",level3);
	GetStatic(342)->SetText(msg);
	sprintf(msg,"Lv.%d",level4);
	GetStatic(343)->SetText(msg);
	sprintf(msg,"Lv.%d",level5);
	GetStatic(344)->SetText(msg);

	sprintf(msg,"需要扣除：     %d锭",money1/10000);
	GetStatic(400)->SetText(msg);
	sprintf(msg,"需要扣除：     %d锭",money2/10000);
	GetStatic(401)->SetText(msg);
	sprintf(msg,"需要扣除：     %d锭",money3/10000);
	GetStatic(402)->SetText(msg);
	sprintf(msg,"需要扣除：     %d锭",money4/10000);
	GetStatic(403)->SetText(msg);
	sprintf(msg,"需要扣除：     %d锭",money5/10000);
	GetStatic(404)->SetText(msg);

	sprintf(msg,"%d",exp1);
	GetStatic(410)->SetText(msg);
	sprintf(msg,"%d",exp2);
	GetStatic(411)->SetText(msg);
	sprintf(msg,"%d",exp3);
	GetStatic(412)->SetText(msg);
	sprintf(msg,"%d",exp4);
	GetStatic(413)->SetText(msg);
	sprintf(msg,"%d",exp5);
	GetStatic(414)->SetText(msg);

	sprintf(msg,"+%d",num1);
	GetStatic(130)->SetText(msg);
	sprintf(msg,"+%d",num2);
	GetStatic(131)->SetText(msg);
	sprintf(msg,"+%d",num3);
	GetStatic(132)->SetText(msg);
	sprintf(msg,"+%d",num4);
	GetStatic(133)->SetText(msg);
	sprintf(msg,"+%d",num5);
	GetStatic(134)->SetText(msg);
	
}
