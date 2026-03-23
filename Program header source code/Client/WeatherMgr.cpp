/**
 *\file		WeatherMgr.cpp
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:18:00 中国标准时间
 *\brief	天气管理器
 *
 * 
 *
*/

#include "public.h"
#include "./GameScene.h"
#include "../RenderD3D/include/RenderD3D.h"
#include "../RenderD3D/include/D3DDevice.h"

#include "./particle/ParticleSysMgr.h"
#include "./MainCharacter.h"
#include "./WeatherMgr.h"
#include "../Media/SoundManager.h"

#define MAX_RAIN_LEVEL	5
#define MAX_SNOW_LEVEL	5
#define MAX_CLOUD_LEVEL	5
#define RAIN_LEVEL_SCALE	4
#define START_LEVEL_RATE	0.0f

static const int rainLevel[] = { 0, 50, 80, 110, 140, 170 };
static const int snowLevel[] = { 0, 50, 80, 110, 140, 170 };
static const int cloudLevel[] = { 130, 150, 170, 190, 210 };

static float  WEATHER_SWITCH_TIME = 10.0f;		//second

CWeatherMgr* g_pWeatherMgr = NULL;

/**
 * \brief 获取天气管理器
 * 
 * 
 * 
 * \return 指向天气管理器的指针
 */
CWeatherMgr* GetWeatherMgr()
{
	FUNCTION_BEGIN;

	return g_pWeatherMgr;

	FUNCTION_END;
}

void CWeatherMgr::OpenWeatherEffect()
{
	if ( GetScene()->GetMainCharacter() )
		GetScene()->GetMainCharacter()->OnDayNumChange();
}

void CWeatherMgr::CloseWeatherEffect()
{
	ReleaseAllWeather();
}

/**
* \brief 播放天气音效
* 
* 
* 
* \return 
*/
void CWeatherMgr::PlaySound()
{
	FUNCTION_BEGIN;

	char szFullPath[MAX_PATH];
	//stop cur sound

	//Play new sound
	sprintf( szFullPath, "sound\\other\\%s", m_nextWeatherName );
	GetSoundManager()->Play( szFullPath, SoundType_Circumstance );	//SoundType_Music

	FUNCTION_END;
}

void CWeatherMgr::PlayLightenSound()
{
	FUNCTION_BEGIN;

	//return;
	
	char szFullPath[MAX_PATH];

	sprintf( szFullPath, "sound\\other\\" );
	if ( RANDOM_NUM > -0.5f )		//
		strcat( szFullPath, "weather-thunderl.wav" );
	else
		strcat( szFullPath, "weather-thunderl.wav" );
	
	szFullPath[strlen(szFullPath)] = 0;
	GetSoundManager()->Play( szFullPath,SoundType_Circumstance );
	/*if ( m_pSound )
	{
		m_pSound->SetVolume( (RANDOM_NUM + 2.0f) / 3.0f );
		m_pSound->Play( false );
	}*/

	m_nextPlayLightenSoundTime = m_nextPlayLightenTime + 1.0f;

	FUNCTION_END;
}

/**
 * \brief 构造函数
 * 
 * 
 * 
 * \return 
 */
//Construction
CWeatherMgr::CWeatherMgr()
{
	FUNCTION_BEGIN;

	m_curWeatherType = WETHER_T_NUM;
	m_curWeatherLevel = -1;

	m_nextWeatherType = WETHER_T_NUM;
	m_nextWeatherLevel = -1;

	m_bWaitingPlay = true;
	m_bPoping = false;

	m_fChangeValue = 0.0f;
	m_fChangeLastTime = 0.0f;

	m_curWeather = NULL;
	m_cloud = NULL;


	//Get live range
	m_rcLiveRange.left = 0;
	m_rcLiveRange.right = GetDevice()->GetWidth();
	m_rcLiveRange.top = 0;
	m_rcLiveRange.bottom = GetDevice()->GetHeight();

	g_pWeatherMgr = this;

	FUNCTION_END;
}

/**
 * \brief 析构函数
 * 
 * 
 * 
 * \return 
 */
CWeatherMgr::~CWeatherMgr()
{
	FUNCTION_BEGIN;

	ReleaseAllWeather()	;

	FUNCTION_END;
}

void CWeatherMgr::ReleaseAllWeather()
{
	SAFE_DELETE( m_cloud );

	if ( m_curWeather && ( (m_curWeatherType == WETHER_T_RAIN) || (m_curWeatherType == WETHER_T_SNOW) ) )
	{
		CParticleSystem* pParticleSys = (CParticleSystem*)m_curWeather;
		GetParticleSysMgr()->UnLoadOneParticleSystem( pParticleSys->GetName() );

		if ( m_curWeatherType == WETHER_T_RAIN )
		{
			//unload rain ground and rain lighten effect
			GetParticleSysMgr()->UnLoadOneParticleSystem( "RainGround" );
			GetParticleSysMgr()->UnLoadOneParticleSystem( "LightenEffect" );
		}
	}

	m_curWeather = NULL;

	m_curWeatherType = WETHER_T_NUM;
	m_curWeatherLevel = -1;
}

/**
 * \brief 装载天气效果
 * 
 * 初始化设备
 * 
 * \return 
 */
void CWeatherMgr::LoadCloudEffect()
{
	FUNCTION_BEGIN;

	if ( NULL == m_cloud )
	{
		m_cloud = new CEffectCloud;
		m_cloud->InitDevice();
	}

    PlayCloudEffect();

	FUNCTION_END;
}

/**
 * \brief 卸载天气效果
 * 
 * 
 * 
 * \return 
 */
void CWeatherMgr::UnLoadCloudEffect()
{
	FUNCTION_BEGIN;

	if ( NULL == m_cloud )
		return;

	StopCloudEffect();
	SAFE_DELETE( m_cloud );

	FUNCTION_END;
}

/**
 * \brief 绘制
 * 
 * 
 * 
 * \param fElapsedTime : 持续的时间
 * \return 
 */
HRESULT CWeatherMgr::Render( float fElapsedTime )
{
	FUNCTION_BEGIN;

	//we render rain snow in particle mgr
	if ( m_cloud && (m_curWeatherType == WETHER_T_CLOUD) )
		m_cloud->Render( fElapsedTime );

	/*
	//When cur wether type is rain
	//We random play lighten effect
	if ( m_curWeatherType == WETHER_T_RAIN )
	{
		CParticleSystem* pLightenEffect = GetParticleSysMgr()->FindParticleSys( "LightenEffect" );
		if ( pLightenEffect && !(pLightenEffect->IsPlaying()) )
		{
			if ( xtimeGetTime() > m_nextPlayLightenTime )
			{
				pLightenEffect->Play( pLightenEffect->GetSysLife() );
				m_nextPlayLightenTime = xtimeGetTime() + (1.1f + RANDOM_NUM) * 20000;
				m_nextPlayLightenSoundTime = xtimeGetTime() + 700;
			}
		}

		if ( (xtimeGetTime() > m_nextPlayLightenSoundTime) && (m_nextPlayLightenSoundTime < m_nextPlayLightenTime) )
			PlayLightenSound();
	}
	*/

	return S_OK;

	FUNCTION_END;
}

/**
 * \brief 刷新
 * 
 * 
 * 
 * \return 
 */
HRESULT CWeatherMgr::Update()
{
	FUNCTION_BEGIN;

	if ( m_bWaitingPlay )
		StopCurWeatherEffect();

	if ( m_bPoping )
		PopNextWeather();

	//we update rain snow in particle mgr
	if ( m_cloud && (m_curWeatherType == WETHER_T_CLOUD) )
		m_cloud->Update();

	return S_OK;

	FUNCTION_END;
}

/**
 * \brief 获取雨发射器位置
 * 
 * 
 * 
 * \param vPos : 位置（返回值）
 * \return 
 */
void CWeatherMgr::GetRainLocation( D3DXVECTOR3& vPos )
{
	FUNCTION_BEGIN;

	vPos.x = GetDevice()->GetWidth()/2;
	//vPos.y = GetDevice()->GetHeight()/2;
	vPos.y = -100;
	vPos.z = 0;

	FUNCTION_END;
}

/**
 * \brief 获取雪发射器位置
 * 
 * 
 * 
 * \param vPos : 位置（返回值）
 * \return 
 */
void CWeatherMgr::GetSnowLocation( D3DXVECTOR3& vPos )
{
	FUNCTION_BEGIN;

	vPos.x = GetDevice()->GetWidth()/2;
	//vPos.y = GetDevice()->GetHeight()/2;
	vPos.y = 0;
	vPos.z = 0;

	FUNCTION_END;
}

/**
 * \brief 重设有效绘制区域
 * 
 * 当设备尺寸变化时调用
 * 
 * \param range : 有效绘制区域
 * \return 
 */
void CWeatherMgr::ResetLiveRange( stRectI range )
{
	m_rcLiveRange = range;

	if ( NULL == m_curWeather )
		return;

	//Reset particle sys location
	D3DXVECTOR3 vPos;
	switch( m_curWeatherType )
	{
	case WETHER_T_RAIN:
		{
			//Reset rain ground in game space
			CParticleSystem* rainGround = GetParticleSysMgr()->FindParticleSys( "RainGround" );
			if ( rainGround )
			{
				rainGround->SetPositionInGameSpace( D3DXVECTOR3(m_rcLiveRange.right/2, m_rcLiveRange.bottom/2, 0) );
				rainGround->SetLiveRange( m_rcLiveRange );
			}
			//Reset rain in game space
			m_rcLiveRange.top = -100;
			vPos.x = GetDevice()->GetWidth()/2;
			vPos.y = -100;
			vPos.z = 0;
			((CParticleSystem*)m_curWeather)->SetPositionInGameSpace( vPos );
			((CParticleSystem*)m_curWeather)->SetLiveRange( m_rcLiveRange );
		}
		break;
	case WETHER_T_SNOW:
		{
			vPos.x = GetDevice()->GetWidth()/2;
			vPos.y = 0;
			vPos.z = 0;
			((CParticleSystem*)m_curWeather)->SetPositionInGameSpace( vPos );
			((CParticleSystem*)m_curWeather)->SetLiveRange( m_rcLiveRange );
		}
		break;
	case WETHER_T_CLOUD:
		break;
	}	
}

/**
 * \brief 卸载当前天气效果
 * 
 * 
 * 
 * \return 
 */
void CWeatherMgr::StopCurWeatherEffect()
{
	FUNCTION_BEGIN;

	m_fChangeLastTime += Engine_GetDrawElapsedTime();

	if ( (m_curWeatherType < WETHER_T_NUM) && (m_curWeather != NULL ) )
	{
		switch( m_curWeatherType )
		{
		case WETHER_T_RAIN:
			{
				CParticleSystem* rain = (CParticleSystem*)m_curWeather;
				int rate = rain->GetEmiterRate() + m_fChangeValue * Engine_GetDrawElapsedTime();
				if ( (rate > 0) && (m_fChangeLastTime < WEATHER_SWITCH_TIME) )
				{
					rain->SetEmitRate( rate );
				}
				else
				{
					//Play next
					if ( m_curWeatherType == m_nextWeatherType )
					{
						rain->SetEmitRate( rainLevel[m_nextWeatherLevel] * RAIN_LEVEL_SCALE );
						m_curWeatherLevel = m_nextWeatherLevel;
					}
					else
					{
						//stop rain and play next weather
						GetParticleSysMgr()->UnLoadOneParticleSystem( wetherName[WETHER_T_RAIN] );
						GetParticleSysMgr()->UnLoadOneParticleSystem( "RainGround" );
						GetParticleSysMgr()->UnLoadOneParticleSystem( "LightenEffect" );

						PlayNextWeather();
					}
					
					m_bWaitingPlay = false;
				}
				return;
			}
			break;
		case WETHER_T_SNOW:
			{
				CParticleSystem* snow = (CParticleSystem*)m_curWeather;
				int rate = snow->GetEmiterRate() + m_fChangeValue * Engine_GetDrawElapsedTime();
				//rate = 0;
				if ( (rate >=0) && (m_fChangeLastTime < WEATHER_SWITCH_TIME) )
				{
					snow->SetEmitRate( rate );
					if ( m_nextWeatherType != WETHER_T_SNOW )
						snow->SetUseOutLiveRange( false );
				}
				else
				{
					//Play next
					if ( m_curWeatherType == m_nextWeatherType )
					{
						snow->SetEmitRate( rainLevel[m_nextWeatherLevel] );
						m_curWeatherLevel = m_nextWeatherLevel;
					}
					else
					{
						GetParticleSysMgr()->UnLoadOneParticleSystem( wetherName[WETHER_T_SNOW] );

						PlayNextWeather();
					}

					m_bWaitingPlay = false;
				}
				return;
			}
			break;
		case WETHER_T_CLOUD:
			{
				int density = m_cloud->GetDensity() + m_fChangeValue * Engine_GetDrawElapsedTime();
				if ( (density > 0) && (density <255) && (m_fChangeLastTime < WEATHER_SWITCH_TIME)  )
				{
					m_cloud->SetDensity( density );
				}
				else
				{
					//Play next
					if ( m_curWeatherType == m_nextWeatherType )
					{
						m_cloud->SetDensity( cloudLevel[m_nextWeatherLevel] );
						m_curWeatherLevel = m_nextWeatherLevel;
					}
					else
					{
						UnLoadCloudEffect();
						PlayNextWeather();
					}					
					
					m_bWaitingPlay = false;
				}
				return;
			}
			break;
		}
	}

	FUNCTION_END;
}

/**
 * \brief 播放粒子类型天气
 * 
 * 
 * 
 * \param  : 是否使用有效绘制区域
 * \return 
 */
void CWeatherMgr::PlayParticleTypeWeather( bool bUseOutLiveRange /* = true  */ )
{
	GetParticleSysMgr()->GetActiveParticleSystem()->SetUseOutLiveRange( bUseOutLiveRange );
	GetParticleSysMgr()->GetActiveParticleSystem()->SetLiveRange( m_rcLiveRange );
	GetParticleSysMgr()->GetActiveParticleSystem()->Play();

	m_curWeather = GetParticleSysMgr()->GetActiveParticleSystem();
}

/**
 * \brief 改变当前天气
 * 
 * 
 * 
 * \param type : 类型
 * \param level : 级别
 * \return 
 */
void CWeatherMgr::ChangeWeather( BYTE type, int level, const char* music )
{
	FUNCTION_BEGIN;

	if ( m_curWeather )		//there has already some weather playing
	{
		if ( (m_curWeatherType == type) && (m_curWeatherLevel == level) )
			return;
		else
		{
			switch( m_curWeatherType )
			{
			case WETHER_T_RAIN:
				{
					if ( level > MAX_RAIN_LEVEL )
						level = MAX_RAIN_LEVEL;

					CParticleSystem* rain = (CParticleSystem*)m_curWeather;
					if ( m_curWeatherType == type )
						m_fChangeValue = ( rainLevel[level] * RAIN_LEVEL_SCALE - rain->GetEmiterRate() ) / WEATHER_SWITCH_TIME;
					else
						m_fChangeValue = ( 0 - rain->GetEmiterRate() ) / WEATHER_SWITCH_TIME;

					m_nextWeatherType = type;
					m_nextWeatherLevel = level;
					sprintf( m_nextWeatherName, music );
					m_bWaitingPlay = true;
					//rain->SetEmitRate( rainLevel[level] );
				}
				break;
			case WETHER_T_SNOW:
				{
					if ( level > MAX_SNOW_LEVEL )
						level = MAX_SNOW_LEVEL;

					CParticleSystem* snow = (CParticleSystem*)m_curWeather;
					if ( m_curWeatherType == type )
						m_fChangeValue = ( snowLevel[level] - snow->GetEmiterRate() ) / WEATHER_SWITCH_TIME;
					else
						m_fChangeValue = ( 0 - snow->GetEmiterRate() ) / WEATHER_SWITCH_TIME;

					m_nextWeatherType = type;
					m_nextWeatherLevel = level;
					sprintf( m_nextWeatherName, music );
					m_bWaitingPlay = true;
					//snow->SetEmitRate( snowLevel[level] );
				}
				break;
			case WETHER_T_CLOUD:
				{
					if ( level > MAX_CLOUD_LEVEL )
						level = MAX_CLOUD_LEVEL;
					if ( m_curWeatherType == type )
						m_fChangeValue = ( cloudLevel[level] - m_cloud->GetDensity() ) / WEATHER_SWITCH_TIME;
					else
						m_fChangeValue = ( 0 - m_cloud->GetDensity() ) / WEATHER_SWITCH_TIME;

					m_nextWeatherType = type;
					m_nextWeatherLevel = level;
					sprintf( m_nextWeatherName, music );
					m_bWaitingPlay = true;
					//m_cloud->SetDensity( cloudLevel[level] );
				}
				break;
			}
		}
		m_bPoping = false;
	}
	else							//there has no any weather playing
	{
		m_nextWeatherType = type;
		m_nextWeatherLevel = level;
		sprintf( m_nextWeatherName, music );
		m_fChangeValue = 0;
		m_bWaitingPlay = false;

		PlayNextWeather();
	}

	m_fChangeLastTime = 0.0f;

	FUNCTION_END;
}

void CWeatherMgr::PopNextWeather()
{
	//Add weather level to ask value
	if ( (m_curWeather == NULL) && (m_cloud == NULL) )
		return;

	switch( m_curWeatherType )
	{
	case WETHER_T_RAIN:
		{
			CParticleSystem* rain = (CParticleSystem*)m_curWeather;
			if ( rain->GetEmiterRate() < rainLevel[m_curWeatherLevel] * RAIN_LEVEL_SCALE )
				rain->SetEmitRate( rain->GetEmiterRate() + 3 );
			else
				m_bPoping = false;
		}
		break;
	case WETHER_T_SNOW:
		{
			CParticleSystem* snow = (CParticleSystem*)m_curWeather;
			if ( snow->GetEmiterRate() < snowLevel[m_curWeatherLevel] )
				snow->SetEmitRate( snow->GetEmiterRate() + 1 );
			else
				m_bPoping = false;
		}
		break;
	case WETHER_T_CLOUD:
		{
			if ( m_cloud->GetDensity() < cloudLevel[m_curWeatherLevel] )
				m_cloud->SetDensity( m_cloud->GetDensity() + 1 );
			else
				m_bPoping = false;
		}
		break;	
	}
}

void CWeatherMgr::PlayNextWeather()
{
	m_curWeather = NULL;
	m_curWeatherType = WETHER_T_NUM;
	m_bPoping = false;

	if ( m_nextWeatherType == WETHER_T_NUM )
	{
		ReleaseAllWeather();
		return;
	}

	//Play new
	switch( m_nextWeatherType )
	{
	case WETHER_T_RAIN:
		{
			//Get rain ground
			GetParticleSysMgr()->LoadOneParticleSystem( "RainGround", D3DXVECTOR3(GetDevice()->GetWidth()/2.0f, GetDevice()->GetHeight()/2.0f, 0.0f) );
			if ( NULL ==  GetParticleSysMgr()->FindParticleSys( "RainGround" ) )
				return;

			GetParticleSysMgr()->GetActiveParticleSystem()->SetEmitRate( rainLevel[m_nextWeatherLevel] * 6 );
			GetParticleSysMgr()->GetActiveParticleSystem()->SetRenderWithManager( false );
			PlayParticleTypeWeather();
			
			//Get rain self
			D3DXVECTOR3 vPos;
			GetRainLocation( vPos );
			GetParticleSysMgr()->LoadOneParticleSystem( wetherName[WETHER_T_RAIN], vPos );
			if ( NULL ==  GetParticleSysMgr()->FindParticleSys( wetherName[WETHER_T_RAIN] ) )
				return;
			GetParticleSysMgr()->GetActiveParticleSystem()->SetUseSpark( false );
			GetParticleSysMgr()->GetActiveParticleSystem()->SetEmitRate( rainLevel[m_nextWeatherLevel] * RAIN_LEVEL_SCALE * START_LEVEL_RATE );
			PlayParticleTypeWeather();

			/*
			//Get lighten effect
			GetParticleSysMgr()->LoadOneParticleSystem( "LightenEffect", D3DXVECTOR3(GetDevice()->GetWidth()/2.0f, GetDevice()->GetHeight()/2.0f, 0.0f) );
			if ( NULL == GetParticleSysMgr()->FindParticleSys( "LightenEffect" ) )
				return;

			GetParticleSysMgr()->GetActiveParticleSystem()->SetLoopPlay( false );
			GetParticleSysMgr()->GetActiveParticleSystem()->SetSysLife( GetParticleSysMgr()->GetActiveParticleSystem()->GetSysLife() * 0.3f );
			GetParticleSysMgr()->GetActiveParticleSystem()->SetSizeStart( GetDevice()->GetWidth() );
			GetParticleSysMgr()->GetActiveParticleSystem()->SetSizeEnd( GetDevice()->GetWidth() );
			//GetParticleSysMgr()->GetActiveParticleSystem()->SetEmitRate( 10 );
			GetParticleSysMgr()->GetActiveParticleSystem()->SetSizeVar( 0 );
			m_nextPlayLightenTime = xtimeGetTime() + (1.0f + RANDOM_NUM) * 20000;
			*/
		}			
		break;
	case WETHER_T_SNOW:
		{
			D3DXVECTOR3 vPos;
			GetSnowLocation( vPos );
			GetParticleSysMgr()->LoadOneParticleSystem( wetherName[WETHER_T_SNOW], vPos );
			if ( NULL == GetParticleSysMgr()->FindParticleSys( wetherName[WETHER_T_SNOW] ) )
				return;

			GetParticleSysMgr()->GetActiveParticleSystem()->SetEmitRate( snowLevel[m_nextWeatherLevel] * START_LEVEL_RATE );
			PlayParticleTypeWeather();
		}
		break;
	case WETHER_T_CLOUD:
		{
			//play cloud
			LoadCloudEffect();
			m_cloud->SetDensity( cloudLevel[m_nextWeatherLevel] * START_LEVEL_RATE );		

			m_curWeather = m_cloud;
		}
		break;
	}

	//Play music
	PlaySound();
    
	m_curWeatherType = m_nextWeatherType;
	m_curWeatherLevel = m_nextWeatherLevel;
	sprintf( m_curWeatherName, m_nextWeatherName );

	//m_nextWeatherType = WETHER_T_NUM;
	//m_nextWeatherLevel = -1;

	m_bPoping = true;
}

//void CWeatherMgr::SwitchNewWeather()
//{
//	FUNCTION_BEGIN;
//
//	if ( m_curWeather && (m_curWeatherType == type) )		//this weather is now playing
//	{
//		if ( m_curWeatherLevel == level )
//			return;
//		else
//		{
//			switch( m_curWeatherType )
//			{
//			case WETHER_T_RAIN:
//				{
//					if ( level > MAX_RAIN_LEVEL )
//						return;
//
//					CParticleSystem* rain = (CParticleSystem*)m_curWeather;
//					rain->SetEmitRate( rainLevel[level] );
//				}
//				break;
//			case WETHER_T_SNOW:
//				{
//					if ( level > MAX_SNOW_LEVEL )
//						return;
//
//					CParticleSystem* snow = (CParticleSystem*)m_curWeather;
//					snow->SetEmitRate( snowLevel[level] );
//				}
//				break;
//			case WETHER_T_CLOUD:
//				{
//					if ( level > MAX_CLOUD_LEVEL )
//						return;
//
//					m_cloud->SetDensity( cloudLevel[level] );
//				}
//				break;
//			}
//		}
//	}
//	else								//there has no same weather playing
//	{
//		//Stop cur weather effect
//		StopCurWeatherEffect();
//		//Play new
//		switch( type )
//		{
//		case WETHER_T_RAIN:
//			{
//				if ( level > MAX_RAIN_LEVEL )
//					return;
//
//				//play rain
//				CParticleSystem* rain = GetParticleSysMgr()->FindParticleSys( wetherName[WETHER_T_RAIN] );
//				if ( rain )
//				{
//					rain->SetEmitRate( rainLevel[level] );
//					rain->Play();
//
//					m_curWeather = rain;
//				}
//				else		//load rain effect
//				{
//					//Get rain ground
//					GetParticleSysMgr()->LoadOneParticleSystem( "RainGround", D3DXVECTOR3(GetDevice()->GetWidth()/2.0f, GetDevice()->GetHeight()/2.0f, 0.0f) );
//					GetParticleSysMgr()->GetActiveParticleSystem()->SetEmitRate( rainLevel[level] * 6 );
//					PlayParticleTypeWeather();
//
//					D3DXVECTOR3 vPos;
//					GetRainLocation( vPos );
//					GetParticleSysMgr()->LoadOneParticleSystem( wetherName[WETHER_T_RAIN], vPos );
//					GetParticleSysMgr()->GetActiveParticleSystem()->SetEmitRate( rainLevel[level] * 4 );
//					PlayParticleTypeWeather();					
//				}
//			}			
//			break;
//		case WETHER_T_SNOW:
//			{
//				if ( level > MAX_SNOW_LEVEL )
//					return;
//
//				//play snow
//				CParticleSystem* snow = GetParticleSysMgr()->FindParticleSys( wetherName[WETHER_T_SNOW] );
//				if ( snow )
//				{
//					snow->SetEmitRate( snowLevel[level] );
//					snow->Play();
//
//					m_curWeather = snow;
//				}
//				else		//load snow effect
//				{
//					D3DXVECTOR3 vPos;
//					GetSnowLocation( vPos );
//					GetParticleSysMgr()->LoadOneParticleSystem( wetherName[WETHER_T_SNOW], vPos );
//					GetParticleSysMgr()->GetActiveParticleSystem()->SetEmitRate( snowLevel[level] );
//					PlayParticleTypeWeather();
//				}
//			}
//			break;
//		case WETHER_T_CLOUD:
//			{
//				if ( level > MAX_CLOUD_LEVEL )
//					return;
//
//				//play cloud
//				LoadCloudEffect();
//				m_cloud->SetDensity( cloudLevel[level] );			
//
//				m_curWeather = m_cloud;
//			}
//			break;
//		}
//
//		m_curWeatherType = type;
//	}
//
//	FUNCTION_END;
//}

