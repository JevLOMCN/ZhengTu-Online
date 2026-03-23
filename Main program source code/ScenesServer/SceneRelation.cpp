/**
 * \file
 * \version  $Id: SceneRelation.cpp  $
 * \author  
 * \date 
 * \brief 实现对关系命令的处理
 *
 */

#include "SceneUser.h"
#include "Scene.h"
#include "RecordClient.h"
#include "Chat.h"
#include "SessionCommand.h"
#include "SessionClient.h"
#include "Team.h"
#include "TeamManager.h"


#define MARRY_REQUEST_LEVEL 40
#define MARRY_REQUEST_M_ITEM_ID 2226
#define MARRY_REQUEST_W_ITEM_ID 2227
#define MARRY_REQUEST_MONEY 3000
#define MARRY_REQUEST_MONEY1 5000
#define MARRY_REQUEST_MONEY2 10000
#define MARRY_PRESENT_MALE 875
#define MARRY_PRESENT_FEME 875


/**
  * \brief 一个比较器
  *
  *  用来比较两个zObject实例是否是相等的
  *  (暂时未使用)
  *
  */
class ItemObjectCompare:public UserObjectCompare 
{
	public:
		DWORD  dwObjectID;

		bool isIt(zObject *object)
		{
			if (object->data.dwObjectID == dwObjectID) return true;
			return false;
		}
};

/**
  * \brief 进行婚姻的匹配，查找符合条件的一对
  * 
  *
  */
struct FindConsort : public TeamMemExec
{
	SceneUser *feme;
	SceneUser *male;
	SceneUser *me;
	FindConsort(SceneUser *pUser)
	{
		feme = NULL;
		male = NULL;
		me = pUser;
	}
	virtual bool exec(TeamMember &member)
	{
       SceneUser *pUser = NULL;
	   if (me->id == member.id)
	   {
		   pUser = me;
	   }
	   else
	   {
		   pUser = SceneUserManager::getMe().getUserByID(member.id);
	   }
		if (pUser)
		{
			switch(pUser->charbase.type)
			{
				case PROFESSION_1:		//侠客
				case PROFESSION_3:		//箭侠
				case PROFESSION_5:		//天师
				case PROFESSION_7:		//法师
					male = pUser;
					break;
				case PROFESSION_2:		//侠女
				case PROFESSION_4:		//箭灵
				case PROFESSION_6:		//美女
				case PROFESSION_8:		//仙女
					feme = pUser;
					break;
				case PROFESSION_NONE:	//无业
				default:
					Zebra::logger->error("错误的职业类型");
					break;
			}
		}
		return true;
	}
};

void SceneUser::addObjectToUserPacket(int objectid, SceneUser *pUser)
{
	zObjectB *base = objectbm.get(objectid);
	if (base)
	{
		zObject *o=zObject::create(base,1);
		if (!pUser->packs.addObject(o, true, AUTO_PACK))
		{
			pUser->scene->addObject(o,pUser->getPos());
		}
		else
		{
			Cmd::stAddObjectPropertyUserCmd item;
			item.byActionType=Cmd::EQUIPACTION_OBTAIN;
			bcopy(&o->data,&item.object,sizeof(item.object));
			pUser->sendCmdToMe(&item,sizeof(item));
		}
	}
}

/**
  * \brief 处理关系命令
  *
  * 处理的关系命令如下:
  *
  * Cmd::MARRY_STATUS_CHECK_PARA
  *
  * \param rev: 关系命令
  * \param cmdLen: 命令长度
  *
  * \return 命令被处理返回TRUE,否则为FALSE
  *
  *
  */
bool SceneUser::doRelationCmd(const Cmd::stRelationUserCmd *rev,unsigned int cmdLen)
{
	switch(rev->byParam)
	{
		case Cmd::MARRY_STATUS_CHECK_PARA:
			{
				/*
				char countrymap[][MAX_NAMESIZE]=
				{
					"单","尚","敖","海润城",
					"耀辉城","禹","贤","岳",
					"炎","恒","玄","湖海城"
				};
				*/
				Cmd::stMarryStatusCheckCmd *ptCmd=(Cmd::stMarryStatusCheckCmd *)rev;

				if (ptCmd->byStep == Cmd::MARRY_REFUSE)
				{
					SceneUser *pUser = NULL;
					pUser = SceneUserManager::getMe().getUserByID(this->friendID);
					if (pUser)
					{
						this->answerMarry=false;
						this->friendID=0;
						Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "非常遗憾，对方拒绝了你！");
					}
					return true;
				}

				if (ptCmd->byStep == Cmd::MARRY_AGREE)
				{
					SceneUser *pUser = NULL;
					this->answerMarry = true;
					pUser = SceneUserManager::getMe().getUserByID(this->friendID);
					if (pUser)
					{
						ptCmd->byStep = Cmd::MARRY_ITEM_CHECK;
						pUser->doRelationCmd(rev, cmdLen);
						return true;
					}
					return true;
				}

				SceneUser *pUser = NULL;
				DWORD leader = team.getLeader();
				if (leader)
				{
					pUser = SceneUserManager::getMe().getUserByTempID(leader);
					if (!pUser) pUser= this;
				}
				else
				{
					pUser = this;
				}

				/*
				unsigned int i=0;
				for(i=0; i<12;i++)
				{
					if (strcmp(countrymap[i], pUser->scene->getName()) ==0) break;
				}

				if (pUser->charbase.country != i)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "不处理外国人的婚姻登记！");
					return true;
				}
				*/

				if (pUser->team.getSize()==2)
				{
					FindConsort callback(this);
					pUser->team.execEveryOne(callback);

					if (callback.feme&&callback.male)
					{
						if (pUser->scene->checkTwoPosIInNine(callback.feme->getPosI(), callback.male->getPosI()))
						{
							if (callback.feme->charbase.country == callback.male->charbase.country)
							{
								if (callback.feme->charbase.level >=MARRY_REQUEST_LEVEL &&
									callback.male->charbase.level >=MARRY_REQUEST_LEVEL)
								{
									if (callback.feme->charbase.consort ==0 &&
										callback.male->charbase.consort ==0)
									{
										if (ptCmd->byStep == Cmd::MARRY_AHEAD_CHECK)
										{
											callback.feme->friendID=0;
											callback.feme->answerMarry=false;
											callback.male->friendID=0;
											callback.male->answerMarry=false;
											sendCmdToMe(rev, cmdLen);
											return true;
										}

										SceneUser *pOther = NULL;
										if (this == callback.feme)
										{
											pOther = callback.male;
										}
										else
										{
											pOther = callback.feme;
										}

										if (ptCmd->byStep == Cmd::MARRY_ITEM_CHECK && !pOther->answerMarry)
										{
											pOther->friendID = this->id;
											ptCmd->byStep = Cmd::MARRY_ANSWER;
											pOther->sendCmdToMe(rev, cmdLen);
											return true;
										}
										else
										{
											pOther->answerMarry = false;
											pOther->friendID = 0;
										}

										ItemObjectCompare found;
										found.dwObjectID = MARRY_REQUEST_M_ITEM_ID;
										zObject *mitemobj = callback.male->packs.uom.getObject(found);
										found.dwObjectID = MARRY_REQUEST_W_ITEM_ID;
										zObject *witemobj = callback.feme->packs.uom.getObject(found);

										if (mitemobj && witemobj)
										{
											if (ptCmd->byStep == Cmd::MARRY_ITEM_CHECK)
											{
												sendCmdToMe(rev, cmdLen);
												return true;
											}
											if (ptCmd->byStep != Cmd::MARRY_PAY_MONEY  &&
												ptCmd->byStep != Cmd::MARRY_PAY_MONEY1 &&
												ptCmd->byStep != Cmd::MARRY_PAY_MONEY2) return true;

											switch(ptCmd->byStep)
											{
												case Cmd::MARRY_PAY_MONEY:
													{
														if (packs.checkMoney(MARRY_REQUEST_MONEY) && packs.removeMoney(MARRY_REQUEST_MONEY,"结婚0"))
														{
															addObjectToUserPacket(MARRY_PRESENT_MALE, callback.male);
															addObjectToUserPacket(MARRY_PRESENT_FEME, callback.feme);
														}
														else
														{
															ptCmd->byStep = Cmd::MARRY_NO_MONEY;
															sendCmdToMe(ptCmd, cmdLen);
															Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你没有足够的钱来交纳结婚费用");
															return true;
														}
													}
													break;
												case Cmd::MARRY_PAY_MONEY1:
													{
														if (packs.checkMoney(MARRY_REQUEST_MONEY1) && packs.removeMoney(MARRY_REQUEST_MONEY1,"结婚1"))
														{
															addObjectToUserPacket(MARRY_PRESENT_MALE, callback.male);
															addObjectToUserPacket(1723, callback.male);
															addObjectToUserPacket(1723, callback.male);
															addObjectToUserPacket(MARRY_PRESENT_FEME, callback.feme);
															addObjectToUserPacket(1723, callback.feme);
															addObjectToUserPacket(1723, callback.feme);
														}
														else
														{
															ptCmd->byStep = Cmd::MARRY_NO_MONEY;
															sendCmdToMe(ptCmd, cmdLen);
															Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你没有足够的钱来交纳结婚费用");
															return true;
														}
													}
													break;
												case Cmd::MARRY_PAY_MONEY2:
													{
														if (packs.checkMoney(MARRY_REQUEST_MONEY2) && packs.removeMoney(MARRY_REQUEST_MONEY2,"结婚2"))
														{
															addObjectToUserPacket(MARRY_PRESENT_MALE, callback.male);
															addObjectToUserPacket(1723, callback.male);
															addObjectToUserPacket(1723, callback.male);
															addObjectToUserPacket(1723, callback.male);
															addObjectToUserPacket(1723, callback.male);
															addObjectToUserPacket(MARRY_PRESENT_FEME, callback.feme);
															addObjectToUserPacket(1723, callback.feme);
															addObjectToUserPacket(1723, callback.feme);
															addObjectToUserPacket(1723, callback.feme);
															addObjectToUserPacket(1723, callback.feme);

															callback.male->summonPet(60001,Cmd::PET_TYPE_TOTEM,180);
															callback.male->summonPet(60003,Cmd::PET_TYPE_TOTEM,180);
															callback.male->summonPet(60003,Cmd::PET_TYPE_TOTEM,180);
															callback.male->summonPet(60003,Cmd::PET_TYPE_TOTEM,180);

															callback.feme->summonPet(60002,Cmd::PET_TYPE_TOTEM,180);
															callback.feme->summonPet(60004,Cmd::PET_TYPE_TOTEM,180);
															callback.feme->summonPet(60004,Cmd::PET_TYPE_TOTEM,180);
															callback.feme->summonPet(60004,Cmd::PET_TYPE_TOTEM,180);
														}
														else
														{
															ptCmd->byStep = Cmd::MARRY_NO_MONEY;
															sendCmdToMe(ptCmd, cmdLen);
															Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你没有足够的钱来交纳结婚费用");
															return true;
														}
													}
													break;
												default:
													break;
											}
											callback.male->packs.removeObject(mitemobj);
											callback.feme->packs.removeObject(witemobj);

	
											callback.feme->charbase.consort = callback.male->charbase.id;
											callback.male->charbase.consort = callback.feme->charbase.id;
	
											Cmd::stRelationStatusCmd send;
											send.byState = Cmd::RELATION_ANSWER_YES;
											send.type = Cmd::RELATION_TYPE_LOVE;
											if (callback.feme == this)
											{
												send.userid = callback.male->charbase.id;
											}
											else
											{
												send.userid = callback.feme->charbase.id;
											}
											this->forwardSession(&send, sizeof(send));

											sendCmdToMe(ptCmd, cmdLen);
											//Channel::sendSys(callback.feme, Cmd::INFO_TYPE_GAME, "恭喜你们结婚成功");
											//Channel::sendSys(callback.male, Cmd::INFO_TYPE_GAME, "恭喜你们结婚成功");


											char buf[MAX_CHATINFO];
											sprintf(buf, "征途恭祝%s与%s喜结莲理，白头偕老！", callback.feme->name,callback.male->name);
											zRTime ctv;
											/////////////////////////////////////////////////////////////////////
											//soke 结婚给出2次滚动公告
											//soke 结婚滚动系统公告（结婚）
											Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEJROLL,"白首齐眉鸳鸯比翼，青阳启瑞桃李同心。%s和%s，祝福你们，祝你们爱情之树常青。愿你们互相珍惜，心心相印，美满幸福，天长地久！",callback.feme->name,callback.male->name);

											//soke 结婚滚动系统公告（结婚）
											Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEJROLL,"白首齐眉鸳鸯比翼，青阳启瑞桃李同心。%s和%s，祝福你们，祝你们爱情之树常青。愿你们互相珍惜，心心相印，美满幸福，天长地久！",callback.feme->name,callback.male->name);
											/////////////////////////////////////////////////////////////////////
											Cmd::stChannelChatUserCmd sendMsg;
											sendMsg.dwType=Cmd::CHAT_TYPE_COUNTRY_MARRY;
											switch(ptCmd->byStep)
											{
												case Cmd::MARRY_PAY_MONEY:
													sendMsg.dwSysInfoType = Cmd::INFO_TYPE_SYS;//系统消息的类型
													break;
												case Cmd::MARRY_PAY_MONEY1:
													sendMsg.dwSysInfoType = Cmd::INFO_TYPE_SKYROCKET1;//系统消息的类型
													break;
												case Cmd::MARRY_PAY_MONEY2:
													sendMsg.dwSysInfoType = Cmd::INFO_TYPE_SKYROCKET2;//系统消息的类型
													break;
												default:
													sendMsg.dwSysInfoType = Cmd::INFO_TYPE_SYS;//系统消息的类型
													break;
											}

											//sendMsg.dwSysInfoType = Cmd::INFO_TYPE_EXP;//系统消息的类型
											sendMsg.dwChatTime = ctv.sec();
											bzero(sendMsg.pstrName, sizeof(sendMsg.pstrName));
											bzero(sendMsg.pstrChat, sizeof(sendMsg.pstrChat));
											strncpy((char *)sendMsg.pstrChat,buf,MAX_CHATINFO-1);
											strncpy((char *)sendMsg.pstrName,callback.feme->name,MAX_NAMESIZE);
											Channel::sendCountry(callback.feme, &sendMsg, sizeof(sendMsg));
											return true;
										}
										else
										{
											if (ptCmd->byStep == Cmd::MARRY_ITEM_CHECK)
											{
												ptCmd->byStep = Cmd::MARRY_NO_ITEM;
												sendCmdToMe(ptCmd, cmdLen);
												if (!mitemobj)
												{
													Channel::sendSys(callback.male, Cmd::INFO_TYPE_FAIL, "你没有准备钻戒？快去找红娘置备彩礼吧");
												}
												if (!witemobj)
												{
													Channel::sendSys(callback.feme, Cmd::INFO_TYPE_FAIL, "你的嫁妆还没有好？快去找红娘问问吧");
												}
												return true;
											}
											else
											{
												Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "没有定情物！");
												return true;
											}
										}
	
									}
									else
									{
										Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你们中间有人已经有配偶，重婚是不允许的！");
									}
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你们等级还不够，成熟点再来吧！");
								}
							}
							else
							{
								Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你们俩的国籍不一样，我这不受理异国婚姻登记，要么改国籍以后再来找我！");
							}
						}
						else
						{
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你们必须都到我跟前来，我才能帮你们办理结婚手续！");
						}
					}
					else
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "恩....同性是不能结婚的！");
					}
				}
				else
				{
					if (team.getSize()<2)
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "得跟你的心上人先组好队！");
					else
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的队伍里面只能有2个人，无关人员请让他们退出！");
				}
				ptCmd->byStep = Cmd::MARRY_AHEAD_CHECK_FAIL;
				sendCmdToMe(ptCmd, cmdLen);
			}
			break;
		default:
			break;
	}
	return false;
}

