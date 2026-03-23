#pragma once

enum enumGameState
{
	eGameState_Null,
	eGameState_Login,
	eGameState_Run,
	eGameState_Logout,
	eGameState_UnLogin,
	eGameState_LoginOK,
};

enum enumGameStateItem
{
	eGameStateItem_Normal,
	eGameStateItem_TradeRequest,// 对方请求状态
	eGameStateItem_TradeRun,	// 交易进行中
	eGameStateItem_NpcDialog,	// 访问NPC中
	eGameStateItem_NpcTrade,
	eGameStateItem_Up,         //物品升级、改造、打造中
	eGameStateItem_Die, 
	eGameStateItem_Quiz,         //答题竞赛中
};


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGameState
{
	enumGameState m_state;
	enumGameStateItem m_stateItem;
public:

	CGameState(void);
	~CGameState(void);

	enumGameState GetState(){ return m_state; }
	enumGameStateItem GetStateItem() { return m_stateItem; }
	void SetState(enumGameState e) { m_state = e;}
	void SetStateItem(enumGameStateItem e) { m_stateItem = e; }
	bool IsState(enumGameState s) { return m_state == s;}
	bool IsStateItem(enumGameStateItem s) { return m_stateItem == s; }
};

extern CGameState* GetGameState();
