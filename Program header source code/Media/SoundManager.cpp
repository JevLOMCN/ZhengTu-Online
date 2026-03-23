/**
 * \file      SoundManager.cpp
 * \version   $Id$
 * \author    $$$03π´Àæ
 *            $$$03π´Àæ@###.com
 * \date      2006-2-23 23:00:48
 * \brief     £ø
 * 
 *	      
 */

#include "../engine/include/engine.h"
#include ".\soundmanager.h"
#include "./MP3.h"
#include "../engine/include/crc.h"
#include "./Music.h"

#define MAX_LIM_TIME  60000

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param lSliderPos : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//-----------------------------------------------------------------------------
// Name: ConvertLinearSliderPosToLogScale()
// Desc: Converts a linear slider position to a quasi logrithmic scale
//  ˝÷µ◊™ªªŒ™∑÷±¥÷µ£®0DB - 100DB)
//-----------------------------------------------------------------------------
FLOAT ConvertLinearSliderPosToLogScale( LONG lSliderPos )
{
	if( lSliderPos > 0 && lSliderPos <= 10 )
	{
		return lSliderPos*0.01f;
	}
	else if( lSliderPos > 10 && lSliderPos <= 20 )
	{
		return (lSliderPos-10)*0.1f;
	}
	else if( lSliderPos > 20 && lSliderPos <= 30 )
	{
		return (lSliderPos-20)*1.0f;
	}
	else if( lSliderPos > 30 && lSliderPos <= 40 )
	{
		return (lSliderPos-30)*10.0f;
	}

	return 0.0f;
}




/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param fValue : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//-----------------------------------------------------------------------------
// Name: ConvertLinearSliderPosToLogScale()
// Desc: Converts a quasi logrithmic scale to a slider position
// ∑÷±¥÷µ◊™ªªŒ™ ˝÷µ£®0DB - 100DB)
//-----------------------------------------------------------------------------
LONG ConvertLogScaleToLinearSliderPosTo( FLOAT fValue )
{
	if( fValue > 0.0f && fValue <= 0.1f )
	{
		return (LONG)(fValue/0.01f);
	}
	else if( fValue > 0.1f && fValue <= 1.0f )
	{
		return (LONG)(fValue/0.1f) + 10;
	}
	else if( fValue > 1.0f && fValue <= 10.0f )
	{
		return (LONG)(fValue/1.0f) + 20;
	}
	else if( fValue > 10.0f && fValue <= 100.0f )
	{
		return (LONG)(fValue/10.0f) + 30;
	}

	return 0;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param fPercent : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
float Percent2DB(float fPercent)
{
	return ConvertLinearSliderPosToLogScale(long(fPercent * 40));
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param fDB : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
float DB2Percent(float fDB)
{
	return ConvertLogScaleToLinearSliderPosTo(fDB) / 40.0f;
}

#ifdef DX_SOUND_API
LONG VolumeFloatToInt(float fValue)
{
	return DSBVOLUME_MIN + (long)(DB2Percent(fValue * 100.0f) * (DSBVOLUME_MAX - DSBVOLUME_MIN));
}
#endif

CSoundManager* GetSoundManager()
{
	static CSoundManager sm;
	return &sm;
}

CSoundManager::CSoundManager(void)
: m_hWnd(NULL)
, m_pSystem(NULL)
, m_pListener(NULL)
, m_bEnable3D(false)
, m_bEnable(true)
, m_fVolume(1.0f)
, m_LastSoundId(-1)
, m_pMP3(NULL)
{
}

CSoundManager::~CSoundManager(void)
{
	if(m_pMP3){
		m_pMP3->Stop();
		delete m_pMP3;
		m_pMP3 = NULL;
	}
	Destroy();
}

void CSoundManager::Run(float fEslaspedTime)
{
	if(CMusic::sInst)
		CMusic::sInst->Run(fEslaspedTime);
}

bool CSoundManager::MsgProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return false;
}

CSoundBuffer* CSoundManager::FindBufferForUse(DWORD fileId)
{
	tMapFileIdToIndex::iterator itMap = m_mapBuffer.find(fileId);
	if(itMap != m_mapBuffer.end())
		return &m_Buffers[itMap->second];

	if(m_FreeBuffers.empty()){
		DWORD curTime = xtimeGetTime();
		for(tSetBufferIndex::iterator it = m_UseBuffers.begin(); it != m_UseBuffers.end(); ){
			CSoundBuffer & buf = m_Buffers[*it];
			if(curTime - buf.GetLastUseTime() > MAX_LIM_TIME){
				if(buf.GetPlayingNum() == 0){
					m_mapBuffer.erase(m_mapBuffer.find(buf.GetFileID()));
					m_FreeBuffers.insert(*it);
					it = m_UseBuffers.erase(it);
					buf.Destroy();
					continue;
				}
			}
			++it;
		}

		int minPlayNum = 9999;
		tSetBufferIndex::iterator minItor;
		if(m_FreeBuffers.empty()){
			for(tSetBufferIndex::iterator it = m_UseBuffers.begin(); it != m_UseBuffers.end(); ++it){
				CSoundBuffer & buf = m_Buffers[*it];
				size_t playNum = buf.GetPlayingNum();
				if(playNum < minPlayNum){
					minPlayNum = playNum;
					minItor = it;
				}
			}

			int index = *minItor;
			CSoundBuffer & buf = m_Buffers[*minItor];
			m_mapBuffer.erase(m_mapBuffer.find(buf.GetFileID()));
			m_UseBuffers.erase(minItor);
			m_FreeBuffers.insert(index);
			buf.Destroy();
		}
	}

	tSetBufferIndex::iterator it = m_FreeBuffers.begin();
	int index = *it;
	return &m_Buffers[index];
}

CSoundBuffer* CSoundManager::FindBufferByFileName(const char* filename)
{
	char szFileName[MAX_PATH];
	strcpy(szFileName,filename);
	strlwr(szFileName);
	filename = szFileName;
	DWORD fileId = calculateCRC(filename,strlen(filename));
	return FindBufferByFileId(fileId);
}

CSoundBuffer* CSoundManager::FindBufferByFileId(DWORD fileId)
{
	tMapFileIdToIndex::iterator it = m_mapBuffer.find(fileId);
	if(it != m_mapBuffer.end())
		return &m_Buffers[it->second];
	return NULL;
}

CSoundManager::SoundIndex CSoundManager::FindBufferById(SoundId id)
{
	tMapSoundIdToIndex::iterator it = m_mapSoundId.find(id);
	if(it != m_mapSoundId.end()){
		return it->second;
	}
	SoundIndex retval;
	retval.idx = -1;
	retval.index = -1;
	return retval;
	/*for(tSetBufferIndex::iterator it = m_UseBuffers.begin(); it != m_UseBuffers.end(); ++it){
		if( m_Buffers[(*it)].GetID() == id){
			return &m_Buffers[(*it)];
		}
	}
	return NULL;*/
}

void	CSoundManager::PlayMP3(const char* filename,enumSoundType type)
{
	if(m_pMP3){
		m_pMP3->Stop();
		delete m_pMP3;
		m_pMP3 = NULL;
	}
	m_pMP3 = new CMP3(filename,type);
	m_pMP3->Play();
}

SoundId CSoundManager::Play( const char* filename,enumSoundType type,bool bLoop,SoundParam * param )
{
	SoundId retval = -1;

	if(!IsEnableOfType(type))
		return retval;

	char szFileName[MAX_PATH];
	strcpy(szFileName,filename);
	strlwr(szFileName);
	filename = szFileName;
	DWORD fileId = calculateCRC(filename,strlen(filename));
	CSoundBuffer * Buffer = FindBufferForUse(fileId);

	if(NULL == Buffer)
		return retval;

	if(Buffer->IsInit() == false){
		Buffer->Init(fileId,param!=NULL,type);
		if(!Buffer->Load(filename)){
			return retval;
		}
		int index = Buffer - m_Buffers;
		m_mapBuffer[fileId] = index;
		m_UseBuffers.insert(index);
		m_FreeBuffers.erase(m_FreeBuffers.find(index));
		//Buffer->SetAllVolume(GetVolumeOfType(type));
	}

	if((param && !Buffer->Is3D()) || (!param && Buffer->Is3D()))
		return retval;
	
	if(Buffer->GetSoundType() != type)
		return retval;

	int idx;
	if(param)
		idx = Buffer->GetFreeBufferIdx(param->bHead);
	else
		idx = Buffer->GetFreeBufferIdx(false);

	if(idx == -1){
		return retval;
	}

	if(param){
		Buffer->SetMinMaxDistance(idx,param->minDist,param->maxDist,true);
		Buffer->SetPosition(idx,param->xPos,param->yPos,param->zPos,true);
		Buffer->SetHead(idx,param->bHead,true);
	}
	Buffer->SetVolume(idx,GetVolumeOfType(type));
	retval = GetNewSoundId();

	SoundIndex si;
	si.index = Buffer - m_Buffers;
	si.idx = idx;
	m_mapSoundId[retval] = si;

	Buffer->Play(idx,retval,bLoop);
	return retval;
}

bool CSoundManager::IsPlaying(const char* filename)
{
	CSoundBuffer* Buffer = FindBufferByFileName(filename);
	if(Buffer)
		return Buffer->IsPlaying(0);
	return false;
}

bool CSoundManager::IsPlaying(SoundId id)
{
	SoundIndex soundIndex = FindBufferById(id);
	if(soundIndex.index != (WORD)-1){
		return m_Buffers[soundIndex.index].IsPlaying(soundIndex.idx);
	}
	return false;
}

void CSoundManager::Play(SoundId sound,SoundParam * param,DWORD mask)
{
	SoundIndex soundIndex = FindBufferById(sound);
	if(soundIndex.index != (WORD)-1){
		CSoundBuffer* Buffer = &m_Buffers[soundIndex.index];
		if(mask & SOUND_PARAM_MASK_POS)
			Buffer->SetPosition(soundIndex.idx,param->xPos,param->yPos,param->zPos,true);
		if(mask & SOUND_PARAM_MASK_MINMAXDIST)
			Buffer->SetMinMaxDistance(soundIndex.idx,param->minDist,param->maxDist,true);
		if(mask & SOUND_PARAM_MASK_HEAD)
			Buffer->SetHead(soundIndex.idx,param->bHead,true);
		if(!Buffer->IsPlaying(soundIndex.idx))
		{
			Buffer->SetVolume(soundIndex.idx,GetVolumeOfType(Buffer->GetSoundType()));
			Buffer->Play(soundIndex.idx,sound,false);
		}
	}
}

void CSoundManager::Stop(SoundId soundId)
{
	SoundIndex soundIndex = FindBufferById(soundId);
	if(soundIndex.index != (WORD)-1){
		m_Buffers[soundIndex.index].Stop(soundIndex.idx);
	}
}

void CSoundManager::SetSoundParam(SoundId sound,SoundParam * param,DWORD mask,bool bApply)
{
	SoundIndex soundIndex = FindBufferById(sound);
	if(soundIndex.index != (WORD)-1){
		CSoundBuffer* Buffer = &m_Buffers[soundIndex.index];
		if(mask & SOUND_PARAM_MASK_POS)
			Buffer->SetPosition(soundIndex.idx,param->xPos,param->yPos,param->zPos,bApply);
		if(mask & SOUND_PARAM_MASK_MINMAXDIST)
			Buffer->SetMinMaxDistance(soundIndex.idx,param->minDist,param->maxDist,bApply);
		if(mask & SOUND_PARAM_MASK_HEAD)
			Buffer->SetHead(soundIndex.idx,param->bHead,bApply);
	}
}

// …Ë÷√“Ù¡ø
void CSoundManager::SetVolume(float fVolume)
{
	if(m_fVolume != fVolume){
		m_fVolume = fVolume;

		for(std::set<size_t>::iterator it = m_UseBuffers.begin(); it != m_UseBuffers.end(); ++it){
			CSoundBuffer & buf = m_Buffers[*it];
			buf.SetAllVolume(GetVolumeOfType(buf.GetSoundType()));
		}
		if(CMusic::sInst)
			CMusic::sInst->OnVolumeChange();
	}
	TRACE("SetVolume  %g\n",fVolume);
}

void CSoundManager::SetVolumeOfType(enumSoundType type,float fVolume)
{
	if(m_aVolume[type] != fVolume){
		m_aVolume[type] = fVolume;
		if(type != SoundType_Music){
			for(std::set<size_t>::iterator it = m_UseBuffers.begin(); it != m_UseBuffers.end();  ++it){
				CSoundBuffer & buf = m_Buffers[*it];
				if(buf.GetSoundType() == type)
					buf.SetAllVolume(GetVolumeOfType(type));
			}
		}else{
			if(CMusic::sInst)
				CMusic::sInst->OnVolumeChange();
		}
		TRACE("SetVolume type:%d, %g\n",type,fVolume);
	}
}

void CSoundManager::SetVolumeFactor(enumSoundType type,float factor)
{
	m_aVolumeFactor[type] = factor;
	if(type != SoundType_Music){
		for(std::set<size_t>::iterator it = m_UseBuffers.begin(); it != m_UseBuffers.end();  ++it){
			CSoundBuffer & buf = m_Buffers[*it];
			if(buf.GetSoundType() == type)
				buf.SetAllVolume(GetVolumeOfType(type));
		}
	}else{
		if(CMusic::sInst)
			CMusic::sInst->OnVolumeChange();
	}
	TRACE("SetVolumeFactor type:%d, factor:%g\n",type,factor);
}

void CSoundManager::SetEnable(bool bEnable)
{
	if(m_bEnable != bEnable){
		m_bEnable = bEnable;
		for(std::set<size_t>::iterator it = m_UseBuffers.begin(); it != m_UseBuffers.end();  ++it){
			CSoundBuffer & buf = m_Buffers[*it];
			buf.SetEnable(IsEnableOfType(buf.GetSoundType()));
		}
		if(CMusic::sInst)
			CMusic::sInst->OnEnabled();
	}
}

void CSoundManager::SetEnableOfType(enumSoundType type,bool bEnable)
{
	if(m_aEnable[type] != bEnable){
		m_aEnable[type] = bEnable;
		if(type != SoundType_Music){
			for(std::set<size_t>::iterator it = m_UseBuffers.begin(); it != m_UseBuffers.end();  ++it){
				CSoundBuffer & buf = m_Buffers[*it];
				if(buf.GetSoundType() == type)
					buf.SetEnable(IsEnableOfType(type));
			}
		}else{
			if(CMusic::sInst)
				CMusic::sInst->OnEnabled();
		}
	}
}

bool CSoundManager::SetListenerOrientation(int nDir,bool bApply)
{
	static stPointI dir[8]={
		stPointI(0,1),
			stPointI(1,1),
			stPointI(1,0),
			stPointI(-1,1),

			stPointI(-1,0),
			stPointI(-1,-1),
			stPointI(-1,0),
			stPointI(1,-1),
	};
	if(nDir < 0 || nDir > 7) return false;
	return SetListenerOrientation((float)dir[nDir].x,0.0f,(float)dir[nDir].y,0.0f,1.0f,0.0f,bApply);
}

DWORD CSoundManager::GetNewSoundId()
{
	++m_LastSoundId;
	//char szMsg[256];
	//sprintf(szMsg,"*** NewSoundId %u\n",m_LastSoundId);
	//OutputDebugString(szMsg);
	return m_LastSoundId;
}

void  CSoundManager::DeleteSoundId(DWORD soundId)
{
	tMapSoundIdToIndex::iterator it = m_mapSoundId.find(soundId);
	if(it != m_mapSoundId.end()){
		m_mapSoundId.erase(it);
		//char szMsg[256];
		//sprintf(szMsg,"*** DeleteSoundId %u, MapSoundIdSize %u\n",soundId,m_mapSoundId.size());
		//OutputDebugString(szMsg);
	}
}