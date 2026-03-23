#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <list>
#include "../engine/include/stack_alloc.h"

extern const int c_nMagicResourceFlags;
extern const int c_nMapResourceFlags;
extern const int c_nTerrResourceFlags;

#define GRID_WIDTH 64
#define GRID_HEIGHT 32

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class iMagic;
typedef std::vector<iMagic*,std::stack_allocator<iMagic*> > tagListMagicGenerate;
//typedef std::vector<iMagic*> tagListMagicGenerate;

#pragma warning( disable : 4244 ) 

#ifdef _USEDLL
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API
#endif

const int c_nMagicTileWidth = 64;
const int c_nMagicTileHeight = 32;

//#define Frame2Time(a) ((a) * time_eslasped * 25 / 1000)
//#define Time2Frame(a) ((a) * 1000 / 25)
#define Time2Speed(a)  ((float)(a) * ((float)(time_eslasped)/1000.f))
const int MAX_MAGICNAME = 16;

enum LightTypeEnum
{
	Light_Point,		//点光源，为椭圆形
	Light_Envirment,	//环境光源，照亮整个屏幕
};
enum LightEffectEnum
{
	Light_Effect_Null,
	Light_FadeColor,	//淡入淡出的效果
	Light_FadeRadius,
	Light_FadeAll,
	Light_Setting,		//使用设置的参数（半径和颜色）
	Light_Custom,		//使用一组图片作为lightmap
};


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class iCharacter;
struct stFly
{
	POINT ptStart;
	POINT ptDes;
	POINT ptPos;
	iCharacter* pSender;
	iCharacter* pDes;
};

const int MAX_EVENT = 2;

struct stEventData
{
	enum EventEnum
	{
		Event_Null,
		Event_Die,
		Event_Music,
		Event_Start,
		Event_End,
		Event_Hit,
		Event_Magic,
		Event_Help,		//帮助
		Event_Reject,	//弹开
		Event_Shake,	//震荡
	};
	EventEnum eEvent;
	enum EventGenerateEnum
	{
		Event_Generate_Null,
		Event_Generate_FrameStart,		//每到帧数
		Event_Generate_FrameEnd,		//每生剩下帧数
		Event_Generate_FrameEach,		//每隔帧数，例如每隔4帧就是0,4,8,12……
		Event_Generate_StartContinue,	//从该帧前连续
		Event_Generate_StartRandom,		//从该帧前随机
		Event_Generate_EndContinue,		//从该帧后连续，每帧伤害
		Event_Generate_EndRandom,		//从该帧后随机
		Event_Generate_EndContinue2,	//从该帧后持续
	};
	EventGenerateEnum eGenerate;
	int nEventID;						//与上面不同特别指定的
	char szEventData[MAX_MAGICNAME];	//使用参数，
	int nEventFrame;					//使用帧数，每到这个帧数就播放事件
	int nEventTime;						//使用时间，只会播放一次
};


struct stEvent : public stEventData
{
	int nMagicType;						//由什么对象引起的event , character填0
	stEvent()
	{
		eEvent = Event_Null;
		eGenerate = Event_Generate_Null;
		nEventID = 0;		
		nEventFrame = -1;
		nEventTime = -1;
		szEventData[0] = 0;
		nMagicType = 0;
	}
	bool Run(long time_eslasped,int preFrame,int curFrame,int allframe) const;	
	bool Run0() const;
};
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class iObj;
struct stEventEx : public stEvent{
	iObj* pSender;
	stEventEx()
		: pSender(NULL) {}
};
#ifndef _ANI_TYPE_ENUM
#define _ANI_TYPE_ENUM
enum AniTypeEnum
{
	Ani_Null = 0,

	Ani_Wait,Ani_Stand = Ani_Wait,	// 站立
	Ani_Walk,						// 走
	Ani_Run,						// 跑

	Ani_Attack,						// 攻击
	Ani_Attack2,					// 攻击2
	Ani_Attack3,					// 攻击3

	Ani_Magic,						// 魔法攻击
	Ani_Magic2,						// 魔法攻击2
	Ani_Magic3,						// 魔法攻击3

	Ani_Bow,						// 弓箭射击
	Ani_Bow2,						// 努攻击

	Ani_Hurt,						// 受伤(被击)
	Ani_Die,						// 死亡

	Ani_Sit,						// 打坐

	Ani_Ride_Wait,					// 骑马站立
	Ani_Ride_Walk,					// 骑马走
	Ani_Ride_Run,					// 骑马跑

	Ani_Ride_Attack,				// 骑马攻击
	Ani_Ride_Magic,					// 骑马施法

	Ani_Ride_Hurt,					// 受伤(骑马被击)
	Ani_Ride_Die,					// 骑马死亡

	Ani_Appear,						// 出现(重生)

	Ani_Attack_Stand,				// 攻击站立
	Ani_Attack2_Stand,				// 攻击2站立
	Ani_Attack3_Stand,				// 攻击2站立

	Ani_Magic_Stand,				// 魔法攻击站立
	Ani_Magic2_Stand,				// 魔法2攻击站立
	Ani_Magic3_Stand,				// 魔法3攻击站立

	Ani_Bow_Stand,					// 弓箭攻击站立
	Ani_Bow2_Stand,					// 努攻击站立

	Ani_Ride_Attack_Stand,			// 骑马攻击站立
	Ani_Ride_Magic_Stand,			// 骑马施法站立

	Ani_Back,						// 后退
	Ani_Ride_Back,					// 骑马后退

	Ani_Ride_Bow,					// 骑马射箭
	Ani_Ride_Bow_Stand,				// 骑马射箭站立

	Ani_Fly_Walk,					// 飞行慢速
	Ani_Fly_Run,					// 飞行快速

	Ani_Fly_Hurt,					// 飞行被击
	Ani_Fly_Die,					// 飞行死亡

	Ani_Gather,						// 采药

	Ani_OpenBox,					// 开箱

	Ani_Fly_Wait,					// 飞行站立

	Ani_FirstRun,					// 第一次走

	Ani_Idle0,                      //待机1
	Ani_Idle1,                      //待机2
	Ani_Idle2,                      //待机3
	Ani_Idle3,                      //待机4

	Ani_Gather_Stand,               //采药站立
	Ani_OpenBox_Stand,              //开箱站立
	Ani_Num,
};
#endif 

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ani : 描述
 * \return 返回值的描述
 */
inline bool IsMoveAni(AniTypeEnum ani)
{
	return ani == Ani_Walk || ani == Ani_Run || ani == Ani_Ride_Run || ani == Ani_Ride_Walk;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ani : 描述
 * \return 返回值的描述
 */
inline bool IsAttackAni(AniTypeEnum ani)
{
	return ani == Ani_Attack || ani == Ani_Attack2 || ani == Ani_Attack3 || ani == Ani_Ride_Attack ;
}
inline bool IsMagicAni(AniTypeEnum ani)
{
	return ani == Ani_Magic || ani == Ani_Magic2 || ani == Ani_Magic3 || ani == Ani_Ride_Magic;
}
inline bool IsBowAni(AniTypeEnum ani)
{
	return ani == Ani_Bow || ani == Ani_Bow2 || ani == Ani_Ride_Bow ;
}
inline bool IsWorkAni(AniTypeEnum ani)
{
	return ani == Ani_Gather;
}
inline bool IsAnyAttackAni(AniTypeEnum ani)
{
	return IsAttackAni(ani) || IsMagicAni(ani) || IsBowAni(ani) || IsWorkAni(ani);
}

const AniTypeEnum c_nAni2AniStand[][2] =
{
	{Ani_Attack,Ani_Attack_Stand},
	{Ani_Attack2,Ani_Attack2_Stand},
	{Ani_Attack3,Ani_Attack3_Stand},

	{Ani_Magic,Ani_Magic_Stand},
	{Ani_Magic2,Ani_Magic2_Stand},
	{Ani_Magic3,Ani_Magic3_Stand},

	{Ani_Bow,Ani_Bow_Stand},
	{Ani_Bow2,Ani_Bow2_Stand},

	{Ani_Ride_Attack,Ani_Ride_Attack_Stand},
	{Ani_Ride_Magic,Ani_Ride_Magic_Stand},
};


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ani : 描述
 * \return 返回值的描述
 */
inline AniTypeEnum AniAttackToAniStand(AniTypeEnum ani)
{
	for(size_t i=0;i<sizeof(c_nAni2AniStand)/sizeof(c_nAni2AniStand[0]);++i)
	{
		if(ani == c_nAni2AniStand[i][0]) return c_nAni2AniStand[i][1];
	}
	return Ani_Null;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ani : 描述
 * \return 返回值的描述
 */
inline AniTypeEnum AniStandToAniAttack(AniTypeEnum ani)
{
	for(size_t i=0;i<sizeof(c_nAni2AniStand)/sizeof(c_nAni2AniStand[0]);++i)
	{
		if(ani == c_nAni2AniStand[i][1]) return c_nAni2AniStand[i][0];
	}
	return Ani_Null;
}

// ==================================================================================
// CAnimationData
// ==================================================================================
struct stAnimationDataStruct{
	enum AnimationTypeEnum
	{
		Animation_Null = -1,
		Animation_Stop,
		Animation_Loop,
		Animation_OverLifeOver,		//动画结束生命结束
		Animation_OverLifeOver2,	//动画结束保持在最后一帧一段时间生命结束 动画淡出
		Animation_Stop0,			//动画直接静止在第一帧，
		Animation_WaitLoop,
		Animation_NoDisplay,		//动画结束不显示
		Animation_Back,				//播到最后一帧，再返回来
	};
	enum MirrorEnum
	{
		Mirror_Null,
		Mirror_Normal,
		Mirror_AddFrame,//当左手右手都做动作时，手脚的顺序不能错
		Mirror_AutoDir,
	};
	enum BlendEnum
	{
		Blend_Null,
		Blend_Add,
		Blend_Add2,
		Blend_AddAlpha,
		Blend_AddAlpha2,
		Blend_Bright,
		Blend_IgnoreTexelColor,
		Blend_ColorAdd,
		Blend_AddAlpha3,
		Blend_AlphaSub,
		Blend_Monochrome,
	};
	enum AniPlayOrderEnum{
		AniPlayOrder_Asc,
		AniPlayOrder_Desc
	};

	char				strAnimation[MAX_MAGICNAME];	//名字
	size_t				nGroup;							//动画组 在 nGroupCount 大于 1时 nGroup 为最小编号
	size_t				nGroupCount;					//动画组的个数 在同一文件夹下面 （一个动作可能对应几组动画）
	size_t				nDir;							//动画方向
	AnimationTypeEnum	eType;							//播放模式
	AniPlayOrderEnum	eOrder;							//播放 顺序 或 倒序
	int					nTimeLoop;						//Animation_OverLifeOver2 模式下才有用 一个循环周期的时间 控制渐隐的速度
	MirrorEnum			eMirror;						//Mirror_AutoDir 有用到 其他都默认是Mirror_Normal
	int					nAnispeed;						// 每帧多少毫秒
	BlendEnum			eBlend;							//混合模式
};
// ==================================================================================
// CAnimationMagicData
// ==================================================================================
struct stAnimationMagicDataStruct : public stAnimationDataStruct
{
	DWORD dwColor;
	int	  nSortAddY;
};
// ==================================================================================
// CAnimationCharacterData
// ==================================================================================
struct stAnimationCharacterDataStruct : public stAnimationDataStruct
{
	int nHeight;			//显示文字的位置
	int nMagicAdjust;		//魔法攻击时作调整
};
// ==================================================================================
// CAnimationInfo
// ==================================================================================
struct stAnimationStruct : public stAnimationDataStruct
{
	DWORD dwColor;
	int nHeight;			//显示文字的位置
	int nMagicAdjust;		//魔法攻击时作调整
	int	  nSortAddY;
	
	stAnimationStruct()
	{
		strcpy(strAnimation,"null");
		nGroup = 0;
		nDir = 1;
		eType = Animation_Loop;
		nTimeLoop = 0;
		eMirror = Mirror_Normal;
		nAnispeed = 100;
		eBlend = Blend_Null;
		
		nHeight = 0;
		nMagicAdjust = 0;
		dwColor = 0xffffffff;
		nAnispeed = 0;
	}
};
// ==================================================================================
// stCharacterDataStruct
// ==================================================================================
struct stCharacterDataStruct
{
	enum EffectEnum
	{
		Character_Effect_Null,
		Character_Image,		//残象
		Character_Trans,		//人物半透明
		Character_FadeOut,
		Character_FadeIn,
		Character_Jump,			//人物瞬间移动
		Character_NoMove,		//人物定格
		Character_Move,			//人物可以行走
	};
	AniTypeEnum eAnimation;
	AniTypeEnum eAnimation2;
	AniTypeEnum eAnimation3;
	EffectEnum eBeginEffect1;
	EffectEnum eBeginEffect2;
	EffectEnum eEndEffect1;
	EffectEnum eEndEffect2;
	char szHitMagic1[MAX_MAGICNAME];	//命中目标后目标的魔法
	char szHitMagic2[MAX_MAGICNAME];	
	enum MagicEffectEnum
	{
		Magic_Effect_Null,
		Magic_Effect_Image,		//残像
		Magic_Effect_FadeOut,
		Magic_Effect_FadeIn,
	};
	MagicEffectEnum eMagicEffect;
};

// ==================================================================================
// stCharacterStruct
// ==================================================================================
struct stCharacterStruct : public stCharacterDataStruct
{
	stCharacterStruct()
	{
		eAnimation = Ani_Magic;
		eBeginEffect1 = Character_Effect_Null;
		eEndEffect1 = Character_Effect_Null;
		eBeginEffect2 = Character_Effect_Null;
		eEndEffect2 = Character_Effect_Null;
		eMagicEffect = Magic_Effect_Null;
		szHitMagic1[0] = 0;
		szHitMagic2[0] = 0;
	}
};

// ==================================================================================
// stTime
// ==================================================================================
struct stTime 
{
	float fTime;	// 毫秒
	float fSpeed;
	stTime()
	{
		fTime = 0.f;
		fSpeed = 100.0f;
		bRun = true;
	}
	bool Loop(float time)
	{
		fTime += time;
		if (fTime > fSpeed)
		{
			fTime = 0;
			return true;
		}
		return false;
	}
	bool bRun;
	void Clear()
	{
		bRun = false;
	}
	bool IsRun()
	{
		return bRun;
	}
};

// ==================================================================================
// CAnimationData
// ==================================================================================
struct stKeepImageControl : public stTime
{
	int nAlphaDec;
	stKeepImageControl();
};

// ==================================================================================
// CAnimationData
// ==================================================================================
struct stFade : public stTime
{
	int nAlpha;	//>0 fadein <0 fadeout
	int nStart,nEnd;
	stFade(bool bOut = true)
	{
		if (bOut)
		{
			nAlpha = -5;
			nStart = 255;
			nEnd = 0;
		}
		else
		{
			nAlpha = 5;
			nStart = 0;
			nEnd = 255;
		}
		fSpeed = 25.f;
	}
};

// ==================================================================================
// CAnimationData
// ==================================================================================
//magic_ring,cast,bow 可以被选择
enum MagicTypeEnum
{
	Magic_Null = -1,
	Magic_Attack,	//一般魔法
	Magic_Ring,		//光环魔法, 只能开启一个
	Magic_Attach,	//附身的魔法，例如治疗术
	Magic_Effect,	//动作的光效，例如刀光
	Magic_Fly,		//飞行魔法，例如火球
	Magic_Result,	//魔法结果
	Magic_Cast,		//施法的魔法
	Magic_Bow,		//弓箭的魔法
	Magic_Temp,		//中间过程
	Magic_Special,	//特殊魔法，升级
	Magic_Attack_Ring,	//光环攻击魔法, 只能开启一个
};
// ==================================================================================
// CAnimationData
// ==================================================================================
enum MagicAttackEnum
{
	Magic_Attack_Null,				//无法用于攻击
	Magic_AttackMe,					//自身使用，例如光环
	Magic_AttackCharacter,			//使用在其他character上，例如火球
	Magic_AttackCharacterThenMe,	//没有点中人的话，用在自己身上，例如治疗术
	Magic_AttackAny,				//在任意点使用，例如冰风暴
};
// ==================================================================================
// CAnimationData
// ==================================================================================
enum MagicUseEnum
{
	Magic_Use_Null,					//无法使用
	Magic_UsePassive,				//主动持续使用，	剑法和魔法箭或者火球术
	Magic_UseOnlyOnce,				//只有一个有效，	例如光环
	Magic_UseOnce,					//点一下使用一次，	例如冰风暴	
	Magic_NoUse,					//被动技能			
};
// ==================================================================================
// CAnimationData
// ==================================================================================
enum MagicWeaponEnum
{
	Magic_Weapon_Null,
	Magic_Weapon_Sword,
	Magic_Weapon_Bow,
	Magic_Weapon_Staff,
};

// ==================================================================================
// CAnimationData
// ==================================================================================
//关于魔法的信息。
struct stMagicDataStruct
{
	MagicTypeEnum	eType;
	//攻击对象
	MagicAttackEnum eAttack;
	//使用方法
	MagicUseEnum	eUsge;
	//需要兵器类型，可以复合
	MagicWeaponEnum eWeaponType;
	//魔法距离
	int nMagicRange;
	//消耗魔法
	int nMagicCost;
};
// ==================================================================================
// CAnimationData
// ==================================================================================
struct stMagicStruct : public stMagicDataStruct
{
	stMagicStruct()
	{
		eType = Magic_Attack;
		eAttack = Magic_Attack_Null;
		eUsge = Magic_Use_Null;
		nMagicRange = 0;
		eWeaponType = Magic_Weapon_Null;
		nMagicCost = 0;
	}
};

EXPORT_API int GetDir8(int x, int y);
EXPORT_API int GetDir16(int x, int y);
