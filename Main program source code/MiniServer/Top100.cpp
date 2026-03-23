#include "Top100.h"
#include "MiniServer.h"
#include "MiniUser.h"
#include "zMetaData.h"

bool Top100::init()
{
	FieldSet* fs = MiniService::metaData->getFields("MINIGAME");

	if (fs)
	{
		connHandleID handle = MiniService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("Top100::init()不能获取数据库句柄");
			return false;
		}

		Record order;
		order.put("score", "desc");

		RecordSet *recordset = MiniService::dbConnPool->exeSelect(handle, fs, NULL, NULL, &order, 200);

		DWORD count=0;
		if (recordset)
		{
			for (unsigned int i=0; i<recordset->size() && count<120; i++)
			{
				Record* rec = recordset->get(i);

				Cmd::MiniUserData d;
				strncpy(d.name, (const char*)rec->get("name"), MAX_NAMESIZE-1);
				d.id = rec->get("charid");
				d.countryID = rec->get("country");
				d.face = rec->get("face");
				d.score.gameType = rec->get("gametype");
				d.score.win = rec->get("win");
				d.score.lose = rec->get("lose");
				d.score.draw = rec->get("draw");
				d.score.score = rec->get("score");

				if (d.score.win||d.score.lose||d.score.draw||d.score.score)
				{
					top100.push_back(d);
					Zebra::logger->debug("Top100	%s(%u)	score=%d", d.name, d.id, d.score.score);
				}
			}

			SAFE_DELETE(recordset)
		}

		MiniService::dbConnPool->putHandle(handle);
		return true;
	}
	return false;
}

struct findID
{
	findID(DWORD id):val(id){}
	bool operator()(const Cmd::MiniUserData &d)
	{
		return d.id==val;
	}
	private:
	DWORD val;
};
struct findScore
{
	findScore(Cmd::MiniGameScore score):val(score){}
	bool operator()(const Cmd::MiniUserData &d)
	{
		return d.score<val || d.score==val;
	}
	private:
	Cmd::MiniGameScore val;
};
void Top100::calculate(MiniUser *u)
{
	if (!u) return;

	Cmd::MiniGameScore s = u->getGameScore();

	/*
	for (top_iter i=top100.begin(); i!=top100.end(); i++)
		if (i->id==u->id)
		{
			top100.erase(i);
			return;
		}
		*/
	top_iter ti = std::find_if(top100.begin(), top100.end(), findID(u->id));
	if (ti!=top100.end()) top100.erase(ti);

	if (top100.size()>=120 && s<top100.rbegin()->score) return;

	//top_iter i=top100.begin();
	//for (; i!=top100.end() && i->score>s; i++);
	top_iter i = std::find_if(top100.begin(), top100.end(), findScore(s));

	Cmd::MiniUserData d;
	u->full_MiniUserData(0, d);
	top100.insert(i, d);

	if (top100.size()>120)
		top100.erase(--top100.end());

	//for (top_iter i=top100.begin(); i!=top100.end(); i++)
	//	Zebra::logger->debug("Top100\t%s(%u)\tscore=%d", i->name, i->id, i->score.score);
}

void Top100::send(MiniUser *u)
{
	if (!u || !top100.size()) return;

	char buf[zSocket::MAX_DATASIZE];
	bzero(buf, sizeof(buf));
	Cmd::stRetTopCommonMiniGameCmd * cmd = (Cmd::stRetTopCommonMiniGameCmd *)buf;
	constructInPlace(cmd);

	top_iter it = top100.begin();
	for (DWORD i=0; i<top100.size() && i<=100; i++,it++)
	{
		cmd->data[cmd->num] = *it;
		cmd->num++;
	}

	u->sendCmdToMe(cmd, sizeof(Cmd::stRetTopCommonMiniGameCmd)+sizeof(Cmd::MiniUserData)*cmd->num);
}

void Top100::remove(DWORD id)
{
	top_iter ti = std::find_if(top100.begin(), top100.end(), findID(id));
	if (ti!=top100.end()) top100.erase(ti);
}
