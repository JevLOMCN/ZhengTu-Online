#pragma once

#include <assert.h>
#include <mmsystem.h>

struct stFootprint{
	POINT pt;
	DWORD time;
	DWORD color;
	bool isride;
	IBitmap* m_pTex;

	stFootprint()
	: m_pTex(NULL)
	, color(0xffffffff)
	{}
	
	~stFootprint()
	{
	}

	stFootprint & operator = (const stFootprint & v)
	{
		memcpy(this,&v,sizeof(stFootprint));
	}

	void SetAlpha(int alpha)
	{
		color = (color & 0xffffff) | (alpha << 24);
	}
	
	int GetAlpha()
	{
		return (color & 0xff000000) >> 24;
	}
	
	void SetImage(const char* szFileName,int group,int frame);
};

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CFootprintManager
{
private:
	std::list<stFootprint> m_footprints;
public:
	CFootprintManager(void);
	~CFootprintManager(void);
	void AddFootprint(const char* filename,int group,int frame,const POINT& pt, bool state)
	{
		m_footprints.push_back(stFootprint());
		m_footprints.back().SetImage(filename,group,frame);
		m_footprints.back().pt = pt;
		m_footprints.back().time = xtimeGetTime();
		m_footprints.back().isride = state;
	}

	void Run();
	void Draw(const POINT & ptMapOffset);
	void Clear(){ m_footprints.clear();	}
};

extern CFootprintManager& GetFootprintManager();