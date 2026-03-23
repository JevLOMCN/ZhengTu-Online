/**
 * \file
 * \version  $Id: Quiz.h  $
 * \author  
 * \date 
 * \brief 定义才对战的基本结构
 *
 */


#ifndef _DARE_H_
#define _DARE_H_

#include "zType.h"

#pragma pack(1)

namespace QuizDef
{
	const DWORD READYTIME  = 300; // 等待应战的时间，单位:秒
	const DWORD ACTIVETIME = 3600; // 对战进行时间，单位:秒


	const DWORD PERSONAL_QUIZ_NEED_GOLD = 100; // 个人问答，所需银两
	enum
	{
		WORLD_QUIZ = 0,  // 全区竞赛
		PERSONAL_QUIZ = 1 // 个人问答
	};

	/// 状态描述
	char str_state[][30]={"QUIZ_READY", "QUIZ_READY_QUESTION", "QUIZ_SEND_QUESTION", "QUIZ_ACTIVE_QUESTION", 
		"QUIZ_END_QUESTION", "QUIZ_READY_OVER", "QUIZ_READ_SORT", "QUIZ_RETURN_GOLD", "QUIZ_OVER"};
	char str_type[][20] = {"WORLD_QUIZ", "PERSONAL_QUIZ"};

}

#pragma pack()
#endif





