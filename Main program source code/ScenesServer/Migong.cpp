#include "SceneUser.h"
#include "SceneNpcManager.h"
#include "NpcTrade.h"
#include "RebuildObject.h"
#include "ScenesServer.h"
#include "CharBase.h"
#include "Command.h"
#include <string>
#include "fjconfig.h"
#include "SessionClient.h"

// 迷宫系统 by:醉梦
bool SceneUser::doMigongCmd(const Cmd::stMigongUserCmd *ptCmd, unsigned int cmdLen)
{
	using namespace Cmd;
	switch (ptCmd->byParam) //这里就是判断子命令了byParam
	{
		case MIGONG_NEXT:
		{
			struct tm tv1;
			time_t timValue = time(NULL);
			zRTime::getLocalTime(tv1, timValue);
			if(tv1.tm_hour <19)//迷宫 19点开始
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "不在迷宫活动时间，每日19点-24点开放");
				return false;
			}
			if (this->charbase.migongcs >= 1)
			{
				Gm::gomap(this, "name=中立区·皇城 pos=832,699");
				Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您今日已经完成迷宫活动");
				return false;
			}
			stMigongNextCmd *cmd = (stMigongNextCmd *)(ptCmd);
			int migongnum = 1;
			if 	(this->scene->getRealMapID()>=223 && this->scene->getRealMapID()<=247 )//在迷宫内
			{
				migongnum = this->scene->getRealMapID() - 222; //获取当前层数
				for (int i = 0; i < 25; i++)//遍历迷宫的密码表，找到当前层数对应的密码
				{
					if(ScenesService::getInstance().migong[i].ceng == migongnum)//找到了当前层数对应的密码
					{
						if(ScenesService::getInstance().migong[i].password == cmd->npcid)//密码正确
						{
							int up  = zMisc::randBetween(1,1);//随机向上移动1-4层
							if(migongnum + up <= 25)//在迷宫层数范围内
							{
								char map[128];
								sprintf(map, "name=中立区·迷宫%d层  pos=53,80",migongnum + up); //获取下一层地图ID
								Gm::gomap(this, map);  //自动传送地图坐标
								if(migongnum + up == 25)//直接给奖励
								{
									this->charbase.migongcs = 1;
									char a[16];
									sprintf(a, "%d", atoi(Zebra::global["MIGONGWC"].c_str())+1);
									Zebra::global["MIGONGWC"]=a;
									if(atoi(Zebra::global["MIGONGNUM1"].c_str())==0)
									{
										//第一

										char a[16];
										sprintf(a, "%d", 1);
										Zebra::global["MIGONGNUM1"]=a;
										SceneUser::addBindObjectNum(120058,1,0,0,1);			
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"恭喜%s的%s第一名到达迷宫25层获得大量奖励！！祝贺TA！！！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL,"恭喜%s的%s第一名到达迷宫25层获得大量奖励！！祝贺TA！！！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
												
									}
									else if(atoi(Zebra::global["MIGONGNUM2"].c_str())==0)
									{
										char a[16];
										sprintf(a, "%d", 1);
										Zebra::global["MIGONGNUM2"]=a;
										SceneUser::addBindObjectNum(120059,1,0,0,1);			
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"恭喜%s的%s第二名到达迷宫25层获得大量奖励！！祝贺TA！！！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL,"恭喜%s的%s第二名到达迷宫25层获得大量奖励！！祝贺TA！！！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
										

									}
									else if(atoi(Zebra::global["MIGONGNUM3"].c_str())==0)
									{
										//第三
										char a[16];
										sprintf(a, "%d", 1);
										Zebra::global["MIGONGNUM3"]=a;
										SceneUser::addBindObjectNum(120060,1,0,0,1);			
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"恭喜%s的%s第三名到达迷宫25层获得大量奖励！！祝贺TA！！！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL,"恭喜%s的%s第三名到达迷宫25层获得大量奖励！！祝贺TA！！！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
										
									}
									else{
										if(atoi(Zebra::global["MIGONGWC"].c_str())>=10)
										{
											//参与奖
											SceneUser::addBindObjectNum(120062,1,0,0,1);			
											Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"%s的%s完成迷宫活动，获得大量奖励！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
											
										}
										else{
											SceneUser::addBindObjectNum(120061,1,0,0,1);			
											Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"%s的%s完成迷宫活动，获得4-10名礼包！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
										
										}
					
									}
								}
							}
							else//直接传送25层
							{
								this->charbase.migongcs =1;
								char map[128];
								sprintf(map, "name=中立区·迷宫25层  pos=53,63"); //获取下一层地图ID
								Gm::gomap(this, map);  //自动传送地图坐标
								//直接给奖励
								char a[16];
								sprintf(a, "%d", atoi(Zebra::global["MIGONGWC"].c_str()) + 1);
								Zebra::global["MIGONGWC"] = a;
								if (atoi(Zebra::global["MIGONGNUM1"].c_str()) == 0)
								{
									// 第一

									char a[16];
									sprintf(a, "%d", 1);
									Zebra::global["MIGONGNUM1"] = a;
									SceneUser::addBindObjectNum(120058, 1, 0, 0, 1);
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "恭喜%s的%s第一名到达迷宫25层获得大量奖励！！祝贺TA！！！", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name);
									Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "恭喜%s的%s第一名到达迷宫25层获得大量奖励！！祝贺TA！！！", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name);
								}
								else if (atoi(Zebra::global["MIGONGNUM2"].c_str()) == 0)
								{
									char a[16];
									sprintf(a, "%d", 1);
									Zebra::global["MIGONGNUM2"] = a;
									SceneUser::addBindObjectNum(120059, 1, 0, 0, 1);
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "恭喜%s的%s第二名到达迷宫25层获得大量奖励！！祝贺TA！！！", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name);
									Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "恭喜%s的%s第二名到达迷宫25层获得大量奖励！！祝贺TA！！！", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name);
								}
								else if (atoi(Zebra::global["MIGONGNUM3"].c_str()) == 0)
								{
									// 第三
									char a[16];
									sprintf(a, "%d", 1);
									Zebra::global["MIGONGNUM3"] = a;
									SceneUser::addBindObjectNum(120060, 1, 0, 0, 1);
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "恭喜%s的%s第三名到达迷宫25层获得大量奖励！！祝贺TA！！！", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name);
									Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "恭喜%s的%s第三名到达迷宫25层获得大量奖励！！祝贺TA！！！", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name);
								}
								else
								{
									if (atoi(Zebra::global["MIGONGWC"].c_str()) >= 10)
									{
										// 参与奖
										SceneUser::addBindObjectNum(120062, 1, 0, 0, 1);
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "%s的%s完成迷宫活动，获得大量奖励！", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name);
									}
									else
									{
										SceneUser::addBindObjectNum(120061, 1, 0, 0, 1);
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "%s的%s完成迷宫活动，获得4-10名礼包！", SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country), this->charbase.name);
									}
								}
							}		
						}
						else //密码错误
						{
							int down = zMisc::randBetween(1,1);
							if(migongnum - down >= 1)//在迷宫层数范围内
							{
								char map[128];
								sprintf(map, "name=中立区·迷宫%d层  pos=53,80",migongnum - down); //获取下一层地图ID
								Gm::gomap(this, map);  //自动传送地图坐标
							}
							else//直接传送1层
							{
								char map[128];
								sprintf(map, "name=中立区·迷宫1层  pos=53,80"); //获取下一层地图ID
								Gm::gomap(this, map);  //自动传送地图坐标
							}
						}
						break;
					}
			
					
					
				}
			}
			
		}
		break;
	}
	return true;
}



