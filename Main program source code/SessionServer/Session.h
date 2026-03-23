#ifndef _SESSION_H_
#define _SESSION_H_

#include "zUser.h"
#include <map>
#include <vector>
#include "zSceneManager.h"
#include "SessionCommand.h"
#include "SessionTask.h"
#include "CRelationManager.h"
#include "zTime.h"
#include <set>

/**
 * \brief 会话类
 * 用户会话和场景会话的基类
 *
 */
class Session:private zNoncopyable
{

	private:
		///创建时间
		time_t createtime;
		///该会话的连接
		SessionTask *task;

	protected:

		/**
		 * \brief 构造函数
		 * \param task 该会话的连接
		 */
		Session(SessionTask *task)
		{
			createtime=time(NULL);
			this->task=task;
		}

	public:
		///请求领养过的人
		DWORD reqAdopter;

		/**
		 * \brief 得到该会话的连接
		 * \return 该会话的连接
		 */
		SessionTask *  getTask() const
		{
			return task;
		}
		
		/**
		 * \brief 向对方发送消息
		 * \param pstrCmd 要发送的消息
		 * \param nCmdLen 消息长度
		 * \return 发送是否成功
		 */
		bool sendCmd(const void *pstrCmd, const int nCmdLen) const
		{
			if (task)
				return task->sendCmd(pstrCmd, nCmdLen);
			else
				return false;
		}

};

/**
 * \brief 场景会话
 *
 */
class SceneSession:public zScene,public Session
{

	public:

		///当前地图允许的最低玩家等级为0表示不限制
		BYTE level;
		/**
		 * \brief 构造函数
		 */
		SceneSession::SceneSession(SessionTask *task):zScene(),Session(task)
		{
		}

		/**
		 * \brief 通过消息注册一个地图
		 * \param reginfo 地图注册消息
		 * \return 是否注册成功
		 */
		bool SceneSession::reg(Cmd::Session::t_regScene_SceneSession *reginfo)
		{
			if(reginfo)
			{
				id=reginfo->dwID;
				tempid=reginfo->dwTempID;
				strncpy(name,(char *)reginfo->byName,MAX_NAMESIZE);
				file = reginfo->fileName;
				level = reginfo->byLevel;
				return true;
			}
			else
				return false;
		}

		///对应的地图文件名
		std::string file;
};

/**
 * \brief 用户会话类
 *
 */
class UserSession:public zUser,public Session
{

	public:
		//std::set<DWORD> cartoonList;
		//std::set<DWORD> adoptList;

		char autoReply[MAX_CHATINFO];//自动回复

		//测试
		static DWORD user_count;

		//国家排序
		static std::map<DWORD , DWORD> country_map;

		///帐号id
		DWORD accid;
		///帮会id
		DWORD unionid;
		///国家id
		DWORD country;
		/// 国家名称
		BYTE countryName[MAX_NAMESIZE+1];
		///家族id
		DWORD septid;
		///门派id
		DWORD schoolid;
		///临时id
		DWORD teamid;
		///等级
		WORD  level;
		///职业
		WORD  occupation;
		/// 在线时间
		zRTime regTime;
		///头像
		DWORD  face;
		
		// 功勋值
		DWORD dwExploit;

		// 文采值
		DWORD dwGrace;

		// 人物当前经验
		QWORD qwExp;

		///系统设置信息
		BYTE sysSetting[20];//系统设置

		///所在的场景会话
		SceneSession *scene;
		///社会关系处理器
		CRelationManager relationManager;

		///下次放烟火的时间
		zRTime nextBlessTime;

		//家族经验
		WORD septExp;


		UserSession(SessionTask *task);
		~UserSession();

		/**
		 * \brief 根据注册一个玩家
		 * \param reginfo 存放玩家信息的消息
		 * \return 是否注册成功
		 */
		bool reg(Cmd::Session::t_regUser_GateSession *reginfo)
		{
			if(reginfo)
			{
				accid=reginfo->accid;
				id=reginfo->dwID;
				tempid=reginfo->dwTempID;
				//TODO septid 初始化
				level=reginfo->wdLevel;
				occupation=reginfo->wdOccupation;
				country = reginfo->wdCountry;
				std::map<DWORD , DWORD>::iterator iter = country_map.find(country);
				if(iter == country_map.end())
				{
					country_map[country] = 0;
				}
				country_map[country]++;
				strncpy(name,(char *)reginfo->byName,MAX_NAMESIZE);
				strncpy((char*)countryName, (char*)reginfo->byCountryName, MAX_NAMESIZE);
				relationManager.setUser(this);
				return true;
			}
			else
				return false;
		}
		static void getCountryUser(std::vector<std::pair<DWORD,DWORD> > &v)
		{
			std::map<DWORD , DWORD>::iterator iter;
			for( iter = country_map.begin() ; iter != country_map.end() ; iter ++)
			{
				v.push_back( std::make_pair(iter->first,iter->second) );
			}
		}

		/**
		 * \brief 设置社会关系信息
		 * \param regsuccess 注册成功的消息
		 * \return 
		 */
		bool setRelationData(const Cmd::Session::t_regUserSuccess_SceneSession *regsuccess)
		{
			if(regsuccess)
			{
				unionid	= regsuccess->dwUnionID;
				country = regsuccess->dwCountryID;
				septid = regsuccess->dwSeptID;
				schoolid = regsuccess->dwSchoolID;
				return true;
			}
			else
				return false;
		}

		/**
		  * \brief 更新配偶信息到场景
		  *
		  */
		void updateConsort();
		
		/**
		  * \brief 更新配偶信息到场景
		  *
		  */
		void updateCountryStar(); 

		bool checkChatCmd(const Cmd::stNullUserCmd *, const unsigned int nCmdLen) const;
		void sendCmdToMe(const void *pstrCmd, const unsigned int nCmdLen) const;
		void sendSysChat(int type, const char *pattern, ...) const;
		void sendGmChat(int type, const char* pattern, ...) const;
		void setFriendDegree(Cmd::Session::t_CountFriendDegree_SceneSession *);
		void sendFriendDegree(Cmd::Session::t_RequestFriendDegree_SceneSession *);
		bool forwardScene(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen);
};

#endif
