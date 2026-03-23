#include "../engine/include/engine.h"

#include "magic_sound.h"

#include "iCharacter.h"
#include "./MapScene.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

float CSceneSound::m_fSceneWidth =0.0f;
float CSceneSound::m_fSceneHeight=0.0f;
float CSceneSound::m_fScreenWidth = 13;
float CSceneSound::m_fScreenHeight = 19;
float CSceneSound::m_fMinDis = 6;//DS3D_DEFAULTMINDISTANCE;
float CSceneSound::m_fMaxDis = 8;
//float CSceneSound::m_fMinDis = DS3D_DEFAULTMINDISTANCE;
//float CSceneSound::m_fMaxDis = m_fScreenWidth ;
stPointI CSceneSound::m_ptCenter(0,0);

const float c_fScaleX = c_nSoundTileWidth;
const float c_fScaleY = c_nSoundTileHeight;

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param e : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
static const char* GetSoundFolder(SoundEnum e)
{
	static const char *s_szFolder[] =
	{
		"sound\\Magic",//Sound_Magic,
		"sound\\character",//Sound_Character,
		"sound\\character\\male",//Sound_Male,
		"sound\\character\\female",//Sound_Female,
		"sound\\npc",//Sound_Npc,
		"sound\\ground",//Sound_Ground
		"sound\\item",
		"sound\\scene"
		"",
	};
	assert(e < SoundEnum_Num);
	if( e<0 || e >= SoundEnum_Num) e = SoundEnum_Num;
	return s_szFolder[e];
}    


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pSound : √Ë ˆ
 * \param ptPos : √Ë ˆ
 * \param velocity : √Ë ˆ
 * \param szFile : √Ë ˆ
 * \param bLoop : √Ë ˆ
 * \param bHead : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
static void PlaySound(CSceneSound* pSound,enumSoundType type,POINT ptPos,POINT velocity, LPCSTR szFile, bool bLoop,bool bHead,
					  int minDist = CSceneSound::m_fMinDis * c_fScaleX)
{
	int maxDist = 20 * c_nMagicTileWidth;
	/*if(type != SoundType_Scene){
		maxDist = 13 * c_nMagicTileWidth;
	}else{
		maxDist = 20 * c_nMagicTileWidth;
	}*/

	POINT ptCenter = GetMapScene()->GetCenterPixelPos();
	int disSq = (ptCenter.x - ptPos.x) * (ptCenter.x - ptPos.x) + (ptCenter.y - ptPos.y ) * (ptCenter.y - ptPos.y);
	if(disSq > maxDist * maxDist)
	{
		return;
	}

	maxDist *= 4;
	if(pSound){
		if (pSound->Create(szFile,type,bHead,true,minDist/c_fScaleX,maxDist/c_fScaleX))	{
			pSound->SetParam(ptPos);
			pSound->Play(bLoop);
		}
	}else{
		CSceneSound sound;
		pSound = &sound;
		if (pSound->Create(szFile,type,bHead,true,minDist/c_fScaleX,maxDist/c_fScaleX))	{
			pSound->SetParam(ptPos);
			pSound->Play(bLoop);
		}
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pSound : √Ë ˆ
 * \param ptPos : √Ë ˆ
 * \param velocity : √Ë ˆ
 * \param e : √Ë ˆ
 * \param nKind : √Ë ˆ
 * \param strSound : √Ë ˆ
 * \param bLoop : √Ë ˆ
 * \param bHead : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void PlayCharacterSound(CSceneSound* pSound,POINT ptPos,POINT velocity,SoundEnum e,size_t nKind,const char* strSound,bool bLoop,bool bHead)
{
	if (strSound[0] == 0)
	{
		if(pSound)
			pSound->Stop();
		return;
	}
	char szFile[256];
	sprintf(szFile,"%s\\%s.wav",GetSoundFolder(e),strSound);
	PlaySound( pSound,SoundType_Scene,ptPos,velocity,szFile,bLoop,bHead );
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pSound : √Ë ˆ
 * \param ptPos : √Ë ˆ
 * \param velocity : √Ë ˆ
 * \param szMusic : √Ë ˆ
 * \param nMagicType : √Ë ˆ
 * \param bLoop : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void PlayMagicSound(CSceneSound* pSound,POINT ptPos,POINT velocity,LPCSTR szMusic, size_t  nMagicType, bool bLoop)
{
	char szFile[256];
	sprintf(szFile,"%s\\%s.wav",GetSoundFolder(Sound_Magic),szMusic);
	PlaySound(pSound,SoundType_Scene,ptPos,velocity,szFile,bLoop,false);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pSound : √Ë ˆ
 * \param ptPos : √Ë ˆ
 * \param velocity : √Ë ˆ
 * \param szMusic : √Ë ˆ
 * \param bLoop : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void PlaySceneSound( CSceneSound* pSound,POINT ptPos,POINT velocity,LPCSTR szMusic,  bool bLoop ,int minDist,int maxDist)
{
	char szFile[256];
	sprintf(szFile,"%s\\%s.wav",GetSoundFolder(Sound_Scene),szMusic);
	PlaySound(pSound,SoundType_Circumstance,ptPos,velocity,szFile,bLoop,false,minDist);
}

void PlayItemSound( CSceneSound* pSound,POINT ptPos,POINT velocity,LPCSTR szMusic,  bool bLoop )
{
	char szFile[256];
	sprintf(szFile,"%s\\%s.wav",GetSoundFolder(Sound_Item),szMusic);
	PlaySound(pSound,SoundType_Scene,ptPos,velocity,szFile,bLoop,false);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param ptPos : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CSceneSound::SetListenerPosition(POINT ptPos)
{
	//ptPos -= Character_GetMainPosition();
	float x,z;
	x = (float)ptPos.x/c_fScaleX;
	z =  - (float)ptPos.y/c_fScaleY;
	GetSoundManager()->SetListenerPosition(x,0.0f,z);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param ptVelocity : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CSceneSound::SetListenerVelocity(POINT ptVelocity)
{
	//*
	float x,z;
	x = (float)ptVelocity.x/c_fScaleX;
	z = - (float)ptVelocity.y/c_fScaleY;
	GetSoundManager()->SetListenerVelocity(x,0.0f,z);
	//*/
}

bool CSceneSound::Create(LPCSTR szFile,enumSoundType type,bool bHead,bool be3d,float minDis , float maxDis )
{
	if(sound != INVALID_SOUND_ID)
		return true;
	m_type = type;
	m_bHead = bHead;
	m_minDist = minDis;
	m_maxDist = maxDis;
	m_b3D = be3d;
	strcpy(m_szFileName,szFile);
	return true;
}

void CSceneSound::Destroy(bool force)
{
	if(sound != INVALID_SOUND_ID){
		if(force || m_bLooping)
			GetSoundManager()->Stop(sound);
		sound = INVALID_SOUND_ID;
	}
	m_bLooping = false;
}


bool CSceneSound::Play(bool bLoop)
{
	if(m_bLooping && bLoop)
	{
		return true;
	}

	SoundParam param;
	param.bHead = m_bHead;
	param.minDist = m_minDist;
	param.maxDist = m_maxDist;
	param.xPos = m_xPos;
	param.yPos = 0.0f;
	param.zPos = m_zPos;
	if(sound == INVALID_SOUND_ID)
		sound = GetSoundManager()->Play(m_szFileName,m_type,bLoop,&param);
	else if(!m_bLooping)
		GetSoundManager()->Play(sound,&param);
	if(sound != INVALID_SOUND_ID){
		m_bLooping = bLoop;
		return true;
	}
	m_bLooping = false;
	return false;
}

void CSceneSound::Stop()
{
	Destroy();
}

bool CSceneSound::SetParam(POINT pos)
{
	m_xPos = (float)pos.x/c_fScaleX;
	m_zPos =  - (float)pos.y/c_fScaleY;

	if(sound != INVALID_SOUND_ID){

		SoundParam param;
		param.xPos = m_xPos;
		param.yPos = 0.0f;
		param.zPos = m_zPos;

		GetSoundManager()->SetSoundParam(sound,&param);
		return true;
	}
	return false;
}

bool CSceneSound::SetParam(POINT pos,bool bHead)
{
	m_xPos = (float)pos.x/c_fScaleX;
	m_zPos =  - (float)pos.y/c_fScaleY;
	m_bHead = bHead;

	if(sound != INVALID_SOUND_ID){

		SoundParam param;
		param.bHead = m_bHead;
		param.minDist = m_minDist;
		param.maxDist = m_maxDist;
		param.xPos = m_xPos;
		param.yPos = 0.0f;
		param.zPos = m_zPos;

		GetSoundManager()->SetSoundParam(sound,&param,SOUND_PARAM_MASK_HEAD | SOUND_PARAM_MASK_POS);
		return true;
	}
	return false;
}