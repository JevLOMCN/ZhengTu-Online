/**
 * \file
 * \version  $Id: zDatabase.h  $
 * \author  
 * \date 
 * \brief Zebra游戏中所有基本数据结构的声明定义
 */

#ifndef _ZDATABASE_H_
#define _ZDATABASE_H_

#include <vector>

#include "zEntry.h"
#include "zMisc.h"
#include "zMutex.h"
#include "zString.h"
#include "zNoncopyable.h"
#include "Zebra.h"
#include "Command.h"
#include "SkillBase.h"
#include "Object.h"

#pragma pack(1)
//------------------------------------
// ObjectBase
//------------------------------------
struct ObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	
	DWORD	dwField0;		// 编号
	char	strField1[64];		// 名称
	DWORD	dwField2;		// 最大数量
	DWORD	dwField3;		// 类型
	DWORD	dwField4;		// 需要性别
	DWORD	dwField5;		// 需要等级
	DWORD	dwField6;		// 道具等级
	char	strField7[256];		// 蓝色标识
	char	strField8[256];		// 金色标识
	DWORD	dwField9;		// 配合物品
	char	strField10[256];		// 药品作用
	DWORD	dwField11;		// 最大生命值
	DWORD	dwField12;		// 最大法术值
	DWORD	dwField13;		// 最大体力值
	DWORD	dwField14;		// 最小物攻
	DWORD	dwField15;		// 最大物攻
	DWORD	dwField16;		// 最小魔攻
	DWORD	dwField17;		// 最大魔攻
	DWORD	dwField18;		// 物防
	DWORD	dwField19;		// 魔防
	DWORD	dwField20;		// 伤害加成
	char	strField21[64];		// 孔
	DWORD	dwField22;		// 攻击速度
	DWORD	dwField23;		// 移动速度
	DWORD	dwField24;		// 命中率
	DWORD	dwField25;		// 躲避率
	DWORD	dwField26;		// 改造
	DWORD	dwField27;		// 合成等级
	DWORD	dwField28;		// 打造
	char	strField29[32];		// 需要技能
	char	strField30[1024];		// 需要原料
	DWORD	dwField31;		// 装备位置
	DWORD	dwField32;		// 耐久度
	DWORD	dwField33;		// 价格
	DWORD	dwField34;		// 蓝色装备掉落几率
	DWORD	dwField35;		// 金色装备掉落几率
	DWORD	dwField36;		// 颜色
	DWORD	dwField37;		// 格子宽
	DWORD	dwField38;		// 格子高
	DWORD	dwField39;		// 金子
	DWORD	dwField40;		// 合成单价
	DWORD	dwField41;		// 重击
	DWORD	dwField42;		// 神圣概率
	char	strField43[256];		// 神圣标识	
};//导出 ObjectBase 成功，共 940 条记录

/**
 * \brief 物品基本表
 */
struct zObjectB:public zEntry
{
	DWORD maxnum;				// 最大数量
	WORD kind;					// 类型
	BYTE sex;					// 需要性别
	WORD level;					// 道具等级
	std::vector<DWORD> blues;	//蓝色标识
	std::vector<DWORD> golds;	//金色标识
	WORD needobject;			// 配合物品
	struct leechdom_t 
	{
		BYTE id; //功能标识
		WORD effect; //效果
		WORD time; //时间
		leechdom_t(const std::string& _id="", const std::string& _effect="", const std::string& _time="") 
			: id(atoi(_id.c_str())), effect(atoi(_effect.c_str())), time(atoi(_time.c_str()))
		{ }
	} leechdom ; 				// 药品作用

	WORD needlevel;				// 需要等级

	DWORD maxhp;					// 最大生命值
	DWORD maxmp;					// 最大法术值
	DWORD maxsp;					// 最大体力值

	DWORD pdamage;				// 最小攻击力
	DWORD maxpdamage;			// 最大攻击力
	DWORD mdamage;				// 最小法术攻击力
	DWORD maxmdamage;			// 最大法术攻击力

	DWORD pdefence;				// 物防
	DWORD mdefence;				// 魔防
	WORD damagebonus;			// 伤害加成
	
	WORD akspeed;				// 攻击速度
	WORD mvspeed;				// 移动速度
	WORD atrating;				// 命中率
	WORD akdodge;				// 躲避率

	DWORD color;				// 颜色	
	
	struct socket
	{
		WORD odds;
		BYTE min;
		BYTE max;
		socket(const std::string& odds_, const std::string& number_) : odds(atoi(odds_.c_str())), min(0), max(0)
		{
			std::string::size_type pos = 0;
			if  ( (pos = number_.find("-")) != std::string::npos ) {
				
				min = atoi(number_.substr(0, pos).c_str());
				max = atoi(number_.substr(pos+strlen("-")).c_str());
				//if (odds) Zebra::logger->debug("odds:%d\tmin:%d\tmax:%d", odds, min, max);
			}
		}
	} hole;						//孔
	
	BYTE recast;				// 改造

	BYTE recastlevel; 			// 合成等级
	WORD recastcost;			// 合成单价
	

	WORD make;					// 打造
	struct skills 
	{
		WORD id;
		BYTE level;
		skills(const std::string& id_="0", const std::string& level_="0") : id(atoi(id_.c_str())), level(atoi(level_.c_str()))
		{ }
	};
	skills need_skill;			// 需要技能
	
	struct material
	{
		DWORD gold;
		struct  stuff
		{
			WORD id;
			WORD number;
			BYTE level;
			stuff(const std::string& id_, const std::string& level_, const std::string& number_) : id(atoi(id_.c_str())), number(atoi(number_.c_str())), level(atoi(level_.c_str()))
			{ }	
		};
		std::vector<stuff> stuffs;
		typedef std::vector<stuff>::iterator stuffs_iterator;
	};
	material need_material;		// 需要原料
	
	BYTE setpos;				// 装备位置
	WORD durability;			// 耐久度
	DWORD price;				// 价格
	DWORD bluerating;			// 蓝色装备掉落几率
	DWORD goldrating;			// 金色装备掉落几率
	BYTE width;					// 格子宽
	BYTE height;				// 格子高
	union
	{
	DWORD cardpoint;			// 金子 (已经无用)
	DWORD cointype;				// 货币类型
	};
	WORD bang;					//重击
	DWORD holyrating;			//神圣概率
	std::vector<DWORD> holys;     //神圣标识
	
	void fill(ObjectBase &data)
	{
		id = data.dwField0;
		strncpy(name, data.strField1, MAX_NAMESIZE);

		maxnum = data.dwField2;	
		kind = data.dwField3;	
		sex = data.dwField4;	
		needlevel =	data.dwField5;	
		level =	data.dwField6;	

		init_identifier(blues, data.strField7);
		init_identifier(golds, data.strField8);

		needobject = data.dwField9;	
		init_leechdom(data.strField10);

		maxhp = data.dwField11;	
		maxmp = data.dwField12;
		maxsp =	data.dwField13;

		pdamage = data.dwField14;
		maxpdamage = data.dwField15;
		mdamage = data.dwField16;
		maxmdamage = data.dwField17;
		pdefence = data.dwField18;
		mdefence = data.dwField19;
		damagebonus = data.dwField20;

		init_socket(data.strField21);

		akspeed = data.dwField22;
		mvspeed = data.dwField23;
		atrating = data.dwField24;
		akdodge = data.dwField25;
		
		recast = data.dwField26;
		recastlevel = data.dwField27;

		make = data.dwField28;

		init_need_skills(data.strField29);
		init_need_material(data.strField30);
		
		setpos = data.dwField31;
		durability = data.dwField32;
		price =	data.dwField33;

		bluerating = data.dwField34;
		goldrating = data.dwField35;
		color = data.dwField36;
		width =	data.dwField37;
		height = data.dwField38;
		cardpoint = data.dwField39;
		recastcost = data.dwField40;
		bang = data.dwField41;

		holyrating = data.dwField42;
		init_identifier(holys, data.strField43);
		
	}

	zObjectB():zEntry(), hole("0", "0-0")
	{
		bzero(this,sizeof(zObjectB));
	};
	
	void init_identifier(std::vector<DWORD>& list, const std::string& info)
	{
		list.clear();
		zMisc::getAllNum(info.c_str(), list);
	}

	void init_leechdom(const std::string& info)
	{
		leechdom_t* p = Misc::Parse3<leechdom_t>()(info, ":");
		if (p) {
			leechdom = *p;
			SAFE_DELETE(p);
		}	
	}	

	void init_socket(const std::string& socket_info)
	{
		std::string::size_type pos = socket_info.find(':');
		if (pos != std::string::npos) {
			hole = socket(socket_info.substr(0, pos), socket_info.substr(pos+1));
		}
		
	}
	
	void init_need_skills(const std::string& skills_list)
	{	
		std::string::size_type pos = skills_list.find(':');
		if (pos != std::string::npos) {
			need_skill = skills(skills_list.substr(0, pos), skills_list.substr(pos+1));
		}
	}

	void init_need_material(const std::string& materials)
	{
		need_material.stuffs.clear();
		Misc::Split<Misc::Parse3> p;
		std::string::size_type pos = materials.find(':');
		if (pos != std::string::npos) {
			need_material.gold = atoi(materials.substr(0, pos).c_str());
			p(materials.substr(pos+1), need_material.stuffs, ";", "-");
		}
	}
	
};

//------------------------------------
// ColorObjectBase
//------------------------------------
struct ColorObjectBase{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	
	DWORD	dwField0;		// 编号
	char	strField1[64];		// 名称
	char	strField2[32];		// 连接符
	char	strField3[128];		// 金色品质
	char	strField4[32];		// 力量
	char	strField5[32];		// 智力
	char	strField6[32];		// 敏捷
	char	strField7[32];		// 精神
	char	strField8[32];		// 体质
	char	strField9[32];		// 五行属性
	char	strField10[32];		// 最小物攻
	char	strField11[32];		// 最大物攻
	char	strField12[32];		// 最小魔攻
	char	strField13[32];		// 最大魔攻
	char	strField14[32];		// 物防
	char	strField15[32];		// 魔防
	char	strField16[32];		// 最大生命值
	char	strField17[32];		// 最大法术值
	char	strField18[32];		// 最大体力值
	char	strField19[32];		// 移动速度
	char	strField20[32];		// 生命值恢复
	char	strField21[32];		// 法术值恢复
	char	strField22[32];		// 体力值恢复
	char	strField23[32];		// 攻击速度
	char	strField24[32];		// 增加物理攻击力
	char	strField25[32];		// 增加物理防御力
	char	strField26[32];		// 增加魔法攻击力
	char	strField27[32];		// 增加魔法防御力
	char	strField28[32];		// 命中率
	char	strField29[32];		// 闪避率
	char	strField30[32];		// 抗毒增加
	char	strField31[32];		// 抗麻痹增加
	char	strField32[32];		// 抗眩晕增加
	char	strField33[32];		// 抗噬魔增加
	char	strField34[32];		// 抗噬力增加
	char	strField35[32];		// 抗混乱增加
	char	strField36[32];		// 抗冰冻增加
	char	strField37[32];		// 抗石化增加
	char	strField38[32];		// 抗失明增加
	char	strField39[32];		// 抗定身增加
	char	strField40[32];		// 抗减速增加
	char	strField41[32];		// 抗诱惑增加
	char	strField42[32];		// 恢复耐久度
	char	strField43[32];		// 重击
	DWORD	dwField44;		// 神圣装备几率
	char	strField45[1024];		// 技能加成
	char	strField46[32];		// 全系技能加成
};

//一个范围值得描述
struct rangeValue
{
	WORD min;
	WORD max;
};

struct luckRangeValue
{
	WORD per;	//是否产生本属性的几率
	rangeValue data;	//产生属性值的随机范围
	WORD sleightValue;	//根据熟练度产生的加权值
};

static void fillRangeValue(const char *str,rangeValue &data)
{
	std::vector<DWORD> num;
	int i =zMisc::getAllNum(str,num);
	if(i!=2)
	{
		data.min=0;
		data.max=0;
	}
	else
	{
		data.min=num[0];
		data.max=num[1];
	}
}

static void fillLuckRangeValue(char *str,luckRangeValue &data)
{
	std::vector<DWORD> num;
	int i =zMisc::getAllNum(str,num);
	if(i<3)
	{
		if(i!=1)
		{
			//Zebra::logger->debug("%s,%s", __PRETTY_FUNCTION__,str);
		}
		data.per=0;
		data.data.min=0;
		data.data.max=0;
		data.sleightValue=0;
	}
	else
	{
		data.per=num[0];
		data.data.min=num[1];
		data.data.max=num[2];
		if(i==4)
			data.sleightValue=num[3];
		else
			data.sleightValue=0;
	}
}

struct skillbonus {
	WORD odds; //几率
	WORD id; //技能 id
	WORD level; // 技能等级
	skillbonus(std::string _odds="0", std::string _id="0", std::string _level="0") : odds(atoi(_odds.c_str())), id(atoi(_id.c_str())), level(atoi(_level.c_str()))
	{ }
}; 

template <class Base, WORD tt>
struct zColorObjectB:public zEntry
{
	//std::string prefix;			// 名称
	char prefix[MAX_NAMESIZE];			// 名称
	//std::string joint;			// 连接符
	char joint[MAX_NAMESIZE];			// 连接符
	std::vector<WORD> golds;	// 金色品质

	union {
		luckRangeValue _p1[5];
		struct {
			luckRangeValue str;			// 力量
			luckRangeValue inte;		// 智力
			luckRangeValue dex;			// 敏捷
			luckRangeValue spi;			// 精神
			luckRangeValue con;			// 体质
		};	
	};
	luckRangeValue five;		// 五行属性
	
	rangeValue pdamage;			// 最小物攻
	rangeValue maxpdamage;		// 最大物攻
	rangeValue mdamage;			// 最小魔攻
	rangeValue maxmdamage;		// 最大魔攻
	rangeValue pdefence;			// 物防
	rangeValue mdefence;			// 魔防
	
	luckRangeValue maxhp;		// 最大生命值
	luckRangeValue maxmp;		// 最大法术值
	luckRangeValue maxsp;		// 最大体力值
	
	luckRangeValue mvspeed;		// 移动速度
	luckRangeValue hpr;			// 生命值恢复
	luckRangeValue mpr;			// 法术值恢复
	luckRangeValue spr;			// 体力值恢复
	luckRangeValue akspeed;		// 攻击速度

	union {
		luckRangeValue _p2[18];
		struct {
			luckRangeValue pdam;		// 增加物理攻击力
			luckRangeValue pdef;		// 增加物理防御力
			luckRangeValue mdam;		// 增加魔法攻击力
			luckRangeValue mdef;		// 增加魔法防御力
			
			luckRangeValue poisondef;	// 抗毒增加
			luckRangeValue lulldef;		// 抗麻痹增加
			luckRangeValue reeldef;		// 抗眩晕增加
			luckRangeValue evildef;		// 抗噬魔增加
			luckRangeValue bitedef;		// 抗噬力增加
			luckRangeValue chaosdef;	// 抗混乱增加
			luckRangeValue colddef;		// 抗冰冻增加
			luckRangeValue petrifydef;		// 抗石化增加
			luckRangeValue blinddef;		// 抗失明增加
			luckRangeValue stabledef;		// 抗定身增加
			luckRangeValue slowdef;		// 抗减速增加
			luckRangeValue luredef;		// 抗诱惑增加

			luckRangeValue atrating;		// 命中率
			luckRangeValue akdodge;		// 闪避率

		};
	};	
	
	luckRangeValue resumedur;		// 恢复耐久度
	luckRangeValue bang;		// 重击
	WORD holyrating;	//神圣装备几率

	std::vector<skillbonus> skill;	// 技能加成
	skillbonus skills;				// 全系技能加成
	
	WORD type;

	public:
	void fill(Base &data)
	{
		id = data.dwField0;
		strncpy(name, data.strField1, MAX_NAMESIZE);

		//prefix =  data.strField1;
		//joint = data.strField2;
		strncpy(prefix, data.strField1, MAX_NAMESIZE);
		strncpy(joint, data.strField2, MAX_NAMESIZE);
		zMisc::getAllNum(data.strField3, golds);
		fillLuckRangeValue(data.strField4, str);
		fillLuckRangeValue(data.strField5, inte);
		fillLuckRangeValue(data.strField6, dex);
		fillLuckRangeValue(data.strField7, spi);
		fillLuckRangeValue(data.strField8, con);
		
		fillLuckRangeValue(data.strField9, five);
		
		fillRangeValue(data.strField10, pdamage);
		fillRangeValue(data.strField11, maxpdamage);
		fillRangeValue(data.strField12, mdamage);
		fillRangeValue(data.strField13, maxmdamage);
		fillRangeValue(data.strField14, pdefence);
		fillRangeValue(data.strField15, mdefence);
		
		fillLuckRangeValue(data.strField16, maxhp);
		fillLuckRangeValue(data.strField17, maxmp);
		fillLuckRangeValue(data.strField18, maxsp);
		fillLuckRangeValue(data.strField19, mvspeed);
		fillLuckRangeValue(data.strField20, hpr);
		fillLuckRangeValue(data.strField21, mpr);
		fillLuckRangeValue(data.strField22, spr);
		fillLuckRangeValue(data.strField23, akspeed);
		fillLuckRangeValue(data.strField24, pdam);
		fillLuckRangeValue(data.strField25, pdef);
		fillLuckRangeValue(data.strField26, mdam);
		fillLuckRangeValue(data.strField27, mdef);
		fillLuckRangeValue(data.strField28, atrating);
		fillLuckRangeValue(data.strField29, akdodge);

		fillLuckRangeValue(data.strField30, poisondef);
		fillLuckRangeValue(data.strField31, lulldef);
		fillLuckRangeValue(data.strField32, reeldef);
		fillLuckRangeValue(data.strField33, evildef);
		fillLuckRangeValue(data.strField34, bitedef);
		fillLuckRangeValue(data.strField35, chaosdef);
		fillLuckRangeValue(data.strField36, colddef);
		fillLuckRangeValue(data.strField37, petrifydef);
		fillLuckRangeValue(data.strField38, blinddef);
		fillLuckRangeValue(data.strField39, stabledef);
		fillLuckRangeValue(data.strField40, slowdef);
		fillLuckRangeValue(data.strField41, luredef);
		fillLuckRangeValue(data.strField42, resumedur);
		//bang = data.dwField43;
		fillLuckRangeValue(data.strField43, bang);
		holyrating = data.dwField44;
		
		init_skill(data.strField45);
		init_skills(data.strField46);

		//Zebra::logger->debug("id:%d, name:%s", id, name);
		#if 0
		//恢复耐久度格式单独处理
		{
			std::vector<DWORD> num;
			int i =zMisc::getAllNum(data.strField47,num);
			if(i!=7)
			{
				bzero(&durpoint,sizeof(durpoint));
				bzero(&dursecond,sizeof(dursecond));
			}
			else
			{
				durpoint.per=num[0];
				durpoint.data.min=num[1];
				durpoint.data.max=num[2];
				durpoint.sleightValue=num[3];
				dursecond.per=0;
				dursecond.data.min=num[4];
				dursecond.data.max=num[5];
				dursecond.sleightValue=num[6];
			}
		}
		#endif

	}
	
	zColorObjectB():zEntry()
	{
		bzero(this,sizeof(zColorObjectB));
		type=tt;
	};

	void init_skill(const std::string& info)
	{
		skill.clear();
		Misc::Split<Misc::Parse3> p;
		p(info, skill, ";", ":");
	}

	void init_skills(const std::string& info)
	{
		skillbonus* p = Misc::Parse3<skillbonus>()(info, ":");
		if (p) {
			skills = *p;
			SAFE_DELETE(p);
		}	
		else if(strcmp(info.c_str(),"0")!=0)
		{       
			Zebra::logger->debug("%s,(%d),%s", __PRETTY_FUNCTION__,id,info.c_str());
		}     
	}	
	
};

typedef ColorObjectBase ZhuanGoldObjectBase;
typedef ColorObjectBase GoldObjectBase;
typedef ColorObjectBase DropGoldObjectBase;
typedef ColorObjectBase BlueObjectBase;
typedef zColorObjectB<BlueObjectBase, 1> zBlueObjectB;
typedef zColorObjectB<GoldObjectBase, 2> zGoldObjectB;
typedef zColorObjectB<DropGoldObjectBase, 3> zDropGoldObjectB;
typedef zColorObjectB<ZhuanGoldObjectBase, 4> zZhuanGoldObjectB;

//------------------------------------
// SetObjectBase
//------------------------------------
struct SetObjectBase{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	char	strField1[32];		// 名称
	char	strField2[64];		// 套装5
	char	strField3[32];		// 套装4
	char	strField4[32];		// 套装3
	char	strField5[32];		// 套装2
	char	strField6[32];		// 套装1
	DWORD	dwField7;		// 属性标识
};//导出 SetObjectBase 成功，共 532 条记录

struct zSetObjectB:public zEntry
{
	struct SET
	{
		WORD odds;
		std::vector<WORD> ids;
	};
	
	typedef std::vector<SET> SETS;
	typedef SETS::iterator iterator;
	SETS sets;
	DWORD mark;
	
	void fill(SetObjectBase& data)
	{
		id = data.dwField0;
		strncpy(name, data.strField1, MAX_NAMESIZE);
		init_set(data.strField2);
		init_set(data.strField3);
		init_set(data.strField4);
		init_set(data.strField5);
		init_set(data.strField6);
		mark = data.dwField7;
	}
	
	zSetObjectB():zEntry()
	{
		bzero(this, sizeof(zSetObjectB));
	};
	
	void init_set(const std::string& info)
	{
	    ///////////////////////////////////////////////////
		//soke 灵魂锁链  如果是第二灵魂 这地方不应该清除  否则sets里面就一个数据 本来应该5个
		//sets.clear();
		///////////////////////////////////////////////////
		std::string::size_type pos = info.find(':');
		SET set;
		if (pos != std::string::npos) 
		{
			set.odds = atoi(info.substr(0, pos).c_str());
			zMisc::getAllNum(info.substr(pos+1).c_str(), set.ids);
		}
		sets.push_back(set);
	}
	
};

//------------------------------------
// FiveSetBase
//------------------------------------
struct FiveSetBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 属性标识
	char	strField1[16];		// 物理伤害减少x%
	char	strField2[16];		// 法术伤害减少x%
	char	strField3[16];		// 增加伤害值x%
	char	strField4[16];		// 伤害反射x%
	char	strField5[16];		// x%忽视目标防御
};//导出 FiveSetBase 成功，共 5 条记录


struct zFiveSetB:public zEntry
{
	rangeValue dpdam;       //物理伤害减少%x
	rangeValue dmdam;       //法术伤害减少%x
	rangeValue bdam;        //增加伤害x%
	rangeValue rdam;        //伤害反射%x
	rangeValue ignoredef;   //%x忽视目标防御
	rangeValue addpdef;     //物理防御力增强x
	rangeValue addmdef;     //魔法防御力增强x
	rangeValue addpdam;     //增加物理攻击力x
	rangeValue addmdam;     //增加魔法攻击力x
	rangeValue addhp;       //增加生命值x

	void fill(FiveSetBase& data)
	{
		id = data.dwField0;
		fillRangeValue(data.strField1, dpdam);
		fillRangeValue(data.strField2, dmdam);
		fillRangeValue(data.strField3, bdam);		
		fillRangeValue(data.strField4, rdam);		
		fillRangeValue(data.strField5, ignoredef);	

		//soke 第二灵魂配置
		addpdef.min = 60;  //物理防御力增强
		addpdef.max = 60;

		addmdef.min = 60;  //魔法防御力增强
		addmdef.max = 60;

		addpdam.min = 120; //增加物理攻击力
		addpdam.max = 120;	

		addmdam.min = 120; //增加魔法攻击力
		addmdam.max = 120;	

		addhp.min = 100;   //增加生命值
		addhp.max = 100;
	}
	
	zFiveSetB():zEntry()
	{
		bzero(this, sizeof(zFiveSetB));
	};	
};

//------------------------------------
// HolyObjectBase
//------------------------------------
struct HolyObjectBase{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	char	strField1[64];		// 名称
	char	strField2[16];		// 神圣一击
	char	strField3[16];		// 增加伤害值x％
	char	strField4[16];		// 五行属性增加
	char	strField5[16];		// 生命值恢复
	char	strField6[16];		// 法术值恢复
	char	strField7[16];		// 体力值恢复
	char	strField8[16];		// 攻击速度
	char	strField9[16];		// 移动速度
	char	strField10[16];		// 命中率
	char	strField11[16];		// 闪避率
	char	strField12[16];		// 技能加成
	char	strField13[16];		// 全系技能加成
	char	strField14[16];		// 双倍经验
	char	strField15[16];		// 增加掉宝率
};//导出 HolyObjectBase 成功，共 705 条记录

struct zHolyObjectB:public zEntry
{
	WORD	holy;				// 神圣一击
	luckRangeValue	damage;		// 增加伤害值x％
	luckRangeValue	fivepoint;		// 五行属性增加

	luckRangeValue hpr;			// 生命值恢复
	luckRangeValue mpr;			// 法术值恢复
	luckRangeValue spr;			// 体力值恢复

	luckRangeValue akspeed;		// 攻击速度
	luckRangeValue mvspeed;		// 移动速度
	
	luckRangeValue atrating;		// 命中率
	luckRangeValue akdodge;			// 闪避率

	std::vector<skillbonus> skill;	// 技能加成
	skillbonus skills;				// 全系技能加成

	luckRangeValue doublexp;		//%x双倍经验
	luckRangeValue mf; 			//掉宝率
	
	void fill(HolyObjectBase &data)
	{
		id = data.dwField0;
		strncpy(name, data.strField1, MAX_NAMESIZE);
		holy = atoi(data.strField2);

		fillLuckRangeValue(data.strField3, damage);		
		fillLuckRangeValue(data.strField4, fivepoint);
		fillLuckRangeValue(data.strField5, hpr);
		fillLuckRangeValue(data.strField6, mpr);
		fillLuckRangeValue(data.strField7, spr);
		fillLuckRangeValue(data.strField8, akspeed);
		fillLuckRangeValue(data.strField9, mvspeed);
		fillLuckRangeValue(data.strField10, atrating);
		fillLuckRangeValue(data.strField11, akdodge);
		
		init_skill(data.strField12);
		init_skills(data.strField13);

		fillLuckRangeValue(data.strField14, doublexp);
		fillLuckRangeValue(data.strField15, mf);

	}
	
	zHolyObjectB():zEntry()
	{
		bzero(this,sizeof(zHolyObjectB));
	};

	void init_skill(const std::string& info)
	{
		skill.clear();
		Misc::Split<Misc::Parse3> p;
		p(info, skill, ";", ":");
	}

	void init_skills(const std::string& info)
	{
		skillbonus* p = Misc::Parse3<skillbonus>()(info, ":");
		if (p) {
			skills = *p;
			SAFE_DELETE(p);
		}
	}	
};

//------------------------------------
// UpgradeObjectBase - 装备升星
//------------------------------------
struct UpgradeObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];		// 名称
	DWORD	dwField3;		// 类型
	DWORD	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 UpgradeObjectBase 成功，共 6345 条记录

struct zUpgradeObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		    // 类型
	
	DWORD stuff;		// 升级原料
	
	DWORD gold;		// 需要银子
	
	WORD odds;		    // 对应成功率
	
	DWORD pdamage;				// 最小攻击力
	DWORD maxpdamage;			// 最大攻击力
	DWORD mdamage;				// 最小法术攻击力
	DWORD maxmdamage;			// 最大法术攻击力

	DWORD pdefence;			    // 物防
	DWORD mdefence;			    // 魔防
	DWORD maxhp;				// 最大生命值
	
	void fill(UpgradeObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;
		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;
		
		pdamage = data.dwField7;
		maxpdamage = data.dwField8;
		mdamage = data.dwField9;
		maxmdamage = data.dwField10;
		
		pdefence = data.dwField11;
		mdefence = data.dwField12;
		
		maxhp = data.dwField13;
	}

	zUpgradeObjectB():zEntry()
	{
		bzero(this,sizeof(zUpgradeObjectB));
	}
};

//------------------------------------
// UpgradeObject2Base - 绿装等升星
//------------------------------------
//soke 白-绿装备升星使用，不然绿装祝福紫装后在改造属性 绿装的升星加成就消失了
struct UpgradeObject2Base
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];		// 名称
	DWORD	dwField3;		// 类型
	DWORD	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 UpgradeObject2Base 成功，共 6345 条记录

struct zUpgradeObject2B:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		    // 类型
	
	DWORD stuff;		// 升级原料
	
	DWORD gold;		// 需要银子
	
	WORD odds;		// 对应成功率
	
	DWORD p1damage;				// 最小攻击力
	DWORD m1axpdamage;			// 最大攻击力
	DWORD m1damage;				// 最小法术攻击力
	DWORD m1axmdamage;			// 最大法术攻击力

	DWORD p1defence;			    // 物防
	DWORD m1defence;			    // 魔防
	DWORD m1axhp;				// 最大生命值
	
	void fill(UpgradeObject2Base  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;
		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;
		
		p1damage = data.dwField7;
		m1axpdamage = data.dwField8;
		m1damage = data.dwField9;
		m1axmdamage = data.dwField10;
		
		p1defence = data.dwField11;
		m1defence = data.dwField12;
		
		m1axhp = data.dwField13;
	}

	zUpgradeObject2B():zEntry()
	{
		bzero(this,sizeof(zUpgradeObject2B));
	}
};

//------------------------------------
// UpgradePurpleBase - 紫色装备升星
//------------------------------------
struct UpgradePurpleBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];		// 名称
	DWORD	dwField3;		// 类型
	DWORD	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 UpgradePurpleBase 成功，共 6345 条记录

struct zUpgradePurpleB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		    // 类型
	
	DWORD stuff;		// 升级原料
	
	DWORD gold;		// 需要银子
	
	WORD odds;		    // 对应成功率
	
	DWORD p2damage;				// 最小攻击力
	DWORD m2axpdamage;			// 最大攻击力
	DWORD m2damage;				// 最小法术攻击力
	DWORD m2axmdamage;			// 最大法术攻击力

	DWORD p2defence;			// 物防
	DWORD m2defence;			// 魔防
	DWORD m2axhp;				// 最大生命值
	
	void fill(UpgradePurpleBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;
		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;
		
		p2damage = data.dwField7;
		m2axpdamage = data.dwField8;
		m2damage = data.dwField9;
		m2axmdamage = data.dwField10;
		
		p2defence = data.dwField11;
		m2defence = data.dwField12;
		
		m2axhp = data.dwField13;
	}

	zUpgradePurpleB():zEntry()
	{
		bzero(this,sizeof(zUpgradePurpleB));
	}
};

//------------------------------------
// AmuletObjectBase - 护身符升级
//------------------------------------
struct AmuletObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 AmuletObjectBase 成功，共 6345 条记录

struct zAmuletObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		    // 类型
	
	DWORD stuff;	// 升级原料
	
	DWORD gold;		// 需要银子
	
	WORD odds;		    // 对应成功率
	
	DWORD pdamage;				// 最小攻击力
	DWORD maxpdamage;			// 最大攻击力
	DWORD mdamage;				// 最小法术攻击力
	DWORD maxmdamage;			// 最大法术攻击力

	DWORD pdefence;			    // 物防
	DWORD mdefence;			    // 魔防
	DWORD maxhp;				// 最大生命值
	
	void fill(AmuletObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;
		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;
		
		pdamage = data.dwField7;
		maxpdamage = data.dwField8;
		mdamage = data.dwField9;
		maxmdamage = data.dwField10;
		
		pdefence = data.dwField11;
		mdefence = data.dwField12;
		
		maxhp = data.dwField13;
	}

	zAmuletObjectB():zEntry()
	{
		bzero(this,sizeof(zAmuletObjectB));
	}
};

//------------------------------------
// UdouhunObjectBase-斗魂升级
//------------------------------------
struct UdouhunObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 UdouhunObjectBase 成功，共 6345 条记录


struct zUdouhunObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型
	
	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子
	
	WORD odds;		    // 对应成功率
	
	DWORD p3damage;				// 最小攻击力
	DWORD m3axpdamage;			// 最大攻击力
	DWORD m3damage;				// 最小法术攻击力
	DWORD m3axmdamage;			// 最大法术攻击力

	DWORD p3defence;			// 物防
	DWORD m3defence;			// 魔防
	DWORD m3axhp;				// 最大生命值
	
	void fill(UdouhunObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;
		
		p3damage = data.dwField7;
		m3axpdamage = data.dwField8;
		m3damage = data.dwField9;
		m3axmdamage = data.dwField10;
		
		p3defence = data.dwField11;
		m3defence = data.dwField12;
		
		m3axhp = data.dwField13;
	}

	zUdouhunObjectB():zEntry()
	{
		bzero(this,sizeof(zUdouhunObjectB));
	}
};

//------------------------------------
// UbutianObjectBase-补天升级
//------------------------------------
struct UbutianObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 UbutianObjectBase 成功，共 6345 条记录


struct zUbutianObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型
	
	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子
	
	WORD odds;		    // 对应成功率
	
	DWORD p6damage;				// 最小攻击力
	DWORD m6axpdamage;			// 最大攻击力
	DWORD m6damage;				// 最小法术攻击力
	DWORD m6axmdamage;			// 最大法术攻击力

	DWORD p6defence;			// 物防
	DWORD m6defence;			// 魔防
	DWORD m6axhp;				// 最大生命值
	
	void fill(UbutianObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;
		
		p6damage = data.dwField7;
		m6axpdamage = data.dwField8;
		m6damage = data.dwField9;
		m6axmdamage = data.dwField10;
		
		p6defence = data.dwField11;
		m6defence = data.dwField12;
		
		m6axhp = data.dwField13;
	}

	zUbutianObjectB():zEntry()
	{
		bzero(this,sizeof(zUbutianObjectB));
	}
};

//------------------------------------
// UdtroughObjectBase-龙星激活/解封
//------------------------------------
struct UdtroughObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 UdtroughObjectBase 成功，共 6345 条记录


struct zUdtroughObjectB :public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		        // 类型

	DWORD stuff;		    // 升级原料
	DWORD gold;		        // 需要银子

	WORD odds;		        // 对应成功率

	DWORD p4damage;		    // 最小攻击力
	DWORD m4axpdamage;		// 最大攻击力
	DWORD m4damage;			// 最小法术攻击力
	DWORD m4axmdamage;		// 最大法术攻击力

	DWORD p4defence;		// 物防
	DWORD m4defence;		// 魔防
	DWORD m4axhp;			// 最大生命值

	void fill(UdtroughObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p4damage = data.dwField7;
		m4axpdamage = data.dwField8;
		m4damage = data.dwField9;
		m4axmdamage = data.dwField10;

		p4defence = data.dwField11;
		m4defence = data.dwField12;

		m4axhp = data.dwField13;
	}

	zUdtroughObjectB() :zEntry()
	{
		bzero(this, sizeof(zUdtroughObjectB));
	}
};

//------------------------------------
// UdrastarObjectBase-龙星升级
//------------------------------------
struct UdrastarObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 UdrastarObjectBase 成功，共 6345 条记录


struct zUdrastarObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		        // 类型
	
	DWORD stuff;		    // 升级原料
	DWORD gold;		        // 需要银子
	
	WORD odds;		        // 对应成功率
	
	DWORD p5damage;		    // 最小攻击力
	DWORD m5axpdamage;		// 最大攻击力
	DWORD m5damage;			// 最小法术攻击力
	DWORD m5axmdamage;		// 最大法术攻击力

	DWORD p5defence;		// 物防
	DWORD m5defence;		// 魔防
	DWORD m5axhp;			// 最大生命值
	
	void fill(UdrastarObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;
		
		p5damage = data.dwField7;
		m5axpdamage = data.dwField8;
		m5damage = data.dwField9;
		m5axmdamage = data.dwField10;
		
		p5defence = data.dwField11;
		m5defence = data.dwField12;
		
		m5axhp = data.dwField13;
	}

	zUdrastarObjectB():zEntry()
	{
		bzero(this,sizeof(zUdrastarObjectB));
	}
};

//------------------------------------
// MedalObjectBasee-勋章升级
//------------------------------------

struct MedaltObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 MedaltObjectBase 成功，共 6345 条记录

struct zMedaltObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		    // 类型

	DWORD stuff;	// 升级原料

	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p7damage;				// 最小攻击力
	DWORD m7axpdamage;			// 最大攻击力
	DWORD m7damage;				// 最小法术攻击力
	DWORD m7axmdamage;			// 最大法术攻击力

	DWORD p7defence;			    // 物防
	DWORD m7defence;			    // 魔防
	DWORD m7axhp;				// 最大生命值

	void fill(MedaltObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;
		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p7damage = data.dwField7;
		m7axpdamage = data.dwField8;
		m7damage = data.dwField9;
		m7axmdamage = data.dwField10;

		p7defence = data.dwField11;
		m7defence = data.dwField12;

		m7axhp = data.dwField13;
	}

	zMedaltObjectB():zEntry()
	{
		bzero(this,sizeof(zMedaltObjectB));
	}
};


//------------------------------------
// HbqlObjectBase-寒冰升级
//------------------------------------
struct UhbqlObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 HbqlObjectBase 成功，共 6345 条记录


struct zUhbqlObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p8damage;				// 最小攻击力
	DWORD m8axpdamage;			// 最大攻击力
	DWORD m8damage;				// 最小法术攻击力
	DWORD m8axmdamage;			// 最大法术攻击力

	DWORD p8defence;			// 物防
	DWORD m8defence;			// 魔防
	DWORD m8axhp;				// 最大生命值

	void fill(UhbqlObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p8damage = data.dwField7;
		m8axpdamage = data.dwField8;
		m8damage = data.dwField9;
		m8axmdamage = data.dwField10;

		p8defence = data.dwField11;
		m8defence = data.dwField12;

		m8axhp = data.dwField13;
	}

	zUhbqlObjectB():zEntry()
	{
		bzero(this,sizeof(zUhbqlObjectB));
	}
};

//------------------------------------
// ShenqiObjectBase-神器升级
//------------------------------------
struct UshenqiObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 ShenqiObjectBase 成功，共 6345 条记录


struct zUshenqiObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p9damage;				// 最小攻击力
	DWORD m9axpdamage;			// 最大攻击力
	DWORD m9damage;				// 最小法术攻击力
	DWORD m9axmdamage;			// 最大法术攻击力

	DWORD p9defence;			// 物防
	DWORD m9defence;			// 魔防
	DWORD m9axhp;				// 最大生命值

	void fill(UshenqiObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p9damage = data.dwField7;
		m9axpdamage = data.dwField8;
		m9damage = data.dwField9;
		m9axmdamage = data.dwField10;

		p9defence = data.dwField11;
		m9defence = data.dwField12;

		m9axhp = data.dwField13;
	}

	zUshenqiObjectB():zEntry()
	{
		bzero(this,sizeof(zUshenqiObjectB));
	}
};

//------------------------------------
// FabaoObjectBase-十二星宿升级
//------------------------------------
struct UfabaoObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 fabaoObjectBase 成功，共 6345 条记录


struct zUfabaoObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p20damage;				// 最小攻击力
	DWORD m20axpdamage;			// 最大攻击力
	DWORD m20damage;				// 最小法术攻击力
	DWORD m20axmdamage;			// 最大法术攻击力

	DWORD p20defence;			// 物防
	DWORD m20defence;			// 魔防
	DWORD m20axhp;				// 最大生命值

	void fill(UfabaoObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p20damage = data.dwField7;
		m20axpdamage = data.dwField8;
		m20damage = data.dwField9;
		m20axmdamage = data.dwField10;

		p20defence = data.dwField11;
		m20defence = data.dwField12;

		m20axhp = data.dwField13;
	}

	zUfabaoObjectB():zEntry()
	{
		bzero(this,sizeof(zUfabaoObjectB));
	}
};


//------------------------------------
// YUANSHENObjectBase-元神升级
//------------------------------------
struct UYUANSHENObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 YUANSHENObjectBase 成功，共 6345 条记录



struct zUYUANSHENObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p22damage;				// 最小攻击力
	DWORD m22axpdamage;			// 最大攻击力
	DWORD m22damage;				// 最小法术攻击力
	DWORD m22axmdamage;			// 最大法术攻击力

	DWORD p22defence;			// 物防
	DWORD m22defence;			// 魔防
	DWORD m22axhp;				// 最大生命值

	void fill(UYUANSHENObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p22damage = data.dwField7;
		m22axpdamage = data.dwField8;
		m22damage = data.dwField9;
		m22axmdamage = data.dwField10;

		p22defence = data.dwField11;
		m22defence = data.dwField12;

		m22axhp = data.dwField13;
	}

	zUYUANSHENObjectB():zEntry()
	{
		bzero(this,sizeof(zUYUANSHENObjectB));
	}
};


//------------------------------------
// YSBSObjectBase-元神宝石升级
//------------------------------------
struct UYSBSObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 YSBSObjectBase 成功，共 6345 条记录



struct zUYSBSObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p23damage;				// 最小攻击力
	DWORD m23axpdamage;			// 最大攻击力
	DWORD m23damage;				// 最小法术攻击力
	DWORD m23axmdamage;			// 最大法术攻击力

	DWORD p23defence;			// 物防
	DWORD m23defence;			// 魔防
	DWORD m23axhp;				// 最大生命值

	void fill(UYSBSObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p23damage = data.dwField7;
		m23axpdamage = data.dwField8;
		m23damage = data.dwField9;
		m23axmdamage = data.dwField10;

		p23defence = data.dwField11;
		m23defence = data.dwField12;

		m23axhp = data.dwField13;
	}

	zUYSBSObjectB():zEntry()
	{
		bzero(this,sizeof(zUYSBSObjectB));
	}
};


//------------------------------------
// SEXXObjectBase-法宝升级
//------------------------------------
struct USEXXObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 SEXXObjectBase 成功，共 6345 条记录


struct zUSEXXObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p21damage;				// 最小攻击力
	DWORD m21axpdamage;			// 最大攻击力
	DWORD m21damage;				// 最小法术攻击力
	DWORD m21axmdamage;			// 最大法术攻击力

	DWORD p21defence;			// 物防
	DWORD m21defence;			// 魔防
	DWORD m21axhp;				// 最大生命值

	void fill(USEXXObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p21damage = data.dwField7;
		m21axpdamage = data.dwField8;
		m21damage = data.dwField9;
		m21axmdamage = data.dwField10;

		p21defence = data.dwField11;
		m21defence = data.dwField12;

		m21axhp = data.dwField13;
	}

	zUSEXXObjectB():zEntry()
	{
		bzero(this,sizeof(zUSEXXObjectB));
	}
};

//------------------------------------
// AngelObjectBase-翅膀升级
//------------------------------------
struct UangelObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 AngelObjectBase 成功，共 6345 条记录


struct zUangelObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p8damage;				// 最小攻击力
	DWORD m8axpdamage;			// 最大攻击力
	DWORD m8damage;				// 最小法术攻击力
	DWORD m8axmdamage;			// 最大法术攻击力

	DWORD p8defence;			// 物防
	DWORD m8defence;			// 魔防
	DWORD m8axhp;				// 最大生命值

	void fill(UangelObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p8damage = data.dwField7;
		m8axpdamage = data.dwField8;
		m8damage = data.dwField9;
		m8axmdamage = data.dwField10;

		p8defence = data.dwField11;
		m8defence = data.dwField12;

		m8axhp = data.dwField13;
	}

	zUangelObjectB():zEntry()
	{
		bzero(this,sizeof(zUangelObjectB));
	}
};

//------------------------------------
// MagicalObjectBasee-神兵升级
//------------------------------------

struct MagicalObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 MagicalObjectBase 成功，共 6345 条记录

struct zMagicalObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		    // 类型

	DWORD stuff;	// 升级原料

	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p7damage;				// 最小攻击力
	DWORD m7axpdamage;			// 最大攻击力
	DWORD m7damage;				// 最小法术攻击力
	DWORD m7axmdamage;			// 最大法术攻击力

	DWORD p7defence;			    // 物防
	DWORD m7defence;			    // 魔防
	DWORD m7axhp;				// 最大生命值

	void fill(MagicalObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;
		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p7damage = data.dwField7;
		m7axpdamage = data.dwField8;
		m7damage = data.dwField9;
		m7axmdamage = data.dwField10;

		p7defence = data.dwField11;
		m7defence = data.dwField12;

		m7axhp = data.dwField13;
	}

	zMagicalObjectB():zEntry()
	{
		bzero(this,sizeof(zMagicalObjectB));
	}
};

//------------------------------------
// MythicalObjectBasee-神兽升级
//------------------------------------

struct MythicalObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 MythicalObjectBase 成功，共 6345 条记录

struct zMythicalObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		    // 类型

	DWORD stuff;	// 升级原料

	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p7damage;				// 最小攻击力
	DWORD m7axpdamage;			// 最大攻击力
	DWORD m7damage;				// 最小法术攻击力
	DWORD m7axmdamage;			// 最大法术攻击力

	DWORD p7defence;			    // 物防
	DWORD m7defence;			    // 魔防
	DWORD m7axhp;				// 最大生命值

	void fill(MythicalObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;
		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p7damage = data.dwField7;
		m7axpdamage = data.dwField8;
		m7damage = data.dwField9;
		m7axmdamage = data.dwField10;

		p7defence = data.dwField11;
		m7defence = data.dwField12;

		m7axhp = data.dwField13;
	}

	zMythicalObjectB():zEntry()
	{
		bzero(this,sizeof(zMythicalObjectB));
	}
};



//------------------------------------
// ULongFengyObjectBase-龙凤吟
//------------------------------------
struct UlongfengyingObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 UlongfengyingObjectBase 成功，共 6345 条记录


struct zUlongfengyingObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		        // 类型
	
	DWORD stuff;		    // 升级原料
	DWORD gold;		        // 需要银子
	
	WORD odds;		        // 对应成功率
	
	DWORD p24damage;		    // 最小攻击力
	DWORD m24axpdamage;		// 最大攻击力
	DWORD m24damage;			// 最小法术攻击力
	DWORD m24axmdamage;		// 最大法术攻击力

	DWORD p24defence;		// 物防
	DWORD m24defence;		// 魔防
	DWORD m24axhp;			// 最大生命值
	
	void fill(UlongfengyingObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;
		
		p24damage = data.dwField7;
		m24axpdamage = data.dwField8;
		m24damage = data.dwField9;
		m24axmdamage = data.dwField10;
		
		p24defence = data.dwField11;
		m24defence = data.dwField12;
		
		m24axhp = data.dwField13;
	}

	zUlongfengyingObjectB():zEntry()
	{
		bzero(this,sizeof(zUlongfengyingObjectB));
	}
};



//------------------------------------
// EquipObjectBase - 装备升级
//------------------------------------
struct EquipObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];		// 名称
	DWORD	dwField3;		// 类型
	DWORD	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 EquipObjectBase 成功，共 6345 条记录

struct zEquipObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		    // 类型

	DWORD stuff;		// 升级原料

	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD pdamage;				// 最小攻击力
	DWORD maxpdamage;			// 最大攻击力
	DWORD mdamage;				// 最小法术攻击力
	DWORD maxmdamage;			// 最大法术攻击力

	DWORD pdefence;			    // 物防
	DWORD mdefence;			    // 魔防
	DWORD maxhp;				// 最大生命值

	void fill(EquipObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;
		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		pdamage = data.dwField7;
		maxpdamage = data.dwField8;
		mdamage = data.dwField9;
		maxmdamage = data.dwField10;

		pdefence = data.dwField11;
		mdefence = data.dwField12;

		maxhp = data.dwField13;
	}

	zEquipObjectB():zEntry()
	{
		bzero(this,sizeof(zEquipObjectB));
	}
};


//------------------------------------
// refineObjectBase-马牌精炼
//------------------------------------
struct UrefineObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 refineObjectBase 成功，共 6345 条记录

struct zUrefineObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p10damage;				// 马牌精炼最小攻击力
	DWORD m10axpdamage;			// 马牌精炼最大攻击力
	DWORD m10damage;				//马牌精炼最小法术攻击力
	DWORD m10axmdamage;			// 马牌精炼最大法术攻击力

	DWORD p10defence;			// 马牌精炼物防
	DWORD m10defence;			// 马牌精炼魔防
	DWORD m10axhp;				// 马牌精炼最大生命值

	void fill(UrefineObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p10damage = data.dwField7;
		m10axpdamage = data.dwField8;
		m10damage = data.dwField9;
		m10axmdamage = data.dwField10;

		p10defence = data.dwField11;
		m10defence = data.dwField12;

		m10axhp = data.dwField13;
	}

	zUrefineObjectB():zEntry()
	{
		bzero(this,sizeof(zUrefineObjectB));
	}
};



//------------------------------------
// BrightObjectBase-璀璨镶嵌
//------------------------------------
struct UbrightObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 refineObjectBase 成功，共 6345 条记录

struct zUbrightObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p11damage;				// 璀璨镶嵌 最小攻击力
	DWORD m11axpdamage;			// 璀璨镶嵌 最大攻击力
	DWORD m11damage;				//璀璨镶嵌 最小法术攻击力
	DWORD m11axmdamage;			// 璀璨镶嵌 最大法术攻击力

	DWORD p11defence;			// 璀璨镶嵌 物防
	DWORD m11defence;			// 璀璨镶嵌 魔防
	DWORD m11axhp;				// 璀璨镶嵌 最大生命值

	void fill(UbrightObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p11damage = data.dwField7;
		m11axpdamage = data.dwField8;
		m11damage = data.dwField9;
		m11axmdamage = data.dwField10;

		p11defence = data.dwField11;
		m11defence = data.dwField12;

		m11axhp = data.dwField13;
	}

	zUbrightObjectB():zEntry()
	{
		bzero(this,sizeof(zUbrightObjectB));
	}
};


//------------------------------------
// xuanhuaObjectBase-神斧镶嵌
//------------------------------------
struct UxuanhuaObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 refineObjectBase 成功，共 6345 条记录

struct zUxuanhuaObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p32damage;				// 神斧镶嵌 最小攻击力
	DWORD m32axpdamage;			// 神斧镶嵌 最大攻击力
	DWORD m32damage;				//神斧镶嵌 最小法术攻击力
	DWORD m32axmdamage;			// 神斧镶嵌 最大法术攻击力

	DWORD p32defence;			// 神斧镶嵌 物防
	DWORD m32defence;			// 神斧镶嵌 魔防
	DWORD m32axhp;				// 神斧镶嵌 最大生命值

	void fill(UxuanhuaObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p32damage = data.dwField7;
		m32axpdamage = data.dwField8;
		m32damage = data.dwField9;
		m32axmdamage = data.dwField10;

		p32defence = data.dwField11;
		m32defence = data.dwField12;

		m32axhp = data.dwField13;
	}

	zUxuanhuaObjectB():zEntry()
	{
		bzero(this,sizeof(zUxuanhuaObjectB));
	}
};


//------------------------------------
// heiyaoObjectBase-黑曜镶嵌
//------------------------------------
struct UheiyaoObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 refineObjectBase 成功，共 6345 条记录

struct zUheiyaoObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p33damage;				// 黑曜镶嵌 最小攻击力
	DWORD m33axpdamage;			// 黑曜镶嵌 最大攻击力
	DWORD m33damage;				//黑曜镶嵌 最小法术攻击力
	DWORD m33axmdamage;			// 黑曜镶嵌 最大法术攻击力

	DWORD p33defence;			// 黑曜镶嵌 物防
	DWORD m33defence;			// 黑曜镶嵌 魔防
	DWORD m33axhp;				// 黑曜镶嵌 最大生命值

	void fill(UheiyaoObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p33damage = data.dwField7;
		m33axpdamage = data.dwField8;
		m33damage = data.dwField9;
		m33axmdamage = data.dwField10;

		p33defence = data.dwField11;
		m33defence = data.dwField12;

		m33axhp = data.dwField13;
	}

	zUheiyaoObjectB():zEntry()
	{
		bzero(this,sizeof(zUheiyaoObjectB));
	}
};



//------------------------------------
// jingjinObjectBase-精金镶嵌
//------------------------------------
struct UjingjinObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 refineObjectBase 成功，共 6345 条记录

struct zUjingjinObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p34damage;				// 精金镶嵌 最小攻击力
	DWORD m34axpdamage;			// 精金镶嵌 最大攻击力
	DWORD m34damage;				//精金镶嵌 最小法术攻击力
	DWORD m34axmdamage;			// 精金镶嵌 最大法术攻击力

	DWORD p34defence;			// 精金镶嵌 物防
	DWORD m34defence;			// 精金镶嵌 魔防
	DWORD m34axhp;				// 精金镶嵌 最大生命值

	void fill(UjingjinObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p34damage = data.dwField7;
		m34axpdamage = data.dwField8;
		m34damage = data.dwField9;
		m34axmdamage = data.dwField10;

		p34defence = data.dwField11;
		m34defence = data.dwField12;

		m34axhp = data.dwField13;
	}

	zUjingjinObjectB():zEntry()
	{
		bzero(this,sizeof(zUjingjinObjectB));
	}
};


//-----------金箍咒强化开始---------------
struct UjgzqhObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
}; 

struct zUjgzqhObjectB:public zEntry
{
	DWORD dwObjectID;   // 物品ID
	WORD level;		    // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		    // 需要银子

	WORD odds;		    // 对应成功率

	DWORD p49damage;			// 金箍咒强化 最小攻击力
	DWORD m49axpdamage;			// 金箍咒强化 最大攻击力
	DWORD m49damage;			// 金箍咒强化 最小法术攻击力
	DWORD m49axmdamage;			// 金箍咒强化 最大法术攻击力

	DWORD p49defence;			// 金箍咒强化 物防
	DWORD m49defence;			// 金箍咒强化 魔防
	DWORD m49axhp;				// 金箍咒强化 最大生命值

	void fill(UjgzqhObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p49damage = data.dwField7;
		m49axpdamage = data.dwField8;
		m49damage = data.dwField9;
		m49axmdamage = data.dwField10;

		p49defence = data.dwField11;
		m49defence = data.dwField12;

		m49axhp = data.dwField13;
	}

	zUjgzqhObjectB():zEntry()
	{
		bzero(this,sizeof(zUjgzqhObjectB));
	}
};
//-----------金箍咒强化结束---------------
//-----------金箍咒字纹开始---------------
struct UjgzzwObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
}; 

struct zUjgzzwObjectB:public zEntry
{
	DWORD dwObjectID;   // 物品ID
	WORD level;		    // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		    // 需要银子

	WORD odds;		    // 对应成功率

	DWORD p50damage;			// 金箍咒字纹 最小攻击力
	DWORD m50axpdamage;			// 金箍咒字纹 最大攻击力
	DWORD m50damage;			// 金箍咒字纹 最小法术攻击力
	DWORD m50axmdamage;			// 金箍咒字纹 最大法术攻击力

	DWORD p50defence;			// 金箍咒字纹 物防
	DWORD m50defence;			// 金箍咒字纹 魔防
	DWORD m50axhp;				// 金箍咒字纹 最大生命值

	void fill(UjgzzwObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p50damage = data.dwField7;
		m50axpdamage = data.dwField8;
		m50damage = data.dwField9;
		m50axmdamage = data.dwField10;

		p50defence = data.dwField11;
		m50defence = data.dwField12;

		m50axhp = data.dwField13;
	}

	zUjgzzwObjectB():zEntry()
	{
		bzero(this,sizeof(zUjgzzwObjectB));
	}
};
//-----------金箍咒字纹结束---------------
//-----------金箍咒淬炼开始---------------
struct UjgzclObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
}; 

struct zUjgzclObjectB:public zEntry
{
	DWORD dwObjectID;   // 物品ID
	WORD level;		    // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		    // 需要银子

	WORD odds;		    // 对应成功率

	DWORD p51damage;			// 金箍咒淬炼 最小攻击力
	DWORD m51axpdamage;			// 金箍咒淬炼 最大攻击力
	DWORD m51damage;			// 金箍咒淬炼 最小法术攻击力
	DWORD m51axmdamage;			// 金箍咒淬炼 最大法术攻击力

	DWORD p51defence;			// 金箍咒淬炼 物防
	DWORD m51defence;			// 金箍咒淬炼 魔防
	DWORD m51axhp;				// 金箍咒淬炼 最大生命值

	void fill(UjgzclObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p51damage = data.dwField7;
		m51axpdamage = data.dwField8;
		m51damage = data.dwField9;
		m51axmdamage = data.dwField10;

		p51defence = data.dwField11;
		m51defence = data.dwField12;

		m51axhp = data.dwField13;
	}

	zUjgzclObjectB():zEntry()
	{
		bzero(this,sizeof(zUjgzclObjectB));
	}
};
//-----------金箍咒淬炼结束---------------
//-----------寒冰神炼开始---------------
struct UhbslObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
}; 

struct zUhbslObjectB:public zEntry
{
	DWORD dwObjectID;   // 物品ID
	WORD level;		    // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		    // 需要银子

	WORD odds;		    // 对应成功率

	DWORD p52damage;			// 寒冰神炼 最小攻击力
	DWORD m52axpdamage;			// 寒冰神炼 最大攻击力
	DWORD m52damage;			// 寒冰神炼 最小法术攻击力
	DWORD m52axmdamage;			// 寒冰神炼 最大法术攻击力

	DWORD p52defence;			// 寒冰神炼 物防
	DWORD m52defence;			// 寒冰神炼 魔防
	DWORD m52axhp;				// 寒冰神炼 最大生命值

	void fill(UhbslObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p52damage = data.dwField7;
		m52axpdamage = data.dwField8;
		m52damage = data.dwField9;
		m52axmdamage = data.dwField10;

		p52defence = data.dwField11;
		m52defence = data.dwField12;

		m52axhp = data.dwField13;
	}

	zUhbslObjectB():zEntry()
	{
		bzero(this,sizeof(zUhbslObjectB));
	}
};
//-----------寒冰神炼结束---------------
//-----------寒冰字纹开始---------------
struct UhbzwObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
}; 

struct zUhbzwObjectB:public zEntry
{
	DWORD dwObjectID;   // 物品ID
	WORD level;		    // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		    // 需要银子

	WORD odds;		    // 对应成功率

	DWORD p53damage;			// 寒冰字纹 最小攻击力
	DWORD m53axpdamage;			// 寒冰字纹 最大攻击力
	DWORD m53damage;			// 寒冰字纹 最小法术攻击力
	DWORD m53axmdamage;			// 寒冰字纹 最大法术攻击力

	DWORD p53defence;			// 寒冰字纹 物防
	DWORD m53defence;			// 寒冰字纹 魔防
	DWORD m53axhp;				// 寒冰字纹 最大生命值

	void fill(UhbzwObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p53damage = data.dwField7;
		m53axpdamage = data.dwField8;
		m53damage = data.dwField9;
		m53axmdamage = data.dwField10;

		p53defence = data.dwField11;
		m53defence = data.dwField12;

		m53axhp = data.dwField13;
	}

	zUhbzwObjectB():zEntry()
	{
		bzero(this,sizeof(zUhbzwObjectB));
	}
};
//-----------寒冰字纹结束---------------
//-----------寒冰纹章开始---------------
struct UhbwzObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
}; 

struct zUhbwzObjectB:public zEntry
{
	DWORD dwObjectID;   // 物品ID
	WORD level;		    // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		    // 需要银子

	WORD odds;		    // 对应成功率

	DWORD p54damage;			// 寒冰纹章 最小攻击力
	DWORD m54axpdamage;			// 寒冰纹章 最大攻击力
	DWORD m54damage;			// 寒冰纹章 最小法术攻击力
	DWORD m54axmdamage;			// 寒冰纹章 最大法术攻击力

	DWORD p54defence;			// 寒冰纹章 物防
	DWORD m54defence;			// 寒冰纹章 魔防
	DWORD m54axhp;				// 寒冰纹章 最大生命值

	void fill(UhbwzObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p54damage = data.dwField7;
		m54axpdamage = data.dwField8;
		m54damage = data.dwField9;
		m54axmdamage = data.dwField10;

		p54defence = data.dwField11;
		m54defence = data.dwField12;

		m54axhp = data.dwField13;
	}

	zUhbwzObjectB():zEntry()
	{
		bzero(this,sizeof(zUhbwzObjectB));
	}
};
//-----------寒冰纹章结束---------------
//------------------------------------
// HandbookObjectBase-百兽图鉴进阶
//------------------------------------
struct UhandbookObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};

struct zUhandbookObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型

	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子

	WORD odds;		    // 对应成功率

	DWORD p13damage;				// 百兽图鉴进阶 最小攻击力
	DWORD m13axpdamage;			// 百兽图鉴进阶 最大攻击力
	DWORD m13damage;				//百兽图鉴进阶 最小法术攻击力
	DWORD m13axmdamage;			// 百兽图鉴进阶 最大法术攻击力

	DWORD p13defence;			// 百兽图鉴进阶 物防
	DWORD m13defence;			// 百兽图鉴进阶 魔防
	DWORD m13axhp;				// 百兽图鉴进阶 最大生命值

	void fill(UhandbookObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;

		p13damage = data.dwField7;
		m13axpdamage = data.dwField8;
		m13damage = data.dwField9;
		m13axmdamage = data.dwField10;

		p13defence = data.dwField11;
		m13defence = data.dwField12;

		m13axhp = data.dwField13;
	}

	zUhandbookObjectB():zEntry()
	{
		bzero(this,sizeof(zUhandbookObjectB));
	}
};


//------------------------------------
// UlongwanglvObjectBase-神龙养成镶嵌升级
//------------------------------------
struct UlongwanglvObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 UlongwanglvObjectBase 成功，共 6345 条记录


struct zUlongwanglvObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型
	
	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子
	
	WORD odds;		    // 对应成功率
	
	DWORD p14damage;		    // 最小攻击力
	DWORD m14axpdamage;			// 最大攻击力
	DWORD m14damage;			// 最小法术攻击力
	DWORD m14axmdamage;			// 最大法术攻击力

	DWORD p14defence;			// 物防
	DWORD m14defence;			// 魔防
	DWORD m14axhp;				// 最大生命值
	
	void fill(UlongwanglvObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;
		
		p14damage = data.dwField7;
		m14axpdamage = data.dwField8;
		m14damage = data.dwField9;
		m14axmdamage = data.dwField10;
		
		p14defence = data.dwField11;
		m14defence = data.dwField12;
		
		m14axhp = data.dwField13;
	}

	zUlongwanglvObjectB():zEntry()
	{
		bzero(this,sizeof(zUlongwanglvObjectB));
	}
};


//------------------------------------
// UnitianlvObjectBase-逆天进阶升级
//------------------------------------
struct UnitianlvObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 UnitianlvObjectBase 成功，共 6345 条记录


struct zUnitianlvObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型
	
	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子
	
	WORD odds;		    // 对应成功率
	
	DWORD p15damage;		    // 最小攻击力
	DWORD m15axpdamage;			// 最大攻击力
	DWORD m15damage;			// 最小法术攻击力
	DWORD m15axmdamage;			// 最大法术攻击力

	DWORD p15defence;			// 物防
	DWORD m15defence;			// 魔防
	DWORD m15axhp;				// 最大生命值
	
	void fill(UnitianlvObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;
		
		p15damage = data.dwField7;
		m15axpdamage = data.dwField8;
		m15damage = data.dwField9;
		m15axmdamage = data.dwField10;
		
		p15defence = data.dwField11;
		m15defence = data.dwField12;
		
		m15axhp = data.dwField13;
	}

	zUnitianlvObjectB():zEntry()
	{
		bzero(this,sizeof(zUnitianlvObjectB));
	}
};


//------------------------------------
// UdouzhansflvObjectBase-斗战圣佛神魂镶嵌升级
//------------------------------------
struct UdouzhansflvObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 UdouzhansflvObjectBase 成功，共 6345 条记录


struct zUdouzhansflvObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型
	
	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子
	
	WORD odds;		    // 对应成功率
	
	DWORD p16damage;		    // 最小攻击力
	DWORD m16axpdamage;			// 最大攻击力
	DWORD m16damage;			// 最小法术攻击力
	DWORD m16axmdamage;			// 最大法术攻击力

	DWORD p16defence;			// 物防
	DWORD m16defence;			// 魔防
	DWORD m16axhp;				// 最大生命值
	
	void fill(UdouzhansflvObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;
		
		p16damage = data.dwField7;
		m16axpdamage = data.dwField8;
		m16damage = data.dwField9;
		m16axmdamage = data.dwField10;
		
		p16defence = data.dwField11;
		m16defence = data.dwField12;
		
		m16axhp = data.dwField13;
	}

	zUdouzhansflvObjectB():zEntry()
	{
		bzero(this,sizeof(zUdouzhansflvObjectB));
	}
};


//------------------------------------
// UaystonelvObjectBase-暗影宝石镶嵌进阶
//------------------------------------
struct UaystonelvObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 UaystonelvObjectBase 成功，共 6345 条记录


struct zUaystonelvObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型
	
	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子
	
	WORD odds;		    // 对应成功率
	
	DWORD p17damage;		    // 最小攻击力
	DWORD m17axpdamage;			// 最大攻击力
	DWORD m17damage;			// 最小法术攻击力
	DWORD m17axmdamage;			// 最大法术攻击力

	DWORD p17defence;			// 物防
	DWORD m17defence;			// 魔防
	DWORD m17axhp;				// 最大生命值
	
	void fill(UaystonelvObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;
		
		p17damage = data.dwField7;
		m17axpdamage = data.dwField8;
		m17damage = data.dwField9;
		m17axmdamage = data.dwField10;
		
		p17defence = data.dwField11;
		m17defence = data.dwField12;
		
		m17axhp = data.dwField13;
	}

	zUaystonelvObjectB():zEntry()
	{
		bzero(this,sizeof(zUaystonelvObjectB));
	}
};


//------------------------------------
// UshangfanglvObjectBase-尚方宝剑升级
//------------------------------------
struct UshangfanglvObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 UshangfanglvObjectBase 成功，共 6345 条记录


struct zUshangfanglvObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型
	
	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子
	
	WORD odds;		    // 对应成功率
	
	DWORD p18damage;		    // 最小攻击力
	DWORD m18axpdamage;			// 最大攻击力
	DWORD m18damage;			// 最小法术攻击力
	DWORD m18axmdamage;			// 最大法术攻击力

	DWORD p18defence;			// 物防
	DWORD m18defence;			// 魔防
	DWORD m18axhp;				// 最大生命值
	
	void fill(UshangfanglvObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;
		
		p18damage = data.dwField7;
		m18axpdamage = data.dwField8;
		m18damage = data.dwField9;
		m18axmdamage = data.dwField10;
		
		p18defence = data.dwField11;
		m18defence = data.dwField12;
		
		m18axhp = data.dwField13;
	}

	zUshangfanglvObjectB():zEntry()
	{
		bzero(this,sizeof(zUshangfanglvObjectB));
	}
};


//------------------------------------
// UzijinlplvObjectBase-紫金龙袍升级
//------------------------------------
struct UzijinlplvObjectBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	char	strField2[64];	// 名称
	DWORD	dwField3;		// 类型
	DWORD 	dwField4;		// 升级原料
	DWORD	dwField5;		// 需要银子
	DWORD	dwField6;		// 对应成功率
	DWORD	dwField7;		// 最小物攻增加
	DWORD	dwField8;		// 最大物攻增加
	DWORD	dwField9;		// 最小魔攻增加
	DWORD	dwField10;		// 最大魔攻增加
	DWORD	dwField11;		// 物防增加
	DWORD	dwField12;		// 魔防增加
	DWORD	dwField13;		// 生命值增加
};//导出 UzijinlplvObjectBase 成功，共 6345 条记录


struct zUzijinlplvObjectB:public zEntry
{
	DWORD dwObjectID;		// 物品ID
	WORD level;		     // 类型
	
	DWORD stuff;		// 升级原料
	DWORD gold;		// 需要银子
	
	WORD odds;		    // 对应成功率
	
	DWORD p19damage;		    // 最小攻击力
	DWORD m19axpdamage;			// 最大攻击力
	DWORD m19damage;			// 最小法术攻击力
	DWORD m19axmdamage;			// 最大法术攻击力

	DWORD p19defence;			// 物防
	DWORD m19defence;			// 魔防
	DWORD m19axhp;				// 最大生命值
	
	void fill(UzijinlplvObjectBase  &data)
	{
		id = data.dwField0;
		dwObjectID = data.dwField1;
		strncpy(name, data.strField2, MAX_NAMESIZE);
		level = data.dwField3;

		stuff = data.dwField4;
		gold = data.dwField5;
		odds = data.dwField6;
		
		p19damage = data.dwField7;
		m19axpdamage = data.dwField8;
		m19damage = data.dwField9;
		m19axmdamage = data.dwField10;
		
		p19defence = data.dwField11;
		m19defence = data.dwField12;
		
		m19axhp = data.dwField13;
	}

	zUzijinlplvObjectB():zEntry()
	{
		bzero(this,sizeof(zUzijinlplvObjectB));
	}
};

//------------------------------------
// NpcBase
//------------------------------------
struct NpcBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	char	strField1[32];		// 名称
	DWORD	dwField2;		// 类型
	DWORD	dwField3;		// 等级
	DWORD	dwField4;		// 生命值
	DWORD	dwField5;		// 经验值

	DWORD	dwField6;		// 力
	DWORD	dwField7;		// 智
	DWORD	dwField8;		// 敏捷
	DWORD	dwField9;		// 精神
	DWORD	dwField10;		// 体质
	DWORD	dwField11;		// 体质

	DWORD	dwField12;		// 颜色
	DWORD	dwField13;		// ai
	DWORD	dwField14;		// 移动间隔
	DWORD	dwField15;		// 攻击间隔
	DWORD	dwField16;		// 最小物理防御力
	DWORD	dwField17;		// 最大物理防御力
	DWORD	dwField18;		// 最小法术防御力
	DWORD	dwField19;		// 最大法术防御力
	DWORD	dwField20;		// 五行属性
	DWORD	dwField21;		// 五行点数
	char	strField22[1024];		// 攻击类型
	DWORD	dwField23;		// 最小法术攻击
	DWORD	dwField24;		// 最大法术攻击
	DWORD	dwField25;		// 最小攻击力
	DWORD	dwField26;		// 最大攻击力
	DWORD	dwField27;		// 技能
	char	strField28[4096];		// 携带物品
	DWORD	dwField29;		// 魂魄之石几率
	char	strField30[1024];		// 使用技能
	char	strField31[1024];		// 状态
	DWORD	dwField32;		// 躲避率
	DWORD	dwField33;		// 命中率
	DWORD	dwField34;		// 图片
	DWORD	dwField35;		// 品质
	DWORD	dwField36;		// 怪物类别
	DWORD	dwField37;		// 纸娃娃图片
	char	strField38[64];		// 回血
	DWORD	dwField39;		// 二进制标志
	DWORD	dwField40;		// 二进制标志
};

struct CarryObject
{
	DWORD id;
	int   rate;
	int   minnum;
	int   maxnum;
	CarryObject()
	{
		id = 0;
		rate = 0;
		minnum = 0;
		maxnum = 0;
	}
};
#ifdef _POOL_ALLOC_
#include <ext/pool_allocator.h>
typedef std::vector<CarryObject, __gnu_cxx::__pool_alloc<CarryObject> > NpcLostObject;
#else
typedef std::vector<CarryObject> NpcLostObject;
#endif

struct NpcCarryObject : private zNoncopyable
{
	NpcCarryObject() {};
	bool set(const char *objects)
	{
		bool retval = true;
		//mlock.lock();
		cov.clear();
		if (strcmp(objects, "0"))
		{
			std::vector<std::string> obs;
			Zebra::stringtok(obs, objects, ";");
			for(std::vector<std::string>::const_iterator it = obs.begin(); it != obs.end(); it++)
			{
				std::vector<std::string> rt;
				Zebra::stringtok(rt, *it, ":");
				if (3 == rt.size())
				{
					CarryObject co;
					co.id = atoi(rt[0].c_str());
					co.rate = atoi(rt[1].c_str());
					std::vector<std::string> nu;
					Zebra::stringtok(nu, rt[2], "-");
					if (2 == nu.size())
					{
						co.minnum = atoi(nu[0].c_str());
						co.maxnum = atoi(nu[1].c_str());
						cov.push_back(co);
					}
					else
						retval = false;
				}
				else
					retval = false;
			}
		}
		//mlock.unlock();
		return retval;
	}

	/**
	 * \brief 物品掉落处理
	 * \param nlo npc携带物品集合
	 * \param value 掉落率打折比
	 * \param value1 掉落率增加
	 * \param value2 银子掉落率增加
	 */
	void lost(NpcLostObject &nlo, int value=1, int value1=0, int value2=0, int vcharm = 0, int vlucky = 0, int player_level=0, bool double_drop=false, int level_double_drop=0)
	{
		//mlock.lock();
		if (vcharm>1000) vcharm=1000;
		if (vlucky>1000) vlucky=1000;
		for(std::vector<CarryObject>::const_iterator it = cov.begin(); it != cov.end(); it++)
		{
			//Zebra::logger->debug("%u, %u, %u, %u", (*it).id, (*it).rate, (*it).minnum, (*it).maxnum);
			switch((*it).id)
			{
				case 665:
					{
						int vrate = (int)(((*it).rate/value)*(1+value1/100.0f)*(1+value2/100.0f)*(1+vcharm/1000.0f)*(1+vlucky/1000.0f));
						if (zMisc::selectByTenTh(vrate))
						{
							nlo.push_back(*it);
						}
					}
					break;
				default:
					{
						int vrate = (int)(((*it).rate/value)*(1+value1/100.0f)*(1+vcharm/1000.0f)*(1+vlucky/1000.0f));
						if (double_drop && player_level<= level_double_drop)
						{
							if (zMisc::selectByTenTh(vrate*2))
							{
								nlo.push_back(*it);
							}
						}
						else
						{
							if (zMisc::selectByTenTh(vrate))
							{
								nlo.push_back(*it);
							}
						}
					}
					break;
			}
		}
		//mlock.unlock();
	}
	/**
	 * \brief 全部物品掉落处理
	 * \param nlo npc携带物品集合
	 * \param value 掉落率打折比
	 * \param value1 掉落率增加
	 * \param value2 银子掉落率增加
	 */
	void lostAll(NpcLostObject &nlo)
	{
		for(std::vector<CarryObject>::const_iterator it = cov.begin(); it != cov.end(); it++)
		{
			nlo.push_back(*it);
		}
	}

	/**
	 * \brief 装备物品全部掉落处理(绿怪专用)
	 * \param nlo npc携带物品集合
	 * \param value 掉落率打折比
	 * \param value1 掉落率增加
	 * \param value2 银子掉落率增加
	 */
	void lostGreen(NpcLostObject &nlo, int value=1, int value1=0, int value2=0, int vcharm = 0, int vlucky = 0);
	private:
		std::vector<CarryObject> cov;
		//zMutex mlock;
};


struct aTypeS{
	aTypeS()
	{
		byValue[0] = 0;
		byValue[1] = 0;
	}
	union {
		struct {
			BYTE byAType;
			BYTE byAction;
		};
		BYTE byValue[2];
	};
};

enum
{
	NPC_TYPE_HUMAN		= 0,	///人型
	NPC_TYPE_NORMAL		= 1,	/// 普通类型
	NPC_TYPE_BBOSS		= 2,	/// 大Boss类型
	NPC_TYPE_LBOSS		= 3,	/// 小Boss类型
	NPC_TYPE_BACKBONE	= 4,	/// 精英类型
	NPC_TYPE_GOLD		= 5,	/// 黄金类型
	NPC_TYPE_TRADE		= 6,	/// 买卖类型
	NPC_TYPE_TASK		= 7,	/// 任务类型
	NPC_TYPE_GUARD		= 8,	/// 士兵类型
	NPC_TYPE_PET		= 9,	/// 宠物类型
	NPC_TYPE_BACKBONEBUG= 10,	/// 精怪类型
	NPC_TYPE_SUMMONS	= 11,	/// 召唤类型
	NPC_TYPE_TOTEM		= 12,	/// 图腾类型
	NPC_TYPE_AGGRANDIZEMENT = 13,/// 强化类型
	NPC_TYPE_ABERRANCE	= 14,	/// 变异类型
	NPC_TYPE_STORAGE	= 15,	/// 仓库类型
	NPC_TYPE_ROADSIGN	= 16,	/// 路标类型
	NPC_TYPE_TREASURE	= 17,	/// 宝箱类型
	NPC_TYPE_WILDHORSE	= 18,	/// 野马类型
	NPC_TYPE_MOBILETRADE	= 19,	/// 流浪小贩
	NPC_TYPE_LIVENPC	= 20,	/// 生活npc（不战斗，攻城时消失）
	NPC_TYPE_DUCKHIT	= 21,	/// 蹲下才能打的npc
	NPC_TYPE_BANNER		= 22,	/// 旗帜类型
	NPC_TYPE_TRAP		= 23,	/// 陷阱类型
	NPC_TYPE_MAILBOX	=24,	///邮箱
	NPC_TYPE_AUCTION	=25,	///拍卖管理员
	NPC_TYPE_UNIONGUARD	=26,	///帮会守卫
	NPC_TYPE_SOLDIER	=27,	///士兵，只攻击外国人
	NPC_TYPE_UNIONATTACKER	=28,	///攻方士兵
	NPC_TYPE_SURFACE = 29,	/// 地表类型
	NPC_TYPE_CARTOONPET = 30,	/// 替身宝宝
	NPC_TYPE_PBOSS = 31,	/// 紫色BOSS
	NPC_TYPE_RESOURCE = 32,  /// 资源类NPC
	NPC_TYPE_REDBOSS = 33	/// 不受保护的BOSS类型	
};

enum
{
	NPC_ATYPE_NEAR		= 1,	/// 近距离攻击
	NPC_ATYPE_FAR		= 2,	/// 远距离攻击
	NPC_ATYPE_MFAR		= 3,	/// 法术远程攻击
	NPC_ATYPE_MNEAR		= 4,	/// 法术近身攻击
	NPC_ATYPE_NOACTION  = 5		/// 无攻击动作
};

///npc使用一个技能的描述
struct npcSkill
{
	DWORD id;///技能id
	int needLevel;///技能id
	int rate;///使用几率
	int coefficient;///升级系数

	npcSkill():id(0),needLevel(0),rate(0),coefficient(0){}
	npcSkill(const npcSkill &skill)
	{
		id = skill.id;
		needLevel = skill.needLevel;
		rate = skill.rate;
		coefficient = skill.coefficient;
	}
	npcSkill& operator = (const npcSkill &skill)
	{
		id = skill.id;
		needLevel = skill.needLevel;
		rate = skill.rate;
		coefficient = skill.coefficient;
		return *this;
	}
};

struct npcRecover
{
	DWORD start;
	BYTE type;
	DWORD num;

	npcRecover()
	{
		start = 0;
		type = 0;
		num = 0;
	}

	void parse(const char * str)
	{
		if (!str) return;

		std::vector<std::string> vec;

		vec.clear();
		Zebra::stringtok(vec, str, ":");
		if (3==vec.size())
		{
			start = atoi(vec[0].c_str());
			type = atoi(vec[1].c_str());
			num = atoi(vec[2].c_str());
		}
	}
};

/**
 * \brief Npc基本表格数据
 *
 */
struct zNpcB : public zEntry
{
	DWORD	kind;				// 类型
	DWORD	level;				// 等级
	QWORD	hp;					// 生命值
	DWORD	exp;				// 经验值
	DWORD	str;				// 力量
	DWORD   inte;				// 智力
	DWORD   dex;				// 敏捷
	DWORD   men;				// 精神
	DWORD   con;				// 体质
	DWORD   cri;				// 暴击
	DWORD	color;				// 颜色
	DWORD	ai;					// ai
	DWORD	distance;			// 移动间隔
	DWORD	adistance;			// 攻击间隔
	DWORD	pdefence;			// 最小物理防御力
	DWORD	maxpdefence;		// 最大物理防御力
	DWORD	mdefence;			// 最小法术防御力
	DWORD	maxmdefence;		// 最大法术防御力
	DWORD	five;				// 五行属性
	DWORD   fivepoint;			// 五行点数
	std::vector<aTypeS> atypelist;	// 攻击类型
	DWORD	mdamage;			// 最小法术攻击
	DWORD	maxmdamage;			// 最大法术攻击
	DWORD	damage;				// 最小攻击力
	DWORD	maxdamage;			// 最大攻击力
	DWORD	skill;				// 技能
	//char	object[1024 + 1];	// 携带物品
	NpcCarryObject nco;
	DWORD	soulrate;			// 魂魄之石几率
	char	skills[1024];		// 使用技能
	char	state[1024];		// 状态
	DWORD	dodge;				// 躲避率
	DWORD	rating;				// 命中率
	DWORD	pic;				// 图片
	DWORD	trait;				//品质
	DWORD	bear_type;			//怪物类别
	DWORD	pet_pic;			//宠物图片
	npcRecover recover;
	DWORD	flags;			//二进制标志，目前有一个，可不可被外国人杀
	DWORD	allyVisit;			//可被盟国访问的等级 0：不可访问 1：1级可访问 2：2级可访问

	std::map<int, std::vector<npcSkill> > skillMap;
	
	bool parseSkills(const char * str)
	{
		skillMap.clear();
		strncpy(skills, str, sizeof(skills));
		
		bool ret = false;
		std::vector<std::string> type_v;
		Zebra::stringtok(type_v, str, ";");
		if (type_v.size()>0)
		{
			std::vector<std::string> type_sub_v, skill_v, prop_v;
			std::vector<std::string>::iterator type_it, skill_it;

			for (type_it=type_v.begin();type_it!=type_v.end();type_it++)
			{
				type_sub_v.clear();
				Zebra::stringtok(type_sub_v, type_it->c_str(), ":");
				if (2==type_sub_v.size())
				{
					int type = atoi(type_sub_v[0].c_str());

					std::vector<npcSkill> oneTypeSkills;
					skill_v.clear();
					Zebra::stringtok(skill_v, type_sub_v[1].c_str(), ",");
					for (skill_it=skill_v.begin();skill_it!=skill_v.end();skill_it++)
					{
						prop_v.clear();
						Zebra::stringtok(prop_v, skill_it->c_str(), "-");
						if (4==prop_v.size())
						{
							npcSkill oneSkill;
							oneSkill.id = atoi(prop_v[0].c_str());
							oneSkill.needLevel = atoi(prop_v[1].c_str());
							oneSkill.rate = atoi(prop_v[2].c_str());
							oneSkill.coefficient = atoi(prop_v[3].c_str());

							oneTypeSkills.push_back(oneSkill);
						}
					}
					if (oneTypeSkills.size()>0)
					{
						skillMap[type] = oneTypeSkills;
						ret = true;
					}
				}
			}
		}
		return ret;
	}

	/**
	 * \brief 根据类型随机取出一个npc技能的描述
	 *
	 * \param type 技能类型
	 * \param skill 返回值，取得的技能描述
	 * \return 是否取得成功
	 */
	bool getRandomSkillByType(int type, npcSkill &skill)
	{
		if (skillMap.find(type)==skillMap.end()) return false;
	
		skill = skillMap[type][zMisc::randBetween(0, skillMap[type].size()-1)];
		return true;
	}

	/**
	 * \brief 取得所有可用的技能ID
	 *
	 *
	 * \param list 技能ID列表
	 * \return bool 是否有技能
	 */
	bool getAllSkills(std::vector<DWORD> & list, WORD level)
	{
		std::map<int, std::vector<npcSkill> >::iterator type_it;
		std::vector<npcSkill>::iterator skill_it;
		for (type_it=skillMap.begin();type_it!=skillMap.end();type_it++)
		{
			for (skill_it=type_it->second.begin();skill_it!=type_it->second.end();skill_it++)
				if (level>=skill_it->needLevel)
					list.push_back(skill_it->id);
		}
		return list.size()>0;
	}

	/**
	 * \brief 增加一个npc技能
	 * \param type 技能分类
	 * \param id 要增加的技能id
	 * \param rate 施放几率
	 * \param coefficient 系数
	 */
	void addSkill(int type, DWORD id, int needLevel, int rate, int coefficient = 0)
	{
		npcSkill s;
		s.id = id;
		s.needLevel = needLevel;
		s.rate = rate;
		s.coefficient = coefficient;
		skillMap[type].push_back(s);
	}

	/**
	 * \brief 删除一个npc技能
	 *
	 *
	 * \param id 要删除的技能id
	 * \return npc没有该技能则返回false
	 */
	bool delSkill(DWORD id)
	{
		std::map<int, std::vector<npcSkill> >::iterator v_it;
		for (v_it=skillMap.begin();v_it!=skillMap.end();v_it++)
		{
			std::vector<npcSkill> v = v_it->second;
			std::vector<npcSkill>::iterator s_it;
			for (s_it=v.begin();s_it!=v.end();s_it++)
			{
				if (s_it->id==id)
				{
					v.erase(s_it);
					return true;
				}
			}
		}
		return false;
	}

	/**
	 * \brief 设置npc的攻击类型
	 *
	 *
	 * \param data 传入的字符串
	 * \param size 字符串大小
	 */
	void setAType(const char *data , int size)
	{
		atypelist.clear(); 
		char Buf[1024];
		bzero(Buf , size + 1);
		strncpy(Buf , data , size);
		std::vector<std::string> v_fir;
		Zebra::stringtok(v_fir , Buf , ":");
		for(std::vector<std::string>::iterator iter = v_fir.begin() ; iter != v_fir.end() ; iter++)
		{
			std::vector<std::string> v_sec;
			Zebra::stringtok(v_sec , iter->c_str() , "-");

			if(v_sec.size() != 2)
			{
				return;
			}

			aTypeS aValue;
			std::vector<std::string>::iterator iter_1 = v_sec.begin();

			for(int i=0; i<2; i++)
			{
				aValue.byValue[i] = (BYTE)atoi(iter_1->c_str());
				iter_1 ++;
			}
			atypelist.push_back(aValue);
#ifdef _DEBUGLOG
			Zebra::logger->error("NPC攻击类型: id=%u name=%s atype=%u action=%u ", id, name, aValue.byValue[0], aValue.byValue[1]);
#endif
		}
#ifdef _DEBUGLOG
			Zebra::logger->error("---------------------------------------------");
#endif
		return;
	}

	/**
	 * \brief 取得npc的攻击类型和动画类型
	 *
	 *
	 * \param type 输出 攻击类型
	 * \param action
	 */
	void getATypeAndAction(BYTE &type, BYTE &action)
	{		
		int size = atypelist.size();
		if (size == 0)
		{
			type = NPC_ATYPE_NEAR;
			action = 4 ;//Cmd::AniTypeEnum::Ani_Attack;//Cmd::Ani_Attack
			return;
		}
		int num = zMisc::randBetween(0,size-1);
		type = atypelist[num].byAType;
		action = atypelist[num].byAction;
	}

	/**
	 * \brief 根据表格中读出的数据填充zNpcB结构
	 *
	 *
	 * \param npc 从表中读出的数据
	 */
	void fill(const NpcBase &npc)
	{
		id=					npc.dwField0;
		strncpy(name,		npc.strField1,MAX_NAMESIZE);
		kind=				npc.dwField2;
		level=				npc.dwField3;
		hp=					npc.dwField4;
#ifdef _DEBUGLOG
		Zebra::logger->error("npc加载name=%s hp=%u", name, hp);
#endif
		exp=				npc.dwField5;
		str=				npc.dwField6;
		inte=				npc.dwField7;
		dex=				npc.dwField8;
		men=				npc.dwField9;
		con=				npc.dwField10;
		cri=				npc.dwField11;
		color=				npc.dwField12;
		ai=					npc.dwField13;
		distance=			(0==npc.dwField14)?640:npc.dwField14;
		adistance= 			(0==npc.dwField15)?1000:npc.dwField15;
		pdefence=			npc.dwField16;
		maxpdefence=		npc.dwField17;
		mdefence=			npc.dwField18;
		maxmdefence=		npc.dwField19;
		five=				npc.dwField20;
		fivepoint=			npc.dwField21;
		setAType(npc.strField22, 1024);
		mdamage=			npc.dwField23;
		maxmdamage=			npc.dwField24;
		damage=				npc.dwField25;
		maxdamage=			npc.dwField26;
		skill=				npc.dwField27;
		if (!nco.set(npc.strField28))
			Zebra::logger->error("Npc表格携带物品格式解析错误：%u, %s, \'%s\'", id, name, npc.strField28);
		soulrate=			npc.dwField29;
#ifdef _XWL_DEBUG
		//parseSkills("1:1-0-50-1,3-0-50-1;2:280-0-30-1;3:282-0-20-1,284-0-100-1;");
		//parseSkills("1:1-12-50-1;");
		parseSkills(npc.strField30);
		//Zebra::logger->error("npcB::fill():解析npc技能出错 npcid=%u skills=%s", id, npc.strField30);
#else
		parseSkills(npc.strField30);
		//parseSkills("1:141-20-1,281-20-1;2:284-20-1;3:1-20-1,211-20-1;");
#endif
		strncpy(state,		npc.strField31, 1024);
		dodge=				npc.dwField32;
		rating=				npc.dwField33;
		pic=				npc.dwField34;
		trait=				npc.dwField35;
		bear_type=			npc.dwField36;
		pet_pic=			npc.dwField37;
		recover.parse(npc.strField38);
		flags=				npc.dwField39;
		allyVisit=				npc.dwField40;
#ifdef _XWL_DEBUG
		//recover.parse("50:1:5");
#else
#endif
	}

	zNpcB() : zEntry()
	{
		id=					0;
		bzero(name,			sizeof(name));
		kind=				0;
		level=				0;
		hp=				0;
		exp=				0;
		str=				0;
		inte=				0;
		dex=				0;
		men=				0;
		con=				0;
		cri=				0;
		color=				0;
		ai=				0;
		distance=			0;
		adistance= 			0;
		pdefence=			0;
		maxpdefence=		0;
		mdefence=			0;
		maxmdefence=		0;
		five=				0;
		fivepoint=			0;
		atypelist.clear();
		mdamage=			0;
		maxmdamage=			0;
		damage=				0;
		maxdamage=			0;
		skill=				0;
		//bzero(object,		sizeof(object));
		soulrate=			0;
		bzero(skills,		sizeof(skills));
		bzero(state,		sizeof(state));
		dodge=				0;
		rating=				0;
		pic=				0;
		trait=				0;
		bear_type=			0;
		pet_pic=			0;
		flags=				0;
		allyVisit=			0;
	}

};

//------------------------------------
// 人物经验Base --- 修正
//------------------------------------
struct ExperienceBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 等级
	QWORD	dwField1;		// 需要经验
	DWORD	dwField2;		// 经验加成
};//导出 人物经验Base 成功，共 300 条记录

struct zExperienceB : public zEntry
{
	DWORD	level;			// 等级
	QWORD	nextexp;		// 需要经验
	DWORD	addition;	    // 经验加成

	void fill(const ExperienceBase &data)
	{
		id = data.dwField0;
		snprintf(name,MAX_NAMESIZE,"%u",id);
		nextexp = data.dwField1;
		addition = data.dwField2;
	}

	zExperienceB () : zEntry()
	{
		id = 0;
		nextexp = 0;
		addition = 0;
	}
};

//------------------------------------
// 人物转生经验Base --- 修正
//------------------------------------
struct ZhuanExperienceBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 等级
	QWORD	dwField1;		// 需要经验
};//导出 人物经验Base 成功，共 300 条记录

struct zZhuanExperienceB : public zEntry
{
	DWORD	level;				// 等级
	QWORD	nextexp;			// 需要经验

	void fill(const ZhuanExperienceBase &data)
	{
		id = data.dwField0;
		snprintf(name,MAX_NAMESIZE,"%u",id);
		nextexp = data.dwField1;
	}

	zZhuanExperienceB () : zEntry()
	{
		id = 0;
		nextexp = 0;
	}
};

//------------------------------------
// 荣誉增加表
//------------------------------------
struct HonorBase
{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 等级
	DWORD	dwField1;		// 需要经验
};//导出 人物经验Base 成功，共 300 条记录

struct zHonorB : public zEntry
{
	DWORD	level;				// 等级
	QWORD	value;			//需要经验

	void fill(const HonorBase &data)
	{
		id = data.dwField0;
		snprintf(name,MAX_NAMESIZE,"%u",id);
		value = data.dwField1;
	}

	zHonorB () : zEntry()
	{
		id = 0;
		value = 0;
	}
};

//------------------------------------
// SkillBase
//------------------------------------
/**
 * \brief 根据技能类型和等级计算一个临时唯一编号
 *
 */
#define skill_hash(type, level) ((type - 1) * 10 + level)

struct SkillBase
{
	const DWORD getUniqueID() const
	{
		return skill_hash(dwField0 , dwField2);
	}

	DWORD	dwField0;			// 技能ID
	char	strField1[32];		// 技能名称
	DWORD	dwField2;			// 技能等级
	DWORD	dwField3;			// 技能系别
	DWORD	dwField4;			// 技能树别
	DWORD	dwField5;			// 需要本线技能点数
	DWORD	dwField6;			// 前提技能一
	DWORD	dwField7;			// 前提技能一等级
	DWORD	dwField8;			// 前提技能二
	DWORD	dwField9;			// 前提技能二等级
	DWORD	dwField10;			// 前提技能三
	DWORD	dwField11;			// 前提技能三等级
	DWORD	dwField12;			// 间隔时间
	DWORD	dwField13;			// 攻击方式
	DWORD	dwField14;			// 能否骑马使用
	DWORD	dwField15;			// 需要物品
	char	strField16[128];	// 需要武器
	DWORD	dwField17;			// 消耗体力值
	DWORD	dwField18;			// 消耗法术值
	DWORD	dwField19;			// 消耗生命值
	DWORD	dwField20;			// 伤害加成
	char	strField21[1024];	// 效果
	DWORD	dwField22;			// 消耗物品类型
	DWORD	dwField23;			// 物品消耗数量
};//导出 SkillBase 成功，共 1 条记录

#define BENIGNED_SKILL_STATE 2
#define BAD_SKILL_STATE 4
#define NONE_SKILL_STATE 1 

//桃子技能表属性
struct SkillElement
{
	SkillElement()
	{
		id = 0;
		value = 0;
		percent = 0;
		time = 0;
		state = 0;
	}
	//突破技能属性
	union {
		struct {
			SDWORD id;   //技能id
			SDWORD percent; //技能百分比
			SDWORD value;   //突破技能属性突破SDWORD DWORD QWORD
			SDWORD time;   //技能持续时间
			SDWORD state;  //技能增加状态图标
		};
		SDWORD element[5];
	};
	static SkillElement *create(SkillElement elem);
};
struct SkillStatus
{
	SkillStatus()
	{
		for(int i = 0 ; i < (int)(sizeof(status) / sizeof(WORD)) ; i ++)
		{
			status[i] = 0;
		}
	}
	union {
		struct {
			WORD id;//技能id
			WORD target;//目标
			WORD center;//中心点
			WORD range;//范围
			WORD mode;//飞行模式
			WORD clear;//能否清除
			WORD isInjure;//是否需要伤害计算
		};
		WORD status[7];
	};
	std::vector<SkillElement> _StatusElementList;
};
struct zSkillB : public zEntry
{
	bool has_needweapon(const WORD weapontype) const
	{
		std::vector<WORD>::const_iterator iter;
		if (weaponlist.empty()) return true;
		for(iter = weaponlist.begin(); iter != weaponlist.end(); iter++)
		{
			if (*iter == weapontype) return true;
		}
		return false;
	}

	bool set_weaponlist(const char *data)
	{
		weaponlist.clear(); 
		std::vector<std::string> v_fir;
		Zebra::stringtok(v_fir , data , ":");
#ifdef _DEBUGLOG
		Zebra::logger->debug("需要武器[%s]size=[%d]" , data, v_fir.size());
#endif 
		for(std::vector<std::string>::iterator iter = v_fir.begin() ; iter != v_fir.end() ; iter++)
		{
			WORD weaponkind = (WORD)atoi(iter->c_str());
			weaponlist.push_back(weaponkind);
		}
		return true;
	}

	bool set_skillState(const char *data)
	{
		skillStatus.clear(); 
		std::vector<std::string> v_fir;
		Zebra::stringtok(v_fir , data , ".");
		for(std::vector<std::string>::iterator iter = v_fir.begin() ; iter != v_fir.end() ; iter++)
		{
				//Zebra::logger->debug("%s" , iter->c_str());
			std::vector<std::string> v_sec;
			Zebra::stringtok(v_sec , iter->c_str() , ":");
			/*
			if(v_sec.size() != 2)
			{
				return false;
			}
			// */
			SkillStatus status;
			std::vector<std::string>::iterator iter_1 = v_sec.begin() ;
			std::vector<std::string> v_thi;
			Zebra::stringtok(v_thi , iter_1->c_str() , "-");
			if(v_thi.size() != 7)
			{
				//Zebra::logger->debug("操作!=7");
				continue;
				//return false;
			}
			std::vector<std::string>::iterator iter_2 = v_thi.begin() ;
			for(int i = 0 ; i < 7 ; i ++)
			{
				status.status[i] = (WORD)atoi(iter_2->c_str());
				//Zebra::logger->debug("status.status[%ld]=%ld" , i , status.status[i]);
				iter_2 ++;
			}
			iter_1 ++;
			if(iter_1 == v_sec.end())
			{
				//Zebra::logger->debug("空操作");
				skillStatus.push_back(status);
				continue;
			}
			std::vector<std::string> v_fou;
			Zebra::stringtok(v_fou , iter_1->c_str() , ";");
			std::vector<std::string>::iterator iter_3 = v_fou.begin() ;
			for( ; iter_3 != v_fou.end() ; iter_3 ++)
			{
				std::vector<std::string> v_fiv;
				Zebra::stringtok(v_fiv , iter_3->c_str() , "-");
				if(v_fiv.size() != 5)
				{
					//Zebra::logger->debug("元素个数不对");
					continue;
					//return false;
				}
				std::vector<std::string>::iterator iter_4 = v_fiv.begin() ;
				SkillElement element;
				for(int i = 0 ; i < 5 ; i ++)
				{
					element.element[i] = (SDWORD)atoi(iter_4->c_str()); //突破技能属性SDWORD
					//Zebra::logger->debug("element.element[%u]=%u" , i , element.element[i]);
					iter_4 ++;
				}
				status._StatusElementList.push_back(element);
			}
			skillStatus.push_back(status);
		}
		return true;
	}
	DWORD	skillid;						//技能ID
	DWORD	level;							//技能等级
	DWORD	kind;							//技能系别
	DWORD	subkind;						//技能树别
	DWORD	needpoint;						//需要本线技能点数
	DWORD	preskill1;						//前提技能1
	DWORD	preskilllevel1;					//前提技能级别1
	DWORD	preskill2;						//前提技能2
	DWORD	preskilllevel2;					//前提技能级别2
	DWORD	preskill3;						//前提技能3
	DWORD	preskilllevel3;					//前提技能级别3
	DWORD	dtime;							//间隔时间
	DWORD	usetype;						//攻击方式
	DWORD	ride;							//可否骑马使用
	DWORD	useBook;						//需要物品
	DWORD	spcost;							//消耗体力值
	DWORD	mpcost;							//消耗法术值
	DWORD	hpcost;							//消耗生命值
	DWORD	damnum;							//伤害加成
	DWORD	objcost;						//消耗物品类型
	DWORD	objnum;							//消耗物品数量
	std::vector<SkillStatus> skillStatus;	//效果
	std::vector<WORD> weaponlist;			//武器列表
	


	void fill(const SkillBase &data)
	{
		id=skill_hash(data.dwField0 , data.dwField2);
		skillid=data.dwField0;								//技能ID
		strncpy(name , data.strField1 , MAX_NAMESIZE);
		level			= data.dwField2;					//技能等级
		kind			= data.dwField3;					//技能系别
		subkind			= data.dwField4;					//技能树别
		needpoint		= data.dwField5;					//需要本线技能点数
		preskill1		= data.dwField6;					//前提技能1
		preskilllevel1	= data.dwField7;					//前提技能级别1
		preskill2		= data.dwField8;					//前提技能2
		preskilllevel2	= data.dwField9;					//前提技能级别2
		preskill3		= data.dwField10;					//前提技能3
		preskilllevel3	= data.dwField11;					//前提技能级别3
		dtime			= data.dwField12;					//间隔时间
		usetype			= data.dwField13;					//攻击方式
		ride			= data.dwField14;					//可否骑马使用
		useBook			= data.dwField15;					//学习需要物品
		set_weaponlist(data.strField16);					//需要武器
		spcost			= data.dwField17;					//消耗体力值
		mpcost			= data.dwField18;					//消耗法术值
		hpcost			= data.dwField19;					//消耗生命值
		damnum			= data.dwField20;					//伤害加成
		set_skillState(data.strField21);
		objcost			= data.dwField22;					//消耗物品类型
		objnum			= data.dwField23;					//消耗物品数量

#ifdef _DEBUGLOG
		Zebra::logger->debug("skillid=%ld:", skillid);
		for(std::vector<SkillStatus>::iterator iter = skillStatus.begin() ; iter != skillStatus.end() ; iter ++)
		{
			Zebra::logger->debug("id=%u , target=%u , center=%u , range=%u , mode=%u , clear=%u:",iter->id , iter->target , iter->center , iter->range , iter->mode , iter->clear); 
			for(std::vector<SkillElement>::iterator iter_1 = iter->_StatusElementList.begin() ; iter_1 != iter->_StatusElementList.end() ; iter_1++)
			{
				Zebra::logger->debug("id=%u , percent=%u , value=%u , time=%u , state=%u",iter_1->id , iter_1->percent , iter_1->value , iter_1->time , iter_1->state);
				
			}
		}
#endif
	}


	zSkillB() : zEntry()
	{
		id = 0;
		skillid = 0;
		bzero(name , sizeof(name));				//说明
		level			= 0;					//技能等级
		kind			= 0;					//技能系别
		subkind			= 0;					//技能树别
		needpoint		= 0;					//需要本线技能点数
		preskill1		= 0;					//前提技能1
		preskilllevel1	= 0;					//前提技能级别1
		preskill2		= 0;					//前提技能2
		preskilllevel2	= 0;					//前提技能级别2
		preskill3		= 0;					//前提技能3
		preskilllevel3	= 0;					//前提技能级别3
		dtime			= 0;					//间隔时间
		usetype			= 0;					//攻击方式
		ride			= 0;					//可否骑马使用
		useBook			= 0;					//需要物品
		spcost			= 0;					//消耗体力值
		mpcost			= 0;					//消耗法术值
		hpcost			= 0;					//消耗生命值
		damnum			= 0;					//伤害加成
		objcost			= 0;					//消耗物品类型
		objnum			= 0;					//消耗物品数量
	}

};

struct LiveSkillBase{

	const DWORD getUniqueID() const
	{
		return ((0xffff & dwField11) << 16) | (0xffff & dwField0);
	}

	DWORD	dwField0;		// 技能ID
	char	strField1[64];		// 技能名称
	DWORD	dwField2;		// 需要工具
	DWORD	dwField3;		// 初始技能
	DWORD	dwField4;		// 对应图素
	DWORD	dwField5;		// 类别
	DWORD	dwField6;		// 技能升级经验
	DWORD	dwField7;		// 可否升级
	DWORD	dwField8;		// 进阶技能
	DWORD	dwField9;		// 前提技能ID
	DWORD	dwField10;		// 所需前提技能等级
	DWORD	dwField11;		// 技能等级
	char	strField12[32];		// 技能称号
	char	strField13[256];		// 获得物品
};

struct zLiveSkillB : public zEntry
{
	enum {
		MAX_EXP_BONUS = 30,
		MIN_POINT_BONUS = 1,
		MAX_POINT_BONUS = 3,
		WORKING_TIME = 6,
		MAX_LEVEL = 30,
	};
			
	//DWORD skill_id; //技能标识
	//DWORD level; //技能等级
	//WORD should be enough
	WORD skill_id; //技能标识
	WORD level; //技能等级
	DWORD point; //升级所需技能点
	DWORD weapon_kind; //武器种类
	//std::string name; //技能名称
	std::string title; //称号
	bool orig; //初始技能
	bool upgrade; //能否升级
	DWORD kind; //技能类别
	DWORD basic_skill_id; //前提技能id
	DWORD basic_skill_level; //前提技能等级]
	DWORD up_skill_id; //进阶技能id
	DWORD map_kind;
	
	class ITEM 
	{
	public:
		DWORD item; //获得物品
		DWORD odds;	//几率
		DWORD min_number; //最小数量
		DWORD max_number; //最大数量

		ITEM( const std::string& odds_, const std::string& item_, const std::string& number_) : item(atoi(item_.c_str())), odds(atoi(odds_.c_str())), min_number(0), max_number(0)
		{
			std::string::size_type pos = 0;
			if  ( (pos = number_.find("-")) != std::string::npos ) {
				
				min_number = atoi(number_.substr(0, pos).c_str());
				max_number = atoi(number_.substr(pos+strlen("-")).c_str());
			}
			//if (item) Zebra::logger->debug("劳动获得物品数据:ID(%d), 几率(%d), 个数(%d-%d)", item, odds, min_number, max_number);
		}
	}; 
		
	typedef std::vector<ITEM> ITEMS;
	ITEMS items;
	
	BYTE min_point_bonus; //最小增加技能点
	BYTE max_point_bonus; //最大增加技能点
	BYTE exp_bonus; //奖励经验
	BYTE max_level; //最大等级

	zLiveSkillB() : zEntry(), skill_id(0), level(0), point(0), weapon_kind(0), /*name("未知"),*/ title(""), orig(false), upgrade(false),
					kind(1), basic_skill_id(0), basic_skill_level(0), up_skill_id(0), map_kind(0), 
					min_point_bonus(MIN_POINT_BONUS), max_point_bonus(MAX_POINT_BONUS), exp_bonus(MAX_EXP_BONUS), 
					max_level(MAX_LEVEL)
	{

	}
		
	void fill(const LiveSkillBase& base)
	{
		skill_id = 0xffff & base.dwField0;
		//name = base.strField1;
		weapon_kind = base.dwField2;
		orig = (base.dwField3==1)?true:false;
		map_kind = base.dwField4;
		kind = base.dwField5;
		point = base.dwField6;
		upgrade = (base.dwField7==1)?true:false;
		up_skill_id = base.dwField8;
		basic_skill_id = base.dwField9;
		basic_skill_level = base.dwField10;
		level = 0xffff & base.dwField11;
		strncpy(name, base.strField1, MAX_NAMESIZE);
		title = base.strField12;
		init_items(base.strField13);
		
		id = (level << 16) | skill_id;
	}
	
	void init_items(const std::string& item_list)
	{
		items.clear();
		Misc::Split<Misc::Parse3> p;
		p(item_list, items, ";", ":");

	}

};

//------------------------------------
// SoulStoneBase
//------------------------------------
struct SoulStoneBase{
	const DWORD getUniqueID() const
	{
		return dwField2;
	}
	
	DWORD	dwField0;		// 编号
	char	strField1[32];		// 名称
	DWORD	dwField2;		// 品质
	char	strField3[16];		// x%吸收生命值y
	char	strField4[16];		// x%吸收法术值y
	char	strField5[16];		// 转换x%生命值为法术值减少
	char	strField6[16];		// 增加银子掉落x%
	char	strField7[16];		// x%双倍经验
	char	strField8[16];		// 增加掉宝率x%
	char	strField9[16];		// 抗毒增加
	char	strField10[16];		// 抗麻痹增加
	char	strField11[16];		// 抗眩晕增加
	char	strField12[16];		// 抗噬魔增加
	char	strField13[16];		// 抗噬力增加
	char	strField14[16];		// 抗混乱增加
	char	strField15[16];		// 抗冰冻增加
	char	strField16[16];		// 抗石化增加
	char	strField17[16];		// 抗失明增加
	char	strField18[16];		// 抗定身增加
	char	strField19[16];		// 抗减速增加
	char	strField20[16];		// 抗诱惑增加
	char	strField21[16];		// 中毒增加
	char	strField22[16];		// 麻痹增加
	char	strField23[16];		// 眩晕增加
	char	strField24[16];		// 噬魔增加
	char	strField25[16];		// 噬力增加
	char	strField26[16];		// 混乱增加
	char	strField27[16];		// 冰冻增加
	char	strField28[16];		// 石化增加
	char	strField29[16];		// 失明增加
	char	strField30[16];		// 定身增加
	char	strField31[16];		// 减速增加
	char	strField32[16];		// 诱惑增加
	DWORD	dwField33;		// 需求等级
	char	strField34[16];		// 力量
	char	strField35[16];		// 智力
	char	strField36[16];		// 敏捷
	char	strField37[16];		// 精神
	char	strField38[16];		// 体质 	
};//导出 SoulStoneBase 成功，共 40 条记录



struct zSoulStoneB : public zEntry
{
	//DWORD id;
	//std::string name;
	
	struct Value
	{ 
		rangeValue odds; 
		rangeValue effect; 
	} hpleech, mpleech; ////x%吸收生命值y, x%吸收法术值y
	
	rangeValue hptomp; //转换生命值为法术值x％

	rangeValue incgold; //增加银子掉落x%
	rangeValue doublexp; //x%双倍经验		
	rangeValue mf; //增加掉宝率x%
	
	rangeValue poisondef; //抗毒增加
	rangeValue lulldef; //抗麻痹增加
	rangeValue reeldef; //抗眩晕增加
	rangeValue evildef; //抗噬魔增加
	rangeValue bitedef; //抗噬力增加
	rangeValue chaosdef; //抗混乱增加
	rangeValue colddef; //抗冰冻增加
	rangeValue petrifydef; //抗石化增加
	rangeValue blinddef; //抗失明增加
	rangeValue stabledef; //抗定身增加
	rangeValue slowdef; //抗减速增加
	rangeValue luredef; //抗诱惑增加

	rangeValue poison; //中毒增加
	rangeValue lull; //麻痹增加
	rangeValue reel; //眩晕增加
	rangeValue evil; //噬魔增加
	rangeValue bite; //噬力增加
	rangeValue chaos; //混乱增加
	rangeValue cold; //冰冻增加
	rangeValue petrify; //石化增加
	rangeValue blind; //失明增加
	rangeValue stable; //定身增加
	rangeValue slow; //减速增加
	rangeValue lure; //诱惑增加
	
	WORD level; 	

	rangeValue xstr;		// 力量-魂魄
	rangeValue xinte;		// 智力-魂魄
	rangeValue xdex;		// 敏捷-魂魄
	rangeValue xspi;		// 精神-魂魄
	rangeValue xcon;		// 体质-魂魄

//	rangeValue str;			// 力量
//	rangeValue inte;		// 智力
//	rangeValue dex;			// 敏捷
//	rangeValue spi;			// 精神
//	rangeValue con;			// 体质
	
	zSoulStoneB() : zEntry()
	{
		
	}
	
	void fill(const SoulStoneBase& base)
	{
		id = base.dwField2;
		strncpy(name, base.strField1, MAX_NAMESIZE);
		
		init_value(base.strField3, hpleech);
		init_value(base.strField4, mpleech);
		
		fillRangeValue(base.strField5, hptomp);
		fillRangeValue(base.strField6, incgold);
		fillRangeValue(base.strField7, doublexp);
		fillRangeValue(base.strField8, mf);

		fillRangeValue(base.strField9, poisondef);
		fillRangeValue(base.strField10, lulldef);
		fillRangeValue(base.strField11, reeldef);	
		fillRangeValue(base.strField12, evildef);
		fillRangeValue(base.strField13, bitedef);
		fillRangeValue(base.strField14, chaosdef);
		fillRangeValue(base.strField15, colddef);
		fillRangeValue(base.strField16, petrifydef);
		fillRangeValue(base.strField17, blinddef);
		fillRangeValue(base.strField18, stabledef);
		fillRangeValue(base.strField19, slowdef);
		fillRangeValue(base.strField20, luredef);
				
		fillRangeValue(base.strField21, poison);
		fillRangeValue(base.strField22, lull);
		fillRangeValue(base.strField23, reel);	
		fillRangeValue(base.strField24, evil);
		fillRangeValue(base.strField25, bite);
		fillRangeValue(base.strField26, chaos);
		fillRangeValue(base.strField27, cold);
		fillRangeValue(base.strField28, petrify);
		fillRangeValue(base.strField29, blind);
		fillRangeValue(base.strField30, stable);
		fillRangeValue(base.strField31, slow);
		fillRangeValue(base.strField32, lure);

		level = base.dwField33;

		fillRangeValue(base.strField34, xstr);
		fillRangeValue(base.strField35, xinte);
		fillRangeValue(base.strField36, xdex);
		fillRangeValue(base.strField37, xspi);
		fillRangeValue(base.strField38, xcon);

//		fillRangeValue(base.strField34, str);
//		fillRangeValue(base.strField35, inte);
//		fillRangeValue(base.strField36, dex);
//		fillRangeValue(base.strField37, spi);
//		fillRangeValue(base.strField38, con);
	}
	
	void init_value(const std::string& src, Value& value)
	{
		std::string::size_type pos = 0;
		if  ( (pos = src.find(';')) != std::string::npos ) {
			fillRangeValue(src.substr(0, pos).c_str(), value.odds);
			fillRangeValue(src.substr(pos+1).c_str(), value.effect);
		}
	}
	
};



//------------------------------------
// HairStyle
//------------------------------------
struct HairStyle{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	char	strField1[32];		// 名称
	DWORD	dwField2;		// 性别
	DWORD	dwField3;		// 动作发型图片
	DWORD	dwField4;		// 纸娃娃发型图片
	DWORD	dwField5;		// 费用
};//导出 HairStyle 成功，共 10 条记录
struct zHairStyleB : public zEntry
{
	DWORD cost;
	zHairStyleB():zEntry()
	{
		cost=0;
	}
	void fill(const HairStyle& base)
	{
		id = base.dwField0;
		strncpy(name, base.strField1, MAX_NAMESIZE);
		cost=base.dwField5;
	}
};

//------------------------------------
// HairColour
//------------------------------------
struct HairColour{
	const DWORD getUniqueID() const
	{
		return dwField2 & 0x00FFFFFF;//发色做
	}
	DWORD	dwField0;		// 编号
	char	strField1[32];		// 名称
	DWORD	dwField2;		// 颜色
	DWORD	dwField3;		// 费用
};//导出 HairColour 成功，共 4 条记录
struct zHairColourB : public zEntry
{
	DWORD color;
	DWORD cost;
	zHairColourB() : zEntry()
	{
		color=0;
		cost=0;
	}
	void fill(const HairColour& base)
	{
		id = base.dwField2 & 0x00FFFFFF;//发色做
		strncpy(name, base.strField1, MAX_NAMESIZE);
		color=base.dwField2;
		cost=base.dwField3;
	}
};
//------------------------------------
// HeadList
//------------------------------------
struct HeadList{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// 编号
	char	strField1[16];		// 头像名
	DWORD	dwField2;		// 性别
	DWORD	dwField3;		// 头像编号
	DWORD	dwField4;		// 费用
};//导出 HeadList 成功，共 10 条记录
struct zHeadListB : public zEntry
{
	DWORD sex;
	DWORD icon;
	DWORD cost;
	zHeadListB() : zEntry()
	{
		sex=0;
		icon=0;
		cost=0;
	}
	void fill(const HeadList& base)
	{
		id = base.dwField0;
		strncpy(name, base.strField1, MAX_NAMESIZE);
		sex=base.dwField2;
		icon=base.dwField3;
		cost=base.dwField4;
	}
};


//------------------------------------
//// PetBase
////------------------------------------
struct PetBase{
	const DWORD getUniqueID() const
	{
		return dwField0;
	}
	DWORD	dwField0;		// id
	DWORD	dwField1;		// 等级
	DWORD	dwField2;		// 类型
	DWORD	dwField3;		// 经验值
	DWORD	dwField4;		// 生命值
	DWORD	dwField5;		// 物攻下限
	DWORD	dwField6;		// 物攻上限
	DWORD	dwField7;		// 魔攻下限
	DWORD	dwField8;		// 魔攻上限
	DWORD	dwField9;		// 物防
	DWORD	dwField10;		// 魔防
	DWORD	dwField11;		// 重击
	DWORD	dwField12;		// 力量
	DWORD	dwField13;		// 智力
	DWORD	dwField14;		// 敏捷
	DWORD	dwField15;		// 精神
	DWORD	dwField16;		// 体质		
};

struct zPetB : public zEntry
{
	DWORD base_id;		// id
	DWORD lv;       	// 等级
	DWORD type;     	// 类型
	DWORD exp;      	// 经验值
	DWORD hp;       	// 生命值
	DWORD atk;      	// 物攻下限
	DWORD maxatk;   	// 物攻上限
	DWORD matk;     	// 魔攻下限
	DWORD maxmatk;  	// 魔攻上限
	DWORD def;      	// 物防
	DWORD mdef;     	// 魔防
	DWORD cri;      	// 重击
	DWORD str;      	// 力量
	DWORD intel;    	// 智力
	DWORD agi;      	// 敏捷
	DWORD men;      	// 精神
	DWORD vit;		// 体质		

	zPetB() : zEntry()
	{
		base_id	= 0;	
		lv	= 0;       
		type	= 0;     
		exp	= 0;      
		hp	= 0;       
		atk	= 0;      
		maxatk	= 0;   
		matk	= 0;     
		maxmatk	= 0;  
		def	= 0;      
		mdef	= 0;     
		cri	= 0;      
		str	= 0;      
		intel	= 0;    
		agi	= 0;      
		men	= 0;      
		vit	= 0;	
	}
	void fill(PetBase &base)
	{
		base_id	= base.dwField0;	
		id	= base.dwField0;	
		char buf[32];
		sprintf(buf, "%d", base.dwField0);
		strncpy(name, buf , MAX_NAMESIZE);
		lv	= base.dwField1;	
		type	= base.dwField2;	
		exp	= base.dwField3;	
		hp	= base.dwField4;	
		atk	= base.dwField5;	
		maxatk	= base.dwField6;		
		matk	= base.dwField7;	
		maxmatk	= base.dwField8;		
		def	= base.dwField9;	
		mdef	= base.dwField10;	
		cri	= base.dwField11;	
		str	= base.dwField12;	
		intel	= base.dwField13;	
		agi	= base.dwField14;	
		men	= base.dwField15;	
		vit	= base.dwField16;			
	}
};
//------------------------------------
// CountryMaterial
//------------------------------------
struct CountryMaterial{
	const DWORD getUniqueID() const
	{
		return dwField1+dwField3;
	}
	
	DWORD	dwField0;		// 编号
	DWORD	dwField1;		// 物品ID
	DWORD	dwField2;		// 材料类型
	DWORD	dwField3;		// 物品类别
};
struct zCountryMaterialB : public zEntry
{
	DWORD dwObjectID;
	DWORD dwMaterialKind;
	DWORD dwKind;
	
	zCountryMaterialB() : zEntry()
	{
		dwObjectID = 0;
		dwMaterialKind = 0;
		dwKind = 0;
	}
	void fill(const CountryMaterial& base)
	{
		id = base.dwField1+base.dwField3;
		
		dwObjectID = base.dwField1;
		dwMaterialKind = base.dwField2;
		dwKind = base.dwField3;
	}
};

#pragma pack()

#endif
