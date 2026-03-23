/**
 * \file
 * \version  $Id: MessageSystem.h 3203 2005-10-03 03:56:05Z fangq $
 * \author  宋仕良,songsiliang@netease.com
 * \date 2005年01月29日 21时00分38秒 CST
 * \brief 系统提示信息
 *
 * 
 */

#ifndef _MessageSystem_h_
#define _MessageSystem_h_

#include <vector>
#include "zType.h"
#include "Command.h"
#include "zRWLock.h"
#include "SceneUser.h"

/**
 * \brief 消息系统
 *
 */
class MessageSystem
{

	public:

		~MessageSystem()
		{
			final();
		}

		/**
		 * \brief 得到唯一实例
		 *
		 */
		static MessageSystem &getInstance()
		{
			if (NULL == instance)
				instance = new MessageSystem();

			return *instance;
		}

		/**
		 * \brief 卸载唯一实例
		 *
		 */
		static void delInstance()
		{
			SAFE_DELETE(instance);
		}

		bool init();
		void check(SceneUser *sceneUser, const bool init = false);

	private:

		static MessageSystem *instance;

		MessageSystem() {};
		void final();

		/**
		 * \brief 消息数据结构
		 *
		 */
		struct t_Message
		{
			time_t starttime;
			time_t endtime;
			bool login;
			WORD interval;
			int count;
			int order;
			Cmd::stChannelChatUserCmd cmd;
		};

		std::vector<t_Message> messages;
		zRWLock rwlock;

};

#endif

