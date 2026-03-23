#pragma once


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CCustomTexture
{
protected:
	int m_width,m_height;
	DWORD m_usage;
	D3DFORMAT m_format;
	IDirect3DTexture8* m_pTex;
public:
	CCustomTexture(int width,int height,D3DFORMAT format,DWORD usage = 0);
	virtual ~CCustomTexture(void);
	bool IsCreated(){ return m_pTex != NULL; }
	virtual bool Init();
	bool LoadFromJpegFile(const char* pszFile,RECT* pClipRect = NULL,const RECT* pRect = NULL);
	bool LoadFromJpegStream(Stream* pStream,RECT* pClipRect = NULL,const RECT* pRect = NULL);
	bool LoadFromTgaFile(const char* pszFile,RECT* pClipRect = NULL);
	bool LoadFromTgaStream(Stream* pStream,RECT* pClipRect = NULL);
	virtual void Release();
	IDirect3DTexture8* GetTex(){ return m_pTex; }

	bool Render(float x,float y,DWORD color = 0xffffffff,BlendType blend = Blend_Null);
	bool Render(float x,float y,const stRectF* rect,const stPointF * scale,DWORD color = 0xffffffff,BlendType blend = Blend_Null);
	
};

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CRenderTargetTexture : public CCustomTexture
{
	IDirect3DSurface8 * m_pOldRenderTarget;
	IDirect3DTexture8 * m_pTmpTex;
	//IDirect3DSurface8 * m_pRenderTarget;
public:
	CRenderTargetTexture(int width,int height,D3DFORMAT format) 
		: CCustomTexture(width,height,format,D3DUSAGE_RENDERTARGET)
		, m_pOldRenderTarget(NULL)
		, m_pTmpTex(NULL)
		//, m_pRenderTarget(NULL)
	{}

	~CRenderTargetTexture()
	{
		//SAFE_RELEASE(m_pRenderTarget);
		SAFE_RELEASE(m_pTmpTex);
	}

	void Release();
	bool Init();
	bool BeginRenderTarget();
	bool UpdateRenderTarget(const RECT* rect = NULL);
	bool EndRenderTarget();
};