#pragma once

#include <string>
#include "iProperty.h"

#define cast_int(a) (*(int*)(&a))

#ifdef _GAME_TOOL

class CDaoDatabase;
//#import "C:\Program Files\Common Files\System\ado\msado20.tlb" no_namespace rename("EOF","adoeof")
//#import "C:\Program Files\Common Files\System\ado\msadox.dll" rename("EOF","adoxeof")
#endif

class Stream;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class cFileOP
{
public:
	cFileOP(iPropertyTable*);
	virtual ~cFileOP(void);

	enum FileEnum
	{
		FILE_NULL,
		FILE_HELP,FILE_DATA,
	};

	int  m_nID;
	char m_strFileBin[256];
	char m_strFileIni[256];
	char m_strFilePack[256];

#ifdef _GAME_TOOL
	char m_strDatabase[256];
#endif

	FileEnum m_eFile;
	FileEnum GetFile(){return m_eFile;}

	struct stData
	{
		FileEnum eFile;
		char strBin[256];
		char strIni[256];
		char strPack[256];

#ifdef _GAME_TOOL
		char strDatabase[256];
#endif

	};

	void SetData(stData& data)
	{
#ifdef _GAME_TOOL
		SetData(data.eFile,data.strBin,data.strIni,data.strPack,data.strDatabase);
#else
		SetData(data.eFile,data.strBin,data.strIni,data.strPack);
#endif
	}

#ifdef _GAME_TOOL
	void SetData(FileEnum e,LPCSTR strBin,LPCSTR strIni,LPCSTR strPack,LPCSTR strDatabase)
#else
	void SetData(FileEnum e,LPCSTR strBin,LPCSTR strIni,LPCSTR strPack)
#endif
	{
		m_eFile = e;
		strcpy(m_strFileBin , strBin);
		strcpy(m_strFileIni , strIni);
		strcpy(m_strFilePack , strPack);
#ifdef _GAME_TOOL
		strcpy(m_strDatabase,strDatabase);
#endif
	}
	
	// 游戏中使用的函数
	bool Load();
	bool Save();


#ifdef _GAME_TOOL
	// 工具使用的函数
	bool LoadFromDatabase();
	bool SaveToDatabase();

	// 格式转换工具使用的函数
	bool Load2(bool bBin);
	bool Save2(bool bBin);

	bool InitTable(CDaoDatabase& db,const char * pszTable);
#endif

protected:
	bool LoadBinFromZip();
	bool LoadBinFromStream(Stream* file);
	bool LoadBin();
	bool LoadIni();

	bool SaveBin(bool bBuildPath = true);
	bool SaveIni(bool bBuildPath = true);

	iPropertyTable* m_pTable;
	iPropertyTable* GetTable(){return m_pTable;};
public:
	static bool GetTableSet(std::vector<int> & aTableID,const char* pszDatabase);
};

