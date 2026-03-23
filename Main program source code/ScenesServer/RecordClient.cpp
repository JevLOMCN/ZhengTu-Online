/**
 * \file
 * \version  $Id: RecordClient.cpp  $
 * \author 
 * \date
 * \brief 定义档案服务器连接客户端
 *
 * 负责与档案服务器交互，存取档案
 * 
 */

#include <unistd.h>
#include <iostream>

#include "zTCPClient.h"
#include "RecordCommand.h"
#include "RecordClient.h"
#include "ScenesServer.h"
#include "Zebra.h"
#include "SceneTask.h"
#include "SessionCommand.h"
#include "SessionClient.h"
#include <zlib.h>
#include "Union.h"
#include "Chat.h"
#include "SceneNpc.h"
#include "SceneTaskManager.h"
#include "MiniCommand.h"
#include "MiniClient.h"

///RecordClient的唯一实例
RecordClient *recordClient = NULL;
/**
 * \brief 解压角色需要保存的数据
 *
 * \param pUser 角色指针
 * \param data 输入：压缩的数据 / 输出：解压后的数据 
 * \param dataSize 输入 数据大小
 * \return 解压后的数据大小
 */
bool uncompressSaveData(SceneUser *pUser , const unsigned char *data ,const DWORD dataSize, unsigned char * petData); 

/**
 * \brief 创建到档案服务器的连接
 *
 * \return 连接是否成功
 */
bool RecordClient::connectToRecordServer()
{
	if (!connect())
	{
		Zebra::logger->error("连接档案服务器失败");
		return false;
	}

	using namespace Cmd::Record;
	t_LoginRecord tCmd;
	tCmd.wdServerID = ScenesService::getInstance().getServerID();
	tCmd.wdServerType = ScenesService::getInstance().getServerType();

	return sendCmd(&tCmd, sizeof(tCmd));
}

/**
 * \brief 重载zTCPBufferClient的run方法
 *
 *
 */
void RecordClient::run()
{
	zTCPBufferClient::run();

	//与档案服务器的连接断开，关闭服务器
	ScenesService::getInstance().Terminate();
	while (sessionClient)
	{
		zThread::msleep(10);
	}
}

/**
 * \brief 解析来自档案服务器的指令
 *
 * \param ptNullCmd 待解析的指令
 * \param nCmdLen 待解析的指令长度
 * \return 解析指令是否成功
 */
bool RecordClient::msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	return MessageQueue::msgParse(ptNullCmd , nCmdLen);
}
bool RecordClient::cmdMsgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	My_FunctionTime_wrapper(__PRETTY_FUNCTION__);
	using namespace Cmd::Record;

	if(ptNullCmd->cmd==CMD_SCENE)
	{
		switch(ptNullCmd->para)
		{
			case PARA_SCENE_USERINFO:
				{
					t_UserInfo_SceneRecord *rev=(t_UserInfo_SceneRecord *)ptNullCmd;
					Scene *scene=SceneManager::getInstance().getSceneByTempID(rev->dwMapTempID);
					SceneUser *pUser=SceneUserManager::getMe().getUserByIDOut(rev->id);
					if(scene)
					{
						//Zebra::logger->debug("得到场景%s",scene->name);
						if(pUser)
						{
							bool dataOk=false;
							//Zebra::logger->trade("读取%ld档案成功",rev->id);
							bcopy(&rev->charbase , &pUser->charbase , sizeof(CharBase));
							//sky 读取一键换装
							memcpy(pUser->packs.m_OneKeyEquip,rev->charbase.KeyEquip,sizeof(pUser->packs.m_OneKeyEquip));
							//sky 读取称号
							memcpy(pUser->packs.m_Chenghao,rev->charbase.chenghao,sizeof(pUser->packs.m_Chenghao));
							//sky 读取时装魔盒
							memcpy(pUser->packs.m_Shizhuang,rev->charbase.shizhuang,sizeof(pUser->packs.m_Shizhuang));
							//sky 读取时装魔盒
							memcpy(pUser->packs.m_Pifeng,rev->charbase.pifeng,sizeof(pUser->packs.m_Pifeng));
							//sky 读取时装魔盒
							memcpy(pUser->packs.m_Chibang,rev->charbase.chibang,sizeof(pUser->packs.m_Chibang));
							//sky 读取时装魔盒
							memcpy(pUser->packs.m_Zuoqi,rev->charbase.zuoqi,sizeof(pUser->packs.m_Zuoqi));
							//sky 读取功勋竞猜
							memcpy(pUser->packs.m_jingcai,rev->charbase.jingcai,sizeof(pUser->packs.m_jingcai));
							if (pUser->charbase.reliveWeakTime>0)
							{
								if (pUser->charbase.reliveWeakTime <=450)
									pUser->charbase.reliveWeakTime = SceneTimeTick::currentTime.sec()%10000+pUser->charbase.reliveWeakTime;
								else
									pUser->charbase.reliveWeakTime=0;
							}

							//Zebra::logger->debug("%u, %u",rev->charbase.x,rev->charbase.y);
							// load Object
							//设置宠物包裹的大小
							PetPack *pack = (PetPack *)pUser->packs.getPackage(Cmd::OBJECTCELLTYPE_PET, 0);
							if (pack)
							{
								pack->setAvailable(pUser->charbase.petPack);
								//Zebra::logger->debug("%s 宠物包裹大小 %u", pUser->name, pUser->charbase.petPack);
							}
							if(rev->dataSize != (DWORD)PARA_SCENE_USER_READ_ERROR)
							{
								BYTE petData[sizeof(int)+sizeof(Cmd::t_PetData)*80];//可以存放10个宠物
								bzero(petData, sizeof(petData));

								if(rev->dataSize != 0)
								{
									dataOk =uncompressSaveData(pUser , (unsigned char *)rev->data , rev->dataSize, petData);
								}
								else
								{
									dataOk=true;
									//暂时时档案补偿
									if(!pUser->isNewCharBase())
									{
										/*
										pUser->charbase.points=pUser->charbase.points
											+pUser->charbase.wdProperty[0]
											+pUser->charbase.wdProperty[1]
											+pUser->charbase.wdProperty[2]
											+pUser->charbase.wdProperty[3]
											+pUser->charbase.wdProperty[4]
											;
										pUser->charbase.wdProperty[0]=0;
										pUser->charbase.wdProperty[1]=0;
										pUser->charbase.wdProperty[2]=0;
										pUser->charbase.wdProperty[3]=0;
										pUser->charbase.wdProperty[4]=0;
										pUser->charbase.skillpoint=pUser->charbase.level;
										pUser->packs.addMoney(pUser->charbase.level * 200000,"给钱");
										//默认系统设置
										using namespace Cmd;
										memset(pUser->sysSetting,0xff,sizeof(pUser->sysSetting));
										pUser->sysSetting[0] = 0;//pk模式
										//clear_state(sysSetting , Cmd::USER_SETTING_AUTOFINDPATH);//自动寻路
										clear_state(pUser->sysSetting , Cmd::USER_SETTING_SHOW_PLAYERNAME);//显示玩家名字
										clear_state(pUser->sysSetting , Cmd::USER_SETTING_AUTO_KILL_SUMMON);//自动打怪
										//通知session
										Cmd::Session::t_sysSetting_SceneSession send;
										strncpy((char *)send.name, pUser->name, MAX_NAMESIZE-1);
										bcopy(pUser->sysSetting, &send.sysSetting, sizeof(send.sysSetting));
										send.face = pUser->charbase.face;
										sessionClient->sendCmd(&send, sizeof(send));

										pUser->chatColor[0] = 0xffffffff;
										pUser->chatColor[1] = 0xffffd100;//COLOR_ARGB(255,255,209,0);
										pUser->chatColor[2] = 0xff4eaa00;//COLOR_ARGB(255,78,170,0);
										pUser->chatColor[3] = 0xffff4818;//COLOR_ARGB(255,255,72,24);
										pUser->chatColor[4] = 0xff006300;//COLOR_ARGB(255,0,99,0);
										pUser->chatColor[5] = 0xff98f417;//COLOR_ARGB(255,152,244,23);
										pUser->chatColor[6] = 0xffff627c;//COLOR_ARGB(255,255,98,124);
										pUser->chatColor[7] = 0xff007fff;//COLOR_ARGB(255,0,127,255);
										//通知客户端
										Cmd::stSystemSettingsUserCmd sendClient;
										bcopy(pUser->sysSetting, &sendClient.data.bySettings, sizeof(sendClient.data.bySettings));
										bcopy(pUser->chatColor, &sendClient.data.dwChatColor, sizeof(sendClient.data.dwChatColor));
										pUser->sendCmdToMe(&sendClient, sizeof(sendClient));
										// */

									}
								}
							// */
								if(pUser&&(pUser->charbase.level<scene->getLevel()))
								{       
									char map[MAX_NAMESIZE+1];
									bzero(map,sizeof(map));
									bcopy(scene->getName(),map,6);
									bcopy("清源村",&map[6],6);
									scene=SceneManager::getInstance().getSceneByName(map);
									if (!scene)
									{
										Zebra::logger->fatal("读取%u档案失败,其等级与地图不符合，但未找到清源村地图",rev->id);
										return true;
									}
								}   
								if(dataOk)
								{
									pUser->initCharBase(scene);
									//用户进入地图，并发送主用户信息
									if(pUser->intoScene(scene,true,pUser->getPos()))
									{
										Cmd::Session::t_regUserSuccess_SceneSession ret; // 通知会话服务器用户进入游戏成功
										ret.dwUserID=rev->id;
										ret.dwGrace = pUser->charbase.grace;
										ret.dwExploit = pUser->charbase.exploit;
										ret.dwCountryID = pUser->charbase.country;
										ret.dwSchoolID = pUser->charbase.schoolid;
										ret.dwSeptID = pUser->charbase.septid;
										ret.dwUnionID = pUser->charbase.unionid;
										ret.qwExp = pUser->charbase.exp;
										sessionClient->sendCmd(&ret,sizeof(ret));
										//如果死亡刷回重生区
										if((int)pUser->charbase.hp <= 0)
										{
											pUser->relive(Cmd::ReliveHome , 0 , 100);
										}
										pUser->loadPetState(petData,sizeof(petData));

										//检查新邮件
										if (Zebra::global["mail_service"]=="on")
										{
										Cmd::Session::t_checkNewMail_SceneSession cn;
											cn.userID = pUser->tempid;
											sessionClient->sendCmd(&cn, sizeof(cn));
										}

										Cmd::stTimeSyncUserCmd send;
										send.serverTime = time(NULL);
										pUser->sendCmdToMe(&send, sizeof(send));
										//检查宠物列表
										Cmd::Session::t_loadCartoon_SceneSession lc;
										lc.userID = pUser->id;
										sessionClient->sendCmd(&lc, sizeof(lc));

										//通知mini服务器
										Cmd::Mini::t_Scene_SetScene ss;
										ss.userID = pUser->id;
										ss.serverID = ScenesService::getInstance().getServerID();
										miniClient->sendCmd(&ss, sizeof(ss));
										//soke 双倍经验							
										if(ScenesService::getInstance().double_exp > 0)
										{
											//Channel::sendSys(pUser, Cmd::INFO_TYPE_SCROLL, "双倍经验时间，装备双倍经验道具将无效果，请不要浪费哦！");
										}
		

										if (pUser->charbase.reliveWeakTime > 0)
										{
											if (pUser->charbase.reliveWeakTime > SceneTimeTick::currentTime.sec()%10000)
											{// 调用预处理，重新计算五项属性
												pUser->showCurrentEffect(Cmd::USTATE_RELIVEWEAK, true); // 更新客户端状态
												pUser->sendtoSelectedReliveWeakState();
											}
										}



										if (pUser->charbase.flower1time)
										{
											pUser->showCurrentEffect(Cmd::USTATE_FLOWER_9, true); // 更新客户端状态
											pUser->sendtoSelected9State();
										}
										if (pUser->charbase.flower2time)
										{
											pUser->showCurrentEffect(Cmd::USTATE_FLOWER_99, true); // 更新客户端状态
											pUser->sendtoSelected99State();
										}
										if (pUser->charbase.flower3time)
										{
											pUser->showCurrentEffect(Cmd::USTATE_FLOWER_999, true); // 更新客户端状态
											pUser->sendtoSelected999State();

											pUser->showCurrentEffect(Cmd::USTATE_USER_MEILI, true); // 更新客户端状态
											pUser->sendtoSelectedyswcState();
										}

										if (pUser->charbase.flower4time)
										{
											pUser->showCurrentEffect(Cmd::USTATE_FLOWERL_9, true); // 更新客户端状态
											pUser->sendtoSelectedx9State();
										}
										if (pUser->charbase.flower5time)
										{
											pUser->showCurrentEffect(Cmd::USTATE_FLOWERL_99, true); // 更新客户端状态
											pUser->sendtoSelectedx99State();
										}
										if (pUser->charbase.flower6time)
										{
											pUser->showCurrentEffect(Cmd::USTATE_FLOWERL_999, true); // 更新客户端状态
											pUser->sendtoSelectedx999State();

											pUser->showCurrentEffect(Cmd::USTATE_USER_RENQI, true); // 更新客户端状态
											pUser->sendtoSelectedyswc2State();
										}
										//soke 登录时砖块特效处理
                                        pUser->doLoongEffWhenLogin();
										/////////////////////////////

										return true;
									}
									else 
									{
										//	scene->removeUser(pUser);
										//	SceneUserManager::getMe().removeUser(pUser);
										//									static_cast<SceneUserVisitor *>(pUser)->destroy();
										//SAFE_DELETE(pUser);
									}
								}
								else
								{
									Zebra::logger->fatal("加载%u档案失败,可能是表格错误",rev->id);
								}
							}
							else
							{
								Zebra::logger->fatal("读取%u档案失败,无效用户数据",rev->id);
							}
						}
						else
							Zebra::logger->fatal("读取%u档案失败,未找到用户数据",rev->id);
					}
					else
						Zebra::logger->fatal("读取%u档案失败,未找到地图,maptempid=%u",rev->id,rev->dwMapTempID);
					/*
					// 通知Session 注册失败
					Cmd::Session::t_unregUser_SceneSession ret;
					ret.dwSceneTempID=rev->dwMapTempID;
					ret.dwUserID=rev->id;
					ret.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
					sessionClient->sendCmd(&ret,sizeof(ret));
					if(pUser)
					{
						// 通知Record 读取失败
						Cmd::Record::t_RemoveUser_SceneRecord rec_ret;
						rec_ret.accid = pUser->accid;
						rec_ret.id = pUser->id;
						recordClient->sendCmd(&rec_ret, sizeof(rec_ret));
						 
						// 通知Gateway 注册失败
						Cmd::Scene::t_Unreg_LoginScene ret;
						ret.dwUserID=pUser->id;
						ret.dwSceneTempID=rev->dwMapTempID;
						ret.retcode=Cmd::Scene::UNREGUSER_RET_ERROR;
						pUser->gatetask->sendCmd(&ret,sizeof(ret));
						SceneUserManager::getMe().removeUser(pUser);
						Zebra::logger->debug("档案失败注销(%s,%u)",pUser->name , pUser->id);
						pUser->destroy();
						SAFE_DELETE(pUser);

						//						static_cast<SceneUserVisitor *>(pUser)->destroy();
					}
					// */
					return true;
				}
				break;
			case PARA_SCENE_USER_EXIST:
				{
					t_userExist_SceneRecord * rev = (t_userExist_SceneRecord *)ptNullCmd;

					SceneUser * pUser = SceneUserManager::getMe().getUserByTempID(rev->fromID);
					if (!pUser)
					{
						Zebra::logger->alarm("[邮件]检查目标玩家 %s 后，发送者已不在线", rev->sm.toName);
						return false;
					}
					pUser->isSendingMail = false;
					if((pUser->privatestore.step() != PrivateStore::NONE) || pUser->tradeorder.hasBegin())
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "交易过程中不能发送邮件");
						return true;
					}

					if (0==rev->toID)
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "不存在该角色，请输入正确的名字");
						return true;
					}

					if (!pUser->packs.checkMoney(rev->sm.sendMoney?rev->sm.sendMoney+Cmd::mail_postage:Cmd::mail_postage))                          
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你的银两不足");

						return true;
					}
					if (!pUser->packs.checkGold(rev->sm.sendGold)) 
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你的金子不足");

						return true;
					}

					Cmd::Session::t_sendMail_SceneSession sm;
					sm.mail.state = Cmd::Session::MAIL_STATE_NEW;
					strncpy(sm.mail.fromName, pUser->name, MAX_NAMESIZE);
					strncpy(sm.mail.toName, rev->sm.toName, MAX_NAMESIZE);
					strncpy(sm.mail.title, rev->sm.title, MAX_NAMESIZE);
					sm.mail.type = Cmd::Session::MAIL_TYPE_MAIL;
					zRTime ct;
					sm.mail.createTime = ct.sec();
					sm.mail.delTime = sm.mail.createTime + 60*60*24*7;
					strncpy(sm.mail.text, rev->sm.text, 256);
					sm.mail.sendMoney = rev->sm.sendMoney;
					sm.mail.recvMoney = rev->sm.recvMoney;
					sm.mail.sendGold = 0;//不发送金币
					sm.mail.recvGold = 0;//不发送金币
					//sm.itemID = rev->sm.itemID;
					sm.mail.itemGot = 0;
					sm.mail.fromID = pUser->id;
					sm.mail.toID = rev->toID;
					if (rev->sm.sendMoney||rev->sm.sendGold)
						sm.mail.accessory = 1;
					//bcopy(&rev->mail, &sm.mail, sizeof(mailInfo));

					if (rev->sm.itemID && rev->sm.itemID != 0xffffffff)//INVALID_THISID
					{
						zObject * srcobj=pUser->packs.uom.getObjectByThisID(rev->sm.itemID);
						if (!srcobj)
						{
							Zebra::logger->trace("%s 发送邮件时未找到该物品物品 id=%u", pUser->name, rev->sm.itemID);
							return false;
						}
						if (!srcobj->canMail())
						{
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你不能邮寄这件物品");
							return true;
						}

						pUser->packs.removeObject(srcobj, true, false); //notify but not delete
						
						/////////////////////////////////////
						//srcobj->getSaveData((SaveObject *)&sm.item);
						SaveObject so;

						srcobj->getSaveData(&so);
						
						sm.item.createtime = so.createtime;
						sm.item.object = so.object;
						///////////////////////////////////////

						sm.mail.itemID = srcobj->data.qwThisID;
						
						//srcobj->getSaveData((SaveObject *)&sm.item);

						zObject::logger(srcobj->createid,srcobj->data.qwThisID,srcobj->base->name,srcobj->data.dwNum,srcobj->data.dwNum,0,pUser->id,pUser->name,0,rev->sm.toName,"邮寄到信箱",srcobj->base,srcobj->data.kind,srcobj->data.upgrade);
						//pUser->packs.removeObject(srcobj);//notify and delete

						zObject::destroy(srcobj);
						sm.mail.accessory = 1;
					}
					pUser->packs.removeMoney(rev->sm.sendMoney+Cmd::mail_postage,"发送邮件邮件");//扣钱
					//pUser->packs.removeGold(rev->sm.sendGold, "发送邮件");//扣钱

					if (sessionClient->sendCmd(&sm, sizeof(Cmd::Session::t_sendMail_SceneSession)))
					{
						pUser->save(OPERATION_WRITEBACK);//立刻存档
						Zebra::logger->trace("发送邮件 %s->%s", pUser->name, rev->sm.toName);
						return true;
					}
					else
					{
						Zebra::logger->error("邮件发送失败 %s->%s money=%u gold=%u", pUser->name, rev->sm.toName, rev->sm.sendMoney, rev->sm.sendGold);
						return false;
					}
					return true;
				}
				break;
			case PARA_SCENE_USER_WRITE_OK:
				{
					t_WriteUser_SceneRecord_Ok * rev = (t_WriteUser_SceneRecord_Ok *)ptNullCmd;
					Cmd::Scene::t_Unreg_LoginScene_Ok ok; 
					ok.type=rev->type;
					ok.id=rev->id;
					ok.accid=rev->accid;
					SceneTaskManager::getInstance().broadcastCmd(&ok,sizeof(ok));
					return true;
				}
				break;
			default:
				break;
		}
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

