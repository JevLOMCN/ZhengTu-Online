/**
 *\file		ParticleSystem.cpp
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:21:00 中国标准时间
 *\brief	粒子系统
 *
 * 
 *
*/

#include "public.h"
#include "../../RenderD3D/include/RenderD3D.h"
#include "../../RenderD3D/include/D3DDevice.h"
#include "../../RenderD3D/include/TextureManager.h"
#include "../../RenderD3D/include/CustomTexture.h"

#include "./ParticleSystem.h"
#include <time.h>

/**
 * \brief 构造函数
 * 
 * 
 * 
 * \param emiter : 发射器
 * \param movement : 运动控制器
 * \return 
 */
// Construction
CParticleSystem::CParticleSystem( CParticleEmiter* emiter, CParticleSysMove* movement )
: m_dwThisID( -1 )
{
	FUNCTION_BEGIN;

	//int i;

	// Set buffer size
	// Same for all
	m_bufferSize = PARTICLE_BUFFER_SIZE;
	m_flushSize = BUFFER_FLUSH_SIZE;
	m_dwBase = PARTICLE_BUFFER_SIZE;

	//set particle pointer to null
	m_pParticles = NULL;
	m_pParticlesFree = NULL;

	//Get emiter
	m_emiter = emiter;
	m_movement = movement;
	
	if ( NULL == m_movement )
		m_bIsMoving = false;
	else
		m_bIsMoving = true;
	//
	m_uParticlesAlive = 0;
	m_fTimeLastUpdate = 0.0f;
	m_fEmissionResidue =  0.0f;
	m_d3dvDirection = D3DXVECTOR3(0,1,0);		//particle emit direction

	m_d3dvLocation = m_d3dvPrevLocation = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_d3dvAttraction = m_d3dvLocation;
	m_bIsAttractive = false;
	m_AttractStrength = 1;
	
	//other force
	m_fOtherForce = 0.0f;
	m_OtherForceDir = OTHER_FD_NUM;

	m_texNum = 1;
	m_pTex = NULL;

	m_uTextureID = 0;
	m_uMovingType = PARTICLE_MT_CIRCLE;
	m_fMovingSpeed = 0.1745f;

	m_uEmiterType = PARTICLE_ET_POINT;

	m_vSystem = D3DXVECTOR3(0.0f,0.0f,0.0f);
	
	m_bMoveInGame = false;

	//particle system life
	m_sysLife = -1.0f;		//live for ever
	m_elaspedTime = 0.0f;

	//
	m_bNeedSpark = false;

	//
	m_fStartTime = 0.0f;

	m_bVisible = false;
	m_bPlaying = false;
	m_bFirstCalledPlay = false;

	//live range
	m_liveRange.left = 0;
	m_liveRange.right = 1280;
	m_liveRange.top = 0;
	m_liveRange.bottom = 1024;

	m_bUseOutLiveRange = false;

	m_blendType = Blend_AlphaAdd;

	//suppress about
	m_bIsSuppressed = false;
	m_fSuppressTimeStart = 0.0f;
	m_fSuppressTimeEnd = 0.0f;

	//sys rotate speed
	m_sysRotSpeed = 0.0f;

	//particle size height/width rate
	m_sizeHWRate = 1.0f;

	m_bUseCharacterOffset = true;

	m_sysSizeScale = 1.0f;

	m_uParticlesAlive = 0;

	m_bLoopPlay = true;

	m_bRenderWithManager = true;

	m_childName[0] = 0L;

	FUNCTION_END;
}

/**
 * \brief 初始化
 * 
 * 参数复位
 * 
 * \return 
 */
//Init system when replay
void CParticleSystem::Init()
{
	FUNCTION_BEGIN;

	//m_fTimeLastUpdate = Engine_GetTime();
	m_fEmissionResidue =  0.0f;

	m_d3dvLocation = m_d3dvPrevLocation = D3DXVECTOR3(0.0f,0.0f,0.0f);

	m_bIsSuppressed = false;

	//particle system life
	m_elaspedTime = 0.0f;

	m_bFirstCalledPlay = true;

	//Reset bomb state
	m_bBombState = false;
	//m_bomb.startTime = 1000;

	FUNCTION_END;
}

/**
 * \brief 播放粒子效果
 * 
 * 
 * 
 * \param  : 粒子系统生命期
 * \return 
 */
//Play system
void CParticleSystem::Play( const float& pSysLife /* = -1 */ )
{
	FUNCTION_BEGIN;

	Init();
	
	m_sysLife = pSysLife;
	m_bPlaying = true;
	m_bVisible = true;

	m_bRePlay = false;

	FUNCTION_END;
}

void CParticleSystem::RePlay()
{
	//DeleteAllParticles();
	Play( m_sysLife );
	m_bRePlay = true;
	m_bLoopPlay = true;

	//m_bFirstCalledPlay	= false;
}

/**
 * \brief 释放设备
 * 
 * 
 * 
 * \return 
 */
//Delete all device
void CParticleSystem::UnInitDevice()
{
	FUNCTION_BEGIN;

	//Release particle texture
	for( int i=0;i<m_texNum;i++ )
	{
		if ( m_pTex[i] )
		{
			GetTextureManager()->DeleteCustomeTexture( m_pTex[i] );
			m_pTex[i] = NULL;
		}
	}

	delete [] m_pTex;
	m_pTex = NULL;

	FUNCTION_END;
}

/**
* \brief 删除所有粒子
* 
* 
* 
* \return 
*/
void CParticleSystem::DeleteAllParticles()
{
	//Delete all particles
	while ( m_pParticles )
	{
		CParticle* pParticle = m_pParticles;
		m_pParticles = pParticle->m_pNext;
		delete pParticle;
	}

	while ( m_pParticlesFree )
	{
		CParticle* pParticle = m_pParticlesFree;
		m_pParticlesFree = pParticle->m_pNext;
		delete pParticle;
	}

	m_uParticlesAlive = 0;
}


/**
 * \brief 析构函数
 * 
 * 
 * 
 * \return 
 */
//-----------------------------------------------------------------------------
//	Destruction
//-----------------------------------------------------------------------------
CParticleSystem::~CParticleSystem()
{
	FUNCTION_BEGIN;

	//int i;

	UnInitDevice();
	
	DeleteAllParticles();

	//Delete emiter
	if ( m_emiter )
		ReleaseEmiter();

	//Delete movement
	if ( m_movement )
	{
		delete m_movement;
		m_movement = NULL;
	}

	FUNCTION_END;
}

/**
 * \brief 设置系统在游戏中的位置
 * 
 * 
 * 
 * \param vGameSpace : 在游戏中的位置
 * \return 
 */
//-----------------------------------------------------------------------------
//	CParticleSystem::SetPositionInGameSpace
//-----------------------------------------------------------------------------
void CParticleSystem::SetPositionInGameSpace( const D3DXVECTOR3	& vGameSpace )
{
	FUNCTION_BEGIN;

	m_vSystem.x = vGameSpace.x;
	m_vSystem.y = vGameSpace.y;
	m_vSystem.z = 0.0f;

	FUNCTION_END;
}

/**
 * \brief 获取系统在游戏中的位置
 * 
 * 
 * 
 * \return 系统在游戏中的位置
 */
//-----------------------------------------------------------------------------
//	CParticleSystem::GetPositionInGameSpace
//-----------------------------------------------------------------------------
D3DXVECTOR3& CParticleSystem::GetPositionInGameSpace()
{
	FUNCTION_BEGIN;

	return m_vSystem;

	FUNCTION_END;
}

/**
 * \brief 获取其他外力的强度
 * 
 * 
 * 
 * \param  : 是否获取原始值
 * \return 其他外力强度
 */
float CParticleSystem::GetOtherForceStrength( bool bOriginal /* = false  */ )
{
	FUNCTION_BEGIN;

	if ( bOriginal )
		return m_fOtherForce;

	switch( m_OtherForceDir )
	{
	case OTHER_FD_LEFT:
		return 0.0f - m_fOtherForce;
		break;
	case OTHER_FD_RIGHT:
		return m_fOtherForce;
		break;
	case OTHER_FD_RANDOM:
		{
			return RANDOM_NUM * m_fOtherForce;
			break;
		}
	case OTHER_FD_NUM:
		return 0.0f;
		break;
	} 

	return 0.0f;

	FUNCTION_END;
}

void CParticleSystem::Bomb( const enmBombDir& dir, int strength /* = 10 */, int lastTime /* = 3000 */, bool bToDie /* = true*/ )
{
	if ( !m_bPlaying )
		return;

    //Change sys life
	if ( bToDie && (m_sysLife*1000 > lastTime) )
        m_sysLife = m_elaspedTime + (float)lastTime / 1000.0f;

	m_bBombState = true;

	//Record bomb start time
	m_bomb.startTime = m_elaspedTime;

	//Save bomb data
	m_bomb.life = lastTime;
	m_bomb.dir = dir;
	m_bomb.strength = strength;
}

void CParticleSystem::UpdateBomb()
{
	if ( !m_bPlaying )
		return;

	if ( (m_elaspedTime >= m_bomb.startTime) && (m_elaspedTime < m_bomb.startTime+(float)m_bomb.life/1000.0f) && !m_bBombState )
        Bomb( m_bomb.dir, m_bomb.strength, m_bomb.life );

	if (  m_elaspedTime >= m_bomb.startTime + (float)m_bomb.life/1000.0f )
	{
		m_bBombState = false;
        return;
	}	
}

int CParticleSystem::GetBombStrength()
{
	if ( !m_bBombState )
        return 0;
	
	int strength = (int)( m_bomb.strength * ( pow( 2.0f, GetBombLeftLife() ) - 1 ) );
	if ( strength <= 0 )
		return 0;

	//Return strength according to dir
	switch( m_bomb.dir )
	{
	case BOMB_DIR_OUTSIDE:
		return strength;
		break;
	case BOMB_DIR_INSIDE:
		return -strength;
		break;
	case BOMB_DIR_RAND:
		return (rand() % 2) ? strength : -strength;
		break;
	default:
		return 0;
		break;
	}

	return 0;
}

int CParticleSystem::GetBombLeftLife()
{
	if ( !m_bBombState )
        return 0;

	return (int)((float)m_bomb.life/1000.0f - (m_elaspedTime-m_bomb.startTime));
}

/**
 * \brief 刷新
 * 
 * 
 * 
 * \param fTime : 时间
 * \param offset : 位置偏移
 * \return 
 */
//-----------------------------------------------------------------------------
//	CParticleSystem::Update
//-----------------------------------------------------------------------------
HRESULT CParticleSystem::Update(float fTime, stPointF offset)
{
	FUNCTION_BEGIN;

	/*if ( !m_bPlaying )
		return S_OK;*/

	/*m_vSystem.x += offset.x;
	m_vSystem.y += offset.y;*/
	
	//int i;
	CParticle* pParticle;
	CParticle** ppParticle;

	ppParticle = &m_pParticles;
	// fTime is absolute time.  So we need to calculate the relative time displacement or fTimeDelta that has elasped
	float fTimeDelta = fTime - m_fTimeLastUpdate;

	if ( ((fabs(m_fTimeLastUpdate) < 0.000001f)) || ((fabs(fTimeDelta) > 0.1f))	//For first time update or next time update
		|| m_bFirstCalledPlay )
	{
		m_bFirstCalledPlay = false;
		fTimeDelta = 0.0f;
	}

	//Add sys age
	m_elaspedTime += fTimeDelta;

	m_fTimeLastUpdate = fTime;

	//we show sys some time later
	if ( m_elaspedTime >= m_fStartTime )
	{
		m_bVisible = true;
		m_bPlaying = true;
	}
	else
	{
		m_bVisible = false;
		m_bPlaying = false;
	}

	if ( !m_bPlaying )
		return S_OK;

	//see if we are suppressed
	if ( (m_elaspedTime > m_fSuppressTimeStart) && (m_elaspedTime < m_fSuppressTimeEnd) )
		m_bIsSuppressed = true;
	else
		m_bIsSuppressed = false;
	

	//to see if sys to be die
	if ( m_sysLife != -1 )
	{
		if ( m_elaspedTime >= m_sysLife )
		{
			//Stop();
			if ( m_bLoopPlay && !m_childName[0] )
				RePlay();
			else
				Stop();
		}
	}

	//Update bomb effect
	UpdateBomb();

	// Particle move type
	if ( m_bIsMoving && m_movement )
	{
		switch( m_movement->m_type )		//m_uMovingType
		{
		case PARTICLE_MT_CIRCLE:
			{
				if ( m_movement )
				{
					CMovementCircle* movement = static_cast<CMovementCircle*>(m_movement);
					float angle = m_elaspedTime * movement->GetMoveSpeed() / (movement->GetRadio().x);
					if ( !movement->IsCW() )
						angle = 0.0f - angle;
					m_d3dvLocation.x = movement->GetRadio().x * cosf( angle );
					m_d3dvLocation.y = movement->GetRadio().y * sinf( angle );
				}
				else
				{
					float angle = m_elaspedTime * 200.0f / 200.0f;
					m_d3dvLocation.x = 200.0f * cosf( angle );
					m_d3dvLocation.y = 200.0f * sinf( angle );
				}				
			}
			break;
		case PARTICLE_MT_LINE:
			{
				if ( m_movement )
				{
					CMovementLine* movement = static_cast<CMovementLine*>(m_movement);
					float s = m_elaspedTime * movement->GetMoveSpeed();
					m_d3dvLocation.x = s * cosf( movement->GetAngle() );
					m_d3dvLocation.y = s * sinf( movement->GetAngle() );
				}
				else
				{
					float s = m_elaspedTime * 200.0f;
					m_d3dvLocation.x = s * cosf( 0.0f );
					m_d3dvLocation.y = s * sinf( 0.0f );
				}				
			}
			break;
		case PARTICLE_MT_LINE_BACK:
			{
				if ( m_movement )
				{
					CMovementLineBack* movement = static_cast<CMovementLineBack*>(m_movement);
					float s = movement->GetWidth() / 2.0f * sinf( m_elaspedTime * movement->GetMoveSpeed() / movement->GetWidth() * D3DX_PI );
					m_d3dvLocation.x = s * cosf( movement->GetAngle() );
					m_d3dvLocation.y = s * sinf( movement->GetAngle() );
				}
				else
				{
					float s = 200.0f / 2.0f * sinf( m_elaspedTime * 200.0f / 200.0f * D3DX_PI );
					m_d3dvLocation.x = s * cosf( 0.0f );
					m_d3dvLocation.y = s * sinf( 0.0f );
				}				
			}
			break;
		case PARTICLE_MT_WAVE:
			{
				if ( m_movement )
				{
					CMovementWave* movement = static_cast<CMovementWave*>(m_movement);
					//we first get pos at horizontal
					m_d3dvLocation.x = m_elaspedTime * movement->GetMoveSpeed();
					m_d3dvLocation.y = movement->GetHeight() * pow( movement->GetScaleRate(), m_elaspedTime )
										* sinf( m_elaspedTime * movement->GetMoveSpeed() / movement->GetWaveLength() * 2.0f * D3DX_PI );
					m_d3dvLocation.z = 0.0f;
					//then we rotate
					D3DXMATRIX matRot;
					D3DXMatrixRotationZ( &matRot, movement->GetSlopAngle() );
					D3DXVec3TransformCoord( &(m_d3dvLocation), &(m_d3dvLocation), &matRot );
				}
				else
				{
					//we first get pos at horizontal
					m_d3dvLocation.x = m_elaspedTime * 200.0f;
					m_d3dvLocation.y = 300.0f * ( 1.0f + 0.95f * m_elaspedTime )
						* sinf( m_elaspedTime * 200.0f / 200.0f * 2.0f * D3DX_PI );
					m_d3dvLocation.z = 0.0f;
					//then we rotate
					D3DXMATRIX matRot;
					D3DXMatrixRotationZ( &matRot, 0.0f );
					D3DXVec3TransformCoord( &(m_d3dvLocation), &(m_d3dvLocation), &matRot );
				}				
			}
			break;
		case PARTICLE_MT_NUM:
			m_d3dvLocation.x = 0.0f;
			m_d3dvLocation.y = 0.0f;
			m_d3dvLocation.z = 0.0f;
			break;
		default:
			break;
		}
	}
	else
	{
		// this will reset it every frame, but most importantly it will reset it to the origin if it isn't Moving
		m_d3dvLocation.x = 0.0f;
		m_d3dvLocation.y = 0.0f;
		m_d3dvLocation.z = 0.0f;

	}
	
	m_d3dvLocation.z = 0.0f;

	// clear our counter variable before counting how many particles are alive
	//m_uParticlesAlive = 0;

	//Update all particles
	while ( *ppParticle )
	{
		pParticle = *ppParticle;
		pParticle->Update( fTimeDelta, offset );

		//Remove dead particles
		if ( pParticle->m_fAge < 0.0f )
		{
			//Emit spark
			if ( m_bNeedSpark && (!pParticle->m_bSpark) )
			{
				for( int i=0;i<pParticle->m_sparkNum;i++ )
				{
					//Get spark
					CParticle* pSpark;
					if ( m_pParticlesFree )
					{
						pSpark = m_pParticlesFree;
						m_pParticlesFree = pSpark->m_pNext;
					}
					else
					{
						if ( NULL == ( pSpark = new CParticle ) )
							return E_OUTOFMEMORY;
					}
					
					//Insert spark into list
					pSpark->m_pNext = pParticle->m_pNext;
					pParticle->m_pNext = pSpark;

					pSpark->m_bSpark = true;

					//Set cur spark property

					//Life about
					pSpark->m_fAge = 0.0f;
					pSpark->m_fLifetime = pParticle->m_fLifetime * 0.3f;
					//pSpark->m_fLifetime = 1.0f;
					
					//Set spark color the same as parent
					//And no fade of color
					pSpark->m_d3dcColor = pParticle->m_d3dcColor;
					pSpark->m_d3dcColorDelta.a = 0.0f;
					pSpark->m_d3dcColorDelta.r = 0.0f;
					pSpark->m_d3dcColorDelta.g = 0.0f;
					pSpark->m_d3dcColorDelta.b = 0.0f;
					// we just reduce spark alpha value
					pSpark->m_fAlpha = 1.0f;
					pSpark->m_fAlphaDelta = ( 0.0f - pSpark->m_fAlpha ) / pSpark->m_fLifetime;
					//pSpark->m_fAlphaDelta = 0.0f;

					//We set spark size 0.7 * size of parent at die
					pSpark->m_fSize = pParticle->m_fSize * 0.7f;
					pSpark->m_fSizeDelta = ( 0.0f - pSpark->m_fSize ) / pSpark->m_fLifetime;
					//pSpark->m_fSizeDelta = 0.0f;
					
					//Obviously spark affected with gravity
					//This time we give it a constant value
					//And with no change during her life time
					pSpark->m_fGravity = 1.0f;

					//Set spark velocity the same as parent at end
					//Except the direction
					float abVelocity = fabs(pParticle->m_d3dvVelocity.y * 0.2f );
					pSpark->m_d3dvVelocity.x = abVelocity * 0.5f * sinf( (float)i / (float)(pParticle->m_sparkNum) * 2.0f * D3DX_PI );
					pSpark->m_d3dvVelocity.y = abVelocity * cosf( RANDOM_NUM * 2.0f * D3DX_PI );
					pSpark->m_d3dvVelocity.z = 0.0f;

					//Spark start pos same as parent
					pSpark->m_d3dvLocation = pParticle->m_d3dvLocation;
					pSpark->m_d3dvLocation.z = 0.0f;

					//Remember old pos
					pSpark->m_d3dvPrevLocation = pSpark->m_d3dvLocation;
					
					//Add to parent system
					pSpark->SetParent( this );
					
					//end property
				}
			}
			// Kill particle
			*ppParticle = pParticle->m_pNext;
			pParticle->m_pNext = m_pParticlesFree;
			m_pParticlesFree = pParticle;

			//Reduce particles num
			if ( !pParticle->m_bSpark )
				m_uParticlesAlive-- ;
		}
		else
		{
			ppParticle = &pParticle->m_pNext;
		}
	}


	// calculate how many particles we should create from ParticlesPerSec and time elapsed taking the 
	// previous frame's EmissionResidue into account.
	float fParticlesNeeded = m_uParticlesPerSec * fTimeDelta + m_fEmissionResidue;

	// cast the float fparticlesNeeded to a INT to see how many particles we really need to create.
	unsigned int uParticlesCreated = (unsigned int)fParticlesNeeded;
	unsigned int dwParticlesEmit = m_uParticlesAlive + uParticlesCreated;

	if ( !m_bIsSuppressed )
	{
		// remember the difference between what we wanted to create and how many we created.
		m_fEmissionResidue = fParticlesNeeded - uParticlesCreated;
	}
	else
	{
		m_fEmissionResidue = fParticlesNeeded;
		uParticlesCreated = 0;
	}

	if ( m_bRePlay )
	{
		if ( m_uParticlesAlive > 0 )
			uParticlesCreated = 0;
		else
		{
			if ( m_bLoopPlay )
				m_bRePlay = false;
			else
				Stop();
		}
	}

	// see if actually have any to create
	if ( (uParticlesCreated > 0) && !m_bBombState )
	{
		// loop through all the particles to see if any are available
		while ( m_uParticlesAlive < MAX_PARTICLES && m_uParticlesAlive < dwParticlesEmit )
		{
			if ( !uParticlesCreated )
				continue;

			if( m_pParticlesFree )
			{
				pParticle = m_pParticlesFree;
				m_pParticlesFree = pParticle->m_pNext;
			}
			else
			{
				if( NULL == ( pParticle = new CParticle ) )
					return E_OUTOFMEMORY;
			}

			pParticle->m_pNext = m_pParticles;
			m_pParticles = pParticle;

			//Add alive particles num
			m_uParticlesAlive++ ;

			//Emit new particle

			//if ( pParticle->m_fAge <= 0.0f )
			{
				//init offset
				pParticle->m_offset.x = 0.0f;
				pParticle->m_offset.y = 0.0f;

				// New particle so it's age is 0.0f 
				// Actually should be some randomization of fTimeDelta since in a perfect world
				// particles would have been created between our last update and now.
				pParticle->m_fAge = 0.0f;
				pParticle->m_fLifetime = m_fLife + RANDOM_NUM * m_fLifeVar;
				Clamp( pParticle->m_fLifetime, MIN_LIFETIME, MAX_LIFETIME ); 

				// our start color is going to be the System's StartColor + the System's color variation
				pParticle->m_d3dcColor.r = m_d3dcColorStart.r + RANDOM_NUM * m_d3dcColorVar.r;
				pParticle->m_d3dcColor.g = m_d3dcColorStart.g + RANDOM_NUM * m_d3dcColorVar.g;
				pParticle->m_d3dcColor.b = m_d3dcColorStart.b + RANDOM_NUM * m_d3dcColorVar.b;
				// we don't use the alpha value of the color here
				pParticle->m_d3dcColor.a = 1.0f;

				// with variation, the Color could overflow
				Clamp( pParticle->m_d3dcColor.r, 0.0f, 1.0f );
				Clamp( pParticle->m_d3dcColor.g, 0.0f, 1.0f );
				Clamp( pParticle->m_d3dcColor.b, 0.0f, 1.0f );

				// calculate a delta so that by the time the particle dies, it will have reached it's "ColorEnd"
				pParticle->m_d3dcColorDelta.r = (m_d3dcColorEnd.r - pParticle->m_d3dcColor.r) / pParticle->m_fLifetime;
				pParticle->m_d3dcColorDelta.g = (m_d3dcColorEnd.g - pParticle->m_d3dcColor.g) / pParticle->m_fLifetime;
				pParticle->m_d3dcColorDelta.b = (m_d3dcColorEnd.b - pParticle->m_d3dcColor.b) / pParticle->m_fLifetime;

				// Calculate our Alpha from the system's AlphaStart and AlphaVar
				pParticle->m_fAlpha = m_fAlphaStart + RANDOM_NUM * m_fAlphaVar;
				// clamp any overflow
				Clamp( pParticle->m_fAlpha, MIN_ALPHA, MAX_ALPHA );
				// calculate a delta so that by the time the particle dies, it will have reached it's "AlphaEnd"
				pParticle->m_fAlphaDelta = (m_fAlphaEnd - pParticle->m_fAlpha) / pParticle->m_fLifetime;


				pParticle->m_fSize = m_fSizeStart + RANDOM_NUM * m_fSizeVar;
				//Clamp( pParticle->m_fSize, MIN_SIZE, MAX_SIZE );
				pParticle->m_fSizeDelta = (m_fSizeEnd - pParticle->m_fSize) / pParticle->m_fLifetime;
				//pParticle->m_fSizeDelta = ( m_fSizeEnd - m_fSizeStart ) / pParticle->m_fLifetime;


				pParticle->m_fGravity = m_fGravityStart*9.8f + RANDOM_NUM * m_fGravityVar*9.8f;
				// yes this is a lot of repetitive calculation ( * 9.8f ), but it keeps the meaning of m_fgravity clear.
				// It's a percentage of normal gravity.
				Clamp( pParticle->m_fGravity, MIN_GRAVITY*9.8f, MAX_GRAVITY*9.8f );
				pParticle->m_fGravityDelta = ( m_fGravityEnd*9.8f - pParticle->m_fGravity ) / pParticle->m_fLifetime;

				// since Velocity is a change in position over time, we calculate a Velocity the particle would have to 
				// have travelled to move from PrevLocation to Location in fTimeDelta seconds.
				// Then we can calculate points in that linear path by passing in different Time deltas (note the 'RANDOM_NUM * fTimeDelta' below)
				D3DXVECTOR3 tempVelocity = (m_d3dvLocation - m_d3dvPrevLocation)/fTimeDelta;

				// Emit the particles from a location between the last known location and the current location
				pParticle->m_d3dvLocation = m_d3dvPrevLocation + tempVelocity * RANDOM_NUM * fTimeDelta;

				//We add particle emiter type
				switch( m_emiter->m_type )		//m_uEmiterType
				{
				case PARTICLE_ET_POINT:
					{
						//This time we use emiter
						if ( m_emiter )
						{
							//Get emiter
							CEmiterPoint* emiter = static_cast<CEmiterPoint*>(m_emiter);
							//pos
							float angle = ( emiter->GetStartAngle() + emiter->GetEndAngle() ) / 2.0f;
							angle += RANDOM_NUM * ( angle - emiter->GetStartAngle() );

							//angle = angle / 360.0f * D3DX_PI;

							pParticle->m_d3dvLocation.x += 10.0f * cosf( angle );
							pParticle->m_d3dvLocation.y += 10.0f * sinf( angle );
							//dir
							pParticle->m_d3dvVelocity.x = cosf( angle );
							pParticle->m_d3dvVelocity.y = sinf( angle );
						}
						else
						{
							//pos
							float angle = RANDOM_NUM * 2.0f * D3DX_PI;
							pParticle->m_d3dvLocation.x += 10.0f * cosf( angle );
							pParticle->m_d3dvLocation.y += 10.0f * sinf( angle );
							//dir
							pParticle->m_d3dvVelocity.x = cosf( angle );
							pParticle->m_d3dvVelocity.y = sinf( angle );
						}
					}
					
					break;
				case PARTICLE_ET_LINE:
					{
						//This time we use emiter
						if ( m_emiter )
						{
							CEmiterLine* emiter = static_cast<CEmiterLine*>(m_emiter);
							//Get emit pos and direction
							switch( emiter->GetEmiterDir() )
							{
							case 0:		//From two end
								{
									//We use the two ends to emit just as point emiter
									//this time simply use Angle 360
									//pos : we give some random among R = 0.05 * width;
									float angle = RANDOM_NUM * 2.0f * D3DX_PI;
									float dir = (RANDOM_NUM > 0) ? 1.0f : -1.0f;
									pParticle->m_d3dvLocation.x += dir * cosf( emiter->GetAngle() ) * emiter->GetWidth() / 2.0f
																	+ cosf( angle ) * 0.05f * emiter->GetWidth();
									pParticle->m_d3dvLocation.y += dir * sinf( emiter->GetAngle() ) * emiter->GetWidth() / 2.0f
																	+ sinf( angle ) * 0.05f * emiter->GetWidth();
									//dir
									// This time we give a range of 360 degree
									float RandomPitch = RANDOM_NUM * 2.0f * D3DX_PI;
									pParticle->m_d3dvVelocity.x = cosf( RandomPitch );
									pParticle->m_d3dvVelocity.y = sinf( RandomPitch );
								}
								break;
							case 1:		//From one side
								{
									//pos
									float dist = RANDOM_NUM;
									pParticle->m_d3dvLocation.x += dist * cosf( emiter->GetAngle() ) * emiter->GetWidth() / 2.0f;
									pParticle->m_d3dvLocation.y += dist * sinf( emiter->GetAngle() ) * emiter->GetWidth() / 2.0f;
									pParticle->m_d3dvLocation.z = 0.0f;
									//dir
									//we use vec cross
									D3DXVECTOR3 zAxis(0.0f,0.0f,(dist>0) ? 1.0f : -1.0f);
									D3DXVec3Cross( &(pParticle->m_d3dvVelocity), &(pParticle->m_d3dvLocation), &(zAxis) );
									//pParticle->m_d3dvVelocity.y = 0.0f - pParticle->m_d3dvVelocity.y;
									/*pParticle->m_d3dvVelocity.x = pParticle->m_d3dvLocation.y;
									pParticle->m_d3dvVelocity.y = 0.0f - pParticle->m_d3dvLocation.x;*/
								}								
								break;
							case 2:		//From both side
								{
									//pos
									float dist = RANDOM_NUM;
									pParticle->m_d3dvLocation.x += dist * cosf( emiter->GetAngle() ) * emiter->GetWidth() / 2.0f;
									pParticle->m_d3dvLocation.y += dist * sinf( emiter->GetAngle() ) * emiter->GetWidth() / 2.0f;
									//dir
									pParticle->m_d3dvVelocity.x = pParticle->m_d3dvLocation.y;
									pParticle->m_d3dvVelocity.y = 0.0f - pParticle->m_d3dvLocation.x;
									float dir = RANDOM_NUM;
									pParticle->m_d3dvVelocity.x *= dir;
									pParticle->m_d3dvVelocity.y *= dir;
									
								}								
								break;
							case 3:		//From one side
								{
									//pos
									float dist = RANDOM_NUM;
									pParticle->m_d3dvLocation.x += dist * cosf( emiter->GetAngle() ) * emiter->GetWidth() / 2.0f;
									pParticle->m_d3dvLocation.y += dist * sinf( emiter->GetAngle() ) * emiter->GetWidth() / 2.0f;
									pParticle->m_d3dvLocation.z = 0.0f;
									//dir									
									pParticle->m_d3dvVelocity.x = pParticle->m_d3dvLocation.y;
									pParticle->m_d3dvVelocity.y = 0.0f - pParticle->m_d3dvLocation.x;
								}								
								break;
							default:		//random
								{
									//pos
									float dist = RANDOM_NUM;
									pParticle->m_d3dvLocation.x += dist * cosf( emiter->GetAngle() ) * emiter->GetWidth() / 2.0f;
									pParticle->m_d3dvLocation.y += dist * sinf( emiter->GetAngle() ) * emiter->GetWidth() / 2.0f;
									//dir
									// This time we give a range of 360 degree
									float RandomPitch = RANDOM_NUM * 2.0f * D3DX_PI;
									pParticle->m_d3dvVelocity.x = cosf( RandomPitch );
									pParticle->m_d3dvVelocity.y = sinf( RandomPitch );									
								}
								break;
							}
						}
						else
						{
							pParticle->m_d3dvLocation.x += RANDOM_NUM * 200.0f;
							pParticle->m_d3dvLocation.y += RANDOM_NUM * 0.2f;
						}
					}					
					break;
				case PARTICLE_ET_CIRCLE:
				case PARTICLE_ET_SPHERE:
					{
						//This time we use emiter
						if ( m_emiter )
						{
							CEmiterCircle* emiter = static_cast<CEmiterCircle*>(m_emiter);
							switch( emiter->GetEmiterDir() )
							{
							case 0:		//outside emit
								{
									//pos
									float angle = RANDOM_NUM * 2.0f * D3DX_PI;
									pParticle->m_d3dvLocation.x += emiter->GetRadio() * cosf( angle );
									pParticle->m_d3dvLocation.y += emiter->GetRadio() * sinf( angle );
									//dir
									pParticle->m_d3dvVelocity.x = cosf( angle );
									pParticle->m_d3dvVelocity.y = sinf( angle );
								}
								break;
							case 1:		//inside emit
								{
									//pos
									float angle = RANDOM_NUM * 2.0f * D3DX_PI;
									pParticle->m_d3dvLocation.x += emiter->GetRadio() * cosf( angle );
									pParticle->m_d3dvLocation.y += emiter->GetRadio() * sinf( angle );
									//dir
									pParticle->m_d3dvVelocity.x = 0.0f - cosf( angle );
									pParticle->m_d3dvVelocity.y = 0.0f - sinf( angle );
								}
								break;
							case 2:		//both side emit
								{
									//pos
									float angle = RANDOM_NUM * 2.0f * D3DX_PI;
									pParticle->m_d3dvLocation.x += emiter->GetRadio() * cosf( angle );
									pParticle->m_d3dvLocation.y += emiter->GetRadio() * sinf( angle );
									//dir
									float dir = RANDOM_NUM;
									pParticle->m_d3dvVelocity.x = cosf( angle ) * dir;
									pParticle->m_d3dvVelocity.y = sinf( angle ) * dir;
								}
								break;
							default:	//in case for some error
								{
									float angle = RANDOM_NUM * D3DX_PI * 2.0f;
									pParticle->m_d3dvLocation.x += 200.0f * cosf( angle );
									pParticle->m_d3dvLocation.y += 200.0f * sinf( angle );
								}
								break;
							}
						}
						else
						{
							float angle = RANDOM_NUM * D3DX_PI * 2.0f;
							pParticle->m_d3dvLocation.x += 200.0f * cosf( angle );
							pParticle->m_d3dvLocation.y += 200.0f * sinf( angle );
						}						
					}
					break;		
				case PARTICLE_ET_PLANE:
					{
						//This time we use emiter
						if ( m_emiter )
						{
							//get emiter
							CEmiterPlane* emiter = static_cast<CEmiterPlane*>(m_emiter);
							//pos
							pParticle->m_d3dvLocation.x += RANDOM_NUM * emiter->GetWidth() / 2.0f;
							pParticle->m_d3dvLocation.y += RANDOM_NUM * emiter->GetHeight() / 2.0f;
							//dir
							//This time we just give a random dir
							float angle = RANDOM_NUM * D3DX_PI * 2.0f;
							pParticle->m_d3dvVelocity.x = cosf( angle );
							pParticle->m_d3dvVelocity.y = sinf( angle );
						}
						else
						{
							//pos
							pParticle->m_d3dvLocation.x += 200.0f * RANDOM_NUM;
							pParticle->m_d3dvLocation.y += 200.0f * RANDOM_NUM;
							//dir
							float angle = RANDOM_NUM * D3DX_PI * 2.0f;
							pParticle->m_d3dvVelocity.x = cosf( angle );
							pParticle->m_d3dvVelocity.y = sinf( angle );
						}						
					}
					break;
				case PARTICLE_ET_POINT_LIST:
					{
						if ( m_emiter )
						{
							//Get emiter
							CEmiterPointList* emiter = static_cast<CEmiterPointList*>(m_emiter);
							//we randomly select one point from list as our init position
							if ( emiter->GetSize() > 0 )
							{
								stPointI pt = emiter->RandomGetOnePoint();
								//stPointI pt = emiter->HScaneGetOnePoint();
								pParticle->m_d3dvLocation.x = (float)pt.x;
								pParticle->m_d3dvLocation.y = (float)pt.y;
							}

							//Get dir
							//This time we just give a random dir
							float angle = RANDOM_NUM * D3DX_PI * 2.0f;
							pParticle->m_d3dvVelocity.x = cosf( angle );
							pParticle->m_d3dvVelocity.y = sinf( angle );
						}
						else
						{
							//pos
							pParticle->m_d3dvLocation.x = 0;
							pParticle->m_d3dvLocation.y = 0;
							//dir
							float angle = RANDOM_NUM * D3DX_PI * 2.0f;
							pParticle->m_d3dvVelocity.x = cosf( angle );
							pParticle->m_d3dvVelocity.y = sinf( angle );
						}
					}
					break;
				case PARTICLE_ET_NUM:
					break;
				default:
					break;
				}

				pParticle->m_d3dvLocation.z = 0.0f;
				pParticle->m_d3dvVelocity.z = 0.0f;
				//Normalize dir
				D3DXVec3Normalize( &(pParticle->m_d3dvVelocity), &(pParticle->m_d3dvVelocity) );

				// Update the previous location so the next update we can remember where we were
				pParticle->m_d3dvPrevLocation = m_d3dvLocation;

				// Give our particle spark effect
				pParticle->m_bSpark = false;
				//pParticle->m_sparkNum = MAX_SPARK_NUM;

				// The emitter has a Direction
				// This time we give a range of 360 degree
				if ( NULL == m_emiter )		//Cause we use emiter to control particle emit
				{
					float RandomPitch = RANDOM_NUM * m_fTheta * 2.0f * D3DX_PI / 180.0f ;
					pParticle->m_d3dvVelocity.x = cosf( RandomPitch );
					pParticle->m_d3dvVelocity.y = sinf( RandomPitch );
					pParticle->m_d3dvVelocity.z = 0.0f;
				}
				

				// Velocity at this point is just a direction (normalized vector ) and needs to be multiplied by 
				// the speed component to be a true velocity
				float fNewSpeed = m_fSpeed + RANDOM_NUM * m_fSpeedVar;
				Clamp( fNewSpeed, MIN_SPEED, MAX_SPEED );
				// Luckily D3D knows how to multiply a Vector by a Scalar
				pParticle->m_d3dvVelocity *= fNewSpeed ;

				// let the particle know who it's Daddy is
				pParticle->SetParent(this);

				//return S_OK;

				// decrement the number of needed particles
				uParticlesCreated--;
			}
		}
	}

	///set prev Location to current location so next frame we know where we were
	m_d3dvPrevLocation = m_d3dvLocation;

	return S_OK;

	FUNCTION_END;
}

/**
 * \brief 创建设备
 * 
 * 
 * 
 * \return 成功则返回true, 否则返回false
 */
//-----------------------------------------------------------------------------
//
//	CParticleSystem::Init all device
//
//	Init all device object after device lost
//
//-----------------------------------------------------------------------------
bool CParticleSystem::InitDevice()
{
	FUNCTION_BEGIN;

	//First release texture
	if ( m_pTex )
	{
		for( int i=0;i<m_texNum;i++ )
		{
			if ( m_pTex[i] )
			{
				GetTextureManager()->DeleteCustomeTexture( m_pTex[i] );
				m_pTex[i] = NULL;
			}
		}
	}

	//Get new texture	
	m_pTex = new CCustomTexture*[m_texNum];

	for( int i=0;i<m_texNum;i++ )
	{
		m_pTex[i] = NULL;

		//Create texture
		m_pTex[i] = GetTextureManager()->CreateCurstomTexture( PARTICLE_TEXTURE_SIZE, PARTICLE_TEXTURE_SIZE, D3DFMT_A4R4G4B4 );
		if ( NULL == m_pTex[i] )
			return false;
		if ( !m_pTex[i]->Init() )
			return false;

		//Backup working path
		char workPath[MAX_PATH];
		GetCurrentDirectory( MAX_PATH, workPath );

		char imgFile[256];
		sprintf( imgFile, "datas\\particle\\%s.tga", particleTexName[m_uTextureID+i] );
		Stream* pStream = OpenPackFileForRead( "data\\datas.pak", imgFile );
		
		if(pStream)
		{
			m_pTex[i]->LoadFromTgaStream( pStream );
			ClosePackFileForRead(pStream);
		}

		//Restore work path
		SetCurrentDirectory( workPath );

		if ( !m_pTex[i]->IsCreated() )
			return false;
	}	

	return true;

	FUNCTION_END;
}

/**
 * \brief 绘制效果
 * 
 * 使用引擎中的方法
 * 
 * \param lpd3dDevice : D3D设备
 * \return 
 */
//Render use engine method
HRESULT CParticleSystem::RenderUseEngineMethod( LPDIRECT3DDEVICE8 lpd3dDevice )
{
	FUNCTION_BEGIN;

	if ( !m_bVisible )
		return S_OK;

	for( int i=0;i<m_texNum;i++ )
	{
		if ( !m_pTex[i]->IsCreated() )
		{
			if ( !InitDevice() )
				return E_FAIL;
		}
	}	

	//First we rotate the whole sys
	D3DXMATRIX matRot;
	D3DXMatrixRotationZ( &matRot, m_sysRotSpeed * xtimeGetTime() / 1000.0f );

	HRESULT hr;
	
	CParticle*		pParticle = m_pParticles;
	// Render each particle
	while( pParticle )
	{
		//rotate
		D3DXVECTOR3 vecPos = pParticle->m_d3dvLocation;
		D3DXVec3TransformCoord( &vecPos, &vecPos, &matRot );

		//change particle properties
		//if ( pParticle->m_fAge >= 0.0f )
		//Only render those particles in live range
		bool bNeedRender = true;
		stPointI pt;
		pt.x = (long)( (vecPos*m_sysSizeScale + m_vSystem).x + pParticle->m_offset.x );
		pt.y = (long)( (vecPos*m_sysSizeScale + m_vSystem).y + pParticle->m_offset.y );

		////Use system scale rate
		//pt.x *= m_sysSizeScale;
		//pt.y *= m_sysSizeScale;

		if ( m_bUseOutLiveRange )
		{
			//This time we just consider character move
			if ( pt.x < m_liveRange.left )
				pt.x += m_liveRange.right - m_liveRange.left;

			if ( pt.x > m_liveRange.right )
				pt.x -= m_liveRange.right- m_liveRange.left;

			if ( pt.y > m_liveRange.bottom )
				pt.y -= m_liveRange.bottom - m_liveRange.top;

			/*if ( pt.y < m_liveRange.top )
				pt.y += m_liveRange.bottom - m_liveRange.top;*/

			bNeedRender = true;
		}
		else
		{
			if ( PtInRect( &m_liveRange, pt ) )
				bNeedRender = true;
			else
				bNeedRender = false;
		}

		if ( bNeedRender )
		{
			stRenderInfo ri;
			//ri.blend = Blend_AlphaAdd;
			ri.blend = (BlendType)m_blendType;
			ri.color = D3DCOLOR_COLORVALUE( pParticle->m_d3dcColor.r, 
				pParticle->m_d3dcColor.g, 
				pParticle->m_d3dcColor.b, 
				pParticle->m_fAlpha );

			//Get tex according to sys time(linear interation)
			int texIndex = (int)(pParticle->m_fAge / m_fLife * m_texNum);
			if ( texIndex >= m_texNum )
				texIndex = m_texNum -1;
			ri.tex = m_pTex[texIndex]->GetTex();

			ri.uv = stRectF(0,0,1,1);
			ri.rect.left = pt.x - pParticle->m_fSize/2.0f;
			ri.rect.top = pt.y - pParticle->m_fSize/2.0f * m_sizeHWRate;
			ri.rect.right = ri.rect.left + pParticle->m_fSize;
			ri.rect.bottom = ri.rect.top + pParticle->m_fSize * m_sizeHWRate;

			hr &= SUCCEEDED(GetD3DDevice()->Render(ri));
		}

		//
		pParticle = pParticle->m_pNext;
	}

	return hr;

	FUNCTION_END;
}

/**
 * \brief 定位粒子系统
 * 
 * 
 * 
 * \param vLocation : 位置
 * \return 
 */
//-----------------------------------------------------------------------------
//
//	CParticleSystem::Move
//
//-----------------------------------------------------------------------------
void CParticleSystem::MoveTo(D3DXVECTOR3 vLocation)
{
	FUNCTION_BEGIN;

	m_d3dvPrevLocation = m_d3dvLocation;
	m_d3dvLocation = vLocation;

	FUNCTION_END;
}

/**
 * \brief 系统是否在移动
 * 
 * 
 * 
 * \return 
 */
//-----------------------------------------------------------------------------
//
//	CParticleSystem::IsMoving
//
//-----------------------------------------------------------------------------
bool CParticleSystem::IsMoving()
{
	FUNCTION_BEGIN;
	
	if ( NULL == m_movement )
		return false;		
	
	return m_bIsMoving;

	FUNCTION_END;
}


/**
 * \brief 是否有吸附作用
 * 
 * 
 * 
 * \return 
 */
//-----------------------------------------------------------------------------
//
//	CParticleSystem::IsAttractive
//
//-----------------------------------------------------------------------------
bool CParticleSystem::IsAttractive()
{
	FUNCTION_BEGIN;

	return m_bIsAttractive;

	FUNCTION_END;
}


/**
 * \brief 设置吸附器位置
 * 
 * 
 * 
 * \param vAttraction : 吸附器位置
 * \return 
 */
void CParticleSystem::SetAttractionLocation( D3DXVECTOR3& vAttraction )
{
	FUNCTION_BEGIN;

	m_d3dvAttraction = vAttraction;
	m_bIsAttractive = true;

	FUNCTION_END;
}
//-----------------------------------------------------------------------------
//
//	CParticleSystem::GetLocation
//
//-----------------------------------------------------------------------------
/**
* \brief 获取系统位置
* 
* 
* 
* \param vLocation : 系统位置（返回值）
* \return 
*/
void CParticleSystem::GetLocation(D3DXVECTOR3& vLocation)
{
	FUNCTION_BEGIN;

	vLocation = m_d3dvLocation;

	FUNCTION_END;
}

/**
 * \brief 获取吸附器位置
 * 
 * 
 * 
 * \param vAttractLocation : 吸附器位置（返回值）
 * \return 
 */
// Get attract location
void CParticleSystem::GetAttractLocation( D3DXVECTOR3& vAttractLocation )
{
	FUNCTION_BEGIN;

	if ( m_bIsAttractive )
		vAttractLocation = m_d3dvAttraction;
	else
		vAttractLocation = m_d3dvLocation;

	FUNCTION_END;
}

/**
 * \brief 获取吸附强度
 * 
 * 
 * 
 * \return 吸附强度
 */
// Get attract strength
int CParticleSystem::GetAttractStrength()
{
	FUNCTION_BEGIN;

	return m_AttractStrength;

	FUNCTION_END;
}


/**
 * \brief 获取总的粒子数目
 * 
 * 
 * 
 * \return 总的粒子数目
 */
//-----------------------------------------------------------------------------
//
//	CParticleSystem::NumParticles
//
//-----------------------------------------------------------------------------
unsigned int CParticleSystem::NumParticles()
{
	FUNCTION_BEGIN;

	return m_uParticlesAlive;

	FUNCTION_END;
}


/**
 * \brief 设置系统纹理
 * 
 * 
 * 
 * \param index : 纹理id
 * \return 
 */
//Reset texture
void CParticleSystem::SetTexture( int index )
{
	FUNCTION_BEGIN;

	if ( m_uTextureID == index )
		return;
	
	//first release all texture
	UnInitDevice();
	m_texNum = 1;

	m_uTextureID = index;
	InitDevice();

	FUNCTION_END;
}

/**
 * \brief 设置纹理桢数
 * 
 * 
 * 
 * \param num : 纹理桢数
 * \return 
 */
//Set texture num
void CParticleSystem::SetTextureNum( int num )
{
	FUNCTION_BEGIN;

	//first release old texture
	UnInitDevice();

	m_texNum = num;
	InitDevice();

	FUNCTION_END;
}

void CParticleSystem::ReleaseEmiter()
{
	if ( NULL == m_emiter )
		return;
    
	switch( m_emiter->m_type )
	{
	case PARTICLE_ET_POINT:
		{
			CEmiterPoint* ptEmiter = (CEmiterPoint*)m_emiter;
			delete ptEmiter;
		}
		break;
	case PARTICLE_ET_LINE:
		{
			CEmiterLine* lEmiter = (CEmiterLine*)m_emiter;
			delete lEmiter;
		}
		break;
	case PARTICLE_ET_CIRCLE:
		{
			CEmiterCircle* cEmiter = (CEmiterCircle*)m_emiter;
			delete cEmiter;
		}
		break;
	case PARTICLE_ET_SPHERE:
		{
			CEmiterSphere* sEmiter = (CEmiterSphere*)m_emiter;
			delete sEmiter;
		}
		break;
	case PARTICLE_ET_PLANE:
		{
			CEmiterPlane* pEmiter = (CEmiterPlane*)m_emiter;
			delete pEmiter;
		}
		break;
	case PARTICLE_ET_POINT_LIST:
		{
			CEmiterPointList* plEmiter = (CEmiterPointList*)m_emiter;
			delete plEmiter;
		}
		break;
	default:
		delete m_emiter;
		break;
	}

	m_emiter = NULL;
}

/**
 * \brief 设置发射器
 * 
 * 
 * 
 * \param type : 发射器类型
 * \return 
 */
//Set new emiter
bool CParticleSystem::SetNewEmiter( int type )
{
	FUNCTION_BEGIN;
	
	//Make sure type valid
	if ( (type<0) || (type >= PARTICLE_ET_NUM) )
		return false;
	//whether we really need change
	if ( m_emiter && (type == m_emiter->m_type) )
		return true;
	//release old emiter
	if ( m_emiter )
		ReleaseEmiter();

	//Get new emiter
	switch( type )
	{
	case PARTICLE_ET_POINT:
		m_emiter = new CEmiterPoint;
		break;
	case PARTICLE_ET_LINE:
		m_emiter = new CEmiterLine;
		break;
	case PARTICLE_ET_CIRCLE:
	case PARTICLE_ET_SPHERE:
		m_emiter = new CEmiterCircle;
		break;
	case PARTICLE_ET_PLANE:
		m_emiter = new CEmiterPlane;
		break;
	case PARTICLE_ET_POINT_LIST:
		m_emiter = new CEmiterPointList;
		break;
	}
	
	Play();

	return true;

	FUNCTION_END;
}

/**
 * \brief 设置运动控制器
 * 
 * 
 * 
 * \param type : 类型
 * \return 
 */
//Set new movement
bool CParticleSystem::SetNewMovement( int type )
{
	FUNCTION_BEGIN;
	
	//make sure type valid
	if ( (type<0) || (type >= PARTICLE_MT_NUM) )
		return false;
	//wether we really need change
	if ( m_movement && (type == m_movement->m_type) )
		return true;

	//release old movement
	SAFE_DELETE( m_movement );

	//Get new emiter
	switch( type )
	{
	case PARTICLE_MT_CIRCLE:
		m_movement = new CMovementCircle;
		break;
	case PARTICLE_MT_LINE:
		m_movement = new CMovementLine;
		break;
	case PARTICLE_MT_LINE_BACK:
		m_movement = new CMovementLineBack;
		break;
	case PARTICLE_MT_WAVE:
		m_movement = new CMovementWave;
		break;
	}

	SetMoving( true );

	Play();

	return true;

	FUNCTION_END;
}

/**
 * \brief 获取运动控制器类型
 * 
 * 
 * 
 * \return 运动控制器类型
 */
int CParticleSystem::GetMovementType()
{
	FUNCTION_BEGIN;

	if ( m_movement )
		return m_movement->m_type;
	else
		return PARTICLE_MT_NUM;

	FUNCTION_END;
}

//Set sys size scale rate
/**
* \brief 设置系统缩放大小
* 
* 
* 
* \param scale: 缩放比例
* \return 
*/
void CParticleSystem::SetSizeScale( float sysScale, float sizeScale /*=10.0f*/ )
{
	m_sysSizeScale = sysScale;
	
	m_fSizeStart	*= sizeScale;
	m_fSizeEnd		*= sizeScale;
	m_fSizeVar		*= sizeScale;
}

void CParticleSystem::SetLoopPlayNum( int num /* = 0  */ )
{
	if ( num <= 0.0f )
	{
		m_bLoopPlay = false;
		m_loopPlayNum = 0;
	}
	else
	{
		m_bLoopPlay = true;
		m_loopPlayNum = num;
	}
}