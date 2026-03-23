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
#include "./GuiTouxian.h"

///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiTouxianDlg::CGuiTouxianDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiTouxianDlg::~CGuiTouxianDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuiTouxianDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();

	leveluptext = GetStatic(27);
	leveluptext->SetText(" 当前提升头衔需要头衔令");
	levelnum = GetProcess(30);

	level = 0;
	exp =0;
	num1=0;
	num2=0;
	num3=0;
	num4=0;
	num5=0;
	num6=0;	
	

	image=GetImage(51);
	image->SetImage(stResourceLocation("data\\interfaces2.gl",65,101));
	
	m_pTableMaterial1 = this->GetTable(21);
	m_pTableMaterial1->m_iTableType = OBJECTCELLTYPE_FUJIAN;
	m_pTableMaterial1->m_iTableID = 0;
	m_pTableMaterial1->m_EquipPosition = stPointI(0,6);

	n1=GetStatic(106);
	n2=GetStatic(107);
	n3=GetStatic(108);

	n4=GetStatic(206);
	n5=GetStatic(207);
	n6=GetStatic(208);

	FUNCTION_END;
}


CRoleItem* CGuiTouxianDlg::GetMaterialItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableMaterial1->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}

bool CGuiTouxianDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		
		switch(nID)
		{

			case 1://确定
			{
				SetVisible(false);

			}	
			break;
			case 888:
			{
				CRoleItem* pRoleItem = this->GetMaterialItem();
				if(!pRoleItem)
				{
					char msg[256];
					sprintf(msg,"请放入头衔令进行升级！");
					leveluptext->SetText(msg);
					return false;
				}

				if(pRoleItem->GetObjectID()!=120025)
				{
					char msg[256];
					sprintf(msg,"请放入头衔令进行升级！");
					leveluptext->SetText(msg);
					return false;
				}

				//开始请求
				stFujianTouxianLevelup cmd; //sky 请求指令
				cmd.itemUID = pRoleItem->GetObject()->qwThisID;
				SEND_USER_CMD(cmd);

			}
			break;
		
		}
		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiTouxianDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiTouxian = NULL;

	FUNCTION_END;
}


void CGuiTouxianDlg::SHUAXIN(void)
{
	char text[32];
	sprintf(text,"%d",num1);
	n1->SetText(text);
	sprintf(text,"%d",num2);
	n2->SetText(text);
	sprintf(text,"%d",num3);
	n3->SetText(text);
	sprintf(text,"%d",num4);
	n4->SetText(text);
	sprintf(text,"%d",num5);
	n5->SetText(text);
	sprintf(text,"%d",num6);
	n6->SetText(text);
	levelnum->SetPos(exp);

	image->SetImage(stResourceLocation("data\\interfaces2.gl",65,100+level));

	char text2[32];
	sprintf(text2,"头衔等级:%d  所需经验:%d/10000",level,exp);
	GetStatic(300)->SetText(text2);
}