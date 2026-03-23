/**
 * \file
 * \version	$Id: RebuildObject.h  $
 * \author	
 * \date		2005-04-18
 * \brief		新物品合成，打造，升星，打孔，镶嵌系统
 * 
 */
 
#ifndef __REBUILD_OBJECT__
#define __REBUILD_OBJECT__


#include <map>

#include "zType.h"
#include "zMisc.h"
#include "zDatabase.h"
#include "zObject.h"
#include "SceneUser.h"

namespace Cmd
{
struct stPropertyUserCmd;
struct stFoundItemPropertyUserCmd; //soke 打造指令
}

class zObjectB;

/**
 * \brief 物品改造基类
 *
 * 封装了几个常用的操作
 *
 */	
class Base
{
public:	
	enum RebuildType
    {
		COMPOSE         = 1,
		MAKE            = 2,
		UPGRADE         = 3,
		HOLE            = 4,
		ENCHANCE        = 5,
		DECOMPOSE       = 6,
		MODIFY = 7,     // 改造
		MAKEZS = 8,     // 祝福改造
		MAKEZQ = 9,     // 马匹祝福改造
		MAKELV = 10,    // 装备升级

		EQUIP_COMPOSE11 = 11,   //装备祝福
		EQUIP_COMPOSE12 = 12,   //斗魂升级
		EQUIP_COMPOSE13 = 13,   //神石镶嵌
		EQUIP_COMPOSE14 = 14,   //装备栏激活
		EQUIP_COMPOSE15 = 15,   //装备栏进阶
		EQUIP_COMPOSE16 = 16,   //神器强化
		EQUIP_COMPOSE17 = 17,   //龙槽激活
		EQUIP_COMPOSE18 = 18,   //龙槽解封
		EQUIP_COMPOSE19 = 19,   //龙星升级
		EQUIP_COMPOSE20 = 20,   //魂魄清除
		EQUIP_COMPOSE21 = 21,   //装备升级
		EQUIP_COMPOSE22 = 22,   //马牌精炼
		EQUIP_COMPOSE23 = 23,   //龙王套装镶嵌
		EQUIP_COMPOSE24 = 24,   //逆天养成进阶
		EQUIP_COMPOSE25 = 25,   //斗战圣佛进阶
		EQUIP_COMPOSE26 = 26,   //暗影石镶嵌
		EQUIP_COMPOSE27 = 27,   //龙凤吟镶嵌
		EQUIP_COMPOSE28 = 28,   //轰天宝石镶嵌
		EQUIP_COMPOSE29 = 29,   //王者升级

		EQUIP_COMPOSE51 = 51,   //勋章升级
		EQUIP_COMPOSE52 = 52,   //寒冰升级
		EQUIP_COMPOSE53 = 53,   //翅膀升级
		EQUIP_COMPOSE54 = 54,   //神兵升级
		EQUIP_COMPOSE55 = 55,   //神兽升级
		EQUIP_COMPOSE56 = 56,   //璀璨镶嵌
		EQUIP_COMPOSE57 = 57,   //神兽宝石镶嵌
		EQUIP_COMPOSE58 = 58,   //尚方宝剑升级
		EQUIP_COMPOSE59 = 59,   //紫金龙袍升级		
		EQUIP_COMPOSE60 = 60,   //十二星宿进阶
		EQUIP_COMPOSE61 = 61,   //神斧
		EQUIP_COMPOSE62 = 62,   //黑曜
		EQUIP_COMPOSE63 = 63,   //精金
		EQUIP_COMPOSE64 = 64,   //金箍咒强化
		EQUIP_COMPOSE65 = 65,   //金箍咒字纹
		EQUIP_COMPOSE66 = 66,   //金箍咒淬炼
		EQUIP_COMPOSE67 = 67,   //寒冰神炼
		EQUIP_COMPOSE68 = 68,   //寒冰字纹
		EQUIP_COMPOSE69 = 69,   //寒冰纹章
		

        EQUIP_COMPOSE101 = 101,   //图鉴进阶
		EQUIP_COMPOSE102 = 102,   //百兽图鉴签名	

		EQUIP_COMPOSE103 = 103,   //法宝进阶
		EQUIP_COMPOSE104 = 104,   //符石进阶		

		EQUIP_COMPOSE105 = 105,   //十二星宿进阶		
		EQUIP_COMPOSE106 = 106,   //元神进阶
		EQUIP_COMPOSE107 = 107,   //元神装备进阶		
		EQUIP_COMPOSE108 = 108,   //元神宝石镶嵌	
	};
	
	static bool check_npc(SceneUser& user, zObjectB* base, int action);
	
	static bool check_space(SceneUser& user, DWORD w, DWORD h);

	static bool remove_object(SceneUser& user, zObject* ob);

    ///删除物品加日志
    static bool removeObject(SceneUser& user, zObject* ob, const char *info);
	
	static bool add_object(SceneUser& user, zObject* ob, bool add = true);

	static bool response(SceneUser& user, int status, RebuildType type);
		
	static void refresh_pack(SceneUser& user, ObjectPack* pack);
    //回复合成等操作的结果
    static bool reply(SceneUser& user, int status, RebuildType type, const char *pattern, ...);
};

/**
 * \brief 孔
 *
 * 封装了对与孔相关的处理函数
 *
 */	
class Hole
{
public:
	enum 
	{
		INVALID_INDEX = 6, //soke 孔的最大数量
		INVALID_NUM = 7,
	};
	
	
	static bool can_hole(zObject * ob); //soke 是否能打孔
	
	static int get_empty_hole(zObject* ob); //soke 取得物品上可用孔 的数量
	
	static int get_hole_num(zObject* ob); //soke 取得物品上所有孔 的数量

	static int add_hole_num(zObject* ob, int num); //soke 在物品上增加孔
	
	static bool add_hole(zObject* ob, int index); //soke 在物品上指定位置增加一个孔

	static bool put_hole(zObject* ob, int index, int id); //soke 在物品上指定孔放置一个魂魄

};

/**
 * \brief 魂魄石
 *
 * 封装了对魂魄石的处理函数，包括镶嵌及合成等
 *
 */	
class SoulStone
{
public:
	static zObject* compose(SceneUser& user, zObject* first, zObject* second, int odds);

	static bool enchance(SceneUser& user, zObject* dest, zObject* src);

	static int id(DWORD trait);
	
	static bool assign(zObject* ob, int monster);
	
private:
	static bool do_enchance(zObject* dest, zObject* src);
	
	static bool do_compose(zObject* first, zObject* second, zObject* dest);
	
	/**     
	 * \brief 魂魄属性计算
	 *
	 * \param first: 第一块魂魄石属性
	 * \param second: 第二块魂魄石属性
	 * \param result: 计算结果
	 * \param level:魂魄石等级
	 * \return 无
	 */	
	template<typename T>
	static void additive(T first, T second, T& result, int level)
	{
		if (first && second) {
			//result = std::max(first, second) + 0.1*std::min(first, second)	
			T max = first, min = second, grade = level & 0xff;
			if (max < min) {
				max = second;
				min = first;
				grade = level >> 8;
			}
			
			result =  max + static_cast<T>(min*0.1*grade);	
		}else {
			result = first + second;
		}
		
	}

	static const int _ids[];
};

/**
 * \brief 升星
 *
 * 实现了物品升星功能
 *
 */	
class Upgrade
{
public:
	static bool upgrade(SceneUser& user, zObject* ob, int extra_odds);
	static bool purple_upgrade(SceneUser& user, zObject* ob, int extra_odds);
	static bool hsf_upgrade(SceneUser& user, zObject* ob, int extra_odds);
	static bool dh_upgrade(SceneUser& user, zObject* ob, int extra_odds);
	static bool bt_upgrade(SceneUser& user, zObject* ob, int extra_odds);
	static bool dtrough_upgrade(SceneUser& user, zObject* ob, int extra_odds);
	static bool drastar_upgrade(SceneUser& user, zObject* ob, int extra_odds);
	static bool medal_upgrade(SceneUser& user, zObject* ob, int extra_odds);
	static bool hb_upgrade(SceneUser& user, zObject* ob, int extra_odds);
	static bool fabao_upgrade(SceneUser& user, zObject* ob, int extra_odds);//法宝升级	
	static bool YUANSHEN_upgrade(SceneUser& user, zObject* ob, int extra_odds);//元神升级	
	static bool YSBS_upgrade(SceneUser& user, zObject* ob, int extra_odds);//元神升级	
	static bool SEXX_upgrade(SceneUser& user, zObject* ob, int extra_odds);//十二星宿升级	
    static bool shenqi_upgrade(SceneUser& user, zObject* ob, int extra_odds); //神器进阶
	static bool refine_upgrade(SceneUser& user, zObject* ob, int extra_odds); //马牌精炼
	static bool bright_upgrade(SceneUser& user, zObject* ob, int extra_odds); //璀璨镶嵌
	static bool handbook_upgrade(SceneUser& user, zObject* ob, int extra_odds); //百兽图鉴进阶	
	static bool longwang_upgrade(SceneUser& user, zObject* ob, int extra_odds); //神龙镶嵌	
	static bool nitian_upgrade(SceneUser& user, zObject* ob, int extra_odds); //逆天进阶	
	static bool douzhansf_upgrade(SceneUser& user, zObject* ob, int extra_odds); //神魂镶嵌进阶	
	static bool aystone_upgrade(SceneUser& user, zObject* ob, int extra_odds); //暗影宝石镶嵌	
	static bool angel_upgrade(SceneUser& user, zObject* ob, int extra_odds);
	static bool magical_upgrade(SceneUser& user, zObject* ob, int extra_odds);
	static bool mythical_upgrade(SceneUser& user, zObject* ob, int extra_odds);
	static bool equip(SceneUser& user, zObject* ob, int extra_odds); //装备升级
	static bool shangfang_upgrade(SceneUser& user, zObject* ob, int extra_odds); //尚方宝剑升级
	static bool zijinlp_upgrade(SceneUser& user, zObject* ob, int extra_odds); //紫金龙袍升级
	static bool jgzqh_upgrade(SceneUser& user, zObject* ob, int extra_odds);   //金箍咒强化	
	static bool jgzzw_upgrade(SceneUser& user, zObject* ob, int extra_odds);   //金箍咒字纹
	static bool jgzcl_upgrade(SceneUser& user, zObject* ob, int extra_odds);   //金箍咒淬炼
	static bool hbsl_upgrade(SceneUser& user, zObject* ob, int extra_odds);   //寒冰神炼
	static bool hbzw_upgrade(SceneUser& user, zObject* ob, int extra_odds);   //寒冰字纹
	static bool hbwz_upgrade(SceneUser& user, zObject* ob, int extra_odds);   //寒冰纹章
	//龙凤吟
    static bool longfengying_upgrade(SceneUser& user, zObject* ob, int extra_odds);
	//神斧
	static bool xuanhua_upgrade(SceneUser& user, zObject* ob, int extra_odds);
	//黑曜
	static bool heiyao_upgrade(SceneUser& user, zObject* ob, int extra_odds);
	//精金
	static bool jingjin_upgrade(SceneUser& user, zObject* ob, int extra_odds);
private:	
	static bool do_handbook_upgrade(zObject* ob, zUhandbookObjectB* base);	  //百兽图鉴
	static bool do_handbook_downgrade(zObject* ob, zUhandbookObjectB* base);   //百兽图鉴
	static bool do_longwanglv_upgrade(zObject* ob, zUlongwanglvObjectB* base);	 
	static bool do_longwanglv_downgrade(zObject* ob, zUlongwanglvObjectB* base);   
    static bool do_nitianlv_upgrade(zObject* ob, zUnitianlvObjectB* base);	  
	static bool do_nitianlv_downgrade(zObject* ob, zUnitianlvObjectB* base);  
    static bool do_douzhansflv_upgrade(zObject* ob, zUdouzhansflvObjectB* base);	  
	static bool do_douzhansflv_downgrade(zObject* ob, zUdouzhansflvObjectB* base);  
    static bool do_aystonelv_upgrade(zObject* ob, zUaystonelvObjectB* base);	  
	static bool do_aystonelv_downgrade(zObject* ob, zUaystonelvObjectB* base);   

	static bool do_shangfang_upgrade(zObject* ob, zUshangfanglvObjectB* base);	  
	static bool do_shangfang_downgrade(zObject* ob, zUshangfanglvObjectB* base); 
	static bool do_zijinlp_upgrade(zObject* ob, zUzijinlplvObjectB* base);	  
	static bool do_zijinlp_downgrade(zObject* ob, zUzijinlplvObjectB* base); 

	static bool do_upgrade(zObject* ob, zUpgradeObjectB* base);		
	static bool do_downgrade(zObject* ob, zUpgradeObjectB* base);
	static bool do_2upgrade(zObject* ob, zUpgradeObject2B* base);		
	static bool do_2downgrade(zObject* ob, zUpgradeObject2B* base);
	static bool pur_upgrade(zObject* ob, zUpgradePurpleB* base);		
	static bool pur_downgrade(zObject* ob, zUpgradePurpleB* base);
	static bool douhun_upgrade(zObject* ob, zUdouhunObjectB* base);		
	static bool douhun_downgrade(zObject* ob, zUdouhunObjectB* base);
	static bool butian_upgrade(zObject* ob, zUbutianObjectB* base);		
	static bool butian_downgrade(zObject* ob, zUbutianObjectB* base);
	static bool do_dtrough_upgrade(zObject* ob, zUdtroughObjectB* base);
	static bool do_dtrough_downgrade(zObject* ob, zUdtroughObjectB* base);
	static bool do_drastar_upgrade(zObject* ob, zUdrastarObjectB* base);	
	static bool do_drastar_downgrade(zObject* ob, zUdrastarObjectB* base);	
	static bool do_medal_upgrade(zObject* ob, zMedaltObjectB* base);		
	static bool do_medal_downgrade(zObject* ob, zMedaltObjectB* base);
	static bool hbql_upgrade(zObject* ob, zUhbqlObjectB* base);		
	static bool hbql_downgrade(zObject* ob, zUhbqlObjectB* base);
	static bool do_shenqi_upgrade(zObject* ob, zUshenqiObjectB* base);		
	static bool do_shenqi_downgrade(zObject* ob, zUshenqiObjectB* base);
	static bool do_fabao_upgrade(zObject* ob, zUfabaoObjectB* base);		
	static bool do_fabao_downgrade(zObject* ob, zUfabaoObjectB* base);
	static bool do_YUANSHEN_upgrade(zObject* ob, zUYUANSHENObjectB* base);		
	static bool do_YUANSHEN_downgrade(zObject* ob, zUYUANSHENObjectB* base);
	static bool do_YSBS_upgrade(zObject* ob, zUYSBSObjectB* base);		
	static bool do_YSBS_downgrade(zObject* ob, zUYSBSObjectB* base);
	static bool do_SEXX_upgrade(zObject* ob, zUSEXXObjectB* base);		
	static bool do_SEXX_downgrade(zObject* ob, zUSEXXObjectB* base);
	static bool do_refine_upgrade(zObject* ob, zUrefineObjectB* base);		
	static bool do_refine_downgrade(zObject* ob, zUrefineObjectB* base);
	static bool do_bright_upgrade(zObject* ob, zUbrightObjectB* base);		
	static bool do_bright_downgrade(zObject* ob, zUbrightObjectB* base);
	static bool do_angel_upgrade(zObject* ob, zUangelObjectB* base);		
	static bool do_angel_downgrade(zObject* ob, zUangelObjectB* base);
	static bool do_magical_upgrade(zObject* ob, zMagicalObjectB* base);		
	static bool do_magical_downgrade(zObject* ob, zMagicalObjectB* base);
	static bool do_mythical_upgrade(zObject* ob, zMythicalObjectB* base);		
	static bool do_mythical_downgrade(zObject* ob, zMythicalObjectB* base);
	static bool do_lanupgrade(zObject* ob, zAmuletObjectB* base);	
	static bool do_liliang(zObject* ob, zAmuletObjectB* base);	
	static bool do_liliang2(zObject* ob, zAmuletObjectB* base);	
	static bool do_liliang3(zObject* ob, zAmuletObjectB* base);	
	static bool do_liliang15(zObject* ob, zAmuletObjectB* base);
	static bool do_minjie(zObject* ob, zAmuletObjectB* base);	
	static bool do_minjie2(zObject* ob, zAmuletObjectB* base);	
	static bool do_minjie3(zObject* ob, zAmuletObjectB* base);	
	static bool do_minjie15(zObject* ob, zAmuletObjectB* base);
	static bool do_zhili(zObject* ob, zAmuletObjectB* base);	
	static bool do_zhili2(zObject* ob, zAmuletObjectB* base);	
	static bool do_zhili3(zObject* ob, zAmuletObjectB* base);	
	static bool do_zhili15(zObject* ob, zAmuletObjectB* base);
	static bool do_tizhi(zObject* ob, zAmuletObjectB* base);	
	static bool do_tizhi2(zObject* ob, zAmuletObjectB* base);	
	static bool do_tizhi3(zObject* ob, zAmuletObjectB* base);	
	static bool do_tizhi15(zObject* ob, zAmuletObjectB* base);
	static bool do_jingshen(zObject* ob, zAmuletObjectB* base);	
	static bool do_jingshen2(zObject* ob, zAmuletObjectB* base);	
	static bool do_jingshen3(zObject* ob, zAmuletObjectB* base);	
	static bool do_jingshen15(zObject* ob, zAmuletObjectB* base);
	static bool hsf_downgrade(zObject* ob, zAmuletObjectB* base);
	static bool do_equip(zObject* ob, zEquipObjectB* base);		
	static bool do_downequip(zObject* ob, zEquipObjectB* base);
	//龙凤吟
	static bool do_longfengying_upgrade(zObject* ob, zUlongfengyingObjectB* base);
	static bool do_longfengying_downgrade(zObject* ob, zUlongfengyingObjectB* base);
	//神斧
	static bool do_xuanhua_upgrade(zObject* ob, zUxuanhuaObjectB* base);
	static bool do_xuanhua_downgrade(zObject* ob, zUxuanhuaObjectB* base);
	//黑曜
	static bool do_heiyao_upgrade(zObject* ob, zUheiyaoObjectB* base);
	static bool do_heiyao_downgrade(zObject* ob, zUheiyaoObjectB* base);
	//精金
	static bool do_jingjin_upgrade(zObject* ob, zUjingjinObjectB* base);
	static bool do_jingjin_downgrade(zObject* ob, zUjingjinObjectB* base);
	static bool do_jgzqh_upgrade(zObject* ob, zUjgzqhObjectB* base);   //金箍咒强化
	static bool do_jgzqh_downgrade(zObject* ob, zUjgzqhObjectB* base); //金箍咒强化	
	static bool do_jgzzw_upgrade(zObject* ob, zUjgzzwObjectB* base);   //金箍咒字纹
	static bool do_jgzzw_downgrade(zObject* ob, zUjgzzwObjectB* base); //金箍咒字纹	
	static bool do_jgzcl_upgrade(zObject* ob, zUjgzclObjectB* base);   //金箍咒淬炼
	static bool do_jgzcl_downgrade(zObject* ob, zUjgzclObjectB* base); //金箍咒淬炼	
	static bool do_hbsl_upgrade(zObject* ob, zUhbslObjectB* base);   //寒冰神炼
	static bool do_hbsl_downgrade(zObject* ob, zUhbslObjectB* base); //寒冰神炼	
	static bool do_hbzw_upgrade(zObject* ob, zUhbzwObjectB* base);   //寒冰字纹
	static bool do_hbzw_downgrade(zObject* ob, zUhbzwObjectB* base); //寒冰字纹	
	static bool do_hbwz_upgrade(zObject* ob, zUhbwzObjectB* base);   //寒冰纹章
	static bool do_hbwz_downgrade(zObject* ob, zUhbwzObjectB* base); //寒冰纹章	
};

/**
 * \brief 物品分解
 *
 * 实现物品分解功能
 *
 */
class Decompose
{
public:	
	/**     
	 * \brief 构造函数
	 *
	 * 初始化相关变量
	 *
	 * param ob : 待分解物品
	 *
	 */   
	Decompose(zObject* ob) : _ob(ob)
	{ }
	
	bool bonus_items(SceneUser& user);
	bool bonus_exp(SceneUser& user);
	bool remove_from(SceneUser& user);	
	int gold() const;
		
private:
	
	int chance() const;
	int index() const;
	
	zObject* _ob;
	
	const static int _odds[];
	const static int _items[];
};

#define COMPUTE_R(x) additive(ob->data.x, bob->x);
#define COMPUTE_L(x) additive(ob->data.x, bob->x, property);

#define BONUS_SKILL std::vector<skillbonus>::iterator it = bob->skill.begin();  \
		bool must = false; \
		for ( ; it!=bob->skill.end(); ++it) { \
			int odds = odds_of_property(it->odds, property); \
			if (zMisc::selectByTenTh(odds) || must) { \
				int i = 0; \
				while ( i <3) {\
					if (ob->data.skill[i].id == 0 || ob->data.skill[i].id==it->id) {\
						ob->data.skill[i].id = it->id; \
						ob->data.skill[i].point += it->level; \
						break;\
					}else {\
						++i;\
					}\
					if (i>2 &&must) break;\
				}\
			} \
		}
//戒指,项链
#define BONUS_SKILL_RING {\
	if(!bob->skill.empty())\
	{\
		int index = zMisc::randBetween(0, bob->skill.size()-1 );\
				int i = 0; \
				while ( i <3) {\
					if (ob->data.skill[i].id == 0 || ob->data.skill[i].id==bob->skill[index].id) {\
						ob->data.skill[i].id = bob->skill[index].id; \
						ob->data.skill[i].point += bob->skill[index].level; \
						break;\
					}else {\
						++i;\
					}\
				}\
		}\
	}

#define BONUS_SKILLS int odds = odds_of_property(bob->skills.odds, property); \
		if (zMisc::selectByTenTh(odds)) { \
			ob->data.skills.id = bob->skills.id; \
			ob->data.skills.point = bob->skills.level; \
		}

/**
 * \brief 打造
 *
 * 实现了物品打造功能
 *
 */	
class EquipMaker
{
	public:
		EquipMaker(SceneUser* user);

		/**     
		 * \brief 析构函数
		 *
		 */     
		~EquipMaker() { }

		bool check_skill(SceneUser& user, zObjectB* ob);

		bool add_skill(SceneUser& user, zObjectB* ob);
		bool add_exp(SceneUser& user, DWORD exp);

		bool check_material(zObjectB* ob, const std::map<DWORD, DWORD>& list, bool is_resource = false);

		//	void pre_level_of_material(zObjectB* base);
		void pre_level_of_material(int id, int level);

		bool level_of_material(DWORD id, DWORD num, DWORD level, zObjectB* base);

		bool is_odds_gem(DWORD kind, DWORD id);

		zObject* make_material(zObjectB* base);

		zObject* make(SceneUser* user, zObjectB* base, int flag = 0);

		zObject* zsmake(SceneUser* user, zObjectB* base, int flag = 0);

		void assign(SceneUser* user, zObject* ob, zObjectB* base, bool drop =false , int flag = 0);
		void assign_zs(SceneUser* user, zObject* ob, zObjectB* base, bool drop =false , int flag = 0);
		void assign1(SceneUser* user, zObject* ob, zObjectB* base, bool drop =false , int flag = 0);
		void assign2(SceneUser* user, zObject* ob, zObjectB* base, bool drop =false , int flag = 0);
		void assign3(SceneUser* user, zObject* ob, zObjectB* base, bool drop =false , int flag = 0);
		void assign4(SceneUser* user, zObject* ob, zObjectB* base, bool drop =false , int flag = 0);

		void fix(zObject* ob);

		void bonus_hole(zObject* ob);

	
		//是否可以改造
		static bool can_modify(zObject * ob);

		//是否可以升级改造
		static bool can_makelv(zObject * ob);

		//soke 紫装改造属性
		int modify_attri16(zObject* ob,zObjectB *objbase);

		//soke 绿装改造属性
		int modify_attri(zObject* ob,zObjectB *objbase);

		//soke 紫装重新绑定
		int modify_rebind16(zObject* ob,zObjectB *objbase);

		//soke 绿装重新绑定
		int modify_rebind(zObject* ob,zObjectB *objbase);

		//soke 重新签名
		int modify_remaker(zObject* ob,SceneUser* user);

		//soke 添加签名
		int modify_addmaker(zObject* ob,SceneUser* user);

		//soke 紫装修改灵魂属性
		int modify_soula16(zObject* ob,zObjectB *objbase);

		//soke 绿装修改灵魂属性
		int modify_soula(zObject* ob,zObjectB *objbase);

		//  修改五行属性
		int modify_fivetype(zObject* ob);

		//soke 紫装修改灵魂属性
		int modify_mdstwokilla16(zObject* ob,zObjectB *objbase);

		//soke 修改第二灵魂属性
		int modify_soullinemd16(zObject* ob,zObjectB *objbase);

		//soke 绿装保留灵魂锁链改造
		int modify_soullinemd(zObject* ob,zObjectB *objbase);

		//soke 紫装技能属性
		int modify_mdskilla16(zObject* ob,zObjectB *objbase);

		//soke 绿装技能属性
		int modify_mdskilla(zObject* ob,zObjectB *objbase);

		//soke 紫装马匹装备重新锻造
		int modify_remakedz16(zObject* ob,zObjectB *objbase);

		//soke 绿装马匹装备重新锻造
		int modify_remakedz(zObject* ob,zObjectB *objbase);

		//soke 绿装重新升星 
		bool re_upgrade(zObject* ob);

		//soke 绿装重新升星 
		bool re2_upgrade(zObject* ob);

	    //soke 紫装重新升星 
		bool repur_upgrade(zObject* ob);

		//soke 紫装重新升星 
		bool repur2_upgrade(zObject* ob);

		//soke 升级改造
		int makelv_upgrade(zObject* ob);

		//继承属性
		int upgrade_inherit(SceneUser& user,zObject* new_ob,zObject* old_ob);

		//soke 升级改造属性
		int upgrade_attri(zObject* ob,zObjectB *objbase);

	private:
		struct Odds
		{
			int per;
			int luck;
			double material_level;
			int skill_level;
			int odds;		
			int sleight;
			int odds_gem;
			Odds() : per(0), luck(0), material_level(0), skill_level(0), odds(0), sleight(0), odds_gem(0)
			{ }
		};

		Odds _odds;
		double _current;
		double _base;

		bool _make;

		int _need;
		int _1_id;
		int _1_level;
		int _2_id;
		int _2_level;

		int odds_of_white(const zObjectB* ob);
		int odds_of_blue(const zObjectB* ob);
		int odds_of_gold(const zObjectB* ob);
		int odds_of_holy(int object);
		//soke 紫装
		int odds_of_purple(int object);
		int odds_of_property(int object, int property);

		template <typename T>
			void fix_kind(T* bob, zObject* ob)
			{
				switch (ob->base->kind)
				{
					case ItemType_ClothBody :         //101代表布质加生命类服装
					case ItemType_StaffFlag:		  //141代表战旗(法师)
	        		case ItemType_CrossFlag:	      //142代表战旗(弓手)
	        		case ItemType_Flag:	              //143代表战旗(仙术)
	        		case ItemType_StickFlag:	      //144代表战旗(召唤)
	        		case ItemType_BladeFlag:	      //145代表战旗(战士)
	        		case ItemType_DaggerFlag:	      //146代表战旗(刺客)
	        		case ItemType_SwordFlag:          //147代表战旗(卫士)
						additivePercent(ob->data.maxhp, bob->maxsp);
						break;
					case ItemType_FellBody :	    //102代表皮甲加魔防类服装
						additivePercent(ob->data.mdefence, bob->maxsp);	
						break;
					case ItemType_MetalBody:	//103代表金属铠甲加物防类服装
					case ItemType_Shield: 	    //112代表盾牌类
						additivePercent(ob->data.pdefence, bob->maxsp);			
						break;
					case ItemType_Blade:		    //104代表武术刀类武器
					case ItemType_Sword:            //105代表武术剑类武器
					case ItemType_Axe:	            //106代表武术斧类武器
					case ItemType_Hammer:	        //107代表武术斧类武器
					case ItemType_Crossbow:	        //109代表箭术弓类武器
						additivePercent(ob->data.pdamage, bob->maxsp);		
						additivePercent(ob->data.maxpdamage, bob->maxsp);		
						break;
					case ItemType_Staff:		    //108代表法术杖类武器
					case ItemType_Stick:	        //111代表召唤棍类武器
					case ItemType_Fan:	            //110代表美女扇类
					case ItemType_Dagger:	        //136代表利刃
						additivePercent(ob->data.mdamage, bob->maxsp);		
						additivePercent(ob->data.maxmdamage, bob->maxsp);		
						break;
					case ItemType_Helm:		        //113代表角色头盔类
					case ItemType_Caestus:	        //114代表角色腰带类
					case ItemType_Cuff:		        //115代表角色护腕类
					case ItemType_Shoes:		    //116代表角色鞋子类
					case ItemType_HorseRope:        //131代表坐骑马绳类（防御）
			        case ItemType_HorseSaddle:      //132代表坐骑马鞍类（防御）
					case ItemType_breastplate:	    //138代表护心镜
					case ItemType_Handgun:          //156代表短枪
						if (zMisc::randBetween(0, 1)) 
						{
							additivePercent(ob->data.pdefence, bob->maxsp);		
						}
						else 
						{
							additivePercent(ob->data.mdefence, bob->maxsp);				
						}
						break;
					case ItemType_Necklace:	          //117代表角色项链类
					case ItemType_Fing:		          //118代表角色戒指类
					case ItemType_HorseShoe:          //130代表坐骑马脚类（攻击）
			        case ItemType_HorseSafe:          //133代表坐骑马甲类（攻击）
		        	case ItemType_HorseIron:          //134代表坐骑马镫类（攻击）
					case ItemType_NewSword:	          //137代表卫士剑
					case ItemType_LongGun:            //155代表长枪
						if (ob->data.pdamage || ob->data.maxpdamage) 
						{
							additivePercent(ob->data.pdamage, bob->maxsp);		
							additivePercent(ob->data.maxpdamage, bob->maxsp);		
						}
						if (ob->data.mdamage || ob->data.maxmdamage) 
						{
							additivePercent(ob->data.mdamage, bob->maxsp);		
							additivePercent(ob->data.maxmdamage, bob->maxsp);		
						}
						break;
				}

			}


		/**     
		 * \brief 属性取最大值
		 *
		 * \param ret: 计算结果
		 * \param lv: 属性取值范围
		 * \return 无
		 */	
		template <typename T>
			bool max(T& ret, const luckRangeValue &rv)
			{
				if( zMisc::selectByTenTh(rv.per) )	{
					ret += rv.data.max;

				}
				/*
				if ( zMisc::selectByPercent(_odds.sleight) ) {
					ret += rv.sleightValue;
					return true;
				}
				// */
				return false;
			}
        	/**     
		 * \brief 生成特殊装备
		 *
		 * \param bob: 对应装备基本表
		 * \param ob: 打造物品
		 * \param kind: 装备类型
		 * \return 当前总是返回true
		 */	
            template <typename T>
			 bool assign_bind(T* bob, zObject* ob, int kind, int props = 0,zObjectB *base=NULL ,bool drop=false)
            {
                char tmp[MAX_NAMESIZE];
                snprintf(tmp, MAX_NAMESIZE, "%s%s%s", bob->prefix, bob->joint, ob->data.strName);
                strncpy(ob->data.strName, tmp, MAX_NAMESIZE);

                int property = 1;

                if(props >= 17)
                {
                    ++property;

                    if (ob->data.fivetype == FIVE_NONE) 
                    {
                        ob->data.fivetype = zMisc::randBetween(0, 4);
                    }
                }
                else
                {
                    if (ob->data.fivetype == FIVE_NONE && zMisc::selectByTenTh(bob->five.per)) 
                    {
                        ob->data.fivetype = zMisc::randBetween(0, 4);
                    }
                }


                COMPUTE_R( pdamage )		// 最小物攻
                COMPUTE_R( maxpdamage )		// 最大物攻
                COMPUTE_R( mdamage )		// 最小魔攻
                COMPUTE_R( maxmdamage )		// 最大魔攻
                COMPUTE_R( pdefence )		// 物防
                COMPUTE_R( mdefence )		// 魔防    

                if (props) 
                {
                    int index = zMisc::randBetween(0, 4);
                    if (index!=5) 
                    {

                        //现在不用随机了,直接取值
                        additivePercent(*ob->_p1[index], bob->_p1[index]);
                    }
                    else 
                    {
                        fix_kind(bob, ob);
                    }
                }
                else 
                {
                    COMPUTE_L( str )			// 力量
                    COMPUTE_L( inte )			// 智力
                    COMPUTE_L( dex )			// 敏捷
                    COMPUTE_L( spi )			// 精神
                    COMPUTE_L( con )			// 体质
                }		

                COMPUTE_L( maxhp )		    // 最大生命值
                COMPUTE_L( maxmp )		    // 最大法术值
                //COMPUTE_L( maxsp )	    // 最大体力值

                COMPUTE_L( mvspeed )		// 移动速度
                COMPUTE_L( hpr )			// 生命值恢复
                COMPUTE_L( mpr )			// 法术值恢复
                COMPUTE_L( spr )			// 体力值恢复
                COMPUTE_L( akspeed )		// 攻击速度  

                if (props) 
                {
                    for (int i=0; i<=17; ++i)
                    {
                        if(props == 17)
                        {
                            if( i == 0 || i == 2)
                            {
                                if(bob->_p2[i].per)
                                    *ob->_p2[i] += bob->_p2[i].data.max;
                            }
                            else if (i == 1 || i == 3)
                            {
                                if(bob->_p2[i].per)
                                {
                                    int temp = zMisc::randBetween(((bob->_p2[i].data.max - bob->_p2[i].data.min)/2 + bob->_p2[i].data.min + 1),bob->_p2[i].data.max);
                                    *ob->_p2[i] += temp;
                                }
                            }
                            else
                            {
                                max(*ob->_p2[i], bob->_p2[i]);
                            }
                        }
                        else if(props == 18)
                        {
                            if( i == 0 || i == 2)
                            {
                                if(bob->_p2[i].per)
                                    *ob->_p2[i] += 10;
                            }
                            else if (i == 1 || i == 3)
                            {
                                if(bob->_p2[i].per)
                                    *ob->_p2[i] += bob->_p2[i].data.max;
                            }
                            else
                            {
                                max(*ob->_p2[i], bob->_p2[i]);
                            }
                        }
                        else
                        {
                            additive(*ob->_p2[i], bob->_p2[i]);
                        }
                    }
                }
                else 
                {
                    COMPUTE_L( pdam )		// 增加物理攻击力
                    COMPUTE_L( mdam )		// 增加魔法攻击力
                    COMPUTE_L( pdef )		// 增加物理防御力
                    COMPUTE_L( mdef )		// 增加魔法防御力
                    COMPUTE_L( atrating )	// 命中率
                    COMPUTE_L( akdodge )	// 闪避率

                    COMPUTE_L( poisondef )	    // 抗毒增加
                    COMPUTE_L( lulldef )		// 抗麻痹增加
                    COMPUTE_L( reeldef )		// 抗眩晕增加
                    COMPUTE_L( evildef )		// 抗噬魔增加
                    COMPUTE_L( bitedef )		// 抗噬力增加
                    COMPUTE_L( chaosdef )	    // 抗混乱增加
                    COMPUTE_L( colddef )		// 抗冰冻增加
                    COMPUTE_L( petrifydef )		// 抗石化增加
                    COMPUTE_L( blinddef )		// 抗失明增加
                    COMPUTE_L( stabledef )		// 抗定身增加
                    COMPUTE_L( slowdef )		// 抗减速增加
                    COMPUTE_L( luredef )		// 抗诱惑增加
                }

                if (!ob->data.durpoint) 
                {
                    if (additive(ob->data.durpoint, bob->resumedur, property)) 
                    {
                        ob->data.durpoint -= bob->resumedur.sleightValue;
                    }
                    if (ob->data.durpoint) ob->data.dursecond = bob->resumedur.sleightValue;
                }

                COMPUTE_L( bang ) 			//重击
                //ob->data.bang += bob->bang;
                //戒指项链至少一个
                if(ob->base->kind == ItemType_Fing || ob->base->kind == ItemType_Necklace)
                {
                    BONUS_SKILL_RING
                }
                BONUS_SKILL
                BONUS_SKILLS

                if (props) 
                    ob->data.kind |= 2;//有色装备
                else 
                    ob->data.kind |= kind;//有色装备

                return true;
            }

            //soke 改造属性、保留灵魂锁链改造
			template <typename T>
			bool modify_assign_color(T* bob, zObject* ob, int kind, int props = 0,zObjectB *base=NULL ,bool drop=false,bool bfive=true)
			{
				//soke 不加这段 改造不会变成最高标识
				memset(ob->data.strName,0,MAX_NAMESIZE); //soke 删除装备名字 防止叠加
		        strncpy(ob->data.strName, ob->base->name, MAX_NAMESIZE);//改造装备名字

				char tmp[MAX_NAMESIZE];
                snprintf(tmp, MAX_NAMESIZE, "%s%s%s", bob->prefix, bob->joint, ob->data.strName); //soke 标识 完美、卓越的等
                strncpy(ob->data.strName, tmp, MAX_NAMESIZE);

				int property = 1;

				if(props >= 17)
				{
					++property;
					if (bfive)
					{
						ob->data.fivetype = zMisc::randBetween(0, 4);
					}

				}
				else 
				{
					if (ob->data.fivetype == FIVE_NONE && zMisc::selectByTenTh(bob->five.per)) 
					{
						if (bfive)
						{
							ob->data.fivetype = zMisc::randBetween(0, 4);
						}
					}
				}


				COMPUTE_R( pdamage )		// 最小物攻
				COMPUTE_R( maxpdamage )		// 最大物攻
				COMPUTE_R( mdamage )			// 最小魔攻
				COMPUTE_R( maxmdamage )		// 最大魔攻
				COMPUTE_R( pdefence )			// 物防
				COMPUTE_R( mdefence )			// 魔防

				if (props) 
				{
					for (int i = 0;i<5; i++)
					{
						*ob->_p1[i] = 0;
					}
					int index = zMisc::randBetween(0, 4);
					if (index!=5) 
					{

						////现在不用随机了,直接取值
						//if(*ob->_p1[index] > 0)
						//{
						//	*ob->_p1[index] = 0;//否则改造的时候同样属性会叠加
						//}
						additivePercent(*ob->_p1[index], bob->_p1[index]);
					}
					else 
					{
						fix_kind(bob, ob);
					}
				}
				else 
				{
					COMPUTE_L( str )			// 力量
					COMPUTE_L( inte )			// 智力
					COMPUTE_L( dex )			// 敏捷
					COMPUTE_L( spi )			// 精神
					COMPUTE_L( con )				// 体质
				}		

				COMPUTE_L( maxhp )		// 最大生命值
				COMPUTE_L( maxmp )		// 最大法术值
				//		COMPUTE_L( maxsp )		// 最大体力值

			   //soke 清除生命值、法术值恢复（否则改造会不断叠加）
				ob->data.hpr = 0;
				ob->data.mpr = 0;
				COMPUTE_L( mvspeed )		// 移动速度
				COMPUTE_L( hpr )			// 生命值恢复
				COMPUTE_L( mpr )			// 法术值恢复
				COMPUTE_L( spr )			// 体力值恢复
				COMPUTE_L( akspeed )		// 攻击速度

				//改造百分比属性不动
				if (props) //品质。。完美 无暇。。。
			{
				for (int i = 0;i<17;i++)
				{
					*ob->_p2[i] = 0;//清0  防止叠加
				}
				for (int i=0; i<=17; ++i)
				{
					if(props == 17)
					{
						if( i == 0 || i == 2)
						{
							//soke 增加攻击力始终都是10％
							if(bob->_p2[i].per)
							{
								*ob->_p2[i] += zMisc::randBetween(bob->_p2[i].data.min,bob->_p2[i].data.max);
								//*ob->_p2[i] = zMisc::randBetween(bob->_p2[i].data.min,bob->_p2[i].data.max)+zMisc::randBetween(0,15);
								//*ob->_p2[i] = 13 + zMisc::randBetween(0,5);
							}
						}
						//改造百分比属性不动
						else if (i == 1 || i == 3)
						{
							if(bob->_p2[i].per)
							{
								int temp = zMisc::randBetween(((bob->_p2[i].data.max - bob->_p2[i].data.min)/2 + bob->_p2[i].data.min + 1),bob->_p2[i].data.max);
								*ob->_p2[i] += temp;
								//*ob->_p2[i] = zMisc::randBetween(bob->_p2[i].data.min,bob->_p2[i].data.max)+zMisc::randBetween(0,15);
								//*ob->_p2[i] = 13 + zMisc::randBetween(0,5);
							}
						}
						else
						{
							max(*ob->_p2[i], bob->_p2[i]);
						}
					}
					else if(props == 18)
					{
						if( i == 0 || i == 2)
						{
							//soke 增加攻击力始终都是10％
							int temp = 0;
							if(ob->data.kind & 16) temp = 5;
							if(bob->_p2[i].per)
							{
								//*ob->_p2[i] = zMisc::randBetween(bob->_p2[i].data.min,bob->_p2[i].data.max)+zMisc::randBetween(0,15);
								//*ob->_p2[i] = 13 + zMisc::randBetween(0,5);
								*ob->_p2[i] = zMisc::randBetween(bob->_p2[i].data.min,bob->_p2[i].data.max)+temp;
							}
						}
						//改造百分比属性不动
						else if (i == 1 || i == 3)
						{
							int temp = 0;
							if(ob->data.kind & 16) temp = 5;
							if(bob->_p2[i].per)
							{
								//*ob->_p2[i] = zMisc::randBetween(bob->_p2[i].data.min,bob->_p2[i].data.max)+zMisc::randBetween(0,15);
								//*ob->_p2[i] = 13 + zMisc::randBetween(0,5);
								*ob->_p2[i] = zMisc::randBetween(bob->_p2[i].data.min,bob->_p2[i].data.max)+temp;
							}
						}
						else
						{
							max(*ob->_p2[i], bob->_p2[i]);
						}
					}
					else
					{
						additive(*ob->_p2[i], bob->_p2[i]);
					}
				}

			}
				//else 
				//{

				//	COMPUTE_L( pdam )		// 增加物理攻击力
				//	COMPUTE_L( mdam )		// 增加魔法攻击力
				//	COMPUTE_L( pdef )		// 增加物理防御力
				//	COMPUTE_L( mdef )		// 增加魔法防御力
				//	COMPUTE_L( atrating )		// 命中率
				//	COMPUTE_L( akdodge )		// 闪避率

				//	COMPUTE_L( poisondef )	// 抗毒增加
				//	COMPUTE_L( lulldef )		// 抗麻痹增加
				//	COMPUTE_L( reeldef )		// 抗眩晕增加
				//	COMPUTE_L( evildef )		// 抗噬魔增加
				//	COMPUTE_L( bitedef )		// 抗噬力增加
				//	COMPUTE_L( chaosdef )	// 抗混乱增加
				//	COMPUTE_L( colddef )		// 抗冰冻增加
				//	COMPUTE_L( petrifydef )		// 抗石化增加
				//	COMPUTE_L( blinddef )		// 抗失明增加
				//	COMPUTE_L( stabledef )		// 抗定身增加
				//	COMPUTE_L( slowdef )		// 抗减速增加
				//	COMPUTE_L( luredef )		// 抗诱惑增加
				//}

				if (!ob->data.durpoint) 
				{
					if (additive(ob->data.durpoint, bob->resumedur, property)) 
					{
						ob->data.durpoint -= bob->resumedur.sleightValue;
					}
					if (ob->data.durpoint) ob->data.dursecond = bob->resumedur.sleightValue;

				}

				COMPUTE_L( bang ) 			//重击
				//ob->data.bang += bob->bang;

				//soke 清除技能（否则技能不能随机刷新）
			    memset(ob->data.skill,0,sizeof(skillbonus)*10);

				//戒指项链至少一个
				if(ob->base->kind == ItemType_Fing || ob->base->kind == ItemType_Necklace)
				{
					BONUS_SKILL_RING
				}
				BONUS_SKILL
				BONUS_SKILLS
				if (props) 
					ob->data.kind |= 2;//有色装备
				else 
					ob->data.kind |= kind;//有色装备

				return true;
		}

		template <typename T>
		bool modify_assign_bind(T* bob, zObject* ob, int kind, int props = 0,zObjectB *base=NULL ,bool drop=false)
		{
			int property = 1;

			if(props >= 17)
			{
				++property;
			}
			if (props) //品质。。完美 无暇。。。
			{
				for (int i = 0;i<17;i++)
				{
					*ob->_p2[i] = 0;//清0  防止叠加
				}
				for (int i=0; i<=17; ++i)
				{
					if(props)
					{
						if( i == 0 || i == 2)
						{
							//soke 增加攻击力始终都是10％
							if(bob->_p2[i].per)
							{
								//*ob->_p2[i] = zMisc::randBetween(bob->_p2[i].data.min,bob->_p2[i].data.max)+zMisc::randBetween(0,15);
								*ob->_p2[i] = 17 + zMisc::randBetween(0,1);
							}
						}
						//改造百分比属性不动
						else if (i == 1 || i == 3)
						{
							if(bob->_p2[i].per)
								//*ob->_p2[i] = zMisc::randBetween(bob->_p2[i].data.min,bob->_p2[i].data.max)+zMisc::randBetween(0,15);
								*ob->_p2[i] = 17 + zMisc::randBetween(0,1);
						}
						else
						{
							max(*ob->_p2[i], bob->_p2[i]);
						}
					}
					else
					{
						additive(*ob->_p2[i], bob->_p2[i]);
					}
				}

			}
			else 
			{

				COMPUTE_L( pdam )		// 增加物理攻击力
				COMPUTE_L( mdam )		// 增加魔法攻击力
				COMPUTE_L( pdef )		// 增加物理防御力
				COMPUTE_L( mdef )		// 增加魔法防御力
				COMPUTE_L( atrating )		// 命中率
				COMPUTE_L( akdodge )		// 闪避率

				COMPUTE_L( poisondef )	// 抗毒增加
				COMPUTE_L( lulldef )		// 抗麻痹增加
				COMPUTE_L( reeldef )		// 抗眩晕增加
				COMPUTE_L( evildef )		// 抗噬魔增加
				COMPUTE_L( bitedef )		// 抗噬力增加
				COMPUTE_L( chaosdef )	// 抗混乱增加
				COMPUTE_L( colddef )		// 抗冰冻增加
				COMPUTE_L( petrifydef )		// 抗石化增加
				COMPUTE_L( blinddef )		// 抗失明增加
				COMPUTE_L( stabledef )		// 抗定身增加
				COMPUTE_L( slowdef )		// 抗减速增加
				COMPUTE_L( luredef )		// 抗诱惑增加
			}

			return true;
		}


        //soke 修改技能
		template <typename T>
		bool modify_assign_skill(T* bob, zObject* ob, int kind, int props = 0,zObjectB *base=NULL ,bool drop=false)
		{
			int property = 1;

			if(props >= 17)
			{
				++property;
			}
			
			//soke 清除技能（否则技能不能随机刷新）
			memset(ob->data.skill,0,sizeof(skillbonus)*10);
			if(ob->base->kind == ItemType_Fing || ob->base->kind == ItemType_Necklace)
			{
				BONUS_SKILL_RING
			}
			    BONUS_SKILL
				BONUS_SKILLS

				return true;
		}

			//soke 紫装改造
			template <typename T>
			bool assign_bind2(T* bob, zObject* ob, int kind, int props = 0,zObjectB *base=NULL ,bool drop=false)
            {
                char tmp[MAX_NAMESIZE];
                snprintf(tmp, MAX_NAMESIZE, "%s%s%s", bob->prefix, bob->joint, ob->data.strName);
                strncpy(ob->data.strName, tmp, MAX_NAMESIZE);

                int property = 1;

                if(props >= 17)
                {
                    ++property;

                    if (ob->data.fivetype == FIVE_NONE) 
                    {
                        ob->data.fivetype = zMisc::randBetween(0, 4);
                    }
                }
                else
                {
                    if (ob->data.fivetype == FIVE_NONE && zMisc::selectByTenTh(bob->five.per)) 
                    {
                        ob->data.fivetype = zMisc::randBetween(0, 4);
                    }
                }


                COMPUTE_R( pdamage )		// 最小物攻
                COMPUTE_R( maxpdamage )		// 最大物攻
                COMPUTE_R( mdamage )		// 最小魔攻
                COMPUTE_R( maxmdamage )		// 最大魔攻
                COMPUTE_R( pdefence )		// 物防
                COMPUTE_R( mdefence )		// 魔防    

                if (props) 
                {
                    int index = zMisc::randBetween(0, 4);
                    if (index!=5) 
                    {

                        //现在不用随机了,直接取值
                        additivePercent(*ob->_p1[index], bob->_p1[index]);
                    }
                    else 
                    {
                        fix_kind(bob, ob);
                    }
                }
                else 
                {
                    COMPUTE_L( str )			// 力量
                    COMPUTE_L( inte )			// 智力
                    COMPUTE_L( dex )			// 敏捷
                    COMPUTE_L( spi )			// 精神
                    COMPUTE_L( con )			// 体质
                }		

                COMPUTE_L( maxhp )		    // 最大生命值
                COMPUTE_L( maxmp )		    // 最大法术值
                //COMPUTE_L( maxsp )	    // 最大体力值

                COMPUTE_L( mvspeed )		// 移动速度
                COMPUTE_L( hpr )			// 生命值恢复
                COMPUTE_L( mpr )			// 法术值恢复
                COMPUTE_L( spr )			// 体力值恢复
                COMPUTE_L( akspeed )		// 攻击速度  

                if (props) 
                {
                    for (int i=0; i<=17; ++i)
                    {
                        if(props == 17)
                        {
                            if( i == 0 || i == 2)
                            {
                                if(bob->_p2[i].per)
                                    *ob->_p2[i] += bob->_p2[i].data.max;
                            }
                            else if (i == 1 || i == 3)
                            {
                                if(bob->_p2[i].per)
                                {
                                    int temp = zMisc::randBetween(((bob->_p2[i].data.max - bob->_p2[i].data.min)/2 + bob->_p2[i].data.min + 1),bob->_p2[i].data.max);
                                    *ob->_p2[i] += temp;
                                }
                            }
                            else
                            {
                                max(*ob->_p2[i], bob->_p2[i]);
                            }
                        }
                        else if(props == 18)
                        {
                            if( i == 0 || i == 2)
                            {
				//soke 增加攻击力始终都是10％
                                if(bob->_p2[i].per)
				*ob->_p2[i] = 17 + zMisc::randBetween(0,1);//重新绑定地址
                            }
                            else if (i == 1 || i == 3)
                            {
                                if(bob->_p2[i].per)
                                    *ob->_p2[i] += bob->_p2[i].data.max;
                            }
                            else
                            {
                                max(*ob->_p2[i], bob->_p2[i]);
                            }
                        }
                        else
                        {
                            additive(*ob->_p2[i], bob->_p2[i]);
                        }
                    }
                }
                else 
                {
                    COMPUTE_L( pdam )		// 增加物理攻击力
                    COMPUTE_L( mdam )		// 增加魔法攻击力
                    COMPUTE_L( pdef )		// 增加物理防御力
                    COMPUTE_L( mdef )		// 增加魔法防御力
                    COMPUTE_L( atrating )	// 命中率
                    COMPUTE_L( akdodge )	// 闪避率

                    COMPUTE_L( poisondef )	    // 抗毒增加
                    COMPUTE_L( lulldef )		// 抗麻痹增加
                    COMPUTE_L( reeldef )		// 抗眩晕增加
                    COMPUTE_L( evildef )		// 抗噬魔增加
                    COMPUTE_L( bitedef )		// 抗噬力增加
                    COMPUTE_L( chaosdef )	    // 抗混乱增加
                    COMPUTE_L( colddef )		// 抗冰冻增加
                    COMPUTE_L( petrifydef )		// 抗石化增加
                    COMPUTE_L( blinddef )		// 抗失明增加
                    COMPUTE_L( stabledef )		// 抗定身增加
                    COMPUTE_L( slowdef )		// 抗减速增加
                    COMPUTE_L( luredef )		// 抗诱惑增加
                }

                if (!ob->data.durpoint) 
                {
                    if (additive(ob->data.durpoint, bob->resumedur, property)) 
                    {
                        ob->data.durpoint -= bob->resumedur.sleightValue;
                    }
                    if (ob->data.durpoint) ob->data.dursecond = bob->resumedur.sleightValue;
                }

                COMPUTE_L( bang ) 			//重击
                //ob->data.bang += bob->bang;
                //戒指项链至少一个
                if(ob->base->kind == ItemType_Fing || ob->base->kind == ItemType_Necklace)
                {
                    BONUS_SKILL_RING
                }
                BONUS_SKILL
                BONUS_SKILLS

                if (props) 
                    ob->data.kind |= 2;//有色装备
                else 
                    ob->data.kind |= kind;//有色装备

                return true;
            }

			 template <typename T>
            bool assign_color(T* bob, zObject* ob, int kind, int props = 0,zObjectB *base=NULL ,bool drop=false)
            {
                char tmp[MAX_NAMESIZE];
                snprintf(tmp, MAX_NAMESIZE, "%s%s%s", bob->prefix, bob->joint, ob->data.strName);
                strncpy(ob->data.strName, tmp, MAX_NAMESIZE);

                int property = 1;

                if(props >= 17)
                {
                    ++property;
                    //ob->data.fivepoint += zMisc::randBetween(bob->five.data.min, bob->five.data.max);
                    if (/*ob->data.fivepoint &&// */ ob->data.fivetype == FIVE_NONE) 
                    {
                        ob->data.fivetype = zMisc::randBetween(0, 4);
                    }
                }
                else //if (additive(ob->data.fivepoint, bob->five, property)) 
                {
                    //ob->data.fivepoint -= bob->five.sleightValue;
                    if (/*ob->data.fivepoint && // */ob->data.fivetype == FIVE_NONE && zMisc::selectByTenTh(bob->five.per)) 
                    {
                        ob->data.fivetype = zMisc::randBetween(0, 4);
                    }
                }


                COMPUTE_R( pdamage )		// 最小物攻
                COMPUTE_R( maxpdamage )		// 最大物攻
                COMPUTE_R( mdamage )		// 最小魔攻
                COMPUTE_R( maxmdamage )		// 最大魔攻
                COMPUTE_R( pdefence )		// 物防
                COMPUTE_R( mdefence )		// 魔防    

                if (props) 
                {
                    int index = zMisc::randBetween(0, 4);
                    if (index!=5) 
                    {

                        //现在不用随机了,直接取值
                        additivePercent(*ob->_p1[index], bob->_p1[index]);
                    }
                    else 
                    {
                        fix_kind(bob, ob);
                    }
                }
                else 
                {
                    COMPUTE_L( str )			// 力量
                    COMPUTE_L( inte )			// 智力
                    COMPUTE_L( dex )			// 敏捷
                    COMPUTE_L( spi )			// 精神
                    COMPUTE_L( con )			// 体质
                }		

                COMPUTE_L( maxhp )		    // 最大生命值
                COMPUTE_L( maxmp )		    // 最大法术值
                //COMPUTE_L( maxsp )	    // 最大体力值

                COMPUTE_L( mvspeed )		// 移动速度
                COMPUTE_L( hpr )			// 生命值恢复
                COMPUTE_L( mpr )			// 法术值恢复
                COMPUTE_L( spr )			// 体力值恢复
                COMPUTE_L( akspeed )		// 攻击速度  

                if (props) 
                {
                    for (int i=0; i<=17; ++i)
                    {
                        if(props == 17)
                        {
                            if( i == 0 || i == 2)
                            {
                                if(bob->_p2[i].per)
                                    *ob->_p2[i] += bob->_p2[i].data.max;
                            }
                            else if (i == 1 || i == 3)
                            {
                                if(bob->_p2[i].per)
                                {
                                    int temp = zMisc::randBetween(((bob->_p2[i].data.max - bob->_p2[i].data.min)/2 + bob->_p2[i].data.min + 1),bob->_p2[i].data.max);
                                    *ob->_p2[i] += temp;
                                }
                            }
                            else
                            {
                                max(*ob->_p2[i], bob->_p2[i]);
                            }
                        }
                        else if(props == 18)
                        {
                            if( i == 0 || i == 2)
                            {
                                if(bob->_p2[i].per)
                                    *ob->_p2[i] += 10;
                            }
                            else if (i == 1 || i == 3)
                            {
                                if(bob->_p2[i].per)
                                    *ob->_p2[i] += bob->_p2[i].data.max;
                            }
                            else
                            {
                                max(*ob->_p2[i], bob->_p2[i]);
                            }
                        }
                        else
                        {
                            additive(*ob->_p2[i], bob->_p2[i]);
                        }
                    }
                    /*
                    std::vector<int> list;
                    for (int i=0; i<=17; ++i) list.push_back(i);
                    //每个都需要随机
                    //int geted = props;
                    // */
                    /*
                    if(props == 5){
                    if(list.size() >=4){
                    additive(*ob->_p2[list[3]], bob->_p2[list[3]]);
                    }
                    if(list.size() >=8){
                    additive(*ob->_p2[list[7]], bob->_p2[list[7]]);
                    }
                    }else{
                    // */
                    //while (/*geted -- > 0 && */list.size() > 0) {
                    /*
                    int index = zMisc::randBetween(0, list.size()-1 );
                    int p = list[index];
                    //取最大值
                    if(props == 17){
                    max(*ob->_p2[p], bob->_p2[p]);
                    }else{
                    additive(*ob->_p2[p], bob->_p2[p]);
                    }
                    std::vector<int>::iterator it = list.begin();
                    list.erase(it+index);
                    // */
                    //}
                    //}

                }
                else 
                {
                    COMPUTE_L( pdam )		// 增加物理攻击力
                    COMPUTE_L( mdam )		// 增加魔法攻击力
                    COMPUTE_L( pdef )		// 增加物理防御力
                    COMPUTE_L( mdef )		// 增加魔法防御力
                    COMPUTE_L( atrating )	// 命中率
                    COMPUTE_L( akdodge )	// 闪避率

                    COMPUTE_L( poisondef )	    // 抗毒增加
                    COMPUTE_L( lulldef )		// 抗麻痹增加
                    COMPUTE_L( reeldef )		// 抗眩晕增加
                    COMPUTE_L( evildef )		// 抗噬魔增加
                    COMPUTE_L( bitedef )		// 抗噬力增加
                    COMPUTE_L( chaosdef )	    // 抗混乱增加
                    COMPUTE_L( colddef )		// 抗冰冻增加
                    COMPUTE_L( petrifydef )		// 抗石化增加
                    COMPUTE_L( blinddef )		// 抗失明增加
                    COMPUTE_L( stabledef )		// 抗定身增加
                    COMPUTE_L( slowdef )		// 抗减速增加
                    COMPUTE_L( luredef )		// 抗诱惑增加
                }

                if (!ob->data.durpoint) 
                {
                    if (additive(ob->data.durpoint, bob->resumedur, property)) 
                    {
                        ob->data.durpoint -= bob->resumedur.sleightValue;
                    }
                    if (ob->data.durpoint) ob->data.dursecond = bob->resumedur.sleightValue;
                }

                COMPUTE_L( bang ) 			//重击
                //ob->data.bang += bob->bang;
                //戒指项链至少一个
                if(ob->base->kind == ItemType_Fing || ob->base->kind == ItemType_Necklace)
                {
                    BONUS_SKILL_RING
                }
                BONUS_SKILL
                BONUS_SKILLS

                if (props) 
                    ob->data.kind |= 2;//有色装备
                else 
                    ob->data.kind |= kind;//有色装备

                return true;
            }
		
		//soke 装备升级改造
			template <typename T>
			bool ungrade_assign_color(T* bob, zObject* ob, int kind, int props = 0,zObjectB *base=NULL ,bool drop=false,bool bfive=true)
			{
				int property = 1;

				if(props >= 17)
				{
					++property;
					if (bfive)
					{
						ob->data.fivetype = zMisc::randBetween(0, 4);
					}

				}
				else 
				{
					if (ob->data.fivetype == FIVE_NONE && zMisc::selectByTenTh(bob->five.per)) 
					{
						if (bfive)
						{
							ob->data.fivetype = zMisc::randBetween(0, 4);
						}
					}
				}


				COMPUTE_R( pdamage )		// 最小物攻
				COMPUTE_R( maxpdamage )		// 最大物攻
				COMPUTE_R( mdamage )		// 最小魔攻
				COMPUTE_R( maxmdamage )		// 最大魔攻
				COMPUTE_R( pdefence )		// 物防
				COMPUTE_R( mdefence )		// 魔防

				if (props)
				{
					for (int i = 0;i<5; i++)  //soke 属性点随机
					{
						*ob->_p1[i] = 0;
					}
					int index = zMisc::randBetween(0, 4);
					if (index!=5) 
					{

						////现在不用随机了,直接取值
						//if(*ob->_p1[index] > 0)
						//{
						//	*ob->_p1[index] = 0;//否则改造的时候同样属性会叠加
						//}
						additivePercent(*ob->_p1[index], bob->_p1[index]);
					}
					else 
					{
						fix_kind(bob, ob);
					}
				}
				else 
				{
					COMPUTE_L( str )			// 力量
					COMPUTE_L( inte )			// 智力
					COMPUTE_L( dex )			// 敏捷
					COMPUTE_L( spi )			// 精神
					COMPUTE_L( con )				// 体质
				}		

				COMPUTE_L( maxhp )		// 最大生命值
				COMPUTE_L( maxmp )		// 最大法术值
			   //COMPUTE_L( maxsp )		// 最大体力值

			   //soke 清除生命值、法术值恢复（否则改造会不断叠加）
				ob->data.hpr = 0;
				ob->data.mpr = 0;
				ob->data.mf  = 0;
				COMPUTE_L( mvspeed )		// 移动速度
				COMPUTE_L( hpr )			// 生命值恢复
				COMPUTE_L( mpr )			// 法术值恢复
				COMPUTE_L( spr )			// 体力值恢复
				COMPUTE_L( akspeed )		// 攻击速度

				//改造百分比属性不动
				//if (props) //品质。。完美 无暇。。。
				//{
				//	for (int i=0; i<=17; ++i)
				//	{
				//		if(props == 17)
				//		{
				//			if( i == 0 || i == 2)
				//			{
				//				if(bob->_p2[i].per)
				//					*ob->_p2[i] += bob->_p2[i].data.max;
				//			}
				//			else if (i == 1 || i == 3)
				//			{
				//				if(bob->_p2[i].per)
				//				{
				//					int temp = zMisc::randBetween(((bob->_p2[i].data.max - bob->_p2[i].data.min)/2 + bob->_p2[i].data.min + 1),bob->_p2[i].data.max);
				//					*ob->_p2[i] += temp;
				//				}
				//			}
				//			else
				//			{
				//				max(*ob->_p2[i], bob->_p2[i]);
				//			}
				//		}
				//		else if(props == 18)
				//		{
				//			if( i == 0 || i == 2)
				//			{
				//				//soke 增加攻击力始终都是10％
				//				if(bob->_p2[i].per)
				//				{
				//					*ob->_p2[i] += bob->_p2[i].data.max;
				//				}
				//			}
				//			//改造百分比属性不动
				//			else if (i == 1 || i == 3)
				//			{
				//				if(bob->_p2[i].per)
				//					*ob->_p2[i] += bob->_p2[i].data.max;
				//			}
				//			else
				//			{
				//				max(*ob->_p2[i], bob->_p2[i]);
				//			}
				//		}
				//		else
				//		{
				//			additive(*ob->_p2[i], bob->_p2[i]);
				//		}
				//	}

				//}
				//else 
				//{

				//	COMPUTE_L( pdam )		// 增加物理攻击力
				//	COMPUTE_L( mdam )		// 增加魔法攻击力
				//	COMPUTE_L( pdef )		// 增加物理防御力
				//	COMPUTE_L( mdef )		// 增加魔法防御力
				//	COMPUTE_L( atrating )		// 命中率
				//	COMPUTE_L( akdodge )		// 闪避率

				//	COMPUTE_L( poisondef )	// 抗毒增加
				//	COMPUTE_L( lulldef )		// 抗麻痹增加
				//	COMPUTE_L( reeldef )		// 抗眩晕增加
				//	COMPUTE_L( evildef )		// 抗噬魔增加
				//	COMPUTE_L( bitedef )		// 抗噬力增加
				//	COMPUTE_L( chaosdef )	// 抗混乱增加
				//	COMPUTE_L( colddef )		// 抗冰冻增加
				//	COMPUTE_L( petrifydef )		// 抗石化增加
				//	COMPUTE_L( blinddef )		// 抗失明增加
				//	COMPUTE_L( stabledef )		// 抗定身增加
				//	COMPUTE_L( slowdef )		// 抗减速增加
				//	COMPUTE_L( luredef )		// 抗诱惑增加
				//}

				if (!ob->data.durpoint) 
				{
					if (additive(ob->data.durpoint, bob->resumedur, property)) 
					{
						ob->data.durpoint -= bob->resumedur.sleightValue;
					}
					if (ob->data.durpoint) ob->data.dursecond = bob->resumedur.sleightValue;

				}

				COMPUTE_L( bang ) 			//重击
				//ob->data.bang += bob->bang;

				//soke 清除技能（否则技能不能随机刷新）
			    memset(ob->data.skill,0,sizeof(skillbonus)*10);

				//戒指项链至少一个
				if(ob->base->kind == ItemType_Fing || ob->base->kind == ItemType_Necklace)
				{
					BONUS_SKILL_RING
				}
				BONUS_SKILL
				BONUS_SKILLS

				if (props) 
					ob->data.kind |= 2;//有色装备
				else 
					ob->data.kind |= kind;//有色装备

				return true;
		}
		
		//soke 马匹装备重新锻造
		template <typename T>
		bool modify_assign_makedz(T* bob, zObject* ob, int kind, int props = 0,zObjectB *base=NULL ,bool drop=false)
		{
			//soke 不加这段 改造不会变成最高标识
			memset(ob->data.strName,0,MAX_NAMESIZE); //soke 删除装备名字 防止叠加
		    strncpy(ob->data.strName, ob->base->name, MAX_NAMESIZE);//改造装备名字

			char tmp[MAX_NAMESIZE];
            snprintf(tmp, MAX_NAMESIZE, "%s%s%s", bob->prefix, bob->joint, ob->data.strName); //soke 标识 完美、卓越的等
            strncpy(ob->data.strName, tmp, MAX_NAMESIZE);

			int property = 1;

			if(props >= 17)
			{
				++property;
			}
			if (props) //品质。。完美 无暇。。。
			{
				for (int i = 0;i<17;i++)
				{
					*ob->_p2[i] = 0;//清0  防止叠加
				}
				for (int i=0; i<=17; ++i)
				{
					if(props == 17)
					{
						if( i == 0 || i == 2)
						{
							if(bob->_p2[i].per)
								*ob->_p2[i] += zMisc::randBetween(bob->_p2[i].data.min,bob->_p2[i].data.max);
						}
						else if (i == 1 || i == 3)
						{
							if(bob->_p2[i].per)
							{
								int temp = zMisc::randBetween(((bob->_p2[i].data.max - bob->_p2[i].data.min)/2 + bob->_p2[i].data.min + 1),bob->_p2[i].data.max);
								*ob->_p2[i] += temp;
							}
						}
						else
						{
							max(*ob->_p2[i], bob->_p2[i]);
						}
					}
					else if(props == 18)
					{
						if( i == 0 || i == 2)
						{
							//soke 增加攻击力始终都是10％
							if(bob->_p2[i].per)
							{
								//*ob->_p2[i] = zMisc::randBetween(bob->_p2[i].data.min,bob->_p2[i].data.max)+zMisc::randBetween(0,15);
								*ob->_p2[i] = 10 + zMisc::randBetween(0,13);
							}
						}
						//改造百分比属性不动
						else if (i == 1 || i == 3)
						{
							if(bob->_p2[i].per)
								//*ob->_p2[i] = zMisc::randBetween(bob->_p2[i].data.min,bob->_p2[i].data.max)+zMisc::randBetween(0,15);
								*ob->_p2[i] = 10 + zMisc::randBetween(0,13);
						}
						else
						{
							max(*ob->_p2[i], bob->_p2[i]);
						}
					}
					else
					{
						additive(*ob->_p2[i], bob->_p2[i]);
					}
				}

			}
			else 
			{

				COMPUTE_L( pdam )		// 增加物理攻击力
				COMPUTE_L( mdam )		// 增加魔法攻击力
				COMPUTE_L( pdef )		// 增加物理防御力
				COMPUTE_L( mdef )		// 增加魔法防御力
				COMPUTE_L( atrating )		// 命中率
				COMPUTE_L( akdodge )		// 闪避率

				COMPUTE_L( poisondef )	// 抗毒增加
				COMPUTE_L( lulldef )		// 抗麻痹增加
				COMPUTE_L( reeldef )		// 抗眩晕增加
				COMPUTE_L( evildef )		// 抗噬魔增加
				COMPUTE_L( bitedef )		// 抗噬力增加
				COMPUTE_L( chaosdef )	// 抗混乱增加
				COMPUTE_L( colddef )		// 抗冰冻增加
				COMPUTE_L( petrifydef )		// 抗石化增加
				COMPUTE_L( blinddef )		// 抗失明增加
				COMPUTE_L( stabledef )		// 抗定身增加
				COMPUTE_L( slowdef )		// 抗减速增加
				COMPUTE_L( luredef )		// 抗诱惑增加
			}

			return true;
		}

		bool assign_holy(zObject* ob, int holy);
		bool assign_purple(zObject* ob, int holy);

		bool assign_set(zObject* ob);

		/**     
		 * \brief 属性计算
		 *
		 * \param ret: 计算结果
		 * \param lv: 属性取值范围
		 * \return 无
		 */	
		template <typename T>
			void additive(T& ret, const rangeValue &rv)
			{
				ret += zMisc::randBetween(rv.min,rv.max);
			}

		/**     
		 * \brief 神圣属性计算
		 *
		 * \param ret: 计算结果
		 * \param lv: 属性取值范围
		 * \param property: 物品当前属性数目
		 * \return 无
		 */	
		template <typename T>
			bool additive(T& ret, const luckRangeValue & lv, int& property)
			{
				int odds = lv.per;
				//		int odds = odds_of_property(lv.per, property);
				//		Zebra::logger->debug("属性产生概率%f, %f", lv.per*1.0, odds*1.0);
				if( zMisc::selectByTenTh(odds) )	{
					++property;

					ret += zMisc::randBetween(lv.data.min, lv.data.max);

					/*
					if ( zMisc::selectByPercent(_odds.sleight) ) {
						ret += lv.sleightValue;
						return true;
					}
					// */
				}

				return false;
			}	

		template <typename T>
			bool additive(T& ret, const luckRangeValue & lv)
			{
				if( zMisc::selectByTenTh(lv.per) )	{
					ret += zMisc::randBetween(lv.data.min, lv.data.max);

				}
				/*
				if ( zMisc::selectByPercent(_odds.sleight) ) {
					ret += lv.sleightValue;
					return true;
				}
				// */

				return false;
			}	
		template <typename T>
			bool additivePercent(T& ret, const luckRangeValue & lv)
			{
				ret += zMisc::randBetween(lv.data.min, lv.data.max);
				/*
				if ( zMisc::selectByPercent(_odds.sleight) ) {
					ret += lv.sleightValue;
					return true;
				}
				// */

				return false;
			}	

};

#include "zDatabaseManager.h"

/**
 * \brief 物品改造
 *
 * 实现各种物品改造功能,提供一个统一入口
 *
 */		
class RebuildObject : public Base
{
public:		
	enum 
	{
		MAX_NUMBER = 50,
		
		HOLE_MONEY = 1000,
		ENCHANCE_MONEY = 500,
		MODIFY_MONEY = 10000, //soke 改造价格
		MAKELV_MONEY = 400000, //soke 升级装备价格 40锭
		
		COMP_SOUL_STONE_ID = 677,
		ENCHANCE_SONTE_ID = 678,
		HOLE_SONTE_ID = 679,
		LEVELUP_STONE_ID = 681, 
		MODIFYSTONE       =548, //soke 改造材料
		MAKELVTONE      =550,  //soke 升级改造材料
		MAKELVNUM		=10,
		JBIND_SOUL_STONE_ID = 1317,     //祝福石
		REBIND_SOUL_STONE_ID = 551,   //改造装备用的星辰石
	//	BIND_SOUL_STONE_ID = 552,     //强化装备用的装备强化石
		ZISEBIND_SOUL_STONE_ID = 553,  //星魂石
		ZISEBIND_SOUL_JIEBA_ID = 549,  //祥云石


	};
	
	static RebuildObject& instance();
			
	bool compose(SceneUser& user, const Cmd::stPropertyUserCmd* cmd);
	bool compose_soul_stone(SceneUser& user, const Cmd::stPropertyUserCmd* cmd);
	bool upgrade(SceneUser& user, const Cmd::stPropertyUserCmd* cmd);
	bool make(SceneUser& user, const Cmd::stPropertyUserCmd* cmd);
	bool hole(SceneUser& user, const Cmd::stPropertyUserCmd* cmd);
	bool enchance(SceneUser& user, const Cmd::stPropertyUserCmd* cmd);
	bool decompose(SceneUser& user, const Cmd::stPropertyUserCmd* cmd);
	
	bool modify(SceneUser& user, const Cmd::stPropertyUserCmd* cmd); //soke 改造

	//yikey
	bool makelv(SceneUser& user, const Cmd::stPropertyUserCmd* cmd); //soke 升级改造
	zObject* tempEquip;

	//
    ///soke 新的装备改造 2015年6月23日
	bool recast(SceneUser& user,zObjectB *base, const Cmd::stPropertyUserCmd* cmd);
   
	///装备祝福
    bool equipCompose11(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
    ///斗魂升级
    bool equipCompose12(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	 ///神石镶嵌
    bool equipCompose13(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	 ///装备栏激活
    bool equipCompose14(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	 ///装备栏进阶
    bool equipCompose15(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	 ///神器强化
    bool equipCompose16(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	 ///龙槽激活
    bool equipCompose17(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	 ///龙槽解封
    bool equipCompose18(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	 ///龙星升级
    bool equipCompose19(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	 ///魂魄清除
    bool equipCompose20(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	///装备升级
	bool equipCompose21(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	///马牌精炼升级
	bool equipCompose22(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	///神龙宝宝镶嵌
    bool equipCompose23(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	///逆天装备进阶
    bool equipCompose24(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	///神魂宝石镶嵌
    bool equipCompose25(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	///暗影宝石镶嵌
    bool equipCompose26(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	///龙凤吟
    bool equipCompose27(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	///轰天宝石镶嵌
    bool equipCompose28(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	///王者升级
    bool equipCompose29(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);



    ///勋章升级
    bool equipCompose51(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	 ///寒冰升级
    bool equipCompose52(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	 ///翅膀升级
    bool equipCompose53(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	 ///神兵升级
    bool equipCompose54(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	 ///神兽升级
    bool equipCompose55(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	///护身符璀璨宝石镶嵌
	bool equipCompose56(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	///护身符神兽宝石镶嵌
	bool equipCompose57(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);	

	///尚方宝剑升级
	bool equipCompose58(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);	
	///资金龙袍升级
	bool equipCompose59(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);	
	///神斧
	bool equipCompose61(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);	
	///黑曜
	bool equipCompose62(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);	
	///精金
	bool equipCompose63(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);	
	 ///金箍咒强化
	 bool equipCompose64(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);	
	 ///金箍咒字纹
	 bool equipCompose65(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);	
	 ///金箍咒淬炼
	 bool equipCompose66(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);	
	 ///寒冰神炼
	 bool equipCompose67(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);	
	 ///寒冰字纹
	 bool equipCompose68(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);	
	 ///寒冰纹章
	 bool equipCompose69(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);	

    ///百兽图鉴进阶
    bool equipCompose101(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	///百兽图鉴签名
    bool equipCompose102(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);

	///法宝进阶
    bool equipCompose103(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	///符石进阶
    bool equipCompose104(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);

	///十二星宿进阶
    bool equipCompose105(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);


	///元神进阶
    bool equipCompose106(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	///元神装备进阶
    bool equipCompose107(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);
	///元神装备进阶
    bool equipCompose108(SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev);


	bool doZhuanhuanCmd(SceneUser &user,const Cmd::stZhuanhuanUserCmd *ptCmd,unsigned int cmdLen);//装备转换



private:	
    /**     
	 * \brief 构造函数
	 *
	 */   	
	RebuildObject() { }
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	~RebuildObject() { }

	
	static RebuildObject* _instance;
};

#endif
