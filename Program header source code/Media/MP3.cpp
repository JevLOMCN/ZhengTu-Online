#include "../engine/include/engine.h"
#include "./SoundManager.h"
#include ".\MP3.h"

#pragma comment( lib ,"strmiids.lib")

CMP3::CMP3(const char* filename,enumSoundType type)
: m_pGB(NULL)
, m_pMC(NULL)
, m_pMS(NULL)
, m_pME(NULL)
, m_pBA(NULL)
, m_pBF(NULL)
, m_hWnd(NULL)
, m_bLoad(false)
, m_eType(type)
, m_fVolume(1.0f)
{
	LoadFile(filename);
}

CMP3::~CMP3(void)
{
	UnInit();
}

bool CMP3::Init()
{
	UnInit();
	HRESULT hr;
	// Create DirectShow Graph
	hr = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)(&m_pGB));
	if( FAILED(hr) )
	{
		TRACE_DXERROR(hr);
		return false;
	}

	// Get the IMediaControl Interface
	hr = m_pGB->QueryInterface(IID_IMediaControl,(void **)(&m_pMC));
	if( FAILED(hr) )
	{
		TRACE_DXERROR(hr);
		return false;
	}
	// Get the IMediaControl Interface
	hr = m_pGB->QueryInterface(IID_IMediaSeeking,(void **)(&m_pMS));
	if( FAILED(hr) )
	{
		TRACE_DXERROR(hr);
		return false;
	}

	hr = m_pMS->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME );
	if( FAILED(hr) )
	{
		TRACE_DXERROR(hr);
		return false;
	}

	hr = m_pGB->QueryInterface(IID_IBasicAudio, (void **)&m_pBA);
	if( FAILED(hr) )
	{
		TRACE_DXERROR(hr);
		return false;
	}

	hr = m_pGB->QueryInterface(IID_IMediaEventEx, (void **)&m_pME);
	if( FAILED(hr) )
	{
		TRACE_DXERROR(hr);
		return false;
	}
	return true;
}

void CMP3::UnInit(void)
{
	SAFE_RELEASE(m_pME);
	SAFE_RELEASE(m_pBA);
	SAFE_RELEASE(m_pMS);
	SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pGB);
	SAFE_RELEASE(m_pBF);
}

bool CMP3::LoadFile(const char* pszFileName)
{

	Init();

	WCHAR szFileName[MAX_PATH];
	mbstowcs(szFileName,pszFileName,strlen(pszFileName) + 1);
	HRESULT hr;
	if(!m_pGB || !m_pME) 
		return false;
	hr = m_pGB->RenderFile(szFileName,NULL);

	if(FAILED(hr)) 
	{
		TRACE_DXERROR(hr);
		return false;
	}

	/*hr = m_pME->SetNotifyWindow((OAHWND)m_hWnd,GetSoundManager()->GetWndMsg(),(LONG_PTR)this);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}*/

	m_bLoad = true;
	return true;
}

void CMP3::ApplyFinalVolume()
{
	if(!m_pBA) 
		return;
	m_pBA->put_Volume(VolumeFloatToInt(GetSoundManager()->GetVolumeOfType(m_eType) * m_fVolume));
}

bool CMP3::Play()
{
	if(!IsEnable()) 
		return true;

	if(!m_bLoad) 
	{
		return false;
	}

	HRESULT hr;

	if(!m_pMC) 
		return false;

	ApplyFinalVolume();

	hr = m_pMC->Run();
	
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	return true;
}

bool CMP3::IsPause()
{
	return GetState() == State_Paused;
}

bool CMP3::IsPlaying()
{
	return GetState() == State_Running;
}

void CMP3::Stop(void)
{
	if(!m_pMC) 
		return ;

	m_pMC->Stop();
}

FILTER_STATE CMP3::GetState()
{
	HRESULT hr;
	if(!m_pMC) return State_Stopped;
	OAFilterState fs;
	hr = m_pMC->GetState(1,&fs);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return State_Stopped;
	}
	return (FILTER_STATE)fs;
}

LONGLONG CMP3::GetTimeLength()
{
	if(!m_pMS)
		return 0;
	LONGLONG stopPos;
	HRESULT hr = m_pMS->GetStopPosition(&stopPos);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return 0;
	}
	return stopPos;
}

bool CMP3::SetVolume(float fVolume)
{
	m_fVolume = fVolume;
	ApplyFinalVolume();
	return true;
}

void CMP3::SetPause(bool bPause)
{
	if(!m_pMC) return;
	if(bPause)
	{
		if(GetState() == State_Running)
			m_pMC->Pause();
	}
	else if(GetState() == State_Paused)
		Play();
}

LONGLONG CMP3::GetPosition()
{
	if(!m_pMS) return -1;
	LONGLONG pos,stopPos;
	m_pMS->GetPositions(&pos,&stopPos);
	return pos;
}

bool CMP3::SetPosition(LONGLONG pos)
{
	if(!m_pMS) 
		return false;
	return SUCCEEDED( m_pMS->SetPositions(&pos, AM_SEEKING_AbsolutePositioning ,NULL, AM_SEEKING_NoPositioning) );
}

bool CMP3::IsEnable()
{
	return GetSoundManager()->IsEnableOfType(m_eType);
}