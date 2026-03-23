/**
 * \file
 * \version  $Id: ScenePet.cpp  $
 * \author 
 * \date 
 * \brief 宠物类的实现
 *
 * 
 */
#include "ScenePet.h"
#include "TimeTick.h"
#include "Scene.h"
#include "Chat.h"
#include "CountryDare.h"
#include "CountryAlly.h"

//std::map<DWORD, ScenePet::petBonus> ScenePet::bonusTable;
petBonus bonusTable[] = 
{
	{0,100,100,100},
	{2,120,100,90},
	{3,120,120,100},
	{4,125,120,120}
};

/**
 * \brief 构造
 */
ScenePet::ScenePet(Scene* scene, zNpcB *npc, const t_NpcDefine *define, const SceneNpcType type, const SceneEntryType entrytype, zNpcB *abase)
:SceneNpc(scene, npc, define, type, entrytype, abase),speedUpOffMaster(false)
{
	this->type = Cmd::PET_TYPE_NOTPET;

	masterID = 0;
	masterType = 0;

	//full_PetDataStruct(petData);
	petData.id = this->id;
	strncpy(petData.name, name, MAX_NAMESIZE-1);
	petData.lv = npc->level;
	petData.hp = this->getMaxHP();
	petData.maxhp = npc->hp;
	petData.exp = 0;
	petData.maxexp = npc->exp;
	petData.atk = getMinPDamage();
	petData.maxatk = getMaxPDamage();
	petData.matk = getMinMDamage();
	petData.maxmatk = getMaxMDamage();
	petData.def = npc->maxpdefence;
	petData.mdef = npc->maxmdefence;

	petData.str = npc->str;
	petData.intel = npc->inte;
	petData.agi = npc->dex;
	petData.men = npc->men;
	petData.vit = npc->con;
	petData.cri = npc->cri;

	bzero(&petData.skills[0], sizeof(petData.skills));
	std::vector<DWORD> list;
	if (npc->getAllSkills(list, petData.lv))
	{
		for (int i=list.size()-3>0?list.size()-3:0;i<(int)list.size();i++)
			if (i>=0)
				petData.skills[i] = list[i];
	}
	petData.ai = 0x0201;

	petData.maxhp_plus = 0;
	petData.atk_plus = 0;
	petData.maxatk_plus = 0;
	petData.pdef_plus = 0;
	petData.mdef_plus = 0;
	petData.type = this->type;
	petData.state = Cmd::PET_STATE_NORMAL;

	masterIsAlive = true;
	//needSave = false;

	delCount = 0;
	switch(npc->id)
	{
		case 46061 ... 46070:
			this->catchme = 100;
			break;
		default:
			break;
	}
}

/**
 * \brief 敌我判断
 * \param entry 判断的对象
 * \param notify 判断失败时是否提示（不能攻击10级以下的玩家）
 * \return 0:友方 1:敌人 -1:中立
 */
int ScenePet::isEnemy(SceneEntryPk * entry, bool notify, bool good)
{
	int ret=0;
	bool isPlayer = false;
	if (entry)
	{
		if (type==Cmd::PET_TYPE_SEMI)
		{
			ret = SceneNpc::isEnemy(entry, notify, good);
		}
		else
		{
			SceneEntryPk * master = getMaster();
			if (master)
			{
				if (master->scene==scene)
				{
					if (scene->zPosShortRange(getPos(), master->getPos(), SCREEN_WIDTH*2, SCREEN_HEIGHT*2))
					{
						if (this!=entry)
						{
							if (!((entry->frenzy)||(frenzy)))
							{
								if (master->getType()==zSceneEntry::SceneEntry_Player)
								{
									isPlayer = true;
									//Zebra::logger->debug("isPetEnemy 119 %s->%s", master->name, entry->name);
									ret = isUserMasterEnemy(entry);
								}
								else
								{
									//Zebra::logger->debug("isPetEnemy 124 %s->%s", master->name, entry->name);
									ret = master->isEnemy(entry, notify);
								}
							}
							else
								ret = 1;
						}
						else
							ret = 0;
					}
					else
						ret = -1;
				}
				else
					ret = -1;
			}
			else
				ret = -1;
		}
	}
	else
		ret = -1;

	if (isPlayer)
	{
		if (ret==1)
			return 1;
		else
			return 0;
	}
	else
	{
		return ret;
	}
}

/**
 * \brief 当主人是玩家时，以主人的身份进行敌我判断
 * \param entry 判断的对象
 * \return 0:友方 1:敌人 -1:中立
 */
int ScenePet::isUserMasterEnemy(SceneEntryPk * entry)
{
	SceneEntryPk * temp = getTopMaster();
	SceneUser * tm = 0;//top master
	if (temp && temp->getType()==zSceneEntry::SceneEntry_Player)
		tm = (SceneUser*)temp;
	else return -1;

	// TODO 判断传入角色与主人是否为朋友关系
	if (tm==entry) return 0;

	SceneEntryPk * entryMaster = entry->getTopMaster();

	using namespace Cmd;
	//if (PKMODE_ENTIRE==pkMode) return 1;

	switch (entryMaster->getType())
	{
		case zSceneEntry::SceneEntry_Player:
			{
				SceneUser *pUser = (SceneUser *)entryMaster;
				if (pUser == tm) return 0;

				bool def_gem = false;
				bool my_gem = false;
				
				if (tm->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER)
				 || tm->issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
				{
					my_gem = true;
				}
				
				if (pUser->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER)
				 || pUser->issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
				{
					def_gem = true;
				}

				//soke 挖宝区挖宝不可被宠物攻击
				if (pUser->issetUState(Cmd::USTATE_USER_WABAO ))
	            {
                    bool isInDanceZone = scene->checkZoneType(pos, ZoneTypeDef::ZONE_DANCE);
                    if ( isInDanceZone )
                    {
					//   if(notify)
					//   Channel::sendSys(this,Cmd::INFO_TYPE_SYS,"在跳舞区挖宝的目标不可攻击");
					
			            return 0;
	                }
	            }
				if ((pUser->charbase.level<20)  && (!pUser->isWarRecord(Cmd::COUNTRY_FORMAL_DARE, tm->charbase.country)) && (!def_gem))
					return 0;
				if ((tm->charbase.level<20)  && (!tm->isWarRecord(Cmd::COUNTRY_FORMAL_DARE, pUser->charbase.country)) && (!my_gem))
					return 0;

				if(tm->scene == pUser->scene && !(my_gem || def_gem))
				{
					//if (tm->charbase.country == pUser->charbase.country)
					//{
						if(tm->charbase.level <= tm->scene->getPkLevel() && pUser->charbase.level >tm->scene->getPkLevel() 
							&& (!tm->isWarRecord(Cmd::SEPT_NPC_DARE, pUser->charbase.septid)))
						{
							return 0;
						}

						if(tm->charbase.level > tm->scene->getPkLevel() && pUser->charbase.level <= tm->scene->getPkLevel() 
							&& (!tm->isWarRecord(Cmd::SEPT_NPC_DARE, pUser->charbase.septid)))
						{
							return 0;
						}
					//}
					//else
					//{
					//	if(tm->charbase.level <= tm->scene->getPkLevel()-10 && pUser->charbase.level >tm->scene->getPkLevel()-10 
					//		&& (!tm->isWarRecord(Cmd::SEPT_NPC_DARE, pUser->charbase.septid)))
					//	{
					//		return 0;
					//	}

					//	if(tm->charbase.level > tm->scene->getPkLevel()-10 && pUser->charbase.level <= tm->scene->getPkLevel()-10 
					//		&& (!tm->isWarRecord(Cmd::SEPT_NPC_DARE, pUser->charbase.septid)))
					//	{
					//		return 0;
					//	}
					//}
				}

				//跳舞被宠物攻击检测
				if(pUser->scene->getRealMapID() == 139 )
				{
					bool isInSpaZone = scene->checkZoneType(pos, ZoneTypeDef::ZONE_PRIVATE_STORE);
					if ( pUser->isDance() )
					{
						if ( isInSpaZone )
						{
							Channel::sendSys(tm,Cmd::INFO_TYPE_SYS,"在广场跳舞的目标不可攻击!");
							return 0;
						}
					}
				}

				if (tm->isDiplomatState() ==0 || pUser->isDiplomatState() == 0)
				{
					return 0;
				}
				
				switch (tm->pkMode)
				{
					case PKMODE_NORMAL:
						{
							return 0;
						}
						break;
					case PKMODE_TEAM:
						{
							//是同一组队或者是增益类魔法
							if((tm->team.getLeader() != 0) && (pUser->team.getLeader() == tm->team.getLeader()))
								return 0;
							else
								return 1;
						}
						break;
					case PKMODE_TONG:
						{
							if(tm->charbase.unionid != 0 && tm->charbase.unionid == pUser->charbase.unionid)
								return 0;
							else
								return 1;
						}
						break;
					case PKMODE_SEPT:
						{
							if(tm->charbase.septid != 0 && tm->charbase.septid == pUser->charbase.septid)
								return 0;
							else
								return 1;
						}
						break;
					case PKMODE_COUNTRY:
						{
							if(tm->charbase.country != 0 
							&& ((tm->charbase.country == pUser->charbase.country)
					|| (CountryAllyM::getMe().getFriendLevel(tm->charbase.country, 
							pUser->charbase.country)>0
		 				&& (pUser->isSpecWar(Cmd::COUNTRY_FORMAL_DARE) 
							|| tm->isSpecWar(Cmd::COUNTRY_FORMAL_DARE)))))
								return 0;
							else
								return 1;
						}
						break;
					case PKMODE_GOODNESS:
						{
							if(pUser->isRedNamed(false)||pUser->charbase.country!=tm->charbase.country)
								return 1;
							else
								return 0;
						}
						break;
					case PKMODE_ALLY:
						{
							if ((CountryAllyM::getMe().getFriendLevel(pUser->charbase.country, tm->charbase.country)>0)||
								(pUser->charbase.country==tm->charbase.country))
								return 0;
							else
								return 1;
						}
						break;
					case PKMODE_ENTIRE:
						{
							if (pUser->isPkZone(tm)&&tm->isPkZone(pUser))
								return 1;
							else
								return 0;
						}
					default:
						break;
				}
				return 0;
			}
			break;
		case zSceneEntry::SceneEntry_NPC:
			{
				SceneNpc * n = (SceneNpc *)entry;

				if (n->id==COUNTRY_MAIN_FLAG	//这几个不在这里判断
						|| n->id==COUNTRY_SEC_FLAG
						|| n->isMainGeneral()
						|| n->id==COUNTRY_KING_MAIN_FLAG
						|| n->id==COUNTRY_KING_SEC_FLAG
						|| n->id==COUNTRY_SEC_GEN
						|| n->id==COUNTRY_EMPEROR_MAIN_GEN
						|| n->id==COUNTRY_EMPEROR_SEC_GEN)
					return 1;

				//国外npc
				if (!n->isBugbear())
				{
					if (n->npc->flags==1 && tm->charbase.country!=n->scene->getCountryID())
						return 1;
					else
						return -1;
				}

				SceneEntryPk * m = n->getMaster();
				if (m)
				{
					//Zebra::logger->debug("isPetEnemy 183 %s->%s", tm->name, n->getMaster()->name);
					if (!scene->zPosShortRange(n->getPos(), m->getPos(), 20) && n->getPetType()==Cmd::PET_TYPE_GUARDNPC)
						return 1;
					else
						return tm->isEnemy(m);
				}

				if ((n->aif&AIF_ATK_REDNAME)||(n->npc->kind==NPC_TYPE_GUARD))
				{
					if (tm->isRedNamed()) return 1;
					if (tm->charbase.country!=tm->scene->getCountryID())
						return 1;
					if (tm->charbase.goodness&Cmd::GOODNESS_ATT)
						return 1;
				}
				switch (n->npc->kind)
				{
					case NPC_TYPE_HUMAN:                    ///人型
					case NPC_TYPE_NORMAL:                   /// 普通类型
					case NPC_TYPE_BBOSS:                    /// 大Boss类型
					case NPC_TYPE_LBOSS:                    /// 小Boss类型
					case NPC_TYPE_PBOSS:                    /// 紫Boss类型
					case NPC_TYPE_REDBOSS:			/// 红Boss类型
					case NPC_TYPE_BACKBONE:                 /// 精英类型
					case NPC_TYPE_GOLD:                             /// 黄金类型
					case NPC_TYPE_SUMMONS:                  /// 召唤类型
					case NPC_TYPE_AGGRANDIZEMENT:   /// 强化类型
					case NPC_TYPE_ABERRANCE:                /// 变异类型
					case NPC_TYPE_BACKBONEBUG:              /// 精怪类型
					case NPC_TYPE_PET:      /// 宠物类型
					case NPC_TYPE_TOTEM:                    /// 图腾类型
						//case NPC_TYPE_DUCKHIT:    /// 花草
						return 1;
					case NPC_TYPE_GUARD:    /// 士兵类型
					case NPC_TYPE_SOLDIER:    /// 士兵类型
						{
							if (tm->charbase.country!=scene->getCountryID())
								return 1;
							if (tm->pkMode==PKMODE_ENTIRE)
								return 1;
							return 0;
						}
					case NPC_TYPE_UNIONGUARD:
						if (tm->isAtt(Cmd::UNION_CITY_DARE))
							return 1;
						else
							if (tm->scene->getUnionDare() && !tm->isSpecWar(Cmd::UNION_CITY_DARE)
									&& !n->isMainGeneral())//大将军第三方不能打
								return 1;//中立方
							else                                                    
								return 0;//城战期间打城战而且不是攻方，就是守方
						break;
					case NPC_TYPE_UNIONATTACKER:
						if (tm->isAtt(Cmd::UNION_CITY_DARE))
							return 0;
						else
							if (tm->scene->getUnionDare() && !tm->isSpecWar(Cmd::UNION_CITY_DARE))
								return 1;//中立方
							else
								return 1;//城战期间打城战而且不是攻方，就是守方
						break;
					default:
						return -1;
				}
				return -1;
			}
			break;
		default:
			return -1;
			break;
	}

}

/**
 * \brief 设置宠物行为方式
 * \param mode 模式
 */
void ScenePet::setPetAI(Cmd::petAIMode mode)
{
	if (mode&0xff00)//设置攻击模式
	{
		petData.ai &= 0x00ff;
		petData.ai |= mode;
		if (petData.ai&(Cmd::PETAI_ATK_NOATK|Cmd::PETAI_ATK_PASSIVE))
		{
			unChaseUser();
			setRecoverTime(SceneTimeTick::currentTime, 3000);
		}
	}       
	if (mode&0x00ff)//设置移动模式
	{
		petData.ai &= 0xff00;
		petData.ai |= mode;
	}

	for (std::list<ScenePet *>::iterator it=totems.begin(); it!=totems.end(); it++)
	{
		(*it)->setPetAI((Cmd::petAIMode)petData.ai);
	}
	if (pet) pet->setPetAI((Cmd::petAIMode)petData.ai);
	if (summon) summon->setPetAI((Cmd::petAIMode)petData.ai);
}

/**
 * \brief 得到宠物行为方式
 * \return 模式
 */
WORD ScenePet::getPetAI()
{
	return petData.ai;
}

/**
 * \brief 向主人移动
 *
 * \return 是否移动成功
 */
bool ScenePet::moveToMaster()
{
#ifdef _XWL_DEBUG
	//Channel::sendNine(this, "ScenePet::moveToMaster()");
#endif
	if (type==Cmd::PET_TYPE_SEMI) return false;
	SceneEntryPk * master = getMaster();
	if (!master) return false;

	AIC->setActRegion(master->getPos(), 2, 2);

	if (!canMove())
	{
		check();
		if (getMaster())
			return false;
		else
			return true;
	}

	//check();
	if (checkMoveTime(SceneTimeTick::currentTime))
	{
		if (master->scene == scene)
		{
			check();
			if (0==masterID) return true;//运镖成功删除了主人

			//超出瞬移范围
			if (!(scene->zPosShortRange(getPos(), master->getPos(), npc_pet_warp_region)))
				return warp(master->getPos());

			//离开较远距离
			int region = isFighting()?npc_pet_run_region+4:npc_pet_run_region;
			if (!(scene->zPosShortRange(getPos(), master->getPos(), region)))
			{
				if (!speedUpOffMaster)
				{
					setSpeedRate(getSpeedRate()*2.0);
					speedUpOffMaster = true;
				}
			}
			else
			{
				//在主人身边
				region = isFighting()?npc_pet_chase_region+5:npc_pet_chase_region;
				if ((scene->zPosShortRange(getPos(), master->getPos(), region)))
				{
					if (speedUpOffMaster)
					{
						resetSpeedRate();
						speedUpOffMaster = false;
					}
					//check();
					return false;
				}
			}

			unChaseUser();
			if (aif&AIF_WARP_MOVE)
				return warp(master->getPos());
			else
			{
				if (!gotoFindPath(getPos(), master->getPos()))
					return goTo(master->getPos());
			}
		}
		else
		{
			if (master->scene)
			{
				if (changeMap(master->scene, master->getPos()))
				{
					check();
					return true;
				}
				else
					return false;

			}
		}
	}
	return false;
}

/**
 * \brief 得到主人的指针
 *
 * \return 主人的指针
 * 
 */
SceneEntryPk * ScenePet::getMaster()
{
	if (needClear()) return 0;

	SceneEntryPk * m = 0;
	switch (masterType)
	{
		case zSceneEntry::SceneEntry_Player:
			m = SceneUserManager::getMe().getUserByID(masterID);
			break;
		case zSceneEntry::SceneEntry_NPC:
			m = SceneNpcManager::getMe().getNpcByTempID(masterID);
			break;
		default:
			break;
	}
	if (!m && Cmd::PET_TYPE_GUARDNPC!=getPetType())
	{
#ifdef _XWL_DEBUG
		//Zebra::logger->trace("%s 找不到主人 masterTpye=%u masterID=%u needClear=%u", name, masterType, masterID, needClear());
#endif

		delCount++;
		if (delCount==100)
			Zebra::logger->trace("%s 找不到主人 masterTpye=%u masterID=%u needClear=%u %s(%u,%u)", name, masterType, masterID, needClear(), scene->name, pos.x, pos.y);

		//if (delCount>=10 && !needClear())
		//	delMyself();
	}
	return m;
}

/**
 * \brief 得到最上层主人的指针
 * 没有主人返回自己
 * \return 主人的指针
 * 
 */
SceneEntryPk * ScenePet::getTopMaster()
{
	if (type==Cmd::PET_TYPE_SEMI) return this;
	SceneEntryPk * m = this;
	while ((m->getType()!=zSceneEntry::SceneEntry_Player) && (m->getMaster()))
		m = m->getMaster();
	if (!m) return this;
	return m;
}

/**
 * \brief 返回是否红名
 *
 * \return 是否红名
 */
bool ScenePet::isRedNamed(bool allRedMode)
{
	if (type==Cmd::PET_TYPE_SEMI) return false;
	SceneEntryPk * master = getMaster();
	if (master)
		return master->isRedNamed(allRedMode);
	//Zebra::logger->error("ScenePet::isRedNamed(): %s 没有主人", name);
	return false;
}

/**
 * \brief 回到活动范围
 *
 */
void ScenePet::returnToRegion()
{
	if (type==Cmd::PET_TYPE_SEMI) return;
	SceneEntryPk * master = getMaster();
	if (master)
	{
		if (!scene->zPosShortRange(master->getPos(), getPos(), SceneNpc::npc_pet_warp_region))
		{
			warp(master->getPos());
			return;
		}
		setSpeedRate(getSpeedRate()*2.0);
		return;
	}
}

/**
 * \brief 填充宠物信息结构
 *
 *
 * \param data 结构地址
 */
void ScenePet::full_PetDataStruct(Cmd::t_PetData & data)
{
	petData.maxhp_plus = getMaxHP() - petData.maxhp;
	petData.atk_plus = getMinPDamage() - petData.atk;
	petData.maxatk_plus = getMaxPDamage() - petData.maxatk;
	petData.matk_plus = getMinMDamage() - petData.matk;
	petData.maxmatk_plus = getMaxMDamage() - petData.maxmatk;
	petData.pdef_plus = getMaxPDefence() - petData.def;
	petData.mdef_plus = getMaxMDefence() - petData.mdef;

	if (hp>getMaxHP())
		hp = getMaxHP();
	petData.hp = hp;
#ifdef _DEBUGLOG
	Zebra::logger->debug("发送宠物hp plus=%u getMaxHP=%u - petData.maxhp=%u", petData.maxhp_plus, getMaxHP(), petData.maxhp);
#endif
	bcopy(&petData, &data, sizeof(petData));
}

/**
 * \brief 是否主动攻击
 *
 * \return 是否主动攻击
 */
bool ScenePet::isActive()
{
	//if (aif&AIF_ACTIVE_MODE) return true;
	if (petData.ai&Cmd::PETAI_ATK_ACTIVE) return true;
	return false;
	//return ((aif&AIF_ACTIVE_MODE)||(petData.ai&Cmd::PETAI_ATK_ACTIVE));
}

/**
 * \brief 是否可以战斗
 *
 * \return 是否可以战斗
 */
bool ScenePet::canFight()
{
	if (aif&AIF_NO_BATTLE) return false;
	if (petData.ai&Cmd::PETAI_ATK_NOATK) return false;
	return true;
	//return ((!(aif&AIF_NO_BATTLE))||(petData.ai&Cmd::PETAI_ATK_PASSIVE));
}

/**
 * \brief 是否可以移动
 *
 * \return 是否可以移动
 */
bool ScenePet::canMove()
{
	if (aif&AIF_NO_MOVE) return false;
	if (petData.ai&Cmd::PETAI_MOVE_STAND) return false;
	return moveAction;
	//return ((!(aif&AIF_NO_MOVE))||(petData.ai&Cmd::PETAI_MOVE_STAND));
}

/**
 * \brief 设置主人
 *
 *
 * \param m 主人指针
 */
void ScenePet::setMaster(SceneEntryPk * m)
{
	if (!m) return;

	masterType = m->getType();;
	if (masterType==zSceneEntry::SceneEntry_Player)
		masterID = m->id;
	else
		masterID = m->tempid;

	if (this->type == Cmd::PET_TYPE_SUMMON)
		m->getSummonAppendDamage(this->appendMinDamage, this->appendMaxDamage);
}

/**
 * \brief 设置主人
 *
 *
 * \param master 主人指针
 */
void ScenePet::setMaster(DWORD id, DWORD type)
{
	if (type!=zSceneEntry::SceneEntry_Player && type!=zSceneEntry::SceneEntry_NPC)
	{
		Zebra::logger->trace("ScenePet::setMaster(): 企图给 %s 设置非法的主人类型 type=%u", name, type);
		return;
	}

	masterID = id;
	masterType = type;

	if (this->type==Cmd::PET_TYPE_SUMMON && getMaster())
		getMaster()->getSummonAppendDamage(appendMinDamage, appendMaxDamage);
}

/**
 * \brief 清除主人指针
 *
 */
void ScenePet::clearMaster()
{
	masterID = 0;
	masterType = 0;
}

/**
 * \brief 设置宠物类型
 *
 *
 * \param petType 宠物类型
 */
void ScenePet::setPetType(Cmd::petType petType)
{
	type = petType;
	petData.type = petType;
}

/**
 * \brief 获取宠物类型
 *
 *
 * \return 宠物类型
 */
Cmd::petType ScenePet::getPetType()
{
	return type;
}

/**
 * \brief 查找主人的敌人
 *
 *
 * \param ret 找到的目标
 * \return 是否找到
 */
bool ScenePet::checkMasterTarget(SceneEntryPk *&ret)
{
	if ((Cmd::PET_TYPE_RIDE==getPetType())
			|| (Cmd::PET_TYPE_GUARDNPC==getPetType())
			|| (Cmd::PET_TYPE_SEMI==getPetType()))
	{
		ret = 0;
		return false;
	}

	SceneEntryPk * master = getMaster();
	if (master)                     
	{                                       
		int r = npc_search_region;
		if (aif&AIF_DOUBLE_REGION)
			r *= 2;

		SceneEntryPk * tmp = 0;
		if (petData.ai&Cmd::PETAI_ATK_ACTIVE)//主人攻击的对象
			tmp = master->getCurTarget();
		else if (petData.ai&Cmd::PETAI_ATK_PASSIVE)//攻击主人的对象
			tmp = master->getDefTarget();

		if ((tmp)&&(tmp!=this))
			if (tmp->getState() == zSceneEntry::SceneEntry_Normal
					&& isEnemy(tmp)
					&& canReach(tmp)
					&& canFight())
			{
				ret = tmp;
				return true;
			}
		return false;
	} 
	//Zebra::logger->error("ScenePet::checkMasterTarget(): %s 没有主人", name);
	ret = 0;
	return false;
}

/**
 * \brief 处理宠物死亡
 *
 */
void ScenePet::petDeath()
{
	SceneEntryPk * master = getMaster();
	if (master)
	{
		/* //马不会死了
		   if (Cmd::PET_TYPE_RIDE==type&&zSceneEntry::SceneEntry_Player==master->getType())
		   {
		   SceneUser * m = (SceneUser *)master;
		   Cmd::stDelPetPetCmd del;
		   del.id= tempid;
		   del.type = Cmd::PET_TYPE_RIDE;
		   m->sendCmdToMe(&del, sizeof(del));

		   m->horse.horse(0);
		   Zebra::logger->trace("%s 的 %s 死亡", m->name, name);
		   }
		   */
		petData.state = Cmd::PET_STATE_DEAD;
		master->killOnePet(this);
	}
	setMoveTime(SceneTimeTick::currentTime, define->interval*1000);
}

/**
 * \brief 向客户端发送自己的数据
 *
 */
void ScenePet::sendData()
{
	SceneEntryPk * master = getMaster();
	if (!master) return;

	if (Cmd::PET_TYPE_RIDE==type)
	{
		((SceneUser *)master)->horse.sendData();
	}
	else
	{
		if (zSceneEntry::SceneEntry_Player==master->getType())
		{
			Cmd::stRefreshDataPetCmd ref;
			ref.type = type;
			ref.id = tempid;
			full_PetDataStruct(ref.data);
#ifdef _XWL_DEBUG
			Zebra::logger->debug("发送宠物信息 name=%s ai=%x hp=%u maxhp=%u", name, petData.ai, petData.hp, petData.maxhp);
#endif
			((SceneUser *)master)->sendCmdToMe(&ref, sizeof(ref));
		}
	}
}

/**
 * \brief 向客户端发送自己的血和经验
 *
 */
void ScenePet::sendHpExp()
{
	SceneEntryPk * master = getMaster();
	if (!master) return;

	if (zSceneEntry::SceneEntry_Player==master->getType())
	{
		Cmd::stHpExpPetCmd ref;
		ref.type = type;
		ref.id = tempid;

		if (hp>getMaxHP())
			hp = getMaxHP();
		ref.hp = petData.hp = hp;
		ref.exp = petData.exp;
		((SceneUser *)master)->sendCmdToMe(&ref, sizeof(ref));
	}
}

/**
 * \brief 得到宠物的等级
 *
 * \return 等级
 */
DWORD ScenePet::getLevel() const
{
	return petData.lv;
}

/**
 * \brief 逃离敌人
 *
 */
bool ScenePet::runOffEnemy(SceneEntryPk_vec& enemies)
{
	return false;
}

/**
 * \brief 在主人周围2格内随机移动
 * 离主人只有1格时随便走，超过1格就要检测下一步是否在活动范围内
 *
 * \return 是否移动成功
 */
bool ScenePet::randomMove()
{
	if (type==Cmd::PET_TYPE_SEMI) return SceneNpc::randomMove();
	if (!canMove()) return false;
	SceneEntryPk * master = getMaster();
	if (!master) return false;

	if (checkMoveTime(SceneTimeTick::currentTime) && canMove())
		if (zMisc::selectByPercent(2)
				|| ((getPetType()==Cmd::PET_TYPE_CARTOON || getPetType()==Cmd::PET_TYPE_RIDE) 
					&& scene->checkBlock(getPos())
					&& scene->zPosShortRange(getPos(), master->getPos(), 1)))
		{
			int dir = zMisc::randBetween(0, 7);
			zPos newPos;
			scene->getNextPos(pos, dir, newPos);
			zPosI newPosI = 0;
			scene->zPos2zPosI(newPos, newPosI);
			if (getPosI()==newPosI//不切屏
					&& scene->zPosShortRange(master->getPos(), newPos, SceneNpc::npc_pet_chase_region))//主人2格内
				return shiftMove(dir);//随机移动
		}
	return false;
}

/**
 * \brief 设置主人棍类附加攻击力
 * \param mindamage 最小攻击力
 * \param maxdamage 最大攻击力
 */
void ScenePet::setAppendDamage(DWORD mindamage, DWORD maxdamage)
{
	appendMinDamage = mindamage;
	appendMaxDamage = maxdamage;
	this->sendData();
}

/**
 * \brief 增加宠物经验
 *
 *
 * \param num 数量
 * \return 是否可以升级
 */
bool ScenePet::addExp(DWORD num)
{
	if (0==num) return false;
	if (getPetType()!=Cmd::PET_TYPE_PET
			&& getPetType()!=Cmd::PET_TYPE_SUMMON
			&& getPetType()!=Cmd::PET_TYPE_CARTOON) return false;

	petData.exp += num;
	bool ret = false;
	if (petData.exp>=petData.maxexp)
	{
		levelUp();
		sendData();
	}
	else
		sendHpExp();

	addPetExp(num);

	return ret;
}

/**
 * \brief 增加宠物的经验
 *
 *
 * \param num 数量
 */
void ScenePet::addPetExp(DWORD num)
{
	/*
	   return;//不用
	   if (summon)
	   if (summon->addExp(num))
	   petLevelUp(summon);
	   */
}

/**
 * \brief 回血
 *
 * \return 是否成功
 */
bool ScenePet::recover()
{
	SceneNpc::recover();
	if ((petData.ai&Cmd::PETAI_ATK_NOATK)&&(checkRecoverTime(SceneTimeTick::currentTime)))
	{
		int hpr = this->getMaxHP()*2/100;
		if (hp+hpr>=this->getMaxHP())//npc->hp)
			hp = this->getMaxHP();
		else
			hp += hpr;
		setRecoverTime(SceneTimeTick::currentTime, 3000);
		return true;
	}
	return false;
}

/**
 * \brief 获取主人的法力值
 * \return 主人法力值
 */
DWORD ScenePet::getMasterMana()
{
	DWORD mana = 0;
	SceneEntryPk * master = getMaster();
	if (master)
		mana= master->getMP();
	return mana;
}

/**
 * \brief 改变并刷新角色属性
 * \author fqnewman
 */
void  ScenePet::changeAndRefreshHMS(bool lock, bool sendData)
{
	this->resetSpeedRate();
	this->sendData();
	this->reSendData = false;
}

/**
 * \brief 检查是否在PK区域
 * \param other PK相关人
 * \return 是否在PK区域
 */
bool ScenePet::isPkZone(SceneEntryPk *other)
{
	SceneEntryPk * master = getMaster();
	if (!master) return false;

	return master->isPkZone(other);
}

/**
 * \brief 选择敌人
 *
 * \return 敌人的指针
 */
SceneEntryPk * ScenePet::chooseEnemy(SceneEntryPk_vec &enemies)
{
	if (type==Cmd::PET_TYPE_SEMI) return SceneNpc::chooseEnemy(enemies);
	//非战斗npc                     
	if (!canFight()) return false;          

	SceneEntryPk * ret = 0;                 

	SceneEntryPk * ct = 0;
	checkChaseAttackTarget(ct);
	if (ct && lockTarget)
	{
		return ct;
	}

	checkMasterTarget(ret);
	if (ret)
	{
		chaseSceneEntry(ret->getType(), ret->tempid);
		return ret;
	}
	else
		return SceneNpc::chooseEnemy(enemies);
}

/*
 * \brief 得到最小物理攻击力
 *
 *
 * \return 最小物理攻击力
 */
DWORD ScenePet::getMinPDamage()
{
  SDWORD value = 0;
  switch (type)
  {
    
    case Cmd::PET_TYPE_PET:
      value =  (SDWORD)((petData.atk+appendMinDamage+skillValue.uppetdamage-skillValue.dpdam-skillValue.theurgy_dpdam)*(1.0f+((float)boostupPet+(float)skillValue.protectUpAtt)/100.0f));
      if (value <0) value = 0;
      return value;
      break;
    case Cmd::PET_TYPE_SUMMON:
      return (DWORD)(SceneNpc::getMinPDamage()+boostupSummon);
      break;
    default:
      return SceneNpc::getMinPDamage();
  }
}

/*
 * \brief 得到最大物理攻击力
 *
 *
 * \return 最大物理攻击力
 */
DWORD ScenePet::getMaxPDamage()
{
  SDWORD value = 0;
  switch (type)
  {
    case Cmd::PET_TYPE_PET:
      value = (SDWORD)((petData.maxatk+appendMaxDamage+skillValue.uppetdamage-skillValue.dpdam-skillValue.theurgy_dpdam)*(1.0f+((float)boostupPet+(float)skillValue.protectUpAtt)/100.0f));
      if (value <0) value =0;
      return value;
      break;
    case Cmd::PET_TYPE_SUMMON:
      return (DWORD)( SceneNpc::getMaxPDamage()+boostupSummon);
      break;
    default:
      return SceneNpc::getMaxPDamage();
  }
}

/*
 * \brief 得到最小魔法攻击力
 *
 *
 * \return 最小魔法攻击力
 */
DWORD ScenePet::getMinMDamage()
{
  SDWORD value = 0;
  switch (type)
  {
    case Cmd::PET_TYPE_PET:
      value = (SDWORD)((petData.matk+appendMinDamage+skillValue.uppetdamage-skillValue.dmdam-skillValue.theurgy_dmdam)*(1.0f+((float)boostupPet+(float)skillValue.protectUpAtt)/100.0f));
      if (value<0) value =0;
      return value;
      break;
    case Cmd::PET_TYPE_SUMMON:
      return (DWORD)(SceneNpc::getMinMDamage()+boostupSummon);
      break;
    default:
      return SceneNpc::getMinMDamage();
  }
}

/*
 * \brief 得到最大魔法攻击力
 *
 *
 * \return 最大魔法攻击力
 */
DWORD ScenePet::getMaxMDamage()
{
  SDWORD value = 0;
  switch (type)
  {
    case Cmd::PET_TYPE_PET:
      value = (SDWORD)((petData.maxmatk+appendMinDamage+skillValue.uppetdamage-skillValue.dmdam-skillValue.theurgy_dmdam)*(1.0f+((float)boostupPet+(float)skillValue.protectUpAtt)/100.0f));
      if (value <0) value =0;
      return value;
      break;
    case Cmd::PET_TYPE_SUMMON:
      return (DWORD)(SceneNpc::getMaxMDamage()+boostupSummon);
      break;
    default:
      return SceneNpc::getMaxMDamage();
  }
}

/*
 * \brief 得到最小物理防御力
 *
 *
 * \return 最小物理防御力
 */
DWORD ScenePet::getMinPDefence()
{
	if (type==Cmd::PET_TYPE_PET)
	{
		SDWORD value = (SDWORD)((petData.def+skillValue.uppetdefence-skillValue.theurgy_dpdef)*(1.0f+((float)boostupPet)/100.0f));
		if (value <0) value =0;
		return value;
	}
	else
		return SceneNpc::getMinPDefence();
}

/*
 * \brief 得到最大物理防御力
 *
 *
 * \return 最大物理防御力
 */
DWORD ScenePet::getMaxPDefence()
{
	if (type==Cmd::PET_TYPE_PET)
	{
		SDWORD value = (SDWORD)((petData.def+skillValue.uppetdefence-skillValue.theurgy_dpdef)*(1.0f+((float)boostupPet)/100.0f));
		if (value <0) value = 0;
		return value;
	}
	else
		return SceneNpc::getMaxPDefence();
}

/*
 * \brief 得到最大魔法防御力
 *
 *
 * \return 最大魔法防御力
 */
DWORD ScenePet::getMinMDefence()
{
	if (type==Cmd::PET_TYPE_PET)
	{
		SDWORD value = (SDWORD)((petData.mdef+skillValue.uppetdefence+this->boostupPetMDef-skillValue.theurgy_dmdef)*(1.0f+((float)boostupPet)/100.0f));
		value = (SDWORD)(value *( 1 - skillValue.dmdefp/100.0f));
		if (value <0) value =0;
		return value;
	}
	else
		return SceneNpc::getMinMDefence();
}

/*
 * \brief 得到最大魔法防御力
 *
 *
 * \return 最大魔法防御力
 */
DWORD ScenePet::getMaxMDefence()
{
	if (type==Cmd::PET_TYPE_PET)
	{
		SDWORD value = (SDWORD)((petData.mdef+skillValue.uppetdefence+this->boostupPetMDef-skillValue.theurgy_dmdef)*(1.0f+((float)boostupPet)/100.0f));
		value = (SDWORD)(value *( 1 - skillValue.dmdefp/100.0f));
		if (value <0) value =0;
		return value;
	}
	else
		return SceneNpc::getMaxMDefence();
}

/*
 * \brief 从表格读取宠物的能力
 *
 */
void ScenePet::getAbilityByLevel(DWORD level)
{
	BYTE aType = getAType();

	if (aType>0)
	{
		zPetB * base = petbm.get((aType-1)*200+level);
		if (!base) return;

		petData.hp = base->hp;
		hp = base->hp;
		petData.maxhp = base->hp;
		petData.maxexp = base->exp;
		petData.atk = base->atk;
		petData.maxatk = base->maxatk;
		petData.matk = base->matk;
		petData.maxmatk = base->maxmatk;
		petData.def = base->def;
		petData.mdef = base->mdef;

		petData.str = base->str;
		petData.intel = base->intel;
		petData.agi = base->agi;
		petData.men = base->men;
		petData.vit = base->vit;
		petData.cri = base->cri;

		for (DWORD i=1; i<(sizeof(bonusTable)/sizeof(petBonus)); i++)
		{
			if (npc->bear_type==bonusTable[i].type)
			{
				petData.maxhp = petData.maxhp*bonusTable[i].hpB/100;
				petData.hp = petData.maxhp;
				hp = petData.maxhp;

				petData.atk = petData.atk*bonusTable[i].atkB/100;
				petData.maxatk = petData.maxatk*bonusTable[i].atkB/100;
				petData.matk = petData.matk*bonusTable[i].atkB/100;
				petData.maxmatk = petData.maxmatk*bonusTable[i].atkB/100;

				petData.def = petData.def*bonusTable[i].defB/100;
				petData.mdef = petData.mdef*bonusTable[i].defB/100;
			}
		}

		bzero(&petData.skills[0], sizeof(petData.skills));
		std::vector<DWORD> list;
		if (npc->getAllSkills(list, level))
		{
			for (int i=list.size()-3>0?list.size()-3:0;i<(int)list.size();i++)
				if (i>=0)
					petData.skills[i] = list[i];
		}
	}
	else
		Zebra::logger->debug("npc %s 未知的攻击类型 type=%d", name, aType);
}

/*
 * \brief 宠物升级
 *
 *
 */
void ScenePet::levelUp()
{
	SceneEntryPk * master = getMaster();
	if (!master) return;
	if (master->getType()!=zSceneEntry::SceneEntry_Player) return;

	DWORD maxlv = 1;
	switch (getPetType())
	{
		case Cmd::PET_TYPE_PET:
			{
				maxlv = ((SceneUser *)master)->charbase.level+5;
				while ((petData.exp>=petData.maxexp)&&(petData.lv<maxlv))
				{ 
					petData.exp -= petData.maxexp;
					petData.lv++;
					getAbilityByLevel(petData.lv);
					hp = getMaxHP();
				}
				if (petData.exp>petData.maxexp)
					petData.exp = petData.maxexp;
			}
			break;
		case Cmd::PET_TYPE_SUMMON:
			{
				maxlv = 2;
				zSkill *s = master->usm.findSkill(npc->soulrate);
				if (s) maxlv = s->actionbase->level+2;
				while ((petData.exp>=petData.maxexp)&&(petData.lv<maxlv))
				{
					zNpcB *base = npcbm.get(id+1);
					if (!base) break;

					petData.exp -= petData.maxexp;
					petData.maxexp = base->exp;
					petData.lv++;
					npc = base;
					id++;
					petData.id++; 
					hp = npc->hp;
				}
				if (petData.exp>petData.maxexp)
					petData.exp = petData.maxexp;
			}
			break;
		case Cmd::PET_TYPE_CARTOON:
			{
				maxlv = ((SceneUser *)master)->charbase.level;
				while ((petData.exp>=petData.maxexp)&&(petData.lv<maxlv))
				{
					petData.exp -= petData.maxexp;
					petData.lv++;
					/*
					   petData.maxexp = base->exp;
					   npc = base;
					   id++;
					   petData.id++; 
					   hp = npc->hp;
					   */
				}
				if (petData.exp>petData.maxexp)
					petData.exp = petData.maxexp;
			}
			break;
		case Cmd::PET_TYPE_GUARDNPC:
		case Cmd::PET_TYPE_RIDE:
		default:
			return;
	}
	sendMeToNine();
}

DWORD ScenePet::getBaseMaxHP()
{
	if (type==Cmd::PET_TYPE_PET)
		return (DWORD)(petData.maxhp+(anpc?anpc->hp:0));
	else
		return SceneNpc::getBaseMaxHP();
}

/*
 * \brief 得到宠物的最大生命值
 *
 * 
 * \return 最大生命值
 */
DWORD ScenePet::getMaxHP()
{
	if (type==Cmd::PET_TYPE_PET)
		return (DWORD)((petData.maxhp+(anpc?anpc->hp:0)+skillValue.maxhp)*(1.0f+boostupPet/100.0f));
	else
		return SceneNpc::getMaxHP();
}

void ScenePet::full_t_MapPetData(Cmd::t_MapPetData &data)
{
	data.tempID = tempid;
	strncpy(data.name, petData.name, MAX_NAMESIZE);

	bzero(data.masterName, sizeof(data.masterName));
	if (getMaster()&&getPetType()!=Cmd::PET_TYPE_SEMI)
	{
		data.masterType = getMaster()->getType();
		data.masterID = getMaster()->tempid;
		if (zSceneEntry::SceneEntry_Player==data.masterType
				&&((SceneUser *)getMaster())->mask.is_masking())
			strncpy(data.masterName, "蒙面人", MAX_NAMESIZE-1);
		else
			strncpy(data.masterName, getMaster()->name, MAX_NAMESIZE-1);
	}
	else
		data.masterID = (DWORD)-1;
	data.pet_type = getPetType();
}

void ScenePet::sendMeToNine()
{
	Cmd::stAddMapNpcPetMapScreenUserCmd ret; 
	full_t_MapNpcDataAndPos(ret.data);
	full_t_MapPetData(ret.pet);
	if (npc->kind == NPC_TYPE_TRAP)
	{
		SceneEntryPk * pMaster = getMaster();
		if (pMaster && pMaster->getType()==zSceneEntry::SceneEntry_Player)
		{
			((SceneUser *)pMaster)->sendCmdToMe(&ret,sizeof(ret));
		}
	}
	else
	{
		scene->sendCmdToNine(getPosI(), &ret,sizeof(ret));
	}
}

//向9屏发送宠物数据
void ScenePet::sendPetDataToNine()
{
	Cmd::stAddMapPetMapScreenUserCmd ret;
	full_t_MapPetData(ret.pet);
	if (npc->kind == NPC_TYPE_TRAP)
	{
		SceneEntryPk * pMaster = getMaster();
		if (pMaster && pMaster->getType()==zSceneEntry::SceneEntry_Player)
		{
			((SceneUser *)pMaster)->sendCmdToMe(&ret,sizeof(ret));
		}
	}
	else
	{
		scene->sendCmdToNine(getPosI(), &ret,sizeof(ret));
	}
}

void ScenePet::delMyself()
{
	if (masterType==zSceneEntry::SceneEntry_Player)
		Zebra::logger->debug("[宠物]pet %s 因找不到主人(%u)而删除 %s(%u,%u)", name, masterID, scene->name, pos.x, pos.y);
	else
		Zebra::logger->debug("[宠物]pet %s 因找不到主人而删除 %s(%u,%u)", name, scene->name, pos.x, pos.y);
	setClearState();
}
