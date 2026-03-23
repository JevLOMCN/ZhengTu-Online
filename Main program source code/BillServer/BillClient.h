/**
 * \file
 * \version  $Id: BillClient.h  $
 * \author  
 * \date 
 * \brief 定义计费客户端
 * <p>
 * 提供计费客户端回调函数类型定义，<br>
 * 以及计费客户端模块初始化和回收接口。
 * </p>
 */


#ifndef _BillClient_h_
#define _BillClient_h_

#include "BillCallback.h"
#include "zTCPClientTask.h"
#include "MessageQueue.h"

enum NetType
{
	NetType_near = 0,		//近程路由，电信区连电信服务器，网通区连网通服务器
	NetType_far = 1			//远端路由，电信区连网通服务器，网通区连电信服务器
};

/**
 * \brief 计费客户端连接
 */
class BillClient : public zTCPClientTask , public MessageQueue
{
	public:

		BillClient(const std::string &ip, const unsigned short port, BillCallback &bc, const unsigned int my_id);
		~BillClient();

		int checkRebound();
		void addToContainer();
		void removeFromContainer();
		bool connect();
		bool action(BillData *bd);
		bool msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);
		bool cmdMsgParse(const Cmd::t_NullCmd *, const unsigned int);

		const NetType getNetType() const
		{
			return netType;
		}

		const unsigned int getID() const
		{
			return my_id;
		}

	private:

		BillCallback &bc;
		NetType netType;
		unsigned int my_id;
		GameZone_t gameZone;
		char gameZone_str[6];

};


#endif

