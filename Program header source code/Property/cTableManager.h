#pragma once

#include "iProperty.h"

#include <hash_map>
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class cPropertyTable;
typedef stdext::hash_map<int, cPropertyTable*> TableMap;

struct stTable
{
	TableMap map;
	std::string name;
	int idDefault;
	iPropertyCallBack* pCallBack;
	stTable()
	{
		pCallBack = NULL;
		idDefault = -1;
	}
};

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class cTableManager : public iTableManager
{
public:
	cTableManager(void);
	virtual ~cTableManager(void);

	stTable m_aTable[Property_Num];

	void SetEditor(bool b);
	int InitProperty();
	int DestroyProperty();
	iPropertyTable* CreatePropertyTable(PropertyTypeEnum e,int id);
	iPropertyTable* GetiPropertyTable(PropertyTypeEnum e,int id );
	iProperty* GetProperty(PropertyTypeEnum e,int id,int idMagic);
	iProperty* GetProperty(PropertyTypeEnum e,int id,LPCSTR szName);

	MagicProperty* GetMagicProperty(int idMagic,int id );
	CharacterProperty* GetCharacterProperty(int idCharacter,int id);
	NpcProperty* GetNpcProperty(int idNpc,int id );
	GuiProperty* GetGuiProperty(int idGui,int id );
	GuiProperty* GetGuiProperty(LPCSTR szGui,int idTable ) ;

	bool CreateProperty(PropertyTypeEnum e,iPropertyCallBack* p, LPCSTR szName, int defaultid)
	{
		m_aTable[e].pCallBack = p;
		m_aTable[e].name = szName;
		m_aTable[e].idDefault = defaultid;
		return true;
	};
	iPropertyCallBack* GetCallBack(PropertyTypeEnum e){return m_aTable[e].pCallBack;};
};

