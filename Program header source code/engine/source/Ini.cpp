#include "../include/engine.h"

#include "../include/ini.h"
#include "../include/stringex.h"
#include <strstream>

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszFName : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CIni::Open(const char* pszFName)
{
	FileStream file;
	if(!file.open(pszFName,FileStream::Read))
		return false;
	return Open(&file);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pStream : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CIni::Open(Stream* pStream)
{
	long len = pStream->getSize();
	char* psz = new char[len+1];
	pStream->read(psz,len);
	psz[len] = 0;
	bool b = Read(psz);
	delete [] psz;
	return b;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszPack : ÃèÊö
 * \param pszFName : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CIni:: Open(const char* pszPack,const char* pszFName)
{
	Stream* pStream = OpenPackFileForRead(pszPack,pszFName);
	if(!pStream) return false;
	bool b = Open(pStream);
	ClosePackFileForRead(pStream);
	return b;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszIn : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CIni::Read(const char* pszIn)
{
	m_vecGroups.clear();
	m_mapGroups.clear();
	m_bIsOpen = FALSE;

	std::vector<char> sz;
	std::vector<char> szTmp;
	std::vector<char> szName;
	std::vector<char> szValue;

	bool bV = FALSE;

	const char* p1 = pszIn;
	const char* p2;

	int iGroup = -1;
	int iKey = -1;
	bool bQuotation=FALSE;
	while(1)
	{
		p2 = p1;
		
		while(*p2 != 0 && *p2 != '\r' && *p2 !='\n') ++p2;
		sz.resize((int)p2-(int)p1 + 1);

		memcpy(&sz[0],p1,(int)p2-(int)p1);

		sz[(int)p2-(int)p1] = 0;

		szTmp.resize(sz.size());
		strcpy(&szTmp[0] , &sz[0]);

		str_trim_right(&sz[0]);
		str_trim_left(&sz[0]);

		if(sz[0] == 0) goto _w_end;
		if(strstr(&sz[0],"//") == &sz[0]) goto _w_end;
		if(sz[0] =='[')
		{
			if(iGroup >=0 && iKey >= 0)
			{
				std::string & str = m_vecGroups[iGroup].vecKeys[iKey].strValue;
				if(str.length() > 0 && str[0] == '\"')
				{
					str.erase(0,1);
				}
				if(str.length() > 0)
				{
					if(str[str.length()-1] == '\"')
					{
						str.erase(str.length()-1,1);
					}
				}
				iKey = -1;
				iGroup = -1;
				bQuotation = FALSE;
			}

			int len =(int)strlen(&sz[0]);
			if(len>=2)
			{
				if(sz[len-1] == ']') sz[len -1 ] = 0;
				if(!m_bCaseSens) strlwr(&sz[0]);
				iGroup = -1;
				for(size_t i = 0;i<m_vecGroups.size();++i)
				{
					if(m_vecGroups[i].strGroupName == &sz[1])
					{
						iGroup = i;
						break;
					}
				}
				if(iGroup == -1)
				{
					iGroup = m_vecGroups.size();
					m_vecGroups.push_back(stGroup());
					m_vecGroups[iGroup].strGroupName  = &sz[1];
					m_mapGroups[&sz[1]] = iGroup;
				}
				iKey = -1;
			}
		}
		else if(iGroup>-1)
		{
			char* p = strstr(&sz[0],"=");
			if(p && !bQuotation)
			{
				bQuotation = FALSE;
				szName.resize(p - &sz[0] + 1);
				memcpy(&szName[0],&sz[0],p - &sz[0]);
				szName[p - &sz[0]] = 0;
				str_trim_left(&szName[0]);
				str_trim_right(&szName[0]);
				szValue.resize(strlen(p+1)+1);
				strcpy(&szValue[0],p+1);
				str_trim_left(&szValue[0]);
				str_trim_right(&szValue[0]);

				if(iGroup >=0 && iKey >= 0)
				{
					std::string & str = m_vecGroups[iGroup].vecKeys[iKey].strValue;
					if(str.length() > 0 && str[0] == '\"')
					{
						str.erase(0,1);
					}
					if(str.length() > 0)
					{
						if(str[str.length()-1] == '\"')
						{
							str.erase(str.length()-1,1);
						}
					}
					iKey = -1;
				}
				if(!m_bCaseSens) strlwr(&szName[0]);
				iKey = m_vecGroups[iGroup].vecKeys.size();
				m_vecGroups[iGroup].mapKey[&szName[0]] = iKey;
				m_vecGroups[iGroup].vecKeys.push_back(stKey());
				
				m_vecGroups[iGroup].vecKeys[iKey].strName = &szName[0];
				m_vecGroups[iGroup].vecKeys[iKey].strValue= &szValue[0];
				if(szValue[0] == '\"') bQuotation = TRUE;
				int len = strlen(&szValue[0]);
				if(len > 0 && szValue[len-1] =='\"') bQuotation = FALSE;
			}
			else
			{
				if(iGroup >=0 && iKey >= 0) 
				{
					m_vecGroups[iGroup].vecKeys[iKey].strValue += "\n";
					m_vecGroups[iGroup].vecKeys[iKey].strValue += &szTmp[0];
					int len = strlen(&szTmp[0]);

					if(len>0 && szTmp[len-1] == '\"')
					{
						std::string & str = m_vecGroups[iGroup].vecKeys[iKey].strValue;
						if(str.length() > 0 && str[0] == '\"')
						{
							str.erase(0,1);
						}
						if(str.length() > 0)
						{
							if(str[str.length()-1] == '\"')
							{
								str.erase(str.length()-1,1);
							}
						}
						iKey = -1;

						bQuotation = FALSE;
					}
				}

			}
		}
_w_end:
		if(*p2 == 0)
			break;
		p1 = p2+1;
		if(*p1 == 0) 
			break;
		if(*p1 == '\n')
		{
			++p1;
		}
	}
	if(iGroup >=0 && iKey >= 0) 
	{
		std::string & str = m_vecGroups[iGroup].vecKeys[iKey].strValue;
		if(str.length() > 0 && str[0] == '\"')
		{
			str.erase(0,1);
		}
		if(str.length() > 0)
		{
			if(str[str.length()-1] == '\"')
			{
				str.erase(str.length()-1,1);
			}
		}
	}
	m_bIsOpen = TRUE;
	return TRUE;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nID : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CIni::ReadFromResource(UINT nID)
{
	HINSTANCE hModule = GetModuleHandle(NULL);
	HRSRC hRC = FindResource(hModule,MAKEINTRESOURCE(nID),"DATA");
	if(hRC == NULL) return FALSE;
	HGLOBAL hGlobal = LoadResource(hModule,hRC);
	if(hGlobal == NULL) return FALSE;
	BYTE* lpBuf = (BYTE*)LockResource(hGlobal);
	if(lpBuf == NULL) return FALSE;
	DWORD dwSize = SizeofResource(hModule,hRC);

	char * pszText = new char [dwSize+1];
	memcpy(pszText,lpBuf,dwSize);
	UnlockResource(hGlobal);
	FreeResource(hGlobal);

	pszText[dwSize] = 0;
	bool b = Read(pszText);
	delete [] pszText;
	return b;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszFName : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CIni::Save(const char* pszFName)
{
	std::ofstream f(pszFName);
	if(!f.is_open()) return FALSE;
	
	size_t i,j;

	for(i = 0;i<m_vecGroups.size(); ++i)
	{
		f<<'['<<m_vecGroups[i].strGroupName<<"]\n";
		for(j = 0;j<m_vecGroups[i].vecKeys.size(); ++j)
		{
			f<<m_vecGroups[i].vecKeys[j].strName<<'='<<m_vecGroups[i].vecKeys[j].strValue<<'\n';
		}
	}
	m_bIsOpen = TRUE;
	return TRUE;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszGroup : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CIni::FindGroup(const char* pszGroup)
{
	stdext::hash_map<std::string,unsigned int>::iterator it;
	std::string str = pszGroup;
	if(!m_bCaseSens)strlwr(&str[0]);
	it = m_mapGroups.find(str.c_str());
	if(it == m_mapGroups.end()) return -1;
	return (it->second);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iGroup : ÃèÊö
 * \param szKeyName : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CIni::FindKey(int iGroup,const char* szKeyName)
{
	stdext::hash_map<std::string,unsigned int>::iterator it;
	std::string str = szKeyName;
	if(!m_bCaseSens)strlwr(&str[0]);
	it = m_vecGroups[iGroup].mapKey.find(str);
	if(it == m_vecGroups[iGroup].mapKey.end())
		return -1;
	return it->second;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszGroup : ÃèÊö
 * \param pszKey : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
const char* CIni::FindKey(const char* pszGroup,const char* pszKey)
{
	stdext::hash_map<std::string,unsigned int>::iterator it;

	std::string str = pszGroup;
	if(!m_bCaseSens)strlwr(&str[0]);

	it = m_mapGroups.find(str.c_str());
	if(it == m_mapGroups.end()) return NULL;

	stdext::hash_map<std::string,unsigned int>::iterator it1;

	str = pszKey;
	if(!m_bCaseSens)strlwr(&str[0]);
	
	it1 = m_vecGroups[it->second].mapKey.find(str);
	if(it1 == m_vecGroups[it->second].mapKey.end()) return NULL;
	return m_vecGroups[it->second].vecKeys[it1->second].strValue.c_str();
	
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszGroup : ÃèÊö
 * \param pszKey : ÃèÊö
 * \param iDefault : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CIni::GetInt(const  char* pszGroup, const  char* pszKey, int iDefault)
{
	const char* strVal;
	if(!(strVal = FindKey(pszGroup,pszKey))) return iDefault;
	return atol(strVal);
}

bool CIni::GetBool(const  char* pszGroup, const  char* pszKey, bool iDefault)
{
	return GetInt(pszGroup,pszKey,iDefault) == 1;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszDstBuffer : ÃèÊö
 * \param nBufferSize : ÃèÊö
 * \param pszGroup : ÃèÊö
 * \param pszKey : ÃèÊö
 * \param pszDefault : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CIni::GetString(char* pszDstBuffer,unsigned int nBufferSize,const char* pszGroup, const char* pszKey, const char* pszDefault)
{
	const char* strVal;
	if(!(strVal = FindKey(pszGroup,pszKey))) 
	{
		strncpy(pszDstBuffer,pszDefault,nBufferSize);
		pszDstBuffer[nBufferSize-1] = 0;
	}
	else
	{
		strncpy(pszDstBuffer,strVal,nBufferSize);
		pszDstBuffer[nBufferSize-1] = 0;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszGroup : ÃèÊö
 * \param pszKey : ÃèÊö
 * \param pszDefault : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
std::string CIni::GetString(const char* pszGroup,const char* pszKey,const char* pszDefault)
{
	const char* strVal;
	if(strVal = FindKey(pszGroup,pszKey)) 
		return std::string(strVal);
	return std::string(pszDefault);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszGroup : ÃèÊö
 * \param pszKey : ÃèÊö
 * \param fDefault : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
float CIni::GetFloat(const  char* pszGroup, const  char* pszKey, float fDefault)
{
	const char* strVal;
	if(!(strVal = FindKey(pszGroup,pszKey))) return fDefault;
	return (float)atof(strVal);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszGroup : ÃèÊö
 * \param pszKey : ÃèÊö
 * \param pszValue : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CIni::SetString(const char* pszGroup, const char* pszKey, const char* pszValue)
{
	char szTemp[256];
	strncpy(szTemp,pszGroup,256);
	szTemp[255] = 0;
	if(!m_bCaseSens)strlwr(szTemp);

	int iGroup = FindGroup(pszGroup);
	stGroup groupTmp;
	stKey keyTmp;
	if(iGroup == -1)
	{
		iGroup = m_vecGroups.size();
		m_vecGroups.push_back(groupTmp);
		m_vecGroups[iGroup].strGroupName = pszGroup;
		m_mapGroups[pszGroup] = iGroup;
	}

	int iKey = FindKey(iGroup,pszKey);

	if(iKey == -1)
	{
		iKey = m_vecGroups[iGroup].vecKeys.size();
		m_vecGroups[iGroup].vecKeys.push_back(keyTmp);
		m_vecGroups[iGroup].vecKeys[iKey].strName = pszKey;
		m_vecGroups[iGroup].mapKey[pszKey] = iKey;
	}
	m_vecGroups[iGroup].vecKeys[iKey].strValue = pszValue;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszGroup : ÃèÊö
 * \param pszKey : ÃèÊö
 * \param iValue : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CIni::SetInt(const char* pszGroup, const char* pszKey, int iValue)
{
	char szValue[64];
	
	_snprintf(szValue,sizeof(szValue),"%d",iValue);
	szValue[sizeof(szValue)-1] = 0;

	SetString(pszGroup,pszKey,szValue);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszGroup : ÃèÊö
 * \param pszKey : ÃèÊö
 * \param fValue : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CIni::SetFloat(const char* pszGroup, const char* pszKey, float fValue)
{
	char szValue[64];
	_snprintf(szValue,sizeof(szValue),"%g",fValue);
	szValue[sizeof(szValue)-1] = 0;
	SetString(pszGroup,pszKey,szValue);
}