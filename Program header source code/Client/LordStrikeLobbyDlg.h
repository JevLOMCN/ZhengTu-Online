#pragma once

class CGuiLordStrikeLobbyDlg : public CGuiDialog
{
public:
	CGuiLordStrikeLobbyDlg();
	~CGuiLordStrikeLobbyDlg();

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);


	enum  enumGameType
	{
		eGameType_Chess = 0,   // 棋类
		eGameType_Card  = 1,   // 牌类
		eGameType_leisure = 2, // 休闲类
		eGameType_Num   = 3,  
	};
	enum enumRoomType
	{
		eNormal,
		eJunior,
		eMiddle,
		eSenior,
	};
	struct stRoom
	{
		std::string strRoomName;
		int   nResideNum;  // 居住者数量
		int   nCapatily;   // 数据 
		int   nMaxTable; // 最大房间数（房间编号规定从0~nMaxTable-1)
		MiniRoomID roomid;  //
		int nNeedMoney;
		enumRoomType type;
	};
	struct stSpecialGame
	{
		std::string strGameName;
		std::list<stRoom>  mListRoom;
	};
	struct stTreeNode
	{
		enumGameType type;
		std::string strType;
		std::list<stSpecialGame> mListGame;
	};

	bool FindRoom(std::string strText,stRoom & room);
	bool FindRoom(MiniRoomID roomid,stRoom & room);
	int  GetRoomNeedMoney(MiniRoomID roomid);
	void RefreshRoom();
	std::list<stTreeNode>  m_TreeData;

private:
	CGuiTree * m_pTree;

	void LoadDescFromFile();
};
