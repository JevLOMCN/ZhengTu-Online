/**
 * \file
 * \version  $Id: SceneUnion.cpp  $
 * \author  
 * \date 
 * \brief 实现帮会命令的处理
 *
 */
#include "SceneUser.h"
#include "Scene.h"
#include "RecordClient.h"
#include "Union.h"
#include "Chat.h"
#include "SessionCommand.h"
#include "SessionClient.h"

using namespace UnionDef;
/**
  * \brief 一个比较器
  *
  *  用于查找建立帮会所需要的道具是否存在
  *  
  *
  */
class UnionObjectCompare:public UserObjectCompare 
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
  * \brief 处理用户帮会命令
  *
  * 处理的帮会命令如下:
  *
  * Cmd::UNION_STATUS_CHECK_PARA
  *
  * Cmd::CREATE_UNION_PARA
  *
  * Cmd::ADD_MEMBER_TO_UNION_PARA
  *
  * \param rev: 帮会命令
  * \param cmdLen: 命令长度
  *
  * \return 命令被处理返回TRUE,否则为FALSE
  *
  *
  */
bool SceneUser::doUnionCmd(const Cmd::stUnionUserCmd *rev,unsigned int cmdLen)
{
	switch(rev->byParam)
	{
		case Cmd::CONTRIBUTE_UNION_PARA:
			{
				Cmd::stContributeUnion* ptCmd = (Cmd::stContributeUnion*)rev;

				if (ptCmd->dwMoney>0 && this->charbase.unionid>0) {
					if (packs.checkMoney(ptCmd->dwMoney) 
							&& packs.removeMoney(ptCmd->dwMoney, "帮会捐献")) {	
						Cmd::Session::t_OpUnionMoney_SceneSession send;
						send.dwUnionID = this->charbase.unionid;
						send.dwMoney = ptCmd->dwMoney;
						sessionClient->sendCmd(&send, sizeof(send));

						this->charbase.exploit += (ptCmd->dwMoney/2000 * exploit_arg);
						
						Cmd::stAddUserMapScreenUserCmd update;
						full_t_MapUserData(update.data);
						sendCmdToMe(&update, sizeof(update));
					}
					else
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "剩余银两不足，不能捐献");
					}
				}
								
				return true;
			}
			break;
		case Cmd::QUESTION_UNION_CITY_INFO_PARA:
			{
				Cmd::Session::t_questionUnionCity_SceneSession send;
				Cmd::stQuestionUnionCityInfo* ptCmd = (Cmd::stQuestionUnionCityInfo*)rev;
					
				send.dwUserID = this->id;
				send.dwCountryID = this->scene->getCountryID();
				send.dwCityID = this->scene->getRealMapID();
				send.byType = ptCmd->byType;
				sessionClient->sendCmd(&send, sizeof(send));
				return true;
			}
			break;
		case Cmd::ENTER_UNION_CITY_AREA_PARA:
			{
				Cmd::Session::t_enterUnionManor_SceneSession send;
				send.dwUserID = this->id;
				send.dwCountryID = this->scene->getCountryID();
				send.dwCityID = this->scene->getRealMapID();

				if (this->charbase.unionid>0)
				{
					sessionClient->sendCmd(&send, sizeof(send));
				}
				else
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你还未加入帮会。不能进入帮会领地");
					return true;
				}
				
				return true;
			}
			break;
		case Cmd::UNION_STATUS_CHECK_PARA:
		case Cmd::CREATE_UNION_PARA:
			{
				DWORD dwItemID = 0; // 用于保存道具对象id
				bool bState = true; //先初始化成成功状态

				// 没有帮会，并且是族长
				if (charbase.unionid == 0 && charbase.septid>0 && this->septMaster) 
				{
						if (charbase.level >= CREATE_UNION_NEED_LEVEL)
						{
								UnionObjectCompare found;
								found.dwObjectID = CREATE_UNION_NEED_ITEM_ID;
								zObject *itemobj = packs.uom.getObject(found);// 查找天羽令
								
								if (itemobj)
								{
										if (packs.checkMoney(CREATE_UNION_NEED_PRICE_GOLD) /*&& packs.removeMoney(CREATE_UNION_NEED_PRICE_GOLD)*/) {
											//报告成功状态
											bState = true;
											dwItemID = itemobj->data.qwThisID;  // 记录任务道具的对象id
										}
										else
										{
											//报告没有足够的钱
											bState = false;
											Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你需要%u文钱来创建帮会，你没有足够的钱",CREATE_UNION_NEED_PRICE_GOLD);
										}//dwItemID = itemobj->data.qwThisID;  // test
									}
									else 
									{
										//报告没有道具
										bState = false;
										Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "缺少任务道具 %s 无法创建帮会", "天羽令");
									}
						}
						else
						{
							//报告等级不够
							bState = false;
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "等级不足%u级无法创建帮会",CREATE_UNION_NEED_LEVEL);
						}
				}
				else
				{
						bState = false;
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, 
							"已经加入帮会无法再创建帮会或不是族长");
				}
				
				//返回帮会建立状态
				if (Cmd::CREATE_UNION_PARA == rev->byParam)
				{
					Cmd::stCreateUnionCmd *ptCmd=(Cmd::stCreateUnionCmd *)rev;
					if (bState)
					{
						Cmd::Session::t_addUnion_SceneSession send;
						
						send.dwMapTempID = scene->tempid;
						send.dwItemID 	 = dwItemID;
						send.info.dwCountryID = this->charbase.country;
						send.info.dwUnionID = 0;					// 帮会的ID
						send.info.dwMana = 0;
						strncpy(send.info.name,ptCmd->UnionName,MAX_NAMESIZE); 			// 帮会名称
						send.info.dwCharID = charbase.id;	                    		  // 会长的角色ID
						strncpy(send.info.masterName,charbase.name,MAX_NAMESIZE);		// 会长的名字
						send.info.wdLevel = 1;     		    	       	                // 帮会级别
						send.info.qwExp = 0;                              		      // 帮会经验
						send.info.byVote = 0; //帮会投票标志
						send.info.dwActionPoint = 0;
						send.info.dwMoney = 0;
						send.info.calltimes = 0;
						sessionClient->sendCmd(&send, sizeof(send));
						return true;
					}
				}
				////////////////////////////////////////
				if (bState)
				{
						Cmd::stUnionStatusCheckCmd retCmd;
						sendCmdToMe(&retCmd,sizeof(retCmd));
				}
				///////////////////////////////////////
				
				return true;
			}
			break;
		case Cmd::ADD_MEMBER_TO_UNION_PARA:
			{
				Cmd::stAddMemberToUnionCmd *ptCmd=(Cmd::stAddMemberToUnionCmd *)rev;
				switch(ptCmd->byState)
				{
					case Cmd::QUESTION:
						{
							if (0 == charbase.unionid)
							{
								Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你必须先创立帮会才能招收成员");
								return true;
							}

							SceneUser *pUser=scene->getUserByName(ptCmd->memberName);
							if (NULL != pUser)
							{
								//if (scene->checkUserInNine(this,pUser))
								//{
									if (!isset_state(pUser->sysSetting , Cmd::USER_SETTING_UNION))                                                            
									{
										Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "玩家 %s加入帮会未开启", pUser->name);                                                   
											return true;
									}  
									
									if (0==pUser->charbase.unionid
									   && pUser->charbase.septid>0
									   && pUser->septMaster
											) // 为0表示未加入帮会，必须是族长
									{
										if(pUser->charbase.country == charbase.country)
										{
											strncpy(ptCmd->memberName,name,MAX_NAMESIZE);
											ptCmd->memberID = id;
											pUser->sendCmdToMe(ptCmd,sizeof(Cmd::stAddMemberToUnionCmd));
											return true;
										}
										else
										{
											Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "他跟你不是一个国家不能邀请他");
										}
									}
									else
									{
										if (!pUser->septMaster)
										{
											Channel::sendSys(this, 
											Cmd::INFO_TYPE_FAIL, "只能招收族长");
										}
										else
										{
										if (pUser->charbase.unionid == charbase.unionid)
											Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "他已经加入本帮了，无需再次邀请");
										else
											Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "他已经入会需要他退会才能加入你的帮会");
										}
									}
								//}
								//else
								//{
								//	Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "距离太远无法邀请");
								//}
							}
							else
							{
								Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "玩家不在跟前，无法回应邀请");
							}
							return true;
						}
						break;
					case Cmd::ANSWER_YES:
						{
							SceneUser *pUser=scene->getUserByID(ptCmd->memberID);
							if (pUser)
							{
								//FunctionTimes times(28,"ANSWER_YES");
								//charbase.unionid = pUser->charbase.unionid;
								
							//	Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "%s接受了你的邀请加入了帮会",name);
								pUser->removeWarRecord(Cmd::UNION_DARE);
								pUser->removeWarRecord(Cmd::UNION_CITY_DARE);
								pUser->sendNineToMe();
								pUser->sendMeToNine();

								Cmd::Session::t_addUnionMember_SceneSession send;
								send.member.dwSeptID = pUser->charbase.septid;
								send.dwUnionID = pUser->charbase.unionid;
								send.member.dwCharID = charbase.id;																// 会员角色ID
								send.member.wdOccupation = charbase.face;
								send.member.wdPower = 0;
								strncpy(send.member.name,charbase.name,MAX_NAMESIZE);   					// 会员角色名称	
								strncpy(send.member.aliasname,DEFAULTMEMBERALIAS,MAX_NAMESIZE); 	// 会员别名
								SETMEMBERPOWER_WD(send.member.wdPower);                    				// 会员权限
								sessionClient->sendCmd(&send, sizeof(send));
							}
							else
							{
								Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对方已经离开，他放弃了这次邀请");
							}
							return true;
						}
						break;
					case Cmd::ANSWER_NO:
						{
							SceneUser *pUser=scene->getUserByID(ptCmd->memberID);
							if (pUser)
							{
								Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "%s不愿意加入帮会, 拒绝了你的邀请",name);
							}
							return true;
						}
						break;
					default:
						break;
				}
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

