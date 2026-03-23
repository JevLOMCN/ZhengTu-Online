#pragma once

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class ILightMap
{
protected:
	bool			   m_bEnabled;
public:
	virtual void AddEnvirmentLight(DWORD clr) = 0;
	virtual void DecEnvirmentLight(DWORD clr) = 0;
	virtual void SetEnvirmentLight(DWORD clr) = 0;
	virtual void AddLight(stPointI pt,stPointI size,DWORD clr) = 0;
	void AddLight(stPointI pt,int size,DWORD clr)
	{
		AddLight(pt,stPointI(size,size/2),clr);
	}

	virtual void GenerateLightMap(DWORD clr,int iScreenWidth,int iScreenHeight) = 0;
	virtual void Draw(const RECT * rc) = 0;
	virtual void BeginLight() = 0;
	virtual void SetEnabled(bool b) = 0;
	bool IsEnabled(){ return m_bEnabled;}
};
