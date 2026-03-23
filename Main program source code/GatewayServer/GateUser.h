/**
 * \file
 * \version  $Id: GateUser.h  $
 * \author  
 * \date 
 * \brief 定义网关用户类
 */

#ifndef _GATEUSER_H_
#define _GATEUSER_H_

#include <set>
#include "zUser.h"
#include "Command.h"
#include "RecordCommand.h"
#include "Zebra.h"
#include "zRWLock.h"

class GateUserManager;

/**
 * \brief 角色选择
 *
 */
class GateSelectUserSession:private zNoncopyable
{
	friend class GateUserAccountID;
	protected:
	Cmd::SelectUserInfo userinfo[Cmd::MAX_CHARINFO];

	GateSelectUserSession(DWORD accid)
	{
		this->accid=accid;
		bzero(userinfo,sizeof(userinfo));
	}
	public:
	DWORD accid;
	WORD face;

	void setSelectUserInfo(const Cmd::Record::t_Ret_SelectInfo_GateRecord *ptCmd);

	void putSelectUserInfo(const Cmd::SelectUserInfo &info);

	/**
	 * \brief 删除选择的角色
	 *
	 *
	 * \param charid: 角色id
	 * \return 删除是否成功
	 */
	bool delSelectUserInfo(DWORD charid)
	{
		bool empty=true;
		for(int i = 0; i < Cmd::MAX_CHARINFO; i++)
		{
			if(userinfo[i].id== charid && userinfo[i].id!=0 && userinfo[i].id!=(DWORD)-1)
			{
				bzero(&userinfo[i],sizeof(Cmd::SelectUserInfo));
			}
			if(userinfo[i].id!=0 && userinfo[i].id!=(DWORD)-1)
				empty=false;
		}
		return empty;
	}

	/**
	 * \brief 根据角色序号得到一个角色信息
	 *
	 *
	 * \param num: 角色序号
	 * \return 角色信息
	 */
	Cmd::SelectUserInfo *getSelectUserInfo(WORD num)
	{
		if(num>=Cmd::MAX_CHARINFO)
			return NULL;
		return &userinfo[num];
	}

	/**
	 * \brief 判断角色是否达到最大角色数量
	 *
	 *
	 * \return 角色满返回ture,否则返回false
	 */
	bool charInfoFull()
	{
		bool retval = false;
		for(int i = 0; i < Cmd::MAX_CHARINFO; i++)
		{
			if (userinfo[i].id != 0 && userinfo[i].id != (DWORD)-1)
			{
				retval = true;
			}
		}
		/*
		bool retval = true;
		for(int i = 0; i < Cmd::MAX_CHARINFO; i++)
		{
			if (userinfo[i].id == 0 || userinfo[i].id == (DWORD)-1)
			{
				retval = false;
			}
		}
		// */
		return retval;
	}
};

class GatewayTask;
class SceneClient;
/**
 * \brief 网关用户
 *
 */
class GateUser:public zUser,public GateSelectUserSession
{
	friend class GatewayTask;
	private:

	/// 用户处于退出状态(socket已经断开)
	bool logout;
	GatewayTask *gatewaytask;
	/// 四个状态不能重叠
	enum Systemstate
	{
		SYSTEM_STATE_INITING,		/// 初始状态
		SYSTEM_STATE_CREATING,		/// 创建角色状态
		SYSTEM_STATE_SELECT,		/// 选择角色状态
		SYSTEM_STATE_PLAY,			/// 游戏状态
		SYSTEM_WAIT_STATE_PLAY,		/// 等待游戏状态
		SYSTEM_WAIT_STATE_UNREG		/// 等待退出角色流程
	};
	volatile Systemstate systemstate;

	bool quiz;
	/// 验证码
	char jpegPassport[5];
	/// 黑名单列表 
	std::set<std::string> blacklist; 
	typedef std::set<std::string>::value_type blackListValueType;
	zRWLock rwlock;

	public:
	/// 缓存一下创建角色指令
	Cmd::Record::t_CreateChar_GateRecord createCharCmd;

	bool backSelect;
	SceneClient *scene;
	DWORD sceneTempID;

	DWORD countryID;
	DWORD sceneID;

	GateUser(DWORD accID,GatewayTask *histask);
	~GateUser();

	/**
	 * \brief 初始化状态设置
	 *
	 */
	void initState()
	{
		systemstate = SYSTEM_STATE_INITING;
	}
	/**
	 * \brief 是否完成初始化
	 *
	 *
	 * \return 如果完成返回ture,否则false
	 */
	bool isInitState() const
	{
		return SYSTEM_STATE_INITING == systemstate;
	}
	/**
	 * \brief 创建角色状态
	 *
	 *
	 */
	void createState()
	{
		systemstate = SYSTEM_STATE_CREATING;
	}
	/**
	 * \brief 是否在创建角色状态
	 *
	 *
	 * \return 如果在创建角色状态返回ture,否则返回false
	 */
	bool isCreateState() const
	{
		return SYSTEM_STATE_CREATING == systemstate;
	}
	/**
	 * \brief 设置选择角色状态
	 *
	 *
	 */
	void selectState()
	{
		systemstate=SYSTEM_STATE_SELECT;
	}
	/**
	 * \brief 是否在等待退出角色状态
	 *
	 *
	 * \return 在退出角色状态返回ture,否则返回false
	 */
	bool isWaitUnregState() const
	{
		return SYSTEM_WAIT_STATE_UNREG == systemstate;
	}

	
	/**
	 * \brief 设置退出等待状态
	 *
	 *
	 */
	void waitUnregState()
	{
		systemstate = SYSTEM_WAIT_STATE_UNREG;
	}
	/**
	 * \brief 是否在选择角色状态
	 *
	 *
	 * \return 在选择状态返回ture,否则返回false
	 */
	bool isSelectState() const
	{
		return SYSTEM_STATE_SELECT == systemstate;
	}

	
	void playState(SceneClient *s=NULL , DWORD scene_tempid=0);

	/**
	 * \brief 设置竞赛状态
	 *
	 */
	void quizState()
	{
		quiz = true;
	}
	
	/**
	 * \brief 清除竞赛状态
	 *
	 */
	void clearQuizState()
	{
		quiz = false;
	}
	
	/**
	 * \brief 是否在竞赛状态
	 *
	 */
	bool isQuizState()
	{
		return quiz;
	}

	/**
	 * \brief 是否在游戏状态
	 *
	 *
	 * \return 在游戏状态返回ture,否则返回false
	 */
	bool isPlayState() const
	{
		return SYSTEM_STATE_PLAY == systemstate;
	}

	/**
	 * \brief 是否在等待游戏状态
	 * 存在一种状态:网关刚刚收到选择角色指令后收到退出指令,可能出现"id正在使用"的情况
	 *
	 *
	 * \return 在游戏状态返回ture,否则返回false
	 */
	bool isWaitPlayState() const
	{
		return SYSTEM_WAIT_STATE_PLAY == systemstate;
	}
	/**
	 * \brief 设置等待游戏状态
	 *
	 *
	 */
	void waitPlayState()
	{
		systemstate = SYSTEM_WAIT_STATE_PLAY;
	}

	void Terminate();
	void refreshCharInfo();
	bool checkPassport(const char *passport);
	/**
	 * \brief 通知客户端没有创建的角色
	 *
	 */
	void noCharInfo()
	{
		using namespace Cmd;
		stServerReturnLoginFailedCmd cmd;
		cmd.byReturnCode=LOGIN_RETURN_USERDATANOEXIST;
		if (sendCmd(&cmd, sizeof(cmd)))
		{
			selectState();
		}
	}

	/**
	 * \brief 通知客户端名字重复
	 *
	 */
	void nameRepeat()
	{
		using namespace Cmd;
		stServerReturnLoginFailedCmd cmd;
		cmd.byReturnCode=LOGIN_RETURN_CHARNAMEREPEAT;
		if (sendCmd(&cmd, sizeof(cmd)))
		{
			selectState();
		}
	}
	
	/**
	 * \brief 获取帐号
	 *
	 */
	const char* getAccount();

	/**
	 * \brief 将消息转发到场景
	 *
	 */
	bool forwardScene(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen);
	bool forwardSceneBill(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen);
	bool forwardBill(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen);
	bool forwardBillScene(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen);
	void setVip(bool b);
		
	bool beginSelect();
	void reg(int charno);
	void unreg(bool out=false);
	bool beginGame();
	bool sendCmd(const void *pstrCmd, const unsigned int nCmdLen,const unsigned int type=0,const char *strName=NULL,const bool hasPacked=false);
	static void cmdFilter(Cmd::stNullUserCmd *cmd,DWORD &type,char *name,DWORD &cmdLen);
	void final();
	void addBlackList(const char *);
	void removeBlackList(const char *);
	bool checkChatCmd(DWORD type, const char *strName);
	public:
	/// 地图屏索引hash key
	DWORD mapScreenIndex;
	/// 是否隐身
	bool hide;

	/// 是否已经添加到索引
	bool inserted;

	/// 系统设置
	BYTE sysSetting[20];

	public:
	/**
	 * \brief 刷新用户屏索引
	 *
	 *
	 * \param screen 新的屏索引hashkey
	 * \return 
	 */
	void setIndexKey(const DWORD screen)
	{
		mapScreenIndex = screen; 	 
	}
	const DWORD getIndexKey() const
	{
		return mapScreenIndex;
	}
	bool isHide() const
	{
		return hide;
	}
};
#endif
