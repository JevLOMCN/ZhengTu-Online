#include "BillUserManager.h"



BillUserManager *BillUserManager::instance(NULL);


BillUserManager::BillUserManager()
{
}
BillUserManager::~BillUserManager()
{
}
bool BillUserManager::getUniqeID(DWORD &tempid)
{
	return true;
}
void BillUserManager::putUniqeID(const DWORD &tempid)
{
}
BillUserManager *BillUserManager::getInstance()
{
	if(instance == NULL)
		instance = new BillUserManager();
	return instance;
}
void BillUserManager::delInstance()
{
	SAFE_DELETE(instance);
}
void BillUserManager::update()
{
	struct RefreshUserExec :public execEntry<BillUser>
	{
		zTime current;
		std::vector<DWORD> _del_vec;
		RefreshUserExec():current()
		{
		}
		bool exec(BillUser *pUser)
		{
			//登陆超时，删除
			switch(pUser->loginTimeOut(current))
			{
				case BillUser::WAIT_LOGIN_TIMEOUT:
					{
						Zebra::logger->debug("登陆信息超时：%u, %u", pUser->id, pUser->tempid);
						_del_vec.push_back(pUser->id);
					}
					break;
				case BillUser::CONF_LOGOUT:
					{
						_del_vec.push_back(pUser->id);
					}
					break;
				case BillUser::CONF_LOGIN:
					{
					}
					break;
				default:
					break;
			}
			return true;
		}
	};
	RefreshUserExec exec;
	BillUserManager::getInstance()->execEveryUser(exec);
	for(std::vector<DWORD>::iterator iter = exec._del_vec.begin() ; iter != exec._del_vec.end() ; iter++)
	{
		BillUser *pUser = BillUserManager::getInstance()->getUserByID(*iter);
		if(pUser)
		{
			BillUserManager::getInstance()->removeUser(pUser);
		}
	}
}
void BillUserManager::removeUserByGatewayID(BillTask *task)
{
	struct RemoveUserExec :public execEntry<BillUser>
	{
		BillTask * _gatewaytask;
		std::vector<DWORD> _del_vec;
		RemoveUserExec(BillTask *task):_gatewaytask(task)
		{
		}
		bool exec(BillUser *pUser)
		{
			if(_gatewaytask == pUser->gatewaytask)
			{
				Zebra::logger->debug("网关关闭，清除登陆数据：%u, %u", pUser->id, pUser->tempid);
				_del_vec.push_back(pUser->id);
			}
			return true;
		}
	};
	RemoveUserExec exec(task);
	BillUserManager::getInstance()->execEveryUser(exec);
	for(std::vector<DWORD>::iterator iter = exec._del_vec.begin() ; iter != exec._del_vec.end() ; iter++)
	{
		BillUser *pUser = BillUserManager::getInstance()->getUserByID(*iter);
		if(pUser)
		{
			BillUserManager::getInstance()->removeUser(pUser);
		}
	}
}
/*
bool BillUserManager::updateGold(DWORD acc , double gold);
bool BillUserManager::updateVipTime(DWORD acc , DWORD vip);
DWORD BillUserManager::getVipTime(DWORD acc);
double BillUserManager::getGold(DWORD acc);
// */
