/**
 * \file
 * \version  $Id: Horse.cpp  $
 * \author 
 * \date 	
 * \brief 	马匹相关
 *
 * 
 */

#include "Horse.h"
#include "SceneUser.h"
#include "Chat.h"
#include "TimeTick.h"

/**     
 * \brief 构造函数
 *
 * 初始化相关变量
 *      
 * \param user: 马匹拥有者
 */  
Horse::Horse(SceneUser& user) : _owner(user), _horse(0), _mount(false),_uesHorseone(false)
{
	bzero(&pkData, sizeof(pkData));
	bzero(&data, sizeof(data));
}

/**     
 * \brief 析构函数
 *
 */  
Horse::~Horse()
{

}

/**     
 * \brief 设置马匹的种类
 *
 * \param horse_id: 马匹种类
 * \return 无
 */  
void Horse::horse(DWORD horse_id)
{
	using namespace Cmd;

	if (horseType(horse_id)==HORSE_TYPE_NOTHORSE)
		horse_id = 0;

	_horse = horse_id;

	if (_horse)
	{
		data.id = horse_id;
		zNpcB *b = npcbm.get(data.id);
		if (!b)
		{
			_horse = 0;
			data.id = 0;
			return;
		}

		if (_owner.ridepet)
			_owner.killOnePet(_owner.ridepet);

		data.horseid = horse_id;
		if(b)
		{
			data.lv = b->level;
			interval = b->adistance;
			data.state = Cmd::HORSE_STATE_PUTUP;
			data.callTime = 0;
			summonTime = SceneTimeTick::currentTime;
			bcopy(b->name, data.name, MAX_NAMESIZE-1);

			if (HORSE_TYPE_SUPER!=horseType(horse_id))
			{
				bzero(&data.str, sizeof(DWORD)*5);
				bzero(&data.poisonRes, sizeof(DWORD)*4);
			}
			if (_owner.emperor && HORSE_TYPE_SUPER==horseType(horse_id))
				data.speed += 50;

			sendData();
		}
		else
		{
			Zebra::logger->debug("没有找到id=%d的马",horse_id);
		}
	}
	else
	{
		bzero(&data, sizeof(data));
		bzero(&pkData, sizeof(pkData));

		Cmd::stDelPetPetCmd del;                             
		del.type = Cmd::PET_TYPE_RIDE;                  
		_owner.sendCmdToMe(&del, sizeof(del));
		_owner.clearUState(Cmd::USTATE_RIDE);
	}

	if (_owner.scene)
	{
		_owner.sendMeToNine();
	}
}

/**     
 * \brief 设置马匹的种类
 *
 * \param horse_id: 马匹种类
 * \return 无
 */  
bool Horse::horse(t_Object & obj)
{
	using namespace Cmd;

	if (!horse())
	{   
		Channel::sendSys(&_owner, Cmd::INFO_TYPE_FAIL, "你还没有马，不能使用马凯");
		return false;
	}
    //修复坐骑等级由ob等级控制
	DWORD horse_id = 0, needLevel = 0;
	switch (obj.dwObjectID)
	{
		case 878: //120级
			horse_id = 3201;
			needLevel = 120;
			break;
		case 880: //100级
			horse_id = 3203;
			needLevel = 100;
			break;
		case 883: //130级
			horse_id = 3205;
			needLevel = 130;
			break;
		case 884: //130级
			horse_id = 3206;
			needLevel = 130;
			break;
		case 885: //130级
			horse_id = 3207;
			needLevel = 130;
			break;
		case 886: //130级
			horse_id = 3208;
			needLevel = 130;
			break;
		case 887: //130级
			horse_id = 3209;
			needLevel = 130;
			break;
		case 888: //150级
			horse_id = 3210;
			needLevel = 150;
			break;
		case 3600:
			horse_id = 3233;
			needLevel = 0;
			break;
		case 3601:
			horse_id = 3234;
			needLevel = 0;
			break;
		case 3602:
			horse_id = 3235;
			needLevel = 0;
			break;
		case 3603:
			horse_id = 3237;
			needLevel = 0;
			break;
		case 3604:
			horse_id = 3238;
			needLevel = 0;
			break;
		case 3605:
			horse_id = 3239;
			needLevel = 0;
			break;
		case 3606:
			horse_id = 3242;
			needLevel = 0;
			break;
		case 3607:
			horse_id = 3244;
			needLevel = 0;
			break;
		case 3608:
			horse_id = 3245;
			needLevel = 0;
			break;
		case 3609:
			horse_id = 3246;
			needLevel = 0;
			break;
		case 3610:
			horse_id = 3247;
			needLevel = 0;
			break;
		case 3611:
			horse_id = 3248;
			needLevel = 0;
			break;
		case 3612:
			horse_id = 3250;
			needLevel = 0;
			break;
		case 3613:
			horse_id = 3254;
			needLevel = 0;
			break;
		case 3614:
			horse_id = 3255;
			needLevel = 0;
			break;
		case 3615:
			horse_id = 3256;
			needLevel = 0;
			break;
		case 3616:
			horse_id = 3259;
			needLevel = 0;
			break;
		case 3617:
			horse_id = 3272;
			needLevel = 0;
			break;
		case 3618:
			horse_id = 3276;
			needLevel = 0;
			break;
		case 3619:
			horse_id = 3285;
			needLevel = 0;
			break;
		case 3620:
			horse_id = 3296;
			needLevel = 0;
			break;
		case 3621:
			horse_id = 3297;
			needLevel = 0;
			break;
		case 3622:
			horse_id = 3298;
			needLevel = 0;
			break;
		case 3623:
			horse_id = 3299;
			needLevel = 0;
			break;
		case 3624:
			horse_id = 3300;
			needLevel = 0;
			break;
		case 3625:
			horse_id = 3304;
			needLevel = 0;
			break;
		case 3626:
			horse_id = 3305;
			needLevel = 0;
			break;
		case 3627:
			horse_id = 3307;
			needLevel = 0;
			break;
		case 3628:
			horse_id = 3308;
			needLevel = 0;
			break;
		case 3629:
			horse_id = 3309;
			needLevel = 0;
			break;
		case 3630:
			horse_id = 3310;
			needLevel = 0;
			break;
		case 3631:
			horse_id = 3312;
			needLevel = 0;
			break;
		case 3632:
			horse_id = 3314;
			needLevel = 0;
			break;
		case 3633:
			horse_id = 3317;
			needLevel = 0;
			break;
		case 3634:
			horse_id = 3318;
			needLevel = 0;
			break;
		case 3635:
			horse_id = 3319;
			needLevel = 0;
			break;
		case 3636:
			horse_id = 3320;
			needLevel = 0;
			break;
		case 3637:
			horse_id = 3321;
			needLevel = 0;
			break;
		case 3638:
			horse_id = 3322;
			needLevel = 0;
			break;
		case 3639:
			horse_id = 3323;
			needLevel = 0;
			break;
		case 3640:
			horse_id = 3328;
			needLevel = 0;		
			break;
		case 3641: //160级
			horse_id = 3329;
			needLevel = 160;
			break;
		case 3642: //170级
			horse_id = 3331;
			needLevel = 170;
			break;
		case 3643: //180级
			horse_id = 3332;
			needLevel = 180;
			break;
		case 3644: //190级
			horse_id = 3333;
			needLevel = 190;
			break;
		case 3645:
			horse_id = 3335;
			needLevel = 0;
			break;
		case 3646:
			horse_id = 3338;
			needLevel = 0;
			break;
		case 3647:
			horse_id = 3339;
			needLevel = 0;
			break;
		case 3648:
			horse_id = 3341;
			needLevel = 0;
			break;
		case 3649:
			horse_id = 3342;
			needLevel = 0;
			break;
		case 3650:
			horse_id = 3343;
			needLevel = 0;
			break;
		case 3651:
			horse_id = 3344;
			needLevel = 0;
			break;
		case 3652:
			horse_id = 3345;
			needLevel = 0;
			break;
		case 3653:
			horse_id = 3346;
			needLevel = 0;
			break;
		case 3654:
			horse_id = 3347;
			needLevel = 0;
			break;
		case 3655:
			horse_id = 3348;
			needLevel = 0;
			break;
		case 3656:
			horse_id = 3350;
			needLevel = 0;
			break;
		case 3657:
			horse_id = 3352;
			needLevel = 0;
			break;
		case 3658:
			horse_id = 3353;
			needLevel = 0;
			break;
		case 3659:
			horse_id = 3354;
			needLevel = 0;
			break;
		case 3660:
			horse_id = 3355;
			needLevel = 0;
			break;
		case 3661:
			horse_id = 3356;
			needLevel = 0;
			break;
		case 3662:
			horse_id = 3357;
			needLevel = 0;
			break;
		case 3663:
			horse_id = 3358;
			needLevel = 0;
			break;
		case 3664:
			horse_id = 3360;
			needLevel = 0;
			break;
		case 3665:
			horse_id = 3361;
			needLevel = 0;
			break;
		case 3666:
			horse_id = 3362;
			needLevel = 0;
			break;
		case 3667://天下第一坐骑1
			horse_id = 3392;
			needLevel = 0;
			break;
		case 3668://天下第一坐骑2
			horse_id = 3392;
			needLevel = 0;
			break;
		case 3669://天下第一坐骑3
			horse_id = 3392;
			needLevel = 0;
			break;
		default:
			{
				Zebra::logger->debug("马匹id不对");
				return false;
			}
	}

	if (_owner.charbase.level<needLevel)
	{
		Channel::sendSys(&_owner, Cmd::INFO_TYPE_FAIL, "你的等级不够");
		return false;
	}

	_horse = horse_id;

	if (_horse)
	{
		data.id = horse_id;
		zNpcB *b = npcbm.get(data.id);
		if (!b)
		{
			_horse = 0;
			data.id = 0;
			return false;
		}

		if (_owner.ridepet)
			_owner.killOnePet(_owner.ridepet);

		//data.id = horse_id;
		data.horseid = horse_id;

		data.lv = b->level;
		//data.hp = data.maxhp = b->hp;
		data.speed = obj.mvspeed;
		if (_owner.emperor)
			data.speed += 50;
		//data.hitDown = b->str;
		interval = b->adistance;
		data.state = Cmd::HORSE_STATE_PUTUP;
		data.callTime = 0;
		summonTime = SceneTimeTick::currentTime;
		bcopy(b->name, data.name, MAX_NAMESIZE-1);

		data.str = obj.str;
		data.intel = obj.inte;
		data.dex = obj.dex;
		data.men = obj.spi;
		data.con = obj.con;

		data.pdam = obj.pdam;
		data.pdef = obj.pdef;
		data.mdam = obj.mdam;
		data.mdef = obj.mdef;

		data.maxhp = obj.maxhp;
		data.maxmp = obj.maxmp;

		data.poisonRes = obj.poisondef;
		data.lullRes = obj.lulldef;
		data.faintRes = obj.reeldef;
		data.chaosRes = obj.chaosdef;
		data.freezeRes = obj.colddef;
		data.petrifyRes = obj.petrifydef;
		data.blindRes = obj.blinddef;
		data.slowRes = obj.slowdef;

		sendData();
	}
	else
	{
		bzero(&pkData, sizeof(pkData));
		bzero(&data, sizeof(data));

		Cmd::stDelPetPetCmd del;                             
		//del.id= kill->tempid;                           
		del.type = Cmd::PET_TYPE_RIDE;                  
		_owner.sendCmdToMe(&del, sizeof(del));
		_owner.clearUState(Cmd::USTATE_RIDE);

		return false;
	}

	if (_owner.scene)
	{
		_owner.sendMeToNine();
	}

	return true;
}

/**     
 * \brief 取得马匹的种类
 *
 * \return 马匹种类
 */  
DWORD Horse::horse() const
{
	return _horse;
}

/**     
 * \brief 切换用户的人马合一状态
 *
 * \param flag: 用户要求上马还是下马
 * \param send: 是否需要通知客户端,死亡下马不需要通知客户端,要在重生的时候刷新下去
 * \param speed: 提升移动速度倍率(百分比)
 * \return 无
 */ 
void Horse::uesHorse(bool flag, WORD speed)
{
	if (flag) 
	{
		putAway();//取消跟随
		_owner.charstate.movespeed = 320;
		_uesHorseone = true;
	}else
	{
		_owner.charstate.movespeed = 640;
		_uesHorseone = false;
	}
	getPkData();
	_owner.setupCharBase();
	/* Cmd::stMainUserDataUserCmd  userinfo;
	_owner.full_t_MainUserData(userinfo.data);
	_owner.sendCmdToMe(&userinfo,sizeof(userinfo)); */
	Cmd::stAddUserMapScreenUserCmd cmd;
	_owner.full_t_MapUserData(cmd.data);
	_owner.sendCmdToMe(&cmd,sizeof(cmd));	
}

/**     
 * \brief 切换用户的骑马状态
 *
 * \param flag: 用户要求上马还是下马
 * \param send: 是否需要通知客户端,死亡下马不需要通知客户端,要在重生的时候刷新下去
 * \param speed: 提升移动速度倍率(百分比)
 * \return 无
 */ 
void Horse::mount(bool flag, WORD speed, bool send)
{
	//ride down
	if (!flag && _mount) 
	{
		_owner.clearUState(Cmd::USTATE_RIDE);
	    _owner.charstate.movespeed = 640; // 还原移动速度
		_mount = false;
		data.state = Cmd::HORSE_STATE_PUTUP;
	}

	//ride up
	if (flag && !_mount) 
	{
		if (_owner.charbase.level < 30 ) 
		{
			Channel::sendSys(&_owner, Cmd::INFO_TYPE_FAIL, "你不满 30 级，不能骑马。");
			return;
		}

		putAway();//取消跟随
		_owner.setUState(Cmd::USTATE_RIDE);
		_owner.charstate.movespeed = (WORD)(_owner.charstate.movespeed/(1+speed/100.0f));
		_mount = true;
		/*
		if (checkSummonTime())
		{
			putAway();//取消跟随
			_owner.setUState(Cmd::USTATE_RIDE);
			_mount = true;
			_owner.sendMeToNine();
		}
		else
		{
			Channel::sendSys(&_owner, Cmd::INFO_TYPE_FAIL, "你的马正在休息");
			return;
		}
		*/
	}

	//设置马匹属性
	getPkData();
	_owner.setupCharBase();
	if(send)
	{
		_owner.sendMeToNine();
	}

	Cmd::stMainUserDataUserCmd  userinfo;
	_owner.full_t_MainUserData(userinfo.data);
	_owner.sendCmdToMe(&userinfo,sizeof(userinfo));
	_owner.save(Cmd::Record::OPERATION_WRITEBACK); //立刻存档

}


/**     
 * \brief 返回用户的骑马状态
 *
 * \return 用户是否骑马
 */ 
bool Horse::mount() const
{
	return _mount;
}

/**     
 * \brief 读取用户的马匹信息
 *
 * \param d 二进制档案内容
 * \return 读档的二进制长度
 */ 
int Horse::load(unsigned char* d)
{
	if (0==d) return 0;

	Cmd::t_HorseData * p = (Cmd::t_HorseData *)d;

	bcopy(p, &data, sizeof(data));

	if (0!=data.id)
	{
		_horse = data.id;
		horse(_horse);
			data.state = Cmd::HORSE_STATE_PUTUP;
	}

	//修正错误数据
	if (data.id!=data.horseid)//npcID和道具ID不符
		data.horseid = data.id;
	return sizeof(Cmd::t_HorseData);
}

/**     
 * \brief 存储用户的马匹信息
 *
 * \param d: 二进制档案内容
 * \return 存档的二进制长度
 */ 
int Horse::save(unsigned char *d)
{
	if (0==d) return 0;
	Cmd::t_HorseData * p = (Cmd::t_HorseData *)d;

	//full_HorseDataStruct(p);
	bcopy(&data, p, sizeof(data));
	if (_owner.emperor && HORSE_TYPE_SUPER==horseType())
		p->speed -= 50;

	return sizeof(Cmd::t_HorseData);
}

/**
 * \brief 设置下次召唤马匹的延迟
 *
 *
 * \param delay 延迟时间
 */
void Horse::setSummonTime(int delay)
{
	summonTime = SceneTimeTick::currentTime;
	summonTime.addDelay(delay);
}

/**
 * \brief 设置下次召唤马匹的延迟
 *
 * \return 检查是否可以召唤马匹
 */
bool Horse::checkSummonTime()
{
	return SceneTimeTick::currentTime>=summonTime;
}

/**
 * \brief 召唤出来跟随主人
 * \return 是否成功
 */
bool Horse::comeOut()
{
	if (!(Cmd::HORSE_STATE_FOLLOW==data.state))
	{
		if ((_mount||Cmd::HORSE_STATE_RIDE==data.state))
			mount(false);

		if (_owner.summonPet(data.id, Cmd::PET_TYPE_RIDE))
		{
			data.state = Cmd::HORSE_STATE_FOLLOW;
			return true;
		}
	}
	return false;
}

/**
 * \brief 收起马匹
 *
 * \return 是否成功，没有马匹返回失败
 */
bool Horse::putAway()
{
	if (Cmd::HORSE_STATE_FOLLOW==data.state)
	{
		if (_owner.ridepet)
		{
			_owner.killOnePet(_owner.ridepet);
		}
	}
	return false;
}

/**
 * \brief 检查马是否在跟随状态
 *
 * \return 是否在跟随
 */
bool Horse::isFollowing()
{
	return Cmd::HORSE_STATE_FOLLOW==data.state;
}

/**
 * \brief 填充马的信息
 *
 *
 * \param d 信息结构地址
 */
bool Horse::full_HorseDataStruct(Cmd::t_HorseData *d) //国王皇帝纸娃娃图片
{
	if (!_horse) return false;

		data.callTime = 0;

	bcopy(&data, d, sizeof(data));

	if (_owner.king)  //国王马纸娃娃
	{
	 	//	d->id = KING_HORSE_ID;
	 	//	d->horseid = KING_HORSE_ID;
	}

	if (_owner.emperor) //皇帝马纸娃娃
	{
	 	//	d->id = EMPEROR_HORSE_ID;
	 	//	d->horseid = EMPEROR_HORSE_ID;
	}

	return true;
}

/**
 * \brief 向客户端发送数据
 *
 */
void Horse::sendData()
{
	if (!_horse) return;
	Cmd::stHorseDataPetCmd ret;
	ret.type = Cmd::PET_TYPE_RIDE;
	full_HorseDataStruct(&ret.data);
	ret.id = ret.data.id;
	_owner.sendCmdToMe(&ret, sizeof(ret));
	Zebra::logger->debug("发送马匹信息 name=%s id=%u horseid=%u state=%u time=%u", data.name, data.id, data.horseid, data.state, data.callTime);
}

/**
 * \brief 判断该马是否可以战斗
 *
 * \return 是否可以战斗
 */
bool Horse::canFight()
{
	if (!_horse) return false;

	if (horseType()==HORSE_TYPE_BATTLE ||
					horseType()==HORSE_TYPE_SUPER) return true;

	//zNpcB *b = npcbm.get(_horse);
	if (!npcbm.get(_horse)) return false;
	zObjectB *base = objectbm.get(_horse);
	if (!base) return false;

	if (base->kind == ItemType_BattleHorse && data.lv>=10)
		//if (3200 == _horse && data.lv>=10)
		return true;
	else
		return false;
}

void Horse::getPkData()
{
		if (_horse && HORSE_TYPE_SUPER==horseType() && (mount() || _uesHorseone))
				bcopy(&data, &pkData, sizeof(pkData));
		else
				bzero(&pkData, sizeof(pkData));
}

DWORD Horse::horseType()
{
		if (3000==_horse) return HORSE_TYPE_NORMAL; //普通
		if (3200==_horse) return HORSE_TYPE_BATTLE;  //战马
		if (_horse>=3201 && _horse<=3210) return HORSE_TYPE_SUPER;//soke 战马范围
		if (_horse>=3233 && _horse<=3239) return HORSE_TYPE_SUPER;
		if (_horse>=3241 && _horse<=3248) return HORSE_TYPE_SUPER;
		if (_horse==3250) return HORSE_TYPE_SUPER;
		if (_horse>=3254 && _horse<=3256) return HORSE_TYPE_SUPER;
		if (_horse==3259) return HORSE_TYPE_SUPER;
		if (_horse==3272) return HORSE_TYPE_SUPER;
		if (_horse==3276) return HORSE_TYPE_SUPER;
		if (_horse==3285) return HORSE_TYPE_SUPER;
		if (_horse>=3296 && _horse<=3300) return HORSE_TYPE_SUPER;
		if (_horse==3304) return HORSE_TYPE_SUPER;
		if (_horse==3305) return HORSE_TYPE_SUPER;
		if (_horse>=3307 && _horse<=3310) return HORSE_TYPE_SUPER;
		if (_horse==3312) return HORSE_TYPE_SUPER;
		if (_horse==3314) return HORSE_TYPE_SUPER;
		if (_horse>=3317 && _horse<=3323) return HORSE_TYPE_SUPER;
		if (_horse==3328) return HORSE_TYPE_SUPER;
		if (_horse==3329) return HORSE_TYPE_SUPER;
		if (_horse>=3331 && _horse<=3333) return HORSE_TYPE_SUPER;
		if (_horse==3335) return HORSE_TYPE_SUPER;
		if (_horse==3338) return HORSE_TYPE_SUPER;
		if (_horse==3339) return HORSE_TYPE_SUPER;
		if (_horse>=3341 && _horse<=3348) return HORSE_TYPE_SUPER;
		if (_horse==3350) return HORSE_TYPE_SUPER;
		if (_horse>=3352 && _horse<=3358) return HORSE_TYPE_SUPER;
		if (_horse>=3360 && _horse<=3500) return HORSE_TYPE_SUPER;

		return HORSE_TYPE_NOTHORSE;  //没马
}

DWORD Horse::horseType(DWORD type)
{
		if (3000==type) return HORSE_TYPE_NORMAL;  //普通
		if (3200==type) return HORSE_TYPE_BATTLE;  //战马
		if (type>=3201 && type<=3203) return HORSE_TYPE_SUPER;  //高级战马
		if (type>=3205 && type<=3500) return HORSE_TYPE_SUPER;  //修复可更换外形马铠
		if (_horse>=3233 && _horse<=3239) return HORSE_TYPE_SUPER;
		if (_horse>=3241 && _horse<=3248) return HORSE_TYPE_SUPER;
		if (_horse==3250) return HORSE_TYPE_SUPER;
		if (_horse>=3254 && _horse<=3256) return HORSE_TYPE_SUPER;
		if (_horse==3259) return HORSE_TYPE_SUPER;
		if (_horse==3272) return HORSE_TYPE_SUPER;
		if (_horse==3276) return HORSE_TYPE_SUPER;
		if (_horse==3285) return HORSE_TYPE_SUPER;
		if (_horse>=3296 && _horse<=3300) return HORSE_TYPE_SUPER;
		if (_horse==3304) return HORSE_TYPE_SUPER;
		if (_horse==3305) return HORSE_TYPE_SUPER;
		if (_horse>=3307 && _horse<=3310) return HORSE_TYPE_SUPER;
		if (_horse==3312) return HORSE_TYPE_SUPER;
		if (_horse==3314) return HORSE_TYPE_SUPER;
		if (_horse>=3317 && _horse<=3323) return HORSE_TYPE_SUPER;
		if (_horse==3328) return HORSE_TYPE_SUPER;
		if (_horse==3329) return HORSE_TYPE_SUPER;
		if (_horse>=3331 && _horse<=3333) return HORSE_TYPE_SUPER;
		if (_horse==3335) return HORSE_TYPE_SUPER;
		if (_horse==3338) return HORSE_TYPE_SUPER;
		if (_horse==3339) return HORSE_TYPE_SUPER;
		if (_horse>=3341 && _horse<=3348) return HORSE_TYPE_SUPER;
		if (_horse==3350) return HORSE_TYPE_SUPER;
		if (_horse>=3352 && _horse<=3358) return HORSE_TYPE_SUPER;
		if (_horse>=3360 && _horse<=3500) return HORSE_TYPE_SUPER;

		return HORSE_TYPE_NOTHORSE; //没马
}
