#include "../engine/include/engine.h"
#include "cpropertytable.h"
#include "cfileop.h"
#include <D3dx8math.h>

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szBuffer : ÃèÊö
 * \param str : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int StringToID(const std::vector<std::string>& szBuffer,std::string str)
{
	for (int i=0; i<(int)szBuffer.size() ;i++)
	{
		if (str == szBuffer[i])
			return i;
	}
	return -1;
}

bool cPropertyTable::m_bIsEditor = false;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param e : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
cPropertyTable::cPropertyTable(PropertyTypeEnum e,int id)
{
	m_strName[0] = 0;
	m_eType = e;
	m_nID = id;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
cPropertyTable::~cPropertyTable(void)
{
	if (cPropertyTable::m_bIsEditor)
		Save();
	ClearProperty();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::ClearProperty()
{
	for (size_t i = 0; i < m_aProperty.size(); i++)
	{
		delete m_aProperty[i];
	}
	for (size_t i = 0; i < m_aPropertyFormat.size(); i++)
	{
		delete m_aPropertyFormat[i];
	}
	m_aProperty.clear();
	m_aPropertyFormat.clear();
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szName : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int cPropertyTable::FindType(LPCSTR szName)
{
	for (size_t i=0; i<m_aProperty.size(); i++)
	{
		if (strcmp(m_aProperty[i]->GetName(),szName) == 0)
			return (int)i;
	}
	return -1;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pro : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
iProperty* cPropertyTable::GetFormat(iProperty* pro)
{
	if (GetPropertyCallBack(m_eType))
		return GetPropertyCallBack(m_eType)->FormatProperty(pro);
	return NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param i : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::UpdateProperty(int i)
{
	delete m_aPropertyFormat[i];
	m_aPropertyFormat[i] = GetFormat(m_aProperty[i]);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param i : ÃèÊö
 * \param pro : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::InsertProperty(int i, iProperty* pro)
{
	m_aProperty.insert(m_aProperty.begin()+i,pro);
	m_aPropertyFormat.insert(m_aPropertyFormat.begin()+i,GetFormat(pro));	
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param i : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::DeleteProperty(int i)
{
	if (i >= 0 && i < m_aProperty.size())
	{
		iProperty* p = m_aProperty[i];
		m_aProperty.erase(m_aProperty.begin()+i);
		delete p;
		iProperty* pf = m_aPropertyFormat[i];
		m_aPropertyFormat.erase(m_aPropertyFormat.begin()+i);
		delete pf;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param p : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::AddProperty(iProperty* p)
{
	m_aProperty.push_back(p);
	m_aPropertyFormat.push_back(GetFormat(p));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
size_t cPropertyTable::GetPropertySize()
{
	return GetDataOffset(m_aItem.size());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
size_t cPropertyTable::GetDataOffset(size_t iItem)
{
	size_t size = 0;
	for (int i=0; i<iItem; i++)
		size += m_aItem[i].size;
	return size;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param p : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//////////////////////////////////////////////////////////////////////////
void cPropertyTable::GetString(size_t iItem,const iProperty* p,std::string& s)
{
	size_t off = GetDataOffset(iItem);
	char* pBuffer = (char*)p;
	pBuffer += off;
	if (m_aItem[iItem].IsString())
		GetString(iItem,pBuffer,s);
	else if (m_aItem[iItem].IsName())
		GetString(iItem,pBuffer,s);
	else if (m_aItem[iItem].IsInt())
		GetString(iItem,*((int*)pBuffer),s);
	else if (m_aItem[iItem].IsEnum())
		GetString(iItem,*((int*)pBuffer),s);
	else if (m_aItem[iItem].IsFloat())
		GetString(iItem,*((float*)pBuffer),s);
	else if(m_aItem[iItem].IsDword() || m_aItem[iItem].IsColor())
		GetString(iItem,*((DWORD*)pBuffer),s);
	else if (m_aItem[iItem].IsArrayDword() || m_aItem[iItem].IsArrayColor())
	{
		DWORD* p = (DWORD*)pBuffer;
		//ÕâÑù×ÓÓĞÒş»¼¾ÍÊÇÁË
		GetString(iItem,p+1,*p,s);
	}
	else if (m_aItem[iItem].IsArrayInt())
	{
		int* p = (int*)pBuffer;
		//ÕâÑù×ÓÓĞÒş»¼¾ÍÊÇÁË
		GetString(iItem,p+1,*p,s);
	}
	else
		Assert(0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param s : ÃèÊö
 * \param p : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::SetValue(size_t iItem,const std::string& s,iProperty* p)
{
	size_t off = GetDataOffset(iItem);
	char* pBuffer = (char*)p;
	pBuffer += off;
	stItem& item = m_aItem[iItem];
	if (item.IsString())
		SetValue(iItem,pBuffer,s);
	else if (item.IsName())
		SetValue(iItem,pBuffer,s);
	else if (item.IsInt())
		SetValue(iItem,*((int*)pBuffer),s);
	else if (item.IsEnum())
		SetValue(iItem,*((int*)pBuffer),s);
	else if (item.IsFloat())
		SetValue(iItem,*((float*)pBuffer),s);
	else if(item.IsDword() || item.IsColor())
		SetValue(iItem,*((DWORD*)pBuffer),s);
	else if (item.IsArrayDword())
	{
		DWORD* p = (DWORD*)pBuffer;
		//ÕâÑù×ÓÓĞÒş»¼¾ÍÊÇÁË
		size_t size;
		SetValue(iItem,p+1,size,s);
		*p = size;
	}
	else if(item.IsArrayColor())
	{
		DWORD* p = (DWORD*)pBuffer;
		//ÕâÑù×ÓÓĞÒş»¼¾ÍÊÇÁË
		size_t size;
		SetColorValue(iItem,p+1,size,s);
		*p = size;
	}
	else if (item.IsArrayInt())
	{
		int* p = (int*)pBuffer;
		//ÕâÑù×ÓÓĞÒş»¼¾ÍÊÇÁË
		size_t size;
		SetValue(iItem,p+1,size,s);
		*p = size;
	}
	else
		Assert(0);
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param p : ÃèÊö
 * \param array : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//////////////////////////////////////////////////////////////////////////
//help function...
void cPropertyTable::Property2String(const iProperty* p, std::vector<std::string>& array)
{
	array.clear();
	std::string s;
	for (size_t i=0;i<m_aItem.size();i++)
	{
		GetString(i,p,s);
		if (s.empty())
			s = "null";
		array.push_back(s);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param array : ÃèÊö
 * \param p : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::String2Property(const std::vector<std::string>& array,iProperty* p)
{
	
	for (size_t i=0;i<m_aItem.size();i++)
	{
		SetValue(i,array[i],p);
	}
	//set value then change format...
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param strValue : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::SetValue(size_t iItem,LPSTR strValue,const std::string& s)
{
	strcpy(strValue,s.c_str());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param strValue : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::SetValue(size_t iItem,std::string& strValue,const std::string& s)
{
	strValue = s;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param strValue : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::GetString(size_t iItem,const std::string& strValue,std::string& s)
{
	s = strValue;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param value : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::SetValue(size_t iItem, float& value,const std::string& s)
{
	value = atof(s.c_str());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param value : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::GetString(size_t iItem,const float& value,std::string& s)
{
	char str[256];
	sprintf(str,"%.4f",value);
	s = str;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param value : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::SetValue(size_t iItem, int& value,const std::string& s)
{
	if (m_aItem[iItem].IsEnum())
	{
		value = StringToID(m_aItem[iItem].aStrHelp,s);
		return;
	}
	char sz[256];
	strcpy(sz,s.c_str());
	strlwr(sz);
	char* p = sz;
	if(strstr(p,"0x") == p)
	{
		p+=2;
		value = strtol(p,NULL,16);
	}
	else
		value = atoi(s.c_str());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param value : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::GetString(size_t iItem,const int& value,std::string& s)
{
	if (m_aItem[iItem].IsEnum())
	{
		if (value < 0 || value >= m_aItem[iItem].aStrHelp.size())
			s = m_aItem[iItem].aStrHelp[0];
		else
			s = m_aItem[iItem].aStrHelp[value];
		return;
	}
	char str[256];
	sprintf(str,"%d",value);
	s = str;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param value : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::SetValue(size_t iItem, long& value,const std::string& s)
{
	char sz[256];
	strcpy(sz,s.c_str());
	strlwr(sz);
	char* p = sz;
	if(strstr(p,"0x") == p)
	{
		p+=2;
		value = strtol(p,NULL,16);
	}
	else
		value = atoi(s.c_str());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param value : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::GetString(size_t iItem,const long& value,std::string& s)
{
	char str[256];
	sprintf(str,"%d",value);
	s = str;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param value : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::SetValue(size_t iItem, size_t& value,const std::string& s)
{
	char sz[256];
	strcpy(sz,s.c_str());
	strlwr(sz);
	char* p = sz;
	if(strstr(p,"0x") == p)
	{
		p+=2;
		value = strtol(p,NULL,16);
	}
	else
		value = atoi(s.c_str());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param value : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::GetString(size_t iItem,const size_t& value,std::string& s)
{
	char str[256];
	sprintf(str,"%d",value);
	s = str;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param value : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::SetValue(size_t iItem, DWORD& value,const std::string& s)
{
	value = strtoul(s.c_str(),NULL,16);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param value : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::GetString(size_t iItem,const DWORD& value,std::string& s)
{
	char str[256];
	sprintf(str,"%x",value);
	s = str;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param data : ÃèÊö
 * \param szPack : ÃèÊö
 * \param szName : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//load to pro 
#define PROPERTY_FOLDER		"property"
#define PROPERTY_BIN_FOLDER "datas"
void SetData(cFileOP::stData& data,LPCSTR szPack,LPCSTR szName,int id)
{
	sprintf(data.strIni,"%s\\%s\\%04d.ini",PROPERTY_FOLDER,szName,id);
	sprintf(data.strBin,"%s\\%s\\%04d.pro",PROPERTY_BIN_FOLDER,szName,id);
	strcpy(data.strPack , szPack);

#ifdef _GAME_TOOL
	sprintf(data.strDatabase,"%s\\%s.mdb",PROPERTY_FOLDER,szName);
#endif

	data.eFile = cFileOP::FILE_DATA;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dataHelp : ÃèÊö
 * \param szPack : ÃèÊö
 * \param szName : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void SetHelp(cFileOP::stData& dataHelp,LPCSTR szPack,LPCSTR szName)
{
	sprintf(dataHelp.strIni,"%s\\%s\\help.ini",PROPERTY_FOLDER,szName);
	sprintf(dataHelp.strBin,"%s\\%s\\help.pro",PROPERTY_BIN_FOLDER,szName);
	strcpy(dataHelp.strPack , szPack);
#ifdef _GAME_TOOL
	sprintf(dataHelp.strDatabase,"%s\\%s.mdb",PROPERTY_FOLDER,szName);
#endif
	dataHelp.eFile = cFileOP::FILE_HELP;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param data : ÃèÊö
 * \param szName : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void SetData2(cFileOP::stData& data,LPCSTR szName,int id)
{
	sprintf(data.strIni,"%s\\%04d.ini",szName,id);
	sprintf(data.strBin,"%s\\%04d.pro",szName,id);
	strcpy(data.strPack , "");

#ifdef _GAME_TOOL
	sprintf(data.strDatabase,"%s.mdb",szName);
#endif

	data.eFile = cFileOP::FILE_DATA;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dataHelp : ÃèÊö
 * \param szName : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void SetHelp2(cFileOP::stData& dataHelp,LPCSTR szName)
{
	sprintf(dataHelp.strIni,"%s\\help.ini",szName);
	sprintf(dataHelp.strBin,"%s\\help.pro",szName);
	sprintf(dataHelp.strPack , "");

#ifdef _GAME_TOOL
	sprintf(dataHelp.strDatabase,"%s.mdb",szName);
#endif

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
	dataHelp.eFile = cFileOP::FILE_HELP;
}
void cPropertyTable::Save()
{
	if (m_strName[0]==0)
		return;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szName : ÃèÊö
 * \param bBin : ÃèÊö
 * \param bSaveHelp : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
	cFileOP::stData data;
	SetData(data,"",m_strName,m_nID);
	cFileOP opHelp(this);
	opHelp.SetData(data);
	opHelp.Save();
}
#ifdef _GAME_TOOL
void cPropertyTable::Save2(LPCSTR szName,bool bBin,bool bSaveHelp)
{

	cFileOP::stData data,dataHelp;
	SetData2(data,szName,m_nID);
	SetHelp2(dataHelp,szName);

	cFileOP op(this);
	op.SetData(data);
	op.Save2(bBin);

	cFileOP opHelp(this);
	opHelp.SetData(dataHelp);
	if(bSaveHelp)
		opHelp.Save2(bBin);
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szName : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cPropertyTable::LoadFromDatabase(LPCSTR szName,int id)
{
	strcpy(m_strName , szName);

	m_nID = id;
	cFileOP::stData data,dataHelp;
	SetData2(data,szName,id);
	SetHelp2(dataHelp,szName);

	cFileOP op(this);
	op.SetData(data);
	cFileOP opHelp(this);
	opHelp.SetData(dataHelp);
	if (opHelp.LoadFromDatabase())
	{
		if (op.LoadFromDatabase())
			return true;
	}
	m_aGroup.clear();
	m_aItem.clear();
	ClearProperty();
	return false;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szName : ÃèÊö
 * \param bSaveHelp : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cPropertyTable::SaveToDatabase(LPCSTR szName,bool bSaveHelp)
{
	cFileOP::stData data,dataHelp;

	SetData2(data,szName,m_nID);
	SetHelp2(dataHelp,szName);

	cFileOP op(this);
	op.SetData(data);
	bool b1 = op.SaveToDatabase();

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szName : ÃèÊö
 * \param id : ÃèÊö
 * \param bBin : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
	cFileOP opHelp(this);
	opHelp.SetData(dataHelp);
	bool b2 = true;
	if(bSaveHelp)
		b2 = opHelp.SaveToDatabase();
	return b1 && b2;
}
bool cPropertyTable::Load2(LPCSTR szName,int id,bool bBin)
{
	strcpy(m_strName , szName);
	m_nID = id;
	cFileOP::stData data,dataHelp;
	SetData2(data,szName,id);
	SetHelp2(dataHelp,szName);

	cFileOP op(this);
	op.SetData(data);
	cFileOP opHelp(this);
	opHelp.SetData(dataHelp);
	if (opHelp.Load2(bBin))
	{
		if (op.Load2(bBin))
			return true;
	}
	m_aGroup.clear();
	m_aItem.clear();
	ClearProperty();
	return false;
}
#endif

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szPack : ÃèÊö
 * \param szName : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cPropertyTable::Load(LPCSTR szPack,LPCSTR szName,int id)
{
	strcpy(m_strName , szName);
	Assert(m_nID != -1);
	if (id == -1)
		id = m_nID;

	cFileOP::stData data,dataHelp;
	SetData(data,szPack,szName,id);
	SetHelp(dataHelp,szPack,szName);

	cFileOP op(this);
	op.SetData(data);
	cFileOP opHelp(this);
	opHelp.SetData(dataHelp);
	if (opHelp.Load())
	{
		if (op.Load())
			return true;
	}
	m_aGroup.clear();
	m_aItem.clear();
	ClearProperty();
	return false;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param e : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
iPropertyCallBack* GetPropertyCallBack(PropertyTypeEnum e)
{
	return Property_GetTableManager()->GetCallBack(e);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iItem : ÃèÊö
 * \param aValue : ÃèÊö
 * \param nValue : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cPropertyTable::SetColorValue(size_t iItem, DWORD* aValue, size_t& nValue, const std::string& s)
{
	std::vector<std::string> aRet;
	_parse_str(aRet,s.c_str(),'_');
	int index = 0;
	int prvIndex = -1;
	D3DXCOLOR prvValue = (DWORD)0;

	for(size_t i=0;i<aRet.size();++i)
	{
		std::vector<std::string> astr;
		_parse_str(astr,aRet[i].c_str(),':');
		if(astr.empty()) continue;

		D3DXCOLOR value;
		if(astr.size() == 2)
		{
			index = atol(astr[0].c_str());
			if(index == -1) index = 31;
			DWORD v;
			SetValue(iItem,v,astr[1]);
			value = v;
		}
		else
		{
			DWORD v;
			SetValue(iItem,v,astr[0]);
			value = v;
		}
		aValue[index] = value;
		
		int dIndex = index-prvIndex;
		for(int k = prvIndex+1; k < index;++k)
		{
			//aValue[k] =  dValue * (float)(k - prvIndex) / (float)dIndex + prvValue;
			D3DXCOLOR curValue;
			D3DXColorLerp(&curValue,&prvValue,&value, (float)(k - prvIndex) / (float)dIndex );
			aValue[k] = curValue;
		}
		prvIndex = index;
		prvValue = value;
		index ++;
	}
	Assert(index >= 0 && index <= 32);
	nValue = index;
}