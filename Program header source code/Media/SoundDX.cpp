#include "../engine/include/engine.h"
#include ".\sound.h"
#include ".\soundmanager.h"

#ifdef DX_SOUND_API

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
CSound::CSound(void)
: m_b3D(false)
, m_bLoop(false)
, m_dwDeleteTime(0)
, m_bDeleted(false)
, m_eType(SoundType_None)
, m_fVolume(1.0f)
, m_bEnabled(true)
, m_pParent(NULL)
{
	m_szFileName[0] = 0;

	m_pSound = NULL;
	m_p3DSound = NULL;

	InitAttributes();
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
void CSound::ClearSoundBuffer(void)
{
	SAFE_RELEASE(m_p3DSound);
	SAFE_RELEASE(m_pSound);
}

void CSound::ApplyAttributes()
{
	HRESULT result;
	if(m_b3D)
	{
		if(!m_p3DSound) return;
		result = m_p3DSound->SetMinDistance(m_fMinDis,DS3D_IMMEDIATE);
		result = m_p3DSound->SetMaxDistance(m_fMaxDis,DS3D_IMMEDIATE);
		if(m_bHeader){
			result = m_p3DSound->SetPosition(0,0,0,DS3D_IMMEDIATE);
			result = m_p3DSound->SetMode( DS3DMODE_HEADRELATIVE,DS3D_IMMEDIATE);
		}else{
			result = m_p3DSound->SetPosition(m_pos.x,m_pos.y,m_pos.z,DS3D_IMMEDIATE);
			result = m_p3DSound->SetMode( DS3DMODE_NORMAL,DS3D_IMMEDIATE);
		}
	}
	ApplyFinalVolume();
}


void CSound::ApplyFinalVolume()
{
	if(m_pSound){
		LONG lVol = VolumeFloatToInt(GetFinalVolume());
		for(size_t i=0; i<10; ++i){
			HRESULT hr = m_pSound->SetVolume(lVol);
			if(FAILED(hr)){
				TRACE_DXERROR(hr);
			}else{
				long l;
				hr = m_pSound->GetVolume(&l);
				if(FAILED(hr)){
					TRACE_DXERROR(hr);
				}else{
					if(l == lVol)
						break;
				}
			}
		}
	}
}

bool CSound::InitSound(const char* szFileName,enumSoundType type, bool b3D)
{
	m_b3D = b3D;
	m_eType = type;
	ClearSoundBuffer();
	strncpy(m_szFileName,szFileName,sizeof(m_szFileName));
	m_szFileName[sizeof(m_szFileName)-1] = 0;
	return true;
}

bool CSound::SetDSBuffer(IDirectSoundBuffer* pDSBuffer)
{
	DWORD dwStatus;
	HRESULT hr = pDSBuffer->GetStatus(&dwStatus);
	if(SUCCEEDED(hr) && 0 != (dwStatus & DSBSTATUS_BUFFERLOST)){
		pDSBuffer->Restore();
	}
	m_pSound = pDSBuffer;
	pDSBuffer->AddRef();

	if(m_b3D)
	{
		HRESULT hr = m_pSound->QueryInterface(IID_IDirectSound3DBuffer,(void**)&m_p3DSound);
		if(FAILED(hr))
		{
			TRACE_DXERROR(hr);
			SAFE_RELEASE(m_pSound);
			return false;
		}
	}
	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CSound::IsPlaying()
{
	if( !IsEnable() ) return false;
	if(!m_pSound) return false;

	DWORD dwStatus;
	if(FAILED(m_pSound->GetStatus(&dwStatus)))
		return false;
	return 0 != (dwStatus & DSBSTATUS_PLAYING);

}

bool CSound::IsLooping()
{
	if( !IsEnable() ) return true;

	if(!m_pSound) return false;
	DWORD dwStatus;
	if(FAILED(m_pSound->GetStatus(&dwStatus)))
		return false;
	return 0 != (dwStatus & DSBSTATUS_LOOPING);

}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param bLoop : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CSound::Play(bool bLoop)
{	
	if(!InitPlay()){
		return false;
	}

	if( IsPlaying() )
	{
		if(m_bLoop && bLoop) return true;
		Stop();
	}

	m_bLoop = bLoop;

	if(m_pSound){
		HRESULT hr = m_pSound->Play(0,0,m_bLoop ? DSBPLAY_LOOPING : 0);
		if(hr == DSERR_BUFFERLOST){
			/*hr = m_pSound->Restore();
			if( hr == DS_OK )
			{
				if( GetSoundManager()->LoadSound(m_szFileName,m_b3D,m_pSound) )
				{
					ApplyAttributes();
					hr = m_pSound->Play(0,0,m_bLoop ? DSBPLAY_LOOPING : 0);
				}				
			}	*/		
		}
		if(FAILED(hr)){
			TRACE_DXERROR(hr);
		}else{
			ApplyAttributes();
		}
	}

	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
void CSound::Stop(void)
{
	if(m_pSound)
	{
		m_pSound->Stop();
		m_pSound->SetCurrentPosition(0);
	}
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param fDistance : ÃèÊö
* \param bApply : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CSound::SetMinMaxDistance(float fMinDistance, float fMaxDistance,bool bApply)
{
	m_fMinDis = fMinDistance;
	m_fMaxDis = fMaxDistance;
	if(!m_p3DSound) return false;
	m_p3DSound->SetMinDistance(fMinDistance,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
	m_p3DSound->SetMaxDistance(fMaxDistance,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
	return true;
}

bool CSound::SetMinDistance(float fMinDistance,bool bApply)
{
	m_fMinDis = fMinDistance;
	if(!m_p3DSound) return false;
	m_p3DSound->SetMinDistance(fMinDistance,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
	return true;
}

bool CSound::SetMaxDistance(float fMaxDistance,bool bApply)
{
	m_fMaxDis = fMaxDistance;
	if(!m_p3DSound) return false;
	m_p3DSound->SetMaxDistance(fMaxDistance,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param dwMode : ÃèÊö
* \param bApply : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CSound::SetHeader(bool bHeader, bool bApply)
{
	m_bHeader = bHeader;
	if(!m_p3DSound) return false;
	m_p3DSound->SetMode( m_bHeader ? DS3DMODE_DISABLE : DS3DMODE_NORMAL,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
	return true;
}

void CSound::SetPause( bool bPause )
{
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param fX : ÃèÊö
* \param fY : ÃèÊö
* \param fZ : ÃèÊö
* \param bApply : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CSound::SetPosition(float fX, float fY, float fZ, bool bApply)
{
	m_pos.x = fX;
	m_pos.y = fY;
	m_pos.z = fZ;
	if(!m_p3DSound) return false;
	if(!m_bHeader)
		m_p3DSound->SetPosition(fX,fY,fZ,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
	return true;
}

bool CSound::SetVelocity(float fX, float fY, float fZ, bool bApply)
{
	m_vel.x = fX;
	m_vel.y = fY;
	m_vel.z = fZ;
	if(!m_p3DSound) return false;
	m_p3DSound->SetVelocity(fX,fY,fZ,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param lVolume : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CSound::SetVolume(float fVolume)
{
	m_fVolume = fVolume;
	if(!m_pSound) return false;
	ApplyFinalVolume();
	return true;
}


unsigned int CSound::GetTimeLength()
{
	unsigned int dwLength = 0 ;
	return dwLength;
}

void CSound::SetLoop(bool bLoop)
{
	DWORD curPlayPos = 0;
	DWORD curWritePos = 0;
	if(m_pSound) m_pSound->GetCurrentPosition(&curPlayPos,&curWritePos);
	Stop();
	Play(true);
	if(curPlayPos)
	{
		if(m_pSound) m_pSound->SetCurrentPosition(curPlayPos);
	}
}

#endif