/**
 * \file
 * \version  $Id: Horse.h $
 * \author  
 * \date 	
 * \brief 	马匹相关
 *
 * 
 */
#ifndef __HORSE_H__
#define __HORSE_H__

class SceneUser;

#include "zType.h"
#include "zTime.h"
#include "Command.h"
#include "zObject.h"

//马的类型
const DWORD HORSE_TYPE_NOTHORSE = 0;//没马
const DWORD HORSE_TYPE_NORMAL = 1;//普通
const DWORD HORSE_TYPE_BATTLE = 2;//战马
const DWORD HORSE_TYPE_SUPER = 3;//高级战马

//马的PK数据.骑乘时加到主人身上的
struct t_HorsePkData
{
		DWORD id;       //npcid
		char name[MAX_NAMESIZE];//名字
		DWORD lv;       //等级
		WORD str;       //力量
		WORD intel;     //智力
		WORD dex;       //敏捷
		WORD men;       //精神
		WORD con;       //体质
		WORD speed;     //速度
		BYTE pdam;      //增加物理攻击力%
		BYTE pdef;      //增加物理防御力%
		BYTE mdam;      //增加魔法攻击力%
		BYTE mdef;      //增加魔法防御力%
		SDWORD maxhp;     //最大HP
		WORD maxmp;     //最大MP
		DWORD callTime;     //召唤间隔
		Cmd::horseState state;   //马的状态
		WORD poisonRes;     //抗毒
		WORD lullRes;       //抗麻痹
		WORD faintRes;      //抗晕眩
		WORD chaosRes;      //抗混乱
		WORD freezeRes;     //抗冰冻
		WORD petrifyRes;    //抗石化
		WORD blindRes;      //抗失明
		WORD slowRes;       //抗减速
		DWORD horseid;      //马的id（道具id）
};

/**
 * \brief 马匹类
 *
 * 封装了有关马匹的处理函数
 */
class Horse
{
public:
	Horse(SceneUser& user);	
	
	~Horse();

	void horse(DWORD horse_id);
	bool horse(t_Object &);
	
	DWORD horse() const;

	void mount(bool flag ,WORD speed=0,bool send=true);
	
	bool mount() const;
	
	void uesHorse(bool flag, WORD speed=0);

	int load(unsigned char* );
	
	int save(unsigned char *);

	void setSummonTime(int delay);
	bool checkSummonTime();
	bool isFollowing();
	bool comeOut();
	bool putAway();

	bool full_HorseDataStruct(Cmd::t_HorseData *);

	void sendData();

	bool canFight();

	bool isuesHorseone() const {return _uesHorseone;}

	Cmd::t_HorseData data;
	t_HorsePkData pkData;
	void getPkData();
	DWORD horseType();
	DWORD horseType(DWORD);
private:
	DWORD interval;
	zRTime summonTime;
	SceneUser& _owner;
	//Cmd::horseState _state;
	DWORD _horse;	
	bool _mount;
	//bool _follow;
	bool _uesHorseone;
};

#endif
