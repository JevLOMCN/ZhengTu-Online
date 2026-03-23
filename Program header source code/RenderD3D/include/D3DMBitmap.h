#ifndef _D3DMBITMAP_H
#define _D3DMBITMAP_H
#include <hash_map>

class CD3DBitmap;
class CMBmpTextureCell;
class CMBmpTextureGroup;
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CD3DMBitmap{

	DWORD m_dwLastRenderTime;
	CD3DBitmap* m_pBmp;
	CD3DBitmap* m_pBmpAlpha;
	//IDirect3DTexture8* m_pMemTex;
public:
	CMBmpTextureCell* m_pTexture;
	CD3DMBitmap();
	~CD3DMBitmap();
	bool SetBitmap(CD3DBitmap* pBmp,CD3DBitmap* pAlpha);
	bool Render(int x,int y,DWORD color,BlendType blend );
	bool Render(int x,int y,const RECT* rect,const stPointF * scale,DWORD color,BlendType blend );
	bool BlendTexture(IDirect3DTexture8* pTex,D3DFORMAT dstFormat,POINT ptTex);
	void ReleaseTexture();
	bool LoadTexture(void);
	void SetLastRenderTime(DWORD dwTime) { m_dwLastRenderTime = dwTime; }
	DWORD GetLastRenderTime(){ return m_dwLastRenderTime;}
};

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CD3DMBitmapManager{
	
	typedef stdext::hash_map<ULONGLONG ,CD3DMBitmap*> tMBitmaps;
	std::lookaside_allocator<CD3DMBitmap,1024> m_allocMBitmap;
	tMBitmaps m_MBitmaps;
	void ClearFreeMBitmap();
	tMBitmaps::iterator FindMBitmap(CD3DBitmap* pBmp,CD3DBitmap* pAlpha);
	//IDirect3DTexture8* m_pMemTex;
public:
	//IDirect3DTexture8* GetMemTex(){ return m_pMemTex;}
	CMBmpTextureGroup* m_TexGroup;
	CD3DMBitmap* GetMBitmap(CD3DBitmap* pBmp,CD3DBitmap* pAlpha);
	void Release();
	CMBmpTextureGroup* GetTextureGroup();
	ULONGLONG GetKey(CD3DBitmap* pBmp,CD3DBitmap* pAlpha)
	{
		DWORD key[2] = {(DWORD)pBmp,(DWORD)pAlpha};
		return *(ULONGLONG*)key;
	}
	CD3DMBitmapManager()
		: m_TexGroup(NULL)
		//, m_pMemTex(NULL)
	{
	}

	void Create();
	void Destroy();
};

#endif