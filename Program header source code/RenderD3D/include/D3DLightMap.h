#pragma once
#include "../../engine/include/ilightmap.h"

class CRenderTargetTexture;
class CCustomTexture;

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CD3DLightMap :
	public ILightMap
{
	struct stLight
	{
		stPointI ptLight;
		stPointI LightSize;
		DWORD color;
		IBitmap* pTexture;
	};
	std::vector<stLight> m_aLight;
	CCustomTexture* m_pTexLight;
	CRenderTargetTexture* m_pTexRender;
	bool			   m_bGenerate;
	D3DXCOLOR		   m_crEnvirment;
	stRectF			   m_uvTex;
public:
	CD3DLightMap(void);
	~CD3DLightMap(void);
	bool Init();
	void Release();
	void GenerateLightMap(DWORD clr,int iScreenWidth,int iScreenHeight);
	bool _GenerateLightMap(DWORD clr,int iScreenWidth,int iScreenHeight);
	void BeginLight(void);
	void AddEnvirmentLight(DWORD clr);
	void DecEnvirmentLight(DWORD clr);
	void SetEnvirmentLight(DWORD clr);
	void AddLight(stPointI pt,stPointI size,DWORD clr);
	void Draw(const RECT * rc);
	void SetEnabled(bool b);
};
