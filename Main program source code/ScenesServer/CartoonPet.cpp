#include "CartoonPet.h"
#include "SceneUser.h"
#include "SessionClient.h"
#include "TimeTick.h"
#include "Chat.h"

/*
 * \brief 构造
 *
 */
CartoonPet::CartoonPet(Scene *scene, zNpcB *npc, const t_NpcDefine *define, const SceneNpcType type, const SceneEntryType entrytype, zNpcB *abase) : ScenePet(scene, npc, define, type, entrytype, abase),_5_sec(5)
{
	//needSave = false;

	bzero(&cartoonData, sizeof(cartoonData));
	cartoonData.npcID = npc->id;
	cartoonID = 0;
	expRate = 3;

	_5_sec_count = 0;

	setPetAI((Cmd::petAIMode)(Cmd::PETAI_MOVE_FOLLOW|Cmd::PETAI_ATK_NOATK));
}

/*
 * \brief 设置宝宝主人
 * 
 * \param master 主人
 */
void CartoonPet::setMaster(SceneEntryPk * master)
{
	if (!master) return;
	ScenePet::setMaster(master);
	cartoonData.masterID = master->id;
}

/*
 * \brief 设置宝宝主人
 * 
 * \param id 主人ID
 * \param type 主人类型
 */
void CartoonPet::setMaster(DWORD id, DWORD type)
{
	ScenePet::setMaster(id, type);

	cartoonData.masterID = id;
}

/*
 * \brief 设置宝宝ID
 * 
 * \param id ID
 */
void CartoonPet::setCartoonID(DWORD id)
{
	cartoonID = id;
}

/*
 * \brief 设置释放经验的速率
 * 
 * \param rate 速率，一次释放数量：等级*速率
 */
void CartoonPet::setExpRate(BYTE rate)
{
	expRate = rate;
	if (3==rate)
		Channel::sendNine(this, "HOHO~我的主人上线了，我要加油，获得1.5倍的经验~");
	else if (2==rate)
		Channel::sendNine(this, "OH~主人下线了，真没劲~");
#ifdef _XWL_DEBUG
	//Zebra::logger->debug("[宠物]%s 设置经验倍率 %u", name, expRate);
#endif
}

/*
 * \brief 收起,收起时要保存
 * 
 * \param saveType 保存类型
 */
void CartoonPet::putAway(Cmd::Session::saveType saveType)
{
	SceneUser * master = (SceneUser *)getMaster();
	if (!master) return;

	cartoonData.repair = 0;
	if (saveType!=Cmd::Session::SAVE_TYPE_DONTSAVE)
		save(saveType);
	master->killOnePet(this);
	setClearState();
}

/*
 * \brief 是被收养的还是自己的
 * 
 * \return 是被收养的还是自己的
 */
bool CartoonPet::isAdopted()
{
	return cartoonData.state == Cmd::CARTOON_STATE_ADOPTED;
}

/*
 * \brief 保存数据到数据库
 * \param type 保存类型
 * 
 */
void CartoonPet::save(Cmd::Session::saveType type)
{
	SceneUser * master = (SceneUser *)getMaster();
	if (!master) return;

	Cmd::Session::t_saveCartoon_SceneSession send;
	strncpy(send.userName, master->name, MAX_NAMESIZE);
	send.type = type;
	send.cartoonID = cartoonID;
	send.data = cartoonData;
	switch (type)
	{
		case Cmd::Session::SAVE_TYPE_RETURN:
			{
				send.data.state = Cmd::CARTOON_STATE_WAITING;
				bzero(send.data.adopter, MAX_NAMESIZE);
			}
			break;
		case Cmd::Session::SAVE_TYPE_TIMEOVER:
		case Cmd::Session::SAVE_TYPE_PUTAWAY:
			send.data.state = Cmd::CARTOON_STATE_PUTAWAY;
			break;
		default:
			break;
	}

	if (!isAdopted())
		master->cartoonList[cartoonID] = cartoonData;
	sessionClient->sendCmd(&send, sizeof(send));

	switch (type)
	{
		case Cmd::Session::SAVE_TYPE_RETURN:
		case Cmd::Session::SAVE_TYPE_TIMEOVER:
		case Cmd::Session::SAVE_TYPE_SYN:
			cartoonData.addExp = 0;//清除经验
			break;
		default:
			break;
	}
}

/*
 * \brief 提取经验到主人身上
 * 
 */
void CartoonPet::drawExp()
{
	SceneUser * master = (SceneUser *)getMaster();
	if (!master) return;

	master->addExp(cartoonData.addExp, true);
	cartoonData.addExp = 0;
}

/*
 * \brief 行动，不战斗，只跟随
 * 每15秒释放一次经验
 * 
 */
bool CartoonPet::normalAction()
{
	if (_5_sec(SceneTimeTick::currentTime))
	{
		_5_sec_count++;

		if (isAdopted())
		{
			if (cartoonData.time<=5)
			{
				cartoonData.time = 0;
				bzero(cartoonData.adopter, MAX_NAMESIZE);
				putAway(Cmd::Session::SAVE_TYPE_TIMEOVER);
				return true;
			}
			cartoonData.time -= 5;

			if (0==_5_sec_count%3)//被领养的，每15秒释放经验
				releaseExp();

			if (0==_5_sec_count%60)//5分钟存档
			{
				save(Cmd::Session::SAVE_TYPE_SYN);
			}
		}
		/* //不计算精气值了
		else if (0==_5_sec_count%12)//练级的，每1分钟减sp1
		{
			if (cartoonData.sp)
			{
				cartoonData.sp--;
				if (0==cartoonData.sp)
				{
					putAway(Cmd::Session::SAVE_TYPE_PUTAWAY);
					return true;
				}
				else
				{
					SceneUser * master = (SceneUser *)getMaster();
					master->cartoonList[cartoonID] = cartoonData;
					sendHpExp();
				}
			}
		}
		*/

	}
	return SceneNpc::normalAction();
}

/*
 * \brief 释放经验
 * 
 */
void CartoonPet::releaseExp()
{
	//固定经验=0.15*(2*主人角色当前等级^2)/4 +1  取整
	if (0==cartoonData.time) return;
	DWORD n = (2*cartoonData.masterLevel*cartoonData.masterLevel)*15/100/4+1;
	cartoonData.addExp += n;

#ifdef _XWL_DEBUG
	//Zebra::logger->debug("[宠物]%s 释放经验 %u,现在 %u", name, n, cartoonData.addExp);
#endif
}

/*
 * \brief 发送宝宝数据
 * 
 */
void CartoonPet::sendData()
{
	if (!cartoonID) return;
	SceneUser * master = (SceneUser *)getMaster();
	if (!master) return;

	if (!isAdopted())
	{
		Cmd::stAddCartoonCmd send;
		send.isMine = true;
		send.cartoonID = cartoonID;
		send.data = cartoonData;
		master->sendCmdToMe(&send, sizeof(send));
	}
	else
	{
		Cmd::stAddWaitingCartoonCmd send;
		send.cartoonID = cartoonID;
		send = cartoonData;
		master->sendCmdToMe(&send, sizeof(send));
	}
}

/*
 * \brief 发送宝宝数据
 * 
 */
void CartoonPet::sendHpExp()
{
	if (!cartoonID) return;
	SceneUser * master = (SceneUser *)getMaster();
	if (!master) return;

	if (!isAdopted())
	{
		Cmd::stHpExpCartoonCmd send;
		send.cartoonID = cartoonID;
		send.sp = cartoonData.sp;
		send.exp = cartoonData.exp;
		master->sendCmdToMe(&send, sizeof(send));
	}
	else
	{
	}
}

/*
 * \brief 宝宝加经验
 * \param num 数量
 * 
 * \return 是否升级(未用)
 */
bool CartoonPet::addExp(DWORD num)
{
	if (0==num) num = 1;

	cartoonData.exp += num;
	if (cartoonData.exp>=cartoonData.maxExp)
	{
		levelUp();
		sendData();
	}
	else
		sendHpExp();
	petData.exp = cartoonData.exp;

	return false;
}

/*
 * \brief 宝宝升级
 * 
 */
void CartoonPet::levelUp()
{
	SceneEntryPk * master = getMaster();
	if (!master || master->getType()!=zSceneEntry::SceneEntry_Player) return;

	DWORD maxlv = ((SceneUser *)master)->charbase.level;
	bool notify = false;
	while ((cartoonData.exp>=cartoonData.maxExp)&&(cartoonData.lv<maxlv))
	{
		cartoonData.exp -= cartoonData.maxExp;
		cartoonData.maxSp += 10;
		cartoonData.sp += 10;
		cartoonData.lv++; 
		zExperienceB *base_exp = experiencebm.get(cartoonData.lv);
		if(base_exp)
			cartoonData.maxExp = base_exp->nextexp/10;

		notify = true;
	}
	if (cartoonData.exp>cartoonData.maxExp)
		cartoonData.exp = cartoonData.maxExp;
	petData.lv = cartoonData.lv;
	petData.maxexp = cartoonData.maxExp;

	save(Cmd::Session::SAVE_TYPE_TIMETICK);

	sendMeToNine();

	if (notify)
	{
		if (id==9005)
			Channel::sendNine(this, "主人~我都升到%u级了，该给我找个伴了吧~~", cartoonData.lv);
		else
			Channel::sendNine(this, "主人~我升到%u级啦！", cartoonData.lv);
	}
}

/*
 * \brief 得到宝宝的等级
 * 
 * \return 等级
 */
DWORD CartoonPet::getLevel() const
{
	return cartoonData.lv;
}

/*
 * \brief 得到宝宝的数据
 * 
 * \return 数据
 */
Cmd::t_CartoonData& CartoonPet::getCartoonData()
{
	return cartoonData;
}

/*
 * \brief 设置宝宝的数据
 * 
 * \param data 数据
 *
 */
void CartoonPet::setCartoonData(Cmd::t_CartoonData& data)
{
	//FunctionTimes times(37,__FUNCTION__);
	cartoonData = data;

	strncpy(petData.name, cartoonData.name, MAX_NAMESIZE);
	strncpy(name, cartoonData.name, MAX_NAMESIZE);
	petData.lv = cartoonData.lv;
	petData.exp = cartoonData.exp;
	petData.maxexp = cartoonData.maxExp;
	petData.hp = cartoonData.sp;
	petData.maxhp = cartoonData.maxSp;

	sendData();
	sendMeToNine();
}

/*
 * \brief 主人跨服时，保存其收养的宝宝的临时数据
 * 
 * \param dest 保存地址
 *
 * \return 数据大小
 */
DWORD CartoonPet::save(BYTE * dest)
{
	*((DWORD *)dest) = cartoonID;//ID
	bcopy(&cartoonData, dest+sizeof(DWORD), sizeof(Cmd::t_CartoonData));//数据
	return sizeof(DWORD)+sizeof(Cmd::t_CartoonData);
}

/*
 * \brief 恢复SP
 * 
 * \param num 数量
 *
 */
void CartoonPet::recoverSp(DWORD num)
{
	if (cartoonData.sp==cartoonData.maxSp) return;
	cartoonData.sp += num;
	if (cartoonData.sp>cartoonData.maxSp)
		cartoonData.sp=cartoonData.maxSp;
	sendHpExp();
}

/*
 * \brief 设置宝宝的名字
 * 
 * \param name 名字
 *
 */
void CartoonPet::setName(char * n)
{
	//FunctionTimes times(38,__FUNCTION__);
	strncpy(name, n, MAX_NAMESIZE);
	strncpy(cartoonData.name, n, MAX_NAMESIZE);
	strncpy(petData.name, n, MAX_NAMESIZE);
	sendMeToNine();
}

/*
 * \brief 给收养的宠物增加可提取的经验
 *
 * \param num 数量
 *
 */
void CartoonPet::releaseExp(DWORD num)
{
	if (0==cartoonData.time) return;
	if (!num) num = 1;
	cartoonData.addExp += num;
}

void CartoonPet::delMyself()
{
	Zebra::logger->alarm("[宠物]cartoon %s 因找不到主人而删除", name);
	save(isAdopted()?Cmd::Session::SAVE_TYPE_RETURN:Cmd::Session::SAVE_TYPE_TIMETICK);
	setClearState();
}
