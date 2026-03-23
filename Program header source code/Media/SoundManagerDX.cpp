/**
 * \file      SoundManagerDX.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2006-2-23 23:01:01
 * \brief     £¿
 * 
 *	      
 */

#include "../engine/include/engine.h"
#include ".\soundmanager.h"

#ifdef DX_SOUND_API

bool CSoundManager::Create(HWND hWnd,UINT nMsg)
{
	m_hWnd = hWnd;
	m_nMsg = nMsg;
	for(size_t i=0 ; i<SOUNDBUFF_NUM; ++i)
	{
		m_FreeBuffers.insert(i);
	}
	for(size_t i=0; i< count_of(m_aVolume);++i)
	{
		m_aVolume[i] = 1.0f;
		m_aEnable[i] = true;
		m_aVolumeFactor[i] = 1.0f;
	}

	HRESULT hr;
	hr = DirectSoundCreate8(NULL,&m_pSystem,NULL);
	if(FAILED(hr)) 
	{
		Engine_WriteLogF("DirectSoundCreate8 failed! %x\n",hr);
		return false;
	}
	hr = m_pSystem->SetCooperativeLevel(hWnd,DSSCL_EXCLUSIVE);
	if(FAILED(hr))
	{
		SAFE_RELEASE(m_pSystem);
		Engine_WriteLogF("SetCooperativeLevel failed! %x\n",hr);
		return false;
	}
	if(!SetPrimaryBufferFormat())
	{
		SAFE_DELETE(m_pSystem);
		Engine_WriteLogF("SetPrimaryBufferFormat failed!\n");
		return false;
	}
	if(!CreateListener())
	{
		SAFE_DELETE(m_pSystem);
		Engine_WriteLogF("CreateListener failed!\n");
		return false;
	}
	m_bEnable3D  = true;

	SetListenerOrientation(0);
	SetListenerPosition(0,0,0);
	CommitDeferredSettings();

	
	return true;
}

bool CSoundManager::CreateListener()
{
	HRESULT             hr;
	DSBUFFERDESC        dsbdesc;
	LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

	ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;

	hr = m_pSystem->CreateSoundBuffer(&dsbdesc,&pDSBPrimary,NULL);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	hr = pDSBPrimary->QueryInterface(IID_IDirectSound3DListener,(LPVOID*)&m_pListener);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}
	DS3DLISTENER dsparam;
	ZeroMemory(&dsparam,sizeof(DS3DLISTENER));
	dsparam.dwSize = sizeof(DS3DLISTENER);
	hr = m_pListener->GetAllParameters(&dsparam);
	SAFE_RELEASE(pDSBPrimary);
	return SUCCEEDED(hr);
}

bool CSoundManager::SetPrimaryBufferFormat()
{
	DSBUFFERDESC dsbd;
	WAVEFORMATEX wfx;
	HRESULT hr;
	IDirectSoundBuffer* pDSBPrimary = NULL;

	memset(&dsbd,0,sizeof(dsbd));
	memset(&wfx,0,sizeof(wfx));

	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
	hr = m_pSystem->CreateSoundBuffer(&dsbd,&pDSBPrimary,NULL);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	DSBCAPS m_caps;
	pDSBPrimary->GetCaps(&m_caps);

	//DWORD nSamplesPerSec  = 44100;
	DWORD nSamplesPerSec  = 22050;

_setFmt:
	wfx.cbSize = sizeof(wfx);
	wfx.nChannels = 2;
	wfx.wFormatTag = (WORD)WAVE_FORMAT_PCM;
	wfx.nSamplesPerSec = nSamplesPerSec;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
	wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);

	hr = pDSBPrimary->SetFormat(&wfx);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);

		if(nSamplesPerSec > 22050)
		{
			nSamplesPerSec = 22050;
			goto _setFmt;
		}
		else
		{
			goto _error;
		}
	}

	SAFE_RELEASE(pDSBPrimary);
	return true;
_error:
	//SAFE_RELEASE(m_pPrimaryBuffer);
	SAFE_RELEASE(pDSBPrimary);
	return false;
}

void CSoundManager::Destroy(void)
{
	for(tSetBufferIndex::iterator it = m_UseBuffers.begin(); it != m_UseBuffers.end(); ++it){
		CSoundBuffer & buf = m_Buffers[*it];
		buf.Destroy();
	}
	m_mapBuffer.clear();
	SAFE_RELEASE(m_pListener);
	SAFE_RELEASE(m_pSystem);
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CSoundManager::CommitDeferredSettings(void)
{
	if(m_pSystem)
	{
		m_pListener->CommitDeferredSettings();
		return true;
	}
	return false;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param xFront : ÃèÊö
* \param yFront : ÃèÊö
* \param zFront : ÃèÊö
* \param xTop : ÃèÊö
* \param yTop : ÃèÊö
* \param zTop : ÃèÊö
* \param bApply : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CSoundManager::SetListenerOrientation(float xFront, 
										   float yFront, 
										   float zFront, 
										   float xTop, 
										   float yTop, 
										   float zTop, 
										   bool  bApply )
{
	if(m_pSystem)
	{
		m_pListener->SetOrientation(xFront,yFront,zFront,xTop,yTop,zTop,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
		return true;
	}
	return false;
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
bool CSoundManager::SetListenerPosition(float fX, float fY, float fZ, bool bApply)
{
	if(m_pSystem)
	{
		m_pListener->SetPosition(fX,fY,fZ,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
		return true;
	}
	return false;
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
bool CSoundManager::SetListenerVelocity(float fX, float fY, float fZ, bool bApply)
{
	if(m_pSystem)
	{
		//m_pListener->SetVelocity(fX,fY,fZ,bApply ? DS3D_IMMEDIATE : DS3D_DEFERRED);
		return true;
	}
	return false;
}

#endif