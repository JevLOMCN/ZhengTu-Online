#pragma once
#include "iCharacter.h"
class iAnimation;
#include <list>
#include <vector>

#ifdef _GAME_TOOL
#include "../MagicEditor/RenderDump.h"
#endif 

#include "CharacterOrder.h"
#include "./magic_sound.h"
const int c_nMaxDir = 8;



class CSceneSound;
class iLight;
class iMagic;

class cCharacterTable;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class cAnimationArray;

#include "cAnimationArray.h"

struct stAni
{
	long nPrevAniEndTime;
	long nDelayTime;
	long nSpeed;
	long nPrevPlaySoundTime;
	float fMoveSpeed;
	char strSoundName[32];

	stSoundFx soundFx;
	stAni()
	{
		nPrevPlaySoundTime = 0;
		nPrevAniEndTime = 0;
		nDelayTime = 0;
		nSpeed = 0;
		fMoveSpeed = 0;
		strSoundName[0] = 0;
	}
	virtual ~stAni();
};


class cFileOP;


/**
* \brief 简短描述
* 
* 详细描述
* 
*/
class cCharacter : public iCharacter
{
	friend class cFileOP;
private:
	struct stPrevAni{
		AniTypeEnum ani;
		float		endFrame; // 跑或者骑马跑要考虑上一帧的连续性
		DWORD		endTime;
	};

	stPrevAni m_prevMoveAni;
	DWORD m_dwPlayRandSoundTime;
	DWORD m_dwChangeIdleTime;
	DWORD m_dwPlayMoveTime;

	ULONG m_ulCount;

	void* m_lData;
	std::string m_strFootprint;

	int m_nMainAniIndex;	// 一个角色包含几个不同部分，每个部分视为１个动画，这个变量用来定位主动画,如主角一般以body部分为MainIndex
	int m_nRunNum;			// 是否第一次运行,(现在第一次除了位置好像没做其他任何东西)
	int m_id;				// 本类所代表的npc id
	long m_nRun;			// 人物的时间控制(用于控制动画的节奏)

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// 数据库中的每个字段是怎样反应的,这是关键
	iPropertyTable* m_pTable;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	stAni m_aAni[Ani_Num];  // 人物每个动作的几个信息(上一次动画结束时间 | 声音等)
	iLight* m_pLight;

	// 在上面加一层颜色,颜色为 0 表示不加层
	DWORD m_dwLayerColor;
	float m_fLayerScaleX;
	float m_fLayerScaleY;
	
	std::vector<stEventEx> m_listEvent;							//生成的事件表
	
	POINT m_ptMagicPos;		//magic pos
	POINT m_ptCenter;
	int m_xSpeed;
	int	m_ySpeed;
	float	m_xPos; //  世界坐标x(象素)
	float	m_yPos; //  世界坐标y(象素)
	POINT	m_ptDes;
	POINT	m_ptSrc;
	DWORD	m_dwMoveBeginTime;
	AniTypeEnum m_ePreAni;			//上一个动作是什么

	//用来处理stand的中间过程.
	int m_nWait;
	int m_nWaitTime;

	std::vector <iMagic*> m_listMagic;       
	std::vector <iMagic*> m_listGenerate;

	cAnimationArray m_Animation;

	int  m_nSex;
 
	//骑乘的
	cCharacter* m_pRide;	

	int m_nSortType;//// always be 0
	static BYTE m_aOrder[c_nMaxDir][Ani_Part_Num];
	static BYTE m_aOnRideOrder[c_nMaxDir][Ani_Part_Num];
	static int	 s_aRideOrder[c_nMaxDir];
	static int s_nGridX,s_nGridY;

	D3DXCOLOR m_crMouseOver;
	AttackType m_eAttack;

	//外部设置
	float m_fRunSpeed;
	bool m_bNpc;
	bool m_bDie;
	bool m_bAppear;
	bool m_bEnableShake;
	bool m_bPauseMove;

	bool m_bArrowInRightHand;
	POINT m_ptMoveDir;
public:	
	DWORD GetRunTime() { return m_nRun;}
	cCharacter(void);
	~cCharacter(void);

	void SetArrowInRightHand(bool bInRightHand) { m_bArrowInRightHand = bInRightHand;}

	bool IsExistAction(AniTypeEnum ani);
	bool IsCreate(){ return m_Animation.GetAnimationCount()> 0;}
	void SetFootprint(const char* filename){m_strFootprint = filename;}
	bool IsHaveFootprint(){ return !m_strFootprint.empty();}
	
	void GetFrameInfo(size_t frame,DWORD & time,POINT & pt);
	
	void SetEnableShake(bool b) {m_bEnableShake = b;}
	bool IsEnableShake() { return m_bEnableShake;}

	int GetObjType(){ return eObjType_Character;}

	void SetKeepImage(long time_eslasped,POINT ptPos,POINT ptCenter,BYTE* pOrder)
	{
		m_Animation.KeepImage(time_eslasped,ptPos,ptCenter,pOrder);
	}
	void ClearKeepImage()
	{
		m_Animation.ClearKeepImage();
	}
	void DrawKeepImage(POINT ptScreen,bool bOrder)
	{
		m_Animation.DrawKeepImage(ptScreen,bOrder);
	}

	void CopyState(const cAnimationArray * pAniArray)
	{
		m_Animation.CopyState(pAniArray);
	}
//////////////////////////////////////////////////////////////////////////
//com
	
	ULONG AddRef() {return ++m_ulCount;};
	ULONG Release();

	//data
	void SetData(void* ldata) { m_lData = ldata;};
	void* GetData() {return m_lData;};
//////////////////////////////////////////////////////////////////////////
//loop
	bool Init(bool bAppear);
	bool CreateCharacter(int nSortType, const std::vector<int>& aID,bool bAppear);
	bool InitCreate(int nSortType, const std::vector<int>& aID,bool bAppear);
	bool ReCreate(int nSortType, const std::vector<int>& aID,bool bAppear = false);
	size_t GetPartID(AniPartEnum part){return m_Animation.GetPartID(part);}
	bool HaveAniStruct(int n);
	
	int Run(long time_eslasped);
	bool Generate(tagListMagicGenerate* listMagic);

	void ReleaseAnimation();
	void FinalRelease();
	HRESULT InvalidateDeviceObjects() ;	//before call reset alt+tab 
	HRESULT RestoreDeviceObjects() ;		//after alt+tab

//////////////////////////////////////////////////////////////////////////
//property
	const stAnimationStruct& GetAnimationStruct(int n,int num = 0);
	const stAniInfo& GetAniInfo(int n);
//////////////////////////////////////////////////////////////////////////
//custom ani data
	stSoundFx& GetSoundFx(int e){return m_aAni[e].soundFx;}

	int GetPrevAniEndTime(int e);
	void SetPrevAniEndTime(int e,int n);
	void UpdatePrevAniEndTime(long time_eslasped);
//////////////////////////////////////////////////////////////////////////
//draw
	POINT GetDrawPosition();
	bool DrawAtScreen(POINT ptScreen,BYTE* aOrder);
	bool DrawAtScreen2(POINT ptScreen,BYTE* aOrder);
	bool DrawCoverAtScreen(POINT ptPosition,BYTE* aOrder);
	bool DrawCoverAtScreen2(POINT ptPosition,BYTE* aOrder);
	bool DrawPart(AniPartEnum ePart,POINT ptOffset);
	bool Draw(POINT ptMapOffset);	//当前的偏移
	bool Draw2(POINT ptMapOffset);	//当前的偏移
	bool DrawAtScreen(POINT ptScreen);	//当前的偏移
	bool DrawAtScreen2(POINT ptScreen);	//当前的偏移
	//adjust
	POINT GetMagicAdjust();
	POINT GetMagicPosition();
	POINT GetMagicPosition(POINT ptPos);
	POINT GetMagicPos() { return m_ptMagicPos; }		//Get magic render pos
	//magic
	bool DrawMagicLowest(POINT ptOffset);
	bool DrawMagicFrontest(POINT ptOffset);
	bool DrawMagicAboveAll(POINT ptOffset);
	//缩骨
	bool DrawMagicLowest2(POINT ptOffset);
	bool DrawMagicFrontest2(POINT ptOffset);
	bool DrawMagicAboveAll2(POINT ptOffset);
	bool DrawMagicDir(POINT ptScreen,int e,int nDir);
	bool DrawMagicDir2(POINT ptScreen,int e,int nDir);//缩骨
	//light
	bool IsLight();
	
	iLight* GetLight(){return m_pLight;}
	bool AddLight(POINT ptMapOffset) ;

	void SetLayerParam(DWORD dwLayerColor,float fScaleX,float fScaleY)
	{
		m_dwLayerColor = dwLayerColor;
		m_fLayerScaleX = fScaleX;
		m_fLayerScaleY = fScaleY;
	}

//////////////////////////////////////////////////////////////////////////
//event
	const stEvent& GetEvent(int n,int i);
	std::vector<stEventEx>& GetEventList(){return m_listEvent;}
	void ClearEventList();
	bool OnEvent(const stEvent& st,int indexEvent,iObj* pSender);

/////////////////////////////////////////////////////////////////////////
//move
/////////////////////////////////////////////////////////////////////////
	//pos
	void SetPos(POINT pt){SetPos(pt.x,pt.y);}
	void SetPos(float fx,float fy);
	POINT GetPos();
	void SetPosCenter(POINT pt);
	
	POINT GetCenter();
	//move
	float GetMoveSpeed(AniTypeEnum n);
	float GetWalkSpeed(){return GetMoveSpeed(Ani_Walk);};
	float GetRunSpeed(){return GetMoveSpeed(Ani_Run);}

	POINT GetDes(){return m_ptDes;}
	POINT GetSrc(){return m_ptSrc;}

	void SetSrc(POINT pt){ m_ptSrc = pt;}

	bool IsReachDes();
	void SetDes(POINT pt);
	void PauseMove(bool b){m_bPauseMove = b; if(m_pRide)m_pRide->PauseMove(b);}
	bool IsPauseMove(){return m_bPauseMove;}
	
//////////////////////////////////////////////////////////////////////////
//frame and dir
	void ChangeFrame(size_t iFrame);
	size_t GetDir();
	size_t GetFrame() ;
	size_t GetDirFrame();
	size_t GetFrameOfDir();
	bool IsFaceScreen(){return IsFaceScreen(GetDir());}
; //当前方向是否朝向屏幕
	bool IsFaceScreen(size_t dir){return !(dir == 0 || dir == 1 || dir == 7);}
;
	int GetDir(POINT pt,int maxDir = 8);
	POINT GetMoveDir();
//turn
	void TurnTo(size_t n);
	void Turn(POINT ptDirection);
	void TurnTo(POINT ptDes);
	void TurnBack();

//////////////////////////////////////////////////////////////////////////
//action
//////////////////////////////////////////////////////////////////////////
//action
	bool Action(AniTypeEnum nAction,bool bServerLogic = false);
	void AssertActionEnd(AniTypeEnum nPrev);
	bool AssertActionBefore(AniTypeEnum nNext);
	bool ActionAni(int n,const stAnimationStruct& stAnimation,bool bServerLogic = false);
	bool ActionAni(int nAni,bool bServerLogic = false);
	int GetCurrentAniPri(){return GetAniPri(GetAction());}
	bool CanChangeToAction(AniTypeEnum nAction);	//equal return false
	bool CanChangeToAction2(AniTypeEnum nAction);	//equal return true

	AniTypeEnum GetAction();
	
	AniTypeEnum GetPreAction(){return m_ePreAni;}

	//ani data
	AniPriEnum GetAniPri(AniTypeEnum n){return GetAniInfo(n).ePri;}
	int GetWaitSpeed(AniTypeEnum n);
	int GetAniDelay(AniTypeEnum n);
	POINT GetAniOffset(AniTypeEnum n){return GetAniInfo(n).ptOffset;}
	void GetAniSound(AniTypeEnum n,char* strBuf,size_t bufSize){
		if (m_aAni[n].strSoundName[0]!=0)
		{
			strncpy(strBuf, m_aAni[n].strSoundName,bufSize);
		}
		else
		{
			strncpy(strBuf,GetAniInfo(n).strSound,bufSize);
		}
	}
	AniTypeEnum GetAniNextAction(AniTypeEnum n){return GetAniInfo(n).eNextAni;}
	int GetAniMoveSpeed(AniTypeEnum n){return GetAniInfo(n).nMoveSpeed;};
	AniTypeEnum GetAniRideAction(AniTypeEnum n){return GetAniInfo(n).eRideAni;};	 // 得到"骑马动作"
	AniTypeEnum GetAniUnrideAction(AniTypeEnum n)
	{
		if(this->m_id == 0){
//#pragma message(ATTENTION(因为编辑器数据错误，是人物时强行判断))
			switch(n){
			case Ani_Ride_Wait:
				return Ani_Wait;
			case Ani_Ride_Walk:
				return Ani_Walk;
			case Ani_Ride_Run:
				return Ani_Run;
			case Ani_Ride_Attack:
				return Ani_Attack;
			case Ani_Ride_Attack_Stand:
				return Ani_Attack_Stand;
			case Ani_Ride_Magic:
				return Ani_Magic;
			case Ani_Ride_Magic_Stand:
				return Ani_Magic_Stand;
			case Ani_Ride_Bow:
				return Ani_Bow;
			case Ani_Ride_Bow_Stand:
				return Ani_Bow_Stand;
			case Ani_Ride_Back:
				return Ani_Back;
			case Ani_Ride_Hurt:
				return Ani_Hurt;
			case Ani_Ride_Die:
				return Ani_Die;
			}
			return n;
		}
		return GetAniInfo(n).eUnrideAni;
	}; // 得到"下马动作"

//22 action
	bool WalkTo(POINT ptDes,bool bServerLogic);
	bool RunTo(POINT ptDes,bool bServerLogic);
	bool FlyTo(POINT ptDes,bool bServerLogic);
	bool MoveTo(POINT ptDesPoint,AniTypeEnum Ani,bool bServerLogic = false);
	bool TurnTo(size_t n,bool bServerLogic);

	enumCreateMagicResult Magic(POINT point, size_t nMagicType,iMagic** ppMagic,bool bServerLogic,int nSetLife,AniTypeEnum nAction = Ani_Null);
	enumCreateMagicResult Magic(iCharacter* pDes, size_t nMagicType,iMagic** ppMagic,bool bServerLogic,int nSetLife,AniTypeEnum nAction = Ani_Null);
	enumCreateMagicResult Magic(stFly& fly, size_t nMagic,iMagic** ppMagic,bool bServerLogic,int nSetLife,AniTypeEnum nAction = Ani_Null);

	bool Hurt();
	bool Back(POINT ptDes,bool bServerLogic);
	bool Defend();
	bool Die();
	bool Appear();

	bool StopIn();
	bool Stop(POINT pt);
	void Idle();

//magic
	void cCharacter::AddMagic(iMagic *p)
	{
		if(m_listMagic.capacity()==0) 
			m_listMagic.reserve(64);
		m_listMagic.push_back(p);
#ifdef _GAME_TOOL
		GetRenderDump()->AddWatchMagic(p);
#endif
	}

	bool FindAndAddMagic(iMagic * p);
	void AddGenerate(iMagic* p)
	{ 
		if(m_listGenerate.capacity() == 0) 
			m_listGenerate.reserve(64); 
		m_listGenerate.push_back(p);
	}
	//animation
	iAnimation* GetMainAnimation()
	{
		return m_Animation.GetAnimation(m_nMainAniIndex);
	};
	int GetAnimationTime(){return (int)m_Animation.GetAnimationTime();}
	int GetAnimationNum(){return (int)m_Animation.GetNum();};
	void ChangePartID(AniPartEnum nAniPart, int id);
	void UpdateAnimation();
	void SetDirFrame(int nFrame);
//sound
	int GetSoundFrame(AniTypeEnum n){return GetAniInfo(n).nSoundFrame;}
	bool IsAniLoop(AniTypeEnum n){return GetSoundFrame(n) == -1;}
	void PlayCurSound();
	
	void PlayActionSound(AniTypeEnum ani);
//一些属性

	bool IsDie(){return m_bDie;}
	bool IsWait();
	bool IsMoving();
	bool IsMovingBreak();
	bool IsMoving(AniTypeEnum nAni);
	bool IsFemale(){return !IsMale(); };
	bool IsMale(){return m_nSex == 0; }
	bool IsNpc(){return m_bNpc;}
	bool IsAttacking();

	void SetSex(int nSex) { m_nSex = nSex; }
//图片相关
	bool IsAt(POINT pt,bool bJustInRect);
	void GetImageSize(int& w,int& h);
	bool Contain(POINT pt);

//不能简单的使用一个sound,比如Hurt,动作结束了但是声音未结束
	void PlayFxSound(int index);
	
	void SetSound(int nAni,LPCSTR szSound)
	{
		if (nAni >= 0 && nAni < Ani_Num)
		strncpy(m_aAni[nAni].strSoundName , szSound,sizeof(m_aAni[nAni].strSoundName));
	}
	void ResetSound(int nAni)
	{
		if (nAni >= 0 && nAni < Ani_Num)
		{
			iProperty* p = m_pTable->GetProperty(nAni);
			Assert(p);
			CharacterProperty* pP = (CharacterProperty*) p;
			strncpy(m_aAni[nAni].strSoundName, pP->stAniinfo.strSound,sizeof(m_aAni[nAni].strSoundName));
		}
	}
	void SetSoundFx(int nAni,const stSoundFx& sound)
	{
		if (nAni >= 0 && nAni < Ani_Num)
		m_aAni[nAni].soundFx = sound;
	}

	void PlayHitSound(LPCSTR szSound);
	void OnHit(int magic, POINT ptKillerPos);

	bool SetRide(int monsterID);
	bool IsRide(){return m_pRide != NULL;};

//effect
	void StartKeepImage(const stKeepImageControl& st){m_Animation.StartKeepImage(st);};
	void Fade(const stFade& st) {m_Animation.Fade(st);};

	void OnMagicBegin(iMagic* p);
	void OnMagicEnd(iMagic* p);
	void OnMagicEffect(stCharacterStruct::EffectEnum e,iMagic* p);

	bool m_bMoveable;
	void SetMoveable(bool b){m_bMoveable = b;}
	bool IsMoveable(){return m_bMoveable;}
public:
	//sort
	BYTE* GetOrder();
	static void InitOrder();
	stCharacterSortStruct* m_pOrder;			// 人物排序数据
	BYTE* GetCharacterOrder()
	{
		if(m_pOrder  == NULL) 
			return NULL;
		return NULL;
	}
	//color
	BYTE GetAlpha(){return m_Animation.GetAlpha();}
	DWORD GetColor(){ return m_Animation.GetColor();}
	
	void ChangeColor(AniPartEnum part,DWORD cr){
		m_Animation.ChangerColor(part,cr);
	}
	void ChangeColor(DWORD cr){
		m_Animation.ChangerColor(cr);
	}
	void ChangeAlpha(AniPartEnum part,BYTE alpha){	
		m_Animation.ChangeAlpha(part,alpha);
	}
	void ChangeAlpha(BYTE alpha)
	{
		#ifdef _DEBUG
			if ( IsMainCharacter() )
				int iiii = 0;
		#endif
		m_Animation.ChangeAlpha(alpha);
		if(m_pRide)
			m_pRide->ChangeAlpha(alpha);
	}

	stAnimationMagicDataStruct::BlendEnum GetBlendType(){ return m_Animation.GetBlendType();}

	void ChangeBlendType(stAnimationMagicDataStruct::BlendEnum eBlend)
	{
		m_Animation.SetBlendType(eBlend);
	}
	void ChangeBlendType(AniPartEnum part,stAnimationMagicDataStruct::BlendEnum eBlend)
	{
		m_Animation.SetBlendType(part,eBlend);
	}
//////////////////////////////////////////////////////////////////////////

	POINT GetMoveSpeed();
	
	void SetMoveSpeed(DWORD dwAniTime);
	void SetAttackSpeed(float percentIntervalTime);
	void SetMagicSpeed(float percentIntervalTime);

	//回到普通速度
	void ResetSpeed() ; 
	//改变所有的speed
	bool ChangeSpeed(float fSpeed) {m_fRunSpeed = fSpeed; return true;};
	void ChangeSpeed(int nXspeed,int nYspeed);
	int  GetSpeedX(){return m_xSpeed;};
	int  GetSpeedY(){return m_ySpeed;};
	float GetSpeed() {return m_fRunSpeed;};
	//改变某个动作的动画速度
	bool ChangeSpeed(AniTypeEnum e, float ms) ;
	//改变某个动作的等待时间
	bool ChangeWaitSpeed(AniTypeEnum e, int ms) ;
	//去掉某个类型的所有魔法
	void RemoveMagic(MagicTypeEnum e); 
	void RemoveAMagic(size_t magicType,size_t magicTable = 0);
	//设置follow人的魔法
	bool IsMagic(MagicTypeEnum e, size_t magicType){return IsMagic(e,magicType,GetMagicTableID());}
	bool IsMagic(MagicTypeEnum e, size_t magicType, int table);
	bool SetMagic(size_t magicType,int nSetLife,int table);
	iMagic* FindMagic(size_t magicType,size_t magicTable = 0);

	bool IsMagicRing(size_t magicType);
	bool IsMagicAttackRing(size_t magicType);
	//简便的施法方式
	bool SetMagicRing(size_t magicType,int nSetLife,int table = 0);
	bool AddMagicRing(iMagic* p);
	//简便的效果方式
	bool SetMagicEffect(size_t magicType,int nSetLife,int table);
	bool SetMagicEffect(LPCSTR szAni);
	bool IsMagicEffect(size_t magicType){return IsMagic(Magic_Effect,magicType,0);};

	//int GetMagicTableID(){return IsNpc()?1:0;}
	int GetMagicTableID(){return g_nCharMagicTableID;}

	void SetAttackType(int type){m_eAttack = (AttackType)type;};

	void SetDefaultCenter(stPointI ptCenter);

//sort
	//character始终在地上！
	POINT GetSortPos(){return GetPos();};

	POINT GetDefaultCenter();
	int GetImageHeight() ;
	POINT GetImageOffset();

	bool GetDrawRect(POINT pt,RECT & rc);

	void SetMainCharacter();
	bool IsMainCharacter();
	int GetMoveSpeedLevel(AniTypeEnum n);
	size_t GetFrameOfDir(int Ani);

	bool IsHasAniImage(AniTypeEnum n);
	AniTypeEnum GetRandAni(AniTypeEnum ani1,AniTypeEnum ani2,AniTypeEnum ani3);

	float GetAniSpeed( AniTypeEnum e);
	bool DieEnd();


	void SetRunAniSpeed(float newms);
    void RestoreRunAniSpeed();
	void SetMoveSpeed( float fSpeed );

	bool ChangeIdleAction();

	bool CanChangeRideState();
};

