#pragma	 once

enum SoundEnum
{
	Sound_Magic = 0,
	Sound_Character,
	Sound_Male,
	Sound_Female,
	Sound_Npc,
	Sound_Ground,
	Sound_Item,
	Sound_Scene,
	SoundEnum_Num,
};

const int c_nSoundTileWidth = 64;
const int c_nSoundTileHeight = 32;

extern const float c_fScaleX;//c_nSoundTileWidth;
extern const float c_fScaleY;//c_nSoundTileHeight;
#include "../Media/SoundManager.h"

//#include "./iCharacter.h"

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CSceneSound
{
public:
	static float m_fSceneWidth;
	static float m_fSceneHeight;
	static float m_fScreenWidth;
	static float m_fScreenHeight;
	static float m_fMinDis;
	static float m_fMaxDis;
	static stPointI m_ptCenter;
private:
	SoundId sound;
	float m_xPos,m_zPos;
	float m_minDist,m_maxDist;
	enumSoundType m_type;
	bool m_bHead;
	bool m_bLooping;
	bool m_b3D;
	char m_szFileName[64];
public:
	CSceneSound()
		: m_bLooping(false)
	{
		sound =  INVALID_SOUND_ID;
	}
	~CSceneSound()
	{
		Destroy();
	}

	static void SetCenterPosition(long x,long y)
	{
		m_ptCenter.x = x;
		m_ptCenter.y = y;
	}

	static void SetSceneSize(float w,float h)
	{
		m_fSceneWidth = w;
		m_fSceneHeight= h;
	}

	static void SetScreenSize(float w,float h)
	{
		m_fScreenWidth = w/c_nSoundTileWidth;
		m_fScreenHeight = h/c_nSoundTileHeight;
		m_fMaxDis = m_fScreenWidth/2;
	}

	bool Create(LPCSTR szFile,enumSoundType type,bool bHead = false,bool be3d = true,float minDis = m_fMinDis, float maxDis = m_fMaxDis);

	void Destroy(bool force = false);


	bool Play(bool bLoop);

	void Stop();

	bool SetParam(POINT pos,bool bHead);
	bool SetParam(POINT pos);

	static void SetListenerPosition(POINT ptPos);
	static void SetListenerVelocity(POINT ptVelocity);

	bool IsLoad(){ return sound != INVALID_SOUND_ID;}

};
void PlaySceneSound(CSceneSound* pSound,POINT ptPos,POINT velocity,LPCSTR szMusic,bool bLoop,int minDist,int maxDist);
void PlayCharacterSound(CSceneSound* pSound,POINT ptPos,POINT velocity,SoundEnum e,size_t nKind,const char* strSound,bool bLoop,bool bHead);
void PlayMagicSound(CSceneSound* pSound,POINT ptPos,POINT velocity,LPCSTR szFile, size_t  nMagicType, bool bLoop = false);
void PlayItemSound(CSceneSound* pSound,POINT ptPos,POINT velocity,LPCSTR szMusic,  bool bLoop =  false);