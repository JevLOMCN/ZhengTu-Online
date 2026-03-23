#pragma once
#include "..\gui\include\guidialog.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CGuiEquip :
	public CGuiDialog
{
	char m_szCharName[MAX_NAMESIZE];
public:
	CGuiEquip(void);
	void OnClose(void);
	
	CGuiImage* m_pConnImg;
	std::map<int,CGuiTable*> m_tables;
	CAnimation m_aniBody[3];

	//void SetCharType(int type);

	void OnCreate();

	CGuiTable* & GetEquipTable(int equip_type,int x = 0)
	{
		int value = (equip_type << 16 | x);
		return m_tables[value];
	}

	HRESULT OnRender(float fElapsedTime);

	void SetConnVisible(bool b);
	const char* GetCharName(){ return m_szCharName;}

	void SetCharName(const char* pszName)
	{
		strncpy(m_szCharName,pszName,sizeof(m_szCharName)); 
		m_szCharName[sizeof(m_szCharName)-1]=0;
	}

	void SetEquipData(void* pData);

	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	//void BodyRender(CGuiTable* pTable,stPointI ptRender);
};
