/**
 * \file
 * \version  $Id: ScreenIndex.cpp  $
 * \author  
 * \date 
 * \brief 屏索引
 *
 * 
 */
#include "ScreenIndex.h"
#include "zSocket.h"

ScreenIndex::ScreenIndex(const DWORD x , const DWORD y):screenx(x),screeny(y),screenMax(x*y)
{
	const int adjust[9][2] = { {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, 0} };
	//预先建立地图九屏索引
	for(DWORD j=0; j < screenMax ; j ++)
	{
		index[j];
		int nScreenX = j % screenx;
		int nScreenY = j / screenx;
		//计算周围九屏
		{
			zPosIVector pv;
			for(int i = 0; i < 9; i++) {
				int x = nScreenX + adjust[i][0];
				int y = nScreenY + adjust[i][1];
				if (x >= 0 && y >= 0 && x < (int)screenx && y < (int)screeny) {
					pv.push_back(y * screenx + x);
				}
			}
			ninescreen.insert(NineScreen_map_value_type(j,pv));
		}
		//计算正向变化五屏或者三屏
		for(int dir = 0; dir < 8; dir++)
		{
			int start, end;
			zPosIVector pv;

			if (1 == dir % 2) {
				//斜方向
				start = 6;
				end = 10;
			}
			else {
				//正方向
				start = 7;
				end = 9;
			}
			for(int i = start; i <= end; i++) {
				int x = nScreenX + adjust[(i + dir) % 8][0];
				int y = nScreenY + adjust[(i + dir) % 8][1];
				if (x >= 0 && y >= 0 && x < (int)screenx && y < (int)screeny) {
					pv.push_back(y * screenx + x);
				}
			}
			direct_screen[dir].insert(NineScreen_map_value_type(j,pv));
		}
		//计算反向变化五屏或者三屏
		for(int dir = 0; dir < 8; dir++)
		{
			int start, end;
			zPosIVector pv;

			if (1 == dir % 2) {
				//斜方向
				start = 2;
				end = 6;
			}
			else {
				//正方向
				start = 3;
				end = 5;
			}
			for(int i = start; i <= end; i++) {
				int x = nScreenX + adjust[(i + dir) % 8][0];
				int y = nScreenY + adjust[(i + dir) % 8][1];
				if (x >= 0 && y >= 0 && x < (int)screenx && y < (int)screeny) {
					pv.push_back(y * screenx + x);
				}
			}
			reversedirect_screen[dir].insert(NineScreen_map_value_type(j,pv));
		}
	}
} 

template <class YourNpcEntry>
void ScreenIndex::execAllOfScreen(const DWORD screen,execEntry<YourNpcEntry> &callback)
{
	SceneEntry_SET &set = index[screen];
	wrlock.rdlock();
	SceneEntry_SET::iterator it = set.begin(), ed = set.end();
	for(; it != ed; ++it)
	{
		callback.exec(*it);
	}
	wrlock.unlock();
}
struct SendNineExec : public execEntry<GateUser>
{
	const void  *_cmd;
	DWORD _cmdLen;
	DWORD _type;
	char _name[MAX_NAMESIZE];
	int _sendLen;
	bool zip;
	t_StackCmdQueue cmd_queue;
	SendNineExec(const void *cmd , const int cmdLen):_cmd(cmd),_cmdLen(cmdLen),_type(0),zip(false)
	{
	}
	bool exec(GateUser *pUser)
	{
		if(!zip)
		{
			zip=true;
			bzero(_name,sizeof(_name));
			GateUser::cmdFilter((Cmd::stNullUserCmd *)_cmd,_type,_name,_cmdLen);
			_sendLen = zSocket::packetPackZip(_cmd,_cmdLen,cmd_queue); 
		}
		pUser->sendCmd(cmd_queue.rd_buf() , _sendLen,_type,_name,true);
		return true;
	}
};

void ScreenIndex::sendCmdToNine(const DWORD posi, const void *pstrCmd, const int nCmdLen)
{
	SendNineExec exec(pstrCmd , nCmdLen);
	const zPosIVector &pv = getNineScreen(posi);
	zPosIVector::const_iterator it = pv.begin(), ed = pv.end();
	for(; it != ed; ++it)
	{
		/*
		Cmd::t_NullCmd *ptNullCmd = (Cmd::t_NullCmd *)pstrCmd;
		if(ptNullCmd->cmd != 6 && ptNullCmd->para !=20)
		Zebra::logger->debug("%s屏索引%d,消息:(%d,%d)",__FUNCTION__,*it,ptNullCmd->cmd,ptNullCmd->para);
		// */
		execAllOfScreen(*it , exec);
	}
}

void ScreenIndex::sendCmdToDirect(const zPosI posi, const int direct, const void *pstrCmd, const int nCmdLen)
{
	SendNineExec exec(pstrCmd , nCmdLen);
	const zPosIVector &pv = getDirectScreen(posi, direct);
	zPosIVector::const_iterator it = pv.begin(), ed = pv.end();
	for(; it != ed; ++it)
	{
		/*
		Cmd::t_NullCmd *ptNullCmd = (Cmd::t_NullCmd *)pstrCmd;
		if(ptNullCmd->cmd != 6 && ptNullCmd->para !=20)
		Zebra::logger->debug("%s屏索引%d,消息:(%d,%d)",__FUNCTION__,*it,ptNullCmd->cmd,ptNullCmd->para);
		// */
		execAllOfScreen(*it , exec);
	}
}

void ScreenIndex::sendCmdToReverseDirect(const zPosI posi, const int direct, const void *pstrCmd, const int nCmdLen)
{
	SendNineExec exec(pstrCmd , nCmdLen);
	const zPosIVector &pv = getReverseDirectScreen(posi, direct);
	zPosIVector::const_iterator it = pv.begin(), ed = pv.end();
	for(; it != ed; ++it)
	{
		/*
		Cmd::t_NullCmd *ptNullCmd = (Cmd::t_NullCmd *)pstrCmd;
		if(ptNullCmd->cmd != 6 && ptNullCmd->para !=20)
		Zebra::logger->debug("%s屏索引%d,消息:(%d,%d)",__FUNCTION__,*it,ptNullCmd->cmd,ptNullCmd->para);
		// */
		execAllOfScreen(*it , exec);
	}
}

void ScreenIndex::sendCmdToAll(const void *pstrCmd, const int nCmdLen)
{
	int _sendLen;
	t_StackCmdQueue cmd_queue;
	DWORD _type=0;
	char _name[MAX_NAMESIZE];
	bzero(_name,sizeof(_name));
	DWORD cmdLen=nCmdLen;
	GateUser::cmdFilter((Cmd::stNullUserCmd *)pstrCmd,_type,_name,cmdLen);
	_sendLen = zSocket::packetPackZip(pstrCmd,nCmdLen,cmd_queue);
	wrlock.rdlock();
	SceneEntry_SET::iterator it=all.begin(), ed = all.end();
	for(; it != ed; ++it)
	{
		(*it)->sendCmd(cmd_queue.rd_buf(),_sendLen,_type,_name,true);
	}
	wrlock.unlock();
}

struct SendNineExecExceptMe : public execEntry<GateUser>
{
	const DWORD _exceptme_id;
	const void  *_cmd;
	DWORD _cmdLen;
	DWORD _type;
	char _name[MAX_NAMESIZE];
	int _sendLen;
	t_StackCmdQueue cmd_queue;
	SendNineExecExceptMe(const DWORD exceptme_id, const void *cmd , const int cmdLen):_exceptme_id(exceptme_id),_cmd(cmd),_cmdLen(cmdLen),_type(0)
	{
		bzero(_name,sizeof(_name));
		GateUser::cmdFilter((Cmd::stNullUserCmd *)_cmd,_type,_name,_cmdLen);
		_sendLen = zSocket::packetPackZip(_cmd,_cmdLen,cmd_queue); 
	}
	bool exec(GateUser *pUser)
	{
		if (_exceptme_id != pUser->id)
			pUser->sendCmd(cmd_queue.rd_buf() , _sendLen,_type,_name,true);
		return true;
	}
};

void ScreenIndex::sendCmdToNineExceptMe(const DWORD posi, const DWORD exceptme_id, const void *pstrCmd, const int nCmdLen)
{
	SendNineExecExceptMe exec(exceptme_id, pstrCmd , nCmdLen);
	const zPosIVector &pv = getNineScreen(posi);
	zPosIVector::const_iterator it = pv.begin(), ed = pv.end();
	for(; it != ed; ++it)
	{
		/*
		Cmd::t_NullCmd *ptNullCmd = (Cmd::t_NullCmd *)pstrCmd;
		if(ptNullCmd->cmd != 6 && ptNullCmd->para !=20)
		Zebra::logger->debug("%s屏索引%d,消息:(%d,%d)",__FUNCTION__,*it,ptNullCmd->cmd,ptNullCmd->para);
		// */
		execAllOfScreen(*it , exec);
	}
}

bool ScreenIndex::refresh(GateUser *e, const DWORD newIndex)
{
	zRWLock_scope_wrlock scope_wrlock(wrlock);
	if(e==NULL) return false;
	//-2 表示删除状态，不可以被场景添加
	//-1 表示等待添加状态
	if(e->getIndexKey() == (DWORD)-2 && newIndex != (DWORD)-1) return false;

	if(e->inserted)
	{
		//已经加入地图索引，只是在屏之间来回切换
		bool ret=false;

		SceneEntry_SET &pimi = index[e->getIndexKey()];
		SceneEntry_SET::const_iterator it = pimi.find(e);
		if (it != pimi.end())
		{
			//Zebra::logger->debug("[地图索引]用户切换：%s, %u, %u", e->name, e->getIndexKey(), newIndex);
			ret=true;
			pimi.erase(it);
			index[newIndex].insert(e);
			e->setIndexKey(newIndex);
		}

		return ret;
	}
	else if (newIndex != (DWORD)-1)
	{
		//在全局索引中添加
		if (all.insert(e).second)
		{
			//Zebra::logger->debug("[地图索引]用户加入：%s, %u, %u", e->name, e->getIndexKey(), newIndex);
			//新加入地图索引
			index[newIndex].insert(e);
		}
		//else
		//	Zebra::logger->error("[地图索引]用户加入失败，已经在索引中：%s, %u, %u", e->name, e->getIndexKey(), newIndex);
		e->inserted=true;
	}
	//else
	//	Zebra::logger->debug("[地图索引]用户设置可加入状态：%s, %u, %u", e->name, e->getIndexKey(), newIndex);

	e->setIndexKey(newIndex);
	return e->inserted;
}

void ScreenIndex::removeGateUser(GateUser *e)
{
	zRWLock_scope_wrlock scope_wrlock(wrlock);
	if(e==NULL || !e->inserted) return;

	SceneEntry_SET::iterator it = all.find(e);
	if (it != all.end())
	{
		//Zebra::logger->debug("[地图索引]用户删除：%s, %u", e->name, e->getIndexKey());
		SceneEntry_SET &pimi = index[e->getIndexKey()];
		e->setIndexKey((DWORD)-2);
		//在全局索引中删除
		all.erase(it);
		//在屏索引中删除
		pimi.erase(e);
		e->inserted=false;
	}
}

/**
 * \brief 以中心屏获取周围9屏的屏幕编号
 * \param posi 中心屏
 * \param pv 输出的屏编号向量
 */
const zPosIVector &ScreenIndex::getNineScreen(const zPosI &posi)
{
	NineScreen_map_iter iter = ninescreen.find((DWORD)posi);
	if(iter != ninescreen.end())
	{
		return iter->second;
	}
	//如果出现异常返回0编号的屏索引
	return ninescreen[(DWORD)-1];
}

/**
 * \brief 以中心屏获取向前的3屏或者5屏的屏幕编号
 * \param posi 中心屏
 * \param direct 方向
 * \return 输出的屏编号向量
 */
const zPosIVector &ScreenIndex::getDirectScreen(const zPosI &posi, const int dir)
{
	NineScreen_map_iter iter = direct_screen[dir].find((DWORD)posi);
	if(iter != direct_screen[dir].end())
	{
		return iter->second;
	}
	//如果出现异常返回0编号的屏索引
	return direct_screen[dir][(DWORD)-1];
}

/**
 * \brief 以中心屏获取向后的3屏或者5屏的屏幕编号
 * \param posi 中心屏
 * \param direct 方向
 * \return 输出的屏编号向量
 */
const zPosIVector &ScreenIndex::getReverseDirectScreen(const zPosI &posi, const int dir)
{
	NineScreen_map_iter iter = reversedirect_screen[dir].find((DWORD)posi);
	if(iter != reversedirect_screen[dir].end())
	{
		return iter->second;
	}
	//如果出现异常返回0编号的屏索引
	return reversedirect_screen[dir][(DWORD)-1];
}

