#include "public.h"
#include ".\magic.h"
#include "GameScene.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CMagic::CMagic(void)
: m_pMagic(NULL)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CMagic::~CMagic(void)
{
	FUNCTION_BEGIN;

	Release();

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CMagic::Release()
{
	FUNCTION_BEGIN;

	if(m_pMagic)
	{
		m_pMagic->FinalRelease();
		m_pMagic->Release();
		m_pMagic = NULL;
	}
	CSceneObject::Release();

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pMagic : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CMagic::SetInterface(iMagic* pMagic)
{
	FUNCTION_BEGIN;

	Release();
	m_pMagic = pMagic;
	if(pMagic) pMagic->AddRef();
	UpdatePosition();

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CMagic::UpdatePosition()
{
	FUNCTION_BEGIN;

	if(m_pMagic)
	{
		m_ptPixelOffset = m_pMagic->GetPos();
		m_ptGrid = GetScene()->PixelPosToGridPos(m_ptPixelOffset);
	}

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param listMagic : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CMagic::Generate(tagListMagicGenerate & listMagic)
{
	FUNCTION_BEGIN;

	if(m_pMagic)
	{
		m_pMagic->Generate(listMagic);
	}

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param time_elasped : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CMagic::Run(long time_elasped)
{
	FUNCTION_BEGIN;

	if(m_pMagic) 
	{
		m_pMagic->Run(time_elasped);
		if(m_pMagic->IsDie()) 
			m_bDeleted = true;
	}

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
void CMagic::Draw(POINT ptMapPixelOffset,DWORD dwColor)
{
	FUNCTION_BEGIN;

	if(m_pMagic)m_pMagic->Draw(ptMapPixelOffset);

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CMagic::InScreen()
{
	FUNCTION_BEGIN;

	if(!m_pMagic) return false;
	return true;
	/*stRectI rc;
	if(!m_pMagic->GetDrawRect(m_ptPixelOffset,rc)) return false;
	rc &= GetScene()->GetScreenPixelRect();
	return !rc.IsRectEmpty();*/

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
POINT CMagic::GetSortPos()
{
	FUNCTION_BEGIN;

	if(m_pMagic)
		return m_pMagic->GetSortPos();
	return stPointI(0,0);

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptMapOffset : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CMagic::AddLight(POINT ptMapOffset)
{
	FUNCTION_BEGIN;

	if(m_pMagic)
	{
		m_pMagic->AddLight(ptMapOffset);
	}

	FUNCTION_END;
}