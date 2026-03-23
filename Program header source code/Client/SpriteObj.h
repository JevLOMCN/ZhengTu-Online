#pragma once

#ifndef _SPIRTE_OBJ
#define _SPIRTE_OBJ

class CSpriteObj
{
public:
	// constructor 
	explicit CSpriteObj();
	~CSpriteObj();

	void  ResetSprite();

	// start 
	virtual void BeginSprite(BOOL bLoop = FALSE);

	// update 
	virtual void UpdateSprite(float fElaspedTime);

	// render 
	virtual void RenderSprite();

	// end 
	virtual void EndSprite(){};

	void SetRenderRect(const RECT & rect);
	const stRectI & GetRenderRect(){return rcRender;};
	void SetText(const char * pText);
	void SetFont(int  font);
	void SetTextColor(DWORD clr);
	void SetBkColor(DWORD clr);
	void SetBackGround(BOOL bRender);
	void SetSpeed(const stPointF & speed);
	void Stop();

	BOOL IsPlaying();

protected:
	// render 
	stRectI     rcRender;
	std::string strText;
	int         iFont;
	DWORD       dwTextColor; 
	DWORD       dwBkColor;

	float  fSpeedX,fSpeedY;

	// loop 
	BOOL bLoop;

	BOOL m_bBackGround;

	// physics
	stPointF  ptSrcPos,ptDstPos;
	stPointF  ptCurPos;

	float  fRunTime,fTotalTime;

	BOOL   bIsPlaying;

};
 
inline 
BOOL CSpriteObj::IsPlaying()
{
	return bIsPlaying;
}

inline 
void CSpriteObj::SetRenderRect(const RECT & rect)
{
	rcRender = rect;
}

inline  
void CSpriteObj::SetText(const char * pText)
{
	strText = pText;
}

inline 
void  CSpriteObj::SetFont(int  font)
{
	iFont = font;
}

inline 
void CSpriteObj::SetTextColor(DWORD color)
{
     dwTextColor = color;
}

inline 
void CSpriteObj::SetBkColor(DWORD color)
{
    dwBkColor = color;
}

inline
void CSpriteObj::SetSpeed(const stPointF & speed)
{
	fSpeedX = speed.x;
	fSpeedY = speed.y;
}

inline 
void CSpriteObj::SetBackGround(BOOL bRender)
{
	m_bBackGround = bRender;
}

inline 
void CSpriteObj::Stop()
{
	bIsPlaying = false;
}

class CGMTextEffect 
	: public CSpriteObj
{
	struct stTextItem
	{ 
		std::string szText;
		byte  nCount;
	};
	typedef std::vector<stTextItem> TEXTLIST;
	TEXTLIST m_vecText;

	int nCurCount;
public:
	// constructor 
	explicit CGMTextEffect();
	~CGMTextEffect();

    void BeginSprite(BOOL bLoop = false);
	void AddText(const char  * pText);

	void EndSprite();

};

class CMiniMapTextEffect
	: public CSpriteObj
{
public:
	// constructor 
	explicit CMiniMapTextEffect();
	~CMiniMapTextEffect();

	void BeginSprite(bool bLoop = false);
	void UpdateSprite(float fElaspedTime);
	void RenderSprite();

	enum enumMouseMode
	{
		MouseOnLeft,
		MouseOnRight,
		MouseOnNull,
	};
	enumMouseMode m_MouseMode;
	stRectI rcRenderLeft;
	stRectI rcRenderRight;
	bool IsAnimation()
	{
		return bAnimation;
	};
private:
	bool bAnimation; 
};

class CCreateRoleTextEffect 
	: public CSpriteObj
{
	IBitmap * m_pBitmap;  
	stPointF  m_fscale;
public:
	void SetRenderScale(const stPointF & scale){m_fscale = scale;};
	void SetBitmap(IBitmap * pBitmap){ m_pBitmap = pBitmap;};
	void BeginSprite(bool bLoop = false);
	void RenderSprite();
	void Release();
};


#endif 