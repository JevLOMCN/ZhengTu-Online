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
#include "./GuiZhuanhuan.h"
#include <sys/timeb.h>
#include "./GameScene.h"
#include "Maincharacter.h"
///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiZhuanhuanDlg::CGuiZhuanhuanDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;
	m_pTableMaterial1 = NULL;

	FUNCTION_END;
}

CGuiZhuanhuanDlg::~CGuiZhuanhuanDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}
void CGuiZhuanhuanDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();

	m_pTableMaterial1 = this->GetTable(80);
	m_pTableMaterial1->m_iTableType = OBJECTCELLTYPE_FUJIAN;
	m_pTableMaterial1->m_iTableID = 0;
	m_pTableMaterial1->m_EquipPosition = stPointI(0,12);

	isneedtype = false;
	zhuangbei = true;
	
	
	FUNCTION_END;
}

CRoleItem* CGuiZhuanhuanDlg::GetMaterialItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableMaterial1->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}


HRESULT CGuiZhuanhuanDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	
	CRoleItem* pRoleItem = this->GetMaterialItem();
	if(!pRoleItem)
	{
		char msg[256];
		sprintf(msg,"请放入道具");
			
	}
	else{
		char msg[256];
		
	}
	return hr;

	FUNCTION_END;
}

bool CGuiZhuanhuanDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if(nEvent == EVENT_CHECKBOX_CHANGED)
	{	
		if (GetCheckBox(11)->GetChecked() && nID==11)
		{
			GetCheckBox(12)->SetChecked(false);
			GetCheckBox(13)->SetChecked(false);
			GetCheckBox(14)->SetChecked(false);
			GetCheckBox(15)->SetChecked(false);
			GetCheckBox(16)->SetChecked(false);
			GetCheckBox(17)->SetChecked(false);
			GetCheckBox(18)->SetChecked(false);
		}
		if (GetCheckBox(12)->GetChecked() && nID==12)
		{
			GetCheckBox(11)->SetChecked(false);
			GetCheckBox(13)->SetChecked(false);
			GetCheckBox(14)->SetChecked(false);
			GetCheckBox(15)->SetChecked(false);
			GetCheckBox(16)->SetChecked(false);
			GetCheckBox(17)->SetChecked(false);
			GetCheckBox(18)->SetChecked(false);
		}
		if (GetCheckBox(13)->GetChecked() && nID==13) 
		{
			GetCheckBox(11)->SetChecked(false);
			GetCheckBox(12)->SetChecked(false);
			GetCheckBox(14)->SetChecked(false);
			GetCheckBox(15)->SetChecked(false);
			GetCheckBox(16)->SetChecked(false);
			GetCheckBox(17)->SetChecked(false);
			GetCheckBox(18)->SetChecked(false);
		}
		if (GetCheckBox(14)->GetChecked() && nID==14)
		{
			GetCheckBox(11)->SetChecked(false);
			GetCheckBox(12)->SetChecked(false);
			GetCheckBox(13)->SetChecked(false);
			GetCheckBox(15)->SetChecked(false);
			GetCheckBox(16)->SetChecked(false);
			GetCheckBox(17)->SetChecked(false);
			GetCheckBox(18)->SetChecked(false);
		}
		if (GetCheckBox(15)->GetChecked() && nID==15)
		{
			GetCheckBox(11)->SetChecked(false);
			GetCheckBox(12)->SetChecked(false);
			GetCheckBox(13)->SetChecked(false);
			GetCheckBox(14)->SetChecked(false);
			GetCheckBox(16)->SetChecked(false);
			GetCheckBox(17)->SetChecked(false);
			GetCheckBox(18)->SetChecked(false);
		}
		if (GetCheckBox(16)->GetChecked() && nID==16)
		{
			GetCheckBox(11)->SetChecked(false);
			GetCheckBox(12)->SetChecked(false);
			GetCheckBox(13)->SetChecked(false);
			GetCheckBox(14)->SetChecked(false);
			GetCheckBox(15)->SetChecked(false);
			GetCheckBox(17)->SetChecked(false);
			GetCheckBox(18)->SetChecked(false);
		}
		if (GetCheckBox(17)->GetChecked() && nID==17)
		{
			GetCheckBox(11)->SetChecked(false);
			GetCheckBox(12)->SetChecked(false);
			GetCheckBox(13)->SetChecked(false);
			GetCheckBox(14)->SetChecked(false);
			GetCheckBox(15)->SetChecked(false);
			GetCheckBox(16)->SetChecked(false);
			GetCheckBox(18)->SetChecked(false);
		}
		if (GetCheckBox(18)->GetChecked() && nID==18)
		{
			GetCheckBox(11)->SetChecked(false);
			GetCheckBox(12)->SetChecked(false);
			GetCheckBox(13)->SetChecked(false);
			GetCheckBox(14)->SetChecked(false);
			GetCheckBox(15)->SetChecked(false);
			GetCheckBox(16)->SetChecked(false);
			GetCheckBox(17)->SetChecked(false);
		}


		
		
		//处理单选
		if(GetCheckBox(11)->GetChecked() || GetCheckBox(12)->GetChecked() || GetCheckBox(13)->GetChecked() || GetCheckBox(14)->GetChecked()|| GetCheckBox(15)->GetChecked()||GetCheckBox(16)->GetChecked())
		{
			GetCheckBox(31)->SetChecked(false);
			GetCheckBox(32)->SetChecked(false);
			zhuangbei = true;
			isneedtype = false;
		}
		else if(GetCheckBox(17)->GetChecked()||GetCheckBox(18)->GetChecked()){
			//单选
			if(!GetCheckBox(32)->GetChecked())
			{
				GetCheckBox(31)->SetChecked(true);
			}
			zhuangbei = true;
			isneedtype = true;
		
		}
		else{
			isneedtype = true;
			zhuangbei = false;
		}

		if (GetCheckBox(31)->GetChecked() && nID==31)
		{
			GetCheckBox(32)->SetChecked(false);
		}
		if (GetCheckBox(32)->GetChecked() && nID==32)
		{
			GetCheckBox(31)->SetChecked(false);
		}
	}
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
					sprintf(msg,"请放入道具进行转换！");
					GetGameGuiManager()->AddClientSystemMessage(msg);
					return false;
				}
				int zb = -1;
				int type = -1; //0物理 1魔法
				if(zhuangbei && isneedtype )//上下勾选
				{
					if(GetCheckBox(31)->GetChecked())
					{
						type = 1;
					}
					if(GetCheckBox(32)->GetChecked())
					{
						type = 2;
					}

					if(GetCheckBox(17)->GetChecked())
					{
						zb = 7;
					}
					if(GetCheckBox(18)->GetChecked())
					{
						zb = 8;
					}

				
				}
				else if(zhuangbei && !isneedtype )//上勾选
				{
					if(GetCheckBox(11)->GetChecked())
					{
						zb = 1;
					}
					if(GetCheckBox(12)->GetChecked())
					{
						zb = 2;
					}
					if(GetCheckBox(13)->GetChecked())
					{
						zb = 3;
					}
					if(GetCheckBox(14)->GetChecked())
					{
						zb = 4;
					}
					if(GetCheckBox(15)->GetChecked())
					{
						zb = 5;
					}
					if(GetCheckBox(16)->GetChecked())
					{
						zb = 6;
					}
					
					type = -1;
				}
				else if(!zhuangbei && isneedtype )//下勾选
				{
					if(GetCheckBox(31)->GetChecked())
					{
						type = 1;
					}
					if(GetCheckBox(32)->GetChecked())
					{
						type = 2;
					}

				}

				if(zhuangbei && isneedtype )//上下勾选
				{
					if(pRoleItem->GetObjectBase()->dwType==137 || pRoleItem->GetObjectBase()->dwType==155 ||pRoleItem->GetObjectBase()->dwType==104 || pRoleItem->GetObjectBase()->dwType==108|| pRoleItem->GetObjectBase()->dwType==109|| pRoleItem->GetObjectBase()->dwType==110|| pRoleItem->GetObjectBase()->dwType==111|| pRoleItem->GetObjectBase()->dwType==136)
					{
						stZhuanhuanCmd cmd; //sky 请求指令
						cmd.itemUID = pRoleItem->GetObject()->qwThisID;
						cmd.zhiye = zb;
						cmd.type = type;
						SEND_USER_CMD(cmd);	
					}
					else{
						char msg[256];
						sprintf(msg,"此道具不能用此模式转换");
						GetGameGuiManager()->AddClientSystemMessage(msg);
					}
				}
				else if(zhuangbei && !isneedtype )//上勾选
				{
					if(pRoleItem->GetObjectBase()->dwType==137 || pRoleItem->GetObjectBase()->dwType==155 ||pRoleItem->GetObjectBase()->dwType==104 || pRoleItem->GetObjectBase()->dwType==108|| pRoleItem->GetObjectBase()->dwType==109|| pRoleItem->GetObjectBase()->dwType==110|| pRoleItem->GetObjectBase()->dwType==111|| pRoleItem->GetObjectBase()->dwType==136)
					{
						stZhuanhuanCmd cmd; //sky 请求指令
						cmd.itemUID = pRoleItem->GetObject()->qwThisID;
						cmd.zhiye = zb;
						cmd.type = type;
						SEND_USER_CMD(cmd);	
					}
					else{
						char msg[256];
						sprintf(msg,"此道具不能用此模式转换");
						GetGameGuiManager()->AddClientSystemMessage(msg);
					}	
				}
				else if(!zhuangbei && isneedtype )//下勾选
				{
					if((pRoleItem->GetObjectBase()->dwType>=113 && pRoleItem->GetObjectBase()->dwType<=118) ||pRoleItem->GetObjectBase()->dwType==102||pRoleItem->GetObjectBase()->dwType==103 ||pRoleItem->GetObjectBase()->dwType==156 || pRoleItem->GetObjectBase()->dwType==138 )
					{
						stZhuanhuanCmd cmd; //sky 请求指令
						cmd.itemUID = pRoleItem->GetObject()->qwThisID;
						cmd.zhiye = zb;
						cmd.type = type;
						SEND_USER_CMD(cmd);	
					}
					else{
						char msg[256];
						sprintf(msg,"此道具不能用此模式转换");
						GetGameGuiManager()->AddClientSystemMessage(msg);
					}

				}
				//开始请求
				
		
			}
			break;
		}		
		return true;
	}
	
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiZhuanhuanDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiZhuanhuan = NULL;

	FUNCTION_END;
}

