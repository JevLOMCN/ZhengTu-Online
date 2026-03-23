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
#include "./Guigongxun.h"

///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuigongxunDlg::CGuigongxunDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuigongxunDlg::~CGuigongxunDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuigongxunDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();

	text1=GetStatic(11);
	text2=GetStatic(15);
	m_pTableMaterial1 = this->GetTable(100);
	m_pTableMaterial1->m_iTableType = OBJECTCELLTYPE_HANDBOOK;
	m_pTableMaterial1->m_iTableID = 0;
	m_pTableMaterial1->m_EquipPosition = stPointI(0,2);
	num=-1;
	FUNCTION_END;
}


CRoleItem* CGuigongxunDlg::GetMaterialItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableMaterial1->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}

bool CGuigongxunDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
			case 1://确定
			{
				if(jingcai[num].object_num!=0)
				{
					char msg[256];
					sprintf(msg,"你已经再此下了赌注");
					GetGameGuiManager()->AddClientSystemMessage(msg);
					return false;
				}
				CRoleItem* pRoleItem = this->GetMaterialItem();
				if(!pRoleItem)
				{
					char msg[256];
					sprintf(msg,"请放入新年卷轴进行下注！");
					GetGameGuiManager()->AddClientSystemMessage(msg);
					return false;
				}
				
				if(pRoleItem->GetObjectID()!=120048)
				{
					char msg[256];
					sprintf(msg,"请放入新年卷轴进行下注！");
					GetGameGuiManager()->AddClientSystemMessage(msg);
					return false;
		
				}

				//请求下注
				stJingcaiXiazhuCmd cmd; //sky 请求指令
				cmd.index=num;
				cmd.itemUID = pRoleItem->GetObject()->qwThisID;
				SEND_USER_CMD(cmd);

				stJingcaiInfoCmd cmds; //sky 请求指令			
				SEND_USER_CMD(cmds);

			}	
			break;
			case 2://取消
			{
				SetVisible(false);
			}
			break;
			case 3://关闭
			{
				SetVisible(false);
			}
			break;
		}
		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuigongxunDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guigongxun = NULL;

	FUNCTION_END;
}

void CGuigongxunDlg::shuaxin(const char* t1,const char* t2)
{
	FUNCTION_BEGIN;

	text1->SetText(t2);

	num=atoi(t1);

	stJingcaiInfoCmd cmd; //sky 请求指令			
	SEND_USER_CMD(cmd);
	
	FUNCTION_END;
}

void CGuigongxunDlg::update(void)
{
	FUNCTION_BEGIN;

	if(jingcai[num].index!=num)
	{
		text2->SetText("");
		return;
	}
	else{
		ObjectBase_t* pBody = GetObjectBase(jingcai[num].object_id);
		if(pBody)
		{
			char t[64];
			sprintf(t,"%s 数量:%d",pBody->strName,jingcai[num].object_num);
			text2->SetText(t);
		}
		else{
			text2->SetText("");
		}
		
	}
	FUNCTION_END;
}