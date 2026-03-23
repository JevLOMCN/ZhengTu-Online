
#include "public.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include "./GuiBoardGameDlg.h"
#include "../gui/include/GuiManager.h"
#include "./Game.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "SafetyUser.h"
#include "./guinpcdialog.h"
#include "./CountryInfo.h"
#include "./GuiItem.h"
#include "./Country.h"
#include "./GuiTop.h"
#include "./GuiTaiMiaoTop.h"
#include "./GuiHalofFameTop.h"
///////////////////////////////////////////////////////////////////////////////
#define ID_BUTTON_GJZHSJ  3    // 国家综合实力榜
#define ID_BUTTON_JZBH    4    // 家族帮会排行榜
#define ID_BUTTON_GJYX    5    // 国家英雄排行榜
#define ID_BUTTON_XHPH    6    // 鲜花排行榜
#define ID_BUTTON_GRMC    7    // 个人名次排行榜
 
 
#define ID_BUTTON_CLOSE   1
///////////////////////////////////////////////////////////////////////////////

CGuiBoardGameDlg::CGuiBoardGameDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}
 
CGuiBoardGameDlg::~CGuiBoardGameDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuiBoardGameDlg::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
 
	FUNCTION_END;
}

bool CGuiBoardGameDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
		case ID_BUTTON_GJZHSJ:  // 国家综合实力榜
			{
				   //soke 排行榜
					//if( GetGameGuiManager()->m_guiTopDialog && GetGameGuiManager()->m_guiTopDialog->IsVisible() )
					//{
					//	GetGameGuiManager()->m_guiTopDialog->Close();
					//}
					//else
				//	{
					//	GetGameGuiManager()->AddTopDialog();
					//	GetGameGuiManager()->m_guiTopDialog->m_pListBoxRankList->SelectItem(1,true);
					//}
			//	GetGameGuiManager()->AddMessageBox("暂未开放国家综合实力榜");
				GetGameGuiManager()->AddTopDialog();
				GetGameGuiManager()->m_guiTopDialog->m_pListBoxRankList->SelectItem(0,true); //等级排行榜
			}
			break;
		case ID_BUTTON_JZBH:  //家族帮会排行榜
			{
			//	GetGameGuiManager()->AddMessageBox("暂未开放家族帮会排行榜");
				GetGameGuiManager()->AddTopDialog();
				GetGameGuiManager()->m_guiTopDialog->m_pListBoxRankList->SelectItem(1,true); //战力排行榜				
			
			}
			break;
		case ID_BUTTON_GJYX: // 国家英雄排行榜
			{                                   
				
				//GetGameGuiManager()->AddMessageBox("暂未开放国家英雄排行榜");
				GetGameGuiManager()->AddTopDialog();
				GetGameGuiManager()->m_guiTopDialog->m_pListBoxRankList->SelectItem(2,true); //今日护国榜

			}			
			break;
		case ID_BUTTON_XHPH: // 鲜花排行榜
			{
			  //GetGameGuiManager()->AddMessageBox("暂未开放鲜花排行榜");
				GetGameGuiManager()->AddTopDialog();
				GetGameGuiManager()->m_guiTopDialog->m_pListBoxRankList->SelectItem(3,true); //昨日护国榜
			}
			break;
		case ID_BUTTON_GRMC: // 个人名次排行榜
			{
			//add 2023-04-07
			
			//GetGameGuiManager()->AddMessageBox("暂未开放个人名次排行榜");
				if( GetGameGuiManager()->m_guiTopDialog && GetGameGuiManager()->m_guiTopDialog->IsVisible() )
				{
					GetGameGuiManager()->m_guiTopDialog->Close();
				}
				else
				{
					GetGameGuiManager()->AddTopDialog();
					GetGameGuiManager()->m_guiTopDialog->m_pListBoxRankList->SelectItem(4,true); //积分榜
					/* GetGameGuiManager()->m_guiTopDialog->ShowWorldLevelRank(false);
					GetGameGuiManager()->m_guiTopDialog->ShowLoongPearlRank(false);
					GetGameGuiManager()->m_guiTopDialog->ShowZhanLiRank(true); */
				}

			}
			break;
		case ID_BUTTON_CLOSE:
			{
			}
			break;
		}
		SetVisible(false);
		
		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}


void CGuiBoardGameDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiBoardGame = NULL;

	FUNCTION_END;
}
