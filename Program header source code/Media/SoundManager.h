#pragma once


#include "./SoundBuffer.h"
class CMP3;
enum enumSoundType{
	SoundType_None = -1,
	SoundType_Scene,
	SoundType_Circumstance,
	SoundType_UI,
	SoundType_Music,
	SoundType_Num,
};

typedef DWORD SoundId;
#define INVALID_SOUND_ID -1

struct SoundParam
{
	float xPos,yPos,zPos;
	float minDist,maxDist;
	bool bHead;
};

#define SOUND_PARAM_MASK_POS 1
#define SOUND_PARAM_MASK_MINMAXDIST 2
#define SOUND_PARAM_MASK_HEAD	4
class CSoundManager
{
	enum
	{
		SOUNDBUFF_NUM = 50,
	};
	
	struct SoundIndex{
		WORD index;
		WORD idx;
	};

	float						m_fVolume;
	float						m_aVolume[SoundType_Num];
	bool						m_aEnable[SoundType_Num];
	bool						m_bEnable;
	bool						m_bEnable3D;
	float                       m_aVolumeFactor[SoundType_Num];
	char						m_szFilePack[MAX_PATH];
	DWORD						m_LastSoundId;
	CSoundBuffer				m_Buffers[SOUNDBUFF_NUM];
	CMP3*						m_pMP3;
	typedef std::map<DWORD,size_t> tMapFileIdToIndex;
	typedef std::map<SoundId,SoundIndex> tMapSoundIdToIndex;
	typedef std::set<size_t>		tSetBufferIndex;
	tMapFileIdToIndex	m_mapBuffer;
	tMapSoundIdToIndex  m_mapSoundId;
	tSetBufferIndex		m_UseBuffers;
	tSetBufferIndex		m_FreeBuffers;

	HWND		 m_hWnd;
	DWORD		 m_nMsg;
#ifdef DX_SOUND_API
	IDirectSound8*				m_pSystem;
	IDirectSound3DListener8*	m_pListener;
	bool SetPrimaryBufferFormat();
	bool CreateListener();
#endif

	CSoundBuffer* FindBufferForUse(DWORD fileId);
	SoundIndex FindBufferById(SoundId id);
	CSoundBuffer* FindBufferByFileName(const char* filename);
	CSoundBuffer* FindBufferByFileId(DWORD fileId);
public:
	CSoundManager(void);
	~CSoundManager(void);
	bool IsEnable3D(){ return m_bEnable3D;}
	bool Create(HWND hWnd,UINT nMsg);
	void Destroy(void);
	bool CommitDeferredSettings(void);
	bool SetListenerOrientation(float xFront, 
		float yFront, 
		float zFront, 
		float xTop, 
		float yTop, 
		float zTop, 
		bool  bApply = false );
	bool SetListenerOrientation(int nDir,bool bApply = false);
	bool SetListenerPosition(float fX, float fY, float fZ, bool bApply = false);
	bool SetListenerVelocity(float fX, float fY, float fZ, bool bApply = false);
	bool MsgProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
	void Run(float fEslaspedTime);

	void	PlayMP3(const char* filename,enumSoundType type = SoundType_Music);
	SoundId Play(const char* filename,enumSoundType type,bool bLoop = false,SoundParam * param = NULL);
	void Play(SoundId sound,SoundParam * param,DWORD mask = SOUND_PARAM_MASK_POS);
	void SetSoundParam(SoundId sound,SoundParam * param,DWORD mask = SOUND_PARAM_MASK_POS,bool bApply = false);
	void Stop(SoundId soundId);

	IDirectSound8* GetSystem()
	{
		return m_pSystem;
	}

	float GetVolume(){ return m_fVolume;}
	void SetVolume(float fVol);

	float	GetVolumeOfType(enumSoundType type) { return m_aVolume[type] * m_aVolumeFactor[type] * m_fVolume; }
	void SetVolumeOfType(enumSoundType type,float fVolume);

	bool IsEnableOfType(enumSoundType type) { return m_aEnable[type] && m_bEnable;}
	void SetEnableOfType(enumSoundType type,bool bEnabled);

	bool IsEnable(){ return m_bEnable; }
	void SetEnable(bool bEnable);

	void SetVolumeFactor(enumSoundType type,float factor);

	DWORD GetWndMsg(){ return m_nMsg;}

	const char* GetFilePackName(){
		return m_szFilePack;
	}
	void SetFilePack(const char* szFilePack)
	{
		strcpy(m_szFilePack,szFilePack);
	}

	bool IsPlaying(SoundId id);
	bool IsPlaying(const char* filename);

	DWORD GetNewSoundId();
	void  DeleteSoundId(DWORD soundId);
};

extern LONG VolumeFloatToInt(float fValue);
extern CSoundManager* GetSoundManager();