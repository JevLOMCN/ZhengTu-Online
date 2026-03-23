#include "../../engine/include/engine.h"
#include "../include/accelkey.h"

//#include <boost/regex.hpp>
//#include "../../greta/regexpr2.h"
//using namespace regex;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszAccelKey : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CAccelKey::CAccelKey(const char* pszAccelKey)
{
	SetAccelKey(pszAccelKey);
	m_group = 0;
}

struct stKeyString{
	char szString[16];
	BYTE key; 
};

static stKeyString s_keyString[] ={
	{"Ctrl",VK_CONTROL},
	{"Shift",VK_SHIFT},
	{"Esc",VK_ESCAPE},
	{"Enter",VK_RETURN},
	{"Home",VK_HOME},
	{"End",VK_END},
	{"PageUp",VK_PRIOR},
	{"PageDown",VK_NEXT},
	{"Del",VK_DELETE},
	{"Alt",VK_MENU},
	{"Tab",VK_TAB},
	{"F1",VK_F1},
	{"F2",VK_F2},
	{"F3",VK_F3},
	{"F4",VK_F4},
	{"F5",VK_F5},
	{"F6",VK_F6},
	{"F7",VK_F7},
	{"F8",VK_F8},
	{"F9",VK_F9},
	{"F10",VK_F10},
	{"F11",VK_F11},
	{"F12",VK_F12},
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszAccelKey : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CAccelKey::SetAccelKey(const char* pszAccelKey)
{
	m_aKey.resize(0);

	//using namespace boost;

	//rpattern pat("\\s*([^\\+]+)\\s*\\+{0,1}");
	//split_results aresult;
	std::vector<std::string> aresult;
	_parse_str(aresult,pszAccelKey,'+');
	//regex_split(std::back_inserter(aresult),std::string(pszAccelKey),expression);
	//pat.split(pszAccelKey,aresult);
	
	for(size_t i=0 ; i<aresult.size() ; ++i)
	{
		char szTmp[256];
		strncpy(szTmp,aresult[i].c_str(),256);
		szTmp[255]=0;
		str_trim_left(szTmp);
		str_trim_right(szTmp);
		aresult[i] = szTmp;
		if(aresult[i].empty()) continue;

		bool bFind = false;
		for(size_t j=0;j<count_of(s_keyString);++j)
		{
			if(stricmp(s_keyString[j].szString,aresult[i].c_str()) == 0)
			{
				m_aKey.push_back(s_keyString[j].key);
				bFind = true;
				break;
			}
		}
		if(!bFind)
		{
			m_aKey.push_back(aresult[i].c_str()[0]);
		}
	}
	return m_aKey.size() > 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CAccelKey::IsKeyDown()
{
	if(m_aKey.empty()) return false;
	for(size_t i=0; i < m_aKey.size(); ++i)
	{
		if(!(GetKeyState(m_aKey[i]) < 0))
			return false;
	}
	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CAccelKey::IsNumKey()
{
	if ( m_aKey.empty() ) return false;
	for( size_t i=0;i<m_aKey.size(); ++i )
	{
		if ( (m_aKey[i]>='0') && (m_aKey[i]<='9') )
			return true;
	}

	return false;
}


bool CAccelKey::IsAltDown()
{
	if ( m_aKey.empty() ) return false;
	for( size_t i=0;i<m_aKey.size(); ++i )
	{
		if(GetKeyState(VK_MENU) < 0) 
		    return true;
	}

	return false;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
std::string CAccelKey::GetAccelKey()
{
	std::string str;
	for(size_t i=0; i < m_aKey.size(); ++i)
	{
		bool bFind = false;
		if(!str.empty()) str += "+";
		for(size_t j = 0; j < count_of(s_keyString); ++j)
		{
			if(m_aKey[i] == s_keyString[j].key)
			{
				str += s_keyString[j].szString;
				bFind = true;
				break;
			}
		}
		if(!bFind)
		{
			char szTemp[2];
			szTemp[0] = m_aKey[i];
			szTemp[1] = 0;
			str += szTemp;
		}
	}
	return str;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pKey : ÃèÊö
 * \param bufSize : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
size_t CAccelKey::GetKey(BYTE* pKey,size_t bufSize)
{
	int c = getMin(bufSize,m_aKey.size());
	int i = 0;
	for(;i<c;++i)
	{
		pKey[i] = m_aKey[i];
	}
	return i;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pKey : ÃèÊö
 * \param bufSize : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CAccelKey::IsKey(BYTE* pKey,size_t bufSize)
{
	size_t countKey = 0;
	for(countKey = 0; countKey < bufSize;++countKey)
	{
		if(pKey[countKey] == 0) break;
	}
	if(countKey != m_aKey.size()) return false;

	for(size_t i=0 ; i < countKey;++i)
	{
		size_t j=0;
		for(j = 0; j< m_aKey.size();++j)
		{
			if(m_aKey[j] == pKey[i])
				break;
		}
		if( j == m_aKey.size()) return false;
	}
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param group : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CAccelKey::IsSameGroup( BYTE group )
{
	return ( group == m_group );
}