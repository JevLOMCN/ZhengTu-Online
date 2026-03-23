#pragma once
//#include <dshow.h>

class CMP3;
extern const DWORD c_dwMusicFadeTime;
/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
*/
class CMusic
{
public:
	enum {
		VOLUME_FULL    = 0,
		VOLUME_SILENCE = -10000,
		VOLUME_CHANGE  = -2000,
		VOLUME_PITCH   = 20,
	};

private:

	HWND			 m_hWnd;
	UINT			 m_nMsg;

	float			 m_fCurVolume;
	float			 m_fDVolume;
	char			 m_szCurFile[MAX_PATH];

	// Save 
	char			 m_szNextFile[MAX_PATH];

	bool			m_bWait;
	bool			m_bPause;
	CMP3		*	m_pSound;
	CMP3		*	m_pNextSound;

	DWORD			m_iMaxInterval;
	DWORD			m_iMinInterval;
	DWORD			m_dwNextPlayTime;
	bool Play(void);
	bool Stop(void);
public:
	CMusic(HWND hWnd,UINT nEventMsg);
	~CMusic(void);
	static CMusic* sInst;
	const char * GetCurPlayingFile()
	{
		return m_szCurFile;
	}

	bool IsEnabled();
	bool Init();
	bool LoadMusic(const char* pszFileName);
	bool SetCurVolume(float fVolume);
	float GetCurVolume();
	void SetPause(bool b);
	bool IsPlaying();
	bool SetPosition(LONGLONG pos);

	void UnInit(void);

	bool PlayNext(const char* pszFileName,DWORD dwFadeTime = c_dwMusicFadeTime,DWORD iMinInternal = 0,DWORD iMaxInternal = 0,bool bWait = false);
	void StopPlay(DWORD dwFadeTime = c_dwMusicFadeTime);

	void Run(float fElapsedTime);

	void OnVolumeChange();

	void OnEnabled();

	bool Exchange();
};
