#pragma once
#include "..\magic\objectbase.h"

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CRoleSkill :
	public CObjectBase
{
	SkillBase_t * m_pSkillBase;
	DWORD		  m_dwLevel;
	DWORD		  m_dwExp;
	DWORD		  m_dwMaxExp;
	DWORD		  m_dwUpNum;
public:
	CRoleSkill(void);
	~CRoleSkill(void);
	bool SetAttrib(stAddUserSkillPropertyUserCmd* pCmd);
	const char* GetName();

	DWORD GetTotalLevel() { return m_dwLevel+m_dwUpNum; }
	DWORD GetLevel(){ return m_dwLevel;}
	DWORD GetUpNum() { return m_dwUpNum; }
	DWORD GetExp(){ return m_dwExp;}
	DWORD GetMaxExp(){return m_dwMaxExp;}
	SkillBase_t* GetSkillBase() { return m_pSkillBase;}
};
