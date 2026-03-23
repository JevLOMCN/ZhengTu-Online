/**
 * \file
 * \version  $Id: BillCallback.h  $
 * \author  
 * \date 
 * \brief 定义计费客户端接口
 * <p>
 * 提供计费客户端回调函数类型定义，<br>
 * 以及计费客户端模块初始化和回收接口。
 * </p>
 */


#ifndef _BillCallback_h_
#define _BillCallback_h_

#ifndef _SingletonBase_
#define _SingletonBase_
template <typename T>
class SingletonBase
{
	public:
		SingletonBase() {}
		virtual ~SingletonBase() {}
		static T& getInstance()
		{
			assert(instance);
			return *instance;
		}
		static void newInstance()
		{
			SAFE_DELETE(instance);
			instance = new T();
		}
		static void delInstance()
		{
			SAFE_DELETE(instance);
		}
	protected:
		static T* instance;
	private:
		SingletonBase(const SingletonBase&);
		SingletonBase & operator= (const SingletonBase &);
};
template <typename T> T* SingletonBase<T>::instance = NULL;
#endif

#include "UserCommand.h"
#ifdef	__cplusplus
extern "C" {
#endif
	/**
	 * \brief 计费交易回调数据
	 * 向回调函数中传数据处理
	 */
	struct BillData
	{ 
		unsigned int	uid;																				//UID 
		char					tid[Cmd::UserServer::SEQ_MAX_LENGTH +1];		//交易序列号 
		int						at;																					//交易类型 
		int						subat;																			//子类型
		unsigned int	point;																			//扣费点数	
		int						result;																			//交易结果，1成功，0失败
		int 					balance;																		//余额
		int 					bonus;																			//积分
		int	 					hadfilled;																	//曾经充值的标志,1=曾经充值,0=没有
		time_t				timeout;																		//超时时间戳
		char					cardid[Cmd::UserServer::CARDID_LENGTH +1];	//充值卡号
		char					ip[MAX_IP_LENGTH];       										//ip地址
		char					remark[Cmd::UserServer::REMARK_LENGTH +1];	//备注
	};

	/**
	 * \brief 计费回调函数接口定义
	 * 在写这些回调函数的时候需要保证是非阻塞模式，<br>
	 * 如果这些函数组塞太久会造成对其他服务的组塞
	 */
	struct BillCallback
	{
		/**
		 * \brief 计费回调接口函数
		 * 计费，点数换金币
		 * 游戏区中本地换取是否成功，如写数据库等
		 * 冲值
		 * 查询余额、消费纪录等信息
		 * \param uid 账号编号
		 * \return 计费是否成功
		 */
		bool (*action)(const BillData *bd);
	};

	/**
	 * \brief 初始化计费客户端模块
	 * \param confile 计费服务器列表文件名
	 * \param tradelog 交易日志文件名
	 * \param bc 初始的回调函数
	 * \return 初始化是否成功
	 */
	extern bool Bill_init(const char *confile, const char *tradelog, struct BillCallback *bc);
	/**
	 * \brief 回收释放计费客户端模块的资源
	 * 关闭连接，释放线程等操作
	 */
	extern void Bill_final();
	/**
	 * \brief 时间回调函数
	 * 主要处理客户端连接断线重连
	 */
	extern void Bill_timeAction();
	/**
	 * \brief 计费
	 * 点数换金币，冲值，查询等
	 * \param uid 账号编号
	 * \return 计费是否成功，如果返回true，则bd->tid是这次交易的唯一序列号
	 */
	extern bool Bill_action(BillData *bd);

#ifdef	__cplusplus
}
#endif

#endif

