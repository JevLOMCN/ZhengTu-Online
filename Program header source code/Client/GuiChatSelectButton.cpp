

#include "public.h"
#include "./GameAppation.h"
#include "./GameGuiManager.h"
#include "./GuiChatSelect.h"
#include "./GuiMain.h"
#include ".\guichatselectbutton.h"

static int g_nChatTypeUserSetting[][2] = {
	{USER_SETTING_CHAT_NINE,	CHAT_TYPE_NINE},		//公聊
	{USER_SETTING_CHAT_COUNTRY,	CHAT_TYPE_COUNTRY},		//国家
	{USER_SETTING_CHAT_AREA,	CHAT_TYPE_AREA},		//区域
	{USER_SETTING_CHAT_WHISPER,	CHAT_TYPE_PERSON},		//密聊
	{USER_SETTING_CHAT_UNION,	CHAT_TYPE_UNION},		//帮会
	{USER_SETTING_CHAT_TEAM,	CHAT_TYPE_TEAM},		//组队
	{USER_SETTING_CHAT_FAMILY,	CHAT_TYPE_FAMILY},		//家族
	//{USER_SETTING_CHAT_PRIVATE,	CHAT_TYPE_PRIVATE},		//私聊
	{USER_SETTING_CHAT_FRIEND,	CHAT_TYPE_FRIEND},		//好友
	{USER_SETTING_CHAT_SCHOOL,	CHAT_TYPE_OVERMAN},		//师门
	{USER_SETTING_CHAT_WORLD,	CHAT_TYPE_WORLD}		//世界
};


bool IsChatTypeClosed(DWORD type)
{
	for(size_t i=0;i<count_of(g_nChatTypeUserSetting);++i)
	{
		if( g_nChatTypeUserSetting[i][1] == type )
		{
			DWORD setting = g_nChatTypeUserSetting[i][0];
			return !GetGameApplication()->bSystemSetted((enumUserSetting)setting);
		}
	}
	return false;
}

///**
//* \brief 聊天按钮对话框开关
//* 
//* 聊天按钮对话框开关
//* 
//* \
//* \return 无
//*/
void OnOffChatSelectButton(stPointI pt)
{
	FUNCTION_BEGIN;

	return;

	if (!GetGameGuiManager()->m_guiChatButton)
		GetGameGuiManager()->AddChatButton();

	if(GetGameGuiManager()->m_guiChatButton->IsVisible())
	{
		GetGameGuiManager()->m_guiChatButton->EndPopup();
	}
	else
	{
		GetGameGuiManager()->m_guiChatButton->SetLocation(pt.x,pt.y);
		
		GetGameGuiManager()->m_guiChatButton->BeginPopup();
	}

	FUNCTION_END;
}

CGuiChatSelectButton::CGuiChatSelectButton(void)
{
	m_bCloseIsHide = true;
	
	m_bSmDialog = false;
}

void CGuiChatSelectButton::OnClose(void)
{
	m_aChatBtnList.Clear();
}

BOOL CGuiChatSelectButton::ContainsPoint( POINT ptScreen )
{
	FUNCTION_BEGIN;

	//if(CGuiDialog::ContainsPoint(ptScreen)) return TRUE;
	stPointI pt1 = ptScreen;
	pt1.x -= m_x + m_clientOffsetTopLeft.x;
	pt1.y -= m_y + m_clientOffsetTopLeft.y;
	CGuiControl* p = this->GetControlAtPoint(pt1);
	if(p)
		return TRUE;
	return FALSE;

	FUNCTION_END;
}


void CGuiChatSelectButton::BeginPopup()
{
	FUNCTION_BEGIN;

	m_aChatBtnList.TurnOffBtnList();

	//SetStyle( GUI_WS_SYSTEM + GUI_WS_NOMOVE + GUI_WS_BOTTOMMOST );
	SetZOrder( GUI_DIALOG_BOTTOMMOST );
	return;
	
	stGuiAnimationInfo st;
	st.dwColorDst = GetColor();

	st.ptDst.x = m_x;
	st.ptDst.y = GetDevice()->GetHeight() - GetHeight();

	st.fTime = 0.5;
	st.eAniComplete = GuiAniEvent_None;
	BeginAnimation( &st );	

	FUNCTION_END;
}


void CGuiChatSelectButton::EndPopup()
{
	FUNCTION_BEGIN;

	m_aChatBtnList.TurnOffBtnList();
	
	//SetStyle( GUI_WS_SYSTEM + GUI_WS_NOMOVE + GUI_WS_BOTTOMMOST );
	SetZOrder( GUI_DIALOG_BOTTOMMOST );
	return;

	stGuiAnimationInfo st;
	st.dwColorDst = GetColor();

	st.ptDst.x = m_x;
	st.ptDst.y = GetDevice()->GetHeight() - GetHeight() + 30;

	st.fTime = 0.5;
	st.eAniComplete = GuiAniEvent_Hide;
	BeginAnimation( &st );

	//CGuiDialog::EndPopup();

	

	FUNCTION_END;
}

void CGuiChatSelectButton::OnCreate()
{
	FUNCTION_BEGIN;

	m_aChatBtnList.Clear();
	m_aChatBtnList.SetType( BTN_LIST_TYPE_UP );
	m_aChatBtnList.SetSpace( 22 );

	

	for( int i=0;i<=CHAT_TYPE_NUM;i++ )
	{
		//m_pBtnChatType[i] = GetButton( 100+i );
		//if ( m_pBtnChatType[i] )
		//	m_pBtnChatType[i]->SetButtonGroup( 1 );
		m_aChatBtnList.AddOneButton( GetButton( 100+i ) );

		if ( GetButton( 100+i ) && GetButton( 100+i )->GetToolTips() )
		{
			GetButton( 100+i )->GetToolTips()->SetAlignType( GUI_ALIGN_LEFT );
		}
	}
	
	m_aChatBtnList.SetLocation( stPointI(0,(m_aChatBtnList.GetBtnNum()-1)*22) );		//378,689
	m_aChatBtnList.SetActiveBtn( GetButton(100) );

	if ( GetImage( 2 ) )
	{
		GetImage( 2 )->SetLocation( 0, (m_aChatBtnList.GetBtnNum()-1)*22);
	}

	OnResetScreenSize();
	
	FUNCTION_END;
}


void CGuiChatSelectButton::OnResetScreenSize()
{
	FUNCTION_BEGIN;

	if ( GetButton( 16 ) )
		GetButton( 16 )->SetLocation( -1000, -1000 );
	
	if ( GetDevice()->GetWidth() == 800 )
	{
		if ( GetGameGuiManager()->m_guiChatSelect )	//
		{
			SetLocation( GetGameGuiManager()->m_guiChatSelect->GetLocation().x + 240 ,
				GetGameGuiManager()->m_guiChatSelect->GetLocation().y + 520 - (m_aChatBtnList.GetBtnNum()-1)*22 );
		}
		else
		{
			SetLocation( GetDevice()->GetWidth() - 1024 + 378,
				GetDevice()->GetHeight() - 768 + 688 - (m_aChatBtnList.GetBtnNum()-1)*22 );
		}
	}
	else
	{
		if ( GetGameGuiManager()->m_guiChatSelect )	//
		{
			SetLocation( GetGameGuiManager()->m_guiChatSelect->GetLocation().x + 378 ,
						GetGameGuiManager()->m_guiChatSelect->GetLocation().y + 688 - (m_aChatBtnList.GetBtnNum()-1)*22 );
		}
		else
		{
			SetLocation( GetDevice()->GetWidth() - 1024 + 378,
						GetDevice()->GetHeight() - 768 + 688 - (m_aChatBtnList.GetBtnNum()-1)*22 );
		}
	}
	

	FUNCTION_END;
}

HRESULT CGuiChatSelectButton::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;

	if ( GetBackImage() )
	{
		if ( !m_aChatBtnList.IsPop() )
			GetBackImage()->SetVisible( false );
		else
			GetBackImage()->SetVisible( true );
	}

	OnResetScreenSize();
	

	return CGuiDialog::OnRender( fElapsedTime );

	FUNCTION_END;
}

static int g_nChatTypeID[][2]={
	{CHAT_TYPE_PRIVATE	,101	},	/// 私聊频道
	{CHAT_TYPE_NINE		,100	},	/// 轻聊频道
	{CHAT_TYPE_TEAM		,103	},	/// 队伍频道
	{CHAT_TYPE_FRIEND	,104	},	/// 好友频道
	{CHAT_TYPE_GM		,0	},	/// GM聊频道
	{CHAT_TYPE_SYSTEM	,0	},	/// 系统频道
	{CHAT_TYPE_UNION	,102	},	/// 行会频道
	{CHAT_TYPE_POP		,0	},	/// 弹出式系
	{CHAT_TYPE_PERSON	,0	},	/// 个人频道
	{CHAT_TYPE_COUNTRY	,105	},	/// 国家频道
	{CHAT_TYPE_AREA	,106	},	/// 地区频道（原师门频道）
	{CHAT_TYPE_FAMILY	,107	},	/// 家族频道
	{CHAT_TYPE_WORLD        ,108    },      /// 世界频道
	{CHAT_TYPE_OVERMAN	,109	},	/// 师门频道
	{CHAT_TYPE_SYTEM	,0	},	/// 系统公告频道
};

void ChangeActiveChatModeBtn()
{
	FUNCTION_BEGIN;

	//We limit send msg frequency
	static DWORD tLastTimeSendChatMode = xtimeGetTime() - 1000;

	if ( xtimeGetTime()-tLastTimeSendChatMode < 500 )
		return;

	//Get cur chat btn
	int curBtn = 0;
	if ( GetGameGuiManager()->m_guiMain )
	{
		int curType = GetGameGuiManager()->m_guiMain->GetChatType();
		for(size_t i=0;i<count_of(g_nChatTypeID);++i)
		{
			if( g_nChatTypeID[i][0] == curType )
			{
				curBtn = g_nChatTypeID[i][1]-100;
				break;
			}
		}
		if ( curBtn < 0 )
			curBtn = 0;
	}	

	//Change chat type btn
	if ( (curBtn >=0) && (curBtn<=CHAT_TYPE_NUM) )
	{
		GetGameGuiManager()->m_guiChatButton->ChangeActiveChatModeBtn( curBtn );
	}

	tLastTimeSendChatMode = xtimeGetTime();

	FUNCTION_END;
}


bool CGuiChatSelectButton::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		if(nID >= 100 && nID <= 100+CHAT_TYPE_NUM)
		{
			SetZOrder( GUI_DIALOG_TOPMOST );
			for(size_t i=0;i<count_of(g_nChatTypeID);++i)
			{
				if( g_nChatTypeID[i][1] == nID )
				{
					int type = GetGameGuiManager()->m_guiMain->GetChatType();
					if( type != g_nChatTypeID[i][0] &&
						IsChatTypeClosed(g_nChatTypeID[i][0]) )
					{
						GetGameGuiManager()->AddClientSystemMessage("该频道已经被你屏蔽了,要开启频道请到系统设置-聊天页面");
					}
					else
					{
						if((enumChatType)g_nChatTypeID[i][0] != CHAT_TYPE_PRIVATE)
						{
							m_aChatBtnList.OnBtnClickEvent( GetButton(nID) );
							GetGameGuiManager()->m_guiMain->SetChatType((enumChatType)g_nChatTypeID[i][0]);
						}
						//Set focus to chat edit box
						if (GetGameGuiManager()->m_guiChatSelect)
						{
							GetGameGuiManager()->m_guiChatSelect->SetChatType((enumChatType)g_nChatTypeID[i][0]);
							GetGuiManager()->RequestFocus(GetGameGuiManager()->m_guiChatSelect);
							GetGameGuiManager()->m_guiChatSelect->SetFocus((CGuiControl*)GetGameGuiManager()->m_guiChatSelect->GetEditChat());
						}
					}

					return true;
				}
			} 
		}
		else
		{
			if ( nID == 16 )
				::ChangeActiveChatModeBtn();
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiChatSelectButton::ChangeActiveChatModeBtn( int type )
{
	FUNCTION_BEGIN;

	//Get next btn according to type
	int nextBtn = type+1;
	if ( nextBtn > CHAT_TYPE_NUM )
		nextBtn = 0;
		
	while ( NULL == GetButton(100+nextBtn) )
		nextBtn++ ;
	//
	//if ( GetButton(100+nextBtn) )
	{
		m_aChatBtnList.SetActiveBtn( GetButton(100+nextBtn) );
		for(size_t i=0;i<count_of(g_nChatTypeID);++i)
		{
			if(g_nChatTypeID[i][1] == 100+nextBtn)
			{
				GetGameGuiManager()->m_guiMain->SetChatType((enumChatType)g_nChatTypeID[i][0]);
				GetGameGuiManager()->m_guiChatSelect->SetChatType((enumChatType)g_nChatTypeID[i][0]);
				break;
			}
		}
	}

	FUNCTION_END;
}

void CGuiChatSelectButton::JumpToChatMode( int mode )
{
	FUNCTION_BEGIN;

	int btnID = -1;
	for(size_t i=0;i<count_of(g_nChatTypeID);++i)
	{
		if( g_nChatTypeID[i][0] == mode )
		{
			btnID = g_nChatTypeID[i][1];
			break;
		}
	}

	if ( btnID == -1 )
		return;

	if ( NULL == GetButton( btnID ) )
		return;

	//Set active btn
	m_aChatBtnList.SetActiveBtn( GetButton( btnID ) );
	//Change chat mode
	GetGameGuiManager()->m_guiMain->SetChatType( (enumChatType)g_nChatTypeID[i][0] );
	GetGameGuiManager()->m_guiChatSelect->SetChatType((enumChatType)g_nChatTypeID[i][0]);

	FUNCTION_END;
}

static int g_nChatTypeAccessKey[][2]={
	{CHAT_TYPE_PRIVATE	,-1	},	/// 私聊频道
	{CHAT_TYPE_NINE		,'1' },	/// 轻聊频道
	{CHAT_TYPE_TEAM		,'3' },	/// 队伍频道
	{CHAT_TYPE_FRIEND	,'4' },	/// 好友频道
	{CHAT_TYPE_GM		,-1	},	/// GM聊频道
	{CHAT_TYPE_SYSTEM	,-1	},	/// 系统频道
	{CHAT_TYPE_UNION	,'6' },	/// 行会频道
	{CHAT_TYPE_POP		,-1	},	/// 弹出式系
	{CHAT_TYPE_PERSON	,-1	},	/// 个人频道
	{CHAT_TYPE_COUNTRY	,'5' },	/// 国家频道
	{CHAT_TYPE_AREA		,'2' },	/// 地区频道（原师门频道）
	{CHAT_TYPE_FAMILY	,'7' },	/// 家族频道
	{CHAT_TYPE_WORLD    ,'9' },  /// 世界频道
	{CHAT_TYPE_OVERMAN	,'8' },	/// 师门频道
	{CHAT_TYPE_SYTEM	,-1	},	/// 系统公告频道
};

       

bool CGuiChatSelectButton::TreateAllChatModeAccessKey( long key )
{
	FUNCTION_BEGIN;

	if ( GetKeyState( VK_CONTROL ) >=0 )
		return false;

	static DWORD tLastSwitchChatModeTime = xtimeGetTime()-1000;
	if ( (xtimeGetTime()-tLastSwitchChatModeTime) < 200 )
		return true;

	for(size_t i=0;i<count_of(g_nChatTypeAccessKey);++i)
	{
		if( g_nChatTypeAccessKey[i][1] == key )
		{
			JumpToChatMode( g_nChatTypeAccessKey[i][0] );

			tLastSwitchChatModeTime = xtimeGetTime();
			return true;
		}
	}

	return false;

	FUNCTION_END;
}