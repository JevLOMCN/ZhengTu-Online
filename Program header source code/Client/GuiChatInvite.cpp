/**
*\file		GuiChatInvite.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2005-7-5   17:12:00 中国标准时间
*\brief	    组聊邀请或聊天选择对话框
*
* 
*
*/

#include "public.h"
#include "./GameGuiManager.h"
#include ".\guichatinvite.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiPrivateChat.h"
#include "./GuiWorldConn.h"
#include "./Chat.h"

/**
 * \brief 构造函数
 * 
 * 构造函数
 * 
 * \return 无
 */
CGuiChatInvite::CGuiChatInvite(tChatInvite* pInvite)
:m_pRadio1(NULL),
m_pRadio2(NULL),
m_pRadio3(NULL),
m_pRadio4(NULL),
m_pRadio5(NULL),
m_pRadio6(NULL),
m_pLabel(NULL)
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;
	m_pChatInvite = pInvite;

	FUNCTION_END;
}


/**
 * \brief 创建对话框
 * 
 * 创建对话框
 * 
 * \return 无
 */
void CGuiChatInvite::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();	

	m_pRadio3 = GetRadioButton(3);
	m_pRadio4 = GetRadioButton(4);
	m_pRadio5 = GetRadioButton(5);
	m_pRadio3->SetChecked(TRUE,TRUE);
	m_pLabel = GetStatic(6);

	FUNCTION_END;
}

void CGuiChatInvite::OnClose(void)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

bool CGuiChatInvite::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		switch(nID)
		{
		case 1:
			{
				if ( m_pChatInvite )
				{
					if (m_pRadio3->GetChecked())
					{
						stJoin_ChannelUserCmd cmd;
						strncpy(cmd.name,GetScene()->GetMainCharacter()->GetName(),MAX_NAMESIZE);
						cmd.dwChannelID = m_pChatInvite->dwChannelID;
						SEND_USER_CMD(cmd);			
						GetChatManager().ShowChatDialogAllwith(m_pChatInvite->dwChannelID,m_pChatInvite->name);
						GetChatManager().RemoveInvite( m_pChatInvite );
					}

					if (m_pRadio4->GetChecked())
					{
						GetChatManager().RemoveChannel( m_pChatInvite->dwChannelID );
						GetChatManager().RemoveInvite( m_pChatInvite );
					}

					if (m_pRadio5->GetChecked())
					{
						m_pChatInvite->fKeepTime = 60;
						m_pChatInvite->fCurrTime = 0;
					}
				}
				Close();
				return true;
			}
		case 2:	// 
			{
				Close();
				return true;
			}
			break;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

/**
 * \brief 设置标题
 * 
 * 设置标题
 * 
 * \param title : 标题字符串
 * \return 无
 */
void CGuiChatInvite::SetTitleInfo(const char* title)
{
	FUNCTION_BEGIN;

	if (m_pLabel)
	{
		m_pLabel->SetText(title);
	}

	FUNCTION_END;
}