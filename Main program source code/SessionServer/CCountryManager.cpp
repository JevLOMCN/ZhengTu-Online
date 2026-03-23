/**
 * \file
 * \version  $Id: CCountryManager.cpp  $
 * \author  
 * \date 
 * \brief 国家管理器
 *
 * 
 */

#include <sstream>
#include <math.h>
#include "CCityManager.h"
#include "CCountryManager.h"
#include "zDBConnPool.h"
#include "zMetaData.h"
#include "SessionServer.h"
#include "SessionCommand.h"
#include "CUnion.h"
#include "CDare.h"
#include "CDareRecordManager.h"
#include "SessionChat.h"
#include "SessionChat.h"
#include "CSept.h"
#include "SessionTaskManager.h"
#include "TimeTick.h"
#include "CVote.h"
#include "CArmy.h"
#include "CGem.h"
#include "CNpcDare.h"
#include "EmperorForbid.h"
#include "CHero.h"

CCountryM::CCountryM()
{
	isBeging = false;
	clearForbidTime = 0;
}

/**
 * \brief 国家管理器初始化
 * \return true 初始化成功 false初始化失败
 */
bool CCountryM::init()
{
	return this->load();
}

/**
 * \brief 析构管理器
 */
void CCountryM::destroyMe()
{
	for (unsigned int i=0; i<CCountryM::getMe().countries.size(); i++)
	{
		CCountryM::getMe().countries[i]->writeDatabase();
	}
}

/**
 * \brief 析构管理器
 */
void CCountryM::save()
{
	this->destroyMe();
}


/**
 * \brief 从数据库中加载国家记录
 * \return true 加载成功
 */
bool CCountryM::load()
{
	//FunctionTimes times(49,__FUNCTION__);
	FieldSet* country = SessionService::metaData->getFields("COUNTRY");
	
	if (country)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return false;
		}

		RecordSet* recordset = NULL;

		if ((connHandleID)-1 != handle)
		{
			recordset = SessionService::dbConnPool->exeSelect(handle, country, NULL, NULL);
		}

		SessionService::dbConnPool->putHandle(handle);

		if (recordset)
		{
			for (unsigned int i=0; i<recordset->size(); i++)
			{
				Record* rec = recordset->get(i);
				CCountry* pCountry = new CCountry();

				if (pCountry)
				{
					pCountry->init(rec);
					pCountry->loadTechFromDB();

					if (CUnionM::getMe().getUnionByID(pCountry->dwKingUnionID) == NULL)
					{//如果拥有者
						pCountry->dwKingUnionID = 0;
						bzero(pCountry->kingName, sizeof(pCountry->kingName));
						Zebra::logger->trace("帮会已不存在，%s:国王清空", pCountry->name);
					}
				}
					
				countries.push_back(pCountry);
			}

			SAFE_DELETE(recordset)
		}
	}
	else
	{
		Zebra::logger->error("国家数据加载失败，COUNTRY表不存在");
		return false;
	}

	return true;
}

CCountry* CCountryM::addNewCountry(DWORD country)
{
	CCountry* pCountry = new CCountry();
	rwlock.wrlock();

	if (pCountry)
	{
		pCountry->dwID = country;

		if (pCountry->insertDatabase())
		{
			countries.push_back(pCountry);
		}
		else
		{
			SAFE_DELETE(pCountry);
		}
	}

	rwlock.unlock();
	
	return pCountry;
}


CCountry* CCountryM::find(DWORD country, DWORD unionid)
{
	CCountry* pCountry = NULL;
	
	rwlock.rdlock();
	for (unsigned int i=0; i<countries.size(); i++)
	{
		if (countries[i]->dwID==country && countries[i]->dwKingUnionID == unionid)
		{
			pCountry = countries[i];
			break;
		}
	}
	rwlock.unlock();
	
	return pCountry;
}

CCountry* CCountryM::find(DWORD country)
{
	CCountry* pCountry = NULL;
	
	rwlock.rdlock();
	for (unsigned int i=0; i<countries.size(); i++)
	{
		if (countries[i]->dwID==country)
		{
			pCountry = countries[i];
			break;
		}
	}
	
	rwlock.unlock();
	
	return pCountry;
}

CCountry* CCountryM::findByDare(DWORD country, bool findDare)
{
	CCountry* pCountry = NULL;
	bool isDare = false;	
	DWORD dwDefCountryID = 0;
	
	rwlock.rdlock();
	for (unsigned int i=0; i<countries.size(); i++)
	{
		if (countries[i]->dwDareCountryID == country)
		{
			isDare = true;
			dwDefCountryID = countries[i]->dwID;
			break;
		}
	}
	rwlock.unlock();
	if (findDare)
	{
		if (isDare)
		{
			pCountry = this->find(country);
		}
	}
	else
	{
		if (isDare)
		{
			pCountry = this->find(dwDefCountryID);
		}
	}
	
	return pCountry;
}

void CCountryM::timerPerHour()
{
	struct tm tv1;
	time_t timValue = time(NULL);
	zRTime::getLocalTime(tv1, timValue);

	std::vector<CCountry*>::iterator vIterator;
	rwlock.rdlock();

	for(vIterator = countries.begin(); vIterator!=countries.end(); vIterator++)
	{
		if (tv1.tm_wday == 1 && CVoteM::getMe().find((*vIterator)->dwID, Cmd::TECH_VOTE)==NULL)
		{
			(*vIterator)->beginTechVote();
		}
	}

	rwlock.unlock();
}

void CCountry::setWinnerExp()
{
	winner_exp = true;
	winner_time = 1;
	Cmd::Session::t_updateWinnerExp_SceneSession send;
	send.countryID = dwID;
	send.type = winner_exp;
	SessionTaskManager::getInstance().broadcastScene(&send, sizeof(send));
}
void CCountry::checkWinnerExp()
{
	if(winner_time)
		winner_time++;
	if(winner_time >= 60 * 3)
	{
		winner_time= 0;
		winner_exp = false;
		Cmd::Session::t_updateWinnerExp_SceneSession send;
		send.countryID = dwID;
		send.type = winner_exp;
		SessionTaskManager::getInstance().broadcastScene(&send, sizeof(send));
	}
}
void CCountryM::timer()
{
	//检测战胜国经验加成标志
	std::vector<CCountry*>::iterator iter;
	rwlock.rdlock();
	for(iter = countries.begin(); iter!=countries.end(); iter++)
	{
		(*iter)->checkWinnerExp();
		if ((*iter)->gen_refreshTime && (*iter)->gen_refreshTime<=SessionTimeTick::currentTime.sec())
			refreshGeneral((*iter)->dwID);
	}	
	rwlock.unlock();


	struct tm tv1;
	time_t timValue = time(NULL);
	zRTime::getLocalTime(tv1, timValue);
	static time_t lastMessage = 0;
		
	if (tv1.tm_hour==20 && tv1.tm_min>=10)
	{
		if (tv1.tm_min<35 && timValue-lastMessage > 10*60)
		{
			std::vector<CCountry*>::iterator vIterator;
			rwlock.rdlock();

			for(vIterator = countries.begin(); vIterator!=countries.end(); vIterator++)
			{
				if ((*vIterator)->dwDareCountryID>0 && (timValue-(*vIterator)->dwLastDareTime > 24*60*60))
				{
					CCountry* pCountry = this->find((*vIterator)->dwDareCountryID);
					if (pCountry)
					{
					SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, (*vIterator)->dwID, 
							"还剩 %d 分钟 %s　将来攻打我国王城",
							abs(40-tv1.tm_min), pCountry->name);

					SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, (*vIterator)->dwDareCountryID, 
							"还剩 %d 分钟 我国将去攻打 %s 王城",
							abs(40-tv1.tm_min), (*vIterator)->name);
					}
				}
			}	
			rwlock.unlock();
			
			lastMessage = timValue;
		}

		if (tv1.tm_min>=35 && tv1.tm_min<40 && timValue-lastMessage > 60)
		{
			std::vector<CCountry*>::iterator vIterator;
			rwlock.rdlock();

			for(vIterator = countries.begin(); vIterator!=countries.end(); vIterator++)
			{

				if ((*vIterator)->dwDareCountryID>0
				&& (timValue-(*vIterator)->dwLastDareTime > 24*60*60))
				{

					CCountry* pCountry = this->find((*vIterator)->dwDareCountryID);
					if (pCountry)
					{
					SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, (*vIterator)->dwID, 
							"还剩 %d 分钟 %s　将来攻打我国王城",
							abs(40-tv1.tm_min), pCountry->name);

					SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, (*vIterator)->dwDareCountryID, 
							"还剩 %d 分钟 我国将去攻打 %s 王城",
							abs(40-tv1.tm_min), (*vIterator)->name);
					}
				}
			}	
			rwlock.unlock();
			
			lastMessage = timValue;
		}
	}
	
	//恋恋屏蔽 国战 皇城战
	if (tv1.tm_hour==20 && (tv1.tm_min>=40 && tv1.tm_min<43) && !isBeging)
	{// 到八点开始新建正式国战对战对象
		this->beginDare();
		isBeging = true; // 在晚上十点强制结束还存在的对战时，重置为false
	}

	if (tv1.tm_hour>=20 && tv1.tm_hour<=22)
	{
		if (tv1.tm_min%10 == 0 && timValue-lastMessage>60)
		{
			std::vector<CCountry*>::iterator vIterator;
			
			for(vIterator = countries.begin(); vIterator!=countries.end(); vIterator++)
			{
				if ((*vIterator)->dwDareCountryID>0
				&& (timValue-(*vIterator)->dwLastDareTime > 24*60*60))
				{
					CCountry* pCountry = this->find((*vIterator)->dwDareCountryID);
					if (pCountry)
					{
						SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, (*vIterator)->dwID, 
								"%s 正在围攻我国的王城,请回城防御或反攻解围",
								pCountry->name);

						SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, 
								(*vIterator)->dwDareCountryID, 
						"我国攻打 %s 的战斗正在进行,各位爱国志士可以通过边防官到 %s 参加战斗",
								(*vIterator)->name, (*vIterator)->name);
					}
				}
			}	
			lastMessage = timValue;
		}
	}

	if (tv1.tm_hour==22 && tv1.tm_min>=40 && tv1.tm_min<44)
	{
		isBeging = false;
		this->endDare();
	}

	if (tv1.tm_hour==0 && tv1.tm_min<5 && SessionTimeTick::currentTime.sec()>clearForbidTime)
	{
		clearForbid();
		clearForbidTime = SessionTimeTick::currentTime.sec();
		clearForbidTime += 86000;
		this->resetCallTimes();
		this->clearCensor();   //soke 外交官（停用）
		this->clearCatcher();    //soke 巡捕（左上）
		this->clearCatchex();    //soke 巡捕（左下）
		this->clearCensor();     //soke 御史大夫（右上）
		this->clearCensos();     //soke 御史大夫（右下）
		this->clearYuanshuai();  //soke 元帅
		this->clearZaixiang();   //soke 宰相
	}
}

bool CCountryM::processUserMessage(UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen)
{
	switch (ptNullCmd->byParam) 
	{
		case REQ_DAILY_EMPEROR_MONEY: //soke 皇帝领取每日薪资
			{
				Cmd::stReqDailyEmperorMoneyCmd* rev = (Cmd::stReqDailyEmperorMoneyCmd*)ptNullCmd;
				this->processReqDailyEmperorMoney(pUser, rev);
				return true;
			}
			break;
		case REQ_DAILY_KING_MONEY: //soke 国王领取每日薪资
			{
				Cmd::stReqDailyKingMoneyCmd* rev = (Cmd::stReqDailyKingMoneyCmd*)ptNullCmd;
				this->processReqDailyKingMoney(pUser, rev);
				return true;
			}
			break;
		case REQ_DAILY_YUANSHUAI_MONEY: //soke 元帅领取每日薪资
			{
				Cmd::stReqDailyYuanshuaiMoneyCmd* rev = (Cmd::stReqDailyYuanshuaiMoneyCmd*)ptNullCmd;
				this->processReqDailyYuanshuaiMoney(pUser, rev);
				return true;
			}
			break;
		case REQ_DAILY_ZAIXIANG_MONEY: //soke 宰相领取每日薪资
			{
				Cmd::stReqDailyZaixiangMoneyCmd* rev = (Cmd::stReqDailyZaixiangMoneyCmd*)ptNullCmd;
				this->processReqDailyZaixiangMoney(pUser, rev);
				return true;
			}
			break;
		case REQ_DAILY_CATCHER_MONEY: //soke 捕头（左）领取每日薪资
			{
				Cmd::stReqDailyCatcherMoneyCmd* rev = (Cmd::stReqDailyCatcherMoneyCmd*)ptNullCmd;
				this->processReqDailyCatcherMoney(pUser, rev);
				return true;
			}
			break;
		case REQ_DAILY_CENSOR_MONEY: //soke 御史大夫（左）领取每日薪资
			{
				Cmd::stReqDailyCensorMoneyCmd* rev = (Cmd::stReqDailyCensorMoneyCmd*)ptNullCmd;
				this->processReqDailyCensorMoney(pUser, rev);
				return true;
			}
			break;
		case REQ_DAILY_CATCHEX_MONEY: //soke 捕头（右）领取每日薪资
			{
				Cmd::stReqDailyCatchexMoneyCmd* rev = (Cmd::stReqDailyCatchexMoneyCmd*)ptNullCmd;
				this->processReqDailyCatchexMoney(pUser, rev);
				return true;
			}
			break;
		case REQ_DAILY_CENSOS_MONEY: //soke 御史大夫（右）领取每日薪资
			{
				Cmd::stReqDailyCensosMoneyCmd* rev = (Cmd::stReqDailyCensosMoneyCmd*)ptNullCmd;
				this->processReqDailyCensosMoney(pUser, rev);
				return true;
			}
			break;
		case CANCEL_DIPLOMAT_PARA://soke 2016 - 3 - 11 撤消外交官（停用）
			{
				Cmd::stCancelDiplomatCmd* rev = (Cmd::stCancelDiplomatCmd*)ptNullCmd;
				processCancelDiplomat(pUser, rev);
				return true;
			}
			break;
		case CANCEL_CATCHER_PARA://soke 2016 - 3 - 11 国王罢免巡捕（左上）
			{
				Cmd::stCancelCatcherCmd* rev = (Cmd::stCancelCatcherCmd*)ptNullCmd;
				processCancelCatcher(pUser, rev);
				return true;
			}
			break;
		case CANCEL_CATCHEX_PARA://soke 2016 - 3 - 11 国王罢免巡捕（左下）
			{
				Cmd::stCancelCatchexCmd* rev = (Cmd::stCancelCatchexCmd*)ptNullCmd;
				processCancelCatchex(pUser, rev);
				return true;
			}
			break;
        case CANCEL_CENSOR_PARA://soke 2016 - 3 - 11 国王罢免御史大夫（右上）
			{
				Cmd::stCancelCensorCmd* rev = (Cmd::stCancelCensorCmd*)ptNullCmd;
				processCancelCensor(pUser, rev);
				return true;
			}
			break;
		case CANCEL_CENSOS_PARA://soke 2016 - 3 - 11 国王罢免御史大夫（右下）
			{
				Cmd::stCancelCensosCmd* rev = (Cmd::stCancelCensosCmd*)ptNullCmd;
				processCancelCensos(pUser, rev);
				return true;
			}
			break;
		case CANCEL_YUANSHUAI_PARA://soke 2016 - 3 - 11 国王罢免元帅
			{
				Cmd::stCancelYuanshuaiCmd* rev = (Cmd::stCancelYuanshuaiCmd*)ptNullCmd;
				processCancelYuanshuai(pUser, rev);
				return true;
			}
			break;
		case CANCEL_ZAIXIANG_PARA://soke 2016 - 3 - 11 国王罢免宰相
			{
				Cmd::stCancelZaixiangCmd* rev = (Cmd::stCancelZaixiangCmd*)ptNullCmd;
				processCancelZaixiang(pUser, rev);
				return true;
			}
			break;
		case APPOINT_DIPLOMAT_PARA://soke 2016 - 3 - 11 任命外交官（停用）
			{
				Cmd::stAppointDiplomatCmd* rev = (Cmd::stAppointDiplomatCmd*)ptNullCmd;
				processSetDiplomat(pUser, rev);
				return true;
			}
			break;
		case APPOINT_CATCHER_PARA://soke 2016 - 3 - 11 国王任命巡捕（左上）
			{
				Cmd::stAppointCatcherCmd* rev = (Cmd::stAppointCatcherCmd*)ptNullCmd;
				processSetCatcher(pUser, rev);
				
				return true;
			}
			break;
		case APPOINT_CATCHEX_PARA://soke 2016 - 3 - 11 国王任命巡捕（左下）
			{
				Cmd::stAppointCatchexCmd* rev = (Cmd::stAppointCatchexCmd*)ptNullCmd;
				processSetCatchex(pUser, rev);
				
				return true;
			}
			break;
		case APPOINT_CENSOR_PARA://soke 2016 - 3 - 11 国王任命御史大夫（右上）
			{
				Cmd::stAppointCensorCmd* rev = (Cmd::stAppointCensorCmd*)ptNullCmd;
				processSetCensor(pUser, rev);
				return true;
			}
			break;
		case APPOINT_CENSOS_PARA://soke 2016 - 3 - 11 国王任命御史大夫（右下）
			{
				Cmd::stAppointCensosCmd* rev = (Cmd::stAppointCensosCmd*)ptNullCmd;
				processSetCensos(pUser, rev);
				return true;
			}
			break;
		case APPOINT_YUANSHUAI_PARA://soke 2016 - 3 - 11 国王任命元帅
			{
				Cmd::stAppointYuanshuaiCmd* rev = (Cmd::stAppointYuanshuaiCmd*)ptNullCmd;
				processSetYuanshuai(pUser, rev);
				return true;
			}
			break;
		case APPOINT_ZAIXIANG_PARA://soke 2016 - 3 - 11 国王任命宰相
			{
				Cmd::stAppointZaixiangCmd* rev = (Cmd::stAppointZaixiangCmd*)ptNullCmd;
				processSetZaixiang(pUser, rev);
				return true;
			}
			break;
		case REQ_COUNTRY_NOTE_PARA:
			{
				CCountry* pCountry = this->find(pUser->country);
				if (pCountry && strncmp(pCountry->note, "", 255))
				{
					Cmd::stCountryNoteCmd send;
					strncpy(send.note, pCountry->note, 255);
					pUser->sendCmdToMe(&send, sizeof(send));
				}
				return true;
			}
			break;
		case COUNTRY_NOTE_PARA:
			{
				Cmd::stCountryNoteCmd * rev = (Cmd::stCountryNoteCmd *)ptNullCmd;
				CCountry* pCountry = this->find(pUser->country);
				if (pCountry && pCountry->isKing(pUser))
				{
					strncpy(pCountry->note, rev->note, 255);

					SessionChannel::sendCountry(pUser->country, rev, sizeof(Cmd::stCountryNoteCmd));
				}
				return true;
			}
			break;
		case ANTI_DARE_COUNTRY_FORMAL_PARA:
			{
				Cmd::stAntiDareCountryFormalCmd* rev = (Cmd::stAntiDareCountryFormalCmd*)ptNullCmd;
				processAntiDareCountry(pUser, rev);
				return true;
			}
			break;
		case CANCEL_TECH_SEARCH_PARA:
			{
				Cmd::stCancelTechSearchUserCmd* rev = (Cmd::stCancelTechSearchUserCmd*)ptNullCmd;
				processCancelTechSearch(pUser, rev);
				return true;
			}
			break;
		case REQ_WAIT_OFFICIAL_PARA:
			{
				Cmd::stReqWaitOfficialUserCmd* rev = (Cmd::stReqWaitOfficialUserCmd*)ptNullCmd;
				processReqWaitOfficial(pUser, rev);
				return true;
			}
			break;
		case CONFIRM_SEARCHER_PARA:
			{
				Cmd::stConfirmSearcherUserCmd* rev = (Cmd::stConfirmSearcherUserCmd*)ptNullCmd;
				processConfirmSearcher(pUser, rev);
				return true;
			}
			break;
		case SET_TECH_SEARCH_PARA:
			{
				Cmd::stSetTechSearchUserCmd* rev = (Cmd::stSetTechSearchUserCmd*)ptNullCmd;
				processSetTechSearch(pUser, rev);
				return true;
			}
			break;
		case UP_TECH_DEGREE_PARA:
			{
				Cmd::stUpTechDegreeUserCmd* rev = (Cmd::stUpTechDegreeUserCmd*)ptNullCmd;
				processUpTech(pUser, rev);
				return true;
			}
			break;
		case  REQ_TECH_PARA:
			{
				Cmd::stReqTechUserCmd *rev = (Cmd::stReqTechUserCmd *)ptNullCmd;
				processRequestTech(pUser, rev);
				return true;
			}
			break;
		case Cmd::SELECT_TRANS_LEVEL:
			{
				Cmd::stSelectTransLevelUserCmd* rev = (Cmd::stSelectTransLevelUserCmd*)ptNullCmd;

				// TODO，检查是否是国王，并检查是否在国战期
				CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);

				if (pUnion)
				{
					if (pUnion->master && (pUnion->master->id == pUser->id))
					{
						CCity* pCity = CCityM::getMe().find(pUser->country, KING_CITY_ID); // 王城
						CCountry* pCountry = this->find(pUser->country);

						if (pCountry && pCity)
						{
							if (pCity->dwUnionID == pUser->unionid)
							{//是国王
								 CDare* pDare = CDareM::getMe().findDareRecordByID(
							Cmd::COUNTRY_FORMAL_DARE, pUser->country);

								if (pDare)
								{
									Cmd::Session::t_selTransCountryWar_SceneSession send;
									send.dwCountryID = pUser->country;
									send.dwLevel = rev->dwLevel;
									SessionTaskManager::getInstance().broadcastScene(&send, sizeof(send));
									pUser->sendSysChat(Cmd::INFO_TYPE_MSG, 
									"你已经成功点燃烽火台。此刻你的国家的各路英雄好汉正在边境处等待着你的一声令下。");

								}
								else
								{
									pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
										"你不在国战状态，不能点燃烽火台");
								}
							}
							else
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
										"你不是国王，不能点燃烽火台");
							}
						}
						else
						{
							pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
									"你不是国王，不能点燃烽火台");
						}
					}
					else
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是国王，不能点燃烽火台");
					}
				}
				else
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是国王，不能点燃烽火台");
				}
				return true;
			}
			break;
		case KING_PUNISH_COUNTRY_PARA:
			{
				if (!pUser) return false;

				CCountry* pCountry = find(pUser->country);
				if (!pCountry) return false;

				if (strcmp(pUser->name, pCountry->kingName)) return true;

				if (pCountry->sendPrison!=0)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你今天已经关押过一个玩家");
					return true;
				}

				Cmd::stKingPunishCountryCmd * rev = (Cmd::stKingPunishCountryCmd *)ptNullCmd;
				if (0==strncmp(rev->name, pUser->name, MAX_NAMESIZE))
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能关押自己");
					return true;
				}

				UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(rev->name);
				if (!u)
				{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", rev->name);
						return true;
				}

				if (pUser->country!=u->country)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你只能关押自己的国民");
					return true;
				}

				Cmd::Session::t_countryPunishUser_SceneSession send;
				strncpy(send.name, rev->name, MAX_NAMESIZE);
				send.method = 2;
				u->scene->sendCmd(&send, sizeof(send));

				Zebra::logger->trace("用户%s(%ld)被国王关进监狱",u->name,u->id);
				SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME,pUser->country,"%s 被国王 %s 关押一小时",u->name,pUser->name);
				pCountry->sendPrison = 1;

				return true;
			}
			break;
		case FORBID_TALK_COUNTRY_PARA:
			{
				if (!pUser) return false;

				CCountry* pCountry = find(pUser->country);
				if (!pCountry) return false;

				if (strcmp(pUser->name, pCountry->kingName)) return true;

				if (pCountry->forbidTalk!=0)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你今天已经禁言过一个玩家");
					return true;
				}

				Cmd::stForbidTalkCountryUserCmd * rev = (Cmd::stForbidTalkCountryUserCmd *)ptNullCmd;
				if (0==strncmp(rev->name, pUser->name, MAX_NAMESIZE))
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能禁言自己");
					return true;
				}

				UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(rev->name);
				if (!u)
				{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", rev->name);
						return true;
				}

				if (pUser->country!=u->country)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你只能禁言自己的国民");
					return true;
				}

				Cmd::Session::t_countryPunishUser_SceneSession send;
				strncpy(send.name, rev->name, MAX_NAMESIZE);
				send.method = 1;
				u->scene->sendCmd(&send, sizeof(send));

				SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, pUser->country, "%s 被国王 %s 禁言一小时", u->name, pUser->name);
				pCountry->forbidTalk = 1;

				return true;
			}
			break;
		case EMPEROR_PUNISH_COUNTRY_PARA:
			{
				if (!pUser) return false;

				if (!CCountryM::getMe().isEmperor(pUser)) return true;

				if (EmperorForbid::getMe().count()>=10)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你今天已经禁言过 10 个玩家");
					return true;
				}

				Cmd::stEmperorPunishCountryCmd * rev = (Cmd::stEmperorPunishCountryCmd *)ptNullCmd;
				if (0==strncmp(rev->name, pUser->name, MAX_NAMESIZE))
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能处罚自己");
					return true;
				}

				UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(rev->name);
				if (!u)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", rev->name);
					return true;
				}

				if (1==rev->method)//禁言
				{
					if (!EmperorForbid::getMe().add(u->id))
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你今天已经禁言 %s 一次了", rev->name);
						return true;
					}

					Cmd::Session::t_countryPunishUser_SceneSession send;
					strncpy(send.name, rev->name, MAX_NAMESIZE);
					send.method = 1;
					u->scene->sendCmd(&send, sizeof(send));

					SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, "%s 被皇帝 %s 禁言一小时", u->name, pUser->name);
				}
				return true;
			}
			break;
		case Cmd::TRANS_DARE_COUNTRY_PARA:
			{
				//TODO：取出要去的战场国家，判断要去的国家ID和挑战的ID是否有与该玩家相同的。有则是去参战国,否则就要收取一两银子。向场景发送转送命令
				/*Cmd::stTransDareCountryCmd* rev = (Cmd::stTransDareCountryCmd*)ptNullCmd;
				CCountry* pCountry = CCountryM::getMe().find(rev->dwCountryID);	
				Cmd::Session::t_transDareCountry_SceneSession	send;
				send.dwUserID = pUser->id;

				if (pCountry->dwDareCountryID>0 && isBeging)
				{
					if (pCountry->dwID==pUser->country || pCountry->dwDareCountryID==pUser->country)
					{
						send.dwMoney = 0;
					}
					else
					{
						send.dwMoney = 100; // 1两
					}

					send.dwCountry = rev->dwCountryID;
					if (pUser->scene) pUser->scene->sendCmd(&send, sizeof(send));
				}
				else
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不在国战状态，不允许使用该功能。");
				}*/
				
				return true;
			}
			break;
		case Cmd::REQUEST_DARE_COUNTRY_PARA:
			{
				Cmd::stRequestDareCountryCmd *rev = (Cmd::stRequestDareCountryCmd *)ptNullCmd;
				processRequestDare(pUser, rev);
				return true;
			}
			break;
		case Cmd::DARE_COUNTRY_FORMAL_PARA:
			{
				Cmd::stDareCountryFormalCmd* rev = (Cmd::stDareCountryFormalCmd*)ptNullCmd;
				processDareCountry(pUser, rev);
				return true;
			}
			break;
		case Cmd::REQUEST_DARE_RECORD_PARA:
			{
				Cmd::stRequestDareRecordCmd* rev = (Cmd::stRequestDareRecordCmd*)ptNullCmd;
				if (rev->byType == Cmd::DARE_RECORD_RESULT)
				{
					return CDareRecordM::getMe().processUserMessage(pUser, ptNullCmd, cmdLen);
				}
				else if (rev->byType == Cmd::DARE_RECORD_STAT)
				{
					BYTE buf[zSocket::MAX_DATASIZE];
					Cmd::stDareStat *tempStat = NULL;

					std::vector<CCountry*>::iterator vIterator;

					rwlock.rdlock();
					Cmd::stReturnDareRecordStatCmd *retCmd=(Cmd::stReturnDareRecordStatCmd *)buf;

					constructInPlace(retCmd);
					tempStat = (Cmd::stDareStat *)retCmd->dare_stat;
					retCmd->dwSize = 0;
					
					for (vIterator = countries.begin(); vIterator!=countries.end(); vIterator++)
					{
						tempStat->dwCountry = (*vIterator)->dwID;
						tempStat->dwFormalWin = (*vIterator)->dwFormalWin;
						tempStat->dwFormalFail = (*vIterator)->dwFormalFail;
						tempStat->dwAnnoyWin = (*vIterator)->dwAnnoyWin;
						tempStat->dwAnnoyFail = (*vIterator)->dwAnnoyFail;
							
						tempStat++;
						retCmd->dwSize++;
					}
					rwlock.unlock();

					pUser->sendCmdToMe(retCmd, 
					(retCmd->dwSize*sizeof(Cmd::stDareStat)+sizeof(Cmd::stReturnDareRecordStatCmd)));
					
				}
				else if (rev->byType == Cmd::DARE_RECORD_PLAN)
				{
					BYTE buf[zSocket::MAX_DATASIZE];
					Cmd::stDarePlan *tempPlan = NULL;

					std::vector<CCountry*>::iterator vIterator;

					rwlock.rdlock();
					Cmd::stReturnDareRecordPlanCmd  *retCmd=(Cmd::stReturnDareRecordPlanCmd *)buf;

					constructInPlace(retCmd);
					tempPlan = (Cmd::stDarePlan *)retCmd->dare_plan;
					retCmd->dwSize = 0;
					
					for (vIterator = countries.begin(); vIterator!=countries.end(); vIterator++)
					{
						if ((*vIterator)->dwDareCountryID > 0)
						{
							tempPlan->attCountry = (*vIterator)->dwDareCountryID;
							tempPlan->defCountry = (*vIterator)->dwID;
							tempPlan->planTime = (*vIterator)->dwLastDareTime;
							tempPlan++;
							retCmd->dwSize++;
						}
					}
					
					rwlock.unlock();

					pUser->sendCmdToMe(retCmd, 
					(retCmd->dwSize*sizeof(Cmd::stDarePlan)+sizeof(Cmd::stReturnDareRecordPlanCmd)));

				}
				
				return true;
			}
			break;
		case Cmd::TAX_COUNTRY_PARA:
			{
				Cmd::stTaxCountryUserCmd* rev = (Cmd::stTaxCountryUserCmd*)ptNullCmd;
				CCountry * pCountry = this->find(rev->dwCountry);
				if (pCountry)
				{
					Cmd::stTaxCountryUserCmd send;
					send.byTax = (BYTE)pCountry->dwTax;
					send.dwCountry = pCountry->dwID;
					pUser->sendCmdToMe(&send, sizeof(send));
				}
				return true;
			}
			break;
		case Cmd::FISK_COUNTRY_PARA:
			{
				Cmd::stFiskCountryUserCmd* rev = (Cmd::stFiskCountryUserCmd*)ptNullCmd;
				CCountry * pCountry = this->find(rev->dwCountry);
				if (pCountry)
				{
					Cmd::stFiskCountryUserCmd send;
					send.qwGold = pCountry->qwGold;
					send.qwStock = pCountry->qwStock;
					send.qwMaterial = pCountry->qwMaterial;
					send.dwCountry = pCountry->dwID;

					pUser->sendCmdToMe(&send, sizeof(send));
				}
				return true;
			}
			break;
		case Cmd::SETTAX_COUNTRY_PARA:
			{
				Cmd::stSetTaxCountryUserCmd* rev = (Cmd::stSetTaxCountryUserCmd*)ptNullCmd;
				CCountry * pCountry = this->find(rev->dwCountry);
				if (pCountry)
				{
					CCity *pCity = CCityM::getMe().find(pUser->country, KING_CITY_ID);
					if (pCity != NULL && pCity->dwUnionID!=0)
					{
						CUnion *pUnion = CUnionM::getMe().getUnionByID(pCity->dwUnionID);
						if (pUnion)
						{
							if (pUnion->master)
							{
								if (pUnion->master->id == pUser->id) //终于确定他是国王
								{
									if (rev->byTax>=20)
									{
										pCountry->dwTax = 20;
									}
									else
									{
										pCountry->dwTax = rev->byTax;
									}
									pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "税率已经调整成%d\%", pCountry->dwTax);
									SceneSessionManager::getInstance()->notifyCountryTax(pCountry->dwID,pCountry->dwTax);

									Cmd::stTaxCountryUserCmd send;
									send.byTax = (BYTE)pCountry->dwTax;
									send.dwCountry = pCountry->dwID;
									SessionTaskManager::getInstance().sendCmdToWorld(&send,sizeof(send));
									return true;
								}
							}
						}
					}
				}
				pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "税率设置失败!");
				return true;
			}
			break;
		case REQ_GEN_COUNTRY_PARA:
			{
				if (!pUser) return true;
				CCountry * pCountry = find(pUser->country);
				if (!pCountry) return false;

				Cmd::stRetGenCountryCmd send;
				send.level = pCountry->gen_level;
				send.exp = pCountry->gen_exp;
				send.maxExp = pCountry->gen_maxexp;

				pUser->sendCmdToMe(&send, sizeof(send));
				return true;
			}
			break;
		case REQ_KING_LIST_PARA:
			{
				using namespace Cmd;
				char buffer[zSocket::MAX_USERDATASIZE];
				stRtnKingListCmd *send = (stRtnKingListCmd*)buffer;
				constructInPlace(send);
				std::vector<CCountry*>::iterator vIterator;
				for (vIterator = countries.begin(); vIterator!=countries.end(); vIterator++)
				{
					send->data[send->size].country_id = (*vIterator)->dwID;
					if((*vIterator)->kingtime)
					{
						send->data[send->size].online_time=(SessionTimeTick::currentTime.sec()-(*vIterator)->kingtime)/3600;
					}
					else
					{
						send->data[send->size].online_time=0;
					}

					strncpy(send->data[send->size].king_name,(*vIterator)->name , MAX_NAMESIZE);
					send->size++;
				}
				pUser->sendCmdToMe(send,sizeof(stRtnKingListCmd) + send->size * sizeof(_KingListItem));
				return true;
			}
			break;
		case REQ_CITY_OWNER_LIST_PARA:
			{
				using namespace Cmd;
				struct CityOwnerCallback : public CCityM::cityCallback
				{
					stRtnCityOwnerListCmd *_send;
					CityOwnerCallback(stRtnCityOwnerListCmd *send):_send(send)
					{
					}
					void exec(CCity * pCity)
					{
						if (pCity->getName()!=NULL)
						{
							CUnion *pUnion = CUnionM::getMe().getUnionByID(pCity->dwUnionID);
							if(pUnion)
							{
								strncpy(_send->list[_send->size].cityName,pCity->getName(),MAX_NAMESIZE);
								strncpy(_send->list[_send->size].unionName,pUnion->name,MAX_NAMESIZE);
								_send->size++;
							}
						}
					}
				};
				char buffer[zSocket::MAX_USERDATASIZE];
				stRtnCityOwnerListCmd * send = (stRtnCityOwnerListCmd*)buffer;
				constructInPlace(send);
				CityOwnerCallback oc(send);
				CCityM::getMe().execEveryCity(oc);
				if(send->size)
				{
					pUser->sendCmdToMe(send,sizeof(stRtnCityOwnerListCmd) + send->size * sizeof(send->list[0]));
				}
				return true;
			}
			break;
		case REQ_NPC_OWNER_LIST_PARA:
			{
				using namespace Cmd;
				struct NpcOwnerCallback : public execEntry<CNpcDareObj>
				{
					stRtnNpcOwnerListCmd *_send;
					DWORD _country;
					NpcOwnerCallback(stRtnNpcOwnerListCmd *send,DWORD country):_send(send),_country(country)
					{
					}
					bool exec(CNpcDareObj * pNpc)
					{
						CSept * pSept = CSeptM::getMe().getSeptByID(pNpc->get_holdseptid());
						if(pSept)
						{
							SceneSession * scene = SceneSessionManager::getInstance()->getSceneByID((pNpc->get_country()<<16)+pNpc->get_mapid());
							if(scene)
							{
								strncpy(_send->list[_send->size].septName,pSept->name,MAX_NAMESIZE);
								strncpy(_send->list[_send->size].mapName,scene->name,MAX_NAMESIZE);
								_send->list[_send->size].x=pNpc->get_posx();
								_send->list[_send->size].y=pNpc->get_posy();
								_send->list[_send->size].npcID=pNpc->get_npcid();
								_send->size++;
							}
						}
						return true;
					}
				};
				 
				char buffer[zSocket::MAX_USERDATASIZE];
				stRtnNpcOwnerListCmd * send = (stRtnNpcOwnerListCmd*)buffer;
				constructInPlace(send);
				NpcOwnerCallback nb(send,pUser->country);
				CNpcDareM::getMe().execEveryOne(nb);
				if(send->size)
				{
					pUser->sendCmdToMe(send,sizeof(stRtnNpcOwnerListCmd) + send->size * sizeof(send->list[0]));
				}
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

bool CCountryM::processSceneMessage(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen)
{
	switch (ptNullCmd->para) 
	{
		case Cmd::Session::PARA_CHECK_USER: //soke 2016 - 3 - 11 检测善恶度巡捕（左上）
			{
				Cmd::Session::t_checkUser_SceneSession* rev = 
					(Cmd::Session::t_checkUser_SceneSession*)ptNullCmd;

				UserSession* pUser  = UserSessionManager::getInstance()->getUserByID(rev->dwCheckedID);
				
				if (pUser)
				{
					CCountry* pCountry = this->find(pUser->country);
					if (pCountry)
					{
						pCountry->changeCatcher(pUser);
						
						UserSession* u  = UserSessionManager::getInstance()->getUserByID(rev->dwCheckID);
						if (u)
						{
							u->sendSysChat(Cmd::INFO_TYPE_MSG, "成功任命 %s 为捕头", pUser->name);
						}
					}
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_CHECK_USER_CATCHEX: //soke 2016 - 3 - 11 检测善恶度巡捕（左下）
			{
				Cmd::Session::t_checkUserCatchex_SceneSession* rev = 
					(Cmd::Session::t_checkUserCatchex_SceneSession*)ptNullCmd;

				UserSession* pUser  = UserSessionManager::getInstance()->getUserByID(rev->dwCheckedID);
				
				if (pUser)
				{
					CCountry* pCountry = this->find(pUser->country);
					if (pCountry)
					{
						pCountry->changeCatchex(pUser);
						
						UserSession* u  = UserSessionManager::getInstance()->getUserByID(rev->dwCheckID);
						if (u)
						{
							u->sendSysChat(Cmd::INFO_TYPE_MSG, "成功任命 %s 为捕头", pUser->name);
						}
					}
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_ALLY_NPC_CLEAR:
			{
				Cmd::Session::t_allyNpcClear_SceneSession* rev =
					(Cmd::Session::t_allyNpcClear_SceneSession*)ptNullCmd;
				
				CAlly* pAlly = CAllyM::getMe().findAlly(rev->dwCountryID);
				CCountry* pCountry = this->find(rev->dwCountryID);
				
				if (pAlly)
				{
					pAlly->changeFriendDegree(50);
					CCountry* pAllyCountry = NULL;

					if (pAlly->dwCountryID == rev->dwCountryID)
					{
						pAllyCountry = this->find(pAlly->dwAllyCountryID);
					}
					else
					{
						pAllyCountry = this->find(pAlly->dwCountryID);
					}

					if (pAllyCountry) {
						pAllyCountry->changeMaterial(COUNTRY_MONEY, COUNTRY_ALLY_NPC_HORTATION_MONEY);
						pAllyCountry->changeMaterial(COUNTRY_MATERIAL, COUNTRY_ALLY_NPC_HORTATION_MATERIAL);
					}
				}

				if (pCountry)
				{
					pCountry->changeMaterial(COUNTRY_MONEY, COUNTRY_ALLY_NPC_HORTATION_MONEY);
					pCountry->changeMaterial(COUNTRY_MATERIAL, COUNTRY_ALLY_NPC_HORTATION_MATERIAL);
				}

				return true;
			}
			break;
		case Cmd::Session::PARA_OP_ALLY_FRIENDDEGREE:
			{
				Cmd::Session::t_opAllyFrienddegree_SceneSession* rev = 
					(Cmd::Session::t_opAllyFrienddegree_SceneSession*)ptNullCmd;

				CAlly* pAlly = CAllyM::getMe().findAlly(rev->dwCountryID);
				if (pAlly)
				{
					pAlly->changeFriendDegree(rev->friendDegree);
					if (pAlly->friendDegree() == 0)
					{
						CAllyM::getMe().fireAlly(pAlly->dwCountryID, pAlly->dwAllyCountryID);
					}
				}
			}
			break;
		case Cmd::Session::PARA_OP_TECH_VOTE:
			{
				Cmd::Session::t_opTechVote_SceneSession* rev = 
					(Cmd::Session::t_opTechVote_SceneSession*)ptNullCmd;

				if (rev->byOpType == 1)
				{
					CCountry* pCountry = this->find(rev->dwCountryID);

					if (pCountry)
					{
						pCountry->beginTechVote();
					}
				}
				else
				{
					CVoteM::getMe().force_close_vote(rev->dwCountryID, Cmd::TECH_VOTE);
				}
				
				return true;
			}
			break;
		case Cmd::Session::PARA_CONTRIBUTE_COUNTRY:
			{
				Cmd::Session::t_ContributeCountry_SceneSession* rev =
					(Cmd::Session::t_ContributeCountry_SceneSession*)ptNullCmd;

				CCountry* pCountry = find(rev->dwCountry);

				if (pCountry)
				{
					if (rev->byType !=3)
					{
						pCountry->changeMaterial(rev->byType, rev->dwValue);
						pCountry->writeDatabase();
					}
					else if (rev->byType == 3)
					{
						CAlly* pAlly = CAllyM::getMe().findAlly(rev->dwCountry);
						if (pAlly){
							pAlly->changeFriendDegree(rev->dwValue);
							if (0 == pAlly->friendDegree()) {
								CAllyM::getMe().fireAlly(rev->dwCountry, 0);	
							}
						}
					}

					pCountry->addGeneralExp(rev->dwValue);
				}

				return true;
			}
			break;
		case Cmd::Session::PARA_VIEW_COUNTRY_DARE:
			{
				Cmd::Session::t_viewCountryDare_SceneSession* rev = 
					(Cmd::Session::t_viewCountryDare_SceneSession*)ptNullCmd;

				UserSession* pUser  = UserSessionManager::getInstance()->getUserByID(rev->dwUserID);
				std::vector<CCountry*>::iterator vIterator;

				rwlock.rdlock();

				for (vIterator = countries.begin(); vIterator!=countries.end(); vIterator++)
				{
					if ((*vIterator)->dwDareCountryID>0)
					{
						CCountry* pAttCountry = 
							CCountryM::getMe().find((*vIterator)->dwDareCountryID);

						if (pAttCountry)
						{
							pUser->sendGmChat(Cmd::INFO_TYPE_GAME, "国战记录:%s　挑战 %s", 
									pAttCountry->name, (*vIterator)->name);
						}
					}
					else
					{
						pUser->sendGmChat(Cmd::INFO_TYPE_GAME, "国战记录: %s 今天无挑战", (*vIterator)->name);
					}
				}

				rwlock.unlock();	
				return true;
			}
			break;
		case Cmd::Session::PARA_COUNTRY_DARE_RESULT:
			{
				Cmd::Session::t_countryDareResult_SceneSession* rev = 
					(Cmd::Session::t_countryDareResult_SceneSession*)ptNullCmd;

				CCountry* pAttCountry = CCountryM::getMe().find(rev->dwAttCountryID);
				CCountry* pDefCountry = CCountryM::getMe().find(rev->dwDefCountryID);

				UserSession* pUser  = UserSessionManager::getInstance()->getUserByID(rev->dwAttUserID);

				if (pAttCountry == NULL || pDefCountry == NULL)		
				{
					return true;
				}

				if (rev->byType == Cmd::Session::COUNTRY_ANNOY_DARE)
				{
					pAttCountry->dwAnnoyWin++;
					pDefCountry->dwAnnoyFail++;

					if (CArmyM::getMe().isCaptain(rev->dwAttUserID))
					{
						pDefCountry->swapMaterialByPer(pAttCountry, 3);
					}
					else
					{
						pDefCountry->swapMaterialByPer(pAttCountry, 2);
					}

					if (pUser)
					{
						SessionChannel::sendAllInfo(Cmd::INFO_TYPE_EXP, 
								" %s %s 砍倒 %s 国旗, %s 成功掠夺 %s 国库百分之三",
								pAttCountry->name, pUser->name, pDefCountry->name,
								pAttCountry->name, pDefCountry->name);

						//soke 2016 - 3 - 11 砍国旗世界深红滚动公告
						SessionChannel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, 
								" %s的玩家%s已将%s国旗斩断, %s颜面尽失, 国威何在？",
								pAttCountry->name, pUser->name, pDefCountry->name,pDefCountry->name);
						SessionChannel::sendAllInfo(Cmd::INFO_TYPE_XG1, 
								" %s的玩家%s已将%s国旗斩断, %s颜面尽失, 国威何在？",
								pAttCountry->name, pUser->name, pDefCountry->name,pDefCountry->name);
					}
					else
					{
						SessionChannel::sendAllInfo(Cmd::INFO_TYPE_EXP, 
								"%s 战胜了 %s, %s 国家物资被 %s 掠夺，%s 3\%的金钱与物资被运到 %s",
								pAttCountry->name, pDefCountry->name,
								pDefCountry->name, pAttCountry->name, 
								pDefCountry->name, pAttCountry->name);

					}

					pAttCountry->writeDatabase();
					pDefCountry->writeDatabase();

					//SessionChannel::sendAllInfo(Cmd::INFO_TYPE_SYS, "%s 战胜了 %s", 
					//		rev->attCountryName, rev->defCountryName);

					CSeptM::getMe().changeAllRepute(pAttCountry->dwID, 4);
					CSeptM::getMe().changeAllRepute(pDefCountry->dwID, -4);
				}
				else if (rev->byType == Cmd::Session::COUNTRY_FORMAL_DARE)
				{
					//TODO:从对战管理器中找到对战对象，进行对战结果计算，并使其进入READY_OVER状态
					CDare* pDare = CDareM::getMe().findDareRecord(Cmd::COUNTRY_FORMAL_DARE,rev->dwAttCountryID, rev->dwDefCountryID);

					//soke 国战胜利奖励
					if (pDare)
					{
						if (pDare->isAtt(rev->dwAttCountryID))
						{//打死大将军的是挑战国
							pDare->grade1 += 2; //挑战国胜，挑战国加两颗星
						}
						else if (pDare->isAtt(rev->dwDefCountryID))
						{
							pDare->grade2 +=2; //反攻成功, 防守国加两颗星
						}

						pDare->setReadyOverState();
					}
				}
				else if (rev->byType == Cmd::Session::COUNTRY_FORMAL_ANTI_DARE)
				{
					CDare*	pDare = CDareM::getMe().findDareRecord(Cmd::COUNTRY_FORMAL_ANTI_DARE,rev->dwAttCountryID, rev->dwDefCountryID);

					if (pDare) {
						if (pDare->isAtt(rev->dwAttCountryID))
						{//打死大将军的是挑战国
							pDare->grade1 += 2; //挑战国胜，挑战国加两颗星
						}
						else if (pDare->isAtt(rev->dwDefCountryID))
						{
							pDare->grade2 +=2; //反攻成功, 防守国加两颗星
						}

						pDare->setReadyOverState();
					}
				}
				else if (rev->byType == Cmd::Session::EMPEROR_DARE)
				{
					CDare* pDare = CDareM::getMe().findDareRecord(Cmd::EMPEROR_DARE, 6, 0);
					pDare->dwWinnerID = rev->dwAttCountryID;
					pDare->setReadyOverState();
				}

				return true;
			}
			break;
		//soke 场景启动时向会话请求国家太庙资源数量
		case Cmd::Session::PARA_REQ_COUNTRY_LOONG_PEARL_NUM:
			{
				Cmd::Session::ReqCountryLoongPearlNum_SceneSession* rev = (Cmd::Session::ReqCountryLoongPearlNum_SceneSession*)ptNullCmd;
                if( !rev ) return true; 
				CCountry* pCountry = CCountryM::getMe().find(rev->countryID);
				if ( NULL == pCountry ) return true;
                pCountry->replyLoongPearlNum(rev->serverID);
                return true;
            }
            break;
		//soke 场景增加或减少太庙资源时同步到会话
        case Cmd::Session::PARA_SYNC_COUNTRY_LOONG_PEARL_NUM:
            {
                Cmd::Session::SyncCountryLoongPearlNum_SceneSession* rev = (Cmd::Session::SyncCountryLoongPearlNum_SceneSession *)ptNullCmd;
                if ( !rev ) return true;
				CCountry* pCountry = CCountryM::getMe().find(rev->countryID);
				if ( NULL == pCountry ) return true;
                pCountry->syncLoongPearlNum(rev->num);
                return true;
            }
            break;
		default:
			break;
	}

	return false;
}

//soke 皇帝领取每日奖励
void CCountryM::processReqDailyEmperorMoney(UserSession* pUser, Cmd::stReqDailyEmperorMoneyCmd* rev)
{
	CCountry* pEmperor = this->find(NEUTRAL_COUNTRY_ID);
	if (pEmperor)
	{
		if (strncmp(pEmperor->kingName, pUser->name, MAX_NAMESIZE) == 0)
		{
			time_t ct = time(NULL);
			if ((ct-pEmperor->dwLastDailyMoney) >= 24*60*60)
			{
				Cmd::Session::t_dareGold_SceneSession send;
				send.dwUserID = pUser->id;
				send.dwNum = 50000; // 5锭
				send.dwType =  Cmd::Session::EMPEROR_GOLD;
				send.dwWarID = 0;

				if (pUser->scene) 
				{       
					pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));
					Zebra::logger->trace("角色 %s 领取了皇帝税金%u文", pUser->name, send.dwNum);
					pEmperor->dwLastDailyMoney = ct;
					pEmperor->writeDatabase();
				}       
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "一天只能领取一次奖励.");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是皇帝不能使用该项功能.");
		}
	}
	else
	{
		Zebra::logger->error("[国家]: 中立国数据不完整.");
	}
}

//soke 国王领取每日薪资
void CCountryM::processReqDailyKingMoney(UserSession* pUser, Cmd::stReqDailyKingMoneyCmd* rev)
{
	CCountry* pKing = this->find(pUser->country);

	if (pKing)
	{
		if (strncmp(pKing->kingName, pUser->name, MAX_NAMESIZE) == 0)
		{
			time_t ct = time(NULL);
			if ((ct-pKing->dwLastKingMoney) >= 24*60*60)
			{
				Cmd::Session::t_dareGold_SceneSession send;
				send.dwUserID = pUser->id;
				send.dwNum = 50000; //soke 这个不是实际给的 5锭
				send.dwType =  Cmd::Session::KING_GOLD;
				send.dwWarID = 0;

				if (pUser->scene) 
				{       
					pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));
                    Zebra::logger->trace("[国家]国王 %s 领取了国王薪资%u文", pUser->name, send.dwNum);

					//soke 增加国家公告	
			        Cmd::Session::t_countryPunishUser_SceneSession send;

			        SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "[系统] 国王 %s 领取了今日薪资，金子500两", pUser->name);

					pKing->dwLastKingMoney = ct;
					pKing->writeDatabase();
				}       
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "一天只能领取一次薪资.");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是国王不能使用该项功能.");
		}
	}
	else
	{
		Zebra::logger->error("[国家]: 国家数据不完整.");
	}
}

//soke 元帅领取每日薪资
void CCountryM::processReqDailyYuanshuaiMoney(UserSession* pUser, Cmd::stReqDailyYuanshuaiMoneyCmd* rev)
{
	CCountry* pYuanshuai = this->find(pUser->country);

	if (pYuanshuai)
	{
		if (strncmp(pYuanshuai->yuanshuaiName, pUser->name, MAX_NAMESIZE) == 0)
		{
			time_t ct = time(NULL);
			if ((ct-pYuanshuai->dwLastYsMoney) >= 24*60*60)
			{
				Cmd::Session::t_dareGold_SceneSession send;
				send.dwUserID = pUser->id;
				send.dwNum = 30000; //soke 这个不是实际给的 3锭
				send.dwType =  Cmd::Session::YS_GOLD;
				send.dwWarID = 0;

				if (pUser->scene) 
				{       
					pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));
                    Zebra::logger->trace("[国家]元帅 %s 领取了元帅薪资%u文", pUser->name, send.dwNum);

					//soke 增加国家公告	
			        Cmd::Session::t_countryPunishUser_SceneSession send;

			        SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "[系统] 元帅 %s 领取了今日薪资，金子300两", pUser->name);

					pYuanshuai->dwLastYsMoney = ct;
					pYuanshuai->writeDatabase();
				}       
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "一天只能领取一次薪资.");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是元帅不能使用该项功能.");
		}
	}
	else
	{
		Zebra::logger->error("[国家]: 国家数据不完整.");
	}
}

//soke宰相领取每日薪资
void CCountryM::processReqDailyZaixiangMoney(UserSession* pUser, Cmd::stReqDailyZaixiangMoneyCmd* rev)
{
	CCountry* pZaixiang = this->find(pUser->country);

	if (pZaixiang)
	{
		if (strncmp(pZaixiang->zaixiangName, pUser->name, MAX_NAMESIZE) == 0)
		{
			time_t ct = time(NULL);
			if ((ct-pZaixiang->dwLastZxMoney) >= 24*60*60)
			{
				Cmd::Session::t_dareGold_SceneSession send;
				send.dwUserID = pUser->id;
				send.dwNum = 30000; //soke 这个不是实际给的 3锭
				send.dwType =  Cmd::Session::ZX_GOLD;
				send.dwWarID = 0;

				if (pUser->scene) 
				{       
					pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));
                    Zebra::logger->trace("[国家]宰相 %s 领取了宰相薪资%u文", pUser->name, send.dwNum);

					//soke 增加国家公告	
			        Cmd::Session::t_countryPunishUser_SceneSession send;

			        SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "[系统] 宰相 %s 领取了今日薪资，金子300两", pUser->name);

					pZaixiang->dwLastZxMoney = ct;
					pZaixiang->writeDatabase();
				}       
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "一天只能领取一次薪资.");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是宰相不能使用该项功能.");
		}
	}
	else
	{
		Zebra::logger->error("[国家]: 国家数据不完整.");
	}
}

//soke 捕头（左）领取每日薪资
void CCountryM::processReqDailyCatcherMoney(UserSession* pUser, Cmd::stReqDailyCatcherMoneyCmd* rev)
{
	CCountry* pCatcher = this->find(pUser->country);

	if (pCatcher)
	{
		if (strncmp(pCatcher->catcherName, pUser->name, MAX_NAMESIZE) == 0)
		{
			time_t ct = time(NULL);
			if ((ct-pCatcher->dwLastCrMoney) >= 24*60*60)
			{
				Cmd::Session::t_dareGold_SceneSession send;
				send.dwUserID = pUser->id;
				send.dwNum = 20000; //soke 这个不是实际给的 2锭
				send.dwType =  Cmd::Session::CR_GOLD;
				send.dwWarID = 0;

				if (pUser->scene) 
				{       
					pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));
                    Zebra::logger->trace("[国家]捕头 %s 领取了捕头薪资%u文", pUser->name, send.dwNum);

					//soke 增加国家公告	
			        Cmd::Session::t_countryPunishUser_SceneSession send;

			        SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "[系统] 捕头 %s 领取了今日薪资，金子200两", pUser->name);

					pCatcher->dwLastCrMoney = ct;
					pCatcher->writeDatabase();
				}       
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "一天只能领取一次薪资.");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是捕头不能使用该项功能.");
		}
	}
	else
	{
		Zebra::logger->error("[国家]: 国家数据不完整.");
	}
}

//soke 御史大夫（左）领取每日薪资
void CCountryM::processReqDailyCensorMoney(UserSession* pUser, Cmd::stReqDailyCensorMoneyCmd* rev)
{
	CCountry* pCensor = this->find(pUser->country);

	if (pCensor)
	{
		if (strncmp(pCensor->censorName, pUser->name, MAX_NAMESIZE) == 0)
		{
			time_t ct = time(NULL);
			if ((ct-pCensor->dwLastDtMoney) >= 24*60*60)
			{
				Cmd::Session::t_dareGold_SceneSession send;
				send.dwUserID = pUser->id;
				send.dwNum = 20000; //soke 这个不是实际给的 2锭
				send.dwType =  Cmd::Session::DT_GOLD;
				send.dwWarID = 0;

				if (pUser->scene) 
				{       
					pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));
                    Zebra::logger->trace("[国家]御史大夫 %s 领取了御史大夫薪资%u文", pUser->name, send.dwNum);

					//soke 增加国家公告	
			        Cmd::Session::t_countryPunishUser_SceneSession send;

			        SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "[系统] 御史大夫 %s 领取了今日薪资，金子200两", pUser->name);

					pCensor->dwLastDtMoney = ct;
					pCensor->writeDatabase();
				}       
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "一天只能领取一次薪资.");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是御史大夫不能使用该项功能.");
		}
	}
	else
	{
		Zebra::logger->error("[国家]: 国家数据不完整.");
	}
}

//soke 捕头（右）领取每日薪资
void CCountryM::processReqDailyCatchexMoney(UserSession* pUser, Cmd::stReqDailyCatchexMoneyCmd* rev)
{
	CCountry* pCatchex = this->find(pUser->country);

	if (pCatchex)
	{
		if (strncmp(pCatchex->catchexName, pUser->name, MAX_NAMESIZE) == 0)
		{
			time_t ct = time(NULL);
			if ((ct-pCatchex->dwLastCxMoney) >= 24*60*60)
			{
				Cmd::Session::t_dareGold_SceneSession send;
				send.dwUserID = pUser->id;
				send.dwNum = 20000; //soke 这个不是实际给的 2锭
				send.dwType =  Cmd::Session::CX_GOLD;
				send.dwWarID = 0;

				if (pUser->scene) 
				{       
					pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));
                    Zebra::logger->trace("[国家]捕头 %s 领取了捕头薪资%u文", pUser->name, send.dwNum);

					//soke 增加国家公告	
			        Cmd::Session::t_countryPunishUser_SceneSession send;

			        SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "[系统] 捕头 %s 领取了今日薪资，金子200两", pUser->name);

					pCatchex->dwLastCxMoney = ct;
					pCatchex->writeDatabase();
				}       
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "一天只能领取一次薪资.");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是捕头不能使用该项功能.");
		}
	}
	else
	{
		Zebra::logger->error("[国家]: 国家数据不完整.");
	}
}

//soke 御史大夫（右）领取每日薪资
void CCountryM::processReqDailyCensosMoney(UserSession* pUser, Cmd::stReqDailyCensosMoneyCmd* rev)
{
	CCountry* pCensos = this->find(pUser->country);

	if (pCensos)
	{
		if (strncmp(pCensos->censosName, pUser->name, MAX_NAMESIZE) == 0)
		{
			time_t ct = time(NULL);
			if ((ct-pCensos->dwLastDxMoney) >= 24*60*60)
			{
				Cmd::Session::t_dareGold_SceneSession send;
				send.dwUserID = pUser->id;
				send.dwNum = 20000; //soke 这个不是实际给的 2锭
				send.dwType =  Cmd::Session::DX_GOLD;
				send.dwWarID = 0;

				if (pUser->scene) 
				{       
					pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));
                    Zebra::logger->trace("[国家]御史大夫 %s 领取了御史大夫薪资%u文", pUser->name, send.dwNum);

					//soke 增加国家公告	
			        Cmd::Session::t_countryPunishUser_SceneSession send;

			        SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "[系统] 御史大夫 %s 领取了今日薪资，金子200两", pUser->name);

					pCensos->dwLastDxMoney = ct;
					pCensos->writeDatabase();
				}       
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "一天只能领取一次薪资.");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是御史大夫不能使用该项功能.");
		}
	}
	else
	{
		Zebra::logger->error("[国家]: 国家数据不完整.");
	}
}

void CCountryM::processUpTech(UserSession* pUser, Cmd::stUpTechDegreeUserCmd* rev)
{
	CCountry* pCountry = this->find(pUser->country);
	if (!pCountry)
	{
		Zebra::logger->error("[国家]:国家信息获取失败,请检查国家信息完整性");
	}

	CTech* pTech = pCountry->getTech(rev->dwOption);
	if (!pTech)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "该项科技不存在，请确认后再升级");
	}

	pTech->upLevel(pUser);
	CCountryM::getMe().broadcastTech(pUser->country);

	Cmd::stReqTechUserCmd send;
	send.dwType = CTech::ACTIVE_TECH;

	CCountryM::getMe().processRequestTech(pUser, &send);
}

void CCountryM::processConfirmSearcher(UserSession* pUser, Cmd::stConfirmSearcherUserCmd* rev)
{
	CCountry* pCountry = this->find(pUser->country);

	if (!pCountry)
	{
		Zebra::logger->error("[国家]:国家信息获取失败,请检查国家信息完整性");
	}

	CTech* pTech = pCountry->getTech(rev->dwOption);
	if (!pTech)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "该项科技不存在，请确认后再升级");
	}

	pTech->setSearcher(pUser);
}

void CCountryM::processReqWaitOfficial(UserSession* pUser, Cmd::stReqWaitOfficialUserCmd* rev)
{
	CCountry* pCountry = this->find(pUser->country);

	if (!pCountry)
	{
		Zebra::logger->error("[国家]:国家信息获取失败,请检查国家信息完整性");
	}

	if (pCountry->isKing(pUser))
	{
		UserSessionManager::getInstance()->sendGraceSort(pUser);
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "您不是国王，不能使用该项功能");
	}
}

//soke 2016 - 3 - 11 罢免外交官（停用）
void CCountryM::processCancelDiplomat(UserSession* pUser, Cmd::stCancelDiplomatCmd* rev)
{
	CCountry* pCountry = this->find(pUser->country);

	if (!pCountry)
	{
		Zebra::logger->error("[国家]:国家信息获取失败,请检查国家信息完整性");
	}

	if (pCountry->isKing(pUser))
	{
		pCountry->cancelDiplomat();
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "罢免外交官成功");
	}
}

//soke 2016 - 3 - 11 国王罢免巡捕（左上）
void CCountryM::processCancelCatcher(UserSession* pUser, Cmd::stCancelCatcherCmd* rev)
{
	CCountry* pCountry = this->find(pUser->country);

	if (!pCountry)
	{
		Zebra::logger->error("[国家]:国家信息获取失败,请检查国家信息完整性");
	}

	if (pCountry->isKing(pUser))
	{
		pCountry->cancelCatcher();
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "罢免捕头成功");

			///////////////////////////////////////////////////////////////////
			//soke 增加国家公告	
			Cmd::Session::t_countryPunishUser_SceneSession send;

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, pUser->country, "奉天承运，国王诏曰：罢免捕头之职，钦此.", pUser->name);
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "奉天承运，国王诏曰：罢免捕头之职，钦此", pUser->name);
			///////////////////////////////////////////////////////////////////
	}
}

//soke 2016 - 3 - 11 国王罢免巡捕（左下）
void CCountryM::processCancelCatchex(UserSession* pUser, Cmd::stCancelCatchexCmd* rev)
{
	CCountry* pCountry = this->find(pUser->country);

	if (!pCountry)
	{
		Zebra::logger->error("[国家]:国家信息获取失败,请检查国家信息完整性");
	}

	if (pCountry->isKing(pUser))
	{
		pCountry->cancelCatchex();
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "罢免捕头成功");

			///////////////////////////////////////////////////////////////////
			//soke 增加国家公告	
			Cmd::Session::t_countryPunishUser_SceneSession send;

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, pUser->country, "奉天承运，国王诏曰：罢免捕头之职，钦此.", pUser->name);
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "奉天承运，国王诏曰：罢免捕头之职，钦此", pUser->name);
			///////////////////////////////////////////////////////////////////
	}
}

//soke 2016 - 3 - 11 罢免御史大夫（右上）
void CCountryM::processCancelCensor(UserSession* pUser, Cmd::stCancelCensorCmd* rev)
{
	CCountry* pCountry = this->find(pUser->country);

	if (!pCountry)
	{
		Zebra::logger->error("[国家]:国家信息获取失败,请检查国家信息完整性");
	}

	if (pCountry->isKing(pUser))
	{
		pCountry->cancelCensor();
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "罢免御史大夫成功");

			///////////////////////////////////////////////////////////////////
			//soke 增加国家公告	
			Cmd::Session::t_countryPunishUser_SceneSession send;

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, pUser->country, "奉天承运，国王诏曰：罢免御史大夫之职，钦此.", pUser->name);
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "奉天承运，国王诏曰：罢免御史大夫之职，钦此", pUser->name);
			///////////////////////////////////////////////////////////////////


	}
}

//soke 2016 - 3 - 11 罢免御史大夫（右下）
void CCountryM::processCancelCensos(UserSession* pUser, Cmd::stCancelCensosCmd* rev)
{
	CCountry* pCountry = this->find(pUser->country);

	if (!pCountry)
	{
		Zebra::logger->error("[国家]:国家信息获取失败,请检查国家信息完整性");
	}

	if (pCountry->isKing(pUser))
	{
		pCountry->cancelCensos();
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "罢免御史大夫成功");

			///////////////////////////////////////////////////////////////////
			//soke 增加国家公告	
			Cmd::Session::t_countryPunishUser_SceneSession send;

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, pUser->country, "奉天承运，国王诏曰：罢免御史大夫之职，钦此.", pUser->name);
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "奉天承运，国王诏曰：罢免御史大夫之职，钦此", pUser->name);
			///////////////////////////////////////////////////////////////////
	}
}

//soke 2016 - 3 - 11 罢免元帅
void CCountryM::processCancelYuanshuai(UserSession* pUser, Cmd::stCancelYuanshuaiCmd* rev)
{
	CCountry* pCountry = this->find(pUser->country);

	if (!pCountry)
	{
		Zebra::logger->error("[国家]:国家信息获取失败,请检查国家信息完整性");
	}

	if (pCountry->isKing(pUser))
	{
		pCountry->cancelYuanshuai();
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "罢免元帅成功");

			///////////////////////////////////////////////////////////////////
			//soke 增加国家公告	
			Cmd::Session::t_countryPunishUser_SceneSession send;

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, pUser->country, "奉天承运，国王诏曰：罢免元帅之职，钦此.", pUser->name);
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "奉天承运，国王诏曰：罢免元帅之职，钦此", pUser->name);
			///////////////////////////////////////////////////////////////////
	}
}

//soke 2016 - 3 - 11 罢免宰相
void CCountryM::processCancelZaixiang(UserSession* pUser, Cmd::stCancelZaixiangCmd* rev)
{
	CCountry* pCountry = this->find(pUser->country);

	if (!pCountry)
	{
		Zebra::logger->error("[国家]:国家信息获取失败,请检查国家信息完整性");
	}

	if (pCountry->isKing(pUser))
	{
		pCountry->cancelZaixiang();
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "罢免宰相成功");

			///////////////////////////////////////////////////////////////////
			//soke 增加国家公告	
			Cmd::Session::t_countryPunishUser_SceneSession send;

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, pUser->country, "奉天承运，国王诏曰：罢免宰相之职，钦此.", pUser->name);
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "奉天承运，国王诏曰：罢免宰相之职，钦此", pUser->name);
			///////////////////////////////////////////////////////////////////
	}
}


void CCountryM::processCancelTechSearch(UserSession* pUser, Cmd::stCancelTechSearchUserCmd* rev)
{
	CCountry* pCountry = this->find(pUser->country);

	if (!pCountry)
	{
		Zebra::logger->error("[国家]:国家信息获取失败,请检查国家信息完整性");
	}

	if (pCountry->isKing(pUser))
	{
		CTech* pTech = pCountry->getTech(rev->dwOption);
		if (!pTech)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "该项科技不存在，请确认后再取消");
			return;
		}

		pTech->clearSearcher(pUser);

		Cmd::stReqTechUserCmd send;
		send.dwType = CTech::ACTIVE_TECH;

		CCountryM::getMe().processRequestTech(pUser, &send);
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是国王，不能使用该项功能");
	}
}

void CCountryM::processSetTechSearch(UserSession* pUser, Cmd::stSetTechSearchUserCmd* rev)
{
	CCountry* pCountry = this->find(pUser->country);

	if (!pCountry)
	{
		Zebra::logger->error("[国家]:国家信息获取失败,请检查国家信息完整性");
	}

	if (pCountry->isKing(pUser))
	{
		CTech* pTech = pCountry->getTech(rev->dwOption);
		if (!pTech)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "该项科技不存在，请确认后再设置");
			return;
		}

		UserSession* pSearcher  = UserSessionManager::getInstance()->getUserByID(rev->dwCharID);
		if (!pSearcher)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "该玩家已不在线，请另外选择");
			return;
		}

		if (this->isOfficial(pSearcher))
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "该玩家已是研究员，请另外选择");
			return;
		}

		Cmd::stConfirmSearcherUserCmd send;
		send.dwOption = pTech->dwType;
		pSearcher->sendCmdToMe(&send, sizeof(send));
		pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "已发送 %s 研究任命书", pTech->szName);
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "您不是国王，不能使用该项功能");
	}
}

//soke 2016 - 3 - 11 任命外交官（停用）
void CCountryM::processSetDiplomat(UserSession* pUser, Cmd::stAppointDiplomatCmd* rev)
{
	if (this->isKing(pUser))
	{
		UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(rev->name);
		if (!u || !u->scene)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", rev->name);
			return;
		}
		
		if (u->id == pUser->id)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命自己为外交官");
			return;
		}

		if (u->country != pUser->country)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "只能任命本国人为外交官");
			return;
		}
		
		if (CCityM::getMe().isCastellan(u) || this->isOfficial(u))
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命城主或研究员为外交官");
			return;
		}

		CCountry* pCountry = this->find(pUser->country);

		if (pCountry)
		{

			if (strncmp(pCountry->diplomatName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->catcherName, rev->name, MAX_NAMESIZE) == 0)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s 已为外交官或捕头,不能再任命", rev->name);
				return;
			}
			
			if (strlen(pCountry->diplomatName)>0)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "已有外交官,不能再任命");
				return;
			}

			if (pCountry->changeDiplomat(u))
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "成功任命 %s 为外交官", rev->name);
			}
		}
		else
		{
			Zebra::logger->error("%d 国家数据不存在", pUser->country);
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "您不是国王，不能使用该项功能");
	}
}

//soke 2016 - 3 - 11 国王任命巡捕（左上）
void CCountryM::processSetCatcher(UserSession* pUser, Cmd::stAppointCatcherCmd* rev)
{
	if (this->isKing(pUser))
	{
		UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(rev->name);
		if (!u || !u->scene)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", rev->name);
			return;
		}
		
		if (u->id == pUser->id)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命自己为捕头");
			return;
		}

		if (u->country != pUser->country)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "只能任命本国人为捕头");
			return;
		}
		
		if (CCityM::getMe().isCastellan(u) || this->isOfficial(u))
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命城主或研究员为捕头");
			return;
		}

		CCountry* pCountry = this->find(pUser->country);

		if (pCountry)
		{
			if (strncmp(pCountry->diplomatName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->censorName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->censosName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->catcherName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->catchexName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->yuanshuaiName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->zaixiangName, rev->name, MAX_NAMESIZE) == 0)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s 已是官员,不能再任命", rev->name);
				return;
			}

			if (strlen(pCountry->catcherName)>0)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "已有捕头,不能再任命");
				return;
			}
			
			Cmd::Session::t_checkUser_SceneSession send;
			send.dwCheckID = pUser->id;
			send.dwCheckedID = u->id;
			u->scene->sendCmd(&send, sizeof(send));
		}
		else
		{
			Zebra::logger->error("%d 国家数据不存在", pUser->country);
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是国王，不能使用该项功能");
	}
}

//soke 2016 - 3 - 11 国王任命巡捕（左下）
void CCountryM::processSetCatchex(UserSession* pUser, Cmd::stAppointCatchexCmd* rev)
{
	if (this->isKing(pUser))
	{
		UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(rev->name);
		if (!u || !u->scene)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", rev->name);
			return;
		}
		
		if (u->id == pUser->id)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命自己为捕头");
			return;
		}

		if (u->country != pUser->country)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "只能任命本国人为捕头");
			return;
		}
		
		if (CCityM::getMe().isCastellan(u) || this->isOfficial(u))
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命城主或研究员为捕头");
			return;
		}

		CCountry* pCountry = this->find(pUser->country);

		if (pCountry)
		{
			if (strncmp(pCountry->diplomatName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->censorName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->censosName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->catcherName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->catchexName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->yuanshuaiName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->zaixiangName, rev->name, MAX_NAMESIZE) == 0)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s 已是官员,不能再任命", rev->name);
				return;
			}

			if (strlen(pCountry->catchexName)>0)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "已有捕头,不能再任命");
				return;
			}
			
			Cmd::Session::t_checkUserCatchex_SceneSession send; //soke 善恶度检测 巡捕（左下）
			send.dwCheckID = pUser->id;
			send.dwCheckedID = u->id;
			u->scene->sendCmd(&send, sizeof(send));
		}
		else
		{
			Zebra::logger->error("%d 国家数据不存在", pUser->country);
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是国王，不能使用该项功能");
	}
}

//soke 2016 - 3 - 11 国王任命御史大夫（右上）
void CCountryM::processSetCensor(UserSession* pUser, Cmd::stAppointCensorCmd* rev)
{
	if (this->isKing(pUser))
	{
		UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(rev->name);
		if (!u || !u->scene)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", rev->name);
			return;
		}
		
		if (u->id == pUser->id)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命自己为御史大夫");
			return;
		}

		if (u->country != pUser->country)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "只能任命本国人为御史大夫");
			return;
		}
		
		if (CCityM::getMe().isCastellan(u) || this->isOfficial(u))
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命城主或研究员为御史大夫");
			return;
		}

		CCountry* pCountry = this->find(pUser->country);

		if (pCountry)
		{

			if (strncmp(pCountry->diplomatName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->censorName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->censosName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->catcherName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->catchexName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->yuanshuaiName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->zaixiangName, rev->name, MAX_NAMESIZE) == 0)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s 已是官员,不能再任命", rev->name);
				return;
			}
			
			if (strlen(pCountry->censorName)>0)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "已有御史大夫,不能再任命");
				return;
			}

			if (pCountry->changeCensor(u))
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "成功任命 %s 为御史大夫", rev->name);
			}
		}
		else
		{
			Zebra::logger->error("%d 国家数据不存在", pUser->country);
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是国王，不能使用该项功能");
	}
}

//soke 2016 - 3 - 11 国王任命御史大夫（右下）
void CCountryM::processSetCensos(UserSession* pUser, Cmd::stAppointCensosCmd* rev)
{
	if (this->isKing(pUser))
	{
		UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(rev->name);
		if (!u || !u->scene)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", rev->name);
			return;
		}
		
		if (u->id == pUser->id)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命自己为御史大夫");
			return;
		}

		if (u->country != pUser->country)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "只能任命本国人为御史大夫");
			return;
		}
		
		if (CCityM::getMe().isCastellan(u) || this->isOfficial(u))
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命城主或研究员为御史大夫");
			return;
		}

		CCountry* pCountry = this->find(pUser->country);

		if (pCountry)
		{

			if (strncmp(pCountry->diplomatName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->censorName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->censosName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->catcherName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->catchexName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->yuanshuaiName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->zaixiangName, rev->name, MAX_NAMESIZE) == 0)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s 已是官员,不能再任命", rev->name);
				return;
			}
			
			if (strlen(pCountry->censosName)>0)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "已有御史大夫,不能再任命");
				return;
			}

			if (pCountry->changeCensos(u))
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "成功任命 %s 为御史大夫", rev->name);
			}
		}
		else
		{
			Zebra::logger->error("%d 国家数据不存在", pUser->country);
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是国王，不能使用该项功能");
	}
}

//soke 2016 - 3 - 11 国王任命元帅
void CCountryM::processSetYuanshuai(UserSession* pUser, Cmd::stAppointYuanshuaiCmd* rev)
{
	if (this->isKing(pUser))
	{
		UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(rev->name);
		if (!u || !u->scene)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", rev->name);
			return;
		}
		
		if (u->id == pUser->id)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命自己为元帅");
			return;
		}

		if (u->country != pUser->country)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "只能任命本国人为元帅");
			return;
		}
		
		if (CCityM::getMe().isCastellan(u) || this->isOfficial(u))
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命城主或研究员为元帅");
			return;
		}

		CCountry* pCountry = this->find(pUser->country);

		if (pCountry)
		{

			if (strncmp(pCountry->diplomatName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->censorName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->censosName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->catcherName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->catchexName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->yuanshuaiName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->zaixiangName, rev->name, MAX_NAMESIZE) == 0)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s 已是官员,不能再任命", rev->name);
				return;
			}
			
			if (strlen(pCountry->yuanshuaiName)>0)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "已有元帅,不能再任命");
				return;
			}

			if (pCountry->changeYuanshuai(u))
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "成功任命 %s 为元帅", rev->name);
			}
		}
		else
		{
			Zebra::logger->error("%d 国家数据不存在", pUser->country);
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是国王，不能使用该项功能");
	}
}

//soke 2016 - 3 - 11 国王任命宰相
void CCountryM::processSetZaixiang(UserSession* pUser, Cmd::stAppointZaixiangCmd* rev)
{
	if (this->isKing(pUser))
	{
		UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(rev->name);
		if (!u || !u->scene)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", rev->name);
			return;
		}
		
		if (u->id == pUser->id)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命自己为宰相");
			return;
		}

		if (u->country != pUser->country)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "只能任命本国人为宰相");
			return;
		}
		
		if (CCityM::getMe().isCastellan(u) || this->isOfficial(u))
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命城主或研究员为宰相");
			return;
		}

		CCountry* pCountry = this->find(pUser->country);

		if (pCountry)
		{

			if (strncmp(pCountry->diplomatName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->censorName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->censosName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->catcherName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->catchexName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->yuanshuaiName, rev->name, MAX_NAMESIZE) == 0
			||  strncmp(pCountry->zaixiangName, rev->name, MAX_NAMESIZE) == 0)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s 已是官员,不能再任命", rev->name);
				return;
			}
			
			if (strlen(pCountry->zaixiangName)>0)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "已有宰相,不能再任命");
				return;
			}

			if (pCountry->changeZaixiang(u))
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "成功任命 %s 为宰相", rev->name);
			}
		}
		else
		{
			Zebra::logger->error("%d 国家数据不存在", pUser->country);
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是国王，不能使用该项功能");
	}
}


void CCountryM::processDareCountry(UserSession* pUser, Cmd::stDareCountryFormalCmd* rev)
{
	CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);

	if (pUser->country == rev->dwCountryID)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不能挑战自己的国家");
		return;
	}

	if (!pUnion)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "你不是国王，不能挑战别的国家");
		return;
	}

	if (!pUnion->master)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "你不是国王，不能挑战别的国家");
		return;
	}

	if (pUnion->master->id == pUser->id)
	{// 是帮主，判断是不是国王
		CCity* pCity = CCityM::getMe().find(pUser->country, KING_CITY_ID); // 王城
		// 王城的ID
		CCountry* pCountry = this->find(pUser->country);

		if (pCountry)
		{
			if (pCity && ((pCity->dwUnionID == pUser->unionid) 
						|| pCountry->dwKingUnionID == pUser->unionid))
			{// 是帮主，是王城城主或是国王，允许挑战,取出被挑国家，判断是否在被挑期限
				struct tm tv1,tv2,tv3;  
				time_t timValue = time(NULL);
				zRTime::getLocalTime(tv1, timValue);
				zRTime::getLocalTime(tv3, (timValue+24*60*60));

				/*CDare* pDare = CDareM::getMe().
					findDareRecord(Cmd::COUNTRY_FORMAL_DARE, rev->dwCountryID,
							pUser->country);
				// */

				if (tv3.tm_wday == 6)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
							"明天将进行皇城争夺战,不允许挑战他国");
					return;
				}

				/*if (pDare != NULL)
				{
					pDare->isAntiAtt = true;
					pDare->sendAntiAtt();

					SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP,
							pUser->country,
							"%d年%d月%d日20点40分,我国向　%s　发起了反攻，请做好进攻准备", 
							tv1.tm_year+1900, tv1.tm_mon+1, tv1.tm_mday,
							this->find(rev->dwCountryID)->name);

					return;
				}
				// */

#ifndef _ZJW_DEBUG
				if (tv1.tm_hour>=18)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
							"请在18点以前来挑战");
					return;
				}
#endif						
				/*if (pCountry->dwDareCountryID>0)	
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "已接受别国挑战！");
					return;
				}*/		       

				if (this->find(6, pUser->unionid)!=NULL)
				{
					if (tv3.tm_wday==6)
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
								"明天将进行皇城争夺战,不允许挑战他国");

						return;
					}
				}

				CCountry* pDefCountry = this->find(rev->dwCountryID);
				if (pDefCountry == NULL)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你挑战的国家不存在");
					return;
				}


				zRTime::getLocalTime(tv2, pDefCountry->dwLastDareTime);
#ifndef _ALL_SUPER_GM
				if ((abs(tv1.tm_mday - tv2.tm_mday) >= 3) ||
						abs(timValue - pDefCountry->dwLastDareTime) >72*60*60)
#endif							
				{
					if (pDefCountry->dwDareCountryID>0)
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
								"该国已接受别国挑战。请明天再来。");
						return;
					}
					else
					{	
						/*if (this->findByDare(pDefCountry->dwID, false) != NULL)
						{
							pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
									"该国已挑战其它国家，不能再对其进行挑战。");
							return;
						}
						*/

						if (this->find(6, pDefCountry->dwKingUnionID) != NULL)
						{
							if (tv3.tm_wday == 6)
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
										"明天将进行皇城争夺战,不允许被挑战");
								return;
							}
						}

						//遍历管理器，看是否已挑战别国
						if (this->findByDare(pUser->country) == NULL)
						{
							rwlock.wrlock();
							pDefCountry->dwDareCountryID = pUser->country;
							pDefCountry->dwLastDareTime = timValue;
							pCountry->dwDareTime = timValue;
							rwlock.unlock();

							pDefCountry->writeDatabase();
							pCountry->writeDatabase();

							/*SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP,
							  pDefCountry->dwID,
							  "一小时后,　%s　将攻打我国，请做好防御准备", 
							  pCountry->name);*/

							SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP,
									pDefCountry->dwID,
									"%d年%d月%d日20点40分,　%s　将攻打我国，请做好防御准备", 
									tv3.tm_year+1900,tv3.tm_mon+1, tv3.tm_mday, pCountry->name);

							SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, 
									pCountry->dwID,
									"%d年%d月%d日20点40分,　%s　国王将带领我们去攻打 %s。", 
									tv3.tm_year+1900, tv3.tm_mon+1, tv3.tm_mday,
									pCountry->name, pDefCountry->name);
							/*SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, 
							  pCountry->dwID,
							  "一小时后,　%s　国王将带领我们去攻打 %s。", 
							  pCountry->name, pDefCountry->name);
							 */
							pUser->sendSysChat(Cmd::INFO_TYPE_EXP, 
									"发起国战请求成功！请在明天八点四十分准时参加!");

							/*pUser->sendSysChat(Cmd::INFO_TYPE_EXP, 
							  "发起国战请求成功！请一小时后准时参加!");*/

#ifdef _ZJW_DEBUG
//							pDefCountry->beginDare();
#endif									
						}
						else
						{
							pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
									"你今天已发起了挑战，不能再挑战了");
						}
					}
				}
#ifndef _ALL_SUPER_GM
				else
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "才被挑了，该让别人歇两天了");
				}
#endif						
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "你不是国王，不能挑战别的国家");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "你不是国王，不能挑战别的国家");
			return;
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "你不是国王，不能挑战别的国家");
	}

	return;
}

void CCountryM::processAntiDareCountry(UserSession* pUser, Cmd::stAntiDareCountryFormalCmd* rev)
{
	if (pUser->country == rev->dwCountryID)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不能挑战自己的国家");
		return;
	}

	if (this->isKing(pUser))
	{
		CDareRecord * pRecord = CDareRecordM::getMe().findLastRecord(rev->dwCountryID, pUser->country);
		if (pRecord)
		{
			CCountry* pDefCountry = this->find(rev->dwCountryID);
			if (pDefCountry && CDareM::getMe().findDareRecordByID(Cmd::COUNTRY_FORMAL_DARE, pDefCountry->dwID)==NULL)
			{
				pDefCountry->beginAntiDare(pUser->country);
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "不满足反攻条件,不能挑战该国");
			return;
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "你不是国王，不能挑战别的国家");
		return;
	}
}

void CCountryM::processRequestTech(UserSession* pUser, Cmd::stReqTechUserCmd* rev)
{
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::stTechItem* tempPoint;
	Cmd::stRtnTechUserCmd* retCmd = (Cmd::stRtnTechUserCmd*)buf;
	constructInPlace(retCmd);

	tempPoint = (Cmd::stTechItem *)retCmd->data;
	retCmd->dwType = rev->dwType;
	CCountry* pCountry = this->find(pUser->country);

	if (!pCountry)
	{
		return;
	}
	
	switch (rev->dwType)
	{
		case  CTech::WAIT_TECH:
			{
				for (int i=1; i<TECH_MAX_NUM; i++)
				{
					CTech* pTech = pCountry->getTech(i);
					if (pTech && pTech->state()==CTech::WAIT_TECH)
					{
						tempPoint->dwOption = pTech->dwType;
						strncpy(tempPoint->szOptionDesc, pTech->szName, MAX_NAMESIZE);
						bzero(tempPoint->szResearchName, sizeof(tempPoint->szResearchName));
						tempPoint->dwLevel = pTech->dwLevel;
						tempPoint->dwProgress = 0;
						tempPoint++;
						retCmd->dwSize++;
					}
				}
			}
			break;
		case CTech::ACTIVE_TECH:
			{
				for (int i=1; i<TECH_MAX_NUM; i++)
				{
					CTech* pTech = pCountry->getTech(i);
					if (pTech && pTech->state()==CTech::ACTIVE_TECH)
					{
						tempPoint->dwOption = pTech->dwType;
						strncpy(tempPoint->szOptionDesc, pTech->szName, MAX_NAMESIZE);
						strncpy(tempPoint->szResearchName, pTech->szResearchName, MAX_NAMESIZE);
						tempPoint->dwLevel = pTech->dwLevel;
						tempPoint->dwProgress = pTech->dwProgress;
						tempPoint++;
						retCmd->dwSize++;
					}
				}
			}
			break;
		case CTech::FINISH_TECH:
			{
				for (int i=1; i<TECH_MAX_NUM; i++)
				{
					CTech* pTech = pCountry->getTech(i);
					if (pTech && pTech->level()>1)
					{
						tempPoint->dwOption = pTech->dwType;
						strncpy(tempPoint->szOptionDesc, pTech->szName, MAX_NAMESIZE);
						strncpy(tempPoint->szResearchName, pTech->szResearchName, MAX_NAMESIZE);
						tempPoint->dwLevel = pTech->dwLevel;
						tempPoint->dwProgress = 0;
						tempPoint++;
						retCmd->dwSize++;
					}
				}
			}
			break;
		default:
			break;
	}

	if (retCmd->dwSize>0)
	{
		pUser->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stTechItem) + sizeof(Cmd::stRtnTechUserCmd)));
	}
}

void CCountryM::processRequestDare(UserSession* pUser, Cmd::stRequestDareCountryCmd* rev)
{
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::stCountryInfo *tempCountry = NULL;

	std::vector<CCountry*>::iterator vIterator;

	rwlock.rdlock();
	Cmd::stReturnDareCountryCmd *retCmd=(Cmd::stReturnDareCountryCmd *)buf;

	constructInPlace(retCmd);
	tempCountry = (Cmd::stCountryInfo *)retCmd->country_list;
	retCmd->byType = rev->byType;
	retCmd->dwSize = 0;

	if (rev->byType == Cmd::REQUEST_BATTLEFIELD_COUNTRY_LIST)
	{
		struct tm tv1;
		time_t timValue = time(NULL);
		zRTime::getLocalTime(tv1, timValue);
		if (tv1.tm_hour <20 || tv1.tm_hour>22)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "非国战时间，不能跳转到战场");
		}
		else
		{
			for (vIterator = countries.begin(); vIterator!=countries.end(); vIterator++)
			{
				if ((*vIterator)->dwDareCountryID>0)
				{
					tempCountry->dwID = (*vIterator)->dwID;
					if (pUser->country == (*vIterator)->dwID 
							|| pUser->country==(*vIterator)->dwDareCountryID)
					{
						tempCountry->byType = 0;
					}
					else
					{
						tempCountry->byType = 1;
					}
					tempCountry++;
					retCmd->dwSize++;
				}
			}
		}	
	}
	else if (rev->byType == Cmd::REQUEST_DARE_COUNTRY_LIST)
	{
		struct tm tv1,tv2;
		time_t timValue = time(NULL);
		zRTime::getLocalTime(tv1, timValue);


		for (vIterator = countries.begin(); vIterator!=countries.end(); vIterator++)
		{

			zRTime::getLocalTime(tv2, (*vIterator)->dwLastDareTime);

#ifndef _ALL_SUPER_GM			
			if ((abs(tv1.tm_mday - tv2.tm_mday) >= 2) || 
					(timValue - (*vIterator)->dwLastDareTime) >48*60*60)
#endif				
			{
				if ((*vIterator)->dwID != pUser->country && (*vIterator)->dwID !=6)
				{
					tempCountry->dwID = (*vIterator)->dwID;
					tempCountry++;
					retCmd->dwSize++;
				}
			}
		}

	}

	pUser->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stCountryInfo)+sizeof(Cmd::stReturnDareCountryCmd)));

	rwlock.unlock();
}

void CCountryM::endDare()
{
	std::vector<CCountry*>::iterator vIterator;
	rwlock.rdlock();

	for (vIterator = countries.begin(); vIterator!=countries.end(); vIterator++)
	{
		if ((*vIterator)->dwDareCountryID > 0)
		{
			(*vIterator)->endDare();
		}
	}	
	rwlock.unlock();
}

void CCountryM::beginDare()
{
	std::vector<CCountry*>::iterator vIterator;
	rwlock.rdlock();

	for(vIterator = countries.begin(); vIterator!=countries.end(); vIterator++)
	{
		(*vIterator)->beginDare();
	}	
	rwlock.unlock();
}

void CCountryM::beginGem()
{
	std::vector<CCountry*>::iterator vIterator;
	rwlock.rdlock();

	for(vIterator = countries.begin(); vIterator!=countries.end(); vIterator++)
	{
		CGemM::getMe().addNewGem((*vIterator)->dwID);
	}	
	rwlock.unlock();
}


void CCountryM::userOnline(UserSession *pUser)
{
	if (pUser)
	{
		CDareM::getMe().userOnlineCountry(pUser);
		/*CDare* pDare = NULL;   
		pDare = CDareM::getMe().findDareRecordByID(Cmd::COUNTRY_FORMAL_DARE, pUser->country);
		if (pDare) 
		{
#ifdef _ZJW_DEBUG
			Zebra::logger->debug("[国战]:%s 进入国战状态", pUser->name);
#endif		 
			pDare->sendActiveStateToScene(pUser);
		}
		else
		{
			pDare = CDareM::getMe().findDareRecordByID(Cmd::COUNTRY_FORMAL_ANTI_DARE, pUser->country);
			if (pDare)
			{
				pDare->sendActiveStateToScene(pUser);
			}
		}
		*/

		BYTE buf[zSocket::MAX_DATASIZE];        
		Cmd::stCountryStar *tempPoint;

		Cmd::stUpdateCountryStarCmd *retCmd=(Cmd::stUpdateCountryStarCmd *)buf;
		constructInPlace(retCmd);
		tempPoint = (Cmd::stCountryStar *)retCmd->data;

		rwlock.rdlock();
		
		for (DWORD i=0; i<countries.size(); i++)
		{
			if (countries[i])
			{
				tempPoint->dwCountry = countries[i]->dwID;
				tempPoint->dwStar = countries[i]->getStar();
				tempPoint++;
				retCmd->dwSize++;
			}
		}
		
		rwlock.unlock();
		pUser->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stCountryStar)+sizeof(Cmd::stUpdateCountryStarCmd)));

		Cmd::stUpdateCountryKingUserCmd send;
		CCountry* pCountry = this->find(pUser->country);
		
		if (pCountry)
		{
			strncpy(send.kingName, pCountry->kingName, MAX_NAMESIZE);
			CCountry* pEmperor = this->find(NEUTRAL_COUNTRY_ID);
			CUnion* pUnion = NULL;
				
			if (pEmperor && pEmperor->dwKingUnionID>0)
			{
				pUnion = CUnionM::getMe().getUnionByID(pEmperor->dwKingUnionID);
				//醉梦 修复皇帝删号 再创建掉5
				if(pUnion)
				{
					if (pUnion->dwCountryID == pUser->country)
					send.isEmperor = 1;
				}
				
			}
			
			pUser->sendCmdToMe(&send, sizeof(send));

			//soke 2016 - 3 - 11 外交官（停用）
			if (strncmp(pCountry->diplomatName, pUser->name, MAX_NAMESIZE) == 0)
			{
				Cmd::Session::t_setDiplomatState_SceneSession send;
				if (pUser->scene) 
				{
					send.byState = 1;
					send.dwUserID = pUser->id;
					pUser->scene->sendCmd(&send, sizeof(send));
				}
			}
			//soke 2016 - 3 - 11 巡捕（左上）
			else if (strncmp(pCountry->catcherName, pUser->name, MAX_NAMESIZE) == 0)
			{
				Cmd::Session::t_setCatcherState_SceneSession send;
				if (pUser->scene)
				{
					send.byState = 1;
					send.dwUserID = pUser->id;
					pUser->scene->sendCmd(&send, sizeof(send));
				}
			}
			//soke 2016 - 3 - 11 巡捕（左下）
			else if (strncmp(pCountry->catchexName, pUser->name, MAX_NAMESIZE) == 0)
			{
				Cmd::Session::t_setCatchexState_SceneSession send;
				if (pUser->scene)
				{
					send.byState = 1;
					send.dwUserID = pUser->id;
					pUser->scene->sendCmd(&send, sizeof(send));
				}
			}
			//soke 2016 - 3 - 11 御史大夫（右上）
			if (strncmp(pCountry->censorName, pUser->name, MAX_NAMESIZE) == 0)
			{
				Cmd::Session::t_setCensorState_SceneSession send;
				if (pUser->scene) 
				{
					send.byState = 1;
					send.dwUserID = pUser->id;
					pUser->scene->sendCmd(&send, sizeof(send));
				}
			}
			//soke 2016 - 3 - 11 御史大夫（右下）
			else if (strncmp(pCountry->censosName, pUser->name, MAX_NAMESIZE) == 0)
			{
				Cmd::Session::t_setCensosState_SceneSession send;
				if (pUser->scene) 
				{
					send.byState = 1;
					send.dwUserID = pUser->id;
					pUser->scene->sendCmd(&send, sizeof(send));
				}
			}
			//soke 2016 - 3 - 11 元帅
			else if (strncmp(pCountry->yuanshuaiName, pUser->name, MAX_NAMESIZE) == 0)
			{
				Cmd::Session::t_setYuanshuaiState_SceneSession send;
				if (pUser->scene) 
				{
					send.byState = 1;
					send.dwUserID = pUser->id;
					pUser->scene->sendCmd(&send, sizeof(send));
				}
			}
			//soke 2016 - 3 - 11 宰相
			else if (strncmp(pCountry->zaixiangName, pUser->name, MAX_NAMESIZE) == 0)
			{
				Cmd::Session::t_setZaixiangState_SceneSession send;
				if (pUser->scene) 
				{
					send.byState = 1;
					send.dwUserID = pUser->id;
					pUser->scene->sendCmd(&send, sizeof(send));
				}
			}
		}
	}

	//soke 2016 - 3 - 11 复制实现国家官员表 - 捕头（左上） - 开始
	if (pUser)
	{
		CDareM::getMe().userOnlineCountry(pUser);

		BYTE buf[zSocket::MAX_DATASIZE];        
		Cmd::stCountryStar *tempPoint;

		Cmd::stUpdateCountryStarCmd *retCmd=(Cmd::stUpdateCountryStarCmd *)buf;
		constructInPlace(retCmd);
		tempPoint = (Cmd::stCountryStar *)retCmd->data;

		rwlock.rdlock();
		
		for (DWORD i=0; i<countries.size(); i++)
		{
			if (countries[i])
			{
				tempPoint->dwCountry = countries[i]->dwID;
				tempPoint->dwStar = countries[i]->getStar();
				tempPoint++;
				retCmd->dwSize++;
			}
		}
		
		rwlock.unlock();
		pUser->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stCountryStar)+sizeof(Cmd::stUpdateCountryStarCmd)));

		Cmd::stUpdateCountryCatcherUserCmd send;  //soke 更新国家官员 - 捕头（左上）
		CCountry* pCountry = this->find(pUser->country);
		
		if (pCountry)
		{
			strncpy(send.catcherName, pCountry->catcherName, MAX_NAMESIZE);		
			pUser->sendCmdToMe(&send, sizeof(send));
            /////////////////////////////////////////////////////////////////////////////
		}
	} //soke 2016 - 3 - 11 国家官员表 -捕头（左上） - 结束

	//soke 2016 - 3 - 11 复制实现国家官员表 - 捕头（左下） - 开始
	if (pUser)
	{
		CDareM::getMe().userOnlineCountry(pUser);

		BYTE buf[zSocket::MAX_DATASIZE];        
		Cmd::stCountryStar *tempPoint;

		Cmd::stUpdateCountryStarCmd *retCmd=(Cmd::stUpdateCountryStarCmd *)buf;
		constructInPlace(retCmd);
		tempPoint = (Cmd::stCountryStar *)retCmd->data;

		rwlock.rdlock();
		
		for (DWORD i=0; i<countries.size(); i++)
		{
			if (countries[i])
			{
				tempPoint->dwCountry = countries[i]->dwID;
				tempPoint->dwStar = countries[i]->getStar();
				tempPoint++;
				retCmd->dwSize++;
			}
		}
		
		rwlock.unlock();
		pUser->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stCountryStar)+sizeof(Cmd::stUpdateCountryStarCmd)));

		Cmd::stUpdateCountryCatchexUserCmd send;  //soke 更新国家官员 - 捕头（右）
		CCountry* pCountry = this->find(pUser->country);
		
		if (pCountry)
		{
			strncpy(send.catchexName, pCountry->catchexName, MAX_NAMESIZE);		
			pUser->sendCmdToMe(&send, sizeof(send));
            /////////////////////////////////////////////////////////////////////////////
		}
	} //soke 国家官员表 -捕头（右） - 结束

	//soke 复制实现国家官员表 - 御史大夫（左） - 开始
	if (pUser)
	{
		CDareM::getMe().userOnlineCountry(pUser);

		BYTE buf[zSocket::MAX_DATASIZE];        
		Cmd::stCountryStar *tempPoint;

		Cmd::stUpdateCountryStarCmd *retCmd=(Cmd::stUpdateCountryStarCmd *)buf;
		constructInPlace(retCmd);
		tempPoint = (Cmd::stCountryStar *)retCmd->data;

		rwlock.rdlock();
		
		for (DWORD i=0; i<countries.size(); i++)
		{
			if (countries[i])
			{
				tempPoint->dwCountry = countries[i]->dwID;
				tempPoint->dwStar = countries[i]->getStar();
				tempPoint++;
				retCmd->dwSize++;
			}
		}
		
		rwlock.unlock();
		pUser->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stCountryStar)+sizeof(Cmd::stUpdateCountryStarCmd)));

		Cmd::stUpdateCountryCensorUserCmd send;  //soke 更新国家官员 - 御史大夫（左）
		CCountry* pCountry = this->find(pUser->country);
		
		if (pCountry)
		{
			strncpy(send.censorName, pCountry->censorName, MAX_NAMESIZE);		
			pUser->sendCmdToMe(&send, sizeof(send));
            /////////////////////////////////////////////////////////////////////////////
		}
	} //soke 国家官员表 -御史大夫（左） - 结束

	//soke 复制实现国家官员表 - 御史大夫（右） - 开始
	if (pUser)
	{
		CDareM::getMe().userOnlineCountry(pUser);

		BYTE buf[zSocket::MAX_DATASIZE];        
		Cmd::stCountryStar *tempPoint;

		Cmd::stUpdateCountryStarCmd *retCmd=(Cmd::stUpdateCountryStarCmd *)buf;
		constructInPlace(retCmd);
		tempPoint = (Cmd::stCountryStar *)retCmd->data;

		rwlock.rdlock();
		
		for (DWORD i=0; i<countries.size(); i++)
		{
			if (countries[i])
			{
				tempPoint->dwCountry = countries[i]->dwID;
				tempPoint->dwStar = countries[i]->getStar();
				tempPoint++;
				retCmd->dwSize++;
			}
		}
		
		rwlock.unlock();
		pUser->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stCountryStar)+sizeof(Cmd::stUpdateCountryStarCmd)));

		Cmd::stUpdateCountryCensosUserCmd send;  //soke 更新国家官员 - 御史大夫（右）
		CCountry* pCountry = this->find(pUser->country);
		
		if (pCountry)
		{
			strncpy(send.censosName, pCountry->censosName, MAX_NAMESIZE);		
			pUser->sendCmdToMe(&send, sizeof(send));
            /////////////////////////////////////////////////////////////////////////////
		}
	} //soke 国家官员表 -御史大夫（右） - 结束

	//soke 复制实现国家官员表 - 元帅 - 开始
	if (pUser)
	{
		CDareM::getMe().userOnlineCountry(pUser);

		BYTE buf[zSocket::MAX_DATASIZE];        
		Cmd::stCountryStar *tempPoint;

		Cmd::stUpdateCountryStarCmd *retCmd=(Cmd::stUpdateCountryStarCmd *)buf;
		constructInPlace(retCmd);
		tempPoint = (Cmd::stCountryStar *)retCmd->data;

		rwlock.rdlock();
		
		for (DWORD i=0; i<countries.size(); i++)
		{
			if (countries[i])
			{
				tempPoint->dwCountry = countries[i]->dwID;
				tempPoint->dwStar = countries[i]->getStar();
				tempPoint++;
				retCmd->dwSize++;
			}
		}
		
		rwlock.unlock();
		pUser->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stCountryStar)+sizeof(Cmd::stUpdateCountryStarCmd)));

		Cmd::stUpdateCountryYuanshuaiUserCmd send;  //soke 更新国家官员 - 元帅
		CCountry* pCountry = this->find(pUser->country);
		
		if (pCountry)
		{
			strncpy(send.yuanshuaiName, pCountry->yuanshuaiName, MAX_NAMESIZE);		
			pUser->sendCmdToMe(&send, sizeof(send));
            /////////////////////////////////////////////////////////////////////////////
		}
	} //soke 国家官员表 - 元帅 - 结束

	//soke 复制实现国家官员表 - 宰相 - 开始
	if (pUser)
	{
		CDareM::getMe().userOnlineCountry(pUser);

		BYTE buf[zSocket::MAX_DATASIZE];        
		Cmd::stCountryStar *tempPoint;

		Cmd::stUpdateCountryStarCmd *retCmd=(Cmd::stUpdateCountryStarCmd *)buf;
		constructInPlace(retCmd);
		tempPoint = (Cmd::stCountryStar *)retCmd->data;

		rwlock.rdlock();
		
		for (DWORD i=0; i<countries.size(); i++)
		{
			if (countries[i])
			{
				tempPoint->dwCountry = countries[i]->dwID;
				tempPoint->dwStar = countries[i]->getStar();
				tempPoint++;
				retCmd->dwSize++;
			}
		}
		
		rwlock.unlock();
		pUser->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stCountryStar)+sizeof(Cmd::stUpdateCountryStarCmd)));

		Cmd::stUpdateCountryZaixiangUserCmd send;  //soke 更新国家官员 - 宰相
		CCountry* pCountry = this->find(pUser->country);
		
		if (pCountry)
		{
			strncpy(send.zaixiangName, pCountry->zaixiangName, MAX_NAMESIZE);		
			pUser->sendCmdToMe(&send, sizeof(send));
            /////////////////////////////////////////////////////////////////////////////
		}
	} //soke 国家官员表 - 宰相 - 结束

}

void CCountryM::execEveryCountry(countryCallback & cb)
{
	for (DWORD i=0; i<countries.size(); i++)
	{
		if (countries[i])
			cb.exec(countries[i]);
		else
			Zebra::logger->error("国家管理器包含空指针");
	}
}

void CCountryM::refreshTax()
{
	for (DWORD i=0; i<countries.size(); i++)
	{
		if (countries[i])
		{
			SceneSessionManager::getInstance()->notifyCountryTax(countries[i]->dwID, countries[i]->dwTax);
		}			
		else
			Zebra::logger->error("国家管理器包含空指针");
	}
}

void CCountryM::broadcastTech(DWORD dwCountryID)
{
	Cmd::Session::t_updateTech_SceneSession send;
	CCountry* pCountry = this->find(dwCountryID);
	
	if (pCountry)
	{
		for (int i=1; i<TECH_MAX_NUM; i++)	
		{
			CTech* pTech = pCountry->getTech(i);
			if (pTech)
			{
				send.data[i-1].dwType = pTech->type();
				send.data[i-1].dwLevel = pTech->level();
			}
		}

		send.dwCountryID = dwCountryID;
		SessionTaskManager::getInstance().broadcastScene(&send, sizeof(send));
	}
	else
	{
		Zebra::logger->trace("[国家]: %d 国家数据不存在。请检查国家数据完整性", dwCountryID);
	}
}

bool CCountryM::isKing(UserSession* pUser)
{
	CCountry* pCountry = this->find(pUser->country);
	
	if (pCountry)
	{
		return pCountry->isKing(pUser);	
	}

	return false;
}

bool CCountryM::isEmperor(UserSession* pUser)
{
	CCountry* pCountry = this->find(NEUTRAL_COUNTRY_ID);
	
	if (pCountry)
	{
		CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);

		if (!pUnion)
		{
			return false;
		}

		// TODO:判断是否是城主或国王
		if (pUnion->master && pUnion->master->id == pUser->id)  
		{//是帮主
			if (pUnion->id == pCountry->dwKingUnionID)
			{
				return true;
			}
		}
	}

	return false;
}


bool CCountryM::isOfficial(UserSession* pUser)
{
	CCountry* pCountry = this->find(pUser->country);
	
	if (pCountry)
	{
		return pCountry->isOfficial(pUser);	
	}

	return false;
}

void CCountryM::refreshTech(SessionTask* scene, DWORD dwCountryID)
{
	Cmd::Session::t_updateTech_SceneSession send;
	CCountry* pCountry = this->find(dwCountryID);
	
	if (pCountry)
	{
		for (int i=1; i<TECH_MAX_NUM; i++)	
		{
			CTech* pTech = pCountry->getTech(i);
			if (pTech)
			{
				send.data[i-1].dwType = pTech->type();
				send.data[i-1].dwLevel = pTech->level();
			}
		}

		send.dwCountryID = dwCountryID;
		scene->sendCmd(&send, sizeof(send));
	}
	else
	{
		Zebra::logger->trace("[国家]: %d 国家数据不存在。请检查国家数据完整性", dwCountryID);
	}
}

/*
 * 每天0点清除禁言和关押的标记
 *
 */
void CCountryM::clearForbid()
{
	for (std::vector<CCountry*>::iterator it=countries.begin(); it!=countries.end(); it++)
	{
		(*it)->forbidTalk = 0;
		(*it)->sendPrison = 0;
	}
}

/*
 * 每天0点清除外交官(停用)
 *
 */
void CCountryM::clearDiplomat()
{
	for (std::vector<CCountry*>::iterator it=countries.begin(); it!=countries.end(); it++)
	{
		(*it)->cancelDiplomat();
	}
}

/*
 * 每天0点清除御史大夫（右上）
 *
 */
void CCountryM::clearCensor()
{
	for (std::vector<CCountry*>::iterator it=countries.begin(); it!=countries.end(); it++)
	{
	//	(*it)->cancelCensor();
	}
}

/*
 * 每天0点清除御史大夫（右）
 *
 */
void CCountryM::clearCensos()
{
	for (std::vector<CCountry*>::iterator it=countries.begin(); it!=countries.end(); it++)
	{
	//	(*it)->cancelCensos();
	}
}

/*
 * 每天0点清除元帅
 *
 */
void CCountryM::clearYuanshuai()
{
	for (std::vector<CCountry*>::iterator it=countries.begin(); it!=countries.end(); it++)
	{
	//	(*it)->cancelYuanshuai();
	}
}

/*
 * 每天0点清除宰相
 *
 */
void CCountryM::clearZaixiang()
{
	for (std::vector<CCountry*>::iterator it=countries.begin(); it!=countries.end(); it++)
	{
	//	(*it)->cancelZaixiang();
	}
}

/*
 * 每天0点清令牌使用次数
 *
 */
void CCountryM::resetCallTimes()
{
	for (std::vector<CCountry*>::iterator it=countries.begin(); it!=countries.end(); it++)
	{
		(*it)->calltimes=0;
		(*it)->writeDatabase();
	}
}

/*
 * 每天0点清除巡捕（左上）
 *
 */
void CCountryM::clearCatcher()
{
	for (std::vector<CCountry*>::iterator it=countries.begin(); it!=countries.end(); it++)
	{
	//	(*it)->cancelCatcher();
	}
}

/*
 * 每天0点清除巡捕（左下）
 *
 */
void CCountryM::clearCatchex()
{
	for (std::vector<CCountry*>::iterator it=countries.begin(); it!=countries.end(); it++)
	{
	//	(*it)->cancelCatchex();
	}
}

void CCountryM::refreshGeneral(DWORD country)
{
	if (0==country)//0则刷新所有
	{
		for (DWORD i=0; i<countries.size(); i++)
		{
			if (countries[i])
				countries[i]->refreshGeneral();
		}
	}
	else
	{
		if (6==country) return;
		CCountry* pCountry = find(country);
		if (!pCountry) return;

		pCountry->refreshGeneral();
	}

	/*
	SceneSession * scene = SceneSessionManager::getInstance()->getSceneByID((country<<16)+KING_CITY_ID);
	if (scene)
	{
		Cmd::Session::t_refreshGen_SceneSession send;
		send.dwCountryID = country;
		send.level = pCountry->gen_level;
		send.exp = pCountry->gen_exp;
		send.maxExp = pCountry->gen_maxexp;

		scene->sendCmd(&send, sizeof(send));

		pCountry->gen_refreshTime = 0;
#ifdef _XWL_DEBUG
		Zebra::logger->trace("刷新大将军 map=%s level=%u", scene->name, pCountry->gen_level);
#endif
	}
	else
		Zebra::logger->error("刷新大将军时未找到地图 mapID=%u", (country<<16)+KING_CITY_ID);
		*/
}

//soke 玩家请求世界太庙资源列表
bool CCountryM::processLoong( UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen )
{	
    switch (ptNullCmd->byParam) 
    {
    case Cmd::REQ_LOONGALTAR_LIST_PARA:
        {
            Cmd::ReqAltarListLoongUserCmd* rev = (Cmd::ReqAltarListLoongUserCmd *)ptNullCmd;
            if ( pUser && rev )
            {
                BUFFER_CMD(Cmd::ReplyAltarListLoongUserCmd ,pSend , zSocket::MAX_USERDATASIZE);
                pSend->altarNum = 0;
                for (std::vector<CCountry*>::iterator it=countries.begin(); it!=countries.end(); ++it)
                {
                    CCountry* pCoun = *it;
                    if ( pCoun )
                    {
                        bcopy(pCoun->name, (pSend->altarList + pSend->altarNum)->counName, MAX_NAMESIZE );
                        (pSend->altarList + pSend->altarNum )->pearlNum = pCoun->pearlNum;
                        pSend->altarNum ++;
                    }
                }
                pUser->sendCmdToMe( pSend, sizeof(Cmd::ReplyAltarListLoongUserCmd) + pSend->altarNum * sizeof(pSend->altarList[0]) );
            }
            return true;
        }
        break;
	case Cmd::REQ_ZHANLI_LIST_PARA://2023-05-10 add by zhanli 排行榜读取1000名
        {
            Cmd::ReqAltarListLoongUserCmd* rev = (Cmd::ReqAltarListLoongUserCmd *)ptNullCmd;
            if ( pUser && rev )
            {
                BUFFER_CMD(Cmd::ReplyAltarListZhanLIUserCmd ,pSend , zSocket::MAX_USERDATASIZE);
                pSend->altarNum = 0;
				DWORD infoSize = 100; //排行榜读取数据
				Cmd::SelectUserInfoA info[infoSize];
				{
					DWORD retcode = 0;
					getZhanliRank(retcode,&info[0],infoSize);
					if ((unsigned int)-1 == retcode
							|| 0 == retcode)
					{
						Zebra::logger->debug("没有找到记录");

						//ret10->num = 0;
						bzero(info, sizeof(Cmd::SelectUserInfoA)*100); //排行榜读取数据
					
					}
					else
					{
						Zebra::logger->debug("找到 %u 条合格的角色记录", retcode);
						
						//ret10->num = retcode;
						//add 2023-05-12 zhanli
						for(int i=0;i<retcode;i++)
						{
							//Zebra::logger->debug("*****-%d-****找到mapName=%d;name=%s;unionid=%u;zhanlilNum=%u;", i,info[i].country,info[i].name,info[i].unionid,info[i].zhanli);
							//bcopy(info[i].mapName, (pSend->altarList + pSend->altarNum)->counName, MAX_NAMESIZE );
							if(info[i].CHARID>100)//GM不参与
							{
								char  mapName[MAX_NAMESIZE];	/// 角色所在地图名称
								if(info[i].country == 8)
								{
									bcopy("汉国", mapName, MAX_NAMESIZE );
								}
								else if(info[i].country == 10)
								{
									bcopy("赵国", mapName, MAX_NAMESIZE );
								}
								else if(info[i].country == 12)
								{
									bcopy("楚国", mapName, MAX_NAMESIZE );
								}
								bcopy(mapName, (pSend->altarList + pSend->altarNum)->counName, MAX_NAMESIZE );
								
								
								bcopy(info[i].name, (pSend->altarList + pSend->altarNum)->name, MAX_NAMESIZE );
								//(pSend->altarList + pSend->altarNum )->counName = ret10.info[i].country; 
								//(pSend->altarList + pSend->altarNum )->bitmask = ret10.info[i].bitmask;
								if(info[i].unionid>0)
								{
									CUnion* pUnion = CUnionM::getMe().getUnionByID(info[i].unionid);
									if(pUnion)
										strncpy((pSend->altarList + pSend->altarNum )->unionname, pUnion->name, MAX_NAMESIZE);
								}

								(pSend->altarList + pSend->altarNum )->charid = info[i].CHARID;
								(pSend->altarList + pSend->altarNum )->unionid = info[i].unionid;
								(pSend->altarList + pSend->altarNum )->zhanlilNum = info[i].zhanli;
								Zebra::logger->debug("*****-%d-**pSend**找到mapName=%s;name=%s;unionid=%u;zhanlilNum=%u;", i,(pSend->altarList + pSend->altarNum )->counName,(pSend->altarList + pSend->altarNum )->name,(pSend->altarList + pSend->altarNum )->unionid,(pSend->altarList + pSend->altarNum )->zhanlilNum);
								pSend->altarNum ++;
								//CountryTechM::getMe().addZhanLi(i,temp);
							}
							
						}

					}
					
						//ret10.accid=accid;

				#if 0
					ret.num=1;
					std::string s;
					ret.info[0].id=rev->accid*2;
					char name[32];
					snprintf(name,32,"测试用户%d",ret.info[0].id);
					s=name;
					strcpy((char *)ret.info[0].name,s.c_str());
					ret.info[0].level=1;
					ret.info[0].mapid=1;
					s="测试地图";
					strcpy((char *)ret.info[0].mapName,s.c_str());
				#endif


						//return sendCmd(&ret10,sizeof(ret10));
					
				}
				//end
                pUser->sendCmdToMe( pSend, sizeof(Cmd::ReplyAltarListZhanLIUserCmd) + pSend->altarNum * sizeof(pSend->altarList[0]) );
            }
            return true;
        }
        break;
    case Cmd::REQ_LEVELUP_LIST_PARA: 
        {
            Cmd::ReqAltarListLoongUserCmd* rev = (Cmd::ReqAltarListLoongUserCmd *)ptNullCmd;
            if ( pUser && rev )
            {
                BUFFER_CMD(Cmd::ReplyAltarListLevelupUserCmd ,pSend , zSocket::MAX_USERDATASIZE);
                pSend->altarNum = 0;
				DWORD infoSize = 100; //排行榜读取数据
				Cmd::SelectUserInfoLevel info[infoSize];
				{
					DWORD retcode = 0;
					getLevelupRank(retcode,&info[0],infoSize);
					if ((unsigned int)-1 == retcode
							|| 0 == retcode)
					{
						Zebra::logger->debug("没有找到记录");

						//ret10->num = 0;
						bzero(info, sizeof(Cmd::SelectUserInfoLevel)*100); //排行榜读取数据
					
					}
					else
					{
						Zebra::logger->debug("找到 %u 条合格的角色记录", retcode);
						
						//ret10->num = retcode;
						//add 2023-05-12 levelup
						for(int i=0;i<retcode;i++)
						{
							//Zebra::logger->debug("*****-%d-****找到mapName=%d;name=%s;unionid=%u;leveluplNum=%u;", i,info[i].country,info[i].name,info[i].unionid,info[i].levelup);
							//bcopy(info[i].mapName, (pSend->altarList + pSend->altarNum)->counName, MAX_NAMESIZE );
							if(info[i].CHARID>100)//GM不参与
							{
								char  mapName[MAX_NAMESIZE];	/// 角色所在地图名称
								if(info[i].country == 8)
								{
									bcopy("汉国", mapName, MAX_NAMESIZE );
								}
								else if(info[i].country == 10)
								{
									bcopy("赵国", mapName, MAX_NAMESIZE );
								}
								else if(info[i].country == 12)
								{
									bcopy("楚国", mapName, MAX_NAMESIZE );
								}
								bcopy(mapName, (pSend->altarList + pSend->altarNum)->counName, MAX_NAMESIZE );
								
								
								bcopy(info[i].name, (pSend->altarList + pSend->altarNum)->name, MAX_NAMESIZE );
								//(pSend->altarList + pSend->altarNum )->counName = ret10.info[i].country; 
								//(pSend->altarList + pSend->altarNum )->bitmask = ret10.info[i].bitmask;
								if(info[i].unionid>0)
								{
									CUnion* pUnion = CUnionM::getMe().getUnionByID(info[i].unionid);
									if(pUnion)
										strncpy((pSend->altarList + pSend->altarNum )->unionname, pUnion->name, MAX_NAMESIZE);
								}

								(pSend->altarList + pSend->altarNum )->charid = info[i].CHARID;
								(pSend->altarList + pSend->altarNum )->unionid = info[i].unionid;
								(pSend->altarList + pSend->altarNum )->levelupNum = info[i].level;
								Zebra::logger->debug("*****-%d-**pSend**找到mapName=%s;name=%s;unionid=%u;level=%u;", i,(pSend->altarList + pSend->altarNum )->counName,(pSend->altarList + pSend->altarNum )->name,(pSend->altarList + pSend->altarNum )->unionid,(pSend->altarList + pSend->altarNum )->levelupNum);
								pSend->altarNum ++;
								//CountryTechM::getMe().addZhanLi(i,temp);
							}
							
						}

					}
					
						//ret10.accid=accid;

				#if 0
					ret.num=1;
					std::string s;
					ret.info[0].id=rev->accid*2;
					char name[32];
					snprintf(name,32,"测试用户%d",ret.info[0].id);
					s=name;
					strcpy((char *)ret.info[0].name,s.c_str());
					ret.info[0].level=1;
					ret.info[0].mapid=1;
					s="测试地图";
					strcpy((char *)ret.info[0].mapName,s.c_str());
				#endif


						//return sendCmd(&ret10,sizeof(ret10));
					
				}
				//end
                pUser->sendCmdToMe( pSend, sizeof(Cmd::ReplyAltarListLevelupUserCmd) + pSend->altarNum * sizeof(pSend->altarList[0]) );
            }
            return true;
        }
        break;	
	case Cmd::REQ_TICKETUP_LIST_PARA: 
        {
            Cmd::ReqAltarListLoongUserCmd* rev = (Cmd::ReqAltarListLoongUserCmd *)ptNullCmd;
            if ( pUser && rev )
            {
                BUFFER_CMD(Cmd::ReplyAltarListTicketupUserCmd ,pSend , zSocket::MAX_USERDATASIZE);
                pSend->altarNum = 0;
				DWORD infoSize = 100; //排行榜读取数据
				Cmd::SelectUserInfoTicket info[infoSize];
				{
					DWORD retcode = 0;
					getTicketupRank(retcode,&info[0],infoSize);
					if ((unsigned int)-1 == retcode || 0 == retcode)
					{
						Zebra::logger->debug("没有找到记录");

						//ret10->num = 0;
						bzero(info, sizeof(Cmd::SelectUserInfoTicket)*100); //排行榜读取数据
					
					}
					else
					{
						Zebra::logger->debug("找到 %u 条合格的角色记录", retcode);

						//ret10->num = retcode;
						//add 2023-05-12 ticketup
						for(int i=0;i<retcode;i++)
						{
							//Zebra::logger->debug("*****-%d-****找到mapName=%d;name=%s;unionid=%u;ticketuplNum=%u;", i,info[i].country,info[i].name,info[i].unionid,info[i].ticketup);
							//bcopy(info[i].mapName, (pSend->altarList + pSend->altarNum)->counName, MAX_NAMESIZE );
							if(info[i].CHARID>100)//GM不参与
							{
								char  mapName[MAX_NAMESIZE];	/// 角色所在地图名称
								if(info[i].country == 8)
								{
									bcopy("汉国", mapName, MAX_NAMESIZE );
								}
								else if(info[i].country == 10)
								{
									bcopy("赵国", mapName, MAX_NAMESIZE );
								}
								else if(info[i].country == 12)
								{
									bcopy("楚国", mapName, MAX_NAMESIZE );
								}
								bcopy(mapName, (pSend->altarList + pSend->altarNum)->counName, MAX_NAMESIZE );
								
								
								bcopy(info[i].name, (pSend->altarList + pSend->altarNum)->name, MAX_NAMESIZE );
								//(pSend->altarList + pSend->altarNum )->counName = ret10.info[i].country; 
								//(pSend->altarList + pSend->altarNum )->bitmask = ret10.info[i].bitmask;
								if(info[i].unionid>0)
								{
									CUnion* pUnion = CUnionM::getMe().getUnionByID(info[i].unionid);
									if(pUnion)
										strncpy((pSend->altarList + pSend->altarNum )->unionname, pUnion->name, MAX_NAMESIZE);
								}

								(pSend->altarList + pSend->altarNum )->charid = info[i].CHARID;
								(pSend->altarList + pSend->altarNum )->unionid = info[i].unionid;
								(pSend->altarList + pSend->altarNum )->ticketupNum = info[i].ticket;
								Zebra::logger->debug("*****-%d-**pSend**找到mapName=%s;name=%s;unionid=%u;ticket=%u;", i,(pSend->altarList + pSend->altarNum )->counName,(pSend->altarList + pSend->altarNum )->name,(pSend->altarList + pSend->altarNum )->unionid,(pSend->altarList + pSend->altarNum )->ticketupNum);
								pSend->altarNum ++;
								//CountryTechM::getMe().addZhanLi(i,temp);
							}
						}

					}
					
						//ret10.accid=accid;

				#if 0
					ret.num=1;
					std::string s;
					ret.info[0].id=rev->accid*2;
					char name[32];
					snprintf(name,32,"测试用户%d",ret.info[0].id);
					s=name;
					strcpy((char *)ret.info[0].name,s.c_str());
					ret.info[0].ticket=1;
					ret.info[0].mapid=1;
					s="测试地图";
					strcpy((char *)ret.info[0].mapName,s.c_str());
				#endif


						//return sendCmd(&ret10,sizeof(ret10));
					
				}
				//end
                pUser->sendCmdToMe( pSend, sizeof(Cmd::ReplyAltarListTicketupUserCmd) + pSend->altarNum * sizeof(pSend->altarList[0]) );
            }
            return true;
        }
        break;
	case Cmd::REQ_FLOWER_LIST_PARA:
        {
            Cmd::ReqAltarListLoongUserCmd* rev = (Cmd::ReqAltarListLoongUserCmd *)ptNullCmd;
            if ( pUser && rev )
            {
                BUFFER_CMD(Cmd::ReplyAltarListFlowerUserCmd ,pSend , zSocket::MAX_USERDATASIZE);
                pSend->altarNum = 0;
				DWORD infoSize = 100; //排行榜读取数据
				Cmd::SelectUserInfoFlower info[infoSize];
				{
					DWORD retcode = 0;
					getFlowerRank(retcode,&info[0],infoSize);
					if ((unsigned int)-1 == retcode || 0 == retcode)
					{
						Zebra::logger->debug("没有找到记录");

						//ret10->num = 0;
						bzero(info, sizeof(Cmd::SelectUserInfoFlower)*100); //排行榜读取数据
					
					}
					else
					{
						Zebra::logger->debug("找到 %u 条合格的角色记录", retcode);

						//ret10->num = retcode;
						//add 2023-05-12 flower
						for(int i=0;i<retcode;i++)
						{
							//Zebra::logger->debug("*****-%d-****找到mapName=%d;name=%s;unionid=%u;flowerlNum=%u;", i,info[i].country,info[i].name,info[i].unionid,info[i].flower);
							//bcopy(info[i].mapName, (pSend->altarList + pSend->altarNum)->counName, MAX_NAMESIZE );
							if(info[i].CHARID>100)//GM不参与
							{
								char  mapName[MAX_NAMESIZE];	/// 角色所在地图名称
								if(info[i].country == 8)
								{
									bcopy("汉国", mapName, MAX_NAMESIZE );
								}
								else if(info[i].country == 10)
								{
									bcopy("赵国", mapName, MAX_NAMESIZE );
								}
								else if(info[i].country == 12)
								{
									bcopy("楚国", mapName, MAX_NAMESIZE );
								}
								bcopy(mapName, (pSend->altarList + pSend->altarNum)->counName, MAX_NAMESIZE );
								
								
								bcopy(info[i].name, (pSend->altarList + pSend->altarNum)->name, MAX_NAMESIZE );
								//(pSend->altarList + pSend->altarNum )->counName = ret10.info[i].country; 
								//(pSend->altarList + pSend->altarNum )->bitmask = ret10.info[i].bitmask;
								if(info[i].unionid>0)
								{
									CUnion* pUnion = CUnionM::getMe().getUnionByID(info[i].unionid);
									if(pUnion)
										strncpy((pSend->altarList + pSend->altarNum )->unionname, pUnion->name, MAX_NAMESIZE);
								}
					
								(pSend->altarList + pSend->altarNum )->charid = info[i].CHARID;
								(pSend->altarList + pSend->altarNum )->gender = info[i].type;
								(pSend->altarList + pSend->altarNum )->unionid = info[i].unionid;
								(pSend->altarList + pSend->altarNum )->flowerNum = info[i].folwers;
								Zebra::logger->debug("*****-%d-**pSend**找到mapName=%s;name=%s;unionid=%u;flower=%u;", i,(pSend->altarList + pSend->altarNum )->counName,(pSend->altarList + pSend->altarNum )->name,(pSend->altarList + pSend->altarNum )->unionid,(pSend->altarList + pSend->altarNum )->flowerNum);
								pSend->altarNum ++;
								//CountryTechM::getMe().addZhanLi(i,temp);
							}
						}

					}
					
						//ret10.accid=accid;

				#if 0
					ret.num=1;
					std::string s;
					ret.info[0].id=rev->accid*2;
					char name[32];
					snprintf(name,32,"测试用户%d",ret.info[0].id);
					s=name;
					strcpy((char *)ret.info[0].name,s.c_str());
					ret.info[0].flower=1;
					ret.info[0].mapid=1;
					s="测试地图";
					strcpy((char *)ret.info[0].mapName,s.c_str());
				#endif


						//return sendCmd(&ret10,sizeof(ret10));
					
				}
				//end
                pUser->sendCmdToMe( pSend, sizeof(Cmd::ReplyAltarListFlowerUserCmd) + pSend->altarNum * sizeof(pSend->altarList[0]) );
            }
            return true;
        }
        break;		
	case Cmd::REQ_HERO_LIST_PARA:
		{
			Cmd::ReqAltarLisHeroUserCmd* rev = (Cmd::ReqAltarLisHeroUserCmd *)ptNullCmd;
            if ( pUser && rev )
			{
				RecordSet* recordset = NULL;
				recordset = CHero::getMe().queryHeroKill(rev->countryID,rev->datevalue);
				if (recordset)
				{
					FieldSet* fs = SessionService::metaData->getFields("CHARBASE");
					connHandleID handle = (connHandleID)-1;
					if (fs)
					{
						handle = SessionService::dbConnPool->getHandle();
						if ((connHandleID)-1 == handle)
						{
							Zebra::logger->error("不能获取数据库句柄");
							return false;
						}
					}
					
					BUFFER_CMD(Cmd::ReplyAltarListHeroUserCmd ,pSend , zSocket::MAX_USERDATASIZE);
                	pSend->altarNum = 0;
					pSend->datevalue = rev->datevalue;

					for (DWORD i = 0; i < recordset->size(); i++)
					{
						Record* recnum = recordset->get(i);
						DWORD num = recnum->get("NUM");
						char name[MAX_NAMESIZE+1] ;
						strncpy(name, recnum->get("CHARNAME"), MAX_NAMESIZE+1);
						DWORD charid = recnum->get("CHARID");
						Zebra::logger->debug("countryid=%d,num=%d,name=%s,charID=%d", 8, num, name,charid);

						bcopy(name, (pSend->altarList + pSend->altarNum)->name, MAX_NAMESIZE+1 );
						
						//获取帮会名
						DWORD unionid = 0;
						RecordSet* recunionid = NULL;
						Record where;   
						std::ostringstream oss; 
						oss << "CHARID='" << charid << "'";
						where.put("CHARID", oss.str());

						if ((connHandleID)-1 != handle)
							recunionid = SessionService::dbConnPool->exeSelect(handle, fs, NULL, &where);
						if (recunionid)
						{
							Record* getunionid = recunionid->get(0);
							unionid = getunionid->get("UNIONID");
						}
						if(unionid > 0 )
						{
							CUnion* pUnion = CUnionM::getMe().getUnionByID(unionid);
							if(pUnion)
							{
								strncpy((pSend->altarList + pSend->altarNum )->unionname, pUnion->name, MAX_NAMESIZE);
							}
							else{
								strncpy((pSend->altarList + pSend->altarNum )->unionname, "无", MAX_NAMESIZE);
							}
							
						}
						else
						{
							strncpy((pSend->altarList + pSend->altarNum )->unionname, "无", MAX_NAMESIZE);
						}

						(pSend->altarList + pSend->altarNum)->killnum = num;

						pSend->altarNum ++;

						//只能获取在线用户
						/* UserSession* pGenUser = UserSessionManager::getInstance()->getUserByID(charid);
						if(pGenUser)
							Zebra::logger->debug("帮会:%d",pGenUser->unionid);
						else
							Zebra::logger->debug("获取失败"); */
					}
					SessionService::dbConnPool->putHandle(handle);
					pUser->sendCmdToMe( pSend, sizeof(Cmd::ReplyAltarListHeroUserCmd) + pSend->altarNum * sizeof(pSend->altarList[0]) );
				}
			}
		}
		break;
    default:
        break;
    }

    return true;
}

bool CCountryM::getZhanliRank(DWORD &retcode,Cmd::SelectUserInfoA *info,DWORD infoSize)
{
	static const dbCol charinfo_define[] = {
		{ "`CHARID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`NAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		//{ "`TYPE`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
		//{ "`LEVEL`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
		//{ "`MAPID`", zDBConnPool::DB_DWORD, sizeof(DWORD) },
		//{ "`MAPNAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) },
		{ "`COUNTRY`", zDBConnPool::DB_WORD, sizeof(WORD) },
		//{ "`FACE`", zDBConnPool::DB_WORD,   sizeof(WORD) },
		//{ "", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) },
		//{ "`BITMASK`", zDBConnPool::DB_DWORD, sizeof(DWORD) },
		{ "`UNIONID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, //add 2023-04-25
		{ "`ZHANLI`", zDBConnPool::DB_DWORD, sizeof(DWORD) },  //zhanli 战力
		{ NULL, 0, 0}
	};
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}
	bzero(where, sizeof(where));
	retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`CHARBASE`", charinfo_define, NULL, "ZHANLI DESC"/* , CHARID DESC" */, infoSize, (BYTE *)info);
	SessionService::dbConnPool->putHandle(handle);
	return true;
}

bool CCountryM::ProBossInfoSystem( UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen )
{	
	switch (ptNullCmd->byParam)
	{
		case REQ_KILLER_BOSSINFO_USERCMD: //客户端请求BOSS存活信息
		{
			Cmd::stReqKillerBossInfoUserCmd* rev = (Cmd::stReqKillerBossInfoUserCmd *)ptNullCmd;
			
			connHandleID handle = SessionService::dbConnPool->getHandle();
			if ((connHandleID)-1 == handle)
			{               
				Zebra::logger->error("[BOSSINFO信息]: 得到数据库句柄失败");
				return false;
			}
           
			BUFFER_CMD(Cmd::stRunKillerBossInfoUserCmd ,pSend , zSocket::MAX_USERDATASIZE);

			RecordSet* recordset = NULL;
			FieldSet* fs = SessionService::metaData->getFields("BOSSINFO");

			Record where;  
			std::ostringstream oss; 
			oss.str("");
			oss << "NPCID != '0'";
			where.put("NPCID", oss.str());

			if ((connHandleID)-1 != handle)
			{
				recordset = SessionService::dbConnPool->exeSelect(handle, fs, NULL, &where);
			}

			if (recordset)
			{  
				for (unsigned int i=0; i<recordset->size(); i++)
				{
					Record* rec = recordset->get(i);
					pSend->m_NpcBossInfo[i].TypeID = rec->get("TYPEID");
					pSend->m_NpcBossInfo[i].Pagenum = rec->get("PAGENUM");
					pSend->m_NpcBossInfo[i].Number = rec->get("NUMBER");
					pSend->m_NpcBossInfo[i].NpcID = rec->get("NPCID");
					pSend->m_NpcBossInfo[i].CountryID = rec->get("COUNTRYID");
					bcopy(rec->get("MAPNAME"), pSend->m_NpcBossInfo[i].MapName, MAX_NAMESIZE+1);
					pSend->m_NpcBossInfo[i].NpcState = rec->get("KILLSTATE");
					pSend->m_NpcBossInfo[i].ReviveTime = rec->get("REVIVETIME");
				}	
				pUser->sendCmdToMe( pSend, sizeof(Cmd::stRunKillerBossInfoUserCmd) *sizeof(pSend->m_NpcBossInfo[64]) );
		    }
		    SessionService::dbConnPool->putHandle(handle);
		}
		break;
    }
    return true;
}

bool CCountryM::getLevelupRank(DWORD &retcode,Cmd::SelectUserInfoLevel *info,DWORD infoSize)
{
	static const dbCol charinfo_define[] = {
		{ "`CHARID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`NAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`LEVEL`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
		{ "`COUNTRY`", zDBConnPool::DB_WORD, sizeof(WORD) },
		{ "`UNIONID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ NULL, 0, 0}
	};
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}
	bzero(where, sizeof(where));
	retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`CHARBASE`", charinfo_define, NULL, "LEVEL DESC"/* , CHARID DESC" */, infoSize, (BYTE *)info);
	SessionService::dbConnPool->putHandle(handle);
	return true;
}

bool CCountryM::getTicketupRank(DWORD &retcode,Cmd::SelectUserInfoTicket *info,DWORD infoSize)
{
	static const dbCol charinfo_define[] = {
		{ "`CHARID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`NAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`TICKET`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`COUNTRY`", zDBConnPool::DB_WORD, sizeof(WORD) },
		{ "`UNIONID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ NULL, 0, 0}
	};
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}
	bzero(where, sizeof(where));
	retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`CHARBASE`", charinfo_define, NULL, "TICKET DESC"/* , CHARID DESC" */, infoSize, (BYTE *)info);
	SessionService::dbConnPool->putHandle(handle);
	return true;
}

bool CCountryM::getFlowerRank(DWORD &retcode,Cmd::SelectUserInfoFlower *info,DWORD infoSize)
{
	static const dbCol charinfo_define[] = {
		{ "`CHARID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`NAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`TYPE`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
		{ "`FOLWERS`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`COUNTRY`", zDBConnPool::DB_WORD, sizeof(WORD) },
		{ "`UNIONID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ NULL, 0, 0}
	};
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}
	bzero(where, sizeof(where));
	retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`CHARBASE`", charinfo_define, "TYPE = 2", "FOLWERS DESC", infoSize, (BYTE *)info);
	SessionService::dbConnPool->putHandle(handle);
	return true;
}


void CCountryM::getAllCountry(std::vector<Cmd::Session::stCountryList> &countrys)
{
	for (unsigned int i=0; i<CCountryM::getMe().countries.size(); i++)
	{
		Cmd::Session::stCountryList temp;
		temp.countryID = CCountryM::getMe().countries[i]->dwID;
		bcopy(temp.countryName, CCountryM::getMe().countries[i]->name, MAX_NAMESIZE+1 );
		countrys.push_back(temp);
	}
}



//------------------------------------------------------------------------------------------------------------


void CCountry::init(Record* rec)
{
	if (rec)
	{
		this->dwID = rec->get("id");
		this->dwKingUnionID  = rec->get("kingunionid");
		this->dwLastDareTime = rec->get("lastdaretime");
		this->dwLastDailyMoney = rec->get("lastdailymoney");
		this->dwLastKingMoney = rec->get("lastkingmoney"); //soke 国王领取每日薪资时间
		this->dwLastYsMoney = rec->get("lastysmoney");     //soke 元帅领取每日薪资时间
		this->dwLastZxMoney = rec->get("lastzxmoney");     //soke 宰相领取每日薪资时间
		this->dwLastCrMoney = rec->get("lastcrmoney");     //soke 捕头（左)领取每日薪资时间
		this->dwLastDtMoney = rec->get("lastdtmoney");     //soke 御史（左)领取每日薪资时间
		this->dwLastCxMoney = rec->get("lastcxmoney");     //soke 捕头（右)领取每日薪资时间
		this->dwLastDxMoney = rec->get("lastdxmoney");     //soke 御史（右)领取每日薪资时间
		this->dwDareTime     = rec->get("daretime");
		this->dwFormalWin   = rec->get("formalwin");
		this->dwFormalFail  = rec->get("formalfail");
		this->dwAnnoyWin    = rec->get("annoywin");
		this->dwAnnoyFail   = rec->get("annoyfail");
		this->dwStar	    = rec->get("star");
		this->dwDareCountryID = rec->get("darecountryid");
		this->dwTax = rec->get("tax");
		this->qwGold = rec->get("gold");
		this->qwSilk = rec->get("silk");
		this->qwOre = rec->get("ore");
		this->qwBowlder = rec->get("bowlder");
		this->qwWood = rec->get("wood");
		this->qwCoat = rec->get("coat");
		this->qwHerbal = rec->get("herbal");
		this->qwMaterial = rec->get("material");
		this->qwStock = rec->get("stock");
		this->forbidTalk = rec->get("forbidtalk");
		this->sendPrison = rec->get("sendprison");
		this->gen_level = rec->get("gen_level");
		this->gen_exp = rec->get("gen_exp");
		this->gen_maxexp = rec->get("gen_maxexp");
		this->calltimes = rec->get("calltimes");
		this->kingtime = rec->get("kingtime");
		this->pearlNum = rec->get("pearlnum"); //soke 太庙资源数量
		
		strncpy(this->name, (const char*)rec->get("name"), MAX_NAMESIZE);
		strncpy(this->kingName, (const char*)rec->get("kingname"), MAX_NAMESIZE);
		strncpy(this->diplomatName, (const char*)rec->get("diplomatname"), MAX_NAMESIZE);
        strncpy(this->censorName, (const char*)rec->get("yushiname1"), MAX_NAMESIZE);
		strncpy(this->censosName, (const char*)rec->get("yushiname2"), MAX_NAMESIZE);
		strncpy(this->catcherName, (const char*)rec->get("catchername1"), MAX_NAMESIZE);
		strncpy(this->catchexName, (const char*)rec->get("catchername2"), MAX_NAMESIZE);
		strncpy(this->yuanshuaiName, (const char*)rec->get("yuanshuainame"), MAX_NAMESIZE);
		strncpy(this->zaixiangName, (const char*)rec->get("zaixiangname"), MAX_NAMESIZE);
		strncpy(this->note, (const char*)rec->get("note"), 255);
	}
}

/** 
  * \brief 更新数据库记录
  * \author 
  */   
void CCountry::writeDatabase()
{
	//FunctionTimes times(48,__FUNCTION__);
	Record rec,where;

	std::ostringstream oss;
	
	oss << "id='" << this->dwID << "'";
	where.put("id", oss.str());

	rec.put("kingname", this->kingName);
	rec.put("diplomatname", this->diplomatName);
    rec.put("yushiname1", this->censorName);
	rec.put("yushiname2", this->censosName);
	rec.put("catchername1", this->catcherName);
	rec.put("catchername2", this->catchexName);
	rec.put("yuanshuainame", this->yuanshuaiName);
	rec.put("zaixiangname", this->zaixiangName);
	rec.put("formalwin", this->dwFormalWin);
	rec.put("formalfail", this->dwFormalFail);
	rec.put("annoywin", this->dwAnnoyWin);
	rec.put("annoyfail", this->dwAnnoyFail);
	rec.put("star", this->dwStar);
	rec.put("lastdaretime", this->dwLastDareTime);
	rec.put("lastdailymoney", this->dwLastDailyMoney);
	rec.put("lastkingmoney", this->dwLastKingMoney); //soke 国王领取每日薪资表
	rec.put("lastysmoney", this->dwLastYsMoney);     //soke 元帅领取每日薪资表
	rec.put("lastzxmoney", this->dwLastZxMoney);     //soke 宰相领取每日薪资表
	rec.put("lastcrmoney", this->dwLastCrMoney);     //soke 捕头（左）领取每日薪资表
	rec.put("lastdtmoney", this->dwLastDtMoney);     //soke 御史（左）领取每日薪资表
	rec.put("lastcxmoney", this->dwLastCxMoney);     //soke 捕头（右）领取每日薪资表
	rec.put("lastdxmoney", this->dwLastDxMoney);     //soke 御史（右）领取每日薪资表
	rec.put("daretime", this->dwDareTime);
	rec.put("darecountryid", this->dwDareCountryID);
	rec.put("kingunionid", this->dwKingUnionID);
	rec.put("tax", this->dwTax);
	rec.put("gold", this->qwGold);
	rec.put("silk", this->qwSilk);
	rec.put("ore", this->qwOre);
	rec.put("bowlder", this->qwBowlder);
	rec.put("wood", this->qwWood);
	rec.put("coat", this->qwCoat);
	rec.put("herbal", this->qwHerbal);
	rec.put("material", this->qwMaterial);
	rec.put("stock", this->qwStock);
	rec.put("note", this->note);
	rec.put("forbidtalk", this->forbidTalk);
	rec.put("sendprison", this->sendPrison);
	rec.put("gen_level", this->gen_level);
	rec.put("gen_exp", this->gen_exp);
	rec.put("gen_maxexp", this->gen_maxexp);
	rec.put("calltimes", this->calltimes);
	rec.put("kingtime" ,this->kingtime);
	rec.put("pearlnum" ,this->pearlNum); //soke 太庙资源数量

	FieldSet* country = SessionService::metaData->getFields("COUNTRY");

	if (country)       
	{       
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return;
		}

		if ((connHandleID)-1 != handle)
		{
			SessionService::dbConnPool->exeUpdate(handle, country, &rec, &where);
		}

		SessionService::dbConnPool->putHandle(handle);
	}
	else
	{
		Zebra::logger->error("国家数据保存失败，COUNTRY表不存在");
		return;
	}
}

bool CCountry::insertDatabase()
{
	//FunctionTimes times(47,__FUNCTION__);
	Record rec;
	rec.put("id", this->dwID);
	rec.put("kingunionid", this->dwKingUnionID);
	rec.put("lastdaretime", this->dwLastDareTime);
	rec.put("daretime", this->dwDareTime);
	rec.put("darecountryid", this->dwDareCountryID);
	rec.put("name", this->name);
	rec.put("kingname", this->kingName);
	rec.put("formalwin", this->dwFormalWin);
	rec.put("formalfail", this->dwFormalFail);
	rec.put("annoywin", this->dwAnnoyWin);
	rec.put("annoyfail", this->dwAnnoyFail);
	rec.put("star", this->dwStar);
	rec.put("gold", this->qwGold);
	rec.put("tax", this->dwTax);
	rec.put("silk", this->qwSilk);
	rec.put("ore", this->qwOre);
	rec.put("bowlder", this->qwBowlder);
	rec.put("wood", this->qwWood);
	rec.put("coat", this->qwCoat);
	rec.put("herbal", this->qwHerbal);
	rec.put("material", this->qwMaterial);
	rec.put("stock", this->qwStock);
	rec.put("note", this->note);
	rec.put("forbidtalk", this->forbidTalk);
	rec.put("sendprison", this->sendPrison);
	rec.put("gen_level", this->gen_level);
	rec.put("gen_exp", this->gen_exp);
	rec.put("gen_maxexp", this->gen_maxexp);
	rec.put("pearlnum", this->pearlNum);

	FieldSet* country = SessionService::metaData->getFields("COUNTRY");

	if (country)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return false;
		}
		else
		{
			SessionService::dbConnPool->exeInsert(handle, country, &rec);
		}

		SessionService::dbConnPool->putHandle(handle);
	}
	else
	{
		Zebra::logger->error("国家数据新建失败，COUNTRY表不存在");
		return false;
	}

	return true;
}

void CCountry::loadTechFromDB()
{
	//FunctionTimes times(46,__FUNCTION__);
	FieldSet* tech = SessionService::metaData->getFields("TECH");

	if (tech)
	{       
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{       
			Zebra::logger->error("不能获取数据库句柄");
			return;
		}               

		RecordSet* recordset = NULL;
		Record where;   
		std::ostringstream oss; 

		oss << "countryid='" << this->dwID << "'";
		where.put("countryid", oss.str());

		if ((connHandleID)-1 != handle)
		{
			recordset = SessionService::dbConnPool->exeSelect(handle, tech, NULL, &where);
		}

		SessionService::dbConnPool->putHandle(handle);
		if (recordset)
		{
			for (unsigned int i=0; i<recordset->size(); i++)
			{
				Record* rec = recordset->get(i);
				CTech* pTech = new CTech();

				if (pTech)
				{
					pTech->init(rec);
				}

				this->addTech(pTech->dwType, pTech);
			}

			SAFE_DELETE(recordset)
		}
	}
	else
	{
		Zebra::logger->error("科技详细数据加载失败，TECH表不存在");
		return;
	}

	return;
}

CTech* CCountry::getTech(DWORD dwType)
{
	CTechMap::iterator pos;
	pos = techIndex.find(dwType);
	
	if (pos != techIndex.end())
	{
		return pos->second;
	}
	
	return NULL;
}

void CCountry::addTech(DWORD dwType, CTech* pTech)
{
	techIndex[dwType] = pTech;
}

void CCountry::updateKing(UserSession* pUser)
{
	if (pUser && strncmp(this->kingName, pUser->name, MAX_NAMESIZE)!=0)
	{
		this->dwKingUnionID = pUser->unionid;
		strncpy(this->kingName, pUser->name, MAX_NAMESIZE);

		this->writeDatabase();
	}
}

bool CCountry::changeKing(UserSession* pUser)
{
	if (pUser == NULL)
	{// 清空国王信息
		rwlock.wrlock();
		this->dwKingUnionID = 0;
		bzero(kingName, sizeof(kingName));
		this->writeDatabase();
		rwlock.unlock();
	}
	else
	{
		CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);
		if (pUser && pUnion)
		{
			rwlock.wrlock();
			this->dwKingUnionID = pUser->unionid;
			
			if (pUnion->master)
			{
				strncpy(kingName, pUnion->master->name, MAX_NAMESIZE);
			}

			this->kingtime=SessionTimeTick::currentTime.sec();
			this->writeDatabase();
			rwlock.unlock();
			//soke 修改显示玩家名
			pUser->sendSysChat(Cmd::INFO_TYPE_EXP, "恭喜%s成为 %s 国王", kingName,this->name);
		//	SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_ZISETROLL, pUser->country, "恭喜%s登上 %s 国王宝座，一统江山，指日可待。", kingName,this->name);
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//soke 有了国王以后夺城或者放弃在占领全新公告 支持时间判断 显示不同公告（系统红色滚动公告）
			{
			 time_t timValue = time(NULL);
			 struct tm tmValue; 
			 zRTime::getLocalTime(tmValue, timValue);
			 if (tmValue.tm_hour<6)
			 	SessionChannel::sendAllInfo(Cmd::INFO_TYPE_ZISETROLL, "哇～ %s登上 %s 国王宝座，真是太厉害了 ", kingName,this->name);
			 else if (tmValue.tm_hour<11)
				SessionChannel::sendAllInfo(Cmd::INFO_TYPE_ZISETROLL, "哇～ %s的 %s 在王城登基称王啦，太不可思议了 ",this->name, kingName);
			 else if (tmValue.tm_hour<15)
				SessionChannel::sendAllInfo(Cmd::INFO_TYPE_ZISETROLL, "天哪～ %s的 %s 荣登王位，九天十地，唯我独尊 ",this->name, kingName);
			 else if (tmValue.tm_hour<20)
				SessionChannel::sendAllInfo(Cmd::INFO_TYPE_ZISETROLL, "真牛～ %s的 %s 登上王位，千秋霸业，指日可待 ", kingName,this->name);
			 else//soke 最后没有指定时间的 在加载下面这个公告
				SessionChannel::sendAllInfo(Cmd::INFO_TYPE_ZISETROLL, "恭喜%s登上 %s 国王宝座，一统天下，谁与争锋 ", kingName,this->name);
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if (pUnion->master) pUnion->master->update_data();
		}
	}
	
	return true;
}

bool CCountry::changeEmperor(DWORD dwCountryID)
{
	if (this->dwID == NEUTRAL_COUNTRY_ID)
	{
		CCountry* pCountry = CCountryM::getMe().find(dwCountryID);
		CUnion* pUnion = CUnionM::getMe().getUnionByID(pCountry->dwKingUnionID);
		CUnion* pOldUnion = CUnionM::getMe().getUnionByID(this->dwKingUnionID);

		if (pCountry && pUnion)
		{
			rwlock.wrlock();
			this->dwKingUnionID = pCountry->dwKingUnionID;
			if (pUnion->master)
			{
				strncpy(kingName, pUnion->master->name, MAX_NAMESIZE);
			}
			rwlock.unlock();

			this->writeDatabase();

			if (pUnion->master) pUnion->master->update_data();
			if (pOldUnion && pOldUnion->master) pOldUnion->master->update_data();
			DWORD dwMapID = (NEUTRAL_COUNTRY_ID<<16) + 134;
			SceneSession *pScene = SceneSessionManager::getInstance()->getSceneByID(dwMapID);
			if (pScene)
			{
				Cmd::Session::t_setEmperorHold_SceneSession send;
				send.dwCountryID = dwCountryID;
				pScene->sendCmd(&send, sizeof(send));
			}

		}
	}
	
	return true;
}

//soke 2016 - 3 - 11 罢免外交官（停用）
bool CCountry::cancelDiplomat()
{
	UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->diplomatName);

	if (u && u->scene)
	{
		Cmd::Session::t_setDiplomatState_SceneSession send;
		send.byState = 0;
		send.dwUserID = u->id;
		u->scene->sendCmd(&send, sizeof(send));
		u->sendSysChat(Cmd::INFO_TYPE_GAME, "你被国王罢免外交官之职");
	}

	rwlock.wrlock();
	bzero(this->diplomatName, MAX_NAMESIZE);
	rwlock.unlock();
	this->writeDatabase();

	return true;
}


//soke 2016 - 3 - 11 国王罢免巡捕（左上）
bool CCountry::cancelCatcher()
{
	UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->catcherName);

	if (u && u->scene)
	{
		Cmd::Session::t_setCatcherState_SceneSession send;
		send.byState = 0;
		send.dwUserID = u->id;
		u->scene->sendCmd(&send, sizeof(send));
		u->sendSysChat(Cmd::INFO_TYPE_GAME, "你被国王罢免捕头之职");
	}

	rwlock.wrlock();
	bzero(this->catcherName, MAX_NAMESIZE);
	rwlock.unlock();
	this->writeDatabase();

	return true;
}

//soke 2016 - 3 - 11 国王罢免巡捕（左下）
bool CCountry::cancelCatchex()
{
	UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->catchexName);

	if (u && u->scene)
	{
		Cmd::Session::t_setCatchexState_SceneSession send;
		send.byState = 0;
		send.dwUserID = u->id;
		u->scene->sendCmd(&send, sizeof(send));
		u->sendSysChat(Cmd::INFO_TYPE_GAME, "你被国王罢免捕头之职");
	}

	rwlock.wrlock();
	bzero(this->catchexName, MAX_NAMESIZE);
	rwlock.unlock();
	this->writeDatabase();

	return true;
}

//soke 2016 - 3 - 11 国王罢免御史大夫（右上）
bool CCountry::cancelCensor()
{
	UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->censorName);

	if (u && u->scene)
	{
		Cmd::Session::t_setCensorState_SceneSession send;
		send.byState = 0;
		send.dwUserID = u->id;
		u->scene->sendCmd(&send, sizeof(send));
		u->sendSysChat(Cmd::INFO_TYPE_GAME, "你被国王罢免御史大夫之职");
	}

	rwlock.wrlock();
	bzero(this->censorName, MAX_NAMESIZE);
	rwlock.unlock();
	this->writeDatabase();

	return true;
}

//soke 2016 - 3 - 11 国王罢免御史大夫（右下）
bool CCountry::cancelCensos()
{
	UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->censosName);

	if (u && u->scene)
	{
		Cmd::Session::t_setCensosState_SceneSession send;
		send.byState = 0;
		send.dwUserID = u->id;
		u->scene->sendCmd(&send, sizeof(send));
		u->sendSysChat(Cmd::INFO_TYPE_GAME, "你被国王罢免御史大夫之职");
	}

	rwlock.wrlock();
	bzero(this->censosName, MAX_NAMESIZE);
	rwlock.unlock();
	this->writeDatabase();

	return true;
}

//soke 2016 - 3 - 11 国王罢免元帅
bool CCountry::cancelYuanshuai()
{
	UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->yuanshuaiName);

	if (u && u->scene)
	{
		Cmd::Session::t_setYuanshuaiState_SceneSession send;
		send.byState = 0;
		send.dwUserID = u->id;
		u->scene->sendCmd(&send, sizeof(send));
		u->sendSysChat(Cmd::INFO_TYPE_GAME, "你被国王罢免元帅之职");
	}

	rwlock.wrlock();
	bzero(this->yuanshuaiName, MAX_NAMESIZE);
	rwlock.unlock();
	this->writeDatabase();

	return true;
}

//soke 2016 - 3 - 11 国王罢免宰相
bool CCountry::cancelZaixiang()
{
	UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->zaixiangName);

	if (u && u->scene)
	{
		Cmd::Session::t_setZaixiangState_SceneSession send;
		send.byState = 0;
		send.dwUserID = u->id;
		u->scene->sendCmd(&send, sizeof(send));
		u->sendSysChat(Cmd::INFO_TYPE_GAME, "你被国王罢免宰相之职");
	}

	rwlock.wrlock();
	bzero(this->zaixiangName, MAX_NAMESIZE);
	rwlock.unlock();
	this->writeDatabase();

	return true;
}

//soke 2016 - 3 - 11 任命外交官（停用）
bool CCountry::changeDiplomat(UserSession* pUser)
{
	if (strncmp(pUser->name, this->diplomatName, MAX_NAMESIZE) != 0)
	{
#ifdef _ZJW_DEBUG
		Zebra::logger->debug("[国家]:%s 被任命为外交官", pUser->name);
#endif		
		UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->diplomatName);
		if (u && u->scene)
		{
			Cmd::Session::t_setDiplomatState_SceneSession send;
			send.byState = 0;
			send.dwUserID = u->id;
			u->scene->sendCmd(&send, sizeof(send));
		}
		
		rwlock.wrlock();
		strncpy(this->diplomatName, pUser->name, MAX_NAMESIZE);
		rwlock.unlock();

		Cmd::Session::t_setDiplomatState_SceneSession send;
		if (pUser->scene) 
		{
			send.byState = 1;
			send.dwUserID = pUser->id;
			pUser->scene->sendCmd(&send, sizeof(send));

			pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "您被国王任命为外交官");
		}
	}

	this->writeDatabase();

	return true;
}

//soke 2016 - 3 - 11 国王任命巡捕（左上）
bool CCountry::changeCatcher(UserSession* pUser)
{
	if (strncmp(pUser->name, catcherName, MAX_NAMESIZE) != 0)
	{
		UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->catcherName);
		if (u && u->scene)
		{
			Cmd::Session::t_setCatcherState_SceneSession send;
			send.byState = 0;
			send.dwUserID = u->id;
			u->scene->sendCmd(&send, sizeof(send));
		}
		
		rwlock.wrlock();
		strncpy(this->catcherName, pUser->name, MAX_NAMESIZE);
		rwlock.unlock();

		Cmd::Session::t_setCatcherState_SceneSession send;
		if (pUser->scene) 
		{
			send.byState = 1;
			send.dwUserID = pUser->id;
			pUser->scene->sendCmd(&send, sizeof(send));

			pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你被国王任命为捕头");

			///////////////////////////////////////////////////////////////////
			//soke 增加国家公告	
			Cmd::Session::t_countryPunishUser_SceneSession send;

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, pUser->country, "奉天承运，国王诏曰：任命%s为捕头，即日上任，钦此.", pUser->name);
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "奉天承运，国王诏曰：任命%s为捕头，即日上任，钦此", pUser->name);
			///////////////////////////////////////////////////////////////////

		}
	}

    this->writeDatabase();

	return true;	

}

//soke 2016 - 3 - 11 国王任命巡捕（左下）
bool CCountry::changeCatchex(UserSession* pUser)
{
	if (strncmp(pUser->name, catchexName, MAX_NAMESIZE) != 0)
	{
		UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->catchexName);
		if (u && u->scene)
		{
			Cmd::Session::t_setCatchexState_SceneSession send;
			send.byState = 0;
			send.dwUserID = u->id;
			u->scene->sendCmd(&send, sizeof(send));
		}
		
		rwlock.wrlock();
		strncpy(this->catchexName, pUser->name, MAX_NAMESIZE);
		rwlock.unlock();

		Cmd::Session::t_setCatchexState_SceneSession send;
		if (pUser->scene) 
		{
			send.byState = 1;
			send.dwUserID = pUser->id;
			pUser->scene->sendCmd(&send, sizeof(send));

			pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你被国王任命为捕头");

			///////////////////////////////////////////////////////////////////
			//soke 增加国家公告	
			Cmd::Session::t_countryPunishUser_SceneSession send;

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, pUser->country, "奉天承运，国王诏曰：任命%s为捕头，即日上任，钦此.", pUser->name);
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "奉天承运，国王诏曰：任命%s为捕头，即日上任，钦此", pUser->name);
			///////////////////////////////////////////////////////////////////

		}
	}

    this->writeDatabase();
	
	return true;
	
}

//soke 2016 - 3 - 11 国王任命御史大夫（右上）
bool CCountry::changeCensor(UserSession* pUser)
{
	if (strncmp(pUser->name, this->censorName, MAX_NAMESIZE) != 0)
	{
#ifdef _ZJW_DEBUG
		Zebra::logger->debug("[国家]:%s 被任命为御史大夫", pUser->name);
#endif		
		UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->censorName);
		if (u && u->scene)
		{
			Cmd::Session::t_setCensorState_SceneSession send;
			send.byState = 0;
			send.dwUserID = u->id;
			u->scene->sendCmd(&send, sizeof(send));
		}
		
		rwlock.wrlock();
		strncpy(this->censorName, pUser->name, MAX_NAMESIZE);
		rwlock.unlock();

		Cmd::Session::t_setCensorState_SceneSession send;
		if (pUser->scene) 
		{
			send.byState = 1;
			send.dwUserID = pUser->id;
			pUser->scene->sendCmd(&send, sizeof(send));

			pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你被国王任命为御史大夫");

			///////////////////////////////////////////////////////////////////
			//soke 增加国家公告	
			Cmd::Session::t_countryPunishUser_SceneSession send;

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, pUser->country, "奉天承运，国王诏曰：任命%s为御史大夫，即日上任，钦此.", pUser->name);
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "奉天承运，国王诏曰：任命%s为御史大夫，即日上任，钦此", pUser->name);
			///////////////////////////////////////////////////////////////////
		}
	}

	this->writeDatabase();

	return true;
}

//soke 2016 - 3 - 11 国王任命御史大夫（右下）
bool CCountry::changeCensos(UserSession* pUser)
{
	if (strncmp(pUser->name, this->censosName, MAX_NAMESIZE) != 0)
	{
#ifdef _ZJW_DEBUG
		Zebra::logger->debug("[国家]:%s 被任命为御史大夫", pUser->name);
#endif		
		UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->censosName);
		if (u && u->scene)
		{
			Cmd::Session::t_setCensosState_SceneSession send;
			send.byState = 0;
			send.dwUserID = u->id;
			u->scene->sendCmd(&send, sizeof(send));
		}
		
		rwlock.wrlock();
		strncpy(this->censosName, pUser->name, MAX_NAMESIZE);
		rwlock.unlock();

		Cmd::Session::t_setCensosState_SceneSession send;
		if (pUser->scene) 
		{
			send.byState = 1;
			send.dwUserID = pUser->id;
			pUser->scene->sendCmd(&send, sizeof(send));

			pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你被国王任命为御史大夫");

			///////////////////////////////////////////////////////////////////
			//soke 增加国家公告	
			Cmd::Session::t_countryPunishUser_SceneSession send;

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, pUser->country, "奉天承运，国王诏曰：任命%s为御史大夫，即日上任，钦此.", pUser->name);
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "奉天承运，国王诏曰：任命%s为御史大夫，即日上任，钦此", pUser->name);
			///////////////////////////////////////////////////////////////////
		}
	}

	this->writeDatabase();

	return true;
}

//soke 2016 - 3 - 11 国王任命元帅
bool CCountry::changeYuanshuai(UserSession* pUser)
{
	if (strncmp(pUser->name, yuanshuaiName, MAX_NAMESIZE) != 0)
	{
		UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->yuanshuaiName);
		if (u && u->scene)
		{
			Cmd::Session::t_setYuanshuaiState_SceneSession send;
			send.byState = 0;
			send.dwUserID = u->id;
			u->scene->sendCmd(&send, sizeof(send));
		}
		
		rwlock.wrlock();
		strncpy(this->yuanshuaiName, pUser->name, MAX_NAMESIZE);
		rwlock.unlock();

		Cmd::Session::t_setYuanshuaiState_SceneSession send;
		if (pUser->scene) 
		{
			send.byState = 1;
			send.dwUserID = pUser->id;
			pUser->scene->sendCmd(&send, sizeof(send));

			pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你被国王任命为元帅");

			///////////////////////////////////////////////////////////////////
			//soke 增加国家公告	
			Cmd::Session::t_countryPunishUser_SceneSession send;

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, pUser->country, "奉天承运，国王诏曰：任命%s为元帅，即日上任，钦此.", pUser->name);
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "奉天承运，国王诏曰：任命%s为元帅，即日上任，钦此", pUser->name);
			///////////////////////////////////////////////////////////////////

		}
	}

    this->writeDatabase();
	
	return true;
	
}

//soke 2016 - 3 - 11 国王任命宰相
bool CCountry::changeZaixiang(UserSession* pUser)
{
	if (strncmp(pUser->name, zaixiangName, MAX_NAMESIZE) != 0)
	{
		UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->zaixiangName);
		if (u && u->scene)
		{
			Cmd::Session::t_setZaixiangState_SceneSession send;
			send.byState = 0;
			send.dwUserID = u->id;
			u->scene->sendCmd(&send, sizeof(send));
		}
		
		rwlock.wrlock();
		strncpy(this->zaixiangName, pUser->name, MAX_NAMESIZE);
		rwlock.unlock();

		Cmd::Session::t_setZaixiangState_SceneSession send;
		if (pUser->scene) 
		{
			send.byState = 1;
			send.dwUserID = pUser->id;
			pUser->scene->sendCmd(&send, sizeof(send));

			pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你被国王任命为宰相");

			///////////////////////////////////////////////////////////////////
			//soke 增加国家公告	
			Cmd::Session::t_countryPunishUser_SceneSession send;

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, pUser->country, "奉天承运，国王诏曰：任命%s为宰相，即日上任，钦此.", pUser->name);
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "奉天承运，国王诏曰：任命%s为宰相，即日上任，钦此", pUser->name);
			///////////////////////////////////////////////////////////////////

		}
	}

    this->writeDatabase();
	
	return true;
	
}


bool CCountry::isKing(UserSession* pUser)
{
	CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);
	
	if (!pUnion)
	{
		return false;
	}
	
	// TODO:判断是否是城主或国王
	if (pUnion->master && pUnion->master->id == pUser->id)  
	{//是帮主
		if (CCityM::getMe().find(pUser->country, KING_CITY_ID, pUser->unionid) !=NULL)
		{//是国王
			return true;
		}
	}

	return false;
}

bool CCountry::isOfficial(UserSession* pUser)
{
	CTechMap::iterator pos;

	for (pos = techIndex.begin(); pos!=techIndex.end(); ++pos)
	{
		CTech* pTech = (CTech*)pos->second;
		if (pTech && pTech->dwResearchID == pUser->id)
		{
			return true;
		}
	}

	return false;
}

bool CCountry::isMe(DWORD country)
{
	if (this->dwID==country)
		return true;

	return false;
}

void CCountry::endDare()
{
/*	CDare* pDare = CDareM::getMe().findDareRecordByRelation(Cmd::COUNTRY_FORMAL_DARE, dwDareCountryID, dwID);

	if (pDare)
	{
		pDare->setReadyOverState();
	}
*/
#ifndef _ZJW_DEBUG	
	time_t timValue  = time(NULL);
	if (abs(timValue-this->dwLastDareTime) > 24*60*60)
#endif		
	{
		isBeging = false;
		dwDareCountryID = 0;
		this->writeDatabase();
	}
}

void CCountry::beginDare()
{
	if (this->dwID == NEUTRAL_COUNTRY_ID)
	{//如果是中立国,每到周六,开始皇城争夺战
#ifndef _ZJW_DEBUG	
		struct tm tv1;
		time_t timValue = time(NULL);
		zRTime::getLocalTime(tv1, timValue);
		if (tv1.tm_wday == 6)
#endif		
		{
		std::vector<DWORD> dare_list;
		isBeging = true;	
		Cmd::Session::t_createDare_SceneSession pCmd;
        //皇城战总时间 2小时
		pCmd.active_time	=	7200;
		
		pCmd.ready_time 	=       1;
		
		pCmd.relationID2        =       this->dwID;
		pCmd.type               =       Cmd::EMPEROR_DARE;
	
		CDareM::getMe().createDare_sceneSession(&pCmd, dare_list);
		}
	}
	else
	{
#ifndef _ZJW_DEBUG	
	time_t timValue  = time(NULL);
	if (dwDareCountryID>0 && !isBeging 
	&& (abs(timValue-this->dwLastDareTime) > 24*60*60)
	)
#endif		
	{
		std::vector<DWORD> dare_list;
		dare_list.push_back(this->dwDareCountryID);
		isBeging = true;	
		Cmd::Session::t_createDare_SceneSession pCmd;
        // 国战总时间改为1.5小时	
		pCmd.active_time    =   5400;
	
		pCmd.ready_time =       1;
		
		pCmd.relationID2        =       this->dwID;
		pCmd.type               =       Cmd::COUNTRY_FORMAL_DARE;
	
		CDareM::getMe().createDare_sceneSession(&pCmd, dare_list);

		CCountry* pCountry = CCountryM::getMe().find(this->dwDareCountryID);
		if (pCountry)
		{
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, this->dwID, 
					"%s 攻打 我国 的国战现在开始,注意防守", 
					pCountry->name);

			SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, "%s 攻打 %s 的国战现在开始。", 
					pCountry->name,
					this->name);
		}

		SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, this->dwDareCountryID,
				"我国 攻打 %s 的国战现在开始", this->name);

		// TODO:取消掉外交官(停用)
		this->cancelDiplomat();
		//soke 2016 - 3 - 11 TODO:取消掉御史大夫（右上）
		this->cancelCensor();
		//soke 2016 - 3 - 11 TODO:取消掉御史大夫（右下）
		this->cancelCensos();
		//soke 2016 - 3 - 11 TODO:取消掉元帅
		this->cancelYuanshuai();
		//soke 2016 - 3 - 11 TODO:取消掉宰相
		this->cancelZaixiang();
	}
	}
}


/**
 * \brief 开始反攻
 *
 */
void CCountry::beginAntiDare(DWORD dwAttCountry)
{
	std::vector<DWORD> dare_list;
	dare_list.push_back(dwAttCountry);

	Cmd::Session::t_createDare_SceneSession pCmd;
    //反攻总时间 1小时
	pCmd.active_time	=	3600;
		
	pCmd.ready_time =       1;

	pCmd.relationID2        =       this->dwID;
	pCmd.type               =       Cmd::COUNTRY_FORMAL_ANTI_DARE;

	CDareM::getMe().createDare_sceneSession(&pCmd, dare_list);

	CCountry* pCountry = CCountryM::getMe().find(dwAttCountry);
	if (pCountry)
	{
		SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, this->dwID, "%s 反攻 我国 的国战现在开始,注意防守", 
				pCountry->name);

		SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, "%s 反攻 %s 的国战现在开始。", 
				pCountry->name,
				this->name);
	}

	SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, dwAttCountry,
			"我国 反攻 %s 的国战现在开始", this->name);
}

void CCountry::addTaxMoney(QWORD qwTaxMoney)
{
	this->qwGold+=qwTaxMoney;
	//this->writeDatabase();
}

void CCountry::changeStar(int star)
{
	rwlock.wrlock();
	dwStar = (int)dwStar + star;

	if ((int)dwStar<0)
	{
		dwStar = 0;
	}
	
	rwlock.unlock();


	BYTE buf[zSocket::MAX_DATASIZE];        
	Cmd::stCountryStar *tempPoint;

	Cmd::stUpdateCountryStarCmd *retCmd=(Cmd::stUpdateCountryStarCmd *)buf;
	constructInPlace(retCmd);
	tempPoint = (Cmd::stCountryStar *)retCmd->data;
	retCmd->dwSize = 1;
	tempPoint->dwCountry = this->dwID;
	tempPoint->dwStar = this->dwStar;

	SessionTaskManager::getInstance().sendCmdToWorld(retCmd, 
			(retCmd->dwSize*sizeof(Cmd::stCountryStar)+sizeof(Cmd::stUpdateCountryStarCmd)));
}

DWORD CCountry::getStar()
{
	return dwStar;
}

int CCountry::changeMaterialByPer(int type, float per)
{
	int value = 0;

	switch (type)
	{
		case 0:
			{
				value = (int)(this->qwMaterial * per/100);
			}
			break;
		/*	
		case 1:
			{
				value = (int)(this->qwSilk * per/100);
			}
			break;
		case 2:
			{
				value = (int)(this->qwOre * per/100);
			}
			break;
		case 3:
			{
				value = (int)(this->qwBowlder * per/100);
			}
			break;
		case 4:
			{
				value = (int)(this->qwWood * per/100);
			}
			break;
		case 5:
			{
				value = (int)(this->qwCoat * per/100);
			}
			break;
		case 6:
			{
				value = (int)(this->qwHerbal * per/100);
			}
			break;
		*/
		case 1:
			value = (int)(this->qwStock * per/100);
			break;
		case 2:
			{
				value = (int)(this->qwGold * per/100);
			}
			break;
		default:
			{
			}
	}
	
	if (value!=0)
	{
		this->changeMaterial(type, value);
	}

	return value;
}

QWORD CCountry::getMaterial(int type)
{
	QWORD value;
	
	switch (type)
	{
		case 0:
			{
				value = this->qwMaterial;
			}
			break;
		case 1:
			{
				value = this->qwStock;
			}
			break;
		case 2:
			{
				value = this->qwGold;
			}
			break;
		default:
			{
			value = 0;
			}
	}
	
	return value;
}

void CCountry::changeMaterial(int type, int value)
{
	switch (type)
	{
		case 0:
			{
				if (value<0)
				{
					if ((int)this->qwMaterial>abs(value))
					{
						this->qwMaterial+=value;
					}
					else
					{
						this->qwMaterial = 0;
					}
				}
				else
				{
					this->qwMaterial += value;
				}
			}
			break;
		case 1:
			{
				if (value<0)
				{
					if ((int)this->qwStock>abs(value))
					{
						this->qwStock+=value;
					}
					else
					{
						this->qwStock = 0;
					}
				}
				else
				{
					this->qwStock += value;
				}
			}
			break;
		case 2:
			{
				if (value<0)
				{
					if ((int)this->qwGold>abs(value))
					{
						this->qwGold+=value;
					}
					else
					{
						this->qwGold = 0;
					}
				}
				else
				{
					this->qwGold += value;
				}
			}
			break;
		default:
			break;
	}

	this->writeDatabase();
}

void CCountry::swapMaterialByPer(CCountry* pToCountry, float per)
{
	for (int i=0; i<=2; i++)
	{
		int value = this->changeMaterialByPer(i, -per);
		pToCountry->changeMaterial(i, -value);
	}
}

void CCountry::beginTechVote()
{
	std::vector<CTech*> vote_tech;

	rwlock.wrlock();
	for (int i=1; i<TECH_MAX_NUM; i++)
	{
		CTech* pTech = this->getTech(i);
		if (pTech && pTech->state()!=CTech::ACTIVE_TECH)
		{
			pTech->state(CTech::INIT_TECH);
			vote_tech.push_back(pTech);
		}
	}
	rwlock.unlock();

	if (!CVoteM::getMe().createNewVote(this->dwID, Cmd::TECH_VOTE, vote_tech))
	{
		Zebra::logger->error("[国家]:新建科技投票失败");
	}
}

void CCountry::addGeneralExp(DWORD num)
{
	gen_exp += num;
	while (gen_exp>=gen_maxexp && gen_level<10)
	{
		gen_level++;
		gen_exp -= gen_maxexp;
		gen_maxexp = (gen_level+1)*500;

		refreshGeneral();
		SessionChannel::sendCountryInfo(dwID, Cmd::INFO_TYPE_EXP, "我国大将军王达到 %u 级", gen_level);
		Zebra::logger->trace("大将军升级 country=%u level=%u", dwID, gen_level);
	}
	if (gen_exp>gen_maxexp)
		gen_exp = gen_maxexp;
}

void CCountry::generalLevelDown()
{
	if (gen_level<=1) return;

	gen_level--;
	gen_maxexp = (gen_level+1)*500;
	gen_exp = 0;

	//CCountryM::getMe().refreshGeneral(dwID);
	SessionChannel::sendCountryInfo(dwID, Cmd::INFO_TYPE_EXP, "国战失败，大将军王降为 %u 级", gen_level);
	Zebra::logger->trace("大将军降级 country=%u level=%u", dwID, gen_level);
}

void CCountry::refreshGeneral()
{
	SceneSession * scene = SceneSessionManager::getInstance()->getSceneByID((dwID<<16)+KING_CITY_ID);
	if (scene)
	{
		Cmd::Session::t_refreshGen_SceneSession send;
		send.dwCountryID = dwID;
		send.level = gen_level;
		send.exp = gen_exp;
		send.maxExp = gen_maxexp;

		scene->sendCmd(&send, sizeof(send));

		gen_refreshTime = 0;
#ifdef _XWL_DEBUG
		Zebra::logger->trace("刷新大将军 map=%s level=%u", scene->name, gen_level);
#endif
	}
	else
		Zebra::logger->error("刷新大将军时未找到地图 mapID=%u", (dwID<<16)+KING_CITY_ID);
}

//soke 回复场景太庙资源数目
void CCountry::replyLoongPearlNum( DWORD serverID )
{
    if( 0 == serverID ) return;
    using namespace Cmd::Session;
    ReplyCountryPearlNum_SceneSession send;
    
    send.countryID = this->dwID;
    send.num = this->pearlNum;
    
    SessionTaskManager::getInstance().broadcastByID(serverID , &send , sizeof(send));
}

void CCountry::syncLoongPearlNum( DWORD num )
{
   if ( num>=0 || num < 6000)
   {
       pearlNum = num;
    //   if ( 0 == pearlNum%5 )
       {
           this->writeDatabase();
       }
   }
}

//------------------------------------------------------------------------------------------------------------
CTech::CTech()
{
	dwUID = 0;
	bzero(szName, sizeof(szName));
	bzero(szResearchName, sizeof(szResearchName));
	dwCountryID = 0;
	dwType = 0;
	dwProgress = 0;
	dwStatus = 0;
	dwResearchID = 0;
	dwLastUpTime = 0;
}

CTech::~CTech()
{
}

void CTech::init(Record* rec)
{
	if (rec)
	{
		this->dwUID = rec->get("uid");
		this->dwCountryID = rec->get("countryid");
		this->dwType = rec->get("type");
		this->dwProgress = rec->get("progress");
		this->dwResearchID  = rec->get("researchid");
		this->dwStatus = rec->get("status");
		this->dwLevel = rec->get("level");
		this->dwLastUpTime = rec->get("lastuptime");
		
		strncpy(this->szName, (const char*)rec->get("name"), MAX_NAMESIZE);
		strncpy(this->szResearchName, (const char*)rec->get("researchname"), MAX_NAMESIZE);
	}
}

void CTech::writeDatabase()
{
	//FunctionTimes times(45,__FUNCTION__);
	Record rec,where;
	std::ostringstream oss;
	
	oss << "uid='" << this->dwUID << "'";
	where.put("uid", oss.str());

	rec.put("progress", this->dwProgress);
	rec.put("researchid", this->dwResearchID);
	rec.put("researchname", this->szResearchName);
	rec.put("status", this->dwStatus);
	rec.put("level", this->dwLevel);
	rec.put("lastuptime", this->dwLastUpTime);

	FieldSet* tech = SessionService::metaData->getFields("TECH");

	if (tech)
	{       
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return;
		}

		if ((connHandleID)-1 != handle)
		{
			SessionService::dbConnPool->exeUpdate(handle, tech, &rec, &where);
		}

		SessionService::dbConnPool->putHandle(handle);
	}
	else
	{
		Zebra::logger->error("国家科技数据保存失败，TECH表不存在");
		return;
	}
}

void CTech::upLevel(UserSession* pUser)
{
	zTime cur_time;
	
	rwlock.rdlock();
	if (this->dwResearchID != pUser->id)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是该项科技的官员，不能对其升级");
	}

#ifndef _ZJW_DEBUG	
	if (abs(cur_time.sec()-this->dwLastUpTime)<60*60)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "科技只能一小时升级一次");
	}
#endif	


	if (this->dwStatus != CTech::ACTIVE_TECH)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "科技还未通过投票，进入可研究阶段");
	}

	rwlock.unlock();

	CCountry* pCountry = CCountryM::getMe().find(pUser->country);
/**** 调低国家科技升级所需资金物资和原料前的情况
	int up_need_base[] = {160, 160, 640*100};
//	int up_need_base[] = {2000, 2000, 5000};
	
	int up_need_material[8][3] = {{0,0,0}};

	for (int i=1; i<8; i++)
	{
		for (int j=0; j<3; j++)
		{
			up_need_material[i][j] = up_need_base[j] * (int)pow(2, i-1);
		}
	}
*/
 	//调低国家科技升级所需资金物资和原料
	int up_need_base[] = {2000, 2000, 5000};
	int up_need_material[8][3] = {{0,0,0}};


	for (int i=1; i<8; i++)
	{
		for (int j=0; j<3; j++)
		{
			up_need_material[i][j] = up_need_base[j] * i ;
		}
	}

	bool success = false;
	for (int i=0; i<3; i++)
	{
		QWORD value = pCountry->getMaterial(i);
		
		if (value>=(QWORD)up_need_material[this->dwLevel][i])
		{
			int cur_need_material = (int)up_need_material[this->dwLevel][i];
			pCountry->changeMaterial(i, -cur_need_material);
			success = true;
		}
		else
		{
			success = false;
			break;
		}
	}
			
	if (success)
	{
		if (this->dwProgress+2<100)
		{
			rwlock.wrlock();
			this->dwProgress = this->dwProgress + 2;
			this->dwLastUpTime = cur_time.sec();
			rwlock.unlock();
			pUser->sendSysChat(Cmd::INFO_TYPE_EXP, "%s 本次研究成功", this->szName);
		}
		else
		{
			rwlock.wrlock();
			this->dwLastUpTime = 0;
			this->dwLevel++;
			this->dwStatus=CTech::INIT_TECH;
			this->dwResearchID = 0;
			this->dwProgress = 0;
			bzero(this->szResearchName, MAX_NAMESIZE);
			rwlock.unlock();

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, 
					pUser->country, "%s 第%d级研制成功", this->szName, this->dwLevel-1);
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "国家物资不足，不能升级科技");
	}
	
	this->writeDatabase();
}

void CTech::setSearcher(UserSession* pUser)
{
	if (this->dwStatus != CTech::WAIT_TECH)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "科技还未通过投票,不能设置研究员");
		return;
	}

	rwlock.wrlock();
	this->dwResearchID = pUser->id;
	strncpy(this->szResearchName, pUser->name, MAX_NAMESIZE);
	this->dwStatus = CTech::ACTIVE_TECH;
	rwlock.unlock();
	pUser->sendSysChat(Cmd::INFO_TYPE_EXP, "恭喜你成为 %s 研究员", this->szName);

	this->writeDatabase();

}

void CTech::clearSearcher(UserSession* pUser)
{
	if (this->dwResearchID<=0)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "科技不在研究期间或没有研究员,不能取消研究员");
		return;
	}

	rwlock.wrlock();
	this->dwResearchID = 0;
	bzero(this->szResearchName, MAX_NAMESIZE);
	this->dwStatus = CTech::WAIT_TECH;
	rwlock.unlock();

	this->writeDatabase();
}

