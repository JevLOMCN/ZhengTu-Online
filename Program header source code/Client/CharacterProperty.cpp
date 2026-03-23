/**
 *\file		CharacterProperty.cpp
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:09:00 中国标准时间
 *\brief	人物状态绘制
 *
 * 
 *
*/

#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./CharacterProperty.h"
#include "./GuiPrivateChat.h"
#include "./GameState.h"
#include "./guiteam.h"
#include "./GameCursor.h"
#include "./GuiTrade.h"
#include "GuiWorldConn.h"
#include "./RoleItem.h"
#include "./GuiShopDialog.h"
#include "./GuiChatOutput.h"
#include "./Chat.h"

struct szCmdItem
{
	enum enumCMD
	{
		CMD_NAME,
		CMD_LINE,
		CMD_FOLLOW,
		CMD_CHAT,
		CMD_TRADE,
		CMD_GROUP_HONOR, // 荣誉模式组队
		CMD_GROUP_NORMAL, // 组队模式组队
 		CMD_LOOK,
//		CMD_FIGHT,
//		CMD_REQUESTSHOP,
		CMD_ADDFRIEND,   // 添加好友
		CMD_ADDSCHOOL,   // 添加师徒
		CMD_PRIVATECHAT, // 密聊
		CMD_FLOWERS,	//soke 匿名送花9朵
		CMD_FLOWERS1,	//soke 匿名送花99朵
		CMD_FLOWERS2,	//soke 匿名送花365朵
		CMD_FLOWERS3,	//soke 匿名送花999朵
		CMD_FLOWERS4,	//soke 签名送花9朵
		CMD_FLOWERS5,	//soke 签名送花99朵
		CMD_FLOWERS6,	//soke 签名送花365朵
		CMD_FLOWERS7,	//soke 签名送花999朵
		CMD_FLOWERS20,	//soke 匿名送花9朵
		CMD_FLOWERS21,	//soke 匿名送花99朵
		CMD_FLOWERS22,	//soke 匿名送花365朵
		CMD_FLOWERS23,	//soke 匿名送花999朵
		CMD_FLOWERS41,	//soke 签名送蓝色妖姬9朵
		CMD_FLOWERS51,	//soke 签名送蓝色妖姬99朵
		CMD_FLOWERS61,	//soke 签名送蓝色妖姬365朵
		CMD_FLOWERS71,	//soke 签名送蓝色妖姬999朵
		CMD_OPEN_PET_PACK,	//打开宠物包裹
#ifdef _OPEN_MINIGAME
		CMD_MINIGAME,//小游戏
#endif
		CMD_NUM
	};
	enumCMD		cmd;
	std::string     strCmd;
	bool            bCanUse;
};

static szCmdItem s_cmdItem[] = 
{
	{ szCmdItem::CMD_NAME,		"",            true },
	{ szCmdItem::CMD_LINE,		"",            false},
	{ szCmdItem::CMD_FOLLOW,	"跟随",        true },
	{ szCmdItem::CMD_CHAT,		"对话",      true },
	{ szCmdItem::CMD_TRADE,		"交易",        true },
	{ szCmdItem::CMD_GROUP_HONOR,	"荣誉组队",    true }, // 荣誉模式组队
	{ szCmdItem::CMD_GROUP_NORMAL,	"普通组队",    true }, // 组队模式组队
	{ szCmdItem::CMD_LOOK,		"观察",        false},
//	{ szCmdItem::CMD_FIGHT,		"决斗" ,       true },     //soke 取消显示
//	{ szCmdItem::CMD_REQUESTSHOP,	"查看摊位",    false}, //soke 取消显示
	{ szCmdItem::CMD_ADDFRIEND,	"添加好友",    true },// 添加好友
	{ szCmdItem::CMD_ADDSCHOOL,	"招收徒弟",    true},
	{ szCmdItem::CMD_PRIVATECHAT,	"密聊",    true},
//	{ szCmdItem::CMD_OPEN_PET_PACK,	"打开宠物包裹",   false},//soke 取消显示
	{ szCmdItem::CMD_FLOWERS,		"匿名送花9朵",    true},
	{ szCmdItem::CMD_FLOWERS1,		"匿名送花99朵",    true},
	{ szCmdItem::CMD_FLOWERS2,		"匿名送花365朵",    true},
	{ szCmdItem::CMD_FLOWERS3,		"匿名送花999朵",    true},
	{ szCmdItem::CMD_FLOWERS4,		"签名送花9朵",    true},
	{ szCmdItem::CMD_FLOWERS5,		"签名送花99朵",    true},
	{ szCmdItem::CMD_FLOWERS6,		"签名送花365朵",    true},
	{ szCmdItem::CMD_FLOWERS7,		"签名送花999朵",    true},
	{ szCmdItem::CMD_FLOWERS20,     "匿名送蓝色妖姬9朵", true },
	{ szCmdItem::CMD_FLOWERS21,     "匿名送蓝色妖姬99朵", true },
	{ szCmdItem::CMD_FLOWERS22,     "匿名送蓝色妖姬365朵", true },
	{ szCmdItem::CMD_FLOWERS23,     "匿名送蓝色妖姬999朵", true },
	{ szCmdItem::CMD_FLOWERS41,     "签名送蓝色妖姬9朵", true },
	{ szCmdItem::CMD_FLOWERS51,		"签名送蓝色妖姬99朵",    true},
	{ szCmdItem::CMD_FLOWERS61,		"签名送蓝色妖姬365朵",    true},
	{ szCmdItem::CMD_FLOWERS71,     "签名送蓝色妖姬999朵", true },

#ifdef _OPEN_MINIGAME
	{ szCmdItem::CMD_MINIGAME,	"小游戏",    true}
#endif 
};


bool CanMakeGroupWithMe(int iLevel,int iMode)
{
	FUNCTION_BEGIN;
	
	if(  TEAM_NORMAL != iMode &&  TEAM_HONOR != iMode )
	     return false;
        if( TEAM_NORMAL == iMode )
		return true;
	else if( TEAM_HONOR == iMode)
	{
		// 荣誉组队的判断条件：
		//	1、	等级必须达到30级才出现 荣誉组队 选项 
		//	2、	使用荣誉组队时，只能组比自己等级低的玩家。 
		//	3、	使用荣誉组队，不可组超过50级的人。
		if( GetScene()->GetMainCharacter()->GetLevel() > 30 
			&& iLevel < GetScene()->GetMainCharacter()->GetLevel() 
			&&  iLevel <= 50 )
			return true;
		return false; 
	} 

	return true;

	FUNCTION_END;
}


/**
 * \brief 构造函数
 * 
 * 
 * 
 * \return 
 */
CGuiCharacterProperty::CGuiCharacterProperty(void)
: m_pTeamHeadIcon(NULL)
, m_bShowDetail(false)
, m_activeCMD(-1)
, m_pDetail(NULL)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}



/**
 * \brief 关闭对话框
 * 
 * 关闭对话框时调用
 * 
 * \return 
 */
void CGuiCharacterProperty::OnClose(void)
{
	FUNCTION_BEGIN;
	
	if ( m_pDetail )
	{
		delete m_pDetail;
		m_pDetail = NULL;
	}

	if ( m_menu )
	{
		m_menu->Close();
		m_menu = NULL;
	}

	if( m_CartoonPetMenu )
	{
		m_CartoonPetMenu->Close();
		m_CartoonPetMenu = NULL;
	}

	FUNCTION_END;

}

/**
* \brief 创建对话框
* 
* 创建对话框时调用
* 
* \return 
*/
void CGuiCharacterProperty::OnCreate()
{
	FUNCTION_BEGIN;


	//Locate dlg
	SetLocation( 260, 0 );

	//Get the three bar
	IBitmapList* pBmpList = GetDevice()->FindResource(GetGuiGraphicPackName());

	//--------------------------------------------------------------------
	// Bar Info 
	m_aBar.resize(0);
	stBarShowInfo bar;
	m_aBar.reserve(2);
	bar.pBmp = pBmpList->LoadFrame(1,243); // red 
	bar.rcClip = bar.pBmp->GetClipRect();
	bar.type = BarTypeVert;
	m_aBar.push_back(bar);

	bar.pBmp = pBmpList->LoadFrame(1,244);  // blue
	bar.rcClip = bar.pBmp->GetClipRect();
	bar.type = BarTypeVert;
	m_aBar.push_back(bar);

	//bar.pBmp = pBmpList->LoadFrame(1,59);  // green,no used now 
	//bar.rcClip = bar.pBmp->GetClipRect();
	//bar.type = BarTypeHoriz;
	//m_aBar.push_back(bar);
	//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	//Add cmd menu
	m_menu = GetGuiManager()->CreateMenu( this );

	for ( int i = 0 ; i < count_of(s_cmdItem) ; i++ )
	{
		if ( s_cmdItem[i].cmd == szCmdItem::CMD_LINE)
			m_menu->AddMenuItem( -1, "", CGuiMenu::MenuStyle_Separate );
		else
			m_menu->AddMenuItem( i, s_cmdItem[i].strCmd.c_str(), CGuiMenu::MenuStyle_Command, s_cmdItem[i].bCanUse );
	}
	SetPopupMenu( m_menu );

	m_CartoonPetMenu = GetGuiManager()->CreateMenu( this );
	m_CartoonPetMenu->AddMenuItem(szCmdItem::CMD_OPEN_PET_PACK,"打开包裹",CGuiMenu::MenuStyle_Command,true);

	//--------------------------------------------------------------------

	//----------------------------------------------------------------------
	// team flag
	rlTeam.SetFileName(GetGuiGraphicPackName());
	rlTeam.group = 24;	 
	rlTeam.frame = 1;
	m_pTeamHeadIcon = GetDevice()->FindBitmaps(&rlTeam);     // 五角星
	//----------------------------------------------------------------------

	OnResetScreenSize();

	FUNCTION_END;
}

/**
 * \brief 初始化脚本
 * 
 * 
 * 
 * \return 
 */
void CGuiCharacterProperty::OnInitScript()
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}

/**
 * \brief 重设设备尺寸时调用
 * 
 * 
 * 
 * \return 
 */
void CGuiCharacterProperty::OnResetScreenSize()
{
	FUNCTION_BEGIN;
	
	m_ptIcon.x =  12 ;
	m_ptIcon.y =  12 ;
	
	if ( GetImage( 2 ) )
		GetImage( 2 )->SetLocation( 0, 0 );

	FUNCTION_END;
}

/**
 * \brief 处理控件事件
 * 
 * 
 * 
 * \param nEvent : 事件类别
 * \param nID : 控件id
 * \param pControl : 控件
 * \return 被处理则返回true, 否则返回false
 */
bool CGuiCharacterProperty::OnGuiEvent( UINT nEvent,UINT nID,CGuiControl* pControl )
{
	FUNCTION_BEGIN;


	if ( (NULL == GetScene()->m_pInterestNpc)
		|| (GetScene()->m_pInterestNpc->IsDie() ) )
	{
		return true;
	}
	
	switch( nEvent )
	{
	case EVENT_DIALOG_COMMAND:
		{
			Debug_Int(nID);
			switch( nID )
			{
			case szCmdItem::CMD_FOLLOW:
				{
					GetScene()->GetMainCharacter()->SetFollowState();
					return true;
				}
				break;
			case szCmdItem::CMD_CHAT:
				{
					GetChatManager().PrivateChatWith( GetScene()->m_pInterestNpc->GetName() );
					return true;
				}
				break;
			case szCmdItem::CMD_TRADE:
				{					
					AddTrade(GetScene()->m_pInterestNpc->GetName());
					return true;
				}
				break;
#ifdef _OPEN_MINIGAME
			case szCmdItem::CMD_MINIGAME:
				{	
					DWORD id = GetScene()->m_pInterestNpc->GetID();
					char temp[256] = "";
					sprintf(temp,"你正准备和 %s 玩比数字游戏,请输入游戏投注金额",
						GetScene()->m_pInterestNpc->GetName());
					GetGameGuiManager()->AddMoneyInputBox(temp,eAction_MiniGame_MoneyInput,(void*)id);
					//GetGameGuiManager()->AddMiniGameListDlg();
					return true;
				}
				break;
#endif
			case szCmdItem::CMD_GROUP_HONOR:
			case szCmdItem::CMD_GROUP_NORMAL:
				{
					//TBD
					GetGameGuiManager()->AddTeam();		
					if(GetGameGuiManager()->m_guiTeam)
					{
#ifdef __REMOTE__CAN__MAKE__TEAM__
						if( nID == szCmdItem::CMD_GROUP_HONOR)
							GetGameGuiManager()->m_guiTeam->ToAddTeamMember(GetScene()->m_pInterestNpc->GetName(),TEAM_HONOR);
						else if( nID == szCmdItem::CMD_GROUP_NORMAL)
							GetGameGuiManager()->m_guiTeam->ToAddTeamMember(GetScene()->m_pInterestNpc->GetName(),TEAM_NORMAL);			
#else
						if( nID == szCmdItem::CMD_GROUP_HONOR)
							GetGameGuiManager()->m_guiTeam->ToAddTeamMember(GetScene()->m_pInterestNpc->GetID(),TEAM_HONOR);
						else if( nID == szCmdItem::CMD_GROUP_NORMAL)
							GetGameGuiManager()->m_guiTeam->ToAddTeamMember(GetScene()->m_pInterestNpc->GetID(),TEAM_NORMAL);
#endif
					}
					return true;
				}
				break;
			case szCmdItem::CMD_FLOWERS: //soke 匿名送花9朵
				{
					//TBD
					stSelectFlowersPropertyUserCmd  cmd;
					cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
					cmd.byType = MAPDATATYPE_USER;

					SEND_USER_CMD(cmd);
					return true;
				}
				break;
			case szCmdItem::CMD_FLOWERS1: //soke 匿名送花99朵
				{
					//TBD
					stSelectFlowers1PropertyUserCmd  cmd;
					cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
					cmd.byType = MAPDATATYPE_USER;

					SEND_USER_CMD(cmd);
					return true;
				}
				break;
			case szCmdItem::CMD_FLOWERS2: //soke 匿名送花365朵
				{
					//TBD
					stSelectFlowers2PropertyUserCmd  cmd;
					cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
					cmd.byType = MAPDATATYPE_USER;

					SEND_USER_CMD(cmd);
					return true;
				}
				break;
			case szCmdItem::CMD_FLOWERS3: //soke 匿名送花999朵
				{
					//TBD
					stSelectFlowers3PropertyUserCmd  cmd;
					cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
					cmd.byType = MAPDATATYPE_USER;

					SEND_USER_CMD(cmd);
					return true;
				}
				break;
			case szCmdItem::CMD_FLOWERS4: //soke 签名送花9朵
				{
					//TBD
					stSelectFlowers4PropertyUserCmd  cmd;
					cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
					cmd.byType = MAPDATATYPE_USER;

					SEND_USER_CMD(cmd);
					return true;
				}
				break;
			case szCmdItem::CMD_FLOWERS5: //soke 签名送花99朵
				{
					//TBD
					stSelectFlowers5PropertyUserCmd  cmd;
					cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
					cmd.byType = MAPDATATYPE_USER;

					SEND_USER_CMD(cmd);
					return true;
				}
				break;
			case szCmdItem::CMD_FLOWERS6: //soke 签名送花365朵
				{
					//TBD
					stSelectFlowers6PropertyUserCmd  cmd;
					cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
					cmd.byType = MAPDATATYPE_USER;

					SEND_USER_CMD(cmd);
					return true;
				}
				break;
			case szCmdItem::CMD_FLOWERS7: //soke 签名送花999朵
				{
					//TBD
					stSelectFlowers7PropertyUserCmd  cmd;
					cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
					cmd.byType = MAPDATATYPE_USER;

					SEND_USER_CMD(cmd);
					return true;
				}
				break;
			case szCmdItem::CMD_FLOWERS20: //soke 匿名送蓝色妖姬9朵
			{
				//TBD
				stSelectFlowers20PropertyUserCmd  cmd;
				cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
				cmd.byType = MAPDATATYPE_USER;

				SEND_USER_CMD(cmd);
				return true;
			}
			break;
			case szCmdItem::CMD_FLOWERS21: //soke 匿名送蓝色妖姬99朵
			{
				//TBD
				stSelectFlowers21PropertyUserCmd  cmd;
				cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
				cmd.byType = MAPDATATYPE_USER;

				SEND_USER_CMD(cmd);
				return true;
			}
			break;
			case szCmdItem::CMD_FLOWERS22: //soke 匿名送蓝色妖姬365朵
			{
				//TBD
				stSelectFlowers22PropertyUserCmd  cmd;
				cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
				cmd.byType = MAPDATATYPE_USER;

				SEND_USER_CMD(cmd);
				return true;
			}
			break;
			case szCmdItem::CMD_FLOWERS23: //soke 匿名送蓝色妖姬999朵
			{
				//TBD
				stSelectFlowers23PropertyUserCmd  cmd;
				cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
				cmd.byType = MAPDATATYPE_USER;

				SEND_USER_CMD(cmd);
				return true;
			}
			break;
			case szCmdItem::CMD_FLOWERS41: //soke 签名送蓝色妖姬9朵
			{
				//TBD
				stSelectFlowers41PropertyUserCmd  cmd;
				cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
				cmd.byType = MAPDATATYPE_USER;

				SEND_USER_CMD(cmd);
				return true;
			}
			break;
			case szCmdItem::CMD_FLOWERS51: //soke 签名送蓝色妖姬99朵
			{
				//TBD
				stSelectFlowers51PropertyUserCmd  cmd;
				cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
				cmd.byType = MAPDATATYPE_USER;

				SEND_USER_CMD(cmd);
				return true;
			}
			break;
			case szCmdItem::CMD_FLOWERS61: //soke 签名送蓝色妖姬365朵
			{
				//TBD
				stSelectFlowers61PropertyUserCmd  cmd;
				cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
				cmd.byType = MAPDATATYPE_USER;

				SEND_USER_CMD(cmd);
				return true;
			}
			break;
			case szCmdItem::CMD_FLOWERS71: //soke 签名送蓝色妖姬999朵
			{
				//TBD
				stSelectFlowers71PropertyUserCmd  cmd;
				cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
				cmd.byType = MAPDATATYPE_USER;

				SEND_USER_CMD(cmd);
				return true;
			}
			break;
			case szCmdItem::CMD_LOOK:
				{
					//TBD
					stSelectMainUserPropertyUserCmd  cmd;
					cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
					cmd.byType = MAPDATATYPE_USER;

					SEND_USER_CMD(cmd);
					return true;
				}
				break;
			/*case szCmdItem::CMD_FIGHT:
				{
					//TBD
					return true;
				}
				break;*/
		/*	case szCmdItem::CMD_REQUESTSHOP:
				{
					DWORD dwID = GetScene()->m_pInterestNpc->GetID() ;
					CGuiShopDialog::OnOpenShop( dwID );
				}
				break;*/
			case szCmdItem::CMD_ADDFRIEND:  // 添加好友
				{
					if( GetScene()->m_pInterestNpc )
					{
						AddFriend(GetScene()->m_pInterestNpc->GetName());
					}
				}
				break;
			case szCmdItem::CMD_ADDSCHOOL:
				{
					if( GetScene()->m_pInterestNpc )
					{
					       AddPrentice( GetScene()->m_pInterestNpc->GetName());
					}
				}
				break;
			case szCmdItem::CMD_OPEN_PET_PACK:
				{
					GetGameGuiManager()->AddPetPackDlg();
				}
				break;
			case szCmdItem::CMD_PRIVATECHAT:
				{
					if( GetScene()->m_pInterestNpc )
					{
						InsertPrivateChat( (char *)GetScene()->m_pInterestNpc->GetName());
					}
				}
				break;

			}
		}		
		break;	
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

/**
 * \brief 对话框获取焦点
 * 
 * 详细描述
 * 
 * \param ptScreen : 鼠标位置
 * \return 鼠标在对话框内则返回true, 否则返回false
 */
BOOL CGuiCharacterProperty::ContainsPoint( POINT ptScreen )
{
	FUNCTION_BEGIN;
	
	if ( CGuiDialog::ContainsPoint(ptScreen) )
		return true;
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
 * \brief 保存选中NPC的名字
 * 
 * 
 * 
 * \param pName : 选中ＮＰＣ的名字
 * \return 
 */
void CGuiCharacterProperty::SetName( const char* pName, DWORD dwColor )
{
	char szName[MAX_NAMESIZE+10];
	sprintf( szName, "昵称：%s", pName );
	m_menu->SetMenuItem( 0, szCmdItem::CMD_NAME, szName );
	CGuiElement element;
	element.FontColor.Current = (D3DXCOLOR)dwColor;

	m_menu->SetMenuItemUseCustomeElement( 0, element );
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
bool CGuiCharacterProperty::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;
	
	//switch( uMsg )
	//{
	//case WM_RBUTTONDOWN:
	//	{
	//		//Turn on/off detail info
	//		if ( GetScene()->m_pInterestNpc )
	//		{
	//			if ( GetScene()->m_pInterestNpc->IsCharacter() )
	//			{
	//				stPointI pt = Engine_GetCursor()->GetPosition();
	//				stRectI rcPos;

	//				rcPos.left = m_x;
	//				rcPos.top = m_y;
	//				rcPos.right = rcPos.left + 50;
	//				rcPos.bottom = rcPos.top + 50;

	//				if ( rcPos.PtInRect( pt ) )
	//					m_bShowDetail = !m_bShowDetail;					
	//				//return true;
	//			}
	//		}
	//	}
	//	break;
	//case WM_LBUTTONDOWN:
	//	break;
	//default:
	//	break;
	//}
	return CGuiDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	FUNCTION_END;
}

/**
 * \brief 判断菜单是否激活（已不用）
 * 
 * 
 * 
 * \return 
 */
bool CGuiCharacterProperty::CanTreateCMD()
{
	/*if ( !m_bShowDetail )
		return false;

	if ( (m_activeCMD < 0) || (m_activeCMD >=CMD_NUM) )
		return false;

	return true;*/

	return false;
}


/**
 * \brief 初始化对话框图标
 * 
 * 
 * 
 * \return 
 */
void CGuiCharacterProperty::InitIcon()
{
	FUNCTION_BEGIN;

	if ( GetScene()->m_pInterestNpc )
	{
		if ( GetScene()->m_pInterestNpc->IsCharacter() )			//set character head icon
		{
			stResourceLocation rl;
			if ( GetScene()->m_pInterestNpc->IsChangeFace() )
			{
				rl.SetFileName( "data\\icons.gl" );
				rl.group = 4;
				rl.frame = GetNpcImageID( GetScene()->m_pInterestNpc->GetChangeFaceID() );
			}
			else
			{
				//soke 修改人物头像
				rl.SetFileName( "data\\interfaces.gl" );
				/*rl.group = 22;
				rl.frame = 20 + GetCharTypeInfo( static_cast<CCharacter*>(GetScene()->m_pInterestNpc)->GetType() ).equippic;*/
				rl.group = 33;
				rl.frame = ((CCharacter*)(GetScene()->m_pInterestNpc))->GetFace();
			}			

			SetIcon( &rl );
		}
		else
		{
			//TBD
			stResourceLocation rl;
			rl.SetFileName( "data\\icons.gl" );
			rl.group = 4;
			rl.frame = GetScene()->m_pInterestNpc->GetNpcPic();

			SetIcon( &rl );
		}		
	}	

	FUNCTION_END;
}

/**
 * \brief 选中玩家是否可以跟随
 * 
 * 
 * 
 * \return 是则返回ｔｒｕｅ，　否则返回ｆａｌｓｅ
 */
bool CGuiCharacterProperty::CanFollow()
{
	FUNCTION_BEGIN;

	//TBD
	/*int dis = Scene_GetDis( GetScene()->GetMainCharacter()->GetGridPos(), GetScene()->m_pInterestNpc->GetGridPos() );
	if(dis > 4)
		return false;*/

	if ( NULL == GetScene()->m_pInterestNpc )
		return false;

	if ( GetScene()->GetMainCharacter() && GetScene()->GetMainCharacter()->IsSitDown() )
		return false;

	if ( GetScene()->m_pInterestNpc->InScreen() )
		return true;

	return false;

	FUNCTION_END;
}

/**
 * \brief 选中ＮＰＣ是否可以私聊
 * 
 * 
 * 
 * \return 是则返回ｔｒｕｅ，　否则返回ｆａｌｓｅ
 */
bool CGuiCharacterProperty::CanChatPrivate()
{
	//TBD
	return true;
}

/**
 * \brief 选中玩家是否可以交易
 * 
 * 
 * 
 * \return 是则返回ｔｒｕｅ，　否则返回ｆａｌｓｅ
 */
bool CGuiCharacterProperty::CanExchange()
{
	FUNCTION_BEGIN;

	if ( GetGameState()->IsStateItem( eGameStateItem_Normal ) )
		return true;

	return false;

	FUNCTION_END;
}

bool bHasInTeam(CNpc * pNpc)
{
	if( !pNpc ) return false;
	if( !pNpc->IsCharacter() ) return false;
	CCharacter * pCharacter = (CCharacter *)pNpc;
	if( pCharacter->GetTeamState() != TEAD_STATE_NONE)
		return true;
	else 
		return false;
}
/**
 * \brief 选中玩家是否可以组队
 * 
 * 
 * 
 * \return 是则返回ｔｒｕｅ，　否则返回ｆａｌｓｅ
 */
bool CGuiCharacterProperty::CanMakeGroupNormal()
{
	//TBD
	FUNCTION_BEGIN;

	if(GetGameGuiManager()->m_guiTeam && GetGameGuiManager()->m_guiTeam->m_aTeamMember.size()>6
		|| !GetScene()->m_pInterestNpc
		|| !GetScene()->m_pInterestNpc->IsCharacter()
		//||  (((CCharacter *)GetScene()->m_pInterestNpc)->GetTeamState() != TEAD_STATE_NONE))
		|| bHasInTeam(GetScene()->m_pInterestNpc))
	{
	    m_menu->SetMenuItem(6,szCmdItem::CMD_GROUP_NORMAL,"普通组队",CGuiMenu::MenuStyle_Command,false);
		return false;
	}

	return true;

	FUNCTION_END;
}


bool CGuiCharacterProperty::CanMakeGroupHonor()
{
	//TBD
	FUNCTION_BEGIN;

	if(   GetGameGuiManager()->m_guiTeam && GetGameGuiManager()->m_guiTeam->m_aTeamMember.size() > 6 
					  ||  !GetScene()->m_pInterestNpc 
					  ||  !GetScene()->m_pInterestNpc->IsCharacter()
					//  ||  (((CCharacter *)GetScene()->m_pInterestNpc)->GetTeamState() != TEAD_STATE_NONE)
					|| bHasInTeam(GetScene()->m_pInterestNpc)
					  ||  !CanMakeGroupWithMe(GetScene()->m_pInterestNpc->GetLevel(),TEAM_HONOR))  
	{
		m_menu->SetMenuItem(5,szCmdItem::CMD_GROUP_HONOR,"荣誉组队",CGuiMenu::MenuStyle_Command,false);
		return false;
	}

	return true;

	FUNCTION_END;
}


/**
 * \brief 选中玩家的详细信息是否可以察看
 * 
 * 
 * 
 * \return 是则返回ｔｒｕｅ，　否则返回ｆａｌｓｅ
 */
bool CGuiCharacterProperty::CanLookAt()
{
	//TBD
	return true;
}

/**
 * \brief 选中玩家是否可以单挑
 * 
 * 
 * 
 * \return 是则返回ｔｒｕｅ，　否则返回ｆａｌｓｅ
 */
bool CGuiCharacterProperty::CanFight()
{
	//TBD
	return true; //修复右键头像加好友
}

bool CGuiCharacterProperty::CanRequestShop()
{
	if( GetScene()->m_pInterestNpc && GetScene()->m_pInterestNpc->IsCharacter())
	{
		return GetScene()->m_pInterestNpc->bIsShopStart();
	}
	return false;
}

/**
 * \brief 切换选中玩家时更新菜单（已不用）
 * 
 * 
 * 
 * \return 
 */
void CGuiCharacterProperty::InitDetail()
{
	FUNCTION_BEGIN;

	return;
	
	if ( NULL == GetScene()->m_pInterestNpc )
		return;

	if ( m_pDetail )
	{
		delete m_pDetail;
		m_pDetail = NULL;
	}
	
	//Only show character info
	if ( !GetScene()->m_pInterestNpc->IsCharacter() )
	{
		m_bShowDetail = false;
		return;
	}

	//Get cmd can use info
	/*cmdCanUse[2] = CanFollow();
	cmdCanUse[3] = CanChatPrivate();
	cmdCanUse[4] = CanExchange();
	cmdCanUse[5] = CanMakeGroup();
	cmdCanUse[6] = CanLookAt();
	cmdCanUse[7] = CanFight();*/

	m_pDetail = new CToolTips;
	m_pDetail->Clear();
	m_pDetail->SetBkColor( D3DCOLOR_ARGB(128,0,0,0) );
	m_pDetail->SetAlignType( GUI_ALIGN_HCENTER );
	m_pDetail->SetBorderColor( 0 );
	m_pDetail->SetLineGaps( 3 );
	m_pDetail->SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
	m_pDetail->AddText( GetScene()->m_pInterestNpc->GetName(), 0 );
	
	m_pDetail->SetCurColor( COLOR_ARGB(255,255,255,0) );
	for( int i=0; i < count_of(s_cmdItem); i++ )
	{
		if ( !s_cmdItem[i].bCanUse )
			m_pDetail->SetCurColor( D3DCOLOR_ARGB(255,128,128,128) );
		else
			m_pDetail->SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );

		m_pDetail->AddText( s_cmdItem[i].strCmd.c_str() );		
	}

	m_pDetail->Resize();

	FUNCTION_END;
}

/**
 * \brief 更新菜单（已不用）
 * 
 * 
 * 
 * \return 
 */
void CGuiCharacterProperty::UpdateDetail()
{
	FUNCTION_BEGIN;

	return;
	
	if ( !m_bShowDetail )
		return;

	//See which cmd is mouse on
	if ( NULL == m_pDetail )
		InitDetail();

	if ( NULL == m_pDetail )
		return;

	stPointI pt = Engine_GetCursor()->GetPosition();
	stRectI rcPos;
	for( int i=0;i<count_of(s_cmdItem) ; i++ )
	{
		rcPos.left = m_x + 185;
		rcPos.top = m_y + 25 + i*(m_pDetail->GetHeight()-15)/6;
		rcPos.right = rcPos.left + m_pDetail->GetWidth();
		rcPos.bottom = rcPos.top + (m_pDetail->GetHeight()-15)/6;

		if ( rcPos.PtInRect( pt ) )
			break;
	}
	
	if ( i < count_of(s_cmdItem) && s_cmdItem[i].bCanUse )
		m_activeCMD = i;
	else
		m_activeCMD = -1;

	//Update detail
	{
		m_pDetail->Clear();
		//m_pDetail->SetCurColor( static_cast<CCharacter*>(GetScene()->m_pInterestNpc)->GetNameColor() );
		m_pDetail->AddText( GetScene()->m_pInterestNpc->GetName(), 0 );
		
		for( int k=0; k < count_of(s_cmdItem) ; k++ )
		{
			if ( i == k )
			{
				if ( s_cmdItem[k].bCanUse )
					m_pDetail->SetCurColor( COLOR_ARGB(255,255,255,255) );
				else
					m_pDetail->SetCurColor( COLOR_ARGB(255,128,128,128) );
			}
			else
				m_pDetail->SetCurColor( COLOR_ARGB(255,255,255,0) );

			m_pDetail->AddText( s_cmdItem[k].strCmd.c_str() );
		}

		m_pDetail->Resize();
	}
	//
	FUNCTION_END;
}

void CGuiCharacterProperty::RefreshMenuStatus()
{
	FUNCTION_BEGIN;
	//Get cmd can use info
	s_cmdItem[0].bCanUse = true;
	s_cmdItem[1].bCanUse = false;
	s_cmdItem[2].bCanUse = CanFollow();
	s_cmdItem[3].bCanUse = CanChatPrivate();
	s_cmdItem[4].bCanUse = CanExchange();
	s_cmdItem[5].bCanUse = CanMakeGroupHonor();
        s_cmdItem[6].bCanUse = CanMakeGroupNormal();
	s_cmdItem[7].bCanUse = CanLookAt();
	s_cmdItem[8].bCanUse = CanFight();
	s_cmdItem[9].bCanUse = CanRequestShop();

	for( int i=0; i < count_of(s_cmdItem) ; i++ )
		m_menu->SetMenuEnabled( i, s_cmdItem[i].bCanUse );

	FUNCTION_END;
}

/**
 * \brief 绘制选中玩家的等级
 * 
 * 
 * 
 * \return 
 */
HRESULT CGuiCharacterProperty::RenderLevel()
{
	FUNCTION_BEGIN;

	if ( NULL == GetScene()->m_pInterestNpc )
		return S_OK;
		
//////////////////////////////////////////////////////////
	//soke 2015 7
	std::string s = GetScene()->m_pInterestNpc->GetName();
	stPointI ptx( 105 + m_x, 4 + m_y );
    DWORD color = COLOR_ARGB(255,0,255,0);//soke 名字显示绿色
	int iFont = GetDevice()->SetFont(1);  //soke 改变字体开始
	GetDevice()->DrawString( s.c_str(), ptx,color);
	GetDevice()->SetFont(iFont);          //soke 改变字体结束

//soke 先取消等级显示	
//	char c [ 1024 ] = "";
	
//	ptx.x = 3 + m_x;
//	ptx.y = 55 + m_y;
//	sprintf( c, "%d", GetScene()->m_pInterestNpc->GetLevel() );
//	ptx.x += ( 3 - strlen( c ) ) * 3;
//	GetDevice()->DrawString( c, pt );

//	s = GetScene()->m_pInterestNpc->GetCountryName();
//	ptx.x = 11 + m_x;
//	ptx.y = 7 + m_y;
//	GetDevice()->DrawString( s.substr( 0, 2 ).c_str(), ptx );
///////////////////////////////////////////////////////////

    int fiveType = FIVE_NONE;
	if ( GetScene()->m_pInterestNpc->IsCharacter() )
	{
		if ( GetScene()->m_pInterestNpc->IsChangeFace() )
			fiveType = GetScene()->m_pInterestNpc->GetChangeFaceFiveType();
		else
			return S_OK;
	}
	
	if ( !GetScene()->m_pInterestNpc->IsCharacter() )
		fiveType = GetScene()->m_pInterestNpc->GetMainFive();
	if ( (fiveType < FIVE_METAL) || (fiveType >= FIVE_NONE) )
		return S_OK;
	
	static char* _fiveName[] = { "金", "木", "土", "水", "火", "" };
	stPointI pt(81+m_x,7+m_y);
	GetDevice()->DrawString( _fiveName[fiveType], pt, fiveColor[fiveType] );

	return S_OK;

	FUNCTION_END;
}

 

/**
 * \brief 绘制选中NPC的生命、魔法、体力等
 * 
 * 
 * 
 * \param bar : 相应显示条
 * \param iCur : 当前值
 * \param iMax : 最大值
 * \param index : 种类
 * \return 
 */
//soke 修复NPC血条的显示
void CGuiCharacterProperty::RenderBar( CGuiCharacterProperty::stBarShowInfo& bar,uint64_t iCur,uint64_t iMax,int index )
{
	FUNCTION_BEGIN;

	if(!bar.pBmp) return;

	/*if ( g_bSinglePlayer )
	{
		iCur = 100;
		iMax = 200;
	}*/

	/*static const char* desc[] = { "生命值  当前生命值为0时人会死亡，在非战斗状态和吃药时生命值可恢复",
		"法术值  施用技能消耗法术值，在非战斗状态和吃药时法术值可恢复",
		"体力值  进行各种行动要消耗体力值，在非战斗状态和吃药时体力值可恢复",
		"经验值" };*/
	//sky 
	static const char* desc[] = { "生命值  ",
	"法术值  ",
	"体力值  ",
	"经验值" };

	//make sure max value is valid
	if ( iCur < 0 )
		iCur = 0;

	if ( iMax <= 0 || (iMax <iCur) )
	{
		if ( iCur > 0 )
			iMax = iCur;
		else
			iMax = 1;
	}	
	
	stRectI rc = bar.rcClip;
	stPointI pt(rc.left,rc.top);
	if( iMax > 0 )
	{
		if(bar.type == BarTypeVert)
		{
			rc.top = bar.rcClip.bottom - (long long)(bar.rcClip.Height() * iCur/iMax);
			pt.y = rc.top;
		}
		else
		{
			rc.right = bar.rcClip.left + bar.rcClip.Width() * iCur/iMax;
			pt.x = rc.left;
		}

		pt.x += m_x;
		pt.y += m_y;

		// alpha = 128 据说可以防止按键精灵
		bar.pBmp->Render( pt.x, pt.y, &rc, NULL,COLOR_ARGB(217,255,255,255));

	}	


    //sky 增加显示
	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	stRectI rctBound( bar.rcClip.left + m_x, bar.rcClip.top + m_y,
		bar.rcClip.right + m_x, bar.rcClip.bottom + m_y );
	bool bMouseOver = rctBound.PtInRect( ptCursor );

	if( !bMouseOver )
		return;

	char strValue[64];
	switch( index )
	{
	case 0:
		sprintf( strValue, "( %I64u / %I64u )", iCur, iMax );
		break;
	case 1:
		sprintf( strValue, "( %I64u / %I64u )", iCur, iMax );
		break;
	}

	
	if ( index == 1 && iMax == 0)
		return;

	m_AllTips.Clear();
	m_AllTips.SetBkColor( D3DCOLOR_ARGB(128,0,0,0) );
	m_AllTips.SetAlignType( GUI_ALIGN_LEFT );
	m_AllTips.SetBorderColor( 0 );
	m_AllTips.SetLineGaps( 3 );
	m_AllTips.SetCurColor( D3DCOLOR_ARGB(255,152,244,23) );
	m_AllTips.AddText( desc[index] );
	m_AllTips.SetCurColor( D3DCOLOR_ARGB(255,255,0,0) );
	m_AllTips.AddText( strValue );
	m_AllTips.Resize();

	stPointI ptTips;
	ptTips.x = m_x + bar.rcClip.right + 5;
	ptTips.y = m_y + bar.rcClip.top - 5;

	if ( NULL == GetGuiManager()->GetToolTips() )
		GetGuiManager()->SetToolTips( &m_AllTips, ptTips );

	FUNCTION_END;
}

/**
 * \brief 绘制菜单（已不用）
 * 
 * 
 * 
 * \return 
 */
HRESULT CGuiCharacterProperty::RenderDetail()
{
	FUNCTION_BEGIN;

	return S_OK;
	
	UpdateDetail();
	//Render
	stPointI	pt;
	pt.x = m_x + 185;
	pt.y = m_y + 10;

	if ( m_pDetail )
		m_pDetail->Render( pt.x, pt.y );

	return S_OK;

	FUNCTION_END;
}

HRESULT CGuiCharacterProperty::RenderTeamFlag()
{
	FUNCTION_BEGIN;
	if ( NULL == GetScene()->m_pInterestNpc )
		return S_OK;

	if ( !GetScene()->m_pInterestNpc->IsCharacter() )
		return S_OK;

	CCharacter * pInterset = (CCharacter *)GetScene()->m_pInterestNpc;
	if( m_pTeamHeadIcon && pInterset->GetTeamState() == TEAM_STATE_LEADER )
	{
		m_pTeamHeadIcon->RenderAni(m_x,m_y+20,xtimeGetTime(),-1,Blend_Null);
	}

	return S_OK;

	FUNCTION_END;
}


HRESULT CGuiCharacterProperty::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;
	
	if ( (NULL == GetScene()->m_pInterestNpc) || ( GetScene()->m_pInterestNpc->IsDie() ) || ( GetScene()->m_pInterestNpc->CanRemove() ) )
	{
		SetVisible( false );
		if ( m_menu )
			m_menu->SetVisible( false );
		if( m_CartoonPetMenu )
		{
			m_CartoonPetMenu->SetVisible(false);
		}

		GetScene()->UnSelectInterestNpc();

		return S_OK;
	}

	//Render dialog
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	
	//Draw level num
	//In fact we draw five type
        hr |= RenderLevel();

	hr |= RenderTeamFlag();

	//Render the three
	if ( GetScene()->m_pInterestNpc )
	{
		
		unsigned int nCurHp = GetScene()->m_pInterestNpc->GetHP();
		unsigned int nMaxHp = GetScene()->m_pInterestNpc->GetMaxHP();
		// 注释: 以前有人在CSceneObject::CSceneObject()中, 
		//  m_dwHP(0)
		// , m_dwMaxHP(1)
		// , m_dwMP(0)
		// , m_dwMaxMP(0)
		// 我不知道为什么这样m_dwMaxHP(1)，而m_dwMaxMP(0)，可能是为了避免除０，但不这样也可以很轻松的避免阿
		// 反正下面这条语句也用１，所以如果CSceneObject改了(不用１)，这里要重新改
		if( nMaxHp <= 1 && GetScene()->m_pInterestNpc->IsNpc() && !GetScene()->m_pInterestNpc->IsDie())
		{
			// 没有死的话，认定这一帧之时还没有收到服务器返回信息，就假定让它满血吧，
			// 这样是避免血条闪烁
                        nCurHp = nMaxHp = 1; 
		}
		RenderBar(m_aBar[0],nCurHp,nMaxHp,0);
		RenderBar(m_aBar[1],GetScene()->m_pInterestNpc->GetMP(),GetScene()->m_pInterestNpc->GetMaxMP(),1);
		//RenderBar(m_aBar[2],GetScene()->m_pInterestNpc->GetSP(),GetScene()->m_pInterestNpc->GetMaxSP(),2);
	}


	//Refresh menu cmd enable state
	RefreshMenuStatus();

	//if (  m_bShowDetail )
		//RenderDetail();

	return hr;

	FUNCTION_END;
}

/**
 * \brief 调整菜单位置
 * 
 * 
 * 
 * \return 成功则返回true, 否则返回false
 */
bool CGuiCharacterProperty::OnContextMenu()
{
	FUNCTION_BEGIN;

	stRectI rc;
	rc.left = m_x + 0;
	rc.right = rc.left + 80;
	rc.top = m_y + 0;
	rc.bottom =rc.top + 80;

	if( rc.PtInRect( Engine_GetCursor()->GetPosition() ) )
	{
		CNpc* pNpc = GetScene()->m_pInterestNpc;
		if ( pNpc->IsNpc() )
		{
			if( pNpc->IsMyCartoonPet() && m_CartoonPetMenu )
			{
				SetPopupMenu( m_CartoonPetMenu );
				m_CartoonPetMenu->Popup();
			}
			return false;
		}
		if ( pNpc->IsCharacter()
			&& pNpc->IsChangeFace() )
		{
			return false;
		}

		if( m_menu )
		{
			RefreshMenuStatus();
			SetPopupMenu( m_menu );
			m_menu->Popup();
			return true;
		}
	}
	return false;

	FUNCTION_END;
}

void CGuiCharacterProperty::CopyNameToClipboard()
{
	if ( NULL == GetScene()->m_pInterestNpc )
		return;

	if( OpenClipboard( NULL ) )
	{
		EmptyClipboard();

		int nLength = strlen( GetScene()->m_pInterestNpc->GetName() ) + 1;

		HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, sizeof(char) * nLength );
		if( hBlock )
		{
			char *pszText  = (char*)GlobalLock( hBlock );
			if( pszText )
			{
				pszText[0] = 0L;
				sprintf( pszText, GetScene()->m_pInterestNpc->GetName() );
				GlobalUnlock( hBlock );
			}

			SetClipboardData( CF_TEXT, hBlock );
		}
		CloseClipboard();
		// We must not free the object until CloseClipboard is called.
		if( hBlock )
			GlobalFree( hBlock );
	}
}
