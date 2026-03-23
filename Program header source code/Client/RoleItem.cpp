/********************************************************************
创建时间:	27:1:2005   15:23
文件名: 	RoleItem.cpp
文件类型:	cpp
作者:		$$$03公司

用途:	  角色身上的物品类
***********没来**********************************************************/
#include "public.h"
#include "../gui/include/Guitypes.h" 
#include "./GameGuiManager.h"
#include "./GuiChatSelect.h"
#include "./GuiItem.h"
#include "./GuiItemUp.h"
#include "./GuiItemMake.h"
#include "./roleitem.h"
#include "./GameGuiFloatItem.h"
#include "./Game.h"
#include "./GameCursor.h"
#include "./GuiInputBox.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiTrade.h"
#include "./GameState.h"
#include "./GuiNpcTrade.h"
#include "./GuiSalaryTrade.h" //工资
#include "./GuiTicketTrade.h"  //积分
#include "./GuiChongzhiTrade.h" //充值点
#include "./GuiSaveBox.h"
#include "./GuiMessageBox.h"
#include "GuiShopDialog.h"
#include "./RoleSkill.h"
#include "./GuiSkill.h"
#include "./GuiUserInfo.h"
#include "./GuiNpcMake.h"
#include "./GuiMain.h"
#include "./GuiPackage.h"
#include "./GuiShopDialog.h"
#include "./GuiSendMailDialog.h"
#include "./GuiRecvMailDialog.h"
#include "./GuiAuctionDialog.h"
#include "./GuiOthersShopDialog.h"
#include "./GuiMoneyInputBox.h"
#include "./CountryInfo.h"
#include "./Country.h"
#include "./GuiClearPointDlg.h"
#include ".\GuiPetPackDialog.h"
#include ".\GuiPetDialog.h"
#include "./GuiNpcDialog.h"
#include "./Particle/ParticleSysMgr.h"
#include "SafetyUser.h"
#include "./DlgEquipRecast.h"//装备助手
#include "./DlgEquipAnnexpack.h" //附件助手
#include "./GuiWare.h"
#include "./GuiCowryboxdialog.h"
#include "./DlgEquiphandbook.h"   //soke 百兽图鉴
#include "./DlgEquipbookRecast.h" //soke 百兽图鉴助手
#include "./DlgEquipFabaoRecast.h" //soke 法宝助手
#include "./DlgEquipYuanshenRecast.h" //soke 元神助手
#include "./Guijianzhong.h"
#include "./Guizhengtuzhuan.h"
#include "./Guishengxiao.h"
#include "./Guishengqi.h"
#include "./Guijingmai.h"
#include "./DlgMall.h" //新商城
#include "./GuiTouxian.h" //头衔
#include "./GuiBaby.h" //儿女
#include "./GuiHuishou.h" //道具回收
#include "./GuiBieshu.h" //云天别墅
#include "./GuiZhuanhuan.h" //装备转换
#include "./GuiAutoUse.h" //自动使用
#include "./GuiJiazuboss.h" //家族BOSS
#include "./GuiZuoqi.h" //坐骑图鉴
//功勋竞猜
#include "./Guigongxun.h"

const char* c_szItemGrpPack  = "data\\items.gl";		// 物品图包
//soke 物品最高升星级别
const int c_maxItemUpgrade = 201;

tItemThisID	CRoleItem::s_dwMaxThisID = 0;
bool	CRoleItem::s_bCanMoveItem = true;
stPointI g_ptMoneyPos(-1,-1);

struct stCellToItemType {
	WORD  eCellType;
	WORD  eItemType;
};

struct stTableContext {
	WORD  eCellType;
	char  szItemType[32];
	char  szItemAction[8];
};


stCellToItemType  g_CelleToItemType[] = {
	{EQUIPCELLTYPE_HELM,ItemType_Helm},   /// 头盔
	{EQUIPCELLTYPE_BODY,ItemType_ClothBody},   /// 服装
	{EQUIPCELLTYPE_BODY,ItemType_FellBody},   /// 服装
	{EQUIPCELLTYPE_BODY,ItemType_MetalBody},   /// 服装
	{EQUIPCELLTYPE_HANDR,ItemType_Blade},  /// 右手
	{EQUIPCELLTYPE_HANDR,ItemType_Bow},  /// 处理弓箭左手 弓武器
	{EQUIPCELLTYPE_HANDR,ItemType_Sword},  /// 右手
	{EQUIPCELLTYPE_HANDR,ItemType_Axe},  /// 右手
	{EQUIPCELLTYPE_HANDR,ItemType_Staff},  /// 右手
	{EQUIPCELLTYPE_HANDR,ItemType_Hammer},  /// 右手
	{EQUIPCELLTYPE_HANDR,ItemType_Fan},  /// 右手
	{EQUIPCELLTYPE_HANDR,ItemType_Stick},  /// 右手
	{EQUIPCELLTYPE_HANDL,ItemType_Arrow},  /// 处理弓箭右手箭只
	{EQUIPCELLTYPE_HANDR,ItemType_Flower},  /// 右手
	{EQUIPCELLTYPE_HANDL,ItemType_Shield},  /// 左手
	{EQUIPCELLTYPE_NECKLACE,ItemType_Necklace},  /// 项链
	{EQUIPCELLTYPE_GLOVES,ItemType_Cuff},   /// 手套,手镯
	{EQUIPCELLTYPE_RING,ItemType_Fing},    /// 戒指
	{EQUIPCELLTYPE_BELT,ItemType_Caestus},   /// 腰带
	{EQUIPCELLTYPE_SHOES,ItemType_Shoes},  /// 鞋子
	{EQUIPCELLTYPE_PACKAGE,ItemType_Pack},  /// 包裹
	{EQUIPCELLTYPE_MAKE,ItemType_Change},  /// 道具
	{EQUIPCELLTYPE_ADORN,ItemType_Tonic},  /// 自动补血
	{EQUIPCELLTYPE_ADORN,ItemType_DoubleExp},  /// 双倍经验
	{EQUIPCELLTYPE_ADORN,ItemType_Honor},  /// 荣誉之球
	{EQUIPCELLTYPE_FASHION,ItemType_FashionBody},  /// 时装
	{EQUIPCELLTYPE_FASHION,ItemType_HighFashionBody}, ///高级时装
	{EQUIPCELLTYPE_FASHION,ItemType_XHighFashionBody}, ///新高级时装
	{EQUIPCELLTYPE_FASHION,ItemType_SHighFashionBody}, ///新高级时装
	{EQUIPCELLTYPE_ADORN,ItemType_FillMP},  /// 自动补蓝道具
	{EQUIPCELLTYPE_ADORN,ItemType_FillXHP},  ///soke 自动补血道具
	{EQUIPCELLTYPE_ADORN,ItemType_Amulet},  /// 护身符
	{EQUIPCELLTYPE_ADORN,ItemType_Hanbing},  /// 寒冰麒麟
	{EQUIPCELLTYPE_AMULET,ItemType_Amulets},          /// 护身符
	{EQUIPCELLTYPE_HORSESHOE,ItemType_HorseShoe},  /// 马脚
	{EQUIPCELLTYPE_HORSEROPE,ItemType_HorseRope},  /// 马绳
	{EQUIPCELLTYPE_HORSESADDLE,ItemType_HorseSaddle},  /// 马鞍
	{EQUIPCELLTYPE_HORSESAFE,ItemType_HorseSafe},  /// 马甲
	{EQUIPCELLTYPE_HORSEIRON,ItemType_HorseIron},  /// 马镫
	{EQUIPCELLTYPE_HORSEFASHION,ItemType_HorseFashion},  /// 马匹圣石
	{EQUIPCELLTYPE_CBAMULET,ItemType_Chibang},    /// 翅膀
	{EQUIPCELLTYPE_XZAMULET,ItemType_Xunzhang},  /// 勋章
	{EQUIPCELLTYPE_SBAMULET,ItemType_Shenbing0},  /// 神兵1
	{EQUIPCELLTYPE_SBAMULET,ItemType_Shenbing1},  /// 神兵2
	{EQUIPCELLTYPE_SBAMULET,ItemType_Shenbing2},  /// 神兵3
	{EQUIPCELLTYPE_SSAMULET,ItemType_Shenshou1},  /// 神兽1
	{EQUIPCELLTYPE_SSAMULET,ItemType_Shenshou2},  /// 神兽2
	{EQUIPCELLTYPE_PFAMULET,ItemType_pifeng},    /// 披风
	{EQUIPCELLTYPE_HANDR,ItemType_Dagger},       // 右手
	{EQUIPCELLTYPE_HANDR,ItemType_NewSword},    /// 右手
	{EQUIPCELLTYPE_HANDL,ItemType_breastplate}, /// 左手
	{EQUIPCELLTYPE_HANDL,ItemType_StaffFlag},   /// 左手
	{EQUIPCELLTYPE_HANDL,ItemType_CrossFlag},   /// 处理弓箭右手弓战旗
	{EQUIPCELLTYPE_HANDL,ItemType_Flag},        /// 左手
	{EQUIPCELLTYPE_HANDL,ItemType_StickFlag},   /// 左手
	{EQUIPCELLTYPE_HANDL,ItemType_BladeFlag},   /// 左手
	{EQUIPCELLTYPE_HANDL,ItemType_DaggerFlag},  /// 左手
	{EQUIPCELLTYPE_HANDL,ItemType_SwordFlag},   /// 左手
	{EQUIPCELLTYPE_HANDR,ItemType_LongGun},     /// 右手
	{EQUIPCELLTYPE_HANDL,ItemType_Handgun},     /// 左手
	{EQUIPCELLTYPE_SHANGFANGBAOJIAN,ItemType_ShangfangBaojian},     /// 尚方宝剑
	{EQUIPCELLTYPE_ZIJINLONGPAO,ItemType_ZijinLongpao},     /// 紫金龙袍
	{EQUIPCELLTYPE_FABAO,ItemType_Fabao},  /// 法宝
	{EQUIPCELLTYPE_FABAOFJ1,ItemType_Fabaofj1},  /// 法宝
	{EQUIPCELLTYPE_FABAOFJ2,ItemType_Fabaofj2},  /// 法宝
	{EQUIPCELLTYPE_FABAOFJ3,ItemType_Fabaofj3},  /// 法宝
	{EQUIPCELLTYPE_FABAOFJ4,ItemType_Fabaofj4},  /// 法宝
	{EQUIPCELLTYPE_FABAOFJ5,ItemType_Fabaofj5},  /// 法宝
	{EQUIPCELLTYPE_FABAOFJ6,ItemType_Fabaofj6},  /// 法宝
	{EQUIPCELLTYPE_FABAOFJ7,ItemType_Fabaofj7},  /// 法宝
	{EQUIPCELLTYPE_FABAOFJ8,ItemType_Fabaofj8},  /// 法宝
	{EQUIPCELLTYPE_FABAOFJ9,ItemType_Fabaofj9},  /// 法宝
	{EQUIPCELLTYPE_FABAOFJ10,ItemType_Fabaofj10},  /// 法宝
	{EQUIPCELLTYPE_FABAOFJ11,ItemType_Fabaofj11},  /// 法宝
	{EQUIPCELLTYPE_FABAOFJ12,ItemType_Fabaofj12},  /// 法宝
	{EQUIPCELLTYPE_YUANSHEN,ItemType_YUANSHEN},  /// 元神
	{EQUIPCELLTYPE_YUANSHENFJ1,ItemType_YUANSHENFJ1},  /// 元神
	{EQUIPCELLTYPE_YUANSHENFJ2,ItemType_YUANSHENFJ2},  /// 元神
	{EQUIPCELLTYPE_YUANSHENFJ3,ItemType_YUANSHENFJ3},  /// 元神
	{EQUIPCELLTYPE_YUANSHENFJ4,ItemType_YUANSHENFJ4},  /// 元神
	{EQUIPCELLTYPE_YUANSHENFJ5,ItemType_YUANSHENFJ5},  /// 元神
	{EQUIPCELLTYPE_YUANSHENFJ6,ItemType_YUANSHENFJ6},  /// 元神
	{EQUIPCELLTYPE_TUJIAN1,ItemType_handbookitem1},     ///  图鉴
	{EQUIPCELLTYPE_TUJIAN2,ItemType_handbookitem2},     ///  图鉴
	{EQUIPCELLTYPE_TUJIAN3,ItemType_handbookitem3},     ///  图鉴
	{EQUIPCELLTYPE_TUJIAN4,ItemType_handbookitem4},     ///  图鉴
	{EQUIPCELLTYPE_TUJIAN5,ItemType_handbookitem5},     ///  图鉴
	{EQUIPCELLTYPE_TUJIAN6,ItemType_handbookitem6},     ///  图鉴
	{EQUIPCELLTYPE_TUJIAN7,ItemType_handbookitem7},     ///  图鉴
	{EQUIPCELLTYPE_TUJIAN8,ItemType_handbookitem8},     ///  图鉴
	{EQUIPCELLTYPE_TUJIAN9,ItemType_handbookitem9},     ///  图鉴
	{EQUIPCELLTYPE_TUJIAN10,ItemType_handbookitem10},   ///  图鉴1
	{EQUIPCELLTYPE_SEXX1,ItemType_SEXX1},  /// 十二星宿
	{EQUIPCELLTYPE_SEXX2,ItemType_SEXX2},  /// 十二星宿
	{EQUIPCELLTYPE_SEXX3,ItemType_SEXX3},  /// 十二星宿
	{EQUIPCELLTYPE_SEXX4,ItemType_SEXX4},  /// 十二星宿
	{EQUIPCELLTYPE_SEXX5,ItemType_SEXX5},  /// 十二星宿
	{EQUIPCELLTYPE_SEXX6,ItemType_SEXX6},  /// 十二星宿
	{EQUIPCELLTYPE_SEXX7,ItemType_SEXX7},  /// 十二星宿
	{EQUIPCELLTYPE_SEXX8,ItemType_SEXX8},  /// 十二星宿
	{EQUIPCELLTYPE_SEXX9,ItemType_SEXX9},  /// 十二星宿
	{EQUIPCELLTYPE_SEXX10,ItemType_SEXX10},  /// 十二星宿
	{EQUIPCELLTYPE_SEXX11,ItemType_SEXX11},  /// 十二星宿
	{EQUIPCELLTYPE_SEXX12,ItemType_SEXX12},  /// 十二星宿
	{EQUIPCELLTYPE_BABYFJ1,ItemType_BABYFJ1},  /// 宝宝
	{EQUIPCELLTYPE_BABYFJ2,ItemType_BABYFJ2},  /// 宝宝
	{EQUIPCELLTYPE_BABYFJ3,ItemType_BABYFJ3},  /// 宝宝
	{EQUIPCELLTYPE_BABYFJ4,ItemType_BABYFJ4},  /// 宝宝
	{EQUIPCELLTYPE_BABYFJ5,ItemType_BABYFJ5},  /// 宝宝
	{EQUIPCELLTYPE_BABYFJ6,ItemType_BABYFJ6},  /// 宝宝
	
};

// 2010 2
stTableContext  g_TableContext[] = {
	{EQUIPCELLTYPE_HELM,"头部","防御力"},
	{EQUIPCELLTYPE_BODY,"身体","防御力"},
	{EQUIPCELLTYPE_HANDR,"右手","防御力"},
	{EQUIPCELLTYPE_HANDL,"左手","防御力"},
	{EQUIPCELLTYPE_NECKLACE,"颈部","防御力"},
	{EQUIPCELLTYPE_GLOVES,"手腕","防御力"},
	{EQUIPCELLTYPE_RING,"手指","防御力"},
	{EQUIPCELLTYPE_BELT,"腰部","防御力"},
	{EQUIPCELLTYPE_SHOES,"脚","防御力"},
	{EQUIPCELLTYPE_PACKAGE,"额外包裹",""},
	{EQUIPCELLTYPE_MAKE,"道具",""},
	{EQUIPCELLTYPE_ADORN,"饰品","防御力"},
	{EQUIPCELLTYPE_FASHION,"时装",""},
	{EQUIPCELLTYPE_AMULET,"护身符",""},
	{EQUIPCELLTYPE_HORSESHOE,"马脚",""},
	{EQUIPCELLTYPE_HORSEROPE,"马绳",""},
	{EQUIPCELLTYPE_HORSESADDLE,"马鞍",""},
	{EQUIPCELLTYPE_HORSESAFE,"马甲",""},
	{EQUIPCELLTYPE_HORSEIRON,"马镫",""},
	{EQUIPCELLTYPE_HORSEFASHION,"马牌",""},
	{EQUIPCELLTYPE_CBAMULET,"翅膀",""},
	{EQUIPCELLTYPE_XZAMULET,"勋章",""},
	{EQUIPCELLTYPE_SBAMULET,"神兵",""},
	{EQUIPCELLTYPE_SSAMULET,"神兽",""},
	{EQUIPCELLTYPE_SHANGFANGBAOJIAN,"尚方宝剑",""},
	{EQUIPCELLTYPE_ZIJINLONGPAO,"紫金龙袍",""},
	{EQUIPCELLTYPE_PFAMULET,"披风",""},
	{EQUIPCELLTYPE_FABAO,"法宝",""},
	{EQUIPCELLTYPE_FABAOFJ1,"法宝符石",""},
	{EQUIPCELLTYPE_FABAOFJ2,"法宝符石",""},
	{EQUIPCELLTYPE_FABAOFJ3,"法宝符石",""},
	{EQUIPCELLTYPE_FABAOFJ4,"法宝符石",""},
	{EQUIPCELLTYPE_FABAOFJ5,"法宝符石",""},
	{EQUIPCELLTYPE_FABAOFJ6,"法宝符石",""},
	{EQUIPCELLTYPE_FABAOFJ7,"法宝灵石",""},
	{EQUIPCELLTYPE_FABAOFJ8,"法宝灵石",""},
	{EQUIPCELLTYPE_FABAOFJ9,"法宝灵石",""},
	{EQUIPCELLTYPE_FABAOFJ10,"法宝灵石",""},
	{EQUIPCELLTYPE_FABAOFJ11,"法宝灵石",""},
	{EQUIPCELLTYPE_FABAOFJ12,"法宝灵石",""},
	{EQUIPCELLTYPE_YUANSHEN,"元神",""},
	{EQUIPCELLTYPE_YUANSHENFJ1,"元神装备",""},
	{EQUIPCELLTYPE_YUANSHENFJ2,"元神装备",""},
	{EQUIPCELLTYPE_YUANSHENFJ3,"元神装备",""},
	{EQUIPCELLTYPE_YUANSHENFJ4,"元神装备",""},
	{EQUIPCELLTYPE_YUANSHENFJ5,"元神装备",""},
	{EQUIPCELLTYPE_YUANSHENFJ6,"元神装备",""},
	{EQUIPCELLTYPE_SEXX1,"十二星宿·白羊座",""},
	{EQUIPCELLTYPE_SEXX2,"十二星宿·金牛座",""},
	{EQUIPCELLTYPE_SEXX3,"十二星宿·双子座",""},
	{EQUIPCELLTYPE_SEXX4,"十二星宿·巨蟹座",""},
	{EQUIPCELLTYPE_SEXX5,"十二星宿·狮子座",""},
	{EQUIPCELLTYPE_SEXX6,"十二星宿·处女座",""},
	{EQUIPCELLTYPE_SEXX7,"十二星宿·天秤座",""},
	{EQUIPCELLTYPE_SEXX8,"十二星宿·天蝎座",""},
	{EQUIPCELLTYPE_SEXX9,"十二星宿·射手座",""},
	{EQUIPCELLTYPE_SEXX10,"十二星宿·摩羯座",""},
	{EQUIPCELLTYPE_SEXX11,"十二星宿·水瓶座",""},
	{EQUIPCELLTYPE_SEXX12,"十二星宿·双鱼座",""},
	{EQUIPCELLTYPE_BABYFJ1,"宝宝装备",""},
	{EQUIPCELLTYPE_BABYFJ2,"宝宝装备",""},
	{EQUIPCELLTYPE_BABYFJ3,"宝宝装备",""},
	{EQUIPCELLTYPE_BABYFJ4,"宝宝装备",""},
	{EQUIPCELLTYPE_BABYFJ5,"宝宝装备",""},
	{EQUIPCELLTYPE_BABYFJ6,"宝宝装备",""},
	{EQUIPCELLTYPE_TUJIAN1,"山寨霸王",""},
	{EQUIPCELLTYPE_TUJIAN2,"至尊蚌魔",""},
	{EQUIPCELLTYPE_TUJIAN3,"魔徒",""},
	{EQUIPCELLTYPE_TUJIAN4,"恶魔统领",""},
	{EQUIPCELLTYPE_TUJIAN5,"铁爪鬼王",""},
	{EQUIPCELLTYPE_TUJIAN6,"恶魔尊主",""},
	{EQUIPCELLTYPE_TUJIAN7,"嗜血屠魔",""},
	{EQUIPCELLTYPE_TUJIAN8,"僵尸小将",""},
	{EQUIPCELLTYPE_TUJIAN9,"刑天",""},
	{EQUIPCELLTYPE_TUJIAN10,"南山老怪",""},	
};



#define NEED_ADD_REPAIR_VALUE	4

const char* ItemKind2String(BYTE kind)
{
	//物品类型, 0普通, 1蓝色, 2金色, 4神圣, 8套装, 16紫装
	switch( kind )
	{
	case 0: return "普通";
	case 1: return "蓝色";
	case 2: return "金色";
	case 4: return "神圣";
	case 8: return "套装";
	case 16: return "紫装";
	default:break;
	}
	return "";
}

bool IsHandsArm(byte type)
{
	switch(type) {
case ItemType_Axe:
case ItemType_Hammer:
case ItemType_Staff:
case ItemType_Stick:
	{
		return true;
	}
	}
	return false;
}


bool IsTonic()
{
	if (GetGameGuiManager()->m_guiUserInfo)
	{
		return GetGameGuiManager()->m_guiUserInfo->IsTonic();
	}
	return false;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param eCellType : 描述
* \param eItemType : 描述
* \return 返回值的描述
*/
bool IsCellItemType(WORD eCellType,WORD eItemType)
{
	FUNCTION_BEGIN;

	static int count = sizeof(g_CelleToItemType) / sizeof(stCellToItemType);

	for (int i=0;i<count;i++)
	{
		if ((g_CelleToItemType[i].eCellType==eCellType)&&(g_CelleToItemType[i].eItemType==eItemType))
		{
			return true;
		}
	}

	return false;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param eItemType : 描述
* \return 返回值的描述
*/
WORD IsEquipItem(WORD eItemType)
{
	FUNCTION_BEGIN;

	static int count = sizeof(g_CelleToItemType) / sizeof(stCellToItemType);

	for (int i=0;i<count;i++)
	{
		if (g_CelleToItemType[i].eItemType==eItemType)
		{
			return g_CelleToItemType[i].eCellType;
		}
	}

	return 0;

	FUNCTION_END;
}

#define ISPROPERTY(value)  if (value!=0) nNum++

WORD GetObjectPropertyNum(t_Object*  m_pObject)
{
	WORD nNum = 0;

	ISPROPERTY(m_pObject->fivepoint);
	ISPROPERTY(m_pObject->maxmp);
	ISPROPERTY(m_pObject->mvspeed);
	ISPROPERTY(m_pObject->bang);
	ISPROPERTY(m_pObject->hpr);
	ISPROPERTY(m_pObject->mpr);
	ISPROPERTY(m_pObject->spr);
	ISPROPERTY(m_pObject->akspeed);
	ISPROPERTY(m_pObject->pdam);
	ISPROPERTY(m_pObject->pdef);

	ISPROPERTY(m_pObject->mdam);
	ISPROPERTY(m_pObject->mdef);
	ISPROPERTY(m_pObject->atrating);
	ISPROPERTY(m_pObject->akdodge);
	ISPROPERTY(m_pObject->poisondef);
	ISPROPERTY(m_pObject->lulldef);
	ISPROPERTY(m_pObject->reeldef);
	ISPROPERTY(m_pObject->evildef);
	ISPROPERTY(m_pObject->bitedef);
	ISPROPERTY(m_pObject->chaosdef);

	ISPROPERTY(m_pObject->colddef);
	ISPROPERTY(m_pObject->petrifydef);
	ISPROPERTY(m_pObject->blinddef);
	ISPROPERTY(m_pObject->stabledef);
	ISPROPERTY(m_pObject->slowdef);
	ISPROPERTY(m_pObject->luredef);
	ISPROPERTY(m_pObject->skill[0].point);
	ISPROPERTY(m_pObject->skills.point);
	ISPROPERTY(m_pObject->holy);
	ISPROPERTY(m_pObject->hpleech.odds);

	ISPROPERTY(m_pObject->mpleech.odds);
	ISPROPERTY(m_pObject->hptomp);
	ISPROPERTY(m_pObject->dhpp);
	ISPROPERTY(m_pObject->dmpp);
	ISPROPERTY(m_pObject->incgold);
	ISPROPERTY(m_pObject->doublexp);
	ISPROPERTY(m_pObject->mf);
	ISPROPERTY(m_pObject->dpdam);     //物理伤害减少
	ISPROPERTY(m_pObject->dmdam);     //法术伤害减少
	ISPROPERTY(m_pObject->bdam);      //增加伤害 x%
	ISPROPERTY(m_pObject->rdam);      //伤害反射 %x
	ISPROPERTY(m_pObject->ignoredef); //%x 忽视目标防御
	ISPROPERTY(m_pObject->addpdef);   //物理防御力增强
	ISPROPERTY(m_pObject->addmdef);   //魔法防御力增强
	ISPROPERTY(m_pObject->addpdam);   //增加物理攻击力
	ISPROPERTY(m_pObject->addmdam);   //增加魔法攻击力
	ISPROPERTY(m_pObject->addhp);     //增加生命值
	ISPROPERTY(m_pObject->poison);
	ISPROPERTY(m_pObject->lull);
	ISPROPERTY(m_pObject->reel);
	ISPROPERTY(m_pObject->evil);
	ISPROPERTY(m_pObject->bite);
	ISPROPERTY(m_pObject->chaos);
	ISPROPERTY(m_pObject->cold);
	ISPROPERTY(m_pObject->petrify);

	ISPROPERTY(m_pObject->blind);
	ISPROPERTY(m_pObject->stable);
	ISPROPERTY(m_pObject->slow);
	ISPROPERTY(m_pObject->lure);

	return nNum;
}

float GetKindOdds(byte kind)
{
	float Odds = 0.0f;

	if (kind&0x4)
	{
		Odds = 2.0f;
	}
	else if (kind&0x2)
	{
		Odds = 1.5f;
	}
	else if (kind&0x1)
	{
		Odds = 1.2f;
	}
	else
		Odds = 1.0f;

	return Odds;
}

float GetObjectSellMoney(t_Object*  m_pObject)
{
	float nMoney = 0.0f;
	if (!m_pObject)
		return nMoney;

	ObjectBase_t* pObjectBase = GetObjectBase(m_pObject->dwObjectID);
	if (!pObjectBase)
		return nMoney;


	nMoney = 0.25*pObjectBase->price*(1+GetObjectPropertyNum(m_pObject)*0.1f)*GetKindOdds(m_pObject->kind);

	if (pObjectBase->price>0)
		nMoney = nMoney + 1.0f;

	return nMoney;
}


float GetDurRate(t_Object*  m_pObject)
{
	DWORD maxdur = m_pObject->maxdur;
	ObjectBase_t* pObjectBase = GetObjectBase(m_pObject->dwObjectID);
	if (pObjectBase!=NULL)
	{
		maxdur = pObjectBase->dwMaxDur;
	}
	return (float)((m_pObject->dur+49)/50)/((maxdur+49)/50);
}

//soke 物品出售的价格
DWORD GetObjectSellMoneyByDur(t_Object*  m_pObject)
{
	DWORD nMoney = 0;

	float fSell = GetObjectSellMoney(m_pObject);

	ObjectBase_t* pObjectBase = GetObjectBase(m_pObject->dwObjectID);
	if (pObjectBase!=NULL)
	{
		if (pObjectBase->dwID == 655)
		{
			fSell = 4000.0f;
		}
		else if ( pObjectBase->dwID == 881 )//龙之生命--
		{
			fSell = 20000.0f;
		}
		else if ( pObjectBase->dwID == 860 )//天之生命--
		{
			fSell = 30000.0f;
		}
		else if ( pObjectBase->dwID == 929 )//神之生命--
		{
			fSell = 40000.0f;
		}
		else if ( pObjectBase->dwID == 685 )//魔力之源 
		{
			fSell = 4000.0f;
		}
		else if ( pObjectBase->dwID == 760 )//洗髓宝珠 
		{
			fSell = 200000.0f;
		}
		else if ( pObjectBase->dwID == 761 )//易筋宝珠 
		{
			fSell = 200000.0f;
		}
		else if ( pObjectBase->dwID == 882 )//龙之魔力--- 
		{
			fSell = 20000.0f;
		}
		else if ( pObjectBase->dwID == 861 )//天之魔力---
		{
			fSell = 30000.0f;
		}
		else if ( pObjectBase->dwID == 930 )//神之魔力---
		{
			fSell = 40000.0f;
		}
		else if ( pObjectBase->dwID == 942 )//天之活力
		{
			fSell = 20000.0f;
		}
		else if ( pObjectBase->dwID == 952 )//龙之活力
		{
			fSell = 30000.0f;
		}
		else if ( pObjectBase->dwID == 953 )//神之活力
		{
			fSell = 40000.0f;
		}
	}

	nMoney = fSell*((m_pObject->maxdur==0)?1:GetDurRate(m_pObject));

	return nMoney;
}



float GetRepairDurRate(t_Object*  m_pObject)
{
	DWORD maxdur = m_pObject->maxdur;
	ObjectBase_t* pObjectBase = GetObjectBase(m_pObject->dwObjectID);
	if (pObjectBase!=NULL)
	{
		maxdur = pObjectBase->dwMaxDur;
	}
	return (float)((m_pObject->maxdur-m_pObject->dur+49)/50)/((maxdur+49)/50);
}


/**
* \brief 简短描述
* 
* 详细描述
* 
* \param m_pObject : 描述
* \return 返回值的描述
*/
DWORD GetObjectRepairMoney(t_Object*  m_pObject)
{
	FUNCTION_BEGIN;

	DWORD nMoney = 0;
	if (!m_pObject)
		return nMoney;

	ObjectBase_t* pObjectBase = GetObjectBase(m_pObject->dwObjectID);
	if (!pObjectBase)
		return nMoney;

	if (pObjectBase->dwUp==0)
		return nMoney;


	nMoney = 2*GetObjectSellMoney(m_pObject)*GetRepairDurRate(m_pObject);

	return nMoney;

	FUNCTION_END;
}


DWORD RepairMoney2RepairGold(DWORD dwObjectRepair)
{
	return (DWORD)((float)(dwObjectRepair/50)+0.99);
}


DWORD GetObjectRepairByGold(t_Object*  m_pObject)
{
	return RepairMoney2RepairGold(GetObjectRepairMoney(m_pObject));
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param dwMoney : 描述
* \return 返回值的描述
*/
//soke 金钱换算
const char* Money2String(DWORD dwMoney)
{
	FUNCTION_BEGIN;

	static char szText[MAX_PATH]={0};
	char szMoney[MAX_PATH]={0};
	char szTmp[MAX_NAMESIZE]={0};
	const int nIngotRate = INGOT_RATE;
	const int nLiangRate = TAEL_RATE;
	int nIngot = (int)(dwMoney / nIngotRate);
	int nLiang = (int)((dwMoney % nIngotRate) / nLiangRate);
	int nCent = (int)((dwMoney % nIngotRate) % nLiangRate);

	if (nIngot>0)
	{
		sprintf(szTmp,"%d%s",nIngot,INGOT_TXT);
		strncat(szMoney,szTmp,sizeof(szTmp));
	}

	if (nLiang>0)
	{
		sprintf(szTmp,"%d%s",nLiang,TAEL_TXT);
		strncat(szMoney,szTmp,sizeof(szTmp));
	}

	if ((nCent>0)||(strlen(szMoney)==0))
	{
		sprintf(szTmp,"%d%s",nCent,CENT_TXT);
		strncat(szMoney,szTmp,sizeof(szTmp));
	}

	strncpy(szText,szMoney,MAX_PATH);
	return szText;

	FUNCTION_END;
}

///////////////////////////star100511
int Money2String_ReturnOneNumChr(DWORD dwMoney, WORD wType)
{
	FUNCTION_BEGIN;
	/*
	static char szText[MAX_PATH]={0};
	char szMoney[MAX_PATH]={0};
	const int nIngotRate = INGOT_RATE;
	const int nLiangRate = TAEL_RATE;

	char szTmp  =0;
	int nIngot = (int)(dwMoney / nIngotRate);
	int nLiang = (int)((dwMoney % nIngotRate) / nLiangRate);
	int nCent = (int)((dwMoney % nIngotRate) % nLiangRate);

	if (nIngot>0 && wType == 1)
	{
	return nIngot;
	}
	if (nLiang>0 && wType ==2 )
	{
	return nLiang;
	}
	if ((nCent>0)||(strlen(szMoney)==0))
	{
	return nCent;
	}

	return 0;
	*/
	int MoneyNum = 0;
	switch(wType)
	{
	case 1:
		{
			MoneyNum = (int)(dwMoney / INGOT_RATE);
			break;
		}
	case 2:
		{
			MoneyNum = (int)((dwMoney % INGOT_RATE) / TAEL_RATE);
			break;
		}
	case 3:
		{
			MoneyNum = (int)((dwMoney % INGOT_RATE) % TAEL_RATE);
			break;
		}
	default : break;
	}
	return MoneyNum;
	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param dwMoney : 描述
* \return 返回值的描述
*/
//soke 积分换算
const char* Ticket2String(DWORD dwtikcet)
{
	FUNCTION_BEGIN;

	static char szText[MAX_PATH]={0};
	char szTicket[MAX_PATH]={0};
	char szTmp[MAX_NAMESIZE]={0};
	const int nIngotRate = TICKET_RATE;
	int nIngot = (int)(dwtikcet / nIngotRate);

	if (nIngot>0 ||(strlen(szTicket)==0)) //修复0积分显示不正常情况
	{
		sprintf(szTmp,"%d%s",nIngot,TICKET_TXT);
		strncat(szTicket,szTmp,sizeof(szTmp));
	}

	strncpy(szText,szTicket,MAX_PATH);
	return szText;

	FUNCTION_END;
}

///////////////////////////star100511
int Ticket2String_ReturnOneNumChr(DWORD dwTicket, WORD wType)
{
	FUNCTION_BEGIN;
	
	int TicketNum = 0;
	switch(wType)
	{
	case 1:
		{
			TicketNum = (int)(dwTicket / TICKET_RATE);
			break;
		}
	default : break;
	}
	return TicketNum;
	FUNCTION_END;
}
///////////////////////////end
/**
* \brief 简短描述
* 
* 详细描述
* 
* \param dwMoney : 描述
* \return 返回值的描述
*/
//soke 充值点换算
const char* Chongzhi2String(DWORD dwchongzhi)
{
	FUNCTION_BEGIN;

	static char szText[MAX_PATH]={0};
	char szChongzhi[MAX_PATH]={0};
	char szTmp[MAX_NAMESIZE]={0};
	const int nIngotRate = CHONGZHI_RATE;
	int nIngot = (int)(dwchongzhi / nIngotRate);

	if (nIngot>0 ||(strlen(szChongzhi)==0)) //修复0充值点显示不正常情况
	{
		sprintf(szTmp,"%d%s",nIngot,CHONGZHI_TXT);
		strncat(szChongzhi,szTmp,sizeof(szTmp));
	}

	strncpy(szText,szChongzhi,MAX_PATH);
	return szText;

	FUNCTION_END;
}

///////////////////////////star100511
int Chongzhi2String_ReturnOneNumChr(DWORD dwChongzhi, WORD wType)
{
	FUNCTION_BEGIN;
	
	int ChongzhiNum = 0;
	switch(wType)
	{
	case 1:
		{
			ChongzhiNum = (int)(dwChongzhi / CHONGZHI_RATE);
			break;
		}
	default : break;
	}
	return ChongzhiNum;
	FUNCTION_END;
}
///////////////////////////end

float GetItemPrice(DWORD price)
{
	float buyPrice = price;

	DWORD goodness = GetScene()->GetMainCharacter()->GetGoodnessAfterAdjust();
	switch(goodness)
	{
	case GOODNESS_3:
		buyPrice *= 1.2f;
		break;
	case GOODNESS_4:
		buyPrice *= 1.5f;
		break;
	case GOODNESS_5:
	case GOODNESS_6:
		buyPrice *= 2.0f;
		break;
	}

	//Consider middle country
	if ( GetScene()->IsCurMapIsMiddleCountry() )
	{
		buyPrice *= 1.1f;
	}

	return (buyPrice + 0.99f);
}

const char* Minute2DayHourMinute(DWORD dwMinute, enmDayHourMinuteType type)
{
	FUNCTION_BEGIN;

	static char szText[MAX_PATH] = {0};
	char szMinute[MAX_PATH] = {0};
	char szTmp[MAX_NAMESIZE] = {0};
	const int nDayRate = 24 * 60;
	const int nHourRate = 60;

	int nDay = (int)( dwMinute / nDayRate );
	int nHour = (int)( (dwMinute % nDayRate) / nHourRate );
	int nMinute = (int)( (dwMinute % nDayRate) % nHourRate );

	if ( type >= TYPE_DAY )
	{
		if ( nDay > 0 )
		{
			sprintf( szTmp, "%d%s", nDay, "天" );
			strncat( szMinute, szTmp, sizeof(szTmp) );
		}
		if ( type >= TYPE_DAY_HOUR )
		{
			if ( nHour > 0 )
			{
				sprintf( szTmp, "%d%s", nHour, "时" );
				strncat( szMinute, szTmp, sizeof(szTmp) );
			}

			if ( type >= TYPE_DAY_HOUR_MINUTE )
			{
				if ( (nMinute > 0) || (strlen(szMinute) == 0) )
				{
					sprintf( szTmp, "%d%s", nMinute, "分" );
					strncat( szMinute, szTmp, sizeof(szTmp) ); 
				}
			}			
		}		
	}	

	strncpy( szText, szMinute, MAX_PATH );

	return szText;

	FUNCTION_END;
}

//soke 新增时间显示
const char* Minute3DayHourMinute(DWORD dwMinute, enmDayHourMinuteType type)
{
	FUNCTION_BEGIN;

	static char szText[MAX_PATH] = {0};
	char szMinute[MAX_PATH] = {0};
	char szTmp[MAX_NAMESIZE] = {0};
	const int nDayRate = 24 * 60;
	const int nHourRate = 60;

	int nDay = (int)( dwMinute / nDayRate );
	int nHour = (int)( (dwMinute % nDayRate) / nHourRate );
	int nMinute = (int)( (dwMinute % nDayRate) % nHourRate );

	if ( type >= TYPE_DAY )
	{
		if ( nDay > 0 )
		{
			sprintf( szTmp, "%d%s", nDay, "天" );
			strncat( szMinute, szTmp, sizeof(szTmp) );
		}
		if ( type >= TYPE_DAY_HOUR )
		{
			if ( nHour > 0 )
			{
				sprintf( szTmp, "%d%s", nHour, "小时" );
				strncat( szMinute, szTmp, sizeof(szTmp) );
			}

			if ( type >= TYPE_DAY_HOUR_MINUTE )
			{
				if ( (nMinute > 0) || (strlen(szMinute) == 0) )
				{
					sprintf( szTmp, "%d%s", nMinute, "分钟" );
					strncat( szMinute, szTmp, sizeof(szTmp) ); 
				}
			}			
		}		
	}	

	strncpy( szText, szMinute, MAX_PATH );

	return szText;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param strMoney : 描述
* \return 返回值的描述
*/
DWORD String2Money(const char* strMoney)
{
	FUNCTION_BEGIN;

	std::string str = strMoney;

	int nIngotPos = str.find(INGOT_TXT);
	int nTaelPos = str.find(TAEL_TXT);
	int nCentPos = str.find(CENT_TXT);

	int nIngotMoney = 0;
	int nTaelMoney = 0;
	int nCentMoney = 0;

	if (nIngotPos!=-1)
		nIngotMoney = atoi(str.substr(0,nIngotPos).c_str());

	if (nTaelPos!=-1)
	{
		if (nIngotPos!=-1)
			nTaelMoney = atoi(str.substr(nIngotPos+strlen(INGOT_TXT),nTaelPos-nIngotPos).c_str());
		else
			nTaelMoney = atoi(str.substr(0,nTaelPos).c_str());
	}

	if (nCentPos!=-1)
	{
		if (nTaelPos!=-1)
			nCentMoney = atoi(str.substr(nTaelPos+strlen(TAEL_TXT),nCentPos-nTaelPos).c_str());
		else if (nIngotPos!=-1)
			nCentMoney = atoi(str.substr(nIngotPos+strlen(TAEL_TXT),nCentPos-nIngotPos).c_str());
		else
			nCentMoney = atoi(str.substr(0,nTaelPos).c_str());
	}

	return (nIngotMoney*INGOT_RATE + nTaelMoney*TAEL_RATE + nCentMoney);

	FUNCTION_END;
}


/**
* \brief 简短描述
* 
* 详细描述
* 
* \param eCellType : 描述
* \return 返回值的描述
*/
char* GetTableContext(WORD eCellType)
{
	FUNCTION_BEGIN;

	static int count = sizeof(g_TableContext) / sizeof(stTableContext);
	static char szContext[64] = {0};

	for (int i=0;i<count;i++)
	{
		if (g_TableContext[i].eCellType==eCellType)
		{
			sprintf(szContext," %s",g_TableContext[i].szItemType);
			return szContext;
		}
	}

	return "";

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pObjectBase : 描述
* \return 返回值的描述
*/
stResourceLocation GetObjectBasePicLocation(ObjectBase_t* pObjectBase)
{
	FUNCTION_BEGIN;

	Assert(pObjectBase);
	stResourceLocation rl;
	if(!pObjectBase) return rl;
	rl.SetFileName(c_szItemGrpPack);

	rl.group = c_nGroupItemEquip;
	rl.frame = pObjectBase->dwEquipPic;
	//Engine_WriteLogF("%s\n",rl.szFileName);
	//sky 寒冰手拿武器的显示
	if(GetScene()->GetMainCharacter()->GetProperty()->hanbing == 1)
	{
		if(pObjectBase->dwType == 155) //sky 枪
		{
			rl.frame = 1165;
		}
		else if(pObjectBase->dwType == 137) //sky 剑
		{
			rl.frame = 1164;
		}
		else if(pObjectBase->dwType == 136) //sky 利刃
		{
			rl.frame = 1171;
		}
		else if(pObjectBase->dwType == 111) //sky 棍
		{
			rl.frame = 1168;
		}
		else if(pObjectBase->dwType == 110) //sky 扇
		{
			rl.frame = 1166;
		}
		else if(pObjectBase->dwType == 109) //sky 弓
		{
			rl.frame = 1169;
		}
		else if(pObjectBase->dwType == 108) //sky 杖
		{
			rl.frame = 1167;
		}
		else if(pObjectBase->dwType == 106 || pObjectBase->dwType == 106) //sky 斧
		{
			rl.frame = 1163;
		}
		else if(pObjectBase->dwType == 105) //sky 剑
		{
			rl.frame = 1164;
		}
		else if(pObjectBase->dwType == 104) //sky 刀
		{
			rl.frame = 1162;
		}
	}
	else
		rl.frame = pObjectBase->dwEquipPic;

	return rl;

	FUNCTION_END;
}



/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pObjectBase : 描述
* \return 返回值的描述
*/
stResourceLocation GetObjectBasePicLocation2(ObjectBase_t* pObjectBase)
{
	FUNCTION_BEGIN;

	Assert(pObjectBase);
	stResourceLocation rl;
	if(!pObjectBase) return rl;
	rl.SetFileName(c_szItemGrpPack);

	rl.group = c_nGroupItemEquip;
	
	rl.frame = pObjectBase->dwEquipPic;

	return rl;

	FUNCTION_END;
}
/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pObjectBase : 描述
* \return 返回值的描述
*/
stResourceLocation GetObjectBasePatternPicLocation(ObjectBase_t* pObjectBase)
{
	FUNCTION_BEGIN;

	Assert(pObjectBase);
	stResourceLocation rl;
	if(!pObjectBase) return rl;
	rl.SetFileName(c_szItemGrpPack);

	rl.group = c_nGroupItemEquipPattern;
	rl.frame = pObjectBase->dwEquipPic;

	return rl;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pObjectBase : 描述
* \return 返回值的描述
*/
stResourceLocation GetObjectBasePattern2PicLocation(ObjectBase_t* pObjectBase)
{
	FUNCTION_BEGIN;

	Assert(pObjectBase);
	stResourceLocation rl;
	if(!pObjectBase) return rl;
	rl.SetFileName(c_szItemGrpPack);

	rl.group = c_nGroupItemEquipPattern2;
	rl.frame = pObjectBase->dwEquipPic;

	return rl;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pObject : 描述
* \return 返回值的描述
*/
stResourceLocation GetItemPicLocation(t_Object * pObject)
{
	FUNCTION_BEGIN;

	ObjectBase_t* pObjectBase = GetObjectBase(pObject->dwObjectID);
	Assert(pObjectBase);
	stResourceLocation rl;
	if(!pObjectBase) return rl;
	rl.SetFileName(c_szItemGrpPack);

	//sky 装备栏上武器的图片
	rl.group = c_nGroupItemCommon;
	rl.frame = pObjectBase->dwItemPic;
 
	//sky 寒冰装备的显示
	if(GetScene()->GetMainCharacter()->GetProperty()->hanbing == 1)
	{
		if(pObjectBase->dwType == 155) //sky 枪
		{
			rl.frame = 1490;
		}
		else if(pObjectBase->dwType == 137) //sky 剑
		{
			rl.frame = 1481;
		}
		else if(pObjectBase->dwType == 136) //sky 利刃
		{
			rl.frame = 1495;
		}
		else if(pObjectBase->dwType == 111) //sky 棍
		{
			rl.frame = 1492;
		}
		else if(pObjectBase->dwType == 110) //sky 扇
		{
			rl.frame = 1491;
		}
		else if(pObjectBase->dwType == 109) //sky 弓
		{
			rl.frame = 1494;
		}
		else if(pObjectBase->dwType == 108) //sky 杖
		{
			rl.frame = 1493;
		}
		else if(pObjectBase->dwType == 106 || pObjectBase->dwType == 106) //sky 斧
		{
			rl.frame = 1488;
		}
		else if(pObjectBase->dwType == 105) //sky 剑
		{
			rl.frame = 1489;
		}
		else if(pObjectBase->dwType == 104) //sky 刀
		{
			rl.frame = 1487;
		}
	}
	else
		rl.frame = pObjectBase->dwItemPic;

	
	return rl;

	FUNCTION_END;
}

stResourceLocation GetObjectImageLocation(ObjectBase_t * pObject)
{
	return stResourceLocation(c_szItemGrpPack,c_nGroupItemCommon,pObject->dwItemPic);
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pObject : 描述
* \return 返回值的描述
*/
stResourceLocation GetItemPatternPicLocation(t_Object * pObject)
{
	FUNCTION_BEGIN;

	ObjectBase_t* pObjectBase = GetObjectBase(pObject->dwObjectID);
	Assert(pObjectBase);
	stResourceLocation rl;
	if(!pObjectBase) return rl;
	rl.SetFileName(c_szItemGrpPack);
	/*if(pObject->pos.dwLocation == OBJECTCELLTYPE_EQUIPSHOW && pObjectBase->dwEquipPic != -1)
	{
	rl.group = c_nGroupItemEquipPattern;
	rl.frame = pObjectBase->dwEquipPic;
	}
	else*/
	{
		rl.group = c_nGroupItemCommonPattern;
		rl.frame = pObjectBase->dwItemPic;
	}
	return rl;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pObject : 描述
* \return 返回值的描述
*/
stResourceLocation GetItemPattern2PicLocation(t_Object * pObject)
{
	FUNCTION_BEGIN;

	ObjectBase_t* pObjectBase = GetObjectBase(pObject->dwObjectID);
	Assert(pObjectBase);
	stResourceLocation rl;
	if(!pObjectBase) return rl;
	rl.SetFileName(c_szItemGrpPack);
	/*if(pObject->pos.dwLocation == OBJECTCELLTYPE_EQUIPSHOW && pObjectBase->dwEquipPic != -1)
	{
	rl.group = c_nGroupItemEquipPattern2;
	rl.frame = pObjectBase->dwEquipPic;
	}
	else*/
	{
		rl.group = c_nGroupItemCommonPattern2;
		rl.frame = pObjectBase->dwItemPic;
	}
	return rl;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pos : 描述
* \return 返回值的描述
*/
CGuiTable* GetItemTable(const stObjectLocation & pos)
{
	FUNCTION_BEGIN;

	//TRACE("CGuiTable* GetItemTable:pos:Location->%d\tID:%d\tX:%d\tY:%d\t\n",pos.dwLocation,pos.dwTableID,pos.x,pos.y);
	switch(pos.dwLocation)
	{
	case OBJECTCELLTYPE_RECAST:
		{
			if (GetGameGuiManager()->m_pDlgEquipRecast)
			{
				switch(pos.y) 
				{
				case MAKECELLTYPE_EQUIP:
					return GetGameGuiManager()->m_pDlgEquipRecast->m_pTableFocusItem;
				case MAKECELLTYPE_MATERIAL1:
					return GetGameGuiManager()->m_pDlgEquipRecast->m_pTableMaterial1;
			//	case MAKECELLTYPE_MATERIAL2:
			//		return GetGameGuiManager()->m_pDlgEquipRecast->m_pTableMaterial2;
			//	case MAKECELLTYPE_MATERIAL3:
			//		return GetGameGuiManager()->m_pDlgEquipRecast->m_pTableMaterial3;
			//	case MAKECELLTYPE_MATERIAL4:
			//		return GetGameGuiManager()->m_pDlgEquipRecast->m_pTableMaterial4;
				}
			}
		}
		break;
	case OBJECTCELLTYPE_FUJIAN:
		{
			if(pos.y == 1)
			{
				return GetGameGuiManager()->m_guijianzhong->m_pTableMaterial1;
			}
			if(pos.y == 5)
			{
				return GetGameGuiManager()->m_guizhengtuzhuan->m_pTableMaterial1;
			}
			if(pos.y == 3)
			{
				return GetGameGuiManager()->m_guishengxiao->m_pTableMaterial1;
			}
			if(pos.y == 4)
			{
				return GetGameGuiManager()->m_guishengqi->m_pTableMaterial1;
			}
			if(pos.y == 2)
			{
				return GetGameGuiManager()->m_guijingmai->m_pTableMaterial1;
			}
			if(pos.y == 6)
			{
				//头衔
				return GetGameGuiManager()->m_guiTouxian->m_pTableMaterial1;
			}
			if(pos.y == 7)
			{
				//儿女
				return GetGameGuiManager()->m_guiBaby->m_pTableMaterial1;
			}
			//道具回收
			if(pos.y == 10)
			{
				return GetGameGuiManager()->m_guiHuishou->m_pTableMaterial1;
			}
			//云天别墅
			if(pos.y == 11)
			{
				return GetGameGuiManager()->m_guiBieShu->m_pTableMaterial1;
			}
			//装备转换
			if(pos.y == 12)
			{
				return GetGameGuiManager()->m_guiZhuanhuan->m_pTableMaterial1;
			}
			//自动使用
			if(pos.y == 13)
			{
				return GetGameGuiManager()->m_guiAutoUse->m_pTableMaterial1;
			}
			//家族BOSS
			if(pos.y == 0)
			{
				return GetGameGuiManager()->m_guiJiazuboss->m_pTableMaterial1;
			}
			//坐骑图鉴
			if(pos.y == 15)
			{
				return GetGameGuiManager()->m_guiZuoqi->m_pTableMaterial1;
			}
		}
	//商城新	
	case OBJECTCELLTYPE_MALL:
		{
			if (GetGameGuiManager()->m_pDlgMall)
			{
				return GetGameGuiManager()->m_pDlgMall->m_pGuiWare[pos.y]->m_pTable;
			}
		}
		break;	
	///////////////////////////////////////////////////////////////////////////////////		
		case OBJECTCELLTYPE_ANNEX: 
		{
			if (GetGameGuiManager()->m_pDlgEquipAnnexpack)
			{
				switch(pos.y) 
				{
				case MAKECELLTYPE_EQUIP:
					return GetGameGuiManager()->m_pDlgEquipAnnexpack->m_pTableFocusItem;
				case MAKECELLTYPE_MATERIAL1:
					return GetGameGuiManager()->m_pDlgEquipAnnexpack->m_pTableMaterial1;
				}
			}
		}
		break;	
	case OBJECTCELLTYPE_HANDBOOK: //图鉴助手
		{
			//功勋竞猜 借用2号table
			if(pos.y == 2)
			{
				return GetGameGuiManager()->m_guigongxun->m_pTableMaterial1;
			}
		    if (GetGameGuiManager()->m_pDlgEquipbookRecast)
			{
				switch(pos.y) 
				{
				case MAKECELLTYPE_EQUIP:
					return GetGameGuiManager()->m_pDlgEquipbookRecast->m_pTableSFocusItem;
				case MAKECELLTYPE_MATERIAL1:
					return GetGameGuiManager()->m_pDlgEquipbookRecast->m_pTableSMaterial1;
				}
			}
	    }
		break;	
	case OBJECTCELLTYPE_FABAO: //法宝助手
		{
		    if (GetGameGuiManager()->m_pDlgEquipFabaoRecast)
			{
				switch(pos.y) 
				{
				case MAKECELLTYPE_EQUIP:
					return GetGameGuiManager()->m_pDlgEquipFabaoRecast->m_pTableSFocusItem;
				case MAKECELLTYPE_MATERIAL1:
					return GetGameGuiManager()->m_pDlgEquipFabaoRecast->m_pTableSMaterial1;
				}
			}
	    }
		break;	
	case OBJECTCELLTYPE_YUANSHEN: //元神助手
		{
		    if (GetGameGuiManager()->m_pDlgEquipYuanshenRecast)
			{
				switch(pos.y) 
				{
				case MAKECELLTYPE_EQUIP:
					return GetGameGuiManager()->m_pDlgEquipYuanshenRecast->m_pTableSFocusItem;
				case MAKECELLTYPE_MATERIAL1:
					return GetGameGuiManager()->m_pDlgEquipYuanshenRecast->m_pTableSMaterial1;
				}
			}
	    }
		break;
	case OBJECTCELLTYPE_COWRYBOX:
		{
			if (GetGameGuiManager()->m_guiCowrybox)
			{
				return GetGameGuiManager()->m_guiCowrybox->m_pTable[pos.y];
			}
		}
		break;
	case OBJECTCELLTYPE_MAKE:
		{
			if (GetGameGuiManager()->m_guiMakeTo1)
			{
				if (GetGameGuiManager()->m_guiMakeTo1->m_nPos == pos.dwTableID)
				{
					switch(pos.y) 
					{
					case MAKECELLTYPE_EQUIP:
						return GetGameGuiManager()->m_guiMakeTo1->m_pEquipTable;
					case MAKECELLTYPE_MATERIAL1:
						return GetGameGuiManager()->m_guiMakeTo1->m_pMateroalTable1;
					case MAKECELLTYPE_MATERIAL2:
						return GetGameGuiManager()->m_guiMakeTo1->m_pMateroalTable2;
					case MAKECELLTYPE_MATERIAL3:
						return GetGameGuiManager()->m_guiMakeTo1->m_pMateroalTable3;
					case MAKECELLTYPE_MATERIAL4:
						return GetGameGuiManager()->m_guiMakeTo1->m_pMateroalTable4;
					}
				}
			}

			if (GetGameGuiManager()->m_guiMakeTo2)
			{
				if (GetGameGuiManager()->m_guiMakeTo2->m_nPos == pos.dwTableID)
				{
					switch(pos.y) 
					{
					case MAKECELLTYPE_EQUIP:
						return GetGameGuiManager()->m_guiMakeTo2->m_pEquipTable;
					case MAKECELLTYPE_MATERIAL1:
						return GetGameGuiManager()->m_guiMakeTo2->m_pMateroalTable1;
					case MAKECELLTYPE_MATERIAL2:
						return GetGameGuiManager()->m_guiMakeTo2->m_pMateroalTable2;
					case MAKECELLTYPE_MATERIAL3:
						return GetGameGuiManager()->m_guiMakeTo2->m_pMateroalTable3;
					case MAKECELLTYPE_MATERIAL4:
						return GetGameGuiManager()->m_guiMakeTo2->m_pMateroalTable4;
					}
				}
			}
		}
		break;
	case OBJECTCELLTYPE_EQUIP:
		switch(pos.y) 
		{
		case EQUIPCELLTYPE_PACKAGE:
			if (GetGameGuiManager()->m_guiItem)
			{
               //soke 包裹的额外包裹
				switch(pos.x) 
				{
				case EQUIPCELLTYPE_LEFT:
					return GetGameGuiManager()->m_guiItem->m_pTableLeft;
				case EQUIPCELLTYPE_RIGHT:
					return GetGameGuiManager()->m_guiItem->m_pTableRight;
				}
			}
			break;
		case EQUIPCELLTYPE_MAKE:
			if (GetGameGuiManager()->m_guiMain)
			{
				switch(pos.x) 
				{
				case EQUIPCELLTYPE_LEFT:
					return GetGameGuiManager()->m_guiMain->m_pTableLeft;
				case EQUIPCELLTYPE_RIGHT:
					return GetGameGuiManager()->m_guiMain->m_pTableRight;
				}
			}
			break;
		default:
			return GetGameGuiManager()->m_guiItem->GetEquipTable(pos.y,pos.x);
		}
		break;
	case OBJECTCELLTYPE_EQUIPSHOW:
		if (GetGameGuiManager()->m_guiUserInfoShow)
			return GetGameGuiManager()->m_guiUserInfoShow->GetEquipTable(pos.y,pos.x);
		break;
	case OBJECTCELLTYPE_MOUSE:
		return GetGameGuiManager()->m_guiFloatItem->m_pTable;
		break;
	case OBJECTCELLTYPE_COMMON:
		return GetGameGuiManager()->m_guiItem->m_pTable;
		break;
	case OBJECTCELLTYPE_TRADE:
	case OBJECTCELLTYPE_OTHERTRADE:
		return GetGameGuiManager()->m_guiTrade->GetItemTable(pos.dwLocation);
		break;
	case OBJECTCELLTYPE_SELL: //soke 2016 - 3 -10 增加工资界面物品显示
    	if(GetGameGuiManager()->m_guiTicketTrade) //积分
			return GetGameGuiManager()->m_guiTicketTrade->m_pTable;	
    	if(GetGameGuiManager()->m_guiChongzhiTrade) //充值点
			return GetGameGuiManager()->m_guiChongzhiTrade->m_pTable;				
		if(GetGameGuiManager()->m_guiSalaryTrade) //工资
			return GetGameGuiManager()->m_guiSalaryTrade->m_pTable;
		if(GetGameGuiManager()->m_guiNpcTrade) //金子
			return GetGameGuiManager()->m_guiNpcTrade->m_pTable;	
		else
			return NULL;
		break;
	case OBJECTCELLTYPE_STORE:
		{
			if(GetGameGuiManager()->m_guiSavingBoxDlg)
			{
				return GetGameGuiManager()->m_guiSavingBoxDlg->GetBoxTable(pos);
			}
		}
	case OBJECTCELLTYPE_PACKAGE:
		{
			if (GetGameGuiManager()->m_guiPackage1)
			{
				if (GetGameGuiManager()->m_guiPackage1->m_pTable->m_iTableID == pos.dwTableID)
				{
					return GetGameGuiManager()->m_guiPackage1->m_pTable;
				}
			}

			if (GetGameGuiManager()->m_guiPackage2)
			{
				if (GetGameGuiManager()->m_guiPackage2->m_pTable->m_iTableID == pos.dwTableID)
				{
					return GetGameGuiManager()->m_guiPackage2->m_pTable;
				}
			}
		}
		break;
	case OBJECTCELLTYPE_MYSHOP:
		{
			if(GetGameGuiManager()->m_guiShop)
			{
				return GetGameGuiManager()->m_guiShop->m_shopTable;
			}
		}		
		break;
	case OBJECTCELLTYPE_OTHERSSHOP:
		{
			if(GetGameGuiManager()->m_guiOthersShop)
			{
				return GetGameGuiManager()->m_guiOthersShop->m_shopTable;
			}
		}		
		break;
	case OBJECTCELLTYPE_MAIL:
		{
			switch( pos.dwTableID )
			{
			case 0 :
				{
					if(GetGameGuiManager()->m_guiSendMailDlg)
					{
						return GetGameGuiManager()->m_guiSendMailDlg->m_pItemTable;
					}
				}
				break;
			case 1:
				if(GetGameGuiManager()->m_guiRecvMailDlg)
				{
					return GetGameGuiManager()->m_guiRecvMailDlg->m_pItemTable;
				}
				break;
			default:
				if(GetGameGuiManager()->m_guiAuctionDlg)
				{
					return GetGameGuiManager()->m_guiAuctionDlg->GetTableByPos( pos );
				}
				break;
			}
		}
		break;
	case OBJECTCELLTYPE_COUNTRY_SAVEBOX:
		{
			if(GetGameGuiManager()->m_guiCountrySaveingBoxDlg)
				return GetGameGuiManager()->m_guiCountrySaveingBoxDlg->m_pTable;
		}
		break;
	case OBJECTCELLTYPE_PET:
		{
			if( GetGameGuiManager()->m_guiPetPackDlg )
			{
				return GetGameGuiManager()->m_guiPetPackDlg->m_pPetPackTable;
			}
		}
		break;
	}
	return NULL;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param dwLocation : 描述
* \param count : 描述
* \return 返回值的描述
*/
void SetMoneyText(DWORD dwLocation,DWORD count)
{
	FUNCTION_BEGIN;

	switch(dwLocation)
	{
	case OBJECTCELLTYPE_COMMON:
		if(GetGameGuiManager()->m_guiItem)
			GetGameGuiManager()->m_guiItem->SetMoney(count);
		break;
	case OBJECTCELLTYPE_TRADE:
	case OBJECTCELLTYPE_OTHERTRADE:
		if(GetGameGuiManager()->m_guiTrade)
			return GetGameGuiManager()->m_guiTrade->SetMoney(dwLocation,count);
		break;
	}

	FUNCTION_END;
}

inline bool bIsShopDialogShown()
{
	if(GetGameGuiManager()->m_guiShop)
	{
		return GetGameGuiManager()->m_guiShop->IsVisible();
	}
	if(GetGameGuiManager()->m_guiOthersShop)
	{
		return GetGameGuiManager()->m_guiOthersShop->IsVisible();
	}
	return false;
}

inline bool bShowMyTip(CGuiTable* pTable)
{
	if( NULL == pTable )
	{
		return false;
	}
	if( pTable->m_iTableType == OBJECTCELLTYPE_MYSHOP ||
		pTable->m_iTableType == OBJECTCELLTYPE_OTHERSSHOP ||
		(pTable->m_iTableType == OBJECTCELLTYPE_COMMON && 
		bIsShopDialogShown() ))
	{
		return true;
	}
	return false;	
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pTable : 描述
* \param fElapsedTime : 描述
* \return 返回值的描述
*/
void TableRender(CGuiTable* pTable,float fElapsedTime)
{
	FUNCTION_BEGIN;
	
	if( NULL == pTable )
	{
		return ;
	}

	static CToolTips s_ToolTips;
	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	CGuiFloatItem* pFloatItem = NULL;
	stRectI tableRect = pTable->m_itemRect;
	bool    m_bMouseOn = false;

	pTable->m_pDialog->ClientToScreen(&tableRect);
	if(tableRect.PtInRect(ptCursor))
	{
		m_bMouseOn = true;
	}

	//sky 装备栏的显示
	if ((pTable->m_iTableType==OBJECTCELLTYPE_EQUIP || OBJECTCELLTYPE_EQUIPSHOW)&&(pTable->m_listItem.size()==1)) //修复查看别人装备栏问题OBJECTCELLTYPE_EQUIPSHOW是非自己穿着装备
	{
		CRoleItem* pItem =(CRoleItem*) ((CGuiItemCell*)pTable->m_listItem[0])->m_pItemData;
		if (pItem&&GetGameGuiManager()->m_guiUserInfo&&((pFloatItem==NULL)||(pFloatItem&&pFloatItem->GetItem()->m_pItemData!=pItem)))
		{
			switch(pTable->m_EquipPosition.y)
			{
			case EQUIPCELLTYPE_HELM:
			case EQUIPCELLTYPE_NECKLACE:
			case EQUIPCELLTYPE_GLOVES:
			case EQUIPCELLTYPE_RING:
			case EQUIPCELLTYPE_BELT:
			case EQUIPCELLTYPE_SHOES:
			case EQUIPCELLTYPE_BODY:
			case EQUIPCELLTYPE_HANDR:
			case EQUIPCELLTYPE_HANDL:
				{
					//CRoleItem* pItem = (CRoleItem*)pFloatItem->GetItem()->m_pItemData;
					if (IsCellItemType(pTable->m_EquipPosition.y,pItem->GetItemType()))
					{
						//sky 把斗魂的特效也给显示
						/*if(pItem->GetObject()->douhun >= 13)
						{
							IBitmaps* pBmp=NULL;
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",7,2)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-316,tableRect.top-132,xtimeGetTime(),-1,Blend_Null);
							}
						}*/

						if(pItem->GetObject()->retain2 == 6)
						{
							IBitmaps* pBmp=NULL;
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",11,4)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-10,tableRect.top-6,xtimeGetTime(),-1,Blend_Null);
							}
						}
						else if(pItem->GetObject()->retain2 == 5)
						{
							IBitmaps* pBmp=NULL;
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",11,3)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-10,tableRect.top-6,xtimeGetTime(),-1,Blend_Null);
							}
						}
						else if(pItem->GetObject()->retain2 == 4)
						{
							IBitmaps* pBmp=NULL;
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",11,2)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-10,tableRect.top-6,xtimeGetTime(),-1,Blend_Null);
							}
						}
						else if(pItem->GetObject()->retain2 == 3)
						{
							IBitmaps* pBmp=NULL;
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",11,1)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-10,tableRect.top-6,xtimeGetTime(),-1,Blend_Null);
							}
						}
						else if(pItem->GetObject()->retain2 == 2)
						{
							IBitmaps* pBmp=NULL;
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",11,0)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-10,tableRect.top-6,xtimeGetTime(),-1,Blend_Null);
							}
						}
					}
				}
				break;
			case EQUIPCELLTYPE_AMULET:
                {
                    //CRoleItem* pItem = (CRoleItem*)pFloatItem->GetItem()->m_pItemData;
                    if (IsCellItemType(pTable->m_EquipPosition.y,pItem->GetItemType()))
                    {
                        //sky 护身符装备闪
                        if (pItem->GetItemType() ==  ItemType_Amulets)
                        {
                            IBitmaps* pBmp=NULL;
                            pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces3.gl",7,4)));
                            if (pBmp)
                            {
                                pBmp->RenderAni(tableRect.left-8,tableRect.top-6,xtimeGetTime(),-1,Blend_Null);
                            }
                        }
                    }
                }
                break;
			case EQUIPCELLTYPE_ADORN:
				{
					//CRoleItem* pItem = (CRoleItem*)pFloatItem->GetItem()->m_pItemData;
					if (IsCellItemType(pTable->m_EquipPosition.y,pItem->GetItemType()))
					{
						//sky 寒冰麒麟装备闪
						if (pItem->GetItemType() ==  ItemType_Hanbing)
						{
							IBitmaps* pBmp=NULL;
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces3.gl",7,4)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-8,tableRect.top-6,xtimeGetTime(),-1,Blend_Null);
							}
						}
					}
				}
				break;
			// case EQUIPCELLTYPE_CBAMULET: //sky 翅膀内形坐标
			// 	{
			// 		if (IsCellItemType(pTable->m_EquipPosition.y,pItem->GetItemType()))
			// 		{
			// 			//sky 
			// 			if (pItem->GetItemType() ==  ItemType_Chibang)
			// 			{
			// 				if(GetScene()->GetMainCharacter()->GetProperty()->angel == 1)
			// 				{
							
			// 					if(pItem->GetObject()->retain4 > 401 && pItem->GetObject()->retain4 <= 1600)
			// 					{
			// 						IBitmaps* pBmp=NULL;
			// 						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\items.gl",3,1328)));
			// 						if (pBmp)
			// 						{
			// 							pBmp->RenderAni(tableRect.left-308,tableRect.top-160,xtimeGetTime(),-1,Blend_Null);
			// 						}
			// 					}
			// 					if(pItem->GetObject()->retain4 > 1600)
			// 					{
			// 						IBitmaps* pBmp=NULL;
			// 						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\items.gl",3,1712)));
			// 						if (pBmp)
			// 						{
			// 							pBmp->RenderAni(tableRect.left-308,tableRect.top-160,xtimeGetTime(),-1,Blend_Null);
			// 						}
			// 					}
								
			// 				}
			// 			}
			// 		}
			// 	}
			// 	break;
			
			case EQUIPCELLTYPE_XZAMULET:
				{
					//CRoleItem* pItem = (CRoleItem*)pFloatItem->GetItem()->m_pItemData;
					if (IsCellItemType(pTable->m_EquipPosition.y,pItem->GetItemType()))
					{
						//sky 勋章装备闪
						if (pItem->GetItemType() ==  ItemType_Xunzhang)
						{
							IBitmaps* pBmp=NULL;
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces3.gl",7,4)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-8,tableRect.top-6,xtimeGetTime(),-1,Blend_Null);
							}
						}
					}
				}
				break;
			
			}
			
		}
	}

	if ((pTable->m_iTableType==OBJECTCELLTYPE_EQUIP)&&(pFloatItem = GetGuiManager()->GetFloatItem()))
	{
		if (((CGuiUserInfo*)pTable->m_pDialog)->IsMainInfo())
		{
			switch(pTable->m_EquipPosition.y)
			{
			case EQUIPCELLTYPE_HELM:
			case EQUIPCELLTYPE_NECKLACE:
			case EQUIPCELLTYPE_GLOVES:
			case EQUIPCELLTYPE_RING:
			case EQUIPCELLTYPE_BELT:
			case EQUIPCELLTYPE_SHOES:
			case EQUIPCELLTYPE_BODY:
			case EQUIPCELLTYPE_HANDR:
			case EQUIPCELLTYPE_HANDL:
			case EQUIPCELLTYPE_ADORN:
			case EQUIPCELLTYPE_FASHION:  //soke 时装
			case EQUIPCELLTYPE_AMULET:   //soke 护身符
			case EQUIPCELLTYPE_HORSESHOE:  //soke 马脚
			case EQUIPCELLTYPE_HORSEROPE:  //soke 马绳
			case EQUIPCELLTYPE_HORSESADDLE:  //soke 马鞍
			case EQUIPCELLTYPE_HORSESAFE:  //soke 马甲
			case EQUIPCELLTYPE_HORSEIRON:  //soke 马镫
			case EQUIPCELLTYPE_HORSEFASHION:  //soke 马匹圣石
			case EQUIPCELLTYPE_CBAMULET:	/// 翅膀
			case EQUIPCELLTYPE_XZAMULET:	/// 段位勋章
			case EQUIPCELLTYPE_SBAMULET:	/// 神兵
			case EQUIPCELLTYPE_SSAMULET:	/// 神兽
			case EQUIPCELLTYPE_SHANGFANGBAOJIAN:	//尚方宝剑
			case EQUIPCELLTYPE_ZIJINLONGPAO: 	//紫金龙袍
			case EQUIPCELLTYPE_PFAMULET:	/// 披风
			case EQUIPCELLTYPE_SEXX1:	/// 十二星宿附件
			case EQUIPCELLTYPE_SEXX2:	/// 十二星宿附件
			case EQUIPCELLTYPE_SEXX3:	/// 十二星宿附件
			case EQUIPCELLTYPE_SEXX4:	/// 十二星宿附件
			case EQUIPCELLTYPE_SEXX5:	/// 十二星宿附件
			case EQUIPCELLTYPE_SEXX6:	/// 十二星宿附件
			case EQUIPCELLTYPE_SEXX7:	/// 十二星宿附件
			case EQUIPCELLTYPE_SEXX8:	/// 十二星宿附件
			case EQUIPCELLTYPE_SEXX9:	/// 十二星宿附件
			case EQUIPCELLTYPE_SEXX10:	/// 十二星宿附件
			case EQUIPCELLTYPE_SEXX11:	/// 十二星宿附件
			case EQUIPCELLTYPE_SEXX12:	/// 十二星宿附件
				{
					CRoleItem* pItem = (CRoleItem*)pFloatItem->GetItem()->m_pItemData;
					if (IsCellItemType(pTable->m_EquipPosition.y,pItem->GetItemType()))
					{
						//soke 装备界面鼠标点击装备移动显示装备原位置
						IBitmaps* pBmp=NULL;
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,50)));
						if (pBmp)
						{
							pBmp->RenderAni(tableRect.left-2,tableRect.top-2,xtimeGetTime(),-1,Blend_Null);
						}
					}
				}
				break;
			}
		}
	}
	
	//装备五行激活特效
	if ((pTable->m_iTableType==OBJECTCELLTYPE_EQUIP)&&(pTable->m_listItem.size()==1))
	{
		CRoleItem* pItem =(CRoleItem*) ((CGuiItemCell*)pTable->m_listItem[0])->m_pItemData;
		if (pItem&&GetGameGuiManager()->m_guiUserInfo&&((pFloatItem==NULL)||(pFloatItem&&pFloatItem->GetItem()->m_pItemData!=pItem)))
		{
            //soke 神圣祝福装备灵魂激活特效
			if (GetGameGuiManager()->m_guiUserInfo->IsActiveItem(pItem))
			{
				if( !(pItem->GetObject()->kind & 4 )|| (pItem->GetObject()->kind & 16))
				{
					IBitmaps* pBmp=NULL;
					switch(pItem->GetObject()->fivetype )
					{
					//soke 装备激活灵魂特效
					case FIVE_METAL:
						{
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",49,2)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
							}
						}
						break;
					case FIVE_WOOD:
						{
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",49,3)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
							}
						}
						break;
					case FIVE_SOIL:
						{
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",49,4)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
							}
						}
						break;
					case FIVE_WATER:
						{
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",49,5)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
							}
						}
						break;
					case FIVE_FIRE:
						{
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",49,6)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
							}
						}
						break;
					}
				}
				else if( (pItem->GetObject()->kind & 4) || !(pItem->GetObject()->kind & 16))
				{
					IBitmaps* pBmp=NULL;
					switch(pItem->GetObject()->fivetype )
					{
					//soke 装备激活灵魂特效
					case FIVE_METAL:
						{
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,100)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
							}
						}
						break;
					case FIVE_WOOD:
						{
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,101)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
							}
						}
						break;
					case FIVE_SOIL:
						{
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,102)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
							}
						}
						break;
					case FIVE_WATER:
						{
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,103)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
							}
						}
						break;
					case FIVE_FIRE:
						{
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,104)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
							}
						}
						break;
					}
				}
			}
		}
	}
	if ((pTable->m_iTableType == OBJECTCELLTYPE_EQUIP || pTable->m_iTableType == OBJECTCELLTYPE_EQUIPSHOW) && (pTable->m_listItem.size() == 1))
	    	{

				CRoleItem* pItem =(CRoleItem*) ((CGuiItemCell*)pTable->m_listItem[0])->m_pItemData;
			 	switch (pItem->GetItemType())
				{
					case ItemType_Fabao:
					case ItemType_Fabaofj1:
					case ItemType_Fabaofj2:
					case ItemType_Fabaofj3:
					case ItemType_Fabaofj4:
					case ItemType_Fabaofj5:
					case ItemType_Fabaofj6:
					case ItemType_Fabaofj7:
					case ItemType_Fabaofj8:
					case ItemType_Fabaofj9:
					case ItemType_Fabaofj10:
					case ItemType_Fabaofj11:
					case ItemType_Fabaofj12:
					{
						if ( pTable->m_iTableType == OBJECTCELLTYPE_EQUIPSHOW )	//soke 对方的装备
						{
							GetGameGuiManager()->m_guiUserInfoShow->OnPutFocusItem(pItem->GetItemType(),pItem);
						}
						else{
							GetGameGuiManager()->m_guiUserInfo->OnPutFocusItem(pItem->GetItemType(),pItem);
						}
						if(pItem->GetObject()->retain20 >=1000){
							IBitmaps* pBmp=NULL;
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",7,0)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-8,tableRect.top-6,xtimeGetTime(),-1,Blend_Null);
							}
						}
					}
					break;
					case ItemType_YUANSHEN://元神装备回调
					case ItemType_YUANSHENFJ1:
					case ItemType_YUANSHENFJ2:
					case ItemType_YUANSHENFJ3:
					case ItemType_YUANSHENFJ4:
					case ItemType_YUANSHENFJ5:
					case ItemType_YUANSHENFJ6:
					{
						if ( pTable->m_iTableType == OBJECTCELLTYPE_EQUIPSHOW )	//soke 对方的装备
						{
							GetGameGuiManager()->m_guiUserInfoShow->OnPutFocusItem2(pItem->GetItemType(),pItem);
						}
						else{
							GetGameGuiManager()->m_guiUserInfo->OnPutFocusItem2(pItem->GetItemType(),pItem);
						}
						if(pItem->GetObject()->retain22 >=1000){
							IBitmaps* pBmp=NULL;
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",7,0)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-8,tableRect.top-6,xtimeGetTime(),-1,Blend_Null);
							}
						}
					}
					break;
					case ItemType_BABYFJ1://宝宝装备回调
					case ItemType_BABYFJ2:
					case ItemType_BABYFJ3:
					case ItemType_BABYFJ4:
					case ItemType_BABYFJ5:
					case ItemType_BABYFJ6:
					{
						
						GetGameGuiManager()->m_guiBaby->OnPutFocusItem(pItem->GetItemType(),pItem);
						
						if(pItem->GetObject()->retain35 >=1000){
							IBitmaps* pBmp=NULL;
							pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",7,0)));
							if (pBmp)
							{
								pBmp->RenderAni(tableRect.left-8,tableRect.top-6,xtimeGetTime(),-1,Blend_Null);
							}
						}
					}
					break;
					default:
					{
						if ( pTable->m_iTableType == OBJECTCELLTYPE_EQUIPSHOW )	//soke 对方的装备
						{
							GetGameGuiManager()->m_guiUserInfoShow->OnPutFocusItem(pItem->GetItemType(),pItem);
							GetGameGuiManager()->m_guiUserInfoShow->OnPutFocusItem2(pItem->GetItemType(),pItem);
						}
						else{
							GetGameGuiManager()->m_guiUserInfo->OnPutFocusItem(pItem->GetItemType(),pItem);
							GetGameGuiManager()->m_guiUserInfo->OnPutFocusItem2(pItem->GetItemType(),pItem);
						}
						
					}
					break;
				}
															  					
    	    }	
	//装备五行激活特效 修复对方看坐骑马匹装备界面激活效果不显示
	if ((pTable->m_iTableType == OBJECTCELLTYPE_EQUIP || pTable->m_iTableType == OBJECTCELLTYPE_EQUIPSHOW) && (pTable->m_listItem.size() == 1))
	{
		CRoleItem* pItem =(CRoleItem*) ((CGuiItemCell*)pTable->m_listItem[0])->m_pItemData;
		    if (pItem->GetItemType() ==  ItemType_HorseShoe||pItem->GetItemType() ==  ItemType_HorseRope||pItem->GetItemType() ==  ItemType_HorseSaddle
			   ||pItem->GetItemType() ==  ItemType_HorseSafe||pItem->GetItemType() ==  ItemType_HorseIron||pItem->GetItemType() ==  ItemType_HorseFashion)
			{
				IBitmaps* pBmp=NULL;
				switch(pItem->GetObject()->fivetype)
				{
					
				//soke 装备激活灵魂特效
				case FIVE_METAL:
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,100)));
						if (pBmp)
						{
							pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
						}
					}
					break;
				case FIVE_WOOD:
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,101)));
						if (pBmp)
						{
							pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
						}
					}
					break;
				case FIVE_SOIL:
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,102)));
						if (pBmp)
						{
							pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
						}
					}
					break;
				case FIVE_WATER:
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,103)));
						if (pBmp)
						{
							pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
						}
					}
					break;
				case FIVE_FIRE:
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,104)));
						if (pBmp)
						{
							pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
						}
					}
					break;
				}
			}
		}
       //百兽图鉴装备后激活卡牌
		if ((pTable->m_iTableType == OBJECTCELLTYPE_EQUIP) && (pTable->m_listItem.size() == 1))
	    {

		    CRoleItem* pItem =(CRoleItem*) ((CGuiItemCell*)pTable->m_listItem[0])->m_pItemData;
			IBitmaps* pBmp=NULL;
			    //图鉴1//////////////////////////////////////////////////////////////////////////////////////////////////////////
				if (pItem->GetItemType() == ItemType_handbookitem1)
				{

					if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)
					{
				    	pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1106)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-242,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)//增加白色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,223)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-275,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}						
					else if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1109)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-242,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)//增加蓝色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,89)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-275,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}						
					else if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1112)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-242,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)//增加黄色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-275,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}						
					else if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1115)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-242,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)//增加绿色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,227)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-275,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}						
					else if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1118)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-242,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}	
				    if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500) //增加紫色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,228)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-275,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}	
					else if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1118)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-242,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}	
				    if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600) //增加橙色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-275,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}														
					else if(pItem->GetObject()->handbooklvg >= 601)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1992)));				
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-242,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
				    if(pItem->GetObject()->handbooklvg >= 601) //增加红色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,224)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-275,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
				}
                //图鉴2//////////////////////////////////////////////////////////////////////////////////////////////////////////
				if (pItem->GetItemType() == ItemType_handbookitem2)
				{
					if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)
					{ 
					 	pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1121)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-154,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					} 
				    if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)//增加白色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,223)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-187,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}						
					else if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1124)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-154,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)//增加蓝色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,89)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-187,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}						
					else if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1127)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-154,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)//增加黄色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-187,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}						
					else if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1130)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-154,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)//增加绿色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,227)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-187,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1133)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-154,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500) //增加紫色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,228)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-187,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}	
					else if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1666)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-154,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600) //增加橙色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-187,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}	
					else if(pItem->GetObject()->handbooklvg >= 601)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1995)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-154,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}													
				    if(pItem->GetObject()->handbooklvg >= 601) //增加红色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,224)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-187,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}						
				}
				 //图鉴3//////////////////////////////////////////////////////////////////////////////////////////////////////////
				if (pItem->GetItemType() == ItemType_handbookitem3)
				{
					if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)
					{ 
					 	pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1136)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-66,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					} 
				    if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)//增加白色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,223)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-99,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1139)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-66,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)//增加蓝色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,89)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-99,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1142)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-66,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)//增加黄色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-99,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1145)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-66,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)//增加绿色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,227)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-99,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
						}
					}   					
					else if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1148)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-66,tableRect.top-364,xtimeGetTime(),-1,Blend_Null); 
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500) //增加紫色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,228)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-99,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1669)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-66,tableRect.top-364,xtimeGetTime(),-1,Blend_Null); 
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600) //增加橙色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-99,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 601)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1998)));	
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-66,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
				    if(pItem->GetObject()->handbooklvg >= 601) //增加红色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,224)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-99,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}																
				}
                 //图鉴4//////////////////////////////////////////////////////////////////////////////////////////////////////////
				if (pItem->GetItemType() == ItemType_handbookitem4)
				{
					if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)
					{ 
					 	pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1151)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+20,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					} 
				    if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)//增加白色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,223)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-13,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}						
					else if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1154)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+20,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)//增加蓝色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,89)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-13,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1157)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+20,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)//增加黄色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-13,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}						
					else if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1160)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+20,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)//增加绿色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,227)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-13,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1163)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+20,tableRect.top-364,xtimeGetTime(),-1,Blend_Null); 
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500) //增加紫色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,228)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-13,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1672)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+20,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}	
				    if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600) //增加橙色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-13,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}														
					else if(pItem->GetObject()->handbooklvg >= 601)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,2001)));				
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+20,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
				    if(pItem->GetObject()->handbooklvg >= 601) //增加红色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,224)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-13,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}						
				}
				//图鉴5//////////////////////////////////////////////////////////////////////////////////////////////////////////
				if (pItem->GetItemType() == ItemType_handbookitem5)
				{
					if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)
					{ 
					 	pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1166)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+108,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)//增加白色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,223)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+75,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}					 
					else if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1169)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+108,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)//增加蓝色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,89)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+75,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1172)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+108,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)//增加黄色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+75,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}						
					else if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1175)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+108,tableRect.top-364,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)//增加绿色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,227)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+75,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1178)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+108,tableRect.top-364,xtimeGetTime(),-1,Blend_Null); 
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500) //增加紫色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,228)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+75,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1675)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+108,tableRect.top-364,xtimeGetTime(),-1,Blend_Null); 
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600) //增加橙色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+75,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}						
					else if(pItem->GetObject()->handbooklvg >= 601)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,2004)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+108,tableRect.top-364,xtimeGetTime(),-1,Blend_Null); 
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 601) //增加红色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,224)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left+75,tableRect.top-392,xtimeGetTime(),-1,Blend_Null);
					    }
					}																	
				}
				//图鉴6//////////////////////////////////////////////////////////////////////////////////////////////////////////
				if (pItem->GetItemType() == ItemType_handbookitem6)
				{
					if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)
					{ 
					 	pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1181)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-432,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					} 
				    if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)//增加白色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,223)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-465,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}						
					else if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1184)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-432,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)//增加蓝色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,89)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-465,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1187)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-432,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)//增加黄色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-465,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1190)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-432,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)//增加绿色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,227)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-465,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1193)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-432,tableRect.top-218,xtimeGetTime(),-1,Blend_Null); 
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500) //增加紫色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,228)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-465,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1678)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-432,tableRect.top-218,xtimeGetTime(),-1,Blend_Null); 
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600) //增加橙色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-465,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 601)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,2007)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-432,tableRect.top-218,xtimeGetTime(),-1,Blend_Null); 
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 601) //增加红色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,224)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-465,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}																
				}
                //图鉴7//////////////////////////////////////////////////////////////////////////////////////////////////////////
				if (pItem->GetItemType() == ItemType_handbookitem7)
				{
					if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)
					{ 
					 	pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1196)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-343,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					} 
				    if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)//增加白色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,223)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-376,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}						
					else if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1199)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-343,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)//增加蓝色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,89)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-376,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1202)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-343,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)//增加黄色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-376,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1205)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-343,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)//增加绿色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,227)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-376,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1208)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-343,tableRect.top-218,xtimeGetTime(),-1,Blend_Null); 
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500) //增加紫色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,228)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-376,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1681)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-343,tableRect.top-218,xtimeGetTime(),-1,Blend_Null); 
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600) //增加橙色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-376,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 601)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,2010)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-343,tableRect.top-218,xtimeGetTime(),-1,Blend_Null); 
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 601) //增加红色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,224)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-376,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}																
				}
				//图鉴8//////////////////////////////////////////////////////////////////////////////////////////////////////////
                if (pItem->GetItemType() == ItemType_handbookitem8)
				{
					if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)
					{ 
					 	pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,848)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-256,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)//增加白色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,223)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-289,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}						 
					else if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,851)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-256,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)//增加蓝色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,89)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-289,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,854)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-256,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)//增加黄色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-289,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,857)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-256,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)//增加绿色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,227)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-289,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,860)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-256,tableRect.top-218,xtimeGetTime(),-1,Blend_Null); 
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500) //增加紫色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,228)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-289,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1654)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-256,tableRect.top-218,xtimeGetTime(),-1,Blend_Null); 
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600) //增加橙色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-289,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 601)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1983)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-256,tableRect.top-218,xtimeGetTime(),-1,Blend_Null); 
					    }
					}	
				    if(pItem->GetObject()->handbooklvg >= 601) //增加红色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,224)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-289,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}															
				}
				//图鉴9//////////////////////////////////////////////////////////////////////////////////////////////////////////
                if (pItem->GetItemType() == ItemType_handbookitem9)
				{
					if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)
					{ 
					 	pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1777)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-169,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					} 
				    if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)//增加白色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,223)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-202,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1780)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-169,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)//增加蓝色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,89)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-202,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1783)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-169,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)//增加黄色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-202,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1786)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-169,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)//增加绿色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,227)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-202,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1789)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-169,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}	
				    if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500) //增加紫色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,228)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-202,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}																
					else if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1812))); 
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-169,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600) //增加橙色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-202,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}	
					else if(pItem->GetObject()->handbooklvg >= 601)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,2076))); 
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-169,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				     if(pItem->GetObject()->handbooklvg >= 601)//增加红色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,224)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-202,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}											
				}
                //图鉴10//////////////////////////////////////////////////////////////////////////////////////////////////////////
                if (pItem->GetItemType() == ItemType_handbookitem10)
				{
					if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)
					{ 
					 	pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,880)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-81,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 0 && pItem->GetObject()->handbooklvg <= 100)//增加白色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,223)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-114,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					 
					else if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,883)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-81,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 101 && pItem->GetObject()->handbooklvg <= 200)//增加蓝色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,89)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-114,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,886)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-81,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 201 && pItem->GetObject()->handbooklvg <= 300)//增加黄色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-114,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,889)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-81,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 301 && pItem->GetObject()->handbooklvg <= 400)//增加绿色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,227)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-114,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,892)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-81,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 401 && pItem->GetObject()->handbooklvg <= 500) //增加紫色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,228)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-114,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1660)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-81,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 501 && pItem->GetObject()->handbooklvg <= 600) //增加橙色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,226)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-114,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}					
					else if(pItem->GetObject()->handbooklvg >= 601)
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,1989)));
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-81,tableRect.top-218,xtimeGetTime(),-1,Blend_Null);
					    }
					}
				    if(pItem->GetObject()->handbooklvg >= 601) //增加红色图鉴闪光
					{
						pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces2.gl",374,224)));						
					    if (pBmp)
					    {
						   pBmp->RenderAni(tableRect.left-114,tableRect.top-244,xtimeGetTime(),-1,Blend_Null);
					    }
					}																	
				
				}			  					
    	        }
			
	if ((pTable->m_iTableType==OBJECTCELLTYPE_EQUIP)&&(pTable->m_listItem.size()==0)&&m_bMouseOn && !GetGuiManager()->GetToolTips())
	{
		if (GetGuiManager()->GetMouseOverDlg()==pTable->m_pDialog)
		{
			s_ToolTips.Clear();
			s_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
			s_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			s_ToolTips.AddText(GetTableContext(pTable->m_EquipPosition.y));
			s_ToolTips.Resize();

			stPointI pt(tableRect.left + tableRect.Width()/2,tableRect.bottom);
			//s_ToolTips.Render(pt.x,pt.y);
			s_ToolTips.RenderTopLeftPrefer(tableRect,GetDevice()->GetWidth(),GetDevice()->GetHeight());
			//GetGuiManager()->SetToolTips(&s_ToolTips,pt);
		}
	}
	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pTable : 描述
* \param pCell : 描述
* \param fElapsedTime : 描述
* \return 返回值的描述
*/
void RoleItemRender(CGuiTable* pTable,CGuiItemCell* pCell,float fElapsedTime)
{
	FUNCTION_BEGIN;

	stRectI cellRect = pTable->GetCellPixelRect(pCell);

	if(pTable->m_iTableType != OBJECTCELLTYPE_MOUSE /*&& pTable->m_iTableType != OBJECTCELLTYPE_EQUIP*/)
	{
		CRoleItem* pRoleItem = (CRoleItem*)pCell->m_pItemData;
		if(pRoleItem)
		{		

			DWORD color = COLOR_ARGB(0,32,38,120); // alpha==0,去除可使用物品图标底色
			if(!pCell->IsEnabled())
				color = COLOR_ARGB(255,128,128,128);
			else if(pRoleItem->IsCanEquip())
				color = COLOR_ARGB(128,136,48,26); 
			else
			{
				stPointI ptCursor = Engine_GetCursor()->GetPosition();
				if(cellRect.PtInRect(ptCursor))
					color = COLOR_ARGB(128,46,60,24);
			}
			GetDevice()->FillRect(&cellRect,color);
		}
	}

	
	{
		CRoleItem* pRoleItem = (CRoleItem*)pCell->m_pItemData;
		if(pRoleItem)
			pCell->SetImageColor(1,pRoleItem->GetObjectBase()->dwColor);

		//Add need repair item to list
		if ( pRoleItem )
		{
			if ( pRoleItem->NeedConsiderDur() )
			{
				if ( ((pRoleItem->GetObject()->dur+49)/50 < NEED_ADD_REPAIR_VALUE) )
				{
					if ( GetScene()->GetMainCharacter() )
						GetScene()->GetMainCharacter()->AddOneNeedRepairItem( pRoleItem->GetThisID() );
				}
				else
				{
					if ( GetScene()->GetMainCharacter() )
						GetScene()->GetMainCharacter()->DeleteOneNeedRepairItem( pRoleItem->GetThisID() );
				}
			}
		}
	}

	bool m_bMouseOn = false;

	CRoleItem* pItem = (CRoleItem*)pCell->m_pItemData;

	if( pItem && pTable->m_iTableType != OBJECTCELLTYPE_MOUSE && !GetGuiManager()->GetToolTips() && GetGuiManager()->GetMouseOverDlg() == pTable->m_pDialog )
	{
		stPointI ptCursor = Engine_GetCursor()->GetPosition();

		if(cellRect.PtInRect(ptCursor))
		{
			m_bMouseOn = true;
		}

	}

	//For some special item we add used sign img
	if ( pItem )
	{
		if ( (pItem->GetObjectID() == c_nXuanYuanFu)
			|| (pItem->GetObjectID() == c_nXianTianFu)
			|| (pItem->GetObjectID() == 666) )
		{
			if ( pItem->GetObject()->maker[0] != 0 )	//Used
			{
				IBitmaps* pBmp=NULL;
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,300)));
				if ( pBmp )
				{
					pBmp->RenderAni(cellRect.left-56,cellRect.top-56,xtimeGetTime(),-1,Blend_Null);
				} 
			}
		}
	}

	//soke 物品等绑定特效
	if ( pItem )
	{   
		//soke 绑定物品显示的特效(包裹、装备、马匹装备、额外包裹、采集手套、马匹圣石、箭支、回血药 除外)
		if ( (pItem->Get2ObjectID() >= 16 && pItem->Get2ObjectID() <= 20) || pItem->Get2ObjectID() == 25 || (pItem->Get2ObjectID() >= 27 && pItem->Get2ObjectID() <= 34) || pItem->Get2ObjectID() == 37 || pItem->Get2ObjectID() == 44 \
			|| pItem->Get2ObjectID() == 49 || (pItem->Get2ObjectID() >= 66 && pItem->Get2ObjectID() <= 68) || (pItem->Get2ObjectID() >= 73 && pItem->Get2ObjectID() <= 78) || pItem->Get2ObjectID() == 81 || pItem->Get2ObjectID() == 85\
			|| pItem->Get2ObjectID() == 86 || pItem->Get2ObjectID() == 127 || pItem->Get2ObjectID() == 128 || (pItem->Get2ObjectID() >= 203 && pItem->Get2ObjectID() <= 205) || pItem->Get2ObjectID() == 217)
		{
			if ( pItem->GetObject()->bind == 1 )	//soke 物品等级
			{
				IBitmaps* pBmp=NULL;
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,501)));
				if ( pBmp )
				{
					pBmp->RenderAni(cellRect.left,cellRect.top,xtimeGetTime(),-1,Blend_Null);
				} 
			}
		}

		//soke 斗魂激活特效显示
		if ( (pItem->Get2ObjectID() >= 101 && pItem->Get2ObjectID() <= 118) 
		  || (pItem->Get2ObjectID() >= 130 && pItem->Get2ObjectID() <= 134) 
		  || (pItem->Get2ObjectID() == 136 ) 
		  || (pItem->Get2ObjectID() == 137 ) 
		  || (pItem->Get2ObjectID() == 138 ) 
		  || (pItem->Get2ObjectID() == 140 ) 
		  || (pItem->Get2ObjectID() >= 141 && pItem->Get2ObjectID() <= 147) 
		  || (pItem->Get2ObjectID() == 155 ) 
		  || (pItem->Get2ObjectID() == 156 ) )		  
		{
			if ( pItem->GetObject()->douhun >= 13 )	//soke 13星以上斗魂激活
			{
				IBitmaps* pBmp=NULL;
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",16,2)));
				if ( pBmp )
				{
					pBmp->RenderAni(cellRect.left-20,cellRect.top-20,xtimeGetTime(),-1,Blend_Null);
				} 
			}
			else if ( pItem->GetObject()->douhun > 0 &&  pItem->GetObject()->douhun <= 12)	 //soke 13星以下斗魂激活
			{
				IBitmaps* pBmp=NULL;
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",16,3)));
				if ( pBmp )
				{
					pBmp->RenderAni(cellRect.left,cellRect.top,xtimeGetTime(),-1,Blend_Null);
				} 
			} 

		}
		if ( pItem->Get2ObjectID() == 140)
		{
			{
				IBitmaps* pBmp=NULL;
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,400)));
				if ( pBmp )
				{
					pBmp->RenderAni(cellRect.left-100,cellRect.top-122,xtimeGetTime(),-1,Blend_Null);
				} 
			}
		}
		//soke 增加一等特定道具显示特效-新
		if ( pItem->GetObjectID() == 3540 || (pItem->GetObjectID() == 680))
		{
			{
				IBitmaps* pBmp=NULL;
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",256,29)));
				if ( pBmp )
				{
					pBmp->RenderAni(cellRect.left-6,cellRect.top-7,xtimeGetTime(),-1,Blend_Null);
				} 
			}
		}
		//soke 增加二等特定道具显示特效-新
		if ( pItem->GetObjectID() == 3541 || (pItem->GetObjectID() == 748))
		{
			{
				IBitmaps* pBmp=NULL;
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",256,30)));
				if ( pBmp )
				{
					pBmp->RenderAni(cellRect.left-6,cellRect.top-7,xtimeGetTime(),-1,Blend_Null);
				} 
			}
		}
		//soke 增加三等特定道具显示特效-新
		if ( pItem->GetObjectID() == 3542 || (pItem->GetObjectID() == 749))
		{
			{
				IBitmaps* pBmp=NULL;
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",256,31)));
				if ( pBmp )
				{
					pBmp->RenderAni(cellRect.left-6,cellRect.top-7,xtimeGetTime(),-1,Blend_Null);
				} 
			}
		}
		//soke 增加四等特定道具显示特效-新
		if ( pItem->GetObjectID() == 3543 || (pItem->GetObjectID() == 750))
		{
			{
				IBitmaps* pBmp=NULL;
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",256,32)));
				if ( pBmp )
				{
					pBmp->RenderAni(cellRect.left-6,cellRect.top-7,xtimeGetTime(),-1,Blend_Null);
				} 
			}
		}
		//soke 增加五等特定道具显示特效-新
		if ( pItem->GetObjectID() == 3544 || (pItem->GetObjectID() == 751))
		{
			{
				IBitmaps* pBmp=NULL;
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",256,33)));
				if ( pBmp )
				{
					pBmp->RenderAni(cellRect.left-6,cellRect.top-7,xtimeGetTime(),-1,Blend_Null);
				} 
			}
		}
		//soke 材料等级特效支持类型显示
		if ( pItem->Get2ObjectID() == ItemType_Resource || (pItem->GetObjectID() == 678 || pItem->GetObjectID() == 679))
		{
			if ( pItem->GetObject()->upgrade == 0 )	//soke 物品等级
			{
				IBitmaps* pBmp=NULL;
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",256,29)));
				if ( pBmp )
				{
					pBmp->RenderAni(cellRect.left-6,cellRect.top-7,xtimeGetTime(),-1,Blend_Null);
				} 
			}
			if ( pItem->GetObject()->upgrade == 1 )	//soke 物品等级
			{
				IBitmaps* pBmp=NULL;
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",256,30)));
				if ( pBmp )
				{
					pBmp->RenderAni(cellRect.left-6,cellRect.top-7,xtimeGetTime(),-1,Blend_Null);
				} 
			}
			if ( pItem->GetObject()->upgrade == 2 )	//soke 物品等级
			{
				IBitmaps* pBmp=NULL;
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",256,31)));
				if ( pBmp )
				{
					pBmp->RenderAni(cellRect.left-6,cellRect.top-7,xtimeGetTime(),-1,Blend_Null);
				} 
			}
			if ( pItem->GetObject()->upgrade == 3 )	//soke 物品等级
			{
				IBitmaps* pBmp=NULL;
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",256,32)));
				if ( pBmp )
				{
					pBmp->RenderAni(cellRect.left-6,cellRect.top-7,xtimeGetTime(),-1,Blend_Null);
				} 
			}
			if ( pItem->GetObject()->upgrade >= 4 )	//soke 物品等级
			{
				IBitmaps* pBmp=NULL;
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",256,33)));
				if ( pBmp )
				{
					pBmp->RenderAni(cellRect.left-6,cellRect.top-7,xtimeGetTime(),-1,Blend_Null);
				} 
			}
		}
	}
	if ( m_bMouseOn )
	{
		//If this item is one of a set we simply update tips when mouse on
		//This may be some FPS lower
		/*if ( pItem->GetObject()->kind & 8 )
		pItem->UpdateToolTips();*/


		//Show hole image and diamond
		pItem->ShowHoleImage();
		//render cell
		pCell->Render( fElapsedTime );

		//Get tips position
		stPointI pt(cellRect.left, cellRect.top - pItem->GetToolTips()->GetHeight() );
		pt.x -= pItem->GetToolTips()->GetWidth();
		//Give tips some EQ
		//First make sure tips full visible
		//Then we make sure tips not on item
		if(pt.x < 0)
			pt.x = cellRect.right;
		if( pt.x + pItem->GetToolTips()->GetWidth() > GetDevice()->GetWidth() )
			pt.x = cellRect.left - pItem->GetToolTips()->GetWidth();

		if(pt.y < 0)
			pt.y = cellRect.bottom;
		if( pt.y + pItem->GetToolTips()->GetHeight() > GetDevice()->GetHeight() )
			pt.y = GetDevice()->GetHeight() - pItem->GetToolTips()->GetHeight();

		//Render
		if (GetGameCursor()->GetCursorImageType()==eCursorRepair)
		{
			if (GetGameGuiManager()->m_guiNpcTrade)
			{
				if (GetGameGuiManager()->m_guiNpcTrade->IsRepair()==CGuiNpcDialog::eTradeRepairByGold)
				{
					pItem->SetTipFlags(pItem->GetTipFlags()|ITEMTIPS_FLAG_REPAIR_PRICEBYGOLD);
				}
				else
				{
					pItem->SetTipFlags(pItem->GetTipFlags()|ITEMTIPS_FLAG_REPAIR_PRICE);
				}
			}
			else
			{
				pItem->SetTipFlags(pItem->GetTipFlags()|ITEMTIPS_FLAG_REPAIR_PRICE);
			}
		}
		else
		{
			if ( !(pItem && pItem->GetObject()->upgrade >= 10) )
				pItem->SetTipFlags(pItem->GetTipFlags()&0xfffffffd);
		}

		pItem->ShowToolTips(pt);
	}
	else
	{
		//Hide hole image and diamond
		pItem->ShowHoleImage( true );
		//render cell
		pCell->Render( fElapsedTime );
	}


	FUNCTION_END;
}

/****************************** table 对应的 物品函数 ********************************/
//坐标点对应的格子
inline CGuiItemCell* GetTableCell(CGuiTable* pTable,POINT ptGrid)
{
	if(NULL == pTable)
	{
		return NULL;
	}

	if( pTable->IsValidGrid(ptGrid) )
	{
		return pTable->GetItemCell(ptGrid.x,ptGrid.y);
	}
	return NULL;
}

//选中的格子
inline CGuiItemCell* GetSelectCell(CGuiTable* pTable,POINT pt)
{
	if(NULL == pTable)
	{
		return NULL;
	}
	stPointI ptDestPixel = pt;

	// 计算目标格子	
	ptDestPixel.x -= pTable->GetX();
	ptDestPixel.y -= pTable->GetY();
	stPointI ptDestGrid = pTable->GetGridPosByPixelPos(ptDestPixel);

	return GetTableCell(pTable,ptDestGrid);
}

//查找table内格子ptGrid内的物品
inline CRoleItem* GetCellItem(CGuiTable* pTable,POINT ptGrid)
{
	CGuiItemCell* pCell = GetTableCell(pTable,ptGrid);
	if( NULL != pCell )
	{
		return (CRoleItem*)(pCell->m_pItemData);
	}	
	return NULL;
}

//得到鼠标选中的物品
inline CRoleItem* GetSelectItem(CGuiTable* pTable,POINT pt)
{
	CGuiItemCell* pCell = GetSelectCell(pTable,pt);
	if(NULL == pCell)
	{
		return NULL;
	}
	return (CRoleItem*)(pCell->m_pItemData);
}


//根据object找对应的CRoleItem
inline CRoleItem* GetItemByObject( const t_Object& object )
{
	CGuiTable* pTable = GetItemTable(object.pos);
	return GetCellItem( pTable,stPointI(object.pos.x,object.pos.y) );
}

/****************************** table 对应的 物品函数 ********************************/

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  处理分裂物品事件
*/
static bool OnSplitItemEvent(CGuiTable* pTable,UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam)
{
	FUNCTION_BEGIN;

	if(!pTable) return false;
	CGuiItemCell* pDestCell = GetSelectCell(pTable,pt);
	if ( !pDestCell || !pDestCell->IsEnabled() )
	{
		return true;
	}
	CRoleItem* pSrcItem = CRoleItem::GetRoleItem(pDestCell);
	Assert(pSrcItem);
	if( pSrcItem && pSrcItem->GetCount() > 1 && pSrcItem->GetMaxCount() > 1 && pSrcItem->GetCellCount()<=1)
	{
		static stSplitItemData st;
		st.qwThisID = pSrcItem->GetThisID();
		st.dwCurNum = pSrcItem->GetCount();
		st.dst.dwLocation = OBJECTCELLTYPE_MOUSE;
		st.dst.dwTableID = 0;
		st.dst.x = 0;
		st.dst.y = 0;
		st.pSrcCell = pDestCell;
		CGuiInputBox* pBox = GetGameGuiManager()->AddInputBox("请输入要移动的数量：",eAction_SplitItem,&st);
		pBox->SetNumber(0,pSrcItem->GetCount());
		CRoleItem::s_bCanMoveItem = false;
		return true;
	}
	return false;

	FUNCTION_END;
}


/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  处理维修单个物品事件
*/
static bool OnRepairItemEvent(CGuiTable* pTable,UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam)
{
	FUNCTION_BEGIN;

	CGuiItemCell* pDestCell = NULL;
	CRoleItem* pSrcItem = NULL;
	CRoleItem* pDestItem = NULL;

	stPointI ptDestGrid;
	stPointI ptDestPixel = pt;

	GetGameCursor()->SetCursor( CCursor::CursorType_Normal, eCursorNormal);

	if(!pTable) return false;

	// 计算目标格子
	ptDestPixel.x -= pTable->GetX();
	ptDestPixel.y -= pTable->GetY();
	ptDestGrid = pTable->GetGridPosByPixelPos(ptDestPixel);

	if(pTable->IsValidGrid(ptDestGrid) && (pDestCell = pTable->GetItemCell(ptDestGrid.x,ptDestGrid.y)) )
	{
		// 灰掉的物品不能襙作
		if(!pDestCell->IsEnabled())return true;

		pSrcItem = (CRoleItem*)pDestCell->m_pItemData;
		Assert(pSrcItem);
		if( pSrcItem)
		{
			switch(pSrcItem->GetObjectID()) 
			{
			case 690:
			case 691:
			case 692:
			case 693:
			case 694:
				{
					GameMessageBox("新手装备不能修理！");
					return true;
				}
				break;
			}

			if (GetGameGuiManager()->m_guiNpcTrade)
			{
				if (GetGameGuiManager()->m_guiNpcTrade->IsRepair()==CGuiNpcDialog::eTradeRepairByGold)
				{
					stRepairObjectGoldNpcTradeUserCmd cmd;
					cmd.id = pSrcItem->GetThisID();

					SEND_USER_CMD(cmd);
				}
				else
				{
					stRepairObjectNpcTradeUserCmd cmd;
					cmd.id = pSrcItem->GetThisID();
					cmd.gem_id = 0;

					SEND_USER_CMD(cmd);
				}

			}

			return true;
		}
	}
	return false;

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  处理买物品事件
*/
static bool OnBuyItemEvent(CGuiItemCell* pCell,bool bLMouseDown)
{
	FUNCTION_BEGIN;

	CRoleItem* pItem = (CRoleItem*)pCell->m_pItemData;
	Assert(pItem);
	static stObjectLocation pos;
	pos = pItem->GetLocation();

	CGuiDialog* pDlg = NULL;

	//soke 最大数量 >1;
	if ((pItem->GetObjectBase()->dwMaxCount>1)&&bLMouseDown)
	{
		//soke 物品等级 >1;
		if (pItem->GetUpGrade() > 1 )
		{
		   //soke 材料的价格
		   float levelopt = pow(5,pItem->GetUpGrade());

		   //soke 银子购买
		   if (pItem->GetObjectBase()->dwTradeType & eBuySilver)
		   {
			   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s，你是否要购买？",pItem->GetName(),Money2String(pItem->GetPrice( ITEM_PRICE_BUY )* pItem->GetCount()* levelopt)),eAction_BuyItem,&pos,c_szMBOKCancel);
		   }
		   //soke 金子购买
		   else if (pItem->GetObjectBase()->dwTradeType & eBuyGold && GetGameGuiManager()->m_guiNpcTrade)
		   {
			   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 金子，你是否要购买？",pItem->GetName(),Money2String(pItem->GetObjectBase()->price * pItem->GetCount()* levelopt)),eAction_BuyItem,&pos,c_szMBOKCancel);
		   }
		   //soke 积分点数购买
		   else if (pItem->GetObjectBase()->dwTradeType & eBuyTicket && GetGameGuiManager()->m_guiTicketTrade)
		   {
			   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 积分，你是否要购买？",pItem->GetName(),Ticket2String(pItem->GetObjectBase()->price * pItem->GetCount()* levelopt)),eAction_TicketBuyItem,&pos,c_szMBOKCancel);
		   }
		   //soke 充值点数购买
		   else if (pItem->GetObjectBase()->dwTradeType & eBuyChongzhi && GetGameGuiManager()->m_guiChongzhiTrade)
		   {
			   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 充值点，你是否要购买？",pItem->GetName(),Chongzhi2String(pItem->GetObjectBase()->price * pItem->GetCount()* levelopt)),eAction_ChongzhiBuyItem,&pos,c_szMBOKCancel);
		   }		   
		   //soke 工资购买
		   else if (pItem->GetObjectBase()->dwTradeType & eBuySalary && GetGameGuiManager()->m_guiSalaryTrade)
		   {
			   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 工资，你是否要购买？",pItem->GetName(),Money2String( (pItem->GetObjectBase()->price) * pItem->GetCount()* levelopt)),eAction_SalaryBuyItem,&pos,c_szMBOKCancel);
		   }
	    }
		else //soke 物品等级 <1
		{
		   //soke 银子购买
		   if (pItem->GetObjectBase()->dwTradeType & eBuySilver)
		   {
			   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s，你是否要购买？",pItem->GetName(),Money2String(pItem->GetPrice( ITEM_PRICE_BUY )* pItem->GetCount())),eAction_BuyItem,&pos,c_szMBOKCancel);
		   }
		   //soke 金子购买
		   else if (pItem->GetObjectBase()->dwTradeType & eBuyGold && GetGameGuiManager()->m_guiNpcTrade)
		   {
			   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 金子，你是否要购买？",pItem->GetName(),Money2String(pItem->GetObjectBase()->price * pItem->GetCount())),eAction_BuyItem,&pos,c_szMBOKCancel);
		   }
		   //soke 积分点数购买  
		   else if (pItem->GetObjectBase()->dwTradeType & eBuyTicket && GetGameGuiManager()->m_guiTicketTrade)
		   {
			  pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 积分，你是否要购买？",pItem->GetName(),Ticket2String(pItem->GetObjectBase()->price * pItem->GetCount())),eAction_TicketBuyItem,&pos,c_szMBOKCancel);
		   }
		   //soke 充值点数购买  
		   else if (pItem->GetObjectBase()->dwTradeType & eBuyChongzhi && GetGameGuiManager()->m_guiChongzhiTrade)
		   {
			  pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 充值点，你是否要购买？",pItem->GetName(),Chongzhi2String(pItem->GetObjectBase()->price * pItem->GetCount())),eAction_ChongzhiBuyItem,&pos,c_szMBOKCancel);
		   }		   
		   //soke 工资购买 
		   else if (pItem->GetObjectBase()->dwTradeType & eBuySalary && GetGameGuiManager()->m_guiSalaryTrade)
		   {
			   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 工资，你是否要购买？",pItem->GetName(),Money2String( (pItem->GetObjectBase()->price ) * pItem->GetCount())),eAction_SalaryBuyItem,&pos,c_szMBOKCancel);
		   }
	    }
	}
	else
	{
		if (pItem->GetObjectBase()->dwType==ItemType_DoubleExp)
		{
			pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("获得 %s 需要 %d 荣誉点，你是否要交换？",pItem->GetName(),pItem->GetObjectBase()->price),eAction_BuyItem,&pos,c_szMBOKCancel);
		}
		else
		{
			//soke 数量=1;等级>1;
			if (pItem->GetUpGrade() > 1)
			{
			   //soke 材料的等级价格
		       float levelopt = pow(5,pItem->GetUpGrade());

			   //soke 银子购买
			   if (pItem->GetObjectBase()->dwTradeType & eBuySilver)
			   {
				   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s，你是否要购买？",pItem->GetName(),Money2String(pItem->GetPrice( ITEM_PRICE_BUY )*levelopt)),eAction_BuyItem,&pos,c_szMBOKCancel);
			   }
			   //soke 金子购买
			   else if (pItem->GetObjectBase()->dwTradeType & eBuyGold && GetGameGuiManager()->m_guiNpcTrade)
			   {
				   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 金子，你是否要购买？",pItem->GetName(),Money2String(pItem->GetObjectBase()->price*levelopt)),eAction_BuyItem,&pos,c_szMBOKCancel);
			   }
			   //soke 积分点购买
			   else if (pItem->GetObjectBase()->dwTradeType & eBuyTicket && GetGameGuiManager()->m_guiTicketTrade)
			   {
				  pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 积分，你是否要购买？",pItem->GetName(),Ticket2String(pItem->GetObjectBase()->price*levelopt)),eAction_TicketBuyItem,&pos,c_szMBOKCancel);
			   }
			   //soke 充值点购买
			   else if (pItem->GetObjectBase()->dwTradeType & eBuyChongzhi && GetGameGuiManager()->m_guiChongzhiTrade)
			   {
				  pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 充值点，你是否要购买？",pItem->GetName(),Chongzhi2String(pItem->GetObjectBase()->price*levelopt)),eAction_ChongzhiBuyItem,&pos,c_szMBOKCancel);
			   }			   
			   //soke 工资购买
			   else if (pItem->GetObjectBase()->dwTradeType & eBuySalary && GetGameGuiManager()->m_guiSalaryTrade)
			   {
				   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 工资，你是否要购买？",pItem->GetName(),Money2String((pItem->GetObjectBase()->price)*levelopt)),eAction_SalaryBuyItem,&pos,c_szMBOKCancel);
			   }
		    }
			else //soke //soke 数量=1;等级<1;
			{
			   //soke 银子购买
			   if (pItem->GetObjectBase()->dwTradeType & eBuySilver)
			   {
				   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s，你是否要购买？",pItem->GetName(),Money2String(pItem->GetPrice( ITEM_PRICE_BUY ))),eAction_BuyItem,&pos,c_szMBOKCancel);
			   }
			   //soke 金子购买
			   else if (pItem->GetObjectBase()->dwTradeType & eBuyGold && GetGameGuiManager()->m_guiNpcTrade)
			   {
				   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 金子，你是否要购买？",pItem->GetName(),Money2String(pItem->GetObjectBase()->price)),eAction_BuyItem,&pos,c_szMBOKCancel);
			   }
			   //soke 积分点数购买
			   else if (pItem->GetObjectBase()->dwTradeType & eBuyTicket && GetGameGuiManager()->m_guiTicketTrade)
			   {
				   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 积分，你是否要购买？",pItem->GetName(),Ticket2String(pItem->GetObjectBase()->price)),eAction_TicketBuyItem,&pos,c_szMBOKCancel);
			   }
			   //soke 充值点数购买
			   else if (pItem->GetObjectBase()->dwTradeType & eBuyChongzhi && GetGameGuiManager()->m_guiChongzhiTrade)
			   {
				   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 充值点，你是否要购买？",pItem->GetName(),Chongzhi2String(pItem->GetObjectBase()->price)),eAction_ChongzhiBuyItem,&pos,c_szMBOKCancel);
			   }			   
			   //soke 工资购买
			   else if (pItem->GetObjectBase()->dwTradeType & eBuySalary && GetGameGuiManager()->m_guiSalaryTrade)
			   {
				   pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("购买 %s 需要 %s 工资，你是否要购买？",pItem->GetName(),Money2String(pItem->GetObjectBase()->price)),eAction_SalaryBuyItem,&pos,c_szMBOKCancel);
			   }
		    }
		}

		if (!bLMouseDown)
		{
			pDlg->SetVisible(false);
			((CGuiMessageBox*)pDlg)->OnButtonClick(GUI_IDOK);
			return true;
		}
	}

	if (GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = (void*) pDlg;

	int width  = pDlg->GetWidth();
	int height = pDlg->GetHeight();

	stPointI pt = Engine_GetCursor()->GetPosition();
	pt.x -= width/2;
	pt.y -= height/2;
	if(pt.x < 0) pt.x = 0;
	if(pt.y < 0) pt.y = 0;
	if (pDlg)
	{
		pDlg->SetAlign(GUI_ALIGN_NONE);
		pDlg->SetColor(COLOR_ARGB(255,255,255,255));
		pDlg->SetLocation(pt.x,pt.y);
	}
	return true;

	FUNCTION_END;
}

bool CRoleItem::IsNeedConfirmBeforeSell()
{
	//三等以上的原材料
	if ( GetItemType() == ItemType_Resource )
	{
		if ( GetObject()->upgrade > 2 )
			return true;
	}

	//9星以上的装备，包括九星的
	if ( IsEquipment() )
	{
		if ( GetObject()->upgrade > 8 )
			return true;
	}

	//神圣装备
	if ( GetObject()->kind & 0x4 )
		return true;

    //神圣祝福装备
	if ( GetObject()->kind & 0x16 )
		return true;

	//非装备以及材料
	if ( (GetItemType() == ItemType_DoubleExp) ||
		(GetItemType() == ItemType_Honor) ||
		(GetItemType() == ItemType_LevelUp) ||
		(GetItemType() == ItemType_ClearPoint) ||
		(GetItemType() == 59) ||
		(GetItemType() == ItemType_SOULSTONE) )
	{
		return true;
	}

	return false;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  处理卖物品事件
*/
static bool OnSellItemEvent( CGuiItemCell* pCell )
{
	FUNCTION_BEGIN;

	CRoleItem* pItem = (CRoleItem*)pCell->m_pItemData;
	Assert(pItem);
	if ( pItem->IsBind())
	{
		GetGameGuiManager()->AddMessageBox("当前物品已经被灵魂绑定，确定要卖给我吗？",eAction_Bind,(void*)pItem,c_szMBOKCancel);
		return true;
	}

	if ( pItem->IsNeedConfirmBeforeSell() )
	{
		GetGameGuiManager()->AddMessageBox( "这可是个好东西,确定要卖给我吗?", eAction_Bind, (void*)pItem, c_szMBOKCancel );
		return true;
	}

	pItem->Sell();
	return true;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pCell : 描述
* \return 返回值的描述
*/
static bool OnSaveItemEvent( CGuiItemCell* pCell )
{
	FUNCTION_BEGIN;

	CRoleItem* pItem = (CRoleItem*)pCell->m_pItemData;
	Assert(pItem);
	pItem->Save();
	return true;

	FUNCTION_END;
}

/**
* \brief 处理自动装备箭支
* 
* 处理自动装备箭支
* 
* \param pCell : 描述
* \return 返回值的描述
*/
bool EquipArrow(const int& num)
{
	if (GetGameGuiManager()->m_guiUserInfo==NULL)
		return false;

	CGuiTable* pTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable(EQUIPCELLTYPE_HANDR);

	if (pTable->m_listItem.size()!=1)
		return false;

	CRoleItem* pRoleItem = (CRoleItem*)pTable->m_listItem[0]->m_pItemData;
	if (pRoleItem==NULL)
		return false;

	if (pRoleItem->GetItemType()!=ItemType_Bow)
		return false;

	pTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable(EQUIPCELLTYPE_HANDL);

	pRoleItem = GetScene()->GetMainCharacter()->GetCanUseArrow( num );

	if (pRoleItem==NULL)
		return false;


	// 交换物品
	stObjectLocation dst;

	PlayItemSound(pRoleItem->GetObjectBase()->itmeSound);
	dst.dwLocation = pTable->m_iTableType;
	dst.dwTableID = pTable->m_iTableID;

	dst.x = pTable->m_EquipPosition.x;
	dst.y = pTable->m_EquipPosition.y;

	pRoleItem->MoveTo(dst);

	return true;
}
/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  处理装备双手道具事件
*/
bool EquipHandsArm(CRoleItem* pRoleItem,CGuiTable* pDestTable)
{
	if (IsHandsArm(pRoleItem->GetItemType())==false)
		return true;

	byte ItemType = IsEquipItem(pRoleItem->GetItemType());
	if (ItemType == 0)
		return true;

	if (pDestTable->m_iTableType != OBJECTCELLTYPE_EQUIP)
		return true;

	if (pDestTable->m_EquipPosition.y != ItemType)
		return true;

	stObjectLocation pos;
	pos.dwLocation = OBJECTCELLTYPE_COMMON;
	CGuiTable* pTable = GetItemTable(pos);
	if (pTable == NULL)
		return false;

	stPointI point = pTable->GetNewPos(1,1);
	if (point == stPointI(-1,-1))
		return false;

	stObjectLocation dst;
	dst.dwLocation = pTable->m_iTableType;
	dst.dwTableID = pTable->m_iTableID;
	dst.x = point.x;
	dst.y = point.y;

	switch(ItemType)
	{
	case EQUIPCELLTYPE_HANDR:
		pTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable(EQUIPCELLTYPE_HANDL);
		break;
	case EQUIPCELLTYPE_HANDL:
		pTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable(EQUIPCELLTYPE_HANDR);
		break;
	default:
		return true;
	}


	CRoleItem* pSrcItem = NULL;
	if (pTable->m_listItem.size()!=1)
		return true;

	pSrcItem = (CRoleItem*)pTable->m_listItem[0]->m_pItemData;
	if (pSrcItem == NULL)
		return false;

	PlayItemSound(pSrcItem->GetObjectBase()->itmeSound);
	pSrcItem->MoveTo(dst);
	return true;
}

bool EquipItem(CRoleItem* pSrcItem,int bLeft)
{
	static bool bRingLeft = true;
	static bool bGloveLeft = true;
	static bool bAdornLeft = true;

	if ( GetScene()->GetMainCharacter()->IsCombinationState() )
	{
		GetGameGuiManager()->AddClientSystemMessage( "合体状态下无法更换装备!" );
		return false;
	}

	CGuiItemCell* pDestCell = NULL;

	byte ItemType = IsEquipItem(pSrcItem->GetItemType());
	if (ItemType==0)
		return false;

	DWORD dwErrorID = pSrcItem->IsCanEquip();
	if( dwErrorID != 0)
	{
		switch(dwErrorID)
		{
		case CANNOTUSE_CHARTYPE:
			GetGameGuiManager()->AddClientSystemMessage("不能装备!");
			break;
		case CANNOTUSE_LEVEL:
			if( pSrcItem->GetObject()->dwObjectID ==90 || pSrcItem->GetObject()->dwObjectID ==82 || (pSrcItem->GetObject()->dwObjectID >= 101 && pSrcItem->GetObject()->dwObjectID <= 118 )|| (pSrcItem->GetObject()->dwObjectID >= 141 && pSrcItem->GetObject()->dwObjectID <= 147))
				GetGameGuiManager()->AddClientSystemMessage("等级不够,不能装备!");
			else
				GetGameGuiManager()->AddClientSystemMessage("等级不够，不能使用");
			break;
		}
		return false;
	}

	if (GetGameGuiManager()->m_guiUserInfo==NULL)
		return false;

	CGuiTable* pEquipTable = NULL;

	switch(ItemType) 
	{
	case EQUIPCELLTYPE_HELM:
	case EQUIPCELLTYPE_BODY:
	case EQUIPCELLTYPE_HANDR:
	case EQUIPCELLTYPE_HANDL:
	case EQUIPCELLTYPE_NECKLACE:
	case EQUIPCELLTYPE_BELT:
	case EQUIPCELLTYPE_SHOES:
	case EQUIPCELLTYPE_FASHION:  //soke 时装
	case EQUIPCELLTYPE_AMULET:   //soke 护身符
	case EQUIPCELLTYPE_CBAMULET:	/// 翅膀
	case EQUIPCELLTYPE_XZAMULET:	/// 段位勋章
	case EQUIPCELLTYPE_SBAMULET:	/// 神兵
	case EQUIPCELLTYPE_SSAMULET:	/// 神兽
	case EQUIPCELLTYPE_SHANGFANGBAOJIAN:	//尚方宝剑
	case EQUIPCELLTYPE_ZIJINLONGPAO: 	//紫金龙袍
	case EQUIPCELLTYPE_PFAMULET:	/// 披风
	case EQUIPCELLTYPE_FABAO:	/// 法宝
	case EQUIPCELLTYPE_FABAOFJ1:	/// 法宝附件
	case EQUIPCELLTYPE_FABAOFJ2:	/// 法宝附件
	case EQUIPCELLTYPE_FABAOFJ3:	/// 法宝附件
	case EQUIPCELLTYPE_FABAOFJ4:	/// 法宝附件
	case EQUIPCELLTYPE_FABAOFJ5:	/// 法宝附件
	case EQUIPCELLTYPE_FABAOFJ6:	/// 法宝附件
	case EQUIPCELLTYPE_FABAOFJ7:	/// 法宝附件
	case EQUIPCELLTYPE_FABAOFJ8:	/// 法宝附件
	case EQUIPCELLTYPE_FABAOFJ9:	/// 法宝附件
	case EQUIPCELLTYPE_FABAOFJ10:	/// 法宝附件
	case EQUIPCELLTYPE_FABAOFJ11:	/// 法宝附件
	case EQUIPCELLTYPE_FABAOFJ12:	/// 法宝附件
	case EQUIPCELLTYPE_YUANSHEN:	/// 元神
	case EQUIPCELLTYPE_YUANSHENFJ1:	/// 元神附件
	case EQUIPCELLTYPE_YUANSHENFJ2:	/// 元神附件
	case EQUIPCELLTYPE_YUANSHENFJ3:	/// 元神附件
	case EQUIPCELLTYPE_YUANSHENFJ4:	/// 元神附件
	case EQUIPCELLTYPE_YUANSHENFJ5:	/// 元神附件
	case EQUIPCELLTYPE_YUANSHENFJ6:	/// 元神附件
	case EQUIPCELLTYPE_SEXX1:	/// 十二星宿附件
	case EQUIPCELLTYPE_SEXX2:	/// 十二星宿附件
	case EQUIPCELLTYPE_SEXX3:	/// 十二星宿附件
	case EQUIPCELLTYPE_SEXX4:	/// 十二星宿附件
	case EQUIPCELLTYPE_SEXX5:	/// 十二星宿附件
	case EQUIPCELLTYPE_SEXX6:	/// 十二星宿附件
	case EQUIPCELLTYPE_SEXX7:	/// 十二星宿附件
	case EQUIPCELLTYPE_SEXX8:	/// 十二星宿附件
	case EQUIPCELLTYPE_SEXX9:	/// 十二星宿附件
	case EQUIPCELLTYPE_SEXX10:	/// 十二星宿附件
	case EQUIPCELLTYPE_SEXX11:	/// 十二星宿附件
	case EQUIPCELLTYPE_SEXX12:	/// 十二星宿附件
		pEquipTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable(ItemType);
		if (EquipHandsArm(pSrcItem,pEquipTable) == false)
			return false;
		break;
	case EQUIPCELLTYPE_HORSESHOE:  //soke 马脚
	case EQUIPCELLTYPE_HORSEROPE:  //soke 马绳
	case EQUIPCELLTYPE_HORSESADDLE:  //soke 马鞍
	case EQUIPCELLTYPE_HORSESAFE:  //soke 马甲
	case EQUIPCELLTYPE_HORSEIRON:  //soke 马镫
	case EQUIPCELLTYPE_HORSEFASHION:  //soke 马匹圣石
		pEquipTable = GetGameGuiManager()->m_guiPetDlg->GetEquipTable(ItemType);
		if (EquipHandsArm(pSrcItem,pEquipTable) == false)
			return false;
    case EQUIPCELLTYPE_TUJIAN1:  //soke 百兽图鉴 
	case EQUIPCELLTYPE_TUJIAN2:  //soke 百兽图鉴
	case EQUIPCELLTYPE_TUJIAN3:  //soke 百兽图鉴
	case EQUIPCELLTYPE_TUJIAN4:  //soke 百兽图鉴
	case EQUIPCELLTYPE_TUJIAN5:  //soke 百兽图鉴
	case EQUIPCELLTYPE_TUJIAN6:  //soke 百兽图鉴
	case EQUIPCELLTYPE_TUJIAN7:  //soke 百兽图鉴
	case EQUIPCELLTYPE_TUJIAN8:  //soke 百兽图鉴
	case EQUIPCELLTYPE_TUJIAN9:  //soke 百兽图鉴
	case EQUIPCELLTYPE_TUJIAN10:  //soke 百兽图鉴
		pEquipTable = GetGameGuiManager()->m_pDlgEquiphandbook->GetEquipTable(ItemType);
		if (EquipHandsArm(pSrcItem,pEquipTable) == false)
			return false;

		break;
	case EQUIPCELLTYPE_BABYFJ1:  //soke 宝宝 
	case EQUIPCELLTYPE_BABYFJ2:  //soke 宝宝
	case EQUIPCELLTYPE_BABYFJ3:  //soke 宝宝
	case EQUIPCELLTYPE_BABYFJ4:  //soke 宝宝
	case EQUIPCELLTYPE_BABYFJ5:  //soke 宝宝
	case EQUIPCELLTYPE_BABYFJ6:  //soke 宝宝
		pEquipTable = GetGameGuiManager()->m_guiBaby->GetEquipTable(ItemType);
		if (EquipHandsArm(pSrcItem,pEquipTable) == false)
			return false;

		break;
	case EQUIPCELLTYPE_GLOVES:
	case EQUIPCELLTYPE_RING:
		{
			if(bLeft == 0)
			{
				pEquipTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable(ItemType,EQUIPCELLTYPE_LEFT);
			}
			else if(bLeft == 1)
			{
				pEquipTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable(ItemType,EQUIPCELLTYPE_RIGHT);
			}
			else
			{
				pEquipTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable(ItemType,EQUIPCELLTYPE_LEFT);
				if (pEquipTable == NULL)
					return false;

			pDestCell = pEquipTable->GetItemCell(0,0);
			if( pDestCell)
			{
				pEquipTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable(ItemType,EQUIPCELLTYPE_RIGHT);
				pDestCell = pEquipTable->GetItemCell(0,0);
				if (pDestCell)
				{
					switch(ItemType) 
					{
					case EQUIPCELLTYPE_GLOVES:
						{
							if (bGloveLeft)
								pEquipTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable(ItemType,EQUIPCELLTYPE_LEFT);
							else
								pEquipTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable(ItemType,EQUIPCELLTYPE_RIGHT);

							bGloveLeft = !bGloveLeft;

						}
						break;
					case EQUIPCELLTYPE_RING:
						{
							if (bRingLeft)
								pEquipTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable(ItemType,EQUIPCELLTYPE_LEFT);
							else
								pEquipTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable(ItemType,EQUIPCELLTYPE_RIGHT);

							bRingLeft = !bRingLeft;

							}
							break;
						}
					}
				}
			}
		}
		break;
	case EQUIPCELLTYPE_ADORN:
		{
			//To see if has the same type as the two
			CRoleItem* pLeftItem = GetGameGuiManager()->m_guiUserInfo->GetEquipItem( ItemType, EQUIPCELLTYPE_LEFT );
			CRoleItem* pRightItem = GetGameGuiManager()->m_guiUserInfo->GetEquipItem( ItemType, EQUIPCELLTYPE_RIGHT );
			//soke 左手和右手
			if ( pLeftItem && (pLeftItem->GetItemType() == pSrcItem->GetItemType()) )
				pEquipTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable( ItemType, EQUIPCELLTYPE_LEFT );
			else if ( pRightItem && (pRightItem->GetItemType() == pSrcItem->GetItemType()) )
				pEquipTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable( ItemType, EQUIPCELLTYPE_RIGHT );
			else
			{
				if ( pLeftItem )
					pEquipTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable( ItemType, EQUIPCELLTYPE_RIGHT );
				else
					pEquipTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable( ItemType, EQUIPCELLTYPE_LEFT );
			}
		}
		break;
	case EQUIPCELLTYPE_PACKAGE:
		{
			if (GetGameGuiManager()->m_guiItem==NULL)
				return false;

			pEquipTable = GetGameGuiManager()->m_guiItem->m_pTableLeft;
			if (pEquipTable == NULL)
				return false;

			pDestCell = pEquipTable->GetItemCell(0,0);
			if( pDestCell)
			{
				pEquipTable = GetGameGuiManager()->m_guiItem->m_pTableRight;
				pDestCell = pEquipTable->GetItemCell(0,0);
				if (pDestCell)
				{
					pEquipTable = NULL;
					GetGameGuiManager()->AddClientSystemMessage("包裹栏已满，不能装备");
					return false;
				}
			}
		}
		break;
	case EQUIPCELLTYPE_MAKE:
		{
			if (GetGameGuiManager()->m_guiMain==NULL)
				return false;

			pEquipTable = GetGameGuiManager()->m_guiMain->m_pTableLeft;
			if (pEquipTable == NULL)
				return false;

			pDestCell = pEquipTable->GetItemCell(0,0);
			if( pDestCell)
			{
				pEquipTable = GetGameGuiManager()->m_guiMain->m_pTableRight;
				pDestCell = pEquipTable->GetItemCell(0,0);
				if (pDestCell)
				{
					pEquipTable = NULL;
					GetGameGuiManager()->AddClientSystemMessage("道具栏已满，不能装备");
					return false;
				}
			}
		}
		break;
	}

	if (pEquipTable == NULL)
		return false;

	pDestCell = pEquipTable->GetItemCell(0,0);
	if( pDestCell)
	{
		if(!pDestCell->IsEnabled())
		{
			GetGameGuiManager()->AddClientSystemMessage("目标道具已经被锁定，无法替换装备！");
			return true;
		}
	}

	// 交换物品
	stObjectLocation dst;

	PlayItemSound(pSrcItem->GetObjectBase()->itmeSound);
	dst.dwLocation = pEquipTable->m_iTableType;
	dst.dwTableID = pEquipTable->m_iTableID;

	dst.x = pEquipTable->m_EquipPosition.x;
	dst.y = pEquipTable->m_EquipPosition.y;

	pSrcItem->MoveTo(dst);

	return true;
}

//添加交易物品
bool AddTradItem(CGuiTable* pTable,POINT ptDestGrid,CRoleItem* pSrcItem)
{
	if (pSrcItem->GetObject()->pos.dwLocation != OBJECTCELLTYPE_COMMON)
	{
		GetGameGuiManager()->AddClientSystemMessage("只有放在主包裹中的物品才能进行交易！");
		return true;
	}

	if (pSrcItem->IsBind())
	{
		GetGameGuiManager()->AddClientSystemMessage("已经被灵魂绑定的装备物品不能进行交易！");
		return true;
	}

   //soke 21类型箭支物品禁止交易
	if (pSrcItem->GetItemType()==ItemType_Arrow)
	{
		GetGameGuiManager()->AddClientSystemMessage("箭支不能进行交易！");
		return true;
	}

    //soke 34类型物品禁止交易
	if (pSrcItem->GetItemType()==ItemType_Quest)  
	{
		if( ClearPoint_CannotTrade(pSrcItem) ) 
			return true;
		GetGameGuiManager()->AddClientSystemMessage("任务物品不能进行交易！");
		return true;
	}

	CGuiItemCell* pCell = NULL;
	int count = 0;
	stObjectLocation dst;
	dst.dwLocation = pTable->m_iTableType;
	dst.dwTableID = pTable->m_iTableID;
	dst.x = ptDestGrid.x;
	dst.y = ptDestGrid.y;


	pSrcItem->DestoryAllVirtualCell();
	pSrcItem->GetTrueCell(pCell,count);

	pSrcItem->AddShowLocation(pCell,count,dst,false,true);


	stAddObjectTradeUserCmd cmd;
	memcpy(&(cmd.object) , pSrcItem->GetObject() , sizeof(cmd.object));
	cmd.user_id = GetScene()->GetMainCharacter()->GetID();
	cmd.x = ptDestGrid.x;
	cmd.y = ptDestGrid.y;

	SEND_USER_CMD(cmd);

	return true;
}



//添加交易物品
bool AddTradItem(CGuiFloatItem* pFloatItem,CGuiTable* pTable,POINT ptDestGrid,CRoleItem* pSrcItem)
{
	if(!pTable || !pSrcItem || !pFloatItem) return false;

	if (pSrcItem->GetItemCountInCell(pFloatItem->GetItem())!=pSrcItem->GetCount())
		return true;
	return AddTradItem(pTable,ptDestGrid,pSrcItem);
}

bool SplitItem(CGuiFloatItem* pFloatItem,CRoleItem* pSrcItem,CRoleItem* pDestItem)
{
	if(!pDestItem || !pSrcItem || !pFloatItem) return false;
	//pSrcItem->UnionTo(pDestItem);
	int maxNum = pSrcItem->GetMaxCount();
	int sliptNum = maxNum - pDestItem->GetCount();
	if(sliptNum > 0)
	{
		stObjectLocation dst = pDestItem->GetLocation();
		pSrcItem->SplitTo(dst,min(sliptNum,pSrcItem->GetItemCountInCell(pFloatItem->GetItem())) );
		// 播放物品放下的声音
		PlayItemSound(pSrcItem->GetObjectBase()->itmeSound);
	}
	return true;
}

inline void ResetLocationIfStore(stObjectLocation& dst)
{
	/*
	*仓库的格子处理起来比较复杂,仓库有3个页面,把它当作一个页面来处理，
	*第二个页面的y坐标也就是实际的y坐标加上第一个页面的高度
	*第三个页面类推
	*这里是考虑放置物品到仓库的空格子时也要进行特殊处理
	*/
	if(dst.dwLocation == OBJECTCELLTYPE_STORE)
	{
		//int index = GetStoreTableIndex(dst);
		//if(index == -1) return false;
		//CMainCharacter *pMainChar = GetScene()->GetMainCharacter();
		//if(pMainChar)
		//{
		//	//这里判断目标格子是否到期
		//	if( pMainChar->GetSavingBox()->days[index] == 0 ) 
		//	{
		//		GetGameGuiManager()->AddMessageBox("你租用的仓库到期了！");
		//		return false;
		//	}
		//}
		switch(dst.dwTableID)
		{
		case 1:	dst.y += 0;break;
		case 2: dst.y += CGuiSavingBoxDialog::csShowRowCount;break;
		case 3: dst.y += CGuiSavingBoxDialog::csShowRowCount*2;break;
		}
		dst.dwTableID = 0;
	}
}

//分裂物品
bool SplitItem(CGuiFloatItem* pFloatItem,CGuiTable* pTable,POINT ptDestGrid,CRoleItem* pSrcItem)
{
	if(!pTable || !pSrcItem || !pFloatItem) return false;
	stObjectLocation dst;
	dst.dwLocation = pTable->m_iTableType;
	dst.dwTableID = pTable->m_iTableID;
	dst.x = ptDestGrid.x;
	dst.y = ptDestGrid.y;

	int num = pSrcItem->GetItemCountInCell( pFloatItem->GetItem() );
	if( num < pSrcItem->GetCount() )
	{	
		ResetLocationIfStore(dst);
		pSrcItem->SplitTo(dst,num);
	}
	else
	{
		pSrcItem->MoveTo(dst);
	}
	// 播放物品放下的声音
	PlayItemSound(pSrcItem->GetObjectBase()->itmeSound);

	return true;
}

bool IsEnabledPack(CGuiTable* pTable)
{
	if (pTable==NULL)
		return false;

	if (pTable->m_iTableType != OBJECTCELLTYPE_PACKAGE)
		return true;

	CGuiPackage* pDlg = (CGuiPackage*)pTable->m_pDialog;
	if (pDlg==NULL)
		return false;

	CRoleItem* pItem = pDlg->GetPackItem();
	if (pItem==NULL)
		return false;

	if (pItem->GetItemDur()==0)
	{
		GetGameGuiManager()->AddServerSystemMessage("当前包裹的耐久度为0，不能再存放物品。");
		return false;
	}

	return true;
}
/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  处理移动物品事件
*/
static bool OnMoveItemEvent( CGuiTable* pTable,UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;

	CGuiFloatItem* pFloatItem;
	CGuiItemCell* pDestCell = NULL;
	CRoleItem* pSrcItem = NULL;
	CRoleItem* pDestItem = NULL;

	stPointI ptDestGrid;
	stPointI ptDestPixel = pt;

	// 计算目标格子
	if(pTable)
	{
		ptDestPixel.x -= pTable->GetX();
		ptDestPixel.y -= pTable->GetY();
		ptDestGrid = pTable->GetGridPosByPixelPos(ptDestPixel);
	}
	else
	{
		ptDestGrid = stPointI(0,0);
	}

	if( pFloatItem = GetGuiManager()->GetFloatItem() )
	{
		// 如果鼠标上有物品
		Assert(pFloatItem->GetItem());
		pSrcItem = (CRoleItem*)pFloatItem->GetItem()->m_pItemData;

		//
		//If this a special table (EQUIPCELLTYPE_ADORN)
		//We do some additional operation
		if ( pTable && (pTable->m_iTableType == OBJECTCELLTYPE_EQUIP) && (ptDestGrid.y == EQUIPCELLTYPE_ADORN) )
		{
			//To see if equiped the same type item already
			CRoleItem* pItem = NULL;
			if ( ptDestGrid.x == EQUIPCELLTYPE_LEFT )
				pItem = GetGameGuiManager()->m_guiUserInfo->GetEquipItem( EQUIPCELLTYPE_ADORN, EQUIPCELLTYPE_RIGHT );
			else
				pItem = GetGameGuiManager()->m_guiUserInfo->GetEquipItem( EQUIPCELLTYPE_ADORN, EQUIPCELLTYPE_LEFT );

			if ( pItem && (pItem != pSrcItem) && (pItem->GetItemType() == pSrcItem->GetItemType()) )
			{
				int x = (ptDestGrid.x == 0) ? EQUIPCELLTYPE_RIGHT : EQUIPCELLTYPE_LEFT;
				pTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable( EQUIPCELLTYPE_ADORN, x );

				//Reset dest about
				ptDestPixel.x -= pTable->GetX();
				ptDestPixel.y -= pTable->GetY();
				ptDestGrid = pTable->GetGridPosByPixelPos(ptDestPixel);
			}
		}		
		//
		// 计算放下的位置
		SIZE size = pFloatItem->GetItem()->GetSize();

		bool bCanPush = false;

		if(pTable && pTable->m_iTableType == OBJECTCELLTYPE_SELL)
		{
			// 卖物品

			return OnSellItemEvent(pFloatItem->GetItem());
		}

		if(pTable)
		{
			ptDestPixel.x -= size.cx * pTable->m_cellWidth/2 - pTable->m_cellWidth/2;
			ptDestPixel.y -= size.cy * pTable->m_cellHeight/2 - pTable->m_cellHeight/2;
			stPointI pt1;
			ptDestGrid = pTable->GetGridPosByPixelPos(ptDestPixel);
			stRectI rect(ptDestGrid.x,ptDestGrid.y,ptDestGrid.x + size.cx,ptDestGrid.y + size.cy);

			bCanPush = pTable->CanPush(&rect);
		}
		else
		{
			if(pSrcItem->GetMaxCount() > 1)
			{
				if(pSrcItem->GetItemCountInCell(pFloatItem->GetItem()) != pSrcItem->GetCount())
				{
					// 一部分的物品不能销毁
					return true;
				}
			}
		}
		// 客户端预判断

		if( !pTable || bCanPush )
		{
			// 如果能够放下

			Assert(pSrcItem);

			if(pTable)
			{
				stObjectLocation location  = pSrcItem->GetLocation();

				if (EquipHandsArm(pSrcItem,pTable)==false)
					return true;

				if(IsEnabledPack(pTable)==false)
					return true;


				pDestCell = pTable->GetItemCell(ptDestGrid.x,ptDestGrid.y);
				if( pDestCell )
				{
					pDestItem = (CRoleItem*)pDestCell->m_pItemData;
					Assert(pDestItem);

					//If the float item is at special table (EQUIPCELLTYPE_ADORN)
					//We do some addition operation
					if ( (pSrcItem->GetLocation().dwLocation == OBJECTCELLTYPE_EQUIP) && (pSrcItem->GetLocation().y == EQUIPCELLTYPE_ADORN) )
					{
						//To see if dest table cell is has any item can not swap
						if ( pDestItem )
						{
							int x = (pSrcItem->GetLocation().x == 0) ? EQUIPCELLTYPE_RIGHT : EQUIPCELLTYPE_LEFT;
							CRoleItem* pSpeItem = GetGameGuiManager()->m_guiUserInfo->GetEquipItem( EQUIPCELLTYPE_ADORN, x );
							if ( pSpeItem )
							{
								if ( pSpeItem->GetItemType() == pDestItem->GetItemType() )
									return false;		
							}
						}

					}

					if ((pTable->m_iTableType == OBJECTCELLTYPE_TRADE) )
						return true;

					if( pDestItem == pSrcItem )
					{
						// 相同位置的物品
						pDestItem->OnRemoveCell(pFloatItem->GetItem());
						pFloatItem->DestroyCell();

						if (pTable->m_iTableType == OBJECTCELLTYPE_RECAST)
						{
							if (GetGameGuiManager()->m_pDlgEquipRecast)
							{
								GetGameGuiManager()->m_pDlgEquipRecast->OnPutFocusItem(true);
							}
						}
						if (pTable->m_iTableType == OBJECTCELLTYPE_ANNEX)
						{
							if (GetGameGuiManager()->m_pDlgEquipAnnexpack)
							{
								GetGameGuiManager()->m_pDlgEquipAnnexpack->OnPutFocusItem(true);
							}
						}
						if (pTable->m_iTableType == OBJECTCELLTYPE_HANDBOOK) //soke 百兽图鉴助手
						{
							if (GetGameGuiManager()->m_pDlgEquipbookRecast)
							{
								GetGameGuiManager()->m_pDlgEquipbookRecast->OnPutFocusItembook(true);
							}	
						}
						if (pTable->m_iTableType == OBJECTCELLTYPE_FABAO) //soke 法宝助手
						{
							if (GetGameGuiManager()->m_pDlgEquipFabaoRecast)
							{
								GetGameGuiManager()->m_pDlgEquipFabaoRecast->OnPutFocusItembook(true);
							}	
						}
						if (pTable->m_iTableType == OBJECTCELLTYPE_YUANSHEN) //soke 元神助手
						{
							if (GetGameGuiManager()->m_pDlgEquipYuanshenRecast)
							{
								GetGameGuiManager()->m_pDlgEquipYuanshenRecast->OnPutFocusItembook(true);
							}	
						}

						return true;
					}

					// 灰掉的物品不能襙作
					if(!pDestCell->IsEnabled())return true;

					// 修理宝石修理
					if ((pSrcItem->GetObjectID()==870)&&(pDestItem->GetObject()->maxdur<pDestItem->GetObjectBase()->dwMaxDur)&&(pTable->m_iTableType==OBJECTCELLTYPE_COMMON))
					{
						stRepairObjectNpcTradeUserCmd  cmd;

						cmd.gem_id = pSrcItem->GetThisID();
						cmd.id = pDestItem->GetThisID();
						SEND_USER_CMD(cmd);
						return true;
					}

					if( ( pDestItem->GetObjectID() == pSrcItem->GetObjectID() )
						&& (pDestItem->GetObject()->upgrade == pSrcItem->GetObject()->upgrade)
						&& pSrcItem->GetMaxCount() > 1)
					{
						// 分裂物品
						return SplitItem(pFloatItem,pSrcItem,pDestItem);
					}

					if(pSrcItem->GetMaxCount() > 1)
					{
						// 如果是可以放多个的物品，目标类型不一样，鼠标上不是拿起所有数量物品，则不能放。
						if(pSrcItem->GetItemCountInCell(pFloatItem->GetItem()) != pSrcItem->GetCount())
							return true;
					}
				}
				else
				{
					if (pTable->m_iTableType==OBJECTCELLTYPE_MAKE)
					{
						stObjectLocation dst;

						dst.dwLocation = pTable->m_iTableType;
						dst.dwTableID = pTable->m_iTableID;
						dst.x = ptDestGrid.x;
						dst.y = ptDestGrid.y;

					}


					// 目标上没物品
					if( pSrcItem->GetMaxCount() > 1 )
					{						
						//交易的格子
						if(pTable->m_iTableType == OBJECTCELLTYPE_TRADE)
						{
							return AddTradItem(pFloatItem,pTable,ptDestGrid,pSrcItem);
						}
						else
						{
							//分裂物品(放置在一个空位置)
							return SplitItem(pFloatItem,pTable,ptDestGrid,pSrcItem);
						}
					}
				}

				if(pTable->m_iTableType == OBJECTCELLTYPE_EQUIP)
				{
					// 目标是装备栏
					if( pSrcItem->IsMoppet() )
					{
						// 是纸娃娃
						if(!IsMoppet(pTable->m_EquipPosition.y))
							return true;
						ptDestGrid.x = 0;
						ptDestGrid.y = pSrcItem->GetEquipType();
					}
					else
					{
						// 其它装备
						if( pSrcItem->GetEquipType() != pTable->m_EquipPosition.y )
							return true;
						ptDestGrid = pTable->m_EquipPosition;
					}
				}

				if(pTable->m_iTableType == OBJECTCELLTYPE_TRADE)
				{
					return AddTradItem(pTable,ptDestGrid,pSrcItem);
				}

				if(pTable->m_iTableType == OBJECTCELLTYPE_OTHERTRADE)
				{
					return true;
				}
			}

			// 交换物品
			stObjectLocation dst;
			if(pTable)
			{
				// 播放物品放下的声音
				PlayItemSound(pSrcItem->GetObjectBase()->itmeSound);
				dst.dwLocation = pTable->m_iTableType;
				dst.dwTableID = pTable->m_iTableID;
			}
			else
			{
				dst.dwLocation = OBJECTCELLTYPE_NONE;
				dst.dwTableID = 0;
			}

			dst.x = ptDestGrid.x;
			dst.y = ptDestGrid.y;

			//see if we want to exchange item
			if ( pTable )
			{
				pSrcItem->MoveTo(dst);
			}
			else
			{
				if ( GetKeyState( VK_SHIFT ) >= 0 )
				{
					pSrcItem->MoveTo(dst);
				}
			}
			return true;
		}
	}
	else 
	{
		// 拿起一个物品
		if(pTable->IsValidGrid(ptDestGrid) && (pDestCell = pTable->GetItemCell(ptDestGrid.x,ptDestGrid.y)) )
		{

			// 灰掉的物品不能襙作
			if(!pDestCell->IsEnabled())return true;

			if(pTable && pTable->m_iTableType == OBJECTCELLTYPE_SELL)
			{
				return OnBuyItemEvent(pDestCell,uMsg == WM_LBUTTONDOWN);
			}

			//对方的装备位置不能拿起来
			if(pTable && pTable->m_iTableType == OBJECTCELLTYPE_EQUIPSHOW)
			{
				return true;
			}

			//时装魔盒位置不能拿起来
			if(pTable && pTable->m_iTableType == OBJECTCELLTYPE_EQUIP && pTable->m_EquipPosition.y ==  EQUIPCELLTYPE_FASHION)
			{
				return true;
			}

			pSrcItem = (CRoleItem*)pDestCell->m_pItemData;
			Assert(pSrcItem);

			//有物品的包裹不能拿起来
			if ((pTable->m_iTableType == OBJECTCELLTYPE_EQUIP)&&(pTable->m_EquipPosition.y == EQUIPCELLTYPE_PACKAGE))
			{
				stObjectLocation  pos;
				pos.dwLocation = OBJECTCELLTYPE_PACKAGE;
				pos.dwTableID = pSrcItem->GetThisID();
				CGuiTable* pPackageTable = GetItemTable(pos);
				if (pPackageTable)
				{	
					if (pPackageTable->m_listItem.size()>0)
						return true;
					else
						pPackageTable->m_pDialog->SetVisible(false);
				}
			}

			//有物品的合成包裹不能拿起来
			if ((pTable->m_iTableType == OBJECTCELLTYPE_EQUIP)&&(pTable->m_EquipPosition.y == EQUIPCELLTYPE_MAKE))
			{
				stObjectLocation  pos;
				pos.dwLocation = OBJECTCELLTYPE_MAKE;
				pos.dwTableID = pSrcItem->GetThisID();
				pos.y = MAKECELLTYPE_EQUIP;
				CGuiTable* pPackageTable = GetItemTable(pos);
				if (pPackageTable)
				{	
					if (((CGuiMakeTo*)pPackageTable->m_pDialog)->GetItemCount()>0)
						return true;
					else
						pPackageTable->m_pDialog->SetVisible(false);
				}
			}


			stObjectLocation dst;
			dst.dwLocation = OBJECTCELLTYPE_MOUSE;
			dst.dwTableID = 0;
			dst.x = 0;
			dst.y = 0;
			// 显示到鼠标上

			int nCount = pSrcItem->GetCount();
			if(pTable->m_iTableType == OBJECTCELLTYPE_TRADE)
			{
				//

				int count;
				nCount = pSrcItem->GetItemCountInCell(pDestCell);
				pSrcItem->GetTrueCell(pDestCell,count);
				pSrcItem->DestoryAllVirtualCell();
				Assert(pDestCell);

				stRemoveObjectTradeUserCmd cmd;
				cmd.id = pSrcItem->GetThisID();
				cmd.user = GetScene()->GetMainCharacter()->GetID();

				SEND_USER_CMD(cmd);


			}
			pSrcItem->AddShowLocation(pDestCell,nCount,dst,pSrcItem->GetCount() == nCount && pTable->m_iTableType != OBJECTCELLTYPE_TRADE);

			if (pTable->m_iTableType == OBJECTCELLTYPE_RECAST)
			{
				if (GetGameGuiManager()->m_pDlgEquipRecast)
				{
					GetGameGuiManager()->m_pDlgEquipRecast->OnPutFocusItem(false);
				}
			}
			if (pTable->m_iTableType == OBJECTCELLTYPE_ANNEX)
			{
				if (GetGameGuiManager()->m_pDlgEquipAnnexpack)
				{
					GetGameGuiManager()->m_pDlgEquipAnnexpack->OnPutFocusItem(false);
				}
			}
			if (pTable->m_iTableType == OBJECTCELLTYPE_HANDBOOK)
			{			
				if (GetGameGuiManager()->m_pDlgEquipbookRecast)
				{
					GetGameGuiManager()->m_pDlgEquipbookRecast->OnPutFocusItembook(false);
				}
			}
			if (pTable->m_iTableType == OBJECTCELLTYPE_FABAO)
			{			
				if (GetGameGuiManager()->m_pDlgEquipFabaoRecast)
				{
					GetGameGuiManager()->m_pDlgEquipFabaoRecast->OnPutFocusItembook(false);
				}
			}
				if (pTable->m_iTableType == OBJECTCELLTYPE_YUANSHEN)
			{			
				if (GetGameGuiManager()->m_pDlgEquipYuanshenRecast)
				{
					GetGameGuiManager()->m_pDlgEquipYuanshenRecast->OnPutFocusItembook(false);
				}
			}
			// 播放物品拿起的声音
			PlayItemSound(g_nItemTakeUpSound);
			return true;
		}
	}
	return false;

	FUNCTION_END;
}

inline DWORD GetFireIdFromItemId(const DWORD& id)
{
	static DWORD g_FireItem[][2] = {
		{1709, 618},
		{1710, 625},
		{1711, 624},
		{1712, 619},
		{1713, 620},
		{1714, 621},
		{1715, 622},
		{1716, 623},
		{1717, 614},
		{1718, 617},
		{1719, 616},
		{1720, 615},
		{1721, 637},
		{1722, 637},
		{1723, 0},
	};

	for(int i=0;i<count_of(g_FireItem);i++ )
	{
		if ( g_FireItem[i][0] == id )
			return g_FireItem[i][1];
	}

	return -1;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  处理使用物品事件
*/
static bool OnUseItemEvent(CGuiTable* pTable,UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam)
{
	FUNCTION_BEGIN;
	Engine_WriteLogF("OnUseItemEvent被调用\n");

	CGuiFloatItem* pFloatItem;
	CGuiItemCell* pDestCell = NULL;
	CRoleItem* pSrcItem = NULL;
	CRoleItem* pDestItem = NULL;
	if(pTable)
	{
		TRACE("Table Type:%d\n",pTable->m_iTableType);
	}

	stPointI ptDestGrid;
	stPointI ptDestPixel = pt;

	// 计算目标格子
	ptDestPixel.x -= pTable->GetX();
	ptDestPixel.y -= pTable->GetY();
	ptDestGrid = pTable->GetGridPosByPixelPos(ptDestPixel);
	
	if( pFloatItem = GetGuiManager()->GetFloatItem() )
	{

	}
	else
	{
		
		if(pTable && pTable->m_iTableType == OBJECTCELLTYPE_SELL)
			return false;

		if(pTable->IsValidGrid(ptDestGrid) && (pDestCell = pTable->GetItemCell(ptDestGrid.x,ptDestGrid.y)) )
		{
			if(!pDestCell->IsEnabled())
				return false;

			pSrcItem = (CRoleItem*)pDestCell->m_pItemData;
			Assert(pSrcItem);

			// //soke 检测宝箱NPC类物品相关的道具被使用消息
            // if((pSrcItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON) && ( pSrcItem->GetItemType() == ItemType_Gift) )
			// {
			// 	// 使用物品
			// 	Engine_WriteLogF("使用召唤NPC类道具ID:%u——物品名字:%s\n", pSrcItem->GetObjectBase()->dwID, pSrcItem->GetObjectBase()->strName);
			// 	stUseObjectPropertyUserCmd cmd;
			// 	cmd.qwThisID = pSrcItem->GetThisID();
			// 	cmd.dwNumber = 1;
			// 	SEND_USER_CMD( cmd );
			
			// 	return true;
			// }

			if ((pSrcItem->GetObjectBase()->dwType==ItemType_Pack)&&(pTable->m_iTableType==OBJECTCELLTYPE_EQUIP))
			{
				if (pTable->m_EquipPosition.y == EQUIPCELLTYPE_PACKAGE)
				{
					switch(pTable->m_EquipPosition.x) 
					{
					case EQUIPCELLTYPE_LEFT:
						GetGameGuiManager()->AddPackage1(pSrcItem);

						break;
					case EQUIPCELLTYPE_RIGHT:
						GetGameGuiManager()->AddPackage2(pSrcItem);

						break;
					}
				}
				return true;
			}


			if ((pSrcItem->GetObjectBase()->dwID==689)&&(pTable->m_iTableType==OBJECTCELLTYPE_EQUIP))
			{
				if (pTable->m_EquipPosition.y == EQUIPCELLTYPE_MAKE)
				{
					switch(pTable->m_EquipPosition.x)
					{
					case EQUIPCELLTYPE_LEFT:
						GetGameGuiManager()->AddMakeTo1(pSrcItem);
						break;
					case EQUIPCELLTYPE_RIGHT:
						GetGameGuiManager()->AddMakeTo2(pSrcItem);
						break;
					}
				}
				return true;
			}

			//缩骨丹
			if(pSrcItem->GetObjectBase()->dwID==45678)
			{
				if(!GetScene()->GetMainCharacter()->bIsSuogu())
				{
					GetScene()->GetMainCharacter()->OnSelectSkill( 4000 );
				}
				
				stBikeSkillCmd cmd;
				cmd.type = 99;
				cmd.itemID = pSrcItem->GetThisID();
				SEND_USER_CMD(cmd);
				return true;
				
			}

			
			//自行车道具使用
			if(pSrcItem->GetObjectBase()->dwID==944)
			{
				//先获取周围全部玩家
				std::vector< CCharacter*, std::stack_allocator<CCharacter*> > aCharacter;
				GetScene()->GetObjectOrderByDistance(aCharacter,type2type_t<CCharacter>());

				stBikeSkillCmd cmd;
			
				for(int i=0;i<aCharacter.size();i++)
				{

					if(i>=9)
					{
						break;
					}
					CCharacter* pChar = aCharacter[i];
					cmd.playerid[i] = pChar->GetID();
				}
				cmd.type = 1;
				cmd.itemID = pSrcItem->GetThisID();
				SEND_USER_CMD(cmd);
				return true;
			}
			if(pSrcItem->GetObjectBase()->dwID==945)
			{
				//先获取周围全部玩家
				std::vector< CCharacter*, std::stack_allocator<CCharacter*> > aCharacter;
				GetScene()->GetObjectOrderByDistance(aCharacter,type2type_t<CCharacter>());

				stBikeSkillCmd cmd;
			
				for(int i=0;i<aCharacter.size();i++)
				{

					if(i>=9)
					{
						break;
					}
					CCharacter* pChar = aCharacter[i];
					cmd.playerid[i] = pChar->GetID();
				}
				cmd.type = 2;
				cmd.itemID = pSrcItem->GetThisID();
				SEND_USER_CMD(cmd);
				return true;
			}
			if(pSrcItem->GetObjectBase()->dwID==946)
			{
				//先获取周围全部玩家
				std::vector< CCharacter*, std::stack_allocator<CCharacter*> > aCharacter;
				GetScene()->GetObjectOrderByDistance(aCharacter,type2type_t<CCharacter>());

				stBikeSkillCmd cmd;
			
				for(int i=0;i<aCharacter.size();i++)
				{

					if(i>=9)
					{
						break;
					}
					CCharacter* pChar = aCharacter[i];
					cmd.playerid[i] = pChar->GetID();
				}
				cmd.type = 3;
				cmd.itemID = pSrcItem->GetThisID();
				SEND_USER_CMD(cmd);
				return true;
				
			}
			if(pSrcItem->GetObjectBase()->dwID==947)
			{
				//先获取周围全部玩家
				std::vector< CCharacter*, std::stack_allocator<CCharacter*> > aCharacter;
				GetScene()->GetObjectOrderByDistance(aCharacter,type2type_t<CCharacter>());

				stBikeSkillCmd cmd;
			
				for(int i=0;i<aCharacter.size();i++)
				{

					if(i>=9)
					{
						break;
					}
					CCharacter* pChar = aCharacter[i];
					cmd.playerid[i] = pChar->GetID();
				}
				cmd.type = 4;
				cmd.itemID = pSrcItem->GetThisID();
				SEND_USER_CMD(cmd);
				return true;
			}
			if(pSrcItem->GetObjectBase()->dwID==948)
			{
				//先获取周围全部玩家
				std::vector< CCharacter*, std::stack_allocator<CCharacter*> > aCharacter;
				GetScene()->GetObjectOrderByDistance(aCharacter,type2type_t<CCharacter>());

				stBikeSkillCmd cmd;
			
				for(int i=0;i<aCharacter.size();i++)
				{

					if(i>=9)
					{
						break;
					}
					CCharacter* pChar = aCharacter[i];
					cmd.playerid[i] = pChar->GetID();
				}
				cmd.type = 5;
				cmd.itemID = pSrcItem->GetThisID();
				SEND_USER_CMD(cmd);
				return true;
			}
			if(pSrcItem->GetObjectBase()->dwID==949)
			{
				//先获取周围全部玩家
				std::vector< CCharacter*, std::stack_allocator<CCharacter*> > aCharacter;
				GetScene()->GetObjectOrderByDistance(aCharacter,type2type_t<CCharacter>());

				stBikeSkillCmd cmd;
			
				for(int i=0;i<aCharacter.size();i++)
				{

					if(i>=9)
					{
						break;
					}
					CCharacter* pChar = aCharacter[i];
					cmd.playerid[i] = pChar->GetID();
				}
				cmd.type = 6;
				cmd.itemID = pSrcItem->GetThisID();
				SEND_USER_CMD(cmd);
				return true;
			}




			// use clearpint 
			if( OnUseClearPointItem(pSrcItem)) 
			{
				
				return true;
			}
				

			//Christ. change face  item
			if ( (pSrcItem->GetObjectBase()->dwID == 1700) && 0 )		//Not use this method
			{
				if ( GetScene()->GetMainCharacter()->IsChangeFace()
					|| GetScene()->GetMainCharacter()->IsCombinationState() )
				{
					return true;
				}
				//First set change face state
				set_state( GetScene()->GetMainCharacter()->GetState(), USTATE_CHANGEFACE );
				GetScene()->GetMainCharacter()->ResetState();
				//Then send change face cmd
				stChangeFaceMapScreenUserCmd cmd;
				cmd.dwUserTempID = GetScene()->GetMainCharacter()->GetID();
				if ( GetScene()->GetMainCharacter()->GetSex() == eSex_Male )
					cmd.dwChangeFaceID = 44031;
				else
					cmd.dwChangeFaceID = 44032;
				SEND_USER_CMD( cmd );

				//Delete this item
				stObjectLocation dst;
				dst.dwLocation = OBJECTCELLTYPE_NONE;
				//msg
				stSwapObjectPropertyUserCmd cmdDel;
				cmdDel.qwThisID = pSrcItem->GetThisID();
				cmdDel.dst = dst;
				SEND_USER_CMD(cmdDel);

				return true;
			}
			//soke 检测经验被率相关的道具被使用消息
            if((pSrcItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON) && ( (pSrcItem->GetItemType() == ItemType_DoubleExpZ)
				 || (pSrcItem->GetItemType() == ItemType_DoubleExpC)||(pSrcItem->GetItemType() == ItemType_DoubleExpG)) )
			{
				// 使用物品
				Engine_WriteLogF("使用经验倍率相关道具ID:%u——物品名字:%s\n", pSrcItem->GetObjectBase()->dwID, pSrcItem->GetObjectBase()->strName);
				stUseObjectPropertyUserCmd cmd;
				cmd.qwThisID = pSrcItem->GetThisID();
				cmd.dwNumber = 1;
				SEND_USER_CMD( cmd );
				return true;
			}


			
			// 检测73类物品相关的道具被使用消息
            if((pSrcItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON) && ( pSrcItem->GetItemType() == ItemType_Tianxia || pSrcItem->GetItemType() == ItemType_Add_Exploit) )
			{
				// 使用物品
				Engine_WriteLogF("使用73/128类相关道具ID:%u——物品名字:%s\n", pSrcItem->GetObjectBase()->dwID, pSrcItem->GetObjectBase()->strName);
				stUseObjectPropertyUserCmd cmd;
				cmd.qwThisID = pSrcItem->GetThisID();
				cmd.dwNumber = 1;
				SEND_USER_CMD( cmd );
				return true;
			}

			
			//soke 检测幸运丹物品相关的道具被使用消息
            if((pSrcItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON) && ( pSrcItem->GetItemType() == ItemType_LuncyDan) )
			{
				// 使用物品
				Engine_WriteLogF("使用跳舞双倍经验倍率相关道具ID:%u——物品名字:%s\n", pSrcItem->GetObjectBase()->dwID, pSrcItem->GetObjectBase()->strName);
				stUseObjectPropertyUserCmd cmd;
				cmd.qwThisID = pSrcItem->GetThisID();
				cmd.dwNumber = 1;
				SEND_USER_CMD( cmd );
				return true;
			}
			//soke 检测合成魂魄石物品相关的道具被使用消息
            if((pSrcItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON) && ( pSrcItem->GetItemType() == ItemType_HzStone) )
			{
				// 使用物品
				Engine_WriteLogF("使用合成魂魄石相关道具ID:%u——物品名字:%s\n", pSrcItem->GetObjectBase()->dwID, pSrcItem->GetObjectBase()->strName);
				stUseObjectPropertyUserCmd cmd;
				cmd.qwThisID = pSrcItem->GetThisID();
				cmd.dwNumber = 1;
				SEND_USER_CMD( cmd );
				return true;
			}

			//密银之匙
            if((pSrcItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON) && ( pSrcItem->GetObjectBase()->dwID==963 || pSrcItem->GetObjectBase()->dwID==964) )
			{
				// 使用物品
				//Engine_WriteLogF("使用密银之匙ID:%u——物品名字:%s\n", pSrcItem->GetObjectBase()->dwID, pSrcItem->GetObjectBase()->strName);
				if(!GetGameGuiManager()->m_guiCowrybox)
				{
					stSetCowBoxKeyCmd cmd;
					cmd.Key_id = pSrcItem->GetObjectBase()->dwID;
					cmd.qwThisID = pSrcItem->GetThisID();
					cmd.isContinue = false;
					SEND_USER_CMD( cmd );
				}else
					GetGameGuiManager()->AddClientSystemMessage( "不能同时开启两个宝箱！"); 
				
				return true;
			}
			
			//Use firework item
			if ( (pSrcItem->GetItemType() == 63) && (pSrcItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON) )
			{
				DWORD dwFireID = GetFireIdFromItemId( pSrcItem->GetObjectID() );
				if ( dwFireID != -1 )
				{
					static DWORD tLastTimeSendCMD = xtimeGetTime() - 60000;

					//We control send msg rate
					if ( xtimeGetTime()-tLastTimeSendCMD < 5000 )
						return true;

					if ( dwFireID == 0 )		//We pre play a fire work
					{
						//We play Rose Rose I LOVE U
						if ( GetParticleSysMgr() )
						{
							//First we play a firework
							//And let Rose Rose IOU be his child
							D3DXVECTOR3 vEffect;
							vEffect.x = GetDevice()->GetWidth() / 2;
							vEffect.y = GetDevice()->GetHeight() / 3;
							vEffect.z = 0;

							float fStartTime = 0.0f;

							GetParticleSysMgr()->LoadOneParticleSystem( "Spring\\SpringFireWork0", vEffect );
							CParticleSystem* pSys = GetParticleSysMgr()->GetActiveParticleSystem();
							if ( pSys )
							{
								pSys->UseCharacterOffset( false );

								//Set sys life						
								pSys->Play( pSys->GetSysLife() );
								pSys->SetLoopPlay( false );

								fStartTime = pSys->GetSuppressTimeEnd();
							}
							//Load	RoseRoseIOU
							vEffect.y = 0;
							GetParticleSysMgr()->LoadOneParticleSystem( "Spring\\RoseRoseIOU", vEffect );
							pSys = GetParticleSysMgr()->GetActiveParticleSystem();
							if ( pSys )
							{
								pSys->UseCharacterOffset( false );

								//Set sys life						
								pSys->Play( 20 );
								pSys->SetLoopPlay( false );

								pSys->SetSuppressTimeStart( 0.0f );
								pSys->SetSuppressTimeEnd( fStartTime );
							}
						}
					}

					//Send msg to server
					{
						//Send action fire cmd
						stFireWorkUserCmd cmd;
						cmd.qwObjectTempID = pSrcItem->GetThisID();
						cmd.dwUserID = GetScene()->GetMainCharacter()->GetID();
						cmd.byType = MAPDATATYPE_USER;
						cmd.dwFireID = GetFireIdFromItemId( pSrcItem->GetObjectID() );
						SEND_USER_CMD( cmd );
					}

					tLastTimeSendCMD = xtimeGetTime();
				} 

				return true;
			}

			if(pSrcItem->IsCanUse()	|| (pSrcItem->GetItemType() == ItemType_BMW) )
			{
				// 使用物品
				stUseObjectPropertyUserCmd cmd;
				CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
				CALL_USEITEM_P(pMainRole,pSrcItem->GetThisID());	
				return true;
			}

			if ( ( (pSrcItem->GetObjectBase()->dwType == ItemType_SkillUp) || (pSrcItem->GetObjectBase()->dwType == ItemType_ZhenSkillUp) )
				&& (pTable->m_iTableType == OBJECTCELLTYPE_COMMON) )
			{
				DWORD skillID = GetSkillIdFromBook( pSrcItem->GetObjectID() );
				if ( skillID == -1 )
					return false;

				SkillBase_t* pSkill = GetSkillBase( skillID );
				if ( NULL == pSkill )
					return false;

				CRoleSkill* pRoleSkill = NULL;				
				if ( pSkill->dwSerialType != SKILL_SERIAL_SPECIAL )
				{
					pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( skillID );

					if ( pRoleSkill )
					{
						char szMsg[256];
						sprintf( szMsg, "你已经学习了%s!", pRoleSkill->GetName() );
						GetGameGuiManager()->AddClientSystemMessage( szMsg );
						//GetGameGuiManager()->AddMessageBox( szMsg );
					}
					else
					{   //soke 技能书右键使用判断职业（阵法除外）
						if ( GetScene()->GetMainCharacter()->IsCanStudySkill( pSkill, 1, true ) )
						{
							if( GetScene()->GetMainCharacter()->GetProperty()->changejob !=  pSkill->dwSerialType && GetScene()->GetMainCharacter()->GetProperty()->changejob !=9 && pSkill->dwSerialType !=9)
						    {
							   if(GetScene()->GetMainCharacter()->GetProperty()->changejob==0)
							   {
							      GetGameGuiManager()->AddClientSystemMessage( "你没有选择职业，不能学习职业技能！"); 
							   }
							   else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==1)
							   {
							      GetGameGuiManager()->AddClientSystemMessage( "你的技能职业是：武术，不能学习其它系职业技能！"); 
							   }
							   else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==2)
							   {
							      GetGameGuiManager()->AddClientSystemMessage( "你的技能职业是：射术，不能学习其它系职业技能！"); 
							   }
							   else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==3)
							   {
							       GetGameGuiManager()->AddClientSystemMessage( "你的技能职业是：法术，不能学习其它系职业技能！"); 
							   }
							   else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==4)
							   {
							      GetGameGuiManager()->AddClientSystemMessage( "你的技能职业是：召唤，不能学习其它系职业技能！"); 
							   }
							   else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==5)
							   {
							      GetGameGuiManager()->AddClientSystemMessage( "你的技能职业是：仙术，不能学习其它系职业技能！"); 
							   }
							   else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==6)
							   {
								   GetGameGuiManager()->AddClientSystemMessage( "你的技能职业是：刺客，不能学习其它系职业技能！"); 
							   }
							   else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==7)
							   {
								   GetGameGuiManager()->AddClientSystemMessage( "你的技能职业是：卫士，不能学习其它系职业技能！"); 
							   }
							   else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==8)
							   {
								   GetGameGuiManager()->AddClientSystemMessage( "你的技能职业是：枪系，不能学习其它系职业技能！"); 
							   }
							   return false;
							}
							else
							{
							   char szMsg[256];
							   sprintf( szMsg, "确定要学习%s吗？", pSkill->pstrName );
							   GetGameGuiManager()->AddMessageBox( szMsg, eAction_SkillUp, pSkill, "确定|1|Enter|取消|2|Esc" );
							}
						}
						else
						{
							GetGameGuiManager()->AddClientSystemMessage( "条件不足,不能学习!" );
							//GetGameGuiManager()->AddMessageBox( "条件不足,不能学习!", eAction_SkillUpFail );
						}
					}		
				}
				else
				{
					pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( skillID );
					DWORD level = GetSkillLevelFromBook( pSrcItem->GetObjectID() );
					if ( level == -1 )
						return false;

					if ( pRoleSkill && (pRoleSkill->GetLevel() == level) )
					{
						char szMsg[256];
						sprintf( szMsg, "你已经学习了%s!", pRoleSkill->GetName() );
						GetGameGuiManager()->AddClientSystemMessage( szMsg );
						//GetGameGuiManager()->AddMessageBox( szMsg );
					}
					else
					{
						SkillLevelBase_t* pSkillLevel = GetSkillLevelBase( skillID, level );
						if ( pSkillLevel && GetGameGuiManager()->m_guiSkill->IsCanStudySpecialSkill((enmSkillSpecialType)pSkillLevel->dwTreeType) )	//&& GetScene()->GetMainCharacter()->IsCanStudySkill( pSkill, 1)
						{
							if ( (pSkillLevel->dwNeedSkill3Level > GetScene()->GetMainCharacter()->GetLevel())
								|| (pSkillLevel->dwNeedSkill3Level >= OPEN_SKILL_MAX_LEVEL ) )
							{
								GetGameGuiManager()->AddClientSystemMessage( "条件不足,不能学习!" );
								//GetGameGuiManager()->AddMessageBox( "条件不足,不能学习!", eAction_SkillUpFail );								
							}
							else
							{
								char szMsg[256];
								sprintf( szMsg, "确定要学习%s吗？", pSkillLevel->pstrName );
								GetGameGuiManager()->AddMessageBox( szMsg, eAction_SkillUp, pSkill, "确定|1|Enter|取消|2|Esc" );
							}
						}						
					}
				}

				return true;
			}
		}
	}
	return false;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pTable : 描述
* \param pt : 描述
* \return 返回值的描述
*/
bool AddItemToExchange( CGuiTable* pTable, POINT pt )
{
	FUNCTION_BEGIN;

	CRoleItem* pSrcItem = GetSelectItem(pTable,pt);

	if(pTable && pTable->m_iTableType == OBJECTCELLTYPE_SELL)
	{
		return false;
	}
	if( pSrcItem )
	{
		CGuiEditBox* pEdit = CGuiEditBox::GetLastFocusEditBox();
		//Add cur item to main edit box
		if (  pEdit && (pEdit->GetStyle() & CGuiEditBox::GUI_ES_MULTIMEDIA) )
		{			
			pEdit->RequestFocus();
			//Split item name
			char* pName;
			char namePartOne[MAX_NAMESIZE];

			pName = strpbrk( pSrcItem->GetObjectName(), "0" );
			if ( pName )
				sprintf( namePartOne, ++pName );
			else
				sprintf( namePartOne, pSrcItem->GetObjectName() );
			//End split item name
			return pEdit->AddOneMedia( pSrcItem->GetThisID(), namePartOne );
		}
	}
	return false;

	FUNCTION_END;
}

/**
* \brief 判断目标物品类型是否是17,19,25,26中的一种
* 
* 在ItemTableEvent函数中作为触发"快捷使用物品"的判断条件
* 
* \param pTable : 目标格子
* \param pt : 目标点
* \return bool
*/
inline bool IsAccelKeyObject(CGuiTable* pTable,POINT pt)
{
	DWORD kinds[5] = {17,19,25,26};
	CRoleItem* pSrcItem = GetSelectItem(pTable,pt);
	if(NULL == pSrcItem)
	{
		//Assert(0);
		return false;
	}
	for(int i = 0;i < 4;i ++)
	{
		if(kinds[i] == pSrcItem->GetItemType())
		{
			return true;
		}
	}

	return false;
}

bool MoveItem2MyShop(CRoleItem* pSrcItem)
{
	if(NULL == pSrcItem)
	{
		return true;
	}
	if( pSrcItem->IsBind() ) // 不可丢弃类物品主要有灵魂绑定的物品
	{
		GetGameGuiManager()->AddClientSystemMessage("绑定的物品不能出售！");
		return true;
	}	
	if ( pSrcItem->GetItemType()==ItemType_Quest )
	{
		if( ClearPoint_CannotMail(pSrcItem) )
			return true;
		GetGameGuiManager()->AddClientSystemMessage("任务物品不能出售！");
		return true;
	}

	if( GetGameGuiManager()->m_guiShop )
	{
		stPointI pt(0,0);
		bool bRes = GetGameGuiManager()->m_guiShop->bItemCanput(pSrcItem,pt);
		if( bRes )
		{
			//设置出售物品价格
			char szTemp[MAX_PATH];
			_snprintf( szTemp, sizeof( szTemp ), "请输入总价 %s  ", pSrcItem->GetObjectName() );
			szTemp[MAX_PATH - 1] = 0;
			CGuiMoneyInputBox* pInputBox = GetGameGuiManager()->AddMoneyInputBox(szTemp,eAction_EnterShopSalePrice,pSrcItem);
			if( pInputBox )
			{
				pInputBox->SetDefaultMoney( pSrcItem->GetPrice(ITEM_PRICE_SHOP) );
			}
		}
		return true;
	}

	return true;
}

//从自己的摊位删除物品
bool DeleteItemFromShop(CRoleItem* pSrcItem)
{
	if(NULL == pSrcItem)
	{
		return true;
	}
	pSrcItem->OnRemove();
	delete pSrcItem;
	return true;
}

//从别人的摊位购买物品
bool BuyItemFromOthersShop(CRoleItem* pItem)
{
	if(NULL == pItem)
	{
		return true;
	}

	int price = pItem->GetPrice(ITEM_PRICE_SHOP);
	if( GetScene()->GetMainCharacter()->GetMoney() >= price )
	{
		char szTemp[MAX_PATH];
		_snprintf( szTemp, sizeof( szTemp ), "购买 %s(%d) 需要 %s ,确定要买吗?", pItem->GetObjectName(),pItem->GetCount(),Money2String(price) );
		szTemp[MAX_PATH - 1] = 0;
		GetGameGuiManager()->AddMessageBox(szTemp,eAction_BuyShopItem,pItem,c_szMBOKCancel);
		return true;
	}
	else
	{
		GetGameGuiManager()->AddClientSystemMessage("对不起,你的钱不够");
		return true;
	}
	return false;
}

DWORD GetItemTotalPrice(t_Object& obj)
{
	CRoleItem* pItem = GetItemByObject(obj);
	if( pItem )
	{
		int price = pItem->GetPrice(ITEM_PRICE_SHOP);
		if( 0 !=  price)
		{	
			return price;
		}
	}
	return obj.dwNum * obj.price;
}

bool OnMoveItemOnMailTable(CGuiTable* pTable)
{
	if( !pTable || pTable->m_iTableType != OBJECTCELLTYPE_MAIL )
	{
		return true;
	}
	if( (pTable->m_iTableID == 0) || (pTable->m_iTableID == 313) )	//发件箱 || 拍卖界面
	{
		CGuiFloatItem* pFloatItem = GetGuiManager()->GetFloatItem();
		if ( pFloatItem )
		{
			CRoleItem* pItem = (CRoleItem*)pFloatItem->GetItem()->m_pItemData;
			if ( pItem )
			{
				if( pItem->IsBind() ) // 不可丢弃类物品主要有灵魂绑定的物品
				{
					return true;
				}
				else  if ( pItem->GetItemType() == ItemType_Quest )//任务物品 
				{
					if( ClearPoint_CannotTradeAndMail(pItem->GetObjectBase()->dwID))
					{
						char szTip[40] ={0};
						if( pTable->m_iTableID == 0 )
							_snprintf(szTip,sizeof(szTip),"对不起,%s不可邮寄",pItem->GetObjectBase()->strName);
						else 
							_snprintf(szTip,sizeof(szTip),"对不起,%s不可拍卖",pItem->GetObjectBase()->strName);
						szTip[sizeof(szTip)-1] = 0;
						GetGameGuiManager()->AddClientSystemMessage(szTip);
					}
					return true;
				}
				//char szTemp[64] = {0};
				//_snprintf(szTemp,64,"%d",pItem->GetObject()->dwNum);
				//szTemp[63] = 0;
				//CGuiItemCell* pCell = pFloatItem->GetItem();
				//if( pCell && pCell->GetText() != szTemp )
				//{
				//	return true;
				//}

				stObjectLocation dst;

				dst.dwLocation = OBJECTCELLTYPE_MAIL;
				dst.dwTableID = pTable->m_iTableID;
				dst.x = 0;
				dst.y = 0;
				pItem->CopyTo( dst );
				pItem->MoveBackOldLocation();
				GetGameCursor()->SetCursor(CCursor::CursorType_Normal,eCursorNormal);
			}
		}
		return true;
	}

	return true;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  处理装备道具事件
*/
static bool OnEquipItemEvent( CGuiTable* pTable,UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;

	CGuiFloatItem* pFloatItem;
	CGuiItemCell* pDestCell = NULL;
	CRoleItem* pSrcItem = NULL;
	CRoleItem* pDestItem = NULL;

	stPointI ptDestGrid;
	stPointI ptDestPixel = pt;

	if (pTable->m_iTableType!=OBJECTCELLTYPE_COMMON)
		return false;

	if( pFloatItem = GetGuiManager()->GetFloatItem() )
		return false;

	// 计算目标格子
	if(pTable)
	{
		ptDestPixel.x -= pTable->GetX();
		ptDestPixel.y -= pTable->GetY();
		ptDestGrid = pTable->GetGridPosByPixelPos(ptDestPixel);
	}
	else
	{
		ptDestGrid = stPointI(0,0);
	}

	pDestCell = pTable->GetItemCell(ptDestGrid.x,ptDestGrid.y);
	if( !pDestCell )
		return false;

	if(!pDestCell->IsEnabled())
		return false;

	pSrcItem = (CRoleItem*)pDestCell->m_pItemData;

	if (!pSrcItem)
		return false;

	return EquipItem(pSrcItem);

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pTable : 描述
* \param uMsg : 描述
* \param pt : 描述
* \param wParam : 描述
* \param lParam : 描述
* \return 返回值的描述
*/
//Item table event treatment
bool ItemTableEvent(CGuiTable* pTable,UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam)
{
	FUNCTION_BEGIN;

	if(!CRoleItem::s_bCanMoveItem) return false;

	CRoleItem* pSrcItem = GetSelectItem(pTable,pt);
	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();

	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		///触发"快捷使用物品"帮助教程事件
		/*if(bIsUserUnSetted(USER_SETTING_HELP_ACCELKEY))
		{
		if(IsAccelKeyObject(pTable,pt))
		ShowHelpTip(USER_SETTING_HELP_ACCELKEY);
		}*/

		if( GetGameCursor()->GetCursorTypeEx() == eCursorGetMailItem )
		{
			CGuiTable *pMailTable = NULL;
			if( GetGameGuiManager()->m_guiSendMailDlg )
			{
				pMailTable = GetGameGuiManager()->m_guiSendMailDlg->m_pItemTable;
			}
			if( pTable && pTable->m_iTableType == OBJECTCELLTYPE_COMMON )
			{
				CRoleItem* pMailItem = GetCellItem(pMailTable,stPointI(0,0));
				if( pSrcItem )
				{
					if( pMailItem )
					{
						//找到他的主物品
						CRoleItem* pItemImageMaster = pMailItem->GetImageMasterItem();
						if( pItemImageMaster )
						{
							pItemImageMaster->ClearItemImage();
						}		
					}
					stObjectLocation dst;
					dst.dwLocation = OBJECTCELLTYPE_MAIL;
					dst.dwTableID = 0;
					dst.x = 0;
					dst.y = 0;
					pSrcItem->CopyTo(dst);

					GetGameCursor()->SetCursorTypeEx(eCursorNormal) ;
					GetGameCursor()->SetCursor(CCursor::CursorType_Normal,eCursorNormal);
				}
			}
			return true;
		}

		if( pTable && pTable->m_iTableType == OBJECTCELLTYPE_COUNTRY_SAVEBOX)
		{
			if( GetGameGuiManager()->m_guiCountrySaveingBoxDlg)
				GetGameGuiManager()->m_guiCountrySaveingBoxDlg->OnLeftClickedOnItem();
			return true;
		}

		//对于邮件系统的物品禁止用鼠标直接拖动
		if( pTable && pTable->m_iTableType == OBJECTCELLTYPE_MAIL )
		{
			return OnMoveItemOnMailTable(pTable);
		}

		//对于自己的摊位格子,屏蔽鼠标左键
		if( pTable && pTable->m_iTableType == OBJECTCELLTYPE_MYSHOP )
		{
			return false;
		}

		// 商城的商品格子，屏蔽鼠标左键
		if (pTable && pTable->m_iTableType == OBJECTCELLTYPE_MALL)
		{
			return false;
		}

		//如果自己的摆摊界面打开且摊位没有打开,在包裹格子内选择物品
		if( NULL == GetGuiManager()->GetFloatItem() &&
			GetGameGuiManager()->m_guiShop && 
			GetGameGuiManager()->m_guiShop->IsVisible() &&
			!pMainRole->bIsShopStart() &&
			pTable && 			
			pTable->m_iTableType == OBJECTCELLTYPE_COMMON )
		{
			//从包裹移动物品到摊位
			return MoveItem2MyShop(pSrcItem);
		}

		//如果在对方的格子内选择物品,则表示买(自己没有摆摊的情况下)
		if( pTable && 
			pTable->m_iTableType == OBJECTCELLTYPE_OTHERSSHOP &&
			!pMainRole->bIsShopStart() )
		{
			return BuyItemFromOthersShop(pSrcItem);
		}

		//如果物品处于摆摊状态,其他所有操作都被限制了
		if( pSrcItem &&
			pMainRole->bIsShopStart() )
		{
			if( GetGameGuiManager()->m_guiShop &&  
				GetGameGuiManager()->m_guiShop->IsItemIn(pSrcItem->GetThisID()) )
			{
				GetGameGuiManager()->AddClientSystemMessage("该物品正在摊位内销售,不能执行");
				return true;
			}
		}
		if( pTable && 
			pTable->m_iTableType == OBJECTCELLTYPE_OTHERSSHOP )
		{
			return true;
		}
		//如果宝箱就限制操作
		if( pTable && 
			pTable->m_iTableType == OBJECTCELLTYPE_COWRYBOX )
		{
			return false;
		}


		//如果物品有镜像则禁止左键操作
//		if ( pSrcItem )
//		{
//			CRoleItem* pItem = pSrcItem->GetRoleItemImage();
//			if ( pItem )
//			{
//				return true;
//			}
//		}

		//如果物品有镜像则禁止左键操作
		//soke 一键换装相关
		if (pSrcItem && pSrcItem->IsImageItem() )
		{
			//CRoleItem* pItem = pSrcItem->GetRoleItemImage();
			//if ( pItem )
			//{
				return true;
			//}
		}

	
		if( GetKeyState(VK_CONTROL) < 0 )
		{
			// 分裂物品
			if(pTable && pTable->m_iTableType == OBJECTCELLTYPE_SELL)
				return false;
			return OnSplitItemEvent(pTable,uMsg,pt,wParam,lParam);
		}
		else
		{
			if (GetGameCursor()->GetCursorImageType()==eCursorRepair)
			{
				return OnRepairItemEvent(pTable,uMsg,pt,wParam,lParam);
			}
			else
			{
				//GetGameCursor()->SetCursor( CCursor::CursorType_Drag, eCursorNormal );
				if ( GetKeyState( VK_SHIFT) < 0 )
				{
					//for item exchange
					return AddItemToExchange( pTable, pt );
				}
				else
				{
					return OnMoveItemEvent(pTable,uMsg,pt,wParam,lParam);
				}
			}
		}
		return false;
		break;
	case WM_RBUTTONDOWN:
		{
			//对于邮件系统的物品禁止用鼠标直接拖动
			if( pTable && pTable->m_iTableType == OBJECTCELLTYPE_MAIL )
			{
				return true;
			}
		

			//如果物品处于摆摊状态,其他所有操作都被限制了
			if( pSrcItem &&
				pMainRole->bIsShopStart() )
			{
				if( GetGameGuiManager()->m_guiShop &&  
					GetGameGuiManager()->m_guiShop->IsItemIn(pSrcItem->GetThisID()) )
				{
					GetGameGuiManager()->AddClientSystemMessage("该物品正在摊位内销售,不能执行");
					return true;
				}
			}
			//如果是自己的摊位格子
			if( pTable &&
				pTable->m_iTableType == OBJECTCELLTYPE_MYSHOP)
			{				
				if ( !pMainRole->bIsShopStart() )
				{
					//如果在摊位格子里面点则从摊位删除该物品
					return DeleteItemFromShop(pSrcItem);						
				}			
				else
				{
					GetGameGuiManager()->AddClientSystemMessage("该物品正在摊位内销售,不能执行");
					return true;
				}
			}
			if( pTable && 
				pTable->m_iTableType == OBJECTCELLTYPE_OTHERSSHOP )
			{
				return true;
			}
			//如果物品有镜像则禁止右键操作
			/*if ( pSrcItem )
			{
				CRoleItem* pItem = pSrcItem->GetRoleItemImage();
				if ( pItem )
				{
					return true;
				}
			}*/

			//sky 修改换装镜像右键操作
			if (pSrcItem && pSrcItem->IsImageItem() )
			{
				return true;
			}

			if(pTable && pTable->m_iTableType == OBJECTCELLTYPE_SELL)
			{
				return OnMoveItemEvent(pTable,uMsg,pt,wParam,lParam);
			}

			if (OnEquipItemEvent(pTable,uMsg,pt,wParam,lParam)==false)
			{				
				Engine_WriteLogF("物品被点击右键\n");
				//Engine_WriteLogF("1物品名: %s\n",pSrcItem->GetObject()->strName);	
				return OnUseItemEvent(pTable,uMsg,pt,wParam,lParam);
			}

			return true;
		}
		break;
	case WM_LBUTTONUP:		
		break;
	}
	return false;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
CRoleItem::CRoleItem(void)
: m_pObjectBase(NULL)
, m_bNeedUpdateTips(false)
, m_dwTipFlags(0)
, m_dwLastUseTime(0)
, m_price(0)
{
	FUNCTION_BEGIN;

	memset(&m_object,0,sizeof(m_object));

	m_ToolTips.SetAlignType( GUI_ALIGN_LEFT );
	m_ToolTips.SetLineGaps(3);

	m_bIsImage = false;
	m_pImageItem = NULL;
	m_pMasterItem = NULL;
	//ZH 
	m_bCheck = false;//临时列表中是否被点击
	//////////////////////////////////////////////////////////////////////////
	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
CRoleItem::~CRoleItem(void)
{
	FUNCTION_BEGIN;

	ClearItemImage();
	m_ToolTips.Clear();	

	//如果本身是镜像,在自己析构的时候使主物品指向自己的指针为null
	if ( m_pMasterItem )
	{
		m_pMasterItem->m_pImageItem = NULL;
	}

	FUNCTION_END;
}

void CRoleItem::ClearItemImage()
{
	if( m_pImageItem )
	{
		m_pImageItem->OnRemove();
		delete m_pImageItem;
		m_pImageItem = NULL;
	}
}

void CRoleItem::Copy(CRoleItem* pItem)
{
	if ( NULL == pItem ) return ;
	//t_Object obj = pItem->GetTObject();
	m_object = pItem->GetTObject();
	m_pObjectBase = pItem->GetObjectBase();
	m_bNeedUpdateTips = true;
	m_bIsImage = true;		//现在就成了镜像了
	m_pMasterItem = pItem;	
}

void CRoleItem::CopyTo( const stObjectLocation & dst )
{
	//镜像不能再copy了
	if ( m_bIsImage ) return ;
	m_pImageItem = new CRoleItem();
	if( m_pImageItem )
	{
		CGuiTable* pTable = ::GetItemTable(dst);
		if( pTable )
		{
			CRoleItem* pItem = ::GetCellItem(pTable,stPointI(dst.x,dst.y));
			if( pItem )
			{
				//如果想再copy一遍则直接返回
				if (pItem->GetThisID() == GetThisID())
				{
					delete m_pImageItem;
					return;
				}
				pItem->OnRemove();
				delete pItem;
				pItem = NULL;
			}
		}
		//m_pImageItem->SetObject(GetObject());
		//m_pImageItem->m_pMasterItem = this;
		m_pImageItem->Copy( this );
		m_pImageItem->GetObject()->pos = dst;
		m_pImageItem->OnUpdate(-1);
	}
}

void CRoleItem::CopyTo( CGuiTable* pDstTable)
{
	//镜像不能再copy了
	if ( m_bIsImage ) return ;
	m_pImageItem = new CRoleItem();
	if( m_pImageItem )
	{
		//CGuiTable* pTable = ::GetItemTable(dst);
		if( pDstTable )
		{
			CRoleItem* pItem = ::GetCellItem(pDstTable,stPointI(0,0));
			if( pItem )
			{
				//如果想再copy一遍则直接返回
				if (pItem->GetThisID() == GetThisID())
				{
					delete m_pImageItem;
					return;
				}
				pItem->OnRemove();
				delete pItem;
				pItem = NULL;
			}
		}
		stObjectLocation dst;
		dst.dwTableID = pDstTable->GetID();
		dst.x = 0;
		dst.y = 0;
		m_pImageItem->Copy( this );
		m_pImageItem->GetObject()->pos = dst;
		m_pImageItem->OnUpdate(-1,pDstTable);
	}
}
bool CRoleItem::SetObject(t_Object* pObj)
{
	FUNCTION_BEGIN;

	if( NULL == pObj )
	{
		return false;
	}

	m_object = *pObj;

	if (m_object.upgrade > c_maxItemUpgrade )
		m_object.upgrade = c_maxItemUpgrade;

	m_pObjectBase = ::GetObjectBase(m_object.dwObjectID);
	Assert(m_pObjectBase);
	if(!m_pObjectBase)
		return false;

	SetName(m_object.strName);
	m_bNeedUpdateTips = true;

	if (m_object.pos.dwLocation == OBJECTCELLTYPE_RECAST)
	{
		if (!GetGameGuiManager()->m_pDlgEquipRecast)
		{
			GetGameGuiManager()->AddDlgEquipRecast(false);
		}
	}
		if (m_object.pos.dwLocation == OBJECTCELLTYPE_ANNEX)
	{
		if (!GetGameGuiManager()->m_pDlgEquipAnnexpack)
		{
			GetGameGuiManager()->AddDlgEquipAnnexpack(false);
		}
	}
	if (m_object.pos.dwLocation == OBJECTCELLTYPE_HANDBOOK) //soke 百兽图鉴
	{
        if (!GetGameGuiManager()->m_pDlgEquipbookRecast)
		{
			GetGameGuiManager()->AddDlgEquipbookRecast(false);
		}
	
	}
	if (m_object.pos.dwLocation == OBJECTCELLTYPE_FABAO) //soke 法宝
	{
        if (!GetGameGuiManager()->m_pDlgEquipFabaoRecast)
		{
			GetGameGuiManager()->AddDlgEquipFabaoRecast(false);
		}
	
	}
	if (m_object.pos.dwLocation == OBJECTCELLTYPE_YUANSHEN) //soke 法宝
	{
        if (!GetGameGuiManager()->m_pDlgEquipYuanshenRecast)
		{
			GetGameGuiManager()->AddDlgEquipYuanshenRecast(false);
		}
	
	}
	OnUpdate(-1);

	if(m_object.qwThisID > s_dwMaxThisID)
		s_dwMaxThisID = m_object.qwThisID;


	switch(m_object.pos.dwLocation) 
	{
	case OBJECTCELLTYPE_EQUIP:
		{
			if (GetGameGuiManager()->m_guiUserInfo)
			{
				GetGameGuiManager()->m_guiUserInfo->SetUserImage(m_object.pos.y,&m_object);
			}

			GetGameGuiManager()->AddPackage(m_object.pos,this);
			GetGameGuiManager()->AddMakeTo(m_object.pos,this);
		}
		break;
	case OBJECTCELLTYPE_EQUIPSHOW:
		{
			if (GetGameGuiManager()->m_guiUserInfoShow)
			{
				GetGameGuiManager()->m_guiUserInfoShow->SetUserImage(m_object.pos.y,&m_object);
			}
		}
		break;
	}


	//Add to repair list
	if ( NeedConsiderDur() )
	{
		if ( ((m_object.dur+49)/50 < NEED_ADD_REPAIR_VALUE) )
		{
			if ( GetScene()->GetMainCharacter() )
				GetScene()->GetMainCharacter()->AddOneNeedRepairItem( m_object.qwThisID );
		}
		else
		{
			if ( GetScene()->GetMainCharacter() )
				GetScene()->GetMainCharacter()->DeleteOneNeedRepairItem( m_object.qwThisID );
		}
	}
	return true;

	FUNCTION_END;
}
/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pCmd : 描述
* \return 返回值的描述
*/
bool CRoleItem::SetAttrib(stAddObjectPropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	return SetObject(&(pCmd->object));
	return true;

	FUNCTION_END;
}


void CRoleItem::DestoryAllVirtualCell()
{
	if(!GetGameGuiManager()->m_guiItem) return;
	tCells::iterator it = m_cells.begin();
	++it;
	for( ; it != m_cells.end(); )
	{
		stCellInfo& info = (*it);

		if(info.pCell->m_pTable->m_pDialog == GetGuiManager()->GetFloatItem())
		{
			GetGuiManager()->SetMouseDlg(NULL);
		}

		info.pCell->m_pTable->RemoveItem(info.pCell->GetPos().x,info.pCell->GetPos().y,true);

		it = m_cells.erase(it);
	}
	if(!m_cells.empty())
	{
		m_cells.front().pCell->SetVisible(true);
		m_cells.front().pCell->SetEnabled(true);
		m_cells.front().count = GetCount();
		if(GetMaxCount() > 1 && GetCount() > 1)
		{
			m_cells.front().pCell->SetText(avar("%d",GetCount()));
		}
		else
		{
			m_cells.front().pCell->SetText("");
		}
	}
}

void CRoleItem::DestoryAllCell()
{
	for(tCells::iterator it = m_cells.begin(); it != m_cells.end(); ++it)
	{
		stCellInfo& info = (*it);
		//Engine_WriteLogF("1物品名: %s\n",this->GetObject()->strName);
		if(info.pCell && info.pCell->m_pTable)
		{
			if(info.pCell->m_pTable->m_pDialog == GetGuiManager()->GetFloatItem())
			{
				GetGuiManager()->SetMouseDlg(NULL);
			}

			info.pCell->m_pTable->RemoveItem(info.pCell->GetPos().x,info.pCell->GetPos().y,true);
		}
	}
	m_cells.clear();
}

void CRoleItem::AccessKeyOnRemove( const DWORD newItemID )
{
	//Remove all access key of this item
	if ( GetGameGuiManager()->m_guiMain )
	{
		if ( -1 == newItemID )
		{
			DWORD typeID = IsEquipment() ? GetThisID() : GetObjectID();
			GetGameGuiManager()->m_guiMain->DeleteAccessKey( IsEquipment() ? GameAccelKeyType_Equipment : GameAccelKeyType_Item, typeID );
		}
		else
		{
			if ( GetItemType() == ItemType_FillHP )
				GetGameGuiManager()->m_guiMain->ReplaceAccessKey( GameAccelKeyType_Item, m_object.qwThisID, newItemID );
		}
	}
}


/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CRoleItem::OnRemove()
{
	FUNCTION_BEGIN;

	if(IsMoney())
	{
		SetMoneyText(GetLocation().dwLocation,0);
	}
	/*if(GetGameState()->IsStateItem(eGameStateItem_TradeRun) && IsTradeCell())
	{
	GetGameGuiManager()->m_guiTrade->OnItemChange();
	}*/

	//Clear user info equip pos img
	if ( m_object.pos.dwLocation==OBJECTCELLTYPE_EQUIP )
	{
		if (GetGameGuiManager()->m_guiUserInfo)
		{
			GetGameGuiManager()->m_guiUserInfo->SetUserImage(m_object.pos.y);
		}
	}

	//包裹离开装备位置
	{
		CGuiTable* pTable = NULL;
		if ((m_object.pos.dwLocation==OBJECTCELLTYPE_EQUIP)&&(m_object.pos.y==EQUIPCELLTYPE_PACKAGE))
		{
			stObjectLocation pos;
			pos.dwLocation = OBJECTCELLTYPE_PACKAGE;
			pos.dwTableID = m_object.qwThisID;
			pTable = GetItemTable(pos);
		}
		if ((m_object.pos.dwLocation==OBJECTCELLTYPE_EQUIP)&&(m_object.pos.y==EQUIPCELLTYPE_MAKE))
		{
			stObjectLocation pos;
			pos.dwLocation = OBJECTCELLTYPE_MAKE;
			pos.dwTableID = m_object.qwThisID;
			pos.y = MAKECELLTYPE_EQUIP;
			pTable = GetItemTable(pos);
		}
		if (pTable)
		{
			if (pTable->m_pDialog)
			{
				pTable->m_pDialog->m_bCloseIsHide = false;
				pTable->m_pDialog->Close();
			}
		}
	}

	DestoryAllCell();

	/*if(m_pCell)
	{
	CGuiTable* pGuiTable = m_pCell->m_pTable;
	Assert(pGuiTable);
	POINT pos = m_pCell->GetPos();
	pGuiTable->RemoveItem(pos.x,pos.y,true);
	if(GetLocation().dwLocation == OBJECTCELLTYPE_MOUSE)
	{
	GetGuiManager()->SetMouseDlg(NULL);
	}
	}*/

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pCmd : 描述
* \return 返回值的描述
*/
void CRoleItem::OnSplit(stSplitObjectPropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;
	//宝合一个个放的功能
	//if ((pCmd->dst.dwLocation != OBJECTCELLTYPE_MAKE)||(m_object.dwNum == pCmd->dwNum))
	{
		DestoryAllVirtualCell();
	}

	CRoleItem* pDstItem = GetScene()->GetMainCharacter()->FindItemByLocation(pCmd->dst);
	DWORD num = 0;
	stObjectLocation m_oldPos = m_object.pos;

	if(pDstItem)
	{
		Assert(pDstItem->GetObjectBase()->dwID == m_object.dwObjectID);

		//Assert(0);
		//GetScene()->GetMainCharacter()->RemoveItem(pDstItem->GetThisID());

		num = pDstItem->GetCount();
	}

	Assert(pCmd->dwNum <= m_object.dwNum);

	m_object.dwNum -= pCmd->dwNum;

	stAddObjectPropertyUserCmd cmd;
	cmd.byActionType = EQUIPACTION_INIT;
	cmd.object = m_object;
	cmd.object.qwThisID = pCmd->qwNewThisID;
	cmd.object.dwNum = pCmd->dwNum + num;
	cmd.object.pos = pCmd->dst;

	if(m_object.dwNum == 0)
	{
		GetScene()->GetMainCharacter()->RemoveItem(GetThisID());
	}
	else
	{
		//宝合一个个放的功能
		//if (pCmd->dst.dwLocation == OBJECTCELLTYPE_MAKE)
		//	OnUpdate(UPDATE_FLAG_MAKECOUNT);
		//else
		OnUpdate(UPDATE_FLAG_COUNT);
	}

	GetScene()->GetMainCharacter()->AddItem(&cmd);

	/*if(GetGameState()->IsStateItem(eGameStateItem_TradeRun) && 
	( ::IsTradeCell(pCmd->dst.dwLocation) || IsTradeCell()) )
	{
	GetGameGuiManager()->m_guiTrade->OnItemChange();
	}*/

	if ((pCmd->dst.dwLocation==OBJECTCELLTYPE_MAKE)||(m_oldPos.dwLocation==OBJECTCELLTYPE_MAKE))
	{
		if (GetGameGuiManager()->m_guiMakeTo1)
		{
			GetGameGuiManager()->m_guiMakeTo1->UpdateData(true);
		}
		if (GetGameGuiManager()->m_guiMakeTo2)
		{
			GetGameGuiManager()->m_guiMakeTo2->UpdateData(true);
		}
	}

	FUNCTION_END;
}

bool CRoleItem::SetTrueCellCount(int count)
{
	if(m_cells.empty()) return false;
	m_cells.front().count = count;
	m_cells.front().pCell->SetText(avar("%d",count));
	return true;
}


/**
* \brief  为物品增加一个显示位置
*
*
*
* \return  无
*
*/
void CRoleItem::AddShowLocation(CGuiItemCell* pSrcCell,int count,const stObjectLocation & dst,bool bHideSrcLocation,bool bDisableSrcLocation)
{
	CGuiTable* pDestTable = GetItemTable(dst);
	Assert(pDestTable);

	CGuiItemCell* pDestCell = NULL;
	stRectI rect(dst.x,dst.y,dst.x + GetObjectBase()->cx,dst.y + GetObjectBase()->cy);
	int count1 = pDestTable->GetRectItem(&rect,&pDestCell,1);
	Assert(count1 <= 1);
	Assert(pDestCell == NULL);
	if(pDestCell)
	{
		pDestTable->RemoveItem(pDestCell->GetPos().x,pDestCell->GetPos().y,true);
	}

	pDestCell = new CGuiItemCell;
	pDestCell->m_pItemData = this;
	//pDestCell->SetPos(0,0);     //zzf

	pDestCell->SetPos(dst.x,dst.y);
	pDestCell->SetSize(GetObjectBase()->cx,GetObjectBase()->cy);

	pDestTable->AddItem(pDestCell);


	OnAddCell(pDestCell,count);

	if( GetObjectBase()->dwMaxCount > 1 && GetCount() > 1)
	{
		int num = GetCount() - count;
		SetTrueCellCount(num);
		pDestCell->SetText(avar("%d",count));

	}

	if (dst.dwLocation == OBJECTCELLTYPE_MOUSE) //zzf
	{
		GetGuiManager()->SetMouseDlg(GetGuiFloatItem()); 
	}
	else
	{
		//GetGuiManager()->SetMouseDlg(NULL); 
	}

	// 如果不是原始的格子，则删除
	CGuiItemCell* pCell;
	GetTrueCell(pCell,count1);
	if(pSrcCell != pCell)
	{
		pCell->m_pTable->RemoveItem(pCell->GetPos().x,pCell->GetPos().y,true);
	}
	OnUpdate(UPDATE_FLAG_IMAGE);
	if(bHideSrcLocation)
		pSrcCell->SetVisible(false);
	if(bDisableSrcLocation)
		pSrcCell->SetEnabled(false);
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pCmd : 描述
* \return 返回值的描述
*/
void CRoleItem::OnSwap(stSwapObjectPropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	DestoryAllVirtualCell();

	if ( pCmd->dst.dwLocation == OBJECTCELLTYPE_STORE )
		AccessKeyOnRemove();

	/*if(GetGameState()->IsStateItem(eGameStateItem_TradeRun) && 
	( ::IsTradeCell(pCmd->dst.dwLocation) || IsTradeCell()) )
	{
	GetGameGuiManager()->m_guiTrade->OnItemChange();
	}*/

	CGuiTable* pDestTable = GetItemTable(pCmd->dst);
	stObjectLocation m_oldPos = m_object.pos;
	Assert(pDestTable);

	DWORD dwFlags = UPDATE_FLAG_LOCATION;
	if(pCmd->dst.dwLocation != GetLocation().dwLocation)
		dwFlags |= UPDATE_FLAG_IMAGE;
	if ( IsArrow() )
		dwFlags |= UPDATE_FLAG_COUNT;

	if(pDestTable)
	{
		CGuiItemCell* pDestCell = NULL;
		stRectI rect(pCmd->dst.x,pCmd->dst.y,pCmd->dst.x + GetObjectBase()->cx,pCmd->dst.y + GetObjectBase()->cy);
		if(pCmd->dst.dwLocation == OBJECTCELLTYPE_STORE)
		{
			int y = pCmd->dst.y % CGuiSavingBoxDialog::csShowRowCount;
			rect.SetRect(pCmd->dst.x,y,pCmd->dst.x + GetObjectBase()->cx,y + GetObjectBase()->cy);
		}
		int count = pDestTable->GetRectItem(&rect,&pDestCell,1);
		Assert(count <= 1);

		if ((m_object.pos.dwLocation==OBJECTCELLTYPE_EQUIP)&&(dwFlags & UPDATE_FLAG_IMAGE))
		{
			if (GetGameGuiManager()->m_guiUserInfo)
			{
				GetGameGuiManager()->m_guiUserInfo->SetUserImage(m_object.pos.y);
			}
		}

		if ((pDestTable->m_iTableType==OBJECTCELLTYPE_EQUIP)&&(dwFlags & UPDATE_FLAG_IMAGE))
		{
			if (GetGameGuiManager()->m_guiUserInfo)
			{
				GetGameGuiManager()->m_guiUserInfo->SetUserImage(pDestTable->m_EquipPosition.y,&m_object);
			}
		}

		if( pDestCell )
		{
			// 交换位置
			pDestTable->RemoveItem(pDestCell->GetPos().x,pDestCell->GetPos().y,false);
			pDestCell->m_pTable = NULL;

			CRoleItem* pDestItem = ( CRoleItem* )pDestCell->m_pItemData;
			Assert( pDestItem );

			if ((m_object.pos.dwLocation==OBJECTCELLTYPE_EQUIP)&&(dwFlags & UPDATE_FLAG_IMAGE))
			{
				if (GetGameGuiManager()->m_guiUserInfo)
				{
					GetGameGuiManager()->m_guiUserInfo->SetUserImage(m_object.pos.y,pDestItem->GetObject());
				}
			}

			//包裹离开装备位置
			{
				CGuiTable* pTable = NULL;
				if ((m_object.pos.dwLocation==OBJECTCELLTYPE_EQUIP)&&(m_object.pos.y==EQUIPCELLTYPE_PACKAGE))
				{
					stObjectLocation pos;
					pos.dwLocation = OBJECTCELLTYPE_PACKAGE;
					pos.dwTableID = m_object.qwThisID;
					pTable = GetItemTable(pos);
				}
				if ((pDestItem->m_object.pos.dwLocation==OBJECTCELLTYPE_EQUIP)&&(pDestItem->m_object.pos.y==EQUIPCELLTYPE_PACKAGE))
				{
					stObjectLocation pos;
					pos.dwLocation = OBJECTCELLTYPE_PACKAGE;
					pos.dwTableID = pDestItem->m_object.qwThisID;
					pTable = GetItemTable(pos);
				}
				if ((m_object.pos.dwLocation==OBJECTCELLTYPE_EQUIP)&&(m_object.pos.y==EQUIPCELLTYPE_MAKE))
				{
					stObjectLocation pos;
					pos.dwLocation = OBJECTCELLTYPE_MAKE;
					pos.dwTableID = pDestItem->m_object.qwThisID;
					pos.y = MAKECELLTYPE_EQUIP;
					pTable = GetItemTable(pos);
				}
				if (pTable)
				{
					pTable->m_pDialog->m_bCloseIsHide = false;
					pTable->m_pDialog->Close();
				}
			}


			pDestItem->m_object.pos = m_object.pos;

			m_object.pos = pCmd->dst;
			OnUpdate(dwFlags);

			if ( pDestItem->IsArrow() )
				pDestItem->OnUpdate(dwFlags | UPDATE_FLAG_COUNT);
			else
				pDestItem->OnUpdate(dwFlags);

			//包裹加入装备位置
			{
				if (m_object.pos.dwLocation==OBJECTCELLTYPE_EQUIP)
				{
					GetGameGuiManager()->AddPackage(m_object.pos,this);
					GetGameGuiManager()->AddMakeTo(m_object.pos,this);
				}
				if (pDestItem->m_object.pos.dwLocation==OBJECTCELLTYPE_EQUIP)
				{
					GetGameGuiManager()->AddPackage(pDestItem->m_object.pos,pDestItem);
					GetGameGuiManager()->AddMakeTo(pDestItem->m_object.pos,pDestItem);
				}
			}

		}
		else
		{
			if( m_object.pos.dwLocation == OBJECTCELLTYPE_MOUSE )
			{
				GetGuiManager()->SetMouseDlg(NULL);
			}

			//包裹离开装备位置
			{
				CGuiTable* pTable = NULL;
				if ((m_object.pos.dwLocation==OBJECTCELLTYPE_EQUIP)&&(m_object.pos.y==EQUIPCELLTYPE_PACKAGE))
				{
					stObjectLocation pos;
					pos.dwLocation = OBJECTCELLTYPE_PACKAGE;
					pos.dwTableID = m_object.qwThisID;
					pTable = GetItemTable(pos);
				}
				if ((m_object.pos.dwLocation==OBJECTCELLTYPE_EQUIP)&&(m_object.pos.y==EQUIPCELLTYPE_MAKE))
				{
					stObjectLocation pos;
					pos.dwLocation = OBJECTCELLTYPE_MAKE;
					pos.dwTableID = m_object.qwThisID;
					pos.y = MAKECELLTYPE_EQUIP;
					pTable = GetItemTable(pos);
				}
				if (pTable)
				{
					pTable->m_pDialog->m_bCloseIsHide = false;
					pTable->m_pDialog->Close();
				}
			}

			m_object.pos = pCmd->dst;
			OnUpdate(dwFlags);

			//包裹加入装备位置
			if (m_object.pos.dwLocation==OBJECTCELLTYPE_EQUIP)
			{
				GetGameGuiManager()->AddPackage(m_object.pos,this);
				GetGameGuiManager()->AddMakeTo(m_object.pos,this);
			}
		}

		if ((pDestTable->m_iTableType==OBJECTCELLTYPE_MAKE)||(m_oldPos.dwLocation==OBJECTCELLTYPE_MAKE))
		{
			if (GetGameGuiManager()->m_guiMakeTo1)
			{
				GetGameGuiManager()->m_guiMakeTo1->UpdateData(true);
			}
			if (GetGameGuiManager()->m_guiMakeTo2)
			{
				GetGameGuiManager()->m_guiMakeTo2->UpdateData(true);
			}
		}

		if ((pDestTable->m_iTableType==OBJECTCELLTYPE_EQUIP)||(m_oldPos.dwLocation==OBJECTCELLTYPE_EQUIP))
		{
			GetScene()->GetMainCharacter()->RefreshActiveItem(OBJECTCELLTYPE_EQUIP);
		}

		if ((pDestTable->m_iTableType == OBJECTCELLTYPE_RECAST) || (m_oldPos.dwLocation == OBJECTCELLTYPE_RECAST))
		{
			if (GetGameGuiManager()->m_pDlgEquipRecast)
			{
				GetGameGuiManager()->m_pDlgEquipRecast->OnPutFocusItem(true);
			}
		}
		if ((pDestTable->m_iTableType == OBJECTCELLTYPE_ANNEX) || (m_oldPos.dwLocation == OBJECTCELLTYPE_ANNEX))
		{
			if (GetGameGuiManager()->m_pDlgEquipAnnexpack)
			{
				GetGameGuiManager()->m_pDlgEquipAnnexpack->OnPutFocusItem(true);
			}
		}
		if ((pDestTable->m_iTableType == OBJECTCELLTYPE_HANDBOOK) || (m_oldPos.dwLocation == OBJECTCELLTYPE_HANDBOOK))
		{
			if (GetGameGuiManager()->m_pDlgEquipbookRecast)
			{
				GetGameGuiManager()->m_pDlgEquipbookRecast->OnPutFocusItembook(true);
			}
		}
		if ((pDestTable->m_iTableType == OBJECTCELLTYPE_FABAO) || (m_oldPos.dwLocation == OBJECTCELLTYPE_FABAO))
		{
			if (GetGameGuiManager()->m_pDlgEquipFabaoRecast)
			{
				GetGameGuiManager()->m_pDlgEquipFabaoRecast->OnPutFocusItembook(true);
			}
		}
		if ((pDestTable->m_iTableType == OBJECTCELLTYPE_YUANSHEN) || (m_oldPos.dwLocation == OBJECTCELLTYPE_YUANSHEN))
		{
			if (GetGameGuiManager()->m_pDlgEquipYuanshenRecast)
			{
				GetGameGuiManager()->m_pDlgEquipYuanshenRecast->OnPutFocusItembook(true);
			}
		}
	}	

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pCmd : 描述
* \return 返回值的描述
*/
void CRoleItem::OnSetCount(stRefCountObjectPropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	m_object.dwNum = pCmd->dwNum;
	OnUpdate(UPDATE_FLAG_COUNT);
	m_bNeedUpdateTips = true;

	/*if(GetGameState()->IsStateItem(eGameStateItem_TradeRun) && 
	IsTradeCell() )
	{
	GetGameGuiManager()->m_guiTrade->OnItemChange();
	}*/

	FUNCTION_END;
}

void CRoleItem::OnRemoveCell(CGuiItemCell* pCell)
{
	tCells::iterator it = FindCell(pCell);
	if(it != m_cells.end())
	{
		if( GetCount() > 1 )
		{
			SetTrueCellCount(m_cells.front().count + (*it).count);
		}

		if(pCell->m_pTable->m_pDialog == GetGuiManager()->GetFloatItem())
			GetGuiManager()->SetMouseDlg(NULL);

		m_cells.erase(it);
	}
	if(m_cells.size() == 1) 
	{
		m_cells.front().pCell->SetVisible(true);
		m_cells.front().pCell->SetEnabled(true);
	}
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param dwFlags : 描述
* \return 返回值的描述
*/
void CRoleItem::OnUpdate(DWORD dwFlags,CGuiTable* pDstTable)
{
	FUNCTION_BEGIN;

	if(IsMoney())
	{
		SetMoneyText(GetLocation().dwLocation,GetCount());
		return;
	}

	CGuiTable* pTable = pDstTable;
	//Assert(pTable);
	if(!pTable) return;

	if(m_cells.empty())
	{
		CGuiItemCell* pCell = new CGuiItemCell;
		pCell->m_pItemData = this;
		SetTrueCell(pCell,GetCount());
	}

	CGuiItemCell* pCell = NULL;
	int count1;
	GetTrueCell(pCell,count1);
	Assert(pCell);
	if(!pCell) return ;

	if(dwFlags & UPDATE_FLAG_LOCATION)
	{
		DestoryAllVirtualCell();

		if(pCell->m_pTable)
		{
			pCell->m_pTable->RemoveItem(pCell->GetPos().x,pCell->GetPos().y,false);
			pCell->m_pTable = NULL;
		}

		pCell->SetSize(GetObjectBase()->cx,GetObjectBase()->cy);

		stPointI pos = stPointI(m_object.pos.x,m_object.pos.y);

		/*
		*仓库的格子处理起来比较复杂,仓库有3个页面,把它当作一个页面来处理，
		*第二个页面的y坐标也就是实际的y坐标加上第一个页面的高度
		*第三个页面类推
		*根据服务端发来的y坐标,计算得到相应的格子,然后把物品放到仓库对应的页面上的相应格子
		*例如pos.y == 6表示第二页的第1行
		*/
		if( pTable->m_iTableType == OBJECTCELLTYPE_STORE )
		{
			pos.y %= CGuiSavingBoxDialog::csShowRowCount;
		}
		pCell->SetPos(pos.x,pos.y);

		pTable->AddItem(pCell);

		if(m_object.pos.dwLocation == OBJECTCELLTYPE_MOUSE)
		{
			GetGuiManager()->SetMouseDlg(GetGuiFloatItem());
		}
	}

	if( dwFlags & UPDATE_FLAG_IMAGE )
	{
		SetImage();
	}

	//Get or Update hole image
	if ( dwFlags & UPDATE_FLAG_IMAGE )			//UPDATE_FLAG_HOLE
	{
		//SetHoleImage();
	}

	if(dwFlags & UPDATE_FLAG_COUNT)
	{

		DestoryAllVirtualCell();

		char szText[256];

		DWORD num = 0;
		if ( IsArrow() )
		{
			if ( m_object.qwThisID == INVALID_THISID )
			{
				if ( m_pObjectBase )
					num = m_pObjectBase->dwMaxDur;
			}
			else
				num = m_object.dur;
		}		
		else
			num = m_object.dwNum;

		if(num > 1)
		{
			sprintf(szText,"%u",num);
		}
		else
		{
			szText[0] = 0;
		}
		pCell->SetText(szText);
	}

	if(dwFlags & UPDATE_FLAG_MAKECOUNT)
	{
		char szText[256];
		DWORD num = IsArrow()?m_object.dur:m_object.dwNum;
		if(num > 1)
		{
			sprintf(szText,"%u",num);
		}
		else
		{
			szText[0] = 0;
		}

		for( tCells::iterator it = m_cells.begin(); it != m_cells.end(); ++it)
		{
			stCellInfo& info = (*it);
			Assert(info.pCell);
			if(info.pCell && info.pCell->m_pTable && info.pCell->m_pTable->m_pDialog == GetGuiManager()->GetFloatItem())
			{
				info.count = GetCount();
				info.pCell->SetText(szText);
				break;
			}
			
		}
	}

	FUNCTION_END;
}
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwFlags : 描述
 * \return 返回值的描述
 */
void CRoleItem::OnUpdate(DWORD dwFlags)
{
	FUNCTION_BEGIN;

	if(IsMoney())
	{
		SetMoneyText(GetLocation().dwLocation,GetCount());
		return;
	}

	CGuiTable* pTable = GetItemTable(m_object.pos);
	//Assert(pTable);
	if(!pTable) return;

	if(m_cells.empty())
	{
		CGuiItemCell* pCell = new CGuiItemCell;
		pCell->m_pItemData = this;
		SetTrueCell(pCell,GetCount());
	}

	CGuiItemCell* pCell = NULL;
	int count1;
	GetTrueCell(pCell,count1);
	Assert(pCell);
	if(!pCell) return ;

	if(dwFlags & UPDATE_FLAG_LOCATION)
	{
		DestoryAllVirtualCell();

		if(pCell->m_pTable)
		{
			pCell->m_pTable->RemoveItem(pCell->GetPos().x,pCell->GetPos().y,false);
			pCell->m_pTable = NULL;
		}

		pCell->SetSize(GetObjectBase()->cx,GetObjectBase()->cy);

		stPointI pos = stPointI(m_object.pos.x,m_object.pos.y);
		
		/*
		*仓库的格子处理起来比较复杂,仓库有3个页面,把它当作一个页面来处理，
		*第二个页面的y坐标也就是实际的y坐标加上第一个页面的高度
		*第三个页面类推
		*根据服务端发来的y坐标,计算得到相应的格子,然后把物品放到仓库对应的页面上的相应格子
		*例如pos.y == 6表示第二页的第1行
		*/
		if( pTable->m_iTableType == OBJECTCELLTYPE_STORE )
		{
			pos.y %= CGuiSavingBoxDialog::csShowRowCount;
		}
		pCell->SetPos(pos.x,pos.y);

		pTable->AddItem(pCell);

		if(m_object.pos.dwLocation == OBJECTCELLTYPE_MOUSE)
		{
			GetGuiManager()->SetMouseDlg(GetGuiFloatItem());
		}
	}

	if( dwFlags & UPDATE_FLAG_IMAGE )
	{
		SetImage();
	}
	
	//Get or Update hole image
	if ( dwFlags & UPDATE_FLAG_IMAGE )			//UPDATE_FLAG_HOLE
	{
		//SetHoleImage();
	}

	if(dwFlags & UPDATE_FLAG_COUNT)
	{

		DestoryAllVirtualCell();

		char szText[256];

		DWORD num = 0;
		if ( IsArrow() )
		{
			if ( m_object.qwThisID == INVALID_THISID )
			{
				if ( m_pObjectBase )
					num = m_pObjectBase->dwMaxDur;
			}
			else
				num = m_object.dur;
		}		
		else
			num = m_object.dwNum;

		if(num > 1)
		{
			sprintf(szText,"%u",num);
		}
		else
		{
			szText[0] = 0;
		}
		pCell->SetText(szText);
	}

	if(dwFlags & UPDATE_FLAG_MAKECOUNT)
	{
		char szText[256];
		DWORD num = IsArrow()?m_object.dur:m_object.dwNum;
		if(num > 1)
		{
			sprintf(szText,"%u",num);
		}
		else
		{
			szText[0] = 0;
		}

		for( tCells::iterator it = m_cells.begin(); it != m_cells.end(); ++it)
		{
			stCellInfo& info = (*it);
			Assert(info.pCell);
			if(info.pCell && info.pCell->m_pTable && info.pCell->m_pTable->m_pDialog == GetGuiManager()->GetFloatItem())
			{
				info.count = GetCount();
				info.pCell->SetText(szText);
				break;
			}
			
		}
	}

	FUNCTION_END;
}

DWORD CRoleItem::GetExteriorCount()
{
	return ( IsArrow() ? m_object.dur : m_object.dwNum );
}


/**
* \brief 简短描述
* 
* 详细描述
* 
* \param  : 描述
* \return 返回值的描述
*/
//Show or hide hole image
void CRoleItem::ShowHoleImage(bool bHide /* = false  */)
{
	FUNCTION_BEGIN;

	//show or hide hole
	for(tCells::iterator it = m_cells.begin(); it != m_cells.end(); ++it)
	{
		(*it).pCell->ShowImageByType( CGuiImage::enumUSE_TYPE::USE_TYPE_HOLE, bHide );
		//show or hide diamond
		(*it).pCell->ShowImageByType( CGuiImage::enumUSE_TYPE::USE_TYPE_DIAMOND, bHide );
	}

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param id : 描述
* \return 返回值的描述
*/
int GetDiamondResourceFrameFromID( DWORD id )
{
	switch( id )
	{
		case 680:
			return 610;
			break;
		case 748:
			return 606;
			break;
		case 749:
			return 608;
			break;
		case 750:
			return 607;
			break;
		case 751:
			return 609;
			break;
	}

	return -1;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
//Get or Update hole image
void CRoleItem::SetHoleImage()
{
	FUNCTION_BEGIN;

	//Thie time we set a special res location
	stResourceLocation rHole;
	rHole.SetFileName( "data\\interfaces.gl" );
	rHole.group = 4;
	rHole.frame = 605;

	stResourceLocation rDiamond;
	rDiamond.SetFileName( "data\\interfaces.gl" );
	rDiamond.group = 4;
	rDiamond.frame = 610;		//167

	CGuiItemCell* pCell;int count1;
	GetTrueCell(pCell,count1);
	Assert(pCell);
	//see if this res empty
	if ( !rHole.IsEmpty() )
	{
		//Get diamond num and hole position
		int numHole = 0;
		//hole size scale
		float scaleHole = 0.5f;
		float sizeHole = 50.0f;
		sizeHole *= scaleHole;
		float sizeHoleHalf = 0.5f * sizeHole;

		//item position
		stPointI sizeItem;
		stPointI offsetCell;
		ObjectBase_t* pObjectBase = ::GetObjectBase( m_object.dwObjectID );
		assert( pObjectBase );
		if( pObjectBase )
		{
			//diamond num
			for( int i=0;i<6;i++ )
			{
				if ( m_object.socket[i] != 0 )
					numHole++ ;
			}
			if ( pCell->m_pTable->m_bVirtual )
			{
				offsetCell.x = pCell->GetImage()->GetFrame(0)->GetClipRect().left;
				offsetCell.y = pCell->GetImage()->GetFrame(0)->GetClipRect().top;
				sizeItem.x = pCell->GetImage()->GetFrame(0)->GetClipRect().right - pCell->GetImage()->GetFrame(0)->GetClipRect().left;
				sizeItem.y = pCell->GetImage()->GetFrame(0)->GetClipRect().bottom - pCell->GetImage()->GetFrame(0)->GetClipRect().top;
			}
			else
			{
				offsetCell.x = 0;
				offsetCell.y = 0;
				sizeItem.x = pObjectBase->cx * CGuiTable::s_ctrlCellSize.cx;
				sizeItem.y = pObjectBase->cy * CGuiTable::s_ctrlCellSize.cy;
			}  			
		}

		stRectI rcHoleImage;
		rcHoleImage.SetRect( 0, 0, sizeHole, sizeHole );
		//We add hole and diamond
		//numHole = 3;
		switch( numHole )
		{
		case 0:		//Item has no socket
			break;
		case 1:
			{
				stRectI rcHolePos;
				//add hole
				rcHolePos.SetRect( sizeItem.x / 2.0f - sizeHoleHalf, sizeItem.y / 2.0f - sizeHoleHalf, sizeItem.x / 2.0f + sizeHoleHalf, sizeItem.y / 2.0f + sizeHoleHalf );
				rcHolePos.OffsetRect( offsetCell );
				//Then we add diamond
				for( int i=0;i<1;i++ )
				{
					for(tCells::iterator it = m_cells.begin(); it != m_cells.end(); ++it)
					{
						(*it).pCell->AddAnimation( rHole, -1, NULL, &rcHolePos, false, CGuiImage::enumUSE_TYPE::USE_TYPE_HOLE );
						if ( (m_object.socket[i] != -1) && (m_object.socket[i] != 0) )		//
						{
							//cur hole has diamond
							/*ObjectBase_t* pDiamond = ::GetObjectBase( m_object.socket[i] );
							if ( !pDiamond )
							{
							continue;
							}
							stResourceLocation rDiamond = GetObjectBasePicLocation( pDiamond );*/
							//rDiamond.frame = 304;
							stRectI rcDiamond = rcHolePos;
							if ( GetDiamondResourceFrameFromID( m_object.socket[i] ) >= 0 )
								rDiamond.frame = GetDiamondResourceFrameFromID( m_object.socket[i] );
							else
							//	rDiamond.frame = 9;
							    rDiamond.frame = 610;
							//add diamond
							(*it).pCell->AddAnimation( rDiamond, -1, NULL, &rcDiamond, false, CGuiImage::enumUSE_TYPE::USE_TYPE_DIAMOND );
						}
					}
				}
			}
			break;
		case 2:
			{
				stRectI rcHolePos[2];
				//add hole
				float gapsY = (sizeItem.y-2.0f*sizeHole) / 3.0f;
				rcHolePos[0].SetRect( sizeItem.x / 2.0f - sizeHoleHalf, gapsY, sizeItem.x / 2.0f + sizeHoleHalf, gapsY+sizeHole );
				rcHolePos[1].SetRect( sizeItem.x / 2.0f - sizeHoleHalf, 2.0f*gapsY+sizeHole, sizeItem.x / 2.0f + sizeHoleHalf, sizeItem.y-gapsY );
				rcHolePos[0].OffsetRect( offsetCell );
				rcHolePos[1].OffsetRect( offsetCell );
				//Then we add diamond
				int diamondNum = 0;
				for( int i=0;i<2;i++ )
				{
					for(tCells::iterator it = m_cells.begin(); it != m_cells.end(); ++it)
					{
						(*it).pCell->AddAnimation( rHole, -1, NULL, &rcHolePos[i], false, CGuiImage::enumUSE_TYPE::USE_TYPE_HOLE );
						if ( (m_object.socket[i] != -1) && (m_object.socket[i] != 0) )
						{
							//cur hole has diamond
							/*ObjectBase_t* pDiamond = ::GetObjectBase( m_object.socket[i] );
							if ( !pDiamond )
							{
							continue;
							}
							stResourceLocation rDiamond = GetObjectBasePicLocation( pDiamond );*/
							stRectI rcDiamond = rcHolePos[diamondNum];
							if ( GetDiamondResourceFrameFromID( m_object.socket[i] ) >= 0 )
								rDiamond.frame = GetDiamondResourceFrameFromID( m_object.socket[i] );
							else
							//	rDiamond.frame = 9;
							    rDiamond.frame = 610;
							//add diamond
							(*it).pCell->AddAnimation( rDiamond, -1, NULL, &rcDiamond, false, CGuiImage::enumUSE_TYPE::USE_TYPE_DIAMOND );
							diamondNum++ ;
						}
					}
				}
			}
			break;
		case 3:
			{
				stRectI rcHolePos[3];
				//add hole
				float gapsY = (sizeItem.y-3.0f*sizeHole) / 4.0f;
				rcHolePos[0].SetRect( sizeItem.x / 2.0f - sizeHoleHalf, gapsY, sizeItem.x / 2.0f + sizeHoleHalf, gapsY+sizeHole );
				rcHolePos[1].SetRect( sizeItem.x / 2.0f - sizeHoleHalf, 2.0f*gapsY+sizeHole, sizeItem.x / 2.0f + sizeHoleHalf, 2.0f*(gapsY+sizeHole) );
				rcHolePos[2].SetRect( sizeItem.x / 2.0f - sizeHoleHalf, 3.0f*gapsY+2.0f*sizeHole, sizeItem.x / 2.0f + sizeHoleHalf, sizeItem.y-gapsY );
				rcHolePos[0].OffsetRect( offsetCell );
				rcHolePos[1].OffsetRect( offsetCell );
				rcHolePos[2].OffsetRect( offsetCell );
				//Then we add diamond
				int diamondNum = 0;
				for( int i=0;i<3;i++ )
				{
					for(tCells::iterator it = m_cells.begin(); it != m_cells.end(); ++it)
					{
						(*it).pCell->AddAnimation( rHole, -1, NULL, &rcHolePos[i], false, CGuiImage::enumUSE_TYPE::USE_TYPE_HOLE );
						if ( (m_object.socket[i] != -1) && (m_object.socket[i] != 0) )		//
						{
							////cur hole has diamond
							//ObjectBase_t* pDiamond = ::GetObjectBase( m_object.socket[i] );
							//if ( !pDiamond )
							//{
							//	continue;
							//}
							//stResourceLocation rDiamond = GetObjectBasePicLocation( pDiamond );
							stRectI rcDiamond = rcHolePos[diamondNum];
							if ( GetDiamondResourceFrameFromID( m_object.socket[i] ) >= 0 )
								rDiamond.frame = GetDiamondResourceFrameFromID( m_object.socket[i] );
							else
							//	rDiamond.frame = 9;
							    rDiamond.frame = 610;
							//add diamond
							(*it).pCell->AddAnimation( rDiamond, -1, NULL, &rcDiamond, false, CGuiImage::enumUSE_TYPE::USE_TYPE_DIAMOND );
							diamondNum++ ;
						}
					}
				}
			}
			break;
		case 4:
			{
				stRectI rcHolePos[4];
				//add hole
				float gapsX = (sizeItem.x-2.0f*sizeHole) / 3.0f;
				float gapsY = (sizeItem.y-2.0f*sizeHole) / 3.0f;
				rcHolePos[0].SetRect( gapsX, gapsY, gapsX+sizeHole, gapsY+sizeHole );
				rcHolePos[1].SetRect( 2.0f*gapsX+sizeHole, gapsY, sizeItem.x-gapsX, gapsY+sizeHole );
				rcHolePos[2].SetRect( gapsX, 2.0f*gapsY+sizeHole, gapsX+sizeHole, sizeItem.y-gapsY );
				rcHolePos[3].SetRect( 2.0f*gapsX+sizeHole, 2.0f*gapsY+sizeHole, sizeItem.x-gapsX, sizeItem.y-gapsY );
				rcHolePos[0].OffsetRect( offsetCell );
				rcHolePos[1].OffsetRect( offsetCell );
				rcHolePos[2].OffsetRect( offsetCell );
				rcHolePos[3].OffsetRect( offsetCell );
				//Then we add diamond
				int diamondNum = 0;
				for( int i=0;i<4;i++ )
				{
					for(tCells::iterator it = m_cells.begin(); it != m_cells.end(); ++it)
					{
						(*it).pCell->AddAnimation( rHole, -1, NULL, &rcHolePos[i], false, CGuiImage::enumUSE_TYPE::USE_TYPE_HOLE );
						if ( (m_object.socket[i] != -1) && (m_object.socket[i] != 0) )
						{
							//cur hole has diamond
							/*ObjectBase_t* pDiamond = ::GetObjectBase( m_object.socket[i] );
							if ( !pDiamond )
							{
							continue;
							}
							stResourceLocation rDiamond = GetObjectBasePicLocation( pDiamond );*/
							stRectI rcDiamond = rcHolePos[diamondNum];
							if ( GetDiamondResourceFrameFromID( m_object.socket[i] ) >= 0 )
								rDiamond.frame = GetDiamondResourceFrameFromID( m_object.socket[i] );
							else
							//	rDiamond.frame = 9;
							    rDiamond.frame = 610;
							//add diamond
							(*it).pCell->AddAnimation( rDiamond, -1, NULL, &rcDiamond, false, CGuiImage::enumUSE_TYPE::USE_TYPE_DIAMOND );
							diamondNum++ ;
						}
					}
				}
			}

			break;
		case 5:
			{
				stRectI rcHolePos[5];
				//add hole
				float gapsX = (sizeItem.x-2.0f*sizeHole) / 3.0f;
				float gapsY = (sizeItem.y-2.0f*sizeHole) / 3.0f;
				rcHolePos[0].SetRect( gapsX, gapsY, gapsX+sizeHole, gapsY+sizeHole );
				rcHolePos[1].SetRect( 2.0f*gapsX+sizeHole, gapsY, sizeItem.x-gapsX, gapsY+sizeHole );
				rcHolePos[2].SetRect( sizeItem.x/2.0f-sizeHoleHalf, sizeItem.y/2.0f-sizeHoleHalf, sizeItem.x/2.0f+sizeHoleHalf, sizeItem.y/2.0f+sizeHoleHalf );
				rcHolePos[3].SetRect( gapsX, 2.0f*gapsY+sizeHole, gapsX+sizeHole, sizeItem.y-gapsY );
				rcHolePos[4].SetRect( 2.0f*gapsX+sizeHole, 2.0f*gapsY+sizeHole, sizeItem.x-gapsX, sizeItem.y-gapsY );
				rcHolePos[0].OffsetRect( offsetCell );
				rcHolePos[1].OffsetRect( offsetCell );
				rcHolePos[2].OffsetRect( offsetCell );
				rcHolePos[3].OffsetRect( offsetCell );
				rcHolePos[4].OffsetRect( offsetCell );
				//Then we add diamond
				int diamondNum = 0;
				for( int i=0;i<5;i++ )
				{
					for(tCells::iterator it = m_cells.begin(); it != m_cells.end(); ++it)
					{
						(*it).pCell->AddAnimation( rHole, -1, NULL, &rcHolePos[i], false, CGuiImage::enumUSE_TYPE::USE_TYPE_HOLE );
						if ( (m_object.socket[i] != -1) && (m_object.socket[i] != 0) )
						{
							//cur hole has diamond
							/*ObjectBase_t* pDiamond = ::GetObjectBase( m_object.socket[i] );
							if ( !pDiamond )
							{
							continue;
							}
							stResourceLocation rDiamond = GetObjectBasePicLocation( pDiamond );*/
							stRectI rcDiamond = rcHolePos[diamondNum];
							if ( GetDiamondResourceFrameFromID( m_object.socket[i] ) >= 0 )
								rDiamond.frame = GetDiamondResourceFrameFromID( m_object.socket[i] );
							else
							//	rDiamond.frame = 9;
							    rDiamond.frame = 610;
							//add diamond
							(*it).pCell->AddAnimation( rDiamond, -1, NULL, &rcDiamond, false, CGuiImage::enumUSE_TYPE::USE_TYPE_DIAMOND );
							diamondNum++ ;
						}
					}
				}
			}

			break;
		case 6:
			{
				stRectI rcHolePos[6];
				//add hole
				float gapsX = (sizeItem.x-2.0f*sizeHole) / 3.0f;
				float gapsY = (sizeItem.y-3.0f*sizeHole) / 4.0f;
				rcHolePos[0].SetRect( gapsX, gapsY, gapsX+sizeHole, gapsY+sizeHole );
				rcHolePos[1].SetRect( 2.0f*gapsX+sizeHole, gapsY, sizeItem.x-gapsX, gapsY+sizeHole );
				rcHolePos[2].SetRect( gapsX, 2.0f*gapsY+sizeHole, gapsX+sizeHole, 2.0f*(gapsY+sizeHole) );
				rcHolePos[3].SetRect( 2.0f*gapsX+sizeHole, 2.0f*gapsY+sizeHole, sizeItem.x-gapsX, 2.0f*(gapsY+sizeHole) );
				rcHolePos[4].SetRect( gapsX, 3.0f*gapsY+2.0f*sizeHole, gapsX+sizeHole, sizeItem.y-gapsY );
				rcHolePos[5].SetRect( 2.0f*gapsX+sizeHole, 3.0f*gapsY+2.0f*sizeHole, sizeItem.x-gapsX, sizeItem.y-gapsY );
				rcHolePos[0].OffsetRect( offsetCell );
				rcHolePos[1].OffsetRect( offsetCell );
				rcHolePos[2].OffsetRect( offsetCell );
				rcHolePos[3].OffsetRect( offsetCell );
				rcHolePos[4].OffsetRect( offsetCell );
				rcHolePos[5].OffsetRect( offsetCell );
				//Then we add diamond
				int diamondNum = 0;
				for( int i=0;i<6;i++ )
				{
					for(tCells::iterator it = m_cells.begin(); it != m_cells.end(); ++it)
					{
						(*it).pCell->AddAnimation( rHole, -1, NULL, &rcHolePos[i], false, CGuiImage::enumUSE_TYPE::USE_TYPE_HOLE );
						if ( (m_object.socket[i] != -1) && (m_object.socket[i] != 0) )			//
						{
							//cur hole has diamond
							/*ObjectBase_t* pDiamond = ::GetObjectBase( m_object.socket[i] );
							if ( !pDiamond )
							{
							continue;
							}
							stResourceLocation rDiamond = GetObjectBasePicLocation( pDiamond );*/
							stRectI rcDiamond = rcHolePos[diamondNum];
							if ( GetDiamondResourceFrameFromID( m_object.socket[i] ) >= 0 )
								rDiamond.frame = GetDiamondResourceFrameFromID( m_object.socket[i] );
							else
							//	rDiamond.frame = 9;
							    rDiamond.frame = 610;
							//add diamond
							(*it).pCell->AddAnimation( rDiamond, -1, NULL, &rcDiamond, false, CGuiImage::enumUSE_TYPE::USE_TYPE_DIAMOND );
							diamondNum++ ;
						}
					}
				}
			}

			break;
		default:
			break;
		}

	}


	FUNCTION_END;
}	

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CRoleItem::SetImage()
{
	FUNCTION_BEGIN;

	stResourceLocation rl0 = GetItemPicLocation(&m_object);
	stResourceLocation rl1 = GetItemPatternPicLocation(&m_object);
	stResourceLocation rl2 = GetItemPattern2PicLocation(&m_object);

	for(tCells::iterator it = m_cells.begin(); it != m_cells.end(); ++it)
	{
		(*it).pCell->Clear();
		if(!rl0.IsEmpty())
		{
			//Engine_WriteLogF("1物品名: %s\n",this->GetObject()->strName);
			(*it).pCell->AddAnimation(rl0);
		}

		if(!rl1.IsEmpty())
		{
			if ( GetLocation().dwLocation == OBJECTCELLTYPE_EQUIP )
			{
				if ( (GetLocation().y == EQUIPCELLTYPE_BODY)
					|| (GetLocation().y == EQUIPCELLTYPE_HANDR)
					|| (GetLocation().y == EQUIPCELLTYPE_HANDL) )
				{
					DWORD color = -1;
					switch( GetLocation().y )
					{
					case EQUIPCELLTYPE_BODY:
						color = GetObjectBase()->dwColor;
						break;
					case EQUIPCELLTYPE_HANDR:
						color = m_object.color;
						break;
					case EQUIPCELLTYPE_HANDL:
						color = GetScene()->GetMainCharacter()->GetLeftWeaponColor();
						break;
					}
					(*it).pCell->AddAnimation(rl1,  m_object.color);		//color
				}
			}
			else
			{
				(*it).pCell->AddAnimation(rl1, m_object.color);
			}

		}
		if(!rl2.IsEmpty())
		{
			if ( GetLocation().dwLocation == OBJECTCELLTYPE_EQUIP )
			{
				if ( (GetLocation().y == EQUIPCELLTYPE_BODY)
					|| (GetLocation().y == EQUIPCELLTYPE_HANDR)
					|| (GetLocation().y == EQUIPCELLTYPE_HANDL) )
				{
					DWORD color = -1;
					switch( GetLocation().y )
					{
					case EQUIPCELLTYPE_BODY:
						color = GetScene()->GetMainCharacter()->GetBodyCustomColor();
						break;
					case EQUIPCELLTYPE_HANDR:
						color = GetScene()->GetMainCharacter()->GetRightWeaponColor();
						break;
					case EQUIPCELLTYPE_HANDL:
						color = GetScene()->GetMainCharacter()->GetLeftWeaponColor();
						break;
					}
					(*it).pCell->AddAnimation(rl2,  m_object.color);		//color
				}
			}
			else
				(*it).pCell->AddAnimation(rl2, m_object.color);
		}

		if((*it).pCell->IsNullImage(0) && (*it).pCell->IsNullImage(1) && (*it).pCell->IsNullImage(2))
		{
			rl0.SetFileName(c_szItemGrpPack);
			rl0.group = c_nGroupItemCommon;
			rl0.frame = 0;
			(*it).pCell->AddAnimation(rl0);
		}
	}

	FUNCTION_END;
}



/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CRoleItem::SetImage2()
{
	FUNCTION_BEGIN;

	stResourceLocation rl0 = GetItemPicLocation(&m_object);
	stResourceLocation rl1 = GetItemPatternPicLocation(&m_object);
	stResourceLocation rl2 = GetItemPattern2PicLocation(&m_object);

	for(tCells::iterator it = m_cells.begin(); it != m_cells.end(); ++it)
	{
		(*it).pCell->Clear();
		if(!rl0.IsEmpty())
		{
			//Engine_WriteLogF("1物品名: %s\n",this->GetObject()->strName);
			(*it).pCell->AddAnimation(rl0);
		}

		if(!rl1.IsEmpty())
		{
			if ( GetLocation().dwLocation == OBJECTCELLTYPE_EQUIP )
			{
				if ( (GetLocation().y == EQUIPCELLTYPE_BODY)
					|| (GetLocation().y == EQUIPCELLTYPE_HANDR)
					|| (GetLocation().y == EQUIPCELLTYPE_HANDL) )
				{
					DWORD color = -1;
					switch( GetLocation().y )
					{
					case EQUIPCELLTYPE_BODY:
						color = GetObjectBase()->dwColor;
						break;
					case EQUIPCELLTYPE_HANDR:
						color = m_object.color;
						break;
					case EQUIPCELLTYPE_HANDL:
						color = GetScene()->GetMainCharacter()->GetLeftWeaponColor();
						break;
					}
					(*it).pCell->AddAnimation(rl1,  m_object.color);		//color
				}
			}
			else
			{
				(*it).pCell->AddAnimation(rl1, m_object.color);
			}

		}
		if(!rl2.IsEmpty())
		{
			if ( GetLocation().dwLocation == OBJECTCELLTYPE_EQUIP )
			{
				if ( (GetLocation().y == EQUIPCELLTYPE_BODY)
					|| (GetLocation().y == EQUIPCELLTYPE_HANDR)
					|| (GetLocation().y == EQUIPCELLTYPE_HANDL) )
				{
					DWORD color = -1;
					switch( GetLocation().y )
					{
					case EQUIPCELLTYPE_BODY:
						color = GetScene()->GetMainCharacter()->GetBodyCustomColor();
						break;
					case EQUIPCELLTYPE_HANDR:
						color = GetScene()->GetMainCharacter()->GetRightWeaponColor();
						break;
					case EQUIPCELLTYPE_HANDL:
						color = GetScene()->GetMainCharacter()->GetLeftWeaponColor();
						break;
					}
					(*it).pCell->AddAnimation(rl2,  m_object.color);		//color
				}
			}
			else
				(*it).pCell->AddAnimation(rl2, m_object.color);
		}

		if((*it).pCell->IsNullImage(0) && (*it).pCell->IsNullImage(1) && (*it).pCell->IsNullImage(2))
		{
			rl0.SetFileName(c_szItemGrpPack);
			rl0.group = c_nGroupItemCommon;
			rl0.frame = 0;
			(*it).pCell->AddAnimation(rl0);
		}
	}

	FUNCTION_END;
}

int GetStoreTableIndex(const stObjectLocation dst)
{
	int i = CGuiSavingBoxDialog::csShowRowCount;
	if(dst.x < 0	|| dst.x >= CGuiSavingBoxDialog::csShowColCount)
		return -1;
	if(dst.y >= 0	&& dst.y < i )		return 0;
	if(dst.y >= i	&& dst.y < i*2 )	return 1;
	if(dst.y >= i*2 && dst.y < i*3)		return 2;
	return -1;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param dst : 描述
* \return 返回值的描述
*/
void CRoleItem::MoveTo(const stObjectLocation & dst)
{
	FUNCTION_BEGIN;

	stObjectLocation dstLocation = dst;

	ResetLocationIfStore(dstLocation);
	if ( (dstLocation.dwLocation == OBJECTCELLTYPE_NONE) /*&& !g_bSinglePlayer*/ )		//Give up to ground
	{
		//Change cursor to normal status
		RestoreMouseIcon();		
		//Show confirm message box
		CRoleItem* pFloatItem = (CRoleItem*)(GetGuiFloatItem()->GetItem()->m_pItemData);
		if ( pFloatItem )
		{
			if( pFloatItem->IsBind() ) // 不可丢弃类物品主要有灵魂绑定的物品
			{
				GetGameGuiManager()->AddMessageBox("该物品已经被灵魂绑定,不可丢弃!");
			}
			else  if ( pFloatItem->GetItemType() == ItemType_Quest )		//任务物品
			{
				if (!GetQueryManager()->FindListData(eAction_DelItemConfirm,1))
					GetGameGuiManager()->AddMessageBox("任务物品被销毁可能会导致放弃任务，你是否要继续", eAction_DelItemConfirm, NULL,"确定|1|Enter|取消|2|Esc");
			}
			else
			{
				if (!GetQueryManager()->FindListData(eAction_DelItemConfirm,1))
					GetGameGuiManager()->AddMessageBox("是否将物品销毁", eAction_DelItemConfirm, NULL,"确定|1|Enter|取消|2|Esc");
			}
		}
		//GetGuiManager()->SetMouseDlg( NULL );
	}	
	else
	{
		if( dst.dwLocation == OBJECTCELLTYPE_PET )
		{
			if( GetGameGuiManager()->m_guiPetPackDlg &&
				!GetGameGuiManager()->m_guiPetPackDlg->bIsCellCanUse(dst.x,dst.y) )
			{
				return ;
			}
			if (IsBind())
			{
				GetGameGuiManager()->AddClientSystemMessage("已经被灵魂绑定的装备物品不能进行交易！");
				return ;
			}
			if (GetItemType()==ItemType_Arrow)
			{
				GetGameGuiManager()->AddClientSystemMessage("箭支不能进行交易！");
				return ;
			}
			if (GetItemType()==ItemType_Quest)  
			{
				GetGameGuiManager()->AddClientSystemMessage("任务物品不能进行交易！");
				return ;
			}
		}
		stSwapObjectPropertyUserCmd cmd;
		cmd.qwThisID = GetThisID();
		cmd.dst = dstLocation;
		SEND_USER_CMD(cmd);

		if(g_bSinglePlayer)
		{
			if(dst.dwLocation == OBJECTCELLTYPE_NONE)
			{
				// 丢到地上
				stAddMapObjectMapScreenUserCmd cmd1;
				cmd1.data.dwMapObjectTempID = 1;
				cmd1.data.dwObjectID = GetObjectID();
				strcpy(cmd1.data.pstrName,GetName());
				cmd1.data.wdLevel  = 1;
				cmd1.data.wdNumber = 1;
				cmd1.data.x = GetScene()->GetMainCharacter()->GetGridPos().x;
				cmd1.data.y = GetScene()->GetMainCharacter()->GetGridPos().y;
				GetClient()->PushCommand(&cmd1,sizeof(cmd1));

				stRemoveObjectPropertyUserCmd cmd2;
				cmd2.qwThisID = GetThisID();
				GetClient()->PushCommand(&cmd2,sizeof(cmd2));
			}
			else
			{
				GetClient()->PushCommand(&cmd,sizeof(cmd));
			}
		}
	}	

	FUNCTION_END;
}


/**
* \brief 简短描述
* 
* 详细描述
* 
* \param dst : 描述
* \param num : 描述
* \return 返回值的描述
*/
void CRoleItem::SplitTo( const stObjectLocation & dst,DWORD num )
{
	FUNCTION_BEGIN;

	if( dst.dwLocation == OBJECTCELLTYPE_PET )
	{
		if( GetGameGuiManager()->m_guiPetPackDlg &&
			!GetGameGuiManager()->m_guiPetPackDlg->bIsCellCanUse(dst.x,dst.y) )
		{
			return ;
		}
		if (IsBind())
		{
			GetGameGuiManager()->AddClientSystemMessage("已经被灵魂绑定的装备物品不能进行交易！");
			return ;
		}
		if (GetItemType()==ItemType_Arrow)
		{
			GetGameGuiManager()->AddClientSystemMessage("箭支不能进行交易！");
			return ;
		}
		if (GetItemType()==ItemType_Quest)  
		{
			GetGameGuiManager()->AddClientSystemMessage("任务物品不能进行交易！");
			return ;
		}
	}
	stSplitObjectPropertyUserCmd cmd;
	cmd.qwThisID = GetThisID();
	cmd.dst = dst;
	cmd.qwNewThisID = 0;
	if( num > GetCount() )
	{
		Assert(0);
		return;
	}
	cmd.dwNum = num;
	SEND_USER_CMD(cmd);	

	if(g_bSinglePlayer)
	{
		if(!IsMoney())
		{
			CRoleItem* pDstItem = GetScene()->GetMainCharacter()->FindItemByLocation(dst);
			if(pDstItem)
			{
				stRemoveObjectPropertyUserCmd cmd1;
				cmd1.qwThisID = GetThisID();
				GetClient()->PushCommand(&cmd1,sizeof(cmd1));

				stRefCountObjectPropertyUserCmd cmd2;
				cmd2.qwThisID = pDstItem->GetThisID();
				cmd2.dwNum = pDstItem->GetCount() + num;
				GetClient()->PushCommand(&cmd2,sizeof(cmd2));
			}
			else
			{
				cmd.qwNewThisID = s_dwMaxThisID+1;
				GetClient()->PushCommand(&cmd,sizeof(cmd));
			}
		}
	}

	FUNCTION_END;
}

enmCallDutyType CRoleItem::IsCallDutyItem()
{
	enmCallDutyType type = CALL_DUTY_NUM;
	switch( GetObjectID() )
	{
	case 841:
		type = CALL_DUTY_KING;
		break;
	case 821:
		type = CALL_DUTY_UNION;
		break;
	case 831:
		type = CALL_DUTY_SEPT;
		break;
	}

	return type;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CRoleItem::Sell()
{
	FUNCTION_BEGIN;

	PlayItemSound(g_nMoneySound);	// 播放钱的声音


	stSellObjectNpcTradeUserCmd cmd;
	cmd.qwThisID = GetThisID();
	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CRoleItem::Save()
{
	FUNCTION_BEGIN;



	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
bool CRoleItem::NeedConsiderDur()
{
	return ( (GetLocation().dwLocation == OBJECTCELLTYPE_EQUIP) && (m_object.maxdur>0) && !IsCanUse() && !IsArrow() && (m_pObjectBase->dwType != ItemType_Change) );		// 
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
//bool CRoleItem::IsHorse()
//{
//	FUNCTION_BEGIN;
//
//	return ( (m_pObjectBase->dwID >= 737) && (m_pObjectBase->dwID <= 737) );		
//
//	FUNCTION_END;
//}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
bool CRoleItem::IsArrow()
{
	FUNCTION_BEGIN;

	return ISARRAW(m_pObjectBase->dwType);

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
DWORD  CRoleItem::GetMaxCount()
{
	FUNCTION_BEGIN;

	return m_pObjectBase->dwMaxCount;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pDestItem : 描述
* \return 返回值的描述
*/
void CRoleItem::UnionTo(CRoleItem* pDestItem)
{
	FUNCTION_BEGIN;

	Assert(pDestItem);
	stUnionObjectPropertyUserCmd cmd;
	cmd.qwSrcThisID = GetThisID();
	cmd.qwDstThisID = pDestItem->GetThisID();
	SEND_USER_CMD(cmd);

	if( g_bSinglePlayer )
	{
		DWORD dwDestNum = getMin(pDestItem->GetCount() + GetCount(),pDestItem->GetMaxCount());
		DWORD dwMoveNum = dwDestNum - pDestItem->GetCount();
		DWORD dwSrcNum = GetCount() - dwMoveNum;

		stRefCountObjectPropertyUserCmd cmd;
		cmd.qwThisID = pDestItem->GetThisID();
		cmd.dwNum = dwDestNum;
		GetClient()->PushCommand(&cmd,sizeof(cmd));

		if(dwSrcNum == 0)
		{
			stRemoveObjectPropertyUserCmd cmd;
			cmd.qwThisID = GetThisID();
			GetClient()->PushCommand(&cmd,sizeof(cmd));
		}
		else
		{
			stRefCountObjectPropertyUserCmd cmd;
			cmd.qwThisID = GetThisID();
			cmd.dwNum = dwSrcNum;
			GetClient()->PushCommand(&cmd,sizeof(cmd));
		}
	}

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param rcBBox : 描述
* \return 返回值的描述
*/
void CRoleItem::ShowToolTipsTopLeftPrefer( stRectI rcBBox )
{
	FUNCTION_BEGIN;

	if(GetGameState()->IsStateItem(eGameStateItem_NpcTrade))
	{
		if(GetLocation().dwLocation == OBJECTCELLTYPE_COMMON || 
			GetLocation().dwLocation == OBJECTCELLTYPE_TRADE)
		{
			CGuiNpcTrade* pDlg = GetGameGuiManager()->m_guiNpcTrade;
			Assert(pDlg);
			if(pDlg)
			{
				SetTipFlags(GetTipFlags() | ITEMTIPS_FLAG_PRICE);
				if(pDlg->IsBuyObject(GetObjectID()))
				{
					SetTipFlags(GetTipFlags() | ITEMTIPS_FLAG_PRICE);
				}

				if(pDlg->IsSellObject(GetObjectID()))
				{
					SetTipFlags(GetTipFlags() | ITEMTIPS_FLAG_PRICE);
				}

				if(pDlg->IsRepairObject(GetObjectID()))
				{
					SetTipFlags(GetTipFlags() | ITEMTIPS_FLAG_REPAIR_PRICE);
				}
			}
		}
		else if(GetLocation().dwLocation == OBJECTCELLTYPE_SELL)
		{
			SetTipFlags(GetTipFlags() | ITEMTIPS_FLAG_PRICE);
		}

		SetTipFlags(GetTipFlags() & (~ITEMTIPS_FLAG_RIGHT_CLICK_USE));
	}
	else
	{
		SetTipFlags(GetTipFlags() & (~ITEMTIPS_FLAG_PRICE));
		SetTipFlags(GetTipFlags() & (~ITEMTIPS_FLAG_REPAIR_PRICE));
		SetTipFlags(GetTipFlags() & (~ITEMTIPS_FLAG_REPAIR_PRICEBYGOLD));
		SetTipFlags(GetTipFlags() | ITEMTIPS_FLAG_RIGHT_CLICK_USE);
	}

	if(m_bNeedUpdateTips)
	{
		UpdateToolTips();
		m_bNeedUpdateTips = false;
	}

	m_ToolTips.RenderTopLeftPrefer( rcBBox, GetDevice()->GetWidth(), GetDevice()->GetHeight() );

	/*if (NULL == GetGuiManager()->GetToolTips() )
	GetGuiManager()->SetToolTips(&m_ToolTips,pt);*/

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
 //soke 取消装备比较
void CRoleItem::ShowToolTips(POINT pt)
{
	FUNCTION_BEGIN;

	if(GetGameState()->IsStateItem(eGameStateItem_NpcTrade))
	{
		if(GetLocation().dwLocation == OBJECTCELLTYPE_COMMON || 
			GetLocation().dwLocation == OBJECTCELLTYPE_TRADE)
		{
			CGuiNpcTrade* pDlg = GetGameGuiManager()->m_guiNpcTrade;
			Assert(pDlg);
			if(pDlg)
			{
				SetTipFlags(GetTipFlags() | ITEMTIPS_FLAG_PRICE);
				if(pDlg->IsBuyObject(GetObjectID()))
				{
					SetTipFlags(GetTipFlags() | ITEMTIPS_FLAG_PRICE);
				}

				if(pDlg->IsSellObject(GetObjectID()))
				{
					SetTipFlags(GetTipFlags() | ITEMTIPS_FLAG_PRICE);
				}

				if(pDlg->IsRepairObject(GetObjectID()))
				{
					SetTipFlags(GetTipFlags() | ITEMTIPS_FLAG_REPAIR_PRICE);
				}
			}
		}
		else if(GetLocation().dwLocation == OBJECTCELLTYPE_SELL)
		{
			SetTipFlags(GetTipFlags() | ITEMTIPS_FLAG_PRICE);
		}
	}
	else
	{
		if ( m_object.upgrade < 10 )
		{
			SetTipFlags(GetTipFlags() & (~ITEMTIPS_FLAG_PRICE));
			SetTipFlags(GetTipFlags() & (~ITEMTIPS_FLAG_REPAIR_PRICE));
			SetTipFlags(GetTipFlags() | ITEMTIPS_FLAG_RIGHT_CLICK_USE);
		}                                                              		
	}

	if(m_bNeedUpdateTips)
	{
		UpdateToolTips();
		m_bNeedUpdateTips = false;
	}

	if (NULL == GetGuiManager()->GetToolTips() )
		GetGuiManager()->SetToolTips(&m_ToolTips,pt);

	//m_ToolTips.Render(pt.x,pt.y);

	/**
	* \brief 简短描述
	* 
	* 详细描述
	* 
	* \return 返回值的描述
	*/
	FUNCTION_END;
}

#define Tips_AddStringDefFont(szText) \
	if( (szText)[0] ) {\
	\
	_snprintf(szTemp,sizeof(szTemp),"%s",szText);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText(szTemp,c_nDefTipFont);\
	}\

#define Tips_AddStringDefFontNAME(szText) \
	if( (szText)[0] ) {\
	\
	_snprintf(szTemp,sizeof(szTemp),"%s",szText);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText1(szTemp,c_nDefTipFont);\
	}\

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
#define Tips_AddString(szText) \
	if( (szText)[0] ) {\
	\
	_snprintf(szTemp,sizeof(szTemp),"%s",szText);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText(szTemp,0);\
	}\

//桃子增加第一框页面不限宽度	
#define Tips_AddString3(szText) \
	if( (szText)[0] ) {\
	\
	_snprintf(szTemp,sizeof(szTemp),"%s",szText);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText3(szTemp,0);\
	}\

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
#define Tips_AddText(szFormat,szText) \
	if( (szText)[0] ) {\
	\
	_snprintf(szTemp,sizeof(szTemp),szFormat,szText);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText(szTemp,0);\
	}\

#define Tips_AddText1(szFormat,szText) \
	if( (szText)[0] ) {\
	\
	_snprintf(szTemp,sizeof(szTemp),szFormat,szText);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText1(szTemp,0);\
	}\

#define Tips_AddText2(szFormat,szText) \
	if( (szText)[0] ) {\
	\
	_snprintf(szTemp,sizeof(szTemp),szFormat,szText);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText1(szTemp,0);\
	}\

#define Tips_AddNum(szFormat,nNum)\
	if(nNum){\
	_snprintf(szTemp,sizeof(szTemp),szFormat,nNum);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText(szTemp,0);\
	}\
// myy 新增不限制字符串长度
#define Tips_AddNum1(szFormat,nNum)\
	if(nNum){\
	_snprintf(szTemp,sizeof(szTemp),szFormat,nNum);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText1(szTemp,0);\
	}\

#define Tips_AddAllNum(szFormat,nNum)\
{\
	_snprintf(szTemp,sizeof(szTemp),szFormat,nNum);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText(szTemp,0);\
}\

#define Tips_AddNum2(szFormat,nNum1,nNum2)\
	if(nNum1 || nNum2){\
	_snprintf(szTemp,sizeof(szTemp),szFormat,nNum1,nNum2);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText(szTemp,0);\
	}\

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pSkill : 描述
* \param m_ToolTips : 描述
* \param pRoleSkill : 描述
* \return 返回值的描述
*/
void UpdateSkillToolTips(SkillBase_t* pSkill,CToolTips & m_ToolTips,CRoleSkill* pRoleSkill)
{
	FUNCTION_BEGIN;

	if ( pSkill == NULL )
		return;

	//make sure we got role skill
	if ( pRoleSkill == NULL )
		pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( pSkill->dwID );

	DWORD colorRed = COLOR_ARGB(255,255,90,0);
	DWORD colorWhite = COLOR_ARGB(255,255,255,255); //技能名字，释放动作，是否上马，冷却时间
	DWORD colorBlue = COLOR_ARGB(255,0,170,255);
	DWORD colorGreen = COLOR_ARGB(255,0,255,0);  //需要武器颜色
	DWORD colorYellow = COLOR_ARGB(255,202,166,10); //技能说明颜色

	m_ToolTips.Clear();

	m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
	m_ToolTips.SetAlignType( GUI_ALIGN_LEFT );
	m_ToolTips.SetLineGaps( 3 );

	
	char szTemp[512];
	
	m_ToolTips.SetCurColor(colorGreen);
	#ifdef _DEBUG
	Tips_AddStringDefFont(pSkill->pstrName); //技能名称
	Tips_AddNum("[%u]",pSkill->dwID);  //技能ID
	#else
	//soke 增加已学习技能显示等级

	char szTmp[512];
	if (pRoleSkill)
	{
		sprintf(szTmp, "%s(%u级)", pSkill->pstrName, pRoleSkill->GetLevel());
		Tips_AddStringDefFont( szTmp );
	}
	else
	{
		Tips_AddStringDefFont(pSkill->pstrName);
	}
	//soke end
	#endif

	//Add use method
	/*if ( (pSkill->dwTarget == SkillTarget_Self) && (pSkill->dwType != SkillType_Passive) )
	Tips_AddString( "\n瞬间施放" );
	if	( (pSkill->dwTarget & SkillTarget_Single)
	|| (pSkill->dwTarget & SkillTarget_Place)
	|| (pSkill->dwTarget & SkillTarget_Npc)
	|| (pSkill->dwTarget & SkillTarget_Enemy) )
	{
	if ( pSkill->dwType == SkillType_Attack )
	{
	Tips_AddString( "\n右键施放" );
	}
	else if ( pSkill->dwType == SkillType_Active )
	{
	Tips_AddString( "\n右键连续施放" );
	}
	}

	if ( !(pSkill->dwTarget & SkillTarget_Place) && (pSkill->dwType == SkillType_StateAttack) )
	{
		Tips_AddString( "\n选中后左键施放" );	
	}*/
	if ( pSkill->dwType != SkillType_Passive )
	{
		//Add use mothod
		if ( 1 )
		{
			m_ToolTips.SetCurColor(colorWhite); //白色显示
			if ( (pSkill->dwTarget == SkillTarget_Self) && (pSkill->dwType != SkillType_Passive) )
				Tips_AddString( "\n瞬间施放" );
			if	( (pSkill->dwTarget == 42)
				|| (pSkill->dwTarget == 44) )
			{
				if ( pSkill->dwType == SkillType_Attack )
				{
					Tips_AddString( "\n右键施放" );
				}
				else if ( pSkill->dwType == SkillType_Active )
				{
					Tips_AddString( "\n右键连续施放" );
				}
			}

			if ( ( (pSkill->dwTarget == 19) || (pSkill->dwTarget == 42) || (pSkill->dwTarget == 83) )
				&& (pSkill->dwType == SkillType_StateAttack) )
			{
				Tips_AddString( "\n选中后左键施放" );	
			}
		}
		else
		{
			Tips_AddString( "\n施放方式：" );
			int nMethod = 0;
			if ( pSkill->dwUseMethod & SkillUseMethod_Self )
				Tips_AddNum( "\n    %u 自身施放", nMethod++ );

			if ( pSkill->dwUseMethod & SkillUseMethod_BlueHand )
				Tips_AddNum( "\n    %u 使用技能快捷键或者左键点击技能图标,待鼠标变色后点击施放到目标上", nMethod++ );

			if ( pSkill->dwUseMethod & SkillUseMethod_SelectFirst )
				Tips_AddNum( "\n    %u 先鼠标左键或者shift+左键选中目标,然后使用技能热键或者点击技能图标施放到目标上", nMethod++ );

			if ( pSkill->dwUseMethod & SkillUseMethod_Lock )
				Tips_AddNum( "\n    %u 点击技能图标或者使用技能热键选中此技能,然后右键点击目标施放", nMethod++ );
		}
		
		//Add attack range
		if ( pSkill->dwID == SKILL_THREATEN )
		{
			Tips_AddString( "\n攻击距离：周围八格" );
		}
		else
		{
			switch( pSkill->dwDis )
			{
			case 0:
				Tips_AddString( "\n攻击距离:自身施放" );
				break;
			case 1:
				Tips_AddString( "\n攻击距离:面对方向一格" );
				break;
			case 3:
				Tips_AddString( "\n攻击距离:面对方向三格" );
				break;
			case 11:			
				Tips_AddString( "\n攻击距离:本屏内" );
				break;
			}
		}
	}
	

	/*if ( pSkill->dwType == SkillType_Attack )
	Tips_AddString( "\n法术攻击" );
	if ( pSkill->dwType == SkillType_Active )
	Tips_AddString( "\n物理攻击" );*/

	if ( pSkill->dwType != SkillType_Passive )
	{
		if ( pSkill->dwAllowRide )
		{
			Tips_AddString( "\n允许骑马使用" );
		}
		else
		{
			Tips_AddString( "\n不可骑马使用" );
		}
	}

	//end use method

	CMainCharacter* pMainData = GetScene()->GetMainCharacter();
	int nSkillLevel = (pRoleSkill == NULL)? 0 : pRoleSkill->GetTotalLevel();
	
	if (nSkillLevel != 0)
	{
		if (!pMainData)
			goto end;

		SkillLevelBase_t* pSkillLevelUp = GetSkillLevelBase( pSkill->dwID, nSkillLevel );
		if ( pSkillLevelUp == NULL )
			goto end;

		Tips_AddString( "\n" );

		//need level
		if ( pSkillLevelUp->dwNeedSkill3Level > GetScene()->GetMainCharacter()->GetLevel() )
			m_ToolTips.SetCurColor( colorRed );
		else
			m_ToolTips.SetCurColor( colorGreen );
		Tips_AddNum("\n需要等级 %u", pSkillLevelUp->dwNeedSkill3Level );

		//cool time
		m_ToolTips.SetCurColor(colorWhite);
		if (pSkill->dwIntervalTime>0)
			{
				//Tips_AddNum("\n冷却时间:%.2f秒",(float)pSkillLevelUp->dwIntervalTime/1000);

				int leftTime = (DWORD)pSkillLevelUp->dwIntervalTime/1000;

				if ( leftTime <= 60 )
				{
					Tips_AddNum( "\n冷却时间:%.2f秒", (float)leftTime );
				}
				else if ( leftTime <= 3600 )
				{
					Tips_AddNum("\n冷却时间:%u分钟", leftTime/60);
					Tips_AddNum("%02u秒", leftTime%60);
				}
				else
				{
					Tips_AddNum("\n冷却时间:%u小时", leftTime/3600);
					Tips_AddNum("%02u分钟", leftTime%3600/60);
					Tips_AddNum("%02u秒", leftTime%3600%60);
				}
			}
		//Tips_AddString("\n");
		Tips_AddNum("\n生命值消耗:%u点",pSkillLevelUp->dwHP);

		Tips_AddNum("\n法术值消耗:%u点",pSkillLevelUp->dwMP);

		//Tips_AddNum("\n体力值消耗:%u点",pSkillLevelUp->dwSP);

		Tips_AddNum("\n单体伤害损耗 %u%%", pSkillLevelUp->dwHurtAdd);

		m_ToolTips.SetCurColor(colorYellow); //技能说明
		Tips_AddText("\n%s", pSkillLevelUp->strDesc);

		//Add cur disable statu
		m_ToolTips.SetCurColor(colorWhite);
		{
			int nTime = 0;
			nTime = pSkillLevelUp->dwIntervalTime - ( xtimeGetTime() - GetScene()->GetMainCharacter()->GetSkillStartTime( pSkill ) );
			nTime /= 1000;
			if ( nTime > 0 )
				{
					if ( nTime <= 60 )
					{
						Tips_AddNum( "\n剩余时间：%u秒", nTime);
					}
					else if ( nTime <= 3600 )
					{
						Tips_AddNum("\n冷却时间:%u分钟", nTime/60);
						Tips_AddNum("%02u秒", nTime%60);
					}
					else
					{
						Tips_AddNum("\n冷却时间:%u小时", nTime/3600);
						Tips_AddNum("%02u分钟", nTime%3600/60);
						Tips_AddNum("%02u秒", nTime%3600%60);
					}
				}
			}
		//Tips_AddNum( "\n剩余时间：%u秒", (int)( (1.0f -GetScene()->GetMainCharacter()->GetSkillDisableStatus( pSkill ) ) * pSkillLevelUp->dwIntervalTime/1000 ) );
	}

	//Add item cost
	{
		int curLevel = nSkillLevel;
		if ( curLevel < 1 )
			curLevel = 1;

		SkillLevelBase_t* pSkillCurLevelBase = GetSkillLevelBase( pSkill->dwID, curLevel );
		if ( pSkillCurLevelBase )
		{
			ObjectBase_t* pObject = NULL;
			pObject = GetObjectBase(pSkillCurLevelBase->dwCostID);
			if (pObject)
			{
				CRoleItem* pRoleItem = pMainData->FindItemByConstID( pObject->dwID );
				if( pRoleItem )
				{
					if ( pRoleItem->GetExteriorCount() < pSkillCurLevelBase->dwCostNum ) 
						m_ToolTips.SetCurColor( colorRed );
					else
						m_ToolTips.SetCurColor( colorGreen );
				}
				else
					m_ToolTips.SetCurColor( colorRed );

				Tips_AddText("\n需要%s ", pObject->strName );
				Tips_AddNum( "%u", pSkillCurLevelBase->dwCostNum );
			}
		}
	}

	// up level

	bool bShowNextLevel = true;
	if ( pRoleSkill )
	{
		if ( pRoleSkill->GetLevel() >= MAX_SKILL_LEVEL )
			bShowNextLevel = false;
	}
	if ( bShowNextLevel )		//nSkillLevel < MAX_SKILL_LEVEL
	{
		SkillLevelBase* pSkillLevelBase = GetSkillLevelBase(pSkill->dwID,++nSkillLevel);

		if ((pMainData==NULL)||(pSkillLevelBase==NULL))
			goto end;
		
		m_ToolTips.AddText( "\n" );
		m_ToolTips.AddText( "\n下一等级" );

		//need level
		if ( pSkillLevelBase->dwNeedSkill3Level > GetScene()->GetMainCharacter()->GetLevel() )
			m_ToolTips.SetCurColor( colorRed );
		else
			m_ToolTips.SetCurColor( colorGreen );
		Tips_AddNum("\n需要等级 %u", pSkillLevelBase->dwNeedSkill3Level );
        
		m_ToolTips.SetCurColor(colorWhite);
		if (pSkillLevelBase->dwIntervalTime>0)
			Tips_AddNum("\n冷却时间:%.2f秒",(float)pSkillLevelBase->dwIntervalTime/1000);

		Tips_AddNum("\n生命值消耗:%u点  ",pSkillLevelBase->dwHP);

		Tips_AddNum("\n法术值消耗:%u点  ",pSkillLevelBase->dwMP);

		//Tips_AddNum("\n体力值消耗:%u点",pSkillLevelBase->dwSP);

		Tips_AddNum("\n单体伤害损耗 %u%%", pSkillLevelBase->dwHurtAdd);

		m_ToolTips.SetCurColor(colorYellow);
		Tips_AddText("\n%s", pSkillLevelBase->strDesc);

		if (pSkillLevelBase->dwUpItemID!=0)
		{
			if (pMainData->GetItemCount(pSkillLevelBase->dwUpItemID)>0)
				m_ToolTips.SetCurColor(colorGreen);
			else
				m_ToolTips.SetCurColor(colorRed);

			ObjectBase_t* pObject = NULL;
			pObject = GetObjectBase(pSkillLevelBase->dwUpItemID);
			if (pObject)
				Tips_AddText("\n需要%s", pObject->strName );
		}		

		// Get pre skill 
		if ( pSkillLevelBase->dwNeedSkill1 )
		{
			SkillBase_t* pNeedSkill = NULL;
			pNeedSkill = GetSkillBase( pSkillLevelBase->dwNeedSkill1 );
			if ( pNeedSkill )
			{
				CRoleSkill* pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( pNeedSkill->dwID );
				if ( pRoleSkill && (pSkillLevelBase->dwNeedSkill1Level <= pRoleSkill->GetLevel()) )
					m_ToolTips.SetCurColor(colorGreen);
				else
					m_ToolTips.SetCurColor(colorRed);

				if ( pSkillLevelBase->dwNeedSkill1Level > 0 )
				{
					Tips_AddText( "\n需要%s:", pNeedSkill->pstrName );
					Tips_AddNum( " %u点", pSkillLevelBase->dwNeedSkill1Level );
				}				
			}
		}
		if ( pSkillLevelBase->dwNeedSkill2 )
		{
			SkillBase_t* pNeedSkill = NULL;
			pNeedSkill = GetSkillBase( pSkillLevelBase->dwNeedSkill2 );
			if ( pNeedSkill )
			{
				CRoleSkill* pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( pNeedSkill->dwID );
				if ( pRoleSkill && (pSkillLevelBase->dwNeedSkill2Level <= pRoleSkill->GetLevel()) )
					m_ToolTips.SetCurColor(colorGreen);
				else
					m_ToolTips.SetCurColor(colorRed);

				if ( pSkillLevelBase->dwNeedSkill2Level > 0 )
				{
					Tips_AddText( "\n需要%s:", pNeedSkill->pstrName );
					Tips_AddNum( " %u点", pSkillLevelBase->dwNeedSkill2Level );
				}				
			}
		}
		/*if ( pSkillLevelBase->dwNeedSkill3 )
		{
		SkillBase_t* pNeedSkill = NULL;
		pNeedSkill = GetSkillBase( pSkillLevelBase->dwNeedSkill3 );
		if ( pNeedSkill )
		{
		CRoleSkill* pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( pNeedSkill->dwID );
		if ( pRoleSkill && (pSkillLevelBase->dwNeedSkill3Level <= pRoleSkill->GetLevel()) )
		m_ToolTips.SetCurColor(colorGreen);
		else
		m_ToolTips.SetCurColor(colorRed);

		if ( pSkillLevelBase->dwNeedSkill3Level > 0 )
		{
		Tips_AddText( "\n需要%s:", pNeedSkill->pstrName );
		Tips_AddNum( " %u点\n", pSkillLevelBase->dwNeedSkill3Level );
		}				
		}
		}*/

		//need tree points
		int treeSkillPoints = 0xffff;
		char szDesc[64];
		if ( GetGameGuiManager()->m_guiSkill )
		{
			treeSkillPoints = GetGameGuiManager()->m_guiSkill->GetSerialSkillPoints( pSkillLevelBase->dwSerialType, pSkillLevelBase->dwTreeType );
			GetGameGuiManager()->m_guiSkill->GetNeedPointsDesc( pSkillLevelBase->dwSerialType, pSkillLevelBase->dwTreeType, szDesc );
		}

		if ( pSkillLevelBase->dwSkillPoint > treeSkillPoints )
			m_ToolTips.SetCurColor( colorRed );
		else
			m_ToolTips.SetCurColor( colorGreen );

		if ( pSkillLevelBase->dwSkillPoint > 0 )
		{
			Tips_AddText( "\n需要%s点数: ", szDesc );
			Tips_AddNum( "%u 点", pSkillLevelBase->dwSkillPoint );
		}		
	}


	{
		//Add need weapon
		std::vector<UINT, std::stack_allocator<UINT> > aid;
		aid.reserve(16);
		_parse_str_num(aid,pSkill->pstrConsume);
		static const char* weaponType[] = { "刀类", "剑类", "斧类", "锤类", "杖类", "弓类", "扇类", "棍类", "利刃类", "剑类", "枪类" };		//104 ~ 111
		
		if ( aid.size() > 0 )
		{
			//First we make sure wether we have corresponding  weapon
			{
				int count = 0;
				for( size_t i=0;i<aid.size();i++ )
				{
					if ( (aid[i] >= 104&& aid[i] <= 111) || (aid[i] == 136)|| (aid[i] == 137)|| (aid[i] == 155) )
					{
						count += pMainData->GetItemExteriorCountByType( aid[i] );
					}
				}

				if ( count > 0 )
					m_ToolTips.SetCurColor( colorGreen );
				else
					m_ToolTips.SetCurColor( colorRed );
			}

			Tips_AddString( "\n需要如下武器类型之一：" );
			for(size_t i=0;i<aid.size();i++)
			{
				if ( (aid[i] >= 104) && (aid[i] <= 111) )
				{
					Tips_AddString( weaponType[aid[i]-104] );
					if ( i < (aid.size() -1) )
						Tips_AddString( "、" );
				}
				else if ( aid[i] == 136 || aid[i] == 137)
				{
					Tips_AddString( weaponType[aid[i]-128] );
					Tips_AddString( "、" );
				}
				else if ( (aid[i] == 155) )
				{
					Tips_AddString( weaponType[aid[i]-145] );
					Tips_AddString( "、" );
				}
			}
		}
	}
		
end:
	m_ToolTips.Resize();

	FUNCTION_END;
}

template <class tRecord>
bool DataGreatByValue(tRecord& data1,tRecord & data2)
{
	FUNCTION_BEGIN;

	if ( data1.dwPoints >= data2.dwPoints )
	{
		if ( data1.dwPoints == data2.dwPoints  )
			return data1.dwID > data2.dwID;

		return true;
	}
	
	return false;

	FUNCTION_END;
}

void UpdateSkillUpItemToolTips( t_Object& pObject, CToolTips& m_ToolTips, DWORD m_dwTipFlags )
{
	FUNCTION_BEGIN;

	DWORD colorRed = COLOR_ARGB(255,255,90,0);
	DWORD colorWhite = COLOR_ARGB(255,255,255,255);
	DWORD colorBlue = COLOR_ARGB(255,0,170,255);
	DWORD colorGreen = COLOR_ARGB(255,0,255,0);
	DWORD colorYellow = COLOR_ARGB(255,202,166,10);

	m_ToolTips.Clear();
	m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
	m_ToolTips.SetAlignType( GUI_ALIGN_LEFT );
	m_ToolTips.SetLineGaps( 3 );
	m_ToolTips.SetCurColor( colorWhite );

	char szTemp[512];

	Tips_AddStringDefFont(pObject.strName);

	//价格
	if(m_dwTipFlags & ITEMTIPS_FLAG_PRICE)
	{
		Tips_AddString( "\n" );
		if ( pObject.pos.dwLocation == OBJECTCELLTYPE_SELL )
		{
			//Buy price
			//DWORD buyPrice = GetPrice( ITEM_PRICE_BUY );
			float buyPrice = pObject.price;
			//Make some adjust according to character goodness
			DWORD goodness = GetScene()->GetMainCharacter()->GetGoodnessAfterAdjust();
			switch(goodness)
			{
			case GOODNESS_3:
				buyPrice *= 1.2f;
				break;
			case GOODNESS_4:
				buyPrice *= 1.5f;
				break;
			case GOODNESS_5:
			case GOODNESS_6:
				buyPrice *= 2.0f;
				break;
			}
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n购买价格 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddText( "%s", Money2String( (DWORD)(buyPrice*pObject.dwNum + 0.99f) ) );
		}
		else if ( pObject.pos.dwLocation == OBJECTCELLTYPE_MYSHOP )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n销售价格 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddText( "%s", Money2String( GetItemTotalPrice(pObject) ) );
		}
		else if ( pObject.pos.dwLocation == OBJECTCELLTYPE_OTHERSSHOP)
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n购买价格 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddText( "%s", Money2String( GetItemTotalPrice(pObject) ) );
		}
		else
		{
			//Sell price
			//DWORD sellPrice = m_object.GetPrice( ITEM_PRICE_SELL );
			float sellPrice = GetObjectSellMoneyByDur(&pObject);
			////Consider dur
			//if ( m_object.maxdur > 0 )
			//{
			//	if ( (m_pObjectBase->dwType == ItemType_Arrow) )
			//		sellPrice *= m_object.dur/m_object.maxdur;
			//	else
			//		sellPrice *= ((m_object.dur/25)*25) / m_object.maxdur;
			//}
			//
			////Make some adjust according to character goodness
			//sellPrice *= 0.50f;		
			DWORD goodness = GetScene()->GetMainCharacter()->GetGoodnessAfterAdjust();
			switch(goodness)
			{
			case GOODNESS_3:
				sellPrice *= 0.7f;
				break;
			case GOODNESS_4:
				sellPrice *= 0.4f;
				break;
			case GOODNESS_5:
			case GOODNESS_6:
				sellPrice *= 0.1f;
				break;
			}
			//Tips_AddNum("\n价格 %u", (DWORD)(sellPrice + 0.99f));		//GetPrice()
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n出售价格 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddText( "%s", Money2String( (DWORD)(sellPrice*pObject.dwNum + 0.99f) ) );
		}

	}

	//Get object base
	ObjectBase_t* pObjectBase = GetObjectBase( pObject.dwObjectID );

	if ( NULL == pObjectBase )
	{
		m_ToolTips.Resize();
		return;
	}

	//See if we already study this skill
	DWORD skillID = GetSkillIdFromBook( pObject.dwObjectID );
	SkillBase_t* pSkill = GetSkillBase( skillID );

	if ( NULL == pSkill )
	{
		m_ToolTips.Resize();
		return;
	}

	CRoleSkill* pRoleSkill = NULL;
	if ( skillID != -1 )
	{
		pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( skillID );
		if ( pRoleSkill )
		{
			bool bStudied = true;
			if ( pSkill->dwSerialType == SKILL_SERIAL_SPECIAL )
			{
				DWORD level = GetSkillLevelFromBook( pObject.dwObjectID );
				if ( level == -1 )
					bStudied = false;

				if ( level > pRoleSkill->GetLevel() )
					bStudied = false;

				if ( bStudied )
				{
					m_ToolTips.SetCurColor( colorRed );
					Tips_AddString( "\n已学会" );
				}
			}			
		}
		else
		{
			//Tips_AddString( "\n" );			
		}
	}
	else
	{
		m_ToolTips.Resize();
		return;
	}

	//Add skill serial and tree name
	m_ToolTips.SetCurColor( colorWhite );
	if ( (NULL != GetSkillSerialName(pSkill)) && (NULL != GetSkillTreeName(pSkill)) )
	{
		char szName[256];
		sprintf( szName, GetSkillSerialName(pSkill) );
		Tips_AddText( "\n%s", szName );
		szName[0] = 0L;
		if ( pSkill->dwType == SkillType_Passive )
			sprintf( szName, "%s被动技能", GetSkillTreeName(pSkill) );
		else
			sprintf( szName, "%s技能", GetSkillTreeName(pSkill) );
		Tips_AddString( szName );
		//Tips_AddString( "\n" );
	}

	//Show item description
	if ( ( strcmp( pObjectBase->strDesc, "无" ) != 0 )
		&& ( strcmp( pObjectBase->strDesc, "" ) != 0 ) )
	{
		m_ToolTips.SetCurColor( colorWhite );
		Tips_AddText( "\n%s", pObjectBase->strDesc );
	}

	//
	//Tips_AddString( "\n" );

	//Need skill
	//see if all pre skill has studied
	SkillLevelBase_t* pSkillLevel = NULL;
	if ( pRoleSkill )
		pSkillLevel = GetSkillLevelBase( skillID, pRoleSkill->GetLevel() + 1 );
	else
		pSkillLevel = GetSkillLevelBase( skillID, 1 );

	if ( NULL == pSkillLevel )
	{
		m_ToolTips.Resize();
		return;
	}

	if ( (pSkill->dwNeedSkill1 && pSkillLevel->dwNeedSkill1Level)
		|| (pSkill->dwNeedSkill2 && pSkillLevel->dwNeedSkill2Level)
		|| (pSkill->dwNeedSkill3 && pSkillLevel->dwNeedSkill3Level) )
	{
		Tips_AddString( "\n学习此技能需要:" );
	}

	if ( pSkill->dwNeedSkill1 && pSkillLevel->dwNeedSkill1Level )
	{
		CRoleSkill* pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( pSkill->dwNeedSkill1 );
		if( pRoleSkill )
		{
			if ( pRoleSkill->GetLevel() < pSkillLevel->dwNeedSkill1Level )
				m_ToolTips.SetCurColor( colorRed );
			else
				m_ToolTips.SetCurColor( colorGreen );

			Tips_AddText( "\n%s ", pRoleSkill->GetName() );
			Tips_AddNum( "%u级", pSkillLevel->dwNeedSkill1Level );
		}
		else
		{
			SkillBase_t* pNeedSkill1 = GetSkillBase( pSkill->dwNeedSkill1 );
			if ( pNeedSkill1 )
			{
				m_ToolTips.SetCurColor( colorRed );
				Tips_AddText( "\n%s ", pNeedSkill1->pstrName );
				Tips_AddNum( "%u级", pSkillLevel->dwNeedSkill1Level );
			}
		}
	}
	if ( pSkill->dwNeedSkill2 && pSkillLevel->dwNeedSkill2Level )
	{
		CRoleSkill* pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( pSkill->dwNeedSkill2 );
		if( pRoleSkill )
		{
			if ( pRoleSkill->GetLevel() < pSkillLevel->dwNeedSkill2Level )
				m_ToolTips.SetCurColor( colorRed );
			else
				m_ToolTips.SetCurColor( colorGreen );

			Tips_AddText( "\n%s ", pRoleSkill->GetName() );
			Tips_AddNum( "%u级", pSkillLevel->dwNeedSkill2Level );
		}
		else
		{
			SkillBase_t* pNeedSkill2 = GetSkillBase( pSkill->dwNeedSkill2 );
			if ( pNeedSkill2 )
			{
				m_ToolTips.SetCurColor( colorRed );
				Tips_AddText( "\n%s ", pNeedSkill2->pstrName );
				Tips_AddNum( "%u级", pSkillLevel->dwNeedSkill2Level );
			}
		}
	}
	/*if ( pSkill->dwNeedSkill3 && pSkillLevel->dwNeedSkill3Level )
	{
	CRoleSkill* pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( pSkill->dwNeedSkill3 );
	if( pRoleSkill )
	{
	if ( pRoleSkill->GetLevel() < pSkillLevel->dwNeedSkill3Level )
	m_ToolTips.SetCurColor( colorRed );
	else
	m_ToolTips.SetCurColor( colorGreen );

	Tips_AddText( "\n%s ", pRoleSkill->GetName() );
	Tips_AddNum( "%u级", pSkillLevel->dwNeedSkill3Level );
	}
	else
	{
	SkillBase_t* pNeedSkill3 = GetSkillBase( pSkill->dwNeedSkill3 );
	if ( pNeedSkill3 )
	{
	m_ToolTips.SetCurColor( colorRed );
	Tips_AddText( "\n%s ", pNeedSkill3->pstrName );
	Tips_AddNum( "%u级", pSkillLevel->dwNeedSkill3Level );
	}
	}
	}*/

	//Need tree points
	if ( pSkillLevel->dwSkillPoint > 0 )
	{
		//see if the tree level is ok
		//Get skill at tree points
		if ( NULL == GetGameGuiManager()->m_guiSkill )
		{
			GetGameGuiManager()->AddSkill();
			if ( GetGameGuiManager()->m_guiSkill )
				GetGameGuiManager()->m_guiSkill->SetVisible( false );
		}

		//need tree points
		int treeSkillPoints = 0xffff;
		char szDesc[64];
		if ( GetGameGuiManager()->m_guiSkill )
		{
			treeSkillPoints = GetGameGuiManager()->m_guiSkill->GetSerialSkillPoints( pSkillLevel->dwSerialType, pSkillLevel->dwTreeType );
			GetGameGuiManager()->m_guiSkill->GetNeedPointsDesc( pSkillLevel->dwSerialType, pSkillLevel->dwTreeType, szDesc );
		}

		if ( treeSkillPoints < pSkillLevel->dwSkillPoint )
			m_ToolTips.SetCurColor( colorRed );
		else
			m_ToolTips.SetCurColor( colorGreen );
		if ( pSkillLevel->dwSkillPoint > 0 )
		{
			Tips_AddText( "\n需要%s点数: ", szDesc );
			Tips_AddNum( "%u 点", pSkillLevel->dwSkillPoint );
		}		
	}

	//Need weapon
	std::vector<UINT, std::stack_allocator<UINT> > aid;
	aid.reserve(16);
	_parse_str_num(aid,pSkill->pstrConsume);
	static const char* weaponType[] = { "刀类", "剑类", "斧类", "锤类", "杖类", "弓类", "扇类", "棍类", "利刃类", "剑类", "枪类" };		//104 ~ 111

	if ( aid.size() > 0 )
	{
		//First we make sure wether we have corresponding  weapon
		{
			int count = 0;
			for( size_t i=0;i<aid.size();i++ )
			{
				if ( (aid[i] >= 104 && aid[i] <= 111) || aid[i] == 136|| aid[i] == 137|| aid[i] == 155)
				{
					count += GetScene()->GetMainCharacter()->GetItemExteriorCountByType( aid[i] );
				}
			}

			if ( count > 0 )
				m_ToolTips.SetCurColor( colorGreen );
			else
				m_ToolTips.SetCurColor( colorRed );
		}

		Tips_AddString( "\n需要如下武器类型之一：" );
		for(size_t i=0;i<aid.size();i++)
		{
			if ( (aid[i] >= 104) && (aid[i] <= 111) )
			{
				Tips_AddString( weaponType[aid[i]-104] );
				if ( i < (aid.size() -1) )
					Tips_AddString( "、" );
			}
			else if ( aid[i] == 136 ||aid[i] == 137 )
			{
				Tips_AddString( weaponType[aid[i]-128] );
			}
			else if ( aid[i] == 155 )
			{
				Tips_AddString( weaponType[aid[i]-145] );
			}

		}
	}

	//All tree skill points
	//Tips_AddString( "\n" );
	//Get skill tree points list
	if ( NULL == GetGameGuiManager()->m_guiSkill )
	{
		GetGameGuiManager()->AddSkill();
		if ( GetGameGuiManager()->m_guiSkill )
			GetGameGuiManager()->m_guiSkill->SetVisible( false );
	}

	if ( GetGameGuiManager()->m_guiSkill )
	{
		CGuiSkill::t_SkillTree& skillTree = GetGameGuiManager()->m_guiSkill->GetSkillTreeList();
		std::sort( skillTree.begin(), skillTree.end(), DataGreatByValue<CGuiSkill::stSkillTree> );

		for( CGuiSkill::t_SkillTree::iterator it=skillTree.begin(); it != skillTree.end(); ++it )
		{
			if ( (*it).dwPoints )
			{
				Tips_AddText( "\n已经学习%s( ", (*it).strName );
				Tips_AddNum( "%u点)", (*it).dwPoints );
				//TRACE( "\nSkill tree ID=%u, NAME=%s, POINTS=%u", (*it).dwID, (*it).strName, (*it).dwPoints );
			}
		}
	}

	m_ToolTips.SetCurColor( COLOR_ARGB(255,121,100,36) );
	Tips_AddString( "\n右键点击可以使用" );

	m_ToolTips.Resize();
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pObject : 描述
* \param m_ToolTips : 描述
* \param flags : 描述
* \return 返回值的描述
*/
void UpdateObjectToolTips(ObjectBase_t* pObject , CToolTips& m_ToolTips ,DWORD flags, DWORD num, DWORD level)
{
	FUNCTION_BEGIN;

	m_ToolTips.Clear();
	m_ToolTips.SetAlignType( GUI_ALIGN_LEFT );
	m_ToolTips.SetLineGaps( 3 );
	m_ToolTips.SetBorderColor(0);
	m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
	m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));

	bool canUse =  enumItemType(pObject->dwType) == ItemType_Leechdom 
		|| enumItemType(pObject->dwType) == ItemType_Food 
		|| enumItemType(pObject->dwType) == ItemType_Tools
		|| enumItemType(pObject->dwType) == ItemType_Pack
		|| enumItemType(pObject->dwType) == ItemType_Move
		|| enumItemType(pObject->dwType) == ItemType_Scroll
		|| enumItemType(pObject->dwType) == ItemType_Scrxll //soke 新类型 15
		|| enumItemType(pObject->dwType) == ItemType_Tonic
		|| enumItemType(pObject->dwType) == ItemType_Gift
		|| enumItemType(pObject->dwType) == ItemType_MASK
		|| enumItemType(pObject->dwType) == ItemType_FillHP
		|| enumItemType(pObject->dwType) == ItemType_FillMP
		|| enumItemType(pObject->dwType) == ItemType_FillXHP ; //soke 增加自动补血 72类

	bool canUseLevel = pObject->needLevel <= GetScene()->GetMainCharacter()->GetLevel();

	bool bVessalType = enumItemType(pObject->dwType) == ItemType_Pack
		|| enumItemType(pObject->dwType) == ItemType_Tonic
		|| enumItemType(pObject->dwType) == ItemType_Change
		|| enumItemType(pObject->dwType) == ItemType_PackHide ;

	//Special type
	enum enumSpecialType
	{
		ST_NONE,
		ST_LeechdomFood,
	};
	enumSpecialType spType = ST_NONE;
	//Get some special type
	if ( ( enumItemType(pObject->dwType) == ItemType_Leechdom )
		|| ( enumItemType(pObject->dwType) == ItemType_Food ) ) //soke 新类型 15
	{
		if ( pObject->maxhp > 0 )
			spType = ST_LeechdomFood;
	}

	char szTemp[512];
	//Name
	{
		if ( (pObject->dwID == 678)
			|| (pObject->dwID == 679)
			|| (pObject->dwID == 681)
			|| (pObject->dwID == 792) 
			|| (pObject->dwID == 794)
			|| (pObject->dwID == 795)
			|| (pObject->dwID == 791)
			|| (pObject->dwID == 790)
			|| (pObject->dwID == 680)
			|| (pObject->dwID == 748)
			|| (pObject->dwID == 749)
			|| (pObject->dwID == 750)
			|| (pObject->dwID == 751)
			|| (pObject->dwID == 799)
			|| (pObject->dwID == 1170)
			|| (pObject->dwType == 27)
			|| (pObject->dwType == 49)
			|| (pObject->dwType == 73)
			|| (pObject->dwType == 74)
			|| (pObject->dwType == 78)
			|| (pObject->dwType == 31)
			|| (pObject->dwType == 203)
			|| (pObject->dwType == 204)
			|| (pObject->dwType == 205)
			|| (pObject->dwType == 217)
			|| (pObject->dwType == 218)
			|| (pObject->dwType == 126)
			|| (pObject->dwType == 127)						
			|| (pObject->dwID == 4300))		//结束
		{
			if ( (pObject->dwID == 678)
				|| (pObject->dwID == 679) )	
			{
				m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
			}
			else
			{
				m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,255,137) );
			}
		}
	}

	//soke 商店材料等级显示
	if( pObject->dwType == ItemType_Resource/*||pObject->dwType == ItemType_LevelUp*/ || ( pObject->dwID == 678  || pObject->dwID == 679))	//Set id limit		//劳动原料类		m_pObjectBase->dwType == 16
	{
		//Set color
		switch( level)
		{
		case 0:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
			break;
		case 1:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,86,234) );
			break;
		case 2:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,222,3) );
			break;
		case 3:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,255,0) );
			break;
		case 4:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
			break;
		case 5:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
			break;
		case 6:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
			break;
		case 7:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
			break;
		case 8:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
			break;
		case 9:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
			break;
		}

		Tips_AddStringDefFont( pObject->strName );

		//Add upgrade
		Tips_AddStringDefFont( upgradeName[level] );	

	}
	else

	Tips_AddStringDefFont(pObject->strName);

	Tips_AddString( "\n  " );

	//价格
	m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
	if(flags & ITEMTIPS_FLAG_PRICE)
	{
		//if ( !bVessalType )
		{
			if (pObject->dwTradeType & eBuySilver)
			{
				DWORD totalPrice = (DWORD)GetItemPrice(pObject->price)*num;

				if (pObject->dwType==ItemType_DoubleExp)
				{
					Tips_AddNum( "\n需要荣誉点 %d", totalPrice );
				}
				else
				{
					//This time we add revenue rate
					//soke 材料的银子价格
				    float levelopt = pow(5,level);
					totalPrice = totalPrice*levelopt;

					DWORD revRate = CCountry::Instance()->GetRevenue( totalPrice );
					Tips_AddText( "\n购买价格 %s", Money2String( totalPrice ) );
					if ( revRate > 0 )
					{
						m_ToolTips.SetCurColor( COLOR_ARGB(255,70,190,27) );
						Tips_AddText( " + %s", Money2String( revRate ) );
					}	
				}
			}
		//	else
		//	{
		//		Tips_AddNum( "\n购买价格 %s 金子(点券)", Money2String(pObject->price*num) );
		//	}
			//soke 金子购买
			else if (pObject->dwTradeType & eBuyGold && GetGameGuiManager()->m_guiNpcTrade)
			{
				//soke  物品等级价格
				float levelopt = pow(5,level);
				Tips_AddNum( "\n购买价格 %s 金子", Money2String(pObject->price*num*levelopt) );
			}
			//soke 积分购买
			else if (pObject->dwTradeType & eBuyTicket && GetGameGuiManager()->m_guiTicketTrade)
			{
				//soke  物品等级价格
				float levelopt = pow(5,level);
				m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
				Tips_AddNum( "\n购买价格 %s 积分", Ticket2String(pObject->price*num*levelopt) );
			}
			//soke 充值点购买
			else if (pObject->dwTradeType & eBuyChongzhi && GetGameGuiManager()->m_guiChongzhiTrade)
			{
				//soke  物品等级价格
				float levelopt = pow(5,level);
				m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,255) );
				Tips_AddNum( "\n购买价格 %s ", Chongzhi2String(pObject->price*num*levelopt) );
			}							
			//soke 工资购买 双倍价格
			else if (pObject->dwTradeType & eBuySalary && GetGameGuiManager()->m_guiSalaryTrade)
			{
				//soke  物品等级价格
				float levelopt = pow(5,level);
				Tips_AddNum( "\n购买价格 %s 工资", Money2String((pObject->price)*num*levelopt) );
			}

			/*float fNeedMoney1 = buyPrice*num + 0.99f;
			Tips_AddText( "\n购买价格 %s", Money2String( (DWORD)(fNeedMoney1) ) );*/

			//Tips_AddNum("\n价格 %u",(DWORD)(buyPrice + 0.99f));
		}		
	}

	//Add equip position
	static const char* equipPos[] = { "", "头部", "身体", "右手", "左手","颈部", "手腕", "手指", "腰部", "脚" };		//1 ~ 9
	static const char* equipName[] = { "衣服", "衣服", "衣服", "刀", "剑","斧", "锤", "法杖", "弓", "扇子"
		                              ,"棍", "盾牌", "头盔", "腰带", "护腕","鞋子", "项链", "戒指"};		//101 ~ 118	

	static const char* equipName12[] = { "利刃","剑","护心镜" };
	static const char* equipName13[] = { "战旗","战旗","战旗","战旗","战旗","战旗","战旗" };
	static const char* equipName14[] = { "长枪","短枪" };

	if ( (pObject->dwEquipType >= 1 && pObject->dwEquipType <= 9) )
	{
		if ( pObject->dwType >= 101 && pObject->dwType <= 118  )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n装备类型		" );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddString( equipName[pObject->dwType-101] );
		}

		if ( ( pObject->dwType >= 136) && ( pObject->dwType <= 138) )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n装备类型 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddString( equipName12[pObject->dwType-136] );
		}
		if ( ( pObject->dwType >= 141) && ( pObject->dwType <= 147) )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n装备类型 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddString( equipName13[pObject->dwType-141] );
		}
		if ( ( pObject->dwType == 155 || pObject->dwType == 156) )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n装备类型 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddString( equipName14[pObject->dwType-155] );
		}

		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddString( "\n装备位置		" );
		m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
		Tips_AddString( equipPos[pObject->dwEquipType] );
	}
	//end equip pos
	//Add special equip position
	if ( pObject->dwType == 120 )
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddString( "\n装备类型		" );
		m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
		/*if ( pObject->dwID == 875 )
		{
		Tips_AddString( "花" );
		}
		else if ( pObject->dwID == 876 )
		{
		Tips_AddString( "采集手套" );
		}
		else*/
		Tips_AddString( "特殊" );

		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddString( "\n装备位置		" );
		m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
		Tips_AddString( "右手" );
	}

	//需要等级
	if ( pObject->dwID == 680 )		//魂魄石
	{
		if( !canUseLevel )
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,1,19));
		else
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));

		Tips_AddNum("\n需要等级 %u",pObject->needLevel);
	}
	else
	{
		if( !canUseLevel )
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,1,19));
		else
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));

		if ( (pObject->needLevel>0) && !canUse )
			Tips_AddNum("\n需要等级 %u",pObject->needLevel);
	}

	//取消耐久度
	if ( (pObject->dwMaxDur>0) && !canUse && !ISARRAW(pObject->dwType) && !bVessalType
		&& !(pObject->dwType == ItemType_Honor)
		&& !(pObject->dwType == ItemType_DoubleExp)
		&& !(pObject->dwType == ItemType_Move)
		&& !(pObject->dwType == ItemType_Scroll)
		&& !(pObject->dwType == ItemType_Scrxll) //soke 新类型 15
		&& !(pObject->dwType == ItemType_Pack)
		&& !(pObject->dwType == ItemType_Xuest)
		&& !(pObject->dwType == ItemType_DoubleExpC)
		&& !(pObject->dwType == ItemType_DoubleExpZ)
		&& !(pObject->dwType == ItemType_DoubleExpC)
		&& !(pObject->dwType == ItemType_DoubleExpG)
		&& !(pObject->dwType == ItemType_LuncyDan)
		&& !(pObject->dwType == ItemType_HzStone)
		&& !(pObject->dwType == ItemType_Amulets)
		&& !(pObject->dwType == ItemType_Hanbing)
		&& !(pObject->dwType == ItemType_Xunzhang)
		&& !(pObject->dwType == ItemType_Chibang)
		&& !(pObject->dwType == ItemType_Shenbing0)
		&& !(pObject->dwType == ItemType_Shenbing1)
		&& !(pObject->dwType == ItemType_Shenbing2)
		&& !(pObject->dwType == ItemType_Shenshou1)
		&& !(pObject->dwType == ItemType_Shenshou2)
		&& !(pObject->dwType == ItemType_ShangfangBaojian) //取消耐久显示
		&& !(pObject->dwType == ItemType_ZijinLongpao) //取消耐久显示
		&& !(pObject->dwType == ItemType_FashionBody)
		&& !(pObject->dwType == ItemType_HighFashionBody)
		&& !(pObject->dwType == ItemType_XHighFashionBody)
		&& !(pObject->dwType == ItemType_SHighFashionBody)
		&& !(pObject->dwType == ItemType_FillHP)
		&& !(pObject->dwType == ItemType_FillMP)
		&& !(pObject->dwType == ItemType_FillXHP)
		&& !(pObject->dwType == ItemType_Fabao)
		&& !(pObject->dwType == ItemType_Fabaofj1)
		&& !(pObject->dwType == ItemType_Fabaofj2)
		&& !(pObject->dwType == ItemType_Fabaofj3)
		&& !(pObject->dwType == ItemType_Fabaofj4)
		&& !(pObject->dwType == ItemType_Fabaofj5)
		&& !(pObject->dwType == ItemType_Fabaofj6)
		&& !(pObject->dwType == ItemType_Fabaofj7)
		&& !(pObject->dwType == ItemType_Fabaofj8)
		&& !(pObject->dwType == ItemType_Fabaofj9)
		&& !(pObject->dwType == ItemType_Fabaofj10)
		&& !(pObject->dwType == ItemType_Fabaofj11)
		&& !(pObject->dwType == ItemType_Fabaofj12)
		&& !(pObject->dwType == ItemType_YUANSHEN)
		&& !(pObject->dwType == ItemType_YUANSHENFJ1)
		&& !(pObject->dwType == ItemType_YUANSHENFJ2)
		&& !(pObject->dwType == ItemType_YUANSHENFJ3)
		&& !(pObject->dwType == ItemType_YUANSHENFJ4)
		&& !(pObject->dwType == ItemType_YUANSHENFJ5)
		&& !(pObject->dwType == ItemType_YUANSHENFJ6)
		&& !(pObject->dwType == ItemType_BABYFJ1)
		&& !(pObject->dwType == ItemType_BABYFJ2)
		&& !(pObject->dwType == ItemType_BABYFJ3)
		&& !(pObject->dwType == ItemType_BABYFJ4)
		&& !(pObject->dwType == ItemType_BABYFJ5)
		&& !(pObject->dwType == ItemType_BABYFJ6)
		&& !(pObject->dwType == ItemType_SEXX1)
		&& !(pObject->dwType == ItemType_SEXX2)
		&& !(pObject->dwType == ItemType_SEXX3)
		&& !(pObject->dwType == ItemType_SEXX4)
		&& !(pObject->dwType == ItemType_SEXX5)
		&& !(pObject->dwType == ItemType_SEXX6)
		&& !(pObject->dwType == ItemType_SEXX7)
		&& !(pObject->dwType == ItemType_SEXX8)
		&& !(pObject->dwType == ItemType_SEXX9)
		&& !(pObject->dwType == ItemType_SEXX10)
		&& !(pObject->dwType == ItemType_SEXX11)
		&& !(pObject->dwType == ItemType_SEXX12)) //soke 增加自动补血 72类
		
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		Tips_AddString( "\n最大耐久度 " );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddNum("%u",(pObject->dwMaxDur+49)/50);
	}

	//For 荣誉之球
	if ( (pObject->dwType == ItemType_Honor) && (pObject->dwMaxDur > 0) )
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		Tips_AddString( "\n荣誉点 " );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddNum( "%u",pObject->dwMaxDur );
	}

	//For Fill HP item
	if ( (pObject->dwType == ItemType_FillHP) && (pObject->dwMaxDur > 0) )
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddString( "\n最大血量 " );
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,86,234) );
		Tips_AddNum( "%u", pObject->dwMaxDur * 100 );	//We multi the value by 50 cause of max dur is WORD but the real value is a DWORD
	}

	//For Fill MP item
	if ( (pObject->dwType == ItemType_FillMP) && (pObject->dwMaxDur > 0) )
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddString( "\n最大法力 " );
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,86,234) );
		Tips_AddNum( "%u", pObject->dwMaxDur * 50 );	//We multi the value by 50 cause of max dur is WORD but the real value is a DWORD
	}

	//soke 新自动补血 72类
	if ( (pObject->dwType == ItemType_FillXHP) && (pObject->dwMaxDur > 0) )
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddString( "\n最大活力 " );
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,86,234) );
		Tips_AddNum( "%u", pObject->dwMaxDur * 100 );	//We multi the value by 50 cause of max dur is WORD but the real value is a DWORD
	}

	//For 双倍经验道具
	if ( (pObject->dwType == ItemType_DoubleExp) && (pObject->dwMaxDur > 0) )
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		Tips_AddString( "\n时间 " );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddNum( "%u 分", pObject->dwMaxDur/60 );		//Server send time in seconds
	}

	//soke 购买时，额外包裹、高级时装显示天数
	if ( (pObject->dwType == ItemType_Pack) )
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddString( "\n最大时间 " );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		if ( pObject->dwType == ItemType_FashionBody )
		{
			Tips_AddText( "%s", Minute2DayHourMinute( pObject->dwMaxDur, TYPE_DAY_HOUR_MINUTE ) );		
		}
		else
			Tips_AddText( "%s", Minute2DayHourMinute( pObject->dwMaxDur, TYPE_DAY ) );		
	}

	//soke  购买时，高级时装、马匹圣石 显示天数
	if ( pObject->dwType == ItemType_HighFashionBody || pObject->dwType == ItemType_XHighFashionBody || pObject->dwType == ItemType_SHighFashionBody )
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddString( "\n最大时间 " );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddText( "%s", Minute2DayHourMinute( pObject->dwMaxDur, TYPE_DAY ) );
	}

	//soke 购买时，双倍经验、幸运丹 显示天数
	if ( (pObject->dwType == ItemType_DoubleExpC || pObject->dwType == ItemType_DoubleExpZ || pObject->dwType == ItemType_DoubleExpG || pObject->dwType == ItemType_LuncyDan ) )
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddString( "\n时效 " );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		if ( pObject->dwType == ItemType_DoubleExpZ || pObject->dwType == ItemType_DoubleExpC || pObject->dwType == ItemType_DoubleExpG || pObject->dwType == ItemType_LuncyDan)
		{
			Tips_AddText( "%s", Minute3DayHourMinute( pObject->dwMaxDur, TYPE_DAY_HOUR_MINUTE ) );		
		}
		else
			Tips_AddText( "%s", Minute3DayHourMinute( pObject->dwMaxDur, TYPE_DAY ) );		
	}

	//For fashion
	if ( pObject->dwType == ItemType_FashionBody )
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		Tips_AddString( "\n最大耐久度 " );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddNum("%u",pObject->dwMaxDur);
	}

	
	if ( pObject->dwType == ItemType_MASK )		//蒙面巾
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddNum( "\n使用时间 %u秒",  pObject->dwMaxDur );
	}

	// 2010 6
	//if ( pObject->dwType == ItemType_Change )		//月光宝盒
	//{
	//	m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
	//	Tips_AddNum( "\n最大合成次数 %u次",  pObject->dwMaxDur );
	//}

	
	//soke 商店中的时装、马匹圣石 鼠标移动显示生命值百分比
	if ( pObject->dwType == ItemType_FashionBody || pObject->dwType == ItemType_HighFashionBody || pObject->dwType == ItemType_XHighFashionBody || pObject->dwType == ItemType_SHighFashionBody)
	{
	   //增加角色属性（包括当前生命值、最大生命值、当前法术值、最大法术值、当前体力值、最大体力值）
	   if( !( (spType == ST_LeechdomFood) && ( (pObject->maxhp == 8) || (pObject->maxhp == 9) ) ) )
	   {
		   if ( pObject->maxhp )
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			   Tips_AddString( "\n增加生命值" );
			   if ( pObject->maxhp <=12 )
			        m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		       else
		            m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			   Tips_AddNum(" %u%%",pObject->maxhp);
		   }
	   }
    }
	else
	{
	   //增加角色属性（包括当前生命值、最大生命值、当前法术值、最大法术值、当前体力值、最大体力值）
	   if( !( (spType == ST_LeechdomFood) && ( (pObject->maxhp == 8) || (pObject->maxhp == 9) ) ) )
	   {
		   if ( pObject->maxhp > 0 )
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			   Tips_AddString( "\n增加生命值" );
			        m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			        Tips_AddNum(" %u", pObject->maxhp);
		   }
	   }
    }

	//soke 商店中的时装、马匹圣石 鼠标移动显示法术值百分比
	if ( pObject->dwType == ItemType_FashionBody || pObject->dwType == ItemType_HighFashionBody)
	{
	   if( !( (spType == ST_LeechdomFood) && ( (pObject->maxhp == 10) || (pObject->maxhp == 11) ) ) )
	   {
		   if ( pObject->maxmp )
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			   Tips_AddString( "\n增加法术值" );
			   if ( pObject->maxmp <=12 )
			        m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		       else
		            m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			   Tips_AddNum(" %u%%",pObject->maxmp);
		   }
	   }
	}
	//soke 商店中的新时装 鼠标移动显示法术值百分比
	else if (pObject->dwType == ItemType_XHighFashionBody || pObject->dwType == ItemType_SHighFashionBody)
	{
	   if( !( (spType == ST_LeechdomFood) && ( (pObject->maxhp == 10) || (pObject->maxhp == 11) ) ) )
	   {
		   if ( pObject->maxmp )
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			   Tips_AddString( "\n增加法术值" );
			   if ( pObject->maxmp <=12 )
			        m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		       else
		            m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			   Tips_AddNum(" %u%%",pObject->maxmp);

			    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			   Tips_AddString( "\n增加生命值" );
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			    Tips_AddString( " 10000" );

		   }
	   }
	}
	else
	{
	   if( !( (spType == ST_LeechdomFood) && ( (pObject->maxhp == 10) || (pObject->maxhp == 11) ) ) )
	   {
		   if ( pObject->maxmp )
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			   Tips_AddString( "\n增加魔法值" );
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			   Tips_AddNum(" %u",pObject->maxmp);
		   }
	   }
	}   
     // 开始---------------------------------
	if ( pObject->dwID == 202400 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 0;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke --------------------------------- 	
	    else if ( pObject->dwID == 202401 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 1;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202402 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 2;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202403 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 3;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	else if ( pObject->dwID == 202404 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 4;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202405 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 5;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202406 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 6;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	    else if ( pObject->dwID == 202407 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 7;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202408 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 8;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	    else if ( pObject->dwID == 202409 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 9;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202410 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 10;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202411 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 11;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202412 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 12;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	    else if ( pObject->dwID == 202413 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 13;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	    else if ( pObject->dwID == 202414 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 14;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202415 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 15;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202416 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 16;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202417 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 17;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	    else if ( pObject->dwID == 202418 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 18;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202419 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 19;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202420 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 20;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202421 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 21;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	else if ( pObject->dwID == 202422 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 22;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	    else if ( pObject->dwID == 202423 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 23;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202424 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 24;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202425 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 25;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202426 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 26;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202427 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 27;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	    else if ( pObject->dwID == 202428 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 28;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202429 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 29;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( pObject->dwID == 202430 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 30;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024001 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1183;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024002 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1185;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024003 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1186;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024004 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1191;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024005 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1211;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024006 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1212;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024007 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1227;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024008 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1230;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024009 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1239;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024010 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1241;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024011 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1251;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024012 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1253;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024013)
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1257;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024014 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1261;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024015 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1267;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024016 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1275;
		m_ToolTips.AddAnimation( &rl, false );
	}	
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024017 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1279;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024018 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1284;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024019 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1288;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024020 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1293;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024021 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1302;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024022 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1305;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024023 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1308;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024024 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1309;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024025)
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1314;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024026 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1316;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024027 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1319;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024028 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1321;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //soke ---------------------------------
	    else if ( pObject->dwID == 2024029 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 5;
		rl.frame = 1324;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //结束 ---------------------------------
	/*if( !( (spType == ST_LeechdomFood)
	&& ( (pObject->maxhp == 4)
	|| (pObject->maxhp == 5)
	|| (pObject->maxhp == 6)
	|| (pObject->maxhp == 7)
	|| (pObject->maxhp == 12) ) ) )
	{
	if ( pObject->maxsp )
	{
	m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
	Tips_AddString( "\n增加体力值" );
	m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
	Tips_AddNum(" %u",pObject->maxsp);
	}
	}*/
	
	//soke 商店中的 时装、马匹圣石 鼠标移动显示物理攻击百分比
	if ( pObject->dwType == ItemType_FashionBody || pObject->dwType == ItemType_HighFashionBody || pObject->dwType == ItemType_XHighFashionBody|| pObject->dwType == ItemType_SHighFashionBody)
	{
	   //攻击力（包括攻击力、远程伤害、仙术、法术）
	   if ( (pObject->maxpdam > 0) && !bVessalType )
	   {
		   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		   Tips_AddString( "\n增加物理攻击力 " );
		   if ( pObject->maxpdam <=12 )
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		   else
		       m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234)); //12以上颜色
		   Tips_AddNum("%u%%",pObject->maxpdam);
	   }
	}
	else
	{
	   //攻击力（包括攻击力、远程伤害、仙术、法术）
	   if ( (pObject->maxpdam > 0) && !bVessalType )
	   {
		   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		   Tips_AddString( "\n物理攻击力 " );
		   //m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		   Tips_AddNum2("%u - %u",pObject->pdam, pObject->maxpdam);
	   }
	}

	/*if ( pObject->maxrdam > 0 )
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		Tips_AddString( "\n远程攻击力 " );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddNum2("%u - %u",pObject->rdam,pObject->maxrdam);
	}*/

	/*if ( pObject->maxtdam > 0 )
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		Tips_AddString( "\n仙术攻击力 " );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddNum2("%u - %u",pObject->tdam,pObject->maxtdam);
	}*/

	//soke 商店中的时装、马匹圣石 鼠标移动显示魔法攻击百分比
    if ( pObject->dwType == ItemType_FashionBody || pObject->dwType == ItemType_HighFashionBody || pObject->dwType == ItemType_XHighFashionBody || pObject->dwType == ItemType_SHighFashionBody)
	{
	   if ( (pObject->maxmdam > 0) && !bVessalType )
	   {
		   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		   Tips_AddString( "\n增加魔法攻击力 " );
		   if ( pObject->maxmdam <=12 )
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		   else
		       m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234)); //12以上颜色
		   Tips_AddNum("%u%%", pObject->maxmdam);
	   }
	}
	else
	{
	   if ( (pObject->maxmdam > 0) && !bVessalType )
	   {
		   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		   Tips_AddString( "\n魔法攻击力 " );
		   //m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		   Tips_AddNum2("%u - %u", pObject->mdam, pObject->maxmdam);
	   }
	}

	//防御力（包括防御力、法术防御力）
	if ( !canUse )
	{
		if ( pObject->pDef > 0 )
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n物理防御力 " );
			//m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			Tips_AddNum("%u",pObject->pDef);
		}

		if ( pObject->mDef > 0 )
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n魔法防御力 " );
			//m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			Tips_AddNum("%u",pObject->mDef);
		}
	}


	//速度（物理攻击速度、施法速度、移动速度）
    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
	Tips_AddNum("\n攻击速度 %u",pObject->atspeed);
	Tips_AddNum("\n移动速度 %u",pObject->mvspeed);

	//命中率（包括命中率、法术命中率）
	Tips_AddNum("\n命中率 %u%%",pObject->atrating);

	//闪避率（包括躲避率、法术躲避率）
	Tips_AddNum("\n躲避率 %u%%",pObject->akdodge);	

	//Can used for skill tips
	m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
	if ( (pObject->dwType >=104) && (pObject->dwType<=107) )
		Tips_AddString( "\n适用于武术系技能" );
	if ( pObject->dwType == 108 )
		Tips_AddString( "\n适用于法术系技能" );
	if ( pObject->dwType == 109 )
		Tips_AddString( "\n适用于射术系技能" );
	if ( pObject->dwType == 110 )
		Tips_AddString( "\n适用于仙术系技能" );
	if ( pObject->dwType == 111 )
		Tips_AddString( "\n适用于召唤系技能" );
	if ( pObject->dwType == 136 )
		Tips_AddString( "\n适用于刺客系技能" );
	if ( pObject->dwType == 137 )
		Tips_AddString( "\n适用于卫士系技能" );
	if ( pObject->dwType == 155 )
		Tips_AddString( "\n适用于枪系技能" );
	//

	//Show item description
	if ( ( strcmp( pObject->strDesc, "无" ) != 0 )
		&& ( strcmp( pObject->strDesc, "" ) != 0 ) )
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddText( "\n%s", pObject->strDesc );
	}

	//soke 打造装备列表看到的说明
	m_ToolTips.SetCurColor( COLOR_ARGB(255,121,100,36) );
	if ( (pObject->dwType >= 101 && pObject->dwType <= 118) || (pObject->dwType >= 130 && pObject->dwType <= 134) || (pObject->dwType >= 136 && pObject->dwType <= 138)
		|| (pObject->dwType >= 140 && pObject->dwType <= 147) || pObject->dwType == 155 || pObject->dwType == 156 )
	{
		Tips_AddString( "\n右键点击可以装备" );
	}
	else if( (pObject->dwType == 17)
		|| (pObject->dwType == 19 )
		|| (pObject->dwType == 20 )			
		|| (pObject->dwType == 25 )
		|| (pObject->dwType == 26 )			
		|| (pObject->dwType == 33 ) )		//|| (pObject->dwType == 41 )
	{
		Tips_AddString( "\n右键点击可以使用" );
	}
	else if( (pObject->dwType == 23 )
		|| (pObject->dwType == 30 )
		|| (pObject->dwType == 31 ) )
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,121,100,36) );
		Tips_AddString( "\n可装备至功能包裹栏" );
	}
	else if( pObject->dwType == 41 )
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,121,100,36) );
		Tips_AddString( "\n可装备至功能包裹栏" );
		Tips_AddNum( "\n最大使用次数 %u", pObject->dwMaxDur );
	}
	/*else if (m_pObjectBase->dwType == 23 )
	{
	if (m_object.pos.dwLocation!=OBJECTCELLTYPE_EQUIP)
	{
	Tips_AddString( "\n（右键点击可以装备）" );
	}
	else
	{
	Tips_AddString( "\n（右键点击可以打开）" );
	}
	}*/

	//Add some special info
	//Add for Leechdom and food
	/*if (  spType == ST_LeechdomFood )
	{
	m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
	switch( pObject->maxhp )
	{
	case 1:
	Tips_AddNum2( "\n%u 秒内增加攻击力 %u 点", pObject->maxmp, pObject->maxsp );
	break;
	case 2:
	Tips_AddNum2( "\n%u 秒内增加防御力 %u 点" , pObject->maxmp, pObject->maxsp );
	break;
	case 3:
	Tips_AddNum( "\n目标 %u 秒内处于中毒状态", pObject->maxmp );
	break;
	case 4:
	Tips_AddNum( "\n%u 秒内不消耗体力", pObject->maxmp );
	break;
	case 5:
	Tips_AddNum( "\n增加体力 %u%%", pObject->maxsp );
	break;
	case 6:
	Tips_AddNum2( "\n%u 秒内每秒减少体力消耗 %u 点", pObject->maxmp, pObject->maxsp );
	break;
	case 7:
	Tips_AddNum2( "\n%u 秒内每秒增加体力恢复 %u 点", pObject->maxmp, pObject->maxsp );
	break;
	case 8:
	Tips_AddNum( "\n恢复生命值 %u 点", pObject->maxhp );
	break;
	case 9:
	Tips_AddNum2( "\n%u 秒内每秒恢复生命值恢复生命 %u 点", pObject->maxmp, pObject->maxhp );
	break;
	case 10:
	Tips_AddNum( "\n恢复法术值 %u 点", pObject->maxmp );
	break;
	case 11:
	Tips_AddNum2( "\n%u 秒内每秒恢复法术值 %u 点", pObject->maxmp, pObject->maxmp );
	break;
	case 12:
	Tips_AddNum( "\n恢复体力 %u", pObject->maxsp );
	break;
	default:
	break;
	}	
	}*/

	//Tips_AddNum("\n金 %u",pObject->gold);
	//Tips_AddNum("\n木 %u",pObject->wood);
	//Tips_AddNum("\n水 %u",pObject->water);
	//Tips_AddNum("\n火 %u",pObject->fire);
	//Tips_AddNum("\n土 %u",pObject->soil);
	//Tips_AddNum("\n金系抵抗 %u",pObject->goldr);
	//Tips_AddNum("\n木系抵抗 %u",pObject->woodr);
	//Tips_AddNum("\n土系抵抗 %u",pObject->waterr);
	//Tips_AddNum("\n水系抵抗 %u",pObject->firer);
	//Tips_AddNum("\n火系抵抗 %u",pObject->soilr);
	//Tips_AddNum("\n双倍经验 %u",pObject->doublexp);
	//Tips_AddNum("\n防御力增强 %u",pObject->pdefup);
	//Tips_AddNum("\n法术防御力增强 %u",pObject->mdefup);
	//Tips_AddNum("\n抗毒增加 %u",pObject->poisondef);
	//Tips_AddNum("\n抗冰冻增加 %u",pObject->colddef);
	//Tips_AddNum("\n抗石化增加 %u",pObject->petrifydef);
	//Tips_AddNum("\n抗失明增加 %u",pObject->blinddef);
	//Tips_AddNum("\n抗混乱增加 %u",pObject->chaosdef);

	//Tips_AddNum("\n伤害反射 %u%%",pObject->reflectp);
	//Tips_AddNum("\n伤害反射 人物等级/%u",pObject->reflect);

	//Tips_AddNum("\n恢复装备耐久度 %u",pObject->durspeed);
	//Tips_AddNum("\n神圣一击 %u",pObject->holy);

	//Tips_AddNum("\n增加伤害值 %u%%",pObject->ehpp);
	//Tips_AddNum("\n增加伤害值 人物等级/%u",pObject->ehpv);

	//Tips_AddNum("\n增加攻击力 %u%%",pObject->epdamp);
	//Tips_AddNum("\n增加攻击力 人物等级/%u",pObject->erdamp);

	//Tips_AddNum("\n远程攻击力 %u%%",pObject->erdamp);
	//Tips_AddNum("\n远程攻击力 人物等级/%u",pObject->erdamv);

	//Tips_AddNum("\n增加仙术攻击 %u%%",pObject->etdamp);
	//Tips_AddNum("\n增加仙术攻击 人物等级/%u",pObject->etdamv);

	//Tips_AddNum("\n法术攻击 %u%%",pObject->emdamp);
	//Tips_AddNum("\n法术攻击 人物等级/%u",pObject->emdamv);

	//Tips_AddNum("\n五行属性增加 %u",pObject->fivepro);
	//Tips_AddNum("\n吸收生命值 %u",pObject->hpleech);
	//Tips_AddNum("\n吸收法术值 %u",pObject->mpleech);
	//Tips_AddNum("\n转换生命值为法术值 %u%%",pObject->hptomp);

	//Tips_AddNum("\n物理伤害值减少 人物等级/%u",pObject->dhpv);
	//Tips_AddNum("\n物理伤害减少 %u%%",pObject->dhpp);

	//Tips_AddNum("\n法术伤害值减少 人物等级/%u",pObject->dmpv);
	//Tips_AddNum("\n法术伤害值减少 %u%%",pObject->dmpp);

	//Tips_AddNum("\n%u%% 忽视目标防御",pObject->invtarget);
	//Tips_AddNum("\n增加银子掉落 %u%%",pObject->incgold);
	//Tips_AddNum("\n%u%% 使目标冰冻",pObject->cold);
	//Tips_AddNum("\n%u%% 使目标中毒",pObject->poison);
	//Tips_AddNum("\n%u%% 使目标石化",pObject->petrify);
	//Tips_AddNum("\n%u%% 使目标混乱",pObject->chaos);
	//Tips_AddNum("\n%u%% 使目标失明",pObject->blind);
	//Tips_AddNum("\n增加掉宝率 %u%%",pObject->mf);
	//Tips_AddNum("\n魅力增加 %u",pObject->charm);
	//Tips_AddNum("\n熟练度增加 %u%%",pObject->sleight);

	//Add tips
	/*m_ToolTips.SetCurColor( COLOR_ARGB(255,42,246,138) );
	if ( (pObject->dwType >= 1) && (pObject->dwType <= 15 ) )
	{
	Tips_AddString( "\n（右键点击可以装备）" );
	}
	else if( (pObject->dwType == 17)
	|| (pObject->dwType == 19 )
	|| (pObject->dwType == 20 )
	|| (pObject->dwType == 25 )
	|| (pObject->dwType == 26 )
	|| (pObject->dwType == 33 ) )
	{
	Tips_AddString( "\n（右键点击可以使用）" );
	}
	else if( (pObject->dwType == 23 )
	|| (pObject->dwType == 30 )
	|| (pObject->dwType == 31 ) )
	{
	Tips_AddString( "\n（鼠标点击可以打开）" );
	}*/

	m_ToolTips.Resize();

	FUNCTION_END;
}

inline int GetFiveBack(const int& fiveType)
{
	int ret = fiveType+1;
	if ( ret >= FIVE_NONE )
		ret -= FIVE_NONE;

	return ret;
}

inline int GetFiveBorn(const int& fiveType)
{
	if ( fiveType < 0 || fiveType >= FIVE_NONE )
		return 0;

	int ret = fiveType + 3;
	if ( ret >= FIVE_NONE )
		ret -= FIVE_NONE;

	return ret;
}

inline int GetFiveBornFrom(const int& fiveType)
{
	if ( fiveType < 0 || fiveType >= FIVE_NONE )
		return 0;

	int ret = fiveType - 3;
	if ( ret < 0 )
		ret += FIVE_NONE;

	return ret;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CRoleItem::UpdateToolTips()
{
	FUNCTION_BEGIN;

	//make sure we have base object info
	if ( NULL == m_pObjectBase )
		m_pObjectBase = ::GetObjectBase( m_object.dwObjectID );	

	if ( m_price != 0 )
	{
		m_dwTipFlags |= ITEMTIPS_FLAG_PRICE;
	}
#ifdef _AUTOPLAY
	m_dwTipFlags |= ITEMTIPS_FLAG_PRICE | ITEMTIPS_FLAG_REPAIR_PRICE | ITEMTIPS_FLAG_REPAIR_PRICEBYGOLD;
#endif
	UpdateMyObjectToolTips( m_object, m_pObjectBase, m_ToolTips, m_dwTipFlags );
	FUNCTION_END;
}



/**
 * \brief 简短描述
 * 
 * 详细描述 道具详细信息
 * 
 * \param m_object : 描述
 * \param m_pObjectBase : 描述
 * \param m_ToolTips : 描述
 * \param m_dwTipFlags : 描述
 * \return 返回值的描述
 */
void UpdateMyObjectToolTips( t_Object& m_object, ObjectBase_t* m_pObjectBase, CToolTips& m_ToolTips, DWORD m_dwTipFlags )
{
	FUNCTION_BEGIN;

	if(m_object.qwThisID == INVALID_THISID )
	{
		UpdateObjectToolTips(m_pObjectBase,m_ToolTips,m_dwTipFlags, m_object.dwNum,m_object.upgrade);
		return;
	}

	if ( (enumItemType(m_pObjectBase->dwType) == ItemType_SkillUp)
		|| (m_pObjectBase->dwType == 51) )
	{
		UpdateSkillUpItemToolTips( m_object, m_ToolTips, m_dwTipFlags );
		return;
	}

	DWORD dwCanEquip = 0;
	{
		if(0 == (m_pObjectBase->dwCharType & GetScene()->GetMainCharacter()->GetType()))
			dwCanEquip = CANNOTUSE_CHARTYPE;
		if(m_object.needlevel > GetScene()->GetMainCharacter()->GetLevel())
			dwCanEquip = CANNOTUSE_LEVEL;
	}

	enumItemType itemType = enumItemType( m_pObjectBase->dwType );
	bool bCanUse = itemType == ItemType_Leechdom 
		|| itemType == ItemType_Food 
		|| itemType == ItemType_Tools
		|| itemType == ItemType_Pack
		|| itemType == ItemType_Move
		|| itemType == ItemType_Xunzhang
		|| itemType == ItemType_Hanbing
		|| itemType == ItemType_Chibang
		|| itemType == ItemType_Scroll
		|| itemType == ItemType_Scrxll //soke 新类型 15
		|| itemType == ItemType_Tonic
		|| itemType == ItemType_Gift
		|| itemType == ItemType_Change
		|| itemType == ItemType_MASK
		|| itemType == ItemType_FillHP
		|| itemType == ItemType_FillMP
		|| itemType == ItemType_FillXHP; //soke 自动补血 72类

	bool bVessalType = itemType == ItemType_Pack
		|| itemType == ItemType_Tonic
		|| itemType == ItemType_Change
		|| itemType == ItemType_PackHide ;

	bool bDefenceEquip = itemType == 101
		|| itemType == 102
		|| itemType == 103
		|| itemType == 112
		|| itemType == 113
		|| itemType == 114
		|| itemType == 115
		|| itemType == 116
		|| itemType == 138
		|| itemType == 141
		|| itemType == 142
		|| itemType == 143
		|| itemType == 144
		|| itemType == 145
		|| itemType == 146
		|| itemType == 147
		|| itemType == 156;

	bool bAttackEquip = itemType == 104
		|| itemType == 105
		|| itemType == 106
		|| itemType == 107
		|| itemType == 108
		|| itemType == 109
		|| itemType == 110
		|| itemType == 111
		|| itemType == 117
		|| itemType == 118
		|| itemType == 136
		|| itemType == 137
		|| itemType == 155;

	//Special type
	enum enumSpecialType
	{
		ST_NONE,
		ST_LeechdomFood,
	};
	enumSpecialType spType = ST_NONE;
	//Get some special type
	if ( ( enumItemType(m_pObjectBase->dwType) == ItemType_Leechdom )
		|| ( enumItemType(m_pObjectBase->dwType) == ItemType_Food ) )
	{
		if ( m_object.maxhp > 0 )
			spType = ST_LeechdomFood;
	}

		m_ToolTips.Clear();
		m_ToolTips.SetBorderColor(0);
		m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		char szTemp[512];

		//Name
	//soke 根据装备类型设置名字颜色
	//	if ( m_object.kind == 8 )			//橙装
	//m_ToolTips.SetCurColor( COLOR_ARGB(255,255,128,0) );
	
//		if ( m_object.kind & 8 )		//set
//		m_ToolTips.SetCurColor( COLOR_ARGB(255,146,77,22) );
	if( m_object.kind & 16 )

	m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,137) );
	else if( m_object.kind & 4 )
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,137) );
	else if( m_object.kind &2 )
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,222,3) );
	else if( m_object.kind & 1 )
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,86,234) );
	else
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
    //soke 这个会显示两个名
	//Tips_AddStringDefFont( m_object.strName );
		//Get diamond about
		int diamondNum = 0;
		for( int i=0;i<6;i++ )
		{
			if ( m_object.socket[i] == 1 )
				diamondNum++ ;
		}
		/*if ( diamondNum > 0 )
		Tips_AddString( "镶了魂魄的" );*/

		//Just a test
		/*m_object.strName[0] = 0L;
		sprintf( m_object.strName, "MeiLiDe0HuDie" );
		m_object.kind = 1;
		m_pObjectBase->dwID == 678;*/
		//End test


		//Split item name
		char* pName;
		char namePartOne[MAX_NAMESIZE];
		char namePartTwo[MAX_NAMESIZE];
		char nameFiveFull[MAX_NAMESIZE];

	namePartOne[0] = 0L;
	namePartTwo[0] = 0L;
	nameFiveFull[0] = 0L;
	
	pName = strpbrk( m_object.strName, "0" );
	if ( pName )
	{
		sprintf( namePartOne, ++pName );
		int len = strlen(m_object.strName)-strlen(namePartOne)-1;
		strncpy( namePartTwo, m_object.strName, len );
		namePartTwo[len] = 0;
	}
	else  
	{
	   if (m_pObjectBase->dwType != ItemType_XHighFashionBody && m_pObjectBase->dwType != ItemType_SHighFashionBody && m_pObjectBase->dwType != ItemType_HighFashionBody)
	   {
		   sprintf( namePartOne, m_object.strName ); //soke 物品的名字
	   }
    }

	if (m_pObjectBase->dwType == ItemType_XHighFashionBody || m_pObjectBase->dwType == ItemType_SHighFashionBody  || m_pObjectBase->dwType == ItemType_HighFashionBody)
	{
		//soke 新时装物品的颜色
        m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		m_ToolTips.AddText("当前时装");
	}

	//Add five type name to partone
	if ( (m_object.fivetype >= FIVE_METAL) && (m_object.fivetype < FIVE_NONE) )
	{
		strcat( nameFiveFull, "【" );
		strcat( nameFiveFull, fiveName[m_object.fivetype] );
		strcat( nameFiveFull, "】" );
	}

		//End split item name

	/*static char* upgradeName[] = { " 一等", " 二等", " 三等", " 四等", " 五等",
			" 六等", " 七等", " 八等", " 九等", " 十等", "十一等", "十二等", "十三等", "十四等", "十五等" };*/

	if ( 16 & m_object.kind && !(m_pObjectBase->dwType == ItemType_Amulets) )	//神圣祝福道具
	{
	//   //soke 神器显示特效
	//   if (m_object.retain3 > 0)
	//   {
	// 	stResourceLocation rl;
	// 	rl.SetFileName( "data\\interfaces1.gl");
	// 	rl.group = 16;
	// 	rl.frame = 4;
	// 	m_ToolTips.AddAnimation1( &rl, false );
	// 	Tips_AddString( "\n" );
	//   }
	if(m_object.retain24>0 && m_object.retain24<=16) //龙凤吟
		{
			switch (m_object.retain24)
			{
			case 1:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 111;
					rl.frame = 12;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			case 2:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 111;
					rl.frame = 13;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			case 3:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 111;
					rl.frame = 14;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			case 4:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 111;
					rl.frame = 15;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			case 5:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 111;
					rl.frame = 16;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			case 6:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 72;
					rl.frame = 1;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			case 7:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 81;
					rl.frame = 1;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			case 8:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 81;
					rl.frame = 2;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			case 9:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 82;
					rl.frame = 3;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			case 10:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 82;
					rl.frame = 4;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			case 11:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 120;
					rl.frame = 0;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			case 12:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 120;
					rl.frame = 1;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			case 13:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 120;
					rl.frame = 2;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			case 14:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 120;
					rl.frame = 3;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			case 15:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 120;
					rl.frame = 4;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			case 16:
				{
					stResourceLocation rl;
					rl.SetFileName( "data\\interfaces3.gl");
					rl.group = 120;
					rl.frame = 5;
					m_ToolTips.AddAnimation1( &rl, false );
					Tips_AddString( "\n" );
				}
				break;
			
			default:
				stResourceLocation rl;
				rl.SetFileName( "data\\interfaces.gl");
				rl.group = 150;
				rl.frame = 505;
				m_ToolTips.AddAnimation1( &rl, false );
				Tips_AddString( "\n" );
				break;
			}
			
		}
	  //soke 紫装15星紫装显示特效-删除
	  else if (m_object.upgrade >= 15)
	  {
		stResourceLocation rl;
		rl.SetFileName( "data\\interfaces.gl");
		rl.group = 150;
		rl.frame = 5051;
		m_ToolTips.AddAnimation1( &rl, false );
		Tips_AddString( "\n" );
	  }

	  //soke 紫装15星以下显示特效-删除
	  else if (m_object.upgrade < 15)
	  {
		stResourceLocation rl;
		rl.SetFileName( "data\\interfaces.gl");
		rl.group = 3;
		rl.frame = 2501;
		m_ToolTips.AddAnimation1( &rl, false );
		Tips_AddString( "\n" );
	  }
		//soke 紫装显示钻石标志
		stResourceLocation rt;
		rt.SetFileName( "data\\interfaces.gl");
		rt.group = 4;
		rt.frame = 71;
		m_ToolTips.AddAnimation1( &rt, false );
		
		//sky 神器显示
      
		if (m_object.retain3 > 0 )
		{
			Tips_AddStringDefFontNAME( "神器·");
			Tips_AddStringDefFontNAME( namePartOne ); // 此处获取装备NAME名字 单独调用字符串长度
		}
		else
		{
			Tips_AddStringDefFontNAME( namePartOne );
		}
        if (m_object.itemrefine > 0 )
		{
        	    if (m_object.itemrefine == 15)
				{
					Tips_AddStringDefFontNAME( "+15" );
				}
				else if (m_object.itemrefine == 14)
				{
					Tips_AddStringDefFontNAME( "+14" );
				}
				else if (m_object.itemrefine == 13)
				{
					Tips_AddStringDefFontNAME( "+13" );
				}
				else if (m_object.itemrefine == 12)
				{
					Tips_AddStringDefFontNAME( "+12" );
				}
				else if (m_object.itemrefine == 11)
				{
					Tips_AddStringDefFontNAME( "+11" );
				}
				else if (m_object.itemrefine == 10)
				{
					Tips_AddStringDefFontNAME( "+10" );
				}
				else if (m_object.itemrefine == 9)
				{
					Tips_AddStringDefFontNAME( "+9" );
				}
				else if (m_object.itemrefine == 8)
				{
					Tips_AddStringDefFontNAME( "+8" );
				}
				else if (m_object.itemrefine == 7)
				{
					Tips_AddStringDefFontNAME( "+7" );
				}
				else if (m_object.itemrefine == 6)
				{
					Tips_AddStringDefFontNAME( "+6" );
				}
				else if (m_object.itemrefine == 5)
				{
					Tips_AddStringDefFontNAME( "+5" );
				}
				else if (m_object.itemrefine == 4)
				{
					Tips_AddStringDefFontNAME( "+4" );
				}
				else if (m_object.itemrefine == 3)
				{
					Tips_AddStringDefFontNAME( "+3" );
				}
				else if (m_object.itemrefine == 2)
				{
					Tips_AddStringDefFontNAME( "+2" );
				}
				else if (m_object.itemrefine == 1)
				{
					Tips_AddStringDefFontNAME( "+1" );
				}

        }

			if ( strlen(nameFiveFull) > 0 )
			{
				//Back cur color
				DWORD color = m_ToolTips.GetCurColor();
				m_ToolTips.SetCurColor( fiveColor[m_object.fivetype] );
				Tips_AddStringDefFontNAME( nameFiveFull );
				m_ToolTips.SetCurColor( color );
			}

		//soke 转生装备 转标志
	    if ( m_object.round >= 0)
	    {
		   stResourceLocation rl;
		   rl.SetFileName( "data\\interfaces1.gl");
		   rl.group = 1;
		   rl.frame = 7;
		   m_ToolTips.AddAnimation1( &rl, false );
	    }
		
		//Add name part two
		if ( strlen(namePartTwo) > 0 )
		{
			Tips_AddString( "\n" );
			Tips_AddStringDefFontNAME( namePartTwo );
		}


		//sky 龙星显示（上） 
		if (m_object.drastar > 0)
		{
			Tips_AddStringDefFontNAME("\n");
			if (((m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118) || (m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147)
				|| (m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 134)|| (m_pObjectBase->dwType >= 136 && m_pObjectBase->dwType <= 138) || m_pObjectBase->dwType == 155 || m_pObjectBase->dwType == 156) && (m_object.upgrade > 0))
			{
				stResourceLocation rHole;
				rHole.SetFileName("data\\interfaces1.gl");
				rHole.group = 32;
				if (m_object.dtrough == 1)
				{
					rHole.frame = 30;
				}
				else if (m_object.dtrough == 2)
				{
					rHole.frame = 30;
				}
				else if (m_object.dtrough == 3)
				{
					rHole.frame = 30;
				}
				else if (m_object.dtrough == 4)
				{
					rHole.frame = 32;
				}
				else if (m_object.dtrough == 5)
				{
					rHole.frame = 32;
				}
				else if (m_object.dtrough == 6)
				{
					rHole.frame = 32;
				}
				else if (m_object.dtrough == 7)
				{
					rHole.frame = 34;
				}
				else if (m_object.dtrough == 8)
				{
					rHole.frame = 34;
				}
				else if (m_object.dtrough == 9)
				{
					rHole.frame = 34;
				}
				else if (m_object.dtrough == 10)
				{
					rHole.frame = 36;
				}
				else if (m_object.dtrough == 11)
				{
					rHole.frame = 36;
				}
				else if (m_object.dtrough == 12)
				{
					rHole.frame = 36;
				}
				else if (m_object.dtrough == 13)
				{
					rHole.frame = 38;
				}
				else if (m_object.dtrough == 14)
				{
					rHole.frame = 38;
				}
				else if (m_object.dtrough > 14)
				{
					rHole.frame = 38;
				}
				for (int i = 0; i<m_object.drastar; i++)

				// //sky 大于10就要调整宽度了
				// if (m_object.dtrough < 10)
				// 	m_ToolTips.AddAnimation3(&rHole, false);
				// else
					m_ToolTips.AddAnimation4(&rHole, false);
			}
		}
		//sky 龙槽显示（下）
		if (m_object.dtrough > 0)
		{
			//sky 有龙星九不要换行了
			if (m_object.drastar > 0)
			{
			    Tips_AddStringDefFontNAME("");
			}
			else
			{
				Tips_AddStringDefFontNAME("\n");
			}

			if (((m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118) || (m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147)
				|| (m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 134)|| (m_pObjectBase->dwType >= 136 && m_pObjectBase->dwType <= 138) || m_pObjectBase->dwType == 155 || m_pObjectBase->dwType == 156) && (m_object.upgrade > 0))
			{
				stResourceLocation rHole;
				rHole.SetFileName("data\\interfaces1.gl");
				rHole.group = 32;
				if (m_object.dtrough == 1) //1-3 31
				{
					rHole.frame = 31;
				}
				else if (m_object.dtrough == 2)
				{
					rHole.frame = 31;
				}
				else if (m_object.dtrough == 3)
				{
					rHole.frame = 31;
				}
				else if (m_object.dtrough == 4)
				{
					rHole.frame = 33;
				}
				else if (m_object.dtrough == 5)
				{
					rHole.frame = 33;
				}
				else if (m_object.dtrough == 6)
				{
					rHole.frame = 33;
				}
				else if (m_object.dtrough == 7)
				{
					rHole.frame = 35;
				}
				else if (m_object.dtrough == 8)// 35
				{
					rHole.frame = 35;
				}
				else if (m_object.dtrough == 9)//35
				{
					rHole.frame = 35;
				}
				else if (m_object.dtrough == 10) //37
				{
					rHole.frame = 37;
				}
				else if (m_object.dtrough == 11) //37
				{
					rHole.frame = 37;
				}
				else if (m_object.dtrough == 12) //37
				{
					rHole.frame = 37;
				}
				else if (m_object.dtrough == 13) //39
				{
					rHole.frame = 39;
				}
				else if (m_object.dtrough == 14)
				{
					rHole.frame = 39;
				}
				else if (m_object.dtrough > 14)
				{
					rHole.frame = 39;
				}
				if (m_object.drastar > 0) //sky 龙星判断
				{
					for (int i = m_object.drastar; i<m_object.dtrough; i++)
					
					//sky 大于10就要调整了
					// if (m_object.dtrough < 10)
					// m_ToolTips.AddAnimation3(&rHole, false);
					// else
					
						m_ToolTips.AddAnimation4(&rHole, false);
				}
				else
				{   
					//sky 龙槽
					for (int i = 0; i<m_object.dtrough; i++)

					// //sky 大于10就要调整了
					// if (m_object.dtrough < 10)
					// 	m_ToolTips.AddAnimation3(&rHole, false);
					// else
						m_ToolTips.AddAnimation4(&rHole, false);
				}
			}
		}

		//sky 正常的紫装星星
		if (m_object.upgrade > 0)
		{
			Tips_AddStringDefFontNAME( "\n" );
			if ( ((m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118) || (m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147)
				|| (m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 134)|| (m_pObjectBase->dwType >= 136 && m_pObjectBase->dwType <= 138) || m_pObjectBase->dwType == 155 || m_pObjectBase->dwType == 156) && (m_object.upgrade > 0))
			{
				stResourceLocation rHole;
				rHole.SetFileName( "data\\interfaces.gl" );
				rHole.group = 4;						
				if (m_object.upgrade == 1)
				{
					rHole.frame = 500;
				}
				else if (m_object.upgrade == 2)
				{
					rHole.frame = 500;
				}
				else if (m_object.upgrade == 3)
				{
					rHole.frame = 500;
				}
				else if (m_object.upgrade == 4)
				{
					rHole.frame = 500;
				}
				else if (m_object.upgrade == 5)
				{
					rHole.frame = 500;
				}
				else if (m_object.upgrade == 6)
				{
					rHole.frame = 501;
				}
				else if (m_object.upgrade == 7)
				{
					rHole.frame = 501;
				}
				else if (m_object.upgrade == 8)
				{
					rHole.frame = 501;
				}
				else if (m_object.upgrade == 9)
				{
					rHole.frame = 501;
				}
				else if (m_object.upgrade == 10)
				{
					rHole.frame = 501;
				}
				else if (m_object.upgrade == 11)
				{
					rHole.frame = 502;
				}
				else if (m_object.upgrade == 12)
				{
					rHole.frame = 503;
				}
				else if (m_object.upgrade == 13)
				{
					rHole.frame = 504;
				}
				else if (m_object.upgrade == 14)
				{
					rHole.frame = 505;
				}
				else if (m_object.upgrade > 14)
				{
					rHole.frame = 506;
				}
				if (m_object.dtrough > 0)
				{
				    for (int i = 0; i< m_object.dtrough; i++)

					//sky 大于10就要调整了
					// if (m_object.dtrough < 10)
					// {
					// 	m_ToolTips.AddAnimation5(&rHole, false);
					// }
					// else
					// {
						m_ToolTips.AddAnimation6(&rHole, false);
					// }
				
					for (int i = m_object.dtrough; i<m_object.upgrade; i++)
					m_ToolTips.AddAnimation(&rHole, false);
				}
				else
				{
					//sky 这里才是装备的星星
				    for (int i = 0; i < m_object.upgrade; i++)
					m_ToolTips.AddAnimation(&rHole, false);
				}
			}
		}
	}
	else if ( 16 & m_object.kind && (m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 134 || m_pObjectBase->dwType == 140 ) )	//神圣祝福马匹装备道具
	{
	  //soke 未转生紫装15星特效
	 /* if (m_object.upgrade >= 0 && m_object.round > 0)
	  {
		stResourceLocation rl;
		rl.SetFileName( "data\\interfaces1.gl");
		rl.group = 0;
		rl.frame = 1;
		m_ToolTips.AddAnimation2( &rl, false );
		Tips_AddString( "\n" );
	  }
	  else
	  {
	    //soke 未转生紫装显示特效
		stResourceLocation rl2;
		rl2.SetFileName( "data\\interfaces.gl");
		rl2.group = 3;
		rl2.frame = 250;
		m_ToolTips.AddAnimation2( &rl2, false );
		Tips_AddString( "\n" );
	  }*/
		//soke 紫装显示钻石标志
		stResourceLocation rl3;
		rl3.SetFileName( "data\\interfaces1.gl");
		rl3.group = 21;
		rl3.frame = 3;
		m_ToolTips.AddAnimation( &rl3, false );
		
		//Tips_AddStringDefFont( "神圣" );
		Tips_AddStringDefFontNAME( namePartOne );
        //装备精炼
        if (m_object.itemrefine > 0 )
		{
        	    if (m_object.itemrefine == 15)
				{
					Tips_AddStringDefFontNAME( "+15" );
				}
				else if (m_object.itemrefine == 14)
				{
					Tips_AddStringDefFontNAME( "+14" );
				}
				else if (m_object.itemrefine == 13)
				{
					Tips_AddStringDefFontNAME( "+13" );
				}
				else if (m_object.itemrefine == 12)
				{
					Tips_AddStringDefFontNAME( "+12" );
				}
				else if (m_object.itemrefine == 11)
				{
					Tips_AddStringDefFontNAME( "+11" );
				}
				else if (m_object.itemrefine == 10)
				{
					Tips_AddStringDefFontNAME( "+10" );
				}
				else if (m_object.itemrefine == 9)
				{
					Tips_AddStringDefFontNAME( "+9" );
				}
				else if (m_object.itemrefine == 8)
				{
					Tips_AddStringDefFontNAME( "+8" );
				}
				else if (m_object.itemrefine == 7)
				{
					Tips_AddStringDefFontNAME( "+7" );
				}
				else if (m_object.itemrefine == 6)
				{
					Tips_AddStringDefFontNAME( "+6" );
				}
				else if (m_object.itemrefine == 5)
				{
					Tips_AddStringDefFontNAME( "+5" );
				}
				else if (m_object.itemrefine == 4)
				{
					Tips_AddStringDefFontNAME( "+4" );
				}
				else if (m_object.itemrefine == 3)
				{
					Tips_AddStringDefFontNAME( "+3" );
				}
				else if (m_object.itemrefine == 2)
				{
					Tips_AddStringDefFontNAME( "+2" );
				}
				else if (m_object.itemrefine == 1)
				{
					Tips_AddStringDefFontNAME( "+1" );
				}
        }

		if ( strlen(nameFiveFull) > 0 )
		{
			//Back cur color
			DWORD color = m_ToolTips.GetCurColor();
			m_ToolTips.SetCurColor( fiveColor[m_object.fivetype] );
			Tips_AddStringDefFontNAME( nameFiveFull );
			m_ToolTips.SetCurColor( color );
		}

		//soke 转生装备 转标志
	    if ( m_object.round >= 0)
	    {
		   stResourceLocation rl13;
		   rl13.SetFileName( "data\\interfaces1.gl");
		   rl13.group = 1;
		   rl13.frame = 7;
		   m_ToolTips.AddAnimation1( &rl13, false );
	    }
		
		//Add name part two
		if ( strlen(namePartTwo) > 0 )
		{
			Tips_AddString( "\n" );
			Tips_AddStringDefFontNAME( namePartTwo );
		}

		//soke 神圣祝福装备升星特效显示
        char szUpgrade[MAX_NAMESIZE];
		szUpgrade[0] = 0L;
		for( int k=0;k<m_object.upgrade;k++ )
			strcat( szUpgrade, "★" );
		szUpgrade[strlen(szUpgrade)] = 0L;

		if ( strlen(szUpgrade) > 0 )
		{
			Tips_AddStringDefFontNAME( "\n" );
			if ( (m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 134) && (m_object.upgrade > 0))
			{
				stResourceLocation rHole;
				rHole.SetFileName( "data\\interfaces.gl" );
				rHole.group = 4;						
				if (m_object.upgrade == 1)
				{
					rHole.frame = 500;
				}
				else if (m_object.upgrade == 2)
				{
					rHole.frame = 500;
				}
				else if (m_object.upgrade == 3)
				{
					rHole.frame = 500;
				}
				else if (m_object.upgrade == 4)
				{
					rHole.frame = 500;
				}
				else if (m_object.upgrade == 5)
				{
					rHole.frame = 500;
				}
				else if (m_object.upgrade == 6)
				{
					rHole.frame = 501;
				}
				else if (m_object.upgrade == 7)
				{
					rHole.frame = 501;
				}
				else if (m_object.upgrade == 8)
				{
					rHole.frame = 501;
				}
				else if (m_object.upgrade == 9)
				{
					rHole.frame = 501;
				}
				else if (m_object.upgrade == 10)
				{
					rHole.frame = 501;
				}
				else if (m_object.upgrade == 11)
				{
					rHole.frame = 502;
				}
				else if (m_object.upgrade == 12)
				{
					rHole.frame = 503;
				}
				else if (m_object.upgrade == 13)
				{
					rHole.frame = 504;
				}
				else if (m_object.upgrade == 14)
				{
					rHole.frame = 505;
				}
				else if (m_object.upgrade > 14)
				{
					rHole.frame = 506;
				}
				for( int i=0;i<m_object.upgrade;i++ )
					m_ToolTips.AddAnimation( &rHole, false );
			}
			else
				Tips_AddStringDefFont( szUpgrade );
		}
	}
	else if ( (4 & m_object.kind || 2 & m_object.kind || 1 & m_object.kind  ) && !(m_pObjectBase->dwType == ItemType_Amulets ) )	//神圣道具
	{
	  //soke 绿装15星显示钻石标志
	  if (m_object.upgrade > 14)
	  {
		stResourceLocation rl;
		rl.SetFileName( "data\\interfaces1.gl");
		rl.group = 21;
		rl.frame = 3;
		m_ToolTips.AddAnimation( &rl, false );
	  }
	
		//Tips_AddStringDefFont( "神圣" );
		Tips_AddStringDefFontNAME( namePartOne );

			if ( strlen(nameFiveFull) > 0 )
			{
				//Back cur color
				DWORD color = m_ToolTips.GetCurColor();
				m_ToolTips.SetCurColor( fiveColor[m_object.fivetype] );
				Tips_AddStringDefFontNAME( nameFiveFull );
				m_ToolTips.SetCurColor( color );
			}

			//Add name part two
			if ( strlen(namePartTwo) > 0 )
			{
				Tips_AddString( "\n" );
				Tips_AddStringDefFontNAME( namePartTwo );
			}

		//Add grade 这里是装备升星的配置
		char szUpgrade[MAX_NAMESIZE];
		szUpgrade[0] = 0L;
		for( int k=0;k<m_object.upgrade;k++ )
			strcat( szUpgrade, "★" );
		szUpgrade[strlen(szUpgrade)] = 0L;

		//Set color
		switch( m_object.upgrade )
		{
		case 0:
			break;
		case 1:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
			break;
		case 2:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,86,234) );
			break;
		case 3:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,86,234) );
			break;
		case 4:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,222,3) );
			break;
		case 5:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,222,3) );
			break;
		case 6:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,255,0) );
			break;
		case 7:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,255,0) );
			break;
		case 8:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,113,0) );
			break;
		case 9:
		case 10:
		case 11:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
			break;
		}

		if ( strlen(szUpgrade) > 0 )
		{
			Tips_AddStringDefFont( "\n" );
			if ( ((m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118) || (m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 134)|| (m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147)
				|| (m_pObjectBase->dwType >= 136 && m_pObjectBase->dwType <= 138) || m_pObjectBase->dwType == 155 || m_pObjectBase->dwType == 156) && (m_object.upgrade > 9))
			{
				stResourceLocation rHole;
				rHole.SetFileName( "data\\interfaces.gl" );
				rHole.group = 4;						

				if (m_object.upgrade == 10)
				{
					rHole.frame = 14;
				}
				else if (m_object.upgrade == 11)
				{
					rHole.frame = 15;
				}
				else if (m_object.upgrade == 12)
				{
					rHole.frame = 50;
				}
				else if (m_object.upgrade == 13 || m_object.upgrade == 14)
				{
					rHole.frame = 80;
				}
				else if (m_object.upgrade > 14)
				{
					rHole.frame = 100;
				}
				for( int i=0;i<m_object.upgrade;i++ )
					m_ToolTips.AddAnimation( &rHole, false );
			}
			else
				Tips_AddStringDefFont( szUpgrade );
		}
	}
	else if ( !(16 & m_object.kind || 4 & m_object.kind|| 3 & m_object.kind || 2 & m_object.kind || 1 & m_object.kind  ) && ((m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118)|| (m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147)) )	//神圣道具
	{
		//Tips_AddStringDefFont( "神圣" );
		Tips_AddStringDefFontNAME( namePartOne );

			if ( strlen(nameFiveFull) > 0 )
			{
				//Back cur color
				DWORD color = m_ToolTips.GetCurColor();
				m_ToolTips.SetCurColor( fiveColor[m_object.fivetype] );
				Tips_AddStringDefFontNAME( nameFiveFull );
				m_ToolTips.SetCurColor( color );
			}

			//Add name part two
			if ( strlen(namePartTwo) > 0 )
			{
				Tips_AddString( "\n" );
				Tips_AddStringDefFontNAME( namePartTwo );
			}

		//Add grade 这里是装备升星的配置
		char szUpgrade[MAX_NAMESIZE];
		szUpgrade[0] = 0L;
		for( int k=0;k<m_object.upgrade;k++ )
			strcat( szUpgrade, "★" );
		szUpgrade[strlen(szUpgrade)] = 0L;

		//Set color
		switch( m_object.upgrade )
		{
		case 0:
			break;
		case 1:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
			break;
		case 2:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,86,234) );
			break;
		case 3:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,86,234) );
			break;
		case 4:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,222,3) );
			break;
		case 5:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,222,3) );
			break;
		case 6:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,255,0) );
			break;
		case 7:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,255,0) );
			break;
		case 8:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,113,0) );
			break;
		case 9:
		case 10:
		case 11:
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
			break;
		}

		if ( strlen(szUpgrade) > 0 )
		{
			Tips_AddStringDefFont( "\n" );
			if ( ((m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118) || (m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 134)|| (m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147)
				|| (m_pObjectBase->dwType >= 136 && m_pObjectBase->dwType <= 138) || m_pObjectBase->dwType == 155 || m_pObjectBase->dwType == 156) && (m_object.upgrade > 9))
			{
				stResourceLocation rHole;
				rHole.SetFileName( "data\\interfaces.gl" );
				rHole.group = 4;						

				if (m_object.upgrade == 10)
				{
					rHole.frame = 14;
				}
				else if (m_object.upgrade == 11)
				{
					rHole.frame = 15;
				}
				else if (m_object.upgrade == 12)
				{
					rHole.frame = 50;
				}
				else if (m_object.upgrade == 13 || m_object.upgrade == 14)
				{
					rHole.frame = 80;
				}
				else if (m_object.upgrade > 14)
				{
					rHole.frame = 100;
				}
				for( int i=0;i<m_object.upgrade;i++ )
					m_ToolTips.AddAnimation( &rHole, false );
			}
			else
				Tips_AddStringDefFont( szUpgrade );
		}
	}
	else if ( m_pObjectBase->dwType >= 221 && m_pObjectBase->dwType <= 230) 
	{
		//soke 自定义补丁物品的颜色
		if (m_object.handbooklvg >= 601)
		{
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,0,0) );
			Tips_AddStringDefFont( "稀有·" );
			Tips_AddStringDefFontNAME( namePartOne );	//soke 红色图鉴名称颜色
		}
		else if (m_object.handbooklvg >= 501)
		{
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,253,94,4));
			Tips_AddStringDefFont( "卓越·" );
			Tips_AddStringDefFontNAME( namePartOne );	//soke 橙色图鉴名称颜色
		}		
		else if (m_object.handbooklvg >= 401)
		{
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,135,24,198));
			Tips_AddStringDefFont( "完美·" );
			Tips_AddStringDefFontNAME( namePartOne );	//soke 紫色图鉴名称颜色
		}		
		else if (m_object.handbooklvg >= 301)
		{
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,255,0));
			Tips_AddStringDefFont( "无暇·" );
			Tips_AddStringDefFontNAME( namePartOne );	//soke 绿色图鉴名称颜色
		}
		else if (m_object.handbooklvg >= 201)
		{
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,250,0));
			Tips_AddStringDefFont( "精致·" );
			Tips_AddStringDefFontNAME( namePartOne );	//soke 黄色图鉴名称颜色
		}
		else if (m_object.handbooklvg >= 101)
		{
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,36,112,255));
			Tips_AddStringDefFont( "强化·" );
			Tips_AddStringDefFontNAME( namePartOne );	//soke 蓝色图鉴名称颜色
		}
		else if (m_object.handbooklvg >= 0)
		{
			m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,255,255));
			Tips_AddStringDefFont( "普通·" );
			Tips_AddStringDefFontNAME( namePartOne );	//soke 白色图鉴名称颜色
		}		
	}				
	else if ( m_pObjectBase->dwType == ItemType_Amulets ) //护身符
	{
		// soke 护身符支持自定义补丁物品的颜色
		m_ToolTips.SetCurColor(m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFont(namePartOne); // soke 物品名
		if (m_object.upgrade > 0)
		{
			char szTmp[512];
			sprintf(szTmp, " + %u", m_object.upgrade);
			Tips_AddStringDefFont(szTmp);
		}

		if (m_object.retain32 > 0)
		{
			Tips_AddString( "\n" );
			//soke 斧子
			stResourceLocation rl;	
			rl.SetFileName( "data\\interfaces3.gl");
			rl.group = 117;
			rl.frame = 102;
			m_ToolTips.AddAnimation1( &rl, false );
			
			Tips_AddStringDefFont("\n");
			stResourceLocation rHole;
			rHole.SetFileName("data\\interfaces1.gl");
			rHole.group = 32;
			rHole.frame = 38;
			for (int i = 0; i < 15; i++)
			{
				m_ToolTips.AddAnimation4(&rHole, false);
			}

			Tips_AddStringDefFont("\n");
			rHole.SetFileName( "data\\interfaces4.gl" );
			rHole.group = 9;	
			rHole.frame = 129;				
			for (int i = 0; i < 15; i++)
			{
				m_ToolTips.AddAnimation6(&rHole, false);
			}

			Tips_AddStringDefFont("\n");
			rl.SetFileName( "data\\interfaces3.gl");
			rl.group = 114;
			rl.frame = 15;
			m_ToolTips.AddAnimation1s(&rl, -20,-50,0,-0);
		
		}
		

		
		
		
		
	}
	else if ( m_pObjectBase->dwType == ItemType_Xunzhang &&  m_pObjectBase->dwID != 3831) //段位勋章
	{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFont( namePartOne );	//soke 物品名
		if ( m_object.dmedalt >= 0 )
		{
			char szTmp[512];
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【等级 %u】", m_object.dmedalt);
			Tips_AddStringDefFont( szTmp );
		}
	}
	else if ( m_pObjectBase->dwType == ItemType_ShangfangBaojian ) //尚方宝剑
	{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFont( namePartOne );	//soke 物品名
		if ( m_object.shangfanglv >= 0 )
		{
			char szTmp[512];
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【等级 %u】", m_object.shangfanglv);
			Tips_AddStringDefFont( szTmp );
		}
	}
	else if ( m_pObjectBase->dwType == ItemType_ZijinLongpao ) //紫金龙袍
	{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFont( namePartOne );	//soke 物品名
		if ( m_object.zijinlplv >= 0 )
		{
			char szTmp[512];
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【等级 %u】", m_object.zijinlplv);
			Tips_AddStringDefFont( szTmp );
		}
	}
	else if ( m_pObjectBase->dwID == 3831) //段位勋章1
	{
		if (m_object.dmedalt >= 0 && m_object.dmedalt <= 100) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFont( "紧箍咒(一阶)" );	//soke 物品名			
			char szTmp[512];
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【等级 %u】", m_object.dmedalt);
			Tips_AddStringDefFont( szTmp );
		}
		if (m_object.dmedalt >= 101 && m_object.dmedalt <= 200) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFont( "紧箍咒(二阶)" );	//soke 物品名			
			char szTmp[512];
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【等级 %u】", m_object.dmedalt-100);
			Tips_AddStringDefFont( szTmp );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n紧箍咒：%u/100", m_object.dmedalt-100);
			Tips_AddStringDefFont( szTmp );			
		}	
		if (m_object.dmedalt >= 201 && m_object.dmedalt <= 300) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFont( "紧箍咒(三阶)" );	//soke 物品名			
			char szTmp[512];
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【等级 %u】", m_object.dmedalt-200);
			Tips_AddStringDefFont( szTmp );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n紧箍咒：%u/100", m_object.dmedalt-200);
			Tips_AddStringDefFont( szTmp );
		}	
		if (m_object.dmedalt >= 301 && m_object.dmedalt <= 400) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFont( "紧箍咒(四阶)" );	//soke 物品名			
			char szTmp[512];
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【等级 %u】", m_object.dmedalt-300);
			Tips_AddStringDefFont( szTmp );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n紧箍咒：%u/100", m_object.dmedalt-300);
			Tips_AddStringDefFont( szTmp );
		}
		if (m_object.dmedalt >= 401 && m_object.dmedalt <= 500) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFont( "紧箍咒(五阶)" );	//soke 物品名			
			char szTmp[512];
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【等级 %u】", m_object.dmedalt-400);
			Tips_AddStringDefFont( szTmp );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n紧箍咒：%u/100", m_object.dmedalt-400);
			Tips_AddStringDefFont( szTmp );
		}
		if (m_object.dmedalt >= 501 && m_object.dmedalt <= 600) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFont( "紧箍咒(六阶)" );	//soke 物品名			
			char szTmp[512];
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【等级 %u】", m_object.dmedalt-500);
			Tips_AddStringDefFont( szTmp );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n紧箍咒：%u/100", m_object.dmedalt-500);
			Tips_AddStringDefFont( szTmp );
		}
		if (m_object.dmedalt >= 601 && m_object.dmedalt <= 700) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFont( "紧箍咒(七阶)" );	//soke 物品名			
			char szTmp[512];
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【等级 %u】", m_object.dmedalt-600);
			Tips_AddStringDefFont( szTmp );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n紧箍咒：%u/100", m_object.dmedalt-600);
			Tips_AddStringDefFont( szTmp );
		}
		if (m_object.dmedalt >= 701 && m_object.dmedalt <= 800) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFont( "紧箍咒(八阶)" );	//soke 物品名			
			char szTmp[512];
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【等级 %u】", m_object.dmedalt-700);
			Tips_AddStringDefFont( szTmp );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n紧箍咒：%u/100", m_object.dmedalt-700);
			Tips_AddStringDefFont( szTmp );
		}
		if (m_object.dmedalt >= 801 && m_object.dmedalt <= 900) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFont( "紧箍咒(九阶)" );	//soke 物品名			
			char szTmp[512];
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【等级 %u】", m_object.dmedalt-800);
			Tips_AddStringDefFont( szTmp );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n紧箍咒：%u/100", m_object.dmedalt-800);
			Tips_AddStringDefFont( szTmp );
		}
		if (m_object.dmedalt >= 901 && m_object.dmedalt <= 1000) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFont( "紧箍咒(十阶)" );	//soke 物品名			
			char szTmp[512];
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【等级 %u】", m_object.dmedalt-900);
			Tips_AddStringDefFont( szTmp );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n紧箍咒：%u/100", m_object.dmedalt-900);
			Tips_AddStringDefFont( szTmp );			
		}
	    if (m_object.dmedalt >= 1)  //增加孙悟空特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 106;
		  rl.frame = 4;
		  m_ToolTips.AddAnimation1s(&rl,-150,+20,-25,-65);
		  //Tips_AddString( "\n" );
	    }				
		//sky 带一星的龙槽特效
		if (m_object.syxqgrade21 == 0) //强化等于1
		{	
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带一星的龙槽特效
		if (m_object.syxqgrade21 >= 1 && m_object.syxqgrade21 <= 50) //强化小于1
		{	
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 30;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带二星的龙槽特效
		if (m_object.syxqgrade21 >= 51 && m_object.syxqgrade21 <= 100) //强化等于51
		{	
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 30;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 30;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );			
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带三星的龙槽特效
		if (m_object.syxqgrade21 >= 101 && m_object.syxqgrade21 <= 150) //强化等于101
		{	
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 30;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 30;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 30;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );			
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带四星的龙槽特效
		if (m_object.syxqgrade21 >= 151 && m_object.syxqgrade21 <= 200) //强化等于151
		{	
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;									
			m_ToolTips.AddAnimation3( &Aangelvimg, false );			
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带五星的龙槽特效
		if (m_object.syxqgrade21 >= 201 && m_object.syxqgrade21 <= 250) //强化等于201
		{	
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );				
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带六星的龙槽特效
		if (m_object.syxqgrade21 >= 251 && m_object.syxqgrade21 <= 300) //强化等于251
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );							
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带七星的龙槽特效
		if (m_object.syxqgrade21 >= 301 && m_object.syxqgrade21 <= 350) //强化等于301
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );				
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带八星的龙槽特效
		if (m_object.syxqgrade21 >= 351 && m_object.syxqgrade21 <= 400) //强化等于351
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );				
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带九星的龙槽特效
		if (m_object.syxqgrade21 >= 401 && m_object.syxqgrade21 <= 450) //强化等于401
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );			
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带十星的龙槽特效
		if (m_object.syxqgrade21 >= 451 && m_object.syxqgrade21 <= 500) //强化等于451
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );		
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );				
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带十一星的龙槽特效
		if (m_object.syxqgrade21 >= 501 && m_object.syxqgrade21 <= 550) //强化等于501
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );		
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );				
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带十二星的龙槽特效
		if (m_object.syxqgrade21 >= 551 && m_object.syxqgrade21 <= 600) //强化等于551
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );		
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );			
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带十三星的龙槽特效
		if (m_object.syxqgrade21 >= 601 && m_object.syxqgrade21 <= 650) //强化等于601
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );		
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );				
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带十四星的龙槽特效
		if (m_object.syxqgrade21 >= 651 && m_object.syxqgrade21 <= 700) //强化等于651
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );		
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );				
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}																			
	}
		//sky 带十五星的龙槽特效
		if (m_object.syxqgrade21 >= 701 && m_object.syxqgrade21 <= 1000) //强化等于701
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );		
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );				
     	if (m_object.dmedalt >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.dmedalt >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.dmedalt >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}																			
	    }
		if (m_object.syxqgrade23 >= 1 && m_object.syxqgrade23 <= 5) //增加金箍淬炼框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 1;
		  m_ToolTips.AddAnimation1s(&rl, -405,-32,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade23 >= 6 && m_object.syxqgrade23 <= 10) //增加金箍淬炼框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 2;
		  m_ToolTips.AddAnimation1s(&rl, -405,-32,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade23 >= 11 && m_object.syxqgrade23 <= 15) //增加金箍淬炼框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 3;
		  m_ToolTips.AddAnimation1s(&rl, -405,-32,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade23 >= 16 && m_object.syxqgrade23 <= 20) //增加金箍淬炼框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 4;
		  m_ToolTips.AddAnimation1s(&rl, -405,-32,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade23 >= 21 && m_object.syxqgrade23 <= 25) //增加金箍淬炼框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 5;
		  m_ToolTips.AddAnimation1s(&rl, -405,-32,-25,-65);
		  //Tips_AddString( "\n" );
	    }	
  	    if (m_object.syxqgrade23 >= 26 && m_object.syxqgrade23 <= 30) //增加金箍淬炼框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 6;
		  m_ToolTips.AddAnimation1s(&rl, -405,-32,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade23 >= 31 && m_object.syxqgrade23 <= 35) //增加金箍淬炼框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 7;
		  m_ToolTips.AddAnimation1s(&rl, -405,-32,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade23 >= 36 && m_object.syxqgrade23 <= 40) //增加金箍淬炼框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 8;
		  m_ToolTips.AddAnimation1s(&rl, -405,-32,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade23 >= 41 && m_object.syxqgrade23 <= 45) //增加金箍淬炼框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 9;
		  m_ToolTips.AddAnimation1s(&rl, -405,-32,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade23 >= 46 && m_object.syxqgrade23 <= 50) //增加金箍淬炼框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 10;
		  m_ToolTips.AddAnimation1s(&rl, -405,-32,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade23 >= 51 && m_object.syxqgrade23 <= 55) //增加金箍淬炼框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 11;
		  m_ToolTips.AddAnimation1s(&rl, -405,-32,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade23 >= 56 && m_object.syxqgrade23 <= 60) //增加金箍淬炼框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 12;
		  m_ToolTips.AddAnimation1s(&rl, -405,-32,-25,-65);
		  //Tips_AddString( "\n" );
	    }	
  	    if (m_object.syxqgrade23 >= 61 && m_object.syxqgrade23 <= 65) //增加金箍淬炼框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 13;
		  m_ToolTips.AddAnimation1s(&rl, -405,-32,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade23 >= 66 && m_object.syxqgrade23 <= 70) //增加金箍淬炼框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 14;
		  m_ToolTips.AddAnimation1s(&rl, -405,-32,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade23 >= 71 && m_object.syxqgrade23 <= 100) //增加金箍淬炼框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 15;
		  m_ToolTips.AddAnimation1s(&rl, -405,-32,-25,-65);
		  //Tips_AddString( "\n" );
	    }																						
	}
	else if ( m_pObjectBase->dwType == ItemType_Hanbing ) //寒冰麒麟
	{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFont( namePartOne );	//soke 物品名
		if ( m_object.retain1 >= 0 )
		{
			char szTmp[512];
			if ( m_object.dwObjectID == 3865)
			{
				sprintf(szTmp, "【五彩鎏光 %u阶】", m_object.retain1);
			}
			else if ( m_object.dwObjectID == 3864)
			{
				sprintf(szTmp, "【紫色 %u阶】", m_object.retain1);
			}
			else if ( m_object.dwObjectID == 3863)
			{
				sprintf(szTmp, "【绿色 %u阶】", m_object.retain1);
			}
			else if ( m_object.dwObjectID == 3862)
			{
				sprintf(szTmp, "【黄色 %u阶】", m_object.retain1);
			}
			else if ( m_object.dwObjectID == 3861)
			{
				sprintf(szTmp, "【蓝色 %u阶】", m_object.retain1);
			}
			else if ( m_object.dwObjectID == 3860)
			{
				sprintf(szTmp, "【白色 %u阶】", m_object.retain1);
			}
			Tips_AddStringDefFont( szTmp );
		}
		//sky 带一星的龙槽特效寒冰麒麟
	    if ( m_object.dwObjectID == 3865) //3865道具ID
	    {						
		if (m_object.syxqgrade24 == 0) //强化等于1
		{	
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带一星的龙槽特效
		if (m_object.syxqgrade24 >= 1 && m_object.syxqgrade24 <= 50) //强化小于1
		{	
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 30;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带二星的龙槽特效
		if (m_object.syxqgrade24 >= 51 && m_object.syxqgrade24 <= 100) //强化等于51
		{	
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 30;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 30;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );			
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带三星的龙槽特效
		if (m_object.syxqgrade24 >= 101 && m_object.syxqgrade24 <= 150) //强化等于101
		{	
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 30;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 30;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 30;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );			
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带四星的龙槽特效
		if (m_object.syxqgrade24 >= 151 && m_object.syxqgrade24 <= 200) //强化等于151
		{	
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;									
			m_ToolTips.AddAnimation3( &Aangelvimg, false );			
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带五星的龙槽特效
		if (m_object.syxqgrade24 >= 201 && m_object.syxqgrade24 <= 250) //强化等于201
		{	
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );				
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带六星的龙槽特效
		if (m_object.syxqgrade24 >= 251 && m_object.syxqgrade24 <= 300) //强化等于251
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 32;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );							
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带七星的龙槽特效
		if (m_object.syxqgrade24 >= 301 && m_object.syxqgrade24 <= 350) //强化等于301
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );				
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带八星的龙槽特效
		if (m_object.syxqgrade24 >= 351 && m_object.syxqgrade24 <= 400) //强化等于351
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );				
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带九星的龙槽特效
		if (m_object.syxqgrade24 >= 401 && m_object.syxqgrade24 <= 450) //强化等于401
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 34;
			m_ToolTips.AddAnimation3( &Aangelvimg, false );			
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带十星的龙槽特效
		if (m_object.syxqgrade24 >= 451 && m_object.syxqgrade24 <= 500) //强化等于451
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );		
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );				
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带十一星的龙槽特效
		if (m_object.syxqgrade24 >= 501 && m_object.syxqgrade24 <= 550) //强化等于501
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );		
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );				
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带十二星的龙槽特效
		if (m_object.syxqgrade24 >= 551 && m_object.syxqgrade24 <= 600) //强化等于551
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );		
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 36;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );			
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带十三星的龙槽特效
		if (m_object.syxqgrade24 >= 601 && m_object.syxqgrade24 <= 650) //强化等于601
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );		
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );				
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation( &Aangelvimg, false );
		}																			
	}
		//sky 带十四星的龙槽特效
		if (m_object.syxqgrade24 >= 651 && m_object.syxqgrade24 <= 700) //强化等于651
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );		
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );				
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation5( &Aangelvimg, false );
		}																			
	}
		//sky 带十五星的龙槽特效
		if (m_object.syxqgrade24 >= 701 && m_object.syxqgrade24 <= 1000) //强化等于701
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );		
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );	
			Aangelvimg.SetFileName( "data\\interfaces7.gl");
			Aangelvimg.group = 32;
			Aangelvimg.frame = 51;
			m_ToolTips.AddAnimation4( &Aangelvimg, false );				
     	if (m_object.retain1 >= 1)  //正常紫星效果
		{
			Tips_AddStringDefFont( "\n" );
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 51)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 101)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 151)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 201)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 251)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 301)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 351)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 401)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 451)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 501)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}	
		if (m_object.retain1 >= 551)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 601)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 651)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}
		if (m_object.retain1 >= 701)  //正常紫星效果
		{
			stResourceLocation Aangelvimg;
			Aangelvimg.SetFileName( "data\\interfaces.gl");
			Aangelvimg.group = 4;
			Aangelvimg.frame = 506;
			m_ToolTips.AddAnimation6( &Aangelvimg, false );
		}																			
	    }
		if (m_object.syxqgrade26 >= 1 && m_object.syxqgrade26 <= 5) //增加寒冰纹章框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 1;
		  m_ToolTips.AddAnimation1s(&rl, -405,-33,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade26 >= 6 && m_object.syxqgrade26 <= 10) //增加寒冰纹章框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 2;
		  m_ToolTips.AddAnimation1s(&rl, -405,-33,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade26 >= 11 && m_object.syxqgrade26 <= 15) //增加寒冰纹章框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 3;
		  m_ToolTips.AddAnimation1s(&rl, -405,-33,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade26 >= 16 && m_object.syxqgrade26 <= 20) //增加寒冰纹章框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 4;
		  m_ToolTips.AddAnimation1s(&rl, -405,-33,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade26 >= 21 && m_object.syxqgrade26 <= 25) //增加寒冰纹章框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 5;
		  m_ToolTips.AddAnimation1s(&rl, -405,-33,-25,-65);
		  //Tips_AddString( "\n" );
	    }	
  	    if (m_object.syxqgrade26 >= 26 && m_object.syxqgrade26 <= 30) //增加寒冰纹章框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 6;
		  m_ToolTips.AddAnimation1s(&rl, -405,-33,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade26 >= 31 && m_object.syxqgrade26 <= 35) //增加寒冰纹章框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 7;
		  m_ToolTips.AddAnimation1s(&rl, -405,-33,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade26 >= 36 && m_object.syxqgrade26 <= 40) //增加寒冰纹章框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 8;
		  m_ToolTips.AddAnimation1s(&rl, -405,-33,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade26 >= 41 && m_object.syxqgrade26 <= 45) //增加寒冰纹章框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 9;
		  m_ToolTips.AddAnimation1s(&rl, -405,-33,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade26 >= 46 && m_object.syxqgrade26 <= 50) //增加寒冰纹章框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 10;
		  m_ToolTips.AddAnimation1s(&rl, -405,-33,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade26 >= 51 && m_object.syxqgrade26 <= 55) //增加寒冰纹章框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 11;
		  m_ToolTips.AddAnimation1s(&rl, -405,-33,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade26 >= 56 && m_object.syxqgrade26 <= 60) //增加寒冰纹章框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 12;
		  m_ToolTips.AddAnimation1s(&rl, -405,-33,-25,-65);
		  //Tips_AddString( "\n" );
	    }	
  	    if (m_object.syxqgrade26 >= 61 && m_object.syxqgrade26 <= 65) //增加寒冰纹章框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 13;
		  m_ToolTips.AddAnimation1s(&rl, -405,-33,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade26 >= 66 && m_object.syxqgrade26 <= 70) //增加寒冰纹章框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 14;
		  m_ToolTips.AddAnimation1s(&rl, -405,-33,-25,-65);
		  //Tips_AddString( "\n" );
	    }
  	    if (m_object.syxqgrade26 >= 71 && m_object.syxqgrade26 <= 100) //增加寒冰纹章框特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 4;
		  rl.frame = 15;
		  m_ToolTips.AddAnimation1s(&rl, -405,-33,-25,-65);
		  //Tips_AddString( "\n" );
	    }																						
	}		
	}
	else if ( m_pObjectBase->dwType == ItemType_Chibang ) //翅膀
	{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		Tips_AddStringDefFontNAME( namePartOne );	//soke 物品名
		if ( m_object.retain1 >= 0 )
	  	{
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));

			if (m_object.retain4 >= 1601 &&  m_object.retain4 <= 1800)
			{
				sprintf(szTmp, "\n当前羽翼品质:九翼\n精炼等级: %u级", (m_object.retain4 - 1600));
			}
			else if (m_object.retain4 >= 1401 &&  m_object.retain4 <= 1600)
			{
				sprintf(szTmp, "\n当前羽翼品质:八翼\n精炼等级: %u级", (m_object.retain4 - 1400));
			}
			else if (m_object.retain4 >= 1201 &&  m_object.retain4 <= 1400)
			{
				sprintf(szTmp, "\n当前羽翼品质:七翼\n精炼等级: %u级", (m_object.retain4 - 1200));
			}
			else if (m_object.retain4 >= 1001 &&  m_object.retain4 <= 1200)
			{
				sprintf(szTmp, "\n当前羽翼品质:六翼\n精炼等级: %u级", (m_object.retain4 - 1000));
			}
			else if (m_object.retain4 >= 801 &&  m_object.retain4 <= 1000)
			{
				sprintf(szTmp, "\n当前羽翼品质:五翼\n精炼等级: %u级", (m_object.retain4 - 800));
			}
			else if (m_object.retain4 >= 601 &&  m_object.retain4 <= 800)
			{
				sprintf(szTmp, "\n当前羽翼品质:四翼\n精炼等级: %u级", (m_object.retain4 - 600));
			}
			else if (m_object.retain4 >= 401 &&  m_object.retain4 <= 600)
			{
				sprintf(szTmp, "\n当前羽翼品质:三翼\n精炼等级: %u级", (m_object.retain4 - 400));
			}
			else if (m_object.retain4 >= 201 &&  m_object.retain4 <= 400)
			{
				sprintf(szTmp, "\n当前羽翼品质:二翼\n精炼等级: %u级", (m_object.retain4 - 200));
			}
			else if (m_object.retain4 >= 0 &&  m_object.retain4 <= 200)
			{
				sprintf(szTmp, "\n当前羽翼品质:一翼\n精炼等级: %u级", m_object.retain4);
			}
			Tips_AddString( szTmp );
		}
		    
	}
	else if ( m_pObjectBase->dwType >= ItemType_Shenbing0 && m_pObjectBase->dwType <= ItemType_Shenbing2) //神兵
	{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);
		if ( m_object.retain5 > 0)
		{
			Tips_AddStringDefFont( "神兵·" );
		}
		Tips_AddStringDefFontNAME( namePartOne );	//soke 物品名
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));

		if ( m_object.retain5 >= 0 )
		{
			char szTmp[512];
			if ( m_object.retain5 >= 81 && m_object.retain5 <= 10000)
			{
				sprintf(szTmp, "\n当前神兵品质：六阶\n精炼等级：+ %u 级",(m_object.retain5));
			}
			else if ( m_object.retain5 >= 61 && m_object.retain5 <= 8000)
			{
				sprintf(szTmp, "\n当前神兵品质：五阶\n精炼等级：+ %u 级",(m_object.retain5));
			}
			else if ( m_object.retain5 >= 41 && m_object.retain5 <= 6000)
			{
				sprintf(szTmp, "\n当前神兵品质：四阶\n精炼等级：+ %u 级",(m_object.retain5));
			}
			else if ( m_object.retain5 >= 21 && m_object.retain5 <= 4000)
			{
				sprintf(szTmp, "\n当前神兵品质：三阶\n精炼等级：+ %u 级",(m_object.retain5));
			}
			else if ( m_object.retain5 >= 11 && m_object.retain5 <= 2000)
			{
				sprintf(szTmp, "\n当前神兵品质：二阶\n精炼等级：+ %u 级",(m_object.retain5));
			}
			else if ( m_object.retain5 >= 0 && m_object.retain5 <= 1000)
			{
				sprintf(szTmp, "\n当前神兵品质：一阶\n精炼等级：+ %u 级",m_object.retain5);
			}
			Tips_AddString( szTmp );
		}

		Tips_AddStringDefFont( "\n" );

		if (m_object.retain5 >= 1)
		{
			stResourceLocation rHole1;
			rHole1.SetFileName( "data\\interfaces1.gl");
			rHole1.group = 10;
			rHole1.frame = 5;
			m_ToolTips.AddAnimation( &rHole1, false );
		}
		if (m_object.retain5 >= 11)
		{
			stResourceLocation rHole2;
			rHole2.SetFileName( "data\\interfaces1.gl");
			rHole2.group = 10;
			rHole2.frame = 5;
			m_ToolTips.AddAnimation( &rHole2, false );
		}
		if (m_object.retain5 >= 21)
		{
			stResourceLocation rHole3;
			rHole3.SetFileName( "data\\interfaces1.gl");
			rHole3.group = 10;
			rHole3.frame = 5;
			m_ToolTips.AddAnimation( &rHole3, false );
		}
		if (m_object.retain5 >= 41)
		{
			stResourceLocation rHole4;
			rHole4.SetFileName( "data\\interfaces1.gl");
			rHole4.group = 10;
			rHole4.frame = 5;
			m_ToolTips.AddAnimation( &rHole4, false );
		}
		if (m_object.retain5 >= 61)
		{
			stResourceLocation rHole5;
			rHole5.SetFileName( "data\\interfaces1.gl");
			rHole5.group = 10;
			rHole5.frame = 5;
			m_ToolTips.AddAnimation( &rHole5, false );
		}
		if (m_object.retain5 >= 81)
		{
			stResourceLocation rHole6;
			rHole6.SetFileName( "data\\interfaces1.gl");
			rHole6.group = 10;
			rHole6.frame = 5;
			m_ToolTips.AddAnimation( &rHole6, false );
		}
	}
	else if ( m_pObjectBase->dwType == ItemType_Fabao) //法宝
	{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,0));
		
			Tips_AddStringDefFont( "法宝·" );
		
		Tips_AddStringDefFont( namePartOne );	//soke 物品名
		if ( m_object.retain20 >= 0 )
		{
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【%u阶】", m_object.retain20);
			Tips_AddStringDefFont( szTmp );
		}

		
	}
	else if ( m_pObjectBase->dwType == ItemType_YUANSHEN) //元神
	{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,0));
		
			Tips_AddStringDefFont( "元神·" );
		
		Tips_AddStringDefFont( namePartOne );	//soke 物品名
		if ( m_object.retain22 >= 0 )
		{
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【%u阶】", m_object.retain22);
			Tips_AddStringDefFont( szTmp );
		}

		
	}
	else if ( m_pObjectBase->dwType >= ItemType_SEXX1 && m_pObjectBase->dwType <= ItemType_SEXX12) //十二星宿
	{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,0));
		
			Tips_AddStringDefFont( "十二星宿·" );
		
		Tips_AddStringDefFont( namePartOne );	//soke 物品名
		if ( m_object.retain21 >= 0 )
		{
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【%u阶】", m_object.retain21);
			Tips_AddStringDefFont( szTmp );
		}

		
	}
	else if ( m_pObjectBase->dwType>=ItemType_Fabaofj1 && m_pObjectBase->dwType<=ItemType_Fabaofj12) //法宝
	{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,0));
		
			
		
		Tips_AddStringDefFont( namePartOne );	//soke 物品名

		if ( m_object.retain20 >= 0 )
		{
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【%u阶】", m_object.retain20);
			Tips_AddStringDefFont( szTmp );
		}
	}
	else if ( m_pObjectBase->dwType>=ItemType_YUANSHENFJ1 && m_pObjectBase->dwType<=ItemType_YUANSHENFJ6) //元神
	{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,0));
		
			
		
		Tips_AddStringDefFont( namePartOne );	//soke 物品名

		if ( m_object.retain22 >= 0 )
		{
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			sprintf(szTmp, "【%u阶】", m_object.retain22);
			Tips_AddStringDefFont( szTmp );
		}
	}
	else if ( m_pObjectBase->dwType>=ItemType_BABYFJ1 && m_pObjectBase->dwType<=ItemType_BABYFJ6) //宝宝
	{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,0));
		
			Tips_AddStringDefFont( "儿女装备·" );
		
			
		
		Tips_AddStringDefFont( namePartOne );	//soke 物品名

		if ( m_object.retain22 >= 0 )
		{
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			sprintf(szTmp, "【强化 %u 阶】", m_object.retain35);
			Tips_AddStringDefFont( szTmp );
		}
	}
	else if ( m_pObjectBase->dwType == ItemType_Shenshou1 || m_pObjectBase->dwType == ItemType_Shenshou2 
	|| m_pObjectBase->dwType == ItemType_ShangfangBaojian || m_pObjectBase->dwType == ItemType_ZijinLongpao) //神兽
	{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);  
		if ( m_object.retain6 > 0)  
		{  
			Tips_AddStringDefFont( "稀世·" );  
		}  
		Tips_AddStringDefFontNAME( namePartOne ); //soke 物品名  

		if ( m_object.retain6 >= 0)  
		{  
			char szTmp[512];  
			if ( m_object.retain6 >= 81 && m_object.retain6 <= 10000) //原先是100，修改为10000  
			{  
				sprintf(szTmp, " 六品+%u",(m_object.retain6-80));  
			}  
			else if ( m_object.retain6 >= 61 && m_object.retain6 <= 8000) //原先是80，修改为8000  
			{  
				sprintf(szTmp, " 五品+%u",(m_object.retain6-60));  
			}  
			else if ( m_object.retain6 >= 41 && m_object.retain6 <= 6000) //原先是60，修改为6000  
			{  
				sprintf(szTmp, " 四品+%u",(m_object.retain6-40));  
			}  
			else if ( m_object.retain6 >= 21 && m_object.retain6 <= 4000) //原先是40，修改为4000  
			{  
				sprintf(szTmp, " 三品+%u",(m_object.retain6-20));  
			}  
			else if ( m_object.retain6 >= 11 && m_object.retain6 <= 2000) //原先是20，修改为2000  
			{  
				sprintf(szTmp, " 二品+%u",(m_object.retain6-10));  
			}  
			else if ( m_object.retain6 >= 1 && m_object.retain6 <= 1000) //原先是10，修改为1000  
			{  
				sprintf(szTmp, " 一品+%u",m_object.retain6);  
			}  
			Tips_AddStringDefFont( szTmp );  
		}

		Tips_AddStringDefFont( "\n" );
		if (m_object.retain6 >= 1)
		{
			stResourceLocation rHole1;
			rHole1.SetFileName( "data\\interfaces1.gl");
			rHole1.group = 10;
			rHole1.frame = 6;
			m_ToolTips.AddAnimation( &rHole1, false );
		}
		if (m_object.retain6 >= 11)
		{
			stResourceLocation rHole2;
			rHole2.SetFileName( "data\\interfaces1.gl");
			rHole2.group = 10;
			rHole2.frame = 6;
			m_ToolTips.AddAnimation( &rHole2, false );
		}
		if (m_object.retain6 >= 21)
		{
			stResourceLocation rHole3;
			rHole3.SetFileName( "data\\interfaces1.gl");
			rHole3.group = 10;
			rHole3.frame = 6;
			m_ToolTips.AddAnimation( &rHole3, false );
		}
		if (m_object.retain6 >= 41)
		{
			stResourceLocation rHole4;
			rHole4.SetFileName( "data\\interfaces1.gl");
			rHole4.group = 10;
			rHole4.frame = 6;
			m_ToolTips.AddAnimation( &rHole4, false );
		}
		if (m_object.retain6 >= 61)
		{
			stResourceLocation rHole5;
			rHole5.SetFileName( "data\\interfaces1.gl");
			rHole5.group = 10;
			rHole5.frame = 6;
			m_ToolTips.AddAnimation( &rHole5, false );
		}
		if (m_object.retain6 >= 81)
		{
			stResourceLocation rHole6;
			rHole6.SetFileName( "data\\interfaces1.gl");
			rHole6.group = 10;
			rHole6.frame = 6;
			m_ToolTips.AddAnimation( &rHole6, false );
		}
	}
	else
	{
		if( ( (m_pObjectBase->dwType == ItemType_Resource)
			|| (m_pObjectBase->dwType == ItemType_SOULSTONE) 
			|| (m_pObjectBase->dwType == ItemType_LevelUp) )
			&& ( (m_pObjectBase->dwID == 506 )
			|| (m_pObjectBase->dwID == 507)
			|| (m_pObjectBase->dwID == 516)
			|| (m_pObjectBase->dwID == 517)
			|| (m_pObjectBase->dwID == 526)
			|| (m_pObjectBase->dwID == 527)
			|| (m_pObjectBase->dwID == 536)
			|| (m_pObjectBase->dwID == 537)
			|| (m_pObjectBase->dwID == 546)
			|| (m_pObjectBase->dwID == 547)
			|| (m_pObjectBase->dwID == 548)
			|| (m_pObjectBase->dwID == 550)
			|| (m_pObjectBase->dwID == 552)
			|| (m_pObjectBase->dwID == 561)
			|| (m_pObjectBase->dwID == 562)
			|| (m_pObjectBase->dwID == 563)
			|| (m_pObjectBase->dwID == 564)
			|| (m_pObjectBase->dwID == 565)
			|| (m_pObjectBase->dwID == 852)
			|| (m_pObjectBase->dwID == 877)
			|| (m_pObjectBase->dwID == 1507)
			|| (m_pObjectBase->dwID == 1517)
			|| (m_pObjectBase->dwID == 1527)
			|| (m_pObjectBase->dwID == 1537)
			|| (m_pObjectBase->dwID == 1547)) )	//Set id limit		//劳动原料类		m_pObjectBase->dwType == 16
		{
			//Set color
			switch( m_object.upgrade )
			{
			case 0:
				m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
				break;
			case 1:
				m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,86,234) );
				break;
			case 2:
				m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,222,3) );
				break;
			case 3:
				m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,255,0) );
				break;
			case 4:
				m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
				break;
			case 5:
				m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
				break;
			case 6:
				m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
				break;
			case 7:
				m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
				break;
			case 8:
				m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
				break;
			case 9:
				m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
				break;
			}

				Tips_AddStringDefFont( namePartOne );

				if ( strlen(nameFiveFull) > 0 )
				{
					//Back cur color
					DWORD color = m_ToolTips.GetCurColor();
					m_ToolTips.SetCurColor( fiveColor[m_object.fivetype] );
					Tips_AddStringDefFont( nameFiveFull );
					m_ToolTips.SetCurColor( color );
				}

				//Add upgrade
				Tips_AddStringDefFont( upgradeName[m_object.upgrade] );	

				//Add name part two
				if ( strlen(namePartTwo) > 0 )
				{
					Tips_AddString( "\n" );
					Tips_AddStringDefFont( namePartTwo );
				}
			}
			else
			{
				if ( (m_pObjectBase->dwID == 678)
					|| (m_pObjectBase->dwID == 679)

					|| (m_pObjectBase->dwID == 681)
					|| (m_pObjectBase->dwID == 792) 
					|| (m_pObjectBase->dwID == 794)
					|| (m_pObjectBase->dwID == 795)
					|| (m_pObjectBase->dwID == 799)
					|| (m_pObjectBase->dwID == 1170)
					|| (m_pObjectBase->dwID == 4300)

					|| (m_pObjectBase->dwID == 791)
					|| (m_pObjectBase->dwID == 790)

					|| (m_pObjectBase->dwID == 680)
					|| (m_pObjectBase->dwID == 748)
					|| (m_pObjectBase->dwID == 749)
					|| (m_pObjectBase->dwID == 750)
					|| (m_pObjectBase->dwID == 751))		//Diamond
				{
					if ( (m_pObjectBase->dwID == 678)
						|| (m_pObjectBase->dwID == 679) )
					{
						switch( m_object.upgrade )
						{
						case 0:
							m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
							break;
						case 1:
							m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,86,234) );
							break;
						case 2:
							m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,222,3) );
							break;
						case 3:
							m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,255,0) );
							break;
						case 4:
							m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
							break;
						case 5:
							m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
							break;
						case 6:
							m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
							break;
						case 7:
							m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
							break;
						case 8:
							m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
							break;
						case 9:
						case 10:
						case 11:
							m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
							break;
						default:
							m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
							break;
						}
					}
					else if ( (m_pObjectBase->dwID == 681)
						|| (m_pObjectBase->dwID == 792) 
						|| (m_pObjectBase->dwID == 794)
						|| (m_pObjectBase->dwID == 795)
						|| (m_pObjectBase->dwID == 799)
						|| (m_pObjectBase->dwID == 1170)
						|| (m_pObjectBase->dwID == 4300))
					{
						if ( (m_pObjectBase->dwID == 681)
							|| (m_pObjectBase->dwID == 795)
							|| (m_pObjectBase->dwID == 799)
							|| (m_pObjectBase->dwID == 1170)
							|| (m_pObjectBase->dwID == 4300))
							m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,255,137));
						else
							m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,255,137) );
					}
					else if ( (m_pObjectBase->dwID == 791)
						|| (m_pObjectBase->dwID == 790) )
					{
						m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,255,137) );
					}
					else if ( (m_pObjectBase->dwID == 680)
						|| (m_pObjectBase->dwID == 748)
						|| (m_pObjectBase->dwID == 749)
						|| (m_pObjectBase->dwID == 750)
						|| (m_pObjectBase->dwID == 751) )
					{
						m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,255,137) );
					}
					else
					{
						m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
					}

					Tips_AddStringDefFont( namePartOne );	//m_object.strName

					if ( strlen(nameFiveFull) > 0 )
					{
						//Back cur color
						DWORD color = m_ToolTips.GetCurColor();
						m_ToolTips.SetCurColor( fiveColor[m_object.fivetype] );
						Tips_AddStringDefFont( nameFiveFull );
						m_ToolTips.SetCurColor( color );
					}

					if ( (m_pObjectBase->dwID == 678)
						|| (m_pObjectBase->dwID == 679) )
					{
						Tips_AddStringDefFont( upgradeName[m_object.upgrade] );
					}

					if ( strlen( namePartTwo ) > 0 )
					{
						Tips_AddString( "\n" );
						Tips_AddStringDefFont( namePartTwo );
					}
				}
				else
				{
					//soke 这里增加类型就是可以支持自定义补丁物品的颜色了
					//桃子 这里添加类型就是可以支持自定义补丁物品的颜色了-这里只是在包袱内的显示
					if ( (m_pObjectBase->dwType == ItemType_ZhenSkillUp)
						|| (m_pObjectBase->dwType == ItemType_Scrxll) //soke 15类型
						|| (m_pObjectBase->dwType == ItemType_LevelUp) //soke 27类型
						|| (m_pObjectBase->dwType == ItemType_Gift) //soke 31类型
						|| (m_pObjectBase->dwType == 34) //soke 34类型
						|| (m_pObjectBase->dwType == 44) //soke 44类型
						|| (m_pObjectBase->dwType == ItemType_Xuest) //soke 49类型
						|| (m_pObjectBase->dwType == ItemType_DoubleExpZ) //soke 66类型
						|| (m_pObjectBase->dwType == ItemType_DoubleExpC) //soke 67类型
						|| (m_pObjectBase->dwType == ItemType_DoubleExpG) //soke 68类型
					    || (m_pObjectBase->dwType == 73) //soke 73类型
					    || (m_pObjectBase->dwType == 74) //soke 74类型
					    || (m_pObjectBase->dwType == 78)  //soke 78类型
						|| (m_pObjectBase->dwType == 72) //soke 72类型
						|| (m_pObjectBase->dwType == 126) //soke 126宝盒类型
						|| (m_pObjectBase->dwType == 127) //soke 127钥匙类型
						|| (m_pObjectBase->dwType == 128) //soke 128御赐类型												
						|| (m_pObjectBase->dwType == ItemType_FashionBody) //soke 119类型
						|| (m_pObjectBase->dwType == ItemType_HighFashionBody) //soke 124类型
						|| (m_pObjectBase->dwType == 126) //soke 126白云装类型
						|| (m_pObjectBase->dwType == 203)
						|| (m_pObjectBase->dwType == 204)
						|| (m_pObjectBase->dwType == 205)
                        || (m_pObjectBase->dwType == 217)
						|| (m_pObjectBase->dwType == 218)
						|| (m_pObjectBase->dwType == 140)
						|| (m_pObjectBase->dwID == 756)
						|| (m_pObjectBase->dwID == 757)
						|| (m_pObjectBase->dwID == 760)
						|| (m_pObjectBase->dwID == 761) )
                    m_ToolTips.SetCurColor( m_pObjectBase->attackSound | 0xff000000);

					Tips_AddStringDefFont( namePartOne );	//m_object.strName

					if ( strlen(nameFiveFull) > 0 )
					{
						//Back cur color
						DWORD color = m_ToolTips.GetCurColor();
						m_ToolTips.SetCurColor( fiveColor[m_object.fivetype] );
						Tips_AddStringDefFont( nameFiveFull );
						m_ToolTips.SetCurColor( color );
					}

					if ( strlen( namePartTwo ) > 0 )
					{
						Tips_AddString( "\n" );
						Tips_AddStringDefFont( namePartTwo );
					}

					//Add grade
					char szUpgrade[MAX_NAMESIZE];
					szUpgrade[0] = 0L;
					for( int k=0;k<m_object.upgrade;k++ )
						strcat( szUpgrade, "★" );
					szUpgrade[strlen(szUpgrade)] = 0L;

					switch( m_object.upgrade )
					{
					case 0:
						break;
					case 1:
						m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
						break;
					case 2:
						m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,86,234) );
						break;
					case 3:
						m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,86,234) );
						break;
					case 4:
						m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,222,3) );
						break;
					case 5:
						m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,222,3) );
						break;
					case 6:
						m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,255,0) );
						break;
					case 7:
						m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,0,255,0) );
						break;
					case 8:
						m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,255,113,0) );
						break;
					case 9:
					case 10:
					case 11:
						m_ToolTips.SetCurColor( D3DCOLOR_ARGB(255,211,0,237) );
						break;
					}

				if ( strlen(szUpgrade) > 0 )
				{
					Tips_AddStringDefFont( "\n" );
					if ( (m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118) || (m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 134)|| (m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147)
				       || m_pObjectBase->dwType == 136 || m_pObjectBase->dwType == 137 || m_pObjectBase->dwType == 138 || m_pObjectBase->dwType == 155 || m_pObjectBase->dwType == 156 
					   &&  (m_object.upgrade > 9) )
					{
						stResourceLocation rHole;
						rHole.SetFileName( "data\\interfaces.gl" );
						rHole.group = 4;
						if (m_object.upgrade == 10)
						{
						    rHole.frame = 14;
						}
						else if (m_object.upgrade == 11)
						{
							rHole.frame = 15;
						}
						else if (m_object.upgrade == 12)
						{
							rHole.frame = 50;
						}
						else if (m_object.upgrade == 13 || m_object.upgrade == 14)
						{
							rHole.frame = 50;
						}
						else if (m_object.upgrade > 14)
						{
							rHole.frame = 50;
						}
						for( int i=0;i<m_object.upgrade;i++ )
							m_ToolTips.AddAnimation( &rHole, false );
					}
					else
						Tips_AddStringDefFont( szUpgrade );
				}
			}			
		}
	}

	//升星石
	if(m_pObjectBase->dwID >= 2960 && m_pObjectBase->dwID <= 2974)
	{
		Tips_AddString( "\n" );
		stResourceLocation rHole;
		rHole.SetFileName("data\\interfaces.gl");
		rHole.group = 4;
		int oblevel = m_pObjectBase->dwID - 2959;
		if (oblevel > 0 && oblevel < 6)
			rHole.frame = 500;
		else if(oblevel > 5 && oblevel < 11)
			rHole.frame = 501;
		else if(oblevel == 11)
			rHole.frame = 502;
		else if(oblevel == 12)
			rHole.frame = 503;
		else if(oblevel == 13)
			rHole.frame = 504;
		else if(oblevel == 14)
			rHole.frame = 505;
		else if(oblevel == 15)
			rHole.frame = 506;
		for (int i = 0; i < oblevel; i++)
			m_ToolTips.AddAnimation(&rHole, false);
		//Tips_AddString( "\n" );
	}

	//桃子 取消显示打造的套装 
	{
		if( (m_object.dwObjectID != c_nHuoYunFu) &&
			(m_object.dwObjectID != c_nXianTianFu) &&
			(m_object.dwObjectID != c_nXuanYuanFu) &&	
			(m_object.dwObjectID != 675) &&
			(m_object.dwObjectID != 676) &&
			(m_object.dwObjectID != 786) &&  // 小护身符
			(m_object.dwObjectID != 787))    // 大护身符
		{
			
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
			Tips_AddText("\n%s打造",m_object.maker );
		}
	}

	// Bind state
	if ( 1 == m_object.bind && (m_pObjectBase->dwType != ItemType_Amulets) )
	{
		//sky 龙槽判断
		if (m_object.dtrough > 0)
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,122,210,58) );
			Tips_AddString( "\n\n已绑定" );
		}
		else
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255, 122, 210, 58));
			Tips_AddString("\n已绑定");
		}
	}
	
	//soke 护身符绑定
	if ( 1 == m_object.bind && (m_pObjectBase->dwType == ItemType_Amulets) )
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,122,210,58) );
		Tips_AddString( "\n\n已绑定" );
	}

		//Blank line
		Tips_AddString( "\n    " );

		//If this is a ItemType_ZhenSkillUp type item
		//Add serial type
		if ( m_pObjectBase->dwType == ItemType_ZhenSkillUp )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n秘籍名称 " );
			/*if ( (m_pObjectBase->dwID >= 8300) && (m_pObjectBase->dwID <= 8399) )
			{
				Tips_AddString( "奇兵阵法" );
			}
			else if ( (m_pObjectBase->dwID >= 8400) && (m_pObjectBase->dwID <= 8499) )
			{
				Tips_AddString( "诡流阵法" );
			}
			else if ( (m_pObjectBase->dwID >= 8500) && (m_pObjectBase->dwID <= 8599) )
			{
				Tips_AddString( "天书阵法" );
			}*/
			     if ( (m_pObjectBase->dwID >= 8209) && (m_pObjectBase->dwID <= 8299) )
			{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,255) );	
				Tips_AddString( "修炼技能" );
			}
			else if ( (m_pObjectBase->dwID >= 8300) && (m_pObjectBase->dwID <= 8344) )
			{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,255) );	
				Tips_AddString( "家族技能" );
			}
			else if ( (m_pObjectBase->dwID >= 8355) && (m_pObjectBase->dwID <= 8399) )
			{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,255) );	
				Tips_AddString( "帮会技能" );
			}
			else if ( (m_pObjectBase->dwID >= 8400) && (m_pObjectBase->dwID <= 8499) )
			{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,255) );	
				Tips_AddString( "法宝技能" );
			}												
		}

	//Repair price
	if(m_dwTipFlags & ITEMTIPS_FLAG_REPAIR_PRICE)
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddString( "\n修理价格 " );
		m_ToolTips.SetCurColor( COLOR_ARGB(255,239,179,94) );
		Tips_AddText( "%s", Money2String(GetObjectRepairMoney(&m_object)) );		//GetPrice()
	}

	//Repair price by gold
/*	if(m_dwTipFlags & ITEMTIPS_FLAG_REPAIR_PRICEBYGOLD)
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddString( "\n修理价格 " );
		m_ToolTips.SetCurColor( COLOR_ARGB(255,239,179,94) );
		Tips_AddText("%s 金子",Money2String(GetObjectRepairByGold(&m_object)) );		//GetPrice()
	}*/

	//价格
	if(m_dwTipFlags & ITEMTIPS_FLAG_PRICE)
	{
		if ( m_object.pos.dwLocation == OBJECTCELLTYPE_SELL )
		{
			//Buy price
			//DWORD buyPrice = GetPrice( ITEM_PRICE_BUY );
			float buyPrice = m_object.price;
			//Make some adjust according to character goodness
			DWORD goodness = GetScene()->GetMainCharacter()->GetGoodnessAfterAdjust();
			switch(goodness)
			{
			case GOODNESS_3:
				buyPrice *= 1.2f;
				break;
			case GOODNESS_4:
				buyPrice *= 1.5f;
				break;
			case GOODNESS_5:
			case GOODNESS_6:
				buyPrice *= 2.0f;
				break;
			}
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n购买价格 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddText( "%s", Money2String( (DWORD)(buyPrice*m_object.dwNum + 0.99f) ) );
		}
		else if ( m_object.pos.dwLocation == OBJECTCELLTYPE_MYSHOP )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n销售价格 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddText( "%s", Money2String( GetItemTotalPrice(m_object) ) );
		}
		else if ( m_object.pos.dwLocation == OBJECTCELLTYPE_OTHERSSHOP)
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n购买价格 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddText( "%s", Money2String( GetItemTotalPrice(m_object) ) );
		}
		else
		{
			//Sell price
			//DWORD sellPrice = m_object.GetPrice( ITEM_PRICE_SELL );
			float sellPrice = GetObjectSellMoneyByDur(&m_object);
			////Consider dur
			//if ( m_object.maxdur > 0 )
			//{
			//	if ( (m_pObjectBase->dwType == ItemType_Arrow) )
			//		sellPrice *= m_object.dur/m_object.maxdur;
			//	else
			//		sellPrice *= ((m_object.dur/25)*25) / m_object.maxdur;
			//}
			//
			////Make some adjust according to character goodness
			//sellPrice *= 0.50f;		
			DWORD goodness = GetScene()->GetMainCharacter()->GetGoodnessAfterAdjust();
			switch(goodness)
			{
			case GOODNESS_3:
				sellPrice *= 0.7f;
				break;
			case GOODNESS_4:
				sellPrice *= 0.4f;
				break;
			case GOODNESS_5:
			case GOODNESS_6:
				sellPrice *= 0.1f;
				break;
			}
			//Tips_AddNum("\n价格 %u", (DWORD)(sellPrice + 0.99f));		//GetPrice()
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n出售价格 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddText( "%s", Money2String( (DWORD)(sellPrice*m_object.dwNum + 0.99f) ) );
		}

	}

	//sky 神器增加攻击
	if ( (m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118)||(m_pObjectBase->dwType >= 136 && m_pObjectBase->dwType <= 139) ||(m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147)||m_pObjectBase->dwType == 155 || m_pObjectBase->dwType == 156)
	{
		CRoleItem* pItem = NULL;

		if (m_object.m9axhp > 0)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,42,246,138));
			Tips_AddNum( "\n神器增加基础生命:%u%%", m_object.m9axhp);
		}
		else if (m_object.p9damage  > 0)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,42,246,138));
			Tips_AddNum( "\n神器增加基础攻击:%u%%", m_object.p9damage);
		}
		else if (m_object.m9damage  > 0)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,42,246,138));
			Tips_AddNum( "\n神器增加基础攻击:%u%%", m_object.m9damage);
		}
		else if (m_object.p9defence > 0)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,42,246,138));
			Tips_AddNum( "\n神器增加基础防御:%u%%", m_object.p9defence);
		}
		else if (m_object.m9defence > 0)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,42,246,138));
			Tips_AddNum( "\n神器增加基础防御:%u%%", m_object.m9defence);
		}
	}


	//Add equip position
	static const char* equipPos[] = { "", "头部", "身体", "右手", "左手"
		,"颈部", "腕部", "手指", "腰部", "足部" };		//1 ~ 9
	static const char* equipName[] = { "衣服", "衣服", "衣服", "刀", "剑","斧", "锤", "法杖", "弓", "扇子"
		,"棍", "盾牌", "头盔", "腰带", "护腕","鞋子", "项链", "戒指" };		//101 ~ 118

	static const char* equipName2[] = { "利刃","剑","护心镜" };
	static const char* equipName3[] = { "战旗","战旗","战旗","战旗","战旗","战旗","战旗" };
	static const char* equipName4[] = { "长枪","短枪" };

	if ( (m_pObjectBase->dwEquipType >= 1) && (m_pObjectBase->dwEquipType <= 9) )
	{
		if ( (m_pObjectBase->dwType >= 101) && (m_pObjectBase->dwType <= 118) )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n装备类型 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddString( equipName[m_pObjectBase->dwType-101] );
		}
		if ( (m_pObjectBase->dwType >= 136) && (m_pObjectBase->dwType <= 138) )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n装备类型 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddString( equipName2[m_pObjectBase->dwType-136] );
		}
		if ( (m_pObjectBase->dwType >= 141) && (m_pObjectBase->dwType <= 147) )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n装备类型 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddString( equipName3[m_pObjectBase->dwType-141] );
		}
		if ( (m_pObjectBase->dwType == 155 || m_pObjectBase->dwType == 156) )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n装备类型 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddString( equipName4[m_pObjectBase->dwType-155] );
		}

		if ( m_pObjectBase->dwType != 120 )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n装备位置 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
			Tips_AddString( equipPos[m_pObjectBase->dwEquipType] );
		}		
	}
	//end equip pos

	//Add special equip position
	if ( m_pObjectBase->dwType == 120 )
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddString( "\n装备类型 " );
		m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
		/*if ( m_pObjectBase->dwID == 875 )
		{
			Tips_AddString( "花" );
			}
			else if ( m_pObjectBase->dwID == 876 )
			{
			Tips_AddString( "采集手套" );
			}
			else*/
			Tips_AddString( "特殊" );

		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddString( "\n装备位置 " );
		m_ToolTips.SetCurColor( COLOR_ARGB(255,150,226,76) );
		Tips_AddString( "右手" );
	}

	//myy 附件装备类型 需要等级 显示
	if ( m_pObjectBase->dwType == ItemType_Chibang || m_pObjectBase->dwType == ItemType_Hanbing || m_pObjectBase->dwType == ItemType_Xunzhang)	
	{
		if( m_object.needlevel <= GetScene()->GetMainCharacter()->GetLevel() )
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		else
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,30));
		Tips_AddNum("\n需要等级 %u",m_object.needlevel);
	}

	//需要等级
	if ( m_object.dwObjectID == 680 )		//魂魄石
	{
		if( m_object.needlevel <= GetScene()->GetMainCharacter()->GetLevel() )
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		else
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,30));
		Tips_AddNum("\n需要等级 %u",m_object.needlevel);
	}
	else
	{
		if( m_object.needlevel <= GetScene()->GetMainCharacter()->GetLevel() )
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		else
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,30));
		if ( (m_object.needlevel>0) && !bCanUse )
			Tips_AddNum("\n需要等级 %u",m_object.needlevel);
	}

	//sky 增加装备转生需求
	if (m_object.round >= 9)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
	    Tips_AddString( "\n转生需求 九转");   
	}
	else if (m_object.round == 8)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
	    Tips_AddString( "\n转生需求 八转");   
	}
	else if (m_object.round == 7)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255, 255, 255, 0));
		Tips_AddString("\n转生需求 七转");
	}
	else if (m_object.round == 6)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255, 255, 255, 0));
		Tips_AddString("\n转生需求 六转");
	}
	else if (m_object.round == 5)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255, 255, 255, 0));
		Tips_AddString("\n转生需求 五转");
	}
	else if (m_object.round == 4)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255, 255, 255, 0));
		Tips_AddString("\n转生需求 四转");
	}
	else if (m_object.round == 3)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255, 255, 255, 0));
		Tips_AddString("\n转生需求 三转");
	}
	else if (m_object.round == 2)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255, 255, 255, 0));
		Tips_AddString("\n转生需求 二转");
	}
	else if (m_object.round == 1)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255, 255, 255, 0));
		Tips_AddString("\n转生需求 一转");
	}

	//Add equipment type that can use
	if ( m_pObjectBase->dwType == ItemType_SOULSTONE )
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,150,226,76));
		switch( m_object.dwObjectID )
		{
		case 680:
			Tips_AddString( "\n可镶嵌在任意装备上" );
			break;
		case 748:
			Tips_AddString( "\n可镶嵌在任意装备上" );
			break;
		case 749:
			Tips_AddString( "\n可镶嵌在任意装备上" );
			break;
		case 750:
			Tips_AddString( "\n可镶嵌在任意装备上" );
			break;
		case 751:
			Tips_AddString( "\n可镶嵌在任意装备上" );
			break;
		}
	}
	

		/*//使用次数
		if ( ((m_pObjectBase->dwType==ItemType_Change) )
		&& ((m_object.maxdur-m_object.dur) > 0) )
		{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		Tips_AddString( "\n使用次数 " );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddNum("%u",m_object.maxdur-m_object.dur);			//m_pObjectBase->dwMaxDur/25
		}*/

		//For mask
		if ( (m_pObjectBase->dwType==ItemType_Change)
			|| (m_pObjectBase->dwType == ItemType_Move)
			|| (m_pObjectBase->dwType == ItemType_Scroll)
			|| (m_pObjectBase->dwType == ItemType_Scrxll) //soke 新类型 15
			/*|| (m_pObjectBase->dwType == ItemType_MASK)*/ )
		{
			if ( (m_object.maxdur-m_object.dur) > 0 )
			{

			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
					Tips_AddString( "\n使用次数 " );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			Tips_AddNum("%u",m_object.maxdur-m_object.dur);			//m_pObjectBase->dwMaxDur/25
		}

		if ( m_object.dur > 0 )
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
					Tips_AddString( "\n剩余使用次数 " );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
					Tips_AddNum("%u",m_object.dur);			//m_pObjectBase->dwMaxDur/25
				}
			}
		//耐久度
		if ( (m_object.maxdur>0)
			&& !bCanUse
			&& !ISARRAW(m_pObjectBase->dwType)
			&& !(m_pObjectBase->dwType == ItemType_MASK)
			&& !bVessalType
			/*&& (m_object.pos.dwLocation == OBJECTCELLTYPE_EQUIP)*/		//We can see dur anywhere
			&& !(m_pObjectBase->dwType == 53)
			&& !(m_pObjectBase->dwType == 52)
			&& !(m_pObjectBase->dwType == 221)
			&& !(m_pObjectBase->dwType == 222)
			&& !(m_pObjectBase->dwType == 223)
			&& !(m_pObjectBase->dwType == 224)
			&& !(m_pObjectBase->dwType == 225)	
			&& !(m_pObjectBase->dwType == 226)
			&& !(m_pObjectBase->dwType == 227)
			&& !(m_pObjectBase->dwType == 228)
			&& !(m_pObjectBase->dwType == 229)
			&& !(m_pObjectBase->dwType == 230) //221-230为图鉴类型取消耐久度显示														
			&& !(m_pObjectBase->dwType == ItemType_Amulet) //护身符
		    && !(m_pObjectBase->dwType == ItemType_Amulets)//护身符
		    && !(m_pObjectBase->dwType == ItemType_Hanbing)
		    && !(m_pObjectBase->dwType == ItemType_Xunzhang)
			&& !(m_pObjectBase->dwType == ItemType_Chibang)
		    && !(m_pObjectBase->dwType == ItemType_Shenbing0)
		    && !(m_pObjectBase->dwType == ItemType_Shenbing1)
		    && !(m_pObjectBase->dwType == ItemType_Shenbing2)
		    && !(m_pObjectBase->dwType == ItemType_Shenshou1)
		    && !(m_pObjectBase->dwType == ItemType_Shenshou2)
			&& !(m_pObjectBase->dwType == ItemType_ShangfangBaojian)
		    && !(m_pObjectBase->dwType == ItemType_ZijinLongpao)
			&& !(m_pObjectBase->dwType == ItemType_Move)
			&& !(m_pObjectBase->dwType == ItemType_Scroll)
			&& !(m_pObjectBase->dwType == ItemType_Scrxll) //soke 新类型 15
			&& !(m_pObjectBase->dwType == ItemType_Pack)
			&& !(m_pObjectBase->dwType == ItemType_FashionBody)
			&& !(m_pObjectBase->dwType == ItemType_HighFashionBody)
			&& !(m_pObjectBase->dwType == ItemType_XHighFashionBody)
			&& !(m_pObjectBase->dwType == ItemType_SHighFashionBody)
			&& !(m_pObjectBase->dwType == ItemType_DoubleExpZ)
			&& !(m_pObjectBase->dwType == ItemType_DoubleExpC)
			&& !(m_pObjectBase->dwType == ItemType_DoubleExpG)
			&& !(m_pObjectBase->dwType == ItemType_LuncyDan)
			&& !(m_pObjectBase->dwType == ItemType_HzStone)
			&& !(m_pObjectBase->dwType == ItemType_FillHP)
			&& !(m_pObjectBase->dwType == ItemType_FillMP)
			&& !(m_pObjectBase->dwType == ItemType_FillXHP)
			&& !(m_pObjectBase->dwType == ItemType_HorseFashion)
			&& !(m_pObjectBase->dwType == ItemType_Fabao)
			&& !(m_pObjectBase->dwType == ItemType_Fabaofj1)
			&& !(m_pObjectBase->dwType == ItemType_Fabaofj2)
			&& !(m_pObjectBase->dwType == ItemType_Fabaofj3)
			&& !(m_pObjectBase->dwType == ItemType_Fabaofj4)
			&& !(m_pObjectBase->dwType == ItemType_Fabaofj5)
			&& !(m_pObjectBase->dwType == ItemType_Fabaofj6)
			&& !(m_pObjectBase->dwType == ItemType_Fabaofj7)
			&& !(m_pObjectBase->dwType == ItemType_Fabaofj8)
			&& !(m_pObjectBase->dwType == ItemType_Fabaofj9)
			&& !(m_pObjectBase->dwType == ItemType_Fabaofj10)
			&& !(m_pObjectBase->dwType == ItemType_Fabaofj11)
			&& !(m_pObjectBase->dwType == ItemType_Fabaofj12)
			&& !(m_pObjectBase->dwType == ItemType_YUANSHEN)
			&& !(m_pObjectBase->dwType == ItemType_YUANSHENFJ1)
			&& !(m_pObjectBase->dwType == ItemType_YUANSHENFJ2)
			&& !(m_pObjectBase->dwType == ItemType_YUANSHENFJ3)
			&& !(m_pObjectBase->dwType == ItemType_YUANSHENFJ4)
			&& !(m_pObjectBase->dwType == ItemType_YUANSHENFJ5)
			&& !(m_pObjectBase->dwType == ItemType_YUANSHENFJ6)
			&& !(m_pObjectBase->dwType == ItemType_BABYFJ1)
			&& !(m_pObjectBase->dwType == ItemType_BABYFJ2)
			&& !(m_pObjectBase->dwType == ItemType_BABYFJ3)
			&& !(m_pObjectBase->dwType == ItemType_BABYFJ4)
			&& !(m_pObjectBase->dwType == ItemType_BABYFJ5)
			&& !(m_pObjectBase->dwType == ItemType_BABYFJ6)
			&& !(m_pObjectBase->dwType == ItemType_SEXX1)
			&& !(m_pObjectBase->dwType == ItemType_SEXX2)
			&& !(m_pObjectBase->dwType == ItemType_SEXX3)
			&& !(m_pObjectBase->dwType == ItemType_SEXX4)	
			&& !(m_pObjectBase->dwType == ItemType_SEXX5)
			&& !(m_pObjectBase->dwType == ItemType_SEXX6)
			&& !(m_pObjectBase->dwType == ItemType_SEXX7)
			&& !(m_pObjectBase->dwType == ItemType_SEXX8)
			&& !(m_pObjectBase->dwType == ItemType_SEXX9)
			&& !(m_pObjectBase->dwType == ItemType_SEXX10)
			&& !(m_pObjectBase->dwType == ItemType_SEXX11)
			&& !(m_pObjectBase->dwType == ItemType_SEXX12)) //soke 马匹圣石
		{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n耐久度 " );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			Tips_AddNum2("%u/%u",(m_object.dur+49)/50,(m_object.maxdur+49)/50);			//m_pObjectBase->dwMaxDur/25
		}

	//For Mask
	if ( m_pObjectBase->dwType == ItemType_MASK )
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		Tips_AddString( "\n耐久度 " );

		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddNum2("%u/%u", m_object.dur, m_object.maxdur);			//m_pObjectBase->dwMaxDur/25
	}

		//For 荣誉之球
		if ( (m_pObjectBase->dwType == ItemType_Honor) &&(m_object.dur > 0) )
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n荣誉点 " );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			Tips_AddNum( "%u",m_object.dur );
		}

		//For Fill HP Item
		if ( (m_pObjectBase->dwType == ItemType_FillHP) && (m_object.dur > 0) )
		{
			//Attention: We multi the value by 100 cause of max dur is WORD but the real value is a DWORD
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n剩余血量 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,86,234) );
			Tips_AddNum( "%u", m_object.dur * 100 );

			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n最大血量 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,86,234) );
			Tips_AddNum( "%u", m_object.maxdur * 100 );
		}

		//For Fill MP Item
		if ( (m_pObjectBase->dwType == ItemType_FillMP) && (m_object.dur > 0) )
		{
			//Attention: We multi the value by 50 cause of max dur is WORD but the real value is a DWORD
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n剩余法力 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,86,234) );
			Tips_AddNum( "%u", m_object.dur * 50 );

			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n最大法力 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,86,234) );
			Tips_AddNum( "%u", m_object.maxdur * 50 );
		}

		//soke 自动补血 72类
		if ( (m_pObjectBase->dwType == ItemType_FillXHP) && (m_object.dur > 0) )
		{
			//Attention: We multi the value by 100 cause of max dur is WORD but the real value is a DWORD
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n剩余活力 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,86,234) );
			Tips_AddNum( "%u", m_object.dur * 100 );

			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n最大活力 " );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,86,234) );
			Tips_AddNum( "%u", m_object.maxdur * 100 );
		}

		//For 双倍经验道具
		if ( (m_pObjectBase->dwType == ItemType_DoubleExp) && (m_object.dur > 0) )
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n时间 " );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			Tips_AddNum( "%u 分",m_object.dur/60 );		//Server send time in seconds
		}

	//For 自动喝药道具
	if ( (m_pObjectBase->dwType == ItemType_Tonic) && (m_object.dur > 0) )
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		Tips_AddString( "\n耐久度 " );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		//Tips_AddNum( "%u", m_object.dur );
		Tips_AddNum2("%u/%u",m_object.dur,m_object.maxdur);
	}

		//soke 额外包裹、高级时装 显示天数
		if ( m_pObjectBase->dwType == ItemType_Pack || m_pObjectBase->dwType == ItemType_FashionBody )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n最大时间 " );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		//soke 让高级时装显示小时和分钟
		if ( m_pObjectBase->dwType == ItemType_FashionBody )
			{
				Tips_AddText( "%s", Minute2DayHourMinute( m_object.maxdur, TYPE_DAY_HOUR_MINUTE ) );	//3505
			}
			else
				Tips_AddText( "%s", Minute2DayHourMinute( m_object.maxdur, TYPE_DAY ) );

			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddString( "\n剩余时间 " );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		if ( m_pObjectBase->dwType == ItemType_FashionBody )
			{
				Tips_AddText( "%s", Minute2DayHourMinute( m_object.dur, TYPE_DAY_HOUR_MINUTE ) );	//2885
			}
			else
				Tips_AddText( "%s", Minute2DayHourMinute( m_object.dur, TYPE_DAY ) );
	}

	//soke  幸运丹 显示天数
	if ( m_pObjectBase->dwType == ItemType_DoubleExpZ || m_pObjectBase->dwType == ItemType_DoubleExpC || m_pObjectBase->dwType == ItemType_DoubleExpG || m_pObjectBase->dwType == ItemType_LuncyDan )
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddString( "\n时效 " );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		//soke 让高级时装、马匹圣石 显示小时和分钟
		if ( m_pObjectBase->dwType == ItemType_DoubleExpZ || m_pObjectBase->dwType == ItemType_DoubleExpC || m_pObjectBase->dwType == ItemType_DoubleExpG || m_pObjectBase->dwType == ItemType_LuncyDan)
		{
            Tips_AddText( "%s", Minute3DayHourMinute( m_object.maxdur, TYPE_DAY_HOUR_MINUTE ) );	//3505
		}
		else
			Tips_AddText( "%s", Minute3DayHourMinute( m_object.maxdur, TYPE_DAY ) );
	}

	//soke 低级时装显示耐久度
	if ( m_pObjectBase->dwType == ItemType_FashionBody)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		Tips_AddString( "\n耐久度 " );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddNum2("%u/%u",m_object.dur,m_object.maxdur);			//m_pObjectBase->dwMaxDur/25
	}

		//Just for 记忆蝴蝶  小护身符 大护身符
		//Show map name and position
		if ( m_object.dwObjectID == 666 || m_object.dwObjectID == 786 || m_object.dwObjectID == 787)
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddText( "\n%s", m_object.maker );
			Tips_AddNum2( " (%u, %u)", m_object.durpoint, m_object.dursecond );
		}
		//End

		//
		if ( (m_object.dwObjectID == c_nXuanYuanFu) || (m_object.dwObjectID == c_nXianTianFu) )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddText( "\n%s", m_object.maker );
			Tips_AddNum2( " (%u, %u)", m_object.durpoint, m_object.dursecond );
		}


	//Equipment property

    //soke 不是商店里在出售的 时装、马匹圣石 显示物理攻击百分比
	if ( m_pObjectBase->dwType == ItemType_FashionBody || m_pObjectBase->dwType == ItemType_HighFashionBody || m_pObjectBase->dwType == ItemType_XHighFashionBody || m_pObjectBase->dwType == ItemType_SHighFashionBody)
	{
		//攻击力（包括攻击力、远程伤害、仙术、法术）
		if ( (m_object.maxpdamage  || m_object.pdamage) && !bVessalType )
	    {
		    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		    Tips_AddString( "\n增加物理攻击力 " );
		    if ( m_object.maxpdamage <=12 )
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			else
				m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234)); //12以上颜色
		    Tips_AddNum("%u%%", m_object.maxpdamage);
	   }
	}
	else if ( m_pObjectBase->dwType == ItemType_FashionBody || m_pObjectBase->dwType == ItemType_HighFashionBody || m_pObjectBase->dwType == ItemType_XHighFashionBody )
	{
		//攻击力（包括攻击力、远程伤害、仙术、法术）
		if ( (m_object.maxpdamage  || m_object.pdamage) && !bVessalType && !(m_pObjectBase->dwType == ItemType_HorseFashion) && !(m_pObjectBase->dwType == ItemType_Amulets) && !( m_pObjectBase->dwType >= ItemType_handbookitem1 && m_pObjectBase->dwType <= ItemType_handbookitem10)) 
	    {
		    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		    Tips_AddString( "\n增加物理攻击力 " );
		    if ( m_object.maxpdamage <=12 )
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			else
				m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234)); //12以上颜色
		    Tips_AddNum("%u%%", m_object.maxpdamage);
	   }
	}
	else
	{
		//攻击力（包括攻击力、远程伤害、仙术、法术）
		if ( (m_object.maxpdamage  || m_object.pdamage) && !bVessalType && !(m_pObjectBase->dwType == ItemType_HorseFashion) && !(m_pObjectBase->dwType == ItemType_Amulets) && !( m_pObjectBase->dwType >= ItemType_handbookitem1 && m_pObjectBase->dwType <= ItemType_handbookitem10)&& !(m_pObjectBase->dwType >=ItemType_Fabao && m_pObjectBase->dwType<=ItemType_Fabaofj12)&& !(m_pObjectBase->dwType >=ItemType_SEXX1 && m_pObjectBase->dwType<=ItemType_SEXX12)&& !(m_pObjectBase->dwType >=ItemType_YUANSHEN && m_pObjectBase->dwType<=ItemType_YUANSHENFJ6)&& !(m_pObjectBase->dwType >=ItemType_BABYFJ1 && m_pObjectBase->dwType<=ItemType_BABYFJ6))
	    {
		   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		   Tips_AddString( "\n物理攻击力 " );
		   if ( ( (m_object.maxpdamage > m_pObjectBase->maxpdam) || (m_object.pdamage > m_pObjectBase->pdam) ) )
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		   Tips_AddNum2("%u - %u",m_object.pdamage, m_object.maxpdamage);
		   
		   //soke 升星物理攻击力加成紫色显示
		   if ( m_object.p2damage > 0 )
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		       Tips_AddNum(" + %u",m_object.p2damage);
		   }
		   //soke 升星物理攻击力加成绿色显示
		   else if ( m_object.p1damage > 0)
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		       Tips_AddNum(" + %u",m_object.p1damage);
		   }
		   if ( m_object.p4damage > 0) //龙x
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
			   Tips_AddNum(" + %u",(m_object.maxpdamage * (m_object.m4axpdamage+m_object.m5axpdamage)/100));
		   }
	    }
	}

    //soke  不是商店里在出售的 时装、马匹圣石 显示魔法攻击百分比
	if ( m_pObjectBase->dwType == ItemType_FashionBody || m_pObjectBase->dwType == ItemType_HighFashionBody || m_pObjectBase->dwType == ItemType_XHighFashionBody || m_pObjectBase->dwType == ItemType_SHighFashionBody)
	{
	   //Just a range
	   if ( ( m_object.maxmdamage || m_object.mdamage ) && !bVessalType && !(m_pObjectBase->dwType == ItemType_HorseFashion) && !(m_pObjectBase->dwType == ItemType_Amulets) && !( m_pObjectBase->dwType >= ItemType_handbookitem1 && m_pObjectBase->dwType <= ItemType_handbookitem10) )
	   {
		   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		   Tips_AddString( "\n增加魔法攻击力 " );
		   if ( m_object.maxmdamage <=12 )
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		   else
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234)); //12以上颜色
		   Tips_AddNum("%u%%",m_object.maxmdamage);
	   }
	}
	else
	{
	//Just a range
	  if ( ( m_object.maxmdamage || m_object.mdamage ) && !bVessalType && !(m_pObjectBase->dwType == ItemType_HorseFashion) && !(m_pObjectBase->dwType == ItemType_Amulets) && !( m_pObjectBase->dwType >= ItemType_handbookitem1 && m_pObjectBase->dwType <= ItemType_handbookitem10)&& !(m_pObjectBase->dwType >=ItemType_Fabao && m_pObjectBase->dwType<=ItemType_Fabaofj12)&& !(m_pObjectBase->dwType >=ItemType_SEXX1 && m_pObjectBase->dwType<=ItemType_SEXX12)&& !(m_pObjectBase->dwType >=ItemType_YUANSHEN && m_pObjectBase->dwType<=ItemType_YUANSHENFJ6) && !(m_pObjectBase->dwType >=ItemType_BABYFJ1 && m_pObjectBase->dwType<=ItemType_BABYFJ6))
	  {
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		Tips_AddString( "\n魔法攻击力 " );
		if ( (m_object.maxmdamage > m_pObjectBase->maxmdam) || (m_object.mdamage > m_pObjectBase->mdam) )
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddNum2("%u - %u",m_object.mdamage,m_object.maxmdamage);

		   //soke 升星魔法攻击力加成紫色显示
		   if ( m_object.m2damage > 0 )
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		       Tips_AddNum(" + %u",m_object.m2damage);
		   }
		   //soke 升星魔法攻击力加成绿色显示
		   else if ( m_object.m1damage > 0 )
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		       Tips_AddNum(" + %u",m_object.m1damage);
		   }
		   if ( m_object.m4axmdamage > 0) //龙x
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
			   Tips_AddNum(" + %u", (m_object.maxmdamage  * (m_object.m4axmdamage+m_object.m5axmdamage)/100));
		   }
	   }
	}

	//防御力（包括防御力、法术防御力）
	if (m_object.pdefence > 0 && !(m_pObjectBase->dwType == ItemType_HorseFashion) && !(m_pObjectBase->dwType == ItemType_Amulets) && !( m_pObjectBase->dwType >= ItemType_handbookitem1 && m_pObjectBase->dwType <= ItemType_handbookitem10)&& !(m_pObjectBase->dwType >=ItemType_Fabao && m_pObjectBase->dwType<=ItemType_Fabaofj12)&& !(m_pObjectBase->dwType >=ItemType_SEXX1 && m_pObjectBase->dwType<=ItemType_SEXX12) && !(m_pObjectBase->dwType >=ItemType_YUANSHEN && m_pObjectBase->dwType<=ItemType_YUANSHENFJ6) && !(m_pObjectBase->dwType >=ItemType_BABYFJ1 && m_pObjectBase->dwType<=ItemType_BABYFJ6))
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		Tips_AddString( "\n物理防御力 " );
		if ( m_object.pdefence > m_pObjectBase->pDef )
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddNum("%u",m_object.pdefence);

		///soke 升星物理防御力加成紫色显示
		if ( m_object.p2defence > 0 )
		{
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		     Tips_AddNum(" + %u",m_object.p2defence);
		}
		//soke 升星物理防御力加成绿色显示
		else if ( m_object.p1defence > 0 )
		{
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		     Tips_AddNum(" + %u",m_object.p1defence);
		}
		if ( m_object.p4defence > 0) //龙x
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
			Tips_AddNum(" + %u",(m_object.pdefence * (m_object.p4defence+m_object.p5defence)/100));
		}
	}


	if ( m_object.mdefence > 0 && !(m_pObjectBase->dwType == ItemType_HorseFashion) && !(m_pObjectBase->dwType == ItemType_Amulets) && !( m_pObjectBase->dwType >= ItemType_handbookitem1 && m_pObjectBase->dwType <= ItemType_handbookitem10)&& !(m_pObjectBase->dwType >=ItemType_Fabao && m_pObjectBase->dwType<=ItemType_Fabaofj12)&& !(m_pObjectBase->dwType >=ItemType_SEXX1 && m_pObjectBase->dwType<=ItemType_SEXX12)&& !(m_pObjectBase->dwType >=ItemType_YUANSHEN && m_pObjectBase->dwType<=ItemType_YUANSHENFJ6)  && !(m_pObjectBase->dwType >=ItemType_BABYFJ1 && m_pObjectBase->dwType<=ItemType_BABYFJ6))
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		Tips_AddString( "\n魔法防御力 " );
		if ( m_object.mdefence  > m_pObjectBase->mDef )
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddNum("%u",m_object.mdefence);

	//soke 升星魔法防御力加成紫色显示
		if ( m_object.m2defence > 0 )
		{
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		     Tips_AddNum(" + %u",m_object.m2defence);
		}
		//soke 升星魔法防御力加成绿色显示
		else if ( m_object.m1defence > 0 )
		{
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		     Tips_AddNum(" + %u",m_object.m1defence);
		}
		if ( m_object.m4defence > 0) //龙x
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
			Tips_AddNum(" + %u",(m_object.mdefence * (m_object.m4defence+m_object.m5defence)/100));
		}
	}

	m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
	//速度（物理攻击速度、施法速度、移动速度）
	Tips_AddNum("\n攻击速度 %u",m_object.akspeed);
	Tips_AddNum("\n移动速度 %u",m_object.mvspeed);

	m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
	//命中率（包括命中率、法术命中率）
	Tips_AddNum("\n命中率 %u%%",m_object.atrating);
	//闪避率（包括躲避率、法术躲避率）
	Tips_AddNum("\n躲避率 %u%%",m_object.akdodge);

	m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
	//恢复类（包括生命值恢复、法术值恢复、体力值恢复、装备耐久值恢复）
	if ( !( (spType == ST_LeechdomFood)	&& ( (m_object.maxhp == 8) || (m_object.maxhp == 9) ) ) )
		Tips_AddNum("\n生命值恢复 %u",m_object.hpr);
	if ( !( (spType == ST_LeechdomFood)	&& m_object.maxhp == 10) )
		Tips_AddNum("\n法术值恢复 %u",m_object.mpr);

		if ( (m_object.dwObjectID != 666)
			&& (m_object.dwObjectID != 786)//小护身符
			&& (m_object.dwObjectID != 787)//大护身符
			&& (m_pObjectBase->dwType != ItemType_Move)
			&& (m_pObjectBase->dwType != ItemType_Scroll)
			&& (m_pObjectBase->dwType != ItemType_Scrxll) //soke 新类型 15
			&& (m_pObjectBase->dwType != ItemType_Pack)
			&& (m_pObjectBase->dwType != ItemType_FashionBody)
			&& (m_pObjectBase->dwType != ItemType_DoubleExpZ)
			&& (m_pObjectBase->dwType != ItemType_DoubleExpC)
			&& (m_pObjectBase->dwType != ItemType_DoubleExpG)
			&& (m_pObjectBase->dwType != ItemType_LuncyDan)
			&& (m_pObjectBase->dwType != ItemType_HzStone)
			&& (m_pObjectBase->dwType != ItemType_Amulets)
			&& (m_pObjectBase->dwType != ItemType_Hanbing)
			&& (m_pObjectBase->dwType != ItemType_Xunzhang)
			&& (m_pObjectBase->dwType != ItemType_Chibang)
			&& (m_pObjectBase->dwType != ItemType_Shenbing0)
			&& (m_pObjectBase->dwType != ItemType_Shenbing1)
			&& (m_pObjectBase->dwType != ItemType_Shenbing2)
			&& (m_pObjectBase->dwType != ItemType_Shenshou1)
			&& (m_pObjectBase->dwType != ItemType_Shenshou2)
			&& (m_pObjectBase->dwType != ItemType_ShangfangBaojian)
		    && (m_pObjectBase->dwType != ItemType_ZijinLongpao)
			&& (m_pObjectBase->dwType != ItemType_HighFashionBody)
			&& (m_pObjectBase->dwType != ItemType_XHighFashionBody)
			&& (m_pObjectBase->dwType != ItemType_SHighFashionBody)
			&& (m_pObjectBase->dwType != ItemType_HorseFashion)
			&& (m_pObjectBase->dwType <ItemType_Fabao && m_pObjectBase->dwType>ItemType_Fabaofj12)
			&& (m_pObjectBase->dwType <ItemType_SEXX1 && m_pObjectBase->dwType>ItemType_SEXX12)
			&& (m_pObjectBase->dwType <ItemType_YUANSHEN && m_pObjectBase->dwType>ItemType_YUANSHENFJ6)
			&& (m_pObjectBase->dwType <ItemType_BABYFJ1 && m_pObjectBase->dwType>ItemType_BABYFJ6)) //马匹圣石
			
		{
			Tips_AddNum2("\n恢复装备耐久度 %u 于 %u 秒内",m_object.durpoint,m_object.dursecond);
		}

	m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));

     //soke 不是商店里在出售的 时装、马匹圣石 显示生命值百分比(护身符除外)
	if ( (m_pObjectBase->dwType == ItemType_FashionBody || m_pObjectBase->dwType == ItemType_HighFashionBody || m_pObjectBase->dwType == ItemType_XHighFashionBody|| m_pObjectBase->dwType == ItemType_SHighFashionBody) && m_pObjectBase->dwType != ItemType_Amulets )
	{
	   //增加角色属性（包括当前生命值、最大生命值、当前法术值、最大法术值、当前体力值、最大体力值）
	   if( !( (spType == ST_LeechdomFood) && ( (m_object.maxhp == 8) || (m_object.maxhp == 9) ) ) )
	   {
		   if ( m_object.maxhp > 0 && !(m_pObjectBase->dwType == ItemType_HorseFashion) && !(m_pObjectBase->dwType == ItemType_Amulets) && !( m_pObjectBase->dwType >= ItemType_handbookitem1 && m_pObjectBase->dwType <= ItemType_handbookitem10) )
		   {
			//增加额外固定生命值加成
	        m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
	        Tips_AddString( "\n增加生命值" );
	        m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
	        Tips_AddNum(" %u",m_object.maxsp);
           /////////////////////////////////////////////////////
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			   Tips_AddString( "\n增加生命值" );

			   if ( m_object.maxhp <=12 )
				   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			   else
				   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234)); //12以上颜色

			   Tips_AddNum(" %u%%", m_object.maxhp);				   
		   }		   
	   }
    }
	else
	{
	   //增加角色属性（包括当前生命值、最大生命值、当前法术值、最大法术值、当前体力值、最大体力值）(护身符除外)
	   if( !( (spType == ST_LeechdomFood) && ( (m_object.maxhp == 8) || (m_object.maxhp == 9) ) ) )
	   {
		   if ( m_object.maxhp > 0 && !(m_pObjectBase->dwType == ItemType_HorseFashion) && !(m_pObjectBase->dwType == ItemType_Amulets) && !( m_pObjectBase->dwType >= ItemType_handbookitem1 && m_pObjectBase->dwType <= ItemType_handbookitem10) && !(m_pObjectBase->dwType >=ItemType_Fabao && m_pObjectBase->dwType<=ItemType_Fabaofj12)&& !(m_pObjectBase->dwType >=ItemType_SEXX1 && m_pObjectBase->dwType<=ItemType_SEXX12) && !(m_pObjectBase->dwType >=ItemType_YUANSHEN && m_pObjectBase->dwType<=ItemType_YUANSHENFJ6)&& !(m_pObjectBase->dwType >=ItemType_BABYFJ1 && m_pObjectBase->dwType<=ItemType_BABYFJ6))
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			   Tips_AddString( "\n增加生命值" );

			   if ( m_object.maxhp > m_pObjectBase->maxhp )
				   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));

			   Tips_AddNum(" %u", m_object.maxhp);

			   //soke 升星生命值加成紫色显示
		      if ( m_object.m2axhp > 0 )
			   {
			       m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		           Tips_AddNum(" + %u",m_object.m2axhp);
			   }
			   //soke 升星生命值加成绿色显示
		       else if ( m_object.m1axhp > 0 )
			   {
			       m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		           Tips_AddNum(" + %u",m_object.m1axhp);
			   }
			   if ( m_object.m4axhp > 0) //龙x
			   {
				   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
				   Tips_AddNum(" + %u",(m_object.maxhp * (m_object.m4axhp+m_object.m5axhp)/100));
			   }
		   }
	   }
    }

    //soke 不是商店里在出售的 时装、马匹圣石 显示法术值百分比
	if ( m_pObjectBase->dwType == ItemType_FashionBody || m_pObjectBase->dwType == ItemType_HighFashionBody || m_pObjectBase->dwType == ItemType_SHighFashionBody)
	{
	   if( !( (spType == ST_LeechdomFood) && ( (m_object.maxhp == 10) || (m_object.maxhp == 11) ) ) )
	   {
		   if ( m_object.maxmp > 0 && !(m_pObjectBase->dwType == ItemType_HorseFashion) && !(m_pObjectBase->dwType == ItemType_Amulets) && !( m_pObjectBase->dwType >= ItemType_handbookitem1 && m_pObjectBase->dwType <= ItemType_handbookitem10)   )
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			   Tips_AddString( "\n增加法术值" );
			   if ( m_object.maxmp <=12 )
				   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			   else
				   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234)); //12以上颜色
			   Tips_AddNum(" %u%%",m_object.maxmp);
		   }
	   }
    }
	//soke 不是商店里在出售的 新时装 显示法术值百分比
	else if ( m_pObjectBase->dwType == ItemType_XHighFashionBody || m_pObjectBase->dwType == ItemType_SHighFashionBody)
	{
	   if( !( (spType == ST_LeechdomFood) && ( (m_object.maxhp == 10) || (m_object.maxhp == 11) ) ) )
	   {
		   if ( m_object.maxmp > 0  && !(m_pObjectBase->dwType == ItemType_HorseFashion) && !(m_pObjectBase->dwType == ItemType_Amulets) && !( m_pObjectBase->dwType >= ItemType_handbookitem1 && m_pObjectBase->dwType <= ItemType_handbookitem10)  )
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			   Tips_AddString( "\n增加法术值" );
			   if ( m_object.maxmp <=12 )
				   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			   else
				   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234)); //12以上颜色
			   Tips_AddNum(" %u%%",m_object.maxmp);

			// m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			// Tips_AddString( "\n增加生命值" );
			   
			// m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234)); //固定加血颜色
			// Tips_AddString( " 10000" )
		   }
	   }
    }
	else
	{
	   if( !( (spType == ST_LeechdomFood) && ( (m_object.maxhp == 10) || (m_object.maxhp == 11) ) ) )
	   {
		   if ( m_object.maxmp > 0  && !(m_pObjectBase->dwType == ItemType_HorseFashion) && !(m_pObjectBase->dwType == ItemType_Amulets) && !( m_pObjectBase->dwType >= ItemType_handbookitem1 && m_pObjectBase->dwType <= ItemType_handbookitem10)  )
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			   Tips_AddString( "\n增加法术值" );
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			   Tips_AddNum(" %u",m_object.maxmp);
		   }
	   }
    }

	//增加类型140 马匹圣石显示攻防和血量百分比
	if ( m_pObjectBase->dwType == ItemType_HorseFashion )
	{ 
	    if ( m_object.maxpdamage > 0 || m_object.maxmdamage> 0 || m_object.pdefence > 0 || m_object.mdefence > 0  || m_object.maxhp > 0 )
	    {   
	       
				if ( m_object.maxpdamage > 0)
				{ 
					m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
				    Tips_AddString( "\n增加物理攻击力" );
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,0,255));
		            Tips_AddNum(" +%u%%", m_object.maxpdamage);
				}		 
				if ( m_object.m10axpdamage > 0) //物攻百分比计算精炼加成显示
				{
					m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
				    Tips_AddNum("  ( +%u.00 )",(m_object.m10axpdamage)/100);
				}
			    // 分割线------------------------------------------------------
				if ( m_object.maxmdamage > 0)
				{ 
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
      			    Tips_AddString( "\n增加魔法攻击力" );
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,0,255));
		             Tips_AddNum(" +%u%%", m_object.maxmdamage);
				}
				if ( m_object.m10axmdamage > 0) //魔攻百分比计算精炼加成显示
				{
					m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
					Tips_AddNum("  ( +%u.00 )",(m_object.m10axmdamage)/100);
				}
				// 分割线------------------------------------------------------
				if ( m_object.pdefence > 0)
				{ 
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
				    Tips_AddString( "\n增加物理防御力" );
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,0,255));
		            Tips_AddNum(" +%u%%", m_object.pdefence);
				}		 				
				if ( m_object.p10defence > 0) //物防百分比计算精炼加成显示
				{
					m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
					Tips_AddNum("  ( +%u.00 )",(m_object.p10defence)/100);
				}
				// 分割线------------------------------------------------------
				if ( m_object.mdefence > 0)
				{
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
				    Tips_AddString( "\n增加魔法防御力" );
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,0,255));
		            Tips_AddNum(" +%u%%", m_object.mdefence);
				}  
				if ( m_object.m10defence > 0) //魔防百分比计算精炼加成显示
				{
					m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
					Tips_AddNum("  ( +%u.00 )",(m_object.m10defence )/100);
				}	
				// 分割线------------------------------------------------------
			/*	if ( m_object.pdam > 0)
				{
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
				    Tips_AddString( "\n增加物理攻击力" );
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,0,255));
		            Tips_AddNum(" +%u%%", m_object.pdam);
				}  
				// 分割线------------------------------------------------------
				if ( m_object.mdam > 0)
				{
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
				    Tips_AddString( "\n增加魔法攻击力" );
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,0,255));
		            Tips_AddNum(" +%u%%", m_object.mdam);
				}
				// 分割线------------------------------------------------------
				if ( m_object.pdef > 0)
				{
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
				    Tips_AddString( "\n增加物理防御力" );
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,0,255));
		            Tips_AddNum(" +%u%%", m_object.pdef);
				}
				// 分割线------------------------------------------------------
				if ( m_object.mdef > 0)
				{
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
				    Tips_AddString( "\n增加魔法防御力" );
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,0,255));
		            Tips_AddNum(" +%u%%", m_object.mdef);
				}	
				*/																			
				// 分割线------------------------------------------------------
				if ( m_object.maxhp > 0)
				{
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
				    Tips_AddString( "\n增加生命值上限" );
				    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,0,255));
		             Tips_AddNum(" +%u%%", m_object.maxhp); 
				if ( m_object.m10axhp > 0) //生命百分比计算精炼加成显示
				{
					m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
					Tips_AddNum("  ( +%u.00 )",(m_object.m10axhp)/100);			
				}		
				if ( m_object.maxhp > 0)			 
			        m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			        Tips_AddString( "\n马牌说明" );
			        m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			        Tips_AddString( "\n玄麟马牌可使用“玄麟精魄”在装备精炼内进行升级，等级越高各项属性加成更强！" );						 
				}		 
	    }
	}
	else if ( m_pObjectBase->dwID >= 50019 && m_pObjectBase->dwID <= 50028 ||m_pObjectBase->dwID >= 50119 && m_pObjectBase->dwID <= 50128)	 //soke 图鉴碎片
	{
		{
			//增加图鉴碎片显示合成属性
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n性别:" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,234));
			Tips_AddString(" 雄性");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddString("\n等级:");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,234));
			Tips_AddString(" 150");	
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddString("\n魅力指数:");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,234));
			Tips_AddString(" 100");	
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddString("\n\n合成属性:");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddString("\n物攻资质:");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			Tips_AddString(" 500");	
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddString("\n魔攻资质:");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			Tips_AddString(" 500");	
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddString("\n物防资质:");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			Tips_AddString(" 300");	
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddString("\n魔防资质:");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			Tips_AddString(" 300");	
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddString("\n血量资质:");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			Tips_AddString(" 2000");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));			
			Tips_AddString("\n\n温馨提示:");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			Tips_AddString("\n集齐10件图鉴可激活图鉴套装系统，图鉴可使用“怪物灵魂石”进行强化！");												
		}
		Tips_AddString( "\n" );
	}

     //增加类型221-230 图鉴显示基础加成数值
	if ( m_pObjectBase->dwType >= ItemType_handbookitem1 && m_pObjectBase->dwType <= ItemType_handbookitem10)
    { 
		if ( m_object.maxpdamage > 0 || m_object.maxmdamage> 0 || m_object.pdefence > 0 || m_object.mdefence > 0  || m_object.maxhp > 0 )
	    {   
	       
			if ( m_object.maxpdamage > 0)
			{ 
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
				Tips_AddString( "\n增加物攻" );
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,234));
		        Tips_AddNum(" +%u%", m_object.maxpdamage);
			}		 
			if ( m_object.m13axpdamage > 0) //物攻百分比计算加成显示
			{
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
				Tips_AddNum("  ( +%u.00 )",(m_object.m13axpdamage)/100);
			}
			// 分割线------------------------------------------------------
			if ( m_object.maxmdamage > 0)
			{ 
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
      			Tips_AddString( "\n增加魔攻" );
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,234));
		        Tips_AddNum(" +%u%", m_object.maxmdamage);
			}
			if ( m_object.m13axmdamage > 0) //魔攻百分比计算加成显示
			{
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
				Tips_AddNum("  ( +%u.00 )",(m_object.m13axmdamage)/100);
			}
				// 分割线------------------------------------------------------
			if ( m_object.pdefence > 0)
			{ 
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
				Tips_AddString( "\n增加物防" );
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,234));
		        Tips_AddNum(" +%u%", m_object.pdefence);
			}		 				
			if ( m_object.p13defence > 0) //物防百分比计算加成显示
			{
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
				Tips_AddNum("  ( +%u.00 )",(m_object.p13defence)/100);
			}
			// 分割线------------------------------------------------------
			if ( m_object.mdefence > 0)
			{
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
				Tips_AddString( "\n增加魔防" );
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,234));
		        Tips_AddNum(" +%u%", m_object.mdefence);
			}  
			if ( m_object.m13defence > 0) //魔防百分比计算加成显示
			{
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
				Tips_AddNum("  ( +%u.00 )",(m_object.m13defence )/100);
			}						
			// 分割线------------------------------------------------------
			if ( m_object.maxhp > 0)
			{
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
				Tips_AddString( "\n增加生命" );
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,234));
		        Tips_AddNum(" +%u%", m_object.maxhp); 
			}		 
			if ( m_object.m13axhp > 0) //生命百分比计算加成显示
			{
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
				Tips_AddNum(" ( +%u.00 )",(m_object.m13axhp)/100);
			}
        }
	}
			
	if ( m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 134 )
	{
	   if ( !( (spType == ST_LeechdomFood)	&& (m_object.maxhp == 1) ) )
	   {
		   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		   if (m_object.pdam > 0)
		   {
		      Tips_AddNum("\n增加物理攻击力 %u%",m_object.pdam);
		      Tips_AddString(" X 马匹资质的平方");
		   }
		   if (m_object.mdam > 0)
		   {
	          Tips_AddNum("\n增加魔法攻击力 %u%",m_object.mdam);
		      Tips_AddString(" X 马匹资质的平方");
		   }
	   }
    }
	else
	{
	   if ( !( (spType == ST_LeechdomFood)	&& (m_object.maxhp == 1) ) )
	   {
		   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		   Tips_AddNum("\n增加物理攻击力 %u%%",m_object.pdam);
	       Tips_AddNum("\n增加魔法攻击力 %u%%",m_object.mdam);
	   }
   }

	//增加防御力（包括防御力增强、法术防御力增强）
	if ( m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 134 )
	{
	   if( !( (spType == ST_LeechdomFood) && (m_object.maxhp == 2) ) )
	   {
		   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		   if (m_object.pdef > 0)
		   {
	          Tips_AddNum("\n增加物理防御力 %u%",m_object.pdef);
		      Tips_AddString(" X 马匹资质的平方");
		   }
		   if (m_object.mdef > 0)
		   {
	          Tips_AddNum("\n增加魔法防御力 %u%",m_object.mdef);
		      Tips_AddString(" X 马匹资质的平方");
		   }
	   }
    }
	else
	{
	   if( !( (spType == ST_LeechdomFood) && (m_object.maxhp == 2) ) )
	   {
		   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		   Tips_AddNum("\n物理防御力增强 %u%%",m_object.pdef);
	       Tips_AddNum("\n魔法防御力增强 %u%%",m_object.mdef);
	   }
    }

	//Add equipment special addation
	if (m_pObjectBase->dwType != ItemType_HighFashionBody || m_pObjectBase->dwType != ItemType_XHighFashionBody  ||m_pObjectBase->dwType != ItemType_SHighFashionBody || m_pObjectBase->dwType != ItemType_Xunzhang )
	{
		if( m_object.bang == 1)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			Tips_AddNum( "\n增加出现重击几率 %u%%", m_object.bang );
		}
		else if( m_object.bang > 1) //soke 重击大于1 紫色显示
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			Tips_AddString( "\n增加出现重击几率" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			Tips_AddNum(" %u%%", m_object.bang);
		}
	}
	else if (m_pObjectBase->dwType == ItemType_HighFashionBody|| m_pObjectBase->dwType == ItemType_XHighFashionBody|| m_pObjectBase->dwType == ItemType_SHighFashionBody )
	{
		 m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
	     Tips_AddString( "\n增加出现重击几率" );
	     m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
		 Tips_AddNum(" %u%% \n\n", m_object.bang);
	}

	if ( m_pObjectBase->dwType == ItemType_HighFashionBody || m_pObjectBase->dwType == ItemType_XHighFashionBody || m_pObjectBase->dwType == ItemType_SHighFashionBody  )
	{
		//soke 新时装支持自定义补丁物品的颜色
		Tips_AddString( "\n\n" );
        m_ToolTips.SetCurColor( COLOR_ARGB(255,214,0,239));
		sprintf( namePartOne, m_object.strName );
		Tips_AddStringDefFont( namePartOne );	//soke 物品名

		//soke 小于最大时间才显示生效的特效
		if ( m_object.dur <  m_object.maxdur )
		{
		   //soke 效果生效显示
		   stResourceLocation rl2;
		   rl2.SetFileName( "data\\interfaces1.gl");
		   rl2.group = 9;
		   rl2.frame = 8;
		   m_ToolTips.AddAnimation1( &rl2, false );
		   Tips_AddString( "\n" );
	    }
		else
		{
		   Tips_AddString( "\n" );
	    }
	    
		//soke 新时装显示的图片
		stResourceLocation rl;
		rl.SetFileName(c_szItemGrpPack);
		rl.group = c_nGroupItemCommon;
		rl.frame = m_pObjectBase->dwItemPic;
		m_ToolTips.AddAnimation( &rl, false );
	}

	//soke 新高级时装 显示天数
	if (m_pObjectBase->dwType == ItemType_HighFashionBody || m_pObjectBase->dwType == ItemType_XHighFashionBody || m_pObjectBase->dwType == ItemType_SHighFashionBody)
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
		Tips_AddString( "\n剩余时间 " );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		if (m_pObjectBase->dwType == ItemType_HighFashionBody || m_pObjectBase->dwType == ItemType_XHighFashionBody || m_pObjectBase->dwType == ItemType_SHighFashionBody )
		{
			Tips_AddText( "%s", Minute2DayHourMinute( m_object.dur, TYPE_DAY_HOUR_MINUTE ) );	//2885
		}
		else
			Tips_AddText( "%s", Minute2DayHourMinute( m_object.dur, TYPE_DAY ) );
	}

	    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddNum("\n减少进入中毒状态几率 %u%%",m_object.poisondef>100 ? 100 : m_object.poisondef);
		Tips_AddNum("\n减少进入冰冻状态几率 %u%%",m_object.colddef>100 ? 100 : m_object.colddef);
		Tips_AddNum("\n减少进入石化状态几率 %u%%",m_object.petrifydef>100 ? 100: m_object.petrifydef);
		Tips_AddNum("\n减少进入失明状态几率 %u%%",m_object.blinddef>100 ? 100 : m_object.blinddef);
		Tips_AddNum("\n减少进入混乱状态几率 %u%%",m_object.chaosdef>100 ? 100 : m_object.chaosdef);

		Tips_AddNum("\n减少进入麻痹状态几率 %u%%",m_object.lulldef>100 ? 100 : m_object.lulldef);
		Tips_AddNum("\n减少进入眩晕状态几率 %u%%",m_object.reeldef>100 ? 100 : m_object.reeldef);
		Tips_AddNum("\n减少进入噬魔状态几率 %u%%",m_object.evildef>100 ? 100 : m_object.evildef);
		Tips_AddNum("\n减少进入噬力状态几率 %u%%",m_object.bitedef>100 ? 100 : m_object.bitedef);
		Tips_AddNum("\n减少进入定身状态几率 %u%%",m_object.stabledef>100 ? 100 : m_object.stabledef);
		Tips_AddNum("\n减少进入减速状态几率 %u%%",m_object.slowdef>100 ? 100 : m_object.slowdef);
		Tips_AddNum("\n减少进入诱惑状态几率 %u%%",m_object.luredef>100 ? 100 : m_object.luredef);

		Tips_AddNum("\n增加 %u%% 几率使敌人进入中毒状态",m_object.poison);
		Tips_AddNum("\n增加 %u%% 几率使敌人进入冰冻状态",m_object.cold);
		Tips_AddNum("\n增加 %u%% 几率使敌人进入石化状态",m_object.petrify);
		Tips_AddNum("\n增加 %u%% 几率使敌人进入失明状态",m_object.blind);
		Tips_AddNum("\n增加 %u%% 几率使敌人进入混乱状态",m_object.chaos);

		Tips_AddNum("\n增加 %u%% 几率使敌人进入麻痹状态",m_object.lull);
		Tips_AddNum("\n增加 %u%% 几率使敌人进入眩晕状态",m_object.reel);
		Tips_AddNum("\n增加 %u%% 几率使敌人进入噬魔状态",m_object.evil);
		Tips_AddNum("\n增加 %u%% 几率使敌人进入噬力状态",m_object.bite);
		Tips_AddNum("\n增加 %u%% 几率使敌人进入定身状态",m_object.stable);
		Tips_AddNum("\n增加 %u%% 几率使敌人进入减速状态",m_object.slow);
		Tips_AddNum("\n增加 %u%% 几率使敌人进入诱惑状态",m_object.lure);

	//soke 护身符显示的
	if ( m_pObjectBase->dwType == ItemType_Amulets )
	{
	   //Add equipment detail property
	   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
	   Tips_AddNum("\n护身符增加物攻 +%u", m_object.maxpdamage);
	   Tips_AddNum("\n护身符增加魔攻 +%u", m_object.maxmdamage);
	   Tips_AddNum("\n护身符增加物防 +%u", m_object.pdefence);
	   Tips_AddNum("\n护身符增加魔防 +%u", m_object.mdefence);
	   //Add equipment detail property
	    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
	    Tips_AddNum("\n\n力量 + %u", m_object.str + m_object.xstr);
	    Tips_AddNum("\n\n智力 + %u", m_object.inte + m_object.xinte);
	    Tips_AddNum("\n\n敏捷 + %u", m_object.dex + m_object.xdex);
	    Tips_AddNum("\n\n精神 + %u", m_object.spi + m_object.xspi);
	    Tips_AddNum("\n\n体质 + %u", m_object.con + m_object.xcon);
		   m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
	   if(m_object.itemkscuican >0 )  
	   {
	      Tips_AddNum1( "\n灵性 + %u", (m_object.itemkscuican)/10 );
	   }	
        if(m_object.itemkscuican >0 )
	    {
		    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,113,0));
		    Tips_AddString( "\n璀璨属性：" );
		    if ( m_object.m11axpdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			    Tips_AddNum1( "璀璨增加物攻 +%u", m_object.m11axpdamage );
		    }
            if ( m_object.m11axmdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			    Tips_AddNum1( "璀璨增加魔攻 +%u", m_object.m11axmdamage );			
		    }
		    if ( m_object.p11defence > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			    Tips_AddNum1( "璀璨增加物防 +%u", m_object.p11defence );			
		    }
	        if ( m_object.m11defence > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			    Tips_AddNum1( "璀璨增加魔防 +%u", m_object.m11defence );
		    }
		    if ( m_object.m11axhp > 0 )
		    {
			    Tips_AddString( "\n" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			Tips_AddNum1( "璀璨增加生命 +%u", m_object.m11axhp );			
            }
	    }	
	   //soke 护身符奖励属性
	   if( !( (spType == ST_LeechdomFood) && ( (m_object.maxhp == 8) || (m_object.maxhp == 9) ) ) )
	   {
		   if ( m_object.maxhp > 0 )
		   {
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			   Tips_AddString( "\n奖励属性" );
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			   Tips_AddString( "\n1% 忽视目标防御" );
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			   Tips_AddString( "\n5% 增加伤害" );
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			   Tips_AddString( "\n最大生命值增加" );

			   if ( m_object.maxhp > m_pObjectBase->maxhp )
				   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));

			   Tips_AddNum(" %u", m_object.maxhp);
			   //soke 护身符附加功能
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			   Tips_AddString( "\n附加功能" );	
			   m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			   Tips_AddString( "\n自动喝血 开启" );		
			   Tips_AddString( "\n自动喝蓝 开启" );		
		   }
	   }
	    if (m_object.itemkscuican > 0 )
	    {
			Tips_AddString( "\n" );
		    stResourceLocation rt;
		    rt.SetFileName( "data\\interfaces.gl");
	 	    rt.group = 4;
		    rt.frame = 598;
		    m_ToolTips.AddAnimation( &rt, false );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			char szTmp[512];
			WORD cuican_num = 1000;
			{
			   sprintf(szTmp, " 璀璨石剩余镶嵌次数 %u 次", cuican_num-(m_object.itemkscuican)) ;		      
			}
			Tips_AddString( szTmp );
            
	    }
		if (m_object.itemksAmulet > 0 )
	    {
			Tips_AddString( "\n" );
		    stResourceLocation rt;
		    rt.SetFileName( "data\\interfaces.gl");
	 	    rt.group = 4;
		    rt.frame = 596;
		    m_ToolTips.AddAnimation( &rt, false );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			char szTmp[512];
			WORD ssbox_num = 1000;
			{
			   sprintf(szTmp, " 神兽宝石剩余镶嵌次数 %u 次", ssbox_num-(m_object.itemksAmulet)) ;		      
			}
			Tips_AddString( szTmp );           
		    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,113,0));
		    Tips_AddString( "\n神兽宝石属性：" );
	        if ( m_object.m12axpdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    stResourceLocation rt;
		        rt.SetFileName( "data\\interfaces.gl");
	 	        rt.group = 4;
		        rt.frame = 600;
		        m_ToolTips.AddAnimation( &rt, false );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			    Tips_AddNum1( " 白虎石增加物攻 +%u", m_object.m12axpdamage );
		    }
            if ( m_object.m12axmdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    stResourceLocation rt;
		        rt.SetFileName( "data\\interfaces.gl");
	 	        rt.group = 4;
		        rt.frame = 601;
		        m_ToolTips.AddAnimation( &rt, false );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			    Tips_AddNum1( " 青龙石增加魔攻 +%u", m_object.m12axmdamage );
		    }
		    if ( m_object.p12defence > 0 )
		    {
			    Tips_AddString( "\n" );
			    stResourceLocation rt;
		        rt.SetFileName( "data\\interfaces.gl");
	 	        rt.group = 4;
		        rt.frame = 602;
		        m_ToolTips.AddAnimation( &rt, false );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			    Tips_AddNum1( " 玄武石增加物防 +%u", m_object.p12defence );
		    }
	        if ( m_object.m12defence > 0 )
		    {
			    Tips_AddString( "\n" );
			    stResourceLocation rt;
		        rt.SetFileName( "data\\interfaces.gl");
	 	        rt.group = 4;
		        rt.frame = 603;
		        m_ToolTips.AddAnimation( &rt, false );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			    Tips_AddNum1( " 朱雀石增加魔防 +%u", m_object.m12defence );
		    }
		    if ( m_object.m12axhp > 0 )
		    {
			    Tips_AddString( "\n" );
			    stResourceLocation rt;
		        rt.SetFileName( "data\\interfaces.gl");
	 	        rt.group = 4;
		        rt.frame = 604;
		        m_ToolTips.AddAnimation( &rt, false );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			    Tips_AddNum1( " 麒麟石增加生命 +%u", m_object.m12axhp );

            }
	    }

		if (m_object.retain33 > 0 )
	    {
			Tips_AddString( "\n" );
		    stResourceLocation rt;
		    rt.SetFileName( "data\\interfaces.gl");
	 	    rt.group = 4;
		    rt.frame = 598;
		    m_ToolTips.AddAnimation( &rt, false );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			char szTmp[512];
			WORD cuican_num = 1000;
			{
			   sprintf(szTmp, " 黑曜石剩余镶嵌次数 %u 次", cuican_num-(m_object.retain33)) ;		      
			}
			Tips_AddString( szTmp );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		    Tips_AddString( "\n黑曜石一等：增加宝石属性100%" );

		}
		if (m_object.retain32 > 0 )
	    {

			Tips_AddString( "\n" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,113,0));
			Tips_AddString( "\n萱花神斧：" );
			Tips_AddString( "十阶" );
			Tips_AddString( "\n萱花神斧：" );
			Tips_AddString( "100/100" );	
			Tips_AddString( "\n" );
			
		    stResourceLocation rt;
		    rt.SetFileName( "data\\interfaces.gl");
	 	    rt.group = 4;
		    rt.frame = 598;
		    m_ToolTips.AddAnimation( &rt, false );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			char szTmp[512];
			WORD cuican_num = 1000;
			{
			   sprintf(szTmp, " 神斧石剩余镶嵌次数 %u 次", cuican_num-(m_object.retain32)) ;		      
			}
			Tips_AddString( szTmp );
            m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
		    Tips_AddString( "\n神斧石强化属性：" );
		    if ( m_object.m32axpdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,33,226,217));
			    Tips_AddNum1( "神斧石增加物攻 +%u", m_object.m32axpdamage );
		    }
            if ( m_object.m32axmdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,33,226,217));
			    Tips_AddNum1( "神斧石增加魔攻 +%u", m_object.m32axmdamage );			
		    }
		    if ( m_object.p32defence > 0 )
		    {
			    Tips_AddString( "\n" );
			     m_ToolTips.SetCurColor(COLOR_ARGB(255,33,226,217));
			    Tips_AddNum1( "神斧石增加物防 +%u", m_object.p32defence );			
		    }
	        if ( m_object.m32defence > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,33,226,217));
			    Tips_AddNum1( "神斧石增加魔防 +%u", m_object.m32defence );
		    }
		    if ( m_object.m32axhp > 0 )
		    {
			    Tips_AddString( "\n" );			
			 	m_ToolTips.SetCurColor(COLOR_ARGB(255,33,226,217));
			Tips_AddNum1( "神斧石增加生命 +%u", m_object.m32axhp );			
            }
	    }

		//力劈华山
		Tips_AddString( "\n" );
	 	stResourceLocation rt;
		rt.SetFileName( "data\\interfaces3.gl");
	 	rt.group = 49;
		rt.frame = 9;
		m_ToolTips.AddAnimation( &rt, false );	
		rt.SetFileName( "data\\interfaces3.gl");
	 	rt.group = 49;
		rt.frame = 19;
		m_ToolTips.AddAnimation( &rt, false );	
		rt.SetFileName( "data\\interfaces3.gl");
	 	rt.group = 49;
		rt.frame = 29;
		m_ToolTips.AddAnimation( &rt, false );	
		rt.SetFileName( "data\\interfaces3.gl");
	 	rt.group = 49;
		rt.frame = 39;
		m_ToolTips.AddAnimation( &rt, false );	

	
	    if (m_object.retain34 > 0 )
	    {
			Tips_AddString( "\n" );
		    stResourceLocation rt;
		    rt.SetFileName( "data\\interfaces.gl");
	 	    rt.group = 4;
		    rt.frame = 598;
		    m_ToolTips.AddAnimation( &rt, false );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,40,246,33));
			char szTmp[512];
			WORD cuican_num = 1000;
			{
			   sprintf(szTmp, " 精金石剩余镶嵌次数 %u 次", cuican_num-(m_object.retain34)) ;		      
			}
			Tips_AddString( szTmp );
            m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
		    Tips_AddString( "\n精金石强化属性：" );
		    if ( m_object.m34axpdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,238,232,34));
			    Tips_AddNum1( "精金石增加物攻 +%u", m_object.m34axpdamage );
		    }
            if ( m_object.m34axmdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,238,232,34));
			    Tips_AddNum1( "精金石增加魔攻 +%u", m_object.m34axmdamage );			
		    }
		    if ( m_object.p34defence > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,238,232,34));
			    Tips_AddNum1( "精金石增加物防 +%u", m_object.p34defence );			
		    }
	        if ( m_object.m34defence > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,238,232,34));
			    Tips_AddNum1( "精金石增加魔防 +%u", m_object.m34defence );
		    }
		    if ( m_object.m34axhp > 0 )
		    {
			    Tips_AddString( "\n" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,238,232,34));
			Tips_AddNum1( "精金石增加生命 +%u", m_object.m34axhp );			
            }
	    }
    }
	else
	{
	   //Add equipment detail property
	    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
	   Tips_AddNum("\n力量 +%u", m_object.str + m_object.xstr);
	   Tips_AddNum("\n智力 +%u", m_object.inte +  m_object.xinte);
	   Tips_AddNum("\n敏捷 +%u", m_object.dex + m_object.xdex);
	   Tips_AddNum("\n精神 +%u", m_object.spi + m_object.xspi);
	   Tips_AddNum("\n体质 +%u", m_object.con + m_object.xcon);
    }

	//soke 尚方宝剑
	if ( m_pObjectBase->dwType == ItemType_ShangfangBaojian )
	{
		//Add equipment detail property
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum("\n\n额外增加物理攻击力 %u",m_object.p18damage);
		Tips_AddNum("\n额外增加魔法攻击力 %u",m_object.m18damage);
		Tips_AddNum("\n额外增加物理防御力 %u",m_object.p18defence);
		Tips_AddNum("\n额外增加魔法防御力 %u",m_object.m18defence);
		Tips_AddNum("\n额外增加生命值上限 %u",m_object.m18axhp);
	}
	//soke 紫金龙袍
	if ( m_pObjectBase->dwType == ItemType_ZijinLongpao )
	{
		//Add equipment detail property
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum("\n\n额外增加物理攻击力 %u",m_object.p19damage);
		Tips_AddNum("\n额外增加魔法攻击力 %u",m_object.m19damage);
		Tips_AddNum("\n额外增加物理防御力 %u",m_object.p19defence);
		Tips_AddNum("\n额外增加魔法防御力 %u",m_object.m19defence);
		Tips_AddNum("\n额外增加生命值上限 %u",m_object.m19axhp);
	}

	//soke 段位勋章
	if ( m_pObjectBase->dwType == ItemType_Xunzhang )
	{
		//Add equipment detail property
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum1("\n\n额外增加物理攻击力 %d",m_object.p7damage);
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
        Tips_AddNum1("(+%d)",(m_object.p7damage * (m_object.m51defence)/100));
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));	
		Tips_AddNum1("\n额外增加魔法攻击力 %d",m_object.m7damage);
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
        Tips_AddNum1("(+%d)",(m_object.m7damage * (m_object.m51damage)/100));
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));		
		Tips_AddNum1("\n额外增加物理防御力 %d",m_object.p7defence);
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
		Tips_AddNum1("(+%d)",(m_object.p7defence * (m_object.p51defence)/100));
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum1("\n额外增加魔法防御力 %d",m_object.m7defence);
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
		Tips_AddNum1("(+%d)",(m_object.m7defence * (m_object.m51defence)/100));
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum1("\n额外增加生命值上限 %d",m_object.m7axhp);
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
		Tips_AddNum1("(+%d)",(m_object.m7axhp * (m_object.m51axhp)/100));
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum1("\n额外增加重击 %u%%",m_object.bang);
		if ( m_object.dwObjectID == 3831)
		{		
		if (m_object.dmedalt >= 1 && m_object.dmedalt <= 100) 
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n\n当前勋章品质：" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,126,40));
			Tips_AddString( "紧箍咒" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddNum("\n升至下一品质所需等级：%d ", 100-m_object.dmedalt);
			Tips_AddString( "\n" );
		}
		if (m_object.dmedalt >= 101 && m_object.dmedalt <= 200) 
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n\n当前勋章品质：" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,126,40));
			Tips_AddString( "紧箍咒" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddNum("\n升至下一品质所需等级：%d ", 200-m_object.dmedalt);
			Tips_AddString( "\n" );
		}
		if (m_object.dmedalt >= 201 && m_object.dmedalt <= 300) 
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n\n当前勋章品质：" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,126,40));
			Tips_AddString( "紧箍咒" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddNum("\n升至下一品质所需等级：%d ", 300-m_object.dmedalt);
			Tips_AddString( "\n" );
		}
		if (m_object.dmedalt >= 301 && m_object.dmedalt <= 400) 
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n\n当前勋章品质：" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,126,40));
			Tips_AddString( "紧箍咒" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddNum("\n升至下一品质所需等级：%d ", 400-m_object.dmedalt);
			Tips_AddString( "\n" );
		}
		if (m_object.dmedalt >= 401 && m_object.dmedalt <= 500) 
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n\n当前勋章品质：" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,126,40));
			Tips_AddString( "紧箍咒" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddNum("\n升至下一品质所需等级：%d ", 500-m_object.dmedalt);
			Tips_AddString( "\n" );
		}
		if (m_object.dmedalt >= 501 && m_object.dmedalt <= 600) 
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n\n当前勋章品质：" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,126,40));
			Tips_AddString( "紧箍咒" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddNum("\n升至下一品质所需等级：%d ", 600-m_object.dmedalt);
			Tips_AddString( "\n" );
		}
		if (m_object.dmedalt >= 601 && m_object.dmedalt <= 700) 
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n\n当前勋章品质：" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,126,40));
			Tips_AddString( "紧箍咒" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddNum("\n升至下一品质所需等级：%d ", 700-m_object.dmedalt);
			Tips_AddString( "\n" );
		}
		if (m_object.dmedalt >= 701 && m_object.dmedalt <= 800) 
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n\n当前勋章品质：" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,126,40));
			Tips_AddString( "紧箍咒" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddNum("\n升至下一品质所需等级：%d ", 800-m_object.dmedalt);
			Tips_AddString( "\n" );
		}
		if (m_object.dmedalt >= 801 && m_object.dmedalt <= 900) 
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n\n当前勋章品质：" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,126,40));
			Tips_AddString( "紧箍咒" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddNum("\n升至下一品质所需等级：%d ", 900-m_object.dmedalt);
			Tips_AddString( "\n" );
		}
		if (m_object.dmedalt >= 901 && m_object.dmedalt <= 1000) 
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n\n当前勋章品质：" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,126,40));
			Tips_AddString( "紧箍咒" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));			
			Tips_AddNum("\n升至下一品质所需等级：%d ", 1000-m_object.dmedalt);
			Tips_AddString( "\n" );	
		}
	        ///////////////////勋章强化石开始////////////////////
			if (m_object.syxqgrade21 > 0 )
	        {
			stResourceLocation imguserinfo1;
			imguserinfo1.SetFileName( "data\\interfaces7.gl");
			imguserinfo1.group = 400;
		    imguserinfo1.frame = 2;
		    m_ToolTips.AddAnimation( &imguserinfo1, false );
			m_ToolTips.Resize();
			m_ToolTips.SetCurColor(COLOR_ARGB(255,100,255,0));
			char szTmp[512];
			WORD shenfu_num = 1000;
			{
			   sprintf(szTmp,"剩余强化次数 %u 次", shenfu_num-(m_object.syxqgrade21)) ;		      
			}
			Tips_AddString( szTmp );          
	        }	
            if(m_object.syxqgrade21 >0 )
	        {
		    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,126,40));
		    Tips_AddString( "\n强化属性：" );
		    if ( m_object.m49axpdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,237,0,234));
			    Tips_AddNum1( "增加物攻 %d", m_object.m49axpdamage );
		    }
		    if ( m_object.m51axpdamage >= 0) //黑曜石增加物攻百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m49axpdamage * (m_object.m51axpdamage)/100));
		    }			
            if ( m_object.m49axmdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,237,0,234));
			    Tips_AddNum1( "增加魔攻 %d", m_object.m49axmdamage );			
		    }
		    if ( m_object.m51axmdamage >= 0) //黑曜石增加魔攻百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m49axmdamage * (m_object.m51axmdamage)/100));
		    }			
		    if ( m_object.p49defence > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,237,0,234));
			    Tips_AddNum1( "增加物防 %d", m_object.p49defence );			
		    }
		    if ( m_object.p51defence >= 0) //黑曜石增加物防百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.p49defence * (m_object.p51defence)/100));
		    }			
	        if ( m_object.m49defence > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,237,0,234));
			    Tips_AddNum1( "增加魔防 %d", m_object.m49defence );
		    }
		    if ( m_object.m51defence >= 0) //黑曜石增加魔防百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m49defence * (m_object.m51defence)/100));
		    }			
		    if ( m_object.m49axhp > 0 )
		    {
			    Tips_AddString( "\n" );			
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,237,0,234));
			    Tips_AddNum1( "增加生命 %d", m_object.m49axhp );			
            }	
		    if ( m_object.m51axhp >= 0) //黑曜石增加生命百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m49axhp * (m_object.m51axhp)/100));
		    }							
	        }
			///////////////////勋章强化石强化结束////////////////////	
	        ///////////////////勋章字纹石开始////////////////////
			if (m_object.syxqgrade22 > 0 )
	        {		
			Tips_AddString( "\n" );	
			stResourceLocation imguserinfo1;
			imguserinfo1.SetFileName( "data\\interfaces7.gl");
			imguserinfo1.group = 400;
		    imguserinfo1.frame = 2;
		    m_ToolTips.AddAnimation( &imguserinfo1, false );
			m_ToolTips.Resize();
            Tips_AddString( "\n" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,100,255,0));
			char szTmp[512];
			WORD shenfu_num = 1000;
			{
			   sprintf(szTmp,"剩余字纹次数 %u 次", shenfu_num-(m_object.syxqgrade22)) ;		      
			}
			Tips_AddString( szTmp );          
	        }	
            if(m_object.syxqgrade22 >0 )
	        {
		    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,126,40));
		    Tips_AddString( "\n字纹属性：" );
		    if ( m_object.m50axpdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,198));
			    Tips_AddNum1( "增加物攻 %d", m_object.m50axpdamage );
		    }
		    if ( m_object.m51axpdamage >= 0) //黑曜石增加物攻百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m50axpdamage * (m_object.m51axpdamage)/100));
		    }			
            if ( m_object.m50axmdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,198));
			    Tips_AddNum1( "增加魔攻 %d", m_object.m50axmdamage );			
		    }
		    if ( m_object.m51axmdamage >= 0) //黑曜石增加魔攻百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m50axmdamage * (m_object.m51axmdamage)/100));
		    }			
		    if ( m_object.p50defence > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,198));
			    Tips_AddNum1( "增加物防 %d", m_object.p50defence );			
		    }
		    if ( m_object.p51defence >= 0) //黑曜石增加物防百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.p50defence * (m_object.p51defence)/100));
		    }			
	        if ( m_object.m50defence > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,198));
			    Tips_AddNum1( "增加魔防 %d", m_object.m50defence );
		    }
		    if ( m_object.m51defence >= 0) //黑曜石增加魔防百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m50defence * (m_object.m51defence)/100));
		    }			
		    if ( m_object.m50axhp > 0 )
		    {
			    Tips_AddString( "\n" );			
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,198));
			    Tips_AddNum1( "增加生命 %d", m_object.m50axhp );			
            }	
		    if ( m_object.m51axhp >= 0) //黑曜石增加生命百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m50axhp * (m_object.m51axhp)/100));
		    }							
	        }
			///////////////////勋章字纹石字纹结束////////////////////														
		// if (m_object.syxqgrade21 >= 0 && m_object.syxqgrade21 <= 799 )
		// {	
		// 	Tips_AddString( "\n" );
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,128,128,128) );
		// 	Tips_AddString( "\n强化技能：未激活" );
		// 	m_ToolTips.AddText( "\n" );			
	    //     stResourceLocation rl;
	    //     rl.SetFileName( "data\\icons.gl" );
	    //     rl.group = 1;
	    //     rl.frame = 3685;	//未激活
		// 	m_ToolTips.AddAnimation( &rl, false );
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
		// 	Tips_AddString( "\n【定心真言】" );	
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,128,128,128) );
		// 	Tips_AddString3( "\n状态技能，主动施放，使九屏范围内无法使用任何技能，药水，道具。" );				
		// }	
		// if (m_object.syxqgrade21 >= 800 && m_object.syxqgrade21 <= 10000 )
		// {	
		// 	Tips_AddString( "\n" );
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,214,0,239) );
		// 	Tips_AddString( "\n强化技能：已激活" );
		// 	m_ToolTips.AddText( "\n" );			
	    //     stResourceLocation rl;
	    //     rl.SetFileName( "data\\icons.gl" );
	    //     rl.group = 1;
	    //     rl.frame = 3683;	//激活
		// 	m_ToolTips.AddAnimation( &rl, false );
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
		// 	Tips_AddString( "\n【定心真言】" );	
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,214,0,239) );
		// 	Tips_AddString3( "\n状态技能，主动施放，使九屏范围内无法使用任何技能，药水，道具。" );				
		// }			
		// 	//-----------------------------------------------------------------
		// if (m_object.syxqgrade22 >= 0 && m_object.syxqgrade22 <= 799 )
		// {			
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,128,128,128) );
        //     m_ToolTips.AddText( "\n" );
		// 	Tips_AddString( "\n字纹技能：未激活" );
		// 	m_ToolTips.AddText( "\n" );
		// 	//////////////////////////
		// 	stResourceLocation rl;
	    //     rl.SetFileName( "data\\icons.gl" );
	    //     rl.group = 1;
	    //     rl.frame = 1340;	//未激活
		// 	m_ToolTips.AddAnimation( &rl, false );	
		// 	//////////////////////////
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
		// 	Tips_AddString( "\n【碎震直拳】" );	
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,128,128,128) );
		// 	Tips_AddString3( "\n状态技能，主动施放，使目标敌人进入眩晕状态。" );
		// }
		// if (m_object.syxqgrade22 >= 800 && m_object.syxqgrade22 <= 10000 )
		// {	
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,214,0,239) );
        //     m_ToolTips.AddText( "\n" );
		// 	Tips_AddString( "\n字纹技能：已激活" );
		// 	m_ToolTips.AddText( "\n" );
		// 	//////////////////////////
		// 	stResourceLocation rl;
	    //     rl.SetFileName( "data\\icons.gl" );
	    //     rl.group = 1;
	    //     rl.frame = 1338;	//未激活
		// 	m_ToolTips.AddAnimation( &rl, false );	
		// 	//////////////////////////
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
		// 	Tips_AddString( "\n【碎震直拳】" );	
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,214,0,239) );
		// 	Tips_AddString3( "\n状态技能，主动施放，使目标敌人进入眩晕状态。" );					
		// }						
		// //-----------------------------------------------------------------
		// if (m_object.syxqgrade23 >= 0 && m_object.syxqgrade23 <= 79 )
		// {			
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,128,128,128) );
        //     m_ToolTips.AddText( "\n" );
		// 	Tips_AddString( "\n淬炼技能：未激活" );
		// 	m_ToolTips.AddText( "\n" );
		// 	stResourceLocation rl;
	    //     rl.SetFileName( "data\\icons.gl" );
	    //     rl.group = 1;
	    //     rl.frame = 1208;	//激活
		// 	m_ToolTips.AddAnimation( &rl, false );	
		// 	//////////////////////////
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
		// 	Tips_AddString( "\n【裂地震波】" );	
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,128,128,128) );
		// 	Tips_AddString3( "\n状态技能，主动施放，使九屏范围内敌人进入眩晕状态。" );
		// }
		// if (m_object.syxqgrade23 >= 80 && m_object.syxqgrade23 <= 10000 )
		// {	
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,214,0,239) );
        //     m_ToolTips.AddText( "\n" );
		// 	Tips_AddString( "\n淬炼技能：已激活" );
		// 	m_ToolTips.AddText( "\n" );
		// 	stResourceLocation rl;
	    //     rl.SetFileName( "data\\icons.gl" );
	    //     rl.group = 1;
	    //     rl.frame = 1206;	//激活
		// 	m_ToolTips.AddAnimation( &rl, false );	
		// 	//////////////////////////
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
		// 	Tips_AddString( "\n【裂地震波】" );	
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,214,0,239) );
		// 	Tips_AddString3( "\n状态技能，主动施放，使九屏范围内敌人进入眩晕状态。" );
		// }		
		if (m_object.syxqgrade22 >= 0) //等级
		{		
		m_ToolTips.AddText("\n");
		m_ToolTips.AddText("\n");
		m_ToolTips.SetCurColor( COLOR_ARGB(255,241,125,24) );
		Tips_AddString3( "代表功勋卓著经验丰富的勇士可获得更高的属性加成和强大技能！" );			
		m_ToolTips.AddText("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces2.gl");
		rl.group = 5;
		rl.frame = 17;
		m_ToolTips.AddAnimation(&rl, false);
		m_ToolTips.SetCurColor( COLOR_ARGB(255,234,191,140) );
		Tips_AddStringDefFont("   镶嵌效果   ");
		rl.SetFileName("data\\interfaces2.gl");
		rl.group = 5;
		rl.frame = 18;
		m_ToolTips.AddAnimation(&rl, false);
		}
	    if ( m_object.dwObjectID == 3831) //3831道具ID
	    {
        if (m_object.syxqgrade22 == 0)
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 0;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 10;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 20;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 30;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}					
		if (m_object.syxqgrade22 >= 1 && m_object.syxqgrade22 <= 100 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 1;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 11;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 21;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 31;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		}
		if (m_object.syxqgrade22 >= 101 && m_object.syxqgrade22 <= 200 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 2;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 12;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 22;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 32;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}
		if (m_object.syxqgrade22 >= 201 && m_object.syxqgrade22 <= 300 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 3;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 13;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 23;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 33;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}
		if (m_object.syxqgrade22 >= 301 && m_object.syxqgrade22 <= 400 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 4;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 14;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 24;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 34;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}
		if (m_object.syxqgrade22 >= 401 && m_object.syxqgrade22 <= 500 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 5;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 15;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 25;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 35;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}	
		if (m_object.syxqgrade22 >= 501 && m_object.syxqgrade22 <= 600 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 6;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 16;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 26;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 36;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}
		if (m_object.syxqgrade22 >= 601 && m_object.syxqgrade22 <= 700 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 7;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 17;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 27;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 37;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}
		if (m_object.syxqgrade22 >= 701 && m_object.syxqgrade22 <= 800 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 8;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 18;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 28;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 38;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}
		if (m_object.syxqgrade22 >= 801 && m_object.syxqgrade22 <= 1000 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 9;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 19;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 29;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 39;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}													
		}
		//勋章字纹结束			
		}
		else if ( m_object.dwObjectID == 3830)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n\n当前勋章品质：" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,255));
			Tips_AddString( "紫色" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddNum("\n升至紧箍咒品质所需等级：%d ", 100-m_object.dmedalt);
			Tips_AddString( "\n" );
		}		
		else if ( m_object.dwObjectID == 3829)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n\n当前勋章品质：" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,250,0));
			Tips_AddString( "绿色" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddNum("\n升至紫色品质所需等级：%d ", 100-m_object.dmedalt);
			Tips_AddString( "\n" );
		}
		else if ( m_object.dwObjectID == 3828)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n\n当前勋章品质：" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,250,250,50));
			Tips_AddString( "黄色" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddNum("\n升至绿色品质所需等级：%d ", 100-m_object.dmedalt);
			Tips_AddString( "\n" );
		}
		else if ( m_object.dwObjectID == 3827)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n\n当前勋章品质：" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			Tips_AddString( "蓝色" );			
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddNum("\n升至黄色品质所需等级：%d ", 100-m_object.dmedalt);
			Tips_AddString( "\n" );
		}
		else if ( m_object.dwObjectID == 3826)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n\n当前勋章品质：白色" );
			Tips_AddNum("\n升至蓝色品质所需等级：%d ", 100-m_object.dmedalt);
			Tips_AddString( "\n" );
		}		
	}

	if ( m_pObjectBase->dwType == ItemType_Shenbing0 )
	{   //myy  神兵类型94 单独属性显示
		//Add equipment detail property
		Tips_AddString( "\n" );
		stResourceLocation rt;
		rt.SetFileName( "data\\interfaces2.gl");
		rt.group = 500;
		rt.frame = 101;
		m_ToolTips.AddAnimation( &rt, false );
		Tips_AddString( "\n" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddString( "精炼等级加成：" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum("\n\n增加物理攻击力 +%u",m_object.p7damage);
		Tips_AddNum("\n增加魔法攻击力 +%u",m_object.m7damage);
		Tips_AddNum("\n增加物理防御力 +%u",m_object.p7defence);
		Tips_AddNum("\n增加魔法防御力 +%u",m_object.m7defence);
		Tips_AddNum("\n增加生命值上限 +%u",m_object.m7axhp);
		Tips_AddString( "\n" );
	}
    if ( m_pObjectBase->dwType == ItemType_Shenbing1)
	{   //myy  神兵类型95 单独属性显示
		//Add equipment detail property
		Tips_AddString( "\n" );
		stResourceLocation rt;
		rt.SetFileName( "data\\interfaces2.gl");
		rt.group = 500;
		rt.frame = 102;
		m_ToolTips.AddAnimation( &rt, false );
		Tips_AddString( "\n" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddString( "精炼等级加成：" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum("\n\n增加物理攻击力 +%u",m_object.p7damage);
		Tips_AddNum("\n增加魔法攻击力 +%u",m_object.m7damage);
		Tips_AddNum("\n增加物理防御力 +%u",m_object.p7defence);
		Tips_AddNum("\n增加魔法防御力 +%u",m_object.m7defence);
		Tips_AddNum("\n增加生命值上限 +%u",m_object.m7axhp);
		Tips_AddString( "\n" );
	}
	if ( m_pObjectBase->dwType == ItemType_Shenbing2)
	{   //myy  神兵类型96 单独属性显示
		//Add equipment detail property
		Tips_AddString( "\n" );
		stResourceLocation rt;
		rt.SetFileName( "data\\interfaces2.gl");
		rt.group = 500;
		rt.frame = 110;
		m_ToolTips.AddAnimation( &rt, false );
		Tips_AddString( "\n" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddString( "精炼等级加成：" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum("\n\n增加物理攻击力 +%u",m_object.p7damage);
		Tips_AddNum("\n增加魔法攻击力 +%u",m_object.m7damage);
		Tips_AddNum("\n增加物理防御力 +%u",m_object.p7defence);
		Tips_AddNum("\n增加魔法防御力 +%u",m_object.m7defence);
		Tips_AddNum("\n增加生命值上限 +%u",m_object.m7axhp);
		Tips_AddString( "\n" );
	}

	if ( m_pObjectBase->dwType == ItemType_Shenshou1)
	{   //myy  神兵类型97 单独属性显示
		//Add equipment detail property
		Tips_AddString( "\n" );
		stResourceLocation rt;
		rt.SetFileName( "data\\interfaces2.gl");
		rt.group = 500;
		rt.frame = 121;
		m_ToolTips.AddAnimation( &rt, false );
	    Tips_AddString( "\n" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddString( "神兽喂养等级加成：" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum("\n\n增加物理攻击力 +%u",m_object.p7damage);
		Tips_AddNum("\n增加魔法攻击力 +%u",m_object.m7damage);
		Tips_AddNum("\n增加物理防御力 +%u",m_object.p7defence);
		Tips_AddNum("\n增加魔法防御力 +%u",m_object.m7defence);
		Tips_AddNum("\n增加生命值上限 +%u",m_object.m7axhp);
		Tips_AddString( "\n" );
	}
	if ( m_pObjectBase->dwType == ItemType_Shenshou2)
	{   //myy  神兵类型98 单独属性显示
		//Add equipment detail property
		Tips_AddString( "\n" );
		stResourceLocation rt;
		rt.SetFileName( "data\\interfaces2.gl");
		rt.group = 500;
		rt.frame = 122;
		m_ToolTips.AddAnimation( &rt, false );
	    Tips_AddString( "\n" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddString( "神兽喂养等级加成：" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum("\n\n增加物理攻击力 +%u",m_object.p7damage);
		Tips_AddNum("\n增加魔法攻击力 +%u",m_object.m7damage);
		Tips_AddNum("\n增加物理防御力 +%u",m_object.p7defence);
		Tips_AddNum("\n增加魔法防御力 +%u",m_object.m7defence);
		Tips_AddNum("\n增加生命值上限 +%u",m_object.m7axhp);
		Tips_AddString( "\n" );
	}

	//soke 寒冰麒麟 属性显示
	if ( m_pObjectBase->dwType == ItemType_Hanbing )
	{
		//Add equipment detail property
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum1("\n\n额外增加物理攻击力 %d",m_object.p8damage);
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
        Tips_AddNum1("(+%d)",(m_object.p8damage * (m_object.m54defence)/100));
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));	
		Tips_AddNum1("\n额外增加魔法攻击力 %d",m_object.m8damage);
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
        Tips_AddNum1("(+%d)",(m_object.m8damage * (m_object.m54damage)/100));
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));		
		Tips_AddNum1("\n额外增加物理防御力 %d",m_object.p8defence);
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
		Tips_AddNum1("(+%d)",(m_object.p8defence * (m_object.p54defence)/100));
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum1("\n额外增加魔法防御力 %d",m_object.m8defence);
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
		Tips_AddNum1("(+%d)",(m_object.m8defence * (m_object.m54defence)/100));
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum1("\n额外增加生命值上限 %d",m_object.m8axhp);
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
		Tips_AddNum1("(+%d)",(m_object.m8axhp * (m_object.m54axhp)/100));
	    ///////////////////////////////显示加成百分比开始////////////////////////////	
	    //sky 寒冰纹章加成
     	if (m_object.syxqgrade26 > 0)
    	{
		    m_ToolTips.SetCurColor(COLOR_ARGB(255,25,110,230));
	    	Tips_AddNum2("\n麒麟纹章%u%级：提升麒麟基础属性%u%%", m_object.syxqgrade26,m_object.syxqgrade26);
	    }
	    ///////////////////////////////显示加成百分比结束////////////////////////////		
		Tips_AddString( "\n" );
		if (GetScene()->GetMainCharacter()->GetProperty()->hanbing == 1 )
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,25,110,230));
			Tips_AddString( "\n麒麟武器外形 开启" );
		}
		else
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,25,110,230));
			Tips_AddString( "\n麒麟武器外形 关闭" );
		}	
	    if ( m_pObjectBase->dwID == 3865 ) //道具
	    {		
	    if (m_object.syxqgrade24 >= 1)  //增加麒麟特效
	    {
		  stResourceLocation rl;
		  rl.SetFileName( "data\\interfaces7.gl");
		  rl.group = 30;
		  rl.frame = 101;
		  m_ToolTips.AddAnimation1s(&rl,-150,+20,-25,-65);
		  //Tips_AddString( "\n" );
	    }
		}				
	        ///////////////////寒冰神炼石开始////////////////////
			if (m_object.syxqgrade24 > 0 )
	        {		
			Tips_AddString( "\n" );	
			stResourceLocation imguserinfo1;
			imguserinfo1.SetFileName( "data\\interfaces7.gl");
			imguserinfo1.group = 400;
		    imguserinfo1.frame = 2;
		    m_ToolTips.AddAnimation( &imguserinfo1, false );
			m_ToolTips.Resize();
            Tips_AddString( "\n" );
		if (m_object.syxqgrade24 >= 0 && m_object.syxqgrade24 <= 100) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
		Tips_AddStringDefFont( "玄龙真麟（一阶）" );	//soke 物品名			
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n玄龙真麟：%u/100", m_object.syxqgrade24);
			Tips_AddStringDefFont( szTmp );	
		}
		if (m_object.syxqgrade24 >= 101 && m_object.syxqgrade24 <= 200) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
		Tips_AddStringDefFont( "玄龙真麟（二阶）" );	//soke 物品名			
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n玄龙真麟：%u/100", m_object.syxqgrade24-100);
			Tips_AddStringDefFont( szTmp );			
		}	
		if (m_object.syxqgrade24 >= 201 && m_object.syxqgrade24 <= 300) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
		Tips_AddStringDefFont( "玄龙真麟（三阶）" );	//soke 物品名			
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n玄龙真麟：%u/100", m_object.syxqgrade24-200);
			Tips_AddStringDefFont( szTmp );
		}	
		if (m_object.syxqgrade24 >= 301 && m_object.syxqgrade24 <= 400) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
		Tips_AddStringDefFont( "玄龙真麟（四阶）" );	//soke 物品名			
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n玄龙真麟：%u/100", m_object.syxqgrade24-300);
			Tips_AddStringDefFont( szTmp );
		}
		if (m_object.syxqgrade24 >= 401 && m_object.syxqgrade24 <= 500) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
		Tips_AddStringDefFont( "玄龙真麟（五阶）" );	//soke 物品名			
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n玄龙真麟：%u/100", m_object.syxqgrade24-400);
			Tips_AddStringDefFont( szTmp );
		}
		if (m_object.syxqgrade24 >= 501 && m_object.syxqgrade24 <= 600) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
		Tips_AddStringDefFont( "玄龙真麟（六阶）" );	//soke 物品名			
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n玄龙真麟：%u/100", m_object.syxqgrade24-500);
			Tips_AddStringDefFont( szTmp );
		}
		if (m_object.syxqgrade24 >= 601 && m_object.syxqgrade24 <= 700) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
		Tips_AddStringDefFont( "玄龙真麟（七阶）" );	//soke 物品名			
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n玄龙真麟：%u/100", m_object.syxqgrade24-600);
			Tips_AddStringDefFont( szTmp );
		}
		if (m_object.syxqgrade24 >= 701 && m_object.syxqgrade24 <= 800) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
		Tips_AddStringDefFont( "玄龙真麟（八阶）" );	//soke 物品名			
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n玄龙真麟：%u/100", m_object.syxqgrade24-700);
			Tips_AddStringDefFont( szTmp );
		}
		if (m_object.syxqgrade24 >= 801 && m_object.syxqgrade24 <= 900) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
		Tips_AddStringDefFont( "玄龙真麟（九阶）" );	//soke 物品名			
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n玄龙真麟：%u/100", m_object.syxqgrade24-800);
			Tips_AddStringDefFont( szTmp );
		}
		if (m_object.syxqgrade24 >= 901 && m_object.syxqgrade24 <= 1000) 
		{
		//soke 自定义补丁物品的颜色
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
		Tips_AddStringDefFont( "玄龙真麟（十阶）" );	//soke 物品名			
			char szTmp[512];
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,102,0));
			sprintf(szTmp, "\n玄龙真麟：%u/100", m_object.syxqgrade24-900);
			Tips_AddStringDefFont( szTmp );			
		}				
		    Tips_AddString( "\n" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,100,255,0));
			char szTmp[512];
			WORD shenfu_num = 1000;
			{
			   sprintf(szTmp,"剩余神炼次数 %u 次", shenfu_num-(m_object.syxqgrade24)) ;		      
			}
			Tips_AddString( szTmp );          
	        }	
            if(m_object.syxqgrade24 >0 )
	        {
		    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,126,40));
		    Tips_AddString( "\n神炼属性：" );
		    if ( m_object.m52axpdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,237,0,234));
			    Tips_AddNum1( "增加物攻 %d", m_object.m52axpdamage );
		    }
		    if ( m_object.m54axpdamage >= 0) //寒冰纹章增加物攻百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m52axpdamage * (m_object.m54axpdamage)/100));
		    }			
            if ( m_object.m52axmdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,237,0,234));
			    Tips_AddNum1( "增加魔攻 %d", m_object.m52axmdamage );			
		    }
		    if ( m_object.m54axmdamage >= 0) //寒冰纹章增加魔攻百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m52axmdamage * (m_object.m54axmdamage)/100));
		    }			
		    if ( m_object.p52defence > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,237,0,234));
			    Tips_AddNum1( "增加物防 %d", m_object.p52defence );			
		    }
		    if ( m_object.p54defence >= 0) //寒冰纹章增加物防百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.p52defence * (m_object.p54defence)/100));
		    }			
	        if ( m_object.m52defence > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,237,0,234));
			    Tips_AddNum1( "增加魔防 %d", m_object.m52defence );
		    }
		    if ( m_object.m54defence >= 0) //寒冰纹章增加魔防百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m52defence * (m_object.m54defence)/100));
		    }			
		    if ( m_object.m52axhp > 0 )
		    {
			    Tips_AddString( "\n" );			
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,237,0,234));
			    Tips_AddNum1( "增加生命 %d", m_object.m52axhp );			
            }	
		    if ( m_object.m54axhp >= 0) //寒冰纹章增加生命百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m52axhp * (m_object.m54axhp)/100));
		    }							
	        }
			///////////////////寒冰神炼石神炼结束////////////////////	
	        ///////////////////寒冰字纹石开始////////////////////
			if (m_object.syxqgrade25 > 0 )
	        {		
			Tips_AddString( "\n" );	
			stResourceLocation imguserinfo1;
			imguserinfo1.SetFileName( "data\\interfaces7.gl");
			imguserinfo1.group = 400;
		    imguserinfo1.frame = 2;
		    m_ToolTips.AddAnimation( &imguserinfo1, false );
			m_ToolTips.Resize();
            Tips_AddString( "\n" );
			m_ToolTips.SetCurColor(COLOR_ARGB(255,100,255,0));
			char szTmp[512];
			WORD shenfu_num = 1000;
			{
			   sprintf(szTmp,"剩余字纹次数 %u 次", shenfu_num-(m_object.syxqgrade25)) ;		      
			}
			Tips_AddString( szTmp );          
	        }	
            if(m_object.syxqgrade25 >0 )
	        {
		    m_ToolTips.SetCurColor(COLOR_ARGB(255,255,126,40));
		    Tips_AddString( "\n字纹属性：" );
		    if ( m_object.m53axpdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,198));
			    Tips_AddNum1( "增加物攻 %d", m_object.m53axpdamage );
		    }
		    if ( m_object.m54axpdamage >= 0) //寒冰纹章增加物攻百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m53axpdamage * (m_object.m54axpdamage)/100));
		    }			
            if ( m_object.m53axmdamage > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,198));
			    Tips_AddNum1( "增加魔攻 %d", m_object.m53axmdamage );			
		    }
		    if ( m_object.m54axmdamage >= 0) //寒冰纹章增加魔攻百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m53axmdamage * (m_object.m54axmdamage)/100));
		    }			
		    if ( m_object.p53defence > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,198));
			    Tips_AddNum1( "增加物防 %d", m_object.p53defence );			
		    }
		    if ( m_object.p54defence >= 0) //寒冰纹章增加物防百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.p53defence * (m_object.p54defence)/100));
		    }			
	        if ( m_object.m53defence > 0 )
		    {
			    Tips_AddString( "\n" );
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,198));
			    Tips_AddNum1( "增加魔防 %d", m_object.m53defence );
		    }
		    if ( m_object.m54defence >= 0) //寒冰纹章增加魔防百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m53defence * (m_object.m54defence)/100));
		    }			
		    if ( m_object.m53axhp > 0 )
		    {
			    Tips_AddString( "\n" );			
			    m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,198));
			    Tips_AddNum1( "增加生命 %d", m_object.m53axhp );			
            }	
		    if ( m_object.m54axhp >= 0) //寒冰纹章增加生命百分比
		    {
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
			Tips_AddNum("(+%d)",(m_object.m53axhp * (m_object.m54axhp)/100));
		    }							
	        }
	        if ( m_object.dwObjectID == 3865) //3865道具ID
	        {			
			//-----------------------------------------------------------------				
			///////////////////寒冰字纹石字纹结束////////////////////	
		// if (m_object.syxqgrade25 >= 0 && m_object.syxqgrade25 <= 499)
		// {
        //     m_ToolTips.AddText( "\n" );
		// 	stResourceLocation imguserinfo1;
		// 	imguserinfo1.SetFileName( "data\\interfaces7.gl");
		// 	imguserinfo1.group = 400;
		//     imguserinfo1.frame = 2;
		//     m_ToolTips.AddAnimation( &imguserinfo1, false );
		// 	m_ToolTips.Resize();			
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,128,128,128) );
        //     Tips_AddString( "\n字纹技能：未激活" );
		// 	m_ToolTips.AddText( "\n" );
		// 	stResourceLocation rl;
	    //     rl.SetFileName( "data\\icons.gl" );
	    //     rl.group = 1;
	    //     rl.frame = 3823;	//未激活
		// 	m_ToolTips.AddAnimation( &rl, false );	
		// 	//////////////////////////
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
		// 	Tips_AddString( "\n【麒麟真身】" );	
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,0) );
		// 	Tips_AddString3( "\n使自身和九屏内友军获得生命值额外加成并且血量回复满血，生命值加成数" );	
		// 	m_ToolTips.AddText3( "\n" );
		// 	m_ToolTips.AddText3( "\n" );
		// 	m_ToolTips.AddText3( "\n" );
		// 	m_ToolTips.AddText3( "\n" );			
		// 	Tips_AddString3( "\n值为最大生命值的30%，持续60秒。" );
		// 	//////////////////////////		
		// 	//-----------------------------------------------------------------		
		// }		

		// if (m_object.syxqgrade25 >= 500)
		// {
        //     m_ToolTips.AddText( "\n" );
		// 	stResourceLocation imguserinfo1;
		// 	imguserinfo1.SetFileName( "data\\interfaces7.gl");
		// 	imguserinfo1.group = 400;
		//     imguserinfo1.frame = 2;
		//     m_ToolTips.AddAnimation( &imguserinfo1, false );
		// 	m_ToolTips.Resize();			
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,214,0,239) );
		// 	Tips_AddString( "\n字纹技能：已激活" );
		// 	m_ToolTips.AddText( "\n" );
		// 	stResourceLocation rl;
	    //     rl.SetFileName( "data\\icons.gl" );
	    //     rl.group = 1;
	    //     rl.frame = 3821;	//已激活
		// 	m_ToolTips.AddAnimation( &rl, false );	
		// 	//////////////////////////
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
		// 	Tips_AddString( "\n【麒麟真身】" );	
		// 	m_ToolTips.SetCurColor( COLOR_ARGB(255,214,0,239) );
		// 	Tips_AddString3( "\n使自身和九屏内友军获得生命值额外加成并且血量回复满血，生命值加成数" );
		// 	m_ToolTips.AddText3( "\n" );
		// 	m_ToolTips.AddText3( "\n" );
		// 	m_ToolTips.AddText3( "\n" );
		// 	m_ToolTips.AddText3( "\n" );	
		// 	Tips_AddString3( "\n值为最大生命值的30%，持续60秒。" );
		// 	//////////////////////////		
		// 	//-----------------------------------------------------------------		
		// }							
		if (m_object.syxqgrade25 >= 0) //等级
		{		
		m_ToolTips.AddText("\n");
		m_ToolTips.AddText("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces7.gl");
		rl.group = 5;
		rl.frame = 17;
		m_ToolTips.AddAnimation(&rl, false);
		m_ToolTips.SetCurColor( COLOR_ARGB(255,234,191,140) );
		Tips_AddStringDefFont("   镶嵌效果   ");
		rl.SetFileName("data\\interfaces7.gl");
		rl.group = 5;
		rl.frame = 18;
		m_ToolTips.AddAnimation(&rl, false);
		}
		}
	    if ( m_object.dwObjectID == 3865) //3865道具ID
	    {
        if (m_object.syxqgrade25 == 0)
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 300;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 310;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 320;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 330;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}					
		if (m_object.syxqgrade25 >= 1 && m_object.syxqgrade25 <= 100 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 301;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 311;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 321;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 331;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		}
		if (m_object.syxqgrade25 >= 101 && m_object.syxqgrade25 <= 200 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 302;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 312;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 322;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 332;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}
		if (m_object.syxqgrade25 >= 201 && m_object.syxqgrade25 <= 300 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 303;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 313;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 323;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 333;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}
		if (m_object.syxqgrade25 >= 301 && m_object.syxqgrade25 <= 400 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 304;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 314;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 324;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 334;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}
		if (m_object.syxqgrade25 >= 401 && m_object.syxqgrade25 <= 500 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 305;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 315;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 325;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 335;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}	
		if (m_object.syxqgrade25 >= 501 && m_object.syxqgrade25 <= 600 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 306;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 316;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 326;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 336;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}
		if (m_object.syxqgrade25 >= 601 && m_object.syxqgrade25 <= 700 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 307;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 317;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 327;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 337;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}
		if (m_object.syxqgrade25 >= 701 && m_object.syxqgrade25 <= 800 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 308;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 318;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 328;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 338;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}
		if (m_object.syxqgrade25 >= 801 && m_object.syxqgrade25 <= 1000 )
		{
		{
			Tips_AddString( "\n" );
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 309;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 319;
			m_ToolTips.AddAnimation(&rl, false);
		}	
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 329;
			m_ToolTips.AddAnimation(&rl, false);
		}
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces7.gl");
			rl.group = 120;
			rl.frame = 339;
			m_ToolTips.AddAnimation(&rl, false);
		}
		}													
		}
		//寒冰字纹结束			
	}
	//寒冰总属性结束
	else if ( m_pObjectBase->dwType == ItemType_Chibang ) //soke 翅膀
	{
		{
			//Add equipment detail property
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
			Tips_AddString( "\n奖励属性" );

			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
			if (m_object.p8damage > 0 )
			{
				Tips_AddNum("\n增加物理攻击 +%u",m_object.p8damage);
			}
			else
			{
				Tips_AddString("\n增加物理攻击 +0");
			}
			if (m_object.m8damage > 0 )
			{
				Tips_AddNum("\n增加魔法攻击 +%u",m_object.m8damage);
			}
			else
			{
				Tips_AddString("\n增加魔法攻击 +0");
			}
			if (m_object.p8defence > 0 )
			{
				Tips_AddNum("\n增加物理防御 +%u",m_object.p8defence);
			}
			else
			{
				Tips_AddString("\n增加物理防御 +0");
			}
			if (m_object.m8defence > 0 )
			{
				Tips_AddNum("\n增加魔法防御 +%u",m_object.m8defence);
			}
			else
			{
				Tips_AddString("\n增加魔法防御 +0");
			}
			if (m_object.m8axhp > 0 )
			{
				Tips_AddNum("\n增加基础生命 +%u",m_object.m8axhp);
			}
			else
			{
				Tips_AddString("\n增加基础生命 +0");
			}
		}
		if (GetScene()->GetMainCharacter()->GetProperty()->angel == 1 )
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,255));
			Tips_AddString( "\n\n当前翅膀展开状态： 开启" );
		}
		else
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,255));
			Tips_AddString( "\n\n当前翅膀展开状态： 关闭" );
		}

		Tips_AddString( "\n" );
	}
	else if ( m_pObjectBase->dwType == ItemType_Fabao ) //soke 法宝
	{

		

			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			if (m_object.maxpdamage > 0 )
			{
				Tips_AddNum("\n基础物理攻击 +%u",m_object.maxpdamage);
			}
			else
			{
				Tips_AddString("\n基础物理攻击 +0");
			}
			if (m_object.maxmdamage > 0 )
			{
				Tips_AddNum("\n基础魔法攻击 +%u",m_object.maxmdamage);
			}
			else
			{
				Tips_AddString("\n基础魔法攻击 +0");
			}
			if (m_object.pdefence > 0 )
			{
				Tips_AddNum("\n基础物理防御 +%u",m_object.pdefence);
			}
			else
			{
				Tips_AddString("\n增加物理防御 +0");
			}
			if (m_object.mdefence > 0 )
			{
				Tips_AddNum("\n基础魔法防御 +%u",m_object.mdefence);
			}
			else
			{
				Tips_AddString("\n基础魔法防御 +0");
			}
			if (m_object.maxhp > 0 )
			{
				Tips_AddNum("\n基础生命上限 +%u",m_object.maxhp);
			}
			else
			{
				Tips_AddString("\n基础生命上限 +0");
			}
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddString("\n\n法宝说明");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			Tips_AddString("\n法宝可使用“玉竹简”在法宝助手页面进行强化,等级越高各项属性加成更强!");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));

			Tips_AddString("\n\n法宝强化属性");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,128,0));
			if (m_object.m20axpdamage > 0 )
			{
				Tips_AddNum("\n法宝强化物攻 +%u",m_object.m20axpdamage);
			}
			else
			{
				Tips_AddString("\n法宝强化物攻 +0");
			}
			if (m_object.m20axmdamage > 0 )
			{
				Tips_AddNum("\n法宝强化魔攻 +%u",m_object.m20axmdamage);
			}
			else
			{
				Tips_AddString("\n法宝强化物防 +0");
			}
				if (m_object.p20defence > 0 )
			{
				Tips_AddNum("\n法宝强化物防 +%u",m_object.p20defence);
			}
			else
			{
				Tips_AddString("\n法宝强化魔防 +0");
			}
				if (m_object.m20defence > 0 )
			{
				Tips_AddNum("\n法宝强化魔防 +%u",m_object.m20defence);
			}
			else
			{
				Tips_AddString("\n法宝强化魔攻 +0");
			}
				if (m_object.m20axhp > 0 )
			{
				Tips_AddNum("\n法宝强化生命 +%u",m_object.m20axhp);
			}
			else
			{
				Tips_AddString("\n法宝强化生命 +0");
			}

		
		Tips_AddString( "\n" );
	}

	

	else if ( m_pObjectBase->dwType>=ItemType_Fabaofj1 &&  m_pObjectBase->dwType<=ItemType_Fabaofj6) //soke 法宝
	{

			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			if (m_object.maxpdamage > 0 )
			{
				Tips_AddNum("\n基础物理攻击 +%u",m_object.maxpdamage);
			}
			else
			{
				Tips_AddString("\n基础物理攻击 +0");
			}
			if (m_object.maxmdamage > 0 )
			{
				Tips_AddNum("\n基础魔法攻击 +%u",m_object.maxmdamage);
			}
			else
			{
				Tips_AddString("\n基础魔法攻击 +0");
			}
			if (m_object.pdefence > 0 )
			{
				Tips_AddNum("\n基础物理防御 +%u",m_object.pdefence);
			}
			else
			{
				Tips_AddString("\n增加物理防御 +0");
			}
			if (m_object.mdefence > 0 )
			{
				Tips_AddNum("\n基础魔法防御 +%u",m_object.mdefence);
			}
			else
			{
				Tips_AddString("\n基础魔法防御 +0");
			}
			if (m_object.maxhp > 0 )
			{
				Tips_AddNum("\n基础生命上限 +%u",m_object.maxhp);
			}
			else
			{
				Tips_AddString("\n基础生命上限 +0");
			}
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddString("\n\n符石说明");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			Tips_AddString("\n符石可使用“紫金砂”在法宝助手页面进行强化,等级越高各项属性加成更强!");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));

			Tips_AddString("\n\n符石强化属性");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,128,0));
			if (m_object.m20axpdamage > 0 )
			{
				Tips_AddNum("\n符石强化物攻 +%u",m_object.m20axpdamage);
			}
			else
			{
				Tips_AddString("\n符石强化物攻 +0");
			}
			if (m_object.m20axmdamage > 0 )
			{
				Tips_AddNum("\n符石强化魔攻 +%u",m_object.m20axmdamage);
			}
			else
			{
				Tips_AddString("\n符石强化魔攻 +0");
			}
				if (m_object.p20defence > 0 )
			{
				Tips_AddNum("\n符石强化物防 +%u",m_object.p20defence);
			}
			else
			{
				Tips_AddString("\n符石强化物防 +0");
			}
				if (m_object.m20defence > 0 )
			{
				Tips_AddNum("\n符石强化魔防 +%u",m_object.m20defence);
			}
			else
			{
				Tips_AddString("\n符石强化魔防 +0");
			}
				if (m_object.m20axhp > 0 )
			{
				Tips_AddNum("\n符石强化生命 +%u",m_object.m20axhp);
			}
			else
			{
				Tips_AddString("\n符石强化生命 +0");
			}
		

		
		Tips_AddString( "\n" );
	}

	else if ( m_pObjectBase->dwType>=ItemType_Fabaofj7 &&  m_pObjectBase->dwType<=ItemType_Fabaofj12) //soke 法宝
	{

			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			if (m_object.maxpdamage 	> 0 )
			{
				Tips_AddNum("\n基础物理攻击 +%u",m_object.maxpdamage);
			}
			else
			{
				Tips_AddString("\n基础物理攻击 +0");
			}
			if (m_object.maxmdamage > 0 )
			{
				Tips_AddNum("\n基础魔法攻击 +%u",m_object.maxmdamage);
			}
			else
			{
				Tips_AddString("\n基础魔法攻击 +0");
			}
			if (m_object.pdefence > 0 )
			{
				Tips_AddNum("\n基础物理防御 +%u",m_object.pdefence);
			}
			else
			{
				Tips_AddString("\n增加物理防御 +0");
			}
			if (m_object.mdefence > 0 )
			{
				Tips_AddNum("\n基础魔法防御 +%u",m_object.mdefence);
			}
			else
			{
				Tips_AddString("\n基础魔法防御 +0");
			}
			if (m_object.maxhp > 0 )
			{
				Tips_AddNum("\n基础生命上限 +%u",m_object.maxhp);
			}
			else
			{
				Tips_AddString("\n基础生命上限 +0");
			}
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddString("\n\n灵石说明");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			Tips_AddString("\n灵石可使用“紫金砂”在法宝助手页面进行强化,等级越高各项属性加成更强!");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));

			Tips_AddString("\n\n灵石强化属性");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,128,0));
			if (m_object.m20axpdamage > 0 )
			{
				Tips_AddNum("\n灵石强化物攻 +%u",m_object.m20axpdamage);
			}
			else
			{
				Tips_AddString("\n灵石强化物攻 +0");
			}
			if (m_object.m20axmdamage > 0 )
			{
				Tips_AddNum("\n灵石强化魔攻 +%u",m_object.m20axmdamage);
			}
			else
			{
				Tips_AddString("\n灵石强化魔攻 +0");
			}
				if (m_object.p20defence > 0 )
			{
				Tips_AddNum("\n灵石强化物防 +%u",m_object.p20defence);
			}
			else
			{
				Tips_AddString("\n灵石强化物防 +0");
			}
				if (m_object.m20defence > 0 )
			{
				Tips_AddNum("\n灵石强化魔防 +%u",m_object.m20defence);
			}
			else
			{
				Tips_AddString("\n灵石强化魔防 +0");
			}
				if (m_object.m20axhp > 0 )
			{
				Tips_AddNum("\n灵石强化生命 +%u",m_object.m20axhp);
			}
			else
			{
				Tips_AddString("\n灵石强化生命 +0");
			}
		

		
		Tips_AddString( "\n" );
	}

	else if ( m_pObjectBase->dwType == ItemType_YUANSHEN ) //soke 元神
	{

		

			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			if (m_object.maxpdamage > 0 )
			{
				Tips_AddNum("\n基础物理攻击 +%u",m_object.maxpdamage);
			}
			else
			{
				Tips_AddString("\n基础物理攻击 +0");
			}
			if (m_object.maxmdamage > 0 )
			{
				Tips_AddNum("\n基础魔法攻击 +%u",m_object.maxmdamage);
			}
			else
			{
				Tips_AddString("\n基础魔法攻击 +0");
			}
			if (m_object.pdefence > 0 )
			{
				Tips_AddNum("\n基础物理防御 +%u",m_object.pdefence);
			}
			else
			{
				Tips_AddString("\n增加物理防御 +0");
			}
			if (m_object.mdefence > 0 )
			{
				Tips_AddNum("\n基础魔法防御 +%u",m_object.mdefence);
			}
			else
			{
				Tips_AddString("\n基础魔法防御 +0");
			}
			if (m_object.maxhp > 0 )
			{
				Tips_AddNum("\n基础生命上限 +%u",m_object.maxhp);
			}
			else
			{
				Tips_AddString("\n基础生命上限 +0");
			}
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddString("\n\n元神说明");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			Tips_AddString("\n元神可使用“天地元灵”在元神助手页面进行强化,等级越高各项属性加成更强!");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));

			Tips_AddString("\n\n元神强化属性");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,128,0));
			if (m_object.m22axpdamage > 0 )
			{
				Tips_AddNum("\n元神强化物攻 +%u",m_object.m22axpdamage);
			}
			else
			{
				Tips_AddString("\n元神强化物攻 +0");
			}
			if (m_object.m22axmdamage > 0 )
			{
				Tips_AddNum("\n元神强化魔攻 +%u",m_object.m22axmdamage);
			}
			else
			{
				Tips_AddString("\n元神强化物防 +0");
			}
				if (m_object.p22defence > 0 )
			{
				Tips_AddNum("\n元神强化物防 +%u",m_object.p22defence);
			}
			else
			{
				Tips_AddString("\n元神强化魔防 +0");
			}
				if (m_object.m22defence > 0 )
			{
				Tips_AddNum("\n元神强化魔防 +%u",m_object.m22defence);
			}
			else
			{
				Tips_AddString("\n元神强化魔攻 +0");
			}
				if (m_object.m22axhp > 0 )
			{
				Tips_AddNum("\n元神强化生命 +%u",m_object.m22axhp);
			}
			else
			{
				Tips_AddString("\n元神强化生命 +0");
			}
			if (m_object.retain23 >0 )
			{
				Tips_AddString( "\n" );
				stResourceLocation rt;
				rt.SetFileName( "data\\interfaces.gl");
				rt.group = 4;
				rt.frame = 596;
				m_ToolTips.AddAnimation( &rt, false );
				m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
				char szTmp[512];
				WORD ssbox_num = 1000;
				{
				sprintf(szTmp, " 元神宝石剩余镶嵌次数 %u 次", ssbox_num-(m_object.retain23)) ;		      
				}
				Tips_AddString( szTmp );           
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,113,0));
				Tips_AddString( "\n元神宝石属性：" );
				if ( m_object.m23axpdamage > 0 )
				{
					Tips_AddString( "\n" );
					stResourceLocation rt;
					rt.SetFileName( "data\\interfaces.gl");
					rt.group = 4;
					rt.frame = 600;
					m_ToolTips.AddAnimation( &rt, false );
					m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
					Tips_AddNum1( " 攻击宝石(物攻) +%u", m_object.m23axpdamage );
				}
				if ( m_object.m23axmdamage >0 )
				{
					Tips_AddString( "\n" );
					stResourceLocation rt;
					rt.SetFileName( "data\\interfaces.gl");
					rt.group = 4;
					rt.frame = 601;
					m_ToolTips.AddAnimation( &rt, false );
					m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
					Tips_AddNum1( " 攻击宝石(魔攻) +%u", m_object.m23axmdamage );
				}
				if ( m_object.p23defence >0 )
				{
					Tips_AddString( "\n" );
					stResourceLocation rt;
					rt.SetFileName( "data\\interfaces.gl");
					rt.group = 4;
					rt.frame = 602;
					m_ToolTips.AddAnimation( &rt, false );
					m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
					Tips_AddNum1( " 防御宝石(物防) +%u", m_object.p23defence );
				}
				if ( m_object.m23defence >0 )
				{
					Tips_AddString( "\n" );
					stResourceLocation rt;
					rt.SetFileName( "data\\interfaces.gl");
					rt.group = 4;
					rt.frame = 603;
					m_ToolTips.AddAnimation( &rt, false );
					m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
					Tips_AddNum1( " 防御宝石(魔防) +%u", m_object.m23defence );
				}
				if ( m_object.m23axhp >0 )
				{
					Tips_AddString( "\n" );
					stResourceLocation rt;
					rt.SetFileName( "data\\interfaces.gl");
					rt.group = 4;
					rt.frame = 604;
					m_ToolTips.AddAnimation( &rt, false );
					m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
					Tips_AddNum1( " 生命宝石(生命上限) +%u", m_object.m23axhp );
				}
			}
		

		
		Tips_AddString( "\n" );
	}

	

	else if ( m_pObjectBase->dwType>=ItemType_YUANSHENFJ1 &&  m_pObjectBase->dwType<=ItemType_YUANSHENFJ6) //soke 元神
	{

			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			if (m_object.maxpdamage > 0 )
			{
				Tips_AddNum("\n基础物理攻击 +%u",m_object.maxpdamage);
			}
			else
			{
				Tips_AddString("\n基础物理攻击 +0");
			}
			if (m_object.maxmdamage > 0 )
			{
				Tips_AddNum("\n基础魔法攻击 +%u",m_object.maxmdamage);
			}
			else
			{
				Tips_AddString("\n基础魔法攻击 +0");
			}
			if (m_object.pdefence > 0 )
			{
				Tips_AddNum("\n基础物理防御 +%u",m_object.pdefence);
			}
			else
			{
				Tips_AddString("\n增加物理防御 +0");
			}
			if (m_object.mdefence > 0 )
			{
				Tips_AddNum("\n基础魔法防御 +%u",m_object.mdefence);
			}
			else
			{
				Tips_AddString("\n基础魔法防御 +0");
			}
			if (m_object.maxhp > 0 )
			{
				Tips_AddNum("\n基础生命上限 +%u",m_object.maxhp);
			}
			else
			{
				Tips_AddString("\n基础生命上限 +0");
			}
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddString("\n\n元神装备说明");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			Tips_AddString("\n元神装备可使用“女娲之泪”在元神助手页面进行强化,等级越高各项属性加成更强!");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));

			Tips_AddString("\n\n元神装备强化属性");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,128,0));
			if (m_object.m22axpdamage > 0 )
			{
				Tips_AddNum("\n元神装备强化物攻 +%u",m_object.m22axpdamage);
			}
			else
			{
				Tips_AddString("\n元神装备强化物攻 +0");
			}
			if (m_object.m22axmdamage > 0 )
			{
				Tips_AddNum("\n元神装备强化魔攻 +%u",m_object.m22axmdamage);
			}
			else
			{
				Tips_AddString("\n元神装备强化魔攻 +0");
			}
				if (m_object.p22defence > 0 )
			{
				Tips_AddNum("\n元神装备强化物防 +%u",m_object.p22defence);
			}
			else
			{
				Tips_AddString("\n元神装备强化物防 +0");
			}
				if (m_object.m22defence > 0 )
			{
				Tips_AddNum("\n元神装备强化魔防 +%u",m_object.m22defence);
			}
			else
			{
				Tips_AddString("\n元神装备强化魔防 +0");
			}
				if (m_object.m22axhp > 0 )
			{
				Tips_AddNum("\n元神装备强化生命 +%u",m_object.m22axhp);
			}
			else
			{
				Tips_AddString("\n元神装备强化生命 +0");
			}

			if (m_object.retain23 >0 )
			{
				Tips_AddString( "\n" );
				stResourceLocation rt;
				rt.SetFileName( "data\\interfaces.gl");
				rt.group = 4;
				rt.frame = 596;
				m_ToolTips.AddAnimation( &rt, false );
				m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
				char szTmp[512];
				WORD ssbox_num = 1000;
				{
				sprintf(szTmp, " 元神宝石剩余镶嵌次数 %u 次", ssbox_num-(m_object.retain23)) ;		      
				}
				Tips_AddString( szTmp );           
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,113,0));
				Tips_AddString( "\n宝石属性：" );
				if ( m_object.m23axpdamage > 0 )
				{
					Tips_AddString( "\n" );
					stResourceLocation rt;
					rt.SetFileName( "data\\interfaces.gl");
					rt.group = 4;
					rt.frame = 600;
					m_ToolTips.AddAnimation( &rt, false );
					m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
					Tips_AddNum1( " 攻击宝石(物攻) +%u", m_object.m23axpdamage );
				}
				if ( m_object.m23axmdamage >0 )
				{
					Tips_AddString( "\n" );
					stResourceLocation rt;
					rt.SetFileName( "data\\interfaces.gl");
					rt.group = 4;
					rt.frame = 601;
					m_ToolTips.AddAnimation( &rt, false );
					m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
					Tips_AddNum1( " 攻击宝石(魔攻) +%u", m_object.m23axmdamage );
				}
				if ( m_object.p23defence >0 )
				{
					Tips_AddString( "\n" );
					stResourceLocation rt;
					rt.SetFileName( "data\\interfaces.gl");
					rt.group = 4;
					rt.frame = 602;
					m_ToolTips.AddAnimation( &rt, false );
					m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
					Tips_AddNum1( " 防御宝石(物防) +%u", m_object.p23defence );
				}
				if ( m_object.m23defence >0 )
				{
					Tips_AddString( "\n" );
					stResourceLocation rt;
					rt.SetFileName( "data\\interfaces.gl");
					rt.group = 4;
					rt.frame = 603;
					m_ToolTips.AddAnimation( &rt, false );
					m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
					Tips_AddNum1( " 防御宝石(魔防) +%u", m_object.m23defence );
				}
				if ( m_object.m23axhp >0 )
				{
					Tips_AddString( "\n" );
					stResourceLocation rt;
					rt.SetFileName( "data\\interfaces.gl");
					rt.group = 4;
					rt.frame = 604;
					m_ToolTips.AddAnimation( &rt, false );
					m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
					Tips_AddNum1( " 生命宝石(生命上限) +%u", m_object.m23axhp );
				}
			}
		
		Tips_AddString( "\n" );
	}

	else if ( m_pObjectBase->dwType >=ItemType_SEXX1 && m_pObjectBase->dwType <= ItemType_SEXX12 ) //soke 十二星宿
	{


			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			if (m_object.maxpdamage > 0 )
			{
				Tips_AddNum("\n基础物理攻击 +%u",m_object.maxpdamage);
			}
			else
			{
				Tips_AddString("\n基础物理攻击 +0");
			}
			if (m_object.maxmdamage > 0 )
			{
				Tips_AddNum("\n基础魔法攻击 +%u",m_object.maxmdamage);
			}
			else
			{
				Tips_AddString("\n基础魔法攻击 +0");
			}
			if (m_object.pdefence > 0 )
			{
				Tips_AddNum("\n基础物理防御 +%u",m_object.pdefence);
			}
			else
			{
				Tips_AddString("\n增加物理防御 +0");
			}
			if (m_object.mdefence > 0 )
			{
				Tips_AddNum("\n基础魔法防御 +%u",m_object.mdefence);
			}
			else
			{
				Tips_AddString("\n基础魔法防御 +0");
			}
			if (m_object.maxhp > 0 )
			{
				Tips_AddNum("\n基础生命上限 +%u",m_object.maxhp);
			}
			else
			{
				Tips_AddString("\n基础生命上限 +0");
			}
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddString("\n\n十二星宿说明");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
			Tips_AddString("\n十二星宿可使用“星宿石”在装备助手页面进行强化,等级越高各项属性加成更强!");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));

			Tips_AddString("\n\n十二星宿强化属性");
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,128,0));
			if (m_object.m21axpdamage > 0 )
			{
				Tips_AddNum("\n十二星宿强化物攻 +%u",m_object.m21axpdamage);
			}
			else
			{
				Tips_AddString("\n十二星宿强化物攻 +0");
			}
			if (m_object.m21axmdamage > 0 )
			{
				Tips_AddNum("\n十二星宿强化魔攻 +%u",m_object.m21axmdamage);
			}
			else
			{
				Tips_AddString("\n十二星宿强化物防 +0");
			}
				if (m_object.p21defence > 0 )
			{
				Tips_AddNum("\n十二星宿强化物防 +%u",m_object.p21defence);
			}
			else
			{
				Tips_AddString("\n十二星宿强化魔防 +0");
			}
				if (m_object.m21defence > 0 )
			{
				Tips_AddNum("\n十二星宿强化魔防 +%u",m_object.m21defence);
			}
			else
			{
				Tips_AddString("\n十二星宿强化魔攻 +0");
			}
				if (m_object.m21axhp > 0 )
			{
				Tips_AddNum("\n十二星宿强化生命 +%u",m_object.m21axhp);
			}
			else
			{
				Tips_AddString("\n十二星宿强化生命 +0");
			}
			Tips_AddString( "\n" );
			stResourceLocation rHole;
			rHole.SetFileName( "data\\interfaces3.gl" );
			rHole.group = 101;						
			if(m_pObjectBase->dwType == ItemType_SEXX1)
			{
				rHole.frame = 13;
			}
			else if(m_pObjectBase->dwType == ItemType_SEXX2)
			{
				rHole.frame = 9;
			}
			else if(m_pObjectBase->dwType == ItemType_SEXX3)
			{
				rHole.frame = 4;
			}
			else if(m_pObjectBase->dwType == ItemType_SEXX4)
			{
				rHole.frame = 8;
			}
			else if(m_pObjectBase->dwType == ItemType_SEXX5)
			{
				rHole.frame = 5;
			}
			else if(m_pObjectBase->dwType == ItemType_SEXX6)
			{
				rHole.frame = 10;
			}
			else if(m_pObjectBase->dwType == ItemType_SEXX7)
			{
				rHole.frame = 2;
			}
			else if(m_pObjectBase->dwType == ItemType_SEXX8)
			{
				rHole.frame = 1;
			}
			else if(m_pObjectBase->dwType == ItemType_SEXX9)
			{
				rHole.frame = 6;
			}
			else if(m_pObjectBase->dwType == ItemType_SEXX10)
			{
				rHole.frame = 11;
			}
			else if(m_pObjectBase->dwType == ItemType_SEXX11)
			{
				rHole.frame = 12;
			}
			else if(m_pObjectBase->dwType == ItemType_SEXX12)
			{
				rHole.frame = 3;
			}
			m_ToolTips.AddAnimation( &rHole, false );		
			Tips_AddString( "\n" );

	}


	//Skill effect
	{
		int num = 0;
		for( int k=0;k<10;k++ )
		{
			SkillBase_t* pSkill = GetSkillBase( m_object.skill[k].id );
			if ( pSkill )
			{
				if ( num == 0 )
					Tips_AddString( "\n" );
				num++ ;
				 m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
				Tips_AddText( "%s +", pSkill->pstrName );
				Tips_AddNum( "%u ", m_object.skill[k].point ); //soke 增加空格
				if ( num >= 2 )
					num = 0;
			}
		}
	}

		{
			static const char* SkillSerialName[] = { "武术系", "射术系", "法术系", "召唤系", "仙术系", "阵法" };
			if ( (m_object.skills.id > 0) && (m_object.skills.id < 7) )
			{
				Tips_AddText( "\n%s全系技能 +", SkillSerialName[m_object.skills.id-1] );
				Tips_AddNum( "%u", m_object.skills.point );
			}
		}

	//Add experience
	//if ( m_pObjectBase->dwType != ItemType_Pack )
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,86,234) );
		Tips_AddNum( "\n附加经验 %u", m_object.exp );
	}	

		//Green equipment
		//增加伤害值（包括增加伤害值x％、增加伤害值（人物等级/x）、增加远程伤害x%、增加远程伤害（人物等级/x）、）
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,137));
		//if( !( (spType == ST_LeechdomFood) && (m_object.damagebonus != 0) ) )
		//{
		//	Tips_AddNum("\n伤害损耗 %u%%",m_object.damagebonus);
		//	//Tips_AddNum("\n增加伤害值(人物等级/%u)",m_object.ehpv);
		//}

	if ( m_object.hpleech.effect > 0 )
		Tips_AddNum2("\n%u%% 的几率转化对敌人的伤害为自身生命 %u 点",m_object.hpleech.odds, m_object.hpleech.effect);
	if ( m_object.mpleech.effect > 0 )
		Tips_AddNum2("\n%u%% 的几率转化对敌人的伤害为自身法术 %u 点",m_object.mpleech.odds, m_object.mpleech.effect);
	////特殊类（神圣一击、转换生命值为法术值x％、x%忽视目标防御）
	m_ToolTips.SetCurColor(COLOR_ARGB(255,42,246,138));
	Tips_AddNum("\n增加神圣一击几率 %u%%", m_object.holy);
	//End add equipment special addation
	m_ToolTips.SetCurColor(COLOR_ARGB(255,42,246,138));
	Tips_AddNum("\n伤害 %u%% 转化为法术消耗",m_object.hptomp);
	Tips_AddNum("\n增加银子掉落 %u%%",m_object.incgold);
	Tips_AddNum("\n增加掉宝率 %u%%",m_object.mf);
	Tips_AddNum("\n双倍经验 %u%%",m_object.doublexp);
	//Add damage
	//Tips_AddNum( "\n增加伤害值 %u%%", m_object.damage );

	
	if (m_object.kind & 16 )	//神圣祝福道具
	{
		//soke 斗魂升级特效显示
		char szUdouhun[MAX_NAMESIZE];
		szUdouhun[0] = 0L;
		for( int k=0;k<m_object.douhun;k++ )
			strcat( szUdouhun, "★" );
		szUdouhun[strlen(szUdouhun)] = 0L;

		if ( strlen(szUdouhun) > 0 )
		{
			Tips_AddStringDefFontNAME( "\n" );
			if ( (m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118)
			  || (m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 134)
			  || (m_pObjectBase->dwType >= 140 )
			  || (m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147))
			{
				stResourceLocation rHole;
				rHole.SetFileName( "data\\interfaces1.gl" );
				rHole.group = 10;						
				if (m_object.douhun == 1)
				{
					rHole.frame = 1;
				}
				else if (m_object.douhun == 2)
				{
					rHole.frame = 1;
				}
				else if (m_object.douhun == 3)
				{
					rHole.frame = 1;
				}
				else if (m_object.douhun == 4)
				{
					rHole.frame = 1;
				}
				else if (m_object.douhun == 5)
				{
					rHole.frame = 1;
				}
				else if (m_object.douhun == 6)
				{
					rHole.frame = 2;
				}
				else if (m_object.douhun == 7)
				{
					rHole.frame = 2;
				}
				else if (m_object.douhun == 8)
				{
					rHole.frame = 2;
				}
				else if (m_object.douhun == 9)
				{
					rHole.frame = 2;
				}
				else if (m_object.douhun == 10)
				{
					rHole.frame = 3;
				}
				else if (m_object.douhun == 11)
				{
					rHole.frame = 3;
				}
				else if (m_object.douhun == 12)
				{
					rHole.frame = 3;
				}
				if (m_object.douhun <= 12)
				for (int i = 0; i<m_object.douhun; i++)
					m_ToolTips.AddAnimation( &rHole, false );
				else
						for (int i = 0; i<12; i++)
					m_ToolTips.AddAnimation( &rHole, false );		
			}
			else
				Tips_AddStringDefFontNAME( szUdouhun );

			//sky 斗魂13-15的显示
			if (m_object.douhun == 13)
			{
				stResourceLocation rHole1;
				rHole1.SetFileName( "data\\interfaces1.gl");
				rHole1.group = 10;
				rHole1.frame = 4;
				m_ToolTips.AddAnimation( &rHole1, false );
			}
			else if (m_object.douhun == 14)
			{
				stResourceLocation rHole2;
				rHole2.SetFileName( "data\\interfaces1.gl");
				rHole2.group = 10;
				rHole2.frame = 5;
				m_ToolTips.AddAnimation( &rHole2, false );
			}
			else if (m_object.douhun == 15)
			{
				stResourceLocation rHole3;
				rHole3.SetFileName( "data\\interfaces1.gl");
				rHole3.group = 10;
				rHole3.frame = 6;
				m_ToolTips.AddAnimation( &rHole3, false );
			}
		}

		if ( m_object.m3axhp > 0 )
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			Tips_AddNum( "\n斗魂附加生命值 %u%", m_object.m3axhp );
		}
		if ( m_object.m3axpdamage > 0 )
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			Tips_AddNum( "\n斗魂附加物理攻击 %u%", m_object.m3axpdamage );
		}
		if ( m_object.m3axmdamage > 0 )
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			Tips_AddNum( "\n斗魂附加魔法攻击 %u%", m_object.m3axmdamage );
		}
		if ( m_object.p3defence > 0 )
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			Tips_AddNum( "\n斗魂附加物理防御 %u%", m_object.p3defence );
		}
		if ( m_object.m3defence > 0 )
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			Tips_AddNum( "\n斗魂附加魔法防御 %u%", m_object.m3defence );
		}
	}

	if ( m_object.douhun > 0 )
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddString( "\nC键打开斗魂界面" );
	}
	
	//sky 龙槽龙星加成
	if (m_object.m4axhp > 0)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255, 255, 113, 0));
		Tips_AddNum2("\n龙槽龙星提升装备属性 %u%%(龙槽加成%u%%)", (m_object.m4axhp + m_object.m5axhp), m_object.m4axhp);
	}
	else if (m_object.m4axpdamage > 0)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255, 255, 113, 0));
		Tips_AddNum2("\n龙槽龙星提升装备属性 %u%%(龙槽加成%u%%)", (m_object.m4axpdamage + m_object.m5axpdamage), m_object.m4axpdamage);
	}
	else if (m_object.m4axmdamage > 0)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255, 255, 113, 0));
		Tips_AddNum2("\n龙槽龙星提升装备属性 %u%%(龙槽加成%u%%)", (m_object.m4axmdamage + m_object.m5axmdamage), m_object.m4axmdamage);
	}
	else if (m_object.p4defence > 0)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255, 255, 113, 0));
		Tips_AddNum2("\n龙槽龙星提升装备属性 %u%%(龙槽加成%u%%)", (m_object.p4defence + m_object.p5defence), m_object.p4defence);
	}
	else if (m_object.m4defence > 0)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255, 255, 113, 0));
		Tips_AddNum2("\n龙槽龙星提升装备属性 %u%%(龙槽加成%u%%)", (m_object.m4defence + m_object.m5defence), m_object.m4defence);
	}

	
	if(m_object.retain24>0) //龙凤吟
	{
		if(m_object.retain24==1)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n初级龙凤吟 等级 %u级",m_object.retain24 );
		}
		else if(m_object.retain24==2)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n中级龙凤吟 等级 %u级",m_object.retain24 );
		}
		else if(m_object.retain24==3)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n高级龙凤吟 等级 %u级",m_object.retain24 );
		}
		else if(m_object.retain24==4)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n顶级龙凤吟 等级 %u级",m_object.retain24 );
		}
		else if(m_object.retain24==5)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n超顶级龙凤吟 等级 %u级",m_object.retain24 );
		}
		else if(m_object.retain24==6)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n逆天龙凤吟 等级 %u级",m_object.retain24 );
		}
		else if(m_object.retain24==7)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n永恒龙凤吟 等级 %u级",m_object.retain24 );
		}
		else if(m_object.retain24==8)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n无双龙凤吟 等级 %u级",m_object.retain24 );
		}
		else if(m_object.retain24==9)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n真·永恒龙凤吟 等级 %u级",m_object.retain24 );
		}
		else if(m_object.retain24==10)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n真·无双龙凤吟 等级 %u级",m_object.retain24 );
		}
		else if(m_object.retain24==11)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n真·逆天龙凤吟 等级 %u级",m_object.retain24 );
		}
		else if(m_object.retain24==12)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n璀璨钻石龙凤吟 等级 %u级",m_object.retain24 );
		}
		else if(m_object.retain24==13)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n星空天穹龙凤吟 等级 %u级",m_object.retain24 );
		}
		else if(m_object.retain24==14)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n星空青铠龙凤吟 等级 %u级",m_object.retain24 );
		}
		else if(m_object.retain24==15)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n星空神绛龙凤吟 等级 %u级",m_object.retain24 );
		}
		else if(m_object.retain24==16)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
			Tips_AddNum( "\n星空丹绯龙凤吟 等级 %u级",m_object.retain24 );
		}


		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
		Tips_AddNum( "\n    物理攻击增加 + %u",m_object.m24axpdamage );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
		Tips_AddNum( "\n    魔法攻击增加 + %u",m_object.m24axmdamage );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
		Tips_AddNum( "\n    物理防御增加 + %u",m_object.p24defence );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
		Tips_AddNum( "\n    魔法防御增加 + %u",m_object.m24defence );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
		Tips_AddNum( "\n    生命值增加 + %u",m_object.m24axhp );

	}

    //myy 装备精炼等级显示
	if ( (m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118 )|| (m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 138 ) || (m_pObjectBase->dwType >= 140 && m_pObjectBase->dwType <= 147 ) || (m_pObjectBase->dwType >= 155 && m_pObjectBase->dwType <= 156 ))
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,97,0));
		if (m_object.itemrefine > 0 )
		{  
		   char szTmp[512];
		   if(m_pObjectBase->dwType == ItemType_HorseFashion) //马牌
		   {
                sprintf(szTmp, "\n当前马牌精炼等级:%u阶 ", m_object.itemrefine);
		   }
		   else
		   {
		       sprintf(szTmp, "\n当前装备精炼等级:%u阶 ", m_object.itemrefine);
		   }
		   Tips_AddString( szTmp );
		}			
	    if (16 & m_object.kind || 4 & m_object.kind)	
	    {
		     //myy 装备精炼
		     char szUrefine[MAX_NAMESIZE];
		     szUrefine[0] = 0L;
		     for( int k=0;k<m_object.itemrefine;k++ )
			 strcat( szUrefine, "★" );
		     szUrefine[strlen(szUrefine)] = 0L;

		    if ( strlen(szUrefine) > 0 )
		    {
			    Tips_AddStringDefFont( "\n" );
			    if ( (m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118 )|| (m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 138 ) || (m_pObjectBase->dwType >= 140 && m_pObjectBase->dwType <= 147 ) || (m_pObjectBase->dwType >= 155 && m_pObjectBase->dwType <= 156 ))
			    {
			        if (m_pObjectBase->dwType == ItemType_HorseFashion) //判别是否为马牌精炼显示升星图标
                    {			
                        if (m_object.itemrefine >= 1)
		                {
			                 stResourceLocation refineimg;
			                 refineimg.SetFileName( "data\\interfaces2.gl");
			                 refineimg.group = 375;
			                 refineimg.frame = 601;
			                 m_ToolTips.AddAnimation( &refineimg, false );
		                }
		                if (m_object.itemrefine >= 100)
		                {
			                 stResourceLocation refineimg;
			                 refineimg.SetFileName( "data\\interfaces2.gl");
			                 refineimg.group = 375;
			                 refineimg.frame = 603;
			                 m_ToolTips.AddAnimation( &refineimg, false );
		                }
						if (m_object.itemrefine >= 200)
		                {
			                 stResourceLocation refineimg;
			                 refineimg.SetFileName( "data\\interfaces2.gl");
			                 refineimg.group = 375;
			                 refineimg.frame = 605;
			                 m_ToolTips.AddAnimation( &refineimg, false );
		                }
						if (m_object.itemrefine >= 300)
		                {
			                 stResourceLocation refineimg;
			                 refineimg.SetFileName( "data\\interfaces2.gl");
			                 refineimg.group = 375;
			                 refineimg.frame = 607;
			                 m_ToolTips.AddAnimation( &refineimg, false );
		                }
						if (m_object.itemrefine >= 400)
		                {
			                 stResourceLocation refineimg;
			                 refineimg.SetFileName( "data\\interfaces2.gl");
			                 refineimg.group = 375;
			                 refineimg.frame = 609;
			                 m_ToolTips.AddAnimation( &refineimg, false );
		                }
						if (m_object.itemrefine >= 500)
		                {
			                 stResourceLocation refineimg;
			                 refineimg.SetFileName( "data\\interfaces2.gl");
			                 refineimg.group = 375;
			                 refineimg.frame = 1;
			                 m_ToolTips.AddAnimation( &refineimg, false );
		                }
						if (m_object.itemrefine >= 600)
		                {
			                 stResourceLocation refineimg;
			                 refineimg.SetFileName( "data\\interfaces2.gl");
			                 refineimg.group = 375;
			                 refineimg.frame = 2;
			                 m_ToolTips.AddAnimation( &refineimg, false );
		                }
						if (m_object.itemrefine >= 700)
		                {
			                 stResourceLocation refineimg;
			                 refineimg.SetFileName( "data\\interfaces2.gl");
			                 refineimg.group = 375;
			                 refineimg.frame = 3;
			                 m_ToolTips.AddAnimation( &refineimg, false );
		                }
						if (m_object.itemrefine >= 800)
		                {
			                 stResourceLocation refineimg;
			                 refineimg.SetFileName( "data\\interfaces2.gl");
			                 refineimg.group = 375;
			                 refineimg.frame = 4;
			                 m_ToolTips.AddAnimation( &refineimg, false );
		                }
						if (m_object.itemrefine >= 900)
		                {
			                 stResourceLocation refineimg;
			                 refineimg.SetFileName( "data\\interfaces2.gl");
			                 refineimg.group = 375;
			                 refineimg.frame = 5;
			                 m_ToolTips.AddAnimation( &refineimg, false );
		                }
						if (m_object.itemrefine >= 1000)
		                {
			                 stResourceLocation refineimg;
			                 refineimg.SetFileName( "data\\interfaces2.gl");
			                 refineimg.group = 375;
			                 refineimg.frame = 6;
			                 m_ToolTips.AddAnimation( &refineimg, false );
		                }
						if ( m_object.m10axpdamage > 0 )
		                {
			                m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			                Tips_AddNum1( "\n马牌精炼增加物攻 +%u", m_object.m10axpdamage );
		                }
		                if ( m_object.m10axmdamage > 0 )
		                {
			                m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			                Tips_AddNum1( "\n马牌精炼增加魔攻 +%u", m_object.m10axmdamage );
		                }
		                 if ( m_object.p10defence > 0 )
		                {
			                m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			                Tips_AddNum1( "\n马牌精炼增加物防 +%u", m_object.p10defence );
		                }
		                if ( m_object.m10defence > 0 )
		                {
			                m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			                Tips_AddNum1( "\n马牌精炼增加魔防 +%u", m_object.m10defence );
		                }
		                if ( m_object.m10axhp > 0 )
		                {
			                m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			                Tips_AddNum1( "\n马牌精炼增加生命 +%u", m_object.m10axhp );
                        }								    					
			        }
			        else if(m_object.itemrefine > 0)
			        {
				        stResourceLocation rHole;
				        rHole.SetFileName( "data\\interfaces1.gl" );
				        rHole.group = 10;						
				        if (m_object.itemrefine == 1)
				        {
					         rHole.frame = 7;
				        }
				        else if (m_object.itemrefine == 2)
				        {
					        rHole.frame = 7;
				        }
				        else if (m_object.itemrefine == 3)
				        {
					        rHole.frame = 7;
				        }
				        else if (m_object.itemrefine == 4)
				        {
				        	rHole.frame = 7;
				        }
						else if (m_object.itemrefine == 5)
				        {
				        	rHole.frame = 7;
				        }
						else if (m_object.itemrefine == 6)
				        {
				        	rHole.frame = 7;
				        }
						else if (m_object.itemrefine == 7)
				        {
				        	rHole.frame = 7;
				        }
						else if (m_object.itemrefine == 8)
				        {
				        	rHole.frame = 7;
				        }
						else if (m_object.itemrefine == 9)
				        {
				        	rHole.frame = 7;
				        }
						else if (m_object.itemrefine == 10)
				        {
				        	rHole.frame = 7;
				        }
				        else if (m_object.itemrefine > 10)
				        {
					         rHole.frame = 7;
				        }
				        for( int i=0;i<m_object.itemrefine;i++ )
					    m_ToolTips.AddAnimation( &rHole, false );
						if ( m_object.m10axpdamage > 0 )
		                {
			                m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			                Tips_AddNum1( "\n精炼等级加成物攻 +%u", m_object.m10axpdamage );
		                }
		                if ( m_object.m10axmdamage > 0 )
		                {
			                m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			                Tips_AddNum1( "\n精炼等级加成魔攻 +%u", m_object.m10axmdamage );
		                }
		                 if ( m_object.p10defence > 0 )
		                {
			                m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			                Tips_AddNum1( "\n精炼等级加成物防 +%u", m_object.p10defence );
		                }
		                if ( m_object.m10defence > 0 )
		                {
			                m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			                Tips_AddNum1( "\n精炼等级加成魔防 +%u", m_object.m10defence );
		                }
		                if ( m_object.m10axhp > 0 )
		                {
			                m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			                Tips_AddNum1( "\n精炼等级加成生命 +%u", m_object.m10axhp );
                        }					
						
			        }
					else
		            Tips_AddStringDefFont( szUrefine );	
	
			    }
								
			}
							
		}
			
	}
	//myy 百兽图鉴属性展示
	if ( m_pObjectBase->dwType >= ItemType_handbookitem1 && m_pObjectBase->dwType <= ItemType_handbookitem10)
	{
	if (m_object.handbooklvg >= 1) 
    {
		Tips_AddStringDefFont( "\n" );

        if (m_object.handbooklvg >= 1)  //百兽图鉴星星效果
        {
	         stResourceLocation handbooklvgimg;
	         handbooklvgimg.SetFileName( "data\\interfaces2.gl");
	         handbooklvgimg.group = 375;
	         handbooklvgimg.frame = 601;
	         m_ToolTips.AddAnimation( &handbooklvgimg, false );
			 m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));	      
        }
        if (m_object.handbooklvg >= 51)
        {
	         stResourceLocation handbooklvgimg;
	         handbooklvgimg.SetFileName( "data\\interfaces2.gl");
	         handbooklvgimg.group = 375;
	         handbooklvgimg.frame = 603;
	         m_ToolTips.AddAnimation( &handbooklvgimg, false );
        }		
        if (m_object.handbooklvg >= 101)
        {
	         stResourceLocation handbooklvgimg;
	         handbooklvgimg.SetFileName( "data\\interfaces2.gl");
	         handbooklvgimg.group = 375;
	         handbooklvgimg.frame = 605;
	         m_ToolTips.AddAnimation( &handbooklvgimg, false );
        }
        if (m_object.handbooklvg >= 151)
        {
	         stResourceLocation handbooklvgimg;
	         handbooklvgimg.SetFileName( "data\\interfaces2.gl");
	         handbooklvgimg.group = 375;
	         handbooklvgimg.frame = 607;
	         m_ToolTips.AddAnimation( &handbooklvgimg, false );
        }		
		if (m_object.handbooklvg >= 201)
        {
	         stResourceLocation handbooklvgimg;
	         handbooklvgimg.SetFileName( "data\\interfaces2.gl");
	         handbooklvgimg.group = 375;
	         handbooklvgimg.frame = 609;
	         m_ToolTips.AddAnimation( &handbooklvgimg, false );
        }
        if (m_object.handbooklvg >= 251)
        {
	         stResourceLocation handbooklvgimg;
	         handbooklvgimg.SetFileName( "data\\interfaces.gl");
	         handbooklvgimg.group = 4;
	         handbooklvgimg.frame = 503;
	         m_ToolTips.AddAnimation( &handbooklvgimg, false );
        }		
		if (m_object.handbooklvg >= 301)
        {
	         stResourceLocation handbooklvgimg;
	         handbooklvgimg.SetFileName( "data\\interfaces.gl");
	         handbooklvgimg.group = 4;
	         handbooklvgimg.frame = 504;
	         m_ToolTips.AddAnimation( &handbooklvgimg, false );
        }
        if (m_object.handbooklvg >= 351)
        {
	         stResourceLocation handbooklvgimg;
	         handbooklvgimg.SetFileName( "data\\interfaces2.gl");
	         handbooklvgimg.group = 375;
	         handbooklvgimg.frame = 1;
	         m_ToolTips.AddAnimation( &handbooklvgimg, false );
        }		
		if (m_object.handbooklvg >= 401)
        {
	         stResourceLocation handbooklvgimg;
	         handbooklvgimg.SetFileName( "data\\interfaces2.gl");
	         handbooklvgimg.group = 375;
	         handbooklvgimg.frame = 2;
	         m_ToolTips.AddAnimation( &handbooklvgimg, false );
        }
        if (m_object.handbooklvg >= 451)
        {
	         stResourceLocation handbooklvgimg;
	         handbooklvgimg.SetFileName( "data\\interfaces2.gl");
	         handbooklvgimg.group = 375;
	         handbooklvgimg.frame = 3;
	         m_ToolTips.AddAnimation( &handbooklvgimg, false );
        }	
        if (m_object.handbooklvg >= 501)
        {
	         stResourceLocation handbooklvgimg;
	         handbooklvgimg.SetFileName( "data\\interfaces2.gl");
	         handbooklvgimg.group = 375;
	         handbooklvgimg.frame = 4;
	         m_ToolTips.AddAnimation( &handbooklvgimg, false );
        }
        if (m_object.handbooklvg >= 551)
        {
	         stResourceLocation handbooklvgimg;
	         handbooklvgimg.SetFileName( "data\\interfaces2.gl");
	         handbooklvgimg.group = 375;
	         handbooklvgimg.frame = 5;
	         m_ToolTips.AddAnimation( &handbooklvgimg, false );
        }
        if (m_object.handbooklvg >= 601)
        {
	         stResourceLocation handbooklvgimg;
	         handbooklvgimg.SetFileName( "data\\interfaces2.gl");
	         handbooklvgimg.group = 375;
	         handbooklvgimg.frame = 6;
	         m_ToolTips.AddAnimation( &handbooklvgimg, false );
        }
        if (m_object.handbooklvg >= 651)
        {
	         stResourceLocation handbooklvgimg;
	         handbooklvgimg.SetFileName( "data\\interfaces2.gl");
	         handbooklvgimg.group = 375;
	         handbooklvgimg.frame = 7;
	         m_ToolTips.AddAnimation( &handbooklvgimg, false );
        }				
	//////////////////////////////分割线////////////////////////////////////								
		m_ToolTips.SetCurColor(COLOR_ARGB(255,247,129,230));
		if (m_object.handbooklvg >= 601) 
        {
		Tips_AddString( "\n当前图鉴品质:" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
		Tips_AddString(" 红色");		   
		}
		else if (m_object.handbooklvg >= 501) 
		{
		Tips_AddString( "\n当前图鉴品质:" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,251,105,5));
		Tips_AddString(" 橙色");			   
		}			
		else if (m_object.handbooklvg >= 401) 
		{
		Tips_AddString( "\n当前图鉴品质:" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,255));
		Tips_AddString(" 紫色");			   
		}		
		else if (m_object.handbooklvg >= 301) 
		{
		Tips_AddString( "\n当前图鉴品质:" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddString(" 绿色");
		}
		else if (m_object.handbooklvg >= 201) 
		{
		Tips_AddString( "\n当前图鉴品质:" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
		Tips_AddString(" 黄色");
		}
		else if (m_object.handbooklvg >= 101) 
		{
		Tips_AddString( "\n当前图鉴品质:" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,6,58,248));
		Tips_AddString(" 蓝色");
		}
		else if (m_object.handbooklvg >= 1) 
		{
		Tips_AddString( "\n当前图鉴品质:" );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		Tips_AddString(" 白色");
		}
		if ( m_object.m13axpdamage > 0 )
        {
	        m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
	        Tips_AddNum1( "\n图鉴进阶物攻 +%u", m_object.m13axpdamage );
        }
        if ( m_object.m13axmdamage > 0 )
        {
	        m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
	        Tips_AddNum1( "\n图鉴进阶魔攻 +%u", m_object.m13axmdamage );
        }
         if ( m_object.p13defence > 0 )
        {
	        m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
	        Tips_AddNum1( "\n图鉴进阶物防 +%u", m_object.p13defence );
        }
        if ( m_object.m13defence > 0 )
        {
	        m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
	        Tips_AddNum1( "\n图鉴进阶魔防 +%u", m_object.m13defence );
        }
        if ( m_object.m13axhp > 0 )
        {
	        m_ToolTips.SetCurColor(COLOR_ARGB(255,214,0,239));
			Tips_AddNum1( "\n图鉴进阶生命 +%u", m_object.m13axhp );
        }
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
	    Tips_AddNum1( "\n当前进阶次数 %u 次", m_object.handbooklvg );								    					
	}
	}
	//bright moon 逆天进阶属性展示
	if ( (m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118 )|| (m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147)|| (m_pObjectBase->dwType >= 136 && m_pObjectBase->dwType <= 138) || m_pObjectBase->dwType ==155 || m_pObjectBase->dwType ==156)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
		if (m_object.nitianlv > 0)
		{
			Tips_AddNum("\n逆天装备： %u 阶:",(m_object.nitianlv));
		}
		if (m_object.m15axhp > 0)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,249,114,4));
			Tips_AddNum("\n    生命增加 + %u",(m_object.m15axhp));
		}
		if (m_object.m15axpdamage > 0)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,249,114,4));
			Tips_AddNum("\n    物理攻击 + %u",(m_object.m15axpdamage));
		}
		if (m_object.m15axmdamage > 0)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,249,114,4));
			Tips_AddNum("\n    魔法攻击 + %u",(m_object.m15axmdamage));
		}
		if (m_object.p15defence > 0)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,249,114,4));
			Tips_AddNum("\n    物理防御 + %u",(m_object.p15defence));
		}
		if (m_object.m15defence > 0)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,249,114,4));
			Tips_AddNum("\n    魔法防御 + %u",(m_object.m15defence));
		}
	}
	//sky 神器加成
	if ( (m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118 )|| (m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147)|| (m_pObjectBase->dwType >= 136 && m_pObjectBase->dwType <= 138) || m_pObjectBase->dwType ==155 || m_pObjectBase->dwType ==156)
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,235,0));
		if (m_object.retain3 == 7)
		{
			Tips_AddString( "\n神器品质：神器七阶" );
		}
		else if (m_object.retain3 == 6)
		{
			Tips_AddString( "\n神器品质：神器六阶" );
		}
		else if (m_object.retain3 == 5)
		{
			Tips_AddString( "\n神器品质：神器五阶" );
		}
		else if (m_object.retain3 == 4)
		{
			Tips_AddString( "\n神器品质：神器四阶" );
		}
		else if (m_object.retain3 == 3)
		{
			Tips_AddString( "\n神器品质：神器三阶" );
		}
		else if (m_object.retain3 == 2)
		{
			Tips_AddString( "\n神器品质：神器二阶" );
		}
		else if (m_object.retain3 == 1)
		{
			Tips_AddString( "\n神器品质：神器一阶" );
		}

		if (m_object.m9axhp > 0)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,211,0,237));
			Tips_AddNum("\n    生命值增加 + %u",(m_object.maxhp * (m_object.m9axhp)/100));
		}
		else if (m_object.m9axpdamage > 0)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,211,0,237));
			Tips_AddNum("\n    物理攻击 + %u",(m_object.maxpdamage * (m_object.m9axpdamage)/100));
		}
		else if (m_object.m9axmdamage > 0)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,211,0,237));
			Tips_AddNum("\n    魔法攻击 + %u",(m_object.maxmdamage * (m_object.m9axmdamage)/100));
		}
		else if (m_object.p9defence > 0)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,211,0,237));
			Tips_AddNum("\n    物理防御 + %u",(m_object.pdefence * (m_object.p9defence)/100));
		}
		else if (m_object.m9defence > 0)
		{
			m_ToolTips.SetCurColor(COLOR_ARGB(255,211,0,237));
			Tips_AddNum("\n    魔法防御 + %u",(m_object.mdefence * (m_object.m9defence)/100));
		}
	}
		else if ( m_pObjectBase->dwID == 202400 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 0;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke --------------------------------- 	
	    else if ( m_pObjectBase->dwID == 202401 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 1;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202402 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 2;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202403 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 3;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	else if ( m_pObjectBase->dwID == 202404 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 4;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202405 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 5;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202406 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 6;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	    else if ( m_pObjectBase->dwID == 202407 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 7;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202408 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 8;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	    else if ( m_pObjectBase->dwID == 202409 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 9;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202410 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 10;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202411 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 11;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202412 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 12;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	    else if ( m_pObjectBase->dwID == 202413 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 13;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	    else if ( m_pObjectBase->dwID == 202414 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 14;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202415 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 15;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202416 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 16;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202417 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 17;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	    else if ( m_pObjectBase->dwID == 202418 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 18;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202419 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 19;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202420 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 20;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202421 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 21;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	else if ( m_pObjectBase->dwID == 202422 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 22;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	    else if ( m_pObjectBase->dwID == 202423 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 23;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202424 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 24;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202425 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 25;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202426 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 26;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202427 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 27;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------	
	    else if ( m_pObjectBase->dwID == 202428 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 28;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202429 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 29;
		m_ToolTips.AddAnimation( &rl, false );
	}
      //soke ---------------------------------
	    else if ( m_pObjectBase->dwID == 202430 )
	{
		//soke 坐骑图鉴显示坐骑效果的图片
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
		Tips_AddString( "\n右键可更换为此坐骑外形！" );
		Tips_AddString("\n");
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 2;
		rl.frame = 0;
		m_ToolTips.AddAnimation(&rl, false);
		{
		   Tips_AddString( "\n" );
	    }	    
		//soke 坐骑图鉴显示坐骑的图片
		stResourceLocation r2;
		rl.SetFileName( "data\\icons.gl");
		rl.group = 20;
		rl.frame = 30;
		m_ToolTips.AddAnimation( &rl, false );
	}
	  //结束 ---------------------------------
        //soke 这里应该是套装 
		//Golden equipment
		if ( (m_object.kind & 8) )			//&& !(g_tableItemSetBase.empty())
		{
			//Show set property
			//stSetObjectPropertyActive setActive;
			//if ( GetScene()->GetMainCharacter() )
			//{
			//	GetScene()->GetMainCharacter()->GetSetActiveByName( m_object.set, setActive );			//"幽冥"
			//}
			////Draw
			//static const char* property[] = { "\n增加攻击力2％",
			//								"\n增加法术攻击力2％", 
			//								"\n增加远程伤害2％",
			//								"\n增加仙术2％",
			//								"\n增加防御力2％",
			//								"\n增加法术防御力2％",
			//								"\n增加生命值5％", 
			//								"\n增加法术值5％", 
			//								"\n增加体力值5％", 
			//								"\n伤害反射10%", };
			//for( int i=0;i<SET_PROPERTY_NUM;i++ )
			//{
			//	if ( setActive.bActive[i] )
			//		m_ToolTips.SetCurColor( COLOR_ARGB(255,226,146,33) );
			//	else
			//		m_ToolTips.SetCurColor( COLOR_ARGB(255,92,83,79) );
			//	Tips_AddString( property[i] );
			//}
			/*m_ToolTips.SetCurColor( COLOR_ARGB(255,226,146,33) );
			Tips_AddNum( "\n物理伤害减少 %u%%", m_object.dpdam );
			Tips_AddNum( "\n魔法伤害减少 %u%%", m_object.dmdam );
			Tips_AddNum( "\n增加伤害 %u%%", m_object.bdam );
			Tips_AddNum( "\n伤害反射 %u%%", m_object.rdam );
			Tips_AddNum( "\n%%%u忽视目标防御", m_object.ignoredef );*/
		}

		/*//Just a test
		m_object.fivetype = 1;
		m_object.fivepoint = 33;

		m_object.fiveset[0] = 10101;	//fiveType=4
		m_object.fiveset[1] = 10751;	//fiveType=2
		m_object.fiveset[2] = 10801;	//fiveType=5
		m_object.fiveset[3] = 10851;	//fiveType=3
		m_object.fiveset[4] = 0;
		//End test*/

		//Five
		/*if ( (m_object.fivetype >= FIVE_METAL) && (m_object.fivetype<FIVE_NONE) && (m_object.fivepoint > 0) )
		{
		m_ToolTips.SetCurColor( fiveColor[m_object.fivetype] );
		if ( bDefenceEquip )
		{
		Tips_AddText( "\n减少来自无五行与%s属性目标的", fiveName[GetFiveBack(m_object.fivetype)] );
		Tips_AddNum( "伤害 %u%%", m_object.fivepoint );
		}

		if ( bAttackEquip )
		{
			Tips_AddText( "\n对无五行与%s属性目标", fiveName[GetFiveBack(m_object.fivetype)] );
			Tips_AddNum( "伤害上升 %u%%", m_object.fivepoint );
		}
	}*/
	//End five

	//Five set
	static const char* fiveDesc[] = { "\n   物理伤害减少 ", "\n   魔法伤害减少 ","\n   增加伤害 ", "\n   伤害反射 ", "忽视目标防御", "", 
		                              "\n   物理防御力增强 ", "\n   魔法防御力增强 ", "\n   增加物理攻击力 ", "\n   增加魔法攻击力 ",
									  "\n   增加生命值 "};
	struct fivePro
	{
		BYTE fiveValue;
		BYTE proType;
	};
	fivePro fiveProperty[FIVE_NONE];
	for( int k=0;k<FIVE_NONE;k++ )
	{
		fiveProperty[k].fiveValue = 0;
		fiveProperty[k].proType = FIVE_NONE;
	}
	int index = 0;
	if ( m_object.dpdam ) //物理伤害减少 x%
	{
		fiveProperty[index].fiveValue = m_object.dpdam;
		fiveProperty[index++].proType = 0;
	}
	if ( m_object.dmdam ) //魔法伤害减少 x%
	{
		fiveProperty[index].fiveValue = m_object.dmdam;
		fiveProperty[index++].proType = 1;
	}
	if ( m_object.bdam ) //增加伤害 x%
	{
		fiveProperty[index].fiveValue = m_object.bdam;
		fiveProperty[index++].proType = 2;
	}
	if ( m_object.rdam ) //伤害反射 x%
	{
		fiveProperty[index].fiveValue = m_object.rdam;
		fiveProperty[index++].proType = 3;
	}
	if ( m_object.ignoredef ) //x% 忽视目标防御
	{
		fiveProperty[index].fiveValue = m_object.ignoredef;
		fiveProperty[index++].proType = 4;
	}
	//soke 开始第二灵魂配置
	if ( m_object.addpdef ) //物理防御力增强 600
	{
		fiveProperty[index].fiveValue = m_object.addpdef;
		fiveProperty[index++].proType = 6;
	}
	if ( m_object.addmdef ) //魔法防御力增强 600
	{
		fiveProperty[index].fiveValue = m_object.addmdef;
		fiveProperty[index++].proType = 7;
	}
	if ( m_object.addpdam ) //增加物理攻击力 1200
	{
		fiveProperty[index].fiveValue = m_object.addpdam;
		fiveProperty[index++].proType = 8;
	}
	if ( m_object.addmdam ) //增加魔法攻击力 1200
	{
		fiveProperty[index].fiveValue = m_object.addmdam;
		fiveProperty[index++].proType = 9;
	}
	if ( m_object.addhp ) //增加生命值 10000
	{
		fiveProperty[index].fiveValue = m_object.addhp;
		fiveProperty[index++].proType = 10;
	}

	int nNextFiveType = GetFiveBorn( m_object.fivetype );
	for( int five=0;five<FIVE_NONE;five++ )
	{
		if ( m_object.fiveset[five] == 0 )
			break;

		if ( GetScene()->GetMainCharacter() )
		{
			//See if active five property
			bool bActiveFive = m_object.pos.dwLocation == OBJECTCELLTYPE_EQUIP || OBJECTCELLTYPE_STORE || OBJECTCELLTYPE_PACKAGE;
			
			CRoleItem* pItem = NULL;			
			if ( m_object.pos.dwLocation == OBJECTCELLTYPE_EQUIPSHOW )	//soke 对方的装备
			{
				if ( GetGameGuiManager()->m_guiUserInfoShow )
				{
					pItem = GetGameGuiManager()->m_guiUserInfoShow->FindItemInEquipListByFiveTypeConstID(m_object.fiveset[five], nNextFiveType);
					//pItem = GetGameGuiManager()->m_guiUserInfoShow->FindItemInEquipListByFiveTypeConstID( m_object.dwObjectID, m_object.fivetype );
				}
			}
			else
				//soke 装备灵魂激活显示的颜色
				pItem = GetScene()->GetMainCharacter()->FindItemByLocationFiveTypeConstID( m_object.fiveset[five], OBJECTCELLTYPE_EQUIP, nNextFiveType );
			
			if ( NULL == pItem )
			{
				bActiveFive = false;
				//pItem = GetScene()->GetMainCharacter()->FindItemByConstID( m_object.fiveset[five] );
			}

				ObjectBase_t* pObjectBase = GetObjectBase( m_object.fiveset[five] );
				if ( pObjectBase == NULL )
					break;

			{
				//if ( five > 0 )
				{
					if ( pItem && (pItem->GetObject()->fivetype != nNextFiveType) )
						bActiveFive = false;
				}

				//Add property tips
				if ( five == 0 )
				{
				    //soke 修改文字颜色
					m_ToolTips.SetCurColor( COLOR_ARGB(255,184,0,237) );
					Tips_AddString( "\n灵魂锁链：" );
				}

				if ( bActiveFive )
				    //soke 修改灵魂激活颜色
					m_ToolTips.SetCurColor( COLOR_ARGB(255,184,0,237) );
				else
				   //soke 修改灵魂未激活颜色
					m_ToolTips.SetCurColor( COLOR_ARGB(255,73,73,71) );

                //soke 显示激活需要的装备
			    Tips_AddNum( "\n(%u)", five+1 ); // 换行
				Tips_AddText( "%s", pObjectBase->strName );
				if ( 0/*pItem*/ )
				{
					Tips_AddText( "【%s】", fiveName[pItem->GetObject()->fivetype] );
				}
				else
				{   //soke 灵魂锁链的五行 风雨雷电火
					Tips_AddText( "【%s】", fiveName[nNextFiveType] );
				}

				
				if ( fiveProperty[five].fiveValue && (fiveProperty[five].proType != FIVE_NONE) )
				{
					//soke 物理减少 x% 等
					if ( fiveProperty[five].proType <= (FIVE_NONE-2) )
					{
						Tips_AddText( "%s", fiveDesc[fiveProperty[five].proType] );
						Tips_AddNum( "%u%%", fiveProperty[five].fiveValue );
					}
					//soke x% 忽视目标防御(物理减少等相反)
					if ( fiveProperty[five].proType == (FIVE_NONE-1) )
					{
						Tips_AddNum( "\n   %u%% ", fiveProperty[five].fiveValue );
						Tips_AddText( "%s", fiveDesc[fiveProperty[five].proType] );
					}
					//soke 第二灵魂（除生命）
					if ( fiveProperty[five].proType > (FIVE_NONE) && fiveProperty[five].proType <= (FIVE_NONE+4) )
					{
						Tips_AddText( "%s", fiveDesc[fiveProperty[five].proType] );
						Tips_AddNum( "%u0%", fiveProperty[five].fiveValue );
					}
					//soke 第二灵魂（生命）
					if ( fiveProperty[five].proType == (FIVE_NONE+5) )
					{
						Tips_AddText( "%s", fiveDesc[fiveProperty[five].proType] );
						Tips_AddNum( "%u00%", fiveProperty[five].fiveValue );
					}
				}
                //恋恋取消第二灵魂显示
				break;
				    //soke 注释，第二灵魂与第一灵魂五行相同
				//	nNextFiveType = GetFiveBorn( nNextFiveType );


					/*switch( five )
					{
					if ( fiveProperty[five].proType != (FIVE_NONE-1) )
					{
					Tips_AddText( "%s", fiveDesc[fiveProperty[five].proType] );
					Tips_AddNum( "%u%%", fiveProperty[five].fiveValue );
					}
					else
					{
					Tips_AddNum( "\n   %u%% ", fiveProperty[five].fiveValue );
					Tips_AddText( "%s", fiveDesc[fiveProperty[five].proType] );
					}
					}


					/*switch( five )
					{
					if ( fiveProperty[five].proType != (FIVE_NONE-1) )
					{
					Tips_AddText( "%s", fiveDesc[fiveProperty[five].proType] );
					Tips_AddNum( "%u%%", fiveProperty[five].fiveValue );
					}
					else
					{
					Tips_AddNum( "\n   %u%% ", fiveProperty[five].fiveValue );
					Tips_AddText( "%s", fiveDesc[fiveProperty[five].proType] );
					}
					}


					/*switch( five )
					{
					case 0:
					Tips_AddNum( "\n   物理伤害减少 %u%%", m_object.dpdam );
					break;
					case 1:
					Tips_AddNum( "\n   魔法伤害减少 %u%%", m_object.dmdam );
					break;
					case 2:
					Tips_AddNum( "\n   增加伤害 %u%%", m_object.bdam );
					break;
					case 3:
					Tips_AddNum( "\n   伤害反射 %u%%", m_object.rdam );
					break;
					case 4:
					Tips_AddNum( "\n   %u%% 忽视目标防御", m_object.ignoredef );
					break;
					}								

					Tips_AddNum( "\n   %u%% 忽视目标防御", m_object.ignoredef );
					break;
					}				*/				

				}
			}
		}
		//End five set
		//end equipment property

	//soke 增加装备星星数量显示
	if ( (m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118) || (m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 134) || (m_pObjectBase->dwType >= 136 && m_pObjectBase->dwType <= 138)
		|| (m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147) || m_pObjectBase->dwType == 155 || m_pObjectBase->dwType == 156 )
	{
		if ( m_object.xingshu <= 0)
		{
			//soke 0星目前不显示
		    //  m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    //  Tips_AddString( "\n当前最高星数：0星" );
		}
		else if ( m_object.xingshu == 1)
		{
		    m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    Tips_AddString( "\n历史最高星数：1星" );
		}
		else if ( m_object.xingshu == 2)
		{
		    m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    Tips_AddString( "\n历史最高星数：2星" );
		}
		else if ( m_object.xingshu == 3)
		{
		    m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    Tips_AddString( "\n历史最高星数：3星" );
		}
		else if ( m_object.xingshu == 4)
		{
		    m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    Tips_AddString( "\n历史最高星数：4星" );
		}
		else if ( m_object.xingshu == 5)
		{
		    m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    Tips_AddString( "\n历史最高星数：5星" );
		}
		else if ( m_object.xingshu == 6)
		{
		    m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    Tips_AddString( "\n历史最高星数：6星" );
		}
		else if ( m_object.xingshu == 7)
		{
		    m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    Tips_AddString( "\n历史最高星数：7星" );
		}
		else if ( m_object.xingshu == 8)
		{
		    m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    Tips_AddString( "\n历史最高星数：8星" );
		}
		else if ( m_object.xingshu == 9)
		{
		    m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    Tips_AddString( "\n历史最高星数：9星" );
		}
		else if ( m_object.xingshu == 10)
		{
		    m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    Tips_AddString( "\n历史最高星数：10星" );
		}
		else if ( m_object.xingshu == 11)
		{
		    m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    Tips_AddString( "\n历史最高星数：11星" );
		}
		else if ( m_object.xingshu == 12)
		{
		    m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    Tips_AddString( "\n历史最高星数：12星" );
		}
		else if ( m_object.xingshu == 13)
		{
		    m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    Tips_AddString( "\n历史最高星数：13星" );
		}
		else if ( m_object.xingshu == 14)
		{
		    m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    Tips_AddString( "\n历史最高星数：14星" );
		}
		else if ( m_object.xingshu >= 15)
		{
		    m_ToolTips.SetCurColor( COLOR_ARGB(255,255,113,0) );
		    Tips_AddString( "\n历史最高星数：15星" );
		}
	}

	//孔
	//Get hole about
	{
		int holeNum = 0;
		int diamondNum = 0;
		for( int i=0;i<6;i++ )
		{
			if ( m_object.socket[i] != 0 )
			{
				holeNum++ ;
				if ( m_object.socket[i] != -1 )
					diamondNum++ ;
			}
		}
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,86,234));
		Tips_AddNum( "\n有凹槽 %u", holeNum );
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));

		//Add hole & diamond image
		stResourceLocation rHole;
		rHole.SetFileName( "data\\interfaces.gl" );
		rHole.group = 4;
		rHole.frame = 605;

		stResourceLocation rHoleDiamond;
		rHoleDiamond.SetFileName( "data\\interfaces.gl" );
		rHoleDiamond.group = 4;
		rHoleDiamond.frame = 610;

		int k;
	    int num = 0;
		if ( holeNum > 0 )
        {
			Tips_AddString( "\n" );
			for( k=0;k<6;k++ )
			{
				if ( m_object.socket[k] !=0 )
				{
					if ( m_object.socket[k] != -1 )		//diamond
					{
						int imgID = GetDiamondResourceFrameFromID( m_object.socket[k] );
						if ( imgID != -1 )
						{
							num++ ;
							rHoleDiamond.frame = imgID;
							m_ToolTips.AddAnimation( &rHoleDiamond, false );
							if ( num >= 3 )
							{
									num = 0;
									Tips_AddString( "\n" );
							}
						}
					}
					else								//hole
					{
						num++ ;
						m_ToolTips.AddAnimation( &rHole, false );
						if ( num >= 3 )
						{
							num = 0;
								Tips_AddString( "\n" );
						}
					}
				}
			}
		}
	// }			
	// //bright moon 神魂宝石(斗战圣佛)属性展示
	// if ( (m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118 )|| (m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147)|| (m_pObjectBase->dwType >= 136 && m_pObjectBase->dwType <= 138) || m_pObjectBase->dwType ==155 || m_pObjectBase->dwType ==156)
	// {
	// //	m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
	// 	//if (m_object.douzhansflv > 0)
	// //	{
	// 	//	Tips_AddNum("\n神魂宝石 %u 级:",(m_object.douzhansflv));
	// //	}
    //     if (m_object.douzhansflv >= 25)
	// 	{
	// 		Tips_AddString("\n"); //斗
	// 		stResourceLocation rl;
	// 		rl.SetFileName("data\\interfaces2.gl");
	// 		rl.group = 49;
	// 		rl.frame = 9;
	// 		m_ToolTips.AddAnimation(&rl, false);
	// 	}
	// 	if (m_object.douzhansflv >= 50)
	// 	{
	// 		stResourceLocation rl;
	// 		rl.SetFileName("data\\interfaces2.gl");
	// 		rl.group = 49;
	// 		rl.frame = 19;
	// 		m_ToolTips.AddAnimation(&rl, false);
	// 	}
	// 	if (m_object.douzhansflv >= 75) //圣
	// 	{
	// 		stResourceLocation rl;
	// 		rl.SetFileName( "data\\interfaces2.gl");
	// 		rl.group = 49;
	// 		rl.frame = 29;
	// 		m_ToolTips.AddAnimation( &rl, false );
	// 	}
	// 	if (m_object.douzhansflv >= 100) //佛
	// 	{
	// 		stResourceLocation rl;
	// 		rl.SetFileName( "data\\interfaces2.gl");
	// 		rl.group = 49;
	// 		rl.frame = 39;
	// 		m_ToolTips.AddAnimation( &rl, false );
	//     }
	// 	if (m_object.m16axhp > 0)
	// 	{
	// 		m_ToolTips.SetCurColor(COLOR_ARGB(255,193,7,253));
	// 		Tips_AddNum("\n神魂宝石生命 + %u",(m_object.m16axhp));
	// 	}
	// 	if (m_object.m16axpdamage > 0)
	// 	{
	// 		m_ToolTips.SetCurColor(COLOR_ARGB(255,193,7,253));
	// 		Tips_AddNum("\n神魂宝石物攻 + %u",(m_object.m16axpdamage));
	// 	}
	// 	if (m_object.m16axmdamage > 0)
	// 	{
	// 		m_ToolTips.SetCurColor(COLOR_ARGB(255,193,7,253));
	// 		Tips_AddNum("\n神魂宝石魔攻 + %u",(m_object.m16axmdamage));
	// 	}
	// 	if (m_object.p16defence > 0)
	// 	{
	// 		m_ToolTips.SetCurColor(COLOR_ARGB(255,193,7,253));
	// 		Tips_AddNum("\n神魂宝石物防 + %u",(m_object.p16defence));
	// 	}
	// 	if (m_object.m16defence > 0)
	// 	{
	// 		m_ToolTips.SetCurColor(COLOR_ARGB(255,193,7,253));
	// 		Tips_AddNum("\n神魂宝石魔防 + %u",(m_object.m16defence));
	// 	}
	// }	
	
	if ( (m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118) || (m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 134) || (m_pObjectBase->dwType >= 136 && m_pObjectBase->dwType <= 138)
		|| (m_pObjectBase->dwType >= 140 && m_pObjectBase->dwType <= 147) || m_pObjectBase->dwType == 155 || m_pObjectBase->dwType == 156  )
		{
			if(m_object.kind & 16 || m_object.kind & 4) //必须紫装
			{	

				if(m_object.douzhansflv>0)
				{
					Tips_AddString( "\n" ); //网管注释这个换行省点装备空间2025-1-5 03:31:22
					stResourceLocation r2;
					r2.SetFileName("data\\interfaces.gl");
					r2.group = 289;
					r2.frame = 17;
					m_ToolTips.AddAnimation(&r2, false);
					m_ToolTips.SetCurColor( COLOR_ARGB(255,234,191,140) );
					Tips_AddStringDefFont("    镶嵌效果    "); //网管字纹镶嵌图标2024-6-5 15:07:51
					r2.SetFileName("data\\interfaces.gl");
					r2.group = 289;
					r2.frame = 18;
					m_ToolTips.AddAnimation(&r2, false);
				}
				
				Tips_AddString( "\n" );

				if (m_object.hongtiannum1 > 0)//武器
				{
					stResourceLocation r2;
					r2.SetFileName("data\\items.gl");
					r2.group = 0;
					r2.frame = 15000+m_object.hongtiannum1;
					m_ToolTips.AddAnimation(&r2, false);
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum2 > 0)//项链
				{
					stResourceLocation r2;
					r2.SetFileName("data\\items.gl");
					r2.group = 0;
					r2.frame = 15100+m_object.hongtiannum2;
					m_ToolTips.AddAnimation(&r2, false);
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum3 > 0)//护腕
				{
					stResourceLocation r2;
					r2.SetFileName("data\\items.gl");
					r2.group = 0;
					r2.frame = 15200+m_object.hongtiannum3;
					m_ToolTips.AddAnimation(&r2, false);
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum4 > 0)//头盔
				{
					stResourceLocation r2;
					r2.SetFileName("data\\items.gl");
					r2.group = 0;
					r2.frame = 15300+m_object.hongtiannum4;
					m_ToolTips.AddAnimation(&r2, false);
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum5 > 0)//鞋子
				{
					stResourceLocation r2;
					r2.SetFileName("data\\items.gl");
					r2.group = 0;
					r2.frame = 15400+m_object.hongtiannum5;
					m_ToolTips.AddAnimation(&r2, false);
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum6 > 0)//戒指
				{
					stResourceLocation r2;
					r2.SetFileName("data\\items.gl");
					r2.group = 0;
					r2.frame = 15500+m_object.hongtiannum6;
					m_ToolTips.AddAnimation(&r2, false);
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum7 > 0)//腰带
				{
					stResourceLocation r2;
					r2.SetFileName("data\\items.gl");
					r2.group = 0;
					r2.frame = 15600+m_object.hongtiannum7;
					m_ToolTips.AddAnimation(&r2, false);
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum8 > 0)//衣服
				{
					stResourceLocation r2;
					r2.SetFileName("data\\items.gl");
					r2.group = 0;
					r2.frame = 15700+m_object.hongtiannum8;
					m_ToolTips.AddAnimation(&r2, false);
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum9 > 0)//副手
				{
					stResourceLocation r2;
					r2.SetFileName("data\\items.gl");
					r2.group = 0;
					r2.frame = 15800+m_object.hongtiannum9;
					m_ToolTips.AddAnimation(&r2, false);
					Tips_AddString( "\n" );
				}
				
				
				//轰天镶嵌效果
				if (m_object.hongtiannum1 > 0)
				{
					m_ToolTips.SetCurColor(COLOR_ARGB(255,255,69,0));
					Tips_AddString( "轰天镶嵌效果：" );
					Tips_AddNum("\n  涅槃·轰天石·攻击对手时，%u%%的几率使其变成猪头持续3秒",(m_object.hongtiannum1));
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum2 > 0)
				{	
					m_ToolTips.SetCurColor(COLOR_ARGB(255,255,69,0));
					Tips_AddString( "轰天镶嵌效果：" );
					Tips_AddNum("\n  涅槃·轰天印·攻击对手时，%u%%的几率使其每秒减少10%%的血量持续5秒",(m_object.hongtiannum2));
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum3 > 0)
				{	
					m_ToolTips.SetCurColor(COLOR_ARGB(255,255,69,0));
					Tips_AddString( "轰天镶嵌效果：" );
					Tips_AddNum("\n  涅槃·轰天盾·受到攻击时，%u%%的几率使自己无敌持续3秒",(m_object.hongtiannum3));
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum4 > 0)
				{	
					m_ToolTips.SetCurColor(COLOR_ARGB(255,255,69,0));
					Tips_AddString( "轰天镶嵌效果：" );
					Tips_AddNum("\n  涅槃·轰天缴·攻击对手时，%u%%的几率使其进入诅咒持续3秒",(m_object.hongtiannum4));
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum5 > 0)
				{	
					m_ToolTips.SetCurColor(COLOR_ARGB(255,255,69,0));
					Tips_AddString( "轰天镶嵌效果：" );
					Tips_AddNum("\n  涅槃·轰天轮·受到攻击时，%u%%的几率使自己闪避提升50%%持续5秒",(m_object.hongtiannum5));
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum6 > 0)
				{	
					m_ToolTips.SetCurColor(COLOR_ARGB(255,255,69,0));
					Tips_AddString( "轰天镶嵌效果：" );
					Tips_AddNum("\n  涅槃·轰天缨·攻击对手时，%u%%的几率使自己血量回复100%%",(m_object.hongtiannum6));
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum7 > 0)
				{	
					m_ToolTips.SetCurColor(COLOR_ARGB(255,255,69,0));
					Tips_AddString( "轰天镶嵌效果：" );
					Tips_AddNum("\n  涅槃·轰天镜·攻击对手时，%u%%的几率使自己进入神兵觉醒持续3秒",(m_object.hongtiannum7));
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum8 > 0)
				{
					m_ToolTips.SetCurColor(COLOR_ARGB(255,255,69,0));
					Tips_AddString( "轰天镶嵌效果：" );
					Tips_AddNum("\n  涅槃·轰天绫·受到攻击时，%u%%的几率使自己提升防御50%%持续3秒",(m_object.hongtiannum8));
					Tips_AddString( "\n" );
				}
				if (m_object.hongtiannum9 > 0)
				{
					
					m_ToolTips.SetCurColor(COLOR_ARGB(255,255,69,0));
					Tips_AddString( "轰天镶嵌效果：" );
					Tips_AddNum("\n  涅槃·轰天刃·攻击对手时，%u%%的几率使其进入眩晕持续3秒",(m_object.hongtiannum9));
					Tips_AddString( "\n" );
				}

				
				stResourceLocation rl;
				//武器类
				if(m_pObjectBase->dwType >= 104 && m_pObjectBase->dwType <= 111 || m_pObjectBase->dwType == 136 || m_pObjectBase->dwType ==137 ||m_pObjectBase->dwType ==155)
				{
					if(m_object.douzhansflv<=-1)
					{
						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 36;
						rl.frame = 0;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 36;
						rl.frame = 10;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 36;
						rl.frame = 20;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 36;
						rl.frame = 30;
						m_ToolTips.AddAnimation(&rl, false);
					}
					else{
						if(m_object.douzhansflv>0 && m_object.douzhansflv<=10)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 1;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 11;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 21;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 31;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>10 && m_object.douzhansflv<=20)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 2;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 12;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 22;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 32;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>20 && m_object.douzhansflv<=30)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 3;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 13;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 23;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 33;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>30 && m_object.douzhansflv<=40)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 4;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 14;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 24;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 34;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>40 && m_object.douzhansflv<=50)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 5;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 15;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 25;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 35;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>50 && m_object.douzhansflv<=60)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 6;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 16;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 26;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 36;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>60 && m_object.douzhansflv<=70)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 7;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 17;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 27;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 37;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>70 && m_object.douzhansflv<=80)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 8;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 18;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 28;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 38;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>80 && m_object.douzhansflv<=90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 36;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
					}
				}
				
				//副手 盾牌  ||这个符号是或   &&是并且
				if(m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147 || m_pObjectBase->dwType ==112|| m_pObjectBase->dwType ==138 || m_pObjectBase->dwType ==156)
				{
					if(m_object.douzhansflv<=-1)
					{
						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 182;
						rl.frame = 0;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 182;
						rl.frame = 10;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 182;
						rl.frame = 20;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 182;
						rl.frame = 30;
						m_ToolTips.AddAnimation(&rl, false);
					}
					else{
						if(m_object.douzhansflv>0 && m_object.douzhansflv<=10)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 1;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 11;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 21;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 31;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>10 && m_object.douzhansflv<=20)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 2;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 12;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 22;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 32;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>20 && m_object.douzhansflv<=30)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 3;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 13;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 23;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 33;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>30 && m_object.douzhansflv<=40)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 4;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 14;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 24;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 34;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>40 && m_object.douzhansflv<=50)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 5;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 15;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 25;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 35;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>50 && m_object.douzhansflv<=60)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 6;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 16;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 26;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 36;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>60 && m_object.douzhansflv<=70)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 7;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 17;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 27;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 37;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>70 && m_object.douzhansflv<=80)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 8;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 18;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 28;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 38;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>80 && m_object.douzhansflv<=90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 182;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
					}
				}

				//护腕
				if(m_pObjectBase->dwType == 115)
				{
					if(m_object.douzhansflv<=-1)
					{
						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 27;
						rl.frame = 0;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 27;
						rl.frame = 10;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 27;
						rl.frame = 20;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 27;
						rl.frame = 30;
						m_ToolTips.AddAnimation(&rl, false);
					}
					else{
						if(m_object.douzhansflv>0 && m_object.douzhansflv<=10)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 1;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 11;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 21;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 31;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>10 && m_object.douzhansflv<=20)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 2;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 12;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 22;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 32;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>20 && m_object.douzhansflv<=30)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 3;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 13;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 23;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 33;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>30 && m_object.douzhansflv<=40)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 4;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 14;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 24;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 34;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>40 && m_object.douzhansflv<=50)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 5;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 15;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 25;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 35;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>50 && m_object.douzhansflv<=60)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 6;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 16;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 26;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 36;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>60 && m_object.douzhansflv<=70)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 7;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 17;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 27;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 37;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>70 && m_object.douzhansflv<=80)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 8;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 18;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 28;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 38;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>80 && m_object.douzhansflv<=90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 27;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
					}
				}

				//鞋子
				if(m_pObjectBase->dwType == 116)
				{
					if(m_object.douzhansflv<=-1)
					{
						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 39;
						rl.frame = 0;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 39;
						rl.frame = 10;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 39;
						rl.frame = 20;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 39;
						rl.frame = 30;
						m_ToolTips.AddAnimation(&rl, false);
					}
					else{
						if(m_object.douzhansflv>0 && m_object.douzhansflv<=10)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 1;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 11;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 21;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 31;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>10 && m_object.douzhansflv<=20)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 2;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 12;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 22;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 32;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>20 && m_object.douzhansflv<=30)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 3;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 13;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 23;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 33;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>30 && m_object.douzhansflv<=40)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 4;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 14;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 24;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 34;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>40 && m_object.douzhansflv<=50)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 5;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 15;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 25;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 35;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>50 && m_object.douzhansflv<=60)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 6;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 16;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 26;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 36;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>60 && m_object.douzhansflv<=70)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 7;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 17;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 27;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 37;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>70 && m_object.douzhansflv<=80)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 8;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 18;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 28;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 38;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>80 && m_object.douzhansflv<=90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 39;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
					}
				}

				//头盔
				if(m_pObjectBase->dwType == 113)
				{
					if(m_object.douzhansflv<=-1)
					{
						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 31;
						rl.frame = 0;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 31;
						rl.frame = 10;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 31;
						rl.frame = 20;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 31;
						rl.frame = 30;
						m_ToolTips.AddAnimation(&rl, false);
					}
					else{
						if(m_object.douzhansflv>0 && m_object.douzhansflv<=10)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 1;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 11;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 21;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 31;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>10 && m_object.douzhansflv<=20)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 2;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 12;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 22;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 32;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>20 && m_object.douzhansflv<=30)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 3;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 13;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 23;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 33;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>30 && m_object.douzhansflv<=40)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 4;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 14;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 24;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 34;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>40 && m_object.douzhansflv<=50)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 5;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 15;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 25;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 35;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>50 && m_object.douzhansflv<=60)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 6;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 16;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 26;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 36;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>60 && m_object.douzhansflv<=70)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 7;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 17;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 27;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 37;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>70 && m_object.douzhansflv<=80)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 8;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 18;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 28;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 38;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>80 && m_object.douzhansflv<=90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 31;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
					}
				}
				

				//项链
				if(m_pObjectBase->dwType == 117)
				{
					if(m_object.douzhansflv<=-1)
					{
						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 38;
						rl.frame = 0;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 38;
						rl.frame = 10;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 38;
						rl.frame = 20;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 38;
						rl.frame = 30;
						m_ToolTips.AddAnimation(&rl, false);
					}
					else{
						if(m_object.douzhansflv>0 && m_object.douzhansflv<=10)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 1;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 11;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 21;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 31;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>10 && m_object.douzhansflv<=20)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 2;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 12;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 22;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 32;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>20 && m_object.douzhansflv<=30)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 3;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 13;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 23;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 33;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>30 && m_object.douzhansflv<=40)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 4;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 14;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 24;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 34;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>40 && m_object.douzhansflv<=50)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 5;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 15;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 25;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 35;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>50 && m_object.douzhansflv<=60)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 6;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 16;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 26;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 36;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>60 && m_object.douzhansflv<=70)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 7;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 17;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 27;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 37;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>70 && m_object.douzhansflv<=80)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 8;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 18;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 28;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 38;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>80 && m_object.douzhansflv<=90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 38;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
					}
				}

				//衣服
				if(m_pObjectBase->dwType >=101 && m_pObjectBase->dwType <=103)
				{
					if(m_object.douzhansflv<=-1)
					{
						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 41;
						rl.frame = 0;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 41;
						rl.frame = 10;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 41;
						rl.frame = 20;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 41;
						rl.frame = 30;
						m_ToolTips.AddAnimation(&rl, false);
					}
					else{
						if(m_object.douzhansflv>0 && m_object.douzhansflv<=10)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 1;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 11;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 21;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 31;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>10 && m_object.douzhansflv<=20)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 2;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 12;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 22;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 32;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>20 && m_object.douzhansflv<=30)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 3;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 13;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 23;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 33;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>30 && m_object.douzhansflv<=40)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 4;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 14;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 24;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 34;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>40 && m_object.douzhansflv<=50)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 5;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 15;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 25;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 35;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>50 && m_object.douzhansflv<=60)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 6;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 16;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 26;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 36;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>60 && m_object.douzhansflv<=70)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 7;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 17;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 27;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 37;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>70 && m_object.douzhansflv<=80)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 8;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 18;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 28;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 38;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>80 && m_object.douzhansflv<=90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 41;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
					}
				}

				//腰带
				if(m_pObjectBase->dwType ==114)
				{
					if(m_object.douzhansflv<=-1)
					{
						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 45;
						rl.frame = 0;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 45;
						rl.frame = 10;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 45;
						rl.frame = 20;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 45;
						rl.frame = 30;
						m_ToolTips.AddAnimation(&rl, false);
					}
					else{
						if(m_object.douzhansflv>0 && m_object.douzhansflv<=10)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 1;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 11;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 21;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 31;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>10 && m_object.douzhansflv<=20)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 2;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 12;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 22;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 32;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>20 && m_object.douzhansflv<=30)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 3;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 13;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 23;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 33;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>30 && m_object.douzhansflv<=40)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 4;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 14;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 24;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 34;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>40 && m_object.douzhansflv<=50)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 5;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 15;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 25;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 35;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>50 && m_object.douzhansflv<=60)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 6;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 16;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 26;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 36;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>60 && m_object.douzhansflv<=70)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 7;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 17;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 27;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 37;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>70 && m_object.douzhansflv<=80)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 8;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 18;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 28;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 38;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>80 && m_object.douzhansflv<=90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 45;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
					}
				}

				//腰带
				if(m_pObjectBase->dwType ==118)
				{
					if(m_object.douzhansflv<=-1)
					{
						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 25;
						rl.frame = 0;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 25;
						rl.frame = 10;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 25;
						rl.frame = 20;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 25;
						rl.frame = 30;
						m_ToolTips.AddAnimation(&rl, false);
					}
					else{
						if(m_object.douzhansflv>0 && m_object.douzhansflv<=10)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 1;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 11;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 21;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 31;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>10 && m_object.douzhansflv<=20)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 2;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 12;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 22;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 32;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>20 && m_object.douzhansflv<=30)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 3;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 13;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 23;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 33;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>30 && m_object.douzhansflv<=40)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 4;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 14;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 24;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 34;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>40 && m_object.douzhansflv<=50)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 5;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 15;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 25;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 35;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>50 && m_object.douzhansflv<=60)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 6;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 16;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 26;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 36;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>60 && m_object.douzhansflv<=70)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 7;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 17;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 27;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 37;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>70 && m_object.douzhansflv<=80)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 8;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 18;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 28;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 38;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>80 && m_object.douzhansflv<=90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 25;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
					}
				}

				
				//马蹄铁马脚字纹
				if(m_pObjectBase->dwType ==130)
				{
					if(m_object.douzhansflv<=-1)
					{
						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 79;
						rl.frame = 0;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 79;
						rl.frame = 10;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 79;
						rl.frame = 20;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 79;
						rl.frame = 30;
						m_ToolTips.AddAnimation(&rl, false);
					}
					else{
						if(m_object.douzhansflv>0 && m_object.douzhansflv<=10)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 1;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 11;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 21;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 31;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>10 && m_object.douzhansflv<=20)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 2;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 12;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 22;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 32;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>20 && m_object.douzhansflv<=30)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 3;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 13;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 23;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 33;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>30 && m_object.douzhansflv<=40)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 4;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 14;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 24;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 34;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>40 && m_object.douzhansflv<=50)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 5;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 15;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 25;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 35;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>50 && m_object.douzhansflv<=60)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 6;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 16;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 26;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 36;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>60 && m_object.douzhansflv<=70)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 7;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 17;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 27;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 37;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>70 && m_object.douzhansflv<=80)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 8;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 18;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 28;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 38;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>80 && m_object.douzhansflv<=90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 79;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
					}
				}
				//马缰绳字纹
				if(m_pObjectBase->dwType ==131)
				{
					if(m_object.douzhansflv<=-1)
					{
						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 77;
						rl.frame = 0;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 77;
						rl.frame = 10;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 77;
						rl.frame = 20;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 77;
						rl.frame = 30;
						m_ToolTips.AddAnimation(&rl, false);
					}
					else{
						if(m_object.douzhansflv>0 && m_object.douzhansflv<=10)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 1;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 11;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 21;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 31;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>10 && m_object.douzhansflv<=20)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 2;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 12;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 22;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 32;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>20 && m_object.douzhansflv<=30)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 3;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 13;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 23;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 33;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>30 && m_object.douzhansflv<=40)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 4;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 14;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 24;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 34;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>40 && m_object.douzhansflv<=50)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 5;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 15;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 25;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 35;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>50 && m_object.douzhansflv<=60)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 6;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 16;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 26;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 36;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>60 && m_object.douzhansflv<=70)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 7;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 17;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 27;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 37;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>70 && m_object.douzhansflv<=80)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 8;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 18;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 28;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 38;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>80 && m_object.douzhansflv<=90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 77;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
					}
				}
				//马鞍字纹
				if(m_pObjectBase->dwType ==132)
				{
					if(m_object.douzhansflv<=-1)
					{
						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 71;
						rl.frame = 0;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 71;
						rl.frame = 10;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 71;
						rl.frame = 20;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 71;
						rl.frame = 30;
						m_ToolTips.AddAnimation(&rl, false);
					}
					else{
						if(m_object.douzhansflv>0 && m_object.douzhansflv<=10)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 1;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 11;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 21;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 31;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>10 && m_object.douzhansflv<=20)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 2;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 12;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 22;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 32;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>20 && m_object.douzhansflv<=30)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 3;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 13;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 23;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 33;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>30 && m_object.douzhansflv<=40)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 4;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 14;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 24;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 34;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>40 && m_object.douzhansflv<=50)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 5;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 15;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 25;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 35;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>50 && m_object.douzhansflv<=60)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 6;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 16;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 26;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 36;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>60 && m_object.douzhansflv<=70)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 7;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 17;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 27;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 37;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>70 && m_object.douzhansflv<=80)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 8;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 18;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 28;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 38;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>80 && m_object.douzhansflv<=90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 71;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
					}
				}
				//马甲字纹
				if(m_pObjectBase->dwType ==133)
				{
					if(m_object.douzhansflv<=-1)
					{
						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 69;
						rl.frame = 0;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 69;
						rl.frame = 10;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 69;
						rl.frame = 20;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 69;
						rl.frame = 30;
						m_ToolTips.AddAnimation(&rl, false);
					}
					else{
						if(m_object.douzhansflv>0 && m_object.douzhansflv<=10)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 1;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 11;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 21;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 31;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>10 && m_object.douzhansflv<=20)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 2;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 12;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 22;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 32;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>20 && m_object.douzhansflv<=30)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 3;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 13;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 23;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 33;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>30 && m_object.douzhansflv<=40)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 4;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 14;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 24;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 34;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>40 && m_object.douzhansflv<=50)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 5;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 15;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 25;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 35;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>50 && m_object.douzhansflv<=60)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 6;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 16;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 26;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 36;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>60 && m_object.douzhansflv<=70)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 7;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 17;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 27;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 37;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>70 && m_object.douzhansflv<=80)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 8;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 18;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 28;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 38;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>80 && m_object.douzhansflv<=90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 69;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
					}
				}
				//马蹬字纹
				if(m_pObjectBase->dwType ==134)
				{
					if(m_object.douzhansflv<=-1)
					{
						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 64;
						rl.frame = 0;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 64;
						rl.frame = 10;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 64;
						rl.frame = 20;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 64;
						rl.frame = 30;
						m_ToolTips.AddAnimation(&rl, false);
					}
					else{
						if(m_object.douzhansflv>0 && m_object.douzhansflv<=10)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 1;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 11;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 21;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 31;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>10 && m_object.douzhansflv<=20)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 2;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 12;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 22;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 32;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>20 && m_object.douzhansflv<=30)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 3;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 13;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 23;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 33;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>30 && m_object.douzhansflv<=40)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 4;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 14;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 24;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 34;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>40 && m_object.douzhansflv<=50)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 5;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 15;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 25;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 35;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>50 && m_object.douzhansflv<=60)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 6;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 16;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 26;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 36;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>60 && m_object.douzhansflv<=70)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 7;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 17;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 27;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 37;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>70 && m_object.douzhansflv<=80)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 8;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 18;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 28;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 38;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>80 && m_object.douzhansflv<=90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 64;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
					}
				}
				//马牌字纹马铃铛字纹
				if(m_pObjectBase->dwType ==140)
				{
					if(m_object.douzhansflv<=-1)
					{
						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 61;
						rl.frame = 0;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 61;
						rl.frame = 10;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 61;
						rl.frame = 20;
						m_ToolTips.AddAnimation(&rl, false);
						

						rl.SetFileName("data\\interfaces4.gl");
						rl.group = 61;
						rl.frame = 30;
						m_ToolTips.AddAnimation(&rl, false);
					}
					else{
						if(m_object.douzhansflv>0 && m_object.douzhansflv<=10)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 1;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 11;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 21;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 31;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>10 && m_object.douzhansflv<=20)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 2;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 12;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 22;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 32;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>20 && m_object.douzhansflv<=30)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 3;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 13;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 23;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 33;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>30 && m_object.douzhansflv<=40)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 4;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 14;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 24;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 34;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>40 && m_object.douzhansflv<=50)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 5;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 15;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 25;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 35;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>50 && m_object.douzhansflv<=60)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 6;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 16;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 26;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 36;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>60 && m_object.douzhansflv<=70)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 7;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 17;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 27;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 37;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>70 && m_object.douzhansflv<=80)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 8;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 18;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 28;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 38;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>80 && m_object.douzhansflv<=90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
						else if(m_object.douzhansflv>90)
						{
							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 9;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 19;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 29;
							m_ToolTips.AddAnimation(&rl, false);
							

							rl.SetFileName("data\\interfaces4.gl");
							rl.group = 61;
							rl.frame = 39;
							m_ToolTips.AddAnimation(&rl, false);
						}
					}
				}
				if (m_object.m16axhp > 0)
				{
					m_ToolTips.SetCurColor(COLOR_ARGB(255,193,7,253));
					Tips_AddNum("\n字纹宝石生命 + %u",(m_object.m16axhp));
				}
				if (m_object.m16axpdamage > 0)
				{
					m_ToolTips.SetCurColor(COLOR_ARGB(255,193,7,253));
					Tips_AddNum("\n字纹宝石物攻 + %u",(m_object.m16axpdamage));
				}
				if (m_object.m16axmdamage > 0)
				{
					m_ToolTips.SetCurColor(COLOR_ARGB(255,193,7,253));
					Tips_AddNum("\n字纹宝石魔攻 + %u",(m_object.m16axmdamage));
				}
				if (m_object.p16defence > 0)
				{
					m_ToolTips.SetCurColor(COLOR_ARGB(255,193,7,253));
					Tips_AddNum("\n字纹宝石物防 + %u",(m_object.p16defence));
				}
				if (m_object.m16defence > 0)
				{
					m_ToolTips.SetCurColor(COLOR_ARGB(255,193,7,253));
					Tips_AddNum("\n字纹宝石魔防 + %u",(m_object.m16defence));
				}
			}
			
		}
	}
	//sky 装备补天石特效
	if (m_object.butian == 1)
	{
		Tips_AddString("\n"); //补天石
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 4;
		rl.frame = 202;
		m_ToolTips.AddAnimation(&rl, false);
	}
	else if (m_object.butian == 2)
	{
		Tips_AddString("\n"); //补天灵石
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 4;
		rl.frame = 201;
		m_ToolTips.AddAnimation(&rl, false);
	}
	else if (m_object.butian == 3) //补天神石
	{
	    Tips_AddString( "\n" );
		stResourceLocation rl;
		rl.SetFileName( "data\\interfaces.gl");
		rl.group = 4;
		rl.frame = 200;
		m_ToolTips.AddAnimation( &rl, false );
	 }
	else if (m_object.butian == 4) //帝王神石
	{
		Tips_AddString( "\n" );
		stResourceLocation rl;
		rl.SetFileName( "data\\interfaces.gl");
		rl.group = 4;
		rl.frame = 203;
		m_ToolTips.AddAnimation( &rl, false );
	}
	else if (m_object.butian == 5) //弑天圣石
	{
		Tips_AddString( "\n" );
		stResourceLocation rl;
		rl.SetFileName( "data\\interfaces.gl");
		rl.group = 4;
		rl.frame = 204;
		m_ToolTips.AddAnimation( &rl, false );
	}
	//bright moon 神龙镶嵌特效//////////////////////////
	if (m_object.longwanglv == 1)
	{
		Tips_AddString("\n"); //小龙镶嵌特效
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 150;
		rl.frame = 390;
		m_ToolTips.AddAnimation(&rl, false);
	}
	else if (m_object.longwanglv == 2)
	{
		Tips_AddString("\n"); //金龙镶嵌特效
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 150;
		rl.frame = 391;
		m_ToolTips.AddAnimation(&rl, false);
	}
	else if (m_object.longwanglv == 3) //苍龙镶嵌特效
	{
	    Tips_AddString( "\n" );
		stResourceLocation rl;
		rl.SetFileName( "data\\interfaces.gl");
		rl.group = 150;
		rl.frame = 392;
		m_ToolTips.AddAnimation( &rl, false );
	 }
	else if (m_object.longwanglv == 4) //巨龙镶嵌特效
	{
		Tips_AddString( "\n" );
		stResourceLocation rl;
		rl.SetFileName( "data\\interfaces.gl");
		rl.group = 150;
		rl.frame = 393;
		m_ToolTips.AddAnimation( &rl, false );
	}
	else if (m_object.longwanglv == 5) //神苍龙镶嵌特效
	{
		Tips_AddString( "\n" );
		stResourceLocation rl;
		rl.SetFileName( "data\\interfaces.gl");
		rl.group = 150;
		rl.frame = 500;
		m_ToolTips.AddAnimation( &rl, false );
	}
    else if (m_object.longwanglv == 6) //神巨龙镶嵌特效
	{
		Tips_AddString( "\n" );
		stResourceLocation rl;
		rl.SetFileName( "data\\interfaces.gl");
		rl.group = 150;
		rl.frame = 501;
		m_ToolTips.AddAnimation( &rl, false );
	}		
	//神龙镶嵌属性展示
	if (m_object.longwanglv == 6) //神巨龙宝宝
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,249,114,5));
		Tips_AddNum("\n神巨龙降临增加物理攻击力 %u",m_object.p14damage);
		Tips_AddNum("\n神巨龙降临增加魔法攻击力 %u",m_object.m14damage);
		Tips_AddNum("\n神巨龙降临增加物理防御力 %u",m_object.p14defence);
		Tips_AddNum("\n神巨龙降临增加魔法防御力 %u",m_object.m14defence);
		Tips_AddNum("\n神巨龙降临增加最大生命值 %u",m_object.m14axhp);
	}  
    else if (m_object.longwanglv == 5) //神苍龙宝宝
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,255));
		Tips_AddNum("\n神巨苍龙降临增加物理攻击力 %u",m_object.p14damage);
		Tips_AddNum("\n神巨苍龙降临增加魔法攻击力 %u",m_object.m14damage);
		Tips_AddNum("\n神巨苍龙降临增加物理防御力 %u",m_object.p14defence);
		Tips_AddNum("\n神巨苍龙降临增加魔法防御力 %u",m_object.m14defence);
		Tips_AddNum("\n神巨苍龙降临增加最大生命值 %u",m_object.m14axhp);
	}
	else if (m_object.longwanglv == 4) ///巨龙宝宝
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum("\n巨龙降临增加物理攻击力 %u",m_object.p14damage);
		Tips_AddNum("\n巨龙降临增加魔法攻击力 %u",m_object.m14damage);
		Tips_AddNum("\n巨龙降临增加物理防御力 %u",m_object.p14defence);
		Tips_AddNum("\n巨龙降临增加魔法防御力 %u",m_object.m14defence);
		Tips_AddNum("\n巨龙降临增加最大生命值 %u",m_object.m14axhp);
	}
	else if (m_object.longwanglv == 3) //苍龙宝宝
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
		Tips_AddNum("\n苍龙降临增加物理攻击力 %u",m_object.p14damage);
		Tips_AddNum("\n苍龙降临增加魔法攻击力 %u",m_object.m14damage);
		Tips_AddNum("\n苍龙降临增加物理防御力 %u",m_object.p14defence);
		Tips_AddNum("\n苍龙降临增加魔法防御力 %u",m_object.m14defence);
		Tips_AddNum("\n苍龙降临增加最大生命值 %u",m_object.m14axhp);
	}
	else if (m_object.longwanglv == 2) //金龙宝宝
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,5,118,247));
		Tips_AddNum("\n金龙降临增加物理攻击力 %u",m_object.p14damage);
		Tips_AddNum("\n金龙降临增加魔法攻击力 %u",m_object.m14damage);
		Tips_AddNum("\n金龙降临增加物理防御力 %u",m_object.p14defence);
		Tips_AddNum("\n金龙降临增加魔法防御力 %u",m_object.m14defence);
		Tips_AddNum("\n金龙降临增加最大生命值 %u",m_object.m14axhp);
	}
	else if (m_object.longwanglv == 1) //小龙宝宝
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		Tips_AddNum("\n小龙降临增加物理攻击力 %u",m_object.p14damage);
		Tips_AddNum("\n小龙降临增加魔法攻击力 %u",m_object.m14damage);
		Tips_AddNum("\n小龙降临增加物理防御力 %u",m_object.p14defence);
		Tips_AddNum("\n小龙降临增加魔法防御力 %u",m_object.m14defence);
		Tips_AddNum("\n小龙降临增加最大生命值 %u",m_object.m14axhp);
	}
	//补天镶嵌展示
    if (m_object.butian == 5) //弑天圣石
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,100,0));
		Tips_AddNum("\n补天圣石增加物理攻击力 %u",m_object.p6damage);
		Tips_AddNum("\n补天圣石增加魔法攻击力 %u",m_object.m6damage);
		Tips_AddNum("\n补天圣石增加物理防御力 %u",m_object.p6defence);
		Tips_AddNum("\n补天圣石增加魔法防御力 %u",m_object.m6defence);
		Tips_AddNum("\n补天圣石增加最大生命值 %u",m_object.m6axhp);
	}
	else if (m_object.butian == 4) //帝王神石
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,100,0));
		Tips_AddNum("\n补天仙石增加物理攻击力 %u",m_object.p6damage);
		Tips_AddNum("\n补天仙石增加魔法攻击力 %u",m_object.m6damage);
		Tips_AddNum("\n补天仙石增加物理防御力 %u",m_object.p6defence);
		Tips_AddNum("\n补天仙石增加魔法防御力 %u",m_object.m6defence);
		Tips_AddNum("\n补天仙石增加最大生命值 %u",m_object.m6axhp);
	}
	else if (m_object.butian == 3) //补天神石
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,255));
		Tips_AddNum("\n补天神石增加物理攻击力 %u",m_object.p6damage);
		Tips_AddNum("\n补天神石增加魔法攻击力 %u",m_object.m6damage);
		Tips_AddNum("\n补天神石增加物理防御力 %u",m_object.p6defence);
		Tips_AddNum("\n补天神石增加魔法防御力 %u",m_object.m6defence);
		Tips_AddNum("\n补天神石增加最大生命值 %u",m_object.m6axhp);
	}
	else if (m_object.butian == 2) //补天灵石
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
		Tips_AddNum("\n补天灵石增加物理攻击力 %u",m_object.p6damage);
		Tips_AddNum("\n补天灵石增加魔法攻击力 %u",m_object.m6damage);
		Tips_AddNum("\n补天灵石增加物理防御力 %u",m_object.p6defence);
		Tips_AddNum("\n补天灵石增加魔法防御力 %u",m_object.m6defence);
		Tips_AddNum("\n补天灵石增加最大生命值 %u",m_object.m6axhp);
	}
	else if (m_object.butian == 1) //补天石
	{
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,0));
		Tips_AddNum("\n补天石增加物理攻击力 %u",m_object.p6damage);
		Tips_AddNum("\n补天石增加魔法攻击力 %u",m_object.m6damage);
		Tips_AddNum("\n补天石增加物理防御力 %u",m_object.p6defence);
		Tips_AddNum("\n补天石增加魔法防御力 %u",m_object.m6defence);
		Tips_AddNum("\n补天石增加最大生命值 %u",m_object.m6axhp);
	}
	//暗影宝石加成展示
    if (m_object.aystonelv > 0) 
	{		
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
		Tips_AddNum("\n暗影转生增加物理攻击力 %u",m_object.p17damage);
		Tips_AddNum("\n暗影转生增加魔法攻击力 %u",m_object.m17damage);
		Tips_AddNum("\n暗影转生增加物理防御力 %u",m_object.p17defence);
		Tips_AddNum("\n暗影转生增加魔法防御力 %u",m_object.m17defence);
		Tips_AddNum("\n暗影转生增加最大生命值 %u",m_object.m17axhp);
	}
  


	//Can used for skill tips
	m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
	if ( (m_pObjectBase->dwType >=104) && (m_pObjectBase->dwType<=107) )
		Tips_AddString( "\n适用于武术系技能" );
	if ( m_pObjectBase->dwType == 108 )
		Tips_AddString( "\n适用于法术系技能" );
	if ( m_pObjectBase->dwType == 109 )
		Tips_AddString( "\n适用于射术系技能" );
	if ( m_pObjectBase->dwType == 110 )
		Tips_AddString( "\n适用于仙术系技能" );

		// 2010 8
		//if ( m_pObjectBase->dwType == 111 )
		//	Tips_AddString( "\n适用于召唤系技能" );
	if ( m_pObjectBase->dwType == 111 )
		Tips_AddString( "\n适用于召唤系技能" );
	if ( m_pObjectBase->dwType == 136 )
		Tips_AddString( "\n适用于刺客系技能" );
	if ( m_pObjectBase->dwType == 137 )
		Tips_AddString( "\n适用于卫士系技能" );
	if ( m_pObjectBase->dwType == 155 )
		Tips_AddString( "\n适用于枪系技能" );


		//Show item description
		if ( ( strcmp( m_pObjectBase->strDesc, "无" ) != 0 )
			&& ( strcmp( m_pObjectBase->strDesc, "" ) != 0 ) )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,255) );
			Tips_AddText( "\n%s", m_pObjectBase->strDesc );
		}

    //soke 增加紫色装备说明
    if ( (m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118) || (m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 134) 
		|| (m_pObjectBase->dwType >= 136 && m_pObjectBase->dwType <= 138)|| (m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147)
		|| m_pObjectBase->dwType == 155 || m_pObjectBase->dwType == 156)
	{
	   if ( m_object.kind & 16 )
	   {
	      m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
		  Tips_AddString( "\n紫色装备可以用无暇升级宝石进行升星" );
	   }
	   else if ( m_object.kind & 4 )
	   {
	      m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
		  Tips_AddString( "\n该绿色装备可以用精致升级宝石进行升星" );
	   }
	}
	
	if ( m_dwTipFlags & ITEMTIPS_FLAG_RIGHT_CLICK_USE )
	{
		//Add tips
		if ( (m_pObjectBase->dwType >= 101 && m_pObjectBase->dwType <= 118)|| (m_pObjectBase->dwType >= 136 && m_pObjectBase->dwType <= 138)
			|| (m_pObjectBase->dwType >= 141 && m_pObjectBase->dwType <= 147) || m_pObjectBase->dwType == 155 || m_pObjectBase->dwType == 156 )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,50) );
			Tips_AddString( "\n出售该物品获得银子存入钱包中" );
		}

		m_ToolTips.SetCurColor( COLOR_ARGB(255,121,100,36) );
		if ((m_pObjectBase->dwType == 30 )
			|| (m_pObjectBase->dwType == 52 )
			|| (m_pObjectBase->dwType == 53 )
			|| (m_pObjectBase->dwType == 119 )
			|| (m_pObjectBase->dwType == ItemType_Amulet ) 
			|| (m_pObjectBase->dwType == 62 )
			|| (m_pObjectBase->dwType == 120 ) )
		{
			Tips_AddString( "\n右键点击可以装备" );
		}
		m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
		if ((m_pObjectBase->dwType == 57 ))
		{
			Tips_AddString( "\n可在V技能页面天赋技能中使用" );
		}		
		else if( (m_pObjectBase->dwType == 17)
			|| (m_pObjectBase->dwType == 19 )
			|| (m_pObjectBase->dwType == 20 )			
			|| (m_pObjectBase->dwType == 25 )
			|| (m_pObjectBase->dwType == 26 )			
			|| (m_pObjectBase->dwType == 33 )
			|| (m_pObjectBase->dwType == 42 )
			|| (m_pObjectBase->dwType == 54 )
			|| (m_pObjectBase->dwType == 55 )
			|| (m_pObjectBase->dwType == 51 )			
			|| (m_pObjectBase->dwType == 58 )
			|| (m_pObjectBase->dwType == 59 )
			|| (m_pObjectBase->dwType == 60 )
			|| (m_pObjectBase->dwType == 63 )
			|| (m_pObjectBase->dwType == 46 )
			|| (m_pObjectBase->dwID == 756) || (m_pObjectBase->dwID == 757) )		//|| (m_pObjectBase->dwType == 41 )
		{
			Tips_AddString( "\n右键点击可以使用" );
		}
		else if( m_pObjectBase->dwType == 23 )
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,121,100,36) );
			Tips_AddString( "\n可装备至包裹栏" );
		}
		else if( (m_pObjectBase->dwType == 41))//细分 task3605
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,121,100,36) );
			Tips_AddString( "\n可装备至功能栏" );
		}
		/*else if (m_pObjectBase->dwType == 23 )
		{
			if (m_object.pos.dwLocation!=OBJECTCELLTYPE_EQUIP)
			{
			Tips_AddString( "\n（右键点击可以装备）" );
			}
		}
		*/
		else if( (m_pObjectBase->dwType == 31 ))//细分 task3605
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,121,100,36) );
			Tips_AddString( "\n（右键点击可以打开）" );
		}
		//开始 ---------------------------------
		/*
		else if( (m_pObjectBase->dwType == 73 ))//桃子 增加特定道具类型显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,50,255) );
			Tips_AddString( "\n右键点击打开兑换领取！" );
		}	
		*/
	    //结束 --------------------------------- 		
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 795 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于绿色装备升星，请配合月光宝盒使用！" );
		}	
	    //结束 --------------------------------- 			
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 799 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,255) );
			Tips_AddString( "\n可用于紫色装备升星和护身符升级，请配合月光宝盒使用！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 1105 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,255,0) );
			Tips_AddString( "\n可瞬间恢复生命值的灵药！" );
		}	
	    //结束 --------------------------------- 
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 1106 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可瞬间恢复生命值的仙药！" );
		}	
	    //结束 --------------------------------- 
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 1107 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,255) );
			Tips_AddString( "\n可瞬间恢复生命值的圣药！" );
		}	
	    //结束 --------------------------------- 
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 52000)
			|| (m_pObjectBase->dwID == 52001 )
			|| (m_pObjectBase->dwID == 52002 )			
			|| (m_pObjectBase->dwID == 52003) || (m_pObjectBase->dwID == 52004) )//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,255) );
			Tips_AddString( "\n温馨提示：补天石请依次镶嵌，补天石→补天灵石→补天神石→补天仙石！" );
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可使用月光宝盒合成高等级补天石！" );			
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
			else if( (m_pObjectBase->dwType == 78 ))//桃子 增加特定道具类型显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,0) );
			Tips_AddString( "\n温馨提示：需要合成的魂魄石请勿右键打开！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 1410)
			|| (m_pObjectBase->dwID == 1411) )//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
			Tips_AddString( "\n可用于进阶时装等级系统！" );
		}	
	    //结束 ---------------------------------				
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 3838)
			|| (m_pObjectBase->dwID == 3839 )
			|| (m_pObjectBase->dwID == 3840 )			
			|| (m_pObjectBase->dwID == 3841) || (m_pObjectBase->dwID == 3842) )//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,255) );
			Tips_AddString( "\n可用于神兽镶嵌系统！" );
		}	
	    //结束 --------------------------------- 		
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID >= 3843 && m_pObjectBase->dwID <= 3844 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,255) );
			Tips_AddString( "\n可用于装备精炼系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 3850 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,255) );
			Tips_AddString( "\n可用于璀璨镶嵌系统！" );
		}	
	    //结束 --------------------------------- 
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID >= 2960 && m_pObjectBase->dwID <= 2974 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,255,0,255) );
			Tips_AddString( "\n可用于月光宝盒升星系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID >= 4101 && m_pObjectBase->dwID <= 4130 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可使用时装布料进阶时装等级！" );
		}	
	    //结束 ---------------------------------		
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 1317 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于装备祝福系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 1100 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n右键点击心爱之人头像可赠送鲜花！" );
		}	
	    //结束 ---------------------------------	
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 15600 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可对马牌进行精炼强化！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID >= 5301 && m_pObjectBase->dwID <= 5307 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于神龙镶嵌系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 5312 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于逆天进阶系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 5313 || m_pObjectBase->dwID == 5317 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于字纹镶嵌系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 5314 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于暗影镶嵌系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 5315 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于尚方宝剑升级系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 5316 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于紫金龙袍升级系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 3980 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于怪物图鉴强化系统！" );
		}	
	    //结束 ---------------------------------																	
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 3905 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于翅膀升级系统！" );
		}	
	    //结束 --------------------------------- 
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 3924 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于神兵升级系统！" );
		}	
	    //结束 --------------------------------- 
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 3936 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于神兽升级系统！" );
		}	
	    //结束 --------------------------------- 
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID >= 3832 && m_pObjectBase->dwID <= 3837 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于勋章升级系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID >= 3866 && m_pObjectBase->dwID <= 3869 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于寒冰升级系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 1140 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于龙星激活系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 1141 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于龙星解封系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 1153 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于龙星升级系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 3812 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于神器强化系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID >= 3809 && m_pObjectBase->dwID <= 3810 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于斗魂升级系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 2329 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于魂魄清除系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 5310 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于装备栏激活系统！" );
		}	
	    //结束 ---------------------------------
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID == 5311 ))//桃子 增加特定道具ID显示额外颜色字体
		{
			m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			Tips_AddString( "\n可用于装备栏升级系统！" );
		}	
	    //结束 ---------------------------------			
		//开始 ---------------------------------
		else if( (m_pObjectBase->dwID >= 120001 &&  (m_pObjectBase->dwID <= 120024 ) ))//桃子 增加特定道具ID显示额外颜色字体
		{
			if(m_pObjectBase->dwID<120013)
			{
				int num=(m_pObjectBase->dwID%120000);
				Tips_AddString("\n"); 
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces2.gl");
				rl.group = 26;
				rl.frame = 200+num;
				m_ToolTips.AddAnimation(&rl, false);
				m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );

			}
			else{
				int num=(m_pObjectBase->dwID%120000)-12;
				Tips_AddString("\n"); 
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces2.gl");
				rl.group = 27;
				rl.frame = 200+num;
				m_ToolTips.AddAnimation(&rl, false);
				m_ToolTips.SetCurColor( COLOR_ARGB(255,0,255,0) );
			}
			
			Tips_AddString( "\n可用于称号系统激活指定称号！" );
		}	
	    //结束 ---------------------------------
												 						 				 								 	
	}

	//Add set maker name
	if ( IsBelongToMakerSet( m_object ) && !(m_pObjectBase->dwType >= 130 && m_pObjectBase->dwType <= 134 )) //soke 马匹装备不显示打造的马装
	{
		m_ToolTips.SetCurColor( COLOR_ARGB(255,120,240,30) );
		Tips_AddText( "\n%s 打造的套装", m_object.maker );
	}

	m_ToolTips.Resize();

	FUNCTION_END;
}


void  ActiveItem(CRoleItem* pRoleItem, bool bMainCharacter)
{
	FUNCTION_BEGIN;

	if (pRoleItem==NULL)
		return;

	t_Object& m_object=*(pRoleItem->GetObject());

	int nNextFiveType = GetFiveBorn( m_object.fivetype );
	for( int five=0;five<FIVE_NONE;five++ )
	{
		if ( m_object.fiveset[five] == 0 )
			break;

		if ( GetScene() && GetScene()->GetMainCharacter() )
		{
			CRoleItem* pItem = NULL;
			if ( bMainCharacter )
				pItem = GetScene()->GetMainCharacter()->FindItemByLocationFiveTypeConstID( m_object.fiveset[five], OBJECTCELLTYPE_EQUIP, nNextFiveType );
			else
			{
				if ( GetGameGuiManager()->m_guiUserInfoShow )
					pItem = GetGameGuiManager()->m_guiUserInfoShow->FindItemInEquipListByFiveTypeConstID( m_object.fiveset[five], nNextFiveType );
			}

			if ( NULL == pItem )
				break;

			ObjectBase_t* pObjectBase = GetObjectBase( m_object.fiveset[five] );
			if ( pObjectBase == NULL )
				break;

			if ( pItem && (pItem->GetObject()->fivetype != nNextFiveType) )
				break;

			if ( bMainCharacter )
			{
				if (GetGameGuiManager()->m_guiUserInfo)
				{
					GetGameGuiManager()->m_guiUserInfo->InsertActiveItem(pRoleItem);
					//GetGameGuiManager()->m_guiUserInfo->InsertActiveItem(pItem);
				}
			}
			else
			{
				if (GetGameGuiManager()->m_guiUserInfoShow)
				{
					GetGameGuiManager()->m_guiUserInfoShow->InsertActiveItem(pRoleItem);
					//GetGameGuiManager()->m_guiUserInfo->InsertActiveItem(pItem);
				}
			}

		}

		nNextFiveType = GetFiveBorn( nNextFiveType );			
	}

	FUNCTION_END;
}

bool IsBelongToMakerSet(const t_Object& object)
{
	if ( object.kind == 0 )		//White equip
		return false;

	if ( object.bind != 1 )		//Not binded
		return false;

	if ( strlen(object.maker) <= 0 )	//Is not be maked
		return false;

	stObjectLocation pos = object.pos;
	if ( pos.dwLocation == OBJECTCELLTYPE_EQUIP )	//It's yours
	{

		if ( GetGameGuiManager()->m_guiUserInfo )
		{
			if ( !(GetGameGuiManager()->m_guiUserInfo->GetSetEquipType() & object.kind) )	
				return false;
			if ( GetGameGuiManager()->m_guiUserInfo->GetSetMakerName() )
			{
				if ( strcmp( object.maker, GetGameGuiManager()->m_guiUserInfo->GetSetMakerName() ) == 0 )
					return true;
			}
		}		
	}
	else if ( pos.dwLocation == OBJECTCELLTYPE_EQUIPSHOW )		//Other player show
	{
		if ( GetGameGuiManager()->m_guiUserInfoShow )
		{
			if ( GetGameGuiManager()->m_guiUserInfoShow->GetSetEquipType() != object.kind )		//Not the same kind
				return false;
			if ( GetGameGuiManager()->m_guiUserInfoShow->GetSetMakerName() )
			{
				if ( strcmp( object.maker, GetGameGuiManager()->m_guiUserInfoShow->GetSetMakerName() ) == 0 )
					return true;
			}                   			
		}	
	}

	return false;
}

bool CRoleItem::IsBelongToMakerSet()
{
	return ::IsBelongToMakerSet( m_object );	
}

bool IsShowActiveItem(CRoleItem* pRoleItem)
{
	FUNCTION_BEGIN;

	if (pRoleItem==NULL)
		return false;

	t_Object& m_object=*(pRoleItem->GetObject());

	if ( m_object.fiveset[0] == 0 )
		return false;

	return true;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param flag : 描述
* \return 返回值的描述
*/
DWORD CRoleItem::GetPrice( int flag )
{
	FUNCTION_BEGIN;

	float ret = m_pObjectBase->price;

	switch( flag )
	{
	case ITEM_PRICE_SHOP:
		return m_price;
	case ITEM_PRICE_COMMON:		//base price
		{
			if(GetThisID() == INVALID_THISID)
				return ret;
			return 0;
		}
		break;
	case ITEM_PRICE_SELL:		//sell price
		{
			float sellPrice = ret;
			//Consider dur
			if ( m_object.maxdur > 0 )
			{
				if ( (m_pObjectBase->dwType == ItemType_Arrow) )
					sellPrice *= m_object.dur/m_object.maxdur;
				else
					sellPrice *= ((m_object.dur/25)*25) / m_object.maxdur;
			}			

			//Make some adjust according to character goodness
			sellPrice *= 0.25f;

			DWORD goodness = GetScene()->GetMainCharacter()->GetGoodnessAfterAdjust();
			switch(goodness)
			{
			case GOODNESS_3:
				sellPrice *= 0.7f;
				break;
			case GOODNESS_4:
				sellPrice *= 0.4f;
				break;
			case GOODNESS_5:
			case GOODNESS_6:
				sellPrice *= 0.1f ;
				break;
			}

			return (DWORD)(sellPrice + 0.99f);
		}
		break;
	case ITEM_PRICE_BUY:	//buy price
		{
			float buyPrice = ret;			
			//Make some adjust according to character goodness
			DWORD goodness = GetScene()->GetMainCharacter()->GetGoodnessAfterAdjust();
			switch(goodness)
			{
			case GOODNESS_3:
				buyPrice *= 1.2f;
				break;
			case GOODNESS_4:
				buyPrice *= 1.5f;
				break;
			case GOODNESS_5:
			case GOODNESS_6:
				buyPrice *= 2.0f;
				break;
			}

			//Consider middle country
			if ( GetScene()->IsCurMapIsMiddleCountry() )
				buyPrice *= 1.1f;

			return (DWORD)(buyPrice + 0.99f);
		}
		break;
	}

	return ret;
	return 0;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param flag : 描述
* \return 返回值的描述
*/
DWORD CRoleItem::GetRepairPrice( int flag )
{
	FUNCTION_BEGIN;

	return GetPrice( flag );

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param dwFlags : 描述
* \return 返回值的描述
*/
void CRoleItem::SetTipFlags(DWORD dwFlags)
{
	FUNCTION_BEGIN;

	if(m_dwTipFlags != dwFlags)
	{
		m_dwTipFlags = dwFlags;
		m_bNeedUpdateTips = true;
	}

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  能否装备
*/
DWORD CRoleItem::IsCanEquip()
{
	FUNCTION_BEGIN;

	if(0 == (m_pObjectBase->dwCharType & GetScene()->GetMainCharacter()->GetType()))
	{
		return CANNOTUSE_CHARTYPE;
	}

	if ( (m_pObjectBase->dwType == ItemType_Tonic )
		|| (m_pObjectBase->dwType == ItemType_DoubleExp )
		|| (m_pObjectBase->dwType == ItemType_Honor )
		|| (m_pObjectBase->dwType == ItemType_Flower) )
	{
		return 0;
	}

	if(m_object.needlevel > GetScene()->GetMainCharacter()->GetLevel())
	{
		return CANNOTUSE_LEVEL;
	}
	return 0;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param dwDur : 描述
* \return 返回值的描述
*/
void CRoleItem::OnUpdateDur(DWORD dwDur,DWORD dwMaxDur)
{
	FUNCTION_BEGIN;

	m_object.dur =dwDur;
	m_object.maxdur = dwMaxDur;
	m_bNeedUpdateTips = true;

	//Add to repair list
	if ( NeedConsiderDur() )
	{
		if ( (m_object.dur + 49)/50 < NEED_ADD_REPAIR_VALUE )
		{
			if ( GetScene()->GetMainCharacter() )
				GetScene()->GetMainCharacter()->AddOneNeedRepairItem( m_object.qwThisID );
		}
		else
		{
			if ( GetScene()->GetMainCharacter() )
				GetScene()->GetMainCharacter()->DeleteOneNeedRepairItem( m_object.qwThisID );
		}
	}

	if (IsArrow())
		OnUpdate(UPDATE_FLAG_COUNT);

	//Arrow is special 
	if ( IsArrow() && (m_object.pos.dwLocation == OBJECTCELLTYPE_EQUIP) )
	{
		if ( GetExteriorCount() < 20 )
		{
			GetScene()->GetMainCharacter()->AddOneNeedRepairItem( m_object.qwThisID );
		}
		else
		{
			GetScene()->GetMainCharacter()->DeleteOneNeedRepairItem( m_object.qwThisID );
		}		
	}

	//Auto equip fill-mp item
	if ( ( (GetItemType() == ItemType_FillMP) || (GetItemType() == ItemType_Tonic) || (GetItemType() == ItemType_FillXHP)  ) //soke 自动补血 72类
		&& (m_object.dur == 0)
		&& (GetLocation().dwLocation == OBJECTCELLTYPE_EQUIP)
		&& (m_object.bind == 1) )
	{
		GetScene()->GetMainCharacter()->AutoEquipFillItem( GetLocation().x, GetItemType() );
	}

	//Big fill-hp item access key change
	if ( (GetItemType() == ItemType_FillHP)
		&& (dwDur == 0)
		&& (GetLocation().dwLocation == OBJECTCELLTYPE_COMMON)
		&& (m_object.bind == 1) )
	{
		DWORD otherItemID = GetScene()->GetMainCharacter()->FindOtherItemWithSameTypeID( GetThisID(), GetObjectID() );
		if ( otherItemID != -1 )
			AccessKeyOnRemove( otherItemID );
	}

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
//Move float item back
void CRoleItem::MoveBackOldLocation()
{
	FUNCTION_BEGIN;

	DestoryAllVirtualCell();
	//MoveTo( m_oldLocation );

	FUNCTION_END;
}

inline bool CRoleItem::IsCanUse()
{ 
	// 打造字段数值为0  打造类型字段为34 同时物品类型为34
	if( GetItemType() == ItemType_Quest )
	{
		if( m_pObjectBase )
		{
			if( m_pObjectBase->dwMake == 0 || m_pObjectBase->maketype == 34)
			{
				return true;
			}
		}
	}

	if ( (GetObjectID() == 821)
		|| (GetObjectID() == 831)
		|| (GetObjectID() == 841) )
	{
		return true;
	}

	return	GetItemType() == ItemType_Leechdom 
		|| GetItemType() == ItemType_Food 
		|| GetItemType() == ItemType_Tools
		|| GetItemType() == ItemType_Pack
		|| GetItemType() == ItemType_Move
		|| GetItemType() == ItemType_Scroll
		|| GetItemType() == ItemType_Scrxll //soke 新类型 15
		|| GetItemType() == ItemType_Gift
		|| GetItemType() == ItemType_MASK
		|| GetItemType() == ItemType_State
		|| GetItemType() == ItemType_FillHP
		|| GetItemType() == ItemType_FillMP
		|| GetItemType() == ItemType_DoubleExpZ
		|| GetItemType() == ItemType_DoubleExpC
		|| GetItemType() == ItemType_DoubleExpG
		|| GetItemType() == ItemType_LuncyDan
		|| GetItemType() == ItemType_HzStone
		|| GetItemType() == ItemType_FillXHP //soke 自动补血 72类
		|| GetItemType() == ItemType_Tianxia
		|| GetItemType() == 60;
}		


/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void MoveBackFloatItem()
{
	FUNCTION_BEGIN;

	CGuiFloatItem* pFloatItem;
	if ( pFloatItem = GetGuiManager()->GetFloatItem() )
	{
		CRoleItem* pRoleItem = (CRoleItem*) pFloatItem->GetItem()->m_pItemData;
		pRoleItem->MoveBackOldLocation();
	}

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
//Get float item name
const char* GetFloatItemName()
{
	FUNCTION_BEGIN;

	CGuiFloatItem* pFloatItem;
	if ( pFloatItem = GetGuiManager()->GetFloatItem() )
	{
		CRoleItem* pRoleItem = (CRoleItem*) pFloatItem->GetItem()->m_pItemData;
		return pRoleItem->GetName();
	}
	return NULL;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
//Get float item this ID
DWORD GetFloatItemThisID( bool bEquipment )
{
	FUNCTION_BEGIN;

	CGuiFloatItem* pFloatItem;
	if ( pFloatItem = GetGuiManager()->GetFloatItem() )
	{
		CRoleItem* pRoleItem = (CRoleItem*) pFloatItem->GetItem()->m_pItemData;
		if ( bEquipment )
			return pRoleItem->GetThisID();
		else
		{
			if ( pRoleItem->IsAccessNeedThisID() )
				return pRoleItem->GetThisID();
			else
				return pRoleItem->GetObjectID();
		}
	}
	return -1;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param rl : 描述
* \return 返回值的描述
*/
//get float item resource location
bool GetFloatItemResource(stResourceLocation& rl)
{
	FUNCTION_BEGIN;

	CGuiFloatItem* pFloatItem;
	if ( pFloatItem = GetGuiManager()->GetFloatItem() )
	{
		CRoleItem* pRoleItem = (CRoleItem*) pFloatItem->GetItem()->m_pItemData;

		rl = GetItemPicLocation( pRoleItem->GetObject() );
		return true;
	}
	return false;

	FUNCTION_END;
}


bool CRoleItem::CanUseItemNow()
{
	FUNCTION_BEGIN;

	//make sure we have base object info
	if ( NULL == m_pObjectBase )
		m_pObjectBase = ::GetObjectBase( m_object.dwObjectID );

	if( !m_pObjectBase )
		return false;

	DWORD dwNowTime = xtimeGetTime();
	if( dwNowTime >= (m_pObjectBase->dwIntervalTime * 1000 + m_dwLastUseTime) )
	{
		m_dwLastUseTime = dwNowTime;
		return true;
	}
	return false;

	FUNCTION_END;
}

CRoleItem* CRoleItem::GetRoleItem(const CGuiItemCell* pCell)
{
	if( pCell )
	{
		return (CRoleItem*)(pCell->m_pItemData);
	}
	return NULL;
}

CRoleItem* CRoleItem::GetFloatRoleItem()
{
	CGuiFloatItem* pFloatItem = GetGuiManager()->GetFloatItem();
	if( pFloatItem )
	{
		CGuiItemCell* pCell = pFloatItem->GetItem();
		return CRoleItem::GetRoleItem(pCell);
	}
	return NULL;
}
