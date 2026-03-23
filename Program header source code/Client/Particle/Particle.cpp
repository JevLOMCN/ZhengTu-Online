/**
 *\file		Particle.cpp
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:20:00 中国标准时间
 *\brief	粒子
 *
 * 
 *
*/

#include "public.h"

#include "Particle.h"
#include "ParticleSystem.h"

#define GRAVITY D3DXVECTOR3(0.0f, -9.8f, 0.0f)

/**
 * \brief 构造函数
 * 
 * 
 * 
 * \return 
 */
//	Construction
CParticle::CParticle()
{
	FUNCTION_BEGIN;

	m_fAge = 0.0f;
	m_fSize = 1.0f;
	m_d3dcColor.r = 1.0f;
	m_d3dcColor.g = 0.0f;
	m_d3dcColor.b = 0.0f;
	m_d3dcColor.a = 0.5f;

	m_bSpark = false;
	m_sparkNum = 4;

	m_pParent = NULL;
	m_pNext = NULL;

	m_offset.x = 0.0f;
	m_offset.y = 0.0f;

	FUNCTION_END;
}


/**
 * \brief 析构函数
 * 
 * 
 * 
 * \return 
 */
//	Destruction
CParticle::~CParticle()
{
	FUNCTION_BEGIN;

	//We release each particle by his or her parent


	FUNCTION_END;
}

/**
 * \brief 是否在有效绘制区域
 * 
 * 
 * 
 * \return 是则返回true, 否则返回false
 */
bool CParticle::IsInLiveRange()
{
	FUNCTION_BEGIN;

	//This time we don't consider this
	//return true;

	//This time just consider 2D
	if ( m_d3dvLocation.x < m_pParent->GetLiveRange().left )
		return false;
	if ( m_d3dvLocation.x > m_pParent->GetLiveRange().right )
		return false;

	//if ( m_d3dvLocation.y < m_pParent->GetLiveRange().top )
	//	return false;
	if ( m_d3dvLocation.y > m_pParent->GetLiveRange().bottom )
		return false;

	return true;

	FUNCTION_END;
}


/**
 * \brief 刷新
 * 
 * 
 * 
 * \param fTimeDelta : 持续时间
 * \param offset : 位置偏移
 * \return 粒子死亡返回false, 否则返回true
 */
//-----------------------------------------------------------------------------
//	Update particle each frame
//	fTimeDelta : the time passed between two frame
//  If the particle dies in the update, false is returned//
//-----------------------------------------------------------------------------
bool CParticle::Update( float fTimeDelta, stPointF offset )
{
	FUNCTION_BEGIN;


	//If die return false
	if ( (m_fAge + fTimeDelta >= m_fLifetime) )
	{
		m_fAge = -1.0f;
		return false;
	}
	else	//update
	{
		//add offset
		m_offset += offset;

		// everything ages so add the time elapsed
		m_fAge += fTimeDelta;

		// Our current location will be our previous location next frame
		m_d3dvPrevLocation = m_d3dvLocation;

		// this moves the particle using the last known velocity and the time that has passed
		//s = v * t		so sample : )
		m_d3dvLocation += m_d3dvVelocity * fTimeDelta;

		//This time we do not consider collision between particle and other object or among particles

		// Gravity could also be used as other force such as wind
		// We call it Gravity just for reason this time we use it as gravity.
		// v = v0 + a * t		Still remember this : )	
		m_d3dvVelocity += ( GRAVITY * m_fGravity * fTimeDelta );

		//add other force effect
		m_d3dvVelocity += D3DXVECTOR3( m_pParent->GetOtherForceStrength() * fTimeDelta, 0.0f, 0.0f );
		if ( m_pParent->GetOtherForceDir() == OTHER_FD_RANDOM )
		{
			if ( RANDOM_NUM * 100 < -90 )
				m_d3dvVelocity.x = 0;
		}

		// if this particle's parent system supports Attraction
		// This time we just support point attraction
		// To say : we attract all particles to his or her parent location. Just like a black hole
		if ( NULL != m_pParent && m_pParent->IsAttractive() )
		{

			D3DXVECTOR3 AttractLocation;

			// Get attract location
			// This may be at parent location or at use pointed special location
			// Use this you may get some funny effect
			m_pParent->GetAttractLocation( AttractLocation );

			// calculate the vector between the particle and the attracter
			D3DXVECTOR3 AttractDir = AttractLocation - m_d3dvLocation; 

			// You can turn off attraction for certain axes
			AttractDir.z = 0;

			// Add attract velocity
			D3DXVec3Normalize( &AttractDir, &AttractDir );
			m_d3dvVelocity += AttractDir * ((float)m_pParent->GetAttractStrength()) * fTimeDelta;
		}


		//Add bomb effect
		if ( NULL != m_pParent && m_pParent->IsBombState() )
		{
			D3DXVECTOR3 BombDir = m_d3dvLocation;
			//D3DXVec3Normalize( &BombDir, &m_d3dvLocation );
			m_d3dvVelocity += BombDir * (float)m_pParent->GetBombStrength() * fTimeDelta;
		}

		// adjust current Color from calculated Deltas and time elapsed.
		m_d3dcColor.r += m_d3dcColorDelta.r * fTimeDelta;
		m_d3dcColor.g += m_d3dcColorDelta.g * fTimeDelta;
		m_d3dcColor.b += m_d3dcColorDelta.b * fTimeDelta;

		// adjust current Alpha
		m_fAlpha += m_fAlphaDelta * fTimeDelta;
		// adjust current Size
		m_fSize += m_fSizeDelta * fTimeDelta;
		// adjust current Gravity 
		m_fGravity += m_fGravityDelta * fTimeDelta;
	}

	return true;

	FUNCTION_END;
}