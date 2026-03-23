#ifndef _ZOBJECT_H_
#define _ZOBJECT_H_
#include <vector>
#include <set>
#include "zDatabaseManager.h"
#include "Command.h"
#include "zSceneEntry.h"
#include "zTime.h"

//#define BINARY_VERSION 	0
//#define BINARY_VERSION 	20051018
//#define BINARY_VERSION	20051225
//#define BINARY_VERSION 	20060124
//soke 修改下版本号 20060313
#define BINARY_VERSION 		20151025

struct ZlibObject
{
	DWORD 	version;
	DWORD	count;
	unsigned char data[0];
	ZlibObject()
	{
		count = 0;
		version = 0;
	}
};

struct SaveObject
{
	union
	{
		struct 
		{
			DWORD createtime;
			DWORD dwCreateThisID;	
		};
		unsigned long long createid;
	};
	// */
	t_Object object;
};
class GlobalObjectIndex;
class SceneUser;

namespace luabind
{
	namespace detail {
		template<class T>
		struct delete_s;
	}
}

struct zObject:zEntry
{
	friend class GlobalObjectIndex;
	public:
		t_Object data;
		zObjectB *base;

		//only for RebuildObject::make, it's ugly, but .....sigh
		WORD* _p1[5];
		WORD* _p2[18];
		
		static zObject *create(zObjectB *objbase, unsigned int num=1, BYTE level = 0);
		static void destroy(zObject* ob);
		static void logger(QWORD createid,DWORD objid,char *objname,DWORD num,DWORD change,DWORD type,DWORD srcid,char *srcname,DWORD dstid,char *dstname,const char *action, zObjectB *base,BYTE kind,BYTE upgrade);
		static DWORD RepairMoney2RepairGold(DWORD dwObjectRepair);
		static zObject *create(zObject *objsrc);
		static zObject *load(const SaveObject *o);
		bool getSaveData(SaveObject *save);
		int foundSocket();
		bool canMail();
		void checkBind();
		void checkXinBind(); //soke 增加个新的检测绑定
		
		zCountryMaterialB* canContribute(); // 是否能被捐献
		DWORD getMaterialKind(); // 获得原料类别: 0,普通物资，1,丝线,2,矿石,3,矿产,4,木材，5,皮毛,6,草药

		const stObjectLocation &reserve() const;
		void restore(const stObjectLocation &loc);
			
		union
		{
			struct 
			{
				DWORD createtime;
				DWORD dwCreateThisID;	
			};
			unsigned long long createid;
		};
		// */
	private:
		friend class Package;
		friend class Packages;
		friend class MainPack;
		friend class zSceneObject;
		friend class luabind::detail::delete_s<zObject>;

		zObject();
		~zObject();

		bool free() const;
		void free(bool flag);

		void fill(t_Object& d);		
		void generateThisID();
		bool inserted;
};

struct zSceneObject:public zSceneEntry
{
	static zSceneObject *create(zObject *obj, const zRTime &ct)
	{
		if(obj==NULL) return NULL;
		zSceneObject *ret=new zSceneObject(obj,ct);
		return ret;
	}
	/**
	 * \brief 检查地上物品是否过期
	 * 过期的地上物品会消失掉
	 * \param ct 当前时间
	 * \return 是否过期
	 */
	bool checkOverdue(const zRTime &ct) const
	{
		return ct >= this->ct;
	}

	bool checkProtectOverdue(const zRTime &ct)
	{
		if(ownerID == 0)
		{
			return false;
		}
		if(ct > protectTime)
		{
			protectTime = 0;
			ownerID = 0;
			return true;
		}
		return false;
	}

	void setOwner(DWORD dwID, int protime=10)
	{
		if(dwID)
		{
			ownerID = dwID;
			zRTime ctv;
			protectTime = ctv;
			this->protectTime.addDelay(protime * 1000);
		}
	}

	//国家归属
	void setGuoOwner(DWORD dwID, int protime=10)
	{
		if(dwID)
		{
			GuoownerID = dwID;
		
		}
	}

	void setOverDueTime(zRTime &ct_1)
	{
		ct = ct_1;
	}
	DWORD getOwner()
	{
		return ownerID;
	}

	//国家归属
	DWORD getGuoOwner()
	{
		return GuoownerID;
	}

	~zSceneObject()
	{
		if (o) o->free();
		zObject::destroy(o);
	}

	void clear()
	{
		o=NULL;
	}

	zObject *getObject()
	{
		return o;
	}

	private:
	DWORD ownerID;
	//国家归属
	DWORD GuoownerID;
	zRTime protectTime;
	zObject *o;
	zRTime ct;
	zSceneObject(zObject *obj, const zRTime &ct):zSceneEntry(SceneEntry_Object),ct(ct)
	{
		this->ct.addDelay(60 * 1000); //soke 修改地图上掉落的装备消失时间/60秒
		o=obj;
		ownerID=0;
		GuoownerID=0;
		id = obj->data.qwThisID;
		tempid = obj->base->id;
		strncpy(name,obj->data.strName,MAX_NAMESIZE);
	}
};

#define DECLARE(prop, type) type get_##prop() const {return prop;}
#define RESET(prop) prop = 0;
class Equips 
{
public:
	Equips()
	{
		reset();
	}
	
	void reset()
	{
		RESET(maxhp)					// 最大生命值
		RESET(maxmp)					// 最大法术值
		RESET(maxsp)					// 最大体力值
	
		RESET(pdamage)					// 最小攻击力
		RESET(maxpdamage)				// 最大攻击力
		RESET(mdamage)					// 最小法术攻击力
		RESET(maxmdamage)				// 最大法术攻击力
		RESET(appendminpet)           // 宠物增强最小值
		RESET(appendmaxpet)				// 宠物增强最大值
		RESET(pdefence)				    // 物防
		RESET(mdefence)				    // 魔防

		RESET(m1axhp)					// 升星最大生命值
		RESET(p1damage)					// 升星最小攻击力
		RESET(m1axpdamage)				// 升星最大攻击力
		RESET(m1damage)					// 升星最小法术攻击力
		RESET(m1axmdamage)				// 升星最大法术攻击力
		RESET(p1defence)				// 升星物防
		RESET(m1defence)				// 升星魔防

		RESET(m2axhp)					// 升星最大生命值
		RESET(p2damage)					// 升星最小攻击力
		RESET(m2axpdamage)				// 升星最大攻击力
		RESET(m2damage)					// 升星最小法术攻击力
		RESET(m2axmdamage)				// 升星最大法术攻击力
		RESET(p2defence)				// 升星物防
		RESET(m2defence)				// 升星魔防

		RESET(m3axhp)					// 斗魂升星最大生命值
		RESET(p3damage)					// 斗魂升星最小攻击力
		RESET(m3axpdamage)				// 斗魂升星最大攻击力
		RESET(m3damage)					// 斗魂升星最小法术攻击力
		RESET(m3axmdamage)				// 斗魂升星最大法术攻击力
		RESET(p3defence)				// 斗魂升星物防
		RESET(m3defence)				// 斗魂升星魔防
		
		RESET(m4axhp)					// 龙槽*最大生命值
		RESET(p4damage)					// 龙槽*最小攻击力
		RESET(m4axpdamage)				// 龙槽*最大攻击力
		RESET(m4damage)					// 龙槽*最小法术攻击力
		RESET(m4axmdamage)				// 龙槽*最大法术攻击力
		RESET(p4defence)				// 龙槽*物防
		RESET(m4defence)				// 龙槽*魔防
		
		RESET(m5axhp)					// 龙*最大生命值
		RESET(p5damage)					// 龙*最小攻击力
		RESET(m5axpdamage)				// 龙*最大攻击力
		RESET(m5damage)					// 龙*最小法术攻击力
		RESET(m5axmdamage)				// 龙*最大法术攻击力
		RESET(p5defence)				// 龙*物防
		RESET(m5defence)				// 龙*魔防
		
		RESET(m6axhp)					// 补天最大生命值
		RESET(p6damage)					// 补天最小攻击力
		RESET(m6axpdamage)				// 补天最大攻击力
		RESET(m6damage)					// 补天最小法术攻击力
		RESET(m6axmdamage)				// 补天最大法术攻击力
		RESET(p6defence)				// 补天物防
		RESET(m6defence)				// 补天魔防
		
		RESET(m7axhp)					// 预留最大生命值
		RESET(p7damage)					// 预留最小攻击力
		RESET(m7axpdamage)				// 预留最大攻击力
		RESET(m7damage)					// 预留最小法术攻击力
		RESET(m7axmdamage)				// 预留最大法术攻击力
		RESET(p7defence)				// 预留物防
		RESET(m7defence)				// 预留魔防
		
		RESET(m8axhp)					// 预留最大生命值
		RESET(p8damage)					// 预留最小攻击力
		RESET(m8axpdamage)				// 预留最大攻击力
		RESET(m8damage)					// 预留最小法术攻击力
		RESET(m8axmdamage)				// 预留最大法术攻击力
		RESET(p8defence)				// 预留物防
		RESET(m8defence)				// 预留魔防
		
		RESET(m9axhp)					// 预留最大生命值
		RESET(p9damage)					// 预留最小攻击力
		RESET(m9axpdamage)				// 预留最大攻击力
		RESET(m9damage)					// 预留最小法术攻击力
		RESET(m9axmdamage)				// 预留最大法术攻击力
		RESET(p9defence)				// 预留物防
		RESET(m9defence)				// 预留魔防

        RESET(m10axhp)		//sky 马牌精炼 最大生命值
		RESET(p10damage)	//sky 马牌精炼 最小攻击力
		RESET(m10axpdamage)	//sky 马牌精炼 最大攻击力
		RESET(m10damage)	//sky 马牌精炼 最小法术攻击力
		RESET(m10axmdamage)	//sky 马牌精炼 最大法术攻击力
		RESET(p10defence)	//sky 马牌精炼 物防
		RESET(m10defence)	//sky 马牌精炼 魔防

		RESET(m11axhp)		   //MYY 璀璨镶嵌 最大生命值
		RESET(p11damage)	   //MYY 璀璨镶嵌 最小攻击力
		RESET(m11axpdamage)	   //MYY 璀璨镶嵌 最大攻击力
		RESET(m11damage)	   //MYY 璀璨镶嵌 最小法术攻击力
		RESET(m11axmdamage)	   //MYY 璀璨镶嵌 最大法术攻击力
		RESET(p11defence)	   //MYY 璀璨镶嵌 物防
		RESET(m11defence)	   //MYY 璀璨镶嵌 魔防

		RESET(m12axhp)		   //MYY 神兽宝石增加 最大生命值
		RESET(p12damage)	   //MYY 神兽宝石增加 最小攻击力
		RESET(m12axpdamage)	   //MYY 神兽宝石增加 最大攻击力
		RESET(m12damage)	   //MYY 神兽宝石增加 最小法术攻击力
		RESET(m12axmdamage)	   //MYY 神兽宝石增加 最大法术攻击力
		RESET(p12defence)	   //MYY 神兽宝石增加 物防
		RESET(m12defence)	   //MYY 神兽宝石增加 魔防

		RESET(m13axhp)		   //MYY 百兽图鉴进阶 最大生命值
		RESET(p13damage)	   //MYY 百兽图鉴进阶 最小攻击力
		RESET(m13axpdamage)	   //MYY 百兽图鉴进阶 最大攻击力
		RESET(m13damage)	   //MYY 百兽图鉴进阶 最小法术攻击力
		RESET(m13axmdamage)	   //MYY 百兽图鉴进阶 最大法术攻击力
		RESET(p13defence)	   //MYY 百兽图鉴进阶 物防
		RESET(m13defence)	   //MYY 百兽图鉴进阶 魔防

		RESET(m14axhp)		   //MYY 龙王套装增加 最大生命值
		RESET(p14damage)	   //MYY 龙王套装增加 最小攻击力
		RESET(m14axpdamage)	   //MYY 龙王套装增加 最大攻击力
		RESET(m14damage)	   //MYY 龙王套装增加 最小法术攻击力
		RESET(m14axmdamage)	   //MYY 龙王套装增加 最大法术攻击力
		RESET(p14defence)	   //MYY 龙王套装增加 物防
		RESET(m14defence)	   //MYY 龙王套装增加 魔防

		RESET(m15axhp)		   //MYY 逆天进阶增加 最大生命值
		RESET(p15damage)	   //MYY 逆天进阶增加 最小攻击力
		RESET(m15axpdamage)	   //MYY 逆天进阶增加 最大攻击力
		RESET(m15damage)	   //MYY 逆天进阶增加 最小法术攻击力
		RESET(m15axmdamage)	   //MYY 逆天进阶增加 最大法术攻击力
		RESET(p15defence)	   //MYY 逆天进阶增加 物防
		RESET(m15defence)	   //MYY 逆天进阶增加 魔防

		RESET(m16axhp)		   //MYY 斗战圣佛增加 最大生命值
		RESET(p16damage)	   //MYY 斗战圣佛增加 最小攻击力
		RESET(m16axpdamage)	   //MYY 斗战圣佛增加 最大攻击力
		RESET(m16damage)	   //MYY 斗战圣佛增加 最小法术攻击力
		RESET(m16axmdamage)	   //MYY 斗战圣佛增加 最大法术攻击力
		RESET(p16defence)	   //MYY 斗战圣佛增加 物防
		RESET(m16defence)	   //MYY 斗战圣佛增加 魔防

		RESET(m17axhp)		   //MYY 暗影石增加 最大生命值
		RESET(p17damage)	   //MYY 暗影石增加 最小攻击力
		RESET(m17axpdamage)	   //MYY 暗影石增加 最大攻击力
		RESET(m17damage)	   //MYY 暗影石增加 最小法术攻击力
		RESET(m17axmdamage)	   //MYY 暗影石增加 最大法术攻击力
		RESET(p17defence)	   //MYY 暗影石增加 物防
		RESET(m17defence)	   //MYY 暗影石增加 魔防

		RESET(m18axhp)		   //MYY 尚方宝剑增加 最大生命值
		RESET(p18damage)	   //MYY 尚方宝剑增加 最小攻击力
		RESET(m18axpdamage)	   //MYY 尚方宝剑增加 最大攻击力
		RESET(m18damage)	   //MYY 尚方宝剑增加 最小法术攻击力
		RESET(m18axmdamage)	   //MYY 尚方宝剑增加 最大法术攻击力
		RESET(p18defence)	   //MYY 尚方宝剑增加 物防
		RESET(m18defence)	   //MYY 尚方宝剑增加 魔防

		RESET(m19axhp)		   //MYY 紫金龙袍增加 最大生命值
		RESET(p19damage)	   //MYY 紫金龙袍增加 最小攻击力
		RESET(m19axpdamage)	   //MYY 紫金龙袍增加 最大攻击力
		RESET(m19damage)	   //MYY 紫金龙袍增加 最小法术攻击力
		RESET(m19axmdamage)	   //MYY 紫金龙袍增加 最大法术攻击力
		RESET(p19defence)	   //MYY 紫金龙袍增加 物防
		RESET(m19defence)	   //MYY 紫金龙袍增加 魔防	

		RESET(m20axhp)		   //MYY 装备预留功能 最大生命值
		RESET(p20damage)	   //MYY 装备预留功能 最小攻击力
		RESET(m20axpdamage)	   //MYY 装备预留功能 最大攻击力
		RESET(m20damage)	   //MYY 装备预留功能 最小法术攻击力
		RESET(m20axmdamage)	   //MYY 装备预留功能 最大法术攻击力
		RESET(p20defence)	   //MYY 装备预留功能 物防
		RESET(m20defence)	   //MYY 装备预留功能 魔防	

		RESET(m21axhp)		   //MYY 装备预留功能 最大生命值
		RESET(p21damage)	   //MYY 装备预留功能 最小攻击力
		RESET(m21axpdamage)	   //MYY 装备预留功能 最大攻击力
		RESET(m21damage)	   //MYY 装备预留功能 最小法术攻击力
		RESET(m21axmdamage)	   //MYY 装备预留功能 最大法术攻击力
		RESET(p21defence)	   //MYY 装备预留功能 物防
		RESET(m21defence)	   //MYY 装备预留功能 魔防	

		RESET(m22axhp)		   //MYY 装备预留功能 最大生命值
		RESET(p22damage)	   //MYY 装备预留功能 最小攻击力
		RESET(m22axpdamage)	   //MYY 装备预留功能 最大攻击力
		RESET(m22damage)	   //MYY 装备预留功能 最小法术攻击力
		RESET(m22axmdamage)	   //MYY 装备预留功能 最大法术攻击力
		RESET(p22defence)	   //MYY 装备预留功能 物防
		RESET(m22defence)	   //MYY 装备预留功能 魔防	

		RESET(m23axhp)     //MYY 装备预留功能 最大生命值
		RESET(p23damage)    //MYY 装备预留功能 最小攻击力
		RESET(m23axpdamage)    //MYY 装备预留功能 最大攻击力
		RESET(m23damage)    //MYY 装备预留功能 最小法术攻击力
		RESET(m23axmdamage)    //MYY 装备预留功能 最大法术攻击力
		RESET(p23defence)    //MYY 装备预留功能 物防
		RESET(m23defence)    //MYY 装备预留功能 魔防 

		RESET(m24axhp)     //MYY 装备预留功能 最大生命值
		RESET(p24damage)    //MYY 装备预留功能 最小攻击力
		RESET(m24axpdamage)    //MYY 装备预留功能 最大攻击力
		RESET(m24damage)    //MYY 装备预留功能 最小法术攻击力
		RESET(m24axmdamage)    //MYY 装备预留功能 最大法术攻击力
		RESET(p24defence)    //MYY 装备预留功能 物防
		RESET(m24defence)    //MYY 装备预留功能 魔防 

		RESET(m25axhp)     //MYY 装备预留功能 最大生命值
		RESET(p25damage)    //MYY 装备预留功能 最小攻击力
		RESET(m25axpdamage)    //MYY 装备预留功能 最大攻击力
		RESET(m25damage)    //MYY 装备预留功能 最小法术攻击力
		RESET(m25axmdamage)    //MYY 装备预留功能 最大法术攻击力
		RESET(p25defence)    //MYY 装备预留功能 物防
		RESET(m25defence)    //MYY 装备预留功能 魔防 

		RESET(m26axhp)     //MYY 装备预留功能 最大生命值
		RESET(p26damage)    //MYY 装备预留功能 最小攻击力
		RESET(m26axpdamage)    //MYY 装备预留功能 最大攻击力
		RESET(m26damage)    //MYY 装备预留功能 最小法术攻击力
		RESET(m26axmdamage)    //MYY 装备预留功能 最大法术攻击力
		RESET(p26defence)    //MYY 装备预留功能 物防
		RESET(m26defence)    //MYY 装备预留功能 魔防 

		RESET(m27axhp)     //MYY 装备预留功能 最大生命值
		RESET(p27damage)    //MYY 装备预留功能 最小攻击力
		RESET(m27axpdamage)    //MYY 装备预留功能 最大攻击力
		RESET(m27damage)    //MYY 装备预留功能 最小法术攻击力
		RESET(m27axmdamage)    //MYY 装备预留功能 最大法术攻击力
		RESET(p27defence)    //MYY 装备预留功能 物防
		RESET(m27defence)    //MYY 装备预留功能 魔防 

		RESET(m28axhp)     //MYY 装备预留功能 最大生命值
		RESET(p28damage)    //MYY 装备预留功能 最小攻击力
		RESET(m28axpdamage)    //MYY 装备预留功能 最大攻击力
		RESET(m28damage)    //MYY 装备预留功能 最小法术攻击力
		RESET(m28axmdamage)    //MYY 装备预留功能 最大法术攻击力
		RESET(p28defence)    //MYY 装备预留功能 物防
		RESET(m28defence)    //MYY 装备预留功能 魔防 

		RESET(m29axhp)     //MYY 装备预留功能 最大生命值
		RESET(p29damage)    //MYY 装备预留功能 最小攻击力
		RESET(m29axpdamage)    //MYY 装备预留功能 最大攻击力
		RESET(m29damage)    //MYY 装备预留功能 最小法术攻击力
		RESET(m29axmdamage)    //MYY 装备预留功能 最大法术攻击力
		RESET(p29defence)    //MYY 装备预留功能 物防
		RESET(m29defence)    //MYY 装备预留功能 魔防 

		RESET(m30axhp)     //ZM 装备预留功能 最大生命值
		RESET(p30damage)    //ZM 装备预留功能 最小攻击力
		RESET(m30axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m30damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m30axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p30defence)    //ZM 装备预留功能 物防
		RESET(m30defence)    //ZM 装备预留功能 魔防 

		RESET(m31axhp)     //ZM 装备预留功能 最大生命值
		RESET(p31damage)    //ZM 装备预留功能 最小攻击力
		RESET(m31axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m31damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m31axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p31defence)    //ZM 装备预留功能 物防
		RESET(m31defence)    //ZM 装备预留功能 魔防 

		RESET(m32axhp)     //ZM 装备预留功能 最大生命值
		RESET(p32damage)    //ZM 装备预留功能 最小攻击力
		RESET(m32axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m32damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m32axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p32defence)    //ZM 装备预留功能 物防
		RESET(m32defence)    //ZM 装备预留功能 魔防 

		RESET(m33axhp)     //ZM 装备预留功能 最大生命值
		RESET(p33damage)    //ZM 装备预留功能 最小攻击力
		RESET(m33axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m33damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m33axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p33defence)    //ZM 装备预留功能 物防
		RESET(m33defence)    //ZM 装备预留功能 魔防 

		RESET(m34axhp)     //ZM 装备预留功能 最大生命值
		RESET(p34damage)    //ZM 装备预留功能 最小攻击力
		RESET(m34axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m34damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m34axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p34defence)    //ZM 装备预留功能 物防
		RESET(m34defence)    //ZM 装备预留功能 魔防 

		RESET(m35axhp)     //ZM 装备预留功能 最大生命值
		RESET(p35damage)    //ZM 装备预留功能 最小攻击力
		RESET(m35axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m35damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m35axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p35defence)    //ZM 装备预留功能 物防
		RESET(m35defence)    //ZM 装备预留功能 魔防 

		RESET(m36axhp)     //ZM 装备预留功能 最大生命值
		RESET(p36damage)    //ZM 装备预留功能 最小攻击力
		RESET(m36axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m36damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m36axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p36defence)    //ZM 装备预留功能 物防
		RESET(m36defence)    //ZM 装备预留功能 魔防 

		RESET(m37axhp)     //ZM 装备预留功能 最大生命值
		RESET(p37damage)    //ZM 装备预留功能 最小攻击力
		RESET(m37axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m37damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m37axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p37defence)    //ZM 装备预留功能 物防
		RESET(m37defence)    //ZM 装备预留功能 魔防 

		RESET(m38axhp)     //ZM 装备预留功能 最大生命值
		RESET(p38damage)    //ZM 装备预留功能 最小攻击力
		RESET(m38axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m38damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m38axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p38defence)    //ZM 装备预留功能 物防
		RESET(m38defence)    //ZM 装备预留功能 魔防 

		RESET(m39axhp)     //ZM 装备预留功能 最大生命值
		RESET(p39damage)    //ZM 装备预留功能 最小攻击力
		RESET(m39axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m39damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m39axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p39defence)    //ZM 装备预留功能 物防
		RESET(m39defence)    //ZM 装备预留功能 魔防 

		RESET(m40axhp)     //ZM 装备预留功能 最大生命值
		RESET(p40damage)    //ZM 装备预留功能 最小攻击力
		RESET(m40axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m40damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m40axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p40defence)    //ZM 装备预留功能 物防
		RESET(m40defence)    //ZM 装备预留功能 魔防 

		RESET(m41axhp)     //ZM 装备预留功能 最大生命值
		RESET(p41damage)    //ZM 装备预留功能 最小攻击力
		RESET(m41axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m41damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m41axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p41defence)    //ZM 装备预留功能 物防
		RESET(m41defence)    //ZM 装备预留功能 魔防 

		RESET(m42axhp)     //ZM 装备预留功能 最大生命值
		RESET(p42damage)    //ZM 装备预留功能 最小攻击力
		RESET(m42axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m42damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m42axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p42defence)    //ZM 装备预留功能 物防
		RESET(m42defence)    //ZM 装备预留功能 魔防 

		RESET(m43axhp)     //ZM 装备预留功能 最大生命值
		RESET(p43damage)    //ZM 装备预留功能 最小攻击力
		RESET(m43axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m43damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m43axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p43defence)    //ZM 装备预留功能 物防
		RESET(m43defence)    //ZM 装备预留功能 魔防 

		RESET(m44axhp)     //ZM 装备预留功能 最大生命值
		RESET(p44damage)    //ZM 装备预留功能 最小攻击力
		RESET(m44axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m44damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m44axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p44defence)    //ZM 装备预留功能 物防
		RESET(m44defence)    //ZM 装备预留功能 魔防 

		RESET(m45axhp)     //ZM 装备预留功能 最大生命值
		RESET(p45damage)    //ZM 装备预留功能 最小攻击力
		RESET(m45axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m45damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m45axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p45defence)    //ZM 装备预留功能 物防
		RESET(m45defence)    //ZM 装备预留功能 魔防 

		RESET(m46axhp)     //ZM 装备预留功能 最大生命值
		RESET(p46damage)    //ZM 装备预留功能 最小攻击力
		RESET(m46axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m46damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m46axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p46defence)    //ZM 装备预留功能 物防
		RESET(m46defence)    //ZM 装备预留功能 魔防 

		RESET(m47axhp)     //ZM 装备预留功能 最大生命值
		RESET(p47damage)    //ZM 装备预留功能 最小攻击力
		RESET(m47axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m47damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m47axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p47defence)    //ZM 装备预留功能 物防
		RESET(m47defence)    //ZM 装备预留功能 魔防 

		RESET(m48axhp)     //ZM 装备预留功能 最大生命值
		RESET(p48damage)    //ZM 装备预留功能 最小攻击力
		RESET(m48axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m48damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m48axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p48defence)    //ZM 装备预留功能 物防
		RESET(m48defence)    //ZM 装备预留功能 魔防 

		RESET(m49axhp)     //ZM 装备预留功能 最大生命值
		RESET(p49damage)    //ZM 装备预留功能 最小攻击力
		RESET(m49axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m49damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m49axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p49defence)    //ZM 装备预留功能 物防
		RESET(m49defence)    //ZM 装备预留功能 魔防 

		RESET(m50axhp)     //ZM 装备预留功能 最大生命值
		RESET(p50damage)    //ZM 装备预留功能 最小攻击力
		RESET(m50axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m50damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m50axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p50defence)    //ZM 装备预留功能 物防
		RESET(m50defence)    //ZM 装备预留功能 魔防 

		RESET(m51axhp)     //ZM 装备预留功能 最大生命值
		RESET(p51damage)    //ZM 装备预留功能 最小攻击力
		RESET(m51axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m51damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m51axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p51defence)    //ZM 装备预留功能 物防
		RESET(m51defence)    //ZM 装备预留功能 魔防 

		RESET(m52axhp)     //ZM 装备预留功能 最大生命值
		RESET(p52damage)    //ZM 装备预留功能 最小攻击力
		RESET(m52axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m52damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m52axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p52defence)    //ZM 装备预留功能 物防
		RESET(m52defence)    //ZM 装备预留功能 魔防 

		RESET(m53axhp)     //ZM 装备预留功能 最大生命值
		RESET(p53damage)    //ZM 装备预留功能 最小攻击力
		RESET(m53axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m53damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m53axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p53defence)    //ZM 装备预留功能 物防
		RESET(m53defence)    //ZM 装备预留功能 魔防 

		RESET(m54axhp)     //ZM 装备预留功能 最大生命值
		RESET(p54damage)    //ZM 装备预留功能 最小攻击力
		RESET(m54axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m54damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m54axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p54defence)    //ZM 装备预留功能 物防
		RESET(m54defence)    //ZM 装备预留功能 魔防 

		RESET(m55axhp)     //ZM 装备预留功能 最大生命值
		RESET(p55damage)    //ZM 装备预留功能 最小攻击力
		RESET(m55axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m55damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m55axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p55defence)    //ZM 装备预留功能 物防
		RESET(m55defence)    //ZM 装备预留功能 魔防 

		RESET(m56axhp)     //ZM 装备预留功能 最大生命值
		RESET(p56damage)    //ZM 装备预留功能 最小攻击力
		RESET(m56axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m56damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m56axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p56defence)    //ZM 装备预留功能 物防
		RESET(m56defence)    //ZM 装备预留功能 魔防 

		RESET(m57axhp)     //ZM 装备预留功能 最大生命值
		RESET(p57damage)    //ZM 装备预留功能 最小攻击力
		RESET(m57axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m57damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m57axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p57defence)    //ZM 装备预留功能 物防
		RESET(m57defence)    //ZM 装备预留功能 魔防 

		RESET(m58axhp)     //ZM 装备预留功能 最大生命值
		RESET(p58damage)    //ZM 装备预留功能 最小攻击力
		RESET(m58axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m58damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m58axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p58defence)    //ZM 装备预留功能 物防
		RESET(m58defence)    //ZM 装备预留功能 魔防 

		RESET(m59axhp)     //ZM 装备预留功能 最大生命值
		RESET(p59damage)    //ZM 装备预留功能 最小攻击力
		RESET(m59axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m59damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m59axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p59defence)    //ZM 装备预留功能 物防
		RESET(m59defence)    //ZM 装备预留功能 魔防 

		RESET(m60axhp)     //ZM 装备预留功能 最大生命值
		RESET(p60damage)    //ZM 装备预留功能 最小攻击力
		RESET(m60axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m60damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m60axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p60defence)    //ZM 装备预留功能 物防
		RESET(m60defence)    //ZM 装备预留功能 魔防 

		RESET(m61axhp)     //ZM 装备预留功能 最大生命值
		RESET(p61damage)    //ZM 装备预留功能 最小攻击力
		RESET(m61axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m61damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m61axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p61defence)    //ZM 装备预留功能 物防
		RESET(m61defence)    //ZM 装备预留功能 魔防 

		RESET(m62axhp)     //ZM 装备预留功能 最大生命值
		RESET(p62damage)    //ZM 装备预留功能 最小攻击力
		RESET(m62axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m62damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m62axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p62defence)    //ZM 装备预留功能 物防
		RESET(m62defence)    //ZM 装备预留功能 魔防 

		RESET(m63axhp)     //ZM 装备预留功能 最大生命值
		RESET(p63damage)    //ZM 装备预留功能 最小攻击力
		RESET(m63axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m63damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m63axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p63defence)    //ZM 装备预留功能 物防
		RESET(m63defence)    //ZM 装备预留功能 魔防 

		RESET(m64axhp)     //ZM 装备预留功能 最大生命值
		RESET(p64damage)    //ZM 装备预留功能 最小攻击力
		RESET(m64axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m64damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m64axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p64defence)    //ZM 装备预留功能 物防
		RESET(m64defence)    //ZM 装备预留功能 魔防 

		RESET(m65axhp)     //ZM 装备预留功能 最大生命值
		RESET(p65damage)    //ZM 装备预留功能 最小攻击力
		RESET(m65axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m65damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m65axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p65defence)    //ZM 装备预留功能 物防
		RESET(m65defence)    //ZM 装备预留功能 魔防 

		RESET(m66axhp)     //ZM 装备预留功能 最大生命值
		RESET(p66damage)    //ZM 装备预留功能 最小攻击力
		RESET(m66axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m66damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m66axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p66defence)    //ZM 装备预留功能 物防
		RESET(m66defence)    //ZM 装备预留功能 魔防 

		RESET(m67axhp)     //ZM 装备预留功能 最大生命值
		RESET(p67damage)    //ZM 装备预留功能 最小攻击力
		RESET(m67axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m67damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m67axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p67defence)    //ZM 装备预留功能 物防
		RESET(m67defence)    //ZM 装备预留功能 魔防 

		RESET(m68axhp)     //ZM 装备预留功能 最大生命值
		RESET(p68damage)    //ZM 装备预留功能 最小攻击力
		RESET(m68axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m68damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m68axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p68defence)    //ZM 装备预留功能 物防
		RESET(m68defence)    //ZM 装备预留功能 魔防 

		RESET(m69axhp)     //ZM 装备预留功能 最大生命值
		RESET(p69damage)    //ZM 装备预留功能 最小攻击力
		RESET(m69axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m69damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m69axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p69defence)    //ZM 装备预留功能 物防
		RESET(m69defence)    //ZM 装备预留功能 魔防 

		RESET(m70axhp)     //ZM 装备预留功能 最大生命值
		RESET(p70damage)    //ZM 装备预留功能 最小攻击力
		RESET(m70axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m70damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m70axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p70defence)    //ZM 装备预留功能 物防
		RESET(m70defence)    //ZM 装备预留功能 魔防 

		RESET(m71axhp)     //ZM 装备预留功能 最大生命值
		RESET(p71damage)    //ZM 装备预留功能 最小攻击力
		RESET(m71axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m71damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m71axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p71defence)    //ZM 装备预留功能 物防
		RESET(m71defence)    //ZM 装备预留功能 魔防 

		RESET(m72axhp)     //ZM 装备预留功能 最大生命值
		RESET(p72damage)    //ZM 装备预留功能 最小攻击力
		RESET(m72axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m72damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m72axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p72defence)    //ZM 装备预留功能 物防
		RESET(m72defence)    //ZM 装备预留功能 魔防 

		RESET(m73axhp)     //ZM 装备预留功能 最大生命值
		RESET(p73damage)    //ZM 装备预留功能 最小攻击力
		RESET(m73axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m73damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m73axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p73defence)    //ZM 装备预留功能 物防
		RESET(m73defence)    //ZM 装备预留功能 魔防 

		RESET(m74axhp)     //ZM 装备预留功能 最大生命值
		RESET(p74damage)    //ZM 装备预留功能 最小攻击力
		RESET(m74axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m74damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m74axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p74defence)    //ZM 装备预留功能 物防
		RESET(m74defence)    //ZM 装备预留功能 魔防 

		RESET(m75axhp)     //ZM 装备预留功能 最大生命值
		RESET(p75damage)    //ZM 装备预留功能 最小攻击力
		RESET(m75axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m75damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m75axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p75defence)    //ZM 装备预留功能 物防
		RESET(m75defence)    //ZM 装备预留功能 魔防 

		RESET(m76axhp)     //ZM 装备预留功能 最大生命值
		RESET(p76damage)    //ZM 装备预留功能 最小攻击力
		RESET(m76axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m76damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m76axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p76defence)    //ZM 装备预留功能 物防
		RESET(m76defence)    //ZM 装备预留功能 魔防 

		RESET(m77axhp)     //ZM 装备预留功能 最大生命值
		RESET(p77damage)    //ZM 装备预留功能 最小攻击力
		RESET(m77axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m77damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m77axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p77defence)    //ZM 装备预留功能 物防
		RESET(m77defence)    //ZM 装备预留功能 魔防 

		RESET(m78axhp)     //ZM 装备预留功能 最大生命值
		RESET(p78damage)    //ZM 装备预留功能 最小攻击力
		RESET(m78axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m78damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m78axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p78defence)    //ZM 装备预留功能 物防
		RESET(m78defence)    //ZM 装备预留功能 魔防 

		RESET(m79axhp)     //ZM 装备预留功能 最大生命值
		RESET(p79damage)    //ZM 装备预留功能 最小攻击力
		RESET(m79axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m79damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m79axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p79defence)    //ZM 装备预留功能 物防
		RESET(m79defence)    //ZM 装备预留功能 魔防 

		RESET(m80axhp)     //ZM 装备预留功能 最大生命值
		RESET(p80damage)    //ZM 装备预留功能 最小攻击力
		RESET(m80axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m80damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m80axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p80defence)    //ZM 装备预留功能 物防
		RESET(m80defence)    //ZM 装备预留功能 魔防 

		RESET(m81axhp)     //ZM 装备预留功能 最大生命值
		RESET(p81damage)    //ZM 装备预留功能 最小攻击力
		RESET(m81axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m81damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m81axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p81defence)    //ZM 装备预留功能 物防
		RESET(m81defence)    //ZM 装备预留功能 魔防 

		RESET(m82axhp)     //ZM 装备预留功能 最大生命值
		RESET(p82damage)    //ZM 装备预留功能 最小攻击力
		RESET(m82axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m82damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m82axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p82defence)    //ZM 装备预留功能 物防
		RESET(m82defence)    //ZM 装备预留功能 魔防 

		RESET(m83axhp)     //ZM 装备预留功能 最大生命值
		RESET(p83damage)    //ZM 装备预留功能 最小攻击力
		RESET(m83axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m83damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m83axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p83defence)    //ZM 装备预留功能 物防
		RESET(m83defence)    //ZM 装备预留功能 魔防 

		RESET(m84axhp)     //ZM 装备预留功能 最大生命值
		RESET(p84damage)    //ZM 装备预留功能 最小攻击力
		RESET(m84axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m84damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m84axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p84defence)    //ZM 装备预留功能 物防
		RESET(m84defence)    //ZM 装备预留功能 魔防 

		RESET(m85axhp)     //ZM 装备预留功能 最大生命值
		RESET(p85damage)    //ZM 装备预留功能 最小攻击力
		RESET(m85axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m85damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m85axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p85defence)    //ZM 装备预留功能 物防
		RESET(m85defence)    //ZM 装备预留功能 魔防 

		RESET(m86axhp)     //ZM 装备预留功能 最大生命值
		RESET(p86damage)    //ZM 装备预留功能 最小攻击力
		RESET(m86axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m86damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m86axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p86defence)    //ZM 装备预留功能 物防
		RESET(m86defence)    //ZM 装备预留功能 魔防 

		RESET(m87axhp)     //ZM 装备预留功能 最大生命值
		RESET(p87damage)    //ZM 装备预留功能 最小攻击力
		RESET(m87axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m87damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m87axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p87defence)    //ZM 装备预留功能 物防
		RESET(m87defence)    //ZM 装备预留功能 魔防 

		RESET(m88axhp)     //ZM 装备预留功能 最大生命值
		RESET(p88damage)    //ZM 装备预留功能 最小攻击力
		RESET(m88axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m88damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m88axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p88defence)    //ZM 装备预留功能 物防
		RESET(m88defence)    //ZM 装备预留功能 魔防 

		RESET(m89axhp)     //ZM 装备预留功能 最大生命值
		RESET(p89damage)    //ZM 装备预留功能 最小攻击力
		RESET(m89axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m89damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m89axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p89defence)    //ZM 装备预留功能 物防
		RESET(m89defence)    //ZM 装备预留功能 魔防 

		RESET(m90axhp)     //ZM 装备预留功能 最大生命值
		RESET(p90damage)    //ZM 装备预留功能 最小攻击力
		RESET(m90axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m90damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m90axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p90defence)    //ZM 装备预留功能 物防
		RESET(m90defence)    //ZM 装备预留功能 魔防 

		RESET(m91axhp)     //ZM 装备预留功能 最大生命值
		RESET(p91damage)    //ZM 装备预留功能 最小攻击力
		RESET(m91axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m91damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m91axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p91defence)    //ZM 装备预留功能 物防
		RESET(m91defence)    //ZM 装备预留功能 魔防 

		RESET(m92axhp)     //ZM 装备预留功能 最大生命值
		RESET(p92damage)    //ZM 装备预留功能 最小攻击力
		RESET(m92axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m92damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m92axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p92defence)    //ZM 装备预留功能 物防
		RESET(m92defence)    //ZM 装备预留功能 魔防 

		RESET(m93axhp)     //ZM 装备预留功能 最大生命值
		RESET(p93damage)    //ZM 装备预留功能 最小攻击力
		RESET(m93axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m93damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m93axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p93defence)    //ZM 装备预留功能 物防
		RESET(m93defence)    //ZM 装备预留功能 魔防 

		RESET(m94axhp)     //ZM 装备预留功能 最大生命值
		RESET(p94damage)    //ZM 装备预留功能 最小攻击力
		RESET(m94axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m94damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m94axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p94defence)    //ZM 装备预留功能 物防
		RESET(m94defence)    //ZM 装备预留功能 魔防 

		RESET(m95axhp)     //ZM 装备预留功能 最大生命值
		RESET(p95damage)    //ZM 装备预留功能 最小攻击力
		RESET(m95axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m95damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m95axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p95defence)    //ZM 装备预留功能 物防
		RESET(m95defence)    //ZM 装备预留功能 魔防 

		RESET(m96axhp)     //ZM 装备预留功能 最大生命值
		RESET(p96damage)    //ZM 装备预留功能 最小攻击力
		RESET(m96axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m96damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m96axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p96defence)    //ZM 装备预留功能 物防
		RESET(m96defence)    //ZM 装备预留功能 魔防 

		RESET(m97axhp)     //ZM 装备预留功能 最大生命值
		RESET(p97damage)    //ZM 装备预留功能 最小攻击力
		RESET(m97axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m97damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m97axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p97defence)    //ZM 装备预留功能 物防
		RESET(m97defence)    //ZM 装备预留功能 魔防 

		RESET(m98axhp)     //ZM 装备预留功能 最大生命值
		RESET(p98damage)    //ZM 装备预留功能 最小攻击力
		RESET(m98axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m98damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m98axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p98defence)    //ZM 装备预留功能 物防
		RESET(m98defence)    //ZM 装备预留功能 魔防 

		RESET(m99axhp)     //ZM 装备预留功能 最大生命值
		RESET(p99damage)    //ZM 装备预留功能 最小攻击力
		RESET(m99axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m99damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m99axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p99defence)    //ZM 装备预留功能 物防
		RESET(m99defence)    //ZM 装备预留功能 魔防 

		RESET(m100axhp)     //ZM 装备预留功能 最大生命值
		RESET(p100damage)    //ZM 装备预留功能 最小攻击力
		RESET(m100axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m100damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m100axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p100defence)    //ZM 装备预留功能 物防
		RESET(m100defence)    //ZM 装备预留功能 魔防 

		RESET(m101axhp)     //ZM 装备预留功能 最大生命值
		RESET(p101damage)    //ZM 装备预留功能 最小攻击力
		RESET(m101axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m101damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m101axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p101defence)    //ZM 装备预留功能 物防
		RESET(m101defence)    //ZM 装备预留功能 魔防 

		RESET(m102axhp)     //ZM 装备预留功能 最大生命值
		RESET(p102damage)    //ZM 装备预留功能 最小攻击力
		RESET(m102axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m102damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m102axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p102defence)    //ZM 装备预留功能 物防
		RESET(m102defence)    //ZM 装备预留功能 魔防 

		RESET(m103axhp)     //ZM 装备预留功能 最大生命值
		RESET(p103damage)    //ZM 装备预留功能 最小攻击力
		RESET(m103axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m103damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m103axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p103defence)    //ZM 装备预留功能 物防
		RESET(m103defence)    //ZM 装备预留功能 魔防 

		RESET(m104axhp)     //ZM 装备预留功能 最大生命值
		RESET(p104damage)    //ZM 装备预留功能 最小攻击力
		RESET(m104axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m104damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m104axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p104defence)    //ZM 装备预留功能 物防
		RESET(m104defence)    //ZM 装备预留功能 魔防 

		RESET(m105axhp)     //ZM 装备预留功能 最大生命值
		RESET(p105damage)    //ZM 装备预留功能 最小攻击力
		RESET(m105axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m105damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m105axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p105defence)    //ZM 装备预留功能 物防
		RESET(m105defence)    //ZM 装备预留功能 魔防 

		RESET(m106axhp)     //ZM 装备预留功能 最大生命值
		RESET(p106damage)    //ZM 装备预留功能 最小攻击力
		RESET(m106axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m106damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m106axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p106defence)    //ZM 装备预留功能 物防
		RESET(m106defence)    //ZM 装备预留功能 魔防 

		RESET(m107axhp)     //ZM 装备预留功能 最大生命值
		RESET(p107damage)    //ZM 装备预留功能 最小攻击力
		RESET(m107axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m107damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m107axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p107defence)    //ZM 装备预留功能 物防
		RESET(m107defence)    //ZM 装备预留功能 魔防 

		RESET(m108axhp)     //ZM 装备预留功能 最大生命值
		RESET(p108damage)    //ZM 装备预留功能 最小攻击力
		RESET(m108axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m108damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m108axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p108defence)    //ZM 装备预留功能 物防
		RESET(m108defence)    //ZM 装备预留功能 魔防 

		RESET(m109axhp)     //ZM 装备预留功能 最大生命值
		RESET(p109damage)    //ZM 装备预留功能 最小攻击力
		RESET(m109axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m109damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m109axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p109defence)    //ZM 装备预留功能 物防
		RESET(m109defence)    //ZM 装备预留功能 魔防 

		RESET(m110axhp)     //ZM 装备预留功能 最大生命值
		RESET(p110damage)    //ZM 装备预留功能 最小攻击力
		RESET(m110axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m110damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m110axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p110defence)    //ZM 装备预留功能 物防
		RESET(m110defence)    //ZM 装备预留功能 魔防 

		RESET(m111axhp)     //ZM 装备预留功能 最大生命值
		RESET(p111damage)    //ZM 装备预留功能 最小攻击力
		RESET(m111axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m111damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m111axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p111defence)    //ZM 装备预留功能 物防
		RESET(m111defence)    //ZM 装备预留功能 魔防 

		RESET(m112axhp)     //ZM 装备预留功能 最大生命值
		RESET(p112damage)    //ZM 装备预留功能 最小攻击力
		RESET(m112axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m112damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m112axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p112defence)    //ZM 装备预留功能 物防
		RESET(m112defence)    //ZM 装备预留功能 魔防 

		RESET(m113axhp)     //ZM 装备预留功能 最大生命值
		RESET(p113damage)    //ZM 装备预留功能 最小攻击力
		RESET(m113axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m113damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m113axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p113defence)    //ZM 装备预留功能 物防
		RESET(m113defence)    //ZM 装备预留功能 魔防 

		RESET(m114axhp)     //ZM 装备预留功能 最大生命值
		RESET(p114damage)    //ZM 装备预留功能 最小攻击力
		RESET(m114axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m114damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m114axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p114defence)    //ZM 装备预留功能 物防
		RESET(m114defence)    //ZM 装备预留功能 魔防 

		RESET(m115axhp)     //ZM 装备预留功能 最大生命值
		RESET(p115damage)    //ZM 装备预留功能 最小攻击力
		RESET(m115axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m115damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m115axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p115defence)    //ZM 装备预留功能 物防
		RESET(m115defence)    //ZM 装备预留功能 魔防 

		RESET(m116axhp)     //ZM 装备预留功能 最大生命值
		RESET(p116damage)    //ZM 装备预留功能 最小攻击力
		RESET(m116axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m116damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m116axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p116defence)    //ZM 装备预留功能 物防
		RESET(m116defence)    //ZM 装备预留功能 魔防 

		RESET(m117axhp)     //ZM 装备预留功能 最大生命值
		RESET(p117damage)    //ZM 装备预留功能 最小攻击力
		RESET(m117axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m117damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m117axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p117defence)    //ZM 装备预留功能 物防
		RESET(m117defence)    //ZM 装备预留功能 魔防 

		RESET(m118axhp)     //ZM 装备预留功能 最大生命值
		RESET(p118damage)    //ZM 装备预留功能 最小攻击力
		RESET(m118axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m118damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m118axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p118defence)    //ZM 装备预留功能 物防
		RESET(m118defence)    //ZM 装备预留功能 魔防 

		RESET(m119axhp)     //ZM 装备预留功能 最大生命值
		RESET(p119damage)    //ZM 装备预留功能 最小攻击力
		RESET(m119axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m119damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m119axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p119defence)    //ZM 装备预留功能 物防
		RESET(m119defence)    //ZM 装备预留功能 魔防 

		RESET(m120axhp)     //ZM 装备预留功能 最大生命值
		RESET(p120damage)    //ZM 装备预留功能 最小攻击力
		RESET(m120axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m120damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m120axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p120defence)    //ZM 装备预留功能 物防
		RESET(m120defence)    //ZM 装备预留功能 魔防 

		RESET(m121axhp)     //ZM 装备预留功能 最大生命值
		RESET(p121damage)    //ZM 装备预留功能 最小攻击力
		RESET(m121axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m121damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m121axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p121defence)    //ZM 装备预留功能 物防
		RESET(m121defence)    //ZM 装备预留功能 魔防 

		RESET(m122axhp)     //ZM 装备预留功能 最大生命值
		RESET(p122damage)    //ZM 装备预留功能 最小攻击力
		RESET(m122axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m122damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m122axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p122defence)    //ZM 装备预留功能 物防
		RESET(m122defence)    //ZM 装备预留功能 魔防 

		RESET(m123axhp)     //ZM 装备预留功能 最大生命值
		RESET(p123damage)    //ZM 装备预留功能 最小攻击力
		RESET(m123axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m123damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m123axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p123defence)    //ZM 装备预留功能 物防
		RESET(m123defence)    //ZM 装备预留功能 魔防 

		RESET(m124axhp)     //ZM 装备预留功能 最大生命值
		RESET(p124damage)    //ZM 装备预留功能 最小攻击力
		RESET(m124axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m124damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m124axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p124defence)    //ZM 装备预留功能 物防
		RESET(m124defence)    //ZM 装备预留功能 魔防 

		RESET(m125axhp)     //ZM 装备预留功能 最大生命值
		RESET(p125damage)    //ZM 装备预留功能 最小攻击力
		RESET(m125axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m125damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m125axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p125defence)    //ZM 装备预留功能 物防
		RESET(m125defence)    //ZM 装备预留功能 魔防 

		RESET(m126axhp)     //ZM 装备预留功能 最大生命值
		RESET(p126damage)    //ZM 装备预留功能 最小攻击力
		RESET(m126axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m126damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m126axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p126defence)    //ZM 装备预留功能 物防
		RESET(m126defence)    //ZM 装备预留功能 魔防 

		RESET(m127axhp)     //ZM 装备预留功能 最大生命值
		RESET(p127damage)    //ZM 装备预留功能 最小攻击力
		RESET(m127axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m127damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m127axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p127defence)    //ZM 装备预留功能 物防
		RESET(m127defence)    //ZM 装备预留功能 魔防 

		RESET(m128axhp)     //ZM 装备预留功能 最大生命值
		RESET(p128damage)    //ZM 装备预留功能 最小攻击力
		RESET(m128axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m128damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m128axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p128defence)    //ZM 装备预留功能 物防
		RESET(m128defence)    //ZM 装备预留功能 魔防 

		RESET(m129axhp)     //ZM 装备预留功能 最大生命值
		RESET(p129damage)    //ZM 装备预留功能 最小攻击力
		RESET(m129axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m129damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m129axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p129defence)    //ZM 装备预留功能 物防
		RESET(m129defence)    //ZM 装备预留功能 魔防 

		RESET(hongtiannum8)     //ZM 装备预留功能 最大生命值
		RESET(hongtiannum9)    //ZM 装备预留功能 最小攻击力
		RESET(m130axpdamage)    //ZM 装备预留功能 最大攻击力
		RESET(m130damage)    //ZM 装备预留功能 最小法术攻击力
		RESET(m130axmdamage)    //ZM 装备预留功能 最大法术攻击力
		RESET(p130defence)    //ZM 装备预留功能 物防
		RESET(m130defence)    //ZM 装备预留功能 魔防 

		RESET(hongtiannum1)     //ZM 装备预留功能 最大生命值
		RESET(hongtiannum2)    //ZM 装备预留功能 最小攻击力
		RESET(hongtiannum3)    //ZM 装备预留功能 最大攻击力
		RESET(hongtiannum4)    //ZM 装备预留功能 最小法术攻击力
		RESET(hongtiannum5)    //ZM 装备预留功能 最大法术攻击力
		RESET(hongtiannum6)    //ZM 装备预留功能 物防
		RESET(hongtiannum7)    //ZM 装备预留功能 魔防 

	

		RESET(damagebonus)			// 伤害加成
		RESET(damage)				// 增加伤害值x％
		
		RESET(akspeed)				// 攻击速度
		RESET(mvspeed)				// 移动速度
		RESET(atrating)				// 命中率
		RESET(akdodge)				// 躲避率
	
		RESET(str)				  	// 力量
		RESET(inte)  				// 智力
		RESET(dex)  				// 敏捷
		RESET(spi)  				// 精神
		RESET(con)  				// 体质

		RESET(xstr)				  	// 力量-魂魄
		RESET(xinte)  				// 智力-魂魄
		RESET(xdex)  				// 敏捷-魂魄
		RESET(xspi)  				// 精神-魂魄
		RESET(xcon)  				// 体质-魂魄
		
		RESET(hpr)					// 生命值恢复
		RESET(mpr)  					// 法术值恢复
		RESET(spr)  					// 体力值恢复
		
		RESET(holy)						//神圣一击
		RESET(bang)						//重击率
		RESET(pdam) 					// 增加物理攻击力
		RESET(pdef)  					// 增加物理防御力
		RESET(mdam)  					// 增加魔法攻击力
		RESET(mdef)  					// 增加魔法防御力
		
		RESET(poisondef) 				//抗毒增加
		RESET(lulldef) 				//抗麻痹增加
		RESET(reeldef) 				//抗眩晕增加
		RESET(evildef) 				//抗噬魔增加
		RESET(bitedef) 				//抗噬力增加
		RESET(chaosdef) 				//抗混乱增加
		RESET(colddef) 				//抗冰冻增加
		RESET(petrifydef) 			//抗石化增加
		RESET(blinddef) 				//抗失明增加
		RESET(stabledef) 				//抗定身增加
		RESET(slowdef)				 //抗减速增加
		RESET(luredef) 				//抗诱惑增加
		
		RESET(poison) 				//中毒增加
		RESET(lull) 					//麻痹增加
		RESET(reel) 					//眩晕增加
		RESET(evil) 					//噬魔增加
		RESET(bite)					//噬力增加
		RESET(chaos) 					//混乱增加
		RESET(cold) 					//冰冻增加
		RESET(petrify) 				//石化增加
		RESET(blind) 					//失明增加
		RESET(stable) 				//定身增加
		RESET(slow) 					//减速增加
		RESET(lure) 					//诱惑增加
		RESET(hpleech.odds) 
		RESET(hpleech.effect) 
		RESET(mpleech.odds) 			//x%吸收生命值y
		RESET(mpleech.effect) 		//x%吸收法术值y

		RESET(hptomp)					//转换生命值为法术值x％
		RESET(dhpp) 					//物理伤害减少x%	
		RESET(dmpp)					//法术伤害值减少x%		
	
		RESET(incgold)				//增加金钱掉落x%
		RESET(doublexp)				//x%双倍经验		
		RESET(mf)						 //增加掉宝率x%

		//五行套装相关属性
		RESET(dpdam)			//物理伤害减少%x
		RESET(dmdam)			//法术伤害减少%x
		RESET(bdam)				//增加伤害x%
		RESET(rdam)				//伤害反射%x
		RESET(ignoredef)		//%x忽视目标防御
		//soke 灵魂锁链 
		RESET(addpdef)          //物理防御力增强xxx
		RESET(addmdef)          //魔法防御力增强xxx
        RESET(addpdam)          //增加物理攻击力xxxx
        RESET(addmdam)          //增加魔法攻击力xxxx
        RESET(addhp)            //增加生命值xxxxx
		
//		RESET(aftype)
		aftype = FIVE_NONE;
		RESET(afpoint)
//		RESET(dftype)
		dftype = FIVE_NONE;
		RESET(dfpoint)
		RESET(maxhprate)
		RESET(maxmprate)
		
		skill.clear();
		skills.clear();
	}
	
	DECLARE( maxhp, DWORD )				// 最大生命值
	DECLARE( maxmp, DWORD )				// 最大法术值
	DECLARE( maxsp, DWORD )				// 最大体力值

	DECLARE( pdamage, DWORD )			// 最小攻击力
	DECLARE( maxpdamage, DWORD )		// 最大攻击力
	DECLARE( mdamage, DWORD )			// 最小法术攻击力
	DECLARE( maxmdamage, DWORD )		// 最大法术攻击力
	DECLARE( appendminpet, DWORD )  // 最小宠物增强
	DECLARE( appendmaxpet, DWORD )  // 最大宠物增强

	DECLARE( pdefence, DWORD )			// 物防
	DECLARE( mdefence, DWORD )			// 魔防

	DECLARE( m1axhp, DWORD )			// 升星最大生命值
	DECLARE( p1damage, DWORD )			// 升星最小攻击力
	DECLARE( m1axpdamage, DWORD )		// 升星最大攻击力
	DECLARE( m1damage, DWORD )			// 升星最小法术攻击力
	DECLARE( m1axmdamage, DWORD )		// 升星最大法术攻击力
	DECLARE( p1defence, DWORD )			// 升星物防
	DECLARE( m1defence, DWORD )			// 升星魔防

	DECLARE( m2axhp, DWORD )			// 升星最大生命值
	DECLARE( p2damage, DWORD )			// 升星最小攻击力
	DECLARE( m2axpdamage, DWORD )		// 升星最大攻击力
	DECLARE( m2damage, DWORD )			// 升星最小法术攻击力
	DECLARE( m2axmdamage, DWORD )		// 升星最大法术攻击力
	DECLARE( p2defence, DWORD )			// 升星物防
	DECLARE( m2defence, DWORD )			// 升星魔防

	DECLARE( m3axhp, DWORD )			// 斗魂升星最大生命值
	DECLARE( p3damage, DWORD )			// 斗魂升星最小攻击力
	DECLARE( m3axpdamage, DWORD )		// 斗魂升星最大攻击力
	DECLARE( m3damage, DWORD )			// 斗魂升星最小法术攻击力
	DECLARE( m3axmdamage, DWORD )		// 斗魂升星最大法术攻击力
	DECLARE( p3defence, DWORD )			// 斗魂升星物防
	DECLARE( m3defence, DWORD )			// 斗魂升星魔防
	
	DECLARE( m4axhp, DWORD )			// 龙槽*最大生命值
	DECLARE( p4damage, DWORD )			// 龙槽*最小攻击力
	DECLARE( m4axpdamage, DWORD )		// 龙槽*最大攻击力
	DECLARE( m4damage, DWORD )			// 龙槽*最小法术攻击力
	DECLARE( m4axmdamage, DWORD )		// 龙槽*最大法术攻击力
	DECLARE( p4defence, DWORD )			// 龙槽*物防
	DECLARE( m4defence, DWORD )			// 龙槽*魔防
	
	DECLARE( m5axhp, DWORD )			// 龙*最大生命值
	DECLARE( p5damage, DWORD )			// 龙*最小攻击力
	DECLARE( m5axpdamage, DWORD )		// 龙*最大攻击力
	DECLARE( m5damage, DWORD )			// 龙*最小法术攻击力
	DECLARE( m5axmdamage, DWORD )		// 龙*最大法术攻击力
	DECLARE( p5defence, DWORD )			// 龙*物防
	DECLARE( m5defence, DWORD )			// 龙*魔防
	
	DECLARE( m6axhp, DWORD )			// 补天最大生命值
	DECLARE( p6damage, DWORD )			// 补天最小攻击力
	DECLARE( m6axpdamage, DWORD )		// 补天最大攻击力
	DECLARE( m6damage, DWORD )			// 补天最小法术攻击力
	DECLARE( m6axmdamage, DWORD )		// 补天最大法术攻击力
	DECLARE( p6defence, DWORD )			// 补天物防
	DECLARE( m6defence, DWORD )			// 补天魔防
	
	DECLARE( m7axhp, DWORD )			// 预留最大生命值
	DECLARE( p7damage, DWORD )			// 预留最小攻击力
	DECLARE( m7axpdamage, DWORD )		// 预留最大攻击力
	DECLARE( m7damage, DWORD )			// 预留最小法术攻击力
	DECLARE( m7axmdamage, DWORD )		// 预留最大法术攻击力
	DECLARE( p7defence, DWORD )			// 预留物防
	DECLARE( m7defence, DWORD )			// 预留魔防
	
	DECLARE( m8axhp, DWORD )			// 预留最大生命值
	DECLARE( p8damage, DWORD )			// 预留最小攻击力
	DECLARE( m8axpdamage, DWORD )		// 预留最大攻击力
	DECLARE( m8damage, DWORD )			// 预留最小法术攻击力
	DECLARE( m8axmdamage, DWORD )		// 预留最大法术攻击力
	DECLARE( p8defence, DWORD )			// 预留物防
	DECLARE( m8defence, DWORD )			// 预留魔防
	
	DECLARE( m9axhp, DWORD )			// 预留最大生命值
	DECLARE( p9damage, DWORD )			// 预留最小攻击力
	DECLARE( m9axpdamage, DWORD )		// 预留最大攻击力
	DECLARE( m9damage, DWORD )			// 预留最小法术攻击力
	DECLARE( m9axmdamage, DWORD )		// 预留最大法术攻击力
	DECLARE( p9defence, DWORD )			// 预留物防
	DECLARE( m9defence, DWORD )			// 预留魔防

    DECLARE( m10axhp, DWORD )			//sky 马牌精炼 最大生命值
	DECLARE( p10damage, DWORD )		//sky 马牌精炼 最小攻击力
	DECLARE( m10axpdamage, DWORD )		//sky 马牌精炼 最大攻击力
	DECLARE( m10damage, DWORD )		//sky 马牌精炼 最小法术攻击力
	DECLARE( m10axmdamage, DWORD )		//sky 马牌精炼 最大法术攻击力
	DECLARE( p10defence, DWORD )		//sky 马牌精炼 物防
	DECLARE( m10defence, DWORD )		//sky 马牌精炼 魔防

	DECLARE( m11axhp, DWORD )			   //MYY 璀璨镶嵌 最大生命值
	DECLARE( p11damage, DWORD )		   //MYY 璀璨镶嵌 最小攻击力
	DECLARE( m11axpdamage, DWORD )		   //MYY 璀璨镶嵌 最大攻击力
	DECLARE( m11damage, DWORD )		   //MYY 璀璨镶嵌 最小法术攻击力
	DECLARE( m11axmdamage, DWORD )		   //MYY 璀璨镶嵌 最大法术攻击力
	DECLARE( p11defence, DWORD )		   //MYY 璀璨镶嵌 物防
	DECLARE( m11defence, DWORD )		   //MYY 璀璨镶嵌 魔防

	DECLARE( m12axhp, DWORD )			   //MYY 神兽宝石增加 最大生命值
	DECLARE( p12damage, DWORD )		   //MYY 神兽宝石增加 最小攻击力
	DECLARE( m12axpdamage, DWORD )		   //MYY 神兽宝石增加 最大攻击力
	DECLARE( m12damage, DWORD )		   //MYY 神兽宝石增加 最小法术攻击力
	DECLARE( m12axmdamage, DWORD )		   //MYY 神兽宝石增加 最大法术攻击力
	DECLARE( p12defence, DWORD )		   //MYY 神兽宝石增加 物防
	DECLARE( m12defence, DWORD )		   //MYY 神兽宝石增加 魔防

	DECLARE( m13axhp, DWORD )			   //MYY 百兽图鉴进阶 最大生命值
	DECLARE( p13damage, DWORD )		   //MYY 百兽图鉴进阶 最小攻击力
	DECLARE( m13axpdamage, DWORD )		   //MYY 百兽图鉴进阶 最大攻击力
	DECLARE( m13damage, DWORD )		   //MYY 百兽图鉴进阶 最小法术攻击力
	DECLARE( m13axmdamage, DWORD )		   //MYY 百兽图鉴进阶 最大法术攻击力
	DECLARE( p13defence, DWORD )		   //MYY 百兽图鉴进阶 物防
	DECLARE( m13defence, DWORD )		   //MYY 百兽图鉴进阶 魔防

	DECLARE( m14axhp, DWORD )			   //MYY 龙王套装增加 最大生命值
	DECLARE( p14damage, DWORD )		   //MYY 龙王套装增加 最小攻击力
	DECLARE( m14axpdamage, DWORD )		   //MYY 龙王套装增加 最大攻击力
	DECLARE( m14damage, DWORD )		   //MYY 龙王套装增加 最小法术攻击力
	DECLARE( m14axmdamage, DWORD )		   //MYY 龙王套装增加 最大法术攻击力
	DECLARE( p14defence, DWORD )		   //MYY 龙王套装增加 物防
	DECLARE( m14defence, DWORD )		   //MYY 龙王套装增加 魔防

	DECLARE( m15axhp, DWORD )			   //MYY 逆天进阶增加 最大生命值
	DECLARE( p15damage, DWORD )		   //MYY 逆天进阶增加 最小攻击力
	DECLARE( m15axpdamage, DWORD )		   //MYY 逆天进阶增加 最大攻击力
	DECLARE( m15damage, DWORD )		   //MYY 逆天进阶增加 最小法术攻击力
	DECLARE( m15axmdamage, DWORD )		   //MYY 逆天进阶增加 最大法术攻击力
	DECLARE( p15defence, DWORD )		   //MYY 逆天进阶增加 物防
	DECLARE( m15defence, DWORD )		   //MYY 逆天进阶增加 魔防

	DECLARE( m16axhp, DWORD )			   //MYY 斗战圣佛增加 最大生命值
	DECLARE( p16damage, DWORD )		   //MYY 斗战圣佛增加 最小攻击力
	DECLARE( m16axpdamage, DWORD )		   //MYY 斗战圣佛增加 最大攻击力
	DECLARE( m16damage, DWORD )		   //MYY 斗战圣佛增加 最小法术攻击力
	DECLARE( m16axmdamage, DWORD )		   //MYY 斗战圣佛增加 最大法术攻击力
	DECLARE( p16defence, DWORD )		   //MYY 斗战圣佛增加 物防
	DECLARE( m16defence, DWORD )		   //MYY 斗战圣佛增加 魔防

	DECLARE( m17axhp, DWORD )			   //MYY 暗影石增加 最大生命值
	DECLARE( p17damage, DWORD )		   //MYY 暗影石增加 最小攻击力
	DECLARE( m17axpdamage, DWORD )		   //MYY 暗影石增加 最大攻击力
	DECLARE( m17damage, DWORD )		   //MYY 暗影石增加 最小法术攻击力
	DECLARE( m17axmdamage, DWORD )		   //MYY 暗影石增加 最大法术攻击力
	DECLARE( p17defence, DWORD )		   //MYY 暗影石增加 物防
	DECLARE( m17defence, DWORD )		   //MYY 暗影石增加 魔防

	DECLARE( m18axhp, DWORD )			   //MYY 尚方宝剑增加 最大生命值
	DECLARE( p18damage, DWORD )		   //MYY 尚方宝剑增加 最小攻击力
	DECLARE( m18axpdamage, DWORD )		   //MYY 尚方宝剑增加 最大攻击力
	DECLARE( m18damage, DWORD )		   //MYY 尚方宝剑增加 最小法术攻击力
	DECLARE( m18axmdamage, DWORD )		   //MYY 尚方宝剑增加 最大法术攻击力
	DECLARE( p18defence, DWORD )		   //MYY 尚方宝剑增加 物防
	DECLARE( m18defence, DWORD )		   //MYY 尚方宝剑增加 魔防

	DECLARE( m19axhp, DWORD )			   //MYY 紫金龙袍增加 最大生命值
	DECLARE( p19damage, DWORD )		   //MYY 紫金龙袍增加 最小攻击力
	DECLARE( m19axpdamage, DWORD )		   //MYY 紫金龙袍增加 最大攻击力
	DECLARE( m19damage, DWORD )		   //MYY 紫金龙袍增加 最小法术攻击力
	DECLARE( m19axmdamage, DWORD )		   //MYY 紫金龙袍增加 最大法术攻击力
	DECLARE( p19defence, DWORD )		   //MYY 紫金龙袍增加 物防
	DECLARE( m19defence, DWORD )		   //MYY 紫金龙袍增加 魔防	

	DECLARE( m20axhp, DWORD )			   //MYY 装备预留功能 最大生命值
	DECLARE( p20damage, DWORD )		   //MYY 装备预留功能 最小攻击力
	DECLARE( m20axpdamage, DWORD )		   //MYY 装备预留功能 最大攻击力
	DECLARE( m20damage, DWORD )		   //MYY 装备预留功能 最小法术攻击力
	DECLARE( m20axmdamage, DWORD )		   //MYY 装备预留功能 最大法术攻击力
	DECLARE( p20defence, DWORD )		   //MYY 装备预留功能 物防
	DECLARE( m20defence, DWORD )		   //MYY 装备预留功能 魔防	
	
	DECLARE(m21axhp, DWORD)     //MYY 装备预留功能 最大生命值
	DECLARE(p21damage, DWORD)    //MYY 装备预留功能 最小攻击力
	DECLARE(m21axpdamage, DWORD)    //MYY 装备预留功能 最大攻击力
	DECLARE(m21damage, DWORD)    //MYY 装备预留功能 最小法术攻击力
	DECLARE(m21axmdamage, DWORD)    //MYY 装备预留功能 最大法术攻击力
	DECLARE(p21defence, DWORD)    //MYY 装备预留功能 物防
	DECLARE(m21defence, DWORD)    //MYY 装备预留功能 魔防 

	DECLARE(m22axhp, DWORD)     //MYY 装备预留功能 最大生命值
	DECLARE(p22damage, DWORD)    //MYY 装备预留功能 最小攻击力
	DECLARE(m22axpdamage, DWORD)    //MYY 装备预留功能 最大攻击力
	DECLARE(m22damage, DWORD)    //MYY 装备预留功能 最小法术攻击力
	DECLARE(m22axmdamage, DWORD)    //MYY 装备预留功能 最大法术攻击力
	DECLARE(p22defence, DWORD)    //MYY 装备预留功能 物防
	DECLARE(m22defence, DWORD)    //MYY 装备预留功能 魔防 

	DECLARE(m23axhp, DWORD)     //MYY 装备预留功能 最大生命值
	DECLARE(p23damage, DWORD)    //MYY 装备预留功能 最小攻击力
	DECLARE(m23axpdamage, DWORD)    //MYY 装备预留功能 最大攻击力
	DECLARE(m23damage, DWORD)    //MYY 装备预留功能 最小法术攻击力
	DECLARE(m23axmdamage, DWORD)    //MYY 装备预留功能 最大法术攻击力
	DECLARE(p23defence, DWORD)    //MYY 装备预留功能 物防
	DECLARE(m23defence, DWORD)    //MYY 装备预留功能 魔防 

	DECLARE(m24axhp, DWORD)     //MYY 装备预留功能 最大生命值
	DECLARE(p24damage, DWORD)    //MYY 装备预留功能 最小攻击力
	DECLARE(m24axpdamage, DWORD)    //MYY 装备预留功能 最大攻击力
	DECLARE(m24damage, DWORD)    //MYY 装备预留功能 最小法术攻击力
	DECLARE(m24axmdamage, DWORD)    //MYY 装备预留功能 最大法术攻击力
	DECLARE(p24defence, DWORD)    //MYY 装备预留功能 物防
	DECLARE(m24defence, DWORD)    //MYY 装备预留功能 魔防 

	DECLARE(m25axhp, DWORD)     //MYY 装备预留功能 最大生命值
	DECLARE(p25damage, DWORD)    //MYY 装备预留功能 最小攻击力
	DECLARE(m25axpdamage, DWORD)    //MYY 装备预留功能 最大攻击力
	DECLARE(m25damage, DWORD)    //MYY 装备预留功能 最小法术攻击力
	DECLARE(m25axmdamage, DWORD)    //MYY 装备预留功能 最大法术攻击力
	DECLARE(p25defence, DWORD)    //MYY 装备预留功能 物防
	DECLARE(m25defence, DWORD)    //MYY 装备预留功能 魔防 

	DECLARE(m26axhp, DWORD)     //MYY 装备预留功能 最大生命值
	DECLARE(p26damage, DWORD)    //MYY 装备预留功能 最小攻击力
	DECLARE(m26axpdamage, DWORD)    //MYY 装备预留功能 最大攻击力
	DECLARE(m26damage, DWORD)    //MYY 装备预留功能 最小法术攻击力
	DECLARE(m26axmdamage, DWORD)    //MYY 装备预留功能 最大法术攻击力
	DECLARE(p26defence, DWORD)    //MYY 装备预留功能 物防
	DECLARE(m26defence, DWORD)    //MYY 装备预留功能 魔防 

	DECLARE(m27axhp, DWORD)     //MYY 装备预留功能 最大生命值
	DECLARE(p27damage, DWORD)    //MYY 装备预留功能 最小攻击力
	DECLARE(m27axpdamage, DWORD)    //MYY 装备预留功能 最大攻击力
	DECLARE(m27damage, DWORD)    //MYY 装备预留功能 最小法术攻击力
	DECLARE(m27axmdamage, DWORD)    //MYY 装备预留功能 最大法术攻击力
	DECLARE(p27defence, DWORD)    //MYY 装备预留功能 物防
	DECLARE(m27defence, DWORD)    //MYY 装备预留功能 魔防 

	DECLARE(m28axhp, DWORD)     //MYY 装备预留功能 最大生命值
	DECLARE(p28damage, DWORD)    //MYY 装备预留功能 最小攻击力
	DECLARE(m28axpdamage, DWORD)    //MYY 装备预留功能 最大攻击力
	DECLARE(m28damage, DWORD)    //MYY 装备预留功能 最小法术攻击力
	DECLARE(m28axmdamage, DWORD)    //MYY 装备预留功能 最大法术攻击力
	DECLARE(p28defence, DWORD)    //MYY 装备预留功能 物防
	DECLARE(m28defence, DWORD)    //MYY 装备预留功能 魔防 

	DECLARE(m29axhp, DWORD)     //MYY 装备预留功能 最大生命值
	DECLARE(p29damage, DWORD)    //MYY 装备预留功能 最小攻击力
	DECLARE(m29axpdamage, DWORD)    //MYY 装备预留功能 最大攻击力
	DECLARE(m29damage, DWORD)    //MYY 装备预留功能 最小法术攻击力
	DECLARE(m29axmdamage, DWORD)    //MYY 装备预留功能 最大法术攻击力
	DECLARE(p29defence, DWORD)    //MYY 装备预留功能 物防
	DECLARE(m29defence, DWORD)    //MYY 装备预留功能 魔防 

	DECLARE(m30axhp, DWORD)     //MYY 装备预留功能 最大生命值
	DECLARE(p30damage, DWORD)    //MYY 装备预留功能 最小攻击力
	DECLARE(m30axpdamage, DWORD)    //MYY 装备预留功能 最大攻击力
	DECLARE(m30damage, DWORD)    //MYY 装备预留功能 最小法术攻击力
	DECLARE(m30axmdamage, DWORD)    //MYY 装备预留功能 最大法术攻击力
	DECLARE(p30defence, DWORD)    //MYY 装备预留功能 物防
	DECLARE(m30defence, DWORD)    //MYY 装备预留功能 魔防 


	DECLARE(m31axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p31damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m31axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m31damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m31axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p31defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m31defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m32axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p32damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m32axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m32damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m32axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p32defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m32defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m33axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p33damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m33axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m33damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m33axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p33defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m33defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m34axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p34damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m34axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m34damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m34axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p34defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m34defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m35axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p35damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m35axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m35damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m35axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p35defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m35defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m36axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p36damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m36axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m36damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m36axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p36defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m36defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m37axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p37damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m37axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m37damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m37axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p37defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m37defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m38axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p38damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m38axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m38damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m38axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p38defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m38defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m39axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p39damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m39axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m39damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m39axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p39defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m39defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m40axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p40damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m40axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m40damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m40axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p40defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m40defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m41axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p41damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m41axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m41damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m41axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p41defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m41defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m42axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p42damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m42axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m42damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m42axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p42defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m42defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m43axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p43damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m43axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m43damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m43axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p43defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m43defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m44axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p44damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m44axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m44damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m44axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p44defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m44defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m45axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p45damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m45axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m45damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m45axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p45defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m45defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m46axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p46damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m46axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m46damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m46axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p46defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m46defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m47axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p47damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m47axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m47damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m47axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p47defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m47defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m48axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p48damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m48axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m48damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m48axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p48defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m48defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m49axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p49damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m49axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m49damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m49axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p49defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m49defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m50axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p50damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m50axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m50damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m50axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p50defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m50defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m51axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p51damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m51axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m51damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m51axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p51defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m51defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m52axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p52damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m52axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m52damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m52axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p52defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m52defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m53axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p53damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m53axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m53damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m53axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p53defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m53defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m54axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p54damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m54axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m54damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m54axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p54defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m54defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m55axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p55damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m55axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m55damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m55axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p55defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m55defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m56axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p56damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m56axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m56damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m56axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p56defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m56defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m57axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p57damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m57axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m57damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m57axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p57defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m57defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m58axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p58damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m58axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m58damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m58axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p58defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m58defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m59axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p59damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m59axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m59damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m59axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p59defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m59defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m60axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p60damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m60axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m60damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m60axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p60defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m60defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m61axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p61damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m61axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m61damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m61axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p61defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m61defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m62axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p62damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m62axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m62damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m62axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p62defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m62defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m63axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p63damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m63axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m63damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m63axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p63defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m63defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m64axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p64damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m64axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m64damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m64axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p64defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m64defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m65axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p65damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m65axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m65damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m65axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p65defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m65defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m66axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p66damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m66axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m66damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m66axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p66defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m66defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m67axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p67damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m67axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m67damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m67axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p67defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m67defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m68axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p68damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m68axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m68damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m68axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p68defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m68defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m69axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p69damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m69axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m69damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m69axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p69defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m69defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m70axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p70damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m70axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m70damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m70axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p70defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m70defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m71axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p71damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m71axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m71damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m71axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p71defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m71defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m72axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p72damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m72axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m72damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m72axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p72defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m72defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m73axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p73damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m73axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m73damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m73axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p73defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m73defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m74axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p74damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m74axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m74damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m74axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p74defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m74defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m75axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p75damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m75axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m75damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m75axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p75defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m75defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m76axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p76damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m76axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m76damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m76axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p76defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m76defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m77axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p77damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m77axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m77damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m77axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p77defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m77defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m78axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p78damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m78axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m78damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m78axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p78defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m78defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m79axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p79damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m79axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m79damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m79axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p79defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m79defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m80axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p80damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m80axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m80damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m80axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p80defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m80defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m81axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p81damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m81axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m81damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m81axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p81defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m81defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m82axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p82damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m82axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m82damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m82axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p82defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m82defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m83axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p83damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m83axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m83damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m83axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p83defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m83defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m84axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p84damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m84axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m84damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m84axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p84defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m84defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m85axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p85damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m85axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m85damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m85axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p85defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m85defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m86axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p86damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m86axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m86damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m86axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p86defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m86defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m87axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p87damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m87axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m87damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m87axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p87defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m87defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m88axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p88damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m88axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m88damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m88axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p88defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m88defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m89axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p89damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m89axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m89damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m89axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p89defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m89defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m90axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p90damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m90axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m90damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m90axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p90defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m90defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m91axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p91damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m91axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m91damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m91axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p91defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m91defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m92axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p92damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m92axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m92damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m92axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p92defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m92defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m93axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p93damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m93axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m93damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m93axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p93defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m93defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m94axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p94damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m94axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m94damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m94axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p94defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m94defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m95axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p95damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m95axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m95damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m95axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p95defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m95defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m96axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p96damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m96axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m96damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m96axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p96defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m96defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m97axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p97damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m97axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m97damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m97axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p97defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m97defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m98axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p98damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m98axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m98damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m98axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p98defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m98defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m99axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p99damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m99axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m99damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m99axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p99defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m99defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m100axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p100damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m100axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m100damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m100axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p100defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m100defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m101axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p101damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m101axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m101damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m101axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p101defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m101defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m102axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p102damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m102axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m102damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m102axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p102defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m102defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m103axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p103damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m103axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m103damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m103axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p103defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m103defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m104axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p104damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m104axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m104damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m104axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p104defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m104defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m105axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p105damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m105axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m105damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m105axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p105defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m105defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m106axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p106damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m106axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m106damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m106axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p106defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m106defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m107axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p107damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m107axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m107damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m107axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p107defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m107defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m108axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p108damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m108axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m108damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m108axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p108defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m108defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m109axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p109damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m109axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m109damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m109axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p109defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m109defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m110axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p110damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m110axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m110damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m110axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p110defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m110defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m111axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p111damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m111axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m111damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m111axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p111defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m111defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m112axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p112damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m112axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m112damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m112axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p112defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m112defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m113axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p113damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m113axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m113damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m113axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p113defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m113defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m114axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p114damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m114axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m114damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m114axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p114defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m114defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m115axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p115damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m115axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m115damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m115axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p115defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m115defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m116axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p116damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m116axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m116damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m116axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p116defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m116defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m117axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p117damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m117axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m117damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m117axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p117defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m117defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m118axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p118damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m118axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m118damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m118axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p118defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m118defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m119axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p119damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m119axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m119damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m119axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p119defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m119defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m120axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p120damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m120axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m120damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m120axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p120defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m120defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m121axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p121damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m121axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m121damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m121axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p121defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m121defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m122axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p122damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m122axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m122damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m122axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p122defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m122defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m123axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p123damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m123axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m123damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m123axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p123defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m123defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m124axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p124damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m124axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m124damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m124axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p124defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m124defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m125axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p125damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m125axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m125damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m125axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p125defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m125defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m126axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p126damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m126axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m126damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m126axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p126defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m126defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m127axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p127damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m127axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m127damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m127axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p127defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m127defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m128axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p128damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m128axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m128damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m128axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p128defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m128defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(m129axhp, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(p129damage, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m129axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m129damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m129axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p129defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m129defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(hongtiannum8, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(hongtiannum9, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(m130axpdamage, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(m130damage, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(m130axmdamage, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(p130defence, DWORD)    //ZM 装备预留功能 物防
	DECLARE(m130defence, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE(hongtiannum1, DWORD)     //ZM 装备预留功能 最大生命值
	DECLARE(hongtiannum2, DWORD)    //ZM 装备预留功能 最小攻击力
	DECLARE(hongtiannum3, DWORD)    //ZM 装备预留功能 最大攻击力
	DECLARE(hongtiannum4, DWORD)    //ZM 装备预留功能 最小法术攻击力
	DECLARE(hongtiannum5, DWORD)    //ZM 装备预留功能 最大法术攻击力
	DECLARE(hongtiannum6, DWORD)    //ZM 装备预留功能 物防
	DECLARE(hongtiannum7, DWORD)    //ZM 装备预留功能 魔防 

	DECLARE( damagebonus, BYTE )		// 伤害加成
	DECLARE( damage, BYTE )				// 增加伤害值x％
	
	DECLARE( akspeed, WORD )			// 攻击速度
	DECLARE( mvspeed, WORD )			// 移动速度
	DECLARE( atrating, WORD )			// 命中率
	DECLARE( akdodge, WORD )			// 躲避率

	DECLARE( str, WORD )  				// 力量
	DECLARE( inte, WORD ) 				// 智力
	DECLARE( dex, WORD ) 				// 敏捷
	DECLARE( spi, WORD )  				// 精神
	DECLARE( con, WORD )  				// 体质

	DECLARE( xstr, WORD )  				// 力量-魂魄
	DECLARE( xinte, WORD ) 				// 智力-魂魄
	DECLARE( xdex, WORD ) 				// 敏捷-魂魄
	DECLARE( xspi, WORD )  				// 精神-魂魄
	DECLARE( xcon, WORD )  				// 体质-魂魄
	
	DECLARE( hpr, WORD )  				// 生命值恢复
	DECLARE( mpr, WORD )  				// 法术值恢复
	DECLARE( spr, WORD )  				// 体力值恢复
	
	DECLARE( holy, WORD )				//神圣一击
	DECLARE( bang, WORD )				//重击率
	DECLARE( pdam, WORD )  				// 增加物理攻击力
	DECLARE( pdef, WORD )  				// 增加物理防御力
	DECLARE( mdam, WORD )  				// 增加魔法攻击力
	DECLARE( mdef, WORD )  				// 增加魔法防御力
	
	DECLARE( poisondef, WORD ) 			//抗毒增加
	DECLARE( lulldef, WORD ) 			//抗麻痹增加
	DECLARE( reeldef, WORD ) 			//抗眩晕增加
	DECLARE( evildef, WORD ) 			//抗噬魔增加
	DECLARE( bitedef, WORD ) 			//抗噬力增加
	DECLARE( chaosdef, WORD ) 			//抗混乱增加
	DECLARE( colddef, WORD ) 			//抗冰冻增加
	DECLARE( petrifydef, WORD ) 		//抗石化增加
	DECLARE( blinddef, WORD ) 			//抗失明增加
	DECLARE( stabledef, WORD ) 			//抗定身增加
	DECLARE( slowdef, WORD ) 			//抗减速增加
	DECLARE( luredef, WORD ) 			//抗诱惑增加
		
	DECLARE( poison, WORD ) 			//中毒增加
	DECLARE( lull, WORD ) 				//麻痹增加
	DECLARE( reel, WORD ) 				//眩晕增加
	DECLARE( evil, WORD ) 				//噬魔增加
	DECLARE( bite, WORD ) 				//噬力增加
	DECLARE( chaos, WORD ) 				//混乱增加
	DECLARE( cold, WORD ) 				//冰冻增加
	DECLARE( petrify, WORD ) 			//石化增加
	DECLARE( blind, WORD ) 				//失明增加
	DECLARE( stable, WORD ) 			//定身增加
	DECLARE( slow, WORD ) 				//减速增加
	DECLARE( lure, WORD ) 				//诱惑增加

	DECLARE( hptomp, BYTE ) 			//转换生命值为法术值x％
	DECLARE( dhpp, BYTE ) 				//物理伤害减少x%	
	DECLARE( dmpp, BYTE ) 				//法术伤害值减少x%		

	DECLARE( incgold, BYTE ) 			//增加金钱掉落x%
	DECLARE( doublexp, BYTE ) 			//x%双倍经验		
	DECLARE( mf, BYTE ) 				//增加掉宝率x%

	//五行套装相关属性
	DECLARE(dpdam, BYTE)			//物理伤害减少%x
	DECLARE(dmdam, BYTE)			//法术伤害减少%x
	DECLARE(bdam, BYTE)				//增加伤害x%
	DECLARE(rdam, BYTE)				//伤害反射%x
	DECLARE(ignoredef, BYTE)		//%x忽视目标防御
	//soke 灵魂锁链
	DECLARE(addpdef, BYTE)			//物理防御力增强xxx
	DECLARE(addmdef, BYTE)			//魔法防御力增强xxx
	DECLARE(addpdam, BYTE)			//增加物理攻击力xxxx
	DECLARE(addmdam, BYTE)			//增加魔法攻击力xxxx
	DECLARE(addhp, BYTE)			//增加生命值xxxxx
	DECLARE(maxhprate, WORD)		//按百分比增加最大hp
	DECLARE(maxmprate, WORD)		//按百分比增加最大mp

	BYTE get_hpleech_odds()		const	{return hpleech.odds;}
	WORD get_hpleech_effect()	const	{return hpleech.effect;}
	BYTE get_mpleech_odds()		const	{return mpleech.odds;} 			//x%吸收生命值y
	WORD get_mpleech_effect()	const	{return mpleech.effect;} 		//x%吸收法术值y

	/**
	 * \brief 获取攻击五行
	 * \return 攻击五行类型
	 */
	DWORD getAttFive() const {return aftype;}

	/**
	 * \brief 获取攻击五行点数
	 * \return 五行点数
	 */
	DWORD getAttFivePoint() const {return afpoint;}

	/**
	 * \brief 获取防御五行
	 * \return 防御五行类型
	 */
	DWORD getDefFive() const {return dftype;}

	/**
	 * \brief 获取防御五行点数
	 * \return 五行点数
	 */
	DWORD getDefFivePoint() const {return dfpoint;}
	
	/**
	 * \brief 获取装备对单个技能的加成
	 *
	 * \param id: 技能id
	 * \return 加成等级
	 */	
	DWORD getMaxSkill(WORD id) const
	{
		const_iterator it = skill.find(id);
		if (it != skill.end()) {
			return it->second;
		}
		
		return 0;		
	}
	
	/**
	 * \brief 获取装备对某系技能的加成
	 *
	 * \param id: 技能系别
	 * \return 加成等级
	 */	
	DWORD getMaxSkills(WORD id) const
	{
		const_iterator it = skills.find(id);
		if (it != skills.end()) {
			return it->second;
		}
		
		return 0;		
	}

private:
	friend class EquipPack;
	DWORD maxhp;					// 最大生命值
	DWORD maxmp;					// 最大法术值
	DWORD maxsp;					// 最大体力值

	DWORD pdamage;				// 最小攻击力
	DWORD maxpdamage;			// 最大攻击力
	DWORD mdamage;				// 最小法术攻击力
	DWORD maxmdamage;			// 最大法术攻击力
	WORD appendminpet;         // 给宠召唤兽能力增强最小值
	WORD appendmaxpet;         // 给宠召唤兽能力增强最大值

	DWORD pdefence;				// 物防
	DWORD mdefence;				// 魔防
	
	DWORD m1axhp;				// 最大生命值
	DWORD p1damage;				// 最小攻击力
	DWORD m1axpdamage;			// 最大攻击力
	DWORD m1damage;				// 最小法术攻击力
	DWORD m1axmdamage;			// 最大法术攻击力
	DWORD p1defence;			// 物防
	DWORD m1defence;			// 魔防

	DWORD m2axhp;				// 最大生命值
	DWORD p2damage;				// 最小攻击力
	DWORD m2axpdamage;			// 最大攻击力
	DWORD m2damage;				// 最小法术攻击力
	DWORD m2axmdamage;			// 最大法术攻击力
	DWORD p2defence;			// 物防
	DWORD m2defence;			// 魔防

	DWORD m3axhp;				// 斗魂最大生命值
	DWORD p3damage;				// 斗魂最小攻击力
	DWORD m3axpdamage;			// 斗魂最大攻击力
	DWORD m3damage;				// 斗魂最小法术攻击力
	DWORD m3axmdamage;			// 斗魂最大法术攻击力
	DWORD p3defence;			// 斗魂物防
	DWORD m3defence;			// 斗魂魔防
	
	DWORD m4axhp;				// 龙槽*最大生命值
	DWORD p4damage;				// 龙槽*最小攻击力
	DWORD m4axpdamage;			// 龙槽*最大攻击力
	DWORD m4damage;				// 龙槽*最小法术攻击力
	DWORD m4axmdamage;			// 龙槽*最大法术攻击力
	DWORD p4defence;			// 龙槽*物防
	DWORD m4defence;			// 龙槽*魔防
	
	DWORD m5axhp;				// 龙*最大生命值
	DWORD p5damage;				// 龙*最小攻击力
	DWORD m5axpdamage;			// 龙*最大攻击力
	DWORD m5damage;				// 龙*最小法术攻击力
	DWORD m5axmdamage;			// 龙*最大法术攻击力
	DWORD p5defence;			// 龙*物防
	DWORD m5defence;			// 龙*魔防
	
	DWORD m6axhp;				// 补天最大生命值
	DWORD p6damage;				// 补天最小攻击力
	DWORD m6axpdamage;			// 补天最大攻击力
	DWORD m6damage;				// 补天最小法术攻击力
	DWORD m6axmdamage;			// 补天最大法术攻击力
	DWORD p6defence;			// 补天物防
	DWORD m6defence;			// 补天魔防
	
	DWORD m7axhp;				// 预留最大生命值
	DWORD p7damage;				// 预留最小攻击力
	DWORD m7axpdamage;			// 预留最大攻击力
	DWORD m7damage;				// 预留最小法术攻击力
	DWORD m7axmdamage;			// 预留最大法术攻击力
	DWORD p7defence;			// 预留物防
	DWORD m7defence;			// 预留魔防
	
	DWORD m8axhp;				// 预留最大生命值
	DWORD p8damage;				// 预留最小攻击力
	DWORD m8axpdamage;			// 预留最大攻击力
	DWORD m8damage;				// 预留最小法术攻击力
	DWORD m8axmdamage;			// 预留最大法术攻击力
	DWORD p8defence;			// 预留物防
	DWORD m8defence;			// 预留魔防
	
	DWORD m9axhp;				// 预留最大生命值
	DWORD p9damage;				// 预留最小攻击力
	DWORD m9axpdamage;			// 预留最大攻击力
	DWORD m9damage;				// 预留最小法术攻击力
	DWORD m9axmdamage;			// 预留最大法术攻击力
	DWORD p9defence;			// 预留物防
	DWORD m9defence;			// 预留魔防

	DWORD m10axhp;			//sky 马牌精炼 最大生命值
	DWORD p10damage;		//sky 马牌精炼 最小攻击力
	DWORD m10axpdamage;		//sky 马牌精炼 最大攻击力
	DWORD m10damage;		//sky 马牌精炼 最小法术攻击力
	DWORD m10axmdamage;		//sky 马牌精炼 最大法术攻击力
	DWORD p10defence;		//sky 马牌精炼 物防
	DWORD m10defence;		//sky 马牌精炼 魔防

	DWORD m11axhp;			   //MYY 璀璨镶嵌 最大生命值
	DWORD p11damage;		   //MYY 璀璨镶嵌 最小攻击力
	DWORD m11axpdamage;		   //MYY 璀璨镶嵌 最大攻击力
	DWORD m11damage;		   //MYY 璀璨镶嵌 最小法术攻击力
	DWORD m11axmdamage;		   //MYY 璀璨镶嵌 最大法术攻击力
	DWORD p11defence;		   //MYY 璀璨镶嵌 物防
	DWORD m11defence;		   //MYY 璀璨镶嵌 魔防

	DWORD m12axhp;			   //MYY 神兽宝石增加 最大生命值
	DWORD p12damage;		   //MYY 神兽宝石增加 最小攻击力
	DWORD m12axpdamage;		   //MYY 神兽宝石增加 最大攻击力
	DWORD m12damage;		   //MYY 神兽宝石增加 最小法术攻击力
	DWORD m12axmdamage;		   //MYY 神兽宝石增加 最大法术攻击力
	DWORD p12defence;		   //MYY 神兽宝石增加 物防
	DWORD m12defence;		   //MYY 朱雀石增加 魔防

	DWORD m13axhp;			   //MYY 装备预留功能 最大生命值
	DWORD p13damage;		   //MYY 装备预留功能 最小攻击力
	DWORD m13axpdamage;		   //MYY 装备预留功能 最大攻击力
	DWORD m13damage;		   //MYY 装备预留功能 最小法术攻击力
	DWORD m13axmdamage;		   //MYY 装备预留功能 最大法术攻击力
	DWORD p13defence;		   //MYY 装备预留功能 物防
	DWORD m13defence;		   //MYY 装备预留功能 魔防

	DWORD m14axhp;			   //MYY 装备预留功能 最大生命值
	DWORD p14damage;		   //MYY 装备预留功能 最小攻击力
	DWORD m14axpdamage;		   //MYY 装备预留功能 最大攻击力
	DWORD m14damage;		   //MYY 装备预留功能 最小法术攻击力
	DWORD m14axmdamage;		   //MYY 装备预留功能 最大法术攻击力
	DWORD p14defence;		   //MYY 装备预留功能 物防
	DWORD m14defence;		   //MYY 装备预留功能 魔防

	DWORD m15axhp;			   //MYY 装备预留功能 最大生命值
	DWORD p15damage;		   //MYY 装备预留功能 最小攻击力
	DWORD m15axpdamage;		   //MYY 装备预留功能 最大攻击力
	DWORD m15damage;		   //MYY 装备预留功能 最小法术攻击力
	DWORD m15axmdamage;		   //MYY 装备预留功能 最大法术攻击力
	DWORD p15defence;		   //MYY 装备预留功能 物防
	DWORD m15defence;		   //MYY 装备预留功能 魔防

	DWORD m16axhp;			   //MYY 装备预留功能 最大生命值
	DWORD p16damage;		   //MYY 装备预留功能 最小攻击力
	DWORD m16axpdamage;		   //MYY 装备预留功能 最大攻击力
	DWORD m16damage;		   //MYY 装备预留功能 最小法术攻击力
	DWORD m16axmdamage;		   //MYY 装备预留功能 最大法术攻击力
	DWORD p16defence;		   //MYY 装备预留功能 物防
	DWORD m16defence;		   //MYY 装备预留功能 魔防

	DWORD m17axhp;			   //MYY 装备预留功能 最大生命值
	DWORD p17damage;		   //MYY 装备预留功能 最小攻击力
	DWORD m17axpdamage;		   //MYY 装备预留功能 最大攻击力
	DWORD m17damage;		   //MYY 装备预留功能 最小法术攻击力
	DWORD m17axmdamage;		   //MYY 装备预留功能 最大法术攻击力
	DWORD p17defence;		   //MYY 装备预留功能 物防
	DWORD m17defence;		   //MYY 装备预留功能 魔防

	DWORD m18axhp;			   //MYY 装备预留功能 最大生命值
	DWORD p18damage;		   //MYY 装备预留功能 最小攻击力
	DWORD m18axpdamage;		   //MYY 装备预留功能 最大攻击力
	DWORD m18damage;		   //MYY 装备预留功能 最小法术攻击力
	DWORD m18axmdamage;		   //MYY 装备预留功能 最大法术攻击力
	DWORD p18defence;		   //MYY 装备预留功能 物防
	DWORD m18defence;		   //MYY 装备预留功能 魔防

	DWORD m19axhp;			   //MYY 装备预留功能 最大生命值
	DWORD p19damage;		   //MYY 装备预留功能 最小攻击力
	DWORD m19axpdamage;		   //MYY 装备预留功能 最大攻击力
	DWORD m19damage;		   //MYY 装备预留功能 最小法术攻击力
	DWORD m19axmdamage;		   //MYY 装备预留功能 最大法术攻击力
	DWORD p19defence;		   //MYY 装备预留功能 物防
	DWORD m19defence;		   //MYY 装备预留功能 魔防	

	DWORD m20axhp;			   //MYY 装备预留功能 最大生命值
	DWORD p20damage;		   //MYY 装备预留功能 最小攻击力
	DWORD m20axpdamage;		   //MYY 装备预留功能 最大攻击力
	DWORD m20damage;		   //MYY 装备预留功能 最小法术攻击力
	DWORD m20axmdamage;		   //MYY 装备预留功能 最大法术攻击力
	DWORD p20defence;		   //MYY 装备预留功能 物防
	DWORD m20defence;		   //MYY 装备预留功能 魔防	

	
	DWORD m21axhp;			   //MYY 十二星宿 最大生命值
	DWORD p21damage;		   //MYY 十二星宿 最小攻击力
	DWORD m21axpdamage;		   //MYY 十二星宿 最大攻击力
	DWORD m21damage;		   //MYY 十二星宿 最小法术攻击力
	DWORD m21axmdamage;		   //MYY 十二星宿 最大法术攻击力
	DWORD p21defence;		   //MYY 十二星宿 物防
	DWORD m21defence;		   //MYY 十二星宿 魔防

	DWORD m22axhp;			   //MYY 预留 最大生命值
	DWORD p22damage;		   //MYY 预留 最小攻击力
	DWORD m22axpdamage;		   //MYY 预留 最大攻击力
	DWORD m22damage;		   //MYY 预留 最小法术攻击力
	DWORD m22axmdamage;		   //MYY 预留 最大法术攻击力
	DWORD p22defence;		   //MYY 预留 物防
	DWORD m22defence;		   //MYY 预留 魔防

	DWORD m23axhp;      //MYY 预留 最大生命值
	DWORD p23damage;      //MYY 预留 最小攻击力
	DWORD m23axpdamage;      //MYY 预留 最大攻击力
	DWORD m23damage;     //MYY 预留 最小法术攻击力
	DWORD m23axmdamage;     //MYY 预留 最大法术攻击力
	DWORD p23defence;     //MYY 预留 物防
	DWORD m23defence;     //MYY 预留 魔防

	DWORD m24axhp;      //MYY 预留 最大生命值
	DWORD p24damage;      //MYY 预留 最小攻击力
	DWORD m24axpdamage;      //MYY 预留 最大攻击力
	DWORD m24damage;     //MYY 预留 最小法术攻击力
	DWORD m24axmdamage;     //MYY 预留 最大法术攻击力
	DWORD p24defence;     //MYY 预留 物防
	DWORD m24defence;     //MYY 预留 魔防

	DWORD m25axhp;      //MYY 预留 最大生命值
	DWORD p25damage;      //MYY 预留 最小攻击力
	DWORD m25axpdamage;      //MYY 预留 最大攻击力
	DWORD m25damage;     //MYY 预留 最小法术攻击力
	DWORD m25axmdamage;     //MYY 预留 最大法术攻击力
	DWORD p25defence;     //MYY 预留 物防
	DWORD m25defence;     //MYY 预留 魔防

	DWORD m26axhp;      //MYY 预留 最大生命值
	DWORD p26damage;      //MYY 预留 最小攻击力
	DWORD m26axpdamage;      //MYY 预留 最大攻击力
	DWORD m26damage;     //MYY 预留 最小法术攻击力
	DWORD m26axmdamage;     //MYY 预留 最大法术攻击力
	DWORD p26defence;     //MYY 预留 物防
	DWORD m26defence;     //MYY 预留 魔防

	DWORD m27axhp;      //MYY 预留 最大生命值
	DWORD p27damage;      //MYY 预留 最小攻击力
	DWORD m27axpdamage;      //MYY 预留 最大攻击力
	DWORD m27damage;     //MYY 预留 最小法术攻击力
	DWORD m27axmdamage;     //MYY 预留 最大法术攻击力
	DWORD p27defence;     //MYY 预留 物防
	DWORD m27defence;     //MYY 预留 魔防

	DWORD m28axhp;      //MYY 预留 最大生命值
	DWORD p28damage;      //MYY 预留 最小攻击力
	DWORD m28axpdamage;      //MYY 预留 最大攻击力
	DWORD m28damage;     //MYY 预留 最小法术攻击力
	DWORD m28axmdamage;     //MYY 预留 最大法术攻击力
	DWORD p28defence;     //MYY 预留 物防
	DWORD m28defence;     //MYY 预留 魔防

	DWORD m29axhp;      //MYY 预留 最大生命值
	DWORD p29damage;      //MYY 预留 最小攻击力
	DWORD m29axpdamage;      //MYY 预留 最大攻击力
	DWORD m29damage;     //MYY 预留 最小法术攻击力
	DWORD m29axmdamage;     //MYY 预留 最大法术攻击力
	DWORD p29defence;     //MYY 预留 物防
	DWORD m29defence;     //MYY 预留 魔防

	DWORD m30axhp;      //MYY 预留 最大生命值
	DWORD p30damage;      //MYY 预留 最小攻击力
	DWORD m30axpdamage;      //MYY 预留 最大攻击力
	DWORD m30damage;     //MYY 预留 最小法术攻击力
	DWORD m30axmdamage;     //MYY 预留 最大法术攻击力
	DWORD p30defence;     //MYY 预留 物防
	DWORD m30defence;     //MYY 预留 魔防


	DWORD m31axhp;      //ZM 预留 最大生命值
	DWORD p31damage;      //ZM 预留 最小攻击力
	DWORD m31axpdamage;      //ZM 预留 最大攻击力
	DWORD m31damage;     //ZM 预留 最小法术攻击力
	DWORD m31axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p31defence;     //ZM 预留 物防
	DWORD m31defence;     //ZM 预留 魔防

	DWORD m32axhp;      //ZM 预留 最大生命值
	DWORD p32damage;      //ZM 预留 最小攻击力
	DWORD m32axpdamage;      //ZM 预留 最大攻击力
	DWORD m32damage;     //ZM 预留 最小法术攻击力
	DWORD m32axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p32defence;     //ZM 预留 物防
	DWORD m32defence;     //ZM 预留 魔防

	DWORD m33axhp;      //ZM 预留 最大生命值
	DWORD p33damage;      //ZM 预留 最小攻击力
	DWORD m33axpdamage;      //ZM 预留 最大攻击力
	DWORD m33damage;     //ZM 预留 最小法术攻击力
	DWORD m33axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p33defence;     //ZM 预留 物防
	DWORD m33defence;     //ZM 预留 魔防

	DWORD m34axhp;      //ZM 预留 最大生命值
	DWORD p34damage;      //ZM 预留 最小攻击力
	DWORD m34axpdamage;      //ZM 预留 最大攻击力
	DWORD m34damage;     //ZM 预留 最小法术攻击力
	DWORD m34axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p34defence;     //ZM 预留 物防
	DWORD m34defence;     //ZM 预留 魔防

	DWORD m35axhp;      //ZM 预留 最大生命值
	DWORD p35damage;      //ZM 预留 最小攻击力
	DWORD m35axpdamage;      //ZM 预留 最大攻击力
	DWORD m35damage;     //ZM 预留 最小法术攻击力
	DWORD m35axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p35defence;     //ZM 预留 物防
	DWORD m35defence;     //ZM 预留 魔防

	DWORD m36axhp;      //ZM 预留 最大生命值
	DWORD p36damage;      //ZM 预留 最小攻击力
	DWORD m36axpdamage;      //ZM 预留 最大攻击力
	DWORD m36damage;     //ZM 预留 最小法术攻击力
	DWORD m36axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p36defence;     //ZM 预留 物防
	DWORD m36defence;     //ZM 预留 魔防

	DWORD m37axhp;      //ZM 预留 最大生命值
	DWORD p37damage;      //ZM 预留 最小攻击力
	DWORD m37axpdamage;      //ZM 预留 最大攻击力
	DWORD m37damage;     //ZM 预留 最小法术攻击力
	DWORD m37axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p37defence;     //ZM 预留 物防
	DWORD m37defence;     //ZM 预留 魔防

	DWORD m38axhp;      //ZM 预留 最大生命值
	DWORD p38damage;      //ZM 预留 最小攻击力
	DWORD m38axpdamage;      //ZM 预留 最大攻击力
	DWORD m38damage;     //ZM 预留 最小法术攻击力
	DWORD m38axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p38defence;     //ZM 预留 物防
	DWORD m38defence;     //ZM 预留 魔防

	DWORD m39axhp;      //ZM 预留 最大生命值
	DWORD p39damage;      //ZM 预留 最小攻击力
	DWORD m39axpdamage;      //ZM 预留 最大攻击力
	DWORD m39damage;     //ZM 预留 最小法术攻击力
	DWORD m39axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p39defence;     //ZM 预留 物防
	DWORD m39defence;     //ZM 预留 魔防

	DWORD m40axhp;      //ZM 预留 最大生命值
	DWORD p40damage;      //ZM 预留 最小攻击力
	DWORD m40axpdamage;      //ZM 预留 最大攻击力
	DWORD m40damage;     //ZM 预留 最小法术攻击力
	DWORD m40axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p40defence;     //ZM 预留 物防
	DWORD m40defence;     //ZM 预留 魔防

	DWORD m41axhp;      //ZM 预留 最大生命值
	DWORD p41damage;      //ZM 预留 最小攻击力
	DWORD m41axpdamage;      //ZM 预留 最大攻击力
	DWORD m41damage;     //ZM 预留 最小法术攻击力
	DWORD m41axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p41defence;     //ZM 预留 物防
	DWORD m41defence;     //ZM 预留 魔防

	DWORD m42axhp;      //ZM 预留 最大生命值
	DWORD p42damage;      //ZM 预留 最小攻击力
	DWORD m42axpdamage;      //ZM 预留 最大攻击力
	DWORD m42damage;     //ZM 预留 最小法术攻击力
	DWORD m42axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p42defence;     //ZM 预留 物防
	DWORD m42defence;     //ZM 预留 魔防

	DWORD m43axhp;      //ZM 预留 最大生命值
	DWORD p43damage;      //ZM 预留 最小攻击力
	DWORD m43axpdamage;      //ZM 预留 最大攻击力
	DWORD m43damage;     //ZM 预留 最小法术攻击力
	DWORD m43axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p43defence;     //ZM 预留 物防
	DWORD m43defence;     //ZM 预留 魔防

	DWORD m44axhp;      //ZM 预留 最大生命值
	DWORD p44damage;      //ZM 预留 最小攻击力
	DWORD m44axpdamage;      //ZM 预留 最大攻击力
	DWORD m44damage;     //ZM 预留 最小法术攻击力
	DWORD m44axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p44defence;     //ZM 预留 物防
	DWORD m44defence;     //ZM 预留 魔防

	DWORD m45axhp;      //ZM 预留 最大生命值
	DWORD p45damage;      //ZM 预留 最小攻击力
	DWORD m45axpdamage;      //ZM 预留 最大攻击力
	DWORD m45damage;     //ZM 预留 最小法术攻击力
	DWORD m45axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p45defence;     //ZM 预留 物防
	DWORD m45defence;     //ZM 预留 魔防

	DWORD m46axhp;      //ZM 预留 最大生命值
	DWORD p46damage;      //ZM 预留 最小攻击力
	DWORD m46axpdamage;      //ZM 预留 最大攻击力
	DWORD m46damage;     //ZM 预留 最小法术攻击力
	DWORD m46axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p46defence;     //ZM 预留 物防
	DWORD m46defence;     //ZM 预留 魔防

	DWORD m47axhp;      //ZM 预留 最大生命值
	DWORD p47damage;      //ZM 预留 最小攻击力
	DWORD m47axpdamage;      //ZM 预留 最大攻击力
	DWORD m47damage;     //ZM 预留 最小法术攻击力
	DWORD m47axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p47defence;     //ZM 预留 物防
	DWORD m47defence;     //ZM 预留 魔防

	DWORD m48axhp;      //ZM 预留 最大生命值
	DWORD p48damage;      //ZM 预留 最小攻击力
	DWORD m48axpdamage;      //ZM 预留 最大攻击力
	DWORD m48damage;     //ZM 预留 最小法术攻击力
	DWORD m48axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p48defence;     //ZM 预留 物防
	DWORD m48defence;     //ZM 预留 魔防

	DWORD m49axhp;      //ZM 预留 最大生命值
	DWORD p49damage;      //ZM 预留 最小攻击力
	DWORD m49axpdamage;      //ZM 预留 最大攻击力
	DWORD m49damage;     //ZM 预留 最小法术攻击力
	DWORD m49axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p49defence;     //ZM 预留 物防
	DWORD m49defence;     //ZM 预留 魔防

	DWORD m50axhp;      //ZM 预留 最大生命值
	DWORD p50damage;      //ZM 预留 最小攻击力
	DWORD m50axpdamage;      //ZM 预留 最大攻击力
	DWORD m50damage;     //ZM 预留 最小法术攻击力
	DWORD m50axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p50defence;     //ZM 预留 物防
	DWORD m50defence;     //ZM 预留 魔防

	DWORD m51axhp;      //ZM 预留 最大生命值
	DWORD p51damage;      //ZM 预留 最小攻击力
	DWORD m51axpdamage;      //ZM 预留 最大攻击力
	DWORD m51damage;     //ZM 预留 最小法术攻击力
	DWORD m51axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p51defence;     //ZM 预留 物防
	DWORD m51defence;     //ZM 预留 魔防

	DWORD m52axhp;      //ZM 预留 最大生命值
	DWORD p52damage;      //ZM 预留 最小攻击力
	DWORD m52axpdamage;      //ZM 预留 最大攻击力
	DWORD m52damage;     //ZM 预留 最小法术攻击力
	DWORD m52axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p52defence;     //ZM 预留 物防
	DWORD m52defence;     //ZM 预留 魔防

	DWORD m53axhp;      //ZM 预留 最大生命值
	DWORD p53damage;      //ZM 预留 最小攻击力
	DWORD m53axpdamage;      //ZM 预留 最大攻击力
	DWORD m53damage;     //ZM 预留 最小法术攻击力
	DWORD m53axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p53defence;     //ZM 预留 物防
	DWORD m53defence;     //ZM 预留 魔防

	DWORD m54axhp;      //ZM 预留 最大生命值
	DWORD p54damage;      //ZM 预留 最小攻击力
	DWORD m54axpdamage;      //ZM 预留 最大攻击力
	DWORD m54damage;     //ZM 预留 最小法术攻击力
	DWORD m54axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p54defence;     //ZM 预留 物防
	DWORD m54defence;     //ZM 预留 魔防

	DWORD m55axhp;      //ZM 预留 最大生命值
	DWORD p55damage;      //ZM 预留 最小攻击力
	DWORD m55axpdamage;      //ZM 预留 最大攻击力
	DWORD m55damage;     //ZM 预留 最小法术攻击力
	DWORD m55axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p55defence;     //ZM 预留 物防
	DWORD m55defence;     //ZM 预留 魔防

	DWORD m56axhp;      //ZM 预留 最大生命值
	DWORD p56damage;      //ZM 预留 最小攻击力
	DWORD m56axpdamage;      //ZM 预留 最大攻击力
	DWORD m56damage;     //ZM 预留 最小法术攻击力
	DWORD m56axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p56defence;     //ZM 预留 物防
	DWORD m56defence;     //ZM 预留 魔防

	DWORD m57axhp;      //ZM 预留 最大生命值
	DWORD p57damage;      //ZM 预留 最小攻击力
	DWORD m57axpdamage;      //ZM 预留 最大攻击力
	DWORD m57damage;     //ZM 预留 最小法术攻击力
	DWORD m57axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p57defence;     //ZM 预留 物防
	DWORD m57defence;     //ZM 预留 魔防

	DWORD m58axhp;      //ZM 预留 最大生命值
	DWORD p58damage;      //ZM 预留 最小攻击力
	DWORD m58axpdamage;      //ZM 预留 最大攻击力
	DWORD m58damage;     //ZM 预留 最小法术攻击力
	DWORD m58axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p58defence;     //ZM 预留 物防
	DWORD m58defence;     //ZM 预留 魔防

	DWORD m59axhp;      //ZM 预留 最大生命值
	DWORD p59damage;      //ZM 预留 最小攻击力
	DWORD m59axpdamage;      //ZM 预留 最大攻击力
	DWORD m59damage;     //ZM 预留 最小法术攻击力
	DWORD m59axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p59defence;     //ZM 预留 物防
	DWORD m59defence;     //ZM 预留 魔防

	DWORD m60axhp;      //ZM 预留 最大生命值
	DWORD p60damage;      //ZM 预留 最小攻击力
	DWORD m60axpdamage;      //ZM 预留 最大攻击力
	DWORD m60damage;     //ZM 预留 最小法术攻击力
	DWORD m60axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p60defence;     //ZM 预留 物防
	DWORD m60defence;     //ZM 预留 魔防

	DWORD m61axhp;      //ZM 预留 最大生命值
	DWORD p61damage;      //ZM 预留 最小攻击力
	DWORD m61axpdamage;      //ZM 预留 最大攻击力
	DWORD m61damage;     //ZM 预留 最小法术攻击力
	DWORD m61axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p61defence;     //ZM 预留 物防
	DWORD m61defence;     //ZM 预留 魔防

	DWORD m62axhp;      //ZM 预留 最大生命值
	DWORD p62damage;      //ZM 预留 最小攻击力
	DWORD m62axpdamage;      //ZM 预留 最大攻击力
	DWORD m62damage;     //ZM 预留 最小法术攻击力
	DWORD m62axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p62defence;     //ZM 预留 物防
	DWORD m62defence;     //ZM 预留 魔防

	DWORD m63axhp;      //ZM 预留 最大生命值
	DWORD p63damage;      //ZM 预留 最小攻击力
	DWORD m63axpdamage;      //ZM 预留 最大攻击力
	DWORD m63damage;     //ZM 预留 最小法术攻击力
	DWORD m63axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p63defence;     //ZM 预留 物防
	DWORD m63defence;     //ZM 预留 魔防

	DWORD m64axhp;      //ZM 预留 最大生命值
	DWORD p64damage;      //ZM 预留 最小攻击力
	DWORD m64axpdamage;      //ZM 预留 最大攻击力
	DWORD m64damage;     //ZM 预留 最小法术攻击力
	DWORD m64axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p64defence;     //ZM 预留 物防
	DWORD m64defence;     //ZM 预留 魔防

	DWORD m65axhp;      //ZM 预留 最大生命值
	DWORD p65damage;      //ZM 预留 最小攻击力
	DWORD m65axpdamage;      //ZM 预留 最大攻击力
	DWORD m65damage;     //ZM 预留 最小法术攻击力
	DWORD m65axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p65defence;     //ZM 预留 物防
	DWORD m65defence;     //ZM 预留 魔防

	DWORD m66axhp;      //ZM 预留 最大生命值
	DWORD p66damage;      //ZM 预留 最小攻击力
	DWORD m66axpdamage;      //ZM 预留 最大攻击力
	DWORD m66damage;     //ZM 预留 最小法术攻击力
	DWORD m66axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p66defence;     //ZM 预留 物防
	DWORD m66defence;     //ZM 预留 魔防

	DWORD m67axhp;      //ZM 预留 最大生命值
	DWORD p67damage;      //ZM 预留 最小攻击力
	DWORD m67axpdamage;      //ZM 预留 最大攻击力
	DWORD m67damage;     //ZM 预留 最小法术攻击力
	DWORD m67axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p67defence;     //ZM 预留 物防
	DWORD m67defence;     //ZM 预留 魔防

	DWORD m68axhp;      //ZM 预留 最大生命值
	DWORD p68damage;      //ZM 预留 最小攻击力
	DWORD m68axpdamage;      //ZM 预留 最大攻击力
	DWORD m68damage;     //ZM 预留 最小法术攻击力
	DWORD m68axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p68defence;     //ZM 预留 物防
	DWORD m68defence;     //ZM 预留 魔防

	DWORD m69axhp;      //ZM 预留 最大生命值
	DWORD p69damage;      //ZM 预留 最小攻击力
	DWORD m69axpdamage;      //ZM 预留 最大攻击力
	DWORD m69damage;     //ZM 预留 最小法术攻击力
	DWORD m69axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p69defence;     //ZM 预留 物防
	DWORD m69defence;     //ZM 预留 魔防

	DWORD m70axhp;      //ZM 预留 最大生命值
	DWORD p70damage;      //ZM 预留 最小攻击力
	DWORD m70axpdamage;      //ZM 预留 最大攻击力
	DWORD m70damage;     //ZM 预留 最小法术攻击力
	DWORD m70axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p70defence;     //ZM 预留 物防
	DWORD m70defence;     //ZM 预留 魔防

	DWORD m71axhp;      //ZM 预留 最大生命值
	DWORD p71damage;      //ZM 预留 最小攻击力
	DWORD m71axpdamage;      //ZM 预留 最大攻击力
	DWORD m71damage;     //ZM 预留 最小法术攻击力
	DWORD m71axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p71defence;     //ZM 预留 物防
	DWORD m71defence;     //ZM 预留 魔防

	DWORD m72axhp;      //ZM 预留 最大生命值
	DWORD p72damage;      //ZM 预留 最小攻击力
	DWORD m72axpdamage;      //ZM 预留 最大攻击力
	DWORD m72damage;     //ZM 预留 最小法术攻击力
	DWORD m72axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p72defence;     //ZM 预留 物防
	DWORD m72defence;     //ZM 预留 魔防

	DWORD m73axhp;      //ZM 预留 最大生命值
	DWORD p73damage;      //ZM 预留 最小攻击力
	DWORD m73axpdamage;      //ZM 预留 最大攻击力
	DWORD m73damage;     //ZM 预留 最小法术攻击力
	DWORD m73axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p73defence;     //ZM 预留 物防
	DWORD m73defence;     //ZM 预留 魔防

	DWORD m74axhp;      //ZM 预留 最大生命值
	DWORD p74damage;      //ZM 预留 最小攻击力
	DWORD m74axpdamage;      //ZM 预留 最大攻击力
	DWORD m74damage;     //ZM 预留 最小法术攻击力
	DWORD m74axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p74defence;     //ZM 预留 物防
	DWORD m74defence;     //ZM 预留 魔防

	DWORD m75axhp;      //ZM 预留 最大生命值
	DWORD p75damage;      //ZM 预留 最小攻击力
	DWORD m75axpdamage;      //ZM 预留 最大攻击力
	DWORD m75damage;     //ZM 预留 最小法术攻击力
	DWORD m75axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p75defence;     //ZM 预留 物防
	DWORD m75defence;     //ZM 预留 魔防

	DWORD m76axhp;      //ZM 预留 最大生命值
	DWORD p76damage;      //ZM 预留 最小攻击力
	DWORD m76axpdamage;      //ZM 预留 最大攻击力
	DWORD m76damage;     //ZM 预留 最小法术攻击力
	DWORD m76axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p76defence;     //ZM 预留 物防
	DWORD m76defence;     //ZM 预留 魔防

	DWORD m77axhp;      //ZM 预留 最大生命值
	DWORD p77damage;      //ZM 预留 最小攻击力
	DWORD m77axpdamage;      //ZM 预留 最大攻击力
	DWORD m77damage;     //ZM 预留 最小法术攻击力
	DWORD m77axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p77defence;     //ZM 预留 物防
	DWORD m77defence;     //ZM 预留 魔防

	DWORD m78axhp;      //ZM 预留 最大生命值
	DWORD p78damage;      //ZM 预留 最小攻击力
	DWORD m78axpdamage;      //ZM 预留 最大攻击力
	DWORD m78damage;     //ZM 预留 最小法术攻击力
	DWORD m78axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p78defence;     //ZM 预留 物防
	DWORD m78defence;     //ZM 预留 魔防

	DWORD m79axhp;      //ZM 预留 最大生命值
	DWORD p79damage;      //ZM 预留 最小攻击力
	DWORD m79axpdamage;      //ZM 预留 最大攻击力
	DWORD m79damage;     //ZM 预留 最小法术攻击力
	DWORD m79axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p79defence;     //ZM 预留 物防
	DWORD m79defence;     //ZM 预留 魔防

	DWORD m80axhp;      //ZM 预留 最大生命值
	DWORD p80damage;      //ZM 预留 最小攻击力
	DWORD m80axpdamage;      //ZM 预留 最大攻击力
	DWORD m80damage;     //ZM 预留 最小法术攻击力
	DWORD m80axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p80defence;     //ZM 预留 物防
	DWORD m80defence;     //ZM 预留 魔防

	DWORD m81axhp;      //ZM 预留 最大生命值
	DWORD p81damage;      //ZM 预留 最小攻击力
	DWORD m81axpdamage;      //ZM 预留 最大攻击力
	DWORD m81damage;     //ZM 预留 最小法术攻击力
	DWORD m81axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p81defence;     //ZM 预留 物防
	DWORD m81defence;     //ZM 预留 魔防

	DWORD m82axhp;      //ZM 预留 最大生命值
	DWORD p82damage;      //ZM 预留 最小攻击力
	DWORD m82axpdamage;      //ZM 预留 最大攻击力
	DWORD m82damage;     //ZM 预留 最小法术攻击力
	DWORD m82axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p82defence;     //ZM 预留 物防
	DWORD m82defence;     //ZM 预留 魔防

	DWORD m83axhp;      //ZM 预留 最大生命值
	DWORD p83damage;      //ZM 预留 最小攻击力
	DWORD m83axpdamage;      //ZM 预留 最大攻击力
	DWORD m83damage;     //ZM 预留 最小法术攻击力
	DWORD m83axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p83defence;     //ZM 预留 物防
	DWORD m83defence;     //ZM 预留 魔防

	DWORD m84axhp;      //ZM 预留 最大生命值
	DWORD p84damage;      //ZM 预留 最小攻击力
	DWORD m84axpdamage;      //ZM 预留 最大攻击力
	DWORD m84damage;     //ZM 预留 最小法术攻击力
	DWORD m84axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p84defence;     //ZM 预留 物防
	DWORD m84defence;     //ZM 预留 魔防

	DWORD m85axhp;      //ZM 预留 最大生命值
	DWORD p85damage;      //ZM 预留 最小攻击力
	DWORD m85axpdamage;      //ZM 预留 最大攻击力
	DWORD m85damage;     //ZM 预留 最小法术攻击力
	DWORD m85axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p85defence;     //ZM 预留 物防
	DWORD m85defence;     //ZM 预留 魔防

	DWORD m86axhp;      //ZM 预留 最大生命值
	DWORD p86damage;      //ZM 预留 最小攻击力
	DWORD m86axpdamage;      //ZM 预留 最大攻击力
	DWORD m86damage;     //ZM 预留 最小法术攻击力
	DWORD m86axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p86defence;     //ZM 预留 物防
	DWORD m86defence;     //ZM 预留 魔防

	DWORD m87axhp;      //ZM 预留 最大生命值
	DWORD p87damage;      //ZM 预留 最小攻击力
	DWORD m87axpdamage;      //ZM 预留 最大攻击力
	DWORD m87damage;     //ZM 预留 最小法术攻击力
	DWORD m87axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p87defence;     //ZM 预留 物防
	DWORD m87defence;     //ZM 预留 魔防

	DWORD m88axhp;      //ZM 预留 最大生命值
	DWORD p88damage;      //ZM 预留 最小攻击力
	DWORD m88axpdamage;      //ZM 预留 最大攻击力
	DWORD m88damage;     //ZM 预留 最小法术攻击力
	DWORD m88axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p88defence;     //ZM 预留 物防
	DWORD m88defence;     //ZM 预留 魔防

	DWORD m89axhp;      //ZM 预留 最大生命值
	DWORD p89damage;      //ZM 预留 最小攻击力
	DWORD m89axpdamage;      //ZM 预留 最大攻击力
	DWORD m89damage;     //ZM 预留 最小法术攻击力
	DWORD m89axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p89defence;     //ZM 预留 物防
	DWORD m89defence;     //ZM 预留 魔防

	DWORD m90axhp;      //ZM 预留 最大生命值
	DWORD p90damage;      //ZM 预留 最小攻击力
	DWORD m90axpdamage;      //ZM 预留 最大攻击力
	DWORD m90damage;     //ZM 预留 最小法术攻击力
	DWORD m90axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p90defence;     //ZM 预留 物防
	DWORD m90defence;     //ZM 预留 魔防

	DWORD m91axhp;      //ZM 预留 最大生命值
	DWORD p91damage;      //ZM 预留 最小攻击力
	DWORD m91axpdamage;      //ZM 预留 最大攻击力
	DWORD m91damage;     //ZM 预留 最小法术攻击力
	DWORD m91axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p91defence;     //ZM 预留 物防
	DWORD m91defence;     //ZM 预留 魔防

	DWORD m92axhp;      //ZM 预留 最大生命值
	DWORD p92damage;      //ZM 预留 最小攻击力
	DWORD m92axpdamage;      //ZM 预留 最大攻击力
	DWORD m92damage;     //ZM 预留 最小法术攻击力
	DWORD m92axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p92defence;     //ZM 预留 物防
	DWORD m92defence;     //ZM 预留 魔防

	DWORD m93axhp;      //ZM 预留 最大生命值
	DWORD p93damage;      //ZM 预留 最小攻击力
	DWORD m93axpdamage;      //ZM 预留 最大攻击力
	DWORD m93damage;     //ZM 预留 最小法术攻击力
	DWORD m93axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p93defence;     //ZM 预留 物防
	DWORD m93defence;     //ZM 预留 魔防

	DWORD m94axhp;      //ZM 预留 最大生命值
	DWORD p94damage;      //ZM 预留 最小攻击力
	DWORD m94axpdamage;      //ZM 预留 最大攻击力
	DWORD m94damage;     //ZM 预留 最小法术攻击力
	DWORD m94axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p94defence;     //ZM 预留 物防
	DWORD m94defence;     //ZM 预留 魔防

	DWORD m95axhp;      //ZM 预留 最大生命值
	DWORD p95damage;      //ZM 预留 最小攻击力
	DWORD m95axpdamage;      //ZM 预留 最大攻击力
	DWORD m95damage;     //ZM 预留 最小法术攻击力
	DWORD m95axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p95defence;     //ZM 预留 物防
	DWORD m95defence;     //ZM 预留 魔防

	DWORD m96axhp;      //ZM 预留 最大生命值
	DWORD p96damage;      //ZM 预留 最小攻击力
	DWORD m96axpdamage;      //ZM 预留 最大攻击力
	DWORD m96damage;     //ZM 预留 最小法术攻击力
	DWORD m96axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p96defence;     //ZM 预留 物防
	DWORD m96defence;     //ZM 预留 魔防

	DWORD m97axhp;      //ZM 预留 最大生命值
	DWORD p97damage;      //ZM 预留 最小攻击力
	DWORD m97axpdamage;      //ZM 预留 最大攻击力
	DWORD m97damage;     //ZM 预留 最小法术攻击力
	DWORD m97axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p97defence;     //ZM 预留 物防
	DWORD m97defence;     //ZM 预留 魔防

	DWORD m98axhp;      //ZM 预留 最大生命值
	DWORD p98damage;      //ZM 预留 最小攻击力
	DWORD m98axpdamage;      //ZM 预留 最大攻击力
	DWORD m98damage;     //ZM 预留 最小法术攻击力
	DWORD m98axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p98defence;     //ZM 预留 物防
	DWORD m98defence;     //ZM 预留 魔防

	DWORD m99axhp;      //ZM 预留 最大生命值
	DWORD p99damage;      //ZM 预留 最小攻击力
	DWORD m99axpdamage;      //ZM 预留 最大攻击力
	DWORD m99damage;     //ZM 预留 最小法术攻击力
	DWORD m99axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p99defence;     //ZM 预留 物防
	DWORD m99defence;     //ZM 预留 魔防

	DWORD m100axhp;      //ZM 预留 最大生命值
	DWORD p100damage;      //ZM 预留 最小攻击力
	DWORD m100axpdamage;      //ZM 预留 最大攻击力
	DWORD m100damage;     //ZM 预留 最小法术攻击力
	DWORD m100axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p100defence;     //ZM 预留 物防
	DWORD m100defence;     //ZM 预留 魔防

	DWORD m101axhp;      //ZM 预留 最大生命值
	DWORD p101damage;      //ZM 预留 最小攻击力
	DWORD m101axpdamage;      //ZM 预留 最大攻击力
	DWORD m101damage;     //ZM 预留 最小法术攻击力
	DWORD m101axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p101defence;     //ZM 预留 物防
	DWORD m101defence;     //ZM 预留 魔防

	DWORD m102axhp;      //ZM 预留 最大生命值
	DWORD p102damage;      //ZM 预留 最小攻击力
	DWORD m102axpdamage;      //ZM 预留 最大攻击力
	DWORD m102damage;     //ZM 预留 最小法术攻击力
	DWORD m102axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p102defence;     //ZM 预留 物防
	DWORD m102defence;     //ZM 预留 魔防

	DWORD m103axhp;      //ZM 预留 最大生命值
	DWORD p103damage;      //ZM 预留 最小攻击力
	DWORD m103axpdamage;      //ZM 预留 最大攻击力
	DWORD m103damage;     //ZM 预留 最小法术攻击力
	DWORD m103axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p103defence;     //ZM 预留 物防
	DWORD m103defence;     //ZM 预留 魔防

	DWORD m104axhp;      //ZM 预留 最大生命值
	DWORD p104damage;      //ZM 预留 最小攻击力
	DWORD m104axpdamage;      //ZM 预留 最大攻击力
	DWORD m104damage;     //ZM 预留 最小法术攻击力
	DWORD m104axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p104defence;     //ZM 预留 物防
	DWORD m104defence;     //ZM 预留 魔防

	DWORD m105axhp;      //ZM 预留 最大生命值
	DWORD p105damage;      //ZM 预留 最小攻击力
	DWORD m105axpdamage;      //ZM 预留 最大攻击力
	DWORD m105damage;     //ZM 预留 最小法术攻击力
	DWORD m105axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p105defence;     //ZM 预留 物防
	DWORD m105defence;     //ZM 预留 魔防

	DWORD m106axhp;      //ZM 预留 最大生命值
	DWORD p106damage;      //ZM 预留 最小攻击力
	DWORD m106axpdamage;      //ZM 预留 最大攻击力
	DWORD m106damage;     //ZM 预留 最小法术攻击力
	DWORD m106axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p106defence;     //ZM 预留 物防
	DWORD m106defence;     //ZM 预留 魔防

	DWORD m107axhp;      //ZM 预留 最大生命值
	DWORD p107damage;      //ZM 预留 最小攻击力
	DWORD m107axpdamage;      //ZM 预留 最大攻击力
	DWORD m107damage;     //ZM 预留 最小法术攻击力
	DWORD m107axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p107defence;     //ZM 预留 物防
	DWORD m107defence;     //ZM 预留 魔防

	DWORD m108axhp;      //ZM 预留 最大生命值
	DWORD p108damage;      //ZM 预留 最小攻击力
	DWORD m108axpdamage;      //ZM 预留 最大攻击力
	DWORD m108damage;     //ZM 预留 最小法术攻击力
	DWORD m108axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p108defence;     //ZM 预留 物防
	DWORD m108defence;     //ZM 预留 魔防

	DWORD m109axhp;      //ZM 预留 最大生命值
	DWORD p109damage;      //ZM 预留 最小攻击力
	DWORD m109axpdamage;      //ZM 预留 最大攻击力
	DWORD m109damage;     //ZM 预留 最小法术攻击力
	DWORD m109axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p109defence;     //ZM 预留 物防
	DWORD m109defence;     //ZM 预留 魔防

	DWORD m110axhp;      //ZM 预留 最大生命值
	DWORD p110damage;      //ZM 预留 最小攻击力
	DWORD m110axpdamage;      //ZM 预留 最大攻击力
	DWORD m110damage;     //ZM 预留 最小法术攻击力
	DWORD m110axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p110defence;     //ZM 预留 物防
	DWORD m110defence;     //ZM 预留 魔防

	DWORD m111axhp;      //ZM 预留 最大生命值
	DWORD p111damage;      //ZM 预留 最小攻击力
	DWORD m111axpdamage;      //ZM 预留 最大攻击力
	DWORD m111damage;     //ZM 预留 最小法术攻击力
	DWORD m111axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p111defence;     //ZM 预留 物防
	DWORD m111defence;     //ZM 预留 魔防

	DWORD m112axhp;      //ZM 预留 最大生命值
	DWORD p112damage;      //ZM 预留 最小攻击力
	DWORD m112axpdamage;      //ZM 预留 最大攻击力
	DWORD m112damage;     //ZM 预留 最小法术攻击力
	DWORD m112axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p112defence;     //ZM 预留 物防
	DWORD m112defence;     //ZM 预留 魔防

	DWORD m113axhp;      //ZM 预留 最大生命值
	DWORD p113damage;      //ZM 预留 最小攻击力
	DWORD m113axpdamage;      //ZM 预留 最大攻击力
	DWORD m113damage;     //ZM 预留 最小法术攻击力
	DWORD m113axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p113defence;     //ZM 预留 物防
	DWORD m113defence;     //ZM 预留 魔防

	DWORD m114axhp;      //ZM 预留 最大生命值
	DWORD p114damage;      //ZM 预留 最小攻击力
	DWORD m114axpdamage;      //ZM 预留 最大攻击力
	DWORD m114damage;     //ZM 预留 最小法术攻击力
	DWORD m114axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p114defence;     //ZM 预留 物防
	DWORD m114defence;     //ZM 预留 魔防

	DWORD m115axhp;      //ZM 预留 最大生命值
	DWORD p115damage;      //ZM 预留 最小攻击力
	DWORD m115axpdamage;      //ZM 预留 最大攻击力
	DWORD m115damage;     //ZM 预留 最小法术攻击力
	DWORD m115axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p115defence;     //ZM 预留 物防
	DWORD m115defence;     //ZM 预留 魔防

	DWORD m116axhp;      //ZM 预留 最大生命值
	DWORD p116damage;      //ZM 预留 最小攻击力
	DWORD m116axpdamage;      //ZM 预留 最大攻击力
	DWORD m116damage;     //ZM 预留 最小法术攻击力
	DWORD m116axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p116defence;     //ZM 预留 物防
	DWORD m116defence;     //ZM 预留 魔防

	DWORD m117axhp;      //ZM 预留 最大生命值
	DWORD p117damage;      //ZM 预留 最小攻击力
	DWORD m117axpdamage;      //ZM 预留 最大攻击力
	DWORD m117damage;     //ZM 预留 最小法术攻击力
	DWORD m117axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p117defence;     //ZM 预留 物防
	DWORD m117defence;     //ZM 预留 魔防

	DWORD m118axhp;      //ZM 预留 最大生命值
	DWORD p118damage;      //ZM 预留 最小攻击力
	DWORD m118axpdamage;      //ZM 预留 最大攻击力
	DWORD m118damage;     //ZM 预留 最小法术攻击力
	DWORD m118axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p118defence;     //ZM 预留 物防
	DWORD m118defence;     //ZM 预留 魔防

	DWORD m119axhp;      //ZM 预留 最大生命值
	DWORD p119damage;      //ZM 预留 最小攻击力
	DWORD m119axpdamage;      //ZM 预留 最大攻击力
	DWORD m119damage;     //ZM 预留 最小法术攻击力
	DWORD m119axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p119defence;     //ZM 预留 物防
	DWORD m119defence;     //ZM 预留 魔防

	DWORD m120axhp;      //ZM 预留 最大生命值
	DWORD p120damage;      //ZM 预留 最小攻击力
	DWORD m120axpdamage;      //ZM 预留 最大攻击力
	DWORD m120damage;     //ZM 预留 最小法术攻击力
	DWORD m120axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p120defence;     //ZM 预留 物防
	DWORD m120defence;     //ZM 预留 魔防

	DWORD m121axhp;      //ZM 预留 最大生命值
	DWORD p121damage;      //ZM 预留 最小攻击力
	DWORD m121axpdamage;      //ZM 预留 最大攻击力
	DWORD m121damage;     //ZM 预留 最小法术攻击力
	DWORD m121axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p121defence;     //ZM 预留 物防
	DWORD m121defence;     //ZM 预留 魔防

	DWORD m122axhp;      //ZM 预留 最大生命值
	DWORD p122damage;      //ZM 预留 最小攻击力
	DWORD m122axpdamage;      //ZM 预留 最大攻击力
	DWORD m122damage;     //ZM 预留 最小法术攻击力
	DWORD m122axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p122defence;     //ZM 预留 物防
	DWORD m122defence;     //ZM 预留 魔防

	DWORD m123axhp;      //ZM 预留 最大生命值
	DWORD p123damage;      //ZM 预留 最小攻击力
	DWORD m123axpdamage;      //ZM 预留 最大攻击力
	DWORD m123damage;     //ZM 预留 最小法术攻击力
	DWORD m123axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p123defence;     //ZM 预留 物防
	DWORD m123defence;     //ZM 预留 魔防

	DWORD m124axhp;      //ZM 预留 最大生命值
	DWORD p124damage;      //ZM 预留 最小攻击力
	DWORD m124axpdamage;      //ZM 预留 最大攻击力
	DWORD m124damage;     //ZM 预留 最小法术攻击力
	DWORD m124axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p124defence;     //ZM 预留 物防
	DWORD m124defence;     //ZM 预留 魔防

	DWORD m125axhp;      //ZM 预留 最大生命值
	DWORD p125damage;      //ZM 预留 最小攻击力
	DWORD m125axpdamage;      //ZM 预留 最大攻击力
	DWORD m125damage;     //ZM 预留 最小法术攻击力
	DWORD m125axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p125defence;     //ZM 预留 物防
	DWORD m125defence;     //ZM 预留 魔防

	DWORD m126axhp;      //ZM 预留 最大生命值
	DWORD p126damage;      //ZM 预留 最小攻击力
	DWORD m126axpdamage;      //ZM 预留 最大攻击力
	DWORD m126damage;     //ZM 预留 最小法术攻击力
	DWORD m126axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p126defence;     //ZM 预留 物防
	DWORD m126defence;     //ZM 预留 魔防

	DWORD m127axhp;      //ZM 预留 最大生命值
	DWORD p127damage;      //ZM 预留 最小攻击力
	DWORD m127axpdamage;      //ZM 预留 最大攻击力
	DWORD m127damage;     //ZM 预留 最小法术攻击力
	DWORD m127axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p127defence;     //ZM 预留 物防
	DWORD m127defence;     //ZM 预留 魔防

	DWORD m128axhp;      //ZM 预留 最大生命值
	DWORD p128damage;      //ZM 预留 最小攻击力
	DWORD m128axpdamage;      //ZM 预留 最大攻击力
	DWORD m128damage;     //ZM 预留 最小法术攻击力
	DWORD m128axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p128defence;     //ZM 预留 物防
	DWORD m128defence;     //ZM 预留 魔防

	DWORD m129axhp;      //ZM 预留 最大生命值
	DWORD p129damage;      //ZM 预留 最小攻击力
	DWORD m129axpdamage;      //ZM 预留 最大攻击力
	DWORD m129damage;     //ZM 预留 最小法术攻击力
	DWORD m129axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p129defence;     //ZM 预留 物防
	DWORD m129defence;     //ZM 预留 魔防

	DWORD hongtiannum8;      //ZM 预留 最大生命值
	DWORD hongtiannum9;      //ZM 预留 最小攻击力
	DWORD m130axpdamage;      //ZM 预留 最大攻击力
	DWORD m130damage;     //ZM 预留 最小法术攻击力
	DWORD m130axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p130defence;     //ZM 预留 物防
	DWORD m130defence;     //ZM 预留 魔防

	DWORD hongtiannum1;      //ZM 预留 最大生命值
	DWORD hongtiannum2;      //ZM 预留 最小攻击力
	DWORD hongtiannum3;      //ZM 预留 最大攻击力
	DWORD hongtiannum4;     //ZM 预留 最小法术攻击力
	DWORD hongtiannum5;     //ZM 预留 最大法术攻击力
	DWORD hongtiannum6;     //ZM 预留 物防
	DWORD hongtiannum7;     //ZM 预留 魔防

	BYTE damagebonus;			// 伤害加成
	BYTE damage;				// 增加伤害值x％
	
	WORD akspeed;				// 攻击速度
	WORD mvspeed;				// 移动速度
	WORD atrating;				// 命中率
	WORD akdodge;				// 躲避率

	WORD str;  // 力量
	WORD inte;  // 智力
	WORD dex;  // 敏捷
	WORD spi;  // 精神
	WORD con;  // 体质

	WORD xstr;  // 力量-魂魄
	WORD xinte; // 智力-魂魄
	WORD xdex;  // 敏捷-魂魄
	WORD xspi;  // 精神-魂魄
	WORD xcon;  // 体质-魂魄
	
	
//			WORD fivetype;  // 五行属性
//			WORD fivepoint; // 五行属性
	
	WORD hpr;  // 生命值恢复
	WORD mpr;  // 法术值恢复
	WORD spr;  // 体力值恢复
	
	WORD holy;  //神圣一击	
	WORD bang;	//重击率
	WORD pdam;  // 增加物理攻击力
	WORD pdef;  // 增加物理防御力
	WORD mdam;  // 增加魔法攻击力
	WORD mdef;  // 增加魔法防御力
	
	WORD poisondef; //抗毒增加
	WORD lulldef; //抗麻痹增加
	WORD reeldef; //抗眩晕增加
	WORD evildef; //抗噬魔增加
	WORD bitedef; //抗噬力增加
	WORD chaosdef; //抗混乱增加
	WORD colddef; //抗冰冻增加
	WORD petrifydef; //抗石化增加
	WORD blinddef; //抗失明增加
	WORD stabledef; //抗定身增加
	WORD slowdef; //抗减速增加
	WORD luredef; //抗诱惑增加
		
	WORD poison; //中毒增加
	WORD lull; //麻痹增加
	WORD reel; //眩晕增加
	WORD evil; //噬魔增加
	WORD bite; //噬力增加
	WORD chaos; //混乱增加
	WORD cold; //冰冻增加
	WORD petrify; //石化增加
	WORD blind; //失明增加
	WORD stable; //定身增加
	WORD slow; //减速增加
	WORD lure; //诱惑增加
	
	struct leech
	{
		BYTE odds;    //x
		WORD effect;	//y
	};
	leech hpleech; //x%吸收生命值y
	leech mpleech; //x%吸收法术值y
	
	BYTE hptomp; //转换生命值为法术值x％
	BYTE dhpp; //物理伤害减少x%	
	BYTE dmpp; //法术伤害值减少x%		

	BYTE incgold; //增加金钱掉落x%
	BYTE doublexp; //x%双倍经验		
	BYTE mf; //增加掉宝率x%

	union {
		//soke 灵魂锁链
		BYTE _five_props[10];
		struct 
		{
			//五行套装相关属性
			BYTE dpdam; //物理伤害减少%x
			BYTE dmdam; //法术伤害减少%x
			BYTE bdam; //增加伤害x%
			BYTE rdam; //伤害反射%x
			BYTE ignoredef; //%x忽视目标防御
			BYTE addpdef;     //物理防御力增强xxx
			BYTE addmdef;     //魔法防御力增强xxx
			BYTE addpdam;     //增加物理攻击力xxxx
			BYTE addmdam;     //增加物理攻击力xxxx
			BYTE addhp;       //增加生命值xxxxx
		};
	};
	
	WORD aftype;  
	WORD afpoint;
	WORD dftype;
	WORD dfpoint;
	
	typedef std::map<DWORD, DWORD> SKILL;
	typedef SKILL::const_iterator const_iterator;
	SKILL skill;  //单个技能加成
	SKILL skills; //全系技能加成 
	WORD maxhprate;  //按百分比增加最大hp
	WORD maxmprate;  //按百分比增加最大mp
};	


struct PackageCallback
{
	virtual bool exec(zObject * o)=0;
	virtual ~PackageCallback(){};
};

class Package:private zNoncopyable
{
protected:
    virtual bool add(zObject *object ,bool find);
    virtual bool remove(zObject *object);
    virtual bool checkAdd(SceneUser *pUser,zObject *object,WORD x,WORD y);		
	
public:
    Package(WORD type,DWORD id, WORD w, WORD h);
    virtual ~Package();

    virtual bool getObjectByZone(zObject **ret,WORD x,WORD y);
    virtual bool getObjectByID(zObject **ret,DWORD id);
    virtual void execEvery(PackageCallback &callback);

    virtual WORD space() const;
    virtual WORD size() const;
    WORD type() const;
    DWORD id() const;

    void setSpace(WORD);

public:
    zObject** container;

private:
    friend class Packages;

    virtual void removeAll();	
    bool find_space(WORD &x,WORD &y) const;
    virtual int position(WORD x, WORD y) const;

    WORD _type;
    DWORD _id;
    WORD _width;
    WORD _height;

    WORD _space;
    WORD _size;

};

#if 0
class MultiPack:public Package
{
	protected:
		WORD width;
		WORD height;
		std::vector<std::vector<zObject *> >grid;
		std::set<zObject *> allset;
	public:
		MultiPack(WORD type,DWORD id ,WORD width,WORD height);
		virtual ~MultiPack();
		virtual bool add(zObject *object ,bool find);
		virtual void remove(zObject *object);
		virtual void removeAll();
		virtual bool checkAdd(SceneUser *pUser,zObject *object,WORD x,WORD y);
		virtual bool getObjectByZone(zObject **ret,WORD x,WORD y,WORD width,WORD height);
		virtual void execEvery(PackageCallback &callback);
		virtual bool findSpace(WORD owidth,WORD oheight,WORD &x,WORD &y);
		std::set<zObject *> &getAllset()
		{
			return allset;
		}
		
		WORD get_width() const 
		{
			return width;
		}
		
		WORD get_height() const 
		{
			return height;
		}

};
#endif

class ObjectPack : public Package
{
public:
	ObjectPack(zObject* ob, int type = Cmd::OBJECTCELLTYPE_PACKAGE, bool consume = true);
	~ObjectPack();
	
	bool empty() const 
	{
		return space() == size();
	}
	
	zObject* object()
	{
		return _ob;
	}

	void consume_dur_by(SceneUser* user, const zRTime& current);
	bool can_input()
	{
		if (!_ob || _ob->data.dur == 0) {
			return false;
		}
		return true;
	}
	
private:	
	bool checkAdd(SceneUser *pUser,zObject *object,WORD x,WORD y);
	//bool add(zObject *object ,bool find);
	
	zObject* _ob;
	bool _cosume_by_time;
	Timer _one_min;
};

		
class EquipPack : public Package
{
	public:

		enum EQUIPNO{		
			BODY,
			HEAD,
			HANDL,
			HANDR,
			BELT,
			NECKLACE, //5
			
			RINGL,
			RINGR,
			BANGL,
			BANGR,
			BOOTS, //10
			
			OTHERS,	
			OTHERS1,	
			OTHERS2,	
			OTHERS3,
			OTHERS4,	//15
			OTHERS5, 
			OTHERS6, 
			OTHERS7,
			OTHERS8,
			OTHERS9,
			OTHERS10,
		};
		
		enum PACKNO
		{
			R_PACK,
			L_PACK,
			R_MAKE,
			L_MAKE,
		};
		EquipPack(SceneUser* user);
		~EquipPack();
		
		DWORD doubleexp_obj;
		DWORD doubleexp_obj_time;
		DWORD family_obj_times;
		DWORD family_obj_time;
		DWORD tong_obj_times;
		DWORD tong_obj_time;
		DWORD king_obj_times;
		DWORD king_obj_time;
		const Equips &getEquips() const;
		ObjectPack* pack(PACKNO no) const;
		zObject* equip(EQUIPNO no) const;
			
		//soke 检测物品的装备职业是否符合当前玩家的职业*/
	    bool IsJobEquip(SceneUser *pUser,DWORD changejob, DWORD EquipType);
		bool getObjectByZone(zObject **ret,WORD x,WORD y);
		zObject *getObjectByEquipNo(EQUIPNO no);
		zObject *getObjectByEquipPos(DWORD no);

		zObject *getObjectByType(int type);

		bool costAttackDur(SceneUser *pThis);
		bool costDefenceDur(SceneUser *pThis);
		bool costAttackDurByPet(SceneUser *pThis);
		bool costDefenceDurByPet(SceneUser *pThis);
		void restituteDurability(SceneUser *pThis , const zRTime &ct);
		void updateDurability(SceneUser *pThis , DWORD value);
		DWORD fullAllEquiped(char *buf);

		bool needRecalc;
		int effecthanbing;
		int effectangel;
		int effectCount;
		int effect1star;
		int effect2star;
		int effect3star;
		int effect5star;
		int effect8star;
		int effect10star;
		int effect12star;
		int effect13star;
		int effect14star;
		int effect15star;
        // nyy 套装装备效果
		int effect16star; //桃子 乾坤套装
		int effect17star; //桃子 炙天套装
		int effect18star; //桃子 轩辕套装
		int effect19star; //桃子 卓越套装
		int effect20star; //桃子 传说套装
		int effect21star; //桃子 天尊套装
		int effect22star; //桃子 英雄套装
		int effect23star; //桃子 圣者套装
		int effect24star; //桃子 战神套装
		int effect25star; //桃子 帝王套装
		int effect26star; //桃子 弑天套装
		int effect27star; //桃子 永恒套装
		int effecttj1star; //桃子 白色图鉴套装
		int effecttj2star; //桃子 蓝色图鉴套装
		int effecttj3star; //桃子 黄色图鉴套装
		int effecttj4star; //桃子 绿色图鉴套装
		int effecttj5star; //桃子 紫色图鉴套装	
		int effecttj6star; //桃子 橙色图鉴套装
		int effecttj7star; //桃子 红色图鉴套装							
		int effect21linghun;
		int effect22linghun;
		int effect23linghun;
		int effect24linghun;
		int effect25linghun;
		int effect5mapai;
		int effect8mapai;
		int effect10mapai;
		int effect12mapai;
		int effect13mapai;

		// myy 斗魂套装效果
		int effect3douhun;  //斗魂3星套
		int effect6douhun;  //斗魂6星套
		int effect9douhun;  //斗魂9星套
		int effect12douhun;  //斗魂12星套
		int effect15douhun;  //斗魂15星套

		int effect5longx;
		int effect8longx;
		int effect11longx;
		int effect13longx;
		int effect15longx;

		int effect1butian;
		int effect2butian;
		int effect3butian;
		int effect4butian;
		int effect5butian;


		int effect2zhuangbei;
		int effect3zhuangbei;
		int effect4zhuangbei;
		int effect5zhuangbei;
		int effect6zhuangbei;

		int effect1shenqi;
		int effect2shenqi;
		int effect3shenqi;
		int effect4shenqi;
		int effect5shenqi;
		int effect6shenqi;
		int effect7shenqi;

		int effect1shenlong; //小龙套装 
		int effect2shenlong; //金龙套装
		int effect3shenlong; //苍龙套装
		int effect4shenlong; //巨龙套装
		int effect5shenlong; //神苍龙套装
		int effect6shenlong; //神巨龙套装

		int effect1nitian; //逆天进阶15阶套装 
		int effect2nitian; //逆天进阶30阶套装 
		int effect3nitian; //逆天进阶50阶套装 
		int effect4nitian; //逆天进阶80阶套装 
		int effect5nitian; //逆天进阶100阶套装 

		int effect1douzhansf; //神魂宝石25阶套装 
		int effect2douzhansf; //神魂宝石50阶套装 
		int effect3douzhansf; //神魂宝石75阶套装 
		int effect4douzhansf; //神魂宝石100阶套装 

		int effect1aystone; //暗影宝石20阶套装 
		int effect2aystone; //暗影宝石40阶套装 
		int effect3aystone; //暗影宝石60阶套装 
		int effect4aystone; //暗影宝石80阶套装 
		int effect5aystone; //暗影宝石100阶套装 

		int effect1lfy; //龙凤吟8级
		int effect2lfy; //龙凤吟10级
		int effect3lfy; //龙凤吟12级
		int effect4lfy; //龙凤吟14级
		int effect5lfy; //龙凤吟16级



		bool skillReduceObject(SceneUser* pThis, DWORD kind, WORD num);
		bool skillCheckReduceObject(SceneUser* pThis, DWORD id, WORD num);
		bool canChangeColor();
		void calcAll();
		int reduceDur(SceneUser *pThis , DWORD which , DWORD type , DWORD num , bool needFresh=true , bool needCal=true);
		bool isTonic();

		void obtain_exp(SceneUser* user, DWORD exp, bool force = false);

		void obtain_round(SceneUser* user, DWORD round, bool force = false);

	private:
		bool add(zObject *object ,bool find);
		bool remove(zObject *object);
		bool checkAdd(SceneUser *pUser,zObject *object,WORD x,WORD y);

		virtual int position(WORD x, WORD y) const;
		
		bool reduceDur(SceneUser *pThis , DWORD which);
		inline void freshDurability(SceneUser *pThis , zObject *o);
		inline void freshUserData(SceneUser *pThis);

		bool process_extra_add(zObject* ob);
		bool process_extra_remove(zObject* ob);
		bool process_extra_get(zObject** ob, WORD x, WORD y);
		
		SceneUser* owner;
		ObjectPack** packs;
		Equips equips;
		const static int _poses[240];

};

class SceneUser;

class MainPack:public Package
{
	public:
		enum {
			WIDTH = 7,
			HEIGHT = 6,
		};
		
		MainPack();
		~MainPack();
		
		bool getObjectByZone(zObject **ret,WORD x,WORD y);
		zObject * getGold();
		DWORD getGoldNum();
		
		bool skillReduceObject(SceneUser* pThis, DWORD kind, DWORD num);

	private:
		bool add(zObject *object ,bool find);
		bool remove(zObject *object);
		bool checkAdd(SceneUser *pUser,zObject *object,WORD x,WORD y);
		
		zObject *gold;
};

class StorePack : public Package
{
public:
	enum {
		W = 7,
		H = 6,
	};
	
	std::vector<BYTE> days;
	
	StorePack();
	~StorePack();
		
	void goldstore(SceneUser *pThis,bool notify=true);
	int load(unsigned char* dest);
	int save(unsigned char* dest);
	
	void notify(SceneUser* user);
	
private:
	bool checkAdd(SceneUser* pUser, zObject* object, WORD x, WORD y);
	
};

class PetPack : public Package
{
public:
	static const BYTE W = 8;
	static const BYTE H = 10;

	PetPack();
	~PetPack();

	WORD size() const;//返回最大可用数
	void setAvailable(WORD);//设置最大可用数

	bool isEmpty() const;

private:
	WORD available;//可用的总数

	bool checkAdd(SceneUser* pUser, zObject* object, WORD x, WORD y);
};

/**
 * \brief  打造装备助手合成的随身容器，包裹格子视需求而定
 * 
 */
class RecastPack : public Package
{
public:
    enum RECAST_PACK_WH 
    {
        RECAST_PACK_WIDTH  = 1,
        RECAST_PACK_HEIGHT = 5,
    };

    RecastPack();
    ~RecastPack();

};

/**
 * \brief  打造合成的随身容器，包裹格子视需求而定
 * 
 */
class FujianPack : public Package
{
public:
    enum FUJIAN_PACK_WH 
    {
        FUJIAN_PACK_WIDTH  = 1,
        FUJIAN_PACK_HEIGHT = 100,//醉梦附件格子100借用
    };

    FujianPack();
    ~FujianPack();

};

/**
 * \brief  打造附件助手合成的随身容器，包裹格子视需求而定
 * 
 */
class AnnexPack : public Package
{
public:
    enum ANNEX_PACK_WH 
    {
        RECAST_PACK_WIDTH  = 1,
        RECAST_PACK_HEIGHT = 5,
    };

    AnnexPack();
    ~AnnexPack();

};

/**
 * \brief  打造百兽图鉴合成的随身容器，包裹格子视需求而定
 * 
 */
class HandbookPack : public Package
{
public:
    enum HANDBOOK_PACK_WH 
    {
        RECAST_PACK_WIDTH  = 1,
        RECAST_PACK_HEIGHT = 5,
    };

    HandbookPack();
    ~HandbookPack();

};
/**
 * \brief  打造百兽图鉴合成的随身容器，包裹格子视需求而定
 * 
 */
class FabaoPack : public Package
{
public:
    enum FABAO_PACK_WH 
    {
        RECAST_PACK_WIDTH  = 1,
        RECAST_PACK_HEIGHT = 5,
    };

    FabaoPack();
    ~FabaoPack();

};

/**
 * \brief  打造百兽图鉴合成的随身容器，包裹格子视需求而定
 * 
 */
class YuanshenPack : public Package
{
public:
    enum YUANSHEN_PACK_WH 
    {
        RECAST_PACK_WIDTH  = 1,
        RECAST_PACK_HEIGHT = 5,
    };

    YuanshenPack();
    ~YuanshenPack();

};



#include "ObjectManager.h"

template<typename T>
class Type2Type
{
public:
	typedef T BASE;
};


class Packages
{
	private:
		SceneUser *owner;

		Package** getPackage(int packs);
		
	public:
		enum 
        {
			MAIN_PACK       = 1,
			LEFT_PACK       = 2,
			RIGHT_PACK      = 4, 
			EQUIP_PACK      = 8, 
			STORE_PACK      = 16,
			LM_PACK         = 32, 
			RM_PACK         = 64,
			PET_PACK        = 128,
            RECAST_PACK     = 256,  ///< 材料合成、装备升星的包裹，只有一个格子
		};
		
		UserObjectM uom;
		MainPack main;
		EquipPack equip;
		StorePack store;
		PetPack petPack;
		
		//sky 一键换装
		Cmd::One_Key_Equip m_OneKeyEquip[10];
		//sky 功勋竞猜
		struct jingcai_pro
		{
			int index;
			int object_id;
			int object_level;
			int object_num;
		};
		struct Chenghao_pro
		{
			DWORD state;
		};
		struct Shizhuang_pro
		{
			DWORD level;
			DWORD state;
		};
		struct Pifeng_pro
		{
			DWORD level;
			DWORD state;
		};
		struct Chibang_pro
		{
			DWORD level;
			DWORD state;
		};
		struct Zuoqi_pro
		{
			DWORD level;
			DWORD state;
		};
	    jingcai_pro m_jingcai[28];
		Chenghao_pro m_Chenghao[60];//称号
		Shizhuang_pro m_Shizhuang[300];//时装魔盒
		Pifeng_pro m_Pifeng[100];//时装魔盒
		Chibang_pro m_Chibang[100];//时装魔盒
		Zuoqi_pro m_Zuoqi[100];//时装魔盒
		RecastPack recastPack;
		AnnexPack annexPack;
		HandbookPack handbookPack;
		FabaoPack fabaoPack;
		FujianPack fujianPack;
		YuanshenPack yuanshenPack;
		Packages(SceneUser* user);
		~Packages();
		void addMoney(DWORD num, const char *disc , const char *note = NULL, bool notify = true);
		void addMoney2(DWORD num, const char *disc , const char *note = NULL, bool notify = true);
		void addGold(DWORD num, const char *disc , const char *note = NULL, bool notify = true,bool pack=false);
		void addTicket(DWORD num, const char *disc , const char *note = NULL, bool notify = true);
		void addTicket2(DWORD num, const char *disc , const char *note = NULL, bool notify = true);
		void addSalary(DWORD num, const char *disc , const char *note = NULL, bool notify = true);
		void addSalary2(DWORD num, const char *disc , const char *note = NULL, bool notify = true);
		void addSalary3(DWORD num, const char *disc , const char *note = NULL, bool notify = true);
		void addCharvip(DWORD num, const char *disc , const char *note = NULL, bool notify = true); //VIP点数
		void addChongzhi(DWORD num, const char *disc , const char *note = NULL, bool notify = true); //脚本增加充值点1
		void addChongzhi2(DWORD num, const char *disc , const char *note = NULL, bool notify = true); //脚本增加充值点2
		void addChangejob(DWORD num, const char *disc , const char *note = NULL, bool notify = true);
		void addFolwers(DWORD num, const char *disc , const char *note = NULL, bool notify = true);
		void addAngel(DWORD num, const char *disc , const char *note = NULL, bool notify = true);
		void addHanbing(DWORD num, const char *disc , const char *note = NULL, bool notify = true);
		void addMoods(DWORD num, const char *disc , const char *note = NULL, bool notify = true);
		void addFlloves(DWORD num, const char *disc , const char *note = NULL, bool notify = true);
		bool checkMoney(DWORD need);
		bool checkTicket(DWORD need);
		bool checkSalary(DWORD need);
		bool checkCharvip(DWORD need);
		bool checkChongzhi(DWORD need); //检测充值点
        bool checkChangejob(DWORD need);
		bool checkFolwers(DWORD need);
		bool checkMoods(DWORD need);
		bool checkFlloves(DWORD need);
		bool checkAngel(DWORD need);
		bool checkHanbing(DWORD need);
		bool checkGold(DWORD need);
		bool removeMoney(DWORD num ,const char *disc);
		bool removeGold(DWORD num ,const char *disc , bool need=true);
		bool removeTicket(DWORD num ,const char *disc);
		bool removeGoldgive(DWORD num ,const char *disc);
		bool removeSalary(DWORD num ,const char *disc);
		bool removeCharvip(DWORD num ,const char *disc);
		bool removeChongzhi(DWORD num ,const char *disc); //扣除充值点
		bool removeChangejob(DWORD num ,const char *disc);
		bool removeFolwers(DWORD num ,const char *disc);
		bool removeAngel(DWORD num ,const char *disc);
		bool removeHanbing(DWORD num ,const char *disc);
		bool removeMoods(DWORD num ,const char *disc);
		bool removeFlloves(DWORD num ,const char *disc);
		
		bool moveObject(SceneUser *pUser,zObject *srcObj,stObjectLocation &dst);
		bool moveObjectToScene(zObject *o , const zPos &pos , DWORD overdue_msecs=0 , const unsigned long dwID=0);
		bool removeObject(zObject *srcObj, bool notify = true, bool del = true);
		bool addObject(zObject *srcObj, bool needFind, int packs = 0);
		Package* getPackage(DWORD type,DWORD id);
		SceneUser *getOwner()
		{
			return owner;
		}
		
		template<typename T>
		void execEvery(Package* pack, T t)
		{
			typename T::BASE cb(this);
			pack->execEvery(cb);
		}
		
		template<typename T, typename P1>
		void execEvery(Package* pack, T t, P1 p1)
		{
			typename T::BASE cb(this, p1);
			pack->execEvery(cb);
		}
		
		zObject *getGold();
		DWORD getGoldNum();
};

#define AUTO_PACK Packages::MAIN_PACK |Packages::LEFT_PACK| Packages::RIGHT_PACK

struct Combination : public PackageCallback
{
public:
	Combination(SceneUser* user, zObject* get) : _user(user), _get(get), _num(0)
	{ }

	virtual bool exec(zObject* o);

	int num() const
	{
		return _num;
	}

private:
	SceneUser* _user;
	zObject* _get;
	int _num;
};


struct Combination2 : public PackageCallback
{
public:
	Combination2(SceneUser* user, zObject* get) : _user(user), _get(get), _num(0)
	{ }

	virtual bool exec(zObject* o);

	int num() const
	{
		return _num;
	}

private:
	SceneUser* _user;
	zObject* _get;
	int _num;
};
struct ClearPack : public PackageCallback
{
public: 
	ClearPack(const Packages* ps) : _ps(ps)
	{ }
	
	virtual bool exec(zObject* p);
private:
	const Packages* _ps;
};

struct DropFromPack : public PackageCallback
{
public:
	struct Param
	{
		Package* pack;
		int drop_num;
		zPos pos;

		Param(Package* p, int d, const zPos& s) : pack(p), drop_num(d), pos(s)
		{ }
	};
	
	DropFromPack(const Packages* ps, const Param&  p) ;
	virtual bool exec(zObject* ob);
private:
	const Packages* _ps;
	Param _p;

	int _begin;
	int _pos;
};


#endif

