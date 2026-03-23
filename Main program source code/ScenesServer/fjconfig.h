
#include "zType.h"
#include "zRWLock.h"
#include "zDatabase.h"

class fjconfig
{

	public:
		
		struct	SJ
		{
		DWORD	level;		// 编号
		DWORD	value;		// 物品ID
		DWORD	costID;		// 材料类型
		DWORD	costNum;		// 物品类别
		DWORD	salary;		// 物品类别
		DWORD	pDam;		// 物品类别
		DWORD	mDam;		// 物品类别
		DWORD	pDef;		// 物品类别
		DWORD	mDef;		// 物品类别
		DWORD	hp;		// 物品类别
		};
		struct	ZTZ
		{
		DWORD	level;		// 编号
		DWORD	value;		// 物品ID
		DWORD	costID;		// 材料类型
		DWORD	costNum;		// 物品类别
		DWORD	salary;		// 物品类别
		DWORD	pDam;		// 物品类别
		DWORD	mDam;		// 物品类别
		DWORD	pDef;		// 物品类别
		DWORD	mDef;		// 物品类别
		DWORD	hp;		// 物品类别
		};
		struct	SHENGXIAO
		{
		DWORD	level;		// 编号
		DWORD	value;		// 物品ID
		DWORD	costID;		// 材料类型
		DWORD	costNum;		// 物品类别
		DWORD	salary;		// 物品类别
		DWORD	pDam;		// 物品类别
		DWORD	mDam;		// 物品类别
		DWORD	pDef;		// 物品类别
		DWORD	mDef;		// 物品类别
		DWORD	hp;		// 物品类别
		};
		struct	SHENGQI
		{
		DWORD	level;		// 编号
		DWORD	value;		// 物品ID
		DWORD	costID;		// 材料类型
		DWORD	costNum;		// 物品类别
		DWORD	salary;		// 物品类别
		DWORD	pDam;		// 物品类别
		DWORD	mDam;		// 物品类别
		DWORD	pDef;		// 物品类别
		DWORD	mDef;		// 物品类别
		DWORD	hp;		// 物品类别
		};
		struct	JINGMAI
		{
		DWORD	level;		// 编号
		DWORD	value;		// 物品ID
		DWORD	costID;		// 材料类型
		DWORD	costNum;		// 物品类别
		DWORD	salary;		// 物品类别
		DWORD	pDam;		// 物品类别
		DWORD	mDam;		// 物品类别
		DWORD	pDef;		// 物品类别
		DWORD	mDef;		// 物品类别
		DWORD	hp;		// 物品类别
		};
		struct	TOUXIAN //头衔
		{
		DWORD	level;		// 编号
		DWORD	exp;		// 经验
		DWORD	costID;		// 材料类型
		DWORD	salary;		// 物品类别
		DWORD	pDam;		// 物品类别
		DWORD	mDam;		// 物品类别
		DWORD	pDef;		// 物品类别
		DWORD	mDef;		// 物品类别
		DWORD	hp;		// 物品类别
		};
		struct	CHENGHAO //称号
		{
		DWORD	id;		// 编号
		DWORD	stateID;		// 经验
		DWORD	pDam;		// 物品类别
		DWORD	mDam;		// 物品类别
		DWORD	pDef;		// 物品类别
		DWORD	mDef;		// 物品类别
		DWORD	hp;		// 物品类别
		};
		struct	ERNV //儿女
		{
		DWORD	level;		// 编号
		DWORD	exp;		// 经验
		DWORD	costID;		// 材料类型
		DWORD	salary;		// 物品类别
		DWORD	pDam;		// 物品类别
		DWORD	mDam;		// 物品类别
		DWORD	pDef;		// 物品类别
		DWORD	mDef;		// 物品类别
		DWORD	hp;		// 物品类别
		DWORD	ll;
		DWORD	zl;
		DWORD	js;
		DWORD	mj;
		DWORD	tz;
		DWORD	skillpoint;
		
		};
		struct	HUISHOU //道具回收
		{
		DWORD	id;		// 编号
		DWORD	objectID;		// 经验
		char	name[MAX_NAMESIZE];		// 物品名称
		DWORD	price;		// 物品类别
		DWORD	type;		// 物品类别
		};

		struct	BIESHU //云天别墅
		{
		DWORD	level;		// 等级
		DWORD	exp;		// 经验
		DWORD	costID;		// 材料类型
		DWORD	salary;		// 物品类别
		DWORD	gong;		// 物品类别
		DWORD	fang;		// 物品类别
		DWORD	hp;		// 物品类别
		};
		struct	XIULIAN //能力修炼
		{
		DWORD	level;		// 编号
		DWORD	exp;		// 经验
		DWORD	salary;		// 物品类别
		DWORD	pDam;		// 物品类别
		DWORD	mDam;		// 物品类别
		DWORD	pDef;		// 物品类别
		DWORD	mDef;		// 物品类别
		DWORD	hp;		// 物品类别
		};
		struct	ZUOQI //坐骑图鉴
		{
		DWORD	level;		// 编号
		DWORD	exp;		// 经验
		DWORD	costID;		// 物品类别
		DWORD	salary;		// 物品类别
		DWORD	ll;
		DWORD	zl;
		DWORD	js;
		DWORD	mj;
		DWORD	tz;
		DWORD	speed;
		DWORD	wugong;
		DWORD	mogong;
		DWORD	wufang;
		DWORD	mofang;
		DWORD	pDam;		// 物品类别
		DWORD	mDam;		// 物品类别
		DWORD	pDef;		// 物品类别
		DWORD	mDef;		// 物品类别
		DWORD	hp;		// 物品类别
		};
		struct	MOHE //时装魔盒
		{
		DWORD	level;		// 编号
		DWORD	exp;		// 经验
		DWORD	pDam;		// 物品类别
		DWORD	mDam;		// 物品类别
		DWORD	pDef;		// 物品类别
		DWORD	mDef;		// 物品类别
		DWORD	hp;		// 物品类别
		};
		struct	SHIZHUANG //时装
		{
		char	name[MAX_NAMESIZE];		// 物品名称
		DWORD	exp;		// 经验
		DWORD	costID;		// 物品类别
		DWORD	jihuoID;		// 物品类别
		DWORD	jihuoNum;		// 物品类别
		DWORD	bodyNum;		// 物品类别
		};
		struct	SHIZHUANGLEVEL //时装
		{
		DWORD   level;		// 等级
		DWORD	exp;		// 经验
		DWORD	costID;		// 物品类别
		DWORD	costNum;		// 物品类别
		DWORD	wugong;		// 物品类别
		DWORD	mogong;		// 物品类别
		DWORD   hp;
		DWORD   shengming;
		DWORD   fashu;
		DWORD   bang;
		};

		struct	PIFENG //披风
		{
		char	name[MAX_NAMESIZE];		// 物品名称
		DWORD	exp;		// 经验
		DWORD	costID;		// 物品类别
		DWORD	jihuoID;		// 物品类别
		DWORD	jihuoNum;		// 物品类别
		DWORD	itemNum;		// 物品类别
		};
		struct	PIFENGLEVEL //披风
		{
		DWORD   level;		// 等级
		DWORD	exp;		// 经验
		DWORD	costID;		// 物品类别
		DWORD	costNum;		// 物品类别
		DWORD	wugong;		// 物品类别
		DWORD	mogong;		// 物品类别
		DWORD   hp;
		DWORD   shengming;
		DWORD   fashu;
		DWORD   bang;
		};

		struct	CHIBANG //翅膀
		{
		char	name[MAX_NAMESIZE];		// 物品名称
		DWORD	exp;		// 经验
		DWORD	costID;		// 物品类别
		DWORD	jihuoID;		// 物品类别
		DWORD	jihuoNum;		// 物品类别
		DWORD	itemNum;		// 物品类别
		};
		struct	CHIBANGLEVEL //翅膀
		{
		DWORD   level;		// 等级
		DWORD	exp;		// 经验
		DWORD	costID;		// 物品类别
		DWORD	costNum;		// 物品类别
		DWORD	wugong;		// 物品类别
		DWORD	mogong;		// 物品类别
		DWORD   hp;
		DWORD   shengming;
		DWORD   fashu;
		DWORD   bang;
		};

		struct	ZUOQI2 //坐骑
		{
		char	name[MAX_NAMESIZE];		// 物品名称
		DWORD	exp;		// 经验
		DWORD	npcID;		// 物品类别
		DWORD	jihuoID;		// 物品类别
		DWORD	jihuoNum;		// 物品类别
		DWORD	npcNum;		// 物品类别
		};
		static fjconfig &getInstance()
		{
			if (NULL == instance)
				instance = new fjconfig();

			return *instance;
		}
		std::vector<SJ> fjlist;
		std::vector<ZTZ> ztzlist;
		std::vector<SHENGXIAO> shengxiaolist;
		std::vector<SHENGQI> shengqilist;
		std::vector<JINGMAI> jingmailist;
		std::vector<TOUXIAN> touxianlist;
		std::vector<CHENGHAO> chenghaolist;
		std::vector<ERNV> ernvlist;
		std::vector<HUISHOU> huishoulist;
		std::vector<BIESHU> bieshulist;
		std::vector<XIULIAN> xiulianlist;
		std::vector<ZUOQI> zuoqilist;
		std::vector<MOHE> mohelist;
		std::vector<SHIZHUANG> shizhuangnanlist;
		std::vector<SHIZHUANG> shizhuangnvlist;
		std::vector<SHIZHUANGLEVEL> shizhuanglevellist;
		std::vector<PIFENG> pifenglist;
		std::vector<PIFENGLEVEL> pifenglevellist;
		std::vector<CHIBANG> chibanglist;
		std::vector<CHIBANGLEVEL> chibanglevellist;
		std::vector<ZUOQI2> zuoqi2list;
		~fjconfig()
		{

		}
		bool init();
		bool initztz();
		bool initshengxiao();
		bool initshengqi();
		bool initjingmai();
		bool initTouxian();
		bool initChenghao();
		bool initErnv();
		bool initHuishou();
		bool initBieshu();
		bool initXiulian();
		bool initZuoqi();
		bool initMohe();
		bool initShizhuang();
		bool initShiZhuangLevel();
		bool initPifeng();
		bool initPiFengLevel();
		bool initChibang();
		bool initChiBangLevel();
		bool initZuoqi2();
		
		
	private:
		static fjconfig *instance;
		fjconfig() {};

		
	
};
