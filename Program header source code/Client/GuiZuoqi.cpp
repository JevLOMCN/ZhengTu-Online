/**
*\file		GuiTop.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    后门系统
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
#include "./GuiZuoqi.h"
#include <sys/timeb.h>
#include "./GameScene.h"
#include "Maincharacter.h"
///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiZuoqiDlg::CGuiZuoqiDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;
	m_pTableMaterial1 = NULL;
	

	FUNCTION_END;
}

CGuiZuoqiDlg::~CGuiZuoqiDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}
void CGuiZuoqiDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();

	m_pTableMaterial1 = this->GetTable(5);
	m_pTableMaterial1->m_iTableType = OBJECTCELLTYPE_FUJIAN;
	m_pTableMaterial1->m_iTableID = 0;
	m_pTableMaterial1->m_EquipPosition = stPointI(0,15);

	level = 1;
	exp = 0;
	num1 = 0;
	num2 = 0;
	num3 = 0;
	num4 = 0;
	num5 = 0;
	num6 = 0;
	num7 = 0;
	num8 = 0;
	num9 = 0;
	num10 = 0;
	num11 = 0;
	num12 = 0;
	num13 = 0;
	num14 = 0;
	num15 = 0;

	FUNCTION_END;
}


HRESULT CGuiZuoqiDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);


	stPointI ptText ;
	ptText = stPointI(395,130) + GetLocation();
	char msg[256];
	sprintf(msg, "力量：%d", num1);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );	

	ptText = stPointI(395,148) + GetLocation();
	sprintf(msg, "智力：%d", num2);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );	

	ptText = stPointI(395,166) + GetLocation();
	sprintf(msg, "敏捷：%d", num3);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );	

	ptText = stPointI(395,184) + GetLocation();
	sprintf(msg, "精神：%d", num4);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );	

	ptText = stPointI(395,202) + GetLocation();
	sprintf(msg, "体质：%d", num5);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );	

	ptText = stPointI(395,220) + GetLocation();
	sprintf(msg, "移动速度：%d", num6);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );	

	ptText = stPointI(395,238) + GetLocation();
	sprintf(msg, "增加物理攻击力：%d%%", num7);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );	

	ptText = stPointI(395,256) + GetLocation();
	sprintf(msg, "增加魔法攻击力：%d%%", num8);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );	

	ptText = stPointI(395,274) + GetLocation();
	sprintf(msg, "增加物理防御力：%d%%", num9);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );	

	ptText = stPointI(395,292) + GetLocation();
	sprintf(msg, "增加魔法防御力：%d%%", num10);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );



	ptText = stPointI(395,340) + GetLocation();
	sprintf(msg, "物理攻击：+%d", num11);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );	

	ptText = stPointI(395,358) + GetLocation();
	sprintf(msg, "魔法攻击：+%d", num12);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );	

	ptText = stPointI(395,376) + GetLocation();
	sprintf(msg, "物理防御：+%d", num13);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );	

	ptText = stPointI(395,394) + GetLocation();
	sprintf(msg, "魔法防御：+%d", num14);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );	

	ptText = stPointI(395,412) + GetLocation();
	sprintf(msg, "最大生命值：+%d", num15);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );	



	GetDevice()->SetFont(1); //字体大小
	ptText = stPointI(145,384) + GetLocation();
	sprintf(msg, "当前图鉴等级 %d 级", level);
	GetDevice()->DrawString(msg,ptText,0xff00ff00);	

	GetDevice()->SetFont(0); //字体大小
	ptText = stPointI(170,415) + GetLocation();
	sprintf(msg, "图鉴经验：%d/100", exp);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );


	ptText = stPointI(140,448) + GetLocation();
	sprintf(msg, "需要：1个战马精魄", exp);
	GetDevice()->DrawString(msg,ptText,0xff00ff00 );	
	
	return hr;

	FUNCTION_END;
}


CRoleItem* CGuiZuoqiDlg::GetMaterialItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableMaterial1->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}




bool CGuiZuoqiDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
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
			case 10:
			{
				CRoleItem* pRoleItem = this->GetMaterialItem();
				if(!pRoleItem)
				{
					char msg[256];
					sprintf(msg,"请放入战马精魄进行坐骑图鉴升级");
					GetGameGuiManager()->AddClientSystemMessage(msg);
					return false;
				}
				
				if(pRoleItem->GetObjectID()!=852)
				{
					char msg[256];
					sprintf(msg,"请放入战马精魄进行坐骑图鉴升级");
					GetGameGuiManager()->AddClientSystemMessage(msg);
					return false;
		
				}
				
				//请求升级
				stZuoqiLevelUpCmd cmd; //sky 请求指令
				cmd.itemid = pRoleItem->GetObject()->qwThisID;
				SEND_USER_CMD(cmd);
			}
			break;
			case 11:
			{
				//请求领取
				stZuoqiLingQuCmd cmd; //sky 请求指令
				SEND_USER_CMD(cmd);
			}
			break;
			

		}		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiZuoqiDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiZuoqi = NULL;

	FUNCTION_END;
}

void CGuiZuoqiDlg::shuaxin(void)
{	
	GetProcess(7)->SetPos(exp);
	if(level == 1)
	{
		GetImage(20)->SetImage(stResourceLocation("data\\interfaces2.gl",135,100));
		GetImage(21)->SetImage(stResourceLocation("data\\interfaces2.gl",135,0));
	}
	else if(level == 2)
	{
		GetImage(20)->SetImage(stResourceLocation("data\\interfaces2.gl",135,101));
		GetImage(21)->SetImage(stResourceLocation("data\\interfaces2.gl",135,1));
	}
	else if(level == 3)
	{
		GetImage(20)->SetImage(stResourceLocation("data\\interfaces2.gl",135,102));
		GetImage(21)->SetImage(stResourceLocation("data\\interfaces2.gl",135,2));
	}
	else if(level == 4)
	{
		GetImage(20)->SetImage(stResourceLocation("data\\interfaces2.gl",135,103));
		GetImage(21)->SetImage(stResourceLocation("data\\interfaces2.gl",135,3));
	}

	

}