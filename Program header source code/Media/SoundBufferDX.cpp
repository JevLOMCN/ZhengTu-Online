/**
 * \file      SoundBufferDX.cpp
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-2-23 23:00:04
 * \brief     ？
 * 
 *	      
 */

#include "../engine/include/engine.h"
#include ".\soundbuffer.h"
#include "./SoundManager.h"
#include "./WaveModInfo.h"
#ifdef DX_SOUND_API

void CSoundBuffer::Destroy()
{
	for(int i=DSBUFFER_NUM-1; i>=0; --i)
	{
		if(m_aSoundId[i] != -1){
			GetSoundManager()->DeleteSoundId(m_aSoundId[i]);
			m_aSoundId[i] = -1;
		}
		SAFE_RELEASE(m_p3DDSBuffer[i]);
		SAFE_RELEASE(m_pDSBuffer[i]);
	}
}

bool CSoundBuffer::CreateSoundBuffer(WAVEFORMATEX * wfx,DWORD dataSize)
{
	HRESULT hr;
	DSBUFFERDESC dsbdesc;
	// Set up DSBUFFERDESC structure.
	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize        = sizeof(DSBUFFERDESC);
	dsbdesc.dwBufferBytes = dataSize; 
	dsbdesc.lpwfxFormat   = (LPWAVEFORMATEX)wfx;
	dsbdesc.dwFlags		  = DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME;
	if( m_b3D ){
		dsbdesc.dwFlags|=DSBCAPS_CTRL3D|DSBCAPS_MUTE3DATMAXDISTANCE;
		dsbdesc.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
	}

	IDirectSound8* pDS = GetSoundManager()->GetSystem();
	hr = pDS->CreateSoundBuffer(&dsbdesc,&m_pDSBuffer[0],NULL);
	if(FAILED(hr)){
		TRACE_DXERROR(hr);
		return false;
	}
	if(m_b3D && GetSoundManager()->IsEnable3D()){
		hr = m_pDSBuffer[0]->QueryInterface(IID_IDirectSound3DBuffer,(void**)&m_p3DDSBuffer[0]);
		if(FAILED(hr))
			TRACE_DXERROR(hr);
	}
	return true;
}

bool CSoundBuffer::Load(const char* filename)
{
	//For those player who have hardware problem with their PC's sound card
	if ( NULL == GetSoundManager()->GetSystem() )
		return false;
		
	Stream* pStream = OpenPackFileForRead(GetSoundManager()->GetFilePackName(),filename);
	if(!pStream)
		return false;

	CWaveModInfo wi(pStream);
	if(wi.mOk == false){
		ClosePackFileForRead(pStream);
		return false;
	}

	if(!CreateSoundBuffer((WAVEFORMATEX*)&wi.mFormat,wi.mDataSize)){
		ClosePackFileForRead(pStream);
		return false;
	}

	HRESULT hr;
	void* pLockBuffer,*pLockBuffer2;
	DWORD lockSize,lockSize2;

	hr = m_pDSBuffer[0]->Lock(0,wi.mDataSize,&pLockBuffer,&lockSize,&pLockBuffer2,&lockSize2,DSBLOCK_ENTIREBUFFER);
	if(FAILED(hr)){
		TRACE_DXERROR(hr);
		SAFE_RELEASE(m_pDSBuffer[0]);
		SAFE_RELEASE(m_p3DDSBuffer[0]);
		ClosePackFileForRead(pStream);
		return false;
	}

	wi.readData(pLockBuffer,lockSize);
	if(lockSize2)
		wi.readData(pLockBuffer2,lockSize2);
	hr = m_pDSBuffer[0]->Unlock(pLockBuffer,lockSize,pLockBuffer2,lockSize2);

	ClosePackFileForRead(pStream);

	if(FAILED(hr)){
		TRACE_DXERROR(hr);
		SAFE_RELEASE(m_pDSBuffer[0]);
		SAFE_RELEASE(m_p3DDSBuffer[0]);
		return false;
	}

	IDirectSound8* pDS = GetSoundManager()->GetSystem();
	if(m_b3D && GetSoundManager()->IsEnable3D()){
		for(int i=1; i<DSBUFFER_NUM;++i){
			hr = pDS->DuplicateSoundBuffer(m_pDSBuffer[0],&m_pDSBuffer[i]);
			if(FAILED(hr)){
				--i;
				for(; i >= 0; ++i){
					SAFE_RELEASE(m_pDSBuffer[i]);
					SAFE_RELEASE(m_p3DDSBuffer[i]);
				}
				TRACE_DXERROR(hr);
				return false;
			}
			hr = m_pDSBuffer[i]->QueryInterface(IID_IDirectSound3DBuffer,(void**)&m_p3DDSBuffer[i]);
			if(FAILED(hr)){
				TRACE_DXERROR(hr);
			}
		}
	}
	m_nCurrent = 0;
	return true;
}

void CSoundBuffer::SetEnable(bool bEnable)
{
	if(!bEnable) 
		StopAll();
	else
	{
		size_t num = (m_b3D ? DSBUFFER_NUM : 1);
		for(size_t i=0; i<num; ++i)
		{
			HRESULT hr;
			DWORD Status;
			IDirectSoundBuffer * Buffer;

			Buffer=m_pDSBuffer[i];
			hr = Buffer->GetStatus(&Status);

			if(FAILED(hr)) 
				Status = 0;

			if((Status & DSBSTATUS_LOOPING) == DSBSTATUS_LOOPING){
				Buffer->Play(0,0,DSBPLAY_LOOPING);
				m_LastUseTime = xtimeGetTime();
			}
		}
	}
}

bool CSoundBuffer::IsPlaying(int idx)
{
	IDirectSoundBuffer * Buffer;
	DWORD Status;
	Buffer=m_pDSBuffer[idx];
	HRESULT hr = Buffer->GetStatus(&Status);

	if(FAILED(hr)) 
		Status = 0;

	return ((Status & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING);
}

int CSoundBuffer::GetFreeBufferIdx(bool force)
{
	if(m_pDSBuffer[0] == NULL)
		return -1;

	HRESULT hr;
	DWORD Status;
	IDirectSoundBuffer * Buffer;

	Buffer=m_pDSBuffer[m_nCurrent];
	hr = Buffer->GetStatus(&Status);

	if(FAILED(hr)) 
		Status = 0;

	if((Status & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING){
		if(!m_b3D)
			return -1;

		int idx = ((m_nCurrent + 1) % DSBUFFER_NUM);

		while(idx != m_nCurrent){
	
			Buffer=m_pDSBuffer[idx];
			hr = Buffer->GetStatus(&Status);

			if(FAILED(hr)) 
				Status = 0;

			if((Status & DSBSTATUS_PLAYING) != DSBSTATUS_PLAYING)
				return idx;

			idx = ((idx+1) % DSBUFFER_NUM);
		}

		if(force){
			int retval = (m_nCurrent + 1) % DSBUFFER_NUM;
			m_pDSBuffer[retval]->Stop();
			return retval;
		}

		return -1;
	}

	return m_nCurrent;
}

void CSoundBuffer::Play(int idx,DWORD soundId,bool bLoop)
{
	if(m_pDSBuffer[idx]){
		m_nCurrent = idx;

		if(m_aSoundId[idx] != soundId)
		{
			if(m_aSoundId[idx] != -1)
				GetSoundManager()->DeleteSoundId(m_aSoundId[idx]);
			m_aSoundId[idx] = soundId;	
		}
		
		DWORD dwStatus;
		HRESULT hr = m_pDSBuffer[idx]->GetStatus(&dwStatus);
		if(SUCCEEDED(hr) && DSBSTATUS_BUFFERLOST == (dwStatus & DSBSTATUS_BUFFERLOST)){
			m_pDSBuffer[idx]->Restore();
		}
		m_pDSBuffer[idx]->Play(0,0,bLoop ? DSBPLAY_LOOPING : 0);
		m_LastUseTime = xtimeGetTime();
	}
}

void CSoundBuffer::Stop(int idx)
{
	if(m_pDSBuffer[idx]){
		m_pDSBuffer[idx]->Stop();
	}
}

void CSoundBuffer::SetMinMaxDistance(int idx,float minDist,float maxDist,bool bApply)
{
	if(m_p3DDSBuffer[idx]){
		m_p3DDSBuffer[idx]->SetMinDistance(minDist,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
		m_p3DDSBuffer[idx]->SetMaxDistance(maxDist,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
	}
}

void CSoundBuffer::SetPosition(int idx,float x,float y,float z,bool bApply)
{
	if(m_p3DDSBuffer[idx])
		m_p3DDSBuffer[idx]->SetPosition(x,y,z,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
}

void CSoundBuffer::SetVolume(int idx,float fVol)
{
	if(m_pDSBuffer[idx])
		m_pDSBuffer[idx]->SetVolume(VolumeFloatToInt(fVol));
}

bool CSoundBuffer::IsInit()
{
	return m_pDSBuffer[0] != NULL;
}

void CSoundBuffer::SetHead(int idx,bool bHead,bool bApply)
{
	if(m_p3DDSBuffer[idx]){
		m_p3DDSBuffer[idx]->SetMode(bHead ? DS3DMODE_DISABLE : DS3DMODE_NORMAL,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
		//m_p3DDSBuffer[idx]->SetMode(bHead ? DS3DMODE_HEADRELATIVE : DS3DMODE_NORMAL,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
		//if(bHead)
		//	m_p3DDSBuffer[idx]->SetPosition(0.0f,0.0f,0.0f,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
	}
}

#endif