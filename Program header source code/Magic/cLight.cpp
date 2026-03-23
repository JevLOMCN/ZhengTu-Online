#include "../engine/include/engine.h"
#include "clight.h"

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
 * \return ·µ»ØÖµµÄÃèÊö
 */
cLight::cLight(void)
{
	m_ulCount = 1;
	m_bEnable = false;
	m_eType = Light_Point;
	m_eEffect = Light_Effect_Null;
	m_nRadius = 0.f;
	m_crColor = 0;
	m_ptPos.x = 0;
	m_ptPos.y = 0;
	m_lData = 0;
	m_bEnable = true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
cLight::~cLight(void)
{
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param time_eslasped : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int cLight::Run(long time_eslasped)
{
	return false;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptMapOffset : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cLight::Draw(POINT ptMapOffset)
{
	if (!IsEnable())
		return false;
	POINT ptOffset = (POINT)GetPos() - ptMapOffset;
	return DrawAtScreen(ptOffset);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptOffset : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cLight::DrawAtScreen(POINT ptOffset)
{
	if (!IsEnable())
		return false;
	switch(GetType()) {
	case Light_Envirment:
		GetLightMap()->AddEnvirmentLight(GetColor());
		break;
	case Light_Point:
		GetLightMap()->AddLight(ptOffset,GetRadius(),GetColor());
		break;
	default:
		;
	}
	return true;
}

//static std::lookaside_allocator<cLight> s_allocLight;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
#ifdef _DEBUG
EXPORT_API iLight* Light_CreateLight(const char* fileName,int line)
{
	//return s_allocLight.alloc(fileName,line);
	return new cLight;
}
#else
EXPORT_API iLight* Light_CreateLight()
{
	//return s_allocLight.alloc();
	return new cLight;
}
#endif
void Light_DestoryLight(iLight * p)
{
	delete p;
	//s_allocLight.freePointer((cLight*)p);
}


