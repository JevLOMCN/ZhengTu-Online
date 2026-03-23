#ifndef _USER_LOCAL_INFO_H
#define _USER_LOCAL_INFO_H

//#include "public.h"
#include "../xml_parse/tinyxml.h"
#include "../xml_parse/XmlParseEx.h"

typedef std::pair <int, std::string> int_string_Pair;
typedef std::pair <DWORD, DWORD> D_D_Pair;

class CCharacter;
typedef std::vector<DWORD> vRequestCharacter;
typedef std::pair <DWORD, vRequestCharacter> D_Character_Pair;
class CUserLocalInfo 
{
public:
	CUserLocalInfo();
	~CUserLocalInfo();

	int m_curZone;
	DWORD m_curVersion;
	//名字符串和服务器发来id的映射
	std::map<int,std::string> m_mapUnionName;
	std::map<int,std::string> m_mapSpetName;
	std::map<int,std::string> :: iterator m_pIter;

	//已经请求过的id;
	std::map<DWORD,vRequestCharacter> curRequesetUnionId;
	std::map<DWORD,vRequestCharacter> curRequesetSpetId;
	std::map<DWORD,vRequestCharacter> :: iterator m_pRequestIter;

	bool SaveUserLocalFile(const char* pszFileName);
	bool LoadUserLocalInfo(const char* pszFileName);

	int GetCurZone();

	DWORD GetCurVersion();
	void SetCurVersion(DWORD nCurVersion);
	
	void Clear();
protected:

private:

};
//extern CUserLocalInfo g_UserLocalInfo;
#endif