/**
 * \file      SoundBuffer.cpp
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-2-23 23:00:38
 * \brief     ？
 * 
 *	      
 */

#include "../engine/include/engine.h"
#include ".\soundbuffer.h"
#include "./SoundManager.h"

CSoundBuffer::CSoundBuffer()
: m_LastUseTime(0)
, m_fileId(-1)
, m_b3D(false)
{
#ifdef DX_SOUND_API
	memset(m_pDSBuffer,0,sizeof(m_pDSBuffer));
	memset(m_p3DDSBuffer,0,sizeof(m_p3DDSBuffer));
#endif
	memset(m_aSoundId,0xff,sizeof(m_aSoundId));
}

void CSoundBuffer::Init(DWORD fileId,bool b3D,enumSoundType type)
{
	m_fileId = fileId;
	m_b3D = (b3D);
	m_eType = (type);
	m_nCurrent = (-1);
}

CSoundBuffer::~CSoundBuffer(void)
{
	Destroy();
}

void CSoundBuffer::StopAll()
{
	if(m_b3D){
		for(size_t i=0; i<DSBUFFER_NUM; ++i)
			Stop(i);
	}else{
		Stop(0);
	}
}

void CSoundBuffer::SetAllVolume(float fVol)
{
	if(m_b3D){
		for(size_t i=0; i<DSBUFFER_NUM; ++i)
			SetVolume(i,fVol);
	}else{
		SetVolume(0,fVol);
	}
}

int CSoundBuffer::GetPlayingNum()
{
	int retval = 0;
	size_t num = (m_b3D ? DSBUFFER_NUM : 1);
	for(size_t i=0; i<num; ++i)
	{
		if(IsPlaying(i))
			retval++;
	}
	return retval;
}