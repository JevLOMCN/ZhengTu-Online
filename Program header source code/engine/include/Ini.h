#ifndef _CLAS_INI_H
#define _CLAS_INI_H
#pragma warning( push )
#pragma warning( disable : 4996 )

#include <fstream>
#include <string>
#include <hash_map>

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CIni
{
private:
	struct stKey
	{
		std::string strName;
		std::string strValue;
	};
	struct stGroup
	{
		std::string strGroupName;
		stdext::hash_map<std::string,unsigned int> mapKey;
		std::vector<stKey> vecKeys;
	};
	std::vector<stGroup> m_vecGroups;
	stdext::hash_map<std::string,unsigned int> m_mapGroups;

private:
	bool	m_bIsOpen;
	bool	m_bCaseSens;	// «¯∑÷¥Û–°–¥
	const char* CIni::FindKey(const char* pszGroup,const char* pszKey);
public:
	CIni(bool bCaseSens = FALSE){m_bIsOpen = FALSE;m_bCaseSens = bCaseSens;}
	CIni(const char* pszFName,bool bCaseSens = FALSE){m_bCaseSens = bCaseSens;Open(pszFName);}
	bool IsOpen(){return m_bIsOpen;}
	bool Open(Stream* pStream);
	bool Open(const char* pszFName);
	bool Open(const char* pszPack,const char* pszFName);
	bool Read(const char* pszIn);
	bool ReadFromResource(UINT nID);
	bool GetBool(const  char* pszGroup, const  char* pszKey, bool iDefault = false);
	int GetInt(const  char* pszGroup, const  char* pszKey, int iDefault = 0);
	float GetFloat(const  char* pszGroup, const  char* pszKey, float fDefault = 0.0f);
	void GetString(char* pszDstBuffer,unsigned int nBufferSize,const char* pszGroup, const char* pszKey, const char* pszDefault);
	std::string GetString(const char* pszGroup,const char* pszKey,const char* pszDefault);
	void SetString(const char* pszGroup, const char* pszKey, const char* pszValue);
	void SetInt(const char* pszGroup, const char* pszKey, int iValue);
	void SetFloat(const char* pszGroup, const char* pszKey, float fValue);
	bool Save(const char* pszFName);

	int GetGroupCount(){ return (int)m_vecGroups.size();}
	int GetKeyCount(const char* szGroup){ int i = FindGroup(szGroup);
		if(i==-1) return 0;
		return m_vecGroups[i].vecKeys.size();
	}
	int FindGroup(const char* pszGroup);
	int FindKey(int iGroup,const char* szKeyName);
	
	std::string GetGroupName(int i)
	{
		return m_vecGroups[i].strGroupName;
	}
	std::string GetKeyName(int iGroup,int iKey)
	{
		return m_vecGroups[iGroup].vecKeys[iKey].strName;
	}
	std::string GetKeyValue(int iGroup,int iKey)
	{
		return m_vecGroups[iGroup].vecKeys[iKey].strValue;
	}
};

#pragma warning( pop )
#endif