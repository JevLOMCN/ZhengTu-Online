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
#include "./Guijianzhong.h"

///////////////////////////////////////////////////////////////////////////////
#define ID_BUTTON_JIANZHONG  200    // 剑冢
#define ID_BUTTON_JINGMAI    201    // 经脉
#define ID_BUTTON_SHENGXIAO  202   // 生肖
#define ID_BUTTON_SHENGQI    203    // 圣器
#define ID_BUTTON_ZHENHGTUZHUAN  204 // 征途传
#define ID_BUTTON_TISHENG     29 // 提升
 
#define ID_BUTTON_JIAN1  100    // 剑1
#define ID_BUTTON_JIAN2  101   // 剑2
#define ID_BUTTON_JIAN3  102    // 剑3
#define ID_BUTTON_JIAN4  103 // 剑4
#define ID_BUTTON_JIAN5  104 // 剑5
#define ID_BUTTON_JIAN6  105 // 剑6
 

#define CAILIAO       799   //升级材料

#define ID_BUTTON_CLOSE       1   //关闭
///////////////////////////////////////////////////////////////////////////////
CGuijianzhongDlg::CGuijianzhongDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;
	m_pTableMaterial1 = NULL;
	FUNCTION_END;
}

CGuijianzhongDlg::~CGuijianzhongDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuijianzhongDlg::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	jianimg = GetImage(12);
	jianname = GetImage(13);
	jian1 = GetButton(ID_BUTTON_JIAN1);
	jian2 = GetButton(ID_BUTTON_JIAN2);
	jian3 = GetButton(ID_BUTTON_JIAN3);
	jian4 = GetButton(ID_BUTTON_JIAN4);
	jian5 = GetButton(ID_BUTTON_JIAN5);
	jian6 = GetButton(ID_BUTTON_JIAN6);
	select =1;
	m_pTableMaterial1 = this->GetTable(130);
	m_pTableMaterial1->m_iTableType = OBJECTCELLTYPE_FUJIAN;
	m_pTableMaterial1->m_iTableID = 0;
	m_pTableMaterial1->m_EquipPosition = stPointI(0,1);
	cltext = GetStatic(27);
	moneytext = GetStatic(28);

	FUNCTION_END;
}


CRoleItem* CGuijianzhongDlg::GetMaterialItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableMaterial1->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}

void CGuijianzhongDlg::UPDATE()
{
	if(j1>0)
	{
		jian1->SetBitmap(stResourceLocation("data\\interfaces3.gl",2,221),1); 
	}
	else{
		jian1->SetBitmap(stResourceLocation("data\\interfaces3.gl",2,224),1); 
	}

	if(j2>0)
	{
		jian2->SetBitmap(stResourceLocation("data\\interfaces3.gl",2,232),1); 
	}
	else{
		jian2->SetBitmap(stResourceLocation("data\\interfaces3.gl",2,235),1); 
	}

	if(j3>0)
	{
		jian3->SetBitmap(stResourceLocation("data\\interfaces3.gl",2,237),1); 
	}
	else{
		jian3->SetBitmap(stResourceLocation("data\\interfaces3.gl",2,240),1); 
	}

	if(j4>0)
	{
		jian4->SetBitmap(stResourceLocation("data\\interfaces3.gl",2,242),1); 
	}
	else{
		jian4->SetBitmap(stResourceLocation("data\\interfaces3.gl",2,245),1); 
	}

	if(j5>0)
	{
		jian5->SetBitmap(stResourceLocation("data\\interfaces3.gl",2,293),1); 
	}
	else{
		jian5->SetBitmap(stResourceLocation("data\\interfaces3.gl",2,296),1); 
	}

	if(j6>0)
	{
		jian6->SetBitmap(stResourceLocation("data\\interfaces3.gl",2,298),1); 
	}
	else{
		jian6->SetBitmap(stResourceLocation("data\\interfaces3.gl",2,301),1); 
	}
	char level1[256];
	if(select==1)
	{
		if(jianimg)
		{	
			if(j1>0)
			{
				jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,217));
				sprintf(level1,"%d",j1);
				GetStatic(24)->SetText(level1);
				GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 255, 255, 0));
				
			}
			else{
				jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,266));
				GetStatic(24)->SetText("未激活");
				GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 145, 145, 145));
				
			}
			UPDATEJZSX(j1);
					
		}
		if(jianname)
		{	
			jianname->SetImage(stResourceLocation("data\\interfaces3.gl",2,225));
		}
	}	
	
	if(select==2)
	{
		if(jianimg)
				{	
					if(j2>0)
					{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,219));
						sprintf(level1,"%d",j2);
						GetStatic(24)->SetText(level1);
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					}
					else{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,263));
						GetStatic(24)->SetText("未激活");
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 145, 145, 145));
					}
					UPDATEJZSX(j2);
				}
				if(jianname)
				{	
					jianname->SetImage(stResourceLocation("data\\interfaces3.gl",2,236));
				}
	}	


	if(select==3)
	{
		if(jianimg)
				{	
					if(j3>0)
					{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,220));
						sprintf(level1,"%d",j3);
						GetStatic(24)->SetText(level1);
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					}
					else{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,264));
						GetStatic(24)->SetText("未激活");
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 145, 145, 145));
					}
					UPDATEJZSX(j3);
				}
				if(jianname)
				{	
					jianname->SetImage(stResourceLocation("data\\interfaces3.gl",2,241));
				}
	}	


	if(select==4)
	{
		if(jianimg)
				{	
					if(j4>0)
					{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,216));
						sprintf(level1,"%d",j4);
						GetStatic(24)->SetText(level1);
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					}
					else{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,265));
						GetStatic(24)->SetText("未激活");
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 145, 145, 145));
					}
					UPDATEJZSX(j4);
				}
				if(jianname)
				{	
					jianname->SetImage(stResourceLocation("data\\interfaces3.gl",2,246));
				}
	}	


	if(select==5)
	{
		if(jianimg)
				{	
					if(j5>0)
					{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,309));
						sprintf(level1,"%d",j5);
						GetStatic(24)->SetText(level1);
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					}
					else{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,311));
						GetStatic(24)->SetText("未激活");
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 145, 145, 145));
					}
					UPDATEJZSX(j5);
				}
				if(jianname)
				{	
					jianname->SetImage(stResourceLocation("data\\interfaces3.gl",2,297));
				}
	}	



	if(select==6)
	{
		if(jianimg)
		{	
			if(j6>0)
			{
				jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,310));
				sprintf(level1,"%d",j6);
				GetStatic(24)->SetText(level1);
				GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 255, 255, 0));
			}
			else{
				jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,312));
				GetStatic(24)->SetText("未激活");
				GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 145, 145, 145));
			}
			UPDATEJZSX(j6);
		}
		if(jianname)
		{	
			jianname->SetImage(stResourceLocation("data\\interfaces3.gl",2,302));
		}
	}	


	//神剑总属性
	int pDam=0;
	int mDam=0;
	int pDef=0;
	int mDef=0;
	int hp=0;
	shenjianpeizhi sjs;
	for(int i=0;i<j1;i++)
	{
		sjs = g_jianzhong[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<j2;i++)
	{
		sjs = g_jianzhong[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<j3;i++)
	{
		sjs = g_jianzhong[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<j4;i++)
	{
		sjs = g_jianzhong[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<j5;i++)
	{
		sjs = g_jianzhong[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<j6;i++)
	{
		sjs = g_jianzhong[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	char num[256];
	
	sprintf(num,"+%d",pDam);
	GetStatic(123)->SetText(num);
	sprintf(num,"+%d",mDam);
	GetStatic(124)->SetText(num);
	sprintf(num,"+%d",pDef);
	GetStatic(125)->SetText(num);
	sprintf(num,"+%d",mDef);
	GetStatic(126)->SetText(num);
	sprintf(num,"+%d",hp);
	GetStatic(127)->SetText(num);			
		
}

void CGuijianzhongDlg::UPDATEJZSX(DWORD j1){

	int pDam=0;
	int mDam=0;
	int pDef=0;
	int mDef=0;
	int hp=0;
	for(int i=0;i<j1;i++)
	{
		shenjianpeizhi sjs = g_jianzhong[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	char num[256];
	sprintf(num,"增加物攻:%d",pDam);
	GetStatic(107)->SetText(num);
	sprintf(num,"增加魔攻:%d",mDam);
	GetStatic(108)->SetText(num);
	sprintf(num,"增加物防:%d",pDef);
	GetStatic(109)->SetText(num);
	sprintf(num,"增加魔防:%d",mDef);
	GetStatic(110)->SetText(num);
	sprintf(num,"增加生命:%d",hp);
	GetStatic(111)->SetText(num);

	shenjianpeizhi sj = g_jianzhong[j1];
	sprintf(num,"增加物攻:%d",sj.pDam);
	GetStatic(113)->SetText(num);
	sprintf(num,"增加魔攻:%d",sj.mDam);
	GetStatic(114)->SetText(num);
	sprintf(num,"增加物防:%d",sj.pDef);
	GetStatic(115)->SetText(num);
	sprintf(num,"增加魔防:%d",sj.mDef);
	GetStatic(116)->SetText(num);
	sprintf(num,"增加生命:%d",sj.hp);
	GetStatic(117)->SetText(num);

	int money=sj.salary;

	int ding =0;
	int liang =0;
	char moneytexts[32];
	if(money>10000){
		ding = money/10000;
		liang = (money-(ding*10000))/100;
		sprintf(moneytexts,"             需要银子:%d锭%d两",ding,liang);
	}
	else{
		liang = money/100;
		sprintf(moneytexts,"             需要银子:%d两",liang);
	}
	if(j1<100)
	{
		moneytext->SetText(moneytexts);
		ObjectBase_t* pObject = GetObjectBase(sj.costID);
		char cailiao[256];
		sprintf(cailiao,"           需要%s 数量:%d",pObject->strName,sj.costNum);
		cltext->SetText(cailiao);
	}
	
	
	
		
	

}

bool CGuijianzhongDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	char level1[256];
	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
		case ID_BUTTON_CLOSE:  // 关闭
			{
				SetVisible(false);
			}
			break;
		case ID_BUTTON_JINGMAI:  // 经脉
			{
				GetGameGuiManager()->Addjingmai();
				SetVisible(false);
			}
			break;
		case ID_BUTTON_JIANZHONG:  // 剑冢
			{
				
			}
			break;
		case ID_BUTTON_SHENGXIAO:  // 生肖
			{
				GetGameGuiManager()->Addshengxiao();
				SetVisible(false);
			}
			break;
		case ID_BUTTON_SHENGQI:  // 圣器
			{
				GetGameGuiManager()->Addshengqi();
				SetVisible(false);
			}
			break;
		case ID_BUTTON_ZHENHGTUZHUAN:  // 征途转
			{
				GetGameGuiManager()->Addzhengtuzhuan();
				SetVisible(false);
			}
			break;
		case ID_BUTTON_JIAN1:
			{
				if(jianimg)
				{	
					if(j1>0)
					{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,217));
						sprintf(level1,"%d",j1);
						GetStatic(24)->SetText(level1);
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					}
					else{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,266));
						GetStatic(24)->SetText("未激活");
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 145, 145, 145));
					}
					UPDATEJZSX(j1);
					
				}
				if(jianname)
				{	
					jianname->SetImage(stResourceLocation("data\\interfaces3.gl",2,225));
				}
				select=1;
			}
			break;
		case ID_BUTTON_JIAN2:
			{
				if(jianimg)
				{	
					if(j2>0)
					{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,219));
						sprintf(level1,"%d",j2);
						GetStatic(24)->SetText(level1);
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					}
					else{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,263));
						GetStatic(24)->SetText("未激活");
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 145, 145, 145));
					}
					UPDATEJZSX(j2);
				}
				if(jianname)
				{	
					jianname->SetImage(stResourceLocation("data\\interfaces3.gl",2,236));
				}
				select=2;
			}
			break;
		case ID_BUTTON_JIAN3:
			{
				if(jianimg)
				{	
					if(j3>0)
					{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,220));
						sprintf(level1,"%d",j3);
						GetStatic(24)->SetText(level1);
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					}
					else{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,264));
						GetStatic(24)->SetText("未激活");
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 145, 145, 145));
					}
					UPDATEJZSX(j3);
				}
				if(jianname)
				{	
					jianname->SetImage(stResourceLocation("data\\interfaces3.gl",2,241));
				}
				select=3;
			}
			break;
		case ID_BUTTON_JIAN4:
			{

				if(jianimg)
				{	
					if(j4>0)
					{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,216));
						sprintf(level1,"%d",j4);
						GetStatic(24)->SetText(level1);
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					}
					else{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,265));
						GetStatic(24)->SetText("未激活");
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 145, 145, 145));
					}
					UPDATEJZSX(j4);
				}
				if(jianname)
				{	
					jianname->SetImage(stResourceLocation("data\\interfaces3.gl",2,246));
				}
				select=4;
			}
			break;
		case ID_BUTTON_JIAN5:
			{
				
				if(jianimg)
				{	
					if(j5>0)
					{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,309));
						sprintf(level1,"%d",j5);
						GetStatic(24)->SetText(level1);
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					}
					else{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,311));
						GetStatic(24)->SetText("未激活");
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 145, 145, 145));
					}
					UPDATEJZSX(j5);
				}
				if(jianname)
				{	
					jianname->SetImage(stResourceLocation("data\\interfaces3.gl",2,297));
				}
				select=5;
			}
			break;
		case ID_BUTTON_JIAN6:
			{
				if(jianimg)
				{	
					if(j6>0)
					{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,310));
						sprintf(level1,"%d",j6);
						GetStatic(24)->SetText(level1);
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					}
					else{
						jianimg->SetImage(stResourceLocation("data\\interfaces3.gl",2,312));
						GetStatic(24)->SetText("未激活");
						GetStatic(24)->SetTextColor(D3DCOLOR_ARGB(255, 145, 145, 145));
					}
					UPDATEJZSX(j6);
				}
				if(jianname)
				{	
					jianname->SetImage(stResourceLocation("data\\interfaces3.gl",2,302));
				}
				select=6;
			}
			break;

		case 29://提升
			{
				CRoleItem* pRoleItem = this->GetMaterialItem();
				if(!pRoleItem)
				{

					return false;
				}
				
					//请求提升剑冢
				stFujianJianzhongLevelup cmd; //sky 请求指令
				cmd.jian=select;
				cmd.itemUID = pRoleItem->GetObject()->qwThisID;
				SEND_USER_CMD(cmd);
				
				
			}
		}
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuijianzhongDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guijianzhong = NULL;

	FUNCTION_END;
}