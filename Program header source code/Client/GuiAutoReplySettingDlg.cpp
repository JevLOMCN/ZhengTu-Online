#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GuiAutoReplySettingDlg.h"
#include "./RoleItem.h"
#include "./GuiMiniMap.h"
#include "./GameAppation.h"

CGuiAutoReplySettingDlg::CGuiAutoReplySettingDlg(void)
{
	m_pMenu = NULL;
}

CGuiAutoReplySettingDlg::~CGuiAutoReplySettingDlg(void)
{
}

void CGuiAutoReplySettingDlg::OnCreate(void)
{
	CGuiDialog::OnCreate();

	m_pMLEditBox = GetMLEditBox( 1 );
	if( !m_pMLEditBox )
	{
		GetGameGuiManager()->AddMessageBox("自动回复设置脚本出错");
		Engine_WriteLog("自动回复设置脚本出错");
		Close();
		return ;
	}
	//const char* pszText = GetGameApplication()->GetConfig()->szAutoReply;
	m_pMLEditBox->SetFont(1);
	m_pMLEditBox->SetText(GetGameApplication()->GetConfig()->szAutoReply);
}

bool CGuiAutoReplySettingDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		if( nID == 2 ) return OnIDOK();
		if( nID == 3 ) return OnIDCancel();
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiAutoReplySettingDlg::OnClose(void)
{
	GetGameGuiManager()->m_guiAutoReply = NULL;
}

void CGuiAutoReplySettingDlg::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x ,pt.y - 74);
	}

	FUNCTION_END;
}

bool CGuiAutoReplySettingDlg::OnIDOK()
{
	FUNCTION_BEGIN;
	const char* pszText = m_pMLEditBox->GetText();
	if( NULL != pszText && strlen(pszText) > 0 )
	{
		GetGameApplication()->GetConfig()->SetClientSet( enumAuto_Reply,true );
		GetGameApplication()->GetConfig()->SetAutoReplayText( pszText );
		//这里一定要放在SetAutoReplyText的后面,
		//因为自动回复语句的消息是通过下面这个函数发出的,因此在此之前
		//就应该保存有这条语句
		GetGameApplication()->SetAutoReplyType(AutoReply_UserDefine);
		Close();
		if( m_pMenu )
		{
			m_pMenu->SetMenuItemEnbale(AutoReply_Switch,true);
			m_pMenu->SetMenuItemCheck(AutoReply_UserDefine);
		}
		if( GetGameGuiManager()->m_guiMiniMap )
		{
			char szTemp[512];
			_snprintf(szTemp,512,"自动回复:(自定义)\n%s",pszText);
			szTemp[511] = 0;
			GetGameGuiManager()->m_guiMiniMap->SetAutoReplyButtonToolTips(szTemp);
		}
	}
	
	return true;
	FUNCTION_END;
}

bool CGuiAutoReplySettingDlg::OnIDCancel()
{
	FUNCTION_BEGIN;
	Close();
	return true;
	FUNCTION_END;
}
