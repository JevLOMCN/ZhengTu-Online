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
#include "./guiuserinfo.h"
#include "./Game.h"
#include "./GuiBaby.h"
#include "./RoleItem.h"

namespace
{
	const int LISTBOX_LIST = 2022;

}

CGuiBabyDlg::CGuiBabyDlg()
{
	FUNCTION_BEGIN;

	putway = false;
	m_pTableMaterial1 = NULL;
	m_bSmDialog = true;
	m_pListBoxList = NULL; // 列表

	FUNCTION_END;
}

CGuiBabyDlg::~CGuiBabyDlg()
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}

void CGuiBabyDlg::OnCreate()
{
	FUNCTION_BEGIN;

	select = 0;

	cltext = GetStatic(3002);

	pLevel = GetProcess(1004);

	pLevel2 = GetProcess(1005);

	Sll = GetStatic(1121);
	Snum1 = GetStatic(1124);

	Szl = GetStatic(1126);
	Snum2 = GetStatic(1129);

	Smj = GetStatic(1131);
	Snum3 = GetStatic(1134);

	Sjs = GetStatic(1136);
	Snum4 = GetStatic(1139);

	Stz = GetStatic(1141);
	Snum5 = GetStatic(1144);

	Sskillpoint = GetStatic(2291);

	level = GetStatic(1000);
	exps = GetStatic(1002);

	CGuiDialog::OnCreate();

	SetBabyImage(310, 182);

	pImage = GetImage(999);
	if (pImage)
	{
		pImage->SetImage(stResourceLocation("data\\interfaces.gl", 236, 200));
	}

	m_pTableMaterial1 = this->GetTable(3003);
	m_pTableMaterial1->m_iTableType = OBJECTCELLTYPE_FUJIAN;
	m_pTableMaterial1->m_iTableID = 0;
	m_pTableMaterial1->m_EquipPosition = stPointI(0, 7);

	m_pListBoxList = this->GetMLTextBox(LISTBOX_LIST);
	if (m_pListBoxList)
	{
		char szIcon[MAX_PATH];
		sprintf(szIcon,"data\\interfaces\\236\\57");
		m_pListBoxList->AppendParagraphButton(" 切换性别", D3DCOLOR_ARGB(255,255,196,0),0,szIcon,0);
		char szIcon2[MAX_PATH];
		sprintf(szIcon2,"data\\interfaces\\236\\59");
		m_pListBoxList->AppendParagraphButton(" 装备升级", D3DCOLOR_ARGB(255,255,196,0),0,szIcon2,0);
		char szIcon3[MAX_PATH];
		// sprintf(szIcon3,"data\\interfaces\\236\\58");
		// m_pListBoxList->AppendParagraphButton(" 婴儿指南", D3DCOLOR_ARGB(255,255,196,0),0,szIcon3,0);
		// char szIcon4[MAX_PATH];
		// sprintf(szIcon4,"data\\interfaces\\236\\61");
		// m_pListBoxList->AppendParagraphButton(" 神童之路", D3DCOLOR_ARGB(255,255,196,0),0,szIcon4,0);
		// char szIcon5[MAX_PATH];
		// sprintf(szIcon5,"data\\interfaces\\236\\62");
		// m_pListBoxList->AppendParagraphButton(" 孩子能力", D3DCOLOR_ARGB(255,255,196,0),0,szIcon5,0);
		// char szIcon6[MAX_PATH];
		// sprintf(szIcon6,"data\\interfaces\\236\\60");
		// m_pListBoxList->AppendParagraphButton(" 每日任务", D3DCOLOR_ARGB(255,255,196,0),0,szIcon6,0);
		// char szIcon7[MAX_PATH];
		// sprintf(szIcon7,"data\\interfaces\\236\\63");
		// m_pListBoxList->AppendParagraphButton(" 成长考验", D3DCOLOR_ARGB(255,255,196,0),0,szIcon7,0);

		m_pListBoxList->SetFont(0);

	}


	byte nTableType = OBJECTCELLTYPE_EQUIP;


	GetEquipTable(EQUIPCELLTYPE_BABYFJ1) = GetTable(4000);
	GetEquipTable(EQUIPCELLTYPE_BABYFJ1)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_BABYFJ1)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_BABYFJ1);

	GetEquipTable(EQUIPCELLTYPE_BABYFJ2) = GetTable(4001);
	GetEquipTable(EQUIPCELLTYPE_BABYFJ2)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_BABYFJ2)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_BABYFJ2);

	GetEquipTable(EQUIPCELLTYPE_BABYFJ3) = GetTable(4002);
	GetEquipTable(EQUIPCELLTYPE_BABYFJ3)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_BABYFJ3)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_BABYFJ3);

	GetEquipTable(EQUIPCELLTYPE_BABYFJ4) = GetTable(4003);
	GetEquipTable(EQUIPCELLTYPE_BABYFJ4)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_BABYFJ4)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_BABYFJ4);

	GetEquipTable(EQUIPCELLTYPE_BABYFJ5) = GetTable(4004);
	GetEquipTable(EQUIPCELLTYPE_BABYFJ5)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_BABYFJ5)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_BABYFJ5);

	GetEquipTable(EQUIPCELLTYPE_BABYFJ6) = GetTable(4005);
	GetEquipTable(EQUIPCELLTYPE_BABYFJ6)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_BABYFJ6)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_BABYFJ6);	
	FUNCTION_END;
}


//宝宝
void CGuiBabyDlg::OnPutFocusItem(int index,CRoleItem* pRoleItem)
{

		CRoleItem* pItem = GetEquipItem(EQUIPCELLTYPE_BABYFJ1);
		if(pItem)
		{

		}
		// {
		// 	if(pItem->GetObject()->retain22>0 && pItem->GetObject()->retain22<1000 )
		// 	{			
		// 		GetButton(1208)->SetBitmap(stResourceLocation("data\\interfaces3.gl",102,3),1); 		
		// 	}
		// 	else if(pItem->GetObject()->retain22>=1000 && pItem->GetObject()->retain22<2000 )
		// 	{			
		// 		GetButton(1208)->SetBitmap(stResourceLocation("data\\interfaces3.gl",102,4),1); 		
		// 	}
		// 	else if(pItem->GetObject()->retain22>=2000 && pItem->GetObject()->retain22<3000 )
		// 	{			
		// 		GetButton(1208)->SetBitmap(stResourceLocation("data\\interfaces3.gl",102,5),1); 		
		// 	}
		// 	else if(pItem->GetObject()->retain22>=3000 && pItem->GetObject()->retain22<4000 )
		// 	{			
		// 		GetButton(1208)->SetBitmap(stResourceLocation("data\\interfaces3.gl",102,6),1); 		
		// 	}
		// 	else if(pItem->GetObject()->retain22>=4000)
		// 	{			
		// 		GetButton(1208)->SetBitmap(stResourceLocation("data\\interfaces3.gl",102,7),1); 		
		// 	}
		// 	else{
		// 		GetButton(1208)->SetBitmap(stResourceLocation("data\\interfaces3.gl",102,2),1); 
		// 	}		
		// }
		// else{
		// 	GetButton(1208)->SetBitmap(stResourceLocation("data\\interfaces3.gl",102,999),1); 
		// }
		// 	char msg[256];		
		// 	sprintf(msg,"元神增加物理攻击 %u\n",yssx1);
		// 	GetStatic(1210)->SetText(msg);
		// 	sprintf(msg,"元神增加魔法攻击 %u\n",yssx2);
		// 	GetStatic(1211)->SetText(msg);
		// 	sprintf(msg,"元神增加物理防御 %u\n",yssx3);
		// 	GetStatic(1212)->SetText(msg);
		// 	sprintf(msg,"元神增加魔法防御 %u\n",yssx4);
		// 	GetStatic(1213)->SetText(msg);
		// 	sprintf(msg,"元神增加生命上限 %u\n",yssx5);
		// 	GetStatic(1214)->SetText(msg);

}

CRoleItem* CGuiBabyDlg::GetEquipItem( int equip_type,int x /* = 0 */ )
{
	CRoleItem* pRoleItem = NULL;

	CGuiTable* pTable = GetEquipTable(equip_type, x);
	if (pTable)
	{
		if (pTable->m_listItem.size()>0)
		{
			pRoleItem =(CRoleItem*) pTable->m_listItem[0]->m_pItemData;
		}
	}

	return pRoleItem;
}

CRoleItem *CGuiBabyDlg::GetMaterialItem()
{
	CRoleItem *pRoleItem = NULL;
	CGuiItemCell *pItemCell = m_pTableMaterial1->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem *>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}

void CGuiBabyDlg::SetBabyImage(int id, DWORD dwImage)
{
	CGuiImage *pImage = GetImage(id);
	if (pImage)
	{
		pImage->SetImage(stResourceLocation("data\\interfaces.gl", 236, dwImage));
	}
}

bool CGuiBabyDlg::OnGuiEvent(UINT nEvent, UINT nID, CGuiControl *pControl)
{
	FUNCTION_BEGIN;

	if (nEvent == EVENT_BUTTON_CLICKED)
	{
		switch (nID)
		{
		case 1006: // 孩子跟随
		{
			if (putway)
			{
				putway = false;
				GetButton(1006)->SetText("孩子跟随");
				stErnvPutUserCmd cmd; // sky 请求指令
				cmd.babytype = select;
				cmd.state = 0;
				SEND_USER_CMD(cmd);
			}
			else
			{
				putway = true;
				GetButton(1006)->SetText("回家休息");
				stErnvPutUserCmd cmd; // sky 请求指令
				cmd.babytype = select;
				cmd.state = 1;
				SEND_USER_CMD(cmd);
			}

			// // dwCartoonID == 0 取消跟随
			// stFollowBabyCmd cmd;
			// cmd.TYPEID = 1;
			// SEND_USER_CMD( cmd );
		}
		break;
		case 3000: // 喂养
		{
			CRoleItem *pRoleItem = this->GetMaterialItem();
			if (!pRoleItem)
			{
				cltext->SetText("请放入材料:练习册");
				return false;
			}

			// 请求提升娃娃等级
			stErnvLevelup cmd; // sky 请求指令
			cmd.babytype = select;
			cmd.itemUID = pRoleItem->GetObject()->qwThisID;
			SEND_USER_CMD(cmd);
		}
		break;
		case 2293: // 图文帮助
		{
		}
		break;
		case 201:
		case 202:
		case 203:
		case 204:
		case 205:
		{
			DWORD skill = nID - 200;
			stErnvSkillLevelup cmd; // sky 请求指令
			cmd.babytype = select;
			cmd.skill = skill;
			SEND_USER_CMD(cmd);
		}
		break;
		case -1:
		{
			SetVisible(false);
		}
		break;
		}

		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent, nID, pControl);
	FUNCTION_END;
}

void CGuiBabyDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiBaby = NULL;

	FUNCTION_END;
}

void CGuiBabyDlg::shuaxin(void)
{

	if (pLevel)
	{
		pLevel->SetPos(babylevel);
	}

	if (pLevel2)
	{
		pLevel2->SetPos(babyexp);
	}

	char nums[256];
	sprintf(nums, " 当前等级:%d", babylevel);
	level->SetText(nums);
	sprintf(nums, " 经验:%d/2000", babyexp);
	exps->SetText(nums);
	sprintf(nums, "%d", num1);
	Snum1->SetText(nums);
	sprintf(nums, "%d", num2);
	Snum2->SetText(nums);
	sprintf(nums, "%d", num3);
	Snum3->SetText(nums);
	sprintf(nums, "%d", num4);
	Snum4->SetText(nums);
	sprintf(nums, "%d", num5);
	Snum5->SetText(nums);

	sprintf(nums, "%d", ll);
	Sll->SetText(nums);
	sprintf(nums, "%d", zl);
	Szl->SetText(nums);
	sprintf(nums, "%d", mj);
	Smj->SetText(nums);
	sprintf(nums, "%d", js);
	Sjs->SetText(nums);
	sprintf(nums, "%d", tz);
	Stz->SetText(nums);

	sprintf(nums, "%d级", babyskill1);
	GetStatic(301)->SetText(nums);
	sprintf(nums, "%d级", babyskill2);
	GetStatic(302)->SetText(nums);
	sprintf(nums, "%d级", babyskill3);
	GetStatic(303)->SetText(nums);
	sprintf(nums, "%d级", babyskill4);
	GetStatic(304)->SetText(nums);
	sprintf(nums, "%d级", babyskill5);
	GetStatic(305)->SetText(nums);

	sprintf(nums, "技能点数：%d", babyskillpoint);
	Sskillpoint->SetText(nums);

	CToolTips *pToolTips = GetButton(101)->GetToolTips();
	pToolTips->Clear();
	pToolTips->SetBkColor(D3DCOLOR_ARGB(128, 0, 0, 0));
	pToolTips->SetAlignType(GUI_ALIGN_LEFT);
	pToolTips->SetBorderColor(0);
	pToolTips->SetLineGaps(3);
	pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
	pToolTips->AddText("物理攻击增强\n\n\n");
	pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
	pToolTips->AddText("类型：被动技能\n\n", 0);
	pToolTips->AddText("效果：\n\n", 0);
	char bfb[256];
	sprintf(bfb, " 永久提升角色物理攻击 %d%%", babyskill1);
	pToolTips->AddText(bfb, 0);
	pToolTips->Resize();

	CToolTips *pToolTips2 = GetButton(102)->GetToolTips();
	pToolTips2->Clear();
	pToolTips2->SetBkColor(D3DCOLOR_ARGB(128, 0, 0, 0));
	pToolTips2->SetAlignType(GUI_ALIGN_LEFT);
	pToolTips2->SetBorderColor(0);
	pToolTips2->SetLineGaps(3);
	pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
	pToolTips2->AddText("魔法攻击增强\n\n\n");
	pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
	pToolTips2->AddText("类型：被动技能\n\n", 0);
	pToolTips2->AddText("效果：\n\n", 0);
	sprintf(bfb, " 永久提升角色魔法攻击 %d%%", babyskill2);
	pToolTips2->AddText(bfb, 0);
	pToolTips2->Resize();

	CToolTips *pToolTips3 = GetButton(103)->GetToolTips();
	pToolTips3->Clear();
	pToolTips3->SetBkColor(D3DCOLOR_ARGB(128, 0, 0, 0));
	pToolTips3->SetAlignType(GUI_ALIGN_LEFT);
	pToolTips3->SetBorderColor(0);
	pToolTips3->SetLineGaps(3);
	pToolTips3->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
	pToolTips3->AddText("物理防御增强\n\n\n");
	pToolTips3->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
	pToolTips3->AddText("类型：被动技能\n\n", 0);
	pToolTips3->AddText("效果：\n\n", 0);
	sprintf(bfb, " 永久提升角色物理防御 %d%%", babyskill3);
	pToolTips3->AddText(bfb, 0);
	pToolTips3->Resize();

	CToolTips *pToolTips4 = GetButton(104)->GetToolTips();
	pToolTips4->Clear();
	pToolTips4->SetBkColor(D3DCOLOR_ARGB(128, 0, 0, 0));
	pToolTips4->SetAlignType(GUI_ALIGN_LEFT);
	pToolTips4->SetBorderColor(0);
	pToolTips4->SetLineGaps(3);
	pToolTips4->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
	pToolTips4->AddText("魔防防御增强\n\n\n");
	pToolTips4->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
	pToolTips4->AddText("类型：被动技能\n\n", 0);
	pToolTips4->AddText("效果：\n\n", 0);
	sprintf(bfb, " 永久提升角色魔法防御 %d%%", babyskill4);
	pToolTips4->AddText(bfb, 0);

	pToolTips4->Resize();

	CToolTips *pToolTips5 = GetButton(105)->GetToolTips();
	pToolTips5->Clear();
	pToolTips5->SetBkColor(D3DCOLOR_ARGB(128, 0, 0, 0));
	pToolTips5->SetAlignType(GUI_ALIGN_LEFT);
	pToolTips5->SetBorderColor(0);
	pToolTips5->SetLineGaps(3);
	pToolTips5->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
	pToolTips5->AddText("角色生命增强\n\n\n");
	pToolTips5->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
	pToolTips5->AddText("类型：被动技能\n\n", 0);
	pToolTips5->AddText("效果：\n\n", 0);
	sprintf(bfb, " 永久提升角色生命 %d%%", babyskill5);
	pToolTips5->AddText(bfb, 0);
	pToolTips5->Resize();

	if (babyskill1 >= 10)
	{
		GetButton(201)->SetText("满级");
		GetButton(201)->SetEnabled(false);
	}
	else
	{
		GetButton(201)->SetText("升级");
		GetButton(201)->SetEnabled(true);
	}
	if (babyskill2 >= 10)
	{
		GetButton(202)->SetText("满级");
		GetButton(202)->SetEnabled(false);
	}
	else
	{
		GetButton(202)->SetText("升级");
		GetButton(202)->SetEnabled(true);
	}
	if (babyskill3 >= 10)
	{
		GetButton(203)->SetText("满级");
		GetButton(203)->SetEnabled(false);
	}
	else
	{
		GetButton(203)->SetText("升级");
		GetButton(203)->SetEnabled(true);
	}
	if (babyskill4 >= 10)
	{
		GetButton(204)->SetText("满级");
		GetButton(204)->SetEnabled(false);
	}
	else
	{
		GetButton(204)->SetText("升级");
		GetButton(204)->SetEnabled(true);
	}
	if (babyskill5 >= 10)
	{
		GetButton(205)->SetText("满级");
		GetButton(205)->SetEnabled(false);
	}
	else
	{
		GetButton(205)->SetText("升级");
		GetButton(205)->SetEnabled(true);
	}


	if(babyskillpoint == 0)
	{
		GetButton(201)->SetEnabled(false);
		GetButton(202)->SetEnabled(false);
		GetButton(203)->SetEnabled(false);
		GetButton(204)->SetEnabled(false);
		GetButton(205)->SetEnabled(false);
	}
	// todo 技能 显示
}

CGuiTable* & CGuiBabyDlg::GetEquipTable(int equip_type,int x )
{
	FUNCTION_BEGIN;

	return GetGameGuiManager()->m_guiUserInfo->GetEquipTable(equip_type,x);

	FUNCTION_END;
}
