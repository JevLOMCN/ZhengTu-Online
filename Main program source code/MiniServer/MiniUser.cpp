#include "MiniUser.h"
#include "MiniUserCommand.h"
#include "zMetaData.h"
#include <stdarg.h>
#include "MiniServer.h"
#include "zMetaData.h"
#include "Top100.h"

MiniUser::MiniUser(DWORD i, char *n, WORD c, WORD f, MiniTask *t, MiniTask *s)
	:face(f),country(c),minitask(t),scene(s)
{
	id = i;
	minitask = t;
	strncpy(name, n, MAX_NAMESIZE-1);
	needSave = false;
}

void MiniUser::setScene(MiniTask *s)
{
	if (s)
		scene = s;
	else
		Zebra::logger->error("MiniUser::setScene %s(%u)试图切换不存在的场景", name, id);
}

bool MiniUser::sendCmdToMe(const void *pstrCmd, const int nCmdLen) const
{
	if(minitask)
		return minitask->sendCmdToUser(id, pstrCmd, nCmdLen);
	return false;
}

bool MiniUser::sendCmdToScene(const void *pstrCmd, const int nCmdLen) const
{
	if(scene)
		return scene->sendCmd(pstrCmd, nCmdLen);
	return false;
}

#define getMessage(msg,msglen,pat)  \
	do  \
{   \
	    va_list ap; \
	    bzero(msg, msglen); \
	    va_start(ap, pat);      \
	    vsnprintf(msg, msglen - 1, pat, ap);    \
	    va_end(ap); \
}while(false)   

void MiniUser::sendSys(int type, const char *pattern, ...) const
{
	char buf[MAX_CHATINFO];

	getMessage(buf,MAX_CHATINFO,pattern);
	Cmd::stChannelChatUserCmd send;
	send.dwType=Cmd::CHAT_TYPE_SYSTEM;
	send.dwSysInfoType = type;
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));
	strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);
	sendCmdToMe(&send,sizeof(send));
}

void MiniUser::sendMiniInfo(int type, const char *info, ...) const
{
	using namespace Cmd;

	stChatCommonMiniGameCmd send;
	send.type = type;
	send.pos = id;
	getMessage(send.content, sizeof(send.content), info);

	sendCmdToMe(&send,sizeof(send));
}

Cmd::MiniUserPosition MiniUser::getGamePos(const BYTE &t)
{
	if (getGameState(t)!=Cmd::MUS_NOTPLAY)
		return gameList[t].pos;
	return 0;
}

void MiniUser::setGamePos(const BYTE &t, const Cmd::MiniUserPosition &p)
{
	gameList[t].pos = p;
	Zebra::logger->debug("%s(%u) 位置 %u", name, id, p.id());
}

Cmd::MiniUserState MiniUser::getGameState(const BYTE &t)
{
	if (gameList.find(t)==gameList.end()) return Cmd::MUS_NOTPLAY;

	return gameList[t].state;
}

void MiniUser::setGameState(const BYTE &t, const Cmd::MiniUserState &s)
{
	//if (s==Cmd::MUS_NOTPLAY) gameList.erase(t);

	gameList[t].state = s;
	if (s==Cmd::MUS_NOTPLAY) gameList[t].pos = Cmd::MiniUserPosition();
}

void MiniUser::full_MiniUserData(const BYTE &t, Cmd::MiniUserData &data)
{
	data.id = id;
	strncpy(data.name, name, MAX_NAMESIZE-1);
	data.countryID = country;
	data.face = face;
	data.pos = getGamePos(t);
	data.state = getGameState(t);
	data.score = getGameScore();
}

void MiniUser::addScore(Cmd::MiniGameScore s, bool isNew)
{
	score += s;
	if (isNew)
	{
		needSave = true;
		Top100::getMe().calculate(this);
	}
}

bool MiniUser::save()
{
	if (!needSave) return true;

	connHandleID handle = MiniService::dbConnPool->getHandle();

	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("MiniUser::save 不能获取数据库句柄");
		return false;
	}

	FieldSet* fs = MiniService::metaData->getFields("MINIGAME");
	Record rec,where;
	char w[32];
	bzero(w, sizeof(w));
	snprintf(w, sizeof(w)-1, "`CHARID`=%u", id);
	where.put("charid", w);

	rec.put("name", name);
	rec.put("country", country);
	rec.put("face", face);
	rec.put("win", score.win);
	rec.put("win", score.win);
	rec.put("lose", score.lose);
	rec.put("draw", score.draw);
	rec.put("score", score.score);
	rec.put("money", score.money);

	DWORD ret = MiniService::dbConnPool->exeUpdate(handle, fs, &rec, &where);
	MiniService::dbConnPool->putHandle(handle);
	if ((DWORD)-1==ret)
	{
		Zebra::logger->error("用户保存，写数据库失败! %s(%u) score=%u money=%u ret=%d", name, id, score.score, score.money, ret);
		return false;
	}

	return true;
}

Cmd::MiniGameScore MiniUser::getGameScore()
{
	return score;
}

void MiniUser::sendGameScore(MiniUser *u)
{
	if (!u) u = this;

	Cmd::stUserScoreCommonMiniGameCmd send;
	send.userID = id;
	send.score = score;
	u->sendCmdToMe(&send, sizeof(send));
}

int MiniUser::getMoney()
{
	return score.money;
}

bool MiniUser::addMoney(int num)
{
	if (num<=0) return true;

	needSave = true;

	if (score.money+num>MAX_MONEY)
	{
		score.money = MAX_MONEY;
		return false;
	}

	score.money += num;

	Cmd::stUserScoreCommonMiniGameCmd send;
	send.userID = id;
	send.score = score;
	sendCmdToMe(&send, sizeof(send));
	return true;
}

bool MiniUser::checkMoney(int num)
{
	return score.money>=num;
}

bool MiniUser::removeMoney(int num)
{
	if (num<=0) return true;
	if (score.money<num) return false;

	score.money -= num;
	needSave = true;

	Cmd::stUserScoreCommonMiniGameCmd send;
	send.userID = id;
	send.score = score;
	sendCmdToMe(&send, sizeof(send));
	return true;
}
