#include "../../engine/include/engine.h"
#include "../include/D3DDevice.h"
#include "../include/d3dlightmap.h"
#include "../Include/RenderD3D.h"
#include "../include/TextureManager.h"
#include "../include/CustomTexture.h"

const size_t c_nLightSize = 256;

 
/*
*	º¯Êı:		
*	·µ»ØÖµ£º	
*	²ÎÊı£º		
*	
*	ÓÃÍ¾: 		?
*/

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDstSurface : ÃèÊö
 * \param pSrcSurface : ÃèÊö
 * \param POINT* : ÃèÊö
 * \param RECT : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CopyD3DSurface(IDirect3DSurface8* pDstSurface,IDirect3DSurface8* pSrcSurface,const POINT* pDstPoint,const RECT * pSrcRect)
{
	return GetD3DDevice()->__CopyRects(pSrcSurface,pSrcRect,(pSrcRect ? 1 : 0),pDstSurface,pDstPoint);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param * : ÃèÊö
 * \param pSrcSurface : ÃèÊö
 * \param POINT* : ÃèÊö
 * \param RECT : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CopyD3DSurfaceToTexture(IDirect3DTexture8 * pDstTexture,IDirect3DSurface8* pSrcSurface,const POINT* pDstPoint,const RECT * pSrcRect)
{
	HRESULT hr;
	IDirect3DSurface8* pDstSurface;
	hr = pDstTexture->GetSurfaceLevel(0,&pDstSurface);
	
	if(FAILED(hr))
		return hr;

	hr = CopyD3DSurface(pDstSurface,pSrcSurface,pDstPoint,pSrcRect);

	SAFE_RELEASE(pDstSurface);
	return hr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CD3DLightMap::CD3DLightMap(void)
: m_pTexLight(NULL)
, m_pTexRender(NULL)
, m_bGenerate(false)
, m_crEnvirment(0.0f,0.0f,0.0f,0.0f)
{
	m_bEnabled = (false);
	m_aLight.reserve(c_maxRectangleNum);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CD3DLightMap::~CD3DLightMap(void)
{
	Release();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CD3DLightMap::Init()
{
	if(!m_pTexRender)
	{
		D3DFORMAT format = GetD3DDevice()->GetBackFormat();
		m_pTexRender = GetTextureManager()->CreateRenderTargetTexture(c_nLightSize,c_nLightSize,format);
		if(!m_pTexRender) return false;
	}
	if(!m_pTexRender->GetTex() && !m_pTexRender->Init()) return false;

	if(!m_pTexLight)
	{
		m_pTexLight = GetTextureManager()->CreateCurstomTexture(c_nLightSize,c_nLightSize,D3DFMT_A8R8G8B8);
		if(!m_pTexLight) m_pTexLight = GetTextureManager()->CreateCurstomTexture(c_nLightSize,c_nLightSize,D3DFMT_A4R4G4B4);
		if(!m_pTexLight)
		{
			Release();
			return false;
		}
	}

	if(m_pTexLight->GetTex() == NULL)
	{
		Stream* pStream  = OpenPackFileForRead("data\\datas.pak","datas\\light\\light6.tga");
		if(!pStream)
		{
			Release();
			return false;
		}
		stRectI clip;
		if( !m_pTexLight->LoadFromTgaStream(pStream,&clip) )
		{
			Release();
			ClosePackFileForRead(pStream);
			return false;
		}
		ClosePackFileForRead(pStream);

		/*m_uvTex.left = (float)clip.left/(float)c_nLightSize;
		m_uvTex.top = (float)clip.top/(float)c_nLightSize;
		m_uvTex.right = (float)clip.right/(float)c_nLightSize;
		m_uvTex.bottom = (float)clip.bottom/(float)c_nLightSize;*/
		m_uvTex.left = 0;
		m_uvTex.top = 0;
		m_uvTex.right = 1;
		m_uvTex.bottom = 1;

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
void CD3DLightMap::Release()
{
	if(m_pTexRender)
	{
		GetTextureManager()->DeleteCustomeTexture(m_pTexRender);
		m_pTexRender = NULL;
	}

	if(m_pTexLight)
	{
		GetTextureManager()->DeleteCustomeTexture(m_pTexLight);
		m_pTexLight = NULL;
	}
	
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param clr : ÃèÊö
 * \param iScreenWidth : ÃèÊö
 * \param iScreenHeight : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline bool CD3DLightMap::_GenerateLightMap(DWORD clr,int iScreenWidth,int iScreenHeight)
{
	D3DRECT d3dRectBack = {0,0,256,256};
	stVertexBitmap * aVertex;
	HRESULT hr;
	DWORD nStartVertex;
	
	//hr = GetD3DDevice()->GetVB()->Lock( 0 , sizeof(stVertexBitmap) * m_aLight.size() * 4, (BYTE**)&aVertex,0);
#ifdef USE_INDEX_BUFFER
	hr = GetD3DDevice()->LockVertexBuffer(m_aLight.size() * 4,(void**)&aVertex,nStartVertex);
#else
	hr = GetD3DDevice()->LockVertexBuffer(m_aLight.size() * 6,(void**)&aVertex,nStartVertex);
#endif
	if(FAILED(hr)) goto _error;

	const float c_error = 0.5f;

	for(size_t i=0;i<m_aLight.size();++i)
	{

		stLight& st = m_aLight[i];
		st.ptLight.x  = st.ptLight.x * c_nLightSize/iScreenWidth;
		st.ptLight.y  = st.ptLight.y * c_nLightSize/iScreenHeight;
		st.LightSize.x = st.LightSize.x * c_nLightSize/iScreenWidth;
		st.LightSize.y = st.LightSize.y * c_nLightSize/iScreenHeight;

		const float c_f1 = 1.0f;

#ifdef USE_INDEX_BUFFER

		aVertex[0].v = D3DXVECTOR4( st.ptLight.x - st.LightSize.x/2 * c_f1 - c_error , st.ptLight.y - st.LightSize.y/2 - c_error,0.0f,1.0f );
		aVertex[1].v = D3DXVECTOR4( st.ptLight.x - st.LightSize.x/2  - c_error , aVertex[0].v.y + st.LightSize.y,0.0f,1.0f );
		aVertex[2].v = D3DXVECTOR4( st.ptLight.x + st.LightSize.x/2 * c_f1 - c_error , aVertex[0].v.y,0.0f,1.0f );
		aVertex[3].v = D3DXVECTOR4( st.ptLight.x + st.LightSize.x/2  - c_error,aVertex[1].v.y,0.0f,1.0f );

		aVertex[0].uv = D3DXVECTOR2(m_uvTex.left,m_uvTex.top);
		aVertex[1].uv = D3DXVECTOR2(m_uvTex.left,m_uvTex.bottom);
		aVertex[2].uv = D3DXVECTOR2(m_uvTex.right,m_uvTex.top);
		aVertex[3].uv = D3DXVECTOR2(m_uvTex.right,m_uvTex.bottom);

		aVertex[0].color = st.color;
		aVertex[1].color = st.color;
		aVertex[2].color = st.color;
		aVertex[3].color = st.color;

		aVertex+=4;

#else

		aVertex[0].v = D3DXVECTOR4( st.ptLight.x - st.LightSize.x/2 * c_f1 - c_error , st.ptLight.y - st.LightSize.y/2 - c_error,0.0f,1.0f );
		aVertex[1].v = D3DXVECTOR4( st.ptLight.x - st.LightSize.x/2  - c_error , aVertex[0].v.y + st.LightSize.y,0.0f,1.0f );
		aVertex[2].v = D3DXVECTOR4( st.ptLight.x + st.LightSize.x/2 * c_f1 - c_error , aVertex[0].v.y,0.0f,1.0f );
		aVertex[3].v = D3DXVECTOR4( st.ptLight.x + st.LightSize.x/2  - c_error,aVertex[1].v.y,0.0f,1.0f );

		aVertex[0].uv = D3DXVECTOR2(m_uvTex.left,m_uvTex.top);
		aVertex[1].uv = D3DXVECTOR2(m_uvTex.left,m_uvTex.bottom);
		aVertex[2].uv = D3DXVECTOR2(m_uvTex.right,m_uvTex.top);
		aVertex[3].uv = D3DXVECTOR2(m_uvTex.right,m_uvTex.bottom);

		aVertex[0].color = st.color;
		aVertex[1].color = st.color;
		aVertex[2].color = st.color;
		aVertex[3].color = st.color;

		aVertex[4] = aVertex[2];
		aVertex[5] = aVertex[1];

		aVertex+=6;

#endif

	}

	hr = GetD3DDevice()->UnlockVertexBuffer();
	if(FAILED(hr)) goto _error;

#ifdef USE_INDEX_BUFFER

	BYTE* pIndex;
	DWORD startIndex;
	//hr = GetD3DDevice()->GetIB()->Lock(0,GetD3DDevice()->GetD3DIndexSize() * m_aLight.size() * 6,&pIndex,0);
	hr = GetD3DDevice()->LockIndexBuffer(m_aLight.size()*6,(void**)&pIndex,startIndex);
	if(FAILED(hr)) goto _error;

	FillIndexBuffer(pIndex,GetD3DDevice()->GetD3DIndexSize(),0,m_aLight.size());

	hr = GetD3DDevice()->UnlockIndexBuffer();
	if(FAILED(hr)) goto _error;

#endif

	hr = GetD3DDevice()->BeginScene();
	if(FAILED(hr)) goto _error;

	if ( clr == 0xff000000 )			//Dard scene
		m_crEnvirment = D3DXCOLOR(0,0,0,1);
	else
		m_crEnvirment += (D3DXCOLOR)clr;
	m_crEnvirment.a = 1.0f;
	hr = GetD3DDevice()->__Clear(1,&d3dRectBack,D3DCLEAR_TARGET,m_crEnvirment,1.0f,0);
	if(FAILED(hr)) 
	{
		GetD3DDevice()->EndScene();
		goto _error;
	}

	GetD3DDevice()->SetStreamSource(sizeof(stVertexBitmap),D3DFVF_BITMAP);

#ifdef USE_INDEX_BUFFER

	hr = GetD3DDevice()->__SetIndices(GetD3DDevice()->GetIB(),nStartVertex);

#endif

	hr = GetD3DDevice()->__SetTexture(0,m_pTexLight->GetTex());

	GetD3DDevice()->SetDeviceDrawState(CD3DDevice::DrawStateTex,Blend_AlphaAdd5);

#ifdef USE_INDEX_BUFFER
	
	hr = GetD3DDevice()->__DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_aLight.size()*4,startIndex,m_aLight.size()*2);

#else
	
	hr = GetD3DDevice()->__DrawPrimitive(D3DPT_TRIANGLELIST,nStartVertex,m_aLight.size()*2);

#endif

	hr = GetD3DDevice()->EndScene();

	
	if(FAILED(hr)) goto _error;
	
	return true;
_error:
	TRACE_DXERROR(hr);
	return false;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param clr : ÃèÊö
 * \param iScreenWidth : ÃèÊö
 * \param iScreenHeight : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CD3DLightMap::GenerateLightMap(DWORD clr,int iScreenWidth,int iScreenHeight)
{
	m_bGenerate = false;
	if(!m_bEnabled) return;
	
	// È«°××´Ì¬²»ÓÃ´¦Àí¹âÕÕ
	if(m_crEnvirment.r >= 1.0f && m_crEnvirment.g >= 1.0f && m_crEnvirment.b >= 1.0f)
		return;
	if(m_aLight.empty())
		return;

	HRESULT hr = 0;
	
	D3DRECT rc1 = {0,0,c_nLightSize,c_nLightSize};
	m_bGenerate = true;

	if(GetD3DDevice()->TestCooperativeLevel() == false)
		return;
	if(!Init()) return;

	if(!m_pTexRender->BeginRenderTarget())
	{
		m_bGenerate = false;
		return;
	}
	//GetD3DDevice()->SetBufferLockFlags(true);
	if(!_GenerateLightMap(clr,iScreenWidth,iScreenHeight))
			m_bGenerate = false;
	//GetD3DDevice()->SetBufferLockFlags(false);
	//D3DXSaveTextureToFile("c:\\111.bmp",D3DXIFF_BMP,m_pTexRender->GetTex(),NULL);

	if(!m_pTexRender->UpdateRenderTarget())
		m_bGenerate = false;

	m_pTexRender->EndRenderTarget();
	
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CD3DLightMap::BeginLight(void)
{
	if(!m_bEnabled) return;
	m_aLight.resize(0);
	m_crEnvirment = D3DXCOLOR(0.0f,0.0f,0.0f,0.0f);
	m_bGenerate = false;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param clr : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CD3DLightMap::AddEnvirmentLight(DWORD clr)
{
	if(!m_bEnabled) return;
	m_crEnvirment += (D3DXCOLOR)clr;
}

void CD3DLightMap::DecEnvirmentLight(DWORD clr)
{
	if(!m_bEnabled) return;
	m_crEnvirment -= (D3DXCOLOR)clr;
}

void CD3DLightMap::SetEnvirmentLight( DWORD clr )
{
	if ( !m_bEnabled ) return;
	
	m_crEnvirment = (D3DXCOLOR)clr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \param size : ÃèÊö
 * \param clr : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CD3DLightMap::AddLight(stPointI pt,stPointI size,DWORD clr)
{
	if(!m_bEnabled) return;
	if(m_aLight.size() >= c_maxRectangleNum) return;
	if(!m_pTexRender || !m_pTexLight) return;
	stLight st;
	st.color = clr;
	st.LightSize = size;
	st.pTexture = NULL;
	st.ptLight = pt;
	m_aLight.push_back(st);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param RECT : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CD3DLightMap::Draw(const RECT * rc)
{
	if(!m_bEnabled) return;
	if(!m_bGenerate) return;
	if(!m_pTexRender || !m_pTexRender->GetTex()) return;
	GetD3DDevice()->DrawTexture(m_pTexRender->GetTex(),rc,NULL,0xffffffff,Blend_MulColor);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param b : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CD3DLightMap::SetEnabled(bool b)
{
	if ( b && !m_bEnabled )
	{
		Init();
	}
	m_bEnabled = b;
	if(!b) Release();
}
