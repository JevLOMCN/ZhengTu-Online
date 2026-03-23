/**
 * \file
 * \version  $Id: BillClientManager.h  $
 * \author  
 * \date 
 * \brief 定义统一用户平台计费客户端连接的管理容器
 */


#ifndef _BillClientManager_h_
#define _BillClientManager_h_

#include <set>

#include "zType.h"
#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "BillClient.h"
#include "zTime.h"
#include "zLogger.h"
#include "zConfile.h"

/**
 * \brief 统一用户平台登陆服务器的计费客户端连接类管理器
 */
class BillClientManager : public SingletonBase<BillClientManager>
{

	public:

		bool init(const std::string &confile, const std::string &tradelog, BillCallback &bc);
		void timeAction(const zTime &ct);
		void add(BillClient *billClient);
		void remove(BillClient *billClient);

		bool action(BillData *bd);
		void execEvery();

		/**
		 * \brief 交易记录的log
		 */
		static zLogger *tlogger;

		/**
		 * \brief 充值消费来源,从配置文件里取得
		 */
		static int	source;

	private:

		friend class SingletonBase<BillClientManager>;
		BillClientManager();
		~BillClientManager();

		BillCallback bc;

		/**
		 * \brief 客户端连接管理池
		 */
		zTCPClientTaskPool *billClientPool;
		/**
		 * \brief 进行断线重连检测的时间记录
		 */
		zTime actionTimer;
		unsigned int maxID;

		struct lt_client
		{
			bool operator()(BillClient *s1, BillClient *s2) const
			{
				if (s1->getID() == s2->getID())
					return s1->getNetType() < s2->getNetType();
				else
					return s1->getID() < s2->getID();
			}
		};
		/**
		 * \brief 存放连接已经成功的连接容器类型
		 */
		typedef std::multiset<BillClient *, lt_client> BillClientContainer;
		typedef BillClientContainer::iterator iter;
		typedef BillClientContainer::const_iterator const_iter;
		typedef BillClientContainer::size_type size_type;
		/**
		 * \brief 存放连接已经成功的连接容器
		 */
		BillClientContainer allClients;
		/**
		 * \brief 容器访问互斥变量
		 */
		zRWLock rwlock;

};

#endif

