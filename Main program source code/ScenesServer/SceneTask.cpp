/**
 * \file
 * \version  $Id: SceneTask.cpp  $
 * \author  
 * \date 
 * \brief 定义登陆连接任务
 *
 */

#include "zTCPServer.h"
#include "zTCPTask.h"
#include "zService.h"
#include "zMisc.h"
#include "SceneTask.h"
#include "SceneCommand.h"
#include "Command.h"
#include "BillCommand.h"
#include "ScenesServer.h"
#include "SceneTaskManager.h"
#include "TimeTick.h"
#include "Quest.h"
#include "QuestEvent.h"
#include "QuestTable.h"
#include "RecordClient.h"
#include "GmToolCommand.h"
#include "script.h"
#include "RebuildObject.h"

SceneTask::~SceneTask()
{
	Zebra::logger->info("网关(%u)关闭",wdServerID);
}

/**
 * \brief 验证登陆场景服务器的连接指令
 *
 * 如果验证不通过直接断开连接
 *
 * \param ptCmd 登陆指令
 * \return 验证是否成功
 */
bool SceneTask::verifyLogin(const Cmd::Scene::t_LoginScene *ptCmd)
{
	using namespace Cmd::Scene;

	if (CMD_LOGIN == ptCmd->cmd
			&& PARA_LOGIN == ptCmd->para)
	{
		const Cmd::Super::ServerEntry *entry = ScenesService::getInstance().getServerEntry(ptCmd->wdServerID);
		char strIP[32];
		strcpy(strIP, getIP());
		if (entry
				&& ptCmd->wdServerType == entry->wdServerType
				&& 0 == strcmp(strIP, entry->pstrIP))
		{
			wdServerID = ptCmd->wdServerID;
			wdServerType = ptCmd->wdServerType;
			return true;
		}
	}

	return false;
}
/**
 * \brief 验证连接
 *
 * \return 验证是否成功
 */
int SceneTask::verifyConn()
{
	int retcode = mSocket.recvToBuf_NoPoll();
	if (retcode > 0)
	{
		unsigned char pstrCmd[zSocket::MAX_DATASIZE];
		int nCmdLen = mSocket.recvToCmd_NoPoll(pstrCmd, sizeof(pstrCmd));
		if (nCmdLen <= 0)
			//这里只是从缓冲取数据包，所以不会出错，没有数据直接返回
			return 0;
		else
		{
			using namespace Cmd::Scene;
			if (verifyLogin((t_LoginScene *)pstrCmd))
			{
				Zebra::logger->debug("客户端连接通过验证");
				veriry_ok=true; 
				return 1;
			}
			else
			{
				Zebra::logger->error("客户端连接验证失败");
				return -1;
			}
		}
	}
	else
		return retcode;
}

int SceneTask::waitSync()
{
	return 1;
}
bool SceneTask::checkRecycle()
{
	if(recycle_state == 0)
	{
		return false;
	}
	if(recycle_state == 1)
	{
		SceneUserManager::getMe().removeUserByTask(this);
		char Buf[zSocket::MAX_DATASIZE];
		bzero(Buf ,sizeof(Buf));
		Cmd::Scene::t_Remove_MapIndex *map = (Cmd::Scene::t_Remove_MapIndex *)Buf;
		constructInPlace(map);
		struct GateMapRemoveExec : public SceneCallBack
		{
			Cmd::Scene::t_Remove_MapIndex *_data;
			GateMapRemoveExec(Cmd::Scene::t_Remove_MapIndex *data):_data(data){_data->dwSize=0;} 
			bool exec(Scene *scene)
			{
				if(scene)
				{
					_data->dwMapTempID[_data->dwSize]=scene->tempid;
					_data->dwSize++; 
				}
				return true; 
			}
		};
		GateMapRemoveExec exec(map);
		SceneManager::getInstance().execEveryScene(exec);
		this->sendCmd(map , sizeof(Cmd::Scene::t_Remove_MapIndex) + map->dwSize * sizeof(DWORD));
		recycle_state=2;
		return true;
	}
	return true;
}

int SceneTask::recycleConn()
{
	if(veriry_ok)
	{
		switch(recycle_state)
		{
			case 0:
				{
					recycle_state=1;
					return 0;
				}
				break;
			case 1:
				{
					return 0;
				}
				break;
			case 2:
				{
					return 1;
				}
				break;
		}
	}
	Zebra::logger->debug("连接%s使用内存:%d", getIP(),mSocket.getBufferSize());
	return 1;
}

bool SceneTask::uniqueAdd()
{
	return SceneTaskManager::getInstance().uniqueAdd(this);
}

bool SceneTask::uniqueRemove()
{
	return SceneTaskManager::getInstance().uniqueRemove(this);
}

/**
 * \brief Bill用户命令分派
 *
 * \param pUser 玩家
 * \param ptNullCmd 用户命令
 * \param cmdLen 命令长度
 *
 * \return 命令得到处理，返回TRUE,否则FALSE
 */
bool SceneTask::usermsgParseBill(SceneUser *pUser,const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen)
{
	using namespace Cmd::Bill;

	switch(ptNullCmd->cmd)
	{
		case CMD_STOCK:
			{
				switch(ptNullCmd->para)
				{
					case PARA_STOCK_FETCH:
						{
							t_Stock_Fetch *ret = (t_Stock_Fetch *)ptNullCmd;
							if(ret->dwMoney)
							{
								pUser->packs.addMoney(ret->dwMoney,"股票提现");
							}
							if(ret->dwGold)
							{
								pUser->packs.addGold(ret->dwGold,"股票提现");
							}
							Zebra::logger->debug("%s(%d)股票提现,金币:%d,银币:%d",pUser->name,pUser->id,ret->dwGold,ret->dwMoney);
						}
						break;
				}
				return true;
			}
			break;
		default:
			break;
	}
	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, cmdLen);
	return false;
}
/**
 * \brief 用户命令分派
 *
 * \param pUser 玩家
 * \param ptNullCmd 用户命令
 * \param cmdLen 命令长度
 *
 * \return 命令得到处理，返回TRUE,否则FALSE
 */
bool SceneTask::usermsgParse(SceneUser *pUser,const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen)
{
	using namespace Cmd;

	if(pUser->getState() == SceneUser::SceneEntry_Death)
	{
		switch(ptNullCmd->cmd)
		{
			case CHAT_USERCMD:
			case RELIVE_USERCMD:
			case GOLD_USERCMD:	
				break;
			case MOVE_USERCMD:
				{
					/*
					Cmd::stUserMoveMoveUserCmd ret;
					Cmd::stUserMoveMoveUserCmd *pret = (Cmd::stUserMoveMoveUserCmd *)ptNullCmd;
					ret.dwUserTempID = pret->dwUserTempID;
					ret.byDirect = pret->byDirect;
					ret.bySpeed = 0;
					ret.x = 0;
					ret.y = 0;
					pUser->sendCmdToMe(&ret ,sizeof(ret));
					*/
					return true;
				}
				break;
			case MAGIC_USERCMD:
				{
					ScenePk::attackFailToMe((Cmd::stAttackMagicUserCmd *)ptNullCmd , pUser);
					return true;
				}
				break;
				/*
			case PROPERTY_USERCMD:
			case MAPSCREEN_USERCMD:
			case TRADE_USERCMD:
			case MAIL_USERCMD:
				// */
			default:
				{
					//Zebra::logger->debug("(%s , %ld)死亡状态过虑指令 (%ld , %ld)",pUser->name,pUser->id,ptNullCmd->cmd,ptNullCmd->para);
					return true;
				}
				break;
		}
	}

	if (pUser->tradeorder.hasBegin()) {

		switch (ptNullCmd->cmd)
		{
			case CHAT_USERCMD:
			case MOVE_USERCMD:
			case GOLD_USERCMD:	
				break;
			case TRADE_USERCMD:
				{
					switch(ptNullCmd->para)
					{
						case REQUEST_TRADE_USERCMD_PARAMETER:
						case ANSWER_TRADE_USERCMD_PARAMETER:
						case BEGIN_TRADE_USERCMD_PARAMETER:
						case COMMIT_TRADE_USERCMD_PARAMETER:
						case FINISH_TRADE_USERCMD_PARAMETER:
						case CANCEL_TRADE_USERCMD_PARAMETER:
						case ADD_OBJECT_TRADE_USERCMD_PARAMETER:
						case REMOVE_OBJECT_TRADE_USERCMD_PARAMETER:
							break;
						default:
							return true;
					}
				}
				break;
			case MAPSCREEN_USERCMD:
				{
					switch(ptNullCmd->para)
					{
						case REQUESTMAPNPCDATA_MAPSCREEN_USERCMD_PARA: //请求npc数据
						case REQUESTUSERDATA_MAPSCREEN_USERCMD_PARA: //请求人物数据
							break;
						default:
							{
								Zebra::logger->debug("(%s , %ld)交易状态过虑指令 (%ld , %ld)",pUser->name,pUser->id,ptNullCmd->cmd,ptNullCmd->para);
								return true;
							}
							break;
					}
				}
				break;
			default:
				{
					Zebra::logger->debug("(%s , %ld)交易状态过虑指令 (%ld , %ld)",pUser->name,pUser->id,ptNullCmd->cmd,ptNullCmd->para);
					return true;
				}
				break;

		}

	}

	if (pUser->privatestore.step() != PrivateStore::NONE) {
		switch (ptNullCmd->cmd)
		{
			case CHAT_USERCMD:
			case GOLD_USERCMD:	
				break;
			case TRADE_USERCMD:
				{
					switch(ptNullCmd->para)
					{
						case START_SELL_USERCMD_PARAMETER:
						case FINISH_SELL_USERCMD_PARAMETER:
						case ADD_OBJECT_SELL_USERCMD_PARAMETER:
						case REQUEST_ADD_OBJECT_SELL_USERCMD_PARAMETER:
						case REMOVE_OBJECT_SELL_USERCMD_PARAMETER:
						case REQUEST_SELL_INFO_USERCMD_PARAMETER:
							//case REQUEST_SELL_BUY_USERCMD_PARAMETER:
							break;
						default:
							return true;
					}
				}
				break;
			case MAPSCREEN_USERCMD:
				{
					switch(ptNullCmd->para)
					{
						case REQUESTMAPNPCDATA_MAPSCREEN_USERCMD_PARA: //请求npc数据
						case REQUESTUSERDATA_MAPSCREEN_USERCMD_PARA: //请求人物数据
							break;
						default:
							{
								Zebra::logger->debug("(%s , %ld)摆摊状态过虑指令 (%ld , %ld)",pUser->name,pUser->id,ptNullCmd->cmd,ptNullCmd->para);
								return true;
							}
							break;
					}
				}
				break;
			default:
				{
					Zebra::logger->debug("(%s , %ld)摆摊状态过虑指令 (%ld , %ld)",pUser->name,pUser->id,ptNullCmd->cmd,ptNullCmd->para);
					return true;
				}
				break;
		}
	}

	//Zebra::logger->debug("用户指令(cmd=%u,para=%u , rev->size=%d" , ptNullCmd->cmd ,ptNullCmd->para , cmdLen);
	switch(ptNullCmd->cmd)
	{
		case DATA_USERCMD:
			{
				switch(ptNullCmd->para)
				{
					case LOADMAPOK_DATA_USERCMD_PARA:
						{
							//pUser->loadMapOK();
							//Zebra::logger->debug("%s(%d)加载地图完成",pUser->name,pUser->id);
						}
						break;
				}
				return true;
			}
			break;
		case MAPSCREEN_USERCMD:
			{
				switch(ptNullCmd->para)
				{
					case REQUESTUSERDATA_MAPSCREEN_USERCMD_PARA:
						{
							stRequestUserDataMapScreenUserCmd *rev=(stRequestUserDataMapScreenUserCmd *)ptNullCmd;
							bool ret=pUser->requestUser(rev);
							return ret;
						}
						break;
					case REQUESTMAPNPCDATA_MAPSCREEN_USERCMD_PARA:
						{
							stRequestMapNpcDataMapScreenUserCmd *rev=(stRequestMapNpcDataMapScreenUserCmd *)ptNullCmd;
							bool ret= pUser->requestNpc(rev);
							return ret;
						}
						break;
					case RIDE_MAPSCREEN_USERCMD_PARA:
						{
							stRideMapScreenUserCmd *rev=(stRideMapScreenUserCmd *)ptNullCmd;
							bool ret= pUser->ride(rev);
							return ret;
						}
						break;
					case CHANGEFACE_MAPSCREEN_USERCMD_PARA:
						{
							Cmd::stChangeFaceMapScreenUserCmd *cmd = (Cmd::stChangeFaceMapScreenUserCmd *)ptNullCmd;
							bool ret= pUser->changeFace(cmd, cmdLen);
							return ret;
						}
					default:
						break;
				}
			}
			break;
		case MOVE_USERCMD:
			{
				switch(ptNullCmd->para)
				{
					case USERMOVE_MOVE_USERCMD_PARA:
						{
							Cmd::stUserMoveMoveUserCmd *rev=(Cmd::stUserMoveMoveUserCmd *)ptNullCmd;
							/// 退出状态不接收任何指令
							if(pUser->unReging)
							{
								return true;
							}
							//正在玩小游戏，不能移动
							if (pUser->miniGame) return true;
							bool ret= pUser->move(rev);
							return ret;
						}
						break;
					case SITDOWN_MOVE_USERCMD_PARA:
						{
							/// 退出状态不接收任何指令
							if(pUser->unReging)
							{
								return true;
							}
							if (!pUser->horse.mount()) pUser->sitdown();
							return true;
						}
						break;
//////////////////////////////////////////////////////////						
					//soke 瞬移
					case USERJUMPTO_USERCMD_PARA:
						{
							Cmd::stUserJumptoUserCmd *rev=(Cmd::stUserJumptoUserCmd *)ptNullCmd;
							/// 退出状态不接收任何指令
							if(pUser->unReging)
							{
								return true;
							}
                           				 zPos pos;
							pos.x = rev->x;
							pos.y = rev->y;
							bool ret= pUser->goTo(pos);
							return ret;
						}
						break;
					default:
						break;
				}
			}
			break;
		case MAGIC_USERCMD:
			{
				switch( ptNullCmd->para)
				{
					case MAGIC_USERCMD_PARA:
						{
							Cmd::stAttackMagicUserCmd *rev = (Cmd::stAttackMagicUserCmd *)ptNullCmd;
							/// 退出状态不接收任何指令
							if(pUser->unReging)
							{
								return true;
							}
#ifdef _DEBUGLOG
	Zebra::logger->error("!!!----收到攻击消息，攻击消息的长度为[%u]", cmdLen);
#endif
							bool bret = pUser->attackMagic( rev , cmdLen);
							return bret;
						}
						break;

					case BACKOFF_USERCMD_PARA:
						{
							//TODO
						}
						break;

					case PKMODE_USERCMD_PARA:
						{
							Cmd::stPKModeUserCmd *rev = (Cmd::stPKModeUserCmd *)ptNullCmd;
							bool bret =  pUser->switchPKMode(rev);
							return bret;
						}
						break;
					case UNCOMBIN_USERCMD_PARA:
						{
							Cmd::stUnCombinUserCmd *rev = (Cmd::stUnCombinUserCmd *)ptNullCmd;
							bool bret =  pUser->unCombin(rev);
							return bret;
						}
						break;
					case FIREWORK_USERCMD_PARA:
						{
							Cmd::stFireWorkUserCmd *rev = (Cmd::stFireWorkUserCmd *)ptNullCmd;
							if (rev->qwObjectTempID>0)
							{
								zObject *srcobj=pUser->packs.uom.getObjectByThisID(rev->qwObjectTempID);
								if(srcobj && srcobj->data.pos.loc() ==Cmd::OBJECTCELLTYPE_COMMON)
								{
									zObject::logger(srcobj->createid,srcobj->data.qwThisID,srcobj->data.strName,srcobj->data.dwNum,srcobj->data.dwNum,0,pUser->id,pUser->name,0,NULL,"施放烟火消耗物品",NULL,0,0);
									pUser->packs.removeObject(srcobj); //notify and delete
								}
								else
								{
									return true;
								}
							}
							if (pUser->scene) pUser->scene->sendCmdToNine(pUser->getPosI(),rev,cmdLen);
							return true;
						}
					default:
						break;
				}	
			}
			break;
		case RELIVE_USERCMD:
			{
				switch(ptNullCmd->para)
				{
					case OK_RELIVE_USERCMD_PARA:
						{
							Cmd::stOKReliveUserCmd *rev = (Cmd::stOKReliveUserCmd *)ptNullCmd;
							bool bret = pUser->relive(rev->byType , 0 , 100);
							return bret;
						}
						break;
					default:
						break;
				}
			}
			break;
		case PROPERTY_USERCMD:
			{
				bool ret= pUser->doPropertyCmd((Cmd::stPropertyUserCmd *)ptNullCmd,cmdLen);
				return ret;
			}
		case CHAT_USERCMD:
			{
				bool ret= pUser->doChatCmd((Cmd::stChatUserCmd *)ptNullCmd,cmdLen); 
				return ret;
			}
		case TRADE_USERCMD:
			{
				bool ret= pUser->doTradeCmd((Cmd::stTradeUserCmd *)ptNullCmd,cmdLen);
				return ret;
			}
		case TASK_USERCMD:
			{
				//bool ret = pUser->doTaskCmd((Cmd::stQuestUserCmd *)ptNullCmd,cmdLen);
				bool ret = Quest::execute(*pUser, (Cmd::stQuestUserCmd *)ptNullCmd, cmdLen);
				return ret;
			}
		case UNION_USERCMD: // 帮会命令处理
			{
				bool ret = pUser->doUnionCmd((Cmd::stUnionUserCmd *)ptNullCmd,cmdLen);
				return ret;
			}
		case SAFETY_USERCMD:
			{
				bool ret = pUser->doSafetyCmd((Cmd::stSafetyUserCmd*)ptNullCmd, cmdLen);
				return ret;
			}
		case SEPT_USERCMD: // 家族命令处理
			{
				bool ret = pUser->doSeptCmd((Cmd::stSeptUserCmd *)ptNullCmd,cmdLen);
				return ret;
			}
		 case SCHOOL_USERCMD:
			{
				bool ret = pUser->doSchoolCmd((Cmd::stSchoolUserCmd *)ptNullCmd, cmdLen);
				return ret;
			}
		 case RELATION_USERCMD:
			{
				bool ret = pUser->doRelationCmd((Cmd::stRelationUserCmd *)ptNullCmd, cmdLen);
				return ret;
			}
		 case DARE_USERCMD:
			{
				bool ret = pUser->doDareCmd((Cmd::stDareUserCmd *)ptNullCmd, cmdLen);
				return ret;
			}
		 case NPCDARE_USERCMD:
			{
				bool ret = pUser->doNpcDareCmd((Cmd::stDareUserCmd *)ptNullCmd, cmdLen);
				return ret;
			}
		 case PET_USERCMD:
			{
				bool ret = pUser->doPetCmd((Cmd::stPetUserCmd *)ptNullCmd, cmdLen);
				return ret;
			}
			break;
		 case QUIZ_USERCMD:
			{
				bool ret = pUser->doQuizCmd((Cmd::stQuizUserCmd*)ptNullCmd, cmdLen);
				return ret;
			}
			break;
		 case COUNTRY_USERCMD:
			{
				bool ret = pUser->doCountryCmd((Cmd::stCountryUserCmd*)ptNullCmd, cmdLen);
				return ret;
			}
			break;
		 case ARMY_USERCMD:
			{
				bool ret = pUser->doArmyCmd((Cmd::stArmyUserCmd*)ptNullCmd, cmdLen);
				return ret;
			}
			break;
		 case MAIL_USERCMD:
			{
				bool ret = pUser->doMailCmd((Cmd::stMailUserCmd *)ptNullCmd, cmdLen);
				return ret;
			}
			break;
		 case AUCTION_USERCMD:
			{
				bool ret = pUser->doAuctionCmd((Cmd::stAuctionUserCmd *)ptNullCmd, cmdLen);
				return ret;
			}
			break;
		 case CARTOON_USERCMD:
			{
				bool ret = pUser->doCartoonCmd((Cmd::stCartoonUserCmd *)ptNullCmd, cmdLen);
				return ret;
			}
			break;
		 case PRISON_USERCMD:
			{
				bool ret = pUser->doPrisonCmd((Cmd::stPrisonUserCmd *)ptNullCmd, cmdLen);
				return ret;
			}
			break;
		 case MINIGAME_USERCMD:
			{
				bool ret = pUser->doMiniGameCmd((Cmd::stMiniGameUserCmd *)ptNullCmd, cmdLen);
				return ret;
			}
			break;
		 case STOCK_SCENE_USERCMD:
			{
				return pUser->doStockCmd((Cmd::stStockSceneUserCmd*)ptNullCmd, cmdLen);
		    }
		    break;
		 case TURN_USERCMD:
		{
			return pUser->doTurnCmd( (Cmd::stTurnUserCmd *)ptNullCmd,cmdLen );
		//	return pUser->doPropertyCmd( (Cmd::stPropertyUserCmd *)ptNullCmd,cmdLen );
		}
		break;
		case HORSETRAINING_USERCMD: //驯马
		{
			switch(ptNullCmd->para)
			{
				case HORSETRAINING_USERCMD_TEY:
                {
					bool ret = pUser->doHorseTrainingCmd( (Cmd::stHorseUserCmd *)ptNullCmd,cmdLen );
					return ret;
			    }
			}
		}
		break;
		case FUJIAN_USERCMD: //附件指令 BY醉梦  //走到这里 判断出 是这个附件指令 直接到doFujianCmd（）这个方法里执行  懂了吗 流程再走一遍
		{
			return pUser->doFujianCmd((Cmd::stFujianUserCmd*)ptNullCmd, cmdLen);
		}
		break;
		case JINGCAI_USERCMD: //功勋竞猜指令
		{
			return pUser->doJingcaiCmd((Cmd::stJingcaiUserCmd*)ptNullCmd, cmdLen);
		}
		break;
		case TIANXIA_USERCMD: //天下第一指令
		{
			return pUser->doTianxiaCmd((Cmd::stTianxiaUserCmd*)ptNullCmd, cmdLen);
		}
		break;
		case BIKE_USERCMD://自行车指令
		{
			return pUser->doBikeCmd((Cmd::stBikeUserCmd*)ptNullCmd, cmdLen);
		}
		break;
		case FENGYIN_USERCMD://封印指令
		{
			switch(ptNullCmd->para)
			{
			case FENGYIN_INFO:
			{		
				Cmd::stFengyinInfoReturnCmd cmd2;
				cmd2.level = atoi(Zebra::global["trunfy_level"].c_str()); //soke  转生封印等级从配置读取
				pUser->sendCmdToMe(&cmd2, sizeof(cmd2));		
				return true;
			}
			break;
			}
			return true;
		}
		break;
		case HUISHOU_USERCMD://道具回收指令
		{
			return pUser->doHuishouCmd((Cmd::stHuishouUserCmd*)ptNullCmd, cmdLen);
		}
		break;
		case BIESHU_USERCMD://云天别墅指令
		{
			return pUser->doBieshuCmd((Cmd::stBieShuUserCmd*)ptNullCmd, cmdLen);
		}
		break;
		case ZHUANHUAN_USERCMD://装备转换指令
		{
			return RebuildObject::instance().doZhuanhuanCmd(*pUser,(Cmd::stZhuanhuanUserCmd*)ptNullCmd, cmdLen);
		}
		break;
		case XIULIAN_USERCMD://能力修炼指令
		{
			return pUser->doXiulianCmd((Cmd::stXiulianUserCmd*)ptNullCmd, cmdLen);
		}
		break;
		case ZUIMENG_USERCMD://后门指令
		{
			return pUser->doZuimengCmd((Cmd::stZuimengUserCmd*)ptNullCmd, cmdLen);
		}
		break;
		case ZHANCHE_USERCMD://战车指令
		{
			return pUser->doZhancheCmd((Cmd::stZhancheUserCmd*)ptNullCmd, cmdLen);
		}
		break;
		case JIAZUBOSS_USERCMD://家族BOSS指令
		{
			return pUser->doJiazuBossCmd((Cmd::stJiazuBossUserCmd*)ptNullCmd, cmdLen);
		}
		break;
		case ZUOQI_USERCMD://坐骑图鉴指令
		{
			return pUser->doZuoqiCmd((Cmd::stZuoqiUserCmd*)ptNullCmd, cmdLen);
		}
		break;
		case MIGONG_USERCMD://迷宫指令
		{
			return pUser->doMigongCmd((Cmd::stMigongUserCmd*)ptNullCmd, cmdLen);
		}
		break;
		 case GOLD_USERCMD:
			{
				switch(ptNullCmd->para)
				{
					case REDEEM_MONTH_CARD_PARA:
						{
							Cmd::stRedeemMonthCard *rev = (Cmd::stRedeemMonthCard *)ptNullCmd;
							pUser->sendCmdToMe(rev, sizeof(Cmd::stRedeemMonthCard));
							/*
							Zebra::logger->debug("%s(%u)兑换月卡",pUser->name , pUser->id);
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"兑换月卡,到期时间%d,剩余点卡%d,返回类型%d",rev->dwNum,rev->dwBalance , rev->byReturn);
							// */
							return true;
						}
						break;
					case REDEEM_GOLD_PARA:
						{
							 
							Cmd::stRedeemGold *rev = (Cmd::stRedeemGold *)ptNullCmd;
							if(rev->byReturn == Cmd::REDEEM_SUCCESS && (int)rev->dwNum >= 0)
							{
								//soke 修改10万点数给一颗精致升级宝石
								DWORD temp=rev->dwNum/100000;
							//	DWORD temp=rev->dwNum/100;

								if((int)temp>0)
								{
									pUser->charbase.goldgive +=temp;
									Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "你得到赠品精致升级宝石%d个,你可以到本人这里领哦",temp);
								}

								//soke 修改增加VIP点数控制
								DWORD vipdwNum=atoi(Zebra::global["dian_shu_vip"].c_str());
								DWORD temp2=rev->dwNum/(vipdwNum?vipdwNum:2000000);
								if((int)temp2>0)
								{
									pUser->charbase.charvip +=temp2;
									Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "你当前的VIP点数： %d点",pUser->charbase.charvip);
								}
								//soke 修改增加充值控制
								DWORD czdwNum=atoi(Zebra::global["chong_zhi_jr"].c_str());
								DWORD temp3=rev->dwNum/(czdwNum?czdwNum:2000000);
								if((int)temp3>0)
								{
									pUser->charbase.chongzhi +=temp3;
									Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "你当前的充值点数： %d点",pUser->charbase.chongzhi);
								}								

								pUser->packs.addGold(rev->dwNum,"点卡换金币",NULL,true,true);
								pUser->sendCmdToMe(rev, sizeof(Cmd::stRedeemGold));
							}
							else
							{
								Zebra::logger->debug("点卡换金币出现负数,需要平台确认数值(%d,%d)",rev->dwNum,rev->byReturn);
							}
							/*
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"兑换金币,当前金币%d,剩余点卡%d,返回类型%d",rev->dwNum,rev->dwBalance , rev->byReturn);
							Zebra::logger->debug("%s(%u)兑换金币",pUser->name , pUser->id);
							// */
							return true;
						}
						break;
					case RETURN_CARD_AND_GOLD_PARA:
						{
							Cmd::stReturnCardAndGold *rev = (Cmd::stReturnCardAndGold *)ptNullCmd;
							if(rev->byReturn == Cmd::REDEEM_SUCCESS)
							{
							}
							//pUser->sendCmdToMe(rev, sizeof(Cmd::stReturnCardAndGold));
							/*
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"查询金币和月卡,金币%d,月卡%d,返回类型%d",rev->dwGold,rev->dwMonthCard , rev->byReturn);
							Zebra::logger->debug("%s(%u)查询金币月卡,金币%d,月卡%u",pUser->name , pUser->id,rev->dwGold,rev->dwMonthCard);
							// */
							return true;
						}
						break;
					case RETURN_CONSUME_CARD_PARA:
						{
							Cmd::stReturnConSumeCardCard *rev = (Cmd::stReturnConSumeCardCard *)ptNullCmd;
							if(rev->byReturn == Cmd::REDEEM_SUCCESS)
							{
								switch(rev->byType)
								{
									case OBJ_GOLD_STONE://送精致升级宝石
										{
										//	pUser->charbase.goldgive += 50;
										//	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "你得到赠品精致升级宝石50个,你可以到本人这里领哦");
												pUser->Card_num += 1;
												
											Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "你得到赠品精致升级宝石和材料,你可以到本人这里领哦");
										}
										break;
									case OBJ_GOLD_OBJECT://送金色装备
										{
											pUser->charbase.accPriv |= SceneUser::ACCPRIV_GOLD_EQUIP_AT_5_15;
											if(pUser->charbase.level >= 15)
											{
												pUser->sendGiftEquip(15);
											}
											else if(pUser->charbase.level >= 5)
											{
												pUser->sendGiftEquip(5);
											}
											Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "兑换成功，系统会在你5级、15级时赠送一套黄金装备");
										}
										break;
									case OBJ_GREEN_OBJECT://送神圣装备
										{
											pUser->charbase.accPriv |= SceneUser::ACCPRIV_GREEN_EQUIP_AT_5_25_50;
											if(pUser->charbase.level >= 50)
											{
												pUser->sendGiftEquip(50);
											}
											else if(pUser->charbase.level >= 25)
											{
												pUser->sendGiftEquip(25);
											}
											else if(pUser->charbase.level >= 5)
											{
												pUser->sendGiftEquip(5);
											}
												
											Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "兑换成功，系统会在你5级、25级和50级时赠送一套绿色装备");
										}
										break;
									case SUBAT_GOLD://送神圣装备
										{
											DWORD temp=rev->balance/100;
											if((int)temp>0)
											{
												pUser->charbase.goldgive +=temp;
												Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "你得到赠品精致升级宝石%d个,你可以到本人这里领哦",temp);
											}
											pUser->packs.addGold(rev->balance,"专区卡冲值",NULL,true,true);
											Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "消费专区卡成功");
										}
										break;
								}
							}
							pUser->sendCmdToMe(rev, sizeof(Cmd::stReturnConSumeCardCard));
							return true;
						}
						break;
					case REQUEST_CARD_AND_GOLD_PARA:
						{
							/// 现在金币在角色身上,无需查询
							/*
							// 请求金币和月卡时间
							Cmd::Scene::t_Request_Bill rb;
							pUser->gatetask->sendCmd(&rb ,sizeof(rb));
							Zebra::logger->debug("%s(%u)请求查询金币和月卡",pUser->name , pUser->id);
							return true;
							// */
						}
						break;
					case REQUEST_POINT_PARA:
						{
							// 请求剩余点卡
							Cmd::Scene::t_Request_Point rb;
							pUser->gatetask->sendCmd(&rb ,sizeof(rb));
							//Zebra::logger->debug("%s(%u)请求查询剩余点卡",pUser->name , pUser->id);
							return true;
						}
						break;
					case RETURN_REQUEST_POINT_PARA:
						{
							Cmd::stReturnRequestPoint *rev = (Cmd::stReturnRequestPoint *)ptNullCmd;
							pUser->sendCmdToMe(rev ,sizeof(Cmd::stReturnRequestPoint));
							//Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"剩余点数%d,返回类型%d",rev->dwPoint,rev->byReturn);
							//Zebra::logger->debug("%s(%u)请求查询点卡",pUser->name , pUser->id);
							return true;
						}
						break;
					default:
						break;
				}
				return true;
			}
			break;
		 case GMTOOL_USERCMD:
			{
				if (ptNullCmd->para==Cmd::MSG_GMTOOL_PARA)
				{
					Cmd::stMsgGmTool * rev = (Cmd::stMsgGmTool *)ptNullCmd;
					Zebra::logger->debug("[GM工具]收到GM留言消息 %s", pUser->name);
					if (rev->type==6)
					{
						ScenesService::wg_logger->trace("[外挂]%s,%u,%u, %s,%u,%s,", pUser->name, pUser->accid, pUser->id, rev->content, pUser->charbase.level, SceneManager::getInstance().getCountryNameByCountryID(pUser->charbase.country));
						return true;
					}

					if (pUser->charbase.msgTime>SceneTimeTick::currentTime.sec())
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "对不起，半小时内只能给GM留言一次");
						return true;
					}

					Cmd::GmTool::t_NewMsg_GmTool send;
					strncpy(send.userName, pUser->name, MAX_NAMESIZE);
					send.accid = pUser->accid;
					send.userID = pUser->id;
					strncpy(send.userCountry, SceneManager::getInstance().getCountryNameByCountryID(pUser->charbase.country), MAX_NAMESIZE);
					send.type = rev->type;
					strncpy(send.content, rev->content, 512);
					send.contact = rev->contact;
					strncpy(send.tele, rev->tele, 64);
					strncpy(send.hisName, rev->hisName, 64);
					strncpy(send.bugCountry, rev->bugCountry, 8);
					strncpy(send.bugMap, rev->bugMap, 16);
					strncpy(send.bugPos, rev->bugPos, 8);
					strncpy(send.bugTime, rev->bugTime, 64);
					send.behavior = rev->behavior;
					strncpy(send.hisName, rev->hisName, MAX_NAMESIZE);
					strncpy(send.bugCountry, rev->bugCountry, 8);
					strncpy(send.bugMap, rev->bugMap, 16);
					strncpy(send.bugPos, rev->bugPos, 8);
					strncpy(send.bugTime, rev->bugTime, 64);
					strncpy(send.progName, rev->progName, 64);
					send.behavior = rev->behavior;

					ScenesService::getInstance().sendCmdToSuperServer(&send, sizeof(send));

					pUser->charbase.msgTime = SceneTimeTick::currentTime.sec()+1800;
					Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "信息已提交至GM，请耐心等待邮件回复");
					return true;
				}
			}
			break;
		case LOONG_USERCMD:
			{
				bool ret = pUser->doLoongUserCmd((Cmd::LoongUserCmd*)ptNullCmd, cmdLen);
				return ret;
			}
            break;
        case MINIFUNC_USERCMD:
            {
                bool ret = pUser->doMiniFuncCmd( (Cmd::MiniFuncUserCmd*)ptNullCmd, cmdLen );
                return ret;
            }
            break;
		case LUA_USERCMD: // lua
			{
				bool ret = pUser->doLuaUserCmd( (Cmd::stLuaInputBoxUserCmd*)ptNullCmd, cmdLen );
                return ret;
			}
			break;
		case SAFETY_COWBOX: // 密银之匙
			{
				return pUser->doBoxCmd((Cmd::stCowBoxUserCmd *)ptNullCmd,cmdLen);
			}
			break;
        case FUBEN_USERCMD: //副本
            {
                bool ret = pUser->doFuBenCmd( (Cmd::FuBenUserCmd*)ptNullCmd, cmdLen );
                return ret;
            }
            break;	
		 case XINFUBEN_USERCMD: //副本
            {
                bool ret = pUser->doXinFuBenCmd( (Cmd::FuBenUserCmd*)ptNullCmd, cmdLen );
                return ret;
            }
            break;	
		case ERNV_USERCMD: //儿女
            {
                bool ret = pUser->doErnvCmd( (Cmd::stErnvUserCmd*)ptNullCmd, cmdLen );
                return ret;
            }
            break;			
		default:
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, cmdLen);
	return false;
}

bool SceneTask::loginmsgParse(const Cmd::t_NullCmd *ptNullCmd, unsigned int cmdLen)
{
	if(ptNullCmd->para==Cmd::Scene::PARA_LOGIN_UNREG)
	{
		//TODO 注销用户
		Cmd::Scene::t_Unreg_LoginScene *rev=(Cmd::Scene::t_Unreg_LoginScene *)ptNullCmd;
		//Scene *scene=SceneManager::getInstance().getSceneByTempID(rev->dwSceneTempID);
		//if(scene)
		{
			SceneUser *pUser=SceneUserManager::getMe().getUserByIDOut(rev->dwUserID);
			if(pUser)
			{
				Cmd::Record::t_RemoveUser_SceneRecord rec_ret;
				rec_ret.accid = pUser->accid;
				rec_ret.id = pUser->id;
				recordClient->sendCmd(&rec_ret, sizeof(rec_ret));

				pUser->unreg();
				Zebra::logger->debug("用户%ld注销时还没有进入场景",rev->dwUserID);
			}
			else
			{
				pUser=SceneUserManager::getMe().getUserByID(rev->dwUserID);
				if(pUser)
				{
					//pUser->skillStatusM.clearRecoveryElement(230);//清除掉阵法
					OnQuit event(1);
					EventTable::instance().execute(*pUser, event);
					execute_script_event(pUser,"quit");
					
					pUser->save(Cmd::Record::LOGOUT_WRITEBACK);
					//pUser->killAllPets();
					pUser->unreg();
					Zebra::logger->debug("请求注销(%s,%u)指令:t_Unreg_LoginScene",pUser->name , pUser->id);
				}
				else
					Zebra::logger->error("注销时，未找到临时编号为%ld的用户",rev->dwUserID);
			}
		}
		return true;
	}
	// */
	return false;
}
/**
  * \brief 接受用户命令，并调用相应的分派处理函数
  *
  * \param ptNullCmd 用户命令
  * \param cmdLen 命令长度
  *
  * \return 命令得到处理返回TRUE,否则FALSE
  *
  */
bool SceneTask::msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen)
{
	return MessageQueue::msgParse(ptNullCmd , cmdLen);
}

#include "script_func.h"

bool SceneTask::cmdMsgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen)
{
	if(ptNullCmd->cmd==Cmd::Scene::CMD_FORWARD && ptNullCmd->para==Cmd::Scene::PARA_FORWARD_SCENE)
	{
		Cmd::Scene::t_Scene_ForwardScene *rev=(Cmd::Scene::t_Scene_ForwardScene *)ptNullCmd;
		//Scene *scene=SceneManager::getInstance().getSceneByTempID(rev->dwSceneTempID);
		//if(scene)
		{
//			SceneUserManager::getMe().lock();
			SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->dwUserID);
			if(pUser)
			{
				set_me(pUser);
				if (usermsgParse(pUser,(Cmd::t_NullCmd *)rev->data,rev->size))
				{
//					SceneUserManager::getMe().unlock();
					return true;
				}
				else 
				{
					return true;
					//Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, cmdLen);
//					SceneUserManager::getMe().unlock();
				}
			}
			else
			{
				Zebra::logger->error("处理用户指令(%d,%d)时,未找到临时编号为%ld的用户",ptNullCmd->cmd , ptNullCmd->para , rev->dwUserID);
//				SceneUserManager::getMe().unlock();
				return true;
			}
		}
	}
	else if(ptNullCmd->cmd==Cmd::Scene::CMD_LOGIN)
	{
		return loginmsgParse(ptNullCmd, cmdLen);
	}
	else if(ptNullCmd->cmd==Cmd::Scene::CMD_FORWARD && ptNullCmd->para==Cmd::Scene::PARA_BILL_FORWARD_SCENE)
	{
		Cmd::Scene::t_Bill_ForwardScene *rev=(Cmd::Scene::t_Bill_ForwardScene*)ptNullCmd;
		SceneUser *pUser=SceneUserManager::getMe().getUserByID(rev->dwUserID);
		if(pUser)
		{
			return usermsgParseBill(pUser,(Cmd::t_NullCmd *)rev->data,rev->size);
		}
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, cmdLen);
	return false;
}

