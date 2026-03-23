/**
 * \file
 * \version  $Id: Dare.h 6741 2006-06-05 10:12:13Z zjw $
 * \author  仲俊伟,mark.zhong@gmail.com
 * \date 2005年03月16日 10时32分01秒 CST
 * \brief 定义才对战的基本结构
 *
 */


#ifndef _DARE_H_
#define _DARE_H_

#include "zType.h"

#pragma pack(1)

namespace DareDef
{

	const DWORD CREATE_DARE_NEED_PRICE_GOLD = 500; // 对战所扣金额
	const DWORD DARE_WINNER_GOLD = 800; // 对战胜者一方，所获金额
	const DWORD READYTIME  = 300; // 等待应战的时间，单位:秒
	const DWORD ACTIVETIME = 3600; // 对战进行时间，单位:秒
	const DWORD CREATE_UNION_CITY_DARE_NEED_PRICE_MONEY = 20000; //两锭
	const DWORD CREATE_UNION_KING_CITY_DARE_NEED_PRICE_MONEY = 50000; //五锭
	const DWORD CREATE_UNION_NEUTRAL_CITY_DARE_NEED_PRICE_MONEY = 50000; //五锭

	class SupportDareInterface
	{
		public:
			SupportDareInterface(){};
			virtual ~SupportDareInterface(){};


	};

	/// 状态描述
	char str_state[][20]={"DARE_READY", "DARE_READY_QUESTION", "DARE_READY_ACTIVE", "DARE_RETURN_GOLD",
		"DARE_ACTIVE", "DARE_DATAPROCESS", "DARE_READY_OVER", "DARE_WAIT_BOUNTY", "DARE_OVER"};

	char str_type[][30] = {"UNION_DARE", "SCHOOL_DARE", "SEPT_DARE", "SEPT_NPC_DARE","UNION_CITY_DARE","COUNTRY_FORMAL_DARE", "COUNTRY_FORMAL_ANTI_DARE"};

}

#pragma pack()
#endif





