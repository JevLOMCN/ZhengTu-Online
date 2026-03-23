/**
 * \file
 * \version  $Id: zHttpTask.h  $
 * \author  
 * \date 
 * \brief 定义实现轻量级(lightweight)的http服务框架
 */


#ifndef _zHttpTask_h_
#define _zHttpTask_h_

#include <string>
#include <vector>
#include <queue>
#include <list>
#include <unistd.h>
#include <sys/timeb.h>

#include "zSocket.h"
#include "zNoncopyable.h"
#include "zTime.h"

class zHttpTaskPool;

/**
 * \brief 定义轻量级http任务类，封装一些低层接口
 */
class zHttpTask : private zNoncopyable
{

	public:

		/**
		 * \brief 构造函数，用于创建一个对象
		 * \param pool 所属连接池指针
		 * \param sock 套接口
		 * \param addr 地址
		 */
		zHttpTask(
				zHttpTaskPool *pool,
				const int sock,
				const struct sockaddr_in *addr = NULL) : pool(pool), lifeTime()
		{
			pSocket = new zSocket(sock, addr, false);
		}

		/**
		 * \brief 析构函数，用于销毁一个对象
		 */
		virtual ~zHttpTask()
		{
			SAFE_DELETE(pSocket) ;
		}

#ifdef _USE_EPOLL_
		/**
		 * \brief 添加检测事件到epoll描述符
		 * \param kdpfd epoll描述符
		 * \param events 待添加的事件
		 * \param ptr 额外参数
		 */
		void addEpoll(int kdpfd, __uint32_t events, void *ptr)
		{
			if (pSocket)
				pSocket->addEpoll(kdpfd, events, ptr);
		}
		/**
		 * \brief 从epoll描述符中删除检测事件
		 * \param kdpfd epoll描述符
		 * \param events 待添加的事件
		 */
		void delEpoll(int kdpfd, __uint32_t events)
		{
			if (pSocket)
				pSocket->delEpoll(kdpfd, events);
		}
#else
		/**
		 * \brief 填充pollfd结构
		 * \param pfd 待填充的结构
		 * \param events 等待的事件参数
		 */
		void fillPollFD(struct pollfd &pfd, short events)
		{
			if (pSocket)
				pSocket->fillPollFD(pfd, events);
		}
#endif

		/**
		 * \brief 检测是否验证超时
		 * \param ct 当前系统时间
		 * \param interval 超时时间，毫秒
		 * \return 检测是否成功
		 */
		bool checkHttpTimeout(const zRTime &ct, const unsigned long long interval = 2000) const
		{
			return (lifeTime.elapse(ct) > interval);
		}

		/**
		 * \brief http任务主处理函数
		 * \return 是否成功，1表示成功，0，表示还要继续等待，-1，表示失败
		 */
		virtual int httpCore()
		{
			return 1;
		}

		bool sendCmd(const void *pstrCmd, int nCmdLen);

	protected:

		zSocket *pSocket;								/**< 底层套接口 */

	private:

		zHttpTaskPool *pool;							/**< 任务所属的池 */
		zRTime lifeTime;								/**< 连接创建时间记录 */

};

#endif

