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
#include "./GuiChenghao.h"
#include "./GameScene.h"
#include "./character.h"
#include "./MainCharacter.h"

///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiChenghaoDlg::CGuiChenghaoDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiChenghaoDlg::~CGuiChenghaoDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuiChenghaoDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();

	m_pTab = GetTab(7);
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);	
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);	

	
	for(int i=0;i<5;i++)	
	{
		for(int j=1;j<=12;j++)
		{
			m_pTab->AddControl(i,(CGuiControl*)GetButton(((i+1)*100)+j));
		}
	}
	m_pTab->SetVisible(false);
	GetButton(1001)->SetVisible(false);
	UpdateChenghao();

	
	select=0;
	FUNCTION_END;
}


CRoleItem* CGuiChenghaoDlg::GetMaterialItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableMaterial1->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}

bool CGuiChenghaoDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
			case 2://确定
			{
				Close();
			}	
			break;
			case 10:
			{
				m_pTab->SetCurItem(0);
				GetButton(1001)->SetVisible(false);
				select = 0;
				UpdateChenghao();
			}	
			break;
			case 11://确定
			{
				m_pTab->SetCurItem(1);
				GetButton(1001)->SetVisible(false);
				select = 0;
				UpdateChenghao();
			}	
			break;
			case 12://确定
			{
				m_pTab->SetCurItem(2);
				GetButton(1001)->SetVisible(false);
				select = 0;
				UpdateChenghao();
			}
			break;
			case 13://确定
			{
				m_pTab->SetCurItem(3);
				GetButton(1001)->SetVisible(false);
				select = 0;
				UpdateChenghao();
			}
			break;
			case 14://确定
			{
				m_pTab->SetCurItem(4);
				GetButton(1001)->SetVisible(false);
				select = 0;
				UpdateChenghao();
			}
			break;
			case 1001://勾选
			{
				if(GetButton(1001)->GetChecked())
				{
					//请求取消佩戴称号
					stSetNotChenghaoUserCmd cmd1;
					CMainCharacter* pMainRole = (CMainCharacter*)GetScene()->GetMainCharacter();
					if (pMainRole)
						cmd1.dwOldTempID = cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
					else
						cmd1.dwOldTempID = 0;

					cmd1.chenghaoid = select;
					SEND_USER_CMD(cmd1);
					GetButton(1001)->SetChecked(false);
					GetButton(1001)->SetBitmap(stResourceLocation("data\\interfaces.gl",3,20), 1);
				}
				else
				{
					if(m_Chenghao[select-1].state!=1)
					{
						GameMessageBox("您未激活此称号无法佩戴！");
					}
					else
					{
						//请求佩戴称号
						stSetChenghaoUserCmd cmd1;
						CMainCharacter* pMainRole = (CMainCharacter*)GetScene()->GetMainCharacter();
						if (pMainRole)
							cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
						else
							cmd1.dwOldTempID = 0;

						cmd1.chenghaoid = select;
						SEND_USER_CMD(cmd1);
						GetButton(1001)->SetChecked(true);
						GetButton(1001)->SetBitmap(stResourceLocation("data\\interfaces.gl",3,22), 1);
					}
				}
				
			}	
			break;
		
		}

		if(nID>=101 && nID<=512)
		{
			if(nID/100==1)
			{
				GetButton(1001)->SetVisible(true);
				int id=nID%100;
				select=id;
				int y=0;
				if(id== 1 || id== 2 || id== 3)
				{
					y=190;
				}
				else if(id== 4 || id== 5 || id== 6){
					y=290;
				}
				else if(id== 7 || id== 8 || id== 9){
					y=390;
				}
				else if(id== 10 || id== 11 || id== 12){
					y=490;
				}

				int x=0;
				if(id== 1 || id== 4 || id== 7|| id== 10)
				{
					x=320;
				}
				if(id== 2 || id== 5 || id== 8|| id== 11)
				{
					x=630;
				}
				if(id== 3 || id== 6 || id== 9|| id== 12)
				{
					x=930;
				}
				GetButton(1001)->SetLocation(x,y);
			}
			else if(nID/100==2)
			{
				GetButton(1001)->SetVisible(true);
				int id=nID%100;
				select=12+id;
				int y=0;
				if(id== 1 || id== 2 || id== 3)
				{
					y=190;
				}
				else if(id== 4 || id== 5 || id== 6){
					y=290;
				}
				else if(id== 7 || id== 8 || id== 9){
					y=390;
				}
				else if(id== 10 || id== 11 || id== 12){
					y=490;
				}

				int x=0;
				if(id== 1 || id== 4 || id== 7|| id== 10)
				{
					x=320;
				}
				if(id== 2 || id== 5 || id== 8|| id== 11)
				{
					x=630;
				}
				if(id== 3 || id== 6 || id== 9|| id== 12)
				{
					x=930;
				}
				GetButton(1001)->SetLocation(x,y);

			}
			else if(nID/100==3)
			{
				GetButton(1001)->SetVisible(true);
				int id=nID%100;
				select=24+id;
				int y=0;
				if(id== 1 || id== 2 || id== 3)
				{
					y=190;
				}
				else if(id== 4 || id== 5 || id== 6){
					y=290;
				}
				else if(id== 7 || id== 8 || id== 9){
					y=390;
				}
				else if(id== 10 || id== 11 || id== 12){
					y=490;
				}

				int x=0;
				if(id== 1 || id== 4 || id== 7|| id== 10)
				{
					x=320;
				}
				if(id== 2 || id== 5 || id== 8|| id== 11)
				{
					x=630;
				}
				if(id== 3 || id== 6 || id== 9|| id== 12)
				{
					x=930;
				}
				GetButton(1001)->SetLocation(x,y);
			}
			else if(nID/100==4)
			{
				GetButton(1001)->SetVisible(true);
				int id=nID%100;
				select=36+id;
				int y=0;
				if(id== 1 || id== 2 || id== 3)
				{
					y=190;
				}
				else if(id== 4 || id== 5 || id== 6){
					y=290;
				}
				else if(id== 7 || id== 8 || id== 9){
					y=390;
				}
				else if(id== 10 || id== 11 || id== 12){
					y=490;
				}

				int x=0;
				if(id== 1 || id== 4 || id== 7|| id== 10)
				{
					x=320;
				}
				if(id== 2 || id== 5 || id== 8|| id== 11)
				{
					x=630;
				}
				if(id== 3 || id== 6 || id== 9|| id== 12)
				{
					x=930;
				}
				GetButton(1001)->SetLocation(x,y);
			}
			else if(nID/100==5)
			{
				GetButton(1001)->SetVisible(true);
				int id=nID%100;
				select=48+id;
				int y=0;
				if(id== 1 || id== 2 || id== 3)
				{
					y=190;
				}
				else if(id== 4 || id== 5 || id== 6){
					y=290;
				}
				else if(id== 7 || id== 8 || id== 9){
					y=390;
				}
				else if(id== 10 || id== 11 || id== 12){
					y=490;
				}

				int x=0;
				if(id== 1 || id== 4 || id== 7|| id== 10)
				{
					x=320;
				}
				if(id== 2 || id== 5 || id== 8|| id== 11)
				{
					x=630;
				}
				if(id== 3 || id== 6 || id== 9|| id== 12)
				{
					x=930;
				}
				GetButton(1001)->SetLocation(x,y);
			}

			if(select == GetScene()->GetMainCharacter()->GetProperty()->chenghao_select)
			{
				GetButton(1001)->SetChecked(true);
				GetButton(1001)->SetBitmap(stResourceLocation("data\\interfaces.gl",3,22), 1);
			}
			else{
				GetButton(1001)->SetChecked(false);
				GetButton(1001)->SetBitmap(stResourceLocation("data\\interfaces.gl",3,20), 1);
			}
		}
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiChenghaoDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiChenghao = NULL;

	FUNCTION_END;
}

void CGuiChenghaoDlg::UpdateChenghao(void)
{
	int m_pTabCurItem = m_pTab->GetCurItem();
	DWORD dwStateO[] = { 99 };
	if(m_pTabCurItem == 0)
	{

		for(int i=0;i<12;i++)
		{
			
			GetButton(100+i+1)->SetBitmap(stResourceLocation("data\\interfaces2.gl",25,105), 1,dwStateO);
		
		}
		POINT pt;
		pt.x = 0;
		pt.y=0;
		for (int i = 0; i < 12; i++)
		{
			if(i < 3)
			{
				if(i==0)
				{
					pt.x = 81;
				}
				else if(i==1)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				
				pt.y = 135;
			}
			else if(i<6)
			{
				if(i==3)
				{
					pt.x = 81;
				}
				else if(i==4)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				pt.y = 235;
			}
			else if(i<9)
			{
				if(i==6)
				{
					pt.x = 81;
				}
				else if(i==7)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				pt.y = 334;
			}
			else
			{
				if(i==9)
				{
					pt.x = 81;
				}
				else if(i==10)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				pt.y = 433;
			}
			stResourceLocation rlAniBody;
			rlAniBody.SetFileName("data\\magic9.pak");
			rlAniBody.group = 900;
			if(m_Chenghao[i].state == 1)
			{
				rlAniBody.frame = i+1;
				m_AniBody[i].Create(&rlAniBody);
				m_AniBody[i].SetLoopPlay(true);
				m_AniBody[i].SetSpeed(150);
				m_AniBody[i].SetColor(COLOR_ARGB(255,255,255,255));
			}
			else{
				rlAniBody.frame = i+1;
				m_AniBody[i].Create(&rlAniBody);
				m_AniBody[i].SetLoopPlay(true);
				m_AniBody[i].SetSpeed(150);
				m_AniBody[i].SetColor(COLOR_ARGB(50,255,255,255));
			}

			
		}
	}
	else if(m_pTabCurItem == 1)
	{
		for(int i=12;i<24;i++)
		{	
			GetButton(200+i+1-12)->SetBitmap(stResourceLocation("data\\interfaces2.gl",26,100+i+1-12), 1,dwStateO);

		}
		POINT pt;
		pt.x = 0;
		pt.y=0;
		for (int i = 0; i < 12; i++)
		{
			if(i < 3)
			{
				if(i==0)
				{
					pt.x = 81;
				}
				else if(i==1)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				
				pt.y = 135;
			}
			else if(i<6)
			{
				if(i==3)
				{
					pt.x = 81;
				}
				else if(i==4)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				pt.y = 235;
			}
			else if(i<9)
			{
				if(i==6)
				{
					pt.x = 81;
				}
				else if(i==7)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				pt.y = 334;
			}
			else
			{
				if(i==9)
				{
					pt.x = 81;
				}
				else if(i==10)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				pt.y = 433;
			}
			stResourceLocation rlAniBody;
			rlAniBody.SetFileName("data\\magic9.pak");
			rlAniBody.group = 901;
			if(m_Chenghao[i+12].state == 1)
			{
				rlAniBody.frame = i+1;
				m_AniBody[i].Create(&rlAniBody);
				m_AniBody[i].SetLoopPlay(true);
				m_AniBody[i].SetSpeed(150);
				m_AniBody[i].SetColor(COLOR_ARGB(255,255,255,255));
			}
			else{
				rlAniBody.frame = i+1;
				m_AniBody[i].Create(&rlAniBody);
				m_AniBody[i].SetLoopPlay(true);
				m_AniBody[i].SetSpeed(150);
				m_AniBody[i].SetColor(COLOR_ARGB(50,255,255,255));
			}
			
			
			

			
		}
	}
	else if(m_pTabCurItem == 2)
	{
		for(int i=24;i<36;i++)
		{
				GetButton(300+i+1-24)->SetBitmap(stResourceLocation("data\\interfaces2.gl",27,100+i+1-24), 1,dwStateO);
		}
		POINT pt;
		pt.x = 0;
		pt.y=0;
		for (int i = 0; i < 12; i++)
		{
			if(i < 3)
			{
				if(i==0)
				{
					pt.x = 81;
				}
				else if(i==1)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				
				pt.y = 135;
			}
			else if(i<6)
			{
				if(i==3)
				{
					pt.x = 81;
				}
				else if(i==4)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				pt.y = 235;
			}
			else if(i<9)
			{
				if(i==6)
				{
					pt.x = 81;
				}
				else if(i==7)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				pt.y = 334;
			}
			else
			{
				if(i==9)
				{
					pt.x = 81;
				}
				else if(i==10)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				pt.y = 433;
			}
			stResourceLocation rlAniBody;
			rlAniBody.SetFileName("data\\magic9.pak");
			rlAniBody.group = 902;
			if(m_Chenghao[i+24].state == 1)
			{
				rlAniBody.frame = i+1;
				m_AniBody[i].Create(&rlAniBody);
				m_AniBody[i].SetLoopPlay(true);
				m_AniBody[i].SetSpeed(150);
				m_AniBody[i].SetColor(COLOR_ARGB(255,255,255,255));
			}
			else{
				rlAniBody.frame = i+1;
				m_AniBody[i].Create(&rlAniBody);
				m_AniBody[i].SetLoopPlay(true);
				m_AniBody[i].SetSpeed(150);
				m_AniBody[i].SetColor(COLOR_ARGB(50,255,255,255));
			}
			
			
			

			
		}
	}

	else if(m_pTabCurItem == 3)
	{
		for(int i=36;i<48;i++)
		{		
				GetButton(400+i+1-36)->SetBitmap(stResourceLocation("data\\interfaces2.gl",27,100+i+1-36), 1,dwStateO);
		}
		POINT pt;
		pt.x = 0;
		pt.y=0;
		for (int i = 0; i < 12; i++)
		{
			if(i < 3)
			{
				if(i==0)
				{
					pt.x = 81;
				}
				else if(i==1)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				
				pt.y = 135;
			}
			else if(i<6)
			{
				if(i==3)
				{
					pt.x = 81;
				}
				else if(i==4)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				pt.y = 235;
			}
			else if(i<9)
			{
				if(i==6)
				{
					pt.x = 81;
				}
				else if(i==7)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				pt.y = 334;
			}
			else
			{
				if(i==9)
				{
					pt.x = 81;
				}
				else if(i==10)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				pt.y = 433;
			}
			stResourceLocation rlAniBody;
			rlAniBody.SetFileName("data\\magic9.pak");
			rlAniBody.group = 914;
			if(m_Chenghao[i+36].state == 1)
			{
				rlAniBody.frame = i+1;
				m_AniBody[i].Create(&rlAniBody);
				m_AniBody[i].SetLoopPlay(true);
				m_AniBody[i].SetSpeed(150);
				m_AniBody[i].SetColor(COLOR_ARGB(255,255,255,255));
			}
			else{
				rlAniBody.frame = i+1;
				m_AniBody[i].Create(&rlAniBody);
				m_AniBody[i].SetLoopPlay(true);
				m_AniBody[i].SetSpeed(150);
				m_AniBody[i].SetColor(COLOR_ARGB(50,255,255,255));
			}
			
			
			

			
		}
	}
	else if(m_pTabCurItem == 4)
	{
		for(int i=48;i<60;i++)
		{		
				GetButton(500+i+1-48)->SetBitmap(stResourceLocation("data\\interfaces2.gl",27,100+i+1-48), 1,dwStateO);
		}
		POINT pt;
		pt.x = 0;
		pt.y=0;
		for (int i = 0; i < 12; i++)
		{
			if(i < 3)
			{
				if(i==0)
				{
					pt.x = 81;
				}
				else if(i==1)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				
				pt.y = 135;
			}
			else if(i<6)
			{
				if(i==3)
				{
					pt.x = 81;
				}
				else if(i==4)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				pt.y = 235;
			}
			else if(i<9)
			{
				if(i==6)
				{
					pt.x = 81;
				}
				else if(i==7)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				pt.y = 334;
			}
			else
			{
				if(i==9)
				{
					pt.x = 81;
				}
				else if(i==10)
				{
					pt.x = 386;
				}
				else{
					pt.x = 691;
				}
				pt.y = 433;
			}
			stResourceLocation rlAniBody;
			rlAniBody.SetFileName("data\\magic9.pak");
			rlAniBody.group = 915;
			if(m_Chenghao[i+48].state == 1)
			{
				rlAniBody.frame = i+1;
				m_AniBody[i].Create(&rlAniBody);
				m_AniBody[i].SetLoopPlay(true);
				m_AniBody[i].SetSpeed(150);
				m_AniBody[i].SetColor(COLOR_ARGB(255,255,255,255));
			}
			else{
				rlAniBody.frame = i+1;
				m_AniBody[i].Create(&rlAniBody);
				m_AniBody[i].SetLoopPlay(true);
				m_AniBody[i].SetSpeed(150);
				m_AniBody[i].SetColor(COLOR_ARGB(50,255,255,255));
			}
			
			
			

			
		}
	}
}


HRESULT CGuiChenghaoDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	POINT pt;
	POINT pt2 = GetLocation();
	for (int i = 0; i < 12; i++)
	{
		if(i < 3)
			{
				if(i==0)
				{
					pt.x = 81 +pt2.x;
				}
				else if(i==1)
				{
					pt.x = 386+pt2.x;
				}
				else{
					pt.x = 691+pt2.x;
				}
				
				pt.y = 135+pt2.y;
			}
			else if(i<6)
			{
				if(i==3)
				{
					pt.x = 81+pt2.x;
				}
				else if(i==4)
				{
					pt.x = 386+pt2.x;
				}
				else{
					pt.x = 691+pt2.x;
				}
				pt.y = 235+pt2.y;
			}
			else if(i<9)
			{
				if(i==6)
				{
					pt.x = 81+pt2.x;
				}
				else if(i==7)
				{
					pt.x = 386+pt2.x;
				}
				else{
					pt.x = 691+pt2.x;
				}
				pt.y = 334+pt2.y;
			}
			else
			{
				if(i==9)
				{
					pt.x = 81+pt2.x;
				}
				else if(i==10)
				{
					pt.x = 386+pt2.x;
				}
				else{
					pt.x = 691+pt2.x;
				}
				pt.y = 433+pt2.y;
			}
			stPointF a;
			a.x = (float)0.68;
			a.y = (float)0.68f;

			m_AniBody[i].Render(pt.x-45,pt.y-30,NULL,&a);
	}
	
	return hr;

	FUNCTION_END;
}
