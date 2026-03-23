#include "../engine/include/engine.h"
#include ".\music.h"
#include "./SoundManager.h"
#include ".\backmusic.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CBackMusic::CBackMusic(HWND hWnd,UINT nEventMsg)
: m_pCurMusic(new CMusic(hWnd,nEventMsg))
, m_pNextMusic(new CMusic(hWnd,nEventMsg + 1))
, m_fVolume(1.0f)
, m_fCurVolume(1.0f)
, m_fDVolume(0.0f)
{
	m_szCurFile[0] = 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CBackMusic::~CBackMusic(void)
{
	delete m_pCurMusic;
	delete m_pNextMusic;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CBackMusic::Release()
{
	m_pCurMusic->UnInit();
	m_pNextMusic->UnInit();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CBackMusic::IsPlaying()
{
	return m_pCurMusic->IsPlaying();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fVolume : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CBackMusic::SetVolume(float fVolume)
{
	m_fVolume = fVolume;
	return m_pCurMusic->SetVolume(fVolume);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fVolume : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CBackMusic::SetCurVolume(float fVolume)
{
	m_fCurVolume = fVolume;
	return m_pCurMusic->SetVolume(fVolume);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dwFadeTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CBackMusic::StopPlay(DWORD dwFadeTime)
{
	PlayNext("",dwFadeTime);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszFileName : ÃèÊö
 * \param dwFadeTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CBackMusic::PlayNext(const char* pszFileName,DWORD dwFadeTime)
{
	if( ( 0 == stricmp(pszFileName,m_szCurFile) ) && IsPlaying() )
		return true;

	if( !dwFadeTime || !IsPlaying() )
	{
		strncpy(m_szCurFile,pszFileName,sizeof(m_szCurFile));
		m_szCurFile[sizeof(m_szCurFile)-1] = 0;
		m_pCurMusic->UnInit();
		m_pNextMusic->UnInit();
		if(m_szCurFile[0] == 0) return true;
		
		m_pCurMusic->Init();
		m_pCurMusic->LoadMusic(pszFileName);
		m_pCurMusic->Play();
		SetVolume(GetVolume());
		m_fCurVolume = GetVolume();
		m_fDVolume = 0.0f;

		return true;
	}

	strncpy( m_szCurFile,pszFileName,MAX_PATH );
	m_szCurFile[MAX_PATH-1] = 0;

	m_fCurVolume = Percent2DB( (float)(GetCurVolume() - DSBVOLUME_MIN)/(float)(DSBVOLUME_MAX - DSBVOLUME_MIN) )/100.0f;
	m_fDVolume = - m_fCurVolume / (dwFadeTime /1000.f);
	m_pNextMusic->UnInit();
	if(m_szCurFile[0])
	{
		m_pNextMusic->Init();
		m_pNextMusic->LoadMusic(m_szCurFile);
	}

	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CBackMusic::Run(float fElapsedTime)
{
	if(m_fDVolume != 0.0f)
	{
		m_fCurVolume += fElapsedTime * m_fDVolume;
		if(m_fCurVolume < 0.0f)
		{
			m_fCurVolume = 0.0f ;
			m_fDVolume = -m_fDVolume ;

			m_pCurMusic->Stop();
			m_pCurMusic->UnInit();

			if(m_szCurFile[0])
			{
				m_pNextMusic->Play();
				m_fCurVolume = 1.0 * GetVolume();

				CMusic* pTmp = m_pCurMusic;
				m_pCurMusic = m_pNextMusic;
				m_pNextMusic = pTmp;

			}
			else
			{
				m_fDVolume = 0.0f;
			}
		}
		else if(m_fCurVolume > GetVolume())
		{
			m_fCurVolume = GetVolume();
			m_fDVolume = 0.0f;
		}

		SetCurVolume(DB2Percent(m_fCurVolume * 100) * (DSBVOLUME_MAX - DSBVOLUME_MIN) + DSBVOLUME_MIN);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param uMsg : ÃèÊö
 * \param wParam : ÃèÊö
 * \param lParam : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CBackMusic::MsgProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	m_pNextMusic->FreeEvent(uMsg,wParam,lParam);
	return m_pCurMusic->MsgProc(uMsg,wParam,lParam);
}
