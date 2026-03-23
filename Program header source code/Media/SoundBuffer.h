#pragma once

#define DX_SOUND_API

#ifdef DX_SOUND_API
#include <Dsound.h>
#endif


enum enumSoundType;
class CSoundBuffer
{
public:
	enum{
		DSBUFFER_NUM = 5,
	};
private:
	bool	m_b3D;
	enumSoundType m_eType;
	DWORD	m_fileId;
	int		m_nCurrent;
	DWORD	m_LastUseTime;
	DWORD	m_aSoundId[DSBUFFER_NUM];
#ifdef DX_SOUND_API
	IDirectSoundBuffer*	m_pDSBuffer[DSBUFFER_NUM];
	IDirectSound3DBuffer*	m_p3DDSBuffer[DSBUFFER_NUM];
	bool CreateSoundBuffer(WAVEFORMATEX * wfx,DWORD dataSize);
#endif

public:
	CSoundBuffer();
	~CSoundBuffer(void);

	enumSoundType GetSoundType(){ return m_eType;}
	bool Is3D() {	return m_b3D;	}
	bool IsInit();
	void Init(DWORD fileId,bool b3D,enumSoundType type);
	void Destroy();

	DWORD GetLastUseTime(){	return m_LastUseTime;}
	bool Load(const char* filename);
	DWORD GetFileID(){ return m_fileId;}
	int GetFreeBufferIdx(bool force = false);
	int GetPlayingNum();
	void Play(int idx,DWORD soundId,bool bLoop = false);
	void Stop(int idx);
	void StopAll();
	void SetMinMaxDistance(int idx,float minDist,float maxDist,bool bApply = false);
	void SetPosition(int idx,float x,float y,float z,bool bApply = false);
	void SetVolume(int idx,float fVol);
	void SetAllVolume(float fVol);
	void SetEnable(bool bEnable);
	void SetHead(int idx,bool bHead,bool bApply = false);
	bool IsPlaying(int idx);
};
