/**
 * \file
 * \version  $Id: zebraclientManager.cpp  $
 * \author   
 * \date
 * \brief 机器人管理器
 *
 * 
 */

#include "zebraclientManager.h"
#include "zebraclient.h"
#include <algorithm>



ZebraClientManager *ZebraClientManager::_instance=NULL;
ZebraClientManager *ZebraClientManager::getInstance()
{
	if(!_instance)
	{
		_instance=new ZebraClientManager();
	}
	return _instance;
}
void ZebraClientManager::delInstance()
{
	if(_instance)
	{
		delete _instance;
	}
}
void ZebraClientManager::add(zebraClient *client)
{
	rwlock.wrlock();
	this->client.push_back(client);
	rwlock.unlock();
}
void ZebraClientManager::remove(zebraClient *client)
{
	rwlock.wrlock();
	ClientContainerIter iter = find(this->client.begin() , this->client.end() , client); 
	if(iter != this->client.begin())
	{
		this->client.erase(iter);
	}
	rwlock.unlock();
}

void ZebraClientManager::timeAction()
{
	rwlock.rdlock();
	for(ClientContainerIter iter = client.begin();iter != client.end(); iter++)
	{
		(*iter)->timeAction();
	}
	rwlock.unlock();
}
bool ZebraClientManager::addClientTask(zebraClient *client , DWORD acc , DWORD tempid , DES_cblock key)
{
	if(!client)
	{
		return false;
	}
	client->connect();
	//client->getNextState();
	//client->setState(zTCPClientTask::okay);
	//if (!client->loginGatewayServer(acc , tempid))
	//	return false;
	if(!client->init(acc , tempid , key))
	{
		client->Terminate(zTCPClientTask::TM_service_close);
		return false;
	}
	taskPool->addCheckwait(client);
	//add(client);
	//taskPool->put(client); 
	//taskPool->addMain(client);
	return true;
}
bool ZebraClientManager::init()
{
	int state = state_none;
	Zebra::to_lower(Zebra::global["initThreadPoolState"]);
	if ("repair" == Zebra::global["initThreadPoolState"]
			|| "maintain" == Zebra::global["initThreadPoolState"])
		state = state_maintain;
	taskPool = new zTCPClientTaskPool();/*(atoi(Zebra::global["threadPoolCapacity"].c_str()), state);*/
	if (NULL == taskPool
			|| !taskPool->init())
		return false;
	return true;
}
