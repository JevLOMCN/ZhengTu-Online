#include <stdarg.h>
#include <fstream>
#include <sstream>
#include "SceneManager.h"
#include "zRegex.h"
#include "Chat.h"
#include "zProperties.h"
#include "Team.h"
#include "SessionClient.h"
#include "QuestEvent.h"
#include "QuestTable.h"
#include "zSceneEntry.h"
#include "RebuildObject.h"
#include "ScenesServer.h"
#include "SceneTaskManager.h"
#include "SceneCommand.h"
#include <string.h>
#include <stdio.h>
#include "GmToolCommand.h"
#include "CartoonPet.h"
#include "BillCommand.h"
#include "Dice.h"
#include "QuestCondition.h"
#include "MessageSystem.h"

extern unsigned int cancel_country_need_money;          
extern unsigned int is_cancel_country; 


/**
 * \brief GM指令数组
 *
 */
Gm GmCmds[]=
{
	//普通GM用
	{"help",Gm::help,Gm::gm_mode,"显示指令帮助"},
	{"levelup",Gm::levelup,Gm::gm_mode,"升级"},
	{"goto",Gm::goTo_Gm,Gm::gm_mode,"跳转位置"},
	{"gomap",Gm::gomap_Gm,Gm::gm_mode,"跳转地图"},
	{"finduser",Gm::finduser,Gm::gm_mode,"寻找玩家"},
	{"gotouser",Gm::gotouser,Gm::gm_mode,"跟踪玩家"},
	{"catchuser",Gm::catchuser,Gm::gm_mode,"召唤玩家"},
	{"bczone",Gm::bczone,Gm::gm_mode,"发送区域广播"},
	{"bcworld",Gm::bcworld,Gm::gm_mode,"发送世界广播"},
	{"bcwgamemsg",Gm::bcwgamemsg,Gm::gm_mode,"发送游戏性世界广播"},
	{"kick",Gm::kick,Gm::gm_mode,"踢掉玩家"},
	{"donttalk",Gm::donttalk,Gm::gm_mode,"禁言"},
	{"talk",Gm::talk,Gm::gm_mode,"解除禁言"},
	{"gohome",Gm::goHome,Gm::gm_mode,"回到主城"},
	{"hideme",Gm::hideMe,Gm::gm_mode,"隐藏自己"},
	{"clearseptunion",Gm::clearseptunion,Gm::gm_mode,"恢复普通模式"},
	{"showme",Gm::showMe,Gm::gm_mode,"恢复可见状态"},
	{"god",Gm::god,Gm::gm_mode,"无敌模式"},
	{"normal",Gm::normal,Gm::gm_mode,"恢复普通模式"},
	{"clearseptunion",Gm::clearSeptUnion,Gm::gm_mode,"清除老版帮会和家族数据"},
	{"clearpoint",Gm::clearPoint,Gm::gm_mode,"清除人物属性点"},
	{"clearskillpoint",Gm::clearSkillPoint,Gm::gm_mode,"清除技能点"},
	{"studyskill",Gm::studySkill, Gm::gm_mode,"学习新技能"},
	{"settire",Gm::setTire, Gm::gm_mode,"设置疲劳时间"},
	{"checkcountryinfo",Gm::checkCountryInfo, Gm::gm_mode,"检查国家信息"},
	{"loadprocess",Gm::loadProcess, Gm::gm_mode,"重新加载外挂特征码文件"},
	{"cleararea",Gm::clearArea, Gm::gm_mode,"驱散周围玩家"},
	{"embar",Gm::embar, Gm::gm_mode,"囚禁指定的玩家"},
	{"refreshgeneral",Gm::refreshGeneral, Gm::gm_mode,"手动刷新各国大将军"},

    {"loadmessage",Gm::loadMessage,Gm::gm_mode,"重新加载Meaaage配置"},
	{"loadquest",Gm::loadQuest,Gm::gm_mode,"加载任务脚本"},
	{"loadnewquest",Gm::loadNewQuest, Gm::gm_mode,"加载lua任务脚本"},
    {"loadgiftBox",Gm::loadGiftBox, Gm::gm_mode,"加载宝盒配置"},
	{"loadnpctrade",Gm::loadNpctrade,Gm::gm_mode,"重新加载NPC交易脚本"},
	{"loadtbl",Gm::loadTbl,Gm::gm_mode,"重新加载TBL脚本"},
	{"loadtbl2",Gm::loadTbl2,Gm::gm_mode,"重新加载TBL脚本"},
	{"loadgameconfig",Gm::loadGameConfig, Gm::gm_mode,"加载温泉配置脚本"},
	{"loadfuben",Gm::loadFuBenConfig, Gm::gm_mode,"加载副本配置脚本"}, //副本	

	//组长用
	{"closenpc",Gm::closenpc,Gm::gm_mode,"关闭功能NPC"},
	{"countrypower",Gm::countrypower,Gm::gm_mode,"处理国家强弱"},
	{"summon",Gm::summon,Gm::gm_mode,"GM召唤Npc"},
	{"usleep",Gm::usleep,Gm::gm_mode,"线程usleep的毫秒数"},
	{"createrush",Gm::createRush,Gm::gm_mode,"引发一次怪物攻城"},

	//超级GM用
	{"kickgateuser",Gm::kickGateUser,Gm::gm_mode,"踢掉网关上的玩家"},
	{"loadgift",Gm::loadGift,Gm::gm_mode,"重新加载礼官的数据"},
	{"enableregister",Gm::enableRegister,Gm::gm_mode,"允许注册"},
	{"enablelogin",Gm::enableLogin,Gm::gm_mode,"允许登陆"},
	{"setservice",Gm::setService,Gm::gm_mode,"设置游戏功能"},
	{"setfylevel",Gm::setFylevel, Gm::gm_mode,"设置游戏(未转生)封印等级"},
	{"setmaxlevel",Gm::setMaxlevel, Gm::gm_mode,"设置游戏(未转生)最大等级"},
	{"settrunfylevel",Gm::setTrunFylevel, Gm::gm_mode,"设置游戏(转生)封印等级"},
	{"settrunmaxlevel",Gm::setTrunMaxlevel, Gm::gm_mode,"设置游戏(转生)最大等级"},
	{"decitydare",Gm::debugCityDare,Gm::gm_mode,"启动和停止帮会夺城战"},
	{"deempdare",Gm::debugEmperorDare,Gm::gm_mode,"启动和停止皇城争夺战"},
	{"createquiz",Gm::createQuiz,Gm::gm_mode,"发起一次全区智力竞赛"},
	{"setpriv",Gm::setPriv,Gm::gm_mode,"设置角色权限"},
	{"newzone",Gm::newzone,Gm::gm_mode,"设置运行参数"},
	{"stockconsign",Gm::stockconsign,Gm::gm_mode,"股票委托管理"},
	{"killer",Gm::killer, Gm::gm_mode,"必杀模式"},
	{"tong",Gm::tong,Gm::gm_mode,"激活帮会战-使用方法://tong t1=XXX t2=XXX"},
	{"clearworld",Gm::clearWorld,Gm::super_mode |Gm::gm_mode,"清除范围内的怪物"},
	{"showspecialnpc",Gm::showSpecialNpc,Gm::gm_mode,"查看当前的特殊NPC"},
	{"qaccount",Gm::qAccount,Gm::gm_mode,"查询帐号"},
	{"userdistribute",Gm::countServerUser,Gm::gm_mode,"得到本组服务器的人数分布"},
	{"shutdown",Gm::shutdown,Gm::gm_mode,"停机维护"},
	{"systime",Gm::systime,Gm::gm_mode,"得到系统当前时间"},
	{"goldsystem",Gm::goldsystem,Gm::gm_mode,"金币系统"},
	{"stocksystem",Gm::stocksystem,Gm::gm_mode,"股票系统"},


	//调试用
	{"viewcountrydare",Gm::viewCountryDare,Gm::gm_mode,"查看国战记录"},
	{"givegold", Gm::givegold, Gm::gm_mode, "金币补偿"},
	{"givemoney", Gm::givemoney, Gm::gm_mode, "银币补偿"},
	{"givestock", Gm::givestock, Gm::gm_mode, "股票补偿"},
	{"version",Gm::version, Gm::gm_mode,"动态设置服务器版本号"},
	{"createunion", Gm::createUnion, Gm::gm_mode, "创建一个帮会"},
	{"createfamily", Gm::createFamily, Gm::gm_mode, "创建一个家族"},
	{"debugvote", Gm::debugVote, Gm::gm_mode, "启动和停止投票"},
	{"debuggem", Gm::debugGem, Gm::gm_mode, "启动和停止护宝任务"},
	{"addsepexp", Gm::addSeptExp, Gm::gm_mode, "领取家族经验"},
	{"setrepute", Gm::setRepute, Gm::gm_mode, "设置家族声望"},
	{"setseptlevel", Gm::setSeptLevel, Gm::gm_mode, "设置家族等级"},
	{"setallyfd", Gm::setAllyFriendDegree, Gm::gm_mode, "设置联盟友好度"},
	{"changecountry", Gm::changeCountry, Gm::gm_mode, "开启和停止叛国功能"},
	{"loadmap",Gm::loadMap, Gm::gm_mode,"加载地图"},
	{"unloadmap",Gm::unloadMap, Gm::gm_mode,"卸载地图"},
	{"callpet",Gm::callPet, Gm::gm_mode,"召唤宠物"},
	{"lockvalue",Gm::lockValue, Gm::gm_mode,"锁定消耗"},
	{"svote",Gm::svote,Gm::gm_mode,"家族投票-使用方法://svote name=XXX"},
	{"uvote",Gm::uvote,Gm::gm_mode,"帮会投票-使用方法://uvote name=XXX"},       
	{"createschool", Gm::createSchool, Gm::gm_mode, "创建一个门派"},
	{"fetch",Gm::fetch, Gm::gm_mode,"GM造物品"},
	{"gzfetch",Gm::gzfetch, Gm::gm_mode,"GM改造物品"},
	{"showentries", Gm::showEntries, Gm::gm_mode, "查看周围的npc和玩家"},
	{"getsize", Gm::getsize, Gm::gm_mode, "查看角色所占空间"},
	{"setpetai", Gm::setPetAI, Gm::gm_mode, "设置宠物行为模式"},
	{"setpetaif", Gm::setPetAIF, Gm::gm_mode, "设置宠物AIF"},
	{"bank", Gm::bank, Gm::gm_mode, "设置仓库相关"},
	{"backoff",Gm::backoff, Gm::gm_mode,"GM造物品"},
	{"upskill",Gm::upgradeSkill, Gm::gm_mode,"学习魔法"},
	{"abandon",Gm::abandon, Gm::gm_mode,"清除任务"},
	{"showmaps",Gm::showMaps, Gm::gm_mode,"查看已经加载的地图"},
	{"showpets",Gm::showPets, Gm::gm_mode,"查看当前的宠物"},
	{"killpets",Gm::killPets, Gm::gm_mode,"查看当前的宠物"},
	{"leechdom",Gm::leechdom, Gm::gm_mode,"使用药品"},
	{"value",Gm::value, Gm::gm_mode,"设置hp,mp,sp"},
	{"goodness",Gm::goodness, Gm::gm_mode,"取得当前善恶度"},
	{"getvalue",Gm::getvalue, Gm::gm_mode,"取得某个数值,方便QA测试,目前只有魅力"},
	{"setvalue",Gm::setvalue, Gm::gm_mode,"设置某个数值,方便QA测试"},
	{"skill",Gm::skill, Gm::gm_mode,"技能测试指令"},
	{"addexp", Gm::addExp, Gm::gm_mode, "增加经验值"},
	{"setquest", Gm::setQuest, Gm::gm_mode, "更改任务变量"},
	{"checkquest", Gm::checkQuest, Gm::gm_mode, "检查可以接的任务"},
	{"setblock", Gm::setBlock, Gm::gm_mode, "设置阻挡"},
	{"checkblock", Gm::checkBlock, Gm::gm_mode, "设置阻挡"},
	{"npcdare", Gm::npcDare, Gm::gm_mode, "立即触发NPC争夺战"},
	{"team", Gm::team, Gm::gm_mode, "组队"},
	{"throwobject", Gm::throwobject, Gm::gm_mode, "扔东西"},
	{"querypoint", Gm::querypoint, Gm::gm_mode, "查询点数"},
	{"redeemgold", Gm::redeemgold, Gm::gm_mode, "兑换金币"},
	{"querygold", Gm::querygold, Gm::gm_mode, "兑换金币"},
//	{"redeemmonthcard", Gm::redeemmonthcard, Gm::gm_mode, "兑换月卡"},
    {"redeemmonthcard", Gm::redeemmonthcard, Gm::normal_mode, "兑换月卡"},
	{"stockuser", Gm::stockuser, Gm::gm_mode, "股票用户管理"},
	{"stocktransfer", Gm::stocktransfer, Gm::gm_mode, "股票资金管理"},
	{"clearcolddown", Gm::clearColddown, Gm::gm_mode, "清除冷却时间"},
	{"queryincmap", Gm::queryincmap, Gm::gm_mode, "请求增值地图列表"},
	{"showaddexp", Gm::showAddExp, Gm::gm_mode, "显示替身 宠物附加的经验"},
	{"changehorse", Gm::changeHorse, Gm::gm_mode, "换马"},
	{"contribute", Gm::contribute, Gm::gm_mode, "捐献国库"},
	{"clearskill", Gm::clearSkill, Gm::gm_mode, "清除指定技能"},
	{"getgive", Gm::getgive, Gm::gm_mode, "获取赠品"},
	{NULL,NULL,0,NULL}
};

/*
 * \brief 向用户发送消息
 * 如果用户不在该服务器，就由session转发
 *
 * \param id 用户的ID
 * \param cmd 消息
 * \param len 消息长度
 *
 * \return 是否成功
 */
bool sendCmdByID(DWORD id, const void *cmd, int len)
{
	SceneUser * pUser = SceneUserManager::getMe().getUserByID(id);
	if (pUser)
		pUser->sendCmdToMe(cmd, len);
	else
	{
		char buf[zSocket::MAX_DATASIZE];
		Cmd::Session::t_forwardUser_SceneSession * send = (Cmd::Session::t_forwardUser_SceneSession *)buf;
		constructInPlace(send);

		send->id = id;
		send->cmd_len = len;
		bcopy(cmd, send->cmd, len);

		sessionClient->sendCmd(send, sizeof(Cmd::Session::t_forwardUser_SceneSession)+len);
	}
	return true;
}


/*
 * \brief 向用户发送消息
 * 如果用户不在该服务器，就由session转发
 *
 * \param id 用户的临时ID
 * \param cmd 消息
 * \param len 消息长度
 *
 * \return 是否成功
 */
bool sendCmdByTempID(DWORD id, const void *cmd, int len)
{
	SceneUser * pUser = SceneUserManager::getMe().getUserByTempID(id);
	if (pUser)
		pUser->sendCmdToMe(cmd, len);
	else
	{
		char buf[zSocket::MAX_DATASIZE];
		Cmd::Session::t_forwardUser_SceneSession * send = (Cmd::Session::t_forwardUser_SceneSession *)buf;
		constructInPlace(send);

		send->tempid = id;
		send->cmd_len = len;
		bcopy(cmd, send->cmd, len);

		sessionClient->sendCmd(send, sizeof(Cmd::Session::t_forwardUser_SceneSession)+len);
	}
	return true;
}


/*
 * \brief 向用户发送消息
 * 如果用户不在该服务器，就由session转发
 *
 * \param name 用户的名字 
 * \param cmd 消息
 * \param len 消息长度
 *
 * \return 是否成功
 */
bool sendCmdByName(char * name, const void *cmd, int len)
{
	SceneUser * pUser = SceneUserManager::getMe().getUserByName(name);
	if (pUser)
		pUser->sendCmdToMe(cmd, len);
	else
	{
		char buf[zSocket::MAX_DATASIZE];
		Cmd::Session::t_forwardUser_SceneSession * send = (Cmd::Session::t_forwardUser_SceneSession *)buf;
		constructInPlace(send);

		strncpy(send->name, name, MAX_NAMESIZE);
		send->cmd_len = len;
		bcopy(cmd, send->cmd, len);

		sessionClient->sendCmd(send, sizeof(Cmd::Session::t_forwardUser_SceneSession)+len);
	}
	return true;
}

/**
 * \brief 构造函数
 *
 *
 * \param pUser: 创建该频道的用户
 */
Channel::Channel(SceneUser *pUser):zEntry()
{
	strncpy(name,pUser->name,MAX_NAMESIZE);
	creater.id=pUser->id;
	creater.tempid=pUser->tempid;
	strncpy(creater.name,pUser->name,MAX_NAMESIZE);
}

/**
 * \brief 发送给该频道的所有用户
 *
 *
 * \param cmd: 聊天内容
 * \param len:内容长度
 * \return 发送是否成功
 */
bool Channel::sendCmdToAll(const void *cmd,int len)
{
	if(cmd==NULL) return false;
	for(unsigned int i=0;i<userlist.size();i++)
	{
		SceneUser *temp = SceneUserManager::getMe().getUserByName(userlist[i].name);
		if(temp!=NULL)
		{
			temp->sendCmdToMe(cmd,len);
		}
	}
	return true;
}

/**
 * \brief 离开频道
 *
 *
 * \param uname: 离开该频道的用户
 * \return 该频道是否还能继续存在
 */
bool Channel::remove(const char *uname)
{
	WORD found=has(uname);
	if(found!=(WORD)-1)
	{
		Zebra::logger->gbug("%s离开%s的聊天频道",uname,creater.name);
		Cmd::stLeave_ChannelUserCmd send;
		send.dwChannelID=tempid;
		strncpy(send.name,uname,MAX_NAMESIZE);
		sendCmdToAll(&send,sizeof(send));

		if((found+1)<(WORD)userlist.size())
		{
			userlist[found]=userlist.back();
		}
		userlist.pop_back();
	}
	if(userlist.empty())
		return false;
	else
		return true;
}

/**
 * \brief 加入聊天频道
 *
 *
 * \param pUser: 要加入的用户
 * \return 加入是否成功
 */
bool Channel::add(const char *uname)
{
	return false;
}

/**
 * \brief 加入聊天频道
 *
 *
 * \param pUser: 要加入的用户
 * \return 加入是否成功
 */
bool Channel::add(SceneUser *pUser)
{
	if(pUser!=NULL)
	{
		WORD found=has(pUser->name);
		if(found==(WORD)-1)
		{
			Zebra::logger->gbug("%s加入%s的聊天频道",pUser->name,creater.name);
			//添加
			zEntryC temp;
			temp.id=pUser->id;
			temp.tempid=pUser->tempid;
			strncpy(temp.name,pUser->name,MAX_NAMESIZE);
			userlist.push_back(temp);

			//把自己发送给所有人
			Cmd::stJoin_ChannelUserCmd send;
			send.dwChannelID=tempid;
			strncpy(send.name,pUser->name,MAX_NAMESIZE);
			sendCmdToAll(&send,sizeof(send));

			//发送所有成员给自己
			for(unsigned int i=0;i<userlist.size();i++)
			{
				strncpy(send.name,userlist[i].name,MAX_NAMESIZE);
				pUser->sendCmdToMe(&send,sizeof(send));
			}
			return true;
		}
	}
	return false;
}

/**
 * \brief 聊天频道中是否存在某人
 *
 *
 * \param name: 用户名
 * \return 存在返回该用户在频道中的位置,不存在返回-1
 */
WORD Channel::has(const char *name)
{
	if(name)
	{
		for(unsigned int i=0;i<userlist.size();i++)
		{
			if(strncmp(userlist[i].name,name,MAX_NAMESIZE)==0)
				return i;
		}
	}
	return (WORD)-1;
}

/**
 * \brief 发送聊天信息到9屏
 *
 *
 * \param pUser: 请求发送者
 * \param content: 发送的内容
 * \return 发送是否成功
 */
bool Channel::sendNine(SceneUser *pUser,const char *content)
{
	if(content[0]=='/' && content[1]=='/')
	{
		Gm::exec(pUser,(char *)content+2);
	}
	else
	{
		Cmd::stChannelChatUserCmd send;
		zRTime ctv;
		send.dwType=Cmd::CHAT_TYPE_NINE;
		send.dwChatTime = ctv.sec();
		strncpy((char *)send.pstrChat,(char *)content,MAX_CHATINFO-1);
		if (pUser->mask.is_masking()) {
			strncpy((char *)send.pstrName, "蒙面人",MAX_NAMESIZE);
		}else {
			strncpy((char *)send.pstrName,pUser->name,MAX_NAMESIZE);
		}
		pUser->scene->sendCmdToNine(pUser->getPosI(),&send,sizeof(send));
	}
	return true;
}

/**
 * \brief 转发聊天信息到9屏
 *
 *
 * \param pUser: 请求发送者
 * \param rev: 转发的内容 
 * \param cmdLen: 内容长度
 * \return 发送是否成功
 */
bool Channel::sendNine(SceneUser *pUser,const Cmd::stChannelChatUserCmd *rev,unsigned int cmdLen)
{
	if(rev->pstrChat[0]=='/' && rev->pstrChat[1]=='/')
	{
		Gm::exec(pUser,(char *)rev->pstrChat+2);
	}
	else
	{
		if (pUser->mask.is_masking()) {
			strncpy((char *)rev->pstrName, "蒙面人",MAX_NAMESIZE);
		}
		pUser->scene->sendCmdToNine(pUser->getPosI(),rev,cmdLen);
	}
	return true;
}

#define getMessage(msg,msglen,pat)	\
	do	\
{	\
	va_list ap;	\
	bzero(msg, msglen);	\
	va_start(ap, pat);		\
	vsnprintf(msg, msglen - 1, pat, ap);	\
	va_end(ap);	\
}while(false)

/**
 * \brief 在频道中转发信息
 *
 *
 * \param pUser: 请求转发者
 * \param pattern: 转发的内容
 * \return 是否成功
 */
bool Channel::sendToAll(SceneUser *pUser,const char *pattern, ...)
{
	char buf[MAX_CHATINFO];
	getMessage(buf,MAX_CHATINFO,pattern);
	zRTime ctv;
	Cmd::stChannelChatUserCmd send;
	send.dwType=Cmd::CHAT_TYPE_PERSON;
	send.dwCharType = pUser->charbase.type; //修复换头像不显示昵称
	send.dwChatTime = ctv.sec();
	send.dwChannelID=tempid;
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));
	strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);
	strncpy((char *)send.pstrName,pUser->name,MAX_NAMESIZE);
	for(unsigned int i=0;i<userlist.size();i++)
	{
		SceneUser *temp = SceneUserManager::getMe().getUserByName(userlist[i].name);
		if(temp!=NULL && temp!=pUser)
		{
			temp->sendCmdToMe(&send,sizeof(send));
		}
	}
	return true;
}

/**
 * \brief 在频道中转发信息
 *
 *
 * \param pUser: 请求转发者
 * \param rev: 转发的指令
 * \param cmdLen: 指令长度
 * \return 是否成功
 */
bool Channel::sendToAll(SceneUser *pUser,const Cmd::stChannelChatUserCmd *rev, unsigned int cmdLen)
{
	if(rev->pstrChat[0]=='/' && rev->pstrChat[1]=='/')
	{
		Gm::exec(pUser,(char *)rev->pstrChat+2);
	}
	else
	{
		for(unsigned int i=0;i<userlist.size();i++)
		{
			SceneUser *temp = SceneUserManager::getMe().getUserByName(userlist[i].name);
			if(temp!=NULL && temp!=pUser)
			{
				temp->sendCmdToMe(rev,cmdLen);
			}
		}
	}
	return true;
}

/**
 * \brief 发送国家公告
 *
 *
 * \param countryID: countryID
 * \param infoType: infoType
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendCountryInfo(DWORD countryID, DWORD infoType, const char *pattern, ...)
{
	char buf[MAX_CHATINFO];
	getMessage(buf,MAX_CHATINFO,pattern);

	Cmd::Session::t_countryNotify_SceneSession send;
	send.infoType = infoType;
	send.dwCountryID = countryID;
	strncpy(send.info, buf, MAX_CHATINFO);
	sessionClient->sendCmd(&send, sizeof(send));
	return true;
}

/**
 * \brief 发送全服公告
 *
 * \param infoType: infoType
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendAllInfo(DWORD infoType, const char *pattern, ...)
{
	char buf[MAX_CHATINFO];
	getMessage(buf,MAX_CHATINFO,pattern);

	Cmd::Session::t_serverNotify_SceneSession send;
	send.infoType = infoType;
	strncpy(send.info, buf, MAX_CHATINFO);
	sessionClient->sendCmd(&send, sizeof(send));
	return true;
}

/**
 * \brief 发送信息到国家频道
 *
 *
 * \param pUser: 请求发送者
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendCountry(SceneUser *pUser, const char *pattern, ...)
{
	char buf[MAX_CHATINFO];
	getMessage(buf,MAX_CHATINFO,pattern);
	Cmd::stChannelChatUserCmd send;
	send.dwCharType = pUser->charbase.type; //修复换头像不显示昵称
	zRTime ctv;
	send.dwType=Cmd::CHAT_TYPE_COUNTRY;
	send.dwChatTime = ctv.sec();
	//	send.dwChannelID=tempid;
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));
	strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);
	strncpy((char *)send.pstrName,pUser->name,MAX_NAMESIZE);
	pUser->forwardSession(&send, sizeof(Cmd::stChannelChatUserCmd));
	return true;
}

/**
 * \brief 请求发送信息到国家频道
 *
 *
 * \param pUser: 请求指令的用户
 * \param rev: 收到的指令
 * \param cmdLen: 指令长度
 * \return 发送是否成功
 */
bool Channel::sendCountry(SceneUser *pUser, const Cmd::stChannelChatUserCmd *rev, unsigned int cmdLen)
{
	if(rev->pstrChat[0]=='/' && rev->pstrChat[1]=='/')
	{
		Gm::exec(pUser,(char *)rev->pstrChat+2);
	}
	else
	{
		pUser->forwardSession(rev, cmdLen);
	}
	return true;
}

/**
 * \brief 发送给该地图的所有用户
 *
 * \param mapID 地图ID
 * \param infoType 聊天类型
 * \param pattern: 聊天内容
 * \return 发送是否成功
 */
bool Channel::sendMapInfo(DWORD mapID, DWORD infoType, const char *pattern, ...)
{
	char buf[MAX_CHATINFO];
	getMessage(buf,MAX_CHATINFO,pattern);

	Cmd::stChannelChatUserCmd send;
	send.dwType = Cmd::CHAT_TYPE_SYSTEM;
	send.dwSysInfoType = infoType;
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));
	sprintf((char*)send.pstrChat, "%s", buf);

	Scene * s = SceneManager::getInstance().getSceneByID(mapID);
	if (s)
		 return s->sendCmdToScene(&send, sizeof(send));
	else
	{
		Cmd::Session::t_sendCmd_SceneSession sc;
		sc.mapID = mapID;
		sc.len = sizeof(send);
		bcopy(&send, sc.cmd, sizeof(send));
		sessionClient->sendCmd(&sc, sizeof(sc)+sizeof(send));
	}

	return true;
}

/**
 * \brief 发送给该地图的所有用户
 *
 * \param mapID 地图ID
 * \param infoType 聊天类型
 * \param pattern: 聊天内容
 * \return 发送是否成功
 */
bool Channel::sendMapInfo(Scene * s, DWORD infoType, const char *pattern, ...)
{
	if (!s) return false;

	char buf[MAX_CHATINFO];
	getMessage(buf,MAX_CHATINFO,pattern);

	Cmd::stChannelChatUserCmd send;
	send.dwType = Cmd::CHAT_TYPE_SYSTEM;
	send.dwSysInfoType = infoType;
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));
	sprintf((char*)send.pstrChat, "%s", buf);

	s->sendCmdToScene(&send, sizeof(send));

	return true;
}

/**
 * \brief 发送给该地图的所有用户
 *
 * \param scene 地图
 * \param cmd 消息
 * \param len: 长度
 * \return 发送是否成功
 */
bool Channel::sendCmdToMap(Scene * s, const void *cmd,int len)
{
	if(cmd==NULL) return false;
	if (!s) return false;

	s->sendCmdToScene(cmd, len);
	return true;
}

/**
 * \brief 发送给该地图的所有用户
 *
 * \param mapID 地图ID
 * \param cmd 消息
 * \param len: 长度
 * \return 发送是否成功
 */
bool Channel::sendCmdToMap(DWORD mapID, const void *cmd,int len)
{
	if(cmd==NULL) return false;

	Scene * s = SceneManager::getInstance().getSceneByID(mapID);
	if (s)
		 return s->sendCmdToScene(cmd, len);
	else
	{
		Cmd::Session::t_sendCmd_SceneSession sc;
		sc.mapID = mapID;
		sc.len = len;
		bcopy(cmd, sc.cmd, len);
		return sessionClient->sendCmd(&sc, sizeof(sc)+len);
	}
}

/**
 * \brief 队伍频道
 *
 */
struct ChatTeamExec : public TeamMemExec
{
	SceneUser *leader;
	Cmd::stChannelChatUserCmd send;
	ChatTeamExec(SceneUser *u , const char *data)
	{
		zRTime ctv;
		send.dwChatTime = ctv.sec();	
		leader = u;
		send.dwType=Cmd::CHAT_TYPE_TEAM;
		send.dwCharType = leader->charbase.type; //修复换头像不显示昵称
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat , data , MAX_CHATINFO-1);
	}
	bool exec(TeamMember &member)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(member.tempid);
		if(pUser)
		{
			pUser->sendCmdToMe(&send , sizeof(send));
		}
		return true;
	}

};

/**
 * \brief 队伍指令转发
 *
 */
struct ChatTransmitTeamExec : public TeamMemExec
{
	SceneUser *leader;
	const Cmd::stChannelChatUserCmd *send;
	unsigned int cmdLen;
	ChatTransmitTeamExec(SceneUser *u , const Cmd::stChannelChatUserCmd *rev, unsigned int len)
	{
		leader = u;
		send = rev;
		cmdLen = len;
	}
	bool exec(TeamMember &member)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(member.tempid);
		if(pUser)
		{
			pUser->sendCmdToMe(send , cmdLen);
		}
		return true;
	}

};

/**
 * \brief 发送队伍频道
 *
 *
 * \param teamid: 队伍id
 * \param pattern: 内容
 * \return 转发是否成功
 */
bool Channel::sendTeam(DWORD teamid ,const char *pattern, ...)					
{

	char buf[MAX_CHATINFO];
	getMessage(buf,MAX_CHATINFO,pattern);
	SceneUser *leader = SceneUserManager::getMe().getUserByTempID(teamid);
	if(leader)
	{
		ChatTeamExec exec(leader , buf);
		leader->team.execEveryOne(exec);
	}
	return true;
}


/**
 * \brief 发送9屏信息
 *
 *
 * \param pUser: 请求发送者
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendNine(SceneUser *pUser ,const char *pattern, ...)
{
	if(pUser)
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);
		zRTime ctv;
		Cmd::stChannelChatUserCmd send;
		send.dwType=Cmd::CHAT_TYPE_NINE;
		send.dwCharType = pUser->charbase.type; //修复换头像不显示昵称
		send.dwChatTime = ctv.sec();
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);
		strncpy((char *)send.pstrName,pUser->name,MAX_NAMESIZE-1);
		pUser->scene->sendCmdToNine(pUser->getPosI(),&send,sizeof(send));
		return true;
	}
	return false;
}

/**
 * \brief 发送9屏信息
 *
 *
 * \param pNpc: 请求发送的npc
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendNine(const SceneNpc *pNpc ,const char *pattern, ...)
{
	if(0==pNpc)  return false;

	char buf[MAX_CHATINFO];
	getMessage(buf,MAX_CHATINFO,pattern);
	zRTime ctv;
	Cmd::stChannelChatUserCmd send;
	send.dwType = Cmd::CHAT_TYPE_NPC;
	send.dwFromID = pNpc->tempid;
	send.dwChatTime = ctv.sec();
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));
	strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);
	strncpy((char *)send.pstrName,pNpc->name,MAX_NAMESIZE-1);
	pNpc->scene->sendCmdToNine(pNpc->getPosI(),&send,sizeof(send));
	return true;
}

/**
 * \brief 转发队伍信息
 *
 *
 * \param teamid: 队伍id
 * \param rev: 需要转发的指令
 * \param cmdLen: 指令长度
 * \return 转发是否成功
 */
bool Channel::sendTeam(DWORD teamid ,const Cmd::stChannelChatUserCmd *rev,unsigned int cmdLen)
{

	SceneUser *leader = SceneUserManager::getMe().getUserByTempID(teamid);
	if(leader)
	{
		ChatTransmitTeamExec exec(leader , rev, cmdLen);
		leader->team.execEveryOne(exec);
	}
	return true;
}

/**
 * \brief 发送系统消息
 *
 *
 * \param dwUserTempID: 要求发送消息的用户
 * \param type: 系统消息类型
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendSys(DWORD dwUserTempID,int type, const char *pattern, ...)
{
	SceneUser *pToUser=SceneUserManager::getMe().getUserByTempID(dwUserTempID);
	if(pToUser)
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);
		zRTime ctv;
		Cmd::stChannelChatUserCmd send;
		send.dwType=Cmd::CHAT_TYPE_SYSTEM;
		send.dwSysInfoType = type;//系统消息的类型
		send.dwChatTime = ctv.sec();
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);

		pToUser->sendCmdToMe(&send,sizeof(send));
		return true;
	}
	return false;
}

/**
 * \brief 发送系统消息
 *
 *
 * \param pUser: 要求发送消息的用户
 * \param type: 系统消息类型
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendSys(SceneUser *pUser, int type, const char *pattern, ...)
{
	if (pUser)
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);
		zRTime ctv;
		Cmd::stChannelChatUserCmd send;
		send.dwType=Cmd::CHAT_TYPE_SYSTEM;
		send.dwSysInfoType = type;//系统消息的类型
		send.dwChatTime = ctv.sec();
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);

		pUser->sendCmdToMe(&send,sizeof(send));
		return true;
	}
	return false;
}
/**
 * \brief 发送系统金子消息
 *
 *
 * \param pUser: 要求发送消息的用户
 * \param type: 系统消息类型
 * \param gold: 消息显示的金子数量
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendGold(SceneUser *pUser, int type, DWORD gold , const char *pattern, ...)
{
	if (pUser)
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);

		//金子换算
		if (gold)
		{
			if(gold/10000 > 0)
			{
				sprintf(buf+strlen(buf) , "%d锭" , gold/10000);
				gold = gold % 10000;
			}

			if(gold/100 > 0)
			{
				sprintf(buf+strlen(buf) , "%d两" , gold/100);
				gold = gold % 100;
			}

			if(gold)
			{
				sprintf(buf+strlen(buf) , "%d文" , gold);
			}
		}
		else
		{
			sprintf(buf+strlen(buf) , "%d文" , gold);
		}

		zRTime ctv;
		Cmd::stChannelChatUserCmd send;
		send.dwType=Cmd::CHAT_TYPE_SYSTEM;
		send.dwSysInfoType = type;//系统消息的类型
		send.dwChatTime = ctv.sec();
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);

		pUser->sendCmdToMe(&send,sizeof(send));
		return true;
	}
	return false;
}

/**
 * \brief 发送系统积分点数消息
 *
 *
 * \param pUser: 要求发送消息的用户
 * \param type: 系统消息类型
 * \param gold: 消息显示的铜钱数量
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendTicket(SceneUser *pUser, int type, DWORD ticket , const char *pattern, ...)
{
	if (pUser)
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);

		//积分点数换算
		if (ticket)
		{
			sprintf(buf+strlen(buf) , "%d点" , ticket);
		}

		zRTime ctv;
		Cmd::stChannelChatUserCmd send;
		send.dwType=Cmd::CHAT_TYPE_SYSTEM;
		send.dwSysInfoType = type;//系统消息的类型
		send.dwChatTime = ctv.sec();
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);

		pUser->sendCmdToMe(&send,sizeof(send));
		return true;
	}
	return false;
}
/**
 * \brief 发送系统工资消息
 *
 *
 * \param pUser: 要求发送消息的用户
 * \param type: 系统消息类型
 * \param gold: 消息显示的工资数量
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendSalary(SceneUser *pUser, int type, DWORD salary , const char *pattern, ...)
{
	if (pUser)
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);

		//工资换算
		if (salary)
		{
			if(salary/10000 > 0)
			{
				sprintf(buf+strlen(buf) , "%d锭" , salary/10000);
				salary = salary % 10000;
			}

			if(salary/100 > 0)
			{
				sprintf(buf+strlen(buf) , "%d两" , salary/100);
				salary = salary % 100;
			}

			if(salary)
			{
				sprintf(buf+strlen(buf) , "%d文" , salary);
			}
		}
		else
		{
			sprintf(buf+strlen(buf) , "%d文" , salary);
		}

		zRTime ctv;
		Cmd::stChannelChatUserCmd send;
		send.dwType=Cmd::CHAT_TYPE_SYSTEM;
		send.dwSysInfoType = type;//系统消息的类型
		send.dwChatTime = ctv.sec();
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);

		pUser->sendCmdToMe(&send,sizeof(send));
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////
//soke 增加VIP点数算法
//////////////////////////////////////////////////////////
/**
 * \brief 发送VIP点数消息
 *
 *
 * \param pUser: 要求发送消息的用户
 * \param type: 系统消息类型
 * \param gold: 消息显示的VIP点数
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendCharvip(SceneUser *pUser, int type, DWORD charvip , const char *pattern, ...)
{
	if (pUser)
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);

		//VIP点数算法
		if (charvip)
		{
			if(charvip)
			{
				sprintf(buf+strlen(buf) , "%d点" , charvip);
			}
		}
		else
		{
			sprintf(buf+strlen(buf) , "%d点" , charvip);
		}

		zRTime ctv;
		Cmd::stChannelChatUserCmd send;
		send.dwType=Cmd::CHAT_TYPE_SYSTEM;
		send.dwSysInfoType = type;//系统消息的类型
		send.dwChatTime = ctv.sec();
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);

		pUser->sendCmdToMe(&send,sizeof(send));
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////
//soke 增加充值点数算法
//////////////////////////////////////////////////////////
/**
 * \brief 发送充值点数消息
 *
 *
 * \param pUser: 要求发送消息的用户
 * \param type: 系统消息类型
 * \param gold: 消息显示的充值点数
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendChongzhi(SceneUser *pUser, int type, DWORD chongzhi , const char *pattern, ...)
{
	if (pUser)
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);

		//充值点数算法
		if (chongzhi)
		{
			if(chongzhi)
			{
				sprintf(buf+strlen(buf) , "%d点" , chongzhi);
			}
		}
		else
		{
			sprintf(buf+strlen(buf) , "%d点" , chongzhi);
		}

		zRTime ctv;
		Cmd::stChannelChatUserCmd send;
		send.dwType=Cmd::CHAT_TYPE_SYSTEM;
		send.dwSysInfoType = type;//系统消息的类型
		send.dwChatTime = ctv.sec();
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);

		pUser->sendCmdToMe(&send,sizeof(send));
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////
//soke 增加职业点数算法
//////////////////////////////////////////////////////////
/**
 * \brief 发送VIP点数消息
 *
 *
 * \param pUser: 要求发送消息的用户
 * \param type: 系统消息类型
 * \param gold: 消息显示的职业点数
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendChangejob(SceneUser *pUser, int type, DWORD changejob , const char *pattern, ...)
{
	if (pUser)
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);

		//职业点数算法
		if (changejob)
		{
			if(changejob)
			{
				sprintf(buf+strlen(buf) , "%d点" , changejob);
			}
		}
		else
		{
			sprintf(buf+strlen(buf) , "%d点" , changejob);
		}

		zRTime ctv;
		Cmd::stChannelChatUserCmd send;
		send.dwType=Cmd::CHAT_TYPE_SYSTEM;
		send.dwSysInfoType = type;//系统消息的类型
		send.dwChatTime = ctv.sec();
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);

		pUser->sendCmdToMe(&send,sizeof(send));
		return true;
	}
	return false;
}

bool Channel::sendHanbing(SceneUser *pUser, int type, DWORD hanbing , const char *pattern, ...)
{
	if (pUser)
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);

		//点数算法
		if (hanbing)
		{
			if(hanbing)
			{
				sprintf(buf+strlen(buf) , "%d点" , hanbing);
			}
		}
		else
		{
			sprintf(buf+strlen(buf) , "%d点" , hanbing);
		}

		zRTime ctv;
		Cmd::stChannelChatUserCmd send;
		send.dwType=Cmd::CHAT_TYPE_SYSTEM;
		send.dwSysInfoType = type;//系统消息的类型
		send.dwChatTime = ctv.sec();
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);

		pUser->sendCmdToMe(&send,sizeof(send));
		return true;
	}
	return false;
}

bool Channel::sendAngel(SceneUser *pUser, int type, DWORD angel , const char *pattern, ...)
{
	if (pUser)
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);

		//点数算法
		if (angel)
		{
			if(angel)
			{
				sprintf(buf+strlen(buf) , "%d点" , angel);
			}
		}
		else
		{
			sprintf(buf+strlen(buf) , "%d点" , angel);
		}

		zRTime ctv;
		Cmd::stChannelChatUserCmd send;
		send.dwType=Cmd::CHAT_TYPE_SYSTEM;
		send.dwSysInfoType = type;//系统消息的类型
		send.dwChatTime = ctv.sec();
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);

		pUser->sendCmdToMe(&send,sizeof(send));
		return true;
	}
	return false;
}


////////////////////////////////////////////////////////
/**
 * \brief 发送鲜花数消息
 *
 *
 * \param pUser: 要求发送消息的用户
 * \param type: 系统消息类型
 * \param gold: 消息显示的鲜花数
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendFolwers(SceneUser *pUser, int type, DWORD folwers , const char *pattern, ...)
{
	if (pUser)
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);

		//鲜花数算法
		if (folwers)
		{
			if(folwers)
			{
				sprintf(buf+strlen(buf) , "%d朵" , folwers);
			}
		}
		else
		{
			sprintf(buf+strlen(buf) , "%d朵" , folwers);
		}

		zRTime ctv;
		Cmd::stChannelChatUserCmd send;
		send.dwType=Cmd::CHAT_TYPE_SYSTEM;
		send.dwSysInfoType = type;//系统消息的类型
		send.dwChatTime = ctv.sec();
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);

		pUser->sendCmdToMe(&send,sizeof(send));
		return true;
	}
	return false;
}

/**
 * \brief 发送人气数消息
 *
 *
 * \param pUser: 要求发送消息的用户
 * \param type: 系统消息类型
 * \param gold: 消息显示的人气数
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendMoods(SceneUser *pUser, int type, DWORD moods , const char *pattern, ...)
{
	if (pUser)
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);

		//人气数算法
		if (moods)
		{
			if(moods)
			{
				sprintf(buf+strlen(buf) , "%d点" , moods);
			}
		}
		else
		{
			sprintf(buf+strlen(buf) , "%d点" , moods);
		}

		zRTime ctv;
		Cmd::stChannelChatUserCmd send;
		send.dwType=Cmd::CHAT_TYPE_SYSTEM;
		send.dwSysInfoType = type;//系统消息的类型
		send.dwChatTime = ctv.sec();
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);

		pUser->sendCmdToMe(&send,sizeof(send));
		return true;
	}
	return false;
}

/**
 * \brief 发送爱心数消息
 *
 *
 * \param pUser: 要求发送消息的用户
 * \param type: 系统消息类型
 * \param gold: 消息显示的爱心数
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendFlloves(SceneUser *pUser, int type, DWORD flloves , const char *pattern, ...)
{
	if (pUser)
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);

		//鲜花数算法
		if (flloves)
		{
			if(flloves)
			{
				sprintf(buf+strlen(buf) , "%d颗" , flloves);
			}
		}
		else
		{
			sprintf(buf+strlen(buf) , "%d颗" , flloves);
		}

		zRTime ctv;
		Cmd::stChannelChatUserCmd send;
		send.dwType=Cmd::CHAT_TYPE_SYSTEM;
		send.dwSysInfoType = type;//系统消息的类型
		send.dwChatTime = ctv.sec();
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);

		pUser->sendCmdToMe(&send,sizeof(send));
		return true;
	}
	return false;
}

/**
 * \brief 发送系统银子消息
 *
 *
 * \param pUser: 要求发送消息的用户
 * \param type: 系统消息类型
 * \param money: 消息显示的银子数量
 * \param pattern: 内容
 * \return 发送是否成功
 */
bool Channel::sendMoney(SceneUser *pUser, int type, DWORD money , const char *pattern, ...)
{
	if (pUser)
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);

		//银子换算
		if (money)
		{
			if(money/10000 > 0)
			{
				sprintf(buf+strlen(buf) , "%d锭" , money/10000);
				money = money % 10000;
			}

			if(money/100 > 0)
			{
				sprintf(buf+strlen(buf) , "%d两" , money/100);
				money = money % 100;
			}

			if(money)
			{
				sprintf(buf+strlen(buf) , "%d文" , money);
			}
		}
		else
		{
			sprintf(buf+strlen(buf) , "%d文" , money);
		}

		zRTime ctv;
		Cmd::stChannelChatUserCmd send;
		send.dwType=Cmd::CHAT_TYPE_SYSTEM;
		send.dwSysInfoType = type;//系统消息的类型
		send.dwChatTime = ctv.sec();
		bzero(send.pstrName, sizeof(send.pstrName));
		bzero(send.pstrChat, sizeof(send.pstrChat));
		strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);

		pUser->sendCmdToMe(&send,sizeof(send));
		return true;
	}
	return false;
}

/**
 * \brief 私聊
 *
 *
 * \param pUser: 请求发送者
 * \param toName: 目标用户名
 * \param content: 发送内容
 * \return 是否成功
 */
bool Channel::sendPrivate(const char * fromName,const char *toName,const char *pattern, ...)
{
	if (!fromName || !toName || !pattern) return false;
	char buf[MAX_CHATINFO];
	getMessage(buf,MAX_CHATINFO,pattern);

	SceneUser *pToUser=SceneUserManager::getMe().getUserByName((char *)toName);
	//Zebra::logger->debug("私了");
	if(0==strncmp(pToUser->name, fromName, MAX_NAMESIZE))
	{
		Zebra::logger->alarm("%s(%ld)跟自己聊天",pToUser->name,pToUser->id);
		return false;
	}

	zRTime ctv;
	Cmd::stChannelChatUserCmd send;
	send.dwType=Cmd::CHAT_TYPE_PRIVATE;
	send.dwCharType = 1;
	send.dwChatTime = ctv.sec();
	strncpy((char *)send.pstrChat,(char *)buf,MAX_CHATINFO-1);
	strncpy((char *)send.pstrName, fromName,MAX_NAMESIZE);

	sendCmdByTempID(pToUser->tempid, &send, sizeof(send));
	return true;
}

/**
 * \brief 私聊
 *
 *
 * \param pUser: 请求发送者
 * \param toName: 目标用户名
 * \param content: 发送内容
 * \return 是否成功
 */
bool Channel::sendPrivate(SceneUser *pUser,const char *toName,const char *pattern, ...)
{
	if (0==pUser) return false;
	char buf[MAX_CHATINFO];
	getMessage(buf,MAX_CHATINFO,pattern);

	SceneUser *pToUser=SceneUserManager::getMe().getUserByName((char *)toName);
	//Zebra::logger->debug("私了");
	if(pToUser==pUser)
	{
		Zebra::logger->alarm("%s(%ld)跟自己聊天",pUser->name,pUser->id);
		return false;
	}

	if (strstr(toName, "GM")||strstr(toName, "gm"))
	{
		//发送到GM工具
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::GmTool::t_Chat_GmTool *cmd=(Cmd::GmTool::t_Chat_GmTool *)buf;
		bzero(buf, sizeof(buf));
		constructInPlace(cmd);

		strncpy(cmd->userName, pUser->name, MAX_NAMESIZE);
		strncpy(cmd->gmName, toName, MAX_NAMESIZE);
		cmd->countryID = pUser->scene->getCountryID();
		cmd->sceneID = pUser->scene->getRealMapID();
		cmd->dwType = Cmd::CHAT_TYPE_PRIVATE;
		strncpy(cmd->content, pattern, 255);
		cmd->size = 0;
		ScenesService::getInstance().sendCmdToSuperServer(cmd, sizeof(Cmd::GmTool::t_Chat_GmTool)+cmd->size*sizeof(Cmd::stTradeObject));
	}

	zRTime ctv;
	Cmd::stChannelChatUserCmd send;
	send.dwType=Cmd::CHAT_TYPE_PRIVATE;
	send.dwCharType = pUser->charbase.type; //修复换头像不显示昵称
	send.dwChatTime = ctv.sec();
	strncpy((char *)send.pstrChat,(char *)buf,MAX_CHATINFO-1);
	strncpy((char *)send.pstrName,pUser->name,MAX_NAMESIZE);
	//send.dwCharType = pUser->charbase.face;

	sendCmdByTempID(pUser->tempid, &send,sizeof(send));
	return true;

}

/**
 * \brief 私聊
 *
 *
 * \param pUser: 请求发送者
 * \param rev:请求指令
 * \param cmdLen: 指令长度
 * \return 发送是否成功
 */
bool Channel::sendPrivate(SceneUser *pUser,const Cmd::stChannelChatUserCmd *rev,unsigned int cmdLen)
{
	if(rev->pstrChat[0]=='/' && rev->pstrChat[1]=='/')
	{
		Gm::exec(pUser,(char *)rev->pstrChat+2);
	}
	else
	{
		if (strstr(rev->pstrName, "GM")||strstr(rev->pstrName, "gm"))
		{
			//发送到GM工具
			BYTE buf[zSocket::MAX_DATASIZE];
			Cmd::GmTool::t_Chat_GmTool *cmd=(Cmd::GmTool::t_Chat_GmTool *)buf;
			bzero(buf, sizeof(buf));
			constructInPlace(cmd);

			strncpy(cmd->userName, pUser->name, MAX_NAMESIZE);
			strncpy(cmd->gmName, rev->pstrName, MAX_NAMESIZE);
			cmd->countryID = pUser->scene->getCountryID();
			cmd->sceneID = pUser->scene->getRealMapID();
			cmd->dwType = Cmd::CHAT_TYPE_PRIVATE;
			strncpy(cmd->content, rev->pstrChat, 255);
			cmd->size = rev->size;
			if (cmd->size)
				bcopy(rev->tobject_array, cmd->tobject_array, cmd->size*sizeof(Cmd::stTradeObject));      
			ScenesService::getInstance().sendCmdToSuperServer(cmd, sizeof(Cmd::GmTool::t_Chat_GmTool)+cmd->size*sizeof(Cmd::stTradeObject));
		}

		SceneUser *pToUser=SceneUserManager::getMe().getUserByName(rev->pstrName);
		//Zebra::logger->debug("收到%s 发给 %s 的私聊消息", pUser->name, rev->pstrName);
		if(pToUser==pUser)
		{
			Zebra::logger->alarm("%s(%ld)跟自己聊天",pUser->name,pUser->id);
			return false;
		}

		//rev->dwCharType = pUser->charbase.face;

		if(pToUser)
		{
			//Zebra::logger->debug("%s vs %s",pUser->name,(char *)toName);
			strncpy((char *)rev->pstrName,pUser->name,MAX_NAMESIZE);
			if (pToUser->checkChatCmd(rev,cmdLen))
			{
				pToUser->sendCmdToMe(rev,cmdLen);
			}
			else
				Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "玩家 %s 拒绝所有私聊消息", pToUser->name);
			//pUser->sendCmdToMe(&send,sizeof(send));
			return true;
		}
		else
		{
			char buf[zSocket::MAX_DATASIZE];
			Cmd::Session::t_privateChat_SceneSession * send = (Cmd::Session::t_privateChat_SceneSession *)buf;
			constructInPlace(send);

			send->err_code = 0;
			strncpy((char *)send->src_name, pUser->name, MAX_NAMESIZE-1);
			strncpy((char *)send->dst_name, (char *)rev->pstrName, MAX_NAMESIZE-1);
			strncpy((char *)rev->pstrName,pUser->name,MAX_NAMESIZE);
			bcopy(rev, send->chat_cmd, cmdLen);
			send->cmd_size = cmdLen;

			sessionClient->sendCmd(send, sizeof(Cmd::Session::t_privateChat_SceneSession)+cmdLen);
			return true;
		}
	}
	return true;
}

/**
 * \brief 唯一实例
 *
 */
ChannelM *ChannelM::cm(NULL);
ChannelM::ChannelM()
{
	channelUniqeID =new zUniqueDWORDID(1000);
}

ChannelM::~ChannelM()
{
	SAFE_DELETE(channelUniqeID);
}

/**
 * \brief 得到一个唯一编号
 *
 *
 * \param tempid: 唯一编号(输出)
 * \return 得到的编号是否唯一
 */
bool ChannelM::getUniqeID(DWORD &tempid)
{
	tempid=channelUniqeID->get();
	return (tempid!=channelUniqeID->invalid());
}

/**
 * \brief 收回一个唯一编号
 *
 *
 * \param tempid: 收回的编号
 */
void ChannelM::putUniqeID(const DWORD &tempid)
{
	channelUniqeID->put(tempid);
}

/**
 * \brief 得到唯一实例
 *
 *
 * \return  唯一实例
 */
ChannelM &ChannelM::getMe()
{
	if(cm==NULL)
		cm=new ChannelM();
	return *cm;
}

/**
 * \brief 删除唯一实例
 *
 *
 */
void ChannelM::destroyMe()
{
	SAFE_DELETE(cm);
}

/**
 * \brief 增加聊天频道
 *
 *
 * \param channel: 频道
 * \return 增加是否成功
 */
bool ChannelM::add(Channel *channel)
{
	bool ret;
	mlock.lock();
	ret=addEntry(channel);
	mlock.unlock();
	return ret;
}

/**
 * \brief 从聊天频道中删除一个用户
 *
 *
 * \param name: 用户名
 */
void ChannelM::removeUser(const char *name)
{
	// 清理所有频道里的name用户
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		Channel *temp=(Channel *)it->second;
		temp->remove(name);
	}
}

/**
 * \brief 从聊天频道中删除一个聊天频道
 *
 *
 * \param dwChannelID: 频道id
 */
void ChannelM::remove(DWORD dwChannelID)
{
	Channel *ret=NULL;
	mlock.lock();
	ret=(Channel *)getEntryByTempID(dwChannelID);
	removeEntry(ret);
	mlock.unlock();
}

/**
 * \brief 得到一个聊天频道
 *
 *
 * \param dwChannelID: 频道id
 * \return 聊天频道
 */
Channel *ChannelM::get(DWORD dwChannelID)
{
	Channel *ret=NULL;
	mlock.lock();
	ret=(Channel *)getEntryByTempID(dwChannelID);
	mlock.unlock();
	return ret;
}

/**
 * \brief 将16进制数转为10进制
 *
 *
 * \param ch: 16进制字符
 * \return 转换猴的10进制数
 */
BYTE hexchtoi(char &ch)
{
	if (ch>='0' && ch <='9')
	{
		return BYTE(ch-'0');
	}
	else if ((ch >='A' && ch <='F')||(ch >='a' && ch <='f'))
	{
		return BYTE(toupper(ch)-'A')+10;
	}
	return 0;
}

/*\brief 对要发送的聊天消息进行预先检查
 *如果是//开头的特殊指令则执行指令
 *如果是普通聊天消息则检查角色是否被禁言
 *
 * \param pUser: 用户
 * \param cchat: 聊天指令
 * \param cmdLen:指令长度
 *\return 该消息是否可以被发出
 * 
 */
bool Channel::doPreCheck(SceneUser *pUser, Cmd::stChannelChatUserCmd *cchat, unsigned int cmdLen)
{
	//先测试GM指令
	if(cchat->pstrChat[0]=='/' && cchat->pstrChat[1]=='/')
	{
		Gm::exec(pUser,(char *)cchat->pstrChat+2);
		return false;
	}

	//检查是否被禁言
	if (pUser->isForbidTalk())
	{
		zRTime now;
		int delay = pUser->charbase.forbidtalk - now.sec();
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你已经被禁言，%d秒后解除",delay);
		return false;
	}

	if (pUser->getPriv()>Gm::normal_mode)
		ScenesService::gm_logger->trace("GM %s:%s", pUser->name, cchat->pstrChat);
	return true;
}

/**
 * \brief 解析聊天指令
 *
 *
 * \param rev: 收到的指令
 * \param cmdLen: 收到的指令长度
 * \return 是否识别该指令
 */
bool SceneUser::doChatCmd(const Cmd::stChatUserCmd *rev,unsigned int cmdLen)
{
	using namespace Cmd;

	if (this->isQuiz)
	{
		return true;
	}

	switch(rev->byParam)
	{
		case DIRTY_USER_USERCMD_PARA://举报外挂
			{
				stDirtyUserUserCmd *cmd = (stDirtyUserUserCmd *)rev;
				switch (cmd->type)
				{
					case DIRTYUSER_DLL_CALL:
						{
							ScenesService::wg_logger->trace("[举报外挂]%s,%u,%u,%u,%s,%u,%s", 
											name, id, accid, charbase.level, 
											SceneManager::getInstance().getCountryNameByCountryID(charbase.country), 
											charbase.gold, "外部DLL调用本进程函数，一般外挂用户");
						}
						break;
					case DIRTYUSER_DEBUGGER:
						{
							ScenesService::wg_logger->trace("[举报外挂]%s,%u,%u,%u,%s,%u,%s", 
											name, id, accid, charbase.level, 
											SceneManager::getInstance().getCountryNameByCountryID(charbase.country), 
											charbase.gold, "启用调试器调试本进程，可能是外挂制作者");
						}
						break;
					default:
						{
							ScenesService::wg_logger->trace("[举报外挂]%s,%u,%u,%u,%s,%u,%s", 
											name, id, accid, charbase.level, 
											SceneManager::getInstance().getCountryNameByCountryID(charbase.country), 
											charbase.gold, "客户端检测到玩家使用外挂");
						}
						break;
				}

				if (!dropTime)
				{
						DWORD t = zMisc::randBetween(180, 600);
						dropTime = SceneTimeTick::currentTime.sec() + t;
						Zebra::logger->debug("%s 使用外挂，%u 秒后断线", name, t);
				}

				return true;
			}
			break;
		case ALL_CHAT_USERCMD_PARAMETER:
			{
				char buf[zSocket::MAX_DATASIZE];
				stChannelChatUserCmd *cchat = NULL;
				unsigned int curCmdLen = cmdLen;
				zRTime ctv;

				stChannelChatUserCmd *temp=(stChannelChatUserCmd *)rev;
				temp->dwChatTime = ctv.sec();
				temp->dwCharType = charbase.type; //修复换头像不显示昵称
				if (this->tosh)
				{
					static const char charTable[] = "0123456789~!@#$%^&*(){}<>?.";
					static const int charTable_length = strlen(charTable);
					int i = 0;
					for(; i < MAX_CHATINFO/2; i++)
					{
						temp->pstrChat[i] = charTable[zMisc::randBetween(0, charTable_length - 1)];
					}
					temp->pstrChat[i]=0;
				}

				if (temp->size>0) 
				{
					/*DWORD tempLen = sizeof(struct Cmd::stChannelChatUserCmd);
					  DWORD itemNum;
					  if (cmdLen>tempLen)
					  itemNum = (cmdLen-sizeof(struct Cmd::stChannelChatUserCmd))/sizeof(DWORD);
					  else
					  itemNum = 0;
					 */

					//					Zebra::logger->debug("[*]镶嵌物品数量%u cmdLen=%u struct size=%u",itemNum,cmdLen,sizeof(struct Cmd::stChannelChatUserCmd));
					DWORD itemNum = temp->size;
					cchat = (stChannelChatUserCmd *)buf;
					memcpy(cchat, rev, cmdLen);
					stTradeObject *tempObject = cchat->tobject_array;

					cchat->size =0;
					for(DWORD i=0; i<itemNum; i++)
					{
						zObject *itemobj = packs.uom.getObjectByThisID(
								temp->tobject_array[i].dwObjectTempID);

						if (itemobj)
						{
							tempObject->dwObjectID = itemobj->data.dwObjectID;
							strncpy(tempObject->strName, itemobj->data.strName, MAX_NAMESIZE);
							tempObject->upgrade = itemobj->data.upgrade;
							tempObject->kind = itemobj->data.kind;

							tempObject++;
							cchat->size++;
						}
					}
					curCmdLen = sizeof(stChannelChatUserCmd)+cchat->size*sizeof(stTradeObject);
					/*
					   if (cchat->size >0)
					   {
					   bool sendok = false;

					   DWORD pay = 0;
					   switch(temp->dwType)
					   {
					   case CHAT_TYPE_PRIVATE:			/// 私聊频道
					   pay = SHELLITEM_IN_PRIVATE;
					   break;
					   case CHAT_TYPE_NINE:			/// 轻聊频道
					   pay = SHELLITEM_IN_NINE;
					   break;
					   case CHAT_TYPE_TEAM:			/// 队伍频道
					   {
					   if (this->team.IsTeamed())
					   {
					   pay = SHELLITEM_IN_TEAM;
					   }
					   else
					   {
					   pay = 0;
					   }
					   }
					   break;
					   case CHAT_TYPE_FRIEND:			/// 好友频道
					   pay = SHELLITEM_IN_FRIEND;
					   break;
					   case CHAT_TYPE_UNION:			/// 帮会频道
					   {
					   if (this->charbase.unionid > 0)
					   {
					   pay = SHELLITEM_IN_UNION;
					   }
					   else
					   {
					   pay = 0;
					   }
					   }
					   break;
					   case CHAT_TYPE_PERSON:			/// 个人频道
					   pay = SHELLITEM_IN_PERSON;
					   break;
					   case CHAT_TYPE_COUNTRY:			/// 国家频道
					   {
					   if (this->getLevel() >= 25)
					   {
					   pay = SHELLITEM_IN_COUNTRY;
					   }
					   else
					   {
					   pay = 0;
					   }
					   }
					   break;
					   case CHAT_TYPE_OVERMAN:			/// 师门频道
					   {
					   pay = SHELLITEM_IN_ZONE;
					//pay = 0;
					}
					break;
					case CHAT_TYPE_FAMILY:			/// 家族频道
					{
					if (this->charbase.septid > 0)
					{
					pay = SHELLITEM_IN_SEPT_TOP;
					}
					else
					{
					pay = 0;
					}
				}
				break;
				}

				if (packs.checkMoney(pay) && packs.removeMoney(pay,"广告费")) {
					sendok = true; /// 付钱成功
					Channel::sendMoney(this, Cmd::INFO_TYPE_GAME, pay, "花掉了广告费用");
				}

				if (sendok)
				{
					curCmdLen = sizeof(stChannelChatUserCmd)+cchat->size*sizeof(t_Object);
				}
				else
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你没有足够的银子来支付物品信息广告费用！");
					cchat->size = 0;
					curCmdLen = sizeof(stChannelChatUserCmd);
				}
				}
				*/
				}
				else
				{
					cchat = (stChannelChatUserCmd *)rev;
				}

				//测试外挂举报
				if(cchat->pstrChat[0]==125 && cchat->pstrChat[1]==127)
				{
					for (DWORD i=2; i<strlen(cchat->pstrChat) && i<MAX_CHATINFO-1-2; i++)
					{
						if (0==i%2)
							cchat->pstrChat[i] ^= 125;
						else
							cchat->pstrChat[i] ^= 127;
					}
					ScenesService::wg_logger->trace("[菲菲]%s,%u,%u,%u,%s,%u,%s", 
									name, id, accid, charbase.level, 
									SceneManager::getInstance().getCountryNameByCountryID(charbase.country), 
									charbase.gold, &cchat->pstrChat[2]);
					return true;
				}

				switch(cchat->dwType)
				{
					case CHAT_TYPE_MINIGAME:
						{
							if (miniGame)
								if (Channel::doPreCheck(this, cchat, curCmdLen))
								{
									strncpy(cchat->pstrName, name, MAX_NAMESIZE); // 防止外挂作弊伪装他人说话
									miniGame->sendCmdToAll(cchat, curCmdLen);
								}
						}
						break;
					case CHAT_TYPE_EMOTION://表情
					case CHAT_TYPE_NINE:
					case CHAT_TYPE_SHOPADV: //摆摊广告
						{
							if (Channel::doPreCheck(this, cchat, curCmdLen))
							{
								//检查是否隐身
								if (SceneEntry_Hide==getState()&&!this->hideme)
								{
									Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "隐身状态下不能轻聊");
									return false;
								}
								strncpy(cchat->pstrName, this->name, MAX_NAMESIZE); // 防止外挂作弊伪装他人说话
								bool ret=Channel::sendNine(this, cchat, curCmdLen);
								return ret; 
							}
						}
						break;
					case CHAT_TYPE_PRIVATE:	///私聊
					case CHAT_TYPE_WHISPER:	/// 悄悄话
						{
							if (Channel::doPreCheck(this, cchat, curCmdLen))
								return Channel::sendPrivate(this, cchat, curCmdLen);
						}
						break;
					case CHAT_TYPE_TEAM:				/// 队伍频道
						{
							if (Channel::doPreCheck(this, cchat, curCmdLen))
							{
								strncpy(cchat->pstrName, this->name, MAX_NAMESIZE); // 防止外挂作弊伪装他人说话
								if(this->team.IsTeamed())
								{
									return Channel::sendTeam(this->team.getLeader() , cchat, curCmdLen);
								}
								else
								{
									Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "没有队伍,不能进行队伍聊天");
								}
							}
						}
						break;
					case CHAT_TYPE_GM:
					case CHAT_TYPE_SYTEM:
						break;
					case CHAT_TYPE_WORLD:	///世界频道
						{
							if (charbase.level<60)
							{
								Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您不满60级，不可以使用世界频道聊天");
								return true;
							}
							if (Channel::doPreCheck(this, cchat, curCmdLen))
							{
								if (!packs.checkMoney(1000)
										|| !packs.removeMoney(1000,"世界频道花费"))
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的银子不足");
									return true;
								}
								Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "你向世界人民呐喊，花费银子10两.");

                                //soke 皇帝、女皇世界说话显示称号
                            //  if (this->emperor)
							//	{
							//	  cchat->dwSysInfoType = Cmd::INFO_TYPE_EMPEROR;
							//	}
                                //soke 皇夫、皇后世界说话显示称号
								if (this->kingConsort == 2)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_CONSORZ;
								}

								//soke 宋国玩家世界说话
								if (charbase.country==2)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_SONGGUO;
								}
								//soke 魏国玩家世界说话
								if (charbase.country==3)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_WEIGUO;
								}
								//soke 齐国玩家世界说话
								if (charbase.country==4)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_QIGUO;
								}
								//soke 燕国玩家世界说话
								if (charbase.country==5)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_YANGUO;
								}
								//soke 唐国玩家世界说话
								if (charbase.country==7)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_TANGGUO;
								}
								//soke 汉国玩家世界说话
								if (charbase.country==8)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_HANGUO;
								}
								//soke 吴国玩家世界说话
								if (charbase.country==9)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_WUGUO;
								}
								//soke 赵国玩家世界说话
								if (charbase.country==10)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_ZHAOGUO;
								}
								//soke 周国玩家世界说话
								if (charbase.country==11)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_ZHOUGUO;
								}
								//soke 楚国玩家世界说话
								if (charbase.country==12)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_CHUGUO;
								}
								
								if (checkUserCmd(cchat, curCmdLen))
								{
									forwardSession(cchat,curCmdLen);
								}
							}
						}
						break;
					case CHAT_TYPE_COUNTRY:			/// 国家频道
						{
							if (Channel::doPreCheck(this, cchat, curCmdLen))
							{
								if (charbase.level<25)
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您不满25级，不可以使用国家频道聊天");
									return true;
								}

                                //soke 国王、女王国家说话显示称号
								if (this->king && !this->emperor)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_KING;
								}
								//soke 王夫、王后国家说话显示称号
								if (this->kingConsort == 1)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_CONSORT;
								}
								//soke 巡捕(左上)说话显示称号
								if (this->isCatcherState() && !this->kingConsort == 1)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_CATCHER;
								}
								//soke 巡捕(左下)说话显示称号
								if (this->isCatchexState() && !this->kingConsort == 1)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_CATCHEX;
								}
								//soke 御史大夫(右上)说话显示称号
								if (this->isCensorState() && !this->kingConsort == 1)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_CENSOR;
								}
								//soke 御史大夫(右下)说话显示称号
								if (this->isCensosState() && !this->kingConsort == 1)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_CENSOS;
								}
								//soke 元帅说话显示称号
								if (this->isYuanshuaiState() && !this->kingConsort == 1)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_YUANSHUAI;
								}
								//soke 宰相说话显示称号
								if (this->isZaixiangState() && !this->kingConsort == 1)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_ZAIXIANG;
								}
								//soke 征途宝贝显示称号
								if (charbase.flower3time > 0)
								{
									if ( 2 == charbase.type ) //性别判断
                                    {
								        cchat->dwSysInfoType = Cmd::INFO_TYPE_ZTBB;
									}
								}
								
								//soke VIP1玩家说话
								/*if (charbase.levelvip>0 && charbase.levelvip<=5)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_VIP1;
								}
								//soke VIP2玩家说话
								if (charbase.levelvip>6 && charbase.levelvip<=10)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_VIP2;
								}
								//soke VIP3玩家说话
								if (charbase.levelvip>10 && charbase.levelvip<=15)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_VIP3;
								}
								//soke VIP4玩家说话
								if (charbase.levelvip>15 && charbase.levelvip<=20)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_VIP4;
								}
								//soke VIP5玩家说话
								if (charbase.levelvip>20)
								{
								  cchat->dwSysInfoType = Cmd::INFO_TYPE_VIP5;
								}*/

								strncpy(cchat->pstrName, this->name, MAX_NAMESIZE); // 防止外挂作弊伪装他人说话
								forwardSession(cchat,curCmdLen);
								return true;
							}
						}
						break;
					case CHAT_TYPE_AREA:			/// 现在是区域聊天
						{
							strncpy(cchat->pstrName, this->name, MAX_NAMESIZE); // 防止外挂作弊伪装他人说话
							if (Channel::doPreCheck(this, cchat, curCmdLen))
							{
								scene->sendCmdToScene(cchat, curCmdLen);

								//发送到GM工具
								BYTE buf[zSocket::MAX_DATASIZE];
								Cmd::GmTool::t_Chat_GmTool *cmd=(Cmd::GmTool::t_Chat_GmTool *)buf;
								bzero(buf, sizeof(buf));
								constructInPlace(cmd);

								strncpy(cmd->userName, name, MAX_NAMESIZE);
								cmd->countryID = scene->getCountryID();
								cmd->sceneID = scene->getRealMapID();
								cmd->dwType = cchat->dwType;
								strncpy(cmd->content, cchat->pstrChat, 255);
								cmd->size = cchat->size;
								if (cmd->size)
									bcopy(cchat->tobject_array, cmd->tobject_array, cmd->size*sizeof(Cmd::stTradeObject));
								ScenesService::getInstance().sendCmdToSuperServer(cmd, sizeof(Cmd::GmTool::t_Chat_GmTool)+cmd->size*sizeof(Cmd::stTradeObject));
								return true;
							}
						}
						break;
					case CHAT_TYPE_OVERMAN_AFFICHE:	/// 师门公告
					case CHAT_TYPE_OVERMAN:			/// 师门频道
					case CHAT_TYPE_FAMILY_AFFICHE:	/// 家族公告
					case CHAT_TYPE_FAMILY:			/// 家族频道
						//strncpy(cchat->pstrName, this->name, MAX_NAMESIZE); // 防止外挂作弊伪装他人说话
					case CHAT_TYPE_FRIEND_AFFICHE:	/// 好友公告
					case CHAT_TYPE_UNION_AFFICHE:	/// 帮会公告
					case CHAT_TYPE_FRIEND:			/// 好友频道
					case CHAT_TYPE_UNION:			/// 帮会频道
					case CHAT_TYPE_PERSON:		//个人频道
						strncpy(cchat->pstrName, this->name, MAX_NAMESIZE); // 防止外挂作弊伪装他人说话
						// 私聊填的是对方名字所以不覆盖。
					case CHAT_TYPE_FRIEND_PRIVATE:	/// 好友私聊
					case CHAT_TYPE_UNION_PRIVATE:	/// 帮会私聊
					case CHAT_TYPE_FAMILY_PRIVATE:	/// 家族私聊
					case CHAT_TYPE_OVERMAN_PRIVATE:	/// 师门私聊
						{
							if (Channel::doPreCheck(this, cchat, curCmdLen))
								if (checkUserCmd(cchat, curCmdLen))
									forwardSession(cchat,curCmdLen);
						}
						break;
				}
			}
			break;
			//解散队伍
		case REMOVE_TEAM_USERCMD_PARA:
			break;
			//添加队员,刷新队员数据指令
		case ADD_TEAMMEMBER_USERCMD_PARA:
			break;
			//删除队员指令
		case REMOVE_TEAMMEMBER_USERCMD_PARA:
			{
				//Zebra::logger->debug("收到删除队员指令(%s , %ld)" , this->name , this->id);
				stRemoveTeamMemberUserCmd *rem = (stRemoveTeamMemberUserCmd *)rev;

				SceneUser *leader=SceneUserManager::getMe().getUserByTempID(rem->dwTeamID);
				if(leader)
				{
					//如果是队长则解散队伍
					if(rem->dwTempID == leader->tempid)
					{
						leader->team.deleteTeam(leader);
					}
					else
					{
						//如果是队长踢人
						if(leader == this)
						{
							leader->team.kickoutMember(leader , (stRemoveTeamMemberUserCmd *)rev);
						}
						else
						{
							if(rem->dwTempID == this->tempid)
							{
								this->team.setLeader(0);
								leader->team.removeMember(leader , (stRemoveTeamMemberUserCmd *)rev);
							}
						}
						if(leader->team.getSize() == 1)
						{
							leader->team.deleteTeam(leader);
						}
					}
				}
				return true;
			}
			break;
			//请求设置物品分配模式
		case OBJMODE_TEAMMEMBER_USERCMD_PARA:
			{
				stObjModeTeamMemberUserCmd *emt = (stObjModeTeamMemberUserCmd *)rev;
				if(this->tempid != this->team.getLeader() || this->team_mode != Cmd::TEAM_NORMAL)
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "物品分配模式设置失败");
				}
				/**
				 * \brief 设置经验分配模式
				 *
				 */
				struct ObjModeTeamExec : public TeamMemExec
				{
					BYTE obj_mode;
					stObjModeTeamMemberUserCmd ret; 
					ObjModeTeamExec(BYTE type):obj_mode(type)
					{
						ret.byType=type;
					}
					bool exec(TeamMember &member)
					{
						SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(member.tempid);
						if(pUser)
						{
							pUser->team.setObjMode(obj_mode);
							pUser->sendCmdToMe(&ret , sizeof(ret));
						}
						return true;
					}

				};
				ObjModeTeamExec exec(emt->byType);
				this->team.execEveryOne(exec); 
				return true;
			}
			break;
			//请求设置经验分配模式
		case EXPMODE_TEAMMEMBER_USERCMD_PARA:
			{
				stExpModeTeamMemberUserCmd *emt = (stExpModeTeamMemberUserCmd *)rev;
				if(this->tempid != this->team.getLeader() || this->team_mode != Cmd::TEAM_NORMAL)
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "经验分配模式设置失败");
				}
				/**
				 * \brief 设置经验分配模式
				 *
				 */
				struct ExpModeTeamExec : public TeamMemExec
				{
					BYTE exp_mode;
					stExpModeTeamMemberUserCmd ret; 
					ExpModeTeamExec(BYTE type):exp_mode(type)
					{
						ret.byType=type;
					}
					bool exec(TeamMember &member)
					{
						SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(member.tempid);
						if(pUser)
						{
							pUser->team.setExpMode(exp_mode);
							pUser->sendCmdToMe(&ret , sizeof(ret));
						}
						return true;
					}

				};
				ExpModeTeamExec exec(emt->byType);
				this->team.execEveryOne(exec); 
				return true;
			}
			break;
			//请求组队指令
		case REQUEST_NAME_TEAM_USERCMD_PARA:
			{
				//Zebra::logger->debug("收到邀请队员指令(%s , %ld)" , this->name , this->id);
				stRequestNameTeamUserCmd *req = (stRequestNameTeamUserCmd *)rev;
				stRequestNameTeamUserCmd ret;
				strncpy(ret.byAnswerUserName,req->byAnswerUserName,MAX_NAMESIZE);
				if(this->scene->noTeam())
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "本地图不能组队");
					return true;
				}
				if(this->team.IsFull())
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你的组队已满");
					return true;
				}
				if((this->team.getLeader() != 0) && (this->tempid != this->team.getLeader()))
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你无权邀请队员");
					return true;
				}
				if(strncmp(this->name , ret.byAnswerUserName,MAX_NAMESIZE) == 0)
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "不能邀请自己");
					return true;
				}

				if (this->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER)
						|| issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你处于护宝状态不能组队");
					return true;
				}

				SceneUser *pUser = SceneUserManager::getMe().getUserByName(req->byAnswerUserName);
				if(pUser)
				{
					//设置组队模式
                    //桃子 处理本国和敌国人可组队问题
				    if(pUser->charbase.country != this->charbase.country )
				        {
				     	   Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "不能邀请不属于同国家的人组队");
					       return true;
				        }
                       //结束						
					if(!this->team.IsTeamed())
					{
						if(req->dwTeamMode < Cmd::TEAM_MAX)
						{
							this->team_mode=req->dwTeamMode;
						}
						else
						{
							Zebra::logger->iffy("收到无效模式的邀请队员指令(%s , %ld)" , this->name , this->id);
							return true;
						}
					}
					else
					{
						if(this->team_mode != req->dwTeamMode)
						{
							Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "一个队伍只能使用一种组队模式");
							return true;
						}
					}
					if(this->team_mode == Cmd::TEAM_HONOR) 
					{
						if(this->charbase.level < TEAM_HONOR_LEADER_LEVEL)
						{
							Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "荣誉模式需要队长等级%d级",TEAM_HONOR_LEADER_LEVEL);
							return true;
						}
						if(pUser->charbase.level >= TEAM_HONOR_MEMBER_LEVEL)
						{
							Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "荣誉模式队员等级不能大于%d级",TEAM_HONOR_MEMBER_LEVEL);
							return true;
						}
						if(this->charbase.level <= pUser->charbase.level)
						{
							Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "荣誉模式只能组比自己等级低的玩家");
							return true;
						}
					}
					if(!pUser->team.IsOpen())
					{
						Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "对方组队未开启");
						return true;
					}
					if(pUser->team.IsTeamed())
					{
						Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "对方已有组队");
						return true;
					}

					if (pUser->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER)
					|| pUser->issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对方处于护宝状态,不能组队");
						return true;
					}

					strncpy(ret.byAnswerUserName,this->name,MAX_NAMESIZE);
					ret.dwTeamMode = this->team_mode;
					pUser->sendCmdToMe(&ret , sizeof(ret));
					return true;
				}
			}
			break;
			//请求组队指令
		case REQUEST_TEAM_USERCMD_PARA:
			{
				//Zebra::logger->debug("收到邀请队员指令(%s , %ld)" , this->name , this->id);
				stRequestTeamUserCmd *req = (stRequestTeamUserCmd *)rev;
				stRequestTeamUserCmd ret;
				ret.dwAnswerUserID = req->dwAnswerUserID;
				if(this->scene->noTeam())
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "本地图不能组队");
					return true;
				}
				if(this->team.IsFull())
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你的组队已满");
					return true;
				}
				if((this->team.getLeader() != 0) && (this->tempid != this->team.getLeader()))
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你无权邀请队员");
					return true;
				}
				if(this->tempid == ret.dwAnswerUserID)
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "不能邀请自己");
					return true;
				}

				if (this->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER)
						|| this->issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你处于护宝状态不能组队");
					return true;
				}

				SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(ret.dwAnswerUserID);
				if(pUser)
				{
					//设置组队模式
					if(!this->team.IsTeamed())
					{
						if(req->dwTeamMode < Cmd::TEAM_MAX)
						{
							this->team_mode=req->dwTeamMode;
						}
						else
						{
							Zebra::logger->iffy("收到无效模式的邀请队员指令(%s , %ld)" , this->name , this->id);
							return true;
						}
					}
					else
					{
						if(this->team_mode != req->dwTeamMode)
						{
							Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "一个队伍只能使用一种组队模式");
							return true;
						}
					}
					if(this->team_mode == Cmd::TEAM_HONOR) 
					{
						if(this->charbase.level < TEAM_HONOR_LEADER_LEVEL)
						{
							Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "荣誉模式需要队长等级%d级",TEAM_HONOR_LEADER_LEVEL);
							return true;
						}
						if(pUser->charbase.level >= TEAM_HONOR_MEMBER_LEVEL)
						{
							Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "荣誉模式队员等级不能大于%d级",TEAM_HONOR_MEMBER_LEVEL);
							return true;
						}
						if(this->charbase.level <= pUser->charbase.level)
						{
							Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "荣誉模式只能组比自己等级低的玩家");
							return true;
						}
					}
					if(!pUser->team.IsOpen())
					{
						Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "对方组队未开启");
						return true;
					}
					if(pUser->team.IsTeamed())
					{
						Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "对方已有组队");
						return true;
					}

					if (pUser->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER)
					|| pUser->issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对方处于护宝状态,不能组队");
						return true;
					}

					ret.dwAnswerUserID = this->tempid;
					ret.dwTeamMode = this->team_mode;
					pUser->sendCmdToMe(&ret , sizeof(ret));
					return true;
				}
			}
			break;
			//回答组队指令
		case ANSWER_NAME_TEAM_USERCMD_PARA:
			{
				Zebra::logger->debug("收到回答组队指令(%s , %ld)" , this->name , this->id);
				stAnswerNameTeamUserCmd *ans = (stAnswerNameTeamUserCmd *)rev;
				if(this->scene->noTeam())
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "本地图不能组队");
					return true;
				}
				if(this->team.IsTeamed())
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "只能加入一个队伍");
					return true;
				}
				if(ans->byAgree)
				{
					SceneUser *leader = SceneUserManager::getMe().getUserByName(ans->byRequestUserName);
					if(leader)
					{
						if(leader->scene->noTeam())
						{
							Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "队长所在地图不能组队");
							return true;
						}
						stAnswerTeamUserCmd temp;
						temp.dwRequestUserID = leader->tempid;
						temp.dwAnswerUserID = this->tempid;
						temp.byAgree = ans->byAgree;
						temp.dwTeamMode = ans->dwTeamMode;
						//TODO添加队员
						//如果是新队伍
						if(leader->team.getSize() == 0)
						{
							if(leader->team.IsTeamed())
							{
								Channel::sendSys(this ,Cmd::INFO_TYPE_FAIL,"组队失败,队长已经撤销组队请求");
								return true;
							}
							leader->team_mode = ans->dwTeamMode;
							if(leader->team.addNewLeader(leader))
							{
								if(!leader->team.addNewMember(leader , (stAnswerTeamUserCmd *)(&temp)))
								{
									//TODO取消组队
									leader->team.removeTeam(leader , leader->tempid);
								}
							}
						}
						else
						{
							if(ans->dwTeamMode != leader->team_mode)
							{
								Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "队长取消了对你的组队邀请");
								return true;
							}
							if(leader->team.IsFull())
							{
								Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "组队失败,队伍已满");
								return true;
							}
							leader->team.addNewMember(leader , (stAnswerTeamUserCmd *)(&temp));
						}
					}
				}
				else
				{
					SceneUser *pUser = SceneUserManager::getMe().getUserByName(ans->byRequestUserName);
					if(pUser)
					{
						Channel::sendSys(pUser , Cmd::INFO_TYPE_FAIL, "对方拒绝了你的邀请");
						return true;
					}
				}
			}
			break;
			//回答组队指令
		case ANSWER_TEAM_USERCMD_PARA:
			{
				//Zebra::logger->debug("收到回答组队指令(%s , %ld)" , this->name , this->id);
				stAnswerTeamUserCmd *ans = (stAnswerTeamUserCmd *)rev;
				if(this->scene->noTeam())
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "本地图不能组队");
					return true;
				}
				if(this->team.IsTeamed())
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "只能加入一个队伍");
					return true;
				}
				if(ans->byAgree)
				{
					SceneUser *leader = SceneUserManager::getMe().getUserByTempID(ans->dwRequestUserID);
					if(leader)
					{
						if(leader->scene->noTeam())
						{
							Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "队长所在地图不能组队");
							return true;
						}
						//TODO添加队员
						//如果是新队伍
						if(leader->team.getSize() == 0)
						{
							if(leader->team.IsTeamed())
							{
								Channel::sendSys(this ,Cmd::INFO_TYPE_FAIL,"组队失败,队长已经撤销组队请求");
								return true;
							}
							leader->team_mode = ans->dwTeamMode;
							if(leader->team.addNewLeader(leader))
							{
								if(!leader->team.addNewMember(leader , (stAnswerTeamUserCmd *)rev))
								{
									//TODO取消组队
									leader->team.removeTeam(leader , leader->tempid);
								}
							}
						}
						else
						{
							if(ans->dwTeamMode != leader->team_mode)
							{
								Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "队长取消了对你的组队邀请");
								return true;
							}
							if(leader->team.IsFull())
							{
								Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "组队失败,队伍已满");
								return true;
							}
							leader->team.addNewMember(leader , (stAnswerTeamUserCmd *)rev);
						}
					}
				}
				else
				{
					SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(ans->dwRequestUserID);
					if(pUser)
					{
						Channel::sendSys(pUser , Cmd::INFO_TYPE_FAIL, "对方拒绝了你的邀请");
						return true;
					}
				}
			}
			break;
		//请求队伍坐标
		case REQUEST_TEAMMEMBER_POSITION_USERCMD_PARA:
			{
				using namespace Cmd;
				struct TeamPositionExec : public TeamMemExec
				{
					stTeamMemberPosition *data;
					int leavesize;
					int dwSize;
					TeamPositionExec(char *buf , int max)
					{
						data = (stTeamMemberPosition *)buf;
						leavesize = max;
						dwSize = 0;
					}
					bool exec(TeamMember &member)
					{
						SceneUser *pUser = SceneUserManager::getMe().getUserByID(member.id);
						if(pUser && leavesize > 0)
						{
							strncpy(data[dwSize].szName , member.name , MAX_NAMESIZE);
							strncpy(data[dwSize].szMapName , pUser->scene->name , MAX_NAMESIZE);
							data[dwSize].x = pUser->getPos().x;
							data[dwSize].y = pUser->getPos().y;
							leavesize -=sizeof(stTeamMemberPosition);
							++dwSize;
						}
						return true;
					}
				};
				char Buf[sizeof(stReturnTeamMemberPosition) + sizeof(stTeamMemberPosition) * (Team::MaxMember+1)];
				stReturnTeamMemberPosition *rtm = (stReturnTeamMemberPosition*)Buf;
				constructInPlace(rtm);
				SceneUser *leader = SceneUserManager::getMe().getUserByTempID(this->team.getLeader());
				if(leader)
				{
					TeamPositionExec tpe((char*)rtm->memberPos , sizeof(stTeamMemberPosition) * (Team::MaxMember+1));
					leader->team.execEveryOne(tpe);
					rtm->size = tpe.dwSize;
					this->sendCmdToMe(rtm , sizeof(stReturnTeamMemberPosition) + sizeof(stTeamMemberPosition)*rtm->size);
				}
				return true;
			}
			break;
		case AUTO_REPLY_USERCMD_PARA:
			{
				using namespace Cmd;
				stAutoReplyUserCmd * cmd = (stAutoReplyUserCmd *)rev;
				strncpy(replyText, cmd->text, MAX_CHATINFO);
#ifdef _XWL_DEBUG
				Zebra::logger->debug("%s 设置自动回复，\"%s\"", name, cmd->text);
#endif
			}
			break;
		//soke 好友升级祝贺
		case FRIEND_CONGRATULATEINFO_USERCMD_PARA:
			{
				stFriendCongratulateInfoUserCmd * cmd = (stFriendCongratulateInfoUserCmd *)rev;
				SceneUser *pUser = SceneUserManager::getMe().getUserByID(cmd->PlayerId);
				if (pUser)
				{
					time_t timValue = time(NULL);

					if (pUser->m_dLevelupTime>0&&(timValue-pUser->m_dLevelupTime)/60<15)
					{
						if (pUser->m_nCongratulateNum<=20)
						{
							int exp = pUser->charstate.nextexp/200;
							pUser->addExp(exp);
							int exp1 = charstate.nextexp/200;
							addExp(exp1);
							pUser->m_nCongratulateNum++;
							char c[512];
						//	sprintf(c,"好友 %s 对你的鼓励：%s",cmd->name,cmd->info);
							sprintf(c,"好友 %s %s",cmd->name,cmd->info);
							Channel::sendSys(pUser, Cmd::INFO_TYPE_ZISEYROLL, c); //soke 好友膜拜改成专用的滚动（深红）
						}
						else//不是前20人
						{ //soke 改成弹出的提示
							Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你的好友在20名玩家的簇拥下，你的膜拜淹没在人海中");
						}
						
					}
					else//超过15分钟
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "膜拜消息已超过15分钟，你的好友已经等得不耐烦了");
					}
					
				}
			}
			break;	
			
		default:
			break;
	}
	return false;
}

/**
 * \brief 解析GM指令
 *
 *
 * \param pUser: 发出指令者
 * \param cmd: 发出的指令
 */
void Gm::exec(SceneUser *pUser,char *cmd)
{
	if(pUser && cmd)
	{
		char *para=NULL;
		char *space=index(cmd,' ');
		if(space==NULL)
		{
			space=cmd+strlen(cmd);
			para=space;
		}
		else
		{
			para=space+1;
		}
		if(space-cmd>32) return;

		char cmdName[MAX_NAMESIZE+1];
		bzero(cmdName,sizeof(cmdName));
		strncpy(cmdName,cmd,space-cmd);

		int i=0;
		while(GmCmds[i].cmd)
		{
			if(strcasecmp(GmCmds[i].cmd,cmdName)==0)
			{
				if (0==(Gm::normal_mode & GmCmds[i].priv))//普通玩家不可执行
				{
					if (0==(pUser->getPriv() & GmCmds[i].priv)) break;//没有权限的不可执行

				}
				if (GmCmds[i].parse(pUser,para))
					sendLog(pUser, cmdName, para);
			}
			i++;
		}
	}
}

/*\brief 显示命令行帮助
 * 只显示该用户有权限执行的
 *
 * \param pUser: 发出指令者
 * \param para: 无用
 * \return true
 */
bool Gm::help(SceneUser *pUser,const char *para)
{
	int i=0;
	while(GmCmds[i].cmd)
	{
		if (!(GmCmds[i].priv & Gm::normal_mode) && (pUser->id>1000))//id>1000的不可执行
		{
			i++;
			continue;
		}
		if (pUser->getPriv() & GmCmds[i].priv)
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "%-8s %s",GmCmds[i].cmd,GmCmds[i].desc);
		}
		i++;
	}
	/*
	//系统消息测试
	Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "Cmd::INFO_TYPE_SYS");
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "Cmd::INFO_TYPE_GAME");
	Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "Cmd::INFO_TYPE_FAIL");
	Channel::sendSys(pUser, Cmd::INFO_TYPE_STATE, "Cmd::INFO_TYPE_STATE");
	 */
	return true;
}

/**
 * \brief 召唤宠物
 *
 *
 * \param pUser: 用户
 * \param para: 字串
 * \return 召唤是否成功
 */
bool Gm::callPet(SceneUser *pUser,const char *para)
{
	if (0==pUser) return false;
	zProperties prop;
	prop.parseCmdLine(para);

	DWORD id = atoi(prop["id"].c_str());
	if (0==id) return false;

	Cmd::petType pt;
	switch (atoi(prop["type"].c_str()))
	{
		case 0:
			pt = Cmd::PET_TYPE_PET;
			break;
		case 1:
			pt = Cmd::PET_TYPE_RIDE;
			break;
		case 2:
			pt = Cmd::PET_TYPE_PET;
			break;
		case 3:
			pt = Cmd::PET_TYPE_SUMMON;
			break;
		case 4:
			pt = Cmd::PET_TYPE_TOTEM;
			break;
		case 5:
			pt = Cmd::PET_TYPE_GUARDNPC;
			break;
		case 6:
			pt = Cmd::PET_TYPE_SEMI;
			break;
		case 7:
			pt = Cmd::PET_TYPE_CARTOON;
			break;
		default:
			ScenesService::gm_logger->trace("GM %s(%u) 使用callpet指令，类型错误，type=%u", pUser->name, pUser->id, atoi(prop["type"].c_str()));
			return false;
			break;
	}

	if (0!=id)
	{
		if (pt==2)
			pUser->petData.state = Cmd::PET_STATE_PUTUP;
		ScenePet * p = pUser->summonPet(id, pt, 0, 0, prop["name"].c_str());
		if (p)
		{
			if (pt==Cmd::PET_TYPE_PET)
			{
				p->petData.state = Cmd::PET_STATE_NORMAL;
				p->getAbilityByLevel(p->npc->level);
				bcopy(&p->petData, &pUser->petData, sizeof(pUser->petData));
				p->sendData();
			}
			ScenesService::gm_logger->trace("GM %s(%u) 使用callpet指令，召唤 %s type=%u", pUser->name, pUser->id, p->name, pt);
			return true;
		}
		ScenesService::gm_logger->trace("GM %s(%u) 使用callpet指令，召唤失败，para=%s", pUser->name, pUser->id, para);
	}
	return false;
}

/*\brief 直接设置玩家的数值
 * 包括五行值、技能点数、恢复装备耐久度、经验值
 *
 * \param pUser: 用户
 * \param para:内容
 * return true
 */
bool Gm::setvalue(SceneUser* pUser , const char* para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	if(atoi(prop["skill"].c_str()) != 0)
	{
		pUser->charbase.skillpoint = atoi(prop["skill"].c_str());

		ScenesService::gm_logger->trace("GM:%s 设置了自己的技能点数 %d", pUser->name, pUser->charbase.skillpoint);
		//pUser->charstate.skillpoint = atoi(prop["skill"].c_str());
	}
	if(""!=prop["dur"])
	{
		pUser->packs.equip.updateDurability(pUser , atoi(prop["dur"].c_str())*50);

		ScenesService::gm_logger->trace("GM:%s 设置了自己的装备耐久度 %d", pUser->name, atoi(prop["resume"].c_str()));
	}
	if(""!=prop["equip_exp"])
	{
		pUser->packs.equip.obtain_exp(pUser, atoi(prop["equip_exp"].c_str()), true);

		ScenesService::gm_logger->trace("GM:%s 设置了自己的装备经验值 %d", pUser->name, atoi(prop["equip_exp"].c_str()));
	}
	
	if(atoi(prop["exp"].c_str()) != 0)
	{
		pUser->charbase.exp = atoi(prop["exp"].c_str());
		if(pUser->charbase.exp >= pUser->charstate.nextexp)
		{
			pUser->upgrade(); 
		}

		ScenesService::gm_logger->trace("GM:%s 设置了自己的经验值 %d", pUser->name, pUser->charbase.exp);
	}
	if(atoi(prop["cooling"].c_str()) != 0)
	{
		//pUser->packs.equip.updateDurability(pUser , atoi(prop["resume"].c_str()));
	}

	if(atoi(prop["str"].c_str()) != 0 || atoi(prop["xstr"].c_str()) != 0)
	{
		pUser->charbase.wdStr = atoi(prop["str"].c_str()) + atoi(prop["xstr"].c_str());
	}
	else if(atoi(prop["str"].c_str()) != 0)
	{
		pUser->charbase.wdStr = atoi(prop["str"].c_str());
	}
	else if(atoi(prop["xstr"].c_str()) != 0)
	{
		pUser->charbase.wdStr = atoi(prop["xstr"].c_str());
	}
	if(atoi(prop["int"].c_str()) != 0 || atoi(prop["xint"].c_str()) != 0)
	{
		pUser->charbase.wdInt = atoi(prop["int"].c_str()) + atoi(prop["xint"].c_str());
	}
	else if(atoi(prop["int"].c_str()) != 0)
	{
		pUser->charbase.wdInt = atoi(prop["int"].c_str());
	}
	else if(atoi(prop["xint"].c_str()) != 0)
	{
		pUser->charbase.wdInt = atoi(prop["xint"].c_str());
	}
	if(atoi(prop["dex"].c_str()) != 0 || atoi(prop["xdex"].c_str()) != 0)
	{
		pUser->charbase.wdDex = atoi(prop["dex"].c_str()) + atoi(prop["xdex"].c_str());
	}
	else if(atoi(prop["dex"].c_str()) != 0)
	{
		pUser->charbase.wdDex = atoi(prop["dex"].c_str());
	}
	else if(atoi(prop["xdex"].c_str()) != 0)
	{
		pUser->charbase.wdDex = atoi(prop["xdex"].c_str());
	}
	if(atoi(prop["men"].c_str()) != 0 || atoi(prop["xmen"].c_str()) != 0)
	{
		pUser->charbase.wdMen = atoi(prop["men"].c_str()) + atoi(prop["xmen"].c_str());
	}
	else if(atoi(prop["men"].c_str()) != 0)
	{
		pUser->charbase.wdMen = atoi(prop["men"].c_str());
	}
	else if(atoi(prop["xmen"].c_str()) != 0)
	{
		pUser->charbase.wdMen = atoi(prop["xmen"].c_str());
	}
	if(atoi(prop["con"].c_str()) != 0 || atoi(prop["xcon"].c_str()) != 0)
	{
		pUser->charbase.wdCon = atoi(prop["con"].c_str()) + atoi(prop["xcon"].c_str());
	}
	else if(atoi(prop["con"].c_str()) != 0)
	{
		pUser->charbase.wdCon = atoi(prop["con"].c_str());
	}
	else if(atoi(prop["xcon"].c_str()) != 0)
	{
		pUser->charbase.wdCon = atoi(prop["xcon"].c_str());
	}
	if(atoi(prop["peace"].c_str()) != 0)
	{
		if(atoi(prop["peace"].c_str()))
		{
			pUser->charbase.pkaddition = atoi(prop["peace"].c_str());
		}
		Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"你现在的和平度累计时间%d分钟",
				pUser->charbase.pkaddition);
	}
	if (atoi(prop["actionpoint"].c_str()) != 0)
	{
		if (pUser->charbase.unionid>0)
		{
			Cmd::Session::t_OpUnionAction_SceneSession send;
			send.dwUnionID = pUser->charbase.unionid;
			send.dwAction = atoi(prop["actionpoint"].c_str());
			sessionClient->sendCmd(&send, sizeof(send));
		}
		else
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"你现在还未加入任何帮会");
		}
	}

	//通知客户端
	Cmd::stMainUserDataUserCmd send;
	pUser->full_t_MainUserData(send.data);
	pUser->sendCmdToMe(&send,sizeof(send));
	pUser->sendInitHPAndMp();
	return true;
}

/*\brief 得到玩家的某个数值
 * 包括魅力、经验值
 *
 * \param pUser: 用户
 * \param para:内容
 * return 得到是否成功 
 */
bool Gm::getvalue(SceneUser* pUser , const char* para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if(!strcmp(prop["type"].c_str() , "魅力"))
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "你现在的魅力值=%u",pUser->charstate.charm);

		ScenesService::gm_logger->trace("GM:%s 查看自己的魅力值", pUser->name);
		return true;
	}
	if(!strcmp(prop["type"].c_str() , "经验"))
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"你现在的经验值=%u , 升级经验值=%u",pUser->charbase.exp ,pUser->charstate.nextexp );

		ScenesService::gm_logger->trace("GM:%s 查看自己的系统设置", pUser->name);
		return true;
	}
	if("sys"==prop["type"])
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"%x %x %x %x",pUser->sysSetting[0] ,pUser->sysSetting[1], pUser->sysSetting[2], pUser->sysSetting[3]);

		ScenesService::gm_logger->trace("GM:%s 查看自己的经验值", pUser->name);
		return true;
	}

	ScenesService::gm_logger->trace("GM:%s 使用getvalue命令，参数错误", pUser->name);
	return false;
}

/*\brief 设置和显示当前善恶
 *
 * \param pUser: 用户
 * \param para:内容
 * return true
 */
bool Gm::goodness(SceneUser* pUser, const char* para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	//zRegex regex;
	pUser->pkState.clearProtect();
	if(!strcmp(prop["type"].c_str() , "英雄"))
	{
		pUser->charbase.goodness = (DWORD)Cmd::GOODNESS_0;
	}
	else if(!strcmp(prop["type"].c_str() , "侠士"))
	{
		pUser->charbase.goodness = (DWORD)Cmd::GOODNESS_1;
	}
	else if(!strcmp(prop["type"].c_str() , "普通"))
	{
		pUser->charbase.goodness = (DWORD)Cmd::GOODNESS_2_1;
		while (!pUser->pkState.cancelProtect(pUser));
	}
	else if(!strcmp(prop["type"].c_str() , "歹徒"))
	{
		pUser->charbase.goodness = (DWORD)Cmd::GOODNESS_3;
	}
	else if(!strcmp(prop["type"].c_str() , "恶徒"))
	{
		pUser->charbase.goodness = (DWORD)Cmd::GOODNESS_4;
	}
	else if(!strcmp(prop["type"].c_str() , "恶魔"))
	{
		pUser->charbase.goodness = (DWORD)Cmd::GOODNESS_5;
	}
	else if(!strcmp(prop["type"].c_str() , "魔头"))
	{
		pUser->charbase.goodness = (DWORD)Cmd::GOODNESS_6;
	}
	else if(!strcmp(prop["type"].c_str() , "主动"))
	{
		pUser->charbase.goodness = (DWORD)Cmd::GOODNESS_ATT;
	}
	else if(!strcmp(prop["type"].c_str() , "自卫"))
	{
		pUser->charbase.goodness = (DWORD)Cmd::GOODNESS_DEF;
	}
	Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"你现在是%s,数值是:%u",
			pUser->getGoodnessName(),pUser->charbase.goodness&0X0000FFFF);
 	Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"你现在是%s,数值是:%u",
                        pUser->getGoodnessName(),pUser->charbase.goodness);

	ScenesService::gm_logger->trace("GM:%s 把设置自己的善恶度为 %s", pUser->name, prop["type"].c_str());

	pUser->sendGoodnessToNine();

	return true;
}

/*\brief 后退
 *
 * \param pUser: 用户
 * \param para:内容
 * return true
 */
bool Gm::backoff(SceneUser* pUser, const char* para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	//zRegex regex;
	DWORD dir=atoi(prop["dir"].c_str());
	DWORD grid=atoi(prop["grid"].c_str());
	dir = dir?dir%8:(pUser->getDir() + 4)%8;
	grid = grid?grid:2;
	pUser->backOff(dir , grid);
	return true;
}
/*\brief 设置hp , mp , sp
 *
 * \param pUser: 用户
 * \param para:内容
 * return true
 */
bool Gm::value(SceneUser* pUser, const char* para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	//zRegex regex;
	DWORD hp=atoi(prop["hp"].c_str());
	DWORD mp=atoi(prop["mp"].c_str());
	DWORD sp=atoi(prop["sp"].c_str());
	DWORD ep=atoi(prop["ep"].c_str());
	DWORD honor=atoi(prop["honor"].c_str());
	DWORD gold=atoi(prop["gold"].c_str());
	DWORD ticket=atoi(prop["ticket"].c_str());
	DWORD salary=atoi(prop["salary"].c_str());
	DWORD chongzhi=atoi(prop["chongzhi"].c_str());
	DWORD charvip=atoi(prop["charvip"].c_str());
	DWORD changejob=atoi(prop["changejob"].c_str());
	DWORD hanbing=atoi(prop["hanbing"].c_str());
	DWORD angel=atoi(prop["angel"].c_str());
	DWORD folwers=atoi(prop["folwers"].c_str());
	DWORD moods=atoi(prop["moods"].c_str());
	DWORD flloves=atoi(prop["flloves"].c_str());
	DWORD pk=atoi(prop["pk"].c_str());
	DWORD caller=atoi(prop["caller"].c_str());

	if(caller)
	{
		pUser->packs.equip.king_obj_times= caller;
		pUser->packs.equip.king_obj_time = SceneTimeTick::currentTime.sec(); 
		pUser->packs.equip.family_obj_times= caller;
		pUser->packs.equip.family_obj_time = SceneTimeTick::currentTime.sec(); 
		pUser->packs.equip.tong_obj_times= caller;
		pUser->packs.equip.tong_obj_time = SceneTimeTick::currentTime.sec(); 
		Cmd::Session::t_Reset_CallTimes_SceneSession ss;
		ss.leaderTempID=pUser->tempid;
		sessionClient->sendCmd(&ss,sizeof(ss));
	}
	if(hp)
	{
		pUser->charbase.hp = hp;
		ScenesService::gm_logger->trace("GM:%s 设置自己的生命值 %d", pUser->name, hp);
	}

	if(mp)
	{
		pUser->charbase.mp = mp;
		ScenesService::gm_logger->trace("GM:%s 设置自己的魔法值 %d", pUser->name, mp);
	}

	if(sp)
	{
		pUser->charbase.sp = sp;
		ScenesService::gm_logger->trace("GM:%s 设置自己的体力值 %d", pUser->name, sp);
	}

	if (ep)
	{
		pUser->charbase.exploit = ep;
		pUser->reSendMyMapData();
		ScenesService::gm_logger->trace("GM:%s 设置自己的功勋值 %d", pUser->name, ep);
	}
	if (honor)
	{
		pUser->charbase.honor = honor;
		pUser->charbase.maxhonor = honor;
		ScenesService::gm_logger->trace("GM:%s 设置自己的荣誉值 %d", pUser->name, honor);
	}
	if(gold)
	{
		pUser->packs.addGold(gold,"GM指令",NULL,true,true);
		Cmd::stMainUserDataUserCmd send;
		pUser->full_t_MainUserData(send.data);
		pUser->sendCmdToMe(&send,sizeof(send));
	}
	if(ticket)
	{
		pUser->packs.addTicket(ticket,"GM指令");
	}
	if(salary)
	{
		pUser->packs.addSalary(salary,"GM指令");
	}
	if(chongzhi)
	{
		pUser->packs.addChongzhi(chongzhi,"GM指令");
	}	
	if(charvip)
	{
		pUser->packs.addCharvip(charvip,"GM指令");
	}
	if(changejob)
	{
		pUser->packs.addChangejob(changejob,"GM指令");
	}
	if(hanbing)
	{
		pUser->packs.addHanbing(hanbing,"GM指令");
	}
	if(angel)
	{
		pUser->packs.addAngel(angel,"GM指令");
	}
	if(folwers)
	{
		pUser->packs.addFolwers(folwers,"GM指令");
	}
	if(moods)
	{
		pUser->packs.addMoods(moods,"GM指令");
	}
	if(flloves)
	{
		pUser->packs.addFlloves(flloves,"GM指令");
	}
	if(pk)
	{
		if (pk>9999) pk = 9999;
		pUser->charbase.goodness = (pUser->charbase.goodness & 0xffff0000) + pk;

		Cmd::stMainUserDataUserCmd  userinfo;
		pUser->full_t_MainUserData(userinfo.data);
		pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
		pUser->sendMeToNine();
	}

	pUser->attackRTHpAndMp();

	return true;
}

/**
 * \brief 使用药品
 *
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 使用是否成功
 */
bool Gm::leechdom(SceneUser *pUser,const char *para)
{
	//WAIT TO DO
#if 0
	zProperties prop;
	prop.parseCmdLine(para);
	//zRegex regex;
	if ("" == prop["id"] && "" == prop["name"].c_str())
	{
		ScenesService::gm_logger->trace("GM:%s 使用leechdom命令，参数错误", pUser->name);
		return false;
	}
	zObjectB *base = objectbm.get(atoi(prop["id"].c_str()));
	if (NULL == base)
		base = objectbm.get(prop["name"].c_str());
	if (base)
	{
		if(base->kind != ItemType_Leechdom && base->kind != ItemType_Food)
		{
			return false;
		}
		DWORD num=atoi(prop["num"].c_str());
		DWORD level=atoi(prop["level"].c_str());
		DWORD hole=atoi(prop["hole"].c_str());
		if (hole>6) hole=6;
		if(num==0) num=1;
		//num=1;
		zObject *o=zObject::create(base,num, level);
		if(o)
		{
			if(pUser->leechdom.isCooling(o->data.maxhp))
			{
				Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"该类药品处于冷却时期%s%ld个",o->name,o->data.dwNum);
				SAFE_DELETE(o);
				ScenesService::gm_logger->trace("GM:%s 使用药品 %s ，该药品处于冷却时期", pUser->name, o->name);
				return false;
			}
			for (unsigned int i=0; i<num; i++)
				pUser->useLeechdom(o);
			pUser->attackRTHpAndMp();
			Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"使用药品%s%ld个",o->name,o->data.dwNum);
			ScenesService::gm_logger->trace("GM:%s 使用药品 %s %d 个", pUser->name, o->name, o->data.dwNum);
			return true;
		}
	}
	ScenesService::gm_logger->trace("GM:%s 使用leechdom命令，不存在该道具 id=%d name=%s", pUser->name,atoi(prop["id"].c_str()), prop["name"].c_str());
#endif 
	return false;
}

/**
 * \brief 得到物品
 *
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 得到是否成功
 */
bool Gm::bindfetch(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	zRegex regex;
	if ("" == prop["id"] && "" == prop["name"].c_str())
	{
		ScenesService::gm_logger->trace("GM:%s 使用fetch命令，参数错误", pUser->name);
		return false;
	}

	zObjectB *base = objectbm.get(atoi(prop["id"].c_str()));
	if (NULL == base)
		base = objectbm.get(prop["name"].c_str());
	if (base)
	{
		//不能获得马匹
		if (base->kind==ItemType_HORSE || base->kind==ItemType_BattleHorse)
			return false;

		DWORD num=atoi(prop["num"].c_str());
		if (num>(DWORD)base->maxnum) num = base->maxnum;
		int level=atoi(prop["level"].c_str());
		if (level>11) level=11;
		DWORD hole=atoi(prop["hole"].c_str());
		if (hole>6) hole=6;

		//天魔石最高6级
		if ((679==atoi(prop["id"].c_str()))||("天魔石"==prop["name"].c_str()))
			if (level>6) level=6;

		//定情物没有等级
		if ((682==atoi(prop["id"].c_str()))||("定情信物"==prop["name"].c_str()))
			level=0;

		if(num==0) num=1;
		//gm指令制造银子有内存泄漏,修改ok
		if(base->id == 665)
		{
			pUser->packs.addMoney(num,"GM指令创建");
			ScenesService::gm_logger->trace("GM:%s 创造了 %u 钱", pUser->name, num);
			return true;
		}
		zObject* o = NULL;
		if (base->recast) 
		{
			o = zObject::create(base,num, 0);
			if (o && level) 
			{
				do 
				{
					Upgrade::upgrade(*pUser, o, 100); //must success
				}
				while (--level >0); 
			}
		}
		else 
		{
			o = zObject::create(base,num, level);
		}

		if(o)
		{
			zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,o->data.dwNum,1,0,NULL,pUser->id,pUser->name,"fetch",o->base,o->data.kind,o->data.upgrade);
			//魂魄石
			if (o->base->kind == ItemType_SOULSTONE) 
			{
				DWORD monster_id = atoi(prop["monster"].c_str());
				if (!monster_id) monster_id=1;
				SoulStone::assign(o, monster_id);
			}
			else 
			{			
				if(o->base->make == 1)
				{
					BYTE type=atoi(prop["type"].c_str());
					EquipMaker maker(NULL);
					maker.assign(NULL, o, base,false, type);
				}
				if (hole) 
				{
					while (hole--) o->data.socket[hole]=(DWORD)-1;
				}
			}
			if(o->data.dwNum!=0 && pUser->packs.addObject(o,true, AUTO_PACK))
			{		
				//手动设置物品属性
				if (""!=prop["strName"]) strncpy(o->data.strName, prop["strName"].c_str(), MAX_NAMESIZE-1);
				int value;
				//if (0!=(value=atoi(prop["sex"].c_str()))) o->data.sex = value;
				if (0!=(value=atoi(prop["color"].c_str()))) o->data.color = value;
				if (0!=(value=atoi(prop["needlevel"].c_str()))) o->data.needlevel = value;
				//if (0!=(value=atoi(prop["level"].c_str()))) o->data.level = value;
				//if (0!=(value=atoi(prop["hp"].c_str())))o->data.hp = value;
				if (0!=(value=atoi(prop["maxhp"].c_str()))) o->data.maxhp = value;
				//if (0!=(value=atoi(prop["mp"].c_str()))) o->data.mp = value;
				if (0!=(value=atoi(prop["maxmp"].c_str()))) o->data.maxmp = value;
				//if (0!=(value=atoi(prop["sp"].c_str()))) o->data.sp = value;
				if (0!=(value=atoi(prop["maxsp"].c_str()))) o->data.maxsp = value;
				if (0!=(value=atoi(prop["pdamage"].c_str()))) o->data.pdamage = value;
				if (0!=(value=atoi(prop["maxpdamage"].c_str()))) o->data.maxpdamage = value;
				if (0!=(value=atoi(prop["pdam"].c_str()))) o->data.pdam = value;
				//if (0!=(value=atoi(prop["maxrdam"].c_str()))) o->data.maxrdam = value;
				//if (0!=(value=atoi(prop["tdam"].c_str()))) o->data.tdam = value;
				//if (0!=(value=atoi(prop["maxtdam"].c_str()))) o->data.maxtdam = value;
				if (0!=(value=atoi(prop["mdamage"].c_str()))) o->data.mdamage = value;
				if (0!=(value=atoi(prop["maxmdamage"].c_str()))) o->data.maxmdamage = value;
				if (0!=(value=atoi(prop["pdefence"].c_str()))) o->data.pdefence = value;
				if (0!=(value=atoi(prop["pdef"].c_str()))) o->data.pdef = value;
				if (0!=(value=atoi(prop["mdefence"].c_str()))) o->data.mdefence = value;
				if (0!=(value=atoi(prop["mdam"].c_str()))) o->data.mdam = value;
				if (0!=(value=atoi(prop["mdef"].c_str()))) o->data.mdef = value;

				if (0!=(value=atoi(prop["m1axhp"].c_str()))) o->data.m1axhp = value;
				if (0!=(value=atoi(prop["p1damage"].c_str()))) o->data.p1damage = value;
				if (0!=(value=atoi(prop["m1axpdamage"].c_str()))) o->data.m1axpdamage = value;
				if (0!=(value=atoi(prop["m1damage"].c_str()))) o->data.m1damage = value;
				if (0!=(value=atoi(prop["m1axmdamage"].c_str()))) o->data.m1axmdamage = value;
				if (0!=(value=atoi(prop["p1defence"].c_str()))) o->data.p1defence = value;
				if (0!=(value=atoi(prop["m1defence"].c_str()))) o->data.m1defence = value;

				if (0!=(value=atoi(prop["m2axhp"].c_str()))) o->data.m2axhp = value;
				if (0!=(value=atoi(prop["p2damage"].c_str()))) o->data.p2damage = value;
				if (0!=(value=atoi(prop["m2axpdamage"].c_str()))) o->data.m2axpdamage = value;
				if (0!=(value=atoi(prop["m2damage"].c_str()))) o->data.m2damage = value;
				if (0!=(value=atoi(prop["m2axmdamage"].c_str()))) o->data.m2axmdamage = value;
				if (0!=(value=atoi(prop["p2defence"].c_str()))) o->data.p2defence = value;
				if (0!=(value=atoi(prop["m2defence"].c_str()))) o->data.m2defence = value;

				if (0!=(value=atoi(prop["m3axhp"].c_str()))) o->data.m3axhp = value;
				if (0!=(value=atoi(prop["p3damage"].c_str()))) o->data.p3damage = value;
				if (0!=(value=atoi(prop["m3axpdamage"].c_str()))) o->data.m3axpdamage = value;
				if (0!=(value=atoi(prop["m3damage"].c_str()))) o->data.m3damage = value;
				if (0!=(value=atoi(prop["m3axmdamage"].c_str()))) o->data.m3axmdamage = value;
				if (0!=(value=atoi(prop["p3defence"].c_str()))) o->data.p3defence = value;
				if (0!=(value=atoi(prop["m3defence"].c_str()))) o->data.m3defence = value;



				if (0!=(value=atoi(prop["hpr"].c_str()))) o->data.hpr = value;
				if (0!=(value=atoi(prop["mpr"].c_str()))) o->data.mpr = value;
				if (0!=(value=atoi(prop["spr"].c_str()))) o->data.spr = value;
				if (0!=(value=atoi(prop["akspeed"].c_str()))) o->data.akspeed = value;
				//if (0!=(value=atoi(prop["mgspeed"].c_str()))) o->data.mgspeed = value;
				if (0!=(value=atoi(prop["mvspeed"].c_str()))) o->data.mvspeed = value;
				if (0!=(value=atoi(prop["atrating"].c_str()))) o->data.atrating = value;
				if (0!=(value=atoi(prop["akdodge"].c_str()))) o->data.akdodge = value;
				//if (0!=(value=atoi(prop["mgrating"].c_str()))) o->data.mgrating = value;
				//if (0!=(value=atoi(prop["mgdodge"].c_str()))) o->data.mgdodge = value;
				if (0!=(value=atoi(prop["dur"].c_str()))) o->data.dur = value;
				if (0!=(value=atoi(prop["maxdur"].c_str()))) o->data.maxdur = value;
				if (0!=(value=atoi(prop["price"].c_str()))) o->data.price = value;
				if (0!=(value=atoi(prop["cardpoint"].c_str()))) o->data.cardpoint = value;

				if (0!=(value=atoi(prop["str"].c_str()))) o->data.str = value;
				if (0!=(value=atoi(prop["inte"].c_str()))) o->data.inte = value;
				if (0!=(value=atoi(prop["dex"].c_str()))) o->data.dex = value;
				if (0!=(value=atoi(prop["spi"].c_str()))) o->data.spi = value;
				if (0!=(value=atoi(prop["con"].c_str()))) o->data.con = value;

				if (0!=(value=atoi(prop["xstr"].c_str()))) o->data.xstr = value;
				if (0!=(value=atoi(prop["xinte"].c_str()))) o->data.xinte = value;
				if (0!=(value=atoi(prop["xdex"].c_str()))) o->data.xdex = value;
				if (0!=(value=atoi(prop["xspi"].c_str()))) o->data.xspi = value;
				if (0!=(value=atoi(prop["xcon"].c_str()))) o->data.xcon = value;

				if (0!=(value=atoi(prop["fivetype"].c_str()))) o->data.fivetype = value;
				if ("0"==prop["fivetype"]) o->data.fivetype = 0;//金属性
				if (0!=(value=atoi(prop["fivepoint"].c_str()))) o->data.fivepoint = value;
				if (0!=(value=atoi(prop["kind"].c_str()))) o->data.kind = value;
				if (0!=(value=atoi(prop["upgrade"].c_str()))) o->data.upgrade = value;

				if (0!=(value=atoi(prop["doublexp"].c_str()))) o->data.doublexp = value;
				//if (0!=(value=atoi(prop["pdefup"].c_str()))) o->data.pdefup = value;
				//if (0!=(value=atoi(prop["mdefup"].c_str()))) o->data.mdefup = value;

				if (0!=(value=atoi(prop["poisondef"].c_str()))) o->data.poisondef = value;
				if (0!=(value=atoi(prop["lulldef"].c_str()))) o->data.lulldef = value;
				if (0!=(value=atoi(prop["reeldef"].c_str()))) o->data.reeldef = value;
				if (0!=(value=atoi(prop["evildef"].c_str()))) o->data.evildef = value;
				if (0!=(value=atoi(prop["bitedef"].c_str()))) o->data.bitedef = value;
				if (0!=(value=atoi(prop["stabledef"].c_str()))) o->data.stabledef = value;
				if (0!=(value=atoi(prop["slowdef"].c_str()))) o->data.slowdef = value;
				if (0!=(value=atoi(prop["luredef"].c_str()))) o->data.luredef = value;
				if (0!=(value=atoi(prop["colddef"].c_str()))) o->data.colddef = value;
				if (0!=(value=atoi(prop["petrifydef"].c_str()))) o->data.petrifydef = value;
				if (0!=(value=atoi(prop["blinddef"].c_str()))) o->data.blinddef = value;
				if (0!=(value=atoi(prop["chaosdef"].c_str()))) o->data.chaosdef = value;

				if (0!=(value=atoi(prop["poison"].c_str()))) o->data.poison = value;
				if (0!=(value=atoi(prop["lull"].c_str()))) o->data.lull = value;
				if (0!=(value=atoi(prop["reel"].c_str()))) o->data.reel = value;
				if (0!=(value=atoi(prop["evil"].c_str()))) o->data.evil = value;
				if (0!=(value=atoi(prop["bite"].c_str()))) o->data.bite = value;
				if (0!=(value=atoi(prop["stable"].c_str()))) o->data.stable = value;
				if (0!=(value=atoi(prop["slow"].c_str()))) o->data.slow = value;
				if (0!=(value=atoi(prop["lure"].c_str()))) o->data.lure = value;
				if (0!=(value=atoi(prop["cold"].c_str()))) o->data.cold = value;
				if (0!=(value=atoi(prop["petrify"].c_str()))) o->data.petrify = value;
				if (0!=(value=atoi(prop["blind"].c_str()))) o->data.blind = value;
				if (0!=(value=atoi(prop["chaos"].c_str()))) o->data.chaos = value;

				if (0!=(value=atoi(prop["durpoint"].c_str()))) o->data.durpoint = value;
				if (0!=(value=atoi(prop["dursecond"].c_str()))) o->data.dursecond = value;
				if (0!=(value=atoi(prop["holy"].c_str()))) o->data.holy = value;
				if (0!=(value=atoi(prop["bang"].c_str()))) o->data.bang = value;

				if (0!=(value=atoi(prop["hpleech"].c_str()))) o->data.hpleech.odds = value;
				if (0!=(value=atoi(prop["hpleech_value"].c_str()))) o->data.hpleech.effect = value;
				if (0!=(value=atoi(prop["mpleech"].c_str()))) o->data.mpleech.odds = value;
				if (0!=(value=atoi(prop["mpleech_value"].c_str()))) o->data.mpleech.effect = value;

				if (0!=(value=atoi(prop["hptomp"].c_str()))) o->data.hptomp = value;
				//if (0!=(value=atoi(prop["dhpv"].c_str()))) o->data.dhpv = value;
				if (0!=(value=atoi(prop["dhpp"].c_str()))) o->data.dhpp = value;
				//if (0!=(value=atoi(prop["dmpv"].c_str()))) o->data.dmpv = value;
				if (0!=(value=atoi(prop["dmpp"].c_str()))) o->data.dmpp = value;

				if (0!=(value=atoi(prop["incgold"].c_str()))) o->data.incgold = value;
				if (0!=(value=atoi(prop["doublexp"].c_str()))) o->data.doublexp = value;
				if (0!=(value=atoi(prop["mf"].c_str()))) o->data.mf = value;

				if (0!=(value=atoi(prop["dpdam"].c_str()))) o->data.dpdam = value;         //物理伤害减少 x%
				if (0!=(value=atoi(prop["dmdam"].c_str()))) o->data.dmdam = value;         //魔法伤害减少 x%
				if (0!=(value=atoi(prop["bdam"].c_str()))) o->data.bdam = value;           //增加伤害 x%
				if (0!=(value=atoi(prop["rdam"].c_str()))) o->data.rdam = value;           //伤害反射 %x
				if (0!=(value=atoi(prop["ignoredef"].c_str()))) o->data.ignoredef = value; //%x 忽视目标防御
				//soke 第二灵魂
				if (0!=(value=atoi(prop["addpdef"].c_str()))) o->data.addpdef = value;     //物理防御力增强 x
				if (0!=(value=atoi(prop["addmdef"].c_str()))) o->data.addmdef = value;     //魔法防御力增强 x
				if (0!=(value=atoi(prop["addpdam"].c_str()))) o->data.addpdam = value;     //增加物理攻击力 x
				if (0!=(value=atoi(prop["addmdam"].c_str()))) o->data.addmdam = value;     //增加魔法攻击力 x
				if (0!=(value=atoi(prop["addhp"].c_str()))) o->data.addhp = value;         //增加生命值 x

				if (0!=(value=atoi(prop["fiveset0"].c_str()))) o->data.fiveset[0] = value;
				if (0!=(value=atoi(prop["fiveset1"].c_str()))) o->data.fiveset[1] = value;
				if (0!=(value=atoi(prop["fiveset2"].c_str()))) o->data.fiveset[2] = value;
				if (0!=(value=atoi(prop["fiveset3"].c_str()))) o->data.fiveset[3] = value;
				if (0!=(value=atoi(prop["fiveset4"].c_str()))) o->data.fiveset[4] = value;

				//if (0!=(value=atoi(prop["invtarget"].c_str()))) o->data.invtarget = value;

				if (0!=(value=atoi(prop["exp"].c_str()))) o->data.exp = value;
				if (0!=(value=atoi(prop["round"].c_str()))) o->data.round = value;
				if (0!=(value=atoi(prop["xingshu"].c_str()))) o->data.xingshu = value;
				if (0!=(value=atoi(prop["douhun"].c_str()))) o->data.douhun = value;
				if (""!=prop["maker"]) strncpy(o->data.maker, prop["maker"].c_str(), MAX_NAMESIZE-1);
				//if (""!=prop["set"]) strncpy(o->data.set, prop["set"].c_str(), MAX_NAMESIZE-1);

				if (0!=(value=atoi(prop["skill_id"].c_str()))) o->data.skills.id = value;
				if (0!=(value=atoi(prop["skill_lv"].c_str()))) o->data.skills.point = value;

				if (0!=(value=atoi(prop["sid1"].c_str()))) o->data.skill[0].id = value;
				if (0!=(value=atoi(prop["slv1"].c_str()))) o->data.skill[0].point = value;

				if (0!=(value=atoi(prop["sid2"].c_str()))) o->data.skill[1].id = value;
				if (0!=(value=atoi(prop["slv2"].c_str()))) o->data.skill[1].point = value;

				if (0!=(value=atoi(prop["sid3"].c_str()))) o->data.skill[2].id = value;
				if (0!=(value=atoi(prop["slv3"].c_str()))) o->data.skill[2].point = value;

				if (0!=(value=atoi(prop["sid4"].c_str()))) o->data.skill[3].id = value;
				if (0!=(value=atoi(prop["slv4"].c_str()))) o->data.skill[3].point = value;

				if (0!=(value=atoi(prop["sid5"].c_str()))) o->data.skill[4].id = value;
				if (0!=(value=atoi(prop["slv5"].c_str()))) o->data.skill[4].point = value;

				if (0!=(value=atoi(prop["sid6"].c_str()))) o->data.skill[5].id = value;
				if (0!=(value=atoi(prop["slv6"].c_str()))) o->data.skill[5].point = value;

				if (0!=(value=atoi(prop["sid7"].c_str()))) o->data.skill[6].id = value;
				if (0!=(value=atoi(prop["slv7"].c_str()))) o->data.skill[6].point = value;

				if (0!=(value=atoi(prop["sid8"].c_str()))) o->data.skill[7].id = value;
				if (0!=(value=atoi(prop["slv8"].c_str()))) o->data.skill[7].point = value;

				if (0!=(value=atoi(prop["sid9"].c_str()))) o->data.skill[8].id = value;
				if (0!=(value=atoi(prop["slv9"].c_str()))) o->data.skill[8].point = value;

				if (0!=(value=atoi(prop["sid10"].c_str()))) o->data.skill[9].id = value;
				if (0!=(value=atoi(prop["slv10"].c_str()))) o->data.skill[9].point = value;

				//soke 道具需要绑定
				o->data.bind=1;
				Cmd::stAddObjectPropertyUserCmd send;
				bcopy(&o->data,&send.object,sizeof(t_Object));
				pUser->sendCmdToMe(&send,sizeof(send));
				Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "得到物品%s%ld个",o->name,o->data.dwNum);
				return true;
			}
			zObject::destroy(o);
		}
		return false;
	}
	ScenesService::gm_logger->trace("GM:%s 试图创造不存在的物品 name=%s id=%s ", pUser->name, prop["name"].c_str(), prop["id"].c_str());
	return false;
}

/**
 * \brief 得到物品
 *
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 得到是否成功
 */
bool Gm::fetch(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	zRegex regex;
	if ("" == prop["id"] && "" == prop["name"].c_str())
	{
		ScenesService::gm_logger->trace("GM:%s 使用fetch命令，参数错误", pUser->name);
		return false;
	}

	zObjectB *base = objectbm.get(atoi(prop["id"].c_str()));
	if (NULL == base)
		base = objectbm.get(prop["name"].c_str());
	if (base)
	{
		//不能获得马匹
		if (base->kind==ItemType_HORSE || base->kind==ItemType_BattleHorse)
			return false;

		DWORD num=atoi(prop["num"].c_str());
		if (num>(DWORD)base->maxnum) num = base->maxnum;
		int level=atoi(prop["level"].c_str());
		if (level>11) level=11;
		DWORD hole=atoi(prop["hole"].c_str());
		if (hole>6) hole=6;

		//天魔石最高6级
		if ((679==atoi(prop["id"].c_str()))||("天魔石"==prop["name"].c_str()))
			if (level>6) level=6;

		//定情物没有等级
		if ((682==atoi(prop["id"].c_str()))||("定情信物"==prop["name"].c_str()))
			level=0;

		if(num==0) num=1;
		//gm指令制造银子有内存泄漏,修改ok
		if(base->id == 665)
		{
			pUser->packs.addMoney(num,"GM指令创建");
			ScenesService::gm_logger->trace("GM:%s 创造了 %u 钱", pUser->name, num);
			return true;
		}
		zObject* o = NULL;
		if (base->recast) 
		{
			o = zObject::create(base,num, 0);
			if (o && level) 
			{
				do 
				{
					Upgrade::upgrade(*pUser, o, 100); //must success
				}
				while (--level >0); 
			}
		}
		else 
		{
			o = zObject::create(base,num, level);
		}

		if(o)
		{
			zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,o->data.dwNum,1,0,NULL,pUser->id,pUser->name,"fetch",o->base,o->data.kind,o->data.upgrade);
			//魂魄石
			if (o->base->kind == ItemType_SOULSTONE) 
			{
				DWORD monster_id = atoi(prop["monster"].c_str());
				if (!monster_id) monster_id=1;
				SoulStone::assign(o, monster_id);
			}
			else 
			{			
				if(o->base->make == 1)
				{
					BYTE type=atoi(prop["type"].c_str());
					EquipMaker maker(NULL);
					maker.assign(NULL, o, base,false, type);
				}
				if (hole) 
				{
					while (hole--) o->data.socket[hole]=(DWORD)-1;
				}
			}
			if(o->data.dwNum!=0 && pUser->packs.addObject(o,true, AUTO_PACK))
			{		
				//手动设置物品属性
				if (""!=prop["strName"]) strncpy(o->data.strName, prop["strName"].c_str(), MAX_NAMESIZE-1);
				int value;
				//if (0!=(value=atoi(prop["sex"].c_str()))) o->data.sex = value;
				if (0!=(value=atoi(prop["color"].c_str()))) o->data.color = value;
				if (0!=(value=atoi(prop["bind"].c_str()))) o->data.bind = value;
				if (0!=(value=atoi(prop["needlevel"].c_str()))) o->data.needlevel = value;
				//if (0!=(value=atoi(prop["level"].c_str()))) o->data.level = value;
				//if (0!=(value=atoi(prop["hp"].c_str())))o->data.hp = value;
				if (0!=(value=atoi(prop["maxhp"].c_str()))) o->data.maxhp = value;
				//if (0!=(value=atoi(prop["mp"].c_str()))) o->data.mp = value;
				if (0!=(value=atoi(prop["maxmp"].c_str()))) o->data.maxmp = value;
				//if (0!=(value=atoi(prop["sp"].c_str()))) o->data.sp = value;
				if (0!=(value=atoi(prop["maxsp"].c_str()))) o->data.maxsp = value;
				if (0!=(value=atoi(prop["pdamage"].c_str()))) o->data.pdamage = value;
				if (0!=(value=atoi(prop["maxpdamage"].c_str()))) o->data.maxpdamage = value;
				if (0!=(value=atoi(prop["pdam"].c_str()))) o->data.pdam = value;
				//if (0!=(value=atoi(prop["maxrdam"].c_str()))) o->data.maxrdam = value;
				//if (0!=(value=atoi(prop["tdam"].c_str()))) o->data.tdam = value;
				//if (0!=(value=atoi(prop["maxtdam"].c_str()))) o->data.maxtdam = value;
				if (0!=(value=atoi(prop["mdamage"].c_str()))) o->data.mdamage = value;
				if (0!=(value=atoi(prop["maxmdamage"].c_str()))) o->data.maxmdamage = value;
				if (0!=(value=atoi(prop["pdefence"].c_str()))) o->data.pdefence = value;
				if (0!=(value=atoi(prop["pdef"].c_str()))) o->data.pdef = value;
				if (0!=(value=atoi(prop["mdefence"].c_str()))) o->data.mdefence = value;
				if (0!=(value=atoi(prop["mdam"].c_str()))) o->data.mdam = value;
				if (0!=(value=atoi(prop["mdef"].c_str()))) o->data.mdef = value;
				if (0!=(value=atoi(prop["m3axhp"].c_str()))) o->data.m3axhp = value;
				if (0!=(value=atoi(prop["p3damage"].c_str()))) o->data.p3damage = value;
				if (0!=(value=atoi(prop["m3axpdamage"].c_str()))) o->data.m3axpdamage = value;
				if (0!=(value=atoi(prop["m3damage"].c_str()))) o->data.m3damage = value;
				if (0!=(value=atoi(prop["m3axmdamage"].c_str()))) o->data.m3axmdamage = value;
				if (0!=(value=atoi(prop["p3defence"].c_str()))) o->data.p3defence = value;
				if (0!=(value=atoi(prop["m3defence"].c_str()))) o->data.m3defence = value;

				if (0!=(value=atoi(prop["hpr"].c_str()))) o->data.hpr = value;
				if (0!=(value=atoi(prop["mpr"].c_str()))) o->data.mpr = value;
				if (0!=(value=atoi(prop["spr"].c_str()))) o->data.spr = value;
				if (0!=(value=atoi(prop["akspeed"].c_str()))) o->data.akspeed = value;
				//if (0!=(value=atoi(prop["mgspeed"].c_str()))) o->data.mgspeed = value;
				if (0!=(value=atoi(prop["mvspeed"].c_str()))) o->data.mvspeed = value;
				if (0!=(value=atoi(prop["atrating"].c_str()))) o->data.atrating = value;
				if (0!=(value=atoi(prop["akdodge"].c_str()))) o->data.akdodge = value;
				//if (0!=(value=atoi(prop["mgrating"].c_str()))) o->data.mgrating = value;
				//if (0!=(value=atoi(prop["mgdodge"].c_str()))) o->data.mgdodge = value;
				if (0!=(value=atoi(prop["dur"].c_str()))) o->data.dur = value;
				if (0!=(value=atoi(prop["maxdur"].c_str()))) o->data.maxdur = value;
				if (0!=(value=atoi(prop["price"].c_str()))) o->data.price = value;
				if (0!=(value=atoi(prop["cardpoint"].c_str()))) o->data.cardpoint = value;

				if (0!=(value=atoi(prop["str"].c_str()))) o->data.str = value;
				if (0!=(value=atoi(prop["inte"].c_str()))) o->data.inte = value;
				if (0!=(value=atoi(prop["dex"].c_str()))) o->data.dex = value;
				if (0!=(value=atoi(prop["spi"].c_str()))) o->data.spi = value;
				if (0!=(value=atoi(prop["con"].c_str()))) o->data.con = value;

				if (0!=(value=atoi(prop["xstr"].c_str()))) o->data.xstr = value;
				if (0!=(value=atoi(prop["xinte"].c_str()))) o->data.xinte = value;
				if (0!=(value=atoi(prop["xdex"].c_str()))) o->data.xdex = value;
				if (0!=(value=atoi(prop["xspi"].c_str()))) o->data.xspi = value;
				if (0!=(value=atoi(prop["xcon"].c_str()))) o->data.xcon = value;

				if (0!=(value=atoi(prop["fivetype"].c_str()))) o->data.fivetype = value;
				if ("0"==prop["fivetype"]) o->data.fivetype = 0;//金属性
				if (0!=(value=atoi(prop["fivepoint"].c_str()))) o->data.fivepoint = value;
				if (0!=(value=atoi(prop["kind"].c_str()))) o->data.kind = value;
				if (0!=(value=atoi(prop["upgrade"].c_str()))) o->data.upgrade = value;

				if (0!=(value=atoi(prop["doublexp"].c_str()))) o->data.doublexp = value;
				//if (0!=(value=atoi(prop["pdefup"].c_str()))) o->data.pdefup = value;
				//if (0!=(value=atoi(prop["mdefup"].c_str()))) o->data.mdefup = value;

				if (0!=(value=atoi(prop["poisondef"].c_str()))) o->data.poisondef = value;
				if (0!=(value=atoi(prop["lulldef"].c_str()))) o->data.lulldef = value;
				if (0!=(value=atoi(prop["reeldef"].c_str()))) o->data.reeldef = value;
				if (0!=(value=atoi(prop["evildef"].c_str()))) o->data.evildef = value;
				if (0!=(value=atoi(prop["bitedef"].c_str()))) o->data.bitedef = value;
				if (0!=(value=atoi(prop["stabledef"].c_str()))) o->data.stabledef = value;
				if (0!=(value=atoi(prop["slowdef"].c_str()))) o->data.slowdef = value;
				if (0!=(value=atoi(prop["luredef"].c_str()))) o->data.luredef = value;
				if (0!=(value=atoi(prop["colddef"].c_str()))) o->data.colddef = value;
				if (0!=(value=atoi(prop["petrifydef"].c_str()))) o->data.petrifydef = value;
				if (0!=(value=atoi(prop["blinddef"].c_str()))) o->data.blinddef = value;
				if (0!=(value=atoi(prop["chaosdef"].c_str()))) o->data.chaosdef = value;

				if (0!=(value=atoi(prop["poison"].c_str()))) o->data.poison = value;
				if (0!=(value=atoi(prop["lull"].c_str()))) o->data.lull = value;
				if (0!=(value=atoi(prop["reel"].c_str()))) o->data.reel = value;
				if (0!=(value=atoi(prop["evil"].c_str()))) o->data.evil = value;
				if (0!=(value=atoi(prop["bite"].c_str()))) o->data.bite = value;
				if (0!=(value=atoi(prop["stable"].c_str()))) o->data.stable = value;
				if (0!=(value=atoi(prop["slow"].c_str()))) o->data.slow = value;
				if (0!=(value=atoi(prop["lure"].c_str()))) o->data.lure = value;
				if (0!=(value=atoi(prop["cold"].c_str()))) o->data.cold = value;
				if (0!=(value=atoi(prop["petrify"].c_str()))) o->data.petrify = value;
				if (0!=(value=atoi(prop["blind"].c_str()))) o->data.blind = value;
				if (0!=(value=atoi(prop["chaos"].c_str()))) o->data.chaos = value;

				//if (0!=(value=atoi(prop["reflectp"].c_str()))) o->data.reflectp = value;
				//if (0!=(value=atoi(prop["reflect"].c_str()))) o->data.reflect = value;

				if (0!=(value=atoi(prop["durpoint"].c_str()))) o->data.durpoint = value;
				if (0!=(value=atoi(prop["dursecond"].c_str()))) o->data.dursecond = value;
				if (0!=(value=atoi(prop["holy"].c_str()))) o->data.holy = value;
				if (0!=(value=atoi(prop["bang"].c_str()))) o->data.bang = value;

				//if (0!=(value=atoi(prop["ehpp"].c_str()))) o->data.ehpp = value;
				//if (0!=(value=atoi(prop["ehpv"].c_str()))) o->data.ehpv = value;
				//if (0!=(value=atoi(prop["epdamp"].c_str()))) o->data.epdamp = value;
				//if (0!=(value=atoi(prop["epdamv"].c_str()))) o->data.epdamv = value;
				//if (0!=(value=atoi(prop["erdamp"].c_str()))) o->data.erdamp = value;
				//if (0!=(value=atoi(prop["erdamv"].c_str()))) o->data.erdamv = value;
				//if (0!=(value=atoi(prop["etdamp"].c_str()))) o->data.etdamp = value;
				//if (0!=(value=atoi(prop["etdamv"].c_str()))) o->data.etdamv = value;
				//if (0!=(value=atoi(prop["emdamp"].c_str()))) o->data.emdamp = value;
				//if (0!=(value=atoi(prop["emdamv"].c_str()))) o->data.emdamv = value;
				//if (0!=(value=atoi(prop["fivepro"].c_str()))) o->data.fivepro = value;
				if (0!=(value=atoi(prop["hpleech"].c_str()))) o->data.hpleech.odds = value;
				if (0!=(value=atoi(prop["hpleech_value"].c_str()))) o->data.hpleech.effect = value;
				if (0!=(value=atoi(prop["mpleech"].c_str()))) o->data.mpleech.odds = value;
				if (0!=(value=atoi(prop["mpleech_value"].c_str()))) o->data.mpleech.effect = value;

				if (0!=(value=atoi(prop["hptomp"].c_str()))) o->data.hptomp = value;
				//if (0!=(value=atoi(prop["dhpv"].c_str()))) o->data.dhpv = value;
				if (0!=(value=atoi(prop["dhpp"].c_str()))) o->data.dhpp = value;
				//if (0!=(value=atoi(prop["dmpv"].c_str()))) o->data.dmpv = value;
				if (0!=(value=atoi(prop["dmpp"].c_str()))) o->data.dmpp = value;

				if (0!=(value=atoi(prop["incgold"].c_str()))) o->data.incgold = value;
				if (0!=(value=atoi(prop["doublexp"].c_str()))) o->data.doublexp = value;
				if (0!=(value=atoi(prop["mf"].c_str()))) o->data.mf = value;

				if (0!=(value=atoi(prop["dpdam"].c_str()))) o->data.dpdam = value;         //物理伤害减少 x%
				if (0!=(value=atoi(prop["dmdam"].c_str()))) o->data.dmdam = value;         //魔法伤害减少 x%
				if (0!=(value=atoi(prop["bdam"].c_str()))) o->data.bdam = value;           //增加伤害 x%
				if (0!=(value=atoi(prop["rdam"].c_str()))) o->data.rdam = value;           //伤害反射 %x
				if (0!=(value=atoi(prop["ignoredef"].c_str()))) o->data.ignoredef = value; //%x 忽视目标防御
				//soke 第二灵魂
				if (0!=(value=atoi(prop["addpdef"].c_str()))) o->data.addpdef = value;     //物理防御力增强 x
				if (0!=(value=atoi(prop["addmdef"].c_str()))) o->data.addmdef = value;     //魔法防御力增强 x
				if (0!=(value=atoi(prop["addpdam"].c_str()))) o->data.addpdam = value;     //增加物理攻击力 x
				if (0!=(value=atoi(prop["addmdam"].c_str()))) o->data.addmdam = value;     //增加魔法攻击力 x
				if (0!=(value=atoi(prop["addhp"].c_str()))) o->data.addhp = value;         //增加生命值 x

				if (0!=(value=atoi(prop["fiveset0"].c_str()))) o->data.fiveset[0] = value;
				if (0!=(value=atoi(prop["fiveset1"].c_str()))) o->data.fiveset[1] = value;
				if (0!=(value=atoi(prop["fiveset2"].c_str()))) o->data.fiveset[2] = value;
				if (0!=(value=atoi(prop["fiveset3"].c_str()))) o->data.fiveset[3] = value;
				if (0!=(value=atoi(prop["fiveset4"].c_str()))) o->data.fiveset[4] = value;

				//if (0!=(value=atoi(prop["invtarget"].c_str()))) o->data.invtarget = value;

				if (0!=(value=atoi(prop["exp"].c_str()))) o->data.exp = value;
				if (0!=(value=atoi(prop["round"].c_str()))) o->data.round = value;
				if (0!=(value=atoi(prop["xingshu"].c_str()))) o->data.xingshu = value;
				if (""!=prop["maker"]) strncpy(o->data.maker, prop["maker"].c_str(), MAX_NAMESIZE-1); //soke 打造签名
				//if (""!=prop["set"]) strncpy(o->data.set, prop["set"].c_str(), MAX_NAMESIZE-1);

				if (0!=(value=atoi(prop["skill_id"].c_str()))) o->data.skills.id = value;
				if (0!=(value=atoi(prop["skill_lv"].c_str()))) o->data.skills.point = value;

				if (0!=(value=atoi(prop["sid1"].c_str()))) o->data.skill[0].id = value;
				if (0!=(value=atoi(prop["slv1"].c_str()))) o->data.skill[0].point = value;

				if (0!=(value=atoi(prop["sid2"].c_str()))) o->data.skill[1].id = value;
				if (0!=(value=atoi(prop["slv2"].c_str()))) o->data.skill[1].point = value;

				if (0!=(value=atoi(prop["sid3"].c_str()))) o->data.skill[2].id = value;
				if (0!=(value=atoi(prop["slv3"].c_str()))) o->data.skill[2].point = value;

				if (0!=(value=atoi(prop["sid4"].c_str()))) o->data.skill[3].id = value;
				if (0!=(value=atoi(prop["slv4"].c_str()))) o->data.skill[3].point = value;

				if (0!=(value=atoi(prop["sid5"].c_str()))) o->data.skill[4].id = value;
				if (0!=(value=atoi(prop["slv5"].c_str()))) o->data.skill[4].point = value;

				if (0!=(value=atoi(prop["sid6"].c_str()))) o->data.skill[5].id = value;
				if (0!=(value=atoi(prop["slv6"].c_str()))) o->data.skill[5].point = value;

				if (0!=(value=atoi(prop["sid7"].c_str()))) o->data.skill[6].id = value;
				if (0!=(value=atoi(prop["slv7"].c_str()))) o->data.skill[6].point = value;

				if (0!=(value=atoi(prop["sid8"].c_str()))) o->data.skill[7].id = value;
				if (0!=(value=atoi(prop["slv8"].c_str()))) o->data.skill[7].point = value;

				if (0!=(value=atoi(prop["sid9"].c_str()))) o->data.skill[8].id = value;
				if (0!=(value=atoi(prop["slv9"].c_str()))) o->data.skill[8].point = value;

				if (0!=(value=atoi(prop["sid10"].c_str()))) o->data.skill[9].id = value;
				if (0!=(value=atoi(prop["slv10"].c_str()))) o->data.skill[9].point = value;

				//如果是双倍经验道具和荣誉道具需要绑定
				// o->checkBind();
				Cmd::stAddObjectPropertyUserCmd send;
				bcopy(&o->data,&send.object,sizeof(t_Object));
				pUser->sendCmdToMe(&send,sizeof(send));
				Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "得到物品%s%ld个",o->name,o->data.dwNum);
				ScenesService::gm_logger->trace("GM:%s 创造了物品 %s %d 个", pUser->name, o->name, o->data.dwNum);
				return true;
			}
			zObject::destroy(o);
		}
		return false;
	}
	ScenesService::gm_logger->trace("GM:%s 试图创造不存在的物品 name=%s id=%s ", pUser->name, prop["name"].c_str(), prop["id"].c_str());
	return false;
}

/**
 * \brief 改造得到物品
 *
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 得到是否成功
 */
bool Gm::gzfetch(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	int _nKind =0;

	if ("" == prop["id"] && "" == prop["name"].c_str())
	{
		ScenesService::gm_logger->trace("GM:%s 使用gzfetch命令，参数错误", pUser->name);
		return false;
	}

	zObjectB *base = objectbm.get(atoi(prop["id"].c_str()));

	_nKind = atoi( prop["color"].c_str() );
	if(base)
		Zebra::logger->error("kind = %u",base->kind);
		
	if (NULL == base)
		base = objectbm.get(prop["name"].c_str());
	if (base)
	{
		//不能获得马匹
		if (base->kind==ItemType_HORSE || base->kind==ItemType_BattleHorse)
			return false;

		DWORD num=atoi(prop["num"].c_str());
		if (num>(DWORD)base->maxnum) num = base->maxnum;
		int level=atoi(prop["level"].c_str());
		if (level>11) level=11;
		DWORD hole=atoi(prop["hole"].c_str());

		DWORD AddPoint = 0;
		AddPoint = atoi(prop["point"].c_str());

		if (hole>6) hole=6;

		//天魔石最高6级
		if ((679==atoi(prop["id"].c_str()))||("天魔石"==prop["name"].c_str()))
			if (level>6) level=6;

		//定情物没有等级
		if ((682==atoi(prop["id"].c_str()))||("定情信物"==prop["name"].c_str()))
			level=0;

		if(num==0) num=1;
		//gm指令制造银子有内存泄漏,修改ok
		if(base->id == 665)
		{
			pUser->packs.addMoney(num,"GM指令创建");
			ScenesService::gm_logger->trace("GM:%s 创造了 %u 钱", pUser->name, num);
			return true;
		}
		zObject* o = NULL;
		if (base->recast) 
		{
			o = zObject::create(base,num, 0);
			if (o && level) 
			{
				do 
				{
					Upgrade::upgrade(*pUser, o, 100); //must success
				}while (--level >0); 
			}
		}
		else 
		{
			o = zObject::create(base,num, level);
		}

		if(o)
		{
			zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,o->data.dwNum,1,0,NULL,pUser->id,pUser->name,"fetch",o->base,o->data.kind,o->data.upgrade);
			//魂魄石
			if (o->base->kind == ItemType_SOULSTONE) 
			{
				DWORD monster_id = atoi(prop["monster"].c_str());
				if (!monster_id) monster_id=1;
				SoulStone::assign(o, monster_id);
			}
			else 
			{			
				if(o->base->make == 1)
				{
					BYTE type=atoi(prop["type"].c_str());
					EquipMaker maker(NULL);
					maker.assign(NULL, o, base,false, type);
					o->data.bind = 1;
				    strncpy(o->data.maker, pUser->name, MAX_NAMESIZE);//打造者名字
				}
				if (hole) 
				{
					while (hole--) o->data.socket[hole]=(DWORD)-1;
				}
			}
			if(o->data.dwNum!=0 && pUser->packs.addObject(o,true, AUTO_PACK))
			{		
				//手动设置物品属性
				if (""!=prop["strName"]) strncpy(o->data.strName, prop["strName"].c_str(), MAX_NAMESIZE-1);
				int value;
				//if (0!=(value=atoi(prop["sex"].c_str()))) o->data.sex = value;
				if (0!=(value=atoi(prop["color"].c_str()))) o->data.color = value;
				if (0!=(value=atoi(prop["bind"].c_str()))) o->data.bind = value;
				if (0!=(value=atoi(prop["needlevel"].c_str()))) o->data.needlevel = value;
				//if (0!=(value=atoi(prop["level"].c_str()))) o->data.level = value;
				//if (0!=(value=atoi(prop["hp"].c_str())))o->data.hp = value;
				if (0!=(value=atoi(prop["maxhp"].c_str()))) o->data.maxhp = value;
				//if (0!=(value=atoi(prop["mp"].c_str()))) o->data.mp = value;
				if (0!=(value=atoi(prop["maxmp"].c_str()))) o->data.maxmp = value;
				//if (0!=(value=atoi(prop["sp"].c_str()))) o->data.sp = value;
				if (0!=(value=atoi(prop["maxsp"].c_str()))) o->data.maxsp = value;
				if (0!=(value=atoi(prop["pdamage"].c_str()))) o->data.pdamage = value;
				if (0!=(value=atoi(prop["maxpdamage"].c_str()))) o->data.maxpdamage = value;
				if (0!=(value=atoi(prop["pdam"].c_str()))) o->data.pdam = value;
				//if (0!=(value=atoi(prop["maxrdam"].c_str()))) o->data.maxrdam = value;
				//if (0!=(value=atoi(prop["tdam"].c_str()))) o->data.tdam = value;
				//if (0!=(value=atoi(prop["maxtdam"].c_str()))) o->data.maxtdam = value;
				if (0!=(value=atoi(prop["mdamage"].c_str()))) o->data.mdamage = value;
				if (0!=(value=atoi(prop["maxmdamage"].c_str()))) o->data.maxmdamage = value;
				if (0!=(value=atoi(prop["pdefence"].c_str()))) o->data.pdefence = value;
				if (0!=(value=atoi(prop["pdef"].c_str()))) o->data.pdef = value;
				if (0!=(value=atoi(prop["mdefence"].c_str()))) o->data.mdefence = value;
				if (0!=(value=atoi(prop["mdam"].c_str()))) o->data.mdam = value;
				if (0!=(value=atoi(prop["mdef"].c_str()))) o->data.mdef = value;
				if (0!=(value=atoi(prop["hpr"].c_str()))) o->data.hpr = value;
				if (0!=(value=atoi(prop["mpr"].c_str()))) o->data.mpr = value;
				if (0!=(value=atoi(prop["spr"].c_str()))) o->data.spr = value;
				if (0!=(value=atoi(prop["akspeed"].c_str()))) o->data.akspeed = value;
				//if (0!=(value=atoi(prop["mgspeed"].c_str()))) o->data.mgspeed = value;
				if (0!=(value=atoi(prop["mvspeed"].c_str()))) o->data.mvspeed = value;
				if (0!=(value=atoi(prop["atrating"].c_str()))) o->data.atrating = value;
				if (0!=(value=atoi(prop["akdodge"].c_str()))) o->data.akdodge = value;
				//if (0!=(value=atoi(prop["mgrating"].c_str()))) o->data.mgrating = value;
				//if (0!=(value=atoi(prop["mgdodge"].c_str()))) o->data.mgdodge = value;
				if (0!=(value=atoi(prop["dur"].c_str()))) o->data.dur = value;
				if (0!=(value=atoi(prop["maxdur"].c_str()))) o->data.maxdur = value;
				if (0!=(value=atoi(prop["price"].c_str()))) o->data.price = value;
				if (0!=(value=atoi(prop["cardpoint"].c_str()))) o->data.cardpoint = value;

				if (0!=(value=atoi(prop["str"].c_str()))) o->data.str = value;
				if (0!=(value=atoi(prop["inte"].c_str()))) o->data.inte = value;
				if (0!=(value=atoi(prop["dex"].c_str()))) o->data.dex = value;
				if (0!=(value=atoi(prop["spi"].c_str()))) o->data.spi = value;
				if (0!=(value=atoi(prop["con"].c_str()))) o->data.con = value;

				if (0!=(value=atoi(prop["fivetype"].c_str()))) o->data.fivetype = value;
				if ("0"==prop["fivetype"]) o->data.fivetype = 0;//金属性
				if (0!=(value=atoi(prop["fivepoint"].c_str()))) o->data.fivepoint = value;
				if (0!=(value=atoi(prop["kind"].c_str()))) o->data.kind = value;
				if (0!=(value=atoi(prop["upgrade"].c_str()))) o->data.upgrade = value;

				if (0!=(value=atoi(prop["doublexp"].c_str()))) o->data.doublexp = value;
				//if (0!=(value=atoi(prop["pdefup"].c_str()))) o->data.pdefup = value;
				//if (0!=(value=atoi(prop["mdefup"].c_str()))) o->data.mdefup = value;

				if (0!=(value=atoi(prop["poisondef"].c_str()))) o->data.poisondef = value;
				if (0!=(value=atoi(prop["lulldef"].c_str()))) o->data.lulldef = value;
				if (0!=(value=atoi(prop["reeldef"].c_str()))) o->data.reeldef = value;
				if (0!=(value=atoi(prop["evildef"].c_str()))) o->data.evildef = value;
				if (0!=(value=atoi(prop["bitedef"].c_str()))) o->data.bitedef = value;
				if (0!=(value=atoi(prop["stabledef"].c_str()))) o->data.stabledef = value;
				if (0!=(value=atoi(prop["slowdef"].c_str()))) o->data.slowdef = value;
				if (0!=(value=atoi(prop["luredef"].c_str()))) o->data.luredef = value;
				if (0!=(value=atoi(prop["colddef"].c_str()))) o->data.colddef = value;
				if (0!=(value=atoi(prop["petrifydef"].c_str()))) o->data.petrifydef = value;
				if (0!=(value=atoi(prop["blinddef"].c_str()))) o->data.blinddef = value;
				if (0!=(value=atoi(prop["chaosdef"].c_str()))) o->data.chaosdef = value;

				if (0!=(value=atoi(prop["poison"].c_str()))) o->data.poison = value;
				if (0!=(value=atoi(prop["lull"].c_str()))) o->data.lull = value;
				if (0!=(value=atoi(prop["reel"].c_str()))) o->data.reel = value;
				if (0!=(value=atoi(prop["evil"].c_str()))) o->data.evil = value;
				if (0!=(value=atoi(prop["bite"].c_str()))) o->data.bite = value;
				if (0!=(value=atoi(prop["stable"].c_str()))) o->data.stable = value;
				if (0!=(value=atoi(prop["slow"].c_str()))) o->data.slow = value;
				if (0!=(value=atoi(prop["lure"].c_str()))) o->data.lure = value;
				if (0!=(value=atoi(prop["cold"].c_str()))) o->data.cold = value;
				if (0!=(value=atoi(prop["petrify"].c_str()))) o->data.petrify = value;
				if (0!=(value=atoi(prop["blind"].c_str()))) o->data.blind = value;
				if (0!=(value=atoi(prop["chaos"].c_str()))) o->data.chaos = value;

				//if (0!=(value=atoi(prop["reflectp"].c_str()))) o->data.reflectp = value;
				//if (0!=(value=atoi(prop["reflect"].c_str()))) o->data.reflect = value;

				if (0!=(value=atoi(prop["durpoint"].c_str()))) o->data.durpoint = value;
				if (0!=(value=atoi(prop["dursecond"].c_str()))) o->data.dursecond = value;
				if (0!=(value=atoi(prop["holy"].c_str()))) o->data.holy = value;
				if (0!=(value=atoi(prop["bang"].c_str()))) o->data.bang = value;

				//if (0!=(value=atoi(prop["ehpp"].c_str()))) o->data.ehpp = value;
				//if (0!=(value=atoi(prop["ehpv"].c_str()))) o->data.ehpv = value;
				//if (0!=(value=atoi(prop["epdamp"].c_str()))) o->data.epdamp = value;
				//if (0!=(value=atoi(prop["epdamv"].c_str()))) o->data.epdamv = value;
				//if (0!=(value=atoi(prop["erdamp"].c_str()))) o->data.erdamp = value;
				//if (0!=(value=atoi(prop["erdamv"].c_str()))) o->data.erdamv = value;
				//if (0!=(value=atoi(prop["etdamp"].c_str()))) o->data.etdamp = value;
				//if (0!=(value=atoi(prop["etdamv"].c_str()))) o->data.etdamv = value;
				//if (0!=(value=atoi(prop["emdamp"].c_str()))) o->data.emdamp = value;
				//if (0!=(value=atoi(prop["emdamv"].c_str()))) o->data.emdamv = value;
				//if (0!=(value=atoi(prop["fivepro"].c_str()))) o->data.fivepro = value;
				if (0!=(value=atoi(prop["hpleech"].c_str()))) o->data.hpleech.odds = value;
				if (0!=(value=atoi(prop["hpleech_value"].c_str()))) o->data.hpleech.effect = value;
				if (0!=(value=atoi(prop["mpleech"].c_str()))) o->data.mpleech.odds = value;
				if (0!=(value=atoi(prop["mpleech_value"].c_str()))) o->data.mpleech.effect = value;

				if (0!=(value=atoi(prop["hptomp"].c_str()))) o->data.hptomp = value;
				//if (0!=(value=atoi(prop["dhpv"].c_str()))) o->data.dhpv = value;
				if (0!=(value=atoi(prop["dhpp"].c_str()))) o->data.dhpp = value;
				//if (0!=(value=atoi(prop["dmpv"].c_str()))) o->data.dmpv = value;
				if (0!=(value=atoi(prop["dmpp"].c_str()))) o->data.dmpp = value;

				if (0!=(value=atoi(prop["incgold"].c_str()))) o->data.incgold = value;
				if (0!=(value=atoi(prop["doublexp"].c_str()))) o->data.doublexp = value;
				if (0!=(value=atoi(prop["mf"].c_str()))) o->data.mf = value;

				if (0!=(value=atoi(prop["dpdam"].c_str()))) o->data.dpdam = value;
				if (0!=(value=atoi(prop["dmdam"].c_str()))) o->data.dmdam = value;
				if (0!=(value=atoi(prop["bdam"].c_str()))) o->data.bdam = value;
				if (0!=(value=atoi(prop["rdam"].c_str()))) o->data.rdam = value;
				if (0!=(value=atoi(prop["ignoredef"].c_str()))) o->data.ignoredef = value;

				if (0!=(value=atoi(prop["fiveset0"].c_str()))) o->data.fiveset[0] = value;
				if (0!=(value=atoi(prop["fiveset1"].c_str()))) o->data.fiveset[1] = value;
				if (0!=(value=atoi(prop["fiveset2"].c_str()))) o->data.fiveset[2] = value;
				if (0!=(value=atoi(prop["fiveset3"].c_str()))) o->data.fiveset[3] = value;
				if (0!=(value=atoi(prop["fiveset4"].c_str()))) o->data.fiveset[4] = value;

				//if (0!=(value=atoi(prop["invtarget"].c_str()))) o->data.invtarget = value;

				if (0!=(value=atoi(prop["exp"].c_str()))) o->data.exp = value;
				if (""!=prop["maker"]) strncpy(o->data.maker, prop["maker"].c_str(), MAX_NAMESIZE-1); //soke 打造签名
				//if (""!=prop["set"]) strncpy(o->data.set, prop["set"].c_str(), MAX_NAMESIZE-1);

				if (0!=(value=atoi(prop["skill_id"].c_str()))) o->data.skills.id = value;
				if (0!=(value=atoi(prop["skill_lv"].c_str()))) o->data.skills.point = value;

				if (0!=(value=atoi(prop["sid1"].c_str()))) o->data.skill[0].id = value;
				if (0!=(value=atoi(prop["slv1"].c_str()))) o->data.skill[0].point = value;

				if (0!=(value=atoi(prop["sid2"].c_str()))) o->data.skill[1].id = value;
				if (0!=(value=atoi(prop["slv2"].c_str()))) o->data.skill[1].point = value;

				if (0!=(value=atoi(prop["sid3"].c_str()))) o->data.skill[2].id = value;
				if (0!=(value=atoi(prop["slv3"].c_str()))) o->data.skill[2].point = value;

				if (0!=(value=atoi(prop["sid4"].c_str()))) o->data.skill[3].id = value;
				if (0!=(value=atoi(prop["slv4"].c_str()))) o->data.skill[3].point = value;

				if (0!=(value=atoi(prop["sid5"].c_str()))) o->data.skill[4].id = value;
				if (0!=(value=atoi(prop["slv5"].c_str()))) o->data.skill[4].point = value;

				if (0!=(value=atoi(prop["sid6"].c_str()))) o->data.skill[5].id = value;
				if (0!=(value=atoi(prop["slv6"].c_str()))) o->data.skill[5].point = value;

				if (0!=(value=atoi(prop["sid7"].c_str()))) o->data.skill[6].id = value;
				if (0!=(value=atoi(prop["slv7"].c_str()))) o->data.skill[6].point = value;

				if (0!=(value=atoi(prop["sid8"].c_str()))) o->data.skill[7].id = value;
				if (0!=(value=atoi(prop["slv8"].c_str()))) o->data.skill[7].point = value;

				if (0!=(value=atoi(prop["sid9"].c_str()))) o->data.skill[8].id = value;
				if (0!=(value=atoi(prop["slv9"].c_str()))) o->data.skill[8].point = value;

				if (0!=(value=atoi(prop["sid10"].c_str()))) o->data.skill[9].id = value;
				if (0!=(value=atoi(prop["slv10"].c_str()))) o->data.skill[9].point = value;

				//soke GM改造得到物品都是绑定的
			    o->data.bind = 1;
			//	strncpy(o->data.maker, pUser->name, MAX_NAMESIZE);//打造者名字
				Cmd::stAddObjectPropertyUserCmd send;
				bcopy(&o->data,&send.object,sizeof(t_Object));
				pUser->sendCmdToMe(&send,sizeof(send));
				Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "得到物品%s%ld个",o->name,o->data.dwNum);
				ScenesService::gm_logger->trace("GM:%s 改造创造了物品 %s %d 个", pUser->name, o->name, o->data.dwNum);
				return true;
			}
			zObject::destroy(o);
		}
		return false;
	}
	ScenesService::gm_logger->trace("GM:%s 试图改造不存在的物品 name=%s id=%s ", pUser->name, prop["name"].c_str(), prop["id"].c_str());
	return false;
}

/**
 * \brief 召唤npc
 *
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 得到是否成功
 */
bool Gm::summon(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	//prop.dump(std::cout);

	if ("" == prop["id"] && "" == prop["name"].c_str())
	{
		ScenesService::gm_logger->trace("GM:%s 使用summon命令，参数错误。位置 %s (%d,%d)", pUser->name, pUser->scene->name, pUser->getPos().x, pUser->getPos().y);
		return false;
	}
	zNpcB *base = npcbm.get(atoi(prop["id"].c_str()));
	if (NULL == base)
		base = npcbm.get(prop["name"].c_str());
	if (base)
	{
		t_NpcDefine define;
		zPos pos = pUser->getPos();
		define.id = base->id;
		strcpy(define.name, base->name);
		if ("" == prop["x"]
				|| "" == prop["y"])
		{
			define.pos = pUser->getPos();
		}
		else
		{
			define.pos.x = atoi(prop["x"].c_str());
			define.pos.y = atoi(prop["y"].c_str());
		}
		define.width = SCREEN_WIDTH;
		define.height = SCREEN_HEIGHT;
		define.num = atoi(prop["num"].c_str());
		define.num = define.num >? 1;
		define.num = define.num <? 100;
		define.interval = 30;
		define.initstate = zSceneEntry::SceneEntry_Normal;
		define.scriptID = atoi(prop["script"].c_str());

		pUser->scene->zPosRevaluate(define.pos);
		define.pos -= zPosHalfScreen;
		pUser->scene->zPosRevaluate(define.pos);
		pos -= zPosHalfScreen;
		pUser->scene->zPosRevaluate(pos);
		pUser->scene->initRegion(define.region, define.pos, define.width, define.height);
		int num = pUser->scene->summonNpc(define, pos, base);

		ScenesService::gm_logger->trace("GM:%s 召唤怪物 %s %d 个。位置 %s (%d,%d)", pUser->name, define.name, num, pUser->scene->name, define.pos.x, define.pos.y);
		return true;
	}

	ScenesService::gm_logger->trace("GM:%s 使用summon命令，参数错误。位置 %s (%d,%d)", pUser->name, pUser->scene->name, pUser->getPos().x, pUser->getPos().y);
	return false;
}

/*\brief GM给自己升级
 * 
 * 超级GM可以用这条命令给别的用户升级
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 升级是否成功
 */
bool Gm::levelup(SceneUser *pUser,const char *para)
{
	if (0==pUser) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	int num=atoi(prop["num"].c_str());
	int level2=atoi(Zebra::global["serverfy_level"].c_str()); //soke 未转生封印等级从配置读取
	int level3=atoi(Zebra::global["servermax_level"].c_str());//soke  未转生最大等级从配置读取

	if (level2 <= level3 && pUser->charbase.round <= 0)
	{
	    if (num>level2) num = level2;
      //if (num>MAX_LEVEL) num = MAX_LEVEL;
	    if (num <=0) num = 1;
	
	    //给自己升级
	    if (""==prop["name"] || !strncmp(pUser->name, prop["name"].c_str(), MAX_NAMESIZE))
	    {
		    if (pUser->upgrade(num))
		    {
			    Channel::sendSys(pUser,Cmd::INFO_TYPE_GAME,"您升到%d级了",pUser->charbase.level);
			    ScenesService::gm_logger->trace("GM %s 使用levelup指令， 提升了自己的等级 num=%d", pUser->name, num);
			    return true;
		    }
		    else
		    {
			    Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"升级失败");
			    ScenesService::gm_logger->trace("GM %s 使用levelup指令， 提升自己的等级失败, num=%d", pUser->name, num);
			    return false;
		    }
	    }
        else//给别人升级
        {
		    //只有超级GM才可以给其他人升级
		    if (!(pUser->getPriv() & Gm::super_mode) && !(pUser->getPriv() & Gm::debug_mode))
		    {
			    Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"你不能提升别人的等级！权限不足");
			    ScenesService::gm_logger->trace("GM %s 使用levelup指令， 试图提升 %s 的等级, 权限不足", pUser->name, prop["name"].c_str());
			    return false;
		    }
		    else
		    {
			    //没有锁管理器
			    SceneUser * pTarget=SceneUserManager::getMe().getUserByName(prop["name"].c_str());

			    if (pTarget)//玩家在线
			    {
				    if (pUser->getPriv() <= pTarget->getPriv())
				    {                       
					    Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"只能操作比自己级别低的玩家!");
					    ScenesService::gm_logger->trace("GM %s 使用用levelup命令，试图使 %s 升级，权限不足", pUser->name, prop["name"].c_str());                                    
					    return false;
				    }
				    if (pTarget->upgrade(num))
				    {
					    Channel::sendSys(pTarget, Cmd::INFO_TYPE_GAME,"恭喜！%s 提升了你的等级 %d", pUser->name, num);
					    Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"提升 %s 的等级 %d 成功", pTarget->name, num);
					    ScenesService::gm_logger->trace("GM %s 使用levelup指令， 提升了 %s 的等级。 num=%d", pUser->name, prop["name"].c_str(), num);
					    return true;
				    }
				    else
				    {
					    Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"升级失败");
					    ScenesService::gm_logger->trace("GM %s 使用levelup指令， 提升 %s 的等级失败, num=%d", pUser->name, prop["name"].c_str(), num);
					    return false;
				    }
			    }
			    else//玩家不在线或不在当前场景服务器
			    {
				    Cmd::Session::t_gmCommand_SceneSession cmd;
				    cmd.gm_cmd = Cmd::Session::GM_COMMAND_LEVELUP;
				    cmd.src_priv = pUser->getPriv();
				    cmd.x = num;//参数，要提升的等级数
				    strncpy((char *)cmd.src_name, pUser->name, MAX_NAMESIZE-1);
				    strncpy((char *)cmd.dst_name, prop["name"].c_str(), MAX_NAMESIZE-1);

				    sessionClient->sendCmd(&cmd, sizeof(cmd));
				    return true;
			    }
		    }
		}
	}
	//soke 增加封印等级溢出说明
	if (level2 > level3)
	{
		 Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"当前服务器未转生最大等级：%d级，提升等级失败",level3);
		 return false;
    }

	//soke 从配置文件中读取最大转生上限配置
	DWORD MaxRound=atoi(Zebra::global["trun_maxround"].c_str());
	int level4=atoi(Zebra::global["trunfy_level"].c_str()); //soke 转生封印等级从配置读取
	int level5=atoi(Zebra::global["maxtrun_level"].c_str());//soke  转生最大等级从配置读取

	if (level4<=level5 && (pUser->charbase.round > 0 && pUser->charbase.round <= MaxRound) )
	{
	    if (num>level4) num = level4;
      //if (num>MAX_LEVEL) num = MAX_LEVEL;
	    if (num <=0) num = 1;
	
	    //给自己升级
	    if (""==prop["name"] || !strncmp(pUser->name, prop["name"].c_str(), MAX_NAMESIZE))
	    {
		    if (pUser->upgrade(num))
		    {
			    Channel::sendSys(pUser,Cmd::INFO_TYPE_GAME,"您升到%d级了",pUser->charbase.level);
			    ScenesService::gm_logger->trace("GM %s 使用levelup指令， 提升了自己的等级 num=%d", pUser->name, num);
			    return true;
		    }
		    else
		    {
			    Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"升级失败");
			    ScenesService::gm_logger->trace("GM %s 使用levelup指令， 提升自己的等级失败, num=%d", pUser->name, num);
			    return false;
		    }
	    }
        else//给别人升级
        {
		    //只有超级GM才可以给其他人升级
		    if (!(pUser->getPriv() & Gm::super_mode) && !(pUser->getPriv() & Gm::debug_mode))
		    {
			    Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"你不能提升别人的等级！权限不足");
			    ScenesService::gm_logger->trace("GM %s 使用levelup指令， 试图提升 %s 的等级, 权限不足", pUser->name, prop["name"].c_str());
			    return false;
		    }
		    else
		    {
			    //没有锁管理器
			    SceneUser * pTarget=SceneUserManager::getMe().getUserByName(prop["name"].c_str());

			    if (pTarget)//玩家在线
			    {
				    if (pUser->getPriv() <= pTarget->getPriv())
				    {                       
					    Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"只能操作比自己级别低的玩家!");
					    ScenesService::gm_logger->trace("GM %s 使用用levelup命令，试图使 %s 升级，权限不足", pUser->name, prop["name"].c_str());                                    
					    return false;
				    }
				    if (pTarget->upgrade(num))
				    {
					    Channel::sendSys(pTarget, Cmd::INFO_TYPE_GAME,"恭喜！%s 提升了你的等级 %d", pUser->name, num);
					    Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"提升 %s 的等级 %d 成功", pTarget->name, num);
					    ScenesService::gm_logger->trace("GM %s 使用levelup指令， 提升了 %s 的等级。 num=%d", pUser->name, prop["name"].c_str(), num);
					    return true;
				    }
				    else
				    {
					    Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"升级失败");
					    ScenesService::gm_logger->trace("GM %s 使用levelup指令， 提升 %s 的等级失败, num=%d", pUser->name, prop["name"].c_str(), num);
					    return false;
				    }
			    }
			    else//玩家不在线或不在当前场景服务器
			    {
				    Cmd::Session::t_gmCommand_SceneSession cmd;
				    cmd.gm_cmd = Cmd::Session::GM_COMMAND_LEVELUP;
				    cmd.src_priv = pUser->getPriv();
				    cmd.x = num;//参数，要提升的等级数
				    strncpy((char *)cmd.src_name, pUser->name, MAX_NAMESIZE-1);
				    strncpy((char *)cmd.dst_name, prop["name"].c_str(), MAX_NAMESIZE-1);

				    sessionClient->sendCmd(&cmd, sizeof(cmd));
				    return true;
			    }
		    }
		}
	}
	//soke 增加转生封印等级溢出说明
	if (level4 > level5)
	{
		 Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"当前服务器转生最大等级：%d级，提升等级失败",level5);
		 return false;
    }
	return false;
}

/*\brief 地图跳转
 * 
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 跳转是否成功
 */
bool Gm::gomap_Gm(SceneUser *pUser,const char *para)
{
	bool ignoreUserLevel = false;
	zProperties prop;
	prop.parseCmdLine(para);

	if ("true" == prop["ignoreUserLevel"])
	{
		ignoreUserLevel = true;
	}
	if ("" == prop["name"])
	{
		//		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"请指定地图和坐标，如：//gomap name=XXX pos=100,100");
		ScenesService::gm_logger->trace("GM %s 使用gomap命令，参数错误", pUser->name);
		return false;
	}
	pUser->charbase.gomaptype=atoi(prop["type"].c_str());
	std::string name = prop["name"];
	if(strstr(prop["name"].c_str() , "·") == NULL)
	{
		SceneManager::CountryMap_iter country_iter
			=SceneManager::getInstance().country_info.find(pUser->scene->getCountryID());
		if(country_iter != SceneManager::getInstance().country_info.end() && !(country_iter->second.function &0x01)/*不能跳到未开放国家*/ && pUser->scene->getCountryID()!=6)
		{
			name = country_iter->second.name;
			name+= "·";
			name+= prop["name"];
		}
		else
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "未开国家");
			Zebra::logger->debug("%s(%u)不能跳到未开放国家",pUser->name,pUser->id);
			return false;
		}
	}
	else
	{
		char country_name[MAX_NAMESIZE+1];
		bzero(country_name,sizeof(country_name));
		strncpy(country_name,name.c_str(),MAX_NAMESIZE);
		char *real = strstr(country_name , "·");
		if(real)
		{
			*real=0;
		}
		SceneManager::CountryMap_iter country_iter=SceneManager::getInstance().country_info.find(SceneManager::getInstance().getCountryIDByCountryName(country_name));
		if(country_iter != SceneManager::getInstance().country_info.end() && (country_iter->second.function &0x01)/*不能跳到未开放国家*/ && country_iter->first!=6)
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "未开国家");
			Zebra::logger->debug("%s(%u)不能跳到未开放国家",pUser->name,pUser->id);
			return false;
		}
	}
	Scene * scene=SceneManager::getInstance().getSceneByName(name.c_str());

	zPos Pos;
	Pos.x = 0;
	Pos.y = 0;
	bool findPos=true;

	if (""!=prop["pos"])
	{
		zRegex regex;
		if((regex.compile("(.*)[ ,](.*)") && regex.match(prop["pos"].c_str())))
		{
			std::string s;
			regex.getSub(s,1);
			Pos.x=atoi(s.c_str());
			regex.getSub(s,2);
			Pos.y=atoi(s.c_str());
			findPos=false; 
		}
	}
	else if (""!=prop["cpos"])
	{
		zPos cPos;
		zRegex regex;
		if((regex.compile("(.*)[ ,](.*)") && regex.match(prop["cpos"].c_str())))
		{
			std::string s;
			regex.getSub(s,1);
			cPos.x=atoi(s.c_str());
			regex.getSub(s,2);
			cPos.y=atoi(s.c_str());
			int rlen = atoi(prop["rlen"].c_str());
			if(!rlen)
			{
				rlen=6;
			}
			Pos.x=zMisc::randBetween((cPos.x - rlen)>0?(cPos.x - rlen):0,cPos.x + rlen);
			Pos.y=zMisc::randBetween((cPos.y - rlen)>0?(cPos.y - rlen):0,cPos.y + rlen);
			findPos=false; 
		}
	}

	if(scene)
	{
		if ((!ignoreUserLevel)&&(!scene->checkUserLevel(pUser))&&(pUser->scene!=scene)) return false;

		if (Pos.x>scene->width()) Pos.x = scene->width();
		if (Pos.y>scene->height()) Pos.y = scene->height();
		if (!findPos) return pUser->changeMap(scene,Pos, ignoreUserLevel);

		const WayPoint *wp=pUser->scene->getWayPoint(scene->getFileName());
		if(wp==NULL)//没有从pUser->scene到scene的跳转点
		{
			wp=scene->getRandWayPoint();//随便找一个scene上的跳转点
			if(wp)
			{
				const zPos *pos=wp->getRandPoint();
				bool suc = pUser->changeMap(scene,*pos, ignoreUserLevel);
				if (suc)
				{
					ScenesService::gm_logger->trace("GM %s 使用gomap命令，目的 %s (%d,%d)", pUser->name, scene->name, pos->x, pos->y);
					return true;
				}
				else
				{
					ScenesService::gm_logger->trace("GM %s 使用gomap命令，跳转地图失败，目的 %s (%d,%d)", pUser->name, scene->name, pos->x, pos->y);
					return false;
				}
			}
			else
			{
				//				Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"地图上没有跳转点");
				ScenesService::gm_logger->trace("GM %s 使用gomap命令，地图 %s 没有跳转点", pUser->name, name.c_str());
				return false;
			}
		}
		else
		{
			const zPos pos=wp->getRandDest()->pos;
			bool suc = pUser->changeMap(scene,pos, ignoreUserLevel);
			if (suc)
			{
				ScenesService::gm_logger->trace("GM %s 使用gomap命令，目的 %s (%d,%d)", pUser->name, scene->name, pos.x, pos.y);
				return true;
			}
			else
			{
				ScenesService::gm_logger->trace("GM %s 使用gomap命令，跳转地图失败，目的 %s (%d,%d)", pUser->name, scene->name, pos.x, pos.y);
				return false;
			}
		}
	}
	else
	{
		//ScenesService::gm_logger->debug("切换场景服务器(%s, %d, %d)", dp->name, dp->pos.x, dp->pos.y);
		if (pUser->guard && pUser->guard->canMove()) pUser->saveGuard = true;//使镖车跟随指令使用者
		if (pUser->adoptList.size()) pUser->saveAdopt = true;
		Cmd::Session::t_changeScene_SceneSession cmd;
		cmd.id = pUser->id;
		cmd.temp_id = pUser->tempid;
		cmd.x = Pos.x;
		cmd.y = Pos.y;
		cmd.map_id = 0;
		cmd.map_file[0] = '\0';
		strncpy((char *)cmd.map_name, name.c_str(), MAX_NAMESIZE);
		sessionClient->sendCmd(&cmd, sizeof(cmd));
		return true;
	}
}

/*\brief 地图跳转
 * 
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 跳转是否成功
 */
bool Gm::gomap(SceneUser *pUser,const char *para)
{

	if(pUser->isBike() || pUser->isZC() || pUser->isSpa())
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"当前状态不允许传送");	
		return false;
	}
	bool ignoreUserLevel = false;
	zProperties prop;
	prop.parseCmdLine(para);

	if ("true" == prop["ignoreUserLevel"])
	{
		ignoreUserLevel = true;
	}
	if ("" == prop["name"])
	{
		return false;
	}
	pUser->charbase.gomaptype=atoi(prop["type"].c_str());
	std::string name = prop["name"];
	if(strstr(prop["name"].c_str() , "·") == NULL)
	{
		SceneManager::CountryMap_iter country_iter
			=SceneManager::getInstance().country_info.find(pUser->scene->getCountryID());
		if(country_iter != SceneManager::getInstance().country_info.end() && !(country_iter->second.function &0x01)/*不能跳到未开放国家*/ && pUser->scene->getCountryID()!=6)
		{
			name = country_iter->second.name;
			name+= "·";
			name+= prop["name"];
		}
		else
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "未开国家");
			Zebra::logger->debug("%s(%u)不能跳到未开放国家",pUser->name,pUser->id);
			return false;
		}
	}
	else
	{
		char country_name[MAX_NAMESIZE+1];
		bzero(country_name,sizeof(country_name));
		strncpy(country_name,name.c_str(),MAX_NAMESIZE);
		char *real = strstr(country_name , "·");
		if(real)
		{
			*real=0;
		}
		SceneManager::CountryMap_iter country_iter=SceneManager::getInstance().country_info.find(SceneManager::getInstance().getCountryIDByCountryName(country_name));
		if(country_iter != SceneManager::getInstance().country_info.end() && (country_iter->second.function &0x01)/*不能跳到未开放国家*/ && country_iter->first!=6)
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "未开国家");
			Zebra::logger->debug("%s(%u)不能跳到未开放国家",pUser->name,pUser->id);
			return false;
		}
	}
	Scene * scene=SceneManager::getInstance().getSceneByName(name.c_str());

	zPos Pos;
	Pos.x = 0;
	Pos.y = 0;
	bool findPos=true;

	if (""!=prop["pos"])
	{
		zRegex regex;
		if((regex.compile("(.*)[ ,](.*)") && regex.match(prop["pos"].c_str())))
		{
			std::string s;
			regex.getSub(s,1);
			Pos.x=atoi(s.c_str());
			regex.getSub(s,2);
			Pos.y=atoi(s.c_str());
			findPos=false; 
		}
	}
	else if (""!=prop["cpos"])
	{
		zPos cPos;
		zRegex regex;
		if((regex.compile("(.*)[ ,](.*)") && regex.match(prop["cpos"].c_str())))
		{
			std::string s;
			regex.getSub(s,1);
			cPos.x=atoi(s.c_str());
			regex.getSub(s,2);
			cPos.y=atoi(s.c_str());
			int rlen = atoi(prop["rlen"].c_str());
			if(!rlen)
			{
				rlen=6;
			}
			Pos.x=zMisc::randBetween((cPos.x - rlen)>0?(cPos.x - rlen):0,cPos.x + rlen);
			Pos.y=zMisc::randBetween((cPos.y - rlen)>0?(cPos.y - rlen):0,cPos.y + rlen);
			findPos=false; 
		}
	}

	if(scene)
	{
		if ((!ignoreUserLevel)&&(!scene->checkUserLevel(pUser))&&(pUser->scene!=scene)) return false;

		if (Pos.x>scene->width()) Pos.x = scene->width();
		if (Pos.y>scene->height()) Pos.y = scene->height();
		if (!findPos) return pUser->changeMap(scene,Pos, ignoreUserLevel);

		const WayPoint *wp=pUser->scene->getWayPoint(scene->getFileName());
		if(wp==NULL)//没有从pUser->scene到scene的跳转点
		{
			wp=scene->getRandWayPoint();//随便找一个scene上的跳转点
			if(wp)
			{
				const zPos *pos=wp->getRandPoint();
				return pUser->changeMap(scene,*pos, ignoreUserLevel);
			}
			else
				return false;
		}
		else
		{
			const zPos pos=wp->getRandDest()->pos;
			return pUser->changeMap(scene,pos, ignoreUserLevel);
		}
	}
	else
	{
		if (pUser->guard && pUser->guard->canMove()) pUser->saveGuard = true;//使镖车跟随指令使用者
		if (pUser->adoptList.size()) pUser->saveAdopt = true;
		Cmd::Session::t_changeScene_SceneSession cmd;
		cmd.id = pUser->id;
		cmd.temp_id = pUser->tempid;
		cmd.x = Pos.x;
		cmd.y = Pos.y;
		cmd.map_id = 0;
		cmd.map_file[0] = '\0';
		strncpy((char *)cmd.map_name, name.c_str(), MAX_NAMESIZE);
		sessionClient->sendCmd(&cmd, sizeof(cmd));
		return true;
	}
}

/*\brief 瞬移
 * 
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 是否成功
 */
bool Gm::goTo_Gm(SceneUser *pUser,const char *para)
{
	char para_1[MAX_CHATINFO + 1];
	bzero(para_1,sizeof(para_1));
	strncpy(para_1,para,MAX_CHATINFO);
	while(strlen(para_1) && para_1[strlen(para_1)-1] == 32)
	{
		para_1[strlen(para_1)-1] = 0;
	}
	zRegex regex;
	zPos newPos;
	//Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"你的坐标是(%ld,%ld),城镇中心在(265,290)",pUser->getPos().x,pUser->getPos().y);
	if((regex.compile("(.*)[ ,](.*)") && regex.match(para_1)))
	{
		std::string s;
		regex.getSub(s,1);
		newPos.x=atoi(s.c_str());
		regex.getSub(s,2);
		//newPos.y=atoi(trim(s.c_str()));
		newPos.y=atoi(s.c_str());
		if(newPos.x>pUser->scene->width() || newPos.y>pUser->scene->height()) return false;
	}
	else
	{
		ScenesService::gm_logger->trace("GM %s 使用goto命令，参数错误 %s", pUser->name, para);
		return false;
	}
	bool suc = pUser->goTo(newPos);
	if (suc)
	{
		ScenesService::gm_logger->trace("GM %s 使用goto命令，目的 %s (%d,%d)", pUser->name, pUser->scene->name, newPos.x, newPos.y);
		return true;
	}
	else
	{
		ScenesService::gm_logger->trace("GM %s 使用goto命令失败，目的 %s (%d,%d)", pUser->name, pUser->scene->name, newPos.x, newPos.y);
		return false;
	}
}


/*\brief 瞬移
 * 
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 是否成功
 */
bool Gm::goTo(SceneUser *pUser,const char *para)
{
	char para_1[MAX_CHATINFO + 1];
	bzero(para_1,sizeof(para_1));
	strncpy(para_1,para,MAX_CHATINFO);
	while(strlen(para_1) && para_1[strlen(para_1)-1] == 32)
	{
		para_1[strlen(para_1)-1] = 0;
	}
	zRegex regex;
	zPos newPos;
	if((regex.compile("(.*)[ ,](.*)") && regex.match(para_1)))
	{
		std::string s;
		regex.getSub(s,1);
		newPos.x=atoi(s.c_str());
		regex.getSub(s,2);
		newPos.y=atoi(s.c_str());
		if(newPos.x>pUser->scene->width() || newPos.y>pUser->scene->height()) return false;
	}
	else
	{
		return false;
	}
	return pUser->goTo(newPos);
}

/*\brief 给技能升级
 * 
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 升级是否成功
 */
bool Gm::upgradeSkill(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["id"])
	{
		ScenesService::gm_logger->trace("GM %s 使用upskill命令，参数错误 para=%s", pUser->name, para);
		return false;
	}

	bool suc = pUser->upgradeSkill(atoi(prop["id"].c_str()));
	if (suc)
	{
		ScenesService::gm_logger->trace("GM %s 使用upskill命令，id=%s", pUser->name, prop["id"].c_str());
		return true;
	}
	else
	{
		ScenesService::gm_logger->trace("GM %s 使用upskill命令失败，id=%s", pUser->name, prop["id"].c_str());
		return false;
	}
}

/*\brief 放弃任务
 * 
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 放弃是否成功
 */
bool Gm::abandon(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["id"])
	{
		ScenesService::gm_logger->trace("GM %s 使用abandon命令，参数错误 para=%s", pUser->name, para);
		return false;
	}

	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"清除任务(%s)", prop["id"].c_str());
	pUser->quest_list.abandon(*pUser, atoi(prop["id"].c_str()), true);
	ScenesService::gm_logger->trace("GM %s 使用abandon命令，取消任务 id=%s", pUser->name, prop["id"].c_str());
	return true;
}

/**
 * \brief GM得到玩家所在的区域、位置
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 得到是否成功
 */
bool Gm::finduser(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数");
		ScenesService::gm_logger->trace("GM %s 使用finduser命令，参数错误 para=%s", pUser->name, para);
		return false;
	}
	SceneUser *pTarget=SceneUserManager::getMe().getUserByName(prop["name"].c_str());
	if(pTarget)
	{					
		//比较权限级别
		if (pUser->getPriv() <= pTarget->getPriv())
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"只能操作比自己级别低的玩家!");
			ScenesService::gm_logger->trace("GM %s 使用finduser命令，试图得到 %s 的位置，权限不足", pUser->name, prop["name"].c_str());
			return false;
		}

		zPos Pos=pTarget->getPos();
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"玩家 %s 在区域:%s 坐标:(%d,%d)",prop["name"].c_str(),pTarget->scene->getName(),Pos.x,Pos.y);
		ScenesService::gm_logger->trace("GM %s 使用finduser命令，得到 %s 的位置", pUser->name, prop["name"].c_str());
		return true;
	}
	else
	{
		Cmd::Session::t_gmCommand_SceneSession cmd;
		bzero(cmd.src_name,MAX_NAMESIZE);
		bzero(cmd.dst_name,MAX_NAMESIZE);
		bzero(cmd.map_file,MAX_NAMESIZE);
		bzero(cmd.map_name,MAX_NAMESIZE);
		cmd.gm_cmd = Cmd::Session::GM_COMMAND_FINDUSER;
		cmd.cmd_state = Cmd::Session::GM_COMMAND_STATE_REQ;
		cmd.id = pUser->id;
		cmd.src_priv = pUser->getPriv();
		strncpy((char *)&cmd.src_name, pUser->name, MAX_NAMESIZE);
		strncpy((char *)&cmd.dst_name, prop["name"].c_str(), MAX_NAMESIZE);
		return sessionClient->sendCmd(&cmd, sizeof(cmd));
	}
}

/**
 * \brief 把GM带到一个玩家身边
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::gotouser(SceneUser *pUser,const char *para)
{               
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "缺少参数");
		ScenesService::gm_logger->trace("GM %s 使用gotouser命令，参数错误 para=%s", pUser->name, para);
		return false;
	}
	SceneUser *pTarget=SceneUserManager::getMe().getUserByName(prop["name"].c_str());
	if(pTarget)
	{
		//比较权限级别
		if (pUser->getPriv() <= pTarget->getPriv())
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "只能操作比自己级别低的玩家!");
			ScenesService::gm_logger->trace("GM %s 使用gotouser命令，试图到达 %s 的位置，权限不足", pUser->name, prop["name"].c_str());
			return false;
		}

		//位置
		std::string mapName = "name=";
		mapName += pTarget->scene->getName();
		//坐标
		zPos Pos = pTarget->getPos();
		char position[MAX_NAMESIZE];
		bzero(position,sizeof(position));
		sprintf(position,"%d,%d",Pos.x,Pos.y);
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,position);

		//跳转
		gomap(pUser,mapName.c_str());
		goTo(pUser,position);

		ScenesService::gm_logger->trace("GM %s 使用gotouser命令，到达 %s 的位置", pUser->name, prop["name"].c_str());
		return true;
	}       
	else    
	{
		Cmd::Session::t_gmCommand_SceneSession cmd;
		bzero(cmd.src_name,MAX_NAMESIZE);
		bzero(cmd.dst_name,MAX_NAMESIZE);
		bzero(cmd.map_file,MAX_NAMESIZE);
		bzero(cmd.map_name,MAX_NAMESIZE);
		cmd.gm_cmd = Cmd::Session::GM_COMMAND_GOTOUSER;
		cmd.cmd_state = Cmd::Session::GM_COMMAND_STATE_REQ;
		cmd.id = pUser->id;
		cmd.src_priv = pUser->getPriv();
		strncpy((char *)&cmd.src_name, pUser->name, MAX_NAMESIZE);
		strncpy((char *)&cmd.dst_name, prop["name"].c_str(), MAX_NAMESIZE);
		return sessionClient->sendCmd(&cmd, sizeof(cmd));
	}
}

/**
 * \brief 补偿一个玩家股票
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::givestock(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数");
		ScenesService::gm_logger->trace("GM %s 使用catchuser命令，参数错误 para=%s", pUser->name, para);
		return false;
	}
	DWORD gold=atoi(prop["gold"].c_str());
	DWORD money=atoi(prop["money"].c_str());
	DWORD accid=atoi(prop["accid"].c_str());
	SceneUser *pTarget=SceneUserManager::getMe().getUserByName(prop["name"].c_str());
	if(pTarget && (accid == pTarget->charbase.accid) && (pUser->scene->getCountryID() == pTarget->charbase.country))
	{
		Cmd::Bill::t_Stock_Save save;
		save.dwMoney=money;
		save.dwGold=gold;
		pTarget->sendSceneCmdToBill(&save,sizeof(save));
		ScenesService::gm_logger->trace("GM %s 使用givestock命令，给玩家%s(%d)补偿股票金币:%d,股票银币:%d", pUser->name, pTarget->name,pTarget->id,gold,money);
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "给玩家%s(%d)补偿股票操作成功",pTarget->name,pTarget->charbase.accid);
		return true;
	}
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "给玩家补偿银币操作失败,原因:1.玩家不在线;2.名字和accid不对应;3.不在玩家所在国家");
	return false;
}
/**
 * \brief 补偿一个玩家银币
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::givemoney(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数");
		ScenesService::gm_logger->trace("GM %s 使用catchuser命令，参数错误 para=%s", pUser->name, para);
		return false;
	}
	DWORD num=atoi(prop["num"].c_str());
	DWORD accid=atoi(prop["accid"].c_str());
	if((strncmp(prop["type"].c_str(),"股票提现",MAX_NAMESIZE)!=0))
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "给玩家补偿银币操作失败,原因:类型不正确");
		return false;
	}
	SceneUser *pTarget=SceneUserManager::getMe().getUserByName(prop["name"].c_str());
	if(pTarget && (accid == pTarget->charbase.accid) && (pUser->scene->getCountryID() == pTarget->charbase.country))
	{
		pTarget->packs.addMoney(num,prop["type"].c_str());
		ScenesService::gm_logger->trace("GM %s 使用givemoney命令，给玩家%s(%d)补偿银币:%d,类型:%s", pUser->name, pTarget->name,pTarget->id,num,prop["type"].c_str());
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "给玩家%s(%d)补偿银币操作成功",pTarget->name,pTarget->charbase.accid);
		return true;
	}
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "给玩家补偿银币操作失败,原因:1.玩家不在线;2.名字和accid不对应;3.不在玩家所在国家");
	return false;
}
/**
 * \brief 补偿一个玩家金币
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::givegold(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数");
		ScenesService::gm_logger->trace("GM %s 使用catchuser命令，参数错误 para=%s", pUser->name, para);
		return false;
	}
	DWORD num=atoi(prop["num"].c_str());
	DWORD accid=atoi(prop["accid"].c_str());
	if((strncmp(prop["type"].c_str(),"股票提现",MAX_NAMESIZE)!=0) && (strncmp(prop["type"].c_str(),"点卡换金币",MAX_NAMESIZE)!=0))
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "给玩家补偿金币操作失败,原因:类型不正确");
		return false;
	}
	SceneUser *pTarget=SceneUserManager::getMe().getUserByName(prop["name"].c_str());
	if(pTarget && (accid == pTarget->charbase.accid) && (pUser->scene->getCountryID() == pTarget->charbase.country))
	{
		if(strncmp(prop["type"].c_str(),"点卡换金子",MAX_NAMESIZE)==0)
		{
			DWORD temp=num/300;
			if(temp)
			{
				pTarget->charbase.goldgive +=temp;
				Channel::sendSys(pTarget, Cmd::INFO_TYPE_GAME, "你得到赠品精致升级宝石%d个,你可以到本人这里领哦",temp);
			}
		}
		pTarget->packs.addGold(num,prop["type"].c_str());
		ScenesService::gm_logger->trace("GM %s 使用givegold命令，给玩家%s(%d)补偿金币:%d,类型:%s", pUser->name, pTarget->name,pTarget->id,num,prop["type"].c_str());
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "给玩家%s(%d)补偿金币操作成功",pTarget->name,pTarget->charbase.accid);
		return true;
	}
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "给玩家补偿银币操作失败,原因:1.玩家不在线;2.名字和accid不对应;3.不在玩家所在国家");
	return false;
}

/**
 * \brief 把一个玩家带到GM身边
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::catchuser(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数");
		ScenesService::gm_logger->trace("GM %s 使用catchuser命令，参数错误 para=%s", pUser->name, para);
		return false;
	}
	SceneUser *pTarget=SceneUserManager::getMe().getUserByName(prop["name"].c_str());
	if(pTarget)
	{
		//比较权限级别
		if (pUser->getPriv() <= pTarget->getPriv())
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"只能操作比自己级别低的玩家!");
			ScenesService::gm_logger->trace("GM %s 使用catchuser命令，试图捕捉 %s ，权限不足", pUser->name, prop["name"].c_str());
			return false;
		}

		//地图
		std::string mapName = "name=";
		mapName += pUser->scene->getName();
		//坐标
		zPos Pos = pUser->getPos();
		char position[MAX_NAMESIZE];
		bzero(position,sizeof(position));
		sprintf(position,"%d,%d",Pos.x,Pos.y);
		//Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,position);

		//坐标
		gomap(pTarget,mapName.c_str());
		goTo(pTarget,position);

		ScenesService::gm_logger->trace("GM %s 使用catchuser命令，到达 %s 的位置", pUser->name, prop["name"].c_str());
		return true;
	}
	else
	{
		Cmd::Session::t_gmCommand_SceneSession cmd;
		bzero(cmd.src_name,MAX_NAMESIZE);
		bzero(cmd.dst_name,MAX_NAMESIZE);
		bzero(cmd.map_file,MAX_NAMESIZE);
		bzero(cmd.map_name,MAX_NAMESIZE);
		cmd.gm_cmd = Cmd::Session::GM_COMMAND_CATCHUSER;
		cmd.cmd_state = Cmd::Session::GM_COMMAND_STATE_REQ;
		cmd.id = pUser->id;
		cmd.src_priv = pUser->getPriv();
		cmd.x = pUser->getPos().x;
		cmd.y = pUser->getPos().y;
		strncpy((char *)&cmd.src_name, pUser->name, MAX_NAMESIZE);
		strncpy((char *)&cmd.dst_name, prop["name"].c_str(), MAX_NAMESIZE);
		strncpy((char *)&cmd.map_name, pUser->scene->name, MAX_NAMESIZE);
		return sessionClient->sendCmd(&cmd, sizeof(cmd));
	}
}

/**
 * \brief 把一个玩家丢到活动室囚禁
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::embar(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数");
		ScenesService::gm_logger->trace("GM %s 使用catchuser命令，参数错误 para=%s", pUser->name, para);
		return false;
	}
	SceneUser *pTarget=SceneUserManager::getMe().getUserByName(prop["name"].c_str());
	if(pTarget)
	{
		//比较权限级别
		if (pUser->getPriv() <= pTarget->getPriv())
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"只能操作比自己级别低的玩家!");
			ScenesService::gm_logger->trace("GM %s 使用embar命令，试图捕捉 %s ，权限不足", pUser->name, prop["name"].c_str());
			return false;
		}

		//地图
		std::string mapName = "name=活动室 pos=65,54 ignoreUserLevel=true";
		//Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,position);

		//坐标
		pTarget->killAllPets();
		gomap(pTarget,mapName.c_str());

		ScenesService::gm_logger->trace("GM %s 使用embar命令，到达 %s 的位置", pUser->name, prop["name"].c_str());
		return true;
	}
	else
	{
		Cmd::Session::t_gmCommand_SceneSession cmd;
		bzero(cmd.src_name,MAX_NAMESIZE);
		bzero(cmd.dst_name,MAX_NAMESIZE);
		bzero(cmd.map_file,MAX_NAMESIZE);
		bzero(cmd.map_name,MAX_NAMESIZE);
		cmd.gm_cmd = Cmd::Session::GM_COMMAND_CATCHUSER;
		cmd.cmd_state = Cmd::Session::GM_COMMAND_STATE_REQ;
		cmd.id = pUser->id;
		cmd.src_priv = pUser->getPriv();
		cmd.x = 65;
		cmd.y = 54;
		strncpy((char *)&cmd.src_name, pUser->name, MAX_NAMESIZE);
		strncpy((char *)&cmd.dst_name, prop["name"].c_str(), MAX_NAMESIZE);
		strncpy((char *)&cmd.map_name, "活动室", MAX_NAMESIZE);
		return sessionClient->sendCmd(&cmd, sizeof(cmd));
	}
}


/**
 * \brief  金币系统控制
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::goldsystem(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	if(!strcmp(prop["service"].c_str(),"start"))
	{
		Cmd::Scene::t_ServiceGold_GateScene ret;
		ret.byType=Cmd::Scene::SERVICE_START;
		Zebra::logger->debug("%s(%d)启动金币服务",pUser->name,pUser->id);
		SceneTaskManager::getInstance().broadcastCmd(&ret,sizeof(ret));
	}
	if(!strcmp(prop["service"].c_str(),"stop"))
	{
		Cmd::Scene::t_ServiceGold_GateScene ret;
		ret.byType=Cmd::Scene::SERVICE_STOP;
		Zebra::logger->debug("%s(%d)停止金币服务",pUser->name,pUser->id);
		SceneTaskManager::getInstance().broadcastCmd(&ret,sizeof(ret));
	}
	return true;
}
/**
 * \brief  股票系统控制
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::stocksystem(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	if(!strcmp(prop["service"].c_str(),"start"))
	{
		Cmd::Scene::t_ServiceStock_GateScene ret;
		ret.byType=Cmd::Scene::SERVICE_START;
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"启动股票服务成功");
		Zebra::logger->debug("%s(%d)启动股票服务",pUser->name,pUser->id);
		SceneTaskManager::getInstance().broadcastCmd(&ret,sizeof(ret));
	}
	if(!strcmp(prop["service"].c_str(),"stop"))
	{
		Cmd::Scene::t_ServiceStock_GateScene ret;
		ret.byType=Cmd::Scene::SERVICE_STOP;
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"停止股票服务成功");
		Zebra::logger->debug("%s(%d)停止股票服务",pUser->name,pUser->id);
		SceneTaskManager::getInstance().broadcastCmd(&ret,sizeof(ret));
	}
	return true;
}
/**
 * \brief  股票委托管理
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::stockconsign(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	if(!strcmp(prop["put"].c_str(),"true"))
	{
		Cmd::stPutListStockUserCmd ret;
		ret.dwNum=atoi(prop["num"].c_str());
		ret.dwPrice=atoi(prop["price"].c_str());
		ret.byType=atoi(prop["type"].c_str());
		pUser->sendCmdToBill(&ret , sizeof(ret));
	}
	if(!strcmp(prop["cancel"].c_str(),"true"))
	{
		Cmd::stRequestCancelListStockUserCmd ret;
		ret.id=atoi(prop["id"].c_str());
		ret.byType=atoi(prop["type"].c_str());
		pUser->sendCmdToBill(&ret , sizeof(ret));
	}
	if(!strcmp(prop["cancelall"].c_str(),"true"))
	{
		Cmd::t_CancelListAllStock_GateScene ret;
		pUser->sendCmdToBill(&ret , sizeof(ret));
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"股票休市成功");
		Zebra::logger->debug("%s(%d)股票休市",pUser->name,pUser->id);
	}
	if(!strcmp(prop["history"].c_str(),"true"))
	{
		Cmd::stRequstHistoryStockUserCmd ret;
		pUser->sendCmdToBill(&ret , sizeof(ret));
	}
	if(!strcmp(prop["myhistory"].c_str(),"true"))
	{
		Cmd::stRequstSelfHistoryStockUserCmd ret;
		ret.begintime=atoi(prop["time"].c_str());
		ret.num=atoi(prop["num"].c_str());
		pUser->sendCmdToBill(&ret , sizeof(ret));
	}
	return true;
}
/**
 * \brief  股票资金管理
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::stocktransfer(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	if(!strcmp(prop["query"].c_str(),"true"))
	{
		Cmd::stQueryFundStockUserCmd qfs;
		pUser->sendCmdToBill(&qfs , sizeof(qfs));
	}
	if(!strcmp(prop["save"].c_str(),"true"))
	{
		pUser->stockSave(atoi(prop["money"].c_str()),atoi(prop["gold"].c_str()));
		/*
		Cmd::stTransferFundStockSaveUserCmd tfs;
		tfs.dwGold=atoi(prop["gold"].c_str());
		tfs.dwMoney=atoi(prop["money"].c_str());
		pUser->sendCmdToBill(&tfs , sizeof(tfs));
		// */
	}
	else if(!strcmp(prop["fetch"].c_str(),"true"))
	{
		Cmd::stTransferFundStockFetchUserCmd tff;
		tff.dwGold=atoi(prop["gold"].c_str());
		tff.dwMoney=atoi(prop["money"].c_str());
		pUser->sendCmdToBill(&tff , sizeof(tff));
	}
	return true;
}
/**
 * \brief  股票用户
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::stockuser(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	Cmd::stPassowrdStockUserCmd ret;
	strncpy(ret.byPawword,prop["password"].c_str(),sizeof(ret.byPawword));
	strncpy(ret.byNew1,prop["new1"].c_str(),sizeof(ret.byNew1));
	strncpy(ret.byNew2,prop["new2"].c_str(),sizeof(ret.byNew2));
	pUser->sendCmdToBill(&ret , sizeof(ret));
	//pUser->gatetask->sendCmd(&ret , sizeof(ret));
	return true;
}
/**
 * \brief  兑换月卡
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::redeemmonthcard(SceneUser *pUser,const char *para)
{

	return true;
}
/**
 * \brief 兑换金币 
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::redeemgold(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	DWORD point = atoi(prop["point"].c_str());
	Cmd::Scene::t_Request_RedeemGold rcg;
	rcg.dwUserID = pUser->id;
	rcg.dwNum = point>0?point:1;
	pUser->gatetask->sendCmd(&rcg , sizeof(rcg));
	return true;
}
/**
 * \brief  查询月卡和金币
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::querygold(SceneUser *pUser,const char *para)
{
	Cmd::Scene::t_Request_Bill rcg;
	rcg.dwUserID = pUser->id;
	pUser->gatetask->sendCmd(&rcg , sizeof(rcg));
	return true;
}
/**
 * \brief 查询剩余点卡数
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::querypoint(SceneUser *pUser,const char *para)
{
	Cmd::Scene::t_Request_Point rb;
	rb.dwUserID=pUser->id;
	pUser->gatetask->sendCmd(&rb , sizeof(rb));
	return true;
}
/**
 * \brief 扔东西
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::throwobject(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	DWORD type = atoi(prop["type"].c_str());
	
	if(type == 0) {
		pUser->packs.execEvery(&pUser->packs.main, Type2Type<DropFromPack>(), DropFromPack::Param(&pUser->packs.main, pUser->packs.main.size(), pUser->getPos()));
	}
	if(type == 1) {
		pUser->packs.execEvery(&pUser->packs.equip, Type2Type<DropFromPack>(), DropFromPack::Param(&pUser->packs.equip, pUser->packs.equip.size(), pUser->getPos()));
		if(pUser->packs.equip.needRecalc)
		{
			pUser->setupCharBase();
			Cmd::stMainUserDataUserCmd  userinfo;
			pUser->full_t_MainUserData(userinfo.data);
			pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
			pUser->sendMeToNine();
		}
	}

#if 0
//TODOBYLQY
	zProperties prop;
	prop.parseCmdLine(para);

	DWORD type = atoi(prop["type"].c_str());
	std::vector<zObject *> temp_vec;
	int begin = 0;
	std::set<zObject *>::iterator iter;
	if(type == 0)
	{
		for(iter = pUser->packs.main.getAllset().begin(); iter != pUser->packs.main.getAllset().end() ; iter ++)
		{
			//TODO 其它不可掉落物品
			if((*iter)->data.upgrade > 5 || (*iter)->data.bind || (*iter)->data.pos.y == Cmd::EQUIPCELLTYPE_PACKAGE || (*iter)->data.pos.y == Cmd::EQUIPCELLTYPE_MAKE || (*iter)->base->kind==ItemType_MASK)
			{
				continue;
			}
			temp_vec.push_back(*iter);
		}
		int mainpack = temp_vec.size();
		for(int i = begin; i < mainpack ;  i ++)
		{
			pUser->packs.moveObjectToScene(&*temp_vec[i] , pUser->getPos());
		}
	}
	if(type == 1)
	{
		bool needRecalc = false;
		begin = 0;
		temp_vec.clear();
		for(iter = pUser->packs.equip.getAllset().begin(); iter != pUser->packs.equip.getAllset().end() ; iter ++)
		{
			//TODO 其它不可掉落物品
			if((*iter)->data.upgrade > 5 || (*iter)->data.bind || (*iter)->data.pos.y == Cmd::EQUIPCELLTYPE_PACKAGE || (*iter)->data.pos.y == Cmd::EQUIPCELLTYPE_MAKE )
			{
				continue;
			}
			temp_vec.push_back(*iter);
		}
		int	equippack = temp_vec.size();
		for(int i = begin; i < equippack ;  i ++)
		{
			pUser->packs.moveObjectToScene(&*temp_vec[i] , pUser->getPos());
			needRecalc = true;
		}
		if(needRecalc)
		{
			pUser->setupCharBase();
			Cmd::stMainUserDataUserCmd  userinfo;
			pUser->full_t_MainUserData(userinfo.data);
			pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
			pUser->sendMeToNine();
		}
	}
#endif
	return true;
}
/**
 * \brief 组队:mode:模式,name:队员
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::team(SceneUser *pLeader,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{
		Channel::sendSys(pLeader, Cmd::INFO_TYPE_FAIL,"缺少参数");
		ScenesService::gm_logger->trace("GM %s 使用team命令，参数错误 para=%s", pLeader->name, para);
		return false;
	}
	SceneUser *pUser=SceneUserManager::getMe().getUserByName(prop["name"].c_str());
	if(pUser)
	{
		DWORD mode = atoi(prop["mode"].c_str());
		//设置组队模式
		if(!pLeader->team.IsTeamed())
		{
			if(mode < Cmd::TEAM_MAX)
			{
				pLeader->team_mode=mode;
			}
			else
			{
				Zebra::logger->iffy("收到无效模式的邀请队员指令(%s , %ld)" , pLeader->name , pLeader->id);
				return false;
			}
		}
		else
		{
			if(pLeader->team_mode != mode)
			{
				Channel::sendSys(pLeader , Cmd::INFO_TYPE_FAIL, "一个队伍只能使用一种组队模式");
				return false;
			}
		}
		if(pLeader->team_mode == Cmd::TEAM_HONOR) 
		{
			if(pLeader->charbase.level < TEAM_HONOR_LEADER_LEVEL)
			{
				Channel::sendSys(pLeader , Cmd::INFO_TYPE_FAIL, "荣誉模式需要队长等级%d级",TEAM_HONOR_LEADER_LEVEL);
				return false;
			}
			if(pUser->charbase.level >= TEAM_HONOR_MEMBER_LEVEL)
			{
				Channel::sendSys(pLeader , Cmd::INFO_TYPE_FAIL, "荣誉模式队员等级不能大于%d级",TEAM_HONOR_MEMBER_LEVEL-1);
				return false;
			}
		}
		if(!pUser->team.IsOpen())
		{
			Channel::sendSys(pLeader , Cmd::INFO_TYPE_FAIL, "对方组队未开启");
			return false;
		}
		if(pUser->team.IsTeamed())
		{
			Channel::sendSys(pLeader , Cmd::INFO_TYPE_FAIL, "对方已有组队");
			return false;
		}
		
		if (pUser->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER)
		||  pUser->issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
		{
			Channel::sendSys(pLeader , Cmd::INFO_TYPE_FAIL, "对方处于护宝状态，不能组队");
			return false;
		}

		Cmd::stRequestTeamUserCmd ret;
		ret.dwAnswerUserID = pLeader->tempid;
		ret.dwTeamMode = pLeader->team_mode;
		pUser->sendCmdToMe(&ret , sizeof(ret));
	}
	return false;
}

/**
 * \brief GM发送区域广播消息
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::bczone(SceneUser *pUser,const char *para)
{
	if ((NULL==pUser)||(NULL==para))
		return false;
	if (!strcmp(para,""))
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数");
		ScenesService::gm_logger->trace("GM %s 使用bczone命令，参数错误 para=%s", pUser->name, para);
		return false;
	}

	//消息
	Cmd::stChannelChatUserCmd send;
	zRTime ctv;
	send.dwType=Cmd::CHAT_TYPE_GM;
	send.dwChatTime = ctv.sec();
	strncpy((char *)send.pstrChat,para,MAX_CHATINFO-1);

	/*
	//隐藏身份
	if (pUser->mask.is_masking()) 
	{
	strncpy((char *)send.pstrName, "蒙面人",MAX_NAMESIZE);
	}
	else 
	 */
	{
		strncpy((char *)send.pstrName,pUser->name,MAX_NAMESIZE);
	}

	pUser->scene->sendCmdToScene(&send,sizeof(send));

	ScenesService::gm_logger->trace("GM %s 使用bczone命令，para=%s", pUser->name, para);
	return true;
}

/**
 * \brief GM发送世界广播消息
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::bcworld(SceneUser *pUser,const char *para)
{
	if ((NULL==pUser)||(NULL==para))
		return false;
	if (""==para)
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数");
		ScenesService::gm_logger->trace("GM %s 使用bcworld命令，参数错误 para=%s", pUser->name, para);
		return false;
	}

	//消息
	Cmd::stChannelChatUserCmd send;
	zRTime ctv;
	send.dwType = Cmd::CHAT_TYPE_GM;
	send.dwChatTime = ctv.sec();
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));
	strncpy((char *)send.pstrChat, para,MAX_CHATINFO-1);
	/*
	//隐藏身份
	if (pUser->mask.is_masking()) 
	{
	strncpy((char *)send.pstrName, "蒙面人", MAX_NAMESIZE);
	}
	else 
	 */
	{
		strncpy((char *)send.pstrName, pUser->name, MAX_NAMESIZE);
	}
	pUser->forwardSession(&send, sizeof(Cmd::stChannelChatUserCmd));

	ScenesService::gm_logger->trace("GM %s 使用bcworld命令，para=%s", pUser->name, para);
	return true;
}

/**
 * \brief GM发送世界广播消息
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::bcwgamemsg(SceneUser *pUser,const char *para)
{
	if ((NULL==pUser)||(NULL==para))
		return false;
	if (""==para)
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数");
		ScenesService::gm_logger->trace("GM %s 使用bcwgamemsg命令，参数错误 para=%s", pUser->name, para);
		return false;
	}

	//消息
	Cmd::stChannelChatUserCmd send;
	zRTime ctv;
	send.dwType = Cmd::CHAT_TYPE_GM;
	send.dwSysInfoType = Cmd::INFO_TYPE_EXP;
	send.dwChatTime = ctv.sec();
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));
	strncpy((char *)send.pstrChat, para,MAX_CHATINFO-1);
	/*
	//隐藏身份
	if (pUser->mask.is_masking()) 
	{
	strncpy((char *)send.pstrName, "蒙面人", MAX_NAMESIZE);
	}
	else 
	 */
	{
		strncpy((char *)send.pstrName, pUser->name, MAX_NAMESIZE);
	}
	pUser->forwardSession(&send, sizeof(Cmd::stChannelChatUserCmd));

	ScenesService::gm_logger->trace("GM %s 使用bcwgamemsg命令，para=%s", pUser->name, para);
	return true;
}

/**
 * \brief GM使一个玩家不能发言
 * 如：//donttalk name=xwl time=86400
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::donttalk(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{       
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，请指定玩家名字");
		ScenesService::gm_logger->trace("GM %s 使用donttalk命令，参数错误 para=%s", pUser->name, para);
		return false;
	}

	int delay = atoi(prop["time"].c_str());
	if (86400<delay)
		delay = 86400;//最多禁言1天
	if (0>=delay)
		delay = 600;//默认禁言1小时

	SceneUser *pTarget=SceneUserManager::getMe().getUserByName(prop["name"].c_str());
	if(pTarget)
	{
		//比较权限级别
		if (pUser->getPriv() <= pTarget->getPriv())
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"只能操作比自己级别低的玩家!");
			ScenesService::gm_logger->trace("GM %s 使用donttalk命令，试图使 %s 禁言，权限不足", pUser->name, prop["name"].c_str());
			return false;
		}
		pTarget->delayForbidTalk(delay);
		Channel::sendSys(pTarget, Cmd::INFO_TYPE_GAME,"你被 %s 禁言 %d 秒", pUser->name, delay);
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"玩家 %s 被禁言 %d 秒", pTarget->name, delay);
		ScenesService::gm_logger->trace("GM %s 使用donttalk命令，玩家 %s 禁言 %d 秒", pUser->name, prop["name"].c_str(), delay);
		return true;
	}
	else    
	{
		Cmd::Session::t_gmCommand_SceneSession cmd;
		bzero(cmd.src_name,MAX_NAMESIZE);
		bzero(cmd.dst_name,MAX_NAMESIZE);
		bzero(cmd.map_file,MAX_NAMESIZE);
		bzero(cmd.map_name,MAX_NAMESIZE);
		cmd.gm_cmd = Cmd::Session::GM_COMMAND_DONTTALK;
		cmd.cmd_state = Cmd::Session::GM_COMMAND_STATE_REQ;
		cmd.id = pUser->id;
		cmd.src_priv = pUser->getPriv();
		cmd.x = delay;
		cmd.y = 0;
		strncpy((char *)&cmd.src_name, pUser->name, MAX_NAMESIZE);
		strncpy((char *)&cmd.dst_name, prop["name"].c_str(), MAX_NAMESIZE);
		return sessionClient->sendCmd(&cmd, sizeof(cmd));
	}
}

/**
 * \brief GM使一个玩家恢复发言
 * 如：//talk name=xwl
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::talk(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	SceneUser *pTarget=NULL;

	if ("" == prop["name"])
	{       
		/*                Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，请指定玩家名字");
				  ScenesService::gm_logger->trace("GM %s 使用talk命令，参数错误 para=%s", pUser->name, para);
				  return false;*/
		pTarget = pUser;
	}
	else
	{
		pTarget=SceneUserManager::getMe().getUserByName(prop["name"].c_str());
	}

	if(pTarget)
	{
		//比较权限级别
		if ((pUser->getPriv() <= pTarget->getPriv()) && (pUser->getPriv()!=Gm::super_mode))
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"只能操作比自己级别低的玩家!");
			ScenesService::gm_logger->trace("GM %s 使用talk命令，试图使 %s 恢复禁言，权限不足", pUser->name, prop["name"].c_str());
			return false;
		}

		if (pTarget->isForbidTalk())
		{
			pTarget->delayForbidTalk(0);
			Channel::sendSys(pTarget, Cmd::INFO_TYPE_GAME,"%s 解除了你的禁言，现在可以说话了", pUser->name);
			ScenesService::gm_logger->trace("GM %s 使用talk命令，解除玩家 %s 的禁言", pUser->name, prop["name"].c_str());
		}
		return true;
	}
	else    
	{
		Cmd::Session::t_gmCommand_SceneSession cmd;
		bzero(cmd.src_name,MAX_NAMESIZE);
		bzero(cmd.dst_name,MAX_NAMESIZE);
		bzero(cmd.map_file,MAX_NAMESIZE);
		bzero(cmd.map_name,MAX_NAMESIZE);
		cmd.gm_cmd = Cmd::Session::GM_COMMAND_TALK;
		cmd.cmd_state = Cmd::Session::GM_COMMAND_STATE_REQ;
		cmd.id = pUser->id;
		cmd.src_priv = pUser->getPriv();
		cmd.x = 0;
		cmd.y = 0;
		strncpy((char *)&cmd.src_name, pUser->name, MAX_NAMESIZE);
		strncpy((char *)&cmd.dst_name, prop["name"].c_str(), MAX_NAMESIZE);
		return sessionClient->sendCmd(&cmd, sizeof(cmd));
	}
}


/**
 * \brief GM踢掉一个玩家
 * 如：//kick name=xwl
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::kick(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{       
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，请指定玩家名字");
		ScenesService::gm_logger->trace("GM %s 使用kick命令，参数错误 para=%s", pUser->name, para);
		return false;
	}
	SceneUser *pTarget=SceneUserManager::getMe().getUserByName(prop["name"].c_str());
	if(pTarget && pUser)
	{       
		//比较权限级别
		if (pUser->getPriv() <= pTarget->getPriv())
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"不能踢比自己高级别的玩家!");
			ScenesService::gm_logger->trace("GM %s 使用kick命令，试图把 %s 踢出服务器，权限不足", pUser->name, prop["name"].c_str());
			return false;
		}

		//SceneUserManager::getMe().lock();
		Channel::sendSys(pTarget, Cmd::INFO_TYPE_FAIL,"你已经被%s踢出服务器！",pUser->name);

		OnQuit event(1);
		EventTable::instance().execute(*pTarget, event);

		pTarget->save(Cmd::Record::LOGOUT_WRITEBACK);
		pTarget->killAllPets();

		//通知session服务器
		Cmd::Session::t_unregUser_SceneSession ret;
		ret.dwUserID=pTarget->id;
		ret.dwSceneTempID=pTarget->scene->tempid;
		ret.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
		sessionClient->sendCmd(&ret,sizeof(ret));

		//通知网关服务器
		Cmd::Scene::t_Unreg_LoginScene retgate;
		retgate.dwUserID = pTarget->id;
		retgate.dwSceneTempID = pTarget->scene->tempid;
		retgate.retcode = Cmd::Scene::UNREGUSER_RET_ERROR;
		pTarget->gatetask->sendCmd(&retgate,sizeof(retgate));

		pTarget->unreg();
		ScenesService::gm_logger->trace("GM %s 使用kick命令，把 %s 踢出服务器", pUser->name, prop["name"].c_str());
		return true;
	}
	else    
	{
		Cmd::Session::t_gmCommand_SceneSession cmd;
		bzero(cmd.src_name,MAX_NAMESIZE);
		bzero(cmd.dst_name,MAX_NAMESIZE);
		bzero(cmd.map_file,MAX_NAMESIZE);
		bzero(cmd.map_name,MAX_NAMESIZE);
		cmd.gm_cmd = Cmd::Session::GM_COMMAND_KICK;
		cmd.cmd_state = Cmd::Session::GM_COMMAND_STATE_REQ;
		cmd.src_priv = pUser->getPriv();
		strncpy((char *)&cmd.src_name, pUser->name, MAX_NAMESIZE);
		strncpy((char *)&cmd.dst_name, prop["name"].c_str(), MAX_NAMESIZE);
		return sessionClient->sendCmd(&cmd, sizeof(cmd));
	}
}

bool Gm::kickGateUser(SceneUser *pUser,const char *para)
{
	if (!pUser || !para) return false;
	zProperties prop;
	prop.parseCmdLine(para);

	Cmd::Scene::t_kickUser_GateScene send;
	send.userID = atoi(prop["userid"].c_str());
	send.accid = atoi(prop["accid"].c_str());

	struct SceneTaskSendCmdExec :public execEntry<SceneTask>
	{       
		void *_data; 
		DWORD _size;
		SceneTaskSendCmdExec(void *data , DWORD size):_data(data),_size(size){} 
		bool exec(SceneTask *task)
		{       
			if(task)
			{       
				task->sendCmd(_data,_size);
			}       
			return true;
		}       
	};      
	SceneTaskSendCmdExec exec(&send , sizeof(Cmd::Scene::t_kickUser_GateScene)); 
	SceneTaskManager::getInstance().execEverySceneTask(exec);
	return true;
}

/**
 * \brief 显示宠物
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::showPets(SceneUser *pUser,const char *para)
{
	if (0==pUser) return false;
	int count = 0;
	if (pUser->pet)
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "宠物:%s id=%d pos=%d,%d", pUser->pet->name, pUser->pet->tempid, pUser->pet->getPos().x, pUser->pet->getPos().y);
		count++;
	}
	if (pUser->summon)
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "召唤兽:%s id=%d pos=%d,%d", pUser->summon->name, pUser->summon->tempid, pUser->summon->getPos().x, pUser->summon->getPos().y);
		count++;
	}
	for (std::list<ScenePet *>::iterator it=pUser->totems.begin(); it!=pUser->totems.end(); it++)
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "图腾：%s id=%d pos=%d,%d", (*it)->name, (*it)->tempid, (*it)->getPos().x, (*it)->getPos().y);
		count++;
	} 
	if (pUser->ridepet)
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "镖车:%s id=%d pos=%d,%d", pUser->ridepet->name, pUser->ridepet->tempid, pUser->ridepet->getPos().x, pUser->ridepet->getPos().y);
		count++;
	}
	if (pUser->guard)
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "镖车:%s id=%d pos=%d,%d", pUser->guard->name, pUser->guard->tempid, pUser->guard->getPos().x, pUser->guard->getPos().y);
		count++;
	}
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "共 %d 个", count);
	return true;
}

/**
 * \brief 显示地图
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::showMaps(SceneUser *pUser,const char *para)
{
	struct showMapsExec : public SceneCallBack
	{
		SceneUser * pUser;
		showMapsExec(SceneUser * p)
		{
			pUser = p;
		}
		~showMapsExec(){}

		bool exec(Scene *s)
		{
			return Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, s->name);
		}
	};
	if (0==pUser) return false;

	showMapsExec sme(pUser);
	SceneManager::getInstance().execEveryScene(sme);
	return true;
}
/**
 * \brief 加载地图，只有超级GM才可以使用
 * 如：//loadmap name=舜·青州
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::loadMap(SceneUser *pUser,const char *para)
{
	if (0==pUser) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["mapname"])
	{       
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，请指定地图名字");
		ScenesService::gm_logger->trace("GM %s 使用loadmap命令，参数错误 para=%s", pUser->name, para);
		return false;
	}
	//Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"该指令目前不可用");
	DWORD countryid = 0;
	DWORD mapid = 0;
	if ("" == prop["countryname"])
	{
		SceneManager::CountryMap_iter country_iter
			=SceneManager::getInstance().country_info.find(pUser->scene->getCountryID());
		if(country_iter != SceneManager::getInstance().country_info.end())
		{
			countryid = pUser->scene->getCountryID();
		}
	}
	else
	{
		countryid = SceneManager::getInstance().getCountryIDByCountryName(prop["countryname"].c_str());
	}
	if(countryid == 0)
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"输入的国家名称不正确");
		return false;
	}

	mapid = SceneManager::getInstance().getMapIDByMapName(prop["mapname"].c_str());
	if(mapid == 0)
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"输入的地图名称不正确");
		return false;
	}
	//判断是否重复加载
	Scene *loaded = SceneManager::getInstance().getSceneByID(SceneManager::getInstance().buildMapID(countryid , mapid));
	if(loaded)
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"不能重复加载地图");
		return false;
	}

	DWORD serverid=atoi(prop["serverid"].c_str());
	if(!serverid || serverid == ScenesService::getInstance().getServerID())
	{
		loaded = SceneManager::getInstance().loadScene(Scene::STATIC, countryid , mapid);
		if(loaded)
		{
			using namespace Cmd::Session;
			ScenesService::gm_logger->info("加载%s(%ld,%ld)成功",loaded->name,loaded->id,loaded->tempid);
			t_regScene_SceneSession regscene;
			regscene.dwCountryID = countryid;
			regscene.dwID=loaded->id;
			regscene.dwTempID=loaded->tempid;
			strncpy(regscene.byName,loaded->name,MAX_NAMESIZE);
			strncpy(regscene.fileName, loaded->getFileName(), MAX_NAMESIZE);
			sessionClient->sendCmd(&regscene,sizeof(regscene));
			ScenesService::gm_logger->trace("GM %s 使用loadmap命令，加载地图 %s·%s", 
					pUser->name, prop["countryname"].c_str() , prop["mapname"].c_str());
		}
	}
	else
	{
		using namespace Cmd::Session;
		t_reqAddScene_SceneSession ass;
		ass.dwServerID = serverid;
		ass.dwCountryID = countryid;
		ass.dwMapID = mapid;
		sessionClient->sendCmd(&ass,sizeof(ass));
		ScenesService::gm_logger->trace("GM %s 使用loadmap命令，加载地图 %s·%s", 
				pUser->name, prop["countryname"].c_str() , prop["mapname"].c_str());
	}

	// */
	return true;
}

/**
 * \brief 卸载地图，只有超级GM才可以使用
 * 如：//unloadmap name=舜·青州
 * 
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::unloadMap(SceneUser *pUser,const char *para)
{
	if (0==pUser) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["mapname"])
	{       
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，请指定地图名字");
		ScenesService::gm_logger->trace("GM %s 使用unloadmap命令，参数错误 para=%s", pUser->name, para);
		return false;
	}
	DWORD countryid = 0;
	DWORD mapid = 0;
	if ("" == prop["countryname"])
	{
		SceneManager::CountryMap_iter country_iter
			=SceneManager::getInstance().country_info.find(pUser->scene->getCountryID());
		if(country_iter != SceneManager::getInstance().country_info.end())
		{
			countryid = pUser->scene->getCountryID();
		}
	}
	else
	{
		countryid = SceneManager::getInstance().getCountryIDByCountryName(prop["countryname"].c_str());
	}
	if(countryid == 0)
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"输入的国家名称不正确");
		return false;
	}

	mapid = SceneManager::getInstance().getMapIDByMapName(prop["mapname"].c_str());
	if(mapid == 0)
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"输入的地图名称不正确");
		return false;
	}
	ScenesService::gm_logger->trace("GM %s 使用unloadmap命令，卸载地图 %s", pUser->name, prop["mapname"].c_str());
	Cmd::Session::t_unloadScene_SceneSession ret;
	//strncpy((char*)ret.map_name , name.c_str() , MAX_NAMESIZE);
	ret.map_id = SceneManager::getInstance().buildMapID(countryid , mapid);
	sessionClient->sendCmd(&ret, sizeof(ret));
	return false;
}


/**
 * \brief 设置玩家权限，只有超级GM才可以用
 * 如：//setpriv name=xwl priv=2
 * 
 * priv 目前有3个值： 0.任何指令都不能执行 1.普通玩家指令 2.普通GM指令 3.所有指令都能执行
 * 
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::setPriv(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{       
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，请指定玩家名字");
		ScenesService::gm_logger->trace("GM %s 使用setpriv命令，参数错误 para=%s", pUser->name, para);
		return false;
	}
	SceneUser *pTarget=SceneUserManager::getMe().getUserByName(prop["name"].c_str());
	if (pUser==pTarget && (pUser->getPriv()!=Gm::super_mode))
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"不能设置自己！");
		ScenesService::gm_logger->trace("GM %s 使用setpriv命令，试图设置自己的权限", pUser->name);
		return false;
	}

	int level = atoi(prop["priv"].c_str());
	if(pTarget && pUser)
	{       
		switch (level)
		{
			case 0:
				pTarget->setPriv(level);
				Channel::sendSys(pTarget, Cmd::INFO_TYPE_FAIL,"你被 %s 禁用所有命令",pUser->name);
				break;
			case 1:
				pTarget->setPriv(level);
				Channel::sendSys(pTarget, Cmd::INFO_TYPE_SYS,"你恢复到普通玩家状态");
				break;
			case 2:
				if (pTarget->id>1000)
				{
					Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"%s 不能成为GM",pTarget->name);
					ScenesService::gm_logger->trace("GM %s 使用setpriv命令，试图设置 %s 的权限，id太大， id=%d", pUser->name, prop["name"].c_str(), pTarget->id);
					return false;
				}
				pTarget->setPriv(level);
				Channel::sendSys(pTarget, Cmd::INFO_TYPE_SYS,"你成为GM了！");
				break;
				/*
			case 3:
				{
					pTarget->setPriv(Gm::super_mode);
					Channel::sendSys(pTarget, Cmd::INFO_TYPE_SYS,"你恢复到超级GM玩家状态");
				}
				break;
				*/
			default:
				{
					Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"权限参数错误");
					ScenesService::gm_logger->trace("GM %s 使用setpriv命令，参数错误 para=%s", pUser->name, para);
					return false;
				}
		}
		Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"设置成功");
		ScenesService::gm_logger->trace("GM %s 使用setpriv命令，设置 %s 的权限， priv=%d", pUser->name, prop["name"].c_str(), level);
		return true;
	}
	else    
	{
		Cmd::Session::t_gmCommand_SceneSession cmd;
		bzero(cmd.src_name,MAX_NAMESIZE);
		bzero(cmd.dst_name,MAX_NAMESIZE);
		bzero(cmd.map_file,MAX_NAMESIZE);
		bzero(cmd.map_name,MAX_NAMESIZE);
		cmd.gm_cmd = Cmd::Session::GM_COMMAND_SETPRIV;
		cmd.cmd_state = Cmd::Session::GM_COMMAND_STATE_REQ;
		cmd.src_priv = pUser->getPriv();
		cmd.x = level;
		strncpy((char *)&cmd.src_name, pUser->name, MAX_NAMESIZE);
		strncpy((char *)&cmd.dst_name, prop["name"].c_str(), MAX_NAMESIZE);
		return sessionClient->sendCmd(&cmd, sizeof(cmd));
	}
}

bool Gm::clearseptunion(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	Cmd::Scene::t_kickUser_GateScene send;
	send.userID = atoi(prop["loadmaze"].c_str());
	send.accid = atoi(prop["setquiz"].c_str());

	struct SceneTaskSendCmdExec :public execEntry<SceneTask>
	{       
		void *_data; 
		DWORD _size;
		SceneTaskSendCmdExec(void *data , DWORD size):_data(data),_size(size){} 
		bool exec(SceneTask *task)
		{       
			if(task)
			{       
				task->sendCmd(_data,_size);
			}       
			return true;
		}       
    };

	SceneTaskSendCmdExec exec(&send , sizeof(Cmd::Scene::t_kickUser_GateScene)); 
	SceneTaskManager::getInstance().execEverySceneTask(exec);
	return true;
	//soke end
	
	if ("" == prop["name"])
	{       
		//Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，请指定玩家名字");
	//	ScenesService::gm_logger->trace("GM %s 使用kict命令，参数错误 para=%s", pUser->name, para);
		return false;
	}
	SceneUser *pTarget=SceneUserManager::getMe().getUserByName(prop["name"].c_str());
	if(pTarget && pUser)
	{       

		//Channel::sendSys(pTarget, Cmd::INFO_TYPE_FAIL,"你已经被%s踢出服务器！",pUser->name);

		OnQuit event(1);
		EventTable::instance().execute(*pTarget, event);

		pTarget->save(Cmd::Record::LOGOUT_WRITEBACK);
		pTarget->killAllPets();

		//通知session服务器
		Cmd::Session::t_unregUser_SceneSession ret;
		ret.dwUserID=pTarget->id;
		ret.dwSceneTempID=pTarget->scene->tempid;
		ret.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
		sessionClient->sendCmd(&ret,sizeof(ret));

		//通知网关服务器
		Cmd::Scene::t_Unreg_LoginScene retgate;
		retgate.dwUserID = pTarget->id;
		retgate.dwSceneTempID = pTarget->scene->tempid;
		retgate.retcode = Cmd::Scene::UNREGUSER_RET_ERROR;
		pTarget->gatetask->sendCmd(&retgate,sizeof(retgate));
		pTarget->unreg();
		//ScenesService::gm_logger->trace("GM %s 使用kict命令，把 %s 踢出服务器", pUser->name, prop["name"].c_str());
		return true;
	}
	else    
	{
		Cmd::Session::t_gmCommand_SceneSession cmd;
		bzero(cmd.src_name,MAX_NAMESIZE);
		bzero(cmd.dst_name,MAX_NAMESIZE);
		bzero(cmd.map_file,MAX_NAMESIZE);
		bzero(cmd.map_name,MAX_NAMESIZE);
		cmd.gm_cmd = Cmd::Session::GM_COMMAND_KICK;
		cmd.cmd_state = Cmd::Session::GM_COMMAND_STATE_REQ;
		cmd.src_priv = pUser->getPriv();
		strncpy((char *)&cmd.src_name, pUser->name, MAX_NAMESIZE);
		strncpy((char *)&cmd.dst_name, prop["name"].c_str(), MAX_NAMESIZE);
		return sessionClient->sendCmd(&cmd, sizeof(cmd));
	}
}

/**
 * \brief 隐藏自己 
 * 隐身之后任何人都看不见该角色
 * 
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::hideMe(SceneUser *pUser,const char *para)
{
	if(pUser)
	{       
		pUser->setState(zSceneEntry::SceneEntry_Hide);
        //////////////////////////////////////////////////
		//主人物信息
        Cmd::stMainUserDataUserCmd  userinfo;
        pUser->full_t_MainUserData(userinfo.data);
        pUser->sendCmdToMe(&userinfo,sizeof(userinfo));

        //控制移动速度
        Cmd::stAddUserMapScreenUserCmd cmd;
        pUser->full_t_MapUserData(cmd.data);
        pUser->sendCmdToMe(&cmd,sizeof(cmd));
		///////////////////////////////////////////////////				     

		//通知9屏删除该角色
		Cmd::stRemoveUserMapScreenUserCmd remove;
		remove.dwUserTempID=pUser->tempid;
		pUser->scene->sendCmdToNine(pUser->getPosI(),&remove,sizeof(remove));

		//移除block
		pUser->scene->clearBlock(pUser->getPos());

		Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"隐身成功");
		ScenesService::gm_logger->trace("GM %s 使用hideme命令，隐身", pUser->name);
		return true;
	}
	return false;
}

/**
 * \brief 取消隐身 
 * 恢复可见状态
 * 
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::showMe(SceneUser *pUser,const char *para)
{
	if(pUser)
	{       
		pUser->setState(zSceneEntry::SceneEntry_Normal);

        ////////////////////////////////////////////
		//控制移动速度
        Cmd::stAddUserMapScreenUserCmd cmd;
        pUser->full_t_MapUserData(cmd.data);
        pUser->sendCmdToMe(&cmd,sizeof(cmd));
		////////////////////////////////////////////				     

		zPos curPos = pUser->getPos();
		pUser->goTo(curPos);
		Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"恢复到可见状态");
		ScenesService::gm_logger->trace("GM %s 使用showme命令，解除隐身", pUser->name);
		return true;
	}
	return false;
}

/**
 * \brief 无敌模式 
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::god(SceneUser *pUser,const char *para)
{
	if (pUser)
	{
		if(!pUser->angelMode)
		{
			pUser->angelMode = true;

			Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"进入无敌模式！");
			ScenesService::gm_logger->info("GM %s 使用god命令,进入无敌模式", pUser->name);
			return true;
		}
		else
		{
			pUser->angelMode = false;
			Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"退出无敌模式！");
			ScenesService::gm_logger->info("GM %s 使用god命令,退出无敌模式",pUser->name);
			return true;
		}
		
	}
	return false;
}

/**
 * \brief 必杀模式 
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::killer(SceneUser *pUser,const char *para)
{
	if (pUser)
	{   
		if(pUser->afreetMode)
		{
			pUser->afreetMode = false;
			Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"退出必杀模式！");
		}
		else
		{
			pUser->afreetMode = true;
			Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"进入必杀模式！");
			ScenesService::gm_logger->info("GM %s 使用killer命令,进入必杀模式",pUser->name);
		}

		return true;
	}
	return false;
}
bool Gm::newzone(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	if(pUser)
	{       
		if(strncmp("true",prop["service"].c_str(),MAX_NAMESIZE)==0)
		{
			if(!SceneManager::getInstance().isNewZoneConfig())
			{
				SceneManager::getInstance().setNewZoneConfig(true);
				SceneManager::getInstance().addNewZonePos(151,211); 
				SceneManager::getInstance().addNewZonePos(250,175); 
				SceneManager::getInstance().addNewZonePos(212,330); 
				SceneManager::getInstance().addNewZonePos(159,343); 
			}
			else
			{
				SceneManager::getInstance().setNewZoneConfig(true);
			}
			Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"设置新区配置成功");
			ScenesService::gm_logger->trace("GM %s 设置新区配置", pUser->name);
		}
		if(strncmp("false",prop["service"].c_str(),MAX_NAMESIZE)==0)
		{
			SceneManager::getInstance().setNewZoneConfig(false);
			Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"取消新区配置成功");
			ScenesService::gm_logger->trace("GM %s 取消新区配置", pUser->name);
		}
		DWORD x = atoi(prop["x"].c_str());
		DWORD y = atoi(prop["y"].c_str());
		if(x && y)
		{
			SceneManager::getInstance().addNewZonePos(x,y); 
			Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"增加新区配置重生坐标点:x=%d,y=%d",x,y);
			ScenesService::gm_logger->trace("GM %s 增加新区配置重生坐标点:x=%d,y=%d", pUser->name,x,y);
		}
		Cmd::Session::t_gmCommand_SceneSession send;
		send.gm_cmd=Cmd::Session::GM_COMMAND_NEWZONE;
		send.cmd_state=2;
		send.x=0;
		send.y=0;
		if(strncmp("true",prop["serviceall"].c_str(),MAX_NAMESIZE)==0)
		{
			send.cmd_state=1;
			Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"设置全区新区配置成功");
			ScenesService::gm_logger->trace("GM %s 设置新区配置", pUser->name);
		}
		if(strncmp("false",prop["serviceall"].c_str(),MAX_NAMESIZE)==0)
		{
			send.cmd_state=0;
			Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"取消全区新区配置成功");
			ScenesService::gm_logger->trace("GM %s 取消新区配置", pUser->name);
		}
		DWORD xall = atoi(prop["xall"].c_str());
		DWORD yall = atoi(prop["yall"].c_str());
		if(xall && yall)
		{
			send.x=xall;
			send.y=yall;
			Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"增加新区配置重生坐标点:x=%d,y=%d",xall,yall);
			ScenesService::gm_logger->trace("GM %s 增加新区配置重生坐标点:x=%d,y=%d", pUser->name,xall,yall);
		}
		if((send.cmd_state != 2) || (send.x != 0 && send.y != 0))
		{
			sessionClient->sendCmd(&send, sizeof(send));
		}
		if(strncmp("true",prop["query"].c_str(),MAX_NAMESIZE)==0)
		{
			SceneManager::NewZoneVec &vec = SceneManager::getInstance().queryNewZonePos(); 
			SceneManager::newZoneVec_iter iter = vec.begin();
			for(;iter != vec.end() ; iter ++)
			{
				Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"新区配置重生坐标点:x=%d,y=%d",(*iter).first,(*iter).second);
			}
		}
		return true;
	}
	return false;
}

/**
 * \brief 恢复普通模式 
 * 取消无敌和必杀模式
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::normal(SceneUser *pUser,const char *para)
{
	if(pUser)
	{       
		pUser->angelMode = false;
		pUser->afreetMode = false;
		Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS,"恢复普通模式");
		ScenesService::gm_logger->trace("GM %s 使用normal命令，恢复普通模式", pUser->name);

        //soke 移动速度恢复正常
        pUser->charstate.movespeed  = 640;
        //soke 主人物信息
        Cmd::stMainUserDataUserCmd  userinfo;
        pUser->full_t_MainUserData(userinfo.data);
        pUser->sendCmdToMe(&userinfo,sizeof(userinfo));

		return true;
	}
	return false;
}

/**
 * \brief 回到国家主城 
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::goHome(SceneUser *pUser,const char *para)
{
	if(pUser)
	{       
		zXMLParser parser = zXMLParser();
		char str[MAX_NAMESIZE];
		bzero(str,MAX_NAMESIZE);
		if(parser.initFile(Zebra::global["sceneinfofile"].c_str()))
		{
			xmlNodePtr root = parser.getRootNode("ScenesInfo");
			if (0==root) return false;

			xmlNodePtr infoNode = parser.getChildNode(root,"countryinfo");
			if (0==infoNode) return false;

			xmlNodePtr countryNode = parser.getChildNode(infoNode,"country");
			while (countryNode)
			{
				unsigned int id=0;
				parser.getNodePropNum(countryNode, "id", &id, sizeof(id));
				if (id==pUser->charbase.country)
				{
					std::string s = "name=";
					parser.getNodePropStr(countryNode, "name", str, sizeof(str));
					s += str;
					s += "·";

					unsigned int cap_id;//主城的mapID
					parser.getNodePropNum(countryNode, "mapID", &cap_id, sizeof(cap_id));

					infoNode = parser.getChildNode(root,"mapinfo");
					if (0==infoNode) return false;

					xmlNodePtr mapNode = parser.getChildNode(infoNode,"map");

					while (mapNode)
					{
						parser.getNodePropNum(mapNode, "mapID", &id, sizeof(id));
						if (id==cap_id)
						{
							parser.getNodePropStr(mapNode, "name", str, sizeof(str));
							s += str;
							s += " pos=";

							parser.getNodePropStr(mapNode, "fileName", str, sizeof(str));

							zXMLParser parser2 = zXMLParser();
							char mapFile[MAX_NAMESIZE];
							bzero(mapFile, sizeof(mapFile));
							sprintf(mapFile, "map/%s.xml",str);
							if(parser2.initFile(mapFile))
							{
								xmlNodePtr root2 = parser2.getRootNode("map");
								xmlNodePtr zoneNode = parser2.getChildNode(root2,"zonedef");
								int num;
								while (zoneNode)
								{
									parser2.getNodePropNum(zoneNode, "type", &num, sizeof(num));
									if (8==num)
									{
										parser2.getNodePropStr(zoneNode, "x", str, sizeof(str));
										s += str;
										s += ",";
										parser2.getNodePropStr(zoneNode, "y", str, sizeof(str));
										s += str;

										break;
									}
									zoneNode = parser2.getNextNode(zoneNode, "zonedef");
								}
							}
							else
								ScenesService::gm_logger->trace("GM %s 使用gohome指令,打开 %s 文件失败", pUser->name, mapFile);
							break;
						}
						mapNode = parser.getNextNode(mapNode, "map");
					}

					gomap(pUser, s.c_str());

					//Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "主城，country=%d, param=%s", pUser->charbase.country, s.c_str());
					ScenesService::gm_logger->trace("GM %s 使用gohome命令，返回主城 param=%s ", pUser->name, s.c_str());
					return true;
				}
				countryNode = parser.getNextNode(countryNode, "country");
			}
			Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "未找到主城，country=%d", pUser->charbase.country);
			ScenesService::gm_logger->trace("GM %s 使用gohome命令失败，没找到主城。countryID=%d", pUser->name, pUser->charbase.country);
		}
		return false;
	}
	return false;
}

/**
 * \brief 技能测试指令
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::skill(SceneUser *pUser,const char *para)
{
	if ((NULL==pUser)||(NULL==para))
		return false;
	if (""==para)
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数");
		ScenesService::gm_logger->trace("GM %s 使用skill命令，参数错误 para=%s", pUser->name, para);
		return false;
	}

	zProperties prop;
	prop.parseCmdLine(para);

	if ("pkvalue" == prop["show"])
	{
		pUser->skillStatusM.showValue();
	}
	else if ("active" == prop["show"])
	{
		pUser->skillStatusM.showActive();
	}
	else if ("recovery" == prop["show"])
	{
		pUser->skillStatusM.showRecovery();
	}
	else if ("passiveness" == prop["show"])
	{
		pUser->skillStatusM.showPassiveness();
	}
	else if ("log" == prop["show"])
	{
		pUser->skillStatusM.showValueToLog();
	}



	if (!("" == prop["defencer"] ||
				"" == prop["magictype"] ||
				"" == prop["attacktype"]))
	{
		Cmd::stAttackMagicUserCmd send;

		SceneUser *defencer = SceneUserManager::getMe().getUserByName(prop["defencer"].c_str());
		if (defencer)
		{
			send.dwUserTempID = pUser->tempid;
			send.dwDefenceTempID = defencer->tempid;
			send.wdMagicType = atoi(prop["magictype"].c_str());
			send.xDes = (WORD)pUser->getPos().x;
			send.yDes = (WORD)pUser->getPos().y;
			send.byDirect = pUser->getDir();
			send.byAttackType = atoi(prop["attacktype"].c_str());
			send.byAction = 0;
			Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "我攻击了%s",defencer->name);
			pUser->attackMagic(&send, sizeof(send));
			ScenesService::gm_logger->trace("GM %s 使用skill命令，攻击了 %s ", pUser->name, defencer->name);
			return true;
		}
		else
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"玩家 %s 不在线",prop["defencer"].c_str());
			ScenesService::gm_logger->trace("GM %s 使用skill命令，玩家 %s 不在线", pUser->name, prop["defencer"].c_str());
			return false;
		}
	}
	else    
	{
		ScenesService::gm_logger->trace("GM %s 使用skill命令，参数错误 para=%s", pUser->name, para);
		return false;
	}
}


/**
 * \brief 对玩家进行锁血等处理
 * 范例
 * //lockvalue name=member1 lock=mp
 * //lockvalue name=member1 unlock=all
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::lockValue(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	/*        if ("" == prop["name"])
		  {       
		  Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，请指定玩家名字");
		  ScenesService::gm_logger->trace("GM %s 使用lockvalue命令，参数错误 para=%s", pUser->name, para);
		  return false;
		  }
	 */
	Cmd::Session::t_gmCommand_SceneSession cmd;
	cmd.err_code = 0;//错误信息
	bzero(cmd.src_name,MAX_NAMESIZE);
	bzero(cmd.dst_name,MAX_NAMESIZE);
	bzero(cmd.map_file,MAX_NAMESIZE);
	bzero(cmd.map_name,MAX_NAMESIZE);
	cmd.gm_cmd = Cmd::Session::GM_COMMAND_LOCKVALUE;
	cmd.cmd_state = Cmd::Session::GM_COMMAND_STATE_REQ;
	cmd.src_priv = pUser->getPriv();

	strncpy((char *)&cmd.src_name, pUser->name, MAX_NAMESIZE);

	if ("" == prop["name"])
	{
		strncpy((char*)&cmd.dst_name, pUser->name, MAX_NAMESIZE);
	}
	else
	{
		strncpy((char *)&cmd.dst_name, prop["name"].c_str(), MAX_NAMESIZE);
	}

	if ("mp" == prop["lock"])
	{
		cmd.x = 2;
	}
	else if ("hp" == prop["lock"])
	{
		cmd.x = 1;
	}
	else if ("sp" == prop["lock"])
	{
		cmd.x = 3;
	}
	else if ("all" == prop["lock"])
	{
		cmd.x = 4;
	}
	else
	{
		cmd.x = 5;
	}

	if ("mp" == prop["unlock"])
	{
		cmd.y = 2;
	}
	else if ("hp" == prop["unlock"])
	{
		cmd.y = 1;
	}
	else if ("sp" == prop["unlock"])
	{
		cmd.y = 3;
	}
	else if ("all" == prop["unlock"])
	{
		cmd.y = 4;
	}
	else
	{
		cmd.y = 5;
	}
	return sessionClient->sendCmd(&cmd, sizeof(cmd));

}


/**
 * \brief 模拟家族投票
 * 范例
 * //svote name=暗黑族
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::svote(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{       
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，请指定家族名字");
		return false;
	}

	Cmd::stVoteListSeptCmd send1;
	pUser->forwardSession(&send1, sizeof(Cmd::stVoteListSeptCmd));

	Cmd::stVoteSeptCmd send;
	strncpy(send.septName,prop["name"].c_str(),MAX_NAMESIZE);

	pUser->forwardSession(&send, sizeof(Cmd::stVoteSeptCmd));
	return true;
}

/**
 * \brief 模拟帮会投票
 * 范例
 * //uvote name=暗黑帮
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::uvote(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，请指定帮会名字");
		return false;
	}

	Cmd::stVoteListUnionCmd send1;
	pUser->forwardSession(&send1, sizeof(Cmd::stVoteListUnionCmd));

	Cmd::stVoteUnionCmd send;
	strncpy(send.unionName,prop["name"].c_str(),MAX_NAMESIZE);

	pUser->forwardSession(&send, sizeof(Cmd::stVoteUnionCmd));
	return true;
}

/**
 * \brief 强制激活帮会战
 * 范例
 * //tong t1=暗黑帮 t2=魔兽帮...
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::tong(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["t1"])
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，请指定帮会名字");
		return false;
	}

	if ("" == prop["t2"])
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，请指定帮会名字");
		return false;
	}


	Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "帮会战状态已激活。");
	return true;

}

/**
 * \brief 显示特定npc的具体位置信息
 *
 */
class ShowSpecialNpcCallBack : public specialNpcCallBack
{
	private:
		SceneUser * pUser;

	public:
		int count;
		ShowSpecialNpcCallBack(SceneUser * user):pUser(user),count(0){}
		bool exec(SceneNpc * npc)
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "特殊NPC: %s 位置:%s (%u,%u)", npc->name, npc->scene->name, npc->getPos().x, npc->getPos().y);
			count++;
			return true;
		}
};

/**
 * \brief 显示特定npc的位置信息
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::showSpecialNpc(SceneUser *pUser,const char *para)
{
	ShowSpecialNpcCallBack ssncb(pUser);
	SceneNpcManager::getMe().execAllSpecialNpc(ssncb);
	Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "共 %u 个", ssncb.count);
	return true;
}

/**
 * \brief 技能宠物
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::killPets(SceneUser *pUser,const char *para)
{
	pUser->killAllPets();
	return true;
}
/**
 * \brief 攻城
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::createRush(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["id"])
	{       
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，参照//createrush id=1 delay=60");
		ScenesService::gm_logger->trace("GM %s 使用createrush命令，参数错误 para=%s", pUser->name, para);
		return false;
	}

	int id = atoi(prop["id"].c_str());
	int delay = atoi(prop["delay"].c_str());
	zXMLParser parser = zXMLParser();
	unsigned int countryID=0;
	if (""!=prop["country"])
	{
		char str[MAX_NAMESIZE];
		bzero(str,MAX_NAMESIZE);
		if(parser.initFile(Zebra::global["sceneinfofile"].c_str()))
		{
			xmlNodePtr root = parser.getRootNode("ScenesInfo");
			if (0==root) return false;

			xmlNodePtr infoNode = parser.getChildNode(root,"countryinfo");
			if (0==infoNode) return false;

			xmlNodePtr countryNode = parser.getChildNode(infoNode,"country");
			while (countryNode)
			{
				parser.getNodePropStr(countryNode, "name", str, sizeof(str));
				if (!strcmp(str, prop["country"].c_str()))
				{
					parser.getNodePropNum(countryNode, "id", &countryID, sizeof(countryID));
					break;
				}
				countryNode = parser.getNextNode(countryNode, "country");
			}
			if (!id)
			{
				ScenesService::gm_logger->trace("GM %s 使用createrush指令，未找到国家 country=%s", pUser->name, prop["country"].c_str());
				return false;
			}
		}
		else
		{
			ScenesService::gm_logger->trace("GM %s 使用createrush指令，找不到文件 %s", pUser->name, Zebra::global["sceneinfofile"].c_str());
			return false;
		}
	}
	else
	{
		countryID = pUser->scene->getCountryID();
	}

	Rush * rush = new Rush(id, delay, countryID);
	if (rush)
	{
		if (rush->init())
		{
			ScenesService::gm_logger->trace("GM %s 使用createrush命令, 在 %s 引发怪物攻城", pUser->name, rush->mapName);
			return true;
		}

		SAFE_DELETE(rush);
		ScenesService::gm_logger->trace("GM %s 使用createrush命令, 初始化攻城数据失败 para=%s", pUser->name, para);
		return false;
	}

	ScenesService::gm_logger->trace("GM %s 使用createrush命令, 引发攻城失败 para=%s", pUser->name, para);
	return false;
}

/**
 * \brief 攻城
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::createQuiz(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	Cmd::Session::t_createQuiz_SceneSession send;

	send.active_time  = atoi(prop["subjects"].c_str());
	send.ready_time   = atoi(prop["ready_time"].c_str());
	send.subject_type	  = atoi(prop["type"].c_str());
	send.type = 0;
	sessionClient->sendCmd(&send, sizeof(send));

	ScenesService::gm_logger->trace("GM %s 使用createquiz命令, 引发全区智力竞赛 para=%s", pUser->name, para);
	return true;
}

/**
 * \brief 清除一个地图上某个区域的npc
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::clearWorld(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	int region = 0;
	if ("" == prop["region"])
		region = 5;
	else
		region = atoi(prop["region"].c_str());

	int side = 0;
	int direct = 0;
	int clockwise = 1;
	int clearCount = 0;
	int count = 0;
	zPos pos;
	pUser->scene->getNextPos(side, direct, pUser->getPos(), clockwise, pos);
	do
	{
		SceneNpc * sceneNpc = pUser->scene->getSceneNpcByPos(pos);
		if (sceneNpc)
		{
			switch(sceneNpc->npc->kind)
			{
				case NPC_TYPE_NORMAL:
				case NPC_TYPE_BBOSS:
				case NPC_TYPE_LBOSS:
				case NPC_TYPE_PBOSS:
				case NPC_TYPE_REDBOSS:
				case NPC_TYPE_BACKBONE:
				case NPC_TYPE_GOLD:
				case NPC_TYPE_SUMMONS:
				case NPC_TYPE_AGGRANDIZEMENT:
				case NPC_TYPE_ABERRANCE:
				case NPC_TYPE_BACKBONEBUG:
				case NPC_TYPE_GUARD:
				case NPC_TYPE_PET:
				case NPC_TYPE_TOTEM:
				case NPC_TYPE_HUMAN:
				case NPC_TYPE_WILDHORSE:
				case NPC_TYPE_DUCKHIT:
				case NPC_TYPE_BANNER:
				case NPC_TYPE_TRAP:
				case NPC_TYPE_UNIONGUARD:
				case NPC_TYPE_SOLDIER:
				case NPC_TYPE_TASK:
				case NPC_TYPE_TRADE:
					{
						if (!(sceneNpc->getMaster()==pUser))
						{
							sceneNpc->toDie(pUser->tempid);
							clearCount++;
						}
					}
					break;
				default:
					break;
			}
		}
		if (++count>=(region*2+1)*(region)*2+1)
			break;
	} while(pUser->scene->getNextPos(side, direct, pUser->getPos(), clockwise, pos) && side <= region);
	ScenesService::gm_logger->trace("GM %s 使用 clearworld 命令，杀死了半径 %d 范围内的 %d 只NPC", pUser->name, region, clearCount);
	return true;
}

/**
 * \brief 清除一个地图上某个区域的npc
 * 
 * \param pUser: 用户
 * \param para:内容
 * \return 操作是否成功
 */
bool Gm::clearArea(SceneUser *pUser,const char *para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	int region = 0;
	if ("" == prop["region"])
		region = 5;
	else
		region = atoi(prop["region"].c_str());

	int side = 0;
	int direct = 0;
	int clockwise = 1;
	int clearCount = 0;
	int count = 0;
	zPos pos;
	pUser->scene->getNextPos(side, direct, pUser->getPos(), clockwise, pos);
	do
	{
		SceneUser * sceneUser = pUser->scene->getSceneUserByPos(pos);
		if (sceneUser)
		{
			if (sceneUser!=pUser)
			{
				sceneUser->goToRandomRect(sceneUser->getPos(), SCREEN_WIDTH*2+region, SCREEN_HEIGHT*2+region);
				sceneUser->catchMyPet();
				clearCount++;
			}
		}
		if (++count>=(region*2+1)*(region)*2+1)
			break;
	} while(pUser->scene->getNextPos(side, direct, pUser->getPos(), clockwise, pos) && side <= region);
	ScenesService::gm_logger->trace("GM %s 使用 cleararea 命令，驱散了%s(%u,%u)周围的玩家", pUser->name, pUser->scene->name, pUser->getPos().x, pUser->getPos().y);
	return true;
}

/**
 * \brief 清除帮会与家族数据
 *
 *
 * \param pUser 指令使用者
 * \param para 参数
 * \return 指令执行是否成功
 */
bool Gm::clearSeptUnion(SceneUser *pUser,const char *para)
{
/*	Cmd::Session::t_ClearRelation_SceneSession send;
	send.dwUserID = pUser->id;

	sessionClient->sendCmd(&send, sizeof(send));
	*/

	Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"该命令已禁用");
	return false;
}

/**
 * \brief 创建一个家族
 *
 *
 * \param pUser 指令使用者
 * \param para 参数
 * \return 指令执行是否成功
 */
bool Gm::createFamily(SceneUser *pUser,const char *para)
{
	if ((!pUser)||(!para)) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，参照//createfamily name=我的家族");
		ScenesService::gm_logger->trace("GM %s 使用createfamily命令，参数错误 para=%s", pUser->name, para);
		return false;
	}

	Cmd::Session::t_addSept_SceneSession send;

	send.dwMapTempID = pUser->scene->tempid;
	send.info.dwUnionID = 0;
	send.info.dwRepute = 0;
	send.info.dwSeptID = 0;			 			// 家族的ID
	strncpy(send.info.name,prop["name"].c_str(),MAX_NAMESIZE-1); 	// 家族名称
	send.info.dwCharID = pUser->charbase.id;			// 族长的角色ID
	send.info.dwCountryID = pUser->charbase.country;		// 家族所属国家
	send.info.byVote = 0;						// 默认进入投票期间
	strncpy(send.info.masterName,pUser->name,MAX_NAMESIZE-1);	// 会长的名字
	send.info.dwSpendGold = 0;		// 家族花费金币
	send.info.dwLevel = 1;		// 家族等级
	bzero(send.info.note, sizeof(send.info.note));
	sessionClient->sendCmd(&send, sizeof(send));
	return true;
}

/**
 * \brief 创建一个帮会
 *
 *
 * \param pUser 指令使用者
 * \param para 参数
 * \return 指令执行是否成功
 */
bool Gm::createUnion(SceneUser *pUser,const char *para)
{

	Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"该命令已禁用");
	/*
	if ((!pUser)||(!para)) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，参照//createunion name=我的帮会");
		ScenesService::gm_logger->trace("GM %s 使用createunion命令，参数错误 para=%s", pUser->name, para);
		return false;
	}

	Cmd::Session::t_addUnion_SceneSession send;

	send.dwMapTempID = pUser->scene->tempid;
	send.dwItemID 	 = 0;
	send.info.dwUnionID = 0;					// 帮会的ID
	strncpy(send.info.name,prop["name"].c_str(),MAX_NAMESIZE-1);		// 帮会名称
	send.info.dwCharID = pUser->charbase.id;	                    	// 会长的角色ID
	send.info.dwCountryID = pUser->charbase.country;
	strncpy(send.info.masterName,pUser->name,MAX_NAMESIZE-1);	// 会长的名字
	send.info.wdLevel = 1;     		    	       	        // 帮会级别
	send.info.qwExp = 0;                              	  	// 帮会经验
	send.info.byVote = 0; //帮会投票标志
	sessionClient->sendCmd(&send, sizeof(send));
	*/
	return false;
}

/**
 * \brief 创建一个门派
 *
 *
 * \param pUser 指令使用者
 * \param para 参数
 * \return 指令执行是否成功
 */
bool Gm::createSchool(SceneUser *pUser,const char *para)
{
	if ((!pUser)||(!para)) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	if ("" == prop["name"])
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少参数，参照//createunion name=我的帮会");
		ScenesService::gm_logger->trace("GM %s 使用createunion命令，参数错误 para=%s", pUser->name, para);
		return false;
	}

	Cmd::Session::t_createSchool_SceneSession cmd;
	strncpy((char *)cmd.masterName, pUser->name, MAX_NAMESIZE-1);
	strncpy((char *)cmd.schoolName, prop["name"].c_str(), MAX_NAMESIZE-1);//要创建的门派名字

	sessionClient->sendCmd(&cmd, sizeof(cmd));
	return true;
}

/**
 * \brief 查看国家对战信息
 *
 *
 * \param pUser 指令使用者
 * \param para 参数
 * \return 指令执行是否成功
 */
bool Gm::viewCountryDare(SceneUser *pUser,const char *para)
{
	if ((!pUser)||(!para)) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	Cmd::Session::t_viewCountryDare_SceneSession send;
	if ("" == prop["type"])
		send.byType = 3;
	else
		send.byType = atoi(prop["type"].c_str());

	send.dwUserID = pUser->id;

	sessionClient->sendCmd(&send, sizeof(send));
	return true;
}

/**
 * \brief 查看周围的npc和玩家
 *
 * \param pUser 指令使用者
 * \param para 参数
 * radius 查找半径
 * type 查找的类型 1:玩家 2:npc 0:全部
 * \return 是否执行成功
 */
bool Gm::showEntries(SceneUser *pUser,const char *para)
{
	if ((!pUser)||(!para)) return false;
	zProperties prop;
	prop.parseCmdLine(para);

	int radius = atoi(prop["radius"].c_str());
	if (0==radius) radius=2;
	int type = atoi(prop["type"].c_str());

	int side = 0;
	int direct = 0;
	int clockwise = 1;
	int count = 0;
	int playerCount = 0,npcCount = 0;
	zPos curPos(0,0);
	pUser->scene->getNextPos(side, direct, pUser->getPos(), clockwise, curPos);
	do
	{
		SceneEntryPk *entry;
		char str[MAX_CHATINFO];
		std::string state;
		if ((0==type)||(1==type))
		{
			entry = pUser->scene->getSceneUserByPos(curPos);
			if (entry)
			{
				switch (entry->getState())
				{
					case zSceneEntry::SceneEntry_Normal:
						state = "普通";
						break;
					case zSceneEntry::SceneEntry_Death:
						state = "死亡";
						break;
					case zSceneEntry::SceneEntry_Hide:
						state = "隐身";
						break;
					default:
						state = "未知";
						break;
				}
				playerCount++;
				snprintf(str, MAX_CHATINFO-1,"(%u,%u) 玩家 %s %s", curPos.x, curPos.y, entry->name, state.c_str());
				Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, str);
			}
		}

		if ((0==type)||(2==type))
		{
			entry = pUser->scene->getSceneNpcByPos(curPos);
			if (entry)
			{
				switch (entry->getState())
				{
					case zSceneEntry::SceneEntry_Normal:
						state = "普通";
						break;
					case zSceneEntry::SceneEntry_Death:
						state = "死亡";
						break;
					case zSceneEntry::SceneEntry_Hide:
						state = "隐身";
						break;
					default:
						state = "未知";
						break;
				}
				npcCount++;
				snprintf(str, MAX_CHATINFO-1,"(%u,%u) npc %s %s", curPos.x, curPos.y, entry->name, state.c_str());
				Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, str);
			}
		}
		if (++count>=(radius*2+1)*(radius*2+1)) break;
	} while(pUser->scene->getNextPos(side, direct, pUser->getPos(), clockwise, curPos) && side <= radius);

	if (playerCount||npcCount)
		Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "%d 个玩家 %d 个npc", playerCount, npcCount);

	ScenesService::gm_logger->trace("GM %s 使用showentries命令(%u,%u) para=\"%s\"", pUser->name, pUser->getPos().x, pUser->getPos().y, para);

	return true;
	//Channel::sendNine(this, "?? %d ???", entries.size());
	//        return !entries.empty();
}

/*\brief 查询角色大小
 * 
 * 查询角色在内存中所占的空间大小
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 升级是否成功
 */
bool Gm::getsize(SceneUser *pUser,const char *para)
{
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"%s角色所占空间为%d",pUser->name, sizeof(*pUser));
	return true;
}

/**
 * \brief 设置宠物行为模式
 *
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 是否设置成功
 */
bool Gm::setPetAI(SceneUser *pUser,const char *para)
{
	if ((!pUser)||(!para)) return false;
	zProperties prop;
	prop.parseCmdLine(para);

	int t;
	Cmd::petAIMode m=Cmd::PETAI_MOVE_STAND;
	t = atoi(prop["type"].c_str());
	if ("follow"==prop["mode"]) m = Cmd::PETAI_MOVE_FOLLOW;
	if ("stand"==prop["mode"]) m = Cmd::PETAI_MOVE_STAND;
	if ("active"==prop["mode"]) m = Cmd::PETAI_ATK_ACTIVE;
	if ("passive"==prop["mode"]) m = Cmd::PETAI_ATK_PASSIVE;
	if ("noatk"==prop["mode"]) m = Cmd::PETAI_ATK_NOATK;

	if (2==t) if (pUser->pet) pUser->pet->setPetAI(m);
	if (3==t) if (pUser->summon) pUser->summon->setPetAI(m);
	if (4==t)
	{
		for (std::list<ScenePet *>::iterator it=pUser->totems.begin(); it!=pUser->totems.end(); it++)
			(*it)->setPetAI(m);
	}
	ScenesService::gm_logger->trace("GM %s 使用setpetai指令设置宠物行动方式 type=%d ai=%s",pUser->name, t, prop["mode"].c_str());
	return true;
}

/**
 * \brief 设置宠物AIF
 *
 *
 * \param pUser: 用户
 * \param para:内容
 * \return 是否设置成功
 */
bool Gm::setPetAIF(SceneUser *pUser,const char *para)
{
	if ((!pUser)||(!para)) return false;
	zProperties prop;
	prop.parseCmdLine(para);

	int t = atoi(prop["aif"].c_str());

	if (pUser->pet) pUser->pet->aif = t;
	if (pUser->summon) pUser->summon->aif = t;
	return true;
}

bool Gm::bank(SceneUser* pUser, const char* para)
{
	if (!pUser || !para) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	BYTE page = atoi(prop["page"].c_str());
	BYTE day = atoi(prop["day"].c_str());

	if (page >= pUser->packs.store.days.size()) {
		pUser->packs.store.days.push_back(day);
	}else {
		pUser->packs.store.days[page] = day;
	}

	pUser->packs.store.notify(pUser);
	return true;
}

/**
 * \brief 增加经验值
 * 最多加10w
 *
 * \param pUser 使用者
 * \param para 参数
 * \return 是否执行成功
 */
bool Gm::addExp(SceneUser* pUser, const char* para)
{
	if ((!pUser)||(!para)) return false;
	zProperties prop;
	prop.parseCmdLine(para);

	SceneUser * target = SceneUserManager::getMe().getUserByName(prop["name"].c_str());
	if (!target) target = pUser;
	int num = atoi(prop["num"].c_str());
	if (num<=0) return false;
	if (num>2000000000) num = 2000000000;
	target->addExp(num, true, 0, 0, true);
	return true;
}

/**
 * \brief 查询用户帐号
 * 
 *
 * \param pUser 使用者
 * \param para 参数
 * \return 是否执行成功
 */
bool Gm::qAccount(SceneUser* pUser, const char* para)
{
	if ((!pUser)||(!para)) return false;
	zProperties prop;
	prop.parseCmdLine(para);

	SceneUser * target = SceneUserManager::getMe().getUserByName(prop["name"].c_str());
	if (!target) target = pUser;

	Cmd::Scene::t_Query_AccountScene send;
	send.dwUserID = pUser->id;
	send.dwDestID = target->id;

	pUser->gatetask->sendCmd(&send, sizeof(send));

	return true;
}

/**
 * \brief 调整线程sleep的时间
 * 
 *
 * \param pUser 超级GM
 * \param para 参数
 * \return 是否执行成功
 */
bool Gm::usleep(SceneUser* pUser, const char* para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	int uscene=atoi(prop["scene"].c_str());
	int ugate=atoi(prop["gate"].c_str());
	int uclient=atoi(prop["client"].c_str());
	if(uscene)
	{
		zTCPTaskPool::setUsleepTime(uscene);
		Zebra::logger->info("调整场景服务器usleep时间:%dus",uscene);
	}
	Cmd::Scene::t_Usleep_GateScene ret;
	if(ugate)
	{
		ret.utask=ugate;
	}
	if(uclient)
	{
		ret.uclient=uclient;
	}
	if(ugate || uclient)
	{
		if(strncmp("true",prop["all"].c_str(),MAX_NAMESIZE)==0)
		{
			SceneTaskManager::getInstance().broadcastCmd(&ret, sizeof(ret));
			if(ugate)
				Zebra::logger->info("调整所有网关服务器usleep时间:%dus",ugate);
			if(uclient)
				Zebra::logger->info("调整所有网关客户端usleep时间:%dus",uclient);
		}
		else
		{
			pUser->gatetask->sendCmd(&ret, sizeof(ret));
			if(ugate)
				Zebra::logger->info("调整所有网关服务器usleep时间:%dus",ugate);
			if(uclient)
				Zebra::logger->info("调整所有网关客户端usleep时间:%dus",uclient);
		}
	}
	return true;
}
/**
 * \brief 得到系统当前时间
 * 
 *
 * \param pUser 超级GM
 * \param para 参数
 * \return 是否执行成功
 */
bool Gm::systime(SceneUser* pUser, const char* para)
{
	struct tm tm_1;
	time_t timValue = time(NULL);
	zRTime::getLocalTime(tm_1, timValue);
	//tm_1=*localtime(&timValue);
	Channel::sendSys(pUser, Cmd::INFO_TYPE_EXP2, "系统当前时间%d年%d月%d日%d时%d分%d秒",tm_1.tm_year+1900,tm_1.tm_mon+1,tm_1.tm_mday,tm_1.tm_hour,tm_1.tm_min,tm_1.tm_sec);
	return true;
}
/**
 * \brief 停机维护
 * 
 *
 * \param pUser 超级GM
 * \param para 参数
 * \return 是否执行成功
 */
bool Gm::shutdown(SceneUser* pUser, const char* para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	//zRegex regex;
	Cmd::Session::t_shutdown_SceneSession send;
	struct tm tm_1;
	time_t timValue = time(NULL);
	tm_1=*localtime(&timValue);
	//zRTime::getLocalTime(tm_1, timValue);
	//Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "系统当前时间%d年%d月%d日%d时%d分%d秒",tm_1.tm_year+1900,tm_1.tm_mon+1,tm_1.tm_mday,tm_1.tm_hour,tm_1.tm_min,tm_1.tm_sec);
	tm_1.tm_year=atoi(prop["year"].c_str())!=0?atoi(prop["year"].c_str()):tm_1.tm_year;
	tm_1.tm_mon=atoi(prop["month"].c_str())!=0?atoi(prop["month"].c_str()):tm_1.tm_mon;
	tm_1.tm_mday=atoi(prop["day"].c_str())!=0?atoi(prop["day"].c_str()):tm_1.tm_mday;
	tm_1.tm_hour=atoi(prop["hour"].c_str())!=0?atoi(prop["hour"].c_str()):tm_1.tm_hour;
	tm_1.tm_min=atoi(prop["minute"].c_str());
	tm_1.tm_sec=atoi(prop["second"].c_str());
	send.time=mktime(&tm_1);
	if(send.time<=timValue)
	{
		send.time=0;
		 Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "取消停机维护");
	}
	strncpy((char *)send.info, prop["info"].c_str(),MAX_CHATINFO-1);
	sessionClient->sendCmd(&send, sizeof(send));
	//Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "系统将于%d年%d月%d日%d时%d分%d秒停机维护",tm_1.tm_year+1900,tm_1.tm_mon+1,tm_1.tm_mday,tm_1.tm_hour,tm_1.tm_min,tm_1.tm_sec);
	return true;
}

/**
 * \brief 得到本场景服务器的人数分布状况
 * 
 *
 * \param pUser 使用者
 * \param para 参数
 * \return 是否执行成功
 */
bool Gm::countServerUser(SceneUser* pUser, const char* para)
{
	struct SceneUserCountExec : public SceneCallBack
	{
		SceneUser *pUser;
		SceneUserCountExec(SceneUser *u):pUser(u) 
		{
		}
		bool exec(Scene *scene)
		{
			if (scene)
			{
				DWORD count = scene->countUser();
				Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "场景%s当前人数:%d", scene->name, count);
			}
			return true;
		}
	};
	SceneUserCountExec esea(pUser); 
	SceneManager::getInstance().execEveryScene(esea);
	struct SceneTaskCountExec :public execEntry<SceneTask>
	{
		SceneUser *pUser;
		SceneTaskCountExec(SceneUser *u):pUser(u)
		{
		}
		bool exec(SceneTask *task)
		{
			if(task)
			{
				DWORD count = SceneUserManager::getMe().countUserByTask(task);
				Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "网关%u当前人数:%d", task->getID(), count);
			}
			return true;
		}
	};
	SceneTaskCountExec stce(pUser); 
	SceneTaskManager::getInstance().execEverySceneTask(stce);

	return true;
}

#include "QuestAction.h"
bool Gm::setQuest(SceneUser* pUser, const char* para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	zRegex regex;

	std::string var_name  = prop["var"];
	std::string value = prop["value"];
	int id = atoi(prop["id"].c_str());
	int type = atoi(prop["type"].c_str());

	std::string info = "Error Type!";
	Vars* vars = NULL;
	if (1)
	{
		switch (type) 
		{
			case 0:
				info = pUser->quest_list.info();
				vars = pUser->quest_list.vars(id);
				break;
			case 1:
				info = GlobalVar::instance().show();
				vars = GlobalVar::instance().add(id);
				break;
			case 2:
				info = TongVar::instance().show();
				vars = TongVar::instance().add(id);
				break;
			case 3:
				info = FamilyVar::instance().show();
				vars = FamilyVar::instance().add(id);
				break;
			case 4:
				info = UsersVar::instance().show();
				vars = UsersVar::instance().add(id);
				break;
		}		
		if (var_name.empty() || value.empty()) {
			Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, info.c_str());
			return false;
		}
	}


	bool new_vars = false;
	if (!vars) {
		vars = new Vars;
		new_vars = true;
	}

	Op::Set<int> op;
	vars->set_value(op, var_name,  atoi(prop["value"].c_str()), 0, pUser);
	if (type == 0) {
		vars->notify(*pUser, var_name);
		pUser->quest_list.add_quest(id, *vars, *pUser, false);
		pUser->quest_list.update(*pUser, true);
	}

	if (new_vars) SAFE_DELETE(vars);

	return true;
}

bool Gm::setBlock(SceneUser* pUser, const char* para)
{
	int radius = 2;

	int side = 0;
	int direct = 0;
	int clockwise = 1;
	int count = 0;
	zPos curPos(0,0);
	pUser->scene->getNextPos(side, direct, pUser->getPos(), clockwise, curPos);
	do
	{
		pUser->scene->setBlock(curPos);
		if (++count>=(radius*2+1)*(radius*2+1)) break;
	} while(pUser->scene->getNextPos(side, direct, pUser->getPos(), clockwise, curPos) && side <= radius);
	ScenesService::gm_logger->debug("GM %s 使用clearblock命令，设置阻挡", pUser->name);

	return true;
}

bool Gm::checkBlock(SceneUser* pUser, const char* para)
{
	int radius = 2;

	int side = 0;
	int direct = 0;
	int clockwise = 1;
	int count = 0;
	int blockCount = 0;
	zPos curPos(0,0);
	pUser->scene->getNextPos(side, direct, pUser->getPos(), clockwise, curPos);
	do
	{
		if (pUser->scene->checkBlock(curPos))
			blockCount++;
		if (++count>=(radius*2+1)*(radius*2+1)) break;
	} while(pUser->scene->getNextPos(side, direct, pUser->getPos(), clockwise, curPos) && side <= radius);
	Channel::sendSys(pUser,Cmd::INFO_TYPE_FAIL, "周围2格内有 %d 个阻挡点", blockCount);

	return true;
}

bool Gm::npcDare(SceneUser *pUser, const char * para)
{
	Cmd::Session::t_NpcDare_GMCmd_SceneSession send;
	sessionClient->sendCmd(&send, sizeof(send));
	ScenesService::gm_logger->debug("GM %s 使用npcdare命令，主动触发NPC争夺战", pUser->name);
	return true;
}

bool Gm::clearPoint(SceneUser *pUser, const char * para)
{
	if (pUser)
	{
				pUser->charbase.points =pUser->charbase.points
						+pUser->charbase.wdProperty[0]
						+pUser->charbase.wdProperty[1]
						+pUser->charbase.wdProperty[2]
						+pUser->charbase.wdProperty[3]
						+pUser->charbase.wdProperty[4];
				pUser->charbase.wdProperty[0]=0;
				pUser->charbase.wdProperty[1]=0;
				pUser->charbase.wdProperty[2]=0;
				pUser->charbase.wdProperty[3]=0;
				pUser->charbase.wdProperty[4]=0;
				pUser->setupCharBase();
				//刷新用户数据
				Cmd::stMainUserDataUserCmd  userinfo;
				pUser->full_t_MainUserData(userinfo.data);
				pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
				ScenesService::gm_logger->debug("GM %s 使用clearpoint清自己的人物属性点", pUser->name);
	}
	return true;
}

bool Gm::clearSkillPoint(SceneUser *pUser, const char * para)
{
	if (pUser)
	{
				pUser->charbase.skillpoint = pUser->charbase.level;
				pUser->usm.clear();
				Cmd::stClearSkillUserCmd send;
				pUser->sendCmdToMe(&send,sizeof(send));
				pUser->skillValue.init();
				pUser->setupCharBase();
				//刷新用户数据
				Cmd::stMainUserDataUserCmd  userinfo;
				pUser->full_t_MainUserData(userinfo.data);
				pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
				ScenesService::gm_logger->debug("GM %s 使用clearskillpoint清自己的技能点", pUser->name);
	}
	return true;
}

bool Gm::clearSkill(SceneUser *pUser, const char * para)
{
	if (pUser)
	{
		zProperties prop;
		prop.parseCmdLine(para);
		if (prop["id"]!="")
		{
			DWORD skillid = atoi(prop["id"].c_str());

			pUser->usm.clearskill(skillid);
			Cmd::stClearSkillUserCmd send;
			pUser->sendCmdToMe(&send,sizeof(send));
			pUser->usm.refresh();
			pUser->skillValue.init();
			pUser->setupCharBase();
				//刷新用户数据
			Cmd::stMainUserDataUserCmd  userinfo;
			pUser->full_t_MainUserData(userinfo.data);
			pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
			ScenesService::gm_logger->debug("GM %s 使用clearskill清自己的指定技能", pUser->name);
		}
	}
	return true;
}


bool Gm::studySkill(SceneUser *pUser, const char * para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	if (pUser)
	{
		std::string skill_name  = prop["name"];
		if (!skill_name.empty())
		{
			zSkillB * skillb = skillbm.get(skill_name.c_str());
			if (skillb)
			{
                pUser->upgradeSkill(skillb->skillid);
			}
		}
	}
	return true;
}

bool Gm::queryincmap(SceneUser *pUser, const char * para)
{
	zObject *obj = pUser->packs.equip.getObjectByEquipPos(Cmd::EQUIPCELLTYPE_ADORN);
	if(obj && obj->base->kind == ItemType_Amulet)
	{
		char buf[1024];
		bzero(buf,sizeof(buf));
		Cmd::stScrollMapPropertyUserCmd *smp = (Cmd::stScrollMapPropertyUserCmd*)buf;
		smp->qwThisID = obj->data.qwThisID;
		constructInPlace(smp);
		smp->size = pUser->scene->getIncCity((const char*)smp->mapname);
		pUser->sendCmdToMe(smp, sizeof(Cmd::stScrollMapPropertyUserCmd) + MAX_NAMESIZE * smp->size);				
	}
	return true;
}
bool Gm::clearColddown(SceneUser *pUser, const char * para)
{
	if (pUser)
	{
		pUser->reSendOther=true;
		pUser->usm.clearAllUseTime();
	}
	return true;
}

bool Gm::debugCityDare(SceneUser *pUser, const char * para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	Cmd::Session::t_GMCreateUnionCity_SceneSession send;
	send.byOpType = atoi(prop["state"].c_str());
	send.dwCityID = pUser->scene->getRealMapID();
	send.dwCountryID = pUser->charbase.country;
	sessionClient->sendCmd(&send, sizeof(send));

	return true;
}

bool Gm::debugEmperorDare(SceneUser *pUser, const char * para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	Cmd::Session::t_beginEmperorDare_SceneSession send;
	sessionClient->sendCmd(&send, sizeof(send));

	return true;
}

bool Gm::changeCountry(SceneUser *pUser, const char * para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	is_cancel_country = atoi(prop["state"].c_str());
	cancel_country_need_money = atoi(prop["money"].c_str());

	return true;
}

bool Gm::setSeptLevel(SceneUser *pUser, const char * para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	Cmd::Session::t_OpLevel_SceneSession send;
	
	send.dwLevel = atoi(prop["value"].c_str());
	send.dwSeptID = pUser->charbase.septid;
	sessionClient->sendCmd(&send, sizeof(send));

	return true;
}

bool Gm::setAllyFriendDegree(SceneUser *pUser, const char * para)
{
	zProperties prop;
	prop.parseCmdLine(para);
	Cmd::Session::t_opAllyFrienddegree_SceneSession send;
	
	send.friendDegree = atoi(prop["value"].c_str());
	send.dwCountryID = pUser->charbase.country;
	sessionClient->sendCmd(&send, sizeof(send));
	
	ScenesService::gm_logger->trace("GM %s 使用setallyfd命令，设置 %s 国的联盟友好度", 
			pUser->name, prop["value"].c_str());                                    

	return true;
}

bool Gm::setRepute(SceneUser *pUser, const char * para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	Cmd::Session::t_OpRepute_SceneSession send;
	send.dwRepute = atoi(prop["value"].c_str());
	send.dwSeptID = pUser->charbase.septid;
	sessionClient->sendCmd(&send, sizeof(send));

	return true;
}

bool Gm::debugVote(SceneUser *pUser, const char * para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	Cmd::Session::t_opTechVote_SceneSession send;
	send.byOpType = atoi(prop["state"].c_str());
	send.dwCountryID = pUser->charbase.country;
	sessionClient->sendCmd(&send, sizeof(send));
	
	return true;
}

bool Gm::debugGem(SceneUser *pUser, const char * para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	Cmd::Session::t_OpGemState_SceneSession send;
	send.dwState = atoi(prop["state"].c_str());
	sessionClient->sendCmd(&send, sizeof(send));

	return true;
}

bool Gm::addSeptExp(SceneUser *pUser, const char * para)
{
	zProperties prop;
	prop.parseCmdLine(para);

	Cmd::Session::t_opTechVote_SceneSession send;
	sessionClient->sendCmd(&send, sizeof(send));

	return true;
}

bool Gm::checkQuest(SceneUser *pUser, const char * para)
{
	if (pUser)
		EventManager<OnVisit>::instance().get_valid_quest_str(*pUser);
	return true;
}

/*
 * \brief 设置游戏功能的开关
 * \param pUser 指令使用者
 * \param para 参数
 *
 * \return 执行结果
 *
 */
bool Gm::setService(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;
	using namespace Cmd::Session;

	zProperties prop;
	if (0==prop.parseCmdLine(para))
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "当前服务状态 flag=%s", Zebra::global["service_flag"].c_str());
		return true;
	}

	DWORD f = atoi(Zebra::global["service_flag"].c_str());
	if (prop["mail"]=="on")
		f |= SERVICE_MAIL;
	else if (prop["mail"]=="off")
		f &= ~SERVICE_MAIL;

	if (prop["auction"]=="on")
		f |= SERVICE_AUCTION;
	else if (prop["auction"]=="off")
		f &= ~SERVICE_AUCTION;

	if (prop["process"]=="on")
		f |= SERVICE_PROCESS;
	else if (prop["process"]=="off")
		f &= ~SERVICE_PROCESS;

	if (prop["horse"]=="on")
		f |= SERVICE_HORSE;
	else if (prop["horse"]=="off")
		f &= ~SERVICE_HORSE;

	if (prop["flag"]!="")
		f = atoi(prop["flag"].c_str());

	/*
	char buf[32];
	bzero(buf, sizeof(buf));
	snprintf(buf, 32, "%u", f);
	Zebra::global["service_flag"] = buf;
	*/

	Cmd::Session::t_SetService_SceneSession send;
	send.flag = f;
	sessionClient->sendCmd(&send, sizeof(send));
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "当前服务状态 flag=%u", f);
	ScenesService::gm_logger->trace("GM %s 设置服务 flag=%u", pUser->name, f);

	return true;
}

/*
 * \brief 设置游戏功能的开关
 * \param pUser 指令使用者
 * \param para 参数
 *
 * \return 执行结果
 *
 */
bool Gm::setTire(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	if (prop["oper"]=="clear")
	{
		if (prop["hour"] != "")
		{
			int tm_hour = atoi(prop["hour"].c_str());
			if (tm_hour<0 || tm_hour>23) return true;
			for(int i = 0; i < 60; i++)
			{
				int j = (tm_hour*60+i)/5;
				Cmd::clear_state(((unsigned char *)(pUser->charbase.tiretime)),j);
			}
		}
		else
		{
			bzero(pUser->charbase.tiretime, 36);
			pUser->wdTire = 0;
		}
	}
	else if (prop["oper"]=="set")
	{
		if (prop["hour"] != "")
		{
			int tm_hour = atoi(prop["hour"].c_str());
			if (tm_hour<0 || tm_hour>23) return false;
			for(int i = 0; i < 60; i++)
			{
				int j = (tm_hour*60+i)/5;
				Cmd::set_state(((unsigned char *)(pUser->charbase.tiretime)),j);
			}
		}
		else
		{
			for(int x = 0; x < 24; x++)
			for(int i = 0; i < 60; i++)
			{
				int j = (x*60+i)/5;
				Cmd::set_state(((unsigned char *)(pUser->charbase.tiretime)),j);
			}
		}
	}
	int count=0;
	for(int k=0; k<288;k++)
	{
		if (Cmd::isset_state(((unsigned char *)(pUser->charbase.tiretime)), k))
		{
			count++;
		}
	}
	pUser->wdTire=count;
	pUser->callProcessTire();
	return true;
}

/*
 * \brief 设置开启/关闭某国家注册
 * \param pUser 指令使用者
 * \param para 参数
 *
 * \return 执行结果
 *
 */
bool Gm::enableRegister(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	if (prop["countryid"]!="")
	{
		WORD wdCountryID = atoi(prop["countryid"].c_str());
		if (prop["status"]=="true")
		{
			Cmd::Scene::t_ChangeCountryStatus send;
			send.oper = Cmd::Scene::ENABLE_REGISTER;
			send.country = wdCountryID;
			send.dwUserID = pUser->id;
			SceneTaskManager::getInstance().broadcastCmd(&send,sizeof(send));
			ScenesService::gm_logger->trace("GM %s 设置了国家%u注册允许", pUser->name, wdCountryID);
		}
		else if (prop["status"]=="false")
		{
			Cmd::Scene::t_ChangeCountryStatus send;
			send.oper = Cmd::Scene::DISABLE_REGISTER;
			send.country = wdCountryID;
			send.dwUserID = pUser->id;
			SceneTaskManager::getInstance().broadcastCmd(&send,sizeof(send));
			ScenesService::gm_logger->trace("GM %s 设置了国家%u注册关闭", pUser->name, wdCountryID);
		}
	}
	return true;
}

/*
 * \brief 设置开启/关闭某国家注册
 * \param pUser 指令使用者
 * \param para 参数
 *
 * \return 执行结果
 *
 */
bool Gm::enableLogin(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	if (prop["countryid"]!="")
	{
		WORD wdCountryID = atoi(prop["countryid"].c_str());
		if (prop["status"]=="true")
		{
			Cmd::Scene::t_ChangeCountryStatus send;
			send.oper = Cmd::Scene::ENABLE_LOGIN;
			send.country = wdCountryID;
			send.dwUserID = pUser->id;
			SceneTaskManager::getInstance().broadcastCmd(&send,sizeof(send));
			ScenesService::gm_logger->trace("GM %s 设置了国家%u登陆允许", pUser->name, wdCountryID);
		}
		else if (prop["status"]=="false")
		{
			Cmd::Scene::t_ChangeCountryStatus send;
			send.oper = Cmd::Scene::DISABLE_LOGIN;
			send.country = wdCountryID;
			send.dwUserID = pUser->id;
			SceneTaskManager::getInstance().broadcastCmd(&send,sizeof(send));
			ScenesService::gm_logger->trace("GM %s 设置了国家%u登陆关闭", pUser->name, wdCountryID);
		}
	}
	return true;
}

/*
 * \brief 设置/显示 服务器版本号
 * \param pUser 指令使用者
 * \param para 参数
 *
 * \return 执行结果
 *
 */
bool Gm::version(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	if (prop["oper"]=="set")
	{
		if (prop["id"]!="")
		{
			DWORD wdVersionID = atoi(prop["id"].c_str());
			Cmd::Scene::t_ChangeVerifyVersion send;
			send.oper = Cmd::Scene::CHANGECODE;
			send.versionCode = wdVersionID;
			send.dwUserID = pUser->id;
			SceneTaskManager::getInstance().broadcastCmd(&send,sizeof(send));
			ScenesService::gm_logger->trace("GM %s 设置服务器版本校验号为%u", pUser->name, wdVersionID);
		}
	}
	else if (prop["oper"]=="show")
	{
		Cmd::Scene::t_ChangeVerifyVersion send;
		send.oper = Cmd::Scene::SHOWCODE;
		send.versionCode = 0;
		send.dwUserID = pUser->id;
		SceneTaskManager::getInstance().broadcastCmd(&send,sizeof(send));
		ScenesService::gm_logger->trace("GM %s 查看版本校验号", pUser->name);
	}
	return true;
}

/*
 * \brief 显示国家注册/登陆允许状态列表
 * \param pUser 指令使用者
 * \param para 参数
 *
 * \return 执行结果
 *
 */
bool Gm::checkCountryInfo(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;

	Cmd::Scene::t_ChangeCountryStatus send;
	send.oper = Cmd::Scene::LIST_COUNTRY_INFO;
	send.dwUserID = pUser->id;
	pUser->gatetask->sendCmd(&send,sizeof(send));
	return true;
}

bool Gm::showAddExp(SceneUser *pUser, const char * para)
{
	if (!pUser || !para) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	DWORD id = atoi(prop["id"].c_str());

	if (pUser->cartoon && pUser->cartoon->getCartoonID()==id)
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "npc: %u", pUser->cartoon->getCartoonData().addExp);
	if (pUser->cartoonList.find(id)!=pUser->cartoonList.end())
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "cartoonList: %u", pUser->cartoonList[id].addExp);
	return true;
}

bool Gm::changeHorse(SceneUser *pUser, const char * para)
{
	if (!pUser || !para) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	DWORD id = atoi(prop["id"].c_str());

	pUser->horse.horse(id);
	ScenesService::gm_logger->trace("GM %s 用changehorse指令换马 id=%u", pUser->name, id);
	return true;
}

bool Gm::loadProcess(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;

	Cmd::Session::t_gmCommand_SceneSession send;
	strncpy((char *)send.src_name, pUser->name, MAX_NAMESIZE);
	send.gm_cmd = Cmd::Session::GM_COMMAND_LOAD_PROCESS;
	sessionClient->sendCmd(&send, sizeof(send));

	ScenesService::gm_logger->trace("GM %s 使用loadProcess指令", pUser->name);
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "加载process文件，请查看各场景日志");
	return true;
}

bool Gm::loadGift(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;

	Cmd::Session::t_gmCommand_SceneSession send;
	strncpy((char *)send.src_name, pUser->name, MAX_NAMESIZE);
	send.gm_cmd = Cmd::Session::GM_COMMAND_LOAD_GIFT;
	sessionClient->sendCmd(&send, sizeof(send));

	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "加载礼官数据，请查看session日志");
	return true;
}

bool Gm::contribute(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	Cmd::Session::t_ContributeCountry_SceneSession send;
	send.dwCountry = atoi(prop["country"].c_str())?atoi(prop["country"].c_str()):2;
	send.dwValue = atoi(prop["num"].c_str())?atoi(prop["num"].c_str()):1000;
	send.byType = atoi(prop["type"].c_str())?atoi(prop["type"].c_str()):1;
	sessionClient->sendCmd(&send, sizeof(send));

	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "贡献，country=%u type=%u num=%u", send.dwCountry, send.byType, send.dwValue);
	return true;
}

bool Gm::refreshGeneral(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;

	zProperties prop;
	prop.parseCmdLine(para);

	Cmd::Session::t_gmCommand_SceneSession send;
	send.gm_cmd = Cmd::Session::GM_COMMAND_REFRESH_GENERAL;
	sessionClient->sendCmd(&send, sizeof(send));

	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "刷新大将军，请查看场景或session的日志");
	return true;
}

bool Gm::closenpc(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;

	Cmd::Session::t_CloseNpc_SceneSession send;
	sessionClient->sendCmd(&send, sizeof(send));

	ScenesService::gm_logger->trace("GM %s 使用closenpc指令", pUser->name);
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "关闭功能NPC准备停机");
	return true;
}

bool Gm::countrypower(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;

	Cmd::Session::t_DebugCountryPower_SceneSession send;
	sessionClient->sendCmd(&send, sizeof(send));

	ScenesService::gm_logger->trace("GM %s 使用countrypower指令", pUser->name);
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "立即处理国家强弱计算");
	return true;
}

void Gm::sendLog(SceneUser *pUser, const char *cmd, const char * content)
{
	using namespace Cmd::GmTool;
	t_Log_GmTool send;
	send.time = SceneTimeTick::currentTime.sec();
	send.charID = pUser->id;
	send.accID = pUser->accid;
	strncpy(send.GM, pUser->name, MAX_NAMESIZE);
	snprintf(send.pos, MAX_NAMESIZE, "%s (%u,%u)", pUser->scene->name, pUser->getPos().x, pUser->getPos().y);
	strncpy(send.cmd, cmd, MAX_NAMESIZE);
	strncpy(send.content, content, MAX_NAMESIZE);

	ScenesService::getInstance().sendCmdToSuperServer(&send, sizeof(send));
}

bool Gm::getgive(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;
	pUser->charbase.goldgive +=10;
	pUser->Give_MatarialNum +=2;
	pUser->Card_num +=10;
    Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "获得赠品");
	return true;

}

//soke 新的脚本命令
bool Gm::loadMessage(SceneUser *pUser, const char * para)
{
	Cmd::Session::t_gmCommand_SceneSession send;
	send.gm_cmd = Cmd::Session::GM_COMMAND_LOADMESSAGE;
	sessionClient->sendCmd(&send, sizeof(send));
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "重新加载Message系统");
	return true;

}

bool Gm::loadQuest(SceneUser *pUser, const char * para)
{
	//暂时关闭反外挂系统
	ScenesService::getInstance().fanwaiguarongcuo +=1;
	//soke 世界公告
	Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"尊敬的玩家您好，服务器即将更新配置文件，短暂卡顿属于正常现象，请您耐心等候，谢谢！" );	
	Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"尊敬的玩家您好，服务器即将更新配置文件，短暂卡顿属于正常现象，请您耐心等候，谢谢！" );	
	Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"尊敬的玩家您好，服务器即将更新配置文件，短暂卡顿属于正常现象，请您耐心等候，谢谢！" );	
	Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"尊敬的玩家您好，服务器即将更新配置文件，短暂卡顿属于正常现象，请您耐心等候，谢谢！" );	
	Cmd::Session::t_gmCommand_SceneSession send;
	send.gm_cmd = Cmd::Session::GM_COMMAND_LOADQUEST;
	sessionClient->sendCmd(&send, sizeof(send));
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "重新加载任务脚本");
	return true;

}

bool Gm::loadNewQuest(SceneUser *pUser, const char * para)
{
	Cmd::Session::t_gmCommand_SceneSession send;
	send.gm_cmd = Cmd::Session::GM_COMMAND_LOADNEWQUEST;
	sessionClient->sendCmd(&send, sizeof(send));
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "使用GM命令重新加载lua任务脚本");
	return true;

}

bool Gm::loadGiftBox(SceneUser *pUser, const char * para)
{
	Cmd::Session::t_gmCommand_SceneSession send;
	send.gm_cmd = Cmd::Session::GM_COMMAND_LOADGIFTBOX;
	sessionClient->sendCmd(&send, sizeof(send));
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "重新加载GiftBoxConfig.xml脚本");
	return true;

}
bool Gm::loadNpctrade(SceneUser *pUser, const char * para)
{
	Cmd::Session::t_gmCommand_SceneSession send;
	send.gm_cmd = Cmd::Session::GM_COMMAND_LOADNPCTRADE;
	sessionClient->sendCmd(&send, sizeof(send));
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "重新加载npctrade.xml脚本");
	return true;

}

bool Gm::loadTbl(SceneUser *pUser, const char * para)
{
	//soke 世界公告
	Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"尊敬的玩家您好，服务器即将更新配置文件，短暂卡顿属于正常现象，请您耐心等候，谢谢！" );	
	Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"尊敬的玩家您好，服务器即将更新配置文件，短暂卡顿属于正常现象，请您耐心等候，谢谢！" );	
	Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"尊敬的玩家您好，服务器即将更新配置文件，短暂卡顿属于正常现象，请您耐心等候，谢谢！" );	
	Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"尊敬的玩家您好，服务器即将更新配置文件，短暂卡顿属于正常现象，请您耐心等候，谢谢！" );	
	Cmd::Session::t_gmCommand_SceneSession send;
	send.gm_cmd = Cmd::Session::GM_COMMAND_LOADTBL;
	sessionClient->sendCmd(&send, sizeof(send));
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "重新加载TBL文件");
	return true;

}

bool Gm::loadTbl2(SceneUser *pUser, const char * para)
{
	//soke 世界公告
	Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"尊敬的玩家您好，服务器即将更新配置文件，短暂卡顿属于正常现象，请您耐心等候，谢谢！" );	
	Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"尊敬的玩家您好，服务器即将更新配置文件，短暂卡顿属于正常现象，请您耐心等候，谢谢！" );	
	Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"尊敬的玩家您好，服务器即将更新配置文件，短暂卡顿属于正常现象，请您耐心等候，谢谢！" );	
	Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"尊敬的玩家您好，服务器即将更新配置文件，短暂卡顿属于正常现象，请您耐心等候，谢谢！" );	
	Cmd::Session::t_gmCommand_SceneSession send;
	send.gm_cmd = Cmd::Session::GM_COMMAND_LOADTBL2;
	sessionClient->sendCmd(&send, sizeof(send));
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "重新加载TBL文件");
	return true;

}

bool Gm::loadGameConfig( SceneUser *pUser, const char * para )
{
	Cmd::Session::t_gmCommand_SceneSession send;
	send.gm_cmd = Cmd::Session::GM_COMMAND_LOAD_GAME_CONFIG;
	sessionClient->sendCmd(&send, sizeof(send));
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "重新加载温泉配置脚本");
	return true;
}
bool Gm::loadFuBenConfig( SceneUser *pUser, const char * para )
{
	Cmd::Session::t_gmCommand_SceneSession send;
	send.gm_cmd = Cmd::Session::GM_COMMAND_LOAD_FUBEN_CONFIG;
	sessionClient->sendCmd(&send, sizeof(send));
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "重新加载副本配置脚本"); //副本
	return true;
}
/*
 * \brief 设置游戏等级封印功能的开关（未转生）
 * \param pUser 指令使用者
 * \param para 参数
 *
 * \return 执行结果
 *
 */
bool Gm::setFylevel(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;
	using namespace Cmd::Session;

	zProperties prop;
	if (0==prop.parseCmdLine(para))
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "当前（未转生）封印等级 flag=%s", Zebra::global["serverfy_level"].c_str());
		return true;
	}

	DWORD f = atoi(Zebra::global["serverfy_level"].c_str());

	if (prop["process"]=="on")
		f |= SERVICE2_PROCESS;
	else if (prop["process"]=="off")
		f &= ~SERVICE2_PROCESS;

	if (prop["flag"]!="")
		f = atoi(prop["flag"].c_str());

	Cmd::Session::t_SetFylevel_SceneSession send;
	send.flag = f;
	sessionClient->sendCmd(&send, sizeof(send));
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "当前（未转生）封印等级 flag=%u", f);
	ScenesService::gm_logger->trace("GM %s 设置（未转生）封印等级 flag=%u", pUser->name, f);

	return true;
}

/*
 * \brief 设置游戏满级等级功能的开关（未转生）
 * \param pUser 指令使用者
 * \param para 参数
 *
 * \return 执行结果
 *
 */
bool Gm::setMaxlevel(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;
	using namespace Cmd::Session;

	zProperties prop;
	if (0==prop.parseCmdLine(para))
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "服务器（未转生）最高等级 flag=%s", Zebra::global["servermax_level"].c_str());
		return true;
	}

	DWORD f = atoi(Zebra::global["servermax_level"].c_str());

	if (prop["process"]=="on")
		f |= SERVICE3_PROCESS;
	else if (prop["process"]=="off")
		f &= ~SERVICE3_PROCESS;

	if (prop["flag"]!="")
		f = atoi(prop["flag"].c_str());

	Cmd::Session::t_SetMaxlevel_SceneSession send;
	send.flag = f;
	sessionClient->sendCmd(&send, sizeof(send));
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "当前服务器（未转生）最高等级 flag=%u", f);
	ScenesService::gm_logger->trace("GM %s 设置（未转生）最高服务器等级 flag=%u", pUser->name, f);

	return true;
}

/*
 * \brief 设置游戏等级封印功能的开关（转生）
 * \param pUser 指令使用者
 * \param para 参数
 *
 * \return 执行结果
 *
 */
bool Gm::setTrunFylevel(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;
	using namespace Cmd::Session;

	zProperties prop;
	if (0==prop.parseCmdLine(para))
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "当前（转生）封印等级 flag=%s", Zebra::global["trunfy_level"].c_str());
		return true;
	}

	DWORD f = atoi(Zebra::global["trunfy_level"].c_str());

	if (prop["process"]=="on")
		f |= SERVICE4_PROCESS;
	else if (prop["process"]=="off")
		f &= ~SERVICE4_PROCESS;

	if (prop["flag"]!="")
		f = atoi(prop["flag"].c_str());

	Cmd::Session::t_SetTrunFylevel_SceneSession send;
	send.flag = f;
	sessionClient->sendCmd(&send, sizeof(send));
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "当前（转生）封印等级 flag=%u", f);
	ScenesService::gm_logger->trace("GM %s 设置（转生）封印等级 flag=%u", pUser->name, f);

	return true;
}

/*
 * \brief 设置游戏满级等级功能的开关（转生）
 * \param pUser 指令使用者
 * \param para 参数
 *
 * \return 执行结果
 *
 */
bool Gm::setTrunMaxlevel(SceneUser *pUser, const char * para)
{
	if (!pUser) return false;
	using namespace Cmd::Session;

	zProperties prop;
	if (0==prop.parseCmdLine(para))
	{
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "服务器（转生）最高等级 flag=%s", Zebra::global["maxtrun_level"].c_str());
		return true;
	}

	DWORD f = atoi(Zebra::global["maxtrun_level"].c_str());

	if (prop["process"]=="on")
		f |= SERVICE5_PROCESS;
	else if (prop["process"]=="off")
		f &= ~SERVICE5_PROCESS;

	if (prop["flag"]!="")
		f = atoi(prop["flag"].c_str());

	Cmd::Session::t_SetTrunMaxlevel_SceneSession send;
	send.flag = f;
	sessionClient->sendCmd(&send, sizeof(send));
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "当前服务器（转生）最高等级 flag=%u", f);
	ScenesService::gm_logger->trace("GM %s 设置（转生）最高服务器等级 flag=%u", pUser->name, f);

	return true;
}
