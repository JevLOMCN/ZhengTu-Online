 
#pragma once 
 #include "..\gui\include\guidialog.h"
class CGuijingmaiDlg :
	public CGuiDialog
{	
public:
	CGuijingmaiDlg();
	~CGuijingmaiDlg();
	int select;
	int jm1;
	int jm2;
	int jm3;
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void update(int num);//ok  和.cpp名字对应即可 既然加了一个参数.h也要对应
	void SHUAXIN();//ok  和.cpp名字对应即可 既然加了一个参数.h也要对应
	CGuiTable*	m_pTableMaterial1; //table  Table类型 代表 格子 就是那个材料的格子
	CGuiStatic *cltext; //两个文字
	CGuiStatic *moneytext;  //文字
	 
private:
CRoleItem* GetMaterialItem();
CGuiImage *jingmaibg;
CGuiImage *jingmairenwu;
CGuiButton *jingm1;
CGuiButton *jingm2;
CGuiButton *jingm3;

//小圆点开始
CGuiImage *dian1;
CGuiImage *dian2;
CGuiImage *dian3;
CGuiImage *dian4;
CGuiImage *dian5;
CGuiImage *dian6;
CGuiImage *dian7;
CGuiImage *dian8;
CGuiImage *dian9;
CGuiImage *dian10;
CGuiImage *dian11;
CGuiImage *quanbj2;
CGuiStatic *wenzi;
CGuiTable*			m_pTableMaterials[1];
};
