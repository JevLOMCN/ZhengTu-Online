/**
 * \file
 * \version  $Id: SessionClient.cpp$
 * \author 
 * \date 
 * \brief 定义Session服务器连接客户端
 *
 */

#include <unistd.h>
#include <iostream>
#include "zTCPClient.h"
#include "SessionCommand.h"
#include "SessionClient.h"
#include "ScenesServer.h"
#include "Zebra.h"
#include "SceneManager.h"
#include "SceneTaskManager.h"
#include "RecordClient.h"
#include "Chat.h"
#include "School.h"
#include "ObjectManager.h"
#include "QuestEvent.h"
#include "QuestTable.h"
#include "NpcDare.h"
#include "NpcTrade.h"
#include "ScenesServer.h"
#include "CartoonPet.h"
#include "SceneUserManager.h"
#include "script.h"
#include "GlobalBox.h"
#include "CountryTech.h"
#include "CountryAlly.h"
#include "RebuildObject.h"
#include "GameConfigMgr.h"
#include "MessageSystem.h"
#include "Scene.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "FuBenMgr.h" //副本
SessionClient *sessionClient = NULL;

/**
 * \brief 统计存活的指定NPC的个数
 */
struct ClearAllNotUnion : public zSceneEntryCallBack
{
	DWORD dwUnionID;
	ClearAllNotUnion(DWORD unionid) : dwUnionID(unionid){};
	std::vector<SceneUser*> _remove_list;

	/**
	 * \brief 回调函数
	 * \param entry 地图物件,这里是玩家
	 * \return 回调是否送成功
	 */
	bool exec(zSceneEntry *entry)
	{
		if (((SceneUser *)entry)->charbase.unionid != dwUnionID)
		{
			_remove_list.push_back((SceneUser*)entry);
		}

		return true;
	}

	void reliveAll()
	{
		std::vector<SceneUser*>::iterator tIterator;

		for (tIterator = _remove_list.begin(); tIterator!=_remove_list.end(); tIterator++)
		{
			(*tIterator)->relive();
		}
	}
};
/**
 * \brief  物品比较回掉函数，用来寻找某特定类型的物品
 */
class SessionItemObjectCompare:public UserObjectCompare 
{
	public:
		DWORD  dwObjectID;

		bool isIt(zObject *object)
		{
			if (object->base->id == dwObjectID) return true;
			return false;
		}
};

/**
 * \brief  登录会话服务器
 * \return  true 登录消息发送成功， false 无法发送消息
 */
bool SessionClient::connectToSessionServer()
{
	if (!connect())
	{
		Zebra::logger->error("连接Session服务器失败");
		return false;
	}

	using namespace Cmd::Session;
	t_LoginSession tCmd;
	tCmd.wdServerID = ScenesService::getInstance().getServerID();
	tCmd.wdServerType = ScenesService::getInstance().getServerType();

	return sendCmd(&tCmd, sizeof(tCmd));
}

/**
 * \brief 重载zThread中的纯虚函数，是线程的主回调函数，用于处理接收到的指令
 *
 */
void SessionClient::run()
{
	zTCPBufferClient::run();

	//与Session之间的连接断开，需要关闭服务器
	ScenesService::getInstance().Terminate();
	while(!ScenesService::getInstance().isSequeueTerminate())
	{
		zThread::msleep(10);
	}

}

/**
 * \brief  重新请求组队中的社会关系列表（计算友好度用）一般用在角色社会关系发生变化的情况下
 * \param  pUser 社会关系发生变化的角色
 */
void SessionClient::requestFriendDegree(SceneUser *pUser)
{
#ifdef _DEBUGLOG
	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "为组队重新请求社会关系列表");
#endif
	if (pUser->team.getLeader() == 0) return;
	SceneUser *leader = SceneUserManager::getMe().getUserByTempID(pUser->team.getLeader());
	if (leader)
	{
#ifdef _DEBUGLOG
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "为组队重新请求社会关系列表成功");
#endif
		leader->team.requestFriendDegree();
	}

}

/**
 * \brief  处理会话发送过来的消息处理内容包括
 1 社会关系战
 2 场景的注册注销
 3 角色的注册注销
 4 动态加载，卸载地图
 5 用户临时存档
 6 帮会，家族，门派的相关处理
 7 私聊消息处理
 * \param  
 * \return  
 */
bool SessionClient::msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	return MessageQueue::msgParse(ptNullCmd , nCmdLen);
}

bool SessionClient::cmdMsgParse_Gem(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	switch (ptNullCmd->para)
	{
		case Cmd::Session::SUMMON_GEMNPC_SCENE_PARA:
			{
				Cmd::Session::t_SummonGemNPC_SceneSession* rev = 
					(Cmd::Session::t_SummonGemNPC_SceneSession*)ptNullCmd;
				Scene * scene = SceneManager::getInstance().getSceneByID(rev->dwMapID);
				if (!scene)
				{
					Zebra::logger->debug("SUMMON_GEMNPC: 召唤npc时找不到地图 mapid=%u", rev->dwMapID);
					return false;
				}
				zNpcB *base = npcbm.get(rev->dwBossID);
				if (NULL == base)
				{
					Zebra::logger->debug("SUMMON_GEMNPC: 召唤npc时找不到NPC id=%d", rev->dwBossID);
					return false;
				}

				t_NpcDefine define;
				define.id = base->id;
				strcpy(define.name, base->name);
				define.pos = zPos(rev->x, rev->y);
				define.num = 1;
				define.interval = 5;
				define.initstate = zSceneEntry::SceneEntry_Normal;
				define.width = 2;
				define.height = 2;
				define.scriptID = 0;
				scene->initRegion(define.region, define.pos, define.width, define.height);

				SceneNpc * boss = scene->summonOneNpc<SceneNpc>(define, zPos(rev->x, rev->y), base);
				if (!boss)
				{
					Zebra::logger->debug("SUMMON_GEMNPC: 召唤NPC失败 id=%d", base->id);
					return false;
				}

				scene->bossMap[rev->dwBossID] = boss;
				return true;
			}
			break;
		case Cmd::Session::SET_GEMSTATE_SCENE_PARA:
			{
				Cmd::Session::t_SetGemState_SceneSession* rev = 
					(Cmd::Session::t_SetGemState_SceneSession*)ptNullCmd;

				SceneUser* pUser = NULL;
				pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);

				if (pUser)
				{
					if (rev->dwState==1)
					{
						pUser->showCurrentEffect(Cmd::USTATE_TOGETHER_WITH_DRAGON, true);
						pUser->skillStatusM.clearActiveSkillStatus();
						pUser->mask.on_gem();
						pUser->leaveTeam();
					}
					else if (rev->dwState==2)
					{
						pUser->showCurrentEffect(Cmd::USTATE_TOGETHER_WITH_TIGER, true);
						pUser->skillStatusM.clearActiveSkillStatus();
						pUser->mask.on_gem();
						pUser->leaveTeam();
					}
					else if (rev->dwState==0)
					{
						pUser->showCurrentEffect(Cmd::USTATE_TOGETHER_WITH_DRAGON, false);
						pUser->showCurrentEffect(Cmd::USTATE_TOGETHER_WITH_TIGER, false);
					}

					pUser->setupCharBase();
					Cmd::stMainUserDataUserCmd  userinfo;
					pUser->full_t_MainUserData(userinfo.data);
					pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
				}
			}
			break;
		case Cmd::Session::CLEAR_GEMNPC_SCENE_PARA:
			{
				Cmd::Session::t_ClearGemNPC_SceneSession *rev = (Cmd::Session::t_ClearGemNPC_SceneSession *)ptNullCmd;

				Scene * scene = SceneManager::getInstance().getSceneByID(rev->dwMapID);
				if (!scene)
				{
					Zebra::logger->debug("CLEAR_GEMNPC: 清除npc时找不到地图 mapid=%u", rev->dwMapID);
					return false;
				}

				if (scene->bossMap[rev->dwBossID])
					scene->bossMap[rev->dwBossID]->setClearState();

				return true;
			}
			break;
		case Cmd::Session::BLAST_GEMNPC_SCENE_PARA:
			{
				Cmd::Session::t_BlastGemNPC_SceneSession * rev = (Cmd::Session::t_BlastGemNPC_SceneSession *)ptNullCmd;

				SceneUser * pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);
				if (!pUser)
				{
					Zebra::logger->debug("BLAST_GEMNPC: npc爆物品时找不到玩家 userID=%u", rev->dwUserID);
					return false;
				}

				pUser->showCurrentEffect(Cmd::USTATE_TOGETHER_WITH_DRAGON, false);
				pUser->showCurrentEffect(Cmd::USTATE_TOGETHER_WITH_TIGER, false);
				pUser->setupCharBase();

				Cmd::stMainUserDataUserCmd  userinfo;
				pUser->full_t_MainUserData(userinfo.data);
				pUser->sendCmdToMe(&userinfo,sizeof(userinfo));

				zNpcB *base = npcbm.get(rev->dwBossID);
				if (NULL == base)
				{
					Zebra::logger->debug("BLAST_GEMNPC: 召唤npc时找不到NPC id=%d", rev->dwBossID);
					return false;
				}

				t_NpcDefine define;
				define.id = base->id;
				strcpy(define.name, base->name);
				define.pos = pUser->getPos();
				define.num = 1;
				define.interval = 5;
				define.initstate = zSceneEntry::SceneEntry_Normal;
				define.width = 2;
				define.height = 2;
				define.scriptID = 0;
				pUser->scene->initRegion(define.region, define.pos, define.width, define.height);

				SceneNpc * boss = pUser->scene->summonOneNpc<SceneNpc>(define, pUser->getPos(), base);
				if (!boss)
				{
					Zebra::logger->debug("SUMMON_GEMNPC: 召唤NPC失败 id=%d", base->id);
					return false;
				}

				boss->toDie(0);

				return true;
			}
			break;
		default:
			break;
	}


	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

bool SessionClient::cmdMsgParse_Army(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	switch (ptNullCmd->para)
	{
		case Cmd::Session::SEND_USER_ARMY_INFO_PARA:
			{
				Cmd::Session::t_sendUserArmyInfo_SceneSession* rev = 
					(Cmd::Session::t_sendUserArmyInfo_SceneSession*)ptNullCmd;

				SceneUser* pUser = NULL;
				pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);
				if (pUser)
				{
					strncpy(pUser->armyName, rev->title, MAX_NAMESIZE);

					pUser->dwArmyState = rev->byType;
					pUser->sendMeToNine();
					// zjw modify by 2006-3-2
					//pUser->sendNineToMe();
				}

				return true;
			}
			break;
		default:
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

bool SessionClient::cmdMsgParse_Recommend(const Cmd::t_NullCmd* ptNullCmd, const unsigned int nCmdLen)
{
	switch (ptNullCmd->para)
	{
		case Cmd::Session::PICKUP_RECOMMEND_SCENE_PARA:
			{
				Cmd::Session::t_PickupRecommend_SceneSession* rev = 
					(Cmd::Session::t_PickupRecommend_SceneSession*)ptNullCmd;
				
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);
				if (!pUser)
				{
					return true;
				}

				if (rev->byType == 0)
				{
					pUser->packs.addMoney(rev->dwMoney, "推荐人领取奖励", NULL, true);
				}
				else
				{
					pUser->packs.addMoney(rev->dwMoney, "被推荐人领取奖励", NULL, true);
				}

				return true;
			}
		default:
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;

}

bool SessionClient::cmdMsgParse_Dare(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	switch (ptNullCmd->para)
	{
		case Cmd::Session::PARA_SET_EMPEROR_DARE:
			{
				Cmd::Session::t_setEmperorDare_SceneSession* rev =
					(Cmd::Session::t_setEmperorDare_SceneSession*)ptNullCmd;

#ifdef _ZJW_DEBUG				
				Zebra::logger->debug("收到设置皇城争夺战命令: state:%u defcountry:%u", 
						rev->byState, rev->dwDefCountryID);
#endif				
				DWORD map_id = (6 << 16) + 134;
				Scene* pScene = SceneManager::getInstance().getSceneByID(map_id);

				if (pScene)
				{
					pScene->setEmperorDare(rev->byState, rev->dwDefCountryID);
					if (rev->byState == 1)
					{
						SceneUserManager::getMe().setEmperorDare(pScene);
					}
					else
					{
						SceneUserManager::getMe().clearEmperorDare(pScene);
					}
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SET_ANTI_ATT_FLAG:
			{
				Cmd::Session::t_setAntiAttFlag_SceneSession* rev = 
					(Cmd::Session::t_setAntiAttFlag_SceneSession*)ptNullCmd;
#ifdef _ZJW_DEBUG				
				Zebra::logger->debug("收到设置反攻命令: from:%d to:%d", 
						rev->dwToRelationID, rev->dwToRelationID);
#endif				

				SceneUserManager::getMe().setAntiAtt(rev->dwType, 
						rev->dwFromRelationID, rev->dwToRelationID);
				return true;

			}
			break;
		case Cmd::Session::PARA_ENTERWAR:
			{
				Cmd::Session::t_enterWar_SceneSession* rev = 
					(Cmd::Session::t_enterWar_SceneSession*)ptNullCmd;

#ifdef _ZJW_DEBUG				
				Zebra::logger->debug("收到激活对战命令:type:%d fromrelation:%d torelation:%d state:%d isatt:%d isAntiAtt:%d", 							rev->dwWarType, rev->dwFromRelationID, rev->dwToRelationID, 
						rev->dwStatus, rev->isAtt, rev->isAntiAtt);
#endif				

				SceneUser* pUser = NULL;

				pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);

				if (rev->dwStatus == 1)
				{
					if (rev->dwWarType == Cmd::COUNTRY_FORMAL_DARE 
							|| rev->dwWarType == Cmd::COUNTRY_FORMAL_ANTI_DARE)
					{
						//rev->dwWarType = Cmd::COUNTRY_FORMAL_DARE;

						if (rev->dwUserID == 0)
						{
							SceneUserManager::getMe().enterWar(rev);
						}
						else if(pUser)
						{
							pUser->addWarRecord(rev->dwWarType, rev->dwToRelationID, 
									rev->isAtt);

							if (rev->isAntiAtt)
							{
								pUser->setAntiAttState(rev->dwWarType, rev->dwToRelationID);
							}

							if (pUser->scene->getRealMapID() == 139 
									&& rev->dwToRelationID==pUser->scene->getCountryID()
									&& pUser->scene->getCountryDareBackToMapID())
							{
								pUser->deathBackToMapID =  (rev->dwToRelationID << 16) + 
									pUser->scene->getCountryDareBackToMapID();
							}
						}
					}
					else
					{
						if (pUser == NULL)
						{
							Zebra::logger->error("无效的用户ID(%d)， 取消激活对战命令", 
									rev->dwUserID);
							return true;
						}
						if (rev->dwWarType == Cmd::SEPT_NPC_DARE && pUser->charbase.level >=60)
						{
							DWORD mapid = pUser->scene->getRealMapID();
							if (mapid == 101 ||
								mapid == 102 ||
								mapid == 104)
							{
								/// 超过59的玩家不允许在凤凰城，凤尾村和清源村进行NPC争夺战。
								return true;
							}
						}

						pUser->addWarRecord(rev->dwWarType, rev->dwToRelationID, rev->isAtt);
						if (rev->isAntiAtt)
						{
							pUser->setAntiAttState(rev->dwWarType, rev->dwToRelationID);
						}

						if (rev->dwWarType == Cmd::UNION_CITY_DARE)
						{
							pUser->scene->setUnionDare(true);
						}
					}
				}
				else
				{	
					if (rev->dwWarType == Cmd::COUNTRY_FORMAL_DARE
						|| rev->dwWarType == Cmd::COUNTRY_FORMAL_ANTI_DARE)
					{
						//rev->dwWarType = Cmd::COUNTRY_FORMAL_DARE;

						if (pUser == NULL)
						{
							SceneUserManager::getMe().enterWar(rev);
						}
						else
						{
							pUser->removeWarRecord(rev->dwWarType, rev->dwToRelationID);
							pUser->setDeathBackToMapID(pUser->scene);
						}
					}
					else
					{
						if (pUser == NULL)
						{
							Zebra::logger->error("无效的用户ID(%d)， 取消激活对战命令", 
									rev->dwUserID);
							return true;
						}

						if (rev->dwWarType == Cmd::UNION_CITY_DARE)
						{
							pUser->scene->setUnionDare(false);
						}

						if (rev->dwWarType == Cmd::SEPT_NPC_DARE)
						{
							pUser->notifySessionNpcDareResult();
						}

						pUser->removeWarRecord(rev->dwWarType);
					}
				}

#ifdef _ZJW_DEBUG
				if (pUser)
				{
					Zebra::logger->debug("%s 已有 %d 条交战记录", pUser->name, pUser->warSize());
				}
#endif				
				if (pUser != NULL)
				{
					pUser->sendNineToMe(); // 及时更新对战状态
					pUser->setStateToNine(Cmd::USTATE_WAR);
				}
				
				return true;
			}
			break;
		case Cmd::Session::PARA_DARE_GOLD:
			{
				Cmd::Session::t_dareGold_SceneSession* rev = (Cmd::Session::t_dareGold_SceneSession*)ptNullCmd;
				Cmd::Session::t_activeDare_SceneSession send;
#ifdef _ZJW_DEBUG				
				Zebra::logger->debug("%ld 银子操作 : %d", rev->dwWarID, rev->dwNum);
#endif				

				SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->dwUserID);

				if (pUser == NULL)
				{
					return true;
				}

				send.dwWarID = rev->dwWarID;
				if (rev->dwNum>=0)
				{
					pUser->packs.addMoney(rev->dwNum, "挑战返还",NULL, false);
					send.dwStatus = Cmd::Session::SCENE_ACTIVEDARE_SUCCESS;

					if (rev->dwType == Cmd::Session::RETURN_DARE_GOLD)
					{
						if (rev->dwWarType == Cmd::UNION_DARE 
								|| rev->dwWarType == Cmd::UNION_CITY_DARE)
						{
							Zebra::logger->trace("[夺城战]: %s(%d) 返还挑战金 %d 文", 
									pUser->name, pUser->charbase.unionid, rev->dwNum);
						}
						else if (rev->dwWarType == Cmd::SEPT_DARE)
						{
							Zebra::logger->trace("[家族日志]: %s(%d) 返还挑战金 %d 文", 
									pUser->name, pUser->charbase.septid, rev->dwNum);
						}
					}
					else if (rev->dwType == Cmd::Session::WINNER_GOLD)
					{
						if (rev->dwWarType == Cmd::UNION_DARE 
								|| rev->dwWarType == Cmd::UNION_CITY_DARE)
						{
							Zebra::logger->trace("[夺城战]: %s(%d) 对战奖励 %d 文", 
									pUser->name, pUser->charbase.unionid, rev->dwNum);
						}
						else if (rev->dwWarType == Cmd::SEPT_DARE)
						{
							Zebra::logger->trace("[家族日志]: %s(%d) 对战奖励 %d 文", 
									pUser->name, pUser->charbase.septid, rev->dwNum);
						}
					}
					//soke 皇帝领取每日薪资
					else if (rev->dwType == Cmd::Session::EMPEROR_GOLD)
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "领取每日奖励金 5 锭");

						Zebra::logger->trace("[国家]: %s 每日奖励 %d 文", 
								pUser->name, rev->dwNum);
					}
					//soke 国王领取每日薪资
					else if (rev->dwType == Cmd::Session::KING_GOLD)
					{
                        //soke 增加领取金子直接到包裹
					    pUser->packs.addGold(50000,"领取每日薪资5锭金子");
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "成功领取今日5锭金子薪资");

						Zebra::logger->trace("[国家]: %s 领取每日薪资 %d 文",pUser->name, rev->dwNum);
					}
					//soke 元帅领取每日薪资
					else if (rev->dwType == Cmd::Session::YS_GOLD)
					{
                        //soke 增加领取金子直接到包裹
					    pUser->packs.addGold(30000,"领取每日薪资3锭金子");
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "成功领取今日3锭金子薪资");

						Zebra::logger->trace("[国家]: %s 领取每日薪资 %d 文",pUser->name, rev->dwNum);
					}
					//soke 宰相领取每日薪资
					else if (rev->dwType == Cmd::Session::ZX_GOLD)
					{
                        //soke 增加领取金子直接到包裹
					    pUser->packs.addGold(30000,"领取每日薪资3锭金子");
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "成功领取今日3锭金子薪资");

						Zebra::logger->trace("[国家]: %s 领取每日薪资 %d 文",pUser->name, rev->dwNum);
					}
					//soke 捕头（左）领取每日薪资
					else if (rev->dwType == Cmd::Session::CR_GOLD)
					{
                        //soke 增加领取金子直接到包裹
					    pUser->packs.addGold(20000,"领取每日薪资2锭金子");
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "成功领取今日2锭金子薪资");

						Zebra::logger->trace("[国家]: %s 领取每日薪资 %d 文",pUser->name, rev->dwNum);
					}
					//soke 御史（左）领取每日薪资
					else if (rev->dwType == Cmd::Session::DT_GOLD)
					{
                        //soke 增加领取金子直接到包裹
					    pUser->packs.addGold(20000,"领取每日薪资2锭金子");
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "成功领取今日2锭金子薪资");

						Zebra::logger->trace("[国家]: %s 领取每日薪资 %d 文",pUser->name, rev->dwNum);
					}
					//soke 捕头（右）领取每日薪资
					else if (rev->dwType == Cmd::Session::CX_GOLD)
					{
                        //soke 增加领取金子直接到包裹
					    pUser->packs.addGold(20000,"领取每日薪资2锭金子");
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "成功领取今日2锭金子薪资");

						Zebra::logger->trace("[国家]: %s 领取每日薪资 %d 文",pUser->name, rev->dwNum);
					}
					//soke 御史（右）领取每日薪资
					else if (rev->dwType == Cmd::Session::DX_GOLD)
					{
                        //soke 增加领取金子直接到包裹
					    pUser->packs.addGold(20000,"领取每日薪资2锭金子");
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "成功领取今日2锭金子薪资");

						Zebra::logger->trace("[国家]: %s 领取每日薪资 %d 文",pUser->name, rev->dwNum);
					}

				}
				else if(rev->dwNum<0)
				{
					if (pUser->packs.checkMoney(abs(rev->dwNum)) && pUser->packs.removeMoney(abs(rev->dwNum),"扣除对战金")) 
					{
						//Zebra::logger->trace("对战银子跟踪：%s([%s]帮 [%s]家族) 扣除 %d 文", 
						//		pUser->name, pUser->unionName, pUser->septName, rev->dwNum);
						if (rev->dwType == Cmd::Session::DARE_GOLD)
						{
							if (rev->dwWarType == Cmd::UNION_DARE 
									|| rev->dwWarType == Cmd::UNION_CITY_DARE)
							{
								Zebra::logger->trace("[夺城战]: %s(%d) 扣除对战金 %d 文", 
										pUser->name, pUser->charbase.unionid, rev->dwNum);
							}
							else if (rev->dwWarType == Cmd::SEPT_DARE)
							{
								Zebra::logger->trace("[家族日志]: %s(%d) 扣除对战金 %d 文", 
										pUser->name, pUser->charbase.septid, rev->dwNum);
							}
						}

						send.dwStatus = Cmd::Session::SCENE_ACTIVEDARE_SUCCESS;
					}
					else 
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "银子不足,不能进入对战");

						send.dwStatus = Cmd::Session::SCENE_ACTIVEDARE_FAIL;
					}
				}

				if (rev->dwWarID !=0)
				{
					sendCmd(&send, sizeof(Cmd::Session::t_activeDare_SceneSession));
				}

				return true;
			}
			break;
			case Cmd::Session::PARA_GOTO_LEADER:
			{
				Cmd::Session::t_GoTo_Leader_SceneSession *rev=(Cmd::Session::t_GoTo_Leader_SceneSession*)ptNullCmd;
				SceneUser *pUser=SceneUserManager::getMe().getUserByTempID(rev->leaderTempID);
				if(pUser && !pUser->guard)
				{
					char buffer[256];
					bzero(buffer,sizeof(buffer));
					sprintf(buffer,"name=%s pos=%d,%d",rev->mapName,rev->x,rev->y);
					Gm::gomap(pUser,buffer);
				}
				return true;
			}
			break;
			case Cmd::Session::PARA_GOTO_LEADER_CHECK:
			{
				Cmd::Session::t_GoTo_Leader_Check_SceneSession * rev =(Cmd::Session::t_GoTo_Leader_Check_SceneSession *)ptNullCmd;
				SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(rev->userTempID);
				if(pUser && !pUser->guard)
				{
					Cmd::stDareCallDuty ret;
					ret.leaderTempID = rev->leaderTempID;
					ret.byCallerType = rev->type;
					strncpy(ret.mapName,rev->mapName,sizeof(ret.mapName));
					ret.x = rev->x;
					ret.y = rev->y;
					pUser->sendCmdToMe(&ret,sizeof(ret));
				}
				return true;
			}
			break;
			case Cmd::Session::PARA_RETURN_CALLTIMES_LEADER:
			{
				Cmd::Session::t_Return_CallTimes_SceneSession *rev = (Cmd::Session::t_Return_CallTimes_SceneSession*)ptNullCmd;
				SceneUser *pUser=SceneUserManager::getMe().getUserByTempID(rev->leaderTempID);
				if(pUser)
				{
					if ((pUser->privatestore.step() != PrivateStore::NONE)||
							(pUser->tradeorder.hasBegin())||
							(pUser->getState() == SceneUser::SceneEntry_Death))
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "使用道具后不允许交易和摆摊");
						return false;
					}
					// if(!rev->times)
					// {
					// 	switch(rev->type)
					// 	{
					// 		case Cmd::CALL_DUTY_KING:
					// 			{
					// 				Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "今天不能再用国王令啦");
					// 			}
					// 			break;
					// 		case Cmd::CALL_DUTY_UNION:
					// 			{
					// 				Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "今天不能再用帮会令啦");
					// 			}
					// 			break;
					// 		case Cmd::CALL_DUTY_SEPT:
					// 			{
					// 				Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "今天不能再用家族令啦");
					// 			}
					// 			break;
					// 	}
					// }
					// else
					// {
						zObject *srcobj=pUser->packs.uom.getObjectByThisID(rev->qwThisID);
						if(srcobj && srcobj->data.pos.loc() ==Cmd::OBJECTCELLTYPE_COMMON)
						{
							pUser->useCallObj(srcobj);
						}
					// }
				}
				return true;
			}
			break;
		default:
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

bool SessionClient::cmdMsgParse_Country(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	switch (ptNullCmd->para)
	{
	  case Cmd::Session::PARA_SET_DIPLOMAT_STATE: //soke 2016 - 3 - 11 外交官（停用）
			{
				Cmd::Session::t_setDiplomatState_SceneSession* rev = 
					(Cmd::Session::t_setDiplomatState_SceneSession*)ptNullCmd;

				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);

				if (pUser)
				{
					pUser->setDiplomatState(rev->byState);
					pUser->reSendMyMapData();
				}
				
				return true;
			}
			break;
		case Cmd::Session::PARA_SET_CATCHER_STATE: //soke 2016 - 3 - 11 巡捕（左上）
			{
				Cmd::Session::t_setCatcherState_SceneSession* rev =
					(Cmd::Session::t_setCatcherState_SceneSession*)ptNullCmd;

				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);
				
				if (pUser)
				{
					pUser->setCatcherState(rev->byState);
					pUser->reSendMyMapData();
				}

				return true;
			}
			break;
		case Cmd::Session::PARA_SET_CATCHEX_STATE: //soke 2016 - 3 - 11 巡捕（左下）
			{
				Cmd::Session::t_setCatchexState_SceneSession* rev =
					(Cmd::Session::t_setCatchexState_SceneSession*)ptNullCmd;

				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);
				
				if (pUser)
				{
					pUser->setCatchexState(rev->byState);
					pUser->reSendMyMapData();
				}

				return true;
			}
			break;
		
		case Cmd::Session::PARA_SET_CENSOR_STATE: //soke 2016 - 3 - 11 御史大夫（右上）
			{
				Cmd::Session::t_setCensorState_SceneSession* rev = 
					(Cmd::Session::t_setCensorState_SceneSession*)ptNullCmd;

				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);

				if (pUser)
				{
					pUser->setCensorState(rev->byState);
					pUser->reSendMyMapData();
				}
				
				return true;
			}
			break;
			case Cmd::Session::PARA_SET_CENSOS_STATE: //soke 2016 - 3 - 11 御史大夫（右下）
			{
				Cmd::Session::t_setCensosState_SceneSession* rev = 
					(Cmd::Session::t_setCensosState_SceneSession*)ptNullCmd;

				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);

				if (pUser)
				{
					pUser->setCensosState(rev->byState);
					pUser->reSendMyMapData();
				}
				
				return true;
			}
			break;
		case Cmd::Session::PARA_SET_YUANSHUAI_STATE: //soke 2016 - 3 - 11 元帅
			{
				Cmd::Session::t_setYuanshuaiState_SceneSession* rev = 
					(Cmd::Session::t_setYuanshuaiState_SceneSession*)ptNullCmd;

				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);

				if (pUser)
				{
					pUser->setYuanshuaiState(rev->byState);
					pUser->reSendMyMapData();
				}
				
				return true;
			}
			break;
		case Cmd::Session::PARA_SET_ZAIXIANG_STATE: //soke 2016 - 3 - 11 宰相
			{
				Cmd::Session::t_setZaixiangState_SceneSession* rev = 
					(Cmd::Session::t_setZaixiangState_SceneSession*)ptNullCmd;

				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);

				if (pUser)
				{
					pUser->setZaixiangState(rev->byState);
					pUser->reSendMyMapData();
				}
				
				return true;
			}
			break;
		case Cmd::Session::PARA_CHECK_USER: //soke 2016 - 3 - 11 捕头(左上)善恶检测
			{
				Cmd::Session::t_checkUser_SceneSession* rev = 
					(Cmd::Session::t_checkUser_SceneSession*)ptNullCmd;

				SceneUser * pUser = SceneUserManager::getMe().getUserByID(rev->dwCheckedID);
				if (pUser->getGoodnessState() == Cmd::GOODNESS_2_1)
				{      
					rev->byState = 1;
				}
				else
				{
					rev->byState = 0;
				}

				sessionClient->sendCmd(rev, nCmdLen);
				
				return true;
			}
			break;

        //soke 2016 - 3 - 11 捕头（右）善恶检测
		case Cmd::Session::PARA_CHECK_USER_CATCHEX:
			{
				Cmd::Session::t_checkUserCatchex_SceneSession* rev = 
					(Cmd::Session::t_checkUserCatchex_SceneSession*)ptNullCmd;

				SceneUser * pUser = SceneUserManager::getMe().getUserByID(rev->dwCheckedID);
				if (pUser->getGoodnessState() == Cmd::GOODNESS_2_1)
				{      
					rev->byState = 1;
				}
				else
				{
					rev->byState = 0;
				}

				sessionClient->sendCmd(rev, nCmdLen);
				
				return true;
			}
			break;
		case Cmd::Session::PARA_COUNTRY_POWER_SORT:
			{
				Cmd::Session::t_countryPowerSort_SceneSession * rev = (Cmd::Session::t_countryPowerSort_SceneSession *)ptNullCmd;
				for(int i=0; i<13;i++) ScenesService::getInstance().countryPower[i] = rev->country[i];
				return true;
			}
			break;
		case Cmd::Session::PARA_REFRESH_GEN:
			{
				Cmd::Session::t_refreshGen_SceneSession *rev = (Cmd::Session::t_refreshGen_SceneSession *)ptNullCmd;
				Scene * s = SceneManager::getInstance().getSceneByID((rev->dwCountryID<<16)+139);
				if (s)
				{
					SceneNpc * old = s->bossMap[COUNTRY_MAIN_GEN];

					if (0==rev->level) rev->level = 1;//0级算1级
					zNpcB *base = npcbm.get(COUNTRY_MAIN_GEN+(rev->level-1)*10);
					//zNpcB *base = npcbm.get(COUNTRY_MAIN_GEN);
					if (0==base)
					{
						Zebra::logger->error("PARA_REFRESH_GEN: 召唤npc时找不到NPC id=%d", COUNTRY_MAIN_GEN+rev->level);
						return true;
					}

					t_NpcDefine define;
					define.id = base->id;
					strcpy(define.name, base->name);
					define.pos = old?old->getPos():zPos(492, 494);
					define.num = 1;
					define.interval = 5;
					define.initstate = zSceneEntry::SceneEntry_Normal;
					define.width = 2;
					define.height = 2;
					define.scriptID = 0;
					s->initRegion(define.region, define.pos, define.width, define.height);

					SceneNpc * boss = s->summonOneNpc<SceneNpc>(define, define.pos, base);
					if (!boss)
					{
						Zebra::logger->debug("PARA_REFRESH_GEN: 召唤NPC失败 id=%d", base->id);
						return true;
					}

					if (old) old->setClearState();
					s->bossMap[COUNTRY_MAIN_GEN] = boss;

					Zebra::logger->trace("刷新大将军 map=%s level=%u pos=(%u,%u)", s->name, rev->level, boss->getPos().x, boss->getPos().y);
				}
				else
					Zebra::logger->error("刷新大将军时没找到地图 mapID=%u", (rev->dwCountryID<<16)+139);
				return true;
			}
			break;
		case Cmd::Session::PARA_SET_EMPEROR_HOLD:
			{
				Cmd::Session::t_setEmperorHold_SceneSession* rev = 
					(Cmd::Session::t_setEmperorHold_SceneSession*)ptNullCmd;

				Scene* pScene = SceneManager::getInstance().getSceneByID(SceneManager::getInstance().buildMapID(6, 134));
				
				if (pScene)
				{
					pScene->setHoldCountry(rev->dwCountryID);
				}
				
				return true;
			}
			break;
		case Cmd::Session::PARA_SUMMON_ALLY_NPC:
				{
					Cmd::Session::t_summonAllyNpc_SceneSession* rev = (Cmd::Session::t_summonAllyNpc_SceneSession*)ptNullCmd;

					char mapName[MAX_NAMESIZE];
					bzero(mapName, MAX_NAMESIZE);
					snprintf(mapName, MAX_NAMESIZE, "%s·王城", SceneManager::getInstance().getCountryNameByCountryID(rev->dwCountryID));

					Scene * scene = SceneManager::getInstance().getSceneByName(mapName);
					if (!scene)
					{
							Zebra::logger->debug("PARA_SUMMON_ALLY_NPC: 召唤npc时找不到地图 mapName=%s", mapName);
							return false;
					}
					zNpcB *base = npcbm.get(ALLY_GUARDNPC);
					if (NULL == base)
					{
							Zebra::logger->debug("PARA_SUMMON_ALLY_NPC: 召唤npc时找不到NPC id=%d", ALLY_GUARDNPC);
							return false;
					}

					t_NpcDefine define;
					define.id = base->id;
					strcpy(define.name, base->name);
					define.pos = zPos(472, 474);
					define.num = 5;
					define.interval = 5;
					define.initstate = zSceneEntry::SceneEntry_Normal;
					define.width = 2;
					define.height = 2;
					define.scriptID = 9001;
					scene->initRegion(define.region, define.pos, define.width, define.height);

					int num = scene->summonNpc(define, define.pos, base);
					if (num<5)
					{
							define.num = 5-num;
							scene->summonNpc(define, define.pos, base);
					}

					Zebra::logger->debug("%s 盟国镖车出发",
									SceneManager::getInstance().getCountryNameByCountryID(scene->getCountryID()));
					return true;
				}
				break;

		case Cmd::Session::PARA_UPDATE_ALLY:
			{
				Cmd::Session::t_updateAlly_SceneSession* rev =
					(Cmd::Session::t_updateAlly_SceneSession*)ptNullCmd;
				CountryAllyM::getMe().processUpdate(rev);
				
				return true;
			}
			break;
		case Cmd::Session::PARA_UPDATE_SCENE_UNION:
			{
				Cmd::Session::t_updateSceneUnion_SceneSession* rev = 
					(Cmd::Session::t_updateSceneUnion_SceneSession*)ptNullCmd;

				Scene* pScene = SceneManager::getInstance().getSceneByID(rev->dwSceneID);
				if (pScene)
				{
					pScene->setHoldUnion(rev->dwUnionID);
				}

				return true;
			}
			break;
		case Cmd::Session::PARA_UPDATE_TECH:
			{
				Cmd::Session::t_updateTech_SceneSession* rev = 
					(Cmd::Session::t_updateTech_SceneSession*)ptNullCmd;
				CountryTech* pCountryTech = CountryTechM::getMe().getCountryTech(rev->dwCountryID);

				if (pCountryTech == NULL)
				{
					CountryTechM::getMe().addCountryTech(rev->dwCountryID);
					pCountryTech = CountryTechM::getMe().getCountryTech(rev->dwCountryID);
				}

				if (pCountryTech)
				{
					pCountryTech->init(rev);
				}
				else
				{
					Zebra::logger->trace("[国家]: %d 未找到对应国家科技信息。", rev->dwCountryID);
				}

				return true;
			}
			break;
		case Cmd::Session::PARA_SEL_TRANS_COUNTRY_WAR:
			{
				Cmd::Session::t_selTransCountryWar_SceneSession* rev =
					(Cmd::Session::t_selTransCountryWar_SceneSession*)ptNullCmd;
				SceneUserManager::getMe().countryTrans(rev->dwCountryID, rev->dwLevel);
				return true;
			}
			break;
		case Cmd::Session::PARA_COUNTRY_PUNISH_USER:
			{
				Cmd::Session::t_countryPunishUser_SceneSession * rev = (Cmd::Session::t_countryPunishUser_SceneSession *)ptNullCmd;
				SceneUser * pUser = SceneUserManager::getMe().getUserByName(rev->name);
				if (!pUser) return true;

				if (1==rev->method)//禁言
				{
					pUser->delayForbidTalk(3600);
					Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你被国王禁言一小时, 一小时之内无法在任何频道聊天");
				}
				else if (2==rev->method)//关监狱
				{
					if (pUser->scene->getRealMapID()!=189 && pUser->scene->getRealMapID()!=203)
					{
						pUser->charbase.punishTime = 60;
						Scene * s=SceneManager::getInstance().getSceneByName("中立区·监牢");
						if (s)
							pUser->changeMap(s, zPos(80,70));
						else
						{
							Cmd::Session::t_changeScene_SceneSession cmd;
							cmd.id = pUser->id;
							cmd.temp_id = pUser->tempid;
							cmd.x = 80;
							cmd.y = 70;
							cmd.map_id = 0;
							cmd.map_file[0] = '\0';
							strncpy((char *)cmd.map_name, "中立区·监牢", MAX_NAMESIZE);
							sessionClient->sendCmd(&cmd, sizeof(cmd));
						}
					}
				}

				return true;
			}
			break;
		case Cmd::Session::PARA_SET_COUNTRY_WAR:
			{
					Cmd::Session::t_setCountryWar_SceneSession* rev = 
							(Cmd::Session::t_setCountryWar_SceneSession*)ptNullCmd;

					Scene* pScene = SceneManager::getInstance().getSceneByID(		                                                SceneManager::getInstance().buildMapID(rev->dwCountryID, rev->dwAreaID));

					if (pScene)
					{
							if (rev->byStatus == 1)
							{

								pScene->setCountryDare(true/*, rev->dwAttCountryID*/);
							}
							else
							{
								pScene->setCountryDare(false);
								pScene->reliveSecGen();
							}
					}

					return true;
			}
			break;
		case Cmd::Session::PARA_WINNER_EXP_SCENE_COUNTRY:
			{
					Cmd::Session::t_updateWinnerExp_SceneSession *rev = 
							(Cmd::Session::t_updateWinnerExp_SceneSession*)ptNullCmd;
					struct WinnerExpSceneExec :public SceneCallBack
					{
							WinnerExpSceneExec(DWORD id,bool type)
							{
									_countryid = id;
									_type = type;
							}
							DWORD _countryid;
							bool _type;
							bool exec(Scene *scene)
							{
									if(_countryid == scene->getCountryID())
									{
											scene->winner_exp = _type;
									}
									return true;
							}
					};
					WinnerExpSceneExec exec(rev->countryID , rev->type); 
					SceneManager::getInstance().execEveryScene(exec);
					return true;
			}
			break;
		//soke 会话回复场景国家太庙资源数量
        case Cmd::Session::PARA_REPLY_COUNTRY_PEARL_NUM:
            {
                using namespace Cmd::Session;
                ReplyCountryPearlNum_SceneSession* rev = (ReplyCountryPearlNum_SceneSession *)ptNullCmd;
                if( !rev ) return true;
                CountryTechM::getMe().addLoongAltar(rev->countryID, rev->num);
            }
            break;
        default:
            Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
            break;
    }
	return true;
}

bool SessionClient::cmdMsgParse_Temp(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Session;
	using namespace Cmd::Record;

	switch (ptNullCmd->para)
	{
		case Cmd::Session::GET_CREATE_UNION_ITEM_PARA:
			{
				Cmd::Session::t_GetCreateUnionItem_SceneSession* rev = 
					(Cmd::Session::t_GetCreateUnionItem_SceneSession*)ptNullCmd;

				Cmd::Session::t_ReturnCreateUnionItem_SceneSession send;
				send.dwUserID = rev->dwUserID;

				zObject* item = zObject::create(objectbm.get(UnionDef::CREATE_UNION_NEED_ITEM_ID), 1); 

				if (item)
				{
					zObject::logger(item->createid,item->data.qwThisID,item->data.strName,item->data.dwNum,item->data.dwNum,1,0,"帮会建立道具",rev->dwUserID,NULL,"创建",item->base,item->data.kind,item->data.upgrade);
					zObject::logger(item->createid,item->data.qwThisID,item->data.strName,item->data.dwNum,item->data.dwNum,0,0,"帮会建立道具",rev->dwUserID,NULL,"邮寄到信箱",item->base,item->data.kind,item->data.upgrade);
					item->getSaveData((SaveObject *)&send.item);
					sessionClient->sendCmd(&send, sizeof(send));
				}

				zObject::destroy(item);

				return true;
			}
			break;
		default:
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

bool SessionClient::cmdMsgParse_Union(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Session;
	switch (ptNullCmd->para)
	{
		case Cmd::Session::PARA_CHECK_USER_CITY:
			{
				Cmd::Session::t_checkUserCity_SceneSession* rev = 
					(Cmd::Session::t_checkUserCity_SceneSession*)ptNullCmd;

				SceneUser * pUser = SceneUserManager::getMe().getUserByID(rev->dwCheckedID);
				if (pUser->getGoodnessState() == Cmd::GOODNESS_2_1)
				{      
					rev->byState = 1;
				}
				else
				{
					rev->byState = 0;
				}

				sessionClient->sendCmd(rev, nCmdLen);

		Zebra::logger->debug("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
		Zebra::logger->debug("检测善舞度%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
				return true;
			}
			break;
		default:
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

bool SessionClient::cmdMsgParse_Sept(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Session;
	switch (ptNullCmd->para)
	{
		case Cmd::Session::SEND_SEPT_NORMAL_PARA:
			{
				t_SendSeptNormal_SceneSession* rev = (t_SendSeptNormal_SceneSession*)ptNullCmd;

				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);
				
				if (pUser)
				{
					pUser->dwSeptRepute = rev->dwRepute;
					pUser->sendMeToNine();
				}
				
				return true;
			}
			break;
		case Cmd::Session::GET_SEPT_EXP_PARA:
			{
				t_GetSeptExp_SceneSession* rev = (t_GetSeptExp_SceneSession*)ptNullCmd;

				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);

				if (pUser)
				{
					pUser->addNineSeptExp(rev->dwSeptID);
				}

				return true;
			}
			break;
		case Cmd::Session::GET_SEPT_NORMAL_EXP_PARA:
			{
				t_GetSeptNormalExp_SceneSession* rev = (t_GetSeptNormalExp_SceneSession*)ptNullCmd;

				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);

				if (pUser)
				{
					pUser->addNineSeptNormalExp(rev->dwSeptID);
				}

				return true;
			}
			break;
		default:
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

bool SessionClient::cmdMsgParse_Other(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Session;
	using namespace Cmd::Record;
	typedef std::map<DWORD, DWORD>::value_type valType;

	switch (ptNullCmd->para)
	{
			/*
		case PARA_SCENE_LOAD_PROCESS:
			{
				Zebra::logger->debug("加载特征码文件...%u 字节", ScenesService::updateStampData());
				return true;
			}
			break;
			*/
		//召唤家族BOSS
		case PARA_SCENE_ZHAOHUANJIAZUBOSS_RANK_REWARD:
		{
			Cmd::Session::t_sendZhaoHuanJiazuBossRankReward *rev=(Cmd::Session::t_sendZhaoHuanJiazuBossRankReward *)ptNullCmd;
			SceneUser* pUser = NULL;
			pUser = SceneUserManager::getMe().getUserByID(rev->charID);
			if(pUser)
			{
				int level = rev->level;
				switch (level)
				{
				// case 2:
				// 	{
				// 		Gm::summon(pUser,"id=144002 num=1");
				// 	}
				// 	break;
				// case 3:
				// 	{
				// 		Gm::summon(pUser,"id=144003 num=1");
				// 	}
				// 	break;
				// case 4:
				// 	{
				// 		Gm::summon(pUser,"id=144004 num=1");
				// 	}
				// 	break;
				// case 5:
				// 	{
				// 		Gm::summon(pUser,"id=144005 num=1");
				// 	}
				// 	break;
				// case 6:
				// 	{
				// 		Gm::summon(pUser,"id=144009 num=1");
				// 	}
				// 	break;
				// case 7:
				// 	{
				// 		Gm::summon(pUser,"id=144010 num=1");
				// 	}
				// 	break;
				// case 8:
				// 	{
				// 		Gm::summon(pUser,"id=144011 num=1");
				// 	}
				// 	break;
				// case 9:
				// 	{
				// 		Gm::summon(pUser,"id=144012 num=1");
				// 	}
				// 	break;
				case 10:
					{
						Gm::summon(pUser,"id=144013 num=1 x=122 y=32");
					}
					break;
				default:
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "只有10级的BOSS可以召唤");
						return true;
					}
					break;
				}
				
				Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "召唤成功");
			}

		}
		break;
		//战车归还
		case PARA_SCENE_ZHANCHEGUIHUAN_RANK_REWARD:
		{
			Cmd::Session::t_sendZhancheGuihuanRankReward *rev=(Cmd::Session::t_sendZhancheGuihuanRankReward *)ptNullCmd;
			SceneUser* pUser = NULL;
			pUser = SceneUserManager::getMe().getUserByID(rev->charID);
			if(pUser)
			{
				pUser->charbase.zhancheid = 0;
				pUser->charbase.zhanchetype = 0;
				Channel::sendSys(pUser, Cmd::INFO_TYPE_EXP5, "战车归还成功");
				Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "战车归还成功");
				Cmd::stZhancheLingyongReturnCmd send;
				send.state = 2;
                pUser->sendCmdToMe(&send,sizeof(send));
				if(pUser->issetUState(1410) )
				{
					pUser->showCurrentEffect(1410, false ); 
				}
				//刷新用户数据
				Cmd::stMainUserDataUserCmd  userinfo;
				pUser->full_t_MainUserData(userinfo.data);
				pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
			}

		}
		break;
		//战车领用
		case PARA_SCENE_ZHANCHELINGYONG_RANK_REWARD:
		{
			Cmd::Session::t_sendZhancheLingYongRankReward *rev=(Cmd::Session::t_sendZhancheLingYongRankReward *)ptNullCmd;
			SceneUser* pUser = NULL;
			pUser = SceneUserManager::getMe().getUserByID(rev->charID);
			if(pUser)
			{
				pUser->charbase.zhancheid = rev->zhancheid;
				pUser->charbase.zhanchetype = rev->zhanchetype;
				Channel::sendSys(pUser, Cmd::INFO_TYPE_EXP5, "战车领用成功");
				Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "战车领用成功");
				Cmd::stZhancheLingyongReturnCmd send;
				send.state = 1;
                pUser->sendCmdToMe(&send,sizeof(send));
				//刷新用户数据
				Cmd::stMainUserDataUserCmd  userinfo;
				pUser->full_t_MainUserData(userinfo.data);
				pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
			}

		}
		break;
		case Cmd::Session::PARA_SCENE_GONGXUN_RANK_REWARD://功勋竞猜开奖
			{
				Cmd::Session::t_sendGongxunRankReward *rev=(Cmd::Session::t_sendGongxunRankReward *)ptNullCmd;
			
				Zebra::logger->info("[功勋竞猜]功勋开奖开始发送道具：名字：%s ID:%d 物品ID:%d 物品数量:%d",rev->name,rev->charID,rev->itemId,rev->itemNum);
				
				zObjectB *base = objectbm.get(rev->itemId);
				if (base)
				{
					Zebra::logger->info("[功勋竞猜]物品查找成功%s ID:%d 物品ID:%d 物品数量:%d",rev->name,rev->charID,rev->itemId,rev->itemNum);
					
					if(rev->itemNum*rev->beishu>base->maxnum)
					{
						//醉梦 处理发邮件max值问题
						int allnum = rev->itemNum*rev->beishu;//全部数量
						while(allnum>0)
						{
							if(allnum>=base->maxnum)
							{
								zObject* o = zObject::create(base, base->maxnum);
								if (o)
								{
									o->data.upgrade = rev->itemLevel;
									Zebra::logger->info("[功勋竞猜]物品创建成功%s ID:%d 物品ID:%d 物品数量:%d",rev->name,rev->charID,rev->itemId,base->maxnum);
							
									o->data.bind = 1; //功勋竞猜非绑
									Zebra::logger->info("功勋开奖道具发送：%s",rev->name);
									char text[64];
									sprintf(text, "恭喜您成功赢的本期功勋竞猜，获得奖励：物品:%s 数量:%d！",o->data.strName,o->data.dwNum);
									sendMail("功勋竞猜奖品", 0, rev->name, rev->charID, Cmd::Session::MAIL_TYPE_SYS, 0, o, text);
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP,"恭喜%s在功勋竞猜活动中中奖，获得%s%d个，真是令人羡慕啊！", rev->name,o->data.strName,allnum);	
									zObject::logger(o->createid, o->data.qwThisID, o->base->name, o->data.dwNum, 1, 2, 0, NULL, rev->charID, rev->name, "活动奖品",o->base,o->data.kind,o->data.upgrade);
									zObject::destroy(o);
								}	
								allnum-=base->maxnum;
							}
							else{
								zObject* o = zObject::create(base, allnum);
								if (o)
								{
									o->data.upgrade = rev->itemLevel;
									Zebra::logger->info("[功勋竞猜]物品创建成功%s ID:%d 物品ID:%d 物品数量:%d",rev->name,rev->charID,rev->itemId,base->maxnum);
							
									o->data.bind = 1; //功勋竞猜非绑
									Zebra::logger->info("功勋开奖道具发送：%s",rev->name);
									char text[64];
									sprintf(text, "恭喜您成功赢的本期功勋竞猜，获得奖励：物品:%s 数量:%d！",o->data.strName,o->data.dwNum);
									sendMail("功勋竞猜奖品", 0, rev->name, rev->charID, Cmd::Session::MAIL_TYPE_SYS, 0, o, text);
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP,"恭喜%s在功勋竞猜活动中中奖，获得%s%d个，真是令人羡慕啊！", rev->name,o->data.strName,allnum);	
								
									zObject::logger(o->createid, o->data.qwThisID, o->base->name, o->data.dwNum, 1, 2, 0, NULL, rev->charID, rev->name, "活动奖品",o->base,o->data.kind,o->data.upgrade);
									zObject::destroy(o);
								}	
								allnum=0;
							}
						}	
					}
					else{
						zObject* o = zObject::create(base, rev->itemNum*rev->beishu);
						if (o)
						{
							o->data.upgrade = rev->itemLevel;
							Zebra::logger->info("[功勋竞猜]物品创建成功%s ID:%d 物品ID:%d 物品数量:%d",rev->name,rev->charID,rev->itemNum*rev->beishu,rev->itemNum);
					
							o->data.bind = 1; //功勋竞猜非绑
							Zebra::logger->info("功勋开奖道具发送：%s",rev->name);
							char text[64];
							sprintf(text, "恭喜您成功赢的本期功勋竞猜，获得奖励：物品:%s 数量:%d！",o->data.strName,o->data.dwNum);
							sendMail("功勋竞猜奖品", 0, rev->name, rev->charID, Cmd::Session::MAIL_TYPE_SYS, 0, o, text);
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP,"恭喜%s在功勋竞猜活动中中奖，获得%s%d个，真是令人羡慕啊！", rev->name,o->data.strName,o->data.dwNum);	
							zObject::logger(o->createid, o->data.qwThisID, o->base->name, o->data.dwNum, 1, 2, 0, NULL, rev->charID, rev->name, "活动奖品",o->base,o->data.kind,o->data.upgrade);
							zObject::destroy(o);
						}	
					}
					
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_GONGXUN_RANK_REWARD2://功勋竞猜未中奖
			{
				Cmd::Session::t_sendGongxunRankReward2 *rev=(Cmd::Session::t_sendGongxunRankReward2 *)ptNullCmd;
			
				SceneUser* pUser = NULL;
				pUser = SceneUserManager::getMe().getUserByID(rev->charID);
				if(pUser)
				{
					pUser->charbase.jingcai0_objectid = 0;
					pUser->charbase.jingcai0_objectlevel = 0;
					pUser->charbase.jingcai0_objectnum = 0;
						
					pUser->charbase.jingcai1_objectid = 0;
					pUser->charbase.jingcai1_objectlevel = 0;
					pUser->charbase.jingcai1_objectnum = 0;

					pUser->charbase.jingcai2_objectid = 0;
					pUser->charbase.jingcai2_objectlevel = 0;
					pUser->charbase.jingcai2_objectnum = 0;

					pUser->charbase.jingcai3_objectid = 0;
					pUser->charbase.jingcai3_objectlevel = 0;
					pUser->charbase.jingcai3_objectnum = 0;

					pUser->charbase.jingcai4_objectid = 0;
					pUser->charbase.jingcai4_objectlevel = 0;
					pUser->charbase.jingcai4_objectnum = 0;

					pUser->charbase.jingcai5_objectid = 0;
					pUser->charbase.jingcai5_objectlevel = 0;
					pUser->charbase.jingcai5_objectnum = 0;

					pUser->charbase.jingcai6_objectid = 0;
					pUser->charbase.jingcai6_objectlevel = 0;
					pUser->charbase.jingcai6_objectnum = 0;

					pUser->charbase.jingcai7_objectid = 0;
					pUser->charbase.jingcai7_objectlevel = 0;
					pUser->charbase.jingcai7_objectnum = 0;

					pUser->charbase.jingcai8_objectid = 0;
					pUser->charbase.jingcai8_objectlevel = 0;
					pUser->charbase.jingcai8_objectnum = 0;

					pUser->charbase.jingcai9_objectid = 0;
					pUser->charbase.jingcai9_objectlevel = 0;
					pUser->charbase.jingcai9_objectnum = 0;		

					pUser->charbase.jingcai10_objectid = 0;
					pUser->charbase.jingcai10_objectlevel = 0;
					pUser->charbase.jingcai10_objectnum = 0;		

					pUser->charbase.jingcai11_objectid = 0;
					pUser->charbase.jingcai11_objectlevel = 0;
					pUser->charbase.jingcai11_objectnum = 0;		
					Zebra::logger->info("[功勋竞猜]: 非中奖用户，执行清零完毕 名字:%s ",pUser->charbase.name);
					pUser->save(Cmd::Record::OPERATION_WRITEBACK); //立刻存档			
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_ZHANLI_RANK_REWARD:
			{
				Cmd::Session::t_sendZhanliRankReward *rev=(Cmd::Session::t_sendZhanliRankReward *)ptNullCmd;
				int itemID[] = {202301,202302,202303,202304,202305,202306,202307,202308,202309,202310};
				int len = (rev->len > 10) ? 10 : rev->len;
				for (int i = 0; i < len; i++)
				{
					// if(i<3)
					// {
						SceneUser* pUser = NULL;
						pUser = SceneUserManager::getMe().getUserByID(rev->zhanliName[i].charID);
						if(pUser)
						{
							pUser->charbase.zhanlicount = i+1;
							
						}
					//战力榜离线发放
						else{
								Cmd::Session::t_Zhanlifafang_SceneSession send;
								send.charid = rev->zhanliName[i].charID;
								send.index =  i+1;
								sessionClient->sendCmd(&send, sizeof(send));
						}
					// }
					// Zebra::logger->debug("name:%s",rev->zhanliName[i].name);
					// zObjectB *base = objectbm.get(itemID[i]);
					// if (base)
					// {
					// 	zObject* o = zObject::create(base, 1);
					// 	if (o)
					// 	{
					// 		o->data.bind = 1;
					// 		sendMail("战力榜发放员", 0, rev->zhanliName[i].name, rev->zhanliName[i].charID, Cmd::Session::MAIL_TYPE_SYS, 0, o, "恭喜您获得系统下发的战力排行榜奖励");
					// 		zObject::logger(o->createid, o->data.qwThisID, o->base->name, o->data.dwNum, 1, 2, 0, NULL, rev->zhanliName[i].charID, rev->zhanliName[i].name, "活动奖品",o->base,o->data.kind,o->data.upgrade);
					// 		zObject::destroy(o);
					// 	}
					// }
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_HERO_RANK_REWARD:
			{
				Cmd::Session::t_sendHeroliRankReward *rev=(Cmd::Session::t_sendHeroliRankReward *)ptNullCmd;
				/* for (DWORD i = 0; i < rev->len; i++)
					Zebra::logger->debug("name:%s,charid:%d",rev->heroName[i].name, rev->heroName[i].charID); */
					//此处存在 字符和无字符比对
				int itemID[] = {202311,202312,202313};
				int len = (rev->len > 3) ? 3 : rev->len;
				for (int i = 0; i < rev->len; i++)
				{
					SceneUser* pUser = NULL;
				    pUser = SceneUserManager::getMe().getUserByID(rev->heroName[i].charID);
                    if(pUser)
					{
						pUser->charbase.herocount = 1;
						//Zebra::logger->debug("设置护国榜前三名玩家[ID:%d][%s]herocount=%d",rev->flowerName[i].charID,rev->flowerName[i].name,pUser->charbase.herocount);

					}
					//护国榜离线发放
					else{
							Cmd::Session::t_Huguofafang_SceneSession send;
							send.charid = rev->heroName[i].charID;
							send.index =  1;
							sessionClient->sendCmd(&send, sizeof(send));
					}
					// zObjectB *base = objectbm.get(itemID[i]);
					// if (base)
					// {
					// 	zObject* o = zObject::create(base, 1);
					// 	if (o)
					// 	{
					// 		o->data.bind = 1;
					// 		sendMail("护国榜发放员", 0, rev->heroName[i].name, rev->heroName[i].charID, Cmd::Session::MAIL_TYPE_SYS, 0, o, "恭喜您获得系统下发的英雄排行榜奖励");
					// 		zObject::logger(o->createid, o->data.qwThisID, o->base->name, o->data.dwNum, 1, 2, 0, NULL, rev->heroName[i].charID, rev->heroName[i].name, "活动奖品",o->base,o->data.kind,o->data.upgrade);
					// 		zObject::destroy(o);
					// 	}
					// }
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_TICKET_RANK_REWARD:
			{
				Cmd::Session::t_sendTicketRankReward *rev=(Cmd::Session::t_sendTicketRankReward *)ptNullCmd;
				// int itemID[] = {202394,202395,202396};
				// int len = (rev->len > 3) ? 3 : rev->len;
				// for (int i = 0; i < len; i++)
				// {
				// 	Zebra::logger->debug("name:%s",rev->ticketName[i].name);
				// 	zObjectB *base = objectbm.get(itemID[i]);
				// 	if (base)
				// 	{
				// 		zObject* o = zObject::create(base, 1);
				// 		if (o)
				// 		{
				// 			o->data.bind = 1;
				// 			sendMail("积分榜发放员", 0, rev->ticketName[i].name, rev->ticketName[i].charID, Cmd::Session::MAIL_TYPE_SYS, 0, o, "恭喜您获得系统下发的积分排行榜奖励");
				// 			zObject::logger(o->createid, o->data.qwThisID, o->base->name, o->data.dwNum, 1, 2, 0, NULL, rev->ticketName[i].charID, rev->ticketName[i].name, "活动奖品",o->base,o->data.kind,o->data.upgrade);
				// 			zObject::destroy(o);
				// 		}
				// 	}
				// }
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_FLOWER_RANK_REWARD:
			{
				Cmd::Session::t_sendFlowerRankReward *rev=(Cmd::Session::t_sendFlowerRankReward *)ptNullCmd;
				int itemID[] = {202391,202392,202393};
				int len = (rev->len > 3) ? 3 : rev->len;
				for (int i = 0; i < len; i++)
				{
					Zebra::logger->debug("name:%s",rev->flowerName[i].name);
					SceneUser* pUser = NULL;
				    pUser = SceneUserManager::getMe().getUserByID(rev->flowerName[i].charID);
                    if(pUser)
					{
						pUser->charbase.xhbbcount = 1;
						//Zebra::logger->debug("设置鲜花榜前三名玩家[ID:%d][%s]xhbbcount=%d",rev->flowerName[i].charID,rev->flowerName[i].name,pUser->charbase.xhbbcount);
					}
					//鲜花榜离线发放
					else{
						Cmd::Session::t_Xianhuafafang_SceneSession send;
						send.charid = rev->flowerName[i].charID;
						send.index =  1;
						sessionClient->sendCmd(&send, sizeof(send));
					}
					// zObjectB *base = objectbm.get(itemID[i]);
					// if (base)
					// {
					// 	zObject* o = zObject::create(base, 1);
					// 	if (o)
					// 	{
					// 		o->data.bind = 1;
					// 		sendMail("鲜花榜发放员", 0, rev->flowerName[i].name, rev->flowerName[i].charID, Cmd::Session::MAIL_TYPE_SYS, 0, o, "恭喜您获得系统下发的鲜花排行榜奖励");
					// 		zObject::logger(o->createid, o->data.qwThisID, o->base->name, o->data.dwNum, 1, 2, 0, NULL, rev->flowerName[i].charID, rev->flowerName[i].name, "活动奖品",o->base,o->data.kind,o->data.upgrade);
					// 		zObject::destroy(o);
					// 	}
					// }
				}
				return true;
			}
			break;		
		case Cmd::Session::PARA_CHECKRELATION_RESULT:
			{
				Cmd::Session::t_CheckRelationEmptyResult *checkCmd = (Cmd::Session::t_CheckRelationEmptyResult *)ptNullCmd;
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(checkCmd->dwUserID);	
				pUser->doTurnCmd((Cmd::stTurnUserCmd *)ptNullCmd,sizeof(*checkCmd));
				return true;
			}
			break;
		case Cmd::Session::PICKUP_MASTER_SCENE_PARA:
			{
				Cmd::Session::t_PickupMaster_SceneSession* rev = 
					(Cmd::Session::t_PickupMaster_SceneSession*)ptNullCmd;

				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);
				if (!pUser)
				{
					return true;
				}

				pUser->packs.addMoney(rev->dwMoney, "推荐人领取奖励", NULL, true);

				return true;
			}
			break;
		case Cmd::Session::PARA_CLOSE_NPC:
			{
				SceneNpcManager::getMe().closeFunctionNpc();
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_SEND_CMD:
			{
				Cmd::Session::t_sendCmd_SceneSession * rev = (t_sendCmd_SceneSession *)ptNullCmd;
				Scene * s = SceneManager::getInstance().getSceneByID(rev->mapID);
				if (!s) return false;

				s->sendCmdToScene(rev->cmd, rev->len);
				return true;
			}
			break;
		case Cmd::Session::PARA_SET_SERVICE:
			{
				Cmd::Session::t_SetService_SceneSession *rev = (Cmd::Session::t_SetService_SceneSession *)ptNullCmd;

				DWORD old = atoi(Zebra::global["service_flag"].c_str());

				char buf[32];
				bzero(buf, sizeof(buf));
				snprintf(buf, 32, "%u", rev->flag);
				Zebra::global["service_flag"] = buf;
				Zebra::logger->debug("设置服务 flag=%s", buf);

				if (((old^rev->flag)&Cmd::Session::SERVICE_PROCESS) && ScenesService::pStampData)
				{
					if (rev->flag & Cmd::Session::SERVICE_PROCESS)
						ScenesService::updateStampData();
					else
						ScenesService::pStampData->dwChannelID = false;
					Zebra::logger->debug("%sprocess服务",ScenesService::pStampData->dwChannelID?"打开":"关闭");
				}
				return true;
			}
			break;
		case Cmd::Session::PARA2_SET_SERVICE://soke 封印等级(未转生)
			{
				Cmd::Session::t_SetFylevel_SceneSession *rev = (Cmd::Session::t_SetFylevel_SceneSession *)ptNullCmd;

				DWORD old = atoi(Zebra::global["serverfy_level"].c_str());

				char buf[32];
				bzero(buf, sizeof(buf));
				snprintf(buf, 32, "%u", rev->flag);
				Zebra::global["serverfy_level"] = buf;
				Zebra::logger->debug("设置服务器封印等级 flag=%s", buf);

				if (((old^rev->flag)&Cmd::Session::SERVICE2_PROCESS) && ScenesService::pStampData)
				{
					if (rev->flag & Cmd::Session::SERVICE2_PROCESS)
						ScenesService::updateStampData();
					else
						ScenesService::pStampData->dwChannelID = false;
					Zebra::logger->debug("%sprocess服务",ScenesService::pStampData->dwChannelID?"打开":"关闭");
				}
				return true;
			}
			break;
		case Cmd::Session::PARA3_SET_SERVICE://soke 服务器最高等级(未转生)
			{
				Cmd::Session::t_SetMaxlevel_SceneSession *rev = (Cmd::Session::t_SetMaxlevel_SceneSession *)ptNullCmd;

				DWORD old = atoi(Zebra::global["servermax_level"].c_str());

				char buf[32];
				bzero(buf, sizeof(buf));
				snprintf(buf, 32, "%u", rev->flag);
				Zebra::global["servermax_level"] = buf;
				Zebra::logger->debug("设置服务器最高等级 flag=%s", buf);

				if (((old^rev->flag)&Cmd::Session::SERVICE3_PROCESS) && ScenesService::pStampData)
				{
					if (rev->flag & Cmd::Session::SERVICE3_PROCESS)
						ScenesService::updateStampData();
					else
						ScenesService::pStampData->dwChannelID = false;
					Zebra::logger->debug("%sprocess服务",ScenesService::pStampData->dwChannelID?"打开":"关闭");
				}
				return true;
			}
			break;
			case Cmd::Session::PARA4_SET_SERVICE://soke 封印等级(转生)
			{
				Cmd::Session::t_SetTrunFylevel_SceneSession *rev = (Cmd::Session::t_SetTrunFylevel_SceneSession *)ptNullCmd;

				DWORD old = atoi(Zebra::global["trunfy_level"].c_str());

				char buf[32];
				bzero(buf, sizeof(buf));
				snprintf(buf, 32, "%u", rev->flag);
				Zebra::global["trunfy_level"] = buf;
				Zebra::logger->debug("设置服务器(转生)封印等级 flag=%s", buf);

				if (((old^rev->flag)&Cmd::Session::SERVICE4_PROCESS) && ScenesService::pStampData)
				{
					if (rev->flag & Cmd::Session::SERVICE4_PROCESS)
						ScenesService::updateStampData();
					else
						ScenesService::pStampData->dwChannelID = false;
					Zebra::logger->debug("%sprocess服务",ScenesService::pStampData->dwChannelID?"打开":"关闭");
				}
				return true;
			}
			break;
		case Cmd::Session::PARA5_SET_SERVICE://soke 服务器最高等级(转生)
			{
				Cmd::Session::t_SetTrunMaxlevel_SceneSession *rev = (Cmd::Session::t_SetTrunMaxlevel_SceneSession *)ptNullCmd;

				DWORD old = atoi(Zebra::global["maxtrun_level"].c_str());

				char buf[32];
				bzero(buf, sizeof(buf));
				snprintf(buf, 32, "%u", rev->flag);
				Zebra::global["maxtrun_level"] = buf;
				Zebra::logger->debug("设置服务器（转生）最高等级 flag=%s", buf);

				if (((old^rev->flag)&Cmd::Session::SERVICE5_PROCESS) && ScenesService::pStampData)
				{
					if (rev->flag & Cmd::Session::SERVICE5_PROCESS)
						ScenesService::updateStampData();
					else
						ScenesService::pStampData->dwChannelID = false;
					Zebra::logger->debug("%sprocess服务",ScenesService::pStampData->dwChannelID?"打开":"关闭");
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_GUARD_FAIL:
			{
				Cmd::Session::t_guardFail_SceneSession * rev = (Cmd::Session::t_guardFail_SceneSession *)ptNullCmd;
				SceneUser *pUser=SceneUserManager::getMe().getUserByTempID(rev->userID);
				if (pUser)
				{
					OnOther event(2);
					EventTable::instance().execute(*pUser, event);
					Channel::sendSys(pUser, Cmd::INFO_TYPE_EXP, "你的护送目标死亡");
				}
			}
			break;
		case Cmd::Session::PARA_UNION_DARE_NOTIFY:
			{
				Cmd::Session::t_unionDareNotify_SceneSession *rev = (Cmd::Session::t_unionDareNotify_SceneSession*)ptNullCmd;
				Scene * scene = SceneManager::getInstance().getSceneByID(rev->sceneID);
				if(!scene) return true;

				scene->setUnionDare(rev->state);
				Zebra::logger->debug("收到帮会状态消息 state=%u", rev->state);
				return true;
			}
			break;
		//soke 2016 - 3 - 10 给师父分配银子
		case Cmd::Session::OVERMAN_MONEY_ADD:
			{
				Cmd::Session::t_OvermanMoneyAdd *rev = (Cmd::Session::t_OvermanMoneyAdd*)ptNullCmd;
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->id);
				if(pUser)
				{
					std::string disc = "消费奖励:"; 
					disc += rev->name;
					pUser->packs.addMoney(rev->money,disc.c_str(),rev->name);
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_CLEAR_UNION_MANOR:
			{
				Cmd::Session::t_clearUnionManor_SceneSession* rev = 
					(Cmd::Session::t_clearUnionManor_SceneSession*)ptNullCmd;

				Scene* pScene = SceneManager::getInstance().getSceneByID(
						SceneManager::getInstance().buildMapID(rev->dwCountryID, rev->dwAreaID));

				if (pScene)
				{
					ClearAllNotUnion clearMaron(rev->dwUnionID);
					pScene->execAllOfScene(zSceneEntry::SceneEntry_Player, clearMaron);
					clearMaron.reliveAll();
				}

				return true;
			}
			break;
		case Cmd::Session::PARA_RETURN_ENTER_UNION_MANOR:
			{
				Cmd::Session::t_returnEnterUnionManor_SceneSession* rev = 
					(Cmd::Session::t_returnEnterUnionManor_SceneSession*)ptNullCmd;

				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);
				Cmd::Session::t_changeScene_SceneSession cmd;
				Scene* pScene= SceneManager::getInstance().getSceneByID(SceneManager::getInstance().buildMapID(rev->dwCountryID, rev->dwAreaID));

				if (pUser) 
				{
					if (pUser->isAtt(Cmd::UNION_CITY_DARE))
					{
						pUser->charbase.gomaptype = ZoneTypeDef::ZONE_PRIVATE_DARE_UNION;
					}
					else
					{
						if (pUser->isSpecWar(Cmd::UNION_CITY_DARE))
						{//对战时，守方送往这里
							pUser->charbase.gomaptype = ZoneTypeDef::ZONE_PRIVATE_UNION; // 帮会所属地跳转点类型
						}
						else
						{
							if (pUser->scene->getUnionDare())
							{// 对战时，第三方送往这里
								pUser->charbase.gomaptype = 
									ZoneTypeDef::ZONE_PRIVATE_THIRD_UNION;
							}
							else
							{
								// 帮会所属地跳转点类型
								pUser->charbase.gomaptype = ZoneTypeDef::ZONE_PRIVATE_UNION;
							}
						}
					}
				}

				if (pScene && pUser)
				{//本服
					zPos Pos;
					Pos.x = 0;
					Pos.y = 0;

					pUser->changeMap(pScene, Pos);
				}
				else if (pUser)
				{

					cmd.id = pUser->id;
					cmd.temp_id = pUser->tempid;
					cmd.x = 0;
					cmd.y = 0;
					cmd.map_id = SceneManager::getInstance().buildMapID(rev->dwCountryID, rev->dwAreaID);
					bzero(cmd.map_file, sizeof(cmd.map_file));
					bzero(cmd.map_name, sizeof(cmd.map_file));
					sessionClient->sendCmd(&cmd, sizeof(cmd));
				}

				return true;
			}
			break;
		case Cmd::Session::PARA_TRANS_DARE_COUNTRY:
			{
				Cmd::Session::t_transDareCountry_SceneSession* rev = 
					(Cmd::Session::t_transDareCountry_SceneSession*)ptNullCmd;

				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);

				if (rev->dwMoney<=0 || (pUser->packs.checkMoney(abs(rev->dwMoney)) 
							&& pUser->packs.removeMoney(abs(rev->dwMoney),"国战传送"))) 
				{
					Cmd::Session::t_changeScene_SceneSession cmd;
					pUser->charbase.gomaptype = ZoneTypeDef::ZONE_PRIVATE_DARE; // 国战跳转区域

					cmd.id = pUser->id;
					cmd.temp_id = pUser->tempid;
					cmd.x = 0;
					cmd.y = 0;
					cmd.map_id = SceneManager::getInstance().buildMapID(rev->dwCountry, 139); //无双城

					bzero(cmd.map_file, sizeof(cmd.map_file));
					bzero(cmd.map_name, sizeof(cmd.map_file));
					sessionClient->sendCmd(&cmd, sizeof(cmd));
				}

				return true;
			}
			break;
		case Cmd::Session::PARA_RETURN_CHANGE_COUNTRY:
			{
				Cmd::Session::t_returnChangeCountry_SceneSession* rev = (Cmd::Session::
						t_returnChangeCountry_SceneSession*)ptNullCmd;

				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);
				if (pUser) pUser->relive();
				return true;
			}
			break;
		case Cmd::Session::PARA_QUIZ_AWARD:
			{
				Cmd::Session::t_quizAward_SceneSession* rev = (Cmd::Session::t_quizAward_SceneSession*)ptNullCmd;
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);

				if (pUser)
				{
					DWORD addExp = rev->dwExp;

					if (rev->byType == 1)
					{
						int cur_answer_count = 0;
						if ((int)pUser->charbase.answerCount>1)
						{
							cur_answer_count = pUser->charbase.answerCount - 1;
						}
						else
						{
							cur_answer_count = 0;
						}
						//int(0.14*答题分数*角色等级^2*(0.75+0.25*N)+200)
				//		addExp = (DWORD)(addExp*(0.75+0.25*cur_answer_count)) + 200;
				
					//int(0.2*答题分数*角色等级^2*(0.8+0.2*N)+200)
					//	addExp = (DWORD)(addExp*(0.8+0.2*cur_answer_count)) + 200;

//前4次答题所给经验=int(（0.12+0.05*n）*答题分数*角色等级^2 +200)
//  第五次答题所给经验=int(1*答题分数*角色等级^2 +200)
//addExp=答题分数*角色等级^2
						if ( pUser->charbase.answerCount > 5 )
							addExp = (DWORD)(addExp) + 200;
						else
							addExp = (DWORD)(addExp*(0.12+0.05*cur_answer_count)) + 200;
						Zebra::logger->trace("[个人答题]: %s(%u) 获得经验 %d", pUser->name, pUser->id, addExp);
						struct tm tv1;
						time_t timValue = time(NULL);
						zRTime::getLocalTime(tv1, timValue);

/*
						//tv1.tm_wday = (1-7)
						//给个东西
	                                        if (pUser->charbase.answerCount==5 && tv1.tm_wday ==3)
        	                                {
							
                	                                zObjectB *base = NULL;
                        	                        base = objectbm.get(1986);
                                	                if(!base)
                                        	                Zebra::logger->error("无法获取物品");
                                                	zObject* o = NULL;
	                                                o = zObject::create(base,1, 0);
        	                                        if(o)
                	                                {
                        	                                Zebra::logger->debug("create");
                                	                        zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,o->data.dwNum,1,0,NULL,pUser->id,pUser->name,"fetch",o->base,o->data.kind,o->data.upgrade);
                                        	                if(o->data.dwNum!=0 && pUser->packs.addObject(o,true, AUTO_PACK))
                                                	        {
                                                        	        Cmd::stAddObjectPropertyUserCmd send;
                                                                	bcopy(&o->data,&send.object,sizeof(t_Object));
                                                                	pUser->sendCmdToMe(&send,sizeof(send));
                                                                	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "得到物品%s%ld个",o->name,o->data.dwNum);

                  	                                        }
                                                	}
                                        	}*/
					}
					else
					{
						Zebra::logger->trace("[全国答题]: %s(%u) 获得经验 %d", pUser->name, pUser->id, addExp);
					}

					pUser->addExp(addExp);
					Channel::sendSys(pUser, Cmd::INFO_TYPE_EXP, "得到经验值 %d", addExp);

					if (rev->dwMoney>0)
					{
						pUser->packs.addMoney(rev->dwMoney, "竞赛奖励",NULL);
					}


					if (pUser->charbase.country != PUBLIC_COUNTRY)
					{
						pUser->charbase.grace += rev->dwGrace;
					}

					// 恢复可见
					pUser->setState(zSceneEntry::SceneEntry_Normal);
					zPos curPos = pUser->getPos();
					pUser->goTo(curPos);
					pUser->isQuiz = false;
				}

				return true;
			}
			break;
		case Cmd::Session::PARA_QUESTION_OBJECT:
			{
				Cmd::Session::t_questionObject_SceneSession* rev = 
					(Cmd::Session::t_questionObject_SceneSession*)ptNullCmd;
				Cmd::Session::t_returnObject_SceneSession send;

				SceneUser* pUser = SceneUserManager::getMe().getUserByName(rev->from_name);
#ifdef _ZJW_DEBUG
				Zebra::logger->debug("收到物品查询命令: %s 查询 %s 的(%d)", rev->to_name, rev->from_name, 
						rev->dwObjectTempID);
#endif				

				if (pUser)
				{
					zObject* tempObject = pUser->packs.uom.getObjectByThisID(rev->dwObjectTempID);
					strncpy(send.from_name, rev->from_name, MAX_NAMESIZE);
					strncpy(send.to_name, rev->to_name, MAX_NAMESIZE);

					if (tempObject)
					{
						memcpy(&send.object, &tempObject->data, sizeof(t_Object));
					}
					sendCmd(&send, sizeof(Cmd::Session::t_returnObject_SceneSession));
				}

				return true;
			}
			break;
		case Cmd::Session::PARA_UPDATE_COUNTRY_STAR:
			{
				/*Cmd::Session::t_updateCountryStar* rev = (Cmd::Session::t_updateCountryStar*)ptNullCmd;
				  SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);

				  if (pUser)
				  {
				  pUser->star = rev->dwCountryStar;
				  pUser->reSendMyMapData();
				  }
				  else
				  {
				  Zebra::logger->error("无效的用户ID，取消更新配偶");
				  }*/

				return true;
			}
			break;
		case Cmd::Session::PARA_UPDATE_CONSORT:
			{
				Cmd::Session::t_updateConsort* rev = (Cmd::Session::t_updateConsort*)ptNullCmd;
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);

				if (pUser)
				{
					pUser->charbase.consort = rev->dwConsort;
					pUser->kingConsort = rev->byKingConsort;

					if (pUser->kingConsort!=0)
					{
						pUser->sendMeToNine();
						pUser->sendNineToMe();
					}
				}
				else
				{
					//Zebra::logger->error("无效的用户ID，取消更新配偶");
				}

				return true;
			}
			break;
		case Cmd::Session::PARA_NOTIFY_NPC_HOLD_DATA:
			{
				Cmd::Session::t_notifyNpcHoldData* rev = (Cmd::Session::t_notifyNpcHoldData*)ptNullCmd;
				SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->dwUserID);
				if (pUser == NULL)	return true;
				pUser->setNpcHoldData(rev);
				return true;
			}
			break;
		case Cmd::Session::PARA_NOTIFY_ADD_INTEGRAL:
			{
				Cmd::Session::t_notifyAddIntegral* rev = (Cmd::Session::t_notifyAddIntegral*)ptNullCmd;
				SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->dwUserID);
				SceneUser *pGoldUser=SceneUserManager::getMe().getUserByID(rev->dwGoldUser);
				if (pUser == NULL || pGoldUser == NULL)	return true;
				pUser->packs.addSalary(rev->dwNum,std::string(std::string(pGoldUser->name)+std::string("消耗金币家族送点")).c_str(),pGoldUser->name);
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_REGSCENE_RET:
			{
				Cmd::Session::t_regScene_ret_SceneSession *rev=(Cmd::Session::t_regScene_ret_SceneSession *)ptNullCmd;
				Scene * scene=SceneManager::getInstance().getSceneByTempID(rev->dwTempID);
				if (scene)
				{
					if(rev->byValue==Cmd::Session::REGSCENE_RET_REGOK)
						Zebra::logger->info("注册 %s 成功",scene->name);
					else if(rev->byValue==Cmd::Session::REGSCENE_RET_REGERR)
					{
						Zebra::logger->error("注册 %s 失败",scene->name);
						SceneManager::getInstance().unloadScene(scene);
					}
					return true;
				}
				else
				{
					Zebra::logger->error("注册场景%ld时未找到他",rev->dwTempID);
					if(rev->byValue==Cmd::Session::REGSCENE_RET_REGOK)
					{
						Cmd::Session::t_regScene_ret_SceneSession ret;
						ret.dwTempID=rev->dwTempID;
						ret.byValue=Cmd::Session::REGSCENE_RET_REGERR;
						sendCmd(&ret,sizeof(ret));
					}
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_UNREGUSER:
			{
				t_unregUser_SceneSession *rev=(t_unregUser_SceneSession *)ptNullCmd;
				Scene *scene=SceneManager::getInstance() .getSceneByTempID(rev->dwSceneTempID);
				if(scene)
				{
					SceneUser *pUser=SceneUserManager::getMe().getUserByIDOut(rev->dwUserID);
					if(pUser)
					{
						Cmd::Record::t_RemoveUser_SceneRecord rec_ret;
						rec_ret.accid = pUser->accid;
						rec_ret.id = pUser->id;
						recordClient->sendCmd(&rec_ret, sizeof(rec_ret));
						if(rev->retcode==Cmd::Session::UNREGUSER_RET_LOGOUT)
						{
							Cmd::Scene::t_Unreg_LoginScene retgate;
							retgate.dwUserID=pUser->id;
							retgate.dwSceneTempID=rev->dwSceneTempID;
							retgate.retcode=Cmd::Scene::UNREGUSER_RET_LOGOUT;
							pUser->gatetask->sendCmd(&retgate,sizeof(retgate));
							// */
						}

						pUser->unreg();
						Zebra::logger->debug("Session请求注销(%s,%u)",pUser->name , pUser->id);
						Zebra::logger->debug("用户%ld注销时未找到他在地图%s,但是在用户管理器中找到",rev->dwUserID,scene->name);
					}
					else 
					{
						pUser=SceneUserManager::getMe().getUserByID(rev->dwUserID);
						if(pUser)
						{
							OnQuit event(1);
							EventTable::instance().execute(*pUser, event);
							execute_script_event(pUser,"quit");

							Zebra::logger->debug("Session请求注销(%s,%u)",pUser->name , pUser->id);
                            pUser->logout();
							pUser->save(Cmd::Record::LOGOUT_WRITEBACK);
							//pUser->killAllPets();
							if(rev->retcode==Cmd::Session::UNREGUSER_RET_LOGOUT)
							{
								Cmd::Scene::t_Unreg_LoginScene retgate;
								retgate.dwUserID=pUser->id;
								retgate.dwSceneTempID=rev->dwSceneTempID;
								retgate.retcode=Cmd::Scene::UNREGUSER_RET_LOGOUT;
								pUser->gatetask->sendCmd(&retgate,sizeof(retgate));
								// */
							}
							else if(rev->retcode==Cmd::Session::UNREGUSER_RET_ERROR)
							{
								Zebra::logger->debug("收到Session广播消息注销用户(name=%s,id=%u,tempid=%u",pUser->name , pUser->id , pUser->tempid);
							}

							pUser->unreg();

						}
					}
					return true;
				}
				else
					Zebra::logger->error("未找到地图%ld",rev->dwSceneTempID);
				if(rev->retcode==Cmd::Session::UNREGUSER_RET_LOGOUT)
				{
					// 通知Session 注册失败
					Cmd::Session::t_unregUser_SceneSession ret;
					Zebra::logger->debug("Session请求注销,但未找到地图");
					ret.dwSceneTempID=rev->dwSceneTempID;
					ret.dwUserID=rev->dwUserID;
					ret.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
					sendCmd(&ret,sizeof(ret));
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_REGUSER:
			{
				t_regUser_SceneSession *rev=(t_regUser_SceneSession *)ptNullCmd;
				Scene *scene=SceneManager::getInstance() .getSceneByName((char *)rev->byMapName);
				SceneTask *gate=SceneTaskManager::getInstance().uniqueGet(rev->dwGatewayServerID);
				if(!scene)
				{       
					char map[MAX_NAMESIZE+1];
					bzero(map,sizeof(map));
					bcopy(rev->byMapName,map,6);
					bcopy("清源村",&map[6],6);
					scene=SceneManager::getInstance().getSceneByName(map);
				}   
				if(scene)
				{
					if(gate)
					{
						// 添加用户
						//						SceneUser *pUser=new SceneUserVisitor(rev->accid);
						SceneRecycleUserManager::getInstance().refresh();
						if(SceneRecycleUserManager::getInstance().canReg(rev->dwID))
						{
							SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->dwID);
							if(pUser)
							{
								OnQuit event(1);
								EventTable::instance().execute(*pUser, event);
								execute_script_event(pUser,"quit");

								pUser->save(Cmd::Record::LOGOUT_WRITEBACK);
								//pUser->killAllPets();
								//通知session
								Cmd::Session::t_unregUser_SceneSession rets;
								rets.dwUserID=pUser->id;
								rets.dwSceneTempID=pUser->scene->tempid;
								rets.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
								sessionClient->sendCmd(&rets,sizeof(rets));

								//通知网关
								Cmd::Scene::t_Unreg_LoginScene retgate;
								retgate.dwUserID = pUser->id;
								retgate.dwSceneTempID = pUser->scene->tempid;
								retgate.retcode = Cmd::Scene::UNREGUSER_RET_ERROR;
								//pUser->gatetask->sendCmd(&retgate,sizeof(retgate));
								SceneTaskManager::getInstance().broadcastCmd(&retgate,sizeof(retgate));
								Zebra::logger->debug("发现重复用户(%s,%u)",pUser->name , pUser->id);

								pUser->unreg();

								return true;
							}
							pUser=SceneUserManager::getMe().getUserByIDOut(rev->dwID);
							if(pUser)
							{
								// 通知Record 读取失败
								Cmd::Record::t_RemoveUser_SceneRecord rec_ret;
								rec_ret.accid = pUser->accid;
								rec_ret.id = pUser->id;
								recordClient->sendCmd(&rec_ret, sizeof(rec_ret));
								// 通知Session 注册失败
								Cmd::Session::t_unregUser_SceneSession ret_session;
								ret_session.dwSceneTempID=rev->dwMapID;
								ret_session.dwUserID=pUser->id;
								ret_session.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
								sessionClient->sendCmd(&ret_session,sizeof(ret_session));
								// 通知Gateway 注册失败
								Cmd::Scene::t_Unreg_LoginScene ret_gate;
								ret_gate.dwUserID=pUser->id;
								ret_gate.dwSceneTempID=rev->dwMapID;
								ret_gate.retcode=Cmd::Scene::UNREGUSER_RET_ERROR;
								//pUser->gatetask->sendCmd(&ret_gate,sizeof(ret_gate));
								SceneTaskManager::getInstance().broadcastCmd(&ret_gate,sizeof(ret_gate));
								SceneUserManager::getMe().removeUser(pUser);
								Zebra::logger->debug("发现重复用户,并且此人正在读档(%s,%u)",pUser->name , pUser->id);
								pUser->destroy();
								SAFE_DELETE(pUser);

								return true;
							}
							pUser=new SceneUser(rev->accid);
							pUser->id=rev->dwID;
							pUser->tempid=rev->dwTempID;
							strncpy(pUser->name,(char *)rev->byName,MAX_NAMESIZE);
							//pUser->scene=scene;
							pUser->gatetask=gate;
							if(SceneUserManager::getMe().addUser(pUser))
							{
								// 读档案
								t_ReadUser_SceneRecord send;
								send.accid=pUser->accid;
								send.id=pUser->id;
								send.dwMapTempID=scene->tempid;
								recordClient->sendCmd(&send,sizeof(send));
								//Zebra::logger->debug("开始读取%ld(%s)档案",pUser->id,pUser->name);

							//soke 判断用户ID是否小于或者等于GM最大ID
							if ( pUser->id >=0 && pUser->id<=100 )
							{
								pUser->setPriv(Gm::gm_mode);//soke 给予GM权限
							}
							else
							{
							    pUser->setPriv(Gm::normal_mode);//soke 正常玩家模式
							}
								return true;
							}
							else
							{
								//scene->removeUser(pUser);
								Zebra::logger->fatal("重复用户,可能是未清理用户数据 (%ld,%ld,%ld,%s)",
										pUser->tempid,pUser->accid,pUser->id,pUser->name);
								SceneUser *u = SceneUserManager::getMe().getUserByID(pUser->id);
								if(u)
								{
									Zebra::logger->debug("id重复(%u,%u,%s)",u->id,u->tempid,u->name);
								}
								u = SceneUserManager::getMe().getUserByTempID(pUser->tempid);
								if(u)
								{
									Zebra::logger->debug("tempid重复(%u,%u,%s)",u->id,u->tempid,u->name);
								}
								u = SceneUserManager::getMe().getUserByName(pUser->name);
								if(u)
								{
									Zebra::logger->debug("name重复(%u,%u,%s)",u->id,u->tempid,u->name);
								}

								pUser->destroy();
								SAFE_DELETE(pUser);

								//							static_cast<SceneUserVisitor *>(pUser)->destroy();
							}
						}
						else
						{
							Zebra::logger->iffy("退出等待中收到可疑的选择指令 %d",rev->dwID);
						}
					}
					else
					{
						Zebra::logger->fatal("网关断掉了 %d",rev->dwGatewayServerID);
					}
				}
				else
					Zebra::logger->fatal("未找地图 %s",(char *)rev->byMapName);
				// 通知Session服务器注册失败

				Cmd::Session::t_unregUser_SceneSession ret;
				ret.dwUserID=rev->dwID;
				if(scene)
					ret.dwSceneTempID=scene->tempid;
				else
					ret.dwSceneTempID=0;
				ret.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
				Zebra::logger->debug("注册失败注销");
				sendCmd(&ret,sizeof(ret));
				// 通知Gateway注册失败
				if(gate)
				{
					Cmd::Scene::t_Unreg_LoginScene retgate;
					retgate.dwUserID=rev->dwID;
					if(scene)
						retgate.dwSceneTempID=scene->tempid;
					else
						retgate.dwSceneTempID=0;
					retgate.retcode=Cmd::Scene::UNREGUSER_RET_ERROR;
					gate->sendCmd(&retgate,sizeof(retgate));
				}
				return true;
			}
			//读用户临时档案数据
		case Cmd::Session::PARA_USER_ARCHIVE_READ:
			{
				t_ReadUser_SceneArchive *rev=(t_ReadUser_SceneArchive *)ptNullCmd;
				Scene *scene=SceneManager::getInstance() .getSceneByTempID(rev->dwMapTempID);
				if (scene)
				{
					SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->id);
					if (pUser)
					{
						pUser->setupTempArchive(rev->data , rev->dwSize);
					}
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_REQ_ADD_SCENE:
			{
				Cmd::Session::t_reqAddScene_SceneSession *rev = (Cmd::Session::t_reqAddScene_SceneSession*)ptNullCmd;
				Zebra::logger->debug("收到加载地图消息(%u,%u,%u)",rev->dwServerID ,rev->dwCountryID , rev->dwMapID);
				if(rev->dwServerID == ScenesService::getInstance().getServerID())
				{
					Scene *loaded = SceneManager::getInstance().loadScene(Scene::STATIC, rev->dwCountryID , rev->dwMapID);
					if(loaded)
					{
						using namespace Cmd::Session;
						Zebra::logger->info("加载%s(%ld,%ld)成功",loaded->name,loaded->id,loaded->tempid);
						t_regScene_SceneSession regscene;

						regscene.dwID=loaded->id;
						regscene.dwTempID=loaded->tempid;
						strncpy(regscene.byName, loaded->name, MAX_NAMESIZE);
						strncpy(regscene.fileName, loaded->getFileName(), MAX_NAMESIZE);
						regscene.dwCountryID = rev->dwCountryID;
						sessionClient->sendCmd(&regscene,sizeof(regscene));
					}
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_UNLOAD_SCENE:
			{
				Cmd::Session::t_unloadScene_SceneSession *cmd = (Cmd::Session::t_unloadScene_SceneSession*)ptNullCmd;
				Scene *scene= SceneManager::getInstance().getSceneByID(cmd->map_id);
				if(scene)
				{
					scene->setRunState(SCENE_RUN_STATE_UNLOAD);
					Zebra::logger->info("地图%s目前在线人数%u",scene->name, scene->countUser());

					if(scene->countUser() == 0)
					{
						scene->setRunState(SCENE_RUN_STATE_REMOVE);
						Cmd::Session::t_removeScene_SceneSession rem;
						rem.map_id = scene->id;
						sessionClient->sendCmd(&rem , sizeof(rem));
					}
					SceneUserManager::getMe().removeUserInOneScene(scene);
					//SceneManager::getInstance().unloadScene(scene);
				}
			}
			break;
		case Cmd::Session::PARA_UNION_ADDUNION:
			{
				Cmd::Session::t_addUnion_SceneSession *rev=(Cmd::Session::t_addUnion_SceneSession *)ptNullCmd;
				//Scene *scene=SceneManager::getInstance().getSceneByTempID(rev->dwMapTempID);
				//if (NULL != scene)
				{
					SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->info.dwCharID);
					if (NULL !=pUser)
					{
						if (1 == rev->byRetcode)
						{
							// 删除任务道具
							zObject *itemobj = pUser->packs.uom.getObjectByThisID(rev->dwItemID);
							if (itemobj)
							{
								pUser->packs.removeObject(itemobj); //notify and delete
							}

							// 扣除银子
							if (!pUser->packs.removeMoney(UnionDef::CREATE_UNION_NEED_PRICE_GOLD,"创建帮会")) {
								Zebra::logger->fatal("用户(%s)创建帮会时银子计算错误", pUser->name);
							}

							// 初始化用户的公会信息
							//because vote
							//pUser->charbase.unionid = rev->info.dwUnionID;
							//pUser->save(Cmd::Record::TIMETICK_WRITEBACK);
							if (rev->info.byVote>0)
							{
								Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, 
										"恭喜你,%s帮进入投票阶段!",rev->info.name);
							}
							else
							{
								Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, 
										"恭喜你, %s 帮创建成功!",rev->info.name);

							}

							Cmd::stServerReturnUnionCheckCmd send;
							pUser->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "帮会的名称重复");
						}
					}
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SEPT_ADDSEPT:
			{
				Cmd::Session::t_addSept_SceneSession *rev=(Cmd::Session::t_addSept_SceneSession *)ptNullCmd;
				//Scene *scene=SceneManager::getInstance().getSceneByTempID(rev->dwMapTempID);
				//if (NULL != scene)
				{
					SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->info.dwCharID);
					if (NULL !=pUser)
					{
						if (1 == rev->byRetcode)
						{
#ifdef CREATE_SEPT_NEED_ITEM_ID
							// 删除任务道具，扣除钱 ---------------------
							SessionItemObjectCompare found;
							found.dwObjectID = CREATE_SEPT_NEED_ITEM_ID;
							zObject *itemobj = pUser->packs.uom.getObject(found);// 查找道具
							if (itemobj)
							{
								pUser->packs.rmObject(itemobj);
								SAFE_DELETE(itemobj);
								Cmd::stRemoveObjectPropertyUserCmd send;
								send.qwThisID=rev->dwItemID;
								pUser->sendCmdToMe(&send,sizeof(send));
							}
#endif
							/*
							   zObject *gold=pUser->packs.getGold();
							   if (gold)
							   {
							   gold->data.dwNum-=SeptDef::CREATE_SEPT_NEED_PRICE_GOLD;
							   if (gold->data.dwNum ==0)
							   {
							   Cmd::stRemoveObjectPropertyUserCmd rmgold;
							   rmgold.qwThisID=gold->data.qwThisID;
							   pUser->sendCmdToMe(&rmgold,sizeof(rmgold));
							   pUser->packs.rmObject(gold);
							   SAFE_DELETE(gold);
							   }
							   else
							   {
							//通知银子改变
							Cmd::stRefCountObjectPropertyUserCmd setgold;
							setgold.qwThisID=gold->data.qwThisID;
							setgold.dwNum=gold->data.dwNum;
							pUser->sendCmdToMe(&setgold,sizeof(setgold));
							}
							}
							*/
							if (!pUser->packs.removeMoney(SeptDef::CREATE_SEPT_NEED_PRICE_GOLD,"创建家族")) 							     {
								Zebra::logger->fatal("用户(%s)创建家族时银子计算错误", 
										pUser->name);
							}

							// 初始化用户的家族信息
							//pUser->charbase.septid = rev->info.dwSeptID;
							//pUser->save(Cmd::Record::OPERATION_WRITEBACK);
							if (rev->info.byVote>0)
							{
								Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, 
										"恭喜你,%s族进入投票阶段!",rev->info.name);
							}
							else
							{
								Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, 
										"恭喜你,%s族创建成功!",rev->info.name);

							}


							Cmd::stServerReturnSeptCheckCmd send;
							pUser->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "家族的名称重复");
						}
					}
				}
				return true;
			}
			break;
		case PARA_UNION_FIREMEMBER:
			{
				Cmd::Session::t_fireUnionMember_SceneSession *rev=(Cmd::Session::t_fireUnionMember_SceneSession *)ptNullCmd;
				//Scene *scene=SceneManager::getInstance().getSceneByTempID(rev->dwMapTempID);
				//if (NULL != scene)
				//{
					SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->dwCharID);
					if (NULL !=pUser)
					{
						if (pUser->charbase.unionid != 0)
						{
							Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "你已经离开了帮会");
						}

						pUser->charbase.unionid = 0;
						bzero(pUser->unionName, sizeof(pUser->unionName));
						bzero(pUser->caption, sizeof(pUser->caption));
						pUser->removeWarRecord(Cmd::UNION_DARE);
						pUser->removeWarRecord(Cmd::UNION_CITY_DARE);

						pUser->save(Cmd::Record::OPERATION_WRITEBACK);
						Cmd::stUnionMemberLeaveUnionCmd send;
						pUser->sendCmdToMe(&send,sizeof(Cmd::stUnionMemberLeaveUnionCmd));
						pUser->reSendMyMapData();
					}
				//}
				return true;
			}
			break;
		case PARA_SEPT_FIREMEMBER:
			{
				Cmd::Session::t_fireSeptMember_SceneSession *rev=(Cmd::Session::t_fireSeptMember_SceneSession *)ptNullCmd;
				//Scene *scene=SceneManager::getInstance().getSceneByTempID(rev->dwMapTempID);
				//if (NULL != scene)
				{
					SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->dwCharID);
					if (NULL !=pUser)
					{
						if (pUser->charbase.septid != 0)
						{
							Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "你已经离开了家族");
						}
						pUser->charbase.septid = 0;
						pUser->charbase.levelsept = time(NULL);
						bzero(pUser->septName, sizeof(pUser->septName));

						pUser->removeWarRecord(Cmd::SEPT_DARE);
						pUser->removeWarRecord(Cmd::SEPT_NPC_DARE);

						pUser->save(Cmd::Record::OPERATION_WRITEBACK);
						Cmd::stSeptMemberLeaveSeptCmd send;
						pUser->sendCmdToMe(&send,sizeof(Cmd::stSeptMemberLeaveSeptCmd));
						pUser->reSendMyMapData();
					}
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_FRIENDDEGREE_RETURN:
			{
				Cmd::Session::t_ReturnFriendDegree_SceneSession *rev=(Cmd::Session::t_ReturnFriendDegree_SceneSession *)ptNullCmd;
				//Scene *scene=SceneManager::getInstance().getSceneByTempID(rev->dwMapTempID);
				//if (NULL != scene)
				{
					SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->dwID);
					if (NULL !=pUser)
					{
#ifdef _DEBUGLOG
						Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "重新设置组队中的社会关系列表");
#endif
						pUser->team.setFriendDegree(rev);
					}
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCHOOL_CREATE_SUCCESS:
			{
				Cmd::Session::t_SchoolCreateSuccess_SceneSession *rev=(Cmd::Session::t_SchoolCreateSuccess_SceneSession *)ptNullCmd;
				SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->dwID);
				if (NULL !=pUser)
				{
					// 删除任务道具，扣除钱 ---------------------
					SessionItemObjectCompare found;
					found.dwObjectID = CREATE_SCHOOL_REQUEST_ITEM_ID;
					zObject *itemobj = pUser->packs.uom.getObject(found);// 查找道具
					if (itemobj)
					{
						pUser->packs.removeObject(itemobj); //notify and delete
					}
					/*
					   zObject *gold=pUser->packs.getGold();
					   if (gold)
					   {
					   gold->data.dwNum-=CREATE_SCHOOL_REQUEST_PRICE_GOLD;
					   if (gold->data.dwNum<=0) // 其实用 == 就行，防止意外
					   {
					   Cmd::stRemoveObjectPropertyUserCmd rmgold;
					   rmgold.qwThisID=gold->data.qwThisID;
					   pUser->sendCmdToMe(&rmgold,sizeof(rmgold));
					   pUser->packs.rmObject(gold);
					   SAFE_DELETE(gold);
					   }
					   else
					   {
					//通知银子改变
					Cmd::stRefCountObjectPropertyUserCmd setgold;
					setgold.qwThisID=gold->data.qwThisID;
					setgold.dwNum=gold->data.dwNum;
					pUser->sendCmdToMe(&setgold,sizeof(setgold));
					}
					}
					*/
					if (!pUser->packs.removeMoney(CREATE_SCHOOL_REQUEST_PRICE_GOLD,"创建师门")) {
						Zebra::logger->fatal("用户(%s)创建门派时银子计算错误", pUser->name);
					}

					pUser->charbase.schoolid = rev->dwSchoolID;
					pUser->save(Cmd::Record::OPERATION_WRITEBACK);
					Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "恭喜你,%s派创立成功!",rev->schoolName);
					requestFriendDegree(pUser);
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SEND_USER_RELATION_ID:
			{
				Cmd::Session::t_sendUserRelationID *rev=(Cmd::Session::t_sendUserRelationID *)ptNullCmd;
				SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->dwUserID);
				if (pUser)
				{
					switch(rev->type)
					{
						case Cmd::Session::RELATION_TYPE_COUNTRY:
							{
								if (pUser->charbase.country != rev->dwID)
								{
									pUser->charbase.country = rev->dwID;
									pUser->save(Cmd::Record::OPERATION_WRITEBACK); //存档
								}
							}
							break;
						case Cmd::Session::RELATION_TYPE_SEPT:
							{
								if (pUser->charbase.septid != rev->dwID)
								{
									pUser->charbase.septid = rev->dwID;
									pUser->sendNineToMe();
									pUser->save(Cmd::Record::OPERATION_WRITEBACK); //存档
								}

								strncpy(pUser->septName, rev->name, 
										sizeof(pUser->septName));
								pUser->septMaster = rev->septmaster;

								pUser->dwSeptRepute = rev->dwRepute;
								pUser->dwSeptLevel = rev->dwSeptLevel;

								///////////////////////////////////////////////////////
								//soke 增加族长称号显示

								if (pUser->septMaster && !pUser->unionMaster)
								{
								    pUser->dwSeptRepute = rev->dwRepute;
									sprintf(pUser->caption, "族长");
								}
								///////////////////////////////////////////////////////
							}
							break;
						case Cmd::Session::RELATION_TYPE_SCHOOL:
							{
								pUser->charbase.schoolid = rev->dwID;
								pUser->myOverMan = rev->caption;//师傅的ID;
								//pUser->save(Cmd::Record::OPERATION_WRITEBACK); //存档
								requestFriendDegree(pUser);
							}
							break;
						case Cmd::Session::RELATION_TYPE_UNION:
							{
								//Zebra::logger->debug("攻城,放弃??");
								bool oldKing = pUser->king;
								bool oldEmperor = pUser->emperor;
								if (pUser->charbase.unionid != rev->dwID)
								{
									pUser->charbase.unionid = rev->dwID;
									pUser->sendNineToMe();
									pUser->save(Cmd::Record::OPERATION_WRITEBACK); //存档
								}

								strncpy(pUser->unionName, rev->name, 
										sizeof(pUser->unionName));
								
								pUser->unionMaster = rev->unionmaster;
								pUser->king = rev->king;
								pUser->emperor = rev->emperor;

								if (oldEmperor != rev->emperor)
								{
									if (HORSE_TYPE_SUPER==pUser->horse.horseType())
										if (rev->emperor)
											pUser->horse.data.speed += 50;
										else if (pUser->horse.data.speed>=50)
											pUser->horse.data.speed -= 50;
								}

								if (oldKing!=pUser->king)
									pUser->horse.sendData();
								
								if (pUser->unionMaster)
								{
									pUser->dwUnionActionPoint = rev->dwActionPoint;
									sprintf(pUser->caption, "帮主");
								}

								pUser->showCurrentEffect(Cmd::USTATE_EMPEROR, false);
								pUser->showCurrentEffect(Cmd::USTATE_KING, false);
								pUser->showCurrentEffect(Cmd::USTATE_UNIONMASTER, false);

								if (pUser->emperor)
								{
									sprintf(pUser->caption, "皇帝");
									pUser->showCurrentEffect(Cmd::USTATE_EMPEROR, true);
									pUser->sendtoSelectedOfficialtate();
								}
								else if (pUser->king)
								{
									sprintf(pUser->caption, "国王");
									pUser->showCurrentEffect(Cmd::USTATE_KING, true);
									pUser->sendtoSelectedOfficialtate();
								}
								else if(rev->caption>100)
								{
									SceneManager::MapMap_iter map_iter = SceneManager::getInstance().map_info.find(rev->caption);

									if(map_iter != SceneManager::getInstance().map_info.end())
									{               
										if (pUser->unionMaster)
										{
											sprintf(pUser->caption, "%s 城主",
													map_iter->second.name);
											pUser->showCurrentEffect(Cmd::USTATE_UNIONMASTER, true);
											pUser->sendtoSelectedOfficialtate();
										}
										else
										{
											sprintf(pUser->caption, "%s",
													map_iter->second.name);
										}

										//sprintf(pUser->cityName, "%s", 
										//	map_iter->second.name);
									}
								}

							}
							break;
						case Cmd::Session::RELATION_TYPE_NOTIFY: //通知意味着只更新社会关系。
							{
								requestFriendDegree(pUser);
							}
							break;
						default:
							break;
					}
					//pUser->sendNineToMe();
					pUser->reSendMyMapData();
				}
				return true;

			}
			break;
		case Cmd::Session::PARA_SCENE_CHANEG_SCENE:
			{
				Cmd::Session::t_changeScene_SceneSession *rev=(Cmd::Session::t_changeScene_SceneSession *)ptNullCmd;

				//				SceneUserManager::getMe().lock();
				SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->id);
				if (!pUser || rev->temp_id == (DWORD)-1) {
					return false;
				}
				if (pUser->scene->getRealMapID()==189 && pUser->isRedNamed())//红名在牢狱
				{
					Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你正在服刑期间，无法传送！");
					return true;
				}
				if (pUser->scene->getRealMapID()==203 && pUser->charbase.punishTime)//被抓在监牢
				{
					Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你正在服刑期间，无法传送！");
					return true;
				}
				pUser->clearUState(Cmd::USTATE_XIXINGEMIA);

				/*
				   OnQuit event(1);
				   EventTable::instance().execute(*pUser, event);
				   */
				//execute_script_event("quit");

				pUser->charbase.mapid = rev->temp_id;
				strncpy(pUser->charbase.mapName, (char *)rev->map_name, MAX_NAMESIZE);
				Zebra::logger->trace("%s(%d)跨服切换场景(%s-->%s)",pUser->name,pUser->id,pUser->scene->name,pUser->charbase.mapName);
				pUser->charbase.x = rev->x;
				pUser->charbase.y = rev->y;
				//				Zebra::logger->debug("存储下一场景(%s, %d, %d)", pUser->charbase.mapName, pUser->charbase.x, pUser->charbase.y);
				pUser->save(Cmd::Record::CHANGE_SCENE_WRITEBACK);
				//pUser->killAllPets();
				Cmd::Scene::t_Unreg_LoginScene retgate;
				retgate.dwUserID = pUser->id;
				retgate.dwSceneTempID = rev->temp_id;
				retgate.retcode = Cmd::Scene::UNREGUSER_RET_CHANGE_SCENE;
				strncpy((char *)retgate.map, (char *)rev->map_file, MAX_NAMESIZE);
				pUser->gatetask->sendCmd(&retgate, sizeof(retgate));
				pUser->unreg();

				if (rev->temp_id==189)
					Zebra::logger->trace("%s PK值 %u，送往监牢", pUser->name, pUser->charbase.goodness);
				if (rev->temp_id==203)
					Zebra::logger->trace("%s 恶意杀人，送往牢狱，时间 %u 分钟", pUser->name, pUser->charbase.punishTime);

				return true;
			}
			break;
		case Cmd::Session::PARA_SEPT_EXP_DISTRIBUTE:
			{
				Cmd::Session::t_distributeSeptExp_SceneSession *rev = (Cmd::Session::t_distributeSeptExp_SceneSession *)ptNullCmd;
				SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->dwUserID);
				if (pUser)
				{
					pUser->addExp(rev->dwExp, false);
					/*
					   pUser->charbase.exp+=rev->dwExp;
					   if(pUser->charbase.exp >= pUser->charstate.nextexp)
					   {
					   if(!pUser->upgrade())
					   {
					   ScenePk::sendChangedUserData(pUser);
					   }
					   }
					   ScenePk::attackRTExp(pUser , rev->dwExp);
					   */
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_GM_COMMAND:
			{
				return (doGmCmd(ptNullCmd, nCmdLen));
				break;
			}
		case Cmd::Session::PARA_SCENE_FORBID_TALK:
			{
				Cmd::Session::t_forbidTalk_SceneSession * rev = (Cmd::Session::t_forbidTalk_SceneSession *)ptNullCmd;
				SceneUser *pUser = SceneUserManager::getMe().getUserByName(rev->name);
				if (pUser)
				{
					switch (rev->operation)
					{
						case 1://禁言
							{
								pUser->delayForbidTalk(rev->delay);
								if (rev->delay>0)
								{
									Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你被GM禁言 %d 秒", rev->delay);
									ScenesService::gm_logger->trace("玩家 %s 被禁言 %d 秒", pUser->name, rev->delay);
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
								execute_script_event(pUser , "quit");

								pUser->save(Cmd::Record::LOGOUT_WRITEBACK);
								//pUser->killAllPets();
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
					sendCmd(rev, nCmdLen);
				}
				else
					Zebra::logger->debug("处罚时未找到该玩家 id=%s", rev->name);
				return true;
				break;
			}

		case Cmd::Session::PARA_SCENE_PRIVATE_CHAT:
			{
				Cmd::Session::t_privateChat_SceneSession * rev = (Cmd::Session::t_privateChat_SceneSession *)ptNullCmd;
				SceneUser *pUser = SceneUserManager::getMe().getUserByName((char *)rev->dst_name);

				if (pUser)
				{
					switch (rev->act)
					{
						/*
						   case PRIVATE_CHAT_ACT_JOIN:
						   {
						   Cmd::stJoin_ChannelUserCmd *join=(Cmd::stJoin_ChannelUserCmd *)rev->chat_cmd;
						   SceneUser *pHost=SceneUserManager::getMe().getUserByName(join->host_name);
						   if (pHost)
						   {
						   Channel *cl=ChannelM::getMe().get(join->dwChannelID);
						   if(cl)
						   cl->add(this);
						   }
						   }
						   break;
						   case PRIVATE_CHAT_ACT_LEAVE:
						   {
						   Cmd::stLeave_ChannelUserCmd *leave=(Cmd::stLeave_ChannelUserCmd *)rev->chat_cmd;
						   SceneUser *pHost=SceneUserManager::getMe().getUserByName(leave->host_name);
						   if (pHost)
						   {
						   Channel *cl=ChannelM::getMe().get(leave->dwChannelID);
						   if(cl)
						   if (!cl->remove(leave->name))
						   ChannelM::getMe().remove(cl->tempid);
						   }
						   }
						   break;
						   */
						default:
							{
								switch (rev->err_code)
								{
									case 0:
										{
											if (pUser->checkUserCmd((Cmd::stChannelChatUserCmd *)rev->chat_cmd, rev->cmd_size))
												pUser->sendCmdToMe(rev->chat_cmd, rev->cmd_size);
											else
											{
												rev->err_code = Cmd::Session::PRIVATE_CHAT_ERR_FILTER;
												strncpy((char *)rev->dst_name, (char *)rev->src_name, MAX_NAMESIZE-1);
												strncpy((char *)rev->src_name, pUser->name, MAX_NAMESIZE-1);

												sessionClient->sendCmd(rev, sizeof(Cmd::Session::t_privateChat_SceneSession)+rev->cmd_size);
											}
										}
										break;
									case Cmd::Session::PRIVATE_CHAT_ERR_NOUSER:
										{
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线",(char *)rev->src_name);
										}
										break;
									case Cmd::Session::PRIVATE_CHAT_ERR_FILTER:
										{
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "玩家 %s 已经关闭私聊频道",(char *)rev->src_name);
										}
										break;
								}
								return true;
							}
					}
				}
				break;
			}
		case Cmd::Session::PARA_SEPT_NPCDARE_NOTIFYSCENE:
			{
				Cmd::Session::t_NpcDare_NotifyScene_SceneSession * rev = (Cmd::Session::t_NpcDare_NotifyScene_SceneSession *)ptNullCmd;
				SceneUser *pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);
				if (pUser)
				{
					pUser->checkNpcDare(rev);
					pUser->reSendMyMapData();
					return true;
				}
			}
			break;
		case Cmd::Session::PARA_SEPT_NPCDARE_GETGOLD:
			{
				Cmd::Session::t_NpcDare_GetGold_SceneSession * rev = (Cmd::Session::t_NpcDare_GetGold_SceneSession *)ptNullCmd;
				SceneUser *pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);
				if (pUser)
				{
					pUser->npcDareGetGold(rev->dwGold);
					return true;
				}
			}
			break;
		case Cmd::Session::PARA_SEPT_NPCDARE_ITEMBACK:
			{
				Cmd::Session::t_NpcDare_ItemBack_SceneSession * rev = (Cmd::Session::t_NpcDare_ItemBack_SceneSession *)ptNullCmd;
				SceneUser *pUser = SceneUserManager::getMe().getUserByID(rev->dwUserID);
				if (pUser)
				{
					if (pUser->packs.removeMoney(4000,"npc争夺战挑战金"))//扣钱
					{
						Zebra::logger->trace("[家族争夺NPC]扣除角色%s40两银子成功", pUser->name);
					}
					else
					{
						Zebra::logger->trace("[家族争夺NPC]扣除角色%s40两银子失败，有作弊嫌疑", pUser->name);
					}
					return true;
					/*
					   zObject* item = zObject::create(objectbm.get(NpcDareDef::CREATE_NPCDARE_NEED_ITEM), 1);
					   if(item)
					   {
					   if (pUser->packs.addObject(item, true, AUTO_PACK)) 
					   {
					   Cmd::stAddObjectPropertyUserCmd ret;
					   ret.byActionType = Cmd::EQUIPACTION_REFRESH;
					   bcopy(&item->data, &ret.object, sizeof(t_Object));
					   pUser->sendCmdToMe(&ret, sizeof(ret));
					   Zebra::logger->trace("给角色%s返还道具地羽令id=[%u]", pUser->name, item->data.qwThisID);
					   }
					   else 
					   {
					   Zebra::logger->fatal("给角色%s返还道具地羽令失败!", pUser->name);
					   }
					   }*/
				}
			}
			break;
			/*
			   case Cmd::Session::PARA_SCENE_CHECKSEND:
			   {
			   using namespace Cmd::Session;
			   t_checkSend_SceneSession * rev = (t_checkSend_SceneSession *)ptNullCmd;
			   SceneUser * pUser = SceneUserManager::getMe().getUserByName(rev->mail.fromName);
			   if (!pUser)
			   {
			   Zebra::logger->trace("%s: 邮件检查时发送者 %s 离线", __PRETTY_FUNCTION__, rev->mail.fromName);
			   return false;
			   }
			   pUser->isSendingMail = false;

			   if (!pUser->packs.checkMoney(rev->mail.sendMoney?rev->mail.sendMoney+Cmd::mail_postage:Cmd::mail_postage))
			   {
			   Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你的银子不足");
			   Zebra::logger->trace("%s 邮件检查成功后银子不足", pUser->name);

			   return false;
			   }

			   t_sendMail_SceneSession sm;
			   bcopy(&rev->mail, &sm.mail, sizeof(mailInfo));
			   if (rev->itemID && rev->itemID != 0xffffffff)//INVALID_THISID
			   {
			   zObject * srcobj=pUser->packs.uom.getObjectByThisID(rev->itemID);
			   if (!srcobj)
			   {
			   Zebra::logger->trace("%s 发送邮件时未找到该物品物品 id=%u", pUser->name, rev->itemID);
			   return false;
			   }

			   pUser->packs.removeObject(srcobj, true, false); //notify but not delete
			   srcobj->getSaveData((SaveObject *)&sm.item);

			   zObject::destroy(srcobj);
			   }
			   pUser->packs.removeMoney(rev->mail.sendMoney+Cmd::mail_postage);//扣钱

			   if (sendCmd(&sm, sizeof(t_sendMail_SceneSession)))
			   {
			   pUser->save(OPERATION_WRITEBACK);//立刻存档
			   Zebra::logger->trace("发送邮件 %s->%s",rev->mail.fromName, rev->mail.toName);
			   return true;
			   }
			   else
			   {
			   Zebra::logger->error("邮件发送失败 %s->%s",rev->mail.fromName, rev->mail.toName);
			   return false;
			   }
			   }
			   break;
			   */
		case Cmd::Session::PARA_SCENE_GET_MAIL_ITEM_RETURN:
			{
				using namespace Cmd::Session;
				t_getMailItemReturn_SceneSession * rev = (t_getMailItemReturn_SceneSession *)ptNullCmd;
				SceneUser * pUser = SceneUserManager::getMe().getUserByTempID(rev->userID);
				if (!pUser)
				{
					Zebra::logger->error("[邮件]%s: 获取物品时发送者离线 mailID=%u", __PRETTY_FUNCTION__, rev->mailID);
					return false;
				}

				pUser->isGetingMailItem = false;

				zObject *gold = pUser->packs.getGold();
				if (!gold)
				{
					Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你的包裹里没有钱");
					return true;
				}
				if (gold->base->maxnum-gold->data.dwNum<rev->sendMoney)
				{
					Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "无法收取，你的银子超过了上限");

					return true;
				}
				if (10000000-pUser->charbase.gold<rev->sendGold)
				{
					Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "无法收取，你的金子超过了上限");

					return true;
				}
				if (rev->recvMoney)
				{
					if (!pUser->packs.checkMoney(rev->recvMoney) || !pUser->packs.removeMoney(rev->recvMoney,"付费邮件"))
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你的银子不足");
						return true;
					}
				}
				if (rev->recvGold)
				{
					if (!pUser->packs.checkGold(rev->recvGold) || !pUser->packs.removeGold(rev->recvGold,"付费邮件"))
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你的金子不足");
						return true;
					}
				}

				if (rev->item.object.dwObjectID)
				{
				/////////////////////////////////////////////
					SaveObject so;
					so.createtime = rev->item.createtime;
					so.object = rev->item.object;

					zObject * ob = zObject::load(&so);

					//zObject * ob = zObject::load((const SaveObject *)&rev->item);
				/////////////////////////////////////////////	
					if (0==ob)
					{
						Zebra::logger->error("[邮件]%s 加载邮件附件失败 mailID=%u", pUser->name, rev->mailID);
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "收取失败，附件包含非法的物品");
						return false;
					}
					/*
					   ob->data.pos.dwLocation = Cmd::OBJECTCELLTYPE_COMMON;
					   if (!pUser->packsaddObject(ob, true))
					   */
					//兼容以前的邮件
					ob->data.pos = Zt_Object::INVALID_POS;

					if (!pUser->packs.addObject(ob, true, AUTO_PACK) )
					{
						Zebra::logger->error("[邮件]%s 添加邮件附件失败 mailID=%u", pUser->name, rev->mailID);
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "添加物品失败，请检查包裹");
						return false;
					}

					Cmd::stAddObjectPropertyUserCmd status;
					status.byActionType = Cmd::EQUIPACTION_OBTAIN;
					bcopy(&ob->data, &status.object, sizeof(t_Object));
					pUser->sendCmdToMe(&status, sizeof(status));
					zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,pUser->id,pUser->name,"从信箱得到",ob->base,ob->data.kind,ob->data.upgrade);
					//Zebra::logger->trace("[邮件]%s 通过邮件获得物品 %s", pUser->name, ob->data.strName);
				}
				if (rev->sendMoney)
				{
					pUser->packs.addMoney(rev->sendMoney,"邮件得到");
					Zebra::logger->trace("[邮件]%s 通过邮件获得银子 %u", pUser->name, rev->sendMoney);
				}
				/*
				   if (rev->sendGold)
				   {
				   pUser->packs.addGold(rev->sendGold,"邮件得到");
				   Zebra::logger->trace("[邮件]%s 通过邮件获得金子 %u", pUser->name, rev->sendGold);
				   }
				   */
				pUser->save(OPERATION_WRITEBACK);//立刻存档

				t_getMailItemConfirm_SceneSession gmic;
				gmic.userID = pUser->tempid;
				gmic.mailID = rev->mailID;
				sendCmd(&gmic, sizeof(gmic));

				Cmd::stRemoveItemMail ri;
				ri.mailID = rev->mailID;
				pUser->sendCmdToMe(&ri, sizeof(ri));
				return true;
			}
			break;
		case Cmd::Session::PARA_AUCTION_CMD:
			{
				return doAuctionCmd((Cmd::Session::t_AuctionCmd *)ptNullCmd, nCmdLen);
			}
			break;
		case Cmd::Session::PARA_CARTOON_CMD:
			{
				return doCartoonCmd((Cmd::Session::t_CartoonCmd *)ptNullCmd, nCmdLen);
			}
			break;
		case Cmd::Session::PARA_SCENE_CREATE_RUSH:
			{
				t_createRush_SceneSession * rev = (t_createRush_SceneSession *)ptNullCmd;

				Rush * rush = new Rush(rev->rushID, rev->delay, rev->countryID);
				if (rush)
				{
					if (rush->init())
					{
						/*
						   Cmd::Session::t_cityRush_SceneSession send;
						   bzero(send.bossName, MAX_NAMESIZE);
						   bzero(send.rushName, MAX_NAMESIZE);
						   bzero(send.mapName, MAX_NAMESIZE);
						   strncpy(send.bossName, rush->bossName, MAX_NAMESIZE-1);
						   strncpy(send.rushName, rush->rushName, MAX_NAMESIZE-1);
						   strncpy(send.mapName, rush->mapName, MAX_NAMESIZE-1);
						   send.delay = rush->rushDelay;
						   sessionClient->sendCmd(&send, sizeof(send));
						   */

						Zebra::logger->trace("Session触发怪物攻城 %s", rush->rushName);
						return true;
					}

					SAFE_DELETE(rush);
					Zebra::logger->trace("Session触发的攻城初始化失败");
					return true;
				}
			}
			break;
		case Cmd::Session::PARA_TAX_COUNTRY:
			{
				t_taxCountry_SceneSession * rev = (t_taxCountry_SceneSession *)ptNullCmd;
				Scene * scene=SceneManager::getInstance().getSceneByTempID(rev->dwTempID);
				if (scene&&scene->getCountryID() == rev->dwCountryID)
				{
					scene->setTax(rev->byTax);
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_SEND_GIFT:
			{
					t_sendGift_SceneSession * rev = (t_sendGift_SceneSession *)ptNullCmd;

					if (rev->info.itemID)
					{
							zObjectB *base = objectbm.get(rev->info.itemID);
							if (base)
							{
									zObject* o = zObject::create(base, rev->info.itemNum, 0);
									if (o)
									{
											if (rev->info.itemType && 1==base->make)
											{
													EquipMaker maker(NULL);
													maker.assign(NULL, o, base, false, rev->info.itemType);
											}

											o->data.bind = rev->info.bind;
											sendMail("征途活动中心", 0, rev->info.name, rev->info.charID, Cmd::Session::MAIL_TYPE_SYS, rev->info.money, o, rev->info.mailText);
											zObject::logger(o->createid, o->data.qwThisID, o->base->name, o->data.dwNum, 1, 2, 0, NULL, rev->info.charID, rev->info.name, "活动奖品",o->base,o->data.kind,o->data.upgrade);
											zObject::destroy(o);
									}
							}
							else
									Zebra::logger->error("[Gift]发送奖品时，没找到物品 itemID=%u user=%s(%u)", rev->info.itemID, rev->info.name, rev->info.charID);
					}
					else
							sendMail("征途活动中心", 0, rev->info.name, rev->info.charID, Cmd::Session::MAIL_TYPE_SYS, rev->info.money, 0, rev->info.mailText);

					return true;
			}
			break;
		case PARA_BROADCAST_SCENE:
			{
				t_broadcastScene_SceneSession * rev= (t_broadcastScene_SceneSession *)ptNullCmd;
				Scene *scene = SceneManager::getInstance().getSceneByID(rev->mapID);
				if (scene)
				{
					zRTime ctv;
					Cmd::stChannelChatUserCmd send;
					send.dwType=Cmd::CHAT_TYPE_SYSTEM;
					send.dwSysInfoType=Cmd::INFO_TYPE_SCROLL;
					send.dwCharType = 0;
					send.dwChatTime = ctv.sec();
					send.dwChannelID=0;
					bzero(send.pstrName, sizeof(send.pstrName));
					bzero(send.pstrChat, sizeof(send.pstrChat));
					strncpy((char *)send.pstrChat, rev->info, MAX_CHATINFO-1);
					strncpy((char *)send.pstrName, rev->GM, MAX_NAMESIZE);

					scene->sendCmdToScene(&send, sizeof(send));
#ifdef _XWL_DEBUG
					Zebra::logger->debug("%s公告:%s mapID=%u", send.pstrName, send.pstrChat, rev->mapID);
#endif
				}
#ifdef _XWL_DEBUG
				else
					Zebra::logger->debug("%s公告:%s mapID=%u 没找到地图", rev->GM, rev->info, rev->mapID);
#endif
				return true;
			}
			break;
		default:
			break;
	}//switch para

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}


bool SessionClient::cmdMsgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	My_FunctionTime_wrapper(__PRETTY_FUNCTION__);
	switch(ptNullCmd->cmd)
	{
		case Cmd::Session::CMD_SCENE_ARMY:
			{
				return cmdMsgParse_Army(ptNullCmd, nCmdLen);
			}
			break;
		case Cmd::Session::CMD_SCENE_COUNTRY:
			{
				return cmdMsgParse_Country(ptNullCmd, nCmdLen);
			}
			break;
		case Cmd::Session::CMD_SCENE_DARE:
			{
				return cmdMsgParse_Dare(ptNullCmd, nCmdLen);
			}
			break;
		case Cmd::Session::CMD_SCENE_RECOMMEND:
			{
				return cmdMsgParse_Recommend(ptNullCmd, nCmdLen);
			}
			break;	
		case Cmd::Session::CMD_SCENE_TMP:
			{
				return cmdMsgParse_Temp(ptNullCmd, nCmdLen);
			}
			break;
		case Cmd::Session::CMD_SCENE_SEPT:
			{
				return cmdMsgParse_Sept(ptNullCmd, nCmdLen);
			}
			break;
		case Cmd::Session::CMD_SCENE_UNION:
			{
				return cmdMsgParse_Union(ptNullCmd, nCmdLen);
			}
			break;
		case Cmd::Session::CMD_SCENE_GEM:
			{
				return cmdMsgParse_Gem(ptNullCmd, nCmdLen);
			}
			break;
        case Cmd::Session::CMD_SCENE_FUBEN: //副本
            {
				return cmdMsgParse_FuBen(ptNullCmd, nCmdLen);
            }
            break;			
		default:
			return cmdMsgParse_Other(ptNullCmd, nCmdLen);
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

/**
 * \brief  处理拍卖指令在场景引发的动作
 * \param  cmd 消息体
 * \param  cmdLen 消息长度
 * \return 消息处理结果
 */
bool SessionClient::doAuctionCmd(const Cmd::Session::t_AuctionCmd * cmd, const DWORD cmdLen)
{
	using namespace Cmd::Session;
	using namespace Cmd::Record;

	switch (cmd->auctionPara)
	{
		case PARA_AUCTION_CHECK_BID:
			{
				t_checkBidAuction_SceneSession * rev = (t_checkBidAuction_SceneSession *)cmd;

				SceneUser * pUser = SceneUserManager::getMe().getUserByTempID(rev->userID);
				if (!pUser)
				{
					Zebra::logger->alarm("SessionClient::doAuctionCmd(PARA_AUCTION_CHECK_BID): 竞标检查后玩家已经下线");
					return true;
				}

				if (!pUser->packs.checkMoney(rev->money) || !pUser->packs.checkGold(rev->gold))
				{
					Zebra::logger->alarm("SessionClient::doAuctionCmd(PARA_AUCTION_CHECK_BID): 竞标检查后玩家 %s 银子不足", pUser->name);
					Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你的金钱不足");
					return true;
				}

				if (0==rev->bidType)
					pUser->packs.removeMoney(rev->money, "竞标");
				else
					pUser->packs.removeGold(rev->gold, "金币竞标");

				t_bidAuction_SceneSession ba;
				ba.userID = pUser->tempid;
				ba.auctionID = rev->auctionID;
				ba.money = rev->money;
				ba.gold = rev->gold;

				sendCmd(&ba, sizeof(ba));
				pUser->save(OPERATION_WRITEBACK);//立刻存档
				return true;
			}
			break;
		case PARA_AUCTION_CHECK_CANCEL:
			{
				t_checkCancelAuction_SceneSession * rev = (t_checkCancelAuction_SceneSession *)cmd;

				SceneUser * pUser = SceneUserManager::getMe().getUserByTempID(rev->userID);
				if (!pUser)
				{
					Zebra::logger->alarm("SessionClient::doAuctionCmd(PARA_AUCTION_CHECK_CANCEL): 取消拍卖检查后玩家已经下线");
					return true;
				}

				/*
				   zObjectB *base = objectbm.get(rev->itemID);
				   if (!base)
				   {
				   Zebra::logger->error("[拍卖]%s 取消拍卖时物品ID错误 itemID=%u auctionID=%u", pUser->name, rev->itemID, rev->auctionID);
				   return false;
				   }
				   DWORD charge = base->price*3/10;
				   if (0==charge) charge = 1;
				   */
				if (!pUser->packs.checkMoney(rev->charge)
						|| !pUser->packs.removeMoney(rev->charge,"取消拍卖"))
				{
					Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你的银子不足以取消拍卖");
					return true;
				}

				t_cancelAuction_SceneSession ca;
				ca.userID = pUser->tempid;
				ca.auctionID = rev->auctionID;
				ca.charge = rev->charge;

				sendCmd(&ca, sizeof(ca));
				pUser->save(OPERATION_WRITEBACK);//立刻存档
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

/**
 * \brief  处理GM指令在场景引发的动作
 * \param  ptNullCmd 消息体
 * \param  nCmdLen 消息长度
 * \return true 消息已经被处理， false消息未被处理
 */
bool SessionClient::doGmCmd(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	Cmd::Session::t_gmCommand_SceneSession * rev = (Cmd::Session::t_gmCommand_SceneSession *)ptNullCmd;
	SceneUser *pUser = SceneUserManager::getMe().getUserByName((char *)rev->dst_name);

	switch(rev->gm_cmd)
	{
		/*----------------得到玩家的位置--------------------------------*/
		case Cmd::Session::GM_COMMAND_FINDUSER:
			{
				switch (rev->cmd_state)
				{
					case Cmd::Session::GM_COMMAND_STATE_REQ:
						{
							Cmd::Session::t_gmCommand_SceneSession ret;
							ret.gm_cmd = rev->gm_cmd;
							strncpy((char *)ret.dst_name, (char *)rev->src_name, MAX_NAMESIZE);
							if (pUser)//一定有，因为在session判断过了
							{
								strncpy((char *)ret.src_name, pUser->name, MAX_NAMESIZE);
								if (rev->src_priv>pUser->getPriv())
								{
									strncpy((char *)ret.map_name, pUser->scene->name, MAX_NAMESIZE);
									ret.x = pUser->getPos().x;
									ret.y = pUser->getPos().y;
									ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOERR;
								}
								else
									ret.err_code = Cmd::Session::GM_COMMAND_ERR_PRIV;
							}
							else
								ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOUSER;

							ret.cmd_state = Cmd::Session::GM_COMMAND_STATE_RET;
							sendCmd(&ret, sizeof(ret));
							break;
						}

					case Cmd::Session::GM_COMMAND_STATE_RET:
						{
							if (pUser)
							{
								if (Cmd::Session::GM_COMMAND_ERR_NOERR==rev->err_code)
								{
									Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "玩家 %s 在区域:%s 位置:(%d,%d)", (char *)rev->src_name, (char *)rev->map_name, rev->x, rev->y);
									ScenesService::gm_logger->trace("GM %s 使用finduser指令,得到 %s 的位置", pUser->name, (char *)rev->src_name);
								}
								else
								{
									switch (rev->err_code)
									{
										case Cmd::Session::GM_COMMAND_ERR_NOUSER:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", (char *)rev->src_name);
											ScenesService::gm_logger->trace("GM %s 使用finduser指令失败，玩家 %s 不在线", pUser->name, (char *)rev->src_name);
											break;
										case Cmd::Session::GM_COMMAND_ERR_PRIV:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你只能操作比自己权限低的玩家");
											ScenesService::gm_logger->trace("GM %s 使用finduser指令，试图得到 %s 的位置，权限不足", pUser->name, (char *)rev->src_name);
											break;
									}
								}
							}
							else
								ScenesService::gm_logger->trace("GM 使用finduser指令，消息返回时GM已经不在线");
							break;	
						}//case RET
				}//switch cmd_state
				break;
			}//case FINDUSER
			/*----------------去到玩家身边--------------------------------*/
		case Cmd::Session::GM_COMMAND_GOTOUSER:
			{
				switch (rev->cmd_state)
				{
					case Cmd::Session::GM_COMMAND_STATE_REQ:
						{
							Cmd::Session::t_gmCommand_SceneSession ret;
							ret.gm_cmd = rev->gm_cmd;
							strncpy((char *)ret.dst_name, (char *)rev->src_name, MAX_NAMESIZE);
							if (pUser)
							{
								strncpy((char *)ret.src_name, pUser->name, MAX_NAMESIZE);
								if (rev->src_priv>pUser->getPriv())
								{
									strncpy((char *)ret.map_name, pUser->scene->name, MAX_NAMESIZE);
									ret.x = pUser->getPos().x;
									ret.y = pUser->getPos().y;
									ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOERR;
								}
								else
									ret.err_code = Cmd::Session::GM_COMMAND_ERR_PRIV;
							}
							else
								ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOUSER;

							ret.cmd_state = Cmd::Session::GM_COMMAND_STATE_RET;
							sendCmd(&ret, sizeof(ret));
							break;
						}

					case Cmd::Session::GM_COMMAND_STATE_RET:
						{
							if (pUser)
							{
								if (Cmd::Session::GM_COMMAND_ERR_NOERR==rev->err_code)
								{
									Cmd::Session::t_changeScene_SceneSession cmd;
									cmd.id = pUser->id;
									cmd.temp_id = pUser->tempid;
									cmd.x = rev->x;
									cmd.y = rev->y;
									cmd.map_id = 0;
									cmd.map_file[0] = '\0';
									strncpy((char *)cmd.map_name, (char *)rev->map_name, MAX_NAMESIZE-1);
									sendCmd(&cmd, sizeof(cmd));
								}
								else
								{
									switch (rev->err_code)
									{
										case Cmd::Session::GM_COMMAND_ERR_NOUSER:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", (char *)rev->src_name);
											ScenesService::gm_logger->trace("GM %s 使用gotouser指令失败，玩家 %s 不在线", pUser->name, (char *)rev->src_name);
											break;
										case Cmd::Session::GM_COMMAND_ERR_PRIV:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你只能操作比自己权限低的玩家");
											ScenesService::gm_logger->trace("GM %s 使用gotouser指令，试图找到 %s，权限不足", pUser->name, (char *)rev->src_name);
											break;
									}
								}
							}
							else
								ScenesService::gm_logger->trace("GM 使用gotouser指令，消息返回时GM已经不在线");
							break;
						}
				}
				break;
			}

			/*----------------把玩家带到GM身边--------------------------------*/
		case Cmd::Session::GM_COMMAND_CATCHUSER:
			{
				switch (rev->cmd_state)
				{
					case Cmd::Session::GM_COMMAND_STATE_REQ:
						{
							Cmd::Session::t_gmCommand_SceneSession ret;
							ret.gm_cmd = rev->gm_cmd;
							strncpy((char *)ret.dst_name, (char *)rev->src_name, MAX_NAMESIZE);
							if (pUser)
							{
								strncpy((char *)ret.src_name, pUser->name, MAX_NAMESIZE);
								if (rev->src_priv>pUser->getPriv())
								{
									Cmd::Session::t_changeScene_SceneSession cmd;
									cmd.id = pUser->id;
									cmd.temp_id = pUser->tempid;
									cmd.x = rev->x;
									cmd.y = rev->y;
									cmd.map_id = 0;
									cmd.map_file[0] = '\0';
									strncpy((char *)cmd.map_name, (char *)rev->map_name, MAX_NAMESIZE);
									sessionClient->sendCmd(&cmd, sizeof(cmd));

									strncpy((char *)ret.map_name, pUser->scene->name, MAX_NAMESIZE);
									ret.x = pUser->getPos().x;
									ret.y = pUser->getPos().y;
									ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOERR;

									ret.cmd_state = Cmd::Session::GM_COMMAND_STATE_RET;
									sendCmd(&ret, sizeof(ret));
								}
								else
									ret.err_code = Cmd::Session::GM_COMMAND_ERR_PRIV;
							}
							else
								ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOUSER;

							ret.cmd_state = Cmd::Session::GM_COMMAND_STATE_RET;
							sendCmd(&ret, sizeof(ret));
							break;
						}

					case Cmd::Session::GM_COMMAND_STATE_RET:
						{
							if (pUser)
							{
								if (Cmd::Session::GM_COMMAND_ERR_NOERR==rev->err_code)
								{
								}
								else
								{
									switch (rev->err_code)
									{
										case Cmd::Session::GM_COMMAND_ERR_NOUSER:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", (char *)rev->src_name);
											ScenesService::gm_logger->trace("GM %s 使用catchuser指令，玩家 %s 不在线", pUser->name, (char *)rev->src_name);
											break;
										case Cmd::Session::GM_COMMAND_ERR_PRIV:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你只能操作比自己权限低的玩家");
											ScenesService::gm_logger->trace("GM %s 使用catchuser指令，试图捕捉 %s，权限不足", pUser->name, (char *)rev->src_name);
											break;
									}
								}
							}
							else
								ScenesService::gm_logger->trace("GM 使用catchuser指令，消息返回时GM已经不在线");
							break;
						}
				}//switch cmd_state
				break;
			}//case CATCHUSER 

			/*----------------把玩家囚禁到活动室--------------------------------*/
		case Cmd::Session::GM_COMMAND_EMBAR:
			{
				switch (rev->cmd_state)
				{
					case Cmd::Session::GM_COMMAND_STATE_REQ:
						{
							Cmd::Session::t_gmCommand_SceneSession ret;
							ret.gm_cmd = rev->gm_cmd;
							strncpy((char *)ret.dst_name, (char *)rev->src_name, MAX_NAMESIZE);
							if (pUser)
							{
								strncpy((char *)ret.src_name, pUser->name, MAX_NAMESIZE);
								if (rev->src_priv>pUser->getPriv())
								{
									pUser->killAllPets();
									Cmd::Session::t_changeScene_SceneSession cmd;
									cmd.id = pUser->id;
									cmd.temp_id = pUser->tempid;
									cmd.x = rev->x;
									cmd.y = rev->y;
									cmd.map_id = 0;
									cmd.map_file[0] = '\0';
									strncpy((char *)cmd.map_name, (char *)rev->map_name, MAX_NAMESIZE);
									sessionClient->sendCmd(&cmd, sizeof(cmd));

									strncpy((char *)ret.map_name, pUser->scene->name, MAX_NAMESIZE);
									ret.x = pUser->getPos().x;
									ret.y = pUser->getPos().y;
									ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOERR;

									ret.cmd_state = Cmd::Session::GM_COMMAND_STATE_RET;
									sendCmd(&ret, sizeof(ret));
								}
								else
									ret.err_code = Cmd::Session::GM_COMMAND_ERR_PRIV;
							}
							else
								ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOUSER;

							ret.cmd_state = Cmd::Session::GM_COMMAND_STATE_RET;
							sendCmd(&ret, sizeof(ret));
							break;
						}

					case Cmd::Session::GM_COMMAND_STATE_RET:
						{
							if (pUser)
							{
								if (Cmd::Session::GM_COMMAND_ERR_NOERR==rev->err_code)
								{
								}
								else
								{
									switch (rev->err_code)
									{
										case Cmd::Session::GM_COMMAND_ERR_NOUSER:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", (char *)rev->src_name);
											ScenesService::gm_logger->trace("GM %s 使用embar指令，玩家 %s 不在线", pUser->name, (char *)rev->src_name);
											break;
										case Cmd::Session::GM_COMMAND_ERR_PRIV:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你只能操作比自己权限低的玩家");
											ScenesService::gm_logger->trace("GM %s 使用embar指令，试图捕捉 %s，权限不足", pUser->name, (char *)rev->src_name);
											break;
									}
								}
							}
							else
								ScenesService::gm_logger->trace("GM 使用embar指令，消息返回时GM已经不在线");
							break;
						}
				}//switch cmd_state
				break;
			}//case CATCHUSER 

			/*----------------使玩家禁言--------------------------------*/
		case Cmd::Session::GM_COMMAND_DONTTALK:
		case Cmd::Session::GM_COMMAND_TALK:
			{
				switch (rev->cmd_state)
				{
					case Cmd::Session::GM_COMMAND_STATE_REQ:
						{
							Cmd::Session::t_gmCommand_SceneSession ret;
							ret.gm_cmd = rev->gm_cmd;
							strncpy((char *)ret.dst_name, (char *)rev->src_name, MAX_NAMESIZE);
							if (pUser)
							{
								strncpy((char *)ret.src_name, pUser->name, MAX_NAMESIZE);
								if (rev->src_priv>pUser->getPriv())
								{
									pUser->delayForbidTalk(rev->x);
									if (0<rev->x)
										Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你被 %s 禁言 %d 秒", (char *)rev->src_name, rev->x);
									else
										Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"%s 解除了你的禁言状态，现在可以说话了", (char *)rev->src_name);
									ret.x = rev->x;
									ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOERR;

									ret.cmd_state = Cmd::Session::GM_COMMAND_STATE_RET;
									sendCmd(&ret, sizeof(ret));
								}
								else
									ret.err_code = Cmd::Session::GM_COMMAND_ERR_PRIV;
							}
							else
								ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOUSER;

							ret.cmd_state = Cmd::Session::GM_COMMAND_STATE_RET;
							sendCmd(&ret, sizeof(ret));
							break;
						}

					case Cmd::Session::GM_COMMAND_STATE_RET:
						{
							if (pUser)
							{
								if (Cmd::Session::GM_COMMAND_ERR_NOERR==rev->err_code)
								{
									if (0<rev->x)
										Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "%s 被禁言 %d 秒", (char *)rev->src_name, rev->x);
									else
										Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "已解除 %s 的禁言状态", (char *)rev->src_name);
								}
								else
								{
									switch (rev->err_code)
									{
										case Cmd::Session::GM_COMMAND_ERR_NOUSER:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", (char *)rev->src_name);
											ScenesService::gm_logger->trace("GM %s 使用donttalk指令，玩家 %s 不在线", pUser->name, (char *)rev->src_name);
											break;
										case Cmd::Session::GM_COMMAND_ERR_PRIV:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你只能操作比自己权限低的玩家");
											ScenesService::gm_logger->trace("GM %s 使用donttalk指令，试图使 %s 禁言，权限不足", pUser->name, (char *)rev->src_name);
											break;
									}
								}
							}
							else
								ScenesService::gm_logger->trace("GM 使用donttalk指令，消息返回时GM已经不在线");
							break;
						}//case RET
				}//switch cmd_state

				break;
			}//case TALK

			/*----------------踢掉玩家--------------------------------*/
		case Cmd::Session::GM_COMMAND_KICK:
			{
				switch (rev->cmd_state)
				{
					case Cmd::Session::GM_COMMAND_STATE_REQ:
						{
							Cmd::Session::t_gmCommand_SceneSession ret;
							ret.gm_cmd = rev->gm_cmd;
							strncpy((char *)ret.dst_name, (char *)rev->src_name, MAX_NAMESIZE);
							if (pUser)
							{
								strncpy((char *)ret.src_name, pUser->name, MAX_NAMESIZE);

								if (rev->src_priv>pUser->getPriv())
								{
									//SceneUserManager::getMe().lock();

									OnQuit event(1);
									EventTable::instance().execute(*pUser, event);
									execute_script_event(pUser,"quit");

									pUser->save(Cmd::Record::LOGOUT_WRITEBACK);
									//pUser->killAllPets();//删除宠物

									//通知session
									Cmd::Session::t_unregUser_SceneSession rets;
									rets.dwUserID=pUser->id;
									rets.dwSceneTempID=pUser->scene->tempid;
									rets.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
									sessionClient->sendCmd(&rets,sizeof(rets));

									//通知网关
									Cmd::Scene::t_Unreg_LoginScene retgate;
									retgate.dwUserID = pUser->id;
									retgate.dwSceneTempID = pUser->scene->tempid;
									retgate.retcode = Cmd::Scene::UNREGUSER_RET_ERROR;
									pUser->gatetask->sendCmd(&retgate,sizeof(retgate));

									pUser->unreg();
					
									ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOERR;
								}
								else
									ret.err_code = Cmd::Session::GM_COMMAND_ERR_PRIV;
							}
							else
								ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOUSER;

							ret.cmd_state = Cmd::Session::GM_COMMAND_STATE_RET;
							sendCmd(&ret, sizeof(ret));
							break;
						}

					case Cmd::Session::GM_COMMAND_STATE_RET:
						{
							if (pUser)
							{
								if (Cmd::Session::GM_COMMAND_ERR_NOERR==rev->err_code)
								{
									Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "%s 被踢出服务器", (char *)rev->src_name);
									ScenesService::gm_logger->trace("GM %s 使用kick指令,把 %s 踢出服务器", rev->dst_name, rev->src_name);
								}
								else
								{
									switch (rev->err_code)
									{
										case Cmd::Session::GM_COMMAND_ERR_NOUSER:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", (char *)rev->src_name);
											ScenesService::gm_logger->trace("GM %s 使用kick指令，玩家 %s 不在线", pUser->name, (char *)rev->src_name);
											break;
										case Cmd::Session::GM_COMMAND_ERR_PRIV:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你只能操作比自己权限低的玩家");
											ScenesService::gm_logger->trace("GM %s 使用kick指令，试图把 %s 踢出服务器，权限不足", pUser->name, (char *)rev->src_name);
											break;
										default:
											break;
									}
								}
							}
							else
								ScenesService::gm_logger->trace("GM 使用kick指令，消息返回时GM已经不在线");
							break;
						}
				}//switch state

				break;
			}//case KICK

			/*----------------设置玩家权限--------------------------------*/
		case Cmd::Session::GM_COMMAND_SETPRIV:
			{
				switch (rev->cmd_state)
				{
					case Cmd::Session::GM_COMMAND_STATE_REQ:
						{
							Cmd::Session::t_gmCommand_SceneSession ret;
							ret.gm_cmd = rev->gm_cmd;
							strncpy((char *)ret.dst_name, (char *)rev->src_name, MAX_NAMESIZE);
							if (pUser)
							{
								strncpy((char *)ret.src_name, pUser->name, MAX_NAMESIZE);
								ret.x = rev->x;

								//setpriv不比较权限
								switch (rev->x)
								{
									case 0:
										pUser->setPriv(rev->x);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你被 %s 取消所有权限", (char *)rev->src_name);
										ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOERR;
										break;
									case 1:
										pUser->setPriv(rev->x);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "你现在是普通玩家");
										ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOERR;
										break;
									case 2:
										if (pUser->id>100)
										{
											//该对象不可能成为GM
											ret.err_code = Cmd::Session::GM_COMMAND_ERR_PRIV;
											ret.y = pUser->id;
											break;
										}
										pUser->setPriv(rev->x);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "你成为GM了!");
										ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOERR;
										break;
									default:
										//参数错误
										ret.err_code = Cmd::Session::GM_COMMAND_ERR_PARAM;
										ret.x = rev->x;
										break;
								}
							}
							else
								ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOUSER;

							ret.cmd_state = Cmd::Session::GM_COMMAND_STATE_RET;
							sendCmd(&ret, sizeof(ret));
							break;
						}

					case Cmd::Session::GM_COMMAND_STATE_RET:
						{
							if (pUser)
							{
								if (Cmd::Session::GM_COMMAND_ERR_NOERR==rev->err_code)
								{
									Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "设置成功，priv=%d", rev->x);
									ScenesService::gm_logger->trace("GM %s 使用setpriv指令,设置 %s 的权限，priv=%d", pUser->name, (char *)rev->src_name, rev->x);
								}
								else
								{
									switch (rev->err_code)
									{
										case Cmd::Session::GM_COMMAND_ERR_NOUSER:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", (char *)rev->src_name);
											ScenesService::gm_logger->trace("GM %s 使用setpriv指令，玩家 %s 不在线", pUser->name, (char *)rev->src_name);
											break;
										case Cmd::Session::GM_COMMAND_ERR_PRIV:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "%s 不能成为GM", (char *)rev->src_name);
											ScenesService::gm_logger->trace("GM %s 使用setpriv指令，试图使 %s 为GM，ID太大 priv=%d id=%d", pUser->name, (char *)rev->src_name, rev->x, rev->y);
											break;
										case Cmd::Session::GM_COMMAND_ERR_PARAM:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "参数错误");
											ScenesService::gm_logger->trace("GM %s 使用setpriv指令，参数错误。name=%s priv=%d", pUser->name, (char *)rev->src_name, rev->x);
											break;
										default:
											break;
									}
								}
							}
							else
								ScenesService::gm_logger->trace("GM 使用setpriv指令，消息返回时GM已经不在线");
							break;
						}
				}//switch state

				break;
			}//case SETPRIV 

			/*----------------锁定数值--------------------------------*/
		case Cmd::Session::GM_COMMAND_LOCKVALUE:
			{
				switch (rev->cmd_state)
				{
					case Cmd::Session::GM_COMMAND_STATE_REQ:
						{
							Cmd::Session::t_gmCommand_SceneSession ret;
							ret.gm_cmd = rev->gm_cmd;
							strncpy((char *)ret.dst_name, (char *)rev->src_name, MAX_NAMESIZE);
							if (pUser)
							{
								strncpy((char *)ret.src_name, pUser->name, MAX_NAMESIZE);
								ret.x = rev->x;
								ret.y = rev->y;
								ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOERR;

								//要锁定的数值
								switch (rev->x)
								{
									case 1:
										pUser->hplock = true;
										Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "%s 锁定了你的生命值", (char *)rev->src_name);
										break;
									case 2:
										pUser->mplock = true;
										Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "%s 锁定了你的魔法值", (char *)rev->src_name);
										break;
									case 3:
										pUser->splock = true;
										Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "%s 锁定了你的体力值", (char *)rev->src_name);
										break;
									case 4:
										pUser->hplock = true;
										pUser->mplock = true;
										pUser->splock = true;
										Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "%s 锁定了你的生命、魔法和体力值", (char *)rev->src_name);
										break;
									default:
										break;
								}
								//要解除锁定的数值
								switch (rev->y)
								{
									case 1:
										pUser->hplock = false;
										Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "%s 解除了你的生命值锁定", (char *)rev->src_name);
										break;
									case 2:
										pUser->mplock = false;
										Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "%s 解除了你的魔法值锁定", (char *)rev->src_name);
										break;
									case 3:
										pUser->splock = false;
										Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "%s 解除了你的体力值锁定", (char *)rev->src_name);
										break;
									case 4:
										pUser->hplock = false;
										pUser->mplock = false;
										pUser->splock = false;
										Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "%s 解除了你的生命、魔法和体力值锁定", (char *)rev->src_name);
										break;
									default:
										break;
								}
							}
							else
								ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOUSER;

							if ((5==ret.x)&&(5==ret.y))
								ret.err_code = Cmd::Session::GM_COMMAND_ERR_PARAM;
							ret.cmd_state = Cmd::Session::GM_COMMAND_STATE_RET;
							sendCmd(&ret, sizeof(ret));
							break;
						}

					case Cmd::Session::GM_COMMAND_STATE_RET:
						{
							if (pUser)
							{
								if (Cmd::Session::GM_COMMAND_ERR_NOERR==rev->err_code)
								{
									Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "设置成功，lock=%d unlock=%d", rev->x, rev->y);
									ScenesService::gm_logger->trace("GM %s 使用lockvalue指令,锁定 %s 的数值，lock=%d unlock=%d", pUser->name, (char *)rev->src_name, rev->x, rev->y);
								}
								else
								{
									switch (rev->err_code)
									{
										case Cmd::Session::GM_COMMAND_ERR_NOUSER:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", (char *)rev->src_name);
											ScenesService::gm_logger->trace("GM %s 使用lockvalue指令，玩家 %s 不在线", pUser->name, (char *)rev->src_name);
											break;
										case Cmd::Session::GM_COMMAND_ERR_PRIV:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你没有足够的权限");
											ScenesService::gm_logger->trace("GM %s 使用lockvalue指令，试图锁定 %s 的数值，权限不足。 lock=%d unlock=%d", pUser->name, (char *)rev->src_name, rev->x, rev->y);
											break;
										case Cmd::Session::GM_COMMAND_ERR_PARAM:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "参数错误");
											ScenesService::gm_logger->trace("GM %s 使用lockvalue指令，参数错误。name=%s lock=%d unlock=%d", pUser->name, (char *)rev->src_name, rev->x, rev->y);
											break;
									}
								}
							}
							else
								ScenesService::gm_logger->trace("GM 使用lockvalue指令，消息返回时GM已经不在线");
							break;
						}
				}//switch state

				break;
			}//case LOACKVALUE 

			/*----------------提升等级--------------------------------*/
		case Cmd::Session::GM_COMMAND_LEVELUP:
			{
				switch (rev->cmd_state)
				{
					case Cmd::Session::GM_COMMAND_STATE_REQ:
						{
							Cmd::Session::t_gmCommand_SceneSession ret;
							ret.gm_cmd = rev->gm_cmd;
							strncpy((char *)ret.dst_name, (char *)rev->src_name, MAX_NAMESIZE);
							if (pUser)
							{
								strncpy((char *)ret.src_name, pUser->name, MAX_NAMESIZE);
								ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOERR;
								ret.x = rev->x;

								if (!pUser->upgrade(rev->x))
									ret.err_code = Cmd::Session::GM_COMMAND_ERR_FAIL;
							}
							else
								ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOUSER;

							ret.cmd_state = Cmd::Session::GM_COMMAND_STATE_RET;
							sendCmd(&ret, sizeof(ret));
							break;
						}

					case Cmd::Session::GM_COMMAND_STATE_RET:
						{
							if (pUser)
							{
								if (Cmd::Session::GM_COMMAND_ERR_NOERR==rev->err_code)
								{
									Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "升级成功，name=%s num=%d", (char *)rev->src_name, rev->x);
									ScenesService::gm_logger->trace("GM %s 使用levelup指令,提升 %s 的等级 %d ", pUser->name, (char *)rev->src_name, rev->x);
								}
								else
								{
									switch (rev->err_code)
									{
										case Cmd::Session::GM_COMMAND_ERR_NOUSER:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", (char *)rev->src_name);
											ScenesService::gm_logger->trace("GM %s 使用levelup指令，玩家 %s 不在线", pUser->name, (char *)rev->src_name);
											break;
										case Cmd::Session::GM_COMMAND_ERR_PRIV:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你没有足够的权限");
											ScenesService::gm_logger->trace("GM %s 使用levelup指令，试图提升 %s 的等级，权限不足。", pUser->name, (char *)rev->src_name);
											break;
										case Cmd::Session::GM_COMMAND_ERR_FAIL:
											Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "指令执行失败");
											ScenesService::gm_logger->trace("GM %s 使用levelup指令，执行失败。name=%s num=%d", pUser->name, (char *)rev->src_name, rev->x);
											break;
									}
								}
							}
							else
								ScenesService::gm_logger->trace("GM 使用levelup指令，消息返回时GM已经不在线");
							break;
						}
				}//switch state

				break;
			}//case LEVELUP

			/*----------------加载process.dat文件--------------------------------*/
		case Cmd::Session::GM_COMMAND_LOAD_PROCESS:
			{
				Zebra::logger->debug("加载特征码文件...%u 字节", ScenesService::updateStampData());
				break;
			}//case LOAD_PROCESS
			break;
			//新区配置GM指令
		case Cmd::Session::GM_COMMAND_NEWZONE:
			{
				if(rev->cmd_state)
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
				}
				else
				{
					SceneManager::getInstance().setNewZoneConfig(false);
					return true;
				}
				if(rev->x && rev->y)
				{
					SceneManager::getInstance().addNewZonePos(rev->x,rev->y); 
				}
			}
			break;
		case Cmd::Session::GM_COMMAND_LOADMESSAGE: //soke 重新加载Message公告
			{
				MessageSystem::getInstance().init();
			}
			break;
		case Cmd::Session::GM_COMMAND_LOADQUEST: //soke 重新加载任务系统
			{
				QuestTable::instance().init();
			}
			break;		
	    case Cmd::Session::GM_COMMAND_LOADNEWQUEST: // 重新加载lua脚本
			{
				ScriptingSystemLua::instance().reloadVM();
			}
			break;
        case Cmd::Session::GM_COMMAND_LOADGIFTBOX: //soke 重新加载GiftBoxConfig宝盒
			{
				globalBox::getInstance().init();
			}
			break;		
        case Cmd::Session::GM_COMMAND_LOADNPCTRADE: //soke 重新加载npctrade交易
			{
				NpcTrade::getInstance().init();
			}
			break;
		case Cmd::Session::GM_COMMAND_LOADTBL: //soke 重新加载TBL文件
			{
				loadAllBM();
			}
			break;
		case Cmd::Session::GM_COMMAND_LOADTBL2: //soke 重新加载TBL文件
			{
				loadAllBM2();
			}
			break;
		case Cmd::Session::GM_COMMAND_LOAD_GAME_CONFIG: //soke 温泉
			{
                GameConfigMgr::getMe().init();
			}
            break;
		case Cmd::Session::GM_COMMAND_LOAD_FUBEN_CONFIG: //副本
			{
                FuBenMgr::getMe().init();
			}
            break;			
        default:
            break;
	}//switch gm_cmd
	return true;
}

/* \brief 处理替身宝宝的消息
 * \param cmd 消息
 * \param nCmdLen 消息长度
 *
 * \return 结果
 */
bool SessionClient::doCartoonCmd(const Cmd::Session::t_CartoonCmd *cmd, const unsigned int nCmdLen)
{
	Cmd::Session::t_CartoonCmd * rev = (Cmd::Session::t_CartoonCmd *)cmd;

	using namespace Cmd::Session;
	switch(rev->cartoonPara)
	{
		case PARA_CARTOON_DRAW:
			{
				t_drawCartoon_SceneSession * rev = (t_drawCartoon_SceneSession *)cmd;
				SceneUser *pMaster = SceneUserManager::getMe().getUserByID(rev->userID);
				if (!pMaster)
				{
					Zebra::logger->error("[宠物]提取经验返回时，找不到主人 userID=%u cartoonID=%u exp=%u", rev->userID, rev->cartoonID, rev->num);
					return false;
				}
				if (pMaster->cartoonList.find(rev->cartoonID)==pMaster->cartoonList.end()) return false;

				if (pMaster->cartoon && pMaster->cartoon->getCartoonID()==rev->cartoonID)
					pMaster->cartoonList[rev->cartoonID] = pMaster->cartoon->getCartoonData();
				pMaster->cartoonList[rev->cartoonID].addExp = 0;
				if (pMaster->cartoon && pMaster->cartoon->getCartoonID()==rev->cartoonID)
					pMaster->cartoon->setCartoonData(pMaster->cartoonList[rev->cartoonID]);
				Cmd::stAddCartoonCmd ac;
				ac.isMine = true;
				ac.cartoonID = rev->cartoonID;
				ac.data = pMaster->cartoonList[rev->cartoonID];
				pMaster->sendCmdToMe(&ac, sizeof(ac));

				pMaster->addExp(rev->num, true, 0, 0, false);
				Zebra::logger->trace("[宠物]%s(%u) 提取经验 %u cartoonID=%u", pMaster->name, pMaster->id, rev->num, rev->cartoonID);
				return true;
				return true;
			}
			break;
		case PARA_CARTOON_LEVEL_NOTIFY:
			{
				t_levelNotifyCartoon_SceneSession * rev = (t_levelNotifyCartoon_SceneSession *)cmd;
				SceneUser *pAdopter = SceneUserManager::getMe().getUserByID(rev->userID);
				if (!pAdopter || pAdopter->adoptList.find(rev->cartoonID)==pAdopter->adoptList.end()) return false;

				CartoonPet * p = pAdopter->adoptList[rev->cartoonID];
				if (!p) return false;

				Cmd::t_CartoonData d = p->getCartoonData();
				d.masterLevel = rev->level;
				p->setCartoonData(d);

				Channel::sendNine(p, "哦也～我主人 %s 升到%u级了，我练级也变快了～", d.masterName, rev->level);
				return true;
			}
			break;
		case PARA_CARTOON_CHARGE_NOTIFY:
			{
				t_chargeNotifyCartoon_SceneSession * rev = (t_chargeNotifyCartoon_SceneSession *)cmd;

				SceneUser *pAdopter = SceneUserManager::getMe().getUserByName(rev->adopter);
				if (!pAdopter)
				{
					Zebra::logger->error("[宠物]宠物充值返回时找不到领养者 adopter=%s cartoonID=%u time=%u", rev->adopter, rev->cartoonID, rev->time);
					return false;
				}

				if (pAdopter->adoptList.find(rev->cartoonID)==pAdopter->adoptList.end())
				{
					t_correctCartoon_SceneSession c;
					c.cartoonID = rev->cartoonID;
					sessionClient->sendCmd(&c, sizeof(c));
					return false;
				}

				t_saveCartoon_SceneSession send;
				send.cartoonID = rev->cartoonID;
				send.data = pAdopter->adoptList[rev->cartoonID]->getCartoonData();
				send.data.time += rev->time;
				send.type = SAVE_TYPE_SYN;
				sessionClient->sendCmd(&send, sizeof(send));

				pAdopter->adoptList[rev->cartoonID]->setCartoonData(send.data);
				return true;
			}
			break;
		case PARA_CARTOON_SALE:
			{
				t_saleCartoon_SceneSession * rev = (t_saleCartoon_SceneSession *)cmd;
				SceneUser *pUser = SceneUserManager::getMe().getUserByID(rev->userID);
				if (!pUser)
				{
					Zebra::logger->error("[宠物]出售宠物返回时找不到主人 userID=%u cartoonID=%u", rev->userID, rev->cartoonID);
					return false;
				}

				if (pUser->cartoonList.find(rev->cartoonID)==pUser->cartoonList.end()) return true;

				if (pUser->cartoon && pUser->cartoon->getCartoonID()==rev->cartoonID)
					pUser->cartoon->putAway(Cmd::Session::SAVE_TYPE_DONTSAVE);
				pUser->packs.addMoney(3, "出售替身宠物",NULL);

				Cmd::stRemoveCartoonCmd send;
				send.cartoonID = rev->cartoonID;
				pUser->sendCmdToMe(&send, sizeof(send));

				Zebra::logger->trace("[宠物]%s 出售宠物 %s", pUser->name, pUser->cartoonList[rev->cartoonID].name);
				pUser->cartoonList.erase(rev->cartoonID);

				pUser->refreshPetPackSize();
				return true;
			}
			break;
		case PARA_CARTOON_ADD:
			{
				t_addCartoon_SceneSession * rev = (t_addCartoon_SceneSession *)cmd;
				SceneUser *pUser = SceneUserManager::getMe().getUserByID(rev->userID);
				if (!pUser) return false;
				pUser->cartoonList[rev->cartoonID] = rev->data;

				if (pUser->cartoon && pUser->cartoon->getCartoonID()==rev->cartoonID)
				{
					if (rev->data.state==Cmd::CARTOON_STATE_WAITING
							|| rev->data.state==Cmd::CARTOON_STATE_ADOPTED)
					{
						CartoonPet * tmp = pUser->cartoon;
						pUser->killOnePet(pUser->cartoon);
						tmp->setClearState();
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "%s 正在等待或已经被领养，不能放出，如有问题请重新登录游戏", rev->data.name);
						Zebra::logger->debug("[宠物]%s(%u) 的宠物 %s(%u) 在跟随时被删除，session状态 %u", pUser->name, pUser->id, rev->data.name, rev->cartoonID, rev->data.state);
					}
					else
						pUser->cartoon->setCartoonData(pUser->cartoonList[rev->cartoonID]);
				}

				Cmd::stAddCartoonCmd ac;
				ac.isMine = true;
				ac.cartoonID = rev->cartoonID;
				ac.data = rev->data;
				pUser->sendCmdToMe(&ac, sizeof(ac));

				if (rev->data.state==Cmd::CARTOON_STATE_FOLLOW && !pUser->cartoon)
				{
					CartoonPet * cp = (CartoonPet *)pUser->summonPet(rev->data.npcID, Cmd::PET_TYPE_CARTOON);
					if (cp)
					{
						cp->setCartoonID(rev->cartoonID);
						cp->setCartoonData(rev->data);
					}
				}
				else if (rev->data.state==Cmd::CARTOON_STATE_PUTAWAY && !pUser->cartoon && pUser->charbase.petPoint)
				{
					//升级奖励的宠物修炼时间，一次性加到宠物身上
					pUser->cartoonList[rev->cartoonID].time += pUser->charbase.petPoint;

					/*
					Cmd::Session::t_saveCartoon_SceneSession s;
					strncpy(s.userName, pUser->name, MAX_NAMESIZE);
					s.type = SAVE_TYPE_CHARGE;
					s.cartoonID = rev->cartoonID;
					s.data = pUser->cartoonList[rev->cartoonID];
					sessionClient->sendCmd(&s, sizeof(s));
					*/
					Cmd::Session::t_chargeCartoon_SceneSession send;
					send.masterID = pUser->id;
					send.cartoonID = rev->cartoonID;
					send.time = pUser->charbase.petPoint;
					sessionClient->sendCmd(&send, sizeof(send));

					Cmd::stAddCartoonCmd ac;
					ac.isMine = true;
					ac.cartoonID = rev->cartoonID;
					ac.data = pUser->cartoonList[rev->cartoonID];
					pUser->sendCmdToMe(&ac, sizeof(ac));

					Zebra::logger->trace("%s 的宠物%s(%u)获得%u秒修炼时间", pUser->name, pUser->cartoonList[rev->cartoonID].name, rev->cartoonID, pUser->charbase.petPoint);
					pUser->charbase.petPoint = 0;
				}

				pUser->refreshPetPackSize();
				return true;
			}
			break;
		case PARA_CARTOON_GET_BACK:
			{
				t_getBackCartoon_SceneSession * rev = (t_getBackCartoon_SceneSession *)cmd;
				SceneUser *pUser = SceneUserManager::getMe().getUserByID(rev->userID);

				if (!pUser || pUser->adoptList.find(rev->cartoonID)==pUser->adoptList.end())//纠错，宠物没被收养但是标记为被该玩家收养了
				{
					t_correctCartoon_SceneSession c;
					c.cartoonID = rev->cartoonID;
					sessionClient->sendCmd(&c, sizeof(c));
					return true;
				}

				if (pUser) pUser->adoptList[rev->cartoonID]->putAway(SAVE_TYPE_RETURN);
				return true;
			}
			break;
		case PARA_CARTOON_NOTIFY:
			{
				t_notifyCartoon_SceneSession * rev = (t_notifyCartoon_SceneSession *)cmd;
				SceneUser *pUser = SceneUserManager::getMe().getUserByName(rev->adopter);
				if (!pUser) return true;
				if (pUser->adoptList.find(rev->cartoonID)==pUser->adoptList.end()) return true;

				pUser->adoptList[rev->cartoonID]->setExpRate(rev->state?3:2);

				if (rev->state)
					Channel::sendNine(pUser->adoptList[rev->cartoonID], "HOHO～主人上线了，我要加油，获得1.5倍的经验!");
				else
					Channel::sendNine(pUser->adoptList[rev->cartoonID], "主人下线了，我练级也没劲了...");
				return true;
			}
			break;
			/*
			   case PARA_CARTOON_ASK:
			   {
			   t_askCartoon_SceneSession * rev = (t_askCartoon_SceneSession *)cmd;
			   SceneUser *pUser = SceneUserManager::getMe().getUserByID(rev->masterID);
			   if (!pUser) return false;

			   for (SceneUser::cartoon_it it=pUser->cartoonList.begin(); it!=pUser->cartoonList.end(); it++)
			   {
			   if (it->second.state==Cmd::CARTOON_STATE_WAITING)
			   {
			   Cmd::stAddCartoonCmd add;
			   add.isMine = false;
			   add.cartoonID = it->first;
			   add.data = it->second;
			   sendCmdByID(rev->askerID, &add, sizeof(add));
			   return true;
			   }
			   }
			   return true;
			   }
			   break;
			   */
		case PARA_CARTOON_ADOPT:
			{
				t_adoptCartoon_SceneSession * rev = (t_adoptCartoon_SceneSession *)cmd;
				SceneUser *pUser = SceneUserManager::getMe().getUserByID(rev->userID);
				if (!pUser) return false;

				if (pUser->adoptList.size()>=5)
				{
					Cmd::Session::t_saveCartoon_SceneSession send;
					strncpy(send.userName, pUser->name, MAX_NAMESIZE);
					send.type = Cmd::Session::SAVE_TYPE_RETURN;
					send.cartoonID = rev->cartoonID;
					send.data = rev->data;
					send.data.state = Cmd::CARTOON_STATE_WAITING;
					bzero(send.data.adopter, MAX_NAMESIZE);
					sessionClient->sendCmd(&send, sizeof(send));

					Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "请不要重复领养");
					return true;
				}

				zNpcB *base = npcbm.get(rev->data.npcID);
				if (!base) return false;

				for(SceneUser::adopt_it it=pUser->adoptList.begin(); it!=pUser->adoptList.end(); it++)
					if (it->first==rev->cartoonID)
					{
						it->second->setClearState();
						pUser->adoptList.erase(it);
					}

				t_NpcDefine define;
				define.id = base->id;
				strncpy(define.name, base->name, MAX_NAMESIZE-1);
				define.pos = pUser->getPos();
				define.num = 1;
				define.interval = 5;
				define.initstate = zSceneEntry::SceneEntry_Normal;
				define.width = SceneUser::CALL_PET_REGION;
				define.height = SceneUser::CALL_PET_REGION;
				define.pos -= zPos(SceneUser::CALL_PET_REGION/2,SceneUser::CALL_PET_REGION/2);
				define.scriptID = 0;
				pUser->scene->initRegion(define.region, define.pos, define.width, define.height);

				CartoonPet * newPet = pUser->scene->summonOneNpc<CartoonPet>(define, zPos(0,0), base, 0, 0, 0);
				if (newPet)
				{
					strncpy(newPet->name, rev->data.name, MAX_NAMESIZE);
					newPet->setPetType(Cmd::PET_TYPE_CARTOON);
					newPet->setMaster(pUser);
					SceneNpcManager::getMe().addSpecialNpc(newPet);

					newPet->setCartoonID(rev->cartoonID);
					newPet->setCartoonData(rev->data);
					newPet->setExpRate(rev->masterState?3:2);
					pUser->adoptList[rev->cartoonID] = newPet;

					//Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "你收养了一只替身宝宝，现在开始可以获得%5额外的经验值");
					Channel::sendNine(newPet, "谢谢你带我练级～我可以帮助你获得5%额外的经验值:)");
					Zebra::logger->debug("[宠物]%s(%u级)领养了 %s(%u级) 的宠物(%u),现在有%u只 addExp=%u", pUser->name, pUser->charbase.level, rev->data.masterName, rev->data.masterLevel, rev->cartoonID, pUser->adoptList.size(), rev->data.addExp);
				}

				return true;
			}
			break;
		case PARA_CARTOON_CONSIGN:
			{
				t_consignCartoon_SceneSession *rev = (t_consignCartoon_SceneSession *)cmd;
				SceneUser * pUser = SceneUserManager::getMe().getUserByID(rev->userID);
				if (!pUser) return false;

				if (pUser->adoptList.size()>=5)
				{
					t_consignRetCartoon_SceneSession send;
					send.userID = pUser->id;
					send.ret = 2;
					send.cartoonID = rev->cartoonID;
					sendCmd(&send, sizeof(send));

					return true;
				}

				Cmd::stConsignCartoonCmd ccc;
				strncpy(ccc.name, rev->name, MAX_NAMESIZE);
				ccc.cartoonID = rev->cartoonID;
				pUser->sendCmdToMe(&ccc, sizeof(ccc));
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}
///////////////副本开始//////////////////////
bool SessionClient::cmdMsgParse_FuBen( const Cmd::t_NullCmd* ptNullCmd, const unsigned int nCmdLen )
{
    if( !ptNullCmd ) return false;
    using namespace Cmd::Session;
	switch (ptNullCmd->para)
    {
    case REQ_OPEN_ENTER_FUBEN_SESSION_PARA:
        {
            Req_OpenEnterFuBen_SceneSession *rev = (Req_OpenEnterFuBen_SceneSession *)ptNullCmd;
            if ( Cmd::FB_ACTION_TYPE_OPEN == rev->actType )
            {
                FuBenMgr::getMe().doReqOpenFuBen(rev);
            }
            else if ( Cmd::FB_ACTION_TYPE_ENTER == rev->actType )
            {
                FuBenMgr::getMe().doReqEnterFuBen(rev);
            }
            return true;
        }
        break;
    case REPLY_OPEN_ENTER_FUBEN_SESSION_PARA:
        {
            Reply_OpenEnterFuBen_SceneSession *rev = (Reply_OpenEnterFuBen_SceneSession *)ptNullCmd;
            if ( Cmd::FB_ACTION_TYPE_OPEN == rev->actType )
            {
                FuBenMgr::getMe().doReplyOpenFuBen(rev);
            }
            else if ( Cmd::FB_ACTION_TYPE_ENTER == rev->actType )
            {
                FuBenMgr::getMe().doReplyEnterFuBen(rev);
            }
            return true;
        }
        break;
	//家族地图
	case REQ_OPEN_ENTER_JIAZU_SESSION_PARA:
        {
            Req_OpenEnterJiaZu_SceneSession *rev = (Req_OpenEnterJiaZu_SceneSession *)ptNullCmd;
            if ( Cmd::FB_ACTION_TYPE_OPEN == rev->actType )
            {
                FuBenMgr::getMe().doReqOpenJiaZu(rev);
            }
            return true;
        }
        break;
	 case REPLY_OPEN_ENTER_JIAZU_SESSION_PARA:
        {
            Reply_OpenEnterJiaZu_SceneSession *rev = (Reply_OpenEnterJiaZu_SceneSession *)ptNullCmd;
            if ( Cmd::FB_ACTION_TYPE_OPEN == rev->actType )
            {
                FuBenMgr::getMe().doReplyEnterJiaZu(rev);
            }
            return true;
        }
        break;
    default:
        break;
    }


	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return true;
}
