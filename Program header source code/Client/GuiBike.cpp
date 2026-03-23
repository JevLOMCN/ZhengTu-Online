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
#include "./GuiBike.h"
#include "./RoleItem.h"
#include "./GameScene.h"
#include "Maincharacter.h"

CGuiBikeDlg::CGuiBikeDlg()
{
	FUNCTION_BEGIN;



	FUNCTION_END;
}

CGuiBikeDlg::~CGuiBikeDlg()
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}

void CGuiBikeDlg::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	itemtype1=0;
	itemtype2=0;

	item1=GetButton(2);
	item2=GetButton(3);

	FUNCTION_END;
}


bool CGuiBikeDlg::OnGuiEvent(UINT nEvent, UINT nID, CGuiControl *pControl)
{
	FUNCTION_BEGIN;

	if (nEvent == EVENT_BUTTON_CLICKED)
	{
		switch (nID)
		{
			case 2:
				{
					if(itemtype1!=0)
					{
						//先获取周围全部玩家
						std::vector< CCharacter*, std::stack_allocator<CCharacter*> > aCharacter;
						GetScene()->GetObjectOrderByDistance(aCharacter,type2type_t<CCharacter>());

						stBikeSkillCmd cmd;
					
						for(int i=0;i<aCharacter.size();i++)
						{

							if(i>=9)
							{
								break;
							}
							CCharacter* pChar = aCharacter[i];
							cmd.playerid[i] = pChar->GetID();
						}
						cmd.type = itemtype1-943;
						// cmd.itemID = pSrcItem->GetThisID();
						SEND_USER_CMD(cmd);

						itemtype1=0;
						item1->SetBitmap(stResourceLocation("data\\interfaces.gl",148,11),1); 
						item1->SetToolTips("访问赛车女郎获得卡片");
					}

				}
				break;
			case 3:
				{
					if(itemtype2!=0)
					{
						//先获取周围全部玩家
						std::vector< CCharacter*, std::stack_allocator<CCharacter*> > aCharacter;
						GetScene()->GetObjectOrderByDistance(aCharacter,type2type_t<CCharacter>());

						stBikeSkillCmd cmd;
					
						for(int i=0;i<aCharacter.size();i++)
						{

							if(i>=9)
							{
								break;
							}
							CCharacter* pChar = aCharacter[i];
							cmd.playerid[i] = pChar->GetID();
						}
						cmd.type = itemtype2-943;
						// cmd.itemID = pSrcItem->GetThisID();
						SEND_USER_CMD(cmd);


						itemtype2=0;
						item2->SetBitmap(stResourceLocation("data\\interfaces.gl",148,11),1); 
						item2->SetToolTips("访问赛车女郎获得卡片");
					}
				}
				break;
		}

		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent, nID, pControl);
	FUNCTION_END;
}

void CGuiBikeDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiBike = NULL;

	FUNCTION_END;
}

void CGuiBikeDlg::Clear(void)
{
	FUNCTION_BEGIN;

	itemtype1=0;
	item1->SetBitmap(stResourceLocation("data\\interfaces.gl",148,11),1); 
	item1->SetToolTips("访问赛车女郎获得卡片");

	itemtype2=0;
	item2->SetBitmap(stResourceLocation("data\\interfaces.gl",148,11),1); 
	item2->SetToolTips("访问赛车女郎获得卡片");

	FUNCTION_END;
}

void CGuiBikeDlg::Additem(int itemtype)
{
	if(itemtype1==0)
	{
		itemtype1=itemtype;
	}
	else if(itemtype2==0)
	{
		itemtype2=itemtype;
	}


	if(itemtype1!=0)
	{
		item1->SetBitmap(stResourceLocation("data\\interfaces.gl",148,itemtype1),1); 
		switch (itemtype1)
		{
			case 946://失明
				{
					item1->SetToolTips("失明");
				}
				break;
			case 947://混乱
				{
					item1->SetToolTips("混乱");
				}
				break;
			case 948://加速
				{
					item1->SetToolTips("加速");
				}
				break;
			case 949://定身
				{
					item1->SetToolTips("定身");
				}
				break;
			default:
				break;
		}
	}
	else{
		item1->SetBitmap(stResourceLocation("data\\interfaces.gl",148,11),1); 
		item1->SetToolTips("访问赛车女郎获得卡片");
	
	}

	if(itemtype2!=0)
	{
		item2->SetBitmap(stResourceLocation("data\\interfaces.gl",148,itemtype2),1); 
		switch (itemtype2)
		{
			case 946://失明
				{
					item2->SetToolTips("失明");
				}
				break;
			case 947://混乱
				{
					item2->SetToolTips("混乱");
				}
				break;
			case 948://加速
				{
					item2->SetToolTips("加速");
				}
				break;
			case 949://定身
				{
					item2->SetToolTips("定身");
				}
				break;
			default:
				break;
		}
	}
	else{
		item2->SetBitmap(stResourceLocation("data\\interfaces.gl",148,11),1); 
		item2->SetToolTips("访问赛车女郎获得卡片");
	}
	
}