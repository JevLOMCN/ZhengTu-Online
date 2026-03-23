/**
 * \file
 * \version  $Id: Sept.h  $
 * \author  
 * \date 
 * \brief 定义家族的基本结构
 *
 */


#ifndef _SEPT_H_
#define _SEPT_H_

#include "zType.h"

#pragma pack(1)

namespace SeptDef
{

//#define DWORD CREATE_SEPT_NEED_ITEM_ID    = 0	// 建立家族所需要的道具ID 为0表示不需要
const DWORD CREATE_SEPT_NEED_PRICE_GOLD = 2000;	// 建立家族需要金钱20两
const DWORD CREATE_SEPT_NEED_LEVEL     = 35;	// 建立家族需要的人物等级
const DWORD JOIN_SEPT_NEED_LEVEL		= 20;	// 加入家族需要的人物等级
const DWORD PER_LEVEL_MAN_NUM		= 1;		// 每级别增加人数
const DWORD DESTROYTIME					= 30*24*60*60; //投票有效期 一个月的秒数
const DWORD CREATE_SEPT_NEED_MAN_NUM = 200;		// 建立家族人数 恋恋
		
struct stSeptInfo											// 数据库读取结构，请不要随意修改，修改注意更新所有代码
{
	DWORD		dwSeptID;									  // 家族的ID
	char		name[MAX_NAMESIZE+1];   		// 家族名称
	DWORD		dwCharID;             			// 会长的角色ID
	char		masterName[MAX_NAMESIZE+1];		// 会长的名字
	BYTE		byVote;					// 投票标志为1表示处于投票期间
	DWORD		dwCrTime;				// 家族创立时间
	DWORD 		dwRepute;				// 声望
	DWORD 		dwCountryID;				// 家族所属国家（以族长国籍为根据）
	DWORD 		dwUnionID;				// 家族所属帮会
	DWORD 		dwLevel;				// 家族等级
	DWORD 		dwSpendGold;				// 家族消费金币
	DWORD 		dwIsExp;				// 是否领取家族经验
	char		note[254+1];				// 家族介绍
	DWORD		calltimes;				// 令牌使用次数
	DWORD		calldaytime;				// 令牌使用天数
	DWORD		normalexptime;				// 令牌使用天数
	DWORD		bossexp;				// 家族BOSS经验

}__attribute__ ((packed));


struct stSeptMemberInfo                			// 数据库读取结构，请不要随意修改，修改注意更新所有代码
{
	DWORD	dwCharID;				// 会员角色ID
	char	name[MAX_NAMESIZE+1];			// 会员角色名称		
	char    aliasname[MAX_NAMESIZE+1];		// 别名
	WORD    wdOccupation;				// 会员的职业
}__attribute__ ((packed));

}

#pragma pack()
#endif






