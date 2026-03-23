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
#include "./GuiBieShu.h"
#include <sys/timeb.h>
#include "./GameScene.h"
#include "Maincharacter.h"
///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiBieShuDlg::CGuiBieShuDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;
	m_pTableMaterial1 = NULL;

	FUNCTION_END;
}

CGuiBieShuDlg::~CGuiBieShuDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}
void CGuiBieShuDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();

	
	m_pTableMaterial1 = this->GetTable(80);
	m_pTableMaterial1->m_iTableType = OBJECTCELLTYPE_FUJIAN;
	m_pTableMaterial1->m_iTableID = 0;
	m_pTableMaterial1->m_EquipPosition = stPointI(0,11);



	FUNCTION_END;
}


CRoleItem* CGuiBieShuDlg::GetMaterialItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableMaterial1->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}


HRESULT CGuiBieShuDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	CRoleItem* pRoleItem = this->GetMaterialItem();
	if(!pRoleItem)
	{
		// if(bieshu_level == 0)
		// {
		// 	GetStatic(110)->SetText("首次建造别墅需要:别墅图纸");
		// }
		// else{
		// 	GetStatic(110)->SetText("建造别墅需要:极品灵木");

		// }
	}
	else{
		
		
	}
	return hr;

	FUNCTION_END;
}
bool CGuiBieShuDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
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
			case 100:
			{
				CRoleItem* pRoleItem = this->GetMaterialItem();
				if(!pRoleItem)
				{
					GetStatic(110)->SetText("请放入道具！");
					return false;
				}
				
				if(bieshu_level == 0)
				{
					//激活别墅
					stBieShuJiHuoCmd cmd; //sky 请求指令
					cmd.itemID = pRoleItem->GetObject()->qwThisID;
					SEND_USER_CMD(cmd);	
				}
				else{
					//激活别墅
					stBieShuLevelUpCmd cmd; //sky 请求指令
					cmd.itemID = pRoleItem->GetObject()->qwThisID;
					SEND_USER_CMD(cmd);	
				}
						
			}
			break;
		}		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiBieShuDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiBieShu = NULL;

	FUNCTION_END;
}

void CGuiBieShuDlg::shuaxin(void)
{	
	char msg[256];
	if(bieshu_level == 0)
	{
		sprintf(msg, "云天别墅(未建造)", bieshu_level);
		GetStatic(202)->SetText(msg);
	}
	else{
		sprintf(msg, "云天别墅(%d级)", bieshu_level);
		GetStatic(202)->SetText(msg);
	}
	
	GetProcess(205)->SetPos(bieshu_exp);
	sprintf(msg, "%d", bieshu_gong);
	GetStatic(212)->SetText(msg);
	sprintf(msg, "%d", bieshu_fang);
	GetStatic(214)->SetText(msg);
	sprintf(msg, "%d", bieshu_hp);
	GetStatic(216)->SetText(msg);

	sprintf(msg, "      %d/10000", bieshu_exp);
	GetStatic(206)->SetText(msg);


	if(bieshu_level == 0)
	{
		GetStatic(110)->SetText("首次建造别墅需要:别墅图纸");

	}
	else{
		GetStatic(110)->SetText("提升别墅等级需要:极品灵木");

	}
	
}
