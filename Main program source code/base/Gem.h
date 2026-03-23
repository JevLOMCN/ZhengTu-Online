/**
 * \file
 * \version  $Id: Gem.h 5600 2006-02-28 12:39:55Z zjw $
 * \author  仲俊伟,mark.zhong@gmail.com
 * \date 2005年03月16日 10时32分01秒 CST
 * \brief 定义护宝任务的基本结构
 *
 */


#ifndef _GEM_H_
#define _GEM_H_

#include "zType.h"

#pragma pack(1)

namespace GemDef
{

	const DWORD GEM_ACTIVE_TIME = 4*60*60; // 护宝任务进行时间


	/// 状态描述
	char str_gem_state[][20]={"GEM_READY", "GEM_ACTIVE", "GEM_READY_OVER", "GEM_OVER"};

}

#pragma pack()
#endif





