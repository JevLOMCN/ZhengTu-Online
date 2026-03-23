#pragma once

#define  IDB_MG_START			14	//开始按钮
#define  IDB_MG_STOP			13	//停止
#define  IDB_MG_CHATSEND		15	//小游戏聊天
#define  IDB_MG_CHATEMOI		16	//聊天图片表情

#define  IDE_CHATINPUT			18	//聊天输入
#define  IDML_CHATOUTPUT		17	//聊天输出

#define  IDS_L_FIRST			21	//左边第一个文本框..第一次扔出的数字
#define  IDS_L_SECOND			20	//第二个
#define  IDS_L_THIRD			25	//第三个

#define  IDS_R_FIRST			22	//右边第一个文本框
#define  IDS_R_SECOND			23	//	
#define  IDS_R_THIRD			24	//	

#define  IDS_SELF_MONEY			28	//显示自己的钱
#define  IDS_GAMENOW_BET		27	//显示本局游戏的投注

#define  IDS_SELF_NAME			19	//自己的名字
#define  IDS_O_NAME				26	//对方的

#define  IDIMG_SELF_DICE			//自己显示色子
#define  IDIMG_O_DICE				//对方的

#define  IDIMG_SELF_CHARA		34	//自己角色头像
#define  IDIMG_O_CHARA			35	//对方的

#define  IDS_DAO_TIMER			40  //倒计时显示

#define  IDIMG_SELF_HP			44
#define  IDIMG_SELF_MP			45

#define  IDIMG_O_HP				54
#define  IDIMG_O_MP				55


#define LIMT_TIME				21	//一局时间限制(秒)

class CMGNumberDlg : public CGuiDialog
{
public:
	CMGNumberDlg(void);
	~CMGNumberDlg(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	void OnSetCloseLocation();
	void OnInitInfo(void *pData);
	void Start();
	void Stop();
	void onResult(DWORD dWRes);
	void Restart(DWORD dwMoney);
	void SetDiceNum(DWORD dwUserId,DWORD dwDiceNum);
	void Update();
	static void ParseChatMessage(stChannelChatUserCmd* pCmd);
	void AppendMessage(TiXmlElement* pPage);
	void SendChat();
	void OnStart();
	
private:
	CAnimation m_ImageSai[2];

	DWORD m_selfid;
	DWORD m_selfdicenum;
	bool m_bResultDaoSelf;
	DWORD m_oid;
	DWORD m_odicenum;
	bool m_bResultDaoO;

	bool m_bUpdateSelf;
	bool m_bUpdateO;

	bool m_bResultDao;
	DWORD m_res;

	DWORD m_dwCurTime3;
};
