#pragma once

class CMusic;

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CBackMusic
{

	float			 m_fVolume;
	float			 m_fCurVolume;
	float			 m_fDVolume;
	char			 m_szCurFile[MAX_PATH];

public:
	CBackMusic(HWND hWnd,UINT nEventMsg);
	~CBackMusic(void);

	void Release();
	CMusic* m_pCurMusic;
	CMusic* m_pNextMusic;

	void StopPlay(DWORD dwFadeTime = c_dwMusicFadeTime);
	bool PlayNext(const char* pszFileName,DWORD dwFadeTime = c_dwMusicFadeTime);
	bool IsPlaying();

	bool SetVolume(float fVolume);
	float GetVolume() { return m_fVolume;}

	float GetCurVolume() { return m_fCurVolume;}
	bool SetCurVolume(float fVolume);

	void Run(float fElapsedTime);
	bool MsgProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
};
