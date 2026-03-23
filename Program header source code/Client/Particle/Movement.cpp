/**
 *\file		Movement.cpp
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:19:00 中国标准时间
 *\brief	粒子运动控制器
 *
 * 
 *
*/

#include "public.h"
#include "./Movement.h"

/**
 * \brief 构造函数
 * 
 * 
 * 
 * \param  : 速度
 * \return 
 */
//base movement class
CParticleSysMove::CParticleSysMove( float speed /* = 20.0f */ )
{
	FUNCTION_BEGIN;

	m_moveSpeed = speed;

	m_type = PARTICLE_MT_NUM;

	FUNCTION_END;
}

/**
 * \brief 析构函数
 * 
 * 
 * 
 * \return 
 */
CParticleSysMove::~CParticleSysMove()
{
	FUNCTION_BEGIN;



	FUNCTION_END;
}

/**
 * \brief 构造函数
 * 
 * 
 * 
 * \param  : 半径
 * \param  : 方向
 * \return 
 */
//circle move
CMovementCircle::CMovementCircle( float radio /* = 200 */, bool dir /* = true */ )
{
	FUNCTION_BEGIN;

	m_type = PARTICLE_MT_CIRCLE;

	m_radio.x = radio;
	m_radio.y = radio;

	m_dir = dir;

	FUNCTION_END;
}

/**
 * \brief 析构函数
 * 
 * 
 * 
 * \return 
 */
CMovementCircle::~CMovementCircle()
{
	FUNCTION_BEGIN;



	FUNCTION_END;
}

/**
 * \brief 构造函数
 * 
 * 
 * 
 * \param  : 角度
 * \return 
 */
//line move
CMovementLine::CMovementLine( float angle /* = 0 */ )
{
	FUNCTION_BEGIN;

	m_type = PARTICLE_MT_LINE;
	
	while ( angle >= 360.0f )
		angle -= 360.0f;
	while ( angle < 0.0f )
		angle += 360.0f;

	m_angle = angle / 180.0f * D3DX_PI;

	FUNCTION_END;
}

/**
 * \brief 析构函数
 * 
 * 
 * 
 * \return 
 */
CMovementLine::~CMovementLine()
{
	FUNCTION_BEGIN;



	FUNCTION_END;
}

/**
 * \brief 构造函数
 * 
 * 
 * 
 * \param  : 角度
 * \param  : 宽度
 * \return 
 */
//line back-forward move
CMovementLineBack::CMovementLineBack( float angle /* = 0 */, float width /* = 400 */ )
{
	FUNCTION_BEGIN;

	m_type = PARTICLE_MT_LINE_BACK;
	
	while ( angle >= 360.0f )
		angle -= 360.0f;
	while ( angle < 0.0f )
		angle += 360.0f;
	m_angle = angle / 180.0f * D3DX_PI;

	m_width = width;

	FUNCTION_END;
}

/**
 * \brief 析构函数
 * 
 * 
 * 
 * \return 
 */
CMovementLineBack::~CMovementLineBack()
{
	FUNCTION_BEGIN;



	FUNCTION_END;
}

/**
 * \brief 构造函数
 * 
 * 
 * 
 * \param  : 高度
 * \param  : 波长
 * \param  : 角度
 * \param  : 缩放率
 * \return 
 */
//wave move
CMovementWave::CMovementWave( float height /* = 100 */, float lamda /* = 200 */, float angle /* = 0 */, float scale /* = 0.0f */ )
{
	FUNCTION_BEGIN;

	m_type = PARTICLE_MT_WAVE;

	m_Height = height;
	m_Lamda = lamda;

	while ( angle >= 360.0f )
		angle -= 360.0f;
	while ( angle < 0.0f )
		angle += 360.0f;
	m_slopAngle = angle / 180.0f * D3DX_PI;

	m_scaleRate = scale;

	FUNCTION_END;
}



