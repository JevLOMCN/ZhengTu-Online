/**
 * \file
 * \version  $Id: Union.h  $
 * \author  
 * \date 
 * \brief 定义帮会的基本结构
 *
 */


#ifndef _UNION_H_
#define _UNION_H_

#include "zType.h"

#pragma pack(1)

namespace UnionDef
{

#define WD2BA(wd,ba)       \
do                         \
{                          \
	ba[0]= (wd >>8)&0xFF;	   \
	ba[1]= wd& 0xFF;         \
}while(false)

#define BA2WD(ba,wd)			\
do												\
{													\
		wd=0;									\
		wd=ba[0];							\
		wd = wd << 8;					\
		wd|=ba[1];						\
}while(false)
    
#define SETMEMBERPOWER(ba) Cmd::set_state(ba,Cmd::GENERAL)

#define SETMASTERPOWER(ba)                \
do                                        \
{                                         \
	Cmd::set_state(ba,Cmd::CHANGE_ALIAS);   \
	Cmd::set_state(ba,Cmd::ADD_MEMBER);     \
	Cmd::set_state(ba,Cmd::FIRE_MEMBER);    \
	Cmd::set_state(ba,Cmd::GENERAL);        \
	Cmd::set_state(ba,Cmd::MASTER);        \
	Cmd::set_state(ba,Cmd::DARE);        \
	Cmd::set_state(ba,Cmd::NOTIFY);        \
}while(false)

#define SETSECONDPOWER(ba)                \
do                                        \
{                                         \
	Cmd::set_state(ba,Cmd::CHANGE_ALIAS);   \
	Cmd::set_state(ba,Cmd::ADD_MEMBER);     \
	Cmd::set_state(ba,Cmd::FIRE_MEMBER);    \
	Cmd::set_state(ba,Cmd::GENERAL);        \
	Cmd::set_state(ba,Cmd::NOTIFY); \
}while(false)

#define SETMEMBERPOWER_WD(WD) \
do                            \
{                             \
	BYTE temp[2];               \
	temp[0] = 0x00;			\
	temp[1] = 0x00;			\
	SETMEMBERPOWER(temp);       \
	BA2WD(temp,WD);             \
}while(false)


#define SETMASTERPOWER_WD(WD) \
do                            \
{                             \
	BYTE temp[2];               \
	temp[0] = 0x00;			\
	temp[1] = 0x00;			\
	SETMASTERPOWER(temp);       \
	BA2WD(temp,WD);             \
}while(false)


const DWORD CREATE_UNION_NEED_PRICE_GOLD = 50000; // 建立帮会需要金钱10万
const DWORD CREATE_UNION_NEED_ITEM_ID    = 684;     // 建立帮会所需要的道具ID 天羽令
const DWORD CREATE_UNION_NEED_LEVEL      = 45;      // 建立帮会需要的人物等级
const DWORD DESTROYTIME	 		 = 30*24*60*60; //投票有效期 一个月的秒数
const DWORD CREATE_UNION_NEED_VOTE       =  2;
const DWORD CREATE_UNION_NEED_MAN_NUM    =30;	// 帮会成立需要人数
		
#define DEFAULTMASTERALIAS "帮主"
#define DEFAULTMEMBERALIAS "帮众"
		
struct stUnionInfo	 					// 数据库读取结构，请不要随意修改，修改注意更新所有代码
{
	DWORD		dwUnionID;				// 帮会的ID
	char		name[MAX_NAMESIZE+1];   		// 帮会名称
	DWORD		dwCharID;             			// 帮主的角色ID
	char		masterName[MAX_NAMESIZE+1];		// 帮主的名字
	BYTE		byVote;					// 签名标志为1表示处于签名期间
	DWORD		dwCrTime;				// 帮会创立时间
	WORD		wdLevel;     		    	       	// 帮会级别
	QWORD		qwExp;        		         	// 帮会经验
	DWORD 		dwMana;					// 帮会威望
	DWORD		dwCountryID;				// 所属国家
	DWORD		dwActionPoint;				// 帮会行动力
	DWORD 		dwMoney;				// 帮会资金
	char		note[254+1];				// 帮会介绍
	DWORD		calltimes;				// 令牌使用次数
}__attribute__ ((packed));

struct stUnionMemberInfo                			// 数据库读取结构，请不要随意修改，修改注意更新所有代码
{
	DWORD	dwCharID;					// 会员角色ID
	char	name[MAX_NAMESIZE+1];				// 会员角色名称		
	char    aliasname[MAX_NAMESIZE+1]; 			// 会员别名
	WORD    wdPower;					// 会员权限
	WORD    wdOccupation;					// 会员的职业
	DWORD   dwSeptID;					// 会员所属家族
}__attribute__ ((packed));

}

#pragma pack()
#endif





