 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuigongxunDlg :
	public CGuiDialog
{	
public:
	CGuigongxunDlg();
	~CGuigongxunDlg();
	CGuiTable*	m_pTableMaterial1;
	void shuaxin(const char* t1,const char* t2);
	void OnClose(void);
	void OnCreate(void);
	void update(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	int num;
	//sky 功勋竞猜
		struct jingcai_pro
		{
			int index;
			int object_id;
			int object_level;
			int object_num;
		};
	    jingcai_pro jingcai[28];
private:
	CRoleItem* GetMaterialItem(); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
	CGuiStatic * text1;
	CGuiStatic * text2;
	CGuiTable*			m_pTableMaterials[1];
};
