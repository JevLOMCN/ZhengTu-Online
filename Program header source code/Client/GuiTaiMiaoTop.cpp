/**
*\file		GuiTaiMiaoTop.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    太庙资源排行榜对话框
*
* 
*
*/
#include "public.h"

#include "GameGuiManager.h"
#include "RanksMgr.h"
#include "Maincharacter.h"
#include "GameScene.h"
#include "GuiTaiMiaoTop.h"

//#define ID_STATIC_CKZB 208
namespace
{
	//add 2023-04-07
	const int LISTBOX_RANKLIST = 2;//2;

	const int LOONGPEARL_LISTBOX = 21; // 太庙资源榜列表


	const RANK_TAIMIAO_TYPE s_RankList[RANK_TAIMIAO_TYPE_COUNT] =
	{
		RANK_TAIMIAO_TYPE_LOONGPEARL,    // 太庙资源排行榜
	};
}

CGuiTaiMiaoTopDialog::CGuiTaiMiaoTopDialog()
{
	FUNCTION_BEGIN;
	m_pListBoxRankList = NULL;  // 排行榜列表

	FUNCTION_END;
}

// 刷新太庙资源榜
void CGuiTaiMiaoTopDialog::RefreshLoongPearlList()
{
	if (!m_pListBoxLoongPearl) // 太庙资源榜列表
	{
		return;
	}

	m_pListBoxLoongPearl->RemoveAllItems();

	//char szRank[MAX_NAMESIZE] = {0};
	for (int i = 0; i < GetRanksMgr().GetLoongPearlNum(); ++i)
	{
		LoongAltarInfo info;
		if (GetRanksMgr().GetLoongPearlInfo(i, info))
		{
			m_pListBoxLoongPearl->AddItem("", 0);

			//itoa(i + 1, szRank, 10);
			int nIndex = m_pListBoxLoongPearl->GetItemCount() - 1;
			//m_pListBoxLoongPearl->SetItemText2(nIndex, 0, szRank, 60);
			char szcounName[30] = {0};
			sprintf(szcounName,"  %s",info.counName);
			m_pListBoxLoongPearl->SetItemText2(nIndex, 0, szcounName, 130);//国家位置
			char szLoongNum[30] = {0};
			sprintf(szLoongNum,"%d",info.pearlNum);
			m_pListBoxLoongPearl->SetItemText2(nIndex, 1, szLoongNum, 95); //砖块数量位置

			m_pListBoxLoongPearl->SetItemText2(nIndex, 2, "千江御敌", 95); //太庙效果
			m_pListBoxLoongPearl->SetItemColor(nIndex,2,D3DCOLOR_ARGB(255,0,255,0));

			
		}
	}
}

void CGuiTaiMiaoTopDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiTaiMiaoTopDialog = NULL;

	FUNCTION_END;
}

void CGuiTaiMiaoTopDialog::OnSetCloseLocation()
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

void CGuiTaiMiaoTopDialog::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_pListBoxRankList = this->GetListBox(LISTBOX_RANKLIST);
	if (m_pListBoxRankList)
	{
		m_pListBoxRankList->RemoveAllItems();
		m_pListBoxRankList->AddItem("国家太庙资源", 0);
		m_pListBoxRankList->SetFont(1);
		//m_pListBoxRankList->SetItemColor(0, 0, D3DCOLOR_ARGB(255,255,255,0));
		//m_pListBoxRankList->SetCurItem(static_cast<int>(GetRanksMgr().GetCurRanktaim()));
	}

	// 太庙资源榜控件
	m_pListBoxLoongPearl = this->GetListBox(LOONGPEARL_LISTBOX);
	if (m_pListBoxLoongPearl)
	{
		m_pListBoxLoongPearl->RemoveAllItems();
	}

	this->Update();

	FUNCTION_END;
}

void CGuiTaiMiaoTopDialog::OnResetScreenSize()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnResetScreenSize();

	m_x = ( GetDevice()->GetWidth() - GetImage( 0 )->GetWidth() ) / 2;
	m_y = ( GetDevice()->GetHeight() - GetImage( 0 )->GetHeight() ) / 2;

	FUNCTION_END;
}

bool CGuiTaiMiaoTopDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
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
		
		}
		break;
	}

	return CGuiDialog::OnGuiEvent(nEvent, nID, pControl);

	FUNCTION_END;
}

HRESULT CGuiTaiMiaoTopDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	return CGuiDialog::OnRender(fElapsedTime);

	FUNCTION_END;
}


//sky 请求太庙列表
void CGuiTaiMiaoTopDialog::ShowLoongPearlRank(bool bShow)
{
	if (!m_pListBoxLoongPearl)
		return;

	m_pListBoxLoongPearl->SetVisible(bShow);

	if (bShow)
	{
		ReqAltarListLoongUserCmd cmd; //sky 请求指令
		SEND_USER_CMD(cmd);
	}
}

void CGuiTaiMiaoTopDialog::Update()
{
	if (m_pListBoxRankList) // 排行榜列表
	{
		int nIndex = m_pListBoxRankList->GetCurItem();
		//int nIndex = m_pListBoxRankList->GetSelectedIndex();
		GetRanksMgr().SetCurRanktaim(s_RankList[nIndex]);

		switch (s_RankList[nIndex])
		{
        	case RANK_TAIMIAO_TYPE_LOONGPEARL: // 太庙资源排行榜
			{				
				this->ShowLoongPearlRank(true);
			}
			break;
		default:
			break;
		}
	}
}
