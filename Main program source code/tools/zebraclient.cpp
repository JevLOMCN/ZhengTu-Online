/**
 * \file
 * \version  $Id: zebraclient.cpp  $
 * \author  
 * \date 
 * \brief 测试客户端程序
 *
 * 
 */
#include "zebraclient.h"
#include "zebraclientManager.h"
#include "zebraclientService.h"


zTiles zebraClient::allTiles;
zPos zebraClient::sceneWH;
void zebraClient::firstAction()
{
	firstaction = false;
	//ArrayTable::getInstance()->init(charInfo[0].type);
	/*
	char buf[256];
	bzero(buf , sizeof(buf));
	strcpy(buf , "//value sp=1000 mp=1000");
	gm(buf , 256);
	bzero(buf , sizeof(buf));
	//strcpy(buf , "//levelup num=300");
	gm(buf , 256);
	bzero(buf , sizeof(buf));
	strcpy(buf ,"//setvalue five=100");
	gm(buf , 256);
	bzero(buf , sizeof(buf));
	strcpy(buf ,"//setvalue skill=300");
	gm(buf , 256);
	Cmd::stAddSkillPointPropertyUserCmd ret;
	sendCmd(&ret , sizeof(ret));
	for(int i = 0 ;zebraclientService::getInstance().isTerminate(), i < (int)ArrayTable::getInstance()->getSize() ; i ++)
	{
		Cmd::stAddSkillPointPropertyUserCmd ret;
		ret.dwSkillID = ArrayTable::getInstance()->get(i);
		sendCmd(&ret , sizeof(ret));
	}
	Cmd::stPKModeUserCmd pkm;
	pkm.byPKMode = Cmd::PKMODE_ENTIRE;
	sendCmd(&pkm , sizeof(pkm));
	// */
	operation(0);
	//moveReturned=true;
	//move(zMisc::randBetween(0, 7) , 1);
}
void zebraClient::gm(const char *data , const unsigned int size)
{
	Cmd::stChannelChatUserCmd chat;
	chat.dwType = Cmd::CHAT_TYPE_NINE;
	bcopy(data , &chat.pstrChat , sizeof(chat.pstrChat));
	sendCmd(&chat , sizeof(chat));
}
bool zebraClient::msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd;
	const stNullUserCmd *ptNull = (const stNullUserCmd *)ptNullCmd;
	//Zebra::logger->debug("收到消息:(%d,%d)",ptNull->byCmd,ptNull->byParam);
	switch(ptNull->byCmd)
	{
		case NULL_USERCMD:
			{
			return true;
			}
			break;
		case LOGON_USERCMD:
			{
			if (msgParse_logon(ptNull, nCmdLen))
				return true;
			}
			break;
		case TIME_USERCMD:
			{
			if (msgParse_time(ptNull, nCmdLen))
				return true;
			}
			break;
		case DATA_USERCMD:
			{
			if (msgParse_data(ptNull, nCmdLen))
				return true;
			}
			break;
		case PROPERTY_USERCMD:
			{
				return true;
			}
			break;
		case MAPSCREEN_USERCMD:
			{
				if (msgParse_mapscreen(ptNull, nCmdLen))
				return true;
			}
			break;
		case MOVE_USERCMD:
			{
			if (msgParse_move(ptNull, nCmdLen))
				return true;
			}
			break;
		case BUILD_USERCMD:
			{
				return true;
			}
			break;
		case MAKEOBJECT_USERCMD:
			{
				return true;
			}
			break;
		case RELIVE_USERCMD:
			{
			if (msgParse_relive(ptNull, nCmdLen))
				return true;
			}
			break;
		case CHAT_USERCMD:
			{
			if (msgParse_chat(ptNull, nCmdLen))
				return true;
			}
			break;
		case LEAVEONLINE_USERCMD:
			{
				return true;
			}
			break;
		case TRADE_USERCMD:
			{
				return true;
			}
			break;
		case MAGIC_USERCMD:
			{
				if (msgParse_magic(ptNull, nCmdLen))
				return true;
			}
			break;
		case UNION_USERCMD:
			{
				return true;
			}
			break;
		case COUNTRY_USERCMD:
			{
				return true;
			}
			break;
		case TASK_USERCMD:
			{
				return true;
			}
			break;
		case SELECT_USERCMD:
			{
				if (msgParse_select(ptNull, nCmdLen))
					return true;
			}
			break;
		case RELATION_USERCMD:
			{
				if (msgParse_relation(ptNull, nCmdLen))
					return true;
				return true;
			}
			break;
		case SCHOOL_USERCMD:
			{
				return true;
			}
			break;
		case SEPT_USERCMD:
			{
				return true;
			}
			break;
		case QUIZ_USERCMD:
			{
				switch(ptNull->byParam)
				{
					case QUERY_QUIZ_PARA:
						{
							stQueryQuiz *qq = (stQueryQuiz *)ptNull;
							stQueryQuiz ret = *qq;
							qq->byMsgType = QUIZ_YES;
							sendCmd(&ret , sizeof(stQueryQuiz));
							Zebra::logger->debug("收到请求答题消息");
						}
						break;
				}
			}
			break;
	}
	return false;
}

bool zebraClient::connect()
{
	using namespace Cmd;
	if (!zTCPClientTask::connect())
		return false;
	pSocket->setEncMethod(CEncrypt::ENCDEC_RC5);
	//Zebra::logger->debug("LogginTask: 起用RC5加密");
	//pSocket->enc.set_key_rc5((const unsigned char *)Zebra::global["rc5_key"].c_str(), 16, 12);
	//myy 登陆密钥
	unsigned char key[16] = {28, 196, 25, 36, 194, 125, 86, 196, 35, 92, 194, 41, 31, 241, 38, 224};
	//myy 客户端密钥
	/*BYTE key[16] = { 0x1c,0xc4,0x19,0x24,0xB8,0x7d,0x56,0xBA,0x23,0x5c,0xc2,0x29,0x1f,0xf2,0x25,0xdf};*/
	pSocket->set_key_rc5((const unsigned char *)key, 16, 12);
	if (!versionCmd())
		return false;
	return true;
}

bool zebraClient::msgParse_select(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen)
{
	using namespace Cmd;
	switch(ptNull->byParam)
	{
		case USERINFO_SELECT_USERCMD_PARA:
			{
				stUserInfoUserCmd *ptCmd = (stUserInfoUserCmd *)ptNull;
				bcopy(ptCmd->charInfo, charInfo, sizeof(charInfo));
				for(int i = 0; i < Cmd::MAX_CHARINFO; i++)
				{
					//Zebra::logger->debug("收到第%d个选择用户信息",i);
					//Zebra::logger->debug("id=%d name=%s level=%d mapName=%s",
							//charInfo[i].id,charInfo[i].name,charInfo[i].level,charInfo[i].mapName);
				}
				waitingCharInfo = true;
				return true;
			}
			break;
	}
	return false;
}
bool zebraClient::msgParse_relation(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen)
{
	using namespace Cmd;
	switch(ptNull->byParam)
	{
		case RELATION_STATUS_PARA:
			{
				stRelationStatusCmd *ptCmd = (stRelationStatusCmd*)ptNull;
				if(ptCmd->byState ==  RELATION_QUESTION)
				{
					ptCmd->byState = RELATION_ANSWER_YES;
					sendCmd(ptCmd , sizeof(stRelationStatusCmd));
				}
			}
			break;
	}
	return false;
}
bool zebraClient::msgParse_time(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen)
{
	using namespace Cmd;
	switch(ptNull->byParam)
	{
		case GAMETIME_TIMER_USERCMD_PARA:
			{
				stGameTimeTimerUserCmd *ptCmd = (stGameTimeTimerUserCmd *)ptNull;
				qwGameTime = ptCmd->qwGameTime;
				startTime.now();
				//Zebra::logger->debug("初始化游戏时间成功 %llu", qwGameTime);
				return true;
			}
			break;
		case REQUESTUSERGAMETIME_TIMER_USERCMD_PARA:
			{
				zRTime currentTime;
				if(!dwGameRunTime)
				{
					dwGameRunTime = currentTime.msecs();
				}
				stUserGameTimeTimerUserCmd cmd;
				cmd.dwUserTempID = 0;
				cmd.qwGameTime = qwGameTime + startTime.elapse(currentTime) / 1000;
				//Zebra::logger->debug("%u 检测游戏时间 %llu", accid, qwGameTime + startTime.elapse(currentTime) / 1000);
				return sendCmd(&cmd, sizeof(cmd));
			}
			break;
	}
	return false;
}
bool zebraClient::msgParse_mapscreen(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen)
{
	using namespace Cmd;
	switch(ptNull->byParam)
	{
		case ADDMAPOBJECT_MAPSCREEN_USERCMD_PARA:
			{
				stAddMapObjectMapScreenUserCmd *ptCmd = (stAddMapObjectMapScreenUserCmd*)ptNull;
				stNpcPosition obj;
				obj.dwTempID = ptCmd->data.dwMapObjectTempID;
				obj.x = ptCmd->data.x;
				obj.y = ptCmd->data.y;
				map_object.insert(MapEntry_value_type(obj.dwTempID , obj));
				zPos p;
				p.x = obj.x;
				p.y = obj.y;
				setBlock(p , TILE_OBJECT_BLOCK);
					//Zebra::logger->debug("设置物品阻挡k");
			}
			break;
		case REMOVEMAPOBJECT_MAPSCREEN_USERCMD_PARA:
			{
				stRemoveMapObjectMapScreenUserCmd *ptCmd = (stRemoveMapObjectMapScreenUserCmd*)ptNull;
				MapEntry_interator iter = map_object.find(ptCmd->dwMapObjectTempID);
				if(iter != map_object.end())
				{
					zPos p;
					p.x = iter->second.x;
					p.y = iter->second.y;
					clearBlock(p , TILE_OBJECT_BLOCK);
					//Zebra::logger->debug("删除物品阻挡k");
					map_object.erase(ptCmd->dwMapObjectTempID);
				}
			}
			break;
		case ALL_MAPSCREEN_USERCMD_PARA:
			{
				stAllMapScreenUserCmd *ptCmd = (stAllMapScreenUserCmd*)ptNull;
				switch(ptCmd->mdih.type)
				{
					case MAPDATATYPE_NPC:
						{
							for(MapEntry_interator iter = map_npc.begin() ; iter != map_npc.end() ; iter ++)
							{
								zPos pos;
								pos.x = iter->second.x;
								pos.y = iter->second.x;
								clearBlock(pos, TILE_ENTRY_BLOCK);	
							}
							map_npc.clear();
							for(int i = 0 ; i < ptCmd->mdih.size ; i ++)
							{
								stNpcPosition npc;
								bcopy(&ptCmd->psi[i] , &npc , sizeof(npc));
								if(npc.dwTempID < 100000 || npc.dwTempID > 101000)
								{
									map_npc.insert(MapEntry_value_type(npc.dwTempID , npc));
								}
								zPos pos;
								pos.x=npc.x;
								pos.y=npc.y;
								setBlock(pos , TILE_ENTRY_BLOCK);	
							}
							//Zebra::logger->debug("用户%u收到地图npc数量:%u",charbase.data.dwUserTempID ,ptCmd->mdih.size);
						}
						break;
					case MAPDATATYPE_USER:
						{
							for(MapEntry_interator iter = map_user.begin() ; iter != map_user.end() ; iter ++)
							{
								zPos pos;
								pos.x = iter->second.x;
								pos.y = iter->second.x;
								clearBlock(pos, TILE_ENTRY_BLOCK);	
							}
							map_user.clear();
							for(int i = 0 ; i < ptCmd->mdih.size ; i ++)
							{
								stNpcPosition user;
								bcopy(&ptCmd->psi[i] , &user , sizeof(user));
								map_user.insert(MapEntry_value_type(user.dwTempID , user));
								zPos pos;
								pos.x=user.x;
								pos.y=user.y;
								setBlock(pos , TILE_ENTRY_BLOCK);	
							}
							//Zebra::logger->debug("收到地图user数量:%u" , ptCmd->mdih.size);
						}
						break;
				}
			}
			break;
		case REMOVEMAPNPC_MAPSCREEN_USERCMD_PARA:
			{
				stRemoveMapNpcMapScreenUserCmd *ptCmd = (stRemoveMapNpcMapScreenUserCmd*)ptNull;
				map_npc.erase(ptCmd->dwMapNpcDataPosition);
			}
			break;
		case ADDMAPNPC_MAPSCREEN_USERCMD_PARA:
			{
				stAddMapNpcMapScreenUserCmd *ptCmd = (stAddMapNpcMapScreenUserCmd*)ptNull;
				if(isset_state(ptCmd->data.byState , Cmd::USTATE_HIDE))
				{
					/*
					MapEntry_interator iter = map_npc.find(ptCmd->data.dwMapNpcDataPosition);
					if(iter != map_npc.end())
					{
						zPos p;
						p.x = iter->second.x;
						p.y = iter->second.y;
						clearBlock(p, TILE_ENTRY_BLOCK);	
					}
					map_npc.erase(ptCmd->data.dwMapNpcDataPosition);
					Zebra::logger->debug("npc死亡删除%u" , ptCmd->data.dwMapNpcDataPosition);
					// */
				}
				else
				{
					MapEntry_interator iter = map_npc.find(ptCmd->data.dwMapNpcDataPosition);
					if(iter == map_npc.end())
					{
						stNpcPosition npc;
						bzero(&npc , sizeof(npc));
						npc.dwTempID = ptCmd->data.dwMapNpcDataPosition;
						if(iter->second.dwTempID < 100000 || iter->second.dwTempID > 101000)
						{
							map_npc.insert(MapEntry_value_type(npc.dwTempID , npc));
						}
						//Zebra::logger->debug("地图上未找到npc%u",ptCmd->data.dwMapNpcDataPosition);
					}
					else
					{
						zPos p;
						p.x = iter->second.x;
						p.y = iter->second.y;
						clearBlock(p, TILE_ENTRY_BLOCK);	
						iter->second.byDir = ptCmd->data.byDirect;
						const int walk_adjust[9][2]
							= { {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, 0} };
						iter->second.x += walk_adjust[ptCmd->data.byDirect][0];
						iter->second.y += walk_adjust[ptCmd->data.byDirect][1];
						p.x = iter->second.x;
						p.y = iter->second.y;
						setBlock(p, TILE_ENTRY_BLOCK);	
					}
				}
			}
			break;
		case REMOVEUSER_MAPSCREEN_USERCMD_PARA:
			{
				stRemoveUserMapScreenUserCmd *ptCmd = (stRemoveUserMapScreenUserCmd*)ptNull;
				map_user.erase(ptCmd->dwUserTempID);
			}
			break;
		case ADDUSER_MAPSCREEN_USERCMD_PARA:
			{
				stAddUserMapScreenUserCmd *ptCmd = (stAddUserMapScreenUserCmd*)ptNull;
					MapEntry_interator iter = map_user.find(ptCmd->data.dwUserTempID);
					if(iter == map_user.end())
					{
						stNpcPosition user;
						bzero(&user , sizeof(user));
						user.dwTempID = ptCmd->data.dwUserTempID;
						map_user.insert(MapEntry_value_type(user.dwTempID , user));
					}
					else
					{
					}
			}
			break;
		case MAPDATA_MAPSCREEN_USERCMD_PARA:
			{
				using namespace Cmd;
				stMapDataMapScreenUserCmd *ptCmd = (stMapDataMapScreenUserCmd*)ptNull; 
				MapData_ItemHeader *head=&ptCmd->mdih;
				for(;;)
				{
					switch(head->type)
					{
						case MAPDATATYPE_NPC:
							{
								t_MapNpcDataPos *cmd = (t_MapNpcDataPos *)(head + 1);
								for(int i=0; i < head->size ; i++)
								{
									if(isset_state(cmd->byState , Cmd::USTATE_HIDE))
									{
									}
									else
									{
										MapEntry_interator iter = map_npc.find(cmd->dwMapNpcDataPosition);
										if(iter == map_npc.end())
										{
											stNpcPosition npc;
											bzero(&npc , sizeof(npc));
											npc.dwTempID = cmd->dwMapNpcDataPosition;
											if(iter->second.dwTempID < 100000 || iter->second.dwTempID > 101000)
											{
												map_npc.insert(MapEntry_value_type(npc.dwTempID , npc));
											}
											//Zebra::logger->debug("地图上未找到npc%u",cmd->dwMapNpcDataPosition);
										}
										else
										{
											zPos p;
											p.x = iter->second.x;
											p.y = iter->second.y;
											clearBlock(p, TILE_ENTRY_BLOCK);	
											iter->second.byDir = cmd->byDirect;
											const int walk_adjust[9][2]
												= { {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, 0} };
											iter->second.x += walk_adjust[cmd->byDirect][0];
											iter->second.y += walk_adjust[cmd->byDirect][1];
											p.x = iter->second.x;
											p.y = iter->second.y;
											setBlock(p, TILE_ENTRY_BLOCK);	
										}
									}
								}
								head = (MapData_ItemHeader*)((char*)head + sizeof(MapData_ItemHeader) + head->size * sizeof(t_MapNpcDataPos));
							}
							break;
						case MAPDATATYPE_USER:
							{
								t_MapUserDataPos *cmd = (t_MapUserDataPos *)(head + 1);
								for(int i=0; i < head->size ; i++)
								{
									MapEntry_interator iter = map_user.find(cmd->dwUserTempID);
									if(iter == map_user.end())
									{
										stNpcPosition user;
										bzero(&user , sizeof(user));
										user.dwTempID = cmd->dwUserTempID;
										map_user.insert(MapEntry_value_type(user.dwTempID , user));
									}
									else
									{
									}
								}
								//head = head + sizeof(MapData_ItemHeader) + head->size * sizeof(t_MapUserDataPos);
								head = (MapData_ItemHeader*)((char*)head + sizeof(MapData_ItemHeader) + head->size * sizeof(t_MapUserDataPos));
							}
							break;
						case MAPDATATYPE_PET:
							{
								head = (MapData_ItemHeader*)((char*)head + sizeof(MapData_ItemHeader) + head->size * sizeof(t_MapPetData));
							}
						default:
							break;
					}
					if(!head->size)
					{
						break;
					}
				}
			}
			break;
		default:
			break;

	}
	return true;
}
bool zebraClient::msgParse_data(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen)
{
	using namespace Cmd;
	switch(ptNull->byParam)
	{
		case MERGE_VERSION_CHECK_USERCMD_PARA:
			{
				return true;
			}
			break;
		case MAIN_USER_DATA_USERCMD_PARA:
		{
			stMainUserDataUserCmd *ptCmd = (stMainUserDataUserCmd *)ptNull;
			bcopy(ptCmd , &charbase , sizeof(stMainUserDataUserCmd));
			dwUserTempID = ptCmd->data.dwUserTempID;
			return true;
		}
		break;
		case ENDOFINITDATA_DATA_USERCMD_PARA:
			{
				Zebra::logger->debug("接收初始数据成功");
				waitingRoleLogon = true;
				moveReturned = true;
				lastMoveTime.now();
				lastMoveTime.addDelay(2000);
				death = false;
				lastReliveTime.now();
				lastReliveTime.addDelay(1000);
				/*
				Cmd::stAddSkillPointPropertyUserCmd ret;
				sendCmd(&ret , sizeof(ret));
				for(int i = 1 ; i <= (int)skill_table[getType(charInfo[0].type)][0]; i ++)
				{
					Cmd::stAddSkillPointPropertyUserCmd ret;
					ret.dwSkillID = skill_table[getType(charInfo[0].type)][i];
					sendCmd(&ret , sizeof(ret));
				}
				// */
				return true;
			}
			break;
		case MAPSCREENSIZE_DATA_USERCMD_PARA:
			{
				stMapScreenSizeDataUserCmd *ptCmd = (stMapScreenSizeDataUserCmd*)ptNull;
				for(int i = 0 ; i < ptCmd->npc_count ; i ++)
				{
					zPos pos;
					pos.x = ptCmd->npc_list[i].x;
					pos.y = ptCmd->npc_list[i].y;
					function_npc_pos.push_back(pos);
				}
			}
			break;
	}
	return false;
}
bool zebraClient::msgParse_move(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen)
{
	using namespace Cmd;
	switch(ptNull->byParam)
	{
		case NPCMOVE_MOVE_USERCMD_PARA:
			{
				stNpcMoveMoveUserCmd *ptCmd = (stNpcMoveMoveUserCmd *)ptNull;
					MapEntry_interator iter = map_npc.find(ptCmd->dwNpcTempID);
					if(iter != map_npc.end())
					{
						zPos pos;
						pos.x=iter->second.x;
						pos.y=iter->second.y;
						clearBlock(pos , TILE_ENTRY_BLOCK);	
						iter->second.x = ptCmd->x;
						iter->second.y = ptCmd->y;
						iter->second.byDir = ptCmd->byDirect;
						pos.x=ptCmd->x;
						pos.y=ptCmd->y;
						setBlock(pos , TILE_ENTRY_BLOCK);	
					}
					else
					{
						stNpcPosition npc;
						npc.x = ptCmd->x;
						npc.y = ptCmd->y;
						npc.byDir = ptCmd->byDirect;
						npc.dwTempID = ptCmd->dwNpcTempID;
						zPos pos;
						pos.x=ptCmd->x;
						pos.y=ptCmd->y;
						setBlock(pos , TILE_ENTRY_BLOCK);	
						map_npc.insert(MapEntry_value_type(npc.dwTempID , npc));
					}

			}
			break;
		case USERMOVE_MOVE_USERCMD_PARA:
			{
				stUserMoveMoveUserCmd *ptCmd = (stUserMoveMoveUserCmd *)ptNull;
				if(dwUserTempID == ptCmd->dwUserTempID)
				{
					if(ptCmd->x != x && ptCmd->y != y)
					{
						moveto_x = ptCmd->x;
						moveto_y = ptCmd->y;
						if(firstaction)
						{
							firstAction();
						}
					}
					zPos p;
					p.x=x;
					p.y=y;
					clearBlock(p, TILE_ENTRY_BLOCK);	
					//int rand = zMisc::randBetween(1, 5);
					x = ptCmd->x;// + 2 - rand;
					y = ptCmd->y;// + 2 - rand;
					dir = ptCmd->byDirect;
					p.x=x;
					p.y=y;
					setBlock(p, TILE_ENTRY_BLOCK);	
					moveReturned = true;
					lastMoveTime.now();
					if(atoi(ArrayTable::getInstance()->global["move_speed"].c_str()))
					{
						lastMoveTime.addDelay(atoi(ArrayTable::getInstance()->global["move_speed"].c_str()));
					}
					else
					{
						lastMoveTime.addDelay(2000);
					}
					/*
					   Cmd::stAddSkillPointPropertyUserCmd ret;
					   sendCmd(&ret , sizeof(ret));
					   for(int i = 0 ; i < (int)(sizeof(skill_table) / sizeof(DWORD)) ; i ++)
					   {
					   Cmd::stAddSkillPointPropertyUserCmd ret;
					   ret.dwSkillID = skill_table[i];
					   sendCmd(&ret , sizeof(ret));
					   }
					// */
				}
				else
				{
					MapEntry_interator iter = map_user.find(ptCmd->dwUserTempID);
					if(iter != map_user.end())
					{
						zPos pos;
						pos.x=iter->second.x;
						pos.y=iter->second.y;
						clearBlock(pos , TILE_ENTRY_BLOCK);	
						iter->second.x = ptCmd->x;
						iter->second.y = ptCmd->y;
						iter->second.byDir = ptCmd->byDirect;
						pos.x=ptCmd->x;
						pos.y=ptCmd->y;
						setBlock(pos , TILE_ENTRY_BLOCK);	
					}
					else
					{
						stNpcPosition user;
						user.x = ptCmd->x;
						user.y = ptCmd->y;
						user.byDir = ptCmd->byDirect;
						user.dwTempID = ptCmd->dwUserTempID;
						zPos pos;
						pos.x=ptCmd->x;
						pos.y=ptCmd->y;
						setBlock(pos , TILE_ENTRY_BLOCK);	
						map_user.insert(MapEntry_value_type(user.dwTempID , user));
					}

				}
				return true;
			}
			break;
	}
	return false;
}
bool zebraClient::msgParse_magic(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen)
{
	using namespace Cmd;
	switch(ptNull->byParam)
	{
		case NPCDEATH_USERCMD_PARA:
			{
				 Cmd::stNpcDeathUserCmd *ptCmd = (Cmd::stNpcDeathUserCmd*)ptNull;
					MapEntry_interator iter = map_npc.find(ptCmd->dwNpcTempID);
					if(iter != map_npc.end())
					{
						zPos p;
						p.x = iter->second.x;
						p.y = iter->second.y;
						clearBlock(p, TILE_ENTRY_BLOCK);	
					}
					map_npc.erase(ptCmd->dwNpcTempID);
			}
			break;
		case PKMODE_USERCMD_PARA:
			{
				stPKModeUserCmd *ptCmd = (stPKModeUserCmd*)ptNull;
				if(ptCmd->byPKMode == PKMODE_ENTIRE)
				{
					pk_mode_ok = true;
				}
			}
			break;
		case RTMAGIC_POS_USERCMD_PARA:
			{
				stRTMagicPosUserCmd *ptCmd = (stRTMagicPosUserCmd*)ptNull;
				switch(ptCmd->byTarget)
				{
					case Cmd::MAPDATATYPE_NPC:
						{
							MapEntry_interator iter = map_npc.find(ptCmd->pos.dwTempID);
							if(iter != map_npc.end())
							{
								zPos pos;
								pos.x=iter->second.x;
								pos.y=iter->second.y;
								clearBlock(pos , TILE_ENTRY_BLOCK);	
								iter->second.x = ptCmd->pos.x;
								iter->second.y = ptCmd->pos.y;
								iter->second.byDir = ptCmd->pos.byDir;
								pos.x=ptCmd->pos.x;
								pos.y=ptCmd->pos.y;
								setBlock(pos , TILE_ENTRY_BLOCK);	
							}
						}
						break;
					case Cmd::MAPDATATYPE_USER:
						{
							if(ptCmd->pos.dwTempID == charbase.data.dwUserTempID)
							{
								x = ptCmd->pos.x;
								y = ptCmd->pos.y;
							}
							else
							{
							MapEntry_interator iter = map_user.find(ptCmd->pos.dwTempID);
							if(iter != map_user.end())
							{
								zPos pos;
								pos.x=iter->second.x;
								pos.y=iter->second.y;
								clearBlock(pos , TILE_ENTRY_BLOCK);	
								iter->second.x = ptCmd->pos.x;
								iter->second.y = ptCmd->pos.y;
								iter->second.byDir = ptCmd->pos.byDir;
								pos.x=ptCmd->pos.x;
								pos.y=ptCmd->pos.y;
								setBlock(pos , TILE_ENTRY_BLOCK);	
							}
							}
						}
						break;
						// */
				}
			}
			break;
	}
	return true;
}
bool zebraClient::msgParse_chat(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen)
{
	using namespace Cmd;
	switch(ptNull->byParam)
	{
		case REQUEST_TEAM_USERCMD_PARA:
			{
				stRequestTeamUserCmd *ptCmd = (stRequestTeamUserCmd*)ptNull;
				stAnswerTeamUserCmd ret;
				ret.dwRequestUserID = ptCmd->dwAnswerUserID;
				ret.dwAnswerUserID = dwUserTempID;
				ret.byAgree = true;
				sendCmd(&ret , sizeof(ret));
			}
			break;
	}
	return true;
}

bool zebraClient::msgParse_relive(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen)
{
	using namespace Cmd;
	switch(ptNull->byParam)
	{
		case MAINUSERDEATH_RELIVE_USERCMD_PARA:
			{
				stMainUserDeathReliveUserCmd *ptCmd = (stMainUserDeathReliveUserCmd *)ptNull;
				if (ptCmd->dwUserTempID == dwUserTempID)
				{
					Zebra::logger->debug("主角死亡");
					death = true;
					lastReliveTime.now();
					lastReliveTime.addDelay(1000);
				}
				return true;
			}
			break;
		case MAINUSERRELIVE_RELIVE_USERCMD_PARA:
			{
				stMainUserReliveReliveUserCmd *ptCmd = (stMainUserReliveReliveUserCmd *)ptNull;
				if (ptCmd->dwUserTempID == dwUserTempID)
				{
					Zebra::logger->debug("主角复活成功");
					death = false;
					moveReturned = true;
					moveto_x = 0;
					moveto_y = 0;
					lastMoveTime.now();
					lastMoveTime.addDelay(2000);
					operation(0);
				}
				return true;
			}
			break;
	}
	return false;
}

bool zebraClient::versionCmd()
{
	using namespace Cmd;	
	stUserVerifyVerCmd tCmd;
	tCmd.version = GAME_VERSION;
	return sendCmd(&tCmd, sizeof(tCmd));
}

bool zebraClient::createAcc(const char *name, const char *passwd)
{
	using namespace Cmd;
	stAccountLogonUserCmd tCmd;
	strncpy(tCmd.strName, name, sizeof(tCmd.strName));
	strncpy(tCmd.strPassword, passwd, sizeof(tCmd.strPassword));
	//图形验证码忽略
	unsigned char pstrCmd[zSocket::MAX_DATASIZE];
	int nCmdLen = pSocket->recvToCmd(pstrCmd, zSocket::MAX_DATASIZE, true);
	if (!sendCmd(&tCmd, sizeof(tCmd)))
		return false;

	stServerReturnLoginFailedCmd *ptCmd = (stServerReturnLoginFailedCmd *)pstrCmd;
	nCmdLen = pSocket->recvToCmd(pstrCmd, zSocket::MAX_DATASIZE, true);
	if (nCmdLen > 0
			&& LOGON_USERCMD == ptCmd->byCmd
			&& SERVER_RETURN_LOGIN_FAILED == ptCmd->byParam
			&& LOGON_RETURN_ACCOUNTSUCCESS == ptCmd->byReturnCode) 
	{
		return true;
	}

	return false;
}

bool zebraClient::loginGatewayServer()
{
	using namespace Cmd;

	stPasswdLogonUserCmd tCmd;
	tCmd.loginTempID = loginTempID;
	tCmd.dwUserID = accid;
	if (!pSocket->sendCmd(&tCmd, sizeof(tCmd)))
		return false;

	pSocket->setEncMethod(CEncrypt::ENCDEC_DES);
	pSocket->set_key_des(&key_des);
	//Zebra::logger->debug("设置密钥%u, %u, %u, %u, %u, %u, %u, %u 位置：", key_des[0], key_des[1], key_des[2], key_des[3], key_des[4], key_des[5], key_des[6], key_des[7]);
	waitingCharInfo = false;
	roleNotExists = false;
	do {
		unsigned char pstrCmd[zSocket::MAX_DATASIZE];
		int nCmdLen = pSocket->recvToCmd(pstrCmd, zSocket::MAX_DATASIZE, true);
		if (nCmdLen < 0)
			break;
		else if (nCmdLen > 0)
			if (!msgParse((const Cmd::t_NullCmd *)pstrCmd, nCmdLen))
				break;
		// */
		//::usleep(10000);
	} while(!(waitingCharInfo | roleNotExists));
	return (waitingCharInfo | roleNotExists);
}


bool zebraClient::changePasswd(const char *name, const char *passwd, const char *newPasswd)
{
	using namespace Cmd;
	stPasswordLogonUserCmd tCmd;
	strncpy(tCmd.strName, name, sizeof(tCmd.strName));
	strncpy(tCmd.strPassword, passwd, sizeof(tCmd.strPassword));
	strncpy(tCmd.strNewPassword, newPasswd, sizeof(tCmd.strNewPassword));
	if (!sendCmd(&tCmd, sizeof(tCmd)))
		return false;

	unsigned char pstrCmd[zSocket::MAX_DATASIZE];
	stServerReturnLoginFailedCmd *ptCmd = (stServerReturnLoginFailedCmd *)pstrCmd;
	int nCmdLen = pSocket->recvToCmd(pstrCmd, zSocket::MAX_DATASIZE, true);
	if (nCmdLen > 0
			&& LOGON_USERCMD == ptCmd->byCmd
			&& SERVER_RETURN_LOGIN_FAILED == ptCmd->byParam
			&& LOGIN_RETURN_CHANGEPASSWORD == ptCmd->byReturnCode) 
	{
		return true;
	}

	return false;
}

bool zebraClient::createRole(const char *name)
{
	using namespace Cmd;
	stCreateSelectUserCmd cmd;
	bzero(cmd.strUserName , 0);
	strcpy(cmd.strUserName, name);
	cmd.charType = 1;//1 << zMisc::randBetween(0, 7);
	// 调节角色几率
	/*
	if(cmd.charType == 4 || cmd.charType == 16)
	{
		cmd.charType = zMisc::randBetween(0, 2) == 1 ? 1 : cmd.charType;
	}
	cmd.charType = zMisc::randBetween(0, 2) == 1 ? 64 : cmd.charType;
	// */
	cmd.byHairType = 0;
	switch(cmd.charType)
	{
		case 1:
			cmd.byHairType = 1;
			break;
		case 2:
			cmd.byHairType = 5;
			break;
		case 4:
			cmd.byHairType = 1;
			break;
		case 8:
			cmd.byHairType = 2;
			break;
		case 16:
			cmd.byHairType = 1;
			break;
		case 32:
			cmd.byHairType = 3;
			break;
		case 64:
			cmd.byHairType = 1;
			break;
		case 128:
			cmd.byHairType = 4;
			break;

	}
	cmd.byRGB = zMisc::randBetween(0, RAND_MAX);
	/*
	stRequestSelectUserCmd req; 
	if (!sendCmd(&req, sizeof(req)))
		return false;
	do {
		unsigned char pstrCmd[zSocket::MAX_DATASIZE];
		int nCmdLen = pSocket->recvToCmd(pstrCmd, zSocket::MAX_DATASIZE, true);
		if (nCmdLen < 0)
			break;
		else if (nCmdLen > 0)
			if (!msgParse((const Cmd::t_NullCmd *)pstrCmd, nCmdLen))
				break;
	} while(!createCountry);
	cmd.country = createCountry;
	// */
	cmd.country = 8;//zMisc::randBetween(2 , 5);
	cmd.five = zMisc::randBetween(0 , 4);
	Zebra::logger->debug("注册用户名%s",cmd.strUserName);
	if (!sendCmd(&cmd, sizeof(cmd)))
		return false;

	waitingCharInfo = false;
	do {
		unsigned char pstrCmd[zSocket::MAX_DATASIZE];
		int nCmdLen = pSocket->recvToCmd(pstrCmd, zSocket::MAX_DATASIZE, true);
		if (nCmdLen < 0)
			break;
		else if (nCmdLen > 0)
			if (!msgParse((const Cmd::t_NullCmd *)pstrCmd, nCmdLen))
				break;
	} while(!waitingCharInfo);

	if (waitingCharInfo)
		Zebra::logger->debug("创建角色成功");

	return waitingCharInfo;
}

bool zebraClient::logonRole(const int i)
{
	using namespace Cmd;

			
	waitingRoleLogon = false;
	if (i < Cmd::MAX_CHARINFO
			&& charInfo[i].id!=0
			&& charInfo[i].id!=(DWORD)-1)
	{
		stLoginSelectUserCmd cmd;
		cmd.charNo = i;
		role_num = i;
		if (!sendCmd(&cmd, sizeof(cmd)))
			return false;

		do {
			unsigned char pstrCmd[zSocket::MAX_DATASIZE];
			int nCmdLen = pSocket->recvToCmd(pstrCmd, zSocket::MAX_DATASIZE, true);
			if (nCmdLen < 0)
				break;
			else if (nCmdLen > 0)
				if (!msgParse((const Cmd::t_NullCmd *)pstrCmd, nCmdLen))
					;//break;
		} while(!waitingRoleLogon);
	}

	/*
	Cmd::stAddSkillPointPropertyUserCmd ret;
	ret.dwSkillID = 254;
	sendCmd(&ret , sizeof(ret));
	for(int i = 0 ; i < (int)(sizeof(skill_table) / sizeof(DWORD)) ; i ++)
	{
		Cmd::stAddSkillPointPropertyUserCmd ret;
		ret.dwSkillID = skill_table[i];
		sendCmd(&ret , sizeof(ret));
	}
	// */
	Cmd::stPKModeUserCmd stp;
	stp.byPKMode = PKMODE_ENTIRE;
	sendCmd(&stp , sizeof(stp));
	return waitingRoleLogon;
}

bool zebraClient::init(DWORD acc ,DWORD tempid , DES_cblock key)
{
	using namespace Cmd;

	accid=acc;
	loginTempID=tempid;
	bcopy(key , key_des , sizeof(DES_cblock));
	if (!loginGatewayServer())
		return false;
	if ("true" == Zebra::global["cacc"])
	{
		createAcc(user.c_str(), Zebra::global["passwd"].c_str());
		return true;
	}
	else if ("true" == Zebra::global["change"])
	{
		changePasswd(user.c_str(), Zebra::global["passwd"].c_str(), Zebra::global["newPasswd"].c_str());
		return true;
	}
	else if ("true" == Zebra::global["role"])
	{
		stDeleteSelectUserCmd dsu;
		dsu.charNo=0;
		if (!sendCmd(&dsu, sizeof(dsu)))
		{
			return  false;
		}
		dsu.charNo=1;
		if (!sendCmd(&dsu, sizeof(dsu)))
		{
			return  false;
		}
		char name[32];
		bzero(name ,32);
		memcpy(name, user.c_str() , 5);
		std::string role ;
		role =  name;
		role = role + "1";
		createRole(role.c_str());
		::usleep(100 * 300);
		return true;
		/*
		if(createRole(role.c_str()))
		{
			::usleep(1000 * 10);
			role =  name;
			role = role + "2";

			return createRole(role.c_str());
		}
		return  false;
		// */
	}
	else 
	{
	}
	return logonRole(0);
	/*
	if (!connect())
		return false;
	return true;
	// */
}
void zebraClient::addToContainer()
{
	ZebraClientManager::getInstance()->add(this); 
}
void zebraClient::removeFromContainer()
{
	ZebraClientManager::getInstance()->remove(this); 
}
bool zebraClient::timeAction()
{
	bool need_action = true;
	zRTime currentTime;
	//如果不在小退状态
	if(isPlayState)
	{
		if(_one_sec(currentTime))
		{
			need_action = one_sec_action();
			action();
		}
		if(_five_sec(currentTime))
		{
			if(need_action)
				need_action = five_sec_action();
			else
				five_sec_action();
		}
		if(_ten_sec(currentTime))
			ten_sec_action();
		if(_one_min(currentTime))
			one_min_action();
		if(need_action)
		{
			//action();
		}
	}
	action();
	return true;
}
/*
bool zebraClient::main()
{
	unsigned char pstrCmd[zSocket::MAX_DATASIZE];
	int nCmdLen;

	pSocket->setReadTimeout(1000);
	nCmdLen = pSocket->recvToCmd(pstrCmd, zSocket::MAX_DATASIZE, false);
	if (nCmdLen > 0) 
	{
		Cmd::t_NullCmd *ptNullCmd = (Cmd::t_NullCmd *)pstrCmd;
		if (Cmd::CMD_NULL == ptNullCmd->cmd
				&& Cmd::PARA_NULL == ptNullCmd->para)
		{
			if (!pSocket->sendCmd(pstrCmd, nCmdLen))
			{
				return false;
			}
		}
		else
			msgParse(ptNullCmd, nCmdLen);
	}
	else if (-1 == nCmdLen)
	{
		return false;
	}

	bool need_action = true;
	zRTime currentTime;
	//如果不在小退状态
	if(isPlayState)
	{
		if(_one_sec(currentTime))
		{
			need_action = one_sec_action();
			action();
		}
		if(_five_sec(currentTime))
		{
			if(need_action)
				need_action = five_sec_action();
			else
				five_sec_action();
		}
		if(_ten_sec(currentTime))
			ten_sec_action();
		if(_one_min(currentTime))
			one_min_action();
		if(need_action)
		{
			//action();
		}
	}
	return true;
}
// */

std::map<DWORD , DWORD> map_timer;
void zebraClient::operation(DWORD timer)
{
	using namespace Cmd;
	ArrayTable::str_vec v = ArrayTable::getInstance()->getOperation(timer);
	if(timer && !v.empty())
	{
		map_timer[timer] =((map_timer[timer])++)%v.size();
		{
			/*
			   if(!pk_mode_ok)
			   {
			   Cmd::stPKModeUserCmd pkm;
			   pkm.byPKMode = PKMODE_ENTIRE;
			   sendCmd(&pkm , sizeof(pkm));
			   }
			// */
			//Zebra::logger->debug("操作:%s",(*iter).c_str());
			if(strncmp(v[map_timer[timer]].c_str() , "//" , 2) == 0) 
			{
				char buf[256];
				bzero(buf , sizeof(buf));
				strcpy(buf , (v[map_timer[timer]]).c_str()+2);
				gm(buf , 256);
			}
			else if(strcmp((v[map_timer[timer]]).c_str() , "攻击") == 0)
			{
				//查找最近的攻击目标
				Cmd::stNpcPosition target;
				target.dwTempID = 0;
				target.x = 0;
				target.y = 0;
				target.byDir = 0;
				if(!targetid)
				{
					for(MapEntry_interator iter = map_npc.begin() ; iter != map_npc.end() ; iter ++)
					{
						bool isFunctionNpc = false;
						for(VecNpcPos_iterator iter_1 = function_npc_pos.begin() ; iter_1 != function_npc_pos.end() ; iter_1 ++)
						{
							if(iter_1->x == iter->second.x && iter_1->y == iter->second.y)
							{
								isFunctionNpc = true;
								break;
							}
						}
						if(!isFunctionNpc)
						{
							if(!target.dwTempID)
							{
								target = iter->second;
								targetid = iter->first;
								targettype = Cmd::ATTACKTYPE_U2N;
							}
							else if((abs(iter->second.x - x) + abs(iter->second.y - y) < 
										abs(target.x - x) + abs(target.y - y)))
							{
								target = iter->second;
								targetid = iter->first;
								targettype = Cmd::ATTACKTYPE_U2N;
							}
						}
					}
				}
				//int rand = zMisc::randBetween(0 , ArrayTable::getInstance()->getSize() - 1);
				if(target.dwTempID && abs(target.x - x)<=1 && abs(target.y - y)<=1 && targettype == Cmd::ATTACKTYPE_U2N)
				{
					attack(target.dwTempID , target.x , target.y , target.byDir);
				}
			}
			else if(strcmp((v[map_timer[timer]]).c_str() , "pk") == 0)
			{
				Cmd::stNpcPosition target;
				target.dwTempID = 0;
				target.x = 0;
				target.y = 0;
				target.byDir = 0;
				if(!targetid)
				{
					for(MapEntry_interator iter = map_user.begin() ; iter != map_user.end() ; iter ++)
					{
						if(iter->first != dwUserTempID)
						{
							if(!target.dwTempID)
							{
								target = iter->second;
								targetid = iter->first;
								targettype = Cmd::ATTACKTYPE_U2U;
							}
							else if((abs(iter->second.x - x) + abs(iter->second.y - y) < 
										abs(target.x - x) + abs(target.y - y)))
							{
								target = iter->second;
								targetid = iter->first;
								targettype = Cmd::ATTACKTYPE_U2U;
							}
						}
					}
				}
				if(target.dwTempID && abs(target.x - x)<=1 && abs(target.y - y)<=1 && targettype == Cmd::ATTACKTYPE_U2U)
				{
					attack(target.dwTempID , target.x , target.y , target.byDir);
					Zebra::logger->debug("pk(%u , %u)" , dwUserTempID , target.dwTempID);
				}
				/*
				   sta.dwUserTempID = dwUserTempID;
				   sta.wdMagicType = ArrayTable::getInstance()->get(rand);
				   sta.byDirect = dir;
				   sta.xDes = x;
				   sta.yDes = y;
				   sta.byAttackType = Cmd::ATTACKTYPE_U2P;
				   sta.byAction = Cmd::Ani_Null;
				// */
			}
			else if(strcmp((v[map_timer[timer]]).c_str() , "补体力") == 0) 
			{
				char buf[256];
				bzero(buf , sizeof(buf));
				strcpy(buf , "//value sp=1000");
				gm(buf , 256);
			}
			else if(strcmp((v[map_timer[timer]]).c_str() , "补魔法") == 0) 
			{
				char buf[256];
				bzero(buf , sizeof(buf));
				strcpy(buf , "//value mp=1000");
				gm(buf , 256);
			}
			else if(strcmp((v[map_timer[timer]]).c_str() , "恢复耐久") == 0) 
			{
				char buf[256];
				bzero(buf , sizeof(buf));
				strcpy(buf , "//setvalue resume=100");
				gm(buf , 256);
			}
			else if(strcmp((v[map_timer[timer]]).c_str() , "取消技能冷却") == 0) 
			{
				char buf[256];
				bzero(buf , sizeof(buf));
				strcpy(buf , "//setvalue cooling=0");
				gm(buf , 256);
			}
			else if(strcmp((v[map_timer[timer]]).c_str() , "小退") == 0) 
			{
				stBackSelectUserCmd bsu;
				sendCmd(&bsu , sizeof(bsu));
				Zebra::logger->debug("小退");
				isPlayState = false;
				if(atoi(ArrayTable::getInstance()->global["login_sleep"].c_str()))
				{
					sleep(atoi(ArrayTable::getInstance()->global["login_sleep"].c_str()));
				}
				else
				{
					sleep(2);
				}
				if(strcmp(ArrayTable::getInstance()->global["random_role"].c_str() , "true") == 0)
				{
					logonRole(zMisc::randBetween(0 , 1));
				}
				else
				{
					logonRole(0);
				}
				isPlayState = true;
			}
			else if(strcmp((v[map_timer[timer]]).c_str() , "退出") == 0) 
			{
				zebraclientService::getInstance().Terminate();
				if(strcmp(ArrayTable::getInstance()->global["loop_run"].c_str() , "true") == 0)
				{
					zebraclientService::getInstance().setLoopRun();
				}
			}
			else if(strcmp((v[map_timer[timer]]).c_str() , "解散队伍") == 0) 
			{
				stRemoveTeamUserCmd rtu;
				sendCmd(&rtu , sizeof(rtu));
				//Zebra::logger->debug("队伍解散");
			}
			else if(strcmp((v[map_timer[timer]]).c_str() , "招收队员") == 0) 
			{
				stRequestTeamUserCmd rtu;
				rtu.dwTeamMode = 0;//zMisc::randBetween(0,1); 
				for(MapEntry_interator iter = map_user.begin() ; iter != map_user.end() ; iter ++)
				{
					rtu.dwAnswerUserID = iter->first;
					sendCmd(&rtu , sizeof(rtu));
				}
			}
			else if(strcmp((v[map_timer[timer]]).c_str() , "召怪") == 0) 
			{
				if(map_npc.size() < 5)
				{
					char buf[256];
					bzero(buf , sizeof(buf));
					strcpy(buf , "//summon id=44 num=10");
					gm(buf , 256);
				}
				else
				{
					//Zebra::logger->debug("有足够的怪物,无需召怪");
				}
			}
			else if(strcmp((v[map_timer[timer]]).c_str() , "分散") == 0) 
			{
				char buf[256];
				bzero(buf , sizeof(buf));
				zPos p;
				do
				{
					p.x = zMisc::randBetween(0, zebraClient::sceneWH.x);
					p.y = zMisc::randBetween(0, zebraClient::sceneWH.y);
				}while(checkBlock(p , TILE_ENTRY_BLOCK));
				sprintf(buf , "//goto %d %d" , p.x , p.y);
				gm(buf , 256);
			}
		}
	}
	else
	{
		for(ArrayTable::str_vec_iterator iter = v.begin() ; iter != v.end() ; iter ++)
		{
			/*
			   if(!pk_mode_ok)
			   {
			   Cmd::stPKModeUserCmd pkm;
			   pkm.byPKMode = PKMODE_ENTIRE;
			   sendCmd(&pkm , sizeof(pkm));
			   }
			// */
			//Zebra::logger->debug("操作:%s",(*iter).c_str());
			if(strncmp((*iter).c_str() , "//" , 2) == 0) 
			{
				char buf[256];
				bzero(buf , sizeof(buf));
				strcpy(buf , (*iter).c_str()+2);
				gm(buf , 256);
			}
			else if(strcmp((*iter).c_str() , "攻击") == 0)
			{
				//查找最近的攻击目标
				Cmd::stNpcPosition target;
				target.dwTempID = 0;
				target.x = 0;
				target.y = 0;
				target.byDir = 0;
				if(!targetid)
				{
					for(MapEntry_interator iter = map_npc.begin() ; iter != map_npc.end() ; iter ++)
					{
						bool isFunctionNpc = false;
						for(VecNpcPos_iterator iter_1 = function_npc_pos.begin() ; iter_1 != function_npc_pos.end() ; iter_1 ++)
						{
							if(iter_1->x == iter->second.x && iter_1->y == iter->second.y)
							{
								isFunctionNpc = true;
								break;
							}
						}
						if(!isFunctionNpc)
						{
							if(!target.dwTempID)
							{
								target = iter->second;
								targetid = iter->first;
								targettype = Cmd::ATTACKTYPE_U2N;
							}
							else if((abs(iter->second.x - x) + abs(iter->second.y - y) < 
										abs(target.x - x) + abs(target.y - y)))
							{
								target = iter->second;
								targetid = iter->first;
								targettype = Cmd::ATTACKTYPE_U2N;
							}
						}
					}
				}
				//int rand = zMisc::randBetween(0 , ArrayTable::getInstance()->getSize() - 1);
				if(target.dwTempID && abs(target.x - x)<=1 && abs(target.y - y)<=1 && targettype == Cmd::ATTACKTYPE_U2N)
				{
					attack(target.dwTempID , target.x , target.y , target.byDir);
				}
			}
			else if(strcmp((*iter).c_str() , "pk") == 0)
			{
				Cmd::stNpcPosition target;
				target.dwTempID = 0;
				target.x = 0;
				target.y = 0;
				target.byDir = 0;
				if(!targetid)
				{
					for(MapEntry_interator iter = map_user.begin() ; iter != map_user.end() ; iter ++)
					{
						if(iter->first != dwUserTempID)
						{
							if(!target.dwTempID)
							{
								target = iter->second;
								targetid = iter->first;
								targettype = Cmd::ATTACKTYPE_U2U;
							}
							else if((abs(iter->second.x - x) + abs(iter->second.y - y) < 
										abs(target.x - x) + abs(target.y - y)))
							{
								target = iter->second;
								targetid = iter->first;
								targettype = Cmd::ATTACKTYPE_U2U;
							}
						}
					}
				}
				if(target.dwTempID && abs(target.x - x)<=1 && abs(target.y - y)<=1 && targettype == Cmd::ATTACKTYPE_U2U)
				{
					attack(target.dwTempID , target.x , target.y , target.byDir);
					Zebra::logger->debug("pk(%u , %u)" , dwUserTempID , target.dwTempID);
				}
				/*
				   sta.dwUserTempID = dwUserTempID;
				   sta.wdMagicType = ArrayTable::getInstance()->get(rand);
				   sta.byDirect = dir;
				   sta.xDes = x;
				   sta.yDes = y;
				   sta.byAttackType = Cmd::ATTACKTYPE_U2P;
				   sta.byAction = Cmd::Ani_Null;
				// */
			}
			else if(strcmp((*iter).c_str() , "补体力") == 0) 
			{
				char buf[256];
				bzero(buf , sizeof(buf));
				strcpy(buf , "//value sp=1000");
				gm(buf , 256);
			}
			else if(strcmp((*iter).c_str() , "补魔法") == 0) 
			{
				char buf[256];
				bzero(buf , sizeof(buf));
				strcpy(buf , "//value mp=1000");
				gm(buf , 256);
			}
			else if(strcmp((*iter).c_str() , "恢复耐久") == 0) 
			{
				char buf[256];
				bzero(buf , sizeof(buf));
				strcpy(buf , "//setvalue resume=100");
				gm(buf , 256);
			}
			else if(strcmp((*iter).c_str() , "取消技能冷却") == 0) 
			{
				char buf[256];
				bzero(buf , sizeof(buf));
				strcpy(buf , "//setvalue cooling=0");
				gm(buf , 256);
			}
			else if(strcmp((*iter).c_str() , "小退") == 0) 
			{
				stBackSelectUserCmd bsu;
				sendCmd(&bsu , sizeof(bsu));
				Zebra::logger->debug("小退");
				isPlayState = false;
				if(atoi(ArrayTable::getInstance()->global["login_sleep"].c_str()))
				{
					sleep(atoi(ArrayTable::getInstance()->global["login_sleep"].c_str()));
				}
				else
				{
					sleep(2);
				}
				if(strcmp(ArrayTable::getInstance()->global["random_role"].c_str() , "true") == 0)
				{
					logonRole(zMisc::randBetween(0 , 1));
				}
				else
				{
					logonRole(0);
				}
				isPlayState = true;
			}
			else if(strcmp((*iter).c_str() , "退出") == 0) 
			{
				zebraclientService::getInstance().Terminate();
				if(strcmp(ArrayTable::getInstance()->global["loop_run"].c_str() , "true") == 0)
				{
					zebraclientService::getInstance().setLoopRun();
				}
			}
			else if(strcmp((*iter).c_str() , "解散队伍") == 0) 
			{
				stRemoveTeamUserCmd rtu;
				sendCmd(&rtu , sizeof(rtu));
				//Zebra::logger->debug("队伍解散");
			}
			else if(strcmp((*iter).c_str() , "招收队员") == 0) 
			{
				stRequestTeamUserCmd rtu;
				for(MapEntry_interator iter = map_user.begin() ; iter != map_user.end() ; iter ++)
				{
					rtu.dwAnswerUserID = iter->first;
					sendCmd(&rtu , sizeof(rtu));
				}
			}
			else if(strcmp((*iter).c_str() , "召怪") == 0) 
			{
				if(map_npc.size() < 5)
				{
					char buf[256];
					bzero(buf , sizeof(buf));
					strcpy(buf , "//summon id=44 num=10");
					gm(buf , 256);
				}
				else
				{
					//Zebra::logger->debug("有足够的怪物,无需召怪");
				}
			}
			else if(strcmp((*iter).c_str() , "分散") == 0) 
			{
				char buf[256];
				bzero(buf , sizeof(buf));
				zPos p;
				do
				{
					p.x = zMisc::randBetween(0, zebraClient::sceneWH.x);
					p.y = zMisc::randBetween(0, zebraClient::sceneWH.y);
				}while(checkBlock(p , TILE_ENTRY_BLOCK));
				sprintf(buf , "//goto %d %d" , p.x , p.y);
				gm(buf , 256);
			}
		}
	}
}
void zebraClient::attack(DWORD tempid , DWORD xx , DWORD yy , BYTE byDir)
{
	//const int walk_adjust[9][2]	= { {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, 0} };
	if(abs(x - xx) == 0 && abs(y - yy) == -1)
	{
		dir = 0;
	}
	else if(abs(x - xx) == 1 && abs(y - yy) == -1)
	{
		dir = 1;
	}
	else if(abs(x - xx) == 1 && abs(y - yy) == 0)
	{
		dir = 2;
	}
	else if(abs(x - xx) == 1 && abs(y - yy) == 1)
	{
		dir = 3;
	}
	else if(abs(x - xx) == 0 && abs(y - yy) == 1)
	{
		dir = 4;
	}
	else if(abs(x - xx) == -1 && abs(y - yy) == 1)
	{
		dir = 5;
	}
	else if(abs(x - xx) == -1 && abs(y - yy) == 0)
	{
		dir = 6;
	}
	else if(abs(x - xx) == -1 && abs(y - yy) == -1)
	{
		dir = 7;
	}
	Cmd::stAttackMagicUserCmd sta;
	sta.dwUserTempID = dwUserTempID;
	sta.dwDefenceTempID = tempid;
	sta.wdMagicType = 343;//ArrayTable::getInstance()->get(rand);
	sta.byDirect = dir;//(byDir + 4)%8;
	//dir = sta.byDirect;
	sta.xDes = xx;
	sta.yDes = yy;
	sta.byAttackType = targettype;
	sta.byAction = Cmd::Ani_Attack3;
	//Zebra::logger->debug("%u对目标进行攻击dwTempID(%u , %u)" ,dwUserTempID , tempid , targettype);
	sendCmd(&sta , sizeof(sta));
}
bool  zebraClient::one_sec_action()
{
	operation(1);
	return true;
}
bool  zebraClient::five_sec_action()
{
	operation(5);
	return true;
}
bool zebraClient::ten_sec_action()
{
	//move(dir,1);
	operation(10);
	return true;
}
bool zebraClient::one_min_action()
{
	operation(60);
	return true;
}
void zebraClient::action()
{
	zRTime currentTime;

	if (!death
			&& moveReturned
			&& currentTime >= lastMoveTime)
	{
		zPos p;
		Cmd::stNpcPosition target;
		target.dwTempID = 0;
		target.x = 0;
		target.y = 0;
		target.byDir = 0;
		if(targetid && targettype == Cmd::ATTACKTYPE_U2N)
		{
			MapEntry_interator iter = map_npc.find(targetid);
			if(iter == map_npc.end())
			{
				Zebra::logger->debug("%u追踪的npc(%u)丢失2" , dwUserTempID , targetid);
				targetid = 0;
				targettype = (DWORD)-1;
			}
			else
			{
				target = iter->second;
				p.x = iter->second.x;
				p.y = iter->second.y;
			}
		}
		if(targetid && targettype == Cmd::ATTACKTYPE_U2U)
		{
			if(targetid != dwUserTempID)
			{
				MapEntry_interator iter = map_user.find(targetid);
				if(iter == map_user.end())
				{
					Zebra::logger->debug("%u追踪的user(%u)丢失2" , dwUserTempID , targetid);
					targetid = 0;
					targettype = (DWORD)-1;
				}
				else
				{
					target = iter->second;
					p.x = iter->second.x;
					p.y = iter->second.y;
				}
			}
		}
		//如果正在攻击目标,无需移动
		if((p.x || p.y)&& abs(p.x - x)<=1 && abs(p.y - y)<=1)
		{
			attack(target.dwTempID , target.x , target.y , target.byDir);
			return;
		}
		if(p.x || p.y)
		{
			zPos src;
			src.x=x;
			src.y=y;
			if(gotoFindPath(src , p))
			{
				//Zebra::logger->debug("寻路成功npd(%u) , 坐标(%u , %u)" , target.dwTempID , target.x , target.y);
				zPos posMe;
				posMe.x = x;
				posMe.y = y;
				int d = getDir(posMe , p);
				move(d , 1);
				return;
			}
			else
			{
				move(zMisc::randBetween(0, 7) , 1);
			}
		}
		else
		{
			move(zMisc::randBetween(0, 7) , 1);
		}
	}

	if (death
			&& currentTime >= lastReliveTime)
	{
		Zebra::logger->debug("主角死亡复活");
		lastReliveTime.now();
		lastReliveTime.addDelay(5000);
		Cmd::stOKReliveUserCmd cmd;
		cmd.dwUserTempID = dwUserTempID;
		sendCmd(&cmd, sizeof(cmd));
	}
}

DWORD zebraClient::getDir(zPos pos_1, zPos pos_2)
{
	DWORD dir=0;
	if(pos_2.x - pos_1.x == (DWORD)0 && pos_2.y - pos_1.y == (DWORD)-1)
	{
		dir = 0;
	}
	else if(pos_2.x - pos_1.x == (DWORD)1 && pos_2.y - pos_1.y == (DWORD)-1)
	{
		dir = 1;
	}
	else if(pos_2.x - pos_1.x == (DWORD)1 && pos_2.y - pos_1.y == (DWORD)0)
	{
		dir = 2;
	}
	else if(pos_2.x - pos_1.x == (DWORD)1 && pos_2.y - pos_1.y == (DWORD)1)
	{
		dir = 3;
	}
	else if(pos_2.x - pos_1.x == (DWORD)0 && pos_2.y - pos_1.y == (DWORD)1)
	{
		dir = 4;
	}
	else if(pos_2.x - pos_1.x == (DWORD)-1 && pos_2.y - pos_1.y == (DWORD)1)
	{
		dir = 5;
	}
	else if(pos_2.x - pos_1.x == (DWORD)-1 && pos_2.y - pos_1.y == (DWORD)0)
	{
		dir = 6;
	}
	else if(pos_2.x - pos_1.x == (DWORD)-1 && pos_2.y - pos_1.y == (DWORD)-1)
	{
		dir = 7;
	}
	return dir;
}
bool zebraClient::move(const int direct, const int step)
{
	using namespace Cmd;
	const int walk_adjust[9][2]	= { {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, 0} };
	Cmd::stUserMoveMoveUserCmd cmd;
	cmd.dwUserTempID = dwUserTempID;

	bool canmove = false;
	zPos p;
		zPos posMe;
		posMe.x = x;
		posMe.y = y;
	int d = direct;
	//if(moveto_x != 0 && moveto_y !=0 && moveto_x != x && moveto_y != y)
	if(false)
	{
		if(abs(p.x - x) <= 1 && abs(p.y - y )<= 1) 
		{
			p.x = moveto_x;
			p.y = moveto_y;
			d = getDir(posMe , p);
			canmove = true;
		}
		else
		{
			moveto_x=0;
			moveto_y = 0;
		}
		/*
		if(!locked)
		{
			//找到阻挡点bug,锁血,不要别人攻击
			char buf[256];
			bzero(buf , sizeof(buf));
			sprintf(buf , "//lockvalue name=%s lock=hp" , charInfo[role_num].name);
			gm(buf , 256);
			//不允许任何操作
			ArrayTable::getInstance()->clearOperation();
			locked = true;
		}
		// */
	}
	else
	{
		/*
		//拣东西
		for(int i = 0 ; i < 7; i ++)
		{
			p.x = x + walk_adjust[i][0];
			p.y = y + walk_adjust[i][1];
			if(checkBlock(p , TILE_OBJECT_BLOCK))
			{
				stPickUpItemPropertyUserCmd ppu;
				ppu.x=p.x;
				ppu.y=p.y;
				sendCmd(&ppu , sizeof(ppu));
				//Zebra::logger->debug("拣东西");
			}
		}
		// */
		p.x = x + walk_adjust[direct][0];
		p.y = y + walk_adjust[direct][1];
		if(checkBlock(p , TILE_ENTRY_BLOCK))
		{
			for(int i = 0 ; i < 7; i ++)
			{
				d = (d + i)%8;
				p.x = x + walk_adjust[d][0];
				p.y = y + walk_adjust[d][1];
				if(checkBlock(p , TILE_ENTRY_BLOCK))
				{
					continue;
				}
				else
				{
					d = getDir(posMe , p);
					canmove = true;
					break;
				}
			}
		}
		else
		{
			d = getDir(posMe , p);
			canmove = true;
		}
	}
	if(canmove && moveReturned)
	{
		cmd.byDirect = d; 
		cmd.bySpeed = step;//zMisc::randBetween(1, 2);
		cmd.x = p.x;
		cmd.y = p.y;
		moveto_x = cmd.x;
		moveto_y = cmd.y;
		//cmd.x+=walk_adjust[cmd.byDirect][0]*step;
		//cmd.y+=walk_adjust[cmd.byDirect][1]*step;
		sendCmd(&cmd, sizeof(cmd));
		moveReturned = false;
		//Zebra::logger->debug("%u(%u,%u)移动目标点(%u,%u) , 目标(%u , %u)",dwUserTempID , x , y , p.x , p.y , targettype , targetid);
	}
	return true;
}


bool zebraClient::firstRun()
		{
			if(first)
			{
				first = false;
				return true;
			}
			else
			{
				return false;
			}
		}

/*
void zebraClient::run()
{
	while(!isFinal())
	{
		main();
		usleep(1000);
	}
}
// */
bool zebraClient::msgParse_logon(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen)
{
	using namespace Cmd;
	switch(ptNull->byParam)
	{
		case SERVER_RETURN_COUNTRY_INFO:
			{
				stCountryInfoUserCmd *ptCmd = (stCountryInfoUserCmd*)ptNull; 
				createCountry = ptCmd->countryinfo[0].id;
				return true;
			}
			break;
		case SERVER_RETURN_LOGIN_FAILED:
			{
				stServerReturnLoginFailedCmd *ptCmd = (stServerReturnLoginFailedCmd *)ptNull;
				bool retval = false;
				switch(ptCmd->byReturnCode)
				{
					case LOGIN_RETURN_UNKNOWN:
						Zebra::logger->error("%s:未知错误", __FUNCTION__);
						break;
					case LOGIN_RETURN_VERSIONERROR:
						Zebra::logger->error("%s:版本错误", __FUNCTION__);
						break;
					case LOGIN_RETURN_UUID:
						Zebra::logger->error("%s:UUID登陆方式没有实现", __FUNCTION__);
						break;
					case LOGIN_RETURN_DB:
						Zebra::logger->error("%s:数据库出错", __FUNCTION__);
						break;
					case LOGIN_RETURN_PASSWORDERROR:
						Zebra::logger->error("%s:帐号密码错误", __FUNCTION__);
						break;
					case LOGIN_RETURN_CHANGEPASSWORD:
						Zebra::logger->error("%s:修改密码成功", __FUNCTION__);
						retval = true;
						break;
					case LOGIN_RETURN_IDINUSE:
						Zebra::logger->error("%s:ID正在被使用中", __FUNCTION__);
						break;
					case LOGIN_RETURN_IDINCLOSE:
						Zebra::logger->error("%s:ID被封", __FUNCTION__);
						break;
					case LOGIN_RETURN_GATEWAYNOTAVAILABLE:
						Zebra::logger->error("%s:网关服务器未开", __FUNCTION__);
						break;
					case LOGIN_RETURN_USERMAX:
						Zebra::logger->error("%s:用户满", __FUNCTION__);
						break;
					case LOGIN_RETURN_ACCOUNTEXIST:
						Zebra::logger->error("%s:账号已经存在", __FUNCTION__);
						break;
					case LOGON_RETURN_ACCOUNTSUCCESS:
						Zebra::logger->error("%s:注册账号成功", __FUNCTION__);
						retval = true;
						break;
					case LOGIN_RETURN_CHARNAMEREPEAT:
						Zebra::logger->error("%s:角色名称重复", __FUNCTION__);
						break;
					case LOGIN_RETURN_USERDATANOEXIST:
						Zebra::logger->error("%s:用户档案不存在", __FUNCTION__);
						roleNotExists = true;
						retval = true;
						break;
					case LOGIN_RETURN_USERNAMEREPEAT:
						Zebra::logger->error("%s:用户名重复", __FUNCTION__);
						break;
					case LOGIN_RETURN_TIMEOUT:
						Zebra::logger->error("%s:连接超时", __FUNCTION__);
						break;
					case LOGIN_RETURN_PAYFAILED:
						Zebra::logger->error("%s:计费失败", __FUNCTION__);
						break;
				}
				return retval;
			}
			break;
	}
	return false;
}
int zebraClient::checkRebound()
{
	return 1;
}
