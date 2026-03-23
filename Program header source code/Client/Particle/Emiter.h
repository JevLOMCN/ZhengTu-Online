/**
 *\file		Emiter.h
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:19:00 中国标准时间
 *\brief	粒子发射器
 *
 * 
 *
*/

#ifndef PARTICLE_EMITER__
#define PARTICLE_EMITER__

#pragma  once

//Particle emiter type
enum PARTICLE_EMITER_TYPE
{
	PARTICLE_ET_POINT = 0,
	PARTICLE_ET_LINE,
	PARTICLE_ET_CIRCLE,
	PARTICLE_ET_SPHERE,
	PARTICLE_ET_PLANE,
	PARTICLE_ET_POINT_LIST,
	PARTICLE_ET_NUM,
};

//particle emiter base class
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CParticleEmiter
{
	///粒子发射器位置
	stPointF m_pos;		//Pos in local space
public:
	~CParticleEmiter() { ReleaseAll(); }
	void ReleaseAll() {};
	///粒子发射器类型
	PARTICLE_EMITER_TYPE	m_type;
	
	/**
	* \brief 设置发射器位置
	* 
	* 
	* 
	* \param  pos: 位置
	* \return 
	*/
	void SetLocalPosition(stPointF pos) { m_pos = pos; }
	/**
	* \brief 获取发射器位置
	* 
	* 
	* 
	* \return 发射器位置
	*/
	stPointF GetLocalPosition() { return m_pos; }
};

//Point emiter
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CEmiterPoint : public CParticleEmiter
{
	//particle emiter angle range
	//which determine velocity directional vector
	//in degree		0~360
	//After set value we convert degree to radians
	///起始角度
	float	m_fStartAngle;
	///终止角度
	float	m_fEndAngle;

public:
	CEmiterPoint(float startAngle = 0, float endAngle = 360);
	~CEmiterPoint() { ReleaseAll(); }
	void ReleaseAll() {};
	
	/**
	* \brief 获取起始角度
	* 
	* 
	* 
	* \return 起始角度
	*/
	float GetStartAngle();
	/**
	* \brief 获取终止角度
	* 
	* 
	* 
	* \return 终止角度
	*/
	float GetEndAngle();
	
	/**
	* \brief 设置起始角度
	* 
	* 
	* 
	* \param angle: 角度
	* \return 
	*/
	void SetStartAngle( float angle ) { m_fStartAngle = angle / 180.0f * D3DX_PI; }
	/**
	* \brief 设置终止角度
	* 
	* 
	* 
	* \param angle: 角度
	* \return 
	*/
	void SetEndAngle( float angle ) { m_fEndAngle = angle / 180.0f * D3DX_PI; }
};

//Line emiter
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CEmiterLine : public CParticleEmiter
{
	//width
	///宽度
	float m_width;
	
	//rotate angle
	//in degree 0~180
	//CW
	//horizontal = 0
	///角度
	float	m_angle;		//after set value we convert degree to radians
	//direction
	// 0 :from two end the same effect as two point emiter
	// 1: from one side
	// 2: from both side
	// 3: from one side inverse dir for two half
	///方向
	int m_direction;	

public:
	CEmiterLine(float width = 100, float angle = 0, int dir = 1);
	~CEmiterLine() { ReleaseAll(); }
	void ReleaseAll() {};
	/**
	* \brief 获取宽度
	* 
	* 
	* 
	* \return 宽度
	*/
	float GetWidth() { return m_width; }

	/**
	* \brief 获取角度
	* 
	* 角度
	* 
	* \return 倾斜角度
	*/
	float GetAngleDegree() { return m_angle / D3DX_PI * 180.0f; }
	/**
	* \brief 获取倾斜角度
	* 
	* 弧度
	* 
	* \return 倾斜角度
	*/
	float GetAngle() { return m_angle; }
	/**
	* \brief 获取方向
	* 
	* 
	* 
	* \return 发射方向
	*/
	int GetEmiterDir() { return m_direction; }
	
	/**
	* \brief 设置宽度
	* 
	* 
	* 
	* \param width: 宽度
	* \return 
	*/
	void SetWidth( float width ) { m_width = width; }
	/**
	* \brief 设置倾斜角度
	* 
	* 
	* 
	* \param angle: 角度
	* \return 
	*/
	void SetAngleDegree( float angle ) { m_angle = angle / 180.0f * D3DX_PI; }
	/**
	* \brief 设置方向
	* 
	* 
	* 
	* \param dir: 方向
	* \return 
	*/
	void SetDirection( int dir ) { m_direction = dir; }
};

//Circle emiter
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CEmiterCircle : public CParticleEmiter
{
protected:
	///半径
	float m_radio;
	//direction
	// 0: outside
	// 1: inside
	// 2: both side
	///方向
	int m_direction;

public:
	CEmiterCircle(float radio = 200.0f, int dir = 0);
	~CEmiterCircle() { ReleaseAll(); }
	void ReleaseAll() {};
	
	/**
	* \brief 获取半径
	* 
	* 
	* 
	* \return 发射半径
	*/
	float GetRadio() { return m_radio; }
	/**
	* \brief 获取方向
	* 
	* 
	* 
	* \return 发射方向
	*/
	int GetEmiterDir() { return m_direction; }
	
	/**
	* \brief 设置半径
	* 
	* 
	* 
	* \param radio: 半径
	* \return 
	*/
	void SetRadio( float radio ) { m_radio = radio; }
	/**
	* \brief 设置方向
	* 
	* 
	* 
	* \param dir: 方向
	* \return 
	*/
	void SetDirection( int dir ) { m_direction = dir; }
};

//Sphere emiter
//This emiter in 2D just the same as circle emiter
//We set this just for extend in future
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CEmiterSphere : public CEmiterCircle
{
public:
	CEmiterSphere(float radio = 200.0f, int dir = 0);
	~CEmiterSphere() { ReleaseAll(); }
};

//Plane emiter
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CEmiterPlane : public CParticleEmiter
{
	//width
	///宽度
	float m_width;
	//height
	///高度
	float m_height;
public:
	CEmiterPlane(float width = 100.0f, float height = 100.0f);
	~CEmiterPlane() { ReleaseAll(); }
	void ReleaseAll() {};
	
	/**
	* \brief 获取宽度
	* 
	* 
	* 
	* \return 宽度
	*/
	float GetWidth() { return m_width; }
	/**
	* \brief 获取高度
	* 
	* 
	* 
	* \return 高度
	*/
	float GetHeight() { return m_height; }
	
	/**
	* \brief 设置宽度
	* 
	* 
	* 
	* \param width: 宽度
	* \return 
	*/
	void SetWidth( float width ) { m_width = width; }
	/**
	* \brief 设置高度
	* 
	* 
	* 
	* \param height: 高度
	* \return 
	*/
	void SetHeight( float height ) { m_height = height; }
};

class CEmiterPointList : public CParticleEmiter
{
	//point list
	int m_size;
	stPointI* m_ptList;

	stPointI m_ptCenter;
	stRectI m_rcBoundingBox;
public:
	CEmiterPointList(void);
	~CEmiterPointList() { ReleaseAll(); }
	void ReleaseAll();
	bool AllocateMem(int size);
	void Init(stPointI* ptList, int size);
	void InitFromString(const String& pStr);

	//Method
	stPointI* GetPointList() { return m_ptList; }
	int GetSize() { return m_size; }
	stPointI RandomGetOnePoint();
	stPointI HScaneGetOnePoint();
	stRectI GetBoundingRect() { return m_rcBoundingBox; }
	stPointI GetCenter() { return m_ptCenter; }
};


#endif