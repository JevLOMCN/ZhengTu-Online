#pragma	 once

#include "public.h"
#include <string>
#include <vector>

#define string_null(a) strcpy(a,"null")
const int MAX_PROPERTY_CHAR = 128;

struct stGroup
{
	char strGroupName[64];
	size_t nStart;
};
struct stItem
{
	char strName[64];
	char strValue[64];
	int size;
	std::vector<std::string> aStrHelp;
	bool IsFloat()const {return strcmp(strValue , "float") == 0;}
	bool IsInt()const {return strcmp(strValue , "int") == 0;}
	bool IsDword()const {return strcmp(strValue , "dword") == 0;}
	bool IsColor()const {return strcmp(strValue , "color") == 0;}
	bool IsEnum()const {return strcmp(strValue , "enum") == 0;}
	bool IsName()const {return strcmp(strValue , "name") == 0;}
	bool IsString()const {return strcmp(strValue , "string") == 0;}
	bool IsArrayDword()const {return strcmp(strValue , "arraydword") == 0;}
	bool IsArrayColor()const {return strcmp(strValue , "arraycolor") == 0;}
	bool IsArrayInt()const {return strcmp(strValue , "arrayint") == 0;}
	bool IsArray()const{return IsArrayDword() || IsArrayInt();}
	bool IsLongString()const{ return strcmp(strValue,"longstring") == 0; }

	void CalculateSize()
	{
		if (IsName())
			size = 16;
		else if (IsString())
			size = 16;
		else if (IsArray())
			size = 132;
		else //is enum or is int
			size = 4;
	}
	std::string GetDefaultValue()
	{
		std::string strValue = "null";
		if(IsColor())
		{
			strValue = "FFFFFFFF";
		}
		else if(IsEnum())
		{
			Assert(aStrHelp.size());
			strValue = aStrHelp[0];
		}
		else if(IsString() || IsName())
		{
			strValue = "null";
		}
		else if(IsLongString())
		{
			strValue = "";
		}
		else if(IsFloat())
		{
			strValue = "0.0";
		}
		else
		{
			strValue = "0";
		}
		return strValue;
	}
	std::string GetHelpString()
	{
		std::string str;
		for(size_t i=0;i<aStrHelp.size();++i)
		{
			if(i) str += ";";
			str+=aStrHelp[i];
		}
		return str;
	}
};

enum PropertyTypeEnum
{
	/*
	Property_Magic_Begin,
	Property_Magic_Process,
	Property_Magic_End,
	//*/
	Property_Magic,
	Property_Character,
	Property_Npc,
	Property_Gui,
	Property_Num,
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class iProperty
{
public:
	enum
	{
		MAX_NAME = 16,
	};
	char strName[MAX_NAME];
	iProperty(){string_null(strName);}
	LPCSTR GetName(){return strName;}
};

struct iPropertyTable
{
public:
	//编辑器使用
	//virtual std::vector<iProperty*>& GetPropertyVector()=0;
	//help
	virtual std::vector<stItem>& GetItemVector()= 0;
	virtual std::vector<stGroup>& GetGroupVector()= 0;
	virtual void Property2String(const iProperty* ppro, std::vector<std::string>& array) = 0;
	virtual void String2Property(const std::vector<std::string>& array,iProperty* pro) = 0;

	virtual size_t GetPropertyCount() = 0; //得到个数
	virtual void ClearProperty() = 0;
	virtual void AddProperty(void* pro) = 0;
	virtual void AddProperty(iProperty* pro) = 0;
	virtual void AddProperty(const std::vector<std::string>& array) = 0;
	virtual void UpdateProperty(int i) = 0;
	virtual void InsertProperty(int i, iProperty* pro) = 0; //插入
	virtual void DeleteProperty(int i) = 0; 
	virtual size_t GetPropertySize() = 0;	//得到每个的存储大小
	virtual iProperty* GetRawProperty(int n) = 0;

	//for editor
	virtual void Save() = 0;
	//for file op
	virtual void ClearGroup() = 0;
	virtual void AddGroup(stGroup group) = 0;
	virtual void ClearItem() = 0;
	virtual void AddItem(stItem item) = 0;
	//其他模块使用
	virtual iProperty* GetProperty(int i) = 0;
	virtual iProperty* GetProperty(LPCSTR szName) = 0;
	virtual int FindType(LPCSTR szName) = 0;
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class iPropertyCallBack
{
public:
	virtual iProperty* FormatProperty(const iProperty* pRaw) = 0;
};

//EXPORT_API void Property_SetCallBack(PropertyTypeEnum e,iPropertyCallBack* p);

class MagicProperty;
class NpcProperty;
class CharacterProperty;
class GuiProperty;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class iTableManager
{
public:
	virtual void SetEditor(bool b) = 0; //如果是编辑器的话，自动存档。
	/*
	e		指定类型， 
	p		callback用于格式化数据， 
	szname  类型的名称，例如“magic”同时property会自动的去"propery\\magic"这个目录下面搜索help.ini（索引）, nnnn.ini（数据）
	default 默认id，如果一个id没有数据文件，那么使用这个默认id的数据
	*/
	virtual bool CreateProperty(PropertyTypeEnum e, iPropertyCallBack* p, LPCSTR szName,int defaultid = -1) = 0;

	virtual iPropertyTable* GetiPropertyTable(PropertyTypeEnum e,int id = -1) = 0;
	
	virtual MagicProperty* GetMagicProperty(int idMagic,int idTable = 0) = 0;
	virtual NpcProperty* GetNpcProperty(int idMagic,int idTable = 0) = 0;
	virtual CharacterProperty* GetCharacterProperty(int idMagic,int idTable = 0) = 0;
	virtual GuiProperty* GetGuiProperty(int idMagic,int idTable = 0) = 0;
	virtual GuiProperty* GetGuiProperty(LPCSTR szGui,int idTable = 0) = 0;

	virtual iPropertyCallBack* GetCallBack(PropertyTypeEnum e) = 0;
	virtual int DestroyProperty() = 0;
};

EXPORT_API iTableManager* Property_GetTableManager();


