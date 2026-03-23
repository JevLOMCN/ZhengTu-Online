#pragma once

/**
* \brief 简短描述
* 
* 详细描述
* 脚印特效类
*/
class CFootprint
{
private:
	IBitmaps*	m_image;
	DWORD	m_startTime;
	stPointF pt;

public:
	CFootprint(void);
	~CFootprint(void);

	int GetWidth(){ return ( m_image && m_image->GetFrameCount() ?  m_image->GetBitmap(0)->GetWidth() : 0 ); }
	int GetHeight(){ return ( m_image && m_image->GetFrameCount() ?  m_image->GetBitmap(0)->GetHeight() : 0 ); }

	void Draw(POINT ptMapPixelOffset);

	void Settime(DWORD stime);//设置开始时间
	void Setpos(stPointF pos);//设置特效位置
	void Setresourse(const char* filename, int group, int frame); //设置加载资源
};

extern CFootprint& GetCFootprint();
