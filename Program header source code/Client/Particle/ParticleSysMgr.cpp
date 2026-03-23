/**
 *\file		ParticleSysMgr.cpp
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:20:00 中国标准时间
 *\brief	粒子系统管理器
 *
 * 
 *
*/

#include "public.h"
#include "../GameScene.h"
#include "../../RenderD3D/include/RenderD3D.h"
#include "../../RenderD3D/include/D3DDevice.h"
#include "../../RenderD3D/include/TextureManager.h"
#include "../../RenderD3D/include/CustomTexture.h"

#include "./ParticleSysMgr.h"
#include "./EffectStruct.h"

#define RENDER_USE_ENGINE_METHOD


CParticleSysManager*	g_ParticleSysMgr = NULL;

/**
 * \brief 获取粒子系统管理器
 * 
 * 
 * 
 * \return 粒子系统管理器指针
 */
CParticleSysManager*	GetParticleSysMgr()
{
	FUNCTION_BEGIN;

	return g_ParticleSysMgr;

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
CParticleSysManager::CParticleSysManager()
{
	FUNCTION_BEGIN;

	m_offset = stPointI(0,0);
	
	m_aParticleSystem.clear();
	m_activeParticleSystem = NULL;
	m_usage = PSM_USE_GAME;
	g_ParticleSysMgr = this;

	FUNCTION_END;
}

int CParticleSysManager::GetTotalSysNum()
{
	if ( m_aParticleSystem.empty() )
        return 0;

	return m_aParticleSystem.size();
}

/**
 * \brief 析构函数
 * 
 * 
 * 
 * \return 
 */
//Destruction
CParticleSysManager::~CParticleSysManager()
{
	FUNCTION_BEGIN;

	DestroyParticleSys();

	FUNCTION_END;
}

/**
 * \brief 装载一个粒子效果
 * 
 * 
 * 
 * \param pName : 效果名称
 * \param  : 位置
 * \return 
 */
//Load one particle system
HRESULT CParticleSysManager::LoadOneParticleSystem( const char* pName, const D3DXVECTOR3& vPosition /* = D3DXVECTOR3 */ )
{
	FUNCTION_BEGIN;

	if ( !m_aParticleSystem.empty() && m_aParticleSystem.size() >= MAX_PARTICLE_SYS_NUM )
		return E_FAIL;

	switch( m_usage )
	{
	case PSM_USE_EDIT:
		{
			//if ( strcmp( pName, "sample" ) == 0 )		//Load default effect
			if ( 0 )
			{
				CEmiterPoint* emiter = new CEmiterPoint();
				CParticleSysMove* movement = NULL;
				//Construct new particle system
				CParticleSystem* pParticleSystem = new CParticleSystem( emiter, movement );
				//we release memory of emiter in destruction of CParticleSystem
				emiter = NULL;

				//Get effect factor
				pParticleSystem->m_d3dcColorStart.r = 255;
				pParticleSystem->m_d3dcColorStart.g = 0;
				pParticleSystem->m_d3dcColorStart.b = 0;

				pParticleSystem->m_d3dcColorVar.r = 0;
				pParticleSystem->m_d3dcColorVar.g = 0;
				pParticleSystem->m_d3dcColorVar.b = 0;

				pParticleSystem->m_d3dcColorEnd.r = 100;
				pParticleSystem->m_d3dcColorEnd.g = 0;
				pParticleSystem->m_d3dcColorEnd.b = 0;

				pParticleSystem->m_fAlphaStart = 255;
				pParticleSystem->m_fAlphaVar = 0;
				pParticleSystem->m_fAlphaEnd = 0;

				pParticleSystem->m_fSizeStart = 100;
				pParticleSystem->m_fSizeVar = 0;
				pParticleSystem->m_fSizeEnd = 20;

				pParticleSystem->m_fSpeed = 100;
				pParticleSystem->m_fSpeedVar = 0;

				pParticleSystem->m_fLife = 5;
				pParticleSystem->m_fLifeVar = 0;

				pParticleSystem->m_fGravityStart = 0;
				pParticleSystem->m_fGravityVar = 0;
				pParticleSystem->m_fGravityEnd = 0;

				pParticleSystem->m_uParticlesPerSec = 50;

				pParticleSystem->m_bIsMoving = false;
				pParticleSystem->m_bIsAttractive = false;
				pParticleSystem->m_AttractStrength = 50;

				pParticleSystem->m_uTextureID = 16;
				pParticleSystem->m_vSystem = D3DXVECTOR3(640.0f,512.0f,0.0f);
				pParticleSystem->m_d3dvAttraction = D3DXVECTOR3(0.0f,0.0f,0.0f);

				//moving about
				pParticleSystem->m_uMovingType = PARTICLE_MT_NUM;

				//Emiter about
				pParticleSystem->m_uEmiterType = PARTICLE_ET_POINT;
				pParticleSystem->m_fStartTime = 0.0f;

				//Spark about
				pParticleSystem->m_bNeedSpark = false;

				//system life
				pParticleSystem->m_sysLife = 10;		//second

				//Get name
				strcpy( pParticleSystem->m_name, pName );

				//Get blend type
				pParticleSystem->m_blendType = Blend_AlphaAdd;

				//Get suppress aobut
				pParticleSystem->m_fSuppressTimeStart = 0.0f;
				pParticleSystem->m_fSuppressTimeEnd = 0.0f;

				//End load

				//Push to manager
				m_aParticleSystem.push_back( pParticleSystem );

				m_activeParticleSystem = pParticleSystem;

				m_activeParticleSystem->InitDevice();
			}
			else
			{
				FILE* stream;

				if ( (stream = fopen( pName, "rb" )) == NULL )
				{
					char name[MAX_PATH];
					char* pWorkDir;
					GetCurrentDirectory( MAX_PATH, name );
					pWorkDir = strrchr( name, '\\' );
					if ( pWorkDir )
					{
						int pos = (int)(pWorkDir-name) + 1;
						name[pos] = 0;
						strcat( name, "ParticleEditer\\Effect\\" );
						strcat( name, pName );
						strcat( name, ".psf");

						if ( (stream = fopen( name, "rb" )) == NULL )
							return E_FAIL;
					}
					else
						return E_FAIL;
					//sprintf( name, "F:\\Project\\MMORPG\\main\\ParticleEditer\\Effect\\%s.psf", pName );					
				}

				//Get effect
				stParticleEffect effect;
				fread( &effect, sizeof(stParticleEffect), 1, stream );

				//Get emiter
				CParticleEmiter* emiter = NULL;
				switch( effect.mEmiterType )
				{
				case PARTICLE_ET_POINT:
					emiter = new CEmiterPoint();
					fread( emiter, sizeof(CEmiterPoint), 1, stream );
					break;
				case PARTICLE_ET_LINE:
					emiter = new CEmiterLine();
					fread( emiter, sizeof(CEmiterLine), 1, stream );
					break;
				case PARTICLE_ET_CIRCLE:
				case PARTICLE_ET_SPHERE:
					emiter = new CEmiterCircle();
					fread( emiter, sizeof(CEmiterCircle), 1, stream );
					break;
				case PARTICLE_ET_PLANE:
					emiter = new CEmiterPlane();
					fread( emiter, sizeof(CEmiterPlane), 1, stream );
					break;	
				case PARTICLE_ET_POINT_LIST:
					CEmiterPointList* plEmiter = new CEmiterPointList;
					fread( plEmiter, sizeof(CEmiterPointList), 1, stream );
					//Get point list
					int size = plEmiter->GetSize();
					if ( size > 0 )
					{
						//Allocate mem
						plEmiter->AllocateMem( size );
						stPointI* ptList = plEmiter->GetPointList();
						if ( ptList )
							fread( ptList, sizeof(stPointI), size, stream );
					}
					emiter = plEmiter;
					break;
				}

				//Get movement if is moving
				CParticleSysMove* movement = NULL;
				if ( effect.mIsMoving )
				{
					switch( effect.mMovementType )
					{
					case PARTICLE_MT_CIRCLE:
						movement = new CMovementCircle();
						fread( movement, sizeof(CMovementCircle), 1, stream );
						break;
					case PARTICLE_MT_LINE:
						movement = new CMovementLine();
						fread( movement, sizeof(CMovementLine), 1, stream );
						break;
					case PARTICLE_MT_LINE_BACK:
						movement = new CMovementLineBack();
						fread( movement, sizeof(CMovementLineBack), 1, stream );
						break;
					case PARTICLE_MT_WAVE:
						movement = new CMovementWave();
						fread( movement, sizeof(CMovementWave), 1, stream );
						break;
					}
				}

				//Get ADDITION
				//other force
				float otherForceStrength = 0;
				int otherForceDir = OTHER_FD_NUM;
				//blend type
				BYTE blendType = Blend_AlphaAdd;
				//suppress about
				float suppressTimeStart = 0.0f;
				float suppressTimeEnd = 0.0f;
				//rotate speed
				float rotSpeed = 0.0f;
				//texture num
				int texNum = 1;
				//size stretch
				float sizeHWRate = 1.0f;
				//Get bomb detail info
				CParticleSystem::stBombData bombData;				

				if ( !feof( stream ) )
				{
					fread( &otherForceStrength, sizeof(float), 1, stream );
					fread( &otherForceDir, sizeof(int), 1, stream );
					
					fread( &blendType, sizeof(BYTE), 1, stream );
					
					fread( &suppressTimeStart, sizeof(float), 1, stream );
					fread( &suppressTimeEnd, sizeof(float), 1, stream );

					//Get rotate speed
					fread( &rotSpeed, sizeof(float), 1, stream );

					//Get texture num
					fread( &texNum, sizeof(int), 1, stream );

					//Get size stretch
					fread( &sizeHWRate, sizeof(float), 1, stream );

					//Get bomb data
					fread( &bombData, sizeof(CParticleSystem::stBombData), 1, stream );
				}				

				fclose( stream );

				//Construct new particle system
				CParticleSystem* pParticleSystem = new CParticleSystem( emiter, movement );
				//we release memory of emiter in destruction of CParticleSystem
				emiter = NULL;
				movement = NULL;

				//Get effect factor
				pParticleSystem->m_d3dcColorStart.r = effect.mColorStartR;
				pParticleSystem->m_d3dcColorStart.g = effect.mColorStartG;
				pParticleSystem->m_d3dcColorStart.b = effect.mColorStartB;

				pParticleSystem->m_d3dcColorVar.r = effect.mColorVarR;
				pParticleSystem->m_d3dcColorVar.g = effect.mColorVarG;
				pParticleSystem->m_d3dcColorVar.b = effect.mColorVarB;

				pParticleSystem->m_d3dcColorEnd.r = effect.mColorEndR;
				pParticleSystem->m_d3dcColorEnd.g = effect.mColorEndG;
				pParticleSystem->m_d3dcColorEnd.b = effect.mColorEndB;

				pParticleSystem->m_fAlphaStart = effect.mAlphaStart;
				pParticleSystem->m_fAlphaVar = effect.mAlphaVar;
				pParticleSystem->m_fAlphaEnd = effect.mAlphaEnd;

				pParticleSystem->m_fSizeStart = effect.mSizeStart;
				pParticleSystem->m_fSizeVar = effect.mSizeVar;
				pParticleSystem->m_fSizeEnd = effect.mSizeEnd;

				pParticleSystem->m_fSpeed = effect.mSpeed;
				pParticleSystem->m_fSpeedVar = effect.mSpeedVar;

				pParticleSystem->m_fLife = effect.mLife;
				pParticleSystem->m_fLifeVar = effect.mLifeVar;

				pParticleSystem->m_fGravityStart = effect.mGravityStart;
				pParticleSystem->m_fGravityVar = effect.mGravityVar;
				pParticleSystem->m_fGravityEnd = effect.mGravityEnd;

				pParticleSystem->m_uParticlesPerSec = effect.mParticlePerSec;

				pParticleSystem->m_bIsMoving = effect.mIsMoving;
				pParticleSystem->m_bIsAttractive = effect.mIsAttractive;
				pParticleSystem->m_AttractStrength = effect.mAttractStrength;

				pParticleSystem->m_uTextureID = effect.mTextureID;

				//pParticleSystem->m_vSystem = D3DXVECTOR3(640.0f,-100.0f,0.0f);
				pParticleSystem->m_vSystem = D3DXVECTOR3(512.0f,389.0f,0.0f);

				pParticleSystem->m_d3dvAttraction = D3DXVECTOR3(0.0f,0.0f,0.0f);

				//moving about
				pParticleSystem->m_uMovingType = effect.mMovementType;

				//Emiter about
				pParticleSystem->m_uEmiterType = effect.mMovementType;
				pParticleSystem->m_fStartTime = 0.0f;

				//Spark about
				pParticleSystem->m_bNeedSpark = effect.mSpark;

				//system life
				pParticleSystem->m_sysLife = effect.mSysLife;		//second

				//Get name
				strcpy( pParticleSystem->m_name, pName );

				//Get Addition
				pParticleSystem->m_fOtherForce = otherForceStrength;
				pParticleSystem->m_OtherForceDir = otherForceDir;

				//get blend type
				pParticleSystem->m_blendType = blendType;

				//Get suppress about
				pParticleSystem->m_fSuppressTimeStart = suppressTimeStart;
				pParticleSystem->m_fSuppressTimeEnd = suppressTimeEnd;

				//Get rotate speed
				pParticleSystem->m_sysRotSpeed = rotSpeed;

				//Get texture num
				pParticleSystem->m_texNum = texNum;

				//Get size height/width rate
				pParticleSystem->m_sizeHWRate = sizeHWRate;

				//Set bomb data
				pParticleSystem->m_bomb = bombData;

				//Set loop play false
				//pParticleSystem->m_bLoopPlay = false;

				//End load

				//Push to manager
				m_aParticleSystem.push_back( pParticleSystem );

				m_activeParticleSystem = pParticleSystem;

				m_activeParticleSystem->InitDevice();
			}
		}
		break;
	case PSM_USE_GAME:
		{
			char szFileName[MAX_PATH];
			//soke 粒子效果文件存放位置
			sprintf( szFileName,"datas\\particle\\SampleEffect\\%s.psf",pName );
			Stream* pStream = OpenPackFileForRead("data\\datas.pak",szFileName);		//
			if(!pStream) 
			{
				Engine_WriteLogF( "不能打开粒子效果文件 %s",pName );
				return E_FAIL;
			}

			//Read from stream
			//Get effect
			stParticleEffect effect;
			pStream->read( &effect, sizeof(stParticleEffect) );

			//Get emiter
			CParticleEmiter* emiter = NULL;
			switch( effect.mEmiterType )
			{
			case PARTICLE_ET_POINT:
				emiter = new CEmiterPoint();
				pStream->read( emiter, sizeof(CEmiterPoint) );
				break;
			case PARTICLE_ET_LINE:
				emiter = new CEmiterLine();
				pStream->read( emiter, sizeof(CEmiterLine) );
				break;
			case PARTICLE_ET_CIRCLE:
			case PARTICLE_ET_SPHERE:
				emiter = new CEmiterCircle();
				pStream->read( emiter, sizeof(CEmiterCircle) );
				break;
			case PARTICLE_ET_PLANE:
				emiter = new CEmiterPlane();
				pStream->read( emiter, sizeof(CEmiterPlane) );
				break;
			case PARTICLE_ET_POINT_LIST:
				CEmiterPointList* plEmiter = new CEmiterPointList;
				pStream->read( plEmiter, sizeof(CEmiterPointList) );
				//Get point list
				int size = plEmiter->GetSize();
				if ( size > 0 )
				{
					//Allocate mem
					plEmiter->AllocateMem( size );
					stPointI* ptList = plEmiter->GetPointList();
					if ( ptList )
						pStream->read( ptList, sizeof(stPointI)*size );
				}
				emiter = plEmiter;
				break;
			}

			//Get movement if is moving
			CParticleSysMove* movement = NULL;
			if ( effect.mIsMoving )
			{
				switch( effect.mMovementType )
				{
				case PARTICLE_MT_CIRCLE:
					movement = new CMovementCircle();
					pStream->read( movement, sizeof(CMovementCircle) );
					break;
				case PARTICLE_MT_LINE:
					movement = new CMovementLine();
					pStream->read( movement, sizeof(CMovementLine) );
					break;
				case PARTICLE_MT_LINE_BACK:
					movement = new CMovementLineBack();
					pStream->read( movement, sizeof(CMovementLineBack) );
					break;
				case PARTICLE_MT_WAVE:
					movement = new CMovementWave();
					pStream->read( movement, sizeof(CMovementWave) );
					break;
				}
			}

			//Construct new particle system
			CParticleSystem* pParticleSystem = new CParticleSystem( emiter, movement );
			//we release memory of emiter in destruction of CParticleSystem
			emiter = NULL;
			movement = NULL;

			//Get ADDITION
			//other force
			float otherForceStrength = 0;
			int otherForceDir = OTHER_FD_NUM;
			//blend type
			BYTE blendType = Blend_AlphaAdd;
			//suppress about
			float suppressTimeStart = 0.0f;
			float suppressTimeEnd = 0.0f;
			//rotate speed
			float rotSpeed = 0.0f;
			//texture num
			int texNum = 1;
			//size rate
			float sizeHWRate = 1.0f;
			//Bomb data
			CParticleSystem::stBombData bombData;


			if ( pStream->getStatus() != File::EOS )
			{
				pStream->read( &otherForceStrength, sizeof(float) );
				pStream->read( &otherForceDir, sizeof(int) );
			
				pStream->read( &blendType, sizeof(BYTE) );

				pStream->read( &suppressTimeStart, sizeof(float) );
				pStream->read( &suppressTimeEnd, sizeof(float) );

				//Get rotate speed
				pStream->read( &rotSpeed, sizeof(float) );

				//Get texture num
				pStream->read( &texNum, sizeof(int) );

				//Get size rate
				pStream->read( &sizeHWRate, sizeof(float) );

				//Get bomb data
				pStream->read( &bombData, sizeof(CParticleSystem::stBombData) );
			}						
			
			ClosePackFileForRead( pStream );

			//Get effect factor
			pParticleSystem->m_d3dcColorStart.r = effect.mColorStartR;
			pParticleSystem->m_d3dcColorStart.g = effect.mColorStartG;
			pParticleSystem->m_d3dcColorStart.b = effect.mColorStartB;

			pParticleSystem->m_d3dcColorVar.r = effect.mColorVarR;
			pParticleSystem->m_d3dcColorVar.g = effect.mColorVarG;
			pParticleSystem->m_d3dcColorVar.b = effect.mColorVarB;

			pParticleSystem->m_d3dcColorEnd.r = effect.mColorEndR;
			pParticleSystem->m_d3dcColorEnd.g = effect.mColorEndG;
			pParticleSystem->m_d3dcColorEnd.b = effect.mColorEndB;

			pParticleSystem->m_fAlphaStart = effect.mAlphaStart;
			pParticleSystem->m_fAlphaVar = effect.mAlphaVar;
			pParticleSystem->m_fAlphaEnd = effect.mAlphaEnd;

			pParticleSystem->m_fSizeStart = effect.mSizeStart;
			pParticleSystem->m_fSizeVar = effect.mSizeVar;
			pParticleSystem->m_fSizeEnd = effect.mSizeEnd;

			pParticleSystem->m_fSpeed = effect.mSpeed;
			pParticleSystem->m_fSpeedVar = effect.mSpeedVar;

			pParticleSystem->m_fLife = effect.mLife;
			pParticleSystem->m_fLifeVar = effect.mLifeVar;

			pParticleSystem->m_fGravityStart = effect.mGravityStart;
			pParticleSystem->m_fGravityVar = effect.mGravityVar;
			pParticleSystem->m_fGravityEnd = effect.mGravityEnd;

			pParticleSystem->m_uParticlesPerSec = effect.mParticlePerSec;

			pParticleSystem->m_bIsMoving = effect.mIsMoving;
			pParticleSystem->m_bIsAttractive = effect.mIsAttractive;
			pParticleSystem->m_AttractStrength = effect.mAttractStrength;

			pParticleSystem->m_uTextureID = effect.mTextureID;
			//pParticleSystem->m_vSystem = D3DXVECTOR3(500.0f,400.0f,0.0f);				//500,600,1.0f
			pParticleSystem->m_d3dvAttraction = D3DXVECTOR3(0.0f,0.0f,0.0f);

			//moving about
			pParticleSystem->m_uMovingType = effect.mMovementType;

			//Emiter about
			pParticleSystem->m_uEmiterType = effect.mMovementType;
			pParticleSystem->m_fStartTime = 0.0f;

			//Spark about
			pParticleSystem->m_bNeedSpark = effect.mSpark;

			//system life
			pParticleSystem->m_sysLife = effect.mSysLife;		//second

			//system pos
			pParticleSystem->m_vSystem = vPosition;

			//Get name
			strcpy( pParticleSystem->m_name, pName );
			
			//Addition 
			//Add other force
			pParticleSystem->m_fOtherForce = otherForceStrength;
			pParticleSystem->m_OtherForceDir = otherForceDir;

			//Get blend type
			pParticleSystem->m_blendType = blendType;

			//Get suppress about
			pParticleSystem->m_fSuppressTimeStart = suppressTimeStart;
			pParticleSystem->m_fSuppressTimeEnd = suppressTimeEnd;

			//Get rotate speed
			pParticleSystem->m_sysRotSpeed = rotSpeed;

			//Get texture num
			pParticleSystem->m_texNum = texNum;

			//Get height/width rate
			pParticleSystem->m_sizeHWRate = sizeHWRate;

			//Set bomb data
			pParticleSystem->m_bomb = bombData;

			//End load

			//Before we push this particle sys to list table
			//We set an identical id to sys
			pParticleSystem->SetThisID( GetTotalSysNum() + 1 );

			//Push to manager
			m_aParticleSystem.push_back( pParticleSystem );

			m_activeParticleSystem = pParticleSystem;

			m_activeParticleSystem->InitDevice();
		}
		break;
	}	

	return S_OK;

	FUNCTION_END;
}

/**
 * \brief 搜索粒子效果
 * 
 * 
 * 
 * \param pName : 要搜索的粒子效果名称
 * \return 找到的粒子系统指针
 */
//Find one sys
CParticleSystem* CParticleSysManager::FindParticleSys( const char* pName )
{
	FUNCTION_BEGIN;

	tListParticleSys::iterator	itor;
	for( itor = m_aParticleSystem.begin(); itor != m_aParticleSystem.end(); itor++ )
	{
		if ( strcmp( pName, (*itor)->GetName() ) == 0 )
			return *itor;
	}

	return NULL;

	FUNCTION_END;
}

CParticleSystem* CParticleSysManager::FindParticleSysByID( const DWORD& id )
{
	FUNCTION_BEGIN;

	if ( id > GetTotalSysNum() )
		return NULL;

	tListParticleSys::iterator	itor;
	for( itor = m_aParticleSystem.begin(); itor != m_aParticleSystem.end(); itor++ )
	{
		if ( (*itor)->GetThisID() == id )
			return *itor;
	}

	return NULL;

	FUNCTION_END;
}

/**
 * \brief 搜索粒子效果
 * 
 * 
 * 
 * \param pName : 要搜索的粒子效果名称
 * \return 
 */
//Find sys iterator
CParticleSysManager::tListParticleSys::iterator CParticleSysManager::FindParticleSysItByName( const char* pName )
{
	FUNCTION_BEGIN;
	
	tListParticleSys::iterator itor;
	for( itor = m_aParticleSystem.begin(); itor != m_aParticleSystem.end(); itor++ )
	{
		if ( strcmp( pName, (*itor)->GetName() ) == 0 )
			return itor;
	}

	return m_aParticleSystem.end();

	FUNCTION_END;
}

/**
 * \brief 初始化所有粒子系统设备
 * 
 * 
 * 
 * \return 
 */
//Init all particle system device
HRESULT CParticleSysManager::InitAllParticleSysDevice()
{
	FUNCTION_BEGIN;

	tListParticleSys::iterator		itor;
	for( itor = m_aParticleSystem.begin(); itor != m_aParticleSystem.end(); itor++ )
		(*itor)->InitDevice();

	return S_OK;

	FUNCTION_END;
}	

/**
 * \brief 卸载一个粒子系统
 * 
 * 
 * 
 * \param pName : 要卸载的粒子系统名称
 * \return 
 */
//Delete one sys
HRESULT CParticleSysManager::UnLoadOneParticleSystem( const char* pName )
{
	FUNCTION_BEGIN;

	tListParticleSys::iterator itor = FindParticleSysItByName( pName );
	if ( itor != m_aParticleSystem.end() )
	{
		(*itor)->Stop();
		SAFE_DELETE( *itor );
		SimpleRemoveFromArray( m_aParticleSystem, itor - m_aParticleSystem.begin() );

		return S_OK;
	}	

	return E_FAIL;

	FUNCTION_END;
}

/**
 * \brief 停止一个粒子系统
 * 
 * 
 * 
 * \param pName : 要停止的粒子系统名称
 * \return 
 */
//Stop one particle
HRESULT CParticleSysManager::StopParticleSystem( const char* pName )
{
	FUNCTION_BEGIN;

	if ( CParticleSystem* pSystem = FindParticleSys( pName ) )
	{
		pSystem->Stop();
		return S_OK;
	}

	return E_FAIL;

	FUNCTION_END;
}

/**
 * \brief 暂停一个粒子系统
 * 
 * 
 * 
 * \param pName : 要暂停的粒子系统名称
 * \return 
 */
//Pause one system
HRESULT CParticleSysManager::PauseParticleSystem( const char* pName )
{
	FUNCTION_BEGIN;

	if ( CParticleSystem* pSystem = FindParticleSys( pName ) )
	{
		pSystem->Pause();
		return S_OK;
	}

	return E_FAIL;

	FUNCTION_END;
}

/**
 * \brief 播放一个粒子系统
 * 
 * 
 * 
 * \param pName : 要播放的粒子系统名称
 * \param  : 要播放的粒子系统生命
 * \return 
 */
//Play one system
HRESULT CParticleSysManager::PlayParticleSystem( const char* pName, const float& pSysLife /* = -1 */ )
{
	FUNCTION_BEGIN;

	if ( CParticleSystem* pSystem = FindParticleSys( pName ) )
	{
		pSystem->Play( pSysLife );
		return S_OK;
	}

	return E_FAIL;

	FUNCTION_END;
}

/**
 * \brief 卸载所有的粒子系统
 * 
 * 
 * 
 * \return 
 */
//Destroy all system
void CParticleSysManager::DestroyParticleSys()
{
	FUNCTION_BEGIN;

	tListParticleSys::iterator		itor;
	for( itor = m_aParticleSystem.begin(); itor != m_aParticleSystem.end(); itor++ )
		SAFE_DELETE( *itor );
	m_aParticleSystem.clear();

	FUNCTION_END;
}

/**
 * \brief 绘制所有粒子效果
 * 
 * 
 * 
 * \return 
 */
//Render
HRESULT CParticleSysManager::Render()
{
	FUNCTION_BEGIN;

	if( !GetD3DDevice()->TestCooperativeLevel() )
		return E_FAIL;

	//Render particle system list
	tListParticleSys::iterator		itor;
	for( itor = m_aParticleSystem.begin(); itor != m_aParticleSystem.end(); itor++ )
	{
		//we render "RainGround" at other location
		if ( (*itor)->IsRenderWithManager() )
			//
			(*itor)->RenderUseEngineMethod( NULL );
			//(*itor)->RenderUseEngineMethod( GetD3DDevice()->GetDevice() );
	}

	return S_OK;

	FUNCTION_END;
}

/**
 * \brief 刷新所有的粒子效果
 * 
 * 
 * 
 * \param fTime : 时间
 * \return 返回值的描述
 */
//Update
HRESULT CParticleSysManager::Update( const float& fTime )
{
	FUNCTION_BEGIN;

	if ( m_usage == PSM_USE_GAME )
		UpdateCharacterMovingOffset();

	tListParticleSys::iterator		itor;
	for( itor = m_aParticleSystem.begin(); itor != m_aParticleSystem.end(); itor++ )
	{
		if ( (*itor)->NeedUseCharacterOffset() )
			(*itor)->Update( fTime, m_offset );
		else
			(*itor)->Update( fTime, stPointF(0.0f, 0.0f) );

		if ( !(*itor)->IsPlaying() )
		{
			if ( (*itor)->IsHasChild() )
			{
				D3DXVECTOR3 pos;
				char name[MAX_PATH];
				sprintf( name, (*itor)->GetChildName() );
				(*itor)->GetLocation( pos );
				
				SAFE_DELETE( *itor );
				SimpleRemoveFromArray( m_aParticleSystem, itor - m_aParticleSystem.begin() );

				LoadOneParticleSystem( name, pos );
			}
			else
			{
				SAFE_DELETE( *itor );
				SimpleRemoveFromArray( m_aParticleSystem, itor - m_aParticleSystem.begin() );
			}  		

			break;
		}
	}

	return S_OK;

	FUNCTION_END;
}

/**
 * \brief 刷新人物移动造成的位置偏移
 * 
 * 
 * 
 * \return 
 */
//get character moving offset
void CParticleSysManager::UpdateCharacterMovingOffset()
{
	FUNCTION_BEGIN;

	//for each particle system
	//we add an offset

	static stPointI offsetOld = GetMapScene()->GetPixelOffset();

	m_offset = offsetOld - GetMapScene()->GetPixelOffset();
	
	offsetOld = GetMapScene()->GetPixelOffset();

	//TRACE( "Character moving offset::x=%d****y=%d***\n", m_offset.x, m_offset.y );

	FUNCTION_END;
}

/**
 * \brief 获取列表中的粒子效果数
 * 
 * 
 * 
 * \return 列表中的粒子效果数
 */
// Get particle sys num
int CParticleSysManager::GetParticleSysNum()
{
	FUNCTION_BEGIN;

	if ( m_aParticleSystem.empty() )
		return 0;

	return m_aParticleSystem.size();

	FUNCTION_END;
}

/**
 * \brief 获取所有粒子数目
 * 
 * 
 * 
 * \return 所有的粒子数目
 */
//Get current total alive particles num
unsigned int CParticleSysManager::GetTotalParticleNum()
{
	FUNCTION_BEGIN;

	unsigned int num = 0;
	tListParticleSys::iterator		itor;
	for( itor = m_aParticleSystem.begin(); itor != m_aParticleSystem.end(); itor++ )
		num += (*itor)->NumParticles();

	return num;

	FUNCTION_END;
}

