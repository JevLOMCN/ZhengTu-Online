/**
*\file		GuiChatOutput.cpp
*\version	$Id$
*\author	$$$02公司
*\			$$$02公司@###.com
*\date		2005-7-5   17:11:00 中国标准时间
*\brief	聊天输出界面
*
* 
*
*/

#include "public.h"
#include "../gui/include/guiTypes.h"
#include "game_script.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include ".\guichatoutput.h"
#include "./GuiItemDetailInfo.h"
#include "./command.h"
#include "../xml_parse/XmlParseEx.h"
#include "./GameAppation.h"
#include "./GuiChatSelect.h"
#include "./MainCharacter.h"
#include "./RoleItem.h"
#include "./CharacterProperty.h"
#include "./GuiTeam.h"
#include "GuiWorldConn.h"
#include "../engine/include/PhraseFilter.h"
#include "./Chat.h"
#include "./GuiPrivateChatSelect.h" 

#define MAX_TEXT_LINE   128

extern bool g_bSinglePlayer;

#define  SET_BUTTON_LOCATION(id,x,y)	{CGuiButton* p = GetButton(id);if(p) p->SetLocation(x,y);}
#define  SET_BUTTON_CHECK(id,b)			{CGuiButton* p = GetButton(id);if(p) p->SetChecked(b);}
#define  SET_BUTTON_VISIBLE(id,b)		{CGuiButton* p = GetButton(id);if(p) p->SetVisible(b);}
#define  SET_BUTTON_DELAYHIDE(id)		{CGuiButton* p = GetButton(id);if(p) p->HideDelay(5000);}


struct stChatOutCmdItem
{
	enum enumCMD
	{
		CMD_GROUP_HONOR, // 荣誉模式组队
		CMD_GROUP_NORMAL, // 组队模式组队
		CMD_ADDFRIEND,   // 添加好友
		CMD_PRIVATECHAT, // 密聊
		CMD_COPY_USERNAME,	//复制人名
		CMD_NUM
	};
	enumCMD		cmd;
	std::string     strCmd;
	bool            bCanUse;
};

static stChatOutCmdItem s_chat_cmdItem[] = 
{
	{ stChatOutCmdItem::CMD_GROUP_HONOR,	"荣誉组队", true}, // 荣誉模式组队
	{ stChatOutCmdItem::CMD_GROUP_NORMAL,	"普通组队", true}, // 组队模式组队
	{ stChatOutCmdItem::CMD_ADDFRIEND,		"加为好友", true}, // 加为好友
	{ stChatOutCmdItem::CMD_PRIVATECHAT,	"密聊",		true},  // 密聊
	{ stChatOutCmdItem::CMD_COPY_USERNAME,	"复制人名",	true},  // 复制人名
};


void  InsertPrivateChat(const char* name)
{
	if ((GetGameGuiManager()->m_guiChatSelect)&&(name!=NULL))
	{	
		GetGameGuiManager()->m_guiChatSelect->InsertPrivateChat(name);
		GetGameGuiManager()->m_guiChatSelect->ChatEditSetFocus();
	}
}

/**
* \brief 构造函数
* 
* 
* 
* \return 
*/
CGuiChatOutput::CGuiChatOutput(void)
{
	FUNCTION_BEGIN;

	m_nSBWidth = 22;
	m_clientOffsetTopLeft = stPointI(3,18);
	m_clientOffsetRightBottom = stPointI(3,3);
	m_ptCaptionOffset = stPointI(0,0);
	m_minSize = stPointI(200,100);

	stGuiCtrlBorderInfo* pBorderInfo = GetCustomCtrlBorderInfo(GUI_CONTROL_MLEDITBOX);
	pBorderInfo->rcTexture = stRectI(288,123,310,176);
	pBorderInfo->ptLeftTop = stPointI(2,2);
	pBorderInfo->ptRightBottom = stPointI(2,2);

	m_pEditOutput[0] = NULL;
	m_pEditOutput[1] = NULL;
	m_pEditOutput[2] = NULL;

	m_szPrivateName[0] = 0;
	m_eState = 0;

	m_bEnableScroll[0] = true;
	m_bEnableScroll[1] = true;
	m_bEnableScroll[2] = true;

	m_bOnlyGmMsg[0] = true;
	m_bOnlyGmMsg[1] = true;
	m_bOnlyGmMsg[2] = true;

	m_pMenu = NULL;
	m_bPopMenu = false;
	m_pTab = NULL;

	m_pConfigMenu[0] = NULL;
	m_pConfigMenu[1] = NULL;

	m_tBeginTime = 0;
	m_tBeginOutTime = 0;

	FUNCTION_END;
}

/**
* \brief 创建对话框
* 
* 
* 
* \return 
*/
void CGuiChatOutput::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	m_pEditOutput[0] = GetMLTextBox(1);
	m_pEditOutput[1] = GetMLTextBox(11);
	m_pEditOutput[2] = GetMLTextBox(21);

	//m_strNameList.clear();

	OnGuiEvent(EVENT_BUTTON_CLICKED,2,NULL);

	POINT pt2 = GetButton(2)->GetLocation();
    //soke 没改变区域显示的
	SET_BUTTON_LOCATION( 2,pt2.x,pt2.y);
	SET_BUTTON_LOCATION( 3,pt2.x+17,pt2.y-17);
	SET_BUTTON_LOCATION( 4,pt2.x+34,pt2.y-35);
	SET_BUTTON_LOCATION( 5,20,0);
	SET_BUTTON_LOCATION( 6,96,0);
	SET_BUTTON_LOCATION( 7,172,0);
	SET_BUTTON_VISIBLE(5,false);
	SET_BUTTON_VISIBLE(6,false);
	SET_BUTTON_VISIBLE(7,false);

	m_bEnableScroll[0] = true;
	m_bEnableScroll[1] = true;
	m_bEnableScroll[2] = true;

	m_bOnlyGmMsg[0] = false;
	m_bOnlyGmMsg[1] = false;
	m_bOnlyGmMsg[2] = false;

	m_pEditOutput[0]->SetFilter(m_bOnlyGmMsg[0]);
	m_pEditOutput[1]->SetFilter(m_bOnlyGmMsg[1]);
	m_pEditOutput[2]->SetFilter(m_bOnlyGmMsg[2]);

	m_pMenu = GetGuiManager()->CreateMenu( NULL );

	for ( int i = 0 ; i < count_of(s_chat_cmdItem) ; i++ )
	{
		m_pMenu->AddMenuItem( i, s_chat_cmdItem[i].strCmd.c_str(), CGuiMenu::MenuStyle_Command, s_chat_cmdItem[i].bCanUse );
	}
	SetPopupMenu( m_pMenu );

	m_pTab = GetTab( 8 );
	if ( m_pTab )
	{
		m_pTab->SetCurItem( 0 );
		m_pTab->SetVisible( false );
	}


	m_map1[CHAT_TYPE_NINE] = true;
	m_map1[CHAT_TYPE_AREA] = true;
	m_map1[CHAT_TYPE_TEAM] = true;
	m_map1[CHAT_TYPE_FRIEND] = true;
	m_map1[CHAT_TYPE_COUNTRY] = true;
	m_map1[CHAT_TYPE_UNION] = true;
	m_map1[CHAT_TYPE_FAMILY] = true;
	m_map1[CHAT_TYPE_OVERMAN] = true;
	m_map1[CHAT_TYPE_WORLD] = true;

	m_map2[CHAT_TYPE_NINE] = true;
	m_map2[CHAT_TYPE_AREA] = true;
	m_map2[CHAT_TYPE_TEAM] = true;
	m_map2[CHAT_TYPE_FRIEND] = true;
	m_map2[CHAT_TYPE_COUNTRY] = true;
	m_map2[CHAT_TYPE_UNION] = true;
	m_map2[CHAT_TYPE_FAMILY] = true;
	m_map2[CHAT_TYPE_OVERMAN] = true;
	m_map2[CHAT_TYPE_WORLD] = true;


	for( int i = 0; i < 2; i ++ )
	{
		m_pConfigMenu[i] = GetGuiManager()->CreateMenu( this );
		int base = 100 + i*200;
		if( m_pConfigMenu[i] )
		{
			m_pConfigMenu[i]->AddMenuItem(base+CHAT_TYPE_NINE,"轻声频道",CGuiMenu::MenuStyle_Command);
			m_pConfigMenu[i]->AddMenuItem(base+CHAT_TYPE_AREA,"地区频道",CGuiMenu::MenuStyle_Command);
			m_pConfigMenu[i]->AddMenuItem(base+CHAT_TYPE_TEAM,"组队频道",CGuiMenu::MenuStyle_Command);
			m_pConfigMenu[i]->AddMenuItem(base+CHAT_TYPE_FRIEND,"好友频道",CGuiMenu::MenuStyle_Command);
			m_pConfigMenu[i]->AddMenuItem(base+CHAT_TYPE_COUNTRY,"国家频道",CGuiMenu::MenuStyle_Command);
			m_pConfigMenu[i]->AddMenuItem(base+CHAT_TYPE_UNION,"帮会频道",CGuiMenu::MenuStyle_Command);
			m_pConfigMenu[i]->AddMenuItem(base+CHAT_TYPE_FAMILY,"家族频道",CGuiMenu::MenuStyle_Command);
			m_pConfigMenu[i]->AddMenuItem(base+CHAT_TYPE_OVERMAN,"师门频道",CGuiMenu::MenuStyle_Command);
			m_pConfigMenu[i]->AddMenuItem(base+CHAT_TYPE_WORLD,"世界频道",CGuiMenu::MenuStyle_Command);
			m_pConfigMenu[i]->AddMenuItem(0,"",CGuiMenu::MenuStyle_Separate);	
			m_pConfigMenu[i]->AddMenuItem(base+100,"全选",CGuiMenu::MenuStyle_Command);
			m_pConfigMenu[i]->AddMenuItem(base+101,"清除",CGuiMenu::MenuStyle_Command);

			m_pConfigMenu[i]->SetAllCheck();
			m_pConfigMenu[i]->SetMenuItemCheck(base+100,false);
			m_pConfigMenu[i]->SetMenuItemCheck(base+101,false);

			//m_pConfigMenu[i]->SetMenuItemCheck(base+CHAT_TYPE_NINE,true);
			//m_pConfigMenu[i]->SetMenuItemCheck(base+CHAT_TYPE_AREA,true);
		}
	}

	OnGuiEvent(EVENT_BUTTON_CLICKED,5,NULL);


	FUNCTION_END;
}

/**
* \brief 关闭对话框
* 
* 
* 
* \return 
*/
void CGuiChatOutput::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiChatOutput = NULL;

	if ( m_pConfigMenu[0] )
	{
		m_pConfigMenu[0]->Close();
		m_pConfigMenu[0] = NULL;
	}

	if ( m_pConfigMenu[1] )
	{
		m_pConfigMenu[1]->Close();
		m_pConfigMenu[1] = NULL;
	}

	FUNCTION_END;
}

/**
* \brief 重设窗口时调用
*
* 
* 
* \return 
*/
void CGuiChatOutput::OnResetScreenSize()
{
	FUNCTION_BEGIN;

	const int nOffsetY = 100;
	CGuiDialog::OnResetScreenSize();

	this->SetLocation(0,GetDevice()->GetHeight()-nOffsetY-GetHeight()-25);

	FUNCTION_END;
}

void CGuiChatOutput::OnMenuCmd(UINT nID)
{
	switch(nID)
	{
	case stChatOutCmdItem::CMD_GROUP_NORMAL:// 组队模式组队
		GetGameGuiManager()->AddTeam();
		if(GetGameGuiManager()->m_guiTeam)
		{
			GetGameGuiManager()->m_guiTeam->ToAddTeamMember(m_strChatUser.c_str(),TEAM_NORMAL);
		}
		break;
	case stChatOutCmdItem::CMD_GROUP_HONOR:// 荣誉模式组队
		GetGameGuiManager()->AddTeam();
		if(GetGameGuiManager()->m_guiTeam)
		{
			GetGameGuiManager()->m_guiTeam->ToAddTeamMember(m_strChatUser.c_str(),TEAM_HONOR);
		}
		break;
	case stChatOutCmdItem::CMD_ADDFRIEND:
		AddFriend(m_strChatUser.c_str());
		break;
	case stChatOutCmdItem::CMD_PRIVATECHAT:
		InsertPrivateChat( m_strChatUser.c_str() );
		break;
	case stChatOutCmdItem::CMD_COPY_USERNAME:
		{
			Copy2Clipboard( m_strChatUser.c_str() );
		}
		break;
	case 200:
		{
			for( tMapChatConfig::iterator it = m_map1.begin();
				it != m_map1.end(); ++it )
			{
				it->second = true;
			}
			if( m_pConfigMenu[0] )
			{
				m_pConfigMenu[0]->SetAllCheck();
				m_pConfigMenu[0]->SetMenuItemCheck(200,false);
				m_pConfigMenu[0]->SetMenuItemCheck(201,false);
			}
		}
		break;
	case 201:
		{
			for( tMapChatConfig::iterator it = m_map1.begin();
				it != m_map1.end(); ++it )
			{
				it->second = true;
			}
			if( m_pConfigMenu[0] ) m_pConfigMenu[0]->ClearAllCheck();
		}
		break;
	case 400:
		{
			for( tMapChatConfig::iterator it = m_map2.begin();
				it != m_map2.end(); ++it )
			{
				it->second = true;
			}
			if( m_pConfigMenu[1] )
			{
				m_pConfigMenu[1]->SetAllCheck();
				m_pConfigMenu[1]->SetMenuItemCheck(400,false);
				m_pConfigMenu[1]->SetMenuItemCheck(401,false);
			}
		}
		break;
	case 401:
		{
			for( tMapChatConfig::iterator it = m_map2.begin();
				it != m_map2.end(); ++it )
			{
				it->second = false;
			}
			if( m_pConfigMenu[1] ) m_pConfigMenu[1]->ClearAllCheck();
		}
		break;
	default:
		{
			if( nID >= 100 && nID < 200 )
			{
				tMapChatConfig::iterator it = m_map1.find( nID - 100 );
				if( it != m_map1.end() )
				{					
					bool b = it->second;
					it->second = !b;
					if( m_pConfigMenu[0] )
					{
						m_pConfigMenu[0]->SetMenuItemCheck( nID,!b);
						Debug_Int(nID);
						Debug_Bool(b);
					}	
				}
			}
			if( nID >= 300 && nID < 400 )
			{
				tMapChatConfig::iterator it = m_map2.find( nID - 300 );
				if( it != m_map2.end() )
				{					
					bool b = it->second;
					it->second = !b;
					if( m_pConfigMenu[1] )
					{
						m_pConfigMenu[1]->SetMenuItemCheck( nID,!b);
						Debug_Int(nID);
						Debug_Bool(b);
					}	
				}
			}
		}
		break;
	}
}

/**
* \brief 对话框控件事件处理
* 
* 
* 
* \param nEvent : 
* \param nID : 
* \param pControl : 
* \return 
*/
bool CGuiChatOutput::OnGuiEvent( UINT nEvent,UINT nID,CGuiControl* pControl )
{
	FUNCTION_BEGIN;

	switch( nEvent )
	{
	case EVENT_DIALOG_COMMAND:
		{
			OnMenuCmd( nID );
		}
		break;
	case EVENT_DIALOG_RESIZE:			//当窗口大小变化时产生
		{
			for ( int i = 0; i < 3; i ++ )
			{
				if ( m_pEditOutput[i] )
				{
					m_pEditOutput[i]->SetLocation( 0, 20 );
					m_pEditOutput[i]->SetSize( GetWidth() , GetHeight() - 50 );
					if( m_bEnableScroll[i] )
						m_pEditOutput[i]->ScrollToTail();
				}
			}
			CGuiButton* pButton = GetButton(2);
			if (pButton)
			{
			    //soke 改变区域显示的
				pButton->SetLocation(5,GetHeight()-30);
				POINT pt2 = pButton->GetLocation();			
                SET_BUTTON_LOCATION( 2,pt2.x-12,pt2.y-50);
				SET_BUTTON_LOCATION( 3,pt2.x+5,pt2.y-67);
				SET_BUTTON_LOCATION( 4,pt2.x+22,pt2.y-85);
				SET_BUTTON_LOCATION( 5,20,0);
				SET_BUTTON_LOCATION( 6,96,0);
				SET_BUTTON_LOCATION( 7,172,0);
			}
			OnResetScreenSize();
		}
		break;
	case EVENT_SHELLOBJECT_CLICKED:			//处理媒体链接点击事件
		OpenMediaExchangeHyperLink( nID );
		break;
	case EVENT_ITEM_CLICKED:
		{			
			if ( m_pTab )
			{
				int index = m_pTab->GetCurItem();
				if ( index >= 0 && index < 3 )
				{
					if ( m_pEditOutput[index] )
					{
						InsertPrivateChat((char*)nID);
					}
				}
			}
		}
		break;
	case EVENT_ITEM_RIGHT_CLICKED:
		{
			m_strChatUser = (char*)nID;
			m_bPopMenu = true;
		}
		break;
	case EVENT_BUTTON_CLICKED:
		OnButtonClick( nID );
		break;
	case EVENT_RBUTTON_CLICKED:
		{
			if( nID == 6 ) 
			{
				if( m_pConfigMenu[0] )
				{
					SetPopupMenu( m_pConfigMenu[0] );
					m_pConfigMenu[0]->Popup();
				}
			}
			if( nID == 7 ) 
			{
				if( m_pConfigMenu[1] )
				{
					SetPopupMenu( m_pConfigMenu[1] );
					m_pConfigMenu[1]->Popup();
				}
			}
		}
		break;
	default:
		break;
	}
	return true;

	FUNCTION_END;
}


void CGuiChatOutput::UpdateMakeGroupMenuState(CGuiMenu * pMenu,int * pBtn,int BtnSize)
{
	FUNCTION_BEGIN;


	pMenu->SetMenuEnabled(pBtn[0],true);  // 普通组队
	CCharacter * pSelCharacter = GetScene()->FindCharacterByName(m_strChatUser.c_str());

	bool bCanMakeHonorGroup = false;
	if( !pSelCharacter)
		bCanMakeHonorGroup = false;
	else 
		bCanMakeHonorGroup = CanMakeGroupWithMe(pSelCharacter->GetLevel(),TEAM_HONOR);

	pMenu->SetMenuEnabled(pBtn[1],bCanMakeHonorGroup); // 荣毅组队

	FUNCTION_END;
}


bool CGuiChatOutput::OnContextMenu()
{
	FUNCTION_BEGIN;


	if( m_bPopMenu)
	{
		m_bPopMenu = false;
		int nBtn[] =
		{
			stChatOutCmdItem::CMD_GROUP_NORMAL, // 组队模式组队
				stChatOutCmdItem::CMD_GROUP_HONOR, // 荣誉模式组队

		};
		UpdateMakeGroupMenuState(m_pMenu,nBtn,count_of(nBtn));
		m_pMenu->SetMenuEnabled(stChatOutCmdItem::CMD_ADDFRIEND,true); // 添加好友
		SetPopupMenu( m_pMenu );
		m_pMenu->Popup();

		return true;
	}
	else
	{
		if ( GetGameGuiManager()->m_guiTeam )
		{
			if ( GetGameGuiManager()->m_guiTeam->OnContextMenu() )
				return true;
		}
	}

	return false;

	FUNCTION_END;
}


/**
* \brief 打开交易道具链接
* 
* 
* 
* \param dwID : 道具id
* \return 
*/
void CGuiChatOutput::OpenMediaExchangeHyperLink( const DWORD& dwID )
{
	FUNCTION_BEGIN;

	//This time we just send server object temp id
	mapItemExchange::iterator	itor;
	itor = g_itemExchangeList.find( dwID );

	if ( itor != g_itemExchangeList.end() )
	{
		stQuestionObject cmd;
		cmd.dwObjectTempID = dwID;
		sprintf( cmd.name, itor->second.ownerName );

		SEND_USER_CMD( cmd );
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
void CGuiChatOutput::OnInitScript()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
* \brief 渲染对话框背景
* 
* 
* 
* \param fElapsedTime : 
* \return 
*/
void CGuiChatOutput::RenderBackground(float fElapsedTime)
{
	FUNCTION_BEGIN;

	//stRectI rect(0,0,GetWidth()+5,GetHeight()+10);
	//rect.OffsetRect(m_x-5,m_y);

	//GetDevice()->FillRect(&rect,COLOR_ARGB(128,0,0,0));

	FUNCTION_END;
}

/**
* \brief 绘制对话框
* 
* 
* 
* \param fElapsedTime : 
* \return 
*/
HRESULT CGuiChatOutput::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	stPointI ptCursor = Engine_GetCursor()->GetPosition();

	stPointI ptOldCursor = ptCursor;
	ptCursor.x -= GetClientX();
	ptCursor.y -= GetClientY();

	bool b = false;
	CGuiButton* pBtn = GetButton( 5 );
	if( pBtn )
	{
		b = pBtn->IsVisible();
	}
	stRectI rc(20,0,228,20);
	if( rc.PtInRect(ptCursor) &&
		(xtimeGetTime() - m_tBeginTime) > 2000 	)
	{
		SET_BUTTON_VISIBLE(5,true);
		SET_BUTTON_VISIBLE(6,true);
		SET_BUTTON_VISIBLE(7,true);
		m_tBeginOutTime = 0;
	}
	else
	{
		if( b && (m_pConfigMenu[0] && !m_pConfigMenu[0]->IsVisible()) &&
			(m_pConfigMenu[1] && !m_pConfigMenu[1]->IsVisible()) )
		{
			SET_BUTTON_DELAYHIDE(5);
			SET_BUTTON_DELAYHIDE(6);
			SET_BUTTON_DELAYHIDE(7);
		}
	}

	if ( m_pTab )
	{
		int index = m_pTab->GetCurItem();
		if ( index >= 0 && index < 3 )
		{	
			CGuiButton * pBtn1 = GetButton(3);
			if( pBtn1 && pBtn1->ContainsPoint(ptCursor))
			{
				CToolTips * pToolTips = pBtn1->GetToolTips();
				pToolTips->Clear();
				pToolTips->SetBkColor( D3DCOLOR_ARGB(128,0,0,0) );
				pToolTips->SetAlignType( GUI_ALIGN_LEFT );
				pToolTips->SetBorderColor( 0 );
				pToolTips->SetLineGaps( 3 );
				pToolTips->SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
				if( m_bEnableScroll[index] )
					pToolTips->AddText( "单击左键信息滚动关闭");
				else 
					pToolTips->AddText( "单击左键信息滚动开启");
				pToolTips->Resize();
				ptCursor.x -= pToolTips->GetWidth();
				ptOldCursor.y -= pToolTips->GetHeight()+2;
			}
			else
			{
				CGuiButton * pBtn2 = GetButton(4);
				if( pBtn2 && pBtn2->IsMouseOver())
				{
					CToolTips * pToolTips = pBtn2->GetToolTips();
					pToolTips->Clear();
					pToolTips->SetBkColor( D3DCOLOR_ARGB(128,0,0,0) );
					pToolTips->SetAlignType( GUI_ALIGN_LEFT );
					pToolTips->SetBorderColor( 0 );
					pToolTips->SetLineGaps( 3 );
					pToolTips->SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
					if( m_bOnlyGmMsg[index] )
						pToolTips->AddText( "单击左键隐藏");
					else
						pToolTips->AddText( "单击左键开启");
					pToolTips->Resize();
					ptCursor.x -= pToolTips->GetWidth();
					ptOldCursor.y -= pToolTips->GetHeight()+2;
				}
			}
		}
	}


	//Delete over-time-media-exchange-info
	UpdateMediaExchangeList();


	return CGuiDialog::OnRender( fElapsedTime);

	FUNCTION_END;
}

/**
* \brief 初始化特殊控件属性
* 
* 
* 
* \return 
*/
void CGuiChatOutput::InitCustomElements()
{
	FUNCTION_BEGIN;


	const char* szGrpFileName = "data\\interfaces.gl";
	stResourceLocation rl;
	rl.SetFileName(GetGuiGraphicPackName());
	rl.group = 0;
	rl.frame = 0;

	stRectI rcTexture;
	CGuiElement Element;

    rl.SetFileName( "data\\Interfaces1.gl" ); 
	//------------------------------------
	// Element for the 3D Border ScrollBar(滚动条)
	//------------------------------------
	{
		stRectI rects[9];
		rl.group = 41;
		rl.frame = 11;
		
		rl.group = 41; //图包
		stGuiCtrlBorderInfo * pInfo = GetCustomCtrlBorderInfo(GUI_CONTROL_SCROLLBAR);
		pInfo->rcTexture.SetRect(1,0,12,110);
		pInfo->ptLeftTop = stPointI(3,9);
		pInfo->ptRightBottom = stPointI(3,13);
		pInfo->ptBorderLeftTop = stPointI(-8,-4);
		pInfo->ptBorderRightBottom = stPointI(-8,-7);
		pInfo->CuttRect9(rects);
		m_nSBWidth = pInfo->rcTexture.Width();
		for(size_t i=0;i<count_of(rects);++i)
		{
			pInfo->element[i].SetTexture(&rl,&rects[i]);
			pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
			pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
		}
	}

	//-------------------------------------
	// CGuiScrollBar - Up Arrow
	//-------------------------------------
	rl.frame = 19;
	rcTexture.SetRect( -3, 0, 21, 21);
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 0, &Element );


	rl.frame = 21;
	rcTexture.SetRect( -3, 0, 21, 21);
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 1, &Element );


	//-------------------------------------
	// CGuiScrollBar - Down Arrow
	//-------------------------------------
	rl.frame = 22;
	rcTexture.SetRect( -3,0 ,21, 21 );
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 2, &Element );

	rl.frame = 24;
	rcTexture.SetRect( -3,0 ,21, 21 );
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 3, &Element );

	//-------------------------------------
	// CGuiScrollBar - Button
	//-------------------------------------
	rl.frame = 12;
	rcTexture.SetRect( 4, 0, 14, 28 );
	Element.SetTexture( &rl, &rcTexture );

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 4, &Element );



	FUNCTION_END;
}


void CGuiChatOutput::AddMessage(const char* pszString,DWORD dwColor)
{
	FUNCTION_BEGIN;

	//m_pEditOutput->AppendParagraph(pszString,dwColor);
	//if( m_bEnableScroll )
	//	m_pEditOutput->ScrollToTail();

	FUNCTION_END;
}

/*
return true 表示只在头顶显示
return false 表示在头顶和聊天输出显示
*/
bool CGuiChatOutput::AddNineChatMessage(const stChannelChatUserCmd* & pCmd)
{
	FUNCTION_BEGIN;

	
	if ( pCmd->size <= 0 )
	{
		DWORD dwColor = GetChatColor(pCmd->dwType);

		if(pCmd->dwType == CHAT_TYPE_NPC)
			//npc 说话
		{
			char szCharMsg[512];
			strncpy(szCharMsg,pCmd->pstrChat,sizeof(szCharMsg));
			szCharMsg[sizeof(szCharMsg)-1] = 0;
			CNpc *p = GetScene()->FindNpcByID(pCmd->dwFromID);
			if( p )
			{	
				p->AddNineChatMsg(szCharMsg,dwColor);
				return true;			
			}
		}
		if( pCmd->dwType == CHAT_TYPE_NINE)
			//玩家对话
		{
			char szCharMsg[512];
			strncpy(szCharMsg,pCmd->pstrChat,sizeof(szCharMsg));
			szCharMsg[sizeof(szCharMsg)-1] = 0;
			CCharacter *pChar = GetScene()->FindCharacterByID(pCmd->dwFromID);
			if(pChar) 
			{
				pChar->AddNineChatMsg(szCharMsg,dwColor);
				return false;
			}
		}
		if( pCmd->dwType == CHAT_TYPE_SHOPADV )
			//摆摊广告
		{
			char szCharMsg[512];
			strncpy(szCharMsg,pCmd->pstrChat,sizeof(szCharMsg));
			szCharMsg[sizeof(szCharMsg)-1] = 0;
			CCharacter *pChar = GetScene()->FindCharacterByName(pCmd->pstrName);
			if(pChar) 
			{
				pChar->AddShopAdvMsg(szCharMsg,dwColor);
				return true;
			}
		}
		if (pCmd->dwType == CHAT_TYPE_GM && pCmd->dwSysInfoType == INFO_TYPE_EXP)
		{ // GM公告
			GetGameGuiManager()->AddClientTaskMessage(((stChannelChatUserCmd*)pCmd)->pstrChat,COLOR_ARGB(255,255,250,0));
			return false;
		}
	}

	return false;
	

	FUNCTION_END;
}

void CGuiChatOutput::AppendParagraph(TiXmlElement * page,const stChannelChatUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	for ( int i = 0; i < 3; i++ )
	{
		if( IsCanputType( pCmd->dwType,i ) )
		{
			if( m_bOnlyGmMsg[i] && pCmd->dwType == CHAT_TYPE_GM)
				m_pEditOutput[i]->AppendParagraph(page,&g_GMFilter);
			else
				m_pEditOutput[i]->AppendParagraph(page,NULL);
		}

		if ( m_bEnableScroll[i] )
		{
			if ( m_bOnlyGmMsg[i] )
			{
				if ( pCmd->dwType == CHAT_TYPE_GM )
					m_pEditOutput[i]->ScrollToTail();	
			}
			else
			{
				m_pEditOutput[i]->ScrollToTail();
			}
		}
		else
		{
			if( m_bOnlyGmMsg[i] )
			{
				if( pCmd->dwType == CHAT_TYPE_GM )
					m_pEditOutput[i]->ScrollToTail();				
			}	
		}
	}

	FUNCTION_END;
}

void CGuiChatOutput::ResetChat()
{
	FUNCTION_BEGIN;

	for ( int i = 0; i < 3; i++ )
	{
		if ( m_pEditOutput[i] && 
			m_pEditOutput[i]->GetParagraphCount() > MAX_TEXT_LINE )
		{
			for ( int j = 0; j < 16; j++ )
			{
				m_pEditOutput[i]->RemoveFirstParagraph();
			}
			m_pEditOutput[i]->Reset();

			if( m_bEnableScroll[i] )
				m_pEditOutput[i]->ScrollToTail();
		}
	}
	FUNCTION_END;
}
/**
* \brief 接收服务器传来的聊天内容
* 
* 
* 
* \param pCmd : 协议
* \return 
*/
void CGuiChatOutput::AddChatMessage(stChannelChatUserCmd*  pCmd)
{
	FUNCTION_BEGIN;

	if(pCmd->dwType == CHAT_TYPE_WHISPER)
	{
		sprintf(m_szPrivateName,"/%s",pCmd->pstrName);
		CGuiPrivateChatSelect::AddWhisperChatUser(pCmd->pstrName);
	}

	if(pCmd->dwType == CHAT_TYPE_SYSTEM && pCmd->dwSysInfoType == INFO_TYPE_SCROLL)
	{ // gm不过滤
	}
	else 
		CChatManager::FilterChatMessage(pCmd);

	if (AddNineChatMessage(pCmd))  // show some msg on character's head 
		return;
	TiXmlElement page("p");
	CChatManager::SetChatTitle(pCmd,&page,GetChatColor(pCmd->dwType));
	CChatManager::FormatMsgContent2XML(pCmd,&page);
	AppendParagraph(&page,pCmd);
	ResetChat();

	FUNCTION_END;
}

void CGuiChatOutput::UpdateMediaExchangeList()
{
	FUNCTION_BEGIN;

	static DWORD lastTimeUpdate = xtimeGetTime();
	static const int aliveTime = 20*60;


	//We just do this one time per minute
	if ( xtimeGetTime()-lastTimeUpdate < 60*1000 )
		return;

	lastTimeUpdate = xtimeGetTime();

	mapItemExchange::iterator itor;
	for( itor=g_itemExchangeList.begin(); itor!=g_itemExchangeList.end(); itor++ )
	{
		if ( ((*itor).second.dwAddTime < (int)(xtimeGetTime()/1000 - aliveTime))
			&& (xtimeGetTime()/1000 > aliveTime) )
		{
			g_itemExchangeList.erase( itor );
			return;
		}
	}

	FUNCTION_END;
}

void CGuiChatOutput::AddChatUserCmd(stChannelChatUserCmd*  pCmd)
{
	if(GetGameGuiManager()->m_guiChatOutput)
		GetGameGuiManager()->m_guiChatOutput->AddChatMessage(pCmd);
}

void CGuiChatOutput::OnButtonClick(UINT nID)
{
	int index = 0;
	if ( m_pTab )
	{
		index = m_pTab->GetCurItem();
	}
	if( index < 0 || index >= 3 ) return ;

	switch( nID )
	{
	case 2:
		{
			switch(m_eState) 
			{
			case 0:
				SetSize(310,175);
				break;
			case 1:
				SetSize(310,175+85);
				break;
			case 2:
				SetSize(310,175+175);
				break;
			case 3:
				if( GetDevice()->GetHeight() > 768 )
				{
					SetSize(310,350+90);					
				}
				break;
			}
			m_eState++;
			if (m_eState>=4)
				m_eState = 0;
		}
		break;
	case 3:// 信息滚动开启/关闭
		{
			m_bEnableScroll[index] = !m_bEnableScroll[index];
		}
		break;
	case 4://聊天开启/关闭(只保留gm信息)
		{
			m_bOnlyGmMsg[index] = !m_bOnlyGmMsg[index];
			if(m_pEditOutput[index]) 
				m_pEditOutput[index]->SetFilter(m_bOnlyGmMsg[index]);
		}
		break;
	case 5:	//全部
		if(m_pTab)  m_pTab->SetCurItem(0);
		SET_BUTTON_CHECK(nID,true);
		break;
	case 6://自定义1
		if(m_pTab)  m_pTab->SetCurItem(1);
		SET_BUTTON_CHECK(nID,true);
		break;
	case 7://自定义2
		if(m_pTab)  m_pTab->SetCurItem(2);
		SET_BUTTON_CHECK(nID,true);
		break;
	}
}

bool CGuiChatOutput::IsCanputType(int type,int index)
{
	if( index == 0 ) return true;
	if( index == 1 )
	{
		tMapChatConfig::iterator it = m_map1.find( type );
		if( it != m_map1.end() )
		{
			return it->second;
		}
		return true;
	}
	if( index == 2 )
	{
		tMapChatConfig::iterator it = m_map2.find( type );
		if( it != m_map2.end() )
		{
			return it->second;
		}
		return true;
	}
	return false;
}

bool CGuiChatOutput::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg == WM_MOUSEMOVE )
	{
		if(	GetGuiManager()->GetMouseOverDlg() == this)
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			stPointI ptMouse(mousePoint.x-m_x,mousePoint.y-m_y);
			stRectI rc(20,0,228,20);
			if( rc.PtInRect(ptMouse) )
			{
				m_tBeginTime = xtimeGetTime();				
			}
		}
	}
	return CGuiDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}