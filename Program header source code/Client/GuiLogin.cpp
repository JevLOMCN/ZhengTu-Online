#include "public.h"
#include "../gui/include/guiTypes.h"
#include ".\guilogin.h"
#include ".\guiKeyBoard.h"
#include ".\GameGuiManager.h"
#include "..\gui\include\guiEditbox.h"
#include "game.h"
#include "game_script.h"
#include "./GuiCreateCharDialog.h"
#include "./GuiRegisterDialog.h"
#include "./GameAppation.h"

#include "./SpriteObj.h"

#include "../RenderD3D/include/RenderD3D.h"
#include "../RenderD3D/include/D3DDevice.h"
#include "../RenderD3D/include/TextureManager.h"
#include "../RenderD3D/include/CustomTexture.h"
#include "../luabind/public.h"
//////////////////////////////////////////////////////////////////////////////
#define  BTN_LOGIN_ID    5 
//////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	#ifndef _MDEBUG
	#define _MDEBUG
	#endif
#else
	#ifdef _MDEBUG
	#undef _MDEBUG
	#endif
#endif

/////////////////////////////////////////////////////////////////////

BOOL CPixelContext::m_bIs800By600 = false;
BOOL CPixelContext::m_bFirst = false;
stPointF CPixelContext::m_Scale = stPointF(1.0f,1.0f);

BOOL CPixelContext::IsLogin800By600Internal()
{
	//stConfigBase * pBase = Engine_GetAppation()->GetConfigBase();
	//int nWidth = Engine_GetAppation()->GetConfigBase()->screenWidth;
	//int nHeight = Engine_GetAppation()->GetConfigBase()->screenHeight;

	int nWidth = GetDevice()->GetWidth();
	int nHeigth = GetDevice()->GetHeight();
	return ( nWidth <= 800 );
	//return TRUE;
}

stPointF & CPixelContext::GetRenderScale()
{
    if( m_bFirst)
	    IsLogin800By600();
    return m_Scale;

}

void CPixelContext::ClearFirstFlag()
{
	m_bFirst = FALSE;
}

BOOL CPixelContext::IsLogin800By600()
{
	if( !m_bFirst)
	{
		m_bIs800By600 = IsLogin800By600Internal();
		if( m_bIs800By600 )
		{
			m_Scale.x = (float)800/1024;
			m_Scale.y = (float)600/768;
		}
		else 
		{
			m_Scale.x = 1.0f;
			m_Scale.y = 1.0f;
		}
		m_bFirst = TRUE;
	}
	return m_bIs800By600;
}
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
/// 物品图包
extern const char* c_szItemGrpPack;		

////////////////////////////////////////////////////////////////
char  g_szName[MAX_NAMESIZE]={0};

/////////////////////////////////////////////////////////////////
const DWORD  c_AniFloor       =   130;	// 地板
const DWORD  c_AniPP          =   110;	// 瀑布
const DWORD  c_AniBannerHorse  =  120;	// 旗马
const DWORD  c_AniMenHorse    =   45;	//人马  30
////////////////////////////////////////////////////////////////

CLogoAni::CLogoAni()
{
	FUNCTION_BEGIN;
	Release();
	FUNCTION_END;
}


void CLogoAni::CreateAnimation()
{
	FUNCTION_BEGIN;

	if( bInited )	return;

	stResourceLocation rl;
	rl.SetFileName("data\\logo.gl");
	rl.group = 0;

	rl.frame = 1;
	m_bCreateFloorOk = m_AniFloor.Create(&rl,false);
	if (m_bCreateFloorOk)
	{
		m_AniFloor.SetSpeed(c_AniFloor);
		m_AniFloor.SetLoopPlay(true);
	}
	
	rl.frame = 2;
	m_bCreatePPOk = m_AniPP.Create(&rl,false);
	if ( m_bCreatePPOk )
	{
		m_AniPP.SetSpeed(c_AniPP);
		m_AniPP.SetLoopPlay(true);
	}

	rl.frame = 3;
	m_bCreateBannerHorseOk = m_AniBannerHorse.Create(&rl,false);
	if( m_bCreateBannerHorseOk )
	{
		m_AniBannerHorse.SetSpeed(c_AniBannerHorse);
		m_AniBannerHorse.SetLoopPlay(true);	
	}

	rl.frame = 4;
	m_bCreateMenHorseOk = m_AniMenHorse.Create(&rl,false);
	if( m_bCreateMenHorseOk )
	{
		m_AniMenHorse.SetSpeed(c_AniMenHorse);
		m_AniMenHorse.SetLoopPlay(true);
	}
	
	rl.frame = 0;
	m_bBackAnimation = m_AniBack.Create(&rl,false);
	if(m_bBackAnimation)
	{
		m_AniBack.SetSpeed(100);
		m_AniBack.SetLoopPlay(true);
	}


	bInited = TRUE;

	FUNCTION_END;
}

void CLogoAni::RenderBackAnimation(POINT pt)
{
	if( bInited )
	{	
		if(m_bBackAnimation)      m_AniBack.Render(pt.x,pt.y,NULL,&CPixelContext::GetRenderScale());
	}
}
void CLogoAni::RenderAnimation(POINT pt)
{
	FUNCTION_BEGIN;

	if( pt.x !=-800)
		int ii = 0;
	if( bInited )
	{	
		if(m_bCreateMenHorseOk)      m_AniMenHorse.Render(pt.x,pt.y,NULL,&CPixelContext::GetRenderScale());
		if( m_bCreatePPOk )          m_AniPP.Render(pt.x,pt.y,NULL,&CPixelContext::GetRenderScale());
		if( m_bCreateBannerHorseOk)  m_AniBannerHorse.Render(pt.x,pt.y,NULL,&CPixelContext::GetRenderScale());
		if( m_bCreateFloorOk )       m_AniFloor.Render(pt.x,pt.y,NULL,&CPixelContext::GetRenderScale());
	}

	FUNCTION_END;	
}


void CLogoAni::Release()
{
      FUNCTION_BEGIN;

      m_bCreateMenHorseOk = false;
      m_bCreatePPOk = false;
      m_bCreateBannerHorseOk = false;
      m_bCreateFloorOk = false;

      m_bBackAnimation = false;

      bInited = false;

      FUNCTION_END;

}

CLogoAni * GetLogoAni()
{
	FUNCTION_BEGIN;

	static CLogoAni  logoAni;
	return  & logoAni;

	FUNCTION_END;	
}

///////////////////////////////////////////////////////////////////////////////
const char* msg = "健康游戏公告: "
                  "抵制不良游戏，拒绝盗版游戏。" 
		  "注意自我保护，谨防受骗上当。"
		  "适度游戏益脑，沉迷游戏伤身。"
		  "合理安排时间，享受健康生活。";
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
DWORD s_last_send_time = 0;     
BOOL  s_bLoginEcho  = true;        // 登陆请求消息是否已经返回

void LoginSafeSendCommand()
{
	FUNCTION_BEGIN;

	int nTime = 3000 - (int)(xtimeGetTime() - s_last_send_time);
	if(nTime > 0) 
	{
		Sleep(nTime);
	}
        s_last_send_time = xtimeGetTime();

	//TRACE("login safe send command\n");
        
	s_bLoginEcho  = false;

	FUNCTION_END;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * \brief 构造函数
 * 
 * 构造函数
 * 
 * \return 无
 */
CGuiLogin::CGuiLogin( void )
: m_bKeyBoardOpen(false)
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}


/**
 * \brief 关闭对话框
 * 
 * 关闭对话框
 * 
 * \return 无
 */
void CGuiLogin::OnClose(void)
{
	FUNCTION_BEGIN;

	strncpy(g_szName,GetEditBox(3)->GetText(),sizeof(g_szName));  // User Name 
	
	SavePlayerName();

	GetLogoAni()->Release();
	GetGameGuiManager()->m_guiLogin = NULL;

	if( GetGameGuiManager()->m_guiKeyBoard)
	{
		GetGameGuiManager()->m_guiKeyBoard->Close();
		m_bKeyBoardOpen = false;
	}

	FUNCTION_END;
}

/**
 * \brief 创建对话框
 * 
 * 创建对话框
 * 
 * \return 无
 */
void CGuiLogin::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();


	SetLogining(TRUE);
		 
	//GetEditBox(100)->SetVisible(false);

	
	if(  0 != strcmp("",GetGameApplication()->GetConfig()->szPlayerName) )
	{
		if(GetButton(8))
		{
			GetButton(8)->SetChecked(true);
		}
		if( GetEditBox(3))
		{
			GetEditBox(3)->SetText(GetGameApplication()->GetConfig()->szPlayerName);
			if(0 != strcmp(GetEditBox(3)->GetText(),""))
			{
				if( GetEditBox(4) )
					SetFocus(GetEditBox(4));
			}
		}

	}
	else
	{
		if( GetEditBox(3))
		{
			GetEditBox(3)->SetText(g_szName);
			if(0 != strcmp(GetEditBox(3)->GetText(),""))
			{
				if( GetEditBox(4) )
					SetFocus(GetEditBox(4));
			}
		} 
		if( GetButton(8) )
		{
			GetButton(8)->SetChecked(false);
		}
	}

	PlayBackMusic("login.mp3",3000);

	GetImage(99)->SetVisible(false);
	GetEditBox(100)->SetVisible(false);
	GetEditBox(100)->SetEnabled(false);


	CGuiStatic * pStatic = GetStatic(7);
	if(pStatic)
	{ 
		if(CPixelContext::IsLogin800By600())
		{
			pStatic->SetFont(0);
		}
		pStatic->SetTextColor(COLOR_ARGB(255,255,128,0));
		pStatic->SetText(msg);
	}

	
	GetLogoAni()->CreateAnimation();

 
	GetButton(8)->SetToolTips("保存帐号");
	GetButton(9)->SetToolTips("软键盘");

	FUNCTION_END;
}

void CGuiLogin::RenderBackground(float fElapsedTime)
{
	FUNCTION_BEGIN;
	
	POINT pt = GetLocation();
	pt.x -= GetDevice()->GetWidth();
	pt.y =  pt.y;
	GetLogoAni()->RenderBackAnimation(pt);

	return CGuiDialog::RenderBackground(fElapsedTime);

	FUNCTION_END;
}

HRESULT CGuiLogin::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	
	if( !IsVisible()) return S_OK;
	
	CGuiDialog::OnRender(fElapsedTime);

        /////////////////////////////////////////////////////////////////////////////////
	if( CPixelContext::IsLogin800By600())
	{
		POINT pt = GetLocation();
		pt.x -= GetDevice()->GetWidth();
		/*pt.x -= 800;
		pt.x +=(1024-800)/2;
		pt.y += (768-600)/2;*/
		GetLogoAni()->RenderAnimation(pt);
	}
	else
	{
		POINT pt = GetLocation();
		pt.x -= GetDevice()->GetWidth();
		pt.y =  pt.y;
		GetLogoAni()->RenderAnimation(pt);

	}
	
	/////////////////////////////////////////////////////////////////////////////////
	// NetWork's overtime's estimatation 
	if( s_bLoginEcho == false)
	{
		if( (xtimeGetTime() - s_last_send_time) > 10000) // 10 sec echo limit
		{
			GameMessageBox("服务器繁忙,请稍后再试");
			SetEnabled(true);
			s_bLoginEcho = true;
		}
	}
	else
	{
		//TRACE("已经收到登陆请求回应消息\n");
	}
	/////////////////////////////////////////////////////////////////////////////////
	/*if(CPixelContext::IsLogin800By600())
	{
		POINT pt = GetLocation();
		GetDevice()->DrawString(msg,stPointI(20+pt.x,565+pt.y),COLOR_ARGB(255,255,128,0));
	}*/
	 
	return S_OK;
	
	FUNCTION_END;

}

/**
 * \brief 消息处理函数
 * 
 * 消息处理函数
 * 
 * \param hWnd : 描述
 * \param uMsg : 描述
 * \param wParam : 描述
 * \param lParam : 描述
 * \return 消息处理结果
 */
bool CGuiLogin::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam  )
{
	FUNCTION_BEGIN;


	return CGuiDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	FUNCTION_END;
}

/**
 * \brief 消息处理函数
 * 
 * 消息处理函数
 * 
 * \param nEvent : 消息类型
 * \param nID : 控件ID
 * \param pControl : 控件指针
 * \return 消息处理结果
 */
bool CGuiLogin::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case  EVENT_DIALOG_SHOW:  // 窗口显示时
			{
				PlayBackMusic("login.mp3",3000);
			}
		break;
	case EVENT_BUTTON_CLICKED:
			{
				switch(nID)
				{
					case 5:	// 登陆游戏
						{
							if (0 == strcmp(GetEditBox(3)->GetText(),""))
							{
								SetFocus(GetEditBox(4));
								break;
							}
							else if( 0 == strcmp( GetEditBox(4)->GetText(),""))
							{
								SetFocus(GetEditBox(4));
								break;
							}
							else
							{
								if(LoginGame( GetEditBox(3)->GetText(),GetEditBox(4)->GetText(), GetEditBox(100)->GetText()))
									SetEnabled(false); 
								else
								{
									//ShowMessage(&quot;发送登陆请求失败！&quot;);
									ShowMessage("发送登陆请求失败!");
									break;
								}
							}
							SavePlayerName();
							strncpy(g_szName,GetEditBox(3)->GetText(),sizeof(g_szName));  // User Name 
							if( GetGameGuiManager()->m_guiKeyBoard)
							{
								GetGameGuiManager()->m_guiKeyBoard->Close();
								m_bKeyBoardOpen = false;
							}
						}
						break;
					case 6: // exit
						ExitGame();
						break;
					case 8: //remember the account 
						if(GetButton(8)->GetChecked())
						{
							GetButton(8)->SetChecked(false);
						}
						else
						{
							GetButton(8)->SetChecked(true);
						}
						break;
					case 9:
						m_bKeyBoardOpen = !m_bKeyBoardOpen;
						if(m_bKeyBoardOpen)
						{
							GetGameGuiManager()->AddKeyBoardDlg();
							OnResetScreenSize();
							SetPassEditFocus();
						}
						else 
						{
							if( GetGameGuiManager()->m_guiKeyBoard)
								GetGameGuiManager()->m_guiKeyBoard->Close();
						}
						break;
				}
			}
			break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiLogin::SetPassEditFocus()
{
	FUNCTION_BEGIN;
	GetGuiManager()->RequestFocus(this);
	SetFocus(GetEditBox(4));
	FUNCTION_END;
}
bool CGuiLogin::IsSavePlayerName()
{
	FUNCTION_BEGIN;
	CGuiButton * pBtn =  GetButton(8);
	if (pBtn)
		return pBtn->GetChecked();
	return false;
	FUNCTION_END;
}


void CGuiLogin::ResetOpenKeyBoardStatus()
{
	FUNCTION_BEGIN;
      
	FUNCTION_END;
}

void CGuiLogin::SavePlayerName()
{
	FUNCTION_BEGIN;

	if( IsSavePlayerName() )
	{
		strncpy(GetGameApplication()->GetConfig()->szPlayerName,GetEditBox(3)->GetText(),MAX_NAMESIZE);
	}
	else
	{
                 memset(GetGameApplication()->GetConfig()->szPlayerName,0,MAX_NAMESIZE);
	}	
	GetGameApplication()->GetConfig()->szPlayerName[MAX_NAMESIZE] = 0;
	GetGameApplication()->SaveConfig();

	FUNCTION_END;
}

/**
 * \brief 注册函数脚本
 * 
 * 注册函数脚本
 * 
 * \return 无
 */
void CGuiLogin::OnInitScript()
{
	FUNCTION_BEGIN;

	GameScriptInit(GetScriptEngine());
	using namespace luabind;
	module(GetScriptEngine())
	[
		def("LoginGame",&LoginGame),
		def("ExitGame",&ExitGame),
		def("AddCreateCharDialog",&AddCreateCharDialog)
	];

	FUNCTION_END;
}


void CGuiLogin::AppendPassWord(char szPass)
{
	FUNCTION_BEGIN;

	std::string strPass = GetEditBox(4)->GetText();
	strPass.push_back(szPass);
	GetEditBox(4)->SetText(strPass.c_str());
	
	SetPassEditFocus();

	FUNCTION_END;
}


void CGuiLogin::OnBackClicked()
{
	FUNCTION_BEGIN;
        std::string strPass = GetEditBox(4)->GetText();
	size_t size = strPass.size();
	if( size > 0)
		strPass.erase(size-1);
	GetEditBox(4)->SetText(strPass.c_str());
	SetPassEditFocus();
	FUNCTION_END;
}

void CGuiLogin::ClearPassText()
{
	FUNCTION_BEGIN;

	GetEditBox(4)->ClearText();
	SetFocus(GetEditBox(4));


	FUNCTION_END;
}

void CGuiLogin::OnResetScreenSize()
{
	FUNCTION_BEGIN;

	if( GetGameGuiManager()->m_guiKeyBoard && GetGameGuiManager()->m_guiKeyBoard->IsVisible())
	{
		if( !CPixelContext::IsLogin800By600())
			GetGameGuiManager()->m_guiKeyBoard->SetLocation(642,340);
		else
			GetGameGuiManager()->m_guiKeyBoard->SetLocation(442,300);

	}

	FUNCTION_END;
}

void CGuiLogin::ClickOnLoginButton()
{
	FUNCTION_BEGIN;

	SendEvent( EVENT_BUTTON_CLICKED, true, GetButton(5) );

	FUNCTION_END;
}
