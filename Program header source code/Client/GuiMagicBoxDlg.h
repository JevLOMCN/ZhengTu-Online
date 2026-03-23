 #pragma once 

class CGuiMagicBoxDlg : public CGuiDialog
{	
private:
	CAnimation m_AniBody[6];
	CGuiImage* pImage[6];
	

public:

	CGuiMagicBoxDlg();
	~CGuiMagicBoxDlg();

	int type;

	//Ê±×°
	struct one_shizhuang
	{
		char name[MAX_NAMESIZE];
		DWORD bodyNum;
		DWORD level;
		DWORD state;
	};
	struct one_shizhuang Shizhuang[300]; 
	//Åû·ç
	struct one_pifeng
	{
		char name[MAX_NAMESIZE];
		DWORD itemNum;
		DWORD level;
		DWORD state;
	};
	struct one_pifeng Pifeng[100]; 
	//³á°ò
	struct one_chibang
	{
		char name[MAX_NAMESIZE];
		DWORD itemNum;
		DWORD level;
		DWORD state;
	};
	
	struct one_chibang Chibang[100]; 
	//×øÆï
	struct one_zuoqi
	{
		char name[MAX_NAMESIZE];
		DWORD npcNum;
		DWORD level;
		DWORD state;
	};
	struct one_zuoqi Zuoqi[100]; 
	DWORD mohelevel;
	DWORD moheexp;
	DWORD mohemaxexp;
	DWORD shizhuang_select;
	DWORD pifeng_select;
	DWORD chibang_select;
	DWORD zuoqi_select;
	int shizhuangshu;
	int pifengshu;
	int chibangshu;
	int zuoqishu;
	DWORD num1;
	DWORD num2;
	DWORD num3;
	DWORD num4;
	DWORD num5;
	int page;
	void shuaxin(void);
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);
};
