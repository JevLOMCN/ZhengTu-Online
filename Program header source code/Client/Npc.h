#pragma once

#include "..\magic\sceneobject.h"
#include "..\magic\icharacter.h"

class CEffect;
#define COMBINATION_STATE_NUM	12
static const int cmbState[COMBINATION_STATE_NUM] = { USTATE_COMBIN_STONE_PET,
													 USTATE_COMBIN_BONE_PET,
													 USTATE_COMBIN_GOD_FIGHTER,
													 USTATE_COMBIN_WIND_PET,
													 USTATE_COMBIN_GOD_LEADER,
													 USTATE_EAT_GHOST,
													 USTATE_COMBIN_IRON_PET,
													 USTATE_COMBIN_BONE_GUARD,
													 USTATE_COMBIN_XIAN_PET,
													 USTATE_COMBIN_FIRE_LEADER,
													 USTATE_COMBIN_SIX_GHOST,
													 USTATE_COMBIN_THREE_DIM };
static const int cmbSkill[COMBINATION_STATE_NUM] = { 259, 219, 239, 264, 244, 224, 269, 229, 249, 275, 235, 255 };
static const int cmbNpcID[COMBINATION_STATE_NUM] = { 42301, 42141, 42201, 42381, 42281, 44001, 45021, 44081, 44041, 45061, 46001, 46041 };
static const int cmbNpcPetID[COMBINATION_STATE_NUM] = { 42221, 42061, 42181, 42241, 42161, 42081, 45001, 44061, 42121, 45041, 45081, 46021 };
static const DWORD ShooterJingSkill[] = { 120, 128, 123, 125, 130, 133, 137, 138 };

enum enumCartID{
	Npc_CartID1 = 50005,
	Npc_CartID2 = 50006,
};

enum enumNpcType{
	NPC_TYPE_NONE	=	0,
	NPC_TYPE_ATTACK,
	NPC_TYPE_FUNCTION,
	NPC_TYPE_TRANSPORT,
	NPC_TYPE_GATHER,
};

enum enumNpcTypeItem
{
	NpcTypeItem_None = -1,
	NpcTypeItem_Human = 0,		//人形类
	NpcTypeItem_Normal = 1,		//普通类型
	NpcTypeItem_Boss = 2,		//大BOSS类型
	NpcTypeItem_SmBoss = 3,		//小BOSS类型
	NpcTypeItem_Extract	 = 4,	//精英BOSS
	NpcTypeItem_Gold	 = 5,	//黄金BOSS
	NpcTypeItem_Trade = 6,		//买卖类的
	NpcTypeItem_Task = 7,		//任务类的
	NpcTypeItem_Enlisted = 8,	//守卫的士兵类
	NpcTypeItem_Pet=9,			//宠物
	NpcTypeItem_ExtractBugbear=10,	//精怪类
	NpcTypeItem_Beckon = 11,		//召唤怪物
	NpcTypeItem_Totem=12,			//图腾NPC
	NpcTypeItem_Aggrandizement = 13,/// 强化类型
	NpcTypeItem_Aberrance	= 14,	 /// 变异类型
	NpcTypeItem_SavingBox=15,		//仓库功能类
	NpcTypeItem_Guide = 16,			//路标类型
	NpcTypeItem_Box = 17,			//宝箱
	NpcTypeItem_Wildhorse      = 18,   /// 野马类型
	NpcTypeItem_MobileTrade    = 19,   /// 流浪小贩
	NpcTypeItem_LiveNpc        = 20,    /// 生活npc（不战斗，攻城时消失）
	NpcTypeItem_Flower			= 21,	//特殊怪物 花花 草草的
	NpcTypeItem_Grass		   = 21,	//Grass
	NpcTypeItem_FlagPole		= 22,	//旗帜类
	NpcTypeItem_CatchItem		= 23,	//陷阱
	NpcTypeItem_MailMan		= 24,	//邮箱相关
	NpcTypeItem_Auction		= 25,	//拍卖相关
	NPC_TYPE_SURFACE		= 29,	//地表类NPC，无阻挡
	NpcTypeItem_Cartoon     = 30,	//孩子
	NpcTypeItem_ExGold		= 31,	//紫色类
	NpcTypeItem_Resource	= 32,	//资源类型
	NpcTypeItem_RedBoss     = 33,   //红色BOSS类型

};

//0表示动物类  1表示人形类 2表示灵兽类 3表示魔兽类 4表示神兽类 5表示机械类    10表示其他
enum enumNpcSummonType
{
	NpcSummonType_Animal	= 0,
	NpcSummonType_Human		= 1,
	NpcSummonType_Ling		= 2,
	NpcSummonType_Mo		= 3,
	NpcSummonType_Seng		= 4,
	NpcSummonType_Machine	= 5,
	NpcSummonType_Other		= 6,
};

enum ClientSkill
{
	CLIENT_SKILL_BOW = 4,
	CLIENT_SKILL_ONEATTACK = 1,
};

enum ServerSkill
{
	SERVER_SKILL_ATTACK_NORMAL = 377,
	SERVER_SKILL_BOW_NORMAL = 378,
	SERVER_SKILL_CROSSBOW_NORMAL = 10000,
	SERVER_SKILL_CHANGE_FACE = 220,
};

#define     FIVETYPENUM            5
enum enumFiveType{
	FiveType_Gold = FIVE_METAL,		// 金
	FiveType_Timber = FIVE_WOOD,	// 木
	FiveType_Earth = FIVE_SOIL,		// 土
	FiveType_Water = FIVE_WATER,	// 水
	FiveType_Fire = FIVE_FIRE,		// 火
	/*
	FiveType_Gather,	// 采集
	FiveType_Make,		// 制造
	FiveType_Find,		// 发现
	*/
	FiveType_Business,     //商业
	FiveType_Work,        //劳动
	FiveType_Num,
};

struct stObtainExp{
	DWORD dwSrcID;
	enumMapDataType eSrcType;
	QWORD dwExp;	// 得到的经验值
	QWORD dwAbsExp; // 绝对值
	DWORD dwTime;	// 什么时间收到的
};

#define RUSH_OUT_SPEED_RATE	4

//Is combination skill
inline bool IsCombinSkill(SkillBase_t* pSkill)
{
	for( int i=0;i<COMBINATION_STATE_NUM;i++ )
	{
		if ( pSkill->dwID == cmbSkill[i] )
		{
			return true;
			break;
		}
	}

	return false;
}

//Is shooter jing skill
inline bool IsShooterJingSkill(SkillBase_t* pSkill)
{
	for( int i=0;i<count_of(ShooterJingSkill);i++ )
	{
		if ( pSkill->dwID == ShooterJingSkill[i] )
		{
			return true;
			break;
		}
	}

	return false;
}

//Interest NPC base info
struct stInterestNpcBaseInfo
{
	bool bCharacter;
	bool bNpc;
	DWORD tmpID;
	int	nAttackFailNum;

	void Init()
	{
		bCharacter = false;
		bNpc = false;
		tmpID = -1;
		nAttackFailNum = 0;
	}
};

struct stAttackMagicUserCmd;
struct stRTMagicUserCmd;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CNpc :
	public CSceneObject
{
protected:
	std::map<DWORD,DWORD> m_aSkillUseTime;

	//DWORD		m_dwUseSkillTime;
	//DWORD		m_dwUseSkillIntervalTime;
	
	iCharacter* m_pCharacter;
	iCharacter* m_pChangeFace;
	NpcBase_t*	m_pNpcBase;
	bool		m_bDieWaitingMagic;
	bool		m_bFillObstacle;
	bool		m_bDied;
	bool		m_bDieEnd;
	bool		m_bAshesFadeOut;// 尸体淡出中
	long		m_nAshesFadeOutTime;// 尸体淡出时间
	long		m_nDieWaitingTime;
	t_MapNpcData m_data;
	t_MapPetData* m_pPetData;		//宠物数据
	int			m_nRecieveAttack;
	int			m_nSendAttack;
	int			m_nSendWalk;
	int			m_nRecieveWalk;
	int			m_nDirect;
	int			m_iDrawTop;
	DWORD		m_timeCommand;
	stPointI	m_ptBlock;

	struct stChangingFace
	{
		DWORD		m_dwChangingFaceID;
		DWORD		m_dwChangingFaceLevel;
		int			m_nChangingFaceFiveType;


		stChangingFace()
		{
			Init();
		}

		void Init()
		{
			m_dwChangingFaceID = -1;
			m_dwChangingFaceLevel = 10;
			m_nChangingFaceFiveType = FIVE_NONE;
		}
	};
	stChangingFace	m_proChangingFace;
	DWORD		m_dwCombinationID;
	bool		m_bIsSitting;
	bool		m_bClientSitting;
	bool		m_bClientRideState;

	bool		m_bIsCombination;

	bool		m_bLiquidMoveState;

	DWORD		m_timeSendAttack;

	struct stNumColor
	{
		char szNum[16];
		DWORD color;
		float height;
		stPointF scale;
		BYTE byLuck;
		BYTE byIsExp;
		stNumColor()
		{
			color = -1;
			height = 0.0f;
			scale = stPointF(0.5f,0.5f);
			byLuck = 0;
			byIsExp = 0;
		}
	};

	typedef std::deque<stNullUserCmd*> tListHPCmd;
	tListHPCmd m_queueHPCmd;
	/*typedef std::deque<stRTMagicUserCmd> tListHurt;
	tListHurt		m_listHurtCmd;*/

	typedef std::vector<stNumColor> tListNumColor;
	tListNumColor	m_listNumColor;

	//std::queue<stRTMagicUserCmd>	m_recHurtCmd;

	CStreamQueue m_queueCmd;
public:
	void PushBackNumColor(const stNumColor& nc);
	DWORD GetTeamAttackState();

	//Clear cmd queue when jmp
	void ClearCmdQueue();

	void AddHpMpPopEffect(enumPopEffect popType, int value);

	int GetHorseLevel();
	bool CanUseSkillByTime(SkillBase_t* pSkill, DWORD startTime=0);
	void SetUseSkillTime(SkillBase_t* pSkill, DWORD startTime=xtimeGetTime());
	void ClearUseSkillTime(SkillBase_t* pSkill);
	void DrawSkillDisableStatus(SkillBase_t* pSkill,const RECT * prect);
	void DrawSkillMagicLower(SkillBase_t* pSkill, const RECT * prect);

	virtual void DrawEmotionIcon(stPointI ptMapOffset);
	virtual void DrawCountryWarFlag(stPointI ptMapOffset);

	virtual enumFiveType GetMainFive();
	virtual size_t GetLevel();
	//Get skill start use time
	DWORD GetSkillStartTime(SkillBase_t* pSkill);
	// 获得技能的冷却时间（0 - 1，1表示可用了）
	float GetSkillDisableStatus(SkillBase_t* pSkill);

	DWORD GetNpcPic();
	DWORD GetChangeFaceID() { return m_proChangingFace.m_dwChangingFaceID; }
	DWORD GetChangeFaceLevel() { return m_proChangingFace.m_dwChangingFaceLevel; }
	int GetChangeFaceFiveType() { return m_proChangingFace.m_nChangingFaceFiveType; }

		
	virtual BYTE* GetState(){
		return m_data.byState;
	}
	bool IsDieWaitingMagic(){ return m_bDieWaitingMagic;}
	CNpc(void);
	~CNpc(void);
	
	//enumNpcType GetNpcType() { return m_eNpcType; }
	bool IsNpcType(enumNpcType type);

	virtual bool IsSame(const stInterestNpcBaseInfo& baseInfo);		//If the two refered npc the same

	enumNpcTypeItem GetNpcTypeItem() 
	{ 
		if(m_pNpcBase) return enumNpcTypeItem(m_pNpcBase->dwType); 
		return NpcTypeItem_None; 
	}

	bool IsCanChangeFaceTo();

	virtual bool IsCanAttack()
	{
		return IsNpcType(NPC_TYPE_ATTACK);
	}
	virtual const char* GetCountryName()
	{
		return "";
	}

	bool SetAttrib(t_MapNpcData* pMapNpcData);
	void UpdatePosition(void);
	bool UpdateGridPosition(POINT ptGrid);
	bool IsMainRole() { return (m_dwSceneObjectType & OBJECT_MAINCHARACTER) != 0; }
	bool IsNpc() { return (m_dwSceneObjectType & OBJECT_CHARACTER) == 0; }
	bool IsCharacter(){ return (m_dwSceneObjectType & OBJECT_CHARACTER) != 0;}
	bool IsMoving() { return m_pCharacter->IsMoving(); }
	bool IsAttacking() { return m_pCharacter->IsAttacking(); }
	bool IsCart();
	bool IsMyCart();
	bool IsMyCartoonPet();	//孩子
	bool IsMyPet();
	bool IsPet();
	petType GetPetType();
	bool IsRideFightHorse();
	virtual bool PushCommand(stNullUserCmd* pCmd, size_t size);
	void Run(long time_elasped);
	void Draw(POINT ptMapPixelOffset,DWORD dwColor);
	void Draw2(POINT ptMapPixelOffset,DWORD dwColor);//缩小
	virtual void DrawName(POINT ptMapPixelOffset,DWORD dwColor = 0xffffffff);
	virtual void DrawShopName(POINT ptMapPixelOffset,DWORD dwColor = 0xffffffff);
	void DrawHurt(POINT ptMapPixelOffset);
	virtual void DrawText(POINT ptMapPixelOffset);
	void DrawHP(POINT ptMapPixelOffset);
	void DrawEffect(POINT ptMapPixelOffset);
	bool IsCommandEmpty(){return m_queueCmd.Empty();}
	bool IsCanAutoAttackNpc();

	virtual void OnCommandNotCome();
	bool HandleCommand();
	virtual bool HandleCommand(stNullUserCmd* pCmd);
	bool OnLoopMove(stUserMoveMoveUserCmd* pCmd);
	bool OnLoopMagic(stMagicUserCmd* pCmd);
	bool MoveTo(POINT ptDes,int nSpeed,bool bServerLogic);
	void RemoveDynamicObstacle();
	void AddDynamicObstacle(POINT pt);

	virtual DWORD GetBodySystemColor();
	virtual DWORD GetBodyCustomColor() { return -1; }
	virtual DWORD GetHairColor() { return -1;}

	virtual void	AdjustAxis		( ){};
	virtual void AfterSetAttrib(bool bUpdateImage,bool bUpdateState);

	void Release();
	virtual bool ResetImage();
	virtual bool ResetState();

	virtual void SetDie(bool bDie = true) { m_bDied = bDie; }

	virtual int GetRideID(){ return 255; }
	bool OnRide(int nSwitch);
	void Relive(stMainUserReliveReliveUserCmd*);

	void OnSit(bool sitDown);
	bool IsSitDown();
	virtual void RunSitDown();

	bool IsLiquidMoveState() { return m_bLiquidMoveState; }

	bool IsFireWall();
	bool IsSurfaceType() { return GetNpcTypeItem() == NPC_TYPE_SURFACE; }

	bool IsServerSitDown() { return isset_state(GetState(), USTATE_SITDOWN); }
	bool IsServerRideState() { return isset_state(GetState(), USTATE_RIDE); }

	bool IsClientSitDown() { return m_bClientSitting; }
	bool IsClientRideState() { return m_bClientRideState; }

	void SetClientSitDown( bool bSitDown ) { m_bClientSitting = bSitDown; }
	void SetClientRideState( bool bRideState ) { m_bClientRideState = bRideState; }

	iCharacter* GetInterface() { return m_pCharacter; }

	bool Back(stBackOffMagicUserCmd* pCmd);

	bool OnMagicHurt(CNpc* pSender);

	//缩骨
	void DrawMagicLowest(POINT ptPixelOffset)
	{
		if(isset_state(GetState(), USTATE_SUOGUDAN))
		{
			m_pCharacter->DrawMagicLowest2(ptPixelOffset);
		}
		else{
			m_pCharacter->DrawMagicLowest(ptPixelOffset);
		}
		
	}
	void DrawMagicFrontest(POINT ptPixelOffset)
	{
		if(isset_state(GetState(), USTATE_SUOGUDAN))
		{
			m_pCharacter->DrawMagicFrontest2(ptPixelOffset);
		}
		else{
			m_pCharacter->DrawMagicFrontest(ptPixelOffset);
		}
	}
	void DrawMagicAboveAll(POINT ptPixelOffset)
	{
		if(isset_state(GetState(), USTATE_SUOGUDAN))
		{
			m_pCharacter->DrawMagicAboveAll2(ptPixelOffset);
		}
		else{
			m_pCharacter->DrawMagicAboveAll(ptPixelOffset);
		}
	}

	
	bool InScreen();

	bool TurnTo(int nDir,bool bServerLogic);

	bool HaveCommand(DWORD dwCmdType,DWORD dwParameterType);
	int	 GetCommandNum( DWORD dwCmdType, DWORD dwParameterType );

	bool IsAtPixelPos(POINT pt, bool bJustInRect = false);

	enum enumCastMagicResult{
		eCastMagicResult_Succeeded,
		eCastMagicResult_Obstruct,
		eCastMagicResult_ActionError,
		eCastMagicResult_Other,
	};

	enumCastMagicResult CastMagic(CNpc* pTarget, int iClientSkill, bool bServerLogic,int nLife, POINT& ptObs,AniTypeEnum nAction = Ani_Null);
	enumCastMagicResult CastMagic(stAttackMagicUserCmd* pCmd,POINT& ptObs,bool bServerLogic);
	enumCastMagicResult CastMagic(POINT ptDes, int iClientSkill, bool bServerLogic,int nLife, POINT& ptObs,AniTypeEnum nAction = Ani_Null);
	enumCastMagicResult CastMagicA(POINT ptDes, int iClientSkill, bool bServerLogic,int nLife,AniTypeEnum nAction = Ani_Null);
	virtual int ServerSkillToClientSkill(SkillBase_t* pSkill);
	bool OnSetCursor();

	bool IsValidHurt(stRTMagicUserCmd* pCmd);

	// 受伤,播放受伤动画
	void OnHurt(stRTMagicUserCmd* pCmd);

	// 受伤,不播放受伤动画
	void OnHurtAddHint(stRTMagicUserCmd* pCmd);

	bool HandleHurtCommand(stRTMagicUserCmd & cmd);
	bool OnHit(CNpc* p);
	bool OnLoopDie();

	DWORD GetHurtColor();
	// 获得击中（近身攻击）的声音
	virtual int GetHitSound();
	//获得挥动武器的声音
	virtual int GetAttackSound();
	int	GetImageHeight();
	void Die();

	virtual bool bIsShopStart();
	virtual bool bIsShopFly();
	virtual bool bIsSuogu();//缩骨
	RECT	GetShopTextRect();

	//virtual bool IsUseBow(){return false;};//
	//virtual bool IsUseStaff(){return false;}
	//virtual bool IsUseSword(){return false;}
	//virtual bool IsUseDig(){return false;}
	//virtual bool IsUseOneHandAttack(){return true;};//
	//virtual bool IsUseTwoHandAttack(){return false;};//

	bool IsDie();
	bool IsAshes(){ return m_bDieEnd;}

	DWORD GetTypeID();

	bool IsCanAttackFunctionNpc();

	bool IsChangeFace();
	bool IsCombinationState() { return m_dwCombinationID != -1; }
	bool ChangeFace(DWORD dwID);
	bool CombinationWithPet(DWORD dwID);
	DWORD GetCombinationID() { return m_dwCombinationID; }
	bool IsCombinCanUseSkill(SkillBase_t* pSkill);
	bool SkillCanOnlyUseCombination(SkillBase_t* pSkill);

	bool IsCurStateHasSitDownAni();

	virtual void RunRide();
	bool IsRide();

	bool IsAniStand();
	bool LevelUp();
	void AddLight(POINT ptMapOffset);

	void AddPetData(const t_MapPetData* pData);

	class CShopNameTip
	{
	public:
		CShopNameTip();
		void Render(POINT pt);
		void SetShopName(const char* cText,DWORD dwColor = -1);
		inline RECT GetClipRect() const;
	private:
		IBitmaps*	m_pBmpShopName;
		DWORD		m_dwColor;
		std::string m_strText;
		stRectI		m_rcBmpClip;
		stRectI		m_rcBmpDst;
		stRectI		m_rcShopNameRect;
		stRectI		m_rc[3];
		stRectI		m_rcBmps[3];
		int			m_iBaseWidth;
		int			m_iWidth;
	};

	///轻聊显示
	class CChatTip
	{
	public:
		CChatTip();
		void Render(POINT pt);
		void AddText( const char *cText,DWORD dwColor = -1,DWORD dwShowTime = m_scTotalShowTime );
		int GetTipHeight() 
		{
			return m_height + 14;
		}
		bool empty()
		{
			return m_strTextVec.empty();
		}
	private:
		///矩形框图片
		IBitmaps*	m_pBmpRect;	
		///角图片
		IBitmaps*	m_pBmpAngle;
		///矩形框分割成的11个小矩形
		stRectI m_bmpRect[11];
		///渲染区域分割成的11个小矩形
		stRectI m_renderRect[11];
		typedef std::vector<std::string> strVec;
		///文字（多行）
		strVec m_strTextVec;
		///矩形框大小
		int m_width,m_height;
		///行高
		int m_cellHeight;
		///显示时间
		int m_startTime;			
		///文字颜色
		DWORD m_dwColor;		
		///总显示时间
		DWORD m_dwTotalShowTime;

		//默认总显示时间
		static const DWORD m_scTotalShowTime;
		///横边切割宽度
		static const int m_scWidthIncise;
		///纵边切割宽度
		static const int m_scHeightIncise;
	};

	///轻聊显示
	CChatTip		m_NineChat;	

	///摊位名字显示
	CShopNameTip	m_ShopNameTip;

	/**
	* \brief 添加轻聊消息
	* 
	* 
	* 
	* \param ptGrid : 消息字符串
	* \param dwColor : 消息文字颜色
	* \return void
	*/
	void AddNineChatMsg(const char *pszMsg,DWORD dwColor)
	{
		m_NineChat.AddText(pszMsg,dwColor);
	}
	void AddShopAdvMsg(const char *pszMsg,DWORD dwColor)
	{
		m_NineChat.AddText(pszMsg,dwColor,33000);
	}

	void DrawNineChat(POINT ptMapPixelOffset);

	DWORD GetMasterID();

	void AddEffect(const std::string& strName);
	bool HaveEffect(const std::string& strName);
	void ClearEffect(const std::string& strName);

private:
	std::map<std::string, CEffect*> m_mapEffects;

private:
	inline bool bIsShowLevel();
};
