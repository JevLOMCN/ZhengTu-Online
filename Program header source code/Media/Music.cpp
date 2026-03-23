#include "../engine/include/engine.h"
#include ".\music.h"
#include "./SoundManager.h"
#include "./MP3.h"

#pragma comment( lib ,"strmiids.lib")

//const float c_fVolumeTime = 3;
const DWORD c_dwMusicFadeTime =  3000;
CMusic* CMusic::sInst = NULL;
/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
CMusic::CMusic(HWND hWnd,UINT nEventMsg)
: m_fCurVolume(1.0f)
, m_hWnd(hWnd)
, m_fDVolume(0)
, m_iMinInterval(0)
, m_iMaxInterval(0)
, m_dwNextPlayTime(0)
, m_bWait(false)
, m_bPause(false)
{
	m_szCurFile[0] = 0;
	m_szNextFile[0] = 0;
	sInst = this;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
CMusic::~CMusic(void)
{
	UnInit();
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
bool CMusic::Init()
{
	return true;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param lVolume : 描述
* \return 返回值的描述
*/
bool CMusic::SetCurVolume(float fVolume)
{
	m_fCurVolume = fVolume;
	if(m_pSound)
		m_pSound->SetVolume(fVolume);
	return true;
}

void CMusic::SetPause(bool b)
{
	m_bPause = b;
	if(m_pSound)
		m_pSound->SetPause(b);
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
float CMusic::GetCurVolume()
{
	return m_fCurVolume;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
bool CMusic::Play(void)
{
	if(!IsEnabled()) return true;
	if(m_szCurFile[0] == 0)
		return true;
	if(m_pSound) {
		m_pSound->SetVolume(m_fCurVolume);
		return m_pSound->Play();
	}
	if(m_iMinInterval < m_iMaxInterval){
		m_dwNextPlayTime = xtimeGetTime() + rand() % (m_iMaxInterval - m_iMinInterval) + m_iMinInterval;
	}else{
		m_dwNextPlayTime = 0;
	}
	return false;
}

bool CMusic::IsEnabled()
{
	return GetSoundManager()->IsEnable() && GetSoundManager()->IsEnableOfType(SoundType_Music);
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pos : 描述
* \return 返回值的描述
*/
bool CMusic::SetPosition(LONGLONG pos)
{
	return false;
}

/**
* \brief 简短描述
* 
*  调用stop()并不立即停止播放,直到播放完成(循环的取消循环)才关闭;
*  如果要指定时间停止播放，可调用StopPlay();
* 
* \return 返回值的描述
*/
bool CMusic::Stop(void)
{
	if(m_pSound) 
	{
		m_pSound->Stop();
		return true;
	}
	return false;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
bool CMusic::IsPlaying()
{
	if(m_pSound) 
		return m_pSound->IsPlaying();
	return false;
}

void CMusic::UnInit(void)
{
	if(m_pSound)
		delete m_pSound;
	m_pSound = NULL;
	if(m_pNextSound)
		delete m_pNextSound;
	m_pNextSound = NULL;
}

/**
* \brief 简短描述
* 
* 
* 
* \param dwFadeTime : 描述
* \return 返回值的描述
*/
void CMusic::StopPlay(DWORD dwFadeTime)
{
	if( dwFadeTime )
	{
		PlayNext("",dwFadeTime);
	}
	else
	{
		Stop();
		m_fDVolume = 0.0f;
	}
}


/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pszFileName : 描述
* \param dwFadeTime : 描述
* \return 返回值的描述
*/
bool CMusic::PlayNext(const char* pszFileName,DWORD dwFadeTime,DWORD iMinInterval,DWORD iMaxInterval,bool bWait)
{
	if( ( 0 == stricmp(pszFileName,m_szCurFile)) && IsPlaying() )
		return true;

	m_bWait = bWait;

	strncpy(m_szNextFile,pszFileName,sizeof(m_szNextFile));
	m_szNextFile[sizeof(m_szNextFile)-1] = 0;

	if( !dwFadeTime || !IsEnabled() || m_bWait)
	{
		// 不淡入淡出
		m_fCurVolume = 1.0f;
		m_fDVolume = 0.0f;
		m_iMaxInterval = iMaxInterval;
		m_iMinInterval = iMinInterval;
		if(!m_bWait){
			Exchange();
		}
		return true;
	}

	// 准备淡出淡入

	m_fDVolume = - m_fCurVolume / (dwFadeTime /1000.f);
	if(m_pNextSound)
	{
		delete m_pNextSound;
		m_pNextSound = NULL;
	}

	if( pszFileName[0] )
	{
		m_pNextSound = new CMP3(m_szNextFile,SoundType_Music);

		// 淡入淡出期间不会用到间隔，可以这里设
		m_iMaxInterval = iMaxInterval;
		m_iMinInterval = iMinInterval;
	}

	return true;
}

bool CMusic::Exchange()
{
	if(m_pSound)
	{
		m_pSound->Stop();
		delete m_pSound;
		m_pSound = NULL;
	}

	m_pSound = m_pNextSound;
	strcpy(m_szCurFile,m_szNextFile);

	m_szNextFile[0] = 0;
	m_pNextSound = NULL;
	if(m_bPause)
		return true;
	return Play();
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param fElapsedTime : 描述
* \return 返回值的描述
*/
void CMusic::Run(float fElapsedTime)
{
	if(!IsEnabled())
		return;

	if(m_fDVolume != 0.0f)
	{
		// 处理淡入淡出
		m_fCurVolume += fElapsedTime * m_fDVolume;
		if((m_szNextFile[0] && m_fCurVolume < 0.3f) || m_fCurVolume < 0.0f  || m_pSound == NULL || !m_pSound->IsPlaying())
		{
			// 淡出完成
			m_fCurVolume = 0.6f ;
			m_fDVolume = -m_fDVolume;
			m_fDVolume *= 1.5f;
			if(Exchange())
			{
				SetCurVolume(m_fCurVolume);
			}
			else
			{
				m_fCurVolume = 1.0f;
				m_fDVolume = 0.0f;
			} 
		}
		else if(m_fCurVolume >= 1.0f)
		{ 
			// 淡入完成
			m_fCurVolume = 1.0f;
			m_fDVolume = 0.0f;
			SetCurVolume(m_fCurVolume);
		}
		else
			SetCurVolume(m_fCurVolume);
	}else if(!m_bPause){
		if(m_szNextFile[0] && !m_bWait){
			// 有下一个音乐要立即播放
			Exchange();
		}else{
			if(m_pSound){
				if(m_pSound->IsPlaying() == false && !m_pSound->IsPause()){
					if(m_bWait){
						// 当前声音播放完了，有下一个音乐要接着播放
						Exchange();
						m_bWait = false;
					}else{
						if(xtimeGetTime() > m_dwNextPlayTime){
							m_pSound->SetPosition(0);
							Play();
						}
					}
				}
			}
		}
	}
}

void CMusic::OnVolumeChange()
{
	if(m_pSound)
		m_pSound->ApplyFinalVolume();
}

void CMusic::OnEnabled()
{
	if(!IsEnabled() && m_pSound)
		m_pSound->Stop();
}