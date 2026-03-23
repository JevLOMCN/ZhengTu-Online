/**
 *\file		GuiChatSelect.cpp
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:12:00 中国标准时间
 *\brief	聊天界面
 *
 * 
 *
*/

#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include ".\guichatselect.h"
#include "./GuiMain.h"
#include "./GuiChatInvite.h"
#include "./GuiPrivateChat.h"
#include "./GuiFace.h"
#include "./GuiPrivateChatSelect.h"
#include ".\GuiPetCtrlDialog.h"
#include "./GuiChatSelectButton.h"
#include "../engine/include/crc.h"
#include "../engine/include/PhraseFilter.h"

#include "GuiMiniMap.h"

// 2010 9
#include "GameScene.h"
#include "MainCharacter.h"
#include "FindPath.h"
extern CFindPath g_FindPath;
extern std::string g_sFrom;
extern std::string g_sTo;
extern bool g_bAutoGo;
extern POINT g_NpcPosition;

extern POINT g_NpcFinal;
static void OnNpcFinal(void* pParam)
{
	CNpc* pNpc = GetScene()->GetObjectAtGrid<CNpc>( g_NpcFinal );
	if(pNpc)
		GetScene()->GetMainCharacter()->CallNpc(pNpc);
}

int g_iResetNum = 0;

/**
 * \brief 构造函数
 * 
 * 
 * 
 * \return 
 */
CGuiChatSelect::CGuiChatSelect(void)
: m_iScreenWidth(-1)
, m_pEditChat(NULL)
{
	FUNCTION_BEGIN;

	//for( int i=0;i<8;i++ )
	//	m_pBtnChatType[i] = NULL;

	FUNCTION_END;
}

/**
 * \brief 关闭对话框
 * 
 * 
 *
 * \return 
 */
void CGuiChatSelect::OnClose(void)
{
	FUNCTION_BEGIN;

	m_pEditChat = NULL;
	/*for( int i=0;i<8;i++ )
	{
		if ( m_pBtnChatType[i] )
			m_pBtnChatType[i] = NULL;
	}*/
	m_aChatBtnList.Clear();

	GetGameGuiManager()->m_guiChatSelect = NULL;

	FUNCTION_END;
}


BOOL CGuiChatSelect::ContainsPoint( POINT ptScreen )
{
	FUNCTION_BEGIN;

	if(CGuiDialog::ContainsPoint(ptScreen)) return TRUE;
	stPointI pt1 = ptScreen;
	pt1.x -= m_x + m_clientOffsetTopLeft.x;
	pt1.y -= m_y + m_clientOffsetTopLeft.y;
	CGuiControl* p = this->GetControlAtPoint(pt1);
	if(p)
		return TRUE;
	return FALSE;

	FUNCTION_END;
}

/**
 * \brief 创建对话框
 * 
 * 
 * 
 * \return 
 */
void CGuiChatSelect::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_pEditChat = GetEditBox( 3 );
	if ( m_pEditChat )
	{
		m_pEditChat->SetSaveHistory( true );
		m_pEditChat->SetMaxChatHistory( 20 );
		m_pEditChat->SetUsage(CGuiEditBox::USAGE_CHAT);
	}
	g_iResetNum=0;
	m_aChatBtnList.Clear();
	
	GetGameGuiManager()->AddPrivateChatSelect();

	OnResetScreenSize();


	FUNCTION_END;
}

/**
 * \brief 重设屏幕尺寸时调用
 * 
 * 
 * 
 * \return 
 */
void CGuiChatSelect::OnResetScreenSize()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnResetScreenSize();
    
	int iScreenWidth = GetDevice()->GetWidth();
	m_iScreenWidth = iScreenWidth;
	
	stResourceLocation rl;
	rl.SetFileName( "data\\Interfaces.gl" );
	int grp;
	if ( GetDevice()->GetWidth() == 800 )
	{
		grp = 105;
		//Reset back img
		if ( GetBackImage() )
		{
			rl.group = grp;
			rl.frame = GetBackImage()->GetImgFrame();
			SetBackImage( rl );
		}

		//Reset location
		SetLocation( GetDevice()->GetWidth()-GetWidth(), GetDevice()->GetHeight()-GetHeight()+40 );
		m_x = 0;
		//m_y = 0;
		SetVisible(true);

		m_aChatBtnList.SetLocation( stPointI(270,520) );

		m_pEditChat->SetLocation(290,526);
		m_pEditChat->SetSize(335,14);

		if ( GetButton(2) )
			GetButton(2)->SetLocation( 633, 520 );
	}
	else
	{
		grp = 5;
		//Reset back img
		if ( GetBackImage() )
		{
			rl.group = grp;
			rl.frame = GetBackImage()->GetImgFrame();
			SetBackImage( rl );
		}

		//Reset location
		SetLocation( GetDevice()->GetWidth()-GetWidth(), GetDevice()->GetHeight()-GetHeight()+40 );
		SetVisible(true);

		m_aChatBtnList.SetLocation( stPointI(378,689) );

		m_pEditChat->SetLocation(425,695);
		m_pEditChat->SetSize(385,14);

		if ( GetButton(2) )
			GetButton(2)->SetLocation( 811, 688 );
	}

	EndPopup();

	/*stResourceLocation rl;
	rl.group = iGroup;
	rl.frame = 0;
	rl.SetFileName(GetGuiGraphicPackName());
	SetBackImage(rl,NULL);
	UpdateLocation();
	for( size_t i=0 ; i < m_Controls.size() ; ++i )
	{
		if(m_Controls[i]->GetType() == GUI_CONTROL_BUTTON)
		{
			CGuiButton* pBtn = (CGuiButton*)m_Controls[i];
			if(pBtn->GetButtonType() == CGuiButton::ButtonType_Bitmap)
			{
				int frameNum;
				if(pBtn->GetBitmapInfo(rl,frameNum))
				{
					rl.group = iGroup;
					pBtn->SetBitmap(rl,frameNum);
				}
			}
		}
	}*/

	if (GetGameGuiManager()->m_guiPrivateChatSelect)
	{
		GetGameGuiManager()->m_guiPrivateChatSelect->SetLocation(
			m_iScreenWidth == 1024? 377:504	,GetDevice()->GetHeight() - 180 );
		
	}

	FUNCTION_END;
}

/**
 * \brief 初始化脚本
 * 
 * 
 * 
 * \return 
 */
void CGuiChatSelect::OnInitScript()
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}

/**
 * \brief 开始弹出对话框动画
 * 
 * 
 * 
 * \return 
 */
void CGuiChatSelect::BeginPopup()
{
	FUNCTION_BEGIN;

	stGuiAnimationInfo st;
	st.dwColorDst = GetColor();
	
	st.ptDst.x = m_x;
	st.ptDst.y = GetDevice()->GetHeight() - GetHeight();
	
	st.fTime = 0.5;
	st.eAniComplete = GuiAniEvent_RunFun;
	BeginAnimation( &st );

	m_aChatBtnList.TurnOffBtnList();
	if ( GetGameGuiManager()->m_guiChatButton )
		GetGameGuiManager()->m_guiChatButton->BeginPopup();

	FUNCTION_END;
}


void CGuiChatSelect::OnCompleteAniEvent()
{
	//SetZOrder( GUI_DIALOG_NOMOST );
}
/**
 * \brief 终止弹出对话框动画
 * 
 * 
 * 
 * \return 
 */
void CGuiChatSelect::EndPopup()
{
	FUNCTION_BEGIN;

	stGuiAnimationInfo st;
	st.dwColorDst = GetColor();
	
	st.ptDst.x = m_x;
	st.ptDst.y = GetDevice()->GetHeight() - GetHeight() + 40;

	st.fTime = 0.5;
	st.eAniComplete = GuiAniEvent_Hide;
	BeginAnimation( &st );

	m_aChatBtnList.TurnOffBtnList();
	if ( GetGameGuiManager()->m_guiChatButton )
		GetGameGuiManager()->m_guiChatButton->EndPopup();

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

/**
 * \brief 处理对话框控件事件
 * 
 * 
 * 
 * \param nEvent : 
 * \param nID : 
 * \param pControl : 
 * \return 
 */
bool CGuiChatSelect::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		if ( nID == 2 )
			OnOffChatFaceSelect( m_pEditChat );
		break;
	case EVENT_DIALOG_MOVE:
		if(GetGameGuiManager()->m_guiMain) 
			GetGameGuiManager()->m_guiMain->UpdateHelpBtnState();
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}



/**
 * \brief 发送聊天内容后处理
 * 
 * 
 * 
 * \return 
 */
void CGuiChatSelect::AfterSendChat()
{
	FUNCTION_BEGIN;

	//Set focus to main dlg
	if ( GetGameGuiManager()->m_guiMain )
		GetGuiManager()->RequestFocus( GetGameGuiManager()->m_guiMain );
	//close face dlg
	if ( GetGameGuiManager()->m_guiFace && GetGameGuiManager()->m_guiFace->IsVisible() )
		OnOffChatFaceSelect( m_pEditChat );
	//play push hide anim
	SetZOrder( GUI_DIALOG_BOTTOM );
	EndPopup();

	FUNCTION_END;
}

/**
 * \brief 对话框消息处理
 * 
 * 
 * 
 * \param hWnd : 
 * \param uMsg : 
 * \param wParam : 
 * \param lParam : 
 * \return 
 */
bool CGuiChatSelect::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;

	switch(uMsg)
	{
	case WM_KEYDOWN:

		//Here we treat all chat mode access key key-down
		if ( GetGameGuiManager()->m_guiChatButton )
		{
			if ( GetGameGuiManager()->m_guiChatButton->TreateAllChatModeAccessKey( wParam ) )
				return true;
		}
		//End treat

		switch(wParam)
		{
		case VK_RETURN:
			//if( m_pEditChat->IsFocus() && IsFocus() )
			{

				const char* pszText = m_pEditChat->GetText();

				char szCharText[512]={0};
				strncpy(szCharText,pszText,sizeof(szCharText));
				str_trim_left((char *)szCharText);
				str_trim_right((char *)szCharText);
				if(strlen(szCharText) == 0) 
				{
					AfterSendChat();
					return true;   // 如果为空就直接返回
				}


				if( CheckIndentify() )
				{
					m_pEditChat->ClearText();
					GetGameGuiManager()->AddClientSystemMessage("请不要重复发言！");
				}
				else
				{
					std::string strChat = szCharText;
					if ((strChat.find("/")==0)&&(strChat.find("/",1)!=1))
					{  // 私聊
                                             SendChat();
					}
					else 
					{
						if( m_curChatType == CHAT_TYPE_WORLD)
							GetGameGuiManager()->AddMessageBox("世界频道将花费银子10两,你确认？",eAction_SendCountryChat,NULL,c_szMBOKCancel);
					//	    GetGameGuiManager()->AddMessageBox("世界频道将花费10两,你确认？",eAction_SendCountryChat,NULL,c_szMBOKCancel);
						else 
							SendChat();
					}			

				}
				return true;
			}
			break;
		case VK_ESCAPE:
			{
				m_pEditChat->SetText( "" );
				AfterSendChat();
				return true;
			}			
			break;
		case VK_PRIOR:
		case VK_UP:
			//if ( GetKeyState(VK_SHIFT) < 0 )
			{
				if ( m_pEditChat )
					m_pEditChat->ChangeCurChatHistory( false );
			}
			break;
		case VK_NEXT:
		case VK_DOWN:
			//if ( GetKeyState(VK_SHIFT) < 0 )
			{
				if ( m_pEditChat )
					m_pEditChat->ChangeCurChatHistory( true );
			}
			break;
		}
		break;
	}

	bool bRet =  CGuiDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	if ((uMsg == WM_KEYDOWN)&&(wParam == 191))
	{
		if ( m_pEditChat )
		{
			if (m_pEditChat->GetCurrPos()==0)
			{
				if (GetGameGuiManager()->m_guiPrivateChatSelect)
				{
					if (GetGameGuiManager()->m_guiPrivateChatSelect->GetPrivateChatNameNum()>0)
					{
						GetGameGuiManager()->m_guiPrivateChatSelect->BeginPopup();
						GetGuiManager()->RequestFocus(GetGameGuiManager()->m_guiPrivateChatSelect);
						GetGameGuiManager()->m_guiPrivateChatSelect->SetFocus((CGuiControl*)GetGameGuiManager()->m_guiPrivateChatSelect->m_pListBox);
					}
				}
			}
		}
	}

	return bRet;

	FUNCTION_END;
}


void CGuiChatSelect::SendChat()
{
	FUNCTION_BEGIN;

	InternalSendChat();
	AfterSendChat();

	FUNCTION_END;
}
//void CGuiChatSelect::OnFilter()
//{ 
//#if 0 
//	const char * pword = m_pEditChat->GetText();
//	DWORD * pData = m_pEditChat->GetData();
//	char szPword[MAX_CHATINFO];
//	memset(szPword,0,sizeof(szPword));
//	if( G_PhraseFilterChat(pword,szPword,MAX_CHATINFO))
//	{
//		m_pEditChat->SetText(szPword,false,pData);
//	}
//#endif
//}

/**
 * \brief 发送聊天内容
 * 
 * 
 * 
 * \return 
 */
void CGuiChatSelect::InternalSendChat()
{
	FUNCTION_BEGIN;

	//OnFilter();

	if ( GetGameGuiManager()->m_guiMain )
		GetGameGuiManager()->m_guiMain->SendChat( m_pEditChat );

	FUNCTION_END;
}

void CGuiChatSelect::InsertPrivateChat(const char* name)
{
	FUNCTION_BEGIN;
	//
	if( NULL == name )
	{
		return;
	}
	//
	std::string  str = name;
	std::string  strMark="/";
	std::string  strChat=m_pEditChat->GetText();
	if (strlen(str.c_str())>0)
	{
		str = strMark + str;
		str += " ";
		if (strChat.find(strMark.c_str())!=0)
		{
			str += strChat;
		}
		else
		{
			int nPos = strChat.find(" ");
			if (nPos!=-1)
			{
				str += strChat.substr(nPos+1);
			}
		}
		m_pEditChat->SetText(str.c_str());
		if (IsVisible()==false)
			GetGameGuiManager()->m_guiMain->MsgProc(NULL,WM_KEYDOWN,VK_RETURN,0);
	}
	FUNCTION_END;
}

HRESULT CGuiChatSelect::OnRender( float fElapsedTime )
{
	//hide chat type btn
	//we render it ourself

	//Set focuse to chat select dlg
	//ChatEditSetFocus();

	return CGuiDialog::OnRender( fElapsedTime );
}

void CGuiChatSelect::ChatEditSetFocus()
{
	FUNCTION_BEGIN;
	GetGuiManager()->RequestFocus(this);
	GetGameGuiManager()->m_guiChatSelect->SetFocus((CGuiControl*)m_pEditChat);
	FUNCTION_END;
}

bool CGuiChatSelect::CheckIndentify()
{
	FUNCTION_BEGIN;
#ifdef _DEBUG
	return false;
#endif
	const char * pCurText = m_pEditChat->GetText();
	if ( NULL == pCurText )
		return false;
	if(strlen(pCurText) == 0) return false;
       
	stChatInfo curChatInfo;
	_snprintf( curChatInfo.szChatStr,sizeof(curChatInfo.szChatStr)-1, pCurText );
	curChatInfo.szChatStr[sizeof(curChatInfo.szChatStr)-1] =0;

	std::map<int,stChatTypeInfo>::iterator it = m_MapHistory.find(m_curChatType);

	if(it != m_MapHistory.end())
	{
		stChatTypeInfo & chatTypeInfo = it->second;
		chatTypeInfo.m_strChatHistory.push_back(curChatInfo);
		if( chatTypeInfo.m_strChatHistory.size() <= 3)
			return false;
		else if( chatTypeInfo.m_strChatHistory.size() > 4 )
			chatTypeInfo.m_strChatHistory.pop_front();
		assert(chatTypeInfo.m_strChatHistory.size()==4);
		    
		std::list<stChatInfo>::reverse_iterator iter;
		for(iter = chatTypeInfo.m_strChatHistory.rbegin(); iter != chatTypeInfo.m_strChatHistory.rend();iter++)
		{
                        stChatInfo & chatInfo = *iter;
			if( 0 != strcmp(chatInfo.szChatStr,pCurText))
				return false;
		}
		return true;
		 
	}
	else
	{
		stChatTypeInfo & chatTypeInfo = m_MapHistory[m_curChatType];
		chatTypeInfo.m_strChatHistory.push_back(curChatInfo);
		return false;
	}
	FUNCTION_END;
}

void CGuiChatSelect::SetChatType(enumChatType iChatType)
{
	FUNCTION_BEGIN;

	m_curChatType = iChatType;

	FUNCTION_END;
}
