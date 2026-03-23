#pragma once

struct tSpellItem
{
	enum 
	{
		SPELL_LENGTH = 8,
	};
	char		pszSpell[SPELL_LENGTH];
	char*		pszHz;
	size_t		iSize;
};

class CSpell
{
public:
	typedef std::vector<tSpellItem>		tListSpell;
	typedef tListSpell::iterator		tListSpellIt;

public:
	CSpell(void);
	~CSpell(void);

	void LoadSpellMap();
	void UnLoadSpellMap();

	char GetSpell(WORD wChar);
    char GetFirstSpell(const char* ps);
private:
	const char* GetAllSpell(WORD wChar);
	char GetSpellI(WORD wChar);
	char GetSpellII(WORD wChar);
private:
	tListSpell	m_List;
};

inline CSpell& GetSpellManager()
{
	static CSpell s;return s;
}

bool Compare2StringBySpell(const char* psz1,const char* psz2);