#pragma once
#include "npc.h"
#include "../Magic/cAnimationArray.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */

struct stFaceInfo{
	int imgID;
};


struct stBitmapShow	// 显示控制
{
	stResourceLocation rl;
	IBitmaps * pBitmps;
	DWORD  dwStartTime;
	void Render(const POINT & pt);
	void Run();
	BYTE  alpha;
	bool IsNeedShow();
	void SetFrame(int nFrame);
	void StartShow();
	stBitmapShow();
	stRectI rcClip;
};

class CCharacter :
	public CNpc
{
public:
#ifdef _WYC_OPT_SER_
	wyc::t_MapUserData m_data;
	CAnimation vipImage;
	CAnimation touxian;
	CAnimation duanwei;
#else
	::t_MapUserData m_data;
	CAnimation vipImage;
#endif
	
	DWORD   m_dwStar;
	BOOL    m_bShowWarFlag;	
	int m_tiletype;

public:
	stPointI m_ptPrevGrid;
	virtual void OnGridPosChange(POINT prevGrid,POINT ptCurGrid);

	CCharacter(void);
	~CCharacter(void);
	bool IsCanAttack();
	bool IsMyFriend();
	void AfterSetAttrib(bool bUpdateImage,bool bUpdateState);
	bool SetAttrib(::t_MapUserData* pCmd);
	bool ResetImage(); 
	bool ResetState();
	enumSex GetSex() { return m_data.type == PROFESSION_1 ? eSex_Male : eSex_Female; }
	int  GetFace(){ return m_data.face;}
	void Release();
	void Run(long time_elasped);
	void Draw(POINT ptMapPixelOffset,DWORD dwColor);

	bool CanAttack();
	//bool HandleCommand();
	size_t GetType(){ return m_data.type;}
	size_t GetHairType() { return m_data.sculpt.dwHairID;}
	
	const char* GetTypeName();
	char  m_countryName[MAX_NAMESIZE];
	const char* GetCountryName();
	const char* GetUnionName() { return m_data.unionName;}
	const char* GetSeptName() { return m_data.septName;}
	const char* GetGraceName(); 
	const char* GetOfficeName();
	
	void SetBodyCustomColor(DWORD dwColor);
	void SetBodySystemColor(DWORD dwColor);
	DWORD GetBodySystemColor();
	DWORD GetBodyCustomColor() { return m_data.dwBodyColorCustom | 0xff000000;}
	DWORD GetHairColor() { return m_data.dwHairRGB | 0xff000000;}
	DWORD GetLeftWeaponColor();
	DWORD GetRightWeaponColor();
	DWORD GetGoodness() { return m_data.goodness; }
	void  SetGoodness(DWORD dwGoodness);
	DWORD GetGoodnessAfterAdjust();
	enumFiveType GetMainFive(){ return enumFiveType(0);}//if( m_data.five >= FiveType_Num ) return enumFiveType(0); return (enumFiveType)m_data.five;}
	DWORD GetCountry() { return m_data.country; }

	int ServerSkillToClientSkill(SkillBase_t* pSkill);
	t_MapUserSculpt* GetSculpt(){ return &m_data.sculpt; }
	int GetRideID();

	virtual BYTE* GetState(){
		return m_data.state;
	}

	int GetAniSpeed() { return m_data.movespeed; }
	int GetAttackSpeedServer() { return m_data.attackspeed; }

	int GetHitSound();
	int GetAttackSound();

	DWORD GetLeftWeaponID(){ return m_data.sculpt.dwLeftHandID;}
	DWORD GetRightWeaponID(){ return m_data.sculpt.dwRightHandID;}
	enumItemType GetRightWeaponType();
	enumItemType GetLeftWeaponType();
	void OnEquipChanged();
	bool IsEquipedPickResourceTool();

	void DrawName(POINT ptMapPixelOffset,DWORD dwColor = 0xffffffff);
	bool HandleCommand(stNullUserCmd* pCmd);
	void AddLight(POINT ptMapOffset);
	bool OnSetCursor();

#ifdef _WYC_OPT_SER_
	wyc::t_MapUserData* GetProData() { return &m_data; }
#else
	::t_MapUserData* GetProData() { return &m_data; }
#endif
	
	ObjectBase_t*	GetBodyObjectBase() { return GetObjectBase(m_data.sculpt.dwBodyID);}
	ObjectBase_t*	GetBodyObjectBaseKind() { return GetObjectBase(m_data.sculpt.dwBodyKind);}
	size_t GetLevel();

	stBitmapShow m_BitmapShow;
	void DrawEmotionIcon(stPointI ptMapPixelOffset);
	void DrawCountryWarFlag(stPointI ptMapOffset);

	virtual bool bIsShopStart();
	bool IsUnKnowPerson();


	bool IsKing(); // 是否国王
	bool IsEmperor();	// 是否皇帝
	bool IsYuanshuai(); //soke 是否元帅
	bool IsZaixiang();  //soke 是否宰相
	bool IsCatcher();   //soke 是否捕头（左）
	bool IsDiplomat();  //soke 是否御史（左）
	bool IsCatchex();   //soke 是否捕头（右）
	bool IsDiplomax();  //soke 是否御史（右）
	bool IsGen();  // 是否将军
	bool IsCityLeader(); // 是否城主
	bool IsArmyer(); //是否军人
	bool IsSeptLeader();	//是否族长
	bool IsUnionLeader();	//是否帮主
	bool IsDragonTigerState();

	DWORD   GetTeamState(); //得到队伍状态信息
	void SetCountryStar(DWORD dwNewStar){
		m_dwStar = dwNewStar;
	}
	DWORD GetCountryStar(){
		return m_dwStar;
	}
	void SetShowWarFlag(BOOL bShow){
		m_bShowWarFlag = bShow;
	}
	//DWORD   GetTeamState(); //得到队伍状态信息

	void SetInSpaState(bool bSwim);
	void SetInSpaState5(bool bSwim);
	void SetInFlyState(bool bFly);
	
	void SetInDanceState(bool bDance);
	void Set2InDanceState(bool bDance);
	void Set3InDanceState(bool bDance);

	//自行车
	void SetInBikeState(bool bBike);
	void SetInBikeState2(bool bBike);

	//战车
	void SetInZCState(bool bZc);

private:
	cAnimationArray m_AniArraySwim; // 控制播放游泳的动作
	cAnimationArray m_AniArraySwimStand; // 控制播放游泳站立
	cAnimationArray m_AniArrayFly; // 控制飞行物体动作
	cAnimationArray m_AniArraySwim5;      // 控制播放挖宝的动作
	cAnimationArray m_AniArraySwimStand5; // 控制播放挖宝站立
	
	cAnimationArray m_AniArrayDance;     // 控制播放跳舞动作
	cAnimationArray m2_AniArrayDance;    // 控制播放跳舞发型
	cAnimationArray m3_AniArrayDance;    // 控制跳舞服装填充

	cAnimationArray m_AniArrayBike; // 控制播放自行车
	cAnimationArray m_AniArrayBikeStand; // 控制播放自行车

	cAnimationArray m_AniArrayBike2; // 控制播放自行车补充
	cAnimationArray m_AniArrayBikeStand2; // 控制播放自行车补充

	
	cAnimationArray m_AniArrayZC; // 战车控制器 by 醉梦
	cAnimationArray m_AniArrayZCStand; // 战车控制器 by 醉梦
};


extern  DWORD GetNameColor(DWORD dwGoodNess);