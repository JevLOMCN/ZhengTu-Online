#include "public.h"
#include "GameGuiManager.h"
#include "game_script.h"
#include ".\GuiNameMoneyInputboxDialog.h"
#include "RoleItem.h"
#include "../luabind/public.h"

void CGuiNameMoneyInputBoxDialog::bind_lua(lua_State* L)
{
	using namespace luabind;
	CGuiDialog::bind_lua(L);
	module(L)
	[
		class_<CGuiNameMoneyInputBoxDialog,CGuiDialog>("CGuiNameMoneyInputBoxDialog")
			.def("OnMoneyInputBoxEnter",&CGuiNameMoneyInputBoxDialog::OnMoneyInputBoxEnter)
	];
	m_lua->SetGlobal("this",this);
}

#define NameMoney_SetNumber(pe,min,max)	{if(pe) { pe->SetMinNumber(min);pe->SetMaxNumber(max);pe->SetStyle(CGuiEditBox::GUI_ES_NUMBER); }}

CGuiNameMoneyInputBoxDialog::CGuiNameMoneyInputBoxDialog(const char* pszText1,const char* pszText2,enumGuiActionType type,void* pData)
{
	m_strText1 = pszText1;
	m_strText2 = pszText2;
	m_action = type;
	m_pData = pData;

	m_bSmDialog = true;
}

CGuiNameMoneyInputBoxDialog::~CGuiNameMoneyInputBoxDialog(void)
{
}

void CGuiNameMoneyInputBoxDialog::OnInitScript()
{
	FUNCTION_BEGIN;

	GameScriptInit(GetScriptEngine());
	FUNCTION_END;
}

void CGuiNameMoneyInputBoxDialog::OnClose(void)
{
	GetGameGuiManager()->m_guiNameMoneyInputDlg = NULL;
}

void CGuiNameMoneyInputBoxDialog::OnCreate()
{
	CGuiDialog::OnCreate();
	CGuiStatic* pText1 = GetStatic(1);
	AssertCreateCtrl( pText1,"namemoneyinputdlg出错" );
	pText1->SetText(m_strText1.c_str());

	m_pEditBox[0] = GetEditBox(2);
	m_pEditBox[1] = GetEditBox(3);
	m_pEditBox[2] = GetEditBox(4);
	AssertCreateCtrl( m_pEditBox[0],"namemoneyinputdlg出错" );
	AssertCreateCtrl( m_pEditBox[1],"namemoneyinputdlg出错" );
	AssertCreateCtrl( m_pEditBox[2],"namemoneyinputdlg出错" );

	NameMoney_SetNumber(m_pEditBox[0],0,100);
	NameMoney_SetNumber(m_pEditBox[1],0,99);
	NameMoney_SetNumber(m_pEditBox[1],0,99);

	m_pEditBox[0]->SetText("0");
	m_pEditBox[1]->SetText("0");
	m_pEditBox[2]->SetText("0");

	m_pNameEdit = GetEditBox(11);
	AssertCreateCtrl( m_pNameEdit,"namemoneyinputdlg出错" );
	m_pNameEdit->SetText("");

	CGuiStatic* pText2 = GetStatic(10);
	AssertCreateCtrl( pText2,"namemoneyinputdlg出错" );
	pText2->SetText(m_strText2.c_str());	
}

int CGuiNameMoneyInputBoxDialog::GetInputMoney()
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

const char* CGuiNameMoneyInputBoxDialog::GetInputName()
{
	FUNCTION_BEGIN;
	return m_pNameEdit->GetText();
	FUNCTION_END;
}

void CGuiNameMoneyInputBoxDialog::OnMoneyInputBoxEnter(bool bOk)
{
	switch(m_action)
	{
	case eAction_InputBailPrison:
		{
			if( bOk )
			{
				int money = GetInputMoney();
				if( money > 0 )
				{
					stBailPrison cmd;
					cmd.money = GetInputMoney();
					_snprintf(cmd.name,MAX_NAMESIZE,GetInputName());
					cmd.name[MAX_NAMESIZE-1] = 0;
					SEND_USER_CMD(cmd);
					Close();
					return ;
				}				
			}
			else
			{
				Close();
			}
		}
		break;
	default:
		break;
	}
}

