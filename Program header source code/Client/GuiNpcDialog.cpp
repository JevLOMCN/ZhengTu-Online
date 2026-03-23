
/**
*\file		GuiNpcDialog.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    NPC对话框
*
* 
*
*/

#include "public.h"
#include "miniusercommand.h"
#include "./Game.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GameGuiManager.h"
#include ".\guinpcdialog.h"
#include ".\guinpctrade.h"
#include ".\guisalarytrade.h"   //工资
#include ".\guiTickettrade.h"   //积分
#include ".\guiChongzhitrade.h" //充值点
#include "./GameState.h"
#include "./GuiSkill.h"
#include "./GuiWorldConn.h"
#include "./GuiTaskDialog.h"
#include "./GuiNpcMake.h"
#include "./Country.h"
#include "./CountryInfo.h"
#include "./GuiStock.h"
#include "./GuiUserInfo.h"
#include ".\GuiInputBox2Dialog.h"
#include "./GuiNameMoneyInputBoxDialog.h"
#include "./DlgProgressBar.h"
#include "./Dlg2ProgressBar.h"
#include "../Media/SoundManager.h"
#include "../luabind/public.h"
#include "guisavebox.h"
#include "./GuiMain.h"
#include "./game_script.h"
#include "./RanksMgr.h"
#include "./GuiTop.h"
#include "./GuiTaiMiaoTop.h"
#include "./GuiHalofFameTop.h"
//功勋竞猜
#include "./Guigongxun.h"
//理财
#include "./GuiLicai.h"
//天下第一
#include "./GuiTianxia.h"
//战车列表
#include "./GuiZhanchelist.h"
//家族BOSS
#include "./GuiJiazuboss.h"

#ifdef _DEBUG
	#define _DEBUG_DLG
#else
	#undef _DEBUG_DLG
#endif 

//#ifdef _DEBUG_DLG  // 发布本必须去掉warning
	#include "LordStrikeManager.h"
	#include "LordStrikeLobby.h"
	#include "LSScoreDlg.h"
	#include "SafetyUser.h"
//#endif 

const int c_nMaxOffsetWithNpc = 5;


static void AddSetNpcScriptCommand(const char* script)
{
	SetFrameAllocator fa;

	stVisitNpcTradeUserCmd * p = (stVisitNpcTradeUserCmd*)FrameAllocator::alloc(sizeof(stVisitNpcTradeUserCmd) + strlen(script));
	constructInPlace(p);
	p->byReturn = 1;
	strcpy(p->menuTxt,script);
	GetClient()->PushCommand(p,sizeof(stVisitNpcTradeUserCmd) + strlen(script));
}

void CGuiNpcDialog::bind_npc_dlg_lua(lua_State* L)
{
	using namespace luabind;
	module(L)
	[
		def("AddSetNpcScriptCommand",&AddSetNpcScriptCommand),
		class_<CGuiNpcDialog>("CGuiNpcDialog")
#ifdef __SPRING__
			.def("AddSpringCmdItem",&CGuiNpcDialog::AddSpringCmdItem)
#endif
			.def("AddApplyConn",&CGuiNpcDialog::AddApplyConn)
			.def("AddTalk",&CGuiNpcDialog::AddTalk)
			// [Conglin modify]
			.def("AddSalaryDialog",&CGuiNpcDialog::AddSalaryDialog) //领取工资
			.def( "AddRenascenceDlg", &CGuiNpcDialog::AddRenascenceDlg )
			.def( "AddGraceChangeToExp", &CGuiNpcDialog::AddGraceChangeToExp )
			.def( "AddSeptMPBuff", &CGuiNpcDialog::AddSeptMPBuff )
			.def( "AddEnterBattle", &CGuiNpcDialog::AddEnterBattle )
			.def( "AddChangeCountry", &CGuiNpcDialog::AddChangeCountry )
			.def( "AddChangeCountryBySalary", &CGuiNpcDialog::AddChangeCountryBySalary )
			// [Conglin]

			// [sky]
			.def( "AddGoToMaleHotspringItem", &CGuiNpcDialog::AddGoToMaleHotspringItem )
			.def( "AddGoToFemaleHotspringItem", &CGuiNpcDialog::AddGoToFemaleHotspringItem )
			.def( "AddExitHotspringItem", &CGuiNpcDialog::AddExitHotspringItem)

			// [sky]训马师
			.def( "AddHorseTraining_one", &CGuiNpcDialog:: AddHorseTraining_one )
			.def( "AddHorseTraining_two", &CGuiNpcDialog::AddHorseTraining_two )
			.def( "AddHorseTraining_three", &CGuiNpcDialog::AddHorseTraining_three )
			.def( "ClearHorseTraining", &CGuiNpcDialog::ClearHorseTraining )
			.def( "AddRenascenceDlg", &CGuiNpcDialog::AddRenascenceDlg ) //soke 转生

			// [sky]大臣
			.def( "ExchangeExp", &CGuiNpcDialog::ExchangeExp)
			// [sky end]

			.def("AddGiveUnionMoney",&CGuiNpcDialog::AddGiveUnionMoney)
			.def("AddQueryUnionMoney",&CGuiNpcDialog::AddQueryUnionMoney)
			.def("AddStock",&CGuiNpcDialog::AddStock)
			.def("PlaySound",&CGuiNpcDialog::PlaySound)
			.def("AddWarOperator",&CGuiNpcDialog::AddWarOperator)
			.def("AddCountryWarQuery",&CGuiNpcDialog::AddCountryWarQuery)
			.def("AddChoiceWarField",&CGuiNpcDialog::AddChoiceWarField)
			.def("AddChangeWarField",&CGuiNpcDialog::AddChangeWarField)
			.def("AddChangeCountry",&CGuiNpcDialog::AddChangeCountry)
			.def("AddDialog",&CGuiNpcDialog::AddDialog)
			.def("AddButtonItem",&CGuiNpcDialog::AddButtonItem)
			.def("AddTradeItem",&CGuiNpcDialog::AddTradeItem)
			.def("AddManagerSalaryDlg",&CGuiNpcDialog::AddManagerSalaryDlg) //soke 工资购买
			.def("AddManagerTicketDlg",&CGuiNpcDialog::AddManagerTicketDlg) //soke 积分点数购买
			.def("AddManagerChongzhiDlg",&CGuiNpcDialog::AddManagerChongzhiDlg) //soke 充值点数购买

			.def("AddGoodsStore",&CGuiNpcDialog::AddGoodsStore)
			.def("AddTradeRepairItem",&CGuiNpcDialog::AddTradeRepairItem)
			.def("AddTradeRepairItemByGold",&CGuiNpcDialog::AddTradeRepairItemByGold)
			.def("AddChangeItem",&CGuiNpcDialog::AddChangeItem)
			.def("AddMakeItem",&CGuiNpcDialog::AddMakeItem)
			.def("AddMakeFood",&CGuiNpcDialog::AddMakeFood)
			.def("AddMakeDrug",&CGuiNpcDialog::AddMakeDrug)
			.def("AddBackoutItem",&CGuiNpcDialog::AddBackoutItem)
			.def("AddUpItem",&CGuiNpcDialog::AddUpItem)
			.def("AddKaChangeItem",&CGuiNpcDialog::AddKaChangeItem)
			.def("AddKaMakeItem",&CGuiNpcDialog::AddKaMakeItem)
			.def("AddHoleMakeItem",&CGuiNpcDialog::AddHoleMakeItem)
			.def("AddTong",&CGuiNpcDialog::AddTong)
			.def("AddSept",&CGuiNpcDialog::AddSept)
			.def("AddTongBallot",&CGuiNpcDialog::AddTongBallot)
			.def("AddSeptBallot",&CGuiNpcDialog::AddSeptBallot)
			.def("AddSchool",&CGuiNpcDialog::AddSchool)
			.def("AddMarry",&CGuiNpcDialog::AddMarry)
			.def("AddGiveGift",&CGuiNpcDialog::AddGiveGift)

			.def("AddRecoSys_A_Input",&CGuiNpcDialog::AddRecoSys_A_Input)
			.def("AddRecoSys_B_Query_Take",&CGuiNpcDialog::AddRecoSys_B_Query_Take)
			.def("AddRecoSys_A_Query_Take",&CGuiNpcDialog::AddRecoSys_A_Query_Take)
			
			.def("AddLeaveMarry",&CGuiNpcDialog::AddLeaveMarry)
			.def("AddCompetitionNpc",&CGuiNpcDialog::AddCompetitionNpc)
			.def("DestroyTong",&CGuiNpcDialog::DestroyTong)
			.def("DestroySept",&CGuiNpcDialog::DestroySept)
			.def("DestroySchool",&CGuiNpcDialog::DestroySchool)
			.def("AddDialogItem",&CGuiNpcDialog::AddDialogItem)
			.def("AddCloseItem",&CGuiNpcDialog::AddCloseItem)
			.def("Action",&CGuiNpcDialog::Action)
			.def("BeginAddDialog",&CGuiNpcDialog::BeginAddDialog)
			.def("EndAddDialog",&CGuiNpcDialog::EndAddDialog)
			.def("EndDialog",&CGuiNpcDialog::EndDialog)
			.def("AddTaskCmdItem",&CGuiNpcDialog::AddTaskCmdItem)
			.def("AddCountryStrategy",&CGuiNpcDialog::AddCountryStrategy)			
			.def("AddHairDialog",&CGuiNpcDialog::AddHairDialog)
			.def("AddHeadListDialog",&CGuiNpcDialog::AddHeadListDialog)
			.def("AddClothColorDialog",&CGuiNpcDialog::AddClothColorDialog)
			.def("AddLaunchAttack",&CGuiNpcDialog::AddLaunchAttack)
			.def("IsAttackPop",&CGuiNpcDialog::IsAttackPop)
			.def("OnActionBegin",&CGuiNpcDialog::OnActionBegin)
			.def("OnActionEnd",&CGuiNpcDialog::OnActionEnd)
			.def("AddScriptButton",&CGuiNpcDialog::AddScriptButton)
			.def("CloseNpcDialog",&CGuiNpcDialog::CloseNpcDialog)
			.def("ResetSavingBoxOwnNum",&CGuiNpcDialog::ResetSavingBoxOwnNum)
			.def("SendTaskCmd",&CGuiNpcDialog::SendTaskCmd)
			.def("PlaySound",&CGuiNpcDialog::PlaySound)
			.def("AddeSaleHorseCmd",&CGuiNpcDialog::AddeSaleHorseCmd)
			.def("ShowSavingBox",&CGuiNpcDialog::ShowSavingBox)
			.def("GetMainSkillType",&CGuiNpcDialog::GetMainSkillType)	
			.def("GetLevel",&CGuiNpcDialog::GetLevel)
			.def("IsHasHorse",&CGuiNpcDialog::IsHasHorse)
			.def("IsHaveNewMail",&CGuiNpcDialog::IsHaveNewMail)		
			.def("SellHorse",&CGuiNpcDialog::SellHorse)
			.def("ResetHorseOwnStatus",&CGuiNpcDialog::ResetHorseOwnStatus)
			.def("GetSavingBoxNum",&CGuiNpcDialog::GetSavingBoxNum)
			.def("GetRealSavingBoxNum",&CGuiNpcDialog::GetRealSavingBoxNum)
			.def("AddBuySavingBoxCmd",&CGuiNpcDialog::AddBuySavingBoxCmd)
			.def("IsCanBuyFirstSavingBox",&CGuiNpcDialog::IsCanBuyFirstSavingBox)
			.def("IsCanBuy2ndSavingBox",&CGuiNpcDialog::IsCanBuy2ndSavingBox)
			.def("IsCanBuy3rdSavingBox",&CGuiNpcDialog::IsCanBuy3rdSavingBox)
			.def("AddBuyItemCmd",&CGuiNpcDialog::AddBuyItemCmd)
			.def("AddSeptAttackNpc",&CGuiNpcDialog::AddSeptAttackNpc)
			.def("AddSeptNpcCustom",&CGuiNpcDialog::AddSeptNpcCustom)
			.def("AddTongAttackTown",&CGuiNpcDialog::AddTongAttackTown)
			.def("AddTongTownCustom",&CGuiNpcDialog::AddTongTownCustom)
			.def("AddEmperorRequestDailyMoney",&CGuiNpcDialog::AddEmperorRequestDailyMoney)
			.def("AddKingRequestDailyMoney",&CGuiNpcDialog::AddKingRequestDailyMoney) //soke 国王领取薪资
			.def("AddYsRequestDailyMoney",&CGuiNpcDialog::AddYsRequestDailyMoney)   //soke 元帅领取薪资
			.def("AddZxRequestDailyMoney",&CGuiNpcDialog::AddZxRequestDailyMoney)   //soke 宰相领取薪资
			.def("AddCrRequestDailyMoney",&CGuiNpcDialog::AddCrRequestDailyMoney)   //soke 捕头（左）领取薪资
			.def("AddDtRequestDailyMoney",&CGuiNpcDialog::AddDtRequestDailyMoney)   //soke 御史（左）领取薪资
			.def("AddCxRequestDailyMoney",&CGuiNpcDialog::AddCxRequestDailyMoney)   //soke 捕头（右）领取薪资
			.def("AddDxRequestDailyMoney",&CGuiNpcDialog::AddDxRequestDailyMoney)   //soke 御史（右）领取薪资
			.def("AddSeptTownExp",&CGuiNpcDialog::AddSeptTownExp)
			.def("AddeHonorToPk",&CGuiNpcDialog::AddeHonorToPk)
			.def("AddSendMailDialog",&CGuiNpcDialog::AddSendMailDialog)
			.def("AddRecvMailDialog",&CGuiNpcDialog::AddRecvMailDialog)
			.def("AddAuctionDialog",&CGuiNpcDialog::AddAuctionDialog)
			.def("AddGoldExchangeDialog",&CGuiNpcDialog::AddGoldExchangeDialog)
			.def("AddBuyMonthCardCmd",&CGuiNpcDialog::AddBuyMonthCardCmd)
			.def("AddModifyRevenue",&CGuiNpcDialog::AddModifyRevenue)
			.def("AddQueryRevenue",&CGuiNpcDialog::AddQueryRevenue)
			.def("AddCountryBoxGuard",&CGuiNpcDialog::AddCountryBoxGuard)
			.def("AddCountryRevenue",&CGuiNpcDialog::AddCountryRevenue)
			.def("GetHorseID",&CGuiNpcDialog::GetHorseID)
			.def("AddCheckValidQuestCmd",&CGuiNpcDialog::AddCheckValidQuestCmd)
			.def("AddRecommend",&CGuiNpcDialog::AddRecommend)
			.def("AddTechResearcher",&CGuiNpcDialog::AddTechResearcher)
			.def("AddTechUpdate",&CGuiNpcDialog::AddTechUpdate)
			.def("AddTechBallot",&CGuiNpcDialog::AddTechBallot)
			.def("CreateArmy",&CGuiNpcDialog::CreateArmy)
			.def("LeaveArmy",&CGuiNpcDialog::LeaveArmy)
			.def("QueryArmy",&CGuiNpcDialog::QueryArmy)
			.def("RecruitArmyer",&CGuiNpcDialog::RecruitArmyer)
			.def("ArmymenLevel",&CGuiNpcDialog::ArmymenLevel)
			.def("KingStopOneTalk",&CGuiNpcDialog::KingStopOneTalk)
			.def("GetCartoonPetNum",&CGuiNpcDialog::GetCartoonPetNum)
			.def("AddBuyCartoonPetCmd",&CGuiNpcDialog::AddBuyCartoonPetCmd)
			.def("AddSelectMarriageTypeCmd",&CGuiNpcDialog::AddSelectMarriageTypeCmd)
			.def("AddGoldGiveCmd",&CGuiNpcDialog::AddGoldGiveCmd)
			.def("AddSaleCartoonPetCmd",&CGuiNpcDialog::AddSaleCartoonPetCmd)
			.def("ConfirmSaleCartoonPet",&CGuiNpcDialog::ConfirmSaleCartoonPet)
			.def("AddDialogItemEx",&CGuiNpcDialog::AddDialogItemEx) //正常副本
			.def("AddFubenOpen",&CGuiNpcDialog::AddFubenOpen) //副本开启
			.def("AddFubenEnter",&CGuiNpcDialog::AddFubenEnter) //副本进入
			.def("AddFubenLeave",&CGuiNpcDialog::AddFubenLeave) //副本退出
			.def("AddJiaZuOpen",&CGuiNpcDialog::AddJiaZuOpen) //家族地图
			.def("AddJiaZuBoss",&CGuiNpcDialog::AddJiaZuBoss) //家族BOSS

			.def("TranslateToMoreExp",&CGuiNpcDialog::TranslateToMoreExp)
			.def("AntiDareCountry",&CGuiNpcDialog::AntiDareCountry)
			.def("QuerySeptAndUnionSort",&CGuiNpcDialog::QuerySeptAndUnionSort)
			.def("AddQuestGoldGiveNum",&CGuiNpcDialog::AddQuestGoldGiveNum)
			.def("AddQuestDragonSprite",&CGuiNpcDialog::AddQuestDragonSprite)
			.def("AddQuestTigerSprite",&CGuiNpcDialog::AddQuestTigerSprite)
			.def("SeptGiveupOccupy",&CGuiNpcDialog::SeptGiveupOccupy)
			.def("UnionGiveupOccupy",&CGuiNpcDialog::UnionGiveupOccupy)
			.def("AddSendOutPrisonCmd",&CGuiNpcDialog::AddSendOutPrisonCmd)
			.def("AddOpenBribeInputBox",&CGuiNpcDialog::AddOpenBribeInputBox)
			.def("AddOpenBailPersonInputBox",&CGuiNpcDialog::AddOpenBailPersonInputBox)
			.def("AddOpenBribeInputBox",&CGuiNpcDialog::AddOpenBribeInputBox)
			.def("RequestGiftList",&CGuiNpcDialog::RequestGiftList)
			.def("AddRequestItemCardForGift",&CGuiNpcDialog::AddRequestItemCardForGift)
			.def("AddRequestAllyCountry",&CGuiNpcDialog::AddRequestAllyCountry)
			.def("AddCancelAllyCountry",&CGuiNpcDialog::AddCancelAllyCountry)
			.def("AddRequestAllyInfo",&CGuiNpcDialog::AddRequestAllyInfo)
			.def("AddLeavePrisonCmd",&CGuiNpcDialog::AddLeavePrisonCmd)
			.def("AddImmureDlg",&CGuiNpcDialog::AddImmureDlg)
			.def("AddQueryGenInfo",&CGuiNpcDialog::AddQueryGenInfo)
			.def("AddInputDialog",&CGuiNpcDialog::AddInputDialog)	
			.def("SendGoTrainUserCmd",&CGuiNpcDialog::SendGoTrainUserCmd)
			.def("SendGoFubenUserCmd",&CGuiNpcDialog::SendGoFubenUserCmd)
            .def("AdFubenjhGenInfo",&CGuiNpcDialog::AdFubenjhGenInfo)
			.def("AddAppointMember",&CGuiNpcDialog::AddAppointMember)	
			.def("AddShowCountryWeak3",&CGuiNpcDialog::AddShowCountryWeak3)
			.def("AddQuestOccupierList",&CGuiNpcDialog::AddQuestOccupierList)
		    .def("AddRequestSeptNormalExp",&CGuiNpcDialog::AddRequestSeptNormalExp)

			.def("AddReqMasterBounty",&CGuiNpcDialog::AddReqMasterBounty)
			.def("AddReqPrenticeBounty",&CGuiNpcDialog::AddReqPrenticeBounty)

			.def("AddDaojuGetBtn",&CGuiNpcDialog::AddDaojuGetBtn)
			
			//soke 改造
			.def("AddModifyAttri",&CGuiNpcDialog::AddModifyAttri)
			.def("AddReBind",&CGuiNpcDialog::AddReBind)
			.def("AddReMaker",&CGuiNpcDialog::AddReMaker)
			.def("AddAddMaker",&CGuiNpcDialog::AddAddMaker)
			.def("AddModifySoulA",&CGuiNpcDialog::AddModifySoulA)
			.def("AddLVSoullineMD",&CGuiNpcDialog::AddLVSoullineMD)
			.def("AddModifyDxSoulA",&CGuiNpcDialog::AddModifyDxSoulA)
			.def("AddMDSkillA",&CGuiNpcDialog::AddMDSkillA)
			.def("AddEquipLV",&CGuiNpcDialog::AddEquipLV)
			.def("AddEquipRound",&CGuiNpcDialog::AddEquipRound)
			.def("AddSalaryDialog",&CGuiNpcDialog::AddSalaryDialog) //soke  领取工资
			.def("OpenHorseEquipAttributeModify",&CGuiNpcDialog::OpenHorseEquipAttributeModify)
			.def("AddChouJiang",&CGuiNpcDialog::AddChouJiang) //soke  抽奖
			// 2010 7
			//soke 太庙和温泉等
			.def("RecvTMTask",&CGuiNpcDialog::RecvTMTask)                   //拆除太庙
			.def("ReTTMTask",&CGuiNpcDialog::ReTTMTask)                     //建设太庙
			.def("AddViewTMListBoard",&CGuiNpcDialog::AddViewTMListBoard)   //查看资源排行榜
			.def("ExcRecvTMTask",&CGuiNpcDialog::ExcRecvTMTask)             //更换资源
			.def( "AddGoToMaleHotspringItem", &CGuiNpcDialog::AddGoToMaleHotspringItem )
			.def( "AddGoToFemaleHotspringItem", &CGuiNpcDialog::AddGoToFemaleHotspringItem )
			.def( "AddExitHotspringItem", &CGuiNpcDialog::AddExitHotspringItem)
			//功勋竞猜
			.def( "AddExpGame", &CGuiNpcDialog::AddExpGame)
			.def( "AddForTuneFundDlg", &CGuiNpcDialog::AddForTuneFundDlg)
			.def( "AddTianxiaDlg", &CGuiNpcDialog::AddTianxiaDlg)//天下第一
			.def( "StartBikeGame", &CGuiNpcDialog::StartBikeGame)//申请加入自行车大赛
			.def( "DeleteBikeRance", &CGuiNpcDialog::DeleteBikeRance)//申请加入自行车大赛
			.def( "NextBikeGame", &CGuiNpcDialog::NextBikeGame)//访问赛车女郎
			.def( "AddZhanCheListDlg",&CGuiNpcDialog::AddZhanCheListDlg)
			.def( "AddMiGong", &CGuiNpcDialog::AddMiGong)
			.def( "AddTianxiaLingquDlg", &CGuiNpcDialog::AddTianxiaLingquDlg)//天下第一
	];
}
/**
 * \brief 构造函数
 * 
 * 构造函数
 * 
 * \return 无
 */
CGuiNpcDialog::CGuiNpcDialog(void)
: m_state(STATE_NORMAL)
{
	FUNCTION_BEGIN;

	m_dwNpcId = 0;
	m_pNpcScript = NULL;
	m_bSmDialog = false;
	m_guiTalk = NULL;
	m_iActionId = 0;
	m_bEndAction = false;

	m_pMsgDlg = NULL;
	FUNCTION_END;
}

CGuiNpcDialog::~CGuiNpcDialog()
{
	SAFE_DELETE(m_pNpcScript);
}

/**
 * \brief 关闭对话框
 * 
 * 关闭对话框
 * 
 * \return 无
 */
void CGuiNpcDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiNpcDialog = NULL;
	GetGameState()->SetStateItem(eGameStateItem_Normal);

        m_state = STATE_NORMAL;

	if (m_pMsgDlg)
	{
		((CGuiDialog*)m_pMsgDlg)->Close();
		m_pMsgDlg = NULL;
	}

	FUNCTION_END;
}

/**
 * \brief 重画界面区域
 * 
 * 重画界面区域7
 * 
 * \return 无
 */
void CGuiNpcDialog::UpdateClientRect()
{
	FUNCTION_BEGIN;

	CGuiDialog::UpdateClientRect();

	FUNCTION_END;
}

/**
 * \brief 创建对话框
 * 
 * 创建对话框
 * 
 * \return 无
 */
void CGuiNpcDialog::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	m_guiTalk = GetMLTextBox(1);
	m_guiTalk->SetSpacing(10);

	m_ptIcon.x = 33;
	m_ptIcon.y = 90;

	m_FunMapVoid.Register("SellHorse",(CGuiNpcDialog::SellHorse),this);
	m_FunMapVoid.Register("SetSavingBoxPassword",(CGuiNpcDialog::SetSavingBoxPassword),this);	

	FUNCTION_END;
}

/**
 * \brief 刷新画面和数据
 * 
 * 刷新画面和数据
 * 
 * \param fElapsedTime : 刷新时间
 * \return 处理结果
 */
HRESULT CGuiNpcDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	/*
	CNpc* pNpc = GetScene()->FindNpcAndCharacter(m_dwNpcId,true);
	if(pNpc)
	{
		int dis = Scene_GetDis(GetScene()->GetMainCharacter()->GetGridPos(),pNpc->GetGridPos());
		if (dis>c_nMaxOffsetWithNpc)
		{
			GetGameGuiManager()->EndNpcDialog();
		}
	}
	*/
	return CGuiDialog::OnRender(fElapsedTime);

	FUNCTION_END;
}

/**
 * \brief NPC对话框结束
 * 
 * NPC对话框结束
 * 
 * \return 无
 */
void CGuiNpcDialog::OnActionEnd()
{
	FUNCTION_BEGIN;

	if(m_bEndAction)Close();
	else 
	{
		SetEnabled(true);
		SetVisible(true);
	}
	m_state = STATE_NORMAL;

	FUNCTION_END;
}

/**
 * \brief NPC对话框开始
 * 
 * NPC对话框开始
 * 
 * \return 无
 */
void CGuiNpcDialog::OnActionBegin()
{
	FUNCTION_BEGIN;

	SetEnabled(false);
	SetVisible(false);

	FUNCTION_END;
}



void CGuiNpcDialog::OnInitScript()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}


/**
* \brief 更新对话框皮肤
* 
* 更新对话框皮肤
* 
* \return 无
*/
void CGuiNpcDialog::InitCustomElements()
{
	FUNCTION_BEGIN;


	/*CGuiElement Element;
	DWORD dwTextFormat = DT_LEFT | DT_TOP ;
	Element.SetFont(0,COLOR_ARGB(255,255,255,255),dwTextFormat);
	Element.FontColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 0);
	Element.FontColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.FontColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.FontColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 0);

	SetCustomElement(GUI_CONTROL_BUTTON,0,&Element);
	SetCustomElement(GUI_CONTROL_BUTTON,3,&Element);

	Element.SetFont(0,COLOR_ARGB(255,255,255,255),dwTextFormat);
	Element.FontColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.FontColor.States[ GUI_STATE_PRESSED ] = 0;
	Element.FontColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	SetCustomElement(GUI_CONTROL_BUTTON,1,&Element);
	SetCustomElement(GUI_CONTROL_BUTTON,4,&Element);

	Element.SetFont(0,COLOR_ARGB(255,255,255,255),dwTextFormat);
	Element.FontColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.FontColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 0, 0);
	Element.FontColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.FontColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	SetCustomElement(GUI_CONTROL_BUTTON,2,&Element);
	SetCustomElement(GUI_CONTROL_BUTTON,5,&Element);*/


	FUNCTION_END;
}

//---------------------------------------------
// script functions
//---------------------------------------------

/**
* \brief 初始化对话框
* 
* 初始化对话框
* 
* \return 无
*/
void CGuiNpcDialog::BeginAddDialog()
{
	FUNCTION_BEGIN;


	//if(m_guiTalk)
	//{
	//	RemoveControl(m_guiTalk);
	//	m_guiTalk = NULL;
	//}
	//for(size_t i=0;i<m_actions.size();++i)
	//{
	//	RemoveControl(m_actions[i].button);
	//	if(m_actions[i].image)	RemoveControl(m_actions[i].image);//
	//}
	m_guiTalk->SetText("");
	m_actions.resize(0);
	m_xmlTalk.Clear();
	m_xmlTalk.InsertEndChild(TiXmlElement("body"));
	m_buttons.resize(0);

	FUNCTION_END;
}

/**
* \brief 显示对话内容
* 
* 显示对话内容
* 
* \return 无
*/
void CGuiNpcDialog::EndAddDialog()
{
	FUNCTION_BEGIN;

	TiXmlElement* body = m_xmlTalk.FirstChildElement("body");
	TiXmlNode* lastNode = body->LastChild("p");
	if(lastNode)
	{
		TiXmlElement * pageCur = lastNode->ToElement();
		if(pageCur)
		{
			TiXmlNode* lastNode = pageCur->LastChild("n");
			if(lastNode)
			{
				TiXmlElement * nodeCur = lastNode->ToElement();
				if(nodeCur)
				{
					const char* pszText = XML_GetNodeText(nodeCur);
					if(pszText)
					{
						std::string strText = pszText;
						strText += "\n\n";
						XML_SetNodeText(nodeCur,strText.c_str());
					}
				}
			}
		}
	}

	m_guiTalk->SetXMLDoc(m_xmlTalk);

	for(size_t i=0;i<m_buttons.size();++i)
	{
		char szIcon[MAX_PATH];
		sprintf(szIcon,"data\\interfaces\\21\\%d",m_buttons[i].index);
		m_guiTalk->AppendParagraphButton(m_buttons[i].label.c_str(),m_buttons[i].color,m_buttons[i].id,szIcon,m_buttons[i].soundindex);
	}

	ResetControlLocation();

	FUNCTION_END;
}

/**
* \brief 加入新的功能NPC对话框
* 
* 加入新的功能NPC对话框
* 
* \param pszFunction : 服务器传过来的脚本内容
* \return 无
*/
void CGuiNpcDialog::AddDialog(const char* pszFunction)
{
	FUNCTION_BEGIN;

	char szFunction[MAX_PATH];
	strncpy(szFunction,pszFunction,MAX_PATH);
	szFunction[MAX_PATH-1] = 0;
	BeginAddDialog();

	m_pNpcScript->CallRTVoid(szFunction);	

	EndAddDialog();

	m_dwNpcId = GetScene()->GetMainCharacter()->GetNpcIdByLast();

	FUNCTION_END;
}

void CGuiNpcDialog::AddDialogEx(const char* pszFunction,const char* pszPara)
{
	FUNCTION_BEGIN;	

	char szFunction[MAX_PATH];
	strncpy(szFunction,pszFunction,MAX_PATH);
	szFunction[MAX_PATH-1] = 0;

	char szFunctionPara[MAX_PATH];
	strncpy(szFunctionPara,pszPara,MAX_PATH);
	szFunctionPara[MAX_PATH-1] = 0;

	Debug_String(szFunction);
	Debug_String(szFunctionPara);

	BeginAddDialog();
	m_pNpcScript->Call<bool,const char*>(szFunction,false,szFunctionPara);	
	EndAddDialog();

	m_dwNpcId = GetScene()->GetMainCharacter()->GetNpcIdByLast();

	FUNCTION_END;
}
// ===== Conglin begin ======
int CGuiNpcDialog::AddRenascenceDlg( const char* pzText )
{
	//int a = 0;
	//int bfgbdfgsfs = 0;
	//if( pzText != NULL )
 //       ::MessageBox( 0, pzText, pzText, MB_OK );

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pzText );
	st.action = CGuiNpcDialog::eRenascenceRequest;
	return st.actionid;

}

int CGuiNpcDialog::AddGraceChangeToExp( const char* pzText )
{
	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pzText );
	st.action = CGuiNpcDialog::eGraceChangeToExp;
	return st.actionid;
}

int CGuiNpcDialog::AddSeptMPBuff( const char* pzText )
{
	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pzText );
	st.action = CGuiNpcDialog::eSeptMPBuff;
	return st.actionid;
}

int CGuiNpcDialog::AddEnterBattle( const char* pzText )
{
	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pzText );
	st.action = CGuiNpcDialog::eEnterBattle;
	return st.actionid;
}

// ===== Conglin end ======
/**
* \brief 加入对话
* 
* 加入对话
* 
* \param pszTalk : 对话的内容
* \return 返回值的描述
*/
void CGuiNpcDialog::AddTalk(const char* pszt1)
{
	FUNCTION_BEGIN;	

	std::string strText = pszt1;
	const char * pszName = GetScene()->GetMainCharacter()->GetName();
	strText = str_Replace( strText,"(GetName)", pszName);

	std::string strTalk = strText.c_str();
	TiXmlElement* body = m_xmlTalk.FirstChildElement("body");
	if(strstr(strText.c_str(),"<p") == strText.c_str())
	{
		// 是XML
		for(;;)
		{
			TiXmlElement page("p");
			const char* p = page.Parse(strTalk.c_str(),NULL,TIXML_ENCODING_UNKNOWN);
			if(p)
			{
				//Debug_String(p);
				body->InsertEndChild(page);
				strTalk = p;
			}
			else break;
			//if(*p == 0) break;
		}
	}
	if(strstr(strText.c_str(),"<") == strText.c_str())
	{
		// 是XML
		for(;;)
		{
			TiXmlElement node("n");
			const char* p = node.Parse(strTalk.c_str(),NULL,TIXML_ENCODING_UNKNOWN);
			if(p)
			{
				TiXmlElement* pageCur = m_xmlTalk.LastChild("p")->ToElement();
				if(pageCur) 
					pageCur->InsertEndChild(node);
				else
				{
					TiXmlElement page("p");
					page.InsertEndChild(node);
					body->InsertEndChild(page);
				}
				strTalk = p;
			}
			else break;
			//if(*p == 0) break;
		}
	}
	else
	{
		TiXmlElement page("p");
		TiXmlElement node("n");
		XML_SetNodeText(&node,strText.c_str());
		page.InsertEndChild(node);
		body->InsertEndChild(page);
	}

	FUNCTION_END;
}

// = COLOR_ARGB(255,255,0,0)

int CGuiNpcDialog::AddButtonItemEx(const char* pszLabel,DWORD dwColor)
{
	FUNCTION_BEGIN;

	int index;
	index =-1;
	int soundindex;
	soundindex = -1;
	char temp[16];
	std::string iconIndex ;
	std::string label;
	while(*pszLabel)
	{
		if(*pszLabel=='(')
		{
			pszLabel++;
			while(*pszLabel!=')')
			{
				sprintf(temp,"%c",*pszLabel);
				iconIndex+=temp;
				pszLabel++;
			}
			pszLabel++;
			index = atoi(iconIndex.c_str());
			iconIndex.clear();
		}
		else if(*pszLabel=='{')
		{
			pszLabel++;
			while(*pszLabel!='}')
			{
				sprintf(temp,"%c",*pszLabel);
				iconIndex+=temp;
				pszLabel++;
			}
			soundindex = atoi(iconIndex.c_str());
			pszLabel++;
		}
		else
		{
			label+=*pszLabel++;
		}

	}
	stButtonInfo btn;
	btn.id = m_iActionId++;
	btn.label = label;
	btn.index = index;
	btn.soundindex= soundindex;
	btn.color = dwColor;
	m_buttons.push_back(btn);

	return btn.id;

	FUNCTION_END;
}
/**
* \brief 加入可点击的NPC功能按纽
* 
* 加入可点击的NPC功能按纽
* 
* \param pszLabel : NPC功能按纽名称
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddButtonItem(const char* pszLabel)
{
	return AddButtonItemEx(pszLabel,COLOR_ARGB(255,204,165,10));
}

/**
* \brief 加入买卖选项
* 
* 加入买卖选项
* 
* \param pszLabel : 选项标题
* \param pszBuyObjects : 买的物品种类
* \param pszSellObjects : 卖的物品种类
* \param pszTalk : 买卖对话框的标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddTradeItem(const char* pszLabel,const char* pszBuyObjects,const char* pszSellObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eTrade;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszSellObjects);
	st.params.push_back(pszTalk);
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 加入买卖选项
* 
* 加入买卖选项
* 
* \param pszLabel : 选项标题
* \param pszBuyObjects : 买的物品种类
* \param pszSellObjects : 卖的物品种类
* \param pszTalk : 买卖对话框的标题
* \return NPC功能按纽的ID
*/
//soke 工资购买
int CGuiNpcDialog::AddManagerSalaryDlg(const char* pszLabel,const char* pszBuyObjects,const char* pszSellObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eSalary;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszSellObjects);
	st.params.push_back(pszTalk);
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 加入积分点数买卖选项
* 
* 加入买卖选项
* 
* \param pszLabel : 选项标题
* \param pszBuyObjects : 买的物品种类
* \param pszSellObjects : 卖的物品种类
* \param pszTalk : 买卖对话框的标题
* \return NPC功能按纽的ID
*/
//soke 积分购买
int CGuiNpcDialog::AddManagerTicketDlg(const char* pszLabel,const char* pszBuyObjects,const char* pszSellObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eTicket;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszSellObjects);
	st.params.push_back(pszTalk);
	return st.actionid;

	FUNCTION_END;
}
/**
* \brief 加入充值点数买卖选项
* 
* 加入买卖选项
* 
* \param pszLabel : 选项标题
* \param pszBuyObjects : 买的物品种类
* \param pszSellObjects : 卖的物品种类
* \param pszTalk : 买卖对话框的标题
* \return NPC功能按纽的ID
*/
//soke 充值点购买
int CGuiNpcDialog::AddManagerChongzhiDlg(const char* pszLabel,const char* pszBuyObjects,const char* pszSellObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eChongzhi; //充值点
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszSellObjects);
	st.params.push_back(pszTalk);
	return st.actionid;

	FUNCTION_END;
}
int CGuiNpcDialog::AddFubenOpen(const char* pszLabel, int type) //副本开启
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size() + 1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eFubenOpen;
	char szData[32] = {0};
	sprintf(szData, "%d", type);
	st.params.push_back(szData);
	return st.actionid;

	FUNCTION_END;
}

//家族地图
int CGuiNpcDialog::AddJiaZuOpen(const char* pszLabel) //副本开启
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size() + 1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eJiaZuOpen;
	return st.actionid;

	FUNCTION_END;
}

//家族BOSS
int CGuiNpcDialog::AddJiaZuBoss(const char* pszLabel) //副本开启
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size() + 1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eJiaZuBoss;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddFubenEnter(const char* pszLabel, int type) //副本进入
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size() + 1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eFubenEnter;
	char szData[32] = {0};
	sprintf(szData, "%d", type);
	st.params.push_back(szData);
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddFubenLeave(const char* pszLabel) //副本退出
{
	FUNCTION_BEGIN;
	return AddActionItem(pszLabel,CGuiNpcDialog::eFubenLeave);
	FUNCTION_END;
}
//////////////////////////////////////////////////////////////////////////
/**
* \brief 加入买卖/修理选项
* 
* 加入买卖/修理选项
* 
* \param pszLabel : 选项标题
* \param pszBuyObjects : 买的物品种类
* \param pszSellObjects : 卖的物品种类
* \param pszRepairObjects : 修理的物品种类
* \param pszTalk : 对话框的标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddTradeRepairItem(const char* pszLabel,const char* pszBuyObjects,const char* pszSellObjects,const char* pszRepairObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eTradeRepair;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszSellObjects);
	st.params.push_back(pszRepairObjects);
	st.params.push_back(pszTalk);
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 加入买卖/修理选项(金子)
* 
* 加入买卖/修理选项
* 
* \param pszLabel : 选项标题
* \param pszBuyObjects : 买的物品种类
* \param pszSellObjects : 卖的物品种类
* \param pszRepairObjects : 修理的物品种类
* \param pszTalk : 对话框的标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddTradeRepairItemByGold(const char* pszLabel,const char* pszBuyObjects,const char* pszSellObjects,const char* pszRepairObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eTradeRepairByGold;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszSellObjects);
	st.params.push_back(pszRepairObjects);
	st.params.push_back(pszTalk);
	return st.actionid;

	FUNCTION_END;
}
/**
* \brief 物品仓库
* 
* 物品仓库
* 
* \param pszLabel : 对话框的标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddGoodsStore(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eStore;
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 加入合成选项
* 
* 加入合成选项
* 
* \param pszLabel : 选项标题
* \param pszBuyObjects : 合成的物品种类
* \param pszTalk : 对话框的标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddChangeItem(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eChange;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 加入打造选项
* 
* 加入打造选项
* 
* \param pszLabel : 选项标题
* \param pszBuyObjects : 打造的物品种类
* \param pszTalk : 对话框的标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddMakeItem(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eMake;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}


int CGuiNpcDialog::AddScriptButton(const char* pszLabel,const char* pszFunction)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eScriptButton;
	st.params.push_back(pszFunction);
	return st.actionid;

	FUNCTION_END;	
}

/**
* \brief 加入烹饪选项
* 
* 加入烹饪选项
* 
* \param pszLabel : 选项标题
* \param pszBuyObjects : 烹饪的物品种类
* \param pszTalk : 对话框的标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddMakeFood(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eMakeFood;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 加入炼药选项
* 
* 加入炼药选项
* 
* \param pszLabel : 选项标题
* \param pszBuyObjects : 炼药的物品种类
* \param pszTalk : 对话框的标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddMakeDrug(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eMakeDrug;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 加入拆除装备选项
* 
* 加入拆除装备选项
* 
* \param pszLabel : 选项标题
* \param pszBuyObjects : 拆除的物品种类
* \param pszTalk : 对话框的标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddBackoutItem(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eBackout;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}


/**
* \brief 加入升级选项
* 
* 加入升级选项
* 
* \param pszLabel : 选项标题
* \param pszBuyObjects : 打造的物品种类
* \param pszTalk : 对话框的标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddUpItem(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eUp;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 加入灵魂合成选项
* 
* 加入灵魂合成选项
* 
* \param pszLabel : 选项标题
* \param pszBuyObjects : 灵魂ID
* \param pszTalk : 对话框的标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddKaChangeItem(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eKaChange;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 加入灵魂镶嵌选项
* 
* 加入灵魂镶嵌选项
* 
* \param pszLabel : 选项标题
* \param pszBuyObjects : 灵魂ID
* \param pszTalk : 对话框的标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddKaMakeItem(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eKaMake;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 装备打孔选项
* 
* 装备打孔选项
* 
* \param pszLabel : 选项标题
* \param pszBuyObjects : 装备ID
* \param pszTalk : 对话框的标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddHoleMakeItem(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eHoleMake;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 建立帮会
* 
* 建立帮会
* 
* \param pszLabel : 选项标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddTong(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eTong;
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 建立家族
* 
* 建立家族
* 
* \param pszLabel : 选项标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddSept(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eSept;
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 建立门派
* 
* 建立门派
* 
* \param pszLabel : 选项标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddSchool(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eSchool;
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 结婚选项
* 
* 结婚选项
* 
* \param pszLabel : 选项标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddMarry(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eMarry;
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 发放礼品选项
* 
* 发放礼品选项
* 
* \param pszLabel : 选项标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddGiveGift(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eGiveGift;
	return st.actionid;

	FUNCTION_END;
}

//////////////////////////////////////////////////////////////////////////
//推荐人系统(A推荐B)

//输入推荐人姓名
int CGuiNpcDialog::AddRecoSys_A_Input(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eRecoSys_A_Input;
	return st.actionid;

	FUNCTION_END;
}

//被推荐人奖励查询和领取
int CGuiNpcDialog::AddRecoSys_B_Query_Take(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eRecoSys_B_Query_Take;
	return st.actionid;

	FUNCTION_END;
}

//推荐人奖金查询和领取
int CGuiNpcDialog::AddRecoSys_A_Query_Take(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eRecoSys_A_Query_Take;
	return st.actionid;

	FUNCTION_END;
}

//////////////////////////////////////////////////////////////////////////
//请求领取家族普通经验
int CGuiNpcDialog::AddRequestSeptNormalExp(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eRequestSeptNormalExp;
	return st.actionid;

	FUNCTION_END;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//做为师傅查询/领取进贡
int CGuiNpcDialog::AddReqMasterBounty(const char* pszLabel, const char* operatorID)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eMasterBounty;
	st.params.push_back(operatorID);
	return st.actionid;

	FUNCTION_END;
}
//做为徒弟查询/进行进贡
int CGuiNpcDialog::AddReqPrenticeBounty(const char* pszLabel, const char* operatorID)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::ePrenticeBounty;
	st.params.push_back(operatorID);
	return st.actionid;

	FUNCTION_END;
}
//////////////////////////////////////////////////////////////////////////

int CGuiNpcDialog::AddLeaveMarry(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eLeaveMarry;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddChangeCountry(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eChangeCountry;
	return st.actionid;

	FUNCTION_END;
}


int CGuiNpcDialog::AddChangeWarField(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eChangeWarField;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddChoiceWarField(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eChoiceWarField;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddCountryWarQuery(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eCountryWarQuery;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddWarOperator(const char* pszLabel,const char* operatorID)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eWarOperator;
	st.params.push_back(operatorID);
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddSeptAttackNpc(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eSeptAttackNpc;
	return st.actionid;

	FUNCTION_END;

}
int CGuiNpcDialog::AddSeptNpcCustom(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eSeptNpcCustom;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddTongAttackTown(const char* pszLabel, const char* operatorID)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eTongAttackTown;
	st.params.push_back(operatorID);
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddTongTownCustom(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eTongTownCustom;
	return st.actionid;

	FUNCTION_END;
}

//soke 皇帝领取每日薪资
int CGuiNpcDialog::AddEmperorRequestDailyMoney( const char* pszLabel )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel );
	st.action = CGuiNpcDialog::eEmperorRequestDailyMoney;
	return st.actionid;

	FUNCTION_END;
}

//soke 国王领取每日薪资
int CGuiNpcDialog::AddKingRequestDailyMoney( const char* pszLabel )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel );
	st.action = CGuiNpcDialog::eKingRequestDailyMoney;
	return st.actionid;

	FUNCTION_END;
}

//soke 元帅领取每日薪资
int CGuiNpcDialog::AddYsRequestDailyMoney( const char* pszLabel )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel );
	st.action = CGuiNpcDialog::eYuanshuaiRequestDailyMoney;
	return st.actionid;

	FUNCTION_END;
}

//soke 宰相领取每日薪资
int CGuiNpcDialog::AddZxRequestDailyMoney( const char* pszLabel )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel );
	st.action = CGuiNpcDialog::eZaixiangRequestDailyMoney;
	return st.actionid;

	FUNCTION_END;
}

//soke 捕头（左）领取每日薪资
int CGuiNpcDialog::AddCrRequestDailyMoney( const char* pszLabel )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel );
	st.action = CGuiNpcDialog::eCatcherRequestDailyMoney;
	return st.actionid;

	FUNCTION_END;
}

//soke 御史（左）领取每日薪资
int CGuiNpcDialog::AddDtRequestDailyMoney( const char* pszLabel )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel );
	st.action = CGuiNpcDialog::eDiplomatRequestDailyMoney;
	return st.actionid;

	FUNCTION_END;
}

//soke 捕头（右）领取每日薪资
int CGuiNpcDialog::AddCxRequestDailyMoney( const char* pszLabel )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel );
	st.action = CGuiNpcDialog::eCatchexRequestDailyMoney;
	return st.actionid;

	FUNCTION_END;
}

//soke 御史（右）领取每日薪资
int CGuiNpcDialog::AddDxRequestDailyMoney( const char* pszLabel )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel );
	st.action = CGuiNpcDialog::eDiplomaxRequestDailyMoney;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddSeptTownExp(const char* pszLabel)
{
	FUNCTION_BEGIN;
	
	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eSeptTownExp;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddQuestDragonSprite( const char* pszLable )
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLable);
	st.action = CGuiNpcDialog::eQuestDragonSprite;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddQuestTigerSprite( const char* pszLable )
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLable);
	st.action = CGuiNpcDialog::eQuestTigerSprite;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::SeptGiveupOccupy( const char* pszLable )
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLable);
	st.action = CGuiNpcDialog::eSeptGiveupOccupy;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::UnionGiveupOccupy( const char* pszLable )
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLable);
	st.action = CGuiNpcDialog::eUnionGiveupOccupy;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddRequestItemCardForGift( const char* pszLable )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pszLable );
	st.action = CGuiNpcDialog::eRequestItemCardForGift;
	return st.actionid;

	FUNCTION_END;
}


int CGuiNpcDialog::AddCompetitionNpc(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eCompetitionNpc;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddRequestAllyCountry( const char* pszLable )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pszLable );
	st.action = CGuiNpcDialog::eRequestAllyCountry;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddCancelAllyCountry( const char* pszLable )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pszLable );
	st.action = CGuiNpcDialog::eCancelAllyCountry;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddRequestAllyInfo(const char * pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pszLabel );
	st.action = CGuiNpcDialog::eRequestAllyCountryInfo;
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 判断是否有战斗的权限
* 
* 判断是否有战斗的权限
* 
* \param nState : 类别
* \return 检查结果
*/
bool CGuiNpcDialog::IsAttackPop(int nState)
{
	FUNCTION_BEGIN;

	switch(nState) {
	case UNION_DARE:
		if (strncmp(GetScene()->GetMainCharacter()->GetName(),
			GetGameGuiManager()->m_guiWorldConn->m_TongInfo.master,
			sizeof(GetGameGuiManager()->m_guiWorldConn->m_TongInfo.master))==0)
			return true;
		break;

	case SCHOOL_DARE:
		if (GetScene()->GetMainCharacter()->GetID() == GetGameGuiManager()->m_guiWorldConn->m_SchoolInfo.dwMasterTempID)
			return true;
		break;

	case SEPT_DARE:
		if (strncmp(GetScene()->GetMainCharacter()->GetName(),
			GetGameGuiManager()->m_guiWorldConn->m_SeptInfo.master,
			sizeof(GetGameGuiManager()->m_guiWorldConn->m_SeptInfo.master))==0)
			return true;
		break;
	}

	return false;

	FUNCTION_END;
}

/**
* \brief 帮会、家族战选项
* 
* 帮会、家族战选项
* 
* \param pszLabel : 选项标题
* \param nState : 选项类别
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddLaunchAttack(const char* pszLabel,int nState)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	switch(nState) {
	case UNION_DARE:
		st.action = CGuiNpcDialog::eTongAttack;
		break;
	case SCHOOL_DARE:
		st.action = CGuiNpcDialog::eSchoolAttack;
		break;
	case SEPT_DARE:
		st.action = CGuiNpcDialog::eSeptAttack;
		break;
	}
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 解散帮会
* 
* 解散帮会
* 
* \param pszLabel : 选项标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::DestroyTong(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eDestroyTong;
	return st.actionid;

	FUNCTION_END;
}


/**
* \brief 解散家族
* 
* 解散家族
* 
* \param pszLabel : 选项标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::DestroySept(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eDestroySept;
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 解散门派
* 
* 解散门派
* 
* \param pszLabel : 选项标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::DestroySchool(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eDestroySchool;
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 加入对话框选项
* 
* 加入对话框选项
* 
* \param pszLabel : 选项标题
* \param pszFunction : 脚本内容
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddDialogItem(const char* pszLabel,const char* pszFunction)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	if (strstr(pszLabel,"任务：") != NULL) 
	{
		st.actionid = AddButtonItemEx(pszLabel,COLOR_ARGB(255,0,255,0));
		//st.actionid = AddButtonItem(pszLabel);
	}
	else
	{
		st.actionid = AddButtonItem(pszLabel);
	}
	st.action = CGuiNpcDialog::eDialog;
	st.params.push_back(pszFunction);

	return st.actionid;


	FUNCTION_END;
}

int CGuiNpcDialog::AddDialogItemEx(const char* pszLabel,const char* pszFunction,const char* pszPara) //正常副本
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eDialogEx;
	st.params.push_back(pszFunction);
	st.params.push_back(pszPara);
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 加入关闭对话框选项
* 
* 加入关闭对话框选项
* 
* \param pszLabel : 选项标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddCloseItem(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eClose;
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 帮会投票
* 
* 帮会投票
* 
* \param pszLabel : 选项标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddTongBallot(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eTongBallot;
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 家族投票
* 
* 家族投票
* 
* \param pszLabel : 选项标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddSeptBallot(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eSeptBallot;
	return st.actionid;

	FUNCTION_END;
}


/**
* \brief 增加任务选项
* 
* 增加任务选项
* 
* \param pszLabel : 选项标题
* \param pszCmd : 任务ID
* \param pszCmd2 : 任务ID 2
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddTaskCmdItem(const char* pszLabel,const char* pszCmd, int pszCmd1, int pszCmd2)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItemEx(pszLabel,COLOR_ARGB(255,0,255,0));
	st.action = CGuiNpcDialog::eTask;

	char	data[32]={0};
	sprintf( data, "%s", pszCmd );
	st.params.push_back( data );

	sprintf( data, "%u", pszCmd1 );
	st.params.push_back( data );

	sprintf( data, "%u", pszCmd2 );
	st.params.push_back(data);

	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddCountryStrategy(const char* pszLabel,const char* pszCmd, int pszCmd1, int pszCmd2)
{
	FUNCTION_BEGIN;

	const char * pOwnCountry = GetScene()->GetMainCharacter()->GetCountryName();
	if(NULL != strstr(pszLabel,pOwnCountry) || CCountry::Instance()->IsAllyCountryAsName(pszLabel))
	{// 本国或者盟国不能在国家任务列表
		return 1;
	}

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItemEx(pszLabel,COLOR_ARGB(255,0,255,0));
	st.action = CGuiNpcDialog::eTask;

	char	data[32]={0};
	sprintf( data, "%s", pszCmd );
	st.params.push_back( data );

	sprintf( data, "%u", pszCmd1 );
	st.params.push_back( data );

	sprintf( data, "%u", pszCmd2 );
	st.params.push_back(data);

	return st.actionid;

	FUNCTION_END;
}


/**
* \brief 改变头发
* 
* 改变头发
* 
* \param pszLabel : 选项标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddHairDialog(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eHair;
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 改变衣服颜色
* 
* 改变衣服颜色
* 
* \param pszLabel : 选项标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddClothColorDialog(const char* pszLabel)
{
	FUNCTION_BEGIN;

	if(GetScene()->GetMainCharacter()->GetClothID() == 0)
	{
		Close();
		GetGameGuiManager()->AddClientSystemMessage("对不起，你未穿衣服");
		return 0;
	}
	if(false == GetScene()->GetMainCharacter()->bCanChangeClothColor())
	{
		Close();
		GetGameGuiManager()->AddClientSystemMessage("对不起，你的衣服不能染色");
		return 0;
	}

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eClothColor;
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 主角是否有马匹
* 
* 
* 
* \return 有则返回true, 否则返回false
*/
bool CGuiNpcDialog::IsHasHorse()
{
	if ( GetScene()->GetMainCharacter() )
		return GetScene()->GetMainCharacter()->IsHasHorse();

	return false;
}

/**
* \brief 主角卖马匹
* 
* 
* 
* \return 
*/
int CGuiNpcDialog::SellHorse()
{	
	//GetGameGuiManager()->AddMessageBox( "你真的想卖马吗？",eAction_HorseSellConfirm,NULL,"确定|1|Enter|取消|2|Esc");
	///Close();
	if ( GetScene()->GetMainCharacter() )
	{
		GetScene()->GetMainCharacter()->SellHorse();
	}
	Close();
	return 1;
	//m_pMsgDlg = (void*) pDlg;
	//GetGameGuiManager()->AddMessageBox( "你真的想卖马吗？",eAction_HorseSellConfirm,NULL,"确定|1|Enter|取消|2|Esc");
}

/**
* \brief 刷新主角马匹拥有状态
* 
* 
* 
* \return 
*/
void CGuiNpcDialog::ResetHorseOwnStatus()
{
	if ( GetScene()->GetMainCharacter() )
		GetScene()->GetMainCharacter()->ResetHorseOwnStatus();
}


void CGuiNpcDialog::ResetSavingBoxOwnNum()
{
	if ( GetScene()->GetMainCharacter() )
		GetScene()->GetMainCharacter()->ResetSavingBoxOwnNum();
}

int CGuiNpcDialog::GetSavingBoxNum()
{
	int iRes = 0;
	if ( GetScene()->GetMainCharacter() )
		iRes = GetScene()->GetMainCharacter()->GetSavingBoxNum();

	return iRes;
}

int  CGuiNpcDialog::GetRealSavingBoxNum()
{
	int iRes = 0;
	if ( GetScene()->GetMainCharacter() )
		iRes = GetScene()->GetMainCharacter()->GetRealSavingBoxNum();

	return iRes;	
}

void CGuiNpcDialog::AddBuySavingBoxCmd(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eBuySavingBox;

	FUNCTION_END;
}

/**
* \brief 实现当前已经加载的脚本内容
* 
* 实现当前已经加载的脚本内容
* 
* \param id : 要触发的功能ID
* \return 无
*/
void CGuiNpcDialog::Action(int id)
{
	FUNCTION_BEGIN;

	for(size_t i=0;i<m_actions.size();++i)
	{
		if(m_actions[i].actionid == id)
		{
			OnNpcAction(id);
			break;
		}
	}

	FUNCTION_END;
}


/**
* \brief 触发NPC脚本功能
* 
* 触发NPC脚本功能
* 
* \param index : 脚本功能ID
* \return 无
*/
void CGuiNpcDialog::OnNpcAction(int index)
{
	FUNCTION_BEGIN;

	m_state = STATE_NORMAL;

	stAction& st = m_actions[index];
	switch(st.action)
	{
	case eSchool:			
		{
			OnActionBegin();
			RequestSchool();
			GetGameGuiManager()->OnNpcActionEnd();
		}
		break;

	case eMarry:
		{
			OnActionBegin();
			RequestMarry();
		}
		break;
	case eGiveGift:
		{
			OnActionBegin();
			GetGameGuiManager()->AddGiveGiftDlg();
		}
		break;
	//////////////////////////////////////////////////////////////////////////
	//推荐人系统(A推荐B)
	case eRecoSys_A_Input:
		{
			int curLevel = GetScene()->GetMainCharacter()->GetLevel();
			if (/*curLevel>10 && */curLevel<= 15) 
			{
				OnActionBegin();
				GetGameGuiManager()->AddRecoSys_A_InputDlg();
			}
			else
			{
				GetGameGuiManager()->AddMessageBox("你当前的等级不能填写推荐人");
			}			
		}
		break;
	case eRecoSys_B_Query_Take:
		{
			OnActionBegin();

			TRACE("@@@@@@---请求[被]推荐人奖励[查询]---wyc");
			stReqRecommendedBountyUserCmd cmd;
			cmd.byState = QUERY_BOUNTY;
			SEND_USER_CMD(cmd);
			
			GetGameGuiManager()->AddRecoSys_B_Query_TakeDlg();
		}
		break;
	case eRecoSys_A_Query_Take:
		{
			OnActionBegin();

			TRACE("@@@@@@---请求推荐人奖励[查询]---wyc");
			stReqRecommendBountyUserCmd cmd;
			cmd.byState = QUERY_BOUNTY;
			SEND_USER_CMD(cmd);

			GetGameGuiManager()->AddRecoSys_A_Query_TakeDlg();
		}
		break;
	//////////////////////////////////////////////////////////////////////////
	//领取家族普通经验
	case eRequestSeptNormalExp:
		{
			OnActionBegin();
			if (GetScene()) 
			{
				if (GetScene()->GetMainCharacter())
				{
					if(GetScene()->GetMainCharacter()->IsSeptLeader())
					{
						TRACE("_wyc_请求领取家族普通经验");
						stRequestSeptNormalExpCmd cmd;
						SEND_USER_CMD(cmd);
					}
					else
					{
						GetGameGuiManager()->AddMessageBox("你不是族长不能领取家族经验!");
					}
				}
			}
					
		}
		break;
	//////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	//做为师傅查询/领取进贡
	case eMasterBounty:
		{
			OnActionBegin();
			if (GetScene()) 
			{
				if (GetScene()->GetMainCharacter())
				{
					if(1)//if(GetScene()->GetMainCharacter()->isMaster())
					{
						switch(atoi(st.params[0].c_str())) 
						{
						case 0://查询
							{
								TRACE("_wyc_做为师傅--[查询]--进贡");
								stReqMasterBountyUserCmd cmd;
								cmd.byState = QUERY_SCHOOL_BOUNTY;
								SEND_USER_CMD(cmd);
								GetGameGuiManager()->AddMasterBountyDlg();
							}							
							break;
						case 1://领取
							{
								TRACE("_wyc_做为师傅--[领取]--进贡");
								stReqMasterBountyUserCmd cmd;
								cmd.byState = GET_SCHOOL_BOUNTY;
								SEND_USER_CMD(cmd);
							}		
							break;
						default:
							;
						}					
					}
					else
					{
						GetGameGuiManager()->AddMessageBox("你没有徒弟不能进入师徒进贡系统!");
					}
				}
			}
		}
		break;
	//做为徒弟查询/进行进贡
	case ePrenticeBounty:
		{
			OnActionBegin();
			if (GetScene()) 
			{
				if (GetScene()->GetMainCharacter())
				{
					if(1)//if(GetScene()->GetMainCharacter()->isPrentice())
					{
						switch(atoi(st.params[0].c_str())) 
						{
						case 0://查询
							{
								TRACE("_wyc_做为徒弟--[查询]--进贡");
								stReqPrenticeBountyUserCmd cmd;
								cmd.byState = QUERY_SCHOOL_BOUNTY;
								SEND_USER_CMD(cmd);
								GetGameGuiManager()->AddPrenticeBountyDlg();
							}							
							break;
						case 1://进行
							{
								TRACE("_wyc_做为徒弟--[进行]--进贡");
								stReqPrenticeBountyUserCmd cmd;
								cmd.byState = PUT_SCHOOL_BOUNTY;
								SEND_USER_CMD(cmd);
							}		
							break;
						default:
							;
						}					
					}
					else
					{
						GetGameGuiManager()->AddMessageBox("你没有师父不能进入师徒进贡系统!");
					}
				}
			}
		}
		break;
	/////////////////////////////////////////////////////////////////////////
	
	case eTong:
		{
			OnActionBegin();
			RequestTong();
			GetGameGuiManager()->OnNpcActionEnd();
		}
		break;

	case eSept:
		{
			OnActionBegin();
			RequestSept();
			GetGameGuiManager()->OnNpcActionEnd();
		}
		break;

	case eDestroySchool:			
		{
			OnActionBegin();
			ShutDownSchool();
			GetGameGuiManager()->OnNpcActionEnd();
		}
		break;

	case eDestroyTong:
		{
			OnActionBegin();
			ShutDownTong();
			GetGameGuiManager()->OnNpcActionEnd();
		}
		break;

	case eDestroySept:
		{
			OnActionBegin();
			ShutDownSept();
			GetGameGuiManager()->OnNpcActionEnd();
		}
		break;

	case eTrade: //soke 金子购买界面
		if(!GetGameGuiManager()->m_guiNpcTrade)
		{
			OnActionBegin();
			CGuiNpcTrade* pDlg = GetGameGuiManager()->AddNpcTrade(eTrade);

			if ( !m_bShowTradeDlg )
				((CGuiDialog*)pDlg)->SetVisible( false );

			pDlg->SetParam(st.params);
			pDlg->SetPage(m_pNpcScript->Call<int>("GetTradeStartPage",0));

			if ( m_bShowTradeDlg )
				GetGameGuiManager()->AddItem();
		}
		break;

	case eSalary: //soke 2016 - 3 - 10 工资购买界面
		if(!GetGameGuiManager()->m_guiSalaryTrade)
		{
			OnActionBegin();
			CGuiSalaryTrade* pDlg = GetGameGuiManager()->AddSalaryTrade();

			if ( !m_bShowTradeDlg )
				((CGuiDialog*)pDlg)->SetVisible( false );

			pDlg->SetParam(st.params);
			pDlg->SetPage(m_pNpcScript->Call<int>("GetTradeStartPage",0));

			if ( m_bShowTradeDlg )
				GetGameGuiManager()->AddItem();
		}
		break;	

	case eTicket: //myy 2023 - 7 - 31 积分购买界面
		if(!GetGameGuiManager()->m_guiTicketTrade)
		{
			OnActionBegin();
			CGuiTicketTrade* pDlg = GetGameGuiManager()->AddTicketTrade();

			if ( !m_bShowTradeDlg )
				((CGuiDialog*)pDlg)->SetVisible( false );

			pDlg->SetParam(st.params);
			pDlg->SetPage(m_pNpcScript->Call<int>("GetTradeStartPage",0));

			if ( m_bShowTradeDlg )
				GetGameGuiManager()->AddItem();
		}
		break;	
	case eChongzhi: //myy 2023 - 10 - 10 充值点购买界面
		if(!GetGameGuiManager()->m_guiChongzhiTrade)
		{
			OnActionBegin();
			CGuiChongzhiTrade* pDlg = GetGameGuiManager()->AddChongzhiTrade();

			if ( !m_bShowTradeDlg )
				((CGuiDialog*)pDlg)->SetVisible( false );

			pDlg->SetParam(st.params);
			pDlg->SetPage(m_pNpcScript->Call<int>("GetTradeStartPage",0));

			if ( m_bShowTradeDlg )
				GetGameGuiManager()->AddItem();
		}
		break;

	case eChange:
	case eUp:
	case eMake:
	case eKaChange:
	case eKaMake:
	case eHoleMake:
	case eBackout:
	case eMakeFood:
	//soke 改造
	case eModifyAttri:          // 改造属性
	case eAddReBind:			// 重新绑定
	case eAddReMaker:			// 修改签名
	case eAddAddMaker:			// 装备签名
	case eAddModifySoulA:		// 修改灵魂属性
	case eAddLVSoullineMD:		// 保留灵魂锁链改造
	case eAddModifyDxSoulA:		// 修改第二灵魂属性
	case eAddMDSkillA:			// 修改技能属性
	case eAddEquipRound:		// 非转生装备改为转生
	case eAddMDMake:		    // 马匹装备重新锻造
	case eAddEquipMake:		    // 装备祝福
	case eAddEquipZsMake:		// 马匹装备祝福
	case eAddEquipMaklv:		// 装备升级
	case eMakeDrug:
		if(!GetGameGuiManager()->m_guiNpcMake)
		{
			OnActionBegin();
			CGuiNpcMake* pDlg = GetGameGuiManager()->AddNpcMake(st.action);
			pDlg->LoadData(st.params);	
		}
		break;
	case eAddHorseTraining_one:		// 1级训练
		{
			if( GetScene()->GetMainCharacter()->IsHasHorse() )
			{
				if( GetScene()->GetMainCharacter()->GetMoney() > 1000 )
				{
					stHorseTrainingUserCmd Cmd;
					Cmd.CmdTey = 0;
					SEND_USER_CMD(Cmd);
				}
				else
					GetGameGuiManager()->AddClientSystemMessage("身上的银子不够拉！多赚点钱在来吧");
			}
			else
				GetGameGuiManager()->AddClientSystemMessage("你没有马！怎么训练？");
		}
		break;
	case eAddHorseTraining_two:		// 2级训练
		{
			if( GetScene()->GetMainCharacter()->IsHasHorse() )
			{
				if( GetScene()->GetMainCharacter()->GetMoney() > 2000 )
				{
					stHorseTrainingUserCmd Cmd;
					Cmd.CmdTey = 1;
					SEND_USER_CMD(Cmd);
				}
				else
					GetGameGuiManager()->AddClientSystemMessage("身上的银子不够拉！多赚点钱在来吧");
			}
			else
				GetGameGuiManager()->AddClientSystemMessage("你没有马！怎么训练？");
		}
		break;
	case eAddHorseTraining_three:	// 3级训练
		{
			if( GetScene()->GetMainCharacter()->IsHasHorse() )
			{
				if( GetScene()->GetMainCharacter()->GetMoney() > 5000 )
				{
					stHorseTrainingUserCmd Cmd;
					Cmd.CmdTey = 2;
					SEND_USER_CMD(Cmd);
				}
				else
					GetGameGuiManager()->AddClientSystemMessage("身上的银子不够拉！多赚点钱在来吧");
			}
			else
				GetGameGuiManager()->AddClientSystemMessage("你没有马！怎么训练？");
		}
		break;
	case eClearHorseTraining:		// 清除训练
		{
			if( GetScene()->GetMainCharacter()->IsHasHorse() )
			{
				stHorseTrainingUserCmd Cmd;
				Cmd.CmdTey = 4;
				SEND_USER_CMD(Cmd);
			}
			else
				GetGameGuiManager()->AddClientSystemMessage("你没有马！怎么训练？");
		}
		break;
	case eExchangeExp:        //功勋换经验
		{
			if( GetScene()->GetMainCharacter()->GetProData()->exploit > 0 )
			{
				//stHorseTrainingUserCmd Cmd;
				//Cmd.CmdTey = 4;
				//SEND_USER_CMD(Cmd);
			}
			else
				GetGameGuiManager()->AddClientSystemMessage("别防碍公务！一点功勋都没有！还想换经验！卫兵把他抓起来！！");
		}
		break;
	case eChangeCountryBySalary:           //工资移民
		{
			GetGameGuiManager()->AddChangeCountrySalaryDialog();
		}
		break;
	case eTradeRepair:
		if(!GetGameGuiManager()->m_guiNpcTrade)
		{
			OnActionBegin();
			CGuiNpcTrade* pDlg = GetGameGuiManager()->AddNpcTrade(eTradeRepair);
			pDlg->SetParam(st.params);
			pDlg->SetPage(m_pNpcScript->Call<int>("GetTradeStartPage",0));
			GetGameGuiManager()->AddItem();
		}
		break;

	case eTradeRepairByGold:
		if(!GetGameGuiManager()->m_guiNpcTrade)
		{
			OnActionBegin();
			CGuiNpcTrade* pDlg = GetGameGuiManager()->AddNpcTrade(eTradeRepairByGold);
			pDlg->SetParam(st.params);
			pDlg->SetPage(m_pNpcScript->Call<int>("GetTradeStartPage",0));
			GetGameGuiManager()->AddItem();
		}
		break;

	case eTongBallot:
		if(!GetGameGuiManager()->m_guiBallot)
		{
			OnActionBegin();
			CGuiBallot* pDlg = GetGameGuiManager()->AddBallot(eAction_Tong);
		}
		break;

	case eSeptBallot:
		if(!GetGameGuiManager()->m_guiBallot)
		{
			OnActionBegin();
			CGuiBallot* pDlg = GetGameGuiManager()->AddBallot(eAction_Sept);
		}
		break;

	case eStore:
		{
			ShowSavingBox();
		}
		break;

	case eTongAttack:
		if (isset_state(GetGameGuiManager()->m_guiWorldConn->m_TongPower,DARE))
		{
			//GetGameGuiManager()->AddMessageBox( "发起帮会战斗需要付50两,确定战斗吗？",eAction_TongAttack,NULL,c_szMBOKCancel);
			GetGameGuiManager()->AddMessageBox( "确定发起帮会战斗吗？",eAction_TongAttack,NULL,c_szMBOKCancel);
		}
		else
		{
			GameMessageBox("你无发起帮会战斗的权限！");
		}
		break;

	case eSeptAttack:
		//GetGameGuiManager()->AddMessageBox( "发起家族战斗需要付10两,确定战斗吗？",eAction_SeptAttack,NULL,c_szMBOKCancel);
		GetGameGuiManager()->AddMessageBox( "确定发起家族战斗吗？",eAction_SeptAttack,NULL,c_szMBOKCancel);
		break;

	case eSchoolAttack:
		//GetGameGuiManager()->AddMessageBox( "发起门派战斗需要付5两,确定战斗吗？",eAction_SchoolAttack,NULL,c_szMBOKCancel);
		GetGameGuiManager()->AddMessageBox( "确定发起门派战斗吗？",eAction_SchoolAttack,NULL,c_szMBOKCancel);
		break;

	case eDialog:
		if(st.params.size() > 0)
		{
			AddDialog(st.params[0].c_str());
		}
		break;
	case eDialogEx:
		if(st.params.size() > 1)
		{
			AddDialogEx(st.params[0].c_str(),st.params[1].c_str());
		}
		break;
	case eTask:
		{
			if( st.params.size() == 3 )
			{
				stRequestQuestUserCmd	cmd;

				_snprintf(cmd.target,16,st.params[0].c_str());
				cmd.target[15]=0;
				cmd.id = atoi( st.params[1].c_str() );
				cmd.offset = atoi( st.params[2].c_str() );

				SEND_USER_CMD(cmd);
			}
			Close();
		}
		break;
		// 2010 7
	case eFetchPearl: 
		{
			GetGameGuiManager()->AddDlgProgressBar(PROGRESSBAR_TYPE_LOONGPEARL);
			GetGameGuiManager()->m_pDlgProgressBar->StartCountDown(3.f); //soke 2016 - 3 - 28 拆除太庙时间3秒
		}
		break;
	case ePutPearl:
		{
			ReqPutLoongPearlLoongUserCmd	cmd;

			SEND_USER_CMD(cmd);
			Close();
		}
		break;
	case eOpenPearlRank:
		{
			if( GetGameGuiManager()->m_guiTopDialog && GetGameGuiManager()->m_guiTopDialog->IsVisible() )
				{
					GetGameGuiManager()->m_guiTopDialog->Close();
				}
				else
				{
					GetGameGuiManager()->AddTaiMiaoTopDialog();
					GetGameGuiManager()->m_guiTaiMiaoTopDialog->m_pListBoxRankList->SelectItem(0,true);
				}
		}
		break;
	case eFetch2Pearl: //soke 更换太庙资源
		{
			//检测是否携带已拆除的太庙砖块资源 
			if ( isset_state(GetScene()->GetMainCharacter()->GetState(), USTATE_LOONGPEARL_WHITE) 
			|| isset_state(GetScene()->GetMainCharacter()->GetState(), USTATE_LOONGPEARL_BLUE)  
			|| isset_state(GetScene()->GetMainCharacter()->GetState(), USTATE_LOONGPEARL_YELLOW)  
			|| isset_state(GetScene()->GetMainCharacter()->GetState(), USTATE_LOONGPEARL_GREEN)  
			|| isset_state(GetScene()->GetMainCharacter()->GetState(), USTATE_LOONGPEARL_PURPLE))  	
            {     
			    GetGameGuiManager()->AddDlg2ProgressBar(PROGRESSBAR2_TYPE_LOONGPEARL); 
				//soke 2017 - 1 - 11 更换太庙时间3秒 
			    GetGameGuiManager()->m_pDlg2ProgressBar->StartCountDown(3.f); 
            }
		    else
		    {
	            GameMessageBox("你的身上没有携带太庙砖块资源!请先拆除太庙资源");
            }
		}
		break;
	case eSeptGoToMale:
		{
			if( GetScene()->GetMainCharacter()->GetSex() == eSex_Male )
			{
				ReqMaleEnterSpaMiniFuncUserCmd cmd;

			    SEND_USER_CMD(cmd);
			    Close();
			}
			else
			{
				GameMessageBox( "女性不可以进入男温泉的哦！" );
			}
		}
		break;
	case eSeptGoToFemale:
		{
			if( GetScene()->GetMainCharacter()->GetSex() == eSex_Female )
			{
				ReqFemaleEnterSpaMiniFuncUserCmd cmd;

			    SEND_USER_CMD(cmd);
			    Close();
			}
			else
			{
				GameMessageBox( "男性不可以进入女温泉的哦！" );
			}
		}
		break;
	case eSeptExitHotspring:
		{
			ReqLeaveSpaMiniFuncUserCmd	    cmd;

			SEND_USER_CMD(cmd);
			Close();
		}
		break;
	case eFubenOpen: //副本开启
		{
			ReqOpenFuBenUserCmd cmd;
			cmd.dwType = static_cast<DWORD>(atoi(st.params[0].c_str()));
			SEND_USER_CMD(cmd);
			this->Close();
		}
		break;
	case eFubenEnter: //副本进入
		{
			ReqEnterFuBenUserCmd cmd;
			cmd.dwType = static_cast<DWORD>(atoi(st.params[0].c_str()));
			SEND_USER_CMD(cmd);
			this->Close();
		}
		break;
	case eFubenLeave: //副本退出
		{
			ReqLeaveFuBenUserCmd cmd;
			SEND_USER_CMD(cmd);
			this->Close();
		}
		break;
	case eJiaZuOpen: //家族地图进入
		{
			ReqOpenJiaZuUserCmd cmd;
			SEND_USER_CMD(cmd);
			this->Close();
		}
		break;
	case eJiaZuBoss: //家族BOSS
		{
			if (GetGameGuiManager()->m_guiJiazuboss && GetGameGuiManager()->m_guiJiazuboss->IsVisible())
			{
				GetGameGuiManager()->m_guiJiazuboss->Close();
			}
			else
			{
				GetGameGuiManager()->AddJiazuboss();
			}
		}
		break;
		//////////////////////////////////////////////////////////////////////////		
	case eHair:
		{
			GetGameGuiManager()->AddHairDlg();
		}
		break;
	case eHead:
		{
			GetGameGuiManager()->AddHeadListDlg();
		}
		break;
	case eSendMail:
		{
			GetGameGuiManager()->AddSendMailDlg();
		}
		break;
	case eRecvMail:
		{
			GetGameGuiManager()->AddRecvMailDlg();
		}
		break;
	case eClothColor:
		{
			GetGameGuiManager()->AddClothColorDlg();
		}
		break;
	case eAuction:	//拍卖
		{
			GetGameGuiManager()->AddAuctionDlg();
		}
		break;
	case eClose:
		Close();
		break;
	case eScriptButton:
		{
			if(st.params.size() > 0)
			{	
				Debug_String(st.params[0].c_str());
				//根据params[0]调用函数map中对应的函数
				//如果失败则调用默认的脚本函数
				int iRes = m_FunMapVoid(st.params[0].c_str());
				if( iRes != 1)
				{
					m_pNpcScript->CallRTVoid(st.params[0].c_str());
				}
			}
		}
		break;
	case eBuySavingBox:
		{
			stBuyObjectNpcTradeUserCmd cmd;
			cmd.dwObjectID = 695;
			cmd.dwNum = 1;
			SEND_USER_CMD(cmd);
			Close();
		}
		break;
	case eSaleHorse:
		{
			GetScene()->GetMainCharacter()->SellHorse();
			Close();
		}
		break;
	case eCompetitionNpc:
		{
			if (g_bSinglePlayer)
			{
				GetGameGuiManager()->AddCompetition(eCompetitionNpc);
				m_pMsgDlg = (void*) GetGameGuiManager()->AddMessageBox("进行智力问答需要花费10两银子，是否确定？",eAction_CompetitionNpc,NULL,c_szMBOKCancel);
			}
			
			stCreateQuiz cmd;

			SEND_USER_CMD(cmd);
			
		}
		break;

	case  eSeptAttackNpc:         //家族争夺NPC战
		{

			m_pMsgDlg = (void*) GetGameGuiManager()->AddMessageBox("发起NPC争夺战需40两，是否确定？",eAction_SeptAttackNpc,(void*)m_dwNpcId,c_szMBOKCancel);
		/*	stDareNpcDare cmd;

			cmd.dwNpcID = m_dwNpcId;

			SEND_USER_CMD(cmd);*/
		}
		break;

	case  eLeaveMarry:
		{
			m_pMsgDlg = (void*) GetGameGuiManager()->AddMessageBox("真的要离婚吗？",eAction_LeaveMarry,(void*)m_dwNpcId,c_szMBOKCancel);
		}
		break;

	case  eSeptNpcCustom:          //家族获得NPC税金
		{
			stDareNpcGetGold cmd;
			CNpc* pNpc = GetScene()->FindNpcByID(m_dwNpcId);
			if(pNpc==NULL) break;

			cmd.dwNpcID = pNpc->GetTypeID();

			SEND_USER_CMD(cmd);
		}
		break;

	case  eTongAttackTown:         //帮会争夺城镇战
		{
			stActiveUnionCityDare cmd;
			cmd.toCountryID = atoi(st.params[0].c_str());
			SEND_USER_CMD(cmd);
		}
		break;

	case  eTongTownCustom:         //帮会获得城镇税金
		{
			stGetUnionCityTax cmd;

			SEND_USER_CMD(cmd);
		}
		break;
	case eEmperorRequestDailyMoney:		//皇帝每天可以在皇城大臣处领取5D金钱作为奖励
		{
			if ( GetScene()->GetMainCharacter()->IsEmperor() )
			{
				GetGameGuiManager()->AddMessageBox("皇帝陛下,你确定要现在领取么？",eAction_EmperorRequDailyMoney,NULL, "现在领取|1|Enter|等会再领|2|Esc");
			}
			else
			{
				GameMessageBox( "对不起，你不是皇帝，无权领取！" );
			}
		}
		break;
	case eKingRequestDailyMoney:		//soke 国王每天可以在大臣处领取5D金子薪资
		{
			if ( GetScene()->GetMainCharacter()->IsKing() )
			{
				GetGameGuiManager()->AddMessageBox("国王陛下,你确定要现在领取么？",eAction_KingRequDailyMoney,NULL, "现在领取|1|Enter|等会再领|2|Esc");
			}
			else
			{
				GameMessageBox( "对不起，你不是国王，无权领取！" );
			}
		}
		break;
	case eYuanshuaiRequestDailyMoney:		//soke 元帅每天可以在大臣处领取3D金子薪资
		{
			if ( GetScene()->GetMainCharacter()->IsYuanshuai() )
			{
				GetGameGuiManager()->AddMessageBox("元帅大人,你确定要现在领取么？",eAction_YuanshuaiRequDailyMoney,NULL, "现在领取|1|Enter|等会再领|2|Esc");
			}
			else
			{
				GameMessageBox( "对不起，你不是元帅，无权领取！" );
			}
		}
		break;
	case eZaixiangRequestDailyMoney:		//soke 宰相每天可以在大臣处领取3D金子薪资
		{
			if ( GetScene()->GetMainCharacter()->IsZaixiang() )
			{
				GetGameGuiManager()->AddMessageBox("宰相大人,你确定要现在领取么？",eAction_ZaixiangRequDailyMoney,NULL, "现在领取|1|Enter|等会再领|2|Esc");
			}
			else
			{
				GameMessageBox( "对不起，你不是宰相，无权领取！" );
			}
		}
		break;
	case eCatcherRequestDailyMoney:		//soke 捕头（左）每天可以在大臣处领取2D金子薪资
		{
			if ( GetScene()->GetMainCharacter()->IsCatcher() )
			{
				GetGameGuiManager()->AddMessageBox("捕头大人,你确定要现在领取么？",eAction_CatcherRequDailyMoney,NULL, "现在领取|1|Enter|等会再领|2|Esc");
			}
			else
			{
				GameMessageBox( "对不起，你不是捕头，无权领取！" );
			}
		}
		break;
	case eDiplomatRequestDailyMoney:		//soke 御史大夫（左）每天可以在大臣处领取2D金子薪资
		{
			if ( GetScene()->GetMainCharacter()->IsDiplomat() )
			{
				GetGameGuiManager()->AddMessageBox("御史大人,你确定要现在领取么？",eAction_CensorRequDailyMoney,NULL, "现在领取|1|Enter|等会再领|2|Esc");
			}
			else
			{
				GameMessageBox( "对不起，你不是御史大夫，无权领取！" );
			}
		}
		break;
	case eCatchexRequestDailyMoney:		//soke 捕头（右）每天可以在大臣处领取2D金子薪资
		{
			if ( GetScene()->GetMainCharacter()->IsCatchex() )
			{
				GetGameGuiManager()->AddMessageBox("捕头大人,你确定要现在领取么？",eAction_CatchexRequDailyMoney,NULL, "现在领取|1|Enter|等会再领|2|Esc");
			}
			else
			{
				GameMessageBox( "对不起，你不是捕头，无权领取！" );
			}
		}
		break;
	case eDiplomaxRequestDailyMoney:		//soke 御史大夫（右）每天可以在大臣处领取2D金子薪资
		{
			if ( GetScene()->GetMainCharacter()->IsDiplomax() )
			{
				GetGameGuiManager()->AddMessageBox("御史大人,你确定要现在领取么？",eAction_CensosRequDailyMoney,NULL, "现在领取|1|Enter|等会再领|2|Esc");
			}
			else
			{
				GameMessageBox( "对不起，你不是御史大夫，无权领取！" );
			}
		}
		break;
	case eRenascenceRequest: //soke 转生
		{
			stTurnRequestCmd cmd;
			SEND_USER_CMD(cmd);
		//	GameMessageBox( "对不起，你不能转生!" );
			
		}
		break;
	case eGraceChangeToExp:
		{
			// [Conglin]
			GetGameGuiManager()->AddMessageBox("文采值换经验OK" );
			break;
		}
	case eSeptMPBuff:			// 领取家族BUFF
		{
			GetGameGuiManager()->AddMessageBox( "领取家族BUFF" );
			break;
		}
	case eEnterBattle:			// 传往战场
		{
			GetGameGuiManager()->AddMessageBox( "传往战场" );
			break;
		}
	case eAddSalary: //领取工资
		{
			GetGameGuiManager()->AddSalary();
			 Close();
		}
		break;
	case eAddChoujiang: //抽奖
		{
			stAddCountryChouCmd cmd;
			SEND_USER_CMD(cmd);
			Close();
		}
		break;	
	case eSeptTownExp:
		{
			//If cur player has right
			if ( strlen( GetScene()->GetMainCharacter()->GetSeptName() ) <= 0 )
				break;
			if ( strlen( GetScene()->GetMainCharacter()->GetUnionName() ) <= 0 )
				break;

			if ( GetScene()->GetMainCharacter()->IsSeptLeader() )
				m_pMsgDlg = (void*) GetGameGuiManager()->AddMessageBox("由于贵帮成功保护了本地的安定，所以你和你的族人可以领取一次经验奖励，请保持在我视线范围之内，否则会领取不到奖励的。你确定要现在领取么？",eAction_AskSeptExp,NULL, "现在领取|1|Enter|再召集人|2|Esc");
			else
				GameMessageBox("对不起,你不是族长，无权领取！");
		}
		break;
	case eChangeCountry:           //更换国家
		{
			GetGameGuiManager()->AddChangeCountryDialog();
		}
		break;

	case eChangeWarField:           //更换战场
		{
			GetGameGuiManager()->AddChangeWarFieldDialog();

			stRequestDareCountryCmd cmd;
			cmd.byType = 0;

			SEND_USER_CMD(cmd);
		}
		break;

	case eChoiceWarField:           //发起国战
		{
			if (GetScene()->GetMainCharacter()->IsKing()==true)
			{
				GetGameGuiManager()->AddChoiceWarFieldDialog();

				stRequestDareCountryCmd cmd;
				cmd.byType = 1;

				SEND_USER_CMD(cmd);
			}
			else
			{
				GameMessageBox("对不起,非国王无法发布此命令！");
			}
		}
		break;

	case eCountryWarQuery:
		{
			GetGameGuiManager()->AddCountryWarQueryDialog();
			
			stRequestDareRecordCmd cmd;
			cmd.byType = DARE_RECORD_RESULT;
			SEND_USER_CMD(cmd);

			cmd.byType = DARE_RECORD_STAT;
			SEND_USER_CMD(cmd);

			cmd.byType = DARE_RECORD_PLAN;
			SEND_USER_CMD(cmd);
		}
		break;

	case eWarOperator:
		{
			if (st.params.size()==1)
			{
				int operatorID = atoi(st.params[0].c_str());
				switch(operatorID) {
				case 1:  // 进入城主的帮会领地
					{
						stEnterUnionCityArea cmd;
						
						SEND_USER_CMD(cmd);
					}
					break;
				case 2: // 查询城市所属
					{
						stQuestionUnionCityInfo cmd;
						cmd.byType = QUESTION_CITY_HOLD;

						SEND_USER_CMD(cmd);
					}
					break;
                case 3: // 当天城市争夺战
					{
						stQuestionUnionCityInfo cmd;
						cmd.byType = QUESTION_CITY_DARE;

						SEND_USER_CMD(cmd);
					}
					break;
				case 4:
					{
						stQuestionNpcDareInfo cmd;
						cmd.byType = QUESTION_NPCDARE_HOLD;
						CNpc* pNpc = GetScene()->FindNpcByID(m_dwNpcId);
						if(pNpc==NULL) break;

						cmd.dwNpcID = pNpc->GetTypeID();

						SEND_USER_CMD(cmd);
					}
					break;
				case 5:
					{
						stQuestionNpcDareInfo cmd;
						cmd.byType = QUESTION_NPCDARE_DARE;
						CNpc* pNpc = GetScene()->FindNpcByID(m_dwNpcId);
						if(pNpc==NULL) break;

						cmd.dwNpcID = pNpc->GetTypeID();

						SEND_USER_CMD(cmd);
					}
					break;
				case 6:
					{
						/*stCancelCountryCmd cmd;
						
						SEND_USER_CMD(cmd);*/

						m_pMsgDlg = (void*) GetGameGuiManager()->AddMessageBox("你确定要叛国并接受处罚么？",eAction_LeaveCountry,NULL,c_szMBOKCancel);
					}
					break;

				}
			}
		}
		break;

	case eHonorToPk:                //荣誉点换PK值
		{	
			m_pMsgDlg = (void*) GetGameGuiManager()->AddMessageBox("五点荣誉值可缩短一分钟的罪恶时间，是否确定？",eAction_HonorToPk,NULL,c_szMBOKCancel);
		}
		break;
	case eBuyItem:
		{
			Assert(st.params.size() == 2);
			if(st.params.size() == 2 )
			{
				stBuyObjectNpcTradeUserCmd cmd;
				cmd.dwObjectID = atoi( st.params[0].c_str() );
				cmd.dwNum = atoi( st.params[1].c_str() );
				SEND_USER_CMD(cmd);
			}			
			Close();
		}
		break;
	case eBuyCartoonPetCmd:
		{
			if(st.params.size() == 1 )
			{
				CCartoonPet::BuyCartoonPet( atoi( st.params[0].c_str() ) );
			}			
			Close();
		}
		break;
	case eSelectMarriageType:
		{
			if(st.params.size() == 1 )
			{
				stMarryStatusCheckCmd cmd;
				cmd.byStep = MARRY_PAY_MONEY + atoi( st.params[0].c_str() );
				SEND_USER_CMD( cmd );
			}			
			Close();
		}
		break;
	case eStock:
		{
			if( CSafetyUser::Instance()->IsSafeGoldStock())
			{
				GetGameGuiManager()->AddClientSystemMessage("金币购买股票,需要验证!");
			}
			else 
				GetGameGuiManager()->AddStockDialog();
		    
				
			Close();
		}
		break;
	case eModifyRevenue: //调税
		{
		       if( !GetScene()->GetMainCharacter()->IsKing() ) 
			       GetGameGuiManager()->AddClientSystemMessage("只有国王才能调税率！");
		       else
		       {
			       CCountry::Instance()->RuquestTax();

			       GetGameGuiManager()->AddModifyRevenueBox();
		       }
		}
		break;
	case eQueryRevenue:  //查询税
		{
			if( m_state != STATE_QUREY_TAX)
			{
				CCountry::Instance()->RuquestTax();
				m_state = STATE_QUREY_TAX;
			}
		}
		break;
	case eQueryCountrySaveBox: //查询国库
		{
			if( m_state != STATE_QUERY_COUNTRY_SAVEBOX)
			{
				CCountry::Instance()->RequestCountrySaveBox();
				m_state = STATE_QUERY_COUNTRY_SAVEBOX;
			}
		}
		break;
	case eGoldExchange://点数兑换金子
		{
			//if( GetScene()->GetMainCharacter()->GetPointNum() == 0 )
			//{
			//	///GetGameGuiManager()->AddMessageBox("你没有点券,不能兑换");
			//	Close();
			//}
			//else
			//{
				OnActionBegin();
				GetScene()->GetMainCharacter()->SetPointNum( 0 );
				stRequestPoint cmd;
				SEND_USER_CMD(cmd);
				GetGameGuiManager()->AddGoldExchangeDlg();
			//}			
		}
		break;
	case eBuyMonthCard://包月
		{
			Close();
			CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
			DWORD dwPoint = pMainRole->GetPointNum();
			char szTemp[MAX_PATH];
			if( dwPoint >= REDEEM_RATE_MONTH_CARD )
			{
				_snprintf(szTemp,MAX_PATH,"你为 %s 购买 游戏包月 服务将从你帐户中扣除 %d 个点券，该效果将持续30天，你是否确定要这样做吗？",pMainRole->GetName(),REDEEM_RATE_MONTH_CARD );
                szTemp[MAX_PATH - 1] = 0;
				GetGameGuiManager()->AddMessageBox(szTemp, eAction_BuyMonthCard, NULL,"确定|1|Enter|取消|2|Esc");
			}
			else
			{
				if( dwPoint == 0 )
				{
                    _snprintf(szTemp,MAX_PATH,"你没有点券,不能包月");
					szTemp[MAX_PATH - 1] = 0;
				}
				else
				{
					_snprintf(szTemp,MAX_PATH,"包月需要 %d 个点券,你的只有 %d 个点券,还不够支付",REDEEM_RATE_MONTH_CARD,dwPoint);
					szTemp[MAX_PATH - 1] = 0;
				}				
				GetGameGuiManager()->AddMessageBox(szTemp);
			}			
		}
		break;
	case eCheckValidQuest:
		{
			stCheckValidQuestUserCmd cmd;
			SEND_USER_CMD(cmd);
			Close();
		}
		break;
	case eApplyConn:
		{
			GetGameGuiManager()->AddApplyConn();
		}
		break;
	case eRecommend:
		{
			GetGameGuiManager()->AddInputBox("请输入推荐人姓名:",eAction_Recommend,NULL);   
		}
		break;

	case eContributeSaveBox: //捐献到国库
		{
			if( m_state != STATE_CONTRIBUTE_COUNTRY)
			{
				OnActionBegin();
				GetGameGuiManager()->AddCountrySaveingBoxDlg();
				GetGameGuiManager()->AddItem();

				m_state = STATE_CONTRIBUTE_COUNTRY;
			}			
		}
		break;
	case eArmymemLevel: // 国王指定等级
		{
			if( !GetScene()->GetMainCharacter()->IsKing() ) 
				GetGameGuiManager()->AddClientSystemMessage("对不起，你不是国王！");
			else 
			{
                             stSelectTransLevelUserCmd  cmd;
			     cmd.dwLevel = atoi(st.params[0].c_str());
			     SEND_USER_CMD(cmd);

			     GetGameGuiManager()->OnNpcActionEnd();

			}
		}
		break;

		/////////////sort///////////////////
	case 	eSeptNativeSort:
		{
			stReqSeptSort cmd;
			cmd.byType =	COUNTRY_SEPT_SORT;
			SEND_USER_CMD(cmd);
#ifdef _DEBUG_1
			GetClient()->PushCommand(&cmd,sizeof(cmd));
#endif 
			m_state= STATE_SEPT_NATIVE_SORT;
		}
		break;
	case 	eSeptWorldSort:
		{
			stReqSeptSort cmd;
			cmd.byType =	WORLD_SEPT_SORT;
			SEND_USER_CMD(cmd);
#ifdef _DEBUG_1
			GetClient()->PushCommand(&cmd,sizeof(cmd));
#endif 
			m_state = STATE_SEPT_WORLD_SORT;
		}
		break;
	case 	eUnionNativeSort:
		{
			stReqUnionSort cmd;
			cmd.byType = COUNTRY_UNION_SORT;
			SEND_USER_CMD(cmd);
#ifdef _DEBUG_1
			GetClient()->PushCommand(&cmd,sizeof(cmd));
#endif 
			m_state = STATE_UNION_NATIVE_SORT;
		}
		break;
	case    eUnionWorldSort:
		{
			stReqUnionSort cmd;
			cmd.byType = WORLD_UNION_SORT;
			SEND_USER_CMD(cmd);
#ifdef _DEBUG_1
			GetClient()->PushCommand(&cmd,sizeof(cmd));
#endif 
			m_state = STATE_UNION_WORLD_SORT ;
		}
		break;
		/////////////end sort///////////////////

	case    eTechResearcher:  //国王指定研究员
		{
			if( !GetScene()->GetMainCharacter()->IsKing() ) 
				GetGameGuiManager()->AddClientSystemMessage("对不起，你不是国王！");
			else 
			{
				OnActionBegin();
				GetGameGuiManager()->AddCountryTechDlg();
				if( GetGameGuiManager()->m_guiCountryTechDlg)
					GetGameGuiManager()->m_guiCountryTechDlg->SetSelTab((CGuiCountryTechDlg::RESEARCHER));

			}
		}
		break;
	case eTechUpdate: // 科技人员升级
		{
				OnActionBegin();
				GetGameGuiManager()->AddCountryTechDlg();
				if( GetGameGuiManager()->m_guiCountryTechDlg)
					GetGameGuiManager()->m_guiCountryTechDlg->SetSelTab((CGuiCountryTechDlg::STUDYING));			
		}
		break;
	case eTechBallot://国名投票
		{
			OnActionBegin();
			GetGameGuiManager()->AddCountryTechDlg();
			if( GetGameGuiManager()->m_guiCountryTechDlg)
				GetGameGuiManager()->m_guiCountryTechDlg->SetSelTab((CGuiCountryTechDlg::BALLOT));
		}
		break;
	case 	eCreateArmy:          // 建立军队
		{

			m_state = STATE_NORMAL;
			
			if(GetScene()->GetMainCharacter()->IsCityLeader() || GetScene()->GetMainCharacter()->IsKing())
				CArmyManager::GetSingleton()->AddCreateArmyGuiDlg();
			else 
				GetGameGuiManager()->AddClientSystemMessage("对不起,你不是城主!");


			//GetGameGuiManager()->AddInputBox("军队名:",eAction_CreateArmy,NULL);   

		}
		break;
	case    eRecruitArmyer:
		{
			if( !GetScene()->GetMainCharacter()->IsGen()) 
			{
				GetGameGuiManager()->AddClientSystemMessage("对不起,你不是将军!");
				break;
			}
			else 
			{
				GetGameGuiManager()->AddInputBox("招收队长:",eAction_RecruitArmy,NULL);
			}
		}
		break;
	case 	eLeaveArmy:            // 离开军队
		{
			m_state = STATE_NORMAL;
			if( !GetScene()->GetMainCharacter()->IsArmyer())
			{
				GetGameGuiManager()->AddClientSystemMessage("你未加入任何军队!");
			}
			else
				GetGameGuiManager()->AddMessageBox("确定离开军队吗？",eAction_LeaveArmy,NULL,c_szMBOKCancel);
		}
		break;
	case 	eQueryArmy:            // 查询军队
		{	
			if(m_state != STATE_ARMY_QUERY)
			{
				m_state = STATE_ARMY_QUERY;
				CArmyManager::GetSingleton()->QuestCityArmyList();
			}
			
		}
		break;

	case eKingStopOneTalk:  // 国王禁止某人发言
		{
			if( !GetScene()->GetMainCharacter()->IsKing() ) 
				GetGameGuiManager()->AddClientSystemMessage("只有国王才能使用禁言！");
			else 
				GetGameGuiManager()->AddInputBox("请输入要要禁言的玩家名字:",eAction_StopOneTalk);
		}
		break;
	case eGoldGive:
		{
			stGoldGiveTradeUserCmd cmd;
			SEND_USER_CMD(cmd);
			//Close();
		}
		break;
	case eGongxunjingcai://功勋竞猜
		{
			
			if( GetGameGuiManager()->m_guigongxun && GetGameGuiManager()->m_guigongxun->IsVisible() )
			{
				GetGameGuiManager()->m_guigongxun->SetVisible(false);
			}	
			else	
			{
				GetGameGuiManager()->AddGongxun();
				GetGameGuiManager()->m_guigongxun->shuaxin(st.params[0].c_str(),st.params[1].c_str());
			}
		}
		break;
	case eTianxia://天下第一
		{
			
			if( GetGameGuiManager()->m_guiTianxia && GetGameGuiManager()->m_guiTianxia->IsVisible() )
			{
				GetGameGuiManager()->m_guiTianxia->SetVisible(false);
			}	
			else	
			{
				GetGameGuiManager()->AddTianxia();
			}
		}
		break;
		
	case eBikeGame://自行车
		{
			stBikeStartCmd cmd;
			SEND_USER_CMD(cmd);
			Close();
		}
		break;
	case eDelBikeGame://自行车
		{
			GetGameGuiManager()->AddMessageBox("确定要注销自行车比赛吗?",eAction_DelBike,NULL, "确定|1|Enter|取消|3|Esc");	
			Close();
		}
		break;
	case eNextBikeGame://自行车
		{
			stBikeScnlCmd cmd;
			cmd.npcid = atoi(st.params[0].c_str());
			SEND_USER_CMD(cmd);
			Close();
		}
		break;
	case eZhanCheList:
		{
			if( GetGameGuiManager()->m_guiZhanchelist && GetGameGuiManager()->m_guiZhanchelist->IsVisible() )
			{
				GetGameGuiManager()->m_guiZhanchelist->SetVisible(false);
			}	
			else	
			{
				GetGameGuiManager()->AddZhanchelist();
			}
		}
		break;
	case eLicai://理財
		{
			
			if( GetGameGuiManager()->m_guilicai && GetGameGuiManager()->m_guilicai->IsVisible() )
			{
				GetGameGuiManager()->m_guilicai->SetVisible(false);
			}	
			else	
			{
				GetGameGuiManager()->AddLicai();
			}
		}
		break;
	case eMiGong://迷宫
		{
			stMigongNextCmd cmd;
			cmd.npcid = atoi(st.params[0].c_str());
			SEND_USER_CMD(cmd);
			Close();
		}
		break;
	case eTianxiaLingqu://天下第一
		{
			
			stTianxiaDuanweiCmd cmd;
			SEND_USER_CMD(cmd);
			Close();
		}
		break;
	case eGiveUnionMoney:
		{
			if (strlen(GetGameGuiManager()->m_guiWorldConn->m_TongInfo.unionName)>0)
			{
				GetGameGuiManager()->AddMoneyInputBox("给帮会捐献二十两可以获得一点功勋值，你要给帮会捐献多少银子？",eAction_GiveUnoinMoney);
			}
			else
			{
				GameMessageBox("你还未加入帮会，无法捐献！");
			}

		}
		break;

	case eQueryUnionMoney:
		{
			if (strlen(GetGameGuiManager()->m_guiWorldConn->m_TongInfo.unionName)>0)
			{
				stReqUnionMoney cmd;
				SEND_USER_CMD(cmd);
			}
			else
			{
				GameMessageBox("你还未加入帮会！");
			}

		}
		break;
#ifdef __SPRING__
	case eSpringCmd:
		{
			CGuiInputBox2Dialog* pDlg = GetGameGuiManager()->AddInputBox2("请输入朋友名字:","请输入祝福语",eAction_SendBlessMessage,NULL);
			if(pDlg)
			{
				pDlg->SetTextLength(20,60);
			}
		}
		break;
#endif
		//Add By David For 2006-3-20
	case eImmure:
		{
			//
			if( !GetScene()->GetMainCharacter()->IsKing() ) 
			{
				GetGameGuiManager()->AddClientSystemMessage("只有国王才能关闭玩家！");
				break;
			}
			//
			GetGameGuiManager()->AddInputBox("请输入你想要关闭玩家名字:",eAction_PrisonCountry );
		}
		break;
	case eConfirmSaleCartoon:
		{
			if( st.params.size() == 1 )
			{
				stSaleCartoonCmd cmd;
				cmd.cartoonID = strtoul(st.params[0].c_str(),NULL,10);
				SEND_USER_CMD(cmd);
				Close();
			}			
		}
		break;
	case eTranslateToMoreExp:
		{

			if( GetGameGuiManager()->m_guiUserInfo)
			{
				bool bEquiped = GetGameGuiManager()->m_guiUserInfo->IsAdornObjectByType(ItemType_Amulet);
				
				if(!bEquiped)
				{
					if( GetScene()->GetMainCharacter()->FindItemItByLocationType(OBJECTCELLTYPE_COMMON,ItemType_Amulet))
					{
						GetGameGuiManager()->AddClientSystemMessage("你还没有装备护身符！");
						break;
					}
					else 
					{
						GetGameGuiManager()->AddClientSystemMessage("你还没有购买护身符,请到金子NPC处购买！");
						break;
					}		
				}
		
				stRequestIncMapPropertyUserCmd  cmd;	 
				SEND_USER_CMD(cmd);
			}
		}
		break;
	case eAntiDareCountry:
		{
			if(!GetScene()->GetMainCharacter()->IsKing())
			{
				GetGameGuiManager()->AddClientSystemMessage("对不起，只有国王才能申请反攻！");
			}
			/*else if(!GetScene()->IsAntiDarkeCountryWarTime())
			{
			        GetGameGuiManager()->AddClientSystemMessage("申请反攻时间为22:40-23:10!");
			}*/
			else
				CCountryConn::Instance()->AddAntiDareCountryDlg(enumAntiDareCountry);
		}
		break;
	case eQuestGoldGive:
		{
			if( m_state != STATE_QUEST_GOLDGIVE_NUM )
			{
				m_state = STATE_QUEST_GOLDGIVE_NUM;
				stRequestGoldGiveTradeUserCmd cmd;
				SEND_USER_CMD(cmd);		
			}			
		}
		break;
	case eQuestDragonSprite:
		{
			stReqDragonUserCmd cmd;
			SEND_USER_CMD( cmd );
		}
		break;
	case eQuestTigerSprite:
		{
			stReqTigerUserCmd cmd;
			SEND_USER_CMD( cmd );
		}
		break;
	case eSeptGiveupOccupy:
		{
			if( !GetScene()->GetMainCharacter()->IsSeptLeader() )
			{
				GetGameGuiManager()->AddClientSystemMessage("你不是族长，无权放弃!");
			}
			else
				GetGameGuiManager()->AddMessageBox("确定放弃当前占领吗？",eAction_SeptGiveupOccupy,NULL,c_szMBOKCancel);
		}
		break;
	case eUnionGiveupOccupy:
		{
			if( !GetScene()->GetMainCharacter()->IsUnionLeader() )
			{
				GetGameGuiManager()->AddClientSystemMessage("你不是帮主，无权放弃!");
			}
			else
				GetGameGuiManager()->AddMessageBox("确定放弃当前占领吗？",eAction_UnionGiveupOccupy,NULL,c_szMBOKCancel);
		}
		break;
	case eRequestItemCardForGift:
		{
			GetGameGuiManager()->AddInputBoxFiveDialog( "请输入你的20位序列号" );
			Close();
		}
		break;
	case eRequestAllyCountry:
		{
                       if( !GetScene()->GetMainCharacter()->IsKing() )
			{
				GetGameGuiManager()->AddClientSystemMessage("对不起，你不是国王!");
			}
			else
			{
				CCountryConn::Instance()->AddAntiDareCountryDlg(enumAllyCountry);
			}
		}
		break;
	case eCancelAllyCountry:
		{
			if( !GetScene()->GetMainCharacter()->IsKing() )
			{
				GetGameGuiManager()->AddClientSystemMessage("对不起，你不是国王!");
			}
			else
			{
				DWORD allyid;
				if (CCountry::Instance()->GetAllyCountry(allyid))
				{
					char szAllyName[50] ={0};
					if( CCountry::Instance()->GetCountryName(allyid,szAllyName))
					{
						char szMsg[120];
						_snprintf(szMsg,120,"确定与%s解除盟国关系吗？",szAllyName);
						szMsg[119] = 0;
						GetGameGuiManager()->AddMessageBox(szMsg,eAction_CancelAllyCountry,(void *)allyid,c_szMBOKCancel);
					        break;
					}
				}				
				GetGameGuiManager()->AddClientSystemMessage("对不起，我国还没有盟国!");
				
			}
		}
		break;
	case eRequestAllyCountryInfo:
		{
		/*	if( !GetScene()->GetMainCharacter()->IsKing() )
			{
				GetGameGuiManager()->AddClientSystemMessage("对不起，你不是国王!");
			}
			else */
			{
				if( m_state != STATE_QUEST_ALLYCOUNTRY_INFO)
				{
					CCountry::Instance()->QuestAllyCountryInfo();
					m_state = STATE_QUEST_ALLYCOUNTRY_INFO;
				}				
			}
		}
		break;
	case eSendOutPrisonCmd:
		{
			stOutPrison cmd;
			SEND_USER_CMD(cmd);
			Close();
		}
		break;
	case eLeavePrison:
		{
			stLeavePrison cmd;
			SEND_USER_CMD(cmd);
			Close();
		}
		break;
	case eDaojuGet:
		{
			int type = atoi(st.params[1].c_str());

			stGoldGiveTradeUserCmd cmd;
			cmd.type = type; 
			SEND_USER_CMD(cmd);
			TRACE("－－※※※※※向服务器请求领取id ＝ %d 的道具材料％％％％－－\n",type);
		}
		break;
	case eOpenBribeInputBox:
		{
			GetGameGuiManager()->AddMoneyInputBox("输入贿赂的银两数:(1锭洗50点pk值)",eAction_InputBribePrisonMoney);
			Close();
		}
		break;
	case eOpenBailInputBox:
		{
			GetGameGuiManager()->AddNameMoneyInputDlg("输入取保就医者名字","输入银两(二两一分钟)",eAction_InputBailPrison);
			Close();
		}
		break;
	case eRequestGiftDetails:
		{
			if(st.params.size() == 1 )
			{
				stGetDetailGift cmd;
				cmd.id = atoi( st.params[0].c_str() );
				SEND_USER_CMD( cmd );
			}			
			Close();
		}
		break;
	case eRequestGiftItem:
		{
			if(st.params.size() == 1 )
			{
				stGetItemGift cmd;
				cmd.id = atoi( st.params[0].c_str() );
				SEND_USER_CMD( cmd );
			}			
			Close();
		}
		break;
	case eRequestWinnerList:
		{
			if(st.params.size() == 1 )
			{
				stGetWinnerGift cmd;
				cmd.id = atoi( st.params[0].c_str() );
				SEND_USER_CMD( cmd );
			}			
			Close();
		}
		break;
	case eQeustForGenInfo:
		{
			if( m_state != STATE_QUEST_FOR_GEN_INFO)
			{
				OnActionBegin();
				m_state = STATE_QUEST_FOR_GEN_INFO;
				CCountryConn::Instance()->AddGenInfoDlg();
				
			}
		}
		break;
	case eFubenjhGenInfo:
		{
			GetGameGuiManager()->AddMessageBox( "你确定要开启副本吗？",eAction_Fubenjhxt,NULL,"确定|1|Enter|取消|2|Esc");
			Close();
		}
		break;
	case eAppointMember:
		{
			int type = atoi(st.params[1].c_str());
			//soke 顺序变了 这里也要修改 新增御史、元帅、宰相
			if( type == 5 || type == 6 || type == 7 || type == 8 || type == 9 || type == 10 || type == 11 || type == 12 || type == 13 || type == 14 || type == 15 || type == 16 || type == 17 || type == 18)
			{
				if(!GetScene()->GetMainCharacter()->IsKing())
				{
					GetGameGuiManager()->AddClientSystemMessage("对不起，您不是国王!");
					break;
				}
			}
			//soke 顺序变了 这里也要修改
			else if(type == 3 || type == 4)
			{
				if(!GetScene()->GetMainCharacter()->IsCityLeader())
				{
					GetGameGuiManager()->AddClientSystemMessage("对不起，您不是城主!");
					break;
				}
			}

			switch(type)
			{
				//3:  // 任命外交官（停用）
				//4:  // 撤消外交官（停用）
				//5:  // 城主任命捕头（停用）
				//6:  // 城主罢免捕头（停用）
				//7:  // 国王任命巡捕（左上）
				//8:  // 国王罢免巡捕（左上）
				//9:  // 国王任命巡捕（左下）
				//10: // 国王罢免巡捕（左下）
				//11: // 国王任命御史大夫（右上）
				//12: // 国王罢免御史大夫（右上）
				//13: // 国王任命御史大夫（右下）
				//14: // 国王罢免御史大夫（右下）
				//15: // 国王任命元帅
				//16: // 国王罢免元帅
				//17: // 国王任命宰相
				//18: // 国王罢免宰相

			case 4: //soke 2016 -3 -11 撤消外交官（停用）
				{
					stCancelDiplomatCmd cmd;
					SEND_USER_CMD(cmd);
				}
				break;
			case 6: //soke 2016 -3 -11 城主撤消巡捕（停用）
				{
					stCancelCityCatcherCmd cmd;
					SEND_USER_CMD(cmd);
				}
				break;
			case 8: //soke 2016 -3 -11 国王罢免巡捕（左上）
				{
					stCancelCatcherCmd cmd;
					SEND_USER_CMD(cmd);
				}
				break;
			case 10: //soke 2016 -3 -11 国王罢免巡捕（左下）
				{
					stCancelCatchexCmd cmd;
					SEND_USER_CMD(cmd);
				}
				break;

			case 12: //soke 2016 -3 -11 国王罢免御史大夫（右上）
				{
					stCancelCensorCmd cmd;
					SEND_USER_CMD(cmd);
				}
				break;
			case 14: //soke 2016 -3 -11 国王罢免御史大夫（右下）
				{
					stCancelCensosCmd cmd;
					SEND_USER_CMD(cmd);
				}
				break;
			case 16: //soke 2016 -3 -11 国王罢免元帅
				{
					stCancelYuanshuaiCmd cmd;
					SEND_USER_CMD(cmd);
				}
				break;
			case 18: //soke 2016 -3 -11 国王罢免宰相
				{
					stCancelZaixiangCmd cmd;
					SEND_USER_CMD(cmd);
				}
				break;
			default:
				AddInputDialog( st.params[0].c_str(), type ); 
				break;
			}	                                

		}
		break;
	case  eQuestOccupierList:
		{
			int type = atoi(st.params[1].c_str());
			switch(type)
			{
			case 1: //占领帮会列表
				{
					stReqCityOwnerListCmd cmd;
					SEND_USER_CMD(cmd);
					//CCountryConn::Instance()->Debug(Usage_OccupierTong);
					
				}
				break;
			case 2://占领家族列表 
				{
					stReqNpcOwnerListCmd  cmd;
					SEND_USER_CMD(cmd);
					//CCountryConn::Instance()->Debug(Usage_OccupierSept);
				}
				break;
			case 3://国王排行表
				{
					//CCountryConn::Instance()->Debug(Usage_KingSort);
					stReqKingListCmd  cmd;
					SEND_USER_CMD(cmd);
				}
				break;
			}
		}
		break;
	case eShowCountryWeak3:
		{		
#ifdef _DEBUG_DLG
			CSafetyUser::Instance()->AddNumDlg(eUsage_OpenSafety);
			CSafetyUser::Instance()->AddOptionSafeDlg();
#else 
			stRequestCountryPowerCmd cmd;
			SEND_USER_CMD( cmd );

#ifdef _DEBUG_WYC_
			stReturnCountryPowerCmd cmd2;
			cmd2.country[0] = 0;
			cmd2.country[1] = 0;
			set_state(cmd2.country,1);
			GetClient()->PushCommand(&cmd2,sizeof(cmd2));
#endif
			Close();
#endif 
		}
		break;
	}
	FUNCTION_END;
}
/*
void CGuiNpcDialog::OnQueryArmy()
{
	BeginAddDialog();

	const tVecArmyBaseInfo& armylist =  CArmyManager::GetSingleton()->GetCityArmyBaseInfo();
	for(c_ArmyBaseInfoIterator citer = armylist.begin() ; citer != armylist.end(); ++citer)
	{
		const stArmyBaseInfo * pArmyInfo = *citer;

		// caption 
		char szLabel[MAX_PATH];
		_snprintf(szLabel,sizeof(szLabel),"%s城军队:\n",pArmyInfo->cityname);
		szLabel[MAX_PATH-1] = 0;

		std::string strObject;
		strObject.append(szLabel);
		AddTalk(strObject.c_str());

		// button 
		_snprintf(szLabel,sizeof(szLabel),"%s \n",pArmyInfo->name);
		szLabel[MAX_PATH-1] = 0;

		m_actions.resize(m_actions.size()+1);
		CGuiNpcDialog::stAction &st = m_actions.back();
		st.actionid = AddButtonItem(szLabel);
		st.action = CGuiNpcDialog::eQueryArmyItem;

		char	data[10]={0};
		sprintf( data, "%u", pArmyInfo->dwArmyID );  // 军队id
		st.params.push_back( data );
		 		
	}
	AddDialogItem("返回","MainDialog");
	EndAddDialog();

}

void CGuiNpcDialog::OnQueryArmyItem(const char * ArmyName)
{
	const tHashMapArmyItem &  armyItemList = CArmyManager::GetSingleton()->GetCityArmyItemList();
	for(c_HashMapArmyItemIteator citer = armyItemList.begin(); citer != armyItemList.end() ;++citer)
	{
	if( stricmp(ArmyName,(*citer).first.c_str()) == 0 )
		{
				BeginAddDialog();

				// caption 
				char szLabel[MAX_PATH];
				_snprintf(szLabel,sizeof(szLabel),"当前城军队:%s\n",ArmyName);
			szLabel[MAX_PATH-1] = 0;

				std::string strObject;
				strObject.append(szLabel);

			strObject.append("队员:\n");
				

                               const stRtnArmyItem & item = (*citer).second;
				for(size_t j = 0 ; j < item.vecBase.size() ;++j)
			{
					_snprintf(szLabel,sizeof(szLabel),"    %s\n",item.vecBase[j].strCaptionName.c_str());
					szLabel[MAX_PATH-1] = 0;
					strObject.append(szLabel);
				}
			AddTalk(strObject.c_str());
				
				AddActionItem("返回",CGuiNpcDialog::eQueryArmy);

				EndAddDialog();
			}
		{	break;
		}
	}	
}*/

void CGuiNpcDialog::ShowRevenue()
{
	if( m_state == STATE_QUREY_TAX )
	{
		char szLabel[20] = {0};
		sprintf(szLabel,"当前税:%d",CCountry::Instance()->GetTax());
		strcat(szLabel,"%");
		szLabel[MAX_PATH-1] = 0;
		BeginAddDialog();
		AddTalk(szLabel);
		AddDialogItem("返回","MainDialog");
		EndAddDialog();
		m_dwNpcId = GetScene()->GetMainCharacter()->GetNpcIdByLast();
		m_state = STATE_NORMAL;
	}	
}

void CGuiNpcDialog::ShowCountrySaveBox()
{

	if( m_state == STATE_QUERY_COUNTRY_SAVEBOX)
	{
		std::string strObject;
		strObject.append("当前国库:\n\n");


		CCountry::stCountryObject & object = CCountry::Instance()->GetObject();

		char szLabel[MAX_PATH];
		
		_snprintf(szLabel,sizeof(szLabel),"银子:%s\n\n",Money2String(object.qwGold));
		szLabel[MAX_PATH-1] = 0;
		strObject.append(szLabel);

		_snprintf(szLabel,sizeof(szLabel),"原料:  %-2d 单位\n",object.qwStock);
		szLabel[MAX_PATH-1] = 0;
		strObject.append(szLabel);	 


		_snprintf(szLabel,sizeof(szLabel),"物资:  %-2d 单位\n",object.qwMaterial);
		  szLabel[MAX_PATH-1] = 0;
		 strObject.append(szLabel);

		BeginAddDialog();
		AddTalk(strObject.c_str());
		AddDialogItem("返回","MainDialog");
		EndAddDialog();
		m_dwNpcId = GetScene()->GetMainCharacter()->GetNpcIdByLast();
		m_state = STATE_NORMAL;
	}
	
}

int CGuiNpcDialog::GetMainSkillType()
{
	if (GetGameGuiManager()->m_guiSkill)
		return (GetGameGuiManager()->m_guiSkill->UpdateCurSerialTypeAccordingToSkillPoints()-1);

	return 0;
}

int CGuiNpcDialog::GetLevel()
{
	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
	if( pMainRole )
	{
		return pMainRole->GetLevel();
	}
	return 1;
}

void CGuiNpcDialog::ShowSavingBox()
{
	OnActionBegin();
	GetGameGuiManager()->AddSavingBoxDlg();
	GetGameGuiManager()->AddItem();
	if(GetGameGuiManager()->m_guiSavingBoxDlg)
		GetGameGuiManager()->m_guiSavingBoxDlg->ResetContents();
	Close();
}

void CGuiNpcDialog::SendTaskCmd(const char* pszTarget,int taskID,int selectID)
{
	stRequestQuestUserCmd	cmd;
	_snprintf(cmd.target,16,pszTarget);
	cmd.target[15]=0;
	cmd.id = taskID;
	cmd.offset = selectID;
	SEND_USER_CMD(cmd);
}

//----------------------------------------------
//----------------------------------------------

/**
 * \brief 重新设置控件位置
 * 
 * 重新设置控件位置
 * 
 * \return 无
 */
void CGuiNpcDialog::ResetControlLocation()
{
	FUNCTION_BEGIN;

	/*GetGameGuiManager()->m_guiNpcDialog->SetSize(362,440);
	if(m_guiTalk)
	{
		m_guiTalk->SetLocation(-20,0);
		m_guiTalk->SetSize(GetClientWidth(),330);
	}*/


	FUNCTION_END;
}


/**
 * \brief 解析NPC脚本
 * 
 * 解析NPC脚本
 * 
 * \param script : 脚本内容
 * \return 无
 */
void CGuiNpcDialog::SetNpcScript(const char* script)
{
	FUNCTION_BEGIN;

	SAFE_DELETE(m_pNpcScript);

	if( NULL == script || strlen(script) < 1 )
	{
		return;
	}

	char pScript[1024*20];//1024*5会导致国王访问大臣npc失败,脚本太长了
	strcpy(pScript,script);

	str_trim_left(pScript);

	TRACE("%s\n",pScript);

	m_pNpcScript = new CLuaState;
	GameScriptInit(m_pNpcScript->GetHandle()); // add by snip3r
	bind_npc_dlg_lua(*m_pNpcScript);
	if(!m_pNpcScript->LoadString(pScript))
	{

#if defined _DEBUG || defined RELEASE_TEST
		Engine_WriteLogF("NPC脚本解析失败\n%s\n",pScript);
		Engine_WriteLog(pScript);
		Engine_WriteLog("\n");
#endif
		return;
	}

	//TRACE("%s\n",pScript);
	m_pNpcScript->SetGlobal("this",this);

	if(!m_pNpcScript->Do())
	{

#if defined _DEBUG || defined RELEASE_TEST
		Engine_WriteLog("************执行NPC脚本异常***************\n");
		Engine_WriteLog(pScript);
		Engine_WriteLog("\n");
		Engine_WriteLog("\n");
#endif
		return;
	}
	GetGuiManager()->RequestFocus(this);
	FUNCTION_END;
}

bool CGuiNpcDialog::IsCanClose()
{
	return m_pNpcScript->Call<bool>("CanClose",true);
	return true;
}

//人物移动是否可以关闭界面
bool CGuiNpcDialog::IsMoveCanClose()
{
	return m_pNpcScript->Call<bool>("MoveCanClose",true);
	return true;
}


/**
 * \brief 消息处理函数
 * 
 * 消息处理函数
 * 
 * \param nEvent : 消息类别
 * \param nID : 控件ID
 * \param pControl : 控件指针
 * \return 消息处理结果
 */
bool CGuiNpcDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		/*for(size_t i=0;i<m_actions.size();++i)
		{
			if(pControl == m_actions[i].button)
			{
				OnNpcAction(i);
				return true;
			}
		}*/
		switch(nID)
		{
		case GUI_CLOSEBOX_ID:
			if( !IsCanClose() ) return true;
			break;
		}
		break;
	case EVENT_ITEM_CLICKED:
		for(size_t i=0;i<m_actions.size();++i)
		{
			if(nID==m_actions[i].actionid)
			{
				OnNpcAction(i);
				return true;
			}
		}
		break;
	case EVENT_SCRIPT_BUTTON_CLICK:
		{
			const char* pFun = (const char* )(LPVOID)nID;
			if(!pFun || pFun[0] == 0) return false;
			m_pNpcScript->CallRTVoid(pFun);
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiNpcDialog::CloseNpcDialog()
{
	Close();
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CGuiNpcDialog::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x ,pt.y + 4);
	}

	FUNCTION_END;
}

void CGuiNpcDialog::PlaySound(const char* pszFileName)
{
	if( NULL == pszFileName || strlen(pszFileName) < 1) 
	{
		return ;
	}	
	
	static char szLastFileName[MAX_PATH] = {0};
	
	if( strlen(szLastFileName) > 0 && GetSoundManager()->IsPlaying(szLastFileName))
	{  
		return;
	}

	SetFrameAllocator fa;
	std::vector<std::stack_string,std::stack_allocator<std::stack_string> > strs;
	_parse_str(strs,pszFileName,',');


	int i = rand() % strs.size();	
	sprintf(szLastFileName,"sound\\ui\\%s",strs[i].c_str() );
	PlayGameSound( szLastFileName,SoundType_UI );
}

int CGuiNpcDialog::AddeSaleHorseCmd(const char* pszLabel)
{
	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eSaleHorse;
	return 0;
}

bool CGuiNpcDialog::IsCanBuyFirstSavingBox()
{
	CMainCharacter* pMainChar = GetScene()->GetMainCharacter();
	if(pMainChar)
	{
		bool bRes = (pMainChar->GetLevel() >= 20) && (pMainChar->GetSavingBoxNum() == 0);
		return bRes;
	}
	return false;
}

bool CGuiNpcDialog::IsCanBuy2ndSavingBox()
{
	CMainCharacter* pMainChar = GetScene()->GetMainCharacter();
	if(pMainChar)
	{
		bool bRes = (pMainChar->GetLevel() >= 40) && (pMainChar->GetSavingBoxNum() == 1);
		return bRes;
	}
	return false;
}

bool CGuiNpcDialog::IsCanBuy3rdSavingBox()
{
	//CMainCharacter* pMainChar = GetScene()->GetMainCharacter();
	//if(pMainChar)
	//{
	//	bool bRes = (pMainChar->GetLevel() >= 80) && (pMainChar->GetSavingBoxNum() == 2);
	//	return bRes;
	//}
	return false;
}

void CGuiNpcDialog::AddBuyItemCmd(const char* pszLabel,int iItemID,int iItemnum)
{
	FUNCTION_BEGIN;

	Assert(iItemID > 0 && iItemID < 99999999);
	Assert(iItemnum > 0 && iItemID < 99999999);

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eBuyItem;

	char	data[10]={0};
	sprintf( data, "%u", iItemID );
	st.params.push_back( data );

	sprintf( data, "%u", iItemnum );
	st.params.push_back(data);

	FUNCTION_END;
}

int  CGuiNpcDialog::GetCartoonPetNum()
{
	FUNCTION_BEGIN;

	return GetScene()->GetCartoonPetList()->GetSize();

	FUNCTION_END;
}

void CGuiNpcDialog::AddBuyCartoonPetCmd(const char* pszLabel,int npcID)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eBuyCartoonPetCmd;

	char	data[10]={0};
	sprintf( data, "%u", npcID );
	st.params.push_back( data );	

	FUNCTION_END;
}

void CGuiNpcDialog::AddSelectMarriageTypeCmd( const char* pszLable,int type )
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLable);
	st.action = CGuiNpcDialog::eSelectMarriageType;

	char	data[10]={0};
	sprintf( data, "%u", type );
	st.params.push_back( data );

	FUNCTION_END;
}


int CGuiNpcDialog::SetSavingBoxPassword()
{
	FUNCTION_BEGIN;

	TRACE("CGuiNpcDialog::SetSavingBoxPassword\n");

	return 1;
	FUNCTION_END;
}

int CGuiNpcDialog::AddHeadListDialog(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eHead;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddSendMailDialog(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eSendMail;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddRecvMailDialog(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eRecvMail;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddAuctionDialog(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eAuction;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddeHonorToPk(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eHonorToPk;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddStock(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eStock;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddGiveUnionMoney(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eGiveUnionMoney;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddApplyConn(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eApplyConn;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddQueryUnionMoney(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eQueryUnionMoney;
	return st.actionid;

	FUNCTION_END;
}

bool CGuiNpcDialog::IsHaveNewMail()
{
	bool bNewMail = GetScene()->GetMainCharacter()->GetMailManager()->bIsHaveNewMail();
	Debug_Bool(bNewMail);
	return bNewMail;
	return false;
}

int  CGuiNpcDialog::GetHorseID()
{
	CPet* pHorse = GetScene()->FindPetByType(PET_TYPE_RIDE);
	if ( pHorse && false == pHorse->bIsDelete() )
	{
		return pHorse->GetNpcID();
	}
	return 0;
}

int CGuiNpcDialog::AddGoldExchangeDialog(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eGoldExchange;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddBuyMonthCardCmd(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eBuyMonthCard;
	return st.actionid;

	FUNCTION_END;
}


////////////////////////////////////////////////////////////////////////////////
// 关于国家开始
////////////////////////////////////////////////////////////////////////////////
int CGuiNpcDialog::AddActionItem(const char * pLabel,const enumAction & Action)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pLabel);
	st.action = Action;
	return st.actionid;

	FUNCTION_END;
}

// 调税
int CGuiNpcDialog::AddModifyRevenue(const char* pLabel)
{
	FUNCTION_BEGIN;
	return AddActionItem(pLabel,CGuiNpcDialog::eModifyRevenue);
	FUNCTION_END;
}

// 查询税务
int CGuiNpcDialog::AddQueryRevenue(const char * pLabel)
{
	FUNCTION_BEGIN;
	return AddActionItem(pLabel,CGuiNpcDialog::eQueryRevenue);
	FUNCTION_END;
}

// 国库管理员
int CGuiNpcDialog::AddCountryBoxGuard(const char* pLabel)
{
	FUNCTION_BEGIN;
	return AddActionItem(pLabel,CGuiNpcDialog::eQueryCountrySaveBox);
	FUNCTION_END;
}

// 国家捐献
int CGuiNpcDialog::AddCountryRevenue(const char* pLabel)
{
	FUNCTION_BEGIN;
	return AddActionItem(pLabel,CGuiNpcDialog::eContributeSaveBox);
	FUNCTION_END;
}

// 国王指派研究员
int CGuiNpcDialog::AddTechResearcher(const char * pLabel)
{
	FUNCTION_BEGIN;
	return AddActionItem(pLabel,CGuiNpcDialog::eTechResearcher);
	FUNCTION_END;
}

// 科技人员升级
int CGuiNpcDialog::AddTechUpdate(const char * pLabel)
{
	FUNCTION_BEGIN;
	return AddActionItem(pLabel,CGuiNpcDialog::eTechUpdate);
	FUNCTION_END;
}

// 国民投票选举科技人员
int CGuiNpcDialog::AddTechBallot(const char * pLabel)
{
	FUNCTION_BEGIN;
	return AddActionItem(pLabel,CGuiNpcDialog::eTechBallot);
	FUNCTION_END;
}

//soke 拆除太庙资源
int CGuiNpcDialog::RecvTMTask( const char* pLabel )
{
	FUNCTION_BEGIN;
	//AddTalk( pLabel );
	return AddActionItem(pLabel,CGuiNpcDialog::eFetchPearl);
	FUNCTION_END;
}

//soke 建设太庙
int CGuiNpcDialog::ReTTMTask( const char* pLabel )
{
	FUNCTION_BEGIN;
	return AddActionItem(pLabel,CGuiNpcDialog::ePutPearl);
	FUNCTION_END;
}

//soke 查看太庙资源
int CGuiNpcDialog::AddViewTMListBoard( const char* pszLabel )
{
	FUNCTION_BEGIN;
	return AddActionItem(pszLabel,CGuiNpcDialog::eOpenPearlRank);
	FUNCTION_END;
}

//soke 更换太庙资源
int CGuiNpcDialog::ExcRecvTMTask( const char* pLabel )
{
	FUNCTION_BEGIN;
	//AddTalk( pLabel );
	return AddActionItem(pLabel,CGuiNpcDialog::eFetch2Pearl);
	FUNCTION_END;
}

int CGuiNpcDialog::AddGoToMaleHotspringItem( const char* pzText )
{
	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pzText );
	st.action = CGuiNpcDialog::eSeptGoToMale;
	return st.actionid;
}

int CGuiNpcDialog::AddGoToFemaleHotspringItem( const char* pzText )
{
	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pzText );
	st.action = CGuiNpcDialog::eSeptGoToFemale;
	return st.actionid;
}

int CGuiNpcDialog::AddExitHotspringItem( const char* pzText )
{
	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pzText );
	st.action = CGuiNpcDialog::eSeptExitHotspring;
	return st.actionid;
}

// 建立军队
int CGuiNpcDialog::CreateArmy(const char * pLabel)
{
	FUNCTION_BEGIN;
	return AddActionItem(pLabel,CGuiNpcDialog::eCreateArmy);
	FUNCTION_END;
}

// 离开军队
int CGuiNpcDialog::LeaveArmy(const char * pLabel)
{
	FUNCTION_BEGIN;
	return AddActionItem(pLabel,CGuiNpcDialog::eLeaveArmy);
	FUNCTION_END;
}

// 查询军队
int CGuiNpcDialog::QueryArmy(const char * pLabel)
{
	FUNCTION_BEGIN;
	return AddActionItem(pLabel,CGuiNpcDialog::eQueryArmy);
	FUNCTION_END;
}

// 招收队长
int CGuiNpcDialog::RecruitArmyer(const char * pLabel)
{
	FUNCTION_BEGIN;
	return AddActionItem(pLabel,CGuiNpcDialog::eRecruitArmyer);
	FUNCTION_END;
}

int CGuiNpcDialog::ArmymenLevel(const char * pszLabel,int nLevel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eArmymemLevel;

	char	data[10]={0};
	sprintf( data, "%u", nLevel );
	st.params.push_back( data );

	return st.actionid;

	FUNCTION_END;
}

// 国王禁止某人发言
int CGuiNpcDialog::KingStopOneTalk(const char * pszLabel)
{
	FUNCTION_BEGIN;
	return AddActionItem(pszLabel,CGuiNpcDialog::eKingStopOneTalk);
	FUNCTION_END;
}

////////////////////////////////////////////////////////////////////////////////
// 关于国家结束
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// 填写推荐人
////////////////////////////////////////////////////////////////////////////////

int CGuiNpcDialog::AddRecommend(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eRecommend;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddCheckValidQuestCmd(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eCheckValidQuest;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddGoldGiveCmd(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eGoldGive;
	return st.actionid;

	FUNCTION_END;
}

//功勋竞猜
int CGuiNpcDialog::AddExpGame(const char* text1,const char* text2,const char* text3)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(text1);
	st.action = CGuiNpcDialog::eGongxunjingcai;
	st.params.push_back( text2 );	
	st.params.push_back( text3 );	


	return st.actionid;

	FUNCTION_END;
}


//自行车
int CGuiNpcDialog::StartBikeGame(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eBikeGame;
	return st.actionid;


	FUNCTION_END;
}


//自行车
int CGuiNpcDialog::DeleteBikeRance(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eDelBikeGame;
	return st.actionid;


	FUNCTION_END;
}


//访问赛车女郎
int CGuiNpcDialog::NextBikeGame(const char* text1,const char* text2)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(text1);
	st.action = CGuiNpcDialog::eNextBikeGame;
	st.params.push_back( text2 );	

	return st.actionid;

	FUNCTION_END;
}

//理财
int CGuiNpcDialog::AddForTuneFundDlg(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eLicai;
	return st.actionid;


	return st.actionid;

	FUNCTION_END;
}

//天下第一
int CGuiNpcDialog::AddTianxiaDlg(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eTianxia;
	return st.actionid;

	FUNCTION_END;
}


//天下第一
int CGuiNpcDialog::AddTianxiaLingquDlg(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eTianxiaLingqu;
	return st.actionid;

	FUNCTION_END;
}

/**
* \brief 家族战车列表
* 
* 
* \param pszLabel : 选项标题
* \return NPC功能按纽的ID
*/
int CGuiNpcDialog::AddZhanCheListDlg(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eZhanCheList;
	
	return st.actionid;

	FUNCTION_END;
}



//迷宫
int CGuiNpcDialog::AddMiGong(const char* text1,const char* text2)
{
	FUNCTION_BEGIN;
	
	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(text1);
	st.action = CGuiNpcDialog::eMiGong;
	st.params.push_back( text2 );	


	return st.actionid;

	FUNCTION_END;
}

void CGuiNpcDialog::AddSaleCartoonPetCmd(int cartoonindex,const char* pszFunction) //正常副本
{
	FUNCTION_BEGIN;

	CCartoonPet* pCartoon = GetScene()->GetCartoonPetList()->GetAt(cartoonindex - 1);
	if( pCartoon )
	{
		char szTemp[32] = {0};
		_snprintf(szTemp,32,"%u",pCartoon->GetCartoonID());
		szTemp[31] = 0;
		AddDialogItemEx(pCartoon->GetName(),pszFunction,szTemp);
	}	

	FUNCTION_END;
}

void CGuiNpcDialog::ConfirmSaleCartoonPet(const char* pszLabel,const char* pszCartoonID)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	int id = atoi(pszCartoonID);
	CCartoonPet* pCartoon = GetScene()->GetCartoonPetList()->GetCartoon(id);
	if( pCartoon && pCartoon->GetData().time > 0 )
	{
		const char* pszTime = Second2String(pCartoon->GetData().time);
		char szTemp[256];
		_snprintf(szTemp,256,"<p><n color=\"255,0,0,255\">%s 还有 %s的修炼时间</n></p>",pCartoon->GetName(),pszTime);
		AddTalk(szTemp);
	}
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eConfirmSaleCartoon;
	st.params.push_back( pszCartoonID );	

	FUNCTION_END;
}

#ifdef __SPRING__ 
int CGuiNpcDialog::AddSpringCmdItem(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eSpringCmd;
	return st.actionid;
	FUNCTION_END;
}
#endif

//Add By David For 2006-3-20
int CGuiNpcDialog::AddImmureDlg( const char* pszLabel )
{
	FUNCTION_BEGIN;

	//
	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eImmure;
	return st.actionid;

	//
	FUNCTION_END;
}


int CGuiNpcDialog::TranslateToMoreExp(const char * pszLabel)
{
	FUNCTION_BEGIN;
 
	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eTranslateToMoreExp;
	st.params.push_back( pszLabel );
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AntiDareCountry(const char * pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eAntiDareCountry;
	st.params.push_back( pszLabel );
	return st.actionid;

	FUNCTION_END;
}



int  CGuiNpcDialog::QuerySeptAndUnionSort(const char * pszLabel,int type)
{
        FUNCTION_BEGIN;

	assert(type == 1 || type == 2 || type == 3 || type == 4);

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
 
	if( type == 1)  
	{
		st.action = CGuiNpcDialog::eSeptNativeSort;
	}
	else if( type == 2 )
	{
		st.action = CGuiNpcDialog::eSeptWorldSort;
	}
	else if (type == 3)
	{
		st.action = CGuiNpcDialog::eUnionNativeSort;
	}
	else if(type == 4)
	{
		st.action = CGuiNpcDialog::eUnionWorldSort;
	}

	char	data[10]={0};
	sprintf( data, "%u", type );
	data[9] = 0;
	st.params.push_back( data );

	return st.actionid;

	FUNCTION_END;
}

void CGuiNpcDialog::ShowSeptNativeSort()
{
     if(STATE_SEPT_NATIVE_SORT == m_state)    
     {
	     std::string strObject;
	     strObject.append("本国家族排行榜:\n\n");
	     strObject.append("家族名称\t家族声望\n\n");

	 

	     CCountry::SEPTSORTINFO & septlist = CCountry::Instance()->GetSeptNativeSort();

	     for(size_t i = 0 ; i < septlist.size() ; i++)
	     {
		     const stSeptSortInfo & sept = septlist[i];

		     char szLabel[MAX_PATH];
		     _snprintf(szLabel,sizeof(szLabel),"%-15s",sept.septName);
		     szLabel[MAX_PATH-1] = 0 ;
		     strObject.append(szLabel);

		     
		     _snprintf(szLabel,sizeof(szLabel),"%-8d\n",sept.dwRepute);
		     szLabel[MAX_PATH-1] = 0;
		     strObject.append(szLabel);	 
 

		     BeginAddDialog();
		     AddTalk(strObject.c_str());
		     AddDialogItem("返回","MainDialog");
		     EndAddDialog();
		     m_dwNpcId = GetScene()->GetMainCharacter()->GetNpcIdByLast();
		     m_state = STATE_NORMAL;
	     }
	  
     }
}

void CGuiNpcDialog::ShowSeptWorldSort()
{
	if(STATE_SEPT_WORLD_SORT == m_state)    
	{
		CCountry::SEPTSORTINFO & septlist = CCountry::Instance()->GetSeptWorldSort();



		std::string strObject;
		strObject.append("世界家族排行榜:\n\n");
		strObject.append("家族名称\t家族声望\t国家\n\n");


		for(size_t i = 0 ; i < septlist.size() ; i++)
		{
			const stSeptSortInfo & sept = septlist[i];

			char szLabel[MAX_PATH];
			_snprintf(szLabel,sizeof(szLabel),"%-15s",sept.septName);
			szLabel[MAX_PATH-1] = 0 ;
			strObject.append(szLabel);


			_snprintf(szLabel,sizeof(szLabel),"%-8d",sept.dwRepute);
			szLabel[MAX_PATH-1] = 0;
			strObject.append(szLabel);	 

			char szCountryName[10] = {0};
			CCountry::Instance()->GetCountryName(sept.dwCountryID,szCountryName);
			szCountryName[9] = 0;
			_snprintf(szLabel,sizeof(szLabel),"%-4s\n",szCountryName);
			szLabel[MAX_PATH-1] = 0;
			strObject.append(szLabel);

			BeginAddDialog();
			AddTalk(strObject.c_str());
			AddDialogItem("返回","MainDialog");
			EndAddDialog();
			m_dwNpcId = GetScene()->GetMainCharacter()->GetNpcIdByLast();
			m_state = STATE_NORMAL;
		}


	}
}

void CGuiNpcDialog::ShowUnionNativeSort()
{
	if(STATE_UNION_NATIVE_SORT == m_state)    
	{
		CCountry::UNIONSORTINFO & unionlist = CCountry::Instance()->GetUnionNativeSort();

		std::string strObject;
		strObject.append("本地帮会排行榜:\n\n");
		strObject.append("帮会名称\t帮会威望\n\n");


		for(size_t i = 0 ; i < unionlist.size() ; i++)
		{
			const stUnionSortInfo & uu = unionlist[i];

			char szLabel[MAX_PATH];
			_snprintf(szLabel,sizeof(szLabel),"%-15s",uu.unionName);
			szLabel[MAX_PATH-1] = 0 ;
			strObject.append(szLabel);


			_snprintf(szLabel,sizeof(szLabel),"%-8d\n",uu.dwMana);
			szLabel[MAX_PATH-1] = 0;
			strObject.append(szLabel);	 


			BeginAddDialog();
			AddTalk(strObject.c_str());
			AddDialogItem("返回","MainDialog");
			EndAddDialog();
			m_dwNpcId = GetScene()->GetMainCharacter()->GetNpcIdByLast();
			m_state = STATE_NORMAL;
		}

	}
}

void CGuiNpcDialog::ShowUnionWorldSort()
{
	if(STATE_UNION_WORLD_SORT == m_state)    
	{
		CCountry::UNIONSORTINFO & unionlist = CCountry::Instance()->GetUnionWorldSort();


		std::string strObject;
		strObject.append("世界帮会排行榜:\n\n");
		strObject.append("帮会名称\t帮会威望\t国家\n\n");


		for(size_t i = 0 ; i < unionlist.size() ; i++)
		{
			const stUnionSortInfo & uu = unionlist[i];

			char szLabel[MAX_PATH];
			_snprintf(szLabel,sizeof(szLabel),"%-15s",uu.unionName);
			szLabel[MAX_PATH-1] = 0 ;
			strObject.append(szLabel);


			_snprintf(szLabel,sizeof(szLabel),"%-8d",uu.dwMana);
			szLabel[MAX_PATH-1] = 0;
			strObject.append(szLabel);	 

			char szCountryName[10] = {0};
			CCountry::Instance()->GetCountryName(uu.dwCountryID,szCountryName);
			szCountryName[9] = 0;
			_snprintf(szLabel,sizeof(szLabel),"%-4s\n",szCountryName);
			szLabel[MAX_PATH-1] = 0;
			strObject.append(szLabel);

			BeginAddDialog();
			AddTalk(strObject.c_str());
			AddDialogItem("返回","MainDialog");
			EndAddDialog();
			m_dwNpcId = GetScene()->GetMainCharacter()->GetNpcIdByLast();
			m_state = STATE_NORMAL;
		}
	}
}

//////////////end sort////////////////////////
void  CGuiNpcDialog::AddQuestGoldGiveNum(const char * pszLabel)
{
	AddActionItem(pszLabel,CGuiNpcDialog::eQuestGoldGive);
}

void CGuiNpcDialog::ShowGoldGiveNum(int Storn_num,int Matarial_num,int Card_num)
{	
	//if(STATE_QUEST_GOLDGIVE_NUM == m_state)    
	{ 
		std::string strObject;

		char szLabel[MAX_PATH];
		_snprintf(szLabel,sizeof(szLabel),"请你领取道具卡物品\n\n\n\n");
		szLabel[MAX_PATH-1] = 0 ;
		strObject.append(szLabel); 

		BeginAddDialog();
		AddTalk(strObject.c_str());

		AddDaojuGetBtn("  精致升级宝石",STORN);
		AddDaojuGetBtn("  银",SIVER);
		AddDaojuGetBtn("  丝绸",SILK);
		AddDaojuGetBtn("  水晶",CRYSTAL);
		AddDaojuGetBtn("  乌木",EBONY);
		AddDaojuGetBtn("  硬皮",YINGPI);
		
		char temp[256];
		sprintf(temp,"剩余精致宝石数：  %d颗\n",Storn_num);
		AddTalk(temp);
		sprintf(temp,"剩余材料：  %d组\n",Matarial_num);
		AddTalk(temp);
		sprintf(temp,"剩余道具卡数：  %d张\n",Card_num);
		AddTalk(temp);

		AddDialogItem("  返回","MainDialog");
		
		EndAddDialog();
		m_dwNpcId = GetScene()->GetMainCharacter()->GetNpcIdByLast();
		m_state = STATE_NORMAL;
	}
}

int CGuiNpcDialog::AddSendOutPrisonCmd(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eSendOutPrisonCmd;
	st.params.push_back( pszLabel );
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddOpenBribeInputBox(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eOpenBribeInputBox;
	st.params.push_back( pszLabel );
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddOpenBailPersonInputBox(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eOpenBailInputBox;
	st.params.push_back( pszLabel );
	return st.actionid;

	FUNCTION_END;
}


int	 CGuiNpcDialog::RequestGiftList()
{
	stGetListGift cmd;
	SEND_USER_CMD(cmd);
	return 1;
}

inline DWORD GetGiftItemColorByState(BYTE state)
{
	switch( state )
	{
	case 0: return COLOR_ARGB(255,255,255,0);
	case 1: return COLOR_ARGB(255,0,255,0);
	case 2: return COLOR_ARGB(255,255,0,0);	
	default:break;
	}
	return -1;
}

void CGuiNpcDialog::AddGiftListByCmd(stListGift* pCmd)
{
	FUNCTION_BEGIN;

	BeginAddDialog();
	AddTalk("活动列表:");
	if( pCmd->num > 0 )
	{
		for( int i = 0; i < pCmd->num; i++ )
		{
			m_actions.resize(m_actions.size()+1);
			CGuiNpcDialog::stAction &st = m_actions.back();

			char szGiftName[MAX_NAMESIZE];
			_snprintf(szGiftName,MAX_NAMESIZE,"%s",pCmd->titles[i].name);
			szGiftName[MAX_NAMESIZE - 1] = 0;

			st.actionid = AddButtonItemEx(szGiftName,GetGiftItemColorByState(pCmd->titles[i].state));
			//st.actionid = AddButtonItem(szGiftName);
			st.action = CGuiNpcDialog::eRequestGiftDetails;

			char	data[10]={0};
			sprintf( data, "%u", pCmd->titles[i].id );
			data[9] = 0;
			st.params.push_back( data );
		}
	}
	else
	{
		AddTalk("暂无");
	}
	AddCloseItem("关闭");

	EndAddDialog();
	m_dwNpcId = GetScene()->GetMainCharacter()->GetNpcIdByLast();
	m_state = STATE_NORMAL;

	FUNCTION_END;
}

void CGuiNpcDialog::AddGiftDetailByCmd(stDetailGift* pCmd)
{
	FUNCTION_BEGIN;

	BeginAddDialog();
	AddTalk(pCmd->text);
	{
		m_actions.resize(m_actions.size()+1);
		CGuiNpcDialog::stAction &st = m_actions.back();
		st.actionid = AddButtonItem("领取奖品");
		st.action = CGuiNpcDialog::eRequestGiftItem;	
		char	data[10]={0};
		sprintf( data, "%u", pCmd->id );
		data[9] = 0;
		st.params.push_back( data );
	}
	{

		m_actions.resize(m_actions.size()+1);
		CGuiNpcDialog::stAction &st = m_actions.back();
		st.actionid = AddButtonItem("察看获奖列表");
		st.action = CGuiNpcDialog::eRequestWinnerList;	
		char	data[10]={0};
		sprintf( data, "%u", pCmd->id );
		data[9] = 0;
		st.params.push_back( data );		
	}

	AddCloseItem("关闭");
	EndAddDialog();
	m_dwNpcId = GetScene()->GetMainCharacter()->GetNpcIdByLast();
	m_state = STATE_NORMAL;

	FUNCTION_END;
}

void CGuiNpcDialog::AddWinnerListByCmd(stWinnerGift* pCmd)
{
	FUNCTION_BEGIN;

	BeginAddDialog();
	char * p = pCmd->winners;
	AddTalk("获奖名单:");
	if( pCmd->num > 0 )
	{
		for( int i = 0; i < pCmd->num; i++ )
		{
			m_actions.resize(m_actions.size()+1);
			CGuiNpcDialog::stAction &st = m_actions.back();
			char szName[MAX_NAMESIZE*2];
			_snprintf(szName,MAX_NAMESIZE*2,"%s",p);
			AddTalk(szName);
			p += MAX_NAMESIZE;
		}
	}
	else
	{
		AddTalk("暂无");
	}
	AddCloseItem("关闭");

	EndAddDialog();
	m_dwNpcId = GetScene()->GetMainCharacter()->GetNpcIdByLast();
	m_state = STATE_NORMAL;

	FUNCTION_END;
}

void CGuiNpcDialog::RefreshAllyCountryInfo()
{
	FUNCTION_BEGIN;

	if( m_state == STATE_QUEST_ALLYCOUNTRY_INFO)
	{
		m_state = STATE_NORMAL;
		DWORD dwAllyID;
		if( !CCountry::Instance()->GetAllyCountry(dwAllyID))
			return ;
		char szAllyName[40] = {0};
		if( !CCountry::Instance()->GetCountryName(dwAllyID,szAllyName) )
			return;
		szAllyName[39] = 0;
		
		char szLabel[200] = {0};
		sprintf(szLabel,"当前盟国:%s,友好度为:%d",szAllyName,CCountry::Instance()->GetAllyCountryFriendDegree());
		szLabel[199] = 0;

		BeginAddDialog();
		AddTalk(szLabel);
		AddDialogItem("返回","MainDialog");
		EndAddDialog();

		m_dwNpcId = GetScene()->GetMainCharacter()->GetNpcIdByLast();
		
	}

FUNCTION_END;
}

int CGuiNpcDialog::AddLeavePrisonCmd(const char* pszLable)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLable);
	st.action = CGuiNpcDialog::eLeavePrison;
	st.params.push_back( pszLable );
	return st.actionid;

	FUNCTION_END;
}

//领取道具卡转换的物品菜单项
int CGuiNpcDialog::AddDaojuGetBtn(const char* pszLable,int typeID)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	//st.actionid = AddButtonItem(pszLable);
	st.actionid = AddButtonItemEx(pszLable,COLOR_ARGB(255,0,255,0));
	st.action = CGuiNpcDialog::eDaojuGet;
	st.params.push_back( pszLable );
	char sz[32];
	sprintf(sz,"%d",typeID);
	st.params.push_back( sz );
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddQueryGenInfo(const char * pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eQeustForGenInfo;
	st.params.push_back( pszLabel );
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddInputDialog( const char* pszText,int type )
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->AddInputBox(pszText,eAction_NpcDlgInput,(void*)type );	
	return true;
	FUNCTION_END;
}

int CGuiNpcDialog::SendGoTrainUserCmd(const char* pszLevel)
{
	FUNCTION_BEGIN;
	int level = atoi(pszLevel);
	if( level > 0 && level < 10 )
	{
		stGoTrainPropertyUserCmd cmd;
		cmd.level = level;
		SEND_USER_CMD(cmd);
		GetGameGuiManager()->EndNpcDialog();
	}
	return true;
}

int CGuiNpcDialog::SendGoFubenUserCmd(const char* pszLevel)
{
	FUNCTION_BEGIN;
	int level = atoi(pszLevel);
	if( level > 0 && level < 4 ) //soke 共3层
	{
		stGoFubenPropertyUserCmd cmd;
		cmd.level = level;
		SEND_USER_CMD(cmd);
		GetGameGuiManager()->EndNpcDialog();
	}
	return true;
}

int CGuiNpcDialog::AdFubenjhGenInfo(const char * pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eFubenjhGenInfo;
	st.params.push_back( pszLabel );
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddAppointMember(const char* pszLabel,int type)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eAppointMember;
	char szParam[3];
	st.params.push_back(pszLabel);
	_snprintf(szParam,sizeof(szParam),"%d",type);
	szParam[sizeof(szParam)-1] = 0;
	st.params.push_back( szParam ); 
	return st.actionid;

	FUNCTION_END;

}

int CGuiNpcDialog::AddQuestOccupierList(const char* pszLabel,int type)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eQuestOccupierList;
	char szParam[3];
	st.params.push_back(pszLabel);
	_snprintf(szParam,sizeof(szParam),"%d",type);
	szParam[sizeof(szParam)-1] = 0;
	st.params.push_back( szParam ); 
	return st.actionid;

	FUNCTION_END;

}

int CGuiNpcDialog::AddShowCountryWeak3(const char* pszLabel)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eShowCountryWeak3;
	st.params.push_back( pszLabel );

	//GetGameGuiManager()->AddMessageBox("当前最弱的三个国家 : ");
	
	return true;

	FUNCTION_END;
}


//改造属性
int CGuiNpcDialog::AddModifyAttri(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eModifyAttri;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}

//重新绑定
int CGuiNpcDialog::AddReBind(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eAddReBind;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}
//修改签名
int CGuiNpcDialog::AddReMaker(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eAddReMaker;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}
//装备签名
int CGuiNpcDialog::AddAddMaker(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eAddAddMaker;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}
//修改灵魂属性
int CGuiNpcDialog::AddModifySoulA(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eAddModifySoulA;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}
//保留灵魂锁链改造
int CGuiNpcDialog::AddLVSoullineMD(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eAddLVSoullineMD;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}
//修改第二灵魂属性
int CGuiNpcDialog::AddModifyDxSoulA(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eAddModifyDxSoulA;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}
//修改技能属性
int CGuiNpcDialog::AddMDSkillA(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eAddMDSkillA;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}

//非转生装备改造成转生装备
int CGuiNpcDialog::AddEquipRound(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eAddEquipRound;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}

//马匹装备重新锻造
int CGuiNpcDialog::OpenHorseEquipAttributeModify(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eAddMDMake;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}

//========[sky end]===========

//装备升级
int CGuiNpcDialog::AddEquipLV(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk)
{
	FUNCTION_BEGIN;

	m_actions.resize(m_actions.size()+1);
	CGuiNpcDialog::stAction &st = m_actions.back();
	st.actionid = AddButtonItem(pszLabel);
	st.action = CGuiNpcDialog::eAddEquipMaklv;
	st.params.push_back(pszBuyObjects);
	st.params.push_back(pszTalk);
	st.params.push_back(pszLabel);
	return st.actionid;

	FUNCTION_END;
}

//========[sky 马匹训练师]====

int CGuiNpcDialog::AddHorseTraining_one( const char* pzText )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pzText );
	st.action = CGuiNpcDialog::eAddHorseTraining_one;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddHorseTraining_two( const char* pzText )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pzText );
	st.action = CGuiNpcDialog::eAddHorseTraining_two;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::AddHorseTraining_three( const char* pzText )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pzText );
	st.action = CGuiNpcDialog::eAddHorseTraining_three;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::ClearHorseTraining( const char* pzText )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pzText );
	st.action = CGuiNpcDialog::eClearHorseTraining;
	return st.actionid;

	FUNCTION_END;
}

int CGuiNpcDialog::ExchangeExp( const char *pzText )
{
	FUNCTION_BEGIN;

	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pzText );
	st.action	= CGuiNpcDialog::eExchangeExp;
	return st.actionid;

	FUNCTION_END;
}
//========[sky end]==========
//soke 使用工资移民
int CGuiNpcDialog::AddChangeCountryBySalary( const char* pzText )
{
	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pzText );
	st.action = CGuiNpcDialog::eChangeCountryBySalary;
	return st.actionid;
}

//soke 领取工资
int CGuiNpcDialog::AddSalaryDialog( const char* pzText )
{
	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pzText );
	st.action = CGuiNpcDialog::eAddSalary;
	return st.actionid;
}

//soke 抽奖
int CGuiNpcDialog::AddChouJiang( const char* pzText )
{
	m_actions.resize( m_actions.size() + 1 );
	CGuiNpcDialog::stAction& st = m_actions.back();
	st.actionid = AddButtonItem( pzText );
	st.action = CGuiNpcDialog::eAddChoujiang;
	return st.actionid;
}
