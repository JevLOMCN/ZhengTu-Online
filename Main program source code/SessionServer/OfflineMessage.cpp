/**
 * \file
 * \version  $Id: OfflineMessage.cpp  $
 * \author  
 * \date 
 * \brief 离线消息管理类实现
 *
 */

#include <set>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "Zebra.h"
#include "OfflineMessage.h"
#include "Session.h"
#include "zTime.h"
#include "zMisc.h"

#define FILEPOWER 509
#define MAX_MESSAGE_NUMBER 5

struct ltstring
{
	bool operator()(const std::string s1, const std::string s2) const
	{
		return s1<s2;
	}
};

/**
 * \brief 初始化离线聊天系统，删除所有离线信息，
 * \author fqnewman
 * \return 失败返回false,成功返回true
 */
bool COfflineMessage::init()
{
	rootpath = Zebra::global["offlineMsgPath"];
	if (rootpath == "")
	{
		rootpath=std::string("/tmp/offlineMsg");
	}
	else
	{
		rootpath+=std::string("/offlineMsg");
	}

	
	if (access(rootpath.c_str(), F_OK) == 0)
	{
		if (!zMisc::rmDirTree(rootpath.c_str()))
		{
			Zebra::logger->error("初始化删除离线消息目录[%s]失败",rootpath.c_str());
			return false;
		}
	}

	if (mkdir(rootpath.c_str(), FILEPOWER)!=0)
	{
		Zebra::logger->error("初始化建立离线消息目录失败");
		return false;
	}
	return true;
}

/**
 * \brief 写入离线消息
 *
 * \param type:	消息类型
 * \param id:	角色ID
 * \param ptNullCmd: 消息命令
 * \param cmdLen:	消息长度
 *
 * \author fqnewman
 */
void COfflineMessage::writeOfflineMessage(const BYTE &type, const DWORD &id, const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen)
{
	std::set<std::string, ltstring> filelist;
	char buf[MAX_NAMESIZE];
	sprintf(buf,"/%u",id);
	std::string myPath = rootpath+buf;

	if (access(myPath.c_str(), F_OK) != 0)
	{
		mkdir(myPath.c_str(), FILEPOWER);   /// 建立帐号目录
	}

	sprintf(buf,"/%u",ptNullCmd->byCmd);
	myPath = myPath+buf;

	if (access(myPath.c_str(), F_OK) != 0)
	{
		mkdir(myPath.c_str(), FILEPOWER);  /// 建立消息分类目录
	}

	sprintf(buf,"/%u",ptNullCmd->byParam);
	myPath = myPath+buf;

	if (access(myPath.c_str(), F_OK) != 0)
	{
		mkdir(myPath.c_str(), FILEPOWER);  /// 建立消息号目录
	}

	sprintf(buf,"/%u",type);
	myPath = myPath+buf;

	if (access(myPath.c_str(), F_OK) != 0)
	{
		mkdir(myPath.c_str(), FILEPOWER);  /// 建立自定义类型目录
	}
/// 开始检查文件数目
	struct dirent *record;
	DIR* tDir = opendir(myPath.c_str());
	if (tDir != NULL)
	{
		while(NULL != (record = readdir(tDir)))
		{
			if (record->d_type == DT_REG)
			{
				filelist.insert(std::string(myPath+"/"+record->d_name));
			}
		}
		closedir(tDir);
	}
	while(filelist.size()>= MAX_MESSAGE_NUMBER) /// 如果保存消息数目超过某类型消息数目，则删除老的记录以保持指定的数量
	{
		unlink(filelist.begin()->c_str());
		filelist.erase(filelist.begin());
	}

	do {
		zRTime ctv;
		sprintf(buf,"/%lu",ctv.sec());
	}while(access(std::string(myPath+buf).c_str(), F_OK) == 0);
    
	myPath = myPath+buf;  // 最终的路径文件名

//	Zebra::logger->debug("写入一个新的离线消息[%s]",myPath.c_str());
	int fd;

	if ((int)-1 != (fd = open(myPath.c_str(), O_CREAT|O_WRONLY, FILEPOWER)))
	{
		write(fd,ptNullCmd,cmdLen);
		close(fd);
	}
	else
	{
		Zebra::logger->error("无法写入角色[%u]的离线消息",id);
	}
}

/**
 * \brief 上线的角色查找自己的离线消息
 * \param pUser:	上线的角色
 * \author: fqnewman
 */
void COfflineMessage::getOfflineMessage(const UserSession *pUser)
{
	char buf[MAX_NAMESIZE];
	sprintf(buf,"/%u",pUser->id);
	std::string myPath = rootpath+buf;
	getOfflineMessageSetAndSend(pUser, myPath);
}

/**
 * \brief 从指定路径开始查找本层极其子目录中的离线消息并发送
 * \param pUser:	角色
 * \param path:	路径
 * \author fqnewman
 */
void COfflineMessage::getOfflineMessageSetAndSend(const UserSession *pUser, std::string path)
{
	struct dirent *record;
	std::set<std::string, ltstring> filelist;
	std::set<std::string, ltstring>::iterator tIterator;

	DIR* tDir = opendir(path.c_str());
	if (tDir == NULL) return;
	while(NULL != (record = readdir(tDir)))
	{
		if (record->d_type == DT_DIR)
		{
			if (strcmp(record->d_name,".") == 0 ||	strcmp(record->d_name,"..") == 0) continue;

			getOfflineMessageSetAndSend(pUser, (path+"/"+record->d_name).c_str());
		}
		else if (record->d_type == DT_REG)
		{
			filelist.insert(std::string(path+"/"+record->d_name));
		}
	}
	closedir(tDir);

	for (tIterator = filelist.begin(); tIterator != filelist.end(); tIterator++)
	{
		int fd;
		unsigned int cmdLen;
		BYTE buf[zSocket::MAX_DATASIZE];

		bzero(buf,zSocket::MAX_DATASIZE);
		if ((int)-1 != (fd = open((*tIterator).c_str(), O_RDONLY)))
		{
			cmdLen = read(fd,buf,zSocket::MAX_DATASIZE);
			pUser->sendCmdToMe(buf, cmdLen);
			close(fd);
		}
		else
		{
			Zebra::logger->error("无法读取角色[%s]的离线消息",pUser->name);
		}
		unlink(tIterator->c_str());
	}

	rmdir(path.c_str());
	return;
}
