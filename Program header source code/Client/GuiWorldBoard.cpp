/**
*\file		GuiWorldBoard.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2005-7-5   17:12:00 中国标准时间
*\brief	    公告查看,写入对话框
*
* 
*
*/

#include "public.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GameTime.h"
#include ".\guiworldboard.h"
#include "./GuiMessageBox.h"
#include "game_script.h"


/**
 * \brief 公告查看,写入对话框构造函数
 * 
 * 公告查看,写入对话框构造函数
 * 
 * \return 无
 */
CGuiWorldBoard::CGuiWorldBoard(const char* pszCaption,const char* pszTitle,const char* pszText,
	enumGuiActionType type,bool bEdit,void* pData)
	:m_strText(pszText)
	,m_strTitle(pszTitle)
	,m_strCaption(pszCaption)
	,m_bEdit(bEdit)
	,m_action(type)
	,m_pData(pData)
{
	FUNCTION_BEGIN;

	

	FUNCTION_END;
}


/**
 * \brief 公告查看,写入对话框关闭函数
 * 
 * 公告查看,写入对话框关闭函数
 * 
 * \return 无
 */
void CGuiWorldBoard::OnClose(void)
{
	FUNCTION_BEGIN;

	GetQueryManager()->DeleteListData(m_action,m_bEdit);

	FUNCTION_END;
}


/**
 * \brief 创建公告查看,写入对话框
 * 
 * 创建公告查看,写入对话框
 * 
 * \return 无
 */
void CGuiWorldBoard::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	CGuiStatic* pText = GetStatic(1);
	Assert(pText);
	pText->SetText(m_strTitle.c_str());
	SetCaptionText(m_strCaption.c_str());
	m_pEditBox = GetMLEditBox(2);
	if (!m_bEdit)
	{
		m_pEditBox->SetStyle(0x11);
		m_pEditBox->SetText(m_strText.c_str());
		SetIcon(&(stResourceLocation("data\\interfaces.gl",22,3)));
		this->RequestFocus(GetButton(5));
	}
	else
	{
		SetIcon(&(stResourceLocation("data\\interfaces.gl",22,8)));
	}

	

	FUNCTION_END;
}

/**
 * \brief 公告查看,写入对话框消息处理函数
 * 
 * 公告查看,写入对话框消息处理函数
 * 
 * \param nEvent : 消息事件
 * \param nID : 发消息的控件ID
 * \param pControl : 发消息的控件指针
 * \return 消息处理结果
 */
bool CGuiWorldBoard::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent) {
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 5:
				if (OnWorldBoardEnter())
					Close();
				break;

			case -1:
				
				break;
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

/**
 * \brief 发送公告消息的检查
 * 
 * 发送公告消息的检查
 * 
 * \return 是否发送了消息
 */
bool CGuiWorldBoard::OnWorldBoardEnter()
{
	FUNCTION_BEGIN;

	if (!m_bEdit)
		return true;
	

	switch(m_action) {
	case eAction_TongBoard:
		{
			return SendBoard(CHAT_TYPE_UNION_AFFICHE);
		}
		break;
	case eAction_SchoolBoard:
		{	
			return SendBoard(CHAT_TYPE_OVERMAN_AFFICHE);
		}
		break;
	case eAction_SeptBoard:
		{
			return SendBoard(CHAT_TYPE_FAMILY_AFFICHE);
		}
		break;
	case eAction_FriendBoard:
		{	
			return SendBoard(CHAT_TYPE_FRIEND_AFFICHE);
		}
		break;
	case eAction_TongExplain:
		{
			stNoteUnionCmd  cmd;
	
			strncpy(cmd.noteBuf,m_pEditBox->GetText(),sizeof(cmd.noteBuf));
			SEND_USER_CMD(cmd);
			return true;
		}
		break;
	case eAction_SeptExplain:
		{
			stNoteSeptCmd  cmd;

			strncpy(cmd.noteBuf,m_pEditBox->GetText(),sizeof(cmd.noteBuf));
			SEND_USER_CMD(cmd);
			return true;
		}
		break;
	}
	return false;

	FUNCTION_END;
}


/**
 * \brief 发送公告消息
 * 
 * 发送公告消息
 * 
 * \param state : 公告类型
 * \return 是否发送成功
 */
bool CGuiWorldBoard::SendBoard(byte state)
{
	FUNCTION_BEGIN;

	const char* szText = m_pEditBox->GetText();
	if(szText[0])
	{
		stChannelChatUserCmd cmd;
		
		cmd.dwType = state;
		strncpy(cmd.pstrName, GetScene()->GetMainCharacter()->GetName(),sizeof(cmd.pstrName));
		strncpy(cmd.pstrChat,szText,sizeof(cmd.pstrChat));

		SEND_USER_CMD(cmd);
		return true;
	}
	return false;

	FUNCTION_END;
}