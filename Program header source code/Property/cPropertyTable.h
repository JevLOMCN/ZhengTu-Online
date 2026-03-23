#pragma once

#include "iProperty.h"
#include <sstream>

//#define DEFAULT_PACK "data\\magic.zip"

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class cPropertyTable : public iPropertyTable
{
public:
	cPropertyTable(PropertyTypeEnum e,int id);
	PropertyTypeEnum m_eType;
	int m_nID;
	void ClearProperty();
	virtual ~cPropertyTable(void);

	std::vector<iProperty*> m_aProperty;
	std::vector<iProperty*> m_aPropertyFormat;
	size_t GetPropertyCount(){return m_aProperty.size();};
	iProperty* GetProperty(LPCSTR szName)
	{
		int id = FindType(szName);
		if (id == -1)
			return NULL;
		return GetProperty(id);
	}
	// ªÒµ√‘≠ ºµƒ Ù–‘
	iProperty* GetRawProperty(int i)
	{
		if (i < 0 || (unsigned int)i >= m_aProperty.size())
			return NULL;
		return m_aProperty[i];
	}
	// ªÒµ√∏Ò Ωµƒ Ù–‘
	iProperty* GetProperty(int i)
	{
		if (i < 0 || (unsigned int)i >= m_aPropertyFormat.size())
			return NULL;
		if (m_aPropertyFormat[i] != NULL)
			return m_aPropertyFormat[i];
		return m_aProperty[i];
	}
	int FindType(LPCSTR szName);

	std::vector<stItem> m_aItem;
	std::vector<stItem>& GetItemVector(){return m_aItem;};
	stItem* GetFirstItemByName(const char* szName)
	{
		for(size_t i=0;i<m_aItem.size();++i)
		{
			if(m_aItem[i].strName == szName)
				return &m_aItem[i];
		}
		return NULL;
	}
	int ItemNameToFirstIndex(const char* szName)
	{
		for(size_t i=0;i<m_aItem.size();++i)
		{
			if(m_aItem[i].strName == szName)
				return i;
		}
		return -1;
	}
	void ClearItem(){m_aItem.clear();}
	void AddItem(stItem item){m_aItem.push_back(item);}
	std::vector<stGroup> m_aGroup;
	std::vector<stGroup>& GetGroupVector(){return m_aGroup;};
	void AddGroup(stGroup group){m_aGroup.push_back(group);}
	void ClearGroup(){m_aGroup.clear();}

	void AddProperty(iProperty* p);
	void AddProperty(void* buffer)
	{
		char* p = new char[GetPropertySize()];
		memcpy(p,buffer,GetPropertySize());
		AddProperty((iProperty*)p);
	}
	void AddProperty(const std::vector<std::string>& array)
	{
		iProperty* p = CreateProperty();
		String2Property(array,p);
		AddProperty(p);
	}

	void UpdateProperty(int i);
	void InsertProperty(int i, iProperty* pro);
	iProperty* GetFormat(iProperty* pro);
	void DeleteProperty(int i); 


	void SetValue(size_t iItem,const std::string& s,iProperty* p) ;
	void GetString(size_t iItem,const iProperty* p,std::string& s);

	void Property2String(const iProperty* pro, std::vector<std::string>& array);
	void String2Property(const std::vector<std::string>& array,iProperty* pro) ;

	static bool m_bIsEditor; //editor or user...

//help function
	void SetValue(size_t iItem,LPSTR strValue,const std::string& s);
	void SetValue(size_t iItem,std::string& strValue,const std::string& s);
	void GetString(size_t iItem,const std::string& strValue,std::string& s);
	void SetValue(size_t iItem, int& value, const std::string& s);
	void GetString(size_t iItem,const int& value,std::string& s);
	void SetValue(size_t iItem, long& value, const std::string& s);
	void GetString(size_t iItem,const long& value,std::string& s);
	void SetValue(size_t iItem, float& value, const std::string& s);
	void GetString(size_t iItem,const float& value,std::string& s);

	void SetValue(size_t iItem, DWORD& value,const std::string& s);
	void GetString(size_t iItem,const DWORD& value,std::string& s);
	void SetValue(size_t iItem, size_t& value,const std::string& s);
	void GetString(size_t iItem,const size_t& value,std::string& s);

	template <class T>
	void SetValue(size_t iItem, T* aValue, size_t& nValue, const std::string& s);
	template <class T>
	void GetString(size_t iItem,const T* value, size_t nValue, std::string& s);
	void SetColorValue(size_t iItem, DWORD* aValue, size_t& nValue, const std::string& s);

	size_t GetDataOffset(size_t iItem); //µ√µΩ∏√œÓµƒ∆´“∆µÿ÷∑
	size_t GetPropertySize();
	iProperty* CreateProperty() {
		int i=GetPropertySize();
		return (iProperty*)new char[i];
	};

	char m_strName[256];
	
	void Save();
	bool Load(LPCSTR szPack,LPCSTR szName,int id = -1);

#ifdef _GAME_TOOL
	bool Load2(LPCSTR szName,int id,bool bBin);
	void Save2(LPCSTR szName,bool bBin,bool bSaveHelp);

	bool LoadFromDatabase(LPCSTR szName,int id);
	bool SaveToDatabase(LPCSTR szName,bool bSaveHelp);
#endif
};

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param iItem : √Ë ˆ
 * \param aValue : √Ë ˆ
 * \param nValue : √Ë ˆ
 * \param s : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
template <class T>
void cPropertyTable::SetValue(size_t iItem, T* aValue, size_t& nValue, const std::string& s)
{
	std::vector<std::string> aRet;
	_parse_str(aRet,s.c_str(),'_');
	int index = 0;
	int prvIndex = -1;
	T prvValue = 0;
	for(size_t i=0;i<aRet.size();++i)
	{
		std::vector<std::string> astr;
		_parse_str(astr,aRet[i].c_str(),':');
		if(astr.empty()) continue;

		T value;
		if(astr.size() == 2)
		{
			index = atol(astr[0].c_str());
			if(index == -1) index = 31;
			SetValue(iItem,value,astr[1]);
		}
		else
		{
			SetValue(iItem,value,astr[0]);
		}
		aValue[index] = value;

		long dValue = value - prvValue;
		int dIndex = index-prvIndex;
		for(int k = prvIndex+1; k < index;++k)
		{
			aValue[k] = (k - prvIndex) * dValue / dIndex + prvValue;
		}
		prvIndex = index;
		prvValue = value;
		index ++;
	}
	Assert(index >= 0 && index <= 32);
	nValue = index;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param iItem : √Ë ˆ
 * \param aValue : √Ë ˆ
 * \param nValue : √Ë ˆ
 * \param s : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
template <class T>
void cPropertyTable::GetString(size_t iItem,const T* aValue, size_t nValue, std::string& s)
{
	std::ostringstream myString;
	std::string str;
	for (size_t i=0; i<nValue; i++)
	{
		GetString(iItem,*aValue++,str);
		myString << str;
		if (i != nValue - 1)
			myString << '_';
	}
	s = myString.str();
}

iPropertyCallBack* GetPropertyCallBack(PropertyTypeEnum e);
