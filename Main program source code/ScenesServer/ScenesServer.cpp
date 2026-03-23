/**
 * \file
 * \version  $Id: ScenesServer.cpp  $
 * \author  
 * \date 
 * \brief zebra项目场景服务器，游戏绝大部分内容都在本实现
 */

#include "zSubNetService.h"
#include "Zebra.h"
#include "SceneManager.h"
#include "SceneTaskManager.h"
#include "zMisc.h"
#include "ScenesServer.h"
#include "SceneTask.h"
#include "RecordClient.h"
#include "SessionClient.h"
#include "zConfile.h"
#include "TimeTick.h"
#include "zDatabaseManager.h"
#include "SceneNpcManager.h"
#include "NpcTrade.h"
#include "MessageSystem.h"
#include "QuestTable.h"
#include "MagicRange.h"
#include "CountryDare.h"
#include "CountryTech.h"
#include "CountryAlly.h"
#include "GmToolCommand.h"
#include "QuestEvent.h"
//#include "QuestTable.h"
#include "script.h"
#include "MiniClient.h"
#include "GameConfigMgr.h"
#include "Scene.h"
#include "GlobalBox.h"
#include "FuBenMgr.h" //副本
#include "MallTrade.h" //新商城
#include "fjconfig.h"//五附件	

ScenesService *ScenesService::instance = NULL;
bool ScenesService::reload=false;
zLogger * ScenesService::gm_logger = NULL;
zLogger * ScenesService::objlogger = NULL;
zLogger * ScenesService::wg_logger = NULL;
//soke 转生增加的技能和属性点
WORD trun_point_rate = 0;
WORD trun_skill_rate = 0;
Cmd::stChannelChatUserCmd * ScenesService::pStampData = 0;
/// 判国所需经费
unsigned int cancel_country_need_money = 50000; //默认五锭
unsigned int is_cancel_country = 0; // 是否允许叛国

//std::string COfflineSkillStatus::rootpath = "";

/**
 * \brief 初始化网络服务器程序
 *
 * 实现了虚函数<code>zService::init</code>
 *
 * \return 是否成功
 */
bool ScenesService::init()
{
	/* if(!zcaaadfss())
		return false; */
	for(int i=0; i<13; i++) countryPower[i]=1;

	//初始化连接线程池
	int state = state_none;
	Zebra::to_lower(Zebra::global["initThreadPoolState"]);
	if ("repair" == Zebra::global["initThreadPoolState"]
			|| "maintain" == Zebra::global["initThreadPoolState"])
		state = state_maintain;
	taskPool = new zTCPTaskPool(atoi(Zebra::global["threadPoolCapacity"].c_str()), state,5000);
	
	//soke转生角色属性点和技能点从配置读取
	trun_point_rate = atoi(Zebra::global["trun_point_rate"].c_str());
	trun_skill_rate = atoi(Zebra::global["trun_skill_rate"].c_str());
	if (NULL == taskPool
			|| !taskPool->init())
		return false;

	strncpy(pstrIP, zSocket::getIPByIfName(Zebra::global["ifname"].c_str()), MAX_IP_LENGTH - 1);
	//Zebra::logger->debug("%s", pstrIP);

	if (!zSubNetService::init())
	{
		return false;
	}

	const Cmd::Super::ServerEntry *serverEntry = NULL;

	//连接档案服务器
	serverEntry = getServerEntryByType(RECORDSERVER);
	if (NULL == serverEntry)
	{
		Zebra::logger->error("不能找到档案服务器相关信息，不能连接档案服务器");
		return false;
	}
	recordClient = new RecordClient("档案服务器", serverEntry->pstrExtIP, serverEntry->wdExtPort);
	if (NULL == recordClient)
	{
		Zebra::logger->error("没有足够内存，不能建立档案服务器客户端实例");
		return false;
	}
	if (!recordClient->connectToRecordServer())
	{
		Zebra::logger->error("连接档案服务器失败 %s", __PRETTY_FUNCTION__);
		return false;
	}
	if(recordClient->start())
		Zebra::logger->info("初始化Record服务器模块(%s:%ld)...成功",serverEntry->pstrExtIP,serverEntry->wdExtPort);

	//连接Session服务器
	serverEntry = getServerEntryByType(SESSIONSERVER);
	if (NULL == serverEntry)
	{
		Zebra::logger->error("不能找到Session服务器相关信息，不能连接Session服务器");
		return false;
	}
	sessionClient = new SessionClient("Session服务器", serverEntry->pstrExtIP, serverEntry->wdExtPort);
	if (NULL == sessionClient)
	{
		Zebra::logger->error("没有足够内存，不能建立Session服务器客户端实例");
		return false;
	}
	if (!sessionClient->connectToSessionServer())
	{
		Zebra::logger->error("连接Session服务器失败 %s", __PRETTY_FUNCTION__);
		return false;
	}
	if(sessionClient->start())
		Zebra::logger->info("初始化Session服务器模块(%s:%ld)...成功",serverEntry->pstrExtIP,serverEntry->wdExtPort);

	//连接小游戏服务器
	serverEntry = getServerEntryByType(MINISERVER);
	if (NULL == serverEntry)
	{
		Zebra::logger->error("不能找到小游戏服务器相关信息，不能连接小游戏服务器");
		return false;
	}
	miniClient = new MiniClient("小游戏服务器", serverEntry->pstrExtIP, serverEntry->wdExtPort,serverEntry->wdServerID);
	if (NULL == miniClient)
	{
		Zebra::logger->error("没有足够内存，不能建立小游戏服务器客户端实例");
		return false;
	}
	if (!miniClient->connectToMiniServer())
	{
		Zebra::logger->error("连接小游戏服务器失败 %s", __PRETTY_FUNCTION__);
		return false;
	}
	if (miniClient->start())
		Zebra::logger->info("初始化Mini服务器模块(%s:%ld)...成功",serverEntry->pstrExtIP,serverEntry->wdExtPort);

	if (SceneNpcManager::getMe().init())
	{
		Zebra::logger->info("初始化NPC管理器...成功");
	}

	if(SceneTimeTick::getInstance().start())
		Zebra::logger->info("初始化TimeTick模块...成功");

	//加载基本数据
	if(!loadAllBM())
	{
		Zebra::logger->error("初始化基本数据模块...失败");
		return false;
	}
	char srv[256];
	bzero(srv, sizeof(srv));
	sprintf(srv , "WS[%d]",getServerID());
	objlogger = new zLogger(srv);
	objlogger->setLevel(Zebra::global["log"]);
	//设置写本地日志文件
	if ("" != Zebra::global["objlogfilename"])
	{
		bzero(srv, sizeof(srv));
		char sub[256];
		bzero(sub, sizeof(sub));
		snprintf(srv , sizeof(srv),"%s",Zebra::global["objlogfilename"].c_str());
		char *tok = strstr(srv , ".");
		if( tok != NULL)
		{
			strncpy(sub,tok,sizeof(sub));
			bzero(tok , strlen(tok));
			sprintf(srv + strlen(srv) , "%d",getServerID());
			strncat(srv,sub,sizeof(srv) - strlen(srv));
		}
		else
		{
			snprintf(srv + strlen(srv) , sizeof(srv) - strlen(srv) , "%d",getServerID());
		}
		objlogger->addLocalFileLog(srv);
		objlogger->removeConsoleLog();
	}

	gm_logger = new zLogger("gm_log");
	gm_logger->setLevel(Zebra::global["log"]);
	if ("" != Zebra::global["gm_logfile"])
		gm_logger->addLocalFileLog(Zebra::global["gm_logfile"]);
	gm_logger->removeConsoleLog();

	wg_logger = new zLogger("wg_log");
	wg_logger->setLevel(Zebra::global["log"]);
	if ("" != Zebra::global["wg_logfile"])
		wg_logger->addLocalFileLog(Zebra::global["wg_logfile"]);
	wg_logger->removeConsoleLog();

	Zebra::logger->info("加载特征码文件...，大小 %u", updateStampData());

	if (!SceneManager::getInstance().init())
	{
		Zebra::logger->error("初始化场景管理器...失败");
		return false;
	}
	else
		Zebra::logger->info("初始化场景管理器...成功");

	//副本新
    if ( FuBenMgr::getMe().init() )
    {
		Zebra::logger->info("初始化副本管理器...成功");
    }

	if(!NpcTrade::getInstance().init())
	{
		Zebra::logger->error("初始化NPC交易配置模块...失败");
		return false;
	}
	//TZ 2023 10 15新商城
	if(!MallTrade::getInstance().init()){
		Zebra::logger->error("初始化商城配置文件失败");
		return false;
	}

	// zRTime cccaaa;
	// cccaaa.now();
	// if(cccaaa.sec() >= 1729458831)//醉梦 后门 2024年5月1日后场景将无法启动
	// {
	// 	Zebra::logger->error("初始化商城配置文件失败");
	// 	exit(0);
	// }
	
	
	
	if(!fjconfig::getInstance().init()){
		Zebra::logger->error("初始化剑冢配置文件失败");
		return false;
	}
	if(!fjconfig::getInstance().initztz()){
		Zebra::logger->error("初始化征途传配置文件失败");
		return false;
	}
	if(!fjconfig::getInstance().initshengxiao()){
		Zebra::logger->error("初始化生肖配置文件失败");
		return false;
	}
	if(!fjconfig::getInstance().initshengqi()){
		Zebra::logger->error("初始化圣器配置文件失败");
		return false;
	}
	
	if(! fjconfig::getInstance().initjingmai()){
		Zebra::logger->error("初始化经脉配置文件失败");
		return false;
	}

	if(! fjconfig::getInstance().initTouxian()){
		Zebra::logger->error("初始化头衔配置文件失败");
		return false;
	}

	if(! fjconfig::getInstance().initChenghao()){
		Zebra::logger->error("初始化称号配置文件失败");
		return false;
	}

	//儿女
	if(! fjconfig::getInstance().initErnv()){
		Zebra::logger->error("初始化生儿育女配置文件失败");
		return false;
	}

	//回收系統
	if(! fjconfig::getInstance().initHuishou()){
		Zebra::logger->error("初始化回收系统配置文件失败");
		return false;
	}

	//云天别墅
	if(! fjconfig::getInstance().initBieshu()){
		Zebra::logger->error("初始化云天别墅配置文件失败");
		return false;
	}
	
	//能力修炼
	if(! fjconfig::getInstance().initXiulian()){
		Zebra::logger->error("初始化能力修炼配置文件失败");
		return false;
	}

	//坐骑图鉴
	if(! fjconfig::getInstance().initZuoqi()){
		Zebra::logger->error("初始化坐骑图鉴配置文件失败");
		return false;
	}

	//时装魔盒
	if(! fjconfig::getInstance().initMohe()){
		Zebra::logger->error("初始化魔盒配置文件失败");
		return false;
	}

	//时装魔盒
	if(! fjconfig::getInstance().initShizhuang()){
		Zebra::logger->error("初始化魔盒【时装类】配置文件失败");
		return false;
	}
	
	//时装魔盒
	if(! fjconfig::getInstance().initShiZhuangLevel()){
		Zebra::logger->error("初始化魔盒【时装进阶类】配置文件失败");
		return false;
	}

	//时装魔盒
	if(! fjconfig::getInstance().initPifeng()){
		Zebra::logger->error("初始化魔盒【披风类】配置文件失败");
		return false;
	}

	//时装魔盒
	if(! fjconfig::getInstance().initPiFengLevel()){
		Zebra::logger->error("初始化魔盒【披风进阶类】配置文件失败");
		return false;
	}

	//时装魔盒
	if(! fjconfig::getInstance().initChibang()){
		Zebra::logger->error("初始化魔盒【翅膀类】配置文件失败");
		return false;
	}

	//时装魔盒
	if(! fjconfig::getInstance().initChiBangLevel()){
		Zebra::logger->error("初始化魔盒【翅膀进阶类】配置文件失败");
		return false;
	}

	//时装魔盒
	if(! fjconfig::getInstance().initZuoqi2()){
		Zebra::logger->error("初始化魔盒【坐骑类】配置文件失败");
		return false;
	}
	
	//初始化天下第一
	for (int i = 0; i < 32; i++)
	{
		tianxia[i].jifen=0;
		tianxia[i].saidian=-1;
		tianxia[i].userid = 0;
	}
	Zebra::logger->info("初始化天下第一系统成功");

	//初始化迷宫系统
	for (int i = 0; i < 25; i++)
	{
		migong[i].ceng=i+1;
		migong[i].password=zMisc::randBetween(1,3);
	}
	Zebra::logger->info("初始化迷宫系统成功");

	//反外挂初始化
	fanwaigua = true;
	fanwaiguarongcuo = 0;
	Zebra::logger->info("初始化反作弊系统成功");
	
	///////////////////////////////////////
	if(!MessageSystem::getInstance().init())
	{
		Zebra::logger->error("初始化Message消息模块...失败");
		return false;
	}

	ALLVARS1(server_id, getServerID());
	ALLVARS(load);

	if(!QuestTable::instance().init())
	{
		Zebra::logger->error("初始化任务模块...失败");
		return false;
	}

	if(!GameConfigMgr::getMe().init())
	{
		Zebra::logger->error("初始化场景服务器温泉配置...失败");
		return false;
	}

	LuaVM* vm = ScriptingSystemLua::instance().createVM();
	LuaScript* script = ScriptingSystemLua::instance().createScriptFromFile("newquest/quest.lua");
	Binder bind;
	bind.bind(vm);
	vm->execute(script);
	SAFE_DELETE(script);
	//sppeed up
	ScriptQuest::get_instance().sort();
	
	if(!MagicRangeInit::getInstance().init())
	{
		Zebra::logger->error("初始化攻击范围定义模块...失败");
		return false;
	}

	if(!globalBox::getInstance().init())
	{
		Zebra::logger->error("初始化宝箱模块...失败");
		return false;
	}

	CountryDareM::getMe().init();
	CountryTechM::getMe().init();
	CountryAllyM::getMe().init();

/*
	if(!COfflineSkillStatus::init())
	{
		Zebra::logger->error("初始化技能离线状态记录模块...失败");
		return false;
	}
*/
//	SceneUserVisitor::Thread::instance().start();
	return true;
}

/**
 * \brief 新建立一个连接任务
 *
 * 实现纯虚函数<code>zNetService::newTCPTask</code>
 *
 * \param sock TCP/IP连接
 * \param addr 地址
 */
void ScenesService::newTCPTask(const int sock, const struct sockaddr_in *addr)
{
	SceneTask *tcpTask = new SceneTask(taskPool, sock, addr);
	if (NULL == tcpTask)
		//内存不足，直接关闭连接
		TEMP_FAILURE_RETRY(::close(sock));
	else if(!taskPool->addVerify(tcpTask))
	{
		//得到了一个正确连接，添加到验证队列中
		SAFE_DELETE(tcpTask);
	}
}

/**
 * \brief 解析来自服务器管理器的指令
 *
 * 这些指令是网关和服务器管理器交互的指令<br>
 * 实现了虚函数<code>zSubNetService::msgParse_SuperService</code>
 *
 * \param ptNullCmd 待解析的指令
 * \param nCmdLen 待解析的指令长度
 * \return 解析是否成功
 */
bool ScenesService::msgParse_SuperService(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	switch(ptNullCmd->cmd)
	{
		case Cmd::GmTool::CMD_GMTOOL:
			{
				using namespace Cmd::GmTool;

				switch(ptNullCmd->para)
				{
					case PARA_PUNISH_GMTOOL:
						{
							t_Punish_GmTool * rev = (t_Punish_GmTool *)ptNullCmd;
							SceneUser *pUser = SceneUserManager::getMe().getUserByName(rev->userName);
							if (!pUser) break;
							switch (rev->operation)
							{
							case 1://禁言
								{
									pUser->delayForbidTalk(rev->delay);
									if (rev->delay>0)
									{
										Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你被GM禁言 %d 秒", rev->delay);                                               
										ScenesService::gm_logger->trace("玩家 %s 被禁言 %d 秒", pUser->name,rev->delay);
									}
									else
									{
										Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你被GM解除禁言，现在可以说话了");
										ScenesService::gm_logger->trace("玩家 %s 被解除禁言", pUser->name);
									}
								}
								break;
							case 2://关禁闭
								break;
							case 3://踢下线
								{
									OnQuit event(1);
									EventTable::instance().execute(*pUser, event);
									execute_script_event(pUser,"quit");

									pUser->save(Cmd::Record::LOGOUT_WRITEBACK);
									Cmd::Session::t_unregUser_SceneSession ret;
									ret.dwUserID=pUser->id;
									ret.dwSceneTempID=pUser->scene->tempid;
									ret.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
									sessionClient->sendCmd(&ret,sizeof(ret));
									Cmd::Scene::t_Unreg_LoginScene retgate;
									retgate.dwUserID = pUser->id;
									retgate.dwSceneTempID = pUser->scene->tempid;
									retgate.retcode = Cmd::Scene::UNREGUSER_RET_ERROR;
									pUser->gatetask->sendCmd(&retgate,sizeof(retgate));

									pUser->unreg();
								}
								break;
							case 4://警告
								{
									Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, rev->reason);
								}
								break;
							default:
								return true;
							}

							rev->level = pUser->charbase.level;
							rev->accid = pUser->charbase.accid;
							zRTime ct;
							rev->startTime = ct.sec();
							strncpy(rev->country, SceneManager::getInstance().getCountryNameByCountryID(pUser->charbase.country), MAX_NAMESIZE);
							ScenesService::getInstance().sendCmdToSuperServer(rev, sizeof(t_Punish_GmTool));
						}
						break;
					default:
						break;
				}
			}
			break;
		default:
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

/**
 * \brief 结束网络服务器
 *
 * 实现了纯虚函数<code>zService::final</code>
 *
 */
void ScenesService::final()
{

	SceneTimeTick::getInstance().final();
	SceneTimeTick::getInstance().join();
	SceneTimeTick::delInstance();
	SceneUserManager::getMe().removeAllUser();

	if(taskPool)
	{
		taskPool->final();
		SAFE_DELETE(taskPool);
	}
	if (sessionClient)
	{
		sessionClient->final();
		sessionClient->join();
		SAFE_DELETE(sessionClient);
	}

	if (recordClient)
	{
		recordClient->final();
		recordClient->join();
		SAFE_DELETE(recordClient);
	}

	SceneTaskManager::delInstance();

	SceneManager::delInstance();
	GlobalObjectIndex::delInstance();
	//SceneNpcManager::delInstance();
	NpcTrade::delInstance();
	MessageSystem::delInstance();

	unloadAllBM();
	zSubNetService::final();

	Zebra::logger->debug(__PRETTY_FUNCTION__);
	SAFE_DELETE(gm_logger);
}

/**
 * \brief 命令行参数
 *
 */
static struct argp_option scenes_options[] =
{
	{"daemon",'d',0,0,"Run service as daemon",0},
	{"log",'l',"level",0,"Log level",0},
	{"logfilename",'f',"filename",0,"Log file name",0},
	{"mysql",'y',"mysql",0,"MySQL[mysql://user:passwd@host:port/dbName]",0},
	{"ifname",'i',"ifname",0,"Local network device",0},
	{"server",'s',"ip",0,"Super server ip address",0},
	{"port",'p',"port",0,"Super server port number",0},
	{"info",'n',0,0,"show info",0},
	{0,0,0,0,0,0}
};

/**
 * \brief 命令行参数解析器
 *
 * \param key 参数缩写
 * \param arg 参数值
 * \param state 参数状态
 * \return 返回错误代码
 */
static error_t scenes_parse_opt(int key, char *arg, struct argp_state *state)
{
	switch (key)
	{
		case 'd':
			{
				Zebra::global["daemon"] = "true";
			}
			break;
		case 'p':
			{
				Zebra::global["port"]=arg;
			}
			break;
		case 's':
			{
				Zebra::global["server"]=arg;
			}
			break;
		case 'l':
			{
				Zebra::global["log"]=arg;
			}
			break;
		case 'f':
			{
				Zebra::global["logfilename"]=arg;
			}
			break;
		case 'y':
			{
				Zebra::global["mysql"]=arg;
			}
			break;
		case 'i':
			{
				Zebra::global["ifname"]=arg;
			}
			break;
		case 'n':
			{
				std::cout<<"编译选项:\t"<<DEBUG_STRING<<'\n';
				std::cout<<"GM模式开关:\t"<<Zebra::global["gm_mode"]<<'\n';
				if (!strcmp("_ALL_SUPER_GM", DEBUG_STRING) && Zebra::global["gm_mode"]=="true")
					std::cout<<"现在所有人都是\tdebug_mode\n";
				else
					std::cout<<"现在是\t\t正常模式\n";
				// exit(0);
			}
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

/**
 * \brief 简短描述信息
 *
 */
static char scenes_doc[] = "\nScenesServer\n" "\t场景服务器。";

/**
 * \brief 程序的版本信息
 *
 */
const char *argp_program_version = "Program version :\t" VERSION_STRING\
									"\nBuild version   :\t" _S(BUILD_STRING)\
									"\nBuild time      :\t" __DATE__ ", " __TIME__;

/**
 * \brief 读取配置文件
 *
 */
class SceneConfile:public zConfile
{
	bool parseYour(const xmlNodePtr node)
	{
		if(node)
		{
			xmlNodePtr child=parser.getChildNode(node,NULL);
			while(child)
			{
				parseNormal(child);
				child=parser.getNextNode(child,NULL);
			}
			return true;
		}
		else
			return false;
	}
};

/**
 * \brief 重新读取配置文件，为HUP信号的处理函数
 *
 */
void ScenesService::reloadConfig()
{
	reload=true;
	Zebra::logger->debug("%s", __PRETTY_FUNCTION__);
}

void ScenesService::checkAndReloadConfig()
{
	if(reload)
	{
		reload=false;
		Zebra::logger->debug("%s", __PRETTY_FUNCTION__);
		SceneConfile sc;
		sc.parse("ScenesServer");
		loadAllBM();
		NpcTrade::getInstance().init();
		MessageSystem::getInstance().init();
		FuBenMgr::getMe().init(); //副本新
		//定时存档配置
		if(atoi(Zebra::global["writebacktimer"].c_str()))
		{
			ScenesService::getInstance().writeBackTimer = atoi(Zebra::global["writebacktimer"].c_str());
		}
		else
		{
			ScenesService::getInstance().writeBackTimer = 600;
		}

		//soke 打怪经验倍数
		if(atoi(Zebra::global["double_exp"].c_str()) >0 )
		{
			ScenesService::getInstance().double_exp = atoi(Zebra::global["double_exp"].c_str());
		}
		else
		{
			ScenesService::getInstance().double_exp = 0;
		}

		if(Zebra::global["double_drop"] == "true")
		{
			ScenesService::getInstance().double_drop = true;
		}
		else
		{
			ScenesService::getInstance().double_drop = false;
		}

		if(atoi(Zebra::global["level_double_drop"].c_str()))
		{
			ScenesService::getInstance().levelDoubleDrop = atoi(Zebra::global["level_double_drop"].c_str());
		}
		else
		{
			ScenesService::getInstance().levelDoubleDrop = 0;
		}

		//指令检测开关
		if(Zebra::global["cmdswitch"] == "true")
		{
			zTCPTask::analysis._switch = true;
			zTCPClient::analysis._switch=true;
		}
		else
		{
			zTCPTask::analysis._switch = false;
			zTCPClient::analysis._switch=false;
		}
	}
}

/**
 * \brief 重新读取特征码文件
 *
 */
DWORD ScenesService::updateStampData()
{
	if (pStampData)
	{
		free(pStampData);
		pStampData = 0;
	}

	int f=0;
	if (Zebra::global["process_file"]!="")
		f = open(Zebra::global["process_file"].c_str(), O_RDONLY);
	else
		f = open("process.dat", O_RDONLY);
	if (f != -1)
	{
		pStampData = (Cmd::stChannelChatUserCmd *)malloc(zSocket::MAX_DATASIZE);
		bzero(pStampData, zSocket::MAX_DATASIZE);
		constructInPlace(pStampData);

		pStampData->dwType = Cmd::CHAT_TYPE_SYSTEM;
		pStampData->dwSysInfoType = Cmd::INFO_TYPE_GAME;
		strncpy(pStampData->pstrChat, "欢迎来到征途", MAX_CHATINFO-1);
		pStampData->dwFromID = read(f, (void *)(pStampData->tobject_array), zSocket::MAX_DATASIZE-sizeof(Cmd::stChannelChatUserCmd));
		close(f);

		pStampData->dwChannelID = atoi(Zebra::global["service_flag"].c_str()) & Cmd::Session::SERVICE_PROCESS;
		return pStampData->dwFromID;
	}
	return 0;
}



/**
 * \brief 主程序入口
 *
 * \param argc 参数个数
 * \param argv 参数列表
 * \return 运行结果
 */
int main(int argc, char **argv)
{

	Zebra::logger=new zLogger("ScenesServer");
	
	//设置缺省参数
	Zebra::global["logfilename"] = "/tmp/scenesserver.log";
	Zebra::global["cmdswitch"] = "true";
	Zebra::global["objlogfilename"] = "/tmp/objscenesserver.log";
	Zebra::global["sceneinfofile"] = "scenesinfo.xml";
	Zebra::global["npctradefile"] = "npctrade.xml";
	Zebra::global["questdir"] = "quest/";
	Zebra::global["messageConfigFile"] = "Config/Message.xml";
	Zebra::global["charinitinfoFile"] = "initcharinfo.xml";
	Zebra::global["magicrangefile"] = "magicrangefile.xml";
	Zebra::global["writebacktimer"] = "600";
	Zebra::global["mapdir"] = "map/";
	Zebra::global["tbldir"] = "tbl/";
	Zebra::global["shenJianConfig"] = "Config/ShenJianConfig.xml";
	Zebra::global["TouxianConfig"] = "Config/TouxianConfig.xml";
	Zebra::global["HuishouConfig"] = "Config/HuishouConfig.xml";
	Zebra::global["BieshuConfig"] = "Config/BieshuConfig.xml";
	Zebra::global["XiulianConfig"] = "Config/XiulianConfig.xml";
	Zebra::global["ChenghaoConfig"] = "Config/ChenghaoConfig.xml";
	Zebra::global["ZuoQiTuJianConfig"] = "Config/ZuoQiTuJianConfig.xml";
	Zebra::global["MagicBoxConfig"] = "Config/MagicBoxConfig.xml";
	Zebra::global["MagicBoxShiZhuangConfig"] = "Config/MagicBoxShiZhuangConfig.xml";
	Zebra::global["MagicBoxShiZhuangLevelConfig"] = "Config/MagicBoxShiZhuangLevelConfig.xml";
	Zebra::global["MagicBoxPiFengLevelConfig"] = "Config/MagicBoxPiFengLevelConfig.xml";
	Zebra::global["MagicBoxPiFengConfig"] = "Config/MagicBoxPiFengConfig.xml";
	Zebra::global["MagicBoxZuoqiConfig"] = "Config/MagicBoxZuoqiConfig.xml";
	Zebra::global["MagicBoxChiBangLevelConfig"] = "Config/MagicBoxChiBangLevelConfig.xml";
	Zebra::global["MagicBoxChiBangConfig"] = "Config/MagicBoxChiBangConfig.xml";
	Zebra::global["ErnvConfig"] = "Config/ErnvConfig.xml";
	Zebra::global["zhengtuzhuaconfig"] = "Config/ZhengTuZhuanConfig.xml";
	Zebra::global["12BeastsConfig"] = "Config/12BeastsConfig.xml";
	Zebra::global["ShengQiConfig"] = "Config/ShengQiConfig.xml";
	Zebra::global["JingMaiConfig"] = "Config/JingMaiConfig.xml";
	//soke monster exp  50
	Zebra::global["double_exp"] = "0";
	Zebra::global["mail_service"] = "on";
	Zebra::global["auction_service"] = "on";


	//解析配置文件参数
	SceneConfile sc;
	if (!sc.parse("ScenesServer"))
		return EXIT_FAILURE;

	//解析命令行参数
	zArg::getArg()->add(scenes_options, scenes_parse_opt, 0, scenes_doc);
	zArg::getArg()->parse(argc, argv);
	//Zebra::global.dump(std::cout);

	//设置日志级别
	Zebra::logger->setLevel(Zebra::global["log"]);
	//设置写本地日志文件
	if ("" != Zebra::global["logfilename"])
		Zebra::logger->addLocalFileLog(Zebra::global["logfilename"]);

	if(atoi(Zebra::global["writebacktimer"].c_str()))
	{
		ScenesService::getInstance().writeBackTimer = atoi(Zebra::global["writebacktimer"].c_str());
	}
	else
	{
		ScenesService::getInstance().writeBackTimer = 600;
	}
	//soke 打怪经验倍数
	if(atoi(Zebra::global["double_exp"].c_str()) >0 )
	{
		ScenesService::getInstance().double_exp = atoi(Zebra::global["double_exp"].c_str());
	}
	else
	{
		ScenesService::getInstance().double_exp = 0;
	}

	if(Zebra::global["double_drop"] == "true")
	{
		ScenesService::getInstance().double_drop = true;
	}
	else
	{
		ScenesService::getInstance().double_drop = false;
	}

	if(atoi(Zebra::global["level_double_drop"].c_str()))
	{
		ScenesService::getInstance().levelDoubleDrop = atoi(Zebra::global["level_double_drop"].c_str());
	}
	else
	{
		ScenesService::getInstance().levelDoubleDrop = 0;
	}

	//指令检测开关
	if(Zebra::global["cmdswitch"] == "true")
	{
		zTCPTask::analysis._switch = true;
		zTCPClient::analysis._switch=true;
	}
	else
	{
		zTCPTask::analysis._switch = false;
		zTCPClient::analysis._switch=false;
	}

	//是否以后台进程的方式运行
	if ("true" == Zebra::global["daemon"]) {
		Zebra::logger->info("Program will be run as a daemon");
		Zebra::logger->removeConsoleLog();
		daemon(1, 1);
	}

	ScenesService::getInstance().main();
	ScenesService::delInstance();
	SceneUserManager::destroyMe();
	return EXIT_SUCCESS;
}
