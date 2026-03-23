#include "../engine/include/engine.h"
#include ".\ctablemanager.h"
#include "cpropertytable.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
cTableManager::cTableManager(void)
{
	InitProperty();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
cTableManager::~cTableManager(void)
{
	DestroyProperty();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
int cTableManager::InitProperty()
{
	return 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param b : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cTableManager::SetEditor(bool b)
{
	cPropertyTable::m_bIsEditor = b;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
int cTableManager::DestroyProperty()
{
	for (int i=0; i<Property_Num; i++)
	{
		for (TableMap::iterator it = m_aTable[i].map.begin(); it != m_aTable[i].map.end(); ++it)
		{
			cPropertyTable* p = (*it).second;
			delete p;
		}
		m_aTable[i].map.clear();
	}
	return 0;
}

#define PROPERTY_ZIPFILE	"data\\datas.pak"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param e : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
iPropertyTable* cTableManager::CreatePropertyTable(PropertyTypeEnum e,int id)
{
	cPropertyTable* p = new cPropertyTable(e,id);
	if (p->Load(PROPERTY_ZIPFILE,m_aTable[e].name.c_str()))
	{
		m_aTable[e].map[id] = p;
		return p;
	}
	else if (m_aTable[e].idDefault != -1)
	{
		if (p->Load(PROPERTY_ZIPFILE,m_aTable[e].name.c_str(),m_aTable[e].idDefault))
		{
			m_aTable[e].map[id] = p;
			return p;
		}
	}
	delete p;
	return NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param e : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
iPropertyTable* cTableManager::GetiPropertyTable(PropertyTypeEnum e,int id)
{
	if (e < 0 || e >= Property_Num)
		return NULL;
	iPropertyTable* p = m_aTable[e].map[id];
	if (p == NULL)
		p = CreatePropertyTable(e,id);
	return p;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param e : ÃèÊö
 * \param id : ÃèÊö
 * \param szName : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
iProperty* cTableManager::GetProperty(PropertyTypeEnum e,int id,LPCSTR szName)
{
	iPropertyTable* pTable = GetiPropertyTable(e,id);
	if (pTable == NULL)
		return NULL;
	iProperty* p = pTable->GetProperty(szName);
	return p;		
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param e : ÃèÊö
 * \param id : ÃèÊö
 * \param idMagic : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
iProperty* cTableManager::GetProperty(PropertyTypeEnum e,int id,int idMagic)
{
	if(id == 0 && e == Property_Magic)
	{
		int iiiiiiii=0;
	}
	iPropertyTable* pTable = GetiPropertyTable(e,id);
	if (pTable == NULL)
		return NULL;
	iProperty* p = pTable->GetProperty(idMagic);
	return p;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param idMagic : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
MagicProperty* cTableManager::GetMagicProperty(int idMagic,int id)
{
	return (MagicProperty*) GetProperty(Property_Magic,id,idMagic);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szGui : ÃèÊö
 * \param idTable : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
GuiProperty* cTableManager::GetGuiProperty(LPCSTR szGui,int idTable )
{
	return (GuiProperty*) GetProperty(Property_Gui,idTable,szGui);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param idGui : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
GuiProperty* cTableManager::GetGuiProperty(int idGui,int id)
{
	return (GuiProperty*) GetProperty(Property_Gui,id,idGui);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param idNpc : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
NpcProperty* cTableManager::GetNpcProperty(int idNpc,int id)
{
	return (NpcProperty*) GetProperty(Property_Npc,id,idNpc);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param idCharacter : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CharacterProperty* cTableManager::GetCharacterProperty(int idCharacter,int id)
{
	return (CharacterProperty*) GetProperty(Property_Character,id,idCharacter);
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
EXPORT_API iTableManager* Property_GetTableManager()
{
	static cTableManager s_manager;
	return &s_manager;
}
