#include "public.h"
#include ".\roleskill.h"
#include "./GameGuiManager.h"
#include "./GuiSkill.h"
#include "./GameScene.h"
#include "./MainCharacter.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CRoleSkill::CRoleSkill(void)
: m_pSkillBase(NULL)
, m_dwLevel(0)
, m_dwUpNum(0)
, m_dwExp(0)
, m_dwMaxExp(0)
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
CRoleSkill::~CRoleSkill(void)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pCmd : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CRoleSkill::SetAttrib(stAddUserSkillPropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	/*TRACE( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@Before set attribute@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n" );
	for(tTableSkillBase::iterator it = g_tableSkillBase.begin(); it != g_tableSkillBase.end(); ++it)
	{ 
		TRACE("skill:%u:%s,charTyp %u, five: %u, level: %u,\n",(*it).dwID,(*it).pstrName,(*it).dwSerialType,(*it).dwTreeType, (*it).dwLevel);
	}*/

	m_pSkillBase = ::GetSkillBase(pCmd->dwSkillID);
	Assert(m_pSkillBase);
	m_dwID = pCmd->dwSkillID;
	if(!m_pSkillBase) return false;
	
	//Stop follow event
	/*if ( GetScene()->GetMainCharacter() )
	{
		if ( GetScene()->GetMainCharacter()->IsFollowState() )
			GetScene()->GetMainCharacter()->ClearRunEvent();
	}	*/

	//Init use skill time = 0

	bool bLevelup = (m_dwLevel != pCmd->wdLevel);
	m_dwLevel = pCmd->wdLevel;

	m_dwExp = pCmd->dwExperience;
	m_dwMaxExp = pCmd->dwMaxExperience;
	m_dwUpNum = pCmd->wdUpNum;

	ReSetSkillData(m_pSkillBase,m_dwLevel);

	/*TRACE( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@After set attribute@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n" );
	for(tTableSkillBase::iterator it = g_tableSkillBase.begin(); it != g_tableSkillBase.end(); ++it)
	{ 
		TRACE("skill:%u:%s,charTyp %u, five: %u, level: %u,\n",(*it).dwID,(*it).pstrName,(*it).dwSerialType,(*it).dwTreeType, (*it).dwLevel);
	}*/

	if(GetGameGuiManager()->m_guiSkill)
		GetGameGuiManager()->m_guiSkill->RefreshState();

	if(bLevelup && GetScene()->GetMainCharacter())
	{
		GetScene()->GetMainCharacter()->ClearUseSkillTime(m_pSkillBase);
		GetScene()->GetMainCharacter()->UpdateAllSkillUpItemToolTips();
	}

	//Set skill disable state from server
	SkillLevelBase_t* pSkillLevel = GetSkillLevelBase( m_pSkillBase->dwID, m_dwLevel );
	if ( pSkillLevel && (pSkillLevel->dwIntervalTime > pCmd->dwExperience) )
	{
		DWORD startTime = 0;
		if ( xtimeGetTime() > (pSkillLevel->dwIntervalTime - pCmd->dwExperience) + 10 )
			startTime = xtimeGetTime() - (pSkillLevel->dwIntervalTime - pCmd->dwExperience) - 10;

		GetScene()->GetMainCharacter()->SetUseSkillTime( m_pSkillBase, startTime );
	}

	return true;

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
const char* CRoleSkill::GetName()
{
	FUNCTION_BEGIN;
 
	return m_pSkillBase->pstrName;

	FUNCTION_END;
}