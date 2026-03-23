/**
 *\file		ParticleSysMgr.h
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:21:00 中国标准时间
 *\brief	粒子系统管理器
 *
 * 
 *
*/

#ifndef __PARTICLE_SYS_MANAGER
#define __PARTICLE_SYS_MANAGER

#pragma once

#include "./ParticleSystem.h"


#define MAX_PARTICLE_SYS_NUM	100

enum enumParticleSysMgrUsage
{
	PSM_USE_GAME,
	PSM_USE_EDIT,
	PSM_USE_NUM,
};

/**
 * \brief 粒子系统管理
 * 
 * 
 * 
 */
class CParticleSysManager
{
public:
	CParticleSysManager();
	~CParticleSysManager();
	
	HRESULT InitAllParticleSysDevice();

	//Load particle sys from file

	HRESULT LoadOneParticleSystem(const char* pName, const D3DXVECTOR3& vPosition = D3DXVECTOR3(0,0,0) );
	HRESULT UnLoadOneParticleSystem(const char* pName);
	
	//Find particle sys by name
	CParticleSystem*	FindParticleSys( const char* pName );
	CParticleSystem*	FindParticleSysByID( const DWORD& id );

	//Stop and clean particle system
	HRESULT StopParticleSystem(const char* pName);
	//Just pause particle update
	HRESULT PauseParticleSystem(const char* pName);
	//Play particle system
	HRESULT PlayParticleSystem(const char* pName, const float& pSysLife = -1);
	
	/**
	* \brief 设置使用类型
	* 
	* 
	* 
	* \param useType: 使用类型
	* \return 
	*/
	void SetUsageType( enumParticleSysMgrUsage useType ) { m_usage = useType; }
	//Render
	HRESULT Render();
	HRESULT Update(const float& fTime);

	//get character moving offset
	void UpdateCharacterMovingOffset();

	//
	int GetParticleSysNum();
	unsigned int GetTotalParticleNum();

	//Get active sys
	/**
	* \brief 获取当前粒子系统
	* 
	* 
	* 
	* \param lamda: 要获取的粒子系统名字
	* \return 当前粒子系统指针
	*/
	CParticleSystem* GetActiveParticleSystem() { return m_activeParticleSystem; }

	//Destroy all sys
	void DestroyParticleSys();

	//Get offset
	/**
	* \brief 获取位置偏移
	* 
	* 人物移动会造成粒子系统的相对运动
	* 
	* \return 人物移动引起的偏移
	*/
	stPointI& GetSceneOffset() { return m_offset; }

	//Get total sys num
	int GetTotalSysNum();

private:
	///因人物移动引起的位置偏移
	stPointI	m_offset;		//offset to draw particle when character is moving
	///使用类型
	enumParticleSysMgrUsage	m_usage;		//where we use this manager
	
	typedef std::vector<CParticleSystem*>	tListParticleSys;
	///粒子系统列表
	tListParticleSys	m_aParticleSystem;
	///当前粒子系统
	CParticleSystem*	m_activeParticleSystem;

	tListParticleSys::iterator FindParticleSysItByName( const char* pName );
};

CParticleSysManager*	GetParticleSysMgr();

#endif