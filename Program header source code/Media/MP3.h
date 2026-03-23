#pragma once

#include <dshow.h>
enum enumSoundType;
class CMP3 
{
	IGraphBuilder   *m_pGB ;
	IMediaControl   *m_pMC ;
	IMediaSeeking   *m_pMS ;
	IMediaEventEx	*m_pME ;
	IBasicAudio		*m_pBA ;
	IBaseFilter		*m_pBF;
	HWND			m_hWnd;
	bool			m_bLoad;
	enumSoundType	m_eType;
	float			m_fVolume;
public:
	CMP3(const char* filename,enumSoundType type);
	~CMP3(void);
	bool Init();
	void UnInit();
	bool Play();
	bool IsPlaying();
	bool IsPause();
	void Stop(void);

	FILTER_STATE GetState();

	bool LoadFile(const char* pszFileName);
	void ApplyFinalVolume();

	LONGLONG GetTimeLength();

	bool SetVolume(float fVolume);
	void SetPause(bool bPause);
	bool SetPosition(LONGLONG pos);
	LONGLONG GetPosition();
	bool IsEnable();
};
