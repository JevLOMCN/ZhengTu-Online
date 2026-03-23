#include "SceneUser.h"
#include "SceneCommand.h"
#include "SceneTask.h"
#include "Command.h"
#include "Scene.h"
#include "zSceneEntryIndex.h"
#include "RecordCommand.h"
#include "RecordClient.h"
#include "Zebra.h"
#include "SceneNpc.h"
#include "zDatabaseManager.h"
#include "TimeTick.h"
#include "Chat.h"
#include <zlib.h>
#include "TeamManager.h"
#include "MessageSystem.h"
#include "SceneManager.h"
#include "Quest.h"
#include "SessionClient.h"
#include "QuestEvent.h"
#include "QuestTable.h"
#include "QuestAction.h"
#include "ScenesServer.h"
#include "GuardNpc.h"
#include "CartoonPet.h"
#include "script.h"
#include "script_func.h"
#include "CountryDare.h"
#include "CountryAlly.h"
#include "Dice.h"
#include "Union.h"
#include "FuBenMgr.h" //副本
#include "SessionClient.h"
#include "GlobalBox.h"
#include "MallTrade.h" //新商城
#include "fjconfig.h"
#include <math.h>
#include <string>

#define MAX_UZLIB_CHAR	(800 * 2048) //桃子修改压缩大小
DWORD SceneUser::Five_Relation[]= 
{
	FIVE_METAL,			/// 金
	FIVE_WOOD,			/// 木
	FIVE_SOIL, 			/// 土
	FIVE_WATER,			/// 水
	FIVE_FIRE,			/// 火
	FIVE_NONE			/// 无
};

class ObjectCompare:public UserObjectCompare 
{
	public:
		DWORD  dwObjectID;

		bool isIt(zObject *object)
		{
			if (object->data.dwObjectID == dwObjectID) return true;
			return false;
		}
};

/**
 * \brief 构造函数
 *
 */
SceneUser::SceneUser(const DWORD accid):SceneEntryPk(SceneEntry_Player), recycle_delay(0), _half_sec(0.3f), _one_sec(1), _five_sec(5), _ten_sec(10), _fiften_sec(15),_fif_sec(15), _one_min(60), _five_min(5*58-1), _15_min(15*60), _30_min(30*60), _writeback_timer(ScenesService::getInstance().writeBackTimer,ScenesService::getInstance().writeBackTimer),loginTime(),lastIncTime(0),horse(*this), packs(this), team(this), team_mode(Cmd::TEAM_NORMAL),tradeorder(this), lastCheckMessage(2000), lastMoveTime(0), moveFastCount(0)
{

#ifdef _TEST_DATA_LOG
	bzero(&chartest , sizeof(chartest));
#endif
	killedNpcNum = 0;
	txdykillnum = 0;//天下第一观战 死亡次数
	//反外挂处理 zm
	jinggaonum = 0;
	yichangnum = 0;
	yichangtime = 3;
	fanwaiguarongcuo = 0;
	hongtian_time1 = 0;//轰天 冷却计数
	step_state = 0;
	backOffing = 0;
	bzero(accelData , sizeof(accelData));
	bzero(sysSetting , sizeof(sysSetting));
	lastChangeCountryTime = 0;
	lastAddCountryTime = 0;
	temp_unsafety_state = 0;
	safety = 0;
	safety_setup = 0;
	Card_num = 0;
	Give_MatarialNum = 0;
	/*
	   zRTime ctv1;
	   tenSecondTime = ctv1;
	   oneSecondTime = ctv1;
	   oneMinuteTime = ctv1;
	   */
	notifyHMS = false;
	deathWaitTime = 0; 
	wdTire = 0;
	wdTirePer = 100;
	sitdownRestitute=false;
	npcdareflag	= false;
	npcdarePosX =0;
	npcdarePosY =0;
	npcdareCountryID=0;
	npcdareMapID=0;
	npcdareNotify = true;
	

	//战车处理
	jzzc = 2;
	//自行车比赛状态时间初始化
	bikeskill1 = 5;
	bikeskill3 = 5;
	bikeskill4 = 3;
	bikeskill5 = 10;
	bikeskill6 = 3;

	lastUseSkill = 0;
	this->accid=accid;
	gatetask=NULL;
	scene=NULL;
	bzero(&charbase, sizeof(charbase));
	bzero(&charstate, sizeof(charstate));
	//bzero(&pkState, sizeof(pkState));
	dwBodyID=0;
	dwBodyKind=0;
	dwLeftHandID=0;
	dwRightHandID=0;
	dwBodyColorSystem=zMisc::randBetween(0xFF000001,0xFFFFFFFE);
	dwBodyColorCustom=zMisc::randBetween(0xFF000001,0xFFFFFFFE);
	//	packs.initMe(this);
	skillStatusM.initMe(this);
	messageOrder = 0;
	systemOrder = 0;
	npc_dwNpcDataID=0;
	npc_dwNpcTempID=0;
	unReging=false;
	curTargetID = 0;
	curTargetType = 0;

	updateNotify=0; //hp,mp,sp 更新通知
	updateCount=0;  //更新记数

	guard = NULL;
	ridepet = 0;
	pet = 0;
	summon = 0;
	deathBackToMapID=0;
	totems.clear();
	vWars.clear();
	isQuiz = false;
	isDiplomat = false; //soke 外交官（停用）
	isCatcher = false;  //soke 巡捕（左上）
	isCatchex = false;  //soke 巡捕（左下）
	isCensor = false;   //soke 御史大夫（右上）
	isCensos = false;   //soke 御史大夫（右下）
	isYuanshuai = false;  //soke 元帅
	isZaixiang = false;   //soke 宰相

	//bzero(unionName, sizeof(unionName));
	//bzero(septName, sizeof(septName));

	bzero(caption, sizeof(caption));
	bzero(armyName, sizeof(armyName));

	bzero(&petData, sizeof(petData));
	king = false;
	emperor = false;
	unionMaster = false;
	septMaster = false;
	kingConsort = 0;
	dwArmyState = 0;
	touxianlevel = 0;
	isSendingMail = false;
	isGetingMailItem = false;
	BossGsStateTime = 0; 

	queryTime = 0;
	cartoon = 0;
	xcartoon = 0;
	xcartoon2 = 0;
	adoptedCartoon = 0;

	bzero(replyText, sizeof(replyText));

	myOverMan =0;
	saveGuard = false;
	saveAdopt = false;

	_5_sec_count = 0;
	bzero(&npcHoldData, sizeof(npcHoldData));

#ifdef _XWL_DEBUG
	processCheckTime = 0;
#else
	processCheckTime = zMisc::randBetween(5, 10);
#endif
#ifdef _XWL_DEBUG
	Zebra::logger->debug("%s(%u) 下次检查间隔 %u 分钟", name, id, processCheckTime);
#endif
	bzero(wait_gomap_name, sizeof(wait_gomap_name));

	bzero(wg_log, sizeof(wg_log));
	wg_log_len = 0;

	lastKiller = 0;
	dropTime = 0;

	miniGame = 0;
	box_item.targetO = NULL;
	box_item.defaultO = NULL;
	m_dLevelupTime = 0;     //soke 好友升级膜拜时间
	m_nCongratulateNum = 0; //soke 好友膜拜人数
}

SceneUser::~SceneUser()
{
	/*
	//把自己从个人聊天频道里删除
	Zebra::logger->debug("清理用户数据");
	if (team.IsTeamed()) team.setLeader(0); // 如果退出的时候还在组队状态则设置成组队退出。结算社会关系中的友好度
	ChannelM::getMe().removeUser(name);
	scene = NULL;
	*/
}

/**
 * \brief 删除用户时，进行相关的清理工作
 *
 *
 */
void SceneUser::destroy()
{
	//把自己从个人聊天频道里删除
	if (scene) scene->removeUser(this);
	SceneUserManager::getMe().removeUser(this);
	//	Zebra::logger->debug("%s(%x) really destroied", this->name, this);
	if (team.IsTeamed()) team.setLeader(0); // 如果退出的时候还在组队状态则设置成组队退出。结算社会关系中的友好度
	ChannelM::getMe().removeUser(name);
	if (guard) guard->reset();
}

void SceneUser::initCharBase(Scene *intoscene)
{
	using namespace Cmd;
	//设置坐标
	pos.x = charbase.x;
	pos.y = charbase.y;

	if (isNewCharBase())
	{
		Zebra::logger->info("初始化角色信息 %u, %u", charbase.accid, charbase.id);

		charbase.face = charbase.type;
		charbase.type =  getCharTypeByFace(charbase.face);
		charbase.fivetype = 5; //默认五行类型为无五行
		charbase.fivelevel = 1;//
		//设置性别
		/*
		   switch(charbase.type)
		   {
		   case PROFESSION_1:		//侠客
		   case PROFESSION_3:		//箭侠
		   case PROFESSION_5:		//天师
		   case PROFESSION_7:		//法师
		   charbase.sex = MALE;
		   break;
		   case PROFESSION_2:		//侠女
		   case PROFESSION_4:		//箭灵
		   case PROFESSION_6:		//美女
		   case PROFESSION_8:		//仙女
		   charbase.sex = FEMALE;
		   break;
		   case PROFESSION_NONE:	//无业
		   default:
		   Zebra::logger->error("错误的职业类型");
		   break;
		   }
		// */
		//在新手出生点随机查找坐标
		bool founded=false;
		if(SceneManager::getInstance().isNewZoneConfig())
		{
			founded=SceneManager::getInstance().randzPosNewZone(intoscene,pos);
			if(founded)
				Zebra::logger->info("查找新手出生点成功：%s, %u, %u", intoscene->name, pos.x, pos.y);
			else
				Zebra::logger->error("查找新手出生点失败：%s, %u, %u", intoscene->name, pos.x, pos.y);
		}
		if(!founded)
		{
			if (intoscene->randzPosByZoneType(ZoneTypeDef::ZONE_NEWBIE, pos))
				Zebra::logger->info("查找新手出生点成功：%s, %u, %u", intoscene->name, pos.x, pos.y);
			else
				Zebra::logger->error("查找新手出生点失败：%s, %u, %u", intoscene->name, pos.x, pos.y);
		}

		charbase.lucky = 10;

		charbase.bodyColor=zMisc::randBetween(0xFF000001,0xFFFFFFFE);
		//Zebra::logger->debug("bodyColor%u", charbase.bodyColor);
		//设置属性
		setupCharBase();

		//give gold object, will not delete
		zObject* gold = zObject::create(objectbm.get(665), 0);
		if(gold && packs.addObject(gold, true, Packages::MAIN_PACK)) {
			Zebra::logger->debug("初始化金钱成功!");
			zObject::logger(gold->createid,gold->data.qwThisID,gold->data.strName,gold->data.dwNum,gold->data.dwNum,1,0,NULL,this->id,this->name,"create",NULL,0,0);
		}else {
			Zebra::logger->fatal("初始化金钱失败!");
		}

		//默认系统设置
		using namespace Cmd;
		memset(sysSetting,0xff,sizeof(sysSetting));
		sysSetting[0] = 0;//pk模式
		//clear_state(sysSetting , Cmd::USER_SETTING_AUTOFINDPATH);//自动寻路
		//soke 默认显示玩家名
		//clear_state(sysSetting , Cmd::USER_SETTING_SHOW_PLAYERNAME);//显示玩家名字
		clear_state(sysSetting , Cmd::USER_SETTING_AUTO_KILL_SUMMON);//自动打怪
		clear_state(sysSetting , Cmd::USER_SETTING_SHOW_HELPTIP);    //soke 关闭显示帮助教程
		//通知session
		Cmd::Session::t_sysSetting_SceneSession send;
		strncpy((char *)send.name, name, MAX_NAMESIZE-1);
		bcopy(sysSetting, &send.sysSetting, sizeof(send.sysSetting));
		send.face = charbase.face;
		sessionClient->sendCmd(&send, sizeof(send));

		//通知网关
		Cmd::Scene::t_sysSetting_GateScene gate_send;
		bcopy(sysSetting, gate_send.sysSetting, sizeof(gate_send.sysSetting));
		gate_send.id=this->id;
		this->gatetask->sendCmd(&gate_send, sizeof(gate_send));

		chatColor[0] = 0xffffffff;
		chatColor[1] = 0xffffd100;//COLOR_ARGB(255,255,209,0);//国家频道
		chatColor[2] = 0xff4eaa00;//COLOR_ARGB(255,78,170,0);//地区频道
		chatColor[3] = 0xffff4818;//COLOR_ARGB(255,246,0,255);//密
		chatColor[4] = 0xff34ffbb;//COLOR_ARGB(255,52,255,187);//帮会频道
		chatColor[5] = 0xff98f417;//COLOR_ARGB(255,152,244,23);//队伍频道
		chatColor[6] = 0xffff627c;//COLOR_ARGB(255,255,98,124);//家族频道
		chatColor[7] = 0xff007fff;//COLOR_ARGB(255,0,127,255);//好友频道
		//chatColor[8] = 0xffff0fa0;//COLOR_ARGB(255,255,240,160);//世界频道
		//chatColor[9] = 0xffffb4ff;//COLOR_ARGB(255,255,180,255);//师门频道

		//通知客户端
		Cmd::stSystemSettingsUserCmd sendClient;
		bcopy(sysSetting, &sendClient.data.bySettings, sizeof(sendClient.data.bySettings));
		bcopy(chatColor, &sendClient.data.dwChatColor, sizeof(sendClient.data.dwChatColor));
		sendCmdToMe(&sendClient, sizeof(sendClient));

		charbase.hp = charstate.maxhp;
		charbase.mp = charstate.maxmp;
		charbase.sp = charstate.maxsp;

		charbase.goodness = Cmd::GOODNESS_2_1;
		charbase.points =3;
		//charbase.skillpoint = 20;//临时
		charbase.skillpoint = 1;//真是
		charbase.bitmask |= CHARBASE_OK;
		charbase.createtime=time(NULL);

		//charbase.petPoint = 86400;//拥有替身宝宝就送24小时修炼时间
		/*
		//临时增加测试用
		switch(charbase.type)
		{
		case PROFESSION_1:		//侠客
		{
		for(int i = 1 ; i < 7 ; i ++)
		{
		zSkill *skill = zSkill::create(this , i , 1) ;
		if(!skill)
		{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "无法加载初始技能");
		}
		}
		}
		break;
		default:
		break;
		}
		*/		
#ifdef _TEST_DATA_LOG
		writeCharTest(Cmd::Record::NEWCHAR_WRITEBACK);
#endif // _TEST_DATA_LOG测试数据
		bzero(this->charbase.tiretime, 36);
	}
		else
		{
			setupCharBase();
#ifdef _TEST_DATA_LOG
			Cmd::Record::t_Read_CharTest_SceneRecord ret;
			strncpy(ret.name , name , MAX_NAMESIZE);
			ret.level = charbase.level;
			recordClient->sendCmd(&ret, sizeof(ret));
#endif // _TEST_DATA_LOG测试数据
		}
		//skillStatusM.processPassiveness();

		initTire();
		//初始化当天答题次数
		initAnswerCount();

		//soke 初始化当天幸运丹次数
		initLuckyCount();

		//soke 初始化每日登陆领取次数
		initLoginCount();

		//soke 初始化每日svip领取次数
		initSvipCount();

		//soke 初始化每日太庙完成次数
		initTaimCount();
		
		//soke 初始化每日副本激活次数
		initFubenCount();

		//soke 初始化每日自行车激活次数
		initBikeCount();


		//soke 初始化榜单称号判定
		//initChshowCount();
	}

	/**
	 * \brief 计算复活虚弱状态时的人物属性
	 *
	 * 由setupCharBase调用
	 *
	 */
	/*void SceneUser::calReliveWeaknessProperty(bool enter)
	  {
	  static DWORD wdCon = 0;
	  static DWORD wdStr = 0;
	  static DWORD wdDex = 0;
	  static DWORD wdInt = 0;
	  static DWORD wdMen = 0;

	  static bool first = true;

#ifdef _ZJW_DEBUG
Zebra::logger->debug("property(%d,%d,%d,%d,%d)", wdCon, wdStr, wdDex, wdInt, wdMen);
#endif	
if (enter)
{
if (first)
{
wdCon = this->charstate.wdCon;
wdStr = this->charstate.wdStr;
wdDex = this->charstate.wdDex;
wdInt = this->charstate.wdInt;
wdMen = this->charstate.wdMen;
}

if (this->charbase.reliveWeakTime > 0 & first)
{

this->charstate.wdCon = (DWORD)(this->charbase.wdCon * 0.6);
this->charstate.wdStr = (DWORD)(this->charbase.wdStr * 0.6);
this->charstate.wdDex = (DWORD)(this->charbase.wdDex * 0.6);
this->charstate.wdInt = (DWORD)(this->charbase.wdInt * 0.6);
this->charstate.wdMen = (DWORD)(this->charbase.wdMen * 0.6);
#ifdef _ZJW_DEBUG
Zebra::logger->debug("计算复活虚弱状态的人物属性值:property(%d,%d,%d,%d,%d)", this->charstate.wdCon, this->charstate.wdStr, 
this->charstate.wdDex, this->charstate.wdInt, this->charstate.wdMen);
#endif	

first = false;
}
}
else if (!first)
{
#ifdef _ZJW_DEBUG
Zebra::logger->debug("复活虚弱状态，恢复属性:property(%d,%d,%d,%d,%d)", wdCon, wdStr, wdDex, wdInt, wdMen);
#endif	

this->charstate.wdCon = wdCon;
this->charstate.wdStr = wdStr;
this->charstate.wdDex = wdDex;
this->charstate.wdInt = wdInt;
this->charstate.wdMen = wdMen;
first = true;
}
}*/

void SceneUser::calReliveWeaknessProperty(bool enter)
{
#ifdef _ZJW_DEBUG
	Zebra::logger->debug("计算复活虚弱状态的人物属性值:property(%d,%d,%d,%d,%d)", this->charstate.wdCon, this->charstate.wdStr, 
			this->charstate.wdDex, this->charstate.wdInt, this->charstate.wdMen);
#endif	

	if (this->charbase.reliveWeakTime > 0)
	{

		this->charstate.wdCon = (DWORD)(this->charstate.wdCon * 0.6);
		this->charstate.wdStr = (DWORD)(this->charstate.wdStr * 0.6);
		this->charstate.wdDex = (DWORD)(this->charstate.wdDex * 0.6);
		this->charstate.wdInt = (DWORD)(this->charstate.wdInt * 0.6);
		this->charstate.wdMen = (DWORD)(this->charstate.wdMen * 0.6);
#ifdef _ZJW_DEBUG
		Zebra::logger->debug("计算复活虚弱状态的人物属性值:property(%d,%d,%d,%d,%d)", this->charstate.wdCon, this->charstate.wdStr, 
				this->charstate.wdDex, this->charstate.wdInt, this->charstate.wdMen);
#endif	
	}
}
/**
 * \brief 设置角色属性信息
 *
 * 包括裸身和装备以及各种状态影响的角色属性数值
 *
 */
void SceneUser::setupCharBase(bool lock)
{
	if((charbase.goodness & 0XFF000000) == 0XFF000000)
	{
		DWORD temp=this->charbase.goodness & 0X00FF0000;
		this->charbase.goodness &= 0X0000FFFF;
		this->pkState.addProtect(this,temp);
	}
	bzero(&charstate,sizeof(charstate));
	//	this->usm->refresh();
	skillStatusM.processPassiveness();

	charstate.resumehp = BASEDATA_M_RESUMEHP;
	charstate.resumemp = BASEDATA_M_RESUMEMP;
	charstate.resumesp = BASEDATA_M_RESUMESP;

	charstate.attackspeed = 640;
	//soke GM速度加快
	if( (getPriv() & Gm::debug_mode) || (getPriv() & Gm::super_mode) || (getPriv() & Gm::captain_mode)  )
	{
		charstate.movespeed = 340;
	}
	else
	{
		charstate.movespeed = 640;
	}
	

	// // //楚国皇城战胜利 醉梦 恋恋
	// // if(!this->issetUState(2000))
	// // {
	// 	if(charbase.country == 12)
	// 	{
	// 		this->sendSkill(5000, 1);
	// 	}
	// // }

	//soke PK善恶保护
	if(charbase.pkaddition > 1800 /*30*60*/)
	{
		this->setUState(Cmd::USTATE_PK);
	}

	// doLoongEffWhenLogin();
    
	if(charbase.charvip > 2690 ) //soke VIP20玩家特效
	{
		this->setUState(Cmd::USTATE_VIP20);
		showCurrentEffect(Cmd::USTATE_VIP19, false);
	}
	else if(charbase.charvip > 2190 && charbase.charvip<=2690 ) //soke VIP19玩家特效
	{
		this->setUState(Cmd::USTATE_VIP19);
		showCurrentEffect(Cmd::USTATE_VIP18, false);
	}
	else if(charbase.charvip > 1690 && charbase.charvip<=2190 ) //soke VIP18玩家特效
	{
		this->setUState(Cmd::USTATE_VIP18);
		showCurrentEffect(Cmd::USTATE_VIP17, false);
	}
	else if(charbase.charvip > 1190 && charbase.charvip<=1690 ) //soke VIP17玩家特效
	{
		this->setUState(Cmd::USTATE_VIP17);
		showCurrentEffect(Cmd::USTATE_VIP16, false);
	}
	//690点V15 691点V16
	else if(charbase.charvip > 690 && charbase.charvip<=1190 ) //soke VIP16玩家特效
	{
		this->setUState(Cmd::USTATE_VIP16);
		showCurrentEffect(Cmd::USTATE_VIP15, false);
	}
	else if(charbase.charvip > 190 && charbase.charvip<=690) //soke VIP15玩家特效
	{
		this->setUState(Cmd::USTATE_VIP15);
		showCurrentEffect(Cmd::USTATE_VIP14, false);
	}
	else if(charbase.charvip > 170 && charbase.charvip<=190) //soke VIP14玩家特效
	{
		this->setUState(Cmd::USTATE_VIP14);
		showCurrentEffect(Cmd::USTATE_VIP13, false);
	}
	else if(charbase.charvip > 150 && charbase.charvip<=170) //soke VIP13玩家特效
	{
		this->setUState(Cmd::USTATE_VIP13);
		showCurrentEffect(Cmd::USTATE_VIP12, false);
	}
	else if(charbase.charvip > 130 && charbase.charvip<=150) //soke VIP12玩家特效
	{
		this->setUState(Cmd::USTATE_VIP12);
		showCurrentEffect(Cmd::USTATE_VIP11, false);
	}
	else if(charbase.charvip > 110 && charbase.charvip<=130) //soke VIP11玩家特效
	{
		this->setUState(Cmd::USTATE_VIP11);
		showCurrentEffect(Cmd::USTATE_VIP10, false);
	}
	else if(charbase.charvip > 90 && charbase.charvip<=110) //soke VIP10玩家特效
	{
		this->setUState(Cmd::USTATE_VIP10);
		showCurrentEffect(Cmd::USTATE_VIP9, false);
	}
	else if(charbase.charvip > 80 && charbase.charvip<=90) //soke VIP9玩家特效
	{
		this->setUState(Cmd::USTATE_VIP9);
		showCurrentEffect(Cmd::USTATE_VIP8, false);
	}
	else if(charbase.charvip > 70 && charbase.charvip<=80) //soke VIP8玩家特效
	{
		this->setUState(Cmd::USTATE_VIP8);
		showCurrentEffect(Cmd::USTATE_VIP7, false);
	}
	else if(charbase.charvip > 60 && charbase.charvip<=70) //soke VIP7玩家特效
	{
		this->setUState(Cmd::USTATE_VIP7);
		showCurrentEffect(Cmd::USTATE_VIP6, false);
	}
	else if(charbase.charvip > 50 && charbase.charvip<=60) //soke VIP6玩家特效
	{
		this->setUState(Cmd::USTATE_VIP6);
		showCurrentEffect(Cmd::USTATE_VIP5, false);
	}
	else if(charbase.charvip > 40 && charbase.charvip<=50) //soke VIP5玩家特效
	{
		this->setUState(Cmd::USTATE_VIP5);
		showCurrentEffect(Cmd::USTATE_VIP4, false);
	}
	else if(charbase.charvip > 30 && charbase.charvip<=40) //soke VIP4玩家特效
	{
		this->setUState(Cmd::USTATE_VIP4);
		showCurrentEffect(Cmd::USTATE_VIP3, false);
	}
	else if(charbase.charvip > 20 && charbase.charvip<=30) //soke VIP3玩家特效
	{
		this->setUState(Cmd::USTATE_VIP3);
		showCurrentEffect(Cmd::USTATE_VIP2, false);
	}
	else if(charbase.charvip > 10 && charbase.charvip<=20) //soke VIP2玩家特效
	{
		this->setUState(Cmd::USTATE_VIP2);
		showCurrentEffect(Cmd::USTATE_VIP1, false);
	}
	else if(charbase.charvip > 0 && charbase.charvip<=10) //soke VIP1玩家特效
	{
		this->setUState(Cmd::USTATE_VIP1);
	}
	this->sendtoSelectedVIPState();


	//直接写 

	int duanweilevel = charbase.duanweiexp/1000;
	//算一下 目前是几级 /1000就行了

	//这样就可以了 根据 段位等级 给玩家发放对应的状态 接下来 只需要
	switch (duanweilevel)
	{
	case 1:
	{
		showCurrentEffect(Cmd::USTATE_DUANWEI1, true);
	}
	break;
	case 2:
	{
		showCurrentEffect(Cmd::USTATE_DUANWEI2, true);
		showCurrentEffect(Cmd::USTATE_DUANWEI1, false);
	}
	break;
	case 3:
	{
		showCurrentEffect(Cmd::USTATE_DUANWEI3, true);
		showCurrentEffect(Cmd::USTATE_DUANWEI2, false);
	}
	break;
	case 4:
	{
		showCurrentEffect(Cmd::USTATE_DUANWEI4, true);
		showCurrentEffect(Cmd::USTATE_DUANWEI3, false);
	}
	break;
	case 5:
	{
		showCurrentEffect(Cmd::USTATE_DUANWEI5, true);
		showCurrentEffect(Cmd::USTATE_DUANWEI4, false);
	}
	break;
	case 6:
	{
		showCurrentEffect(Cmd::USTATE_DUANWEI6, true);
		showCurrentEffect(Cmd::USTATE_DUANWEI5, false);
	}
	break;
	case 7:
	{
		showCurrentEffect(Cmd::USTATE_DUANWEI7, true);
		showCurrentEffect(Cmd::USTATE_DUANWEI6, false);
	}
	break;
	case 8:
	{
		showCurrentEffect(Cmd::USTATE_DUANWEI8, true);
		showCurrentEffect(Cmd::USTATE_DUANWEI7, false);
	}
	break;
	case 9:
	{
		showCurrentEffect(Cmd::USTATE_DUANWEI9, true);
		showCurrentEffect(Cmd::USTATE_DUANWEI8, false);
	}
	break;
	case 10:
	{
		showCurrentEffect(Cmd::USTATE_DUANWEI10, true);
		showCurrentEffect(Cmd::USTATE_DUANWEI9, false);
	}
	break;
	}

	//soke GM特效
	if(charbase.id == 1)
	{
		this->setUState(Cmd::USTATE_PLANTING);
	}
	
	//soke 9朵花特效
	if (charbase.flower1time>0)
	{
		this->setUState(Cmd::USTATE_FLOWER_9);
	}
	//soke 99朵花特效
	if (charbase.flower2time>0)
	{
		charbase.flower1time = 0;
		this->setUState(Cmd::USTATE_FLOWER_99);
	}
	//soke 999朵花特效
	if (charbase.flower3time>0)
	{
		charbase.flower1time = 0;
		charbase.flower2time = 0;
		this->setUState(Cmd::USTATE_FLOWER_999);
		this->setUState(Cmd::USTATE_USER_MEILI);
	}

	//soke 9朵蓝色妖姬特效
	if (charbase.flower4time>0)
	{
		charbase.flower1time = 0;
		charbase.flower2time = 0;
		charbase.flower2time = 0;
		this->setUState(Cmd::USTATE_FLOWERL_9);
	}
	//soke 99朵蓝色妖姬特效
	if (charbase.flower5time>0)
	{
		charbase.flower1time = 0;
		charbase.flower2time = 0;
		charbase.flower3time = 0;
		charbase.flower4time = 0;
		this->setUState(Cmd::USTATE_FLOWERL_99);
	}
	//soke 999朵蓝色妖姬特效
	if (charbase.flower6time>0)
	{
		charbase.flower1time = 0;
		charbase.flower2time = 0;
		charbase.flower3time = 0;
		charbase.flower4time = 0;
		charbase.flower5time = 0;
		this->setUState(Cmd::USTATE_FLOWERL_999);
		this->setUState(Cmd::USTATE_USER_RENQI);
	}

	if (charbase.trainTime)
		this->setUState(Cmd::USTATE_DAOJISHI);


	//soke 增加魂魄属性
	charstate.wdCon = charbase.wdCon + packs.equip.getEquips().get_con() + packs.equip.getEquips().get_xcon() + skillValue.upattribute + skillValue.upcon + skillValue.x_upcon;
	charstate.wdStr = charbase.wdStr + packs.equip.getEquips().get_str() + packs.equip.getEquips().get_xstr() + skillValue.upattribute + skillValue.x_upstr; //soke 主动增加攻击
	charstate.wdDex = charbase.wdDex + packs.equip.getEquips().get_dex() + packs.equip.getEquips().get_xdex() + skillValue.upattribute + skillValue.updex + skillValue.x_updex;
	charstate.wdInt = charbase.wdInt + packs.equip.getEquips().get_inte() + packs.equip.getEquips().get_xinte() + skillValue.upattribute + skillValue.upint + skillValue.x_upint;
	charstate.wdMen = (DWORD)((charbase.wdMen + packs.equip.getEquips().get_spi() + packs.equip.getEquips().get_xspi() + skillValue.upattribute + skillValue.x_upmen)*(1+skillValue.addmenp/100.0f));

	//soke 突破属性限制
	if (this->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER))
	{//虎魄附体，所有属性加成50%
		charstate.wdCon = (WORD)(charstate.wdCon + charstate.wdCon*0.2);
		charstate.wdStr = (WORD)(charstate.wdStr + charstate.wdStr*0.2);
		charstate.wdDex = (WORD)(charstate.wdDex + charstate.wdDex*0.2);
		charstate.wdInt = (WORD)(charstate.wdInt + charstate.wdInt*0.2);
		charstate.wdMen = (WORD)(charstate.wdMen + charstate.wdMen*0.2);
	}

	charstate.wdCon = (WORD)(charstate.wdCon + charbase.wdCon*(skillValue.upattribcon/100.0f));
	charstate.wdStr = (WORD)(charstate.wdStr + charbase.wdStr*(skillValue.upattribstr/100.0f));
	charstate.wdDex = (WORD)(charstate.wdDex + charbase.wdDex*(skillValue.upattribdex/100.0f));
	charstate.wdInt = (WORD)(charstate.wdInt + charbase.wdInt*(skillValue.upattribint/100.0f));
	charstate.wdMen = (WORD)(charstate.wdMen + charbase.wdMen*(skillValue.upattribmen/100.0f));

	charstate.wdStr = (WORD)(charstate.wdStr*(1-skillValue.dpstrdex/100.0f));
	charstate.wdDex = (WORD)(charstate.wdDex*(1-skillValue.dpstrdex/100.0f));
	charstate.wdInt = (WORD)(charstate.wdInt*(1-skillValue.dpintmen/100.0f));
	charstate.wdMen = (WORD)(charstate.wdMen*(1-skillValue.dpintmen/100.0f));

	/* if (this->issetUState(Cmd::USTATE_USE_HORSE_ONE))
	{
		horse.uesHorseone = true;
		horse.getPkData();
	} */
	
	//高级战马
	if (horse.pkData.str) charstate.wdStr += horse.pkData.str;
	if (horse.pkData.intel) charstate.wdInt += horse.pkData.intel;
	if (horse.pkData.dex) charstate.wdDex += horse.pkData.dex;
	if (horse.pkData.men) charstate.wdMen += horse.pkData.men;
	if (horse.pkData.con) charstate.wdCon += horse.pkData.con;

	charstate.wdStr += (DWORD)(charbase.wdStr * (skillValue.upstr/100.0f));
	if (skillValue.upallattrib>0)
	{
		charstate.wdCon = (WORD)(charstate.wdCon + skillValue.upallattrib);
		charstate.wdStr = (WORD)(charstate.wdStr + skillValue.upallattrib);
		charstate.wdDex = (WORD)(charstate.wdDex + skillValue.upallattrib);
		charstate.wdInt = (WORD)(charstate.wdInt + skillValue.upallattrib);
		charstate.wdMen = (WORD)(charstate.wdMen + skillValue.upallattrib);
	}
	if (skillValue.dnallattrib>0)
	{
		if (charstate.wdCon >= (WORD)skillValue.dnallattrib)
			charstate.wdCon = (WORD)(charstate.wdCon - (WORD)skillValue.dnallattrib);
		else
			charstate.wdCon =0;
		if (charstate.wdStr >= (WORD)skillValue.dnallattrib)
			charstate.wdStr = (WORD)(charstate.wdStr - (WORD)skillValue.dnallattrib);
		else
			charstate.wdStr =0;
		if (charstate.wdDex >= (WORD)skillValue.dnallattrib)
			charstate.wdDex = (WORD)(charstate.wdDex - (WORD)skillValue.dnallattrib);
		else
			charstate.wdDex =0;
		if (charstate.wdInt >= (WORD)skillValue.dnallattrib)
			charstate.wdInt = (WORD)(charstate.wdInt - (WORD)skillValue.dnallattrib);
		else
			charstate.wdInt =0;
		if (charstate.wdMen >= (WORD)skillValue.dnallattrib)
			charstate.wdMen = (WORD)(charstate.wdMen - (WORD)skillValue.dnallattrib);
		else
			charstate.wdMen =0;
	}

	if (skillValue.dpallattrib>0)
	{
		charstate.wdCon = (DWORD)(charstate.wdCon * (1-(skillValue.dpallattrib/100.0f)));
		charstate.wdStr = (DWORD)(charstate.wdStr * (1-(skillValue.dpallattrib/100.0f)));
		charstate.wdDex = (DWORD)(charstate.wdDex * (1-(skillValue.dpallattrib/100.0f)));
		charstate.wdInt = (DWORD)(charstate.wdInt * (1-(skillValue.dpallattrib/100.0f)));
		charstate.wdMen = (DWORD)(charstate.wdMen * (1-(skillValue.dpallattrib/100.0f)));
	}

	if (this->charbase.reliveWeakTime > 0)
	{// 只要是复活虚弱状态，则把五个属性降为60%，不因任何原因而改变
		#ifdef _ZJW_DEBUG
		Zebra::logger->debug("复活虚弱状态剩余时间:%d", (this->charbase.reliveWeakTime-(SceneTimeTick::currentTime.sec())%10000));		
		#endif		
		this->calReliveWeaknessProperty(true);
	}

	switch(charbase.type)
	{
		case	PROFESSION_1: // 男
			{
				DWORD tmaxhp = packs.equip.getEquips().get_m1axhp() + packs.equip.getEquips().get_m2axhp() + packs.equip.getEquips().get_m3axhp()+ packs.equip.getEquips().get_m4axhp()+ packs.equip.getEquips().get_m5axhp()+ packs.equip.getEquips().get_m6axhp()
					+ packs.equip.getEquips().get_m7axhp()+ packs.equip.getEquips().get_m8axhp()+ packs.equip.getEquips().get_m9axhp()+ packs.equip.getEquips().get_m24axhp();
				// 最大生命值
				charstate.maxhp=DWORD(BASEDATA_M_HP+LEVELUP_HP_N*charbase.level + charstate.wdStr*2 + charstate.wdCon*15 +  tmaxhp);
				// 最大法术值
				charstate.maxmp=DWORD(BASEDATA_M_MP+LEVELUP_MP_N*charbase.level + charstate.wdInt*1 + charstate.wdMen*3);
				// 最大体力值
				charstate.maxsp=DWORD(BASEDATA_M_SP+LEVELUP_SP_N*charbase.level);

				DWORD tpdamage = packs.equip.getEquips().get_p1damage() + packs.equip.getEquips().get_p2damage() + packs.equip.getEquips().get_p3damage() + packs.equip.getEquips().get_p4damage() + packs.equip.getEquips().get_p5damage() + packs.equip.getEquips().get_p6damage()
					+ packs.equip.getEquips().get_p7damage() + packs.equip.getEquips().get_p8damage() + packs.equip.getEquips().get_p9damage()+ packs.equip.getEquips().get_p24damage();
				// 最小物理攻击力
				charstate.pdamage=DWORD(BASEDATA_M_PDAMAGE+LEVELUP_PDAMAGE_N*charbase.level + charstate.wdStr*1 + tpdamage );
				
				DWORD tmdamage = packs.equip.getEquips().get_m1damage() + packs.equip.getEquips().get_m2damage() + packs.equip.getEquips().get_m3damage() + packs.equip.getEquips().get_m4damage() + packs.equip.getEquips().get_m5damage() + packs.equip.getEquips().get_m6damage()
					+ packs.equip.getEquips().get_m7damage() + packs.equip.getEquips().get_m8damage() + packs.equip.getEquips().get_m9damage() + packs.equip.getEquips().get_m24damage();
				// 最小魔法攻击力
				charstate.mdamage=DWORD(BASEDATA_M_MDAMAGE+LEVELUP_MDAMAGE_N*charbase.level + charstate.wdInt*1.1f + tmdamage);
				
				DWORD tpdefence = packs.equip.getEquips().get_p1defence() + packs.equip.getEquips().get_p2defence() + packs.equip.getEquips().get_p3defence() + packs.equip.getEquips().get_p4defence() + packs.equip.getEquips().get_p5defence() + packs.equip.getEquips().get_p6defence()
					+ packs.equip.getEquips().get_p7defence() + packs.equip.getEquips().get_p8defence() + packs.equip.getEquips().get_p9defence()+ packs.equip.getEquips().get_p24defence();
				// 物理防御力
				charstate.pdefence=DWORD(BASEDATA_M_PDEFENCE+LEVELUP_PDEFENCE_N*charbase.level + charstate.wdStr*0.3f + charstate.wdDex*1.5f + charstate.wdCon*0.4f + tpdefence);
				
				DWORD tmdefence = packs.equip.getEquips().get_m1defence() + packs.equip.getEquips().get_m2defence()+ packs.equip.getEquips().get_m3defence()+ packs.equip.getEquips().get_m4defence()+ packs.equip.getEquips().get_m5defence()+ packs.equip.getEquips().get_m6defence()
					+ packs.equip.getEquips().get_m7defence()+ packs.equip.getEquips().get_m8defence()+ packs.equip.getEquips().get_m9defence()+ packs.equip.getEquips().get_m24defence();
				// 魔法防御力
				charstate.mdefence=DWORD(BASEDATA_M_MDEFENCE+LEVELUP_MDEFENCE_N*charbase.level + charstate.wdInt*0.3f + charstate.wdDex*1.5f + charstate.wdMen*0.8f + charstate.wdCon*0.6f + tmdefence);
				// 重击
				charstate.bang = (WORD)(BASEDATA_M_BANG + charstate.wdCon*0.002f);

				// 标准物理攻击力
				charstate.stdpdamage=DWORD(BASEDATA_M_PDAMAGE+LEVELUP_PDAMAGE_N*charbase.level + charbase.wdStr*1);
				// 标准法术攻击力
				charstate.stdmdamage=DWORD(BASEDATA_M_MDAMAGE+LEVELUP_MDAMAGE_N*charbase.level + charbase.wdInt*1.1f);
				// 标准物理防御力
				charstate.stdpdefence=DWORD(BASEDATA_M_PDEFENCE+LEVELUP_PDEFENCE_N*charbase.level + charbase.wdStr*0.3f + charbase.wdDex*1.5f + charbase.wdCon*0.4f);
				// 标准法术防御力
				charstate.stdmdefence=DWORD(BASEDATA_M_MDEFENCE+LEVELUP_MDEFENCE_N*charbase.level + charbase.wdInt*0.3f + charbase.wdDex*1.5f + charbase.wdMen*0.8f + charbase.wdCon*0.6f);
				// 标准重击率
				charstate.stdbang = (WORD)(BASEDATA_M_BANG + charbase.wdCon*0.002f);
			}
			break;
		case	PROFESSION_2: // 女
			{

				DWORD tmaxhp = packs.equip.getEquips().get_m1axhp() + packs.equip.getEquips().get_m2axhp() + packs.equip.getEquips().get_m3axhp()+ packs.equip.getEquips().get_m4axhp()+ packs.equip.getEquips().get_m5axhp()+ packs.equip.getEquips().get_m6axhp()
					         + packs.equip.getEquips().get_m7axhp()+ packs.equip.getEquips().get_m8axhp()+ packs.equip.getEquips().get_m9axhp()+ packs.equip.getEquips().get_m24axhp();
				// 最大生命值
				charstate.maxhp=DWORD(BASEDATA_F_HP+LEVELUP_HP_N*charbase.level + charstate.wdStr*2 + charstate.wdCon*15 + tmaxhp );
				// 最大法术值
				charstate.maxmp=DWORD(BASEDATA_F_MP+LEVELUP_MP_N*charbase.level + charstate.wdInt*1 + charstate.wdMen*3);
				// 最大体力值
				charstate.maxsp=DWORD(BASEDATA_F_SP+LEVELUP_SP_N*charbase.level);

                 DWORD tpdamage = packs.equip.getEquips().get_p1damage() + packs.equip.getEquips().get_p2damage() + packs.equip.getEquips().get_p3damage() + packs.equip.getEquips().get_p4damage() + packs.equip.getEquips().get_p5damage() + packs.equip.getEquips().get_p6damage()
					            + packs.equip.getEquips().get_p7damage() + packs.equip.getEquips().get_p8damage() + packs.equip.getEquips().get_p9damage()+ packs.equip.getEquips().get_p24damage();
				// 最小物理攻击力
				charstate.pdamage=DWORD(BASEDATA_F_PDAMAGE+LEVELUP_PDAMAGE_N*charbase.level + charstate.wdStr*1 + tpdamage );
				
				
				DWORD tmdamage = packs.equip.getEquips().get_m1damage() + packs.equip.getEquips().get_m2damage() + packs.equip.getEquips().get_m3damage() + packs.equip.getEquips().get_m4damage() + packs.equip.getEquips().get_m5damage() + packs.equip.getEquips().get_m6damage()
					           + packs.equip.getEquips().get_m7damage() + packs.equip.getEquips().get_m8damage() + packs.equip.getEquips().get_m9damage()+ packs.equip.getEquips().get_m24damage();
				// 最小魔法攻击力
				charstate.mdamage=DWORD(BASEDATA_F_MDAMAGE+LEVELUP_MDAMAGE_N*charbase.level + charstate.wdInt*1.1f + tmdamage);
				
				DWORD tpdefence = packs.equip.getEquips().get_p1defence() + packs.equip.getEquips().get_p2defence() + packs.equip.getEquips().get_p3defence() + packs.equip.getEquips().get_p4defence() + packs.equip.getEquips().get_p5defence() + packs.equip.getEquips().get_p6defence()
					            + packs.equip.getEquips().get_p7defence() + packs.equip.getEquips().get_p8defence() + packs.equip.getEquips().get_p9defence()+ packs.equip.getEquips().get_p24defence();
				// 物理防御力
				charstate.pdefence=DWORD(BASEDATA_F_PDEFENCE+LEVELUP_PDEFENCE_N*charbase.level + charstate.wdStr*0.3f + charstate.wdDex*1.5f + charstate.wdCon*0.4f + tpdefence );
				
				DWORD tmdefence = packs.equip.getEquips().get_m1defence() + packs.equip.getEquips().get_m2defence()+ packs.equip.getEquips().get_m3defence()+ packs.equip.getEquips().get_m4defence()+ packs.equip.getEquips().get_m5defence()+ packs.equip.getEquips().get_m6defence()
					            + packs.equip.getEquips().get_m7defence()+ packs.equip.getEquips().get_m8defence()+ packs.equip.getEquips().get_m9defence()+ packs.equip.getEquips().get_m24defence();
				// 魔法防御力
				charstate.mdefence=DWORD(BASEDATA_F_MDEFENCE+LEVELUP_MDEFENCE_N*charbase.level + charstate.wdInt*0.3f + charstate.wdDex*1.5f + charstate.wdMen*0.8f + charstate.wdCon*0.6f + tmdefence);
				// 重击
				charstate.bang = (WORD)(BASEDATA_F_BANG + charstate.wdCon*0.002f);

				// 标准物理攻击力
				charstate.stdpdamage=DWORD(BASEDATA_F_PDAMAGE+LEVELUP_PDAMAGE_N*charbase.level + charbase.wdStr*1);
				// 标准法术攻击力
				charstate.stdmdamage=DWORD(BASEDATA_F_MDAMAGE+LEVELUP_MDAMAGE_N*charbase.level + charbase.wdInt*1.1f);
				// 标准物理防御力
				charstate.stdpdefence=DWORD(BASEDATA_F_PDEFENCE+LEVELUP_PDEFENCE_N*charbase.level + charbase.wdStr*0.3f + charbase.wdDex*1.5f + charbase.wdCon*0.4f);
				// 标准法术防御力
				charstate.stdmdefence=DWORD(BASEDATA_F_MDEFENCE+LEVELUP_MDEFENCE_N*charbase.level + charbase.wdInt*0.3f + charbase.wdDex*1.5f + charbase.wdMen*0.8f + charbase.wdCon*0.6f);
				// 标准重击率
				charstate.stdbang = (WORD)(BASEDATA_F_BANG + charbase.wdCon*0.002f);
			}
			break;
        	}
        	{
		        //soke 绿装15星加成效果
	            if (this->issetUState(Cmd::USTATE_ULTRA_EQUIPMENT ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(12/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(12/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(12/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(12/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(12/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(12/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(12/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(12/100.0f));                 //最大法术值
	            }
				//soke 紫装效果加成
				else if (this->issetUState(Cmd::USTATE_REDHOT ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(12/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(12/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(12/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(12/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(12/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(12/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(12/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(12/100.0f));                 //最大法术值
	            }
				//soke VIP1效果加成
				if (this->issetUState(Cmd::USTATE_VIP1 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(1/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(1/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(1/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(1/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(1/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(1/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(1/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(1/100.0f));                 //最大法术值
	            }
				//soke VIP2效果加成
				else if (this->issetUState(Cmd::USTATE_VIP2 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(2/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(2/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(2/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(2/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(2/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(2/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(2/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(2/100.0f));                 //最大法术值
	            }
				//soke VIP3效果加成
				else if (this->issetUState(Cmd::USTATE_VIP3 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(3/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(3/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(3/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(3/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(3/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(3/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(3/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(3/100.0f));                 //最大法术值
	            }
				//soke VIP4效果加成
				else if (this->issetUState(Cmd::USTATE_VIP4 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(4/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(4/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(4/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(4/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(4/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(4/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(4/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(4/100.0f));                 //最大法术值
	            }
				//soke VIP5效果加成
				else if (this->issetUState(Cmd::USTATE_VIP5 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(5/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(5/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(5/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(5/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(5/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(5/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(5/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(5/100.0f));                 //最大法术值
	            }
				//soke VIP6效果加成
				else if (this->issetUState(Cmd::USTATE_VIP6 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(6/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(6/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(6/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(6/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(6/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(6/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(6/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(6/100.0f));                 //最大法术值
	            }
				//soke VIP7效果加成
				else if (this->issetUState(Cmd::USTATE_VIP7 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(7/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(7/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(7/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(7/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(7/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(7/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(7/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(7/100.0f));                 //最大法术值
	            }
				//soke VIP8效果加成
				else if (this->issetUState(Cmd::USTATE_VIP8 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(8/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(8/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(8/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(8/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(8/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(8/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(8/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(8/100.0f));                 //最大法术值
	            }
				//soke VIP9效果加成
				else if (this->issetUState(Cmd::USTATE_VIP9 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(9/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(9/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(9/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(9/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(9/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(9/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(9/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(9/100.0f));                 //最大法术值
	            }
				//soke VIP10效果加成
				else if (this->issetUState(Cmd::USTATE_VIP10 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(10/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(10/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(10/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(10/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(10/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(10/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(10/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(10/100.0f));                 //最大法术值
	            }
				//soke VIP11效果加成
				else if (this->issetUState(Cmd::USTATE_VIP11 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(11/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(11/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(11/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(11/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(11/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(11/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(11/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(11/100.0f));                 //最大法术值
	            }
				//soke VIP12效果加成
				else if (this->issetUState(Cmd::USTATE_VIP12 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(12/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(12/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(12/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(12/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(12/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(12/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(12/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(12/100.0f));                 //最大法术值
	            }
				//soke VIP13效果加成
				else if (this->issetUState(Cmd::USTATE_VIP13 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(13/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(13/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(13/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(13/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(13/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(13/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(13/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(13/100.0f));                 //最大法术值
	            }
				//soke VIP14效果加成
				else if (this->issetUState(Cmd::USTATE_VIP14 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(14/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(14/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(14/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(14/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(14/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(14/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(14/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(14/100.0f));                 //最大法术值
	            }
				//soke VIP15效果加成
				else if (this->issetUState(Cmd::USTATE_VIP15 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(15/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(15/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(15/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(15/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(15/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(15/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(15/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(15/100.0f));                 //最大法术值
	            }
				//soke VIP16效果加成
				else if (this->issetUState(Cmd::USTATE_VIP16 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(18/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(18/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(18/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(18/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(18/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(18/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(18/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(18/100.0f));                 //最大法术值
	            }
				//soke VIP17效果加成
				else if (this->issetUState(Cmd::USTATE_VIP17 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(20/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(20/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(20/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(20/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(20/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(20/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(20/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(20/100.0f));                 //最大法术值
	            }
				//soke VIP18效果加成
				else if (this->issetUState(Cmd::USTATE_VIP18 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(22/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(22/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(22/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(22/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(22/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(22/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(22/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(22/100.0f));                 //最大法术值
	            }
				//soke VIP19效果加成
				else if (this->issetUState(Cmd::USTATE_VIP19 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(24/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(24/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(24/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(24/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(24/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(24/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(24/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(24/100.0f));                 //最大法术值
	            }
				//soke VIP20效果加成
				else if (this->issetUState(Cmd::USTATE_VIP20 ))
	            {
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(26/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(26/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(26/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(26/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(26/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(26/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(26/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(26/100.0f));                 //最大法术值
	            }

				

				
				if(this->issetUState(2000))
				{
					
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(5/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(5/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(5/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(5/100.0f));  //最大魔法攻击力

					charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(5/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(5/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(10/100.0f)); 
				
				}
					
		

				//sky 国王加成
				if (this->king && !this->emperor)
				{
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(8/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(8/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(8/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(8/100.0f));  //最大魔法攻击力

					charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(6/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(6/100.0f));        //魔防

					charstate.maxhp += DWORD( 500000 );         //最大生命值
					
				}
				//sky 皇帝加成
				else if (this->emperor)
				{
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(10/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(10/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(10/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(10/100.0f));  //最大魔法攻击力

					charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(8/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(8/100.0f));        //魔防

					charstate.maxhp += DWORD( 1000000 );         //最大生命值
					
				}
				else if(strstr(this->caption , "城主") != NULL)
				{
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(6/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(6/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(6/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(6/100.0f));  //最大魔法攻击力

					charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(4/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(4/100.0f));        //魔防

					charstate.maxhp += DWORD( 200000 );         //最大生命值 
				}
				//网管 白砖加成 2024-11-13 20:58:02
	            if (this->issetUState(Cmd::USTATE_LOONGPEARL_WHITE ))
	            {
		            charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(1/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(1/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(1/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(1/100.0f));  //最大魔法攻击力

					charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(1/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(1/100.0f));        //魔防
	            }
				//网管 蓝砖加成 2024-11-13 20:58:02 //2024-11-17 修复if 改 else if
	            else if (this->issetUState(Cmd::USTATE_LOONGPEARL_BLUE ))
	            {
		            charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(1/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(1/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(1/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(1/100.0f));  //最大魔法攻击力

					charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(1/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(1/100.0f));        //魔防
	            }
				//网管 黄砖加成 2024-11-13 20:58:02 //2024-11-17 修复if 改 else if
	            else if (this->issetUState(Cmd::USTATE_LOONGPEARL_YELLOW ))
	            {
		            charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(1/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(1/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(1/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(1/100.0f));  //最大魔法攻击力

					charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(1/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(1/100.0f));        //魔防
	            }
				//网管 绿砖加成 2024-11-13 20:58:02 //2024-11-17 修复if 改 else if
	            else if (this->issetUState(Cmd::USTATE_LOONGPEARL_GREEN ))
	            {
		            charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(1/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(1/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(1/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(1/100.0f));  //最大魔法攻击力

					charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(1/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(1/100.0f));        //魔防
	            }
				//网管 紫砖加成 2024-11-13 20:58:02 //2024-11-17 修复if 改 else if
	            else if (this->issetUState(Cmd::USTATE_LOONGPEARL_PURPLE ))
	            {
		            charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(2/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(2/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(2/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(2/100.0f));  //最大魔法攻击力

					charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(2/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(2/100.0f));        //魔防
	            }	
				//sky 战车加成
				if (this->issetUState(Cmd::USTATE_USER_ZC_TYPE1 ))//生命
				{
					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(30/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(30/100.0f));		
				}

				//sky 战车加成
				if (this->issetUState(Cmd::USTATE_USER_ZC_TYPE2 ))//攻击
				{
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(30/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(30/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(30/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(30/100.0f));  //最大魔法攻击力

				}

				//sky 战车加成
				if (this->issetUState(Cmd::USTATE_USER_ZC_TYPE3))//防御
				{
					charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(30/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(30/100.0f));        //魔防
				}


				//soke 白色图鉴套装加成效果
	            if (this->issetUState(Cmd::USTATE_USER_TZ_TUJIAN1 ))
	            {
		            charstate.pdamage    += DWORD( 2000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 2000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 2000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 2000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 1000 );      //物防
		            charstate.mdefence += DWORD( 1000 );      //魔防
		            charstate.maxhp += DWORD( 20000 );         //最大生命值	
	            }		
				//soke 蓝色图鉴套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_TUJIAN2 ))
	            {
		            charstate.pdamage    += DWORD( 4000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 4000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 4000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 4000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 2000 );      //物防
		            charstate.mdefence += DWORD( 2000 );      //魔防
		            charstate.maxhp += DWORD( 40000 );         //最大生命值	
	            }	
				//soke 黄色图鉴套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_TUJIAN3 ))
	            {
		            charstate.pdamage    += DWORD( 6000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 6000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 6000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 6000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 3000 );      //物防
		            charstate.mdefence += DWORD( 3000 );      //魔防
		            charstate.maxhp += DWORD( 60000 );         //最大生命值	
	            }	
				//soke 绿色图鉴套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_TUJIAN4 ))
	            {
		            charstate.pdamage    += DWORD( 8000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 8000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 8000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 8000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 4000 );      //物防
		            charstate.mdefence += DWORD( 4000 );      //魔防
		            charstate.maxhp += DWORD( 80000 );         //最大生命值	
	            }	
				//soke 紫色图鉴套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_TUJIAN5 ))
	            {
		            charstate.pdamage    += DWORD( 10000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 10000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 10000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 10000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 5000 );      //物防
		            charstate.mdefence += DWORD( 5000 );      //魔防
		            charstate.maxhp += DWORD( 100000 );         //最大生命值	
	            }
				//soke 橙色图鉴套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_TUJIAN6 ))
	            {
		            charstate.pdamage    += DWORD( 13000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 13000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 13000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 13000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 7000 );      //物防
		            charstate.mdefence += DWORD( 7000 );      //魔防
		            charstate.maxhp += DWORD( 130000 );         //最大生命值	
	            }
				//soke 红色图鉴套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_TUJIAN7 ))
	            {
		            charstate.pdamage    += DWORD( 16000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 16000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 16000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 16000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 9000 );      //物防
		            charstate.mdefence += DWORD( 9000 );      //魔防
		            charstate.maxhp += DWORD( 160000 );         //最大生命值	
	            }																											
				//soke 乾坤套装加成效果
	            if (this->issetUState(Cmd::USTATE_USER_TZ_QIANKUN ))
	            {
		            charstate.pdamage    += DWORD( 2000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 2000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 2000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 2000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 1000 );      //物防
		            charstate.mdefence += DWORD( 1000 );      //魔防
		            charstate.maxhp += DWORD( 20000 );         //最大生命值	
	            }
				//soke 炙天套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_ZHITIAN ))
	            {
		            charstate.pdamage    += DWORD( 4000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 4000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 4000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 4000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 2000 );      //物防
		            charstate.mdefence += DWORD( 2000 );      //魔防
		            charstate.maxhp += DWORD( 40000 );         //最大生命值	
	            }
				//soke 轩辕套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_XUANYUAN ))
	            {
		            charstate.pdamage    += DWORD( 6000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 6000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 6000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 6000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 3000 );      //物防
		            charstate.mdefence += DWORD( 3000 );      //魔防
		            charstate.maxhp += DWORD( 60000 );         //最大生命值	
	            }
			   //soke 卓越套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_ZHUOYUE ))
	            {
		            charstate.pdamage    += DWORD( 8000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 8000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 8000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 8000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 4000 );      //物防
		            charstate.mdefence += DWORD( 4000 );      //魔防
		            charstate.maxhp += DWORD( 80000 );         //最大生命值	
	            }
			   //soke 传说套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_CHUANSHUO ))
	            {
		            charstate.pdamage    += DWORD( 10000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 10000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 10000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 10000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 5000 );      //物防
		            charstate.mdefence += DWORD( 5000 );      //魔防
		            charstate.maxhp += DWORD( 100000 );         //最大生命值	
	            }				
			   //soke 天尊套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_TIANZUN ))
	            {
		            charstate.pdamage    += DWORD( 12000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 12000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 12000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 12000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 6000 );      //物防
		            charstate.mdefence += DWORD( 6000 );      //魔防
		            charstate.maxhp += DWORD( 120000 );         //最大生命值	
	            }
				//soke 英雄套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_YINGXIONG ))
	            {
		            charstate.pdamage    += DWORD( 14000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 14000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 14000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 14000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 7000 );      //物防
		            charstate.mdefence += DWORD( 7000 );      //魔防
		            charstate.maxhp += DWORD( 140000 );         //最大生命值	
	            }	
			   //soke 圣者套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_SHENGZHE ))
	            {
		            charstate.pdamage    += DWORD( 16000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 16000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 16000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 16000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 8000 );      //物防
		            charstate.mdefence += DWORD( 8000 );      //魔防
		            charstate.maxhp += DWORD( 160000 );         //最大生命值	
	            }
				//soke 战神套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_ZHANSHEN ))
	            {
		            charstate.pdamage    += DWORD( 18000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 18000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 18000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 18000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 9000 );      //物防
		            charstate.mdefence += DWORD( 9000 );      //魔防
		            charstate.maxhp += DWORD( 180000 );         //最大生命值	
	            }
				//soke 帝王套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_DIWANG ))
	            {
		            charstate.pdamage    += DWORD( 20000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 20000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 20000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 20000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 10000 );      //物防
		            charstate.mdefence += DWORD( 10000 );      //魔防
		            charstate.maxhp += DWORD( 200000 );         //最大生命值	
	            }
				//soke 弑天套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_SHITIAN ))
	            {
		            charstate.pdamage    += DWORD( 48000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 48000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 48000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 48000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 24000 );      //物防
		            charstate.mdefence += DWORD( 24000 );      //魔防
		            charstate.maxhp += DWORD( 480000 );         //最大生命值	
	            }				
				//soke 永恒套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_TZ_YONGHENG ))
	            {
		            charstate.pdamage    += DWORD( 30000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 30000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 30000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 30000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 15000 );      //物防
		            charstate.mdefence += DWORD( 15000 );      //魔防
		            charstate.maxhp += DWORD( 300000 );         //最大生命值	
	            }				
					
				//soke 补天石1级套装加成效果
	            if (this->issetUState(Cmd::USTATE_ULTRA_BUTIAN1_QI ))
	            {
		            charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(3/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(3/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(3/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(3/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(2/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(2/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(3/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(3/100.0f));                 //最大法术值
	            }	
				//soke 补天石2级套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_BUTIAN2_QI ))
	            {
		            charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(6/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(6/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(6/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(6/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(5/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(5/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(6/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(6/100.0f));                 //最大法术值
	            }	
				//soke 补天石3级套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_BUTIAN3_QI ))
	            {
		            charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(10/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(10/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(10/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(10/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(8/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(8/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(10/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(10/100.0f));                 //最大法术值
	            }	
				//soke 补天石4级套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_BUTIAN4_QI ))
	            {
		            charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(15/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(15/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(15/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(15/100.0f));  //最大魔法攻击力

                    charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(12/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(12/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(15/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(15/100.0f));                 //最大法术值
	            }	
				 //soke 补天石5级套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_BUTIAN5_QI ))
	            {
					/*
		            charstate.pdamage    += DWORD( 3200 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 3200 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 3200 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 3200 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 2000 );      //物防
		            charstate.mdefence += DWORD( 2000 );      //魔防
		            charstate.maxhp += DWORD( 20000 );         //最大生命值	
					*/
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(24/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(24/100.0f));  //最大物理攻击力
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(24/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(24/100.0f));  //最大魔法攻击力

					charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(20/100.0f));        //物防
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(20/100.0f));        //魔防

					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(24/100.0f));                 //最大生命值
					charstate.maxmp += (DWORD)((charstate.maxmp+packs.equip.getEquips().get_maxmp())*(24/100.0f));                 //最大法术值
				
	            }	

				// //soke 紫装3星套装加成效果
	            // if (this->issetUState(Cmd::USTATE_ULTRA_THR_XING ))
	            // {
		        //     charstate.pdamage    += DWORD( 1000 );    //最小物理攻击力
		        //     charstate.maxpdamage += DWORD( 1000 );    //最大物理攻击力
		        //     charstate.mdamage    += DWORD( 1000 );    //最小魔法攻击力
		        //     charstate.maxmdamage += DWORD( 1000 );    //最大魔法攻击力
		        //     charstate.pdefence += DWORD( 500 );      //物防
		        //     charstate.mdefence += DWORD( 500 );      //魔防
		        //     charstate.maxhp += DWORD( 10000 );         //最大生命值
					
	            // }
				// //soke 紫装5星套装加成效果
	            if (this->issetUState(Cmd::USTATE_ULTRA_FIV_XING  ))
	            {
		            charstate.pdamage    += DWORD( 5000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 5000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 5000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 5000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 2500 );      //物防
		            charstate.mdefence += DWORD( 2500 );      //魔防
		            charstate.maxhp += DWORD( 50000 );         //最大生命值
	            }
				//soke 紫装8星套装加成效果
	            if (this->issetUState(Cmd::USTATE_ULTRA_EIG_XING ))
	            {
		            charstate.pdamage    += DWORD( 8000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 8000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 8000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 8000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 4000 );      //物防
		            charstate.mdefence += DWORD( 4000 );      //魔防
		            charstate.maxhp += DWORD( 80000 );         //最大生命值
	            }
				//soke 紫装10星套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_TEN_XING ))
	            {
		            charstate.pdamage    += DWORD( 15000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 15000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 15000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 15000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 7500 );       //物防
		            charstate.mdefence += DWORD( 7500 );       //魔防
		            charstate.maxhp += DWORD( 150000 );         //最大生命值
	            }
				//soke 紫装12星套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_TWE_XING   ))
	            {   
		            charstate.pdamage    += DWORD( 30000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 30000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 30000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 30000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 15000 );       //物防
		            charstate.mdefence += DWORD( 15000 );       //魔防
		            charstate.maxhp += DWORD( 300000 );         //最大生命值
	            }
				//soke 紫装13星套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_THI_XING ))
	            {
		            charstate.pdamage    += DWORD( 50000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 50000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 50000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 50000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 25000 );      //物防
		            charstate.mdefence += DWORD( 25000 );      //魔防
		            charstate.maxhp += DWORD( 500000 );         //最大生命值
	            }
				//soke 紫装14星套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_FOU_XING ))
	            {
		            charstate.pdamage    += DWORD( 70000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 70000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 70000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 70000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 35000 );      //物防
		            charstate.mdefence += DWORD( 35000 );      //魔防
		            charstate.maxhp += DWORD( 700000 );        //最大生命值
	            }
				//soke 紫装15星套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_FIF_XING2 ))
	            {
		            charstate.pdamage    += DWORD( 100000 );     //最小物理攻击力
		            charstate.maxpdamage += DWORD( 100000 );     //最大物理攻击力
		            charstate.mdamage    += DWORD( 100000 );     //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 100000 );     //最大魔法攻击力
		            charstate.pdefence += DWORD( 50000 );       //物防
		            charstate.mdefence += DWORD( 50000 );       //魔防
		            charstate.maxhp += DWORD( 1000000 );         //最大生命值
	            }


				//附件神剑剑冢属性加成 by醉梦
				int pDam=0;
				int mDam=0;
				int pDef=0;
				int mDef=0;
				int hp=0;
				if(charbase.jian1>0)
				{
					for(DWORD i=0;i<charbase.jian1;i++)
					{
						pDam += fjconfig::getInstance().fjlist[i].pDam;
						mDam += fjconfig::getInstance().fjlist[i].mDam;
						pDef += fjconfig::getInstance().fjlist[i].pDef;
						mDef += fjconfig::getInstance().fjlist[i].mDef;
						hp += fjconfig::getInstance().fjlist[i].hp;
					}
					
				}
				if(charbase.jian2>0)
				{
					for(DWORD i=0;i<charbase.jian2;i++)
					{
						pDam += fjconfig::getInstance().fjlist[i].pDam;
						mDam += fjconfig::getInstance().fjlist[i].mDam;
						pDef += fjconfig::getInstance().fjlist[i].pDef;
						mDef += fjconfig::getInstance().fjlist[i].mDef;
						hp += fjconfig::getInstance().fjlist[i].hp;
					}
					
				}
				if(charbase.jian3>0)
				{
					for(DWORD i=0;i<charbase.jian3;i++)
					{
						pDam += fjconfig::getInstance().fjlist[i].pDam;
						mDam += fjconfig::getInstance().fjlist[i].mDam;
						pDef += fjconfig::getInstance().fjlist[i].pDef;
						mDef += fjconfig::getInstance().fjlist[i].mDef;
						hp += fjconfig::getInstance().fjlist[i].hp;
					}
					
				}
				if(charbase.jian4>0)
				{
					for(DWORD i=0;i<charbase.jian4;i++)
					{
						pDam += fjconfig::getInstance().fjlist[i].pDam;
						mDam += fjconfig::getInstance().fjlist[i].mDam;
						pDef += fjconfig::getInstance().fjlist[i].pDef;
						mDef += fjconfig::getInstance().fjlist[i].mDef;
						hp += fjconfig::getInstance().fjlist[i].hp;
					}
					
				}
				if(charbase.jian5>0)
				{
					for(DWORD i=0;i<charbase.jian5;i++)
					{
						pDam += fjconfig::getInstance().fjlist[i].pDam;
						mDam += fjconfig::getInstance().fjlist[i].mDam;
						pDef += fjconfig::getInstance().fjlist[i].pDef;
						mDef += fjconfig::getInstance().fjlist[i].mDef;
						hp += fjconfig::getInstance().fjlist[i].hp;
					}
					
				}
				if(charbase.jian6>0)
				{
					for(DWORD i=0;i<charbase.jian6;i++)
					{
						pDam += fjconfig::getInstance().fjlist[i].pDam;
						mDam += fjconfig::getInstance().fjlist[i].mDam;
						pDef += fjconfig::getInstance().fjlist[i].pDef;
						mDef += fjconfig::getInstance().fjlist[i].mDef;
						hp += fjconfig::getInstance().fjlist[i].hp;
					}
					
				}


				//附件征途传 征途传属性加成 by醉梦
				if(charbase.zhuan1>0)
				{
					for(DWORD i=0;i<charbase.zhuan1;i++)
					{
						pDam += fjconfig::getInstance().ztzlist[i].pDam;
						mDam += fjconfig::getInstance().ztzlist[i].mDam;
						pDef += fjconfig::getInstance().ztzlist[i].pDef;
						mDef += fjconfig::getInstance().ztzlist[i].mDef;
						hp += fjconfig::getInstance().ztzlist[i].hp;
					}
					
				}
				if(charbase.zhuan2>0)
				{
					for(DWORD i=0;i<charbase.zhuan2;i++)
					{
						pDam += fjconfig::getInstance().ztzlist[i].pDam;
						mDam += fjconfig::getInstance().ztzlist[i].mDam;
						pDef += fjconfig::getInstance().ztzlist[i].pDef;
						mDef += fjconfig::getInstance().ztzlist[i].mDef;
						hp += fjconfig::getInstance().ztzlist[i].hp;
					}
					
				}
				if(charbase.zhuan3>0)
				{
					for(DWORD i=0;i<charbase.zhuan3;i++)
					{
						pDam += fjconfig::getInstance().ztzlist[i].pDam;
						mDam += fjconfig::getInstance().ztzlist[i].mDam;
						pDef += fjconfig::getInstance().ztzlist[i].pDef;
						mDef += fjconfig::getInstance().ztzlist[i].mDef;
						hp += fjconfig::getInstance().ztzlist[i].hp;
					}
					
				}
				if(charbase.zhuan4>0)
				{
					for(DWORD i=0;i<charbase.zhuan4;i++)
					{
						pDam += fjconfig::getInstance().ztzlist[i].pDam;
						mDam += fjconfig::getInstance().ztzlist[i].mDam;
						pDef += fjconfig::getInstance().ztzlist[i].pDef;
						mDef += fjconfig::getInstance().ztzlist[i].mDef;
						hp += fjconfig::getInstance().ztzlist[i].hp;
					}
					
				}
				if(charbase.zhuan5>0)
				{
					for(DWORD i=0;i<charbase.zhuan5;i++)
					{
						pDam += fjconfig::getInstance().ztzlist[i].pDam;
						mDam += fjconfig::getInstance().ztzlist[i].mDam;
						pDef += fjconfig::getInstance().ztzlist[i].pDef;
						mDef += fjconfig::getInstance().ztzlist[i].mDef;
						hp += fjconfig::getInstance().ztzlist[i].hp;
					}
					
				}
				if(charbase.zhuan6>0)
				{
					for(DWORD i=0;i<charbase.zhuan6;i++)
					{
						pDam += fjconfig::getInstance().ztzlist[i].pDam;
						mDam += fjconfig::getInstance().ztzlist[i].mDam;
						pDef += fjconfig::getInstance().ztzlist[i].pDef;
						mDef += fjconfig::getInstance().ztzlist[i].mDef;
						hp += fjconfig::getInstance().ztzlist[i].hp;
					}
					
				}
				if(charbase.zhuan7>0)
				{
					for(DWORD i=0;i<charbase.zhuan7;i++)
					{
						pDam += fjconfig::getInstance().ztzlist[i].pDam;
						mDam += fjconfig::getInstance().ztzlist[i].mDam;
						pDef += fjconfig::getInstance().ztzlist[i].pDef;
						mDef += fjconfig::getInstance().ztzlist[i].mDef;
						hp += fjconfig::getInstance().ztzlist[i].hp;
					}
					
				}
				if(charbase.zhuan8>0)
				{
					for(DWORD i=0;i<charbase.zhuan8;i++)
					{
						pDam += fjconfig::getInstance().ztzlist[i].pDam;
						mDam += fjconfig::getInstance().ztzlist[i].mDam;
						pDef += fjconfig::getInstance().ztzlist[i].pDef;
						mDef += fjconfig::getInstance().ztzlist[i].mDef;
						hp += fjconfig::getInstance().ztzlist[i].hp;
					}
					
				}
				if(charbase.zhuan9>0)
				{
					for(DWORD i=0;i<charbase.zhuan9;i++)
					{
						pDam += fjconfig::getInstance().ztzlist[i].pDam;
						mDam += fjconfig::getInstance().ztzlist[i].mDam;
						pDef += fjconfig::getInstance().ztzlist[i].pDef;
						mDef += fjconfig::getInstance().ztzlist[i].mDef;
						hp += fjconfig::getInstance().ztzlist[i].hp;
					}
					
				}
				if(charbase.zhuan10>0)
				{
					for(DWORD i=0;i<charbase.zhuan10;i++)
					{
						pDam += fjconfig::getInstance().ztzlist[i].pDam;
						mDam += fjconfig::getInstance().ztzlist[i].mDam;
						pDef += fjconfig::getInstance().ztzlist[i].pDef;
						mDef += fjconfig::getInstance().ztzlist[i].mDef;
						hp += fjconfig::getInstance().ztzlist[i].hp;
					}
					
				}

				//附件十二生肖 十二生肖属性加成 by醉梦
				if(charbase.shengx1>0)
				{
					for(DWORD i=0;i<charbase.shengx1;i++)
					{
						pDam += fjconfig::getInstance().shengxiaolist[i].pDam;
						mDam += fjconfig::getInstance().shengxiaolist[i].mDam;
						pDef += fjconfig::getInstance().shengxiaolist[i].pDef;
						mDef += fjconfig::getInstance().shengxiaolist[i].mDef;
						hp += fjconfig::getInstance().shengxiaolist[i].hp;
					}			
				}
				if(charbase.shengx2>0)
				{
					for(DWORD i=0;i<charbase.shengx2;i++)
					{
						pDam += fjconfig::getInstance().shengxiaolist[i].pDam;
						mDam += fjconfig::getInstance().shengxiaolist[i].mDam;
						pDef += fjconfig::getInstance().shengxiaolist[i].pDef;
						mDef += fjconfig::getInstance().shengxiaolist[i].mDef;
						hp += fjconfig::getInstance().shengxiaolist[i].hp;
					}			
				}
				if(charbase.shengx3>0)
				{
					for(DWORD i=0;i<charbase.shengx3;i++)
					{
						pDam += fjconfig::getInstance().shengxiaolist[i].pDam;
						mDam += fjconfig::getInstance().shengxiaolist[i].mDam;
						pDef += fjconfig::getInstance().shengxiaolist[i].pDef;
						mDef += fjconfig::getInstance().shengxiaolist[i].mDef;
						hp += fjconfig::getInstance().shengxiaolist[i].hp;
					}			
				}
				if(charbase.shengx4>0)
				{
					for(DWORD i=0;i<charbase.shengx4;i++)
					{
						pDam += fjconfig::getInstance().shengxiaolist[i].pDam;
						mDam += fjconfig::getInstance().shengxiaolist[i].mDam;
						pDef += fjconfig::getInstance().shengxiaolist[i].pDef;
						mDef += fjconfig::getInstance().shengxiaolist[i].mDef;
						hp += fjconfig::getInstance().shengxiaolist[i].hp;
					}			
				}
				if(charbase.shengx5>0)
				{
					for(DWORD i=0;i<charbase.shengx5;i++)
					{
						pDam += fjconfig::getInstance().shengxiaolist[i].pDam;
						mDam += fjconfig::getInstance().shengxiaolist[i].mDam;
						pDef += fjconfig::getInstance().shengxiaolist[i].pDef;
						mDef += fjconfig::getInstance().shengxiaolist[i].mDef;
						hp += fjconfig::getInstance().shengxiaolist[i].hp;
					}			
				}
				if(charbase.shengx6>0)
				{
					for(DWORD i=0;i<charbase.shengx6;i++)
					{
						pDam += fjconfig::getInstance().shengxiaolist[i].pDam;
						mDam += fjconfig::getInstance().shengxiaolist[i].mDam;
						pDef += fjconfig::getInstance().shengxiaolist[i].pDef;
						mDef += fjconfig::getInstance().shengxiaolist[i].mDef;
						hp += fjconfig::getInstance().shengxiaolist[i].hp;
					}			
				}
				if(charbase.shengx7>0)
				{
					for(DWORD i=0;i<charbase.shengx7;i++)
					{
						pDam += fjconfig::getInstance().shengxiaolist[i].pDam;
						mDam += fjconfig::getInstance().shengxiaolist[i].mDam;
						pDef += fjconfig::getInstance().shengxiaolist[i].pDef;
						mDef += fjconfig::getInstance().shengxiaolist[i].mDef;
						hp += fjconfig::getInstance().shengxiaolist[i].hp;
					}			
				}
				if(charbase.shengx8>0)
				{
					for(DWORD i=0;i<charbase.shengx8;i++)
					{
						pDam += fjconfig::getInstance().shengxiaolist[i].pDam;
						mDam += fjconfig::getInstance().shengxiaolist[i].mDam;
						pDef += fjconfig::getInstance().shengxiaolist[i].pDef;
						mDef += fjconfig::getInstance().shengxiaolist[i].mDef;
						hp += fjconfig::getInstance().shengxiaolist[i].hp;
					}			
				}
				if(charbase.shengx9>0)
				{
					for(DWORD i=0;i<charbase.shengx9;i++)
					{
						pDam += fjconfig::getInstance().shengxiaolist[i].pDam;
						mDam += fjconfig::getInstance().shengxiaolist[i].mDam;
						pDef += fjconfig::getInstance().shengxiaolist[i].pDef;
						mDef += fjconfig::getInstance().shengxiaolist[i].mDef;
						hp += fjconfig::getInstance().shengxiaolist[i].hp;
					}			
				}
				if(charbase.shengx10>0)
				{
					for(DWORD i=0;i<charbase.shengx10;i++)
					{
						pDam += fjconfig::getInstance().shengxiaolist[i].pDam;
						mDam += fjconfig::getInstance().shengxiaolist[i].mDam;
						pDef += fjconfig::getInstance().shengxiaolist[i].pDef;
						mDef += fjconfig::getInstance().shengxiaolist[i].mDef;
						hp += fjconfig::getInstance().shengxiaolist[i].hp;
					}			
				}if(charbase.shengx11>0)
				{
					for(DWORD i=0;i<charbase.shengx11;i++)
					{
						pDam += fjconfig::getInstance().shengxiaolist[i].pDam;
						mDam += fjconfig::getInstance().shengxiaolist[i].mDam;
						pDef += fjconfig::getInstance().shengxiaolist[i].pDef;
						mDef += fjconfig::getInstance().shengxiaolist[i].mDef;
						hp += fjconfig::getInstance().shengxiaolist[i].hp;
					}			
				}if(charbase.shengx12>0)
				{
					for(DWORD i=0;i<charbase.shengx12;i++)
					{
						pDam += fjconfig::getInstance().shengxiaolist[i].pDam;
						mDam += fjconfig::getInstance().shengxiaolist[i].mDam;
						pDef += fjconfig::getInstance().shengxiaolist[i].pDef;
						mDef += fjconfig::getInstance().shengxiaolist[i].mDef;
						hp += fjconfig::getInstance().shengxiaolist[i].hp;
					}			
				}

				//附件圣器属性加成 by醉梦
				if(charbase.shengq1>0)
				{
					for(DWORD i=0;i<charbase.shengq1;i++)
					{
						pDam += fjconfig::getInstance().shengqilist[i].pDam;
						mDam += fjconfig::getInstance().shengqilist[i].mDam;
						pDef += fjconfig::getInstance().shengqilist[i].pDef;
						mDef += fjconfig::getInstance().shengqilist[i].mDef;
						hp += fjconfig::getInstance().shengqilist[i].hp;
					}			
				}

				//附件经脉属性加成 by醉梦
				if(charbase.jingm1>0)
				{
					for(DWORD i=0;i<charbase.jingm1;i++)
					{
						pDam += fjconfig::getInstance().jingmailist[i].pDam;
						mDam += fjconfig::getInstance().jingmailist[i].mDam;
						pDef += fjconfig::getInstance().jingmailist[i].pDef;
						mDef += fjconfig::getInstance().jingmailist[i].mDef;
						hp += fjconfig::getInstance().jingmailist[i].hp;
					}			
				}
				if(charbase.jingm2>0)
				{
					for(DWORD i=0;i<charbase.jingm2;i++)
					{
						pDam += fjconfig::getInstance().jingmailist[i].pDam;
						mDam += fjconfig::getInstance().jingmailist[i].mDam;
						pDef += fjconfig::getInstance().jingmailist[i].pDef;
						mDef += fjconfig::getInstance().jingmailist[i].mDef;
						hp += fjconfig::getInstance().jingmailist[i].hp;
					}			
				}
				if(charbase.jingm3>0)
				{
					for(DWORD i=0;i<charbase.jingm3;i++)
					{
						pDam += fjconfig::getInstance().jingmailist[i].pDam;
						mDam += fjconfig::getInstance().jingmailist[i].mDam;
						pDef += fjconfig::getInstance().jingmailist[i].pDef;
						mDef += fjconfig::getInstance().jingmailist[i].mDef;
						hp += fjconfig::getInstance().jingmailist[i].hp;
					}			
				}

				//附件头衔属性加成 by醉梦
				for(DWORD i=0;i<charbase.touxianlevel;i++)
				{
					pDam += fjconfig::getInstance().touxianlist[i].pDam;
					mDam += fjconfig::getInstance().touxianlist[i].mDam;
					pDef += fjconfig::getInstance().touxianlist[i].pDef;
					mDef += fjconfig::getInstance().touxianlist[i].mDef;
					hp += fjconfig::getInstance().touxianlist[i].hp;
				}		
				
				//附件称号属性加成 by醉梦
				for(int i=0;i<60;i++)
				{
					if(packs.m_Chenghao[i].state == 1)
					{
						pDam += fjconfig::getInstance().chenghaolist[i+1].pDam;
						mDam += fjconfig::getInstance().chenghaolist[i+1].mDam;
						pDef += fjconfig::getInstance().chenghaolist[i+1].pDef;
						mDef += fjconfig::getInstance().chenghaolist[i+1].mDef;
						hp += fjconfig::getInstance().chenghaolist[i+1].hp;
					}
				}
				
				//附件儿女属性加成 by醉梦
				for(DWORD i=0;i<charbase.babylevel;i++)
				{
					pDam += fjconfig::getInstance().ernvlist[i].pDam;
					mDam += fjconfig::getInstance().ernvlist[i].mDam;
					pDef += fjconfig::getInstance().ernvlist[i].pDef;
					mDef += fjconfig::getInstance().ernvlist[i].mDef;
					hp += fjconfig::getInstance().ernvlist[i].hp;
				}	

				//附件能力修炼属性加成 by醉梦
				for(DWORD i=0;i<=charbase.xiulianlevel1;i++)
				{
					pDam += fjconfig::getInstance().xiulianlist[i].pDam;
					
				}	
				for(DWORD i=0;i<=charbase.xiulianlevel2;i++)
				{
					
					mDam += fjconfig::getInstance().xiulianlist[i].mDam;
					
				}	
				for(DWORD i=0;i<=charbase.xiulianlevel3;i++)
				{
					
					pDef += fjconfig::getInstance().xiulianlist[i].pDef;
				
				}	
				for(DWORD i=0;i<=charbase.xiulianlevel4;i++)
				{
					
					mDef += fjconfig::getInstance().xiulianlist[i].mDef;
		
				}	
				for(DWORD i=0;i<=charbase.xiulianlevel5;i++)
				{
					
					hp += fjconfig::getInstance().xiulianlist[i].hp;
				}	

				//附件云天别墅属性加成
				for(DWORD i=0;i<=charbase.bieshulevel;i++)
				{
					pDam += fjconfig::getInstance().bieshulist[i].gong;
					mDam += fjconfig::getInstance().bieshulist[i].gong;
					pDef += fjconfig::getInstance().bieshulist[i].fang;
					mDef += fjconfig::getInstance().bieshulist[i].fang;
					hp += fjconfig::getInstance().bieshulist[i].hp;
				}		
				

				//附件坐骑图鉴属性加成 by醉梦
				for(DWORD i=0;i<=charbase.zuoqitujianlevel;i++)
				{
					pDam += fjconfig::getInstance().zuoqilist[i].pDam;
					mDam += fjconfig::getInstance().zuoqilist[i].mDam;
					pDef += fjconfig::getInstance().zuoqilist[i].pDef;
					mDef += fjconfig::getInstance().zuoqilist[i].mDef;
					hp += fjconfig::getInstance().zuoqilist[i].hp;
				}	


				//时装魔盒加成

				for(DWORD i=0;i<=charbase.mohelevel;i++)
				{
					pDam += fjconfig::getInstance().mohelist[i].pDam;
					mDam += fjconfig::getInstance().mohelist[i].mDam;
					pDef += fjconfig::getInstance().mohelist[i].pDef;
					mDef += fjconfig::getInstance().mohelist[i].mDef;
					hp += fjconfig::getInstance().mohelist[i].hp;
				}	
				
				charstate.pdamage += (DWORD)(pDam);           //最小物理攻击力
				charstate.maxpdamage += (DWORD)(pDam);   //最大物理攻击力
				charstate.mdamage += (DWORD)(mDam);             //最小魔法攻击力
				charstate.maxmdamage += (DWORD)(mDam);   //最大魔法攻击力
				charstate.pdefence += (DWORD)(pDef);          //物防
				charstate.mdefence += (DWORD)(mDef);          //魔防
				charstate.maxhp += (DWORD)(hp);



				//儿女技能加成

				if(charbase.babyskill1 !=0)
				{
					charstate.pdamage += (DWORD)((charstate.pdamage+packs.equip.getEquips().get_pdamage())*(charbase.babyskill1/100.0f));           //最小物理攻击力
					charstate.maxpdamage += (DWORD)((charstate.maxpdamage+packs.equip.getEquips().get_maxpdamage())*(charbase.babyskill1/100.0f));  //最大物理攻击力
				
				}
				if(charbase.babyskill2 !=0)
				{
					charstate.mdamage += (DWORD)((charstate.mdamage+packs.equip.getEquips().get_mdamage())*(charbase.babyskill2/100.0f));           //最小魔法攻击力
					charstate.maxmdamage += (DWORD)((charstate.maxmdamage+packs.equip.getEquips().get_maxmdamage())*(charbase.babyskill2/100.0f));  //最大魔法攻击力

				}
				if(charbase.babyskill3 !=0)
				{
					charstate.pdefence += (DWORD)((charstate.pdefence+packs.equip.getEquips().get_pdefence())*(charbase.babyskill3/100.0f));        //物防
				}

				if(charbase.babyskill4 != 0)
				{
					charstate.mdefence += (DWORD)((charstate.mdefence+packs.equip.getEquips().get_mdefence())*(charbase.babyskill4/100.0f));        //魔防
				}
				
				if(charbase.babyskill5 != 0)
				{
					charstate.maxhp += (DWORD)((charstate.maxhp+packs.equip.getEquips().get_maxhp())*(charbase.babyskill5/100.0f));                 //最大生命值
				}
			

				// //soke 第二灵魂加成效果+ 50000
	            // if (this->issetUState(Cmd::USTATE_DI_ER_LING_HUN ))
	            // {
		        //     charstate.maxhp += DWORD( 25000 );           //最大生命值
	            // }
				// //soke 第二灵魂属性加成效果(8件激活) 默认加成10件 
	            // if (this->issetUState(Cmd::USTATE_DI_ER_LING_HUN1 ))
	            // {
		        //     charstate.pdefence += DWORD( 6000 );           //第二灵魂物理防御加成
	            // }
				// else if (this->issetUState(Cmd::USTATE_DI_ER_LING_HUN2 ))
	            // {
		        //     charstate.mdefence += DWORD( 6000 );           //第二灵魂魔法防御加成
	            // }
				// else if (this->issetUState(Cmd::USTATE_DI_ER_LING_HUN3 ))
	            // {
		        //     charstate.pdamage += DWORD( 12000 );           //第二灵魂物理攻击加成
				// 	charstate.maxpdamage += DWORD( 12000 );
	            // }
				// else if (this->issetUState(Cmd::USTATE_DI_ER_LING_HUN4 ))
	            // {
		        //     charstate.mdamage += DWORD( 12000 );           //第二灵魂魔法攻击加成
				// 	charstate.maxmdamage += DWORD( 12000 );
	            // }
				// else if (this->issetUState(Cmd::USTATE_DI_ER_LING_HUN5 ))
	            // {
		        //     charstate.maxhp += DWORD( 200000 );           //第二灵魂增加生命加成
	            // }
				
			   //soke 全套斗魂3星套装加成效果
	            if (this->issetUState(Cmd::USTATE_ULTRA_DOUHUN3 ))
	            {
		            charstate.pdamage    += DWORD( 3000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 3000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 3000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 3000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 2500 );      //物防
		            charstate.mdefence += DWORD( 2500 );      //魔防
		            charstate.maxhp += DWORD( 30000 );         //最大生命值
					
	            }
				 //soke 全套斗魂6星套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_DOUHUN6 ))
	            {
		            charstate.pdamage    += DWORD( 6000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 6000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 6000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 6000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 5000 );      //物防
		            charstate.mdefence += DWORD( 5000 );      //魔防
		            charstate.maxhp += DWORD( 60000 );         //最大生命值
	            }
				 //soke 全套斗魂9星套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_DOUHUN9 ))
	            {
		            charstate.pdamage    += DWORD( 9000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 9000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 9000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 9000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 7500 );      //物防
		            charstate.mdefence += DWORD( 7500 );      //魔防
		            charstate.maxhp += DWORD( 90000 );         //最大生命值
	            }
				 //soke 全套斗魂12星套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_DOUHUN12 ))
	            {
		            charstate.pdamage    += DWORD( 20000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 20000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 20000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 20000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 15000 );       //物防
		            charstate.mdefence += DWORD( 15000 );       //魔防
		            charstate.maxhp += DWORD( 180000 );         //最大生命值
	            }
				 //soke 全套斗魂15星套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_DOUHUN15 ))
	            {   
		          	charstate.pdamage    += DWORD( 50000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 50000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 50000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 50000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 30000 );       //物防
		            charstate.mdefence += DWORD( 30000 );       //魔防
		            charstate.maxhp += DWORD( 500000 );         //最大生命值
	            }

				//soke 龙星5星套装加成效果
	            if (this->issetUState(Cmd::USTATE_ULTRA_WU_XING  ))
	            {
		            charstate.pdamage    += DWORD( 5000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 5000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 5000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 5000 );    //最大魔法攻击力
					charstate.pdefence += DWORD( 2500 );       //物防
		            charstate.mdefence += DWORD( 2500 );       //魔防
		            charstate.maxhp += DWORD( 50000 );         //最大生命值
	            }
				//soke 龙星8星套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_BA_XING ))
	            {
		            charstate.pdamage    += DWORD( 10000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 10000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 10000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 10000 );    //最大魔法攻击力
					charstate.pdefence += DWORD( 5000 );       //物防
		            charstate.mdefence += DWORD( 5000 );       //魔防
		            charstate.maxhp += DWORD( 100000 );         //最大生命值
	            }
				//soke 龙星11星套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_SHIYI_XING ))
	            {
		            charstate.pdamage    += DWORD( 30000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 30000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 30000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 30000 );    //最大魔法攻击力
					charstate.pdefence += DWORD( 15000 );       //物防
		            charstate.mdefence += DWORD( 15000 );       //魔防
		            charstate.maxhp += DWORD( 300000 );         //最大生命值
	            }
				//soke 龙星13星套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_SHISA_XING ))
	            {
		            charstate.pdamage    += DWORD( 50000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 50000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 50000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 50000 );    //最大魔法攻击力
					charstate.pdefence += DWORD( 25000 );       //物防
		            charstate.mdefence += DWORD( 25000 );       //魔防
		            charstate.maxhp += DWORD( 500000 );         //最大生命值
	            }
				//soke 龙星15星套装加成效果
	            else if (this->issetUState(Cmd::USTATE_ULTRA_SHIWU_XING ))
	            {
		            charstate.pdamage    += DWORD( 100000 );     //最小物理攻击力
		            charstate.maxpdamage += DWORD( 100000 );     //最大物理攻击力
		            charstate.mdamage    += DWORD( 100000 );     //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 100000 );     //最大魔法攻击力
					charstate.pdefence += DWORD( 50000 );       //物防
		            charstate.mdefence += DWORD( 50000 );       //魔防
		            charstate.maxhp += DWORD( 1000000 );         //最大生命值
	            }

				//soke装备栏蓝色祝福(10件)套装加成效果
				if (this->issetUState(Cmd::USTATE_ULTRA_ZHUANG2_QI   ))
				{
					charstate.pdamage    += DWORD( 5000 );    //最小物理攻击力
					charstate.maxpdamage += DWORD( 5000 );    //最大物理攻击力
					charstate.mdamage    += DWORD( 5000 );    //最小魔法攻击力
					charstate.maxmdamage += DWORD( 5000 );    //最大魔法攻击力
					charstate.pdefence += DWORD( 5000 );       //物防
		            charstate.mdefence += DWORD( 5000 );       //魔防
					charstate.maxhp += DWORD( 50000 );         //最大生命值
					
				}
				//soke装备栏黄色祝福(10件)套装加成效果
				else if (this->issetUState(Cmd::USTATE_ULTRA_ZHUANG3_QI   ))
				{
					charstate.pdamage    += DWORD( 10000 );    //最小物理攻击力
					charstate.maxpdamage += DWORD( 10000 );    //最大物理攻击力
					charstate.mdamage    += DWORD( 10000 );    //最小魔法攻击力
					charstate.maxmdamage += DWORD( 10000 );    //最大魔法攻击力
					charstate.pdefence += DWORD( 10000 );       //物防
		            charstate.mdefence += DWORD( 10000 );       //魔防
					charstate.maxhp += DWORD( 100000 );         //最大生命值
				}
				//soke装备栏绿色祝福(10件)套装加成效果
				else if (this->issetUState(Cmd::USTATE_ULTRA_ZHUANG4_QI   ))
				{
					charstate.pdamage    += DWORD( 15000 );    //最小物理攻击力
					charstate.maxpdamage += DWORD( 15000 );    //最大物理攻击力
					charstate.mdamage    += DWORD( 15000 );    //最小魔法攻击力
					charstate.maxmdamage += DWORD( 15000 );    //最大魔法攻击力
					charstate.pdefence += DWORD( 15000 );       //物防
		            charstate.mdefence += DWORD( 15000 );       //魔防
					charstate.maxhp += DWORD( 150000 );         //最大生命值
				}
				//soke装备栏紫色祝福(10件)套装加成效果
				else if (this->issetUState(Cmd::USTATE_ULTRA_ZHUANG5_QI   ))
				{
					charstate.pdamage    += DWORD( 20000 );    //最小物理攻击力
					charstate.maxpdamage += DWORD( 20000 );    //最大物理攻击力
					charstate.mdamage    += DWORD( 20000 );    //最小魔法攻击力
					charstate.maxmdamage += DWORD( 20000 );    //最大魔法攻击力
					charstate.pdefence += DWORD( 20000 );       //物防
		            charstate.mdefence += DWORD( 20000 );       //魔防
					charstate.maxhp += DWORD( 200000 );         //最大生命值
				}
				//soke装备栏红色祝福(10件)套装加成效果
				else if (this->issetUState(Cmd::USTATE_ULTRA_ZHUANG6_QI   ))
				{
					charstate.pdamage    += DWORD( 30000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 30000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 30000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 30000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 25000 );       //物防
		            charstate.mdefence += DWORD( 25000 );       //魔防
		            charstate.maxhp += DWORD( 300000 );         //最大生命值
				}

				//soke 1阶神器套装效果(10件)套装加成效果
				if (this->issetUState(Cmd::USTATE_ULTRA_SHEN1_QI))
				{
					charstate.pdamage    += DWORD( 3200 );    //最小物理攻击力
					charstate.maxpdamage += DWORD( 3200 );    //最大物理攻击力
					charstate.mdamage    += DWORD( 3200 );    //最小魔法攻击力
					charstate.maxmdamage += DWORD( 3200 );    //最大魔法攻击力
					charstate.pdefence += DWORD( 3200 );       //物防
		            charstate.mdefence += DWORD( 3200 );       //魔防
					charstate.maxhp += DWORD( 32000 );        //最大生命值
				}
				//soke 2阶神器套装效果(10件)套装加成效果
				else if (this->issetUState(Cmd::USTATE_ULTRA_SHEN2_QI))
				{
					charstate.pdamage    += DWORD( 6400 );    //最小物理攻击力
					charstate.maxpdamage += DWORD( 6400 );    //最大物理攻击力
					charstate.mdamage    += DWORD( 6400 );    //最小魔法攻击力
					charstate.maxmdamage += DWORD( 6400 );    //最大魔法攻击力
					charstate.pdefence += DWORD( 6400 );       //物防
		            charstate.mdefence += DWORD( 6400 );       //魔防
					charstate.maxhp += DWORD( 64000 );        //最大生命值
				}
				//soke 3阶神器套装效果(10件)套装加成效果
				else if (this->issetUState(Cmd::USTATE_ULTRA_SHEN3_QI))
				{
					charstate.pdamage    += DWORD( 12800 );    //最小物理攻击力
					charstate.maxpdamage += DWORD( 12800 );    //最大物理攻击力
					charstate.mdamage    += DWORD( 12800 );    //最小魔法攻击力
					charstate.maxmdamage += DWORD( 12800 );    //最大魔法攻击力
					charstate.pdefence += DWORD( 12800 );       //物防
		            charstate.mdefence += DWORD( 12800 );       //魔防
					charstate.maxhp += DWORD( 128000 );        //最大生命值
				}
				//soke 4阶神器套装效果(10件)套装加成效果
				else if (this->issetUState(Cmd::USTATE_ULTRA_SHEN4_QI))
				{
					charstate.pdamage    += DWORD( 19200 );    //最小物理攻击力
					charstate.maxpdamage += DWORD( 19200 );    //最大物理攻击力
					charstate.mdamage    += DWORD( 19200 );    //最小魔法攻击力
					charstate.maxmdamage += DWORD( 19200 );    //最大魔法攻击力
					charstate.pdefence += DWORD( 19200 );       //物防
		            charstate.mdefence += DWORD( 19200 );       //魔防
					charstate.maxhp += DWORD( 192000 );        //最大生命值
				}
				//soke 5阶神器套装效果(10件)套装加成效果
				else if (this->issetUState(Cmd::USTATE_ULTRA_SHEN5_QI))
				{
					charstate.pdamage    += DWORD( 25600 );    //最小物理攻击力
					charstate.maxpdamage += DWORD( 25600 );    //最大物理攻击力
					charstate.mdamage    += DWORD( 25600 );    //最小魔法攻击力
					charstate.maxmdamage += DWORD( 25600 );    //最大魔法攻击力
					charstate.pdefence += DWORD( 25600 );       //物防
		            charstate.mdefence += DWORD( 25600 );       //魔防
					charstate.maxhp += DWORD( 256000 );        //最大生命值
				}
				//soke 6阶神器套装效果(10件)套装加成效果
				else if (this->issetUState(Cmd::USTATE_ULTRA_SHEN6_QI))
				{
					charstate.pdamage    += DWORD( 32000 );    //最小物理攻击力
					charstate.maxpdamage += DWORD( 32000 );    //最大物理攻击力
					charstate.mdamage    += DWORD( 32000 );    //最小魔法攻击力
					charstate.maxmdamage += DWORD( 32000 );    //最大魔法攻击力
					charstate.pdefence += DWORD( 32000 );       //物防
		            charstate.mdefence += DWORD( 32000 );       //魔防
					charstate.maxhp += DWORD( 320000 );        //最大生命值
				}
				//soke 7阶神器套装效果(10件)套装加成效果
				else if (this->issetUState(Cmd::USTATE_ULTRA_SHEN7_QI))
				{
					charstate.pdamage    += DWORD( 50000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 50000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 50000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 50000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 30000 );       //物防
		            charstate.mdefence += DWORD( 30000 );       //魔防
		            charstate.maxhp += DWORD( 500000 );         //最大生命值
				}

				//soke 小龙套装加成效果
	            if (this->issetUState(Cmd::USTATE_USER_LONGWANG_LV1 ))
	            {
		            charstate.pdamage    += DWORD( 3000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 3000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 3000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 3000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 2500 );      //物防
		            charstate.mdefence += DWORD( 2500 );      //魔防
		            charstate.maxhp += DWORD( 30000 );         //最大生命值
					
	            }
				//soke 金龙套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_LONGWANG_LV2  ))
	            {
		            charstate.pdamage    += DWORD( 6000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 6000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 6000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 6000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 5000 );      //物防
		            charstate.mdefence += DWORD( 5000 );      //魔防
		            charstate.maxhp += DWORD( 60000 );         //最大生命值
	            }
				//soke 苍龙套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_LONGWANG_LV3 ))
	            {
		            charstate.pdamage    += DWORD( 9000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 9000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 9000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 9000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 7500 );      //物防
		            charstate.mdefence += DWORD( 7500 );      //魔防
		            charstate.maxhp += DWORD( 90000 );         //最大生命值
	            }
				//soke 巨龙套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_LONGWANG_LV4 ))
	            {
		            charstate.pdamage    += DWORD( 12000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 12000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 12000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 12000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 10000 );       //物防
		            charstate.mdefence += DWORD( 10000 );       //魔防
		            charstate.maxhp += DWORD( 120000 );         //最大生命值
	            }
				//soke 神苍龙套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_LONGWANG_LV5   ))
	            {   
		            charstate.pdamage    += DWORD( 15000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 15000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 15000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 15000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 12500 );       //物防
		            charstate.mdefence += DWORD( 12500 );       //魔防
		            charstate.maxhp += DWORD( 150000 );         //最大生命值
	            }
				//soke 神巨龙套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_LONGWANG_LV6 ))
	            {
		           	charstate.pdamage    += DWORD( 50000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 50000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 50000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 50000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 30000 );       //物防
		            charstate.mdefence += DWORD( 30000 );       //魔防
		            charstate.maxhp += DWORD( 500000 );         //最大生命值
	            }
////////////////////////////////////////////////////////////////////////////////////////////
				 //soke 逆天15阶套装加成效果
	            if (this->issetUState(Cmd::USTATE_USER_NITIAN_LV1 ))
	            {
		            charstate.pdamage    += DWORD( 3000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 3000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 3000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 3000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 2500 );      //物防
		            charstate.mdefence += DWORD( 2500 );      //魔防
		            charstate.maxhp += DWORD( 30000 );         //最大生命值
					
	            }
				//soke 逆天30阶套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_NITIAN_LV2  ))
	            {
		            charstate.pdamage    += DWORD( 6000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 6000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 6000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 6000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 5000 );      //物防
		            charstate.mdefence += DWORD( 5000 );      //魔防
		            charstate.maxhp += DWORD( 60000 );         //最大生命值
	            }
				//soke 逆天50阶套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_NITIAN_LV3 ))
	            {
		            charstate.pdamage    += DWORD( 9000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 9000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 9000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 9000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 7500 );      //物防
		            charstate.mdefence += DWORD( 7500 );      //魔防
		            charstate.maxhp += DWORD( 90000 );         //最大生命值
	            }
				//soke 逆天80阶套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_NITIAN_LV4 ))
	            {
		            charstate.pdamage    += DWORD( 12000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 12000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 12000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 12000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 10000 );       //物防
		            charstate.mdefence += DWORD( 10000 );       //魔防
		            charstate.maxhp += DWORD( 120000 );         //最大生命值
	            }
				//soke 逆天100阶龙套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_NITIAN_LV5   ))
	            {   
		          	charstate.pdamage    += DWORD( 30000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 30000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 30000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 30000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 15000 );       //物防
		            charstate.mdefence += DWORD( 15000 );       //魔防
		            charstate.maxhp += DWORD( 300000 );         //最大生命值
	            }	
///////////////////////////////////////////////////////////////////////////////////////////////
                //soke 神魂宝石(斗战圣佛)25级套装加成效果
	            if (this->issetUState(Cmd::USTATE_USER_DOUZHANSF_LV1 ))
	            {
		            charstate.pdamage    += DWORD( 3000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 3000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 3000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 3000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 2500 );      //物防
		            charstate.mdefence += DWORD( 2500 );      //魔防
		            charstate.maxhp += DWORD( 30000 );         //最大生命值
					
	            }
				//soke 神魂宝石(斗战圣佛)50级套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_DOUZHANSF_LV2  ))
	            {
		            charstate.pdamage    += DWORD( 6000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 6000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 6000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 6000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 5000 );      //物防
		            charstate.mdefence += DWORD( 5000 );      //魔防
		            charstate.maxhp += DWORD( 60000 );         //最大生命值
	            }
				//soke 神魂宝石(斗战圣佛)75级套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_DOUZHANSF_LV3 ))
	            {
		            charstate.pdamage    += DWORD( 9000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 9000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 9000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 9000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 7500 );      //物防
		            charstate.mdefence += DWORD( 7500 );      //魔防
		            charstate.maxhp += DWORD( 90000 );         //最大生命值
	            }
				//soke 神魂宝石(斗战圣佛)100级套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_DOUZHANSF_LV4 ))
	            {
		          	charstate.pdamage    += DWORD( 30000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 30000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 30000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 30000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 15000 );       //物防
		            charstate.mdefence += DWORD( 15000 );       //魔防
		            charstate.maxhp += DWORD( 300000 );         //最大生命值
	            }
////////////////////////////////////////////////////////////////////////////////////////////////
				//soke 暗影宝石20级套装加成效果
	            if (this->issetUState(Cmd::USTATE_USER_AYSTONE_LV1 ))
	            {
		            charstate.pdamage    += DWORD( 3000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 3000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 3000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 3000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 2500 );      //物防
		            charstate.mdefence += DWORD( 2500 );      //魔防
		            charstate.maxhp += DWORD( 30000 );         //最大生命值
					
	            }
				//soke 暗影宝石40级套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_AYSTONE_LV2  ))
	            {
		            charstate.pdamage    += DWORD( 6000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 6000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 6000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 6000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 5000 );      //物防
		            charstate.mdefence += DWORD( 5000 );      //魔防
		            charstate.maxhp += DWORD( 60000 );         //最大生命值
	            }
				//soke 暗影宝石60级套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_AYSTONE_LV3 ))
	            {
		            charstate.pdamage    += DWORD( 9000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 9000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 9000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 9000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 7500 );      //物防
		            charstate.mdefence += DWORD( 7500 );      //魔防
		            charstate.maxhp += DWORD( 90000 );         //最大生命值
	            }
				//soke 暗影宝石80级套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_AYSTONE_LV4 ))
	            {
		            charstate.pdamage    += DWORD( 12000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 12000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 12000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 12000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 10000 );       //物防
		            charstate.mdefence += DWORD( 10000 );       //魔防
		            charstate.maxhp += DWORD( 120000 );         //最大生命值
	            }
				//soke 暗影宝石100级龙套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_AYSTONE_LV5   ))
	            {   
		          	charstate.pdamage    += DWORD( 30000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 30000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 30000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 30000 );    //最大魔法攻击力
		            charstate.pdefence += DWORD( 15000 );       //物防
		            charstate.mdefence += DWORD( 15000 );       //魔防
		            charstate.maxhp += DWORD( 300000 );         //最大生命值
	            }	


				
						
////////////////////////////////////////////////////////////////////////////////////////////////
				//soke 龙凤吟8级套装加成效果
	            if (this->issetUState(Cmd::USTATE_USER_LFY_LV1 ))
	            {
		            charstate.pdamage    += DWORD( 160000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 160000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 160000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 160000 );    //最大魔法攻击力
		  
					
	            }
				//soke 龙凤吟10级套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_LFY_LV2  ))
	            {
		            charstate.pdamage    += DWORD( 100000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 100000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 100000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 100000 );    //最大魔法攻击力
		      
	            }
				//soke 龙凤吟12级套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_LFY_LV3 ))
	            {
		            charstate.pdamage    += DWORD( 240000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 240000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 240000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 240000 );    //最大魔法攻击力
		         
	            }
				//soke 龙凤吟14级套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_LFY_LV4 ))
	            {
		            charstate.pdamage    += DWORD( 280000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 280000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 280000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 280000 );    //最大魔法攻击力
		          
	            }
				//soke 龙凤吟16级套装加成效果
	            else if (this->issetUState(Cmd::USTATE_USER_LFY_LV5   ))
	            {   
		           	charstate.pdamage    += DWORD( 320000 );    //最小物理攻击力
		            charstate.maxpdamage += DWORD( 320000 );    //最大物理攻击力
		            charstate.mdamage    += DWORD( 320000 );    //最小魔法攻击力
		            charstate.maxmdamage += DWORD( 320000 );    //最大魔法攻击力
		         
	            }				
	        }

	charstate.maxhp= charstate.maxhp+packs.equip.getEquips().get_maxhp()+skillValue.maxhp+skillValue.sept_maxhp+skillValue.pmaxhp+skillValue.x1_pmaxhp+skillValue.x2_pmaxhp+skillValue.x3_pmaxhp+skillValue.x4_pmaxhp+skillValue.x5_pmaxhp+skillValue.hpupbylevel*charbase.level+skillValue.introject_maxhp;
	charstate.maxhp += (DWORD)(charstate.maxhp * ((packs.equip.getEquips().get_maxhprate())/100.0f)); //血量百分比 
	if (horse.pkData.maxhp) charstate.maxhp += horse.pkData.maxhp;//高级战马
	if(charbase.hp > charstate.maxhp) charbase.hp = charstate.maxhp;
	charstate.maxmp=charstate.maxmp+packs.equip.getEquips().get_maxmp()+skillValue.maxmp+skillValue.sept_maxmp;
	charstate.maxmp += (DWORD)(charstate.maxmp * packs.equip.getEquips().get_maxmprate()/100.0f);
	if (horse.pkData.maxmp)	charstate.maxmp += horse.pkData.maxmp;//高级战马
	if(charbase.mp > charstate.maxmp) charbase.mp = charstate.maxmp;
	charstate.maxsp=charstate.maxsp+packs.equip.getEquips().get_maxsp()+skillValue.maxsp;

	DWORD temp=0; 


	
	// add by snip3r
	DWORD equips_max_pdam = (DWORD)(packs.equip.getEquips().get_maxpdamage() * (100+skillValue.physic_att)/100.0f);
	DWORD equips_min_pdam = (DWORD)(packs.equip.getEquips().get_pdamage() * (100+skillValue.physic_att)/100.0f);
	DWORD equips_max_mdam = (DWORD)(packs.equip.getEquips().get_maxmdamage() * (100+skillValue.magic_att)/100.0f);
	DWORD equips_min_mdam = (DWORD)(packs.equip.getEquips().get_mdamage()* (100+skillValue.magic_att)/100.0f);


	DWORD cikewu = (DWORD)(packs.equip.getEquips().get_maxpdamage() * (100+skillValue.cikewu)/100.0f);
	DWORD cikemo = (DWORD)(packs.equip.getEquips().get_maxpdamage() * (100+skillValue.cikemo)/100.0f);
	
	
	// end add by snip3r
	// 最大物理攻击力
	temp = (DWORD)((charstate.pdamage+equips_max_pdam+cikewu/*packs.equip.getEquips().get_maxpdamage()*/)*(1+packs.equip.getEquips().get_pdam()/100.0f)*
		(1.0f+((int)skillValue.updamp+(int)skillValue.theurgy_updamp+(int)skillValue.erupt+(int)skillValue.lm_updamp+(int)skillValue.nsc_updamp+(int)skillValue.sept_updamp+(int)skillValue.array_udamp+(int)skillValue.spupdamp+horse.pkData.pdam-(int)skillValue.dpdamp)/100.0f)+skillValue.introject_maxpdam+skillValue.updam+
			skillValue.theurgy_updam+skillValue.lupdam+skillValue.pupdam+skillValue.x1_pupdam+skillValue.x2_pupdam+skillValue.x3_pupdam+skillValue.spupdam+skillValue.supdam+skillValue.pdeftodam+(int)skillValue.sword_udam-skillValue.pdamtodef
			+skillValue.rpupdam-skillValue.dpdam-skillValue.theurgy_dpdam+packs.equip.getEquips().get_maxpdamage()*(skillValue.weaponupdamp/100.0f)+packs.equip.getEquips().get_maxpdamage()*(skillValue.weaponupdamp2/100.0f));
	charstate.maxpdamage = (temp<0?0:temp)+this->team.getAttPlus() - (this->dropweapon?getWeaponPower(1):0);
	

#ifdef _DEBUGLOG
	Zebra::logger->info("charstate.maxpdamage=[%u] temp=[%d]" , charstate.maxpdamage,temp);
#endif
	// 最大魔法攻击力
	temp = (DWORD)((charstate.mdamage+equips_max_mdam+cikemo/*packs.equip.getEquips().get_maxmdamage()*/)*(1+packs.equip.getEquips().get_mdam()/100.0f)*
			(1.0f+((int)skillValue.umdamp+(int)skillValue.pumdamp+(int)skillValue.theurgy_umdamp+(int)skillValue.erupt+(int)skillValue.array_udamp+(int)skillValue.sept_umdamp+horse.pkData.mdam-(int)skillValue.dmdamp)/100.0f)+skillValue.introject_maxmdam+skillValue.umdam+skillValue.theurgy_umdam+skillValue.mdeftodam-skillValue.mdamtodef+
			skillValue.pumdam+skillValue.x1_pumdam+skillValue.x2_pumdam+skillValue.x3_pumdam-skillValue.dmdam-skillValue.theurgy_dmdam+packs.equip.getEquips().get_maxmdamage()*(skillValue.weaponumdamp/100.0f)+packs.equip.getEquips().get_maxmdamage()*(skillValue.weaponumdamp2/100.0f));
	charstate.maxmdamage = (temp<0?0:temp)+this->team.getAttPlus() - (this->dropweapon?getWeaponPower(3):0);
	
	// 最小物理攻击力
	temp = (DWORD)((charstate.pdamage+equips_min_pdam/*packs.equip.getEquips().get_pdamage()*/)*(1+packs.equip.getEquips().get_pdam()/100.0f)*
		(1.0f+((int)skillValue.updamp+(int)skillValue.theurgy_updamp+(int)skillValue.erupt+(int)skillValue.lm_updamp+(int)skillValue.nsc_updamp+(int)skillValue.array_udamp+(int)skillValue.sept_updamp+(int)skillValue.spupdamp+horse.pkData.pdam-(int)skillValue.dpdamp)/100.0f)+skillValue.introject_pdam+skillValue.updam+
			skillValue.theurgy_updam+skillValue.lupdam+skillValue.pupdam+skillValue.x1_pupdam+skillValue.x2_pupdam+skillValue.x3_pupdam+skillValue.spupdam+skillValue.supdam+skillValue.pdeftodam+(int)skillValue.sword_udam-skillValue.pdamtodef
			+skillValue.rpupdam-skillValue.dpdam-skillValue.theurgy_dpdam+packs.equip.getEquips().get_pdamage()*(skillValue.weaponupdamp/100.0f)+packs.equip.getEquips().get_pdamage()*(skillValue.weaponupdamp2/100.0f));
	charstate.pdamage = (temp<0?0:temp)+this->team.getAttPlus() - (this->dropweapon?getWeaponPower(0):0);
	// 最小魔法攻击力
	temp = (DWORD)((charstate.mdamage+equips_min_mdam/*packs.equip.getEquips().get_mdamage()*/)*(1+packs.equip.getEquips().get_mdam()/100.0f)*
			(1.0f+((int)skillValue.umdamp+(int)skillValue.pumdamp+(int)skillValue.theurgy_umdamp+(int)skillValue.erupt+(int)skillValue.array_udamp+(int)skillValue.sept_umdamp+horse.pkData.mdam-(int)skillValue.dmdamp)/100.0f)+skillValue.introject_mdam+skillValue.umdam+skillValue.theurgy_umdam+skillValue.mdeftodam-skillValue.mdamtodef+
			skillValue.pumdam+skillValue.x1_pumdam+skillValue.x2_pumdam+skillValue.x3_pumdam-skillValue.dmdam-skillValue.theurgy_dmdam+packs.equip.getEquips().get_mdamage()*(skillValue.weaponumdamp/100.0f)+packs.equip.getEquips().get_mdamage()*(skillValue.weaponumdamp2/100.0f));
	charstate.mdamage = (temp<0?0:temp)+this->team.getAttPlus() - (this->dropweapon?getWeaponPower(2):0);
	// 新增物防魔防
	DWORD equips_pdef = (SDWORD)(packs.equip.getEquips().get_pdefence() * (100 + skillValue.physic_prt) / 100.0f); //327物防
	DWORD equips_mdef = (SDWORD)(packs.equip.getEquips().get_mdefence() * (100 + skillValue.magic_prt) / 100.0f);  //329魔防
	// end add by snip3r

    // 魔法防御力
	temp = (DWORD)((charstate.mdefence+equips_mdef/*packs.equip.getEquips().get_mdefence()*/)*(1+packs.equip.getEquips().get_mdef()/100.0f)*
			(1.0f+((int)skillValue.umdefp+(int)skillValue.theurgy_umdefp+(int)skillValue.ice_umdefp+(int)skillValue.ice_umdefp2+(int)skillValue.sept_umdefp+horse.pkData.mdef-(int)skillValue.array_ddefp-(int)skillValue.dmdefp)/100.0f)+skillValue.introject_mdef+skillValue.umdef+skillValue.theurgy_umdef+
			skillValue.pumdef+skillValue.x1_pumdef+skillValue.x2_pumdef+skillValue.x3_pumdef+skillValue.udef-skillValue.dmdef-skillValue.theurgy_dmdef+skillValue.mdamtodef-skillValue.mdeftodam);
	charstate.mdefence = (temp<0?0:temp)+this->team.getDefPlus();
	if (this->issetUState(Cmd::USTATE_SITDOWN)) charstate.mdefence = (DWORD)(((float)(charstate.mdefence))*0.6f);

	
	 // 物理防御力
	temp = (DWORD)((charstate.pdefence+equips_pdef/*packs.equip.getEquips().get_pdefence()*/)*(1+packs.equip.getEquips().get_pdef()/100.0f)*
			(1.0f+((int)skillValue.updefp2+(int)skillValue.updefp+(int)skillValue.tgyt_updefp+(int)skillValue.sept_updefp+(int)skillValue.theurgy_updefp+horse.pkData.pdef-(int)skillValue.array_ddefp-(int)skillValue.dpdefp)/100.0f)+skillValue.introject_pdef+skillValue.updef+skillValue.theurgy_updef+
			skillValue.pupdef+skillValue.x1_pupdef+skillValue.x2_pupdef+skillValue.x3_pupdef+skillValue.udef+(int)skillValue.tgzh_updef-skillValue.dpdef-skillValue.theurgy_dpdef+skillValue.pdamtodef-skillValue.pdeftodam);
	charstate.pdefence = (temp<0?0:temp)+this->team.getDefPlus();
	if (this->issetUState(Cmd::USTATE_SITDOWN)) charstate.pdefence = (DWORD)(((float)(charstate.pdefence))*0.6f);
	
	charstate.resumehp		=	(DWORD)((charstate.resumehp*(skillValue.hpspeedup==0?1:(1+skillValue.hpspeedup/100.0f)) + packs.equip.getEquips().get_hpr())*(skillValue.enervation==0?1:(skillValue.enervation/100.0f)));
	charstate.resumemp		=	(DWORD)((charstate.resumemp*(skillValue.mpspeedup==0?1:(1+skillValue.mpspeedup/100.0f)) + packs.equip.getEquips().get_mpr())*(skillValue.enervation==0?1:(skillValue.enervation/100.0f)));
	charstate.resumesp		=	(DWORD)((charstate.resumesp*(skillValue.spspeedup==0?1:(1+skillValue.spspeedup/100.0f)) + packs.equip.getEquips().get_spr())*(skillValue.enervation==0?1:(skillValue.enervation/100.0f)));

	// 攻击速度
	if ((int)charstate.attackspeed - (int)packs.equip.getEquips().get_akspeed() - (int)skillValue.pattackspeed - (int)(charstate.wdMen*0.4f)>0)
	{
		charstate.attackspeed= (WORD)((int)charstate.attackspeed - (int)packs.equip.getEquips().get_akspeed() - (int)skillValue.pattackspeed - (int)(charstate.wdMen*0.4f));
		charstate.attackspeed = (WORD)(charstate.attackspeed*(1+((int)skillValue.dattackspeed+(int)skillValue.enervation-(int)skillValue.uattackspeed)/100.0f));
	}
	else
	{
		charstate.attackspeed = 0;
	}

	// 移动速度
	if ((int)charstate.movespeed -  (int)packs.equip.getEquips().get_mvspeed()>=0)
	{
		charstate.movespeed	=	(WORD)(((int)charstate.movespeed - (int)packs.equip.getEquips().get_mvspeed())*(skillValue.enervation==0?1:(1+skillValue.enervation/100.0f)));
	}
	else
	{
		charstate.movespeed	= 0;
	}
	//高级战马
	if (horse.pkData.speed)
	{
		if (charstate.movespeed>horse.pkData.speed)
			charstate.movespeed -= horse.pkData.speed;
		else
			charstate.movespeed = 0;
	}
	//Zebra::logger->debug("移动速度:%d",getMyMoveSpeed());
	//高级战马
	/*if (horse.pkData.speed)
	{
		if (charstate.movespeed>horse.pkData.speed)
			charstate.movespeed -= horse.pkData.speed;
		else
			charstate.movespeed = 0;
	}*/

	//TODO 加入装备魅力值

	if(this->issetUState(333))
	{
		charstate.movespeed = 640;
	}


	setupCharm();
	charstate.lucky=charbase.lucky;


	/*
	   命中率={98+（攻击方等级 – 防御方等级）*0.5+装备增加或减少（此时为 -）命中点数+技能增加或减少（此时为 -）命中点数）}/100
	   */
	charstate.attackrating=98+packs.equip.getEquips().get_atrating() +skillValue.atrating + skillValue.satrating + skillValue.patrating - skillValue.reduce_atrating ;

	/*
	   闪避率={10+（防御方等级 – 攻击方等级）*2+装备增加闪避点数+技能增加或减少（此时为 -）闪避点数）}/100
	   */
	charstate.attackdodge=10+packs.equip.getEquips().get_akdodge()+skillValue.akdodge - skillValue.reduce_akdodge;
	if (charstate.attackdodge >25) charstate.attackdodge = 25;
	charstate.bang = charstate.bang + packs.equip.getEquips().get_bang() + skillValue.bang;

	if (this->issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
	{//龙精附体，暴击提高100%
		charstate.bang = charstate.bang + charstate.bang;
	}

	

	//soke 转生经验读取
	if (this->charbase.round > 0)
	{
	   //升级经验值
	   zZhuanExperienceB *base_exp = zhuanexperiencebm.get(charbase.level);
	   if(base_exp)
	   {
		  charstate.nextexp = base_exp->nextexp;
	   }
    }
	else
	{
	   //soke 未转生经验读取
	   zExperienceB *base_exp = experiencebm.get(charbase.level);
	   if(base_exp)
	   {
		  charstate.nextexp = base_exp->nextexp;
	   }
    }

	charstate.attackfive = (BYTE)this->getFivePoint();//packs.equip.getEquips().getAttFive();
	charstate.defencefive = (BYTE)this->getFivePoint();//packs.equip.getEquips().getDefFive();

	using namespace Cmd;
	if (charbase.hp>charstate.maxhp) charbase.hp = charstate.maxhp;
	if (charbase.mp>charstate.maxmp) charbase.mp = charstate.maxmp;

	packs.equip.needRecalc=false;

	charbase.zhanli = (charstate.maxpdamage+charstate.maxmdamage+charstate.pdefence+charstate.mdefence)/18;//add 2023-04-10 战力
	//Zebra::logger->debug("马的速度加成:%d——人物移动速度:%u",horse.pkData.speed,getMyMoveSpeed() );
	//Zebra::logger->debug("战力:%d",charbase.zhanli);

	charbase.m13axhp = packs.equip.getEquips().get_m13axhp();// 百兽图鉴 最大生命值
	charbase.p13damage = packs.equip.getEquips().get_p13damage();// 百兽图鉴 最小攻击力
	charbase.m13axpdamage = packs.equip.getEquips().get_m13axpdamage();/// 百兽图鉴 最大攻击力
	charbase.m13damage = packs.equip.getEquips().get_m13damage();/// 百兽图鉴 最小法术攻击力
	charbase.m13axmdamage = packs.equip.getEquips().get_m13axmdamage();// 百兽图鉴 最大法术攻击力
	charbase.p13defence = packs.equip.getEquips().get_p13defence();// 百兽图鉴 物防
	charbase.m13defence = packs.equip.getEquips().get_m13defence();// 百兽图鉴 魔防
	//装备改变攻击力预处理

	calPreValue();
}

/**
 * \brief 设置魅力值
 *
 */
void SceneUser::setupCharm()
{
	charstate.charm = (WORD)(charbase.level / 15);
}

/**
 * \brief 计算魔法、物理的伤害和防御值
 *
 * 通过装备等信息，计算出相关的伤害和防御值
 *
 */
//soke 突破属性  攻击的时候计算的不对。。。。
void SceneUser::calPreValue()
{
	   //装备改变攻击力预处理
	    pkpreValue.fiveexpress = 1 + (float)(charstate.attackfive/100.0f);
	    float five_def_express = 1 + (float)(charstate.defencefive/100.0f);

	    //soke 突破防御65535限制
	    pkpreValue.fivedam = (DWORD)(charstate.pdamage * pkpreValue.fiveexpress); 
	    pkpreValue.fivemaxdam = (DWORD)(charstate.maxpdamage  * pkpreValue.fiveexpress);

	    pkpreValue.nofivedam = charstate.pdamage;
	    pkpreValue.nofivemaxdam = charstate.maxpdamage;

	    pkpreValue.fivedef	= (DWORD)(charstate.pdefence * five_def_express);
	    pkpreValue.nofivedef = charstate.pdefence;

	    pkpreValue.fivemdam = (DWORD)(charstate.mdamage * pkpreValue.fiveexpress); 
	    pkpreValue.fivemaxmdam = (DWORD)(charstate.maxmdamage  * pkpreValue.fiveexpress);

	    pkpreValue.nofivemdam = charstate.mdamage;
	    pkpreValue.nofivemaxmdam = charstate.maxmdamage;

	    pkpreValue.fivemdef	= (DWORD)(charstate.mdefence * five_def_express);
	    pkpreValue.nofivemdef = charstate.mdefence;
        //修复精神技能问题
        pkpreValue.nofivewdstr = charstate.wdStr;
		pkpreValue.nofivewddex = charstate.wdDex;
		pkpreValue.nofivewdint = charstate.wdInt;
		pkpreValue.nofivewdmen = charstate.wdMen;
		pkpreValue.nofivewdcon = charstate.wdCon;

        #ifdef _DEBUGLOG 
	        Zebra::logger->debug("calPreValue():fiveexpress=%f",pkpreValue.fiveexpress);
	        Zebra::logger->debug("calPreValue():five_def_express=%f",five_def_express);
        #endif
}

/**
 * \brief 打包发送玩家的所有物品信息给自己
 *
 *
 */
class sendAllObjectListToUser:public UserObjectExec
{
	public:
		SceneUser *pUser;
		BYTE action;
		char buffer[zSocket::MAX_USERDATASIZE];
		Cmd::stAddObjectListPropertyUserCmd *send;
		sendAllObjectListToUser(SceneUser *u,BYTE act):pUser(u),action(act)
		{
			send = (Cmd::stAddObjectListPropertyUserCmd *)buffer;
			constructInPlace(send);
		}
		bool exec(zObject *object)
		{
			if(sizeof(Cmd::stAddObjectListPropertyUserCmd) + (send->num + 1) * sizeof(send->list[0]) >= sizeof(buffer))
			{
				pUser->sendCmdToMe(send,sizeof(Cmd::stAddObjectListPropertyUserCmd) + send->num * sizeof(send->list[0]));
				send->num=0;
			}
			bcopy(&object->data,&send->list[send->num].object,sizeof(t_Object));
			send->num++;
			return true;
		}
};
/**
 * \brief 发送玩家的所有物品信息给自己
 *
 *
 */
class sendAllObjectToUser:public UserObjectExec
{
	public:
		SceneUser *pUser;
		Cmd::stAddObjectPropertyUserCmd send;
		bool exec(zObject *object)
		{
			bcopy(&object->data,&send.object,sizeof(t_Object));
			pUser->sendCmdToMe(&send,sizeof(send));
			//Zebra::logger->debug("发送物品 %s",object->name);
			return true;
		}
};

/*
 * \brief 判断相克
 * 
 * \five 五行
 * \return  0,无相克关系, 1,克对方, 2,对方克自己
 */
int SceneUser::IsOppose(DWORD five)
{
	if(this->getFiveType() == SceneUser::Five_Relation[(five + 4)%5])
	{
		return 1;
	}
	else
	{
		if(this->getFiveType() == SceneUser::Five_Relation[(five + 1)%5])
		{
			return 2;
		}
	}

	if ((this->getFiveType() != 5) &&(five == 5))
		return 1;
	if ((this->getFiveType() == 5) &&(five != 5))
		return 2;

	return 0;
}

/*
 * \brief 判断相生
 * 
 * \five 五行
 * \return 是否相生
 */
bool SceneUser::IsJoin(DWORD five)
{
	return five == SceneUser::Five_Relation[(this->getFiveType() + 2)%5] || five == SceneUser::Five_Relation[(this->getFiveType() + 3)%5];
}

/**
 * \brief 发送玩家的所有技能给玩家自己
 *
 */
class sendAllSkillToUser :public UserSkillExec
{
	public:
		SceneUser *pUser;
		Cmd::stAddUserSkillPropertyUserCmd ret;
		sendAllSkillToUser(SceneUser *me)
		{
			pUser = me;
		}
		bool exec(zSkill *skill)
		{
			if(skill == NULL)
			{
				return false;
			}
			else
			{
				zRTime ctv;
				DWORD colddown = ctv.msecs() - skill->lastUseTime;
				if (colddown >= skill->base->dtime)
					colddown = 0;
				else
					colddown = skill->base->dtime - colddown;
				ret.dwSkillID = skill->data.skillid;
				ret.wdLevel = skill->data.level;
				ret.wdUpNum = pUser->skillUpLevel(skill->base->skillid, skill->base->kind);
				if (ret.wdUpNum+ret.wdLevel>10) ret.wdUpNum = 10 - ret.wdLevel;
				ret.dwExperience = colddown;
				ret.dwMaxExperience = 0;
				pUser->sendCmdToMe(&ret , sizeof(ret));
			}
			return true;
		}

};

/**
 * \brief 发送玩家所有信息给玩家自己
 *
 *
 */
void SceneUser::sendInitToMe()
{
	using namespace Cmd;

	// add by snip3r
	// 初始化技能点，并更新初始属性
	setupCharBase();
	charbase.hp = charstate.maxhp;
	charbase.mp = charstate.maxmp;
	charbase.sp = charstate.maxsp;
	// end add by snip3r

	//主人物信息
	stMainUserDataUserCmd  userinfo;
	full_t_MainUserData(userinfo.data);
	sendCmdToMe(&userinfo,sizeof(userinfo));
	sendInitHPAndMp();

	packs.store.notify(this);

	//物品信息
#if 0
	sendAllObjectToUser sendao;
	sendao.send.byActionType=EQUIPACTION_INIT;
	sendao.pUser=this;
	packs.uom.execEvery(sendao);
#else
	sendAllObjectListToUser sendao(this,EQUIPACTION_INIT);
	packs.uom.execEvery(sendao);
	if(sendao.send->num)
	{
		this->sendCmdToMe(sendao.send,sizeof(Cmd::stAddObjectListPropertyUserCmd) + sendao.send->num * sizeof(sendao.send->list[0]));
		sendao.send->num=0;
	}
#endif

	//技能信息
	sendAllSkillToUser sexec(this);
	usm.execEvery(sexec);

	//quest info
	Quest::notify(*this);

	//快捷键信息
	char Buf[1024];
	bzero(Buf ,1024);

	Cmd::stAccekKeyPropertyUserCmd *acc = (Cmd::stAccekKeyPropertyUserCmd *)Buf;//&accelData;
	constructInPlace(acc);
	Cmd::stAccekKeyPropertyUserCmd *acc_1 = (Cmd::stAccekKeyPropertyUserCmd *)accelData;
	acc->accelNum = acc_1->accelNum;
	acc->activeGroup = acc_1->activeGroup;
	int len = sizeof(Cmd::stAccekKeyPropertyUserCmd) + acc->accelNum * sizeof(Cmd::stGameAccelKey);
	if(acc->accelNum > 0 && len < 1024)
	{
		memcpy(acc->accelKeys , acc_1->accelKeys , acc->accelNum * sizeof(Cmd::stGameAccelKey));
		sendCmdToMe(acc , len);
	}

	//请求临时档案数据
	Cmd::Session::t_ReqUser_SceneArchive req;
	req.id = this->id;
	req.dwMapTempID = this->scene->tempid;
	sessionClient->sendCmd(&req , sizeof(req));

	if (this->safety)
	{
		Cmd::stNotifySafetyUserCmd send;
		send.byState = Cmd::SAFETY_OPEN;
		send.safe_setup = this->safety_setup;
		sendCmdToMe(&send, sizeof(send));
	}

	//结束初始化
	stEndOfInitDataDataUserCmd  endcmd;
	sendCmdToMe(&endcmd,sizeof(endcmd));

}


/*
 * \brief 通知9屏添加用户
 *
 * \author zhongjunwei
 *
 */
void SceneUser::sendMeToNine()
{
	if (SceneEntry_Hide!=getState()&&!this->hideme)
	{//检查是否隐身
		clearUState( Cmd::USTATE_HIDE );
		BUFFER_CMD(Cmd::stAddUserAndPosMapScreenStateUserCmd ,send , zSocket::MAX_USERDATASIZE);
		this->full_t_MapUserDataPosState(send->data);
		this->scene->sendCmdToNine(getPosI(), send,send->size());
		this->setStateToNine(Cmd::USTATE_WAR);
	}
	else
	{
		BUFFER_CMD(Cmd::stAddUserAndPosMapScreenStateUserCmd ,send , zSocket::MAX_USERDATASIZE);
		this->full_t_MapUserDataPosState(send->data);
		sendCmdToMe(send,send->size());
	}

}

struct SendStateWarToNineEveryOne : public zSceneEntryCallBack
{
	private:
		SceneUser *pUser;
		Cmd::stSetStateMapScreenUserCmd send;
	public:
		SendStateWarToNineEveryOne(SceneUser *pUser,WORD state) : pUser(pUser)
		{
			send.dwTempID = pUser->tempid;
			send.type=Cmd::MAPDATATYPE_USER;
			send.wdState =state;
		}
		bool exec(zSceneEntry *entry)
		{
			if (pUser && entry)
			{
				if (pUser->isWar((SceneUser*)entry))
				{
#if _DEBUGLOG
			Zebra::logger->trace("角色%s将自己的对战状态发给了%s, %u", pUser->name, entry->name, send.wdState);
#endif
					((SceneUser*)entry)->sendCmdToMe(&send, sizeof(send));
				}
			}
			return true;
		}
};
void SceneUser::sendMeToNineDirect(const int direct)
{
	if (SceneEntry_Hide!=getState()&& !this->hideme)
	{
		BUFFER_CMD(Cmd::stAddUserAndPosMapScreenStateUserCmd ,send , zSocket::MAX_USERDATASIZE);
		this->full_t_MapUserDataPosState(send->data);
		this->scene->sendCmdToDirect(getPosI(),direct, send,send->size());

		Cmd::stSetStateMapScreenUserCmd send1;
		send1.type=Cmd::MAPDATATYPE_USER;
		send1.dwTempID = this->tempid;
		send1.wdState =Cmd::USTATE_WAR;
		SendStateWarToNineEveryOne one(this,send1.wdState);
		const zPosIVector &pv = scene->getDirectScreen(getPosI(), direct);
		for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
		{
			scene->execAllOfScreen(zSceneEntry::SceneEntry_Player, *it,one);
		}
	}
	else
	{
		BUFFER_CMD(Cmd::stAddUserAndPosMapScreenStateUserCmd ,send , zSocket::MAX_USERDATASIZE);
		this->full_t_MapUserDataPosState(send->data);
		sendCmdToMe(send,send->size());
		this->scene->sendCmdToWatchTrap(getPosI(), send, send->size());
	}


}

bool SceneUser::isSpecWar(DWORD dwType)
{
	WarIter iter;
	for (iter = vWars.begin(); iter!=vWars.end(); iter++)
	{       
		if (iter->type == dwType || (dwType == Cmd::COUNTRY_FORMAL_DARE && iter->type==Cmd::COUNTRY_FORMAL_ANTI_DARE))
		{
			break;
		} 
	}       

	if (iter!=vWars.end())
	{       
		return true;
	}

	return false;
}

/**
 * \brief 判断自己与某玩家是否处于对战状态
 *
 *
 * \param entry 对方玩家
 *
 * \return 只要与对方玩家处于（帮会战，师门战，家族战）之一时，返回TRUE,否则为FALSE
 *
 */
bool SceneUser::isWar(SceneUser* entry)
{
	if (this == entry)
	{// 过滤自己
		return false;
	}
	
	WarIter iter;
	for (iter = vWars.begin(); iter!=vWars.end(); iter++)
	{       
		if (entry->charbase.septid == iter->relationid && (iter->type == Cmd::SEPT_DARE))
		{//判断有没有家族战
			return true;
		}

		if (entry->charbase.septid == iter->relationid && (iter->type == Cmd::SEPT_NPC_DARE))
		{//判断有没有家族NPC争夺战
			if (entry->npcdareflag && this->npcdareflag) return true;
		}

		if (entry->charbase.unionid == iter->relationid && 
				(iter->type == Cmd::UNION_DARE || iter->type == Cmd::UNION_CITY_DARE))
		{//判断有没有帮会战,帮会夺城战
			return true;
		}
		
		if (entry->charbase.country == iter->relationid && 
				(iter->type == Cmd::COUNTRY_FORMAL_DARE || iter->type == Cmd::COUNTRY_FORMAL_ANTI_DARE))
		{//判断有没有正式国战,国战反攻
			return true;
		}
	}       

	return false;
}

struct GetEnvryOneAddSeptNormalExp : public zSceneEntryCallBack
{
	private:
		DWORD dwSeptID;
		DWORD counter;

	public:
		bool  isTotal;

		GetEnvryOneAddSeptNormalExp(DWORD septid)
		{
			dwSeptID = septid;
			isTotal = true;
			counter = 0;
		}

		inline void total()
		{//统计九屏内该家族的人数
			counter++;
		}

		bool exec(zSceneEntry *entry)
		{
			if (entry && dwSeptID==((SceneUser*)entry)->charbase.septid)
			{
				if (isTotal)
				{
					this->total();
				}
				else
				{
					SceneUser* pUser = (SceneUser*)entry;
					//10*(1.4*主人角色当前等级^2+5*主人角色当前等级) 	
					DWORD exp = 0;
					
					if (pUser->septMaster)
					{
						exp = (DWORD)((50+3*counter) * pUser->charbase.level*pUser->charbase.level);
					}
					else
					{
						exp = (DWORD)((25+3*counter)*pUser->charbase.level*pUser->charbase.level);
					}

					pUser->addExp(exp);

					Channel::sendSys(pUser, Cmd::INFO_TYPE_EXP, "你周围有 %u 个族员, 领取到家族经验 %d", 
							counter, exp);
					Zebra::logger->trace("[家族]: %s(%u) 领取家族经验 %d", pUser->name, pUser->id, exp);
				}
			}

			return true;
		}
};

struct GetEnvryOneAddSeptExp : public zSceneEntryCallBack
{
	private:
		DWORD dwSeptID;

	public:
		GetEnvryOneAddSeptExp(DWORD septid)
		{
			dwSeptID = septid;
		}

		bool exec(zSceneEntry *entry)
		{
			if (entry && dwSeptID==((SceneUser*)entry)->charbase.septid)
			{
				SceneUser* pUser = (SceneUser*)entry;
				//10*(1.4*主人角色当前等级^2+5*主人角色当前等级) 	
				DWORD exp = (DWORD)(10*(1.4*pow(pUser->charbase.level,2)+5*pUser->charbase.level));
				pUser->addExp(exp);
				Channel::sendSys(pUser, Cmd::INFO_TYPE_EXP, "领取到家族经验 %d", exp);

				Zebra::logger->trace("[家族]: %s(%u) 领取家族经验 %d", pUser->name, pUser->id, exp);
			}

			return true;
		}
};

#ifdef _POOL_ALLOC_
typedef std::vector<SceneNpc *, __gnu_cxx::__pool_alloc<SceneNpc *> > SceneNpc_vec;
#else
typedef std::vector<SceneNpc * > SceneNpc_vec;
#endif

const unsigned int SCENE_PACKET_USERDATASIZE = zSocket::zSocket::MAX_USERDATASIZE/4;

struct GetEnvryOneAndSend : public zSceneEntryCallBack
{
	private:
		SceneUser *pUser;
	public:
		static BYTE _buf__map_user[SCENE_PACKET_USERDATASIZE];
		static BYTE _buf_map_npc[SCENE_PACKET_USERDATASIZE];
		static SceneNpc_vec _npc_vec;
		//static BYTE _buf_pos_user[SCENE_PACKET_USERDATASIZE];
		//static BYTE _buf_pos_npc[SCENE_PACKET_USERDATASIZE];
		static BYTE petBuf[zSocket::zSocket::MAX_USERDATASIZE];
		Cmd::stMapDataMapScreenUserCmd *_map_user; 
		Cmd::stMapDataMapScreenUserCmd *_map_npc; 
		//Cmd::stAllMapScreenUserCmd *_pos_user; 
		//Cmd::stAllMapScreenUserCmd *_pos_npc; 
		Cmd::stMapDataMapScreenUserCmd *_map_pet;
		void initMapUser()
		{
			_map_user = (Cmd::stMapDataMapScreenUserCmd *)_buf__map_user;
			constructInPlace(_map_user);
			_map_user->mdih.type = Cmd::MAPDATATYPE_USER;
			_map_user->mdih.size=0;
		}
		void initmapNpc()
		{
			_map_npc = (Cmd::stMapDataMapScreenUserCmd *)_buf_map_npc;
			constructInPlace(_map_npc);
			_map_npc->mdih.type = Cmd::MAPDATATYPE_NPC;
			_map_npc->mdih.size=0;
			_npc_vec.clear();
		}
		void initPet()
		{
			_map_pet = (Cmd::stMapDataMapScreenUserCmd *)petBuf;
			constructInPlace(_map_pet);
			_map_pet->mdih.type = Cmd::MAPDATATYPE_PET;
			_map_pet->mdih.size=0;
		}
		/*
		   void initPosUser()
		   {
		//bzero(_buf_pos_user,sizeof(_buf_pos_user));
		_pos_user = (Cmd::stAllMapScreenUserCmd *)_buf_pos_user;
		constructInPlace(_pos_user);
		_pos_user->mdih.type = Cmd::MAPDATATYPE_USER;
		_pos_user->mdih.size=0;
		}
		void initPosNpc()
		{
		//bzero(_buf_pos_npc,sizeof(_buf_pos_npc));
		_pos_npc = (Cmd::stAllMapScreenUserCmd *)_buf_pos_npc;
		constructInPlace(_pos_npc);
		_pos_npc->mdih.type = Cmd::MAPDATATYPE_NPC;
		_pos_npc->mdih.size=0;
		}
		// */
	public:
		GetEnvryOneAndSend(SceneUser *pUser) : pUser(pUser) 
		{
			initMapUser();
			initmapNpc();
			initPet();
			//initPosUser();
			//initPosNpc();
		}
		bool exec(zSceneEntry *entry)
		{
			if (pUser && entry)
			{
				switch(entry->getType())
				{
					case zSceneEntry::SceneEntry_Player:
						{
							if ((entry->getState() == zSceneEntry::SceneEntry_Normal
										|| entry->getState() == zSceneEntry::SceneEntry_Death)
									&& (!((SceneEntryPk *)entry)->hideme
										|| pUser->watchTrap))
							{
								if(user_outofbound())
								{
									pUser->sendCmdToMe(user_getSendBuffer(),user_getSize());
									initMapUser();
								}
								//Cmd::stAddUserMapScreenUserCmd cmd;
								//((SceneUser *)entry)->full_t_MapUserData(cmd.data);
								//_map_user->mud[_map_user->mdih.size] 
								((SceneUser *)entry)->full_t_MapUserData(_map_user->mud[_map_user->mdih.size]);
								_map_user->mud[_map_user->mdih.size].x=entry->getPos().x;
								_map_user->mud[_map_user->mdih.size].y=entry->getPos().y;
								_map_user->mud[_map_user->mdih.size].byDir=entry->getDir();

								if (pUser->isWar((SceneUser*)entry))
								{
									set_state(_map_user->mud[_map_user->mdih.size].state, Cmd::USTATE_WAR);
								}
								else
								{
									clear_state(_map_user->mud[_map_user->mdih.size].state, Cmd::USTATE_WAR);
								}
								_map_user->mdih.size++;

							}
						}
						break;
					case zSceneEntry::SceneEntry_NPC:
						{
							SceneNpc *sceneNpc=(SceneNpc *)entry;
							if (entry->getState() == zSceneEntry::SceneEntry_Normal
									|| (entry->getState() == zSceneEntry::SceneEntry_Death
										&& !sceneNpc->isUse))
							{
								if (sceneNpc->npc->kind == NPC_TYPE_TRAP)
								{
									if (sceneNpc->getMaster() != pUser && !pUser->watchTrap) break;
								}

								if(sceneNpc->isTaskNpc()) 
								{
									sceneNpc->set_quest_status(pUser); //填充任务状态
								}
								if(npc_outofbound())
								{
									pUser->sendCmdToMe(npc_getSendBuffer(),npc_getSize());
									if(!_npc_vec.empty())
									{
										for(SceneNpc_vec::iterator iter = _npc_vec.begin() ; iter != _npc_vec.end() ; iter ++)
										{
											(*iter)->showHP(pUser,(*iter)->hp);
										}
									}
									initmapNpc();
								}
								sceneNpc->full_t_MapNpcData(_map_npc->mnd[_map_npc->mdih.size]);
								_map_npc->mnd[_map_npc->mdih.size].x=entry->getPos().x;
								_map_npc->mnd[_map_npc->mdih.size].y=entry->getPos().y;
								_map_npc->mnd[_map_npc->mdih.size].byDir=entry->getDir();
								_map_npc->mdih.size++;
								if(sceneNpc->isAttackMe(pUser))
								{
									_npc_vec.push_back(sceneNpc); 
								}

								if(sceneNpc->getPetType()!=Cmd::PET_TYPE_NOTPET
										&& sceneNpc->getPetType()!=Cmd::PET_TYPE_SEMI
										&& !sceneNpc->needClear())
								{
									if(pet_outofbound())
									{
										pUser->sendCmdToMe(pet_getSendBuffer(),pet_getSize());
										initPet();
									}
									((ScenePet *)entry)->full_t_MapPetData(_map_pet->mpd[_map_pet->mdih.size]);
									_map_pet->mdih.size++;
								}
							}
						}
						break;
					case zSceneEntry::SceneEntry_Build:
						break;
					case zSceneEntry::SceneEntry_Object:
						{
							zSceneObject *o=(zSceneObject *)entry;
							Cmd::stAddMapObjectMapScreenUserCmd add;

							add.action = Cmd::OBJECTACTION_UPDATE;
							add.data.dwMapObjectTempID=entry->id;
							add.data.dwObjectID=entry->tempid;
							strncpy(add.data.pstrName,entry->name,MAX_NAMESIZE);
							add.data.x=entry->getPos().x;
							add.data.y=entry->getPos().y;
							add.data.wdNumber=o->getObject()->data.dwNum;
							add.data.wdLevel=o->getObject()->base->level;
							add.data.upgrade = o->getObject()->data.upgrade;
							add.data.kind = o->getObject()->data.kind;							
							pUser->sendCmdToMe(&add,sizeof(add));
						}
						break;
					case zSceneEntry::SceneEntry_MAX:
						break;
					default:
						break;
				}
			}

			return true;
		}
		inline const bool user_outofbound() const
		{
			return (sizeof(Cmd::stMapDataMapScreenUserCmd)+ sizeof(Cmd::t_MapUserDataPos)*(_map_user->mdih.size+1) + sizeof(Cmd::MapData_ItemHeader)) >= SCENE_PACKET_USERDATASIZE;
		}
		inline const bool user_canSend() const
		{
			return _map_user->mdih.size > 0;
		}
		inline const BYTE *user_getSendBuffer()
		{
			Cmd::MapData_ItemHeader *pheader =
				(Cmd::MapData_ItemHeader *)&_buf__map_user[sizeof(Cmd::stMapDataMapScreenUserCmd) + sizeof(Cmd::t_MapUserDataPos) * _map_user->mdih.size];
			pheader->size = 0; pheader->type = 0;
			return _buf__map_user;
		}
		inline const int user_getSize() const
		{
			return sizeof(Cmd::stMapDataMapScreenUserCmd)
				+ sizeof(Cmd::t_MapUserDataPos) * _map_user->mdih.size + sizeof(Cmd::MapData_ItemHeader);
		}
		inline const bool npc_outofbound() const
		{
			return (sizeof(Cmd::stMapDataMapScreenUserCmd)+ sizeof(Cmd::t_MapNpcDataPos) *(_map_npc->mdih.size+1) + sizeof(Cmd::MapData_ItemHeader)) >= SCENE_PACKET_USERDATASIZE;
		}
		inline const bool npc_canSend() const
		{
			return _map_npc->mdih.size > 0;
		}
		inline const BYTE *npc_getSendBuffer()
		{
			Cmd::MapData_ItemHeader *pheader =
				(Cmd::MapData_ItemHeader *)&_buf_map_npc[sizeof(Cmd::stMapDataMapScreenUserCmd)+ sizeof(Cmd::t_MapNpcDataPos) * _map_npc->mdih.size];
			pheader->size = 0; pheader->type = 0;
			return _buf_map_npc;
		}
		inline const int npc_getSize() const
		{
			return sizeof(Cmd::stMapDataMapScreenUserCmd)
				+ sizeof(Cmd::t_MapNpcDataPos) * _map_npc->mdih.size + sizeof(Cmd::MapData_ItemHeader);
		}
		inline const bool pet_outofbound() const
		{
			return _map_pet->mdih.size >= 200;
		}
		inline const bool pet_canSend() const
		{
			return _map_pet->mdih.size > 0;
		}
		inline const BYTE *pet_getSendBuffer()
		{
			Cmd::MapData_ItemHeader *pheader =
				(Cmd::MapData_ItemHeader *)&petBuf[sizeof(Cmd::stMapDataMapScreenUserCmd)+sizeof(Cmd::t_MapPetData)*(_map_pet->mdih.size)];
			pheader->size = 0; pheader->type = 0;
			return petBuf;
		}
		inline const int pet_getSize() const
		{
			return sizeof(Cmd::stMapDataMapScreenUserCmd)
				+ sizeof(Cmd::t_MapPetData) * _map_pet->mdih.size + sizeof(Cmd::MapData_ItemHeader);
		}
};
BYTE GetEnvryOneAndSend::_buf__map_user[SCENE_PACKET_USERDATASIZE];
BYTE GetEnvryOneAndSend::_buf_map_npc[SCENE_PACKET_USERDATASIZE];
SceneNpc_vec GetEnvryOneAndSend::_npc_vec;
//BYTE GetEnvryOneAndSend::_buf_pos_user[SCENE_PACKET_USERDATASIZE];
//BYTE GetEnvryOneAndSend::_buf_pos_npc[SCENE_PACKET_USERDATASIZE];
BYTE GetEnvryOneAndSend::petBuf[zSocket::zSocket::MAX_USERDATASIZE];

/**
 * \brief 发送九屏数据给自己
 *
 */
void SceneUser::sendNineToMe()
{
	GetEnvryOneAndSend one(this);
	const zPosIVector &pv = scene->getNineScreen(getPosI());
	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		//Zebra::logger->debug("sceneindex=%d,%d",this->getPosI(),*it);
		scene->execAllOfScreen(*it,one);
	}
	if(one.user_canSend())
	{
		this->sendCmdToMe(one.user_getSendBuffer(),one.user_getSize());
		//Zebra::logger->debug("one._map_user->mdih.size=%d",one._map_user->mdih.size);
	}
	if(one.npc_canSend())
	{
		this->sendCmdToMe(one.npc_getSendBuffer(),one.npc_getSize());
		if(!one._npc_vec.empty())
		{
			for(SceneNpc_vec::iterator iter = one._npc_vec.begin() ; iter != one._npc_vec.end() ; iter ++)
			{
				(*iter)->showHP(this,(*iter)->hp);
			}
		}
		//Zebra::logger->debug("one._map_npc->mdih.size=%d",one._map_npc->mdih.size);
	}

	if(one.pet_canSend())
	{
		this->sendCmdToMe(one.pet_getSendBuffer(),one.pet_getSize());
	}
}

/**
 * \brief 给九屏同家族的所有玩家增加家族经验
 *
 */
void SceneUser::addNineSeptNormalExp(DWORD dwSeptID)
{
	GetEnvryOneAddSeptNormalExp one(dwSeptID);
	const zPosIVector &pv = scene->getNineScreen(getPosI());

	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		scene->execAllOfScreen(SceneEntry_Player, *it, one);
	}
	
	one.isTotal = false;

	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		scene->execAllOfScreen(SceneEntry_Player, *it, one);
	}
}

/**
 * \brief 给九屏同家族的所有玩家增加普通家族经验
 *
 */
void SceneUser::addNineSeptExp(DWORD dwSeptID)
{
	GetEnvryOneAddSeptExp one(dwSeptID);
	const zPosIVector &pv = scene->getNineScreen(getPosI());
	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		scene->execAllOfScreen(SceneEntry_Player,*it,one);
	}
}

/**
 * \brief 发送指定方向五屏数据给自己
 *
 * \param direct 方向
 */
void SceneUser::sendNineToMeDirect(const int direct)
{
	GetEnvryOneAndSend one(this);
	const zPosIVector &pv = scene->getDirectScreen(getPosI(), direct);
	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		//Zebra::logger->debug("%s: %u", __FUNCTION__, *it);
		scene->execAllOfScreen(*it,one);
	}
	if(one.user_canSend())
	{
		this->sendCmdToMe(one.user_getSendBuffer(),one.user_getSize());
		//Zebra::logger->debug("one._map_user->mdih.size=%d",one._map_user->mdih.size);
	}
	if(one.npc_canSend())
	{
		this->sendCmdToMe(one.npc_getSendBuffer(),one.npc_getSize());
		if(!one._npc_vec.empty())
		{
			for(SceneNpc_vec::iterator iter = one._npc_vec.begin() ; iter != one._npc_vec.end() ; iter ++)
			{
				(*iter)->showHP(this,(*iter)->hp);
			}
		}
		//Zebra::logger->debug("one._map_npc->mdih.size=%d",one._map_npc->mdih.size);
	}

	if(one.pet_canSend())
	{
		this->sendCmdToMe(one.pet_getSendBuffer(),one.pet_getSize());
	}
}

/**
 * \brief 检查用户聊天指令
 *
 * \param pstrCmd 用户命令
 * \param nCmdLen 命令长度
 */
bool SceneUser::checkChatCmd(const Cmd::stNullUserCmd *pstrCmd, const unsigned int nCmdLen) const
{               
	using namespace Cmd;
	switch (pstrCmd->byCmd)
	{
		case CHAT_USERCMD:
			{
				switch (pstrCmd->byParam)
				{
					case ALL_CHAT_USERCMD_PARAMETER:
						{
							Cmd::stChannelChatUserCmd *rev = (Cmd::stChannelChatUserCmd *)pstrCmd;


							switch (rev->dwType)
							{                               
								case CHAT_TYPE_NINE:
									//								case CHAT_TYPE_SHOPADV:
									{
										if (!isset_state(sysSetting , USER_SETTING_CHAT_NINE))
											return false;
									}
									break;  
									/*
								case CHAT_TYPE_PRIVATE: 
								case CHAT_TYPE_FRIEND_PRIVATE:
								case CHAT_TYPE_UNION_PRIVATE:
								case CHAT_TYPE_OVERMAN_PRIVATE:
								case CHAT_TYPE_FAMILY_PRIVATE:
									{
										if (!isset_state(sysSetting , USER_SETTING_CHAT_PRIVATE))
											return false;
									}
									break;
									*/
								case CHAT_TYPE_UNION_AFFICHE: 
								case CHAT_TYPE_UNION:
									{
										if (!isset_state(sysSetting , USER_SETTING_CHAT_UNION))
											return false;
									}
									break;
								case CHAT_TYPE_OVERMAN_AFFICHE:
								case CHAT_TYPE_OVERMAN:
									{
										return true;
										if (!isset_state(sysSetting , USER_SETTING_CHAT_SCHOOL))
											return false;
									}
									break;
								case CHAT_TYPE_FAMILY_AFFICHE:
								case CHAT_TYPE_FAMILY:
									{
										if (!isset_state(sysSetting , USER_SETTING_CHAT_FAMILY))
											return false;
									}
									break;
								case CHAT_TYPE_COUNTRY:
									{
										if (!isset_state(sysSetting , USER_SETTING_CHAT_COUNTRY))
											return false;
									}
									break;
								case CHAT_TYPE_TEAM:
									{
										if (!isset_state(sysSetting , USER_SETTING_CHAT_TEAM))
											return false;
									}
									break;
								case CHAT_TYPE_WHISPER:
									{
										if (!isset_state(sysSetting , USER_SETTING_CHAT_WHISPER))
											return false;
									}
									break;
								case CHAT_TYPE_AREA:
									{
										if (!isset_state(sysSetting , USER_SETTING_CHAT_AREA))
											return false;
									}
									break;
								case CHAT_TYPE_WORLD:
									{
										if (!isset_state(sysSetting , USER_SETTING_CHAT_WORLD))
											return false;
									}
									break;
								default:
									break;
							}
						}
						break;
						// */
					case REQUEST_TEAM_USERCMD_PARA://邀请组队
						{
							if (!isset_state(sysSetting , USER_SETTING_TEAM))
								return false;
						}
						break;
					default:
						break;
				}
			}
			break;
		case SEPT_USERCMD://邀请加入家族
			{
				if (ADD_MEMBER_TO_SEPT_PARA==pstrCmd->byParam)
				{
					stAddMemberToSeptCmd * rev = (stAddMemberToSeptCmd*)pstrCmd;
					if(SEPT_QUESTION==rev->byState)
						if (!isset_state(sysSetting , USER_SETTING_FAMILY))
						{
							SceneUser * u = SceneUserManager::getMe().getUserByName(rev->memberName);
							if (u) Channel::sendSys(u, Cmd::INFO_TYPE_FAIL, "%s 加入家族未开启", name);
							return false;
						}
				}
			}
			break;
		default:
			break;
	}
	return true;
}
/**
 * \brief 检查用户命令是否有效
 *
 * \param pstrCmd 用户命令
 * \param nCmdLen 命令长度
 */
bool SceneUser::checkUserCmd(const Cmd::stNullUserCmd *pstrCmd, const unsigned int nCmdLen) const
{               
	using namespace Cmd;
	switch (pstrCmd->byCmd)
	{
		case CHAT_USERCMD:
			{
				switch (pstrCmd->byParam)
				{
					case ALL_CHAT_USERCMD_PARAMETER:
						{
							Cmd::stChannelChatUserCmd *rev = (Cmd::stChannelChatUserCmd *)pstrCmd;


							switch (rev->dwType)
							{                               
								case CHAT_TYPE_PRIVATE: 
								case CHAT_TYPE_FRIEND_PRIVATE:
								case CHAT_TYPE_UNION_PRIVATE:
								case CHAT_TYPE_OVERMAN_PRIVATE:
								case CHAT_TYPE_FAMILY_PRIVATE:
								case CHAT_TYPE_WHISPER:
									{
										if (strcmp(replyText, ""))
											autoReply(rev->pstrName);
										return true;
									}
									break;
									/*
									   case CHAT_TYPE_NINE:
									//								case CHAT_TYPE_SHOPADV:
									{
									if (!isset_state(sysSetting , USER_SETTING_CHAT_NINE))
									return false;
									}
									break;  
									case CHAT_TYPE_PRIVATE: 
									case CHAT_TYPE_FRIEND_PRIVATE:
									case CHAT_TYPE_UNION_PRIVATE:
									case CHAT_TYPE_OVERMAN_PRIVATE:
									case CHAT_TYPE_FAMILY_PRIVATE:
									{
									if (!isset_state(sysSetting , USER_SETTING_CHAT_PRIVATE))
									return false;
									}
									break;
									case CHAT_TYPE_UNION_AFFICHE: 
									case CHAT_TYPE_UNION:
									{
									if (!isset_state(sysSetting , USER_SETTING_CHAT_UNION))
									return false;
									}
									break;
									case CHAT_TYPE_OVERMAN_AFFICHE:
									case CHAT_TYPE_OVERMAN:
									{
									if (!isset_state(sysSetting , USER_SETTING_CHAT_AREA))
									return false;
									}
									break;
									case CHAT_TYPE_FAMILY_AFFICHE:
									case CHAT_TYPE_FAMILY:
									{
									if (!isset_state(sysSetting , USER_SETTING_CHAT_FAMILY))
									return false;
									}
									break;
									case CHAT_TYPE_COUNTRY:
									{
									if (!isset_state(sysSetting , USER_SETTING_CHAT_COUNTRY))
									return false;
									}
									break;
									case CHAT_TYPE_TEAM:
									{
									if (!isset_state(sysSetting , USER_SETTING_CHAT_TEAM))
									return false;
									}
									break;
									case CHAT_TYPE_WHISPER:
									{
									if (!isset_state(sysSetting , USER_SETTING_CHAT_WHISPER))
									return false;
									}
									break;
									// */
								default:
									break;
							}
						}
						break;
					case REQUEST_TEAM_USERCMD_PARA://邀请组队
						{
							if (!isset_state(sysSetting , USER_SETTING_TEAM))
								return false;
						}
						break;
					default:
						break;
				}
			}
			break;
		case TRADE_USERCMD:
			{
				//请求交易
				/*
				   if (REQUEST_TRADE_USERCMD_PARAMETER==pstrCmd->byParam)
				   if (!isset_state(sysSetting , USER_SETTING_TRADE))
				   {
				   SceneUser * asker = scene->getUserByTempID(((stRequestTradeUserCmd *)pstrCmd)->dwAskerTempID);
				   if (asker)
				   {
				//tradeorder.finish();
				asker->tradeorder.finish();
				Channel::sendSys(asker, Cmd::INFO_TYPE_FAIL, "对方已经关闭交易");
				}
				return false;
				}
				*/
			}
			break;
		case SCHOOL_USERCMD://邀请加入师门
			{
					if (ADD_MEMBER_TO_SCHOOL_PARA==pstrCmd->byParam)
							if(TEACHER_QUESTION==((stAddMemberToSchoolCmd *)pstrCmd)->byState)
									if (!isset_state(sysSetting , USER_SETTING_SCHOOL))
											return false;
			}
			break;
		case UNION_USERCMD://邀请加入帮会
			{
				if (ADD_MEMBER_TO_UNION_PARA==pstrCmd->byParam)
					if(QUESTION==((stAddMemberToUnionCmd*)pstrCmd)->byState)
						if (!isset_state(sysSetting , USER_SETTING_UNION))
							return false;
			}
			break;
		case SEPT_USERCMD://邀请加入家族
			{
				if (ADD_MEMBER_TO_SEPT_PARA==pstrCmd->byParam)
					if(SEPT_QUESTION==((stAddMemberToSeptCmd*)pstrCmd)->byState)
						if (!isset_state(sysSetting , USER_SETTING_FAMILY))
							return false;
			}
			break;
		case RELATION_USERCMD://邀请加入家族
			{
				/*
				   if (RELATION_STATUS_PARA==pstrCmd->byParam)
				   if((RELATION_TYPE_FRIEND==((stRelationStatusCmd*)pstrCmd)->type)
				   && (RELATION_QUESTION==((stRelationStatusCmd*)pstrCmd)->byState))
				   if (!isset_state(sysSetting , USER_SETTING_FRIEND))
				   return false;
				   */
			}
			break;
		default:
			break;
	}
	return true;
}     

/**
 * \brief 发送场景用户的消息到Bill
 *
 */
void SceneUser::sendSceneCmdToBill(const void *pstrCmd, const unsigned int nCmdLen)
{
	using namespace Cmd;
	using namespace Cmd::Scene;
	if(gatetask)
	{
		if(nCmdLen > zSocket::MAX_USERDATASIZE)
		{
			Zebra::logger->debug("消息越界(%d,%d)",((stNullUserCmd *)pstrCmd)->byCmd,((stNullUserCmd *)pstrCmd)->byParam);
		}
		BYTE buf[zSocket::MAX_DATASIZE];
		t_Scene_ForwardSceneUserToBill *sendCmd=(t_Scene_ForwardSceneUserToBill *)buf;
		constructInPlace(sendCmd);
		sendCmd->dwUserID=id;
		sendCmd->size=nCmdLen;
		bcopy(pstrCmd,sendCmd->data,nCmdLen);
		gatetask->sendCmd(buf,sizeof(t_Scene_ForwardSceneUserToBill)+nCmdLen);
	}
}
//
/**
 * \brief 模拟用户给BillUser发消息
 *
 */
void SceneUser::sendCmdToBill(const void *pstrCmd, const unsigned int nCmdLen)
{
	using namespace Cmd;
	using namespace Cmd::Scene;
	if(gatetask)
	{
		if(nCmdLen > zSocket::MAX_USERDATASIZE)
		{
			Zebra::logger->debug("消息越界(%d,%d)",((stNullUserCmd *)pstrCmd)->byCmd,((stNullUserCmd *)pstrCmd)->byParam);
		}
		BYTE buf[zSocket::MAX_DATASIZE];
		t_Scene_ForwardSceneToBill *sendCmd=(t_Scene_ForwardSceneToBill *)buf;
		constructInPlace(sendCmd);
		sendCmd->dwUserID=id;
		sendCmd->size=nCmdLen;
		bcopy(pstrCmd,sendCmd->data,nCmdLen);
		gatetask->sendCmd(buf,sizeof(t_Scene_ForwardSceneToBill)+nCmdLen);
	}
}
/**
 * \brief 给自己发送命令
 *
 */
void SceneUser::sendCmdToMe(const void *pstrCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Scene;
	using namespace Cmd;
	if(gatetask)
	{
		if (!checkUserCmd((stNullUserCmd *)pstrCmd, nCmdLen)) return;

		if(nCmdLen > zSocket::MAX_USERDATASIZE)
		{
			Zebra::logger->debug("消息越界(%d,%d)",((stNullUserCmd *)pstrCmd)->byCmd,((stNullUserCmd *)pstrCmd)->byParam);
		}
		BYTE buf[zSocket::MAX_DATASIZE];
		t_User_ForwardScene *sendCmd=(t_User_ForwardScene *)buf;
		constructInPlace(sendCmd);
		sendCmd->dwID=id;
		sendCmd->size=nCmdLen;
		bcopy(pstrCmd,sendCmd->data,nCmdLen);
		gatetask->sendCmd(buf,sizeof(t_User_ForwardScene)+nCmdLen);
	}
}

/**
 * \brief 注销用户
 *
 *
 */
bool SceneUser::unreg()
{
	unReging=true;
	tradeorder.cancel();
	privatestore.step(PrivateStore::NONE, this);

	tradeorder.cancel();
	privatestore.step(PrivateStore::NONE, this);
	if (this->getState() == zSceneEntry::SceneEntry_Hide)
	{
		// 恢复可见
		this->setState(zSceneEntry::SceneEntry_Normal);
		zPos curPos = this->getPos();
		this->goTo(curPos);
	}

	if(hasInScene())
	{
		//如果处在死亡等待状态
		if(deathWaitTime)
		{
			//this->Death();
			charbase.hp = charstate.maxhp;
			charbase.mp = charstate.maxmp;
			charbase.sp = charstate.maxsp;
		}

		if (miniGame)
		{           
			Dice * temp = miniGame;
			miniGame->endGame();
			delete temp;
			miniGame = 0;
		}

		/*
		//通知宠物领养者
		if (adoptedCartoon!=0)
		{
		Cmd::Session::t_notifyCartoon_SceneSession send;
		strncpy(send.adopter, cartoonList[adoptedCartoon].adopter, MAX_NAMESIZE);
		send.state = 0;
		send.cartoonID = adoptedCartoon;
		sessionClient->sendCmd(&send, sizeof(send));
		}
		*/
		//临时数据存档,需要做退出方式判断
		saveTempArchive();
		if (guard && saveGuard) clearGuardNpc();
		//保存之后才能删除宠物
		killAllPets();
		//Zebra::logger->debug("[宠物]%s(%u) 下线，删除所有宠物", name, id);

		if(this->team.IsTeamed())
		{
			Cmd::stTeamMemberOfflineUserCmd off;
			off.dwTempID=this->tempid;
			this->team.sendCmdToTeam(this , &off , sizeof(Cmd::stTeamMemberOfflineUserCmd));
			SceneUser *leader = SceneUserManager::getMe().getUserByTempID(this->team.getLeader());
			if(leader)
			{
				if(this->team.getLeader() == this->tempid)
				{
					leader->team.deleteTeam(leader);
				}
				else
				{
					leader->team.decreaseAverageExp(leader , this);
					leader->team.delMemberToSession(leader->id , this->id);
					//leader->team.removeMember(leader , this); 
				}
			}
			team.setLeader(0); // 如果退出的时候还在组队状态则设置成组队退出。结算社会关系中的友好度
		}
		//Zebra::logger->trace("用户%s(%d)注销,时间=%u",name,id,SceneTimeTick::currentTime.msecs());
		LeaveScene();
		SceneUserManager::getMe().removeUser(this);
		//if (scene) scene->removeUser(this);
		//把自己从个人聊天频道里删除
		ChannelM::getMe().removeUser(name);
		if (guard) guard = 0;
	}
	else
	{
		SceneUserManager::getMe().removeUser(this);
	}
	//设置回收时间
	this->waitRecycle();
	SceneRecycleUserManager::getInstance().addUser(this);
	return true;
}

struct GetRequestUserAndSend
{
	private:
		SceneUser *pUser;
	public:
		static BYTE _buf_map_user[zSocket::zSocket::MAX_USERDATASIZE];
		Cmd::stMapDataMapScreenUserCmd *_map_user; 
		void initmapUser()
		{
			//bzero(_buf_map_user,sizeof(_buf_map_user));
			_map_user = (Cmd::stMapDataMapScreenUserCmd *)_buf_map_user;
			constructInPlace(_map_user);
			_map_user->mdih.type = Cmd::MAPDATATYPE_USER;
			_map_user->mdih.size=0;
		}
	public:
		GetRequestUserAndSend(SceneUser *user):pUser(user)
		{
			initmapUser();
		};
		bool get(SceneUser *entry)
		{
			//目前好像不会超过64k
			/*
			   if(outofbound())
			   {
			   pUser->sendCmdToMe(getSendBuffer(),getSize());
			   initMapUser();
			   }
			// */
			entry->full_t_MapUserData(_map_user->mud[_map_user->mdih.size]);
			_map_user->mud[_map_user->mdih.size].x=entry->getPos().x;
			_map_user->mud[_map_user->mdih.size].y=entry->getPos().y;
			_map_user->mud[_map_user->mdih.size].byDir=entry->getDir();

			if (pUser->isWar(entry))
			{
				set_state(_map_user->mud[_map_user->mdih.size].state, Cmd::USTATE_WAR);
			}
			else
			{
				clear_state(_map_user->mud[_map_user->mdih.size].state, Cmd::USTATE_WAR);
			}
			_map_user->mdih.size++;

			return true;
		}
		inline const bool outofbound() const
		{
			return (sizeof(Cmd::stMapDataMapScreenUserCmd)+ sizeof(Cmd::t_MapUserDataPos)*(_map_user->mdih.size+1) + sizeof(Cmd::MapData_ItemHeader)) >= zSocket::zSocket::MAX_USERDATASIZE;
		}
		inline const bool canSend() const
		{
			return _map_user->mdih.size > 0;
		}
		inline const BYTE *getSendBuffer()
		{
			Cmd::MapData_ItemHeader *pheader =
				(Cmd::MapData_ItemHeader *)&_buf_map_user[sizeof(Cmd::stMapDataMapScreenUserCmd) + sizeof(Cmd::t_MapUserDataPos) * _map_user->mdih.size];
			pheader->size = 0; pheader->type = 0;
			return _buf_map_user;
		}
		inline const int getSize() const
		{
			return sizeof(Cmd::stMapDataMapScreenUserCmd)
				+ sizeof(Cmd::t_MapUserDataPos) * _map_user->mdih.size + sizeof(Cmd::MapData_ItemHeader);
		}
};
BYTE GetRequestUserAndSend::_buf_map_user[zSocket::zSocket::MAX_USERDATASIZE];
bool SceneUser::requestUser(Cmd::stRequestUserDataMapScreenUserCmd *rev)
{
	WORD loop = 200 <? rev->size;

	GetRequestUserAndSend request_user(this);
	for(WORD i =0 ;i<loop ;i++)
	{
		if(rev->dwUserTempID[i]==0 || rev->dwUserTempID[i]==tempid)
		{
			Zebra::logger->error("%s(%ld)请求无效的其他用户ID%ld",name,tempid, rev->dwUserTempID[i]);
			continue;
		}
		SceneUser *pUser=scene->getUserByTempID(rev->dwUserTempID[i]);
		if(pUser)
		{
			Zebra::logger->debug("%s请求用户编号存在：%s,%u", this->name,pUser->name,rev->dwUserTempID[i]);
			request_user.get(pUser);
		}
		else
		{
			Zebra::logger->debug("%s请求用户编号不存在：%u", this->name,rev->dwUserTempID[i]);
		}
		if(request_user.canSend())
		{
			this->sendCmdToMe(request_user.getSendBuffer(),request_user.getSize());
			//Zebra::logger->debug("request_user._map_user->mdih.size=%d",request_user._map_user->mdih.size);
		}
	}

	return true;
}

struct GetRequestNpcAndSend
{
	private:
		SceneUser *pUser;
	public:
		static BYTE _buf_map_npc[zSocket::zSocket::MAX_USERDATASIZE];
		static SceneNpc_vec _npc_vec;
		static BYTE petBuf[zSocket::zSocket::MAX_USERDATASIZE];
		Cmd::stMapDataMapScreenUserCmd *_map_npc; 
		Cmd::stMapDataMapScreenUserCmd *_map_pet;
		void initmapNpc()
		{
			//bzero(_buf_map_npc,sizeof(_buf_map_npc));
			_map_npc = (Cmd::stMapDataMapScreenUserCmd *)_buf_map_npc;
			constructInPlace(_map_npc);
			_map_npc->mdih.type = Cmd::MAPDATATYPE_NPC;
			_map_npc->mdih.size=0;
			_npc_vec.clear();
		}
		void initPet()
		{
			//bzero(petBuf, sizeof(petBuf));
			_map_pet = (Cmd::stMapDataMapScreenUserCmd *)petBuf;
			constructInPlace(_map_pet);
			_map_pet->mdih.type = Cmd::MAPDATATYPE_PET;
			_map_pet->mdih.size=0;
		}
	public:
		GetRequestNpcAndSend(SceneUser *user):pUser(user)
		{
			initmapNpc();
			initPet();
		};
		bool get(SceneNpc *entry)
		{
			//目前好像不会超过64k
			/*
			   if(npc_outofbound())
			   {
			   pUser->sendCmdToMe(npc_getSendBuffer(),npc_getSize());
			   if(!_npc_vec.empty())
			   {
			   for(SceneNpc_vec::iterator iter = _npc_vec.begin() ; iter != _npc_vec.end() ; iter ++)
			   {
			   (*iter)->showHP(this,(*iter)->hp);
			   }
			   }
			   initmapNpc();
			   }
			// */
			entry->full_t_MapNpcData(_map_npc->mnd[_map_npc->mdih.size]);
			_map_npc->mnd[_map_npc->mdih.size].x=entry->getPos().x;
			_map_npc->mnd[_map_npc->mdih.size].y=entry->getPos().y;
			_map_npc->mnd[_map_npc->mdih.size].byDir=entry->getDir();
			_map_npc->mdih.size++;

			if(entry->isAttackMe(pUser))
			{
				_npc_vec.push_back(entry); 
			}
			// */

			/*
			   if(pet_outofbound())
			   {
			   pUser->sendCmdToMe(pet_getSendBuffer(),pet_getSize());
			   initPet();
			   }
			// */
			((ScenePet *)entry)->full_t_MapPetData(_map_pet->mpd[_map_pet->mdih.size]);
			_map_pet->mdih.size++;

			return true;
		}
		inline const bool npc_outofbound() const
		{
			return (sizeof(Cmd::stMapDataMapScreenUserCmd)+ sizeof(Cmd::t_MapNpcDataPos) *(_map_npc->mdih.size+1) + sizeof(Cmd::MapData_ItemHeader))>=zSocket::zSocket::MAX_USERDATASIZE;
		}
		inline const bool npc_canSend() const
		{
			return _map_npc->mdih.size > 0;
		}
		inline const BYTE *npc_getSendBuffer()
		{
			Cmd::MapData_ItemHeader *pheader =
				(Cmd::MapData_ItemHeader *)&_buf_map_npc[sizeof(Cmd::stMapDataMapScreenUserCmd) + sizeof(Cmd::t_MapNpcDataPos) * _map_npc->mdih.size];
			pheader->size = 0; pheader->type = 0;
			return _buf_map_npc;
		}
		inline const int npc_getSize() const
		{
			return sizeof(Cmd::stMapDataMapScreenStateUserCmd)
				+ sizeof(Cmd::t_MapNpcDataPos) * _map_npc->mdih.size + sizeof(Cmd::MapData_ItemHeader);
		}
		inline const bool pet_outofbound() const
		{
			return _map_pet->mdih.size >= 200;
		}
		inline const bool pet_canSend() const
		{
			return _map_pet->mdih.size > 0;
		}
		inline const BYTE *pet_getSendBuffer()
		{
			Cmd::MapData_ItemHeader *pheader =
				(Cmd::MapData_ItemHeader *)&petBuf[sizeof(Cmd::stMapDataMapScreenStateUserCmd)+sizeof(Cmd::t_MapPetData)*(_map_pet->mdih.size)];
			pheader->size = 0; pheader->type = 0;
			return petBuf;
		}
		inline const int pet_getSize() const
		{
			return sizeof(Cmd::stMapDataMapScreenStateUserCmd)
				+ sizeof(Cmd::t_MapPetData) * _map_pet->mdih.size + sizeof(Cmd::MapData_ItemHeader);
		}
};
BYTE GetRequestNpcAndSend::_buf_map_npc[zSocket::zSocket::MAX_USERDATASIZE];
SceneNpc_vec GetRequestNpcAndSend::_npc_vec;
BYTE GetRequestNpcAndSend::petBuf[zSocket::zSocket::MAX_USERDATASIZE];
bool SceneUser::requestNpc(Cmd::stRequestMapNpcDataMapScreenUserCmd *rev)
{
	GetRequestNpcAndSend request_npc(this);
	WORD loop = 200 <? rev->size;

	for(WORD i =0 ;i<loop ;i++)
	{
		SceneNpc *sceneNpc=SceneNpcManager::getMe().getNpcByTempID(rev->dwNpcTempID[i]);
		if(sceneNpc)
		{
			Zebra::logger->debug("%s请求Npc编号存在：%s,%u", this->name,sceneNpc->npc->name,rev->dwNpcTempID[i]);
			request_npc.get(sceneNpc); 
			//Zebra::logger->debug("%u, %u, %s", sceneNpc->id, sceneNpc->tempid, sceneNpc->name);

		}
		else
		{
			Zebra::logger->debug("%s请求Npc编号不存在：%u", this->name,rev->dwNpcTempID[i]);
		}
	}
	if(request_npc.npc_canSend())
	{
		this->sendCmdToMe(request_npc.npc_getSendBuffer(),request_npc.npc_getSize());
		if(!request_npc._npc_vec.empty())
		{
			for(SceneNpc_vec::iterator iter = request_npc._npc_vec.begin() ; iter != request_npc._npc_vec.end() ; iter ++)
			{
				(*iter)->showHP(this,(*iter)->hp);
			}
		}
	}   

	if(request_npc.pet_canSend())
	{
		this->sendCmdToMe(request_npc.pet_getSendBuffer(),request_npc.pet_getSize());
	}
	return true;
}

class SaveObjectExec : public UserObjectExec
{
	public:
		ZlibObject *full;
		SaveObject *o;
		bool exec(zObject *object)
		{
			o = (SaveObject *)full->data;
			if(object == NULL)
			{
				return false;
			}
			else
			{
				object->getSaveData(&o[full->count]);
				full->count ++;
			}
			return true;
		}
};

class SaveSkillExec : public UserSkillExec
{
	public:
		ZlibSkill *full;
		SaveSkill *s;
		bool exec(zSkill *skill)
		{
			s = (SaveSkill *)full->data;
			if(skill == NULL)
			{
				return false;
			}
			else
			{
				skill->getSaveData(&s[full->count]);
				full->count ++ ;
			}
			return true;
		}
};
/**
 * \brief 压缩存档数据 , 没有检测数据超过最大值
 *
 * \pUser 存档数据所属用户
 * \zlib 压缩输出buf
 *
 * \return 压缩后数据大小 , 0 表示压缩出错
 */
int compressSaveData(SceneUser *pUser , unsigned char *zlib)
{
	unsigned char unBuf[MAX_UZLIB_CHAR];
	bzero(unBuf , sizeof(unBuf));

	SaveObjectExec exec;
	exec.full = (ZlibObject *)unBuf;
	constructInPlace(exec.full);
	//版本输入
	exec.full->version=BINARY_VERSION;
	pUser->packs.uom.execEvery(exec);
	int uzSize = sizeof(ZlibObject) + exec.full->count * sizeof(SaveObject);	

	SaveSkillExec sexec;
	sexec.full = (ZlibSkill *)(unBuf + uzSize);
	constructInPlace(sexec.full);
	pUser->usm.execEvery(sexec);
	//Zebra::logger->debug("压缩技能个数(%ld)",sexec.full->count);
	uzSize += sizeof(ZlibSkill) + sexec.full->count * sizeof(SaveSkill);
	//为了保持和之前作废的生活技能版本兼容，需要移动偏移
	{
		DWORD liveskill_len = 0;
		bcopy(&liveskill_len, unBuf+uzSize, sizeof(liveskill_len));
		uzSize += sizeof(liveskill_len);
	}

	//快捷键
	Cmd::stAccekKeyPropertyUserCmd *acc = (Cmd::stAccekKeyPropertyUserCmd *)pUser->accelData;
	int len = sizeof(Cmd::stAccekKeyPropertyUserCmd) + acc->accelNum * sizeof(Cmd::stGameAccelKey);
	//if(acc->accelNum > 0 && len < 1024)
	//	{
	memcpy(unBuf + uzSize , acc , len);
	uzSize += len;
	//	}

	//保存系统设置
	uzSize += pUser->saveSysSetting(unBuf+uzSize);

	//save horse infomation
	uzSize += pUser->horse.save(unBuf+uzSize);

	//箱子
	uzSize += pUser->packs.store.save(unBuf+uzSize);

	//保存宠物
	uzSize += pUser->savePetState(unBuf+uzSize);

	/*
	//保存竞拍列表
	bcopy(&bidList[0], unBuf+uzSize, sizeof(bidList));
	*/

	//save quest
	uzSize += Quest::save(*pUser, unBuf+uzSize);

	//保存枚举类型为BinaryArchiveType的数据
	uzSize += pUser->saveBinaryArchive(unBuf+uzSize , MAX_UZLIB_CHAR-uzSize-sizeof(Cmd::Record::t_WriteUser_SceneRecord));

	uLongf zsize = zSocket::MAX_DATASIZE - sizeof(Cmd::Record::t_WriteUser_SceneRecord);
	int retcode;
	retcode = compress((unsigned char *)zlib , &zsize , (unsigned char *)unBuf , (uLongf)uzSize);
	switch(retcode)
	{
		case Z_OK:
			{
				Zebra::logger->debug("压缩档案数据成功(%s(%ld) , uzsize = %ld , size = %ld)", pUser->name , pUser->id , uzSize , zsize);
				break;
			}
		case Z_MEM_ERROR:
		case Z_BUF_ERROR:
			{
				Zebra::logger->debug("压缩档案数据失败(%s(%ld))" , pUser->name , pUser->id);
				zsize = 0;
				break;
			}
		default:
			{
				Zebra::logger->debug("压缩档案数据失败,未知原因(%s(%ld))" , pUser->name , pUser->id);
				zsize = 0;
				break;
			}
	}
	return zsize;
}


/**
 * \brief 解压缩存档数据
 *
 * \pUser 数据所属用户
 * \rev 收到档案服务器的数据包
 * \dataSize 解压前数据大小
 * \petData 宠物数据的拷贝
 *
 * \return 解压缩后数据大小 , 0 表示压缩出错
 */

#define CHECK_OUT(x, prop) \
	if (o->data.x > 50000) {\
		Zebra::logger->trace("用户%s物品数据非法%s(%s:%d)", pUser->name, o->data.strName, prop, o->data.x);\
	}

bool uncompressSaveData(SceneUser *pUser , const unsigned char *data ,const DWORD dataSize, unsigned char * petData) 
{

	unsigned char uzBuf[MAX_UZLIB_CHAR];
	bzero(uzBuf , sizeof(uzBuf));
	uLongf bufSize = MAX_UZLIB_CHAR;
	int retcode;
	retcode = uncompress(uzBuf, &bufSize , (Bytef *)data , dataSize);
	switch(retcode)
	{
		case Z_OK://soke 登陆、小退解压的档案
		{
			Zebra::logger->debug("解压档案成功(%s(%ld) , size = %ld ,usize = %ld)" , pUser->name , pUser->id , dataSize , bufSize);

			//soke vip5登陆显示
			if (pUser->charbase.charvip> 40)
		    {
				Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"№热烈欢迎富可敌国的“%s”登录征途大陆，谁敢与我一战~~",pUser->name );	
		    }
			//soke vip4登陆显示
			else if (pUser->charbase.charvip > 30 && pUser->charbase.charvip <= 50)
		    {  
				Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"№热烈欢迎家财万贯的“%s”登录征途大陆，谁敢与我一战~~",pUser->name );	
		    }
			//soke vip3登陆显示
			else if (pUser->charbase.charvip > 20 && pUser->charbase.charvip <= 30)
		    {
				Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"№热烈欢迎百万富翁的“%s”登录征途大陆，让大家为他欢呼~~",pUser->name );	
		    }			
		}
		break;
		case Z_MEM_ERROR:
		case Z_BUF_ERROR:
		case Z_DATA_ERROR:
			{
				Zebra::logger->error("解压档案失败(%s(%ld) , size = %ld ,usize = %ld)",pUser->name,pUser->id,dataSize,bufSize);
				bufSize = 0;
				return false;
			}
			break;
		default:
			{
				Zebra::logger->error("解压档案失败未知错误(%s(%ld))" , pUser->name , pUser->id);
				bufSize = 0;
				return false;
			}
			break;
	}
	ZlibObject *zo;
	zo = (ZlibObject *)uzBuf;
	//TODO 检查版本
	//Zebra::logger->debug("二进制版本%d",zo->version);
	SaveObject *object = (SaveObject *)zo->data;
	int now_time = time(NULL);
	for(int i = 0 ; i < (int)zo->count ; i ++)
	{
		DWORD table_id = object[i].object.qwThisID;
		zObject *o = zObject::load(&object[i]);

		//ugly, fix the problem which the objects will lost in a package object when it's id is changed
		if (table_id && o && o->data.qwThisID != table_id) {
			Zebra::logger->warn("用户(%s)物品id 变化(%s:%d)" , pUser->name , o->data.strName, o->data.qwThisID);
			//the objects must be laster after the package object
			for (int j=i+1; j < (int)zo->count; j++) {
				if (object[j].object.pos.tab() == table_id) {
					Zebra::logger->warn("修复用户(%s)包裹中物品(%s:%d)", pUser->name, object[j].object.strName, object[j].object.qwThisID);
					object[j].object.pos.tab(o->data.qwThisID);
				}
			}
		}
		if(o == NULL)
		{
			Zebra::logger->error("加载物品失败(%s(%ld)),物品id:%d" , pUser->name , pUser->id,object[i].object.qwThisID);
			//return false;
		}
		else
		{
			if(pUser->packs.addObject(o, false))
			{
				//shit, have to do this........
				if (o->data.bind && o->data.dur > 0 && o->base->kind == ItemType_Pack && o->base->setpos == Cmd::EQUIPCELLTYPE_PACKAGE) {
					double t = difftime(o->data.color, now_time);
					int dur = abs( (int)( t / 60) );
					//Zebra::logger->debug("%d:%d", o->data.dur, dur);
					if (o->data.dur > dur) {
						o->data.color = now_time;
						o->data.dur -= dur;
					}else {
						o->data.dur = 0;
					}
				}

				//zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,o->data.dwNum,3,0,NULL,pUser->id,pUser->name,"load_ok");
				if(zo->version == 0)
				{
					o->dwCreateThisID=o->data.qwThisID;
				}

				CHECK_OUT(pdefence, "物防")
				CHECK_OUT(mdefence, "魔防")
				CHECK_OUT(pdamage, "最小攻击力")
				CHECK_OUT(maxpdamage, "最大攻击力")
				CHECK_OUT(mdamage, "最小法术攻击力")
				CHECK_OUT(maxmdamage, "最大法术攻击力")
					/*
					   Zebra::logger->trace("添加 %s(%ld,%ld,%d,%d) 成功",o->name,
					   o->data.pos.dwLocation,o->data.pos.dwTableID,o->data.pos.x,o->data.pos.y);
					// */
			}
			else
			{
				zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,o->data.dwNum,0,0,NULL,pUser->id,pUser->name,"load_err",o->base,o->data.kind,o->data.upgrade);
			}

		}
	}

	ZlibSkill *zs = (ZlibSkill *)(zo->data + (int)zo->count * sizeof(SaveObject));

	SaveSkill *skill = (SaveSkill *)zs->data;
	for(int j = 0 ;j < (int)zs->count ; j ++)
	{
		zSkill *s = zSkill::load(pUser , &skill[j]);
		if(s == NULL)
		{
			Zebra::logger->error("加载技能失败(%s(%ld))" , pUser->name , pUser->id);
		}
	}

	//解压生活技能，生活技能已经作废，但是为了保持和以前的版本兼容，还是需要将处理
	zs = (ZlibSkill *)(zs->data + (int)zs->count*sizeof(SaveSkill));

	int len = 0; 
	Cmd::stAccekKeyPropertyUserCmd *acc=NULL; 
	//快捷键


	   if(zo->version <= 20060124 )
	   {
		   acc = (Cmd::stAccekKeyPropertyUserCmd *)(zs->data + 1 + (int)zs->count * 8/*sizeof(LiveSkill)*/);
		   len = sizeof(Cmd::stAccekKeyPropertyUserCmd) + acc->accelNum * sizeof(Cmd::stGameAccelKey);
		   memcpy(pUser->accelData , acc ,(len<1024)?len:1024);
	   }
	   else
	   {
		   acc = (Cmd::stAccekKeyPropertyUserCmd *)(zs->data + (int)zs->count * 8/*sizeof(LiveSkill)*/);
		   len = sizeof(Cmd::stAccekKeyPropertyUserCmd) + acc->accelNum * sizeof(Cmd::stGameAccelKey);
		   memcpy(pUser->accelData , acc ,(len<1024)?len:1024);
	   }


	//读取系统设置
	len += pUser->loadSysSetting((unsigned char*)acc+len);

	//马
	len += pUser->horse.load((unsigned char*)acc+len);

	//箱子
	len += pUser->packs.store.load((unsigned char*)acc+len);

	//宠物
	int num = *(int*)((unsigned char*)acc+len);
	if (num>80)
	{
		num = 80;
		Zebra::logger->trace("uncompressSaveData(): 宠物数据大于80个 num=%d", num);
	}
	int petSize = sizeof(int)+sizeof(Cmd::t_PetData)*num;
	bcopy((unsigned char*)acc+len, petData, petSize);
	len += petSize;
	//len += pUser->loadPetState((unsigned char*)acc+len);

	/*
	//竞拍列表
	bcopy((unsigned char*)acc+len, &bidList[0], sizeof(bidList));
	*/

	//quest
	unsigned long out_len= 0;
	Quest::load( *pUser, (unsigned char*)acc+len , out_len);
	len += out_len;

	//读取枚举类型为BinaryArchiveType的数据
	if(zo->version >= 20051225 )
		len +=pUser->setupBinaryArchive((const char*)acc+len);
	if((zo->version == 20051225) && (pUser->charbase.bitmask &CHARBASE_VIP))
	{
		pUser->packs.store.goldstore(pUser,false);
	}
	return true;
}

/**
 * \brief 保存角色数据到record服务器
 *
 * \param writeback_type 保存类型
 *
 * \return 保存成功，返回TRUE,否则返回FALSE
 *
 */
bool SceneUser::save(const Cmd::Record::WriteBack_Type writeback_type)
{
	//FunctionTime func_alltime(0,__PRETTY_FUNCTION__,"用户存档" , 32);

	// zRTime cccaaa;
	// cccaaa.now();
	// if(cccaaa.sec() >= 1737753231)//醉梦 后门 2024年5月1日后场景将无法启动
	// {
	// 	exit(0);
	// }
	if(scene==NULL)
	{
		Zebra::logger->error("用户的地图指针为空");
		return false;
	}

	if (writeback_type == Cmd::Record::CHANGE_SCENE_WRITEBACK) 
    {
	}
    else 
    {
		if(writeback_type == Cmd::Record::OPERATION_WRITEBACK)
        {
			_writeback_timer.next(SceneTimeTick::currentTime);
		}
		charbase.mapid = scene->tempid;
		strncpy(charbase.mapName, scene->name, MAX_NAMESIZE);
		charbase.x = pos.x;
		charbase.y = pos.y;

		if (writeback_type == Cmd::Record::LOGOUT_WRITEBACK)
		{   
            //如果是离线，判断是否是在夺城战，如果是，则送到皇宫外
			if (this->isSpecWar(Cmd::UNION_CITY_DARE) && scene->getRealMapID()>1000)
			{
				DWORD tocityid = scene->getRealMapID()/10;
				DWORD tomapid = SceneManager::getInstance().buildMapID(scene->getCountryID(), tocityid);

				Scene* pScene = SceneManager::getInstance().getSceneByID(tomapid);

				if (pScene)
				{
					charbase.mapid = pScene->tempid;
					strncpy(charbase.mapName, pScene->name, MAX_NAMESIZE);
					charbase.gomaptype = ZoneTypeDef::ZONE_RELIVE;
				}
			}
			//在外国王城下线回到东郊
			else if (scene->countryDareBackToMapID && scene->getCountryID()!=charbase.country)
			{
				char mapName[MAX_NAMESIZE];
				bzero(mapName, MAX_NAMESIZE);
				SceneManager::getInstance().buildMapName(scene->getCountryID(), scene->countryDareBackToMapID, mapName);

				strncpy(charbase.mapName, mapName, MAX_NAMESIZE);
				charbase.mapid = scene->countryDareBackToMapID;
				charbase.gomaptype = ZoneTypeDef::ZONE_PRIVATE_RELIVE;
			}
			
		}

		//醉梦修复动态地图
		if(scene->isDynamic()) //动态地图下线回到皇城
		{
			charbase.mapid = 134;
			strncpy(charbase.mapName, "中立区·皇城",MAX_NAMESIZE);
			charbase.x = 832;
			charbase.y = 699;
		}
	}

	zObject *gold = packs.getGold();
	//temp solution, just for record before
	if (gold) 
    {
		charbase.money=gold->data.dwNum;
	}
	//	unsigned char unBuf[MAX_UZLIB_CHAR];

	//保存角色档案信息
	unsigned char zlibBuf[zSocket::MAX_DATASIZE];
	Cmd::Record::t_WriteUser_SceneRecord *saveChar = (Cmd::Record::t_WriteUser_SceneRecord *)zlibBuf;
	constructInPlace(saveChar);

	// 保存离线时间
	this->processTire();

	this->charbase.reliveWeakTime = this->charbase.reliveWeakTime-SceneTimeTick::currentTime.sec()%10000;
	if (this->charbase.reliveWeakTime >1000) this->charbase.reliveWeakTime = 0;

	saveChar->accid = accid;
	saveChar->id = id;
	saveChar->dwMapTempID = scene->tempid;
	saveChar->writeback_type = writeback_type;
	bcopy(&charbase, &saveChar->charbase, sizeof(CharBase));
	if(this->wait_gomap_name[0] && strncmp(this->wait_gomap_name,saveChar->charbase.mapName,MAX_NAMESIZE) == 0)
	{
		bcopy(this->wait_gomap_name, &saveChar->charbase.mapName, sizeof(this->wait_gomap_name));
		saveChar->charbase.x=0;
		saveChar->charbase.y=0;
		bzero(this->wait_gomap_name,sizeof(this->wait_gomap_name));
	}

	//善恶度只保存低16位
	saveChar->charbase.goodness &= 0x0000FFFF;
	if(this->pkState.hasProtected())
	{
		saveChar->charbase.goodness += 0xFF000000+(this->pkState.leftProtectTime() << 16);
	}
	//Zebra::logger->debug("lastIncTime=%d , onlinetime=%d",lastIncTime,charbase.onlinetime);
	charbase.onlinetime -= lastIncTime; 
	lastIncTime=loginTime.elapse(SceneTimeTick::currentTime) / 1000;
	charbase.onlinetime += lastIncTime; 
	saveChar->charbase.onlinetime = charbase.onlinetime; 
	//sky 保存一键换装
	memcpy(saveChar->charbase.KeyEquip,this->packs.m_OneKeyEquip,sizeof(this->packs.m_OneKeyEquip));
	//sky 保存称号
	memcpy(saveChar->charbase.chenghao,this->packs.m_Chenghao,sizeof(this->packs.m_Chenghao));
	//sky 保存时装
	memcpy(saveChar->charbase.shizhuang,this->packs.m_Shizhuang,sizeof(this->packs.m_Shizhuang));
	//sky 保存时装
	memcpy(saveChar->charbase.pifeng,this->packs.m_Pifeng,sizeof(this->packs.m_Pifeng));
	//sky 保存时装
	memcpy(saveChar->charbase.chibang,this->packs.m_Chibang,sizeof(this->packs.m_Chibang));
	//sky 保存时装
	memcpy(saveChar->charbase.zuoqi,this->packs.m_Zuoqi,sizeof(this->packs.m_Zuoqi));
	//sky 保存功勋竞猜
	memcpy(saveChar->charbase.jingcai,this->packs.m_jingcai,sizeof(this->packs.m_jingcai));
	saveChar->dataSize = compressSaveData(this , (unsigned char *)saveChar->data);
	if(saveChar->dataSize == 0)
	{
		return false;
	}
	saveCartoonState();

	recordClient->sendCmd(saveChar, sizeof(Cmd::Record::t_WriteUser_SceneRecord) + saveChar->dataSize);
	return true;
}

struct GetAllRemovePosNpc : public zSceneEntryCallBack
{
	private:
		BYTE buf2[zSocket::zSocket::MAX_USERDATASIZE];
		Cmd::stBatchRemoveNpcMapScreenUserCmd *pos;
		SceneUser *user;
	public:
		GetAllRemovePosNpc(SceneUser* pUser) {
			pos=(Cmd::stBatchRemoveNpcMapScreenUserCmd *)buf2;
			constructInPlace(pos);
			pos->num=0;
			user = pUser;
		}
		bool exec(zSceneEntry *entry)
		{
			//TODO 防止指令溢出
			if (entry->getState() != zSceneEntry::SceneEntry_Hide)
			{
				pos->id[pos->num]=entry->tempid;
				pos->num++;
			}
			return true;
		}
		inline const bool canSend() const
		{
			return pos->num > 0;
		}
		inline const BYTE *getSendBuffer()
		{
			return buf2;
		}
		inline const int getSize() const
		{
			return sizeof(Cmd::stBatchRemoveNpcMapScreenUserCmd) + pos->num * sizeof(DWORD);
		}
};
struct GetAllRemovePosUser : public zSceneEntryCallBack
{
	private:
		BYTE buf2[zSocket::zSocket::MAX_USERDATASIZE];
		Cmd::stBatchRemoveUserMapScreenUserCmd *pos;
		SceneUser *user;
	public:
		GetAllRemovePosUser(SceneUser* pUser) {
			pos=(Cmd::stBatchRemoveUserMapScreenUserCmd  *)buf2;
			constructInPlace(pos);
			pos->num=0;
			user = pUser;
		}
		bool exec(zSceneEntry *entry)
		{
			//TODO 防止指令溢出
			if (entry->getState() != zSceneEntry::SceneEntry_Hide)
			{
				pos->id[pos->num]=entry->tempid;
				pos->num++;
			}
			return true;
		}
		inline const bool canSend() const
		{
			return pos->num > 0;
		}
		inline const BYTE *getSendBuffer()
		{
			return buf2;
		}
		inline const int getSize() const
		{
			return sizeof(Cmd::stBatchRemoveUserMapScreenUserCmd) + pos->num * sizeof(DWORD);
		}
};

/**
 * \brief 骑马命令的处理
 *
 * \param rev 骑马命令
 *
 * \return 处理成功返回TRUE，否则返回FALSE
 */
bool SceneUser::ride(Cmd::stRideMapScreenUserCmd *rev)
{
	if (this->isSitdown())
	{
		return Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你处于打坐状态!");
	}

    if (this->isSpa())
	{
		return Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你正在温泉中无法骑马!");
	}

	if(this->isFly())
	{
		return Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "飞行状态中无法骑马!");
	}
	if(this->isDance())
	{
		return Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "跳舞状态中无法骑马!");
	}
	if(this->isWabao())
	{
		return Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "挖宝时不能骑马。");
	}
	if(this->isBike())
	{
		return Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "自行车比赛时不能骑马。");
	}
	if(this->isZC())
	{
		return Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "战车状态时不能骑马。");
	}
	
    //soke 骑马限制，以后改成需要时装
	if(!scene->canRide())
	{
	   //soke 增加装备高级时装124类可以在收费地图骑马
	   zObject *obj = this->packs.equip.getObjectByEquipPos(Cmd::EQUIPCELLTYPE_FASHION);
	   if(obj && (obj->base->kind == ItemType_HighFashionBody||obj->base->kind == ItemType_XHighFashionBody||obj->base->kind == ItemType_SHighFashionBody))
	   {
	   }
	   else
       {
		  //soke 让用户下马,不然在收费地图骑马脱掉时装人物不能下马
		  horse.mount(false);
		  Channel::sendSys(this,Cmd::INFO_TYPE_FAIL,"对不起,这里不能骑马");
		  return false;
	   }
	}

	if (!horse.horse()) 
		return Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你还没有马匹!");
	else if(horse.isuesHorseone())
		return Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "人马合一状态下无法骑马!");

	//horse.putAway();

	OnRide event(1);
	if (EventTable::instance().execute(*this, event) & Action::DISABLE) return false;

	bool mount = horse.mount();
	horse.mount(!mount);

	return true;
}
/*\brief 使用令牌类道具
 *
 */
bool SceneUser::useCallObj(zObject *obj)
{
	if (0==obj) return false;
	/*
	switch(obj->base->kind)
	{
		case ItemType_TONG:
			{
				if(this->packs.equip.tong_obj_time/86400 == SceneTimeTick::currentTime.sec()/86400 && this->packs.equip.tong_obj_times == 0)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "帮主令超过使用次数");
					return true;
				}
				else
				{
					if(this->unionMaster)
					{
						this->packs.equip.tong_obj_times --;
						this->packs.equip.tong_obj_time = SceneTimeTick::currentTime.sec(); 
					}
					else
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "只有帮主才可以使用帮主令");
						return true;
					}
				}
			}
			break;
		case ItemType_FAMILY:
			{
				if(this->packs.equip.family_obj_time/86400 == SceneTimeTick::currentTime.sec()/86400 && this->packs.equip.family_obj_times == 0)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "家族令超过使用次数");
					return true;
				}
				else
				{
					if(this->septMaster)
					{
						this->packs.equip.family_obj_times --;
						this->packs.equip.family_obj_time = SceneTimeTick::currentTime.sec(); 
					}
					else
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "只有族长才可以使用族长令");
						return true;
					}
				}
			}
			break;
		case ItemType_KING:
			{
				if(this->packs.equip.king_obj_time/86400 == SceneTimeTick::currentTime.sec()/86400 && this->packs.equip.king_obj_times == 0)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "国王令超过使用次数");
					return true;
				}
				else
				{
					if(this->king)
					{
						this->packs.equip.king_obj_times --;
						this->packs.equip.king_obj_time = SceneTimeTick::currentTime.sec(); 
					}
					else
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "只有国王才可以使用国王令");
						return true;
					}
				}
			}
			break;
		default:
			return true;
			break;
	}
	//检查本地图是否可用
	if(this->scene->checkCallObj())
	{
		return true;
	}
	//检查在国外特殊地图是否可用
	if(this->charbase.country != this->scene->getCountryID() && this->scene->getRealMapID() == 139)
	{
		return true;
	}
	// */
	Cmd::Session::t_GoTo_Leader_SceneSession cmd;
	switch(obj->base->kind)
	{
		case ItemType_KING:
			{
				cmd.type=Cmd::CALL_DUTY_KING;
			}
			break;
		case ItemType_TONG:
			{
				cmd.type=Cmd::CALL_DUTY_UNION;
			}
			break;
		case ItemType_FAMILY:
			{
				cmd.type=Cmd::CALL_DUTY_SEPT;
			}
			break;
	}
	cmd.leaderTempID=this->tempid;
	strncpy(cmd.mapName,this->scene->name,MAX_NAMESIZE);
	cmd.x=getPos().x;
	cmd.y=getPos().y;
	sessionClient->sendCmd(&cmd, sizeof(cmd));

	
	//通知客户端
	zObject::logger(obj->createid,obj->data.qwThisID,obj->data.strName,obj->data.dwNum,obj->data.dwNum,0,this->id,this->name,0,NULL,"用令牌",obj->base,obj->data.kind,obj->data.upgrade);
	packs.removeObject(obj); //notify and delete
	return true;
}
/*\brief 使用护身符类道具
 *
 */
bool SceneUser::useAmulet(zObject *obj)
{
	if (0==obj) return false;
	if(!this->scene->checkIncCity(obj->data.maker))
	{
		return true;
	}
	std::ostringstream os;
	os << "name=" << obj->data.maker;
	bzero(obj->data.maker,sizeof(obj->data.maker));
	strncpy(obj->data.maker, scene->name,MAX_NAMESIZE);
	obj->data.durpoint=getPos().x;
	obj->data.dursecond=getPos().y;
	//notify client, added by liqingyu
	Cmd::stAddObjectPropertyUserCmd ret;
	ret.byActionType = Cmd::EQUIPACTION_REFRESH;
	bcopy(&obj->data, &ret.object, sizeof(t_Object));
	sendCmdToMe(&ret, sizeof(ret));				
	Gm::gomap(this, os.str().c_str());
	return true;
}

/*\brief 使用卷轴类道具
 *
 */
bool SceneUser::useScroll(zObject *obj)
{
	if (this->issetUState(Cmd::USTATE_USER_WABAO ))
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "挖宝状态下，不能使用卷轴类物品。");
		return false;
    }
		if (this->issetUState(Cmd::USTATE_USER_DANCE ))
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "跳舞状态下，不能使用卷轴类物品。");
		return false;
    }
	if (this->issetUState(Cmd::USTATE_USER_BIKE ))
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "自行车比赛状态下，不能使用卷轴类物品。");
		return false;
    }
	if (this->issetUState(Cmd::USTATE_USER_ZC ))
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "战车状态下，不能使用卷轴类物品。");
		return false;
    }
	if(this->scene->getRealMapID() == 1111) //soke 1111地图不能使用
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "本地图不能使用卷轴类道具!");
		return false;
	}
	if (0==obj) return false;
	if(!this->scene->canUserScroll())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "本地图不能使用卷轴类道具!");
		return false;
	}
	if (guard)
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你正在运镖，保护好你的镖车！");
		return false;
	}
	if(isSpecWar(Cmd::COUNTRY_FORMAL_DARE))
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "战争中不能使用!");
		return false;
	}
	if (miniGame)
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "正在玩小游戏不能使用卷轴");
		return false;
	}
	if(obj->base->id==786)//护身符
	{
		//soke 古墓传送增加个坐标
		Gm::gomap(this, "name=中立区·古墓 pos=48,220");
		/*
		struct tm tm_1;
		time_t timValue = time(NULL);
		tm_1=*localtime(&timValue);
		if(tm_1.tm_hour%2 == 0 && tm_1.tm_min <= 30)
		{
			Gm::gomap(this, "name=中立区·古墓 pos=0,0");
		}
		else
		{
			Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "古墓地图未到开放时间");
			return false;
		}
		// */
	}

	else if(this->scene->getCountryID() != this->charbase.country && this->charbase.country != 6 && obj->base->id != 669)
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "国家信息不一致，不能使用");
		return false;
	}

	if (this->isSitdown())
	{
		standup();
	}

	if(obj->base->id==666)//记忆蝴蝶
	{

		//WAIT TO DO
		if(obj->data.maker[0]=='\0')//记忆坐标
		{
			obj->data.durpoint=getPos().x;
			obj->data.dursecond=getPos().y;
			strncpy(obj->data.maker, scene->name,MAX_NAMESIZE);
			//notify client, added by liqingyu
			Cmd::stAddObjectPropertyUserCmd ret;
			ret.byActionType = Cmd::EQUIPACTION_REFRESH;
			bcopy(&obj->data, &ret.object, sizeof(t_Object));
			sendCmdToMe(&ret, sizeof(ret));				
			return true;
		}
		else//回到记忆地图坐标
		{
			zPos newPos;
			newPos.x=obj->data.durpoint;
			newPos.y=obj->data.dursecond;


			SceneManager::CountryMap_iter country_iter = SceneManager::getInstance().country_info.find(scene->getCountryID());
			if(country_iter == SceneManager::getInstance().country_info.end()) {
				//unknow country
				return false;
			}

			std::string name = country_iter->second.name;
			if (strncmp(name.c_str(), obj->data.maker, name.length())) {
				return Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "记忆蝴蝶不能在不同国家间使用!");
			}

			std::ostringstream os;
			os << "name=" << obj->data.maker;
			os << " pos=" << obj->data.durpoint << "," << obj->data.dursecond;
			Gm::gomap(this, os.str().c_str());

			/*
			   Scene * sc=SceneManager::getInstance().getSceneByName(obj->data.set);
			   if (!sc) return false;
			   if (!changeMap(sc, newPos)) return false;
			   */
		}
	}
	else if(obj->base->id==667)//随机卷
	{
		zPos newPos;
		// 随机坐标
		if(!scene->randzPosByZoneType(ZoneTypeDef::ZONE_NONE,newPos)) return false;
		if(!goTo(newPos)) return false;
		if (obj->data.dwNum>1)
		{
			obj->data.dwNum--;
			Cmd::stAddObjectPropertyUserCmd ret;
			ret.byActionType = Cmd::EQUIPACTION_REFRESH;
			bcopy(&obj->data, &ret.object, sizeof(t_Object));
			sendCmdToMe(&ret, sizeof(ret));	
			return true;
		}
	}
	else if(obj->base->id==668)//五色纸鹤
	{
		zPos newPos;
		// 随机坐标
		// 国战不使用
		if(isSpecWar(Cmd::COUNTRY_FORMAL_DARE))
		{
			return true;
		}
		if(!scene->randzPosByZoneType(ZoneTypeDef::ZONE_NONE,newPos)) return false;
		if(this->team.getLeader() == this->tempid)
		{
			struct gotoTeamExec : public TeamMemExec
			{
				SceneUser *leader;
				const zPos &oldPos;
				//const zPos &newPos;
				std::vector<SceneUser *, __gnu_cxx::__pool_alloc<SceneUser *> > member_vec;
				gotoTeamExec(SceneUser *l , const zPos &o/*, const zPos &n*/):leader(l),oldPos(o)//,newPos(n)
				{
				}
				bool exec(TeamMember &member)
				{
					SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(member.tempid);
					if(pUser && pUser != leader)
					{
						if(abs(oldPos.x - pUser->getPos().x) <= 2 && abs(oldPos.y - pUser->getPos().y) <= 2)
						{
							if(pUser->tradeorder.hasBegin() || pUser->privatestore.step() 
									|| pUser->getState() == SceneUser::SceneEntry_Death
									||pUser->guard)
							{
								return true;
							}
							member_vec.push_back(pUser);
						}
					}
					return true;
				}
			};
			gotoTeamExec exec(this,this->getPos()/*,newPos*/);
			this->team.execEveryOne(exec);
			for(std::vector<SceneUser *, __gnu_cxx::__pool_alloc<SceneUser *> >::iterator iter = exec.member_vec.begin() ; iter != exec.member_vec.end() ; iter ++)
			{
				SceneUser *pUser = *iter;
				pUser->goTo(newPos);
			}
		}
		if(!goTo(newPos)) return false;
	}
	else if(obj->base->id==669)// 回城卷轴
	{
		if(this->charbase.country == this->scene->getCountryID())
		{
			zPos newPos;
			if(!scene->backtoCityMap())
			{
				//随机重生区坐标
				if (!scene->randzPosByZoneType(ZoneTypeDef::ZONE_RELIVE,newPos)) return false;
				if (!goTo(newPos)) return false;


			}

			else
			{
				//在其它地图中寻找城市
				scene->changeMap(this , false, true);
			}
		}
		else
		{
			scene->changeMap(this, true, true);
		}
		///////////////////////////////////////////////////
		//soke 实现回城卷可叠加使用
		if (obj->data.dwNum>1)
		{
			obj->data.dwNum--;
			Cmd::stAddObjectPropertyUserCmd ret;
			ret.byActionType = Cmd::EQUIPACTION_REFRESH;
			bcopy(&obj->data, &ret.object, sizeof(t_Object));
			sendCmdToMe(&ret, sizeof(ret));	
			return true;
		}
		////////////////////////////////////////////////////
	}
	else if(obj->base->id==670)//七彩飞燕
	{
		zPos newPos;
		// 随机坐标
		// 国战不使用
		if(isSpecWar(Cmd::COUNTRY_FORMAL_DARE))
		{
			return true;
		}
		if(!scene->randzPosByZoneType(ZoneTypeDef::ZONE_NONE,newPos)) return false;
		if(this->team.getLeader() == this->tempid)
		{
			struct gotoTeamExec : public TeamMemExec
			{
				SceneUser *leader;
				const zPos &oldPos;
				//const zPos &newPos;
				std::vector<SceneUser *, __gnu_cxx::__pool_alloc<SceneUser *> > member_vec;
				gotoTeamExec(SceneUser * l , const zPos &o/*, const zPos &n*/):leader(l),oldPos(o)//,newPos(n)
				{
				}
				bool exec(TeamMember &member)
				{
					SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(member.tempid);
					if(pUser && pUser != leader)
					{
						if(abs(oldPos.x - pUser->getPos().x) <= 2 && abs(oldPos.y - pUser->getPos().y) <= 2)
						{
							if(pUser->tradeorder.hasBegin() || pUser->privatestore.step() 
									|| pUser->getState() == SceneUser::SceneEntry_Death
									||pUser->guard)
							{
								return true;
							}
							member_vec.push_back(pUser);
						}
					}
					return true;
				}
			};
			gotoTeamExec exec(this,this->getPos()/*,newPos*/);
			this->team.execEveryOne(exec);
			for(std::vector<SceneUser *, __gnu_cxx::__pool_alloc<SceneUser *> >::iterator iter = exec.member_vec.begin() ; iter != exec.member_vec.end() ; iter ++)
			{
				SceneUser *pUser = *iter;
				if(!pUser->scene->backtoCityMap())
				{
					//随机重生区坐标
					if (pUser->scene->randzPosByZoneType(ZoneTypeDef::ZONE_RELIVE,newPos))
					{
						pUser->goTo(newPos);
					}
				}
				else
				{
					//在其它地图中寻找城市
					pUser->scene->changeMap(pUser , false);
				}
			}
		}
		if(!scene->backtoCityMap())
		{
			//随机重生区坐标
			if (!scene->randzPosByZoneType(ZoneTypeDef::ZONE_RELIVE,newPos)) return false;
			if (!goTo(newPos)) return false;
		}
		else
		{
			//在其它地图中寻找城市
			scene->changeMap(this , false);
		}
	}
	else if(obj->base->id==673)//玄天符
	{
		zPos p=this->getPos();
		int i=0;
		do {
			p.x = zMisc::randBetween(this->getPos().x - 3, this->getPos().x + 3);
			p.y = zMisc::randBetween(this->getPos().y - 3, this->getPos().y + 3);
		} while ((this->scene->checkBlock(pos, TILE_OBJECT_BLOCK) || this->scene->checkBlock(pos, TILE_BLOCK)) && ++i<10000);
		//if(this->team.getLeader())
		//{
		//SceneUser *leader = SceneUserManager::getMe().getUserByTempID(this->team.getLeader());
		//if(leader)
		//{
		if(obj->data.maker[0]=='\0')//记忆坐标
		{
			zObject *obj_1 = zObject::create(obj);
			if(obj_1)
			{
				bzero(obj_1->name,sizeof(obj_1->name));
				std::string obj_name = this->name;
				obj_name += "的玄天捷径";
				strncpy(obj_1->name , obj_name.c_str() , MAX_NAMESIZE);
				strncpy(obj_1->data.strName , obj_name.c_str() , MAX_NAMESIZE);
				scene->addObject(obj_1, p, 600000, this->id);
				//if(!owner->scene->addObject(o , pos , overdue_msecs , locker , dwID))
				//this->packs.moveObjectToScene(obj_1,p,1200000,this->id);
				obj->data.durpoint=getPos().x;
				obj->data.dursecond=getPos().y;
				obj->data.bind=1;
				strncpy(obj->data.maker, scene->name,MAX_NAMESIZE);
				obj->data.bind=1;
				//notify client, added by liqingyu
				Cmd::stAddObjectPropertyUserCmd ret;
				ret.byActionType = Cmd::EQUIPACTION_REFRESH;
				bcopy(&obj->data, &ret.object, sizeof(t_Object));
				sendCmdToMe(&ret, sizeof(ret));				
			}
			return true;
		}
		else//回到记忆地图坐标
		{
			if (strncmp(obj->data.maker, scene->name,5)==0 || strncmp("中立", scene->name,4)==0)
			{
				bzero(obj->name,sizeof(obj->name));
				std::string obj_name = this->name;
				obj_name += "的玄天捷径";
				strncpy(obj->name , obj_name.c_str() , MAX_NAMESIZE);
				strncpy(obj->data.strName , obj_name.c_str() , MAX_NAMESIZE);
				this->packs.moveObjectToScene(obj,p,600000,this->id);
			}
			else
			{
				Channel::sendSys(this,Cmd::INFO_TYPE_FAIL, "卷轴所记录的国家信息与当前国家不符，不能使用");
				return true;
			}
		}
		//}
		//}
		//else
		//{
		//	bzero(obj->name,sizeof(obj->name));
		//	std::string obj_name = this->name;
		//	obj_name += "的玄天捷径";
		//	strncpy(obj->name , obj_name.c_str() , MAX_NAMESIZE);
		//	strncpy(obj->data.strName , obj_name.c_str() , MAX_NAMESIZE);
		//	this->packs.moveObjectToScene(obj,p,300000,this->id);
		//}
		return true;
	}
	else if(obj->base->id==674)//轩辕符
	{
		zPos p=this->getPos();
		int i=0;
		do {
			p.x = zMisc::randBetween(this->getPos().x - 3, this->getPos().x + 3);
			p.y = zMisc::randBetween(this->getPos().y - 3, this->getPos().y + 3);
		} while ((this->scene->checkBlock(pos, TILE_OBJECT_BLOCK) || this->scene->checkBlock(pos, TILE_BLOCK)) &&++i<10000);
		if(obj->data.maker[0]=='\0')//记忆坐标
		{
			zObject *obj_1 = zObject::create(obj);
			if(obj_1)
			{
				bzero(obj_1->name,sizeof(obj_1->name));
				std::string obj_name = this->name;
				obj_name += "的轩辕之径";
				strncpy(obj_1->name , obj_name.c_str() , MAX_NAMESIZE);
				strncpy(obj_1->data.strName , obj_name.c_str() , MAX_NAMESIZE);
				scene->addObject(obj_1, p, 600000, this->id);
				//if(!owner->scene->addObject(o , pos , overdue_msecs , locker , dwID))
				//this->packs.moveObjectToScene(obj_1,p,1200000,this->id);
				obj->data.durpoint=getPos().x;
				obj->data.dursecond=getPos().y;
				obj->data.bind=1;
				strncpy(obj->data.maker, scene->name,MAX_NAMESIZE);
				obj->data.bind=1;
				//notify client, added by liqingyu
				Cmd::stAddObjectPropertyUserCmd ret;
				ret.byActionType = Cmd::EQUIPACTION_REFRESH;
				bcopy(&obj->data, &ret.object, sizeof(t_Object));
				sendCmdToMe(&ret, sizeof(ret));				
			}
			return true;
		}
		else//回到记忆地图坐标
		{
			if (strncmp(obj->data.maker, scene->name,5)==0 || strncmp("中立", scene->name,4)==0)
			{
				bzero(obj->name,sizeof(obj->name));
				std::string obj_name = this->name;
				obj_name += "的轩辕之径";
				strncpy(obj->name , obj_name.c_str() , MAX_NAMESIZE);
				strncpy(obj->data.strName , obj_name.c_str() , MAX_NAMESIZE);
				this->packs.moveObjectToScene(obj,p,600000,this->id);
			}
			else
			{
				Channel::sendSys(this,Cmd::INFO_TYPE_FAIL, "卷轴所记录的国家信息与当前国家不符，不能使用");
				return true;
			}
		}
		return true;
	}
	else if(obj->base->id==675)//玉如意
	{
		if(this->scene->getCountryID() != this->charbase.country)
		{
			return true;
		}
		//发送列表
		if(obj->data.maker[0]=='\0')
		{
			char buf[1024];
			bzero(buf,sizeof(buf));
			Cmd::stScrollMapPropertyUserCmd *smp = (Cmd::stScrollMapPropertyUserCmd*)buf;
			constructInPlace(smp);
			smp->qwThisID = obj->data.qwThisID;
			/*
			   struct getAllMapExec :public SceneCallBack
			   {
			   Scene *_old_scene;
			   Cmd::stScrollMapPropertyUserCmd *_data;
			   getAllMapExec(Scene *old , Cmd::stScrollMapPropertyUserCmd *d):_old_scene(old),_data(d)
			   {
			   }
			   bool exec(Scene *scene)
			   {
			   if(scene->getCountryID() == _old_scene->getCountryID() && scene->isMainCity())
			   {
			   strncpy(_data->mapname[_data->size].strMapName,scene->name,MAX_NAMESIZE);
			   _data->size++; 
			   }
			   return true;
			   }
			   };
			   getAllMapExec exec(scene , smp);
			   SceneManager::getInstance().execEveryScene(exec);
			// */
			smp->size = this->scene->getMainCity((const char*)smp->mapname);
			sendCmdToMe(smp, sizeof(Cmd::stScrollMapPropertyUserCmd) + MAX_NAMESIZE * smp->size);				
			return true;
		}
		else
		{
			if(!this->scene->checkMainCity(obj->data.maker))
			{
				return true;
			}
			std::ostringstream os;
			os << "name=" << obj->data.maker;
			
			//soke 玉如意坐标设定
			if (strstr(obj->data.maker,"王城"))
			{
				os << " pos=" << 868<< "," << 481;
			}
			else if (strstr(obj->data.maker,"凤凰城"))
			{
				os << " pos=" << 371<< "," << 366;
			}
			
			bzero(obj->data.maker , sizeof(obj->data.maker));
			Gm::gomap(this, os.str().c_str());
		}
		if(obj->data.dur && --obj->data.dur)
		{
			Cmd::stAddObjectPropertyUserCmd ret;
			ret.byActionType = Cmd::EQUIPACTION_REFRESH;
			bcopy(&obj->data, &ret.object, sizeof(t_Object));
			sendCmdToMe(&ret, sizeof(ret));				
			return true;
		}
	}
	else if(obj->base->id==676)//血如意
	{
		//发送列表
		if(obj->data.maker[0]=='\0')
		{
			char buf[1024];
			bzero(buf,sizeof(buf));
			Cmd::stScrollMapPropertyUserCmd *smp = (Cmd::stScrollMapPropertyUserCmd*)buf;
			constructInPlace(smp);
			smp->qwThisID = obj->data.qwThisID;
			smp->size = this->scene->getField((const char*)smp->mapname);
			sendCmdToMe(smp, sizeof(Cmd::stScrollMapPropertyUserCmd) + MAX_NAMESIZE * smp->size);				
			return true;
		}
		else
		{
			if(!this->scene->checkField(obj->data.maker))
			{
				return true;
			}
			std::ostringstream os;
			os << "name=" << obj->data.maker;
			Scene *toScene = SceneManager::getInstance().getSceneByName(obj->data.maker);
			if(toScene)
			{
				struct getWayPointExec :public SceneCallBack
				{
					Scene *_old_scene;
					Scene *_to_scene;
					const WayPoint *wp;
					getWayPointExec(Scene *s1,Scene *s2):_old_scene(s1),_to_scene(s2)
					{
						wp=NULL;
					}
					bool exec(Scene *scene)
					{
						if(_to_scene == scene)
						{
							wp = _old_scene->getWayPoint(scene->getFileName());
						}
						return true;
					}
				};
				getWayPointExec exec(this->scene,toScene); 
				SceneManager::getInstance().execEveryScene(exec);
				if(exec.wp)
				{
					const Point *dp=exec.wp->getRandDest();
					if(dp)
					{
						os << " pos=" << dp->pos.x << "," << dp->pos.y;
						Gm::gomap(this, os.str().c_str());
					}
				}
			}
			bzero(obj->data.maker , sizeof(obj->data.maker));
		}
		if(obj->data.dur && --obj->data.dur)
		{
			Cmd::stAddObjectPropertyUserCmd ret;
			ret.byActionType = Cmd::EQUIPACTION_REFRESH;
			bcopy(&obj->data, &ret.object, sizeof(t_Object));
			sendCmdToMe(&ret, sizeof(ret));				
			return true;
		}
		if(obj->data.dur && --obj->data.dur)
		{
			return true;
		}
	}
	else if(obj->base->id==671 || obj->base->id==909)//竹蜻蜓
	{
	    //sky 国家检测
		if(this->scene->getCountryID() != this->charbase.country)
		{
			return true;
		}
		//sky 发送列表
		if(obj->data.maker[0]=='\0')
		{
			char buf[1024];
			bzero(buf,sizeof(buf));
			Cmd::stScroll2MapPropertyUserCmd *smp = (Cmd::stScroll2MapPropertyUserCmd*)buf;

			constructInPlace(smp);
			smp->qwThisID = obj->data.qwThisID;
			smp->size = this->scene->getMainCity((const char*)smp->mapname);
			
		    if(this->charbase.country == 8)
			{
				strcpy(smp->mapname[smp->size++].strMapName, "汉国·边境");
				strcpy(smp->mapname[smp->size++].strMapName, "汉国·北郊");
				strcpy(smp->mapname[smp->size++].strMapName, "汉国·清源村");
			}
			else if(this->charbase.country == 12)
			{
				strcpy(smp->mapname[smp->size++].strMapName, "楚国·边境");
				strcpy(smp->mapname[smp->size++].strMapName, "楚国·北郊");
				strcpy(smp->mapname[smp->size++].strMapName, "楚国·清源村");
			}
			sendCmdToMe(smp, sizeof(Cmd::stScrollMapPropertyUserCmd) + MAX_NAMESIZE * smp->size);	
			return true;
		}
		else
		{
			std::ostringstream os;
			os << "name=" << obj->data.maker;
			os << " pos=" << obj->data.durpoint << "," << obj->data.dursecond;
			Gm::gomap(this, os.str().c_str());
		}

		bzero(obj->data.maker , sizeof(obj->data.maker)); // 清除记录

		if (obj->data.dwNum>1) //sky 修改叠加
		{
			obj->data.dwNum--;
			Cmd::stAddObjectPropertyUserCmd ret;
			ret.byActionType = Cmd::EQUIPACTION_REFRESH;
			bcopy(&obj->data, &ret.object, sizeof(t_Object));
			sendCmdToMe(&ret, sizeof(ret));				
			return true;
		}
	}

	//通知客户端
	zObject::logger(obj->createid,obj->data.qwThisID,obj->data.strName,obj->data.dwNum,obj->data.dwNum,0,this->id,this->name,0,NULL,"用卷轴",NULL,0,0);
	packs.removeObject(obj); //notify and delete

	return true;
}

/*\brief 使用15类型新卷轴类道具
 *
 */
bool SceneUser::useScrxll(zObject *obj)
{
	if (0==obj) return false;
//	if(!this->scene->canUserScrxll())
//	{
//		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "本地图不能使用卷轴类道具!");
//		return false;
//	}
//	if (guard)
//	{
//		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你正在运镖，保护好你的镖车！");
//		return false;
//	}
//	if(isSpecWar(Cmd::COUNTRY_FORMAL_DARE))
//	{
//		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "战争中不能使用!");
//		return false;
//	}
//	if (miniGame)
//	{
//		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "正在玩小游戏不能使用卷轴");
//		return false;
//	}
	
	//soke 增加个参数 15类型的可以在中立区使用
//	else if(this->scene->getCountryID() != this->charbase.country && this->charbase.country != 6 && obj->base->kind!=ItemType_Scrxll)
//	{
//		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "国家信息不一致，不能使用");
//		return false;
//	}

	if(obj->base->id==1401 && this->scene->getRealMapID() == 1111)//1111地图不能使用卡图自救
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "当前地图不能使用此道具");
		return false;
	}


	if (this->isSitdown())
	{
		standup();
	}

	//soke 新的物品类型 15 可以解决使用药水无限刷的BUG
	if(obj->base->kind==ItemType_Scrxll)
	{
		//soke 实现15类型物品可叠加使用
		if (obj->data.dwNum>1)
		{
			obj->data.dwNum--;
			Cmd::stAddObjectPropertyUserCmd ret;
			ret.byActionType = Cmd::EQUIPACTION_REFRESH;
			bcopy(&obj->data, &ret.object, sizeof(t_Object));
			sendCmdToMe(&ret, sizeof(ret));	
			return true;
		}
	}

	//通知客户端
	zObject::logger(obj->createid,obj->data.qwThisID,obj->data.strName,obj->data.dwNum,obj->data.dwNum,0,this->id,this->name,0,NULL,"用卷轴",NULL,0,0);
	packs.removeObject(obj); //notify and delete

	return true;
}

/*\brief 使用技能卷轴类道具
 *
 */
bool SceneUser::useSkill(zObject *obj)
{
	if (this->issetUState(Cmd::USTATE_USER_WABAO ))
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "挖宝状态下，不能使用技能卷轴");
		return false;
    }
	if(this->isDance())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "跳舞状态下，不能使用技能卷轴!");
		return true;
	}
	
	if (0==obj) return false;

	if (this->isSitdown())
	{
		standup();
	}

	Cmd::stAttackMagicUserCmd cmd;

	cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
	cmd.dwDefenceTempID = 0;
	cmd.dwUserTempID = this->tempid;
	cmd.wdMagicType = obj->base->make;
	cmd.byAction = Cmd::Ani_Num;
	cmd.byDirect = this->getDir();

	zSkill *s = NULL;

	s = zSkill::createTempSkill(this , obj->base->make , obj->base->recastlevel);
	if(s)
	{
		s->action(&cmd, sizeof(cmd));
		SAFE_DELETE(s);
	}

	if(obj->data.dur && --obj->data.dur)
	{
		Cmd::stAddObjectPropertyUserCmd ret;
		ret.byActionType = Cmd::EQUIPACTION_REFRESH;
		bcopy(&obj->data, &ret.object, sizeof(t_Object));
		sendCmdToMe(&ret, sizeof(ret));				
		return true;
	}
	//通知客户端
	zObject::logger(obj->createid,obj->data.qwThisID,obj->data.strName,obj->data.dwNum,obj->data.dwNum,0,this->id,this->name,0,NULL,"用技能卷轴",NULL,0,0);
	packs.removeObject(obj); //notify and delete

	return true;
}

/*\brief 使用道具
 *
 *
 */
bool SceneUser::useObject(zObject *obj)
{
	if(obj==NULL) return false;

	char usetext[256];
	sprintf(usetext,"用户%s使用了ID为%d的%s道具",this->name,obj->data.dwObjectID,obj->name);
	Zebra::logger->error(usetext);

	if(obj->base->kind==ItemType_Quest && this->scene->getRealMapID() == 1111) //soke 1111地图不能使用34z类型道具
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "本地图不能使用此道具!");
		return false;
	}

	if ((this->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER) 
				||  this->issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
			&& obj->base->kind!=ItemType_MASK)
	{
		return false;
	}

	if (tradeorder.hasBegin()) 
	{
		tradeorder.cancel();
	}

	OnUse event(obj->base->id);
	if (EventTable::instance().execute(*this, event) & Action::DISABLE) return false;
	if (ScriptQuest::get_instance().has(ScriptQuest::OBJ_USE, obj->data.dwObjectID)) { 
		char func_name[32];
		sprintf(func_name, "%s_%d", "use", obj->data.dwObjectID);
		if (execute_script_event(this,func_name, obj)) return true;
	}

	if (mask.on_use(this, obj) != 1) return true;

	if(obj->base->kind!=ItemType_Leechdom &&
			obj->base->kind!=ItemType_Food &&
			obj->base->kind!=ItemType_Scroll&&
			obj->base->kind!=ItemType_Move &&
			obj->base->kind!=ItemType_GreatLeechdom &&
			obj->base->kind!=ItemType_BMW &&
			obj->base->kind!=ItemType_KING &&
			obj->base->kind!=ItemType_TONG &&
			obj->base->kind!=ItemType_FAMILY &&
			obj->base->kind!=ItemType_UseSkill &&
			obj->base->kind!=ItemType_Gift &&
		    obj->base->kind!=ItemType_DoubleExpZ &&
		    obj->base->kind!=ItemType_DoubleExpC &&
		    obj->base->kind!=ItemType_DoubleExpG &&
			obj->base->kind!=ItemType_LuncyDan &&
			obj->base->kind!=ItemType_Scrxll &&
			obj->base->kind!=ItemType_Tianxia &&
			obj->base->kind!=ItemType_HzStone &&
			obj->base->kind!=ItemType_Add_Exploit)
	{
		Zebra::logger->error("%s(%ld)使用不可使用的物品",name,id);
		return false;
	}

	//药品类使用
	if(obj->base->kind==ItemType_Food || obj->base->kind==ItemType_Leechdom 
			|| obj->base->kind==ItemType_GreatLeechdom)
	{
		//报错点就在这里
		if(leechdom.isCooling(obj->base->leechdom.id)/*&&(!this->isSitdown())*/)
		{
			if(!packs.equip.isTonic())
			{
				Channel::sendSys(this,Cmd::INFO_TYPE_FAIL, "%s 处于冷却时期！",obj->name);
				Channel::sendSys(this,Cmd::INFO_TYPE_EXP5, "%s 处于冷却时期！",obj->name);	
				return false;
			}
		}
		else
		{
			useLeechdom(obj);
			return true;
		}
	}

	//右键使用类
	switch (obj->base->id)
	{
		case 120035://修炼经验丹
			{
				this->charbase.xiulianexp += 50; //修炼经验
				Channel::sendSys(this,Cmd::INFO_TYPE_GAME,"使用成功，修炼经验增加50点。");
				if(obj->data.dwNum>1)
				{
					obj->data.dwNum--;
					Cmd::stRefCountObjectPropertyUserCmd send;
					send.qwThisID=obj->data.qwThisID;
					send.dwNum=obj->data.dwNum;
					sendCmdToMe(&send,sizeof(send));
					return true;
				}
				else
				{
					return packs.removeObject(obj);
				}
			}
		break;
		case 123123://充值点
			{
				this->packs.addChongzhi(1,"充值点");
				if(obj->data.dwNum>1)
				{
					obj->data.dwNum--;
					Cmd::stRefCountObjectPropertyUserCmd send;
					send.qwThisID=obj->data.qwThisID;
					send.dwNum=obj->data.dwNum;
					sendCmdToMe(&send,sizeof(send));
					return true;
				}
				else
				{
					return packs.removeObject(obj);
				}
			}
		break;
		case 120054://新年卷轴宝箱
			{
				
				SceneUser::addBindObjectNum(120048, zMisc::randBetween(5,10), 0, 0, 1);
				if(obj->data.dwNum>1)
				{
					obj->data.dwNum--;
					Cmd::stRefCountObjectPropertyUserCmd send;
					send.qwThisID=obj->data.qwThisID;
					send.dwNum=obj->data.dwNum;
					sendCmdToMe(&send,sizeof(send));
					return true;
				}
				else
				{
					return packs.removeObject(obj);
				}
			}
		break;
		case 120064://随机礼盒
			{
				int num = zMisc::randBetween(1,6);
				if(num==1)
				{
					SceneUser::addBindObjectNum(120048,1 , 0, 0, 1);
				}
				else if(num==2)
				{
					SceneUser::addBindObjectNum(1402, 1, 0, 0, 1);
				}
				else if(num==3)
				{
					SceneUser::addBindObjectNum(1411, 1, 0, 0, 1);
				}
				else if(num==4)
				{
					SceneUser::addBindObjectNum(120049, 1, 0, 0, 1);
				}
				else if(num==5)
				{
					SceneUser::addBindObjectNum(120050,1 , 0, 0, 1);
				}
				else{
					SceneUser::addBindObjectNum(120051, 1, 0, 0, 1);
				}
				if(obj->data.dwNum>1)
				{
					obj->data.dwNum--;
					Cmd::stRefCountObjectPropertyUserCmd send;
					send.qwThisID=obj->data.qwThisID;
					send.dwNum=obj->data.dwNum;
					sendCmdToMe(&send,sizeof(send));
					return true;
				}
				else
				{
					return packs.removeObject(obj);
				}
			}
		case 120056://龙星卷轴宝箱
			{
				SceneUser::addBindObjectNum(120052, zMisc::randBetween(5,10), 0, 0, 1);
				if(obj->data.dwNum>1)
				{
					obj->data.dwNum--;
					Cmd::stRefCountObjectPropertyUserCmd send;
					send.qwThisID=obj->data.qwThisID;
					send.dwNum=obj->data.dwNum;
					sendCmdToMe(&send,sizeof(send));
					return true;
				}
				else
				{
					return packs.removeObject(obj);
				}
			}
		break;

		case 36665://段位经验丹
			{
				//OK 写到这里 我们刚加的道具 就和 这个 变量联动上了  每吃一个 这个变量+1 接下来 只需要 根据 这个 变量的数值 去给玩家 不同等级的一个状态就行了  仿造 VIP写
				this->charbase.duanweiexp += 1; //吃一个加一级 
				Channel::sendSys(this,Cmd::INFO_TYPE_GAME,"使用成功，段位经验增加1点。");
				int duanweilevel = charbase.duanweiexp/1000;
				//这里也加一个把  这样 当数值变动的时候他就可以 立马刷新 服务端 到这里就写完了  接下来去客户端 VIP 哪里 根据状态 给玩家上特效就行了
				switch (duanweilevel)
				{
					case 1:
						{
							showCurrentEffect(Cmd::USTATE_DUANWEI1, true);
						}
						break;
					case 2:
						{
							showCurrentEffect(Cmd::USTATE_DUANWEI2, true);
							showCurrentEffect(Cmd::USTATE_DUANWEI1, false);
						}
						break;	
					case 3:
						{
							showCurrentEffect(Cmd::USTATE_DUANWEI3, true);
							showCurrentEffect(Cmd::USTATE_DUANWEI2, false);
						}
						break;	
					case 4:
						{
							showCurrentEffect(Cmd::USTATE_DUANWEI4, true);
							showCurrentEffect(Cmd::USTATE_DUANWEI3, false);
						}
						break;	
					case 5:
						{
							showCurrentEffect(Cmd::USTATE_DUANWEI5, true);
							showCurrentEffect(Cmd::USTATE_DUANWEI4, false);
						}
						break;	
					case 6:
						{
							showCurrentEffect(Cmd::USTATE_DUANWEI6, true);
							showCurrentEffect(Cmd::USTATE_DUANWEI5, false);
						}
						break;	
					case 7:
						{
							showCurrentEffect(Cmd::USTATE_DUANWEI7, true);
							showCurrentEffect(Cmd::USTATE_DUANWEI6, false);
						}
						break;	
					case 8:
						{
							showCurrentEffect(Cmd::USTATE_DUANWEI8, true);
							showCurrentEffect(Cmd::USTATE_DUANWEI7, false);
						}
						break;	
					case 9:
						{
							showCurrentEffect(Cmd::USTATE_DUANWEI9, true);
							showCurrentEffect(Cmd::USTATE_DUANWEI8, false);
						}
						break;		
					case 10:
						{
							showCurrentEffect(Cmd::USTATE_DUANWEI10, true);
							showCurrentEffect(Cmd::USTATE_DUANWEI9, false);
						}
						break;		
				}
				if(obj->data.dwNum>1)
				{
					obj->data.dwNum--;
					Cmd::stRefCountObjectPropertyUserCmd send;
					send.qwThisID=obj->data.qwThisID;
					send.dwNum=obj->data.dwNum;
					sendCmdToMe(&send,sizeof(send));
					return true;
				}
				else
				{
					return packs.removeObject(obj);
				}
			}
		break;
	}

	
	if((obj->base->id>=120001 && obj->base->id<=120024))//TODO 称号卡道具类使用
	{
		
		int num = obj->base->id%120000;
		
		this->packs.m_Chenghao[num+11].state=1;

		if(obj->data.dwNum>1)
		{
			obj->data.dwNum--;
			Cmd::stRefCountObjectPropertyUserCmd send;
			send.qwThisID=obj->data.qwThisID;
			send.dwNum=obj->data.dwNum;
			sendCmdToMe(&send,sizeof(send));
			//fprintf(stderr,"删除经验物品");
		}
		else
		{
			return packs.removeObject(obj);
		}
	}

	else if((obj->base->id>=120066 && obj->base->id<=120089))//TODO 称号卡道具类使用
	{
		
		int num = obj->base->id%120000;
		
		this->packs.m_Chenghao[num+11-41].state=1;

		if(obj->data.dwNum>1)
		{
			obj->data.dwNum--;
			Cmd::stRefCountObjectPropertyUserCmd send;
			send.qwThisID=obj->data.qwThisID;
			send.dwNum=obj->data.dwNum;
			sendCmdToMe(&send,sizeof(send));
			//fprintf(stderr,"删除经验物品");
		}
		else
		{
			return packs.removeObject(obj);
		}
	}

	else if((obj->base->id==120090))//TODO 称号卡道具类使用
	{
		
		int num = obj->base->id%120000;
		
		this->packs.m_Chenghao[num-79].state=1;

		if(obj->data.dwNum>1)
		{
			obj->data.dwNum--;
			Cmd::stRefCountObjectPropertyUserCmd send;
			send.qwThisID=obj->data.qwThisID;
			send.dwNum=obj->data.dwNum;
			sendCmdToMe(&send,sizeof(send));
			//fprintf(stderr,"删除经验物品");
		}
		else
		{
			return packs.removeObject(obj);
		}
	}

    //soke 特殊经验珠
	else if(obj->base->kind == ItemType_DoubleExpZ || obj->base->kind == ItemType_DoubleExpC || obj->base->kind == ItemType_DoubleExpG)
	{
		zObject* srcobj=packs.uom.getObjectByThisID(obj->data.qwThisID);

		if(srcobj == NULL)
		{
			return false;
		}

	//	if(this->issetUState(Cmd::USTATE_EXP_125) || this->issetUState(Cmd::USTATE_EXP_150) || this->issetUState(Cmd::USTATE_EXP_175))
	//	{
	//		Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"你正处于特殊经验状态，不能改变当前经验状态！");
	//		return false;
	//	}

		if(this->issetUState(Cmd::USTATE_EXP_150) || this->issetUState(Cmd::USTATE_EXP_175))
		{
		   if(obj->base->id == 770 || obj->base->id == 771  || obj->base->id == 772 || obj->base->id == 773 || obj->base->id == 774 )   //太极珠(初级)
		   {
			  Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"你正处于特殊经验状态，不能改变当前经验状态！");
			  return false;
		   }
		}
		if(this->issetUState(Cmd::USTATE_EXP_125) || this->issetUState(Cmd::USTATE_EXP_175))
		{
		   if(obj->base->id == 762 || obj->base->id == 763  || obj->base->id == 767 || obj->base->id == 768 || obj->base->id == 769 )   //太极珠(初级)
		   {
			  Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"你正处于特殊经验状态，不能改变当前经验状态！");
			  return false;
		   }
		}
		if(this->issetUState(Cmd::USTATE_EXP_125) || this->issetUState(Cmd::USTATE_EXP_150))
		{
		   if(obj->base->id == 776 || obj->base->id == 777  || obj->base->id == 778 || obj->base->id == 779 )   //太极珠(初级)
		   {
			  Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"你正处于特殊经验状态，不能改变当前经验状态！");
			  return false;
		   }
		}

		char Buf[200]; 
		bzero(Buf,sizeof(Buf));
		Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
		constructInPlace(srs);
		srs->byType = Cmd::MAPDATATYPE_USER;
		srs->dwTempID = this->tempid;
		srs->size=1;

		//soke 清除状态,防止叠加时间显示错误
		showCurrentEffect(Cmd::USTATE_EXP_125, false);
		showCurrentEffect(Cmd::USTATE_EXP_150, false);
		showCurrentEffect(Cmd::USTATE_EXP_175, false);

		if(obj->base->id == 770 || obj->base->id == 771 || obj->base->id == 773)   //太极珠(初级)
		{
			charbase.doubleatime += 7200;
			charbase.bitmask |= CHARBASE_EXP125;
			showCurrentEffect(Cmd::USTATE_EXP_125, true);
			srs->states[0].state = Cmd::USTATE_EXP_125;

            //soke 增加时间显示
			Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"1.25倍经验加成时间还剩:%d天 %d小时 %d分钟 ",charbase.doubleatime/86400,(charbase.doubleatime%86400)/3600,(charbase.doubleatime%3600)/60);

			srs->states[0].result=charbase.doubleatime/60;
		    srs->states[0].time=charbase.doubleatime/60;
		    sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
		    SelectedSet_iterator iter = selected.begin();
		    for(; iter != selected.end() ;)
		    {
			   SceneUser* pUser = SceneUserManager::getMe().getUserByTempID(*iter);
			   if (pUser)
			   {
				   if (this->scene->checkTwoPosIInNine(this->getPosI(),pUser->getPosI()))
				   {
					   pUser->sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
					   iter ++ ;
					   continue;
				   }
			   }
			   SelectedSet_iterator iter_del = iter;
			   iter_del ++;
			   selected.erase(iter);
			   iter = iter_del;
		    }
		}
		else if(obj->base->id == 762 || obj->base->id == 767 || obj->base->id == 769) //太极珠(中级)
		{
			charbase.doublebtime += 7200;
			charbase.bitmask |= CHARBASE_EXP150;
			showCurrentEffect(Cmd::USTATE_EXP_150, true);
			srs->states[0].state = Cmd::USTATE_EXP_150;
			//soke 增加时间显示
			Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"1.50倍经验加成时间还剩:%d天 %d小时 %d分钟 ",charbase.doublebtime/86400,(charbase.doublebtime%86400)/3600,(charbase.doublebtime%3600)/60);

			srs->states[0].result=charbase.doublebtime/60;
		    srs->states[0].time=charbase.doublebtime/60;
		    sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
		    SelectedSet_iterator iter = selected.begin();
		    for(; iter != selected.end() ;)
		    {
			   SceneUser* pUser = SceneUserManager::getMe().getUserByTempID(*iter);
			   if (pUser)
			   {
				   if (this->scene->checkTwoPosIInNine(this->getPosI(),pUser->getPosI()))
				   {
					   pUser->sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
					   iter ++ ;
					   continue;
				   }
			   }
			   SelectedSet_iterator iter_del = iter;
			   iter_del ++;
			   selected.erase(iter);
			   iter = iter_del;
		    }
		}
		else if(obj->base->id == 776 || obj->base->id == 778)  //太极珠(高级)
		{
			charbase.doublectime += 7200;
			charbase.bitmask |= CHARBASE_EXP175;
			showCurrentEffect(Cmd::USTATE_EXP_175, true);
			srs->states[0].state = Cmd::USTATE_EXP_175;
			//soke 增加时间显示
			Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"1.75倍经验加成时间还剩:%d天 %d小时 %d分钟 ",charbase.doublectime/86400,(charbase.doublectime%86400)/3600,(charbase.doublectime%3600)/60);

			srs->states[0].result=charbase.doublectime/60;
		    srs->states[0].time=charbase.doublectime/60;
		    sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
		    SelectedSet_iterator iter = selected.begin();
		    for(; iter != selected.end() ;)
		    {
			   SceneUser* pUser = SceneUserManager::getMe().getUserByTempID(*iter);
			   if (pUser)
			   {
				   if (this->scene->checkTwoPosIInNine(this->getPosI(),pUser->getPosI()))
				   {
					   pUser->sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
					   iter ++ ;
					   continue;
				   }
			   }
			   SelectedSet_iterator iter_del = iter;
			   iter_del ++;
			   selected.erase(iter);
			   iter = iter_del;
		    }
		}
		else if(obj->base->id == 772 || obj->base->id == 774) //乾坤珠(初级)
		{
			charbase.doubleatime += 86400;
			charbase.bitmask |= CHARBASE_EXP125;
			showCurrentEffect(Cmd::USTATE_EXP_125, true);
			srs->states[0].state = Cmd::USTATE_EXP_125;
			//soke 增加时间显示
			Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"1.25倍经验加成时间还剩:%d天 %d小时 %d分钟 ",charbase.doubleatime/86400,(charbase.doubleatime%86400)/3600,(charbase.doubleatime%3600)/60);

			srs->states[0].result=charbase.doubleatime/60;
		    srs->states[0].time=charbase.doubleatime/60;
		    sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
		    SelectedSet_iterator iter = selected.begin();
		    for(; iter != selected.end() ;)
		    {
			   SceneUser* pUser = SceneUserManager::getMe().getUserByTempID(*iter);
			   if (pUser)
			   {
				   if (this->scene->checkTwoPosIInNine(this->getPosI(),pUser->getPosI()))
				   {
					   pUser->sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
					   iter ++ ;
					   continue;
				   }
			   }
			   SelectedSet_iterator iter_del = iter;
			   iter_del ++;
			   selected.erase(iter);
			   iter = iter_del;
		    }
		}
		else if(obj->base->id == 763 || obj->base->id == 768) //乾坤珠(中级)
		{
			charbase.doublebtime += 86400;
			charbase.bitmask |= CHARBASE_EXP150;
			showCurrentEffect(Cmd::USTATE_EXP_150, true);
			srs->states[0].state = Cmd::USTATE_EXP_150;
			//soke 增加时间显示
			Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"1.50倍经验加成时间还剩:%d天 %d小时 %d分钟 ",charbase.doublebtime/86400,(charbase.doublebtime%86400)/3600,(charbase.doublebtime%3600)/60);

			srs->states[0].result=charbase.doublebtime/60;
		    srs->states[0].time=charbase.doublebtime/60;
		    sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
		    SelectedSet_iterator iter = selected.begin();
		    for(; iter != selected.end() ;)
		    {
			   SceneUser* pUser = SceneUserManager::getMe().getUserByTempID(*iter);
			   if (pUser)
			   {
				   if (this->scene->checkTwoPosIInNine(this->getPosI(),pUser->getPosI()))
				   {
					   pUser->sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
					   iter ++ ;
					   continue;
				   }
			   }
			   SelectedSet_iterator iter_del = iter;
			   iter_del ++;
			   selected.erase(iter);
			   iter = iter_del;
		    }
		}
		else if(obj->base->id == 777 || obj->base->id == 779) //乾坤珠(高级)
		{
			charbase.doublectime += 86400;
			charbase.bitmask |= CHARBASE_EXP175;
			showCurrentEffect(Cmd::USTATE_EXP_175, true);
			srs->states[0].state = Cmd::USTATE_EXP_175;
			//soke 增加时间显示
			Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"1.75倍经验加成时间还剩:%d天 %d小时 %d分钟 ",charbase.doublectime/86400,(charbase.doublectime%86400)/3600,(charbase.doublectime%3600)/60);

			srs->states[0].result=charbase.doublectime/60;
		    srs->states[0].time=charbase.doublectime/60;
		    sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
		    SelectedSet_iterator iter = selected.begin();
		    for(; iter != selected.end() ;)
		    {
			   SceneUser* pUser = SceneUserManager::getMe().getUserByTempID(*iter);
			   if (pUser)
			   {
				   if (this->scene->checkTwoPosIInNine(this->getPosI(),pUser->getPosI()))
				   {
					   pUser->sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
					   iter ++ ;
					   continue;
				   }
			   }
			   SelectedSet_iterator iter_del = iter;
			   iter_del ++;
			   selected.erase(iter);
			   iter = iter_del;
		    }
		}

		//soke 物品使用成功向客户端发送成功消息
		Cmd::stItemUseItemSuccessUserCmd SuccessSend;
		SuccessSend.ItemID = obj->base->id;
		sendCmdToMe(&SuccessSend, sizeof(Cmd::stItemUseItemSuccessUserCmd));

		if(obj->data.dwNum>1)
		{
			obj->data.dwNum--;
			Cmd::stRefCountObjectPropertyUserCmd send;
			send.qwThisID=obj->data.qwThisID;
			send.dwNum=obj->data.dwNum;
			sendCmdToMe(&send,sizeof(send));
			//fprintf(stderr,"删除经验物品");
		}
		else
		{
			return packs.removeObject(obj);
		}
	}
	//soke  73类物品使用
	else if(obj->base->kind == ItemType_Tianxia)
	{
		zObject* srcobj=packs.uom.getObjectByThisID(obj->data.qwThisID);

		if(srcobj == NULL)
		{
			return false;
		}

		if(obj->base->id == 850)
		{
		   if(isSpa())
	       {
		       Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "游泳状态下不能使用飞行道具");
		       return false;
	       }
		    if(isBike())
	       {
		       Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "自行车比赛状态下不能使用飞行道具");
		       return false;
	       }
		     if(isZC())
	       {
		       Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "战车状态下不能使用飞行道具");
		       return false;
	       }
	       if(isWabao())
	       {
		      Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "挖宝状态下不能使用飞行道具");
		      return false;
	       }

		   beginFly();
		}

		if(obj->base->id == 1137)
		{
		   DWORD gold = 0; //50两金子 桃子删除
		   packs.addGold(gold,"兑换金子");
		   Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "使用天下令获得金子0两");
		}

		//soke  物品使用成功向客户端发送成功消息
		Cmd::stItem6UseItemSuccessUserCmd SuccessSend;
		SuccessSend.ItemID = obj->base->id;
		sendCmdToMe(&SuccessSend, sizeof(Cmd::stItem6UseItemSuccessUserCmd));

		if(obj->data.dwNum>1)
		{
			obj->data.dwNum--;
			Cmd::stRefCountObjectPropertyUserCmd send;
			send.qwThisID=obj->data.qwThisID;
			send.dwNum=obj->data.dwNum;
			sendCmdToMe(&send,sizeof(send));
			//fprintf(stderr,"删除经验物品");
		}
		else
		{
			return packs.removeObject(obj);
		}
	}
	//soke 幸运丹物品时间
	else if(obj->base->kind == ItemType_LuncyDan)
	{
		zObject* srcobj=packs.uom.getObjectByThisID(obj->data.qwThisID);

		if(srcobj == NULL)
		{
			return false;
		}

        //soke 幸运丹限制次数
		if (this->charbase.luckyCount>2)
		{
			Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您今日已经使用过3次幸运丹了");
			return false;
		}

		char Buf[200]; 
		bzero(Buf,sizeof(Buf));
		Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
		constructInPlace(srs);
		srs->byType = Cmd::MAPDATATYPE_USER;
		srs->dwTempID = this->tempid;
		srs->size=1;

		//soke 清除状态,防止叠加时间显示错误
		showCurrentEffect(Cmd::USTATE_LUCKY_DAN, false);

		if(obj->base->id == 1272)   //幸运丹
		{
			charbase.luckyCount++;
			charbase.luckytime += 300;
			charbase.bitmask |= CHARBASE_LUNCKY;
			showCurrentEffect(Cmd::USTATE_LUCKY_DAN, true);
			srs->states[0].state = Cmd::USTATE_LUCKY_DAN;
            //soke 增加时间显示
			Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"幸运丹剩余时间:%d小时 %d分钟 ",(charbase.luckytime%86400)/3600,(charbase.luckytime%3600)/60);
		}

		srs->states[0].result=charbase.luckytime/60;
		srs->states[0].time=charbase.luckytime/60;
		sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
		SelectedSet_iterator iter = selected.begin();
		for(; iter != selected.end() ;)
		{
			SceneUser* pUser = SceneUserManager::getMe().getUserByTempID(*iter);
			if (pUser)
			{
				if (this->scene->checkTwoPosIInNine(this->getPosI(),pUser->getPosI()))
				{
					pUser->sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
					iter ++ ;
					continue;
				}
			}
			SelectedSet_iterator iter_del = iter;
			iter_del ++;
			selected.erase(iter);
			iter = iter_del;
		}

		//soke 物品使用成功向客户端发送成功消息
		Cmd::stItem7UseItemSuccessUserCmd SuccessSend;
		SuccessSend.ItemID = obj->base->id;
		sendCmdToMe(&SuccessSend, sizeof(Cmd::stItem7UseItemSuccessUserCmd));

		if(obj->data.dwNum>1)
		{
			obj->data.dwNum--;
			Cmd::stRefCountObjectPropertyUserCmd send;
			send.qwThisID=obj->data.qwThisID;
			send.dwNum=obj->data.dwNum;
			sendCmdToMe(&send,sizeof(send));
			//fprintf(stderr,"删除经验物品");
		}
		else
		{
			return packs.removeObject(obj);
		}
	}

	//soke 使用回城卷等物品
	else if(obj->base->id==672 || obj->base->id==673 || obj->base->id==674 || obj->base->id==675 || obj->base->id==676 || obj->base->kind==ItemType_Scroll)//TODO 卷轴类使用
	{
		useScroll(obj);
	}
	//soke 增加15类型新物品
	else if(obj->base->kind==ItemType_Scrxll)
	{
	    
		//soke 增加卡图自救
		if(obj->base->id==1401 && this->scene->getRealMapID() != 1111)
	    {
		//soke 古墓传送增加个坐标
		if(charbase.country == 12)
		{
		  Gm::gomap(this, "name=楚国·王城 pos=875,500");
		}
		else if(charbase.country == 11)
		{
		  Gm::gomap(this, "name=周国·王城 pos=875,500");
		}
		else if(charbase.country == 10)
		{
		  Gm::gomap(this, "name=赵国·王城 pos=875,500");
		}
		else if(charbase.country == 9)
		{
		  Gm::gomap(this, "name=吴国·王城 pos=875,500");
		}
		else if(charbase.country == 8)
		{
		  Gm::gomap(this, "name=汉国·王城 pos=875,500");
		}
		else if(charbase.country == 7)
		{
		  Gm::gomap(this, "name=唐国·王城 pos=875,500");
		}
		else if(charbase.country == 5)
		{
		  Gm::gomap(this, "name=燕国·王城 pos=875,500");
		}
		else if(charbase.country == 4)
		{
		  Gm::gomap(this, "name=齐国·王城 pos=875,500");
		}
		else if(charbase.country == 3)
		{
		  Gm::gomap(this, "name=魏国·王城 pos=875,500");
		}
		else if(charbase.country == 2)
		{
		  Gm::gomap(this, "name=宋国·王城 pos=875,500");
		}
	   }

		useScrxll(obj);
	}

	//soke 78类合成魂魄石物品使用
	else if(obj->base->kind == ItemType_HzStone)
	{
		zObject* srcobj=packs.uom.getObjectByThisID(obj->data.qwThisID);

		if(srcobj == NULL)
		{
			return false;
		}
		
		//soke 魂魄石（力量）
		if(obj->base->id == 3500)
		{
          Gm::fetch(this,"id=680 monster=130 num=1 bind=1");
		}
		if(obj->base->id == 3501)
		{
          Gm::fetch(this,"id=748 monster=131 num=1 bind=1");
		}
		if(obj->base->id == 3502)
		{
          Gm::fetch(this,"id=749 monster=132 num=1 bind=1");
		}
		if(obj->base->id == 3503)
		{
          Gm::fetch(this,"id=750 monster=133 num=1 bind=1");
		}
		if(obj->base->id == 3504)
		{
          Gm::fetch(this,"id=751 monster=134 num=1 bind=1");
		}
		//soke 魂魄石（智力）
		if(obj->base->id == 3510)
		{
          Gm::fetch(this,"id=680 monster=140 num=1 bind=1");
		}
		if(obj->base->id == 3511)
		{
          Gm::fetch(this,"id=748 monster=141 num=1 bind=1");
		}
		if(obj->base->id == 3512)
		{
          Gm::fetch(this,"id=749 monster=142 num=1 bind=1");
		}
		if(obj->base->id == 3513)
		{
          Gm::fetch(this,"id=750 monster=143 num=1 bind=1");
		}
		if(obj->base->id == 3514)
		{
          Gm::fetch(this,"id=751 monster=144 num=1 bind=1");
		}
		//soke 魂魄石（敏捷）
		if(obj->base->id == 3520)
		{
          Gm::fetch(this,"id=680 monster=150 num=1 bind=1");
		}
		if(obj->base->id == 3521)
		{
          Gm::fetch(this,"id=748 monster=151 num=1 bind=1");
		}
		if(obj->base->id == 3522)
		{
          Gm::fetch(this,"id=749 monster=152 num=1 bind=1");
		}
		if(obj->base->id == 3523)
		{
          Gm::fetch(this,"id=750 monster=153 num=1 bind=1");
		}
		if(obj->base->id == 3524)
		{
          Gm::fetch(this,"id=751 monster=154 num=1 bind=1");
		}
		//soke 魂魄石（精神）
		if(obj->base->id == 3530)
		{
          Gm::fetch(this,"id=680 monster=160 num=1 bind=1");
		}
		if(obj->base->id == 3531)
		{
          Gm::fetch(this,"id=748 monster=161 num=1 bind=1");
		}
		if(obj->base->id == 3532)
		{
          Gm::fetch(this,"id=749 monster=162 num=1 bind=1");
		}
		if(obj->base->id == 3533)
		{
          Gm::fetch(this,"id=750 monster=163 num=1 bind=1");
		}
		if(obj->base->id == 3534)
		{
          Gm::fetch(this,"id=751 monster=164 num=1 bind=1");
		}
		//soke 魂魄石（体质）
		if(obj->base->id == 3535)
		{
          Gm::fetch(this,"id=680 monster=170 num=1 bind=1");
		}
		if(obj->base->id == 3536)
		{
          Gm::fetch(this,"id=748 monster=171 num=1 bind=1");
		}
		if(obj->base->id == 3537)
		{
          Gm::fetch(this,"id=749 monster=172 num=1 bind=1");
		}
		if(obj->base->id == 3538)
		{
          Gm::fetch(this,"id=750 monster=173 num=1 bind=1");
		}
		if(obj->base->id == 3539)
		{
          Gm::fetch(this,"id=751 monster=174 num=1 bind=1");
		}		
		//桃子 等级魂魄-任意部位 
		if(obj->base->id == 3540)
		{
          Gm::fetch(this,"id=680 monster=100 num=1 bind=1");
		}
		if(obj->base->id == 3541)
		{
          Gm::fetch(this,"id=748 monster=101 num=1 bind=1");
		}
		if(obj->base->id == 3542)
		{
          Gm::fetch(this,"id=749 monster=102 num=1 bind=1");
		}
		if(obj->base->id == 3543)
		{
          Gm::fetch(this,"id=750 monster=103 num=1 bind=1");
		}
		if(obj->base->id == 3544)
		{
          Gm::fetch(this,"id=751 monster=104 num=1 bind=1");
		}
		//soke 魂魄之心-任意部位上（增加状态）
		if(obj->base->id == 3550)
		{
          Gm::fetch(this,"id=680 monster=110 num=1 bind=1");
		}
		if(obj->base->id == 3551)
		{
          Gm::fetch(this,"id=680 monster=111 num=1 bind=1");
		}
		if(obj->base->id == 3552)
		{
          Gm::fetch(this,"id=680 monster=112 num=1 bind=1");
		}
		if(obj->base->id == 3553)
		{
          Gm::fetch(this,"id=680 monster=113 num=1 bind=1");
		}
		if(obj->base->id == 3554)
		{
          Gm::fetch(this,"id=680 monster=114 num=1 bind=1");
		}
		//soke 魂魄之心-任意部位上（减少状态）
		if(obj->base->id == 3560)
		{
          Gm::fetch(this,"id=680 monster=120 num=1 bind=1");
		}
		if(obj->base->id == 3561)
		{
          Gm::fetch(this,"id=680 monster=121 num=1 bind=1");
		}
		if(obj->base->id == 3562)
		{
          Gm::fetch(this,"id=680 monster=122 num=1 bind=1");
		}
		if(obj->base->id == 3563)
		{
          Gm::fetch(this,"id=680 monster=123 num=1 bind=1");
		}
		if(obj->base->id == 3564)
		{
          Gm::fetch(this,"id=680 monster=124 num=1 bind=1");
		}

		//soke 物品使用成功向客户端发送成功消息
		Cmd::stItem11UseItemSuccessUserCmd SuccessSend;
		SuccessSend.ItemID = obj->base->id;
		sendCmdToMe(&SuccessSend, sizeof(Cmd::stItem11UseItemSuccessUserCmd));

		if(obj->data.dwNum>1)
		{
			obj->data.dwNum--;
			Cmd::stRefCountObjectPropertyUserCmd send;
			send.qwThisID=obj->data.qwThisID;
			send.dwNum=obj->data.dwNum;
			sendCmdToMe(&send,sizeof(send));
			//fprintf(stderr,"删除经验物品");
		}
		else
		{
			return packs.removeObject(obj);
		}
	}
	else if(obj->base->kind == ItemType_Add_Exploit) //128金符类
	{
		zObject* srcobj=packs.uom.getObjectByThisID(obj->data.qwThisID);

		if(srcobj == NULL)
		{
			return false;
		}
		
		//soke 增加金符类使用，不在需要脚本了
		if(obj->base->id == 965)
		{
			//soke 使用铜符 计算公式 10*人物等级*人物等级
			QWORD exp = (10 * charbase.level * charbase.level);
			addExp(exp);
		}
		if(obj->base->id == 966)
		{
			//soke 使用银符 计算公式 40*人物等级*人物等级
			QWORD exp = (40 * charbase.level * charbase.level);
			addExp(exp);
		}
		if(obj->base->id == 967)
		{
			//soke 使用金符 计算公式 200*人物等级*人物等级
			QWORD exp = (200 * charbase.level * charbase.level);
			addExp(exp * 2);
		}
		if(obj->base->id == 968)
		{
			//soke 使用玉符 计算公式 2000*人物等级*人物等级
			QWORD exp = (2000 * charbase.level * charbase.level);
			addExp(exp);
		}
		if(obj->base->id == 969)
		{
			//soke 使用御赐金符 计算公式 10000*人物等级*人物等级
			QWORD exp = (10000 * charbase.level * charbase.level);
			addExp(exp);
		}

		//soke 物品使用成功向客户端发送成功消息
		Cmd::stItem11UseItemSuccessUserCmd SuccessSend;
		SuccessSend.ItemID = obj->base->id;
		sendCmdToMe(&SuccessSend, sizeof(Cmd::stItem11UseItemSuccessUserCmd));

		if(obj->data.dwNum>1)
		{
			obj->data.dwNum--;
			Cmd::stRefCountObjectPropertyUserCmd send;
			send.qwThisID=obj->data.qwThisID;
			send.dwNum=obj->data.dwNum;
			sendCmdToMe(&send,sizeof(send));
			//fprintf(stderr,"删除经验物品");
		}
		else
		{
			return packs.removeObject(obj);
		}
	}
	else if((obj->base->id>=671 && obj->base->id<=676) || obj->base->id==909 || obj->base->kind==ItemType_Scroll)//TODO 卷轴类使用
	{
		useScroll(obj);
	}
	//soke 使用坐骑
	else if(obj->base->kind==ItemType_HORSE)
	{
		if (horse.horse())
		{
			if (ridepet)
				horse.putAway();
			if (horse.mount())
				horse.mount(false);
			horse.horse((DWORD)0);
		}
		horse.horse(obj->base->id);
	}

	//soke 附带技能类道具
	else if (obj->base->kind == ItemType_UseSkill)
	{
		useSkill(obj);
	}

	//soke 使用马铠
	else if (obj->base->kind == ItemType_BMW)
	{
		/*
		if (!horse.horse())
		{
			Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你还没有马，不能使用马甲");
			return true;
		}
		if (charbase.level<100 || (obj->base->id==878 && charbase.level<120))
		{
			Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的等级不够");
			return true;
		}
		*/
		if (horse.horse(obj->data))
		{
			zObject::logger(obj->createid,obj->data.qwThisID,obj->base->name,obj->data.dwNum,obj->data.dwNum,0,0,NULL,id,name,"使用变成战马",NULL,0,0);

			packs.removeObject(obj, true, true);
		}
	}
	//使用护身符
	/*
	   else if (obj->base->kind == ItemType_Amulet)
	   {
	   useAmulet(obj);
	   }
	// */

	//soke 使用令牌
	else if(obj->base->kind == ItemType_KING || obj->base->kind == ItemType_TONG ||obj->base->kind == ItemType_FAMILY) 
	{
		Cmd::Session::t_Check_CallTimes_SceneSession send;
		switch(obj->base->kind)
		{
			case ItemType_TONG:
				{
					send.type = Cmd::CALL_DUTY_UNION;
					if(!this->unionMaster)
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "只有帮主才可以使用帮主令");
						return true;
					}
					
					if (this->quest_list.state(91030)) //取消家族车无法使用家族帮会令牌问题
					{// 家族运镖不能使用帮主令
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "家族运镖期间不能使用帮主令");
						return true;
					}
				}
				break;
			case ItemType_FAMILY:
				{
					send.type = Cmd::CALL_DUTY_SEPT;
					if(!this->septMaster)
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "只有族长才可以使用族长令");
						return true;
					}

					if (this->quest_list.state(91030)) //取消家族车无法使用家族帮会令牌问题
					{// 家族运镖不能使用族长令
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "家族运镖期间不能使用族长令");
						return true;
					}
				}
				break;
			case ItemType_KING:
				{
					send.type = Cmd::CALL_DUTY_KING;
					if(!this->king)
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "只有国王才可以使用国王令");
						return true;
					}
				}
				break;
			default:
				return true;
				break;
		}
		//于子取消国王令限制
		// if (!this->isSpecWar(Cmd::COUNTRY_FORMAL_DARE) && (this->charbase.country != this->scene->getCountryID() && 6 != this->scene->getCountryID()))
		// {
		// 	Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "现在不能使用该道具");
		// 	return true;
		// }

		if (this->scene->getRealMapID()>=223 && this->scene->getRealMapID()<=247 )//在迷宫内
		{
			Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "本地图不能使用该道具");
			return true;
		}
		
		//醉梦修复动态地图 
		//检查本地图是否可用
		if( !strstr(this->scene->getRealName(),"家族地图"))//在家族地图
		{	
			if(this->scene->isDynamic())
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "本地图不能使用该道具");
				return true;
			}
		}
		
		// if(this->scene->checkCallObj())
		// {
		// 	Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "本地图不能使用该道具");
		// 	return true;
		// }
		
		// //检查在国外特殊地图是否可用
		// if(this->charbase.country != this->scene->getCountryID() && this->scene->getRealMapID() == 139)
		// {
		// 	return true;
		// }
		send.leaderTempID = this->tempid;
		send.qwThisID = obj->data.qwThisID;
		sessionClient->sendCmd(&send, sizeof(send));
		useCallObj(obj);
	}

	else if(obj->base->kind == ItemType_Gift)
   	{

		Zebra::logger->debug("%s(%ld)使用宝箱",name,id);

		zNpcB *base = NULL;
        	base = npcbm.get(obj->base->id+ItemType_Gift*10000);//soke 先算x 在算加
		if(base == NULL)
		{
			Zebra::logger->error("%s(%ld)无法获取NPC",name,id);
			return false;
		}
		t_NpcDefine box_define;
	        box_define.id = base->id;
       		strncpy(box_define.name, base->name, MAX_NAMESIZE-1);
                box_define.pos = getPos();
        	box_define.num = 1;
	        box_define.interval = 5;
        	box_define.initstate = zSceneEntry::SceneEntry_Normal;
  	      	box_define.width = CALL_PET_REGION;
        	box_define.height = CALL_PET_REGION;
        	box_define.pos -= zPos(CALL_PET_REGION/2,CALL_PET_REGION/2);
        	box_define.scriptID = 0;

		scene->initRegion(box_define.region, box_define.pos, box_define.width, box_define.height);

	        SceneNpc * boxPet = 0;
                boxPet = scene->summonOneNpc<SceneNpc>(box_define,zPos(0,0), base);

		if(!boxPet)
			Zebra::logger->error("%s(%ld)无法使用宝箱",name,id);

		packs.removeObject(obj);	
		return true;	
	}
	else
		return false;
	return true;
}
/*
 * 得到外挂日志
 *
 */
void SceneUser::getWgLog(DWORD data)
{
#ifdef _XWL_DEBUG
		//Zebra::logger->debug("%s 外挂日志 %u %u %u %u %u", name, data, ((BYTE *)&data)[0], ((BYTE *)&data)[1], ((BYTE *)&data)[2], ((BYTE *)&data)[3]);
#endif
		for (DWORD i=0; i<sizeof(data); i++)
		{
				//if (2==i) continue;

				if (0==i%2)
						((BYTE *)&data)[i] ^= 'F';
				else
						((BYTE *)&data)[i] ^= 'X';
		}

		if ((!(((BYTE *)&data)[2] & 0x80)) && (!wg_log_len)) return;

		if (!wg_log_len)
		{
				wg_log_len = ((BYTE *)&data)[3];
#ifdef _XWL_DEBUG
				//Zebra::logger->debug("%s 外挂日志长度%u", name, wg_log_len);
#endif
				return;
		}

		/*
		DWORD len = strlen(wg_log);
		for (int i=0; i<3; i++)
		{
				BYTE b = ((BYTE *)&data)[0];

				if (0==len+i)
						wg_log[0] = b;
				else
		}
		*/

		//wg_log[strlen(wg_log)] = ((BYTE *)&data)[0];
		//wg_log[strlen(wg_log)+1] = ((BYTE *)&data)[1];
		//wg_log[strlen(wg_log)+2] = ((BYTE *)&data)[2];
		//wg_log[strlen(wg_log)+3] = ((BYTE *)&data)[3];
		bcopy(&data, &wg_log[strlen(wg_log)], sizeof(DWORD));
		wg_log_len -= wg_log_len <? 4;
#ifdef _XWL_DEBUG
		//Zebra::logger->debug("%s 外挂日志 %s 长度%u", name, (char *)&data, wg_log_len);
#endif

		if (0==wg_log_len)
		{
				ScenesService::wg_logger->trace("[征途菲菲]%s,%u,%u,%u,%s,%u,%s",
								name, id, accid, charbase.level,
								SceneManager::getInstance().getCountryNameByCountryID(charbase.country),
								charbase.gold, wg_log);
				bzero(wg_log, sizeof(wg_log));
		}
}

/**
 * \brief 处理角色向前移动命令
 *
 * \param rev 移动命令
 *
 * \return 移动命令处理成功返回TRUE,否则返回FALSE
 */
bool SceneUser::move(Cmd::stUserMoveMoveUserCmd *rev)
{
	//soke 移动做下温泉地图判断
	if (scene->getRealMapID() == 279 || scene->getRealMapID() == 280)
	{
		//soke 温泉参数
		bool isInSpaZone = scene->checkZoneType(pos, ZoneTypeDef::ZONE_SPA_WARM) || scene->checkZoneType(pos, ZoneTypeDef::ZONE_SPA_HOT);
		if ( isInSpaZone )
		{
			// 更新客户端温泉状态
			showCurrentEffect( Cmd::USTATE_SPA, true );
		}
		else
		{
			showCurrentEffect( Cmd::USTATE_SPA, false );
		}
	}

	if (isSitdown())
	{
		//打坐状态，首先站立
		standup();
	}
	if (tradeorder.hasBegin()) 
	{
		//交易状态，取消交易
		tradeorder.cancel();
	}

	this->keepDir = rev->byDirect;
	bool canmove = false;
	Cmd::stUserMoveMoveUserCmd ret;
	ret.dwUserTempID=tempid;

	//检查外挂日志
	getWgLog(rev->dwUserTempID);

	if(backOffing)
	{
		//如果正在后退中则不能前进,直接返回自己当前坐标
		ret.byDirect=getDir();
		ret.bySpeed=0;
		ret.x=getPos().x;
		ret.y=getPos().y;
		sendCmdToMe(&ret,sizeof(ret));
		return true;
	}
	int xlen = abs(pos.x - rev->x);
	int ylen = abs(pos.y - rev->y);
	//检测移动间隔
	if ((!speedOutMove(rev->dwTimestamp, getMyMoveSpeed(), (xlen>ylen)?xlen:ylen))&&(!this->dread))
	{
		ret.byDirect=getDir();
		ret.bySpeed=0;
		ret.x=getPos().x;
		ret.y=getPos().y;
		sendCmdToMe(&ret,sizeof(ret));
		return true;
	}

	if (this->isQuiz)
	{
		//答题状态，不能移动
		ret.byDirect=getDir();
		ret.bySpeed=0;
		ret.x=getPos().x;
		ret.y=getPos().y;
		sendCmdToMe(&ret,sizeof(ret));
		return true;
	}

	setDir(rev->byDirect);
	lastPos2 = lastPos1;
	lastPos1 = pos;

	zPosI oldPosI=getPosI();
	zPos oldPos=getPos();

	WORD speed=rev->bySpeed>3?1:rev->bySpeed;
    //soke 增加飞行
	if(horse.mount() || isFly())
	{
		//骑马状态
		if(speed==2) speed=3;
	}
    else if ( isSpa() )
    {
        speed = 1;
    }
	else if ( isDance() )
    {
        speed = 0;
    }
	else
	{
		//普通状态
		if(speed==3) speed=2;
	}
	//保证坐标合法
	if( (xlen + ylen) <= ((getDir()%2)?(speed*2):speed)) 
	{
		zPos newPos(rev->x, rev->y);
		if(moveAction&&(!scene->checkBlock(newPos,TILE_BLOCK) && (!scene->checkBlock(newPos,TILE_ENTRY_BLOCK)|| this->liquidState)))
		{
			if (scene->refresh(this,newPos))
			{
				//高8位表示跑得步数，低8位表示走的步数
				if (!horse.mount() && speed==2) step_state += 0x0100;
				if (!horse.mount() && speed==1) ++step_state;

				canmove = true;
				scene->setBlock(newPos);
				scene->clearBlock(oldPos);

				ret.byDirect=getDir();
				ret.bySpeed=speed;

                ret.x=newPos.x;
				ret.y=newPos.y;
				//检查是否隐身
				if (SceneEntry_Hide!=getState()&&!this->hideme)
					scene->sendCmdToNine(oldPosI,&ret,sizeof(ret));
				else
				{
					//隐身则不占用block
					scene->clearBlock(newPos);
					sendCmdToMe(&ret,sizeof(ret));
					scene->sendCmdToWatchTrap(oldPosI,&ret,sizeof(ret));
				}

				if(oldPosI != getPosI())
				{
					Cmd::stRemoveUserMapScreenUserCmd removeUser;
					removeUser.dwUserTempID = tempid;
					scene->sendCmdToReverseDirect(oldPosI,
					scene->getScreenDirect(oldPosI , getPosI()), &removeUser, sizeof(removeUser));

					sendMeToNineDirect(scene->getScreenDirect(oldPosI , getPosI()));
					sendNineToMeDirect(scene->getScreenDirect(oldPosI , getPosI()));

					//校验9屏所有玩家以及Npc坐标
#if 0
					GetAllPos allNpcs(Cmd::MAPDATATYPE_NPC, this);
					GetAllPos allUsers(Cmd::MAPDATATYPE_USER, this);
					const zPosIVector &pv = scene->getNineScreen(getPosI());
					//{
					for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
					{
						//Zebra::logger->debug("%s: %u", __FUNCTION__, *it);
						scene->execAllOfScreen(zSceneEntry::SceneEntry_NPC, *it,allNpcs);
						scene->execAllOfScreen(zSceneEntry::SceneEntry_Player, *it,allUsers);
					}
					//}
					if (allNpcs.canSend())
					{
						sendCmdToMe(allNpcs.getSendBuffer(),allNpcs.getSize());
					}
					if (allUsers.canSend())
					{
						sendCmdToMe(allUsers.getSendBuffer(),allUsers.getSize());
					}
#else
					GetAllRemovePosNpc allNpcs(this);
					GetAllRemovePosUser allUsers(this);
					//const zPosIVector &pv = scene->getNineScreen(getPosI());
					const zPosIVector &pv = scene->getReverseDirectScreen(oldPosI,scene->getScreenDirect(oldPosI,getPosI()));
					//{
					for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
					{
						scene->execAllOfScreen(zSceneEntry::SceneEntry_NPC, *it,allNpcs);
						scene->execAllOfScreen(zSceneEntry::SceneEntry_Player, *it,allUsers);
					}
					//}
					if (allNpcs.canSend())
					{
						sendCmdToMe(allNpcs.getSendBuffer(),allNpcs.getSize());
					}
					if (allUsers.canSend())
					{
						sendCmdToMe(allUsers.getSendBuffer(),allUsers.getSize());
					}
#endif
				}
				//检查是否在跳转点
				if(scene->checkBlock(newPos, TILE_DOOR))
				{
					const WayPoint *wp=scene->getWayPoint(newPos);
					if(wp) {
						const Point *dp=wp->getRandDest();
						if(dp) {
							Scene *toscene=SceneManager::getInstance().getSceneByFileName(dp->name);
							if(toscene)
							{
								changeMap(toscene,dp->pos);
							}
							else 
							{
								if(this->scene->getCountryID() == 6)
								{
									std::string fileName;
									char temp[128];
									bzero(temp,sizeof(temp));
									sprintf(temp , "%d" , this->charbase.country);
									fileName = temp;
									fileName+= ".";
									fileName+= this->scene->getRealFileName(dp->name);
									toscene=SceneManager::getInstance().getSceneByFileName(fileName.c_str());
									if(toscene)
									{
										changeMap(toscene,dp->pos);
									}
									else
									{
										if(this->scene->getCountryID() == 6)
										{
											std::string fileName;
											char temp[128];
											bzero(temp,sizeof(temp));
											sprintf(temp , "%d" , this->charbase.country);
											fileName = temp;
											fileName+= ".";
											fileName+= this->scene->getRealFileName(dp->name);
											toscene=SceneManager::getInstance().getSceneByFileName(fileName.c_str());
											if(toscene)
											{
												changeMap(toscene,dp->pos);
											}
											else
											{
												if (guard && guard->canMove()) saveGuard = true;
												if (adoptList.size()) saveAdopt = true;
												Cmd::Session::t_changeScene_SceneSession cmd;
												//Zebra::logger->debug("切换场景服务器(%s, %d, %d)", fileName.c_str(), dp->pos.x, dp->pos.y);
												cmd.id = id;
												cmd.temp_id = tempid;
												cmd.x = dp->pos.x;
												cmd.y = dp->pos.y;
												cmd.map_id = 0;
												strncpy((char *)cmd.map_file, fileName.c_str(), MAX_NAMESIZE);
												sessionClient->sendCmd(&cmd, sizeof(cmd));
											}
										}
										else
										{
											if (guard && guard->canMove()) saveGuard = true;
											if (adoptList.size()) saveAdopt = true;
											Cmd::Session::t_changeScene_SceneSession cmd;
											//Zebra::logger->debug("切换场景服务器(%s, %d, %d)", dp->name, dp->pos.x, dp->pos.y);
											cmd.id = id;
											cmd.temp_id = tempid;
											cmd.x = dp->pos.x;
											cmd.y = dp->pos.y;
											cmd.map_id = 0;
											strncpy((char *)cmd.map_file, dp->name, MAX_NAMESIZE);
											sessionClient->sendCmd(&cmd, sizeof(cmd));
										}
									}
								}
								else
								{
									if (guard && guard->canMove()) saveGuard = true;
									if (adoptList.size()) saveAdopt = true;
									Cmd::Session::t_changeScene_SceneSession cmd;
									//Zebra::logger->debug("切换场景服务器(%s, %d, %d)", dp->name, dp->pos.x, dp->pos.y);
									cmd.id = id;
									cmd.temp_id = tempid;
									cmd.x = dp->pos.x;
									cmd.y = dp->pos.y;
									cmd.map_id = 0;
									strncpy((char *)cmd.map_file, dp->name, MAX_NAMESIZE);
									sessionClient->sendCmd(&cmd, sizeof(cmd));
								}
							}
						}
					}
				}
			}
		}
	}
	if (!canmove)
	{
		ret.byDirect=getDir();
		ret.bySpeed=0;
		ret.x=getPos().x;
		ret.y=getPos().y;
		sendCmdToMe(&ret,sizeof(ret));
		//不能移动，试图清空前面阻挡，避免空气墙
		zPos p(rev->x, rev->y);
		scene->clearBlock(p);
	}
	return true;
}

/**
 * \brief 处理角色后退移动命令
 *
 *
 * \param direct 移动方向
 * \param grids 格数
 *
 * \return 移动命令处理成功返回TRUE,否则返回FALSE
 */
bool SceneUser::backOff(const int direct, const int grids)
{
	const int walk_adjust[9][2]= { {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, 0} };
	int i = 0;

	zPosI oldPosI = getPosI();
	zPos oldPos = getPos(),  newPos = getPos();
	for(i = 1; i <= grids; i++)
	{
		newPos.x += walk_adjust[direct][0];
		newPos.y += walk_adjust[direct][1];
		if(scene->checkBlock(newPos))
			break;
	}
	if (i > 1)
	{
		Zebra::logger->debug("后退前坐标(%u,%u)", oldPos.x, oldPos.y);
		if(scene->refresh(this , newPos))
		{
			scene->setBlock(newPos);
			scene->clearBlock(oldPos);
			setDir(scene->getReverseDirect(direct));

			Zebra::logger->debug("后退成功坐标(%u,%u)", newPos.x, newPos.y);
			Cmd::stBackOffMagicUserCmd ret;
			ret.dwTempID = this->tempid;
			ret.byType = Cmd::MAPDATATYPE_USER;
			ret.byDirect = direct;
			ret.x = newPos.x;
			ret.y = newPos.y;
			//检查是否隐身
			if (SceneEntry_Hide!=getState()&&!this->hideme)
				scene->sendCmdToNine(oldPosI,&ret,sizeof(ret));
			else
			{
				//隐身则不占用block
				scene->clearBlock(newPos);
				sendCmdToMe(&ret,sizeof(ret));
			}
			if(oldPosI != getPosI())
			{
				Cmd::stRemoveUserMapScreenUserCmd removeUser;
				removeUser.dwUserTempID = tempid;
				scene->sendCmdToReverseDirect(oldPosI,
						scene->getScreenDirect(oldPosI , getPosI()), &removeUser, sizeof(removeUser));
				sendMeToNineDirect(scene->getScreenDirect(oldPosI , getPosI()));
				sendNineToMeDirect(scene->getScreenDirect(oldPosI , getPosI()));

				//校验9屏所有玩家以及Npc坐标
#if 0
				GetAllPos allNpcs(Cmd::MAPDATATYPE_NPC, this);
				GetAllPos allUsers(Cmd::MAPDATATYPE_USER, this);
				const zPosIVector &pv = scene->getNineScreen(getPosI());
				for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
				{
					//Zebra::logger->debug("%s: %u", __FUNCTION__, *it);
					scene->execAllOfScreen(zSceneEntry::SceneEntry_NPC, *it,allNpcs);
					scene->execAllOfScreen(zSceneEntry::SceneEntry_Player, *it,allUsers);
				}
				if (allNpcs.canSend())
				{
					sendCmdToMe(allNpcs.getSendBuffer(),allNpcs.getSize());
				}
				if (allUsers.canSend())
				{
					sendCmdToMe(allUsers.getSendBuffer(),allUsers.getSize());
				}
#else
					GetAllRemovePosNpc allNpcs(this);
					GetAllRemovePosUser allUsers(this);
					//const zPosIVector &pv = scene->getNineScreen(getPosI());
					const zPosIVector &pv = scene->getReverseDirectScreen(oldPosI,scene->getScreenDirect(oldPosI,getPosI()));
					//{
					for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
					{
						//Zebra::logger->debug("%s: %u", __FUNCTION__, *it);
						scene->execAllOfScreen(zSceneEntry::SceneEntry_NPC, *it,allNpcs);
						scene->execAllOfScreen(zSceneEntry::SceneEntry_Player, *it,allUsers);
					}
					//}
					if (allNpcs.canSend())
					{
						sendCmdToMe(allNpcs.getSendBuffer(),allNpcs.getSize());
					}
					if (allUsers.canSend())
					{
						sendCmdToMe(allUsers.getSendBuffer(),allUsers.getSize());
					}
#endif
			}
		}
	}
	backOffing = 1000;
	return true;
}

/**
 * \brief 根据场景计算玩家死亡后应该回到的地图
 *
 * \param s 场景
 *
 */
void SceneUser::setDeathBackToMapID(Scene *s)
{
	if(s->getCountryID() == 6)
	{
		if (s->getCommonCountryBacktoMapID())
		{
			deathBackToMapID = (s->getCountryID() << 16 ) + s->getCommonCountryBacktoMapID();
		}
		else
		{
			deathBackToMapID = (this->charbase.country << 16 ) + s->getForeignerBacktoMapID();
		}
	}
	else
	{
		if(this->charbase.country == 6)
		{
			deathBackToMapID = (this->charbase.country << 16 ) + s->getCommonUserBacktoMapID();
		}
		else
		{
			deathBackToMapID = (this->charbase.country << 16 ) + s->getForeignerBacktoMapID();
		}
	}
}

/**
 * \brief 使角色进入一个指定的场景的指定位置
 *
 * \param newscene 要进入的场景
 * \param needInitInfo 进入场景所需要的一些初始化信息
 * \param initPos 进入的坐标位置
 *
 */
bool SceneUser::intoScene(Scene *newscene,bool needInitInfo,const zPos & initPos)
{
	zPos findedPos;
	//if(scene!=NULL || newscene==NULL /*&& !newscene->addUser(this)*/)
	if(newscene==NULL /*&& !newscene->addUser(this)*/)
	{
		Zebra::logger->error("%s(%d)进入场景%s失败",name,id,newscene?newscene->name:"");
		return false;
	}
	bool founded=false;
	if(this->charbase.gomaptype)
	{
		founded = newscene->randzPosByZoneType(this->charbase.gomaptype , findedPos);
		if(!founded)
		{
			founded = newscene->randzPosByZoneType(ZoneTypeDef::ZONE_NONE , findedPos);
		}
		this->charbase.gomaptype=0;
	}
	else
	{
		if((!initPos.x && !initPos.y) || (initPos.x > newscene->width() || initPos.y > newscene->height()))
		{
			if(this->charbase.country == newscene->getCountryID())
			{
				founded = newscene->randzPosByZoneType(ZoneTypeDef::ZONE_RELIVE , findedPos);
			}
			else
			{
				founded = newscene->randzPosByZoneType(ZoneTypeDef::ZONE_FOREIGN_RELIVE , findedPos);
				if(!founded)
				{
					founded = newscene->randzPosByZoneType(ZoneTypeDef::ZONE_RELIVE , findedPos);
				}
			}
			// */
			if(!founded)
			{
				founded = newscene->randzPosByZoneType(ZoneTypeDef::ZONE_NONE , findedPos);
			}
		}
		else
		{
			founded = newscene->findPosForUser(initPos, findedPos);
			if(!founded)
			{
				founded = newscene->randzPosByZoneType(ZoneTypeDef::ZONE_NONE , findedPos);
			}
		}
	}
	if(newscene->refresh(this,founded ? findedPos : initPos))
	{
		//本地图不能组队
		if(newscene->noTeam())
		{
			leaveTeam(); 
		}
		//通知网关用户刷新Scene服务器
		Cmd::Scene::t_Refresh_LoginScene refresh;
		refresh.dwUserID=id;
		refresh.dwSceneTempID=newscene->tempid;
		gatetask->sendCmd(&refresh,sizeof(refresh));
		//再刷屏索引到网关
		newscene->freshGateScreenIndex(this, getPosI());

		//设置死亡后回到的地图
		setDeathBackToMapID(newscene); 
		scene=newscene;
		scene->addUserCount();
		//Zebra::logger->debug("场景%s目前在线人数%u",scene->name,scene->countUser());
		//检查是否隐身
		if (SceneEntry_Hide!=getState()&&!this->hideme)
			newscene->setBlock(getPos());
		Zebra::logger->trace("将%s(%d)加入%s,本场景目前在线人数:%u",name,id,newscene->name, scene->countUser());
		set_me(this);
		if( 0 == strcmp(newscene->name,"中立区·功勋符地图"))//判断是否功勋竞猜地图
		{
			Cmd::stJingcaiReturnPigTimeCmd ret;
			ret.pigtime = atoi(Zebra::global["pigtime"].c_str());
			this->sendCmdToMe(&ret,sizeof(ret));
			
			Cmd::stJingcaiReturnKillPigTimeCmd ret2;
			ret2.killpigtime = atol(Zebra::global["killpigtime"].c_str());
			ret2.killpigtime_hm = atol(Zebra::global["killpigtime_hm"].c_str());
			this->sendCmdToMe(&ret2,sizeof(ret2));

		}
		//功能npc
		struct FunctionNpc : public zSceneEntryCallBack
		{
			Cmd::stMapScreenSizeDataUserCmd *mapscreen;
			FunctionNpc(Cmd::stMapScreenSizeDataUserCmd *buf)
			{
				mapscreen = buf;
			}

			bool exec(zSceneEntry *entry)
			{
				SceneNpc *npc = (SceneNpc *)entry;
				if (npc->npc)
				{
					mapscreen->npc_list[mapscreen->npc_count].id = npc->npc->id;
					mapscreen->npc_list[mapscreen->npc_count].x = npc->define->pos.x;
					mapscreen->npc_list[mapscreen->npc_count].y=npc->define->pos.y;
					++mapscreen->npc_count;
					if(mapscreen->npc_count >= 300)
					{
						return false;
					}
				}
				return true;
			}
		};


		//this->skillStatusM.processPassiveness();

		//地图信息
		Cmd::stMapScreenSizeDataUserCmd *mapscreen;
		char Buf[sizeof(Cmd::stMapScreenSizeDataUserCmd) + 300 * sizeof(mapscreen->npc_list[0])];
		bzero(Buf ,sizeof(Buf));
		mapscreen = (Cmd::stMapScreenSizeDataUserCmd *)Buf;
		constructInPlace(mapscreen);
		mapscreen->width=scene->width();
		mapscreen->height=scene->height();
		mapscreen->mainRoleX = getPos().x;
		mapscreen->mainRoleY = getPos().y;
		strncpy(mapscreen->pstrMapName,scene->getName(),MAX_NAMESIZE);
		strncpy(mapscreen->pstrFilename,scene->getRealFileName(),MAX_NAMESIZE);
		//Zebra::logger->debug("地图文件名:%s",mapscreen->pstrFilename);
		if(SceneManager::getInstance().getCountryNameByCountryID(scene->getCountryID()))
		{
			strncpy(mapscreen->pstrCountryName,SceneManager::getInstance().getCountryNameByCountryID(scene->getCountryID()),MAX_NAMESIZE);
		}
		mapscreen->setting=0;
		mapscreen->rgb=0;
		FunctionNpc npc_exec(mapscreen);
		scene->execAllOfScene_functionNpc(npc_exec);
		sendCmdToMe(mapscreen,sizeof(Cmd::stMapScreenSizeDataUserCmd)+mapscreen->npc_count*sizeof(mapscreen->npc_list[0]));
		//骑马检测
		if(!scene->canRide())
		{
			//dwHorseID=0;
			//clear_state(byState,Cmd::USTATE_RIDE);
			horse.mount(false,false);
			/*
			   Cmd::stRideMapScreenUserCmd ret;
			   ret.bySwitch=0;
			   ret.dwUserTempID=tempid;
			   scene->sendCmdToNine(getPosI(),&ret,sizeof(ret));
			   */
		}
		//必要时发送主用户信息
		if(needInitInfo)
		{
			sendInitToMe();
			MessageSystem::getInstance().check(this, true);
			OnEnter enter(1);
			EventTable::instance().execute(*this, enter);
			execute_script_event(this,"enter");
		}
		else
		{
			Cmd::stMainUserDataUserCmd  userinfo;
			full_t_MainUserData(userinfo.data);
			sendCmdToMe(&userinfo,sizeof(userinfo));
			sendInitHPAndMp();
			/*
			//结束初始化
			Cmd::stEndOfInitDataDataUserCmd  endcmd;
			sendCmdToMe(&endcmd,sizeof(endcmd));
			// */
		}

		if ((this->getPriv() & Gm::super_mode)||(this->getPriv() & Gm::gm_mode)||(this->getPriv() & Gm::captain_mode))
		{
			Gm::hideMe(this,"");
			Gm::god(this,"");
		}

		//通知网关用户场景变化
		Cmd::Scene::t_countryAndScene_GateScene noti;
		noti.userID = id;
		noti.countryID = charbase.country;
		noti.sceneID = SceneManager::getInstance().getMapIDByMapName(scene->name);
		gatetask->sendCmd(&noti, sizeof(noti));

		/*
		//宠物
		zPos petPos = getPos();
		for (std::list<ScenePet *>::iterator it=totems.begin(); it!=totems.end(); it++)
		{
		(*it)->changeMap(scene, petPos);
		}
		*/

	// */
		// if(this->scene->getRealMapID() == 10999 || this->scene->getRealMapID() >=10001 && this->scene->getRealMapID() <=10401)//如果是国家boss地图醉梦
		// {
		// 	Gm::gomap(this, "name=中立区·皇城 type=4");
		// }

		// 请求金币和月卡时间
		/*
		   Cmd::Scene::t_Request_Bill rb;
		   rb.dwUserID=this->id;
		   this->gatetask->sendCmd(&rb ,sizeof(rb));
		// */
		if(this->scene->getRealMapID() == 190)//如果是古墓
		{
			/*
			struct tm tm_1;
			time_t timValue = time(NULL);
			tm_1=*localtime(&timValue);
			if(tm_1.tm_hour%2 == 0 && tm_1.tm_min <= 55)
			{
			}
			else
			{
				Gm::gomap(this, "name=中立区·皇城 type=4");
			}
			// */
		}
		else if (this->scene->getRealMapID()==139
			&& this->charbase.country == this->scene->getCountryID() && this->scene->getCountryDare())
		{
			this->deathBackToMapID = SceneManager::getInstance().buildMapID(this->scene->getCountryID(),
				                                                this->scene->getCountryDefBackToMapID());	
		}
		else if (this->scene->getRealMapID()==134 && this->scene->getEmperorDare())
		{
			Cmd::stEnterEmperorDareZone send;
			send.state = 1;
			send.dwDefCountryID = this->scene->getEmperorDareDef();

			if (this->charbase.country == this->scene->getEmperorDareDef())
			{
				this->deathBackToMapID = SceneManager::getInstance().buildMapID(this->scene->getCountryID(),
				                                                134);	
				this->charbase.gomaptype = ZoneTypeDef::ZONE_EMPEROR_DEF_RELIVE;
			}
			else
			{
				this->deathBackToMapID = SceneManager::getInstance().buildMapID(this->scene->getCountryID(),
				                                                134);	
				this->charbase.gomaptype = ZoneTypeDef::ZONE_EMPEROR_ATT_RELIVE;
			}
			
			this->sendCmdToMe(&send, sizeof(send));
		}
		/*
		else
		{
			struct tm tm_1;
			time_t timValue = time(NULL);
			tm_1=*localtime(&timValue);
			if((tm_1.tm_hour%2) && (tm_1.tm_min >= 55))
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "古墓地图将在%d分钟后开放",60 - tm_1.tm_min);
			}
			else if(((tm_1.tm_hour%2) == 0) && tm_1.tm_min <= 30)
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "古墓地图已经开放%d分钟",tm_1.tm_min);
			}
		}
		// */
		if (!scene->isTrainingMap() && 0!=charbase.trainTime)
		{
			charbase.trainTime = 0;
			showCurrentEffect(Cmd::USTATE_DAOJISHI, false); // 更新客户端状态
			sendtoSelectedTrainState();
		}

		sendtoSelectedVIPState();
		
		//sky 清除VIP特效
		if(charbase.charvip <=2690) //soke VIP20清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP20, false);
		}
		else if(charbase.charvip <=2190) //soke VIP15清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP19, false);
		}
		else if(charbase.charvip <=1690) //soke VIP15清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP18, false);
		}
		else if(charbase.charvip <=1190) //soke VIP15清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP17, false);
		}
		else if(charbase.charvip <=690) //soke VIP15清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP16, false);
		}
		else if(charbase.charvip <=190) //soke VIP15清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP15, false);
		}
		else if(charbase.charvip <=170) //soke VIP14清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP14, false);
		}
		else if(charbase.charvip <=150) //soke VIP13清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP13, false);
		}
		else if(charbase.charvip <=130) //soke VIP12清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP12, false);
		}
		else if(charbase.charvip <=110) //soke VIP11清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP11, false);
		}
		else if(charbase.charvip <=90) //soke VIP10清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP10, false);
		}
		else if(charbase.charvip <=80) //soke VIP9清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP9, false);
		}
		else if(charbase.charvip <=70) //soke VIP8清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP8, false);
		}
		else if(charbase.charvip <=60) //soke VIP7清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP7, false);
		}
		else if(charbase.charvip <=50) //soke VIP6清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP6, false);
		}
		else if(charbase.charvip <=40) //soke VIP5清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP5, false);
		}
		else if(charbase.charvip <=30) //soke VIP4清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP4, false);
		}
		else if(charbase.charvip <=20) //soke VIP3清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP3, false);
		}
		else if(charbase.charvip <= 10) //soke VIP2清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP2, false);
		}
		else if(charbase.charvip <= 0) //soke VIP1清除玩家特效
		{
			showCurrentEffect(Cmd::USTATE_VIP1, false);
		}
		
		//soke 关闭GM特效
		if (charbase.id !=1)
		{
			showCurrentEffect(Cmd::USTATE_PLANTING, false); // 更新客户端状态
		}

		if (charbase.zhanchetime1 <= 0)
		{
			charbase.zhanchetime1 = 0;
			showCurrentEffect(Cmd::USTATE_USER_ZC_TYPE1, false); // 更新客户端状态
			sendtoZhancheSelectedOfficialtate1();
		}
		if (charbase.zhanchetime2 <= 0)
		{
			charbase.zhanchetime2 = 0;
			showCurrentEffect(Cmd::USTATE_USER_ZC_TYPE2, false); // 更新客户端状态
			sendtoZhancheSelectedOfficialtate2();
		}
		if (charbase.zhanchetime3 <= 0)
		{
			charbase.zhanchetime3 = 0;
			showCurrentEffect(Cmd::USTATE_USER_ZC_TYPE3, false); // 更新客户端状态
			sendtoZhancheSelectedOfficialtate3();
		}

        	//soke 关闭9朵花特效
		if (charbase.flower1time <= 0)
		{
			charbase.flower1time = 0;
			showCurrentEffect(Cmd::USTATE_FLOWER_9, false); // 更新客户端状态
			sendtoSelected9State();
		}
		//soke 关闭99朵花特效
		if (charbase.flower2time <= 0)
		{
			charbase.flower2time = 0;
			showCurrentEffect(Cmd::USTATE_FLOWER_99, false); // 更新客户端状态
			sendtoSelected99State();
		}
		//soke 关闭999朵花特效
		if (charbase.flower3time <= 0)
		{
			charbase.flower3time = 0;
			showCurrentEffect(Cmd::USTATE_FLOWER_999, false); // 更新客户端状态
			showCurrentEffect(Cmd::USTATE_USER_MEILI, false); // 更新客户端状态
			sendtoSelected999State();
			sendtoSelectedyswcState();
		}

		//soke 关闭9朵蓝色妖姬特效
		if (charbase.flower4time <= 0)
		{
			charbase.flower4time = 0;
			showCurrentEffect(Cmd::USTATE_FLOWERL_9, false); // 更新客户端状态
			sendtoSelectedx9State();
		}
		//soke 关闭99朵蓝色妖姬特效
		if (charbase.flower5time <= 0)
		{
			charbase.flower5time = 0;
			showCurrentEffect(Cmd::USTATE_FLOWERL_99, false); // 更新客户端状态
			sendtoSelectedx99State();
		}
		//soke 关闭999朵蓝色妖姬特效
		if (charbase.flower6time <= 0)
		{
			charbase.flower6time = 0;
			showCurrentEffect(Cmd::USTATE_FLOWERL_999, false); // 更新客户端状态
			showCurrentEffect(Cmd::USTATE_USER_RENQI, false); // 更新客户端状态
			sendtoSelectedx999State();
			sendtoSelectedyswc2State();
		}




		// s

		//通知九屏添加用户
		sendMeToNine();
		//得到九屏东西发给自己
		sendNineToMe();

		Cmd::stRTMagicPosUserCmd send;
		this->full_stRTMagicPosUserCmd(send);
		sendCmdToMe(&send,sizeof(send));

		//soke 同时处理下经验倍率时间的事情
		if( this->charbase.doubleatime > 0)
		{
			if( this->issetUState(Cmd::USTATE_EXP_125))
			{
				Zebra::logger->debug("已经存在特殊经验1.25倍状态不需要刷新啦");
			}
			else
			{
				char Buf[200]; 
				bzero(Buf,sizeof(Buf));
				Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
				constructInPlace(srs);
				srs->byType = Cmd::MAPDATATYPE_USER;
				srs->dwTempID = this->tempid;
				srs->size=1;

				if(charbase.bitmask & CHARBASE_EXP125) 
				{
					Zebra::logger->debug("检测到上次遗留的1.25倍经验时间和状态，重新设置1.25倍经验");
					showCurrentEffect(Cmd::USTATE_EXP_125, true);
					srs->states[0].state = Cmd::USTATE_EXP_125;
				}

				//通知客户端跟新状态的图标
				srs->states[0].result=charbase.doubleatime/60;
				srs->states[0].time=charbase.doubleatime/60;
				sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
			}
		}
		if( this->charbase.doublebtime > 0)
		{
			if( this->issetUState(Cmd::USTATE_EXP_150))
			{
				Zebra::logger->debug("已经存在特殊经验1.50倍状态不需要刷新啦");
			}
			else
			{
				char Buf[200]; 
				bzero(Buf,sizeof(Buf));
				Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
				constructInPlace(srs);
				srs->byType = Cmd::MAPDATATYPE_USER;
				srs->dwTempID = this->tempid;
				srs->size=1;

				if(charbase.bitmask & CHARBASE_EXP150) 
				{
					Zebra::logger->debug("检测到上次遗留的1.50倍经验时间和状态，重新设置1.50倍经验");
					showCurrentEffect(Cmd::USTATE_EXP_150, true);
					srs->states[0].state = Cmd::USTATE_EXP_150;
				}

				//通知客户端跟新状态的图标
				srs->states[0].result=charbase.doublebtime/60;
				srs->states[0].time=charbase.doublebtime/60;
				sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
			}
		}
		if( this->charbase.doublectime > 0)
		{
			if( this->issetUState(Cmd::USTATE_EXP_175) )
			{
				Zebra::logger->debug("已经存在特殊经验1.75倍状态不需要刷新啦");
			}
			else
			{
				char Buf[200]; 
				bzero(Buf,sizeof(Buf));
				Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
				constructInPlace(srs);
				srs->byType = Cmd::MAPDATATYPE_USER;
				srs->dwTempID = this->tempid;
				srs->size=1;

				if(charbase.bitmask & CHARBASE_EXP175) 
				{
					Zebra::logger->debug("检测到上次遗留的1.75倍经验时间和状态，重新设置1.75倍经验");
					showCurrentEffect(Cmd::USTATE_EXP_175, true);
					srs->states[0].state = Cmd::USTATE_EXP_175;
				}

				//通知客户端跟新状态的图标
				srs->states[0].result=charbase.doublectime/60;
				srs->states[0].time=charbase.doublectime/60;
				sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
			}
		}
		//soke 同时处理下副本时间的事情
		if( this->charbase.fubentime > 0)
		{
			if( this->issetUState(Cmd::USTATE_FUBEN))
			{
				//Zebra::logger->debug("已经存在副本状态不需要刷新啦");
			}
			else
			{
				char Buf[200]; 
				bzero(Buf,sizeof(Buf));
				Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
				constructInPlace(srs);
				srs->byType = Cmd::MAPDATATYPE_USER;
				srs->dwTempID = this->tempid;
				srs->size=1;

				if(charbase.bitmask & CHARBASE_FUBEN) 
				{
					Zebra::logger->debug("检测到上次遗留的副本时间和状态，重新设置副本状态");
					showCurrentEffect(Cmd::USTATE_FUBEN, true);
					srs->states[0].state = Cmd::USTATE_FUBEN;
				}

				//通知客户端跟新状态的图标
				srs->states[0].result=charbase.fubentime/60;
				srs->states[0].time=charbase.fubentime/60;
				sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
			}
		}
		//soke 同时处理下幸运丹时间的事情
		if( this->charbase.luckytime > 0)
		{
			if( this->issetUState(Cmd::USTATE_LUCKY_DAN))
			{
				//Zebra::logger->debug("已经存在幸运丹状态不需要刷新啦");
			}
			else
			{
				char Buf[200]; 
				bzero(Buf,sizeof(Buf));
				Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
				constructInPlace(srs);
				srs->byType = Cmd::MAPDATATYPE_USER;
				srs->dwTempID = this->tempid;
				srs->size=1;

				if(charbase.bitmask & CHARBASE_LUNCKY) 
				{
					Zebra::logger->debug("检测到上次遗留的幸运丹时间和状态，重新设置幸运丹状态");
					showCurrentEffect(Cmd::USTATE_LUCKY_DAN, true);
					srs->states[0].state = Cmd::USTATE_LUCKY_DAN;
				}

				//通知客户端跟新状态的图标
				srs->states[0].result=charbase.luckytime/60;
				srs->states[0].time=charbase.luckytime/60;
				sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
			}
		}

		//soke 上线后在来添加以前遗留的技能状态
		skillStatusM.processPassiveness();

		return true;
	}
	else
	{
		if (founded) newscene->clearBlock(findedPos);
		Zebra::logger->debug("将%s加入%s失败",name,newscene->name);
		return false;
	}
}

/**
 * \brief 使角色离开一个场景
 *
 *
 */
bool SceneUser::LeaveScene()
{
    if(scene) scene->removeUser(this);
	Cmd::stRemoveUserMapScreenUserCmd remove;
	remove.dwUserTempID=tempid;
	//检查是否隐身
	//if (SceneEntry_Hide!=getState())
	scene->sendCmdToNine(getPosI(),&remove,sizeof(remove));
	//由于客户端需要靠这个指令来关闭对话框,所以必须发给自己一份 
	sendCmdToMe(&remove,sizeof(remove));
	scene->subUserCount();
	//Zebra::logger->debug("场景%s目前在线人数%u",scene->name,scene->countUser());
	if(scene->countUser() ==0 && (scene->getRunState() == SCENE_RUN_STATE_UNLOAD))
	{
		scene->setRunState(SCENE_RUN_STATE_REMOVE);
		Cmd::Session::t_removeScene_SceneSession rem;
		rem.map_id = scene->id;
		sessionClient->sendCmd(&rem , sizeof(rem));
	}

	return true;
}

/**
 * \brief 角色中了恐惧效果
 */
void SceneUser::dreadProcess()
{
	if (dread)
	{
		Cmd::stUserMoveMoveUserCmd send;
		int count=10;
		int curDir=0;

		do {
			send.dwUserTempID = this->tempid;
			if (count == 10)
				curDir = getDir()+zMisc::randBetween(-1,1);
			else
				curDir++;
			if (curDir <0) curDir = 7;
			send.byDirect = curDir%8;
			send.bySpeed = 2;
			send.x = pos.x;
			send.y = pos.y;
			switch (send.byDirect)
			{
				case 0:
					{
						if (send.y-1 >=0) send.y--;
					}
					break;
				case 1:
					{
						if (send.y-1 >=0) send.y--;
						send.x++;
					}
					break;
				case 2:
					{
						send.x++;
					}
					break;
				case 3:
					{
						send.x++;
						send.y++;
					}
					break;
				case 4:
					{
						send.y++;
					}
					break;
				case 5:
					{
						send.y++;
						if (send.x-1 >=0) send.x--;
					}
					break;
				case 6:
					{
						if (send.x-1 >=0) send.x--;
					}
					break;
				case 7:
					{
						if (send.x-1 >=0) send.x--;
						if (send.y-1 >=0) send.y--;
					}
					break;
				default:
					break;
			}
			count--;
		} while(!move(&send)&&count>0);
	}
}

//soke 幸运丹次数
void SceneUser::initLuckyCount()
{
	time_t timValue = time(NULL);
	int value = timValue - this->charbase.offlinetime;
	struct tm tv1;
	zRTime::getLocalTime(tv1, timValue);
	time_t tempvalue = (time_t)this->charbase.offlinetime;
	struct tm tv2;
	zRTime::getLocalTime(tv2, tempvalue);


	if ((value > 24*60*60) || ((tv1.tm_mday!=tv2.tm_mday)))
	{
		this->charbase.luckyCount = 0;
	}
}

//soke 每日登陆领取次数
void SceneUser::initLoginCount()
{
	time_t timValue = time(NULL);
	int value = timValue - this->charbase.offlinetime;
	struct tm tv1;
	zRTime::getLocalTime(tv1, timValue);
	time_t tempvalue = (time_t)this->charbase.offlinetime;
	struct tm tv2;
	zRTime::getLocalTime(tv2, tempvalue);


	if ((value > 24*60*60) || ((tv1.tm_mday!=tv2.tm_mday)))
	{
		this->charbase.loginCount = 0;
	}
}


//soke 每日svip领取次数
void SceneUser::initSvipCount()
{
	time_t timValue = time(NULL);
	int value = timValue - this->charbase.offlinetime;
	struct tm tv1;
	zRTime::getLocalTime(tv1, timValue);
	time_t tempvalue = (time_t)this->charbase.offlinetime;
	struct tm tv2;
	zRTime::getLocalTime(tv2, tempvalue);


	if ((value > 24*60*60) || ((tv1.tm_mday!=tv2.tm_mday)))
	{
		this->charbase.svipCount = 0;
	}
}

//soke 每日太庙完成次数
void SceneUser::initTaimCount()
{
	time_t timValue = time(NULL);
	int value = timValue - this->charbase.offlinetime;
	struct tm tv1;
	zRTime::getLocalTime(tv1, timValue);
	time_t tempvalue = (time_t)this->charbase.offlinetime;
	struct tm tv2;
	zRTime::getLocalTime(tv2, tempvalue);


	if ((value > 24*60*60) || ((tv1.tm_mday!=tv2.tm_mday)))
	{
		this->charbase.taimCount = 0;
	}
}

//soke 每日副本激活次数
void SceneUser::initFubenCount()
{
	time_t timValue = time(NULL);
	int value = timValue - this->charbase.offlinetime;
	struct tm tv1;
	zRTime::getLocalTime(tv1, timValue);
	time_t tempvalue = (time_t)this->charbase.offlinetime;
	struct tm tv2;
	zRTime::getLocalTime(tv2, tempvalue);


	if ((value > 24*60*60) || ((tv1.tm_mday!=tv2.tm_mday)))
	{
		this->charbase.fbCount = 0;
		this->charbase.fba1 = 3;
		this->charbase.fba1 = 3;
		this->charbase.fba1 = 3;
		this->charbase.fba1 = 3;
		this->charbase.fba1 = 3;
		this->charbase.vipfb = 0;
	}
}


//soke 每日自行车完成次数
void SceneUser::initBikeCount()
{
	time_t timValue = time(NULL);
	int value = timValue - this->charbase.offlinetime;
	struct tm tv1;
	zRTime::getLocalTime(tv1, timValue);
	time_t tempvalue = (time_t)this->charbase.offlinetime;
	struct tm tv2;
	zRTime::getLocalTime(tv2, tempvalue);


	if ((value > 24*60*60) || ((tv1.tm_mday!=tv2.tm_mday)))
	{
		this->charbase.bikecs = 0;
		this->charbase.bikenum = 0;
		this->charbase.bikestate = 0;
		this->charbase.migongcs = 0;
	}
}

void SceneUser::initAnswerCount()
{
	time_t timValue = time(NULL);
	int value = timValue - this->charbase.offlinetime;
	struct tm tv1;
	zRTime::getLocalTime(tv1, timValue);
	time_t tempvalue = (time_t)this->charbase.offlinetime;
	struct tm tv2;
	zRTime::getLocalTime(tv2, tempvalue);


	if ((value > 24*60*60) || ((tv1.tm_mday!=tv2.tm_mday)))
	{
		this->charbase.answerCount = 1;
	}
}
void SceneUser::initTire()
{
	time_t timValue = time(NULL);
	int value = timValue - this->charbase.offlinetime;
	struct tm tv1;
	zRTime::getLocalTime(tv1, timValue);
	time_t tempvalue = (time_t)this->charbase.offlinetime;
	struct tm tv2;
	zRTime::getLocalTime(tv2, tempvalue);


	if ((value > 24*60*60) || ((tv1.tm_mday!=tv2.tm_mday)))
	{
		bzero(this->charbase.tiretime, 36);
		this->wdTire = 0;
		this->wdTirePer = 100;
	}
	else
	{
		int count=0;
		for(int k=0; k<288;k++)
		{
			if (Cmd::isset_state(((unsigned char *)(this->charbase.tiretime)), k))
			{
				count++;
			}
		}
		if (count > 228)
		{
			for(time_t i=this->charbase.offlinetime; i<timValue; i+=60)
			{
				struct tm tmValue;
				zRTime::getLocalTime(tmValue, i);
				int j = (tmValue.tm_hour*60+tmValue.tm_min)/5;
				Cmd::set_state(((unsigned char *)(this->charbase.tiretime)),j);
			}
		}
		else
		{
			for(time_t i=this->charbase.offlinetime; i<timValue; i+=60)
			{
				struct tm tmValue;
				zRTime::getLocalTime(tmValue, i);
				int j = (tmValue.tm_hour*60+tmValue.tm_min)/5;
				Cmd::clear_state(((unsigned char *)(this->charbase.tiretime)),j);
			}
		}
		count=0;
		for(int k=0; k<288;k++)
		{
			if (Cmd::isset_state(((unsigned char *)(this->charbase.tiretime)), k))
			{
				count++;
			}
		}
		//Zebra::logger->debug("角色%s上线疲劳值为[%u]", this->name, count);
		this->wdTire=count;

		if (this->wdTire>=287)
		{
			bzero(this->charbase.tiretime, 36);
			this->wdTire = 0;
			this->wdTirePer = 100;
		}

		if (this->wdTire > 144) //12
		{
			if (this->wdTire > 156) //13
			{
				if (this->wdTire > 168) //14
				{
					if (this->wdTire > 180) //15
					{
						if (this->wdTire > 192) //16
						{
							if (this->wdTire > 204) //17
							{
								this->wdTirePer =0;
							}
							else
							{
								this->wdTirePer =20;
							}
						}
						else
						{
							this->wdTirePer =40;
						}
					}
					else
					{
						this->wdTirePer =60;
					}
				}
				else
				{
					this->wdTirePer =80;
				}
			}
			else
			{
				this->wdTirePer =90;
			}
		}
		else
		{
			this->wdTirePer =100;
		}
	}
	//Cmd::stMainUserDataUserCmd  userinfo;
	//full_t_MainUserData(userinfo.data);
	//sendCmdToMe(&userinfo,sizeof(userinfo));
}


/**             
 * \brief 处理幸运丹次数
 * 
 */     
void SceneUser::processLuckyCount()
{               
	time_t timValue = time(NULL);
	struct tm tmValue;
	zRTime::getLocalTime(tmValue, timValue);

	if (tmValue.tm_hour==0 && (tmValue.tm_min==0 || tmValue.tm_min == 1))
	{       
		this->charbase.luckyCount = 0;
	}       
}  

/**             
 * \brief 处理每日登陆领取次数
 * 
 */     
void SceneUser::processLoginCount()
{               
	time_t timValue = time(NULL);
	struct tm tmValue;
	zRTime::getLocalTime(tmValue, timValue);

	if (tmValue.tm_hour==0 && (tmValue.tm_min==0 || tmValue.tm_min == 1))
	{       
		this->charbase.loginCount = 0;
	}       
}  

/**             
 * \brief 处理每日svip领取次数
 * 
 */     
void SceneUser::processSvipCount()
{               
	time_t timValue = time(NULL);
	struct tm tmValue;
	zRTime::getLocalTime(tmValue, timValue);

	if (tmValue.tm_hour==0 && (tmValue.tm_min==0 || tmValue.tm_min == 1))
	{       
		this->charbase.svipCount = 0;
	}       
}  

/**             
 * \brief 处理每日太庙完成次数
 * 
 */     
void SceneUser::processTaimCount()
{               
	time_t timValue = time(NULL);
	struct tm tmValue;
	zRTime::getLocalTime(tmValue, timValue);

	if (tmValue.tm_hour==0 && (tmValue.tm_min==0 || tmValue.tm_min == 1))
	{       
		this->charbase.taimCount = 0;
	}       
}  

/**             
 * \brief  处理每日副本激活次数
 * 
 */     
void SceneUser::processFubenCount()
{               
	time_t timValue = time(NULL);
	struct tm tmValue;
	zRTime::getLocalTime(tmValue, timValue);

	if (tmValue.tm_hour==0 && (tmValue.tm_min==0 || tmValue.tm_min == 1))
	{       
		this->charbase.fbCount = 0;
		this->charbase.fba1 = 3;
		this->charbase.fba1 = 3;
		this->charbase.fba1 = 3;
		this->charbase.fba1 = 3;
		this->charbase.fba1 = 3;
		this->charbase.vipfb = 0;
	}       
}  



/**             
 * \brief  处理每日自行车激活次数
 * 
 */     
void SceneUser::processBikeCount()
{               
	time_t timValue = time(NULL);
	struct tm tmValue;
	zRTime::getLocalTime(tmValue, timValue);

	if (tmValue.tm_hour==0 && (tmValue.tm_min==0 || tmValue.tm_min == 1))
	{       
		this->charbase.bikecs = 0;
		this->charbase.bikenum = 0;
		this->charbase.bikestate = 0;
		this->charbase.migongcs = 0;
	}       
}  


/**             
 * \brief 处理答题次数
 * 
 */     
void SceneUser::processAnswerCount()
{               
	time_t timValue = time(NULL);
	struct tm tmValue;
	zRTime::getLocalTime(tmValue, timValue);

	if (tmValue.tm_hour==0 && (tmValue.tm_min==0 || tmValue.tm_min == 1))
	{       
		this->charbase.answerCount = 1;
	}       
} 

/**             
 * \brief 处理称号判定变量
 * 
 */
inline void SceneUser::processChshowCount()
{
	time_t timValue = time(NULL);
	struct tm tmValue;
	zRTime::getLocalTime(tmValue, timValue);

	if (tmValue.tm_hour==0 && (tmValue.tm_min==0 || tmValue.tm_min == 1))
	{       
		this->charbase.xhbbcount = 0;
		this->charbase.herocount = 0;
		this->charbase.zhanlicount = 0;
	}
	       
}

/**
 * \brief 处理疲劳功能计算
 * 五分钟采样 8*60/5=96 24*60/5=288
 */
void SceneUser::callProcessTire()
{
	this->processTire();
}

/**
 * \brief 处理疲劳功能计算
 * 五分钟采样 8*60/5=96 24*60/5=288
 */
void SceneUser::processTire()
{
	bool bProcess;
	bProcess = true;
	time_t timValue = time(NULL);
	struct tm tmValue;
	WORD bchange = this->wdTirePer;
	zRTime::getLocalTime(tmValue, timValue);
	if ((tmValue.tm_hour==0 && ((tmValue.tm_min >=0) && (tmValue.tm_min < 5)))||(this->wdTire >= 287))
	{
		bzero(this->charbase.tiretime, 36);
		this->wdTire = 0;
		this->wdTirePer =100;
	}
	if (this->scene &&(this->charbase.country==this->scene->getCountryID()))
	{
		DWORD dwMapID = this->scene->getRealMapID();
		if ((dwMapID == 102) || (dwMapID == 139))
		{
			if (this->scene->getZoneType(this->pos)>ZoneTypeDef::ZONE_NONE)
			{
				bProcess = false;
			}
		}
	}
	else
	{
		bProcess = false; //在国外也不涨疲劳度
	}
	if (this->wdTire > 228 || (bProcess && this->wdTire <= 228))
	{
		int value = (tmValue.tm_hour*60+tmValue.tm_min)/5;
		if (!Cmd::isset_state(((unsigned char *)(this->charbase.tiretime)),value)) 	this->wdTire++;
		Cmd::set_state(((unsigned char *)(this->charbase.tiretime)),value);
	}

#ifdef _DEBUGLOG
	Zebra::logger->debug("-------------------------------------------------------------------------");
	Zebra::logger->debug("[%u][%u][%u][%u][%u][%u][%u][%u][%u][%u]",(unsigned char)this->charbase.tiretime[0],
			(unsigned char)this->charbase.tiretime[1],
			(unsigned char)this->charbase.tiretime[2],
			(unsigned char)this->charbase.tiretime[3],
			(unsigned char)this->charbase.tiretime[4],
			(unsigned char)this->charbase.tiretime[5],
			(unsigned char)this->charbase.tiretime[6],
			(unsigned char)this->charbase.tiretime[7],
			(unsigned char)this->charbase.tiretime[8],
			(unsigned char)this->charbase.tiretime[9]);
#endif
#ifdef _DEBUGLOG
	int value = (tmValue.tm_hour*60+tmValue.tm_min)/5;
	Zebra::logger->debug("当前疲劳记数[%u] 小时[%u] 分钟[%u] 当前时间值value[%u]", this->wdTire, tmValue.tm_hour, tmValue.tm_min, value);
#endif

	if (this->wdTire > 144) //12
	{
		if (this->wdTire > 156) //13
		{
			if (this->wdTire > 168) //14
			{
				if (this->wdTire > 180) //15
				{
					if (this->wdTire > 192) //16
					{
						if (this->wdTire > 204) //17
						{
							this->wdTirePer =0;
						}
						else
						{
							this->wdTirePer =20;
						}
					}
					else
					{
						this->wdTirePer =40;
					}
				}
				else
				{
					this->wdTirePer =60;
				}
			}
			else
			{
				this->wdTirePer =80;
			}
		}
		else
		{
			this->wdTirePer =90;
		}
	}
	else
	{
		this->wdTirePer =100;
	}

	if (bchange != this->wdTirePer)
	{
		Cmd::stMainUserDataUserCmd  userinfo;
		full_t_MainUserData(userinfo.data);
		sendCmdToMe(&userinfo,sizeof(userinfo));
	}
	this->charbase.offlinetime = timValue;
}

/**
 * \brief 定时更新角色的属性,并保存到档案服务器
 *
 * \return 始终返回TRUE
 */
bool SceneUser::refreshMe()
{
	My_FunctionTime_wrapper(__PRETTY_FUNCTION__);
	if (mplock)
	{
		charbase.mp = charstate.maxmp;
	}
	if (hplock)
	{
		charbase.hp = charstate.maxhp;
	}
	//if (splock)
	//{
	//	charbase.sp = charstate.maxsp;
	//}
	if (checkMessageTime(SceneTimeTick::currentTime))
		MessageSystem::getInstance().check(this);

	//检查善恶度
	if(checkGoodnessTime(SceneTimeTick::currentTime))
		checkGoodness();

	if (_five_min(SceneTimeTick::currentTime))
	{
		Cmd::Session::t_countryNotify_SceneSession send_gem;
		if (this->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER))
		{
			snprintf(send_gem.info, sizeof(send_gem.info), 
					"虎魄附体持有者正在向 %s %d,%d 方向逃窜", this->scene->getName(), 
					getPos().x, getPos().y);

			send_gem.dwCountryID = this->charbase.country;
			sessionClient->sendCmd(&send_gem, sizeof(send_gem));
		}

		if (this->issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
		{
			snprintf(send_gem.info, sizeof(send_gem.info), 
					"龙精附体持有者正在向 %s %d,%d 方向逃窜", this->scene->getName(), 
					getPos().x, getPos().y);
			send_gem.dwCountryID = this->charbase.country;
			sessionClient->sendCmd(&send_gem, sizeof(send_gem));
		}

		if(xcartoon)
		{
			if (xcartoon->id==9013) //皇帝
			{
				int index = zMisc::randBetween(0, 4);
				switch(index)
				{
					case 0:
						Channel::sendNine(xcartoon, "贺喜皇上，武功又有提升，千秋万载，一统十国。");
						break;
					case 1:
						Channel::sendNine(xcartoon, "终会有一天，我也将拥有自己的影子。");
						break;
					case 2:
						Channel::sendNine(xcartoon, "我们国王说了，是美女的都过来，绝对吃香的喝辣的");
						break;
					case 3:
						Channel::sendNine(xcartoon, "关键不是背着的大剑沉重，而是它能亲吻鲜血的机会。");
						break;
					case 4:
						Channel::sendNine(xcartoon, "如果不想触及国王的威严，请收起你的小刀。");
						break;
					default:
					    break;
				}
			}
			else if (xcartoon->id==9014) //国王
			{
				int index = zMisc::randBetween(0, 6);
				switch(index)
				{
					case 0:
						Channel::sendNine(xcartoon, "让我们以行动来展示对陛下的忠诚。");
						break;
					case 1:
						Channel::sendNine(xcartoon, "我们将和入侵者战斗到最后一秒。");
						break;
					case 2:
						Channel::sendNine(xcartoon, "很少有跟班的有我这么拉风。");
						break;
					case 3:
						Channel::sendNine(xcartoon, "兄弟～你拿根黄瓜在我身上乱砍什么啊？");
						break;
					case 4:
						Channel::sendNine(xcartoon, "低调，做人应该如此。");
						break;
					case 5:
						Channel::sendNine(xcartoon, "毛手毛脚的，你手往哪摸啊 别着急走，我先补个妆。");
						break;
					case 6:
						Channel::sendNine(xcartoon, "有人说我比蝎子还毒，你相信吗？");
						break;
					default:
						break;
				}
			}
			else if (xcartoon->id==9015) //城主
			{
				int index = zMisc::randBetween(0, 4);
				switch(index)
				{
					case 0:
						Channel::sendNine(xcartoon, "恭喜城主，或者我该回去做一枚真正的绣花针。");
						break;
					case 1:
						Channel::sendNine(xcartoon, "恭喜城主功力更进一层。");
						break;
					case 2:
						Channel::sendNine(xcartoon, "美貌和实力成反比，而我是个例外。");
						break;
					case 3:
						Channel::sendNine(xcartoon, "在别人眼里我只是个丫鬟，其实。。。");
						break;
					case 4:
						Channel::sendNine(xcartoon, "终有一天，我将成为城主夫人");
						break;
					default:
						break;
				}
		    }
		}
	}

	if (dread&&_half_sec(SceneTimeTick::currentTime)) dreadProcess();
	if (_one_sec(SceneTimeTick::currentTime))
	{
		Refreshleaderboard(); //定时刷新榜单称号处理
		//Zebra::logger->debug("一秒定时器:%u",this->name);
		//execute_script_event(this,"one_sec");
		if (npcdareflag) checkNpcDareState();
		//上次pk时间
		if(lastPkTime)
		{
			lastPkTime --;
		}
		if(backOffing)
		{
			if(backOffing- 1000 > 0)
			{
				//backOff(getDir() , 3);
				backOffing -= 1000;
			}
			else
			{
				backOffing = 0;
			}
		}

		//轰天技能冷却 
		if(hongtian_time1>0)
		{
			hongtian_time1--;
		}

		//战车处理 
		if(!this->isZC())//如果在自行车比赛状态
		{
			if(this->issetUState(Cmd::USTATE_USER_ZCReday))//战车准备状态
			{
				if(jzzc >0)
				{
					jzzc--;
				}
				else{
					jzzc = 2;
					showCurrentEffect(Cmd::USTATE_USER_ZC, true ); 
					showCurrentEffect(Cmd::USTATE_USER_ZCReday, false ); 
					if (((SceneUser *)this)->horse.mount())
					{
						((SceneUser *)this)->horse.mount(false);
					}
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"战车召唤成功");	
				}
			}

		}
		if(ScenesService::getInstance().fanwaigua == true)
		{
			//反外挂处理 zm
			if(this->yichangtime==0  )
			{
				if(this->yichangnum>6)//这里判定3秒内 异常攻击次数为几次 不用改
				{
					if(this->fanwaiguarongcuo < ScenesService::getInstance().fanwaiguarongcuo)
					{
						this->fanwaiguarongcuo = ScenesService::getInstance().fanwaiguarongcuo;
					}
					else{
						if(this->jinggaonum<2) //这里提示次数 2为3次
						{
							Channel::sendSys(this, Cmd::INFO_TYPE_MSG,"检测到外挂载入，警告一次，继续使用将封号处理");
							Gm::gomap(this, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
						}
						else{
							//SceneUserManager::getMe().lock();
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"使用外挂，你已经被踢出服务器！");

							OnQuit event(1);
							EventTable::instance().execute(*this, event);

							this->save(Cmd::Record::LOGOUT_WRITEBACK);
							this->killAllPets();

							//通知session服务器
							Cmd::Session::t_unregUser_SceneSession ret;
							ret.dwUserID=this->id;
							ret.dwSceneTempID=this->scene->tempid;
							ret.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
							sessionClient->sendCmd(&ret,sizeof(ret));

							//通知网关服务器
							Cmd::Scene::t_Unreg_LoginScene retgate;
							retgate.dwUserID = this->id;
							retgate.dwSceneTempID = this->scene->tempid;
							retgate.retcode = Cmd::Scene::UNREGUSER_RET_ERROR;
							this->gatetask->sendCmd(&retgate,sizeof(retgate));

							this->unreg();
						}
						this->jinggaonum++;
					}	
				}
				this->yichangnum = 0;
				this->yichangtime = 3;//三秒判定
			}
			else{
				this->yichangtime--;
			}
		}
		
		 //处理BOSS归属称号关闭
		 if(BossGsStateTime > 0)
		 {
			 BossGsStateTime--;
			 if(BossGsStateTime == 0)
			 {
				 if (this->issetUState(Cmd::USTATE_BOSSGUISHU))
				 {
					 this->showCurrentEffect(Cmd::USTATE_BOSSGUISHU,false);	
				 }
			 }
		 }
		//自行车比赛状态处理 
		if(this->isBike())//如果在自行车比赛状态
		{
			if(this->issetUState(Cmd::USTATE_USER_BIKESKILL1))//找不到北
			{
				if(bikeskill1 >0)
				{
					bikeskill1--;
				}
				else{
					bikeskill1 = 5;
					showCurrentEffect(Cmd::USTATE_USER_BIKESKILL1, false ); 
				}
			}

			if(this->issetUState(Cmd::USTATE_USER_BIKESKILL3))//失明
			{
				if(bikeskill3 >0)
				{
					blind = true;
					bikeskill3--;
					char text[256];
					sprintf(text,"【自行车】进入失明状态，还剩%d秒",bikeskill3);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);	
					
					showCurrentEffect(Cmd::USTATE_BLIND, true ); 
					blind = true;
				}
				else{
					bikeskill3 = 5;
					blind = false;
					showCurrentEffect(Cmd::USTATE_USER_BIKESKILL3, false ); 
					showCurrentEffect(Cmd::USTATE_BLIND, false ); 
				}
			}
			if(this->issetUState(Cmd::USTATE_USER_BIKESKILL4))//混乱
			{

				if(bikeskill4 >0)
				{
					bikeskill4--;

					char text[256];
					sprintf(text,"【自行车】进入混乱状态，还剩%d秒",bikeskill4);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);	
					
					showCurrentEffect(Cmd::USTATE_CONFUSION, true ); 
					dread = true;
				}
				else{
					bikeskill4 = 3;
					dread = false;
					showCurrentEffect(Cmd::USTATE_USER_BIKESKILL4, false ); 
					showCurrentEffect(Cmd::USTATE_CONFUSION, false ); 
				}
			}
			// if(this->issetUState(Cmd::USTATE_USER_BIKESKILL5))//加速
			// {
			// 	if(bikeskill5 >0)
			// 	{
			// 		bikeskill5--;
			// 		char text[256];
			// 		sprintf(text,"【自行车】进入加速状态，还剩%d秒",bikeskill5);
			// 		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
			// 		Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);	
			// 	}
			// 	else{
			// 		bikeskill5 = 10;
			// 		this->setupCharBase();
			// 		//刷新用户数据
			// 		Cmd::stMainUserDataUserCmd  userinfo;
			// 		this->full_t_MainUserData(userinfo.data);
			// 		this->sendCmdToMe(&userinfo,sizeof(userinfo));
			// 		showCurrentEffect(Cmd::USTATE_USER_BIKESKILL5, false ); 
			// 	}
			// }
			if(this->issetUState(Cmd::USTATE_USER_BIKESKILL6))//定身
			{	
				if(bikeskill6 >0)
				{
					bikeskill6--;
					moveAction = false;
					char text[256];
					sprintf(text,"【自行车】进入定身状态，还剩%d秒",bikeskill6);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);	
				}
				else{
					bikeskill6 = 3;
					moveAction = true;
					showCurrentEffect(Cmd::USTATE_USER_BIKESKILL6, false ); 
				}
			}
		}


		//天下第一自动复活
		if(this->getState() == zSceneEntry::SceneEntry_Death && (scene->getRealMapID()>=10001 && scene->getRealMapID()<=10401))
		{
			this->relive(Cmd::ReliveMaxey , 0 , 100);
		}

		if(this->getState() == zSceneEntry::SceneEntry_Death)
		{

			//如果处在死亡等待状态
			if(deathWaitTime > 0)
			{
				deathWaitTime --;
			}

			if(deathWaitTime == 1)
			{
				deathWaitTime = 0;
				this->relive(Cmd::ReliveHome, 0, 70);
				//this->reliveReady(NULL , true);
			}
		}

		// TODO 复活虚拟状态不进行自动恢复
		if (this->charbase.reliveWeakTime > 0)
		{
			if (this->charbase.reliveWeakTime <= SceneTimeTick::currentTime.sec()%10000)
			{// 调用预处理，重新计算五项属性
				this->charbase.reliveWeakTime = 0;
				this->setupCharBase();

				showCurrentEffect(Cmd::USTATE_RELIVEWEAK, false); // 更新客户端状态
				Cmd::stMainUserDataUserCmd  userinfo;
				full_t_MainUserData(userinfo.data);
				sendCmdToMe(&userinfo,sizeof(userinfo));
				//this->save(Cmd::Record::OPERATION_WRITEBACK);
			}
		}

		restitute();

		skillStatusM.timer();

		if (miniGame) miniGame->timer(SceneTimeTick::currentTime.sec(), this);


		//官员侍卫系统 注释
		if (this->king && !this->emperor)//国王
		{
			if ( !xcartoon )
			{
				summonPet(9014, Cmd::PET_TYPE_XCARTOON);
			}
		}
		else if (this->emperor)//皇帝
		{
			if ( !xcartoon )
			{
				summonPet(9013, Cmd::PET_TYPE_XCARTOON);
			}
		}
		else if(strstr(this->caption , "城主") != NULL)
		{
			if ( !xcartoon )
			{
				summonPet(9015, Cmd::PET_TYPE_XCARTOON);
			}
		}
		else
		{
			if (xcartoon)
			{
				xcartoon->putAway(Cmd::Session::SAVE_TYPE_PUTAWAY);    
			}
		}

        // 桃子增加比武岛自动踢人
		// 将多秒钟定时器放到5秒钟里可以做一点优化^_^
		if (_five_sec(SceneTimeTick::currentTime)) 
		{
			struct tm tv1;
			time_t timValue = time(NULL);
			zRTime::getLocalTime(tv1, timValue);

			//sky 迷宫踢人
			if ((scene->getRealMapID()>= 223 && scene->getRealMapID()<= 247) && ((!(tv1.tm_hour >18 && tv1.tm_hour <24)) || this->charbase.migongcs == 1 )) //开始时间和结束时间推迟1小时
			{
				Scene * s = SceneManager::getInstance().getSceneByName("中立区·皇城");
				if (s)
				{
					bool suc = changeMap(s,zPos(828,708));
					if (!suc)
						Zebra::logger->error("%s 离开比武岛地图失败，目的 %s (%d,%d)", name, s->name, pos.x, pos.y);
					else
						Zebra::logger->error("%s 离开比武岛地图", name);
				}
				else
				{
					Cmd::Session::t_changeScene_SceneSession cmd;
					cmd.id = id;
					cmd.temp_id = tempid;
					cmd.x = 806;
					cmd.y = 716;
					cmd.map_id = 0;
					cmd.map_file[0] = '\0';
					strncpy((char *)cmd.map_name, "中立区·皇城", MAX_NAMESIZE);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "不在开放时间内或您已完成迷宫活动");
					sessionClient->sendCmd(&cmd, sizeof(cmd));
				}
			}

			//sky 比武岛踢人
			if ((scene->getRealMapID()>= 10500 && scene->getRealMapID()<= 10509) && (!(tv1.tm_hour >20 && tv1.tm_hour <24))) //开始时间和结束时间推迟1小时
			{
				Scene * s = SceneManager::getInstance().getSceneByName("中立区·皇城");
				if (s)
				{
					bool suc = changeMap(s,zPos(828,708));
					if (!suc)
						Zebra::logger->error("%s 离开比武岛地图失败，目的 %s (%d,%d)", name, s->name, pos.x, pos.y);
					else
						Zebra::logger->error("%s 离开比武岛地图", name);
				}
				else
				{
					Cmd::Session::t_changeScene_SceneSession cmd;
					cmd.id = id;
					cmd.temp_id = tempid;
					cmd.x = 806;
					cmd.y = 716;
					cmd.map_id = 0;
					cmd.map_file[0] = '\0';
					strncpy((char *)cmd.map_name, "中立区·皇城", MAX_NAMESIZE);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "不在开放时间内不可进入地图");
					sessionClient->sendCmd(&cmd, sizeof(cmd));
				}
			}

			//sky 比武岛踢人
			if ((scene->getRealMapID()== 10999) && (!(tv1.tm_hour >=19 && tv1.tm_hour <=23))) //开始时间和结束时间推迟1小时
			{
				Scene * s = SceneManager::getInstance().getSceneByName("中立区·皇城");
				if (s)
				{
					bool suc = changeMap(s,zPos(828,708));
					if (!suc)
						Zebra::logger->error("%s 离开比武岛地图失败，目的 %s (%d,%d)", name, s->name, pos.x, pos.y);
					else
						Zebra::logger->error("%s 离开比武岛地图", name);
				}
				else
				{
					Cmd::Session::t_changeScene_SceneSession cmd;
					cmd.id = id;
					cmd.temp_id = tempid;
					cmd.x = 806;
					cmd.y = 716;
					cmd.map_id = 0;
					cmd.map_file[0] = '\0';
					strncpy((char *)cmd.map_name, "中立区·皇城", MAX_NAMESIZE);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "不在开放时间内不可进入地图");
					sessionClient->sendCmd(&cmd, sizeof(cmd));
				}
			}
			// //sky 比武岛踢人
			// if (((scene->getRealMapID()>= 475 && scene->getRealMapID()<= 478)||(scene->getRealMapID()>= 251 && scene->getRealMapID()<= 260)||(scene->getRealMapID()>= 31001 && scene->getRealMapID()<= 31007)||(scene->getRealMapID()>= 151 && scene->getRealMapID()<= 152)||(scene->getRealMapID()>= 172 && scene->getRealMapID()<= 173)) && ((tv1.tm_hour >19 && tv1.tm_min>=30) && (tv1.tm_hour <22 && tv1.tm_min<=50) )) //开始时间和结束时间推迟1小时
			// {
			// 	Scene * s = SceneManager::getInstance().getSceneByName("中立区·皇城");
			// 	if (s)
			// 	{
			// 		bool suc = changeMap(s,zPos(828,708));
			// 		if (!suc)
			// 			Zebra::logger->error("%s 离开比武岛地图失败，目的 %s (%d,%d)", name, s->name, pos.x, pos.y);
			// 		else
			// 			Zebra::logger->error("%s 离开比武岛地图", name);
			// 	}
			// 	else
			// 	{
			// 		Cmd::Session::t_changeScene_SceneSession cmd;
			// 		cmd.id = id;
			// 		cmd.temp_id = tempid;
			// 		cmd.x = 806;
			// 		cmd.y = 716;
			// 		cmd.map_id = 0;
			// 		cmd.map_file[0] = '\0';
			// 		strncpy((char *)cmd.map_name, "中立区·皇城", MAX_NAMESIZE);
			// 		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "不在开放时间内不可进入地图");
			// 		sessionClient->sendCmd(&cmd, sizeof(cmd));
			// 	}
			// }
		/////////////////////////////////////////////////////////////////////////	
			_5_sec_count++;

			// 将1分钟定时器放到5秒钟里可以做一点优化^_^
			if(_one_min(SceneTimeTick::currentTime))
			{
	
				//soke 貌似外挂检查  先去掉
//				if ((atoi(Zebra::global["service_flag"].c_str())&Cmd::Session::SERVICE_PROCESS))
//				{
//						if (dropTime && dropTime<SceneTimeTick::currentTime.sec()//断线时间到了
//										&& !guard)//运镖时不踢
//						{
//								Cmd::stMapDataMapScreenUserCmd send;
//								send.mdih.size = zMisc::randBetween(1024, 60000);
//								send.mdih.type = Cmd::MAPDATATYPE_NPC;
//								sendCmdToMe(&send, sizeof(send));
//
//								Zebra::logger->trace("%s 向使用外挂的玩家发送非法信息", name);
//								dropTime = 0;
//						}
//
//						if (0==processCheckTime)
//						{
//								if (ScenesService::pStampData->dwChannelID)
//								{
//										sendCmdToMe(ScenesService::pStampData, sizeof(Cmd::stChannelChatUserCmd)+ScenesService::pStampData->dwFromID);
//										Zebra::logger->debug("%s(%u) 客户端检查外挂", name, id);
//								}
//#ifdef _XWL_DEBUG
//								processCheckTime = 0;
//#else
//								processCheckTime = zMisc::randBetween(60, 120);
//#endif
//						}
//						else
//								processCheckTime--;
//#ifdef _XWL_DEBUG
//						Zebra::logger->debug("%s(%u) 下次检查间隔 %u 分钟", name, id, processCheckTime);
//#endif
//				}
//*********************************************************
				//soke 打坐修炼给银子
				
			/*	if (issetUState(Cmd::USTATE_SITDOWN))
				{
					int addmoney = zMisc::randBetween(3,5);
					packs.addMoney(addmoney*100,"打坐修炼1分钟银子奖励");

					int addsalary = zMisc::randBetween(2,3);
					packs.addSalary(addsalary*100,"打坐修炼1分钟工资奖励");

					int nExp = zMisc::randBetween(100,150);

					addExp(nExp*getLevel());

					//获取星辰石
					if(zMisc::selectByTenTh(200))
					{
					  SceneEntryPk * am = getTopMaster(); //soke 防御者
	                  if (this->scene->getCountryID()!=((SceneUser *)am)->charbase.country) //玩家升级世界滚动公告
	                  Cmd::Session::t_changeScene_SceneSession cmd;
					  Gm::fetch(this,"id=551 num=1 bind=1");
					  Channel::sendAllInfo(Cmd::INFO_TYPE_EXP,"真幸运！%s的 %s 在打坐修炼中获得星辰石一颗",SceneManager::getInstance().getCountryNameByCountryID(((SceneUser *)am)->charbase.country),charbase.name );
					}
					//获取天魔石
					if(zMisc::selectByTenTh(200))
					{
					  SceneEntryPk * am = getTopMaster(); //soke 防御者
	                  if (this->scene->getCountryID()!=((SceneUser *)am)->charbase.country) //玩家升级世界滚动公告
	                  Cmd::Session::t_changeScene_SceneSession cmd;
					  Gm::fetch(this,"id=679 level=3 num=1 bind=1");
					  Channel::sendAllInfo(Cmd::INFO_TYPE_EXP,"真幸运！%s的 %s 在打坐修炼中获得天魔石四等一颗",SceneManager::getInstance().getCountryNameByCountryID(((SceneUser *)am)->charbase.country),charbase.name );
					}
					//获取定魂石
					if(zMisc::selectByTenTh(200))
					{
					  SceneEntryPk * am = getTopMaster(); //soke 防御者
	                  if (this->scene->getCountryID()!=((SceneUser *)am)->charbase.country) //玩家升级世界滚动公告
	                  Cmd::Session::t_changeScene_SceneSession cmd;
					  Gm::fetch(this,"id=678 level=3 num=1 bind=1");
					  Channel::sendAllInfo(Cmd::INFO_TYPE_EXP,"真幸运！%s的 %s 在打坐修炼中获得定魂石四等一颗",SceneManager::getInstance().getCountryNameByCountryID(((SceneUser *)am)->charbase.country),charbase.name );
					}
				}*/
//**********************************************************
				if (this->emperor)//皇帝
				{
					showCurrentEffect(Cmd::USTATE_EMPEROR, true);
					sendtoSelectedOfficialtate();
				}
				else if (this->king )//国王
				{
					showCurrentEffect(Cmd::USTATE_KING, true);
					sendtoSelectedOfficialtate();
				}
				else if(strstr(this->caption , "城主") != NULL)
				{
					showCurrentEffect(Cmd::USTATE_UNIONMASTER, true);
					sendtoSelectedOfficialtate();
				}
				//桃子增加定时传送地图
				struct tm tv1;
		   	    time_t timValue = time(NULL);
			    zRTime::getLocalTime(tv1, timValue);
			
				// //sky 设置定时传送时间1
				// if (tv1.tm_hour==20 && tv1.tm_min==00) //tm_hour是几点 tm_min是分钟
				// {
				// 	Channel::sendSys(this, Cmd::INFO_TYPE_GOTO, "皇城终极BOSS活动开启.");
				// }
				// //sky 设置定时传送时间2
				// if (tv1.tm_hour==21 && tv1.tm_min==00) //tm_hour是几点 tm_min是分钟
				// {
				// 	Channel::sendSys(this, Cmd::INFO_TYPE_GOTO2, "万宝宫活动开启.");
				// }								
                ///////////////////////////////

				//国家镖车传送 汉国国家镖车时间==================================================
				if (tv1.tm_hour==20 && tv1.tm_min==45 && this->charbase.country == 8 ) //tm_hour是几点 tm_min是分钟
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_BIAOCHE1, "汉国镖车.");
				}
				//国家镖车传送 楚国国家镖车时间==================================================
				if ((tv1.tm_hour==21 && tv1.tm_min==15) && this->charbase.country == 12 ) //tm_hour是几点 tm_min是分钟
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_BIAOCHE2, "楚国镖车.");
				}
				//世界BOSS时间==================================================
				if ((tv1.tm_hour==13 && tv1.tm_min==55)  ) //tm_hour是几点 tm_min是分钟
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_BOSS, "世界boss.");
				}
				//世界BOSS时间==================================================
				if ((tv1.tm_hour==21 && tv1.tm_min==45)  ) //tm_hour是几点 tm_min是分钟
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_BOSS, "世界boss.");
				}
				//自行车时间==================================================
				if ((tv1.tm_hour==20 && tv1.tm_min==25)) //tm_hour是几点 tm_min是分钟
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_BIKE, "自行车.");
				}
				//天降鸿福
				if ((tv1.tm_hour>=20 && tv1.tm_hour<21) && tv1.tm_min <=30) //tm_hour是几点 tm_min是分钟(tv1.tm_hour>=20 && tv1.tm_hour<=21) 20-22之间
				{
					// if (zMisc::selectByPercent(1)) // 几率 网管百分之
					// {
						//INFO_TYPE_YANSE2 是橙色  INFO_TYPE_YANSE1 是紫色 换一下就行，没有吧

						time_t cur_time = time(NULL);
						if ((cur_time - this->charbase.tianjiangtime) > 59) //soke 修改为8小时   
						{
							if (zMisc::selectByPercent(50)) // 几率 网管百分之 这里是翻倍
							{
								SceneUser::addBindObjectNum(120048, 80, 0, 0, 1);
								SceneUser::addBindObjectNum(120052, 20, 0, 0, 1);
								Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "恭喜%s的%s在天降洪福中人品大爆发，获得翻倍卷轴奖励！", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name); // 跳舞礼包改神秘礼包网管
								Channel::sendSys(this, Cmd::INFO_TYPE_TIANJIANG, "");
								this->charbase.tianjiangtime = time(NULL);
							}
							else{ //这里是普通的
								SceneUser::addBindObjectNum(120048, 40, 0, 0, 1);
								SceneUser::addBindObjectNum(120052, 10, 0, 0, 1);
								Channel::sendSys(this, Cmd::INFO_TYPE_TIANJIANG, "");
								this->charbase.tianjiangtime = time(NULL);
							}
							
						}
						
					// }
					
				}
				//检查被捕时间
				checkPunishTime();

				//和平追加善恶描述
				if(getGoodnessState() <= (short)Cmd::GOODNESS_2_1)
				{
					charbase.pkaddition++;
					if(charbase.pkaddition > 1800 /*30*60*/)
					{
						if(!this->issetUState(Cmd::USTATE_PK))
						{
							Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "你进入了PK保护状态！");
						}
						this->setUState(Cmd::USTATE_PK);
						reSendMyMapData();
						/*
						   char Buf[200]; 
						   bzero(Buf , sizeof(Buf));
						   Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
						   constructInPlace(srs);
						   srs->byType = Cmd::MAPDATATYPE_USER;
						   srs->dwTempID = this->tempid;
						   srs->size=1;
						   srs->states[0].state = Cmd::USTATE_PK;
						   srs->states[0].result=charbase.pkaddition;
						   srs->states[0].time=0XFFFF;
						   this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						   sizeof(srs->states[0]) * srs->size);
						// */
						this->sendtoSelectedPkAdditionState();
					}
				}

				if (charbase.trainTime)
				{
					if (charbase.trainTime>=60)
						charbase.trainTime -= 60;
					else
					{
						charbase.trainTime = 0;
						this->clearUState(Cmd::USTATE_DAOJISHI);
					}
					sendtoSelectedTrainState();
				}

				//soke 检测战车时间
			    if (charbase.zhanchetime1 > 0)
			    {
				   if (charbase.zhanchetime1>=6000)
				   {
					   charbase.zhanchetime1 -= 6000;
				   }
				   else
				   {
					  charbase.zhanchetime1 = 0;
					  this->clearUState(Cmd::USTATE_USER_ZC_TYPE1);
				   }
				   sendtoZhancheSelectedOfficialtate1();
			    }
				//soke 检测战车时间
			    if (charbase.zhanchetime2 > 0)
			    {
				   if (charbase.zhanchetime2>=6000)
				   {
					   charbase.zhanchetime2 -= 6000;
				   }
				   else
				   {
					  charbase.zhanchetime2 = 0;
					  this->clearUState(Cmd::USTATE_USER_ZC_TYPE2);
				   }
				   sendtoZhancheSelectedOfficialtate2();
			    }
				//soke 检测战车时间
			    if (charbase.zhanchetime3 > 0)
			    {
				   if (charbase.zhanchetime3>=6000)
				   {
					   charbase.zhanchetime3 -= 6000;
				   }
				   else
				   {
					  charbase.zhanchetime3 = 0;
					  this->clearUState(Cmd::USTATE_USER_ZC_TYPE3);
				   }
				   sendtoZhancheSelectedOfficialtate3();
			    }


				//soke 检测9朵花时间
			    if (charbase.flower1time > 0)
			    {
				   if (charbase.flower1time>=60)
				   {
					   charbase.flower1time -= 60;
				   }
				   else
				   {
					  charbase.flower1time = 0;
					  this->clearUState(Cmd::USTATE_FLOWER_9);
				   }
				   sendtoSelected9State();
			    }
				//soke 检测99朵花时间
			    if (charbase.flower2time > 0)
			    {
				   if (charbase.flower2time>=60)
				   {
					   charbase.flower2time -= 60;
				   }
				   else
				   {
					  charbase.flower2time = 0;
					  this->clearUState(Cmd::USTATE_FLOWER_99);
				   }
				   sendtoSelected99State();
			    }
				//soke 检测999朵花时间
			    if (charbase.flower3time > 0)
			    {
				   if (charbase.flower3time>=60)
				   {
					   charbase.flower3time -= 60;
				   }
				   else
				   {
					  charbase.flower3time = 0;
					  this->clearUState(Cmd::USTATE_FLOWER_999);
					   this->clearUState(Cmd::USTATE_USER_MEILI);
				   }
				   sendtoSelected999State();
				   sendtoSelectedyswcState();
			    }

				//soke 检测9朵蓝色妖姬时间
			    if (charbase.flower4time > 0)
			    {
				   if (charbase.flower4time>=60)
				   {
					   charbase.flower4time -= 60;
				   }
				   else
				   {
					  charbase.flower4time = 0;
					  this->clearUState(Cmd::USTATE_FLOWERL_9);
				   }
				   sendtoSelectedx9State();
			    }
				//soke 检测99朵蓝色妖姬时间
			    if (charbase.flower5time > 0)
			    {
				   if (charbase.flower5time>=60)
				   {
					   charbase.flower5time -= 60;
				   }
				   else
				   {
					  charbase.flower5time = 0;
					  this->clearUState(Cmd::USTATE_FLOWERL_99);
				   }
				   sendtoSelectedx99State();
			    }
				//soke 检测999朵蓝色妖姬时间
			    if (charbase.flower6time > 0)
			    {
				   if (charbase.flower6time>=60)
				   {
					   charbase.flower6time -= 60;
				   }
				   else
				   {
					  charbase.flower6time = 0;
					  this->clearUState(Cmd::USTATE_FLOWERL_999);
					   this->clearUState(Cmd::USTATE_USER_RENQI);
				   }
				   sendtoSelectedx999State();
				   sendtoSelectedyswc2State();
			    }

				sendtoSelectedVIPState();

				//踢出修炼地图
				if (scene->isTrainingMap() && 0==charbase.trainTime)
				{
					Scene * s = SceneManager::getInstance().getSceneByName("中立区·皇城");
					if (s)
					{
						bool suc = changeMap(s,zPos(831,705));
						if (!suc)
							Zebra::logger->error("%s 离开训练地图失败，目的 %s (%d,%d)", name, s->name, pos.x, pos.y);
						else
							Zebra::logger->error("%s 离开训练地图", name);
					}
					else
					{
						Cmd::Session::t_changeScene_SceneSession cmd;
						cmd.id = id;
						cmd.temp_id = tempid;
						cmd.x = 831;
						cmd.y = 705;
						cmd.map_id = 0;
						cmd.map_file[0] = '\0';
						strncpy((char *)cmd.map_name, "中立区·皇城", MAX_NAMESIZE);
						sessionClient->sendCmd(&cmd, sizeof(cmd));
					}
				}
				
				//检测马匹训练时间
				if( horse.data.horseXLlevel > 0 )
				{
					if(horse.data.horseXLtime > 60)
					{
						Zebra::logger->debug("原训练剩余时间为:%u",horse.data.horseXLtime);
						horse.data.horseXLtime -= 60;
						Zebra::logger->debug("扣除后的训练时间为:%u",horse.data.horseXLtime);
						horse.sendData();
						Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "马匹移动速度 %u！",horse.data.speed);
						//Channel::sendNine((SceneUser *)this, "移动速度 %u", curSpeed);
					}
					else
					{
						horse.data.horseXLtime	= 0;
						if( horse.data.horseXLlevel == 1 )
						{
							horse.data.speed	-= 10;
						}
						else if( horse.data.horseXLlevel == 2 )
						{
							horse.data.speed	-= 20;
						}
						else if( horse.data.horseXLlevel == 3 )
						{
							horse.data.speed	-= 30;
						}
						horse.data.horseXLlevel = 0;
						horse.mount(false);
						horse.sendData();
						Channel::sendSys(this,Cmd::INFO_TYPE_GAME,"训练有效时间已到，移除战马训练效果");
					}
				}

				//踢出副本地图
				if (scene->isFubeningMap() && 0==charbase.fubentime)
				{
					Scene * s = SceneManager::getInstance().getSceneByName("中立区·皇城");
					if (s)
					{
						bool suc = changeMap(s,zPos(831,705));
						if (!suc)
							Zebra::logger->error("%s 离开副本地图失败，目的 %s (%d,%d)", name, s->name, pos.x, pos.y);
						else
							Zebra::logger->error("%s 离开副本地图", name);
					}
					else
					{
						Cmd::Session::t_changeScene_SceneSession cmd;
						cmd.id = id;
						cmd.temp_id = tempid;
						cmd.x = 831;
						cmd.y = 705;
						cmd.map_id = 0;
						cmd.map_file[0] = '\0';
						strncpy((char *)cmd.map_name, "中立区·皇城", MAX_NAMESIZE);
						sessionClient->sendCmd(&cmd, sizeof(cmd));
					}
				}
			//soke 检测1.25倍经验时间
			if( this->issetUState(Cmd::USTATE_EXP_125) )
			{
				if (charbase.doubleatime>=60)
				{
					charbase.doubleatime -= 60;
				}
				else
				{
					charbase.doubleatime = 0;
					if(this->issetUState(Cmd::USTATE_EXP_125))
					{
						if(charbase.bitmask & CHARBASE_EXP125) 
						{
							charbase.bitmask &= (~CHARBASE_EXP125);
						}
						Zebra::logger->debug("时间到了移除1.25倍经验状态");
						showCurrentEffect(Cmd::USTATE_EXP_125, false);
					}
				}
			}
			//soke 检测1.50倍经验时间
			if( this->issetUState(Cmd::USTATE_EXP_150) )
			{
				if (charbase.doublebtime>=60)
				{
					charbase.doublebtime -= 60;
				}
				else
				{
					charbase.doublebtime = 0;
					if(this->issetUState(Cmd::USTATE_EXP_150))
					{
						if(charbase.bitmask & CHARBASE_EXP150) 
						{
							charbase.bitmask &= (~CHARBASE_EXP150);
						}
						Zebra::logger->debug("时间到了移除1.50倍经验状态");
						showCurrentEffect(Cmd::USTATE_EXP_150, false);
					}
				}
			}
			//soke 检测1.75倍经验时间
			if( this->issetUState(Cmd::USTATE_EXP_175) )
			{
				if (charbase.doublectime>=60)
				{
					charbase.doublectime -= 60;
				}
				else
				{
					charbase.doublectime = 0;
					if(this->issetUState(Cmd::USTATE_EXP_175))
					{
						if(charbase.bitmask & CHARBASE_EXP175) 
						{
							charbase.bitmask &= (~CHARBASE_EXP175);
						}
						Zebra::logger->debug("时间到了移除1.75倍经验状态");
						showCurrentEffect(Cmd::USTATE_EXP_175, false);
					}
				}
			}
            //soke 检测副本时间
			if( this->issetUState(Cmd::USTATE_FUBEN) )
			{
				if (charbase.fubentime>=60)
				{
					charbase.fubentime -= 60;
				}
				else
				{
					charbase.fubentime = 0;
					if(this->issetUState(Cmd::USTATE_FUBEN))
					{
						if(charbase.bitmask & CHARBASE_FUBEN) 
						{
							charbase.bitmask &= (~CHARBASE_FUBEN);
						}
						Zebra::logger->debug("时间到了移除副本状态");
						showCurrentEffect(Cmd::USTATE_FUBEN, false);
					}
				}
			}
			//soke 检测幸运丹时间
			if( this->issetUState(Cmd::USTATE_LUCKY_DAN) )
			{
				if (charbase.luckytime>=60)
				{
					charbase.luckytime -= 60;
				}
				else
				{
					charbase.luckytime = 0;
					if(this->issetUState(Cmd::USTATE_LUCKY_DAN))
					{
						if(charbase.bitmask & CHARBASE_LUNCKY) 
						{
							charbase.bitmask &= (~CHARBASE_LUNCKY);
						}
						Zebra::logger->debug("时间到了移除幸运丹状态");
						showCurrentEffect(Cmd::USTATE_LUCKY_DAN, false);
					}
				}
			}

             //soke 跳舞区在线挖宝随机给物品
			 if (issetUState(Cmd::USTATE_USER_WABAO))
			 {
				bool isInDanceZone = scene->checkZoneType(pos, ZoneTypeDef::ZONE_DANCE);
                if ( isInDanceZone )
                {
					// int addmoney = zMisc::randBetween(1,3);
					// packs.addMoney2(addmoney*1000,"挖宝1分钟银子奖励");

					// if(zMisc::selectByTenTh(400))
					// {
					//    int addsalary = zMisc::randBetween(1,3);
					//    packs.addSalary3(addsalary*200,"挖宝1分钟工资奖励");
					// }
					// if(zMisc::selectByTenTh(20))
					// {
					//    int addsalary = zMisc::randBetween(1,2);
					//    packs.addSalary3(addsalary*10,"挖宝1分钟金子奖励");
					// }

					//获取挖宝礼包
					if(zMisc::selectByPercent(50))
					{
						SceneUser::addBindObjectNum(202388,1,0,0,1);	
					  	Channel::sendAllInfo(Cmd::INFO_TYPE_EXP,"真幸运！%s的%s挖到挖宝礼包1个",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
					}
				}
			}

				countFriendDegree(); //计算友好度
				/*
				 * whj 应林总强烈要求暂时屏蔽疲劳度功能
				 */ 
				processTire();        //soke 检测疲劳时间
				processAnswerCount(); //soke 检测答题次数
				processLuckyCount(); //soke 检测幸运丹次数
				processLoginCount(); //soke 检测每日登陆领取次数
				processSvipCount();  //soke 检测每日svip领取次数
				processTaimCount();  //soke 检测每日太庙完成次数
				processFubenCount(); //soke 检测每日副本激活次数
				processBikeCount(); //soke 检测每日自行车激活次数
				processChshowCount();  //soke 检测称号排行榜变量


				//时间消耗类装备 ,现在只有双倍经验类和自动补给类
				if(this->getState() != SceneUser::SceneEntry_Death)
				{
					zObject *obj = this->packs.equip.getObjectByEquipPos(Cmd::EQUIPCELLTYPE_ADORN);
					if(obj)
						switch(obj->base->kind)
						{
							case ItemType_DoubleExp:
								{
									packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_ADORN , ItemType_DoubleExp , 60,true,true);
								}
								break;
							case ItemType_Tonic:
								{
									packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_ADORN , ItemType_Tonic , 60 , true , false);
								}
								break;
                            //soke 时装位置改了
							/*case ItemType_FashionBody:
								{
									packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_ADORN , ItemType_FashionBody,1 , true ,false);
								}
								break;
							case ItemType_HighFashionBody: //soke 高级时装
						        {
							        packs.equip.reduceDur(this,Cmd::EQUIPCELLTYPE_ADORN,ItemType_HighFashionBody,1,true,false);
						        }
					         	break;*/
							case ItemType_Amulet:
								{
									if(this->scene->isIncScene())
									{
										packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_ADORN , ItemType_Amulet,60 , true ,false);
									}
								}
								break;
						}
					obj = this->packs.equip.getObjectByEquipPos(Cmd::EQUIPCELLTYPE_ADORN + 1);
					if(obj)
						switch(obj->base->kind)
						{
							case ItemType_DoubleExp:
								{
									packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_ADORN + 1 , ItemType_DoubleExp , 60,true,true);
								}
								break;
							case ItemType_Tonic:
								{
									packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_ADORN + 1 , ItemType_Tonic , 60 , true , false);
								}
								break;
                            //soke 时装位置改了
							/*case ItemType_FashionBody:
								{
									packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_ADORN + 1 , ItemType_FashionBody,1 , true ,false);
								}
								break;
							case ItemType_HighFashionBody: //soke 高级时装
						        {
							        packs.equip.reduceDur(this,Cmd::EQUIPCELLTYPE_ADORN + 1,ItemType_HighFashionBody,1,true,false);
						        }
						        break;*/
							case ItemType_Amulet:
								{
									if(this->scene->isIncScene())
									{
										packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_ADORN + 1 , ItemType_Amulet,60 , true ,false);
									}
								}
								break;
						}
			        //soke 时装位置
				    zObject *zbj = this->packs.equip.getObjectByEquipPos(Cmd::EQUIPCELLTYPE_FASHION);
					if(zbj)
						switch(zbj->base->kind)
						{
						//soke 新的位置
							case ItemType_FashionBody:
								{
									packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_FASHION , ItemType_FashionBody,1 , true ,false);
								}
								break;
							case ItemType_HighFashionBody:
								{
									packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_FASHION , ItemType_HighFashionBody ,1 , true , false);
								}
								break;
							case ItemType_XHighFashionBody:
								{
									packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_FASHION , ItemType_XHighFashionBody ,1 , true , false);
								}
								break;
							case ItemType_SHighFashionBody:
								{
									packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_FASHION , ItemType_SHighFashionBody ,1 , true , false);
								}	
								break;	

						}
						////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				}
				if (this->packs.equip.pack(EquipPack::L_PACK)) this->packs.equip.pack(EquipPack::L_PACK)->consume_dur_by(this, SceneTimeTick::currentTime);
				if (this->packs.equip.pack(EquipPack::R_PACK)) this->packs.equip.pack(EquipPack::R_PACK)->consume_dur_by(this, SceneTimeTick::currentTime);


			}

			if (0==_5_sec_count%6)//30sec
			{
				for (cartoon_it it=cartoonList.begin(); it!=cartoonList.end(); it++)
				{
					if ((cartoon && cartoon->getCartoonID()==it->first)
							|| it->second.state==Cmd::CARTOON_STATE_WAITING
							|| it->second.state==Cmd::CARTOON_STATE_ADOPTED)
						continue;

					if (it->second.sp<it->second.maxSp)
					{
						it->second.sp++;
						if (0==it->second.sp%10)
						{
							Cmd::stAddCartoonCmd send;
							send.isMine = true;
							send.cartoonID = it->first;
							send.data = it->second;
							sendCmdToMe(&send, sizeof(send));
						}
					}
				}
			}

			//PK地图15秒加经验
			if (0==_5_sec_count%3 && scene->isPkMap())//15sec
				addExp(charbase.level*charbase.level/4);
		}

		if (_ten_sec(SceneTimeTick::currentTime))
		{
			mask.on_timer();
			checkNpcHoldDataAndPutExp();
			//队长负责队员两分钟掉线的检查
			SceneUser *leader = SceneUserManager::getMe().getUserByTempID(team.getLeader());
			if(leader&&(leader->tempid == tempid))
			{
				leader->team.checkOffline(leader);
			}

            //////////////////////////////////////////////
			//温泉系统 每10秒钟给经验
            spaTimer();
		}

		if (_fiften_sec(SceneTimeTick::currentTime))
		{

			//soke 挖宝系统 每15秒钟给经验
            wabaoTimer();
		}
		
		if (_fif_sec(SceneTimeTick::currentTime))
		{
			//温泉每15秒给经验
            spaTimer();
			//家族地图打坐
			jiazuTimer();
			
			//跳舞每15秒给经验
            if(this->scene->getRealMapID() == 139 )
            {
				bool isInSpaZone = scene->checkZoneType(pos, ZoneTypeDef::ZONE_PRIVATE_STORE); //摆摊区
				if ( isDance() )
				{
					if ( isInSpaZone )
			        {
						addExp(charbase.level*charbase.level);
					}
						if(zMisc::selectByTenTh(500)) //几率
					{
                      SceneUser::addBindObjectNum(202388,1,0,0,1);					  
					  Channel::sendAllInfo(Cmd::INFO_TYPE_EXP,"真幸运！%s的%s在跳舞中获得跳舞礼包1个！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );							

					}					
				}
			}
		}

		//回写档案
		if(_writeback_timer(SceneTimeTick::currentTime))
		{
			save(Cmd::Record::TIMETICK_WRITEBACK);
		}
	}

	if (summon || pet)
	{
		SceneEntryPk * dt = getDefTarget();
		if (dt)
		{
			if (dt->scene!=this->scene
					|| dt->getState()!=SceneEntry_Normal
					|| !scene->zPosShortRange(dt->getPos(), getPos(), 20))
				clearDefTarget();
		}

		if (checkEndBattleTime(SceneTimeTick::currentTime)&&(0!=curTargetID||0!=defTargetID))
			leaveBattle();
	}

	return true;
}



/**
 * \brief 取消某个状态给用户
 *
 * \author zjw
 */

void SceneUser::clearStateToNine(WORD state)
{
	//Zebra::logger->debug("clearStateToNine:%d",state);
	if(!scene) return;
	Cmd::stClearStateMapScreenUserCmd send;
	send.type=Cmd::MAPDATATYPE_USER;
	send.dwTempID = this->tempid;
	send.wdState =state;
	//检查是否隐身
	if (SceneEntry_Hide!=getState()&&!this->hideme)
	{
		this->scene->sendCmdToNine(getPosI(),&send,sizeof(send));
	}
	else
	{
		sendCmdToMe(&send,sizeof(send));
	}
}
/**
 * \brief 设置某个状态给用户
 *
 * \author zjw
 */

void SceneUser::setStateToNine(WORD state)
{
	//Zebra::logger->debug("setStateToNine:%d",state);
	if(!scene) return;
	Cmd::stSetStateMapScreenUserCmd send;
	send.type=Cmd::MAPDATATYPE_USER;
	send.dwTempID = this->tempid;
	send.wdState =state;
	//检查是否隐身
	if (SceneEntry_Hide!=getState()&&!this->hideme)
	{
		if(state == Cmd::USTATE_WAR)
		{
			SendStateWarToNineEveryOne one(this,send.wdState);
			const zPosIVector &pv = scene->getNineScreen(getPosI());
			for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
			{
				scene->execAllOfScreen(zSceneEntry::SceneEntry_Player,*it,one);
			}
		}
		else
		{
			this->scene->sendCmdToNine(getPosI(),&send,sizeof(send));
		}
	}
	else
	{
		sendCmdToMe(&send,sizeof(send));
	}
}

void SceneUser::sendGoodnessToNine()
{
	//Zebra::logger->debug("setStateToNine:%d",state);
	if(!scene) return;
	Cmd::stGoodnessStateMapScreenUserCmd send;
	send.dwTempID = this->tempid;
	send.dwGoodness =this->charbase.goodness;
	//检查是否隐身
	if (SceneEntry_Hide!=getState()&&!this->hideme)
	{
			this->scene->sendCmdToNine(getPosI(),&send,sizeof(send));
	}
	else
	{
		sendCmdToMe(&send,sizeof(send));
	}
}
bool SceneUser::changeMap(Scene *newscene,const zPos &pos, bool ignoreUserLevel)
{
	if (!ignoreUserLevel &&(this->charbase.level < 1) && (newscene != this->scene)) //桃子修改1级可出清源村
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "1级内无法离开清源村进行冒险!");
		return false;
	}
	if (189==scene->getRealMapID() && isRedNamed())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你正在服刑期间，无法传送!");
		return false;
	}
	if (203==scene->getRealMapID() && charbase.punishTime)
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你正在服刑期间，无法传送!");
		return false;
	}
	this->clearUState(Cmd::USTATE_XIXINGEMIA);

	if(newscene)
	{
		Zebra::logger->trace("%s(%d)本服切换场景(%s-->%s),坐标(%d,%d):(%d,%d)",this->name,this->id,this->scene->name,newscene->name,this->getPos().x,this->getPos().y,pos.x,pos.y);
		if ((!newscene->checkUserLevel(this))&&(newscene!=this->scene)) return false;
	}
	//取消交易摆摊状态
	tradeorder.cancel();
	privatestore.step(PrivateStore::NONE, this);

	tradeorder.cancel();
	privatestore.step(PrivateStore::NONE, this);
	//先从目前场景删除
	if(LeaveScene())
		//添加到新场景
		if(intoScene(newscene,false,pos))
		{
			collectPets();
			saveGuard = false;
			saveAdopt = false;
			return true;
		}
	return false;

}
/**
 * \brief 招唤宠物
 * \param id 宠物ID
 * \param type 宠物类型
 * \param standTime 持续时间
 * \param sid 脚本ID
 * \param petName 宠物名称
 * \param anpcid NPC的ID
 * \return 如果该宠物存在，返回宠物对象的指针，否则为NULL
 */
ScenePet * SceneUser::summonPet(DWORD id, Cmd::petType type, DWORD standTime, DWORD sid, const char * petName, DWORD anpcid, zPos pos, BYTE vdir)
{
	if ((Cmd::PET_TYPE_NOTPET>=type)||(Cmd::PET_TYPE_XCARTOON2<type))
	{
		Zebra::logger->trace("SceneUser::summonPet(): %s 召唤未知类型的宠物 type=%d", name, type);
		return false;
	}

	/*
	   if (Cmd::PET_TYPE_PET==type && petData.state==Cmd::PET_STATE_DEAD)
	   {
	   Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，你的宠物已经死亡，请先复活它");
	   return 0;
	   }
	   */

	//国王的马
	if (king && type==Cmd::PET_TYPE_RIDE) id = KING_HORSE_ID;
	if (emperor && type==Cmd::PET_TYPE_RIDE) id = EMPEROR_HORSE_ID;
	zNpcB *base = npcbm.get(id);
	zNpcB *abase = NULL;
	if (anpcid>0) abase = npcbm.get(anpcid);
	if (NULL == base) return false;

	t_NpcDefine define;
	define.id = base->id;
	strncpy(define.name, base->name, MAX_NAMESIZE-1);
	if (pos.x != 0 && pos.y !=0)
		define.pos = pos;
	else
		define.pos = getPos();
	define.num = 1;
	define.interval = 5;
	define.initstate = zSceneEntry::SceneEntry_Normal;
	define.width = CALL_PET_REGION;
	define.height = CALL_PET_REGION;
	define.pos -= zPos(CALL_PET_REGION/2,CALL_PET_REGION/2);
	define.scriptID = sid;
	scene->initRegion(define.region, define.pos, define.width, define.height);

	ScenePet * newPet = NULL;
	if (type==Cmd::PET_TYPE_GUARDNPC)
		newPet = scene->summonOneNpc<GuardNpc>(define, pos, base, standTime, abase,vdir);
	else if (type==Cmd::PET_TYPE_CARTOON)
		newPet = scene->summonOneNpc<CartoonPet>(define, pos, base, standTime, abase, vdir);
	else if (type==Cmd::PET_TYPE_XCARTOON)
		newPet = scene->summonOneNpc<CartoonPet>(define, pos, base, standTime, abase, vdir);
	else if (type==Cmd::PET_TYPE_XCARTOON2)
		newPet = scene->summonOneNpc<CartoonPet>(define, pos, base, standTime, abase, vdir);
	else
		newPet = scene->summonOneNpc<ScenePet>(define, pos, base, standTime, abase, vdir);

	if (newPet)
	{
		if (petName&&(0!=strncmp(petName, "", MAX_NAMESIZE)))
			strncpy(newPet->name, petName, MAX_NAMESIZE-1);

		newPet->setPetType(type);
		newPet->setMaster(this);
		SceneNpcManager::getMe().addSpecialNpc(newPet);
		//newPet->petData.type = type;

		using namespace Cmd;
		switch (type)
		{
			case PET_TYPE_RIDE:
				{
					if (ridepet)
					{
						//ridepet->toDie(tempid);
						killOnePet(ridepet);
					}
					ridepet = newPet;
				}
				break;
			case PET_TYPE_PET:
				{
					if (pet)
					{
						pet->toDie(tempid);
						killOnePet(pet);
					}
					if (summon)
					{
						summon->toDie(tempid);
						killOnePet(summon);
					}
					pet = newPet;
					pet->petData.state = Cmd::PET_STATE_NORMAL;
				}
				break;
			case PET_TYPE_SUMMON:
				{
					if (pet)
					{
						pet->toDie(tempid);
						killOnePet(pet);
					}
					if (summon)
					{
						summon->toDie(tempid);
						killOnePet(summon);
					}
					summon = newPet;
				}
				break;
			case PET_TYPE_GUARDNPC:
				{
					if (guard)
					{
						guard->toDie(tempid);
						killOnePet(guard);
					}
					guard = newPet;
				}
				break;
			case PET_TYPE_TOTEM:
				totems.push_back(newPet);
				break;
			case PET_TYPE_CARTOON:
				{
					if (cartoon)
					{
						cartoon->getCartoonData().state = Cmd::CARTOON_STATE_PUTAWAY;
						cartoon->save(Cmd::Session::SAVE_TYPE_PUTAWAY);
						cartoon->toDie(tempid);
						killOnePet(cartoon);
					}
					cartoon = (CartoonPet *)newPet;
				}
				break;
			case PET_TYPE_XCARTOON: //侍卫宝宝
				{
					if (xcartoon)
					{
						xcartoon->getCartoonData().state = Cmd::CARTOON_STATE_PUTAWAY;
						xcartoon->save(Cmd::Session::SAVE_TYPE_PUTAWAY);
						xcartoon->toDie(tempid);
						killOnePet(xcartoon);
					}
					xcartoon = (CartoonPet *)newPet;
				}
				break;
			case PET_TYPE_XCARTOON2: //侍卫宝宝
				{
					if (xcartoon2)
					{
						xcartoon2->getCartoonData().state = Cmd::CARTOON_STATE_PUTAWAY;
						xcartoon2->save(Cmd::Session::SAVE_TYPE_PUTAWAY);
						xcartoon2->toDie(tempid);
						killOnePet(xcartoon2);
					}
					xcartoon2 = (CartoonPet *)newPet;
				}
				break;
			default:
				Zebra::logger->trace("summonPet(): 未知的宠物类型 %d", type);
				break;
		}
		/*
		   Cmd::stAddPetPetCmd addPet;
		   addPet.type = type;
		   addPet.id = newPet->tempid;
		   sendCmdToMe(&addPet, sizeof(addPet));

		   if (Cmd::PET_TYPE_RIDE==type)
		   {
		   horse.sendData();
		   }
		   else
		   {
		   Cmd::stRefreshDataPetCmd ref;
		   ref.type = type;
		   ref.id = newPet->tempid;
		   newPet->full_PetDataStruct(ref.data);
		   sendCmdToMe(&ref, sizeof(ref));
		   }
		   */
		this->skillStatusM.processPassiveness();	// 处理我的被动状态影响
		newPet->setDir(vdir);
		newPet->sendData();
		newPet->sendPetDataToNine();

		Zebra::logger->debug("[宠物]%s(%u) 增加宠物 %s(%u) 类型 %d", name, id, newPet->name, newPet->tempid, type);
	}
	else
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "太拥挤了，召唤失败-。-");

	return newPet;
}


/**
 * \brief  招唤宠物或图腾
 *
 * \param id 宠物ID或图腾ID
 * \param standTime  图腾持续时间
 *
 * \return 如果该宠物存在，返回宠物对象的指针，否则为NULL
 SceneNpc * SceneUser::summonPet(DWORD id, DWORD standTime, DWORD anpcid)
 {
 zNpcB *base = npcbm.get(id);
 zNpcB *abase = NULL;
 if (anpcid>0) abase = npcbm.get(anpcid);
 if (NULL == base) return false;

 t_NpcDefine define;
 zPos pos = getPos();
 define.id = base->id;
 strcpy(define.name, base->name);
 define.pos = getPos();
 define.num = 1;
 define.interval = 30;
 define.initstate = zSceneEntry::SceneEntry_Normal;
 define.width = 2;
 define.height = 2;
 define.pos -= zPos(1,1);
 scene->initRegion(define.region, define.pos, define.width, define.height);

 SceneNpc * pet = scene->summonOneNpc<SceneNpc>(define, pos, base, standTime, abase);

 if (pet)
 totems.push_back(pet);
 return pet;
 }
 */

//醉梦修复镖车专用
void SceneUser::clearbiaoche(void)
{
	if (guard)
	{
		guard->toDie(tempid);
		killOnePet(guard);
	}
}
void SceneUser::removeNineEntry(zPosI posi)
{
	GetAllRemovePosNpc allNpcs(this);
	GetAllRemovePosUser allUsers(this);
	const zPosIVector &pv = scene->getNineScreen(posi);
	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		scene->execAllOfScreen(zSceneEntry::SceneEntry_NPC, *it,allNpcs);
		scene->execAllOfScreen(zSceneEntry::SceneEntry_Player, *it,allUsers);
	}
	if (allNpcs.canSend())
	{
		sendCmdToMe(allNpcs.getSendBuffer(),allNpcs.getSize());
	}
	if (allUsers.canSend())
	{
		sendCmdToMe(allUsers.getSendBuffer(),allUsers.getSize());
	}
}
/**
 * \brief 使角色移动到新的坐标位置
 *
 * \param newPos 新坐标
 *
 * \return 移动成功返回TRUE,否则返回FALSE
 *
 */
bool SceneUser::goTo(zPos &newPos)
{
	zPos findedPos;
	zPos oldPos = getPos();
	zPosI oldPosI=getPosI();
	bool founded = scene->findPosForUser(newPos, findedPos);
	//这里考虑GM可能有其它用途,暂时不强行弄到有效坐标位置
	/*
	   if(!founded)
	   {
	   founded = scene->randzPosByZoneType(ZoneTypeDef::ZONE_NONE , findedPos);
	   }
	// */
	//取消交易摆摊状态
	tradeorder.cancel();
	privatestore.step(PrivateStore::NONE, this);

	tradeorder.cancel();
	privatestore.step(PrivateStore::NONE, this);
	if(scene->refresh(this,founded ? findedPos : newPos))
	{
		Cmd::stRemoveUserMapScreenUserCmd remove;
		remove.dwUserTempID=tempid;
		scene->clearBlock(oldPos);
		//检查是否隐身
		if (SceneEntry_Hide!=getState()&&!this->hideme)
		{
			scene->sendCmdToNine(oldPosI,&remove,sizeof(remove));
			scene->setBlock(getPos());
		}
		else
		{
			sendCmdToMe(&remove,sizeof(remove));
		}
		removeNineEntry(oldPosI);

		sendMeToNine();
		sendNineToMe();
		//校验9屏所有玩家以及Npc坐标
#if 0
		GetAllPos allNpcs(Cmd::MAPDATATYPE_NPC, this);
		GetAllPos allUsers(Cmd::MAPDATATYPE_USER, this);
		const zPosIVector &pv = scene->getNineScreen(getPosI());
		for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
		{
			scene->execAllOfScreen(zSceneEntry::SceneEntry_NPC, *it,allNpcs);
			scene->execAllOfScreen(zSceneEntry::SceneEntry_Player, *it,allUsers);
		}
		if (allNpcs.canSend())
		{
			sendCmdToMe(allNpcs.getSendBuffer(),allNpcs.getSize());
		}
		if (allUsers.canSend())
		{
			sendCmdToMe(allUsers.getSendBuffer(),allUsers.getSize());
		}
#endif
		return true;
	}
	return false;
}

/**
 * \brief 在屏幕内定点移动
 * \author fqnewman
 */
void SceneUser::jumpTo(zPos &newPos)
{
	goTo(newPos);
}

/**
 * \brief 使角色移动到一个区域的随机的一点上
 *
 * \param center 区域中心点
 * \param rectx 区域X坐标
 * \param recty 区域Y坐标
 *
 * \return 移动成功返回TRUE, 否则返回FALSE
 *
 */
bool SceneUser::goToRandomRect(zPos center , WORD rectx , WORD recty)
{
	if(center.x == 0 && center.y == 0)
	{
		center = getPos();
	}
	zPos randPos;
	int count=0;
	do {
	    if(this->scene->randzPosOnRect(center , randPos , rectx , recty))
	    {
		    if (!scene->checkBlock(randPos,TILE_BLOCK) && (!scene->checkBlock(randPos,TILE_ENTRY_BLOCK)|| this->liquidState))
			{
		        return goTo(randPos);
			}
			count++;
		}
	}while(count<10);
	return false;
}


void SceneUser::sendInitHPAndMp()
{
	Cmd::stSetHPAndMPDataUserCmd ret;
	ret.dwHP = charbase.hp;
	ret.dwMP = charbase.mp;
	//ret.dwSP = charbase.sp;
	sendCmdToMe(&ret , sizeof(ret));
}
/**
 * \brief 填充t_MainUserData命令
 *
 * \param data 待填充的命令
 */
void SceneUser::full_t_MainUserData(Cmd::t_MainUserData &data) const
{
	bzero(&data,sizeof(data));
	data.dwUserTempID=tempid;
	data.userid=charbase.id;
	data.level=charbase.level;
	data.hp=charbase.hp;
	data.maxhp=charstate.maxhp;
	data.resumehp=charstate.resumehp;
	data.mp=charbase.mp;
	data.maxmp=charstate.maxmp;
	data.resumemp=charstate.resumemp;
	data.sp=charbase.sp;
	data.maxsp=charstate.maxsp;
	data.resumesp=charstate.resumesp;
	data.pdamage=charstate.pdamage;
	data.maxpdamage=charstate.maxpdamage;
	data.mdamage=charstate.mdamage;
	data.maxmdamage=charstate.maxmdamage;
	data.pdefence=charstate.pdefence;
	data.mdefence=charstate.mdefence;
	data.exp=charbase.exp;
	data.nextexp=charstate.nextexp;
	data.attackrating=charstate.attackrating;
	data.attackdodge=charstate.attackdodge;
	data.lucky=charstate.lucky;
	data.charm=charstate.charm;
	data.skillPoint=charbase.skillpoint;					/// 技能点数
	data.points=charbase.points;
	data.country=charbase.country;
	data.pkmode=pkMode;
	data.bang=charstate.bang;
	data.zhanli = (data.maxpdamage+data.maxmdamage+data.pdefence+data.mdefence)/18; //add 2023-04-10 战力
	data.wdProperty[0]=charstate.wdProperty[0];
	data.wdProperty[1]=charstate.wdProperty[1];
	data.wdProperty[2]=charstate.wdProperty[2];
	data.wdProperty[3]=charstate.wdProperty[3];
	data.wdProperty[4]=charstate.wdProperty[4];

	data.stdpdamage		= charstate.stdpdamage;					/// 标准物理攻击力
	data.stdmdamage		= charstate.stdmdamage;					/// 标准法术攻击力
	data.stdpdefence	= charstate.stdpdefence;				/// 标准物理防御力
	data.stdmdefence	= charstate.stdmdefence;				/// 标准法术防御力
	data.stdbang		= charstate.stdbang;					/// 标准重击率

	data.wdStdProperty[0]=charbase.wdProperty[0];
	data.wdStdProperty[1]=charbase.wdProperty[1];
	data.wdStdProperty[2]=charbase.wdProperty[2];
	data.wdStdProperty[3]=charbase.wdProperty[3];
	data.wdStdProperty[4]=charbase.wdProperty[4];

	data.wdTire = this->wdTire;
	data.fivetype = this->getFiveType();
	data.fivepoint = this->getFiveLevel();
	data.money = charbase.money;
	data.honor = charbase.honor;
	data.maxhonor = charbase.maxhonor;
	data.gold = charbase.gold;
	data.ticket = charbase.ticket;
	data.salary = charbase.salary;
	data.charvip = charbase.charvip;
	data.chongzhi = charbase.chongzhi; //add 2023-10-02 充值
	data.killerone = charbase.killerone;
	data.killertwo = charbase.killertwo;
	data.killerthe = charbase.killerthe;
	data.changejob = charbase.changejob;
	data.round = charbase.round;
	data.folwers = charbase.folwers; 
	data.moods = charbase.moods;  
	data.flloves = charbase.flloves; 
	data.angel = charbase.angel;				
	data.hanbing = charbase.hanbing;
	data.bitmask = charbase.bitmask;
	data.m13axhp = charbase.m13axhp;	         //MYY 百兽图鉴 最大生命值
	data.p13damage = charbase.p13damage;         //MYY 百兽图鉴 最小攻击力
	data.m13axpdamage = charbase.m13axpdamage;	 //MYY 百兽图鉴 最大攻击力
	data.m13damage = charbase.m13damage;         //MYY 百兽图鉴 最小法术攻击力
	data.m13axmdamage = charbase.m13axmdamage;   //MYY 百兽图鉴 最大法术攻击力
	data.p13defence = charbase.p13defence;		 //MYY 百兽图鉴 物防
	data.m13defence = charbase.m13defence;		 //MYY 百兽图鉴 魔防	
	data.touxianlevel = charbase.touxianlevel;   //头衔
	data.chenghao_select = charbase.chenghao_select; //称号
	data.zhancheid = charbase.zhancheid; //战车id
	data.zhanchetype = charbase.zhanchetype; //战车类型
}

/**
 * \brief  获取我的移动速度
 * \return 移动速度 毫秒
 */
WORD SceneUser::getMyMoveSpeed() const
{
	WORD curSpeed = charstate.movespeed;
	WORD attSpeed = charstate.attackspeed; //soke 增加攻击速度
	if (!horse.mount() || skillValue.movespeed<0)
	{
		if (charstate.movespeed > skillValue.movespeed)
		{
			curSpeed = charstate.movespeed-skillValue.movespeed;
		}
		else
		{
			curSpeed = 0;
		}
	}
	curSpeed = (WORD)(curSpeed*(1 + skillValue.array_dmvspeed/100.0f));
    /////////////////////////////////////////////
	if ( isSpa() ) curSpeed *= 2;
	if (curSpeed <320) curSpeed=320;
	//if (horse.isuesHorseone()) curSpeed = 220;
	//soke 攻击速度限制
	if (attSpeed <440) attSpeed=440;

    //soke 隐身移动速度
    if ( getState() == SceneUser::SceneEntry_Hide ) curSpeed = 320;

	if (this->assault) curSpeed = 640/4;

    //soke 飞行速度
	if ( isFly() ) curSpeed = 640/2;
	////////////////////////////////////////

	//soke 挖宝速度
	if ( isWabao() ) curSpeed = 640/2;

#ifdef _XWL_DEBUG
	//Channel::sendNine((SceneUser *)this, "移动速度 %u", curSpeed);
#endif
	return curSpeed;
}

/**
 * \brief 填充t_UserData命令
 *
 * \param data 待填充的命令
 */
void SceneUser::full_t_UserData(Cmd::t_UserData &data) //国王皇帝马外形
{
	data.dwUserTempID=tempid;

	data.type=charbase.type;
	data.face=charbase.face;
	data.goodness=charbase.goodness;

	if (!mask.is_masking() ) 
	{
		strncpy(data.name,name,MAX_NAMESIZE);
		data.country=charbase.country;
		//live_skills.titles(data.live_skills, sizeof(data.live_skills));
	}
	else 
	{
		strncpy(data.name, "蒙面人",MAX_NAMESIZE);
	}

	data.sculpt.dwHorseID=horse.horse();
	// if (king) data.sculpt.dwHorseID = KING_HORSE_ID;           //soke 显示国王的马
	// if (emperor) data.sculpt.dwHorseID = EMPEROR_HORSE_ID;     //soke 显示皇帝的马
	/*
	//Zebra::logger->debug("马牌:%d", data.sculpt.dwHorseID);
	if(packs.equip.body)
	data.sculpt.dwBodyID=packs.equip.body->base->id;
	else
	data.sculpt.dwBodyID=0;
	// */
	if(packs.equip.equip(EquipPack::HANDL))
	{
		data.sculpt.dwLeftHandID=packs.equip.equip(EquipPack::HANDL)->base->id;
		data.dwLeftWeaponColor=packs.equip.equip(EquipPack::HANDL)->data.color;
	}
	else
	{
		data.sculpt.dwLeftHandID=0;
		data.dwLeftWeaponColor=0;
	}
	if(packs.equip.equip(EquipPack::HANDR))
	{
		data.sculpt.dwRightHandID=packs.equip.equip(EquipPack::HANDR)->base->id;
		data.dwRightWeaponColor=packs.equip.equip(EquipPack::HANDR)->data.color;
	}
	else
	{
		data.sculpt.dwRightHandID=0;
		data.dwRightWeaponColor=0;
	}
	data.sculpt.dwHairID=getHairType();
	data.dwHairRGB=getHairColor();
	//如果有装备Custom颜色取物品颜色,System取道具表中颜色,否则Custom取人物属性随机后的color,系统色取0
    //soke 时装显示改了
	/*if (packs.equip.equip(EquipPack::OTHERS2)&& (packs.equip.equip(EquipPack::OTHERS2)->base->kind == ItemType_FashionBody || packs.equip.equip(EquipPack::OTHERS2)->base->kind == ItemType_HighFashionBody) )
	{
		data.sculpt.dwBodyID=packs.equip.equip(EquipPack::OTHERS2)->base->id;
		data.dwBodyColorCustom = packs.equip.equip(EquipPack::OTHERS2)->data.color;
	}
	else if (packs.equip.equip(EquipPack::OTHERS3)&& (packs.equip.equip(EquipPack::OTHERS3)->base->kind == ItemType_FashionBody || packs.equip.equip(EquipPack::OTHERS3)->base->kind == ItemType_HighFashionBody) )
	{
		data.sculpt.dwBodyID=packs.equip.equip(EquipPack::OTHERS3)->base->id;
		data.dwBodyColorCustom = packs.equip.equip(EquipPack::OTHERS3)->data.color;
	}*/

     //soke 时装显示
    if (packs.equip.equip(EquipPack::OTHERS4) && (packs.equip.equip(EquipPack::OTHERS4)->base->kind == ItemType_FashionBody 
	|| packs.equip.equip(EquipPack::OTHERS4)->base->kind == ItemType_HighFashionBody 
	|| packs.equip.equip(EquipPack::OTHERS4)->base->kind == ItemType_XHighFashionBody) )
	{
		data.sculpt.dwBodyID=packs.equip.equip(EquipPack::OTHERS4)->base->id;
		data.sculpt.dwBodyKind=packs.equip.equip(EquipPack::OTHERS4)->base->kind;
		data.dwBodyColorCustom = packs.equip.equip(EquipPack::OTHERS4)->data.color;
	}
	else if (packs.equip.equip(EquipPack::BODY)) 
	{
		data.sculpt.dwBodyID=packs.equip.equip(EquipPack::BODY)->base->id;
		data.sculpt.dwBodyKind=packs.equip.equip(EquipPack::BODY)->base->kind;
		data.dwBodyColorCustom = packs.equip.equip(EquipPack::BODY)->data.color;
	}
	else 
	{
		data.dwBodyColorCustom = charbase.bodyColor;
		data.sculpt.dwBodyID=0;
		data.sculpt.dwBodyKind=0;
	}

	data.attackspeed=(WORD)((((float)charstate.attackspeed)/640.0f)*100.0f);
	//data.movespeed=  charstate.movespeed-skillValue.movespeed;
	//if (this->assault) data.movespeed = 640/4;
	//Zebra::logger->debug("移动速度:%d",getMyMoveSpeed());
	data.movespeed = getMyMoveSpeed();
	//bcopy(byState,data.state,sizeof(byState));
	data.dwChangeFaceID = this->dwChangeFaceID;
	data.level = this->charbase.level;
	/*
	if(this->charbase.level<10)
	{
		data.level = 1;
	}
	else if(this->charbase.level>=10&&this->charbase.level<20)
	{
		data.level = 11;
	}
	else if(this->charbase.level>=20&&this->charbase.level<TEAM_HONOR_MEMBER_LEVEL)
	{
		data.level = 31; // 荣誉之星按是否等于21来判断是否不可带人
	}
	else
	{
		data.level = 21;
	}
	// */

	data.grace = this->charbase.grace;
	data.exploit = this->charbase.exploit;
	data.dwArmyState = this->dwArmyState;
	data.touxianlevel = this->charbase.touxianlevel;
	data.hanbing = this->charbase.hanbing;

	//strncpy(data.unionName, this->unionName, sizeof(data.unionName));
	//strncpy(data.septName, this->septName, sizeof(data.septName));
	data.dwUnionID = this->charbase.unionid;
	data.dwSeptID = this->charbase.septid;

	if (this->king && !this->emperor)
	{
		strncpy(data.caption, (this->charbase.type==1)?"国王":"女王", sizeof(data.caption));
	}
	else if (this->emperor)
	{
		strncpy(data.caption, (this->charbase.type==1)?"皇帝":"女皇", sizeof(data.caption));
	}
	else if (this->kingConsort == 1 && !this->isCatcherState() && !this->isCatchexState() && !this->isCensorState() && !this->isCensosState() && !this->isZaixiangState() && !this->isYuanshuaiState())
	{
		strncpy(data.caption, (this->charbase.type==1)?"王夫":"王后", sizeof(data.caption));
	}
	else if (this->isCatcherState() && this->kingConsort == 1)
	{
		strncpy(data.caption, (this->charbase.type==1)?"王夫·捕头":"王后·捕头", sizeof(data.caption));
	}
	else if (this->isCatchexState() && this->kingConsort == 1)
	{
		strncpy(data.caption, (this->charbase.type==1)?"王夫·捕头":"王后·捕头", sizeof(data.caption));
	}
    else if (this->isCensorState() && this->kingConsort == 1)
	{
		strncpy(data.caption, (this->charbase.type==1)?"王夫·御史大夫":"王后·御史大夫", sizeof(data.caption));
	}
	else if (this->isCensosState() && this->kingConsort == 1)
	{
		strncpy(data.caption, (this->charbase.type==1)?"王夫·御史大夫":"王后·御史大夫", sizeof(data.caption));
	}
	else if (this->isZaixiangState() && this->kingConsort == 1)
	{
		strncpy(data.caption, (this->charbase.type==1)?"王夫·宰相":"王后·宰相", sizeof(data.caption));
	}
	else if (this->isYuanshuaiState() && this->kingConsort == 1)
	{
		strncpy(data.caption, (this->charbase.type==1)?"王夫·元帅":"王后·元帅", sizeof(data.caption));
	}

	else if (this->kingConsort == 2 && !this->isCatcherState() && !this->isCatchexState() && !this->isCensorState() && !this->isCensosState() && !this->isZaixiangState() && !this->isYuanshuaiState())
	{
		strncpy(data.caption, (this->charbase.type==1)?"皇夫":"皇后", sizeof(data.caption));
	}
	else if (this->isCatcherState() && this->kingConsort == 2)
	{
		strncpy(data.caption, (this->charbase.type==1)?"皇夫·捕头":"皇后·捕头", sizeof(data.caption));
	}
	else if (this->isCatchexState() && this->kingConsort == 2)
	{
		strncpy(data.caption, (this->charbase.type==1)?"皇夫·捕头":"皇后·捕头", sizeof(data.caption));
	}
    else if (this->isCensorState() && this->kingConsort == 2)
	{
		strncpy(data.caption, (this->charbase.type==1)?"皇夫·御史大夫":"皇后·御史大夫", sizeof(data.caption));
	}
	else if (this->isCensosState() && this->kingConsort == 2)
	{
		strncpy(data.caption, (this->charbase.type==1)?"皇夫·御史大夫":"皇后·御史大夫", sizeof(data.caption));
	}
	else if (this->isZaixiangState() && this->kingConsort == 2)
	{
		strncpy(data.caption, (this->charbase.type==1)?"皇夫·宰相":"皇后·宰相", sizeof(data.caption));
	}
	else if (this->isYuanshuaiState() && this->kingConsort == 2)
	{
		strncpy(data.caption, (this->charbase.type==1)?"皇夫·元帅":"皇后·元帅", sizeof(data.caption));
	}
	else if (this->isDiplomatState() == 0) //soke 外交官(停用)
	{
		strncpy(data.caption, "外交官", sizeof(data.caption));
	}
	else if (this->isCatcherState()) //soke 巡捕（左上）
	{
		strncpy(data.caption, "捕头", sizeof(data.caption));
	}
	else if (this->isCatchexState()) //soke 巡捕（左下）
	{
		strncpy(data.caption, "捕头", sizeof(data.caption));
	}
    else if (this->isCensorState()) //soke 御史（右上）
	{
		strncpy(data.caption, "御史大夫", sizeof(data.caption));
	}
	else if (this->isCensosState()) //soke 巡捕（右下）
	{
		strncpy(data.caption, "御史大夫", sizeof(data.caption));
	}
	else if (this->isYuanshuaiState())
	{
		strncpy(data.caption, "元帅", sizeof(data.caption));
	}
	else if (this->isZaixiangState())
	{
		strncpy(data.caption, "宰相", sizeof(data.caption));
	}
	//soke 更新 队长、将军 头顶显示称号
	else if (this->dwArmyState == 1)
	{
		strncpy(data.caption, "队长", sizeof(data.caption));
	}
		else if (this->dwArmyState == 2)
	{
		strncpy(data.caption, "将军", sizeof(data.caption));
	}

	else
	{
		strncpy(data.caption, this->caption, sizeof(data.caption));
	}

	if(this->team.getLeader() == this->tempid)
	{
		data.dwTeamState=Cmd::TEAM_STATE_LEADER;
	}
	else if(this->team.getLeader())
	{
		data.dwTeamState=Cmd::TEAD_STATE_MEMBER;
	}
	else
	{
		data.dwTeamState=Cmd::TEAD_STATE_NONE;
	}
}
/**
 * \brief 填充t_MapUserData命令
 *
 * \param data 待填充的命令
 */
void SceneUser::full_t_MapUserData(Cmd::t_MapUserData &data)
{
	bzero(&data,sizeof(data));
	full_t_UserData(*((Cmd::t_UserData *)&data));
	full_all_UState(data.state);
}
/**
 * \brief 填充t_MapUserDataPos命令
 *
 * \param data 待填充的命令
 */
void SceneUser::full_t_MapUserDataPos(Cmd::t_MapUserDataPos &data)
{
	full_t_MapUserData(*((Cmd::t_MapUserData *)&data));
	data.byDir=getDir();
	data.x=getPos().x;
	data.y=getPos().y;
}
/**
 * \brief 填充t_MapUserDataState命令
 *
 * \param data 待填充的命令
 */
void SceneUser::full_t_MapUserDataState(Cmd::t_MapUserDataState &data)
{
	bzero(&data,sizeof(data));
	full_t_UserData(*((Cmd::t_UserData*) &data));
	data.num=full_UState(data.state);
}
/**
 * \brief 填充t_MapUserDataPos命令
 *
 * \param data 待填充的命令
 */
void SceneUser::full_t_MapUserDataPosState(Cmd::t_MapUserDataPosState &data)
{
	full_t_UserData(*((Cmd::t_UserData *)&data));
	data.byDir=getDir();
	data.x=getPos().x;
	data.y=getPos().y;
	data.num=full_UState(data.state);
}


/**
 * \brief 
 *
 */
bool SceneUser::checkGoodnessTime(const zRTime &ct)
{
	if(ct >= pkState.lastCheckGoodness)
	{
		pkState.lastCheckGoodness.addDelay(ScenePkState::protectPeriod);
		return true;
	}
	else
	{
		return false;
	}
}


/**
 * \brief 重新计算箭侠箭灵的攻击模式
 *
 * \param calcflag 为TRUE通知客户端改变，为FALSE则不进行通知
 *
 * \return 如果模式改变返回TRUE,否则返回FALSE
 *
 */
/*
   bool SceneUser::recalcBySword(bool calcflag)
   {
   bool needRecalc = false;
   switch(charbase.type)
   {
   case PROFESSION_3://箭侠
   {
   if (packs.uom.exist(BOW_ARROW_ITEM_TYPE, 1) != farAttack)  // 没有箭的时候攻击模式由远程变近程，有箭的时候变化过程相反
   {
   needRecalc = true;
   }
   }
   break;
   case PROFESSION_4://箭灵
   {
   if (packs.uom.exist(661, 1) != farAttack)  // 没有箭的时候攻击模式由远程变近程，有箭的时候变化过程相反
   {
   needRecalc = true;
   }
   }
   break;
   default:
   break;
   }
   if(needRecalc && calcflag)
   {
   setupCharBase();
   Cmd::stMainUserDataUserCmd  userinfo;
   full_t_MainUserData(userinfo.data);
   sendCmdToMe(&userinfo,sizeof(userinfo));
// sendMeToNine(); 计算自己的攻击属性变化没必要广播九屏
}
return needRecalc;
}
*/

/**
 * \brief 增加道具数量
 *
 * \param id: 道具ID
 * \param num: 要增加的道具数量
 * \param upgrade: 升级
 * \param notify: 是否通知玩家
 * \param obj: 返回的物品
 *
 * \return 返回增加的数量
 *
 */
int SceneUser::addObjectNum(DWORD id, DWORD num, BYTE upgrade, int notify, bool bindit)
{
	zObject *orig_ob = NULL;
	UserObjectM::Obj_vec new_obs;

	int result = packs.uom.addObjectNum(this, id, num, orig_ob, new_obs, upgrade);
	if (result >= 0) 
	{		
		zObject* ob = orig_ob;
		if (orig_ob) 
		{
			Cmd::stRefCountObjectPropertyUserCmd ret;
			ret.qwThisID = orig_ob->data.qwThisID;
			ret.dwNum = orig_ob->data.dwNum;
			sendCmdToMe(&ret, sizeof(ret));
		}
		UserObjectM::Obj_vec::iterator it = new_obs.begin();
		while (it != new_obs.end()) 
		{
			ob = *it;
			Cmd::stAddObjectPropertyUserCmd ret;
			memcpy(&ret.object, &(*it)->data, sizeof(t_Object));
			sendCmdToMe(&ret,sizeof(ret));
			++it;
		}

		if (ob) 
		{
			OnGet event(ob->data.dwObjectID);
			EventTable::instance().execute(*this, event);
			if (ScriptQuest::get_instance().has(ScriptQuest::OBJ_GET, ob->data.dwObjectID)) 
			{ 
				char func_name[32];
				sprintf(func_name, "%s_%d", "get", ob->data.dwObjectID);
				execute_script_event(this,func_name, ob);
			}
			if (bindit) ob->data.bind=1;
		}

		if (notify && ob)
			Channel::sendSys(this, notify, "得到物品%s%d个", ob->data.strName, num);	
		if(num >= 999 )
		{
		    // Zebra::logger->debug("[非法物品数量警告]玩家 %s 获得物品:%s 瞬时获得数量:%d 个 \n疑似第三方工具刷取物品.当前获取路径:addObjectNum ",name,ob->data.strName,num);
		}	
	}

	return result;
}

/**
 * \brief 增加绑定道具数量
 *
 * \param id: 道具ID
 * \param num: 要增加的绑定道具数量
 * \param upgrade: 升级
 * \param notify: 是否通知玩家
 * \param obj: 返回的物品
 *
 * \return 返回增加的数量
 *
 */

int SceneUser::addBindObjectNum(DWORD id, DWORD num, BYTE upgrade, int notify, bool bindit)
{				
	zObjectB* oo = objectbm.get(id);
	int allnum = num;//全部数量
	
	if(oo)
	{
		while(allnum>0)
		{
			if(allnum>=oo->maxnum)
			{
				zObject *o=zObject::create(oo,oo->maxnum, upgrade);
				o->data.bind = 1;
				Combination2 callback(this, o);
				packs.main.execEvery(callback);
				if (packs.equip.pack(EquipPack::L_PACK) && packs.equip.pack(EquipPack::L_PACK)->can_input()) packs.equip.pack(EquipPack::L_PACK)->execEvery(callback);
				if (packs.equip.pack(EquipPack::R_PACK) && packs.equip.pack(EquipPack::R_PACK)->can_input()) packs.equip.pack(EquipPack::R_PACK)->execEvery(callback);
				if (o->data.dwNum) 
				{
					if (this->packs.uom.space(this) >= 1 && packs.addObject(o, true, AUTO_PACK)) 
					{
						//soke 特定类型需要绑定
						Cmd::stAddObjectPropertyUserCmd status;
						status.byActionType = Cmd::EQUIPACTION_OBTAIN;
						bcopy(&o->data, &status.object, sizeof(t_Object));
						sendCmdToMe(&status, sizeof(status));
					}
					else{
						return 1;
					}
				}						
				allnum-=oo->maxnum;				
			}
			else{
				zObject *o=zObject::create(oo,allnum, upgrade);
				o->data.bind = 1;
				Combination2 callback(this, o);
				packs.main.execEvery(callback);
				if (packs.equip.pack(EquipPack::L_PACK) && packs.equip.pack(EquipPack::L_PACK)->can_input()) packs.equip.pack(EquipPack::L_PACK)->execEvery(callback);
				if (packs.equip.pack(EquipPack::R_PACK) && packs.equip.pack(EquipPack::R_PACK)->can_input()) packs.equip.pack(EquipPack::R_PACK)->execEvery(callback);
				if (o->data.dwNum) 
				{
					if (this->packs.uom.space(this) >= 1 && packs.addObject(o, true, AUTO_PACK)) 
					{
						//soke 特定类型需要绑定
						Cmd::stAddObjectPropertyUserCmd status;
						status.byActionType = Cmd::EQUIPACTION_OBTAIN;
						bcopy(&o->data, &status.object, sizeof(t_Object));
						sendCmdToMe(&status, sizeof(status));
	
					}
					else{
						return 1;
					}
				}
				allnum=0;						
			}
		}
		return 2;
		
	}
	else{
		return 0;
	}



				
						
	// zObject *orig_ob = NULL;
	// UserObjectM::Obj_vec new_obs;

	// int result = packs.uom.addBindObjectNum(this, id, num, orig_ob, new_obs, upgrade);
	// if (result >= 0) 
	// {		
	// 	zObject* ob = orig_ob;
	// 	if (orig_ob) 
	// 	{
	// 		Cmd::stRefCountObjectPropertyUserCmd ret;
	// 		ret.qwThisID = orig_ob->data.qwThisID;
	// 		ret.dwNum = orig_ob->data.dwNum;
	// 		sendCmdToMe(&ret, sizeof(ret));
	// 	}
	// 	UserObjectM::Obj_vec::iterator it = new_obs.begin();
	// 	while (it != new_obs.end()) 
	// 	{
	// 		ob = *it;
	// 		Cmd::stAddObjectPropertyUserCmd ret;
	// 		memcpy(&ret.object, &(*it)->data, sizeof(t_Object));
	// 		sendCmdToMe(&ret,sizeof(ret));
	// 		++it;
	// 	}

	// 	if (ob) 
	// 	{
	// 		OnGet event(ob->data.dwObjectID);
	// 		EventTable::instance().execute(*this, event);
	// 		if (ScriptQuest::get_instance().has(ScriptQuest::OBJ_GET, ob->data.dwObjectID)) 
	// 		{ 
	// 			char func_name[32];
	// 			sprintf(func_name, "%s_%d", "get", ob->data.dwObjectID);
	// 			execute_script_event(this,func_name, ob);
	// 		}
	// 		if (bindit) 
	// 		ob->data.bind=1;
	// 	}

	// 	if (notify && ob)
	// 		Channel::sendSys(this, notify, "得到绑定物品%s%d个", ob->data.strName, num);	
	// 	if(num >= 999 )
	// 	{
	// 	    Zebra::logger->debug("[非法物品数量警告]玩家 %s 获得物品:%s 瞬时获得数量:%d 个 \n疑似第三方工具刷取物品.当前获取路径:addbindObjectNum ",name,ob->data.strName,num);
	// 	}	
	// }

	//return result;
}

/**
 * \brief 增加绑定等级道具数量
 *
 * \param id: 道具ID
 * \param num: 要增加的绑定等级道具数量
 * \param upgrade: 升级
 * \param notify: 是否通知玩家
 * \param obj: 返回的物品
 *
 * \return 返回增加的数量
 *
 */
int SceneUser::addGradeObjectNum(DWORD id, DWORD num, BYTE upgrade, int notify, bool bindit)
{
	zObject *orig_ob = NULL;
	UserObjectM::Obj_vec new_obs;

	int result = packs.uom.addBindObjectNum(this, id, num, orig_ob, new_obs, upgrade);
	if (result >= 0) 
	{		
		zObject* ob = orig_ob;
		if (orig_ob) 
		{
			Cmd::stRefCountObjectPropertyUserCmd ret;
			ret.qwThisID = orig_ob->data.qwThisID;
			ret.dwNum = orig_ob->data.dwNum;
			sendCmdToMe(&ret, sizeof(ret));
		}
		UserObjectM::Obj_vec::iterator it = new_obs.begin();
		while (it != new_obs.end()) 
		{
			ob = *it;
			Cmd::stAddObjectPropertyUserCmd ret;
			memcpy(&ret.object, &(*it)->data, sizeof(t_Object));
			sendCmdToMe(&ret,sizeof(ret));
			++it;
		}

		if (ob) 
		{
			OnGet event(ob->data.dwObjectID);
			EventTable::instance().execute(*this, event);
			if (ScriptQuest::get_instance().has(ScriptQuest::OBJ_GET, ob->data.dwObjectID)) 
			{ 
				char func_name[32];
				sprintf(func_name, "%s_%d", "get", ob->data.dwObjectID);
				execute_script_event(this,func_name, ob);
			}
			if (bindit) 
			ob->data.bind=1;
		}

		if (notify && ob)
			Channel::sendSys(this, notify, "得到绑定物品%s%d个", ob->data.strName, num);	

	}

	return result;
}

/**
* \brief 增加蓝色绑定道具数量
*
* \param id: 道具ID
* \param num: 要增加的道具数量
* \param upgrade: 升级
* \param maker: 是否增加签名
* \param notify: 是否通知玩家
* \param obj: 返回的物品
*
* \return 返回增加的数量
*
*/
int SceneUser::addBlueObjectNum(DWORD id, DWORD num, BYTE upgrade, BYTE maker,int notify, bool bindit)
{
	zObject *orig_ob = NULL;
	UserObjectM::Obj_vec new_obs;

	int result = packs.uom.addBlueObjectNum(this, id, num, orig_ob, new_obs, upgrade);
	if (result >= 0) {		
		zObject* ob = orig_ob;
		if (orig_ob)
		{
			Cmd::stRefCountObjectPropertyUserCmd ret;
			ret.qwThisID = orig_ob->data.qwThisID;
			ret.dwNum = orig_ob->data.dwNum;
			
			sendCmdToMe(&ret, sizeof(ret));
		}
		UserObjectM::Obj_vec::iterator it = new_obs.begin();
		while (it != new_obs.end()) 
		{
			ob = *it;
			if (maker == 1)
			{ 
				strncpy(ob->data.maker, this->name, MAX_NAMESIZE);//打造者名字  	
			} 	
			Cmd::stAddObjectPropertyUserCmd ret;
			memcpy(&ret.object, &(*it)->data, sizeof(t_Object));
			sendCmdToMe(&ret,sizeof(ret));
			++it;
		} 
		
		if (ob) 
		{
			OnGet event(ob->data.dwObjectID);
			EventTable::instance().execute(*this, event);
			if (ScriptQuest::get_instance().has(ScriptQuest::OBJ_GET, ob->data.dwObjectID)) 
			{ 
				char func_name[32];
				sprintf(func_name, "%s_%d", "get", ob->data.dwObjectID);
				execute_script_event(this,func_name, ob);
			}
			if (bindit) ob->data.bind=1;
  	
		}

		if (notify && ob)
			Channel::sendSys(this, notify, "得到物品%s%d个", ob->data.strName, num);			
	}

	return result;
}

/**
* \brief 增加黄色绑定道具数量
*
* \param id: 道具ID
* \param num: 要增加的道具数量
* \param upgrade: 升级
* \param notify: 是否通知玩家
* \param obj: 返回的物品
*
* \return 返回增加的数量
*
*/
int SceneUser::addGoldObjectNum(DWORD id, DWORD num, BYTE upgrade, int notify, bool bindit)
{
	zObject *orig_ob = NULL;
	UserObjectM::Obj_vec new_obs;

	int result = packs.uom.addGoldObjectNum(this, id, num, orig_ob, new_obs, upgrade);
	if (result >= 0) {		
		zObject* ob = orig_ob;
		if (orig_ob) {
			Cmd::stRefCountObjectPropertyUserCmd ret;
			ret.qwThisID = orig_ob->data.qwThisID;
			ret.dwNum = orig_ob->data.dwNum;
			sendCmdToMe(&ret, sizeof(ret));
		}
		UserObjectM::Obj_vec::iterator it = new_obs.begin();
		while (it != new_obs.end()) {
			ob = *it;
			Cmd::stAddObjectPropertyUserCmd ret;
			memcpy(&ret.object, &(*it)->data, sizeof(t_Object));
			sendCmdToMe(&ret,sizeof(ret));
			++it;
		}

		if (ob) 
		{
			OnGet event(ob->data.dwObjectID);
			EventTable::instance().execute(*this, event);
			if (ScriptQuest::get_instance().has(ScriptQuest::OBJ_GET, ob->data.dwObjectID)) 
			{ 
				char func_name[32];
				sprintf(func_name, "%s_%d", "get", ob->data.dwObjectID);
				execute_script_event(this,func_name, ob);
			}
			if (bindit) ob->data.bind=1;
		}

		if (notify && ob)
			Channel::sendSys(this, notify, "得到物品%s%d个", ob->data.strName, num);	

	}

	return result;
}

/**
 * \brief 增加绿色绑定道具数量
 *
 * \param id: 道具ID
 * \param num: 要增加的道具数量
 * \param upgrade: 升级
 * \param notify: 是否通知玩家
 * \param obj: 返回的物品
 *
 * \return 返回增加的数量
 *
 */
int SceneUser::addGreenObjectNum(DWORD id, DWORD num, BYTE upgrade, int notify, bool bindit)
{
	zObject *orig_ob = NULL;
	UserObjectM::Obj_vec new_obs;
	Zebra::logger->debug("增加道具");
	int result = packs.uom.addGreenObjectNum(this, id, num, orig_ob, new_obs, upgrade);
	if (result >= 0) {		
		zObject* ob = orig_ob;
		if (orig_ob) {
			Cmd::stRefCountObjectPropertyUserCmd ret;
			ret.qwThisID = orig_ob->data.qwThisID;
			ret.dwNum = orig_ob->data.dwNum;
			sendCmdToMe(&ret, sizeof(ret));
		}
		UserObjectM::Obj_vec::iterator it = new_obs.begin();
		while (it != new_obs.end()) {
			ob = *it;
			Cmd::stAddObjectPropertyUserCmd ret;
			memcpy(&ret.object, &(*it)->data, sizeof(t_Object));
			sendCmdToMe(&ret,sizeof(ret));
			++it;
		}

		if (ob) {
			OnGet event(ob->data.dwObjectID);
			EventTable::instance().execute(*this, event);
			if (ScriptQuest::get_instance().has(ScriptQuest::OBJ_GET, ob->data.dwObjectID)) { 
				char func_name[32];
				sprintf(func_name, "%s_%d", "get", ob->data.dwObjectID);
				execute_script_event(this,func_name, ob);
			}
			if (bindit) ob->data.bind=1;
		}

		if (notify && ob)
			Channel::sendSys(this, notify, "得到物品%s%d个", ob->data.strName, num);	

	}

	return result;
}

//soke 低级装备换高级装备 得到相应等级的装备ID
int SceneUser::GetEquipmentID(int level,int face)
{
	std::vector<int> Equipmentlist;
	if (level<=10&&level>0)
	{
		if (face==1)//男
		{
			Equipmentlist.push_back(10051);
			Equipmentlist.push_back(10101);
			Equipmentlist.push_back(10052);
			Equipmentlist.push_back(10102);
		}
		if (face==2)//女
		{
			Equipmentlist.push_back(10201);
			Equipmentlist.push_back(10251);
			Equipmentlist.push_back(10202);
			Equipmentlist.push_back(10252);
		}
		Equipmentlist.push_back(10301);
		Equipmentlist.push_back(10351);
		Equipmentlist.push_back(10501);
		Equipmentlist.push_back(10551);
		Equipmentlist.push_back(10751);
		Equipmentlist.push_back(10752);
		Equipmentlist.push_back(10801);
		Equipmentlist.push_back(10802);
		Equipmentlist.push_back(10851);
		Equipmentlist.push_back(10852);
		Equipmentlist.push_back(10901);
		Equipmentlist.push_back(10902);
		Equipmentlist.push_back(10951);
		Equipmentlist.push_back(10952);
		Equipmentlist.push_back(11001);
		Equipmentlist.push_back(11002);
		Equipmentlist.push_back(10701);
		Equipmentlist.push_back(11051);
		Equipmentlist.push_back(11071);
		Equipmentlist.push_back(11091);
		Equipmentlist.push_back(10302);
		Equipmentlist.push_back(10352);
		Equipmentlist.push_back(10502);
		Equipmentlist.push_back(10552);
		Equipmentlist.push_back(10753);
		Equipmentlist.push_back(10754);
		Equipmentlist.push_back(10803);
		Equipmentlist.push_back(10804);
		Equipmentlist.push_back(10853);
		Equipmentlist.push_back(10854);
		Equipmentlist.push_back(10903);
		Equipmentlist.push_back(10904);
		Equipmentlist.push_back(10953);
		Equipmentlist.push_back(10954);
		Equipmentlist.push_back(11003);
		Equipmentlist.push_back(11004);
		Equipmentlist.push_back(10702);
		Equipmentlist.push_back(11052);
		Equipmentlist.push_back(11072);
		Equipmentlist.push_back(11092);

		int n  = zMisc::randBetween(0, Equipmentlist.size()-1);
		return Equipmentlist[n];
	}


	if (level<=20&&level>10)
	{
		if (face==1)//男
		{
			Equipmentlist.push_back(10053);
			Equipmentlist.push_back(10103);
		}
		if (face==2)//女
		{
			Equipmentlist.push_back(10203);
			Equipmentlist.push_back(10253);
		}
		Equipmentlist.push_back(10303);
		Equipmentlist.push_back(10353);
		Equipmentlist.push_back(10503);
		Equipmentlist.push_back(10553);
		Equipmentlist.push_back(10755);
		Equipmentlist.push_back(10756);
		Equipmentlist.push_back(10805);
		Equipmentlist.push_back(10806);
		Equipmentlist.push_back(10855);
		Equipmentlist.push_back(10856);
		Equipmentlist.push_back(10905);
		Equipmentlist.push_back(10906);
		Equipmentlist.push_back(10955);
		Equipmentlist.push_back(10956);
		Equipmentlist.push_back(11005);
		Equipmentlist.push_back(11006);
		Equipmentlist.push_back(10703);
		Equipmentlist.push_back(11053);
		Equipmentlist.push_back(11073);
		Equipmentlist.push_back(11093);

		int n  = zMisc::randBetween(0, Equipmentlist.size()-1);
		return Equipmentlist[n];
	}


	if (level<=30&&level>20)
	{
		if (face==1)//男
		{
			Equipmentlist.push_back(10054);
			Equipmentlist.push_back(10104);
		}
		if (face==2)//女
		{
			Equipmentlist.push_back(10204);
			Equipmentlist.push_back(10254);
		}
		Equipmentlist.push_back(10304);
		Equipmentlist.push_back(10354);
		Equipmentlist.push_back(10504);
		Equipmentlist.push_back(10554);
		Equipmentlist.push_back(10757);
		Equipmentlist.push_back(10758);
		Equipmentlist.push_back(10807);
		Equipmentlist.push_back(10808);
		Equipmentlist.push_back(10857);
		Equipmentlist.push_back(10858);
		Equipmentlist.push_back(10907);
		Equipmentlist.push_back(10908);
		Equipmentlist.push_back(10957);
		Equipmentlist.push_back(10958);
		Equipmentlist.push_back(11007);
		Equipmentlist.push_back(11008);
		Equipmentlist.push_back(10704);
		Equipmentlist.push_back(11054);
		Equipmentlist.push_back(11074);
		Equipmentlist.push_back(11094);

		int n  = zMisc::randBetween(0, Equipmentlist.size()-1);
		return Equipmentlist[n];
	}



	if (level<=40&&level>30)
	{
		if (face==1)//男
		{
			Equipmentlist.push_back(10055);
			Equipmentlist.push_back(10105);
		}
		if (face==2)//女
		{
			Equipmentlist.push_back(10205);
			Equipmentlist.push_back(10255);
		}
		Equipmentlist.push_back(10305);
		Equipmentlist.push_back(10355);
		Equipmentlist.push_back(10505);
		Equipmentlist.push_back(10555);
		Equipmentlist.push_back(10759);
		Equipmentlist.push_back(10760);
		Equipmentlist.push_back(10809);
		Equipmentlist.push_back(10810);
		Equipmentlist.push_back(10859);
		Equipmentlist.push_back(10860);
		Equipmentlist.push_back(10909);
		Equipmentlist.push_back(10910);
		Equipmentlist.push_back(10959);
		Equipmentlist.push_back(10960);
		Equipmentlist.push_back(11009);
		Equipmentlist.push_back(11010);
		Equipmentlist.push_back(10705);
		Equipmentlist.push_back(11055);
		Equipmentlist.push_back(11075);
		Equipmentlist.push_back(11095);

		int n  = zMisc::randBetween(0, Equipmentlist.size()-1);
		return Equipmentlist[n];
	}


	if (level<=50&&level>40)
	{
		if (face==1)//男
		{
			Equipmentlist.push_back(10056);
			Equipmentlist.push_back(10106);
		}
		if (face==2)//女
		{
			Equipmentlist.push_back(10206);
			Equipmentlist.push_back(10256);
		}
		Equipmentlist.push_back(10306);
		Equipmentlist.push_back(10356);
		Equipmentlist.push_back(10506);
		Equipmentlist.push_back(10556);
		Equipmentlist.push_back(10761);
		Equipmentlist.push_back(10762);
		Equipmentlist.push_back(10811);
		Equipmentlist.push_back(10812);
		Equipmentlist.push_back(10861);
		Equipmentlist.push_back(10862);
		Equipmentlist.push_back(10911);
		Equipmentlist.push_back(10912);
		Equipmentlist.push_back(10961);
		Equipmentlist.push_back(10962);
		Equipmentlist.push_back(11011);
		Equipmentlist.push_back(11012);
		Equipmentlist.push_back(10706);
		Equipmentlist.push_back(11056);
		Equipmentlist.push_back(11076);
		Equipmentlist.push_back(11096);

		int n  = zMisc::randBetween(0, Equipmentlist.size()-1);
		return Equipmentlist[n];
	}


	if (level<=60&&level>50)
	{
		if (face==1)//男
		{
			Equipmentlist.push_back(10057);
			Equipmentlist.push_back(10107);
		}
		if (face==2)//女
		{
			Equipmentlist.push_back(10207);
			Equipmentlist.push_back(10257);
		}
		Equipmentlist.push_back(10307);
		Equipmentlist.push_back(10357);
		Equipmentlist.push_back(10507);
		Equipmentlist.push_back(10557);
		Equipmentlist.push_back(10763);
		Equipmentlist.push_back(10764);
		Equipmentlist.push_back(10813);
		Equipmentlist.push_back(10814);
		Equipmentlist.push_back(10863);
		Equipmentlist.push_back(10864);
		Equipmentlist.push_back(10913);
		Equipmentlist.push_back(10914);
		Equipmentlist.push_back(10963);
		Equipmentlist.push_back(10964);
		Equipmentlist.push_back(11013);
		Equipmentlist.push_back(11014);
		Equipmentlist.push_back(10707);
		Equipmentlist.push_back(11057);
		Equipmentlist.push_back(11077);
		Equipmentlist.push_back(11097);

		int n  = zMisc::randBetween(0, Equipmentlist.size()-1);
		return Equipmentlist[n];
	}
	return 0;
}

/**
* \brief 增加紫色绑定道具数量
*
* \param id: 道具ID
* \param num: 要增加的道具数量
* \param upgrade: 升级
* \param notify: 是否通知玩家
* \param obj: 返回的物品
*
* \return 返回增加的数量
*
*/
int SceneUser::addPurpleObjectNum(DWORD id, DWORD num, BYTE upgrade, int notify, bool bindit)
{
	zObject *orig_ob = NULL;
	UserObjectM::Obj_vec new_obs;

	int result = packs.uom.addPurpleObjectNum(this, id, num, orig_ob, new_obs, upgrade);
	if (result >= 0) {		
		zObject* ob = orig_ob;
		if (orig_ob) {
			Cmd::stRefCountObjectPropertyUserCmd ret;
			ret.qwThisID = orig_ob->data.qwThisID;
			ret.dwNum = orig_ob->data.dwNum;
			sendCmdToMe(&ret, sizeof(ret));
		}
		UserObjectM::Obj_vec::iterator it = new_obs.begin();
		while (it != new_obs.end()) {
			ob = *it;
			Cmd::stAddObjectPropertyUserCmd ret;
			memcpy(&ret.object, &(*it)->data, sizeof(t_Object));
			sendCmdToMe(&ret,sizeof(ret));
			++it;
		}

		if (ob) 
		{
			OnGet event(ob->data.dwObjectID);
			EventTable::instance().execute(*this, event);
			if (ScriptQuest::get_instance().has(ScriptQuest::OBJ_GET, ob->data.dwObjectID)) 
			{ 
				char func_name[32];
				sprintf(func_name, "%s_%d", "get", ob->data.dwObjectID);
				execute_script_event(this,func_name, ob);
			}
			if (bindit) ob->data.bind=1;
		}

		if (notify && ob)
			Channel::sendSys(this, notify, "得到物品%s%d个", ob->data.strName, num);	

	}

	return result;
}
/**
 * \brief 减少道具数量
 *
 * \param id: 道具ID
 * \param num: 要减少的道具数量
 * \param upgrade: 升级
 * \param notify: 是否通知玩家
 *
 * \return 返回减少的数量
 *
 */
int SceneUser::reduceObjectNum(DWORD id, DWORD num, BYTE upgrade)
{
	zObject *update_ob = NULL;
	UserObjectM::ObjID_vec del_obs;

	int result = packs.uom.reduceObjectNum(this, id, num, update_ob, del_obs, upgrade);
	if (update_ob) {
		Cmd::stRefCountObjectPropertyUserCmd ret;
		ret.qwThisID = update_ob->data.qwThisID;
		ret.dwNum = update_ob->data.dwNum;
		sendCmdToMe(&ret, sizeof(ret));
	}
	UserObjectM::ObjID_vec::iterator it = del_obs.begin();
	while (it != del_obs.end()) {
		Cmd::stRemoveObjectPropertyUserCmd rm;
		rm.qwThisID = *it;
		sendCmdToMe(&rm, sizeof(rm));
		++it;
	}

	return result;
}
/**
 * \brief 计算友好度
 *
 *
 */
void SceneUser::countFriendDegree()
{
	if (scene->getZoneType(pos) == ZoneTypeDef::ZONE_NONE) team.countFriendDegree();
}

/**
 * \brief 将命令转发到Session服务器
 *
 * \param ptNullCmd 待转发的命令
 * \param nCmdLen 命令长度
 */
bool SceneUser::forwardSession(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen)
{
	if(nCmdLen > zSocket::MAX_USERDATASIZE)
	{
		Zebra::logger->debug("消息越界(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
	}
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::Session::t_Session_ForwardUser *sendCmd=(Cmd::Session::t_Session_ForwardUser *)buf;
	constructInPlace(sendCmd);
	sendCmd->dwID=id;
	sendCmd->size=nCmdLen;
	bcopy(ptNullCmd,sendCmd->data,nCmdLen);
	return sessionClient->sendCmd(buf,sizeof(Cmd::Session::t_Session_ForwardUser)+nCmdLen);
}

/**
 * \brief 得到角色的魔法攻击类型(前4种职业属物理技能,后四种属于魔法技能)
 * 物理技能就是使用对方的物理防御,魔法技能就是指使用对方的法术防御
 * \return 物理技能为true 还是法术技能false
 */
bool SceneUser::getMagicType()
{
	switch(charbase.type)
	{
		case PROFESSION_1:
		case PROFESSION_2:
		case PROFESSION_3:
		case PROFESSION_4:
			{
				return true;
			}
			break;
		case PROFESSION_5:
		case PROFESSION_6:
		case PROFESSION_7:
		case PROFESSION_8:
			{
				return false;
			}
			break;
	}
	return false;
}

bool SceneUser::isAllied(SceneUser *pUser)
{
	//TODO 具体完成
	return this->charbase.country == pUser->charbase.country;
}

/**
 * \brief 判断是否是敌人
 * \author fqnewman
 * \return 1 是 0 不是
 */
int SceneUser::isEnemy(SceneEntryPk * entry, bool notify, bool good)
{
	// TODO 判断传入角色与本身是否为朋友关系
	if (this==entry) return 0;

	using namespace Cmd;
	//if (PKMODE_ENTIRE==pkMode) return 1;

	switch (entry->getType())
	{
		case zSceneEntry::SceneEntry_Player:
			{
				if (this->isDiplomatState() == 0) return 0;
				SceneUser *pUser = (SceneUser *)entry;
				bool def_gem = false;
				bool my_gem = false;

				if (this->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER)
						|| this->issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
				{
					my_gem = true;
				}

				if (pUser->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER)
						|| pUser->issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
				{
					def_gem = true;
				}

				if (pUser == this) return 0;

				if (((pUser->charbase.level<20) && (!pUser->isWarRecord(Cmd::COUNTRY_FORMAL_DARE, this->charbase.country))) && !def_gem)
				{
					//if (notify) Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"不能攻击低于20级的玩家!");
					return 0;
				}

				if (((this->charbase.level<20) && (!this->isWarRecord(Cmd::COUNTRY_FORMAL_DARE, pUser->charbase.country))) && !my_gem)
				{
					//if (notify) Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"你等级低于20级不能进行PK!");
					return 0;
				}

				//自行车判定
				if (pUser->issetUState(333))
				{
					Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"不能攻击正在参加自行车比赛的玩家!");
					return 0;
				}

				//战车判定
				if (pUser->issetUState(1410))
				{
					Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"不能攻击战车");
					return 0;
				}

				//跳舞状态检测
				if(pUser->scene->getRealMapID() == 139 )
				{
					bool isInSpaZone = scene->checkZoneType(pos, ZoneTypeDef::ZONE_PRIVATE_STORE);
					if ( pUser->isDance() )
					{
						if ( isInSpaZone )
						{
							if (notify) 
								Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"在广场跳舞的目标不可攻击!");
							return 0;
						}
					}
				}
				if(!(my_gem || def_gem))
				{
					    //soke 挖宝区挖宝不可被攻击
					    if (pUser->issetUState(Cmd::USTATE_USER_WABAO ))
	                    {
                           bool isInDanceZone = scene->checkZoneType(pos, ZoneTypeDef::ZONE_DANCE);
                           if ( isInDanceZone )
                           {
							   if(notify)
							   Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"在挖宝区挖宝的目标不可攻击");
							
			                   return 0;
	                       }
	                    }
						if(this->charbase.level <= this->scene->getPkLevel() && pUser->charbase.level >this->scene->getPkLevel() 
								&& (!this->isWarRecord(Cmd::SEPT_NPC_DARE, pUser->charbase.septid)))
						{
							if(notify) Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"你不能攻击%d以上的玩家",this->scene->getPkLevel());
							return 0;
						}

						if(this->charbase.level > this->scene->getPkLevel() && pUser->charbase.level <= this->scene->getPkLevel() 
								&& (!this->isWarRecord(Cmd::SEPT_NPC_DARE, pUser->charbase.septid)))
						{
							if(notify) Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"你不能攻击%d级以下的玩家",this->scene->getPkLevel());
							return 0;
						}
				}

				switch(pkMode)
				{
					case PKMODE_NORMAL:
						{
							return 0;
						}
						break;
					case PKMODE_TEAM:
						{
							//是同一组队或者是增益类魔法
							if((this->team.getLeader() != 0) && (pUser->team.getLeader() == this->team.getLeader()))
								//&& tempid != rev->dwDefenceTempID)
								return 0;
						}
						break;
					case PKMODE_TONG:
						{
							if(charbase.unionid != 0 && charbase.unionid == pUser->charbase.unionid)
							{
								return 0;
							}
						}
						break;
					case PKMODE_SEPT:
						{
							if(charbase.septid != 0 && charbase.septid == pUser->charbase.septid)
							{
								return 0;
							}
						}
						break;
						/*case PKMODE_SCHOOL:
						  {
						  if(charbase.schoolid != 0 && charbase.schoolid == pUser->charbase.schoolid)
						  {
						  return 0;
						  }
						  }
						  break;*/
					case PKMODE_COUNTRY:
						{
							if (charbase.country != 0 
							   && ((charbase.country == pUser->charbase.country) ||
					      (CountryAllyM::getMe().getFriendLevel(charbase.country, pUser->charbase.country)>0
				       && (pUser->isSpecWar(Cmd::COUNTRY_FORMAL_DARE) || this->isSpecWar(Cmd::COUNTRY_FORMAL_DARE)))))
							{
								return 0;
							}
						}
						break;
					case PKMODE_GOODNESS:
						{
							if(!pUser->isRedNamed(false)&&pUser->charbase.country==this->charbase.country)
							{
									return 0;
							}
						}
						break;
					case PKMODE_ALLY:
						{
							if ((CountryAllyM::getMe().getFriendLevel(pUser->charbase.country, this->charbase.country)>0) ||
								(pUser->charbase.country == this->charbase.country))
							{
								return 0;
							}
						}
						break;
						//case PKMODE_CHALLENGE:
					case PKMODE_ENTIRE:
					default:
						break;
				}
				return 1;
			}
			break;
		case zSceneEntry::SceneEntry_NPC:
			{
				SceneNpc * n = (SceneNpc *)entry;
				SceneEntryPk * m = n->getMaster();

				if (n->id==COUNTRY_MAIN_FLAG	//这几个不在这里判断
						|| n->id==COUNTRY_SEC_FLAG
						|| n->isMainGeneral()
						|| n->id==COUNTRY_KING_MAIN_FLAG
						|| n->id==COUNTRY_KING_SEC_FLAG
						|| n->id==COUNTRY_SEC_GEN
						|| n->id==COUNTRY_EMPEROR_MAIN_GEN
						|| n->id==COUNTRY_EMPEROR_SEC_GEN)
				{
					if (this->isDiplomatState() == 0)
						return 0;
					return 1;
				}

				if (!n->isBugbear())
				{
					if (n->npc->flags==1 && charbase.country!=n->scene->getCountryID())
						return 1;
					else
						return 0;
				}
				if (m)
				{
					if (m->getType() == zSceneEntry::SceneEntry_Player)
					{
						SceneUser *pUser = (SceneUser *)m;
						if (pUser)
						{
							bool def_gem = false;
							bool my_gem = false;

							if (this->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER)
									|| this->issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
							{
								my_gem = true;
							}

							if (pUser->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER)
									|| pUser->issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
							{
								def_gem = true;
							}

							if (pUser == this) return 0;
							//soke 不是国战时间
							if (((pUser->charbase.level<20)  && (!pUser->isWarRecord(Cmd::COUNTRY_FORMAL_DARE, this->charbase.country))) && !def_gem)
							{
								//if (notify) Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"不能攻击低于20级的玩家!");
								return 0;
							}
							if ((this->charbase.level<20) && (!this->isWarRecord(Cmd::COUNTRY_FORMAL_DARE, pUser->charbase.country)) && !my_gem)
							{
								//if (notify) Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"你等级低于10级不能进行PK!");
								return 0;
							}
							if(this->scene == pUser->scene && !(my_gem || def_gem))
							{
								//if (this->charbase.country == pUser->charbase.country)
								//{
									if(this->charbase.level <= this->scene->getPkLevel() && pUser->charbase.level >this->scene->getPkLevel())
									{
										return 0;
									}
									if(this->charbase.level > this->scene->getPkLevel() && pUser->charbase.level <= this->scene->getPkLevel())
									{
										return 0;
									}
							}
						}
					}

					if (n->getPetType()==Cmd::PET_TYPE_GUARDNPC
							&& !scene->zPosShortRange(n->getPos(), m->getPos(), 20)
							&& pkMode==PKMODE_ENTIRE)
						return 1;
					else
					{
						if (good && this == m && n->getPetType() == Cmd::PET_TYPE_GUARDNPC) return 1;
						return isEnemy(m);
					}
				}

				if ((n->aif&AIF_ATK_REDNAME)||(n->npc->kind==NPC_TYPE_GUARD))
				{
					if (isRedNamed()) return 1;
					if (charbase.country!=scene->getCountryID())
						return 1;
					if (charbase.goodness&Cmd::GOODNESS_ATT)
						return 1;
				}
				switch (n->npc->kind)
				{
					case NPC_TYPE_HUMAN:                    ///人型
					case NPC_TYPE_NORMAL:                   /// 普通类型
					case NPC_TYPE_BBOSS:                    /// 大Boss类型
					case NPC_TYPE_LBOSS:                    /// 小Boss类型
					case NPC_TYPE_PBOSS:                    /// 紫Boss类型
					case NPC_TYPE_REDBOSS:			/// 红Boss类型
					case NPC_TYPE_BACKBONE:                 /// 精英类型
					case NPC_TYPE_GOLD:                             /// 黄金类型
					case NPC_TYPE_SUMMONS:                  /// 召唤类型
					case NPC_TYPE_AGGRANDIZEMENT:   /// 强化类型
					case NPC_TYPE_ABERRANCE:                /// 变异类型
					case NPC_TYPE_BACKBONEBUG:              /// 精怪类型
					case NPC_TYPE_PET:      /// 宠物类型
					case NPC_TYPE_TOTEM:                    /// 图腾类型
					case NPC_TYPE_DUCKHIT:    /// 花草
					case NPC_TYPE_RESOURCE:  /// 资源类NPC
						return 1;
					case NPC_TYPE_GUARD:    /// 士兵类型
					case NPC_TYPE_SOLDIER:    /// 士兵类型
						{
							if (charbase.country!=scene->getCountryID())
								return 1;
							if (pkMode==PKMODE_ENTIRE)
								return 1;
							return 0;
						}
					case NPC_TYPE_UNIONGUARD:
						if (isAtt(Cmd::UNION_CITY_DARE))
							return 1;
						else
							if (scene->getUnionDare() && !isSpecWar(Cmd::UNION_CITY_DARE)
									&& !n->isMainGeneral())//大将军第三方不能打
								return 1;//中立方
							else
								return 0;//城战期间打城战而且不是攻方，就是守方
						break;
					case NPC_TYPE_UNIONATTACKER:
						if (isAtt(Cmd::UNION_CITY_DARE))
							return 0;
						else
							if (scene->getUnionDare() && !isSpecWar(Cmd::UNION_CITY_DARE))
								return 1;//中立方
							else
								return 1;//城战期间打城战而且不是攻方，就是守方
						break;
					default:
						return 0;
				}
			}
			break;
		default:
			return 0;
			break;
	}
}

/**
 * \brief 修改HP
 *
 */
void SceneUser::changeHP(const SDWORD &hp)
{
	SDWORD changeValue = 0;
	if (((int)charbase.hp)+(int)hp>=0)
	{
		changeValue = charbase.hp;
		charbase.hp += hp;
		if (charbase.hp > charstate.maxhp) charbase.hp = charstate.maxhp;
		changeValue = (int)charbase.hp-changeValue;
	}
	else
	{
		changeValue = charbase.hp;
		charbase.hp=0;
	}
	notifyHMS = true;

	//soke 检查是否有自动补血道具
	checkAutoHP();

	if (changeValue !=0)
	{
		Cmd::stObjectHpMpPopUserCmd ret;
		ret.dwUserTempID = this->tempid;
		ret.byTarget = Cmd::MAPDATATYPE_USER;
		ret.vChange = (int)changeValue;
		ret.type = Cmd::POP_HP;
		this->scene->sendCmdToNine(getPosI(), &ret , sizeof(ret));
	}
}

/**
 * \brief 直接伤害
 *
 */
//soke 突破伤害值限制（攻击不显示掉血）
SQWORD SceneUser::directDamage(SceneEntryPk *pAtt, const SDWORD &hp, bool notify)
{
	SceneEntryPk::directDamage(pAtt, hp, notify);

	SQWORD reduceHP;
	if ((SQWORD)charbase.hp-hp>=0)
	{
		charbase.hp -= hp;
		reduceHP = hp;
	}
	else
	{
		reduceHP = charbase.hp;
		charbase.hp=0;
	}

	if (reduceHP !=0 && notify)
	{
		Cmd::stObjectHpMpPopUserCmd ret;
		ret.dwUserTempID = this->tempid;
		ret.byTarget = Cmd::MAPDATATYPE_USER;
		ret.vChange = 0-(int)reduceHP;
		ret.type = Cmd::POP_HP;
		this->scene->sendCmdToNine(getPosI(), &ret , sizeof(ret));
	}

	notifyHMS = true;
	return reduceHP;
}

/**
 * \brief 修改MP
 *
 */
void SceneUser::changeMP(const SDWORD &mp)
{
	SDWORD changeValue = 0;
	if (((int)charbase.mp)+(int)mp>=0)
	{
		changeValue = charbase.mp;
		charbase.mp += mp;
		if (charbase.mp > charstate.maxmp) charbase.mp = charstate.maxmp;
		changeValue = (int)charbase.mp-changeValue;
	}
	else
	{
		changeValue = charbase.mp;
		charbase.mp=0;
	}
	notifyHMS = true;

	//检查是否有自动补魔道具
	checkAutoMP();
	
	if (changeValue !=0)
	{
		Cmd::stObjectHpMpPopUserCmd ret;
		ret.dwUserTempID = this->tempid;
		ret.byTarget = Cmd::MAPDATATYPE_USER;
		ret.vChange = (int)changeValue;
		ret.type = Cmd::POP_MP;
		this->scene->sendCmdToNine(getPosI(), &ret , sizeof(ret));
	}
}

/**
 * \brief 修改SP
 *
 */
void SceneUser::changeSP(const SDWORD &sp)
{
	if (charbase.sp+sp>=0)
	{
		charbase.sp += sp;
		if (charbase.sp > charstate.maxsp) charbase.sp = charstate.maxsp;
	}
	else
	{
		charbase.sp=0;
	}
	notifyHMS = true;
}

/**
 * \brief 获得最大的hp
 * \author fqnewman
 * \return 返回最大值
 */
DWORD SceneUser::getMaxHP()
{
	return charstate.maxhp;
}

/**
 * \brief 获得最大的hp
 * \author fqnewman
 * \return 返回最大值
 */
DWORD SceneUser::getBaseMaxHP()
{
	return charstate.maxhp;
}

/**
 * \brief 获得最大的mp
 * \author fqnewman
 * \return 返回最大值
 */
DWORD SceneUser::getMaxMP()
{
	return charstate.maxmp;
}

/**
 * \brief 获得最大的mp
 * \author fqnewman
 * \return 返回最大值
 */
DWORD SceneUser::getBaseMaxMP()
{
	return charstate.maxmp;
}

/**
 * \brief 一次攻击的预处理
 *
 */
bool SceneUser::preAttackMe(SceneEntryPk *pUser, const Cmd::stAttackMagicUserCmd *rev, bool physics, const bool good)
{
	if (this->issetUState(Cmd::USTATE_PRIVATE_STORE))
	{
		ScenePk::attackFailToMe(rev , this);
		return false;
	}

	if ((pUser!=this)&&isSitdown())
	{
		standup();
	}

	if (pUser->getType() == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pDef = (SceneUser *)pUser;
		
		if (this->npcdareflag != pDef->npcdareflag)
		{
			if (rev) ScenePk::attackFailToMe(rev, pUser, true);
			if (this->npcdareflag)
			{
				Channel::sendSys(pDef,Cmd::INFO_TYPE_GAME, "对方处于家族战状态!");
			}
			else
			{
				Channel::sendSys(pDef,Cmd::INFO_TYPE_GAME, "你处于家族战状态!");
			}
			return false;
		}

		if (this->isDiplomatState() == 0 || pDef->isDiplomatState() == 0)
		{
			if (rev) ScenePk::attackFailToMe(rev, pUser, true);
			return false;
		}
	}

	if (this->getState() == zSceneEntry::SceneEntry_Death && 
			rev->wdMagicType != 295 //复活技能
	   )  
	{
		if (rev) ScenePk::attackFailToMe(rev, pUser, true);
		return false;
	}

if (!good)
{
	int pkAttackDodge = (int)( (int)skillValue.akdodge + (int)skillValue.dodge - (int)((int)skillValue.reduce_akdodge));

	if (pkAttackDodge<0)
	{
		pkAttackDodge = 1;
	}
	else if(pkAttackDodge>99)
	{
		pkAttackDodge = 99;
	}

	if(zMisc::selectByPercent(pkAttackDodge))
	{
		if (rev) ScenePk::attackFailToMe(rev, pUser, false);
		return false;
	}
}
//初始化pk数据
this->pkValue.init();
this->skillValue.init();



this->skillStatusM.processPassiveness();	// 处理我的被动状态影响


if (pUser->getType() == zSceneEntry::SceneEntry_Player)
{
	SceneUser *pAtt = (SceneUser *)pUser;
	ScenePk::calpdamU2U(rev , pAtt , this);
}
else if (pUser->getType() == zSceneEntry::SceneEntry_NPC)
{
	SceneNpc *pAtt = (SceneNpc *)pUser;
	ScenePk::calpdamN2U(rev, pAtt , this);
}
if (pUser->getType() == zSceneEntry::SceneEntry_Player)
{
	SceneUser *pAtt = (SceneUser *)pUser;
	ScenePk::calmdamU2U(rev , pAtt , this);
}
else if (pUser->getType() == zSceneEntry::SceneEntry_NPC)
{
	SceneNpc *pAtt = (SceneNpc *)pUser;
	ScenePk::calmdamN2U(rev, pAtt , this);
}

// 修改耐久度只在被击成功后才扣除耐久
packs.equip.costDefenceDur(this);
return true;
}

/**
 * \brief 得到善恶度的名称
 *
 *  \return 返回当前善恶度数值相对称的名称。如果当前数值无效，返回长度为0的一个字符串
 *
 */
char *SceneUser::getGoodnessName()
{
	switch(getGoodnessState())
	{
		case Cmd::GOODNESS_0:
			{
				return "英雄";
			}
			break;
		case Cmd::GOODNESS_1:
			{
				return "侠士";
			}
			break;
		case Cmd::GOODNESS_2_1:
			{
				return "普通人";
			}
			break;
		case Cmd::GOODNESS_3:
			{
				return "歹徒";
			}
			break;
		case Cmd::GOODNESS_4:
			{
				return "恶徒";
			}
			break;
		case Cmd::GOODNESS_5:
			{
				return "恶魔";
			}
			break;
		case Cmd::GOODNESS_6:
			{
				return "魔头";
			}
			break;
		default:
			{
				return "";
			}
			break;
	}
}

bool SceneUser::isPkAddition()
{
	return charbase.pkaddition > 1800;
}
DWORD SceneUser::getPkAddition()
{
	if(charbase.pkaddition > 1800)
	{
		DWORD ret = (DWORD)((charbase.pkaddition - 1800)/12 + 60);
		return ret>240?240:ret;
	}
	return 0;
}
/**
 * \brief 根据善恶值，计算善恶等级
 *
 *
 * \return 返回善恶等级
 */
short SceneUser::getGoodnessState() const 
{
	if(charbase.goodness & 0x0000FFFF == Cmd::GOODNESS_7)
	{
		return Cmd::GOODNESS_7;
	}
	short good = 0x0000FFFF & charbase.goodness;
	if(good < -60)
	{
		good = Cmd::GOODNESS_0;
	}
	else if(good < 0 && good >= -60)
	{
		good = Cmd::GOODNESS_1;
	}
	else if(good == 0)
	{
		good = Cmd::GOODNESS_2_1;
	}
	else if(good > 0 && good <= 60)
	{
		good = Cmd::GOODNESS_3;
	}
	else if(good > 60 && good <= 120)
	{
		good = Cmd::GOODNESS_4;
	}
	else if(good > 120 && good <= 180)
	{
		good = Cmd::GOODNESS_5;
	}
	else if(good > 180 && good < 10000)
	{
		good = Cmd::GOODNESS_6;
	}
	return good;
}

/**
 * \brief 判断是否红名
 *
 * 
 * \return 如果是红名返回TRUE,否则返回FALSE
 */
bool SceneUser::isRedNamed(bool allRedMode) const
{
	if (allRedMode)
	{
		if (mask.is_masking()) return false;
		if (((charbase.goodness&0x0000ffff)>0)
				&& ((charbase.goodness&0x0000ffff)<=MAX_GOODNESS))
			return true;
	}
	else
	{
		if (((charbase.goodness&0x0000ffff)>=Cmd::GOODNESS_3)
				&& ((charbase.goodness&0x0000ffff)<=MAX_GOODNESS))
			return true;
	}
	//if ((this->getGoodnessState()>=60)&&(this->getGoodnessState()<=(short)MAX_GOODNESS))
	//	return true;
	if (charbase.goodness&Cmd::GOODNESS_ATT)
		return true;
	return false;
}


/**
 * \brief 根据善恶度计算物品价格
 *
 * \param price 物品原价格
 * \param isBuy 
 *
 * \return 真实交易的价格
 */
float SceneUser::getGoodnessPrice(DWORD price , bool isBuy)
{
	float ret = price;
	if(isBuy)
	{
		switch(getGoodnessState())
		{
			case Cmd::GOODNESS_3:
				{
					ret = price * (0.2f + 1.0f);
				}
				break;
			case Cmd::GOODNESS_4:
				{
					ret = price * (0.5f + 1.0f);
				}
				break;
			case Cmd::GOODNESS_5:
			case Cmd::GOODNESS_6:
				{
					ret = price * (1.0f + 1.0f);
				}
				break;
			default:
				break;
		}
	}
	else
	{
		switch(getGoodnessState())
		{
			case Cmd::GOODNESS_3:
				{
					ret = price * 0.7f;
				}
				break;
			case Cmd::GOODNESS_4:
				{
					ret = price * (0.4f);
				}
				break;
			case Cmd::GOODNESS_5:
			case Cmd::GOODNESS_6:
				{
					ret = price * (0.1f);
				}
				break;
			default:
				break;
		}
	}
	return ret;
}


bool SceneUser::needSaveBinayArchive(BinaryArchiveType type)
{
	switch (type)
	{
		case BINARY_DOUBLE_EXP_OBJ:
			{
				return packs.equip.doubleexp_obj?true:false;
			}
			break;
		case BINARY_TONG_OBJ:
			{
				return packs.equip.tong_obj_time;
			}
			break;
		case BINARY_FAMILY_OBJ:
			{
				return packs.equip.family_obj_time;
			}
			break;
		case BINARY_KING_OBJ:
			{
				return packs.equip.king_obj_time;
			}
			break;
		case BINARY_CHANGE_COUNTRY_TIME:
			{
				return this->lastChangeCountryTime;
			}
			break;
		case BINARY_ADD_COUNTRY_TIME: //soke 领取工资时间
			{
				return this->lastAddCountryTime;
			}
			break;
		case BINARY_SAFETY:
			{
				return this->safety;
			}
			break;
		case BINARY_GIVE_MATARIAL_NUM:
			{       
				return this->Give_MatarialNum;
			}
			break;
		case BINARY_CARD_NUM:
			{
				return this->Card_num;
			}
		case BINARY_SAFETY_SETUP:
			{
				return this->safety_setup;
			}
			break;
		/////////////////////////////////////////////////
		//soke 计数管理器
        case BINARY_COUNTER_MANAGER:
            {
                return true;
            }
		/////////////////////////////////////////////////	
		default:
			break;
	}
	return false;
}

bool SceneUser::needSaveTempArchive(TempArchiveType type)
{
	switch (type)
	{
		case TEAM:
			{
				return (team.getLeader() == this->tempid) && (team.getSize() > 1);
			}
			break;
		case ENTRY_STATE:
			{
				return skillStatusM.getSaveStatusSize()>0;
			}
			break;
		case PET:
			{
				return summon || (guard && saveGuard) || (saveAdopt && adoptList.size());
				//return (pet);
			}
			break;
		case SAFETY_STATE:
			{
				return this->temp_unsafety_state;
			}
			break;
		default:
			break;
	}
	return false;
}


struct SaveTeamExec : public TeamMemExec
{
	struct PairIDAndName
	{
		DWORD id;
		char name[MAX_NAMESIZE];
	};
	SceneUser *leader;
	TempArchiveMember *data;
	int leavesize;
	SaveTeamExec(SceneUser * l , char *buf , int max)
	{
		leader = l; 
		data = (TempArchiveMember *)buf;
		leavesize = max;
	}
	bool exec(TeamMember &member)
	{
		if(leader->id == member.id)
		{
			return true;
		}
		if(leavesize - data->size >= sizeof(member.id))
		{
			PairIDAndName s;
			s.id = member.id;
			bcopy(member.name , s.name , MAX_NAMESIZE);
			bcopy(&s , &data->data[data->size] , sizeof(member.id));
			data->size += sizeof(s);
			return true;
		}
		else
		{
			return false;
		}
	}
};

/**
 * \brief 保存宠物状态
 *
 */
DWORD SceneUser::savePetState(unsigned char * data)
{
	if (0==data) return 0;
	int num = 0;

	Cmd::t_PetData * p = (Cmd::t_PetData *)(data+sizeof(int));
	if (pet)
	{
		pet->petData.hp = pet->hp;
		bcopy(&pet->petData, p, sizeof(petData));
	}
	else
		bcopy(&petData, p, sizeof(petData));
	/*
	   Cmd::t_PetData * p = (Cmd::t_PetData *)(data+sizeof(int));
	   p->type = Cmd::PET_TYPE_PET;
	   p->id = pet->npc->id;
	   strncpy(p->name, pet->name, MAX_NAMESIZE-1);
	   p->exp = 0;//pet->exp;
	   p->ai = pet->getPetAI();
	   */

	num++;

	*data = num;
	//Zebra::logger->debug("%s 有 %d 个宠物记录", name, num);

	return sizeof(int)+sizeof(Cmd::t_PetData)*num;
}

/**
 * \brief 载入宠物状态
 *
 */
DWORD SceneUser::loadPetState(unsigned char * data, int size)
{
	if (0==data) return 0;

	int num = *data;
	//Zebra::logger->debug("%s 有 %d 个宠物记录", name, num);
	int off = sizeof(int);

	for (int i=0; i<num; i++)
	{
		if (off>=size) break;

		Cmd::t_PetData * p = (Cmd::t_PetData *)(data+off);
		bcopy(p, &petData, sizeof(petData));

		if (petData.state==Cmd::PET_STATE_NORMAL)
		{
			ScenePet * newPet = NULL;
			if (0==strncmp(p->name, "", MAX_NAMESIZE))
				newPet = summonPet(p->id, Cmd::PET_TYPE_PET);
			else
				newPet = summonPet(p->id, Cmd::PET_TYPE_PET, 0, 0, p->name);

			if (newPet)
			{
				bcopy(p, &newPet->petData, sizeof(Cmd::t_PetData));
				newPet->getAbilityByLevel(newPet->petData.lv);
				newPet->hp = p->hp;
				if (newPet->hp>newPet->petData.maxhp)
					newPet->hp = newPet->petData.maxhp;
				newPet->setPetAI((Cmd::petAIMode)p->ai);
				newPet->sendData();
			}
		}

		off += sizeof(Cmd::t_PetData);
	}

	return off;
}

DWORD SceneUser::saveTempPetState(BYTE *data, DWORD maxSize)
{
	if (maxSize<4)
	{
		Zebra::logger->error("[宠物]保存宠物临时文档失败，没有空间 maxSize=0");
		return 0;
	}

	BYTE num=0;
	DWORD size = sizeof(num);
	if (summon)
	{
		*(data+size) = summon->getPetType();
		size++;

		bcopy(&summon->petData, data+size, sizeof(Cmd::t_PetData));
		size += sizeof(Cmd::t_PetData);
		DWORD *length = (DWORD*)(data+size);
		size += sizeof(DWORD);
		summon->skillStatusM.saveSkillStatus((char *)(data+size), *length);
		size += *length;
		num++;
	}
	if (guard && saveGuard)
	{
		*(data+size) = guard->getPetType();
		size++;
		size += ((GuardNpc *)guard)->save(data+size);
		DWORD *length = (DWORD*)(data+size);
		size += sizeof(DWORD);
		guard->skillStatusM.saveSkillStatus((char *)(data+size), *length);
		size += *length;
		num++;
	}
	if (saveAdopt)
	{
		for (adopt_it it=adoptList.begin(); it!=adoptList.end(); it++)
		{
			*(data+size) = it->second->getPetType();
			size++;

			size += it->second->save(data+size);
			DWORD *length = (DWORD*)(data+size);
			size += sizeof(DWORD);
			it->second->skillStatusM.saveSkillStatus((char *)(data+size), *length);
			size += *length;
			num++;
		}
	}

	if (size>maxSize)
	{
		Zebra::logger->error("[宠物]保存宠物临时文档失败，空间不足 size=%u", size);
		*data = 0;
		return 1;
	}
	*data = num;
	return size;
}

DWORD SceneUser::loadTempPetState(BYTE * data)
{
	if (0==data) return 0;

	BYTE num = *data;
	Zebra::logger->debug("%s 有 %d 个临时宠物记录", name, num);
	DWORD off = sizeof(num);

	for (DWORD i=0; i<num; i++)
	{
		off++;//宠物类型

		switch(*(data+off-1))
		{
			case Cmd::PET_TYPE_SUMMON:
			case Cmd::PET_TYPE_GUARDNPC:
				{
					Cmd::t_PetData * p = (Cmd::t_PetData *)(data+off);
					//bcopy(p, &petData, sizeof(petData));

					ScenePet * newPet = NULL;
					if (p->state==Cmd::PET_STATE_NORMAL)
					{
						if (strncmp(p->name, "", MAX_NAMESIZE) && p->type!=Cmd::PET_TYPE_GUARDNPC)
							newPet = summonPet(p->id, p->type, 0, 0, p->name);
						else
							newPet = summonPet(p->id, p->type);

						if (newPet)
						{
							bcopy(p, &newPet->petData, sizeof(Cmd::t_PetData));
							newPet->hp = p->hp;
							newPet->setPetAI((Cmd::petAIMode)p->ai);
							if (p->type==Cmd::PET_TYPE_GUARDNPC)//镖车数据
							{
								((GuardNpc *)newPet)->owner(this);
								((GuardNpc *)newPet)->dest(zPos(p->str, p->intel));
								((GuardNpc *)newPet)->map(p->name);
								((GuardNpc *)newPet)->gold(p->agi);
								((GuardNpc *)newPet)->exp(p->men);
								strncpy(newPet->petData.name, "镖车", MAX_NAMESIZE);
							}
							newPet->sendData();
						}
					}

					off += sizeof(Cmd::t_PetData);
					DWORD *length = (DWORD*)(data+off);
					off += sizeof(DWORD);
					if (newPet) newPet->skillStatusM.loadSkillStatus((char *)(data+off), *length);
					off += *length;
				}
				break;
			case Cmd::PET_TYPE_CARTOON:
				{
					Cmd::t_CartoonData * p = (Cmd::t_CartoonData *)(data+off+sizeof(DWORD));

					zNpcB *base = npcbm.get(p->npcID);
					if (!base) return false;

					t_NpcDefine define;
					define.id = base->id;
					strncpy(define.name, p->name, MAX_NAMESIZE-1);
					define.pos = getPos();
					define.num = 1;
					define.interval = 5;
					define.initstate = zSceneEntry::SceneEntry_Normal;
					define.width = SceneUser::CALL_PET_REGION;
					define.height = SceneUser::CALL_PET_REGION;
					define.pos -= zPos(SceneUser::CALL_PET_REGION/2,SceneUser::CALL_PET_REGION/2);
					define.scriptID = 0;
					scene->initRegion(define.region, define.pos, define.width, define.height);

					CartoonPet * newPet = (CartoonPet *)scene->summonOneNpc<CartoonPet>(define, zPos(0,0), base, 0, 0, 0);
					if (newPet) 
					{
						//strncpy(newPet->name, p->name, MAX_NAMESIZE);
						newPet->setPetType(Cmd::PET_TYPE_CARTOON);
						newPet->setMaster(this);
						SceneNpcManager::getMe().addSpecialNpc(newPet);

						newPet->setCartoonID(*(DWORD *)(data+off));
						newPet->setCartoonData(*p);
						adoptList[newPet->getCartoonID()] = newPet;
					}

					off += sizeof(DWORD)+sizeof(Cmd::t_CartoonData);
					DWORD *length = (DWORD*)(data+off);
					off += sizeof(DWORD);
					if (newPet) newPet->skillStatusM.loadSkillStatus((char *)(data+off), *length);
					off += *length;
				}
				break;
			case Cmd::PET_TYPE_XCARTOON:
				{
					Cmd::t_CartoonData * p = (Cmd::t_CartoonData *)(data+off+sizeof(DWORD));

					zNpcB *base = npcbm.get(p->npcID);
					if (!base) return false;

					t_NpcDefine define;
					define.id = base->id;
					strncpy(define.name, p->name, MAX_NAMESIZE-1);
					define.pos = getPos();
					define.num = 1;
					define.interval = 5;
					define.initstate = zSceneEntry::SceneEntry_Normal;
					define.width = SceneUser::CALL_PET_REGION;
					define.height = SceneUser::CALL_PET_REGION;
					define.pos -= zPos(SceneUser::CALL_PET_REGION/2,SceneUser::CALL_PET_REGION/2);
					define.scriptID = 0;
					scene->initRegion(define.region, define.pos, define.width, define.height);

					CartoonPet * newPet = (CartoonPet *)scene->summonOneNpc<CartoonPet>(define, zPos(0,0), base, 0, 0, 0);
					if (newPet) 
					{
						//strncpy(newPet->name, p->name, MAX_NAMESIZE);
						newPet->setPetType(Cmd::PET_TYPE_XCARTOON);
						newPet->setMaster(this);
						SceneNpcManager::getMe().addSpecialNpc(newPet);

						newPet->setCartoonID(*(DWORD *)(data+off));
						newPet->setCartoonData(*p);
						adoptList[newPet->getCartoonID()] = newPet;
					}

					off += sizeof(DWORD)+sizeof(Cmd::t_CartoonData);
					DWORD *length = (DWORD*)(data+off);
					off += sizeof(DWORD);
					if (newPet) newPet->skillStatusM.loadSkillStatus((char *)(data+off), *length);
					off += *length;
				}
				break;
			case Cmd::PET_TYPE_XCARTOON2:
				{
					Cmd::t_CartoonData * p = (Cmd::t_CartoonData *)(data+off+sizeof(DWORD));

					zNpcB *base = npcbm.get(p->npcID);
					if (!base) return false;

					t_NpcDefine define;
					define.id = base->id;
					strncpy(define.name, p->name, MAX_NAMESIZE-1);
					define.pos = getPos();
					define.num = 1;
					define.interval = 5;
					define.initstate = zSceneEntry::SceneEntry_Normal;
					define.width = SceneUser::CALL_PET_REGION;
					define.height = SceneUser::CALL_PET_REGION;
					define.pos -= zPos(SceneUser::CALL_PET_REGION/2,SceneUser::CALL_PET_REGION/2);
					define.scriptID = 0;
					scene->initRegion(define.region, define.pos, define.width, define.height);

					CartoonPet * newPet = (CartoonPet *)scene->summonOneNpc<CartoonPet>(define, zPos(0,0), base, 0, 0, 0);
					if (newPet) 
					{
						//strncpy(newPet->name, p->name, MAX_NAMESIZE);
						newPet->setPetType(Cmd::PET_TYPE_XCARTOON2);
						newPet->setMaster(this);
						SceneNpcManager::getMe().addSpecialNpc(newPet);

						newPet->setCartoonID(*(DWORD *)(data+off));
						newPet->setCartoonData(*p);
						adoptList[newPet->getCartoonID()] = newPet;
					}

					off += sizeof(DWORD)+sizeof(Cmd::t_CartoonData);
					DWORD *length = (DWORD*)(data+off);
					off += sizeof(DWORD);
					if (newPet) newPet->skillStatusM.loadSkillStatus((char *)(data+off), *length);
					off += *length;
				}
				break;
			default:
				break;
		}
	}

	return off;
}

/**
 * \brief 保存卡通宠物
 *
 */
void SceneUser::saveCartoonState()
{
	for (cartoon_it it=cartoonList.begin(); it!=cartoonList.end(); it++)
	{
		if (it->second.state==Cmd::CARTOON_STATE_WAITING
				|| it->second.state==Cmd::CARTOON_STATE_ADOPTED)
			continue;

		Cmd::Session::t_saveCartoon_SceneSession send;
		strncpy(send.userName, name, MAX_NAMESIZE);
		send.type = Cmd::Session::SAVE_TYPE_TIMETICK;
		send.cartoonID = it->first;
		send.data = it->second;
		sessionClient->sendCmd(&send, sizeof(send));
	}
	/*
	   for (adopt_it it=adoptList.begin(); it!=adoptList.end(); it++)
	   {
	   Cmd::Session::t_saveCartoon_SceneSession send;
	   send.type = Cmd::Session::SAVE_TYPE_SYN;
	   send.cartoonID = it->first;
	   send.data = it->second->getCartoonData();
	   sessionClient->sendCmd(&send, sizeof(send));
	   }
	   */
}

/**
 * \brief 删除召唤兽
 *
 */
void SceneUser::killSummon()
{
	if (summon)
	{
		summon->skillStatusM.clearActiveSkillStatus();

		summon->killAllPets();
		summon->leaveBattle();
		summon->scene->clearBlock(summon->getPos());
		summon->setState(SceneEntry_Death);
		summon->setMoveTime(SceneTimeTick::currentTime, summon->define->interval*1000);//尸体消失的时间

		Cmd::stNpcDeathUserCmd death;
		death.dwNpcTempID = summon->tempid;
		summon->scene->sendCmdToNine(summon->getPosI() , &death , sizeof(death));

		Cmd::stDelPetPetCmd del;
		del.id= summon->tempid;
		del.type = Cmd::PET_TYPE_SUMMON;
		sendCmdToMe(&del, sizeof(del));

		summon->clearMaster();
		summon = 0;

		//summon->toDie(tempid);
		//killOnePet(summon);
	}
}

void SceneUser::shouhuibaby()
{
		if (xcartoon2)
					{
						xcartoon2->getCartoonData().state = Cmd::CARTOON_STATE_PUTAWAY;
						xcartoon2->save(Cmd::Session::SAVE_TYPE_PUTAWAY);
						xcartoon2->toDie(tempid);
						killOnePet(xcartoon2);
					}
}
/**
 * \brief 删除所有宠物
 *
 */
void SceneUser::killAllPets()
{
	//删除所有宠物
#ifdef _XWL_DEBUG
	//Zebra::logger->debug("SceneUser::killAllPets(): lock %s", name);
#endif
	std::list<ScenePet *> copy(totems);
	for (std::list<ScenePet *>::iterator it=copy.begin(); it!=copy.end(); it++)
	{
		(*it)->skillStatusM.clearActiveSkillStatus();

		(*it)->killAllPets();
		(*it)->leaveBattle();
		(*it)->scene->clearBlock((*it)->getPos());
		(*it)->setState(SceneEntry_Death);
		(*it)->setMoveTime(SceneTimeTick::currentTime, (*it)->define->interval*1000);//尸体消失的时间

		Cmd::stDelPetPetCmd del;
		del.id= (*it)->tempid;
		del.type = Cmd::PET_TYPE_TOTEM;
		sendCmdToMe(&del, sizeof(del));

		(*it)->clearMaster();

		Cmd::stNpcDeathUserCmd death;
		death.dwNpcTempID = (*it)->tempid;
		(*it)->scene->sendCmdToNine((*it)->getPosI() , &death , sizeof(death));
	}
	totems.clear();

	if (ridepet)
	{
		horse.data.state = Cmd::HORSE_STATE_PUTUP;
		horse.sendData();

		ridepet->setClearState();
		ridepet->clearMaster();
		ridepet = 0;
		/*
		   Cmd::stDelPetPetCmd del;
		   del.id= ridepet->tempid;
		   del.type = Cmd::PET_TYPE_RIDE;
		   sendCmdToMe(&del, sizeof(del));

		   ridepet->clearMaster();
		   ridepet->setClearState();
		   ridepet = 0;
		   */
		//ridepet->toDie(tempid);
		//killOnePet(ridepet);
	}
	if (pet)
	{
		pet->skillStatusM.clearActiveSkillStatus();

		pet->petData.hp = pet->hp;
		bcopy(&pet->petData, &petData, sizeof(petData));

		pet->killAllPets();
		pet->leaveBattle();
		pet->scene->clearBlock(pet->getPos());
		pet->setState(SceneEntry_Death);
		pet->setMoveTime(SceneTimeTick::currentTime, pet->define->interval*1000);//尸体消失的时间

		Cmd::stNpcDeathUserCmd death;
		death.dwNpcTempID = pet->tempid;
		pet->scene->sendCmdToNine(pet->getPosI() , &death , sizeof(death));

		Cmd::stDelPetPetCmd del;
		del.id= pet->tempid;
		del.type = Cmd::PET_TYPE_PET;
		sendCmdToMe(&del, sizeof(del));

		pet->clearMaster();
		pet = 0;
		//pet->toDie(tempid);
		//killOnePet(pet);
	}
	if (summon)
	{
		summon->skillStatusM.clearActiveSkillStatus();

		summon->killAllPets();
		summon->leaveBattle();
		summon->scene->clearBlock(summon->getPos());
		summon->setState(SceneEntry_Death);
		summon->setMoveTime(SceneTimeTick::currentTime, summon->define->interval*1000);//尸体消失的时间

		Cmd::stNpcDeathUserCmd death;
		death.dwNpcTempID = summon->tempid;
		summon->scene->sendCmdToNine(summon->getPosI() , &death , sizeof(death));

		Cmd::stDelPetPetCmd del;
		del.id= summon->tempid;
		del.type = Cmd::PET_TYPE_SUMMON;
		sendCmdToMe(&del, sizeof(del));

		summon->clearMaster();
		summon = 0;

		//summon->toDie(tempid);
		//killOnePet(summon);
	}
	if (guard)
	{
		Cmd::stDelPetPetCmd del;
		del.id= guard->tempid;
		del.type = Cmd::PET_TYPE_GUARDNPC;
		sendCmdToMe(&del, sizeof(del));

		//guard->reset();
		//guard->toDie(tempid);
		/*
		   OnOther event(2);
		   EventTable::instance().execute(*this, event);
		   guard->reset();
		//guard->clearMaster();
		guard = 0;
		*/
		//killOnePet(guard);
	}

	//保存跟随的宠物
	if (cartoon)
		cartoon->putAway(Cmd::Session::SAVE_TYPE_PUTAWAY);
	//保存收起的宠物
	for (cartoon_it it=cartoonList.begin(); it!=cartoonList.end(); it++)
	{
		if ((cartoon && cartoon->getCartoonID()==it->first)
				|| it->second.state==Cmd::CARTOON_STATE_WAITING
				|| it->second.state==Cmd::CARTOON_STATE_ADOPTED)
			continue;

		Cmd::Session::t_saveCartoon_SceneSession send;
		strncpy(send.userName, name, MAX_NAMESIZE);
		send.type = Cmd::Session::SAVE_TYPE_PUTAWAY;
		send.cartoonID = it->first;
		it->second.state = Cmd::CARTOON_STATE_PUTAWAY;
		it->second.repair = 0;
		send.data = it->second;
		sessionClient->sendCmd(&send, sizeof(send));
	}
	//归还领养的宠物
	for (adopt_it it=adoptList.begin(); it!=adoptList.end();)
		(it++)->second->putAway(saveAdopt?Cmd::Session::SAVE_TYPE_DONTSAVE:Cmd::Session::SAVE_TYPE_RETURN);
#ifdef _XWL_DEBUG
	//Zebra::logger->debug("SceneUser::killAllPets(): unlock %s", name);
#endif
}

void SceneUser::clearGuardNpc()
{
	if (guard)
	{
		guard->skillStatusM.clearActiveSkillStatus();
		guard->killAllPets();
		guard->leaveBattle();
		guard->scene->clearBlock(guard->getPos());
		guard->setState(SceneEntry_Death);
		guard->setMoveTime(SceneTimeTick::currentTime, guard->define->interval*1000);//尸体消失的时间

		Cmd::stNpcDeathUserCmd death;
		death.dwNpcTempID = guard->tempid;
		guard->scene->sendCmdToNine(guard->getPosI() , &death , sizeof(death));

		Cmd::stDelPetPetCmd del;
		del.id= guard->tempid;
		del.type = Cmd::PET_TYPE_GUARDNPC;
		sendCmdToMe(&del, sizeof(del));

		guard->clearMaster();
		guard = 0;
	}
}
/**
 * \brief 保存二进制数据扩充部分数据
 *
 *
 * \param type 类型(BinaryArchiveType)
 * \param out 输出数据
 * \param maxSize 可用out大小
 * \return 使用out的字节数
 */
DWORD SceneUser::addBinaryArchiveMember(DWORD type , char *out , DWORD maxSize)
{
	DWORD dwSize = 0;
	if(!out)
	{
		return dwSize;
	}
	dwSize = sizeof(BinaryArchiveMember);
	BinaryArchiveMember *data = (BinaryArchiveMember *)out;
	data->type = type;
	data->size = 0;
	switch(data->type)
	{
		case BINARY_DOUBLE_EXP_OBJ:
			{
				*(DWORD*)&data->data[data->size]=packs.equip.doubleexp_obj;
				data->size += sizeof(DWORD);
				*(DWORD*)&data->data[data->size]=packs.equip.doubleexp_obj_time;
				data->size += sizeof(DWORD);
				dwSize += data->size;
			}
			break;
		case BINARY_TONG_OBJ:
			{
				*(DWORD*)&data->data[data->size]=packs.equip.tong_obj_times;
				data->size += sizeof(DWORD);
				*(DWORD*)&data->data[data->size]=packs.equip.tong_obj_time;
				data->size += sizeof(DWORD);
				dwSize += data->size;
			}
			break;
		case BINARY_FAMILY_OBJ:
			{
				*(DWORD*)&data->data[data->size]=packs.equip.family_obj_times;
				data->size += sizeof(DWORD);
				*(DWORD*)&data->data[data->size]=packs.equip.family_obj_time;
				data->size += sizeof(DWORD);
				dwSize += data->size;
			}
			break;
		case BINARY_KING_OBJ:
			{
				*(DWORD*)&data->data[data->size]=packs.equip.king_obj_times;
				data->size += sizeof(DWORD);
				*(DWORD*)&data->data[data->size]=packs.equip.king_obj_time;
				data->size += sizeof(DWORD);
				dwSize += data->size;
			}
			break;
		case BINARY_CHANGE_COUNTRY_TIME:
			{
				*(DWORD*)&data->data[data->size]=this->lastChangeCountryTime;
				data->size += sizeof(DWORD);
				dwSize += data->size;
			}
			break;
		case BINARY_ADD_COUNTRY_TIME: //soke 领取工资时间
			{
				*(DWORD*)&data->data[data->size]=this->lastAddCountryTime;
				data->size += sizeof(DWORD);
				dwSize += data->size;
			}
			break;
		case BINARY_SAFETY:
			{
				*(BYTE*)&data->data[data->size]=this->safety;
				data->size += sizeof(BYTE);
				dwSize += data->size;
			}
			break;
		case BINARY_GIVE_MATARIAL_NUM:
			{
				*(DWORD*)&data->data[data->size]=this->Give_MatarialNum;
				data->size += sizeof(DWORD);
				dwSize += data->size;
			}
			break;
		case BINARY_CARD_NUM:
			{
				*(DWORD*)&data->data[data->size]=this->Card_num;
				data->size += sizeof(DWORD);
				dwSize += data->size;
			}
			break;
		case BINARY_SAFETY_SETUP:
			{
				*(BYTE*)&data->data[data->size]=this->safety_setup;
				data->size += sizeof(BYTE);
				dwSize += data->size;
			}
			break;
		///////////////////////////////////////////////////	
		//soke 计数管理器
		case BINARY_COUNTER_MANAGER:
			{
                data->size += cm.save(&(data->data[data->size]));
                dwSize += data->size;
			}
			break;
		///////////////////////////////////////////////////	
		case BINARY_MAX:
			{
			}
			break;
		default:
			break;
	}
	return dwSize;
}
/**
 * \brief 存储小组到临时文档
 *
 */
DWORD SceneUser::addTempArchiveMember(DWORD type , char *out , DWORD maxSize)
{
	DWORD dwSize = 0;
	if(!out)
	{
		return dwSize;
	}
	dwSize = sizeof(TempArchiveMember);
	TempArchiveMember *data = (TempArchiveMember *)out;
	data->type = type;
	data->size = 0;
	switch(data->type)
	{
		case TEAM:
			{
				SaveTeamExec st(this , out , maxSize);
				team.execEveryOne(st);
				*(DWORD*)&data->data[data->size]=this->team_mode;
				data->size += sizeof(DWORD);
				*(DWORD*)&data->data[data->size]=this->team.getNextObjOwnerID();
				data->size += sizeof(DWORD);
				*(BYTE*)&data->data[data->size]=this->team.getObjMode();
				data->size += sizeof(BYTE);
				*(BYTE*)&data->data[data->size]=this->team.getExpMode();
				data->size += sizeof(BYTE);
				//Zebra::logger->debug("临时存储队伍信息大小%u!" , data->size);
				dwSize += data->size;
			}
			break;
		case ENTRY_STATE:
			{
				skillStatusM.saveSkillStatus(data->data, data->size);
				dwSize += data->size;
				//Zebra::logger->debug("状态临时文档大小%u" , data->size);
			}
			break;
		case PET:
			{
				data->size = saveTempPetState((BYTE *)data->data, maxSize-dwSize);
				dwSize += data->size;
				//Zebra::logger->debug("宠物临时文档大小%u" , data->size);
			}
			break;
		case SAFETY_STATE:
			{
				*(DWORD*)&data->data[data->size]=this->temp_unsafety_state;
				data->size += sizeof(DWORD);
			}
			break;
		default:
			break;
	}
	return dwSize;
}

DWORD SceneUser::saveBinaryArchive(unsigned char *out , const int maxsize)
{
	DWORD size=0;
	unsigned char *data=out;
	if(needSaveBinayArchive(BINARY_DOUBLE_EXP_OBJ))
	{
		size += addBinaryArchiveMember(BINARY_DOUBLE_EXP_OBJ,(char *)data,maxsize-size);
		data = &data[size];
	}
	if(needSaveBinayArchive(BINARY_TONG_OBJ))
	{
		size += addBinaryArchiveMember(BINARY_TONG_OBJ,(char *)data,maxsize-size);
		data = &data[size];
	}
	if(needSaveBinayArchive(BINARY_FAMILY_OBJ))
	{
		size += addBinaryArchiveMember(BINARY_FAMILY_OBJ,(char *)data,maxsize-size);
		data = &data[size];
	}
	if(needSaveBinayArchive(BINARY_KING_OBJ))
	{
		size += addBinaryArchiveMember(BINARY_KING_OBJ,(char *)data,maxsize-size);
		data = &data[size];
	}
	if (needSaveBinayArchive(BINARY_CHANGE_COUNTRY_TIME))
	{
		size += addBinaryArchiveMember(BINARY_CHANGE_COUNTRY_TIME, (char*)data, maxsize-size);
		data = &data[size];
	}
	if (needSaveBinayArchive(BINARY_ADD_COUNTRY_TIME)) //soke 领取工资时间
	{
		size += addBinaryArchiveMember(BINARY_ADD_COUNTRY_TIME, (char*)data, maxsize-size);
		data = &data[size];
	}
	
	if (needSaveBinayArchive(BINARY_SAFETY))
	{
		size += addBinaryArchiveMember(BINARY_SAFETY, (char*)data, maxsize-size);
		data = &data[size];
	}

	if (needSaveBinayArchive(BINARY_GIVE_MATARIAL_NUM))
	{
		size += addBinaryArchiveMember(BINARY_GIVE_MATARIAL_NUM, (char*)data, maxsize-size);
		data = &data[size];
	}
	if (needSaveBinayArchive(BINARY_CARD_NUM))
	{
		size += addBinaryArchiveMember(BINARY_CARD_NUM, (char*)data, maxsize-size);
		data = &data[size];
	}

	if (needSaveBinayArchive(BINARY_SAFETY_SETUP))
	{
		size += addBinaryArchiveMember(BINARY_SAFETY_SETUP, (char*)data, maxsize-size);
		data = &data[size];
	}

	///////////////////////////////////////////////////////
	//soke 计数管理器
	if (needSaveBinayArchive(BINARY_COUNTER_MANAGER))
	{
		size += addBinaryArchiveMember(BINARY_COUNTER_MANAGER, (char*)data, maxsize-size);
		data = &data[size];
	}
	////////////////////////////////////////////////////////

	size += addBinaryArchiveMember(BINARY_MAX,(char *)data,maxsize-size);
	return sizeof(DWORD) + size;
}
/**
 * \brief 保存所有临时档案
 *
 */
void SceneUser::saveTempArchive()
{
	bool saved = false;
	char buf[MAX_TEMPARCHIVE_SIZE + sizeof(TempArchiveMember) + sizeof(Cmd::Session::t_WriteUser_SceneArchive)];

	bzero(buf , sizeof(buf));
	Cmd::Session::t_WriteUser_SceneArchive *ws = (Cmd::Session::t_WriteUser_SceneArchive *)buf;
	constructInPlace(ws);
	ws->id = this->id;
	ws->dwMapTempID = this->scene->tempid; 
	char *data = ws->data;
	//组队临时档案
	if(needSaveTempArchive(TEAM))
	{
		saved = true;
		ws->dwSize += addTempArchiveMember(TEAM , data , MAX_TEMPARCHIVE_SIZE - ws->dwSize);
		data = &ws->data[ws->dwSize];
		//*(DWORD*)data = this->team_mode;
		//data = data + sizeof(DWORD);
	}
	//状态临时档案
	if(needSaveTempArchive(ENTRY_STATE))
	{
		saved = true;
		ws->dwSize += addTempArchiveMember(ENTRY_STATE , data , MAX_TEMPARCHIVE_SIZE - ws->dwSize);
		data = &ws->data[ws->dwSize];
	}
	//宠物状态
	if(needSaveTempArchive(PET))
	{
		saved = true;
		ws->dwSize += addTempArchiveMember(PET , data , MAX_TEMPARCHIVE_SIZE - ws->dwSize);
		data = &ws->data[ws->dwSize];
	}
	// 密码保护状态
	if (needSaveTempArchive(SAFETY_STATE))
	{
		saved = true;
		ws->dwSize += addTempArchiveMember(SAFETY_STATE , data , MAX_TEMPARCHIVE_SIZE - ws->dwSize);
		data = &ws->data[ws->dwSize];
	}
	//Zebra::logger->debug("临时存档数据大小%u" , ws->dwSize);
	//TODO 其他档案数据

	if(saved)
	{
		sessionClient->sendCmd(ws , sizeof(Cmd::Session::t_WriteUser_SceneArchive) + ws->dwSize);
	}
}

DWORD SceneUser::setupBinaryArchive(const char *revData)
{
	DWORD size=0;
	BinaryArchiveMember *data = (BinaryArchiveMember *)revData;
	while(data->size)//为0是BINARY_MAX占位的
	{
		switch(data->type)
		{
			case BINARY_DOUBLE_EXP_OBJ:
				{
					std::pair<DWORD,DWORD> *pair=(std::pair<DWORD,DWORD>*)data->data;
					//判断是否在同一天
					if(SceneTimeTick::currentTime.sec()/86400 == pair->second/86400)
					{
						this->packs.equip.doubleexp_obj=pair->first;
						this->packs.equip.doubleexp_obj_time=pair->second;
					}
				}
				break;
			case BINARY_TONG_OBJ:
				{
					std::pair<DWORD,DWORD> *pair=(std::pair<DWORD,DWORD>*)data->data;
					//判断是否在同一天
					if(SceneTimeTick::currentTime.sec()/86400 == pair->second/86400)
					{
						this->packs.equip.tong_obj_times=pair->first;
						this->packs.equip.tong_obj_time=pair->second;
					}
				}
				break;
		case BINARY_FAMILY_OBJ:
				{
					std::pair<DWORD,DWORD> *pair=(std::pair<DWORD,DWORD>*)data->data;
					//判断是否在同一天
					if(SceneTimeTick::currentTime.sec()/86400 == pair->second/86400)
					{
						this->packs.equip.family_obj_times=pair->first;
						this->packs.equip.family_obj_time=pair->second;
					}
				}
				break;
		case BINARY_KING_OBJ:
				{
					std::pair<DWORD,DWORD> *pair=(std::pair<DWORD,DWORD>*)data->data;
					//判断是否在同一天
					if(SceneTimeTick::currentTime.sec()/86400 == pair->second/86400)
					{
						this->packs.equip.king_obj_times=pair->first;
						this->packs.equip.king_obj_time=pair->second;
					}
				}
				break;
		case BINARY_CHANGE_COUNTRY_TIME:
				{
					this->lastChangeCountryTime = *(DWORD*)data->data;
				}
				break;
		case BINARY_ADD_COUNTRY_TIME: //soke 领取工资时间
				{
					this->lastAddCountryTime = *(DWORD*)data->data;
				}
				break;
		case BINARY_SAFETY:
				{
					this->safety = *(BYTE*)data->data;
				}
				break;
		case BINARY_GIVE_MATARIAL_NUM:
				{
					this->Give_MatarialNum = *(DWORD*)data->data;
				}
				break;
		case BINARY_CARD_NUM:
				{
					this->Card_num = *(DWORD*)data->data;
				}
				break; 
		case BINARY_SAFETY_SETUP:
				{
					this->safety_setup = *(BYTE*)data->data;
				}
				break;
		/////////////////////////////////////////////////
		//soke 计数管理器		
        case BINARY_COUNTER_MANAGER:
            {
                cm.load( (char*)data->data);
            }
            break;
		//////////////////////////////////////////////////	
		default:
				break;
		}
		size += data->size;
		data = (BinaryArchiveMember *)&data->data[data->size];
	}
	return size;
}

void SceneUser::setupTempArchive(const char *revData , const DWORD dwSize)
{
	if(dwSize > MAX_TEMPARCHIVE_SIZE)
	{
		return ;
	}
	char buf[MAX_TEMPARCHIVE_SIZE + sizeof(TempArchiveMember)];
	bzero(buf , sizeof(buf));
	bcopy(revData , buf , dwSize);
	TempArchiveMember *data = (TempArchiveMember *)buf;
	//Zebra::logger->debug("临时文档大小%u" , data->size);
	while(data->size)
	{
		switch(data->type)
		{
			case TEAM:
				{
					team.loadTeam(this , data);
					//Zebra::logger->debug("读取队伍临时文档大小%u" , data->size);
				}
				break;
			case ENTRY_STATE:
				{
					//Zebra::logger->debug("读取状态临时文档大小%u" , data->size);
					skillStatusM.loadSkillStatus(data->data, data->size);
				}
				break;
			case PET:
				{
					loadTempPetState((BYTE *)data->data);
					//Zebra::logger->debug("读取宠物临时文档大小%u" , data->size);
				}
				break;
			case SAFETY_STATE:
				{
					temp_unsafety_state = *(DWORD*)data->data;
					Zebra::logger->debug("%s  密码保护状态: %u", this->name, temp_unsafety_state);

					if (this->safety==1)
					{
						Cmd::stNotifySafetyUserCmd send;
						if (this->temp_unsafety_state==1)
							send.byState = Cmd::SAFETY_TEMP_CLOSE;
						else
							send.byState = Cmd::SAFETY_OPEN;

						this->sendCmdToMe(&send, sizeof(send));
					}
				}
				break;
			default:
				{
					Zebra::logger->debug("加载临时档案失败!");
					return;
				}
				break;
		}
		data = (TempArchiveMember *)&data->data[data->size];
	}
}

/**
 * \brief 掉落装备的处理
 *
 *
 */
void SceneUser::lostObject(SceneUser *pAtt)
{
	if(this->scene && this->scene->isNoRedScene())
	{
		return;
	}
	int mainpack = 0;
	int equippack = 0;
	//如果是被npc杀死
	/*
	   if(!pAtt)//npc
	   {
	// */
	switch(this->getGoodnessState())
	{
		case Cmd::GOODNESS_0:
			{
				return;
			}
			break;
		case Cmd::GOODNESS_1:
			{
				if(zMisc::selectByPercent(2))
				{
					mainpack = 1;
				}
			}
			break;
		case Cmd::GOODNESS_2_1:
			{
				if(zMisc::selectByPercent(30))
				{
					mainpack = zMisc::randBetween(1 , 3);
				}
			}
			break;
		case Cmd::GOODNESS_3:
			{
				if(zMisc::selectByPercent(30))
				{
					mainpack = zMisc::randBetween(1 , 3);
				}
				/*
				   if(zMisc::selectByPercent(50))
				   {
				   mainpack = 1;//zMisc::randBetween(1 , 3);
				   }
				   if(zMisc::selectByPercent(20))
				   {
				   equippack = 1;
				   }
				// */
			}
			break;
		case Cmd::GOODNESS_4:
			{
				if(zMisc::selectByPercent(50))
				{
					mainpack = zMisc::randBetween(1 , 5);
				}
				if(zMisc::selectByPercent(50))
				{
					equippack = 1;
				}
			}
			break;
		case Cmd::GOODNESS_5:
			{
				if(zMisc::selectByPercent(100))
				{
					mainpack = zMisc::randBetween(5 , 10);
				}
				if(zMisc::selectByPercent(100))
				{
					equippack = 1;//zMisc::randBetween(1 , 3);
				}
			}
			break;
		case Cmd::GOODNESS_6:
			{
				if(zMisc::selectByPercent(100))
				{
					mainpack = zMisc::randBetween(5 , 20);
				}
				if(zMisc::selectByPercent(100))
				{
					equippack = 1;//zMisc::randBetween(1 , 4);
				}
			}
			break;
		case Cmd::GOODNESS_7:
			{
				if(zMisc::selectByPercent(100))
				{
					mainpack = zMisc::randBetween(5 , 20);
				}
				if(zMisc::selectByPercent(100))
				{
					equippack = zMisc::randBetween(1 , 4);
				}
			}
			break;
	}
	/*
	   }
	   else if(isAllied(pAtt))//本国,盟国人物
	   {

	   switch(this->getGoodnessState())
	   {
	   case Cmd::GOODNESS_0:
	   {
	   return ;
	   }
	   break;
	   case Cmd::GOODNESS_1:
	   {
	   return ;
	   }
	   break;
	   case Cmd::GOODNESS_2_1:
	   {
	   return ;
	   }
	   break;
	   case Cmd::GOODNESS_3:
	   {
	   if(zMisc::selectByPercent(10))
	   {
	   mainpack = zMisc::randBetween(1 , 3);
	   }
	   }
	   break;
	   case Cmd::GOODNESS_4:
	   {
	   if(zMisc::selectByPercent(20))
	   {
	   mainpack = zMisc::randBetween(1 , 4);
	   }
	   if(zMisc::selectByPercent(10))
	   {
	   equippack = zMisc::randBetween(1 , 2);
	   }
	   }
	   break;
	   case Cmd::GOODNESS_5:
	   {
	   if(zMisc::selectByPercent(40))
	   {
	   mainpack = zMisc::randBetween(1 , 5);
	   }
	   if(zMisc::selectByPercent(20))
	   {
	   equippack = zMisc::randBetween(1 , 3);
	   }
	   }
	   break;
	   case Cmd::GOODNESS_6:
	   {
	   if(zMisc::selectByPercent(60))
	   {
	   mainpack = zMisc::randBetween(1 , 6);
	   }
	   if(zMisc::selectByPercent(40))
	   {
	   equippack = zMisc::randBetween(1 , 4);
	   }
	   }
	   break;
	   case Cmd::GOODNESS_7:
	   {
	   if(zMisc::selectByPercent(40))
	   {
	   mainpack = zMisc::randBetween(1 , 6);
	   }
	if(zMisc::selectByPercent(40))
	{
		equippack = zMisc::randBetween(1 , 4);
	}
}
break;
}
}
else //敌国,中立国人物
{

	switch(this->getGoodnessState())
	{
		case Cmd::GOODNESS_0:
			{
				if(zMisc::selectByPercent(4))
				{
					mainpack = zMisc::randBetween(1 , 2);
				}
				if(zMisc::selectByPercent(2))
				{
					equippack = zMisc::randBetween(1 , 2);
				}
			}
			break;
		case Cmd::GOODNESS_1:
			{
				if(zMisc::selectByPercent(4))
				{
					mainpack = zMisc::randBetween(1 , 6);
				}
				if(zMisc::selectByPercent(2))
				{
					equippack = zMisc::randBetween(1 , 2);
				}
			}
			break;
		case Cmd::GOODNESS_2_1:
			{
				if(zMisc::selectByPercent(4))
				{
					mainpack = zMisc::randBetween(1 , 2);
				}
			}
			break;
		case Cmd::GOODNESS_3:
			{
				if(zMisc::selectByPercent(4))
				{
					mainpack = zMisc::randBetween(1 , 4);
				}
			}
			break;
		case Cmd::GOODNESS_4:
			{
				if(zMisc::selectByPercent(10))
				{
					mainpack = zMisc::randBetween(1 , 4);
				}
				if(zMisc::selectByPercent(4))
				{
					equippack = zMisc::randBetween(1 , 2);
				}
			}
			break;
		case Cmd::GOODNESS_5:
			{
				if(zMisc::selectByPercent(10))
				{
					mainpack = zMisc::randBetween(1 , 6);
				}
				if(zMisc::selectByPercent(10))
				{
					equippack = zMisc::randBetween(1 , 2);
				}
			}
			break;
		case Cmd::GOODNESS_6:
			{
				if(zMisc::selectByPercent(20))
				{
					mainpack = zMisc::randBetween(1 , 6);
				}
				if(zMisc::selectByPercent(10))
				{
					equippack = zMisc::randBetween(1 , 4);
				}
			}
			break;
		case Cmd::GOODNESS_7:
			{
				if(zMisc::selectByPercent(20))
				{
					mainpack = zMisc::randBetween(1 , 6);
				}
				if(zMisc::selectByPercent(20))
				{
					equippack = zMisc::randBetween(1 , 4);
				}
			}
			break;
	}
}
// */
if (mainpack>0) {
	packs.execEvery(&packs.main, Type2Type<DropFromPack>(), DropFromPack::Param(&packs.main, mainpack, getPos()));
}
if(equippack > 0){
	packs.execEvery(&packs.equip, Type2Type<DropFromPack>(), DropFromPack::Param(&packs.equip, equippack, getPos()));
	if(packs.equip.needRecalc)
	{
		setupCharBase();
		Cmd::stMainUserDataUserCmd  userinfo;
		full_t_MainUserData(userinfo.data);
		sendCmdToMe(&userinfo,sizeof(userinfo)); 
		sendMeToNine();
	}

}

#if 0
//TODOBYLQY
std::vector<zObject *> temp_vec;
int begin = 0;
std::set<zObject *>::iterator iter;
if(mainpack > 0)
{
	for(iter = packs.main.getAllset().begin(); iter != packs.main.getAllset().end() ; iter ++)
	{
		//TODO 其它不可掉落物品
		if((*iter)->data.upgrade > 5 || (*iter)->data.bind || (*iter)->data.pos.y == Cmd::EQUIPCELLTYPE_PACKAGE || (*iter)->data.pos.y == Cmd::EQUIPCELLTYPE_MAKE || (*iter)->base->kind==ItemType_MASK  || (*iter)->base->kind==ItemType_Quest)
		{
			continue;
		}
		temp_vec.push_back(*iter);
	}
	if(mainpack < (int)temp_vec.size())
	{
		begin = zMisc::randBetween(0 , temp_vec.size() - mainpack);
	}
	else
	{
		mainpack = temp_vec.size();
	}
	for(int i = begin; i < mainpack ;  i ++)
	{
		this->packs.moveObjectToScene(&*temp_vec[i] , this->getPos());
	}
}
if(equippack > 0)
{
	bool needRecalc = false;
	begin = 0;
	temp_vec.clear();
	for(iter = packs.equip.getAllset().begin(); iter != packs.equip.getAllset().end() ; iter ++)
	{
		//TODO 其它不可掉落物品
		if((*iter)->data.upgrade > 5 || (*iter)->data.bind || (*iter)->data.pos.y == Cmd::EQUIPCELLTYPE_PACKAGE || (*iter)->data.pos.y == Cmd::EQUIPCELLTYPE_MAKE  || (*iter)->base->kind==ItemType_Quest)
		{
			continue;
		}
		temp_vec.push_back(*iter);
	}
	if(equippack < (int)temp_vec.size())
	{
		begin = zMisc::randBetween(0 , temp_vec.size() - equippack);
	}
	else
	{
		equippack = temp_vec.size();
	}
	for(int i = begin; i < equippack ;  i ++)
	{
		this->packs.moveObjectToScene(&*temp_vec[i] , this->getPos());
		needRecalc = true;
	}
	if(needRecalc)
	{
		setupCharBase();
		Cmd::stMainUserDataUserCmd  userinfo;
		full_t_MainUserData(userinfo.data);
		sendCmdToMe(&userinfo,sizeof(userinfo));
		sendMeToNine();
	}
}
#endif
}

void SceneUser::showCurrentEffect(const WORD &state, bool isShow,bool notify)
{
	if (isShow)
	{
		#ifdef _DEBUGLOG
		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "[设置第%u号特效状态]",state);
		#endif
		if(this->setUState(state) && notify)
			this->setStateToNine(state);
	}
	else
	{
		#ifdef _DEBUGLOG
		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "[清除第%u号特效状态]",state);
		#endif
		if(this->clearUState(state) && notify)
		     /////////////////////////////////////////
			this->sendMeToNine();
			//////////////////////////////////////////
			this->clearStateToNine(state);
	}

}

void SceneUser::reSendMyMapData()
{
	if (SceneEntry_Hide!=getState()&&!this->hideme)
	{//检查是否隐身
		BUFFER_CMD(Cmd::stAddUserAndPosMapScreenStateUserCmd ,send , zSocket::MAX_USERDATASIZE);
		this->full_t_MapUserDataPosState(send->data);
		this->scene->sendCmdToNine(getPosI(),send,send->size());
		this->setStateToNine(Cmd::USTATE_WAR);
	}
	else
	{
		BUFFER_CMD(Cmd::stAddUserMapScreenStateUserCmd ,send , zSocket::MAX_USERDATASIZE);
		this->full_t_MapUserDataState(send->data);
		sendCmdToMe(send,send->size());
	}
}

/**
 * \brief 让角色重生
 */
void SceneUser::relive()
{
	relive(Cmd::ReliveHome , 0 , 100);
}

/**
 * \brief 角色被击退N格
 * \author fqnewman
 */
void SceneUser::standBack(const DWORD dwAttTempID, DWORD grids)
{
	SceneUser *att = SceneUserManager::getMe().getUserByTempID(dwAttTempID);
	if (att)
	{
		backOff(Scene::getCompDir(att->getPos(), this->pos), grids);
	}
	else
	{
		backOff(getDir(), grids);
	}
}

/**
 * \brief 让角色死亡
 * \author fqnewman
 */
void SceneUser::toDie(const DWORD &dwTempID)
{
	setDeathState();
}

void SceneUser::goToRandomScreen()
{
	this->goToRandomRect(this->pos, SCREEN_WIDTH*3, SCREEN_HEIGHT*3);
}

/**
 * \brief 通知客户端生命值的变化
 * \author fqnewman
 */
void SceneUser::attackRTHpAndMp()
{
	ScenePk::attackRTHpAndMp(this);
	notifyHMS = false;
}
DWORD SceneUser::autoRestitute(DWORD &updated)
{
	/*
	if (this->live_skills.skill_id) {
		//劳动中
		return updated;
	}*/

	if (!this->isAutoRestitute)
	{
		return updated;
	}

	//this->lock();
	if (this->getState() == SceneUser::SceneEntry_Death) 
    {
	/////////////////////////////////////////////////
		//this->unlock();
		return updated;
	}

    if ( isSpa() )
    {
	//////////////////////////////////////////////////
        return updated;
    }
	
	if ( isDance() )
    {
        return updated;
    }
	/*
	   Zebra::logger->debug("自动恢复hp(%d), mp(%d), sp(%d)", charstate.resumehp + packs.equip.getAllObject().hpr,
	   charstate.resumemp + packs.equip.getAllObject().mpr,
	   charstate.resumesp + packs.equip.getAllObject().spr
	   );
	   */
	//Zebra::logger->debug("用户跑动(%d)，走路(%d), run(%d), walk(%d)", step_state >> 8, step_state & 0x0ff, step_state & 0xff00, step_state & 0x00ff );

	if (!this->IsPking())  {

		if (this->isSitdown())
		{
			if (sitdownRestitute)
			{
				DWORD tmp = charbase.hp;
				DWORD value = (DWORD)(((float)charstate.maxhp)*0.02f)+charstate.resumehp*2;
				if (value<4) value = 4;
				charbase.hp += value;

				if (charbase.hp>charstate.maxhp) charbase.hp = charstate.maxhp;
				if (tmp != charbase.hp) updated |= 0x1;

				tmp = charbase.mp;
				value = (DWORD)(((float)charstate.maxmp)*0.02f)+charstate.resumemp*2;
				if (value<6) value=6;
				charbase.mp +=  value;
				if (charbase.mp>charstate.maxmp) charbase.mp = charstate.maxmp;
				if(tmp != charbase.mp) updated |= 0x02;
			}

			sitdownRestitute = !sitdownRestitute;
		}
		else
		{
			sitdownRestitute = false;
			DWORD tmp = charbase.hp;
			if (step_state & 0xff00) 
            { //user run
				charbase.hp += (packs.equip.getEquips().get_hpr());
			}
            else if (step_state & 0x00ff) 
            { //user walk
				//走路时生命恢复减半
				charbase.hp += (DWORD)((charstate.resumehp) >> 1);
			}
            else 
            { //user rest
				charbase.hp += charstate.resumehp;
			}

			if (charbase.hp>charstate.maxhp) charbase.hp = charstate.maxhp;
			if (tmp != charbase.hp) updated |= 0x1;

			tmp = charbase.mp;
			charbase.mp +=  charstate.resumemp;
			if (charbase.mp>charstate.maxmp) charbase.mp = charstate.maxmp;
			if(tmp != charbase.mp) updated |= 0x02;
		}
	}

	DWORD tmp = charbase.sp;
	if (step_state & 0xff00) { //user run
		charbase.sp += (packs.equip.getEquips().get_spr() - RUN_CONSUME_SP);
		if ((int)charbase.sp <=1) charbase.sp =0;
	}else if (step_state & 0x00ff) { //user walk
		if (!this->IsPking()) charbase.sp +=  (DWORD)(/*charconst->resumesp*/ WALK_RESTITUTE_SP*(skillValue.spspeedup==0?1:(skillValue.spspeedup/100.0f)) + packs.equip.getEquips().get_spr());
	}else {
		if (!this->IsPking()) charbase.sp +=  (DWORD)(/*charconst->resumesp*/ REST_RESTITUTE_SP*(skillValue.spspeedup==0?1:(skillValue.spspeedup/100.0f)) + packs.equip.getEquips().get_spr());
	}
	if (charbase.sp>charstate.maxsp) charbase.sp = charstate.maxsp;
	if (tmp != charbase.sp) updated |= 0x04;

	step_state &= 0x0000;
	/*
	   if (updated) {
	//notify me
	Cmd::stSetHPAndMPDataUserCmd ret;
	ret.wdHP = charbase.hp;
	ret.wdMP = charbase.mp;
	ret.wdSP = charbase.sp;
	this->sendCmdToMe(&ret , sizeof(ret));
	}
	if (updated & 0x01) {
	//notify other
	this->team.sendtoTeamCharData(this);
	}
	*/
	return updated;
	//this->unlock();
}

void SceneUser::restitute()
{
	autoRestitute(updateNotify);
	leechdom.fresh(this , updateNotify);
	updateCount=(updateCount+1)%3;
	if (updateNotify&&updateCount==0) 
    {
		//notify me
		Cmd::stSetHPAndMPDataUserCmd ret;
		ret.dwHP = charbase.hp;
		ret.dwMP = charbase.mp;
		//ret.dwSP = charbase.sp;
		this->sendCmdToMe(&ret , sizeof(ret));

		this->team.sendtoTeamCharData(this);

		if ((updateNotify & 0x01) || (updateNotify & 0x02)) 
        {
			this->sendtoSelectedHpAndMp();
		}
		updateNotify = 0;
	}
}

/**
 * \brief 判断角色是否死亡
 * \author fqnewman
 * \return true为死亡
 */
bool SceneUser::isDie()
{
	if (this->getState() == SceneEntry_Death) return true;
	return false;
}


DWORD SceneUser::getLevel() const
{
	return charbase.level;
}

/**
 * \brief 需要的职业类型，决定可以使用的技能类型
 * \author fqnewman
 */
bool SceneUser::needType(const DWORD &needtype)
{
	return charbase.type == needtype;
}

/**
 * \brief 需要的职业类型，决定可以使用的技能类型
 * \author fqnewman
 */
bool SceneUser::addSkillToMe(zSkill *skill)
{
	return usm.addSkill(skill);
}

/**
 * \brief 是否有该技能需要的武器
 * \author fqnewman
 * \return true 有 false 没有
 */
bool SceneUser::needWeapon(DWORD skillid)
{
	bool bret = false;
	zObject *temp;

	zSkill *s = usm.findSkill(skillid);
	if(s)
	{
		if(packs.equip.getObjectByZone(&temp , 0 , Cmd::EQUIPCELLTYPE_HANDL))
		{
			if(temp)
			{
				if (0 != temp->data.dur)
				{
					WORD kind = temp->base->kind;
					bret = s->base->has_needweapon(kind);
				}
			}
		}
		if(!bret)
		{
			if(packs.equip.getObjectByZone(&temp , 0 , Cmd::EQUIPCELLTYPE_HANDR))
			{
				if(temp)
				{
					if (0 != temp->data.dur)
					{
						WORD kind = temp->base->kind;
						bret = s->base->has_needweapon(kind);
					}
				}
			}
			if (!bret)
			{
				bret = s->base->has_needweapon(65535);
			}
		}
	}
	return bret;
}

/**
 * \brief 获取武器提供的攻击力 
 * \param powerkind 攻击力种类
 0 最小物理攻击
 1 最大物理攻击
 2 最小法术攻击
 3 最大法术攻击
 * \author fqnewman
 * \return 攻击力
 */
SWORD SceneUser::getWeaponPower(int powerkind)
{
	zObject *temp;
	SWORD power = 0;
	if(packs.equip.getObjectByZone(&temp , 0 , Cmd::EQUIPCELLTYPE_HANDL))
	{
		if(temp)
		{
			if (0 != temp->data.dur)
			{
				switch (powerkind)
				{
					case 0:
						power += temp->data.pdamage;
						break;
					case 1:
						power += temp->data.maxpdamage;
						break;
					case 2:
						power += temp->data.mdamage;
						break;
					case 3:
						power += temp->data.maxmdamage;
						break;
				}
			}
		}
	}

	if(packs.equip.getObjectByZone(&temp , 0 , Cmd::EQUIPCELLTYPE_HANDR))
	{
		if(temp)
		{
			if (0 != temp->data.dur)
			{
				switch (powerkind)
				{
					case 0:
						power += temp->data.pdamage;
						break;
					case 1:
						power += temp->data.maxpdamage;
						break;
					case 2:
						power += temp->data.mdamage;
						break;
					case 3:
						power += temp->data.maxmdamage;
						break;
				}
			}
		}
	}

	return power;
}

/**
 * \brief 是否Pk区域
 * \param other PK相关人
 * \author fqnewman
 * \return true 是 false 否
 */
bool SceneUser::isPkZone(SceneEntryPk *other)
{

#ifdef	_DEBUGLOG 
	Channel::sendSys(tempid,Cmd::INFO_TYPE_GAME, "PK区域：%s",!(scene->checkZoneType(this->pos, ZoneTypeDef::ZONE_PK_SAFE) ||
				scene->checkZoneType(this->pos, ZoneTypeDef::ZONE_ABSOLUTE_SAFE))?"是":"不是PK区域");
#endif

	if (this->isSpecWar(Cmd::COUNTRY_FORMAL_DARE) && scene->checkZoneType(this->pos, ZoneTypeDef::ZONE_DARE_SAFE))
	{
		return false;
	}

	if (scene->checkZoneType(this->pos, ZoneTypeDef::ZONE_ABSOLUTE_SAFE))
	{
		/*	if (other)
			{
			SceneEntryPk *curEntry = other->getMaster();
			if (curEntry)
			{
			if (zSceneEntry::SceneEntry_Player == curEntry->getType())
			{
			SceneUser *pUser = (SceneUser *)curEntry;
			if (this->charbase.country == pUser->charbase.country && !isWar(pUser)) return false;
			}
			}
			}
			*/
		return false;
	}
	else if (scene->checkZoneType(this->pos, ZoneTypeDef::ZONE_PK_SAFE))
	{
		if (other)
		{
			SceneEntryPk *curEntry = other->getMaster();
			if (curEntry)
			{
				if (zSceneEntry::SceneEntry_Player == curEntry->getType())
				{
					SceneUser *pUser = (SceneUser *)curEntry;
					if (this->charbase.country == pUser->charbase.country && !isWar(pUser)) return false;
				}
			}
		}
		else
		{
			return false;
		}
	}

	return true;
	//	return !(scene->checkZoneType(this->pos, ZoneTypeDef::ZONE_PK_SAFE) ||
	//			scene->checkZoneType(this->pos, ZoneTypeDef::ZONE_ABSOLUTE_SAFE));
}

/**
 * \brief 依赖物品消耗型法术
 * \param object 消耗物品的类型
 * \param num 消耗物品的数量
 * \author fqnewman
 * \return true 消耗成功 false 失败
 */
bool SceneUser::reduce(const DWORD &object, const BYTE num)
{
	if (object >0)
	{
		if (!packs.equip.skillReduceObject(this , object, num))
		{
			//Channel::sendSys(this ,Cmd::INFO_TYPE_FAIL, "缺少材料,不能使用此技能.");
			return false;
		}
	}
	return true;
}

/**
 * \brief 检查可消耗物品是否足够
 * \param object 消耗物品的类型
 * \param num 消耗物品的数量
 * \author fqnewman
 * \return true 足够 false 不够
 */
bool SceneUser::checkReduce(const DWORD &object, const BYTE num)
{
	if (object>0)
	{
		if (!packs.equip.skillCheckReduceObject(this , object, num))
		{
			//Channel::sendSys(this ,Cmd::INFO_TYPE_FAIL, "缺少材料,不能使用此技能.");
			return false;
		}
	}
	return true;
}

/**
 * \brief 施放技能所导致的消耗MP,HP,SP
 * \param base 技能基本属性对象
 * \author fqnewman
 * \return true 消耗成功 false 失败
 */
bool SceneUser::doSkillCost(const zSkillB *base)
{
	if (angelMode) return true;
	if ((int)charbase.mp - (int)base->mpcost>=0)
	{
		if ((int)charbase.sp - (int)base->spcost>=0)
		{
			if ((int)charbase.hp - (int)base->hpcost>0)
			{
				charbase.mp -= base->mpcost;
				//目前不处理体力所以注释掉charbase.sp -= base->spcost;
				charbase.hp -= base->hpcost;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	//soke 检查是否有自动补血道具
	checkAutoHP();

	//检查是否有自动补魔道具
	checkAutoMP();

	ScenePk::attackRTHpAndMp(this);
	return true;
}

void SceneUser::checkAutoMP()
{
	if(charbase.mp <= charstate.maxmp*0.3)
	{
		DWORD temp=charstate.maxmp-charbase.mp + 49;
		DWORD dur=temp/50;
		DWORD reduce=packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_ADORN , ItemType_GreatLeechdomMp , dur , true , false);
		if(!reduce)
		{
			reduce=packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_ADORN + 1 , ItemType_GreatLeechdomMp , dur , true,false);
		}
		if(reduce)
		{
			charbase.mp += (reduce*50);
			if(charbase.mp > charstate.maxmp)
			{
				charbase.mp=charstate.maxmp;
			}
		}
	}
}

//soke 自动补血 类型 72
void SceneUser::checkAutoHP()
{
	if(charbase.hp <= charstate.maxhp*0.3)
	{
		DWORD temp=charstate.maxhp-charbase.mp + 49;
		DWORD dur=temp/50;
		DWORD reduce=packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_ADORN , ItemType_GreatLeechdomHp , dur , true , false);
		if(!reduce)
		{
			reduce=packs.equip.reduceDur(this , Cmd::EQUIPCELLTYPE_ADORN + 1 , ItemType_GreatLeechdomHp , dur , true,false);
		}
		if(reduce)
		{
			charbase.hp += (reduce*50);
			if(charbase.hp > charstate.maxhp)
			{
				charbase.hp=charstate.maxhp;
			}
		}
	}
}

/**
 * \brief 检查施放技能所导致的消耗MP,HP,SP是否足够
 * \param base 技能基本属性对象
 * \author fqnewman
 * \return true 消耗成功 false 失败
 */
bool SceneUser::checkSkillCost(const zSkillB *base)
{
	if (angelMode) return true;
	if ((int)base->mpcost == 0 ||((int)charbase.mp - (int)base->mpcost>=0))
	{
		//		if ((int)charbase.sp - (int)base->spcost>=0)
		//		{
		if ((int)base->hpcost ==0 || ((int)charbase.hp - (int)base->hpcost>0))
		{
			return true;
		}
		else
		{
			return false;
		}
		//		}
		//		else
		//		{
		//			return false;
		//		}
	}
	else
	{
		return false;
	}
	return true;
}

/**
 * \brief 检查自身的施放成功几率，决定这次技能是否可以施放
 * \author fqnewman
 * \return true 成功 false 失败
 */
bool SceneUser::checkPercent()
{
	//法术不命中
	//	if(getMagicType())
	//	{
	/*	if(!zMisc::selectByPercent(charstate.attackrating+skillValue.atrating))
		{
		return false;
		}*/
	//	}
	//	else
	//	{
	//		if(!zMisc::selectByPercent(charstate.magicrating+skillValue.atrating))
	//		{
	//			return false;
	//		}
	//	}
	return true;
}

void SceneUser::processMaskOnAttack(SceneEntryPk *pDef)
{
	mask.on_attack((SceneUser *)pDef);
}

void SceneUser::processMaskOnDefence()
{
	mask.on_defence();
}

void SceneUser::processAddDam(int &dwDam, int &dwDamDef, bool physics)
{
/* 	//计算增加伤害率
	if(this->packs.equip.getEquips().get_damage()> 0)
	{
		// 对防御者的伤害+=伤害值*伤害率
		dwDamDef += (int)(dwDam * (this->packs.equip.getEquips().get_damage() / 100.0f));
	}
//	Zebra::logger->debug("根据装备的伤害加深值计算出来的结果累加值dwDamDef:%ld，get_damage() ===%d", dwDamDef,this->packs.equip.getEquips().get_damage());
	if(this->packs.equip.getEquips().get_bdam()> 0)
	{
		// 对防御者的伤害+=伤害值*伤害率
		dwDamDef += (int)(dwDam * (this->packs.equip.getEquips().get_bdam() / 100.0f));
	} */

	if(this->packs.equip.getEquips().get_damage()> 0)
	{
		// 对防御者的伤害+=伤害值*伤害率
		dwDamDef += (int)(dwDamDef * (this->packs.equip.getEquips().get_damage() / 100.0f));
	}
	if(this->packs.equip.getEquips().get_bdam()> 0)
	{
		// 对防御者的伤害+=伤害值*伤害率
		dwDamDef += (int)(dwDamDef * (this->packs.equip.getEquips().get_bdam() / 100.0f));
	}
#ifdef _DEBUGLOG 
	Zebra::logger->debug("根据套装装备的伤害加深值计算出来的结果累加值dwDamDef:%ld,get_bdam()==%d", dwDamDef,this->packs.equip.getEquips().get_bdam());
#endif
}

void SceneUser::reduceDam(int &dwDam, int &dwDamDef, bool physics)
{
	if (physics)
	{
		//计算减物理少伤害值
		if(this->packs.equip.getEquips().get_dhpp() > 0)
		{
			dwDamDef -= (int)(dwDam * (this->packs.equip.getEquips().get_dhpp() / 100.0f));
		}
#ifdef _DEBUGLOG 
		Zebra::logger->debug("根据物理减少伤害值计算出来的结果累加值dwDamDef:%ld,get_dhpp() ====%d", dwDamDef,this->packs.equip.getEquips().get_dhpp());
#endif
		//计算减物理少伤害值
		if(this->packs.equip.getEquips().get_dpdam() > 0)
		{
			dwDamDef -= (int)(dwDam * (this->packs.equip.getEquips().get_dpdam() / 100.0f));
		}
#ifdef _DEBUGLOG 
		Zebra::logger->debug("根据五行套装物理减少伤害值计算出来的结果累加值dwDamDef:%ld,get_dpdam()===%d", dwDamDef,this->packs.equip.getEquips().get_dpdam());
#endif
	}
	else
	{
		//计算减法术少伤害值
		if(this->packs.equip.getEquips().get_dmpp() > 0)
		{
			dwDamDef -= (int)(dwDam * (this->packs.equip.getEquips().get_dmpp() / 100.0f));
		}
#ifdef _DEBUGLOG 
		Zebra::logger->debug("根据法术减少伤害值计算出来的结果累加值dwDamDef:%ld,get_dmpp() = %d", dwDamDef,this->packs.equip.getEquips().get_dmpp());
#endif
		//计算减法术少伤害值
		if(this->packs.equip.getEquips().get_dmdam() > 0)
		{
			dwDamDef -= (int)(dwDam * (this->packs.equip.getEquips().get_dmdam() / 100.0f));
		}
#ifdef _DEBUGLOG 
		Zebra::logger->debug("根据五行套装法术减少伤害值计算出来的结果累加值dwDamDef:%ld,get_dmdam() = %d", dwDamDef,this->packs.equip.getEquips().get_dmdam());
#endif
	}
}

//soke  伤害反射
void SceneUser::reflectDam(int &dwDamDef, int &dwDamSelf, bool physics)
{
	//计算伤害反射
	// 计算被攻击者身上装备对伤害的反弹

	if(this->packs.equip.getEquips().get_rdam() > 0)
	{
		dwDamSelf += (int)(dwDamDef * (this->packs.equip.getEquips().get_rdam() / 100.0f));
	}
#ifdef _DEBUGLOG 
	Zebra::logger->debug("根据装备的伤害反弹数值计算出来的结果dwDamSelf:%ld,装备总反射=%d,dwDamDef = %d", dwDamSelf,packs.equip.getEquips().get_rdam() ,dwDamDef);
#endif
	/*
	   if(this->packs.equip.getAllObject().reflect > 0)
	   {
	   dwDamSelf += (int)((float)this->charbase.level / this->packs.equip.getAllObject().reflect + 1.0f);
	   }
#ifdef _DEBUGLOG 
Zebra::logger->debug("根据装备的伤害反弹率计算出来的结果累加值dwDamSelf:%ld", dwDamSelf);
#endif
*/

	if(this->skillValue.reflect2 > 0)
	{
		dwDamSelf += (int)(dwDamDef * (this->skillValue.reflect2 / 100.0f ));
	}
#ifdef _DEBUGLOG 
	Zebra::logger->debug("根据技能的伤害反弹百分比2计算出来的结果dwDamSelf:%ld", dwDamSelf);
#endif
	if(this->skillValue.reflectp > 0)
	{
		dwDamSelf += (int)(dwDamDef * (this->skillValue.reflectp / 100.0f ));
	}
#ifdef _DEBUGLOG 
	Zebra::logger->debug("根据技能的伤害反弹百分比计算出来的结果dwDamSelf:%ld", dwDamSelf);
#endif
	if(this->skillValue.reflect > 0)
	{
		dwDamSelf += this->skillValue.reflect;
	}
#ifdef _DEBUGLOG 
	Zebra::logger->debug("根据技能的伤害反弹数值计算出来的结果累加值dwDamSelf:%ld", dwDamSelf);
#endif

	//soke 反射算法有问题  我打你100  你反射40% 结果：我掉血40 你掉血60===不应该是这样的

	//if (dwDamDef - dwDamSelf >=0)
	//{
	//	dwDamDef -=dwDamSelf;
	//}
	//else
	//{
	//	dwDamDef = 0;
	//}
}

void SceneUser::reflectSkill(SceneEntryPk *pAtt, const Cmd::stAttackMagicUserCmd *rev)
{
	Cmd::stAttackMagicUserCmd cmd;

	if (pAtt&& (skillValue.reflect_ardor>0))
	{
		memcpy(&cmd, rev, sizeof(cmd));

		switch (pAtt->getType())
		{
			case zSceneEntry::SceneEntry_Player:
				{
					cmd.byAttackType = Cmd::ATTACKTYPE_U2U;
				}
				break;
			case zSceneEntry::SceneEntry_NPC:
				{
					cmd.byAttackType = Cmd::ATTACKTYPE_U2N;
				}
				break;
			default:
				{
					cmd.byAttackType = Cmd::ATTACKTYPE_U2U;
				}
				break;
		}

		cmd.dwDefenceTempID = pAtt->tempid;
		cmd.dwUserTempID = this->tempid;
		cmd.wdMagicType = 351;
		cmd.byAction = Cmd::Ani_Null;

		zSkill *s = NULL;

		s = zSkill::createTempSkill(this , 351 , skillValue.reflect_ardor);
		if(s)
		{
			s->action(&cmd, sizeof(cmd));
			SAFE_DELETE(s);
		}
	}

	if (pAtt&& (skillValue.reflect_poison>0))
	{
		memcpy(&cmd, rev, sizeof(cmd));

		switch (pAtt->getType())
		{
			case zSceneEntry::SceneEntry_Player:
				{
					cmd.byAttackType = Cmd::ATTACKTYPE_U2U;
				}
				break;
			case zSceneEntry::SceneEntry_NPC:
				{
					cmd.byAttackType = Cmd::ATTACKTYPE_U2N;
				}
				break;
			default:
				{
					cmd.byAttackType = Cmd::ATTACKTYPE_U2U;
				}
				break;
		}

		cmd.dwDefenceTempID = pAtt->tempid;
		cmd.dwUserTempID = this->tempid;
		cmd.wdMagicType = 352; //李代桃僵
		cmd.byAction = Cmd::Ani_Null;

		zSkill *s = NULL;

		s = zSkill::createTempSkill(this , 352 , skillValue.reflect_poison); //李代桃僵
		if(s)
		{
			s->action(&cmd, sizeof(cmd));
			SAFE_DELETE(s);
		}
	}

	if (pAtt&& (skillValue.reflect_lull>0))
	{
		memcpy(&cmd, rev, sizeof(cmd));

		switch (pAtt->getType())
		{
			case zSceneEntry::SceneEntry_Player:
				{
					cmd.byAttackType = Cmd::ATTACKTYPE_U2U;
				}
				break;
			case zSceneEntry::SceneEntry_NPC:
				{
					cmd.byAttackType = Cmd::ATTACKTYPE_U2N;
				}
				break;
			default:
				{
					cmd.byAttackType = Cmd::ATTACKTYPE_U2U;
				}
				break;
		}

		cmd.dwDefenceTempID = pAtt->tempid;
		cmd.dwUserTempID = this->tempid;
		cmd.wdMagicType = 430;
		cmd.byAction = Cmd::Ani_Null;

		zSkill *s = NULL;

		s = zSkill::createTempSkill(this , 430 , skillValue.reflect_lull);
		if(s)
		{
			s->action(&cmd, sizeof(cmd));
			SAFE_DELETE(s);
		}
	}

	if (pAtt&& (skillValue.reflect_frost>0))
	{
		memcpy(&cmd, rev, sizeof(cmd));

		switch (pAtt->getType())
		{
			case zSceneEntry::SceneEntry_Player:
				{
					cmd.byAttackType = Cmd::ATTACKTYPE_U2U;
				}
				break;
			case zSceneEntry::SceneEntry_NPC:
				{
					cmd.byAttackType = Cmd::ATTACKTYPE_U2N;
				}
				break;
			default:
				{
					cmd.byAttackType = Cmd::ATTACKTYPE_U2U;
				}
				break;
		}

		cmd.dwDefenceTempID = pAtt->tempid;
		cmd.dwUserTempID = this->tempid;
		cmd.wdMagicType = 513;
		cmd.byAction = Cmd::Ani_Null;

		zSkill *s = NULL;

		s = zSkill::createTempSkill(this , 513 , skillValue.reflect_frost);
		if(s)
		{
			s->action(&cmd, sizeof(cmd));
			SAFE_DELETE(s);
		}
	}

	if (pAtt&& (skillValue.reflect_icelull>0))
	{
		memcpy(&cmd, rev, sizeof(cmd));

		switch (pAtt->getType())
		{
			case zSceneEntry::SceneEntry_Player:
				{
					cmd.byAttackType = Cmd::ATTACKTYPE_U2U;
				}
				break;
			case zSceneEntry::SceneEntry_NPC:
				{
					cmd.byAttackType = Cmd::ATTACKTYPE_U2N;
				}
				break;
			default:
				{
					cmd.byAttackType = Cmd::ATTACKTYPE_U2U;
				}
				break;
		}

		cmd.dwDefenceTempID = pAtt->tempid;
		cmd.dwUserTempID = this->tempid;
		cmd.wdMagicType = 632;
		cmd.byAction = Cmd::Ani_Null;

		zSkill *s = NULL;

		s = zSkill::createTempSkill(this , 632 , skillValue.reflect_icelull);
		if(s)
		{
			s->action(&cmd, sizeof(cmd));
			SAFE_DELETE(s);
		}
	}
}

bool SceneUser::processDeath(SceneEntryPk *pAtt)
{
	if (charbase.hp ==0 && getState()!=zSceneEntry::SceneEntry_Death)
	{
		//soke 死亡就停止飞行、挖宝
		endFly();
		endWabao();
		SceneEntryPk * m = pAtt->getTopMaster();
		if (!m) return false;

		if (m->getType() == zSceneEntry::SceneEntry_Player)
		{
			ScenePk::attackDeathUser((SceneUser*)m, this);
		}
		else if (m->getType() == zSceneEntry::SceneEntry_NPC)
		{
			Channel::sendSys(this ,Cmd::INFO_TYPE_FAIL,  "你被%s杀死了." , m->name);
			//lostObject(NULL);	//soke 任何情况下玩家死亡不丢装备;
			setDeathState();
			leaveBattle();
		}
		if (this->diewithme >0 && pAtt)
		{
			if (zMisc::selectByPercent(this->diewithme))
			{
				pAtt->toDie(0);
				this->skillStatusM.clearSkill(345);//清除血债血偿
			}
		}
		if (this->switchdie >0)
		{
			if (zMisc::selectByPercent(this->switchdie))
			{
				zPosVector range;
				this->scene->findEntryPosInOne(this->getPos(), this->getPosI(), range);
				if (range.size()==0) return true;
				int num = zMisc::randBetween(0,range.size()-1);
				zPos pd=range[num];
				SceneUser *pUser = this->scene->getSceneUserByPos(pd);
				if (pUser&&pUser!=this)
				{
					Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "你被转移的死亡状态击中");
					pUser->toDie(0);
					return true;
				}
				SceneNpc *pNpc = this->scene->getSceneNpcByPos(pd);
				if (pNpc)
				{
					pNpc->toDie(0);
				}
			}
		}
		return true;
		/*
		   if (pAtt->getType() == zSceneEntry::SceneEntry_Player)
		   {
		   ScenePk::attackDeathUser((SceneUser*)pAtt, this);
		   }
		   else if (pAtt->getType() == zSceneEntry::SceneEntry_NPC)
		   {
		   SceneEntryPk * m = pAtt->getTopMaster();
		   if (!m)
		   {
		   Channel::sendSys(this ,Cmd::INFO_TYPE_FAIL,  "你被%s杀死了." , pAtt->name);
		   lostObject(NULL);
		   setDeathState();
		   leaveBattle();
		   }
		   else
		   return processDeath(m);
		   }
		   return true;
		   */
	}
	return false;
}

void SceneUser::hp2mp(int &dwDamDef)
{
	int dwMP =0;
	if(this->charbase.mp > 0 && this->packs.equip.getEquips().get_hptomp() > 0)
	{
		dwMP = (int)(dwDamDef * (this->packs.equip.getEquips().get_hptomp()  / 100.0f));
		if((int)this->charbase.mp > dwMP)
		{
			dwDamDef -= dwMP;
			this->charbase.mp -= dwMP;
		}
		else
		{
			dwDamDef -= this->charbase.mp;
			this->charbase.mp = 0;
		}
		this->checkAutoMP();
	}

}

/**
 * \brief 获取自己的主人，一般针对NPC而言，Player的主人是自己
 * \author fqnewman
 * \return NULL或者主人的对象指针
 */
SceneEntryPk *SceneUser::getMaster()
{
	return this;
}

SceneEntryPk *SceneUser::getTopMaster()
{
	return this;
}

#if 0
bool SceneUser::packsaddObject(zObject *srcObj,bool needFind , bool from_record,  bool calcflag)
{
	bool ret;
	stObjectLocation old = srcObj->data.pos;

	//if (needFind)
	//{
	//	srcObj->data.pos.dwLocation=Cmd::OBJECTCELLTYPE_COMMON;
	//	srcObj->data.pos.dwTableID=0;
	//}
	ret = packs.addObject(srcObj, needFind, from_record);

#if 0
	//TODOBYLQY
	//try left package
	if (!ret && needFind &&  /*srcObj->data.pos.dwLocation == Cmd::OBJECTCELLTYPE_COMMON &&*/ packs.equip.left) {
		srcObj->data.pos.dwLocation = Cmd::OBJECTCELLTYPE_PACKAGE;
		srcObj->data.pos.dwTableID = packs.equip.left->object()->data.qwThisID;
		ret = packs.addObject(srcObj, needFind, from_record);
	}

	//try right package
	if (!ret && needFind && /*srcObj->data.pos.dwLocation == Cmd::OBJECTCELLTYPE_PACKAGE &&*/ packs.equip.right) {
		srcObj->data.pos.dwLocation = Cmd::OBJECTCELLTYPE_PACKAGE;
		srcObj->data.pos.dwTableID = packs.equip.right->object()->data.qwThisID;
		ret = packs.addObject(srcObj, needFind, from_record);
	}
#endif

	if (!ret) {
		srcObj->data.pos = old;
		Zebra::logger->alarm("警告，用户(%s)添加物品(%s)失败!", name, srcObj->name);
	}
	/*
	//检查是否马匹
	if (ItemType_HORSE==srcObj->base->kind)
	{
	if (horse.horse())
	{
	horse.mount(false);
	horse.putAway();
	horse.horse(0);
	}
	horse.horse(srcObj->base->id);
	}
	*/
	//	recalcBySword(calcflag);
	return ret;
}
#endif

void SceneUser::sendExpToSept(const WORD &exp)
{
	if (charbase.septid !=0)
	{
		Cmd::Session::t_distributeSeptExp_SceneSession send;
		send.dwExp = (DWORD)(exp*(0.02f));
		if (send.dwExp == 0) send.dwExp=1;
		send.dwUserID = id;
		sessionClient->sendCmd(&send, sizeof(send));
	}
}

/**
 * \brief 广播最新属性
 * \author fqnewman
 */
void SceneUser::changeAndRefreshHMS(bool lock, bool sendData)
{
	//Zebra::logger->debug("sendData=%d,reSendOther=%d,getMyMoveSpeed()=%d",sendData,reSendOther,getMyMoveSpeed());
	setupCharBase(lock);
	if (sendData)
	{
		Cmd::stMainUserDataUserCmd  userinfo;
		full_t_MainUserData(userinfo.data);
		sendCmdToMe(&userinfo,sizeof(userinfo));
		sendInitHPAndMp();
		if(reSendOther)
		{
			sendMeToNine();
		}
	}
	this->reSendData = false;
	this->reSendOther=false; 
}

int SceneUser::saveSysSetting(unsigned char * buf)
{
	int size = 0;
	sysSetting[0] = pkMode; // 将PK模式保存回系统设置
	bcopy(sysSetting, buf+size, sizeof(sysSetting));
	size += sizeof(sysSetting);
	bcopy(chatColor, buf+size, sizeof(chatColor));
	size += sizeof(chatColor);

	return size;
}

int SceneUser::loadSysSetting( unsigned char * buf)
{
	int size = 0;
	bcopy(buf, sysSetting, sizeof(sysSetting));
	pkMode = sysSetting[0]; // 读取PK模式

	size += sizeof(sysSetting);
	bcopy(buf+size, &chatColor[0], sizeof(chatColor));
	size += sizeof(chatColor);

	//通知客户端
	Cmd::stSystemSettingsUserCmd sendClient;
	bcopy(sysSetting, &sendClient.data.bySettings, sizeof(sendClient.data.bySettings));
	bcopy(chatColor, &sendClient.data.dwChatColor, sizeof(sendClient.data.dwChatColor));
	sendCmdToMe(&sendClient, sizeof(sendClient));

	//通知session
	Cmd::Session::t_sysSetting_SceneSession send;
	strncpy((char *)send.name, name, MAX_NAMESIZE-1);
	bcopy(sysSetting, &send.sysSetting, sizeof(send.sysSetting));
	send.face = charbase.face;
	sessionClient->sendCmd(&send, sizeof(send));

	//通知网关
	Cmd::Scene::t_sysSetting_GateScene gate_send;
	bcopy(sysSetting, gate_send.sysSetting, sizeof(gate_send.sysSetting));
	gate_send.id=this->id;
	this->gatetask->sendCmd(&gate_send, sizeof(gate_send));

	return size;
}

void SceneUser::sendSevenStateToMe(DWORD state , WORD value , WORD time)
{
	using namespace Cmd;
	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	stSelectReturnStatesPropertyUserCmd *srs=(stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->states[0].state = state;
	srs->states[0].result = value;
	srs->states[0].time = time;
	srs->size=1;
	this->sendCmdToMe(srs , sizeof(stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]));
}
void SceneUser::sendtoSelectedState(DWORD state , WORD value , WORD time)
{
	/// 如果是易容术作用期间则不再发送数据
	if (this->issetUState(Cmd::USTATE_CHANGEFACE)) return;

	if (this->issetUState(Cmd::USTATE_ZHUTOU)) return;

	using namespace Cmd;
	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	stSelectReturnStatesPropertyUserCmd *srs=(stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->states[0].state = state;
	srs->states[0].result = value;
	srs->states[0].time = time;
	srs->size=1;
	this->team.sendCmdToTeam(this , srs , sizeof(stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]));
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]));
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}
//---------------------------------------------------------------------
void SceneUser::sendtoSelectedReliveWeakStateToUser(SceneUser *pUser)
{
	if(!pUser || this->charbase.reliveWeakTime == 0  || this->charbase.reliveWeakTime <= SceneTimeTick::currentTime.sec()%10000)
	{
		return;
	}
	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_RELIVEWEAK;
	srs->states[0].result=0;
	srs->states[0].time=this->charbase.reliveWeakTime-SceneTimeTick::currentTime.sec()%10000;
	pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
}
void SceneUser::sendtoSelectedReliveWeakState()
{
	if(this->charbase.reliveWeakTime == 0 || this->charbase.reliveWeakTime <= SceneTimeTick::currentTime.sec()%10000)
	{
		return;
	}
	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_RELIVEWEAK;
	srs->states[0].result=0;
	srs->states[0].time= this->charbase.reliveWeakTime-SceneTimeTick::currentTime.sec()%10000;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}

//---------------------------------------------------------------------
void SceneUser::sendtoSelectedPkAdditionStateToUser(SceneUser *pUser)
{
	if(!pUser || this->charbase.pkaddition <= 1800)
	{
		return;
	}
	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_PK;
	srs->states[0].result=this->getPkAddition();
	srs->states[0].time=0XFFFF;
	pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
}
void SceneUser::sendtoSelectedPkAdditionState()
{
	if(this->charbase.pkaddition <= 1800)
	{
		return;
	}
	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_PK;
	srs->states[0].result=this->getPkAddition();
	srs->states[0].time=0XFFFF;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}

//---------------------------------------------------------------------
void SceneUser::sendtoSelectedTrainStateToUser(SceneUser *pUser)
{
	if(!pUser || !this->charbase.trainTime) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_DAOJISHI;
	srs->states[0].result=charbase.trainTime/60;
	srs->states[0].time=charbase.trainTime/60;
	pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
}
void SceneUser::sendtoSelectedTrainState()
{
	if(!this->charbase.trainTime) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_DAOJISHI;
	srs->states[0].result=charbase.trainTime/60;
	srs->states[0].time=charbase.trainTime/60;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}

//soke  9朵花状态处理
void SceneUser::sendtoSelected9StateToUser(SceneUser *pUser)
{
	if(!pUser || !this->charbase.flower1time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_FLOWER_9;
	srs->states[0].result=charbase.flower1time/60;
	srs->states[0].time=charbase.flower1time/60;
	pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
}
void SceneUser::sendtoSelected9State()
{
	if(!this->charbase.flower1time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_FLOWER_9;
	srs->states[0].result=charbase.flower1time/60;
	srs->states[0].time=charbase.flower1time/60;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}

//soke 99朵花状态处理
void SceneUser::sendtoSelected99StateToUser(SceneUser *pUser)
{
	if(!pUser || !this->charbase.flower2time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_FLOWER_99;
	srs->states[0].result=charbase.flower2time/60;
	srs->states[0].time=charbase.flower2time/60;
	pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
}
void SceneUser::sendtoSelected99State()
{
	if(!this->charbase.flower2time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_FLOWER_99;
	srs->states[0].result=charbase.flower2time/60;
	srs->states[0].time=charbase.flower2time/60;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}

//soke 999朵花状态处理
void SceneUser::sendtoSelected999StateToUser(SceneUser *pUser)
{
	if(!pUser || !this->charbase.flower3time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_FLOWER_999;
	srs->states[0].result=charbase.flower3time/60;
	srs->states[0].time=charbase.flower3time/60;
	pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
}
void SceneUser::sendtoSelected999State()
{
	if(!this->charbase.flower3time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_FLOWER_999;
	srs->states[0].result=charbase.flower3time/60;
	srs->states[0].time=charbase.flower3time/60;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}

//soke 999朵花头顶状态处理
void SceneUser::sendtoSelectedyswcStateToUser(SceneUser *pUser)
{
	if(!pUser || !this->charbase.flower3time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_USER_MEILI;
	srs->states[0].result=charbase.flower3time/60;
	srs->states[0].time=charbase.flower3time/60;
	pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
}
void SceneUser::sendtoSelectedyswcState()
{
	if(!this->charbase.flower3time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_USER_MEILI;
	srs->states[0].result=charbase.flower3time/60;
	srs->states[0].time=charbase.flower3time/60;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}

//soke 9朵蓝色妖姬状态处理
void SceneUser::sendtoSelectedx9StateToUser(SceneUser *pUser)
{
	if(!pUser || !this->charbase.flower4time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_FLOWERL_9;
	srs->states[0].result=charbase.flower4time/60;
	srs->states[0].time=charbase.flower4time/60;
	pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
}
void SceneUser::sendtoSelectedx9State()
{
	if(!this->charbase.flower4time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_FLOWERL_9;
	srs->states[0].result=charbase.flower4time/60;
	srs->states[0].time=charbase.flower4time/60;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}

//soke 99朵蓝色妖姬状态处理
void SceneUser::sendtoSelectedx99StateToUser(SceneUser *pUser)
{
	if(!pUser || !this->charbase.flower5time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_FLOWERL_99;
	srs->states[0].result=charbase.flower5time/60;
	srs->states[0].time=charbase.flower5time/60;
	pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
}
void SceneUser::sendtoSelectedx99State()
{
	if(!this->charbase.flower5time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_FLOWERL_99;
	srs->states[0].result=charbase.flower5time/60;
	srs->states[0].time=charbase.flower5time/60;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}

//soke 999朵蓝色妖姬状态处理
void SceneUser::sendtoSelectedx999StateToUser(SceneUser *pUser)
{
	if(!pUser || !this->charbase.flower6time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_FLOWERL_999;
	srs->states[0].result=charbase.flower6time/60;
	srs->states[0].time=charbase.flower6time/60;
	pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
}
void SceneUser::sendtoSelectedx999State()
{
	if(!this->charbase.flower6time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_FLOWERL_999;
	srs->states[0].result=charbase.flower6time/60;
	srs->states[0].time=charbase.flower6time/60;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}

//soke 999朵蓝色妖姬头顶状态处理
void SceneUser::sendtoSelectedyswc2StateToUser(SceneUser *pUser)
{
	if(!pUser || !this->charbase.flower6time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_USER_RENQI;
	srs->states[0].result=charbase.flower6time/60;
	srs->states[0].time=charbase.flower6time/60;
	pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
}
void SceneUser::sendtoSelectedyswc2State()
{
	if(!this->charbase.flower6time) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_USER_RENQI;
	srs->states[0].result=charbase.flower6time/60;
	srs->states[0].time=charbase.flower6time/60;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}

//VIP头顶状态处理
void SceneUser::sendtoSelectedVIPStateToUser(SceneUser *pUser)
{
	if(this->charbase.charvip < 1) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	int vipllist[] = {Cmd::USTATE_VIP1,Cmd::USTATE_VIP2,Cmd::USTATE_VIP3,Cmd::USTATE_VIP4,Cmd::USTATE_VIP5,Cmd::USTATE_VIP6,Cmd::USTATE_VIP7,Cmd::USTATE_VIP8,Cmd::USTATE_VIP9,Cmd::USTATE_VIP10,Cmd::USTATE_VIP11,Cmd::USTATE_VIP12,Cmd::USTATE_VIP13,Cmd::USTATE_VIP14,Cmd::USTATE_VIP15,Cmd::USTATE_VIP16,Cmd::USTATE_VIP17,Cmd::USTATE_VIP18,Cmd::USTATE_VIP19,Cmd::USTATE_VIP20};
	for (int i = sizeof(vipllist) / sizeof(int); i > 0; i--)
	{
		if(this->issetUState(vipllist[i-1]))
		{
			srs->states[0].state = vipllist[i-1];
			break;
		}
	}
	srs->states[0].result=0;
	srs->states[0].time=0XFFFF;
	pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
}

void SceneUser::sendtoSelectedVIPState()
{
	if(this->charbase.charvip < 1) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	
	int vipllist[] = {Cmd::USTATE_VIP1,Cmd::USTATE_VIP2,Cmd::USTATE_VIP3,Cmd::USTATE_VIP4,Cmd::USTATE_VIP5,Cmd::USTATE_VIP6,Cmd::USTATE_VIP7,Cmd::USTATE_VIP8,Cmd::USTATE_VIP9,Cmd::USTATE_VIP10,Cmd::USTATE_VIP11,Cmd::USTATE_VIP12,Cmd::USTATE_VIP13,Cmd::USTATE_VIP14,Cmd::USTATE_VIP15,Cmd::USTATE_VIP16,Cmd::USTATE_VIP17,Cmd::USTATE_VIP18,Cmd::USTATE_VIP19,Cmd::USTATE_VIP20};
	for (int i = sizeof(vipllist) / sizeof(int); i > 0; i--)
	{
		if(this->issetUState(vipllist[i-1]))
		{
			srs->states[0].state = vipllist[i-1];
			break;
		}
	}
	
	srs->states[0].result = 0;
	srs->states[0].time=0XFFFF;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}



//部分头顶状态处理
void SceneUser::sendtoSelectedTaiMiaoToUser(SceneUser *pUser)
{
	//太庙图标处理
	DWORD pearlType = getFetchedPearlType();
    if ( pearlType > 0 )
    {
        // this->setUState(getUStateByPearlType(pearlType));
		showCurrentEffect( getUStateByPearlType(pearlType), true );
        char Buf[200]; 
        bzero(Buf , sizeof(Buf));
        Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
        constructInPlace(srs);
        srs->byType = Cmd::MAPDATATYPE_USER;
        srs->dwTempID = this->tempid;
        srs->size=1;
        srs->states[0].state = getUStateByPearlType(pearlType);
        srs->states[0].result=0;
        srs->states[0].time=0XFFFF;
		pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	}

	//国王图标处理
	int tempstate;
	if (this->emperor)//皇帝
		tempstate = Cmd::USTATE_EMPEROR;
	else if (this->king )//国王
		tempstate = Cmd::USTATE_KING;		
	else if(strstr(this->caption , "城主") != NULL)
		tempstate = Cmd::USTATE_UNIONMASTER;
	else
		return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = tempstate;
	srs->states[0].result=0;
	srs->states[0].time=0XFFFF;
	pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
}


//国王 皇帝 官员 城主  头顶状态处理
void SceneUser::sendtoSelectedOfficialtate()
{
	int tempstate;
	if (this->emperor)//皇帝
		tempstate = Cmd::USTATE_EMPEROR;
	else if (this->king )//国王
		tempstate = Cmd::USTATE_KING;		
	else if(strstr(this->caption , "城主") != NULL)
		tempstate = Cmd::USTATE_UNIONMASTER;
	else
		return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = tempstate;
	srs->states[0].result=0;
	srs->states[0].time=0XFFFF;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}


//战车状态处理
void SceneUser::sendtoZhancheSelectedOfficialtate1()
{
	if(!this->charbase.zhanchetime1) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_USER_ZC_TYPE1;
	srs->states[0].result=charbase.zhanchetime1/60;
	srs->states[0].time=charbase.zhanchetime1/60;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}


//战车状态处理
void SceneUser::sendtoZhancheSelectedOfficialtate2()
{
	if(!this->charbase.zhanchetime2) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_USER_ZC_TYPE2;
	srs->states[0].result=charbase.zhanchetime2/60;
	srs->states[0].time=charbase.zhanchetime2/60;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}


//战车状态处理
void SceneUser::sendtoZhancheSelectedOfficialtate3()
{
	if(!this->charbase.zhanchetime3) return;

	char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = Cmd::USTATE_USER_ZC_TYPE3;
	srs->states[0].result=charbase.zhanchetime3/60;
	srs->states[0].time=charbase.zhanchetime3/60;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}

void SceneUser::sendtoSelectedHpAndMp()
{
	Cmd::stRTSelectedHpMpPropertyUserCmd ret;
	ret.byType = Cmd::MAPDATATYPE_USER;
	ret.dwTempID = this->tempid;//临时编号
	ret.dwHP = this->charbase.hp;//当前血
	ret.dwMaxHp = this->charstate.maxhp;//最大hp
	ret.dwMP = this->charbase.mp;//当前mp
	ret.dwMaxMp = this->charstate.maxmp;//最大mp
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(&ret ,sizeof(ret));
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
	//selected_lock.unlock();
}

/**
 * \brief 获取装备伤害加成
 * \author fqnewman
 * \return 伤害加成
 */
WORD SceneUser::getDamageBonus()
{
	return packs.equip.getEquips().get_damagebonus();
}

/**
 * \brief 检查武器类型是否匹配
 * \param weaponType 武器类型
 * \return true武器类型符合，false武器类型不符合
 */
bool SceneUser::checkWeapon(BYTE weaponType)
{
	bool bret = false;
	zObject *temp;
	if(packs.equip.getObjectByZone(&temp , 0 , Cmd::EQUIPCELLTYPE_HANDL))
	{
		if(temp)
		{
			bret = (weaponType == temp->base->kind);
		}
	}
	if(!bret)
	{
		if(packs.equip.getObjectByZone(&temp , 0 , Cmd::EQUIPCELLTYPE_HANDR))
		{
			if(temp)
			{
				bret = (weaponType == temp->base->kind);
			}
		}
	}
	return bret;
}

void SceneUser::leaveTeam()
{
	if (team.getLeader())
	{
		if (team.getLeader() == this->tempid)
		{//如果自己是队长则解散队伍
			this->team.deleteTeam(this);
		}
		else
		{
			Cmd::stRemoveTeamMemberUserCmd cmd;
			cmd.dwTeamID = team.getLeader();
			cmd.dwTempID = this->tempid;
			SceneUser *leader=SceneUserManager::getMe().getUserByTempID(cmd.dwTeamID);

			if (leader)
			{
				this->team.setLeader(0);
				leader->team.removeMember(leader , (Cmd::stRemoveTeamMemberUserCmd*)&cmd);

				if(leader->team.getSize() == 1)
				{
					leader->team.deleteTeam(leader);
				}
			}
		}
	}
}

/**
 * \brief 获得武器类型
 * \return 武器类型
 */
BYTE SceneUser::getWeaponType()
{
	BYTE weaponType = 0;
	zObject *temp;
	if(packs.equip.getObjectByZone(&temp , 0 , Cmd::EQUIPCELLTYPE_HANDL))
	{
		if(temp)
		{
			weaponType = temp->base->kind;
		}
		if (weaponType <104 || weaponType >111)
		{
			weaponType = 0;
		}
	}
	if(0 == weaponType)
	{
		if(packs.equip.getObjectByZone(&temp , 0 , Cmd::EQUIPCELLTYPE_HANDR))
		{
			if(temp)
			{
				weaponType = temp->base->kind;
			}
		}
	}
	return weaponType;
}

void SceneUser::setPetsChaseTarget(SceneEntryPk *entry)
{
#ifdef _XWL_DEBUG
	//Zebra::logger->debug("SceneUser::setPetsChaseTarget(): lock %s", name);
#endif
	for (std::list<ScenePet *>::iterator it=totems.begin(); it!=totems.end(); it++)
	{
		(*it)->setCurTarget(entry->tempid, entry->getType(), true);
	}
	if (pet) pet->setCurTarget(entry->tempid, entry->getType(), true);
	if (summon) summon->setCurTarget(entry->tempid, entry->getType(), true);
#ifdef _XWL_DEBUG
	//Zebra::logger->debug("SceneUser::setPetsChaseTarget(): unlock %s", name);
#endif
}

/**
 * \brief 获取对象实体
 * \param entryType 目标类型
 * \param entryID 目标类型
 * \return 对象实体或NULL
 */
SceneEntryPk* SceneUser::getSceneEntryPk(DWORD entryType, DWORD entryID) const
{	
	switch (entryType)	
	{		
		case zSceneEntry::SceneEntry_Player:			
			{				
				return SceneUserManager::getMe().getUserByTempID(entryID);			
			}			
			break;		
		case zSceneEntry::SceneEntry_NPC:
			{
				return SceneNpcManager::getMe().getNpcByTempID(entryID);
			}
			break;
		default:
			{
				return NULL;
			}
			break;
	}
	return NULL;
}

/**
 * \brief 获取抗毒增加  
 */
SWORD SceneUser::getPoisondef()
{
	//SceneEntryPk *temp = getSceneEntryPk(curMagicManType, curMagicManID);
	return (SWORD)(packs.equip.getEquips().get_poisondef());//-(temp?temp->getPoison():0));
}

/**
 * \brief 获取抗麻痹增加        
 */
SWORD SceneUser::getLulldef()
{
	//SceneEntryPk *temp = getSceneEntryPk(curMagicManType, curMagicManID);
	return (SWORD)(packs.equip.getEquips().get_lulldef());//-(temp?temp->getLull():0));
}

/**
 * \brief 获取抗眩晕增加        
 */
SWORD SceneUser::getReeldef()
{
	//SceneEntryPk *temp = getSceneEntryPk(curMagicManType, curMagicManID);
	return (SWORD)(packs.equip.getEquips().get_reeldef());//-(temp?temp->getReel():0));
}

/**
 * \brief 获取抗噬魔增加        
 */
SWORD SceneUser::getEvildef()
{
	//SceneEntryPk *temp = getSceneEntryPk(curMagicManType, curMagicManID);
	return (SWORD)(packs.equip.getEquips().get_evildef());//-(temp?temp->getEvil():0));
}

/**
 * \brief 获取抗噬力增加        
 */
SWORD SceneUser::getBitedef()
{
	//SceneEntryPk *temp = getSceneEntryPk(curMagicManType, curMagicManID);
	return (SWORD)(packs.equip.getEquips().get_bitedef());//-(temp?temp->getBite():0));
}

/**
 * \brief 获取抗混乱增加        
 */
SWORD SceneUser::getChaosdef()
{
	//SceneEntryPk *temp = getSceneEntryPk(curMagicManType, curMagicManID);
	return (SWORD)(packs.equip.getEquips().get_chaosdef());//-(temp?temp->getChaos():0));
}

/**
 * \brief 获取抗冰冻增加        
 */
SWORD SceneUser::getColddef()
{
	//SceneEntryPk *temp = getSceneEntryPk(curMagicManType, curMagicManID);
	return (SWORD)(packs.equip.getEquips().get_colddef());//-(temp?temp->getCold():0));
}

/**
 * \brief 获取抗石化增加        
 */
SWORD SceneUser::getPetrifydef()
{
	//SceneEntryPk *temp = getSceneEntryPk(curMagicManType, curMagicManID);
	return (SWORD)(packs.equip.getEquips().get_petrifydef());//-(temp?temp->getPetrify():0));
}

/**
 * \brief 获取抗失明增加        
 */
SWORD SceneUser::getBlinddef()
{
	//SceneEntryPk *temp = getSceneEntryPk(curMagicManType, curMagicManID);
	return (SWORD)(packs.equip.getEquips().get_blinddef());//-(temp?temp->getBlind():0));
}

/**
 * \brief 获取抗定身增加        
 */
SWORD SceneUser::getStabledef()
{
	//SceneEntryPk *temp = getSceneEntryPk(curMagicManType, curMagicManID);
	return (SWORD)(packs.equip.getEquips().get_stabledef());//-(temp?temp->getStable():0));
}

/**
 * \brief 获取抗减速增加        
 */
SWORD SceneUser::getSlowdef()
{
	//SceneEntryPk *temp = getSceneEntryPk(curMagicManType, curMagicManID);
	return (SWORD)(packs.equip.getEquips().get_slowdef());//-(temp?temp->getSlow():0));
}

/**
 * \brief 获取抗诱惑增加
 */
SWORD SceneUser::getLuredef()
{
	//SceneEntryPk *temp = getSceneEntryPk(curMagicManType, curMagicManID);
	return (SWORD)(packs.equip.getEquips().get_luredef());//-(temp?temp->getLure():0));
}

/**
 * \brief 获取毒增加  
 */
SWORD SceneUser::getPoison()
{
	return (SWORD)packs.equip.getEquips().get_poison();
}

/**
 * \brief 获取麻痹增加        
 */
SWORD SceneUser::getLull()
{
	return (SWORD)packs.equip.getEquips().get_lull();
}

/**
 * \brief 获取眩晕增加        
 */
SWORD SceneUser::getReel()
{
	return (SWORD)packs.equip.getEquips().get_reel();
}

/**
 * \brief 获取噬魔增加        
 */
SWORD SceneUser::getEvil()
{
	return (SWORD)packs.equip.getEquips().get_evil();
}

/**
 * \brief 获取噬力增加        
 */
SWORD SceneUser::getBite()
{
	return (SWORD)packs.equip.getEquips().get_bite();
}

/**
 * \brief 获取混乱增加        
 */
SWORD SceneUser::getChaos()
{
	return (SWORD)packs.equip.getEquips().get_chaos();
}

/**
 * \brief 获取冰冻增加        
 */
SWORD SceneUser::getCold()
{
	return (SWORD)packs.equip.getEquips().get_cold();
}

/**
 * \brief 获取石化增加        
 */
SWORD SceneUser::getPetrify()
{
	return (SWORD)packs.equip.getEquips().get_petrify();
}

/**
 * \brief 获取失明增加        
 */
SWORD SceneUser::getBlind()
{
	return (SWORD)packs.equip.getEquips().get_blind();
}

/**
 * \brief 获取定身增加        
 */
SWORD SceneUser::getStable()
{
	return (SWORD)packs.equip.getEquips().get_stable();
}

/**
 * \brief 获取减速增加        
 */
SWORD SceneUser::getSlow()
{
	return (SWORD)packs.equip.getEquips().get_slow();
}

/**
 * \brief 获取诱惑增加
 */
SWORD SceneUser::getLure()
{
	return (SWORD)packs.equip.getEquips().get_lure();
}

/**
 * \brief 设置回收用户内存时间
 * \return 时间回收内存时间
 * */
zRTime& SceneUser::waitRecycle()
{
	zRTime ct;
	recycle_delay = ct;
	recycle_delay.addDelay(3000);
	return recycle_delay;
}

/**
 * \brief 易容处理
 * \param cmd 易容消息
 * \param cmdLen 消息长度
 * \return true 处理成功 false 失败
 **/
bool SceneUser::changeFace(const Cmd::stChangeFaceMapScreenUserCmd *cmd, const unsigned int cmdLen)
{
	dwChangeFaceID = cmd->dwChangeFaceID;
	return scene->sendCmdToNine(getPosI(),cmd,cmdLen);
}

/**
 * \brief 执行拍卖相关的指令
 *
 *
 * \param rev 消息指针
 * \param cmdLen 指令长度
 * \return 是否执行成功
 */
bool SceneUser::doAuctionCmd(const Cmd::stAuctionUserCmd *cmd,unsigned int cmdLen)
{
	if (!(atoi(Zebra::global["service_flag"].c_str())&Cmd::Session::SERVICE_AUCTION))
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，拍卖功能暂时关闭，请留意官方公告");
		return true;
	}
	using namespace Cmd;
	using namespace Cmd::Session;
	using namespace Cmd::Record;

	/*
	//检查是不是在访问拍卖npc
	SceneNpc * npc = SceneNpcManager::getMe().getNpcByTempID(npc_dwNpcTempID);
	if (!npc)
	{
	Zebra::logger->trace("[拍卖]%s 非法拍卖 npcID=%u npcTempID=%u", name, npc_dwNpcDataID, npc_dwNpcTempID);
	return false;
	}
	if (scene != npc->scene)
	{
	Zebra::logger->trace("[拍卖]%s 非法拍卖，不在同一场景", name);
	return false;
	}
	if (!scene->zPosShortRange(npc->getPos(), getPos(), SCREEN_WIDTH, SCREEN_HEIGHT))
	{
	Zebra::logger->trace("[拍卖]%s 非法拍卖，距离太远", name);
	return false;
	}
	*/
	/*
	   if (npc->npc->kind!=NPC_TYPE_MAILBOX)
	   {
	   Zebra::logger->trace("[拍卖]%s 非法拍卖，npc类型错误", name);
	   return false;
	   }
	   */

	switch (cmd->byParam)
	{
		case SALE_AUCTION_PARA:
			{
				if (charbase.level<40)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，40级以上才可以拍卖物品");
					return true;
				}

				stSaleAuction * rev = (stSaleAuction *)cmd;
#ifndef _XWL_DEBUG
				if (rev->bidType!=0) return true;//暂时关闭金币拍卖

				rev->minGold = 0;//暂时关闭金币拍卖
				rev->maxGold = 0;//暂时关闭金币拍卖
#endif

				if ((rev->bidType!=0 && rev->bidType!=1)
						|| (rev->minMoney<1 && rev->minGold<1)
						|| rev->minMoney>10000000
						|| rev->maxMoney>10000000
						|| (0==rev->bidType && rev->minMoney>rev->maxMoney)
						|| (1==rev->bidType && rev->minGold>rev->maxGold)
						|| rev->itemID == 0
						|| rev->itemID == 0xffffffff)
				{
					Zebra::logger->alarm("[拍卖]%s(%ld)非法的拍卖信息",name,id);
					return true;
				}
				t_saleAuction_SceneSession sa;
				sa.userID = tempid;

				zObject* ob = packs.uom.getObjectByThisID(rev->itemID);
				if (!ob)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的包裹里没有该物品");
					Zebra::logger->alarm("[拍卖]%s(%ld)试图拍卖的物品不存在",name,id);
					return true;
				}
				if (!ob->canMail())
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你不能拍卖这件物品");
					return true;
				}

				switch (ob->base->kind)
				{
					case ItemType_Blade:
					case ItemType_Sword:

						sa.info.type = AUCTION_WEAPOM_SWORD;
						break;
					case ItemType_Axe:
					case ItemType_Hammer:
						sa.info.type = AUCTION_WEAPOM_AXE;
						break;
					case ItemType_Crossbow:
						sa.info.type = AUCTION_WEAPOM_BOW;
						break;
					case ItemType_Stick:
						sa.info.type = AUCTION_WEAPOM_STICK;
						break;
					case ItemType_Staff:
						sa.info.type = AUCTION_WEAPOM_WAND;
						break;
					case ItemType_Fan:
						sa.info.type = AUCTION_WEAPOM_FAN;
						break;
					case ItemType_Helm:
						sa.info.type = AUCTION_EQUIP_HEAD;
						break;
					case ItemType_ClothBody:
					case ItemType_FellBody:
					case ItemType_MetalBody:
						sa.info.type = AUCTION_EQUIP_BODY;
						break;
					case ItemType_Cuff:
						sa.info.type = AUCTION_EQUIP_WRIST;
						break;
					case ItemType_Shield:
						sa.info.type = AUCTION_EQUIP_SHIELD;
						break;
					case ItemType_Caestus:
						sa.info.type = AUCTION_EQUIP_WAIST;
						break;
					case ItemType_Shoes:
						sa.info.type = AUCTION_EQUIP_FOOT;
						break;
					case ItemType_Necklace:
						sa.info.type = AUCTION_ACCESSORY_NECKLACE;
						break;
					case ItemType_Fing:
						sa.info.type = AUCTION_ACCESSORY_RING;
						break;
					case ItemType_Adonment:
						sa.info.type = AUCTION_ACCESSORY_ADORNMENT;
						break;
					case ItemType_Book:
						switch (ob->base->maxsp)
						{
							case 1:
								sa.info.type = AUCTION_BOOK_FIGHTER;
								break;
							case 2:
								sa.info.type = AUCTION_BOOK_ARCHER;
								break;
							case 3:
								sa.info.type = AUCTION_BOOK_WIZARD;
								break;
							case 4:
								sa.info.type = AUCTION_BOOK_SUMMONER;
								break;
							case 5:
								sa.info.type = AUCTION_BOOK_PRIEST;
								break;
							default:
								sa.info.type = AUCTION_BOOK_FIGHTER;//不知道类型的分到战士组里
								break;
						}
						break;
					case ItemType_SpecialBook:
						sa.info.type = AUCTION_BOOK_SPECIAL;
						break;
					case ItemType_LevelUp:
					case ItemType_SOULSTONE:
						sa.info.type = AUCTION_OTHER_GEM;
						break;
					case ItemType_Pack:
					case ItemType_Scroll:
					case ItemType_Scrxll: //soke 新类型 15
					case ItemType_Move:
					case ItemType_CaptureWeapon:
					case ItemType_Tonic:
					case ItemType_Gift:
					case ItemType_MASK:
					case ItemType_Wedding:
					case ItemType_Auto:
					case ItemType_SkillUp:
					case ItemType_Renew:
					case ItemType_Repair:
						sa.info.type = AUCTION_OTHER_ITEM;
						break;
					case ItemType_Resource:
						sa.info.type = AUCTION_OTHER_MATERIAL;
						break;
					case ItemType_Leechdom:
						sa.info.type = AUCTION_OTHER_LEECHDOM;
						break;
					default:
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你不能拍卖这件物品");
						return true;
				}
				DWORD charge = 0;
				switch (ob->data.kind%8)
				{
					case 0:
					case 4:
						charge = ob->base->price*ob->data.dwNum/10;
						sa.info.quality = 0;
						break;
					case 1:
					case 5:
						charge = ob->base->price*ob->data.dwNum*3/2/10;//蓝色1.5倍
						sa.info.quality = 1;
						break;
					case 2:
					case 3:
						charge = ob->base->price*ob->data.dwNum*2/10;//金色2倍
						sa.info.quality = 2;
						break;
					case 6:
					case 7:
						charge = ob->base->price*ob->data.dwNum*4/10;//绿色4倍
						sa.info.quality = 4;
						break;
					default:
						Zebra::logger->alarm("[拍卖]%s 拍卖物品品质类型错误 kind=%u", name, ob->data.kind);
						return false;
						break;
				}
				if (charge<100) charge = 100;
				if (!packs.checkMoney(charge)
						|| !packs.removeMoney(charge,"拍卖扣除"))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的金钱不足");
					return true;
				}

				strncpy(sa.info.owner, name, MAX_NAMESIZE);
				sa.info.ownerID = id;
				sa.info.state = AUCTION_STATE_NEW;
				sa.info.charge = charge*3;
				strncpy(sa.info.itemName, ob->data.strName, MAX_NAMESIZE);
				//sa.info.type = ob->base->kind;
				sa.info.needLevel = ob->data.needlevel;
				sa.info.minMoney = rev->bidType?0:rev->minMoney;
				sa.info.maxMoney = rev->bidType?0:rev->maxMoney;
				sa.info.minGold = rev->bidType?rev->minGold:0;
				sa.info.maxGold = rev->bidType?rev->maxGold:0;
				zRTime ct;
				sa.info.startTime = ct.sec();
				sa.info.endTime = sa.info.startTime + 60*60*12;
				//sa.info.bidType = rev->bidType;
				sa.info.bidType = 0;//关闭金币拍卖
				sa.info.itemID = ob->data.qwThisID;//关闭金币拍卖

				packs.removeObject(ob, true, false); //notify but not delete
				ob->getSaveData((SaveObject *)&sa.item);
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,0,this->id,this->name,0,NULL,"拍卖",ob->base,ob->data.kind,ob->data.upgrade);
				zObject::destroy(ob);

				sessionClient->sendCmd(&sa, sizeof(sa));
				save(OPERATION_WRITEBACK);
			}
			break;
		case BID_AUCTION_PARA:
			{
				stBidAuction * rev = (stBidAuction *)cmd;

				if (!packs.checkMoney(rev->money))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的金钱不足");
					return true;
				}

				t_checkBidAuction_SceneSession cba;
				cba.userID = tempid;
				cba.auctionID = rev->auctionID;
				cba.money = rev->money;
				cba.gold = rev->gold;

				sessionClient->sendCmd(&cba, sizeof(cba));
			}
			break;
		case QUERY_AUCTION_PARA:
			{
				if (SceneTimeTick::currentTime.sec()<queryTime)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "正在检索，请5秒后重试");
					return true;
				}

				stQueryAuction * rev = (stQueryAuction *)cmd;

				/*
				   std::string s(rev->name);
				   char * filter = "`~!@#$%^&*;:'\",<.>/?-_=+\\|";
				   if (std::string::npos!=s.find(filter, 0, strlen(filter)))
				   {
				   Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "请输入正确的物品名字");
				   return true;
				   }
				   */
				if (strchr(rev->name, '\'')
						|| strchr(rev->name, ';')
						|| strchr(rev->name, '\"'))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "请输入正确的物品名字");
					return true;
				}

				t_queryAuction_SceneSession qa;
				qa.userID = tempid;
				qa.type = rev->type;
				strncpy(qa.name, rev->name, MAX_NAMESIZE);
				qa.quality = rev->type>30?0:rev->quality;//除了装备其他物品都不考虑品质
				qa.level = rev->level;
				qa.page = rev->page;

				sessionClient->sendCmd(&qa, sizeof(qa));

				queryTime = SceneTimeTick::currentTime.sec()+5;
			}
			break;
		case CANCEL_SALE_AUCTION_PARA:
			{
				stCancelSaleAuction * rev = (stCancelSaleAuction *)cmd;

				t_checkCancelAuction_SceneSession cca;
				cca.userID = tempid;
				cca.auctionID = rev->auctionID;

				sessionClient->sendCmd(&cca, sizeof(cca));
			}
			break;
		case GET_LIST_AUCTION_PARA:
			{
				stGetListAuction * rev = (stGetListAuction *)cmd;
				if (0==rev->list) break;

				t_getListAuction_SceneSession gla;
				gla.userID = tempid;
				gla.list = rev->list;

				sessionClient->sendCmd(&gla, sizeof(gla));
			}
			break;
		default:
			return false;
	}
	/*
	*/
	return false;
}

/**
 * \brief 执行监狱相关的指令
 *
 *
 * \param rev 消息指针
 * \param cmdLen 指令长度
 * \return 是否执行成功
 */
bool SceneUser::doPrisonCmd(const Cmd::stPrisonUserCmd *cmd,unsigned int cmdLen)
{
	//using namespace Cmd;
	switch (cmd->byParam)
	{
		case Cmd::OUT_PRISON_PARA:
			{
				//stOutPrison * rev = (stOutPrison *)cmd;

				//if (scene->getRealMapID()!=189) return false;
				if ((charbase.goodness&0x0000ffff)>0 && (charbase.goodness&0x0000ffff)<=MAX_GOODNESS)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "努力改造，提早出狱！");
					return true;
				}

				Scene * s = SceneManager::getInstance().getSceneByName("中立区·皇城");
				//Scene * s = SceneManager::getInstance().getSceneByID(tomapid);
				if (s)
				{
					//const zPos pos=wp->getRandDest()->pos;
					bool suc = changeMap(s,zPos(831,705));
					if (!suc)
					{
						Zebra::logger->error("%s PK值 %u，出狱失败，目的 %s (%d,%d)", name, charbase.goodness, s->name, pos.x, pos.y);
						return false;
					}
					else
						Zebra::logger->error("%s PK值 %u，出狱", name, charbase.goodness);

					return true;
				}
				else
				{
					//if (guard && guard->canMove()) saveGuard = true;//使镖车跟随指令使用者
					//if (adoptList.size()) saveAdopt = true;
					Cmd::Session::t_changeScene_SceneSession cmd;
					cmd.id = id;
					cmd.temp_id = tempid;
					cmd.x = 831;
					cmd.y = 705;
					cmd.map_id = 0;
					cmd.map_file[0] = '\0';
					strncpy((char *)cmd.map_name, "中立区·皇城", MAX_NAMESIZE);
					sessionClient->sendCmd(&cmd, sizeof(cmd));

					return true;
				}
			}
			break;
		case Cmd::BRIBE_PRISON_PARA:
			{
				Cmd::stBribePrison * rev = (Cmd::stBribePrison *)cmd;

				//检查是不是访问狱卒

				DWORD good = (charbase.goodness&0x0000ffff);
				if (good>0 && good<=MAX_GOODNESS)
				{
					if (!packs.checkMoney(rev->money) || !packs.removeMoney(rev->money, "贿赂狱卒"))
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "小样，没那么多钱就别来找我！");
						return true;
					}

					DWORD minus = std::min(charbase.goodness, rev->money/200);
					charbase.goodness -= minus;
					if (minus>0)
					{
						Channel::sendPrivate("狱卒", name, "嘿嘿，你真是大方，我让你提前%u分钟出去～", minus);
						Cmd::stMainUserDataUserCmd  userinfo;
						full_t_MainUserData(userinfo.data);
						sendCmdToMe(&userinfo,sizeof(userinfo));
						sendMeToNine();
					}
					else
						Channel::sendPrivate("狱卒", name, "好...  (%s大名鼎鼎，没想到出手这么寒碜)", name);
				}
				return true;
			}
			break;
		case Cmd::BAIL_PRISON_PARA:
			{
				Cmd::stBailPrison * rev = (Cmd::stBailPrison *)cmd;

				//检查是不是访问典狱官

				SceneUser * pUser = SceneUserManager::getMe().getUserByName(rev->name);
				if (!pUser)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "%s 现在不在线", rev->name);
					return true;
				}

				DWORD good = (pUser->charbase.goodness&0x0000ffff);
				if (good>0 && good<=MAX_GOODNESS)
				{
					if (!packs.checkMoney(rev->money)
							|| !packs.removeMoney(rev->money, "保释玩家"))
					{
						Channel::sendPrivate("狱卒", name, "小样，没那么多钱就别来找我！");
						return true;
					}

					DWORD minus = std::min(pUser->charbase.goodness, rev->money/200);
					pUser->charbase.goodness -= minus;
					if (minus>0)
					{
						Channel::sendPrivate("狱卒", name, "嘿嘿，你真是大方，我让 %s 提前%u分钟出去～", rev->name, minus);
						Channel::sendPrivate("狱卒", rev->name, "嘿嘿，%s 关照过了，我让你提前%u分钟出狱～", name, minus);
						Cmd::stMainUserDataUserCmd  userinfo;
						pUser->full_t_MainUserData(userinfo.data);
						pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
						pUser->sendMeToNine();
					}
					else
						Channel::sendPrivate("狱卒", name, "好...  (%s大名鼎鼎，没想到出手这么寒碜)", name);
				}
				return true;
			}
			break;
		case Cmd::LEAVE_PRISON_PARA:
			{
				//stOutPrison * rev = (stOutPrison *)cmd;

				//if (scene->getRealMapID()!=189) return false;
				if (charbase.punishTime>0)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "努力改造，提早出狱！你还有 %u 分钟的刑期", charbase.punishTime);
					return true;
				}

				Scene * s = SceneManager::getInstance().getSceneByName("中立区·皇城");
				//Scene * s = SceneManager::getInstance().getSceneByID(tomapid);
				if (s)
				{
					//const zPos pos=wp->getRandDest()->pos;
					bool suc = changeMap(s,zPos(831,705));
					if (!suc)
					{
						Zebra::logger->error("%s PK值 %u，出狱失败，目的 %s (%d,%d)", name, charbase.goodness, s->name, pos.x, pos.y);
						return false;
					}
					else
						Zebra::logger->error("%s PK值 %u，出狱", name, charbase.goodness);

					return true;
				}
				else
				{
					//if (guard && guard->canMove()) saveGuard = true;//使镖车跟随指令使用者
					//if (adoptList.size()) saveAdopt = true;
					Cmd::Session::t_changeScene_SceneSession cmd;
					cmd.id = id;
					cmd.temp_id = tempid;
					cmd.x = 831;
					cmd.y = 705;
					cmd.map_id = 0;
					cmd.map_file[0] = '\0';
					strncpy((char *)cmd.map_name, "中立区·皇城", MAX_NAMESIZE);
					sessionClient->sendCmd(&cmd, sizeof(cmd));

					return true;
				}
			}
			break;
		default:
			break;
	}

	return false;
}

/**
 * \brief 执行邮件相关的指令
 *
 *
 * \param rev 消息指针
 * \param cmdLen 指令长度
 * \return 是否执行成功
 */
bool SceneUser::doMailCmd(const Cmd::stMailUserCmd *rev,unsigned int cmdLen)
{
	using namespace Cmd;
	using namespace Cmd::Session;

	if (!(atoi(Zebra::global["service_flag"].c_str())&SERVICE_MAIL))
	{
		Channel::sendSys(this, INFO_TYPE_FAIL, "邮件服务已停止，请留意系统公告");
		return true;
	}
	//检查是不是在访问邮箱
	//if (rev->byParam!=SEND_MAIL_PARA)
	{
		//SceneNpc * npc = SceneNpcManager::getMe().getNpcByTempID(npc_dwNpcTempID);
		//if (!npc)
		//{
		//	Zebra::logger->trace("[邮件]%s 非法访问邮箱 npcID=%u npcTempID=%u", name, npc_dwNpcDataID, npc_dwNpcTempID);
		//	return false;
		//}
		//if (npc->npc->kind!=NPC_TYPE_MAILBOX)
		//{
		//	Zebra::logger->trace("[邮件]%s 非法访问邮箱，npc类型错误 %d", name, npc->npc->kind);
		//	return false;
		//}
		//if (scene != npc->scene)
		//{
		//	Zebra::logger->trace("[邮件]%s 非法访问邮箱，不在同一场景", name);
		//	return false;
		//}
		//if (!scene->zPosShortRange(npc->getPos(), getPos(), SCREEN_WIDTH, SCREEN_HEIGHT))
		//{
		//	Zebra::logger->trace("[邮件]%s 非法访问邮箱，距离太远", name);
		//	return false;
		//}
	}

	switch (rev->byParam)
	{
		case SEND_MAIL_PARA:
			{
				if (charbase.level<30)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，30级以上才可以发送邮件");
					return true;
				}
				if (isSendingMail)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，你发送邮件速度过快");
					Zebra::logger->alarm("[邮件]%s 发送邮件速度过快", name);
					return false;
				}
				stSendMail * cmd = (stSendMail *)rev;

				if (0!=cmd->sendGold || 0!=cmd->recvGold)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，不可以邮寄金币");
					return true;//关闭邮寄金币
				}

				if (0==strncmp(name, cmd->toName, MAX_NAMESIZE))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "不可以给自己发邮件");
					Zebra::logger->alarm("[邮件]%s(%ld)试图给自己发邮件",name,id);
					return true;
				}

				if (strchr(cmd->toName, '\'')
						|| strchr(cmd->toName, ';')
						|| strchr(cmd->toName, '\"'))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "请输入正确的角色名字");
					return true;
				}

				Cmd::Record::t_userExist_SceneRecord ue;
				ue.fromID = tempid;
				bcopy(rev, &ue.sm, sizeof(Cmd::stSendMail));
				recordClient->sendCmd(&ue, sizeof(ue));

				isSendingMail = true;
				return true;

				/*
				   Session::t_checkSend_SceneSession sm;

				   if (cmd->itemID && cmd->itemID!=INVALID_THISID)
				   {
				   zObject* ob = packs.uom.getObjectByThisID(cmd->itemID);
				   if (!ob)
				   {
				   Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的包裹里没有该物品");
				   Zebra::logger->alarm("[邮件]%s(%ld)试图邮寄的物品不存在",name,id);
				   return true;
				   }
				   if (!ob->canMail())
				   {
				   Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你不能邮寄这件物品");
				   return true;
				   }
				   sm.mail.accessory = 1;
				   }
				   if (cmd->sendMoney)
				   {
				   if (!packs.checkMoney(cmd->sendMoney+mail_postage))
				   {
				   Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的金钱不足,邮资需要50文");
				   return true;
				   }
				   sm.mail.accessory = 1;
				   }

				   sm.mail.state = MAIL_STATE_NEW;
				   strncpy(sm.mail.fromName, name, MAX_NAMESIZE);
				   strncpy(sm.mail.toName, cmd->toName, MAX_NAMESIZE);
				   strncpy(sm.mail.title, cmd->title, MAX_NAMESIZE);
				   sm.mail.type = MAIL_TYPE_MAIL;
				   zRTime ct;
				   sm.mail.createTime = ct.sec();
				   sm.mail.delTime = sm.mail.createTime + 60*60*24*7;
				   strncpy(sm.mail.text, cmd->text, 256);
				   sm.mail.sendMoney = cmd->sendMoney;
				   sm.mail.recvMoney = cmd->recvMoney;
				   sm.mail.sendGold = cmd->sendGold;
				   sm.mail.recvGold = cmd->recvGold;
				   sm.itemID = cmd->itemID;
				   sm.mail.itemGot = 0;
				   sessionClient->sendCmd(&sm,sizeof(sm));
				   return true;
				   */
			}
			break;
		case GET_LIST_MAIL_PARA:
			{
				t_getMailList_SceneSession gml;
				gml.tempID = tempid;
				sessionClient->sendCmd(&gml, sizeof(gml));
			}
			break;
		case OPEN_MAIL_PARA:
			{
				stOpenMail * cmd = (stOpenMail *)rev;

				t_openMail_SceneSession om;
				om.tempID = tempid;
				om.mailID = cmd->mailID;
				sessionClient->sendCmd(&om, sizeof(om));
			}
			break;
		case GET_ITEM_MAIL_PARA:
			{
				/*
				   if (isGetingMailItem)
				   {
				   Zebra::logger->alarm("[邮件]%s 收取附件速度过快", name);
				   Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，你点太快了");
				   return false;
				   }
				   */
				isGetingMailItem = true;

				stGetItemMail * cmd = (stGetItemMail *)rev;

				t_getMailItem_SceneSession gmi;
				gmi.space = packs.uom.space(this);
				gmi.tempID = tempid;
				gmi.mailID =  cmd->mailID;
				zObject *gold = packs.getGold();//银子
				gmi.money = gold?gold->data.dwNum:0;//金子
				gmi.gold =  charbase.gold;
				gmi.mailID =  cmd->mailID;
				sessionClient->sendCmd(&gmi, sizeof(gmi));
			}
			break;
		case DEL_MAIL_PARA:
			{
				stDelMail * cmd = (stDelMail *)rev;

				t_delMail_SceneSession dm;
				dm.tempID = tempid;
				dm.mailID =  cmd->mailID;
				sessionClient->sendCmd(&dm, sizeof(dm));
			}
			break;
		case TURN_BACK_MAIL_PARA:
			{
				stTurnBackMail * cmd = (stTurnBackMail *)rev;

				t_turnBackMail_SceneSession tm;
				tm.userID = tempid;
				tm.mailID =  cmd->mailID;
				sessionClient->sendCmd(&tm, sizeof(tm));
			}
			break;
		default:
			break;
	}
	return false;
}

/**
 * \brief 执行宠物相关的指令
 *
 *
 * \param rev 消息指针
 * \param cmdLen 指令长度
 * \return 是否执行成功
 */
bool SceneUser::doPetCmd(const Cmd::stPetUserCmd *rev,unsigned int cmdLen)
{
	using namespace Cmd;
	switch (rev->byParam)
	{
		case SETAI_PET_PARA:
			{
				stSetAIPetCmd * cmd = (stSetAIPetCmd *)rev;
				//Zebra::logger->debug("doPetCmd():%s 设置宠物AI type=%u, mode=%4x", name, cmd->type, cmd->mode);
				switch (cmd->type)
				{
					case PET_TYPE_PET:
						{
							if (!pet) return false;
							pet->setPetAI(cmd->mode);
						}
						break;
					case PET_TYPE_SUMMON:
						{
							if (!summon) return false;
							summon->setPetAI(cmd->mode);
						}
						break;
					default:
						Zebra::logger->error("doPetCmd SETAI_PET_PARA: 未知的宠物类型 %d", cmd->type);
						break;
				}
			}
			break;
		case REQUESTDATA_PET_PARA:
			{
				stRequestDataPetCmd * cmd = (stRequestDataPetCmd *)rev;
#ifdef _XWL_DEBUG
				Zebra::logger->debug("doPetCmd():  %s 请求宠物状态 type=%u, type=%u", name, cmd->type, cmd->type);
#endif
				switch (cmd->type)
				{
					case PET_TYPE_RIDE:
						{
							if (!horse.horse()) return true;
							stHorseDataPetCmd ret;
							ret.type = PET_TYPE_RIDE;
							ret.id = horse.data.horseid;
							horse.full_HorseDataStruct(&ret.data);
							sendCmdToMe(&ret, sizeof(ret));
						}
						break;
					case PET_TYPE_PET:
						{
							if (!pet) return true;
							stRefreshDataPetCmd ret;
							ret.type = PET_TYPE_PET;
							ret.id = pet->tempid;
							pet->full_PetDataStruct(ret.data);
							sendCmdToMe(&ret, sizeof(ret));
						}
						break;
					case PET_TYPE_SUMMON:
						{
							if (!summon) return true;
							stRefreshDataPetCmd ret;
							ret.type = PET_TYPE_SUMMON;
							ret.id = summon->tempid;
							summon->full_PetDataStruct(ret.data);
							sendCmdToMe(&ret, sizeof(ret));
						}
						break;
					default:
						{
							Zebra::logger->error("doPetCmd(): %s 请求宠物信息，错误的类型 %d", name, cmd->type);
						}
						break;
				}
			}
			break;
		case CHANGENAME_PET_PARA:
			{
				stChangeNamePetCmd * cmd = (stChangeNamePetCmd *)rev;
				//Zebra::logger->debug("doPetCmd():  %s 更改宠物名字 type=%u name=%s", name, cmd->type, cmd->name);
				switch (cmd->type)
				{
					case PET_TYPE_RIDE:
						{
						}
						break;
					case PET_TYPE_PET:
						{
							if (!pet) return true;

							strncpy(pet->name, cmd->name, MAX_NAMESIZE-1);
							strncpy(pet->petData.name, cmd->name, MAX_NAMESIZE-1);

							stRefreshDataPetCmd ret;
							ret.type = PET_TYPE_PET;
							ret.id = pet->tempid;
							pet->full_PetDataStruct(ret.data);
							sendCmdToMe(&ret, sizeof(ret));

							pet->sendPetDataToNine();
							/*
							   Cmd::stAddMapNpcMapScreenUserCmd addNpc;
							   pet->full_t_MapNpcData(addNpc.data);
							   scene->sendCmdToNine(getPosI(), &addNpc, sizeof(addNpc));
							   */
							//sendNineToMe();
						}
						break;
					case PET_TYPE_SUMMON:
						{
							if (!summon) return true;

							strncpy(summon->name, cmd->name, MAX_NAMESIZE-1);
							strncpy(summon->petData.name, cmd->name, MAX_NAMESIZE-1);

							stRefreshDataPetCmd ret;
							ret.type = PET_TYPE_SUMMON;
							ret.id = summon->tempid;
							summon->full_PetDataStruct(ret.data);
							sendCmdToMe(&ret, sizeof(ret));

							summon->sendPetDataToNine();
							/*
							   Cmd::stAddMapNpcMapScreenUserCmd addNpc;
							   summon->full_t_MapNpcData(addNpc.data);
							   scene->sendCmdToNine(getPosI(), &addNpc, sizeof(addNpc));
							//sendNineToMe();
							*/
						}
						break;
					default:
						{
							Zebra::logger->error("doPetCmd(): %s 设置宠物名字，错误的类型 %d", name, cmd->type);
							return true;
						}
						break;
				}
			}
			break;
		case CALLHORSE_PET_PARA:
			{
				//Zebra::logger->debug("%s 放出马", name);
				if (horse.horse())
				{
					if (ridepet)
						horse.putAway();
					else
						horse.comeOut();
					//horse.comeOut();
				}
				else
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你还没有马匹");
			}
			break;
		case PUTHORSE_PET_PARA:
			{
				//Zebra::logger->debug("%s 收起马", name);
				if (horse.horse())
				{
					if (ridepet)
						horse.putAway();
					else
						horse.comeOut();
				}
				else
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你还没有马匹");
			}
			break;
		case CALLFABAO_PET_PARA://展示法宝
		{
			stCallFabaoPetCmd * cmd = (stCallFabaoPetCmd *)rev;
			if(cmd->id==20000)
			{
				fabao = summonPet(9050,Cmd::PET_TYPE_XCARTOON2);//混天绫
			}
			else if(cmd->id==20001){
				fabao = summonPet(9050,Cmd::PET_TYPE_XCARTOON2);//金箍棒
			}
			else if(cmd->id==20002){
				fabao = summonPet(9050,Cmd::PET_TYPE_XCARTOON2);//金刚圈
			}
			else{
				fabao = summonPet(9050,Cmd::PET_TYPE_XCARTOON2);//八卦镜
			}
			
		}
		break;
		// case PUTFABAO_PET_PARA://收起法宝
		// {
		// 	if(!fabao)
		// 	{
		// 		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你还没有展示法宝");
		// 		return false;
		// 	}
		// 	fabao->toDie(0);
		// 	killOnePet(fabao);
		// }
		// break;
		case SETTARGET_PET_PARA:
			{
				stSetTargetPetCmd * cmd = (stSetTargetPetCmd *)rev;
				//Zebra::logger->debug("doPetCmd():  %s设置宠物攻击目标 type=%u name=%s", name, cmd->type, cmd->name);
				SceneEntryPk * target = NULL;
				if (0==cmd->targetType)//玩家
					target = scene->getUserByTempID(cmd->id);
				else if (1==cmd->targetType)//NPC
				{
					target = SceneNpcManager::getMe().getNpcByTempID(cmd->id);
					//if (captureIt((SceneNpc *)target))
					//	return true;
				}

#ifdef _XWL_DEBUG
				Zebra::logger->debug("%s 指定宠物攻击目标 type=%u id=%u", name, cmd->targetType, cmd->id);
#endif
				if (!target || zSceneEntry::SceneEntry_Normal!=target->getState())
					return true;

				switch (cmd->type)
				{
					case PET_TYPE_RIDE:
						{
						}
						break;
					case PET_TYPE_PET:
						{
							if (!pet) return true;
							pet->lockTarget = false;
							if (pet->setCurTarget(target, true))
								pet->lockTarget = true;
							else
								Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "无法攻击该目标或距离太远");
						}
						break;
					case PET_TYPE_SUMMON:
						{
							if (!summon) return true;
							summon->lockTarget = false;
							if (summon->setCurTarget(target, true))
								summon->lockTarget = true;
							else
								Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "无法攻击该目标或距离太远");
						}
						break;
					default:
						{
							return true;
						}
						break;
				}
			}
			break;
		default:
			{
				Zebra::logger->error("doPetCmd(): %s 未知的宠物消息类型 byParam=%d", name, rev->byParam);
				return true;
			}
	}
	return true;
}

/**
 * \brief 执行卡通宠物自动修理装备
 *
 * \param obj 物品指针
 */
void SceneUser::petAutoRepair(zObject *obj)
{
	if (cartoonList.empty()) return;

	bool can = false;

	//优先利用跟随的宠物
	if (cartoon && cartoon->getCartoonData().repair && cartoon->getCartoonData().time>=14400)
	{
		Cmd::t_CartoonData d = cartoon->getCartoonData();
		d.time -= 14400;
		cartoon->setCartoonData(d);
		cartoon->save(Cmd::Session::SAVE_TYPE_TIMETICK);

		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "你的宝宝 %s 帮你修理了所有装备，消耗其4小时修炼时间", cartoon->name);
		Zebra::logger->trace("[宠物]%s 的宝宝 %s(%u) 修理装备，消耗 14400 秒修炼时间，剩余 %u 秒", name, cartoon->name, cartoon->getCartoonID(), d.time);
		can = true;
	}

	if (!can) return;

	// obj->data.dur = obj->data.maxdur;
	RepairEquipUseGold repair(this);
	packs.equip.execEvery(repair);

	packs.equip.calcAll();
}

/**
 * \brief 执行卡通宠物相关的指令
 *
 *
 * \param cmd 消息指针
 * \param cmdLen 指令长度
 * \return 是否执行成功
 */
bool SceneUser::doCartoonCmd(const Cmd::stCartoonUserCmd *cmd,unsigned int cmdLen)
{
	using namespace Cmd;
	using namespace Cmd::Session;

	switch (cmd->byParam)
	{
		case REPAIR_CARTOON_PARA:
			{
				stRepairCartoonCmd * rev = (stRepairCartoonCmd *)cmd;
				if (cartoonList.find(rev->cartoonID)==cartoonList.end())
					return true;
				if (cartoonList[rev->cartoonID].repair == rev->repair)
					return true;

				if (cartoon && cartoon->getCartoonID()==rev->cartoonID)
				{
					t_CartoonData d = cartoon->getCartoonData();
					d.repair = rev->repair;
					cartoon->setCartoonData(d);

					cartoonList[rev->cartoonID] = d;
				}
				else return true;

				Cmd::stAddCartoonCmd a;
				a.isMine = true;
				a.cartoonID = rev->cartoonID;
				a.data = cartoonList[rev->cartoonID];
				sendCmdToMe(&a, sizeof(a));

				
				//恋恋处理自动修理问题?
				t_setRepairCartoon_SceneSession send;
				send.userID = id;
				send.cartoonID = rev->cartoonID;
				send.repair = rev->repair;
				sessionClient->sendCmd(&send, sizeof(send));
				

				Zebra::logger->trace("[自动修理]%s(%u) %s自动修理", name, id, rev->repair?"打开":"关闭");
				Zebra::logger->trace("[自动修理]%s(%u) %s自动修理", name, id, rev->repair?"打开":"关闭");
			}
			break;
		case SELL_ALL_CARTOON_PARA:
			{
				if (!cartoon) return true;

				PetPack *pack = (PetPack *)packs.getPackage(Cmd::OBJECTCELLTYPE_PET, 0);
				if (!pack) return false;

				if (pack->isEmpty()) return true;

				DWORD cartoonID = cartoon->getCartoonID();
				cartoonList[cartoonID] = cartoon->getCartoonData();
				if (cartoonList[cartoonID].sp<30)
				{
					Channel::sendSys(this, INFO_TYPE_FAIL, "%s 体力值不够了", cartoonList[cartoonID].name);
					return true;
				}

				cartoonList[cartoonID].sp -= 30;
				cartoon->setCartoonData(cartoonList[cartoonID]);

				struct SellPack : public PackageCallback
				{
					public:
						SellPack(Packages *p, SceneUser * u) : count(0), pPack(p), pUser(u)
						{ }

						virtual bool exec(zObject* p)
						{
							if (!p || !p->canMail()) return true;

							if(p->data.price>0)
							{
								DWORD price = get_sell_price_by_dur(p);
								DWORD real_price = 0;

								if(p->base->maxnum && (p->base->kind == ItemType_Arrow))
								{
									if(p->base->durability)
										real_price = (DWORD)(pUser->getGoodnessPrice((p->data.dwNum*price),false));
								}
								else
								{
									if(p->base->durability)
										real_price = (DWORD)(pUser->getGoodnessPrice((p->data.dwNum*price),false));
									else
										real_price = (DWORD)(pUser->getGoodnessPrice((p->data.dwNum*price),false));
								}

								if(p->base->id == 655)
								{
									if(p->base->durability)
										real_price = (DWORD)(((float)((p->data.dur+49)/50.0f)/((p->base->durability+49)/50.0f)) * 4000.0f);
								}

								count += real_price;
							}

							if (p->data.exp && p->base->kind != ItemType_Pack )
								pUser->addExp(p->data.exp);

							zObject::logger(p->createid,p->data.qwThisID,p->base->name,p->data.dwNum,p->data.dwNum,0,0,NULL,pUser->id,pUser->name,"pet_sell",NULL,0,0);

							pPack->removeObject(p, false, true);

							return true;
						}

						DWORD count;
					private:
						Packages * pPack;

						SceneUser * pUser;
				};

				SellPack sp(&packs, this);
				pack->execEvery(sp);

				if (sp.count)
				{
					packs.addMoney(sp.count, "宠物出售");
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "%s 帮你卖掉了一些道具", cartoonList[cartoonID].name);
					Zebra::logger->debug("%s 出售宠物包裹物品，获得金钱%u", name, sp.count);
					sendCmdToMe(cmd, cmdLen);//发回去清空包裹
				}
				else
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "没什么好卖的");

				return true;
			}
			break;
		case CHARGE_CARTOON_PARA:
			{
				stChargeCartoonCmd * rev = (stChargeCartoonCmd *)cmd;
				if (cartoonList.find(rev->cartoonID)==cartoonList.end())
					return true;
				if (cartoonList[rev->cartoonID].state == CARTOON_STATE_WAITING
						|| cartoonList[rev->cartoonID].state == CARTOON_STATE_ADOPTED)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "宠物不在你身边，不能充值");
					return true;
				}
				if (cartoonList[rev->cartoonID].time+rev->time > 25920000)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，一个宝宝最多可以充值7200小时");
					return true;
				}

				if (0==rev->time) return true;

				DWORD need = (rev->time%144)>72?1:0;
				need += rev->time/144;
				if(!packs.checkGold(need)||!packs.removeGold(need, "给宠物充值"))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，你的金币不足");
					return true;
				}

				//一定要先保存
				if (cartoon && cartoon->getCartoonID()==rev->cartoonID)
				{
					cartoon->save(Cmd::Session::SAVE_TYPE_TIMETICK);

					cartoonList[rev->cartoonID] = cartoon->getCartoonData();
					cartoonList[rev->cartoonID].time += rev->time;
					cartoon->setCartoonData(cartoonList[rev->cartoonID]);
				}
				else
				{
					Cmd::Session::t_saveCartoon_SceneSession send;
					strncpy(send.userName, name, MAX_NAMESIZE);
					send.type = Cmd::Session::SAVE_TYPE_TIMETICK;
					send.cartoonID = rev->cartoonID;
					send.data = cartoonList[rev->cartoonID];
					sessionClient->sendCmd(&send, sizeof(send));

					cartoonList[rev->cartoonID].time += rev->time;
				}

				t_chargeCartoon_SceneSession send;
				send.masterID = id;
				send.cartoonID = rev->cartoonID;
				send.time = rev->time;
				sessionClient->sendCmd(&send, sizeof(send));

				Cmd::stAddCartoonCmd a;
				a.isMine = true;
				a.cartoonID = rev->cartoonID;
				a.data = cartoonList[rev->cartoonID];
				sendCmdToMe(&a, sizeof(a));
			}
			break;
		case SALE_CARTOON_PARA:
			{
				stSaleCartoonCmd * rev = (stSaleCartoonCmd *)cmd;
				if (cartoonList.find(rev->cartoonID)==cartoonList.end())
					return true;
				if (cartoonList[rev->cartoonID].state == CARTOON_STATE_WAITING
						|| cartoonList[rev->cartoonID].state == CARTOON_STATE_ADOPTED)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "%s 不在你身边，不能出售", cartoonList[rev->cartoonID].name);
					return true;
				}

				if (cartoonList[rev->cartoonID].time>=600)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "%s 还有10分钟以上的修炼时间，请不要卖掉它", cartoonList[rev->cartoonID].name);
					return true;
				}

				PetPack *pack = (PetPack *)packs.getPackage(Cmd::OBJECTCELLTYPE_PET, 0);
				if (!pack) return false;
				if (!pack->isEmpty())
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "请先清空宠物包裹再出售宠物");
					return true;
				}

				t_saleCartoon_SceneSession send;
				send.userID = id;
				send.cartoonID = rev->cartoonID;
				sessionClient->sendCmd(&send, sizeof(send));
			}
			break;
		case REQUEST_LIST_CARTOON_PARA:
			{
				t_getListCartoon_SceneSession send;
				send.userID = id;
				sessionClient->sendCmd(&send, sizeof(send));
			}
			break;
		case BUY_CARTOON_PARA:
			{
				if (cartoonList.size()>=2)
				{
					//Channel::sendSys(this, INFO_TYPE_FAIL, "你的宠物太多了");
					Channel::sendSys(this, INFO_TYPE_FAIL, "你同时只能拥有两只替身宝宝，请先卖掉再购买");
					return true;
				}

				stBuyCartoonCmd * rev = (stBuyCartoonCmd *)cmd;

				DWORD needMoney = 100; //购买宠物价格100
				/*
				if (9005==rev->npcID)//情人节买丘比特
				{
					struct tm tv1;
					time_t timValue = time(NULL);
					zRTime::getLocalTime(tv1, timValue);
					if (tv1.tm_mon!=1||tv1.tm_mday!=14)
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "情人节已经过了，不能购买");
						return true;
					}
					needMoney = 5000;
				}
				*/

				if (!packs.checkMoney(needMoney) || !packs.removeMoney(needMoney,"购买替身宝宝"))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的金钱不足");
					return true;
				}       

				//stBuyCartoonCmd * rev = (stBuyCartoonCmd *)cmd;
				zNpcB *base = npcbm.get(rev->npcID);
				if (!base) return false;

				t_buyCartoon_SceneSession send;
				send.data.masterID = id;
				send.data.npcID = base->id;
				strncpy(send.data.masterName, name, MAX_NAMESIZE);
				strncpy(send.data.name, base->name, MAX_NAMESIZE);
				send.data.lv = 10;
				zExperienceB *base_exp = experiencebm.get(10);
				if(base_exp)
					send.data.maxExp = base_exp->nextexp/10;
				send.data.sp = 100;
				send.data.maxSp = 100;
				send.data.state = Cmd::CARTOON_STATE_PUTAWAY;
				send.data.time = 0;
				send.data.masterLevel = charbase.level;
				sessionClient->sendCmd(&send, sizeof(t_buyCartoon_SceneSession));
				return true;
			}
			break;
		case FOLLOW_CARTOON_PARA:
			{
				//soke 挖宝状态禁止放出宝宝
				if (this->issetUState(Cmd::USTATE_USER_WABAO ))
	            {
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "挖宝状态下，不能放出替身宝宝");
					return false;
                }
				if (this->issetUState(Cmd::USTATE_USER_BIKE ))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "自行车比赛状态下，不能放出替身宝宝。");
					return false;
				}
				if (this->issetUState(Cmd::USTATE_USER_ZC ))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "战车状态下，不能放出替身宝宝。");
					return false;
				}
				stFollowCartoonCmd * rev = (stFollowCartoonCmd *)cmd;
				if(this->isDance())
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "跳舞状态下，不能放出宠物!");
					return true;
				}

				if (cartoon)//取消跟随
				{
					DWORD i = cartoon->getCartoonID();
					cartoon->putAway(SAVE_TYPE_PUTAWAY);

					cartoonList[i].state=Cmd::CARTOON_STATE_PUTAWAY;

					Cmd::stAddCartoonCmd send;
					send.isMine = true;
					send.cartoonID = i;
					send.data = cartoonList[i];
					sendCmdToMe(&send, sizeof(send));
					return true;
				}

				if (cartoonList.find(rev->cartoonID)==cartoonList.end())
					return true;
				if (cartoonList[rev->cartoonID].state==Cmd::CARTOON_STATE_ADOPTED
						|| cartoonList[rev->cartoonID].state==Cmd::CARTOON_STATE_WAITING)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "这个宝宝已经寄养，请先收回它");
					return true;
				}

				cartoonList[rev->cartoonID].repair = 0;
				summonPet(cartoonList[rev->cartoonID].npcID, Cmd::PET_TYPE_CARTOON);
				if (cartoon)
				{
					cartoonList[rev->cartoonID].state = Cmd::CARTOON_STATE_FOLLOW;
					cartoon->setCartoonID(rev->cartoonID);
					cartoon->setCartoonData(cartoonList[rev->cartoonID]);
					cartoon->sendData();

					if (cartoon->id>=9001 && cartoon->id<=9005)
						Channel::sendNine(cartoon, "爸爸妈妈不在家～ 我崩溃 ～ 崩溃～");
					else
					{
						time_t timValue = time(NULL);
						struct tm tmValue;
						zRTime::getLocalTime(tmValue, timValue);
						if (tmValue.tm_hour<6)
							Channel::sendNine(cartoon, "呜～困了～");
						else if (tmValue.tm_hour<7)
							Channel::sendNine(cartoon, "我饿了，想喝鲜牛奶～");
						else if (tmValue.tm_hour<8)
							Channel::sendNine(cartoon, "我给你唱首歌，啦啦啦，啦啦啦～");
						else if (tmValue.tm_hour<9)
							Channel::sendNine(cartoon, "今天天气好好哦～");
						else if (tmValue.tm_hour<10)
							Channel::sendNine(cartoon, "人家还想去游乐园玩嘛～");
						else if (tmValue.tm_hour<11)
							Channel::sendNine(cartoon, "那个人怎么不穿衣服啊，他不冷么～");
						else if (tmValue.tm_hour<12)
							Channel::sendNine(cartoon, "今天的饭真好吃～");
						else if (tmValue.tm_hour<13)
							Channel::sendNine(cartoon, "阿姨好漂亮啊～");
						else if (tmValue.tm_hour<14)
							Channel::sendNine(cartoon, "叔叔好肥啊，哈哈～");
						else if (tmValue.tm_hour<15)
							Channel::sendNine(cartoon, "你是坏人，我不跟你玩～");
						else if (tmValue.tm_hour<16)
							Channel::sendNine(cartoon, "讨厌，我不理你了～");
						else if (tmValue.tm_hour<17)
							Channel::sendNine(cartoon, "白毛浮绿水，嗯～嗯～下一句想不起来了～");
						else if (tmValue.tm_hour<18)
							Channel::sendNine(cartoon, "我饿了，想吃点心～");
						else if (tmValue.tm_hour<19)
							Channel::sendNine(cartoon, "人家小时候可好看了～");
						else if (tmValue.tm_hour<20)
							Channel::sendNine(cartoon, "呀伊伊，伊伊呀呀～");
						else if (tmValue.tm_hour<21)
							Channel::sendNine(cartoon, "你是想和我玩么～");
						else if (tmValue.tm_hour<22)
							Channel::sendNine(cartoon, "呜～困了～");
						else
							Channel::sendNine(cartoon, "爸爸妈妈不在家～ 我崩溃 ～ 崩溃～～");
					}
				}
				return true;
			}
			break;
		case LETOUT_CARTOON_PARA:
			{
				if (this->issetUState(Cmd::USTATE_USER_WABAO ))
	            {
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "挖宝状态下，不能修炼替身宝宝");
					return false;
                }
				if (this->issetUState(Cmd::USTATE_USER_BIKE ))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "自行车比赛状态下，不能修炼替身宝宝。");
					return false;
				}
				if (this->issetUState(Cmd::USTATE_USER_ZC ))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "战车状态下，不能修炼替身宝宝。");
					return false;
				}
				stLetOutCartoonCmd * rev = (stLetOutCartoonCmd *)cmd;
				if (cartoonList.find(rev->cartoonID)==cartoonList.end())
					return true;
				if (cartoonList[rev->cartoonID].time==0)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "不能训练这只宠物，请先充值");
					return true;
				}

				if (cartoon && cartoon->getCartoonID()==rev->cartoonID)
					cartoon->putAway(SAVE_TYPE_PUTAWAY);
				cartoonList[rev->cartoonID].state = Cmd::CARTOON_STATE_WAITING;
				Cmd::Session::t_saveCartoon_SceneSession send;
				strncpy(send.userName, name, MAX_NAMESIZE);
				send.type = SAVE_TYPE_LETOUT;
				send.cartoonID = rev->cartoonID;
				send.data = cartoonList[rev->cartoonID];
				sessionClient->sendCmd(&send, sizeof(send));
				return true;
			}
			break;
		case ADOPT_CARTOON_PARA:
			{
				if (this->issetUState(Cmd::USTATE_USER_WABAO ))
	            {
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "挖宝状态下，不能领养替身宝宝");
					return false;
                }
				if (this->issetUState(Cmd::USTATE_USER_BIKE ))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "自行车比赛状态下，不能领养替身宝宝。");
					return false;
				}
				if (this->issetUState(Cmd::USTATE_USER_ZC ))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "战车状态下，不能领养替身宝宝。");
					return false;
				}
				stAdoptCartoonCmd * rev = (stAdoptCartoonCmd *)cmd;
				if(this->isDance())
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "跳舞状态下，不能放领养宠物!");
					return true;
				}

				if (cartoonList.find(rev->cartoonID)!=cartoonList.end())
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你不能领养自己的宠物");
					return true;
				}
				if (adoptList.size()>=5)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你已经领养了五只宠物了，请先归还再领养");
					return true;
				}

				t_adoptCartoon_SceneSession send;
				send.userID = id;
				send.cartoonID = rev->cartoonID;
				sessionClient->sendCmd(&send, sizeof(send));
				return true;
			}
			break;
		case RETURN_CARTOON_PARA:
			{
				stReturnCartoonCmd * rev = (stReturnCartoonCmd *)cmd;
				if (adoptList.find(rev->cartoonID)==adoptList.end()) return true;

				adoptList[rev->cartoonID]->putAway(SAVE_TYPE_RETURN);
				return true;
			}
			break;
		case GETBACK_CARTOON_PARA:
			{
				bool b = false;
				for (cartoon_it it=cartoonList.begin(); it!=cartoonList.end(); it++)
				{
					if (it->second.state==Cmd::CARTOON_STATE_ADOPTED
							|| it->second.state==Cmd::CARTOON_STATE_WAITING)
					{
						b = true;
						break;
					}
				}
				if (!b)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你没有宠物被收养");
					return true;
				}

				t_getBackCartoon_SceneSession send;
				send.userID = id;
				sessionClient->sendCmd(&send, sizeof(send));

				return true;
			}
			break;
		case DRAWEXP_CARTOON_PARA:
			{
				stDrawExpCartoonCmd * rev = (stDrawExpCartoonCmd *)cmd;
				if (cartoonList.find(rev->cartoonID)==cartoonList.end())
					return true;

				if (cartoonList[rev->cartoonID].state==Cmd::CARTOON_STATE_WAITING
						|| cartoonList[rev->cartoonID].state==Cmd::CARTOON_STATE_ADOPTED)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "宝宝不在你身边");
					return true;
				}

				if (cartoonList[rev->cartoonID].addExp)
				{
					t_drawCartoon_SceneSession send;
					send.userID = id;
					send.cartoonID = rev->cartoonID;
					send.num = 0;
					sessionClient->sendCmd(&send, sizeof(send));

					if (cartoon&&cartoon->getCartoonID()==rev->cartoonID)
						cartoonList[rev->cartoonID] = cartoon->getCartoonData();
					cartoonList[rev->cartoonID].addExp = 0;
					if (cartoon&&cartoon->getCartoonID()==rev->cartoonID)
						cartoon->setCartoonData(cartoonList[rev->cartoonID]);
				}
				return true;
			}
			break;
		case CHANGENAME_CARTOON_PARA:
			{
				stChangeNameCartoonCmd * rev = (stChangeNameCartoonCmd *)cmd;
				if (cartoonList.find(rev->cartoonID)==cartoonList.end())
					return true;

				if (cartoonList[rev->cartoonID].state==Cmd::CARTOON_STATE_WAITING
						|| cartoonList[rev->cartoonID].state==Cmd::CARTOON_STATE_ADOPTED)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "宝宝不在你身边");
					return true;
				}

				if (cartoon) cartoon->setName(rev->name);
				strncpy(cartoonList[rev->cartoonID].name, rev->name, MAX_NAMESIZE);
				stAddCartoonCmd send;
				send.isMine = true;
				send.cartoonID = rev->cartoonID;
				send.data = cartoonList[rev->cartoonID];
				sendCmdToMe(&send, sizeof(send));

				return true;
			}
			break;
		case CONSIGN_CARTOON_PARA:
			{
				return true;//关闭此功能

				stConsignCartoonCmd * rev = (stConsignCartoonCmd *)cmd;
				if (cartoonList.find(rev->cartoonID)==cartoonList.end())
					return true;

				if (cartoon && cartoon->getCartoonID()==rev->cartoonID)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "请先把 %s 收起来", cartoonList[rev->cartoonID].name);
					return true;
				}

				if (cartoonList[rev->cartoonID].state==Cmd::CARTOON_STATE_ADOPTED)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "%s 已经被 %s 领养了", cartoonList[rev->cartoonID].name, cartoonList[rev->cartoonID].adopter);
					return true;
				}

				for (cartoon_it it=cartoonList.begin(); it!=cartoonList.end(); it++)
				{
					if (it->second.state==Cmd::CARTOON_STATE_ADOPTED
							|| it->second.state==Cmd::CARTOON_STATE_WAITING)
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的宝宝 %s 在等待状态或已经被领养了，请先收回它", it->second.name);
						return true;
					}
				}

				if (cartoonList[rev->cartoonID].time==0)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "请先给 %s 充值", cartoonList[rev->cartoonID].name);
					return true;
				}

				t_consignCartoon_SceneSession send;
				send.userID = id;
				send.cartoonID = rev->cartoonID;
				strncpy(send.name, rev->name, MAX_NAMESIZE);
				sessionClient->sendCmd(&send, sizeof(send));

				return true;
			}
			break;
		case CONSIGN_RET_CARTOON_PARA:
			{
				if(this->isWabao())
	            {
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "挖宝状态下，不能领养替身宝宝");
					return false;
                }
				if (this->issetUState(Cmd::USTATE_USER_BIKE ))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "自行车比赛状态下，不能领养替身宝宝。");
					return false;
				}
				if (this->issetUState(Cmd::USTATE_USER_ZC ))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "战车状态下，不能领养替身宝宝。");
					return false;
				}
				return true;//关闭此功能

				stConsignRetCartoonCmd * rev = (stConsignRetCartoonCmd *)cmd;

				Cmd::Session::t_consignRetCartoon_SceneSession send;
				send.userID = id;
				send.ret = rev->ret;
				send.cartoonID = rev->cartoonID;

				if (rev->ret==1 && adoptList.size()>=5)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你已经领养5只宠物了，不能继续领养");
					send.ret = 2;
				}

				sessionClient->sendCmd(&send, sizeof(send));

				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

/**
 * \brief 删除一个宠物
 *
 */
bool SceneUser::killOnePet(ScenePet * kill)
{
	if (!kill) return false;
	if (kill->getMaster()!=this)
	{
		Zebra::logger->error("[宠物]killOnePet %s(%u) 不是 %s 的宠物 getMaster()==%u", kill->name, kill->tempid, name, kill->getMaster());
		return false;
	}

	using namespace Cmd;

	//if (PET_TYPE_RIDE!=kill->getPetType())
	{
		stDelPetPetCmd del;
		del.id= kill->tempid;
		del.type = kill->getPetType();
		sendCmdToMe(&del, sizeof(del));
	}
	//if (PET_TYPE_RIDE==kill->getPetType())
	//	kill->setClearState();
	//else
	//	kill->toDie(this->tempid);

	switch (kill->getPetType())
	{
		case PET_TYPE_RIDE:
			{
				if (ridepet)
				{
					//if (0==ridepet->hp)//马死了，删除马
					{
						/*
						   stDelPetPetCmd del;
						   del.id= kill->tempid;
						   del.type = PET_TYPE_RIDE;
						   sendCmdToMe(&del, sizeof(del));
						   */

						//	horse.horse(0);
					}
					//else
					{
						if (horse.horse())//马活着
						{
							horse.data.state = Cmd::HORSE_STATE_PUTUP;
							horse.sendData();
							ridepet->setClearState();
						}
					}
				}
				ridepet = 0;
			}
			break;
		case PET_TYPE_PET:
			{
				pet->petData.hp = pet->hp;
				bcopy(&pet->petData, &petData, sizeof(petData));
				pet = 0;
			}
			break;
		case PET_TYPE_SUMMON:
			{
				summon = 0;
			}
			break;
		case PET_TYPE_TOTEM:
			{
				totems.remove(kill);
			}
			break;
		case PET_TYPE_GUARDNPC:
			{
				guard = 0;
			}
			break;
		case PET_TYPE_CARTOON:
			{
				if (kill==cartoon)
					cartoon = 0;
				else
					adoptList.erase(((CartoonPet *)kill)->getCartoonID());
				/*
				   if (((CartoonPet *)kill)->isAdopted())
				   adoptList.erase(((CartoonPet *)kill)->getCartoonID());
				   else
				   cartoon = 0;
				   */
			}
			break;
		case PET_TYPE_XCARTOON: //侍卫宝宝
			{
				if (kill==xcartoon)
					xcartoon = 0;
				else
					adoptList.erase(((CartoonPet *)kill)->getCartoonID());
			}
			break;
		case PET_TYPE_XCARTOON2: //侍卫宝宝
			{
				if (kill==xcartoon2)
					xcartoon2 = 0;
				else
					adoptList.erase(((CartoonPet *)kill)->getCartoonID());
			}
			break;
		default:
			break;
	}

	kill->clearMaster();

	return true;
#ifdef _XWL_DEBUG
	//Zebra::logger->debug("SceneUser::killOnePet(): unlock %s", name);
#endif
	//kill->setClearState();
	//Zebra::logger->debug("标记npc %s", kill->name);
}

/**
 * \brief 武器提升对应技能等级
 * \param skilltype 技能的类型
 * \param skillKind 技能的系别
 * \return 提升的技能登记数
 **/
WORD SceneUser::skillUpLevel(WORD skilltype, WORD skillkind)
{
	WORD upLevel = 0;
	WORD temp = 0;
	upLevel = packs.equip.getEquips().getMaxSkill(skilltype);
	temp = packs.equip.getEquips().getMaxSkills(skillkind);
	if ( temp > upLevel) upLevel = temp;
	return upLevel;
}

/**
 * \brief 检查是否骑马
 * \author fqnewman
 * \return true 骑马 false 没骑
 */
bool SceneUser::checkMountHorse()
{
	return horse.mount();
}

/**
 * \brief 人物坐下处理
 */
void SceneUser::sitdown()
{
    //soke 飞行、游泳、挖宝、自行车不能坐下
    if ( isSpa() ) return;
	if ( isBike() ) return;
	if ( isZC() ) return;
	if ( isFly() ) return;
	if ( isWabao() ) return;
	if ( isDance() ) return;
	if (this->issetUState(Cmd::USTATE_SITDOWN))
	{
		this->clearUState(Cmd::USTATE_SITDOWN);
		if(this->issetUState(Cmd::USTATE_XIULIAN))
		{
			this->clearUState(Cmd::USTATE_XIULIAN);
		}
		this->setupCharBase();
		
	}
	else
	{
		this->setUState(Cmd::USTATE_SITDOWN);
		this->setupCharBase();
		//处理家族地图打坐
		if( strstr(this->scene->getRealName(),"家族地图"))//在家族地图
		{	
			struct tm tv1;
			time_t timValue = time(NULL);
			zRTime::getLocalTime(tv1, timValue);
			if (tv1.tm_hour >=0 && tv1.tm_hour <24)
			{
				if(!this->issetUState(Cmd::USTATE_XIULIAN))
				{
					Channel::sendSys(this,Cmd::INFO_TYPE_EXP5,"你已经进入家族打坐修炼状态，持续十五分钟，请勿离开");
					Channel::sendSys(this,Cmd::INFO_TYPE_GAME,"你已经进入家族打坐修炼状态，持续十五分钟，请勿离开");
					this->setUState(Cmd::USTATE_XIULIAN);
				}
				cm.addCounterDay(JIAZU_TIME_IN_WANGCHENG);
			}		
			else{
				this->clearUState(Cmd::USTATE_XIULIAN);
			}		
		}
	}

	Cmd::stMainUserDataUserCmd  userinfo;
	full_t_MainUserData(userinfo.data);
	sendCmdToMe(&userinfo,sizeof(userinfo));

	this->sendMeToNine();
}

/**
 * \brief 人物站立
 */
void SceneUser::standup()
{
	this->clearUState(Cmd::USTATE_SITDOWN);
	this->setupCharBase();

	Cmd::stMainUserDataUserCmd  userinfo;
	full_t_MainUserData(userinfo.data);
	sendCmdToMe(&userinfo,sizeof(userinfo));

	this->sendMeToNine();
}

bool SceneUser::isSitdown()
{
	return this->issetUState(Cmd::USTATE_SITDOWN);
}

//soke 温泉状态
bool SceneUser::isSpa() const
{
	return this->issetUState(Cmd::USTATE_SPA);
}
//soke 飞行状态
bool SceneUser::isFly() const
{
	return this->issetUState(Cmd::USTATE_USER_FLY);
}
//soke 挖宝状态
bool SceneUser::isWabao() const
{
	return this->issetUState(Cmd::USTATE_USER_WABAO);
}
//soke 自行车状态
bool SceneUser::isBike() const
{
	return this->issetUState(Cmd::USTATE_USER_BIKE);
}
//soke 战车状态
bool SceneUser::isZC() const
{
	return this->issetUState(Cmd::USTATE_USER_ZC);
}

bool SceneUser::isDance() const
{
	return this->issetUState(Cmd::USTATE_USER_DANCE);
}

/**
 * \brief 通知装备改变
 */
void SceneUser::notifyEquipChange()
{
	if (summon)
	{
		zObject *temp=NULL;
		if(packs.equip.getObjectByZone(&temp , 0 , Cmd::EQUIPCELLTYPE_HANDR))
		{
			if(temp)
			{
				if (0 != temp->data.dur)
				{
					if (temp->base->kind == ItemType_Stick)
					{
#ifdef _DEBUGLOG
						Zebra::logger->debug("棍子增强召唤兽攻击力 （%u-%u)", packs.equip.getEquips().get_appendminpet(), packs.equip.getEquips().get_appendmaxpet());
#endif
						summon->setAppendDamage((DWORD)(packs.equip.getEquips().get_appendminpet()*(1+packs.equip.getEquips().get_mdam()/100.0f)), 
							(DWORD)(packs.equip.getEquips().get_appendmaxpet()*(1+packs.equip.getEquips().get_mdam()/100.0f)));
						this->usm.refresh(); // 装备改变可能会影响技能等级
						return;
					}
				}
			}
		}
		summon->setAppendDamage(0, 0);
	}
	else
	{
		if (skillValue.introject_maxhp!=0)
		{
			this->skillStatusM.clearRecoveryElement(173);
			this->changeAndRefreshHMS();
		}
	}

	this->usm.refresh(); // 装备改变可能会影响技能等级
}

//soke 增加保存温泉时间
void SceneUser::logout()
{
    //退出之前离开温泉
	showCurrentEffect( Cmd::USTATE_SPA, false );

    //退出之前保存游戏时间
    saveTodayGameTimeWhenLogout();

    //退出之前保存侍卫
	if (xcartoon)
	{
		xcartoon->putAway(Cmd::Session::SAVE_TYPE_PUTAWAY);    
	}

	if (xcartoon2)
	{
		xcartoon2->putAway(Cmd::Session::SAVE_TYPE_PUTAWAY);    
	}

}

/**
 * \brief 获得宠物类的附加攻击力
 */
void SceneUser::getSummonAppendDamage(DWORD &minDamage, DWORD &maxDamage)
{
	zObject *temp;
	if(packs.equip.getObjectByZone(&temp , 0 , Cmd::EQUIPCELLTYPE_HANDR))
	{
		if(temp)
		{
			if (0 != temp->data.dur)
			{
				if (temp->base->kind == ItemType_Stick)
				{
#ifdef _DEBUGLOG
					Zebra::logger->debug("棍子增强召唤兽攻击力 （%u-%u)", packs.equip.getEquips().get_appendminpet(), packs.equip.getEquips().get_appendmaxpet());
#endif
					minDamage = (DWORD)(packs.equip.getEquips().get_appendminpet()*(1+packs.equip.getEquips().get_mdam()/100.0f));
					maxDamage = (DWORD)(packs.equip.getEquips().get_appendmaxpet()*(1+packs.equip.getEquips().get_mdam()/100.0f));
					return;
				}
			}
		}
	}

	minDamage = 0;
	maxDamage = 0;
}

/**
 * \brief 增加经验值
 * \param num 数量
 * \param addPet 是否给宠物加经验
 * \param dwTempID 经验值来源者的temp id
 * \param byType 经验值来源的类型 enum enumMapDataType
 * \param addPet 是否给宠物增加
 * \param addCartoon 是否给卡通宠物加经验
 * \param addPet 是否给替身宠物增加
 */
void SceneUser::addExp(QWORD num, bool addPet, DWORD dwTempID, BYTE byType, bool addCartoon)
{
	// 转生经验设置参数
	if (this->charbase.round > 0)
	{
		int level12=atoi(Zebra::global["trunfy_level"].c_str()); //sky 修改转生封印等级从配置读取
		int level13=atoi(Zebra::global["maxtrun_level"].c_str());//sky 修改转生最大等级从配置读取

		if (level12 <= level13) 
		{
			if (charbase.level<level12) 
		    //if (charbase.level<MAX_LEVEL)
			{
				charbase.exp += num;

				Cmd::stObtainExpUserCmd ret;
				ret.dwTempID = dwTempID;				/** 经验值来源临时编号 */
				ret.byType = byType;				/** 经验值来源 enumMapDataType */
				//	ret.dwUserTempID = pUser->tempid;
				ret.dwExp = num;
				if(this->charbase.exp >= this->charstate.nextexp)
				{
				ret.dwUserExp = this->charbase.exp-this->charstate.nextexp;
				}
				else
				{
				ret.dwUserExp = this->charbase.exp;
				}
				//#ifdef _DEBUGLOG
					Zebra::logger->info("[发送经验增加通知]获得经验：%llu 用户当前转生经验：%llu 用户最大转生经验：%llu" ,ret.dwExp, ret.dwUserExp,charstate.nextexp);
				//#endif
				this->sendCmdToMe(&ret , sizeof(ret));

				if (charbase.exp >= charstate.nextexp) 
				{
					upgrade();
				}
			}
		}
		//soke 增加封印等级溢出说明
		else if (level12 > level13)
		{
			Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"当前服务器转生最大等级：%d级，增加经验失败",level13);
			charbase.exp += 0;
		}
	  		addPetExp(num, addPet, addCartoon);
	}
	else
	{
		int level2=atoi(Zebra::global["serverfy_level"].c_str()); //soke 修改封印等级从配置读取
		int level3=atoi(Zebra::global["servermax_level"].c_str());//soke 修改最大等级从配置读取

		if (level2 <= level3) 
		{
			if (charbase.level<level2) 
		//if (charbase.level<MAX_LEVEL)
			{
				charbase.exp += num;

				Cmd::stObtainExpUserCmd ret;
				ret.dwTempID = dwTempID;				/** 经验值来源临时编号 */
				ret.byType = byType;				/** 经验值来源 enumMapDataType */
				//	ret.dwUserTempID = pUser->tempid;
				ret.dwExp = num;
				if(this->charbase.exp >= this->charstate.nextexp)
				{
				ret.dwUserExp = this->charbase.exp-this->charstate.nextexp;
				}
				else
				{
				ret.dwUserExp = this->charbase.exp;
				}
				//#ifdef _DEBUGLOG
					Zebra::logger->info("[发送经验增加通知]获得经验：%llu 用户当前经验：%llu 用户最大经验：%llu" ,ret.dwExp, ret.dwUserExp,charstate.nextexp);
				//#endif
				this->sendCmdToMe(&ret , sizeof(ret));

				if (charbase.exp >= charstate.nextexp) 
				{
					upgrade();
				}
			}
		}
		//soke 增加封印等级溢出说明
		else if (level2 > level3)
		{
			Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"当前服务器最大等级：%d级，增加经验失败",level3);
			charbase.exp += 0;
		}
	  		addPetExp(num, addPet, addCartoon);
	}
}

/**
 * \brief 增加所有宠物的经验
 *
 * \param num 数量
 * \param addPet 是否给宠物增加
 * \param addCartoon 是否给替身增加
 */
void SceneUser::addPetExp(DWORD num, bool addPet, bool addCartoon)
{
	if (pet && addPet)
		pet->addExp(num);
	if (summon)
		summon->addExp(num);
	if (cartoon && addCartoon)
	{
		cartoon->addExp(num*4/10);
		cartoonList[cartoon->getCartoonID()] = cartoon->getCartoonData();
	}
}

/**
 * \brief 交换自己与宠物的位置
 */
void SceneUser::exchangeMeAndSummonPet()
{
	if (summon)
	{
		if (summon->scene == this->scene)
		{
			if(this->scene->zPosShortRange(this->getPos() , summon->getPos() , SCREEN_WIDTH*3 , SCREEN_HEIGHT*3))
			{
				zPos myPos = this->pos;
				zPos petPos = summon->getPos();

				this->scene->clearBlock(petPos);
				Cmd::stRemoveMapNpcMapScreenUserCmd send;
				send.dwMapNpcDataPosition = summon->tempid;
				this->sendCmdToMe(&send, sizeof(send));

				zPos vPos = summon->getPos();
				this->goTo(vPos);

				summon->warp(myPos, true);
			}
			else
				Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "你和召唤兽之间的距离过远!");
		}
		else
		{
			Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "你和召唤兽不在一张地图内不可以交换位置!");
		}
	}
	else
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "没有召唤兽可以交换位置!");
	}
}

/**
 * \brief 返回当前法术值
 * \return 当前法术值
 */
DWORD SceneUser::getMP()
{
	return this->charbase.mp;
}

/**
 * \brief 清除当前法术值
 */
void SceneUser::clearMana()
{
	this->charbase.mp = 0;
	this->checkAutoMP();
}

/**
 * \brief 向9屏发送宠物信息
 *
 */
void SceneUser::sendPetDataToNine()
{
	if (ridepet) ridepet->sendPetDataToNine();
	if (guard) guard->sendPetDataToNine();
	if (cartoon) cartoon->sendPetDataToNine();
	for (adopt_it it=adoptList.begin(); it!=adoptList.end(); it++)
		it->second->sendPetDataToNine();
	SceneEntryPk::sendPetDataToNine();
}

DWORD SceneUser::getFiveType() const
{
	return charbase.fivetype;
}

DWORD SceneUser::getFivePoint() const
{
	return charbase.fivelevel*5+5;
}

DWORD SceneUser::getFiveLevel() const
{
	return charbase.fivelevel;
}

/*
 * \brief 尝试捕获一只npc
 *
 * \param npc 要抓的npc
 * \param type 要抓的npc类型
 *
 * \return 是否成功
 *
 */
bool SceneUser::captureIt(SceneNpc *npc, BYTE type)
{
	if (!npc) return false;
	if (type!=npc->npc->bear_type) return false;
	if (npc->npc->level>charbase.level) return false;
	if (npc->getPetType()!=Cmd::PET_TYPE_NOTPET) return false;

	ScenePet * newPet = summonPet(npc->npc->id, Cmd::PET_TYPE_PET, 0, 0, 0, 0, npc->getPos());
	if (newPet)
	{
		npc->hp = npc->npc->hp;
		npc->hideMe(60000);

		newPet->petData.state = Cmd::PET_STATE_NORMAL;
		newPet->getAbilityByLevel(newPet->npc->level);
		bcopy(&newPet->petData, &petData, sizeof(petData));
		newPet->sendData();
		return true;
	}
	return false;
}

bool SceneUser::speedOutMove(DWORD time, WORD speed, int len)
{
	//zRTime ctv;
	DWORD steptime=300;

    //soke 增加飞行
	if(horse.mount() || isFly())
	{
		steptime=200;
	}

	steptime = (DWORD)((steptime*speed)/640.0f);
	if (len == 0) len=1;
	if((time - lastMoveTime)/len > steptime)
	{
#ifdef _DEBUGLOG
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "移动积累值:%u 时间差:%ld 单步标准时间=%u 移动步长=%u 平均移动时间%u", moveFastCount, (time - lastMoveTime), steptime, len,(time - lastMoveTime)/len);
#endif
		lastMoveTime = time;
		if (moveFastCount>0) moveFastCount--;
		return true;
	}
	else
	{
		if (moveFastCount<5)
		{
#ifdef _DEBUGLOG
			Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "移动积累值:%u 时间差:%ld 单步标准时间=%u 移动步长=%u 平均移动时间%u", moveFastCount, (time - lastMoveTime), steptime, len, (time - lastMoveTime)/len);
#endif
			lastMoveTime = time;
			moveFastCount++;
			return true;
		}
		else
			if (moveFastCount<10000) moveFastCount++;
#ifdef _DEBUGLOG
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "移动积累值:%u 时间差:%ld 单步标准时间=%u 移动步长=%u 平均移动时间%u", moveFastCount, (time - lastMoveTime), steptime,len,(time - lastMoveTime)/len);
#endif
	}
	return false;
}


/**
 * \brief 根据装备上带的几率增加装备指定数值的血和蓝，根据对敌伤害增加自身数值
 * \param rev 争夺战开始通知消息
 * \author fqnewman
 */
void SceneUser::leech(DWORD dwDam)
{
	if(zMisc::selectByPercent(packs.equip.getEquips().get_hpleech_odds()))
	{
		this->changeHP(packs.equip.getEquips().get_hpleech_effect());
	}
	if(zMisc::selectByPercent(packs.equip.getEquips().get_mpleech_odds()))
	{
		this->changeMP(packs.equip.getEquips().get_mpleech_effect());
	}
	if(skillValue.attackaddhpnum > 0)
	{
		this->changeHP((SDWORD)(dwDam *(skillValue.attackaddhpnum/100.0f)));
	}
}

/**
 * \brief 处理npc争夺战状态检查消息设置争夺状态标志
 * \param rev 争夺战开始通知消息
 * \author fqnewman
 */
void SceneUser::checkNpcDare(Cmd::Session::t_NpcDare_NotifyScene_SceneSession * rev)
{
	if (scene)
	{
		if (charbase.level >19 &&
				rev->dwCountryID == scene->getCountryID() &&
				rev->dwMapID == scene->getRealMapID() &&
				abs(rev->dwPosX - this->pos.x)<=26 &&
				abs(rev->dwPoxY - this->pos.y)<=38)
		{
			if (this->charbase.level >=60)
			{
				DWORD mapid = this->scene->getRealMapID();
				if (mapid == 101 ||
					mapid == 102 ||
					mapid == 104)
				{
					/// 超过59的玩家不允许在凤凰城，凤尾村和清源村进行NPC争夺战。
					npcdareflag = false;
					return;
				}
			}
			npcdareflag = true;
			npcdarePosX = rev->dwPosX;
			npcdarePosY = rev->dwPoxY;
			npcdareCountryID = rev->dwCountryID;
			npcdareMapID = rev->dwMapID;
			npcdareNotify = true;
			Zebra::logger->trace("[家族争夺NPC]家族[%u]中的角色[%s]进入NPC争夺对战状态", this->charbase.septid, this->name);
			return;
		}
	}
	npcdareflag = false;
}

/**
 * \brief 实时检查与被争夺NPC的距离，如果超过距离则取消状态，如果离边界太近则提示
 * \author fqnewman
 */
void SceneUser::checkNpcDareState()
{
	if (scene&&npcdareflag)
	{
		if (npcdareCountryID == scene->getCountryID() &&
				npcdareMapID == scene->getRealMapID() &&
				abs(npcdarePosX - this->pos.x)<=26 &&
				abs(npcdarePosY - this->pos.y)<=38)
		{
			if ((abs(npcdarePosX - this->pos.x)>=20 ||
						abs(npcdarePosY - this->pos.y)>=30)&&
					npcdareNotify)
			{
				npcdareNotify = false;
				Channel::sendSys(this, Cmd::INFO_TYPE_EXP, "你距离战场边界太近，交战时间没结束，再往外走将脱离战斗！");
			}
			else
			{
				npcdareNotify = true;
			}
		}
		else
		{
			Channel::sendSys(this, Cmd::INFO_TYPE_EXP, "你离开战场已经失去对战资格,这也许会导致你的家族战败！！！");
			Zebra::logger->trace("[家族争夺NPC]家族[%u]中的角色%s因为走出战场失去参战资格", this->charbase.septid, this->name);
			npcdareflag = false;
			this->removeWarRecord(Cmd::SEPT_NPC_DARE);

			this->sendNineToMe();
			this->sendMeToNine();
		}
	}
}

/**
 * \brief 实时检查与被争夺NPC的距离，如果超过距离则取消状态，如果离边界太近则提示
 * \author fqnewman
 */
void SceneUser::notifySessionNpcDareResult()
{
	if (npcdareflag)
	{
		npcdareflag = false;
		if (this->getState() != SceneUser::SceneEntry_Death)
		{
			Cmd::Session::t_NpcDare_Result_SceneSession send;
			send.dwSeptID = this->charbase.septid;
			sessionClient->sendCmd(&send,sizeof(send));
			Zebra::logger->trace("[家族争夺NPC]家族[%u]中的角色%s还活着向会话发出战果统计", this->charbase.septid, this->name);
		}
	}
}

/**
 * \brief 商人向角色交保护费
 * \param dwGold 保护费的数额
 * \author fqnewman
 */
void SceneUser::npcDareGetGold(DWORD dwGold)
{
	Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "管理费发出");
	this->packs.addMoney(dwGold,"npc对战商人保护费");
}

/**
 * \brief 发送消息给自己
 * \param pattern 消息体
 * \author fqnewman
 */
void SceneUser::sendMessageToMe(const char *pattern)
{
	Channel::sendSys(this, Cmd::INFO_TYPE_GAME, pattern);
}

/**
 * \brief 让宠物重生
 * \author fqnewman
 */
void SceneUser::relivePet()
{
	if (!pet&&petData.id!=0)
	{
		petData.state = Cmd::PET_STATE_NORMAL;

		ScenePet * newPet = NULL;
		if (0==strncmp(petData.name, "", MAX_NAMESIZE))
			newPet = summonPet(petData.id, Cmd::PET_TYPE_PET);
		else
			newPet = summonPet(petData.id, Cmd::PET_TYPE_PET, 0, 0, petData.name);

		if (newPet)
		{
			bcopy(&petData, &newPet->petData, sizeof(Cmd::t_PetData));
			newPet->getAbilityByLevel(petData.lv);
			newPet->hp = petData.maxhp;
			newPet->setPetAI((Cmd::petAIMode)petData.ai);
			newPet->sendData();
			newPet->sendMeToNine();
		}
	}
}

/**
 * \brief 让宠物回到主人身边
 * \author xwl
 */
void SceneUser::collectPets()
{
	for (std::list<ScenePet *>::iterator it=totems.begin(); it!=totems.end(); it++)
	{
		if ((*it)->canMove())
			(*it)->changeMap(scene, getPos());
	}

	if (ridepet && ridepet->canMove())
		ridepet->changeMap(scene, getPos());
	if (pet && pet->canMove())
		pet->changeMap(scene, getPos());
	if (summon && summon->canMove())
		summon->changeMap(scene, getPos());
	if (guard && guard->canMove())
		guard->changeMap(scene, getPos());
}

bool SceneUser::unCombin(Cmd::stUnCombinUserCmd *rev)
{
	switch (rev->type)
	{
		case Cmd::UN_STATE_COMBIN:
			{
				this->skillStatusM.clearRecoveryElement(173);
			}
			break;
		case Cmd::UN_STATE_CHANGE_FACE:
			{
				this->skillStatusM.clearRecoveryElement(132);
				this->skillStatusM.clearRecoveryElement(16);
			}
			break;
		case Cmd::UN_STATE_TEAM_ATTACK_BIRD:
			{
				this->skillStatusM.clearActiveElement(230);
			}
			break;
		case Cmd::UN_STATE_TEAM_ATTACK_FLOW:
			{
				this->skillStatusM.clearActiveElement(238);
			}
			break;
		case Cmd::UN_STATE_TEAM_ATTACK_ONE_DIM:
			{
				this->skillStatusM.clearActiveElement(264);
			}
			break;
		case Cmd::UN_STATE_PLAY_FLY: //soke 停止飞行
			{
				endFly();
			}
			break;
		default:
			break;
	}
	this->changeAndRefreshHMS();
	return true;
}

/**
 * \brief 获得当前魔法攻击力
 * \author fqnewman
 * \return 魔法攻击力
 */
DWORD SceneUser::getMaxMDamage()
{
	return charstate.maxpdamage;
}

/**
 * \brief 获得当前物理攻击力
 * \author fqnewman
 * \return 物理攻击力
 */
DWORD SceneUser::getMaxPDamage()
{
	return charstate.maxpdamage;
}

/**
 * \brief 获得当前物理防御力
 * \author fqnewman
 * \return 物理防御力
 */
DWORD SceneUser::getPDefence()
{
	return charstate.pdefence;
}

/**
 * \brief 获得当前魔法防御力
 * \author fqnewman
 * \return 魔法防御力
 */
DWORD SceneUser::getMDefence()
{
	return charstate.mdefence;
}

/**
 * \brief 更新用户数据到会话
 * \author zjw
 */
void SceneUser::updateUserData()
{
	Cmd::Session::t_changeUserData_SceneSession send;
	send.wdLevel = this->charbase.level;
	send.dwExploit = this->charbase.exploit;
	send.dwGrace = this->charbase.grace;
	send.dwUserID = this->id;
	sessionClient->sendCmd(&send, sizeof(send));
}

/**
 * \brief 加入一条新的对战记录
 *
 *
 * \param type 对战类型
 * \param relationid 对方社会类型ID
 * \param isAtt 在该对战中，自己是否属于攻击方
 * \return 
 */
void SceneUser::addWarRecord(DWORD type, DWORD relationid, bool isAtt)
{       
	if (!this->isWarRecord(type, relationid))
	{
		WarRecord record;
		record.type = type; 
		record.relationid = relationid;
		record.isAtt = isAtt;
		vWars.push_back(record);
	}
}

/**             
 * \brief 清除指定的对战记录
 *              
 *      
 * \param type 对战类型
 * \return 敌对方社会关系ID
 */     
void SceneUser::removeWarRecord(DWORD type, DWORD relationid)
{
	WarIter iter;
	for (iter = vWars.begin(); iter!=vWars.end();)
	{       
		if (iter->type == type && (iter->relationid == relationid || relationid==0))
		{
			iter = vWars.erase(iter);
		} 
		else {
			iter++;
		}
	}       

	if (iter!=vWars.end())
	{       
		vWars.erase(iter);
	}
}

void SceneUser::setAntiAttState(DWORD type, DWORD relationid)
{
	WarIter iter;
	for (iter = vWars.begin(); iter!=vWars.end(); iter++)
	{       
		if (iter->type == type && iter->relationid == relationid)
		{
			break;
		} 
	}       

	if (iter!=vWars.end())
	{       
		(*iter).isAntiAtt = true;
	}
}

bool SceneUser::isWarRecord(DWORD type, DWORD relationid)
{
	My_FunctionTime_wrapper(__PRETTY_FUNCTION__);
	WarIter iter;
	for (iter = vWars.begin(); iter!=vWars.end(); iter++)
	{       
		if ((iter->type == type || (type == Cmd::COUNTRY_FORMAL_DARE && iter->type==Cmd::COUNTRY_FORMAL_ANTI_DARE) )
				&& iter->relationid == relationid)
		{
			break;
		} 
	}       

	if (iter!=vWars.end())
	{       
		return true;
	}

	return false;
}

bool SceneUser::isAtt(DWORD dwType, DWORD relationid)
{
	WarIter iter;
	for (iter = vWars.begin(); iter!=vWars.end(); iter++)
	{       
		if ((iter->type == dwType || (dwType == Cmd::COUNTRY_FORMAL_DARE && iter->type==Cmd::COUNTRY_FORMAL_ANTI_DARE) ) 
				&& (iter->relationid == relationid || relationid==0))
		{
			break;
		} 
	}       

	if (iter!=vWars.end())
	{       
		return iter->isAtt;
	}

	return false;
}

bool SceneUser::isAntiAtt(DWORD dwType, DWORD relationid)
{
	WarIter iter;
	for (iter = vWars.begin(); iter!=vWars.end(); iter++)
	{       
		if ((iter->type == dwType || (dwType == Cmd::COUNTRY_FORMAL_DARE && iter->type==Cmd::COUNTRY_FORMAL_ANTI_DARE) ) 
				&& (iter->relationid == relationid || relationid==0))
		{
			break;
		} 
	}       

	if (iter!=vWars.end())
	{       
		return iter->isAntiAtt;
	}

	return false;

}

/**
 * \brief 把宠物抓到自己跟前
 */
void SceneUser::catchMyPet()
{
	if (guard) guard->warp(this->getPos(),true);
	if (pet) pet->warp(this->getPos(),true);
	if (summon) summon->warp(this->getPos(), true);
}

/**
 * \brief 聊天自动回复
 * \param toName 回复给谁
 * \author xwl
 */
void SceneUser::autoReply(char * toName) const 
{
	if (!toName) return;

	zRTime ctv;
	Cmd::stChannelChatUserCmd send;
	send.dwType=Cmd::CHAT_TYPE_AUTO;
	send.dwCharType = charbase.type; //修复换头像不显示昵称
	send.dwChatTime = ctv.sec();
	strncpy((char *)send.pstrChat, (char *)replyText, MAX_CHATINFO-1);
	strncpy((char *)send.pstrName, name, MAX_NAMESIZE);

	sendCmdByName(toName, &send, sizeof(send));
}

/**
 * \brief 给该玩家发送物品，通过邮件
 *
 * \author xwl
 * \param fromName 发送人
 * \param toName 接受者名字
 * \param type 邮件类型
 * \param money 发送金钱
 * \param 发送物品
 * \param 文字内容
 */
void sendMail(char * fromName, DWORD fromID, char * toName, DWORD toID, BYTE type, DWORD money, zObject * o, char * text)
{
	if (!fromName||!toName) return;

	Cmd::Session::t_sendMail_SceneSession sm;
	sm.mail.state = Cmd::Session::MAIL_STATE_NEW;
	strncpy(sm.mail.fromName, fromName, MAX_NAMESIZE);
	sm.mail.fromID = fromID;
	sm.mail.toID = toID;
	strncpy(sm.mail.toName, toName, MAX_NAMESIZE);
	strncpy(sm.mail.title, "系统发送的邮件", MAX_NAMESIZE);
	sm.mail.type = type;
	zRTime ct;
	sm.mail.createTime = ct.sec();
	sm.mail.delTime = sm.mail.createTime + 60*60*24*7;
	snprintf(sm.mail.text, 255, text);
	sm.mail.sendMoney = money;
	sm.mail.recvMoney = 0;
	sm.mail.sendGold = 0;
	sm.mail.recvGold = 0;
	sm.mail.itemGot = 0;
	if (o)
	{
			o->getSaveData((SaveObject *)&sm.item);
			sm.mail.itemID = o->data.qwThisID;
	}
	if (money || o) sm.mail.accessory = 1;
	sessionClient->sendCmd(&sm, sizeof(sm));
	Zebra::logger->trace("[邮件]系统发送给玩家 %s item=%s money=%u", toName, o?o->data.strName:"", money);
}
bool SceneUser::canVisitNpc(SceneNpc *pNpc)
{
	//不在同一地图不可以访问
	if(!pNpc || !pNpc->scene || pNpc->scene != this->scene)
	{
		return false;
	}
	//soke 增加判断flag=9  本国人都可以访问
	if(pNpc->scene->getCountryID() == charbase.country && (atoi(Zebra::global["service_flag"].c_str())!=9) )
	{
		return true;
	}
	//盟国访问
	if (pNpc->npc->allyVisit && (CountryAllyM::getMe().getFriendLevel(pNpc->scene->getCountryID(), charbase.country) >= pNpc->npc->allyVisit))
			return true;

	//易容术可以访问
	if( changeface)
	{
		return true;
	}
	//**********************************************
        //soke 边境传送和所有车夫NPC(外国人也可以访问)
	      if( pNpc->id == 95||pNpc->id == 142
		    ||pNpc->id ==143||pNpc->id == 149
		    ||pNpc->id ==150||pNpc->id == 158
		    ||pNpc->id ==301||pNpc->id == 302
		    ||pNpc->id ==513||pNpc->id == 517
		    ||pNpc->id ==845||pNpc->id == 846
		    ||pNpc->id ==847||pNpc->id == 858
		    ||pNpc->id ==1023||pNpc->id ==1039
            ||pNpc->id ==1040||pNpc->id == 1164
		    ||pNpc->id ==1170||pNpc->id == 1627
		    ||pNpc->id ==1678)
	{
		    return true;
	}
   //**********************************************

	//家族地图
	//soke 增加判断flag=9 5000-59999范围的外国人也可以访问
	if(pNpc->id==974 && (atoi(Zebra::global["service_flag"].c_str())!=9))
	{
		return true;
	}

	//soke 增加判断flag=9 5000-59999范围的外国人也可以访问
	if(pNpc->id>=5000 && pNpc->id<=5244 && (atoi(Zebra::global["service_flag"].c_str())!=9))
	{
		return true;
	}

	//soke 增加判断flag=9  5000-59999范围的外国人也可以访问
	if(pNpc->id>=5249&&pNpc->id<=5999 && (atoi(Zebra::global["service_flag"].c_str())!=9))
	{
		return true;
	}
	//soke 增加判断flag=9  5000-59999范围的外国人也可以访问
	if(pNpc->id>=1070&&pNpc->id<=1103 && (atoi(Zebra::global["service_flag"].c_str())!=9))
	{
		return true;
	}

	//soke 增加判断flag=9  5000-59999范围的外国人也可以访问 自行车新NPC
	if(pNpc->id>=52101&&pNpc->id<=52111 && (atoi(Zebra::global["service_flag"].c_str())!=9))
	{
		return true;
	}
	
	//soke 增加判断flag=9  5000-59999范围的外国人也可以访问 迷宫
	if(pNpc->id>=908&&pNpc->id<=913 && (atoi(Zebra::global["service_flag"].c_str())!=9))
	{
		return true;
	}
///////////////////////////////////////////////////
	//5000-59999范围的外国人也可以访问
	//if(pNpc->id>=5000&&pNpc->id<=5999)
	//{
	//	return true;
	//}
///////////////////////////////////////////////////

	//soke 增加判断flag=9
	if (pNpc->id>=7000 && pNpc->id<=7100 && (atoi(Zebra::global["service_flag"].c_str())!=9))
	{
		return true;
	}

	//6000外国人非国战时间可以访问,国战期间不可以访问
	if(pNpc->id==6000 && !this->isSpecWar(Cmd::COUNTRY_FORMAL_DARE))
	{
		return true;
	}

	//soke 增加判断flag=9   5245-5248(太庙) 不是国战国战期间可以访问
	if( pNpc->id >= 5245 && pNpc->id <= 5248  && !this->isSpecWar(Cmd::COUNTRY_FORMAL_DARE) && (atoi(Zebra::global["service_flag"].c_str())!=9))
	//if( pNpc->id >= 800001 && pNpc->id <= 800004 )
	{	
		return true;
	}
		//5245-5248(太庙) 国战期间不可以访问
	if( pNpc->id >= 5245 && pNpc->id <= 5248  && this->isSpecWar(Cmd::COUNTRY_FORMAL_DARE))
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "国战期间不能拆除太庙!");
		return false;
	}

	//soke 增加flag=9 不能访问NPC
	if( pNpc->id >= 0 && (atoi(Zebra::global["service_flag"].c_str())==9))
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "当前未到开区时间，不能访问NPC!");
		return false;
	}

	return false;
}

void SceneUser::setNpcHoldData(Cmd::Session::t_notifyNpcHoldData *rev)
{
	this->npcHoldData.dwPosX = rev->dwPosX;
	this->npcHoldData.dwPosY = rev->dwPosY;
	this->npcHoldData.dwMapID = rev->dwMapID;
}

void SceneUser::checkNpcHoldDataAndPutExp()
{
	if (this->isSitdown())
	{
		if (this->scene->getRealMapID() != npcHoldData.dwMapID) return;
		if (abs((int)npcHoldData.dwPosX - (int)this->getPos().x)<=26 &&
				abs((int)npcHoldData.dwPosY - (int)this->getPos().y)<=38)
		{
			DWORD dwExp =  (DWORD)((1.4*charbase.level*charbase.level+5*charbase.level)/6/10+1);
			this->addExp(dwExp);
		}
	}
}

void SceneUser::checkPunishTime()
{
		if (charbase.punishTime>0)
				charbase.punishTime--;
}

DWORD SceneUser::refreshPetPackSize()
{
	DWORD levelCount = 0;
	for (cartoon_it it=cartoonList.begin(); it!=cartoonList.end(); it++)
		levelCount += it->second.lv;

	PetPack *pack = (PetPack *)packs.getPackage(Cmd::OBJECTCELLTYPE_PET, 0);
	if (!pack) return false;

	DWORD cellCount = cartoonList.size() + levelCount/10;
	pack->setAvailable(cellCount);

	charbase.petPack = (cellCount+1>80) ? 80 : (cellCount+1);//多一个格子，避免宠物回档造成物品丢失

#ifdef _XWL_DEBUG
	Zebra::logger->debug("%s 宠物包裹大小 %u", name, cellCount);
#endif
	return cellCount;
}

//soke 2016 - 3 - 11 外交官（停用）
void SceneUser::setDiplomatState(BYTE newstate)
{
	if (newstate == 0)
	{
		isDiplomat = false;
	}
	else
	{
		isDiplomat = true;
	}
}

int SceneUser::isDiplomatState() // 返回0为外交官状态,返回1为非外交官状态,返回2为是外交官但因为有采集手套,暂时无效
{
	if (isDiplomat)
	{
		zObject *temp=NULL;
		if(this->packs.equip.getObjectByZone(&temp , 0 , Cmd::EQUIPCELLTYPE_HANDR))
		{
			if(temp)
			{
				if (0 != temp->data.dur)
				{
					if (temp->base->id == 876) //采集手套
					{
						return 2;
					}
				}
			}
		}

		return 0;
	}

	return 1;
}

//soke 2016 - 3 - 11 巡捕（左上）
void SceneUser::setCatcherState(BYTE newstate)
{
	if (newstate == 0)
	{
		isCatcher = false;
	}
	else
	{
		isCatcher = true;
	}
}

bool SceneUser::isCatcherState() const
{
	return isCatcher;
}

//soke 2016 - 3 - 11 巡捕（左下）
void SceneUser::setCatchexState(BYTE newstate)
{
	if (newstate == 0)
	{
		isCatchex = false;
	}
	else
	{
		isCatchex = true;
	}
}

bool SceneUser::isCatchexState() const
{
	return isCatchex;
}

//soke 2016 - 3 - 11 御史大夫（右上）
void SceneUser::setCensorState(BYTE newstate)
{
	if (newstate == 0)
	{
		isCensor = false;
	}
	else
	{
		isCensor = true;
	}
}

bool SceneUser::isCensorState() const
{
	return isCensor;
}

//soke 2016 - 3 - 11 御史大夫（右下）
void SceneUser::setCensosState(BYTE newstate)
{
	if (newstate == 0)
	{
		isCensos = false;
	}
	else
	{
		isCensos = true;
	}
}

bool SceneUser::isCensosState() const
{
	return isCensos;
}

//soke 2016 - 3 - 11 元帅
void SceneUser::setYuanshuaiState(BYTE newstate)
{
	if (newstate == 0)
	{
		isYuanshuai = false;
	}
	else
	{
		isYuanshuai = true;
	}
}

bool SceneUser::isYuanshuaiState() const
{
	return isYuanshuai;
}

//soke 宰相
void SceneUser::setZaixiangState(BYTE newstate)
{
	if (newstate == 0)
	{
		isZaixiang = false;
	}
	else
	{
		isZaixiang = true;
	}
}

bool SceneUser::isZaixiangState() const
{
	return isZaixiang;
}


struct TotalSeptGuard : public zSceneEntryCallBack
{
	private:
		SceneUser* master;
		BYTE cmd_type;

	public:
		TotalSeptGuard(SceneUser* u, BYTE cmd=0)
		{
			master = u;
			cmd_type = cmd;
		}
		
		bool exec(zSceneEntry *entry)
		{
			if (cmd_type == 0)
			{// 开始时统计人数
				if (entry && master->charbase.septid==((SceneUser*)entry)->charbase.septid)
				{
					SceneUser* u = (SceneUser*)entry;
					DWORD money = 0;
									
					if (u->charbase.level<50)
					{
						money = 20*100;
					}
					else if (u->charbase.level>=50 && u->charbase.level<70)
					{
						money = 60*100;
					}
					else if (u->charbase.level>=70 && u->charbase.level<90)
					{
						money = 100 * 100;
					}
					else if (u->charbase.level>=90 && u->charbase.level<110)
					{
						money = 140 * 100;
					}
					else if (u->charbase.level>=110 && u->charbase.level<130)
					{
						money = 180 * 100;
					}
					else if (u->charbase.level>=130)
					{
						money = 220 * 100;
					}
					
					if (u->packs.checkMoney(money) && u->packs.removeMoney(money, "家族运镖押金"))
					{
						SeptGuard tmp;
						tmp.id = u->id;
						tmp.money = money;
						tmp.is_finish = false;
						master->venterSeptGuard.push_back(tmp);
						
						Channel::sendSys(u, Cmd::INFO_TYPE_GAME, "扣除运镖押金 %u 文", money);
					}
					else
					{
						Channel::sendSys(u, Cmd::INFO_TYPE_FAIL, "银两不足 %u ,不能进入家族运镖", money);
					}
				}
			}
			else if (cmd_type == 1)
			{//结束时统计人数(首先查找, 如果在enterSeptGuard中找到,则加到完成人员列表中)
				for (unsigned int i=0; i<master->venterSeptGuard.size(); i++)
				{
					if (((SceneUser*)entry)->id == master->venterSeptGuard[i].id)
					{
						master->venterSeptGuard[i].is_finish = true;
						break;
					}
				}
			}

			return true;
		}
};

void SceneUser::enterSeptGuard()
{
	TotalSeptGuard sept(this);
	const zPosIVector &pv = scene->getNineScreen(getPosI());

	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		scene->execAllOfScreen(SceneEntry_Player, *it, sept);
	}

	SceneManager::CountryMap_iter country_iter = SceneManager::getInstance().country_info.find(this->charbase.country);
	if (country_iter != SceneManager::getInstance().country_info.end())
	{
		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP, " %s 的 %s 家族 开始家族运镖", country_iter->second.name, this->septName);
	}
}

void SceneUser::finishSeptGuard()
{
	TotalSeptGuard sept(this, 1);
	const zPosIVector &pv = scene->getNineScreen(getPosI());

	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		scene->execAllOfScreen(SceneEntry_Player, *it, sept);
	}

	int finishnum = 0;
	for (unsigned int i=0; i<venterSeptGuard.size(); i++)
	{
		if (venterSeptGuard[i].is_finish)
			finishnum++;
	}

	for (unsigned int i=0; i<venterSeptGuard.size(); i++)
	{
		SceneUser* pUser = SceneUserManager::getMe().getUserByID(venterSeptGuard[i].id);
		if (pUser && venterSeptGuard[i].is_finish)
		{	
			DWORD addExp = (80+5*finishnum)*pUser->charbase.level*pUser->charbase.level;
			pUser->addExp(addExp);
			Channel::sendSys(pUser, Cmd::INFO_TYPE_EXP, "得到经验值 %d", addExp);
			pUser->packs.addMoney(venterSeptGuard[i].money, "返还家族运镖押金");
		}
	}

	venterSeptGuard.clear();
}

bool SceneUser::isSafety(BYTE byType)
{
	if (this->safety==1 && this->temp_unsafety_state==0)
	{
		if (Cmd::isset_state((unsigned char*)&this->safety_setup, byType))
			return true;
	}

	return false;
}

#ifdef _TEST_DATA_LOG
void SceneUser::readCharTest(Cmd::Record::t_Read_CharTest_SceneRecord *rev)
{
	bcopy(&rev->chartest , &chartest , sizeof(chartest));
}
void SceneUser::writeCharTest(Cmd::Record::enumWriteBackTest_Type type)
{
	using namespace Cmd::Record;
	switch(type)
	{
		case NEWCHAR_WRITEBACK:
			{
				t_Insert_CharTest_SceneRecord ret;
				strncpy(ret.name , charbase.name , MAX_NAMESIZE);
				ret.level=1;
				bzero(&ret.chartest,sizeof(ret.chartest));
				bzero(&chartest,sizeof(chartest));
				recordClient->sendCmd(&ret, sizeof(ret));
			}
			break;
		case LEVELUP_WRITEBACK:
			{
				chartest.upgrade_usetime = charbase.onlinetime - chartest.upgrade_time;
				t_Update_CharTest_SceneRecord ret; 
				strncpy(ret.name , charbase.name , MAX_NAMESIZE);
				ret.level= charbase.level - 1;
				chartest.get_money =packs.getGoldNum() - chartest.money;
				bcopy(&chartest , &ret.chartest , sizeof(chartest));
				recordClient->sendCmd(&ret, sizeof(ret));

				t_Insert_CharTest_SceneRecord ret_1;
				strncpy(ret_1.name , charbase.name , MAX_NAMESIZE);
				ret_1.level=charbase.level;
				bzero(&chartest,sizeof(chartest));
				chartest.upgrade_time = charbase.onlinetime;
				chartest.money=packs.getGoldNum();
				bcopy(&chartest , &ret_1.chartest , sizeof(chartest));
				recordClient->sendCmd(&ret_1, sizeof(ret_1));
			}
			break;
		case DEATH_WRITEBACK:
			{
				t_Update_CharTest_SceneRecord ret; 
				strncpy(ret.name , charbase.name , MAX_NAMESIZE);
				ret.level= charbase.level;
				chartest.death_times++;
				bcopy(&chartest , &ret.chartest , sizeof(chartest));
				recordClient->sendCmd(&ret, sizeof(ret));
			}
			break;
		case HP_WRITEBACK:
			{
				t_Update_CharTest_SceneRecord ret; 
				strncpy(ret.name , charbase.name , MAX_NAMESIZE);
				ret.level = charbase.level;
				chartest.hp_leechdom++;
				bcopy(&chartest , &ret.chartest , sizeof(chartest));
				recordClient->sendCmd(&ret, sizeof(ret));
			}
			break;
		case MP_WRITEBACK:
			{
				t_Update_CharTest_SceneRecord ret; 
				strncpy(ret.name , charbase.name , MAX_NAMESIZE);
				ret.level = charbase.level;
				chartest.mp_leechdom++;
				bcopy(&chartest , &ret.chartest , sizeof(chartest));
				recordClient->sendCmd(&ret, sizeof(ret));
			}
			break;
		case SP_WRITEBACK:
			{
				t_Update_CharTest_SceneRecord ret; 
				strncpy(ret.name , charbase.name , MAX_NAMESIZE);
				ret.level = charbase.level;
				chartest.sp_leechdom++;
				bcopy(&chartest , &ret.chartest , sizeof(chartest));
				recordClient->sendCmd(&ret, sizeof(ret));
			}
			break;
		default:
			break;
	}
}
#endif // _TEST_DATA_LOG测试数据
//soke 温泉
void SceneUser::saveTodayGameTimeWhenLogout()
{
    lastIncTime=loginTime.elapse(SceneTimeTick::currentTime) / 1000;
    cm.addCounterDay(TODAY_GAME_TIME, lastIncTime); 
}

bool SceneUser::doMiniFuncCmd( const Cmd::MiniFuncUserCmd *rev, unsigned int cmdLen )
{
    if( NULL == rev ) return false;
    using namespace  Cmd;
    switch(rev->byParam)
    {
        case REQ_MALE_ENTER_SPA_MINIFUNC_PARA:   //soke 进入男温泉
        {
           //soke 温泉传送等级判断
		   const DWORD lv = 50;
           if ( charbase.level < lv )
           {
                Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "到达%d级才能进入温泉！", lv);
                return false;
           }
           if ( guard )
           {
              Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，你还有护送任务，完成再来吧");
             return false;
           }

		   DWORD needMoney = 1500; //soke 温泉价格15两

		   if (!packs.checkMoney(needMoney) || !packs.removeMoney(needMoney,"传送到温泉的手续费"))
		   {
			   Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您的银两不够！");
			   return true;
		   } 
		   else if (this->packs.removeMoney(needMoney,"传送到温泉的手续费"))
		   {
			   Gm::gomap(this, "name=中立区·男温泉 pos=22,37");
			   Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "进入温泉，扣除手续费，银子15两！");
			   return true;
		   }
        }
        break;
	case REQ_FEMALE_ENTER_SPA_MINIFUNC_PARA: //soke 进入女温泉
        {
		   //soke 温泉传送等级判断
		   const DWORD lv = 50;
           if ( charbase.level < lv )
           {
                Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "到达%d级才能进入温泉！", lv);
                return false;
           }
           if ( guard )
           {
              Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，你还有护送任务，完成再来吧");
             return false;
           }

           DWORD needMoney = 1500; //soke 温泉价格15两

		   if (!packs.checkMoney(needMoney) || !packs.removeMoney(needMoney,"传送到温泉的手续费"))
		   {
			   Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您的银两不够！");
			   return true;
		   } 
		   else if (this->packs.removeMoney(needMoney,"传送到温泉的手续费"))
		   {
		     //  Gm::gomap(this, "name=中立区·女温泉 pos=22,35");
			   Gm::gomap(this, "name=中立区·男温泉 pos=22,37");
			   Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "进入温泉，扣除手续费，银子15两！");
			   return true;
		   }
        }
        break;
    case REQ_LEAVE_SPA_MINIFUNC_PARA:
        {
			Gm::gomap(this, "name=中立区·世外桃源 pos=120,205");
            return true;
        }
        break;
	case REQ_END_FLY_MINIFUNC_PARA:
		{
			endFly();
			return true;
		}
		break;
	case REQ_BEGIN_WABAO_MINIFUNC_PARA:
		{
			beginWabao();
			return true;
		}
		break;
	case REQ_END_WABAO_MINIFUNC_PARA:
		{
			endWabao();
			return true;
		}
		break;
	case REQ_START_DANCE_OPERATE_PARA: //开始跳舞
		{
			beginDance();
			return true;
		}
		break;
	case REQ_STOP_DANCE_OPERATE_PARA:  //结束跳舞
		{
			endDance();
			return true;
		}
		break;
    default:
        break;
    }

    return true;
}

bool SceneUser::doBoxCmd(const Cmd::stCowBoxUserCmd *ptCmd,DWORD cmdLen)
{
    using namespace Cmd;
	switch(ptCmd->byParam)
	{
  //客户端发送钥匙被点击消息
	case SET_COWBOX_KEY_PARAMETER:
		{	
			Cmd::stSetCowBoxKeyCmd *boxkeyCmd = (Cmd::stSetCowBoxKeyCmd *)ptCmd;
			//if(this.packs.uom.exist(963, 1, 0 ,0)&&)
			//DWORD keythisid = this.packs.uom.exist(963, 1, 0 ,0)
			if(this->packs.uom.space(this) < 1)
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "请至少保留一个包裹空间.");
				return false;
			}
			DWORD boxID = boxkeyCmd->Key_id - 2;
			if(this->packs.uom.exist(boxID, 1, 0 ,0) == 0)
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "单把钥匙配合一个宝箱开启。");
				return false;
			}

			if(boxkeyCmd->isContinue)
			{
				if(this->packs.uom.exist(boxkeyCmd->Key_id, 1, 0 ,0) == 0)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "钥匙不足");
					return false;
				}
				this->reduceObjectNum(boxkeyCmd->Key_id,1,0);
			}
			else
			{
				zObject *delObj = this->packs.uom.getObjectByThisID(boxkeyCmd->qwThisID);
				if(delObj->data.dwNum > 1)
				{
					delObj->data.dwNum -=1;
					Cmd::stRefCountObjectPropertyUserCmd send;
					send.qwThisID = delObj->data.qwThisID;
					send.dwNum = delObj->data.dwNum;
					this->sendCmdToMe(&send, sizeof(send));
				}else
				{
					this->packs.removeObject(delObj);
				}
			}	
			
			this->reduceObjectNum(boxID,1,0);


			if( box_item.targetO != NULL)
			{
				zObject::destroy(box_item.targetO);
				box_item.targetO = NULL;
			}

			if( box_item.defaultO != NULL)
			{
				zObject::destroy(box_item.defaultO);
				box_item.defaultO = NULL;
			}

			box *_box = globalBox::getInstance().generateOneBox(boxkeyCmd->Key_id,this->charbase.type == 1 ? 1 : 2,this->charbase.level); //修复换头像不显示昵称

			stGetCowBoxInitCmd initBoxCmd;
			initBoxCmd.Key_id = boxkeyCmd->Key_id;
			initBoxCmd.isContinue = boxkeyCmd->isContinue;
			initBoxCmd.item_Ti = zMisc::randBetween(1,16);

			zObject* tempOs[17];
			bzero(tempOs,sizeof(zObject*)*17);

			bool CreateError = false;

			for(int i = 0; i < 17; ++i)
			{
				zObjectB *base = objectbm.get(_box->_boxObjects[i].obj_id);
				if(NULL == base)
				{
					CreateError = true;
					break;
				}
				zObject* o = NULL;
				o = zObject::create(base,1,_box->_boxObjects[i].itemlevel);
				if(NULL == o)
				{
					CreateError = true;
					break;
				}
                o->data.dwNum = _box->_boxObjects[i].num;
				o->data.kind = _box->_boxObjects[i].itemkind;
				o->data.bind = 1;
				o->base->level =_box->_boxObjects[i].material;
				//if(_box->_boxObjects[i].itemkind == 3)
				//	_box->_boxObjects[i].itemkind = 4;
				if(o->base->make == 1)
				{
					base->goldrating = 10000;
					base->holyrating = 100000;
					EquipMaker maker(NULL);
					maker.modify_addmaker(o,this);
					if(this->charbase.round)
						o->data.round = this->charbase.round;
					if ( o->data.kind & 16 )
					{
						maker.modify_attri16(o,base);
						maker.repur_upgrade(o);
					}
					else
					{
						maker.modify_attri(o,base);
						maker.re_upgrade(o);
					}
				}

				bcopy(&o->data,&initBoxCmd.objects[i],sizeof(t_Object));
				if(i == 0)
				    box_item.defaultO = o;
				if(i == initBoxCmd.item_Ti)
					box_item.targetO = o;
				tempOs[i] = o;
			}

			SAFE_DELETE(_box);
			if(CreateError)//创建对象出错
			{
				Zebra::logger->debug("[宝箱]:用户%s开启宝箱%d失败.", this->charbase.name,boxkeyCmd->Key_id);
				for(int i = 0; i < 17; ++i)
				{
					if(tempOs[i] != NULL)
					zObject::destroy(tempOs[i]);
				}
				//delete box_item;
				//box_item = NULL;
				box_item.defaultO = NULL;
				box_item.targetO = NULL;
				return true;

			}
			Zebra::logger->debug("[宝箱]:用户%s开启宝箱%d成功.", this->charbase.name,boxkeyCmd->Key_id);
			this->sendCmdToMe(&initBoxCmd,sizeof(initBoxCmd));

			//销毁除默认物品和目标物品以外的所有物品
			for(int i = 0; i < 17; ++i)
			{
				if(tempOs[i] != box_item.targetO && tempOs[i] != box_item.defaultO)
				{
					if(tempOs[i] != NULL)
					zObject::destroy(tempOs[i]);
				}
			}
			return true;
		}
		break;
  //用户点击宝盒控件的 提取 按键
	case SET_COWBOX_TIQU_ITEM:
		{
			Zebra::logger->debug("提取物品\n");
			if(this->packs.uom.space(this) < 1)
				return false;
			Cmd::stSetCowBoxTiquCmd *fetchCmd = (Cmd::stSetCowBoxTiquCmd *)ptCmd;

			zObject *targetO;
			if(false == fetchCmd->item_id)
			{
				if(box_item.defaultO != NULL)
					zObject::destroy(box_item.defaultO);
				targetO = box_item.targetO;
				box_item.targetO = NULL;
				box_item.defaultO = NULL;
			}
			else
			{
				if(box_item.targetO != NULL)
					zObject::destroy(box_item.targetO);
				targetO = box_item.defaultO;
				box_item.defaultO = NULL;
				box_item.targetO = NULL;
			}
			
            if(targetO != NULL)
			{
				zObject *o = NULL;

			    o = zObject::create(targetO);

				if(o != NULL)
				{
					this->addBindObjectNum(o->base->id, o->data.dwNum, 0, 1, true);
					if (((o->base->kind >= 101 && o->base->kind <= 118) || (o->base->kind >= 141 && o->base->kind <= 147) || (o->base->kind >= 130 && o->base->kind <= 134) || (o->base->kind == 136) || (o->base->kind == 137) || (o->base->kind == 155)) && (o->base->level >= 1))
					{
						if (o->data.upgrade >= 10 && o->data.upgrade <= 14)
						{
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "%s的%s开启宝箱获得:%d星神圣装备%s", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name, o->data.upgrade, o->data.strName);
						}
						else if (o->data.upgrade == 15)
						{
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP3, "%s的%s开启宝箱获得:%d星神圣装备%s", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name, o->data.upgrade, o->data.strName);
						}
						else
						{
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP, "%s的%s开启宝箱获得:神圣装备%s", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name, o->data.strName);
						}
						Zebra::logger->debug("玩家%s打开密银宝盒获得 %d 星装备 %s ", this->charbase.name, o->data.upgrade, o->data.strName);
					}
					else if (o->base->kind == 16 && o->base->level >= 1)
					{
						if (o->data.upgrade == 3) // 4等材料绿色通知
						{
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "%s的%s开启宝箱获得精品四等材料 %s", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name, o->data.strName);
						}
						else if (o->data.upgrade == 4) // 5等材料紫色通知
						{
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP3, "%s的%s开启宝箱获得精品五等材料 %s", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name, o->data.strName);
						}
						else if (o->data.upgrade == 5) // 6等材料橙色通知
						{
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP3, "%s的%s开启宝箱获得极品六等材料 %s", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name, o->data.strName);
						}
						else if (o->data.upgrade == 6) // 7等红色通知
						{
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "%s的%s开启宝箱获得极品七等材料 %s", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name, o->data.strName);
						}
						else
						{
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP, "%s的%s开启宝箱获得材料 %s", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name, o->data.strName);
						}
						Zebra::logger->debug("玩家%s打开密银宝盒获得 %d 等材料物品 %s %d个 ", this->charbase.name, o->data.upgrade, o->data.strName, o->data.dwNum);
					}
					else
					{
						if (o->base->level == 1) // 黄色通知
						{
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP, "%s的%s开启宝箱获得物品:%s", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name, o->data.strName);
						}
						else if (o->base->level == 2) // 绿色通知
						{
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "%s的%s开启宝箱获得物品:%s", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name, o->data.strName);
						}
						else if (o->base->level == 3) // 紫色通知
						{
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP3, "%s的%s开启宝箱获得物品:%s", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name, o->data.strName);
						}
						else if (o->base->level == 4) // 橙色通知
						{
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP4, "%s的%s开启宝箱获得物品:%s", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name, o->data.strName);
						}
						else if (o->base->level == 5) // 红色通知
						{
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "%s的%s开启宝箱获得物品:%s", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name, o->data.strName);
						}
						Zebra::logger->debug("玩家%s打开密银宝盒获得材料物品 %s %d个  ", this->charbase.name, o->data.strName, o->data.dwNum);
					}

					return true;
				}
				else
				{ 
					zObject::destroy(targetO); 	
					return true;		
				}
											
	    	}
			return false;
		}
		break;
    default:
      break;
  }
  return true;
}
///////////////////////////////////////////修复箱子结束


bool SceneUser::doLuaUserCmd(const Cmd::stLuaInputBoxUserCmd *rev, unsigned int cmdLen)
{
	if( NULL == rev ) return false;
    switch(rev->byParam)
    {
		case LUA_INPUTBOX_USERCMD_TEY:
			{
				
				Cmd::stLuaInputBoxUserCmd * cmd = (Cmd::stLuaInputBoxUserCmd *)rev;
				Zebra::logger->debug("收到回复消息:%s\n",cmd->funName);
				//const char* funname= cmd->funName;
				//std::string str = cmd->text;
				char* pcan1 = strtok(cmd->funName, "-");
				char* pcan2 = strtok(NULL, "-");
				if(pcan1 == NULL)
				{
					execute_script_event(this,cmd->funName,cmd->text);
				}else
				{
					execute_script_event(this,pcan1,cmd->text,pcan2);
				}
				return true;
			}
			break;
		default:
			break;
    }

    return true;
}
static   const DWORD SPA_TIME = 30*60; //half hour 每日温泉最大时间30分钟
bool SceneUser::enterSpa()
{
	if (cartoon) //保存跟随的宠物
    {
		cartoon->putAway(Cmd::Session::SAVE_TYPE_PUTAWAY);
    }

    //soke 温泉到时间后离开，不能在进入温泉
    if ( cm.getCounter(SPA_TIME_IN_WANGCHENG) >= SPA_TIME ) //soke 如果泡澡时间>= 总时间（30分钟）
    {
 //       Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "今天你已经泡过温泉了，明天再来吧");
        return false;
    }


	bool isInSpaZone = scene->checkZoneType(pos, ZoneTypeDef::ZONE_SPA_WARM) || scene->checkZoneType(pos, ZoneTypeDef::ZONE_SPA_HOT);
    /*if ( isInSpaZone )
    {
		// 更新客户端状态
        showCurrentEffect( Cmd::USTATE_SPA, true );
	}
	else
	{
		showCurrentEffect( Cmd::USTATE_SPA, false );
	}*/
 
	if ( isInSpaZone )
    {
        if (horse.horse())
        {
            if (horse.mount())
                horse.mount(false);
        }
        // 更新客户端状态
        showCurrentEffect( Cmd::USTATE_SPA, true );
        cm.addCounterDay(SPA_TIME_IN_WANGCHENG); //soke 进入温泉开始计时

        Cmd::SyncSpaTimeMiniFuncUserCmd send;
        send.spaTime = SPA_TIME - cm.getCounter(SPA_TIME_IN_WANGCHENG); //soke 显示温泉剩余时间

        sendCmdToMe(&send, sizeof(send));

        //控制移动速度
        Cmd::stAddUserMapScreenUserCmd cmd;
        full_t_MapUserData(cmd.data);
        sendCmdToMe(&cmd,sizeof(cmd));				     
    }
    return true;
}

void SceneUser::spaTimer()
{
    if( !isSpa() ) return ;

    if( cm.getCounter( SPA_TIME_IN_WANGCHENG) <= SPA_TIME ) //soke 温泉时间限制
    {
        bool isInSpaZone = scene->checkZoneType(pos, ZoneTypeDef::ZONE_SPA_WARM);
        if ( isInSpaZone )
        {
            addExp( getSpaExp(charbase.level) );
        }
        else if (scene->checkZoneType(this->pos, ZoneTypeDef::ZONE_SPA_HOT))
        {
            SDWORD damHp = -getHotSpaDam();
            changeHP(damHp);
            if ( 0 == charbase.hp) //soke 如果生命值=0
            {
                //发国家公告 
                Channel::sendCountryInfo( scene->getCountryID(), Cmd::INFO_TYPE_EXP, "%s被开水烫至严重昏迷，光着身子被抬了出去。", name);
                Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您被开水烫晕了，请出去冷静一下。");
                Gm::gomap(this, "name=中立区·世外桃源 pos=120,205");
            }
            else
            {
                addExp( getSpaRedExp(charbase.level) );
            }
        }
        cm.addCounterDay(SPA_TIME_IN_WANGCHENG, 15); //soke 计时周期，进入温泉离开在进入，减少时间15秒左右
    }
    else
    {
		 if ( cm.getCounter(SPA_TIME_IN_WANGCHENG) > SPA_TIME ) //soke 如果超过时间未离开温泉，给出提示
         {
           Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您今天已经泡满30分钟，将无法继续获得经验。");
		   showCurrentEffect( Cmd::USTATE_SPA, false );
		 }
    }
}

DWORD SceneUser::getSpaExp( DWORD lv )
{
    //温泉公式 
    DWORD exp =  (charbase.level * charbase.level);
   
    return exp;
}

DWORD SceneUser::getSpaRedExp( DWORD lv )
{
    //温泉公式 
    DWORD exp =  (2 * charbase.level * charbase.level);

    return exp;
}

DWORD SceneUser::getHotSpaDam()
{
    //伤害公式 maxhp的90%
    DWORD dam = 500;
    if ( 0 == dam)
    {
        dam = 1;
    }
    return dam;
}
//////////////////////////////////////////////////////////////////////////
DWORD SceneUser::getFBCounterID( DWORD fbType )
{
    DWORD cntID = 0;
    switch ( fbType )
    {
    case 1:
        cntID = FUBEN_TEYP_1;
        break;
    case 2:
        cntID = FUBEN_TEYP_2;
        break;
    case 3:
        cntID = FUBEN_TEYP_3;
        break;
    case 4:
        cntID = FUBEN_TEYP_4;
        break;
    case 5:
        cntID = FUBEN_TEYP_5;
        break;
    case 6:
        cntID = FUBEN_TEYP_6;
        break;
    case 7:
        cntID = FUBEN_TEYP_7;
        break;
    case 8:
        cntID = FUBEN_TEYP_8;
        break;
    case 9:
        cntID = FUBEN_TEYP_9;
        break;
    case 10:
        cntID = FUBEN_TEYP_10;
        break;
    case 11:
        cntID = FUBEN_TEYP_11;
        break;
    case 12:
        cntID = FUBEN_TEYP_12;
        break;
    case 13:
        cntID = FUBEN_TEYP_13;
        break;
    case 14:
        cntID = FUBEN_TEYP_14;
        break;
    case 15:
        cntID = FUBEN_TEYP_15;
        break;
    default:
        break;
    }
    return cntID;
}

//家族地图打坐
void SceneUser::jiazuTimer()
{
    if( !isSitdown() )
	{
		if(this->issetUState(Cmd::USTATE_XIULIAN))
		{
			this->clearUState(Cmd::USTATE_XIULIAN);
		}
		return;
	} 
	if (strstr(this->scene->getRealName(),"家族地图"))//在家族地图
	{	
		struct tm tv1;
		time_t timValue = time(NULL);
		zRTime::getLocalTime(tv1, timValue);
		if (tv1.tm_hour >=0 && tv1.tm_hour <24)
		{
			if( cm.getCounter( JIAZU_TIME_IN_WANGCHENG) <= 15*60 ) //soke 温泉时间限制
			{
				if(!this->issetUState(Cmd::USTATE_XIULIAN))
				{
					this->setUState(Cmd::USTATE_XIULIAN);
				}
				if(this->charbase.charvip>0)
				{
					this->charbase.xiulianexp += 40; //修炼经验
					Channel::sendSys(this,Cmd::INFO_TYPE_GAME,"参与家族地图打坐，获得修炼经验20点，VIP额外获得20点");
				}
				else{
					this->charbase.xiulianexp += 20; //修炼经验
					Channel::sendSys(this,Cmd::INFO_TYPE_GAME,"参与家族地图打坐，获得修炼经验20点");
				}
				
				cm.addCounterDay(JIAZU_TIME_IN_WANGCHENG, 15); //soke 计时周期，进入温泉离开在进入，减少时间15秒左右
				
			
			}
			else
			{
				if ( cm.getCounter(JIAZU_TIME_IN_WANGCHENG) > 15*60 ) //soke 如果超过时间未离开温泉，给出提示
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您今天已经打坐15分钟，将无法继续获得修为经验。");
					if(this->issetUState(Cmd::USTATE_XIULIAN))
					{
						this->clearUState(Cmd::USTATE_XIULIAN);
					}
					
				}
			}
		}				
	}
	else
	{
		if(this->issetUState(Cmd::USTATE_XIULIAN))
		{
			this->clearUState(Cmd::USTATE_XIULIAN);
		}
	}
   
}

DWORD SceneUser::getFBOpenCount( DWORD fbType )
{
    DWORD counterID = getFBCounterID(fbType);
    DWORD fbTempID = 0;
    DWORD openCnt = 0;
    DWORD enterCnt = 0;
    getFuBenInfo(counterID, fbTempID, openCnt, enterCnt);
    return openCnt;
}

DWORD SceneUser::getFBEnterCount( DWORD fbType )
{
    DWORD counterID = getFBCounterID(fbType);
    DWORD fbTempID = 0;
    DWORD openCnt = 0;
    DWORD enterCnt = 0;
    getFuBenInfo(counterID, fbTempID, openCnt, enterCnt);
    return enterCnt;
}
//////////////////////////////////////////////////////////
//TZ 2023 10 15 新商城增加
bool SceneUser::MallTradeObject(Cmd::stBuyObjectMallUserCmd *ptCmd, zObjectB *baseobject)
{
	if(baseobject == NULL)
	{

		Channel::sendSys(this,Cmd::INFO_TYPE_FAIL,"道具获取失败。");
		return false;
	}
	//充值点购买
	if(charbase.chongzhi >= baseobject->price*ptCmd->dwNum)
	{

		MallTrade::MallItem item;
		
		item.id = baseobject->id;
		item.kind = baseobject->kind;
		item.action = 1;

        //增加空间不足提示
		if(this->packs.uom.space(this) < 1)
		{
			Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "包裹空间不足，请最少保留1个空包裹格子！");
			return false;
		}
		//空间足执行下一段
		else if(MallTrade::getInstance().verifyMallTrade(item))
		{
		
			zObject* object = NULL;
			//充值点扣除
			packs.removeChongzhi(baseobject->price*ptCmd->dwNum,"买东西");						
			object = zObject::create(baseobject,ptCmd->dwNum,0);
			
			if(object)
			{

				Combination callback(this, object);
				packs.main.execEvery(callback);
				if (packs.equip.pack(EquipPack::L_PACK)) 
					packs.equip.pack(EquipPack::L_PACK)->execEvery(callback);
				if (packs.equip.pack(EquipPack::R_PACK))
					packs.equip.pack(EquipPack::R_PACK)->execEvery(callback);

				if (object->data.dwNum) 
			{
			//增加购买绑定	
			{
				object->data.bind=1;
			}									
			///////////////////////////////////			
				if (packs.addObject(object, true, AUTO_PACK)) 
				{						
					Cmd::stAddObjectPropertyUserCmd status;
						
					status.byActionType = Cmd::EQUIPACTION_OBTAIN;
						
					bcopy(&object->data, &status.object, sizeof(t_Object));
						
					sendCmdToMe(&status, sizeof(status));
					//充值点新商城购买目前购买是绑定的
					Channel::sendChongzhi(this, Cmd::INFO_TYPE_GAME, baseobject->price*ptCmd->dwNum,"得到物品 %s(%d)个, 花费充值点",baseobject->name , ptCmd->dwNum);				
					Channel::sendAllInfo(Cmd::INFO_TYPE_YANSE1," 哇～～ %s 在点数商城购买了 %s 真是壕无人性啊！！！ ",this->name, baseobject->name);
										
					return true;						
					}
				}				
			}
		}
		else
			Channel::sendSys(this,Cmd::INFO_TYPE_FAIL,"商品还没上架！");
	}
	    else
	{
		Channel::sendSys(this,Cmd::INFO_TYPE_GAME5,"您的充值点不足");
	}
	return true;
}
////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
//soke 开始飞行
bool SceneUser::beginFly()
{
	if(isFly())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "已经在飞行了");
		return false;
	}

	if(isSpa())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "游泳状态下不能飞行");
		return false;
	}

	if(isBike())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "自行车比赛状态下不能飞行");
		return false;
	}
	
	if(isZC())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "战车状态下不能飞行");
		return false;
	}
	if(isWabao())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "挖宝状态下不能飞行");
		return false;
	}

	if (cartoon)
	{
		cartoon->putAway(Cmd::Session::SAVE_TYPE_PUTAWAY);    
	}

	showCurrentEffect( Cmd::USTATE_USER_FLY, true );

	Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "开始飞行.");

	Cmd::stAddUserMapScreenUserCmd cmd;
	full_t_MapUserData(cmd.data);
	sendCmdToMe(&cmd,sizeof(cmd));

	return true;
}

//soke 结束飞行
void SceneUser::endFly()
{
	if(!isFly()) return;

	showCurrentEffect( Cmd::USTATE_USER_FLY, false );

	Cmd::stAddUserMapScreenUserCmd cmd;
	full_t_MapUserData(cmd.data);
	sendCmdToMe(&cmd,sizeof(cmd));

	Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "安全着陆.");
}

//soke 开始挖宝
bool SceneUser::beginWabao()
{
	horse.mount(false); //soke 挖宝要下马
	if(isWabao())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "已经在挖宝了");
		return false;
	}

	if(isFly())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "飞行状态下不能挖宝");
		return false;
	}

	if(isSpa())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "游泳状态下不能挖宝");
		return false;
	}

	if(isBike())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "自行车比赛状态下不能挖宝");
		return false;
	}

	if(isZC())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "战车状态下不能挖宝");
		return false;
	}

	if(isDance())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "跳舞状态下不能挖宝");
		return false;
	}

	if (cartoon)
	{
		cartoon->putAway(Cmd::Session::SAVE_TYPE_PUTAWAY);    
	}

	showCurrentEffect( Cmd::USTATE_USER_WABAO, true );

	Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "进入挖宝状态。");

	Cmd::stAddUserMapScreenUserCmd cmd;
	full_t_MapUserData(cmd.data);
	sendCmdToMe(&cmd,sizeof(cmd));

	return true;
}

//soke 结束挖宝
void SceneUser::endWabao()
{
	if(!isWabao()) return;

	showCurrentEffect( Cmd::USTATE_USER_WABAO, false );

	Cmd::stAddUserMapScreenUserCmd cmd;
	full_t_MapUserData(cmd.data);
	sendCmdToMe(&cmd,sizeof(cmd));

	Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "离开挖宝状态。");
}

//soke 挖宝时间计算
void SceneUser::wabaoTimer()
{
    if( !scene || !isWabao() ) return ;

	if (this->issetUState(Cmd::USTATE_USER_WABAO ))
	{
        bool isInDanceZone = scene->checkZoneType(pos, ZoneTypeDef::ZONE_DANCE);
        if ( isInDanceZone )
        {
			 DWORD exp = (charbase.level*50);
             addExp( exp );
	    }
	}
}

//开始跳舞
bool SceneUser::beginDance()
{
	//sky 检测骑马状态
	if (horse.mount())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "骑马状态下不可以跳舞！.");
		return true;
	}

	if(isWabao())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "跳舞状态下不能挖宝");
		return false;
	}
	if(isDance())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "已经在跳舞了.");
		return false;
	}

	if(isSpa())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您在游泳.不能跳舞.");
		return false;
	}

	if(isBike())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您在进行自行车比赛.不能跳舞.");
		return false;
	}

	if(isZC())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您在战车状态.不能跳舞.");
		return false;
	}

	if(isSitdown())
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "打坐状态下不能跳舞"); //新增打坐不能跳舞
		return false;
	}

	if (cartoon)
	{
		cartoon->putAway(Cmd::Session::SAVE_TYPE_PUTAWAY);    
	}

	showCurrentEffect( Cmd::USTATE_USER_DANCE, true );

	Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "开始跳舞.");

	Cmd::stAddUserMapScreenUserCmd cmd;
	full_t_MapUserData(cmd.data);
	sendCmdToMe(&cmd,sizeof(cmd));

	return true;
}

void SceneUser::endDance()
{ 
	if(!isDance()) return;

	showCurrentEffect( Cmd::USTATE_USER_DANCE, false );

	Cmd::stAddUserMapScreenUserCmd cmd;
	full_t_MapUserData(cmd.data);
	sendCmdToMe(&cmd,sizeof(cmd));

	Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "停止跳舞.");
}

//soke 排行榜前三排名特效激活
void SceneUser::Refreshleaderboard()
{	
	// //皇城战
	// if(this->charbase.country == 12 )
    // {	
	// 	this->packs.m_Chenghao[10].state = 1;
    // }
    // else
    // {
	//     this->packs.m_Chenghao[10].state = 0;
    // }
	//天下第一
    if(this->charbase.tianxiacount == 1 )
    {	
		this->packs.m_Chenghao[8].state = 1;
    }
	else if(this->charbase.tianxiacount == 2 )
    {	
		this->packs.m_Chenghao[9].state = 1;
    }
    else
    {
	    this->packs.m_Chenghao[8].state = 0;
		this->packs.m_Chenghao[9].state = 0;
    }
	//鲜花榜特效
    if(this->charbase.xhbbcount == 1 )
    {	
		if (this->charbase.folwers >= 1)
		{
			this->packs.m_Chenghao[0].state = 1;
		}
    }
    else
    {
	    this->packs.m_Chenghao[0].state = 0;
    }

	// //战力榜特效
	// if(this->charbase.zhanlicount == 1)
    // {	
	// 	this->packs.m_Chenghao[2].state = 1;
	// 	this->packs.m_Chenghao[3].state = 0;
	// 	this->packs.m_Chenghao[4].state = 0;
	// 	this->packs.m_Chenghao[5].state = 0;
    // }
	// else if(this->charbase.zhanlicount == 2)
    // {	
	// 	this->packs.m_Chenghao[2].state = 0;
	// 	this->packs.m_Chenghao[3].state = 1;
	// 	this->packs.m_Chenghao[4].state = 0;
	// 	this->packs.m_Chenghao[5].state = 0;
    // }
	// else if(this->charbase.zhanlicount == 3)
    // {	
	// 	this->packs.m_Chenghao[2].state = 0;
	// 	this->packs.m_Chenghao[3].state = 0;
	// 	this->packs.m_Chenghao[4].state = 1;
	// 	this->packs.m_Chenghao[5].state = 0;
    // }
	// else if(this->charbase.zhanlicount >3 && this->charbase.zhanlicount <=10)
    // {
	// 	this->packs.m_Chenghao[2].state = 0;
	// 	this->packs.m_Chenghao[3].state = 0;
	// 	this->packs.m_Chenghao[4].state = 0;
	// 	this->packs.m_Chenghao[5].state = 1;
    // }
    // else
    // {
		this->packs.m_Chenghao[2].state = 0;
		this->packs.m_Chenghao[3].state = 0;
		this->packs.m_Chenghao[4].state = 0;
		this->packs.m_Chenghao[5].state = 0;
    // }

	//护国榜特效
	if(this->charbase.herocount == 1)
    {	
		this->packs.m_Chenghao[1].state = 1;
    }
    else
    {
		this->packs.m_Chenghao[1].state = 0;
    }

	//连斩特效
	if(this->charbase.lianzhanmaxnum >= 100)
    {	
	   this->packs.m_Chenghao[6].state = 1;
    }
    else
    {
		this->packs.m_Chenghao[6].state = 0;
    }

	//有称号装备了  称号特效
	if(this->charbase.chenghao_select !=0)
    {	
	    if (!this->issetUState(fjconfig::getInstance().chenghaolist[charbase.chenghao_select].stateID))
        {
            showCurrentEffect(fjconfig::getInstance().chenghaolist[charbase.chenghao_select].stateID, true );  
        }
		for(int i=1;i<=60;i++)
		{
			if(i!=charbase.chenghao_select)//清除其他称号
			{
				if(fjconfig::getInstance().chenghaolist[i].stateID != 0)
				{	
					if(i!=0)
					{
						showCurrentEffect(fjconfig::getInstance().chenghaolist[i].stateID, false );
					}
				}
				
				
			}
		}
    }
    else
    {
		//否则清空配置文件id不为0的全部状态
		for(int i=1;i<=60;i++)
		{
			if(fjconfig::getInstance().chenghaolist[i].stateID != 0)
			{
				if(i!=0)
				{
					showCurrentEffect(fjconfig::getInstance().chenghaolist[i].stateID, false );
				}
			}
		}
	    
    }
  
}
