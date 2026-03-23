/**
 * \file
 * \version  $Id: RecordClient.h  $
 * \author  
 * \date 
 * \brief 定义网关服务器到档案服务器连接客户端
 */

#ifndef _RECORDCLIENT_H_
#define _RECORDCLIENT_H_

#include "zTCPClient.h"
#include "zMutex.h"

/**
 * \brief 网关与档案服务器的连接
 *
 */
class RecordClient : public zTCPBufferClient
{

	public:
		
		/**
		 * \brief 构造函数
		 *
		 * \param name 名称
		 * \param ip 服务器地址
		 * \param port 服务器端口
		 */
		RecordClient(
				const std::string &name, 
				const std::string &ip, 
				const unsigned short port)
			: zTCPBufferClient(name, ip, port) {};

		bool connectToRecordServer();
		void run();
		bool msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);

};

extern RecordClient *recordClient;

#endif
