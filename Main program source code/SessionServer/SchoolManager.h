/**
 * \file
 * \version  $Id: SchoolManager.h  $
 * \author  
 * \date 
 * \brief 定义师门门派管理器
 *
 */

#ifndef __CLASS_SCHOOLMANAGER__
#define __CLASS_SCHOOLMANAGER__

#include <vector>
#include <list>
#include "zRWLock.h"
#include "zMisc.h"
#include "zType.h"
#include "zEntry.h"
#include "zEntryManager.h"
#include "zSocket.h"
#include "Command.h"
#include "SessionCommand.h"
#include "zUser.h"
#include "School.h"

using namespace School;

class UserSession;
class CSchool;
class CSchoolMember;

/**
 * \brief 操作成员列表的类
 *
 */
class CSchoolMemberListCallback
{
private:
	/**
	 * \brief 受影响的成员列表
	 */
	std::list<CSchoolMember * > memberList;
public:

	/**
	 * \brief 清除无效节点关系
	 * \author fqnewman
	 */
	void clearInValidNodeRelation();

	/**
	 * \brief 遍历
	 * \param member 当前节点
	 * \author fqnewman
	 */
	void exec(CSchoolMember *member);

	/**
	 * \brief 发送通知给受影响的成员
	 * \author fqnewman
	 */
	void sendNotifyToMember();
};

/**
 * \brief 向成员发送其他成员信息的回调
 *
 */
class CSendSchoolCallback
{
private:
	///成员列表
	std::list<struct Cmd::stSchoolMember> memberList;
public:
	void exec(CSchoolMember *member, const BYTE tag);
	void sendListToMember(CSchoolMember *member);
};

/**
 * \brief 门派管理器
 *
 */
class CSchoolM : public zEntryManager<zEntryID,zEntryName>
{
private:
		/// 成员索引表
		std::map<std::string, CSchoolMember*> memberIndex;

		/// 类型定义
		typedef std::map<std::string, CSchoolMember*>::value_type memberIndexValueType;

		/// 唯一实例
		static CSchoolM * sm;
		///读写锁
		zRWLock rwlock;
		inline std::map<std::string, CSchoolMember *>::iterator  findMemberIndex(const char *pName);

public:
		
		/**
		 * \brief 析构函数
		 * \author fqnewman 
		 */
		~CSchoolM();
		
		/**
		 * \brief 初始化师门门派管理器容器
		 * \author fqnewman 
		 * \return true 为成功，false为失败
		 */
		bool init();

		template <class YourEntry>
		bool execEveryOne(execEntry<YourEntry> &exec)
		{
			rwlock.rdlock();
			bool ret=execEveryEntry<>(exec);
			rwlock.unlock();
			return ret;
		}

		/**
		 * \brief 获取唯一对象实例
		 * \author fqnewman
		 * \return 唯一对象实例
		 */
		static CSchoolM &getMe();

		/**
		 * \brief 摧毁师门门派管理器容器，做善后工作
		 * \author fqnewman
		 */
		static void destroyMe();

		/**
		 * \brief 角色上线通知，管理器中对应的节点进行上线处理关联UserSession并且对左邻右舍进行通知
		 * \param pUser 上线角色的会话对象
		 * \author fqnewman
		 */
		void userOnline(UserSession *pUser);

		/**
		 * \brief 角色下线通知，管理器中对应的节点进行下线处理，并且对左邻右舍进行通知
		 * \param pUser 下线的角色
		 * \author fqnewman
		 */
		void userOffline(UserSession * pUser);

		/**
		 * \brief 在索引表中填加指定角色的索引
		 * \param name 指定角色的名称
		 * \param member 角色的节点对象
		 * \author fqnewman
		 * \return 失败返回false,成功返回true
		 */
		bool addMemberIndex(const char *name, CSchoolMember *member);

		/**
		 * \brief 在索引表中删除指定的角色的索引
		 * \param name 指定角色的名称
		 * \author fqnewman
		 * \return 失败返回false,成功返回true
		 */
		bool removeMemberIndex(const char *name);

		/**
		 * \brief 处理客户端送过来的消息（也有可能是其他服务器转发过来的客户端消息）
		 * \param pUser 发送消息的角色
		 * \param ptNullCmd 消息结构
		 * \param cmdLen 消息长度
		 * \author fqnewman
		 * \return 失败返回-1, 没有物品被创建返回0, 包裹满返回1, 成功添加所有创建的物品返回2
		 */
		bool processUserMessage(UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen);

		/**
		 * \brief 处理场景服务器送过来的消息
		 * \param ptNullCmd 消息结构
		 * \param cmdLen 消息长度
		 * \author fqnewman
		 * \return 失败返回-1, 没有物品被创建返回0, 包裹满返回1, 成功添加所有创建的物品返回2
		 */
		bool processSceneMessage(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen);

		/**
		 * \brief 处理家族门派聊天消息转发
		 * \param pUser 聊天消息发送者
		 * \param rev 消息结构体
		 * \param cmdLen 消息长度
		 * \author fqnewman
		 */
		void sendSchoolChatMessages(const UserSession *pUser, const Cmd::stChannelChatUserCmd *rev, const unsigned int cmdLen);

		/**
		 * \brief 处理家族门派聊天消息转发
		 * \param pUser 聊天消息发送者
		 * \param rev 消息结构体
		 * \param cmdLen 消息长度
		 * \author fqnewman
		 */
		void sendSchoolPrivateChatMessages(const UserSession *pUser, const Cmd::stChannelChatUserCmd *rev, const unsigned int cmdLen);

		/**
		 * \brief 判断用户的徒弟是否满，
		 * \param master 被检查的角色名称
		 * \param schoolName 门派名称，如果此人的徒弟未满且此人是有门派的那么门派名称通过这里返回
		 * \author fqnewman
		 * \return 返回true表示master的徒弟没收满， 返回false表示master的徒弟已经招收满了
		 */
		bool getUserPrenticeInfo(const char *master, char *schoolName);

		/**
		 * \brief 判断某个人是否为干净的，意思是他没有师傅和徒弟的关系，即他不在此管理器中
		 * \param name 被检查的角色的名称
		 * \author fqnewman
		 * \return true为干净的，false 为已经有师或徒弟关系
		 */
		bool isClean(const char *name);

		/**
		 * \brief 为 master 增加一个徒弟 prentice，
		 *
		 * 在函数中会根据两人的情况进行具体的操作，如果不成功会给对应角色发送消息
		 *
		 * \param master 师傅
		 * \param prentice 徒弟
		 * \author fqnewman
		 */
		void addMember(UserSession *master, UserSession *prentice);

		/**
		 * \brief master 开除一个徒弟 prentice，
		 *
		 * 在函数中会根据两人的情况进行具体的操作，如果不成功会给对应角色发送消息
		 *
		 * \param master 师傅
		 * \param prentice 徒弟
		 * \author fqnewman
		 */
		void frieMember(UserSession *master, const char *prentice);

		/**
		 * \brief 从师门从开除一个角色
		 *
		 * \param roleName 角色名称
		 * \param find 是否只是进行判断
		 *
		 * \return 如果该角色不在任何师门中，则返回2
		 *         如果该角色是族长，则返回0
		 *         如果该角色是族员，并删除成功，则返回1
		 *
		 * \author zhongjunwei
		 */
		int fireSchoolMember(const char* roleName, bool find);

		/**
		 * \brief 初始化的时候根据数据库中load的记录创建一个门派管理器
		 * \param info 门派的信息结构
		 * \author fqnewman
		 * \return 成功返回true,失败返回false
		 */
		bool createSchoolFromDB(const stSchoolInfo &info);

		/**
		 * \brief 从数据库中加载门派的信息建立门派管理器
		 * \author fqnewman
		 * \return true为成功， false为失败
		 */
		bool loadSchoolFromDB();

		/**
		 * \brief 从数据库中加载所有门派的成员
		 * \author fqnewman
		 * \return true为成功，false为失败
		 */
		bool loadSchoolMemberFromDB();

		/**
		 * \brief 检查指定的角色是否具备创建门派的条件
		 * \param pUser 被检查的角色
		 * \author fqnewman
		 * \return true为具备，false为不具备
		 */
		bool checkSchoolCreateCondition(const UserSession *pUser);

		/**
		 * \brief 创建一个新的门派
		 * \param userName 师尊的名字
		 * \param schoolName 门派的名称
		 * \author fqnewman
		 * \return true 为创建成功， false 为创建失败
		 */
		bool createNewSchool(const char *userName, const char *schoolName);

		/**
		 * \brief 响应公告功能设置公告或者读取公告
		 * \param pUser 公告者
		 * \param rev 公告消息
		 * \author fqnewman
		 */
		void processBulletin(const UserSession *pUser, const Cmd::stSchoolBulletinCmd *rev);

		/**
		 * \brief 获取名称所指定的节点对象
		 * \param pName 名称
		 * \author fqnewman
		 * \return 获取指定的节点对象，如果不存在该名称的对象则返回NULL
		 */
		CSchoolMember *getMember(const char *pName);

		/**
		 * \brief 处理成员退出门派或者师门
		 * \param pUser 退出者的UserSession对象
		 * \author fqnewman
		 */
		void processLeaveGroup(UserSession *pUser);

		/**
		 * \brief 处理成员退出门派或者师门
		 * \param roleName 角色名称
		 * \author zhongjunwei
		 */
		void processLeaveGroupDirect(const char* roleName);

		/**
		 * \brief 解散门派，如果条件符合pName所领导的门派将被解散，所有的人员关系回复到原始的师门关系
		 * \param pName 解散门派者的名字
		 * \author fqnewman
		 * \return true 解散成功 false 解散失败
		 */
		bool destroySchool(const char *pName);

		/**
		 * \brief 根据ID获得对应的管理器
		 * \param id 管理器ID
		 * \author fqnewman
		 * \return 如果成功返回管理器对象，失败返回NULL
		 */
		CSchool *getSchool(DWORD id);

		/**
		 * \brief 设置指定用户的级别
		 * \param pName 用户名称
		 * \param level 新级别
		 * \author fqnewman
		 */
		void setUserLevel(const char *pName, const WORD &level);

		CSchool * getSchoolByName( const char * name);
		
};

class CSchool : public zEntryManager<zEntryID, zEntryName>, public zEntry
{
private:
		/**
		 * \师尊的ID
		 * \author fqnewman
		 */
		DWORD dwMasterSerialID;

		/**
		 * \公告
		 * \author fqnewman
		 */
		std::string bulletin;

		/**
		 * \brief 对象有效标志，一般判断依据是是否还存在数据库中
		 * \author fqnewman
		 */
		bool destroy;

		/**
		 * \brief 管理器读写锁
		 * \author fqnewman
		 */
		zRWLock rwlock;

public:
/*
		bool getUniqeID(DWORD &tempid){return true;};
		void putUniqeID(const DWORD &tempid){};
*/

		/**
		 * \brief 遍历模板定义
		 * \author fqnewman
		 */
		template <class YourEntry>
                bool execEveryOne(execEntry<YourEntry> &exec)
		{
			rwlock.rdlock();
			bool ret=execEveryEntry<>(exec);
			rwlock.unlock();
			return ret;
         }
                
         template <class YourEntry>
         void removeOne_if(removeEntry_Pred<YourEntry> &pred)
         {
                rwlock.wrlock();
                removeEntry_if<>(pred);
                rwlock.unlock();
         }


		/**
		 * \brief 构造函数
		 * \author fqnewman
		 */
		CSchool();

		/**
		 * \brief 析构函数
		 * \author fqnewman
		 */
		~CSchool();



		/**
		 * \brief 获取门派名称
		 * \author fqnewman
		 * \return 门派名称
		 */
		char *getSchoolName();

		/**
		 * \brief 设置门派名称未加锁
		 * \param pName 门派名称
		 * \author fqnewman
		 */
		void setSchoolName(const char *pName);

		/**
		 * \brief 设置师尊的节点ID
		 * \param id 师尊的节点ID
		 * \author fqnewman
		 */
		void setMasterSerialID(const DWORD &id);

		/**
		 * \brief 添加一个师门的根节点
		 * \param master 角色对象
		 * \author fqnewman
		 * \return 新加的节点对象，失败返回NULL
		 */
		CSchoolMember *  addTeacher(UserSession *master);

		/**
		 * \brief 向从管理器中增加指定的节点，并将其加入索引表
		 * \param member 指定的节点
		 * \author fqnewman
		 */
		bool addMember(CSchoolMember *member);

		/**
		 * \brief 从管理器中删除指定的节点，并将其从索引表中去除
		 * \param member 指定的节点
		 * \author fqnewman
		 */
		void removeMember(CSchoolMember *member);
	
		/**
		 * \brief 将管理器初始化成师门关系管理器
		 * \author fqnewman
		 * \return 返回NULL表示添加徒弟失败，否则返回徒弟的节点对象
		 */
		void initToNoneSchool();

		/**
		 * \brief 初始化管理器，根据信息结构，从数据库初始化的时候用
		 * \param info 管理器信息结构
		 * \author fqnewman
		 */
		void initSchool(const stSchoolInfo &info);

		/**
		 * \brief 从数据库加载本管理器内的所有节点信息
		 * \author fqnewman
		 * \return 返回true 表示加载成功，返回false表示加载失败
		 */
		bool loadSchoolMemberFromDB();

		/**
		 * \brief 更新门派的信息到数据库
		 * \author fqnewman
		 * \return 返回true 表示加载成功，返回false表示加载失败
		 */
		bool updateSchoolInDB();

		/**
		 * \brief 增加一个节点，数据库初始化的时候调用本方法，通常调用本方法的时候节点数据都是有序的，节点序列号由小到大
		 * \param info 节点信息
		 * \author fqnewman
		 */
		bool addNode(const stSchoolMemberInfo &info);

		/**
		 * \brief 发送命令到整个School里的角色
		 * \param ptNullCmd 消息结构
		 * \param cmdLen 消息长度
		 * \author fqnewman
		 */
		void sendCmdToSchool(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen);

		/**
		 * \brief 插入门派记录到数据库中
		 * \author fqnewman
		 * \return true 插入成功 false 插入失败
		 */
		bool insertSchoolToDB();

		/**
		 * \brief 将本节点管理器从数据库中删除
		 * \author fqnewman
		 * \return true 删除成功 false 删除失败
		 */
		bool deleteSchoolFromDB();

		/**
		 * \brief userName创建了一个新的帮会pSchool将门派中的所有userName的徒子徒孙归到新School管理起pSchool中，这个方法只适用于门派管理器
		 * \param userName 新的师尊的名字
		 * \param pSchool 新的管理器对象指针
		 * \author fqnewman
		 * \return true 成功 false 失败
		 */
		bool moveMemberToNewSchool(const char *userName, CSchool *pSchool);


		/**
		 * \brief 判断一个角色是不是该师门成员
		 * 
		 * \param memberName 玩家名字
		 * \author zhongjunwei
		 *
		 * \return true 是师门成员，FALSE不是师门成员
		 */
		bool	isMember(const char* memberName);
		
		/**
		 * \brief 给所有师门成员发送转发到场景的命令
		 *
		 * \param ptEnterWarCmd 待转发的命令
		 * \param cmdLen 命令长度
		 * \author zhongjunwei
		 *
		 */
		void sendCmdToAllMemberScene(Cmd::Session::t_enterWar_SceneSession* ptEnterWarCmd, const unsigned int cmdLen);

		/**
		 * \brief 直接添加一个节点，该节点已经存在于索引表之中了
		 * \param member 被添加的节点
		 * \author fqnewman
		 */
		void directAdddMember(CSchoolMember *member);

		/**
		 * \brief 直接删除一个节点，不不在索引表中删除该节点
		 * \param member 被添加的节点
		 * \author fqnewman
		 */
		void directRemoveMember(CSchoolMember *member);

		/**
		 * \brief 直接删除一个节点，不不在索引表中删除该节点
		 * \author fqnewman
		 * \return 正常返回门派的节点对象，异常返回NULL
		 */
		CSchoolMember * getMasterNode();

		/**
		 * \brief 设置公告
		 * \param buf 公告内容
		 * \author fqnewman
		 */
		void setBulletin(const char *buf);

		/**
		 * \brief 获得公告
		 * \author fqnewman
		 * \return 公告内容或者NULL
		 */
		const char * getBulletin();

		/**
		 * \brief 获得指定的成员
		 * \param pName 指定的名称
		 * \author fqnewman
		 * \return 找到的成员对象或者NULL
		 */
		CSchoolMember *getMember(const char *pName);

		/**
		 * \brief 处理指定成员的关系断绝（在门派中是完全退出，在师门中是断绝与师傅的关系
		 * \param member 要退出成员的节点对象
		 * \param deleteTeacher是否删除师傅节点，此标志用于防止在函数内删除调用对象，此标志在门派中不起作用，意思是不能在门派成员对象中调用此函数
		 * \author fqnewman
		 * \return true 为成功 false为失败
		 */
		bool processLeaveSchool(CSchoolMember * member, bool deleteTeacher = true);

		/**
		 * \brief 将本门派解散所有成员关系转移到师门管理器管理
		 * \author fqnewman
		 * \return true 为成功 false为失败
		 */
		bool moveMemberToTeacherGroup();

		void notifyWarResult(const char* msg,...);

		DWORD getID(){return id;}
};

class CSchoolMember : public zEntry
{
private:

		/**
		 * \brief 对象有效标志，一般判断依据是是否还存在数据库中
		 * \author fqnewman
		 */
		bool destroy;

		/**
		 * \brief 当前角色的UserSession对象
		 * \author fqnewman
		 */
		DWORD dwCharID;

		/**
		 * \brief 当前角色的职业
		 * \author fqnewman
		 */
		DWORD wdOccupation;

		/**
		 * \brief 当前角色的UserSession对象
		 * \author fqnewman
		 */
		UserSession		*user;

		/**
		 * \brief 师傅节点
		 * \author fqnewman
		 */
		CSchoolMember	*preLevelNode;

		/**
		 * \brief 排序
		 * \author fqnewman
		 */
		struct ltword
		{
			bool operator()(const DWORD s1, const DWORD s2) const
			{
				return s1<s2;
			}
		};
		/**
		 * \brief 徒弟列表
		 * \author fqnewman
		 */
		std::map<DWORD, CSchoolMember*,ltword> prenticeList;

		/// 类型定义
		typedef std::map<DWORD, CSchoolMember*,ltword>::value_type prenticeListValueType;

		/**
		 * \brief 所在的门派管理器（无门派的归结为一个门派）
		 * \author fqnewman
		 */
		CSchool			*school;

		/**
		 * \brief 师傅节点的ID
		 * \author fqnewman
		 */
		DWORD dwMasterID;

		/**
		 * \brief 前一个节点的序列号
		 * \author fqnewman
		 */
		DWORD dwPreSerialID;

		/**
		 * \brief 角色当前级别
		 * \author fqnewman
		 */
		DWORD wdLevel;

		/**
		 * \brief 拜师时间
		 * \author fqnewman
		 */
		DWORD dwJoinTime;

		/**
		 * \brief 与师傅间的友好度
		 * \author fqnewman
		 */
		WORD  wdDegree;

		/**
		 * \brief 最后组队时间
		 * \author fqnewman
		 */
		DWORD dwLastTime;

		/**
		 * \brief 所属门派ID如果无门派为0
		 * \author fqnewman
		 */
		DWORD dwSchoolID;

		/**
		 * \brief 节点有效标记，1为有效，0为无效，无效的节点只存在于门派之中起上下连接之用
		 * \author fqnewman
		 */
		BYTE  byTag;
		
	
		/**
		 * \brief 本节点的读写锁
		 * \author fqnewman
		 */
		zRWLock rwlock;

public:
		/**
		 * \brief 将本节点的信息插入数据库，节点初创的时候用
		 * \author fqnewman
		 * \return true为成功，false 为失败
		 */
		bool insertRecord(); // 将自己插入数据库

		/**
		 * \brief 更新数据库中的节点信息
		 * \param locked 是否已经加锁 默认为否
		 * \author fqnewman
		 * \return true为成功，false 为失败
		 */
		bool updateRecord(bool locked = false); // 将自己插入数据库

		/**
		 * \brief 将自己的数据库记录删除
		 * \author fqnewman
		 * \return true为成功，false 为失败
		 */
		bool deleteRecord();

		/**
		 * \brief 构造函数，对节点进行部分初始化
		 * \param pSchool 本节点所在的管理器对象
		 * \param pUser 节点的拥有者
		 * \author fqnewman
		 */
		CSchoolMember(CSchool *pSchool, UserSession *pUser);

		/**
		 * \brief 析构函数
		 * \author fqnewman
		 */
		~CSchoolMember();

		/**
		 * \brief 获取节点对应的角色ID
		 * \author fqnewman
		 * \return 角色ID
		 */
		DWORD getCharID();

		/**
		 * \brief 获取节点序列号
		 * \author fqnewman
		 * \return 节点序列号
		 */
		DWORD getSerialID();

		/**
		 * \brief 获取节点所在门派的ID
		 * \author fqnewman
		 * \return 门派ID
		 */
		DWORD getSchoolID();

		/**
		 * \brief 获取友好度
		 * \author fqnewman
		 * \return 友好度
		 */
		WORD getDegree();

		/**
		 * \brief 设置友好度
		 * \param degree 友好度
		 * \author fqnewman
		 */
		void setDegree(const WORD &degree);

		/**
		 * \brief 设置最后组队时间
		 * \param lasttime 最后组队时间
		 * \author fqnewman
		 */
		void setLastTime(const DWORD &lasttime);

		/**
		 * \brief 获取本节点的门派名称
		 * \author fqnewman
		 * \return 门派名称
		 */
		char *getSchoolName();

		/**
		 * \brief 获得当前节点的徒弟记数（有效节点数）
		 * \author fqnewman
		 * \return 当前节点的徒弟记数
		 */
		BYTE getPrenticeCount();

		/**
		 * \brief 获得拜师时间
		 * \author fqnewman
		 * \return 拜师时间
		 */
		DWORD getJoinTime();

		/**
		 * \brief 获得最后组队时间
		 * \author fqnewman
		 * \return 最后组队时间
		 */
		DWORD getLastTime();

		/**
		 * \brief 获得角色级别
		 * \author fqnewman
		 * \return 角色级别
		 */
		WORD getLevel();
		
		/**
		 * \brief 为本节点添加一个徒弟
		 * \param pUser 徒弟的UserSession对象
		 * \author fqnewman
		 * \return 返回NULL表示添加徒弟失败，否则返回徒弟的节点对象
		 */
		CSchoolMember *addPrentice(UserSession *pUser);

		/**
		 * \brief 添加一个纯粹的师傅节点，只适用于那些从未招收过徒弟的人第一次招收徒弟时在管理器中初始化自己
		 * \author fqnewman
		 */
		void initRootMember();

		/**
		 * \brief 对徒弟节点进行初始化
		 * \param master 师傅节点的节点对象
		 * \author fqnewman
		 */
		void initGeneralMember(CSchoolMember * master);

		/**
		 * \brief 将无效节点初始化成有效节点
		 * \param master 师傅节点的节点对象
		 * \author fqnewman
		 */
		void initInValidNode(CSchoolMember * master);
		/**
		 * \brief 判断节点是否还存在关联的上级节点或者下级节点
		 * \author fqnewman
		 * \return true为本节点已经与其他节点断绝一切关系，false 为本节点与其他节点还存在关系
		 */
		bool isClean();

		/**
		 * \brief 获得本节点所在的管理器对象
		 * \author fqnewman
		 * \return 成功返回管理器对象，失败返回NULL
		 */
		CSchool * getSchool();

		/**
		 * \brief 将自己从门派中删除掉，并删除对应的数据库记录
		 * \author fqnewman
		 */
		void deleteMe();

		/**
		 * \brief 上线处理
		 * \author fqnewman
		 */
		void online(UserSession *pUser);

		/**
		 * \brief 下线处理
		 * \author fqnewman
		 */
		void offline();


		/**
		 * \brief 找到排位在member之前的师兄
		 * \param member 被检查的节点
		 * \param name 师兄的名称
		 * \author fqnewman
		 */
		void getMyBigBrother(CSchoolMember *member, char *name);

		/**
		 * \brief 通知周遍成员我的加入并发送初始化消息给自己
		 * \author fqnewman
		 */
		void notifyNewMemberAdd();

		/**
		 * \brief 发送个人的师徒门派信息给客户端进行初始化
		 * \param callback 门派信息的回调对象，可以在外面事先获得门派信息适合对门派多数人广播门派信息默认为空
		 * \author fqnewman
		 */
		void sendInfomationToMe(CSendSchoolCallback *callback = NULL);

		/**
		 * \brief 徒弟请求发送师傅和兄弟的信息
		 * \param count 计数器，累计当前已经有多少信息被放如buf
		 * \param point 信息存放buf的指针
		 * \param me 请求信息的角色的节点对象
		 * \author fqnewman
		 * \return true为干净的，false 为已经有师或徒弟关系
		 */
		void prenticeRequestMemberInfo(unsigned int &count, Cmd::stTeacherMember *point, CSchoolMember *me);

		/**
		 * \brief 根据数据库记录初始化节点
		 * \param info 数据库记录信息结构
		 * \author fqnewman
		 */
		void initByDBRecord(const stSchoolMemberInfo &info);

		/**
		 * \brief 判断节点是否存在师傅,根据info属性内容判断
		 * \author fqnewman
		 * \return true为有师傅，false 为没有师傅
		 */
		bool haveTeacher();

		/**
		 * \brief 判断节点是否存在前一节点,在门派中一个人可能没有师傅但是会有前一节点
		 * \author fqnewman
		 * \return true为有节点，false 当前节点为根节点（师尊）
		 */
		bool havePreNode();

		/**
		 * \brief 添加一个下一级节点，不会有通知动作，用在数据库初始化的时候
		 * \param member 下一级节点
		 * \author fqnewman
		 */
		bool addNextLevelNode(CSchoolMember *member);

		/**
		 * \brief 设置本节点的上层节点
		 * \param pPreNode 上层节点
		 * \author fqnewman
		 */
		void setPreLevelNode(CSchoolMember *pPreNode);

		/**
		 * \brief 开除指定名字的徒弟
		 * \param PrenticeName 被开除人的名字
		 * \author fqnewman
		 * \return true 为开除成功， false为没有此人
		 */
		bool firePrentice(const char *PrenticeName);

		/**
		  * \brief 开除所有徒弟
		  *
		  * \author zhongjunwei
		  */
		bool fireAllPrentice();

		/**
		 * \brief 核对本节点是不是名称对应的节点
		 * \param pName 指定的名称
		 * \author fqnewman
		 * \return true 这个节点就是要找的节点， false 节点不叫这个名字
		 */
		bool isMe(const char *pName);

		/**
		 * \brief 节点对应角色是否在线
		 * \author fqnewman
		 * \return true 角色在线， false 角色不在线
		 */
		bool isOnline();

		/**
		 * \brief 判断节点是否是有效的，判断依据为tag是否为1
		 * \author fqnewman
		 * \return true 有效的， false 无效的
		 */
		bool isValid();

		/**
		 * \brief 获得节点在树中的度
		 * \param layer 度
		 * \author fqnewman
		 */
		void getMyLayer(DWORD &layer);

		/**
		 * \brief 通知该节点及其所有徒弟,更新自己的师门列表
		 * \author fqnewman
		 */
		void notifyTeacherGroup();

		/**
		 * \brief 发送命令到节点对应的角色
		 * \param ptNullCmd 消息结构
		 * \param cmdLen 消息长度
		 * \author fqnewman
		 */
       void sendCmdToMe(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen);

		/**
		 * \brief 发送聊天消息到节点对应的对象
		 * \param type 系统消息的类型 
		 * \param pattern 消息
		 * \param ... 参数
		 * \author fqnewman
		 */
		void sendSysChat(int type, const char *pattern, ...);

		/**
		 * \brief 清除师徒关系，preLevelNode指针置空，师傅信息清0
		 * \author fqnewman
		 */
		void clearTeacherRelation();

		/**
		 * \brief 检查此人的徒弟徒孙的数目和级别是否达到成立门派的条件
		 * \author fqnewman
		 * \return true 为条件满足 false 为条件不满足
		 */
		bool checkSchoolCreateCondition();

		/**
		 * \brief 检查节点自己和徒弟的条件是否达到师傅成立门派的要求
		 * \author fqnewman
		 * \return true 为条件满足 false 为条件不满足
		 */
		bool checkMeAndPrenticeNumberAndLevel();

		/**
		 * \brief 将节点自己及其所有的徒弟都挪到管理器pSchool之中
		 * \param pSchool 节点移动的目标管理器
		 * \param memberSet 成员处理回调对象，如果不需要对被移动成员进行处理可以设置为NULL，默认为NULL
		 * \author fqnewman
		 */
		void moveAllToSchool(CSchool *pSchool, CSchoolMemberListCallback *memberSet = NULL);

		/**
		 * \brief 通知本派系所有的节点重新发送初始化消息给客户端
		 * \param callback 门派信息回调对象，用来发送门派的组织结构初始化信息给客户端
		 * \author fqnewman
		 */
		void notifyAllReSendInitData(CSendSchoolCallback *callback);

		/**
		 * \brief 获得门派的所有成员列表（有序的）
		 * \param callback 回调类
		 * \author fqnewman
		 */
		void getSchoolTree(CSendSchoolCallback &callback);

		/**
		 * \brief 获得门派的所有成员列表（有序的）
		 * \param level 当前层
		 * \param tgLevel 此次调用的目标层
		 * \param condition 到底标志，如果已经到了树的底部那么就返回 false
		 * \param callback 回调对象
		 * \param tag 换行换层标志 参见enum SCHOOL_LAYER
		 * \author fqnewman
		 */
		void schoolTreeCallback(DWORD level, DWORD tgLevel, bool &condition, CSendSchoolCallback &callback, BYTE &tag);

		/**
		 * \brief 将公告内容发送给节点对应的角色
		 * \author fqnewman
		 */
		void sendBulletinToMe();

		/**
		 * \brief 设置本门派的公告内容，当然节点必须是师尊才行
		 * \param buf 公告内容
		 * \author fqnewman
		 */
		void setBulletin(const char *buf);

		/**
		 * \brief 向节点及其徒弟发送消息
		 * \param ptNullCmd 消息结构
		 * \param cmdLen 消息长度
		 * \param exceptMe 是否包括本节点，默认为包括
		 * \author fqnewman
		 */
		void sendCmdToTeacherGroup(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen, bool exceptMe = false);

		/**
		 * \brief 向团体通知我的在线状态
		 * \param onlineStatus 节点对应角色的在线状态
		 * \author fqnewman
		 */
		void sendOnlineStatusMessage(BYTE onlineStatus);

		/**
		 * \brief 获得上级节点对象指针
		 * \author fqnewman
		 * \return 上级节点的对象，有可能为NULL
		 */
		CSchoolMember * getPreLevelNode();

		/**
		 * \brief 获得师傅节点，上级节点不一定是师傅节点，他们之间可能并不存在关系所以获得师傅节点不能使用getPreLevelNode
		 * \author fqnewman
		 * \return 如果有直接师傅返回师傅节点的对象，否则为NULL
		 */
		CSchoolMember * getTeacher();

		/**
		 * \brief 将节点设置成无效的.
		 * \author fqnewman
		 */
		void setInValid();

		/**
		 * \brief 断绝和师傅之间的关系
		 * \author fqnewman
		 */
		void clearMaster();

		/**
		 * \brief 发送退出门派的消息给客户端
		 * \author fqnewman
		 */
		void sendDestroyNotifyToMe();

		/**
		 * \brief 直接在本节点的下层节点列表中删除指定的节点关系
		 * \param member 关系需要删除的节点对象。
		 * \author fqnewman
		 */
		void directRemovePrentice(const CSchoolMember *member);

		/**
		 * \brief 通知整个门派的客户端删除本节点对应的角色
		 * \author fqnewman
		 */
		void notifySchoolMemberRemove();

		/**
		 * \brief 获得第一个无效的下级节点对象
		 * \author fqnewman
		 * \return 下级无效节点对象，如果没有合适的对象可能会返回NULL
		 */
		CSchoolMember* getFirstInValideNode();

		/**
		 * \brief 设置有效节点关联的UserSession对象
		 * \param pUser 角色对应的UserSession对象
		 * \author fqnewman
		 */
		void setUser(UserSession *pUser);

		/**
		 * \brief 获得本节点的直接下级节点总数不区分有效无效
		 * \author fqnewman
		 * \return 直接下级节点数
		 */
		BYTE getNextLevelNodeCount();

		/**
		 * \brief 清掉本节点与前一节点的关系属性，将相关的指针和属性置空或置0
		 * \author fqnewman
		 */
		void clearPreLevelNode();

		/**
		 * \brief 清掉本节点与后一级节点的关系属性，将后一级节点相关的指针和属性置空或置0并将后级节点列表清空
		 * \author fqnewman
		 */
		void clearAllNextLevelRelation();

		/**
		 * \brief 组队完毕对友好度进行保存
		 * \param rev 友好度结算消息体
		 * \author fqnewman
		 */
		void setFriendDegree(const Cmd::Session::t_CountFriendDegree_SceneSession *rev);

		/**
		 * \brief 转发聊天消息
		 * \param rev 聊天消息结构体
		 * \param cmdLen 消息长度
		 * \author fqnewman
		 */
		void sendChatMessages(const Cmd::stChannelChatUserCmd *rev, const unsigned int cmdLen);

		/**
		 * \brief 转发聊天消息
		 * \author fqnewman
		 * \return 获取角色对应的UserSession 对象如果角色不在线会返回NULL
		 */
		UserSession *getUser();

		/**
		 * \brief 设置级别
		 * \param level 新的角色等级
		 * \author fqnewman
		 */
		void setLevel(const WORD level);

		/**
		 * \brief 获取角色的职业
		 * \author fqnewman
		 * \param 职业
		 */
		WORD getOccupation();

		/**
		 * \brief 查询当前可以进贡的金额
		 * \author zjw
		 * \param 金额
		 */
		DWORD queryBounty();

		/**
		 * \brief 进贡
		 * \author zjw
		 */
		bool putBounty();

		/**
		 * \brief 发送社会关系更新通知给场景
		 * \author fqnewman
		 */
		void sendNotifyToScene();

		/**
		 * \brief 做为师傅累计收到的进贡,单位:文
		 * \author zjw
		 */
		DWORD  master_total;
		
		/**
		 * \brief 做为师傅当前可以领取的进贡金额,单位:文
		 * \author zjw
		 */
		DWORD  master_balance;
		
		/**
		 * \brief 做为徒弟累计进贡的金额,单位:文
		 * \author zjw
		 */
		DWORD  prentice_total;

		/**
		 * \brief 做为徒弟上次进贡时的等级
		 * \author zjw
		 */
		DWORD  prentice_lastlevel;

};

#endif
