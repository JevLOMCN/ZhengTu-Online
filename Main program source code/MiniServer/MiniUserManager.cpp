#include "MiniUserCommand.h"
#include "MiniUserManager.h"
#include "MiniServer.h"
#include "MiniTaskManager.h"
#include "MiniHall.h"
#include "zMetaData.h"

MiniUserManager *MiniUserManager::instance(NULL);

MiniUserManager::MiniUserManager()
	:saveGroup(1)
{
}
MiniUserManager::~MiniUserManager()
{
}

MiniUserManager *MiniUserManager::getInstance()
{
	if(instance == NULL)
		instance = new MiniUserManager();
	return instance;
}

MiniUserManager &MiniUserManager::getMe()
{
	if(instance == NULL)
		instance = new MiniUserManager();
	return *instance;
}

struct saveCallBack : public execEntry<MiniUser>
{
	BYTE saveGroup;
	saveCallBack(BYTE g=0)
	{
		saveGroup = g%10;
	}
	bool exec(MiniUser *u)
	{
		if (!u) return true;
		if (!saveGroup || (saveGroup && u->id%10==saveGroup))
			u->save();
		return true;
	}
};

void MiniUserManager::update()
{
	saveGroup = saveGroup%10+1;
	saveCallBack s(saveGroup);
	MiniUserManager::getMe().execEveryUser(s);
}

void MiniUserManager::delInstance()
{
	saveCallBack s(0);
	getMe().execEveryUser(s);
	SAFE_DELETE(instance);
}

MiniUser * MiniUserManager::newUser(Cmd::Mini::t_UserLogin_Gateway *info)
{
	if (!info) return 0;

	MiniTask *gate = MiniTaskManager::getInstance().getTaskByID(info->gateServerID);
	if (!gate) return 0;
	MiniTask *scene = MiniTaskManager::getInstance().getTaskByID(info->sceneServerID);
	if (!scene) return 0;

	MiniUser * u = new MiniUser(info->userID, info->name, info->countryID, info->face, gate, scene);
	if (!u) return 0;

	FieldSet* fs = MiniService::metaData->getFields("MINIGAME");

	if (fs)
	{
		connHandleID handle = MiniService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle) 
		{   
			Zebra::logger->error("newUser()不能获取数据库句柄");
			delete u;
			return 0;
		}   

		Record where;   
		char w[32];
		bzero(w, sizeof(w));
		snprintf(w, sizeof(w)-1, "`CHARID`=%u", u->id);
		where.put("charid", w);

		RecordSet *recordset = MiniService::dbConnPool->exeSelect(handle, fs, NULL, &where);

		if (recordset)
		{   
			for (unsigned int i=0; i<recordset->size(); i++)
			{   
				Record* rec = recordset->get(i);
				Cmd::MiniGameScore s;
				//s.gameType = rec->get("gametype");
				s.win = rec->get("win");
				s.lose = rec->get("lose");
				s.draw = rec->get("draw");
				s.score = rec->get("score");
				s.money = rec->get("money");

				u->addScore(s, false);
#ifdef _XWL_DEBUG
				Zebra::logger->debug("%s type=%u win=%u lose=%u draw=%u score=%u", u->name, s.gameType, s.win, s.lose, s.draw, s.score);
#endif
			}

			SAFE_DELETE(recordset)
		}
		else
		{
			Record rec;

			rec.put("charid", u->id);
			rec.put("name", u->name);
			rec.put("country", u->country);
			rec.put("face", u->face);

			MiniService::dbConnPool->exeInsert(handle, fs, &rec);
		}

		MiniService::dbConnPool->putHandle(handle);
	}

	if (addUser(u))
		return u;
	else
	{
		delete u;
		return 0;
	}
}

void MiniUserManager::removeUserByGatewayID(MiniTask *task)
{
	struct RemoveUserExec :public execEntry<MiniUser>
	{
		MiniTask * _gatewaytask;
		std::vector<DWORD> _del_vec;
		RemoveUserExec(MiniTask *task):_gatewaytask(task)
		{
		}
		bool exec(MiniUser *pUser)
		{
			if(_gatewaytask == pUser->minitask)
			{
				Zebra::logger->debug("网关关闭，清除登陆数据：%u, %u", pUser->id, pUser->tempid);
				_del_vec.push_back(pUser->id);
			}
			return true;
		}
	};
	RemoveUserExec exec(task);
	MiniUserManager::getInstance()->execEveryUser(exec);
	for(std::vector<DWORD>::iterator iter = exec._del_vec.begin() ; iter != exec._del_vec.end() ; iter++)
	{
		MiniUser *pUser = MiniUserManager::getInstance()->getUserByID(*iter);
		if(pUser)
		{
			MiniHall::getMe().userLeave(pUser);
			//pUser->save();
			//MiniUserManager::getInstance()->removeUser(pUser);
		}
	}
}
