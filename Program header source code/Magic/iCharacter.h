#pragma once

#include "iObj.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class iLight;


#pragma pack(4)

enum enumCreateMagicResult{
	CreateMagicResult_Succeeded,
	CreateMagicResult_AniError,
	CreateMagicResult_NoFindMagic,
	CreateMagicResult_OtherError,
};

//#define _HAS_WEAPON_SHADOW
#define _HAS_WEAPON_COLOR

enum AniPartEnum
{
	Ani_Part_Null = -1,

#ifdef _HAS_WEAPON_SHADOW
	Ani_Part_LeftWeaponShadow,
	Ani_Part_RightWeaponShadow,
#endif

	Ani_Part_Body,
	Ani_Part_LeftWeapon,
	Ani_Part_RightWeapon,
	Ani_Part_Hair,
	Ani_Part_BodySystemColor,//系统染色部分
	Ani_Part_BodyCustomColor,// 染色部分

#ifdef _HAS_WEAPON_COLOR
	Ani_Part_LeftWeaponColor,	// 武器通道
	Ani_Part_RightWeaponColor,
#endif

	Ani_Part_Num,
};

//优先级表
enum AniPriEnum
{
	Ani_Pri_Null = -1,
	Ani_Pri_Idle,
	Ani_Pri_Stand,
	Ani_Pri_Work,
	Ani_Pri_MoveOrAttack,
	Ani_Pri_Defend,
	Ani_Pri_Hurt,
	Ani_Pri_Back,
	Ani_Pri_Die,
};

//other info of animation
const int MAX_SOUNDNAME = 16;


struct stAniInfoData
{
	AniPriEnum ePri;
	AniTypeEnum eNextAni;	//直接指定下一个动作，例如attack,magic,会转到attack,magic的第一帧,
	AniTypeEnum eRideAni;
	AniTypeEnum eUnrideAni;
	POINT ptOffset;
	int nMoveSpeed;
	int nDelay;				// 间隔时间　//如果是相同的下一个动作，必须要delay，例如两个magic

	char strSound[MAX_SOUNDNAME];
	int nSoundFrame;		//-1为loop
};


struct stAniInfo : public stAniInfoData
{
	void SetParam(AniPriEnum pri,int x, int y,std::string sound,int loop,int speedMove)
	{
		ePri = pri; 
		ptOffset.x = x; ptOffset.y = y; 
		strcpy(strSound,sound.c_str()); 
		nSoundFrame = loop;
		nMoveSpeed = speedMove;
		nDelay = 0;
		eNextAni = Ani_Null;
		eRideAni = Ani_Null;
		eUnrideAni = Ani_Null;
	}
};

enum StatusSpeedEnum
{
	Status_Slow,
	Status_Normal,
	Status_Fast,
	Status_Fastest,
	Status_Speed_Num,
};

enum StatusAppendEnum
{
	Status_Append_Null,
	Status_RedPoison,
	Status_GreenPoison,
	Status_Frost,
	Status_Stone,
	Status_Trans,		//透明
//	Status_Temp_Hurt,	//受伤暂时变红一下
	Status_Append_Num,
};

enum AttackType
{
	Attack_Null,
	Attack_Onehand,
	Attack_Twohand,
	Attack_Bow,
	Attack_Magic,
};


class iMagic;
class iAnimation;

//动作的优先级如下！

//受伤可以打断魔法和运动
//运动被受伤打断以后接着运动
//运动和魔法相互不能打断
//运动和魔法可以打断劳动
//劳动不能打断任何动作

//运动一次只走一格，这个是上一级的原则

//定义长和宽的比例
const float c_xy = (float)c_nMagicTileWidth/c_nMagicTileHeight;
const float c_yx = (float)c_nMagicTileHeight/c_nMagicTileWidth;

const int CHARACTER_ANI_COUNT = 1;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CharacterDataProperty : public iProperty
{
public:
	stAniInfoData stAniInfo;
	stAnimationCharacterDataStruct stAnimation[CHARACTER_ANI_COUNT];
	stEventData aEvent[MAX_EVENT];
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CharacterProperty : public iProperty
{
public:
	stAniInfo stAniinfo;
	stAnimationStruct stAnimation[CHARACTER_ANI_COUNT];
	stEvent aEvent[MAX_EVENT];
};

struct stSoundFx
{
	int nStartID;	//id of sound
	int nNum;		//rand offset select of the id
	stSoundFx()
	{
		nStartID = 0;
		nNum = 0;
	}
};

extern int g_nCharMagicTableID;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class iCharacter : public iObj
{
public:
	static bool s_bHightlight;
	static bool s_bDrawCover;
	virtual bool IsExistAction(AniTypeEnum ani) = 0;
	virtual int GetMoveSpeedLevel(AniTypeEnum n) = 0;
	virtual bool IsCreate()=0;
	virtual POINT GetMoveSpeed() = 0;
	virtual bool ActionAni(int nAni,bool bServerLogic = false) = 0;
	virtual void SetDirFrame(int nFrame)=0;
	virtual ULONG AddRef() = 0;
	virtual ULONG Release() = 0;
	virtual void FinalRelease() = 0;
	virtual void UpdatePrevAniEndTime(long time_eslasped)=0;
	//重生
	virtual bool Init(bool bAppear) = 0;
	virtual bool CreateCharacter(int sorttype,const std::vector<int>& aID,bool bAppear) = 0;
	//part
	virtual size_t GetPartID(AniPartEnum nAniPart) = 0;
	virtual void ChangePartID(AniPartEnum nAniPart, int id) = 0;
	virtual bool IsNpc() = 0;

	//generate
	virtual bool Generate(tagListMagicGenerate* listMagic) = 0;
	virtual void AddMagic(iMagic* p) = 0;

	//Arrow in right hand
	virtual void SetArrowInRightHand(bool bInRightHand) = 0;

	//pos
	virtual void SetPos(POINT pt) = 0;
	virtual POINT GetPos() = 0;
	virtual void SetPosCenter(POINT pt) = 0;
	virtual POINT GetDes() = 0;
	virtual POINT GetMagicPosition() = 0;
	virtual POINT GetMagicPos() = 0;
	virtual POINT GetMagicAdjust() = 0;

	//绘制某个排序类型的magic
	//绘制光环类的
	//virtual void DrawCover(POINT ptOffset) = 0;
	virtual bool DrawMagicLowest(POINT ptOffset) = 0;
	virtual bool DrawMagicFrontest(POINT ptOffset) = 0;
	virtual bool DrawMagicAboveAll(POINT ptOffset) = 0;
	//缩骨
	virtual bool DrawMagicLowest2(POINT ptOffset) = 0;
	virtual bool DrawMagicFrontest2(POINT ptOffset) = 0;
	virtual bool DrawMagicAboveAll2(POINT ptOffset) = 0;
	virtual bool Draw2(POINT ptMapOffset) = 0;
	virtual POINT GetDrawPosition() = 0;
	virtual bool DrawPart(AniPartEnum ePart,POINT ptMapOffset) = 0;

	//一共12组动作
	virtual void Turn(POINT pt) = 0;
	virtual void TurnTo(size_t n) = 0;
	virtual bool TurnTo(size_t n,bool bServerLogic) = 0;
	virtual bool WalkTo(POINT ptDes,bool bServerLogic) = 0;
	virtual bool RunTo(POINT ptDes,bool bServerLogic) = 0;
	virtual bool FlyTo(POINT ptDes,bool bServerLogic) = 0;
	virtual bool MoveTo(POINT ptDes,AniTypeEnum n,bool bServerLogic) =0;
	virtual enumCreateMagicResult Magic(POINT point, size_t n,iMagic** pMagic,bool bServerLogic, int nSetLife,AniTypeEnum nAction = Ani_Null) = 0;
	virtual enumCreateMagicResult Magic(iCharacter* pCharacter, size_t n,iMagic** pMagic,bool bServerLogic, int nSetLife,AniTypeEnum nAction = Ani_Null) = 0;
	virtual bool Hurt() = 0;
	virtual bool Back(POINT ptDes,bool bServerLogic = false) = 0;
	virtual bool Defend() = 0;
	virtual bool Die() = 0;
	virtual bool Stop(POINT pt) = 0; //强制停止
	virtual bool Action(AniTypeEnum n,bool bServerLogic=false) = 0;

	virtual DWORD GetColor() = 0;
	virtual stAnimationMagicDataStruct::BlendEnum GetBlendType() = 0;
	virtual void ChangeBlendType(stAnimationMagicDataStruct::BlendEnum eBlend)=0;
	virtual void ChangeBlendType(AniPartEnum part,stAnimationMagicDataStruct::BlendEnum eBlend)=0;

	//属性
	virtual AniTypeEnum GetAction() = 0;
	virtual size_t GetDir() = 0;
	virtual size_t GetFrame() = 0;
	virtual size_t GetDirFrame() = 0;
	virtual POINT GetMoveDir() = 0;
#ifdef _GAME_TOOL
	virtual size_t GetFrameOfDir() = 0;
	virtual const stAnimationStruct& GetAnimationStruct(int n,int num = 0) = 0;
#endif 
	virtual bool IsMoving() = 0;
	virtual bool IsWait() = 0;	//
	virtual bool IsMovingBreak() = 0;
	virtual bool IsAttacking() = 0;
	virtual bool IsDie() = 0;
	virtual bool CanChangeToAction(AniTypeEnum nAction) = 0;
	virtual bool IsReachDes() = 0;
	//图片的属性
	virtual bool IsAt(POINT pt, bool bJustInRect = false) = 0;	//计算alpha
//	virtual bool Contain(POINT pt) = 0;
	//help function!
	virtual int GetDir(POINT pt,int maxDir = 8) = 0;

	virtual void SetSex(int nSex) = 0;

	//ani
	virtual iAnimation* GetMainAnimation() = 0;

	//sound
	//例如走在沙地的声音跟走在草地不同
	virtual void SetSoundFx(int nAni,const stSoundFx& fx) = 0;
	//因为攻击跟武器有关
	virtual void SetSound(int nAni,LPCSTR szSound) = 0;
	//命中的声音由上一层逻辑决定
	virtual void PlayHitSound(LPCSTR szSound) = 0;
	//virtual void OnHit(int magic, POINT ptKillerPos) = 0;

	//light
	virtual iLight* GetLight() = 0;
	virtual bool AddLight(POINT ptMapOffset) = 0;

	//骑乘的
	virtual bool SetRide(int monsterID) = 0;
	virtual bool IsRide() = 0;

	//////////////////////////////////////////////////////////////////////////
	//效果
	virtual void StartKeepImage(const stKeepImageControl& st) = 0;
	virtual void ClearKeepImage() = 0;
	virtual void Fade(const stFade& st) = 0;
	virtual void OnMagicBegin(iMagic* p) = 0;
	virtual void OnMagicEnd(iMagic* p) = 0;
	virtual void SetMoveable(bool b) = 0;
	
	//////////////////////////////////////////////////////////////////////////
	//时间
	virtual std::vector<stEventEx>& GetEventList() = 0;
	virtual void ClearEventList() = 0;

	//////////////////////////////////////////////////////////////////////////
	//设置整体的状态和属性
	//virtual void SetMainCharacter(bool b) = 0;
	//设置单独的状态和属性
	virtual void ChangeColor(DWORD cr) = 0;
	virtual void ChangeAlpha(BYTE alpha) = 0;
	virtual BYTE GetAlpha() = 0;
	//改变颜色
	virtual void ChangeColor(AniPartEnum part, DWORD cr) = 0;
	virtual void ChangeAlpha(AniPartEnum part, BYTE alpha) = 0;
	//回到普通速度
	virtual void ResetSpeed() = 0; 
	//改变所有的speed
	virtual bool ChangeSpeed(float fSpeed) = 0;
	virtual void ChangeSpeed(int nXspeed,int nYspeed) = 0;
	virtual int  GetSpeedX() = 0 ;
	virtual int  GetSpeedY() = 0 ;
	
	virtual float GetSpeed() = 0;
	//改变某个动作的动画速度
	virtual bool ChangeSpeed(AniTypeEnum e, float ms) = 0;
	//改变某个动作的等待时间	
	virtual bool ChangeWaitSpeed(AniTypeEnum e, int ms) = 0;
	virtual int GetWaitSpeed(AniTypeEnum e) = 0;
	//去掉某个类型的所有魔法
	virtual void RemoveMagic(MagicTypeEnum e) = 0; 
	virtual void RemoveAMagic(size_t i,size_t table = 0)= 0;
	virtual iMagic* FindMagic(size_t magicType,size_t table = 0) = 0;
	virtual int GetMagicTableID() = 0;
	//简便的施法方式
	virtual bool SetMagicRing(size_t magicType,int nSetLife,int table = 0) = 0;
	virtual bool AddMagicRing(iMagic* p) = 0;
	//简便的效果方式
	virtual bool SetMagicEffect(size_t magicType,int nSetLife,int table = 0) = 0;
	virtual bool SetMagicEffect(LPCSTR szAni) = 0;

	//For some special image we change default center
	virtual void SetDefaultCenter(stPointI ptCenter) = 0;

	//draw
	virtual POINT GetDefaultCenter() = 0;
	virtual int GetImageHeight() = 0;
	virtual POINT GetImageOffset() = 0;

	virtual bool GetDrawRect(POINT pt,RECT & rc) = 0;

	virtual void PauseMove(bool b) = 0;
	virtual bool IsPauseMove() = 0;

	virtual void SetLayerParam(DWORD dwLayerColor,float fScaleX,float fScaleY) = 0;

	virtual bool IsMagicRing(size_t magicType) = 0;
	virtual bool IsMagicAttackRing(size_t magicType) = 0;

	virtual void SetMainCharacter() = 0;
	virtual bool IsMainCharacter() = 0;

	virtual bool IsHasAniImage(AniTypeEnum n) = 0;
	virtual AniTypeEnum GetRandAni(AniTypeEnum ani1,AniTypeEnum ani2,AniTypeEnum ani3) = 0;

	virtual void SetMoveSpeed(DWORD dwAniTime) = 0;
	virtual void SetAttackSpeed(float percentIntervalTime) = 0;
	virtual void SetMagicSpeed(float percentIntervalTime) = 0;

	virtual bool DieEnd() = 0;
	virtual bool Appear() = 0;

	virtual float GetAniSpeed( AniTypeEnum e) = 0;
	virtual void SetRunAniSpeed(float newms) = 0;
	virtual void RestoreRunAniSpeed() =0 ;
	virtual void SetMoveSpeed( float fSpeed ) = 0;

	virtual void ResetSound(int nAni) = 0;

	virtual bool FindAndAddMagic(iMagic * p) = 0;

	virtual bool CanChangeRideState() = 0;
};

EXPORT_API iCharacter* Character_CreateCharacter();

extern iCharacter* Character_GetMainCharacter();

extern POINT Character_GetMainPosition();

#pragma pack()