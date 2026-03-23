/**
*\file		GuiTop.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    家族boss
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
#include "./GuiJiazuboss.h"
#include <sys/timeb.h>
#include ".\guimain.h"
#include "./GameScene.h"
#include "Maincharacter.h"
///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiJiazubossDlg::CGuiJiazubossDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;
	m_pTableMaterial1 = NULL;
	
	FUNCTION_END;
}

CGuiJiazubossDlg::~CGuiJiazubossDlg()
{
	FUNCTION_BEGIN;

	

	FUNCTION_END;
}
void CGuiJiazubossDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();
	level = 1;
	exp = 0;
	m_pTableMaterial1 = this->GetTable(14);
	m_pTableMaterial1->m_iTableType = OBJECTCELLTYPE_FUJIAN;
	m_pTableMaterial1->m_iTableID = 0;
	m_pTableMaterial1->m_EquipPosition = stPointI(0,0);


	FUNCTION_END;
}

CRoleItem* CGuiJiazubossDlg::GetMaterialItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableMaterial1->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}


bool CGuiJiazubossDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
			
			case 200:
			{
				SetVisible(false);	
			}	
			break;
			case 13: //喂养
			{
				CRoleItem* pRoleItem = this->GetMaterialItem();
				if(!pRoleItem)
				{
					char msg[256];
					sprintf(msg,"请放入道具进行喂养");
					GetGameGuiManager()->AddClientSystemMessage(msg);
					return false;
				}
				if(pRoleItem->GetObjectID()!=120036 )
				{
					char msg[256];
					sprintf(msg,"请放入家族BOSS食物进行喂养");
					GetGameGuiManager()->AddClientSystemMessage(msg);
					return false;
				}
				//请求喂养
				stJiazuBossWeiyangCmd cmd; //sky 请求指令
				cmd.itemid = pRoleItem->GetObject()->qwThisID;
				SEND_USER_CMD(cmd);		
			}
			break;
			case 16: //召唤
			{
				//请求召唤
				stJiazuBossZhaohuanCmd cmd; //sky 请求指令
				SEND_USER_CMD(cmd);	

				//请求信息
				stJiazuBossInfoCmd cmd2; //sky 请求指令
				SEND_USER_CMD(cmd2);		
			}
			break;

		}		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiJiazubossDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiJiazuboss = NULL;

	FUNCTION_END;
}


void CGuiJiazubossDlg::shuaxin(void)
{
	char msg[256];
	sprintf(msg,"%d级",level);
	GetStatic(3)->SetText(msg);
	sprintf(msg,"%d/1000",exp);
	GetStatic(5)->SetText(msg);
}
