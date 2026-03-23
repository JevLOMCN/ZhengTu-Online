#pragma once

#include "iObj.h"
#include <vector>
#include <mmsystem.h>


class iMagic;
class cMagic;
class iAnimation;
class MagicProperty;
class iCharacter;
class iLight;


#ifndef radian2angle
#define radian2angle(X) ( (X) * 180.f/ PI )
#endif

#ifndef angle2radian
#define angle2radian(X) ( (X) * PI / 180.f )
#endif

const float c_fAngleA = 30.0f*PI/180.0f;
// 坐标的 y 是向下的

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
inline float PtLength(const stPointF& pt)
{
	return sqrtf(pt.x * pt.x + pt.y * pt.y);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param alpha : 描述
 * \return 返回值的描述
 */
inline float standAngle(float alpha)
{
	if(alpha < 0) 
	{
		float f = -alpha;
		int i = f/(PI*2.0f);
		alpha = PI*2.0f - (f - i*PI*2.0f);
	}

	if(alpha > PI*2.0f) 
	{
		float f = alpha;
		int i = f/(PI*2.0f);
		alpha = f - i*PI*2.0f;
	}
	return alpha;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
// 角度是顺时针的
inline float PtToAngle(const stPointF& pt)
{
	float len = PtLength(pt);
	if(len == 0.0f) return 0;
	float alpha = acosf(pt.x/len);
	if(pt.y < 0)
	{
		alpha = -alpha;
	}
	return standAngle(alpha);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \param alpha : 描述
 * \return 返回值的描述
 */
// 旋转,顺时针
inline stPointF PtRotate(const stPointF & pt,float alpha)
{
	float len = PtLength(pt);
	alpha = standAngle(PtToAngle(pt) + standAngle(alpha));
	stPointF pt1(len * cos(alpha),len * sin(alpha));
	return pt1;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
// z 为0的
inline stPointF Pt3DTo2D(const stPointF & pt)
{
	stPointF pt1(pt.x,pt.y * sinf(c_fAngleA));
	return pt1;
}

/**
 * \brief z不为0的地面上的点投影到屏幕上
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
inline stPointF Pt3DTo2D(const stPoint3F &pt)
{
	stPointF pt1(pt.x,pt.y * sinf(c_fAngleA) + pt.z * cosf(c_fAngleA));
	return pt1;
}

/**
 * \brief 地面上的点绕z轴旋转alpha后，再投影到屏幕上
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \param alpha : 描述
 * \return 返回值的描述
 */
inline stPointF Pt3DTo2D(const stPoint3F &pt,float alpha)
{
	alpha = standAngle(alpha + PtToAngle(*(stPointF*)&pt));
	float len = PtLength(*(stPointF*)&pt);
	stPoint3F pt1(len * cosf(alpha),len * sinf(alpha),pt.z);
	return Pt3DTo2D(pt1);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
inline stPoint3F Pt2DTo3D(const stPointF & pt)
{
	stPoint3F pt1(pt.x,pt.y / sinf(c_fAngleA),0.0f);
	return pt1;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
inline stPointF Pt2DTo3D_2(const stPointF & pt)
{
	stPointF pt1(pt.x,pt.y / sinf(c_fAngleA));
	return pt1;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
inline stPointF PtToRightPt(const stPointF& pt)
{
	float len = PtLength(pt);
	stPointF pt1(len,0);
	return pt1;
}



struct stBlockDataStruct 
{
	enum BlockTypeEnum
	{
		Block_Null,
		Block_Character = 0x01,
		Block_Obstacle = 0x02,
		Block_ObstacleANDCharacter = Block_Character|Block_Obstacle,
		Block_Self = 0x04,			//有益魔法
		Block_Firend = 0x05,		//有益魔法
		Block_AllCharacter = 0x06,	//攻击所有物体
		Block_Target = 0x07,		//只攻击该目标
	};
	//碰撞障碍以后必然是die
	enum BlockCharacterResult	//碰撞人物以后
	{
		Block_Result_Null,
		Block_Die,			//消亡
		Block_Stop,			//停止
		Block_Continue,		//继续
		Block_NoBlockAgain,	//继续
	};
	enum BlockGeometryEnum
	{
		Block_Geometry_Null,
		Block_Grid,			//根据格子碰撞
		Block_Circle,
		Block_Line,
		Block_Image,		//图像碰撞
	};
	BlockTypeEnum eBlock;
	BlockCharacterResult eBlockResult;
	BlockGeometryEnum eBlockGeometry;
	POINT ptCenter;
	int fRadius;
};

struct stBlockStruct : public stBlockDataStruct
{
	bool IsBreak(int e) {return (eBlock&e) != 0;};
	bool bBlock;
	stBlockStruct()
	{
		eBlock = Block_Null; 
		eBlockResult = Block_Result_Null;
		eBlockGeometry = Block_Geometry_Null;
		ptCenter.x = 0;ptCenter.y = 0;
		fRadius = 0;
		bBlock = true;
	}
	bool IsBreak(iCharacter* p,POINT ptMagic);
};

struct stMoveDataStruct
{
	enum FollowTypeEnum		//如何跟随
	{
		Move_Follow_Null,		//不跟随
		Move_Follow_Send,		//跟随发射人
		Move_Follow_Des,		//跟随目的人
		Move_Follow_Block,		//跟随被碰撞的人物
		Move_Follow_Magic,		//跟随魔法		
		Move_Follow_Des_Fly,	//跟随目的人飞行
		Move_Follow_Block_Fly,	//跟随被碰撞的人物飞行
		Move_Follow_Magic_Fly,	//跟随魔法飞行		
	};
	enum MoveTypeEnum	//运动类型
	{
		Move_Null,
		Move_Static,
		Move_LineDes,		//直线运动到目的点	炎浪术
		Move_Line,			//直线运动			火球术
		Move_Drop,			//从空中掉下		雷电术
		Move_LineCircle,	//冰封球
		Move_LineFollow,	//跟踪				火球术？
		Move_DropFollow,	//空中下落跟踪
		Move_DropHere,		//从当前点掉下		
		Move_DropStop,		//在空中静止
		Move_Circle,		//旋转
		Move_Up,			//往天上飞
		Move_Parabola,		//抛物线
		Move_MorePower,     //快速移动
	};
	enum MovePosEnum
	{
		Move_Pos_Null,
		Move_Des,
		Move_Pos,		//当前点
		Move_Start,
		Move_SendCharacter,
		Move_TargetCharacter,
		Move_PosTile,
		Move_StartTile,
		Move_DesTile,
		Move_SendCharacter_8_rand_1,  // 周围８格随机一个
	};
	enum SortEnum
	{
		Sort_Null,
		Sort_Lowest,
		Sort_Foot,
		Sort_Back,
		Sort_Front,
		Sort_Frontest,
		Sort_Circle,
		Sort_AboveAll,
	};
	MovePosEnum ePos,eDes;
	FollowTypeEnum eFollow;
	MoveTypeEnum eMove;
	int bSetLife;
	int nLife;
	int nSpeed;			 // 每秒多少像素
	float fAcceleration; // 每秒的速度增量
	SortEnum eSort;
	char strParam1[MAX_MAGICNAME];// y的初始速度 ; 直线向下的角度
	char strParam2[MAX_MAGICNAME];// y的距离
	char strParam3[MAX_MAGICNAME];// 图像偏移X
	char strParam4[MAX_MAGICNAME];// 图像偏移Y
};

struct stMoveStruct : public stMoveDataStruct
{
	//是否可以使用setlife来改变...
	//float fSpeed; //pixel 
	//float nLife;
	bool IsDie(){return nLife <= 0;}
	void SetLife(long f)
	{
		if(nLife == 0)
		{
			int iiiiii=0;
		}
		nLife = f;
	}
	void SetLifeOut(int f)
	{
		if (bSetLife && f>0)
			SetLife(f);
	}
	int GetLife(){return nLife;}
	int GetSpeed(){return nSpeed;}
	float GetCurSpeed();

	stMoveStruct(){
		eFollow = Move_Follow_Null;
		bSetLife = false;
		nLife = 0.0f; 
		nSpeed = 0.f; 
		fAcceleration = 0.f; 
		eMove = Move_Static; 
		ePos = Move_Pos_Null;
		eDes = Move_Pos_Null;
		xPos = 0; yPos = 0;
		ptStart.x = 0; ptStart.y = 0;
		ptDes.x = 0; ptDes.y = 0;
		xSpeed = 0; ySpeed = 0;
		xAcceleration = 0; yAcceleration = 0;
	}
	//冰封球
	struct stCircleStruct
	{
		float fAlpha;
		float fRadius;
		float xCenter,yCenter;
		void Init(){fAlpha = 0.f;fRadius = 8.f; xCenter = 0; yCenter = 0;}
	};
	
	// 抛物线
	struct stParabolaStruct{
		float x;	// 位置
		float y;	
		//float xDis;	// 距离(像素)
		//float yDis;
		float vx0;	// 速度(像素/S)
		float vy0;
		float ay;		// y的加速度
		float total_t;	// 总的时间(S)
		float alpha;
		float magicOffX;
		float magicOffZ;
		void Init(float i_xDis,float i_yDis,float i_vx,float i_vy)
		{
			float xDis = i_xDis;
			float yDis = -i_yDis;
			vx0	 = i_vx;
			vy0  = i_vy;
			total_t = xDis/vx0;
			ay = 2.0f * (yDis - vy0 * total_t) / (total_t*total_t);
		}

		void Run(float time)
		{
			y = vy0 * time + ay * time*time/2.0f;
			x = vx0 * time;
		}
	};

	struct stMoveMorePower
	{
		iCharacter* pSender;
	};

	union{
		stParabolaStruct stParabola;
		stCircleStruct stCircle;
		stMoveMorePower stMorePower;
	};
	
	POINT ptStart,ptDes;
	float xPos,yPos;
	float xSpeed,ySpeed;
	DWORD dwTime;
	void InitParabola();
	void RunParabola();

	void SetSpeed(float x,float y){xSpeed = x;ySpeed = y;}
	void GetSpeed(float& x,float& y){x = xSpeed; y = ySpeed;}
	float xAcceleration,yAcceleration;
	//起始点
	//void Fly(POINT ptS,POINT ptD,POINT ptPos,iAnimation* m_pAnimation);	
	void Fly(const stFly& fly,int iStartSpeed = -1);	
	void FlyTo(POINT ptD,int iStartSpeed = -1);

	void GetFly(POINT& ptS,POINT& ptD);	
	void GetPos(POINT& pt){pt.x = (int)xPos;pt.y = (int)yPos;};
	//对于飞行魔法不要使用
	void SetPos(POINT pt){xPos = (float)pt.x;yPos = (float)pt.y;};
	POINT GetPos(){POINT pt;GetPos(pt);return pt;}
	
	int Move(long time_eslasped,iCharacter* pDes);
	int OnBreak(stBlockStruct::BlockTypeEnum n,stBlockStruct::BlockCharacterResult e);
	POINT GetDir();

	
	float GetTangent(){return PI/2 + stCircle.fAlpha;}
	void SetAlphaRadius(float a,float r){stCircle.fAlpha = a;stCircle.fRadius = r;}
	SortEnum GetSort(){return eSort;}

	POINT GetOvergroundPos();
	float GetRotationAngle()
	{
		float fAngle;
		fAngle = atan(float(ptDes.y-ptStart.y)/float(ptDes.x-ptStart.x));
		if(ptDes.x < ptStart.x)
		{
			return fAngle + PI;
		}
		return fAngle;
	}

};

const int MAX_GENERATE = 4;

struct stGenerateDataStructA
{
	enum GenerateGeometryEnum
	{
		Generate_Geometry_Null,
		Generate_Ring,					//生成一个环
		Generate_Line,					//生成一条线
		Generate_Point,					//生成一个点
		Generate_22,					//生成2*2区域
		Generate_33,					//生成3*3区域
		Generate_Tangent,				//沿着切线生成
		Generate_12,					//生成1*2区域 from start point to end point
		Generate_13,					//生成1*3区域
		Generate_14,					//生成1*4区域
		Generate_15,					//生成1*5区域
		Generate_FollowPoint,			//生成跟随点 火球术的火花
		Generate_Dir_1,					//生成方向一格
		Generate_Round,					//生成圆  生成周围8格
		Generate_Round_Half,			//生成半圆 生成周围4格
		Generate_FullScreenCharacter,	//全屏幕攻击
		Generate_FullScreenDirCharacter,//全屏幕攻击
		Generate_RandomPoint,			//生成一个随机点，冰风暴
		Generate_PointUnCircle,			//逆时针生成一格
		Generate_PointCircle,			//顺时针生成一格
		Generate_Dir_2_UnCircle,		//逆时针2格生成

		Generate_55,					//生成5*5区域
		Generate_Fan,                   //扇形
		Generate_MulTimeStamp,          //间隔时间(可以用一个点(0,0)代替)

		Generate_MulParallel,           // n平行

		Generate_OnCircle_2,			//生成环上两个
		Generate_OnCircle_3,			//生成环上三个

	};
	enum GenerateEventEnum
	{
		Generate_Event_Null,
		Generate_LifePassed ,		//生命剩下一段时间生成
		Generate_LifePass	,		//生命过了一段时间生成
		Generate_LifeStart,			//生命开始生成
		Generate_LifeEnd,			//生命结束生成
		Generate_LifeStartEnd,		//生命开始结束生成
		Generate_LifePassEach ,		//生命每过一段时间生成
		Generate_LifeRandom,		//随机生成
		Generate_Circle,			//旋转一周生成一次
		Generate_LifeRandomEach,	//随机持续生成
		Generate_LifePassEachDis,	//每过距离生成		
		Generate_LifePassRandomEach,//每过时间随机生成
		Generate_Life_MulTimeStamp, //间隔时间
	};
	enum GenerateSpeedEnum
	{
		Generate_Speed_Null = 0,	// 不高速速度
		Generate_Speed_Current,		//当前速度
	};
	char strGenerateType[MAX_MAGICNAME];
	GenerateGeometryEnum eGenerateGeometry;
	GenerateEventEnum eGenerateEvent;
	int nGenerateTime;
	GenerateSpeedEnum eGenerateSpeed;
	char szGenerateData[MAX_MAGICNAME]; 
};

struct stGenerateStructA : public stGenerateDataStructA
{
	int nGenerated;
	int nInitLife;//init life;
	POINT ptInitPos;
	int nTimer;
	std::vector<int> vecTimer;

	stGenerateStructA(){
		string_null(strGenerateType);
		szGenerateData[0] = 0;
		eGenerateGeometry = Generate_Geometry_Null; 
		eGenerateEvent = Generate_Event_Null;
		nGenerateTime = 0;
		nGenerated = 0;
		nInitLife = 0;
		ptInitPos.x = 0;
		ptInitPos.y = 0;
		nTimer= 0;
		vecTimer.clear();
	}
	void Generate(cMagic* pFather,tagListMagicGenerate& listMagic);
	void GenerateByLife(cMagic* pFather,int nLife,tagListMagicGenerate& listMagic);
};

struct stGenerateDataStruct
{
	//生成一个Shadow物体(运动属性一致)在一开始
	char strGenerateShadow[MAX_MAGICNAME];
	//碰撞的时候产生什么样的物体
	char strGenerateBlockCharacter[MAX_MAGICNAME];
	//碰撞的时候产生什么样的物体
	char strGenerateBlockObstacle[MAX_MAGICNAME];
	//stGenerateDataStructA aGenerate[MAX_GENERATE];
};

struct stGenerateStruct : public stGenerateDataStruct
{
	stGenerateStruct(){
		string_null(strGenerateBlockCharacter);
		string_null(strGenerateBlockObstacle);
		string_null(strGenerateShadow);
	}
	iMagic* GenerateByBlock(cMagic* pFather,int n);
};
 
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CSceneSound;
struct stMusicDataStruct
{
	enum MusicEventEnum
	{
		Music_Event_Null,
		Music_LifePassed,		//生命剩下一段时间生成
		Music_LifePass,			//生命过了一段时间生成
		Music_LifeStart,		//生命结束生成
		Music_LifeEnd,			//生命开始生成
		//		Music_LifeStartEnd,		//生命开始结束生成
		//		Music_LifePassEach ,	//生命每过一段时间生成
	};
	int idMusic;
	char szMusic[MAX_MAGICNAME];
	MusicEventEnum eMusicEvent;
	int nMusicTime;
	int bMusicLoop;
};

struct stMusicStruct : public stMusicDataStruct
{
	DWORD run_time;
	bool bMusic;
	stMusicStruct()
	{
		bMusic = false;
		idMusic = 0;
		string_null(szMusic);
		eMusicEvent = Music_Event_Null;
		nMusicTime = 0;
		bMusicLoop = false;		
		run_time = 0;
	}
	int Run(CSceneSound* pSound, POINT ptPos,POINT velocity,int nInitLife, int nLife, size_t nMagicType);
};

struct stLightDataStruct
{
	enum {MAX_LIGHT_FRAME = 32};
	LightTypeEnum eLight;
	LightEffectEnum eEffect;
	int xOffset,yOffset;
	size_t nColor;
	DWORD aColor[MAX_LIGHT_FRAME]; // 动画每一帧时的颜色
	size_t nRadius;
	int aRadius[MAX_LIGHT_FRAME];  // 动画每一帧时的半径
};

struct stLightStruct : public stLightDataStruct
{
	stLightStruct()
	{
		xOffset = 0;
		yOffset = 0;
		eLight = Light_Point;
		eEffect = Light_Effect_Null;
		nColor = 1;
		nRadius = 1;
		memset(aRadius,0,sizeof(aRadius));
		memset(aColor,0,sizeof(aColor));
	}
	DWORD GetColor(size_t nFrame,size_t nFrameCount) const {
		if(nColor ==0 || nFrameCount == 0) return 0;
		if(nFrame >= nFrameCount) return 0;
		return aColor[nFrame * nColor/nFrameCount];
	}
	int GetRadius(size_t nFrame,size_t nFrameCount) const {
		if(nRadius == 0 || nFrameCount == 0) return 0;
		if(nFrame >= nFrameCount) return 0;
		return aRadius[nFrame * nRadius / nFrameCount];	
	}
	POINT GetOffset() const {POINT pt; pt.x = xOffset; pt.y = yOffset; return pt;}
};

//use it for load and save...
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class MagicDataProperty : public iProperty
{
public:
	stMagicDataStruct stMagic;
	stMoveDataStruct stMove;
	stBlockDataStruct stBlock;
	stGenerateDataStruct stGenerate;
	stGenerateDataStructA aGenerate[MAX_GENERATE];
	stCharacterDataStruct stCharacter;
	stAnimationMagicDataStruct stAnimation;
	stMusicDataStruct stMusic;
	stLightDataStruct stLight;
	stEventData aEvent[MAX_EVENT];
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class MagicProperty : public iProperty
{
public:
	stMagicStruct stMagic;
	stMoveStruct stMove;
	stBlockStruct stBlock;
	stGenerateStruct stGenerate;
	stGenerateStructA aGenerate[MAX_GENERATE];
	stCharacterStruct stCharacter;
	stAnimationStruct stAnimation;
	stMusicStruct stMusic;
	stLightStruct stLight;
	stEvent aEvent[MAX_EVENT];
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class iMagic : public iObj
{
public:
	virtual bool GetDrawRect(POINT pt,RECT & rc) = 0;
	virtual bool CreateMagic(bool bLight,LPCSTR szFile = "") = 0;
	//设置图片号码.必须在Create以前调用，否则无效
	//virtual SetAnimationID(LPCSTR szFile) = 0;

	//起始点
	virtual POINT GetStartPos() = 0;
	virtual void GetFly(POINT& ptS,POINT& ptD) = 0;
	virtual void Fly(const stFly& st,int iStartSpeed = -1) = 0;

	virtual void RemoveSubMagic()=0;
	virtual iCharacter* GetSender() = 0;
	virtual iCharacter* GetTarget() = 0;
	virtual bool CanAttackTarget() = 0;

	virtual int GetMoveType() = 0;
	//virtual bool IsFollowType() = 0;
	virtual int GetFollowType() = 0;
	virtual bool IsFollowSender() = 0;
	//virtual stMoveStruct::MovePosEnum GetDesType() = 0;
	//life like time...
	virtual void SetLife(int nLife) = 0;	

	virtual void SetInitLife() = 0;
	//设置第几重的life,例如火墙生成4个火墙生成器，设置火墙的生命期没有意义。
	//例如困魔咒实际的时间是第三层决定的，setlife(3,...)
	//virtual void SetLife(int depth, int nLife) = 0;	
	virtual MagicProperty* GetProperty()=0;
	//会碰到墙壁等东西
	virtual bool IsBreak(stBlockStruct::BlockTypeEnum e) = 0;
	virtual bool IsBreak(iCharacter* p) = 0;
	virtual int OnBreak(stBlockStruct::BlockTypeEnum n,tagListMagicGenerate& listGenerate,iCharacter* pCharacter) = 0;

	//运动,只在客户端使用
	virtual bool Generate(tagListMagicGenerate& listGenerate) = 0;

	virtual bool IsDie() = 0;

	//一般性的参数
	virtual void SetParam(const MagicProperty& param) = 0;

	virtual void SetType(size_t lType) = 0;
	virtual size_t GetType() = 0;
	virtual void SetTable(size_t lTable) = 0;
	virtual size_t GetTable() = 0;
	virtual size_t GetMaxDir() = 0;
	virtual size_t GetDir() = 0;
	virtual void TurnTo(size_t dir) = 0;

	virtual float GetSpeed() = 0;
	virtual void SetSpeedAndTurn(float fx,float fy) = 0;
	virtual void GetSpeed(float& fx,float& fy) = 0;

	//light
//	virtual void EnableLight(bool bLight) = 0;
	virtual iLight* GetLight() = 0;
	virtual bool AddLight(POINT ptScreen) = 0;
	virtual bool AddLightAtScreen(POINT ptScreen) = 0;

	virtual void SetCenter(POINT ptCenter) = 0;

	virtual POINT GetDrawPosition() = 0;
	virtual stMoveStruct::SortEnum GetSort() = 0;

	//是由于碰撞产生的么
	virtual bool IsBlockGenerate() = 0;
	virtual void SetBlockGenerate(bool b) = 0;

	virtual stAnimationStruct& GetAnimationStruct() = 0;
	virtual stCharacterStruct& GetCharacterStruct() = 0;

	//绘制某种排序某个方向上的魔法
	virtual bool DrawSort(POINT ptScreen, POINT ptMagic,stMoveStruct::SortEnum e, int dir) = 0;
	virtual bool DrawSort2(POINT ptScreen, POINT ptMagic,stMoveStruct::SortEnum e, int dir) = 0;

	virtual std::list<stEvent>& GetEventList() = 0;
	virtual void ClearEventList() = 0;

	virtual MagicTypeEnum GetMagicType() = 0;
	virtual MagicAttackEnum GetAttackType() = 0;

	virtual POINT GetDefaultCenter() = 0;

	virtual void SetSender(iCharacter* pSender) = 0;
	virtual void SetAlphaRadius(float a,float r) = 0;

	virtual bool IsHasChild() = 0;
	virtual bool IsHasFollow() = 0;
#ifdef _GAME_TOOL
	virtual size_t GetDirFrame() = 0;
	virtual size_t GetFrameOfDir() = 0;
#endif 
};

enum RangeEnum
{
	Range_Null,
	Range_Here,			//1原地
	Range_Dir,			//2该方向
	Range_Dir1,			//3该方向1格
	Range_Dir2,			//4该方向2格
	Range_Dir3,			//5该方向3格
	Range_Dir4,			//6该方向4格
	Range_Dir5,			//7该方向5格
	Range_HalfCircle,	//8半圆4格	
	Range_Circle,		//9园8格
	Range_33,			//10中心9格
	Range_Cross,		//11十字
	Range_FullScreen,	//12全屏
	Range_FullScreenDir,//13某个方向的全屏
	Range_CrossFlower,	//14玄冰咒
	Range_Upright3,	// 与该方向垂直的3格
	Range_Upright5,	// 与该方向垂直的5格
	Range_Fan3,		// 扇形3格
};


class iCharacter;
/*
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class iMagicCallBack
{
public:
	//得到全屏幕的攻击点
	virtual POINT GetMapOffsetPoint()=0;
	virtual void SetMapOffsetPoint(const POINT& pt)=0;
	virtual void GetCharacter(RangeEnum eRange, std::vector<iCharacter*>& list, POINT ptStart, size_t dir, iCharacter* pExcept = NULL) = 0;
	virtual void MessageBox(const char* szText)=0;
	};
	iMagicCallBack* GetMagicCallBack();
	void Magic_SetCallBack(iMagicCallBack* p);

//*/

iMagic* Magic_CreateMagic(LPCSTR szMagic,size_t type2Magic = 0);
int Magic_GetMagicID(LPCSTR szMagic,size_t type2Magic = 0);
iMagic* Magic_CreateMagic(size_t typeMagic,size_t type2Magic = 0);
int Magic_InitModule();
