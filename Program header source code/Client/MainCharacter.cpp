#include "public.h"
#include "Game.h"
#include ".\maincharacter.h"
#include ".\GameScene.h"
#include "./GameCursor.h"
#include "./RoleItem.h"
#include "./GameGuiManager.h"
#include "./GuiItem.h"
#include "./Item.h"
#include "./GameState.h"
#include "./GameAccelKeyManager.h"
#include ".\roleskill.h"
#include "./GuiSkill.h"
#include "./GuiMain.h"
#include "./GuiFloatAccessKey.h"
#include "./GuiOtherAccessKey.h"
#include "./GuiUserInfo.h"
#include "./GuiTeam.h"
#include "./GuiPrivateChat.h"
#include "./CharacterProperty.h"
#include "./WeatherMgr.h"
#include "./GameTime.h"
#include "GuiPetDialog.h"
#include "./GuiEmotionDialog.h"
#include ".\gameappation.h"
#include ".\GuiSaveBox.h"
#include "./GuiPetCtrlDialog.h"
#include "./GuiChatSelect.h"
#include "./GuiChatSelectButton.h"
#include "../Magic/iMagic.h"
#include "../Magic/FootprintManager.h"
#include "./GuiShopDialog.h"
#include "./GuiSystem.h"
#include "./GuiOthersShopDialog.h"
#include "./Country.h"
#include "./GuiClearPointDlg.h"
#include "./GuiReliveDlg.h"
#include "./GuiNpcTrade.h"
#include "./GuiSalaryTrade.h"  //工资
#include "./GuiTicketTrade.h"  //积分
#include "./GuiChongzhiTrade.h" //充值点
#include "./AutoGame.h"
#include "./KillProcess.h"
#include "./FindDLL.h"
#include "./GuiChatOutput.h"
#include "./GuiNpcDialog.h"
#include "./GuiAutoAttackDlg.h" //////////star100515
#include "./DlgProgressBar.h"
#include "./Dlg2ProgressBar.h"
#include "./TimerDisplayMgr.h"
#include ".\guipackage.h"
#include "./DlgEquiphandbook.h" //soke 百兽图鉴
const int c_nMaxFindPathDisFar = 60;
const int c_nMaxFindPathDisNear = 60;
const int c_nMaxFindPathDisGM = 1000;
#define NEW_FACE_PROTECT_LEVEL		21
#define START_PK_LEVEL				20

#include "GuiMiniMap.h"
#include "./GuiNpcMake.h"

#define RETURN(Val) { retval = (Val); goto _ret;}

extern std::string g_sFrom;
extern bool g_bAutoGo;

namespace
{
	const int NPC_SPA_ID = 5215; //温泉NPC
	bool IsSpaNpc(DWORD dwID) // 是否为泡温泉功能相关npc
	{
		return NPC_SPA_ID == dwID ? true : false;
	}
}

// 得到CPU时钟周期
inline   unsigned   __int64   GetCycleCount()   
{
	//__asm   _emit   0x0F
	//__asm   _emit   0x31
	return GetTickCount();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param size : 描述
 * \return 返回值的描述
 */
static void* __cdecl s_fn_malloc( size_t size )
{
	if(FrameAllocator::smWaterMark + size <= FrameAllocator::smHighWaterMark)
		return FrameAllocator::allocNoAlign(size);
	return malloc(size);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param memblock : 描述
 * \return 返回值的描述
 */
static void __cdecl s_fn_free( void *memblock )
{
	if(memblock >= FrameAllocator::smBuffer + FrameAllocator::smHighWaterMark)
		free(memblock);
}

static stRectI s_rcFindPathRect;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x : 描述
 * \param y : 描述
 * \return 返回值的描述
 */
static int __stdcall  s_fn_FreeTile(int x,int y)
{
	if(s_rcFindPathRect.PtInRect(stPointI(x,y)))
		return !GetScene()->IsMoveObstacle(stPointI(x,y));
	return false;
}

const char* c_szFiveName[] = {
	"金","木","土","水","火","",""
};

const char* c_szFiveToOtherName[] = {
	"麒麟","青龙","玄武","白虎","朱雀","",""
};

enumFiveType g_aMainFive[FIVETYPENUM][5] = 
{
	{FiveType_Gold,		FiveType_Water,		FiveType_Earth, FiveType_Business, FiveType_Work},
	{FiveType_Timber,	FiveType_Fire,		FiveType_Water, FiveType_Business, FiveType_Work},
	{FiveType_Earth,	FiveType_Gold,		FiveType_Fire,  FiveType_Business, FiveType_Work},
	{FiveType_Water,	FiveType_Timber,	FiveType_Gold,  FiveType_Business, FiveType_Work},
	{FiveType_Fire,		FiveType_Earth,		FiveType_Timber,FiveType_Business, FiveType_Work},
};



char* GetPkModeName(DWORD dwGoodNess) {

	FUNCTION_BEGIN;

	DWORD dw=0x0000FFFF;

	short goodness=(short)(dwGoodNess & dw);

	switch(goodness) {
	case GOODNESS_2_1:
	case GOODNESS_2_2:
		return "普通";
		break;
	default:
		{
			if (goodness<(short)(GOODNESS_1 & 0x0000FFFF))
				return "英雄";
			else if (goodness<(short)(GOODNESS_2_1 & dw))
				return "侠士";
			else if (goodness<=(short)(GOODNESS_3 & dw))
				return "歹徒";
			else if (goodness<=(short)(GOODNESS_4 & dw))
				return "恶徒";
			else if (goodness<=(short)(GOODNESS_5 & dw))
				return "恶魔";
			else if (goodness==(short)(GOODNESS_7 & dw))
				return "叛国";
			else
				return "魔头";
		}
		break;
	}
	
	return "普通";

	FUNCTION_END;
}
// ====================================================================
// CMainCharacter
// ====================================================================
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CMainCharacter::CMainCharacter(void)
: m_dwScreenID(-1)
, m_dwSendMoveTime(0)
, m_pActiveSkill(NULL)
, m_pActiveNpc(NULL)
, m_pAttackNpc(NULL)
, m_dwNpcIdByLast(0)
, m_moveSpeed(1)
, m_bNoRunState(false)
, m_bDieDlgWaiting(false)
, m_nDieDlgWaitingTime(0)
, m_skillStartTime(0)
, m_curSkillTreePoints(0)
, m_bFreeMoveState(false)
, m_bLocalSpaceLButtonDown(false)
, m_PkMode(PKMODE_NUM)
, m_pGotoFinalEvent(NULL)
{
	FUNCTION_BEGIN;

	m_dwSceneObjectType |= OBJECT_MAINCHARACTER;
	m_RunEvent.target = eTargetType_None;
	m_RunEvent.event = eEventType_None;
	m_RunEvent.pt = stPointI(0,0);
	m_RunEvent.pSkill = NULL;
	m_InputState.dwLButtonDownTime = 0;
	m_InputState.dwRButtonDownTime = 0;
	m_InputState.bLButtonDown = false;
	m_InputState.bRButtonDown = false;
	m_InputState.ptMouse = stPointI(0,0);
	m_InputState.bGM = false;
	m_InputState.bReFindPath = false;
	m_InputState.dwLButtonDownTimeFirst = 0;

	m_bHasHorse = false;
	m_dwID = -1;

	m_preferAccessGroup = 0;
	m_bRushOut = false;

	m_AStar.SetMallocFn(s_fn_malloc,s_fn_free);
	m_AStarZone.SetMallocFn(s_fn_malloc,s_fn_free);
	m_AStar.SetFreeTile(s_fn_FreeTile);
	//m_AStar.SetMaxFindDis(c_nMaxFindPathDis);

	m_needRepairItemList.clear();

	m_pCharacter->SetMainCharacter();	

	if ( g_tableItemSetBase.empty() )
		m_bSetActive = NULL;
	else
		m_bSetActive = new stSetObjectPropertyActive[g_tableItemSetBase.size()];

	SetMoveSpeed( 2 );

	m_strMainRoleShopAdv = "";

	m_dwPoint = 0;
	m_dwMonthCard = 0;

	//自动打怪时间 随机传送
	autotime = 0;

	m_listItem.clear();
	m_dwDeathType = 0;

	m_infoEmperorDareZone.bNeedDraw = false;
	m_infoEmperorDareZone.dwDefID = -1;
	//ZH
	bNewItem=false;
	//////////////////////////////////////////////////////////////////////////

	//脚本特效
	m_image = false;
	m_pt.x = 0;
	m_pt.y = 0;
	m_startTime = 0;
	
	M_FUNCTION_ARRAY_INIT;		//初始化指针数组

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CMainCharacter::~CMainCharacter(void)
{
	FUNCTION_BEGIN;

	if ( m_bSetActive )
	{
		delete [] m_bSetActive;
		m_bSetActive = NULL;
	}
	Release();

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
bool CMainCharacter::HasItemNeedRepair()
{
	if ( m_needRepairItemList.empty() )
		return false;

	return true;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CMainCharacter::DeleteOneNeedRepairItem( DWORD thisID )
{
	if ( !HasItemNeedRepair() )
		return;

	std::vector<DWORD>::iterator it;
	for( it=m_needRepairItemList.begin();it!=m_needRepairItemList.end();it++ )
	{
		if ( (*it) == thisID )
			break;
	}

	if ( it != m_needRepairItemList.end() ) 
		SimpleRemoveFromArray( m_needRepairItemList, it - m_needRepairItemList.begin() );
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CMainCharacter::AddOneNeedRepairItem( DWORD thisID )
{
	std::vector<DWORD>::iterator it;
	for( it=m_needRepairItemList.begin();it!=m_needRepairItemList.end();it++ )
	{
		if ( (*it) == thisID )
			return;
	}

	m_needRepairItemList.push_back( thisID );
}


CRoleItem* CMainCharacter::GetCanUseArrow(const int& num)
{
	CRoleItem* pItem = NULL;
	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
	{
		if( ((*it)->GetItemType() == ItemType_Arrow)
			&& ((*it)->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON)
			&& ((*it)->GetObject()->dur >= num) )
		{
			pItem = (*it);
			break;
		}
	}
	return pItem;
}
/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CMainCharacter::DrawNeedRepairItemList(stPointI& pt)
{
	if ( HasItemNeedRepair() )
	{
		std::vector<DWORD>::iterator it;
		//First we update list
		for( it=m_needRepairItemList.begin();it!=m_needRepairItemList.end();it++ )
		{
			CRoleItem* pItem = FindItemByID( *it );
			if ( pItem )		//&& pItem->IsArrow()
			{
				if ( pItem->GetLocation().dwLocation != OBJECTCELLTYPE_EQUIP )
				{
					DeleteOneNeedRepairItem( (*it) );
					break;
				}
			}
		}
		//Draw	
		for( it=m_needRepairItemList.begin();it!=m_needRepairItemList.end();it++ )
		{
			CRoleItem* pItem = FindItemByID( *it );
			if ( pItem &&
				pItem->GetObjectBase()->dwType != ItemType_Tonic &&
				pItem->GetObjectBase()->dwType != ItemType_DoubleExp &&
				pItem->GetObjectBase()->dwType != ItemType_Honor &&
				pItem->GetObjectBase()->dwType != ItemType_Move &&
				pItem->GetObjectBase()->dwType != ItemType_Scroll &&
				pItem->GetObjectBase()->dwType != ItemType_Scrxll && 
				pItem->GetObjectBase()->dwType != ItemType_Amulets &&
				pItem->GetObjectBase()->dwType != ItemType_Hanbing &&
				pItem->GetObjectBase()->dwType != ItemType_Xunzhang &&
				pItem->GetObjectBase()->dwType != ItemType_Chibang &&
				pItem->GetObjectBase()->dwType != ItemType_ShangfangBaojian && //取消耐久显示
				pItem->GetObjectBase()->dwType != ItemType_ZijinLongpao && //取消耐久显示				
				pItem->GetObjectBase()->dwType != ItemType_Shenbing0 &&
				(pItem->GetObjectBase()->dwType <220 && pItem->GetObjectBase()->dwType) &&
				pItem->GetObjectBase()->dwType != ItemType_Shenbing1 &&
				pItem->GetObjectBase()->dwType != ItemType_Shenbing2 &&
				pItem->GetObjectBase()->dwType != ItemType_Shenshou1 &&
				pItem->GetObjectBase()->dwType != ItemType_Shenshou2) 
			{
				if ( pItem->IsArrow() )
				{
					char temp[100];
					sprintf( temp, "当前箭支 %u 支", pItem->GetExteriorCount() );
					GetDevice()->DrawString( temp, pt, 0xffffff00 );
					pt.y -= GetDevice()->GetFontHeight();
				}
				else
				{
					char temp[100];
					sprintf( temp, "%s 耐久值 %u / %u", pItem->GetObjectName(), (pItem->GetObject()->dur+49)/50, (pItem->GetObject()->maxdur+49)/50 );
					GetDevice()->DrawString( temp, pt, 0xffffff00 );
					pt.y -= GetDevice()->GetFontHeight();
				}			
			}
		}
	}
	
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMainCharacter::UpdateAllItems( DWORD flag )
{
	FUNCTION_BEGIN;

	for( tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it )
	{
		(*it)->OnUpdate(flag);
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pCmd : 描述
 * \return 返回值的描述
 */
bool CMainCharacter::SetAttrib(t_MainUserData* pCmd)
{
	FUNCTION_BEGIN;

	bool bNeedChangePkMode = (m_data.pkmode != pCmd->pkmode);
	bNeedChangePkMode = false;		//Server is wrong
	bool bNeedRefreshSkillState = (m_data.skillPoint != pCmd->skillPoint);

	m_data = *pCmd;
	Debug_Int(m_data.bitmask);
	m_dwID = pCmd->dwUserTempID;
	//m_dwHP = m_data.hp;
	m_dwMaxHP = m_data.maxhp;

	GetGameGuiManager()->m_guiItem->SetCharType( GetType() );

	if(GetGameGuiManager()->m_guiSkill)
	{
		GetGameGuiManager()->m_guiSkill->SetPoints(GetSkillPoint());
		if ( bNeedRefreshSkillState )
			GetGameGuiManager()->m_guiSkill->RefreshState();
	}

	if(GetGameGuiManager()->m_guiUserInfo)
		GetGameGuiManager()->m_guiUserInfo->RefreshButtonState();

	if ( bNeedChangePkMode )
	{
		if ( GetGameGuiManager()->m_guiMain )
			GetGameGuiManager()->m_guiMain->ChangeAttackModeBtnTips();
	}
	
	for( tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it )
	{
		(*it)->OnDataUpdate();
	}

	return true;

	FUNCTION_END;
}

/**
 * \brief 设置首选快捷键组
 * 
 * 
 * 
 * \param group : 组
 * \return 
 */
void CMainCharacter::SetPreferAccessGroup( BYTE group )
{
	FUNCTION_BEGIN;

	m_preferAccessGroup = group;
	if ( (m_preferAccessGroup<0 ) || (m_preferAccessGroup>=MAIN_ACCESS_KEY_GROUP) )
		m_preferAccessGroup = 0;	

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMainCharacter::UnPauseMove()
{
	FUNCTION_BEGIN;

	m_pCharacter->PauseMove(false);
	m_bSendMove = false;

	FUNCTION_END;
}

/**
 * \brief 是否拥有马匹
 * 
 * 
 * 
 * \return 有则返回true, 否则返回false
 */
bool CMainCharacter::IsHasHorse()
{
	FUNCTION_BEGIN;

	return m_bHasHorse;

	FUNCTION_END;
}

/**
 * \brief 刷新马匹拥有状态
 * 
 * 
 * 
 * \return 
 */
void CMainCharacter::ResetHorseOwnStatus()
{
	FUNCTION_BEGIN;

	m_bHasHorse = ( GetProData()->sculpt.dwHorseID > 0 );

	if( !m_bHasHorse )
	{
		//如果没有马,则看看马匹是否在跟随，如果跟随则也认为他有马
		CPet* pHorse = GetScene()->FindPetByType(PET_TYPE_RIDE);

		if ( pHorse && false == pHorse->bIsDelete() )
		{
			m_bHasHorse = true;
		}
	}

	if(GetGameGuiManager()->m_guiPetDlg)
		GetGameGuiManager()->m_guiPetDlg->UpdateContents();

	FUNCTION_END;
}


CRoleItem* CMainCharacter::GetSavingBoxItem()
{
	FUNCTION_BEGIN;

	return FindItemByConstID( 695 );

	FUNCTION_END;
}

void CMainCharacter::ResetSavingBoxOwnNum()
{
	if(GetGameGuiManager()->m_guiSavingBoxDlg)
		GetGameGuiManager()->m_guiSavingBoxDlg->ResetContents();
}

/**
 * \brief 买马
 * 
 * 
 * 
 * \return 
 */
void CMainCharacter::SellHorse()
{
	FUNCTION_BEGIN;

	if ( m_bHasHorse )
	{		
		stSellHorseNpcTradeUserCmd cmd;
		cmd.action = 0;
		SEND_USER_CMD(cmd);
	}

	FUNCTION_END;
}

/**
* \brief  使用洗点道具
*         
* \return 
*/
void CMainCharacter::ClearPointOrSkill(DWORD dwID)
{
	 MSG_ClearPointOrSkill(dwID);
}
/**
* \brief 弃马
* 
* 发送丢弃马命令到服务器
* 
* \return 
*/
void CMainCharacter::RemoveHorse()
{
	FUNCTION_BEGIN;

	if ( m_bHasHorse )
	{		
		//骑马移动过程中不能弃马
		if(IsMoving() && IsRide()) return ;
		stSellHorseNpcTradeUserCmd cmd;
		cmd.action = 1;
		SEND_USER_CMD(cmd);
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMainCharacter::Release()
{
	FUNCTION_BEGIN;

	memset(&m_data,0,sizeof(m_data));
	m_dwScreenID = -1;
	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++it)
	{
		(*it)->OnRemove();
		m_allocRoleItem.freePointer(*it);
	}
	m_listItem.resize(0);
	m_needRepairItemList.resize(0);

	for(tListSkill::iterator it = m_listSkill.begin(); it != m_listSkill.end(); ++it)
	{
		delete (*it);
	}
	m_listSkill.resize(0);
	
	m_pActiveSkill = NULL;
	CCharacter::Release();

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMainCharacter::UpdateInputState()
{
	FUNCTION_BEGIN;

	m_InputState.ptMouse = Engine_GetCursor()->GetPosition();
	m_InputState.bLButtonDown = Engine_GetCursor()->IsLButtonDown();
	m_InputState.bRButtonDown = Engine_GetCursor()->IsRButtonDown();

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMainCharacter::RefreshEvent()
{
	FUNCTION_BEGIN;

	UpdateInputState();
	switch(m_RunEvent.event)
	{
	case eEventType_Move:     //soke 对移动的指令再次判断
		if ( !(m_InputState.bGM) )
		{
			if(m_InputState.bLButtonDown || (GetGameCursor()->GetCursorImageType() == eCursorFreeMove) )
			{
				m_RunEvent.pt = GetScene()->ScreenPixelPosToPixelPos(m_InputState.ptMouse);
			}
			else
			{
				m_RunEvent.event = eEventType_None;

				if(!GetGameApplication()->GetConfig()->bSystemSetted(USER_SETTING_AUTOFINDPATH))
				{
					ClearPath();
				}
				else
				{
					DWORD timeD = getMin(xtimeGetTime() - m_InputState.dwLButtonDownTime,xtimeGetTime() - m_InputState.dwRButtonDownTime);
					if(timeD > 1000)
					{
						ClearPath();
					}
				}
			}
		}		
		break;
	case eEventType_Attack:
		break;
	case eEventType_PickUp:
		break;
	}

	if ( m_RunEvent.event != eEventType_Attack )
	{
		m_pActiveNpc = NULL;
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMainCharacter::RunMoveEvent()
{
	FUNCTION_BEGIN;

	//on some state can not move
	if ( isset_state( GetState(), USTATE_ROCK )
		|| isset_state( GetState(), USTATE_LOSTSPRITE )
		|| isset_state( GetState(), USTATE_ICE_DEFENCE )
		|| isset_state( GetState(), USTATE_HIDE_NOMOVE_STATE)
		|| isset_state( GetState(), USTATE_USER_WABAO)
		|| isset_state( GetState(), USTATE_USER_DANCE)) //soke 跳舞、挖宝状态不能移动
	{
		goto _ret;
	}

	if ( isset_state( GetState(), USTATE_NO_MOVE )
		|| isset_state( GetState(), USTATE_CONFUSION )
		|| isset_state( GetState(), USTATE_CRAZY_ATTACK ) )
	{
		ClearRunEvent();
		goto _ret;
	}

	if(GetGridPos() == GetScene()->PixelPosToGridPos(m_RunEvent.pt))
	{
		ClearRunEvent();
	}
	else
	{
		if ( IsCombinationState() )
		{
			m_moveSpeed = 1;
			m_RunEvent.speed = 1;
		}
		WalkTo(m_RunEvent.pt,m_RunEvent.speed);

		if(m_InputState.bGM){
			if(m_AStar.IsHavePath() == false){
				if(Scene_GetDis(GetGridPos(),GetScene()->PixelPosToGridPos(m_RunEvent.pt)) < 8){
					ClearRunEvent();
				}
			}
		}
	}
_ret:
	;
	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  访问NPC
*/
CMainCharacter::enumAttackNpcResult CMainCharacter::CallNpc(CNpc* pNpc)
{
	FUNCTION_BEGIN;

	if(!GetGameState()->IsStateItem(eGameStateItem_Normal))
		return eAttackNpcResult_Other;
	if( !pNpc->IsNpcType(NPC_TYPE_FUNCTION) )
		return eAttackNpcResult_Other;

	stVisitNpcTradeUserCmd cmd;
	cmd.dwNpcTempID = pNpc->GetID();
	SEND_USER_CMD(cmd);
	m_dwNpcIdByLast = pNpc->GetID();
	GetGameState()->SetStateItem(eGameStateItem_NpcDialog);

	if(g_bSinglePlayer)
	{
		if(IsFileExist("Npc交易脚本.txt"))
		{
			SetFrameAllocator fa;
			FileStream file;
			file.open("Npc交易脚本.txt",FileStream::Read);
			stVisitNpcTradeUserCmd* pCmd = (stVisitNpcTradeUserCmd*)new char[sizeof(stVisitNpcTradeUserCmd) + file.getSize()];
			constructInPlace(pCmd);
			pCmd->byReturn = 1;
			pCmd->dwNpcTempID = pNpc->GetID();
			file.read(pCmd->menuTxt,file.getSize());
			pCmd->menuTxt[file.getSize()] = 0;

			GetClient()->PushCommand(pCmd,sizeof(stVisitNpcTradeUserCmd) + file.getSize());
		}
	}
	return eAttackNpcResult_Other;

	FUNCTION_END;
}

CMainCharacter::enumAttackNpcResult CMainCharacter::CallNearByNpc(enumNpcTypeItem* npcs)
{
	FUNCTION_BEGIN;

	CNpc* pNpc = GetScene()->GetNearByNpc(npcs);
	if( pNpc )
	{
		return CallNpc(pNpc);
	}

	return eAttackNpcResult_Other;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param id : 描述
 * \param target : 描述
 * \return 返回值的描述
 */
CNpc* GetAttackTarget(DWORD id,CMainCharacter::enumTargetType target)
{
	FUNCTION_BEGIN;

	switch(target)
	{
	case CMainCharacter::eTargetType_Npc:
		return GetScene()->FindNpcByID(id);
		break;
	case CMainCharacter::eTargetType_Character:
		return GetScene()->FindCharacterByID(id);
		break;
	}
	return NULL;

	FUNCTION_END;
}

bool CMainCharacter::IsHasAskedCombinPet( DWORD id )
{
	FUNCTION_BEGIN;

	if ( (id == 0) || (id == -1) )
		return false;

	DWORD petID = GetScene()->GetPetNpcTypeIdByType( PET_TYPE_SUMMON );
	if ( petID == 0 )
		return false;

	for( int i=id;i<=id+10;i++ )
	{
		if ( petID == i )
		{
			return true;
			break;
		}
	}

	return false;

	FUNCTION_END;
}

#define STOP_ATTACK_EVENT { ClearRunEvent(); ClearPath(); return; } 
/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  处理攻击事件
*/
//#pragma optimize("agst",off)
void CMainCharacter::RunAttackEvent()
{
	FUNCTION_BEGIN;

	tTargetList gridList;
	enumAttackNpcResult AttackResult;
	stPointI ptGrid;
	stPointI ptCurGrid;
	if ( (xtimeGetTime() > (m_timeSendAttack + 1360) )
		&& (m_nSendAttack > m_nRecieveAttack ) )
	{
		m_nRecieveAttack = m_nSendAttack;
	}

	//on some state can not attack
	if ( isset_state( GetState(), USTATE_ROCK )
		|| isset_state( GetState(), USTATE_LOSTSPRITE )
		|| isset_state( GetState(), USTATE_PALSY )
		|| isset_state( GetState(), USTATE_ICE_DEFENCE )
		|| isset_state( GetState(), USTATE_COME_ON_BABY )
		|| isset_state( GetState(), USTATE_USER_WABAO )
		|| isset_state( GetState(), USTATE_USER_DANCE )) //soke 跳舞、挖宝状态不能攻击
	{
		goto _ret;
	}

	//On some state can only use common attack
	if ( isset_state( GetState(), USTATE_MAGIC_DEFENCE_SWORD )
		|| isset_state( GetState(), USTATE_NO_USE_SKILL )
		|| isset_state( GetState(), USTATE_USER_WABAO )
		|| isset_state( GetState(), USTATE_USER_DANCE )) //soke 跳舞、挖宝状态不能使用技能
	{
		if ( m_RunEvent.pSkill )
            goto _ret;
	}

	//On dragon tiger state can not change face
	if ( IsDragonTigerState() )
	{
		if ( m_RunEvent.pSkill )
		{
			DWORD id = m_RunEvent.pSkill->dwID;
			if ( (SKILL_CHANGE_FACE == id) || (SKILL_HIDE_ME == id) || (SKILL_DIFFUSE_COUNTRY ==id) )
			{
				goto _ret;
			}
		}
	}

	if ( m_RunEvent.pSkill && (m_RunEvent.pSkill->dwType == SkillType_Passive) )
		goto _ret;

	if( bIsShopStart() )
	{
		GetGameGuiManager()->AddClientSystemMessage("摆摊情况下不可以攻击");
		STOP_ATTACK_EVENT;
	}

	if ( IsChangeFace() && m_RunEvent.pSkill )
		STOP_ATTACK_EVENT;

	//On combination state can only use special skill
	if ( !IsCombinCanUseSkill( m_RunEvent.pSkill ) )
	{
		if ( GetGameApplication()->bClientSetted( enumAuto_Kill_Summon ) )
			m_RunEvent.pSkill = NULL;
		else
		{
			STOP_ATTACK_EVENT;
		}		
	}

	if ( !SkillCanOnlyUseCombination( m_RunEvent.pSkill ) )
		STOP_ATTACK_EVENT;

	CNpc* pNpc = NULL;

	if( (m_RunEvent.target != eTargetType_None) && (m_RunEvent.id != -1) )
	{
		// 对象不存在了
		if( (pNpc = GetAttackTarget(m_RunEvent.id,m_RunEvent.target)) == NULL )
		{
			STOP_ATTACK_EVENT;
		}
		else
		{
			//On some state U can only attack special npc
			//Hear we mean the npc which is attacking by the team leader
			//And U just not the leader
			if ( isset_state( GetState(), USTATE_TEAM_ATTACK_ONE_DIM ) )
			{
				CGuiTeam* pTeam = GetGameGuiManager()->m_guiTeam;
				if ( pTeam && !pTeam->IsTeamHead( GetID() ) )
				{
					if ( pNpc != GetTeamInfo()->GetLeaderAttackNpc() )
					{
						STOP_ATTACK_EVENT;
					}
				}				
			}
			// 更新位置
			m_RunEvent.pt = pNpc->GetPos();
		}

		if(!pNpc->IsNpcType(NPC_TYPE_FUNCTION))
		{
			if( pNpc->IsDie() )
			{
				if(!m_bRushOut)
				{
					// 如果是尸体，只能使用对尸体的技能
					if ( !pNpc->IsDie() || m_RunEvent.pSkill == NULL || m_RunEvent.pSkill->dwTarget != SkillTarget_Death )
						STOP_ATTACK_EVENT;
				}

			}
			else
			{
				if ( m_RunEvent.pSkill && m_RunEvent.pSkill->dwTarget == SkillTarget_Death )
				{
					if ( m_RunEvent.pSkill->dwSerialType == SkillSerial_Call )
					{
						CRoleSkill* pRoleSkill = FindSkillByID( SKILL_DOOR_TO_DARK_WORLD );
						if ( NULL == pRoleSkill )
							STOP_ATTACK_EVENT;
					}
					else
                        STOP_ATTACK_EVENT;
				}
			}

			if ( pNpc
					&& !pNpc->IsMainRole()
					&& !pNpc->IsCharacter()
					&& ( (m_RunEvent.pSkill && !IsCanSelectInHandSkill(m_RunEvent.pSkill)) || (NULL == m_RunEvent.pSkill) ) )
			{
				if ( pNpc->IsMyPet() )
				{
					STOP_ATTACK_EVENT;
				}	
			}
		}
	}
	else
	{
		// 没有对象
		if(m_RunEvent.pSkill == NULL || IsActiveSkill(m_RunEvent.pSkill))
			STOP_ATTACK_EVENT;
	}

	//If this a combination skill
	for( int k=0;k<COMBINATION_STATE_NUM;k++ )
	{
		if ( m_RunEvent.pSkill && (m_RunEvent.pSkill->dwID == cmbSkill[k]) )
		{
			if ( !IsHasAskedCombinPet( cmbNpcPetID[k] ) )
				STOP_ATTACK_EVENT;
		}
	}

	//Can not attack guard when on peace state
	if ( pNpc
		&& (pNpc->GetNpcTypeItem() == NpcTypeItem_Enlisted)
		&& (GetPKMode() == PKMODE_NORMAL) )
	{
		STOP_ATTACK_EVENT;
	}

	if ( pNpc && !pNpc->IsNpcType( NPC_TYPE_FUNCTION ) )
	{
		//When on ride can not attack without fight horse
#ifndef _DEBUG
		if ( IsRide() && !IsRideFightHorse() )
		{
			GetGameGuiManager()->AddClientSystemMessage( "你的坐骑不是战马，不能战斗！" );
			STOP_ATTACK_EVENT;
		}
#endif

		//Wether cur skill be allowed to use while on ride
		if ( (IsRide() ) && (m_RunEvent.pSkill && m_RunEvent.pSkill->dwAllowRide == 0) )
			STOP_ATTACK_EVENT;
	}

	//if attack object flower we change skill to special 364
	if ( pNpc && ( pNpc->GetNpcTypeItem() == NpcTypeItem_Flower) )
	{
		if ( IsRide() || IsCombinationState() )
			STOP_ATTACK_EVENT;

		m_RunEvent.pSkill = GetSkillBase( SKILL_ATTACK_FLOWER );
	}
	if ( pNpc && ( pNpc->GetNpcTypeItem() == NpcTypeItem_Resource) )
	{
		if ( IsRide() || IsCombinationState() )
			STOP_ATTACK_EVENT;

		if (!IsEquipedPickResourceTool() )//手套
			STOP_ATTACK_EVENT;

		m_RunEvent.pSkill = GetSkillBase( SKILL_ATTACK_FLOWER );
	}

	if ( m_RunEvent.pSkill)
	{
		if( m_RunEvent.pSkill->dwID == SKILL_CHANGE_POS )
		{
			// 召唤兽的魔法

			CPet* pPet = GetScene()->FindPetByType(PET_TYPE_SUMMON);
			if(!pPet)
			{
				GetGameGuiManager()->AddClientSystemMessage( "没有召唤兽可以交换位置!" );
					STOP_ATTACK_EVENT;
			}
		}
	}

	//Get target list for special range attack skill
	
	gridList.clear();

	if ( m_RunEvent.pSkill )
	{
		switch( m_RunEvent.pSkill->dwID )
		{		
		case SKILL_STRENGTH_MULTI_ARROW:
			{
				stPointI offset(0,0);
				if ( pNpc )
					offset = pNpc->GetPos() - GetPos();
				else
					offset = m_RunEvent.pt - GetPos();

				int dir = GetDir16( offset.x, offset.y );
				gridList = GetPointList( dir, stPointF(GetDevice()->GetWidth(),GetDevice()->GetHeight()), 5, 45 );
			}
			break;
		case SKILL_SHOOT_THROUGH:
			{
				stPointI offset(0,0);
				if ( pNpc )
					offset = pNpc->GetGridPos()	- GetGridPos();
				gridList = GetPointListSingleArrow( offset, stPointF(GetDevice()->GetWidth(),GetDevice()->GetHeight()), 3 );
			}
			break;
		}
	}
	//Finish get target list


	if( m_RunEvent.pSkill && !IsActiveSkill(m_RunEvent.pSkill) && (!pNpc || !pNpc->IsNpcType(NPC_TYPE_FUNCTION))  )
	{
		//Wether cur skill be allowed to use while on ride
		if ( (IsRide()) && (m_RunEvent.pSkill && m_RunEvent.pSkill->dwAllowRide == 0) )
			STOP_ATTACK_EVENT;

		//攻击魔法,火球之类的
		if ( ( (m_RunEvent.pSkill->dwID == SKILL_RUSH_OUT) ||
		//soke 找了好久的 用征途原版的就可以使用胡家增强了
		       (m_RunEvent.pSkill->dwID == SKILL_RUSH_OUT_FAST) ) )
		//	   (m_RunEvent.pSkill->dwID == SKILL_RUSH_OUT_FAST) || (m_RunEvent.pSkill->dwID == 4) || (m_RunEvent.pSkill->dwID == 900)) )
		{
			// 冲锋
			CNpc* pNpc = GetAttackTarget(m_RunEvent.id,m_RunEvent.target);
			if ( (CanUseSkill_Ok != CanUseSkill(m_RunEvent.pSkill, pNpc, true))
				|| ( ((GetSkillDisableStatus( m_RunEvent.pSkill ) < 1.0f) ) && !m_bRushOut) )
			{
				ClearRunEvent();
				ClearPath();
				goto _ret;
			}

			ptGrid = GetScene()->PixelPosToGridPos( m_RunEvent.pt );
			ptCurGrid = GetGridPos();
			//soke 使用征途原来的
			if ( Scene_GetDis(ptGrid, ptCurGrid) > 2 )
		//	if ( Scene_GetDis(ptGrid, ptCurGrid) > 1 )
			{
				if ( pNpc && !pNpc->IsDie() )
				{
					if ( !m_bRushOut )
					{
						if ( Scene_GetDis(ptGrid, ptCurGrid) > m_RunEvent.pSkill->dwDis )
						{
							GetGameGuiManager()->AddClientSystemMessage( "与目标的距离过远，不能冲锋！" );
							ClearRunEvent();
							ClearPath();
						}
						else
						{
							stPointI ptObs;
							if( GetScene()->GetTileFlagsNum(GetGridPos(),ptGrid,ptObs,TILE_BLOCK) < 1 )
							{
								stAttackMagicUserCmd cmd;
								cmd.dwUserTempID = m_dwID;
								cmd.wdMagicType = m_RunEvent.pSkill->dwID;
								cmd.dwDefenceTempID = pNpc->GetID();
								cmd.byAttackType = pNpc->IsCharacter() ? ATTACKTYPE_U2U : ATTACKTYPE_U2N;
								cmd.byAction = Ani_Run;
								cmd.xDes = ptGrid.x;
								cmd.yDes = ptGrid.y;
								
								TRACE( "&*&*&*&**&*&*&*&*&*&*&Send rush out time=%u&*&*&*&*&*&*&*\n", xtimeGetTime() );
								SEND_USER_CMD( cmd );

								TRACE("发魔法 %s\n",m_RunEvent.pSkill->pstrName);

								SetUseSkillTime(m_RunEvent.pSkill);

								//Set skill start time: public
								m_skillStartTime = xtimeGetTime();

								m_bRushOut = true;	
								m_rushOutStartTime = xtimeGetTime();

								//Add keep image effect
								stKeepImageControl st;
								GetInterface()->StartKeepImage( st );
							}
							else
							{
								GetGameGuiManager()->AddClientSystemMessage( "中间有阻挡，暂时不能冲锋!" );
								ClearRunEvent();
								ClearPath();
							}
							goto _ret;	
						}	
					}
					else
					{
						m_RunEvent.speed = GetMoveSpeed();
						if ( GetAniSpeed() == 640/RUSH_OUT_SPEED_RATE )		//RUSH_OUT_SPEED_RATE
							WalkTo(GetScene()->GridCenterToPixelPos(pNpc->GetGridPos()),m_RunEvent.speed);

						goto _ret;
					}
				}
				else if( pNpc && pNpc->IsDie() )
				{
					if (m_bRushOut)
					{
						m_RunEvent.speed = GetMoveSpeed();
						if ( GetAniSpeed() == 640/RUSH_OUT_SPEED_RATE )		//RUSH_OUT_SPEED_RATE
							WalkTo(GetScene()->GridCenterToPixelPos(pNpc->GetGridPos()),m_RunEvent.speed);

						goto _ret;
					}
					else
					{
						m_RunEvent.pSkill = NULL;
						m_bRushOut = false;	
						GetInterface()->ClearKeepImage();
						ClearRunEvent();
						ClearPath();
					}

				}
			}
			else
			{
				if ( m_bRushOut )
				{
					//CNpc* pNpc = GetAttackTarget(m_RunEvent.id,m_RunEvent.target);
					if ( pNpc && !pNpc->IsDie() )
					{
						m_RunEvent.pSkill = m_pActiveSkill;
					}

                    m_bRushOut = false;	

					//Stop keep image effect
					GetInterface()->ClearKeepImage();
					goto _ret;
				}
			}			
		}
		else
		{
			stPointI pt1 = m_RunEvent.pt;
			//Engine_WriteLogF("m_RunEvent.pt(%d,%d)\n",m_RunEvent.pt.x,m_RunEvent.pt.y);
			AttackResult = CastMagic( m_RunEvent.pSkill,m_RunEvent.pt/*Engine_GetCursor()->GetPosition()*/,pNpc, gridList);
#ifdef _AUTOPLAY
			if(AttackResult == eAttackNpcResult_LessMP){
				// 没有魔法值了
				AttackResult = AttackNpc( pNpc , NULL, gridList );
			}
#endif
			//Engine_WriteLogF("m_RunEvent.pt(%d,%d)\n",m_RunEvent.pt.x,m_RunEvent.pt.y);
			if(pt1 != m_RunEvent.pt)
			{
				int iiiiiiii=0;
			}
			
			switch(AttackResult)
			{
			case eAttackNpcResult_TooFar:
				if(pNpc)
				{
					if ( GetKeyState( VK_CONTROL ) >= 0 )
						WalkTo(GetScene()->GridCenterToPixelPos(pNpc->GetGridPos()),m_RunEvent.speed);
				}
				else
				{
					GetGameGuiManager()->AddClientSystemMessage("超出射程！");
					STOP_ATTACK_EVENT;
				}
				break;
			case eAttackNpcResult_Succeeded:
				if ( m_pActiveSkill && m_pActiveNpc && !m_pActiveNpc->IsMainRole() && (m_RunEvent.pSkill!=m_pActiveSkill) )		//
				{
					m_RunEvent.pSkill = m_pActiveSkill;
					m_RunEvent.id = m_pActiveNpc->GetID();
					m_RunEvent.pt = m_pActiveNpc->GetPos();
					m_RunEvent.target = (m_pActiveNpc->IsCharacter() ? eTargetType_Character : eTargetType_Npc);
					goto _ret;
				}
				break;
			case eAttackNpcResult_IntervalShort:
			case eAttackNpcResult_AniError:
				if ( ((m_pActiveSkill && m_RunEvent.pSkill != m_pActiveSkill)
					|| ((m_pActiveSkill == NULL) && m_RunEvent.pSkill && !IsActiveSkill(m_RunEvent.pSkill))
					|| (m_RunEvent.pSkill == GetSkillBase(SERVER_SKILL_ATTACK_NORMAL) )
					|| (m_RunEvent.pSkill == GetSkillBase(SERVER_SKILL_BOW_NORMAL)) )
					&& m_pActiveNpc )
				{
					goto _ret;
				}
				break;
			default:
				STOP_ATTACK_EVENT;
				break;
			}

			if( m_RunEvent.target == eTargetType_None || 
				(m_RunEvent.pSkill->dwTarget & SkillTarget_Place) || 
				(m_RunEvent.pSkill->dwTarget & SkillTarget_Self) ||
				(m_RunEvent.pSkill->dwTarget & SkillTarget_Pet) ||
				(m_RunEvent.pSkill->dwTarget & SkillTarget_Summon) ||
				(m_RunEvent.pSkill->dwTarget & SkillTarget_Death) ||
				(m_RunEvent.pSkill->dwType == SkillType_StateAttack) ||
				(m_RunEvent.pSkill->dwTarget == SkillTarget_Point) )
			{
				// 群体攻击魔法，不用锁定
				// If run skill is aim at death and distance limited to nearby
				// We do not clear attack event 
				if ( !( (m_RunEvent.pSkill->dwTarget & SkillTarget_Death) && (m_RunEvent.pSkill->dwDis == 1) && (AttackResult == eAttackNpcResult_TooFar) ) )
					STOP_ATTACK_EVENT;
			}

		}
	}
	else
	{
		// 持续使用的魔法,普通攻击，弓箭攻击，刀光魔法

		if(!pNpc)
			STOP_ATTACK_EVENT;

		// 如果不是普通攻击,则使用当前选择的技能
		if( m_RunEvent.pSkill
			&& (m_RunEvent.pSkill!=GetSkillBase(SERVER_SKILL_ATTACK_NORMAL))
			&& (m_RunEvent.pSkill != GetSkillBase(SERVER_SKILL_BOW_NORMAL))
			&& (m_RunEvent.pSkill != GetSkillBase(SKILL_ATTACK_FLOWER)) )
		{
			m_RunEvent.pSkill = m_pActiveSkill;
		}


		//Wether cur skill be allowed to use while on ride
		if ( (IsRide()) && (m_RunEvent.pSkill && m_RunEvent.pSkill->dwAllowRide == 0) )
			STOP_ATTACK_EVENT;

		if( pNpc->IsNpcType(NPC_TYPE_FUNCTION) )
			AttackResult = CallNpc(pNpc);
		else
			AttackResult = AttackNpc( pNpc , m_RunEvent.pSkill, gridList );

		//This static var is for auto attack summon
		//When we try move to attack one summon many times but all goto _ret; too-far result
		//Then we give up this object and find new one
		static int nNumTryAttack = 0;
		switch(AttackResult)
		{
		case eAttackNpcResult_TooFar:
			{
				if ( GetGameApplication()->bClientSetted( enumAuto_Kill_Summon ) )
				{
					nNumTryAttack++ ;
					if ( nNumTryAttack > 200 )
					{
						nNumTryAttack = 0;
						m_pAttackNpc = NULL;
						STOP_ATTACK_EVENT;
					}
				}
				if ( GetKeyState( VK_CONTROL ) >= 0 )
					WalkTo(GetScene()->GridCenterToPixelPos(pNpc->GetGridPos()),m_RunEvent.speed);

				if ( IsSurroundWithMoveObstacle() )
				{
					if ( GetGameApplication()->bClientSetted( enumAuto_Kill_Summon ) )
					{
						m_pAttackNpc = NULL;
						STOP_ATTACK_EVENT;  						
					}
				}
			}			
			break;
		case eAttackNpcResult_Other:
			STOP_ATTACK_EVENT;
			break;
		case eAttackNpcResult_Succeeded:
			{
				nNumTryAttack = 0;
				if(pNpc->IsNpcType(NPC_TYPE_FUNCTION))
					STOP_ATTACK_EVENT;				
			}
			
			break;
		}
	}
_ret:
	;
	FUNCTION_END;
}
//#pragma optimize("",on)

/**
* \brief 处理跟随事件
* 
* 
* 
* \return 
*/
void CMainCharacter::RunFollowEvent()
{
	//NPC not exist
	if ( (NULL == GetScene()->m_pInterestNpc)
		|| (GetScene()->m_pInterestNpc->IsDie())
		|| (GetScene()->m_pInterestNpc->m_bDeleted)
		|| IsSitDown() )
	{
		m_RunEvent.event = eEventType_None;
		return;
	}
	WalkTo( GetScene()->GridCenterToPixelPos(GetScene()->m_pInterestNpc->GetGridPos()),m_RunEvent.speed );
}


void CMainCharacter::UseAutoFeedIn()
{
	FUNCTION_BEGIN;

	//This time we don't use this method
	return;

	static int autoFeedInInterval = 500;
	static int lastTimeClientFeedHP = timeGetTime() - autoFeedInInterval;
	static int lastTimeClientFeedMP = timeGetTime() - autoFeedInInterval;
	
	//soke 应该是药品回血多少
	static const int ITEM_HP_NUM = 11;
	static const int ITEM_MP_NUM = 11;
	static const DWORD ItemHP[ITEM_HP_NUM] = { 578, 579, 580, 581, 582, 583, 584, 601, 602, 603, 604 };
	static const int ItemHpValue[ITEM_HP_NUM] = { 100, 200, 450, 750, 1000, 2000, 4000, 8000, 12000, 16000, 20000 };
	static const DWORD ItemMP[ITEM_MP_NUM] = { 585, 586, 587, 588, 589, 590, 591, 611, 612, 613, 614 };
	static const int ItemMpValue[ITEM_MP_NUM] = { 30, 70, 150, 300, 750, 1000, 1500, 2000, 4000, 6000, 8000 };

	DWORD itemHpID = -1;
	DWORD itemMpID = -1;

	float mpLowest = 0.25f;
	if ( m_pActiveSkill )
	{
		CRoleSkill* pRoleSkill = FindSkillByID( m_pActiveSkill->dwID );
		SkillLevelBase_t* pSkillLevel = GetSkillLevelBase( m_pActiveSkill->dwID, pRoleSkill->GetLevel()+pRoleSkill->GetUpNum() );
		if ( pSkillLevel )
			mpLowest = (float)(pSkillLevel->dwMP*4) / (float)GetMaxMP();
	}

	if ( mpLowest < 0.25f )
		mpLowest = 0.25f;
	if ( mpLowest > 0.70f )
		mpLowest = 0.70f;

	int curNeedHP = ( GetHP() < 0.50f*GetMaxHP() ) ? GetMaxHP()-GetHP() : 0;
	int curNeedMP = ( GetMP() < mpLowest*GetMaxMP() ) ? GetMaxMP()-GetMP() : 0;

	//Get red item
	if ( (curNeedHP > 0) && (timeGetTime() - lastTimeClientFeedHP > autoFeedInInterval) )
	{
		for( int k=0;k<ITEM_HP_NUM;k++ )
		{
			CRoleItem* pItem = FindItemByLocationConstID( ItemHP[k], OBJECTCELLTYPE_COMMON );
			if ( pItem && ( (itemHpID == -1) || ( (itemHpID != -1)
					&& (ItemHpValue[k] > ItemHpValue[itemHpID])
					&& (ItemHpValue[itemHpID] < curNeedHP) ) ) )
			{
				itemHpID = k;
			}
		}
	}

	//Get blue item
	if ( (curNeedMP > 0) && (timeGetTime() - lastTimeClientFeedMP > autoFeedInInterval) )
	{
		for( int k=0;k<ITEM_MP_NUM;k++ )
		{
			CRoleItem* pItem = FindItemByLocationConstID( ItemMP[k], OBJECTCELLTYPE_COMMON );
			if ( pItem && ( (itemMpID == -1) || ( (itemMpID != -1)
					&& (ItemMpValue[k] > ItemMpValue[itemMpID])
					&& (ItemMpValue[itemMpID] < curNeedMP) ) ) )
			{
				itemMpID = k;
			}
		}
	}

	//Use red item
	if ( itemHpID != -1 )
	{
		CRoleItem* pItem = FindItemByLocationConstID( ItemHP[itemHpID], OBJECTCELLTYPE_COMMON );
		if( pItem )
		{
			CALL_USEITEM(pItem->GetThisID());
			lastTimeClientFeedHP = timeGetTime();
		}
	}

	//Use blue item
	if ( itemMpID != -1 )
	{
		CRoleItem* pItem = FindItemByLocationConstID( ItemMP[itemMpID], OBJECTCELLTYPE_COMMON );
		if( pItem )
		{
			CALL_USEITEM(pItem->GetThisID());
			lastTimeClientFeedMP = timeGetTime();
		}
	}	
	
	FUNCTION_END;
}

bool CMainCharacter::IsTeamLeader()
{
	FUNCTION_BEGIN;
	
	if ( NULL == GetGameGuiManager()->m_guiTeam ) return false;
	if ( !GetGameGuiManager()->m_guiTeam->IsTeam() )return false;
	if ( GetGameGuiManager()->m_guiTeam->m_aTeamMember.size() <= 0 ) return false;
	if ( GetGameGuiManager()->m_guiTeam->IsTeamHead( GetID() ) ) return true;
	return false;

	FUNCTION_END;
}

bool CMainCharacter::FollowTeamLeaderAtAutoAttackMode()
{
	FUNCTION_BEGIN;

	static const int nMaxDisBtwLed = 30;
	if ( NULL == GetGameGuiManager()->m_guiTeam )return false;
	if ( !GetGameGuiManager()->m_guiTeam->IsTeam() )return false;
	if ( GetGameGuiManager()->m_guiTeam->m_aTeamMember.size() <= 0 )return false;
	if ( GetGameGuiManager()->m_guiTeam->IsTeamHead( GetID() ) )return false;
	
	CGuiTeam* pTeam = GetGameGuiManager()->m_guiTeam;
	//Get team leader
	CCharacter* pLeader = GetScene()->FindCharacterByID( pTeam->m_dwHeadID );
	if ( NULL == pLeader )
		return false;

	int dis = Scene_GetDis( GetGridPos(), pLeader->GetGridPos() );
	if ( dis < nMaxDisBtwLed )
		return false;

	WalkTo( GetScene()->GridCenterToPixelPos( pLeader->GetGridPos() ), GetMoveSpeed() );

	return true;

	
	FUNCTION_END;
}


#define RANDOM_OFFSET      (((FLOAT)rand()-(FLOAT)rand())/RAND_MAX)
void CMainCharacter::RunAutoAttack( enmAutoAttackObjectType objType /* = eAutoAttack_Summon */ )
{
	FUNCTION_BEGIN;
	if ( !GetGameApplication()->bClientSetted( enumAuto_Kill_Summon ) && (objType == eAutoAttack_Summon) )
	{
	//	ClearRunEvent();
		goto _ret;
	}

	if( bIsShopStart() )
		goto _ret;

	if ( objType == eAutoAttack_Summon )
	{
		if ( FollowTeamLeaderAtAutoAttackMode() )
			goto _ret;
	}	

	//Random we pickup nearby item
	if ( objType == eAutoAttack_Summon )
	{
		if ( (timeGetTime() % 2) && 0 )
		{
			PickUpNearbyItem( false );
			goto _ret;
		}
	}

	CNpc* pNpc = NULL;
	CCharacter* pChar = NULL;

	if ( objType == eAutoAttack_Summon )
	{
		if ( m_pAttackNpc
			&& m_pAttackNpc->IsNpc()
			&& m_pAttackNpc->IsCanAutoAttackNpc()
			&& !(m_pAttackNpc->IsDie())
			&& (m_pAttackNpc->IsNpcType(NPC_TYPE_ATTACK)) )
		{
			//Make sure we attack one NPC to end
			pNpc = m_pAttackNpc;
		}
	}
	
	if ( NULL == pNpc )
	{
		if ( objType & eAutoAttack_Summon )
			pNpc = GetScene()->FindNearbySummon();
		if ( objType & eAutoAttack_Character )
			pChar = GetScene()->FindNearbyCharacter();
	}

	{
	//如果有阻挡就随即走一段距离，然后重新选择攻击目标
	stPointI ptObs,ptGrid1;
	ptGrid1 = GetScene()->PixelPosToGridPos( m_RunEvent.pt );
	if(GetScene()->GetTileFlagsNum(GetGridPos(),ptGrid1,ptObs,TILE_BLOCK) >=1)
	{
		stPointI ptPos = GetGridPos();
		
		ptPos.x += RANDOM_OFFSET * SCREEN_GRID_WIDTH*2.0f;
		ptPos.y += RANDOM_OFFSET * SCREEN_GRID_HEIGHT*2.0f;

		if ( ptPos.x <= 0 )
			ptPos.x = 1;
		if ( ptPos.x >= GetScene()->GetWidth() )
			ptPos.x = GetScene()->GetWidth() - 1;
		if ( ptPos.y <= 0 )
			ptPos.y = 1;
		if ( ptPos.x >= GetScene()->GetHeight() )
			ptPos.y = GetScene()->GetHeight() -1;

		WalkTo( GetScene()->GridCenterToPixelPos( ptPos ), GetMoveSpeed() );
		if(GetScene()->GetTileFlagsNum(GetGridPos(),ptGrid1,ptObs,TILE_BLOCK) >=1)
		{
			pNpc = GetScene()->FindNearbySummon();
		}
		
	}
	}

	if ( (NULL == pNpc) && (NULL == pChar) )
	{
		//We walk somewhere else to try find again
		stPointI ptPos = GetGridPos();
		
		ptPos.x += RANDOM_OFFSET * SCREEN_GRID_WIDTH*2.0f;
		ptPos.y += RANDOM_OFFSET * SCREEN_GRID_HEIGHT*2.0f;

		if ( ptPos.x <= 0 )
			ptPos.x = 1;
		if ( ptPos.x >= GetScene()->GetWidth() )
			ptPos.x = GetScene()->GetWidth() - 1;
		if ( ptPos.y <= 0 )
			ptPos.y = 1;
		if ( ptPos.x >= GetScene()->GetHeight() )
			ptPos.y = GetScene()->GetHeight() -1;

		WalkTo( GetScene()->GridCenterToPixelPos( ptPos ), GetMoveSpeed() );		
		// if(timeGetTime() - autotime >= 10000) //随机传送 10秒
		// {
		// 	autotime = timeGetTime();
		// 	stSuiJiChuanSongUserCmd cmd1;
		// 	SEND_USER_CMD(cmd1);
			
		// }
	}
	else //这里找到了NPC 自动打怪传送
	{
		autotime = timeGetTime();
		//If we attack both npc and character we randomly select one type
		if ( pNpc && pChar )
			pNpc = (rand() % 2) ? pNpc : pChar;

		if ( NULL == pNpc )	//Only attack character
			pNpc = pChar;

		if ( NULL == pNpc )
			goto _ret;

		m_RunEvent.id		= pNpc->GetID();
		m_RunEvent.target	= pNpc->IsCharacter() ? eTargetType_Character : eTargetType_Npc;
		m_RunEvent.pt		= pNpc->GetPos();	
		m_RunEvent.pSkill	= m_pActiveSkill;
		m_RunEvent.speed	= GetMoveSpeed();
		m_RunEvent.event	= eEventType_Attack;
		SelectInterestNpc( pNpc );		
	}
_ret:
	;
	FUNCTION_END;
}

/**
* \brief 触发跟随事件
* 
* 
* 
* \return 
*/
void CMainCharacter::SetFollowState()
{
	//int dis = Scene_GetDis(GetGridPos(),GetScene()->m_pInterestNpc->GetGridPos());
	if ( (NULL == GetScene()->m_pInterestNpc)
		|| (GetScene()->m_pInterestNpc->IsDie())
		|| (GetScene()->m_pInterestNpc->m_bDeleted) )
	{
		return;
	}	

	m_RunEvent.target = eTargetType_None;
	m_RunEvent.pt = GetScene()->m_pInterestNpc->GetPos();
	m_RunEvent.event = eEventType_Follow;
	m_RunEvent.speed = GetMoveSpeed();
}

bool CMainCharacter::IsSurroundWithMoveObstacle()
{
	stPointI ptCenter = GetGridPos();
	//Get nearby grid

	int xStart, xEnd, yStart, yEnd;
	xStart = ptCenter.x - 1;
	if ( xStart < 0 )
		xStart = 0;
	xEnd = ptCenter.x + 1;
	if ( xEnd > GetScene()->GetWidth() )
		xEnd = GetScene()->GetWidth();

	yStart = ptCenter.y - 1;
	if ( yStart < 0 )
		yStart = 0;
	yEnd = ptCenter.y + 1;
	if ( yEnd > GetScene()->GetHeight() )
		yEnd = GetScene()->GetHeight();

	//Loop through the nearby grid to see if there has any item to pick
	for ( int i=xStart;i<=xEnd;i++ )
	{
		for ( int j=yStart;j<=yEnd;j++ )
		{
			if ( (i != ptCenter.x) && (j != ptCenter.y) )
			{
				if ( !GetScene()->IsMoveObstacle( stPointI(i,j) ) )
					return false;
			}
		}
	}

	if ( m_pAttackNpc )
	{
		ptCenter = m_pAttackNpc->GetGridPos();
		xStart = ptCenter.x - 1;
		if ( xStart < 0 )
			xStart = 0;
		xEnd = ptCenter.x + 1;
		if ( xEnd > GetScene()->GetWidth() )
			xEnd = GetScene()->GetWidth();

		yStart = ptCenter.y - 1;
		if ( yStart < 0 )
			yStart = 0;
		yEnd = ptCenter.y + 1;
		if ( yEnd > GetScene()->GetHeight() )
			yEnd = GetScene()->GetHeight();

		//Loop through the nearby grid to see if there has any item to pick
		for ( int i=xStart;i<=xEnd;i++ )
		{
			for ( int j=yStart;j<=yEnd;j++ )
			{
				if ( (i != ptCenter.x) && (j != ptCenter.y) )
				{
					if ( !GetScene()->IsMoveObstacle( stPointI(i,j) ) )
						return false;
				}
			}
		}
	}

	return true;
}

/**
 * \brief 拾取附近的道具
 * 
 * 
 * 
 * \return 
 */
int(* Scene_GetGridDis)(POINT pt1, POINT pt2) = Scene_GetDis;
void CMainCharacter::PickUpNearbyItem( bool bOnlyUnderFoot /* = true */ )
{
	FUNCTION_BEGIN;
/////////////////////////star100716
	static const int ITEM_HP_NUM = 11;
	static const int ITEM_MP_NUM = 11;
	static const DWORD ItemHP[ITEM_HP_NUM] = { 578, 579, 580, 581, 582, 583, 584, 601, 602, 603, 604 };
	static const DWORD ItemMP[ITEM_MP_NUM] = { 585, 586, 587, 588, 589, 590, 591, 611, 612, 613, 614 };
/////////////////////////end
	if ( IsServerSitDown() )
		return;
	
	CItem* pItem = NULL;
	if ( bOnlyUnderFoot )
	{
		if ( pItem = GetScene()->GetObjectAtGrid<CItem>( GetGridPos() ) )
		{
			//Yes we get one
			//Then we send event
			m_RunEvent.target = eTargetType_Item;
			m_RunEvent.id = pItem->GetID();
			m_RunEvent.pt = GetGridPos();
			m_RunEvent.speed = GetMoveSpeed();
			m_RunEvent.event = eEventType_PickUp;
		}

		return;
	}

	//Get nearby grid
	int xStart, xEnd, yStart, yEnd;
	xStart = GetGridPos().x - 1;
	if ( xStart < 0 )
		xStart = 0;
	xEnd = GetGridPos().x + 1;
	if ( xEnd > GetScene()->GetWidth() )
		xEnd = GetScene()->GetWidth();

	yStart = GetGridPos().y - 1;
	if ( yStart < 0 )
		yStart = 0;
	yEnd = GetGridPos().y + 1;
	if ( yEnd > GetScene()->GetHeight() )
		yEnd = GetScene()->GetHeight();

	//Loop through the nearby grid to see if there has any item to pick
	for ( int i=xStart;i<=xEnd;i++ )
	{
		for ( int j=yStart;j<=yEnd;j++ )
		{
			if ( pItem = GetScene()->GetObjectAtGrid<CItem>( stPointI(i,j) ) )
			{
				/////////////////////////star100716 //可以叠加的物品 简单写 以后改
				bool f_in = false;
				for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
				{
					DWORD tempID = (*it)->GetObjectID(); //GetThisID
					for( int i = 0; i!= ITEM_HP_NUM; ++i)
					{
						DWORD dItem = pItem->GetBaseID();//GetID
						if((tempID == ItemHP[i])&&(tempID==dItem))
						{ f_in = true; break;}
						if((tempID == ItemMP[i])&&(tempID==dItem))
						{ f_in = true; break;}
					}
				}
				if( !f_in )
				{
					if(m_listItem.size() > 43) return;
				}
				/////////////////////////end
				//Yes we get one
				//Then we send event
				m_RunEvent.target = eTargetType_Item;
				m_RunEvent.id = pItem->GetID();
				m_RunEvent.pt = stPointI(i,j);
				m_RunEvent.speed = GetMoveSpeed();
				m_RunEvent.event = eEventType_PickUp;
				PickUpItem( pItem );   /////////////////star100515

				return;
			}
		}
	}

	FUNCTION_END;
}

void CMainCharacter::TryPickUpItem( CItem* pItem )
{
	FUNCTION_BEGIN;

	if ( NULL == pItem )
		return;

	m_RunEvent.target = eTargetType_Item;
	m_RunEvent.id = pItem->GetID();
	m_RunEvent.pt = pItem->GetGridPos();
	m_RunEvent.speed = GetMoveSpeed();
	m_RunEvent.event = eEventType_PickUp;

#ifdef _AUTOPLAY
	pItem->m_LastTryPickTime = xtimeGetTime();
#endif

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pItem : 描述
 * \return 返回值的描述
 */
CMainCharacter::enumPickUpResult CMainCharacter::PickUpItem(CItem* pItem)
{
	FUNCTION_BEGIN;

	static DWORD tLastTimePick = xtimeGetTime() - 400;

	//int dis = Scene_GetDis(GetGridPos(),pItem->GetGridPos());//////////star100810
	//if(dis > 1) return ePickUpResult_TooFar;
	//PickUpNearbyItem( false );

	if ( xtimeGetTime()-tLastTimePick < 1 )//拾取速度250改1
		return ePickUpResult_TimeError;

	//---------------------------------------------------------------------------
	DWORD dwBaseID = pItem->GetBaseID();
	if( (dwBaseID != c_nHuoYunFu) && (dwBaseID != c_nXianTianFu) && (dwBaseID != c_nXuanYuanFu) ) 
	{
		DWORD dwOwner = pItem->GetOwner();
		DWORD GuoOwner = pItem->GetGuoOwner();
		
				
		//国家归属
		if(GuoOwner !=0 && GuoOwner!= GetCountry())
		{
			
				GetGameGuiManager()->AddClientSystemMessage("该物品不属于你的国家");
				return ePickUpResult_Other;
		}
		
		if( dwOwner != 0 && dwOwner != GetID())
		{
			bool bCanPickIfTeam =   GetGameGuiManager()->m_guiTeam && 
				NULL!=GetGameGuiManager()->m_guiTeam->FindMember(GetID())
				&& GetGameGuiManager()->m_guiTeam->m_dwCurObjAssign == TEAM_OBJ_MODE_NORMAL;
			if( !bCanPickIfTeam)
			{
				GetGameGuiManager()->AddClientSystemMessage("该物品不属于你！");
				return ePickUpResult_Other;
			}
		}
	}
	//-------------------------------------------------------------------------------


	stPickUpItemPropertyUserCmd cmd;
	cmd.x = pItem->GetGridPos().x;
	cmd.y = pItem->GetGridPos().y;
	SEND_USER_CMD(cmd);

#ifdef _AUTOPLAY
	pItem->m_LastSendPickCmdTime = xtimeGetTime();
#endif

	tLastTimePick = xtimeGetTime();
	return ePickUpResult_Succeeded;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMainCharacter::RunPickUpEvent()
{
	FUNCTION_BEGIN;

	CItem* pItem = GetScene()->FindItemByID(m_RunEvent.id);
	if(!pItem)
	{
		ClearRunEvent();
		ClearPath();
		return;
	}

	enumPickUpResult result = PickUpItem(pItem);
	switch(result)
	{
	case ePickUpResult_TooFar:
		if ( IsSitDown() || IsServerSitDown() )
		{
			OnSitDown();
			return;
		}
#ifdef _AUTOPLAY
		if(xtimeGetTime() - pItem->m_LastTryPickTime > 15000){
			ClearRunEvent();
			ClearPath();
		}else
#endif
		WalkTo(GetScene()->GridCenterToPixelPos(pItem->GetGridPos()),m_RunEvent.speed);
		break;
	case ePickUpResult_TimeError:
		break;
	case ePickUpResult_Succeeded:
	case ePickUpResult_Other:
		ClearRunEvent();
		ClearPath();
		break;
	}

	FUNCTION_END;
}

bool CMainCharacter::CanUseSpecialSkill( SkillBase_t* pSkill )
{
	FUNCTION_BEGIN;

	if ( NULL == pSkill )
		return true;

	if ( pSkill->dwSerialType != SKILL_SERIAL_SPECIAL )
		return true;

	CRoleSkill * pRoleSkill = FindSkillByID( pSkill->dwID );
	if ( NULL == pRoleSkill )
		return false;
	
	//Cond. team member
	CGuiTeam* pTeam = GetGameGuiManager()->m_guiTeam;
    if ( NULL == pTeam )
		return false;

	if ( !pTeam->IsTeam() )
		return false;
	
	//Only team leader can use special skill
	if ( !pTeam->IsTeamHead( GetID() ) )
		return false;
	
	//Team member num is needed
	//Here we use Skill-Max-Dis instead as our Need-Member-Num
	int nNeedMember = 0;
	SkillLevelBase_t* pSkillLevel = GetSkillLevelBase( pSkill->dwID, pRoleSkill->GetLevel() );
	if ( pSkillLevel )
		nNeedMember = pSkillLevel->dwDis;

	if ( pTeam->GetTeamMemberAmongRange( 11 ) < nNeedMember )
		return false;

	return true;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pSkill : 描述
 * \param pTarget : 描述
 * \param bShowHit : 描述
 * \return 返回值的描述
 */
CMainCharacter::enumCanUseSkill CMainCharacter::CanUseSkill(SkillBase_t* pSkill,CNpc* pTarget,bool bShowHit)
{
	FUNCTION_BEGIN;

	if ( IsSitDown() )
		return CanUseSkill_Other;

	if ( NULL == pSkill )
		return CanUseSkill_Other;

	SetFrameAllocator fa;

	#ifdef _DEBUG
		bShowHit = true;
	#else
		bShowHit = true;
	#endif

	if ( IsChangeFace() && (pSkill->dwID != SERVER_SKILL_ATTACK_NORMAL) && (pSkill->dwID !=SERVER_SKILL_CHANGE_FACE) )
		return CanUseSkill_Other;

	//Use special skill need special cond.
	if ( !CanUseSpecialSkill( pSkill ) )
		return CanUseSkill_Other;

	//Use skill control by time
	if(pTarget)
	{
		if( !pTarget->CanUseSkillByTime(pSkill, m_skillStartTime) )
			return CanUseSkill_TimerError;
	}
	else
	{
		if ( isset_state(GetState(), USTATE_COMBIN_GOD_FIGHTER) && (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL) )
		{
			if (!CanUseSkillByTime( GetSkillBase(SERVER_SKILL_BOW_NORMAL), m_skillStartTime) )
				return CanUseSkill_TimerError;
		}
		else
		{
			if (!CanUseSkillByTime(pSkill, m_skillStartTime) )
				return CanUseSkill_TimerError;
		}
		
	}
	
	CRoleSkill * pRoleSkill = FindSkillByID( pSkill->dwID );
	int nLevel = pRoleSkill ? pRoleSkill->GetTotalLevel() : 1; 
	SkillLevelBase_t* pSkillLevelBase = GetSkillLevelBase( pSkill->dwID, nLevel );

	static DWORD s_time = 0;
	const char * pszHit = NULL;
	if ( pSkill && pSkillLevelBase )
	{
		//----------------------------
		// 自动加MP
		//
		//----------------------------
		if(gAutoGameConfig.autoAddMP)
			AutoAddMP(pSkillLevelBase->dwMP*2);
		//----------------------------

		if(GetMP() < pSkillLevelBase->dwMP)
		{
			if(bShowHit && xtimeGetTime() - s_time > 1600)
			{
				s_time = xtimeGetTime();
				GetGameGuiManager()->AddClientSystemMessage("你的法术值不够了！");
			}
			return CanUseSkill_LessMagic;
		}

		if(gAutoGameConfig.autoAddHP)
			AutoAddHP(pSkillLevelBase->dwHP);

		if(GetHP() < pSkillLevelBase->dwHP)
		{
			if(bShowHit && xtimeGetTime() - s_time > 1600)
			{
				s_time = xtimeGetTime();
				GetGameGuiManager()->AddClientSystemMessage("你的生命值不够了！");
			}
			return CanUseSkill_Other;
		}
	}

	//Apply cost item condition
	if ( pSkillLevelBase )
	{

		if ( (ItemType_Arrow == pSkillLevelBase->dwCostID) || (pSkillLevelBase->dwSerialType == 2) )
		{

		/*	if ( !IsAnyArrowHasMoreNum( 1, OBJECTCELLTYPE_EQUIP ) && !IsAnyArrowHasMoreNum( 1, OBJECTCELLTYPE_COMMON ) && (pSkillLevelBase->dwID != SKILL_GAS_ARROW ) && pSkillLevelBase->dwCostNum )
			{
				if(bShowHit && xtimeGetTime() - s_time > 1600)
				{
					s_time = xtimeGetTime();
					GetGameGuiManager()->AddClientSystemMessage( "没有箭支！" );
				}
				return CanUseSkill_Other;
			}

			if ( !IsAnyArrowHasMoreNum( pSkillLevelBase->dwCostNum, OBJECTCELLTYPE_EQUIP ) && !IsAnyArrowHasMoreNum( pSkillLevelBase->dwCostNum, OBJECTCELLTYPE_COMMON )
				&& (pSkillLevelBase->dwID != SKILL_GAS_ARROW) )
			{
				if(bShowHit && xtimeGetTime() - s_time > 1600)
				{
					s_time = xtimeGetTime();
					GetGameGuiManager()->AddClientSystemMessage( "箭支数量不够！" );
				}
				return CanUseSkill_Other;
			}*/
		}
		else
		{
			if ( GetItemCount( pSkillLevelBase->dwCostID ) < pSkillLevelBase->dwCostNum )
			{
				if(bShowHit && xtimeGetTime() - s_time > 1600)
				{
					s_time = xtimeGetTime();
					GetGameGuiManager()->AddClientSystemMessage( "所需物品数量不够！" );
				}
				return CanUseSkill_Other;
			}
		}
	}
	//

	//Following to see wether character has valid weapon in hand
	std::vector<UINT, std::stack_allocator<UINT> > aid;
	aid.reserve(16);
	_parse_str_num(aid,pSkill->pstrConsume);

	ObjectBase_t* pObjWL = NULL,*pObjWR = NULL;

	pObjWL = GetObjectBase(GetSculpt()->dwLeftHandID);
	pObjWR = GetObjectBase(GetSculpt()->dwRightHandID);
	if ( IsRide() && !IsRideFightHorse() )
	{
		GetGameGuiManager()->AddClientSystemMessage( "你的坐骑不是战马，不能战斗！" );
		return CanUseSkill_Other;
	}
	int costItemCount;
	if ( pSkillLevelBase )
		costItemCount = pSkillLevelBase->dwCostNum;
	else
	{
		if ( pSkill== GetSkillBase(SERVER_SKILL_BOW_NORMAL) )
			costItemCount = 1;
		else
			costItemCount = 0;
	}

	for(size_t i=0;i<aid.size();i++)
	{
		if(aid[i] != 0)
		{
			//桃子 弓箭掌握（ID= 71），要求弓箭和箭支都要拿在手里  这里是处理弓箭不能使用技能的问题
			if ( pSkill->dwSerialType == 2 )
			{
			 // if ( pObjWL && (pObjWL->dwType == aid[i]) )//修改弓箭在左手使用技能也必须在左手pObjWL是左手pObjWR是右手	 左手弓
		        if ( pObjWR && (pObjWR->dwType == aid[i]) )//修改弓箭在左手使用技能也必须在左手pObjWL是左手pObjWR是右手	 右手弓
				{
					if ( (pSkill->dwID == SKILL_GAS_ARROW) || (costItemCount == 0) )//箭支战旗在右手
						return CanUseSkill_Ok;
					else
					{
						static bool bEquipArrowState = false;
					  if ( pObjWR && (pObjWR->dwType == ItemType_Arrow || pObjWR->dwType == ItemType_CrossFlag) )		//箭支右手
					  //if ( pObjWL && (pObjWL->dwType == ItemType_Arrow || pObjWL->dwType == ItemType_CrossFlag) )		//箭支左手
						{
							//if( IsAnyArrowHasMoreNum( costItemCount, OBJECTCELLTYPE_EQUIP ) )	// 有箭支
							//{
								bEquipArrowState = false;
								return CanUseSkill_Ok;
							//}
							/*else
							{
								if ( IsAnyArrowHasMoreNum( costItemCount ) )
								{
									if ( bEquipArrowState || EquipArrow( costItemCount ) )
									{
										if ( !bEquipArrowState )
											bEquipArrowState = true;
										return CanUseSkill_TimerError;
									}
									else
										return CanUseSkill_Other;
								}							
								else
								{
									if ( bShowHit )
										GetGameGuiManager()->AddClientSystemMessage( "箭支数量不够！" );
									return CanUseSkill_Other;
								}
							}*/
						}
						else
						{
							if ( IsAnyArrowHasMoreNum( costItemCount ) )
							{
								if ( bEquipArrowState || EquipArrow( costItemCount ) )
								{
									if ( !bEquipArrowState )
										bEquipArrowState = true;

									return CanUseSkill_TimerError;
								}
								else
									return CanUseSkill_Other;
							}
							else
							{
								if ( bShowHit )
									GetGameGuiManager()->AddClientSystemMessage( "箭支数量不够！" );
								return CanUseSkill_Other;
							}							
						}
					}					
				}
			}
			else
			{
				if(pObjWL)
				{
					if(pObjWL->dwType == aid[i]) return CanUseSkill_Ok;
				}
				if(pObjWR)
				{
					if(pObjWR->dwType == aid[i]) return CanUseSkill_Ok;
				}
			}			
		}
		else
		{
			return CanUseSkill_Ok;
		}
	}

	if ( 0 == i )
		return CanUseSkill_Ok;

	if(bShowHit && xtimeGetTime() - s_time > 1600)
	{
		s_time = xtimeGetTime();
		GetGameGuiManager()->AddClientSystemMessage( "没有相应武器，不能施放技能!" );
	}

	return CanUseSkill_Other;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pChar : 描述
 * \return 返回值的描述
 */
bool CMainCharacter::CanAttackCharacter(CCharacter* pChar)
{
	FUNCTION_BEGIN;

	return CanAttack();

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pNpc : 描述
 * \return 返回值的描述
 */
bool CMainCharacter::CanAttackNpc(CNpc* pNpc)
{
	FUNCTION_BEGIN;

	return CanAttack();

	FUNCTION_END;
}

void GetFireWallOffsetByDir(const BYTE dir, stPointI& offset)
{
	switch( dir )
	{
	case 0:
		offset.x = 0;
		offset.y = -2;
		break;
	case 1:
		offset.x = 2;
		offset.y = -2;
		break;
	case 2:
		offset.x = 2;
		offset.y = 0;
		break;
	case 3:
		offset.x = 2;
		offset.y = 2;
		break;
	case 4:
		offset.x = 0;
		offset.y = 2;
		break;
	case 5:
		offset.x = -2;
		offset.y = 2;
		break;
	case 6:
		offset.x = -2;
		offset.y = 0;
		break;
	case 7:
		offset.x = -2;
		offset.y = -2;
		break;
	default:
		offset.x = 0;
		offset.y = 0;
		break;
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pSkill : 描述
 * \param pt : 描述
 * \return 返回值的描述
 */

CMainCharacter::enumAttackNpcResult CMainCharacter::CastMagic(SkillBase_t* pSkill,POINT pt,CNpc * pTarget, tTargetList& targetList)
{
	enumAttackNpcResult retval;

	FUNCTION_BEGIN;
	stAttackMagicUserCmd cmd;
	stPointI ptGrid;
	stPointI ptCurGrid;

	if(m_nSendAttack > m_nRecieveAttack) 
	{
		RETURN(eAttackNpcResult_IntervalShort);
	}
	if(!CanSendCommand(MAPSCREEN_USERCMD)) RETURN(eAttackNpcResult_IntervalShort);
	if (HaveCommand(MAGIC_USERCMD,MAGIC_USERCMD_PARA)) RETURN(eAttackNpcResult_IntervalShort);
	
	enumCanUseSkill rCanUseSkill;
	if ((rCanUseSkill = CanUseSkill(pSkill,NULL,true)) != CanUseSkill_Ok)
	{
		//TRACE("不能发魔法 %s\n",pSkill->pstrName);
		if(rCanUseSkill == CanUseSkill_TimerError)
			RETURN(eAttackNpcResult_IntervalShort);
		if(rCanUseSkill == CanUseSkill_LessMagic)
			RETURN(eAttackNpcResult_LessMP);
		RETURN(eAttackNpcResult_Other);
	}

	int byAttackType = 0;
	byAttackType = ATTACKTYPE_U2U;

	int id = -1;

	BYTE attack = 0;

	CSceneObject* pObj = NULL;

	CCharacter* pChar = NULL;
	CNpc* pNpc = NULL;

	ptGrid = GetScene()->PixelPosToGridPos(pt);
	ptCurGrid = GetGridPos();

	{
		int dis = pSkill->dwDis;
		if (abs(ptGrid.x - ptCurGrid.x) > dis || abs(ptGrid.y - ptCurGrid.y) > dis)
		{
			// 距离太远了
			RETURN(eAttackNpcResult_TooFar);
		}
	}
	

	ClearPath();

	if(pTarget) 
	{
		pNpc = pTarget;
	}
	else
	{
		pNpc = (( GetScene()->m_pMouseOverObject && GetScene()->m_pMouseOverObject->IsSceneObjectType( OBJECT_NPC )) ? (CNpc*)GetScene()->m_pMouseOverObject : NULL);

		if ( pSkill->dwID == SKILL_STRENGTHEN_SUMMON )
		{
			if ( NULL == pNpc )
			{
				CPet* pPet = GetScene()->FindPetByType( PET_TYPE_SUMMON );

				if ( pPet )
					pNpc = pPet->GetNpc();

				if ( NULL == pNpc )
				{
					GetGameGuiManager()->AddClientSystemMessage( "无有效攻击目标！" );
				}
			}
			else
			{
				if( !pNpc->IsNpc() || !pNpc->IsPet() || (pNpc->GetPetType() != PET_TYPE_SUMMON) )
				{
					GetGameGuiManager()->AddClientSystemMessage( "无有效攻击目标！" );
				}
			}
		}
	}
	
	if( pNpc )
	{
		pChar = pNpc->IsSceneObjectType(OBJECT_CHARACTER) ? static_cast<CCharacter*>(pNpc) : NULL;
	}

	if ( pChar )
	{
		if ( pChar
			&& !(pSkill->dwTarget & SkillTarget_Friend)
			&& (pSkill->dwID != SKILL_RELIVE_FRIEND)
			&& !(pSkill->dwTarget & SkillTarget_Self)
			&& !(pSkill->dwTarget & SkillTarget_Pet)
			&& !(pSkill->dwTarget & SkillTarget_Summon) )
		{
			if ( !(CCountry::Instance()->IsCountryWarState()) )
			{
				//Can not attack new face (Level < 10 )
				if ( pChar->GetLevel() < NEW_FACE_PROTECT_LEVEL )
				{
					GetGameGuiManager()->AddClientSystemMessage( "不能攻击低于20级的玩家!" );
					ClearRunEvent();
					ClearPath();
					RETURN(eAttackNpcResult_Other);
				}

				//new face can not attack other character
				if ( GetLevel() < START_PK_LEVEL )
				{
					GetGameGuiManager()->AddClientSystemMessage( "你等级低于20级不能进行PK!" );
					ClearRunEvent();
					ClearPath();
					RETURN(eAttackNpcResult_Other);
				}
			}			

			if ( !pChar->IsCanAttack() )
				RETURN(eAttackNpcResult_Other);
		}
	}

	enumSkillTargetType target = pSkill->dwTarget;

	//If have studied  special skill
	//Don't need death ash for call summon skill
	if ( target == SkillTarget_Death )
	{
		if ( pSkill->dwSerialType == SkillSerial_Call )
		{
			CRoleSkill* pRoleSkill = FindSkillByID( SKILL_DOOR_TO_DARK_WORLD );
			if ( pRoleSkill )
			{
				target = SkillTarget_Place;
			}
		}
	}
	//End

	if ( ( target & SkillTarget_Friend ) || ( target & SkillTarget_Enemy ) )
	{
		// 目的是人物
		if ( pChar == NULL && pNpc == NULL && m_RunEvent.target == eTargetType_Character)
			pChar = GetScene()->FindCharacterByID(m_RunEvent.id);

		if (pChar && CanAttackCharacter(pChar))
		{
			ptGrid = pChar->GetGridPos();
			pObj = pChar;
			attack = ATTACKTYPE_U2U;
		}		
	}

	if ( (target == SkillTarget_Single) && !pObj )
	{
		if ( (pNpc == NULL) && (pChar == NULL) )
		{
			if ( m_RunEvent.target == eTargetType_Character )
			{
				pChar = GetScene()->FindCharacterByID( m_RunEvent.id );
				pNpc = pChar;
			}
			else
				pNpc = GetScene()->FindNpcByID( m_RunEvent.id );
		}

		if ( (pChar == NULL) && IsCharacterUseOnly( pSkill ) )
			RETURN(eAttackNpcResult_Other);

		if ( pNpc && CanAttackNpc(pNpc) )
		{
			ptGrid = pNpc->GetGridPos();
			pObj = pNpc;
			attack = pNpc->IsCharacter() ? ATTACKTYPE_U2U : ATTACKTYPE_U2N;
		}
		else
			RETURN(eAttackNpcResult_Other);
	}
	
	//This time we treat pet and summon separately 
	//This time we treat pet and summon separately 
	if ( (target & SkillTarget_Pet) && !pObj )
	{
		// Target is pets
		if (pChar == NULL && pNpc == NULL && m_RunEvent.target == eTargetType_Npc)
			pNpc = GetScene()->FindNpcByID(m_RunEvent.id);

		if ( pNpc && CanAttackNpc( pNpc ) )
		{
			ptGrid = pNpc->GetGridPos();
			if ( pNpc->IsPet() && (pNpc->GetPetType() == PET_TYPE_PET) )		//Target is pet
			{
				pObj = pNpc;
				attack = ATTACKTYPE_U2N;
			}
			else if ( target == SkillTarget_Pet )
				RETURN(eAttackNpcResult_Other);
		}
	}

	if ( (target & SkillTarget_Summon) && !pObj )
	{
		// Target is pets
		if (pChar == NULL && pNpc == NULL && m_RunEvent.target == eTargetType_Npc)
			pNpc = GetScene()->FindNpcByID(m_RunEvent.id);

		if ( pNpc && CanAttackNpc( pNpc ) )
		{
			ptGrid = pNpc->GetGridPos();
			if ( pNpc->IsPet() && (pNpc->GetPetType() == PET_TYPE_SUMMON) )		//Target is summon
			{
				pObj = pNpc;
				attack = ATTACKTYPE_U2N;
			}
			else if ( target == SkillTarget_Summon )
				RETURN(eAttackNpcResult_Other);
		}
	}

	if ( (target & SkillTarget_Npc) && !pObj )
	{
		// 目标是NPC
		if (pChar == NULL && pNpc == NULL && m_RunEvent.target == eTargetType_Npc)
			pNpc = GetScene()->FindNpcByID(m_RunEvent.id);
		if (pNpc && CanAttackNpc(pNpc))
		{
			NpcBase_t* pNpcBase = GetNpcBase( pNpc->GetTypeID() );
			if ( NULL == pNpcBase )
				RETURN(eAttackNpcResult_Other);
			
			//Valid catch summon npc type
			if ( pSkill->dwID == SKILL_CATCH_SUMMON_YE )
			{
				if ( pNpc->IsPet() || pNpc->IsCharacter() )
					RETURN(eAttackNpcResult_Other);

				if ( pNpc->GetLevel() > GetLevel() )
				{
					GetGameGuiManager()->AddClientSystemMessage( "当前驯捕对象等级太高！" );
					RETURN(eAttackNpcResult_Other);
				}
				if ( pNpcBase->dwSummonType != NpcSummonType_Animal )
				{
					GetGameGuiManager()->AddClientSystemMessage( "当前驯捕对象不是野兽！" );
					RETURN(eAttackNpcResult_Other);
				}
			}
			else if ( pSkill->dwID == SKILL_CATCH_SUMMON_LING )
			{
				if ( pNpc->IsPet() || pNpc->IsCharacter() )
					RETURN(eAttackNpcResult_Other);

				if ( pNpc->GetLevel() > GetLevel() )
				{
					GetGameGuiManager()->AddClientSystemMessage( "当前驯捕对象等级太高！" );
					RETURN(eAttackNpcResult_Other);
				}

				if ( pNpcBase->dwSummonType != NpcSummonType_Ling )
				{
					GetGameGuiManager()->AddClientSystemMessage( "当前驯捕对象不是灵兽！" );
					RETURN(eAttackNpcResult_Other);
				}
			}
			else if ( pSkill->dwID == SKILL_CATCH_SUMMON_MO )
			{
				if ( pNpc->IsPet() || pNpc->IsCharacter() )
					RETURN(eAttackNpcResult_Other);

				if ( pNpc->GetLevel() > GetLevel() )
				{
					GetGameGuiManager()->AddClientSystemMessage( "当前驯捕对象等级太高！" );
					RETURN(eAttackNpcResult_Other);
				}

				if ( pNpcBase->dwSummonType != NpcSummonType_Mo )
				{
					GetGameGuiManager()->AddClientSystemMessage( "当前驯捕对象不是魔兽！" );
					RETURN(eAttackNpcResult_Other);
				}
			}
			else if ( pSkill->dwID == SKILL_CATCH_SUMMON_SENG )
			{
				if ( pNpc->IsPet() || pNpc->IsCharacter() )
					RETURN(eAttackNpcResult_Other);

				if ( pNpc->GetLevel() > GetLevel() )
					RETURN(eAttackNpcResult_Other);

				if ( pNpcBase->dwSummonType != NpcSummonType_Seng )
				{
					GetGameGuiManager()->AddClientSystemMessage( "当前驯捕对象不是圣兽！" );
					RETURN(eAttackNpcResult_Other);
				}
			}

			ptGrid = pNpc->GetGridPos();
			pObj = pNpc;
			attack = ATTACKTYPE_U2N;
		}
	}

	if ( (target & SkillTarget_Death) && !pObj )
	{
		if ( m_RunEvent.target == eTargetType_Npc )			// 目标是NPC
		{
			if ( pSkill->dwID == SKILL_RELIVE_FRIEND )
				RETURN(eAttackNpcResult_Other);

			if (pChar == NULL && pNpc == NULL)
				pNpc = GetScene()->FindNpcByID(m_RunEvent.id);

			if (pNpc && CanAttackNpc(pNpc) && pNpc->IsDie() )
			{
				ptGrid = pNpc->GetGridPos();
				pObj = pNpc;
				attack = ATTACKTYPE_U2N;
			}
			else
				RETURN(eAttackNpcResult_Other);
		}
		else if ( m_RunEvent.target == eTargetType_Character )			// Target is dead player
		{
			if ( pSkill->dwID == SKILL_EAT_DEATH )
				RETURN(eAttackNpcResult_Other);

			if (pChar == NULL && pNpc == NULL)
				pChar = GetScene()->FindCharacterByID(m_RunEvent.id);

			if (pChar && !pChar->IsMainRole() && pChar->IsDie() )
			{
				ptGrid = pChar->GetGridPos();
				pObj = pChar;
				attack = ATTACKTYPE_U2U;
			}
			else
				RETURN(eAttackNpcResult_Other);
		}
	}

	if( (target & SkillTarget_Place) && !pObj)
	{
		// 目标是群体
		if ( pNpc ) ptGrid = pNpc->GetGridPos();

		if ( GetScene()->IsMagicObstacle(ptGrid) )
			RETURN(eAttackNpcResult_Other);

		pObj = pNpc;

		if(pChar) attack = ATTACKTYPE_U2U;
		else if(pNpc)attack = ATTACKTYPE_U2N;
		else attack = ATTACKTYPE_U2P;
	}

	if ((target & SkillTarget_Self) && !pObj)
	{
		// 目标是自己

		if ( pSkill->dwID == SKILL_RELIVE_PET )
		{
			CPet* pPet = GetScene()->FindPetByType(PET_TYPE_PET);

			if ( pPet && !pPet->bIsDelete() )
				RETURN(eAttackNpcResult_Other);
		}
		pObj = this;
		attack = ATTACKTYPE_U2U;
	}

	if(pObj)
	{
		id = pObj->m_dwID;
		ptGrid = pObj->GetGridPos();
		
		if ( (pSkill->dwType == SkillType_Active) || (pSkill->dwType == SkillType_Attack) )
		{
			m_pActiveNpc = static_cast<CNpc*>(pObj);
			m_pAttackNpc = static_cast<CNpc*>(pObj);
		}		
	}
	else
	{
		if ( GetScene()->m_pInterestNpc && !GetScene()->m_pInterestNpc->IsDie() )
			RETURN(eAttackNpcResult_Other);
		
		attack = ATTACKTYPE_U2P;
	}

	cmd.dwUserTempID	= m_dwID;
	cmd.wdMagicType		= pSkill->dwID;
	cmd.dwDefenceTempID = id;
	cmd.byAttackType = attack;
	cmd.xDes = ptGrid.x;
	cmd.yDes = ptGrid.y;

	cmd.byDirect = Scene_GetDir8(cmd.xDes - GetGridPos().x,cmd.yDes - GetGridPos().y);
	if ( ptGrid == GetGridPos() )
		cmd.byDirect = 0;

	if ( pSkill && (pSkill->dwID == SKILL_FIRE_WALL) )
	{
		cmd.dwUserTempID = -1;
	}
	
	if (cmd.byDirect == 0xff)
	{
		if (pObj != this)
			RETURN(eAttackNpcResult_Other);
	}

	POINT ptObs;

	enumCastMagicResult rt = eCastMagicResult_Succeeded;


	if ( pSkill->dwType != SkillType_Attack && pSkill->dwType != SkillType_StateAttack && pSkill->dwType != SkillType_Active)
	{
		// 光环魔法
	}
	else
	{
		int iClientSkill = ServerSkillToClientSkill(pSkill);

		MagicProperty* p = Property_GetTableManager()->GetMagicProperty(iClientSkill,0);

		if(!p) RETURN(eAttackNpcResult_Other);

		DWORD dwTimeAni = xtimeGetTime();
		cmd.byAction = m_pCharacter->GetRandAni(p->stCharacter.eAnimation,p->stCharacter.eAnimation2,p->stCharacter.eAnimation3);

		if(IsRide())
		{
			if(IsMagicAni((AniTypeEnum)cmd.byAction))
				cmd.byAction = Ani_Ride_Magic;
			else
				cmd.byAction = Ani_Ride_Attack;				
		}		

		if(!GetInterface()->IsExistAction((AniTypeEnum)cmd.byAction))
			RETURN(eAttackNpcResult_AniError);

		if(cmd.byAction == Ani_Null || m_pCharacter->CanChangeToAction(Ani_Magic) )
			rt = CNpc::CastMagic(&cmd,ptObs,false);
		else 
			RETURN(eAttackNpcResult_AniError);

		if(rt == eCastMagicResult_Succeeded)
			m_pCharacter->UpdatePrevAniEndTime(xtimeGetTime()-dwTimeAni);
	}

	if ( rt == eCastMagicResult_Succeeded )
	{
		if ( rt == eCastMagicResult_Obstruct )
		{
			// 中间有阻挡
			cmd.dwDefenceTempID = 0;
			cmd.byAttackType = ATTACKTYPE_U2P;
			cmd.xDes = ptObs.x;
			cmd.yDes = ptObs.y;
		}

		if ( pSkill && (pSkill->dwID == SKILL_FIRE_WALL) )
		{
			if ( 0 )
			{
				stPointI offset;
				GetFireWallOffsetByDir( cmd.byDirect, offset );
				cmd.xDes = GetGridPos().x + offset.x;
				cmd.yDes = GetGridPos().y + offset.y;
			}
			else
			{
				cmd.xDes = GetGridPos().x;
				cmd.yDes = GetGridPos().y;
			}
		}

		SEND_USER_CMD(cmd);
		m_nSendAttack++;
		m_timeSendAttack = xtimeGetTime();

		TRACE("发魔法 %s\n",pSkill->pstrName);

		SetUseSkillTime(pSkill);

		//Set skill start time: public
		m_skillStartTime = xtimeGetTime();

#ifdef _DEBUG
		static DWORD lastSendAttackTime = xtimeGetTime(); 
		TRACE( "*************************************************Magic************Send attack time=%u\n", xtimeGetTime()-lastSendAttackTime );
		lastSendAttackTime = xtimeGetTime();
#endif
		if(g_bSinglePlayer)
		{
			GetClient()->PushCommand(&cmd,sizeof(cmd));

			if(id != -1)
			{
				stRTMagicUserCmd rcmd;
				rcmd.byDirect = cmd.byDirect;
				rcmd.byLuck = 0;
				rcmd.byRetcode = (rand()%3) ? RTMAGIC_FAILURE : RTMAGIC_SUCCESS;
				rcmd.bySrc = MAPDATATYPE_USER;
				rcmd.byTarget = MAPDATATYPE_NPC;
				rcmd.dwSrcTempID = GetID();
				rcmd.dwUserTempID = id;
				rcmd.wdHP = 10;
				GetClient()->PushCommand(&rcmd,sizeof(rcmd));
			}
		}

		RETURN(eAttackNpcResult_Succeeded);
	}
_ret:
	;
	FUNCTION_END;
	return retval;
}

int CMainCharacter::GetArrowNum( bool bEquipOn )
{
	int num = 0;
	if ( bEquipOn )
	{
		for( int i=0;i<ARROW_TYPE_NUM;i++ )
		{
			if ( ( num = GetItemExteriorCountByConstID( BOW_ARROW_ITEM_ID,OBJECTCELLTYPE_EQUIP ) ) > 0 )
				break;		
		}
	}
	else
	{
		num = GetItemExteriorCountByType( ItemType_Arrow );
	}

	return num;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pNpc : 描述
 * \param pSkill : 描述
 * \return 返回值的描述
 */
DWORD g_nRunTime = 0;
const int c_nDistanceSign = 20;
//#pragma optimize("agst",off)
CMainCharacter::enumAttackNpcResult CMainCharacter::AttackNpc(CNpc* pNpc,SkillBase_t * pSkill,tTargetList& targetList)
{
	enumAttackNpcResult retval;

	FUNCTION_BEGIN;
	
	stPointI ptDst;
	void* pCMD = NULL;
	retval = eAttackNpcResult_Succeeded;

	if(m_nSendAttack > m_nRecieveAttack)
	{
		//TRACE( "上一个动作未完成，不能攻击！&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n" );
		//Engine_WriteLog("上一个动作未完成，不能攻击！\n");
		RETURN(eAttackNpcResult_IntervalShort);
	}

	//TRACE( "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$Try attack npc time=%u\n", xtimeGetTime() );
	if ( pNpc && pNpc->IsCharacter() && !(CCountry::Instance()->IsCountryWarState()) )
	{
		//Can not attack new face (Level < 10 )
		if ( ((CCharacter*)pNpc)->GetLevel() < NEW_FACE_PROTECT_LEVEL )		//
		{
			GetGameGuiManager()->AddClientSystemMessage( "不能攻击低于20级的玩家!" );
			RETURN(eAttackNpcResult_Other);
		}
		
		//new face can not attack other character
		if ( GetLevel() < START_PK_LEVEL )
		{
			GetGameGuiManager()->AddClientSystemMessage( "你等级低于20级不能进行PK!" );
			RETURN(eAttackNpcResult_Other);
		}		
	}
	
	m_pActiveNpc = pNpc;
	m_pAttackNpc = pNpc;

	if(!CanSendCommand(MAPSCREEN_USERCMD))
		RETURN(eAttackNpcResult_IntervalShort);	
	if (HaveCommand(MAGIC_USERCMD,MAGIC_USERCMD_PARA))
		RETURN(eAttackNpcResult_IntervalShort);	

	if(!pNpc)
		RETURN(eAttackNpcResult_Other);
	if(pNpc->IsDie())
	{
		if ( pSkill )
		{
			if ( !(pSkill->dwTarget == SkillTarget_Death) )
				RETURN(eAttackNpcResult_Other);			
		}
		else
			RETURN(eAttackNpcResult_Died);
	}
	else
	{
		if ( pSkill )
		{
			if ( pSkill->dwTarget == SkillTarget_Death )
				RETURN(eAttackNpcResult_Other);		
		}
	}

	int dis = Scene_GetDisPixel( GetPos(), pNpc->GetPos() );

	if(dis > c_nDistanceSign)
		RETURN(eAttackNpcResult_TooFar);

	if(!CanAttack() || !pNpc->IsCanAttack())
		RETURN(eAttackNpcResult_Other);

	if(pSkill && IsActiveSkill(pSkill))
	{
		enumCanUseSkill rCanUseSkill;
		if ( (rCanUseSkill = CanUseSkill(pSkill,NULL,true)) != CanUseSkill_Ok )
		{
			//TRACE("不能发魔法 %s\n",pSkill->pstrName);
			if(rCanUseSkill == CanUseSkill_TimerError)
				RETURN(eAttackNpcResult_IntervalShort);
			//pSkill = NULL;
		}
	}
    //处理弓箭不能平A问题 GetRightWeaponType右手   GetLeftWeaponType左手
	if( (GetRightWeaponType() == ItemType_Bow) && !IsChangeFace() && !IsCombinationState() )
	{

		if( !pSkill || !IsActiveSkill(pSkill)
			|| (pSkill && (pSkill == GetSkillBase( SERVER_SKILL_ATTACK_NORMAL))))
		{
		
		    m_RunEvent.pSkill = GetSkillBase( SERVER_SKILL_BOW_NORMAL );
			pSkill = m_RunEvent.pSkill;	
    //处理弓箭不能平A问题 GetRightWeaponType右手   GetLeftWeaponType左手
	/*
	if( (GetLeftWeaponType() == ItemType_Bow) && !IsChangeFace() && !IsCombinationState() )
	{

		if( !pSkill || !IsActiveSkill(pSkill)
			|| (pSkill && (pSkill == GetSkillBase( SERVER_SKILL_ATTACK_NORMAL))))
		{
		
		    m_RunEvent.pSkill = GetSkillBase( SERVER_SKILL_BOW_NORMAL );
			pSkill = m_RunEvent.pSkill;*/
	////////////////////////////////////////////////////////////////////////结束		
			/*if( IsAnyArrowHasMoreNum( 1, OBJECTCELLTYPE_EQUIP )
				&& (GetRightWeaponType() == ItemType_Arrow) )		// 有箭支
			{
				m_RunEvent.pSkill = GetSkillBase( SERVER_SKILL_BOW_NORMAL );
				RETURN(eAttackNpcResult_IntervalShort);
			}
			else
			{
				if ( EquipArrow( 1 ) )
				{
					m_RunEvent.pSkill = GetSkillBase( SERVER_SKILL_BOW_NORMAL );
					RETURN(eAttackNpcResult_IntervalShort);
				}
				else
				{
					GetGameGuiManager()->AddClientSystemMessage( "没有箭支！" );
					RETURN(eAttackNpcResult_Other);
				}				
			}*/
		}
	}

	if ( pSkill && (pSkill == GetSkillBase( SERVER_SKILL_BOW_NORMAL)) )
	{
		m_RunEvent.pSkill = GetSkillBase( SERVER_SKILL_BOW_NORMAL );
		pSkill = m_RunEvent.pSkill;

		/*if( IsAnyArrowHasMoreNum( 1, OBJECTCELLTYPE_EQUIP )
			&& (GetRightWeaponType() == ItemType_Arrow) )		// 有箭支
		{
			
		}
		else
		{
			if ( EquipArrow( 1 ) )
			{
				m_RunEvent.pSkill = GetSkillBase( SERVER_SKILL_BOW_NORMAL );
				RETURN(eAttackNpcResult_IntervalShort);
			}
			else
			{
				GetGameGuiManager()->AddClientSystemMessage( "没有箭支！" );
				RETURN(eAttackNpcResult_Other);
			}			
		}*/
	}


	if(!pSkill || !IsActiveSkill(pSkill)) pSkill = GetSkillBase(SERVER_SKILL_ATTACK_NORMAL);

	if( (pSkill != GetSkillBase(SERVER_SKILL_ATTACK_NORMAL)) || isset_state( GetState(), USTATE_COMBIN_GOD_FIGHTER ) )		//IsActiveSkill(pSkill)
	{
		enumCanUseSkill rCanUseSkill;
		if ( (rCanUseSkill = CanUseSkill(pSkill,NULL,true)) != CanUseSkill_Ok )
		{
			//TRACE("不能发魔法 %s\n",pSkill->pstrName);
			if(rCanUseSkill == CanUseSkill_TimerError)
				RETURN(eAttackNpcResult_IntervalShort);
			
			if ( rCanUseSkill == CanUseSkill_LessMagic )
			{
				pSkill = NULL;
             // 左手GetLeftWeaponType  右手GetRightWeaponType
			 // if( IsAnyArrowHasMoreNum( 1, OBJECTCELLTYPE_EQUIP ) && (GetRightWeaponType() == ItemType_Arrow) )		// 桃子 处理弓箭不能放技能问题 有箭支
             // 左手GetLeftWeaponType  右手GetRightWeaponType
				if( IsAnyArrowHasMoreNum( 1, OBJECTCELLTYPE_EQUIP ) && (GetLeftWeaponType() == ItemType_Arrow) )		// 桃子 处理弓箭不能放技能问题 有箭支
				{
					pSkill = GetSkillBase(SERVER_SKILL_BOW_NORMAL);
				}
				else
				{
					pSkill = GetSkillBase(SERVER_SKILL_ATTACK_NORMAL);
				}
			}
			else
				RETURN(eAttackNpcResult_Other);
		}
	}	
	else
	{
		enumCanUseSkill rCanUseSkill;
		if ( (rCanUseSkill = CanUseSkill(GetSkillBase(SERVER_SKILL_ATTACK_NORMAL),NULL,true)) != CanUseSkill_Ok )
		{
			//TRACE("不能发魔法 %s\n",pSkill->pstrName);
			if(rCanUseSkill == CanUseSkill_TimerError)
				RETURN(eAttackNpcResult_IntervalShort);
		}
	}

	int disMax = pSkill->dwDis;

	if ( ((pSkill == NULL) || (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL)) && isset_state(GetState(), USTATE_COMBIN_GOD_FIGHTER) )
		disMax = 11;
	if ( ((pSkill == NULL) || (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL)) && isset_state(GetState(), USTATE_EAT_GHOST) )
		disMax = 11;
	if ( ((pSkill == NULL) || (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL)) && isset_state(GetState(), USTATE_COMBIN_XIAN_PET) )
		disMax = 11;
	if ( ((pSkill == NULL) || (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL)) && isset_state(GetState(), USTATE_COMBIN_WIND_PET) )
		disMax = 11;
	if ( ((pSkill == NULL) || (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL)) && isset_state(GetState(), USTATE_COMBIN_FIRE_LEADER) )
		disMax = 11;
	if ( ((pSkill == NULL) || (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL)) && isset_state(GetState(), USTATE_COMBIN_SIX_GHOST) )
		disMax = 11;
	if ( ((pSkill == NULL) || (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL)) && isset_state(GetState(), USTATE_COMBIN_THREE_DIM) )
		disMax = 11;

	if(dis > disMax)
	{
		RETURN(eAttackNpcResult_TooFar);
	}
	
	ClearPath();

	bool bServerLogic = false;


	m_RunEvent.id = pNpc->GetID();
	m_RunEvent.target = (pNpc->IsCharacter() ? eTargetType_Character : eTargetType_Npc);
	
	if ( targetList.empty() )
		pCMD = malloc( sizeof(stAttackMagicUserCmd) );
	else
		pCMD = malloc( sizeof(stAttackMagicUserCmd) + targetList.size()*sizeof(DWORD) );
	stAttackMagicUserCmd* cmd = (stAttackMagicUserCmd*)pCMD;

	//Get target list
	int objNum=0;
	if ( !targetList.empty() )
	{
		std::vector<stPointI>::iterator itor;

		for( itor=targetList.begin();itor!=targetList.end();itor++ )
		{
			int x = (*itor).x + GetGridPos().x;
			int y = (*itor).y + GetGridPos().y;
			if ( (x>=0) && (x<GetScene()->GetWidth())
				&& (y>=0) && (y<GetScene()->GetHeight()) )
			{
				if ( GetScene()->IsGridHasAnyObject( stPointI(x,y), true ) )
				{
					CSceneObject* pObject = GetScene()->GetObjectAtGrid<CCharacter>(stPointI(x,y));
					if ( NULL == pObject )
						pObject = GetScene()->GetObjectAtGrid<CNpc>(stPointI(x,y));

					if ( pObject && !pObject->IsDie() )
						cmd->dwTempIDList[objNum++] = pObject->GetID();
				}
			}
		}
	}

#ifdef _DEBUG
	for( int i=0;i<objNum;i++ )
	{
		char strMsg[MAX_NAMESIZE];
		sprintf( strMsg, "SendServerID=%u", cmd->dwTempIDList[i] );
		GetGameGuiManager()->AddClientSystemMessage( strMsg, COLOR_ARGB(255,0,255,0) );
	}
	
#endif	
	cmd->byCmd = MAGIC_USERCMD;
	cmd->byParam = MAGIC_USERCMD_PARA;
	cmd->dwUserTempID = GetID();
	cmd->dwDefenceTempID = pNpc->GetID();
	cmd->byAttackType = (pNpc->IsCharacter() ? ATTACKTYPE_U2U : ATTACKTYPE_U2N);
	cmd->wdMagicType = pSkill->dwID;

	ptDst =pNpc->GetGridPos();
	cmd->xDes = ptDst.x;
	cmd->yDes = ptDst.y;
	cmd->byDirect = Scene_GetDir8(ptDst.x - m_ptGrid.x,ptDst.y - m_ptGrid.y);

	int iClientSkill = ServerSkillToClientSkill(pSkill);

	MagicProperty* p = Property_GetTableManager()->GetMagicProperty(iClientSkill,0);
	
	if(!p)
	{
		RETURN(eAttackNpcResult_Other);	
	}

	cmd->byAction = m_pCharacter->GetRandAni(p->stCharacter.eAnimation,p->stCharacter.eAnimation2,p->stCharacter.eAnimation3);
#if defined _DEBUG || defined RELEASE_TEST
	if ( Ani_Null == cmd->byAction )
		Engine_WriteLog( "技能没有相应动作\n" );
#endif
	if(IsRide())
	{
		if(IsAttackAni((AniTypeEnum)cmd->byAction))
			cmd->byAction = Ani_Ride_Attack;
		else if( IsMagicAni((AniTypeEnum)cmd->byAction) )
			cmd->byAction = Ani_Ride_Magic;
		else if(IsBowAni((AniTypeEnum)cmd->byAction))
			cmd->byAction = Ani_Ride_Bow;
		else
		{
			RETURN(eAttackNpcResult_AniError);
		}
	}
	
	if(!GetInterface()->IsExistAction((AniTypeEnum)cmd->byAction))
		RETURN(eAttackNpcResult_AniError);

	if (!m_pCharacter->CanChangeToAction((AniTypeEnum)cmd->byAction))
		RETURN(eAttackNpcResult_AniError);

	POINT ptObs;
	DWORD dwTimeAni = xtimeGetTime();
	
	//For combin state special treat
	if ( isset_state(GetState(), USTATE_COMBIN_GOD_FIGHTER) && (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL) )
		cmd->wdMagicType = SKILL_COMBIN_BOW_ARROW;
	if ( isset_state(GetState(), USTATE_EAT_GHOST) && ((NULL == pSkill) || (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL)) )
		cmd->wdMagicType = SKILL_EAT_GHOST_NORMAL_SKILL;
	if ( isset_state(GetState(), USTATE_COMBIN_XIAN_PET) && ((NULL == pSkill) || (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL)) )
		cmd->wdMagicType = SKILL_COMBIN_XIAN_PET_NORMAL_SKILL;
	if ( isset_state(GetState(), USTATE_COMBIN_WIND_PET) && ((NULL == pSkill) || (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL)) )
		cmd->wdMagicType = SKILL_COMBIN_WIND_PET_NORMAL_SKILL;
	if ( isset_state(GetState(), USTATE_COMBIN_FIRE_LEADER) && ((NULL == pSkill) || (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL)) )
		cmd->wdMagicType = SKILL_COMBIN_FIRE_LEADER_NORMAL_SKILL;
	if ( isset_state(GetState(), USTATE_COMBIN_SIX_GHOST) && ((NULL == pSkill) || (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL)) )
		cmd->wdMagicType = SKILL_COMBIN_SIX_GHOST_NORMAL_SKILL;
	if ( isset_state(GetState(), USTATE_COMBIN_THREE_DIM) && ((NULL == pSkill) || (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL)) )
		cmd->wdMagicType = SKILL_COMBIN_THREE_DIM_NORMAL_SKILL;
	//End special treat

	CNpc::enumCastMagicResult e = CNpc::CastMagic(cmd,ptObs,bServerLogic);
	if( e == eCastMagicResult_Succeeded || e == eCastMagicResult_Obstruct )
	{
		if ( isset_state(GetState(), USTATE_COMBIN_GOD_FIGHTER) && (pSkill->dwID == SERVER_SKILL_ATTACK_NORMAL) )
			SetUseSkillTime( GetSkillBase( SERVER_SKILL_BOW_NORMAL ) );
		else
			SetUseSkillTime(pSkill);

		if( e == eCastMagicResult_Obstruct )
		{
			cmd->dwDefenceTempID = 0;
			cmd->byAttackType = ATTACKTYPE_U2P;
			cmd->xDes = ptObs.x;
			cmd->yDes = ptObs.y;
		}
		
		//For combine state special treat
		//We send bow normal attack to server
		if ( cmd->wdMagicType == SKILL_COMBIN_BOW_ARROW )
			cmd->wdMagicType = SERVER_SKILL_BOW_NORMAL;
		if ( cmd->wdMagicType == SKILL_EAT_GHOST_NORMAL_SKILL )
			cmd->wdMagicType = SERVER_SKILL_BOW_NORMAL;
		if ( cmd->wdMagicType == SKILL_COMBIN_XIAN_PET_NORMAL_SKILL )
			cmd->wdMagicType = SERVER_SKILL_BOW_NORMAL;
		if ( cmd->wdMagicType == SKILL_COMBIN_WIND_PET_NORMAL_SKILL )
			cmd->wdMagicType = SERVER_SKILL_BOW_NORMAL;
		if ( cmd->wdMagicType == SKILL_COMBIN_FIRE_LEADER_NORMAL_SKILL )
			cmd->wdMagicType = SERVER_SKILL_BOW_NORMAL;
		if ( cmd->wdMagicType == SKILL_COMBIN_SIX_GHOST_NORMAL_SKILL )
			cmd->wdMagicType = SERVER_SKILL_BOW_NORMAL;
		if ( cmd->wdMagicType == SKILL_COMBIN_THREE_DIM_NORMAL_SKILL )
			cmd->wdMagicType = SERVER_SKILL_BOW_NORMAL;
		//End special treate

		//For flower npc we send server normal attack
		if ( cmd->wdMagicType == SKILL_ATTACK_FLOWER )
			cmd->wdMagicType = SERVER_SKILL_ATTACK_NORMAL;

		Client_SendCommand(cmd, sizeof(stAttackMagicUserCmd)+objNum*sizeof(DWORD),false)

		long lTime = xtimeGetTime()-dwTimeAni;
		m_pCharacter->UpdatePrevAniEndTime(lTime);

		m_nSendAttack++;
		m_timeSendAttack = xtimeGetTime();
		m_skillStartTime = xtimeGetTime();

		if(g_bSinglePlayer)
		{
			GetClient()->PushCommand( cmd, sizeof(stAttackMagicUserCmd)+objNum*sizeof(DWORD) );

			stRTMagicUserCmd rcmd;
			rcmd.byDirect = cmd->byDirect;
			rcmd.byLuck = 0;
			rcmd.byRetcode = (rand()%3) ? RTMAGIC_FAILURE : RTMAGIC_SUCCESS;
			rcmd.bySrc = MAPDATATYPE_USER;
			rcmd.byTarget = MAPDATATYPE_NPC;
			rcmd.dwSrcTempID = GetID();
			rcmd.dwUserTempID = pNpc->GetID();
			rcmd.wdHP = 10;
			GetClient()->PushCommand(&rcmd,sizeof(rcmd));
		}
		
		RETURN(eAttackNpcResult_Succeeded);
	}
	else
	{
		TRACE("攻击动画失败 %d\n",e);
	}

	if( e == eCastMagicResult_ActionError )
		RETURN(eAttackNpcResult_AniError);

	RETURN(eAttackNpcResult_Other);


_ret:
	free(pCMD);
	FUNCTION_END;
	return retval;
}

//#pragma optimize("",on)
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pCmd : 描述
 * \return 返回值的描述
 */
bool CMainCharacter::HandleCommand(stNullUserCmd* pCmd)
{
	FUNCTION_BEGIN;
	
	switch(pCmd->byCmdType)
	{
	case RELIVE_USERCMD:	// 复活指令
		switch( pCmd->byParameterType ) 
		{
		case MAINUSERDEATH_RELIVE_USERCMD_PARA:	// 主用户死亡指令
			{
				stMainUserDeathReliveUserCmd* p = (stMainUserDeathReliveUserCmd*)pCmd;
				TRACE("handle cmd death\n");
				m_bDieWaitingMagic = true;
				m_bDied = true;
				m_nDieWaitingTime = 1000;
				m_nDieDlgWaitingTime = 2000;
				m_bDieDlgWaiting = true;
				m_dwDeathType = p->deathType;
				Debug_Int(m_dwDeathType);
				return true;
			}
			break;
		}
		break;
	}
	return CCharacter::HandleCommand(pCmd);

	FUNCTION_END;
}

void CMainCharacter::AttackTeamLeaderAttack()
{
	FUNCTION_BEGIN;

	CGuiTeam* pTeam = GetGameGuiManager()->m_guiTeam;
	if ( pTeam && !pTeam->IsTeamHead( GetID() ) )
	{
		CNpc* pNpc = GetTeamInfo()->GetLeaderAttackNpc();
		if ( pNpc )
		{
			//Get team leader
			CCharacter* pLeader = GetScene()->FindCharacterByID( pTeam->m_dwHeadID );
			if ( NULL == pLeader )
				return;
			
			//If you are too far from team leader
			//Do nothing
			int dis = Scene_GetDis( GetGridPos(), pLeader->GetGridPos() );
			if ( dis > 6 )
				return;

			m_RunEvent.id		= pNpc->GetID();
			m_RunEvent.target	= pNpc->IsCharacter() ? eTargetType_Character : eTargetType_Npc;
			m_RunEvent.pt		= pNpc->GetPos();			
			m_RunEvent.pSkill	= m_pActiveSkill;
			m_RunEvent.speed	= GetMoveSpeed();
			m_RunEvent.event	= eEventType_Attack;

			SelectInterestNpc( pNpc );	
		}
	}

	FUNCTION_END;
}

void CMainCharacter::RunAutoAttackResourceBox()
{
	if ( !GetGameApplication()->bClientSetted( enumAuto_Kill_Summon ) )
		return;
	if ( IsCombinationState() )
		return;

	if( bIsShopStart() )
		return;

	CNpc* pNpc = GetScene()->FindNearbySummon( NpcTypeItem_Resource );	

	if ( NULL == pNpc )
	{
		//We walk somewhere else to try find again
		stPointI ptPos = GetGridPos();

		ptPos.x += RANDOM_OFFSET * SCREEN_GRID_WIDTH*2.0f;
		ptPos.y += RANDOM_OFFSET * SCREEN_GRID_HEIGHT*2.0f;

		if ( ptPos.x <= 0 )
			ptPos.x = 1;
		if ( ptPos.x >= GetScene()->GetWidth() )
			ptPos.x = GetScene()->GetWidth() - 1;
		if ( ptPos.y <= 0 )
			ptPos.y = 1;
		if ( ptPos.x >= GetScene()->GetHeight() )
			ptPos.y = GetScene()->GetHeight() -1;

		WalkTo( GetScene()->GridCenterToPixelPos( ptPos ), GetMoveSpeed() );		
		// if(timeGetTime() - autotime >= 10000) //随机传送 10秒
		// {
		// 	autotime = timeGetTime();
		// 	stSuiJiChuanSongUserCmd cmd1;
		// 	SEND_USER_CMD(cmd1);
			
		// }
	}
	else
	{
		m_RunEvent.id		= pNpc->GetID();
		m_RunEvent.target	= eTargetType_Npc;
		m_RunEvent.pt		= pNpc->GetPos();		
		m_RunEvent.pSkill	= NULL;
		m_RunEvent.speed	= GetMoveSpeed();
		m_RunEvent.event	= eEventType_Attack;
		SelectInterestNpc( pNpc );		
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param time_elasped : 描述
 * \return 返回值的描述
 */
static DWORD s_run ;
long Intertime = 0; //控制脚印加载频率
int igroup = -1;
void CMainCharacter::Run(long time_elasped)
{
	FUNCTION_BEGIN;

	//////////添加脚印////////////
	Intertime += time_elasped;
	if(m_pCharacter->IsMoving() && !m_pCharacter->IsReachDes() && Intertime >= 370 && !m_pCharacter->IsRide()
		&& !isset_state(this->GetState(), USTATE_SPA) && !isset_state(this->GetState(), USTATE_USER_FLY)
		&& !isset_state(this->GetState(), USTATE_USER_WABAO)&& !isset_state(this->GetState(), USTATE_USER_DANCE))
	{
		igroup++;//0表示左脚，1表示右脚
		GetFootprintManager().AddFootprint("data\\footprint.gl", igroup, m_pCharacter->GetDir(), m_pCharacter->GetPos(), false);
		Intertime = 0;
		if(igroup == 1)
		{
			igroup = -1;
		}
	}
	else if(m_pCharacter->IsMoving() && !m_pCharacter->IsReachDes() && Intertime >= 400 && m_pCharacter->IsRide())
	{

		igroup++;
		GetFootprintManager().AddFootprint("data\\footprint.gl", igroup+4, m_pCharacter->GetDir(), m_pCharacter->GetPos(), true);
		Intertime = 0;
		if(igroup == 1)
		{
			igroup = -1;
		}
	}

	//----------------------------
	// 自动加血
	// 
	//----------------------------
	if(gAutoGameConfig.autoAddHP)
	{
		DWORD minHP = 0;
		if(gAutoGameConfig.useMinHPPercent){
			minHP = gAutoGameConfig.minHPPercent * GetMaxHP();
		}else{
			minHP = gAutoGameConfig.minHP;
		}
		AutoAddHP(minHP);
	}
	//-----------------------------

	if(GetGameApplication()->bClientSetted( enumAuto_Kill_Summon ))
		RunAutoWork();
///////////////////////////////////////////////star100515
	//Random we pickup nearby item
	if ( GetGameGuiManager()->m_guiAutoAttackDlg->m_fBegin_State)
	{
		if(GetGameGuiManager()->m_guiAutoAttackDlg->m_fAuto_PicUp)
		{
			//PickUpNearbyItem( false );
			RunAutoPickup();////////star100810

		}
	}
///////////////////////////////////////////////end
////////star100604

	if( m_pCharacter->GetAction() > 1)
		GetGameGuiManager()->m_guiAutoAttackDlg->m_fAniToWait = false;
	else
		GetGameGuiManager()->m_guiAutoAttackDlg->m_fAniToWait = true;
	GetGameGuiManager()->m_guiAutoAttackDlg->m_iAni = m_pCharacter->GetAction();
////////end

	//This is a test
	if ( GetGuiManager()->GetDialogByActionType( eAction_Logout ) )
		int tmp = 100;
	//End test

	if ( m_bRushOut )
	{
		if ( xtimeGetTime()-m_rushOutStartTime > 2000 )
		{
			m_bRushOut = false;
			if ( m_RunEvent.event == eEventType_Attack )
			{
				if ( (m_RunEvent.pSkill->dwID == SKILL_RUSH_OUT)
					|| (m_RunEvent.pSkill->dwID == SKILL_RUSH_OUT_FAST) )
				{
					ClearRunEvent();
					if ( GetInterface() )
						GetInterface()->ClearKeepImage();
				}
			}
		}
	}

	AutoAddHpBySpecialEquip();

	//See if elapsed time > 2000
	//If true we change cursor state to eCursorFreeMove
	if ( !m_bLocalSpaceLButtonDown )
	{
		m_InputState.dwLButtonDownTimeFirst = xtimeGetTime();
	}

//#ifndef _AUTOPLAY
	//First time set free move state
	// ZH 100622 自动跟随时间
	if (  m_bLocalSpaceLButtonDown && ((xtimeGetTime() - m_InputState.dwLButtonDownTimeFirst) > 2000) )
	{
		if ( GetGameCursor()->IsNormal() && (GetGameCursor()->GetCursorImageType() == eCursorNormal) )		//
		{
			GetGameCursor()->SetCursor( CCursor::CursorType_Drag, eCursorFreeMove );
			m_bFreeMoveState = true;
		}
	}
//#endif
	if ( m_bDieDlgWaiting )
	{
		m_nDieDlgWaitingTime -= time_elasped;
		if (m_nDieDlgWaitingTime < 0)
		{
			m_nRecieveWalk = 0;
			m_nSendWalk = 0;
			m_nSendAttack = 0;
			m_nRecieveAttack = 0;

			if ( GetGameGuiManager()->m_guiShop )
			{
				GetGameGuiManager()->m_guiShop->Close();
			}
			
			// 重生选择对话框
			if ( IsDie() )		//In some case server relive you before you open this dlg
			{
				CGuiReliveDlg *p = GetGameGuiManager()->AddReliveDlg(m_dwID,m_dwDeathType);
				if(p == NULL)
				{
					stOKReliveUserCmd cmd;
					cmd.dwUserTempID = m_dwID;
					cmd.byType = ReliveHome;
					SEND_USER_CMD(cmd);
				}
			}
			else
			{
				if ( GetGameGuiManager()->m_guiReliveDlg )
					GetGameGuiManager()->m_guiReliveDlg->Close();
			}
			m_bDieDlgWaiting= false;			
		}
	}

	CCharacter::Run(time_elasped);
	
	RefreshEvent();
	if(m_RunEvent.event != eEventType_PickUp && gAutoGameConfig.autoPickupObject && GetGameApplication()->bClientSetted( enumAuto_Kill_Summon ))
	{
		// 2010 8
		if( GetGameGuiManager()->m_guiAutoAttackDlg->m_fBegin_State )
		{
			if(GetGameGuiManager()->m_guiAutoAttackDlg->m_fAuto_PicUp)
			{
				RunAutoPickup();
			}
		}
	}

	switch(m_RunEvent.event)
	{
	case eEventType_Move:
		RunMoveEvent();
		break;
	case eEventType_Attack:
		RunAttackEvent();
		break;
	case eEventType_PickUp:
		RunPickUpEvent();
		break;
	case eEventType_Follow:
		RunFollowEvent();
		break;
	case eEventType_None:
		{
			if ( !IsDie() )
			{
				if ( isset_state(GetState(), USTATE_CRAZY) )
					RunAutoAttack( eAutoAttack_Character );
				else
				{
					if ( isset_state(GetState(), USTATE_TEAM_ATTACK_ONE_DIM) )
					{
						AttackTeamLeaderAttack();
					}
					else
					{
						if ( !GetScene()->IsDarkScene() )		//Map of this type is forbid auto attack
						{
							if ( !IsEquipedPickResourceTool() )//改成求反
								RunAutoAttackResourceBox();
							else
								RunAutoAttack();
						}
					}
				}
			}    						
		}
		break;
	}
	WalkPath();

	if(m_pCharacter->IsMoving() && !m_pCharacter->IsMovingBreak() && !GetGameCursor()->IsLButtonDown() && GetGameApplication()->GetConfig()->bSystemSetted(USER_SETTING_AUTOFINDPATH))
	{
		if ( m_bFirstTimeChangePath )
		{
			m_moveTarget.Play();
			m_bFirstTimeChangePath = false;
		}
		GetScene()->AddDrawObjects(&m_moveTarget);	
	}

	if (!isset_state(this->GetState(), USTATE_SPA))
	{
		GetTimerDisplayMgr().RemoveTimer(TIMERTYPE_SPA);
	}

	FUNCTION_END;
}

void CMainCharacter::SetMainRoleShopAdv(const char* pszAdv)
{
	m_strMainRoleShopAdv = pszAdv;
}
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ptMapPixelOffset : 描述
 * \param dwColor : 描述
 * \return 返回值的描述
 */
void CMainCharacter::Draw(POINT ptMapPixelOffset,DWORD dwColor)
{
	FUNCTION_BEGIN;
	
	if( bIsShopStart() && m_strMainRoleShopAdv.length() > 0 )
	{
		static DWORD dwTime = xtimeGetTime();

		// 2010 9
		if ( xtimeGetTime() - dwTime > 30000 )
		{
			stChannelChatUserCmd cmd;
			cmd.dwType = CHAT_TYPE_SHOPADV;
			strcpy(cmd.pstrName,GetName());
			strcpy(cmd.pstrChat,m_strMainRoleShopAdv.c_str());
			cmd.size = 0;
			cmd.dwChatTime = xtimeGetTime();
			cmd.dwFromID = GetID();
			SEND_USER_CMD( cmd);

			dwTime = xtimeGetTime();
		}
	}
	
	CCharacter::Draw(ptMapPixelOffset, dwColor);	

	FUNCTION_END;
}

void CMainCharacter::GetScene9ScreenID(int ScreenID[9])
{
	FUNCTION_BEGIN;

	stPointI ptScreen(m_ptGrid.x/SCREEN_GRID_WIDTH - 1,m_ptGrid.y/SCREEN_GRID_HEIGHT - 1);

	// 重新计算人物周围9屏的范围
	stPointI pt;
	pt.y = ptScreen.y;
	int index = 0;
	for( int i=0;i<3;++i )
	{
		pt.x = ptScreen.x;
		for( int j=0;j<3;++j,++index )
		{
			if(pt.x < 0 || pt.x >= GetScene()->GetScreenWidth() || pt.y < 0 || pt.y >= GetScene()->GetScreenHeight())
				ScreenID[index] = -1;
			else
				ScreenID[index] = pt.y * GetScene()->GetScreenWidth() + pt.x;
			pt.x++;
		}
		pt.y++;
	}

	FUNCTION_END;
}

void CMainCharacter::OnDayNumChange()
{
#ifdef WEATHER_EFFECT
	if ( Engine_GetAppation()->GetConfigBase()->bWeather )
	{
		int zoneID = GetScene()->GetZoneID( GetGridPos() );
		int time = GetGameTime()->GetDayNum();
		//Get cur weather info
		//醉梦注释天气
		// CGameScene::stZoneWetherInfo* weather = GetScene()->GetMapZoneWetherInfo( zoneID, time );
		// if ( weather )
		// 	GetWeatherMgr()->ChangeWeather( weather->type, weather->level, weather->szBackMusic );		//
		// else
		// 	GetWeatherMgr()->ChangeWeather( WETHER_T_NUM, -1, "" );
	}	
#endif
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param prevGrid : 描述
 * \param ptCurGrid : 描述
 * \return 返回值的描述
 */
void CMainCharacter::OnGridPosChange(POINT prevGrid,POINT ptCurGrid)
{
	FUNCTION_BEGIN_NEW;
	CCharacter::OnGridPosChange(prevGrid,ptCurGrid);

	//对于特殊卷轴类物品，如果人物走到物品上面，则认为捡起它(发送pickup指令，服务器作特殊处理)
	CItem* pItem = GetScene()->GetObjectAtGrid<CItem>( ptCurGrid );
	if( NULL != pItem )
	{
		DWORD dwId = pItem->GetBaseID(); 
		if( (dwId == c_nHuoYunFu) ||
			(dwId == c_nXianTianFu) ||
			(dwId == c_nXuanYuanFu) )
		{
			PickUpItem(pItem);
		}	
	}

	//第一次触发功能npc事件
	if(bIsUserUnSetted(USER_SETTING_HELP_NPCDIALOG))
	{
		if(GetScene()->IsFunctionalNpcNear())
			ShowHelpTip(USER_SETTING_HELP_NPCDIALOG);
	}

	//第一次触发alt自动选择npc
	if( bIsUserUnSetted(USER_SETTING_HELP_ALT_SELECT_NPC) )
	{
		if(GetScene()->IsBugbearNpcNear())
		{
			ShowHelpTip(USER_SETTING_HELP_ALT_SELECT_NPC);
		}
	}

	//第一次触发怪物npc事件
	if(bIsUserUnSetted(USER_SETTING_HELP_ATTACK))
	{
		if(GetScene()->IsBugbearNpcNear())
			ShowHelpTip(USER_SETTING_HELP_ATTACK);
	}

	//OnTipHelp(USER_SETTING_HELP_ATTACK,GetScene()->IsBugbearNpcNear());

	if(prevGrid.x != -1 && prevGrid.y != -1 )
	{
		CNpc* pNpc = GetScene()->FindNpcByID(m_dwNpcIdByLast);
		if( !pNpc || Scene_GetDis(ptCurGrid,pNpc->GetGridPos()) > 1 )
		{
			if(GetGameGuiManager()->m_guiNpcDialog && GetGameGuiManager()->m_guiNpcDialog->IsMoveCanClose())
				GetGameGuiManager()->EndNpcDialog();
			if(GetGameGuiManager()->m_guiSavingBoxDlg && GetGameGuiManager()->m_guiSavingBoxDlg->IsVisible()) 
			{
				GetGameGuiManager()->m_guiSavingBoxDlg->SetVisible(false);
				if(GetGameGuiManager()->m_guiItem)
				{
					GetGameGuiManager()->m_guiItem->SetVisible(false);
				}
			}
			if( GetGameGuiManager()->m_guiOthersShop )
			{
				GetGameGuiManager()->m_guiOthersShop->Close();
			}
		}
	}

	//醉梦注释 修复报错 尝试 2025年1月25日23:18:44
	// GetScene()->LoadNearbyImage(prevGrid,ptCurGrid);
	GetScene()->InitNearbyConver(prevGrid,ptCurGrid);

	if(GetScene()->IsAreaNameChange(prevGrid,ptCurGrid)) 
	{
		//图块名称发生变化,showText on screen
		const char *pszName = GetScene()->GetAreaName(ptCurGrid);
		if(pszName)
		{
			if( GetGameGuiManager()->m_guiMain )
			{
				GetGameGuiManager()->m_guiMain->AddMessage(pszName,CGuiMain::ZoneMessage,COLOR_ARGB(255,0,200,200));
			}
		}

#ifdef WEATHER_EFFECT
		if ( Engine_GetAppation()->GetConfigBase()->bWeather )
		{
			int zoneID = GetScene()->GetZoneID( ptCurGrid );
			int time = GetGameTime()->GetDayNum();
			//Get cur weather info
			// CGameScene::stZoneWetherInfo* weather = GetScene()->GetMapZoneWetherInfo( zoneID, time );
			// if ( weather )
			// 	GetWeatherMgr()->ChangeWeather( weather->type, weather->level, weather->szBackMusic );
			// else
			// 	GetWeatherMgr()->ChangeWeather( WETHER_T_NUM, -1, "" );
			//醉梦注释天气
		}		
#endif
	}

	if(!CCountry::Instance()->IsCountryWarState())
	{
		if(GetScene()->IsAreaBackMusicChange(prevGrid,ptCurGrid)) 
		{
			CGameScene::stBackMusic * pPreBackMusic = GetScene()->GetAreaBackMusic(prevGrid);
			CGameScene::stBackMusic * pBackMusic = GetScene()->GetAreaBackMusic(ptCurGrid);
			//背景音乐发生变化，播放背景音乐

			if(!pBackMusic)	
			{
				pBackMusic  = GetScene()->GetBackMusic();
			}

			if(pBackMusic && pBackMusic->szBackMusic[0])
			{
				PlayBackMusic(pBackMusic->szBackMusic,c_dwMusicFadeTime,pBackMusic->iMinInternal,pBackMusic->iMaxInternal);
			}
			else
			{
				PlayBackMusic("",c_dwMusicFadeTime*3,pBackMusic->iMinInternal,pBackMusic->iMaxInternal,true);
			}
		}
	}
	
	FUNCTION_END_NEW;
}
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMainCharacter::AdjustAxis()
{
	FUNCTION_BEGIN;

	SetFrameAllocator fa;
	std::vector<int,std::stack_allocator<int> > vecScreen;
	vecScreen.reserve(9);

	int	iScreenID = GetScene()->GridPosToScreenID(m_ptGrid);


	if ( iScreenID == m_dwScreenID )
		return;

	int ScreenID[9];
	int i;

	GetScene9ScreenID(ScreenID);

	// 计算需要抛弃的屏幕内容
	for( i = 0; i < 9; i ++ )
	{
		if ( GetScene()->m_aScreenID[i] == -1 )
			continue; 

		if ( !IsHaveScreenID(ScreenID,GetScene()->m_aScreenID[i]) )
		{
			GetScene()->RemoveScreenData( GetScene()->m_aScreenID[i] );
			GetScene()->m_aScreenID[i] = -1;
		}
	}

	// 计算需要请求的屏幕编号
	for( i = 0; i < 9; i ++ )
	{
		if (ScreenID[i] == -1)
			continue;
		if ( !GetScene()->IsHaveScreenID(ScreenID[i]) )	
		{
			vecScreen.push_back(ScreenID[i]);
		}
	}

	GetScene()->RequestScreenData( &vecScreen[0],vecScreen.size() );

	m_dwScreenID	=	iScreenID;

	memcpy(&GetScene()->m_aScreenID,ScreenID,sizeof(ScreenID));

	GetScene()->Update9ScreenRect();

	FUNCTION_END;
}

void CMainCharacter::AttackBackSummon( CNpc* pNpc )
{
	FUNCTION_BEGIN;

	return;		//We not yet open this function

	if ( pNpc && (NULL == m_pActiveNpc)
		 && (m_RunEvent.event != eEventType_Attack) )
	{
		m_RunEvent.id = pNpc->GetID();
		m_RunEvent.target = (pNpc->IsCharacter() ? eTargetType_Character : eTargetType_Npc);

		m_RunEvent.pt = pNpc->GetPos();
		m_RunEvent.pSkill = m_pActiveSkill;

		m_RunEvent.speed = GetMoveSpeed();
		m_RunEvent.event = eEventType_Attack;

		SelectInterestNpc( pNpc );
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nMsg : 描述
 * \return 返回值的描述
 */
bool CMainCharacter::OnInputEvent( UINT nMsg, CSceneObject* pObject /* = NULL */ )
{
	bool retval;
	FUNCTION_BEGIN;

	retval = false;
	//When we use RushOut skill we can't do anything else
	//This will last for 2 seconds at most
	if ( m_bRushOut )
		RETURN( true );
	if ( isset_state(GetState(),USTATE_CRAZY_ACTION)
		|| isset_state(GetState(),USTATE_CRAZY) )
		RETURN( true );

	switch(nMsg)         //[sky]鼠标左右键点击消息处理
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		if(m_InputState.bLButtonDown || m_InputState.bRButtonDown)
		{
			if( m_InputState.bRButtonDown && (NULL == pObject) )		//Right click on empty
			{
				//Turn off interest npc dlg
				if ( GetGameGuiManager()->m_guiCharacterProperty && GetGameGuiManager()->m_guiCharacterProperty->IsVisible() )
				{
					if ( GetGuiManager()->GetMouseOverDlg() != GetGameGuiManager()->m_guiCharacterProperty )
					{
						GetGameGuiManager()->m_guiCharacterProperty->SetVisible( false );
						GetScene()->UnSelectInterestNpc();
					}
				}
				
				//Turn off cart menu
				if ( GetGameGuiManager()->m_guiMain )
					GetGameGuiManager()->m_guiMain->CloseCartMenu();

				//Turn off pk mode list
				if ( NULL == pObject )
				{
					if ( GetGameGuiManager()->m_guiMain )
						GetGameGuiManager()->m_guiMain->TurnOffPkModeList();
				}

				//Turn off chat type btn list
				if ( GetGameGuiManager()->m_guiChatButton )
					GetGameGuiManager()->m_guiChatButton->TurnOffChatTypeBtnList();
			}

			if(pObject && pObject->IsSceneObjectType(OBJECT_ITEM))
			{
				//on some state can not see anything
				if ( isset_state( GetState(), USTATE_BLIND )
					|| IsServerSitDown() )
				{
					RETURN( true );
				}

				CItem* pItem = static_cast<CItem*>(pObject);
				TryPickUpItem( pItem );
				
				RETURN( true );
			}
			else if( pObject && pObject != this
					&& pObject->IsSceneObjectType(OBJECT_NPC) &&  !pObject->IsDie() && !( (static_cast<CNpc*>(pObject))->IsSurfaceType() ) )
			{
				if ( IsSitDown() || IsServerSitDown() )
				{
					OnSitDown();
					RETURN( true );
				}

				CNpc* pNpc = static_cast<CNpc*>(pObject);
				if(pNpc)
				{	
					m_RunEvent.id = pNpc->GetID();
					m_RunEvent.target = (pNpc->IsCharacter() ? eTargetType_Character : eTargetType_Npc);

					m_RunEvent.pt = GetScene()->ScreenPixelPosToPixelPos(m_InputState.ptMouse);
					if( m_InputState.bRButtonDown && pNpc->IsNpcType(NPC_TYPE_ATTACK))
						m_RunEvent.pSkill = m_pActiveSkill;
					else
						m_RunEvent.pSkill = NULL;

					m_RunEvent.speed = GetMoveSpeed();
					m_RunEvent.event = eEventType_Attack;

					//Turn on NPC property dlg
					if ( pNpc->IsNpcType( NPC_TYPE_ATTACK )
						|| ( pNpc->GetNpcTypeItem() == NpcTypeItem_Trade)
						|| ( pNpc->GetNpcTypeItem() == NpcTypeItem_Task)
						|| ( pNpc->GetNpcTypeItem() == NpcTypeItem_MailMan)
						|| ( pNpc->GetNpcTypeItem() == NpcTypeItem_Auction)
						|| ( pNpc->GetNpcTypeItem() == NpcTypeItem_Enlisted)
						|| ( pNpc->GetNpcTypeItem() == NpcTypeItem_Cartoon)
						|| ( pNpc->GetNpcTypeItem() == NpcTypeItem_SavingBox))
					{
						SelectInterestNpc( pNpc );					
					}
				}		
			}
			else if( m_InputState.bRButtonDown && pObject == NULL && (m_pActiveSkill == NULL || m_pActiveSkill->dwType != SkillType_Active) )
			{
				// 允许技能空放
				if ( IsSitDown() || IsServerSitDown() )
				{
					/*stSitDownMoveUserCmd cmd;
					SEND_USER_CMD( cmd );*/
					OnSitDown();
					RETURN( true );
				}

				m_RunEvent.id = -1;
				m_RunEvent.target = eTargetType_None;
				m_RunEvent.pt = GetScene()->ScreenPixelPosToPixelPos(m_InputState.ptMouse);
				m_RunEvent.pSkill = m_pActiveSkill;

				m_RunEvent.speed = GetMoveSpeed();
				m_RunEvent.event = eEventType_Attack;
				
			}
			else if(m_InputState.bLButtonDown)
			{
				//触发"移动"教程事件
				OnTipHelp(USER_SETTING_HELP_MOVE);

				//触发"自动寻路"教程事件
				OnTipHelp(USER_SETTING_HELP_AUTO_FIND_PATH);

				if( bIsShopStart() )
				{
					GetGameGuiManager()->AddShop();
				}

				if ( isset_state(GetState(),USTATE_CRAZY_ACTION)
					|| isset_state(GetState(),USTATE_CRAZY) )
					RETURN( true );

				if ( IsSitDown() || IsServerSitDown() )
				{
					OnSitDown();
					RETURN( true );
				}	

				m_RunEvent.target = eTargetType_None;
				m_RunEvent.pt = GetScene()->ScreenPixelPosToPixelPos(m_InputState.ptMouse);
				m_RunEvent.event = eEventType_Move;
				m_bFirstTimeChangePath = true;
				if(Engine_GetState()->IsCtrlDown())
					m_RunEvent.speed = 0;
				else
					m_RunEvent.speed = GetMoveSpeed();

				//Turn off cart menu
				if ( GetGameGuiManager()->m_guiMain )
					GetGameGuiManager()->m_guiMain->CloseCartMenu();

				//Turn off pk mode list
				if ( NULL == pObject )
				{
					if ( GetGameGuiManager()->m_guiMain )
						GetGameGuiManager()->m_guiMain->TurnOffPkModeList();
				}

				//Turn off chat type btn list
				if ( GetGameGuiManager()->m_guiChatButton )
					GetGameGuiManager()->m_guiChatButton->TurnOffChatTypeBtnList();

				//Shut down progress bar
				if (GetGameGuiManager()->m_pDlgProgressBar)
				{
					GetGameGuiManager()->m_pDlgProgressBar->ShutDown();
				}

				if (GetGameGuiManager()->m_pDlg2ProgressBar)
				{
					GetGameGuiManager()->m_pDlg2ProgressBar->ShutDown();
				}
				
				RETURN( true );
			}			
		}
		break;
	}

_ret:
	;
	FUNCTION_END;
	return  retval;
}

void CMainCharacter::SetPKMode( BYTE pkMode )
{
	m_PkMode = pkMode;
}
////////////////////////star100515
int CMainCharacter::OnUseItem_forAutoAttack(WORD dID)
{
	if ( isset_state( GetState(), USTATE_LOSTSPRITE )
		|| isset_state( GetState(), USTATE_ROCK )
		|| isset_state( GetState(), USTATE_ICE_DEFENCE ) )
	{
		return 1;	
	}
	DWORD dwID = 0;
	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
	{
		CRoleItem* pItem = (*it);
		if( pItem->GetObjectID() == dID ) 
			dwID = pItem->GetThisID();
	}
	if(dwID == 0) return 1;
	CRoleItem* pItem =  FindItemByID( dwID );
	if ( NULL != pItem )
	{
		int count = GetItemCount( pItem->GetObjectID() );
		if ( count > 0 )
		{
			//Call duty item
			if ( pItem->IsCallDutyItem() < CALL_DUTY_NUM )
			{
				if ( !GetScene()->IsCallDutyCanUse() )
					return 1;					
			}
			//
			if( !pItem->CanUseItemNow())
				return 2;
			stUseObjectPropertyUserCmd cmd;
			cmd.qwThisID = dwID;
			cmd.dwNumber = 1;
			SEND_USER_CMD( cmd );
			if ( (pItem->GetItemType() == ItemType_Leechdom) 
				|| (pItem->GetItemType() == ItemType_Food) 
				|| (pItem->GetItemType() == ItemType_Tonic))
			{
				if ( pItem->GetObjectBase() )
				{
					PlayItemSound( pItem->GetObjectBase()->attackSound );
				}
			}
			return 0;
		}
	}
	return 1;
}
////////////////////////end
#if defined USE_DEFENCE
#define MainCharacter_OnUserItem	if( GetGameGuiManager()->m_guiShop  &&\
	GetGameGuiManager()->m_guiShop->IsVisible() )\
{\
	GetGameGuiManager()->AddClientSystemMessage("摆摊界面打开时不能使用物品");\
	return false;\
}\
	if ( isset_state( GetState(), USTATE_LOSTSPRITE )\
	|| isset_state( GetState(), USTATE_ROCK )\
	|| isset_state( GetState(), USTATE_ICE_DEFENCE ) )\
{\
	return false;	\
}\
	CRoleItem* pItem =  FindItemByID( dwID );\
	if ( NULL != pItem )\
{\
	int count = GetItemCount( pItem->GetObjectID() );\
	if ( count > 0 )\
{\
	if( !pItem->CanUseItemNow())\
	return false;\
	stUseObjectPropertyUserCmd cmd;\
	cmd.qwThisID = dwID;\
	cmd.dwNumber = 1;\
	SEND_USER_CMD( cmd );\
	if ( (pItem->GetItemType() == ItemType_Leechdom) \
	|| (pItem->GetItemType() == ItemType_Food) \
	|| (pItem->GetItemType() == ItemType_Tonic)) \
{\
	if ( pItem->GetObjectBase() )\
{\
	PlayItemSound( pItem->GetObjectBase()->attackSound );\
}\
}\
	return true;\
}\
}\
	return false;

M_FUNCTION_DEFINE1_R_8(CMainCharacter,OnUseItem,DWORD,dwID,bool,false,MainCharacter_OnUserItem)
#else
bool CMainCharacter::OnUseItem(DWORD dwID)
{
	if( GetGameGuiManager()->m_guiShop  &&
		GetGameGuiManager()->m_guiShop->IsVisible() )
	{
		GetGameGuiManager()->AddClientSystemMessage("摆摊界面打开时不能使用物品");
		return false;
	}
	if ( isset_state( GetState(), USTATE_LOSTSPRITE )
		|| isset_state( GetState(), USTATE_ROCK )
		|| isset_state( GetState(), USTATE_ICE_DEFENCE ) )
	{
		return false;	
	}

	CRoleItem* pItem =  FindItemByID( dwID );
	if ( NULL != pItem )
	{
		int count = GetItemCount( pItem->GetObjectID() );
		if ( count > 0 )
		{
			//Call duty item
			if ( pItem->IsCallDutyItem() < CALL_DUTY_NUM )
			{
				if ( !GetScene()->IsCallDutyCanUse() )
					return false;					
			}
			//
			if( !pItem->CanUseItemNow())
				return false;
			stUseObjectPropertyUserCmd cmd;
			cmd.qwThisID = dwID;
			cmd.dwNumber = 1;
			SEND_USER_CMD( cmd );
			if ( (pItem->GetItemType() == ItemType_Leechdom) 
				|| (pItem->GetItemType() == ItemType_Food) 
				|| (pItem->GetItemType() == ItemType_Tonic))
			{
				if ( pItem->GetObjectBase() )
				{
					PlayItemSound( pItem->GetObjectBase()->attackSound );
				}
			}
			return true;
		}
	}
	return false;
}
#endif

void CMainCharacter::SetRunEvent( const stRunEvent& event )
{
	m_RunEvent = event;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwID : 描述
 * \return 返回值的描述
 */
bool CMainCharacter::OnSelectSkill(DWORD dwID)
{
	FUNCTION_BEGIN;

	SkillBase_t* pSelectSkill = GetSkillBase(dwID);

	if ( NULL == pSelectSkill )
		return false;

	if ( pSelectSkill->dwType == SkillType_Passive )
		return false;

	if ( GetScene()->m_pInterestNpc
		&& !GetScene()->m_pInterestNpc->IsNpcType(NPC_TYPE_FUNCTION)
		&& GetScene()->IsAnyAliveInterestNpc()
		&& (pSelectSkill->dwType == SkillType_StateAttack) )
	{
		CNpc* pNpc = GetScene()->m_pInterestNpc;

		if ( pSelectSkill->dwType == SkillType_StateAttack )
		{
			//This time we add a new kind of skill-use-method
			if ( pSelectSkill->dwTarget == SkillTarget_Self )
			{
				m_RunEvent.id = GetID();
				m_RunEvent.target = eTargetType_Character;
				m_RunEvent.pSkill = pSelectSkill;
				m_RunEvent.pt = GetPos();
				m_RunEvent.speed = GetMoveSpeed();
				m_RunEvent.event = eEventType_Attack;
				return true;
			}
			
			if ( pSelectSkill->dwTarget & SkillTarget_Friend )
			{
				//Find team member
				if ( pNpc->IsCharacter() && GetGameGuiManager()->m_guiTeam )
				{
					CCharacter* pChar = GetGameGuiManager()->m_guiTeam->FindCharacterByName( pNpc->GetName(), pNpc->GetID() );
					if ( pChar )
					{
						m_RunEvent.id = pChar->GetID();
						m_RunEvent.target = eTargetType_Character;
						m_RunEvent.pSkill = pSelectSkill;
						m_RunEvent.pt = pChar->GetPos();
						m_RunEvent.speed = GetMoveSpeed();
						m_RunEvent.event = eEventType_Attack;
						return true;
					}
					else
					{
						if ( pSelectSkill->dwTarget == SkillTarget_Friend )
							return false;
					}
				}				
			}

			if ( pSelectSkill->dwTarget & SkillTarget_Pet )
			{
				if ( pNpc->IsPet() && (pNpc->GetPetType() == PET_TYPE_PET) )
				{
					m_RunEvent.id = pNpc->GetID();
					m_RunEvent.target = eTargetType_Npc;
					m_RunEvent.pSkill = pSelectSkill;
					m_RunEvent.pt = pNpc->GetPos();
					m_RunEvent.speed = GetMoveSpeed();
					m_RunEvent.event = eEventType_Attack;
					return true;
				}
				else
				{
					if ( pSelectSkill->dwTarget == SkillTarget_Pet )
						return false;
				}
			}

			if ( pSelectSkill->dwTarget & SkillTarget_Summon )
			{
				if ( pNpc->IsPet() && (pNpc->GetPetType() == PET_TYPE_SUMMON) )
				{
					m_RunEvent.id = pNpc->GetID();
					m_RunEvent.target = eTargetType_Npc;
					m_RunEvent.pSkill = pSelectSkill;
					m_RunEvent.pt = pNpc->GetPos();
					m_RunEvent.speed = GetMoveSpeed();
					m_RunEvent.event = eEventType_Attack;
					return true;
				}
				else
				{
					if ( pSelectSkill->dwTarget == SkillTarget_Summon )
					{
						//If pSelectSkill = SKILL_STRENGTHEN_SUMMON
						if ( pSelectSkill->dwID == SKILL_STRENGTHEN_SUMMON )
						{
							//   My pet of type summon
							CPet* pPet = GetScene()->FindPetByType( PET_TYPE_SUMMON );						

							if ( pPet )
							{
								CNpc* pMySummon = pPet->GetNpc();
								if ( pMySummon )
								{
									m_RunEvent.id = pMySummon->GetID();
									m_RunEvent.target = eTargetType_Npc;
									m_RunEvent.pSkill = pSelectSkill;
									m_RunEvent.pt = pMySummon->GetPos();
									m_RunEvent.speed = GetMoveSpeed();
									m_RunEvent.event = eEventType_Attack;
									return true;
								}
							}
							else
								GetGameGuiManager()->AddClientSystemMessage( "无有效攻击目标！" );
						}
						else
							GetGameGuiManager()->AddClientSystemMessage( "无有效攻击目标！" );
						return false;
					}
				}
			}

			if ( pSelectSkill->dwTarget & SkillTarget_Self )
			{
				m_RunEvent.id = GetID();
				m_RunEvent.target = eTargetType_Character;
				m_RunEvent.pSkill = pSelectSkill;
				m_RunEvent.pt = GetPos();
				m_RunEvent.speed = GetMoveSpeed();
				m_RunEvent.event = eEventType_Attack;
				return true;
			}
	
			if ( pSelectSkill->dwTarget & SkillTarget_Enemy )
			{
				m_RunEvent.id = GetScene()->m_pInterestNpc->GetID();
				m_RunEvent.target = GetScene()->m_pInterestNpc->IsCharacter() ? eTargetType_Character : eTargetType_Npc;
				m_RunEvent.pSkill = pSelectSkill;
				m_RunEvent.pt = GetScene()->m_pInterestNpc->GetPos();
				m_RunEvent.speed = GetMoveSpeed();
				m_RunEvent.event = eEventType_Attack;
			}
		}		
	}
	else
	{
		if( pSelectSkill && (pSelectSkill->dwTarget & SkillTarget_Self)
			&& ( !GetScene()->IsAnyAliveInterestNpc() || (pSelectSkill->dwTarget == SkillTarget_Self) ) )
		{
			m_RunEvent.id = GetID();
			m_RunEvent.target = eTargetType_Character;
			m_RunEvent.pSkill = pSelectSkill;
			m_RunEvent.pt = GetPos();
			m_RunEvent.speed = GetMoveSpeed();
			m_RunEvent.event = eEventType_Attack;
			return true;
		}

		if ( pSelectSkill && (pSelectSkill->dwTarget == SkillTarget_Single) )
		{
			if ( m_pActiveNpc && (m_pActiveNpc != this) )
			{
				m_RunEvent.id = m_pActiveNpc->GetID();
				m_RunEvent.target = m_pActiveNpc->IsCharacter() ? eTargetType_Character : eTargetType_Npc;
				m_RunEvent.pSkill = pSelectSkill;
				m_RunEvent.pt = m_pActiveNpc->GetPos();
				m_RunEvent.speed = GetMoveSpeed();
				m_RunEvent.event = eEventType_Attack;
			}
			else
				GetGameGuiManager()->AddClientSystemMessage( "无有效攻击目标！" );

			return true;
		}


		if( pSelectSkill && 
			pSelectSkill->dwType != SkillType_Attack && pSelectSkill->dwType != SkillType_Active )
		{
			m_RunEvent.pt = GetScene()->ScreenPixelPosToPixelPos(m_InputState.ptMouse);
			m_RunEvent.id = -1;
			m_RunEvent.target = eTargetType_None;
			if( GetScene()->m_pMouseOverObject && GetScene()->m_pMouseOverObject != this
				&& GetScene()->m_pMouseOverObject->IsSceneObjectType(OBJECT_NPC)  && !GetScene()->m_pMouseOverObject->IsDie())
			{
				m_RunEvent.id = GetScene()->m_pMouseOverObject->GetID();
				m_RunEvent.target = GetScene()->m_pMouseOverObject->IsSceneObjectType(OBJECT_CHARACTER) ? eTargetType_Character : eTargetType_Npc;
			}

			if ( GetScene()->m_pInterestNpc
				&& !GetScene()->m_pInterestNpc->IsDie()
				&& GetScene()->m_pInterestNpc->IsNpcType(NPC_TYPE_FUNCTION)
				&& !((pSelectSkill->dwTarget & SkillTarget_Pet) || (pSelectSkill->dwTarget & SkillTarget_Summon) || (pSelectSkill->dwTarget & SkillTarget_Self)) )
				return false;

			m_RunEvent.pSkill = pSelectSkill;
			m_RunEvent.speed = GetMoveSpeed();
			m_RunEvent.event = eEventType_Attack;
			return true;
		}		
	}

	if ( (m_pActiveSkill != pSelectSkill) && ( IsActiveSkill( pSelectSkill ) || (pSelectSkill->dwTarget == SkillTarget_Point) || (pSelectSkill->dwType == SkillType_Attack) ) )
	{
		//if ( pSelectSkill->dwType == SkillType_Attack )
		if ( !( (pSelectSkill->dwType == SkillType_Attack) && (pSelectSkill->dwDis == 1) ) ) 
			m_pActiveSkill = pSelectSkill;

		if ( GetGameGuiManager()->m_guiSkill )
			GetGameGuiManager()->m_guiSkill->RefreshState();

		char szMsg[256];
		if ( (m_pActiveSkill != NULL) && (m_pActiveSkill == pSelectSkill) )
		{
			sprintf(szMsg,"你选择 %s 作为当前技能。",m_pActiveSkill->pstrName);	
			GetGameGuiManager()->AddClientSystemMessage(szMsg);
		}
	}

	/////////////取消该技能作为当前技能//////////////////////////
	/* else if ( m_pActiveSkill == pSelectSkill && m_pActiveSkill != NULL)
	{
		 
		m_pActiveSkill = NULL;

		if ( GetGameGuiManager()->m_guiSkill )
			GetGameGuiManager()->m_guiSkill->RefreshState();

		char szMsg[256];
		sprintf(szMsg,"你放弃 %s 作为当前技能。",pSelectSkill->pstrName);
		GetGameGuiManager()->AddClientSystemMessage(szMsg);
	} */
	///////////////////////////////////////////////////////////
	//If has interest npc we try attack
	if ( GetScene()->m_pInterestNpc && !GetScene()->m_pInterestNpc->IsNpcType(NPC_TYPE_FUNCTION) )
	{
		m_RunEvent.id = GetScene()->m_pInterestNpc->GetID();
		m_RunEvent.target = GetScene()->m_pInterestNpc->IsCharacter() ? eTargetType_Character : eTargetType_Npc;
		m_RunEvent.pSkill = pSelectSkill;
		m_RunEvent.pt = GetScene()->m_pInterestNpc->GetPos();
		m_RunEvent.speed = GetMoveSpeed();
		m_RunEvent.event = eEventType_Attack;
	}

	return true;

	FUNCTION_END;
}

bool CMainCharacter::HandleLButtonDownEventOnSceneObject( CSceneObject* pObject, bool bNeedShift /* = false */ )
{
	if ( NULL == pObject )
		pObject = GetScene()->m_pMouseOverObject;

	switch(GetGameCursor()->GetCursorImageType())
	{
	case eCursorSetAccess:	// 鼠标上有技能图标
		//Restore mouse icon as default
		if ( GetGameGuiManager()->m_guiSkill )
		{
			GetGameGuiManager()->m_guiSkill->SetActiveSkillIcon();
			RestoreMouseIcon();
		}
		return true;
		break;
	case eCursorEditAccess:	// Edit access state
		//delete select access key(包含情感图标拖动就丢弃处理)
		{
			GetGameAccelKeyManager()->DeleteCurEditAccess();
			RestoreMouseIcon();
		}
		return true;
		break;
	case eCursorTrade:		// 在请求交易状态
		{
			// 在请求交易状态
			CCharacter* pChar = NULL;
			if ( pObject && pObject->IsSceneObjectType( OBJECT_CHARACTER ) )
				pChar = static_cast<CCharacter*>(pObject);

			if(pChar && (g_bSinglePlayer || pChar != this))
			{
				GetGameGuiManager()->AddMessageBox(avar("是否要与 %s 交易？",pChar->GetName()),eAction_SelfRequestTrade,(void*)pChar->GetID(),"是|1|Enter|否|0|Esc");
				GetGameState()->SetStateItem(eGameStateItem_TradeRequest);
			}
			RestoreMouseIcon();
		}
		return true;
		break;

	case eCursorShowUser:		// 查看玩家
		{
			CCharacter* pChar = NULL;
			if ( pObject && pObject->IsSceneObjectType( OBJECT_CHARACTER ) )
				pChar = static_cast<CCharacter*>(pObject);

			if(pChar && (g_bSinglePlayer || pChar != this))
			{
				stSelectMainUserPropertyUserCmd  cmd;
				cmd.dwTempID = pChar->GetID();
				cmd.byType = MAPDATATYPE_USER;

				SEND_USER_CMD(cmd);
			}
			RestoreMouseIcon();
		}
		return true;
		break;
	case eCursorDictate:    //情感
		{
			CCharacter* pChar = NULL;
			if ( pObject && pObject->IsSceneObjectType( OBJECT_CHARACTER ) )
				pChar = static_cast<CCharacter*>(pObject);
			if(pChar)
			{
				GetEmotionInfo()->OnUseEmotionOnSome(pChar);
				if( g_bSinglePlayer )
				{
					char szChatPrivate[512]={0};
					_snprintf(szChatPrivate,sizeof(szChatPrivate),"%s",GetEmotionInfo()->GetCurIconDesc1().c_str());
					szChatPrivate[sizeof(szChatPrivate)-1] = 0;
					GetGameGuiManager()->AddClientSystemMessage(szChatPrivate,COLOR_ARGB(255,246,0,255));

					pChar->m_BitmapShow.StartShow();
					int nFrame ;
					if( GetEmotionInfo()->GetCurAnimation(nFrame))
					{
						pChar->m_BitmapShow.SetFrame(nFrame);
					}						
				}
			}
			RestoreMouseIcon();
			return true;
			break;
		}
	case eCursorPetAttack:	
		{
			if(GetGameGuiManager()->m_guiPetFloatDlg)
			{
				CNpc* pNpc = NULL;
				if ( pObject && pObject->IsSceneObjectType( OBJECT_NPC ) )
					pNpc = static_cast<CNpc*>(pObject);

				if(pNpc) GetGameGuiManager()->m_guiPetFloatDlg->OnPetAttackNpc(pNpc);
				
				CCharacter* pChar = NULL;
				if ( pObject && pObject->IsSceneObjectType( OBJECT_CHARACTER ) )
					pChar = static_cast<CCharacter*>(pObject);
				if(pChar) GetGameGuiManager()->m_guiPetFloatDlg->OnPetAttackNpc(pChar);
			}
		}
		return true;
	case eCursorFreeMove:
		{
			RestoreMouseIcon();
			m_bFreeMoveState = false;
			return true;
		}
		break;
	}


	if ( pObject )
	{
		if ( isset_state(GetState(),USTATE_CRAZY_ACTION)
			|| isset_state(GetState(),USTATE_CRAZY) )
			return false;

		CNpc* pNpc = NULL;
		if ( pObject->IsSceneObjectType( OBJECT_NPC ) )
			pNpc = static_cast<CNpc*>(pObject);
		else
			pNpc = NULL;

		//Use skill on character
		if ( pNpc )		//&& !(pNpc->IsMainRole())
		{
			if( ( ( (GetKeyState(VK_SHIFT) < 0) || !bNeedShift )
				|| (GetGameCursor()->GetCursorImageType() == eCursorFollowUser)
				|| (GetGameCursor()->GetCursorImageType() == eCursorUseAccess) )
				&& !(pNpc->IsMainRole()) )
			{
				SelectInterestNpc( pNpc );
				
				if ( IsFollowState() )		//clear follow event
				{
					ClearRunEvent();
				}

				
				//Follow user
				if ( GetGameCursor()->GetCursorImageType() == eCursorFollowUser )
				{
					if( pNpc->IsCharacter() && !pNpc->IsMainRole() )
						SetFollowState();

					RestoreMouseIcon();
				}

				if (GetGameCursor()->GetCursorImageType() != eCursorUseAccess)
					return true;
			}

			//When cursor state is use access
			if ( GetGameCursor()->IsDrag() && (GetGameCursor()->GetCursorImageType() == eCursorUseAccess) )
			{
				UseSkillOnCharacter( pNpc );
				RestoreMouseIcon();
				return true;
			}
		}

		if ( !(pObject->IsDie()) )
		{
			CCharacter*	pChar = NULL;
			pChar = (pObject->IsSceneObjectType( OBJECT_CHARACTER ) ? (CCharacter*)pObject : NULL);

			//
			if ( pChar && (pChar != this ) )
			{
				//触发"第一次选中别的角色"教程事件
				OnTipHelp(USER_SETTING_HELP_PK);

				//触发"第一次右键选项"教程事件
				OnTipHelp(USER_SETTING_HELP_RIGHTBTN);

				//第一次触发"组队"事件
				OnTipHelp(USER_SETTING_HELP_TEAM);

				if( GetKeyState(VK_SHIFT) < 0 )
				{

				}
			}
		}
		
	}

	return false;
}

bool CMainCharacter::HandleRButtonDownEventOnSceneObject( CSceneObject* pObject )
{
	if ( pObject )		//use skill on character
	{
		if ( isset_state(GetState(),USTATE_CRAZY_ACTION)
			|| isset_state(GetState(),USTATE_CRAZY) )
			return false;

		CNpc* pNpc = NULL;
		if ( pObject->IsSceneObjectType( OBJECT_NPC ) )
			pNpc = static_cast<CNpc*>(pObject);
		else
			pNpc = NULL;
		if ( pNpc && !(pNpc->IsMainRole()) )
		{
			if ( GetKeyState( VK_SHIFT ) < 0 )
			{
				SelectInterestNpc( pNpc );
				if(pNpc->IsNpcType(NPC_TYPE_FUNCTION))
					return false;
				return true;
			}
		}
	}

	return false;
}

void CMainCharacter::SelectInterestNpc( CNpc* pNpc )
{
	// 选择了NPC
	if ( ( NULL == GetScene()->m_pInterestNpc)
		|| ( GetScene()->m_pInterestNpc && (GetScene()->m_pInterestNpc != pNpc) ) )
	{
		GetScene()->m_pInterestNpc = pNpc;

		//Send msg to server
		stSelectPropertyUserCmd cmd;
		cmd.byType = pNpc->IsCharacter() ? MAPDATATYPE_USER : MAPDATATYPE_NPC;
		cmd.dwTempID = pNpc->GetID();
		cmd.byOldType = GetScene()->m_InterestNpcBaseInfo.bCharacter ? MAPDATATYPE_USER : MAPDATATYPE_NPC;
		cmd.dwOldTempID = GetScene()->m_InterestNpcBaseInfo.tmpID;

		SEND_USER_CMD( cmd );

		//Save interest npc base info
		GetScene()->m_InterestNpcBaseInfo.bCharacter = pNpc->IsCharacter();
		GetScene()->m_InterestNpcBaseInfo.bNpc = pNpc->IsNpc();
		GetScene()->m_InterestNpcBaseInfo.tmpID = pNpc->GetID();
		GetScene()->m_InterestNpcBaseInfo.nAttackFailNum = 0;

		if ( GetGameGuiManager()->m_guiMain )
		{
			while( GetGameGuiManager()->m_guiMain->DeleteInterestNpcStateList() );
		}
	}

	//any way turn on dlg
	if ( GetScene()->m_pInterestNpc )
	{
		GetGameGuiManager()->AddCharacterProperty();
		if ( GetScene()->m_pInterestNpc && !GetScene()->m_pInterestNpc->IsDie() && GetGameGuiManager()->m_guiCharacterProperty )
		{
			GetGameGuiManager()->m_guiCharacterProperty->InitIcon();
			GetGameGuiManager()->m_guiCharacterProperty->InitDetail();

			if ( pNpc->IsCharacter() )
			{
				CCharacter* pChar = (CCharacter*)pNpc;
				DWORD dwColor = GetNameColor( pChar->GetGoodness() );
				GetGameGuiManager()->m_guiCharacterProperty->SetName( pChar->GetName(), dwColor );
			}
			else
			{
				GetGameGuiManager()->m_guiCharacterProperty->SetName( pNpc->GetName() );
			}
		}
	}
}

bool CMainCharacter::AttackAttackingNpc()
{
	FUNCTION_BEGIN;

	if ( isset_state(GetState(),USTATE_CRAZY_ACTION)
		|| isset_state(GetState(),USTATE_CRAZY) )
		return false;

	if ( m_pAttackNpc && !m_pAttackNpc->IsMainRole() )
	{
		m_RunEvent.id = m_pAttackNpc->GetID();
		m_RunEvent.target = (m_pAttackNpc->IsCharacter() ? eTargetType_Character : eTargetType_Npc);

		m_RunEvent.pt = GetScene()->GridCenterToPixelPos( m_pAttackNpc->GetPos() );
		m_RunEvent.pSkill = m_pActiveSkill;
		m_RunEvent.speed = GetMoveSpeed();
		m_RunEvent.event = eEventType_Attack;

		SelectInterestNpc( m_pAttackNpc );

		return true;
	}

	return false;

	FUNCTION_END;
}

void CMainCharacter::SelectSkill(SkillBase_t* pSelectSkill)
{
	if ( GetSkillDisableStatus( pSelectSkill ) < 1 )
	{
		GetGameGuiManager()->AddClientSystemMessage( "技能处于冷却中!" );
		return;
	}

	if ( GetGameCursor()->IsDrag()
		&& (GetGameCursor()->GetCursorImageType() == eCursorFindAccess) )
	{
		if ( GetGameGuiManager()->m_guiMain )
			GetGameGuiManager()->m_guiMain->OnFindAccessSkill();
	}
	else
	{
		if ( GetGameCursor()->GetCursorImageType() == eCursorUseAccess )
		{
			if ( GetGameGuiManager()->m_guiTeam )
				GetGameGuiManager()->m_guiTeam->UseSkillOnTeamMember( false );
		}
		else
		{
			if( IsCanSelectInHandSkill( pSelectSkill ) && (!GetScene()->IsAnyAliveInterestNpc()) )
			{
				//   My pet of type summon
				CPet* pPet = NULL;
				if ( pSelectSkill->dwID == SKILL_STRENGTHEN_SUMMON )
					pPet = GetScene()->FindPetByType( PET_TYPE_SUMMON );
				
				if ( pPet )
				{
					if ( GetGameGuiManager()->m_guiTeam )
						GetGameGuiManager()->m_guiTeam->m_pActiveSkill = pSelectSkill;
					UseSkillOnCharacter( pPet->GetNpc() );
				}
				else
				{
					if ( GetGameGuiManager()->m_guiTeam )
						GetGameGuiManager()->m_guiTeam->m_pActiveSkill = pSelectSkill;
					GetGameCursor()->SetCursor( CCursor::CursorType_Drag, eCursorUseAccess );
				}				
			}
			else
			{
				GetScene()->GetMainCharacter()->OnSelectSkill( pSelectSkill->dwID );
				RestoreMouseIcon();
			}
		}								
	}
}
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nMsg : 描述
 * \param wParam : 描述
 * \param lParam : 描述
 * \return 返回值的描述
 */
bool CMainCharacter::MsgProc( UINT nMsg,WPARAM wParam,LPARAM lParam )
{
	bool retval;
	FUNCTION_BEGIN;

	retval = false;
	stPointI pt( GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) );
	if( nMsg == WM_LBUTTONDOWN || 
		nMsg == WM_RBUTTONDOWN ||
		nMsg == WM_LBUTTONUP ||
		nMsg == WM_RBUTTONUP )
	{
		CGuiDialog* pMouseDlg = GetGuiManager()->GetMouseOverDlg();

		if( pMouseDlg && !pMouseDlg->IsSystemDialog()	) 
			RETURN( false );

		if(pMouseDlg == GetGameGuiManager()->m_guiMain)
		{
			if(GetGameGuiManager()->m_guiMain->m_bMouseInBackground) 
				RETURN( false);
		}
		if( nMsg == WM_RBUTTONDOWN )
		{
			if(pMouseDlg && pMouseDlg->GetPopupMenu() )
			{
				if(GetGameGuiManager()->m_guiChatOutput != pMouseDlg) // 聊天框弹出菜单与正常的有点区别
					RETURN( false);
			}
		}
	}

	switch(nMsg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			if ( (GetKeyState(VK_CONTROL) < 0) && (GetKeyState(VK_SHIFT) < 0) && (wParam == 'X') )
			{
				if ( GetScene()->m_pInterestNpc && GetScene()->m_pInterestNpc->IsCharacter() )
				{
					//Copy player name to clipboard for gm use
					GetGameGuiManager()->m_guiCharacterProperty->CopyNameToClipboard();
					RETURN( true );
				}				
			} 
			
			//See if change attack mode access key key-down
			if ( (GetKeyState(VK_CONTROL) < 0) && (wParam == 'X') )
			{
				SendAttackMode( 255 );
				RETURN( true );
			}
			//End

			//soke 一键换装  快捷键
			for(int i= 1;i< 10;i++)
			{
				if ( (GetKeyState(VK_SHIFT) < 0) && (wParam == '0'+ i) )
				{
					for(int j=0;j<9;j++)
					{
						GetGameGuiManager()->m_guiUserInfo->equipTaoZhuang(i-1);	
						RETURN( true );	
					}
				}
			}

			//soke 一键换装  快捷键
			for(int i= 10;i< 11;i++)
			{
				if ( (GetKeyState(VK_SHIFT) < 0) && (wParam == '0'+ i) )
				{
					for(int j=9;j<10;j++)
					{
						GetGameGuiManager()->m_guiUserInfo->equipTaoZhuang(i-10);	
						RETURN( true );	
					}
				}
			}
			

			
			
			//MAKE 增加`键捡取地上物品
			if ((GetKeyState(VK_CONTROL) < 0) && (wParam == VK_OEM_3))
			{
				RunAutoPickup();
                RETURN( true );
			}

			//Here we treat all chat mode access key key-down
			if ( GetGameGuiManager()->m_guiChatButton )
			{
				if ( GetGameGuiManager()->m_guiChatButton->TreateAllChatModeAccessKey( wParam ) )
					RETURN( true );
			}
			//End treat

			//See if sit-down hot key key-down
			if ( (wParam == 'D') && g_bSitDownHotKeyActive )
			{
				OnSitDown();
				g_bSitDownHotKeyActive = false;
				RETURN( true );				
			}		

			//See if any thing to pick
			if ( wParam == 192 )		// `
			{
				PickUpNearbyItem( false );
			}

			if( wParam == '1' && GetKeyState( VK_CONTROL ) < 0 )
			{
				if(GetGameGuiManager()->m_guiPetFloatDlg)
				{
					CPet* pPet = GetGameGuiManager()->m_guiPetFloatDlg->GetCurrentPet();
					if(pPet)
					{
						pPet->AutoChangeAI();
					}
				}
			}
			//See if we retry attack npc
			if ( wParam == 'A' )
			{
				if ( AttackAttackingNpc() )
					RETURN( true );
			}

			if ( GetKeyState(VK_CONTROL) >= 0 )  // 附近的人 F
			{
			   if (wParam == 'F')
			   {
				   OnOffNearbyPlayer();
				   RETURN( true );
			   }
			}
			else if ( GetKeyState(VK_CONTROL) < 0 ) // 跳舞快捷键 Ctrl+F
			{
			   if (wParam == 'F')
			   {
				   GetGameGuiManager()->AddTiaoGame();
				   RETURN( true );
			   }
			}
			
			//See if any access key active
			//if(wParam >= VK_F1 && wParam <= VK_F12)
			{
				enumGameAccelKeyType keyType;
				DWORD dwKeyID;
				BYTE curGroup;
				if ( GetGameGuiManager()->m_guiMain )
					curGroup = GetGameGuiManager()->m_guiMain->GetCurAccessKeyGroup();
				if(GetGameAccelKeyManager()->GetCurAccelKeyID(keyType,dwKeyID, curGroup))
				{
					switch( keyType )
					{
					case GameAccelKeyType_Skill:
						{
							if ( isset_state(GetState(),USTATE_CRAZY_ACTION)
								|| isset_state(GetState(),USTATE_CRAZY) )
							{
								RestoreMouseIcon();
								break;
							}

							{
								//We call corresponding action according to skill target type
								SkillBase_t* pSelectSkill = GetSkillBase( dwKeyID );
								if ( pSelectSkill )
								{
									// 此处的处理全部转到 SelectSkill 函数中，方便其它地方调用此功能。 
									SelectSkill(pSelectSkill);
								}										
							}
						}
						RETURN( true );
						break;
					case GameAccelKeyType_Item:
						{
							CRoleItem* pItem = GetScene()->GetMainCharacter()->FindItemByID( dwKeyID);
							if ( (NULL == pItem) || !pItem->IsAccessNeedThisID() )
								pItem = GetScene()->GetMainCharacter()->FindItemByLocationConstID( dwKeyID, OBJECTCELLTYPE_COMMON );
							else
							{
								if ( pItem->GetLocation().dwLocation != OBJECTCELLTYPE_COMMON )
									pItem = NULL;
							}

							if ( pItem )
							{
								retval = CALL_USEITEM(pItem->GetThisID());
								goto _ret;
							}
						}	
						break;
					case GameAccelKeyType_Face:
						{
                               WORD dwIconID = dwKeyID;
						       stResourceLocation rl;
						       rl.SetFileName(c_IconGL);
						       rl.group = EMOTION_GROUP;
						       rl.frame = dwIconID - EMOTION_ICON_ID_BASE;
						       GetEmotionInfo()->SetCurIconInfo(dwIconID,FROM_KEYBOARD);
						       GetGameCursor()->SetCursor( CCursor::CursorType_Drag, eCursorDictate );
						       stPointI ptCursor(0,0);
						       Engine_GetCursor()->SetImage( &rl, ptCursor );
						}
						break;
					case GameAccelKeyType_TaoZhuang: //sky 一键换装拖动快捷栏按键
						{
							GetGameGuiManager()->m_guiUserInfo->equipTaoZhuang(dwKeyID);
						}
						break;
					case GameAccelKeyType_Equipment:
						{
							CRoleItem* pItem = GetScene()->GetMainCharacter()->FindItemByID( dwKeyID );
							if ( pItem )
								EquipItem( pItem ,-1);
						}
						break;
					}
				}
			}			
		}
		break;
	case WM_LBUTTONDOWN:
		{
			if( NULL != GetScene()->m_pMouseOverTextObject )
			{
				DWORD dwId = GetScene()->m_pMouseOverTextObject->GetID();
				if( 0 != dwId )
				{
					GetScene()->m_pMouseOverTextObject = NULL;
					RETURN( CGuiShopDialog::OnOpenShop( dwId ) );
				}
			}
			if ( !( GetScene()->m_pMouseOverObject
				&& ( GetScene()->m_pMouseOverObject->IsSceneObjectType(OBJECT_NPC)
					|| GetScene()->m_pMouseOverObject->IsSceneObjectType(OBJECT_CHARACTER) ) ) )
			{
				m_bLocalSpaceLButtonDown = true;
				m_InputState.dwLButtonDownTimeFirst = xtimeGetTime();
			}

			if ( GetScene()->m_pMouseOverObject && GetScene()->m_pMouseOverObject->IsSceneObjectType(OBJECT_NPC) )
			{
				CNpc* pNpc = static_cast<CNpc*>(GetScene()->m_pMouseOverObject);
				if ( pNpc && !pNpc->IsCanAttack() )
					m_bFreeMoveState = false;
			}
			
			{
				CSceneObject* pObject = GetScene()->GetObjectAtScreenPixel<CCharacter>(pt);
				if ( NULL == pObject )
					pObject = GetScene()->GetObjectAtScreenPixel<CNpc>(pt);


				//if ( pObject )
				{
					if( HandleLButtonDownEventOnSceneObject( pObject, true ) )
						RETURN( true );
				}				
			}					

			if(!GetGameCursor()->IsNormal())
			{
				// 不是正常的光标
				RestoreMouseIcon();
				RETURN( true );
			}

			CGuiFloatItem* pFloatItem;
			if( pFloatItem = GetGuiManager()->GetFloatItem() )
			{
				//鼠标上有物品
				if(GetGuiManager()->GetMouseOverDlg())
				{
				}
				else
				{
					// 丢物品					
					ItemTableEvent(NULL,WM_LBUTTONDOWN,stPointI(0,0),0,0);
				}
				RETURN( true );
			}
			m_InputState.dwLButtonDownTime = xtimeGetTime();
			m_InputState.ptMouse = pt;
			m_InputState.bLButtonDown = true;
			m_InputState.bGM = false;
			RETURN( OnInputEvent( nMsg, GetScene()->m_pMouseOverObject ) );
		}
		break;
	case WM_RBUTTONDOWN:
		{
			//宠物攻击
			if( GetGameCursor()->GetCursorImageType() == eCursorPetAttack )
			{
				if( GetGameGuiManager()->m_guiPetFloatDlg )
				{
					GetGameGuiManager()->m_guiPetFloatDlg->OnEndSelectAttack();
					RETURN( true );
				}
			}

			//选择邮寄附件物品
			if ( GetGameCursor()->GetCursorTypeEx() == eCursorGetMailItem )
			{
				GetGameCursor()->SetCursorTypeEx(eCursorNormal) ;
				//光标换成普通光标
				GetGameCursor()->SetCursor(CCursor::CursorType_Normal,eCursorNormal);
				RETURN( true );
			}

			if ( GetScene()->m_pMouseOverObject )
			{
				if ( HandleRButtonDownEventOnSceneObject( GetScene()->m_pMouseOverObject ) )
					RETURN( true );
			}		
		
			//set input event
			{
				m_InputState.dwRButtonDownTime = xtimeGetTime();
				m_InputState.ptMouse = pt;
				m_InputState.bRButtonDown = true;
				m_InputState.bGM = false;
				RETURN( OnInputEvent( nMsg, GetScene()->m_pMouseOverObject ) );
			}
		}
		break;
	case WM_LBUTTONUP:
		m_InputState.ptMouse = pt;
		m_InputState.bLButtonDown = false;
		m_bLocalSpaceLButtonDown = false;
		
		RETURN( true );
		break;
	case WM_RBUTTONUP:
		m_InputState.ptMouse = pt;
		m_InputState.bRButtonDown = false;
		m_InputState.dwLButtonDownTimeFirst = 0;
		RETURN( true );
		break;
	case WM_MOUSEMOVE:
		m_InputState.ptMouse = pt;
		if(!(wParam & MK_LBUTTON)) m_InputState.bLButtonDown = false;
		if(!(wParam & MK_RBUTTON)) m_InputState.bRButtonDown = false;

		if ( GetGameCursor()->GetCursorImageType() == eCursorFreeMove )
		{
			if( bIsShopStart() )
			{
				GetGameGuiManager()->AddClientSystemMessage("摆摊情况下不可以移动");
				RETURN( true );
			}

			if ( isset_state(GetState(),USTATE_CRAZY_ACTION)
				|| isset_state(GetState(),USTATE_CRAZY) )
				RETURN( true );

			m_RunEvent.target = eTargetType_None;
			m_RunEvent.pt = GetScene()->ScreenPixelPosToPixelPos(m_InputState.ptMouse);
			m_RunEvent.event = eEventType_Move;
			m_bFirstTimeChangePath = true;
			if(Engine_GetState()->IsCtrlDown())
				m_RunEvent.speed = 0;
			else
				m_RunEvent.speed = GetMoveSpeed();
		}
		break;
	}

_ret:
	;
	FUNCTION_END;
	return retval;
}


static stRectI s_pathZoneRect;

static int __stdcall  s_fn_FreeTile2(int x1,int y1,int x2,int y2)
{

	if(s_pathZoneRect.PtInRect(stPointI(x1,y1)) && s_pathZoneRect.PtInRect(stPointI(x2,y2)))
	{
		CGameScene::stPathZone & zone = GetScene()->GetPathZone(stPointI(x1,y1));
		stPointI ptOffset(x2-x1,y2-y1);
		
		Assert(ptOffset.x >= -1 && ptOffset.x <= 1);
		Assert(ptOffset.y >= -1 && ptOffset.y <= 1);
		return !zone.IsBlock(ptOffset);
	}

	return false;
}

static int __stdcall  s_fn_FreeTile3(int x,int y)
{
	if(s_pathZoneRect.PtInRect(stPointI(x,y)))
	{
		CGameScene::stPathZone & zone = GetScene()->GetPathZone(stPointI(x,y));
		if(zone.flags) return 0;
		return 1;
	}

	return false;
}

static POINT PathZoneLately(POINT ptZone,POINT ptPos)
{
	stRectI rc;
	rc.left = ptZone.x * PathZoneSize;
	rc.top = ptZone.y * PathZoneSize;
	rc.right = rc.left + PathZoneSize;
	rc.bottom = rc.top + PathZoneSize;
	stRectI rcMap(0,0,GetScene()->GetWidth(),GetScene()->GetHeight());
	rc &= rcMap;
	stPointI minPt(rc.left,rc.top);
	int minDis = 1000000000;
	for(int i=rc.top; i < rc.bottom; ++i)
	{
		for(int j=rc.left; j < rc.left; ++j)
		{
			if(!GetScene()->IsMoveObstacle(stPointI(j,i)))
			{
				int dis = Scene_GetDis(stPointI(j,i),ptPos);
				if(dis < minDis)
				{
					minDis = dis;
					minPt = stPointI(j,i);
				}
			}
		}
	}
	return minPt;
}

bool CMainCharacter::FindPathNear(POINT ptGrid,int speed,int nMaxFindPathDis)
{
	stPointI ptSrc;
	if( m_pCharacter->IsMoving() && !m_pCharacter->IsReachDes() )
		ptSrc = GetScene()->PixelPosToGridPos(m_pCharacter->GetDes());
	else
		ptSrc = GetScene()->PixelPosToGridPos(m_pCharacter->GetPos());
	
	m_AStar.SetMaxFindDis(nMaxFindPathDis);

	s_rcFindPathRect.left = ptSrc.x - nMaxFindPathDis;
	s_rcFindPathRect.top = ptSrc.y - nMaxFindPathDis;
	s_rcFindPathRect.right = ptSrc.x + nMaxFindPathDis;
	s_rcFindPathRect.bottom = ptSrc.y + nMaxFindPathDis;

	stRectI rcMap(0,0,GetScene()->GetWidth(),GetScene()->GetHeight());

	s_rcFindPathRect &= rcMap;

	SetFrameAllocator fa;

	DWORD dwTim1 = xtimeGetTime();

	m_AStar.SetStep(speed);

	bool bOldVal = GetScene()->m_bDoorBlock;

	m_AStar.FindPath(ptSrc.x,ptSrc.y,ptGrid.x,ptGrid.y);

	GetScene()->m_bDoorBlock = bOldVal;

	if( m_AStar.IsHavePath() )
	{
		//FILE* f = fopen("astar.log","at");
		//if ( f )
		//{
		//	fprintf(f,"寻路： 寻路时间 %d，源 (%d,%d) ，目标 (%d,%d)\n",tA,ptSrc.x,ptSrc.y,m_AStar.LastNodeGetX(),m_AStar.LastNodeGetY());
		//}
		//fclose(f);
		//m_moveTarget.UpdateGridPosition(stPointI(m_AStar.LastNodeGetX(),m_AStar.LastNodeGetY()));
#if defined _DEBUG || defined RELEASE_TEST
		//m_AStar.PrintfPath();
		//Engine_WriteLogF("寻路： 寻路时间 %u，预计目标 (%d,%d) ，实际目标 (%d,%d)\n",xtimeGetTime()-dwTim1,ptGrid.x,ptGrid.y,m_AStar.LastNodeGetX(),m_AStar.LastNodeGetY());
#endif
	}

	if(m_AStar.IsHavePath())
	{	
		m_moveTarget.UpdateGridPosition(stPointI(m_AStar.LastNodeGetX(),m_AStar.LastNodeGetY()));

		m_AStar.PathNextNode();
		return true;
	}
	return false;
}

bool CMainCharacter::GetNextZonePathPos(POINT & ptNextGrid,POINT ptTargetPos)
{
	stPointI ptSrcGrid;
	if( m_pCharacter->IsMoving() && !m_pCharacter->IsReachDes() )
	{
		ptSrcGrid = GetScene()->PixelPosToGridPos(m_pCharacter->GetDes());
	}
	else
	{
		ptSrcGrid = GetScene()->PixelPosToGridPos(m_pCharacter->GetPos());
	}

	stPointI ptTargetZone = GetScene()->GridPosToPathZonePos(ptTargetPos);
	stPointI pOldTargetPos(m_AStarZone.GetDstX(),m_AStarZone.GetDstY());
	
	if(pOldTargetPos != ptTargetZone)
	{
		m_AStarZone.FreePath();
		return false;
	}

	stPointI ptZone0(-1,-1);
	bool bGet = false;
	while(m_AStarZone.IsHavePath())
	{
		stPointI ptZone = stPointI(m_AStarZone.NodeGetX(),m_AStarZone.NodeGetY());
		stPointI pt2 = GetScene()->PathZonePosToGridPos(ptZone);

		if( Scene_GetDis(pt2,ptSrcGrid) >= c_nMaxFindPathDisNear )
		{
			if(bGet) break;
		}
		else
		{
			bGet = true;
			ptZone0 = ptZone;
			m_AStarZone.PathNextNode();
			break;
		}
		m_AStarZone.PathNextNode();
	}

	if(ptZone0.x != -1)
	{
		ptNextGrid = PathZoneLately(ptZone0,ptSrcGrid);
		return true;
	}
	return false;
}


bool CMainCharacter::FindPathFar(POINT ptGrid,int speed,int nMaxFindPathDis)
{
	int maxDis = nMaxFindPathDis;
	SetFrameAllocator fa;

	stPointI ptSrcGrid;
	if( m_pCharacter->IsMoving() && !m_pCharacter->IsReachDes() )
	{
		ptSrcGrid = GetScene()->PixelPosToGridPos(m_pCharacter->GetDes());
	}
	else
	{
		ptSrcGrid = GetScene()->PixelPosToGridPos(m_pCharacter->GetPos());
	}

	m_AStarZone.SetStep(1);
	m_AStarZone.SetMallocFn(s_fn_malloc,s_fn_free);
	m_AStarZone.SetFreeTile2(s_fn_FreeTile2);
	m_AStarZone.SetMaxFindDis(maxDis);

	stPointI ptSrc = GetScene()->GridPosToPathZonePos(ptSrcGrid);
	stPointI ptDst = GetScene()->GridPosToPathZonePos(ptGrid);

	s_pathZoneRect.left = ptSrc.x - maxDis;
	s_pathZoneRect.top = ptSrc.y - maxDis;
	s_pathZoneRect.right = ptSrc.x + maxDis;
	s_pathZoneRect.bottom = ptSrc.y + maxDis;

	s_pathZoneRect = stRectI( 0, 0, GetScene()->m_nPathZoneWidth, GetScene()->m_nPathZoneHeight );
	DWORD dwTim1 = xtimeGetTime();
	m_AStarZone.FindPath(ptSrc.x, ptSrc.y, ptDst.x, ptDst.y);
	
	if( m_AStarZone.IsHavePath() )
	{
#if defined _DEBUG || defined RELEASE_TEST
		//Engine_WriteLogF("寻路2： 寻路时间 %u，预计目标 (%d,%d) ，实际目标 (%d,%d)\n",xtimeGetTime()-dwTim1,ptDst.x,ptDst.y,m_AStarZone.LastNodeGetX(),m_AStarZone.LastNodeGetY());
#endif
		m_AStarZone.PathNextNode();	
		return true;
	}

	return false;
}

static POINT GetNewFreePoint(POINT pt)
{
	int count = PathZoneSize/2;

	for(int i=0; i < count;++i)
	{
		int minX = getMax(pt.x - i,(long)0);
		int maxX = getMin(pt.x + i,(long)(GetScene()->GetWidth() - 1));
		int minY = getMax(pt.y - i,(long)0);
		int maxY = getMin(pt.y + i,(long)(GetScene()->GetHeight() - 1));

		int x,y;
		y = minY;
		for(x = minX; x <= maxX; ++x)
		{
			if(!GetScene()->IsStaticObstacle(stPointI(x,y)))
				return stPointI(x,y);
		}

		if(maxY != minY)
		{
			y = maxY;
			for(x = minX; x <= maxX; ++x)
			{
				if(!GetScene()->IsStaticObstacle(stPointI(x,y)))
					return stPointI(x,y);
			}
		}

		x = minX;
		for(y = minY + 1; y <= maxY - 1; ++y)
		{	
			if(!GetScene()->IsStaticObstacle(stPointI(x,y)))
				return stPointI(x,y);
		}

		if(maxX != minX)
		{
			x = maxX;
			for(y = minY + 1; y <= maxY - 1; ++y)
			{	
				if(!GetScene()->IsStaticObstacle(stPointI(x,y)))
					return stPointI(x,y);
			}
		}

	}
	return pt;

}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \param speed : 描述
 * \return 返回值的描述
 */
bool CMainCharacter::FindPath( POINT pt,int speed )
{
	FUNCTION_BEGIN;

	m_AStar.FreePath();

	stPointI ptDst = GetScene()->PixelPosToGridPos(pt);
	stPointI ptSrc = GetGridPos();
	
	if( m_pCharacter->IsMoving() && !m_pCharacter->IsReachDes() )
	{
		ptSrc = GetScene()->PixelPosToGridPos(m_pCharacter->GetDes());
	}
	else
	{
		ptSrc = GetScene()->PixelPosToGridPos(m_pCharacter->GetPos());
	}
	

	extern bool gbGetNewPoint;
	gbGetNewPoint = true;
	if(m_InputState.bGM)
	{
		extern bool gFindPathDisMode2;
		gFindPathDisMode2 = true;
		extern bool g_AStarFast;
		g_AStarFast = true;
		m_AStar.SetFindPathMode(CAStar::eFindPathMode1);
		bool b = FindPathNear(ptDst,speed,c_nMaxFindPathDisGM);
		gFindPathDisMode2 = false;
		return b;
	}
	else
	{
		extern bool gbGetNewPoint;
		extern bool g_AStarFast;
		g_AStarFast = false;
		gbGetNewPoint = true;
		extern bool g_AStarFast;
		g_AStarFast = false;
		m_AStar.SetFindPathMode(CAStar::eFindPathMode1);
		return FindPathNear(ptDst,speed,c_nMaxFindPathDisNear);
	}
	
	return true;
	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \param speed : 描述
 * \return 返回值的描述
 */
bool CMainCharacter::WalkTo(POINT pt,int speed)
{
	bool retval;
	FUNCTION_BEGIN;
	CHECK_RET_ADDR
	retval = false;

	if(m_bNoRunState && !g_bSinglePlayer)
	{
		if( 0 )
			speed = 1;
		else
			m_bNoRunState = false;
	}

	if (m_pCharacter->IsRide())
	{
		if (speed > 1)
			speed = 3;
	}
	else if (isset_state(this->GetState(), USTATE_SPA))
	{
		if (speed > 1)
		{
			speed = 1;
		}
	}
	else
	{
		if(speed > 2)
			speed = 2;
	}
	POINT des = GetScene()->PixelPosToGridPos(pt);
	if(speed == 0)
	{
		if(!CanSendWalkMsg())
			RETURN( false );

		POINT pos = GetScene()->PixelPosToGridPos(m_pCharacter->GetPos());
		int dir = Scene_GetDir8(des.x - pos.x,des.y - pos.y);
		if(dir == -1) RETURN( false );
		return SendWalk(dir,0,pt);
	}

	bool bFindPath = false;
	
	if(m_AStar.IsHavePath() && m_AStar.GetStep() == speed && m_AStar.GetDstX() == des.x && m_AStar.GetDstY() == des.y){
		/*if(m_InputState.bGM && !m_InputState.bReFindPath){
			if(Scene_GetDis(GetGridPos(),des) < c_nMaxFindPathDisGM * 2/3 )
			{
				if(Scene_GetDis(stPointI(m_AStar.LastNodeGetX(),m_AStar.LastNodeGetY()),des) > 15){
					m_InputState.bReFindPath = true;
					RETURN( FindPath(pt,speed) );
				}
			}
		}*/
		RETURN( true );
	}

	RETURN( FindPath(pt,speed) );

_ret:
	;
	FUNCTION_END;
	return retval;
}
static DWORD timeCannotWalk = 0;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
bool CMainCharacter::WalkPath()
{
	FUNCTION_BEGIN;

	//当队列里仍然存在move的指令的时候不能move
	//当队列里仍然存在fight的指令的时候不能fight
	/////////star100515 ori
	if(GetGameApplication()->bClientSetted( enumAuto_Kill_Summon ) && 
		GetGameGuiManager()->m_guiAutoAttackDlg->m_fBegin_Origin)
		return false;
	if ( GetGameGuiManager()->m_guiAutoAttackDlg->m_fBegin_State && ///////////////////star100515
		GetGameGuiManager()->m_guiAutoAttackDlg->m_fAuto_UseMagic)
		RunAutoWorkSkill();

	if(!m_AStar.IsHavePath())
		return false;

	if(!CanSendWalkMsg())
		return false;

	Assert(m_nSendWalk <= m_nRecieveWalk);
	timeCannotWalk = 0;

	POINT pos = GetScene()->PixelPosToGridPos(m_pCharacter->GetPos());
	POINT ptDst ={ m_AStar.NodeGetX(),m_AStar.NodeGetY()};

	int speed;
	speed = GetMoveSpeed();
	if(speed >= 2 && !g_bSinglePlayer)
	{
		/*if( (long)GetSP() <= 0 )
		{
			m_AStar.FreePath();
			m_bNoRunState = true;
			return false;
		}*/
	}

	int dis = Scene_GetDis(pos,ptDst);
	if(m_pCharacter->IsRide())
	{
		//speed = dis;
		if( dis > 3)
		{
			m_AStar.FreePath();
			return false;
		}
	}
	else if ( !m_RunEvent.pSkill )
	{
		if(dis > 2)
		{
			m_AStar.FreePath();
			return false;
		}
	}

	if (m_RunEvent.speed != speed )
	{
		speed = m_RunEvent.speed;
	}

	int dir = Scene_GetDir8(ptDst.x-pos.x,ptDst.y-pos.y);
	if(dir == -1)
	{
		m_AStar.FreePath();
		return FALSE;
	}

	if(GetScene()->IsMoveObstacle(ptDst))
	{
		int speed = m_moveSpeed;
		if(IsRide() && speed ==2) 
			speed = 3;

		if(!FindPath(GetScene()->GridCenterToPixelPos(stPointI(m_AStar.GetDstX(),m_AStar.GetDstY())),speed))
			return false;
	}

	if(SendWalk(dir,speed,ptDst))
	{
		m_AStar.PathNextNode();
		return true;
	}

	return false;

	FUNCTION_END;
}

static DWORD s_dwNextSendMoveTime = 0;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
bool CMainCharacter::CanSendWalkMsg()
{
	FUNCTION_BEGIN;

	if(!CanSendCommand(MOVE_USERCMD))
	{
		return false;
	}

	//to do here:
	if( !m_pCharacter->CanChangeToAction(Ani_Walk) )
	{						
		if (!m_pCharacter->IsMoving())
			return false;
		if (!m_pCharacter->IsReachDes())
			return false;
	}

	if(xtimeGetTime() < s_dwNextSendMoveTime) return false;
	if (HaveCommand(MOVE_USERCMD,USERMOVE_MOVE_USERCMD_PARA))
	{
		return false;
	}

	if (m_nSendWalk > m_nRecieveWalk )
	{
		if (timeCannotWalk == 0)
		{
			timeCannotWalk = xtimeGetTime();
		}
		else if (xtimeGetTime() - timeCannotWalk >= 5000)
		{
			m_nSendWalk = m_nRecieveWalk;
		}

		return false;
	}


	return true;

	FUNCTION_END;
}

DWORD gWalkTimeElapsed = 0;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dir : 描述
 * \param speed : 描述
 * \param ptDst : 描述
 * \return 返回值的描述
 */
bool CMainCharacter::SendWalk(int dir,int speed,POINT ptDst)
{
	bool retval;

	FUNCTION_BEGIN;
	stPointI ptMoveSpeed;
	stPointI ptNext;

	retval = false;
	stUserMoveMoveUserCmd cmd;

	cmd.byDirect	= dir;
	cmd.dwUserTempID	=	m_dwID; // not used now !!!
	cmd.bySpeed = speed;

	cmd.x = ptDst.x;
	cmd.y = ptDst.y;	
	DWORD dwCustomData = GetServerKillProcess()->GetSendData();
	stUserMoveMoveUserCmd  cmd1;
	cmd1.byDirect = cmd.byDirect;
	cmd1.bySpeed = cmd.bySpeed;
	cmd1.x = cmd.x;
	cmd1.y = cmd.y;

	MDATA ddata;
	ddata.dwData = dwCustomData;
	ddata.b[0]  ^= 'F';
	ddata.b[1]  ^= 'X';
	ddata.b[2]  ^= 'F';
	ddata.b[3]  ^= 'X';
	cmd1.dwUserTempID = ddata.dwData;
	SEND_USER_CMD( cmd1)

	//helpfile
	long dis = Scene_GetDis(m_ptGrid,ptDst);

	ptNext = GetScene()->GridCenterToPixelPos(ptDst);

	float fChangeSpeed = 1.0f;
	if(dis)
	{
	}

	m_bSendMove	= TRUE;
	m_pCharacter->PauseMove(FALSE);
	
	if (!isset_state(this->GetState(), USTATE_USER_FLY))
	{
		if (speed == 1)
		m_pCharacter->WalkTo(ptNext,true);
		else if (speed >= 2)
		m_pCharacter->RunTo(ptNext,true);
		else if (cmd.bySpeed == 0)
			m_pCharacter->TurnTo(cmd.byDirect,true);
	}
	else
	{
		m_pCharacter->FlyTo(ptNext,true);
	}

	m_nSendWalk = m_nRecieveWalk + 1;
	m_dwSendMoveTime = xtimeGetTime();

	ptMoveSpeed = m_pCharacter->GetMoveSpeed();
	ptMoveSpeed.x = abs(ptMoveSpeed.x);
	ptMoveSpeed.y = abs(ptMoveSpeed.y);
	int tileWidth = (ptMoveSpeed.x == 0 ? GRID_HEIGHT : GRID_WIDTH);
	int moveSpeed = getMax(ptMoveSpeed.x,ptMoveSpeed.y);

	DWORD dwMoveTimer = moveSpeed ? dis * tileWidth * 1000/moveSpeed : 640;

	s_dwNextSendMoveTime = dwMoveTimer + m_dwSendMoveTime + gWalkTimeElapsed;

#if defined _DEBUG || defined RELEASE_TEST
	static DWORD s_time = 0;
	DWORD dt = xtimeGetTime();	
	s_time = dt;
#endif
	if(g_bSinglePlayer)
	{
		GetClient()->PushCommand(&cmd,sizeof(cmd));
	}

	RETURN(true);

_ret:
	;
	FUNCTION_END;
	return retval;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMainCharacter::ClearRunEvent()
{
	FUNCTION_BEGIN;

	if(m_RunEvent.event == eEventType_Move && m_InputState.bGM){
		if(m_pGotoFinalEvent){
			m_pGotoFinalEvent(m_pGotoFinalParam);
			m_pGotoFinalEvent = NULL;
		}
	}
	m_RunEvent.event = eEventType_None;
	m_RunEvent.target = eTargetType_None;
	m_RunEvent.pSkill = NULL;

	m_pActiveNpc = NULL;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMainCharacter::ClearPath()
{
	FUNCTION_BEGIN;

	m_AStar.FreePath();
	m_AStarZone.FreePath();

	FUNCTION_END;
}

void CMainCharacter::RePrevPathNode()
{
	FUNCTION_BEGIN;

	m_AStar.RePrePathNode();

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		增加某一类格子中物品的显示标志
*/
void CMainCharacter::AddItemTipFlagByLocationType(DWORD dwLocation,DWORD flag)
{
	FUNCTION_BEGIN;

	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
	{
		if((*it)->GetLocation().dwLocation == dwLocation)
		{
			(*it)->SetTipFlags( (*it)->GetTipFlags() | flag );
		}
	}

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		删除某一类格子中物品的显示标志
*/
void CMainCharacter::RemoveItemTipFlagByLocationType(DWORD dwLocation,DWORD flag)
{
	FUNCTION_BEGIN;

	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
	{
		if((*it)->GetLocation().dwLocation == dwLocation)
		{
			(*it)->SetTipFlags( (*it)->GetTipFlags() & (~flag) );
		}
	}

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	k
*	用途: 		根据ID查找物品
*/
CMainCharacter::tListItem::iterator CMainCharacter::FindItemItByID(tItemThisID qwID)
{
	FUNCTION_BEGIN;

	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
	{
		if( (*it)->GetThisID() == qwID ) return it;
	}
	return m_listItem.end();

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwID : 描述
 * \return 返回值的描述
 */
CMainCharacter::tListItem::iterator CMainCharacter::FindItemItByConstID( const DWORD& dwID )
{
	FUNCTION_BEGIN;

	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
	{
		if( ((*it)->GetObjectID() == dwID)
			&& ( ((*it)->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON)
				|| ((*it)->GetLocation().dwLocation == OBJECTCELLTYPE_EQUIP)
				|| ((*it)->GetLocation().dwLocation == OBJECTCELLTYPE_SELL)
				|| ((*it)->GetLocation().dwLocation == OBJECTCELLTYPE_PACKAGE)
				|| ((*it)->GetLocation().dwLocation == OBJECTCELLTYPE_STORE) ) ) return it;
	}
	return m_listItem.end();

	FUNCTION_END;
}

CMainCharacter::tListItem::iterator CMainCharacter::FindItemItByLocationConstID( const DWORD& dwID, const DWORD& location )
{
	FUNCTION_BEGIN;

	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
	{
		if( ((*it)->GetObjectID() == dwID) && ((*it)->GetLocation().dwLocation == location) ) return it;
	}
	return m_listItem.end();

	FUNCTION_END;
}

CMainCharacter::tListItem::iterator CMainCharacter::FindItemItByLocationFiveTypeConstID( const DWORD& dwID, const DWORD& location, const DWORD& fivetype )
{
	FUNCTION_BEGIN;

	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
	{		
		if( ((*it)->GetObjectID() == dwID)
			&& ((*it)->GetLocation().dwLocation == location)
			&& ((*it)->GetObject()->fivetype == fivetype) ) return it;
	}
	return m_listItem.end();

	FUNCTION_END;
}

size_t CMainCharacter::GetItemExteriorCountByConstID( const DWORD& dwID, DWORD dwLocation/* =-1 */ )
{
	FUNCTION_BEGIN;

	size_t count = 0;
	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
	{
		if( ( (*it)->GetObjectID() == dwID )
			&& ( ((*it)->GetLocation().dwLocation == dwLocation) || (dwLocation == -1) ) )
		{
			count += (*it)->GetExteriorCount();
		}
	}

	return count;

	FUNCTION_END;
}

size_t CMainCharacter::GetItemExteriorCountByType( const DWORD& dwType )
{
	FUNCTION_BEGIN;

	size_t count = 0;
	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
	{
		if ( (*it)->GetObjectBase()->dwType == dwType )
			count += (*it)->GetExteriorCount();
	}

	return count;

	FUNCTION_END;
}

bool CMainCharacter::IsAnyArrowHasMoreNum( const int& num, DWORD dwLocation /* = OBJECTCELLTYPE_COMMON */ )
{
	FUNCTION_BEGIN;

	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
	{
		if ( (*it)->IsArrow() && ((*it)->GetLocation().dwLocation == dwLocation) )
		{
			if ( (*it)->GetExteriorCount() >= num )
				return true;
		}
	}

	return false;

	FUNCTION_END;
}

void CMainCharacter::UpdateAllSkillUpItemToolTips()
{
	FUNCTION_BEGIN;

	if ( GetGameGuiManager()->m_guiSkill && GetGameGuiManager()->m_guiSkill->IsVisible() )
	{
		for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
		{
			if ( (*it)->GetItemType() == ItemType_SkillUp )
				(*it)->UpdateToolTips();
		}
	}

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		dwID: 物品ＩＤ
*				dwLocation: 格子类型，如果为０，则搜索能使用的格子类型
*               nLevel: 物品等级，如果为-1，则搜索所有等级
*	用途: 		获得某种物品的数量
*/
size_t CMainCharacter::GetItemCount(DWORD dwID,DWORD dwLocation,int nLevel)
{
	FUNCTION_BEGIN;

	size_t count = 0;
	if(dwLocation == 0)
	{
		for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
		{
			if( ((*it)->GetObjectID() == dwID)
				&& ((*it)->GetLocation().dwLocation != OBJECTCELLTYPE_SELL)
				&& ((*it)->GetLocation().dwLocation != OBJECTCELLTYPE_STORE) )				
			{
				if (((*it)->GetObject()->upgrade==nLevel)||(nLevel==-1))
					count += (*it)->GetCount();
			}
		}
	}
	else
	{
		for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
		{
			if((*it)->GetObjectID() == dwID && (*it)->GetLocation().dwLocation == dwLocation)
			{
				if (((*it)->GetObject()->upgrade==nLevel)||(nLevel==-1))
					count += (*it)->GetCount();
			}
		}
	}
	return count;

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	修理所有物品的银子
*	
*	用途: 		获得修理所有物品的银子
*/
DWORD CMainCharacter::GetAllItemRepairMoney(DWORD flag)
{
	FUNCTION_BEGIN;

	DWORD nMoney = 0;
	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
	{
		if((*it)->GetLocation().dwLocation == OBJECTCELLTYPE_EQUIP)
		{
			if (flag==CGuiNpcDialog::eTradeRepairByGold)
			{
				nMoney += GetObjectRepairByGold((*it)->GetObject());
			}
			else
			{
				nMoney += GetObjectRepairMoney((*it)->GetObject());
			}
				
		}
	}
	
	return nMoney;

	FUNCTION_END;
}

void CMainCharacter::RefreshActiveItem(DWORD dwLocation)
{
	if (dwLocation==OBJECTCELLTYPE_EQUIP)
	{
		if (GetGameGuiManager()->m_guiUserInfo)
			GetGameGuiManager()->m_guiUserInfo->RefreshActiveItem();

		if (GetGameGuiManager()->m_pDlgEquiphandbook)  //2023 08 21 百兽图鉴对话框递归记录
			GetGameGuiManager()->m_pDlgEquiphandbook->RefreshActiveItem();		

		if (GetGameGuiManager()->m_guiNpcTrade)
			GetGameGuiManager()->m_guiNpcTrade->RefreshRepairMoney();
	}
}

void CMainCharacter::AddItem( t_Object& obj,BYTE byActionType)
{
	FUNCTION_BEGIN;
	
	ObjectBase_t* pObject = GetObjectBase(obj.dwObjectID);
	if(pObject)
	{
		DWORD dwType = pObject->dwType;

		///触发"如何装备物品"帮助教程事件
		OnTipHelp(USER_SETTING_HELP_EQUIP,( dwType <= 116 && dwType >= 101 ));

		///触发"额外包裹"帮助教程事件
		OnTipHelp(USER_SETTING_HELP_EXPACK,( dwType == 23 ));

		///触发"原料合成"帮助教程事件
		OnTipHelp(USER_SETTING_HELP_MATERIAL_COMPOSE,( dwType == 16 ));

		///触发"魂魄镶嵌"帮助教程事件
		OnTipHelp(USER_SETTING_HELP_SOULSTONE_MAKE,( dwType == 37 ));

		///触发"道具升级"帮助教程事件
		OnTipHelp(USER_SETTING_HELP_ITEM_UPGRID,( dwType == 27 ));

		if (byActionType == EQUIPACTION_OBTAIN)
		{
			PlayItemSound(pObject->itmeSound);
		}
	}

	tListItem::iterator it ;
	if(obj.qwThisID == INVALID_THISID)
	{
		it = m_listItem.end();
	}
	else
	{
		it = FindItemItByID(obj.qwThisID);
	}

	if(it != m_listItem.end())
	{
		if( (*it)->SetObject( &obj ) == false )
			m_listItem.erase(it);
	}
	else
	{	
		//Engine_WriteLogF("升星后 \n");
		if(GetGameGuiManager()->m_guiMakeTo1)
			GetGameGuiManager()->m_guiMakeTo1->m_isUpdateUp = false;
		if(GetGameGuiManager()->m_guiMakeTo2) //修复月光宝盒第二个自动砸星
			GetGameGuiManager()->m_guiMakeTo2->m_isUpdateUp = false; //修复月光宝盒第二个自动砸星	
		CRoleItem* pItem = LOOKASIDE_ALLOC(m_allocRoleItem);
		if( !pItem->SetObject( &obj ) )
		{
			m_allocRoleItem.freePointer(pItem);
			return;
		}

		m_listItem.push_back(pItem);
		RefreshActiveItem(pItem->GetLocation().dwLocation);

		//Add client msg
		if ( byActionType == EQUIPACTION_OBTAIN )
		{
			char szMsg[MAX_PATH];
			sprintf( szMsg, "你获得了物品 %s", pItem->GetName() );
			str_del( szMsg, '0' );
			GetGameGuiManager()->AddClientSystemMessage( szMsg, 0xff00ff00 );
		}

		if((GetGameGuiManager()->m_guiSkill)&&(pItem->GetItemType()==ItemType_SkillUp))
			GetGameGuiManager()->m_guiSkill->RefreshState();

		//We reset our pre delete access key who has the same id with this new item
		//This may be a very danger operation but cause the old reason by server...
		if ( GetGameAccelKeyManager() && (GetGameAccelKeyManager()->m_preDeleteAccessKey.type == GameAccelKeyType_Equipment) )
		{
			if ( pItem->GetThisID() == GetGameAccelKeyManager()->m_preDeleteAccessKey.dwID )
			{
				GetGameAccelKeyManager()->SetKey( GetGameAccelKeyManager()->m_preDeleteAccessKey );
				GetGameAccelKeyManager()->Save( GetGameAccelKeyManager()->m_preDeleteAccessKey.mGroup );

				if ( GetGameGuiManager()->m_guiMain )
					GetGameGuiManager()->m_guiMain->RefreshKeyGroupState( GetGameAccelKeyManager()->m_preDeleteAccessKey.mGroup );
				if ( GetGameGuiManager()->m_guiFloatAcessKey )
					GetGameGuiManager()->m_guiFloatAcessKey->RefreshKeyGroupState();
				if ( GetGameGuiManager()->m_guiOtherAccessKey )
					GetGameGuiManager()->m_guiOtherAccessKey->RefreshKeyGroupState();

			}
		}

	}

}
/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		添加物品，刷新物品数据
*/
void CMainCharacter::AddItem(stAddObjectPropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;
	AddItem(pCmd->object,pCmd->byActionType);
	
	FUNCTION_END;
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param setID : 描述
 * \param setActive : 描述
 * \return 返回值的描述
 */
void CMainCharacter::GetSetActiveByID( const DWORD& setID, stSetObjectPropertyActive& setActive )
{
	FUNCTION_BEGIN;

	if ( g_tableItemSetBase.empty() )
	{
		setActive.Init();
		return;
	}

	//Find cur set
	
	int i=0;
	for( tTableItemSetBase::iterator itor=g_tableItemSetBase.begin();itor!=g_tableItemSetBase.end();itor++ )
	{
		if ( itor->dwID == setID )
			break;
		i++ ;
	}

	if ( itor != g_tableItemSetBase.end() )
	{
		for( int k=0;k<SET_PROPERTY_NUM;k++ )
		{
			setActive.bActive[k] = m_bSetActive[i].bActive[k];
		}
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param setName : 描述
 * \param setActive : 描述
 * \return 返回值的描述
 */
void CMainCharacter::GetSetActiveByName( const char* setName, stSetObjectPropertyActive& setActive  )
{
	FUNCTION_BEGIN;

	if ( g_tableItemSetBase.empty() )
	{
		setActive.Init();
		return;
	}

	//Find cur set

	int i=0;
	for( tTableItemSetBase::iterator itor=g_tableItemSetBase.begin();itor!=g_tableItemSetBase.end();itor++ )
	{
		if ( strcmp( itor->pstrName, setName ) == 0 )
			break;
		i++ ;
	}

	if ( itor != g_tableItemSetBase.end() )
	{
		for( int k=0;k<SET_PROPERTY_NUM;k++ )
		{
			setActive.bActive[k] = m_bSetActive[i].bActive[k];
		}
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMainCharacter::ChangeSetActiveState()
{
	FUNCTION_BEGIN;

	if ( g_tableItemSetBase.empty() )
		return;
	else
	{
		if ( NULL == m_bSetActive )
		{
			m_bSetActive = new stSetObjectPropertyActive[g_tableItemSetBase.size()];
		}
	}

	tTableItemSetBase::iterator		itor;
	int i=0;
	for	( itor=g_tableItemSetBase.begin();itor!=g_tableItemSetBase.end();itor++ )
	{
		int k;
		int setIteNum = 0;
		//First set all inactive
		for( k=0;k<SET_PROPERTY_NUM;k++ )
			m_bSetActive[i].bActive[k] = false;

		//Get cur set item num
		for( k=0;k<7;k++ )		//7 item each set
		{
			CRoleItem* pItem = FindItemByConstID( itor->itemList[k] );
			if ( pItem )
				setIteNum++ ;
		}
		//
		if ( setIteNum >= 7 )
		{
			for( k=0;k<7;k++ )		//7 property active
			{
				m_bSetActive[i].bActive[itor->idList[k]] = true;
			}
		}
		else if( setIteNum >= itor->effectTwoID[0] )
		{
			for( k=0;k<4;k++ )		//4 property active
			{
				m_bSetActive[i].bActive[itor->effectTwoID[k]] = true;
			}
		}
		else if( setIteNum >= itor->effectOneID[0] )
		{
			for( k=0;k<2;k++ )		//2 property active
			{
				m_bSetActive[i].bActive[itor->effectOneID[k]] = true;
			}
		}		
		//
		i++ ;
	}

	FUNCTION_END;
}

DWORD CMainCharacter::FindOtherItemWithSameTypeID( const DWORD& thisID, const DWORD& typeID )
{
	if (this==NULL) return -1;

	tListItem::iterator it;
	for( it=m_listItem.begin(); it!=m_listItem.end(); it++ )
	{
		if ( ((*it)->GetObjectID() == typeID)
			&& ((*it)->GetThisID() != thisID) )
		{
			return (*it)->GetThisID();
		}
	}

	return -1;
}


/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		删除物品
*/
void CMainCharacter::RemoveItem(stRemoveObjectPropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	if (this==NULL) return;

	tListItem::iterator it = FindItemItByID(pCmd->qwThisID);
	Assert( it != m_listItem.end());
	if(it != m_listItem.end())
	{
		DWORD otherItemID = -1;

		int nAutoEquipFillItemType = -1;
		int nEquipPos = 0;

		if ( (*it)->IsEquipment() )
			otherItemID = -1;
		else
			otherItemID = FindOtherItemWithSameTypeID( pCmd->qwThisID, (*it)->GetObjectID() );

		bool bRefreshSkillTree = ( (*it)->GetItemType() == ItemType_SkillUp );

		//If this is a special item we add client msg
		if ( (*it)->GetItemType() == ItemType_Tonic )
		{
			char szMsg[256];
			sprintf( szMsg, "%s用完,自动喝药功能结束", (*it)->GetName() );
			GetGameGuiManager()->AddClientSystemMessage( szMsg );
		}

		if ( GetGameGuiManager()->m_guiShop )
		{
			GetGameGuiManager()->m_guiShop->RemoveItem( (*it)->GetThisID() );
		}

		DWORD dwLocation = (*it)->GetLocation().dwLocation;

		//If need auto equip fill-mp item
		if ( ( ((*it)->GetItemType() == ItemType_FillMP)
				|| ((*it)->GetItemType() == ItemType_Tonic )
				|| ((*it)->GetItemType() == ItemType_FillXHP )) //soke 增加自动补血 72类
			&& (dwLocation == OBJECTCELLTYPE_EQUIP) )
		{
			nAutoEquipFillItemType = (*it)->GetItemType();
			nEquipPos = (*it)->GetLocation().x;
		}

		(*it)->AccessKeyOnRemove( otherItemID );	
		(*it)->OnRemove();		

		m_allocRoleItem.freePointer(*it);
		SimpleRemoveFromArray(m_listItem,it - m_listItem.begin());
		RefreshActiveItem(dwLocation);

		if( GetGameGuiManager()->m_guiSkill && bRefreshSkillTree )
			GetGameGuiManager()->m_guiSkill->RefreshState();

		//Equip auto fill mp item
		if ( nAutoEquipFillItemType > 0 )
			AutoEquipFillItem( nEquipPos, nAutoEquipFillItemType );
		
	}

	//Change set object active state
	ChangeSetActiveState();



	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		删除某一类格子中物品
*/
void CMainCharacter::RemoveItemByLocationType(DWORD dwLocation)
{
	FUNCTION_BEGIN;
	if (this==NULL) return;

	for(tListItem::iterator it = m_listItem.begin();it != m_listItem.end();)
	{
		if((*it)->GetLocation().dwLocation == dwLocation)
		{
			(*it)->OnRemove();
			m_allocRoleItem.freePointer(*it);
			it = m_listItem.erase(it);
		}
		else
		{
			++it;
		}
	}

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		交换两种类型格子中物品
*/
void CMainCharacter::SwapItemByLocationType(DWORD dwSrcLocation,DWORD dwDstLocation)
{
	FUNCTION_BEGIN;
	
	if (this==NULL) return;

	for(tListItem::iterator it = m_listItem.begin();it != m_listItem.end();++it)
	{
		stSwapObjectPropertyUserCmd cmd;
		if((*it)->GetLocation().dwLocation == dwSrcLocation)
		{
			cmd.qwThisID = (*it)->GetThisID();
			cmd.dst.dwLocation = dwDstLocation;
			cmd.dst.dwTableID = 0;
			cmd.dst.x = (*it)->GetLocation().x;
			cmd.dst.y = (*it)->GetLocation().y;
			(*it)->OnSwap(&cmd);
		}
	}

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		交换物品
*/
void CMainCharacter::SwapItem(stSwapObjectPropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;
	
	if (this==NULL) return;

	tListItem::iterator it = FindItemItByID(pCmd->qwThisID);
	Assert( it != m_listItem.end());
	if(it != m_listItem.end())
	{
		(*it)->OnSwap(pCmd);
	}

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		设置物品数量
*/
void CMainCharacter::RefItemCount(stRefCountObjectPropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;
	if (this==NULL) return;

	tListItem::iterator it = FindItemItByID(pCmd->qwThisID);
	if(it != m_listItem.end())
	{
		//Add client msg
		if ( (*it)->GetCount() < pCmd->dwNum )
		{
			if ( !((*it)->GetItemType() == ItemType_Money ) )
			{
				char szMsg[MAX_PATH];
				sprintf( szMsg, "你获得了物品 %s", (*it)->GetName() );
				GetGameGuiManager()->AddClientSystemMessage( szMsg, 0xff00ff00 );
			}
		}		

		(*it)->OnSetCount(pCmd); 				
	}

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 	分裂物品
*/
void CMainCharacter::SplitItem(stSplitObjectPropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;
	if (this==NULL) return;

	tListItem::iterator it = FindItemItByID(pCmd->qwThisID);
	Assert( it != m_listItem.end());
	if(it != m_listItem.end())
	{
		(*it)->OnSplit(pCmd);
	}

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		根据位置查找物品
*/
CMainCharacter::tListItem::iterator 
CMainCharacter::FindItemItByLocation(const stObjectLocation & location)
{
	FUNCTION_BEGIN;

	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
	{
		if((*it)->GetLocation() == location) return it;
	}
	return m_listItem.end();

	FUNCTION_END;
}

CRoleItem * CMainCharacter::FindItemItByLocationType(const DWORD & dwTypeLocation,const enumItemType & type)
{
	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++it )
	{
		if((*it)->GetLocation().dwLocation == dwTypeLocation && (*it)->GetItemType() == type) 
			return (*it);
	}
	return NULL;
} 

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		删除物品
*/
void CMainCharacter::RemoveItem(tItemThisID qwThisID)
{
	FUNCTION_BEGIN;
	
	if (this==NULL) return;

	tListItem::iterator it = FindItemItByID(qwThisID);
	if(it != m_listItem.end())
	{
		(*it)->OnRemove();
		//delete (*it);
		m_allocRoleItem.freePointer(*it);
		m_listItem.erase(it);		
	}

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		删除物品(某一类格子内的 位于ptGrid 的物品)
*/
void CMainCharacter::RemoveItem(DWORD dwLocation,POINT ptGrid)
{
	FUNCTION_BEGIN;
	
	if (this==NULL) return;

	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
	{
		if( (*it)->GetLocation().dwLocation == dwLocation && 
			(*it)->GetLocation().x == ptGrid.x &&  
			(*it)->GetLocation().y == ptGrid.y )
		{
			(*it)->OnRemove();
			m_allocRoleItem.freePointer(*it);
			m_listItem.erase(it);
			return ;
		}
	}

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		NPC 死亡时调用
*/
void CMainCharacter::OnNpcDeath(CNpc* pNpc)
{
	FUNCTION_BEGIN;

	for(tListObtainExp::iterator it = m_listExpCmd.begin();it != m_listExpCmd.end();++it)
	{
		if(it->dwSrcID == pNpc->GetID())
		{
			AddExp(it->dwExp,it->dwAbsExp);
			it = m_listExpCmd.erase(it) - 1 ;
		}
	}

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		增加经验值
*/
#define COLOR_EXP 0xff0000ff
void CMainCharacter::AddExp(QWORD exp,QWORD dwAbsExp)
{
	FUNCTION_BEGIN;

	char sz[128];
	sprintf(sz,"增加经验值 %I64d ",exp);
	if(GetGameGuiManager()->m_guiMain)
	{
		GetGameGuiManager()->m_guiMain->AddMessage(sz,CGuiMain::ExpMessage,COLOR_ARGB(255,128,255,0));//AddExpMessage(sz,COLOR_ARGB(255,128,255,0));
	}

	//*
	stNumColor t;
	t.color = COLOR_ARGB(255,255,252,24);
	t.height = 0;
	t.byIsExp = 1;
	t.scale = stPointF(0.75f,0.75f);
	sprintf(t.szNum,"经验值 %I64d",exp);
	PushBackNumColor(t);
	m_data.exp	= dwAbsExp;	

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		设置生命魔法值
*/
void CMainCharacter::SetHPAndMP(unsigned int nHP,unsigned int nMP,unsigned int nSP)
{
	FUNCTION_BEGIN;

	m_dwHP		= nHP;
	m_data.hp	= nHP;
	m_data.mp	= nMP;
	FUNCTION_END;
}


/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  查找技能
*/
CMainCharacter::tListSkill::iterator CMainCharacter::FindSkillItByID(DWORD dwID)
{
	FUNCTION_BEGIN;

	for(tListSkill::iterator it = m_listSkill.begin(); it != m_listSkill.end(); ++it)
	{
		if((*it)->GetID() == dwID)
		{
			return it;
		}
	}
	return m_listSkill.end();

	FUNCTION_END;
}


/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		增加更新技能
*/
void CMainCharacter::AddSkill(stAddUserSkillPropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	CRoleSkill* pSkill = FindSkillByID(pCmd->dwSkillID);
	if( pSkill == NULL )
	{
		pSkill = new CRoleSkill;
		m_listSkill.push_back(pSkill);
	}
	pSkill->SetAttrib(pCmd);
	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		删除技能
*/
void CMainCharacter::RemoveSkill(stRemoveUserSkillPropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	tListSkill::iterator it = FindSkillItByID(pCmd->dwSkillID);
	if(it != m_listSkill.end())
	{
		m_listSkill.erase(it);
	}

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  删除玩家所有已经学会的技能
*/
void CMainCharacter::RemoveAllSkill()
{
	FUNCTION_BEGIN;

	for(tListSkill::iterator it = m_listSkill.begin(); it != m_listSkill.end(); ++it)
	{
		delete (*it);
	}
	m_listSkill.clear();

	if ( NULL == GetGameGuiManager()->m_guiSkill )
	{
		GetGameGuiManager()->AddSkill();
		if ( GetGameGuiManager()->m_guiSkill )
		{
			GetGameGuiManager()->m_guiSkill->SetVisible( false );	
			
		}
	}
	
	//Clear all skill access key
	if ( GetGameAccelKeyManager() )
	{
		GetGameAccelKeyManager()->DeleteAccessKeyByType( GameAccelKeyType_Skill );
		if( GetGameGuiManager()->m_guiMain )
			GetGameAccelKeyManager()->Save( GetGameGuiManager()->m_guiMain->GetCurAccessKeyGroup() );
		else
			GetGameAccelKeyManager()->Save( 0 );
	}

	GetGameGuiManager()->m_guiSkill->InitRefresh();

	if ( GetGameGuiManager()->m_guiMain )
		GetGameGuiManager()->m_guiMain->RefreshKeyGroupState( GetGameGuiManager()->m_guiMain->GetCurAccessKeyGroup() );

	if ( GetGameGuiManager()->m_guiFloatAcessKey )
		GetGameGuiManager()->m_guiFloatAcessKey->RefreshKeyGroupState();

	
	 
	FUNCTION_END;
}
/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  取得钱的物品对象
*/
CRoleItem* CMainCharacter::GetMoneyItem(DWORD dwLocation)
{
	FUNCTION_BEGIN;

	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++it)
	{
		if((*it)->GetLocation().dwLocation == dwLocation && (*it)->IsMoney())
		{
			return (*it);
		}
	}
	return NULL;

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  是否是可用的技能
*/
bool CMainCharacter::IsRightSkill(SkillBase_t* pSkill)
{
	FUNCTION_BEGIN;

	return true;

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  是否是该角色的技能
*/
bool CMainCharacter::IsMainUserSkill(SkillBase_t* pSkill)
{
	FUNCTION_BEGIN;

	/*if(0 == (pSkill->dwCharType & GetType())) return false;*/
	return true;

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  是否能学习的技能
*/
bool CMainCharacter::IsCanStudySkill(SkillBase_t* pSkill, const int& upLevel, bool bNeedTips/* = false*/ )
{
	FUNCTION_BEGIN;

	if ( NULL == pSkill )
		return false;

	if ( pSkill->dwSerialType == SKILL_SERIAL_SPECIAL )
		return false;
	
	//already not need this condition
	//always return true
	if(!IsRightSkill(pSkill))
		return false;

	//return true;
	
	//see if we has any points
	if ( GetSkillPoint()  <= 0 )
	{
		if ( bNeedTips )
			GetGameGuiManager()->AddClientSystemMessage( "技能点数不足" );
		return false;
	}

	//see if all pre skill has studied
	SkillLevelBase_t* pSkillLevel = GetSkillLevelBase( pSkill->dwID, upLevel );

	if ( NULL == pSkillLevel )
		return false;

	if ( (pSkillLevel->dwNeedSkill3Level > GetLevel())
		|| (pSkillLevel->dwNeedSkill3Level >= OPEN_SKILL_MAX_LEVEL ) )
		return false;

	if ( pSkill->dwNeedSkill1 )
	{
		CRoleSkill* pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( pSkill->dwNeedSkill1 );
		if( pRoleSkill )
		{
			if ( pRoleSkill->GetLevel() < pSkillLevel->dwNeedSkill1Level )
			{
				GetGameGuiManager()->AddClientSystemMessage( "前提技能点数不足！" );
				return false;
			}			
		}
		else
			return false;
	}
	if ( pSkill->dwNeedSkill2 )
	{
		CRoleSkill* pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( pSkill->dwNeedSkill2 );
		if( pRoleSkill )
		{
			if ( pRoleSkill->GetLevel() < pSkillLevel->dwNeedSkill2Level )
				return false;
		}
		else
			return false;
	}

	//see if the tree level is ok
	//Get skill at tree points
	int treeSkillPoints = 0xffff;
	if ( NULL == GetGameGuiManager()->m_guiSkill )
	{
		GetGameGuiManager()->AddSkill();
		if ( GetGameGuiManager()->m_guiSkill )
			GetGameGuiManager()->m_guiSkill->SetVisible( false );
	}

	if ( GetGameGuiManager()->m_guiSkill )
		treeSkillPoints = GetGameGuiManager()->m_guiSkill->GetSerialSkillPoints( pSkillLevel->dwSerialType, pSkillLevel->dwTreeType );	

	if ( treeSkillPoints < pSkillLevel->dwSkillPoint )
	{
		if ( bNeedTips )
			GetGameGuiManager()->AddClientSystemMessage( "前提技能点数不足！" );

		return false;
	}
	//技能书
	if (pSkillLevel->dwUpItemID > 0)
	{
		if (this->GetItemCount(pSkillLevel->dwUpItemID, OBJECTCELLTYPE_COMMON) <= 0)
		{
			if ( bNeedTips )
			{
				char szMsg[MAX_PATH];
				ObjectBase_t* pItem = GetObjectBase( pSkillLevel->dwUpItemID );
				if ( pItem )
					sprintf( szMsg, "需要秘籍 %s", pItem->strName );
				else
					sprintf( szMsg, "需要秘籍" );

				GetGameGuiManager()->AddClientSystemMessage( szMsg );
			}			

			return false;
		}
	}

	return true;

	FUNCTION_END;
}

int CMainCharacter::GetSkillPrePoints( SkillBase_t* pSkill )
{
	if ( NULL == pSkill )
		return 0;
	
	//get pre skill pre points
	SkillLevelBase_t* pSkillLevel = GetSkillLevelBase( pSkill->dwID, pSkill->dwLevel );

	if ( NULL == pSkillLevel )
		return 0;

	int total = 0;

	if ( pSkill->dwNeedSkill1 )
	{
		CRoleSkill* pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( pSkill->dwNeedSkill1 );
		if( pRoleSkill )
		{
			total += pRoleSkill->GetLevel();
			//
			SkillBase_t* pPreSkill = GetSkillBase( pSkill->dwNeedSkill1 );
			if ( pPreSkill )
				total += GetSkillPrePoints( pPreSkill );
		}
	}
	if ( pSkill->dwNeedSkill2 )
	{
		CRoleSkill* pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( pSkill->dwNeedSkill2 );
		if( pRoleSkill )
		{
			total += pRoleSkill->GetLevel();
			//
			SkillBase_t* pPreSkill = GetSkillBase( pSkill->dwNeedSkill1 );
			if ( pPreSkill )
				total += GetSkillPrePoints( pPreSkill );
		}
	}
	if ( pSkill->dwNeedSkill3 )
	{
		CRoleSkill* pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( pSkill->dwNeedSkill3 );
		if( pRoleSkill )
		{
			total += pRoleSkill->GetLevel();
			//
			SkillBase_t* pPreSkill = GetSkillBase( pSkill->dwNeedSkill1 );
			if ( pPreSkill )
				total += GetSkillPrePoints( pPreSkill );
		}
	}

	return total;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pSkill : 描述
 * \return 返回值的描述
 */
DWORD CMainCharacter::GetUpLevel(SkillLevelBase_t* pSkill)
{
	FUNCTION_BEGIN;

	int nSkillPos = 0;
	DWORD dwUpLevel = -1;

	/*if (pSkill)
	{
		SkillBase_t* pSkillBase = GetSkillBase(pSkill->dwID);
		enumFiveType* aFive = g_aMainFive[GetMainFive()];
		for(int i = 0;i<FIVETYPENUM;++i)
		{
			if(aFive[i] == pSkillBase->dwFive)
			{
				nSkillPos = i;
				break;
			}
		}

		switch(nSkillPos) {
		case 1:
			dwUpLevel = pSkill->dwUpLevel1;
			break;
		case 2:
			dwUpLevel = pSkill->dwUpLevel2;
			break;
		default:
			dwUpLevel = pSkill->dwUpLevel;
		}
	}*/
	return dwUpLevel;


	FUNCTION_END;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param five : 描述
 * \return 返回值的描述
 */
int CMainCharacter::GetFivePoint(enumFiveType five)
{
	FUNCTION_BEGIN;

	if(five >= 5) five = (enumFiveType)0;
	//return m_data.fivePoint[five];
	return 0;
	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  获得界面显示的命中值
*/
int CMainCharacter::GetShowHitValue()
{
	FUNCTION_BEGIN;
	return m_data.attackrating;

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		   获得界面显示的躲避值
*/
int CMainCharacter::GetShowDodgeValue()
{
	FUNCTION_BEGIN;
	return m_data.attackdodge;

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  获得主修三项五行的数值
*/
int CMainCharacter::GetMainFiveItem(int index)
{
	FUNCTION_BEGIN;

	return g_aMainFive[GetMainFive()][index];

	FUNCTION_END;
}



/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ptMapOffset : 描述
 * \return 返回值的描述
 */
void CMainCharacter::AddLight(POINT ptMapOffset)
{
	FUNCTION_BEGIN;

	m_pCharacter->AddLight(ptMapOffset);
	GetLightMap()->AddLight(GetPos()-ptMapOffset,GetScene()->GetLightRadius(),-1);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
bool CMainCharacter::OnSetCursor()
{
	FUNCTION_BEGIN;
	
	if ( (GetGameCursor()->GetCursorImageType() == eCursorUseAccess)
		||( GetGameCursor()->GetCursorImageType() == eCursorShowUser )
		||( GetGameCursor()->GetCursorImageType() == eCursorFollowUser )
		||( GetGameCursor()->GetCursorImageType() == eCursorTrade )
		||( GetGameCursor()->GetCursorImageType() == eCursorFreeMove)
		||( GetGameCursor()->GetCursorImageType() == eCursorPetAttack) 
		||( GetGameCursor()->GetCursorTypeEx() == eCursorGetMailItem) )
		return false;

	GetGameCursor()->SetCursor(CCursor::CursorType_Normal,eCursorNormal);
	return true;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param speed : 描述
 * \return 返回值的描述
 */
void CMainCharacter::SetMoveSpeed(int speed)
{
	FUNCTION_BEGIN;
	
	m_moveSpeed = speed;
	if(speed != m_RunEvent.speed)
	{
		m_AStar.FreePath();
		m_RunEvent.speed = speed;
	}

	FUNCTION_END;
}


/**
 * \brief 跳转到指定位置
 * 
 * 
 * 
 * \param ptGrid : 指定的位置
 * \return 成功则返回true 否则返回false
 */
bool CMainCharacter::Goto(POINT ptGrid,tOnGotoFinalEventFunc pEventFunc,void* pParam)
{
	FUNCTION_BEGIN;

	// 2010 9
	if( g_bAutoGo )
	{
		g_bAutoGo = false;
	}
	else
	{
		g_sFrom = "";
	}

	if( bIsShopStart() )
	{
		GetGameGuiManager()->AddClientSystemMessage("摆摊情况下不可以移动");
		return false;
	}

	if( isset_state(GetState(),USTATE_CRAZY_ACTION)	|| isset_state(GetState(),USTATE_CRAZY) )
		return false;


//wen
	ptDest.x = ptGrid.x * GRID_WIDTH;
	ptDest.y = ptGrid.y * GRID_HEIGHT;
//wen
	ClearRunEvent();
	ClearPath();
	m_RunEvent.target = eTargetType_None;
	m_RunEvent.pt.x = ptGrid.x * GRID_WIDTH;
	m_RunEvent.pt.y = ptGrid.y * GRID_HEIGHT;
	m_RunEvent.event = eEventType_Move;
	m_RunEvent.speed = 2;

	m_InputState.bGM = true;
	m_InputState.bReFindPath = false;
	//Make gui main run btn visible
	if ( m_moveSpeed < 2 )
	{
		if ( GetGameGuiManager()->m_guiMain )
			GetGameGuiManager()->m_guiMain->OnOffWalkRunBtn();
	}	
	m_pGotoFinalEvent = pEventFunc;
	m_pGotoFinalParam = pParam;
	return true;

	FUNCTION_END;
}

void CMainCharacter::UseSkillOnCharacter( CNpc* pNpc )
{
	if ( isset_state(GetState(),USTATE_CRAZY_ACTION)
		|| isset_state(GetState(),USTATE_CRAZY) )
		return;

	if ( NULL == pNpc )
		return;

	if ( NULL == GetGameGuiManager()->m_guiTeam )
		return ;

	if ( GetGameGuiManager()->m_guiTeam->GetActiveSkill() == NULL )
		return;

	if ( pNpc->IsMainRole() )		//Self use
	{
		if ( GetGameGuiManager()->m_guiTeam->GetActiveSkill()->dwTarget & SkillTarget_Self )
			OnSelectSkill( GetGameGuiManager()->m_guiTeam->GetActiveSkill()->dwID );
	}
	else
	{
		if ( pNpc->IsPet() )		//Use on pet ( my or friend's)		
		{
			if ( ( (GetGameGuiManager()->m_guiTeam->GetActiveSkill()->dwTarget & SkillTarget_Pet) && (pNpc->GetPetType() == PET_TYPE_PET) )
				|| ( (GetGameGuiManager()->m_guiTeam->GetActiveSkill()->dwTarget & SkillTarget_Summon) && (pNpc->GetPetType() == PET_TYPE_SUMMON) ) )
			{
				stRunEvent runEvent;
				runEvent.id = pNpc->GetID();
				runEvent.target = pNpc->IsCharacter() ? eTargetType_Character : eTargetType_Npc;
				runEvent.pSkill = GetGameGuiManager()->m_guiTeam->GetActiveSkill();
				runEvent.pt = pNpc->GetPos();
				runEvent.speed = 0;
				runEvent.event = eEventType_Attack;

				SetRunEvent( runEvent );
			}	
		}
		else
		{
			if ( pNpc->IsCharacter() && (static_cast<CCharacter*>(pNpc))->IsMyFriend() )	//Is friend
			{
				if ( (GetGameGuiManager()->m_guiTeam->GetActiveSkill()->dwTarget & SkillTarget_Friend)
					|| (GetGameGuiManager()->m_guiTeam->GetActiveSkill()->dwID == SKILL_RELIVE_FRIEND) )
				{
					stRunEvent runEvent;
					runEvent.id = pNpc->GetID();
					runEvent.target = pNpc->IsCharacter() ? eTargetType_Character : eTargetType_Npc;
					runEvent.pSkill = GetGameGuiManager()->m_guiTeam->GetActiveSkill();
					runEvent.pt = pNpc->GetPos();
					runEvent.speed = 0;
					runEvent.event = eEventType_Attack;

					SetRunEvent( runEvent );
				}
			}
			else
			{
				stRunEvent runEvent;
				runEvent.id = pNpc->GetID();
				runEvent.target = pNpc->IsCharacter() ? eTargetType_Character : eTargetType_Npc;
				runEvent.pSkill = GetGameGuiManager()->m_guiTeam->GetActiveSkill();
				runEvent.pt = pNpc->GetPos();
				if ( (runEvent.pSkill->dwTarget & SkillTarget_Death) && (runEvent.pSkill->dwDis == 1) )
					runEvent.speed = GetMoveSpeed();
				else
					runEvent.speed = 0;
				runEvent.event = eEventType_Attack;

				SetRunEvent( runEvent );
			}
		}
	}

	RestoreMouseIcon();
}

bool CMainCharacter::bCanChangeClothColor()
{
	return true;

}

int  CMainCharacter::GetSavingBoxNum()
{
	int num = m_savingBox.m_num; 
	Debug_Int(num);
	if( (m_data.bitmask & CHARBASE_VIP ) && num > 0 )
	{
		Debug_Int(m_data.bitmask);
		return num - 1; 
	}
	return num; 
}

//-------------------------------------------------------
//
//Add By David For 2006-3-9
void CMainCharacter::RenderGridLine( )
{
	FUNCTION_BEGIN;
	if( NULL == GetScene( ) || NULL == GetDevice( ) )
	{
		return;
	}

	//
	POINT ptRightTopPos			= GetGridPos();
	POINT ptMapPixelPos			= GetScene()->GetPixelOffset( );

	int iScreenGridX	= ptRightTopPos.x - ptRightTopPos.x % 13 - 1 ;
	int iScreenGridY	= ptRightTopPos.y - ptRightTopPos.y % 19 - 1 ;
	int iSrcPosX		= iScreenGridX * GRID_WIDTH  - ptMapPixelPos.x ;
	int	iSrcPosY		= iScreenGridY * GRID_HEIGHT - ptMapPixelPos.y ;

	GetDevice()->DrawLine( iSrcPosX , 0, iSrcPosX, GetDevice()->GetHeight() ,D3DCOLOR_ARGB(255, 120, 255, 0) );
	GetDevice()->DrawLine( iSrcPosX + GRID_WIDTH * 13, 0, iSrcPosX + GRID_WIDTH * 13, GetDevice()->GetHeight() ,D3DCOLOR_ARGB(255, 120, 255, 0) );
	GetDevice()->DrawLine( 0, iSrcPosY, GetDevice()->GetWidth(), iSrcPosY,D3DCOLOR_ARGB(255, 120, 255, 0) );
	GetDevice()->DrawLine( 0 , iSrcPosY + GRID_HEIGHT * 19, GetDevice()->GetWidth(), iSrcPosY + GRID_HEIGHT * 19 ,D3DCOLOR_ARGB(255, 120, 255, 0) );

	FUNCTION_END;
}