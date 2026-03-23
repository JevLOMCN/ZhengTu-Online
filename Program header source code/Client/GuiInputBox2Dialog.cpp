#include "public.h"
#include "./GameGuiManager.h"
#include ".\GuiInputBox2Dialog.h"
#include "../luabind/public.h"
#include "GameScene.h"
#include "MainCharacter.h"

void CGuiInputBox2Dialog::bind_lua(lua_State* L)
{
	using namespace luabind;
	CGuiDialog::bind_lua(L);
	module(L)
	[
		class_<CGuiInputBox2Dialog,CGuiDialog>("CGuiInputBox2Dialog")
			.def("OnInputBoxEnter",&CGuiInputBox2Dialog::OnInputBoxEnter)
	];
	m_lua->SetGlobal("this",this);
}

CGuiInputBox2Dialog::CGuiInputBox2Dialog(const char* pszText1,const char* pszText2,enumGuiActionType type,void* pData)
: m_pData(pData)
, m_strText1(pszText1)
, m_strText2(pszText2)
, m_action(type)
, m_pEditBox1(NULL)
, m_pEditBox2(NULL)
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiInputBox2Dialog::~CGuiInputBox2Dialog(void)
{
}

void CGuiInputBox2Dialog::OnClose(void)
{
	GetGameGuiManager()->m_guiInputBox2Dlg = NULL;
}

void CGuiInputBox2Dialog::OnCreate()
{
	CGuiDialog::OnCreate();
	CGuiStatic* pText = GetStatic(1);
	AssertCreateCtrl(pText,"输入脚本2错误");
	pText->SetText(m_strText1.c_str());
	m_pEditBox1 = GetEditBox(2);
	AssertCreateCtrl(m_pEditBox1,"输入脚本2错误");

	CGuiStatic* pText1 = GetStatic(3);
	AssertCreateCtrl(pText1,"输入脚本2错误");
	pText1->SetText(m_strText2.c_str());
	m_pEditBox2 = GetEditBox(4);
	AssertCreateCtrl(m_pEditBox2,"输入脚本2错误");

	GetGuiManager()->RequestFocus(this);
	this->SetFocus((CGuiControl*)m_pEditBox1);
}

const char* CGuiInputBox2Dialog::GetText1()
{
	if(m_pEditBox1)
		return m_pEditBox1->GetText();
	return NULL;
}

const char* CGuiInputBox2Dialog::GetText2()
{
	if(m_pEditBox2)
		return m_pEditBox2->GetText();
	return NULL;
}

void CGuiInputBox2Dialog::OnInputBoxEnter(bool bOk)
{
	switch(m_action)
	{
	case 0:	break;
#ifdef __SPRING__
	case eAction_SendBlessMessage:
		{
			OnSendBlessMsg(bOk);
		}
		break;
#endif
	default:
		break;
	}
}

bool CGuiInputBox2Dialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		OnInputBoxEnter( nID == 5 );
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

void CGuiInputBox2Dialog::SetTextLength(int length1,int length2)
{
	if(m_pEditBox1)
		m_pEditBox1->SetMaxCharCount(length1);

	if(m_pEditBox2)
		m_pEditBox2->SetMaxCharCount(length2);
}

#ifdef __SPRING__
bool CGuiInputBox2Dialog::OnSendBlessMsg(bool bOk)
{
	if( bOk )
	{
		stChannelChatUserCmd cmd;
		cmd.dwType = CHAT_TYPE_BLESS_MSG;
		cmd.dwFromID = GetScene()->GetMainCharacter()->GetID();
		const char* pszName = GetText1();
		const char* pszChat = GetText2();
		if( strlen(pszName) < 1 || strlen(pszChat) < 1 )
		{
			return false;
		}
		strncpy(cmd.pstrName,pszName,sizeof(cmd.pstrName));	//收件人的名字
		strncpy(cmd.pstrChat,pszChat,sizeof(cmd.pstrChat));	//消息内容
		cmd.size = 0;

		SEND_USER_CMD(cmd);
		Close();
		return true;
	}
	else
	{
		Close();
	}
	return false;
}
#endif