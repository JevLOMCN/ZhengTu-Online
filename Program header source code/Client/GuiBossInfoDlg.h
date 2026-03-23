 
#pragma once 

enum
{
	Puange_Info1 = 1,    //第一页
	Puange_Info2 = 2,    //第二页	
	Puange_Info3 = 3,    //第三页	
	Puange_InfoMax,    //最大页数
};

class CGuiBossInfoDlg :
	public CGuiDialog
{	
public:
	CGuiBossInfoDlg();
	~CGuiBossInfoDlg();

	struct NpcBossInfo
	{
		char MapName[MAX_NAMESIZE+1];  //BOSS出现地图名称
		WORD  TypeID;  //列表类别ID
		WORD  Pagenum;  //BOSS所属类别页
		WORD  Number;   //类别列表索引编号
		DWORD NpcID;    // BOSS的ID
		DWORD CountryID;   // BOSS所属国家ID
		WORD NpcState; // BOSS的状态 1:死亡 0:存活
        uint64_t ReviveTime;  // 复活时间
	};
	struct NpcBossInfo m_NpcBossInfo[64];

    int NaviGationBar; //导航栏目编号
	int Puange; //页数
	int GotoOK; //选择参与的项目
	int MaxPuange;



	void OnClose(void);
	void OnCreate(void);
	void UpData();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	CGuiImage *BgImgae[3];      // Boss显示背景框
	CGuiImage *IconImage[3];    // BOSS头像框
	CGuiImage *IconFace[3];     // BOSS头像
	CGuiStatic *StrName[3];     // BOSS名称
	CGuiStatic *StrMapName[3];  // 出现地点
	CGuiStatic *BossState[3];   // BOSS存活状态
	CGuiStatic *BossTexts[3];   // BOSS存活状态

private:
	HRESULT OnRender(float fElapsedTime);

	CGuiTab*  m_pTab;
};
