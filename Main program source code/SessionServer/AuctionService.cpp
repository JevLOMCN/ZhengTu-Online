#include "AuctionService.h"
#include "MailService.h"
#include "zDBConnPool.h"
#include "SessionServer.h"
#include "Session.h"
#include "SessionManager.h"
#include <sstream>
#include <stdarg.h>

const dbCol auction_define[] = {
	{ "OWNERID",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "OWNER",		zDBConnPool::DB_STR,    sizeof(char[MAX_NAMESIZE+1]) },
	{ "STATE",		zDBConnPool::DB_BYTE,   sizeof(BYTE) },
	{ "CHARGE",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "ITEMID",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "NAME",		zDBConnPool::DB_STR,    sizeof(char[MAX_NAMESIZE+1]) },
	{ "TYPE",		zDBConnPool::DB_BYTE,   sizeof(BYTE) },
	{ "QUALITY",		zDBConnPool::DB_BYTE,   sizeof(BYTE) },
	{ "NEEDLEVEL",		zDBConnPool::DB_WORD,  sizeof(WORD) },
	{ "MINMONEY",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "MAXMONEY",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "MINGOLD",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "MAXGOLD",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "STARTTIME",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "ENDTIME",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "BIDTYPE",		zDBConnPool::DB_BYTE,   sizeof(BYTE) },
	{ "ITEM",		zDBConnPool::DB_BIN,   sizeof(Cmd::Session::SessionObject)},
	{ NULL, 0, 0}           
};

const dbCol auction_bid_define[] = {
	{ "ID",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "OWNERID",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "OWNER",		zDBConnPool::DB_STR,    sizeof(char[MAX_NAMESIZE+1]) },
	{ "STATE",		zDBConnPool::DB_BYTE,   sizeof(BYTE) },
	{ "CHARGE",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "MINMONEY",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "MAXMONEY",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "MINGOLD",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "MAXGOLD",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "BIDDERID",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "BIDDER2ID",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "BIDDER",		zDBConnPool::DB_STR,    sizeof(char[MAX_NAMESIZE+1]) },
	{ "BIDDER2",		zDBConnPool::DB_STR,    sizeof(char[MAX_NAMESIZE+1]) },
	{ "ENDTIME",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "BIDTYPE",		zDBConnPool::DB_BYTE,   sizeof(BYTE) },
	{ "ITEMID",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "ITEM",		zDBConnPool::DB_BIN,   sizeof(Cmd::Session::SessionObject)},
	{ NULL, 0, 0}           
};

const dbCol auction_state_define[] = {
	{ "STATE",		zDBConnPool::DB_BYTE,   sizeof(BYTE) },
	{ NULL, 0, 0}           
};

const dbCol auction_bidder_define[] = {
	{ "BIDDERID",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "BIDDER",		zDBConnPool::DB_STR,    sizeof(char[MAX_NAMESIZE+1]) },
	{ NULL, 0, 0}           
};

AuctionService *AuctionService::as = 0;

AuctionService::AuctionService(){}
AuctionService::~AuctionService(){}

AuctionService& AuctionService::getMe()
{
	if (!as)
		as = new AuctionService();
	return *as;
}

void AuctionService::delMe()
{
	SAFE_DELETE(as);
}

/* \brief 处理拍卖消息
 *
 * \param cmd 消息
 * \param cmdLen 消息长度
 * 
 */
bool AuctionService::doAuctionCmd(const Cmd::Session::t_AuctionCmd *cmd, const DWORD cmdLen)
{
	using namespace Cmd;
	using namespace Cmd::Session;

	switch (cmd->auctionPara)
	{
		case PARA_AUCTION_SALE:
			{
				//FunctionTimes times(44,"t_saleAuction_SceneSession");
				t_saleAuction_SceneSession * rev = (t_saleAuction_SceneSession *)cmd;

				UserSession * pUser = UserSessionManager::getInstance()->getUserSessionByName(rev->info.owner);
				/*
				if (!pUser)
				{
					Zebra::logger->error("[拍卖]doAuctionCmd(PARA_AUCTION_SALE): 添加拍卖记录时未找到拍卖者 %s", rev->info.owner);
					Zebra::logger->error("[拍卖]添加拍卖记录时未找到拍卖者 %s 物品: %s", rev->info.owner, rev->item.object.strName);
					return false;
				}
				*/

				connHandleID handle = SessionService::dbConnPool->getHandle();
				if ((connHandleID)-1 == handle)
				{
					error("[拍卖]doAuctionCmd(PARA_AUCTION_SALE): 得到数据库句柄失败");
					Zebra::logger->error("[拍卖]添加拍卖记录错误 %s 物品: %s 价格：%u-%u", rev->info.owner, rev->item.object.strName, rev->info.minMoney, rev->info.maxMoney);
					if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "拍卖物品失败，请与GM联系");
					return true;
				}
				unsigned int retcode = SessionService::dbConnPool->exeInsert(handle, "`AUCTION`", auction_define, (const unsigned char *)&(rev->info));
				SessionService::dbConnPool->putHandle(handle);
				if ((unsigned int)-1 == retcode)
				{
					error("[拍卖]doAuctionCmd(PARA_AUCTION_SALE): 插入拍卖记录数据库出错");
					Zebra::logger->error("[拍卖]添加拍卖记录错误 %s 物品: %s 价格：%u-%u", rev->info.owner, rev->item.object.strName, rev->info.minMoney, rev->info.maxMoney);
					if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "拍卖失败，请与GM联系");
				}       
				else    
				{       
					if (pUser)
					{
						stAddListAuction al;
						al.list = 3;//拍卖列表
						al.auctionID = retcode;//返回ID
						al.minMoney = rev->info.minMoney;
						al.maxMoney = rev->info.maxMoney;
						al.minGold = rev->info.minGold;
						al.maxGold = rev->info.maxGold;
						al.endTime = rev->info.endTime - rev->info.startTime;
						al.bidType = rev->info.bidType;
						bcopy(&rev->item.object, &al.item, sizeof(t_Object));
						strncpy(al.owner, rev->info.owner, MAX_NAMESIZE);
						al.mine = true;

						pUser->sendCmdToMe(&al, sizeof(al));
						pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你拍卖一件物品");
					}

					Zebra::logger->trace("[拍卖]:%s 拍卖物品 %s auctionID=%u", rev->info.owner, rev->item.object.strName, retcode);
				}

				return true;
			}
			break;
		case PARA_AUCTION_CHECK_BID:
			{
				//FunctionTimes times(43,"t_checkBidAuction_SceneSession");
				t_checkBidAuction_SceneSession * rev = (t_checkBidAuction_SceneSession *)cmd;
				UserSession * pUser = UserSessionManager::getInstance()->getUserByTempID(rev->userID);
				if (!pUser)
				{
					Zebra::logger->error("[拍卖]doAuctionCmd(PARA_AUCTION_CHECK_BID): 竞标时未找到竞标者");
					return false;
				}

				connHandleID handle = SessionService::dbConnPool->getHandle();
				if ((connHandleID)-1 == handle)
				{
					error("[拍卖]doAuctionCmd(PARA_AUCTION_CHECK_BID): 得到数据库句柄失败");
					return true;
				}

				char where[128];
				bzero(where, sizeof(where));
				zRTime ct;
				//snprintf(where, sizeof(where)-1, "ID=%u AND STATE=%u AND ENDTIME>%lu", rev->auctionID, AUCTION_STATE_NEW, ct.sec());
				snprintf(where, sizeof(where)-1, "ID=%u AND STATE=%u", rev->auctionID, AUCTION_STATE_NEW);

				auctionBidInfo bid;
				unsigned int retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`AUCTION`", auction_bid_define, where, NULL, 1, (BYTE *)&bid);
				SessionService::dbConnPool->putHandle(handle);
				if ((DWORD)-1 == retcode)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "竞标失败");
					error("[拍卖]%s 竞标时查询错误 auctionID=%u retCode=%d", pUser->name, rev->auctionID, retcode);
					return false;
				}
				if (1 != retcode)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "竞标失败，该物品已经成交");
					//Zebra::logger->alarm("[拍卖]%s 竞标没有找到记录 auctionID=%u retCode=%d", pUser->name, rev->auctionID, retcode);
					return true;
				}

				if (0==strncmp(bid.bidder, pUser->name, MAX_NAMESIZE))
				{
					if ((0==bid.bidType&&((bid.maxMoney&&rev->money<bid.maxMoney)||0==bid.maxMoney))
							|| (1==bid.bidType&&(bid.maxGold&&rev->gold<bid.maxGold||0==bid.maxGold)))
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "这件物品的最高价已经是你的。如果这件物品设定了一口价，你可以一口价把它买下来");
						return true;
					}
				}

				if ((!strncmp(bid.owner, pUser->name, MAX_NAMESIZE))
						|| (0==bid.bidType && bid.minMoney>rev->money)
						|| (0==bid.bidType && (bid.minMoney==rev->money && (bid.maxMoney!=bid.minMoney)))
						|| (1==bid.bidType && bid.minGold>rev->gold)
						|| (1==bid.bidType && (bid.minGold==rev->gold && (bid.maxGold!=bid.minGold))))
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能执行该操作，非法竞标");
					return false;
				}

				rev->bidType = bid.bidType;
				pUser->scene->sendCmd(rev, sizeof(t_checkBidAuction_SceneSession));
			}
			break;
		case PARA_AUCTION_BID:
			{
				//FunctionTimes times(42,"t_bidAuction_SceneSession");
				t_bidAuction_SceneSession * rev = (t_bidAuction_SceneSession *)cmd;

				UserSession * pUser = UserSessionManager::getInstance()->getUserByTempID(rev->userID);
				if (!pUser)
				{
					Zebra::logger->error("[拍卖]doAuctionCmd(PARA_AUCTION_CHECK_BID): 竞标时未找到竞标者, 丢失%u文",rev->money);
					return false;
				}

				connHandleID handle = SessionService::dbConnPool->getHandle();
				if ((connHandleID)-1 == handle)
				{
					error("[拍卖]doAuctionCmd(PARA_AUCTION_CHECK_BID): 得到数据库句柄失败", pUser->name);
					Zebra::logger->error("[拍卖]竞标失败 %s 丢失 money=%u gold=%u", rev->money, rev->gold);
					return false;
				}

				char where[128];
				bzero(where, sizeof(where));
				zRTime ct;
				snprintf(where, sizeof(where) - 1, "ID=%u AND STATE=%u", rev->auctionID, AUCTION_STATE_NEW);

				auctionBidInfo bid;
				unsigned int retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`AUCTION`", auction_bid_define, where, NULL, 1, (BYTE *)&bid);

				if ((DWORD)-1 == retcode)
				{
					SessionService::dbConnPool->putHandle(handle);
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "竞标失败");
					error("[拍卖]%s 竞标时查询错误 auctionID=%u retCode=%d 丢失 money=%u gold=%u", pUser->name, rev->auctionID, retcode, rev->money, rev->gold);
					return false;
				}
				if (1 != retcode)
				{
					if (MailService::getMe().sendMoneyMail("征途拍卖行", 0, pUser->name, pUser->id, rev->money, "竞拍失败返还的银子", (DWORD)handle, MAIL_TYPE_AUCTION, bid.itemID))
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "竞标失败，该物品已经成交，费用已退回到邮箱");
					else
						Zebra::logger->error("[拍卖]%s 竞标失败返还银子失败 auctionID=%u retCode=%d 丢失 money=%u gold=%u", pUser->name, rev->auctionID, retcode, rev->money, rev->gold);
					SessionService::dbConnPool->putHandle(handle);
					return true;
				}

				//退钱给上一个竞标者
				if (0!=strcmp(bid.bidder, "") || bid.bidderID!=0)
				{
					char buf[128];
					bzero(buf, sizeof(buf));
					snprintf(buf, sizeof(buf), "你竞拍 %s 的价格被压过，返还的金钱", bid.item.object.strName);
					MailService::getMe().sendMoneyMail("征途拍卖行", 0, bid.bidder, bid.bidderID, bid.minMoney, buf, (DWORD)handle, MAIL_TYPE_AUCTION, bid.itemID);
				}

				char bidder3[MAX_NAMESIZE];
				strncpy(bidder3, bid.bidder2, MAX_NAMESIZE);

				//更新价格和竞价者
				bid.minMoney = 0==bid.bidType?rev->money:0;
				bid.minGold = 1==bid.bidType?rev->gold:0;
				strncpy(bid.bidder2, bid.bidder, MAX_NAMESIZE);
				bid.bidder2ID = bid.bidderID;
				strncpy(bid.bidder, pUser->name, MAX_NAMESIZE);
				bid.bidderID = pUser->id;
				retcode = SessionService::dbConnPool->exeUpdate(handle, "`AUCTION`", auction_bid_define, (BYTE *)&bid, where);
				if (1 != retcode)
				{
					if (MailService::getMe().sendMoneyMail("征途拍卖行", 0, pUser->name, pUser->id, rev->money, "竞拍失败返还的金钱", (DWORD)handle, MAIL_TYPE_AUCTION, bid.itemID))
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "竞标失败，该物品已经成交，费用已退回到邮箱");
					else
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "竞标失败");
						Zebra::logger->error("[拍卖]%s 竞标时没找到记录，退回邮件失败 auctionID=%u retCode=%d, 丢失 money=%u gold=%u", pUser->name, rev->auctionID, retcode, rev->money, rev->gold);
						SessionService::dbConnPool->putHandle(handle);
						return false;
					}
					SessionService::dbConnPool->putHandle(handle);
					return true;
				}

				//出价大于一口价
				if ((0==bid.bidType && bid.maxMoney && rev->money>=bid.maxMoney)
						|| (1==bid.bidType && bid.maxGold && rev->gold>=bid.maxGold))
				{
					if (!sendAuctionItem((DWORD)handle, rev->auctionID, AUCTION_STATE_DEAL, false))
					{
						Zebra::logger->error("[拍卖]%s 竞价成功发送物品失败 auctionID=%u", pUser->name, rev->auctionID);
						SessionService::dbConnPool->putHandle(handle);
						return false;
					}
				}
				else
				{
					//通知竞价者刷新记录
					stAddListAuction al;
					al.auctionID = bid.auctionID;
					al.minMoney = bid.minMoney;
					al.maxMoney = bid.maxMoney;
					al.minGold = bid.minGold;
					al.maxGold = bid.maxGold;
					zRTime ct;
					al.endTime = bid.endTime>ct.sec()?bid.endTime-ct.sec():0;
					al.bidType = bid.bidType;
					bcopy(&bid.item.object, &al.item, sizeof(t_Object));
					strncpy(al.owner, bid.owner, MAX_NAMESIZE);

					//al.list = 1;
					al.mine = true;
					//pUser->sendCmdToMe(&al, sizeof(al));
					al.list = 2;
					pUser->sendCmdToMe(&al, sizeof(al));

					al.list = 2;//竞拍列表
					al.mine = false;
					UserSession * u = 0;
					if (strcmp(bidder3, pUser->name))
					{
						u = UserSessionManager::getInstance()->getUserSessionByName(bidder3);
						if (u)
							u->sendCmdToMe(&al, sizeof(al));
					}
					if (0!=strcmp(bid.bidder2, ""))
					{
						u = UserSessionManager::getInstance()->getUserSessionByName(bid.bidder2);
						if (u)
							u->sendCmdToMe(&al, sizeof(al));
					}

					al.list = 3;//拍卖者的拍卖列表
					u = UserSessionManager::getInstance()->getUserSessionByName(bid.owner);
					if (u)
						u->sendCmdToMe(&al, sizeof(al));
				}

				SessionService::dbConnPool->putHandle(handle);

				return true;
			}
			break;
		case PARA_AUCTION_QUERY:
			{
				//FunctionTimes times(41,"t_queryAuction_SceneSession");
				t_queryAuction_SceneSession * rev = (t_queryAuction_SceneSession *)cmd;

				UserSession * pUser = UserSessionManager::getInstance()->getUserByTempID(rev->userID);
				if (!pUser)	return true;

				if (0==strcmp(rev->name,"") && 0==rev->type && 0==rev->quality)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "数量太多，请增加限制条件以缩小搜索范围");
					return true;
				}

				connHandleID handle = SessionService::dbConnPool->getHandle();
				if ((connHandleID)-1 == handle)
				{
					error("[拍卖]doAuctionCmd(PARA_AUCTION_QUERY): 得到数据库句柄失败");
					return false;
				}

				std::string where;
				char tmp[128];
				bzero(tmp, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "STATE = %d", AUCTION_STATE_NEW);
				where = tmp;
				bzero(tmp, sizeof(tmp));
				if (rev->type)
				{
					snprintf(tmp, sizeof(tmp), " AND TYPE=%u ", rev->type);
					where += tmp;
					bzero(tmp, sizeof(tmp));
				}
				if (rev->quality)
				{
					snprintf(tmp, sizeof(tmp), " AND QUALITY=%u ", rev->quality-1);
					where += tmp;
					bzero(tmp, sizeof(tmp));
				}
				if (rev->level!=(WORD)-1)
				{
					snprintf(tmp, sizeof(tmp), " AND NEEDLEVEL=%u ", rev->level);
					where += tmp;
					bzero(tmp, sizeof(tmp));
				}
				if (strcmp(rev->name,""))
				{
					std::string escapeName;
					SessionService::dbConnPool->escapeString(handle,rev->name,escapeName);
					snprintf(tmp, sizeof(tmp), " AND NAME LIKE '%%%s%%' ", escapeName.c_str());
					where += tmp;
					bzero(tmp, sizeof(tmp));
				}

				auctionBidInfo bid[10];
				unsigned int retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`AUCTION`", auction_bid_define, where.c_str(), NULL, 10, (BYTE *)&bid, rev->page*10);

				static const dbCol count_define[] = {
					{ "`RET`", zDBConnPool::DB_DWORD, sizeof(DWORD) },
					{ NULL, 0, 0}
				};
				char sql[1024];
				DWORD count;
				snprintf(sql, 1024, "SELECT COUNT(*) AS `RET` FROM `AUCTION` WHERE %s", where.c_str());
				SessionService::dbConnPool->execSelectSql(handle, sql ,strlen(sql), count_define, 1,(BYTE *)&count);
				SessionService::dbConnPool->putHandle(handle);

				if ((DWORD)-1 == retcode)
				{
					Zebra::logger->error("[拍卖]%s 查询拍卖物品时错误 retCode=%d", pUser->name, retcode);
					return false;
				}

				if (retcode)
				{
					stAddListAuction al;
					al.list = 1;
					al.max = count;
					zRTime ct;
					for (unsigned int i=0; i<retcode && i<10; i++)
					{
						al.auctionID = bid[i].auctionID;
						al.minMoney = bid[i].minMoney;
						al.maxMoney = bid[i].maxMoney;
						al.minGold = bid[i].minGold;
						al.maxGold = bid[i].maxGold;
						al.endTime = bid[i].endTime>ct.sec()?bid[i].endTime-ct.sec():0;
						al.bidType = bid[i].bidType;
						bcopy(&bid[i].item.object, &al.item, sizeof(t_Object));
						strncpy(al.owner, bid[i].owner, MAX_NAMESIZE);
						al.mine = (0==strcmp(bid[i].bidder, pUser->name));

						pUser->sendCmdToMe(&al, sizeof(al));
#ifdef _XWL_DEBUG
				//Zebra::logger->debug("[拍卖]查询到 %u 条记录 %s", count, where.c_str());
#endif
					}
				}
			}
			break;
		case PARA_AUCTION_CHECK_CANCEL:
			{
				//FunctionTimes times(40,"t_checkCancelAuction_SceneSession");
				t_checkCancelAuction_SceneSession * rev = (t_checkCancelAuction_SceneSession *)cmd;

#ifdef _XWL_DEBUG
				//zThread::msleep(3000);
#endif

				UserSession * pUser = UserSessionManager::getInstance()->getUserByTempID(rev->userID);
				if (!pUser)
				{
					Zebra::logger->error("[拍卖]doAuctionCmd(PARA_AUCTION_CHECK_CANCEL): 取消拍卖检查时未找到拍卖者");
					return false;
				}

				connHandleID handle = SessionService::dbConnPool->getHandle();
				if ((connHandleID)-1 == handle)
				{
					error("[拍卖]doAuctionCmd(PARA_AUCTION_CHECK_CANCEL): 得到数据库句柄失败");
					return false;
				}

				char where[128];
				bzero(where, sizeof(where));
				zRTime ct;
				std::string escapeName;
				snprintf(where, sizeof(where) - 1, "ID=%u AND STATE=%u AND OWNER='%s'", rev->auctionID, AUCTION_STATE_NEW, SessionService::dbConnPool->escapeString(handle,pUser->name,escapeName).c_str());

				auctionBidInfo bid;
				unsigned int retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`AUCTION`", auction_bid_define, where, NULL, 1, (BYTE *)&bid);
				SessionService::dbConnPool->putHandle(handle);
				if ((DWORD)-1 == retcode)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "取消拍卖失败");
					error("[拍卖]%s 取消拍卖检查时查询错误 auctionID=%u retCode=%d", pUser->name, rev->auctionID, retcode);
					return false;
				}
				if (1 != retcode)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "无法取消拍卖，物品已经成交或过期");
					return true;
				}

				t_checkCancelAuction_SceneSession cca;
				cca.userID = rev->userID;
				cca.auctionID = rev->auctionID;
				cca.charge = bid.charge;

				pUser->scene->sendCmd(&cca, sizeof(cca));
			}
			break;
		case PARA_AUCTION_CANCEL:
			{
				//FunctionTimes times(39,"t_cancelAuction_SceneSession");
				t_cancelAuction_SceneSession * rev = (t_cancelAuction_SceneSession *)cmd;

				UserSession * pUser = UserSessionManager::getInstance()->getUserByTempID(rev->userID);
				if (!pUser)
				{
					Zebra::logger->error("[拍卖]doAuctionCmd(PARA_AUCTION_CANCEL): 取消拍卖时未找到拍卖者");
					return false;
				}

				connHandleID handle = SessionService::dbConnPool->getHandle();
				if ((connHandleID)-1 == handle)
				{
					error("doAuctionCmd(PARA_AUCTION_CANCEL): 得到数据库句柄失败");
					return true;
				}

				char where[128];
				bzero(where, sizeof(where));
				zRTime ct;
				snprintf(where, sizeof(where) - 1, "ID=%u", rev->auctionID);

				auctionBidInfo bid;
				unsigned int retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`AUCTION`", auction_bid_define, where, NULL, 1, (BYTE *)&bid);
				if (1 != retcode
						|| bid.state!=AUCTION_STATE_NEW)
				{
					if (MailService::getMe().sendMoneyMail("征途拍卖行", 0, pUser->name, pUser->id, rev->charge, "取消拍卖失败返还的费用", (DWORD)handle, MAIL_TYPE_AUCTION, bid.itemID))
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "无法取消拍卖，物品已经成交或过期,手续费返还到邮箱");
					else
						Zebra::logger->error("%s 取消拍卖时没找到记录，退回银子失败 auctionID=%u retCode=%d charge=%u", pUser->name, rev->auctionID, retcode, rev->charge);
					SessionService::dbConnPool->putHandle(handle);
					return true;
				}

				if (!sendAuctionItem((DWORD)handle, rev->auctionID, AUCTION_STATE_CANCEL, true))
				{
					Zebra::logger->error("%s 取消拍卖发送物品失败 auctionID=%u charge=%u itemID=%u", pUser->name, rev->auctionID, rev->charge, bid.itemID);
					SessionService::dbConnPool->putHandle(handle);
					return false;
				}
				SessionService::dbConnPool->putHandle(handle);

				std::ostringstream s;
				s<<"你付出";
				if (rev->charge/10000) s<<rev->charge/10000<<"锭";
				if ((rev->charge%10000)/100) s<<(rev->charge%10000)/100<<"两";
				if (rev->charge%100) s<<rev->charge%100<<"文";
				pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你付出%s，取消了 %s 的拍卖,请找传递者收取物品", s.str().c_str(), bid.item.object.strName);
				Zebra::logger->trace("[拍卖]%s 取消拍卖 %s auctionID=%u itemID=%u", pUser->name, bid.item.object.strName, rev->auctionID, bid.itemID);
			}
			break;
		case PARA_AUCTION_GET_LIST:
			{
				//FunctionTimes times(38,"t_getListAuction_SceneSession");
				t_getListAuction_SceneSession * rev = (t_getListAuction_SceneSession *)cmd;

				UserSession * pUser = UserSessionManager::getInstance()->getUserByTempID(rev->userID);
				if (!pUser)
				{
					Zebra::logger->error("doAuctionCmd(PARA_AUCTION_GET_LIST): 查询时未找到玩家");
					return false;
				}

				connHandleID handle = SessionService::dbConnPool->getHandle();
				if ((connHandleID)-1 == handle)
				{
					error("doAuctionCmd(PARA_AUCTION_GET_LIST): 得到数据库句柄失败");
					return false;
				}

				zRTime ct;
				std::string escapeName;
				SessionService::dbConnPool->escapeString(handle,pUser->name,escapeName);
				char where[128];
				bzero(where, sizeof(where));
				if (rev->list == 3)//拍卖列表
					snprintf(where, sizeof(where)-1, "STATE=%u AND OWNER='%s'", AUCTION_STATE_NEW, escapeName.c_str());
				else
					snprintf(where, sizeof(where)-1, "STATE=%u AND (BIDDER='%s' OR BIDDER2='%s')", AUCTION_STATE_NEW, escapeName.c_str(), escapeName.c_str());

				auctionBidInfo *bid = 0, *tempPoint = 0;
				unsigned int retcode = SessionService::dbConnPool->exeSelect(handle, "`AUCTION`", auction_bid_define, where, NULL, (BYTE **)&bid);
				SessionService::dbConnPool->putHandle(handle);

				if ((DWORD)-1 == retcode)
				{
					error("%s 查询拍卖物品时错误 retCode=%d", pUser->name, retcode);
					return false;
				}

				if (bid)
				{
					stAddListAuction al;
					al.list = rev->list;
					tempPoint = &bid[0];
					for (unsigned int i=0; i<retcode; i++)
					{
						al.auctionID = tempPoint->auctionID;
						al.minMoney = tempPoint->minMoney;
						al.maxMoney = tempPoint->maxMoney;
						al.minGold = tempPoint->minGold;
						al.maxGold = tempPoint->maxGold;
						al.endTime = tempPoint->endTime>ct.sec()?tempPoint->endTime-ct.sec():0;
						al.bidType = tempPoint->bidType;
						bcopy(&tempPoint->item.object, &al.item, sizeof(t_Object));
						strncpy(al.owner, tempPoint->owner, MAX_NAMESIZE);
						if (0==strcmp(tempPoint->bidder, pUser->name))
							al.mine = true;
						else
							al.mine = false;

						pUser->sendCmdToMe(&al, sizeof(al));

						tempPoint++;
					}
					SAFE_DELETE_VEC(bid);
				}
			}
			break;
			/*
		case PARA_AUCTION_BID_LIST:
			{
				t_bidListAuction_SceneSession * rev = (t_bidListAuction_SceneSession *)cmd;

				UserSession * pUser = UserSessionManager::getInstance()->getUserByTempID(rev->userID);
				if (!pUser) return false;

				bcopy(&rev->bidList[0], &pUser->bidList[0], sizeof(pUser->bidList));
			}
			break;
			*/
		default:
			break;
	}
	return false;
}

/* \brief 定时检查数据库
 * 
 * 退回、删除过期的邮件
 * 
 */
void AuctionService::checkDB()
{
	//FunctionTimes times(37,__FUNCTION__);
	using namespace Cmd::Session;

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{               
		error("[拍卖]checkDB: 得到数据库句柄失败");
		return;
	}

	zRTime ct;
	char where[128];

	//结束拍卖时间结束的
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where), "STATE=%u AND ENDTIME<%lu", AUCTION_STATE_NEW, ct.sec());

	auctionBidInfo * bidList, * tempPoint;
	DWORD retcode = SessionService::dbConnPool->exeSelect(handle, "`AUCTION`", auction_bid_define, where, NULL, (BYTE **)&bidList);
	if ((DWORD)-1 == retcode)
	{
		error("[拍卖]checkDB: 常规检查失败 retCode=%d", retcode);
		SessionService::dbConnPool->putHandle(handle);
		return;
	}

	if (bidList)
	{
		tempPoint = &bidList[0];
		for (unsigned int i=0; i< retcode; i++)
		{
			bool toOnwer = true;
			if (0!=strcmp(tempPoint->bidder, ""))
				toOnwer = false;
			if (sendAuctionItem((DWORD)handle, tempPoint->auctionID, AUCTION_STATE_TIMEOVER, toOnwer))
				Zebra::logger->trace("[拍卖]拍卖时间结束，%s物品 %s auctionID=%u itemID=%u", toOnwer?"退回":"发送", tempPoint->item.object.strName, tempPoint->auctionID, tempPoint->itemID);
			else
				Zebra::logger->error("[拍卖]时间结束退回物品失败 auctionID=%u itemID=%u",tempPoint->auctionID, tempPoint->itemID);
			tempPoint++;
		}
		SAFE_DELETE_VEC(bidList);
	}

	//删除过期的
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where), "ENDTIME<%lu", ct.sec()-864000);
	retcode = SessionService::dbConnPool->exeDelete(handle, "`AUCTION`", where);
	if (retcode) Zebra::logger->debug("[拍卖]删除 %u 条10天之前的拍卖记录", retcode);

	SessionService::dbConnPool->putHandle(handle);
}

/* \brief 发送拍卖的物品
 * 可能是买的、卖的、退回的，同时发送钱
 * 
 * \param h 数据库句柄
 * \param auctionID 拍卖记录的ID
 * \param newState 发送之后要设置的状态
 * \param toOwner 是否发送给物品拍卖者
 * 
 */
bool AuctionService::sendAuctionItem(DWORD h, DWORD auctionID, BYTE newState, bool toOwner)
{
	//FunctionTimes times(36,__FUNCTION__);
	using namespace Cmd::Session;

	connHandleID handle = (connHandleID)h;

	if ((connHandleID)-1 == handle)
	{               
		error("[拍卖]sendAuctionItem: 无效的数据库句柄");
		return false;
	}

	char where[128];
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "ID=%u", auctionID);

	auctionBidInfo bid;
	unsigned int retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`AUCTION`", auction_bid_define, where, NULL, 1, (BYTE *)&bid);
	if ((DWORD)-1 == retcode)
	{
		error("[拍卖]sendAuctionItem 数据库查询错误 auctionID=%u retcode=%d", auctionID, retcode);
		return false;
	}
	if (1 != retcode)
		return false;

	t_sendMail_SceneSession sm;
	sm.mail.state = MAIL_STATE_NEW;
	strncpy(sm.mail.fromName, "征途拍卖行", MAX_NAMESIZE);
	sm.mail.type = MAIL_TYPE_AUCTION;
	zRTime ct;
	sm.mail.createTime = ct.sec();
	sm.mail.delTime = sm.mail.createTime + 60*60*24*7;
	sm.mail.accessory = 1;
	sm.mail.itemGot = 0;
	sm.mail.sendMoney = 0;
	sm.mail.recvMoney = 0;
	sm.mail.itemID = bid.itemID;

	char buf[128];
	bzero(buf, sizeof(buf));
	Cmd::stRemoveListAuction rl;
	rl.auctionID = auctionID;
	if (!toOwner)//卖出，给竞标者
	{
		if (strcmp(bid.bidder, ""))
		{
			strncpy(sm.mail.toName, bid.bidder, MAX_NAMESIZE);
			sm.mail.toID = bid.bidderID;
			sm.mail.itemID = bid.itemID;
			snprintf(buf, sizeof(buf), "你拍卖的物品 %s 成功售出，获得的银子。系统收取1%%的成交额作为佣金", bid.item.object.strName);
			MailService::getMe().sendMoneyMail("征途拍卖行", 0, bid.owner, bid.ownerID, bid.minMoney*99/100, buf, (DWORD)handle, MAIL_TYPE_AUCTION, bid.itemID);//给拍卖者钱

			UserSession * u = UserSessionManager::getInstance()->getUserSessionByName(bid.owner);
			if (u)
			{
				u->sendSysChat(Cmd::INFO_TYPE_GAME, "物品 %s 拍卖成功,请注意查收邮件", bid.item.object.strName);
				rl.list = 4;
				u->sendCmdToMe(&rl, sizeof(rl));
			}
			u = UserSessionManager::getInstance()->getUserSessionByName(bid.bidder);
			if (u)
			{
				u->sendSysChat(Cmd::INFO_TYPE_GAME, "物品 %s 竞拍成功,请注意查收邮件", bid.item.object.strName);
				rl.list = 4;
				u->sendCmdToMe(&rl, sizeof(rl));
			}

			snprintf(sm.mail.text, sizeof(sm.mail.text), "竞拍 %s 成功", bid.item.object.strName);
			strcpy(sm.mail.title, "竞拍成功");
			Zebra::logger->trace("[拍卖]%s 赢得物品 %s auctionID=%u itemID=%u", bid.bidder, bid.item.object.strName, auctionID, bid.itemID);
		}
		else
		{
			Zebra::logger->error("[拍卖]sendAuctionItem: 竞标者的名字为空 auctionID=%u, itemID=%u", auctionID, bid.itemID);
			return false;
		}
	}
	else
	{
		strncpy(sm.mail.toName, bid.owner, MAX_NAMESIZE);
		sm.mail.toID = bid.ownerID;
		if (newState==AUCTION_STATE_CANCEL)
		{
			strncpy(sm.mail.title, "取消拍卖", sizeof(sm.mail.title));
			snprintf(sm.mail.text, sizeof(sm.mail.text), "你取消拍卖 %s", bid.item.object.strName);
		}
		else
		{
			strncpy(sm.mail.title, "物品被退回", sizeof(sm.mail.title));
			snprintf(sm.mail.text, sizeof(sm.mail.text), "拍卖行返还了你的 %s", bid.item.object.strName);
		}

		UserSession * u = UserSessionManager::getInstance()->getUserSessionByName(bid.owner);
		if (u)
		{
			u->sendSysChat(Cmd::INFO_TYPE_GAME, "你的物品 %s 结束拍卖,请注意查收邮件", bid.item.object.strName);
			rl.list = 4;
			u->sendCmdToMe(&rl, sizeof(rl));
		}
		if (strcmp(bid.bidder, ""))
		{
			snprintf(buf, sizeof(buf), "你竞拍物品 %s 失败，退回的银子", bid.item.object.strName);
			MailService::getMe().sendMoneyMail("征途拍卖行", 0, bid.bidder, bid.bidderID, bid.minMoney, buf, (DWORD)handle, MAIL_TYPE_AUCTION, bid.itemID);//给竞标者钱
			u = UserSessionManager::getInstance()->getUserSessionByName(bid.bidder);
			if (u)
			{
				rl.list = 4;
				u->sendCmdToMe(&rl, sizeof(rl));
			}
		}
		Zebra::logger->trace("[拍卖]%s 取回物品 %s auctionID=%u itemID=%u", bid.owner, bid.item.object.strName, auctionID, bid.itemID);
	}
	bcopy(&bid.item, &sm.item, sizeof(SessionObject));

	if (MailService::getMe().sendMail((DWORD)handle, sm))
	{
		bid.state = newState;
		retcode = SessionService::dbConnPool->exeUpdate(handle, "`AUCTION`", auction_bid_define, (BYTE *)&bid, where);
		if (retcode!=1)
		{
			error("[拍卖]sendAuctionItem: 更新拍卖状态失败 auctionID=%u itemID=%u", auctionID, bid.itemID);
			return false;
		}
		return true;
	}
	else
	{
		Zebra::logger->error("[拍卖]sendAuctionItem: 拍卖结束发送物品失败 auctionID=%u itemID=%u", auctionID, bid.itemID);
		return false;
	}
}

/* \brief 根据玩家名字删除他所有拍卖记录
 * 包括卖出、竞拍和出价记录全都删除
 * 
 * \param name 角色名字
 * 
 */
void AuctionService::delAuctionRecordByName(char * name)
{
	//FunctionTimes times(35,__FUNCTION__);
	using namespace Cmd::Session;

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		error("[拍卖]delAuctionRecordByName: 得到数据库句柄失败 name=%s", name);
		return;
	}

	std::string escapeName;
	SessionService::dbConnPool->escapeString(handle,name,escapeName);
	char where[128];
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "OWNER='%s' AND STATE=%u", escapeName.c_str(), AUCTION_STATE_NEW);

	BYTE state = AUCTION_STATE_DEL;
	DWORD retcode = SessionService::dbConnPool->exeUpdate(handle, "`AUCTION`", auction_state_define, &state, where);

	char b[sizeof(DWORD) + MAX_NAMESIZE + 1];
	bzero(b, sizeof(b));
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "BIDDER='%s' AND STATE=%u", escapeName.c_str(), AUCTION_STATE_NEW);
	retcode = SessionService::dbConnPool->exeUpdate(handle, "`AUCTION`", auction_bidder_define, (BYTE *)b, where);

	SessionService::dbConnPool->putHandle(handle);

	Zebra::logger->trace("[拍卖]删除角色所有拍卖记录：name=%s", name);
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

bool AuctionService::error(const char * msg, ...)
{
	char buf[MAX_CHATINFO];
	bzero(buf, sizeof(buf));
	getMessage(buf, MAX_CHATINFO, msg);

	Zebra::logger->error("[拍卖]%s", buf);
	SessionService::getInstance().reportGm("[拍卖]", buf);

	return true;
}
