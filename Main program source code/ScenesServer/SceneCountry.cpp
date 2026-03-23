/**
 * \file
 * \version  $Id: SceneCountry.cpp $
 * \author 
 * \date
 * \brief 实现国家相关指令的处理
 *
 * 
 */
#include <math.h>
#include "SceneUser.h"
#include "Scene.h"
#include "RecordClient.h"
#include "Chat.h"
#include "SessionCommand.h"							
#include "SessionClient.h"
#include "SceneManager.h"
#include "ScenesServer.h"


//const unsigned int CHANGE_COUNTRY_NEED_MONEY = 5 * 10000; // 变更国籍，所需银两5锭
//const unsigned int CANCEL_COUNTRY_NEED_MONEY = 5 *  10000; // 取消国籍，所需银两5锭
extern unsigned int cancel_country_need_money;
extern unsigned int is_cancel_country;
//战力排行领取值zhanlijl 2023-05-27 2023-06-02
extern  DWORD zhanli_lqz[10];

/**
  * \brief 一个比较器
  *
  *  用于查找所需要的道具是否存在
  *  
  *
  */
class SessionItemObjectComparet:public UserObjectCompare 
{
public:
	DWORD  dwObjectID;

	bool isIt(zObject *object)
	{
		if (object->base->id == dwObjectID) return true;
		return false;
	}
};

/**
 * \brief 执行国家相关命令
 *
 *
 * \param rev 挑战指令
 * \param cmdLen 消息长度
 * \return 是否成功
 */
bool SceneUser::doCountryCmd(const Cmd::stCountryUserCmd *rev,unsigned int cmdLen)
{
	Zebra::logger->debug("doCountryCmd receive byParam:[%d]", rev->byParam);

	switch (rev->byParam)
	{
		case REQUEST_COUNTRY_POWER_PARA:
			{
				Cmd::stReturnCountryPowerCmd send;
				send.country[0]=0;
				send.country[1]=0;
				for(int i=0; i<13; i++) if (ScenesService::getInstance().countryPower[i]==1) Cmd::set_state(send.country,i);
				this->sendCmdToMe(&send,sizeof(send));
				return true;
			}
			break;
		case Cmd::ANSWER_COUNTRY_DARE_PARA:
			{
				Cmd::stAnswerCountryDareUserCmd* cmd = (Cmd::stAnswerCountryDareUserCmd*)rev;
				if (cmd->byStatus == Cmd::ANSWER_COUNTRY_DARE_YES)
				{
					Cmd::Session::t_changeScene_SceneSession cmd;
					Scene* pScene= SceneManager::getInstance().
						getSceneByID(SceneManager::getInstance().
								buildMapID(this->charbase.country, 137));//边境

					if (this->scene->getRealMapID() == 137)
					{
						return true;
					}

					this->charbase.exploit = this->charbase.exploit+(2*exploit_arg);
					this->charbase.gomaptype = ZoneTypeDef::ZONE_COUNTRY_WAR;

					if (pScene)            
					{//本服                         
						zPos Pos;
						Pos.x = 0;
						Pos.y = 0;     

						this->changeMap(pScene, Pos);
					}       
					else
					{               

						cmd.id = this->id;
						cmd.temp_id = this->tempid;
						cmd.x = 0;              
						cmd.y = 0;
						cmd.map_id = SceneManager::getInstance().buildMapID
							(this->charbase.country, 137);
						//边境

						bzero(cmd.map_file, sizeof(cmd.map_file));
						bzero(cmd.map_name, sizeof(cmd.map_file));
						sessionClient->sendCmd(&cmd, sizeof(cmd));
					}
				}
				return true;
			}
			break;
		case Cmd::CONTRIBUTE_COUNTRY_MATERIAL:
			{
				Cmd::stContributeCountryMaterialCmd* cmd = (Cmd::stContributeCountryMaterialCmd*)rev;
				if (cmd->itemID && cmd->itemID != 0xffffffff)//INVALID_THISID
				{
					zObject * srcobj=this->packs.uom.getObjectByThisID(cmd->itemID);
					if (!srcobj)
					{
						Zebra::logger->trace("%s 国家捐献时未找到该物品 id=%u", 
								this->name, cmd->itemID);
						return true;
					}

					zCountryMaterialB* country_material = srcobj->canContribute();
					if (country_material==NULL)
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你不能捐赠这件物品");
						return true;
					}
						
					Cmd::Session::t_ContributeCountry_SceneSession send;
					send.byType = srcobj->getMaterialKind();
					
					if (send.byType>0)
					{
						if (country_material->dwMaterialKind == 1)	
						{//打造出来的高等级材料
							send.dwValue = (DWORD)pow(5, srcobj->data.upgrade+1) * srcobj->data.dwNum;	
						}
						else
						{//普通材料
							send.dwValue = srcobj->data.dwNum;
						}
						send.byType = 1;
					}
					else
					{
						if (country_material->dwMaterialKind == 3)
						{
							send.dwValue = srcobj->data.dwNum;
							send.byType = 3;
						}
						else
						{
							send.dwValue = (srcobj->base->price*srcobj->data.dwNum)/100;
						}
					}

					if (send.dwValue>0)
					{
						if (send.byType !=3)
						{
						DWORD add_exploit = (DWORD)((((float)(send.dwValue)/10)/5) * exploit_arg);
						
						Zebra::logger->trace("[国家捐献]: 当前功勋:%d 本次获得功勋:%d 当前荣誉值:%d 本次获得荣誉值:%d", 
								this->charbase.exploit, add_exploit,
								this->charbase.honor, (send.dwValue/5));

						this->charbase.exploit = this->charbase.exploit + add_exploit;
						
						BUFFER_CMD(Cmd::stAddUserAndPosMapScreenStateUserCmd ,send2 , zSocket::MAX_USERDATASIZE);
						this->full_t_MapUserDataPosState(send2->data);
						sendCmdToMe(send2,send2->size());
						//又不要加荣誉值了,nb策划
						//this->charbase.honor += send.dwValue/5;
						//this->charbase.maxhonor += send.dwValue/5;
						Cmd::stMainUserDataUserCmd send1;
						this->full_t_MainUserData(send1.data);
						this->sendCmdToMe(&send1, sizeof(send1));
						}
						else if (send.byType == 3)
						{
							this->charbase.exploit = this->charbase.exploit + 
								((send.dwValue*2) * exploit_arg);
							/*Cmd::stMainUserDataUserCmd send1;
							  this->full_t_MainUserData(send1.data);
							  this->sendCmdToMe(&send1, sizeof(send1));*/

							BUFFER_CMD(Cmd::stAddUserAndPosMapScreenStateUserCmd ,send2 , zSocket::MAX_USERDATASIZE);
							this->full_t_MapUserDataPosState(send2->data);
							sendCmdToMe(send2,send2->size());
						}

						send.dwCountry = this->charbase.country;
						sessionClient->sendCmd(&send, sizeof(send));
					}
					
					zObject::logger(srcobj->createid,srcobj->data.qwThisID,srcobj->base->name,
							srcobj->data.dwNum,srcobj->data.dwNum,0,this->id,this->name,0,"国家","捐献到国库",NULL,0,0);

					this->packs.removeObject(srcobj);//移除物品

					this->save(Cmd::Record::OPERATION_WRITEBACK);
					return true;
				}
			}
			break;
		case Cmd::CANCEL_COUNTRY_PARA:
			{
				//Cmd::stCancelCountryCmd * cmd = (Cmd::stCancelCountryCmd *)rev;
				Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "该功能暂时未开放");
				return true;

				SceneManager::CountryMap_iter src_pos = SceneManager::getInstance().
					country_info.find(this->charbase.country);

				if (this->charbase.country == PUBLIC_COUNTRY)	
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "你已经是无国籍人士。");
					return true;
				}

				this->charbase.country = PUBLIC_COUNTRY;
				if (this->scene->getCountryID() == 6)
				{
					deathBackToMapID = (this->charbase.country << 16 ) + 
						this->scene->getCommonCountryBacktoMapID();
				}       
				else
				{
					if(this->charbase.country == 6)
					{
						deathBackToMapID = (this->charbase.country << 16 ) + 
							this->scene->getCommonUserBacktoMapID();                              					     
					}
					else
					{
						deathBackToMapID = (this->charbase.country << 16 ) + 
							this->scene->getForeignerBacktoMapID();
					}
				}             

				// 清除钱庄所有物品
//				this->packs.store.removeAll();	
//				packs.clearPackage(&packs.store);
				packs.execEvery(&packs.store, Type2Type<ClearPack>());

				// 清除所有任务
				this->quest_list.clear(this);

				// 清除功勋和文采值
				this->charbase.grace = 0;
				this->charbase.exploit = 0;
				this->save(Cmd::Record::OPERATION_WRITEBACK);
				Cmd::Session::t_changeCountry_SceneSession send;

				Channel::sendSys(this, Cmd::INFO_TYPE_GAME, 
						"你已经离开 %s，成为无国籍人士，原有社会关系、钱庄物品均清空，希望你在这个国家能愉快的生活",
						src_pos->second.name);

				send.dwUserID = this->id;
				send.dwToCountryID = this->charbase.country;
				sessionClient->sendCmd(&send, sizeof(send));

				return true;
			}
			break;
		case Cmd::APPLY_COUNTRY_PARA:
			{
				Cmd::stApplyCountryCmd* cmd = (Cmd::stApplyCountryCmd*)rev;
				
				Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "该功能暂时未开放");
				return true;

				if (this->charbase.country != PUBLIC_COUNTRY)	
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你必须首先成为无国籍人士，才能申请加入新的国家");
					return true;
				}

				if (cmd->dwToCountryID == PUBLIC_COUNTRY)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"不能申请成为无国籍人士。");
					return true;
				}

				if (is_cancel_country)
				{       
					if (this->packs.checkMoney(cancel_country_need_money)				                                                
						&& this->packs.removeMoney(cancel_country_need_money, "改变国籍"))
					{
						this->charbase.country = cmd->dwToCountryID;

						if (this->scene->getCountryID() == 6)
						{
							deathBackToMapID = (this->charbase.country << 16 ) + 
								this->scene->getCommonCountryBacktoMapID();
						}       
						else
						{
							if(this->charbase.country == 6)
							{
								deathBackToMapID = (this->charbase.country << 16 ) + 
									this->scene->getCommonUserBacktoMapID();                              					     
							}
							else
							{
								deathBackToMapID = (this->charbase.country << 16 ) + 
									this->scene->getForeignerBacktoMapID();
							}
						}            

						this->relive();	
					}
					else
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "金钱不足5锭,不能申请加入新国家");
					}
				}
				else
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "暂时未开放叛国功能。");
				}

				return true;
			}
			break;
		case Cmd::CHANGE_COUNTRY_PARA: //soke 移民（金子）
			{
				Cmd::stChangeCountryCmd* cmd = (Cmd::stChangeCountryCmd*)rev;

				if (this->charbase.country == cmd->dwToCountryID)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你已经是该国国籍。无需变更");
					return true;
				}
				time_t cur_time = time(NULL);

				if ((cur_time - this->lastChangeCountryTime)< 24*60*60*3)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "叛国后三天以后才能再次叛国");
					return true;
				}
				
				if (6 == cmd->dwToCountryID)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "不能变更到中立区");
					return true;
				}

				SceneManager::CountryMap_iter src_pos = SceneManager::getInstance().
					country_info.find(this->charbase.country);

				SceneManager::CountryMap_iter cur_pos = SceneManager::getInstance().
					country_info.find(cmd->dwToCountryID);

				if (src_pos == SceneManager::getInstance().country_info.end() 
						|| cur_pos == SceneManager::getInstance().country_info.end())
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "国家不存在，无法变更");
					return true;
				}

				if (this->charbase.unionid>0 || this->charbase.septid>0)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "请你先退出帮会或家族再变更国籍");
					return true;
				}

				const DWORD change_country_need_gold = 1000000; //soke 移民价格100锭金子

				if (packs.checkGold(change_country_need_gold)
						&& packs.removeGold(change_country_need_gold, "改变国籍"))
				{       
					this->charbase.country = cmd->dwToCountryID;
					this->deathBackToMapID  = (this->charbase.country << 16 ) + 102;

					// 清除钱庄所有物品
					//					this->packs.store.removeAll();	
					//					packs.clearPackage(&packs.store);
					packs.execEvery(&packs.store, Type2Type<ClearPack>());					
					// 清除所有任务
					//this->quest_list.clear(this);

					// 清除功勋和文采值
					this->charbase.grace = 0;
					this->charbase.exploit = 0;
					this->save(Cmd::Record::OPERATION_WRITEBACK);
					Cmd::Session::t_changeCountry_SceneSession send;

					Channel::sendSys(this, Cmd::INFO_TYPE_GAME, 
							"你已经离开 %s 加入了 %s ，原有社会关系、钱庄物品均清空，希望你在这个国家能愉快的生活",
							src_pos->second.name, cur_pos->second.name);

					send.dwUserID = this->id;
					send.dwToCountryID = cmd->dwToCountryID;
					sessionClient->sendCmd(&send, sizeof(send));
					this->lastChangeCountryTime = cur_time;
				}
				else
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "金子不足100锭, 不能变更国籍");
				//	Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "金子不足50两, 不能变更国籍");
				}

				return true;
			}
			break;
		case Cmd::CHANGE_COUNTRY_SALARY_PARA: //soke 移民（工资）
			{
				Cmd::stChangeCountrySalaryCmd* cmd = (Cmd::stChangeCountrySalaryCmd*)rev;


				struct tm tv1;
				time_t timValue = time(NULL);
				zRTime::getLocalTime(tv1, timValue);
		
				if (!(tv1.tm_hour==23 && tv1.tm_min >= 40 && tv1.tm_min <= 59))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "当前不是移民开放时间");
					return false;
				}
				if (this->charbase.country == cmd->dwToCountryID)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您已经是该国国籍。无需变更");
					return true;
				}
				time_t cur_time = time(NULL);

				if ((cur_time - this->lastChangeCountryTime)< 24*60*60*3)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "国籍更改后，三天之内不得再次更改");
					return true;
				}
				
				if (6 == cmd->dwToCountryID)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "不能变更到中立区");
					return true;
				}

				SceneManager::CountryMap_iter src_pos = SceneManager::getInstance().
					country_info.find(this->charbase.country);

				SceneManager::CountryMap_iter cur_pos = SceneManager::getInstance().
					country_info.find(cmd->dwToCountryID);

				if (src_pos == SceneManager::getInstance().country_info.end() 
						|| cur_pos == SceneManager::getInstance().country_info.end())
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "国家不存在，无法变更");
					return true;
				}

				if (this->charbase.unionid>0 || this->charbase.septid>0)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "请您先退出帮会或家族再变更国籍");
					return true;
				}

				const DWORD change_country_need_salary = 100000;

				if (packs.checkSalary(change_country_need_salary) && packs.removeSalary(change_country_need_salary, "改变国籍"))
				{       
					this->charbase.country = cmd->dwToCountryID;
					this->deathBackToMapID  = (this->charbase.country << 16 ) + 102;

					// 清除钱庄所有物品
					// this->packs.store.removeAll();	
					// packs.clearPackage(&packs.store);
					packs.execEvery(&packs.store, Type2Type<ClearPack>());					
					// 清除所有任务
					//this->quest_list.clear(this);

					// 清除功勋和文采值
					this->charbase.grace = 0;
					this->charbase.exploit = 0;
					this->save(Cmd::Record::OPERATION_WRITEBACK);
					Cmd::Session::t_changeCountry_SceneSession send;

					Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "您已经离开 %s 加入了 %s ，原有社会关系、钱庄物品均清空，希望您在这个国家能愉快的生活",src_pos->second.name, cur_pos->second.name);

					send.dwUserID = this->id;
					send.dwToCountryID = cmd->dwToCountryID;
					sessionClient->sendCmd(&send, sizeof(send));
					this->lastChangeCountryTime = cur_time;
				}
				else
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "工资不足10锭, 不能变更国籍");
				}

				return true;
			}
			break;
		case Cmd::ADD_COUNTRY_SALARY_PARA: //soke 领取工资
			{
				if (this->charbase.level < 60)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "不能领取工资，等你到60级以后再来领吧!");
					return false;
				}
				if (this->charbase.honor < 400)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，您的荣誉点不足400，不能领取工资!");
					return false;
				}

				time_t cur_time = time(NULL);

				if ((cur_time - this->lastAddCountryTime)< 12*60*60)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，您已领过或者您上次累计领取时长不足12小时，不能领取工资!");
					return true;
				}

				if (this->charbase.level >= 60)
				{       
					// 清除功勋和文采值
				//	this->charbase.grace = 0;
					this->charbase.honor -= 400;

	                int level2 = (charbase.level * charbase.level);
	              //  DWORD onlinetime2 = (charbase.onlinetime)/36;
				  //  int level3 = (charbase.level * charbase.level)/100;
				  //  DWORD onlinetime3 = (charbase.onlinetime)/3600;

					DWORD gold = level2;
		            packs.addSalary2(gold,"领取每日工资");

					this->save(Cmd::Record::OPERATION_WRITEBACK); //立刻存档

					if (level2/10000 > 0)
					{
					   Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "恭喜您领到了%d锭%d两的工资, 点击“高级杂货”按钮,你就可以看到已经领取的“工资”,并购买物品了．",level2/10000, (level2%10000)/100);
					}
					else
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "恭喜您领到了%d两的工资, 点击“高级杂货”按钮,你就可以看到已经领取的“工资”,并购买物品了．", (level2%10000)/100);
                    }
					this->lastAddCountryTime = cur_time;
				}

				return true;
			}
			break;
		case Cmd::ADD_COUNTRY_LOGIN_PARA: //桃子 领取每日登陆奖励
			{
				if (this->charbase.loginCount>0)
		        {
			        Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您已经领取了今日的登陆奖励");
			        return false;
		        }

				charbase.loginCount++;
				Gm::fetch(this,"id=123123 num=1000 bind=1");
				Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取登陆奖励成功,获得充值点1000点");
				Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日登陆随机奖励,获得充值点1000点！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
				   
			// 	int index = zMisc::randBetween(1,7);

			//     switch (index)
			//     {
			//        case 1:
			// 	   {
			// 	      charbase.loginCount++;
			// 		  Gm::fetch(this,"id=50003 num=100 bind=1");
			// 		  Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取登陆随机奖励成功,获得银子100锭");
			// 		  Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日登陆随机奖励,获得银子100锭！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	   }
			// 	   break;	
			//        case 2:
			// 	   {
			// 		  charbase.loginCount++;
			// 		  Gm::fetch(this,"id=1453 num=1 bind=1");
			// 		  Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取登陆随机奖励成功,获得千锭工资袋1个！");
			// 		  Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日登陆随机奖励,获得千锭工资袋1个！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	   }
			// 	   break;
			// 	   case 3:
			// 	   {
			// 	      charbase.loginCount++;
			// 		  Gm::fetch(this,"id=799 num=200 bind=1");
			// 		  Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取登陆随机奖励成功,获得无暇升级宝石200个！");
			// 		  Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日登陆随机奖励,获得无暇升级宝石200个！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	   }
			// 	   break;
			// 	   case 4:
			// 	   {
			// 	      charbase.loginCount++;
			// 		  Gm::fetch(this,"id=1402 num=50 bind=1");
			// 		  Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取登陆随机奖励成功,获得金子50锭！");
			// 		  Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日登陆随机奖励,获得金子50锭！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	   }
			// 	   break;
			// 	   case 5:
			// 	   {
			// 	      charbase.loginCount++;
			// 		  Gm::fetch(this,"id=50012 num=10 bind=1");
			// 		  Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取登陆随机奖励成功,获得图谱碎片10个！");
			// 		  Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日登陆随机奖励,获得图谱碎片10个！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	   }
			// 	   break;
			// 	   case 6:
			// 	   {
			// 	      charbase.loginCount++;
			// 		  Gm::fetch(this,"id=3540 num=5 bind=1");
			// 		  Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取登陆随机奖励成功,获得魂魄石5个！");
			// 		  Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日登陆随机奖励,获得魂魄石5个！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	   }
			// 	   break;
			// 	   case 7:
			// 	   {
			// 	      charbase.loginCount++;
			// 		  Gm::fetch(this,"id=50000 num=5 bind=1");
			// 		  Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取登陆随机奖励成功,获得补天碎片5个！");
			// 		  Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日登陆随机奖励,获得补天碎片5个！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	   }
			// 	   break;
            //   }
				return true;
			}
			break;
			case Cmd::ADD_COUNTRY_CHOU_PARA: //soke 金山抽奖
				{
					SessionItemObjectComparet found;
					found.dwObjectID = 3807;
					zObject *itemobj = this->packs.uom.getObject(found);// 查找道具
					if (itemobj)
					{
						if(itemobj->data.dwNum>1) //soke 大于9朵直接-9
						{ 
							itemobj->data.dwNum--;
							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID=itemobj->data.qwThisID;
							send.dwNum=itemobj->data.dwNum;
							sendCmdToMe(&send,sizeof(send));
						}
						else
						{ 
							this->packs.removeObject(itemobj);
						}
						
						int index = zMisc::randBetween(1,7);

						switch (index)
						{
						case 1:
							{
								Gm::fetch(this,"id=967 num=2 bind=1");
								Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "金山抽奖获得金符2枚");
								Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s在金山抽奖获得金符2枚",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
							}
							break;	
						case 2:
							{
								Gm::fetch(this,"id=968 num=2 bind=1");
								Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "金山抽奖获得玉符2枚");
								Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s在金山抽奖获得玉符2枚",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
							}
							break;
						case 3:
							{
								//int addgold = 100000;
								Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "金山抽奖获得金子10锭");
								Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s在金山抽奖获得金子10锭",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
							}
							break;
						case 4:
							{
								 Gm::fetch(this,"id=679 level=3 num=2 bind=1");
								Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "金山抽奖获得四等天魔石2颗");
								Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s在金山抽奖获得四等天魔石2颗",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
							}
							break;
						case 5:
							{
								Gm::fetch(this,"id=1140 num=2 bind=1");
								Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "金山抽奖获得龙星激活石2颗");
								Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s在金山抽奖获得龙星激活石2颗",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
							}
							break;
						case 6:
							{
								Gm::fetch(this,"id=1153 num=5 bind=1");
								Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "金山抽奖获得龙星升级宝石5颗");
								Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s在金山抽奖获得龙星升级宝石5颗",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
							}
							break;
						case 7:
							{
								Gm::fetch(this,"id=3812 num=2 bind=1");
								Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "金山抽奖获得神器碎片2颗");
								Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s在金山抽奖获得神器碎片2颗",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
							}
							break;
						}
					}
					else
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "没物品还想抽奖？");
						return false;
					}
					//return true;
				}
				break;
		    // case Cmd::ADD_COUNTRY_VIP_PARA: //soke 领取每日VIP奖励
			// {
			// 	if (this->charbase.svipCount>0)
		    //     {
			//         Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您已经领取了今日的vip福利");
			//         return false;
		    //     }
			// 	if (charbase.charvip<1)
		    //     {
			// 		Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起！你还不是VIP玩家啊！");
		    //     }
			// 	if (charbase.charvip>0 && charbase.charvip <= 10)
		    //     {
			//         charbase.svipCount++;
            //         Gm::fetch(this,"id=202321 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip1福利成功,获得vip1礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip1福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
		    //     }
			// 	else if (charbase.charvip>10 && charbase.charvip <= 20)
		    //     {
			//         charbase.svipCount++;
            //         Gm::fetch(this,"id=202322 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip2福利成功,获得vip2礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip2福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
		    //     }
			// 	else if (charbase.charvip>20 && charbase.charvip <= 30)
		    //     {
			//         charbase.svipCount++;
            //         Gm::fetch(this,"id=202323 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip3福利成功,获得vip3礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip3福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
		    //     }
			// 	else if (charbase.charvip>30 && charbase.charvip <= 40)
		    //     {
			//         charbase.svipCount++;
            //         Gm::fetch(this,"id=202324 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip4福利成功,获得vip4礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip4福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
		    //     }
			// 	else if(charbase.charvip > 40 && charbase.charvip<=50)
		    //     {
			//         charbase.svipCount++;
            //         Gm::fetch(this,"id=202325 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip5福利成功,获得vip5礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip5福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );		
		    //     }
			// 	else if(charbase.charvip > 50 && charbase.charvip<=60)
			// 	{
			// 		charbase.svipCount++;
            //         Gm::fetch(this,"id=202326 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip6福利成功,获得vip6礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip6福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	}
			// 	else if(charbase.charvip > 60 && charbase.charvip<=70)
			// 	{
			// 		charbase.svipCount++;
            //         Gm::fetch(this,"id=202327 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip7福利成功,获得vip7礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip7福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	}
			// 	else if(charbase.charvip > 70 && charbase.charvip<=80)
			// 	{
			// 		charbase.svipCount++;
            //         Gm::fetch(this,"id=202328 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip8福利成功,获得vip8礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip8福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	}
			// 	else if(charbase.charvip > 80 && charbase.charvip<=90)
			// 	{
			// 		charbase.svipCount++;
            //         Gm::fetch(this,"id=202329 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip9福利成功,获得vip9礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip9福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	}
			// 	else if(charbase.charvip > 90 && charbase.charvip<=110) //修改上限
			// 	{
			// 		charbase.svipCount++;
            //         Gm::fetch(this,"id=202330 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip10福利成功,获得vip10礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip10福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	}
			// 	else if(charbase.charvip > 110 && charbase.charvip<=130)
			// 	{
			// 		charbase.svipCount++;
            //         Gm::fetch(this,"id=202331 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip11福利成功,获得vip11礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip11福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	}
			// 	else if(charbase.charvip > 130 && charbase.charvip<=150)
			// 	{
			// 		charbase.svipCount++;
            //         Gm::fetch(this,"id=202332 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip12福利成功,获得vip12礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip12福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	}
			// 	else if(charbase.charvip > 150 && charbase.charvip<=170)
			// 	{
			// 		charbase.svipCount++;
            //         Gm::fetch(this,"id=202333 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip13福利成功,获得vip13礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip13福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	}
			// 	else if(charbase.charvip > 170 && charbase.charvip<=190)
			// 	{
			// 		charbase.svipCount++;
            //         Gm::fetch(this,"id=202334 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip14福利成功,获得vip14礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip14福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	}
			// 	else if(charbase.charvip > 190)
			// 	{
			// 		charbase.svipCount++;
            //         Gm::fetch(this,"id=202335 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip15福利成功,获得vip15礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip15福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	}
			// 	else if(charbase.charvip > 150 && charbase.charvip<=160)
			// 	{
			// 		charbase.svipCount++;
            //         Gm::fetch(this,"id=202336 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip16福利成功,获得vip16礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip16福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	}
			// 	else if(charbase.charvip > 160 && charbase.charvip<=170)
			// 	{
			// 		charbase.svipCount++;
            //         Gm::fetch(this,"id=202337 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip17福利成功,获得vip17礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip17福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	}
			// 	else if(charbase.charvip > 170 && charbase.charvip<=180)
			// 	{
			// 		charbase.svipCount++;
            //         Gm::fetch(this,"id=202338 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip18福利成功,获得vip18礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip18福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	}
			// 	else if(charbase.charvip > 180 && charbase.charvip<=190)
			// 	{
			// 		charbase.svipCount++;
            //         Gm::fetch(this,"id=202339 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip19福利成功,获得vip19礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip19福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	}
			// 	else if(charbase.charvip > 190)
			// 	{
			// 		charbase.svipCount++;
            //         Gm::fetch(this,"id=202340 num=1 bind=1");

			// 		Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "领取vip20福利成功,获得vip20礼包1个！");
			// 		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s领取今日的vip20福利",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );	
			// 	}
			// 	return true;
			// }
			// break;
		default:
			break;
	}

	return false;
}

