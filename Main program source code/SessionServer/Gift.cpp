#include "Gift.h"
#include "zDBConnPool.h"
#include "SessionServer.h"
//#include "Session.h"
//#include "SessionManager.h"

const dbCol act_define[] = {
	{ "ID",                 zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "NAME",		zDBConnPool::DB_STR,    sizeof(char[MAX_NAMESIZE]) },
	{ "STATE",              zDBConnPool::DB_BYTE,   sizeof(BYTE) },
	{ "TEXT",		zDBConnPool::DB_STR,    sizeof(char[MAX_CHATINFO]) },
	{ NULL, 0, 0}           
};

const dbCol gift_define[] = {
	{ "ACTID",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "CHARID",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "NAME",			zDBConnPool::DB_STR,    sizeof(char[MAX_NAMESIZE]) },
	{ "ITEMGOT",		zDBConnPool::DB_BYTE,   sizeof(BYTE) },
	{ "MAILTEXT",		zDBConnPool::DB_STR,    sizeof(char[MAX_CHATINFO]) },
	{ "MONEY",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "ITEMID",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "ITEMTYPE",		zDBConnPool::DB_BYTE,   sizeof(BYTE) },
	{ "ITEMNUM",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },
	{ "BIND",		zDBConnPool::DB_BYTE,   sizeof(BYTE) },
	{ NULL, 0, 0}           
};

std::vector<actInfo> Gift::actList;
std::multimap<DWORD, Cmd::Session::giftInfo> Gift::giftList;
std::multimap<DWORD, std::string> Gift::winnerList;

Gift::Gift(){}

bool Gift::init()
{
	Zebra::logger->debug("[Gift]加载运营活动 %u 个", loadActList());
	Zebra::logger->debug("[Gift]加载获奖列表 %u 个", loadGiftList());

	return true;
}

DWORD Gift::loadActList()
{
	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{               
		Zebra::logger->error("[Gift]loadActList: 得到数据库句柄失败");
		return 0;
	}

	actList.clear();

	//得到活动列表

	actInfo *info;
	DWORD ret = SessionService::dbConnPool->exeSelect(handle, "`ACT`", act_define, "", "ID DESC", (BYTE **)&info);
	SessionService::dbConnPool->putHandle(handle);

	if (info)
	{
		for (unsigned int i=0; i<ret; i++)
			actList.push_back(info[i]);
	}
	SAFE_DELETE_VEC(info);

	return actList.size();
}

DWORD Gift::loadGiftList()
{
	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{               
		Zebra::logger->error("[Gift]loadGiftList: 得到数据库句柄失败");
		return 0;
	}

	giftList.clear();

	//得到活动列表

	Cmd::Session::giftInfo *info;
	DWORD ret = SessionService::dbConnPool->exeSelect(handle, "`GIFT`", gift_define, "", 0, (BYTE **)&info);
	SessionService::dbConnPool->putHandle(handle);

	if (info)
	{
		for (unsigned int i=0; i<ret; i++)
		{
			giftList.insert(std::make_pair(info[i].charID, info[i]));
			winnerList.insert(std::make_pair(info[i].actID, info[i].name));
		}
	}
	SAFE_DELETE_VEC(info);

	return giftList.size();
}

bool Gift::doGiftCmd(UserSession *pUser, const Cmd::stNullUserCmd *cmd, const unsigned int cmdLen)
{
	using namespace Cmd;
	using namespace Cmd::Session;
	switch (cmd->byParam)
	{
		case GET_LIST_GIFT_PARA:
			{
				BYTE buf[zSocket::MAX_DATASIZE];
				bzero(buf, sizeof(buf));
				stListGift *list=(stListGift *)buf;
				constructInPlace(list);

				list->num = actList.size();
				for (DWORD i=0; i<list->num; i++)
				{
					list->titles[i].id = actList[i].id;
					strncpy(list->titles[i].name, actList[i].name, MAX_NAMESIZE);
					list->titles[i].state = actList[i].state;
				}

				pUser->sendCmdToMe(list, sizeof(stListGift)+sizeof(actTitle)*list->num);
				return true;
			}
			break;
		case GET_DETAIL_GIFT_PARA:
			{
				stGetDetailGift * rev = (stGetDetailGift *)cmd;
				for (DWORD i=0; i<actList.size(); i++)
				{
						if (actList[i].id==rev->id)
						{
								stDetailGift send;
								send.id = rev->id;
								strncpy(send.text, actList[i].text, sizeof(send.text)-1);
								pUser->sendCmdToMe(&send, sizeof(send));
						}
				}
				return true;
			}
			break;
		case GET_WINNER_GIFT_PARA:
			{
					stGetWinnerGift * rev = (stGetWinnerGift *)cmd;

					BYTE buf[zSocket::MAX_DATASIZE];
					bzero(buf, sizeof(buf));
					stWinnerGift *cmd=(stWinnerGift *)buf;
					constructInPlace(cmd);

					char * list = cmd->winners;
					typedef std::multimap<DWORD, std::string>::const_iterator I;
					std::pair<I, I> p = winnerList.equal_range(rev->id);
					DWORD i=0;
					for (I it=p.first; it!=p.second; it++)
					{
							strncpy(list, it->second.c_str(), MAX_NAMESIZE);
							list += MAX_NAMESIZE;

							if (++i>=2000) break;
					}

					cmd->num = i;
					pUser->sendCmdToMe(cmd, sizeof(stWinnerGift)+MAX_NAMESIZE*i);
			}
			break;
		case GET_ITEM_GIFT_PARA:
			{
					stGetItemGift * rev = (stGetItemGift *)cmd;
					typedef std::multimap<DWORD, giftInfo>::iterator I;
					std::pair<I, I> p = giftList.equal_range(pUser->id);
					DWORD i=0;
					for (I it=p.first; it!=p.second; it++)
					{
							if (it->second.actID!=rev->id || it->second.itemGot) continue;

							t_sendGift_SceneSession send;
							bcopy(&(it->second), &send.info, sizeof(giftInfo));
							pUser->scene->sendCmd(&send, sizeof(send));

							it->second.itemGot = 1;
							i++;
					}

					if (i)
					{
							connHandleID handle = SessionService::dbConnPool->getHandle();
							if ((connHandleID)-1 == handle)
							{               
									Zebra::logger->error("[Gift]doGiftCmd(GET_ITEM_GIFT_PARA): 得到数据库句柄失败");
									return false;
							}

							const dbCol gift_got_define[] = {
									{ "ITEMGOT",              zDBConnPool::DB_BYTE,   sizeof(BYTE) },
									{ NULL, 0, 0}           
							};

							char where[MAX_CHATINFO];
							bzero(where, sizeof(where));
							snprintf(where, MAX_CHATINFO-1, "ACTID=%u AND CHARID=%u", rev->id, pUser->id);
							BYTE st = 1;
							SessionService::dbConnPool->exeUpdate(handle, "`GIFT`", gift_got_define, &st, where);
							SessionService::dbConnPool->putHandle(handle);

							pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "%u件奖品已经发送至邮箱，请到传递者处领取", i);
					}
					else
							pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "对不起，你没有奖品");
			}
			break;
		default:
			break;
	}
	return true;
}
