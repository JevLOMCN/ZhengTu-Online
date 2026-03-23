/**
 *\file		ParticleSystem.h
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:21:00 中国标准时间
 *\brief	粒子系统
 *
 * 
 *
*/

// Our particle system
// This system has function of a emitter
// All particles in this system has the same texture & same move type & any other change style

//**************
// Key notes
// Al the angle member we use here
//		When you set you use degree as factor
//		When you get you got radian
//**************

#ifndef __CPARTICLESYSTEM_H_
#define __CPARTICLESYSTEM_H_

#pragma once

#include "./Emiter.h"
#include "./Movement.h"
#include "./Particle.h"

// RANDOM_NUM returns a float in the range 0.0f - 1.0f
#define RANDOM_NUM      (((FLOAT)rand()-(FLOAT)rand())/RAND_MAX)

// RANDOM_VECTOR returns a D3DVECTOR whose components (x,y,Z) are between 0.0f and 1.0f 
// remember that this is not a normalized vector.
#define RANDOM_VECTOR   D3DXVECTOR3(RANDOM_NUM,RANDOM_NUM,RANDOM_NUM)

// max number of particles our ParticleSystem can support
#define MAX_PARTICLES	1024
#define PARTICLE_BUFFER_SIZE	256
#define	BUFFER_FLUSH_SIZE		64

#define PARTICLE_TEXTURE_SIZE	64

// some defines for our ParticleSystem.   This makes it easy to change settings
// by using defines and putting them all in one place
#define MIN_SPEED		0.0f	// in world units / sec
#define MIN_LIFETIME	0.0f	// in seconds
#define MIN_SPREAD		0.0f	// in degrees
#define MIN_EMISSION	1.0f	// in particles / sec
#define MIN_SIZE		0.5f	// in world units
#define MIN_GRAVITY		-10.0f	// as a multiple of normal gravity 
#define MIN_ALPHA		0.0f	// as a ratio 

#define MAX_SPEED		1000.0f	// in world units / sec
#define MAX_LIFETIME	20.0f	// in seconds
#define MAX_EMISSION    1000.0f	// in particles / sec
#define MAX_SIZE		100.0f	// in world units
#define MAX_GRAVITY		10.0f	// as a multiple of normal gravity 
#define MAX_ALPHA		1.0f	// as a ratio 

#define MAX_TEXTURES			37
#define BLEND_TYPE_NUM			17

//Spark about
#define MAX_SPARK_NUM	10		
#define SPARK_LIFE		1.0f	//You can make it changeable

enum enumOtherForceDir
{
	OTHER_FD_LEFT,
	OTHER_FD_RIGHT,
	OTHER_FD_RANDOM,
	OTHER_FD_NUM
};

//// particle moving type
//enum PARTICLE_MOVE_TYPE
//{
//	PARTICLE_MT_CIRCLE = 0,
//	PARTICLE_MT_LINE,
//	PARTICLE_MT_LINE_BACK,
//	PARTICLE_MT_WAVE,
//	PARTICLE_MT_NUM,
//};

////Particle emiter type
//enum PARTICLE_EMITER_TYPE
//{
//	PARTICLE_ET_POINT = 0,
//	PARTICLE_ET_LINE,
//	PARTICLE_ET_CIRCLE,
//	PARTICLE_ET_SPHERE,
//	PARTICLE_ET_PLANE,
//	PARTICLE_ET_NUM,
//};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param f : 描述
 * \return 返回值的描述
 */
// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  x = (x<min  ? min : x<max ? x : max);
// Helper function to stuff a FLOAT into a DWORD argument
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

//Particle texture array
//soke 新加一些特效文件
static char* particleTexName[] = {	"Particle000_bubble", 
"Particle001_circle",			"Particle002_cloud",
"Particle003_flare",			"Particle004_spark",
"Particle005_FireSprite",		"Particle006_BlowRed0",
"Particle007_BlowRed1",			"Particle008_BlowRed2",
"Particle009_BlueDirt",			"Particle010_BlueFlare",
"Particle011_Cloud",			"Particle012_Corona0",
"Particle013_Corona1",			"Particle014_Ring",
"Particle015_BigFlare",			"Particle016_RainDrop",
"Particle017_Fire1",			"Particle018_Fire2",
"Particle019_Fire3",			"Particle020_FireGray",
"Particle021_FountainBlack",	"Particle022_FountainBlue",
"Particle023_Grass",			"Particle024_Green",
"Particle025_Marble",			"Particle026_NebulaSprite",
"Particle027_Rain",				"Particle028_Snow",
"Particle029_Dot0",				"Particle030_Dot1",
"Particle031_Dot2",				"Particle032_Dot3",
"Particle033_ParticleBlue1",	"Particle034_ParticleYellow",
"Particle035_RedFlare",			"Particle036_Star",
"particle037_corona2",			"particle038_corona3",
"particle040_quake",			"particle041_bluelover",
"particle041_bluerose9and99",	"particle041_money",
"particle042_bluerose999",		"yanhua0001",
"yanhua0005",			        "yanhua0008",
"yujinxiang9",			        "yujinxiang99" };
class CCustomTexture;
class CParticleEmiter;

// Our ParticleSystem class
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CParticleSystem
{
	friend class CParticleSysManager;

public:
	CParticleSystem(CParticleEmiter* emiter, CParticleSysMove* movement);
	~CParticleSystem();

	bool InitDevice();
	void UnInitDevice();

	void DeleteAllParticles();

	//Set system position in game space
	void SetPositionInGameSpace( const D3DXVECTOR3& vGameSpace );
	//Get system position in game space
	D3DXVECTOR3& GetPositionInGameSpace();

	// Update the system and all particles with the given time delta
	HRESULT		Update(float fTimeDelta, stPointF offset);
	// Render the system
	HRESULT		Render(LPDIRECT3DDEVICE8 lpd3dDevice);
	HRESULT		RenderUseEngineMethod(LPDIRECT3DDEVICE8 lpd3dDevice);
	// Could be used to move the system in game space
	void		MoveTo(D3DXVECTOR3 vLocation);

	// if the system is moving, attractive 
	bool		IsMoving();
	bool		IsAttractive();

	bool IsPlaying() { return m_bPlaying; }

	//
	bool		IsRenderWithManager() { return m_bRenderWithManager; }

	//
	void		SetRenderWithManager(bool bWithManager = true) { m_bRenderWithManager = bWithManager; }

	//
	void		SetLoopPlayNum( int num = 0 );
	int			GetLoopPlayNum();
	
	/**
	* \brief 设置运动状态
	* 
	* 
	* 
	* \param bMoving: 是否运动
	* \return 
	*/
	void		SetMoving( bool bMoving ) { m_bIsMoving = bMoving; }

	//Set attract location
	void		SetAttractionLocation(D3DXVECTOR3& vAttraction);
	void		EnableAttraction( bool bAttract ) { m_bIsAttractive = bAttract; }
	void		SetAttractStrength( int strength ) { m_AttractStrength = strength; }

	// find out where the system is in world space
	void		GetLocation(D3DXVECTOR3& vLocation);
	// Get attract location
	void		GetAttractLocation(D3DXVECTOR3& vAttractLocation);
	// Get attract strength
	int			GetAttractStrength();

	//Get emiter about
	/**
	* \brief 获取发射器
	* 
	* 
	* 
	* \return 发射器
	*/
	CParticleEmiter*	GetEmiter() { return m_emiter; }
	int			GetEmiterType() { return m_emiter->m_type; }
	bool		SetNewEmiter( int type );
	void		ReleaseEmiter();

	//Get movement about
	/**
	* \brief 获取运动控制器
	* 
	* 
	* 
	* \return 运动控制器
	*/
	CParticleSysMove*	GetMovement() { return m_movement; }
	int			GetMovementType();
	bool		SetNewMovement( int type );

	// how many particles are active ?
	unsigned int NumParticles();

	//Get sys life about
	/**
	* \brief 获取系统生命
	* 
	* 
	* 
	* \return 系统生命
	*/
	float		GetSysLife() { return m_sysLife; }
	/**
	* \brief 获取系统年龄
	* 
	* 
	* 
	* \return 系统年龄
	*/
	float		GetEslapedTime() { return m_elaspedTime; }

	// Need Spark about
	/**
	* \brief 设置Spark效果
	* 
	* 
	* 
	* \param bNeed: 粒子死亡时是否产生Spark
	* \return 
	*/
	void		SetSpark( bool bNeed ) { m_bNeedSpark = bNeed; }
	/**
	* \brief 获取是否产生Spark
	* 
	* 
	* 
	* \return 是则返回true, 否则返回false
	*/
	bool		IsNeedSpark() { return m_bNeedSpark; }

	//particle play about
	/**
	* \brief 设置可见性
	* 
	* 
	* 
	* \param bVisible: 系统是否可见
	* \return 
	*/
	void		SetVisible( bool bVisible = true ) { m_bVisible = bVisible; }
	void		Init();
	void		Play(const float& pSysLife = -1);
	void		RePlay();
	void		Stop() { m_bPlaying = false; m_bVisible = false; }
	void		Pause() { m_bPlaying = false; }
	void		SetStartTime( float startTime = 0.0f ) { m_fStartTime = startTime; }
	float		GetStartTime() { return m_fStartTime; }

	//Add bomb effect
	enum enmBombDir
	{
		BOMB_DIR_OUTSIDE,
		BOMB_DIR_INSIDE,
		BOMB_DIR_RAND
	};
	
	struct stBombData
	{
		float		startTime;		//second
		enmBombDir	dir;
		int			strength;
		int			life;		//ms
	};
	stBombData m_bomb;
	bool m_bBombState;

	void		Bomb(const enmBombDir& dir, int strength = 10, int lastTime = 3000, bool bToDie = true);
	void		UpdateBomb();
	int			GetBombStrength();
	int			GetBombLeftLife();	//ms
	bool		IsBombState() { return m_bBombState; }
	void		ResetBombStartTime( float fTime ) { m_bomb.startTime = fTime; }

    	
	/**
	* \brief 获取系统名字
	* 
	* 
	* 
	* \return 系统名字
	*/
	char*		GetName() { return m_name; }
	
	//Live range
	/**
	* \brief 获取有效绘制区域
	* 
	* 
	* 
	* \return 有效绘制区域
	*/
	stRectI&	GetLiveRange() { return m_liveRange; }
	/**
	* \brief 设置是否启用有效绘制区域
	* 
	* 
	* 
	* \param bUse: 是否启用有效绘制区域
	* \return 
	*/
	void		SetUseOutLiveRange( bool bUse = true ) { m_bUseOutLiveRange = bUse; }
	/**
	* \brief 获取是否启用有效绘制区域
	* 
	* 
	* 
	* \return 是则返回true, 否则返回false
	*/
	bool		GetUseOutLiveRange() { return m_bUseOutLiveRange; }

	//other force
	float		GetOtherForceStrength( bool bOriginal = false );
	/**
	* \brief 获取其他外力的方向
	* 
	* 
	* 
	* \return 其他外力的方向
	*/
	int			GetOtherForceDir() { return m_OtherForceDir; }

	//Sys rotate speed
	/**
	* \brief 设置系统旋转速度
	* 
	* 
	* 
	* \param speed: 系统旋转速度
	* \return 
	*/
	void		SetSysRotateSpeed( float speed ) { m_sysRotSpeed = speed * D3DX_PI / 180.0f; }
	/**
	* \brief 获取系统旋转速度
	* 
	* 
	* 
	* \return 系统旋转速度
	*/
	float		GetSysRotateSpeed() { return  m_sysRotSpeed; }

	//Texture num
	void		SetTextureNum( int num );
	/**
	* \brief 获取系统纹理数
	* 
	* 
	* 
	* \return 系统纹理数目
	*/
	int			GetTextureNum() { return m_texNum; }

	//Get temp id
	DWORD		GetThisID() { return m_dwThisID; }
	void		SetThisID(const DWORD& id) { m_dwThisID = id; }

	void		SetChild(const char* name) { strcpy(m_childName, name); }
	char*		GetChildName() { return m_childName; }
	bool		IsHasChild() { return m_childName[0] != 0; }

private:
	//This id
	DWORD	m_dwThisID;
	/// Weather among die out
	bool m_bRePlay;
	/// if system loop play
	bool m_bLoopPlay;

	/// if render in PSM(Particle System Manager)
	bool m_bRenderWithManager;

	/// Loop play num
	int	m_loopPlayNum;

	// Particle draw buffer size
	// This time we use the same for all particle system
	// Just as what we defined at the beginning
	///粒子系统最大设备尺寸
	unsigned int	m_bufferSize;		//max buffer size of our particle system
	unsigned int	m_flushSize;		//a flush block used as a small buffer to draw and write
	unsigned int	m_dwBase;			//current start pos of buffer
										//should be set to bufferSize at beginning

	// pointer back to main application in case we need to reference something
	///应用程序指针(暂时没用)
	void*	m_pApplication;

	// An internal ID for us to remember which texture the system is using
	///纹理id
	unsigned int	m_uTextureID;

	// Particles Per Second to emit.
	///粒子发射速率
	unsigned int	m_uParticlesPerSec;

	// Particles currently active
	///存活粒子数
	unsigned int	m_uParticlesAlive;		//current particle system num of particles

	// Last known location of System ( used for interpolation ).
	///系统上次位置
	D3DXVECTOR3		m_d3dvPrevLocation;
	// current location of system in local space
	///系统当前位置
	D3DXVECTOR3		m_d3dvLocation;
	// attract location
	///吸附器位置
	bool			m_bUseAtractLocation;
	///吸附强度
	int				m_AttractStrength;
	///吸附方向
	D3DXVECTOR3		m_d3dvAttraction;

	// current velocity as a Vector ( cause that's what velocity is )   : )
	D3DXVECTOR3		m_d3dvVelocity;
	// current Direction of System.  Particles are emitted using this vector + some variation
	D3DXVECTOR3		m_d3dvDirection;

	// used as a percentage of normal gravity.   Note: to simulate wind or other forces you could make Gravity a Vector
	// which could have components 
	///起始重力
	float m_fGravityStart;
	///重力分布
	float m_fGravityVar;
	///结束重力
	float m_fGravityEnd;

	//Add and x-direction effect
	///其他外力
	float m_fOtherForce;
	///其他外力方向
	int	m_OtherForceDir;

	// Size of the particles in World units
	///粒子起始尺寸
	float m_fSizeStart;
	///粒子尺寸分布
	float m_fSizeVar;
	///粒子终止尺寸
	float m_fSizeEnd;

	// Translucency of particles. Alpha 0 = invisible, Alpha 1 = full visibility
	///粒子起始透明度
	float m_fAlphaStart;
	///粒子透明度分布
	float m_fAlphaVar;
	///粒子终止透明度
	float m_fAlphaEnd;

	// Color of particles, the color of the particles when they start and end.
	// The color is interpolated linearly between these 2 values over time.
	///粒子起始颜色
	D3DCOLORVALUE	m_d3dcColorStart;
	///粒子颜色分布
	D3DCOLORVALUE	m_d3dcColorVar;
	///粒子终止颜色
	D3DCOLORVALUE	m_d3dcColorEnd;

	// Speed is a scalar and is combined with the normalized direction vector of the system to get a 
	// Velocity vector.
	///粒子速度
	float m_fSpeed;
	///粒子速度分布
	float m_fSpeedVar;		//Make change between each particle

	// Life of the particle in seconds, 
	///粒子寿命
	float m_fLife;
	///粒子寿命分布
	float m_fLifeVar;		//Make change between each particle

	// Picture a vector in space with a start and an end
	// now picture that the vector goes through the center of a Cone such
	// that the tip of the cone is at the start of the vector and the bottom of the cone is at the end of the vector
	// Now picture the angle from the vector out to the edge of the cone.  This is m_fTheta.
	// If it were Zero, the cone would collapse down to the vector and be a line.
	// If it were 90 degrees, then the cone would turn into a hemisphere ( half of a sphere ) 
	// ANd finally if this value were 180 degrees, then the cone would turn into a Sphere

	// this values defines how much randomness the particles have in their directional vector
	///粒子发射角度分布（已不用）
	float m_fTheta;

	// whether or not the system is moving autonomously
	// In local space
	///系统是否移动
	bool m_bIsMoving;		//
	// whether or not the particles are attracted to the system
	///系统是否有吸附力影响
	bool m_bIsAttractive;

	// whether or not we are allowed to emit particles or whether they build up
	// This is very useful when we need to emit a great amount of particles just at the beginning 
	///系统是否处于聚积期
	bool m_bIsSuppressed;

	// wether need sparks when particle is died or collision with some thing
	///粒子死亡时是否产生Spark
	bool m_bNeedSpark;
	///系统聚积起始时间
	float m_fSuppressTimeStart;
	///系统聚积终止时间
	float m_fSuppressTimeEnd;

	// last moment in time that we updated the system
	///系统上次刷新时间
	float m_fTimeLastUpdate;

	// this is a value that allows us to emit very precise amounts of particles.
	// what if we want 0.5 particles per second ?    0.5 is not a full particle ( we don't round up ) so we can't emit one yet
	// so we remember what we wanted to emit and add that next time we try to emit some more particles.
	// Think about a faucet that is leaking very slowly.  The drop slowly gets bigger and bigger till there is enough mass to allow
	// it to break free.
	///剩余发射量
	float m_fEmissionResidue;

	//Particle moving type
	///系统运动类型
	unsigned int m_uMovingType;
	///系统运动控制器
	CParticleSysMove*	m_movement;
	///系统运动速度
	float m_fMovingSpeed;
	//Particle emiter type
	///粒子发射器类型
	unsigned int m_uEmiterType;
	///粒子发射器
	CParticleEmiter* m_emiter;

	//This time we use a dynamically allocation and freeing memory
	///粒子指针
	CParticle*	m_pParticles;
	///粒子指针
	CParticle*	m_pParticlesFree;

	//Texture
	///纹理数目
	int	m_texNum;
	///第一个纹理
	CCustomTexture**			m_pTex;

	// Particle system position in game space
	// That is world space
	///系统位置
	D3DXVECTOR3 m_vSystem;
	///系统在游戏中是否运动
	bool	m_bMoveInGame;

	//Give particle system a name
	///系统名字
	char m_name[256];

	//Child particle system
	char m_childName[32];

	//particle system life
	///系统生命期
	float m_sysLife;		//base on second( <0 means for ever )
	///系统持续的时间
	float m_elaspedTime;	//delta time since play call
	///系统可见的时间
	float m_fStartTime;		//last how much time the sys can be visible since play

	// particle system play about
	///系统是否正在播放
	bool	m_bPlaying;		// wether need update
	///系统是否可见
	bool	m_bVisible;		// visible or hide
	///是否第一次调用播放
	bool	m_bFirstCalledPlay;		//Wether first call play during update

	// particle live range
	// when out of range ,the particle going to die
	///粒子绘制有效区域
	stRectI m_liveRange;
	///是否使用有效区域
	bool	m_bUseOutLiveRange;

	//Texture blend type
	///纹理混合类型
	BYTE m_blendType;

	//sys rotate speed:	degree per second
	///系统旋转速度
	float m_sysRotSpeed;

	//particle size height/width rate
	///粒子尺寸纵横比
	float m_sizeHWRate;

	///Wether or not use offset when main character move
	bool m_bUseCharacterOffset;

	///Whole system scale rate
	float m_sysSizeScale;

public:
	//Set member INTERFACE
	//color
	/**
	* \brief 设置起始红色分量
	* 
	* 
	* 
	* \param red: 红色分量
	* \return 
	*/
	void SetStartColorRed( float red ) { m_d3dcColorStart.r = red; }

	/**
	* \brief 设置起始绿色分量
	* 
	* 
	* 
	* \param green: 绿色分量
	* \return 
	*/
	void SetStartColorGreen( float green ) { m_d3dcColorStart.g = green; }

	/**
	* \brief 设置起始蓝色分量
	* 
	* 
	* 
	* \param blue: 蓝色分量
	* \return 
	*/
	void SetStartColorBlue( float blue ) { m_d3dcColorStart.b = blue; }
	
	/**
	* \brief 设置终止红色分量
	* 
	* 
	* 
	* \param red: 红色分量
	* \return 
	*/
	void SetEndColorRed( float red ) { m_d3dcColorEnd.r = red; }

	/**
	* \brief 设置终止绿色分量
	* 
	* 
	* 
	* \param green: 绿色分量
	* \return 
	*/
	void SetEndColorGreen( float green ) { m_d3dcColorEnd.g = green; }

	/**
	* \brief 设置终止蓝色分量
	* 
	* 
	* 
	* \param blue: 蓝色分量
	* \return 
	*/
	void SetEndColorBlue( float blue ) { m_d3dcColorEnd.b = blue; }
	
	/**
	* \brief 设置红色分量分布
	* 
	* 
	* 
	* \param red: 红色分量分布
	* \return 
	*/
	void SetColorVarRed( float red ) { m_d3dcColorVar.r = red; }

	/**
	* \brief 设置绿色分量分布
	* 
	* 
	* 
	* \param green: 绿色分量
	* \return 
	*/
	void SetColorVarGreen( float green ) { m_d3dcColorVar.g = green; }

	/**
	* \brief 设置蓝色分量分布
	* 
	* 
	* 
	* \param blue: 蓝色分量
	* \return 
	*/
	void SetColorVarBlue( float blue ) { m_d3dcColorVar.b = blue; }
	
	//alpha
	/**
	* \brief 设置起始透明度
	* 
	* 
	* 
	* \param alpha: 起始透明度
	* \return 
	*/
	void SetAlphaBorn( float alpha ) { m_fAlphaStart = alpha; }

	/**
	* \brief 设置终止透明度
	* 
	* 
	* 
	* \param alpha: 终止透明度
	* \return 
	*/
	void SetAlphaEnd( float alpha ) { m_fAlphaEnd = alpha; }

	/**
	* \brief 设置透明度分布
	* 
	* 
	* 
	* \param alpha: 透明度分布
	* \return 
	*/
	void SetAlphaVar( float alpha ) { m_fAlphaVar = alpha; }
	
	//size
	/**
	* \brief 设置起始尺寸
	* 
	* 
	* 
	* \param size: 起始尺寸
	* \return 
	*/
	void SetSizeStart( float size ) { m_fSizeStart = size; }

	/**
	* \brief 设置终止尺寸
	* 
	* 
	* 
	* \param size: 终止尺寸
	* \return 
	*/
	void SetSizeEnd( float size ) { m_fSizeEnd = size; }

	/**
	* \brief 设置尺寸分布
	* 
	* 
	* 
	* \param size: 尺寸分布
	* \return 
	*/
	void SetSizeVar( float size ) { m_fSizeVar = size; }

	//speed
	/**
	* \brief 设置起始速度
	* 
	* 
	* 
	* \param speed: 起始速度
	* \return 
	*/
	void SetSpeedBorn( float speed ) { m_fSpeed = speed; }

	/**
	* \brief 设置速度分布
	* 
	* 
	* 
	* \param speed: 速度分布
	* \return 
	*/
	void SetSpeedVar( float speed ) { m_fSpeedVar = speed; }

	//life
	/**
	* \brief 设置起始生命期
	* 
	* 
	* 
	* \param life: 起始生命期
	* \return 
	*/
	void SetLifeBorn( float life ) { m_fLife = life; }

	/**
	* \brief 设置终止生命期
	* 
	* 
	* 
	* \param life: 终止生命期
	* \return 
	*/
	void SetLifeVar( float life ) { m_fLifeVar = life; }

	//texture
	void SetTexture( int index );

	//emit rate
	/**
	* \brief 设置发射速率
	* 
	* 
	* 
	* \param rate: 发射速率
	* \return 
	*/
	void SetEmitRate( int rate ) { m_uParticlesPerSec = rate; }

	//spark
	/**
	* \brief 设置产生Spark
	* 
	* 
	* 
	* \param bUse: 是否需要Spark
	* \return 
	*/
	void SetUseSpark( bool bUse ) { m_bNeedSpark = bUse; }

	//Emit type
	void SetEmiter( CParticleEmiter* emiter );

	//Emiter move type
	void SetMovement( CParticleSysMove* movement );

	//Emiter move speed
	void SetEmiterMoveSpeed( float speed );

	//Sys life
	/**
	* \brief 设置系统生命期
	* 
	* 
	* 
	* \param sysLife: 系统生命期
	* \return 
	*/
	void SetSysLife( float sysLife = -1 ) { m_sysLife = sysLife; }

	//force affect
	/**
	* \brief 设置起始重力
	* 
	* 
	* 
	* \param start: 起始重力
	* \return 
	*/
	void SetGravityStart( float start ) { m_fGravityStart = start; }

	/**
	* \brief 设置终止重力
	* 
	* 
	* 
	* \param end: 终止重力
	* \return 
	*/
	void SetGravityEnd( float end ) { m_fGravityEnd = end; }

	/**
	* \brief 设置重力分布
	* 
	* 
	* 
	* \param var: 重力分布
	* \return 
	*/
	void SetGravityVar( float var ) { m_fGravityVar = var; }

	//other force
	/**
	* \brief 设置其他力的强度
	* 
	* 
	* 
	* \param strength: 其他力的强度
	* \return 
	*/
	void SetOtherForceStrength( float strength ) { m_fOtherForce = strength; }

	/**
	* \brief 设置其他力的方向
	* 
	* 
	* 
	* \param dir: 其他力的方向
	* \return 
	*/
	void SetOtherForceDir( int dir ) { m_OtherForceDir = dir; }

	//Set live range
	/**
	* \brief 设置绘制有效区域
	* 
	* 
	* 
	* \param range: 绘制有效区域
	* \return 
	*/
	void SetLiveRange( stRectI range ) { m_liveRange = range; }

	//Set blend type
	/**
	* \brief 设置纹理混合类型
	* 
	* 
	* 
	* \param type: 纹理混合类型
	* \return 
	*/
	void SetBlendType( BYTE type ) { m_blendType = type; }

	/**
	* \brief 设置聚积状态
	* 
	* 
	* 
	* \param bSuppressed: 聚积状态
	* \return 
	*/
	void SetSuppressStatus( bool bSuppressed ) { m_bIsSuppressed = bSuppressed; }

	/**
	* \brief 设置聚积起始时间
	* 
	* 
	* 
	* \param startTime: 聚积起始时间
	* \return 
	*/
	void SetSuppressTimeStart( float startTime ) { m_fSuppressTimeStart = startTime; }

	/**
	* \brief 设置聚积终止时间
	* 
	* 
	* 
	* \param endTime: 聚积终止时间
	* \return 
	*/
	void SetSuppressTimeEnd( float endTime ) { m_fSuppressTimeEnd = endTime; }

	//Set size stretch
	/**
	* \brief 设置粒子纵横比
	* 
	* 
	* 
	* \param stretch: 粒子纵横比
	* \return 
	*/
	void SetSizeHWRate( float stretch ) { m_sizeHWRate = stretch; }

	//Set use character offset
	/**
	* \brief 设置是否附加人物移动造成的位置偏移
	* 
	* 
	* 
	* \param bUse: 是否使用 
	* \return 
	*/
	void UseCharacterOffset( bool bUse ) { m_bUseCharacterOffset = bUse; }

	void SetSizeScale( float sysScale, float sizeScale = 1.0f );

	void SetLoopPlay( bool bLoop = true ) { m_bLoopPlay = bLoop; }


	//Get member INTERFACE
	//color
	/**
	* \brief 获取起始颜色红色分量
	* 
	* 
	* 
	* \return 起始颜色红色分量
	*/
	D3DCOLORVALUE GetStartColor() { return m_d3dcColorStart; }

	/**
	* \brief 获取起始颜色绿色分量
	* 
	* 
	* 
	* \return 
	*/
	D3DCOLORVALUE GetEndColor() { return  m_d3dcColorEnd; }

	/**
	* \brief 获取起始颜色蓝色分量
	* 
	* 
	* 
	* \return 
	*/
	D3DCOLORVALUE GetColorVar() { return m_d3dcColorVar; }

	//alpha
	/**
	* \brief 获取起始透明度
	* 
	* 
	* 
	* \return 
	*/
	float GetAlphaBorn() { return m_fAlphaStart; }

	/**
	* \brief 获取终止透明度
	* 
	* 
	* 
	* \return 
	*/
	float GetAlphaEnd() { return m_fAlphaEnd; }

	/**
	* \brief 获取透明度分布
	* 
	* 
	* 
	* \return 
	*/
	float GetAlphaVar() { return m_fAlphaVar; }

	//size
	/**
	* \brief 获取起始尺寸
	* 
	* 
	* 
	* \return 
	*/
	float GetSizeBorn() { return m_fSizeStart; }

	//size
	/**
	* \brief 获取终止尺寸
	* 
	* 
	* 
	* \return 
	*/
	float GetSizeEnd() { return m_fSizeEnd; }

	//size
	/**
	* \brief 获取尺寸分布
	* 
	* 
	* 
	* \return 
	*/
	float GetSizeVar() { return m_fSizeVar; }
	
	//speed
	//size
	/**
	* \brief 获取起始速度
	* 
	* 
	* 
	* \return 
	*/
	float GetSpeedBorn() { return m_fSpeed; }

	//size
	/**
	* \brief 获取速度分布
	* 
	* 
	* 
	* \return 
	*/
	float GetSpeedVar() { return m_fSpeedVar; }

	//life
	//size
	/**
	* \brief 获取起始生命
	* 
	* 
	* 
	* \return 
	*/
	float GetLifeBorn() { return m_fLife; }

	//size
	/**
	* \brief 获取生命分布
	* 
	* 
	* 
	* \return 
	*/
	float GetLifeVar() { return m_fLifeVar; }

	//texture
	//size
	/**
	* \brief 获取纹理id
	* 
	* 
	* 
	* \return 
	*/
	int GetTextureID() { return m_uTextureID; }

	//emit rate
	//size
	/**
	* \brief 获取发射速率
	* 
	* 
	* 
	* \return 
	*/
	int GetEmiterRate() { return m_uParticlesPerSec; }

	//spark
	//size
	/**
	* \brief 获取是否在死亡时产生Spark
	* 
	* 
	* 
	* \return 
	*/
	bool IsNeefSpark() { return m_bNeedSpark; }

	//force affect
	//size
	/**
	* \brief 获取起始重力
	* 
	* 
	* 
	* \return 
	*/
	float GetGravityBorn() { return m_fGravityStart; }

	//size
	/**
	* \brief 获取终止重力
	* 
	* 
	* 
	* \return 
	*/
	float GetGravityEnd() { return m_fGravityEnd; }

	//size
	/**
	* \brief 获取重力分布
	* 
	* 
	* 
	* \return 
	*/
	float GetGravityVar() { return m_fGravityVar; }

	//blend type
	//size
	/**
	* \brief 获取纹理混合类型
	* 
	* 
	* 
	* \return 
	*/
	BYTE GetBlendType() { return m_blendType; }

	//size
	/**
	* \brief 获取聚积状态
	* 
	* 
	* 
	* \return 
	*/
	bool GetSuppressStatus() { return m_bIsSuppressed; }

	//size
	/**
	* \brief 获取聚积起始时间
	* 
	* 
	* 
	* \return 
	*/
	float GetSuppressTimeStart() { return m_fSuppressTimeStart; }

	//size
	/**
	* \brief 获取聚积终止时间
	* 
	* 
	* 
	* \return 
	*/
	float GetSuppressTimeEnd() { return m_fSuppressTimeEnd; }

	//size stretch
	//size
	/**
	* \brief 获取粒子纵横比
	* 
	* 
	* 
	* \return 
	*/
	float GetSizeHWRate() { return m_sizeHWRate; }

	//Get use character offset
	/**
	* \brief 获取是否附加人物移动造成的位置偏移
	* 
	* 
	* 
	* \return 
	*/
	bool NeedUseCharacterOffset() { return m_bUseCharacterOffset; }

	//Get sys size scale rate
	/**
	* \brief 设置系统缩放大小
	* 
	* 
	* 
	* \return 
	*/
	float GetSizeScale() { return m_sysSizeScale; }

	bool IsLoopPlay() { return m_bLoopPlay; }

};


#endif