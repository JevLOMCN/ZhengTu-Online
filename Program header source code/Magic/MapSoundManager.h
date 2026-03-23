#pragma once

#include "./MapAddinManager.h"
#include "./magic_sound.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CSound;

// Ê±¼äÏŞÖÆÇø
struct stMapSceneSoundTimeSegment
{
	DWORD dwStart;  // desc : game time 
	DWORD dwEnd;
};

struct stMapSceneSoundInfo{
	DWORD minInterval;
	DWORD maxInterval;
	DWORD file;
	typedef std::vector<stMapSceneSoundTimeSegment> tSegments;
	tSegments  vecSegments; 
};
struct stMapSceneSound{
	stMapSceneSoundInfo* pMusic;
	POINT	pt;
	CSceneSound sound;
	//CSound* sound;
	DWORD nextPlayTime;
	DWORD lastCheckTime;
	stMapSceneSound(){
		nextPlayTime = 0;
		lastCheckTime = -1;
	}
	~stMapSceneSound(){ sound.Destroy(true); }
	void Run(float time_elapsed);
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CMapSoundManager : public tMapAddinManager<stMapSceneSound>
{
	typedef std::map<DWORD , stMapSceneSoundInfo> tMapMusicInfo;
	tMapMusicInfo m_mapMusic;
public:

	stMapSceneSound* CreateObject( POINT pt,void* pData ) 
	{ 
		DWORD dwID = *(DWORD*)pData;
		tMapMusicInfo::iterator it = m_mapMusic.find(dwID);
		if(it != m_mapMusic.end())
		{
			stMapSceneSound* p = new stMapSceneSound;
			p->pMusic = &it->second;
			p->pt = pt;
			p->nextPlayTime = 0;
			return p;
		}
		return NULL;
	}

	void SetObjectData( stMapSceneSound* pObject,void* pData) 
	{
		DWORD dwID = *(DWORD*)pData;
		tMapMusicInfo::iterator it = m_mapMusic.find(dwID);
		if( it != m_mapMusic.end() )
		{
			pObject->pMusic = &it->second;
		}
	}

	CMapSoundManager(void);
	~CMapSoundManager(void);
	void Init();
};

CMapSoundManager* GetMapSoundManager();