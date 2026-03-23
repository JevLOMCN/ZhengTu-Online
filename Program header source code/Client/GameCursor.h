#pragma once
#include "..\gui\include\cursor.h"

enum enumCursor{
	eCursorNone = -1,
	eCursorNormal,		// 正常
	eCursorTrade,		// 交易状态

	eCursorAttack,

	eCursorWorkDig,
	eCursorWorkLumber,
	eCursorWorkGather,

	eCursorSetAccess,	
	eCursorSetAccessInEmotion, 

	eCursorDictate,		// 情感
	eCursorAppoint,		// 任命
	eCursorFriend,		// 好友
	eCursorTeam,		// 招收队员
	eCursorGetItem,		// 拾取物体
	eCursorExit,		// 出口
	eCursorSpeak,		// 说话
	eCursorConv,		// 传送点
	eCursorRepair,		// 修理
	eCursorShowUser,     //查看玩家  
	eCursorFollowUser,   //跟随玩家  

	eCursorUseAccess,
	eCursorFindAccess,

	eCursorAttack1,		// attack type==21 special NPC

	eCursorFreeMove,	// character move with mouse move

	eCursorEditAccess,
	eCursorPetAttack,	//宠物攻击
	eCursorGetMailItem,	///选择放入邮件的物品
	//eCursorSummonAttack,	//召唤兽攻击
	 

	eCursorNum,
};


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGameCursor :
	public CCursor
{
	enumCursor m_imageType;

	enumCursor m_CursorType;
	std::map<int,stPointI> mHots;
public:
	CGameCursor(void);
	~CGameCursor(void);
	void SetCursor(enumCursorType type,enumCursor image);
	void SetCursorImageType( const enumCursor& type ) { if ( type < eCursorNum ) m_imageType = type; }
	enumCursor GetCursorImageType(){ return m_imageType;}
	void SetCursorTypeEx(const enumCursor& type);
	enumCursor GetCursorTypeEx() { return m_CursorType;}

	virtual void OnSetHardwareCursor(const stResourceLocation* rl,stPointI ptHot);
	virtual HCURSOR GetCursorHandle(const stResourceLocation * rl);
	virtual HCURSOR GetNormalCursorHandle();
};

extern CGameCursor* GetGameCursor();