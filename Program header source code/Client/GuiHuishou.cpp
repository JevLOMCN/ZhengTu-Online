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
#include "./GuiHuishou.h"
#include <sys/timeb.h>
#include "./GameScene.h"
#include "Maincharacter.h"
///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiHuishouDlg::CGuiHuishouDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;
	m_pTableMaterial1 = NULL;

	FUNCTION_END;
}

CGuiHuishouDlg::~CGuiHuishouDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}
void CGuiHuishouDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();

	m_pTableMaterial1 = this->GetTable(80);
	m_pTableMaterial1->m_iTableType = OBJECTCELLTYPE_FUJIAN;
	m_pTableMaterial1->m_iTableID = 0;
	m_pTableMaterial1->m_EquipPosition = stPointI(0,10);


	GetCheckBox(11)->SetVisible(false);
	GetCheckBox(12)->SetVisible(false);
	GetCheckBox(13)->SetVisible(false);

	GetStatic(21)->SetVisible(false);
	GetStatic(31)->SetVisible(false);
	GetStatic(22)->SetVisible(false);
	GetStatic(32)->SetVisible(false);
	
	FUNCTION_END;
}

CRoleItem* CGuiHuishouDlg::GetMaterialItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableMaterial1->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}


HRESULT CGuiHuishouDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	CRoleItem* pRoleItem = this->GetMaterialItem();
	if(!pRoleItem)
	{
		char msg[256];
		sprintf(msg,"请放入道具");
		GetStatic(33)->SetText(msg);	
		GetStatic(34)->SetText(msg);	
		GetStatic(33)->SetTextColor(COLOR_ARGB(255,255,0,0));
		GetStatic(34)->SetTextColor(COLOR_ARGB(255,255,0,0));
		GetStatic(23)->SetTextColor(COLOR_ARGB(255,255,0,0));
		GetStatic(24)->SetTextColor(COLOR_ARGB(255,255,0,0));		
	}
	else{
		char msg[256];
		for(int i=0;i<100;i++)
		{
			
			if(huishou[i].objectID == pRoleItem->GetObjectID())
			{
				if(huishou[i].type==1)
				{
					sprintf(msg,"%d锭%d两 银子",huishou[i].price/10000,(huishou[i].price%10000)/100);
					GetStatic(33)->SetText(msg);	
					sprintf(msg,"%d锭%d两 银子",(huishou[i].price*pRoleItem->GetObject()->dwNum)/10000,((huishou[i].price*pRoleItem->GetObject()->dwNum)%10000)/100);
					GetStatic(34)->SetText(msg);
					GetStatic(33)->SetTextColor(COLOR_ARGB(255,0,255,0));
					GetStatic(34)->SetTextColor(COLOR_ARGB(255,0,255,0));
					GetStatic(23)->SetTextColor(COLOR_ARGB(255,0,255,0));
					GetStatic(24)->SetTextColor(COLOR_ARGB(255,0,255,0));
				}
				else if(huishou[i].type==2)
				{
					sprintf(msg,"%d锭%d两 金子",huishou[i].price/10000,(huishou[i].price%10000)/100);
					GetStatic(33)->SetText(msg);	
					sprintf(msg,"%d锭%d两 金子",(huishou[i].price*pRoleItem->GetObject()->dwNum)/10000,((huishou[i].price*pRoleItem->GetObject()->dwNum)%10000)/100);
					GetStatic(34)->SetText(msg);
					GetStatic(33)->SetTextColor(COLOR_ARGB(255,0,255,0));
					GetStatic(34)->SetTextColor(COLOR_ARGB(255,0,255,0));
					GetStatic(23)->SetTextColor(COLOR_ARGB(255,0,255,0));
					GetStatic(24)->SetTextColor(COLOR_ARGB(255,0,255,0));
				}
				else if(huishou[i].type==3)
				{
					sprintf(msg,"%d锭%d两 工资",huishou[i].price/10000,(huishou[i].price%10000)/100);
					GetStatic(33)->SetText(msg);	
					sprintf(msg,"%d锭%d两 工资",(huishou[i].price*pRoleItem->GetObject()->dwNum)/10000,((huishou[i].price*pRoleItem->GetObject()->dwNum)%10000)/100);
					GetStatic(34)->SetText(msg);
					GetStatic(33)->SetTextColor(COLOR_ARGB(255,0,255,0));
					GetStatic(34)->SetTextColor(COLOR_ARGB(255,0,255,0));
					GetStatic(23)->SetTextColor(COLOR_ARGB(255,0,255,0));
					GetStatic(24)->SetTextColor(COLOR_ARGB(255,0,255,0));
				}
				else{
					char msg[256];
					sprintf(msg,"未知类型，请联系GM");
					GetStatic(33)->SetText(msg);	
					GetStatic(34)->SetText(msg);	
					GetStatic(33)->SetTextColor(COLOR_ARGB(255,255,0,0));
					GetStatic(34)->SetTextColor(COLOR_ARGB(255,255,0,0));
					GetStatic(23)->SetTextColor(COLOR_ARGB(255,255,0,0));
					GetStatic(24)->SetTextColor(COLOR_ARGB(255,255,0,0));		
				}
				break;
					
			}
			else{
				char msg[256];
				sprintf(msg,"此道具不可回收");
				GetStatic(33)->SetText(msg);	
				GetStatic(34)->SetText(msg);	
				GetStatic(33)->SetTextColor(COLOR_ARGB(255,255,0,0));
				GetStatic(34)->SetTextColor(COLOR_ARGB(255,255,0,0));
				GetStatic(23)->SetTextColor(COLOR_ARGB(255,255,0,0));
				GetStatic(24)->SetTextColor(COLOR_ARGB(255,255,0,0));
			}
		}
		
	}
	return hr;

	FUNCTION_END;
}

bool CGuiHuishouDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
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
			case 100://提交
			{
				CRoleItem* pRoleItem = this->GetMaterialItem();
				if(!pRoleItem)
				{
					char msg[256];
					sprintf(msg,"请放入道具进行回收！");
					GetGameGuiManager()->AddClientSystemMessage(msg);
					return false;
				}
				
			
				//请求回收
				stHuishouStart cmd; //sky 请求指令
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

void CGuiHuishouDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiHuishou = NULL;

	FUNCTION_END;
}


//道具回收刷新列表
void CGuiHuishouDlg::shuaxin(void)
{
	CGuiListBox* pList = GetListBox(2);
	pList->RemoveAllItems();
	for(int i=0;i<100;i++)
	{
		if(huishou[i].objectID!=0)
		{
			pList->AddItem(huishou[i].name, 0);
			pList->SetFont(1);
		}
		// pList->SetItemColor(0, 0, D3DCOLOR_ARGB(255,230,230,250));
	}
		
}
