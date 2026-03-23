#include "../engine/include/engine.h"
#include ".\footprintmanager.h"
#include "public.h"
#include ".\cFootprint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szFileName : ÃèÊö
 * \param group : ÃèÊö
 * \param frame : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void stFootprint::SetImage(const char* szFileName,int group,int frame)
{
	IBitmapList* p = ::GetDevice()->FindResource(szFileName,c_nMagicResourceFlags,Usage_Npc);
	assert(p);
	if(!p) return;

	m_pTex = p->LoadFrame(group,frame);
	assert(m_pTex);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CFootprintManager::CFootprintManager(void)
{

}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CFootprintManager::~CFootprintManager(void)
{
}
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CFootprintManager::Run()
{
	DWORD time = xtimeGetTime();
	const int da = 80;
	
	for(std::list<stFootprint>::iterator it = m_footprints.begin(); it != m_footprints.end();)
	{
		int alpha = 255 - (time-(*it).time) * da / 1000;
		if(alpha <= 0)
		{
			it = m_footprints.erase(it);
		}
		else
		{
			(*it).SetAlpha(alpha);
			++it;
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptMapOffset : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CFootprintManager::Draw(const POINT & ptMapOffset)
{
	Run();
	//const POINT ptCenter = {-238,-250};
	for(std::list<stFootprint>::iterator it = m_footprints.begin(); it != m_footprints.end();++it)
	{
		if((*it).m_pTex)
		{
			stPointF pt((*it).pt.x - (*it).m_pTex->GetWidth()/2 - ptMapOffset.x,(*it).pt.y - (*it).m_pTex->GetHeight()/2 - ptMapOffset.y-20);
			//stPointF pt((*it).pt.x - ptMapOffset.x,(*it).pt.y - ptMapOffset.y);
			(*it).m_pTex->Render(pt.x,pt.y,(*it).color);
			
			if((*it).isride)//Ìí¼ÓÆïÂíµÄ»Ò³¾
			{
				GetCFootprint().Settime((*it).time);
				GetCFootprint().Setpos((*it).pt);
				GetCFootprint().Draw(ptMapOffset);
			}
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CFootprintManager& GetFootprintManager()
{
	static CFootprintManager fm;
	return fm;
}