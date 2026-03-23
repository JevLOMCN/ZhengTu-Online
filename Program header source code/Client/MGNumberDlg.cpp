#include "public.h"
#include "./miniusercommand.h"
#include "./Game.h"
#include "./miniusercommand.h"
#include "./GameGuiManager.h"
#include "./MGNumberDlg.h"
#include "./RoleItem.h"
#include "./Chat.h"
#include ".\guiface.h"
#include "./MainCharacter.h"
#include "./GameScene.h"

CMGNumberDlg::CMGNumberDlg(void)
{	
	m_bResultDao = false;
	m_bResultDaoSelf = false;
	m_bResultDaoO = false;
	m_bUpdateSelf = false;
	m_bUpdateO = false;
	m_dwCurTime3 = LIMT_TIME;
}

CMGNumberDlg::~CMGNumberDlg(void)
{
}

void CMGNumberDlg::OnCreate(void)
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();
	
	stResourceLocation rl;
	m_bResultDao = false;
	m_bResultDaoSelf = false;
	m_bResultDaoO = false;

	if (GetStatic(IDS_L_FIRST))
	{
		GetStatic(IDS_L_FIRST)->SetVisible(false);
	}
	if (GetStatic(IDS_L_SECOND))
	{
		GetStatic(IDS_L_SECOND)->SetVisible(false);
	}
	if (GetStatic(IDS_L_THIRD))
	{
		GetStatic(IDS_L_THIRD)->SetVisible(false);
	}
	if (GetStatic(IDS_R_FIRST))
	{
		GetStatic(IDS_R_FIRST)->SetVisible(false);
	}
	if (GetStatic(IDS_R_SECOND))
	{
		GetStatic(IDS_R_SECOND)->SetVisible(false);
	}
	if (GetStatic(IDS_R_THIRD))
	{
		GetStatic(IDS_R_THIRD)->SetVisible(false);
	}
	
	if (GetScene()->GetMainCharacter())
	{	
		rl.SetFileName( "data\\interfaces.gl" );
		rl.group = 33;
		rl.frame = GetScene()->GetMainCharacter()->GetFace();
		if (GetImage(IDIMG_SELF_CHARA))
		{
			GetImage(IDIMG_SELF_CHARA)->SetImage(rl);
			m_selfid = GetScene()->GetMainCharacter()->GetID();
		}		
	}

	if ( GetScene()->m_pInterestNpc )
	{
		if ( GetScene()->m_pInterestNpc->IsCharacter() )
		{
			
			if ( GetScene()->m_pInterestNpc->IsChangeFace() )
			{
				rl.SetFileName( "data\\icons.gl" );
				rl.group = 4;
				rl.frame = GetNpcImageID( GetScene()->m_pInterestNpc->GetChangeFaceID() );
			}
			else
			{
				rl.SetFileName( "data\\interfaces.gl" );
				rl.group = 33;
				rl.frame = ((CCharacter*)(GetScene()->m_pInterestNpc))->GetFace();
			}			
			if (GetImage(IDIMG_O_CHARA)) 
			{
				GetImage(IDIMG_O_CHARA)->SetImage(rl);
			}			
		}
		else
		{			
			stResourceLocation rl;
			rl.SetFileName( "data\\icons.gl" );
			rl.group = 4;
			rl.frame = GetScene()->m_pInterestNpc->GetNpcPic();

			if (GetImage(IDIMG_O_CHARA)) 
			{
				GetImage(IDIMG_O_CHARA)->SetImage(rl);
			}			
		}		
	}	

	for(int i=0;i<2;i++)
	{
        rl.SetFileName( "data\\interfaces.gl" );
		rl.group = 130;						
		rl.frame = 43;
		m_ImageSai[i].Create( &rl,true);
		m_ImageSai[i].SetRandomPlay();
		m_ImageSai[i].SetSpeed(15);
	}
	
	if (GetButton(IDB_MG_START))
	{
		GetButton(IDB_MG_START)->SetVisible(true);
		GetButton(IDB_MG_START)->SetEnabled(true);
	}
	if (GetButton(IDB_MG_STOP))
	{
		GetButton(IDB_MG_STOP)->SetVisible(false);
		GetButton(IDB_MG_STOP)->SetEnabled(false);
	}
	if (GetStatic(IDS_DAO_TIMER))
	{
		GetStatic(IDS_DAO_TIMER)->SetText("请你点[开始]按钮进行游戏!");
	}
	FUNCTION_END;
}

bool CMGNumberDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case IDB_MG_STOP :
			{		
				Stop();
			}
			break;
		case IDB_MG_START :
			{
				Start();
			}
			break;
		case IDB_MG_CHATSEND:
			{
				SendChat();
			}
			break;
		case IDB_MG_CHATEMOI:
			{
				if (GetEditBox(IDE_CHATINPUT))
				{
					OnOffChatFaceSelect( GetEditBox(IDE_CHATINPUT) );
				}				
			}
			break;
		default:
			;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CMGNumberDlg::Update()
{
	char temp[256];
	if (GetButton(IDB_MG_STOP)->IsEnabled())
	{
		static DWORD dwPreTime  = xtimeGetTime();
		
		DWORD dwCurTime  = xtimeGetTime();
		if( dwCurTime - dwPreTime > 1000)
		{
			m_dwCurTime3 -= (dwCurTime-dwPreTime)/1000;
			if( ((int)m_dwCurTime3) < 0)
			{
				m_dwCurTime3 = 0;
			}
			dwPreTime = dwCurTime;
			sprintf(temp,"请你在[%d]秒内点击停止按钮!",m_dwCurTime3);
			GetStatic(IDS_DAO_TIMER)->SetText(temp);
			if ((int)m_dwCurTime3 == 0)
			{
				m_dwCurTime3 = LIMT_TIME;
			}
		}
	}	
}

HRESULT CMGNumberDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	stPointI ptCursor = Engine_GetCursor()->GetPosition();

	m_ImageSai[0].Render(286+m_x,155+m_y,m_bUpdateSelf);
	m_ImageSai[1].Render(365+m_x,155+m_y,m_bUpdateO);

	Update();

	if (m_bResultDao)
	{
		static DWORD dwCurTime = 0;
		if( dwCurTime == 0)
		{
			dwCurTime = xtimeGetTime();
		    return hr;
		}
		else 
		{
			if( xtimeGetTime() - dwCurTime > 1000)
			{				
				GetStatic(IDS_SELF_MONEY)->SetText( Money2String(GetScene()->GetMainCharacter()->GetMoney()));
				switch(m_res) 
				{
				case 0:
					GetGameGuiManager()->AddMessageBox("遗憾,你输了,本局游戏结束,是否继续进行?",
						eAction_MiniGame_Continue,NULL,c_szMBOKCancel);
					break;
				case 1:
					GetGameGuiManager()->AddMessageBox("恭喜,你赢了,本局游戏结束,是否继续进行?",
						eAction_MiniGame_Continue,NULL,c_szMBOKCancel);
					break;
				case 2:
					GetGameGuiManager()->AddMessageBox("平局,本局游戏结束,是否继续进行?",
						eAction_MiniGame_Continue,NULL,c_szMBOKCancel);
					break;
				default:
					;
				}
				m_bResultDao = false;
			}
		}
	}
	if (m_bResultDaoSelf)
	{		
		if ((m_selfdicenum<0) && (m_selfdicenum>6))
		{
			m_selfdicenum = 1;
		}
		m_ImageSai[0].SetFrame(m_selfdicenum-1);
		m_bUpdateSelf = false;
		m_bResultDaoSelf = false;

	}
	if (m_bResultDaoO)
	{		
		if ((m_odicenum<0) && (m_odicenum>6))
		{
			m_odicenum = 1;
		}
		m_ImageSai[1].SetFrame(m_odicenum-1);
		m_bResultDaoO = false;
		m_bUpdateO = false;
	}

	return hr;
	FUNCTION_END;
}

void CMGNumberDlg::OnClose(void)
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->m_MGNumberDlg = NULL;
	
	stEndMiniGame cmd;
	SEND_USER_CMD( cmd );
	
	CGuiDialog::OnClose();
	FUNCTION_END;
}

void CMGNumberDlg::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt( m_pBtnClose->GetLocation() );

		m_pBtnClose->SetLocation(pt.x,pt.y - 80);
	}

	FUNCTION_END;
}

void CMGNumberDlg::OnInitInfo(void *pData)
{
	FUNCTION_BEGIN;
	stStartMiniGame* ptemp = (stStartMiniGame* )pData;
	m_oid = ptemp->tempid;
	
	if (GetStatic(IDS_SELF_NAME))
	{
		GetStatic(IDS_SELF_NAME)->SetText(GetScene()->GetMainCharacter()->GetName());
	}
	if (GetStatic(IDS_O_NAME))
	{
		if (GetScene()->FindCharacterByID(ptemp->tempid))
		{
			GetStatic(IDS_O_NAME)->SetText(GetScene()->FindCharacterByID(ptemp->tempid)->GetName());
		}
		else
		{
			GetStatic(IDS_O_NAME)->SetText("对战玩家");
		}
		
	}	
	stResourceLocation rl;
	rl.SetFileName( "data\\interfaces.gl" );
	rl.group = 33;
	if (GetScene()->FindCharacterByID(ptemp->tempid))
	{
		rl.frame = (GetScene()->FindCharacterByID(ptemp->tempid))->GetFace();
	}
	else
	{
		rl.frame = 1;
	}	
	if (GetImage(IDIMG_O_CHARA)) 
	{
		GetImage(IDIMG_O_CHARA)->SetImage(rl);
	}
	if (GetStatic(IDS_GAMENOW_BET))
	{
		GetStatic(IDS_GAMENOW_BET)->SetText( Money2String(ptemp->money));
	}
	if (GetStatic(IDS_SELF_MONEY))
	{
		GetStatic(IDS_SELF_MONEY)->SetText( Money2String(GetScene()->GetMainCharacter()->GetMoney()));
	}	
	FUNCTION_END;

}

void CMGNumberDlg::Start()
{
	FUNCTION_BEGIN;
	stContinueMiniGame cmd;
	cmd.ret = 1;
	SEND_USER_CMD( cmd );

	if (GetButton(IDB_MG_START))
	{
		GetButton(IDB_MG_START)->SetVisible(false);
		GetButton(IDB_MG_START)->SetEnabled(false);
	}

	FUNCTION_END;
}

void CMGNumberDlg::OnStart()
{
	FUNCTION_BEGIN;

	if (GetButton(IDB_MG_STOP))
	{
		GetButton(IDB_MG_STOP)->SetVisible(true);
		GetButton(IDB_MG_STOP)->SetEnabled(true);
	}

	m_bUpdateSelf = true;
	m_bUpdateO = true;

	FUNCTION_END;
}

void CMGNumberDlg::Stop()
{
	FUNCTION_BEGIN;
	stStopDiceMiniGame cmd;
	SEND_USER_CMD( cmd );
	if (GetButton(IDB_MG_STOP))
	{
		GetButton(IDB_MG_STOP)->SetEnabled(false);
	}				
	m_dwCurTime3 = LIMT_TIME;
	FUNCTION_END;
}

void CMGNumberDlg::Restart(DWORD dwMoney)
{
	FUNCTION_BEGIN;
	m_bResultDao = false;
	m_bResultDaoSelf = false;
	m_bResultDaoO = false;
	m_bUpdateSelf = true;
	m_bUpdateO = true;
	m_dwCurTime3 = LIMT_TIME;
	m_ImageSai[0].SetRandomPlay();
	m_ImageSai[0].SetSpeed(15);
	m_ImageSai[1].SetRandomPlay();
	m_ImageSai[1].SetSpeed(15);
	if (GetButton(IDB_MG_STOP))
	{
		GetButton(IDB_MG_STOP)->SetEnabled(true);
	}
	if (GetStatic(IDS_GAMENOW_BET))
	{
		GetStatic(IDS_GAMENOW_BET)->SetText( Money2String(dwMoney));
	}
	if (GetStatic(IDS_SELF_MONEY))
	{
		GetStatic(IDS_SELF_MONEY)->SetText( Money2String(GetScene()->GetMainCharacter()->GetMoney()));
	}
	FUNCTION_END;
}

void CMGNumberDlg::onResult(DWORD dwRes)
{
	FUNCTION_BEGIN;
	m_bResultDao = true;
	m_res = dwRes;
	FUNCTION_END;
}

void CMGNumberDlg::SetDiceNum(DWORD dwUserId,DWORD dwDiceNum)
{
	FUNCTION_BEGIN;
	if (dwUserId == m_selfid)
	{
		m_selfdicenum = dwDiceNum;
		m_bResultDaoSelf = true;
		GetButton(IDB_MG_STOP)->SetEnabled(false);
		m_dwCurTime3 = LIMT_TIME;
	}
	if (dwUserId == m_oid)
	{
		m_odicenum = dwDiceNum;
		m_bResultDaoO = true;
	}
	FUNCTION_END;
}

void CMGNumberDlg::ParseChatMessage(stChannelChatUserCmd* pCmd)
{
	FUNCTION_BEGIN;
	if (GetGameGuiManager()->m_MGNumberDlg)
	{
		GetChatManager().FilterChatMessage(pCmd);		
		TiXmlElement page("p");
		DWORD dwColor = GetChatColor(pCmd->dwType);
		GetChatManager().FormatMsgTitle(pCmd->pstrName,dwColor,&page);
		GetChatManager().FormatMsgContent2XML(pCmd,&page);
		GetGameGuiManager()->m_MGNumberDlg->AppendMessage(&page);
	}
	FUNCTION_END;
}

void CMGNumberDlg::AppendMessage(TiXmlElement* pPage)
{
	FUNCTION_BEGIN;
	CGuiMLTextBox* pMl = GetMLTextBox(IDML_CHATOUTPUT);
	if ( pMl )
	{
		pMl->AppendParagraph(pPage,NULL);
		pMl->ScrollToTail();
	}		
	FUNCTION_END;
}

bool CMGNumberDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;
	if( uMsg == WM_KEYDOWN && wParam == VK_RETURN )
	{
		CGuiEditBox* pEd = GetEditBox(IDE_CHATINPUT);
		if ( pEd && pEd->IsFocus() )
		{
			SendChat();
			return true;
		}
	}
	return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);
	FUNCTION_END;
}

void CMGNumberDlg::SendChat()
{
	FUNCTION_BEGIN;
	CGuiEditBox* pEdit = GetEditBox(IDE_CHATINPUT);
	const char* pstrName = GetScene()->GetMainCharacter()->GetName();
	CChatManager::SendChatCmd(pEdit,0,CHAT_TYPE_MINIGAME,pstrName);
	FUNCTION_END;
}