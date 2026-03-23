/**
*\file		GuiTrade.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    玩家交易界面对话框
*
* 
*
*/

#include "public.h"
#include "./GameGuiManager.h"
#include ".\guitrade.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GameState.h"
#include "./GameCursor.h"
#include "./Game.h"
#include "./GuiItem.h"
#include "./GuiMoneyInputBox.h"
#include "./RoleItem.h"


void AddTrade(const char* szName)
{
	CCharacter* pCharacter = GetScene()->FindCharacterByName(szName);
	if (pCharacter==NULL)
	{
		GetGameGuiManager()->AddClientSystemMessage("距离太远，无法交易！");
		return;
	}

	stRequestTradeUserCmd cmd;
	cmd.dwAskerTempID = GetScene()->GetMainCharacter()->GetID();
	cmd.dwAnswerTempID = pCharacter->GetID();
	SEND_USER_CMD(cmd);

}
//#define _SET_CONTROL_LOCATION

/**
 * \brief 构造函数
 * 
 * 构造函数
 * 
 * \return 无
 */
CGuiTrade::CGuiTrade(void)
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;

	FUNCTION_END;
}

/**
 * \brief 关闭交易对话框
 * 
 * 关闭交易对话框
 * 
 * \return 无
 */
void CGuiTrade::OnClose(void)
{
	FUNCTION_BEGIN;
	
	GetGameGuiManager()->m_guiTrade = NULL;

	FUNCTION_END;
}

void CGuiTrade::OnSetCloseLocation()
{
	CGuiDialog::OnSetCloseLocation();


	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x,pt.y+24);
	}

}


	


/**
 * \brief 创建对话框
 * 
 * 创建对话框
 * 
 * \return 无
 */
void CGuiTrade::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_pTargetName = GetStatic(11);
	
	
	GetItemTable(OBJECTCELLTYPE_TRADE) = GetTable(1);
	GetItemTable(OBJECTCELLTYPE_TRADE)->m_iTableType = OBJECTCELLTYPE_TRADE;
	GetItemTable(OBJECTCELLTYPE_OTHERTRADE) = GetTable(4);
	GetItemTable(OBJECTCELLTYPE_OTHERTRADE)->m_iTableType = OBJECTCELLTYPE_OTHERTRADE;
	GetItemTable(OBJECTCELLTYPE_OTHERTRADE)->SetEnabled(false);

	m_nDy = GetItemTable(OBJECTCELLTYPE_TRADE)->GetY() - 3;

	GetMoneyEdit(OBJECTCELLTYPE_TRADE) = GetEditBox(2);
	GetMoneyEdit(OBJECTCELLTYPE_OTHERTRADE) = GetEditBox(5);

	GetMoneyButton(OBJECTCELLTYPE_TRADE) = GetButton(3);
	//GetMoneyButton(OBJECTCELLTYPE_OTHERTRADE) = GetButton(6);
	//GetMoneyButton(OBJECTCELLTYPE_OTHERTRADE)->SetEnabled(false);

	GetCommitButton(OBJECTCELLTYPE_TRADE) = GetButton(8);
	GetCommitButton(OBJECTCELLTYPE_OTHERTRADE) = GetButton(9);
	GetCommitButton(OBJECTCELLTYPE_OTHERTRADE)->SetEnabled(false);
	GetCommitButton(OBJECTCELLTYPE_OTHERTRADE)->SetVisible(false);

	

	GetMoneyEdit(OBJECTCELLTYPE_TRADE)->SetEnabled(false);
	GetMoneyEdit(OBJECTCELLTYPE_OTHERTRADE)->SetEnabled(false);


	InitData();

	
	FUNCTION_END;
}

void CGuiTrade::InitData()
{
	GetMoneyEdit(OBJECTCELLTYPE_OTHERTRADE)->SetText(Money2String(0));
	GetMoneyEdit(OBJECTCELLTYPE_TRADE)->SetText(Money2String(0));

	for (int i=0;i<m_listItem.size();i++)
	{
		m_listItem[i]->OnRemove();
		m_allocRoleItem.freePointer(m_listItem[i]);
	}
	m_listItem.clear();

	CGuiTable * pTable = GetItemTable(OBJECTCELLTYPE_TRADE);

	std::vector<CRoleItem*> aItem;
	for (int i=0;i<pTable->m_listItem.size();i++)
		aItem.push_back((CRoleItem*)pTable->m_listItem[i]->m_pItemData);

	for(size_t i=0;i<aItem.size();++i)
		aItem[i]->DestoryAllVirtualCell();

	GetItemTable(OBJECTCELLTYPE_TRADE)->ClearAll();
	GetItemTable(OBJECTCELLTYPE_OTHERTRADE)->ClearAll();

	m_bCommit = false;
	GetItemTable(OBJECTCELLTYPE_TRADE)->SetEnabled(true);
	GetMoneyButton(OBJECTCELLTYPE_TRADE)->SetEnabled(false);

	OnItemChange();
}

/**
 * \brief 开始交易处理
 * 
 * 开始交易处理
 * 
 * \param pCmd : 开始交易消息
 * \return 无
 */
void CGuiTrade::BeginTrade(stBeginTradeUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	if(pCmd->dwAskerTempID != GetScene()->GetMainCharacter()->GetID())
		m_dwOtherID = pCmd->dwAskerTempID;
	else
		m_dwOtherID = pCmd->dwAnswerTempID;
	CCharacter* pChar = GetScene()->FindCharacterByID(m_dwOtherID);
	m_pTargetName->SetText(pChar->GetName());
	GetStatic(12)->SetText(GetScene()->GetMainCharacter()->GetName());
	

	InitData();
//#ifndef _SET_CONTROL_LOCATION
//	GetCommitButton(OBJECTCELLTYPE_OTHERTRADE)->SetVisible(false);
//#endif
//	GetCommitButton(OBJECTCELLTYPE_TRADE)->SetVisible(true);

	
	FUNCTION_END;
}

/**
 * \brief 设置对方交易的内容可见
 * 
 * 设置对方交易的内容可见
 * 
 * \return 无
 */
void CGuiTrade::OtherCommitTrade()
{
	FUNCTION_BEGIN;

	GetCommitButton(OBJECTCELLTYPE_OTHERTRADE)->SetVisible(true);
	GetItemTable(OBJECTCELLTYPE_TRADE)->SetEnabled(false);
	GetMoneyButton(OBJECTCELLTYPE_TRADE)->SetEnabled(false);

	if(g_pMoneyInputBox)
	{
		((CGuiMoneyInputBox*)g_pMoneyInputBox)->Close();
	}

	FUNCTION_END;
}

/**
 * \brief 取消交易
 * 
 * 取消交易
 * 
 * \return 无
 */
void CGuiTrade::CancelTrade()
{
	FUNCTION_BEGIN;

	InitData();

	GetGameState()->SetStateItem(eGameStateItem_Normal);
	
	SetVisible(false);
	//GetScene()->GetMainCharacter()->RemoveItemByLocationType(OBJECTCELLTYPE_OTHERTRADE);
	
	if(GetGameGuiManager()->m_guiItem)
	{
		GetGameGuiManager()->m_guiItem->SetVisible(false);
	}
	if(g_pMoneyInputBox)
	{
		((CGuiMoneyInputBox*)g_pMoneyInputBox)->Close();
	}

	FUNCTION_END;
}

/**
 * \brief 结束交易
 * 
 * 结束交易
 * 
 * \return 无
 */
void CGuiTrade::FinalTrade()
{
	FUNCTION_BEGIN;

	GetGameState()->SetStateItem(eGameStateItem_Normal);

	SetVisible( false );
	/*GetScene()->GetMainCharacter()->RemoveItemByLocationType(OBJECTCELLTYPE_TRADE);
	GetScene()->GetMainCharacter()->SwapItemByLocationType(OBJECTCELLTYPE_OTHERTRADE,OBJECTCELLTYPE_TRADE);*/
	GetGameGuiManager()->m_guiItem->SetVisible(false);



	CGuiTable * pTable = GetItemTable(OBJECTCELLTYPE_TRADE);

	std::vector<CRoleItem*> aItem;
	for (int i=0;i<pTable->m_listItem.size();i++)
		aItem.push_back((CRoleItem*)pTable->m_listItem[i]->m_pItemData);

	for(size_t i=0;i<aItem.size();++i)
	{
		stRemoveObjectPropertyUserCmd cmd;
		cmd.qwThisID = aItem[i]->GetThisID();
		GetScene()->GetMainCharacter()->RemoveItem(&cmd);
	}

	GetItemTable(OBJECTCELLTYPE_TRADE)->ClearAll();


	GetGameGuiManager()->m_guiItem->SetMoney(GetGameGuiManager()->m_guiItem->GetMoney()
		+GetMoney(OBJECTCELLTYPE_OTHERTRADE)-GetMoney(OBJECTCELLTYPE_TRADE));

	InitData();



	FUNCTION_END;
}

/**
 * \brief 交易物品改变的控件状态改变
 * 
 * 交易物品改变的控件状态改变
 * 
 * \return 无
 */
void CGuiTrade::OnItemChange()
{
	FUNCTION_BEGIN;

	GetCommitButton(OBJECTCELLTYPE_OTHERTRADE)->SetVisible(false);
	GetCommitButton(OBJECTCELLTYPE_TRADE)->SetEnabled(true);

	FUNCTION_END;
}

/**
 * \brief 从交易银子框中取回银子
 * 
 * 从交易银子框中取回银子
 * 
 * \return 无
 */
inline void OnMoveMoney()
{
	FUNCTION_BEGIN;

	if(g_pMoneyInputBox) return;

	const char* pText ;
	DWORD dwLocationDst;
	pText = "要取回多少银子？";
	dwLocationDst = OBJECTCELLTYPE_COMMON;

	//CRoleItem* pSrcItem = GetScene()->GetMainCharacter()->GetMoneyItem(OBJECTCELLTYPE_TRADE);
	//if(!pSrcItem) return;

	static stSplitItemData st;

	st.qwThisID = 0;//pSrcItem->GetThisID();
	st.dwCurNum = GetGameGuiManager()->m_guiTrade->GetMoney(OBJECTCELLTYPE_TRADE);//pSrcItem->GetCount();
	st.dst.dwLocation = dwLocationDst;
	st.dst.dwTableID = 0;
	st.dst.x = -1;
	st.dst.y = -1;

	CGuiMoneyInputBox* pBox = GetGameGuiManager()->AddMoneyInputBox(pText,eAction_SplitItem,&st);
	//pBox->SetNumber(0,pSrcItem->GetCount());
	CRoleItem::s_bCanMoveItem = false;
	g_pMoneyInputBox = (void*) pBox;

	FUNCTION_END;
}


/**
 * \brief 交易界面的消息处理
 * 
 * 交易界面的消息处理
 * 
 * \param nEvent : 消息类型
 * \param nID : 控件类型ID
 * \param pControl : 控件指针
 * \return 消息处理结果
 */
bool CGuiTrade::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		if( pControl == GetCommitButton(OBJECTCELLTYPE_TRADE) )
		{
			// 点击确定按钮
			stCommitTradeUserCmd cmd;
			cmd.dwUserTempID = GetScene()->GetMainCharacter()->GetID();
			SEND_USER_CMD(cmd);
			GetCommitButton(OBJECTCELLTYPE_TRADE)->SetEnabled(false);
			GetItemTable(OBJECTCELLTYPE_TRADE)->SetEnabled(false);
			GetMoneyButton(OBJECTCELLTYPE_TRADE)->SetEnabled(false);

			m_bCommit = true;

			if(g_bSinglePlayer)
			{
				stFinishTradeUserCmd cmd1;
				GetClient()->PushCommand(&cmd1,sizeof(cmd1));
			}
			return true;
		}
		else if(  nID == GUI_CLOSEBOX_ID)
		{
			// 点击取消按钮
			if(GetGameState()->IsStateItem(eGameStateItem_TradeRun))
			{
				stCancelTradeUserCmd cmd;
				cmd.dwUserTempID = GetScene()->GetMainCharacter()->GetID();
				SEND_USER_CMD(cmd);

				
				if(g_bSinglePlayer)
				{
					stCancelTradeUserCmd cmd1;
					cmd1.dwUserTempID = GetScene()->GetMainCharacter()->GetID();
					GetClient()->PushCommand(&cmd1,sizeof(cmd1));
				}
			}
			else
			{

				GetGameCursor()->SetCursor( CCursor::CursorType_Normal, eCursorNormal);
				SetVisible(false);
			}

			CancelTrade();
			return true;
		}
		else if( pControl == GetMoneyButton(OBJECTCELLTYPE_TRADE) )
		{
			// 点击钱的按钮
			OnMoveMoney();
			return true;
		}
		/*
		else if( pControl == m_pBtnSelect)
		{
			// 点击选择用户按钮
			if(GetGameState()->IsStateItem(eGameStateItem_Normal))
				GetGameCursor()->SetCursor(CCursor::CursorType_Select,eCursorTrade);
			return true;
		}
		*/

		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

/**
 * \brief 设置交易银子的数量
 * 
 * 设置交易银子的数量
 * 
 * \param dwLocation : 对方或自己的交易银子内容
 * \param dwMoney : 银子数量
 * \return 无
 */
void CGuiTrade::SetMoney(DWORD dwLocation,DWORD dwMoney)
{
	FUNCTION_BEGIN;

	char szText[32];
	sprintf(szText,"%u",dwMoney);
	//GetMoneyEdit(dwLocation)->SetText(szText);
	GetMoneyEdit(dwLocation)->SetText(Money2String(dwMoney));

	if (dwLocation == OBJECTCELLTYPE_TRADE)
	{
		CRoleItem* pSrcItem = GetScene()->GetMainCharacter()->GetMoneyItem(OBJECTCELLTYPE_COMMON);
		if(!pSrcItem) return;


		

		stAddObjectTradeUserCmd cmd;
		memcpy(&(cmd.object) , pSrcItem->GetObject() , sizeof(cmd.object));
		cmd.object.dwNum = dwMoney;
		cmd.user_id = GetScene()->GetMainCharacter()->GetID();
		cmd.x = 0;
		cmd.y = 0;

		SEND_USER_CMD(cmd);
	}
	FUNCTION_END;
}

bool CGuiTrade::IsEdit()
{
	if ((m_bCommit)||(GetCommitButton(OBJECTCELLTYPE_OTHERTRADE)->IsVisible()))
		return false;

	return true;
}
/**
 * \brief 得到交易银子的数量
 * 
 * 得到交易银子的数量
 * 
 * \param dwLocation : 对方或自己的交易银子内容
 * \return 得到交易银子的数量
 */
DWORD CGuiTrade::GetMoney(DWORD dwLocation)
{
	FUNCTION_BEGIN;

	//return strtoul(GetMoneyEdit(dwLocation)->GetText(),NULL,10);

	return String2Money(GetMoneyEdit(dwLocation)->GetText());

	FUNCTION_END;
}

/**
 * \brief 初始化交易脚本函数
 * 
 * 初始化交易脚本函数
 * 
 * \return 无
 */
void CGuiTrade::OnInitScript()
{
	FUNCTION_BEGIN;
	using namespace luabind;
	module(GetScriptEngine())
	[
		def("OnMoveMoney",&::OnMoveMoney)
	];

	FUNCTION_END;
}

/**
 * \brief 初始化交易图标
 * 
 * 初始化交易图标
 * 
 * \return 无
 */
void CGuiTrade::InitIcon()
{
	FUNCTION_BEGIN;

	//stResourceLocation rl(GetGuiGraphicPackName(),22,21 + GetCharTypeInfo(GetScene()->GetMainCharacter()->GetType()).background);
	//SetIcon(&rl);

	FUNCTION_END;
}

void CGuiTrade::InsertObject(t_Object* pObject)
{
	//OnItemChange();

	if (pObject->dwObjectID == 665)
	{
		SetMoney(OBJECTCELLTYPE_OTHERTRADE,pObject->dwNum);
		return;
	}

	CRoleItem* pItem = LOOKASIDE_ALLOC(m_allocRoleItem);
	stAddObjectPropertyUserCmd  cmd;
	memcpy(&(cmd.object),pObject,sizeof(t_Object));


	if( !pItem->SetAttrib(&cmd) )
	{
		m_allocRoleItem.freePointer(pItem);
		return;
	}
	m_listItem.push_back(pItem);

}

void CGuiTrade::DeleteObject(DWORD thisID)
{
	//OnItemChange();

	for(std::vector<CRoleItem*>::iterator it = m_listItem.begin();it != m_listItem.end();++it)
	{
		if ((*it)->GetThisID() == thisID)
		{
			(*it)->OnRemove();
			m_allocRoleItem.freePointer((*it));
			m_listItem.erase(it);
			break;
		}
	}

}