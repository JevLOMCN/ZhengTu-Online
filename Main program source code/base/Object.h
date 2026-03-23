#ifndef _OBJECT_H
#define _OBJECT_H

typedef unsigned long tItemThisID ;
#define INVALID_THISID 0xffffffff

// 道具的类型 这里就是物品类型定义
enum enumItemType
{
	ItemType_None,

	ItemType_Scrxll = 15,	//soke 15代表新物品类
	ItemType_Resource = 16,	//16代表原料类
	ItemType_Leechdom,	//17代表药品类
	ItemType_FoodRes,	//18代表食物原料类
	ItemType_Food,		//19代表食物类
	ItemType_Tools,		//20代表劳动工具类
	ItemType_Arrow,		//21代表配合弓使用的箭支类
	ItemType_BattleHorse,	//22战马
	ItemType_Pack,		//23代表包裹类
	ItemType_Money,		//24代表金钱类
	ItemType_Scroll,	//25代表转移卷轴类
	ItemType_Move,		//26代表特殊移动道具类
	ItemType_LevelUp,	//27代表道具升级需要的材料类 如各种升级宝石
	ItemType_CaptureWeapon,	//28代表驯服宠物用武器
	ItemType_Union,	//29代表创建帮会需要的道具.
	ItemType_Tonic,	//30表示自动补药类道具.
	ItemType_Gift,	//31代表礼品类物品.
	ItemType_Other, //32右键使用召唤NPC
	ItemType_MASK = 33, 	//33代表蒙面巾
	ItemType_Quest = 34,
	ItemType_HORSE = 35,
	ItemType_SOULSTONE = 37, //37代表魂魄石类
	ItemType_Wedding = 38, //38代表婚礼类
	ItemType_Change = 41,   //41 代表合成道具
	ItemType_Auto = 42,   //42 代表自动练功
	ItemType_SkillUp = 43,   //43 代表技能升级道具
	ItemType_Book = 44, //44代表书籍
	ItemType_Store = 45,   //45 代表仓库
	ItemType_Renew = 46,   //46 代表洗点道具
	ItemType_Repair = 47, //47代表修复宝石类
	ItemType_Xuest = 49,  //soke 增加个来支持购买绑定
	ItemType_DoubleExp = 52, //52代表双倍经验类型
	ItemType_Honor = 53, //53代表荣誉之星类型
	ItemType_TONG = 54,  //帮主令	
	ItemType_FAMILY = 55,  //家族令
	ItemType_Adonment = 56, //56代表装饰品
	ItemType_SpecialBook = 57, //57代表特殊书籍
	ItemType_GreatLeechdom = 58, //58大计量自动补血道具
	ItemType_ClearProperty = 59, //59洗点道具
	ItemType_UseSkill = 60, // 附带技能类道具
	ItemType_Amulet = 61, // 护身符类道具
	ItemType_GreatLeechdomMp = 62,//62大计量自动补蓝道具
	ItemType_KING = 65,  //国王令

	ItemType_DoubleExpZ =  66,  //中级经验倍率物品
	ItemType_DoubleExpC =  67,  //初级经验倍率物品
	ItemType_DoubleExpG = 68,   //高级经验倍率物品
	ItemType_GreatLeechdomHp = 72,//72大计量自动补血道具
	
	ItemType_Tianxia = 73,    // 天下令
	ItemType_LuncyDan = 74,   // 幸运丹
	ItemType_HzStone = 78,    // 合成魂魄石
	ItemType_Amulets = 80,    // 护身符
	ItemType_ShangfangBaojian = 88,      // 尚方宝剑
	ItemType_ZijinLongpao = 89,   // 紫金龙袍
	ItemType_Xunzhang = 90,   // 勋章
	ItemType_Hanbing = 91,    // 寒冰麒麟
	ItemType_Chibang = 93,    // 翅膀
	ItemType_Shenbing0 = 94,  // 金箍棒
	ItemType_Shenbing1 = 95,  // 万剑阵
	ItemType_Shenbing2 = 96,  // 王者之刃
	ItemType_Shenshou1 = 97,  // 金月神兽
	ItemType_Shenshou2 = 98,  // 白月神兽
	ItemType_pifeng = 99,    // 披风
	ItemType_ClothBody =101,		//101代表布质加生命类服装
	ItemType_FellBody =102,		    //102代表皮甲加魔防类服装
	ItemType_MetalBody =103,		//103代表金属铠甲加物防类服装
	ItemType_Blade =104,		    //104代表武术刀类武器
	ItemType_Sword =105,	        //105代表武术剑类武器
	ItemType_Axe =106,	           //106代表武术斧类武器
	ItemType_Hammer =107,	        //107代表武术斧类武器
	ItemType_Staff =108,		    //108代表法术杖类武器
	ItemType_Crossbow =109,	        //109代表箭术弓类武器
	ItemType_Fan =110,	           //110代表美女扇类
	ItemType_Stick =111,	        //111代表召唤棍类武器
	ItemType_Shield =112,	//112代表盾牌类
	ItemType_Helm =113,		//113代表角色头盔类
	ItemType_Caestus =114,	//114代表角色腰带类
	ItemType_Cuff = 115,		//115代表角色护腕类
	ItemType_Shoes = 116,		//116代表角色鞋子类
	ItemType_Necklace = 117,	//117代表角色项链类
	ItemType_Fing = 118,		//118代表角色戒指类
	ItemType_FashionBody = 119,		//119代表时装
	ItemType_Flower = 120,		    //120代表鲜花,采集手套...
	ItemType_BMW = 121,		        //119代表坐骑
	ItemType_HighFashionBody = 124, //124代表高级时装
	ItemType_XHighFashionBody = 125,//125代表新高级时装
	ItemType_SHighFashionBody = 126,//126代表新高级透明时装
	ItemType_Add_Exploit = 128,     //128代表金符类
	ItemType_HorseShoe=130,	        //130代表马脚
	ItemType_HorseRope=131,	        //131代表马绳
	ItemType_HorseSaddle=132,	    //132代表马鞍
	ItemType_HorseSafe=133,	        //133代表马甲
	ItemType_HorseIron=134,	        //134代表马镫
	ItemType_Dagger=136,	        //136代表利刃
	ItemType_NewSword=137,	        //137代表卫士剑
	ItemType_breastplate=138,	    //138代表护心镜
	ItemType_HorseFashion=140,	    //140代表马铃铛
	ItemType_StaffFlag =141,		//141代表战旗(法师)
	ItemType_CrossFlag =142,	    //142代表战旗(弓手)
	ItemType_Flag =143,	            //143代表战旗(仙术)
	ItemType_StickFlag =144,	    //144代表战旗(召唤)
	ItemType_BladeFlag =145,	    //145代表战旗(战士)
	ItemType_DaggerFlag=146,	    //146代表战旗(刺客)
	ItemType_SwordFlag=147,         //147代表战旗(卫士)
	ItemType_LongGun=155,           //155代表长枪
	ItemType_Handgun=156,           //156代表短枪
	ItemType_AntiquityStone=203,    //203代表补天石    
	ItemType_AntiquityStone_2=204,  //204代表补天灵石 
	ItemType_AntiquityStone_3=205,  //205代表补天神石 
	ItemType_AntiquityStone_4=206,  //206代表帝王神石
	ItemType_AntiquityStone_5=207,  //207代表弑天圣石
	
    ItemType_handbookitem1=221,	    //图鉴1
	ItemType_handbookitem2=222,	    //图鉴2
	ItemType_handbookitem3=223,	    //图鉴3
	ItemType_handbookitem4=224,	    //图鉴4
	ItemType_handbookitem5=225,	    //图鉴5
	ItemType_handbookitem6=226,	    //图鉴6
	ItemType_handbookitem7=227,	    //图鉴7
	ItemType_handbookitem8=228,	    //图鉴8
	ItemType_handbookitem9=229,	    //图鉴9
	ItemType_handbookitem10=230,	//图鉴10	

	ItemType_Fabao = 240,  // 220代表法宝类
	ItemType_Fabaofj1 = 241,  // 221代表法宝附件1
	ItemType_Fabaofj2 = 242,  // 222代表法宝附件2
	ItemType_Fabaofj3 = 243,  // 223代表法宝附件3
	ItemType_Fabaofj4 = 244,  // 224代表法宝附件4
	ItemType_Fabaofj5 = 245,  // 225代表法宝附件5
	ItemType_Fabaofj6 = 246,  // 226代表法宝附件6
	ItemType_Fabaofj7 = 247,  // 227代表法宝附件7
	ItemType_Fabaofj8 = 248,  // 228代表法宝附件8
	ItemType_Fabaofj9 = 249,  // 229代表法宝附件9
	ItemType_Fabaofj10 = 250,  // 230代表法宝附件10
	ItemType_Fabaofj11 = 251,  // 231代表法宝附件11
	ItemType_Fabaofj12 = 252,  // 232代表法宝附件12

	
	ItemType_YUANSHEN = 260,  // 240代表元神类
	ItemType_YUANSHENFJ1 = 261,  // 241代表元神附件1
	ItemType_YUANSHENFJ2 = 262,  // 242代表元神附件2
	ItemType_YUANSHENFJ3 = 263,  // 243代表元神附件3
	ItemType_YUANSHENFJ4 = 264,  // 244代表元神附件4
	ItemType_YUANSHENFJ5 = 265,  // 245代表元神附件5
	ItemType_YUANSHENFJ6 = 266,  // 246代表元神附件6

	ItemType_SEXX1 = 280,  // 221代表十二星宿附件1
	ItemType_SEXX2 = 281,  // 222代表十二星宿附件2
	ItemType_SEXX3 = 282,  // 223代表十二星宿附件3
	ItemType_SEXX4 = 283,  // 224代表十二星宿附件4
	ItemType_SEXX5 = 284,  // 225代表十二星宿附件5
	ItemType_SEXX6 = 285,  // 226代表十二星宿附件6
	ItemType_SEXX7 = 286,  // 227代表十二星宿附件7
	ItemType_SEXX8 = 287,  // 228代表十二星宿附件8
	ItemType_SEXX9 = 288,  // 229代表十二星宿附件9
	ItemType_SEXX10 = 289,  // 230代表十二星宿附件10
	ItemType_SEXX11 = 290,  // 231代表十二星宿附件11
	ItemType_SEXX12 = 291,  // 232代表十二星宿附件12

	ItemType_BABYFJ1 = 300,  // 241代表宝宝附件1
	ItemType_BABYFJ2 = 301,  // 242代表宝宝附件2
	ItemType_BABYFJ3 = 302,  // 243代表宝宝附件3
	ItemType_BABYFJ4 = 303,  // 244代表宝宝附件4
	ItemType_BABYFJ5 = 304,  // 245代表宝宝附件5
	ItemType_BABYFJ6 = 305,  // 246代表宝宝附件6
};

#define BOW_ARROW_ITEM_TYPE 21

#define BOW2_ARROW_ITEM_TYPE 142

namespace Zt_Object
{

enum {
	INVALID_LOC = (DWORD) -1,
	INVALID_TAB = (DWORD) -1,
	INVALID_X = (WORD) -1,
	INVALID_Y = (WORD) -1,
};

}

#pragma pack(1)

struct stObjectLocation{

private:
	DWORD dwLocation;	// 格子类型
	DWORD dwTableID;	// 包袱ID
	WORD  x;
	WORD  y;

	friend class Package;
public:
	stObjectLocation() 
		: dwLocation(Zt_Object::INVALID_LOC), dwTableID(Zt_Object::INVALID_TAB), 
		x(Zt_Object::INVALID_X), y(Zt_Object::INVALID_Y)
	{

	}

	stObjectLocation(const stObjectLocation& loc) 
		: dwLocation(loc.dwLocation), dwTableID(loc.dwTableID), x(loc.x), y(loc.y)
	{

	}

	stObjectLocation(DWORD loc, DWORD tab, WORD _x, WORD _y) 
		: dwLocation(loc), dwTableID(tab), x(_x), y(_y)
	{

	}
	
	void operator = (const stObjectLocation& loc)
	{
		dwLocation =loc.dwLocation;
		dwTableID = loc.dwTableID;
		x = loc.x;
		y = loc.y;
	}
	
	bool operator == (const stObjectLocation & st) const
	{
		return dwLocation == st.dwLocation && dwTableID == st.dwTableID && x == st.x && y == st.y;
	}

	bool operator != (const stObjectLocation & st) const
	{
		return !(*this == st);
	}

	DWORD tab() const
	{
		return dwTableID;
	}

	void tab(DWORD t) 
	{
		dwTableID = t;
	}

	DWORD loc() const
	{
		return dwLocation;
	}

	WORD xpos() const
	{
		return x;
	}

	WORD ypos() const
	{
		return y;
	}
};

struct oskill
{
	DWORD id;
	BYTE level;
	BYTE count;
	BYTE maxcount;
	BYTE other;
};

typedef struct _Object
{
	DWORD qwThisID;   //物品唯一id
	DWORD dwObjectID;  ////物品类别id
	char strName[MAX_NAMESIZE]; //名称
	
	stObjectLocation pos;	// 位置
	DWORD dwNum;	// 数量
	BYTE upgrade;//物品升级等级
	WORD kind;	//物品类型, 0普通, 1蓝色, 2金色, 4神圣, 8套装
	DWORD exp;  //道具经验
	
	
	WORD needlevel;				// 需要等级
	DWORD round;				// 转生次数
	DWORD xingshu;				// 历史升星数
	WORD butian;		    	// 补天等级
	DWORD douhun;				// 斗魂星数

	WORD dtrough;			// 龙槽等级
	WORD drastar;			// 龙星等级
	WORD dmedalt;			// 勋章等级
	WORD retain1;			// 寒冰等级

	WORD retain2;			// 装备栏
	WORD retain3;			// 神器
	WORD retain4;			// 翅膀
	WORD retain5;			// 神兵
	WORD retain6;			// 神兽
	WORD retain7;			// 装备升级
	WORD retain8;			// 预留参数
	WORD retain9;			// 预留参数
	WORD handbooklvg;		// 百兽图鉴进阶次数		
	WORD longwanglv;		// 龙王套装等级
	WORD nitianlv;			// 逆天进阶等级
	WORD douzhansflv;		// 斗战圣佛等级	
	WORD aystonelv;			// 暗影石等级	
	WORD shangfanglv;		// 尚方宝剑等级
	WORD zijinlplv;			// 紫金龙袍等级	
	WORD retain17;			// 预留参数
	WORD retain18;			// 预留参数
	WORD retain19;			// 预留参数
	WORD retain20;			// 预留参数
	WORD retain21;			// 十二星宿
	WORD retain22;			// 预留参数
	WORD retain23;			// 预留参数
	WORD retain24;			// 龙凤吟等级
	WORD retain25;			// 龙星升级
	WORD retain26;			// 龙星升级
	WORD retain27;			// 龙凤吟升级
	WORD retain28;			// 龙凤吟升级
	WORD retain29;			// 预留参数
	WORD retain30;			// 预留参数
	WORD retain31;			// 龙星重铸
	WORD retain32;			// 神斧
	WORD retain33;			// 黑曜
	WORD retain34;			// 精金
	WORD retain35;
	WORD retain36;          
	WORD retain37;
	WORD retain38;
	WORD retain39;
	WORD retain40;
	WORD syxqgrade21;			// 金箍咒强化
	WORD syxqgrade22;			// 金箍咒字纹
	WORD syxqgrade23;			// 金箍咒淬炼
	WORD syxqgrade24;			// 寒冰神炼
	WORD syxqgrade25;			// 寒冰字纹
	WORD syxqgrade26;			// 寒冰纹章
	WORD retain47;
	WORD retain48;
	WORD retain49;
	WORD retain50;
	WORD retain51;
	WORD retain52;
	WORD retain53;
	WORD retain54;
	WORD retain55;
	WORD retain56;
	WORD retain57;
	WORD retain58;
	WORD retain59;
	WORD retain60;
	WORD retain61;
	WORD retain62;
	WORD retain63;
	WORD retain64;
	WORD retain65;
	WORD retain66;
	WORD retain67;
	WORD retain68;
	WORD retain69;
	WORD retain70;
	WORD retain71;
	WORD retain72;
	WORD retain73;
	WORD retain74;
	WORD retain75;
	WORD retain76;
	WORD retain77;
	WORD retain78;
	WORD retain79;
	WORD retain80;
	WORD retain81;
	WORD retain82;
	WORD retain83;
	WORD retain84;
	WORD retain85;
	WORD retain86;
	WORD retain87;
	WORD retain88;
	WORD retain89;
	WORD retain90;
	WORD retain91;
	WORD retain92;
	WORD retain93;
	WORD retain94;
	WORD retain95;
	WORD retain96;
	WORD retain97;
	WORD retain98;
	WORD retain99;
	WORD retain100;
	WORD retain101;
	WORD retain102;
	WORD retain103;
	WORD retain104;
	WORD retain105;
	WORD retain106;
	WORD retain107;
	WORD retain108;
	WORD retain109;
	WORD retain110;
	WORD retain111;
	WORD retain112;
	WORD retain113;
	WORD retain114;
	WORD retain115;
	WORD retain116;
	WORD retain117;
	WORD retain118;
	WORD retain119;
	WORD retain120;
	WORD retain121;
	WORD retain122;
	WORD retain123;
	WORD retain124;
	WORD retain125;
	WORD retain126;
	WORD retain127;
	WORD retain128;
	WORD retain129;
	WORD retain130;
	WORD retain131;


	WORD itemrefine;		// 马牌精炼
	WORD itemksAmulet;		// 护身符宝石镶嵌次数
	WORD itemkscuican;		// 护身符璀璨宝石镶嵌次数
						
	DWORD maxhp;			// 最大生命值
	DWORD maxmp;			// 最大法术值
	DWORD maxsp;			// 最大体力值

	DWORD pdamage;			// 最小攻击力
	DWORD maxpdamage;		// 最大攻击力
	DWORD mdamage;			// 最小法术攻击力
	DWORD maxmdamage;		// 最大法术攻击力

	DWORD pdefence;			// 物防
	DWORD mdefence;			// 魔防

	DWORD m1axhp;			//sky 绿装升星 最大生命值
	DWORD p1damage;			//sky 绿装升星 最小攻击力
	DWORD m1axpdamage;		//sky 绿装升星 最大攻击力
	DWORD m1damage;			//sky 绿装升星 最小法术攻击力
	DWORD m1axmdamage;		//sky 绿装升星 最大法术攻击力
	DWORD p1defence;		//sky 绿装升星 物防
	DWORD m1defence;		//sky 绿装升星 魔防
	DWORD m2axhp;			//sky 紫装升星 最大生命值
	DWORD p2damage;			//sky 紫装升星 最小攻击力
	DWORD m2axpdamage;		//sky 紫装升星 最大攻击力
	DWORD m2damage;			//sky 紫装升星 最小法术攻击力
	DWORD m2axmdamage;		//sky 紫装升星 最大法术攻击力
	DWORD p2defence;		//sky 紫装升星 物防
	DWORD m2defence;		//sky 紫装升星 魔防
	DWORD m3axhp;			//sky 斗魂升级 最大生命值
	DWORD p3damage;			//sky 斗魂升级 最小攻击力
	DWORD m3axpdamage;		//sky 斗魂升级 最大攻击力
	DWORD m3damage;			//sky 斗魂升级 最小法术攻击力
	DWORD m3axmdamage;		//sky 斗魂升级 最大法术攻击力
	DWORD p3defence;		//sky 斗魂升级 物防
	DWORD m3defence;		//sky 斗魂升级 魔防
	
	DWORD m4axhp;			//sky 龙星激活/解封 最大生命值
	DWORD p4damage;			//sky 龙星激活/解封 最小攻击力
	DWORD m4axpdamage;		//sky 龙星激活/解封 最大攻击力
	DWORD m4damage;			//sky 龙星激活/解封 最小法术攻击力
	DWORD m4axmdamage;		//sky 龙星激活/解封 最大法术攻击力
	DWORD p4defence;		//sky 龙星激活/解封 物防
	DWORD m4defence;		//sky 龙星激活/解封 魔防

	DWORD m5axhp;			//sky 龙星升级 最大生命值
	DWORD p5damage;			//sky 龙星升级 最小攻击力
	DWORD m5axpdamage;		//sky 龙星升级 最大攻击力
	DWORD m5damage;			//sky 龙星升级 最小法术攻击力
	DWORD m5axmdamage;		//sky 龙星升级 最大法术攻击力
	DWORD p5defence;		//sky 龙星升级 物防
	DWORD m5defence;		//sky 龙星升级 魔防
	
	DWORD m6axhp;			//sky 补天 最大生命值
	DWORD p6damage;			//sky 补天 最小攻击力
	DWORD m6axpdamage;		//sky 补天 最大攻击力
	DWORD m6damage;			//sky 补天 最小法术攻击力
	DWORD m6axmdamage;		//sky 补天 最大法术攻击力
	DWORD p6defence;		//sky 补天 物防
	DWORD m6defence;		//sky 补天 魔防
	
	DWORD m7axhp;			//sky 勋章 最大生命值
	DWORD p7damage;			//sky 勋章 最小攻击力
	DWORD m7axpdamage;		//sky 勋章 最大攻击力
	DWORD m7damage;			//sky 勋章 最小法术攻击力
	DWORD m7axmdamage;		//sky 勋章 最大法术攻击力
	DWORD p7defence;		//sky 勋章 物防
	DWORD m7defence;		//sky 勋章 魔防
	
	DWORD m8axhp;			//sky 寒冰 最大生命值
	DWORD p8damage;			//sky 寒冰 最小攻击力
	DWORD m8axpdamage;		//sky 寒冰 最大攻击力
	DWORD m8damage;			//sky 寒冰 最小法术攻击力
	DWORD m8axmdamage;		//sky 寒冰 最大法术攻击力
	DWORD p8defence;		//sky 寒冰 物防
	DWORD m8defence;		//sky 寒冰 魔防
	
	DWORD m9axhp;			//sky 神器 最大生命值
	DWORD p9damage;			//sky 神器 最小攻击力
	DWORD m9axpdamage;		//sky 神器 最大攻击力
	DWORD m9damage;			//sky 神器 最小法术攻击力
	DWORD m9axmdamage;		//sky 神器 最大法术攻击力
	DWORD p9defence;		//sky 神器 物防
	DWORD m9defence;		//sky 神器 魔防

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
	DWORD m12defence;		   //MYY 神兽宝石增加 魔防

	DWORD m13axhp;			   //MYY 百兽图鉴增加 最大生命值
	DWORD p13damage;		   //MYY 百兽图鉴增加 最小攻击力
	DWORD m13axpdamage;		   //MYY 百兽图鉴增加 最大攻击力
	DWORD m13damage;		   //MYY 百兽图鉴增加 最小法术攻击力
	DWORD m13axmdamage;		   //MYY 百兽图鉴增加 最大法术攻击力
	DWORD p13defence;		   //MYY 百兽图鉴增加 物防
	DWORD m13defence;		   //MYY 百兽图鉴增加 魔防

	DWORD m14axhp;			   //MYY 龙王套装增加 最大生命值
	DWORD p14damage;		   //MYY 龙王套装增加 最小攻击力
	DWORD m14axpdamage;		   //MYY 龙王套装增加 最大攻击力
	DWORD m14damage;		   //MYY 龙王套装增加 最小法术攻击力
	DWORD m14axmdamage;		   //MYY 龙王套装增加 最大法术攻击力
	DWORD p14defence;		   //MYY 龙王套装增加 物防
	DWORD m14defence;		   //MYY 龙王套装增加 魔防

	DWORD m15axhp;			   //MYY 逆天进阶增加 最大生命值
	DWORD p15damage;		   //MYY 逆天进阶增加 最小攻击力
	DWORD m15axpdamage;		   //MYY 逆天进阶增加 最大攻击力
	DWORD m15damage;		   //MYY 逆天进阶增加 最小法术攻击力
	DWORD m15axmdamage;		   //MYY 逆天进阶增加 最大法术攻击力
	DWORD p15defence;		   //MYY 逆天进阶增加 物防
	DWORD m15defence;		   //MYY 逆天进阶增加 魔防

	DWORD m16axhp;			   //MYY 斗战圣佛增加 最大生命值
	DWORD p16damage;		   //MYY 斗战圣佛增加 最小攻击力
	DWORD m16axpdamage;		   //MYY 斗战圣佛增加 最大攻击力
	DWORD m16damage;		   //MYY 斗战圣佛增加 最小法术攻击力
	DWORD m16axmdamage;		   //MYY 斗战圣佛增加 最大法术攻击力
	DWORD p16defence;		   //MYY 斗战圣佛增加 物防
	DWORD m16defence;		   //MYY 斗战圣佛增加 魔防

	DWORD m17axhp;			   //MYY 暗影石增加 最大生命值
	DWORD p17damage;		   //MYY 暗影石增加 最小攻击力
	DWORD m17axpdamage;		   //MYY 暗影石增加 最大攻击力
	DWORD m17damage;		   //MYY 暗影石增加 最小法术攻击力
	DWORD m17axmdamage;		   //MYY 暗影石增加 最大法术攻击力
	DWORD p17defence;		   //MYY 暗影石增加 物防
	DWORD m17defence;		   //MYY 暗影石增加 魔防

	DWORD m18axhp;			   //MYY 尚方宝剑增加 最大生命值
	DWORD p18damage;		   //MYY 尚方宝剑增加 最小攻击力
	DWORD m18axpdamage;		   //MYY 尚方宝剑增加 最大攻击力
	DWORD m18damage;		   //MYY 尚方宝剑增加 最小法术攻击力
	DWORD m18axmdamage;		   //MYY 尚方宝剑增加 最大法术攻击力
	DWORD p18defence;		   //MYY 尚方宝剑增加 物防
	DWORD m18defence;		   //MYY 尚方宝剑增加 魔防

	DWORD m19axhp;			   //MYY 紫金龙袍增加 最大生命值
	DWORD p19damage;		   //MYY 紫金龙袍增加 最小攻击力
	DWORD m19axpdamage;		   //MYY 紫金龙袍增加 最大攻击力
	DWORD m19damage;		   //MYY 紫金龙袍增加 最小法术攻击力
	DWORD m19axmdamage;		   //MYY 紫金龙袍增加 最大法术攻击力
	DWORD p19defence;		   //MYY 紫金龙袍增加 物防
	DWORD m19defence;		   //MYY 紫金龙袍增加 魔防	


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

	DWORD m24axhp;      //MYY 龙凤吟 最大生命值
	DWORD p24damage;      //MYY 龙凤吟 最小攻击力
	DWORD m24axpdamage;      //MYY 龙凤吟 最大攻击力
	DWORD m24damage;     //MYY 龙凤吟 最小法术攻击力
	DWORD m24axmdamage;     //MYY 龙凤吟 最大法术攻击力
	DWORD p24defence;     //MYY 龙凤吟 物防
	DWORD m24defence;     //MYY 龙凤吟 魔防


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


	DWORD m30axhp;      //ZM 预留 最大生命值
	DWORD p30damage;      //ZM 预留 最小攻击力
	DWORD m30axpdamage;      //ZM 预留 最大攻击力
	DWORD m30damage;     //ZM 预留 最小法术攻击力
	DWORD m30axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p30defence;     //ZM 预留 物防
	DWORD m30defence;     //ZM 预留 魔防

	DWORD m31axhp;      //ZM 预留 最大生命值
	DWORD p31damage;      //ZM 预留 最小攻击力
	DWORD m31axpdamage;      //ZM 预留 最大攻击力
	DWORD m31damage;     //ZM 预留 最小法术攻击力
	DWORD m31axmdamage;     //ZM 预留 最大法术攻击力
	DWORD p31defence;     //ZM 预留 物防
	DWORD m31defence;     //ZM 预留 魔防

	DWORD m32axhp;      //ZM 神斧 最大生命值
	DWORD p32damage;      //ZM 神斧 最小攻击力
	DWORD m32axpdamage;      //ZM 神斧 最大攻击力
	DWORD m32damage;     //ZM 神斧 最小法术攻击力
	DWORD m32axmdamage;     //ZM 神斧 最大法术攻击力
	DWORD p32defence;     //ZM 神斧 物防
	DWORD m32defence;     //ZM 神斧 魔防

	DWORD m33axhp;      //ZM 黑曜 最大生命值
	DWORD p33damage;      //ZM 黑曜 最小攻击力
	DWORD m33axpdamage;      //ZM 黑曜 最大攻击力
	DWORD m33damage;     //ZM 黑曜 最小法术攻击力
	DWORD m33axmdamage;     //ZM 黑曜 最大法术攻击力
	DWORD p33defence;     //ZM 黑曜 物防
	DWORD m33defence;     //ZM 黑曜 魔防

	DWORD m34axhp;      //ZM 精金 最大生命值
	DWORD p34damage;      //ZM 精金 最小攻击力
	DWORD m34axpdamage;      //ZM 精金 最大攻击力
	DWORD m34damage;     //ZM 精金 最小法术攻击力
	DWORD m34axmdamage;     //ZM 精金 最大法术攻击力
	DWORD p34defence;     //ZM 精金 物防
	DWORD m34defence;     //ZM 精金 魔防

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
	
	BYTE damagebonus;			// 伤害加成 x% from 道具基本表
	BYTE damage;				// 增加伤害值x％ from 神圣装备表
		
	WORD akspeed;				// 攻击速度
	WORD mvspeed;				// 移动速度
	WORD atrating;				// 命中率
	WORD akdodge;				// 躲避率

	DWORD color;				// 颜色	

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
	
	WORD fivetype;  // 五行属性
	WORD fivepoint; // 五行属性
	
	WORD hpr;  // 生命值恢复
	WORD mpr;  // 法术值恢复
	WORD spr;  // 体力值恢复

	WORD holy;  //神圣一击	
	WORD bang;  //重击
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

	WORD durpoint; //恢复装备耐久度点数
	WORD dursecond; //恢复装备耐久度时间单位

	struct skillbonus {
		WORD id; //技能 id
		WORD point; // 技能点数
	} skill[10]; //技能加成

	struct skillsbonus {
		WORD id; //技能 id
		WORD point; // 技能点数
	} skills;	//全系技能加成

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
	
	BYTE bind;  //装备是否绑定

	union 
	{
		BYTE _five_props[10];
		struct 
		{
			//五行套装相关属性
			BYTE dpdam; //物理伤害减少%x
			BYTE dmdam; //法术伤害减少%x
			BYTE bdam; //增加伤害x%
			BYTE rdam; //伤害反射%x
			BYTE ignoredef; //%x忽视目标防御
			//soke 第二灵魂
			BYTE addpdef;     //物理防御力增强xxx
			BYTE addmdef;     //魔法防御力增强xxx
			BYTE addpdam;     //增加物理攻击力xxxx
			BYTE addmdam;     //增加物理攻击力xxxx
			BYTE addhp;       //增加生命值xxxxx
		};
	};

	WORD fiveset[10]; //五行套装, 按顺序排列

	//...
	BYTE width;  //宽度
	BYTE height; //高度
	WORD dur;    //当前耐久
	WORD maxdur; //最大耐久
	
	DWORD socket[6]; //孔
	DWORD price;     //价格
	DWORD cardpoint; //点卡

	char maker[MAX_NAMESIZE]; //打造者

}t_Object;

#pragma pack()

namespace Zt_Object
{
	static stObjectLocation INVALID_POS;
}

#endif

