#pragma once

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class	CAnimation
{
	IBitmaps* m_pImages;
	float	  m_fSpeed;
	float	  m_fColorTime;
	D3DXCOLOR m_CurrentColor;
	D3DXCOLOR m_EndColor;
	BlendType m_BlendType;
	float	  m_fFrame;
	bool	  m_bLoop;
	bool	  m_bStop;
	bool	  m_bRandomPlay;
	WORD 	  m_StartFramel;
	WORD 	  m_EndFrame;	
public:

	CAnimation(void);
	~CAnimation(void);

	void SetLoopPlay(bool b) { m_bLoop = b;}
	void SetRandomPlay(bool bRandomly = true) { m_bRandomPlay = bRandomly; }
	void SetStop(bool b) { m_bStop = b;}
	bool IsLoopPlay(){ return m_bLoop;}
	bool IsPlaying(){ return ! m_bStop;}
	bool IsRevrese(){ return m_fSpeed < 0;}
	void Revrese(){ m_fSpeed = - m_fSpeed;}

	int GetFrameCount();
	void SetFrame(float iFrame);
	float  GetFrame();
	bool Create(const stResourceLocation* prl,bool bRandomBeginFrame = false);
	bool Create(IBitmaps* pBitmaps,bool bRandomBeginFrame =  false);
	bool Create(const stResourceLocation* prl,WORD startFramel,WORD endFrame,bool bRandomBeginFrame = false);

	void SetColor(DWORD BeginColor,DWORD EndColor,DWORD dwTime)
	{
		m_CurrentColor = BeginColor;
		m_EndColor = EndColor;
		m_fColorTime = 1000.f/(float)dwTime;
	}
	
	void SetColor(DWORD dwColor)
	{
		m_CurrentColor = dwColor;
		m_fColorTime = 0;
	}

	DWORD GetColor(){ return m_CurrentColor;}

	void SetBlendType(BlendType type)
	{
		m_BlendType = type;
	}

	void SetSpeed(DWORD dwTime)
	{
		m_fSpeed = 1000.f/(float)dwTime;
	}

	void SetSpeed2(float speed)
	{
		m_fSpeed = speed;
	}

	float GetSpeed();
	
	void UpdateFrame();

	void Render(int x,int y,bool bUpdateFrame = true);
	void Render(int x,int y,const RECT* rcClip,const stPointF* scale,bool bUpdateFrame = true);

	void Render(int x,int y,DWORD color,bool bUpdateFrame = true);
	void Render(int x,int y,const RECT* rcClip,const stPointF* scale,DWORD color,bool bUpdateFrame = true);

	IBitmap* GetFrame(int iFrame);
	int GetWidth(int iFrame = 0);
	int GetHeight(int iFrame = 0);

	CAnimation& operator = (const CAnimation& vAni);
	int GetAlpha(POINT pt);

	bool IsNullImage(int iFrame);
	IBitmaps* GetImages(){ return m_pImages;}
};
