/*
*\file		GuiTop.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    排行榜对话框
*
* 
*
*/
#include "public.h"

#include "GameGuiManager.h"
#include "RanksMgr.h"
#include "Maincharacter.h"
#include "GameScene.h"
#include "GuiTop.h"

#define ID_BUTTON_ZHANLI_CKZB 1121
#define ID_BUTTON_LEVELUP_CKZB 1122
#define ID_BUTTON_TICKET_CKZB 1123
#define ID_STATIC_CKZB1 1119
#define ID_STATIC_CKZB2 1120
#define ID_STATIC_WJMC 209
namespace
{

	const int LISTBOX_RANKLIST = 2;

	const int LEVELUP_LISTBOX = 11; //世界等级排行列表

	const int ZHANLI_LISTBOX = 31; //战力排行列表

	const int TICKETUP_LISTBOX = 61; //积分排行列表

	const RANK_TYPE s_RankList[RANKTYPE_COUNT] =
	{
        RANKTYPE_LEVELUP,  //世界等级排行榜
        RANKTYPE_ZHANLI,   //战力排行榜
        RANKTYPE_T_HERO,   //今日英雄排行榜
        RANKTYPE_Y_HERO,   //昨日英雄排行榜
        RANKTYPE_TICKET,   //个人积分排行榜
	};
}

CGuiTopDialog::CGuiTopDialog()
{
	FUNCTION_BEGIN;

	m_pListBoxRankList = NULL;   //排行榜列表
	m_pListBoxLevelup = NULL;    //世界等级排行列表
	m_pListBoxZhanLi = NULL;     //战力排行榜列表
	m_pListBoxTicketup = NULL;   //积分排行榜列表

	FUNCTION_END;
}

//刷新世界等级排行榜
void CGuiTopDialog::RefreshLevelupList()
{
	if (!m_pListBoxLevelup) // 刷等级榜列表
	{
        return;
	}
	GetStatic(ID_STATIC_WJMC)->SetVisible(true);
    m_pListBoxLevelup->RemoveAllItems();
	bool findchar =false;
	char szRank[MAX_NAMESIZE] = {0};
	for (int i = 0; i < GetRanksMgr().GetLevelupNum(); ++i)
	{
		LevelupAltarInfo info;
		if (GetRanksMgr().GetLevelupInfo(i, info))
		{
			m_pListBoxLevelup->AddItem("", 0);

			itoa(i + 1, szRank, 10);
			int nIndex = m_pListBoxLevelup->GetItemCount() - 1;
			m_pListBoxLevelup->SetItemText2(nIndex, 0, szRank, 47);
			m_pListBoxLevelup->SetItemColor(nIndex,0,D3DCOLOR_ARGB(255,255,255,0));
			

			m_pListBoxLevelup->SetItemText2(nIndex, 1, info.name, 128); //玩家名字
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxLevelup->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxLevelup->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxLevelup->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////
			char szcCountryName[30] = {0};
			
		
			size_t iIndex=4;

			sprintf(szcCountryName,"%s",info.counName);

			m_pListBoxLevelup->SetItemText2(nIndex, 2, szcCountryName, 60); //国家
			m_pListBoxLevelup->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,255,255,0));
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxLevelup->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxLevelup->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxLevelup->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////
			char szLevelup[30] = {0};
			sprintf(szLevelup,"%d",info.levelupNum);
			m_pListBoxLevelup->SetItemText2(nIndex, 3, szLevelup, 83); //等级
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxLevelup->SetItemColor(nIndex,3,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxLevelup->SetItemColor(nIndex,3,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxLevelup->SetItemColor(nIndex,3,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////
			char szUnionid[32] = {0};
			sprintf(szUnionid,"%s",info.unionname);
			//sprintf(szUnionid,"%d",info.unionid);
			if(info.unionid ==0)
			{
				m_pListBoxLevelup->SetItemText2(nIndex, 4, "（无）", 115); //帮会
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxLevelup->SetItemColor(nIndex,4,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxLevelup->SetItemColor(nIndex,4,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxLevelup->SetItemColor(nIndex,4,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////				
			}
			else
			{
				m_pListBoxLevelup->SetItemText2(nIndex, 4, szUnionid, 115); //帮会
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxLevelup->SetItemColor(nIndex,4,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxLevelup->SetItemColor(nIndex,4,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxLevelup->SetItemColor(nIndex,4,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////				
			}

			if(info.charid == GetScene()->GetMainCharacter()->m_data.userid)
			{
				findchar = true;
				char szMC[64] = {0};
				sprintf(szMC,"你当前排在第%d名",i+1);
				GetStatic(ID_STATIC_WJMC)->SetText(szMC);
			}
		}
	}
	if(!findchar)
	{
		GetStatic(ID_STATIC_WJMC)->SetText("对不起，您暂未上榜！");
	}

}

//刷新战力排行榜
void CGuiTopDialog::RefreshZhanLiList()
{
	if (!m_pListBoxZhanLi)
	{
		return;
	}
	GetStatic(ID_STATIC_WJMC)->SetVisible(true);
	m_pListBoxZhanLi->RemoveAllItems(); 
	bool findchar = false;
	char szRank[MAX_NAMESIZE] = {0};
	for (int i = 0; i < GetRanksMgr().GetZhanLiNum(); ++i)
	{
		ZhanLiAltarInfo info;
		if (GetRanksMgr().GetZhanLiInfo(i, info))
		{
			m_pListBoxZhanLi->AddItem("", 0);

			itoa(i + 1, szRank, 10);
			int nIndex = m_pListBoxZhanLi->GetItemCount() - 1;
			m_pListBoxZhanLi->SetItemText2(nIndex, 0, szRank, 47);
			m_pListBoxZhanLi->SetItemColor(nIndex,0,D3DCOLOR_ARGB(255,255,255,0));
			

			m_pListBoxZhanLi->SetItemText2(nIndex, 1, info.name, 128);//玩家名字
			////////////新增排行榜显示特殊战力名字颜色
			if( i == 0 )
			{
			  m_pListBoxZhanLi->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxZhanLi->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxZhanLi->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////
			char szcCountryName[30] = {0};
			
		
			size_t iIndex=4;

			sprintf(szcCountryName,"%s",info.counName);

			m_pListBoxZhanLi->SetItemText2(nIndex, 2, szcCountryName, 60); //国家
			m_pListBoxZhanLi->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,255,255,0));
			////////////新增排行榜显示特殊战力国家颜色
			if( i == 0 )
			{
			  m_pListBoxZhanLi->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxZhanLi->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxZhanLi->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////
			char szZhanLi[30] = {0};
			sprintf(szZhanLi,"%d",info.zhanlilNum);
			m_pListBoxZhanLi->SetItemText2(nIndex, 3, szZhanLi, 83); //战力值zhanli
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxZhanLi->SetItemColor(nIndex,3,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxZhanLi->SetItemColor(nIndex,3,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxZhanLi->SetItemColor(nIndex,3,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////
			char szUnionid[32] = {0};
			sprintf(szUnionid,"%s",info.unionname);
			//sprintf(szUnionid,"%d",info.unionid);
			if(info.unionid ==0)
			{
				m_pListBoxZhanLi->SetItemText2(nIndex, 4, "（无）", 115); //帮会
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxZhanLi->SetItemColor(nIndex,4,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxZhanLi->SetItemColor(nIndex,4,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxZhanLi->SetItemColor(nIndex,4,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////				
			}
			else
			{
				m_pListBoxZhanLi->SetItemText2(nIndex, 4, szUnionid, 115); //帮会
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxZhanLi->SetItemColor(nIndex,4,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxZhanLi->SetItemColor(nIndex,4,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxZhanLi->SetItemColor(nIndex,4,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////				
			}

			//char szJiangLi[30] = {0};
			//if((100-i*10) >0)
			//{
			//	sprintf(szJiangLi,"%d",100-i*10);
			//}
			//else
			//{
			//	sprintf(szJiangLi,"%d",0);
			//}
			//m_pListBoxZhanLi->SetItemText2(nIndex, 5, szJiangLi, 60); //奖励
			//m_pListBoxZhanLi->SetItemColor(nIndex,5,D3DCOLOR_ARGB(255,0,255,0));

			if(info.charid == GetScene()->GetMainCharacter()->m_data.userid)
			{
				findchar = true;
				char szMC[64] = {0};
				sprintf(szMC,"你当前排在第%d名",i+1);
				GetStatic(ID_STATIC_WJMC)->SetText(szMC);
			}
		}
	}
	if(!findchar)
	{
		GetStatic(ID_STATIC_WJMC)->SetText("对不起，您暂未上榜！");
	}
}

//刷新英雄排行榜
void CGuiTopDialog::RefreshReroListT()
{
	if (!m_pListBoxTodayHero) 
	{
		return;
	}
	GetStatic(ID_STATIC_WJMC)->SetVisible(false);
	m_pListBoxTodayHero->RemoveAllItems();

	for (int i = 0; i < GetRanksMgr().GetHeroNum(true); ++i)
	{
		HeroAltarInfo info;
		if (GetRanksMgr().GetHeroInfo(i, info, true))
		{
			m_pListBoxTodayHero->AddItem("", 0);

			int nIndex = m_pListBoxTodayHero->GetItemCount() - 1;

			/* char name[MAX_NAMESIZE+1] = {0};
			sprintf(name,"%s",info.name); */
			m_pListBoxTodayHero->SetItemText2(nIndex, 0, info.name, 160); //玩家名字
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxTodayHero->SetItemColor(nIndex,0,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxTodayHero->SetItemColor(nIndex,0,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxTodayHero->SetItemColor(nIndex,0,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////			
			char unionname[MAX_NAMESIZE] = {0};
			sprintf(unionname,"%s",info.unionname);
			m_pListBoxTodayHero->SetItemText2(nIndex, 1, unionname, 150); //帮会名
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxTodayHero->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxTodayHero->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxTodayHero->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////			
			char killnum[MAX_NAMESIZE] = {0};
			sprintf(killnum,"%d",info.killnum);
			m_pListBoxTodayHero->SetItemText2(nIndex, 2, killnum, 95); //击杀人数
			m_pListBoxTodayHero->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,0,255,0));
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxTodayHero->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxTodayHero->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxTodayHero->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////			
		}
	}
}

//刷新昨日英雄排行榜
void CGuiTopDialog::RefreshReroListY()
{
	if (!m_pListBoxYesterdayHero) 
	{
		return;
	}
	GetStatic(ID_STATIC_WJMC)->SetVisible(false);
	m_pListBoxYesterdayHero->RemoveAllItems();

	for (int i = 0; i < GetRanksMgr().GetHeroNum(false); ++i)
	{
		HeroAltarInfo info;
		if (GetRanksMgr().GetHeroInfo(i, info, false))
		{
			m_pListBoxYesterdayHero->AddItem("", 0);

			int nIndex = m_pListBoxYesterdayHero->GetItemCount() - 1;

			/* char name[MAX_NAMESIZE+1] = {0};
			sprintf(name,"  %s",info.name); */
			m_pListBoxYesterdayHero->SetItemText2(nIndex, 0, info.name, 160); //玩家名字
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxYesterdayHero->SetItemColor(nIndex,0,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxYesterdayHero->SetItemColor(nIndex,0,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxYesterdayHero->SetItemColor(nIndex,0,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////			
			char unionname[MAX_NAMESIZE] = {0};
			sprintf(unionname,"%s",info.unionname);
			m_pListBoxYesterdayHero->SetItemText2(nIndex, 1, unionname, 150); //帮会名
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxYesterdayHero->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxYesterdayHero->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxYesterdayHero->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////			
			char killnum[MAX_NAMESIZE] = {0};
			sprintf(killnum,"%d",info.killnum);
			m_pListBoxYesterdayHero->SetItemText2(nIndex, 2, killnum, 95); //击杀数量
			m_pListBoxYesterdayHero->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,0,255,0));
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxYesterdayHero->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxYesterdayHero->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxYesterdayHero->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////			
		}
	}
	
}

//刷新个人积分排行榜
void CGuiTopDialog::RefreshTicketupList()
{
	if (!m_pListBoxTicketup) 
	{
		return;
	}
	GetStatic(ID_STATIC_WJMC)->SetVisible(true);
	m_pListBoxTicketup->RemoveAllItems();
	bool findchar = false;
	char szRank[MAX_NAMESIZE] = {0};
	for (int i = 0; i < GetRanksMgr().GetTicketupNum(); ++i)
	{
		TicketupAltarInfo info;
		if (GetRanksMgr().GetTicketupInfo(i, info))
		{
			m_pListBoxTicketup->AddItem("", 0);

			itoa(i + 1, szRank, 10);
			int nIndex = m_pListBoxTicketup->GetItemCount() - 1;
			m_pListBoxTicketup->SetItemText2(nIndex, 0, szRank, 47);
			m_pListBoxTicketup->SetItemColor(nIndex,0,D3DCOLOR_ARGB(255,255,255,0));
			

			m_pListBoxTicketup->SetItemText2(nIndex, 1, info.name, 128); //玩家名字
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxTicketup->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxTicketup->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxTicketup->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////	
			char szcCountryName[30] = {0};
			
		
			size_t iIndex=3;

			//sprintf(szcCountryName,"%.*s",strlen(info.counName)-6,info.counName);
			sprintf(szcCountryName,"%s",info.counName);

			m_pListBoxTicketup->SetItemText2(nIndex, 2, szcCountryName, 60); //国家
			m_pListBoxTicketup->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,255,255,0));
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxTicketup->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxTicketup->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxTicketup->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////
			char szTicketup[30] = {0};
			sprintf(szTicketup,"%d",info.ticketupNum);
			m_pListBoxTicketup->SetItemText2(nIndex, 3, szTicketup, 83); //积分点数
			////////////新增排行榜显示特殊战力值颜色
			if( i == 0 )
			{
			  m_pListBoxTicketup->SetItemColor(nIndex,3,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxTicketup->SetItemColor(nIndex,3,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxTicketup->SetItemColor(nIndex,3,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////

			if(info.charid == GetScene()->GetMainCharacter()->m_data.userid)
			{
				findchar = true;
				char szMC[64] = {0};
				sprintf(szMC,"你当前排在第%d名",i+1);
				GetStatic(ID_STATIC_WJMC)->SetText(szMC);
			}
		}
	}
	if(!findchar)
	{
		GetStatic(ID_STATIC_WJMC)->SetText("对不起，您暂未上榜！");
	}
}

void CGuiTopDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiTopDialog = NULL;

	FUNCTION_END;
}

void CGuiTopDialog::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		m_pBtnClose->SetLocation( 329 + 375, 6 + 1 );//关闭按键位置
	    //GetButton(ID_BUTTON_LQJL)->SetLocation(329 + 175, 500);
	    //GetButton(ID_BUTTON_ZHANLI_CKZB)->SetLocation(329 + 275, 500);

	}

	FUNCTION_END;
}

void CGuiTopDialog::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_pListBoxRankList = this->GetListBox(LISTBOX_RANKLIST);
	if (m_pListBoxRankList)
	{
		m_pListBoxRankList->RemoveAllItems();
		m_pListBoxRankList->AddItem("世界等级排行榜", 0);
		m_pListBoxRankList->AddItem("个人战力排行榜", 0);
		m_pListBoxRankList->AddItem("今日护国排行榜", 0);
		m_pListBoxRankList->AddItem("昨日护国排行榜", 0);
		// m_pListBoxRankList->AddItem("个人积分排行榜", 0);	
		m_pListBoxRankList->SetFont(1);
		m_pListBoxRankList->SetItemColor(0, 0, D3DCOLOR_ARGB(255,230,230,250));
		m_pListBoxRankList->SetItemColor(1, 0, D3DCOLOR_ARGB(255,255,240,245));
		m_pListBoxRankList->SetItemColor(2, 0, D3DCOLOR_ARGB(255,84,255,159));
		m_pListBoxRankList->SetItemColor(3, 0, D3DCOLOR_ARGB(255,255,255,0));
		m_pListBoxRankList->SetItemColor(4, 0, D3DCOLOR_ARGB(255,162,192,238));
		//m_pListBoxRankList->SetItemColor(0, 0, D3DCOLOR_ARGB(255,255,255,0));
		//m_pListBoxRankList->SetCurItem(static_cast<int>(GetRanksMgr().GetCurRank()));
	}

    //世界等级榜控件
	m_pListBoxLevelup = this->GetListBox(LEVELUP_LISTBOX);
	if (m_pListBoxLevelup)
	{
		m_pListBoxLevelup->RemoveAllItems();
	}

    //战力榜控件
	m_pListBoxZhanLi = this->GetListBox(ZHANLI_LISTBOX);
	if (m_pListBoxZhanLi)
	{
		m_pListBoxZhanLi->RemoveAllItems();
	}

    //今日英雄排行榜
	m_pListBoxTodayHero = this->GetListBox(41);
	if (m_pListBoxTodayHero)
	{
		m_pListBoxTodayHero->RemoveAllItems();
	}

    //昨日英雄排行榜
	m_pListBoxYesterdayHero = this->GetListBox(51);
	if (m_pListBoxYesterdayHero)
	{
		m_pListBoxYesterdayHero->RemoveAllItems();
	}

    //个人积分榜榜控件
	m_pListBoxTicketup = this->GetListBox(TICKETUP_LISTBOX);
	if (m_pListBoxTicketup)
	{
		m_pListBoxTicketup->RemoveAllItems();
	}

	this->Update();

	FUNCTION_END;
}

void CGuiTopDialog::OnResetScreenSize()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnResetScreenSize();

	m_x = ( GetDevice()->GetWidth() - GetImage( 0 )->GetWidth() ) / 2;
	m_y = ( GetDevice()->GetHeight() - GetImage( 0 )->GetHeight() ) / 2;

	FUNCTION_END;
}

bool CGuiTopDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	unsigned int ent = 0;
	if (nID == LISTBOX_RANKLIST)
	{
		ent = nEvent;
	}
	switch (nEvent)
	{
	case EVENT_LISTBOX_SELECTION:
		{
			if (nID == LISTBOX_RANKLIST)
                 this->Update();
		}
		break;
	case EVENT_DIALOG_SHOW:
		{
			 this->Update();
		}
		break;
	case EVENT_BUTTON_CLICKED:
		{
			if (nID == ID_BUTTON_LEVELUP_CKZB)
			{
				stSelectMainUserPropertyUserCmd  cmd;
				int nCurItem = m_pListBoxLevelup->GetCurItem();
				LevelupAltarInfo info;
				if (GetRanksMgr().GetLevelupInfo(nCurItem, info))
				{
					cmd.dwTempID = info.charid;
					//cmd.dwTempID = GetScene()->GetMainCharacter()->GetID();
				}
				cmd.byType = MAPDATATYPE_USER;
				SEND_USER_CMD(cmd);
				//Close();
			}			
			if (nID == ID_BUTTON_ZHANLI_CKZB)
			{
                stSelectMainUserPropertyUserCmd  cmd;
				int nCurItem = m_pListBoxZhanLi->GetCurItem();
				ZhanLiAltarInfo info;
				if (GetRanksMgr().GetZhanLiInfo(nCurItem, info))
				{
					cmd.dwTempID = info.charid;
					//cmd.dwTempID = GetScene()->GetMainCharacter()->GetID();
				}
				cmd.byType = MAPDATATYPE_USER;
				SEND_USER_CMD(cmd);
				//Close();
			}
			if (nID == ID_BUTTON_TICKET_CKZB)
			{
				stSelectMainUserPropertyUserCmd  cmd;
				int nCurItem = m_pListBoxTicketup->GetCurItem();
				TicketupAltarInfo info;
				if (GetRanksMgr().GetTicketupInfo(nCurItem, info))
				{
					cmd.dwTempID = info.charid;
					//cmd.dwTempID = GetScene()->GetMainCharacter()->GetID();
				}
				cmd.byType = MAPDATATYPE_USER;
	            SEND_USER_CMD(cmd);
				//Close();
			}
		}
		break;
	}

	return CGuiDialog::OnGuiEvent(nEvent, nID, pControl);

	FUNCTION_END;
}

HRESULT CGuiTopDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	return CGuiDialog::OnRender(fElapsedTime);

	FUNCTION_END;
}

//sky 请求等级列表
void CGuiTopDialog::ShowLevelupRank(bool bShow)
{
	if (!m_pListBoxLevelup)
		return;
	
	m_pListBoxLevelup->SetVisible(bShow);
	GetStatic(ID_STATIC_CKZB1)->SetVisible(false);
	GetStatic(ID_STATIC_CKZB2)->SetVisible(false);
	GetStatic(ID_STATIC_WJMC)->SetVisible(true);
	GetButton(ID_BUTTON_ZHANLI_CKZB)->SetVisible(false);
	GetButton(ID_BUTTON_TICKET_CKZB)->SetVisible(false);
	GetButton(ID_BUTTON_LEVELUP_CKZB)->SetVisible(true);
	GetStatic(12)->SetVisible(true);
	GetStatic(13)->SetVisible(true);
	GetStatic(32)->SetVisible(false);
	GetStatic(33)->SetVisible(false);
	GetStatic(42)->SetVisible(false);
	GetStatic(43)->SetVisible(false);
	GetStatic(52)->SetVisible(false);
	GetStatic(53)->SetVisible(false);
	GetStatic(62)->SetVisible(false);
	GetStatic(63)->SetVisible(false);
	if (bShow)
	{
		ReqAltarListLevelupUserCmd cmd; //请求指令
		cmd.byParam = REQ_LEVELUP_LIST_PARA;
		SEND_USER_CMD(cmd);
    }
}

//sky 请求战力列表
void CGuiTopDialog::ShowZhanLiRank(bool bShow)
{
	if (!m_pListBoxZhanLi)
		return;
	
	m_pListBoxZhanLi->SetVisible(bShow);
	GetStatic(ID_STATIC_CKZB1)->SetVisible(true);
	GetStatic(ID_STATIC_CKZB2)->SetVisible(true);
	GetStatic(ID_STATIC_WJMC)->SetVisible(true);
	GetButton(ID_BUTTON_LEVELUP_CKZB)->SetVisible(false);
	GetButton(ID_BUTTON_TICKET_CKZB)->SetVisible(false);
	GetButton(ID_BUTTON_ZHANLI_CKZB)->SetVisible(true);
	GetStatic(12)->SetVisible(false);
	GetStatic(13)->SetVisible(false);
	GetStatic(32)->SetVisible(true);
	GetStatic(33)->SetVisible(true);
	GetStatic(42)->SetVisible(false);
	GetStatic(43)->SetVisible(false);
	GetStatic(52)->SetVisible(false);
	GetStatic(53)->SetVisible(false);
	GetStatic(62)->SetVisible(false);
	GetStatic(63)->SetVisible(false);
	if (bShow)
	{
		ReqAltarListZhanLIUserCmd cmd; //sky 请求指令
		//REQ_ZHANLI_LIST_PARA cmd;
		cmd.byParam = REQ_ZHANLI_LIST_PARA;
		SEND_USER_CMD(cmd);
		//TRACE("CGuiTopDialog::ShowZhanLiRank bShow:%d,byParam:%d\n",bShow,cmd.byParam);
	}
}

//sky 今日英雄排行榜
void CGuiTopDialog::ShowTodayHero(bool bShow)
{
	if (!m_pListBoxTodayHero)
		return;

	m_pListBoxTodayHero->SetVisible(bShow);
	GetStatic(ID_STATIC_CKZB1)->SetVisible(false);
	GetStatic(ID_STATIC_CKZB2)->SetVisible(false);
	GetStatic(ID_STATIC_WJMC)->SetVisible(false);
	GetButton(ID_BUTTON_LEVELUP_CKZB)->SetVisible(false);
	GetButton(ID_BUTTON_TICKET_CKZB)->SetVisible(false);
	GetButton(ID_BUTTON_ZHANLI_CKZB)->SetVisible(false);
	GetStatic(12)->SetVisible(false);
	GetStatic(13)->SetVisible(false);
	GetStatic(32)->SetVisible(false);
	GetStatic(33)->SetVisible(false);
	GetStatic(42)->SetVisible(true);
	GetStatic(43)->SetVisible(true);
	GetStatic(52)->SetVisible(false);
	GetStatic(53)->SetVisible(false);
	GetStatic(62)->SetVisible(false);
	GetStatic(63)->SetVisible(false);
	if (bShow)
	{
		ReqAltarLisHeroUserCmd cmd; //sky 请求指令
		cmd.countryID = GetScene()->GetMainCharacter()->m_data.country;
		cmd.datevalue = 1;
		SEND_USER_CMD(cmd);
	}
}

//sky 昨日英雄排行榜
void CGuiTopDialog::ShowYesterdayHero(bool bShow)
{
	if (!m_pListBoxYesterdayHero)
		return;

	m_pListBoxYesterdayHero->SetVisible(bShow);
	GetStatic(ID_STATIC_CKZB1)->SetVisible(false);
	GetStatic(ID_STATIC_CKZB2)->SetVisible(false);
	GetStatic(ID_STATIC_WJMC)->SetVisible(false);
	GetButton(ID_BUTTON_LEVELUP_CKZB)->SetVisible(false);
	GetButton(ID_BUTTON_TICKET_CKZB)->SetVisible(false);
	GetButton(ID_BUTTON_ZHANLI_CKZB)->SetVisible(false);
	GetStatic(12)->SetVisible(false);
	GetStatic(13)->SetVisible(false);
	GetStatic(32)->SetVisible(false);
	GetStatic(33)->SetVisible(false);
	GetStatic(42)->SetVisible(false);
	GetStatic(43)->SetVisible(false);
	GetStatic(52)->SetVisible(true);
	GetStatic(53)->SetVisible(true);
	GetStatic(62)->SetVisible(false);
	GetStatic(63)->SetVisible(false);

	if (bShow)
	{
		ReqAltarLisHeroUserCmd cmd; //sky 请求指令
		cmd.countryID = GetScene()->GetMainCharacter()->m_data.country;
		cmd.datevalue = 0;
		SEND_USER_CMD(cmd);
	}
}

//sky 请求积分排行列表
void CGuiTopDialog::ShowTicketupRank(bool bShow)
{
	if (!m_pListBoxTicketup)
		return;
	
	m_pListBoxTicketup->SetVisible(bShow);
	GetStatic(ID_STATIC_CKZB1)->SetVisible(false);
	GetStatic(ID_STATIC_CKZB2)->SetVisible(false);
	GetStatic(ID_STATIC_WJMC)->SetVisible(true);
	GetButton(ID_BUTTON_ZHANLI_CKZB)->SetVisible(false);
    GetButton(ID_BUTTON_LEVELUP_CKZB)->SetVisible(false);
	GetButton(ID_BUTTON_TICKET_CKZB)->SetVisible(true);
	GetStatic(12)->SetVisible(false);
	GetStatic(13)->SetVisible(false);
	GetStatic(32)->SetVisible(false);
	GetStatic(33)->SetVisible(false);
	GetStatic(42)->SetVisible(false);
	GetStatic(43)->SetVisible(false);
	GetStatic(52)->SetVisible(false);
	GetStatic(53)->SetVisible(false);
	GetStatic(62)->SetVisible(true);
	GetStatic(63)->SetVisible(true);
	if (bShow)
	{
		ReqAltarListTicketupUserCmd cmd; //请求指令
		cmd.byParam = REQ_TICKETUP_LIST_PARA;
		SEND_USER_CMD(cmd);
    }
}

void CGuiTopDialog::Update()
{
	if (m_pListBoxRankList) //排行榜列表
	{
		int nIndex = m_pListBoxRankList->GetCurItem();

		GetRanksMgr().SetCurRank(s_RankList[nIndex]);
		switch (s_RankList[nIndex])
		{
		case RANKTYPE_LEVELUP: //世界等级排行榜
			{
				GetImage(999)->SetImage(stResourceLocation("data\\interfaces5.gl",1,10));
				this->ShowZhanLiRank(false);
				this->ShowTodayHero(false);
				this->ShowTicketupRank(false);
				this->ShowYesterdayHero(false);
				this->ShowLevelupRank(true);
			}
			break;
		case RANKTYPE_ZHANLI: //战力排行榜
			{
				GetImage(999)->SetImage(stResourceLocation("data\\interfaces5.gl",1,11));
				this->ShowLevelupRank(false);	
				this->ShowTodayHero(false);
				this->ShowTicketupRank(false);
				this->ShowYesterdayHero(false);
				this->ShowZhanLiRank(true);
			}
			break;
		case RANKTYPE_T_HERO: //今日英雄排行榜
			{
				GetImage(999)->SetImage(stResourceLocation("data\\interfaces5.gl",1,12));
				this->ShowLevelupRank(false);
				this->ShowZhanLiRank(false);
				this->ShowTicketupRank(false);
				this->ShowYesterdayHero(false);
				this->ShowTodayHero(true);
			}
			break;
		case RANKTYPE_Y_HERO: //昨日英雄排行榜
			{		
				GetImage(999)->SetImage(stResourceLocation("data\\interfaces5.gl",1,12));
				this->ShowLevelupRank(false);
				this->ShowZhanLiRank(false);
				this->ShowTicketupRank(false);
				this->ShowTodayHero(false);				
				this->ShowYesterdayHero(true);
			}
			break;
		case RANKTYPE_TICKET: //个人积分排行榜
			{
				this->ShowLevelupRank(false);
				this->ShowZhanLiRank(false);
				this->ShowTodayHero(false);
				this->ShowYesterdayHero(false);
				this->ShowTicketupRank(true);
			}
			break;
		default:
			break;
		}
	}
}
