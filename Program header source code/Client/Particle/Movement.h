/**
 *\file		Movement.h
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:19:00 中国标准时间
 *\brief	粒子运动控制器
 *
 * 
 *
*/

#ifndef	PARTICLE_SYS_MOVEMENT__
#define PARTICLE_SYS_MOVEMENT__

#pragma  once

//Here we refer to particle sys movement in local space
//you can also move particle system in game space as you interest

//Note:
//all move type is supposed surround center pos(0,0,0)

// particle moving type
enum PARTICLE_MOVE_TYPE
{
	PARTICLE_MT_CIRCLE = 0,
	PARTICLE_MT_LINE,
	PARTICLE_MT_LINE_BACK,
	PARTICLE_MT_WAVE,
	PARTICLE_MT_NUM,
};

//Movement base class
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CParticleSysMove
{
protected:
	///移动速度
	float m_moveSpeed;
public:
	///运动类型
	PARTICLE_MOVE_TYPE	m_type;
	
	CParticleSysMove(float speed = 20.0f);
	~CParticleSysMove();
	
	/**
	* \brief 设置移动速度
	* 
	* 
	* 
	* \param speed: 速度
	* \return 
	*/
	void SetMoveSpeed(float speed) { m_moveSpeed = speed; }
	/**
	* \brief 获取移动速度
	* 
	* 
	* 
	* \return 移动速度
	*/
	float GetMoveSpeed() { return m_moveSpeed; }
};

//Circle move
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CMovementCircle : public CParticleSysMove
{
	///半径
	stPointF	m_radio;	//move range
	///方向
	bool		m_dir;		//true:CW		false:CCW
public:
	CMovementCircle(float radio = 200, bool dir = true);	//This time we just make union radio
	~CMovementCircle();
	
	/**
	* \brief 获取半径
	* 
	* 
	* 
	* \return 
	*/
	stPointF GetRadio() { return m_radio; }

	/**
	* \brief 是否顺时针运动
	* 
	* 
	* 
	* \return 
	*/
	bool	IsCW() { return m_dir; }
	
	/**
	* \brief 设置半径
	* 
	* 
	* 
	* \param angle: 半径
	* \return 
	*/
	void SetRadio( float radio ) { m_radio.x = radio; m_radio.y = radio; }
	/**
	* \brief 设置方向
	* 
	* 
	* 
	* \param bCW: 方向
	* \return 
	*/
	void SetDirection( bool bCW ) { m_dir = bCW; }
};

//one dir line move
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CMovementLine : public CParticleSysMove
{
protected:
	//角度
    float	m_angle;	//0~360 in radian( 0:horizontal right ) ;
public:
	CMovementLine(float angle = 0);
	~CMovementLine();
	
	/**
	* \brief 获取角度
	* 
	* 
	* 
	* \return 
	*/
	float GetAngle() { return m_angle; }
	
	/**
	* \brief 设置角度
	* 
	* 
	* 
	* \param angle: 角度
	* \return 
	*/
	void SetAngle( float angle ) { m_angle = angle / 180.0f * D3DX_PI; }
};

//line forward-back move
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CMovementLineBack : public CMovementLine
{
	///宽度
	float m_width;		//move range
public:
	CMovementLineBack(float angle = 0, float width = 400);
	~CMovementLineBack();
	
	/**
	* \brief 获取宽度
	* 
	* 
	* 
	* \return 
	*/
	float GetWidth() { return m_width; }
	
	/**
	* \brief 设置宽度
	* 
	* 
	* 
	* \param width: 宽度
	* \return 
	*/
	void SetWidth( float width ) { m_width = width; }
};

//wave move
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CMovementWave : public CParticleSysMove
{
	///高度
	float m_Height;		//wave height(half)
	///波长
	float m_Lamda;		//wave length
	///缩放率
	float m_scaleRate;		//wave reduce(<1.0f) or add(>1.0f) rate
	///斜角
	float	m_slopAngle;	//wave grow dir( 0~360 in radian) :horizontal-right for 0 
public:
	CMovementWave(float height = 100, float lamda = 200, float angle = 0, float scale = 0.0f);
	~CMovementWave();
	
	/**
	* \brief 获取高度
	* 
	* 
	* 
	* \return 高度
	*/
	float GetHeight() { return m_Height; }
	/**
	* \brief 获取波长
	* 
	* 
	* 
	* \return 
	*/
	float GetWaveLength() { return m_Lamda; }
	/**
	* \brief 获取缩放比率
	* 
	* 
	* 
	* \return 
	*/
	float GetScaleRate() { return m_scaleRate; }
	/**
	* \brief 获取倾斜角
	* 
	* 
	* 
	* \return 
	*/
	float GetSlopAngle() { return m_slopAngle; }
	

	/**
	* \brief 设置高度
	* 
	* 
	* 
	* \param height: 高度
	* \return 
	*/
	void SetHeight( float height ) { m_Height = height; }
	/**
	* \brief 设置波长
	* 
	* 
	* 
	* \param lamda: 波长
	* \return 
	*/
	void SetWaveLength( float lamda ) { m_Lamda = lamda; }
	/**
	* \brief 设置缩放率
	* 
	* 
	* 
	* \param rate: 缩放率
	* \return 
	*/
	void SetScaleRate( float rate ) { m_scaleRate = rate; }
	/**
	* \brief 设置倾斜角
	* 
	* 
	* 
	* \param angle: 倾斜角
	* \return 
	*/
	void SetSlopAngle( float angle ) { m_slopAngle = angle / 180.0f * D3DX_PI; }
};

#endif