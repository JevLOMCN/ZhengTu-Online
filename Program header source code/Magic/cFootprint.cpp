#include "../engine/include/engine.h"
#include ".\cFootprint.h"
#include "public.h"
/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
CFootprint::CFootprint(void)
{
	stResourceLocation rl;
	rl.SetFileName("data\\footprint.gl");
	rl.group = 6;
	rl.frame = 0;
	m_image = GetDevice()->FindBitmaps(&rl);
	AssertFatal(m_image,avar("ÎÄ¼þ%s [%d][%d] error or not exist",rl.szFileName,rl.group,rl.frame));

	if(m_image) m_image->SetAniSpeed(80);

}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
CFootprint::~CFootprint(void)
{
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param ptMapPixelOffset : ÃèÊö
* \param dwColor : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void CFootprint::Draw(POINT ptMapPixelOffset)
{

	if(m_image)
	{
		if ( (xtimeGetTime()-m_startTime)/(m_image->GetAniInfo()->wdAniSpeed) < m_image->GetFrameCount() )
		{
			pt.x -= GetWidth()/2;
			pt.y -= GetHeight()/2;
			m_image->RenderAni(pt.x-ptMapPixelOffset.x, pt.y-ptMapPixelOffset.y,xtimeGetTime()-m_startTime, -1);
		}		
	}

}


void CFootprint::Settime(DWORD stime)
{
	m_startTime = stime;
}

void CFootprint::Setpos(stPointF pos)
{
	pt = pos;
}

void CFootprint::Setresourse(const char* filename, int group, int frame)
{
	stResourceLocation rl;
	rl.SetFileName(filename);
	rl.group = group;
	rl.frame = frame;
	m_image = GetDevice()->FindBitmaps(&rl);
	AssertFatal(m_image,avar("ÎÄ¼þ%s [%d][%d] error or not exist",rl.szFileName,rl.group,rl.frame));
}
CFootprint& GetCFootprint()
{
	static CFootprint fp;
	return fp;
}