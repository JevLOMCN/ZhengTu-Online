/**
*\file		GuiMoneyInputBox.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    金钱输入对话框
*
* 
*
*/


#include "public.h"
#include "miniusercommand.h"
#include "GameGuiManager.h"
#include "game_script.h"
#include "./GuiItem.h"
#include "./RoleItem.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include ".\guimoneyinputbox.h"
#include "./GuiTrade.h"
#include "./GuiShopDialog.h"
#include "./GuiSendMailDialog.h"
#include "../luabind/public.h"

void CGuiMoneyInputBox::bind_lua(lua_State* L)
{
	using namespace luabind;
	CGuiDialog::bind_lua(L);
	module(L)
	[
		class_<CGuiMoneyInputBox,CGuiDialog>("CGuiMoneyInputBox")
			.def("OnMoneyInputBoxEnter",&CGuiMoneyInputBox::OnMoneyInputBoxEnter)
	];
	m_lua->SetGlobal("this",this);
}
/**
 * \brief 构造函数
 * 
 * 构造函数
 * 
 * \return 无
 */
CGuiMoneyInputBox::CGuiMoneyInputBox(const char* pszText,enumGuiActionType type,void* pData)
: m_strText(pszText)
, m_action(type)
, m_pData(pData)
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;
	m_pEditBox[0] = NULL;
	m_pEditBox[1] = NULL;
	m_pEditBox[2] = NULL;

	FUNCTION_END;
}

/**
 * \brief 得到输入金钱数量
 * 
 * 得到输入金钱数量
 * 
 * \return 无
 */
int  CGuiMoneyInputBox::GetMoneyNum()
{	
	FUNCTION_BEGIN;

	char szTxt[32]={0};
	sprintf(szTxt,"%d%s%d%s%d%s",strtoul(m_pEditBox[0]->GetText(),NULL,10),INGOT_TXT,
		strtoul(m_pEditBox[1]->GetText(),NULL,10),TAEL_TXT,
		strtoul(m_pEditBox[2]->GetText(),NULL,10),CENT_TXT);

	int nMoney= String2Money(szTxt);

	return nMoney;

	FUNCTION_END;
}

/**
 * \brief 设置金钱数量大小
 * 
 * 设置金钱数量大小
 * 
 * \param pEditBox : 编辑框
 * \param minNumber : 最小值
 * \param maxNumber : 最大值
 * \return 无
 */
void CGuiMoneyInputBox::SetNumber(CGuiEditBox* pEditBox,int minNumber,int maxNumber)
{
	FUNCTION_BEGIN;
	
	if (pEditBox)
	{
		pEditBox->SetMinNumber(minNumber);
		pEditBox->SetMaxNumber(maxNumber);
		pEditBox->SetStyle(CGuiEditBox::GUI_ES_NUMBER);
	}

	FUNCTION_END;
}

/**
 * \brief 关闭对话框
 * 
 * 关闭对话框
 * 
 * \return 无
 */
void CGuiMoneyInputBox::OnClose(void)
{
	FUNCTION_BEGIN;
	
	if(g_pMoneyInputBox == this)
		g_pMoneyInputBox = NULL;

	if( GetGameGuiManager()->m_guiSendMailDlg )
	{
		GetGameGuiManager()->m_guiSendMailDlg->m_pMoneyInputDlg = NULL;			
	}

	FUNCTION_END;
}

/**
 * \brief 对话框创建
 * 
 * 对话框创建
 * 
 * \return 无
 */
void CGuiMoneyInputBox::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	CGuiStatic* pText = GetStatic(1);
	Assert(pText);
	pText->SetText(m_strText.c_str());

	m_pEditBox[0] = GetEditBox(2);
	m_pEditBox[1] = GetEditBox(3);
	m_pEditBox[2] = GetEditBox(4);

	SetNumber(m_pEditBox[0],0,100);
	SetNumber(m_pEditBox[1],0,99);
	SetNumber(m_pEditBox[2],0,99);

	m_pEditBox[0]->SetText("0");
	m_pEditBox[1]->SetText("0");
	m_pEditBox[2]->SetText("0");

	FUNCTION_END;
}


void CGuiMoneyInputBox::OnGiveUnionMoney(bool bQuery)
{
	DWORD dwNum = GetMoneyNum();

	if (GetGameGuiManager()->m_guiItem)
	{
		if (dwNum>GetGameGuiManager()->m_guiItem->GetMoney())
		{
			GameMessageBox("无足够的金钱!");
			return;
		}
	}

	if (bQuery)
	{
		char  szTxt[MAX_PATH]={0};

		sprintf(szTxt,"你是否愿意捐献：%s ? \n这次捐献将增加你功勋值：%d点! ",Money2String(dwNum),dwNum/2000);
		GetGameGuiManager()->AddMessageBox(szTxt,eAction_GiveUnoinMoney,(void*)this,c_szMBOKCancel);
	}
	else
	{
		stContributeUnion  cmd;
		cmd.dwMoney = dwNum;

		SEND_USER_CMD(cmd);

		Close();
	}
}
/**
 * \brief 金钱输入确定按钮
 * 
 * 金钱输入确定按钮
 * 
 * \param bOk : 是否是确定按钮
 * \return 无
 */
void CGuiMoneyInputBox::OnMoneyInputBoxEnter(bool bOk)
{
	FUNCTION_BEGIN;

	switch(m_action)
	{
	case eAction_GiveUnoinMoney:
		{
			if(bOk)
			{
				OnGiveUnionMoney();
			}
			else
			{
				Close();
			}

		}
		break;

	case eAction_SplitItem:		// 分裂物品
		{
			if(bOk)
			{
				stSplitItemData* st = (stSplitItemData*)m_pData;
				Assert(st);
				//CRoleItem* pSrcItem = GetScene()->GetMainCharacter()->FindItemByID(st->qwThisID);
				
				DWORD dwNum = GetMoneyNum();

				if( dwNum && dwNum <= st->dwCurNum )
				{
					switch(st->dst.dwLocation) {
					case OBJECTCELLTYPE_TRADE:
						{
							if (GetGameGuiManager()->m_guiTrade)
							{
								GetGameGuiManager()->m_guiTrade->SetMoney(OBJECTCELLTYPE_TRADE,
									GetGameGuiManager()->m_guiTrade->GetMoney(OBJECTCELLTYPE_TRADE) + dwNum);
							}
						}
						break;
					case OBJECTCELLTYPE_COMMON:
						{
							if (GetGameGuiManager()->m_guiTrade)
							{
								GetGameGuiManager()->m_guiTrade->SetMoney(OBJECTCELLTYPE_TRADE,
									GetGameGuiManager()->m_guiTrade->GetMoney(OBJECTCELLTYPE_TRADE) - dwNum);
							}
						}
						break;
					}
				}
				else
				{
					if (dwNum > st->dwCurNum )
					{
						GameMessageBox("无足够的银子!");
						return;
					}

				}
			}
			Close();
		}
		CRoleItem::s_bCanMoveItem = true;
		break;
	case eAction_EnterShopSalePrice:
		{			
			if( bOk )
			{
				if( GetGameGuiManager()->m_guiShop )
				{
					DWORD dwPrice = GetMoneyNum();
					CRoleItem* pItem = (CRoleItem*)m_pData;
					Assert(pItem);
					pItem->SetShopPrice(dwPrice);
					GetGameGuiManager()->m_guiShop->AddItem(pItem);
				}
			}
			
			Close();
		}
		break;
	case eAction_InputMailSendMoney:
		{
			if( GetGameGuiManager()->m_guiSendMailDlg )
			{
				int iMoney = 0;
				if( bOk )
				{
					iMoney = GetMoneyNum();
				}
				GetGameGuiManager()->m_guiSendMailDlg->SetSendMoney(iMoney);
			}
			Close();
		}
		break;
	case eAction_InputMailSendGoldCoin:
		{
			if( GetGameGuiManager()->m_guiSendMailDlg )
			{
				int iGold = 0;
				if( bOk )
				{
					iGold = GetMoneyNum();
				}
				GetGameGuiManager()->m_guiSendMailDlg->SetSendGoldCoin(iGold);
			}
			Close();
		}
		break;
	case eAction_InputBribePrisonMoney:
		{
			if( bOk )
			{
				int iMoney = GetMoneyNum();
				stBribePrison cmd;
				cmd.money = iMoney;
				SEND_USER_CMD(cmd);				
			}
			Close();
		}
		break;
	case eAction_MiniGame_MoneyInput:
		{
			if( bOk )
			{
				int iMoney = GetMoneyNum();
				
				if ((iMoney < 100) || (iMoney > 10000))
				{
					GetGameGuiManager()->AddMessageBox("投注金额只能在1两和1锭之间");
					break;
				}
				DWORD id = (DWORD)m_pData;
				stInviteMiniGame cmd;
				cmd.money = iMoney;
				cmd.tempid = id;
				SEND_USER_CMD(cmd);	
				TRACE("---wyc---邀请[tempid: %d]进行小游戏,并且投注金额为[iMoney: %d]--",id,iMoney);
				Close();
			}
			else
			{
				Close();
			}			
		}
		break;
	}

	FUNCTION_END;
}

/**
 * \brief 注册脚本函数
 * 
 * 注册脚本函数
 * 
 * \return 无
 */
void CGuiMoneyInputBox::OnInitScript()
{
	FUNCTION_BEGIN;

	GameScriptInit(GetScriptEngine());

	FUNCTION_END;
}

void CGuiMoneyInputBox::SetDefaultMoney(int money)
{
	FUNCTION_BEGIN;
	char sz[32] = {0};
#define EditBox_SetNum(p,n)	{ _snprintf(sz,32,"%d",n);sz[31]=0;p->SetText(sz);}

	EditBox_SetNum( m_pEditBox[0],GETMONEYINGOT(money) );
	EditBox_SetNum( m_pEditBox[1],GETMONEYTAEL (money) );
	EditBox_SetNum( m_pEditBox[2],GETMONEYCENT (money) );
}