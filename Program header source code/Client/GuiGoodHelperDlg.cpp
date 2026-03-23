
#include "public.h"
#include "./GameGuiManager.h"
#include "./GuiGoodHelperDlg.h"
#include "./guiMain.h"
#include "./Game.h"


CGuiGoodHelperDlg::CGuiGoodHelperDlg()
{
	FUNCTION_BEGIN;

	//m_bSmDialog = true;

	FUNCTION_END;
}
 
CGuiGoodHelperDlg::~CGuiGoodHelperDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuiGoodHelperDlg::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	
	//sky 滑动的初始位置
	//m_x = (  GetDevice()->GetWidth() - 204 - GetImage( 0 )->GetWidth() );
	//m_y = 0;
	
	m_x = (  GetDevice()->GetWidth() - 204 - GetImage( 0 )->GetWidth() );
	m_y = -150;

	GetButton(11)->SetLocation( 0, 0 );
	GetButton(12)->SetLocation(0, 26);
	GetButton(13)->SetLocation(0, 52);
	
	m_lastCreateTime = time(NULL);
 
	FUNCTION_END;
}


bool CGuiGoodHelperDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
		    case 11:  // 我的VIP
			{
				GetGameGuiManager()->AddQuestVipGame();
				Close();
			}
			break;
			case 12:  // 金币指南
			{
			    GetGameGuiManager()->AddHelp("data\\help\\goldintroduce.xml");
			    Close();
		    }
			break;
			case 13:  // 资料片帮助
			{
				GetGameGuiManager()->AddHelp();	
				Close();
			}
			break;
		
		//case ID_BUTTON_CLOSE:
		//	{
		//		Close();
		//	}
		//	break;
		}

		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiGoodHelperDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiGoodHelper = NULL;

	FUNCTION_END;
}

void CGuiGoodHelperDlg::OnResetScreenSize()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnResetScreenSize();

	m_x = (  GetDevice()->GetWidth() - 204 - GetImage( 0 )->GetWidth() );
	//m_y = 25;
	m_y = -150;
	
	//GetButton( 11 )->SetLocation( 0, 0 );
	//GetButton( 12 )->SetLocation(0, 25);

	FUNCTION_END;
}

HRESULT CGuiGoodHelperDlg::OnRender( float fElapsedTime )
{
//	GetImage( 0 )->SetVisible( false );
//	GetButton( 11 )->SetVisible( false );
//	GetButton( 12 )->SetVisible( false );
//	GetButton( 13 )->SetVisible( false );

	
	if(24 > m_y )
		m_y += 8; //数值越大速度越快

    //sky 大于10秒关闭
	//if(time(NULL) > m_lastCreateTime + 10 )
	//	Close();

	return CGuiDialog::OnRender(fElapsedTime);
}


