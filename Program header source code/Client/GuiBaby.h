 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiBabyDlg :
	public CGuiDialog
{	
public:
	CGuiBabyDlg();
	~CGuiBabyDlg();
	
	int select;

	int babylevel;
	int babyexp;
	int babyskill1;
	int babyskill2;
	int babyskill3;
	int babyskill4;
	int babyskill5;
	int babyskillpoint;
	int num1;
	int num2;
	int num3;
	int num4;
	int num5;
	int ll;
	int zl;
	int js;
	int mj;
	int tz;
	bool putway;
	CGuiTable*	m_pTableMaterial1;
	CGuiStatic *cltext;
	void shuaxin(void);
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	CRoleItem* GetMaterialItem(); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
	void OnPutFocusItem(int index,CRoleItem* pRoleItem);
	CGuiTable* & GetEquipTable(int equip_type,int x = 0);
	//{
	//	int value = (equip_type << 16 | x);
	//	return m_tables[value];

	//}
	CRoleItem* GetEquipItem(int equip_type,int x = 0);
private:
	CGuiTable*			m_pTableMaterials[1];
	inline void SetBabyImage(int id,DWORD dwImage);
	CGuiMLTextBox* m_pListBoxList; // 列表
	CGuiProcess *pLevel;
	CGuiProcess *pLevel2; 
	CGuiImage *pImage;
	CGuiStatic *Snum1;
	CGuiStatic *Snum2;
	CGuiStatic *Snum3;
	CGuiStatic *Snum4;
	CGuiStatic *Snum5;

	CGuiStatic *Sll;
	CGuiStatic *Szl;
	CGuiStatic *Sjs;
	CGuiStatic *Smj;
	CGuiStatic *Stz;

	CGuiStatic *Sskillpoint;
	
	CGuiStatic *level;
	CGuiStatic *exps;

	
};
