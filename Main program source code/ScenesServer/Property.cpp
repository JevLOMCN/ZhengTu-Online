#include <math.h>
#include <sstream>
#include "SceneUser.h"
#include "Scene.h"
#include "Chat.h"
#include "QuestEvent.h"
#include "QuestTable.h"
#include "RebuildObject.h"
#include "SessionClient.h"
#include "Command.h"
#include "SessionCommand.h"
#include "SceneManager.h"
#include "ScenesServer.h"
#include "script.h"
#include "Horse.h"
#include "fjconfig.h"
/**
  * \brief 一个比较器
  *
  *  用于查找送花所需要的道具是否存在
  *  
  *
  */
class FObjectCompare:public UserObjectCompare 
{
	public:
		DWORD  dwObjectID;
		DWORD  dwNum;

		bool isIt(zObject *object)
		{
			if (object->data.dwObjectID == dwObjectID && object->data.dwNum > 8) return true;
			return false;
		}
};

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
  * \brief 一个比较器
  *
  *  用于查找送花99所需要的道具是否存在
  *  
  *
  */
class FObjectCompare99:public UserObjectCompare 
{
	public:
		DWORD  dwObjectID;
		DWORD  dwNum;

		bool isIt(zObject *object)
		{
			if (object->data.dwObjectID == dwObjectID && object->data.dwNum > 98) return true;
			return false;
		}
};

/**
  * \brief 一个比较器
  *
  *  用于查找送花365所需要的道具是否存在
  *  
  *
  */
class FObjectCompare365:public UserObjectCompare 
{
	public:
		DWORD  dwObjectID;
		DWORD  dwNum;

		bool isIt(zObject *object)
		{
			if (object->data.dwObjectID == dwObjectID && object->data.dwNum > 364) return true;
			return false;
		}
};

/**
  * \brief 一个比较器
  *
  *  用于查找送花999所需要的道具是否存在
  *  
  *
  */
class FObjectCompare999:public UserObjectCompare 
{
	public:
		DWORD  dwObjectID;
		DWORD  dwNum;

		bool isIt(zObject *object)
		{
			if (object->data.dwObjectID == dwObjectID && object->data.dwNum > 998) return true;
			return false;
		}
};

/**     
 * \brief  处理stPropertyUserCmd指令
 *
 *
 * 处理stPropertyUserCmd指令
 *      
 * \param rev: 接受到的指令内容
 * \param cmdLen: 接受到的指令长度
 * \return 处理指令成功返回真,否则返回false
 */     	
bool SceneUser::doPropertyCmd(const Cmd::stPropertyUserCmd *rev,unsigned int cmdLen)
{
	switch(rev->byParam)
	{
			//soke 包裹一键整理 交换（移动）用户物品，多次移动
	    case SWAP_ALL_USER_OBJECT_PROPERTY_USERCMD_PARAMETER:
		{
			Cmd::stSwapAllObjectPropertyUserCmd *rwap =(Cmd::stSwapAllObjectPropertyUserCmd *)rev;
			if (rwap->nNum>99||rwap->nNum<1)
			{
				return false;
			}
			if(this->packs.main.space() < 2)
			{
			   return false;
			}
			for (DWORD i=0;i<rwap->nNum;i++)
			{
				zObject *srcobj=packs.uom.getObjectByThisID(rwap->qwThisID[i]);
				if (srcobj)
				{
					
					Cmd::stSwapObjectPropertyUserCmd swap;
					swap.qwThisID = rwap->qwThisID[i];
					swap.dst = rwap->dst[i];
					stObjectLocation org=srcobj->data.pos;
					if(org != swap.dst && packs.moveObject(this,srcobj,swap.dst))
					{
						//向客户端发送指令指明新的两个地方的物品
						if(swap.dst.loc()!=Cmd::OBJECTCELLTYPE_NONE)
							sendCmdToMe(&swap,sizeof(Cmd::stSwapObjectPropertyUserCmd));
					}
					srcobj->data.pos = rwap->dst[i];

				}
			}
		}
		break;
	//soke 随身改造 升星
	case EQUIPMENTLEVELUP_PROPERTY_USERCMD_PARAMETER:
		{
			Cmd::stEquipmentLevelUpPropertyUserCmd *swap =(Cmd::stEquipmentLevelUpPropertyUserCmd *)rev;
			if (swap->byType==0)//升星
			{
				zObject *srcobj=packs.uom.getObjectByThisID(swap->qwThisID);
				if(srcobj)
				{
					Upgrade::upgrade(*this,srcobj,100);
					zObject* ob = zObject::create(srcobj);
					ob->data.pos = srcobj->data.pos;
					this->packs.removeObject(srcobj);
					if ( ob ) 
					{	
						if (packs.addObject(ob, false)) 
						{			
							Base::add_object(*this, ob, false);
							OnOther event(201);
							EventTable::instance().execute(*this, event);
						}
					}
				}
				else
				{
					return false;
				}
			}

			if (swap->byType==1)//打孔
			{
				zObject *srcobj=packs.uom.getObjectByThisID(swap->qwThisID);
				if(srcobj)
				{
					int i=0;
					for (i=0;i<6;i++)
					{
						if (srcobj->data.socket[i]==(DWORD)0)
						{
							break;
						}
					}
					Hole::add_hole(srcobj,i);
					zObject* ob = zObject::create(srcobj);
					ob->data.pos = srcobj->data.pos;
					this->packs.removeObject(srcobj);
					if ( ob ) 
					{	
						if (packs.addObject(ob, false)) 
						{			
							Base::add_object(*this, ob, false);
							OnOther event(202);
							EventTable::instance().execute(*this, event);
						}
					}
				}
				else
				{
					return false;
				}
			}

			if (swap->byType==2)//签名
			{
				zObject *srcobj=packs.uom.getObjectByThisID(swap->qwThisID);
				if(srcobj)
				{
					if ( srcobj->data.maker[0]=='\0') 
					{
						strncpy(srcobj->data.maker, charbase.name, MAX_NAMESIZE);//打造者名字
					}
					zObject* ob = zObject::create(srcobj);
					ob->data.pos = srcobj->data.pos;
					this->packs.removeObject(srcobj);
					if ( ob ) 
					{	
						if (packs.addObject(ob, false)) 
						{			
							Base::add_object(*this, ob, false);
							OnOther event(203);
							EventTable::instance().execute(*this, event);
						}
					}
				}
				else
				{
					return false;
				}
			}
		}break;
		
		//soke 低级装备换高级装备
		case EXCHANGEEQUIPMENT_PROPERTY_USERCMD_PARAMETER:
		{
			int level=this->charbase.level;
			int face=this->charbase.type;
			int num=0;
			int AllLevel=0;
			if (level<=20)
			{
				num=10;
			}
			if (level>20&&level<=30)
			{
				num=20;
			}
			if (level>30&&level<=40)
			{
				num=30;
			}
			if (level>40&&level<=50)
			{
				num=40;
			}
			if (level>50&&level<=60)
			{
				num=45;
			}
			if (level>60)
			{
				return false;
			}
			Cmd::stExchangeEquipmentPropertyUserCmd *swap =(Cmd::stExchangeEquipmentPropertyUserCmd *)rev;
			for (int i=0;i<num;i++)
			{
				if (swap->qwThisID[i]>0)
				{
					zObject *srcobj=packs.uom.getObjectByThisID(swap->qwThisID[i]);
					if(srcobj)
					{
						//this->reduceObjectNum(srcobj->data.dwObjectID,1);
						if (srcobj->base->needlevel>60)
						{
							return false;
						}
						AllLevel+=srcobj->base->needlevel;
						this->packs.removeObject(srcobj);
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			AllLevel = AllLevel/num;
			int id = this->GetEquipmentID(AllLevel,face);
			if (id>0)
			{
				int n  = zMisc::randBetween(1, 100);
				if (n<=10)
				{
					this->addGreenObjectNum(id,1);
				}
				else
				{
					this->addGoldObjectNum(id,1);
				}
			}
		}
		break;
		case SWAPUSEROBJECT_PROPERTY_USERCMD_PARAMETER:
			{
				Cmd::stSwapObjectPropertyUserCmd *swap=(Cmd::stSwapObjectPropertyUserCmd *)rev;
				//得到原物品
				zObject *srcobj=packs.uom.getObjectByThisID(swap->qwThisID);
				if(srcobj)
				{
					//交易处理
					if(tradeorder.hasBegin() && tradeorder.in_trade(srcobj))
					{
						return true;
					}
				

					if (mask.is_use(srcobj)) 
					{
						return Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "请先解除该蒙面巾!");
					}
					stObjectLocation org=srcobj->data.pos;
					Zebra::logger->trace("[移动物品]%s移动物品%s(%d,%d,%d,%d)->(%d,%u,%d,%d)",name,srcobj->data.strName,
							org.loc(),org.tab(),org.xpos(), org.ypos(),
							swap->dst.loc(),swap->dst.tab(),swap->dst.xpos(),swap->dst.ypos());
					if(org != swap->dst && packs.moveObject(this,srcobj,swap->dst))
					{
						//向客户端发送指令指明新的两个地方的物品
						if(swap->dst.loc()!=Cmd::OBJECTCELLTYPE_NONE)
							sendCmdToMe(swap,sizeof(Cmd::stSwapObjectPropertyUserCmd));

						if(packs.equip.needRecalc/* || recalcBySword(false)*/)
						{
							notifyEquipChange();
							setupCharBase();
							Cmd::stMainUserDataUserCmd  userinfo;
							full_t_MainUserData(userinfo.data);
							sendCmdToMe(&userinfo,sizeof(userinfo));

							sendMeToNine();
							this->reSendData = false;
#ifdef _DEBUGLOG
							Zebra::logger->trace("[移动物品]完毕......");
#endif
						}
					}
					else
						Zebra::logger->trace("[移动物品]%s移动物品%s(%d,%d,%d,%d)->(%d,%u,%d,%d)失败",name,srcobj->data.strName,
								org.loc(),org.tab(),org.xpos(), org.ypos(),
								swap->dst.loc(),swap->dst.tab(),swap->dst.xpos(),swap->dst.ypos());
				}
				else
					Zebra::logger->alarm("[移动物品]%s未找到移动物品 %ld",name,swap->qwThisID);
				return true;
			}
			break;
		case SPLITUSEROBJECT_PROPERTY_USERCMD_PARAMETER:
			{
				Cmd::stSplitObjectPropertyUserCmd *split=(Cmd::stSplitObjectPropertyUserCmd *)rev;
				zObject *srcobj=packs.uom.getObjectByThisID(split->qwThisID);

				if (!srcobj) return true;


				//fix objects which have num can be equiped via this, it's ugly and should be checked at client too
				//NOTICE: this will lead to theses objects can not be equiped, client shouldn't send this cmd , use 
				//stSwapObjectPropertyUserCmd instead when needed.
				if (split->dst.loc() == Cmd::OBJECTCELLTYPE_EQUIP
						|| split->dst.loc() == Cmd::OBJECTCELLTYPE_PET) return true;
				
				//交易处理
				if(tradeorder.hasBegin() && tradeorder.in_trade(srcobj))
				{
					return true;
				}

				if (split->dwNum > srcobj->data.dwNum) {
					return true;
				}

				bool notify = false;

				//其他物品 
				Package *destpack=packs.getPackage(split->dst.loc(),split->dst.tab());
				zObject *destobj=NULL;
				if(destpack && 
					destpack->getObjectByZone(&destobj, split->dst.xpos(), split->dst.ypos()) )
				{					
					if (!destobj) { //split
						destobj = zObject::create(srcobj);
						if(destobj) 	{
							destobj->data.dwNum = split->dwNum;
							destobj->data.pos = split->dst;

							if(packs.addObject(destobj, false)) {
								srcobj->data.dwNum -= split->dwNum;

								//通知客户端
								Cmd::stSplitObjectPropertyUserCmd ret;
								bcopy(split,&ret,sizeof(ret));
								ret.qwNewThisID=destobj->data.qwThisID;
								sendCmdToMe(&ret,sizeof(ret));
								notify = true;
								zObject::logger(destobj->createid,destobj->data.qwThisID,destobj->data.strName,destobj->data.dwNum,destobj->data.dwNum,1,0,NULL,this->id,this->name,"拆分新增",destobj->base,destobj->data.kind,destobj->data.upgrade);
							}
							else {
								zObject::destroy(destobj);
								return true;
							}
							
						}
					} 
					//soke 增加力量的属性 解决不同属性的石头可以叠加的BUG
					else if 
					   (destobj->data.dwObjectID == srcobj->data.dwObjectID && 
						srcobj->base->maxnum>1 &&										
						srcobj->data.dwObjectID==destobj->data.dwObjectID &&  //叠加问题
					    srcobj->data.upgrade==	destobj->data.upgrade &&
						srcobj->data.str==	destobj->data.str &&
						srcobj->data.inte==	destobj->data.inte &&
						srcobj->data.dex==	destobj->data.dex &&
						srcobj->data.spi==	destobj->data.spi &&
						srcobj->data.con==	destobj->data.con &&
						srcobj->data.xstr==	destobj->data.xstr &&
						srcobj->data.xinte==destobj->data.xinte &&
						srcobj->data.xdex==	destobj->data.xdex &&
						srcobj->data.xspi==	destobj->data.xspi &&
						srcobj->data.xcon==	destobj->data.xcon &&
					    srcobj->data.kind== destobj->data.kind && 
                       (srcobj->data.bind==destobj->data.bind || srcobj->data.bind > destobj->data.bind))  //绑定叠加问题绑定跌不绑定还是有问题哦						
						{
						
						if(srcobj->data.bind != destobj->data.bind)
						{
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"物品绑定类型不一致，禁止叠加");
							return false;
						}

						srcobj->data.dwNum -= split->dwNum;
						destobj->data.dwNum += split->dwNum;
                           
						//如果原物品为绑定，目标物品也绑定 
						if(srcobj->data.bind == 1)
						{
							destobj->data.bind = 1;
						}


						if(destobj->data.dwNum>destobj->base->maxnum)
						{
							destobj->data.dwNum=destobj->base->maxnum;
						}

						notify = true;
						
						//通知客户端
					
						Cmd::stSplitObjectPropertyUserCmd ret;
						bcopy(split,&ret,sizeof(ret));
						ret.qwNewThisID=destobj->data.qwThisID;
						sendCmdToMe(&ret,sizeof(ret));


					}
					 else {
					}


					if(srcobj->data.dwNum==0)
					{
						zObject::logger(srcobj->createid,srcobj->data.qwThisID,srcobj->data.strName,srcobj->data.dwNum,destobj->data.dwNum,0,0,NULL,this->id,this->name,"拆分删除",srcobj->base,srcobj->data.kind,srcobj->data.upgrade);
						packs.removeObject(srcobj, false, true);
					}
					else
					{
						zObject::logger(srcobj->createid,srcobj->data.qwThisID,srcobj->data.strName,srcobj->data.dwNum,destobj->data.dwNum,0,0,NULL,this->id,this->name,"被拆分",srcobj->base,srcobj->data.kind,srcobj->data.upgrade);
					}

				}

				return true;
			}
			break;
#if 0
		case UNIONUSEROBJECT_PROPERTY_USERCMD_PARAMETER: //soke 合并物品
			{
				Cmd::stUnionObjectPropertyUserCmd *uobj=(Cmd::stUnionObjectPropertyUserCmd *)rev;
				zObject *srcobj=packs.uom.getObjectByThisID(uobj->qwSrcThisID); //源物品唯一ID
				zObject *destobj=packs.uom.getObjectByThisID(uobj->qwDstThisID); //目标物品唯一ID
				if(srcobj && destobj && srcobj->base->maxnum>1 &&
						srcobj->data.dwObjectID==destobj->data.dwObjectID &&
						srcobj->data.upgrade==destobj->data.upgrade /*&&
						srcobj->data.pos.dwLocation==Cmd::OBJECTCELLTYPE_MOUSE*/)
				{
					if(srcobj->data.dwNum+destobj->data.dwNum>srcobj->base->maxnum)
					{
						srcobj->data.dwNum=srcobj->data.dwNum+destobj->data.dwNum-srcobj->base->maxnum;
						destobj->data.dwNum=srcobj->base->maxnum;
					}
					else
					{
						destobj->data.dwNum+=srcobj->data.dwNum;
						srcobj->data.dwNum=0;
					}
					//交易处理
					if(destobj->data.pos.dwLocation==Cmd::OBJECTCELLTYPE_TRADE && tradeorder.hasBegin())
					{
						tradeorder.rollback();
						SceneUser *an=tradeorder.getAnother();
						if(an)
						{
							an->lock();
							an->tradeorder.rollback();
							Cmd::stRefCountObjectPropertyUserCmd ret;
							ret.qwThisID=destobj->data.qwThisID;
							ret.dwNum=destobj->data.dwNum;
							an->sendCmdToMe(&ret,sizeof(ret));
							an->unlock();
						}
					}

					//通知客户端
					Cmd::stRefCountObjectPropertyUserCmd ret;
					ret.qwThisID=destobj->data.qwThisID;
					ret.dwNum=destobj->data.dwNum;
					sendCmdToMe(&ret,sizeof(ret));
					if(srcobj->data.dwNum==0)
					{
						packs.rmObject(srcobj);
						Cmd::stRemoveObjectPropertyUserCmd rm;
						rm.qwThisID=srcobj->data.qwThisID;
						sendCmdToMe(&rm,sizeof(rm));
						SAFE_DELETE(srcobj);
					}
					else
					{
						ret.qwThisID=srcobj->data.qwThisID;
						ret.dwNum=srcobj->data.dwNum;
						sendCmdToMe(&ret,sizeof(ret));
					}
				}
				return true;
			}
			break;
#endif
		case USEUSEROBJECT_PROPERTY_USERCMD_PARAMETER:
			{
				Cmd::stUseObjectPropertyUserCmd *use=(Cmd::stUseObjectPropertyUserCmd *)rev;
				zObject *srcobj=packs.uom.getObjectByThisID(use->qwThisID);
				//自动使用 增加条件
				if(srcobj && (srcobj->data.pos.loc() ==Cmd::OBJECTCELLTYPE_COMMON || srcobj->data.pos.loc() ==Cmd::OBJECTCELLTYPE_FUJIAN))
				{
					useObject(srcobj);
				}
				return true;
			}
			break;
		case Cmd::SCROLL_MAP_PROPERTY_USERCMD_PARA:
			{
				Cmd::stScrollMapPropertyUserCmd * smp = (Cmd::stScrollMapPropertyUserCmd*)rev;
				zObject *srcobj=packs.uom.getObjectByThisID(smp->qwThisID);
				if(srcobj && (srcobj->base->id==675 || srcobj->base->id==676) && srcobj->data.pos.loc()==Cmd::OBJECTCELLTYPE_COMMON)
				{
					strncpy(srcobj->data.maker, smp->mapname[0].strMapName,MAX_NAMESIZE);
					useObject(srcobj);
				}
				else if(srcobj && srcobj->data.pos.loc()==Cmd::OBJECTCELLTYPE_EQUIP&&srcobj->base->kind == ItemType_Amulet)
				{
					strncpy(srcobj->data.maker, smp->mapname[0].strMapName,MAX_NAMESIZE);
					useAmulet(srcobj);
				}
				return true;
			}
			break;
		case Cmd::SCROLL2_MAP_PROPERTY_USERCMD_PARA:
			{   //竹蜻蜓
				Cmd::stScroll2MapPropertyUserCmd * smp = (Cmd::stScroll2MapPropertyUserCmd*)rev;
				zObject *srcobj=packs.uom.getObjectByThisID(smp->qwThisID);
				if(srcobj && (srcobj->base->id==671 || srcobj->base->id==909) && srcobj->data.pos.loc()==Cmd::OBJECTCELLTYPE_COMMON)
				{
					srcobj->data.durpoint = smp->xPos;
					srcobj->data.dursecond = smp->yPos;
					strncpy(srcobj->data.maker, smp->mapname[0].strMapName,MAX_NAMESIZE);
					useObject(srcobj);
				}
				return true;
			}
			break;
			//请求增值地宫列表
		case Cmd::REQUEST_INC_MAP_PROPERTY_USERCMD_PARA:
			{
				zObject *obj = this->packs.equip.getObjectByEquipPos(Cmd::EQUIPCELLTYPE_ADORN);
				if(obj && obj->base->kind == ItemType_Amulet)
				{
					char buf[1024];
					bzero(buf,sizeof(buf));
					Cmd::stScrollMapPropertyUserCmd *smp = (Cmd::stScrollMapPropertyUserCmd*)buf;
					smp->qwThisID = obj->data.qwThisID;
					constructInPlace(smp);
					smp->size = this->scene->getIncCity((const char*)smp->mapname);
					sendCmdToMe(smp, sizeof(Cmd::stScrollMapPropertyUserCmd) + MAX_NAMESIZE * smp->size);				
				}
				return true;
			}
			break;
		case PICKUPITEM_PROPERTY_USERCMD_PARA:
			{
				Cmd::stPickUpItemPropertyUserCmd *pick=(Cmd::stPickUpItemPropertyUserCmd *)rev;
				zPos p;
				zSceneObject *ret = NULL;
				p.x=pick->x;
				p.y=pick->y;
				ret = scene->getSceneObjectByPos(p);
				if(ret)
				{
					//int ok = 0;
					zObject *o=ret->getObject();
					if(!o)
					{
						return false;
					}
					//无主物品或者在没有组队情况下的物品
					if((o->base->id!=673 || o->base->id!=674) && (!ret->getOwner() || ret->getOwner() == this->id || this->team.IsOurTeam(ret->getOwner())))
					{
						//bool bret = false;
						if(o->base->id==673)//玄天符
						{
							if (guard)
							{
								Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你正在运镖，保护好你的镖车！");
								return true;
							}
							if(o->data.maker[0]=='\0')
							{
								zPos newPos;
								if(!this->scene->backtoCityMap())
								{
									//随机重生区坐标
									if (!this->scene->randzPosByZoneType(ZoneTypeDef::ZONE_RELIVE,newPos)) return true;
									if (!this->goTo(newPos)) return true;
								}
								else
								{
									//在其它地图中寻找城市
									this->scene->changeMap(this , false);
								}
							}
							else
							{
								zPos newPos;
								newPos.x=o->data.durpoint;
								newPos.y=o->data.dursecond;


								SceneManager::CountryMap_iter country_iter = SceneManager::getInstance().country_info.find(scene->getCountryID());
								if(country_iter == SceneManager::getInstance().country_info.end()) {
									//unknow country
									return true;
								}


								std::ostringstream os;
								os << "name=" << o->data.maker;
								os << " pos=" << o->data.durpoint << "," << o->data.dursecond;
								Gm::gomap(this, os.str().c_str());
							}
							return true;
						}
						else if(o->base->id==674)//轩辕符
						{
							if(ret->getOwner() == this->id)
							{
								if (guard)
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你正在运镖，保护好你的镖车！");
									return true;
								}
								if(o->data.maker[0]=='\0')
								{
									zPos newPos;
									if(!this->scene->backtoCityMap())
									{
										//随机重生区坐标
										if (!this->scene->randzPosByZoneType(ZoneTypeDef::ZONE_RELIVE,newPos)) return true;
										if (!this->goTo(newPos)) return true;
									}
									else
									{
										//在其它地图中寻找城市
										this->scene->changeMap(this , false);
									}
								}
								else
								{
									zPos newPos;
									newPos.x=o->data.durpoint;
									newPos.y=o->data.dursecond;


									SceneManager::CountryMap_iter country_iter = SceneManager::getInstance().country_info.find(scene->getCountryID());
									if(country_iter == SceneManager::getInstance().country_info.end()) {
										//unknow country
										return true;
									}


									std::ostringstream os;
									os << "name=" << o->data.maker;
									os << " pos=" << o->data.durpoint << "," << o->data.dursecond;
									Gm::gomap(this, os.str().c_str());
								}
							}
							return true;
						}
					}
					if(!ret->getOwner() || ret->getOwner() == this->id ||(this->team.IsOurTeam(ret->getOwner()) && this->team.isNormalObj()))
					{
						//如果是银子
						/*
						   if(o->base->kind==ItemType_Money)
						   {
						   if(!this->team.IsTeamed())
						   {
						   packs.addMoney(o->data.dwNum,"拣");
						   }
						   else
						   {
						//自己先得到20%
						packs.addMoney((DWORD)(o->data.dwNum * 0.2f),"拣");
						//组队平分银子
						struct TeamMoneyExec :public TeamMemExec
						{
						DWORD num;
						TeamMoneyExec(DWORD n)
						{
						num = n;
						}
						bool exec(TeamMember &member)
						{
						SceneUser *pUser = SceneUserManager::getMe().
						getUserByTempID(member.tempid);
						if(pUser)
						{
						pUser->packs.addMoney(num,"拣");
						return true;
						}
						return false;
						}
						};
						SceneUser *leader = SceneUserManager::getMe().
						getUserByTempID(this->team.getLeader());
						if(leader)
						{
						TeamMoneyExec tm((DWORD)(o->data.dwNum * 0.8f / leader->team.getSize()));
						leader->team.execEveryOne(tm);
						}
						}
						ok |= 1;
						}
						else
						{
						// */
						/*
						   bret = packsaddObject(o,true);
						   if(bret)
						   {
						   Cmd::stAddObjectPropertyUserCmd ao;
						   ao.byActionType=Cmd::EQUIPACTION_OBTAIN;
						   bcopy(&o->data,&ao.object,sizeof(t_Object));
						   sendCmdToMe(&ao,sizeof(ao));
						   }
						   */
						  //恋恋
						o->checkXinBind(); //掉落读取这里的绑定
						Combination2 callback(this, o);
						packs.main.execEvery(callback);
						if (packs.equip.pack(EquipPack::L_PACK) && packs.equip.pack(EquipPack::L_PACK)->can_input()) packs.equip.pack(EquipPack::L_PACK)->execEvery(callback);
						if (packs.equip.pack(EquipPack::R_PACK) && packs.equip.pack(EquipPack::R_PACK)->can_input()) packs.equip.pack(EquipPack::R_PACK)->execEvery(callback);

						bool added = false;
						if (o->data.dwNum) 
						{

							if (this->packs.uom.space(this) >= 1 && packs.addObject(o, true, AUTO_PACK)) 
							{
                                //soke 特定类型需要绑定
                                
								added = true;
								Cmd::stAddObjectPropertyUserCmd status;
								status.byActionType = Cmd::EQUIPACTION_OBTAIN;
								bcopy(&o->data, &status.object, sizeof(t_Object));
								sendCmdToMe(&status, sizeof(status));
							}else {
								Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你的包裹已满");
								Cmd::stAddMapObjectMapScreenUserCmd status;
								status.action = Cmd::OBJECTACTION_DROP;
								status.data.dwMapObjectTempID = o->data.qwThisID;
								status.data.dwObjectID = o->data.dwObjectID;
								strncpy(status.data.pstrName, o->data.strName, MAX_NAMESIZE);
								status.data.x = p.x;
								status.data.y = p.y;
								status.data.wdNumber = o->data.dwNum;
								status.data.wdLevel = o->base->level;
								status.data.upgrade = o->data.upgrade;
								status.data.kind = o->data.kind;
								scene->sendCmdToNine(ret->getPosI(), &status, sizeof(status));	
							}
						}
						
						if (callback.num() || added) {
							OnGet event(o->data.dwObjectID);
							EventTable::instance().execute(*this, event);
							zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,o->data.dwNum,1,this->scene->id,this->scene->name,this->id,this->name,"拣东西",o->base,o->data.kind,o->data.upgrade);
							if (ScriptQuest::get_instance().has(ScriptQuest::OBJ_GET, o->data.dwObjectID)) { 
								char func_name[32];
								sprintf(func_name, "%s_%d", "get", o->data.dwObjectID);
								execute_script_event(this,func_name, o);
							}								
							/*
							Zebra::logger->debug("%s(%u)捡到%s(%u)在(%u,%u)",
									name,id,o->name,o->id,ret->getPos().x,ret->getPos().y);
							// */
						}

						if (added) {
							ret->clear();
						}
						if (!o->data.dwNum || added) {
							scene->removeObject(ret);

							Cmd::stRemoveMapObjectMapScreenUserCmd re;
							re.dwMapObjectTempID=ret->id;
							scene->sendCmdToNine(getPosI(),&re,sizeof(re));

							SAFE_DELETE(ret);
						}

						return true;
						// }
					}
					else
					{
						zObject *o=ret->getObject();
						if(o && o->base->id!=673 && o->base->id!=674)//轩辕符
						{
							Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "对不起,该物品不属于你");
						}
					}
				}
			}
			break;

		case FOUNDITEM_PROPERTY_USERCMD_PARA:
			if(!RebuildObject::instance().make(*this, rev))
			{
				RebuildObject::instance().response(*this, 1, Base::MAKE);
			}
			break;
		case UPGRADEITEM_PROPERTY_USERCMD_PARA:
			if(!RebuildObject::instance().upgrade(*this, rev))
			{
				RebuildObject::instance().response(*this, 1, Base::UPGRADE);
			}
			break;
		case COMPOSEITEM_PROPERTY_USERCMD_PARA:
			if(!RebuildObject::instance().compose(*this, rev))
			{
				RebuildObject::instance().response(*this, 1, Base::COMPOSE);
			}
			break;
		case COMPOSE_SOUL_STONE_PROPERTY_USERCMD_PARA:
			if(!RebuildObject::instance().compose_soul_stone(*this, rev))
			{
				RebuildObject::instance().response(*this, 1, Base::COMPOSE);
			}
			break;
		case HOLE_PROPERTY_USERCMD_PARA:
			if(!RebuildObject::instance().hole(*this, rev))
			{
				RebuildObject::instance().response(*this, 1, Base::HOLE);
			}
			break;
		case ENCHASEL_PROPERTY_USERCMD_PARA:
			if(!RebuildObject::instance().enchance(*this, rev))
			{
				RebuildObject::instance().response(*this, 1, Base::ENCHANCE);
			}
			break;
		case DECOMPOSE_PROPERTY_USERCMD_PARA:
			if(!RebuildObject::instance().decompose(*this, rev))
			{
				RebuildObject::instance().response(*this, 1, Base::DECOMPOSE);
			}
			break;
		//soke 随身改造
         case Cmd::ITEM_RECAST_PROPERTY_USERCMD_PARA:
			if(!RebuildObject::instance().recast(*this,0, rev))
			{
                return true;
			}
			break;
		//soke 装备改造
		case MODIFY_PROPERTY_USERCMD_PARA:
			if(!RebuildObject::instance().modify(*this, rev))
			{
				RebuildObject::instance().response(*this, 1, Base::MODIFY);
			}
			break;
		//soke 装备升级改造
		case MAKELV_PROPERTY_USERCMD_PARA:
			if(!RebuildObject::instance().makelv(*this, rev))
			{
				RebuildObject::instance().response(*this, 1, Base::MAKELV);
			}
			break;

		//更新用户快捷键	
		case Cmd::ACCELKEY_PROPERTY_USERCMD_PARA:
			{
				Cmd::stAccekKeyPropertyUserCmd *acc = (Cmd::stAccekKeyPropertyUserCmd *)rev;
				int len = sizeof(Cmd::stAccekKeyPropertyUserCmd) + acc->accelNum * sizeof(Cmd::stGameAccelKey);
				if(acc->accelNum > 0 && len < 1024)
				{
					memcpy(accelData , rev , len);
				}
				else
				{
					bzero(accelData,sizeof(accelData));
				}
				//Zebra::logger->debug("(%s , %ld)更新用户快捷键成功" , this->name , this->tempid);
				return true;
			}
			break;

		//添加用户技能数据
		case ADDUSERSKILL_PROPERTY_USERCMD_PARA:
			{
				if(!addSkillData((Cmd::stAddUserSkillPropertyUserCmd *)rev))
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "不能再次学习已经学习到技能");
				}
				return true;
			}
			break;
		
		//删除用户技能
		case REMOVEUSERSKILL_PROPERTY_USERCMD_PARAMETER:
			{
				if(removeSkill((Cmd::stRemoveUserSkillPropertyUserCmd *)rev))
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "删除技能失败");
				}
				return true;
			}
			break;

		//技能升级	
		case ADDSKILLPOINT_PROPERTY_USERCMD:
			{
				if(!upgradeSkill(((Cmd::stAddSkillPointPropertyUserCmd *)rev)->dwSkillID))
				{
					Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "不能升级该技能");
				}
				return true;
			}
			break;
		case SYSTEMSETTINGS_PROPERTY_USERCMD_PARA:
			{
				Cmd::stSystemSettingsUserCmd *st = (Cmd::stSystemSettingsUserCmd *)rev;

				//允许组队
				if(isset_state(st->data.bySettings , Cmd::USER_SETTING_TEAM))
				{
					set_state(sysSetting , Cmd::USER_SETTING_TEAM);
					this->team.setOpen(true);
				}
				else
				{
					clear_state(sysSetting , Cmd::USER_SETTING_TEAM);
					this->team.setOpen(false);
				}
				//this->team.setOpen(true);

				bcopy(&st->data.bySettings[1], setting, sizeof(setting));
				bcopy(st->data.dwChatColor, chatColor, sizeof(chatColor));
				/*
				for (int i=Cmd::USER_SETTING_FIREND; i<Cmd::USER_SETTING_CHAT_PRIVATE; i++)
				{
					if(isset_state(st->data.bySettings , i))
						set_state(sysSetting , i);
					else
						clear_state(sysSetting , i);
				}
				*/

#ifdef _XWL_DEBUG
				Zebra::logger->debug("收到系统设置消息:%x %x %x %x,%u %u %u %u %u %u %u %u", st->data.bySettings[0],st->data.bySettings[1],st->data.bySettings[2],st->data.bySettings[3],st->data.dwChatColor[0],st->data.dwChatColor[1],st->data.dwChatColor[2],st->data.dwChatColor[3],st->data.dwChatColor[4],st->data.dwChatColor[5],st->data.dwChatColor[6],st->data.dwChatColor[7]);
#endif
				
				//通知session
				Cmd::Session::t_sysSetting_SceneSession send;
				bcopy(sysSetting, send.sysSetting, sizeof(send.sysSetting));
				strncpy((char *)send.name, name, MAX_NAMESIZE-1);
				sessionClient->sendCmd(&send, sizeof(send));

				//通知网关
				Cmd::Scene::t_sysSetting_GateScene gate_send;
				bcopy(sysSetting, gate_send.sysSetting, sizeof(gate_send.sysSetting));
				gate_send.id=this->id;
				this->gatetask->sendCmd(&gate_send, sizeof(gate_send));


				/*
				 *
				//允许好友
				if(isset_state(st->data.bySettings , Cmd::USER_SETTING_FIREND))
				{
					set_state(sysSetting , Cmd::USER_SETTING_FIREND);
				}
				else
				{
					clear_state(sysSetting , Cmd::USER_SETTING_FIREND);
				}

				//允许交易
				if(isset_state(st->data.bySettings , Cmd::USER_SETTING_TRADE))
				{
					set_state(sysSetting , Cmd::USER_SETTING_TRADE);
				}
				else
				{
					clear_state(sysSetting , Cmd::USER_SETTING_TRADE);
				}

				if(isset_state(st->data.bySettings , Cmd::USER_SETTING_SCHOOL))
				{
					set_state(sysSetting , Cmd::USER_SETTING_SCHOOL);
				}
				else
				{
					clear_state(sysSetting , Cmd::USER_SETTING_SCHOOL);
				}

				if(isset_state(st->data.bySettings , Cmd::USER_SETTING_FAMILY))
				{
					set_state(sysSetting , Cmd::USER_SETTING_FAMILY);
				}
				else
				{
					clear_state(sysSetting , Cmd::USER_SETTING_FAMILY);
				}

				if(isset_state(st->data.bySettings , Cmd::USER_SETTING_UNION))
				{
					set_state(sysSetting , Cmd::USER_SETTING_UNION);
				}
				else
				{
					clear_state(sysSetting , Cmd::USER_SETTING_UNION);
				}

				if(isset_state(st->data.bySettings , Cmd::USER_SETTING_COLOR_NINE))
				{
					set_state(sysSetting , Cmd::USER_SETTING_COLOR_NINE);
				}
				else
				{
					clear_state(sysSetting , Cmd::USER_SETTING_COLOR_NINE);
				}
				*/
				
				return true;
			}
			break;
/*
		//分配五行点数
		case DISTRIBUTEUFIVEPOINT_PROPERTY_USERCMD_PARA:
			{
				Zebra::logger->debug("(%s , %ld)分配五行点数指令" , this->name , this->tempid);
				Cmd::stDistributeFivePointPropertyUserCmd *dis = (Cmd::stDistributeFivePointPropertyUserCmd *)rev;
				if(IsJoin(dis->fiveType) && charbase.points > 0 || (charbase.five == dis->fiveType && charbase.points > 0))
				{
					charbase.fivevalue[dis->fiveType] ++;
					charstate.fivevalue[dis->fiveType] ++;
					int type = dis->fiveType;
				charstate.fivedefence[(type + 1) % 5] = 
					(WORD)sqrt(sqrt((charbase.fivevalue[type] * charbase.fivevalue[type] * charbase.fivevalue[type])));
					if(charbase.fivevalue[type] % 4 == 0)
					{
						type =(type + 3) % 5;
						if(charbase.fivevalue[type] > 0)
						{
							charbase.fivevalue[type] ++;
							charstate.fivevalue[type] ++;
							charstate.fivedefence[(type + 1) % 5] = 
								(WORD)sqrt(sqrt((charbase.fivevalue[type] * charbase.fivevalue[type] * charbase.fivevalue[type])));
							if(charbase.fivevalue[type] % 4 == 0)
							{
								type =(type + 3) % 5;
								charbase.fivevalue[type] ++;
								charstate.fivevalue[type] ++;
								charstate.fivedefence[(type + 1) % 5] = 
									(WORD)sqrt(sqrt((charbase.fivevalue[type] * charbase.fivevalue[type] * charbase.fivevalue[type])));
							}
						}
					}
					charbase.points --;

					//装备改变攻击力预处理
					calPreValue();

					//刷新用户数据
					Cmd::stMainUserDataUserCmd  userinfo;
					full_t_MainUserData(userinfo.data);
					sendCmdToMe(&userinfo,sizeof(userinfo));
					break;
*/
		//分配属性点数
		case DISTRIBUTEUPOINT_PROPERTY_USERCMD_PARA:
			{
				if (charbase.points>0)
				{
					Cmd::stDistributePointPropertyUserCmd *dis = (Cmd::stDistributePointPropertyUserCmd *)rev;
					switch(dis->type)
					{
						case Cmd::PROPERTY_CON:	//体质
						case Cmd::PROPERTY_STR:	//体力
						case Cmd::PROPERTY_DEX:	//敏捷
						case Cmd::PROPERTY_INT:	//智力
						case Cmd::PROPERTY_MEN:	//精神
							{
								charbase.wdProperty[dis->type]++;
								charbase.points--;
								this->setupCharBase();
								//刷新用户数据

								Cmd::stMainUserDataUserCmd  userinfo;
								full_t_MainUserData(userinfo.data);
								sendCmdToMe(&userinfo,sizeof(userinfo));
							}
							break;
						default:
							{
							}
							break;
					}
				}
				return true;
			}
			break;
		//soke 属性自由加点
		case CHANGEUSER_PROPERTY_USERCMD_PARAMETER:
		{
			if (charbase.points>0)
			{
				Cmd::stChangeUserPropertyUserCmd *dis = (Cmd::stChangeUserPropertyUserCmd *)rev;
				switch(dis->dwType)
				{
				case Cmd::PROPERTY_CON:  //体质
				case Cmd::PROPERTY_STR:  //体力
				case Cmd::PROPERTY_DEX:  //敏捷
				case Cmd::PROPERTY_INT:  //智力
				case Cmd::PROPERTY_MEN:  //精神
					{
					   //soke 输入点数小于剩余点数
					   if (dis->dwCount <= charbase.points)
					   {
						  WORD WdProperty =  charbase.wdProperty[dis->dwType];
						
						   charbase.wdProperty[dis->dwType] = WdProperty + dis->dwCount;
						   charbase.points -= dis->dwCount;
						   this->setupCharBase();
						   //刷新用户数据
						   Cmd::stMainUserDataUserCmd  userinfo;
						   full_t_MainUserData(userinfo.data);
						   sendCmdToMe(&userinfo,sizeof(userinfo));
					   }
					   //soke 输入点数越界
					   else if (dis->dwCount > charbase.points)
					   {
						   dis->dwCount = charbase.points;
						   WORD WdProperty =  charbase.wdProperty[dis->dwType];
						
						   charbase.wdProperty[dis->dwType] = WdProperty + charbase.points;
						   charbase.points = 0;
						   this->setupCharBase();
						   //刷新用户数据
						   Cmd::stMainUserDataUserCmd  userinfo;
						   full_t_MainUserData(userinfo.data);
						   sendCmdToMe(&userinfo,sizeof(userinfo));
					   }
					}
					break;
				default:
					{
					}
					break;
				}
			}
			return true;
		}
		break;
		case CLEARPOINT_PROPERTY_USERCMD_PARA:
			{
				Cmd::stClearPointPropertyUserCmd *cmd = (Cmd::stClearPointPropertyUserCmd*)rev;
				switch (cmd->dwItemID)
				{
					case 752:
						if (this->charbase.level<40)
						{
							if (this->reduceObjectNum(752,1)==-1)
							{
								Zebra::logger->trace("角色[%s]使用洗属性点功能失败" , this->name);
								return true;
							}
						}
						else
						{
							Zebra::logger->trace("角色[%s]使用洗属性点功能失败" , this->name);
							return true;
						}
						break;
					case 760:
						if (this->charbase.level>=40)
						{
							if (this->reduceObjectNum(760,1)==-1)
							{
								Zebra::logger->trace("角色[%s]使用洗属性点功能失败" , this->name);
								return true;
							}
						}
						else
						{
							Zebra::logger->trace("角色[%s]使用洗属性点功能失败" , this->name);
							return true;
						}
						break;
					case 756:
						if (this->charbase.level>=40)
						{
							if (this->reduceObjectNum(756,1)==-1)
							{
								Zebra::logger->trace("角色[%s]使用洗属性点功能失败" , this->name);
								return true;
							}
						}
						else
						{
							Zebra::logger->trace("角色[%s]使用洗属性点功能失败" , this->name);
							return true;
						}
						break;
					default:
						{
							Zebra::logger->trace("角色[%s]使用洗属性点功能失败" , this->name);
							return true;
						}
						break;
				}
				charbase.points =charbase.points
						+charbase.wdProperty[0]
						+charbase.wdProperty[1]
						+charbase.wdProperty[2]
						+charbase.wdProperty[3]
						+charbase.wdProperty[4];
				charbase.wdProperty[0]=0;
				charbase.wdProperty[1]=0;
				charbase.wdProperty[2]=0;
				charbase.wdProperty[3]=0;
				charbase.wdProperty[4]=0;
				this->setupCharBase();
				//刷新用户数据
				Cmd::stMainUserDataUserCmd  userinfo;
				full_t_MainUserData(userinfo.data);
				sendCmdToMe(&userinfo,sizeof(userinfo));
				Zebra::logger->trace("角色[%s]使用洗属性点功能成功" , this->name);
				return true;
			}
			break;
		case CLEARPOINT_LIMIT_PROPERTY_USERCMD_PARA:
			{
				Cmd::stClearPointLimitPropertyUserCmd *cmd = (Cmd::stClearPointLimitPropertyUserCmd*)rev;
				zObject *srcobj=packs.uom.getObjectByThisID(cmd->qwThisID);
				if(srcobj && srcobj->data.pos.loc() ==Cmd::OBJECTCELLTYPE_COMMON)
				{
					if (srcobj->base->kind == ItemType_ClearProperty && srcobj->base->id == 755)
					{
						BYTE num = cmd->byProperty%5;
						if (charbase.wdProperty[num]>5)
						{
							charbase.points +=5;
							charbase.wdProperty[num]=charbase.wdProperty[num]-5;
						}
						else
						{
							charbase.points += charbase.wdProperty[num];
							charbase.wdProperty[num]=0;
						}
						this->setupCharBase();
						//刷新用户数据
						Cmd::stMainUserDataUserCmd  userinfo;
						full_t_MainUserData(userinfo.data);
						sendCmdToMe(&userinfo,sizeof(userinfo));

						zObject::logger(srcobj->createid,srcobj->data.qwThisID,srcobj->data.strName,srcobj->data.dwNum,srcobj->data.dwNum,0,this->id,this->name,0,NULL,"用洗5点属性宝石",NULL,0,0);
						Zebra::logger->trace("角色[%s]使用洗5点属性功能成功" , this->name);
						packs.removeObject(srcobj); //notify and delete
					}
				}
				return true;
			}
			break;
		case CLEARUSERSKILLPOINT_PROPERTY_USERCMD_PARAMETER:
			{
				Cmd::stClearUserSkillPointPropertyUserCmd *cmd = (Cmd::stClearUserSkillPointPropertyUserCmd*)rev;
				switch (cmd->dwItemID)
				{
					case 753:
						if (this->charbase.level<40)
						{
							if (this->reduceObjectNum(753,1)==-1)
							{
								Zebra::logger->trace("角色[%s]使用洗技能点功能失败" , this->name);
								return true;
							}
						}
						else
						{
							Zebra::logger->trace("角色[%s]使用洗技能点功能失败" , this->name);
							return true;
						}
						break;
					case 761:
						if (this->charbase.level>=40)
						{
							if (this->reduceObjectNum(761,1)==-1)
							{
								Zebra::logger->trace("角色[%s]使用洗技能点功能失败" , this->name);
								return true;
							}
						}
						else
						{
							Zebra::logger->trace("角色[%s]使用洗技能点功能失败" , this->name);
							return true;
						}
						break;
					case 757:
						if (this->charbase.level>=40)
						{
							if (this->reduceObjectNum(757,1)==-1)
							{
								Zebra::logger->trace("角色[%s]使用洗技能点功能失败" , this->name);
								return true;
							}
						}
						else
						{
							Zebra::logger->trace("角色[%s]使用洗技能点功能失败" , this->name);
							return true;
						}
						break;
					default:
						{
							Zebra::logger->trace("角色[%s]使用洗技能点功能失败" , this->name);
							return true;
						}
						break;
				}
				//soke 转生技能点清洗
				if (this->charbase.round>0)
				{
				   charbase.skillpoint = charbase.level + atoi(Zebra::global["trun_skill_rate"].c_str())*charbase.round;
				   charbase.changejob = 0; //soke 清洗职业
				   usm.clear();
				   Cmd::stClearSkillUserCmd send;
				   sendCmdToMe(&send,sizeof(send));
				   skillValue.init();
				   this->setupCharBase();
				   //刷新用户数据
				   Cmd::stMainUserDataUserCmd  userinfo;
				   full_t_MainUserData(userinfo.data);
				   sendCmdToMe(&userinfo,sizeof(userinfo));
				   Zebra::logger->trace("角色[%s]使用洗技能点功能成功,职业转为：无职业" , this->name);
				   Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"清洗技能点成功，你可以转职了!");
				   return true;
			   }
			   else
			   {
				   charbase.skillpoint = charbase.level;
				   charbase.changejob = 0; //soke 修复不转生清洗职业问题
				   usm.clear();
				   Cmd::stClearSkillUserCmd send;
				   sendCmdToMe(&send,sizeof(send));
				   skillValue.init();
				   this->setupCharBase();
				   //刷新用户数据
				   Cmd::stMainUserDataUserCmd  userinfo;
				   full_t_MainUserData(userinfo.data);
				   sendCmdToMe(&userinfo,sizeof(userinfo));
				   Zebra::logger->trace("角色[%s]使用洗技能点功能成功" , this->name);
				   Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"清洗技能点成功，你可以转职了!");
				   return true;
			   }
			}
			break;
		case BODYCOLOR_PROPERTY_USERCMD_PARA:
			{
				using namespace Cmd;
				stBodyColorPropertyUserCmd *bcp = (stBodyColorPropertyUserCmd*)rev;
				if((bcp->dwBodyColorCustom != charbase.bodyColor) && packs.equip.canChangeColor())
				{
					DWORD cost = 1000;
/*
					zObject *gold=packs.getGold();					
					if(!gold)
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"对不起,你银子不够");
						return true;
					}

					if(cost > gold->data.dwNum)
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"对不起,你银子不够");
						return true;
					}
					gold->data.dwNum -= cost;
					if(gold->data.dwNum==0)
					{
						stRemoveObjectPropertyUserCmd rmgold;
						rmgold.qwThisID=gold->data.qwThisID;
						sendCmdToMe(&rmgold,sizeof(rmgold));
						packs.rmObject(gold);
						SAFE_DELETE(gold);
					}
					else
					{
						stRefCountObjectPropertyUserCmd setgold;
						setgold.qwThisID=gold->data.qwThisID;
						setgold.dwNum=gold->data.dwNum;
						sendCmdToMe(&setgold,sizeof(setgold));
					}
*/
					if (!packs.checkMoney(cost) || !packs.removeMoney(cost,"服装染色")) 
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"对不起,你银子不够");
						return true;
					}
                    Channel::sendMoney(this,Cmd::INFO_TYPE_GAME,cost,"服装染色成功,花费银子");
                    //soke 时装位置改了
					/*if (packs.equip.equip(EquipPack::OTHERS3)  && ( packs.equip.equip(EquipPack::OTHERS3)->base->kind == ItemType_FashionBody || packs.equip.equip(EquipPack::OTHERS3)->base->kind == ItemType_HighFashionBody) )
					{
						packs.equip.equip(EquipPack::OTHERS3)->data.color = bcp->dwBodyColorCustom;
					}
                    else if (packs.equip.equip(EquipPack::OTHERS2)  && ( packs.equip.equip(EquipPack::OTHERS2)->base->kind == ItemType_FashionBody || packs.equip.equip(EquipPack::OTHERS2)->base->kind == ItemType_HighFashionBody) )
					{
						packs.equip.equip(EquipPack::OTHERS2)->data.color = bcp->dwBodyColorCustom;
					}*/
					//soke  时装染色
					if (packs.equip.equip(EquipPack::OTHERS4)  && ( packs.equip.equip(EquipPack::OTHERS4)->base->kind == ItemType_FashionBody || packs.equip.equip(EquipPack::OTHERS4)->base->kind == ItemType_HighFashionBody || packs.equip.equip(EquipPack::OTHERS4)->base->kind == ItemType_XHighFashionBody) )
					{
						packs.equip.equip(EquipPack::OTHERS4)->data.color = bcp->dwBodyColorCustom;
					}
					else if (packs.equip.equip(EquipPack::BODY)) 
					{
						packs.equip.equip(EquipPack::BODY)->data.color = bcp->dwBodyColorCustom;
					}
					else
					{
						charbase.bodyColor = bcp->dwBodyColorCustom;
					}
					reSendMyMapData();
				}
				else
				{
					Zebra::logger->iffy("用户(%u,%s,%u)更换不可更换颜色的衣服" , id , name , tempid);
				}
				return true;
			}
			break;
		case FACE_PROPERTY_USERCMD_PARA:
			{
				using namespace Cmd;
				stFacePropertyUserCmd *fp = (stFacePropertyUserCmd*)rev;
				if(fp->dwFace == charbase.face)
				{
					return true;
				}
				DWORD cost=0;
				zHeadListB *base=headlistbm.get(fp->dwFace);
				if(base)
				{
					cost = base->cost;
				}
				else
				{
					return true;
				}
				if (!packs.checkMoney(cost) || !packs.removeMoney(cost,"更换头像")) {
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"对不起,你银子不够");
					return true;
				}
				
				charbase.face = fp->dwFace;
				Channel::sendMoney(this, Cmd::INFO_TYPE_GAME,cost,"头像更换成功,花费银子");
				//Cmd::stAddUserMapScreenUserCmd cmd;
				//full_t_MapUserData(cmd.data);
				//scene->sendCmdToNine(getPosI() , &cmd,sizeof(cmd) , false);
				reSendMyMapData();
				return true;
			}
			break;
		case ONE_KEY_EQUIP_PROPERTY_USERCMD_PARA: //sky 一键换装保存
			{
				using namespace Cmd;
				stOneKeyEquipPropertyUserCmd* equip = (stOneKeyEquipPropertyUserCmd*)rev;
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);
				if(pUser)
				{
				    pUser->packs.m_OneKeyEquip[equip->one_key_equip.index] = equip->one_key_equip;
				    Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"已设置 %s 套装",equip->one_key_equip.sName);
				}		
			}
			break;
		case GET_ONE_KEY_EQUIP_PROPERTY_USERCMD_PARA: //sky 请求一键换装数据
			{

				using namespace Cmd;

				stGetOneKeyEquipPropertyUserCmd* equip = (stGetOneKeyEquipPropertyUserCmd*)rev;
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);

				if(pUser)
				{
					stResponsePropertyUserCmdOnekeyEquip Send;
					for(int i=0;i<10;i++)
					{
						Send.OneKeyEqiup[i] = pUser->packs.m_OneKeyEquip[i];
					}
					sendCmdToMe(&Send,sizeof(Send));
				}
			}
			break;
		case GET_CHENGHAO_PROPERTY_USERCMD_PARA: //sky 请求称号信息
			{
				using namespace Cmd;
				stGetChenghaoPropertyUserCmd *equip = (stGetChenghaoPropertyUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);

				if(pUser)
				{
					stResponsePropertyUserCmdChenghao Send; 
					for(int i=0;i<60;i++)
					{
						Send.Chenghao[i].state=pUser->packs.m_Chenghao[i].state;
					}
					sendCmdToMe(&Send,sizeof(Send));
				}
			}
			break;
		case SETCHENGHAO_USERCMD: //sky 佩戴称号
			{
				using namespace Cmd;
				stSetChenghaoUserCmd *equip = (stSetChenghaoUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);

				if(pUser)
				{
					pUser->charbase.chenghao_select = equip->chenghaoid;
					Cmd::stMainUserDataUserCmd  userinfo;
					full_t_MainUserData(userinfo.data);
					sendCmdToMe(&userinfo,sizeof(userinfo));
				}


			}
			break;
		case SETNOTCHENGHAO_USERCMD: //sky 取消佩戴称号
			{
				using namespace Cmd;
				stSetNotChenghaoUserCmd *equip = (stSetNotChenghaoUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);

				if(pUser)
				{
					pUser->charbase.chenghao_select = 0;
					Cmd::stMainUserDataUserCmd  userinfo;
					full_t_MainUserData(userinfo.data);
					sendCmdToMe(&userinfo,sizeof(userinfo));
				}
			}
			break;
		case GET_SHIZHUANG_PROPERTY_USERCMD_PARA: //sky 请求时装魔盒信息
			{
				using namespace Cmd;
				stGetShizhuangPropertyUserCmd *equip = (stGetShizhuangPropertyUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);
				if(pUser)
				{
					stResponsePropertyUserCmdShizhuang Send; 
					DWORD exp =0 ;
					DWORD level = 0;
					if ( 1 == pUser->charbase.type ) //性别判断
                    {
						for(int i=0;i<300;i++)
						{
							if(pUser->packs.m_Shizhuang[i].state==1)
							{
								exp+=fjconfig::getInstance().shizhuangnanlist[i].exp;
								for(int j=0;j<=packs.m_Shizhuang[i].level;j++)
								{
									exp += fjconfig::getInstance().shizhuanglevellist[j].exp;
								}
							}
							Send.Shizhuang[i].level=pUser->packs.m_Shizhuang[i].level;
							Send.Shizhuang[i].state=pUser->packs.m_Shizhuang[i].state;
							if(fjconfig::getInstance().shizhuangnanlist[i].name)
							{
								bcopy(fjconfig::getInstance().shizhuangnanlist[i].name,Send.Shizhuang[i].name,sizeof(Send.Shizhuang[i].name));
								Send.Shizhuang[i].bodyNum=fjconfig::getInstance().shizhuangnanlist[i].bodyNum;
							}
							else{
								bcopy("",Send.Shizhuang[i].name,sizeof(Send.Shizhuang[i].name));
								Send.Shizhuang[i].bodyNum=0;
							}
						}
					}
					else{
						for(int i=0;i<300;i++)
						{
							if(pUser->packs.m_Shizhuang[i].state==1)
							{
								exp+=fjconfig::getInstance().shizhuangnvlist[i].exp;
								for(int j=0;j<=packs.m_Shizhuang[i].level;j++)
								{
									exp += fjconfig::getInstance().shizhuanglevellist[j].exp;
								}
								
							}
							Send.Shizhuang[i].level=pUser->packs.m_Shizhuang[i].level;
							Send.Shizhuang[i].state=pUser->packs.m_Shizhuang[i].state;
							if(fjconfig::getInstance().shizhuangnvlist[i].name)
							{
								bcopy(fjconfig::getInstance().shizhuangnvlist[i].name,Send.Shizhuang[i].name,sizeof(Send.Shizhuang[i].name));
								Send.Shizhuang[i].bodyNum=fjconfig::getInstance().shizhuangnvlist[i].bodyNum;
							}
							else{
								bcopy("",Send.Shizhuang[i].name,sizeof(Send.Shizhuang[i].name));
								Send.Shizhuang[i].bodyNum=0;
							}
						}
					}

					for (int i = 0; i < 100; i++)
					{
						if (pUser->packs.m_Pifeng[i].state == 1)
						{
							exp += fjconfig::getInstance().pifenglist[i].exp;
							for (int j = 0; j <= packs.m_Pifeng[i].level; j++)
							{
								exp += fjconfig::getInstance().pifenglevellist[j].exp;
							}
						}
						Send.Pifeng[i].level = pUser->packs.m_Pifeng[i].level;
						Send.Pifeng[i].state = pUser->packs.m_Pifeng[i].state;
						if (fjconfig::getInstance().pifenglist[i].name)
						{
							bcopy(fjconfig::getInstance().pifenglist[i].name, Send.Pifeng[i].name, sizeof(Send.Pifeng[i].name));
							Send.Pifeng[i].itemNum = fjconfig::getInstance().pifenglist[i].itemNum;
						}
						else
						{
							bcopy("", Send.Pifeng[i].name, sizeof(Send.Pifeng[i].name));
							Send.Pifeng[i].itemNum = 0;
						}
					}

					for (int i = 0; i < 100; i++)
					{
						if (pUser->packs.m_Chibang[i].state == 1)
						{
							exp += fjconfig::getInstance().chibanglist[i].exp;
							for (int j = 0; j <= packs.m_Chibang[i].level; j++)
							{
								exp += fjconfig::getInstance().chibanglevellist[j].exp;
							}
						}
						Send.Chibang[i].level = pUser->packs.m_Chibang[i].level;
						Send.Chibang[i].state = pUser->packs.m_Chibang[i].state;
						if (fjconfig::getInstance().chibanglist[i].name)
						{
							bcopy(fjconfig::getInstance().chibanglist[i].name, Send.Chibang[i].name, sizeof(Send.Chibang[i].name));
							Send.Chibang[i].itemNum = fjconfig::getInstance().chibanglist[i].itemNum;
						}
						else
						{
							bcopy("", Send.Chibang[i].name, sizeof(Send.Chibang[i].name));
							Send.Chibang[i].itemNum = 0;
						}
					}


					for (int i = 0; i < 100; i++)
					{
						if (pUser->packs.m_Zuoqi[i].state == 1)
						{
							exp += fjconfig::getInstance().zuoqi2list[i].exp;
							
						}
						Send.Zuoqi[i].level = pUser->packs.m_Zuoqi[i].level;
						Send.Zuoqi[i].state = pUser->packs.m_Zuoqi[i].state;
						if (fjconfig::getInstance().zuoqi2list[i].name)
						{
							bcopy(fjconfig::getInstance().zuoqi2list[i].name, Send.Zuoqi[i].name, sizeof(Send.Zuoqi[i].name));
							Send.Zuoqi[i].npcNum = fjconfig::getInstance().zuoqi2list[i].npcNum;
						}
						else
						{
							bcopy("", Send.Zuoqi[i].name, sizeof(Send.Zuoqi[i].name));
							Send.Zuoqi[i].npcNum = 0;
						}
					}

					//时装魔盒最高等级
					for(int i=0;i<20;i++)
					{
						if(exp >= fjconfig::getInstance().mohelist[i].exp)
						{
							level++;
							
						}
						
					}
					pUser->charbase.mohelevel = level;
					pUser->charbase.moheexp = exp;
					Send.mohelevel = level;
					Send.moheexp = exp;
					Send.mohemaxexp = fjconfig::getInstance().mohelist[level].exp;

					int pDam = 0;
					int mDam = 0;
					int pDef = 0;
					int mDef = 0;
					int hp = 0;
					for(DWORD i=0;i<=pUser->charbase.mohelevel;i++)
					{
						pDam += fjconfig::getInstance().mohelist[i].pDam;
						mDam += fjconfig::getInstance().mohelist[i].mDam;
						pDef += fjconfig::getInstance().mohelist[i].pDef;
						mDef += fjconfig::getInstance().mohelist[i].mDef;
						hp += fjconfig::getInstance().mohelist[i].hp;
					}	
					Send.num1 = pDam;
					Send.num2 = mDam;
					Send.num3 = pDef;
					Send.num4 = mDef;
					Send.num5 = hp;
					Send.shizhuang_select = pUser->charbase.shizhuang_select;
					Send.pifeng_select = pUser->charbase.pifeng_select;
					Send.chibang_select = pUser->charbase.chibang_select;
					Send.zuoqi_select = pUser->charbase.zuoqi_select;
					Send.mohelevel = pUser->charbase.mohelevel;
					Send.moheexp = pUser->charbase.moheexp;
					sendCmdToMe(&Send,sizeof(Send));
				}
			}
			break;
		case SETSHIZHUANG_USERCMD: //sky 佩戴时装
			{
				using namespace Cmd;
				stSetShizhuangUserCmd *equip = (stSetShizhuangUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);
				if(pUser)
				{
					if(pUser->packs.m_Shizhuang[equip->shizhuangid].state!=1)
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"此时装未激活，无法装备");
						return true;
					}
					pUser->charbase.shizhuang_select = equip->shizhuangid;
					DWORD costID =0;
					if ( 1 == pUser->charbase.type ) //性别判断
                    {
						if(fjconfig::getInstance().shizhuangnanlist[equip->shizhuangid].costID)
						{
							costID = fjconfig::getInstance().shizhuangnanlist[equip->shizhuangid].costID;

						}
					}
					else{
						if(fjconfig::getInstance().shizhuangnvlist[equip->shizhuangid].costID)
						{
							costID = fjconfig::getInstance().shizhuangnvlist[equip->shizhuangid].costID;
						}
					}
					if(costID!=0)
					{
						zObjectB *base = objectbm.get(costID);
						if(base)
						{
							int hp = fjconfig::getInstance().shizhuanglevellist[pUser->packs.m_Shizhuang[equip->shizhuangid].level].hp;
							int wugong = fjconfig::getInstance().shizhuanglevellist[pUser->packs.m_Shizhuang[equip->shizhuangid].level].wugong;
							int mogong = fjconfig::getInstance().shizhuanglevellist[pUser->packs.m_Shizhuang[equip->shizhuangid].level].mogong;
							base->maxpdamage = wugong;
							base->maxmdamage = mogong;
							base->maxsp = hp;	
							zObject *ob = zObject::create(base,1);
							if (ob)
							{
								zObject *obj = pUser->packs.equip.getObjectByEquipPos(Cmd::EQUIPCELLTYPE_FASHION);
								if(obj)
								{
									pUser->packs.removeObject(obj);
								}
								int shengming = fjconfig::getInstance().shizhuanglevellist[pUser->packs.m_Shizhuang[equip->shizhuangid].level].shengming;
								int fashu = fjconfig::getInstance().shizhuanglevellist[pUser->packs.m_Shizhuang[equip->shizhuangid].level].fashu;
								int bang = 0;
								bang = fjconfig::getInstance().shizhuanglevellist[pUser->packs.m_Shizhuang[equip->shizhuangid].level].bang;	
								ob->data.maxhp = shengming;
								ob->data.maxmp = fashu;
								ob->data.bang = bang;
								ob->data.bind = 1;
								if (pUser->packs.addObject(ob,true, AUTO_PACK))
								{
								
									Cmd::stAddObjectPropertyUserCmd send;
									bcopy(&ob->data,&send.object,sizeof(t_Object));
									pUser->sendCmdToMe(&send,sizeof(send));

									Cmd::stSwapObjectPropertyUserCmd swap;
									swap.qwThisID = ob->data.qwThisID;
									swap.dst = stObjectLocation(Cmd::OBJECTCELLTYPE_EQUIP, 0, 0, 15);;
									if(packs.moveObject(pUser,ob,swap.dst))
									{
										
										sendCmdToMe(&swap,sizeof(Cmd::stSwapObjectPropertyUserCmd));
									}
				
									pUser->notifyEquipChange();
									
									pUser->setupCharBase();
									Cmd::stMainUserDataUserCmd  userinfo;
									full_t_MainUserData(userinfo.data);
									pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
							
									pUser->sendMeToNine();

								}
							}
						}
						
					}
					
				}

			}
			break;
		case SETNOTSHIZHUANG_USERCMD: //sky 取消佩戴时装
			{
				 using namespace Cmd;
				stSetNotShizhuangUserCmd *equip = (stSetNotShizhuangUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);
				if(pUser)
				{
					pUser->charbase.shizhuang_select = 9999;
					zObject *obj = pUser->packs.equip.getObjectByEquipPos(Cmd::EQUIPCELLTYPE_FASHION);
					if(obj)
					{
						pUser->packs.removeObject(obj);
						pUser->notifyEquipChange();
						pUser->setupCharBase();
						Cmd::stMainUserDataUserCmd  userinfo;
						full_t_MainUserData(userinfo.data);
						pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
						pUser->sendMeToNine();
					}			
				}
			}
			break;
		case SETSHIZHUANGJIHUO_USERCMD: //sky 请求激活/升级时装
			{
				using namespace Cmd;
				stSetShizhuangJihuoUserCmd *equip = (stSetShizhuangJihuoUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);
				if(pUser)
				{
					if(pUser->packs.m_Shizhuang[equip->shizhuangid].state == 0)//激活 
					{
						SessionItemObjectComparet found;
						if ( 1 == pUser->charbase.type ) //性别判断
                    	{
							found.dwObjectID = fjconfig::getInstance().shizhuangnanlist[equip->shizhuangid].jihuoID;
						}
						else
						{
							found.dwObjectID = fjconfig::getInstance().shizhuangnanlist[equip->shizhuangid].jihuoID;
						}
						zObject *itemobj = pUser->packs.uom.getObject(found);// 查找道具
						if(itemobj)
						{
							DWORD jihuoNum = fjconfig::getInstance().shizhuangnanlist[equip->shizhuangid].jihuoNum;
							if(itemobj->data.dwNum < jihuoNum)
							{
								Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少激活时装道具【时装布料】");
								return true;
							}
							if (itemobj->data.dwNum > jihuoNum) // soke 大于9朵直接-9
							{
									itemobj->data.dwNum-=jihuoNum;
									Cmd::stRefCountObjectPropertyUserCmd send;
									send.qwThisID = itemobj->data.qwThisID;
									send.dwNum = itemobj->data.dwNum;
									sendCmdToMe(&send, sizeof(send));
							}
							else
							{
								pUser->packs.removeObject(itemobj);
							}
							pUser->packs.m_Shizhuang[equip->shizhuangid].state=1;
							pUser->packs.m_Shizhuang[equip->shizhuangid].level=0;
							Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"时装激活成功");
					 	}
						else{
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少激活时装道具【时装布料】");
						}
					}
					else{//升级
						if(pUser->packs.m_Shizhuang[equip->shizhuangid].level >= 6)
						{
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"此时装已达到最高等级");
							return true;	
						}
						SessionItemObjectComparet found;
						
						found.dwObjectID = fjconfig::getInstance().shizhuanglevellist[pUser->packs.m_Shizhuang[equip->shizhuangid].level].costID;
						
						zObject *itemobj = pUser->packs.uom.getObject(found);// 查找道具
						DWORD jihuoNum = fjconfig::getInstance().shizhuanglevellist[pUser->packs.m_Shizhuang[equip->shizhuangid].level].costNum;
						if(itemobj)
						{
							
							if(itemobj->data.dwNum < jihuoNum)
							{
								char msg[256];
								sprintf(msg,"时装布料不足，当前等级进阶需要 %d个时装布料",jihuoNum);
								Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,msg);
								return true;
							}
							if (itemobj->data.dwNum > jihuoNum) // soke 大于9朵直接-9
							{
									itemobj->data.dwNum-=jihuoNum;
									Cmd::stRefCountObjectPropertyUserCmd send;
									send.qwThisID = itemobj->data.qwThisID;
									send.dwNum = itemobj->data.dwNum;
									sendCmdToMe(&send, sizeof(send));
							}
							else
							{
								pUser->packs.removeObject(itemobj);
							}
							pUser->packs.m_Shizhuang[equip->shizhuangid].state=1;
							pUser->packs.m_Shizhuang[equip->shizhuangid].level++;
							Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"时装进阶成功");
					 	}
						else{
							char msg[256];
							sprintf(msg,"时装布料不足，当前等级进阶需要 %d个时装布料",jihuoNum);
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,msg);
						}
					}
						
				}
			}
			break;
		case SETPIFENG_USERCMD: //sky 佩戴披风
			{
				using namespace Cmd;
				stSetPifengUserCmd *equip = (stSetPifengUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);
				if(pUser)
				{
					if(pUser->packs.m_Pifeng[equip->pifengid].state!=1)
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"此披风未激活，无法装备");
						return true;
					}
					pUser->charbase.pifeng_select = equip->pifengid;
					DWORD costID =0;
					if(fjconfig::getInstance().pifenglist[equip->pifengid].costID)
					{
						costID = fjconfig::getInstance().pifenglist[equip->pifengid].costID;
					}
					if(costID!=0)
					{
						zObjectB *base = objectbm.get(costID);
						if(base)
						{
							int hp = fjconfig::getInstance().pifenglevellist[pUser->packs.m_Pifeng[equip->pifengid].level].hp;
							int wugong = fjconfig::getInstance().pifenglevellist[pUser->packs.m_Pifeng[equip->pifengid].level].wugong;
							int mogong = fjconfig::getInstance().pifenglevellist[pUser->packs.m_Pifeng[equip->pifengid].level].mogong;
							base->maxpdamage = wugong;
							base->maxmdamage = mogong;
							base->maxsp = hp;	
							zObject *ob = zObject::create(base,1);
							if (ob)
							{
								zObject *obj = pUser->packs.equip.getObjectByType(99);
								if(obj)
								{
									pUser->packs.removeObject(obj);
								}
								int shengming = fjconfig::getInstance().pifenglevellist[pUser->packs.m_Pifeng[equip->pifengid].level].shengming;
								int fashu = fjconfig::getInstance().pifenglevellist[pUser->packs.m_Pifeng[equip->pifengid].level].fashu;
								int bang = 0;
								bang = fjconfig::getInstance().pifenglevellist[pUser->packs.m_Pifeng[equip->pifengid].level].bang;	
								ob->data.maxhp = shengming;
								ob->data.maxmp = fashu;
								ob->data.bang = bang;
								ob->data.bind = 1;
								if (pUser->packs.addObject(ob,true, AUTO_PACK))
								{
								
									Cmd::stAddObjectPropertyUserCmd send;
									bcopy(&ob->data,&send.object,sizeof(t_Object));
									pUser->sendCmdToMe(&send,sizeof(send));

									Cmd::stSwapObjectPropertyUserCmd swap;
									swap.qwThisID = ob->data.qwThisID;
									swap.dst = stObjectLocation(Cmd::OBJECTCELLTYPE_EQUIP, 0, 0, 136);;
									if(packs.moveObject(pUser,ob,swap.dst))
									{
										
										sendCmdToMe(&swap,sizeof(Cmd::stSwapObjectPropertyUserCmd));
									}
				
									pUser->notifyEquipChange();
									
									pUser->setupCharBase();
									Cmd::stMainUserDataUserCmd  userinfo;
									full_t_MainUserData(userinfo.data);
									pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
							
									pUser->sendMeToNine();

								}
							}
						}
						
					}
					
				}

			}
			break;
		case SETNOTPIFENG_USERCMD: //sky 取消佩戴披风
			{
				 using namespace Cmd;
				stSetNotPifengUserCmd *equip = (stSetNotPifengUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);
				if(pUser)
				{
					pUser->charbase.pifeng_select = 9999;
					zObject *obj = pUser->packs.equip.getObjectByType(99);
					if(obj)
					{
						pUser->packs.removeObject(obj);
						pUser->notifyEquipChange();
						pUser->setupCharBase();
						Cmd::stMainUserDataUserCmd  userinfo;
						full_t_MainUserData(userinfo.data);
						pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
						pUser->sendMeToNine();
					}			
				}
			}
			break;
		case SETPIFENGJIHUO_USERCMD: //sky 请求激活/升级披风
			{
				using namespace Cmd;
				stSetPifengJihuoUserCmd *equip = (stSetPifengJihuoUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);
				if(pUser)
				{
					if(pUser->packs.m_Pifeng[equip->pifengid].state == 0)//激活 
					{
						SessionItemObjectComparet found;
						found.dwObjectID = fjconfig::getInstance().pifenglist[equip->pifengid].jihuoID;
						zObject *itemobj = pUser->packs.uom.getObject(found);// 查找道具
						if(itemobj)
						{
							DWORD jihuoNum = fjconfig::getInstance().pifenglist[equip->pifengid].jihuoNum;
							if(itemobj->data.dwNum < jihuoNum)
							{
								Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少激活披风道具【披风碎片】");
								return true;
							}
							if (itemobj->data.dwNum > jihuoNum) // soke 大于9朵直接-9
							{
									itemobj->data.dwNum-=jihuoNum;
									Cmd::stRefCountObjectPropertyUserCmd send;
									send.qwThisID = itemobj->data.qwThisID;
									send.dwNum = itemobj->data.dwNum;
									sendCmdToMe(&send, sizeof(send));
							}
							else
							{
								pUser->packs.removeObject(itemobj);
							}
							pUser->packs.m_Pifeng[equip->pifengid].state=1;
							pUser->packs.m_Pifeng[equip->pifengid].level=0;
							Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"披风激活成功");
					 	}
						else{
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少激活披风道具【披风碎片】");
						}
					}
					else{//升级
						if(pUser->packs.m_Pifeng[equip->pifengid].level >= 6)
						{
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"此披风已达到最高等级");
							return true;	
						}
						SessionItemObjectComparet found;
						
						found.dwObjectID = fjconfig::getInstance().pifenglevellist[pUser->packs.m_Pifeng[equip->pifengid].level].costID;
						
						zObject *itemobj = pUser->packs.uom.getObject(found);// 查找道具
						DWORD jihuoNum = fjconfig::getInstance().pifenglevellist[pUser->packs.m_Pifeng[equip->pifengid].level].costNum;
						if(itemobj)
						{
							
							if(itemobj->data.dwNum < jihuoNum)
							{
								char msg[256];
								sprintf(msg,"披风碎片不足，当前等级进阶需要 %d个披风碎片",jihuoNum);
								Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,msg);
								return true;
							}
							if (itemobj->data.dwNum > jihuoNum) // soke 大于9朵直接-9
							{
									itemobj->data.dwNum-=jihuoNum;
									Cmd::stRefCountObjectPropertyUserCmd send;
									send.qwThisID = itemobj->data.qwThisID;
									send.dwNum = itemobj->data.dwNum;
									sendCmdToMe(&send, sizeof(send));
							}
							else
							{
								pUser->packs.removeObject(itemobj);
							}
							pUser->packs.m_Pifeng[equip->pifengid].state=1;
							pUser->packs.m_Pifeng[equip->pifengid].level++;
							Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"披风进阶成功");
					 	}
						else{
							char msg[256];
							sprintf(msg,"披风碎片不足，当前等级进阶需要 %d个披风碎片",jihuoNum);
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,msg);
						}
					}
						
				}
			}
			break;
		case SETCHIBANG_USERCMD: //sky 佩戴翅膀
			{
				using namespace Cmd;
				stSetChibangUserCmd *equip = (stSetChibangUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);
				if(pUser)
				{
					if(pUser->packs.m_Chibang[equip->chibangid].state!=1)
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"此翅膀未激活，无法装备");
						return true;
					}
					pUser->charbase.chibang_select = equip->chibangid;
					DWORD costID =0;
					if(fjconfig::getInstance().chibanglist[equip->chibangid].costID)
					{
						costID = fjconfig::getInstance().chibanglist[equip->chibangid].costID;
					}
					if(costID!=0)
					{
						zObjectB *base = objectbm.get(costID);
						if(base)
						{
							int hp = fjconfig::getInstance().chibanglevellist[pUser->packs.m_Chibang[equip->chibangid].level].hp;
							int wugong = fjconfig::getInstance().chibanglevellist[pUser->packs.m_Chibang[equip->chibangid].level].wugong;
							int mogong = fjconfig::getInstance().chibanglevellist[pUser->packs.m_Chibang[equip->chibangid].level].mogong;
							base->maxpdamage = wugong;
							base->maxmdamage = mogong;
							base->maxsp = hp;	
							zObject *ob = zObject::create(base,1);
							if (ob)
							{
								zObject *obj = pUser->packs.equip.getObjectByType(93);
								if(obj)
								{
									pUser->packs.removeObject(obj);
								}
								int shengming = fjconfig::getInstance().chibanglevellist[pUser->packs.m_Chibang[equip->chibangid].level].shengming;
								int fashu = fjconfig::getInstance().chibanglevellist[pUser->packs.m_Chibang[equip->chibangid].level].fashu;
								int bang = 0;
								bang = fjconfig::getInstance().chibanglevellist[pUser->packs.m_Chibang[equip->chibangid].level].bang;	
								ob->data.maxhp = shengming;
								ob->data.maxmp = fashu;
								ob->data.bang = bang;
								ob->data.bind = 1;
								if (pUser->packs.addObject(ob,true, AUTO_PACK))
								{
								
									Cmd::stAddObjectPropertyUserCmd send;
									bcopy(&ob->data,&send.object,sizeof(t_Object));
									pUser->sendCmdToMe(&send,sizeof(send));

									Cmd::stSwapObjectPropertyUserCmd swap;
									swap.qwThisID = ob->data.qwThisID;
									swap.dst = stObjectLocation(Cmd::OBJECTCELLTYPE_EQUIP, 0, 0, 37);;
									if(packs.moveObject(pUser,ob,swap.dst))
									{
										
										sendCmdToMe(&swap,sizeof(Cmd::stSwapObjectPropertyUserCmd));
									}
				
									pUser->notifyEquipChange();
									
									pUser->setupCharBase();
									Cmd::stMainUserDataUserCmd  userinfo;
									full_t_MainUserData(userinfo.data);
									pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
							
									pUser->sendMeToNine();

								}
							}
						}
						
					}
					
				}

			}
			break;
		case SETNOTCHIBANG_USERCMD: //sky 取消佩戴翅膀
			{
				 using namespace Cmd;
				stSetNotChibangUserCmd *equip = (stSetNotChibangUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);
				if(pUser)
				{
					pUser->charbase.chibang_select = 9999;
					zObject *obj = pUser->packs.equip.getObjectByType(93);
					if(obj)
					{
						pUser->packs.removeObject(obj);
						pUser->notifyEquipChange();
						pUser->setupCharBase();
						Cmd::stMainUserDataUserCmd  userinfo;
						full_t_MainUserData(userinfo.data);
						pUser->sendCmdToMe(&userinfo,sizeof(userinfo));
						pUser->sendMeToNine();
					}			
				}
			}
			break;
		case SETCHIBANGJIHUO_USERCMD: //sky 请求激活/升级翅膀
			{
				using namespace Cmd;
				stSetChibangJihuoUserCmd *equip = (stSetChibangJihuoUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);
				if(pUser)
				{
					if(pUser->packs.m_Chibang[equip->chibangid].state == 0)//激活 
					{
						SessionItemObjectComparet found;
						found.dwObjectID = fjconfig::getInstance().chibanglist[equip->chibangid].jihuoID;
						zObject *itemobj = pUser->packs.uom.getObject(found);// 查找道具
						if(itemobj)
						{
							DWORD jihuoNum = fjconfig::getInstance().chibanglist[equip->chibangid].jihuoNum;
							if(itemobj->data.dwNum < jihuoNum)
							{
								Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少激活翅膀道具【翅膀碎片】");
								return true;
							}
							if (itemobj->data.dwNum > jihuoNum) // soke 大于9朵直接-9
							{
									itemobj->data.dwNum-=jihuoNum;
									Cmd::stRefCountObjectPropertyUserCmd send;
									send.qwThisID = itemobj->data.qwThisID;
									send.dwNum = itemobj->data.dwNum;
									sendCmdToMe(&send, sizeof(send));
							}
							else
							{
								pUser->packs.removeObject(itemobj);
							}
							pUser->packs.m_Chibang[equip->chibangid].state=1;
							pUser->packs.m_Chibang[equip->chibangid].level=0;
							Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"翅膀激活成功");
					 	}
						else{
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少激活翅膀道具【翅膀碎片】");
						}
					}
					else{//升级
						if(pUser->packs.m_Chibang[equip->chibangid].level >= 6)
						{
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"此翅膀已达到最高等级");
							return true;	
						}
						SessionItemObjectComparet found;
						
						found.dwObjectID = fjconfig::getInstance().chibanglevellist[pUser->packs.m_Chibang[equip->chibangid].level].costID;
						
						zObject *itemobj = pUser->packs.uom.getObject(found);// 查找道具
						DWORD jihuoNum = fjconfig::getInstance().chibanglevellist[pUser->packs.m_Chibang[equip->chibangid].level].costNum;
						if(itemobj)
						{
							
							if(itemobj->data.dwNum < jihuoNum)
							{
								char msg[256];
								sprintf(msg,"翅膀碎片不足，当前等级进阶需要 %d个翅膀碎片",jihuoNum);
								Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,msg);
								return true;
							}
							if (itemobj->data.dwNum > jihuoNum) // soke 大于9朵直接-9
							{
									itemobj->data.dwNum-=jihuoNum;
									Cmd::stRefCountObjectPropertyUserCmd send;
									send.qwThisID = itemobj->data.qwThisID;
									send.dwNum = itemobj->data.dwNum;
									sendCmdToMe(&send, sizeof(send));
							}
							else
							{
								pUser->packs.removeObject(itemobj);
							}
							pUser->packs.m_Chibang[equip->chibangid].state=1;
							pUser->packs.m_Chibang[equip->chibangid].level++;
							Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"翅膀进阶成功");
					 	}
						else{
							char msg[256];
							sprintf(msg,"翅膀碎片不足，当前等级进阶需要 %d个翅膀碎片",jihuoNum);
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,msg);
						}
					}
						
				}
			}
			break;
		case SETZUOQI_USERCMD: //sky 佩戴坐骑
			{
				using namespace Cmd;
				stSetZuoqiUserCmd *equip = (stSetZuoqiUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);
				if(pUser)
				{
					if(pUser->packs.m_Zuoqi[equip->zuoqiid].state!=1)
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"此坐骑未激活，无法装备");
						return true;
					}
					pUser->charbase.zuoqi_select = equip->zuoqiid;
					DWORD npcID =0;
					if(fjconfig::getInstance().zuoqi2list[equip->zuoqiid].npcID)
					{
						npcID = fjconfig::getInstance().zuoqi2list[equip->zuoqiid].npcID;
					}
					if(npcID!=0)
					{
						pUser->horse.horse(npcID);
					}
					
				}

			}
			break;
		case SETNOTZUOQI_USERCMD: //sky 取消佩戴坐骑
			{
				 using namespace Cmd;
				stSetNotZuoqiUserCmd *equip = (stSetNotZuoqiUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);
				if(pUser)
				{
					pUser->charbase.zuoqi_select = 9999;
					pUser->horse.horse(3256);	
				}
			}
			break;
		case SETZUOQIJIHUO_USERCMD: //sky 请求激活/升级坐骑
			{
				using namespace Cmd;
				stSetZuoqiJihuoUserCmd *equip = (stSetZuoqiJihuoUserCmd *)(rev);
				SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);
				if(pUser)
				{
					if(pUser->packs.m_Zuoqi[equip->zuoqiid].state == 0)//激活 
					{
						SessionItemObjectComparet found;
						found.dwObjectID = fjconfig::getInstance().zuoqi2list[equip->zuoqiid].jihuoID;
						zObject *itemobj = pUser->packs.uom.getObject(found);// 查找道具
						if(itemobj)
						{
							DWORD jihuoNum = fjconfig::getInstance().zuoqi2list[equip->zuoqiid].jihuoNum;
							if(itemobj->data.dwNum < jihuoNum)
							{
								Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少激活坐骑道具【坐骑碎片】");
								return true;
							}
							if (itemobj->data.dwNum > jihuoNum) // soke 大于9朵直接-9
							{
									itemobj->data.dwNum-=jihuoNum;
									Cmd::stRefCountObjectPropertyUserCmd send;
									send.qwThisID = itemobj->data.qwThisID;
									send.dwNum = itemobj->data.dwNum;
									sendCmdToMe(&send, sizeof(send));
							}
							else
							{
								pUser->packs.removeObject(itemobj);
							}
							pUser->packs.m_Zuoqi[equip->zuoqiid].state=1;
							pUser->packs.m_Zuoqi[equip->zuoqiid].level=0;
							Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME,"坐骑激活成功");
					 	}
						else{
							Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"缺少激活坐骑道具【坐骑碎片】");
						}
					}
					else{//升级
	
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL,"此坐骑已激活");
						return true;	
					}
						
				}
			}
			break;
		case MEIRIRENWUCHUANSONG_USERCMD://每日任务传送
		{
			using namespace Cmd;
			stMeiRiRenWuChuanSongUserCmd *equip = (stMeiRiRenWuChuanSongUserCmd *)(rev);
			SceneUser* pUser = SceneUserManager::getMe().getUserByID(equip->dwOldTempID);
			if(pUser)
			{
				DWORD num=equip->num;
				switch (num)
				{
				case 1:
					{
						if(pUser->charbase.country == 8){
							Gm::gomap(pUser, "name=汉国·王城 pos=871,488");  //自动传送地图坐标
						}
						else if(pUser->charbase.country == 12)
						{
							Gm::gomap(pUser, "name=楚国·王城 pos=871,488");  //自动传送地图坐标
						}
					}
					break;
				case 2:
					{
						if(pUser->charbase.country == 8){
							Gm::gomap(pUser, "name=汉国·王城 pos=869,490");  //自动传送地图坐标
						}
						else if(pUser->charbase.country == 12)
						{
							Gm::gomap(pUser, "name=楚国·王城 pos=869,490");  //自动传送地图坐标
						}
					}
					break;
				case 3:
					{
						if(pUser->charbase.country == 8){
							Gm::gomap(pUser, "name=汉国·王城 pos=867,493");  //自动传送地图坐标
						}
						else if(pUser->charbase.country == 12)
						{
							Gm::gomap(pUser, "name=楚国·王城 pos=867,493");  //自动传送地图坐标
						}
					}
					break;
				case 4:
					{
						if(pUser->charbase.country == 8){
							Gm::gomap(pUser, "name=汉国·王城 pos=865,495");  //自动传送地图坐标
						}
						else if(pUser->charbase.country == 12)
						{
							Gm::gomap(pUser, "name=楚国·王城 pos=865,495");  //自动传送地图坐标
						}
					}
					break;
				case 5:
					{
						if(pUser->charbase.country == 8){
							Gm::gomap(pUser, "name=汉国·王城 pos=862,499");  //自动传送地图坐标
						}
						else if(pUser->charbase.country == 12)
						{
							Gm::gomap(pUser, "name=楚国·王城 pos=862,499");  //自动传送地图坐标
						}
					}
					break;
				case 6:
					{
						if(pUser->charbase.country == 8){
							Gm::gomap(pUser, "name=汉国·王城 pos=860,501");  //自动传送地图坐标
						}
						else if(pUser->charbase.country == 12)
						{
							Gm::gomap(pUser, "name=楚国·王城 pos=860,501");  //自动传送地图坐标
						}
					}
					break;
				case 7:
					{
						if(pUser->charbase.country == 8){
							Gm::gomap(pUser, "name=汉国·王城 pos=850,446");  //自动传送地图坐标
						}
						else if(pUser->charbase.country == 12)
						{
							Gm::gomap(pUser, "name=楚国·王城 pos=850,446");  //自动传送地图坐标
						}
					}
					break;
				case 8:
					{
						if(pUser->charbase.country == 8){
							Gm::gomap(pUser, "name=汉国·王城 pos=831,446");  //自动传送地图坐标
						}
						else if(pUser->charbase.country == 12)
						{
							Gm::gomap(pUser, "name=楚国·王城 pos=831,446");  //自动传送地图坐标
						}
					}
					break;
				
				}
			}
		}
		break;
		case SUIJICHUANSONG_USERCMD://随机传送
		{
			using namespace Cmd;
			zPos newPos;
			// 随机坐标
			if(!scene->randzPosByZoneType(ZoneTypeDef::ZONE_NONE,newPos)) return false;
			if(!goTo(newPos)) return false;
			
		}
		break;
		case HAIR_PROPERTY_USERCMD_PARA:
			{
				using namespace Cmd;
				stHairPropertyUserCmd *hp = (stHairPropertyUserCmd*)rev;
				if(hp->dwHairColor == charbase.hair && hp->byHairType == getHairType())
				{
					return true;
				}
				DWORD cost = 0;
				int isChange = 0;
				if(hp->byHairType != getHairType())
				{
					zHairStyleB *base=hairstylebm.get(hp->byHairType);
					if(base)
					{
						cost = base->cost;
						isChange = 1;
					}
					/*
					switch(hp->byHairType)
					{
						case HairStype_1:
							{
								isChange = 1;
								cost = 2000;
							}
							break;
						case HairStype_2:
							{
								isChange = 1;
								cost = 2000;
							}
							break;
						case HairStype_3:
							{
								isChange = 1;
								cost = 2000;
							}
							break;
						case HairStype_4:
							{
								isChange = 1;
								cost = 2000;
							}
							break;
						case HairStype_5:
							{
								isChange = 1;
								cost = 2000;
							}
							break;
						case HairStype_6:
							{
								isChange = 1;
								cost = 2000;
							}
							break;
						default:
							break;
					}
					// */
				}
				if((hp->dwHairColor & 0X00FFFFFF) != getHairColor())
				{
					zHairColourB *base=haircolourbm.get(hp->dwHairColor & 0X00FFFFFF);
					if(base)
					{
						/// 如果不是光头换发色才需要银子
						if(hp->byHairType != 1)
						{
							cost += base->cost;
						}
						isChange = 2;
					}
					/*
					switch(hp->dwHairColor)
					{
						case HairColor_black:
							{
								isChange = 2;
								cost += 1000;
							}
							break;
						case HairColor_drink:
							{
								isChange = 2;
								cost += 2000;
							}
							break;
						case HairColor_purple:
							{
								isChange = 2;
								cost += 3000;
							}
							break;
					}
					// */
				}
/*
				zObject *gold=packs.getGold();
				if(!gold)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"对不起,你银子不够");
					return true;
				}
				if(cost > gold->data.dwNum)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"对不起,你银子不够");
					return true;
				}
				gold->data.dwNum -= cost;
				if(gold->data.dwNum==0)
				{
					stRemoveObjectPropertyUserCmd rmgold;
					rmgold.qwThisID=gold->data.qwThisID;
					sendCmdToMe(&rmgold,sizeof(rmgold));
					packs.rmObject(gold);
					SAFE_DELETE(gold);
				}
				else
				{
					stRefCountObjectPropertyUserCmd setgold;
					setgold.qwThisID=gold->data.qwThisID;
					setgold.dwNum=gold->data.dwNum;
					sendCmdToMe(&setgold,sizeof(setgold));
				}
*/
				if (!packs.checkMoney(cost) || !packs.removeMoney(cost,"美发")) {
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"对不起,你银子不够");
					return true;
				}
				
				if(isChange)
				{
					Channel::sendMoney(this, Cmd::INFO_TYPE_GAME,cost,"美发成功,花费银子");
				}
				else
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"所选服务雷同，请选择其他服务");
				}
				setHairType(hp->byHairType);
				setHairColor(hp->dwHairColor);
				//Cmd::stAddUserMapScreenUserCmd cmd;
				//full_t_MapUserData(cmd.data);
				//scene->sendCmdToNine(getPosI() , &cmd,sizeof(cmd) , false);
				reSendMyMapData();
				return true;
			}
			break;
		case SELECT_PROPERTY_USERCMD_PARA:
			{
				using namespace Cmd;
				stSelectPropertyUserCmd * spu = (stSelectPropertyUserCmd*)rev;
				//Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"收到选择指令");
				if(spu->dwOldTempID)
				{
					if(spu->byOldType == MAPDATATYPE_USER)
					{
						SceneUser *pDel = SceneUserManager::getMe().getUserByTempID(spu->dwOldTempID);
						if(pDel)
						{
							//pDel->selected_lock.lock();
							pDel->selected.erase(this->tempid);
							//pDel->selected_lock.unlock();
						}
					}
					else
					{
						SceneNpc *pDel = SceneNpcManager::getMe().getNpcByTempID(spu->dwOldTempID);
						if(pDel)
						{
							//pDel->selected_lock.lock();
							pDel->selected.erase(this->tempid);
							//pDel->selected_lock.unlock();
						}
					}
				}
				//如果目标为-1则表示不再选中别的对象
				if(spu->dwTempID == (DWORD)-1)
				{
					break;
				}
				switch(spu->byType)
				{
					case MAPDATATYPE_NPC:
						{
							SceneNpc *pNpc = SceneNpcManager::getMe().getNpcByTempID(spu->dwTempID);
							if(pNpc)
							{
								//pNpc->selected_lock.lock();
								pNpc->selected.insert(SelectedSet_value_type(this->tempid));
								Cmd::stRTSelectedHpMpPropertyUserCmd ret;
								ret.byType = Cmd::MAPDATATYPE_NPC;
								ret.dwTempID = pNpc->tempid;//临时编号
								ret.dwHP = pNpc->hp;//当前血
								ret.dwMaxHp = pNpc->getMaxHP();//最大hp
								ret.dwMP = 0;//this->charbase.mp;//当前mp
								ret.dwMaxMp = 0;//this->charstate.maxmp;//最大mp
								//pNpc->selected_lock.unlock();
								this->sendCmdToMe(&ret , sizeof(ret));
								char Buf[200]; 
								bzero(Buf , sizeof(Buf));
								stSelectReturnStatesPropertyUserCmd *srs=(stSelectReturnStatesPropertyUserCmd*)Buf;
								constructInPlace(srs);
								srs->byType = MAPDATATYPE_NPC;
								srs->dwTempID = spu->dwTempID;
								pNpc->skillStatusM.getSelectStates(srs , sizeof(Buf));
								if(srs->size > 0)
								{
									this->sendCmdToMe(srs , sizeof(stSelectReturnStatesPropertyUserCmd) + 
											sizeof(srs->states[0]) * srs->size);
								}
							}
						}
						break;
					case MAPDATATYPE_USER:
						{
							using namespace Cmd;
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(spu->dwTempID);
							if(pUser)
							{
								//Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"%s收到选择%s指令",this->name , pUser->name);
								//pUser->selected_lock.lock();
								pUser->selected.insert(SelectedSet_value_type(this->tempid));
								Cmd::stRTSelectedHpMpPropertyUserCmd ret;
								ret.byType = Cmd::MAPDATATYPE_USER;
								ret.dwTempID = pUser->tempid;//临时编号
								ret.dwHP = pUser->charbase.hp;//当前血
								ret.dwMaxHp = pUser->charstate.maxhp;//最大hp
								ret.dwMP = pUser->charbase.mp;//当前mp
								ret.dwMaxMp = pUser->charstate.maxmp;//最大mp
								//pUser->selected_lock.unlock();
								this->sendCmdToMe(&ret , sizeof(ret));
								char Buf[200]; 
								bzero(Buf , sizeof(Buf));
								stSelectReturnStatesPropertyUserCmd *srs=(stSelectReturnStatesPropertyUserCmd*)Buf;
								constructInPlace(srs);
								srs->byType = MAPDATATYPE_USER;
								srs->dwTempID = spu->dwTempID;
								pUser->skillStatusM.getSelectStates(srs , sizeof(Buf));
								if(srs->size > 0)
								{
									this->sendCmdToMe(srs , sizeof(stSelectReturnStatesPropertyUserCmd) + 
											sizeof(srs->states[0]) * srs->size);
								}
								//临时发送pk附加状态,等待以后策划修改^_^处理
								pUser->sendtoSelectedPkAdditionStateToUser(this);
								pUser->sendtoSelectedReliveWeakStateToUser(this);
								pUser->sendtoSelectedTrainStateToUser(this);
								pUser->sendtoSelectedVIPStateToUser(this);
								pUser->sendtoSelectedTaiMiaoToUser(this);//特殊处理
								//pUser->sendtoSelected9StateToUser(this);
								//pUser->sendtoSelected99StateToUser(this);
								//pUser->sendtoSelected999StateToUser(this);
							}
						}
						break;
					default:
						break;
				}
			}
			break;
		case CLEARUSERSKILLPOINT_FUBENJH_USERCMD_PARAMETER: //soke 副本激活
			{
				//Cmd::stClearUserSkillPointFubenjhUserCmd *cmd = (Cmd::stClearUserSkillPointFubenjhUserCmd*)rev;

				if (this->charbase.level < 30)
		        {
			        Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "等级不符，无法开启副本");
			        return false;
		        }
				if (this->charbase.fbCount > 0)
		        {
			        Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "今日的副本已经开启");
			        return false;
		        }
				else
				{
					char Buf[200]; 
					bzero(Buf,sizeof(Buf));
					Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
					constructInPlace(srs);
					srs->byType = Cmd::MAPDATATYPE_USER;
					srs->dwTempID = this->tempid;
					srs->size=1;

					//soke 清除状态,防止叠加时间显示错误
					showCurrentEffect(Cmd::USTATE_FUBEN, false);

					DWORD randNum = zMisc::randBetween(1,500);

                    charbase.fbCount += 1;       //soke 副本次数
					charbase.fubentime = 0;      //soke 清空时间
					charbase.fubentime += 7200;  //soke 副本时间
					charbase.bitmask |= CHARBASE_FUBEN;
			        showCurrentEffect(Cmd::USTATE_FUBEN, true);
			        srs->states[0].state = Cmd::USTATE_FUBEN;
					Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"当前副本还剩:%d小时 %d分钟 ",(charbase.fubentime%86400)/3600,(charbase.fubentime%3600)/60);
					Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "副本开启成功！");
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP1," 哇～ %s 的 %s 成功开启副本 %d ",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->name,randNum);

					srs->states[0].result=charbase.fubentime/60;
					srs->states[0].time=charbase.fubentime/60;
					sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
					SelectedSet_iterator iter = selected.begin();
					for(; iter != selected.end() ;)
					{
						SceneUser* pUser = SceneUserManager::getMe().getUserByTempID(*iter);
						if (pUser)
						{
							if (this->scene->checkTwoPosIInNine(this->getPosI(),pUser->getPosI()))
							{
								pUser->sendCmdToMe(srs,sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + sizeof(srs->states[0]) * srs->size);
								iter ++ ;
								continue;
							}
						}
						SelectedSet_iterator iter_del = iter;
						iter_del ++;
						selected.erase(iter);
						iter = iter_del;
				     }
				 }
			}
			break;
			//请求观察某个用户
		case SELECT_MAINUSER_PROPERTY_USERCMD_PARA:
			{
				using namespace Cmd;
				stSelectMainUserPropertyUserCmd *smu = (stSelectMainUserPropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							//add 2023-05-27 zhangli zb
							//SceneUser *pUser = SceneUserManager::getMe().getUserByID(smu->dwTempID);
							SceneUser *pUser = NULL;
							pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							Zebra::logger->debug("*****-%d-**SceneUser::docmd**找到pUser=%d;",pUser);
							if(pUser == NULL)
							{
								pUser = SceneUserManager::getMe().getUserByID(smu->dwTempID);
								if(pUser)
								{
									Zebra::logger->debug("*****-%d-**SceneUser::docmd-getUserByID**找到pUser.userid=%d;pUser.dwTempID=%d;",pUser->charbase.id,pUser->tempid);
								}
							}
							if(pUser)
							{
							  //Zebra::logger->debug("被查看玩家装备ID:%d-**SceneUser::docmd-getUserByTempID**找到pUser.userid=%d;pUser.dwTempID=%d;",pUser->charbase.id,pUser->tempid);
                                Zebra::logger->debug("被查看玩家id:%d,找到角色临时ID.userid=%d;玩家临时编号.dwTempID=%d;",pUser->charbase.id,pUser->tempid);
								if (!pUser->mask.is_masking())
								{
									char Buf[sizeof(stSelectReturnMainUserPropertyUserCmd) + sizeof(EquipedObject) * 128]; //修复查看装备报错128格子数
									bzero(Buf , sizeof(Buf));
									stSelectReturnMainUserPropertyUserCmd *srm = (stSelectReturnMainUserPropertyUserCmd*)Buf;
									constructInPlace(srm);
									srm->dwTempID = pUser->tempid;
									pUser->full_t_MainUserData(srm->mainuser_data);
									pUser->full_t_MapUserData(srm->mapuser_data);
								
									srm->dwSize = pUser->packs.equip.fullAllEquiped((char*)srm->object_data);
									this->sendCmdToMe(srm , sizeof(stSelectReturnMainUserPropertyUserCmd) 
											+ sizeof(EquipedObject) * srm->dwSize);

									if (pUser->horse.horse())
									{
											stSelectReturnHorsePropertyUserCmd send;
											pUser->horse.full_HorseDataStruct(&send.data);
											sendCmdToMe(&send,sizeof(send));
									}
									char text[32];
									sprintf(text,"%s正在在偷偷观察你!",this->charbase.name);
									Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, text);
								}
								else
								{
									

									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "他是蒙面人无法观察");
			
								}
							}
							else
							{
									Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对方不在线，无法查看装备！");
							}
						}
						break;
						case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS_PROPERTY_USERCMD_PARA: //soke 匿名送花9朵
			{
				using namespace Cmd;
				stSelectFlowersPropertyUserCmd *smu = (stSelectFlowersPropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送花哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送花");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare found;
					                found.dwObjectID = 1100;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>9) //soke 大于9朵直接-9
		                                { 
			                                itemobj->data.dwNum-=9;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于9朵直接清除，不这样写会有BUG 9朵只减8 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=9; //soke 鲜花增加9朵
										pUser->charbase.moods+=9;   //soke 人气增加9点
									    this->charbase.flloves+=9;  //soke 爱心增加9颗

										pUser->charbase.flower1time = 0;      //soke 清空时间
										pUser->charbase.flower1time += 86400; //soke 9朵花时间

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上9朵花状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower1time/86400,(pUser->charbase.flower1time%86400)/3600,(pUser->charbase.flower1time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "神秘人士被你的魅力所倾倒送上了9朵红玫瑰！");
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了9朵红玫瑰！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_EXP," 哇～～ 某神秘人士 向 %s 真诚地献上了9朵红玫瑰，%s 太有魅力了",pUser->name,pUser->name);	
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多鲜花哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送花哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS1_PROPERTY_USERCMD_PARA: //soke 匿名送花99朵
			{
				using namespace Cmd;
				stSelectFlowers1PropertyUserCmd *smu = (stSelectFlowers1PropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送花哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送花");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare99 found;
					                found.dwObjectID = 1100;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>99) //soke 大于99朵直接-99
		                                { 
			                                itemobj->data.dwNum-=99;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于99朵直接清除，不这样写会有BUG 99朵只减98 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=99; //soke 鲜花增加99朵
										pUser->charbase.moods+=99;   //soke 人气增加99点
									    this->charbase.flloves+=99;  //soke 爱心增加99颗

										pUser->charbase.flower2time = 0; //soke 时间清空
										pUser->charbase.flower2time += 86400; //soke 99朵花时间 1天

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上99朵花状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower2time/86400,(pUser->charbase.flower2time%86400)/3600,(pUser->charbase.flower2time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "神秘人士被你的魅力所倾倒送上了99朵红玫瑰！");
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了99朵红玫瑰！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESROLL," 哇～～ 某神秘人士 向 %s 真诚地献上了99朵红玫瑰，%s 太有魅力了 ",pUser->name,pUser->name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESROLL," 哇～～ 某神秘人士 向 %s 真诚地献上了99朵红玫瑰，%s 太有魅力了 ",pUser->name,pUser->name);
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多鲜花哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送花哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS2_PROPERTY_USERCMD_PARA: //soke 匿名送花365朵
			{
				using namespace Cmd;
				stSelectFlowers2PropertyUserCmd *smu = (stSelectFlowers2PropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送花哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送花");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare365 found;
					                found.dwObjectID = 1100;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>365) //soke 大于365朵直接-365
		                                { 
			                                itemobj->data.dwNum-=365;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于365朵直接清除，不这样写会有BUG 365朵只减364 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=365; //soke 鲜花增加365朵
										pUser->charbase.moods+=365;   //soke 人气增加365点
									    this->charbase.flloves+=365;  //soke 爱心增加365颗

										pUser->charbase.flower2time = 0; //soke 时间清空
										pUser->charbase.flower2time += 259200; //soke 365朵花时间 3天

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上365朵花状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower2time/86400,(pUser->charbase.flower2time%86400)/3600,(pUser->charbase.flower2time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "神秘人士被你的魅力所倾倒送上了365朵红玫瑰！");
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了365朵红玫瑰！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESROLL," 哇～～ 某神秘人士 向 %s 送出了整整365朵红玫瑰，%s 太有魅力了 ",pUser->name,pUser->name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESROLL," 哇～～ 某神秘人士 向 %s 送出了整整365朵红玫瑰，%s 太有魅力了 ",pUser->name,pUser->name);
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多鲜花哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送花哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS3_PROPERTY_USERCMD_PARA: //soke 匿名送花999朵
			{
				using namespace Cmd;
				stSelectFlowers3PropertyUserCmd *smu = (stSelectFlowers3PropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送花哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送花");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare999 found;
					                found.dwObjectID = 1100;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>999) //soke 大于999朵直接-999
		                                { 
			                                itemobj->data.dwNum-=999;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于999朵直接清除，不这样写会有BUG 999朵只减998 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=999; //soke 鲜花增加9朵
										pUser->charbase.moods+=999;   //soke 人气增加9点
									    this->charbase.flloves+=999;  //soke 爱心增加9颗

										pUser->charbase.flower3time = 0; //soke 时间清空
										pUser->charbase.flower3time += 518400; //soke 999朵花时间 6天

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上999朵花状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower3time/86400,(pUser->charbase.flower3time%86400)/3600,(pUser->charbase.flower3time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "神秘人士被你的魅力所倾倒送上了999朵红玫瑰！");
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了999朵红玫瑰！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESROLL," 哇～～ 某神秘人士 向 %s 送出了整整999朵红玫瑰，%s 好幸福哦 ",pUser->name,pUser->name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESROLL," 哇～～ 某神秘人士 向 %s 送出了整整999朵红玫瑰，%s 好幸福哦 ",pUser->name,pUser->name);
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多鲜花哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送花哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS4_PROPERTY_USERCMD_PARA: //soke 签名送花9朵
			{
				using namespace Cmd;
				stSelectFlowers4PropertyUserCmd *smu = (stSelectFlowers4PropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送花哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送花");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare found;
					                found.dwObjectID = 1100;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>9) //soke 大于9朵直接-9
		                                { 
			                                itemobj->data.dwNum-=9;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于9朵直接清除，不这样写会有BUG 9朵只减8 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=9; //soke 鲜花增加9朵
										pUser->charbase.moods+=9;   //soke 人气增加9点
									    this->charbase.flloves+=9;  //soke 爱心增加9颗

										pUser->charbase.flower1time = 0; //soke 时间清空
										pUser->charbase.flower1time += 86400; //soke 9朵花时间 1天

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上9朵花状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower1time/86400,(pUser->charbase.flower1time%86400)/3600,(pUser->charbase.flower1time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, " %s被你的魅力所倾倒送上了9朵红玫瑰！",this->name);
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了9朵红玫瑰！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_EXP," 哇～～ %s 向 %s 真诚地献上了9朵红玫瑰，%s 太有魅力了 ",this->name,pUser->name,pUser->name);
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多鲜花哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送花哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS5_PROPERTY_USERCMD_PARA: //soke 签名送花99朵
			{
				using namespace Cmd;
				stSelectFlowers5PropertyUserCmd *smu = (stSelectFlowers5PropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送花哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送花");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare99 found;
					                found.dwObjectID = 1100;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>99) //soke 大于99朵直接-99
		                                { 
			                                itemobj->data.dwNum-=99;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于99朵直接清除，不这样写会有BUG 99朵只减98 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=99; //soke 鲜花增加99朵
										pUser->charbase.moods+=99;   //soke 人气增加99点
									    this->charbase.flloves+=99;  //soke 爱心增加99颗

										pUser->charbase.flower2time = 0; //soke 时间清空
										pUser->charbase.flower2time += 86400; //soke 99朵花时间 1天

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上99朵花状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower2time/86400,(pUser->charbase.flower2time%86400)/3600,(pUser->charbase.flower2time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, " %s被你的魅力所倾倒送上了99朵红玫瑰！",this->name);
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了99朵红玫瑰！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESROLL," 哇～～ %s 居然向 %s 送出了99朵红玫瑰，%s 太有魅力了 ",this->name,pUser->name,pUser->name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESROLL," 哇～～ %s 居然向 %s 送出了99朵红玫瑰，%s 太有魅力了 ",this->name,pUser->name,pUser->name);
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多鲜花哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送花哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS6_PROPERTY_USERCMD_PARA: //soke 签名送花365朵
			{
				using namespace Cmd;
				stSelectFlowers6PropertyUserCmd *smu = (stSelectFlowers6PropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送花哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送花");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare365 found;
					                found.dwObjectID = 1100;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>365) //soke 大于365朵直接-365
		                                { 
			                                itemobj->data.dwNum-=365;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于365朵直接清除，不这样写会有BUG 365朵只减364 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=365; //soke 鲜花增加365朵
										pUser->charbase.moods+=365;   //soke 人气增加365点
									    this->charbase.flloves+=365;  //soke 爱心增加365颗

										pUser->charbase.flower2time = 0; //soke 时间清空
										pUser->charbase.flower2time += 259200; //soke 365朵花时间 3天

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上365朵花状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower2time/86400,(pUser->charbase.flower2time%86400)/3600,(pUser->charbase.flower2time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, " %s被你的魅力所倾倒送上了365朵红玫瑰！",this->name);
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了365朵红玫瑰！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESROLL," 哇～～ %s 居然向 %s 送出了整整365朵红玫瑰，%s 太有魅力了 ",this->name,pUser->name,pUser->name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESROLL," 哇～～ %s 居然向 %s 送出了整整365朵红玫瑰，%s 太有魅力了 ",this->name,pUser->name,pUser->name);
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多鲜花哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送花哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS7_PROPERTY_USERCMD_PARA: //soke 签名送花999朵
			{
				using namespace Cmd;
				stSelectFlowers7PropertyUserCmd *smu = (stSelectFlowers7PropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送花哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送花");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare999 found;
					                found.dwObjectID = 1100;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>999) //soke 大于999朵直接-999
		                                { 
			                                itemobj->data.dwNum-=999;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于999朵直接清除，不这样写会有BUG 999朵只减998 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=999; //soke 鲜花增加999朵
										pUser->charbase.moods+=999;   //soke 人气增加999点
									    this->charbase.flloves+=999;  //soke 爱心增加999颗

										pUser->charbase.flower3time = 0; //soke 时间清空
										pUser->charbase.flower3time += 518400; //soke 999朵花时间 6天

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上999朵花状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower3time/86400,(pUser->charbase.flower3time%86400)/3600,(pUser->charbase.flower3time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, " %s被你的魅力所倾倒送上了999朵红玫瑰！",this->name);
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了999朵红玫瑰！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESROLL," 哇～～ %s 居然向 %s 送出了整整999朵红玫瑰，%s 好幸福哦 ",this->name,pUser->name,pUser->name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESROLL," 哇～～ %s 居然向 %s 送出了整整999朵红玫瑰，%s 好幸福哦 ",this->name,pUser->name,pUser->name);
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多鲜花哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送花哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS20_PROPERTY_USERCMD_PARA: //soke 匿名送蓝色妖姬9朵
			{
				using namespace Cmd;
				stSelectFlowers20PropertyUserCmd *smu = (stSelectFlowers20PropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送蓝色妖姬哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送蓝色妖姬");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare found;
					                found.dwObjectID = 2900;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>9) //soke 大于9朵直接-9
		                                { 
			                                itemobj->data.dwNum-=9;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于9朵直接清除，不这样写会有BUG 9朵只减8 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=9; //soke 鲜花增加9朵
										pUser->charbase.moods+=9;   //soke 人气增加9点
									    this->charbase.flloves+=9;  //soke 爱心增加9颗

										pUser->charbase.flower4time = 0;      //soke 清空时间
										pUser->charbase.flower4time += 86400; //soke 9朵蓝色妖姬时间

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上9朵蓝色妖姬状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower4time/86400,(pUser->charbase.flower4time%86400)/3600,(pUser->charbase.flower4time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "神秘人士被你的魅力所倾倒送上了9朵蓝色妖姬！");
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了9朵蓝色妖姬！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_EXP," 哇～～ 某神秘人士 向 %s 真诚地献上了9朵蓝色妖姬，%s 太有魅力了",pUser->name,pUser->name);	
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多蓝色妖姬哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送蓝色妖姬哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS21_PROPERTY_USERCMD_PARA: //soke 匿名送蓝色妖姬99朵
			{
				using namespace Cmd;
				stSelectFlowers21PropertyUserCmd *smu = (stSelectFlowers21PropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送蓝色妖姬哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送蓝色妖姬");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare99 found;
					                found.dwObjectID = 2900;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>99) //soke 大于99朵直接-99
		                                { 
			                                itemobj->data.dwNum-=99;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于99朵直接清除，不这样写会有BUG 99朵只减98 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=99; //soke 鲜花增加99朵
										pUser->charbase.moods+=99;   //soke 人气增加99点
									    this->charbase.flloves+=99;  //soke 爱心增加99颗

										pUser->charbase.flower5time = 0; //soke 时间清空
										pUser->charbase.flower5time += 86400; //soke 99朵蓝色妖姬时间 1天

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上99朵蓝色妖姬状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower5time/86400,(pUser->charbase.flower5time%86400)/3600,(pUser->charbase.flower5time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "神秘人士被你的魅力所倾倒送上了99朵蓝色妖姬！");
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了99朵蓝色妖姬！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESRXLL," 哇～～ 某神秘人士 向 %s 真诚地献上了99朵蓝色妖姬，%s 太有魅力了 ",pUser->name,pUser->name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESRXLL," 哇～～ 某神秘人士 向 %s 真诚地献上了99朵蓝色妖姬，%s 太有魅力了 ",pUser->name,pUser->name);
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多蓝色妖姬哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送蓝色妖姬哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS22_PROPERTY_USERCMD_PARA: //soke 匿名送蓝色妖姬365朵
			{
				using namespace Cmd;
				stSelectFlowers22PropertyUserCmd *smu = (stSelectFlowers22PropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送蓝色妖姬哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送蓝色妖姬");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare365 found;
					                found.dwObjectID = 2900;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>365) //soke 大于365朵直接-365
		                                { 
			                                itemobj->data.dwNum-=365;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于365朵直接清除，不这样写会有BUG 365朵只减364 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=365; //soke 鲜花增加365朵
										pUser->charbase.moods+=365;   //soke 人气增加365点
									    this->charbase.flloves+=365;  //soke 爱心增加365颗

										pUser->charbase.flower5time = 0; //soke 时间清空
										pUser->charbase.flower5time += 259200; //soke 365朵蓝色妖姬时间 3天

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上365朵蓝色妖姬状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower5time/86400,(pUser->charbase.flower5time%86400)/3600,(pUser->charbase.flower5time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "神秘人士被你的魅力所倾倒送上了365朵蓝色妖姬！");
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了365朵蓝色妖姬！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESRXLL," 哇～～ 某神秘人士 向 %s 送出了整整365朵蓝色妖姬，%s 太有魅力了 ",pUser->name,pUser->name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESRXLL," 哇～～ 某神秘人士 向 %s 送出了整整365朵蓝色妖姬，%s 太有魅力了 ",pUser->name,pUser->name);
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多蓝色妖姬哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送蓝色妖姬哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS23_PROPERTY_USERCMD_PARA: //soke 匿名送蓝色妖姬999朵
			{
				using namespace Cmd;
				stSelectFlowers23PropertyUserCmd *smu = (stSelectFlowers23PropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送蓝色妖姬哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送蓝色妖姬");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare999 found;
					                found.dwObjectID = 2900;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>999) //soke 大于999朵直接-999
		                                { 
			                                itemobj->data.dwNum-=999;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于999朵直接清除，不这样写会有BUG 999朵只减998 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=999; //soke 鲜花增加9朵
										pUser->charbase.moods+=999;   //soke 人气增加9点
									    this->charbase.flloves+=999;  //soke 爱心增加9颗

										pUser->charbase.flower6time = 0; //soke 时间清空
										pUser->charbase.flower6time += 518400; //soke 999朵蓝色妖姬时间 6天

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上999朵蓝色妖姬状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower6time/86400,(pUser->charbase.flower6time%86400)/3600,(pUser->charbase.flower6time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "神秘人士被你的魅力所倾倒送上了999朵蓝色妖姬！");
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了999朵蓝色妖姬！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESRXLL," 哇～～ 某神秘人士 向 %s 送出了整整999朵蓝色妖姬，%s 好幸福哦 ",pUser->name,pUser->name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESRXLL," 哇～～ 某神秘人士 向 %s 送出了整整999朵蓝色妖姬，%s 好幸福哦 ",pUser->name,pUser->name);
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多蓝色妖姬哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送蓝色妖姬哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS41_PROPERTY_USERCMD_PARA: //soke 签名送蓝色妖姬9朵
			{
				using namespace Cmd;
				stSelectFlowers41PropertyUserCmd *smu = (stSelectFlowers41PropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送蓝色妖姬哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送蓝色妖姬");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare found;
					                found.dwObjectID = 2900;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>9) //soke 大于9朵直接-9
		                                { 
			                                itemobj->data.dwNum-=9;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于9朵直接清除，不这样写会有BUG 9朵只减8 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=9; //soke 鲜花增加9朵
										pUser->charbase.moods+=9;   //soke 人气增加9点
									    this->charbase.flloves+=9;  //soke 爱心增加9颗

										pUser->charbase.flower4time = 0; //soke 时间清空
										pUser->charbase.flower4time += 86400; //soke 9朵蓝色妖姬时间 1天

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上9朵蓝色妖姬状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower4time/86400,(pUser->charbase.flower4time%86400)/3600,(pUser->charbase.flower4time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, " %s被你的魅力所倾倒送上了9朵蓝色妖姬！",this->name);
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了9朵蓝色妖姬！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_EXP," 哇～～ %s 向 %s 真诚地献上了9朵蓝色妖姬，%s 太有魅力了 ",this->name,pUser->name,pUser->name);
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多蓝色妖姬哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送蓝色妖姬哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS51_PROPERTY_USERCMD_PARA: //soke 签名送蓝色妖姬99朵
			{
				using namespace Cmd;
				stSelectFlowers51PropertyUserCmd *smu = (stSelectFlowers51PropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送蓝色妖姬哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送蓝色妖姬");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare99 found;
					                found.dwObjectID = 2900;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>99) //soke 大于99朵直接-99
		                                { 
			                                itemobj->data.dwNum-=99;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于99朵直接清除，不这样写会有BUG 99朵只减98 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=99; //soke 鲜花增加99朵
										pUser->charbase.moods+=99;   //soke 人气增加99点
									    this->charbase.flloves+=99;  //soke 爱心增加99颗

										pUser->charbase.flower5time = 0; //soke 时间清空
										pUser->charbase.flower5time += 86400; //soke 99朵蓝色妖姬时间 1天

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上99朵蓝色妖姬状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower5time/86400,(pUser->charbase.flower5time%86400)/3600,(pUser->charbase.flower5time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, " %s被你的魅力所倾倒送上了99朵蓝色妖姬！",this->name);
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了99朵蓝色妖姬！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESRXLL," 哇～～ %s 居然向 %s 送出了99朵蓝色妖姬，%s 太有魅力了 ",this->name,pUser->name,pUser->name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESRXLL," 哇～～ %s 居然向 %s 送出了99朵蓝色妖姬，%s 太有魅力了 ",this->name,pUser->name,pUser->name);
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多蓝色妖姬哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送蓝色妖姬哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS61_PROPERTY_USERCMD_PARA: //soke 签名送蓝色妖姬365朵
			{
				using namespace Cmd;
				stSelectFlowers61PropertyUserCmd *smu = (stSelectFlowers61PropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送蓝色妖姬哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送蓝色妖姬");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare365 found;
					                found.dwObjectID = 2900;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>365) //soke 大于365朵直接-365
		                                { 
			                                itemobj->data.dwNum-=365;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于365朵直接清除，不这样写会有BUG 365朵只减364 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=365; //soke 鲜花增加365朵
										pUser->charbase.moods+=365;   //soke 人气增加365点
									    this->charbase.flloves+=365;  //soke 爱心增加365颗

										pUser->charbase.flower5time = 0; //soke 时间清空
										pUser->charbase.flower5time += 259200; //soke 365朵蓝色妖姬时间 3天

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上365朵蓝色妖姬状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower5time/86400,(pUser->charbase.flower5time%86400)/3600,(pUser->charbase.flower5time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, " %s被你的魅力所倾倒送上了365朵蓝色妖姬！",this->name);
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了365朵蓝色妖姬！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESRXLL," 哇～～ %s 居然向 %s 送出了整整365朵蓝色妖姬，%s 太有魅力了 ",this->name,pUser->name,pUser->name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESRXLL," 哇～～ %s 居然向 %s 送出了整整365朵蓝色妖姬，%s 太有魅力了 ",this->name,pUser->name,pUser->name);
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多蓝色妖姬哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送蓝色妖姬哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case SELECT_FLOWERS71_PROPERTY_USERCMD_PARA: //soke 签名送蓝色妖姬999朵
			{
				using namespace Cmd;
				stSelectFlowers71PropertyUserCmd *smu = (stSelectFlowers71PropertyUserCmd*)rev;
				switch(smu->byType)
				{
					case MAPDATATYPE_USER:
						{
							SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(smu->dwTempID);
							if(pUser)
							{
								if ( 1 == pUser->charbase.type ) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "只有男性玩家才可以送蓝色妖姬哦！");
									 return false;
								}
								if ( pUser->name == this->name) //性别判断
                                {
                                     Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给自己送蓝色妖姬");
									 return false;
								}
								if (!pUser->mask.is_masking())
								{
									// 删除任务道具，扣除钱 ---------------------
					                FObjectCompare999 found;
					                found.dwObjectID = 2900;
					                zObject *itemobj = this->packs.uom.getObject(found);// 查找道具

					                if (itemobj)
					                {
										if(itemobj->data.dwNum>999) //soke 大于999朵直接-999
		                                { 
			                                itemobj->data.dwNum-=999;
			                                Cmd::stRefCountObjectPropertyUserCmd send;
			                                send.qwThisID=itemobj->data.qwThisID;
			                                send.dwNum=itemobj->data.dwNum;
			                                sendCmdToMe(&send,sizeof(send));
		                                }
										else //soke 等于999朵直接清除，不这样写会有BUG 999朵只减998 剩下的1 在叠加就是1+1=1了
		                                { 
			                                this->packs.removeObject(itemobj);
		                                }

									    pUser->charbase.folwers+=999; //soke 鲜花增加999朵
										pUser->charbase.moods+=999;   //soke 人气增加999点
									    this->charbase.flloves+=999;  //soke 爱心增加999颗

										pUser->charbase.flower6time = 0; //soke 时间清空
										pUser->charbase.flower6time += 518400; //soke 999朵蓝色妖姬时间 6天

										Channel::sendSys(pUser,Cmd::INFO_TYPE_SYS,"您身上999朵蓝色妖姬状态还剩:%d天 %d小时 %d分钟 ",pUser->charbase.flower6time/86400,(pUser->charbase.flower6time%86400)/3600,(pUser->charbase.flower6time%3600)/60);
										Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, " %s被你的魅力所倾倒送上了999朵蓝色妖姬！",this->name);
										Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你向 %s 送出了999朵蓝色妖姬！",pUser->name);

									    Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESRXLL," 哇～～ %s 居然向 %s 送出了整整999朵蓝色妖姬，%s 好幸福哦 ",this->name,pUser->name,pUser->name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISESRXLL," 哇～～ %s 居然向 %s 送出了整整999朵蓝色妖姬，%s 好幸福哦 ",this->name,pUser->name,pUser->name);
									 }
									else
							 	    {
									    Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你没有那么多蓝色妖姬哦！");
								    }
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "不能给蒙面人送蓝色妖姬哦！");
								}
							}
						}
						break;
					case MAPDATATYPE_NPC:
						{
						}
						break;
					default:
						break;
				}
			}
			break;
		case HONOR_TO_PKVALUE_PROPERTY_USERCMD_PARA:
			{
				short good = this->charbase.goodness & 0x0000FFFF;
				short old_good = good;
				DWORD old_honor = this->charbase.honor;
				if(good > 0 && good < (short)Cmd::GOODNESS_7 && this->charbase.honor > 0)
				{
					if(good <= (short)(this->charbase.honor / 5))
					{
						this->charbase.honor -= good * 5;
						good = 0;
					}
					else
					{
						good -= (short)(this->charbase.honor / 5);
						this->charbase.honor = this->charbase.honor % 5;
					}
					this->charbase.goodness = this->charbase.goodness & 0XFFFF0000 + good;
					Cmd::stMainUserDataUserCmd  userinfo;
					full_t_MainUserData(userinfo.data);
					sendCmdToMe(&userinfo,sizeof(userinfo));
					reSendMyMapData();
					zObject::logger(0,0,"荣誉值",this->charbase.honor,old_honor - this->charbase.honor,0,this->id,this->name,0,NULL,"洗PK值扣除荣誉值",NULL,0,0);
					return Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "你用%u荣誉点缩短了%u分钟的罪恶时间",old_honor - this->charbase.honor , old_good - good);
				}
				return true;
			}
			break;
		case GO_TRAIN_PROPERTY_USERCMD_PARA:
			{
				if((privatestore.step() != PrivateStore::NONE) || tradeorder.hasBegin())//正在交易
					return true;

				Cmd::stGoTrainPropertyUserCmd * cmd = (Cmd::stGoTrainPropertyUserCmd *)rev;

				char mname[MAX_NAMESIZE];
				bzero(mname, MAX_NAMESIZE);
				DWORD mid = 192+cmd->level;
				SceneManager::getInstance().buildMapName(6, mid, mname);
				zPos p;
				switch (cmd->level)
				{
					case 1:
						p = zPos(172, 265);
						break;
					case 2:
						p = zPos(257, 144);
						break;
					case 3:
						p = zPos(34, 188);
						break;
					case 4:
						p = zPos(139, 270);
						break;
					case 5:
						p = zPos(69, 192);
						break;
					case 6:
						p = zPos(217, 208);
						break;
					case 7:
						p = zPos(245, 200);
						break;
					case 8:
						p = zPos(78, 187);
						break;
					case 9:
						p = zPos(107, 204);
						break;
					default:
						Zebra::logger->error("%s 进入修炼地图层数不对 level=%u", name, cmd->level);
						return true;
						break;
				}

				zObject * o = 0;
				if (!packs.main.getObjectByID(&o, 798))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你缺少一件必要的道具");
					return true;
				}

				if(o->data.dwNum>1) //soke 大于1朵直接-1
		        { 
			        o->data.dwNum-=1;
			        Cmd::stRefCountObjectPropertyUserCmd send;
			        send.qwThisID=o->data.qwThisID;
			        send.dwNum=o->data.dwNum;
			        sendCmdToMe(&send,sizeof(send));
		        }
				else //soke 等于1直接清除，不这样写会有BUG
		        { 
			       packs.removeObject(o, true, true);
		        }
				//packs.removeObject(o, true, true);
				charbase.trainTime = 86399;
				sendtoSelectedTrainState();

				Scene * s=SceneManager::getInstance().getSceneByName(mname);
				if (s)
				{
					if (changeMap(s, p))
						Zebra::logger->trace("%s 进入练级地图 %s(%u,%u)", name, s->name, getPos().x, getPos().y);
					else
						Zebra::logger->trace("%s 进入练级地图失败 %s(%u,%u)", name, s->name, p.x, p.y);
				}
				else
				{       
					Cmd::Session::t_changeScene_SceneSession cmd;
					cmd.id = id;
					cmd.temp_id = tempid;
					cmd.x = p.x;
					cmd.y = p.y;
					cmd.map_id = 0; 
					cmd.map_file[0] = '\0';
					strncpy((char *)cmd.map_name, mname, MAX_NAMESIZE);
					sessionClient->sendCmd(&cmd, sizeof(cmd));
				}
			}
			break;
		case GO_FUBEN_PROPERTY_USERCMD_PARA: //soke 进入副本地图
			{
				if((privatestore.step() != PrivateStore::NONE) || tradeorder.hasBegin())//正在交易
					return true;

				Cmd::stGoFubenPropertyUserCmd * cmd = (Cmd::stGoFubenPropertyUserCmd *)rev;

				if (this->charbase.fubentime <= 1 && this->charbase.fbCount > 0)
		        {
			        Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "今日开启的副本已经结束");
			        return false;
		        }
				if (this->charbase.fubentime <= 1 && this->charbase.fbCount < 1)
		        {
			        Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "今日的副本尚未开启");
			        return false;
		        }
				if (this->charbase.level < 30)
		        {
			        Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "等级不符，无法进入副本");
			        return false;
		        }

				char mname[MAX_NAMESIZE];
				bzero(mname, MAX_NAMESIZE);
				DWORD mid = 386+cmd->level;
				SceneManager::getInstance().buildMapName(6, mid, mname);
				zPos p;
				switch (cmd->level)
				{
					case 1:
						p = zPos(110, 195);
						break;
					case 2:
						p = zPos(189, 50);
						break;
					case 3:
						p = zPos(44, 142);
						break;
					
					default:
						Zebra::logger->error("%s 进入副本地图id不对 level=%u", name, cmd->level);
						return true;
						break;
				}

				zObject * o = 0;
				if (!packs.main.getObjectByID(&o, 754))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你缺少副本金币,我不能为你服务");
					return true;
				}

				if(o->data.dwNum>1) //soke 大于1朵直接-1
		        { 
			        o->data.dwNum-=1;
			        Cmd::stRefCountObjectPropertyUserCmd send;
			        send.qwThisID=o->data.qwThisID;
			        send.dwNum=o->data.dwNum;
			        sendCmdToMe(&send,sizeof(send));
		        }
				else //soke 等于1直接清除，不这样写会有BUG
		        { 
			       packs.removeObject(o, true, true);
		        }

				 //soke 增加时间显示
			    Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"今日副本时间还剩:%d小时 %d分钟 ",(charbase.fubentime%86400)/3600,(charbase.fubentime%3600)/60);

				Scene * s=SceneManager::getInstance().getSceneByName(mname);
				if (s)
				{
					if (changeMap(s, p))
						Zebra::logger->trace("%s 进入副本地图 %s(%u,%u)", name, s->name, getPos().x, getPos().y);
					else
						Zebra::logger->trace("%s 进入副本地图失败 %s(%u,%u)", name, s->name, p.x, p.y);
				}
				else
				{       
					Cmd::Session::t_changeScene_SceneSession cmd;
					cmd.id = id;
					cmd.temp_id = tempid;
					cmd.x = p.x;
					cmd.y = p.y;
					cmd.map_id = 0; 
					cmd.map_file[0] = '\0';
					strncpy((char *)cmd.map_name, mname, MAX_NAMESIZE);
					sessionClient->sendCmd(&cmd, sizeof(cmd));
				}
			}
			break;
		case GO_HANBING_PROPERTY_USERCMD_PARA: //soke 寒冰
			{
				if((privatestore.step() != PrivateStore::NONE) || tradeorder.hasBegin())//正在交易
					return true;

				Cmd::stGoHanbingPropertyUserCmd * cmd = (Cmd::stGoHanbingPropertyUserCmd *)rev;

				switch (cmd->level)
				{
				case 1:
					this->charbase.hanbing = 1;
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "开启 %d",this->charbase.hanbing);
					break;
				case 2:
					this->charbase.hanbing = 0;
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "关闭 %d",this->charbase.hanbing);
					break;
				}
			}
			break;
		case GO_ANGEL_PROPERTY_USERCMD_PARA: //soke 翅膀
			{
				if((privatestore.step() != PrivateStore::NONE) || tradeorder.hasBegin())//正在交易
					return true;

				Cmd::stGoAngelPropertyUserCmd * cmd = (Cmd::stGoAngelPropertyUserCmd *)rev;

				// switch (cmd->level)
				// {
				// case 1:
				// 	this->charbase.angel = 1;
				// 	Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "开启翅膀 %d",this->charbase.angel);
				// 	break;
				// case 2:
				// 	this->charbase.angel = 0;
				// 	Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "关闭翅膀 %d",this->charbase.angel);
				// 	break;
				// }
				if(this->charbase.angel == 0)
				{
					this->charbase.angel = 1;
					summonPet(9050, Cmd::PET_TYPE_XCARTOON2);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"你放出了神兵");
				}
				else{
					this->charbase.angel = 0;
					shouhuibaby();
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"你收回了神兵");
					
				}
			}
			break;
        // 桃子增加定时传送地图	1		
        case GO_CHUANSONG_PROPERTY_USERCMD_PARA: //sky 定时传送活动
		{
			if((privatestore.step() != PrivateStore::NONE) || tradeorder.hasBegin()) //定时传送代码
			    return true;

		
			Gm::gomap(this, "name=中立区·皇城 pos=677,611");  //自动传送地图坐标
			break;
		}
		break;
       ///////////////////////////////结束//////////////////////////////////////////////////////
        // 桃子增加定时传送地图	2		
        case GO_CHUANSONG2_PROPERTY_USERCMD_PARA: //sky 定时传送活动
		{
			if((privatestore.step() != PrivateStore::NONE) || tradeorder.hasBegin()) //定时传送代码
			    return true;

		
			Gm::gomap(this, "name=中立区·皇城 pos=829,699");  //自动传送地图坐标
			break;
		}
		break;
       ///////////////////////////////结束//////////////////////////////////////////////////////
		//国家镖车传送
	    case GO_CHUANSONG3_PROPERTY_USERCMD_PARA: //sky 国家镖车传送
		{
			if((privatestore.step() != PrivateStore::NONE) || tradeorder.hasBegin()) //定时传送代码
			    return true;

			
			Gm::gomap(this, "name=汉国·王城 pos=832,530");  //自动传送地图坐标
			break;
		}
		break;

		 case GO_CHUANSONG4_PROPERTY_USERCMD_PARA: //sky 国家镖车传送
		{
			if((privatestore.step() != PrivateStore::NONE) || tradeorder.hasBegin()) //定时传送代码
			    return true;

			
			Gm::gomap(this, "name=楚国·王城 pos=832,530");  //自动传送地图坐标
			break;
		}
		break;	   	
		 case GO_CHUANSONG5_PROPERTY_USERCMD_PARA: //sky 世界boss
		{
			if((privatestore.step() != PrivateStore::NONE) || tradeorder.hasBegin()) //定时传送代码
			    return true;

			
			Gm::gomap(this, "name=中立区·皇城 pos=825,703");  //自动传送地图坐标
			break;
		}
		break;	   	
		 case GO_CHUANSONG6_PROPERTY_USERCMD_PARA: //sky 国家镖车传送
		{
			if((privatestore.step() != PrivateStore::NONE) || tradeorder.hasBegin()) //定时传送代码
			    return true;

			
			Gm::gomap(this, "name=中立区·世外桃源 pos=188,256");  //自动传送地图坐标
			break;
		}
		break;	   			
		default:
			break;
	}
//	Zebra::logger->debug("SceneUser::doPropertyCmd\tparam:%d", rev->byParam);
	return false;
}

