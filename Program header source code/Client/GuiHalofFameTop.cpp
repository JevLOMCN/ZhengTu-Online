/**
*\file		GuiHalofFameTop.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    鲜花排行榜对话框
*
* 
*
*/
#include "public.h"

#include "GameGuiManager.h"
#include "RanksMgr.h"
#include "Maincharacter.h"
#include "GameScene.h"
#include "GuiHalofFameTop.h"

#define ID_STATIC_RANKING 1101
#define ID_BUTTON_FLOWER_CKZB 1102  

namespace
{
	//add 2023-04-07
	const int LISTBOX_RANKLIST = 2;//2;

	const int FLOWER_LISTBOX = 11; // 鲜花排行榜


	const RANK_CELEBRITY_TYPE s_RankList[RANK_CELEBRITY_TYPE_COUNT] =
	{
		RANK_CELEBRITY_TYPE_FLOWER,    // 鲜花资源排行榜
	};
}

CGuiHalofFameTopDialog::CGuiHalofFameTopDialog()
{
	FUNCTION_BEGIN;
	m_pListBoxRankList = NULL;  // 排行榜列表
    m_pListBoxFlower = NULL;  //鲜花榜

	FUNCTION_END;
}

// 刷新鲜花排行榜 桃子
void CGuiHalofFameTopDialog::RefreshFlowerList()
{
	if (!m_pListBoxFlower) 
	{
		return;
	}
	bool findchar = false;
	m_pListBoxFlower->RemoveAllItems();

	char szRank[MAX_NAMESIZE] = {0};
	for (int i = 0; i < GetRanksMgr().GetFlowerNum(); ++i)
	{
		FlowerAltarInfo info;
		if (GetRanksMgr().GetFlowerInfo(i, info))
		{
			
			m_pListBoxFlower->AddItem("", 0);

			itoa(i + 1, szRank, 10);
			int nIndex = m_pListBoxFlower->GetItemCount() - 1;
			m_pListBoxFlower->SetItemText2(nIndex, 0, szRank, 50);
			m_pListBoxFlower->SetItemColor(nIndex,0,D3DCOLOR_ARGB(255,255,255,0));
		
			m_pListBoxFlower->SetItemText2(nIndex, 1, info.name, 109);//玩家名字
			////////////新增排行榜显示特殊名字颜色
			if( i == 0 )
			{
			  m_pListBoxFlower->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxFlower->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxFlower->SetItemColor(nIndex,1,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////
			char szcCountryName[30] = {0};
			
			size_t iIndex=3;

			sprintf(szcCountryName,"%s",info.counName);

			m_pListBoxFlower->SetItemText2(nIndex, 2, szcCountryName, 102); //国家
			m_pListBoxFlower->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,255,255,0));
			////////////新增排行榜显示特殊国家颜色
			if( i == 0 )
			{
			  m_pListBoxFlower->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxFlower->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxFlower->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////
			char szFlower[30] = {0};
			sprintf(szFlower,"%d",info.flowerNum);
			m_pListBoxFlower->SetItemText2(nIndex, 3, szFlower, 70);     //鲜花数量
			////////////新增排行榜显示特殊鲜花数颜色
			if( i == 0 )
			{
			  m_pListBoxFlower->SetItemColor(nIndex,3,D3DCOLOR_ARGB(255,153,50,204)); //紫色
			}
			else if( i >= 1 &&  i <= 2)
			{
			  m_pListBoxFlower->SetItemColor(nIndex,3,D3DCOLOR_ARGB(255,0,255,0));  //绿色
			}
			else if( i >= 3 &&  i <= 9)
			{
			  m_pListBoxFlower->SetItemColor(nIndex,3,D3DCOLOR_ARGB(255,255,215,0)); //黄色
			}
			////////////////////////////////////////////////			        
			if((info.charid == GetScene()->GetMainCharacter()->m_data.userid))
			{
				findchar = true;
                char szNUM[64] = {0};
				sprintf(szNUM,"你当前排第 %d 名",i+1);
				GetStatic(ID_STATIC_RANKING)->SetText(szNUM);
				
			}
		}
	}
	if(!findchar)
	{
		GetStatic(ID_STATIC_RANKING)->SetText("你当前没有排名");
	}
}


void CGuiHalofFameTopDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiHalofFameTopDialog = NULL;

	FUNCTION_END;
}

void CGuiHalofFameTopDialog::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	if(m_pBtnClose)
	{
		m_pBtnClose->SetVisible(false);
	}
	
	GetButton( 1 )->SetLocation( 673, 122 ); //关闭按钮位置

	m_pListBoxRankList = this->GetListBox(LISTBOX_RANKLIST);
	if (m_pListBoxRankList)
	{
		m_pListBoxRankList->RemoveAllItems();
		m_pListBoxRankList->AddItem("世界鲜花排行榜", 0);
		m_pListBoxRankList->SetFont(1);
		//m_pListBoxRankList->SetItemColor(0, 0, D3DCOLOR_ARGB(255,255,255,0));
		//m_pListBoxRankList->SetCurItem(static_cast<int>(GetRanksMgr().GetCurRankMr()));
	}

	//bright moon 鲜花排行榜控件
	m_pListBoxFlower = this->GetListBox(FLOWER_LISTBOX);
	if (m_pListBoxFlower)
	{
		m_pListBoxFlower->RemoveAllItems();
	}

	this->Update();

	FUNCTION_END;
}

void CGuiHalofFameTopDialog::OnResetScreenSize()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnResetScreenSize();

	m_x = ( GetDevice()->GetWidth() - GetImage( 0 )->GetWidth() ) / 2;
	m_y = ( GetDevice()->GetHeight() - GetImage( 0 )->GetHeight() ) / 2;

	FUNCTION_END;
}

bool CGuiHalofFameTopDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
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
			switch (nID)
			{
				case 1: // 关闭
				{
					SetVisible(false);
				}
				break;
				case 101: // 今日鲜花榜按钮
				{
					m_pListBoxRankList->SelectItem(0,true);
				}
				break;
			}
			if (nID == ID_BUTTON_FLOWER_CKZB)
			{
				stSelectMainUserPropertyUserCmd  cmd;
				int nCurItem = m_pListBoxFlower->GetCurItem();
				FlowerAltarInfo info;
				if (GetRanksMgr().GetFlowerInfo(nCurItem, info))
				{
					cmd.dwTempID = info.charid;	
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

HRESULT CGuiHalofFameTopDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	return CGuiDialog::OnRender(fElapsedTime);

	FUNCTION_END;
}

//bright moon 请求鲜花排行列表
void CGuiHalofFameTopDialog::ShowFlowerRank(bool bShow)
{
	if (!m_pListBoxFlower)
		return;

	m_pListBoxFlower->SetVisible(bShow);
	GetStatic(ID_STATIC_RANKING)->SetVisible(true);
	GetButton(ID_BUTTON_FLOWER_CKZB)->SetVisible(true);
	if (bShow)
	{
		ReqAltarListFlowerUserCmd cmd; //请求指令
		cmd.byParam = REQ_FLOWER_LIST_PARA;
		SEND_USER_CMD(cmd);
    }

}

void CGuiHalofFameTopDialog::Update()
{
	if (m_pListBoxRankList) // 排行榜列表
	{
		int nIndex = m_pListBoxRankList->GetCurItem();
		GetRanksMgr().SetCurRankMr(s_RankList[nIndex]);

		switch (s_RankList[nIndex])
		{
        	case RANK_CELEBRITY_TYPE_FLOWER: // 鲜花排行榜
			{				
				this->ShowFlowerRank(true);
			}
			break;
		default:
			break;
		}
	}
}
