#pragma once
#include "character.h"
#include "AStar.h"
#include "./Object.h"
#include "./command.h"
#include "./SceneImage.h"
#include "Task.h"
#include "Mail.h"
#include "MulFunction.h"

class CRoleItem;
class CItem;
class CRoleSkill;

#define SET_PROPERTY_NUM		10
struct stSetObjectPropertyActive
{
	bool	bActive[SET_PROPERTY_NUM];
	stSetObjectPropertyActive()
	{
		Init();
	}

	void Init()
	{
		for( int i=0;i<SET_PROPERTY_NUM;i++ )
			bActive[i] = false;
	}
};


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param mainFive : 描述
 * \param five : 描述
 * \return 返回值的描述
 */
extern enumFiveType g_aMainFive[5][FIVETYPENUM];
extern const char* c_szFiveName[];
extern const char* c_szFiveToOtherName[];
inline bool IsFiveInMainFive(enumFiveType mainFive,enumFiveType five)
{
	for(size_t i=0;i<FIVETYPENUM;++i)
	{
		if(g_aMainFive[mainFive][i] == five)
			return true;
	}
	return false;
}

struct t_SavingBox
{
	BYTE m_num;
	BYTE days[3];//目前最多3个仓库

	t_SavingBox()
	{
		m_num = 0;
		memset(days,0,sizeof(BYTE)*3);
	}

	void Set(stStoreInfoNpcTradeUserCmd* pcmd)
	{
		m_num = pcmd->page;

		for(int i = 0;i < pcmd->page;++i)
		{
			if( pcmd->page > 3) break;
			days[i] = pcmd->day[i];

		}
	}
};

typedef void (*tOnGotoFinalEventFunc)(void* pParam);
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 角色
 */
class CMainCharacter :
	public CCharacter
{
public:

	IBitmaps* m_image;
	POINT m_pt;
	DWORD m_startTime;

	t_MainUserData m_data;
	CAStar m_AStar;
	CAStar m_AStarZone;

	DWORD m_dwSendMoveTime;
	bool  m_bSendMove;
	bool  m_bNoRunState;
	bool  m_bDieDlgWaiting;
	DWORD m_dwDeathType;
	long  m_nDieDlgWaitingTime;
	DWORD m_dwScreenID;
	
	///是否拥有马匹
	bool m_bHasHorse;

	//Be free move state
	bool m_bFreeMoveState;
	bool m_bLocalSpaceLButtonDown;
	
	//自动打怪时间 随机传送
	DWORD autotime;

	///仓库
	t_SavingBox m_savingBox;
	DWORD		m_dwPoint;		//点数
	DWORD		m_dwMonthCard;	//月卡
	//ZH
	bool bNewItem;
	//////////////////////////////////////////////////////////////////////////
	
	//PK mode
	BYTE m_PkMode;

	///技能公共起始时间
	DWORD m_skillStartTime;
	
	DWORD m_dwNpcIdByLast;
	CTask	m_task;
	CMailManager	m_mail;
	///首选快捷键组
	BYTE		m_preferAccessGroup;
	int			m_curSkillTreePoints;

	bool m_bFirstTimeChangePath;

	bool m_bRushOut;
	DWORD m_rushOutStartTime;

	CSceneImage m_moveTarget;
	//Set object active
	stSetObjectPropertyActive*	m_bSetActive;

	std::vector<DWORD>	m_needRepairItemList;

	std::lookaside_allocator<CRoleItem> m_allocRoleItem;
	//DWORD		m_dwUseSkillTime;
	//DWORD		m_dwUseSkillInterval;

	std::string		m_strMainRoleShopAdv;

	bool HandleCommand(stNullUserCmd* pCmd);
public:	
	void RenderGridLine( ); //Add By David For 2006-3-9
public:	
	bool HandleLButtonDownEventOnSceneObject(CSceneObject* pObject, bool bNeedShift = false);
	bool HandleRButtonDownEventOnSceneObject(CSceneObject* pObject);
	void SelectInterestNpc(CNpc* pNpc);

	void RefreshActiveItem(DWORD dwLocation);

	void RePrevPathNode();
	typedef std::vector<stObtainExp> tListObtainExp;
	tListObtainExp m_listExpCmd;

	CTask* GetTaskList() { return &m_task; }
	CMailManager* GetMailManager() { return &m_mail; }	
	void			SetMainRoleShopAdv(const char* pszAdv);

	int GetSkillPoint(){ return m_data.skillPoint;}
	int GetPoint(){ return m_data.points;}
	bool IsCanStudySkill(SkillBase_t* pSkill, const int& upLevel, bool bNeedTips = false);
	int GetSkillPrePoints(SkillBase_t* pSkill);
	DWORD GetUpLevel(SkillLevelBase_t* pSkill);
	typedef std::vector<CRoleSkill*> tListSkill;
	tListSkill m_listSkill;
	CRoleSkill * SkillItToPointer(tListSkill::iterator it)
	{
		if(it == m_listSkill.end()) return NULL;
		return (*it);
	}

	//
	void SetCurSkillTreePoints( const int& points ) { m_curSkillTreePoints = points; }
	//
	int GetCurSkillTreePoints() { return m_curSkillTreePoints; }

	//Wether has anything to repair
	bool HasItemNeedRepair();
	void AddOneNeedRepairItem(DWORD thisID);
	void DeleteOneNeedRepairItem(DWORD thisID);		//just remove from list
	void DrawNeedRepairItemList(stPointI& pt);

	//Use skill on somebody
	void UseSkillOnCharacter(CNpc* pNpc);

	//Clear rush out state
	void ClearRushOutState() { m_bRushOut = false; }

	//Update all skill-up item
	void UpdateAllSkillUpItemToolTips();

	//PK mode about
	void InitPKMode() { m_PkMode = m_data.pkmode; }
	void SetPKMode(BYTE pkMode);
	BYTE GetPKMode() { return /*m_data.pkmode*/m_PkMode; }	

	//Set property active about
	void ChangeSetActiveState();
	void GetSetActiveByID( const DWORD& setID, stSetObjectPropertyActive& setActive );
	void GetSetActiveByName( const char* setName, stSetObjectPropertyActive& setActive );
	//
	
	tListSkill::iterator FindSkillItByID(DWORD dwID);

	CRoleSkill* FindSkillByID(DWORD dwSkillID)
	{
		return SkillItToPointer(FindSkillItByID(dwSkillID));
	}

	void AddSkill(stAddUserSkillPropertyUserCmd* pCmd);
	void RemoveSkill(stRemoveUserSkillPropertyUserCmd* pCmd);

	typedef std::vector<CRoleItem*> tListItem;
	tListItem m_listItem;
	CRoleItem* ItemItToPointer(tListItem::iterator it)
	{
		if(it == m_listItem.end()) return NULL;
		return (*it);
	}

	tListItem::iterator FindItemItByID(tItemThisID dwID);
	tListItem::iterator FindItemItByConstID(const DWORD& dwID);

	CRoleItem* FindItemByID(tItemThisID dwID)
	{
		return ItemItToPointer(FindItemItByID(dwID));
	}
	CRoleItem * FindItemItByLocationType(const DWORD & dwTypeLocation,const enumItemType & type);

	CRoleItem* FindItemByConstID(const DWORD& dwID)
	{
		return ItemItToPointer( FindItemItByConstID(dwID) );
	}

	//Find item by const id and location
	CRoleItem* FindItemByLocationConstID(const DWORD& dwID, const DWORD& location)
	{
		return ItemItToPointer( FindItemItByLocationConstID( dwID, location ) );
	}
	tListItem::iterator FindItemItByLocationConstID(const DWORD& dwID, const DWORD& location);

	//Find item by const id and location and fivetype
	CRoleItem* FindItemByLocationFiveTypeConstID(const DWORD& dwID, const DWORD& location, const DWORD& fivetype)
	{
		return ItemItToPointer( FindItemItByLocationFiveTypeConstID( dwID, location, fivetype ) );
	}
	tListItem::iterator FindItemItByLocationFiveTypeConstID(const DWORD& dwID, const DWORD& location, const DWORD& fivetype);
	

	DWORD FindOtherItemWithSameTypeID( const DWORD& thisID, const DWORD& typeID );

	tListItem::iterator FindItemItByLocation(const stObjectLocation & location);

	CRoleItem* FindItemByLocation(const stObjectLocation & location)
	{
		return ItemItToPointer(FindItemItByLocation(location));
	}

	bool IsMoving() { return m_pCharacter->IsMoving(); }

	DWORD GetGoldNum()  { return m_data.gold; }	//返回金子数
	DWORD GetticketNum()  { return m_data.ticket; }	//返回积分数
	DWORD GetPointNum() { return m_dwPoint;}	//返回点数
	DWORD GetMonthCard() { return m_dwMonthCard; }	//返回月卡

	void  SetGoldNum(DWORD dwNum)	{ m_data.gold = dwNum; }
	void  SetPointNum(DWORD dwPt) { m_dwPoint = dwPt; } 
	void  SetMonthCard(DWORD dwMc) { m_dwPoint = dwMc; }

	DWORD GetMoney() { return GetItemCount(665);}

	DWORD GetTicket() { return m_data.ticket;}

	CRoleItem* GetMoneyItem(DWORD dwLocation);
	CRoleItem* GetCanUseArrow(const int& num);

	bool IsHasAskedCombinPet(DWORD id);

	//CRoleItem* GetHorseItem();
	void ResetHorseOwnStatus();
	bool IsHasHorse();
	void SellHorse();
	void RemoveHorse();

	CRoleItem* GetSavingBoxItem();
	void ResetSavingBoxOwnNum();
	t_SavingBox* GetSavingBox() { return &m_savingBox; }
	int  GetSavingBoxNum();
	int  GetRealSavingBoxNum() const { return m_savingBox.m_num; }

	void AddItemTipFlagByLocationType(DWORD dwLocation,DWORD flag);
	void RemoveItemTipFlagByLocationType(DWORD dwLocation,DWORD flag);
	void RemoveItemByLocationType(DWORD dwLocation);
	void SwapItemByLocationType(DWORD dwSrcLocation,DWORD dwDstLocation);
	void RemoveItem(tItemThisID qwThisID);
	void RemoveItem(DWORD dwLocation,POINT ptGrid);
	void AddItem(stAddObjectPropertyUserCmd* pCmd);
	void AddItem( t_Object& obj,BYTE byActionType);
	void RemoveItem(stRemoveObjectPropertyUserCmd* pCmd);
	void SwapItem(stSwapObjectPropertyUserCmd* pCmd);
	void SplitItem(stSplitObjectPropertyUserCmd* pCmd);
	void RefItemCount(stRefCountObjectPropertyUserCmd* pCmd);
	size_t GetItemCount(DWORD dwID,DWORD dwLocation = 0,int nLevel = -1);
	size_t GetItemExteriorCountByType(const DWORD& dwType);
	size_t GetItemExteriorCountByConstID(const DWORD& dwID, DWORD dwLocation=-1);
	bool IsAnyArrowHasMoreNum(const int& num, DWORD dwLocation = OBJECTCELLTYPE_COMMON);

	int GetMaxFindDis(){ return m_AStar.GetMaxFindDis();}
	CMainCharacter(void);
	~CMainCharacter(void);
	bool SetAttrib(t_MainUserData* pCmd);
	CAStar & GetAStar(){ return m_AStar; }
	void UnPauseMove();
	void Release();

	void SetPreferAccessGroup(BYTE group);
	BYTE GetPreferAccessGroup() { return m_preferAccessGroup; }

	void Run(long time_elasped);
	void Draw(POINT ptMapPixelOffset,DWORD dwColor);
	void AdjustAxis();
	void GetScene9ScreenID(int aScreenID[9]);
	//bool HandleCommand();

	struct stInput{
		stPointI ptMouse;
		DWORD	 dwLButtonDownTime;
		DWORD	 dwRButtonDownTime;
		bool	 bLButtonDown;
		bool	 bRButtonDown;
		bool	 bGM;
		bool	 bReFindPath;
		DWORD	 dwLButtonDownTimeFirst;
	};
	
	stInput m_InputState;

	int m_moveSpeed;
	/*int GetSpeedByInput()
	{
		if(m_InputState.bRButtonDown)
			return 2;
		return 1;
	}*/
	int GetMoveSpeed(){ return m_moveSpeed;}
	void SetMoveSpeed(int speed);
	
	enum enumTargetType{
		eTargetType_None,
		eTargetType_Npc,
		eTargetType_Character,
		eTargetType_Item,
	};
	
	enum enumEventType{
		eEventType_None,
		eEventType_Move,
		eEventType_Attack,
		eEventType_PickUp,
		eEventType_Follow,
		eEventType_AutoAttack,
	};

	struct stRunEvent{
		enumTargetType target;
		enumEventType  event;
		DWORD			id;
		SkillBase_t		*pSkill;
		stPointI		pt;
		int				speed;
	};

	//Emperor Dare info
	struct stEmperorDareZone{
		bool	bNeedDraw;
		DWORD	dwDefID;
	};
	stEmperorDareZone	m_infoEmperorDareZone;

	enum enumAttackNpcResult{
		eAttackNpcResult_Succeeded,
		eAttackNpcResult_TooFar,
		eAttackNpcResult_TooNear,
		eAttackNpcResult_LessMP,
		eAttackNpcResult_IntervalShort,
		eAttackNpcResult_Died,
		eAttackNpcResult_AniError,
		eAttackNpcResult_RideLevelLess,
		eAttackNpcResult_Other,
	};
	
	enum enumPickUpResult{
		ePickUpResult_Succeeded,
		ePickUpResult_TooFar,
		ePickUpResult_TimeError,
		ePickUpResult_Other,
	};

	enum enmAutoAttackObjectType{
		eAutoAttack_Summon	= 1,
		eAutoAttack_Character	= 2,
	};

	enum enumCanUseSkill{
		CanUseSkill_Ok,
		CanUseSkill_TimerError,
		CanUseSkill_LessMagic,
		CanUseSkill_Other,
	};
	SkillBase_t* m_pActiveSkill;
	CNpc*		 m_pActiveNpc;
	CNpc*		 m_pAttackNpc;

	typedef std::vector<stPointI>	tTargetList;

	bool OnSelectSkill(DWORD dwID);
	
	enumAttackNpcResult CallNpc(CNpc* pNpc);
	enumAttackNpcResult AttackNpc(CNpc* pNpc,SkillBase_t * pSkill, tTargetList& targetList);
	enumAttackNpcResult CastMagic(SkillBase_t* pSkill,POINT pt,CNpc* pTarget, tTargetList& targetList);
	enumCanUseSkill CanUseSkill(SkillBase_t* pSkill,CNpc* pTarget,bool bShowHit);
	bool CanUseSpecialSkill(SkillBase_t* pSkill);
	enumPickUpResult PickUpItem(CItem* pItem);
	enumAttackNpcResult CallNearByNpc(enumNpcTypeItem* npcs);
	stRunEvent m_RunEvent;
	//wen
	stPointI ptDest;
	//wen

	void SetRunEvent( const stRunEvent& event );
	void AttackBackSummon(CNpc* pNpc);
	enumEventType GetRunEventType() { return m_RunEvent.event; }

	void PickUpNearbyItem(bool bOnlyUnderFoot = true);	//This func used to pick one item nearby character. Each time pick one until there has none
	void TryPickUpItem(CItem* pItem);

	void ClearRunEvent();
	//stPointI	m_ptCurPathDest;
	bool MsgProc(UINT nMsg,WPARAM wParam,LPARAM lParam);

	bool WalkTo(POINT pt,int speed);
	bool WalkPath();
	bool SendWalk(int dir,int speed,POINT ptDst);
	bool FindPath(POINT pt,int speed);
	bool CanSendWalkMsg();
	void ClearPath();
	void RefreshEvent();
	bool OnInputEvent(UINT nMsg, CSceneObject* pObject = NULL);
	void UpdateInputState();
	void RunPickUpEvent();
	void RunMoveEvent();
	void RunAttackEvent();
	void RunFollowEvent();
	void RunAutoAttack(enmAutoAttackObjectType objType = eAutoAttack_Summon);
	void RunAutoAttackResourceBox();
	bool FollowTeamLeaderAtAutoAttackMode();
	void AttackTeamLeaderAttack();
	void UseAutoFeedIn();
	void SetFollowState();
	bool IsFollowState() { return m_RunEvent.event == eEventType_Follow; }
	bool IsAttackState() { return m_RunEvent.event == eEventType_Attack; }
	bool IsSurroundWithMoveObstacle();

	void SetEmperorDareZone(const bool& bNeedDraw, const DWORD& dwDefID)
	{
		m_infoEmperorDareZone.bNeedDraw = bNeedDraw;
		m_infoEmperorDareZone.dwDefID = dwDefID;
	}

	stEmperorDareZone& GetEmperorDareZoneInfo() { return m_infoEmperorDareZone; }

	int GetArrowNum(bool bEquipOn = true);

	bool AttackAttackingNpc();

    size_t GetZhan1(){ return m_data.maxpdamage; }
	size_t GetZhan2(){ return m_data.maxmdamage; }
	size_t GetZhan3(){ return m_data.pdefence; }
	size_t GetZhan4(){ return m_data.mdefence; }
	size_t GetCharvip(){ return m_data.charvip; } //vip点数
	size_t GetChangejob(){ return m_data.changejob; }
	size_t GetHonor(){ return m_data.honor; }
	size_t GetSalary(){ return m_data.salary; } //工资数
	size_t GetGold(){ return m_data.gold; }  //金子数
	size_t Getticket(){ return m_data.ticket; } //积分数
	size_t Getchongzhi(){ return m_data.chongzhi; } //充值点数	
	size_t GetLevel(){ return m_data.level; }
	size_t GetRound(){ return m_data.round; }
	size_t GetSP() { return m_data.sp;}			// 当前体力值
	size_t GetMaxSP(){ return m_data.maxsp;}	// 最大体力值
	size_t GetMP() { return m_data.mp;}
	size_t GetMaxMP(){ return m_data.maxmp;}
	uint64_t GetExp() { return m_data.exp;} //sky
	uint64_t GetNextexp() { return m_data.nextexp;} //sky
	WORD GetTire() { return m_data.wdTire; }
	t_MainUserData* GetProperty(){ return &m_data;}
	void AddExp(QWORD exp,QWORD dwAbsExp);


	void OnNpcDeath(CNpc* pNpc);

	void SetHPAndMP(unsigned int nHP,unsigned int nMP,unsigned int nSP);
	bool CanAttackCharacter(CCharacter* pChar);
	bool CanAttackNpc(CNpc* pNpc);

	void UpdateAllItems(DWORD flag = -1);

	bool IsRightSkill(SkillBase_t* pSkill);
	int GetFivePoint(enumFiveType five);

	// 获得界面显示的命中值
	int GetShowHitValue();
	// 获得界面显示的躲避值
	int GetShowDodgeValue();

	// 获得主修三项五行的数值
	int GetMainFiveItem(int index);
	
	//获得最后访问的NPC的ID
	DWORD GetNpcIdByLast(){ return m_dwNpcIdByLast;}

	// 使用洗点道具
	void ClearPointOrSkill(DWORD dwID);
	void AddLight(POINT ptMapOffset);
	void OnGridPosChange(POINT prevGrid,POINT ptCurGrid);
	bool OnSetCursor();
	void OnDayNumChange();

	bool IsMainUserSkill(SkillBase_t* pSkill);

	bool IsTeamLeader();

	tOnGotoFinalEventFunc m_pGotoFinalEvent;
	void				* m_pGotoFinalParam;
	bool Goto(POINT ptGrid,tOnGotoFinalEventFunc pEventFunc = NULL,void* pParam = NULL);

	bool bCanChangeClothColor();//判断是否可以更换服装颜色
	DWORD GetClothID() {return GetProData()->sculpt.dwBodyID;}

	DWORD GetAllItemRepairMoney(DWORD flag);

	void RemoveAllSkill();
	bool FindPathNear(POINT ptGrid,int speed,int nMaxFindPathDis);
	bool FindPathFar(POINT ptGrid,int speed,int nMaxFindPathDis);
	bool GetNextZonePathPos(POINT & ptNextGrid,POINT ptTargetPos);
	//-----------------------------
	// 
	void AutoAddHP(int minHP);
	void AutoAddMP(int minMP);
	bool RunAutoPickup();
	bool RunAutoWork();
	bool RunAutoWorkSkill(); /////////star100515

	void SelectSkill(SkillBase_t* pSelectSkill);
	//-----------------------------
	bool AutoEquipFillItem(const int& xPos, const int& type);

	void AutoAddHpBySpecialEquip();
    int OnUseItem_forAutoAttack(WORD dwID);/////star100515

#if defined USE_DEFENCE
	//以下代码为重写OnUseItem函数部分
	typedef bool(CMainCharacter::* tFunctionDWORD1_Bool)(DWORD);	
	M_FUNCTION_DECLARE1_R_8(OnUseItem,DWORD,bool)			//定义8个OnUseItem成员函数( OnUseItem0(DWORD)......OnUseItem7(DWORD) )
		M_FUNCTION_CALL_DEFINE1_R(OnUseItem,DWORD,bool)			//定义调用OnUseItem函数

private:
	M_FUNCTION_ARRAY_DECLARE(tFunctionDWORD1_Bool,OnUseItem);

	//成员函数数组初始化
	M_FUNCTION_ARRAY_INIT_BEGIN
		M_FUNCTION_ARRAY_DEFINE(tFunctionDWORD1_Bool,OnUseItem);
	M_FUNCTION_ARRAY_INIT_END
		//声明结束

#define CALL_USEITEM(id)		M_FUNCTION_CALL1(OnUseItem,(id))
#define CALL_USEITEM_P(p,id)	M_FUNCTION_CALL1_P(p,OnUseItem,(id))
#else
	bool OnUseItem(DWORD dwID);
#define CALL_USEITEM(id)		OnUseItem(id)
#define CALL_USEITEM_P(p,id)	p->OnUseItem(id)
#endif
};

extern char* GetPkModeName(DWORD dwGoodNess);