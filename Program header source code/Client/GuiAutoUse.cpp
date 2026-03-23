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
#include "./GuiAutoUse.h"
#include <sys/timeb.h>
#include "./GameScene.h"
#include "Maincharacter.h"
///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiAutoUseDlg::CGuiAutoUseDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;
	onuse = false;

	FUNCTION_END;
}

CGuiAutoUseDlg::~CGuiAutoUseDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}
void CGuiAutoUseDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();

	
	m_pTableMaterial1 = this->GetTable(10);
	m_pTableMaterial1->m_iTableType = OBJECTCELLTYPE_FUJIAN;
	m_pTableMaterial1->m_iTableID = 0;
	m_pTableMaterial1->m_EquipPosition = stPointI(0,13);



	FUNCTION_END;
}

HRESULT CGuiAutoUseDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	if(onuse)
	{	
		if(!canuse())
		{
			onuse = false;
			GameMessageBox( "此物品不可自动使用" );
			return hr;
		}
		CRoleItem* pRoleItem = this->GetMaterialItem();
		if(!pRoleItem)
		{
			onuse = false;
		}
		else{
			if(pRoleItem->GetObject()->dwNum <= 0)
			{
				onuse = false;
			}
			else{
				if(pRoleItem->GetObject()->dwNum-1 <= 0)
				{
					onuse = false;
				}
				stUseObjectPropertyUserCmd cmd;
				cmd.qwThisID = pRoleItem->GetThisID();
				cmd.dwNumber = 1;
				SEND_USER_CMD( cmd );
				
			}
		}
	}
	
	return hr;

	FUNCTION_END;
}


CRoleItem* CGuiAutoUseDlg::GetMaterialItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableMaterial1->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}
bool CGuiAutoUseDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
			case 1:
			{
				onuse = false;
				SetVisible(false);
			}	
			break;
			case 100:
			{
				if(onuse)
				{
					GameMessageBox( "正在进行自动使用" );
					return false;
				}
				CRoleItem* pRoleItem = this->GetMaterialItem();
				if(!pRoleItem)
				{
					GameMessageBox( "请放入道具" );
					return false;
				}
				
				bool canuse = false;
				switch (pRoleItem->GetObjectID())
				{
					case 120035:
					case 123123:
					case 120054:
					case 120056:
					case 120064:
					case 36665:
					{
						canuse=true;
					}
					break;
				}
				if(!canuse)
				{
					GameMessageBox( "此物品不可自动使用" );
					return false;
				}
				onuse = true;
			}
			break;
			case 101:
			{
				if(onuse)
				{
					onuse = false;
				}
			}
			break;
		}		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiAutoUseDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiAutoUse = NULL;

	FUNCTION_END;
}


bool CGuiAutoUseDlg::canuse(void)
{
	bool canuse = false;
	CRoleItem* pRoleItem = this->GetMaterialItem();
	if(!pRoleItem)
	{
		return false;
	}
	switch (pRoleItem->GetObjectID())
	{
		case 120035:
		case 123123:
		case 120054:
		case 120056:
		case 120064:
		case 36665:
		{
			canuse=true;
		}
		break;
	}
	return canuse;
}
