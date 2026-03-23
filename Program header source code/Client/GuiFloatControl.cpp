#include "public.h"
#include "../gui/include/guiTypes.h"
#include ".\guimain.h"
#include ".\gameguimanager.h"
#include "Game.h"
#include "game_script.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GameState.h"
//#include "./GuiPrivateChat.h"
//#include "./GuiUserInfo.h"
//#include "./GuiChatOutput.h"
//#include "./GuiNpcDialog.h"
//#include "./GuiTrade.h"
//#include "./GuiMiniMap.h"
//#include "./GuiPrivateChat.h"
//#include "./GuiChatInvite.h"
//#include "./GuiWorldConn.h"
#include "./GuiChatSelect.h"
#include ".\guifloatcontrol.h"
#include "./GuiPetDialog.h"
#include "./GuiShopDialog.h"
#include "./GuiItem.h"
#include "../luabind/public.h"
/**
 * \brief 构造函数
 * 
 * 
 * 
 * \return 
 */
CGuiFloatControl::CGuiFloatControl(void)
{
	FUNCTION_BEGIN;

	m_bPretendedHide = false;

	FUNCTION_END;
}

/**
 * \brief 关闭对话框
 * 
 * 
 * 
 * \return 
 */
void CGuiFloatControl::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiFloatControl = NULL;

	FUNCTION_END;
}

/**
 * \brief 创建对话框
 * 
 * 
 * 
 * \return 
 */
void CGuiFloatControl::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	m_x = GetDevice()->GetWidth()-this->GetWidth();
	m_y = GetDevice()->GetHeight()-500;
	//BerthHide();

	OnResetScreenSize();

	FUNCTION_END;
}

BOOL CGuiFloatControl::ContainsPoint( POINT ptScreen )
{
	FUNCTION_BEGIN;

	if(CGuiDialog::ContainsPoint(ptScreen)) 
	{
		return TRUE;
	}

	stPointI pt1 = ptScreen;
	pt1.x -= m_x + m_clientOffsetTopLeft.x;
	pt1.y -= m_y + m_clientOffsetTopLeft.y;
	CGuiControl* p = this->GetControlAtPoint(pt1);
	if(p)
	{
		return TRUE;
	}
	return FALSE;

	FUNCTION_END;
}
void CGuiFloatControl::OnResetScreenSize()
{
	FUNCTION_BEGIN;

	//*
	CGuiDialog::OnResetScreenSize();

	stResourceLocation rl;
	rl.SetFileName( "data\\Interfaces.gl" );

	int grp;

	if ( GetDevice()->GetWidth() == 800 )
	{
		grp = 101;

		//Reset back img
		if ( GetBackImage() )
		{
			rl.group = grp;
			rl.frame = GetBackImage()->GetImgFrame();
			SetBackImage( rl );
		}

		//Reset location
		m_x = GetDevice()->GetWidth() - 122;
		m_y = GetDevice()->GetHeight() - 56 - 57;

		//Show some button
		if ( GetButton( 20 ) )
			GetButton( 20 )->SetVisible( true );
		if ( GetButton( 21 ) )
			GetButton( 21 )->SetVisible( true );
		if ( GetButton( 24 ) )
			GetButton( 24 )->SetVisible( true );


		//We reset control img
		for( int i=10;i<25;i++ )
		{
			if ( GetButton( i ) )
			{
				int num = 0;
				GetButton( i )->GetBitmapInfo( rl, num );
				rl.group = grp;
				if ( abs(num) > 100 )
					num = 0;

				GetButton( i )->SetBitmap( rl, num );
				continue;
			}
			rl.SetFileName( "data\\Interfaces.gl" );			
		}	
	}
	else
	{
		grp = 1;
		if ( GetBackImage() )
		{
			rl.group = grp;
			rl.frame = GetBackImage()->GetImgFrame();
			SetBackImage( rl );
		}
		
		//Reset dlg position
		if ( GetGameGuiManager()->m_guiChatSelect )
		{
			m_x = GetGameGuiManager()->m_guiChatSelect->GetLocation().x + 856;
			//m_y = GetGameGuiManager()->m_guiChatSelect->GetLocation().y + 687;
			m_y = GetDevice()->GetHeight() - 111;
		}
		else
		{
			m_x = 856;
			m_y = GetDevice()->GetHeight() - 111;
		}
		/*m_x = GetDevice()->GetWidth()-this->GetWidth();
		m_y = GetDevice()->GetHeight()-500;*/

		//Hide some button
		if ( GetButton( 20 ) )
			GetButton( 20 )->SetVisible( false );
		if ( GetButton( 21 ) )
			GetButton( 21 )->SetVisible( false );
		if ( GetButton( 24 ) )
			GetButton( 24 )->SetVisible( false );


		//We reset control img
		for( int i=10;i<25;i++ )
		{
			if ( GetButton( i ) )
			{
				int num = 0;
				GetButton( i )->GetBitmapInfo( rl, num );
				rl.group =grp;
				if ( abs(num) > 100 )
					num = 0;

				GetButton( i )->SetBitmap( rl, num );
				continue;
			}
			rl.SetFileName( "data\\Interfaces.gl" );

			if ( GetImage( i ) )
			{
				rl.group = grp;
				rl.frame = GetImage( i )->GetImgFrame();
				GetImage( i )->SetImage( rl );
			}
		}		
	}

	FUNCTION_END;
}

HRESULT CGuiFloatControl::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;
	
	if ( m_bPretendedHide )
	{
		m_x = -10000;
		m_y = -10000;
	}
	else
	{
		//Set pos follow chat select dlg
		if ( GetGameGuiManager()->m_guiChatSelect )
		{
			if ( GetDevice()->GetWidth() == 800 )
			{
				//m_x = GetGameGuiManager()->m_guiChatSelect->GetLocation().x + 856;
				////m_y = GetGameGuiManager()->m_guiChatSelect->GetLocation().y + 687;
				//m_y = GetDevice()->GetHeight() - 81;

				m_x = GetDevice()->GetWidth() - 122;
				m_y = GetDevice()->GetHeight() - 56 - 57;
			}
			else
			{
				m_x = GetGameGuiManager()->m_guiChatSelect->GetLocation().x + 856;
				//m_y = GetGameGuiManager()->m_guiChatSelect->GetLocation().y + 687;
				m_y = GetDevice()->GetHeight() - 111;
			}			
		}
		else
		{
			if ( GetDevice()->GetWidth() == 800 )
			{
				m_x = GetDevice()->GetWidth() - 122;
				m_y = GetDevice()->GetHeight() - 56 - 57;
			}
			else
			{
				m_x = 856;
				m_y = GetDevice()->GetHeight() - 111;
			}			
		}
	}	

	//Render dialog
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	return hr;

	FUNCTION_END;
}

/**
 * \brief 关闭/打开商店对话框
 * 
 * 
 * 
 * \return 
 */
inline void OnOffShopDialog()
{
	FUNCTION_BEGIN;

	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();

	if(GetGameGuiManager()->m_guiShop && GetGameGuiManager()->m_guiShop->IsVisible())
	{
		GetGameGuiManager()->m_guiShop->SetVisible(false);
	}
	else if(pMainRole && pMainRole->bIsShopStart() )
	{
		GetGameGuiManager()->AddShop();
	}
	else
	{		
		CGuiShopDialog* pShopDlg = GetGameGuiManager()->AddShop();
		if( pShopDlg )
		{
			stRectI rcClient = pShopDlg->GetClipRect();
			int x = GetDevice()->GetWidth() / 2;
			int y = 100;
			pShopDlg->SetLocation(x,y);			
		}
		
		GetGameGuiManager()->AddItem();

		if( GetGameGuiManager()->m_guiItem )
		{
			stRectI rcClient = GetGameGuiManager()->m_guiItem->GetClipRect();
			int x = GetDevice()->GetWidth() / 2 - rcClient.Width();
			int y = 100;
			GetGameGuiManager()->m_guiItem->SetLocation(x,y);
		}
	}

	FUNCTION_END;
}

#include "./GuiSystem.h"
/**
 * \brief 初始化脚本
 * 
 * 
 * 
 * \return 
 */
void CGuiFloatControl::OnInitScript()
{
	FUNCTION_BEGIN;

	GameScriptInit(GetScriptEngine());
	using namespace luabind;
	module(GetScriptEngine())
	[
		def("LogoutGame",&LogoutGame),
		//def("AddSystemDialog",&AddSystemDialog),
		def("OnOffNatioanalMapDialog",&OnOffNatioanalMapDialog),
		def("OnOffShopDialog",&OnOffShopDialog),
		def("ShowHelpDialog",&ShowHelpDialog),
		def("ShowEmotionDialog",&ShowEmotionDialog),
		def("OnRide",&OnRide),
		def("OnSitDown",&OnSitDown),
		def("OnOffSysDialog",&OnOffSysDialog),
		def("OnOffAutoAttackDlg",&OnOffAutoAttackDlg),
		def("OnRunSiRenBieShuDialog",&OnRunSiRenBieShuDialog),//云天别墅
		def("OnUserClickButton1",&OnUserClickButton1),//敬请期待
		def("OnUserClickButton2",&OnUserClickButton2),//敬请期待
		def("OnUserClickButton3",&OnUserClickButton3),//敬请期待
		def("OnUserClickButton4",&OnUserClickButton4)//敬请期待

	];

	FUNCTION_END;
}