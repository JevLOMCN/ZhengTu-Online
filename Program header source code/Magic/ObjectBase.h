#pragma once

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CObjectBase
{ 
	char	m_szName[33];
protected:
public:
	bool	m_bDeleted;
	DWORD	m_dwID;
	virtual bool CanRemove(){ return m_bDeleted;}
public:
	CObjectBase(void);
	virtual ~CObjectBase(void);
	virtual void Run(long time_elasped){}
	virtual void Draw(POINT ptMapPixelOffset,DWORD dwColor){}
	virtual bool IsDie() { return false; }

#ifndef _GAME_TOOL
	virtual void Release(){m_dwID = -1;};
#endif

	const char* GetName() const { return m_szName; }

	void		SetName(const char* pszName)
	{
		strncpy(m_szName,pszName,sizeof(m_szName));
		m_szName[sizeof(m_szName)-1] = 0;
	}

	DWORD		GetID()  const { return m_dwID;   };
	
	virtual bool IsVisible() { return true; }
	virtual bool IsEnabled() { return true; }
};
