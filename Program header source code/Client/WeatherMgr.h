/**
 *\file		WeatherMgr.h
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:18:00 中国标准时间
 *\brief	天气管理器
 *
 * 
 *
*/

#pragma	   once

#include "./GameScene.h"
#include "./EffectCloud.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CSound;
class CWeatherMgr
{
	///当前天气类型
	BYTE m_curWeatherType;
	///当前天气级别
	int m_curWeatherLevel;
	//////////////////////////////////////////////////////////////////////////
	char m_curWeatherName[MAX_NAMESIZE];
	///当前天气指针
	void* m_curWeather;
	///云彩
	CEffectCloud* m_cloud;
	///天气（粒子类的）有效绘制范围
	stRectI	m_rcLiveRange;

	bool m_bWaitingPlay;
	bool m_bPoping;
	float m_fChangeValue;
	float m_fChangeLastTime;

	BYTE m_nextWeatherType;
	int m_nextWeatherLevel;
	char m_nextWeatherName[MAX_NAMESIZE];

	DWORD m_nextPlayLightenTime;
	DWORD m_nextPlayLightenSoundTime;

private:
	void GetRainLocation(D3DXVECTOR3& vPos);
	void GetSnowLocation(D3DXVECTOR3& vPos);

	void LoadCloudEffect();
	void PlayCloudEffect() { if ( m_cloud ) m_cloud->Play(); }
	void StopCloudEffect() { if ( m_cloud ) m_cloud->Stop(); }
	void UnLoadCloudEffect();
public:
	CWeatherMgr(void);
	~CWeatherMgr();

	void OpenWeatherEffect();
	void CloseWeatherEffect();

	void StopCurWeatherEffect();
	void ChangeWeather( BYTE type, int level, const char* music );
	//void SwitchNewWeather();
	void ResetLiveRange( stRectI range );
	void PopNextWeather();
	void PlayNextWeather();
	void PlayParticleTypeWeather( bool bUseOutLiveRange = true );

	void PlaySound();
	void PlayLightenSound();

	HRESULT Render(float fElapsedTime);
	HRESULT Update();

	void ReleaseAllWeather();
};

CWeatherMgr* GetWeatherMgr();