/**
 * \file
 * \version  $Id: Rush.cpp $
 * \author  
 * \date 
 * \brief 攻城处理的类
 *
 * 
 */

#include "zXMLParser.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SceneNpc.h"
#include "TimeTick.h"
#include "Rush.h"
#include "SceneNpcAI.h"
#include "Chat.h"
#include "SessionClient.h"
	
/**
 * \brief 构造函数，不进行初始化
 *
 * \param rushID 攻城脚本的id
 * \param rushDelay 攻城延迟时间（boss复活时间）
 * \param countryID 目标国家id
 */
Rush::Rush(DWORD rushID, DWORD rushDelay, DWORD countryID)
:curPhase(0),id(rushID),countryID(countryID),bossID(0),boss(0),summonTime(0),lastBossHp(0),canSummon(true),clearDelay(0),end(false),lasttime(0),rushDelay(rushDelay)
{
	bzero(rushName, sizeof(rushName));
	bzero(bossName, sizeof(bossName));
	bzero(mapName, sizeof(mapName));
	bzero(text, sizeof(text));

	//summonCount = 0;
}

/**
 * \brief 初始化攻城
 * 读取脚本并向目标场景添加本次攻城
 *
 * \return 初始化是否成功
 */
bool Rush::init(Scene * s)
{
	My_FunctionTime_wrapper(__PRETTY_FUNCTION__);
	if (!loadRushData(id, rushDelay, countryID)) return false;
	setEndTime((rushDelay+lasttime)*1000);

	if (s)
	{
		strncpy(mapName, s->name, MAX_NAMESIZE);
		s->addRush(this);
	}
	else
	{
		Scene *scene=SceneManager::getInstance().getSceneByName(mapName);
		if (!scene) return false;
		scene->addRush(this);
	}

	if (0==strncmp("", text, 128))
	{
		/*
		Cmd::Session::t_cityRush_SceneSession send;
		bzero(send.bossName, MAX_NAMESIZE);
		bzero(send.rushName, MAX_NAMESIZE);
		bzero(send.mapName, MAX_NAMESIZE);
		strncpy(send.bossName, bossName, MAX_NAMESIZE-1);
		strncpy(send.rushName, rushName, MAX_NAMESIZE-1);
		strncpy(send.mapName, mapName, MAX_NAMESIZE-1);
		send.delay = rushDelay;
		send.countryID = countryID;
		sessionClient->sendCmd(&send, sizeof(send));
		*/
	}
	else
	{
		Cmd::Session::t_cityRushCust_SceneSession send;
		bcopy(text, send.text, 128);
		Zebra::logger->debug("%s", text);
		send.countryID = countryID;
		sessionClient->sendCmd(&send, sizeof(send));
	}

	return true;
}

/**
 * \brief 析构函数
 * npc的删除在SceneNpc::AI里做
 *
 */
Rush::~Rush()
{
	/*
	if (boss)
	{
		SceneNpcManager::getMe().removeSceneNpc(boss);
		boss->scene->removeNpc(boss);
		SAFE_DELETE(boss);
	}
	*/
}

/**
 * \brief 根据字符串得到攻城的动作
 *
 * \param action 动作名字
 * \return 动作枚举值
 */
Rush::rushAction Rush::parseAction(const char * action)
{
	if (!strcmp(action, "moveto")) return RUSH_MOVETO;
	if (!strcmp(action, "attack")) return RUSH_ATTACK;
	if (!strcmp(action, "summon")) return RUSH_SUMMON;
	if (!strcmp(action, "summon_rush")) return RUSH_SUMMON_RUSH;
	if (!strcmp(action, "recover")) return RUSH_RECOVER;
	if (!strcmp(action, "summon_on")) return RUSH_SUMMON_ON;
	if (!strcmp(action, "summon_off")) return RUSH_SUMMON_OFF;
	if (!strcmp(action, "clear")) return RUSH_CLEAR;
	if (!strcmp(action, "summon_pet")) return RUSH_SUMMON_PET;
	if (!strcmp(action, "broadcast")) return RUSH_BROADCAST;
	return RUSH_NORMAL;
}

/**
 * \brief 读取攻城脚本
 *
 * \param rushID 脚本id
 * \param rushDelay 攻城延迟时间
 * \param countryID 目标国家id
 * \return 读取是否成功，不成功则不可用
 */
bool Rush::loadRushData(DWORD rushID, DWORD rushDelay, DWORD countryID)
{
	zXMLParser xml;
	if (!xml.initFile(Zebra::global["mapdir"] +"city_rush.xml"))
	{
		Zebra::logger->error("打开Rush配置文件失败 %s", (Zebra::global["mapdir"] + "city_rush.xml").c_str());
		return false;
	}

	xmlNodePtr root = xml.getRootNode("info");
	if (!root) return false;
	xmlNodePtr rushNode = xml.getChildNode(root, "rush");

	while (rushNode)
	{
		unsigned int id=0;
		xml.getNodePropNum(rushNode, "id", &id, sizeof(id));
		if (rushID==id)
		{
			xml.getNodePropNum(rushNode, "boss", &bossID, sizeof(bossID));
			xml.getNodePropStr(rushNode, "bossname", bossName, sizeof(bossName));
			xml.getNodePropStr(rushNode, "name", rushName, sizeof(rushName));
			xml.getNodePropNum(rushNode, "cleardelay", &clearDelay, sizeof(clearDelay));
			xml.getNodePropNum(rushNode, "lasttime", &lasttime, sizeof(bossID));
			
			zXMLParser parser = zXMLParser();
			parser.initFile(Zebra::global["sceneinfofile"].c_str());
			xmlNodePtr root2 = parser.getRootNode("ScenesInfo");
			if (0==root2) return false;
			xmlNodePtr infoNode = parser.getChildNode(root2,"countryinfo");
			if (0==infoNode) return false;
			
			char str[MAX_NAMESIZE];
			xmlNodePtr dataNode = parser.getChildNode(infoNode,"country");
			while (dataNode)
			{                       
				unsigned int id=0;
				parser.getNodePropNum(dataNode, "id", &id, sizeof(id));
				if (id==countryID)
				{
					parser.getNodePropStr(dataNode, "name", str, sizeof(str));
					break;
				}
				dataNode = parser.getNextNode(dataNode, "country");
			}
			if (!dataNode)
			{
				Zebra::logger->debug("读取攻城数据时，未找到目标国家 id=%d", countryID);
				return false;
			}
			strcpy(mapName, str);
			
			unsigned int mapID;
			xml.getNodePropNum(rushNode, "mapid", &mapID, sizeof(mapID));
			infoNode = parser.getChildNode(root2,"mapinfo");
			if (0==infoNode) return false;
			dataNode = parser.getChildNode(infoNode,"map");
			while (dataNode)
			{                       
				unsigned int id=0;
				parser.getNodePropNum(dataNode, "mapID", &id, sizeof(id));
				if (id==mapID)
				{
					parser.getNodePropStr(dataNode, "name", str, sizeof(str));
					break;
				}
				dataNode = parser.getNextNode(dataNode, "map");
			}
			if (!dataNode)
			{
				Zebra::logger->debug("读取攻城数据时，未找到目标地图 id=%d", mapID);
				return false;
			}
			strcat(mapName, "·");
			strcat(mapName, str);

			dataNode = xml.getChildNode(rushNode, "boss");
			while (dataNode)
			{
				bossDefine bd;
				xml.getNodePropNum(dataNode, "x", &bd.pos.x, sizeof(bd.pos.x));
				xml.getNodePropNum(dataNode, "y", &bd.pos.y, sizeof(bd.pos.y));
				xml.getNodePropNum(dataNode, "num", &bd.num, sizeof(bd.num));
				xml.getNodePropNum(dataNode, "region", &bd.region, sizeof(bd.region));
				xml.getNodePropNum(dataNode, "script", &bd.script, sizeof(bd.script));

				bossVector.push_back(bd);

				dataNode = xml.getNextNode(dataNode, "boss");
			}

			dataNode = xml.getChildNode(rushNode, "servant");
			while (dataNode)
			{
				servantDefine sd;
				xml.getNodePropNum(dataNode, "id", &sd.id, sizeof(sd.id));
				xml.getNodePropNum(dataNode, "num", &sd.num, sizeof(sd.num));
				xml.getNodePropNum(dataNode, "rate", &sd.rate, sizeof(sd.rate));
				xml.getNodePropNum(dataNode, "interval", &sd.interval, sizeof(sd.interval));

				servantVector.push_back(sd);

				dataNode = xml.getNextNode(dataNode, "servant");
			}

			dataNode = xml.getChildNode(rushNode, "notify");
			if (dataNode)
				xml.getNodePropStr(dataNode, "str", text, sizeof(text));

			char t[128];
			std::string::size_type pos=0;
			dataNode = xml.getChildNode(rushNode, "starttext");
			if (dataNode)
			{
				xml.getNodePropStr(dataNode, "str", t, sizeof(t));
				startText = t;
				pos = startText.find("time", 0);
				if (pos!=std::string::npos)
					startText.replace(pos, 4, "%s");
			}

			dataNode = xml.getChildNode(rushNode, "endtext");
			if (dataNode)
			{
				xml.getNodePropStr(dataNode, "str", t, sizeof(t));
				endText = t;
				pos = endText.find("time", 0);
				if (pos!=std::string::npos)
					endText.replace(pos, 4, "%s");
			}

			dataNode = xml.getChildNode(rushNode, "phase");
			phaseDefine pd;
			pd.action = RUSH_RELIVE;
			pd.lasttime = rushDelay;
			setPhaseTime(pd.lasttime*1000);
			phaseVector.push_back(pd);
			while (dataNode)
			{
				char action[32];
				bzero(action, sizeof(action));
				bzero(pd.say, sizeof(pd.say));
				xml.getNodePropStr(dataNode, "action", action, sizeof(action));
				pd.action = parseAction(action);
				if (pd.action)
				{
					xml.getNodePropNum(dataNode, "x", &pd.pos.x, sizeof(pd.pos.x));
					xml.getNodePropNum(dataNode, "y", &pd.pos.y, sizeof(pd.pos.y));
					xml.getNodePropNum(dataNode, "region", &pd.region, sizeof(pd.region));
					xml.getNodePropStr(dataNode, "say", pd.say, sizeof(pd.say));
					xml.getNodePropNum(dataNode, "lasttime", &pd.lasttime, sizeof(pd.lasttime));
					xml.getNodePropNum(dataNode, "id", &pd.x, sizeof(pd.x));
					xml.getNodePropNum(dataNode, "num", &pd.y, sizeof(pd.y));
					xml.getNodePropNum(dataNode, "script", &pd.z, sizeof(pd.z));

					phaseVector.push_back(pd);
				}

				dataNode = xml.getNextNode(dataNode, "phase");
			}
			pd.action = RUSH_END;
			bzero(pd.say, sizeof(pd.say));
			phaseVector.push_back(pd);

			//Zebra::logger->info("读取攻城数据成功:name=%s mapname=%s boss=%d servant=%d phase=%d", rushName, mapName, bossVector.size(), servantVector.size(), phaseVector.size());
			return true;
			break;
		}
		rushNode = xml.getNextNode(rushNode, "rush");
	}
	Zebra::logger->info("读取攻城数据失败:rushID=%d countryID=%d", rushID, countryID);
	return false;
}

/**
 * \brief 处理攻城流程的主函数
 * 检查攻城结束时间、阶段结束时间、阶段结束标志
 * 条件成立则进入下一阶段
 *
 */
void Rush::process()
{
	if (end) return;

	if (checkEndTime())
		if (boss && boss->AIC->isActive())
			terminate();
		else
			onTimeOver();
	else
	{
		DWORD t = endTime.sec()-SceneTimeTick::currentTime.sec();
	 	if (t>=900 && t<=3600 && t%900<2)
	//	if (t>=600 && t<=1800 && t%600<2)
		{
			char tex[32];
			snprintf(tex, sizeof(tex)-1, "%u分钟", t/60);
			Channel::sendCountryInfo(countryID, Cmd::CHAT_TYPE_GM, endText.c_str(), tex);
			Zebra::logger->debug(endText.c_str(), tex);
		}
	}

	if (boss)
	{
		if (boss->hp<lastBossHp)
			if (checkSummonTime())
				summonServant();
		lastBossHp = boss->hp;

		if (zSceneEntry::SceneEntry_Death==boss->getState())
		{
			if (curPhase != phaseVector.size()-1)
			{
				curPhase = phaseVector.size()-1;
				setPhaseTime(clearDelay*1000);
				//if (boss->AIC) boss->AIC->setNormalAI();
				Zebra::logger->debug("boss死亡，阶段%d", curPhase);
			}
		}
	}

	if (checkPhaseTime())
		onPhaseTimeOver();

	switch (phaseVector[curPhase].action)
	{
		case RUSH_RELIVE:
			{
				if (rushDelay>=900
						&& endTime.sec()-SceneTimeTick::currentTime.sec()>3600
						&& rushDelay>=nextPhaseTime.sec()-SceneTimeTick::currentTime.sec())
				{	
					char tex[32];
					snprintf(tex, sizeof(tex)-1, "%u分钟", rushDelay/60);
					Channel::sendCountryInfo(countryID, Cmd::CHAT_TYPE_GM, startText.c_str(), tex);
					Zebra::logger->debug(startText.c_str(), tex);
					rushDelay -= 900;
				}
			}
			break;
		case RUSH_MOVETO:
			{
				if (!boss) return;
				if (boss->scene->zPosShortRange(boss->getPos(), phaseVector[curPhase].pos, phaseVector[curPhase].region))
					enterNextPhase();
			}
			break;
		default:
			break;
	}
}

/**
 * \brief 总攻城时间结束时的处理
 * 只要不是已经处于结束阶段，就直接跳转到结束阶段
 * 以清除攻城npc的延迟作为结束阶段的时间
 *
 */
void Rush::onTimeOver()
{
	if (curPhase != phaseVector.size()-1)
	{
		curPhase = phaseVector.size()-1;
		setPhaseTime(clearDelay*1000);
		//if (boss) boss->AIC->setNormalAI();
	}
}

/**
 * \brief 结束攻城
 * 
 */
void Rush::terminate()
{
	if (curPhase != phaseVector.size()-1)
	{
		curPhase = phaseVector.size()-1;
		//if (boss) boss->AIC->setNormalAI();
	}
	setPhaseTime(0);
}

/**
 * \brief 返回是否已经结束
 *
 * \return 是否结束
 */
bool Rush::isEnd()
{
	return end;
}

/**
 * \brief 设置总结束时间
 *
 * \param delay 延迟
 */
void Rush::setEndTime(const int delay)
{
	endTime = SceneTimeTick::currentTime;
	endTime.addDelay(delay);
}

/**
 * \brief 设置阶段结束时间
 *
 * \param delay 延迟
 */
void Rush::setPhaseTime(const int delay)
{
	nextPhaseTime = SceneTimeTick::currentTime;
	nextPhaseTime.addDelay(delay);
}

/**
 * \brief 检查攻城时间是否已到
 *
 * \return 时间是否已到 
 */
bool Rush::checkEndTime()
{
	return SceneTimeTick::currentTime >= endTime;
}

/**
 * \brief 检查阶段结束时间
 *
 * \return 时间是否已到
 */
bool Rush::checkPhaseTime()
{
	return SceneTimeTick::currentTime >= nextPhaseTime;
}

/**
 * \brief 阶段结束事件的处理
 * 不同阶段结束时处理不同
 * 复活、清除的动作是在阶段结束时发生
 * 如果动作是移动，在阶段结束时未到达目的地，则直接跳转过去
 *
 */
void Rush::onPhaseTimeOver()
{
	switch (phaseVector[curPhase].action)
	{
		case RUSH_RELIVE:
			{
				//Zebra::logger->debug("%s : 召唤boss", rushName);
				if (!summonBoss())
				{
					Zebra::logger->error("怪物攻城: %s 召唤boss %s 失败，结束攻城", rushName, bossName);
					end = true;
				}
			}
			break;
		case RUSH_MOVETO:
			{
				//if (!boss) return;
				if (!boss->scene->zPosShortRange(boss->getPos(), phaseVector[curPhase].pos, phaseVector[curPhase].region))							
					boss->warp(phaseVector[curPhase].pos);
			}
			break;
		case RUSH_CLEAR:
			{
				clearServants();
			}
			break;
		case RUSH_END:
			{
				deleteBoss();
				clearServants();
				end=true;
			}
			break;
		default:
			break;
	}
	enterNextPhase();
}

/**
 * \brief 处理进入阶段的事件
 * 恢复、召唤、开关、说话等动作在阶段开始时发生
 * 这些动作是瞬间的，如果阶段持续时间不为0，则在余下的时间内boss做普通攻击
 *
 */
void Rush::enterNextPhase()
{
	if (!end && curPhase<phaseVector.size()-1)
	{
		curPhase++;
		Zebra::logger->debug("%s : 阶段%d", rushName, curPhase);
		setPhaseTime(phaseVector[curPhase].lasttime*1000);
		if (strcmp(phaseVector[curPhase].say,""))
			Channel::sendNine(boss, phaseVector[curPhase].say);

		//按照脚本来动作
		if (boss->AIC->isActive()
				&& phaseVector[curPhase].action!=RUSH_SUMMON_PET
				&& phaseVector[curPhase].action!=RUSH_SUMMON_RUSH
				&& phaseVector[curPhase].action!=RUSH_END)
			return;

		switch (phaseVector[curPhase].action)
		{
			case RUSH_RELIVE:
				break;
			case RUSH_MOVETO:
				{
					t_NpcAIDefine d(NPC_AI_PATROL, phaseVector[curPhase].pos, phaseVector[curPhase].region, phaseVector[curPhase].region, phaseVector[curPhase].lasttime);
					boss->AIC->setAI(d);
				}
				break;
			case RUSH_ATTACK:
				{
					t_NpcAIDefine d(NPC_AI_ATTACK, phaseVector[curPhase].pos, phaseVector[curPhase].region, phaseVector[curPhase].region, phaseVector[curPhase].lasttime);
					boss->AIC->setAI(d);
				}
				break;
			case RUSH_RECOVER:
				{
					boss->hp = boss->getMaxHP();
					//boss->AIC->setNormalAI();
				}
				break;
			case RUSH_SUMMON:
				{
					summonServant();
					//boss->AIC->setNormalAI();
				}
				break;
			case RUSH_BROADCAST:
				{
					Channel::sendCountryInfo(countryID, Cmd::CHAT_TYPE_GM, phaseVector[curPhase].say);
				}
				break;
			case RUSH_SUMMON_RUSH:
				{
					zNpcB *base = npcbm.get(phaseVector[curPhase].x);

					if (base)
					{
						t_NpcDefine define;
						zPos pos = phaseVector[curPhase].pos;
						define.id = base->id;
						strncpy(define.name, base->name, MAX_NAMESIZE-1);
						define.pos = phaseVector[curPhase].pos;
						define.num = 1;
						define.interval = 0x0fffffff;
						define.initstate = zSceneEntry::SceneEntry_Normal;
						define.width = phaseVector[curPhase].region;
						define.height = phaseVector[curPhase].region;
						define.pos -= zPos(phaseVector[curPhase].region/2,phaseVector[curPhase].region/2);
						define.scriptID = phaseVector[curPhase].z;

						Scene * scene = SceneManager::getInstance().getSceneByName(mapName);
						if (!scene)
						{
							Zebra::logger->debug("召唤攻城servant时，未找到该地图 name=%s", mapName);
							return;
						}
						scene->initRegion(define.region, define.pos, define.width, define.height);

						for (unsigned int i=0; i<phaseVector[curPhase].y; i++)
						{
							SceneNpc * servant = scene->summonOneNpc<SceneNpc>(define, zPos(0,0), base, 0,0);
							//summonCount++;
							if (servant)
							{
								servant->aif |= AIF_ACTIVE_MODE;
								servant->isRushNpc = true;
								servants.push_back(servant);
#ifdef _XWL_DEBUG
								Zebra::logger->debug("[怪物攻城]%s (%u,%u)", servant->name, servant->getPos().x, servant->getPos().y);
#endif
							}
						}
					}
					//boss->AIC->setNormalAI();
				}
				break;
			case RUSH_SUMMON_PET:
				{
					summonPet();
				}
				break;
			case RUSH_SUMMON_ON:
				{
					canSummon = true;
					//boss->AIC->setNormalAI();
				}
				break;
			case RUSH_SUMMON_OFF:
				{
					canSummon = false;
					//boss->AIC->setNormalAI();
				}
				break;
			case RUSH_CLEAR:
				{
					//boss->AIC->setNormalAI();
				}
				break;
			case RUSH_END:
				{
					setPhaseTime(clearDelay*1000);
					//boss->AIC->setNormalAI();
				}
				break;
			default:
				break;
		}
	}
#if 0
	else
		Zebra::logger->debug("%s : 攻城结束", rushName);
#endif
}

/**
 * \brief 召唤一个boss
 * 脚本中可定义多个boss，召唤时选择一个，以便从不同的位置出生，增加随机性
 *
 * \return 召唤是否成功
 */
bool Rush::summonBoss()
{
	zNpcB *base = npcbm.get(bossID);
	if (NULL == base)
	{
		//Zebra::logger->debug("召唤攻城boss时，未找到该怪物 id=%d", bossID);
		return false;
	}

	int index = zMisc::randBetween(0,bossVector.size()-1);

	t_NpcDefine define;
	define.id = base->id;
	strcpy(define.name, base->name);
	define.pos = bossVector[index].pos;
	define.num = bossVector[index].num;
	define.interval = 0x0fffffff;//这个时间一定要长 如果短于rush的cleartime，就会提前delete boss，clear时间到的时候就会coredump
	define.initstate = zSceneEntry::SceneEntry_Normal;
	define.width = bossVector[index].region*2;
	define.height = bossVector[index].region*2;
	define.pos -= zPos(bossVector[index].region, bossVector[index].region);
	define.scriptID = bossVector[index].script;

	Scene * scene = SceneManager::getInstance().getSceneByName(mapName);
	if (!scene)
	{
		Zebra::logger->debug("召唤攻城boss时，未找到该地图 name=%s", mapName);
		return false;
	}
	scene->initRegion(define.region, define.pos, define.width, define.height);

	boss = scene->summonOneNpc<SceneNpc>(define, bossVector[index].pos, base);
	//summonCount++;
	if (!boss)
	{
		Zebra::logger->debug("召唤攻城boss失败 id=%d", base->id);
		return false;
	}
	//SceneNpcManager::getMe().addSpecialNpc(boss);

	boss->isRushNpc = true;
	summonTime = SceneTimeTick::currentTime;

	return true;
}

/**
 * \brief 检查是否到了可召唤仆人的时间
 *
 * \return 是否到了可召唤的时间
 */
bool Rush::checkSummonTime()
{
	return SceneTimeTick::currentTime >= summonTime;
}

/**
 * \brief 召唤仆人
 * 脚本中可以定义多个仆人，每个都有召唤几率和间隔
 * 每次召唤时判断所有的仆人，可能一次召唤出所有仆人，也可能一个也没召唤
 * 召唤时间累加，如一次召唤了1组间隔30秒的仆人和一组间隔20秒的仆人，则在50秒内boss不能召唤
 *
 * \return 召唤是否成功
 */
bool Rush::summonServant()
{
	FunctionTime func_alltime(0,__PRETTY_FUNCTION__,"npc召唤仆人需要的时间" , 32);
	if (!boss) return false;
	if (!canSummon) return false;

	int die = zMisc::randBetween(0,100);

	for (unsigned int index=0; index<servantVector.size(); index++)
	{
		if (servantVector[index].rate<die)
			continue;
		
		zNpcB *base = npcbm.get(servantVector[index].id);
		if (NULL == base)
		{
			Zebra::logger->debug("召唤 %s 的仆人时，未找到该怪物 id=%d", boss->name, servantVector[index].id);
			return false;
		}

		t_NpcDefine define;
		define.id = base->id;
		strcpy(define.name, base->name);
		zPos pos(boss->getPos());
		pos -= zPos(summon_servant_region*2, summon_servant_region*2);
		define.pos = pos;
		define.num = servantVector[index].num;
		define.interval = 0x0fffffff;
		define.initstate = zSceneEntry::SceneEntry_Normal;
		define.width = summon_servant_region*4;
		define.height = summon_servant_region*4;
		//define.pos -= zPos(summon_servant_region, summon_servant_region);

		Scene * scene = SceneManager::getInstance().getSceneByName(mapName);
		if (!scene)
		{
			Zebra::logger->debug("召唤 %s 的仆人时，未找到该地图 name=%s", boss->name, mapName);
			return false;
		}
		scene->initRegion(define.region, define.pos, define.width, define.height);

		SceneNpc * servant;
		int count = 0;
		for (int i=0; i<servantVector[index].num; i++)
		{
			servant = scene->summonOneNpc<SceneNpc>(define, pos, base);
			if (servant)
			{
				servant->aif |= AIF_ACTIVE_MODE;
				servant->isRushNpc = true;
				servants.push_back(servant);
				count++;
			}
		}

		summonTime = SceneTimeTick::currentTime;
		summonTime.addDelay(servantVector[index].interval*1000);
		Zebra::logger->debug("%s summonServant:summon %u 次, servant %u 个", rushName, count, servants.size());
	}
	
	return true;
}
bool Rush::summonPet()
{
	FunctionTime func_alltime(0,__PRETTY_FUNCTION__,"npc召唤宠物需要的时间" , 32);
	if (!boss) return false;
	if (!canSummon) return false;

	int die = zMisc::randBetween(0,100);

	for (unsigned int index=0; index<servantVector.size(); index++)
	{
		if (servantVector[index].rate<die)
			continue;
		
		zNpcB *base = npcbm.get(servantVector[index].id);
		if (NULL == base)
		{
			Zebra::logger->debug("召唤 %s 的宠物时，未找到该怪物 id=%d", boss->name, servantVector[index].id);
			return false;
		}

		t_NpcDefine define;
		define.id = base->id;
		strcpy(define.name, base->name);
		zPos pos(boss->getPos());
		pos -= zPos(summon_servant_region*2, summon_servant_region*2);
		define.pos = pos;
		define.num = servantVector[index].num;
		define.interval = 0x0fffffff;
		define.initstate = zSceneEntry::SceneEntry_Normal;
		define.width = summon_servant_region*4;
		define.height = summon_servant_region*4;
		//define.pos -= zPos(summon_servant_region, summon_servant_region);

		Scene * scene = SceneManager::getInstance().getSceneByName(mapName);
		if (!scene)
		{
			Zebra::logger->debug("召唤 %s 的仆人时，未找到该地图 name=%s", boss->name, mapName);
			return false;
		}
		scene->initRegion(define.region, define.pos, define.width, define.height);

		ScenePet * servant;
		int count = 0;
		for (int i=0; i<servantVector[index].num; i++)
		{
			servant = scene->summonOneNpc<ScenePet>(define, pos, base);
			if (servant)
			{
				servant->isRushNpc = true;
				servants.push_back(servant);

				servant->setMaster(boss);
				boss->totems.push_back(servant);
				count++;
			}
		}

		summonTime = SceneTimeTick::currentTime;
		summonTime.addDelay(servantVector[index].interval*1000);
		Zebra::logger->debug("%s summonPet:summon %u 次, servant %u 个", rushName, count, servants.size());
	}
	
	return true;
}

/**
 * \brief 删除boss
 * 设置clearme的标志位，delete动作在SceneNpc::AI进行
 *
 */
void Rush::deleteBoss()
{
	if (boss)
	{
		/*
		Cmd::stRemoveMapNpcMapScreenUserCmd removeNpc;
		removeNpc.dwMapNpcDataPosition = boss->tempid;
		boss->scene->sendCmdToNine(boss->getPosI(), &removeNpc, sizeof(removeNpc), false);

		SceneNpcManager::getMe().removeSceneNpc(boss);
		boss->scene->removeNpc(boss);

		SAFE_DELETE(boss);
		*/
		boss->setClearState();
		SceneNpcManager::getMe().addSpecialNpc(boss, true);
		boss = 0;
		Zebra::logger->debug("%s : 删除boss", rushName);
	}
	else
		Zebra::logger->debug("%s : boss已经删除", rushName);
}

/**
 * \brief 删除所有仆人 
 * 设置clearme的标志位，delete动作在SceneNpc::AI进行
 *
 */
void Rush::clearServants()
{
	for (std::list<SceneNpc *>::iterator it=servants.begin(); it!=servants.end(); it++)
	{
		/*
		Cmd::stRemoveMapNpcMapScreenUserCmd removeNpc;
		removeNpc.dwMapNpcDataPosition = (*it)->tempid;
		(*it)->scene->sendCmdToNine((*it)->getPosI(), &removeNpc, sizeof(removeNpc), false);

		SceneNpcManager::getMe().removeSceneNpc((*it));
		(*it)->scene->removeNpc((*it));

		SceneNpc *npc = *it;
		SAFE_DELETE(npc);
		*/
		(*it)->setClearState();
		SceneNpcManager::getMe().addSpecialNpc((*it), true);
	}
	Zebra::logger->debug("%s : 删除 %u 个servants", rushName, servants.size());
	servants.clear();
}
