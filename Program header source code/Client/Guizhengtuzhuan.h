 
#pragma once 
 #include "..\gui\include\guidialog.h"
class CGuizhengtuzhuanDlg :
	public CGuiDialog
{	
public: //public 公有的 public：后面的内容 是 在这整个源码 任何一个.cpp文件 都可以进行调用的 内容
	CGuizhengtuzhuanDlg();
	~CGuizhengtuzhuanDlg();

	void OnClose(void);
	void OnCreate(void);
	void update(int num);//ok  和.cpp名字对应即可 既然加了一个参数.h也要对应
	void SHUAXIN();//ok  和.cpp名字对应即可 既然加了一个参数.h也要对应
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	CGuiTable*	m_pTableMaterial1; //table  Table类型 代表 格子 就是那个材料的格子
	CGuiStatic *cltext; //两个文字
	CGuiStatic *moneytext;  //文字
	int select;//创建一个整数型的变量 叫 select
	int z1;
	int z2;
	int z3;
	int z4;
	int z5;
	int z6;
	int z7;
	int z8;
	int z9;
	int z10;
	//声明 10个 int类型的变量 用来存储 10个征途传的等级
private: //private 代表私有的  private: 后面的内容 只有在 Guizhengtuzhuan.cpp内 可以访问和调用 但在别的文件 就不可以了
CRoleItem* GetMaterialItem(); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
CGuiButton *zhuan1;
CGuiButton *zhuan2;
CGuiButton *zhuan3;
CGuiButton *zhuan4;
CGuiButton *zhuan5;
CGuiButton *zhuan6;
CGuiButton *zhuan7;
CGuiButton *zhuan8;
CGuiButton *zhuan9;
CGuiButton *zhuan10;

//现在你可能不理解 我会慢慢讲 在这里我们创建了 10个用来存放button类型按钮的对象
CGuiStatic *wenzi;
CGuiTable*			m_pTableMaterials[1];
};
