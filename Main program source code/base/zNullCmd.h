/**
 * \file
 * \version  $Id: zNullCmd.h  $
 * \author  
 * \date 
 * \brief 定义空的指令
 *
 * 负责服务器内部交换使用，和客户端交互的指令需要另外定义
 * 
 */


#ifndef _zNullCmd_h_
#define _zNullCmd_h_

#include "zType.h"

#pragma pack(1)

namespace Cmd
{
	const BYTE CMD_NULL = 0;		/**< 空的指令 */
	const BYTE PARA_NULL = 0;	/**< 空的指令参数 */

	/**
	 * \brief 空操作指令，测试信号和对时间指令
	 *
	 */
	struct t_NullCmd
	{
		BYTE cmd;					/**< 指令代码 */
		BYTE para;					/**< 指令代码子编号 */
		/**
		 * \brief 构造函数
		 *
		 */
		t_NullCmd(const BYTE cmd = CMD_NULL, const BYTE para = PARA_NULL) : cmd(cmd), para(para) {};
	};
};

#pragma pack()

#endif

