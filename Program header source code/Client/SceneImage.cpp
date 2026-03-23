#include "public.h"
#include ".\sceneimage.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CSceneImage::CSceneImage(void)
: m_image(NULL)
{
	FUNCTION_BEGIN;

	m_dwSceneObjectType = OBJECT_IMAGE;
	stResourceLocation rl;
	rl.SetFileName("data\\other.gl");
	rl.group = 3;
	rl.frame = 0;
	m_image = GetDevice()->FindBitmaps(&rl);
	AssertFatal(m_image,avar("ÎÄ¼þ%s [%d][%d] error or not exist",rl.szFileName,rl.group,rl.frame));

	if(m_image) m_image->SetAniSpeed(100);

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CSceneImage::~CSceneImage(void)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
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
void CSceneImage::Draw(POINT ptMapPixelOffset,DWORD dwColor)
{
	FUNCTION_BEGIN;

	if(m_image)
	{
		if ( (xtimeGetTime()-m_startTime)/(m_image->GetAniInfo()->wdAniSpeed) < m_image->GetFrameCount() )
		{
			stPointI pt = m_ptPixelOffset - ptMapPixelOffset;
			pt.x -= GetWidth()/2;
			pt.y -= GetHeight()/2;
			m_image->RenderAni(pt.x,pt.y,xtimeGetTime()-m_startTime,dwColor);
		}		
	}

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
POINT CSceneImage::GetSortPos()
{
	FUNCTION_BEGIN;

	return stPointI(m_ptPixelOffset.x,m_ptPixelOffset.y - GRID_HEIGHT/2 + 1);

	FUNCTION_END;
}

void CSceneImage::Play()
{
	m_startTime = xtimeGetTime();
}