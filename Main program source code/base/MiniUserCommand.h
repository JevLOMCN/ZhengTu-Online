#ifndef _MINI_USER_COMMAND_H_
#define _MINI_USER_COMMAND_H_

#include "zType.h"
#include "Command.h"
#pragma pack(1)

namespace Cmd
{ 

//BEGIN_ONE_CMD
//////////////////////////////////////////////////////////////
/// 定义小游戏命令开始
//////////////////////////////////////////////////////////////

struct stMiniGameUserCmd : public stNullUserCmd
{
	stMiniGameUserCmd()
	{
		byCmd = MINIGAME_USERCMD;
		subParam = 0;
	}
	BYTE subParam;
};

/*公共指令开始***************************************/

enum MiniGameType
{
	DOUDIZHU	= 1,
	CAISHUZI	= 2,
	MAX_GAMETYPE
};

struct MiniUserPosition
{
	BYTE type;
	BYTE room;
	BYTE game;
	BYTE seat;

	MiniUserPosition(DWORD t=0, DWORD r=0, DWORD g=0, DWORD s=0):type(t),room(r),game(g),seat(s){}
	MiniUserPosition(const MiniUserPosition &p):type(p.type),room(p.room),game(p.game),seat(p.seat){}
	MiniUserPosition & operator =(const MiniUserPosition &p)
	{
		type = p.type;
		room = p.room;
		game = p.game;
		seat = p.seat;
		return *this;
	}
	bool operator ==(const MiniUserPosition &p)
	{
		return type==p.type && room==p.room && game==p.game && seat==p.seat;
	}

	DWORD roomID() const{return (type<<24)+(room<<16);}
	DWORD gameID() const{return (type<<24)+(room<<16)+(game<<8);}
	DWORD id() const{return (type<<24)+(room<<16)+(game<<8)+seat;}
};
typedef MiniUserPosition MiniGameID;
typedef MiniUserPosition MiniRoomID;
typedef MiniUserPosition MiniSeatID;

struct MiniGameScore
{
	BYTE gameType;
	DWORD win;
	DWORD lose;
	DWORD draw;
	int score;
	int money;

	MiniGameScore():gameType(0),win(0),lose(0),draw(0),score(0),money(0){}
	MiniGameScore & operator=(const MiniGameScore &s)
	{
		win = s.win;
		lose = s.lose;
		draw = s.draw;
		score = s.score;
		money = s.money;
		return *this;
	}

	MiniGameScore & operator+=(const MiniGameScore &s)
	{
		win += s.win;
		lose += s.lose;
		draw += s.draw;
		score += s.score;
		money += s.money;
		return *this;
	}
	bool operator==(const MiniGameScore &s) const
	{
		return score==s.score && win==s.win && draw==s.draw && lose==s.lose;
	}
	bool operator<(const MiniGameScore &s) const
	{
		if (score==s.score)
			if (win==s.win)
				if (draw==s.draw)
					return lose>s.lose;
				else
					return draw<s.draw;
			else
				return win<s.win;
		else
			return score<s.score;
	}
	bool operator>(const MiniGameScore &s) const
	{
		return !(*this==s) && !(*this<s);
	}
};

enum MiniGameState
{
	MGS_PREPARE = 1,
	MGS_PLAY    = 2
};

#define COMMON_MINI_PARA 1
struct stCommonMiniGameCmd : public stMiniGameUserCmd
{
	stCommonMiniGameCmd() : stMiniGameUserCmd()
	{
		byParam = COMMON_MINI_PARA;
	}
};

/*
#define INVIT_COMMON_MINI_PARA 1
struct stInvitCommonMiniGameCmd : public stCommonMiniGameCmd
{
stInvitCommonMiniGameCmd() : stCommonMiniGameCmd()
{
subParam = INVIT_COMMON_MINI_PARA;
}
DWORD tempID;//被邀请者/邀请者 临时ID
DWORD gameID;//游戏类型
};

#define INVIT_RET_COMMON_MINI_PARA 2
struct stInvitRetCommonMiniGameCmd : public stCommonMiniGameCmd
{
stInvitRetCommonMiniGameCmd() : stCommonMiniGameCmd()
{
subParam = INVIT_RET_COMMON_MINI_PARA;
}
DWORD tempid;//邀请者临时ID
BYTE ret;//0:不同意 1:同意 2:屏蔽
};
*/

#define LOGIN_COMMON_MINI_PARA 3
struct stLoginCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stLoginCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = LOGIN_COMMON_MINI_PARA;
	}
	DWORD ret;//参数/登陆是否成功
};

#define LOGOUT_COMMON_MINI_PARA 4
struct stLogoutCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stLogoutCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = LOGOUT_COMMON_MINI_PARA;
	}
};

struct MiniRoomData
{
	MiniRoomID roomID;
	WORD gameNum;
	WORD oneGameUserNum;
	DWORD userNum;
	DWORD money;//游戏币基数
};
//房间列表
#define GAME_LIST_COMMON_MINI_PARA 5
struct stGameListCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stGameListCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = GAME_LIST_COMMON_MINI_PARA;
	}
	DWORD num;//房间数量
	MiniRoomData data[0];
};

//进入房间
#define ENTER_ROOM_COMMON_MINI_PARA 6
struct stEnterRoomCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stEnterRoomCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = ENTER_ROOM_COMMON_MINI_PARA;
	}
	MiniRoomID roomID;//房间号
};

enum MiniUserState
{
	MUS_NOTPLAY	= 0,
	MUS_HALL	= 1,
	MUS_ROOM	= 2,
	MUS_SEAT	= 3,
	MUS_READY	= 4,
	MUS_PLAY	= 5
};
struct MiniUserData
{
	DWORD id;
	char name[MAX_NAMESIZE];
	WORD face;//头像
	DWORD countryID;
	MiniUserPosition pos;
	BYTE state;
	MiniGameScore score;
	MiniUserData():id(0),face(0),countryID(0),state(0){bzero(name, MAX_NAMESIZE);}
	MiniUserData(const MiniUserData &d)
	{
		id = d.id;
		bcopy(d.name, name, MAX_NAMESIZE-1);
		face = d.face;
		countryID = d.countryID;
		pos = d.pos;
		state = d.state;
		score = d.score;
	}

	MiniUserData & operator=(const MiniUserData &d)
	{
		id = d.id;
		bcopy(d.name, name, MAX_NAMESIZE-1);
		face = d.face;
		countryID = d.countryID;
		pos = d.pos;
		state = d.state;
		score = d.score;
		return *this;
	}
};
//添加/更新房间玩家列表
#define ADD_ROOM_USER_COMMON_MINI_PARA 7
struct stAddRoomUserCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stAddRoomUserCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = ADD_ROOM_USER_COMMON_MINI_PARA;
		num = 0;
	}
	MiniRoomID roomID;
	DWORD num;
	MiniUserData data[0];
};
//添加/更新一个房间玩家列表
#define ADD_ONE_ROOM_USER_COMMON_MINI_PARA 7
struct stAddOneRoomUserCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stAddOneRoomUserCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = ADD_ONE_ROOM_USER_COMMON_MINI_PARA;
		num = 1;
	}
	MiniRoomID roomID;
	DWORD num;
	MiniUserData data;
};

struct MiniSeatData
{
	MiniSeatID seatID;
	BYTE state;//0关闭 1打开
	BYTE isHost;//0否 1是 
};
//刷新座位开关状态
#define SEAT_STATE_COMMON_MINI_PARA 8
struct stSeatStateCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stSeatStateCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = SEAT_STATE_COMMON_MINI_PARA;
		num = 0;
	}
	DWORD num;
	MiniSeatData data[0];
};
//刷新一个座位开关状态
#define ONE_SEAT_STATE_COMMON_MINI_PARA 8
struct stOneSeatStateCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stOneSeatStateCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = ONE_SEAT_STATE_COMMON_MINI_PARA;
		num = 1;
	}
	DWORD num;
	MiniSeatData data;
};

//离开房间
#define LEAVE_ROOM_COMMON_MINI_PARA 9
struct stLeaveRoomCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stLeaveRoomCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = LEAVE_ROOM_COMMON_MINI_PARA;
	}
	MiniRoomID roomID;
};

//坐桌子
#define ENTER_GAME_COMMON_MINI_PARA 10
struct stEnterGameCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stEnterGameCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = ENTER_GAME_COMMON_MINI_PARA;
	}
	MiniSeatID seatID;//座位号
};

/*
//添加/更新游戏玩家列表
#define ADD_GAME_USER_COMMON_MINI_PARA 11
struct stAddGameUserCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stAddGameUserCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = ADD_GAME_USER_COMMON_MINI_PARA;
		num = 0;
	}
	MiniGameID gameID;
	DWORD num;
	MiniUserData data[0];
};

//添加/更新一个游戏玩家列表
#define ADD_ONE_GAME_USER_COMMON_MINI_PARA 11
struct stAddOneGameUserCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stAddOneGameUserCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = ADD_ONE_GAME_USER_COMMON_MINI_PARA;
		num = 1;
	}
	MiniGameID gameID;
	DWORD num;
	MiniUserData data;
};
*/

//离开桌子
#define LEAVE_GAME_COMMON_MINI_PARA 12
struct stLeaveGameCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stLeaveGameCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = LEAVE_GAME_COMMON_MINI_PARA;
	}
	MiniGameID gameID;
};

/*
//成为游戏主持者
#define HOST_GAME_COMMON_MINI_PARA 13
struct stHostGameCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stHostGameCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = HOST_GAME_COMMON_MINI_PARA;
		hostID = 0;
	}
	MiniGameID gameID;
	DWORD hostID;
};
*/

//打开/关闭某个位子
#define TOGGLE_SEAT_COMMON_MINI_PARA 14
struct stToggleSeatCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stToggleSeatCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = TOGGLE_SEAT_COMMON_MINI_PARA;
		open = 1;
	}
	MiniSeatID seatID;
	BYTE open;//0关闭 1打开
};

//踢某人
#define KICK_USER_COMMON_MINI_PARA 15
struct stKickUserCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stKickUserCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = KICK_USER_COMMON_MINI_PARA;
		userID = 0;
	}
	MiniGameID gameID;
	DWORD userID;
};

//切换准备/未准备状态
#define TOGGLE_READY_COMMON_MINI_PARA 16
struct stToggleReadyCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stToggleReadyCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = TOGGLE_READY_COMMON_MINI_PARA;
	}
	MiniGameID gameID;
};

//进入大厅确认消息
#define LOGIN_RET_COMMON_MINI_PARA 17
struct stLoginRetCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stLoginRetCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = LOGIN_RET_COMMON_MINI_PARA;
		ret = 0;
	}
	BYTE ret;//1成功 0失败
};

//进入房间确认消息
#define ENTER_ROOM_RET_COMMON_MINI_PARA 18
struct stEnterRoomRetCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stEnterRoomRetCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = ENTER_ROOM_RET_COMMON_MINI_PARA;
		ret = 0;
	}
	BYTE ret;//1成功 0失败 2房间已满 3已经参加该类型的游戏
	MiniRoomID roomID;
};

//进入游戏确认消息
#define ENTER_GAME_RET_COMMON_MINI_PARA 19
struct stEnterGameRetCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stEnterGameRetCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = ENTER_GAME_RET_COMMON_MINI_PARA;
		ret = 0;
	}
	BYTE ret;//1成功 0失败 2人数已满 3游戏已开始 4座位已有人 5豆子不足 
	MiniSeatID seatID;
};

//刷新玩家状态
#define UPDATE_USER_STATE_COMMON_MINI_PARA 20
struct stUpdateUserStateCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stUpdateUserStateCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = UPDATE_USER_STATE_COMMON_MINI_PARA;
		userID = 0;
		state = MUS_NOTPLAY;
	}
	DWORD userID;//玩家ID
	BYTE state;//玩家状态
	MiniUserPosition pos;//当前位置
};

//游戏开始
#define GAME_START_COMMON_MINI_PARA 21
struct stGameStartCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stGameStartCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = GAME_START_COMMON_MINI_PARA;
	}
	MiniGameID gameID;
};

//游戏结束
#define GAME_END_COMMON_MINI_PARA 22
struct stGameEndCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stGameEndCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = GAME_END_COMMON_MINI_PARA;
	}
	MiniGameID gameID;
};

//通知游戏结果/分数
#define GAME_RESULT_COMMON_MINI_PARA 23
struct stGameResultCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stGameResultCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = GAME_RESULT_COMMON_MINI_PARA;
		userID = 0;
	}
	MiniGameID gameID;
	DWORD userID;
	MiniGameScore score;
};

/*
//请求玩家的分数
#define REQ_SCORE_COMMON_MINI_PARA 24
struct stReqScoreCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stReqScoreCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = REQ_SCORE_COMMON_MINI_PARA;
		userID = 0;
		gameType = 1;
	}
	DWORD userID;
	BYTE gameType;
};
*/

//发送玩家的分数
#define USER_SCORE_COMMON_MINI_PARA 25
struct stUserScoreCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stUserScoreCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = USER_SCORE_COMMON_MINI_PARA;
		userID = 0;
	}
	DWORD userID;
	MiniGameScore score;
};

enum MiniChatType
{
	MCT_NORMAL,//普通
	MCT_SYS,//系统
	MCT_POPUP//弹出窗口
};

//聊天
#define CHAT_COMMON_MINI_PARA 26
struct stChatCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stChatCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = CHAT_COMMON_MINI_PARA;
		userID = 0;
		type = MCT_NORMAL;
		bzero(content, sizeof(content));
	}
	DWORD userID;
	MiniUserPosition pos;//说话位置
	BYTE type;//聊天类型
	char content[64];
};

//请求各房间数据
#define REQ_ROOM_DATA_COMMON_MINI_PARA 27
struct stReqRoomDataCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stReqRoomDataCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = REQ_ROOM_DATA_COMMON_MINI_PARA;
	}
};

//请求排行榜
#define REQ_TOP_COMMON_MINI_PARA 28
struct stReqTopCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stReqTopCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = REQ_TOP_COMMON_MINI_PARA;
	}
};

//发送排行榜
#define RET_TOP_COMMON_MINI_PARA 29
struct stRetTopCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stRetTopCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = RET_TOP_COMMON_MINI_PARA;
		num = 0;
	}
	DWORD num;
	MiniUserData data[0];
};

//充游戏币
#define DEPOSIT_COMMON_MINI_PARA 30
struct stDepositCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stDepositCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = DEPOSIT_COMMON_MINI_PARA;
		num = 0;
	}
	DWORD num;
};

//兑换银两
#define DRAW_COMMON_MINI_PARA 31
struct stDrawCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stDrawCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = DRAW_COMMON_MINI_PARA;
		num = 0;
	}
	DWORD num;
};

//兑换银两返回
#define DRAW_RET_COMMON_MINI_PARA 32
struct stDrawRetCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stDrawRetCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = DRAW_RET_COMMON_MINI_PARA;
		ret = 0;
	}
	BYTE ret;//0失败 1成功 2不足 3包裹满
};

//发送玩家自己的信息
#define USER_DATA_COMMON_MINI_PARA 33
struct stUserDataCommonMiniGameCmd : public stCommonMiniGameCmd
{
	stUserDataCommonMiniGameCmd() : stCommonMiniGameCmd()
	{
		subParam = USER_DATA_COMMON_MINI_PARA;
	}
	MiniUserData data;
};

/*公共指令结束****************************************/

/*牌类指令开始**************************************/
class Card
{
	public:
		enum Suit
		{
			Diamond = 0x10,
			Club = 0x20,
			Heart = 0x30,
			Spade = 0x40,
			Joker = 0x50
		};

		Card():value(0){}
		Card(const Card &c):value(c.value){}
		Card(const unsigned char v):value(v){}
		Card(const Suit s, const unsigned char n):value(s|n){}

		unsigned char suit() const {return value & 0xf0;}
		unsigned char number() const {return value & 0x0f;}

		bool valid()
		{
			if (!suit() || suit()>5) return false;
			if (!number() || number()>13) return false;
			if (Joker==suit() && number()<14) return false;
			return true;
		}

		bool operator==(const Card &c) const{return number()==c.number();}
		bool operator<(const Card &c) const{return number()<c.number();}
		bool operator>(const Card &c) const{return number()>c.number();}
	private:
		unsigned char value;
};

#define CARD_MINI_PARA 2
struct stCardMiniGameCmd : public stMiniGameUserCmd
{
	stCardMiniGameCmd() : stMiniGameUserCmd()
	{
		byParam = CARD_MINI_PARA;
	}
	MiniGameID gameID;
};

//发牌
#define DEAL_CARD_MINI_PARA 1
struct stDealCardMiniGameCmd : public stCardMiniGameCmd
{
	stDealCardMiniGameCmd() : stCardMiniGameCmd()
	{
		subParam = DEAL_CARD_MINI_PARA;
		num = 0;
	}
	BYTE num;
	Card cards[0];
};

//通知某人出牌
#define NOTIFY_PUT_CARD_MINI_PARA 2
struct stNotifyPutCardMiniGameCmd : public stCardMiniGameCmd
{
	stNotifyPutCardMiniGameCmd() : stCardMiniGameCmd()
	{
		subParam = NOTIFY_PUT_CARD_MINI_PARA;
		userID = 0;
	}
	DWORD userID;
};

//出牌
#define PUT_CARD_MINI_PARA 3
struct stPutCardMiniGameCmd : public stCardMiniGameCmd
{
	stPutCardMiniGameCmd() : stCardMiniGameCmd()
	{
		subParam = PUT_CARD_MINI_PARA;
		userID = 0;
		num = 0;
	}
	DWORD userID;
	BYTE num;
	Card cards[0];
};

//通知某人叫分
#define NOTIFY_POINT_DDZ_MINI_PARA 4
struct stNotifyPointDDZMiniGameCmd : public stCardMiniGameCmd
{
	stNotifyPointDDZMiniGameCmd() : stCardMiniGameCmd()
	{
		subParam = NOTIFY_POINT_DDZ_MINI_PARA;
		userID = 0;
	}
	DWORD userID;
};

//斗地主叫分
#define POINT_DDZ_MINI_PARA 5
struct stPointDDZMiniGameCmd : public stCardMiniGameCmd
{
	stPointDDZMiniGameCmd() : stCardMiniGameCmd()
	{
		subParam = POINT_DDZ_MINI_PARA;
		userID = 0;
		num = 0;
	}
	DWORD userID;
	BYTE num;
};

//显示底牌
#define RESERVE_CARDS_DDZ_MINI_PARA 6
struct stReserveCardsDDZMiniGameCmd : public stCardMiniGameCmd
{
	stReserveCardsDDZMiniGameCmd() : stCardMiniGameCmd()
	{
		subParam = RESERVE_CARDS_DDZ_MINI_PARA;
		userID = 0;
		num = 0;
	}
	DWORD userID;
	BYTE num;
	Card cards[0];
};

//摊牌
#define SHOW_HAND_DDZ_MINI_PARA 6
struct stShowHandDDZMiniGameCmd : public stCardMiniGameCmd
{
	stShowHandDDZMiniGameCmd() : stCardMiniGameCmd()
	{
		subParam = SHOW_HAND_DDZ_MINI_PARA;
		userID = 0;
		num = 0;
	}
	DWORD userID;
	BYTE num;
	Card cards[0];
};

/*牌类指令结束****************************************/

/*******************************************************/
// 邀请
#define INVITE_MINI_PARA 11
struct stInviteMiniGame : public stMiniGameUserCmd
{
	stInviteMiniGame()
	{
		byParam = INVITE_MINI_PARA;
	}
	DWORD tempid;//邀请者/被邀请者的临时ID
	DWORD money;//赌注
};

// 邀请回复
#define INVITE_RET_MINI_PARA 12
struct stInviteRetMiniGame : public stMiniGameUserCmd
{
	stInviteRetMiniGame()
	{
		byParam = INVITE_RET_MINI_PARA;
	}
	DWORD tempid;//邀请者的临时ID
	DWORD money;//赌注
	DWORD ret;//1:同意 0:不同意
};

// 开始游戏
#define START_MINI_PARA 13
struct stStartMiniGame : public stMiniGameUserCmd
{
	stStartMiniGame()
	{
		byParam = START_MINI_PARA;
	}
	DWORD tempid;//对方的临时ID
	DWORD money;//赌注
};

// 停止色子
#define STOP_DICE_MINI_PARA 14
struct stStopDiceMiniGame : public stMiniGameUserCmd
{
	stStopDiceMiniGame()
	{
		byParam = STOP_DICE_MINI_PARA;
	}
};

// 色子数字
#define DICE_NUM_MINI_PARA 15
struct stDiceNumMiniGame : public stMiniGameUserCmd
{
	stDiceNumMiniGame()
	{
		byParam = DICE_NUM_MINI_PARA;
	}
	DWORD tempid;//掷色子人的临时ID
	DWORD num;
};

// 结果
#define RESULT_MINI_PARA 16
struct stResultMiniGame : public stMiniGameUserCmd
{
	stResultMiniGame()
	{
		byParam = RESULT_MINI_PARA;
	}
	DWORD res;//0:输 1:赢 2:平
};

// 游戏结束 //主动离开游戏
#define END_MINI_PARA 17
struct stEndMiniGame : public stMiniGameUserCmd
{
	stEndMiniGame()
	{
		byParam = END_MINI_PARA;

		reason = 0;
	}
	DWORD reason;//结束原因 0:正常结束 1:对方离开
};

// 继续游戏
#define CONTINUE_MINI_PARA 18
struct stContinueMiniGame : public stMiniGameUserCmd
{
	stContinueMiniGame()
	{
		byParam = CONTINUE_MINI_PARA;
	}
	DWORD ret;//0:不继续 1:继续
};

// 通知客户端进入游戏
#define ENTER_MINI_PARA 19
struct stEnterMiniGame : public stMiniGameUserCmd
{
	stEnterMiniGame()
	{
		byParam = ENTER_MINI_PARA;
	}
};
/*******************************************************/

//////////////////////////////////////////////////////////////
/// 定义小游戏命令结束
//////////////////////////////////////////////////////////////

}
#pragma pack()

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
