/**
 * \file
 * \version	$Id: RebuildObject.cpp  $
 * \author	
 * \date		
 * \brief		新物品合成，打造，升级，打孔，镶嵌系统
 * 
 */

#include <math.h>
#include <vector>
#include "RebuildObject.h"
#include "zObject.h"
#include "SceneUser.h"
#include "Command.h"
#include "Zebra.h"
#include "Chat.h"
#include "NpcTrade.h"
#include "SceneManager.h"
#include "SessionClient.h"
#include "Scene.h"
#include "CountryTech.h"


/**     
 * \brief NPC访问验证
 *      
 * 验证用户对NPC 的动作是否合法
 
 * \param user:待检查的用户
 * \param base: 物品基本表
 * \param action: 动作类型
 * \return 验证通过返回true,否则返回false
 */
bool Base::check_npc(SceneUser& user, zObjectB* base, int action)
{
	NpcTrade::NpcItem item;
	item.id = base->id;
	item.kind = base->kind;
	item.lowLevel = 0;
	item.level = base->needlevel;
	item.action = action;
	if ( !NpcTrade::getInstance().verifyNpcAction(user.npc_dwNpcDataID, item) ) {
		return false;
	}
	
	return true;	
}

/**     
 * \brief 包裹空间验证
 *      
 * 验证用户包裹中的剩余空间是否满足要求
 
 * \param user:待检查的用户
 * \param w: 需要宽度
 * \param h: 需要高度
 * \return 验证通过返回true,否则返回false
 */
bool Base::check_space(SceneUser& user, DWORD w, DWORD h)
{	
	if(user.packs.main.space() < 1)	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "包袱空间不足");
		return false;
	}
	
	return true;	
}

/**     
 * \brief 删除物品
 *      
 * 从用户包裹中删除一个物品
 
 * \param user:请求的用户
 * \param ob: 待 删除物品
 * \return 当前总是返回true
 */
bool Base::remove_object(SceneUser& user, zObject* ob)
{	
	user.packs.removeObject(ob); //notify and delete
	
	return true;	
}

/**   
 * \brief 删除物品
 *      
 * 从用户包裹中删除一个物品
 * \param user:请求的用户
 * \param ob: 待 删除物品
 * \return 当前总是返回true
 */
bool Base::removeObject( SceneUser& user, zObject* ob, const char *info )
{
    zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,0,user.charbase.id, user.charbase.name, 0,NULL,info,ob->base,ob->data.kind,ob->data.upgrade);
	user.packs.removeObject(ob); //notify and delete
	return true;	
}
/**

 * \brief  发送包裹中添加物品的消息给用户
 
 * \param user:请求的用户
 * \param ob: 添加的物品
 * \param add:添加的类型
 * \return 当前总是返回true
 */
bool Base::add_object(SceneUser& user, zObject* ob, bool add)
{
	Cmd::stAddObjectPropertyUserCmd ret;
	if (add) 
    {
		ret.byActionType = Cmd::EQUIPACTION_OBTAIN;
	}
    else 
    {
		ret.byActionType = Cmd::EQUIPACTION_REFRESH;
	}
	bcopy(&ob->data, &ret.object, sizeof(t_Object));
	user.sendCmdToMe(&ret, sizeof(ret));
	
	return true;	
}

/**     
 * \brief 发送物品改造的结果给用户
 *
 * \param user: 请求的用户
 * \param status: 改造结果
 * \param type: 改造类型
 * \return 当前总是返回true
 */
bool Base::response(SceneUser& user, int status, RebuildType type)
{
	Cmd::stResponsePropertyUserCmd ret;
	ret.kind = type;
	ret.status = status;
	user.sendCmdToMe(&ret, sizeof(ret));
	
	return true;
}

#define GET_MESSAGE(msg,msglen,pat)	\
	do	\
{	\
	va_list ap;	\
	bzero(msg, msglen);	\
	va_start(ap, pat);		\
	vsnprintf(msg, msglen - 1, pat, ap);	\
	va_end(ap);	\
}while(false)
//soke 包裹相关
bool Base::reply(SceneUser& user, int status, RebuildType type, const char *pattern, ...)
{
	Cmd::stResponsePropertyUserCmd send;
	send.kind = type;
	send.status = status;

    char buf[MAX_CHATINFO];
    bzero( buf, sizeof(buf) );
    GET_MESSAGE(buf, MAX_CHATINFO, pattern);

    strncpy((char *)send.info, buf, MAX_CHATINFO-1);
    user.sendCmdToMe(&send, sizeof(send));
    return true;
}

void Base::refresh_pack(SceneUser& user, ObjectPack* pack)
{
	zObject* tool = pack->object();
    if (!tool) return;

    if ( tool->data.dur > 0)
        --tool->data.dur;
    else
        tool->data.dur = 0;

	Cmd::stDurabilityUserCmd ret;
	ret.dwThisID = tool->data.qwThisID;
	ret.dwDur = tool->data.dur;
	ret.dwMaxDur = tool->data.maxdur;
	user.sendCmdToMe(&ret , sizeof(ret));
}

/**     
 * \brief 验证物品是否能打孔
 *
 * \param ob: 待验证物品
 * \return 验证成功返回true,否则返回false
 */	
bool Hole::can_hole(zObject* ob)
{
	//soke 增加马匹装备、利刃、卫士、战旗等装备可以打孔
	if ((ob->base->kind >= ItemType_ClothBody && ob->base->kind <=ItemType_Fing)
	 || (ob->base->kind >= ItemType_HorseShoe && ob->base->kind <=ItemType_HorseIron) 
	 || (ob->base->kind >= ItemType_Dagger && ob->base->kind <=ItemType_breastplate)
	 || (ob->base->kind >= ItemType_StaffFlag && ob->base->kind <=ItemType_SwordFlag)
	 || ob->base->kind == ItemType_LongGun || ob->base->kind == ItemType_Handgun) 
	{
		return true;
	}	
	
	return false;
	
}

/**     
 * \brief 取得物品上可用孔 的数量
 *
 * \param ob: 物品
 * \return 可用孔的数量
 */	
int Hole::get_empty_hole(zObject* ob)
{
	int hole = -1;
	while (hole<INVALID_INDEX && ob->data.socket[++hole]!=(DWORD)-1); // -1 means have hole
	
	return hole;
}

/**     
 * \brief 取得物品上所有孔 的数量
 *
 * \param ob: 物品
 * \return 所有孔的数量
 */	
int Hole::get_hole_num(zObject* ob)
{
	int hole = -1;
	while (hole<INVALID_INDEX && ob->data.socket[++hole]!=(DWORD)0); //0 means none hole
	if (hole==INVALID_INDEX) hole=INVALID_INDEX-1;
	return hole+1;
}

/**     
 * \brief 在物品上增加孔
 *
 * \param ob: 物品
 * \param num:增加的数量
 * \return 物品上孔的数目
 */	
int Hole::add_hole_num(zObject* ob, int num)
{
	int hole = Hole::get_hole_num(ob)-1;
	while (num-- > 0 && hole < Hole::INVALID_INDEX) {
		ob->data.socket[hole++] = (DWORD)-1;
	}

	return hole;
}

/**     
 * \brief 在物品上指定位置增加一个孔
 *
 * \param ob: 物品
 * \param index:增加孔 的位置
 * \return 增加孔成功返回true,否则返回false
 */
bool Hole::add_hole(zObject* ob, int index)
{
	if (index <INVALID_INDEX && ob->data.socket[index] != (DWORD)-1) {
		ob->data.socket[index] = (DWORD) -1;
		return true;
	}

	return false;
}

/**     
 * \brief 在物品上指定孔放置一个魂魄
 *
 * \param ob: 物品
 * \param index:放置 的位置
 * \param id : 放置的魂魄id
 * \return 放置成功返回true,否则返回false
 */
bool Hole::put_hole(zObject* ob, int index, int id)
{
	if (index <INVALID_INDEX && ob->data.socket[index] == (DWORD)-1) 
	{
		ob->data.socket[index] = (DWORD) id; // 1 means have hole and used
		return true;
	}

	return false;
}


const int SoulStone::_ids[] = {/*680, */748, 749, 750, 751};

/**     
 * \brief 选择魂魄石
 *
 * 在所有魂魄石中随机选择一个
 *
 * \return 魂魄石id
 */
int SoulStone::id(DWORD trait)
{
	int index = zMisc::randBetween(0, 4);
	switch(trait)
	{
		case 1:
		case 2:
		case 3:
		case 11:
		case 12:
		case 23:
		case 24:
			{
				index=0;
			}
			break;
		case 4:
		case 5:
		case 13:
		case 14:
		case 19:
		case 25:
		case 26:
			{
				index=1;
			}
			break;
		case 6:
		case 7:
		case 15:
		case 16:
		case 20:
		case 21:
		case 22:
			{
				index=2;
			}
			break;
		case 8:
		case 9:
		case 10:
		case 17:
		case 18:
		case 27:
		case 28:
			{
				index=3;
			}
			break;

		default:
			break;
	};
	return _ids[index];
}

/**
 * \brief 是否存在属性
 * \param value : 属性值
 * \return 存在属性返回true，否则返回false
 */
template <typename T>
bool EXIST_PROP(T& value)
{
	if (value.min || value.max) return true;

	return false;
}

#define PROCESS_PROP(x) \
				if (EXIST_PROP(soul->x)) { \
					ob->data.x += zMisc::randBetween(soul->x.min, soul->x.max); \
				}

bool SoulStone::assign(zObject* ob, int monster)
{
	zSoulStoneB *soul = soulstonebm.get(monster);
	if(soul==NULL) return false;

	ob->data.needlevel = soul->level;
	if (EXIST_PROP(soul->hpleech.odds))  {
		ob->data.hpleech.odds += zMisc::randBetween(soul->hpleech.odds.min, soul->hpleech.odds.max);
		ob->data.hpleech.effect += zMisc::randBetween(soul->hpleech.effect.min, soul->hpleech.effect.max);
	}
	if (EXIST_PROP(soul->mpleech.odds))  {
		ob->data.mpleech.odds += zMisc::randBetween(soul->mpleech.odds.min, soul->mpleech.odds.max);
		ob->data.mpleech.effect += zMisc::randBetween(soul->mpleech.effect.min, soul->mpleech.effect.max);
	}
	PROCESS_PROP( hptomp ) //转换生命值为法术值x％
		PROCESS_PROP( incgold ) //增加金钱掉落x%
		PROCESS_PROP( doublexp ) //x%双倍经验		
		PROCESS_PROP( mf ) //增加掉宝率x%
		PROCESS_PROP( poisondef ) //抗毒增加
		PROCESS_PROP( lulldef ) //抗麻痹增加
		PROCESS_PROP( reeldef ) //抗眩晕增加
		PROCESS_PROP( evildef ) //抗噬魔增加
		PROCESS_PROP( bitedef ) //抗噬力增加
		PROCESS_PROP( chaosdef ) //抗混乱增加
		PROCESS_PROP( colddef ) //抗冰冻增加
		PROCESS_PROP( petrifydef ) //抗石化增加
		PROCESS_PROP( blinddef ) //抗失明增加
		PROCESS_PROP( stabledef ) //抗定身增加
		PROCESS_PROP( slowdef ) //抗减速增加
		PROCESS_PROP( luredef ) //抗诱惑增加
		PROCESS_PROP( poison ) //中毒增加
		PROCESS_PROP( lull ) //麻痹增加
		PROCESS_PROP( reel ) //眩晕增加
		PROCESS_PROP( evil ) //噬魔增加
		PROCESS_PROP( bite ) //噬力增加
		PROCESS_PROP( chaos ) //混乱增加
		PROCESS_PROP( cold ) //冰冻增加
		PROCESS_PROP( petrify ) //石化增加
		PROCESS_PROP( blind ) //失明增加
		PROCESS_PROP( stable ) //定身增加
		PROCESS_PROP( slow ) //减速增加
		PROCESS_PROP( lure ) //诱惑增加
		PROCESS_PROP( xstr ) 
		PROCESS_PROP( xinte ) 
		PROCESS_PROP( xdex ) 
		PROCESS_PROP( xspi ) 
		PROCESS_PROP( xcon ) 
//		PROCESS_PROP( str ) 
//		PROCESS_PROP( inte ) 
//		PROCESS_PROP( dex ) 
//		PROCESS_PROP( spi ) 
//		PROCESS_PROP( con ) 


		/**
		 * \brief 可怜的清玉兄啊,你的代码都被干了啊 nnd
		 */
		/*
		   do {
		   switch (prop % (30+5) ) 
		   {
		   case 0:
		   if (EXIST_PROP(soul->hpleech.odds))  {
		   ob->data.hpleech.odds += zMisc::randBetween(soul->hpleech.odds.min, soul->hpleech.odds.max);
		   ob->data.hpleech.effect += zMisc::randBetween(soul->hpleech.effect.min, soul->hpleech.effect.max);
		   done = true;
		   }
		   break;
		   case 1:
		   if (EXIST_PROP(soul->hpleech.odds))  {
		   ob->data.mpleech.odds += zMisc::randBetween(soul->mpleech.odds.min, soul->mpleech.odds.max);
		   ob->data.mpleech.effect += zMisc::randBetween(soul->mpleech.effect.min, soul->mpleech.effect.max);
		   done = true;
		   }
		   break;

		   case 2:
		   PROCESS_PROP( hptomp ) //转换生命值为法术值x％

		   case 3:
		   PROCESS_PROP( incgold ) //增加金钱掉落x%
		   case 4:
		   PROCESS_PROP( doublexp ) //x%双倍经验		
		   case 5:
		   PROCESS_PROP( mf ) //增加掉宝率x%

		   case 6:
		   PROCESS_PROP( poisondef ) //抗毒增加
		   case 7:
		   PROCESS_PROP( lulldef ) //抗麻痹增加
		   case 8:
		   PROCESS_PROP( reeldef ) //抗眩晕增加
		   case 9:
		   PROCESS_PROP( evildef ) //抗噬魔增加
		   case 10:
		   PROCESS_PROP( bitedef ) //抗噬力增加
		   case 11:
		   PROCESS_PROP( chaosdef ) //抗混乱增加
		   case 12:
		   PROCESS_PROP( colddef ) //抗冰冻增加
		   case 13:
		   PROCESS_PROP( petrifydef ) //抗石化增加
		   case 14:
		   PROCESS_PROP( blinddef ) //抗失明增加
		   case 15:
		   PROCESS_PROP( stabledef ) //抗定身增加
		   case 16:
		   PROCESS_PROP( slowdef ) //抗减速增加
		   case 17:
		   PROCESS_PROP( luredef ) //抗诱惑增加

		   case 18:
		   PROCESS_PROP( poison ) //中毒增加
		   case 19:
		   PROCESS_PROP( lull ) //麻痹增加
		   case 20:
		   PROCESS_PROP( reel ) //眩晕增加
		   case 21:
		   PROCESS_PROP( evil ) //噬魔增加
		   case 22:
		   PROCESS_PROP( bite ) //噬力增加
		   case 23:
		   PROCESS_PROP( chaos ) //混乱增加
		   case 24:
		   PROCESS_PROP( cold ) //冰冻增加
		   case 25:
		   PROCESS_PROP( petrify ) //石化增加
		   case 26:
		   PROCESS_PROP( blind ) //失明增加
	case 27:
		PROCESS_PROP( stable ) //定身增加
	case 28:
			PROCESS_PROP( slow ) //减速增加
	case 29:
				PROCESS_PROP( lure ) //诱惑增加
	case 30:
					PROCESS_PROP( str ) 
	case 31:
						PROCESS_PROP( inte ) 
	case 32:
							PROCESS_PROP( dex ) 
	case 33:
								PROCESS_PROP( spi ) 
	case 34:
									PROCESS_PROP( con ) 

}

}while (!done && ((++prop % (30+5) ) != old));
// */

return true;
}

/**     
 * \brief 魂魄石合成
 *
 * \param user: 请求的用户
 * \param first: 第一块魂魄石
 * \param second:第二块魂魄石
 * \param odds:合成几率
 * \return 合成成功返回true,否则返回false
 */
zObject* SoulStone::compose(SceneUser& user, zObject* first, zObject* second, int odds)
{	
	if ( !zMisc::selectByPercent(odds) ) return false;
	
	/**
	 * \brief 改变镶嵌图片选择方式
	 * whj
	 *
	 */
	zObjectB *base = objectbm.get(id(1));
	if (!base) return false;
	
	int level = std::max(first->data.upgrade, second->data.upgrade);
	
	zObject *ob = zObject::create(base, 1, ++level);
	if (ob) {
		zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"魂魄合成",ob->base,ob->data.kind,ob->data.upgrade);
		do_compose(first, second, ob);
		
		return ob;
	}

	return NULL;	
}

/**     
 * \brief 魂魄石镶嵌
 *
 * \param user: 请求的用户
 * \param dest: 待镶嵌物品
 * \param src:用来镶嵌的魂魄石
 * \return 当前总是返回true
 */
bool SoulStone::enchance(SceneUser& user, zObject* dest, zObject* src)
{
	do_enchance(dest, src);

	return true;
}

#undef COMPUTE
#define COMPUTE(x) dest->data.x += src->data.x;

/**     
 * \brief 魂魄石镶嵌
 *
 * \param dest: 待镶嵌物品
 * \param src:用来镶嵌的魂魄石
 * \return 当前总是返回true
 */
bool SoulStone::do_enchance(zObject* dest, zObject* src)
{
	dest->data.needlevel = std::max(dest->data.needlevel, src->data.needlevel);
	
	//sprintf(dest->data.strName, "镶嵌了魂魄的%s", dest->base->name);
	COMPUTE( hpleech.odds )
	COMPUTE( hpleech.effect ) //x%吸收生命值y,
	COMPUTE( mpleech.odds )
	COMPUTE( mpleech.effect ) // x%吸收法术值y
	
	COMPUTE( hptomp ) //转换生命值为法术值x％

	COMPUTE( incgold ) //增加金钱掉落x%
	COMPUTE( doublexp ) //x%双倍经验		
	COMPUTE( mf ) //增加掉宝率x%
	
	COMPUTE( poisondef ) //抗毒增加
	COMPUTE( lulldef ) //抗麻痹增加
	COMPUTE( reeldef ) //抗眩晕增加
	COMPUTE( evildef ) //抗噬魔增加
	COMPUTE( bitedef ) //抗噬力增加
	COMPUTE( chaosdef ) //抗混乱增加
	COMPUTE( colddef ) //抗冰冻增加
	COMPUTE( petrifydef ) //抗石化增加
	COMPUTE( blinddef ) //抗失明增加
	COMPUTE( stabledef ) //抗定身增加
	COMPUTE( slowdef ) //抗减速增加
	COMPUTE( luredef ) //抗诱惑增加

	COMPUTE( poison ) //中毒增加
	COMPUTE( lull ) //麻痹增加
	COMPUTE( reel ) //眩晕增加
	COMPUTE( evil ) //噬魔增加
	COMPUTE( bite ) //噬力增加
	COMPUTE( chaos ) //混乱增加
	COMPUTE( cold ) //冰冻增加
	COMPUTE( petrify ) //石化增加
	COMPUTE( blind ) //失明增加
	COMPUTE( stable ) //定身增加
	COMPUTE( slow ) //减速增加
	COMPUTE( lure ) //诱惑增加

	COMPUTE( xstr )
	COMPUTE( xinte ) 
	COMPUTE( xdex ) 
	COMPUTE( xspi )
	COMPUTE( xcon )
//	COMPUTE( str )
//	COMPUTE( inte ) 
//	COMPUTE( dex ) 
//	COMPUTE( spi )
//	COMPUTE( con )
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) additive(first->data.x, second->data.x, dest->data.x, level);
/**     
 * \brief 魂魄石合成
 *
 * \param first: 第一块魂魄石
 * \param second:第二块魂魄石
 * \param dest:新生成的魂魄石
 * \param odds:合成几率
 * \return 合成成功返回true,否则返回false
 */
bool SoulStone::do_compose(zObject* first, zObject* second, zObject* dest)
{
	int level = ( (first->data.upgrade & 0xff) << 8 ) | (second->data.upgrade & 0xff);

	dest->data.needlevel = std::max(first->data.needlevel, second->data.needlevel);
	
	COMPUTE( hpleech.odds )
	COMPUTE( hpleech.effect ) //x%吸收生命值y,
	COMPUTE( mpleech.odds )
	COMPUTE( mpleech.effect ) // x%吸收法术值y
	
	COMPUTE( hptomp ) //转换生命值为法术值x％

	COMPUTE( incgold ) //增加金钱掉落x%
	COMPUTE( doublexp ) //x%双倍经验		
	COMPUTE( mf ) //增加掉宝率x%
	
	COMPUTE( poisondef ) //抗毒增加
	COMPUTE( lulldef ) //抗麻痹增加
	COMPUTE( reeldef ) //抗眩晕增加
	COMPUTE( evildef ) //抗噬魔增加
	COMPUTE( bitedef ) //抗噬力增加
	COMPUTE( chaosdef ) //抗混乱增加
	COMPUTE( colddef ) //抗冰冻增加
	COMPUTE( petrifydef ) //抗石化增加
	COMPUTE( blinddef ) //抗失明增加
	COMPUTE( stabledef ) //抗定身增加
	COMPUTE( slowdef ) //抗减速增加
	COMPUTE( luredef ) //抗诱惑增加

	COMPUTE( poison ) //中毒增加
	COMPUTE( lull ) //麻痹增加
	COMPUTE( reel ) //眩晕增加
	COMPUTE( evil ) //噬魔增加
	COMPUTE( bite ) //噬力增加
	COMPUTE( chaos ) //混乱增加
	COMPUTE( cold ) //冰冻增加
	COMPUTE( petrify ) //石化增加
	COMPUTE( blind ) //失明增加
	COMPUTE( stable ) //定身增加
	COMPUTE( slow ) //减速增加
	COMPUTE( lure ) //诱惑增加

	COMPUTE( xstr )
	COMPUTE( xinte ) 
	COMPUTE( xdex ) 
	COMPUTE( xspi )
	COMPUTE( xcon )
//	COMPUTE( str )
//	COMPUTE( inte ) 
//	COMPUTE( dex ) 
//	COMPUTE( spi )
//	COMPUTE( con )
	
	return true;
}

/**     
 * \brief 绿装升星物品
 *
 * \param user:绿装升星物品的用户
 * \param ob: 绿装升星物品
 * \param extra_odds: 额外成功率
 * \return 绿装升星成功返回true,失败返回false
 */
bool Upgrade::upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
	zUpgradeObject2B *base = upgradeobject2bm.get(ob->data.dwObjectID+100000*(ob->data.upgrade+1));
	if(!base) return false;

	if (ob->data.upgrade < 16 && (ob->data.kind & 4 || !ob->data.kind & 16) && ob->base->kind != ItemType_Amulets)
	{
	   Zebra::logger->debug("绿色装备升%d星机率为%d",ob->data.upgrade+1,base->odds + extra_odds);
	   if (zMisc::selectByPercent(base->odds + extra_odds)) 
	   {
		   do_2upgrade(ob, base);
		   if (ob->data.upgrade>14 && (ob->data.kind & 4 || !ob->data.kind & 16))
		   {
               //soke 玩家升星世界滚动公告
		       Channel::sendAllInfo(Cmd::INFO_TYPE_ZISELROLL, "真牛 %s的%s将绿色装备 %s 成功升到%d星！",SceneManager::getInstance().getCountryNameByCountryID(user.charbase.country),user.charbase.name,ob->name,ob->data.upgrade);
		       Zebra::logger->debug("%s的%s将绿色装备 %s 升级到了%d星!",SceneManager::getInstance().getCountryNameByCountryID(user.charbase.country),user.charbase.name,ob->name,ob->data.upgrade);	
		   }
		   return true;
	   }
	   else 
	   {
          	//soke 绿装升星不掉星(已修复)
		  	int down;
			if(zMisc::selectByPercent(50))
				down = 0;
			else
			   //13级升14级失败降到0级，其他情况还是降1级
				down = ob->data.upgrade >= 13?ob->data.upgrade:1;

			for( int i=0;i<down;i++)
			{
				base = upgradeobject2bm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			  	if (base) do_2downgrade(ob, base);
			}
		  return false;
	   }
	}
	else
	{
	   Zebra::logger->debug("装备升%d星机率为%d",ob->data.upgrade+1,base->odds + extra_odds);
	   if (zMisc::selectByPercent(base->odds + extra_odds)) 
	   {
		   do_2upgrade(ob, base);
		   return true;
	   }
	   else 
	   {
		   	//soke 绿装升星不掉星(已修复)
		  	int down;
			if(zMisc::selectByPercent(50))
				down = 0;
			else
			   //13级升14级失败降到0级，其他情况还是降1级
				down = ob->data.upgrade >= 13?ob->data.upgrade:1;				

			for( int i=0;i<down;i++)
			{
			   base = upgradeobject2bm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			   if (base) do_2downgrade(ob, base);
		    }
		   return false;
	   }
	 }
	 return true;
}

/**     
 * \brief 紫装升星物品
 *
 * \param user:紫装升星物品的用户
 * \param ob: 紫装升星物品
 * \param extra_odds: 额外成功率
 * \return 紫装升星成功返回true,失败返回false
 */
bool Upgrade::purple_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
	  zUpgradePurpleB *base = upgradepurplebm.get(ob->data.dwObjectID+100000*(ob->data.upgrade+1));
	  if(!base) return false;

	  if (ob->data.upgrade < 16 && (ob->data.kind & 16 || !ob->data.kind & 4) && ob->base->kind != ItemType_Amulets)
	  {
	     Zebra::logger->debug("紫色装备升%d星机率为%d",ob->data.upgrade+1,base->odds + extra_odds);
	     if (zMisc::selectByPercent(base->odds + extra_odds)) 
	     {
		     pur_upgrade(ob, base);
		     if (ob->data.upgrade>14 && (ob->data.kind & 16 || !ob->data.kind & 4))
		     {
                 //soke 玩家升星世界滚动公告
		         Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEZROLL, "真牛 %s的%s将紫色装备 %s 成功升到%d星！",SceneManager::getInstance().getCountryNameByCountryID(user.charbase.country),user.charbase.name,ob->name,ob->data.upgrade);
			     Zebra::logger->debug("%s的%s将紫色装备 %s 升级到了%d星！",SceneManager::getInstance().getCountryNameByCountryID(user.charbase.country),user.charbase.name,ob->name,ob->data.upgrade);
		     }
		     return true;
		
	   	}
		else if (user.charbase.luckytime > 0 && ob->data.upgrade <= 11)
		{
			//soke 幸运丹状态紫装升星不掉星（已修复）
			return false;
		}
		else
		{
			//soke 紫装升星失败 50%几率不掉星，50%几率掉星 (已修复)

			int down;
			if(zMisc::selectByPercent(50))
				down = 0;
			else
			   //10级升11级失败降到0级，其他情况还是降1级
				down = ob->data.upgrade >= 10?ob->data.upgrade:1;

			for( int i=0;i<down;i++)
			{
				base = upgradepurplebm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
				if (base) pur_downgrade(ob, base);
			}
			return false;
		}
	}
	 return true;
}

/**     
 * \brief 护身符升级物品
 *
 * \param user:护身符升级物品的用户
 * \param ob: 护身符升级物品
 * \param extra_odds: 额外成功率
 * \return 护身符升级成功返回true,失败返回false
 */
bool Upgrade::hsf_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
	//zAmuletObjectB *base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade+1));
	//if(!base) return false;

	if (ob->base->kind == ItemType_Amulets) //soke 护身符升级
	{
		zAmuletObjectB *base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade+1));
	    if(!base) return false;

		/////////////////////////////// 护身符力量 开始/////////////////////////////////
		if ((ob->data.dwObjectID >= 18001 && ob->data.dwObjectID <= 18005) && ob->data.upgrade <= 199 )
		{
		   if (ob->data.dwObjectID==18001)
		   {
	          Zebra::logger->debug("护身符·力量(白)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_liliang(ob, base); //soke 一次加1点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18002)
		   {
	          Zebra::logger->debug("护身符·力量(蓝)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_liliang(ob, base); //soke 一次加1点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18003)
		   {
	          Zebra::logger->debug("护身符·力量(黄)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_liliang2(ob, base); //soke 一次加2点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18004)
		   {
	          Zebra::logger->debug("护身符·力量(绿)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_liliang3(ob, base); //soke 一次加3点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18005)
		   {
	          Zebra::logger->debug("护身符·力量(紫)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_liliang15(ob, base); //soke 一次加15点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
	    }
		else if ((ob->data.dwObjectID >= 18001 && ob->data.dwObjectID <= 18004) && ob->data.upgrade > 199 )
		{
		   if (ob->data.dwObjectID==18001)
		   {
	          Zebra::logger->debug("护身符·力量(白)品质提升(蓝)成功率为%d",base->odds + extra_odds);
		   }
		   else if (ob->data.dwObjectID==18002)
		   {
	          Zebra::logger->debug("护身符·力量(蓝)品质提升(黄)成功率为%d",base->odds + extra_odds);
		   }
		   else if (ob->data.dwObjectID==18003)
		   {
	          Zebra::logger->debug("护身符·力量(黄)品质提升(绿)成功率为%d",base->odds + extra_odds);
		   }
		   else if (ob->data.dwObjectID==18004)
		   {
	          Zebra::logger->debug("护身符·力量(绿)品质提升(紫)成功率为%d",base->odds + extra_odds);
		   }
	       if (zMisc::selectByPercent(base->odds + extra_odds)) 
	       {
		      do_lanupgrade(ob, base);
		      return true;
	       }
		   else 
	       {
		      //soke 护身符升级不掉星
		      int down = 0;
		      for( int i=0;i<down;i++)
		      {
			      base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			      if (base) hsf_downgrade(ob, base);
		      }
		      return false;
	       }
	    }
		///////////////////////////////2017-1-24 护身符力量 结束/////////////////////////////////
	    ///////////////////////////////2017-1-24 护身符敏捷 开始/////////////////////////////////
		else if ((ob->data.dwObjectID >= 18006 && ob->data.dwObjectID <= 18010) && ob->data.upgrade <= 199 )
		{
		   if (ob->data.dwObjectID==18006)
		   {
	          Zebra::logger->debug("护身符·敏捷(白)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_minjie(ob, base); //soke 一次加1点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18007)
		   {
	          Zebra::logger->debug("护身符·敏捷(蓝)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_minjie(ob, base); //soke 一次加1点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18008)
		   {
	          Zebra::logger->debug("护身符·敏捷(黄)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_minjie2(ob, base); //soke 一次加2点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18009)
		   {
	          Zebra::logger->debug("护身符·敏捷(绿)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_minjie3(ob, base); //soke 一次加3点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18010)
		   {
	          Zebra::logger->debug("护身符·敏捷(紫)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_minjie15(ob, base); //soke 一次加15点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
	    }
		else if ((ob->data.dwObjectID >= 18006 && ob->data.dwObjectID <= 18009) && ob->data.upgrade > 199 )
		{
		   if (ob->data.dwObjectID==18006)
		   {
	          Zebra::logger->debug("护身符·敏捷(白)品质提升(蓝)成功率为%d",base->odds + extra_odds);
		   }
		   else if (ob->data.dwObjectID==18007)
		   {
	          Zebra::logger->debug("护身符·敏捷(蓝)品质提升(黄)成功率为%d",base->odds + extra_odds);
		   }
		   else if (ob->data.dwObjectID==18008)
		   {
	          Zebra::logger->debug("护身符·敏捷(黄)品质提升(绿)成功率为%d",base->odds + extra_odds);
		   }
		   else if (ob->data.dwObjectID==18009)
		   {
	          Zebra::logger->debug("护身符·敏捷(绿)品质提升(紫)成功率为%d",base->odds + extra_odds);
		   }
	       if (zMisc::selectByPercent(base->odds + extra_odds)) 
	       {
		      do_lanupgrade(ob, base);
		      return true;
	       }
		   else 
	       {
		      //soke 护身符升级不掉星
		      int down = 0;
		      for( int i=0;i<down;i++)
		      {
			      base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			      if (base) hsf_downgrade(ob, base);
		      }
		      return false;
	       }
	    }
		///////////////////////////////2017-1-24 护身符敏捷 结束/////////////////////////////////
		///////////////////////////////2017-1-24 护身符智力 开始/////////////////////////////////
		else if ((ob->data.dwObjectID >= 18011 && ob->data.dwObjectID <= 18015) && ob->data.upgrade <= 199 )
		{
		   if (ob->data.dwObjectID==18011)
		   {
	          Zebra::logger->debug("护身符·智力(白)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_zhili(ob, base); //soke 一次加1点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18012)
		   {
	          Zebra::logger->debug("护身符·智力(蓝)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_zhili(ob, base); //soke 一次加1点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18013)
		   {
	          Zebra::logger->debug("护身符·智力(黄)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_zhili2(ob, base); //soke 一次加2点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18014)
		   {
	          Zebra::logger->debug("护身符·智力(绿)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_zhili3(ob, base); //soke 一次加3点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18015)
		   {
	          Zebra::logger->debug("护身符·智力(紫)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_zhili15(ob, base); //soke 一次加15点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
	    }
		else if ((ob->data.dwObjectID >= 18011 && ob->data.dwObjectID <= 18014) && ob->data.upgrade > 199 )
		{
		   if (ob->data.dwObjectID==18011)
		   {
	          Zebra::logger->debug("护身符·智力(白)品质提升(蓝)成功率为%d",base->odds + extra_odds);
		   }
		   else if (ob->data.dwObjectID==18012)
		   {
	          Zebra::logger->debug("护身符·智力(蓝)品质提升(黄)成功率为%d",base->odds + extra_odds);
		   }
		   else if (ob->data.dwObjectID==18013)
		   {
	          Zebra::logger->debug("护身符·智力(黄)品质提升(绿)成功率为%d",base->odds + extra_odds);
		   }
		   else if (ob->data.dwObjectID==18014)
		   {
	          Zebra::logger->debug("护身符·智力(绿)品质提升(紫)成功率为%d",base->odds + extra_odds);
		   }
	       if (zMisc::selectByPercent(base->odds + extra_odds)) 
	       {
		      do_lanupgrade(ob, base);
		      return true;
	       }
		   else 
	       {
		      //soke 护身符升级不掉星
		      int down = 0;
		      for( int i=0;i<down;i++)
		      {
			      base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			      if (base) hsf_downgrade(ob, base);
		      }
		      return false;
	       }
	    }
		///////////////////////////////2017-1-24 护身符智力 结束/////////////////////////////////
		///////////////////////////////2017-1-24 护身符体质 开始/////////////////////////////////
		else if ((ob->data.dwObjectID >= 18016 && ob->data.dwObjectID <= 18020) && ob->data.upgrade <= 199 )
		{
		   if (ob->data.dwObjectID==18016)
		   {
	          Zebra::logger->debug("护身符·体质(白)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_tizhi(ob, base); //soke 一次加1点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18017)
		   {
	          Zebra::logger->debug("护身符·体质(蓝)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_tizhi(ob, base); //soke 一次加1点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18018)
		   {
	          Zebra::logger->debug("护身符·体质(黄)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_tizhi2(ob, base); //soke 一次加2点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18019)
		   {
	          Zebra::logger->debug("护身符·体质(绿)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_tizhi3(ob, base); //soke 一次加3点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18020)
		   {
	          Zebra::logger->debug("护身符·体质(紫)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_tizhi15(ob, base); //soke 一次加15点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
	    }
		else if ((ob->data.dwObjectID >= 18016 && ob->data.dwObjectID <= 18019) && ob->data.upgrade > 199 )
		{
		   if (ob->data.dwObjectID==18016)
		   {
	          Zebra::logger->debug("护身符·体质(白)品质提升(蓝)成功率为%d",base->odds + extra_odds);
		   }
		   else if (ob->data.dwObjectID==18017)
		   {
	          Zebra::logger->debug("护身符·体质(蓝)品质提升(黄)成功率为%d",base->odds + extra_odds);
		   }
		   else if (ob->data.dwObjectID==18018)
		   {
	          Zebra::logger->debug("护身符·体质(黄)品质提升(绿)成功率为%d",base->odds + extra_odds);
		   }
		   else if (ob->data.dwObjectID==18019)
		   {
	          Zebra::logger->debug("护身符·体质(绿)品质提升(紫)成功率为%d",base->odds + extra_odds);
		   }
	       if (zMisc::selectByPercent(base->odds + extra_odds)) 
	       {
		      do_lanupgrade(ob, base);
		      return true;
	       }
		   else 
	       {
		      //soke 护身符升级不掉星
		      int down = 0;
		      for( int i=0;i<down;i++)
		      {
			      base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			      if (base) hsf_downgrade(ob, base);
		      }
		      return false;
	       }
	    }
		///////////////////////////////2017-1-24 护身符体质 结束/////////////////////////////////
		///////////////////////////////2017-1-24 护身符精神 开始/////////////////////////////////
		else if ((ob->data.dwObjectID >= 18021 && ob->data.dwObjectID <= 18025) && ob->data.upgrade <= 199 )
		{
		   if (ob->data.dwObjectID==18021)
		   {
	          Zebra::logger->debug("护身符·精神(白)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_jingshen(ob, base); //soke 一次加1点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18022)
		   {
	          Zebra::logger->debug("护身符·精神(蓝)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_jingshen(ob, base); //soke 一次加1点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18023)
		   {
	          Zebra::logger->debug("护身符·精神(黄)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_jingshen2(ob, base); //soke 一次加2点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18024)
		   {
	          Zebra::logger->debug("护身符·精神(绿)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_jingshen3(ob, base); //soke 一次加3点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
		   else if (ob->data.dwObjectID==18025)
		   {
	          Zebra::logger->debug("护身符·精神(紫)升%d级成功率为%d",ob->data.upgrade+1,base->odds + extra_odds);
			  if (zMisc::selectByPercent(base->odds + extra_odds)) 
	          {
		         do_jingshen15(ob, base); //soke 一次加15点
		         return true;
	          }
		      else 
	          {
		         //soke 护身符升级不掉星
		         int down = 0;
		         for( int i=0;i<down;i++)
		         {
			         base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			         if (base) hsf_downgrade(ob, base);
		         }
		         return false;
	          }
		   }
	    }
		else if ((ob->data.dwObjectID >= 18021 && ob->data.dwObjectID <= 18024) && ob->data.upgrade > 199 )
		{
		   if (ob->data.dwObjectID==18021)
		   {
	          Zebra::logger->debug("护身符·精神(白)品质提升(蓝)成功率为%d",base->odds + extra_odds);
		   }
		   else if (ob->data.dwObjectID==18022)
		   {
	          Zebra::logger->debug("护身符·精神(蓝)品质提升(黄)成功率为%d",base->odds + extra_odds);
		   }
		   else if (ob->data.dwObjectID==18023)
		   {
	          Zebra::logger->debug("护身符·精神(黄)品质提升(绿)成功率为%d",base->odds + extra_odds);
		   }
		   else if (ob->data.dwObjectID==18024)
		   {
	          Zebra::logger->debug("护身符·精神(绿)品质提升(紫)成功率为%d",base->odds + extra_odds);
		   }
	       if (zMisc::selectByPercent(base->odds + extra_odds)) 
	       {
		      do_lanupgrade(ob, base);
		      return true;
	       }
		   else 
	       {
		      //8级升9级失败降到0级，其他情况还是降2级
		      int down = 0;
		      for( int i=0;i<down;i++)
		      {
			      base = amuletobjectbm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
			      if (base) hsf_downgrade(ob, base);
		      }
		      return false;
	       }
	    }
		///////////////////////////////2017-1-24 护身符精神 结束/////////////////////////////////
	 }
	 return true;
}

/**   
 * \brief 斗魂升级物品
 *
 * \param user:斗魂升级物品的用户
 * \param ob: 斗魂升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::dh_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUdouhunObjectB *base = udouhunobjectbm.get(ob->data.dwObjectID+100000*(ob->data.douhun + 1));
	if(!base) return false;
	
	Zebra::logger->debug("斗魂升级成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		douhun_upgrade(ob, base);
		return true;
	}
    else 
    {
		//8级升9级失败降到0级，其他情况还是降2级
		int down;
		if(zMisc::selectByPercent(50))
			down = 0;
		else
			down = ob->data.douhun >= 8?ob->data.douhun:2;

		for( int i=0;i<down;i++)
		{
			base = udouhunobjectbm.get(ob->data.dwObjectID+100000*(ob->data.douhun));
			if (base) douhun_downgrade(ob, base);
		}
		return false;
	}
}

/**   
 * \brief 补天升级物品
 *
 * \param user:补天升级物品的用户
 * \param ob: 补天升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::bt_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUbutianObjectB *base = ubutianobjectbm.get(ob->data.dwObjectID+100000*(ob->data.butian + 1));
	if(!base) return false;
	
	Zebra::logger->debug("补天升级成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		butian_upgrade(ob, base);
		return true;
	}
    else 
    {
		//补天不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = ubutianobjectbm.get(ob->data.dwObjectID+100000*(ob->data.butian));
			if (base) butian_downgrade(ob, base);
		}
		return false;
	}
}

/**   
 * \brief 神龙镶嵌龙宝宝
 *
 * \param user:神龙镶嵌龙宝宝的用户
 * \param ob: 神龙镶嵌龙宝宝物品
 * \param extra_odds: 额外成功率
 * \return 镶嵌成功返回true,失败返回false
 */
bool Upgrade::longwang_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUlongwanglvObjectB *base = ulongwanglvobjectbm.get(ob->data.dwObjectID+100000*(ob->data.longwanglv + 1));
	if(!base) return false;
	
	Zebra::logger->debug("神龙镶嵌成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_longwanglv_upgrade(ob, base);
		return true;
	}
    else 
    {
		//神龙镶嵌不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = ulongwanglvobjectbm.get(ob->data.dwObjectID+100000*(ob->data.longwanglv));
			if (base) do_longwanglv_downgrade(ob, base);
		}
		return false;
	}
}

/**   
 * \brief 逆天进阶物品
 *
 * \param user:逆天进阶物品的用户
 * \param ob: 逆天进阶物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::nitian_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUnitianlvObjectB *base = unitianlvobjectbm.get(ob->data.dwObjectID+100000*(ob->data.nitianlv + 1));
	if(!base) return false;
	
	Zebra::logger->debug("逆天进阶成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_nitianlv_upgrade(ob, base);
		return true;
	}
    else 
    {
		//不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = unitianlvobjectbm.get(ob->data.dwObjectID+100000*(ob->data.nitianlv));
			if (base) do_nitianlv_downgrade(ob, base);
		}
		return false;
	}
}

/**   
 * \brief 神魂宝石进阶物品
 *
 * \param user:神魂宝石物品进阶的用户
 * \param ob: 神魂宝石物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::douzhansf_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUdouzhansflvObjectB *base = udouzhansflvobjectbm.get(ob->data.dwObjectID+100000*(ob->data.douzhansflv + 1));
	if(!base) return false;
	
	Zebra::logger->debug("神魂宝石成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_douzhansflv_upgrade(ob, base);
		return true;
	}
    else 
    {
		//不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = udouzhansflvobjectbm.get(ob->data.dwObjectID+100000*(ob->data.douzhansflv));
			if (base) do_douzhansflv_downgrade(ob, base);
		}
		return false;
	}
}

/**   
 * \brief 暗影宝石镶嵌物品
 *
 * \param user:神魂宝石镶嵌的用户
 * \param ob: 神魂宝石物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::aystone_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUaystonelvObjectB *base = uaystonelvobjectbm.get(ob->data.dwObjectID+100000*(ob->data.aystonelv + 1));
	if(!base) return false;
	
	Zebra::logger->debug("神魂宝石成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_aystonelv_upgrade(ob, base);
		return true;
	}
    else 
    {
		//不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = uaystonelvobjectbm.get(ob->data.dwObjectID+100000*(ob->data.aystonelv));
			if (base) do_aystonelv_downgrade(ob, base);
		}
		return false;
	}
}


/**   
 * \brief 尚方宝剑升级物品
 *
 * \param user:尚方宝剑升级的用户
 * \param ob: 尚方宝剑物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::shangfang_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUshangfanglvObjectB *base = ushangfanglvobjectbm.get(ob->data.dwObjectID+10000*(ob->data.shangfanglv + 1));
	if(!base) return false;
	
	Zebra::logger->debug("尚方宝剑成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_shangfang_upgrade(ob, base);
		return true;
	}
    else 
    {
		//不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = ushangfanglvobjectbm.get(ob->data.dwObjectID+10000*(ob->data.shangfanglv));
			if (base) do_shangfang_downgrade(ob, base);
		}
		return false;
	}
}

/**   
 * \brief 紫金龙袍升级物品
 *
 * \param user:紫金龙袍升级的用户
 * \param ob: 紫金龙袍物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::zijinlp_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{	
	zUzijinlplvObjectB *base = uzijinlplvobjectbm.get(ob->data.dwObjectID+10000*(ob->data.zijinlplv + 1));
	if(!base) return false;
	
	Zebra::logger->debug("紫金龙袍成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_zijinlp_upgrade(ob, base);
		return true;
	}
    else 
    {
		//不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = uzijinlplvobjectbm.get(ob->data.dwObjectID+10000*(ob->data.zijinlplv));
			if (base) do_zijinlp_downgrade(ob, base);
		}
		return false;
	}
}



/**   
 * \brief 勋章升级物品
 *
 * \param user:勋章升级物品的用户
 * \param ob: 勋章升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::medal_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{

	zMedaltObjectB *base = medaltobjectbm.get(ob->data.dwObjectID+10000*(ob->data.dmedalt + 1));
	if(!base) return false;

	Zebra::logger->debug("勋章升级成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
	{
		do_medal_upgrade(ob, base);
		return true;
	}
	else 
	{
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = medaltobjectbm.get(ob->data.dwObjectID+10000*(ob->data.dmedalt));
			if (base) do_medal_downgrade(ob, base);
		}
		return false;
	}
}



/**   
 * \brief 寒冰升级物品
 *
 * \param user:寒冰升级物品的用户
 * \param ob: 寒冰升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::hb_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUhbqlObjectB *base = uhbqlobjectbm.get(ob->data.dwObjectID+10000*(ob->data.retain1 + 1));
	if(!base) return false;
	
	Zebra::logger->debug("寒冰升级成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		hbql_upgrade(ob, base);
		return true;
	}
    else 
    {
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = uhbqlobjectbm.get(ob->data.dwObjectID+10000*(ob->data.retain1));
			if (base) hbql_downgrade(ob, base);
		}
		return false;
	}
}

/**   
 * \brief 神器升级物品
 *
 * \param user:神器升级物品的用户
 * \param ob: 神器升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::shenqi_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUshenqiObjectB *base = ushenqiobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain3 + 1));
	if(!base) return false;
	
	Zebra::logger->debug("神器进阶成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_shenqi_upgrade(ob, base);
		return true;
	}
    else 
    {
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = ushenqiobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain3));
			if (base) do_shenqi_downgrade(ob, base);
		}
		return false;
	}
}

/**   法宝
 * \brief 法宝升级物品
 *
 * \param user:法宝升级物品的用户
 * \param ob: 法宝升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::fabao_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUfabaoObjectB *base = ufabaoobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain20 + 1));
	if(!base) return false;
	
	Zebra::logger->debug("法宝进阶成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_fabao_upgrade(ob, base);
		return true;
	}
    else 
    {
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = ufabaoobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain20));
			if (base) do_fabao_downgrade(ob, base);
		}
		return false;
	}
}


/**   元神
 * \brief 元神升级物品
 *
 * \param user:元神升级物品的用户
 * \param ob: 元神升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::YUANSHEN_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUYUANSHENObjectB *base = uYUANSHENobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain22 + 1));
	if(!base) return false;
	
	Zebra::logger->debug("元神进阶成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_YUANSHEN_upgrade(ob, base);
		return true;
	}
    else 
    {
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = uYUANSHENobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain22));
			if (base) do_YUANSHEN_downgrade(ob, base);
		}
		return false;
	}
}


/**   元神
 * \brief 元神宝石升级物品
 *
 * \param user:元神升级物品的用户
 * \param ob: 元神升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::YSBS_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUYSBSObjectB *base = uYSBSobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain23 + 1));
	if(!base) return false;
	
	Zebra::logger->debug("元神进阶成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_YSBS_upgrade(ob, base);
		return true;
	}
    else 
    {
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = uYSBSobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain23));
			if (base) do_YSBS_downgrade(ob, base);
		}
		return false;
	}
}



/**   十二星宿
 * \brief 十二星宿升级物品
 *
 * \param user:十二星宿升级物品的用户
 * \param ob: 十二星宿升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::SEXX_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUSEXXObjectB *base = usexxobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain21 + 1));
	if(!base) return false;
	
	Zebra::logger->debug("十二星宿进阶成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_SEXX_upgrade(ob, base);
		return true;
	}
    else 
    {
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = usexxobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain21));
			if (base) do_SEXX_downgrade(ob, base);
		}
		return false;
	}
}


/**   
 * \brief 马牌精炼升级物品
 *
 * \param user:马牌精炼升级物品的用户
 * \param ob: 马牌精炼升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::refine_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUrefineObjectB *base = urefineobjectbm.get(ob->data.dwObjectID+100000*(ob->data.itemrefine + 1));
	if(!base) return false;
	
	Zebra::logger->debug("[马牌精炼]马牌精炼成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_refine_upgrade(ob, base);
		return true;
	}
    else 
    {
	    //升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = urefineobjectbm.get(ob->data.dwObjectID+100000*(ob->data.itemrefine));
			if (base) do_refine_downgrade(ob, base);
		}
		return false;
	}
}

/**   
 * \brief 璀璨镶嵌升级物品
 *
 * \param user:装备升级物品的用户
 * \param ob: 装备升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::bright_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUbrightObjectB *base = ubrightobjectbm.get(ob->data.dwObjectID+100000*(ob->data.itemkscuican + 1));
	if(!base) return false;
	
	Zebra::logger->debug("[璀璨宝石]装备璀璨镶嵌成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_bright_upgrade(ob, base);
		return true;
	}
    else 
    {
	    //升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = ubrightobjectbm.get(ob->data.dwObjectID+100000*(ob->data.itemkscuican));
			if (base) do_bright_downgrade(ob, base);
		}
		return false;
	}
}

/**   
 * \brief 神斧镶嵌升级物品
 *
 * \param user:装备升级物品的用户
 * \param ob: 装备升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::xuanhua_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUxuanhuaObjectB *base = uxuanhuaobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain32 + 1));
	if(!base) return false;
	
	Zebra::logger->debug("[神斧宝石]装备神斧镶嵌成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_xuanhua_upgrade(ob, base);
		return true;
	}
    else 
    {
	    //升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = uxuanhuaobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain32));
			if (base) do_xuanhua_downgrade(ob, base);
		}
		return false;
	}
}

/**   
 * \brief 黑曜镶嵌升级物品
 *
 * \param user:装备升级物品的用户
 * \param ob: 装备升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::heiyao_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUheiyaoObjectB *base = uheiyaoobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain33 + 1));
	if(!base) return false;
	
	Zebra::logger->debug("[黑曜宝石]装备黑曜镶嵌成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_heiyao_upgrade(ob, base);
		return true;
	}
    else 
    {
	    //升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = uheiyaoobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain33));
			if (base) do_heiyao_downgrade(ob, base);
		}
		return false;
	}
}


/**   
 * \brief 精金镶嵌升级物品
 *
 * \param user:装备升级物品的用户
 * \param ob: 装备升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::jingjin_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUjingjinObjectB *base = ujingjinobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain34 + 1));
	if(!base) return false;
	
	Zebra::logger->debug("[精金宝石]装备精金镶嵌成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_jingjin_upgrade(ob, base);
		return true;
	}
    else 
    {
	    //升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = ujingjinobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain34));
			if (base) do_jingjin_downgrade(ob, base);
		}
		return false;
	}
}


/**   
 * \brief 金箍咒强化物品
 *
 * \param user:金箍咒强化物品的用户
 * \param ob: 金箍咒强化物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::jgzqh_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUjgzqhObjectB *base = ujgzqhobjectbm.get(ob->data.dwObjectID+10000*(ob->data.syxqgrade21 + 1)); //次数
	if(!base) return false;

	if (ob->data.syxqgrade21 < 1001)  //等级
	{
	   Zebra::logger->debug("金箍咒强化升%d星机率为%d",ob->data.syxqgrade21+1,base->odds + extra_odds);
	   if (zMisc::selectByPercent(base->odds + extra_odds)) 
	   {
		   do_jgzqh_upgrade(ob, base);
		  
		   return true;
	   }
    }

    else 
    {
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = ujgzqhobjectbm.get(ob->data.dwObjectID+10000*(ob->data.syxqgrade21)); //次数
			if (base) do_jgzqh_downgrade(ob, base);
		}
		return false;
	}
}
//---------------------------------------------------------------------------------------------------------
/**   
 * \brief 金箍咒字纹物品
 *
 * \param user:金箍咒字纹物品的用户
 * \param ob: 金箍咒字纹物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::jgzzw_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUjgzzwObjectB *base = ujgzzwobjectbm.get(ob->data.dwObjectID+10000*(ob->data.syxqgrade22 + 1)); //次数
	if(!base) return false;
	
	Zebra::logger->debug("金箍咒字纹成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_jgzzw_upgrade(ob, base);
		return true;
	}
    else 
    {
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = ujgzzwobjectbm.get(ob->data.dwObjectID+10000*(ob->data.syxqgrade22)); //次数
			if (base) do_jgzzw_downgrade(ob, base);
		}
		return false;
	}
}
//---------------------------------------------------------------------------------------------------------
/**   
 * \brief 金箍咒淬炼物品
 *
 * \param user:金箍咒淬炼物品的用户
 * \param ob: 金箍咒淬炼物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::jgzcl_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUjgzclObjectB *base = ujgzclobjectbm.get(ob->data.dwObjectID+10000*(ob->data.syxqgrade23 + 1)); //次数
	if(!base) return false;
	
	Zebra::logger->debug("金箍咒淬炼成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_jgzcl_upgrade(ob, base);
		return true;
	}
    else 
    {
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = ujgzclobjectbm.get(ob->data.dwObjectID+10000*(ob->data.syxqgrade23)); //次数
			if (base) do_jgzcl_downgrade(ob, base);
		}
		return false;
	}
}
//---------------------------------------------------------------------------------------------------------
/**   
 * \brief 寒冰神炼物品
 *
 * \param user:寒冰神炼物品的用户
 * \param ob: 寒冰神炼物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::hbsl_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{		
	zUhbslObjectB *base = uhbslobjectbm.get(ob->data.dwObjectID+10000*(ob->data.syxqgrade24+1)); //次数
	if(!base) return false;

	if (ob->data.syxqgrade24 < 1001)  //等级
	{
	   Zebra::logger->debug("寒冰神炼升%d星机率为%d",ob->data.syxqgrade24+1,base->odds + extra_odds);
	   if (zMisc::selectByPercent(base->odds + extra_odds)) 
	   {
		   do_hbsl_upgrade(ob, base);
		  
		   return true;
	   }
    }
    else 
    {
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = uhbslobjectbm.get(ob->data.dwObjectID+10000*(ob->data.syxqgrade24)); //次数
			if (base) do_hbsl_downgrade(ob, base);
		}
		return false;
	}
}
//---------------------------------------------------------------------------------------------------------
/**   
 * \brief 寒冰字纹物品
 *
 * \param user:寒冰字纹物品的用户
 * \param ob: 寒冰字纹物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::hbzw_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUhbzwObjectB *base = uhbzwobjectbm.get(ob->data.dwObjectID+10000*(ob->data.syxqgrade25 + 1)); //次数
	if(!base) return false;
	
	Zebra::logger->debug("寒冰字纹成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_hbzw_upgrade(ob, base);
		return true;
	}

    else 
    {
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = uhbzwobjectbm.get(ob->data.dwObjectID+10000*(ob->data.syxqgrade25)); //次数
			if (base) do_hbzw_downgrade(ob, base);
		}
		return false;
	}
}
//---------------------------------------------------------------------------------------------------------
/**   
 * \brief 寒冰纹章物品
 *
 * \param user:寒冰纹章物品的用户
 * \param ob: 寒冰纹章物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::hbwz_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUhbwzObjectB *base = uhbwzobjectbm.get(ob->data.dwObjectID+10000*(ob->data.syxqgrade26 + 1)); //次数
	if(!base) return false;
	
	Zebra::logger->debug("寒冰纹章成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_hbwz_upgrade(ob, base);
		return true;
	}
    else 
    {
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = uhbwzobjectbm.get(ob->data.dwObjectID+10000*(ob->data.syxqgrade26)); //次数
			if (base) do_hbwz_downgrade(ob, base);
		}
		return false;
	}
}
//---------------------------------------------------------------------------------------------------------
/**   
 * \brief 图鉴进阶升级物品
 *
 * \param user:图鉴进阶升级物品的用户
 * \param ob: 图鉴进阶升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::handbook_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{	
	zUhandbookObjectB *base = uhandbookobjectbm.get(ob->data.dwObjectID+10000*(ob->data.handbooklvg+1));
	if(!base) return false;
	
	Zebra::logger->debug("[图鉴进阶]图鉴进阶成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_handbook_upgrade(ob, base);
		return true;
	}
    else 
    {
	    //升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = uhandbookobjectbm.get(ob->data.dwObjectID+10000*(ob->data.handbooklvg));
			if (base) do_handbook_downgrade(ob, base);
		}
		return false;
	}
}


/**   
 * \brief 翅膀升级物品
 *
 * \param user:翅膀升级物品的用户
 * \param ob: 翅膀升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::angel_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
		
	zUangelObjectB *base = uangelobjectbm.get(ob->data.dwObjectID+10000*(ob->data.retain4 + 1));
	if(!base) return false;
	
	Zebra::logger->debug("天使之翼升级成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_angel_upgrade(ob, base);
		return true;
	}
    else 
    {
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = uangelobjectbm.get(ob->data.dwObjectID+10000*(ob->data.retain4));
			if (base) do_angel_downgrade(ob, base);
		}
		return false;
	}
}

/**
* \brief 龙槽升级物品
*
* \param user:龙槽升级物品的用户
* \param ob: 龙槽升级物品
* \param extra_odds: 额外成功率
* \return 升级成功返回true,失败返回false
*/
bool Upgrade::dtrough_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{
	zUdtroughObjectB *base = udtroughobjectbm.get(ob->data.dwObjectID + 100000 * (ob->data.dtrough + 1));
	if (!base) return false;

	Zebra::logger->debug("装备升%d龙槽成功机率为%d", ob->data.dtrough + 1, base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds))
	{
		do_dtrough_upgrade(ob, base);
		return true;
	}
	else
	{
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = udtroughobjectbm.get(ob->data.dwObjectID + 100000 * (ob->data.dtrough));
			if (base) do_dtrough_downgrade(ob, base);
		}
		return false;
	}
}

/**   
 * \brief 龙星升级物品
 *
 * \param user:龙星升级物品的用户
 * \param ob: 龙星升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::drastar_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{	
	zUdrastarObjectB *base = udrastarobjectbm.get(ob->data.dwObjectID+100000*(ob->data.drastar+1));
	if(!base) return false;
	
	Zebra::logger->debug("装备升%d龙星成功机率为%d",ob->data.drastar+1,base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_drastar_upgrade(ob, base);
		return true;
	}
    else 
    {
		//8级升9级失败降到0级，其他情况还是降2级
		int down;
		if(zMisc::selectByPercent(50))
			down = 0;
		else
			down = ob->data.drastar >= 8?ob->data.drastar:2;

		for( int i=0;i<down;i++)
		{
			base = udrastarobjectbm.get(ob->data.dwObjectID+100000*(ob->data.drastar));
			if (base) do_drastar_downgrade(ob, base);
		}
		return false;
	}
}


/**   
 * \brief 龙凤吟升级物品
 *
 * \param user:龙凤吟升级物品的用户
 * \param ob: 龙凤吟升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::longfengying_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{	
	zUlongfengyingObjectB *base = ulongfengyingobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain24+1));
	if(!base) return false;
	
	Zebra::logger->debug("装备升%d龙凤吟成功机率为%d",ob->data.retain24+1,base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
    {
		do_longfengying_upgrade(ob, base);
		return true;
	}
    else 
    {
		int down;
		if(zMisc::selectByPercent(50))
			down = 0;
		else
			down = ob->data.retain24 >= 8?2:1;

		for( int i=0;i<down;i++)
		{
			base = ulongfengyingobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain24));
			if (base) do_longfengying_downgrade(ob, base);
		}
		return false;
	}
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**   
 * \brief 神兵升级物品
 *
 * \param user:升级物品的用户
 * \param ob: 升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::magical_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{

	zMagicalObjectB *base = magicalobjectbm.get(ob->data.dwObjectID+10000*(ob->data.retain5 + 1));
	if(!base) return false;

	Zebra::logger->debug("神兵升级成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
	{
		do_magical_upgrade(ob, base);
		return true;
	}
	else 
	{
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = magicalobjectbm.get(ob->data.dwObjectID+10000*(ob->data.retain5));
			if (base) do_magical_downgrade(ob, base);
		}
		return false;
	}
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**   
 * \brief 神兽升级物品
 *
 * \param user:升级物品的用户
 * \param ob: 升级物品
 * \param extra_odds: 额外成功率
 * \return 升级成功返回true,失败返回false
 */
bool Upgrade::mythical_upgrade(SceneUser& user, zObject* ob, int extra_odds)
{

	zMythicalObjectB *base = mythicalobjectbm.get(ob->data.dwObjectID+10000*(ob->data.retain6 + 1));
	if(!base) return false;

	Zebra::logger->debug("神兽升级成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
	{
		do_mythical_upgrade(ob, base);
		return true;
	}
	else 
	{
		//升级不掉级
		int down = 0;

		for( int i=0;i<down;i++)
		{
			base = mythicalobjectbm.get(ob->data.dwObjectID+10000*(ob->data.retain6));
			if (base) do_mythical_downgrade(ob, base);
		}
		return false;
	}
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

//sky  升级装备物品
bool Upgrade::equip(SceneUser& user, zObject* ob, int extra_odds)
{
	zEquipObjectB *base = equipobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain7+1));
	if(!base) return false;

	Zebra::logger->debug("装备升级成功机率为%d", base->odds + extra_odds);
	if (zMisc::selectByPercent(base->odds + extra_odds)) 
	{
		if (&user) 
		{ 
			//soke 70级以上装备升级 - 甲（男）
			if (ob->data.dwObjectID == 10007)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;				
				sprintf(ob->data.strName, "[未改造]雷神甲(男)");
			}
			else if (ob->data.dwObjectID == 10008)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神甲(男)");
			}
			else if (ob->data.dwObjectID == 10009)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔甲(男)");
			}
			else if (ob->data.dwObjectID == 10010)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔圣甲(男)");
			}
			else if (ob->data.dwObjectID == 10011)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔圣甲(男)");
			}
			else if (ob->data.dwObjectID == 10012)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥圣甲(男)");
			}
			else if (ob->data.dwObjectID == 10013)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武圣甲(男)");
			}
			else if (ob->data.dwObjectID == 10014)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀帝甲(男)");
			}
			else if (ob->data.dwObjectID == 10015)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎帝甲(男)");
			}
			else if (ob->data.dwObjectID == 10016)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙帝甲(男)");
			}
			else if (ob->data.dwObjectID == 10017)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇仙甲(男)");
			}
			else if (ob->data.dwObjectID == 10018)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊仙甲(男)");
			}
			else if (ob->data.dwObjectID == 10019)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤帝甲(男)");
			}
			else if (ob->data.dwObjectID == 10020)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天帝甲(男)");
			}
			else if (ob->data.dwObjectID == 10021)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕帝甲(男)");
			}
			else if (ob->data.dwObjectID == 10022)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊帝甲(男)");
			}
			else if (ob->data.dwObjectID == 10023)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄帝甲(男)");
			}
			else if (ob->data.dwObjectID == 10024)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者帝甲(男)");
			}
			else if (ob->data.dwObjectID == 10025)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神帝甲(男)");
			}
			else if (ob->data.dwObjectID == 10026)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒帝甲(男)");
			}
			else if (ob->data.dwObjectID == 10027)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰帝甲(男)");
			}
			else if (ob->data.dwObjectID == 10028)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]永恒帝甲(男)");
			}
			else if (ob->data.dwObjectID == 10029)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
			//soke 70级以上装备升级 - 甲（女）
			if (ob->data.dwObjectID == 10057)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]雷神甲(女)");
			}
			else if (ob->data.dwObjectID == 10058)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神甲(女)");
			}
			else if (ob->data.dwObjectID == 10059)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔甲(女)");
			}
			else if (ob->data.dwObjectID == 10060)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔圣甲(女)");
			}
			else if (ob->data.dwObjectID == 10061)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔圣甲(女)");
			}
			else if (ob->data.dwObjectID == 10062)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥圣甲(女)");
			}
			else if (ob->data.dwObjectID == 10063)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武圣甲(女)");
			}
			else if (ob->data.dwObjectID == 10064)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀帝甲(女)");
			}
			else if (ob->data.dwObjectID == 10065)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎帝甲(女)");
			}
			else if (ob->data.dwObjectID == 10066)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙帝甲(女)");
			}
			else if (ob->data.dwObjectID == 10067)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇仙甲(女)");
			}
			else if (ob->data.dwObjectID == 10068)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊仙甲(女)");
			}
			else if (ob->data.dwObjectID == 10069)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤帝甲(女)");
			}
			else if (ob->data.dwObjectID == 10070)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天帝甲(女)");
			}
			else if (ob->data.dwObjectID == 10071)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕帝甲(女)");
			}
			else if (ob->data.dwObjectID == 10072)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊帝甲(女)");
			}
			else if (ob->data.dwObjectID == 10073)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄帝甲(女)");
			}
			else if (ob->data.dwObjectID == 10074)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者帝甲(女)");
			}
			else if (ob->data.dwObjectID == 10075)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神帝甲(女)");
			}
			else if (ob->data.dwObjectID == 10076)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒帝甲(女)");
			}
			else if (ob->data.dwObjectID == 10077)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰帝甲(女)");
			}
			else if (ob->data.dwObjectID == 10078)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]永恒帝甲(女)");
			}
			else if (ob->data.dwObjectID == 10079)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
		   //soke 70级以上装备升级 - 铠（男）
			if (ob->data.dwObjectID == 10107)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]雷神铠(男)");
			}
			else if (ob->data.dwObjectID == 10108)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神铠(男)");
			}
			else if (ob->data.dwObjectID == 10109)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔铠(男)");
			}
			else if (ob->data.dwObjectID == 10110)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔圣铠(男)");
			}
			else if (ob->data.dwObjectID == 10111)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔圣铠(男)");
			}
			else if (ob->data.dwObjectID == 10112)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥圣铠(男)");
			}
			else if (ob->data.dwObjectID == 10113)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武圣铠(男)");
			}
			else if (ob->data.dwObjectID == 10114)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀帝铠(男)");
			}
			else if (ob->data.dwObjectID == 10115)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎帝铠(男)");
			}
			else if (ob->data.dwObjectID == 10116)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙帝铠(男)");
			}
			else if (ob->data.dwObjectID == 10117)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇仙铠(男)");
			}
			else if (ob->data.dwObjectID == 10118)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊仙铠(男)");
			}
			else if (ob->data.dwObjectID == 10119)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤帝铠(男)");
			}
			else if (ob->data.dwObjectID == 10120)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天帝铠(男)");
			}
			else if (ob->data.dwObjectID == 10121)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕帝铠(男)");
			}
			else if (ob->data.dwObjectID == 10122)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊帝铠(男)");
			}
			else if (ob->data.dwObjectID == 10123)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄帝铠(男)");
			}
			else if (ob->data.dwObjectID == 10124)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者帝铠(男)");
			}
			else if (ob->data.dwObjectID == 10125)
			{
				ob->data.dwObjectID += 1;  
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神帝铠(男)");
			}
			else if (ob->data.dwObjectID == 10126)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒帝铠(男)");
			}
			else if (ob->data.dwObjectID == 10127)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰帝铠(男)");
			}
			else if (ob->data.dwObjectID == 10128)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]永恒帝铠(男)");
			}
			else if (ob->data.dwObjectID == 10129)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
			//soke 70级以上装备升级 - 铠（女）
			if (ob->data.dwObjectID == 10157)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]雷神铠(女)");
			}
			else if (ob->data.dwObjectID == 10158)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神铠(女)");
			}
			else if (ob->data.dwObjectID == 10159)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔铠(女)");
			}
			else if (ob->data.dwObjectID == 10160)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔圣铠(女)");
			}
			else if (ob->data.dwObjectID == 10161)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔圣铠(女)");
			}
			else if (ob->data.dwObjectID == 10162)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥圣铠(女)");
			}
			else if (ob->data.dwObjectID == 10163)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武圣铠(女)");
			}
			else if (ob->data.dwObjectID == 10164)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀帝铠(女)");
			}
			else if (ob->data.dwObjectID == 10165)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎帝铠(女)");
			}
			else if (ob->data.dwObjectID == 10166)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙帝铠(女)");
			}
			else if (ob->data.dwObjectID == 10167)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇仙铠(女)");
			}
			else if (ob->data.dwObjectID == 10168)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊仙铠(女)");
			}
			else if (ob->data.dwObjectID == 10169)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤帝铠(女)");
			}
			else if (ob->data.dwObjectID == 10170)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天帝铠(女)");
			}
			else if (ob->data.dwObjectID == 10171)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕帝铠(女)");
			}
			else if (ob->data.dwObjectID == 10172)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊帝铠(女)");
			}
			else if (ob->data.dwObjectID == 10173)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄帝铠(女)");
			}
			else if (ob->data.dwObjectID == 10174)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者帝铠(女)");
			}
			else if (ob->data.dwObjectID == 10175)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神帝铠(女)");
			}
			else if (ob->data.dwObjectID == 10176)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒帝铠(女)");
			}
		    else if (ob->data.dwObjectID == 10177)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰帝铠(女)");
			}
		    else if (ob->data.dwObjectID == 10178)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]永恒帝铠(女)");
			}
			else if (ob->data.dwObjectID == 10179)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
			//soke 70级以上装备升级 - 刀
			if (ob->data.dwObjectID == 10207)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮刀");
			}
			else if (ob->data.dwObjectID == 10208)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神刀");
			}
			else if (ob->data.dwObjectID == 10209)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔刀");
			}
			else if (ob->data.dwObjectID == 10210)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔刀");
			}
			else if (ob->data.dwObjectID == 10211)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔刀");
			}
			else if (ob->data.dwObjectID == 10212)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥刀");
			}
			else if (ob->data.dwObjectID == 10213)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武刀");
			}
			else if (ob->data.dwObjectID == 10214)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀刀");
			}
			else if (ob->data.dwObjectID == 10215)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎刀");
			}
			else if (ob->data.dwObjectID == 10216)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙刀");
			}
			else if (ob->data.dwObjectID == 10217)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇刀");
			}
			else if (ob->data.dwObjectID == 10218)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊刀");
			}
			else if (ob->data.dwObjectID == 10219)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤刀");
			}
			else if (ob->data.dwObjectID == 10220)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天刀");
			}
			else if (ob->data.dwObjectID == 10221)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕刀");
			}
			else if (ob->data.dwObjectID == 10222)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊刀");
			}
			else if (ob->data.dwObjectID == 10223)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄刀");
			}
			else if (ob->data.dwObjectID == 10224)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者刀");
			}
			else if (ob->data.dwObjectID == 10225)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神刀");
			}
			else if (ob->data.dwObjectID == 10226)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒刀");
			}
			else if (ob->data.dwObjectID == 10227)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰刀");
			}
			else if (ob->data.dwObjectID == 10228)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临刀");
			}
			else if (ob->data.dwObjectID == 10229)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
			//soke 70级以上装备升级 - 仗
			if (ob->data.dwObjectID == 10257)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮仗");
			}
			else if (ob->data.dwObjectID == 10258)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神仗");
			}
			else if (ob->data.dwObjectID == 10259)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔仗");
			}
			else if (ob->data.dwObjectID == 10260)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔仗");
			}
			else if (ob->data.dwObjectID == 10261)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔仗");
			}
			else if (ob->data.dwObjectID == 10262)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥仗");
			}
			else if (ob->data.dwObjectID == 10263)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武仗");
			}
			else if (ob->data.dwObjectID == 10264)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀仗");
			}
			else if (ob->data.dwObjectID == 10265)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎仗");
			}
			else if (ob->data.dwObjectID == 10266)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙仗");
			}
			else if (ob->data.dwObjectID == 10267)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇仗");
			}
			else if (ob->data.dwObjectID == 10268)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊仗");
			}
			else if (ob->data.dwObjectID == 10269)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤仗");
			}
			else if (ob->data.dwObjectID == 10270)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天仗");
			}
			else if (ob->data.dwObjectID == 10271)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕仗");
			}
			else if (ob->data.dwObjectID == 10272)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊仗");
			}
			else if (ob->data.dwObjectID == 10273)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄仗");
			}
			else if (ob->data.dwObjectID == 10274)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者仗");
			}
			else if (ob->data.dwObjectID == 10275)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神仗");
			}
			else if (ob->data.dwObjectID == 10276)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒仗");
			}
			else if (ob->data.dwObjectID == 10277)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰仗");
			}
			else if (ob->data.dwObjectID == 10278)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临仗");
			}
			else if (ob->data.dwObjectID == 10279)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
            //soke 70级以上装备升级 - 弓
			if (ob->data.dwObjectID == 10307)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮弓");
			}
			else if (ob->data.dwObjectID == 10308)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神弓");
			}
			else if (ob->data.dwObjectID == 10309)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔弓");
			}
			else if (ob->data.dwObjectID == 10310)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔弓");
			}
			else if (ob->data.dwObjectID == 10311)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔弓");
			}
			else if (ob->data.dwObjectID == 10312)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥弓");
			}
			else if (ob->data.dwObjectID == 10313)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武弓");
			}
			else if (ob->data.dwObjectID == 10314)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀弓");
			}
			else if (ob->data.dwObjectID == 10315)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎弓");
			}
			else if (ob->data.dwObjectID == 10316)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙弓");
			}
			else if (ob->data.dwObjectID == 10317)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇弓");
			}
			else if (ob->data.dwObjectID == 10318)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊弓");
			}
			else if (ob->data.dwObjectID == 10319)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤弓");
			}
			else if (ob->data.dwObjectID == 10320)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天弓");
			}
			else if (ob->data.dwObjectID == 10321)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕弓");
			}
			else if (ob->data.dwObjectID == 10322)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊弓");
			}
			else if (ob->data.dwObjectID == 10323)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄弓");
			}
			else if (ob->data.dwObjectID == 10324)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者弓");
			}
			else if (ob->data.dwObjectID == 10325)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神弓");
			}
			else if (ob->data.dwObjectID == 10326)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒弓");
			}
			else if (ob->data.dwObjectID == 10327)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰弓");
			}
			else if (ob->data.dwObjectID == 10328)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临弓");
			}
			else if (ob->data.dwObjectID == 10329)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}

		  //soke 70级以上装备升级 - 扇
			if (ob->data.dwObjectID == 10357)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮扇");
			}
			else if (ob->data.dwObjectID == 10358)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神扇");
			}
			else if (ob->data.dwObjectID == 10359)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔扇");
			}
			else if (ob->data.dwObjectID == 10360)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔扇");
			}
			else if (ob->data.dwObjectID == 10361)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔扇");
			}
			else if (ob->data.dwObjectID == 10362)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥扇");
			}
			else if (ob->data.dwObjectID == 10363)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武扇");
			}
			else if (ob->data.dwObjectID == 10364)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀扇");
			}
			else if (ob->data.dwObjectID == 10365)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎扇");
			}
			else if (ob->data.dwObjectID == 10366)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙扇");
			}
			else if (ob->data.dwObjectID == 10367)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇扇");
			}
			else if (ob->data.dwObjectID == 10368)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊扇");
			}
			else if (ob->data.dwObjectID == 10369)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤扇");
			}
			else if (ob->data.dwObjectID == 10370)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天扇");
			}
			else if (ob->data.dwObjectID == 10371)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕扇");
			}
			else if (ob->data.dwObjectID == 10372)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊扇");
			}
			else if (ob->data.dwObjectID == 10373)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄扇");
			}
			else if (ob->data.dwObjectID == 10374)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者扇");
			}
			else if (ob->data.dwObjectID == 10375)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神扇");
			}
			else if (ob->data.dwObjectID == 10376)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒扇");
			}
			else if (ob->data.dwObjectID == 10377)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰扇");
			}
			else if (ob->data.dwObjectID == 10378)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临扇");
			}
			else if (ob->data.dwObjectID == 10379)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
           //soke 70级以上装备升级 - 棍
			if (ob->data.dwObjectID == 10407)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮棍");
			}
			else if (ob->data.dwObjectID == 10408)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神棍");
			}
			else if (ob->data.dwObjectID == 10409)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔棍");
			}
			else if (ob->data.dwObjectID == 10410)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔棍");
			}
			else if (ob->data.dwObjectID == 10411)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔棍");
			}
			else if (ob->data.dwObjectID == 10412)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥棍");
			}
			else if (ob->data.dwObjectID == 10413)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武棍");
			}
			else if (ob->data.dwObjectID == 10414)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀棍");
			}
			else if (ob->data.dwObjectID == 10415)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎棍");
			}
			else if (ob->data.dwObjectID == 10416)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙棍");
			}
			else if (ob->data.dwObjectID == 10417)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇棍");
			}
			else if (ob->data.dwObjectID == 10418)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊棍");
			}
			else if (ob->data.dwObjectID == 10419)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤棍");
			}
			else if (ob->data.dwObjectID == 10420)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天棍");
			}
			else if (ob->data.dwObjectID == 10421)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕棍");
			}
			else if (ob->data.dwObjectID == 10422)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊棍");
			}
			else if (ob->data.dwObjectID == 10423)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄棍");
			}
			else if (ob->data.dwObjectID == 10424)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者棍");
			}
			else if (ob->data.dwObjectID == 10425)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神棍");
			}
			else if (ob->data.dwObjectID == 10426)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒棍");
			}
			else if (ob->data.dwObjectID == 10427)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰棍");
			}
			else if (ob->data.dwObjectID == 10428)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临棍");
			}
			else if (ob->data.dwObjectID == 10429)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}       
           //soke 70级以上装备升级 - 剑(物)
			if (ob->data.dwObjectID == 10457)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮剑(物)");
			}
			else if (ob->data.dwObjectID == 10458)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神剑(物)");
			}
			else if (ob->data.dwObjectID == 10459)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔剑(物)");
			}
			else if (ob->data.dwObjectID == 10460)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔剑(物)");
			}
			else if (ob->data.dwObjectID == 10461)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔剑(物)");
			}
			else if (ob->data.dwObjectID == 10462)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥剑(物)");
			}
			else if (ob->data.dwObjectID == 10463)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武剑(物)");
			}
			else if (ob->data.dwObjectID == 10464)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀剑(物)");
			}
			else if (ob->data.dwObjectID == 10465)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎剑(物)");
			}
			else if (ob->data.dwObjectID == 10466)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙剑(物)");
			}
			else if (ob->data.dwObjectID == 10467)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇剑(物)");
			}
			else if (ob->data.dwObjectID == 10468)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊剑(物)");
			}
			else if (ob->data.dwObjectID == 10469)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤剑(物)");
			}
			else if (ob->data.dwObjectID == 10470)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天剑(物)");
			}
			else if (ob->data.dwObjectID == 10471)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕剑(物)");
			}
			else if (ob->data.dwObjectID == 10472)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊剑(物)");
			}
			else if (ob->data.dwObjectID == 10473)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄剑(物)");
			}
			else if (ob->data.dwObjectID == 10474)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者剑(物)");
			}
			else if (ob->data.dwObjectID == 10475)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神剑(物)");
			}
			else if (ob->data.dwObjectID == 10476)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒剑(物)");
			}
			else if (ob->data.dwObjectID == 10477)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰剑(物)");
			}
			else if (ob->data.dwObjectID == 10478)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临剑(物)");
			}
			else if (ob->data.dwObjectID == 10479)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
            //soke 70级以上装备升级 - 剑(魔)
			if (ob->data.dwObjectID == 10507)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮剑(魔)");
			}
			else if (ob->data.dwObjectID == 10508)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神剑(魔)");
			}
			else if (ob->data.dwObjectID == 10509)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔剑(魔)");
			}
			else if (ob->data.dwObjectID == 10510)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔剑(魔)");
			}
			else if (ob->data.dwObjectID == 10511)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔剑(魔)");
			}
			else if (ob->data.dwObjectID == 10512)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥剑(魔)");
			}
			else if (ob->data.dwObjectID == 10513)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武剑(魔)");
			}
			else if (ob->data.dwObjectID == 10514)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀剑(魔)");
			}
			else if (ob->data.dwObjectID == 10515)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎剑(魔)");
			}
			else if (ob->data.dwObjectID == 10516)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙剑(魔)");
			}
			else if (ob->data.dwObjectID == 10517)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇剑(魔)");
			}
			else if (ob->data.dwObjectID == 10518)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊剑(魔)");
			}
			else if (ob->data.dwObjectID == 10519)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤剑(魔)");
			}
			else if (ob->data.dwObjectID == 10520)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天剑(魔)");
			}
			else if (ob->data.dwObjectID == 10521)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕剑(魔)");
			}
			else if (ob->data.dwObjectID == 10522)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊剑(魔)");
			}
			else if (ob->data.dwObjectID == 10523)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄剑(魔)");
			}
			else if (ob->data.dwObjectID == 10524)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者剑(魔)");
			}
			else if (ob->data.dwObjectID == 10525)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神剑(魔)");
			}
			else if (ob->data.dwObjectID == 10526)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒剑(魔)");
			}
			else if (ob->data.dwObjectID == 10527)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰剑(魔)");
			}
			else if (ob->data.dwObjectID == 10528)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临剑(魔)");
			}
			else if (ob->data.dwObjectID == 10529)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
             //soke 70级以上装备升级 - 刃(魔)
			if (ob->data.dwObjectID == 10557)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮刃(魔)");
			}
			else if (ob->data.dwObjectID == 10558)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神刃(魔)");
			}
			else if (ob->data.dwObjectID == 10559)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔刃(魔)");
			}
			else if (ob->data.dwObjectID == 10560)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔刃(魔)");
			}
			else if (ob->data.dwObjectID == 10561)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔刃(魔)");
			}
			else if (ob->data.dwObjectID == 10562)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥刃(魔)");
			}
			else if (ob->data.dwObjectID == 10563)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武刃(魔)");
			}
			else if (ob->data.dwObjectID == 10564)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀刃(魔)");
			}
			else if (ob->data.dwObjectID == 10565)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎刃(魔)");
			}
			else if (ob->data.dwObjectID == 10566)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙刃(魔)");
			}
			else if (ob->data.dwObjectID == 10567)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇刃(魔)");
			}
			else if (ob->data.dwObjectID == 10568)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊刃(魔)");
			}
			else if (ob->data.dwObjectID == 10569)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤刃(魔)");
			}
			else if (ob->data.dwObjectID == 10570)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天刃(魔)");
			}
			else if (ob->data.dwObjectID == 10571)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕刃(魔)");
			}
			else if (ob->data.dwObjectID == 10572)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊刃(魔)");
			}
			else if (ob->data.dwObjectID == 10573)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄刃(魔)");
			}
			else if (ob->data.dwObjectID == 10574)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者刃(魔)");
			}
			else if (ob->data.dwObjectID == 10575)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神刃(魔)");
			}
			else if (ob->data.dwObjectID == 10576)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒刃(魔)");
			}
			else if (ob->data.dwObjectID == 10577)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰刃(魔)");
			}
			else if (ob->data.dwObjectID == 10578)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临刃(魔)");
			}
			else if (ob->data.dwObjectID == 10579)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
            //soke 70级以上装备升级 - 枪(物)
			if (ob->data.dwObjectID == 10607)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮枪(物)");
			}
			else if (ob->data.dwObjectID == 10608)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神枪(物)");
			}
			else if (ob->data.dwObjectID == 10609)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔枪(物)");
			}
			else if (ob->data.dwObjectID == 10610)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔枪(物)");
			}
			else if (ob->data.dwObjectID == 10611)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔枪(物)");
			}
			else if (ob->data.dwObjectID == 10612)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥枪(物)");
			}
			else if (ob->data.dwObjectID == 10613)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武枪(物)");
			}
			else if (ob->data.dwObjectID == 10614)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀枪(物)");
			}
			else if (ob->data.dwObjectID == 10615)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎枪(物)");
			}
			else if (ob->data.dwObjectID == 10616)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙枪(物)");
			}
			else if (ob->data.dwObjectID == 10617)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇枪(物)");
			}
			else if (ob->data.dwObjectID == 10618)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊枪(物)");
			}
			else if (ob->data.dwObjectID == 10619)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤枪(物)");
			}
			else if (ob->data.dwObjectID == 10620)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天枪(物)");
			}
			else if (ob->data.dwObjectID == 10621)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕枪(物)");
			}
			else if (ob->data.dwObjectID == 10622)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊枪(物)");
			}
			else if (ob->data.dwObjectID == 10623)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄枪(物)");
			}
			else if (ob->data.dwObjectID == 10624)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者枪(物)");
			}
			else if (ob->data.dwObjectID == 10625)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神枪(物)");
			}
			else if (ob->data.dwObjectID == 10626)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒枪(物)");
			}
			else if (ob->data.dwObjectID == 10627)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰枪(物)");
			}
			else if (ob->data.dwObjectID == 10628)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临枪(物)");
			}
			else if (ob->data.dwObjectID == 10629)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
            //soke 70级以上装备升级 - 枪(魔)
			if (ob->data.dwObjectID == 10657)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮枪(魔)");
			}
			else if (ob->data.dwObjectID == 10658)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神枪(魔)");
			}
			else if (ob->data.dwObjectID == 10659)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔枪(魔)");
			}
			else if (ob->data.dwObjectID == 10660)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔枪(魔)");
			}
			else if (ob->data.dwObjectID == 10661)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔枪(魔)");
			}
			else if (ob->data.dwObjectID == 10662)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥枪(魔)");
			}
			else if (ob->data.dwObjectID == 10663)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武枪(魔)");
			}
			else if (ob->data.dwObjectID == 10664)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀枪(魔)");
			}
			else if (ob->data.dwObjectID == 10665)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎枪(魔)");
			}
			else if (ob->data.dwObjectID == 10666)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙枪(魔)");
			}
			else if (ob->data.dwObjectID == 10667)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇枪(魔)");
			}
			else if (ob->data.dwObjectID == 10668)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊枪(魔)");
			}
			else if (ob->data.dwObjectID == 10669)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤枪(魔)");
			}
			else if (ob->data.dwObjectID == 10670)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天枪(魔)");
			}
			else if (ob->data.dwObjectID == 10671)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕枪(魔)");
			}
			else if (ob->data.dwObjectID == 10672)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊枪(魔)");
			}
			else if (ob->data.dwObjectID == 10673)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄枪(魔)");
			}
			else if (ob->data.dwObjectID == 10674)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者枪(魔)");
			}
			else if (ob->data.dwObjectID == 10675)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神枪(魔)");
			}
			else if (ob->data.dwObjectID == 10676)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒枪(魔)");
			}
			else if (ob->data.dwObjectID == 10677)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰枪(魔)");
			}
			else if (ob->data.dwObjectID == 10678)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临枪(魔)");
			}
			else if (ob->data.dwObjectID == 10679)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
	        //soke 70级以上装备升级 - 盾
			if (ob->data.dwObjectID == 10757)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮盾");
			}
			else if (ob->data.dwObjectID == 10758)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神盾");
			}
			else if (ob->data.dwObjectID == 10759)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔盾");
			}
			else if (ob->data.dwObjectID == 10760)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔盾");
			}
			else if (ob->data.dwObjectID == 10761)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔盾");
			}
			else if (ob->data.dwObjectID == 10762)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥盾");
			}
			else if (ob->data.dwObjectID == 10763)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武盾");
			}
			else if (ob->data.dwObjectID == 10764)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀盾");
			}
			else if (ob->data.dwObjectID == 10765)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎盾");
			}
			else if (ob->data.dwObjectID == 10766)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙盾");
			}
			else if (ob->data.dwObjectID == 10767)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇盾");
			}
			else if (ob->data.dwObjectID == 10768)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊盾");
			}
			else if (ob->data.dwObjectID == 10769)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤盾");
			}
			else if (ob->data.dwObjectID == 10770)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天盾");
			}
			else if (ob->data.dwObjectID == 10771)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕盾");
			}
			else if (ob->data.dwObjectID == 10772)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊盾");
			}
			else if (ob->data.dwObjectID == 10773)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄盾");
			}
			else if (ob->data.dwObjectID == 10774)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者盾");
			}
			else if (ob->data.dwObjectID == 10775)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神盾");
			}
			else if (ob->data.dwObjectID == 10776)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒盾");
			}
			else if (ob->data.dwObjectID == 10777)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰盾");
			}
			else if (ob->data.dwObjectID == 10778)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临盾");
			}
			else if (ob->data.dwObjectID == 10779)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}

           	//soke 70级以上装备升级 - 战旗(法术)
			if (ob->data.dwObjectID == 12007)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12008)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12009)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜物战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12010)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血物战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12011)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天物战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12012)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12013)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12014)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12015)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12016)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12017)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12018)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12019)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12020)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12021)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12022)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12023)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12024)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12025)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12026)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12027)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12028)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临战旗(法术)");
			}
			else if (ob->data.dwObjectID == 12029)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}

	        //soke 70级以上装备升级 - 战旗(弓手)
			if (ob->data.dwObjectID == 12057)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12058)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12059)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12060)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12061)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12062)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12063)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12064)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12065)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12066)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12067)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12068)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12069)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12070)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12071)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12072)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12073)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12074)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12075)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12076)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12077)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12078)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临战旗(弓手)");
			}
			else if (ob->data.dwObjectID == 12079)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}

           	//soke 70级以上装备升级 - 战旗(仙术)
			if (ob->data.dwObjectID == 12107)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12108)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12109)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜物战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12110)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血物战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12111)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 121;
				sprintf(ob->data.strName, "[未改造]天物战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12112)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12113)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12114)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12115)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12116)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12117)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12118)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12119)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12120)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12121)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12122)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12123)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12124)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12125)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12126)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12127)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12128)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临战旗(仙术)");
			}
			else if (ob->data.dwObjectID == 12129)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}

	        //soke 70级以上装备升级 - 战旗(召唤)
			if (ob->data.dwObjectID == 12157)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12158)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12159)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12160)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12161)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 121;
				sprintf(ob->data.strName, "[未改造]天魔战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12162)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12163)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12164)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12165)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12166)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12167)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12168)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12169)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12170)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12171)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12172)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12173)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12174)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12175)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12176)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12177)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12178)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临战旗(召唤)");
			}
			else if (ob->data.dwObjectID == 12179)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}

           	//soke 70级以上装备升级 - 护心镜(物)
			if (ob->data.dwObjectID == 12207)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12208)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12209)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜物护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12210)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血物护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12211)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天物护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12212)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12213)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12214)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12215)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12216)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12217)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12218)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12219)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12220)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12221)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12222)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12223)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12224)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12225)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12226)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12227)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12228)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临护心镜(物)");
			}
			else if (ob->data.dwObjectID == 12229)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}

	        //soke 70级以上装备升级 - 护心镜(魔)
			if (ob->data.dwObjectID == 12257)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12258)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12259)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12260)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12261)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12262)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12263)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12264)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12265)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12266)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12267)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12268)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12269)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12270)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12271)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12272)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12273)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12274)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12275)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12276)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12277)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12278)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临护心镜(魔)");
			}
			else if (ob->data.dwObjectID == 12279)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}

			//soke 70级以上装备升级 - 短枪(物)
			if (ob->data.dwObjectID == 12307)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮短枪(物)");
			}
			else if (ob->data.dwObjectID == 12308)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神短枪(物)");
			}
			else if (ob->data.dwObjectID == 12309)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜物短枪(物)");
			}
			else if (ob->data.dwObjectID == 12310)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血物短枪(物)");
			}
			else if (ob->data.dwObjectID == 12311)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天物短枪(物)");
			}
			else if (ob->data.dwObjectID == 12312)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥短枪(物)");
			}
			else if (ob->data.dwObjectID == 12313)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武短枪(物)");
			}
			else if (ob->data.dwObjectID == 12314)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀短枪(物)");
			}
			else if (ob->data.dwObjectID == 12315)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎短枪(物)");
			}
			else if (ob->data.dwObjectID == 12316)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙短枪(物)");
			}
			else if (ob->data.dwObjectID == 12317)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇短枪(物)");
			}
			else if (ob->data.dwObjectID == 12318)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊短枪(物)");
			}
			else if (ob->data.dwObjectID == 12319)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤短枪(物)");
			}
			else if (ob->data.dwObjectID == 12320)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天短枪(物)");
			}
			else if (ob->data.dwObjectID == 12321)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕短枪(物)");
			}
			else if (ob->data.dwObjectID == 12322)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊短枪(物)");
			}
			else if (ob->data.dwObjectID == 12323)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄短枪(物)");
			}
			else if (ob->data.dwObjectID == 12324)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者短枪(物)");
			}
			else if (ob->data.dwObjectID == 12325)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神短枪(物)");
			}
			else if (ob->data.dwObjectID == 12326)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒短枪(物)");
			}
			else if (ob->data.dwObjectID == 12327)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰短枪(物)");
			}
			else if (ob->data.dwObjectID == 12328)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临短枪(物)");
			}
			else if (ob->data.dwObjectID == 12329)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}

			//soke 70级以上装备升级 - 短枪(魔)
			if (ob->data.dwObjectID == 12357)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12358)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12359)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12360)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12361)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12362)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12363)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12364)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12365)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12366)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12367)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12368)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12369)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12370)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12371)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12372)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12373)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12374)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12375)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12376)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12377)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12378)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临短枪(魔)");
			}
			else if (ob->data.dwObjectID == 12379)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}

            //soke 70级以上装备升级 - 匕首(魔)
			if (ob->data.dwObjectID == 12407)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12408)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12409)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜物匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12410)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血物匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12411)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天物匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12412)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12413)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12414)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12415)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12416)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12417)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12418)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12419)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12420)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12421)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12422)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12423)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12424)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12425)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12426)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12427)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12428)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临匕首(魔)");
			}
			else if (ob->data.dwObjectID == 12429)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}

            //soke 70级以上装备升级 - 盔(魔)
			if (ob->data.dwObjectID == 10907)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮盔(魔)");
			}
			else if (ob->data.dwObjectID == 10908)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神盔(魔)");
			}
			else if (ob->data.dwObjectID == 10909)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔盔(魔)");
			}
			else if (ob->data.dwObjectID == 10910)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔盔(魔)");
			}
			else if (ob->data.dwObjectID == 10911)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔盔(魔)");
			}
			else if (ob->data.dwObjectID == 10912)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥盔(魔)");
			}
			else if (ob->data.dwObjectID == 10913)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武盔(魔)");
			}
			else if (ob->data.dwObjectID == 10914)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀盔(魔)");
			}
			else if (ob->data.dwObjectID == 10915)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎盔(魔)");
			}
			else if (ob->data.dwObjectID == 10916)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙盔(魔)");
			}
			else if (ob->data.dwObjectID == 10917)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇盔(魔)");
			}
			else if (ob->data.dwObjectID == 10918)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊盔(魔)");
			}
			else if (ob->data.dwObjectID == 10919)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤盔(魔)");
			}
			else if (ob->data.dwObjectID == 10920)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天盔(魔)");
			}
			else if (ob->data.dwObjectID == 10921)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕盔(魔)");
			}
			else if (ob->data.dwObjectID == 10922)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊盔(魔)");
			}
			else if (ob->data.dwObjectID == 10923)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄盔(魔)");
			}
			else if (ob->data.dwObjectID == 10924)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者盔(魔)");
			}
			else if (ob->data.dwObjectID == 10925)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神盔(魔)");
			}
			else if (ob->data.dwObjectID == 10926)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒盔(魔)");
			}
			else if (ob->data.dwObjectID == 10927)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰盔(魔)");
			}
			else if (ob->data.dwObjectID == 10928)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临盔(魔)");
			}
			else if (ob->data.dwObjectID == 10929)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
	        //soke 70级以上装备升级 - 盔(物)
			if (ob->data.dwObjectID == 10957)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮盔(物)");
			}
			else if (ob->data.dwObjectID == 10958)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神盔(物)");
			}
			else if (ob->data.dwObjectID == 10959)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜物盔(物)");
			}
			else if (ob->data.dwObjectID == 10960)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血物盔(物)");
			}
			else if (ob->data.dwObjectID == 10961)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天物盔(物)");
			}
			else if (ob->data.dwObjectID == 10962)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥盔(物)");
			}
			else if (ob->data.dwObjectID == 10963)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武盔(物)");
			}
			else if (ob->data.dwObjectID == 10964)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀盔(物)");
			}
			else if (ob->data.dwObjectID == 10965)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎盔(物)");
			}
			else if (ob->data.dwObjectID == 10966)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙盔(物)");
			}
			else if (ob->data.dwObjectID == 10967)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇盔(物)");
			}
			else if (ob->data.dwObjectID == 10968)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊盔(物)");
			}
			else if (ob->data.dwObjectID == 10969)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤盔(物)");
			}
			else if (ob->data.dwObjectID == 10970)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天盔(物)");
			}
			else if (ob->data.dwObjectID == 10971)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕盔(物)");
			}
			else if (ob->data.dwObjectID == 10972)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊盔(物)");
			}
			else if (ob->data.dwObjectID == 10973)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄盔(物)");
			}
			else if (ob->data.dwObjectID == 10974)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者盔(物)");
			}
			else if (ob->data.dwObjectID == 10975)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神盔(物)");
			}
			else if (ob->data.dwObjectID == 10976)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒盔(物)");
			}
			else if (ob->data.dwObjectID == 10977)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰盔(物)");
			}
			else if (ob->data.dwObjectID == 10978)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临盔(物)");
			}
			else if (ob->data.dwObjectID == 10979)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
           	//soke 70级以上装备升级 - 腰带(魔)
			if (ob->data.dwObjectID == 11007)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11008)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11009)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11010)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11011)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11012)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11013)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11014)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11015)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11016)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11017)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11018)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11019)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11020)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11021)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11022)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11023)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11024)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11025)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11026)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11027)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11028)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临腰带(魔)");
			}
			else if (ob->data.dwObjectID == 11029)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
	        //soke 70级以上装备升级 - 腰带(物)
			if (ob->data.dwObjectID == 11057)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮腰带(物)");
			}
			else if (ob->data.dwObjectID == 11058)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神腰带(物)");
			}
			else if (ob->data.dwObjectID == 11059)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜物腰带(物)");
			}
			else if (ob->data.dwObjectID == 11060)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血物腰带(物)");
			}
			else if (ob->data.dwObjectID == 11061)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天物腰带(物)");
			}
			else if (ob->data.dwObjectID == 11062)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥腰带(物)");
			}
			else if (ob->data.dwObjectID == 11063)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武腰带(物)");
			}
			else if (ob->data.dwObjectID == 11064)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀腰带(物)");
			}
			else if (ob->data.dwObjectID == 11065)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎腰带(物)");
			}
			else if (ob->data.dwObjectID == 11066)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙腰带(物)");
			}
			else if (ob->data.dwObjectID == 11067)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇腰带(物)");
			}
			else if (ob->data.dwObjectID == 11068)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊腰带(物)");
			}
			else if (ob->data.dwObjectID == 11069)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤腰带(物)");
			}
			else if (ob->data.dwObjectID == 11070)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天腰带(物)");
			}
			else if (ob->data.dwObjectID == 11071)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕腰带(物)");
			}
			else if (ob->data.dwObjectID == 11072)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊腰带(物)");
			}
			else if (ob->data.dwObjectID == 11073)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄腰带(物)");
			}
			else if (ob->data.dwObjectID == 11074)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者腰带(物)");
			}
			else if (ob->data.dwObjectID == 11075)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神腰带(物)");
			}
			else if (ob->data.dwObjectID == 11076)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒腰带(物)");
			}
			else if (ob->data.dwObjectID == 11077)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰腰带(物)");
			}
			else if (ob->data.dwObjectID == 11078)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临腰带(物)");
			}
			else if (ob->data.dwObjectID == 11079)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
           	//soke 70级以上装备升级 - 护腕(魔)
			if (ob->data.dwObjectID == 11107)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11108)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11109)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11110)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 111;
				sprintf(ob->data.strName, "[未改造]血魔护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11111)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11112)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11113)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11114)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11115)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11116)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11117)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11118)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11119)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11120)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11121)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11122)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11123)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11124)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11125)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11126)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11127)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11128)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临护腕(魔)");
			}
			else if (ob->data.dwObjectID == 11129)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
	        //soke 70级以上装备升级 - 护腕(物)
			if (ob->data.dwObjectID == 11157)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮护腕(物)");
			}
			else if (ob->data.dwObjectID == 11158)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神护腕(物)");
			}
			else if (ob->data.dwObjectID == 11159)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜物护腕(物)");
			}
			else if (ob->data.dwObjectID == 11160)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 111;
				sprintf(ob->data.strName, "[未改造]血物护腕(物)");
			}
			else if (ob->data.dwObjectID == 11161)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天物护腕(物)");
			}
			else if (ob->data.dwObjectID == 11162)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥护腕(物)");
			}
			else if (ob->data.dwObjectID == 11163)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武护腕(物)");
			}
			else if (ob->data.dwObjectID == 11164)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀护腕(物)");
			}
			else if (ob->data.dwObjectID == 11165)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎护腕(物)");
			}
			else if (ob->data.dwObjectID == 11166)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙护腕(物)");
			}
			else if (ob->data.dwObjectID == 11167)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇护腕(物)");
			}
			else if (ob->data.dwObjectID == 11168)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊护腕(物)");
			}
			else if (ob->data.dwObjectID == 11169)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤护腕(物)");
			}
			else if (ob->data.dwObjectID == 11170)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天护腕(物)");
			}
			else if (ob->data.dwObjectID == 11171)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕护腕(物)");
			}
			else if (ob->data.dwObjectID == 11172)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊护腕(物)");
			}
			else if (ob->data.dwObjectID == 11173)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄护腕(物)");
			}
			else if (ob->data.dwObjectID == 11174)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者护腕(物)");
			}
			else if (ob->data.dwObjectID == 11175)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神护腕(物)");
			}
			else if (ob->data.dwObjectID == 11176)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒护腕(物)");
			}
			else if (ob->data.dwObjectID == 11177)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰护腕(物)");
			}
			else if (ob->data.dwObjectID == 11178)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临护腕(物)");
			}
			else if (ob->data.dwObjectID == 11179)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
           	//soke 70级以上装备升级 - 靴(物)
			if (ob->data.dwObjectID == 11207)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮靴(物)");
			}
			else if (ob->data.dwObjectID == 11208)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神靴(物)");
			}
			else if (ob->data.dwObjectID == 11209)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜物靴(物)");
			}
			else if (ob->data.dwObjectID == 11210)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血物靴(物)");
			}
			else if (ob->data.dwObjectID == 11211)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天物靴(物)");
			}
			else if (ob->data.dwObjectID == 11212)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥靴(物)");
			}
			else if (ob->data.dwObjectID == 11213)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武靴(物)");
			}
			else if (ob->data.dwObjectID == 11214)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀靴(物)");
			}
			else if (ob->data.dwObjectID == 11215)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎靴(物)");
			}
			else if (ob->data.dwObjectID == 11216)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙靴(物)");
			}
			else if (ob->data.dwObjectID == 11217)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇靴(物)");
			}
			else if (ob->data.dwObjectID == 11218)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊靴(物)");
			}
			else if (ob->data.dwObjectID == 11219)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤靴(物)");
			}
			else if (ob->data.dwObjectID == 11220)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天靴(物)");
			}
			else if (ob->data.dwObjectID == 11221)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕靴(物)");
			}
			else if (ob->data.dwObjectID == 11222)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊靴(物)");
			}
			else if (ob->data.dwObjectID == 11223)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄靴(物)");
			}
			else if (ob->data.dwObjectID == 11224)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者靴(物)");
			}
			else if (ob->data.dwObjectID == 11225)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神靴(物)");
			}
			else if (ob->data.dwObjectID == 11226)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒靴(物)");
			}
			else if (ob->data.dwObjectID == 11227)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰靴(物)");
			}
			else if (ob->data.dwObjectID == 11228)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临靴(物)");
			}
			else if (ob->data.dwObjectID == 11229)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
	        //soke 70级以上装备升级 - 靴(魔)
			if (ob->data.dwObjectID == 11257)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮靴(魔)");
			}
			else if (ob->data.dwObjectID == 11258)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神靴(魔)");
			}
			else if (ob->data.dwObjectID == 11259)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔靴(魔)");
			}
			else if (ob->data.dwObjectID == 11260)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔靴(魔)");
			}
			else if (ob->data.dwObjectID == 11261)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔靴(魔)");
			}
			else if (ob->data.dwObjectID == 11262)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥靴(魔)");
			}
			else if (ob->data.dwObjectID == 11263)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武靴(魔)");
			}
			else if (ob->data.dwObjectID == 11264)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀靴(魔)");
			}
			else if (ob->data.dwObjectID == 11265)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎靴(魔)");
			}
			else if (ob->data.dwObjectID == 11266)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙靴(魔)");
			}
			else if (ob->data.dwObjectID == 11267)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇靴(魔)");
			}
			else if (ob->data.dwObjectID == 11268)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊靴(魔)");
			}
			else if (ob->data.dwObjectID == 11269)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤靴(魔)");
			}
			else if (ob->data.dwObjectID == 11270)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天靴(魔)");
			}
			else if (ob->data.dwObjectID == 11271)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕靴(魔)");
			}
			else if (ob->data.dwObjectID == 11272)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊靴(魔)");
			}
			else if (ob->data.dwObjectID == 11273)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄靴(魔)");
			}
			else if (ob->data.dwObjectID == 11274)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者靴(魔)");
			}
			else if (ob->data.dwObjectID == 11275)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神靴(魔)");
			}
			else if (ob->data.dwObjectID == 11276)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒靴(魔)");
			}
			else if (ob->data.dwObjectID == 11277)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰靴(魔)");
			}
			else if (ob->data.dwObjectID == 11278)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临靴(魔)");
			}
			else if (ob->data.dwObjectID == 11279)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
           	//soke 70级以上装备升级 - 项链(物)
			if (ob->data.dwObjectID == 11307)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮项链(物)");
			}
			else if (ob->data.dwObjectID == 11308)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神项链(物)");
			}
			else if (ob->data.dwObjectID == 11309)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜物项链(物)");
			}
			else if (ob->data.dwObjectID == 11310)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血物项链(物)");
			}
			else if (ob->data.dwObjectID == 11311)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天物项链(物)");
			}
			else if (ob->data.dwObjectID == 11312)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥项链(物)");
			}
			else if (ob->data.dwObjectID == 11313)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武项链(物)");
			}
			else if (ob->data.dwObjectID == 11314)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀项链(物)");
			}
			else if (ob->data.dwObjectID == 11315)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎项链(物)");
			}
			else if (ob->data.dwObjectID == 11316)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙项链(物)");
			}
			else if (ob->data.dwObjectID == 11317)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇项链(物)");
			}
			else if (ob->data.dwObjectID == 11318)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊项链(物)");
			}
			else if (ob->data.dwObjectID == 11319)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤项链(物)");
			}
			else if (ob->data.dwObjectID == 11320)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天项链(物)");
			}
			else if (ob->data.dwObjectID == 11321)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕项链(物)");
			}
			else if (ob->data.dwObjectID == 11322)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊项链(物)");
			}
			else if (ob->data.dwObjectID == 11323)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄项链(物)");
			}
			else if (ob->data.dwObjectID == 11324)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者项链(物)");
			}
			else if (ob->data.dwObjectID == 11325)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神项链(物)");
			}
			else if (ob->data.dwObjectID == 11326)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒项链(物)");
			}
			else if (ob->data.dwObjectID == 11327)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰项链(物)");
			}
			else if (ob->data.dwObjectID == 11328)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临项链(物)");
			}
			else if (ob->data.dwObjectID == 11329)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
	        //soke 70级以上装备升级 - 项链(魔)
			if (ob->data.dwObjectID == 11357)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮项链(魔)");
			}
			else if (ob->data.dwObjectID == 11358)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神项链(魔)");
			}
			else if (ob->data.dwObjectID == 11359)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔项链(魔)");
			}
			else if (ob->data.dwObjectID == 11360)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔项链(魔)");
			}
			else if (ob->data.dwObjectID == 11361)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔项链(魔)");
			}
			else if (ob->data.dwObjectID == 11362)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥项链(魔)");
			}
			else if (ob->data.dwObjectID == 11363)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武项链(魔)");
			}
			else if (ob->data.dwObjectID == 11364)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀项链(魔)");
			}
			else if (ob->data.dwObjectID == 11365)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎项链(魔)");
			}
			else if (ob->data.dwObjectID == 11366)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙项链(魔)");
			}
			else if (ob->data.dwObjectID == 11367)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇项链(魔)");
			}
			else if (ob->data.dwObjectID == 11368)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊项链(魔)");
			}
			else if (ob->data.dwObjectID == 11369)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤项链(魔)");
			}
			else if (ob->data.dwObjectID == 11370)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天项链(魔)");
			}
			else if (ob->data.dwObjectID == 11371)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕项链(魔)");
			}
			else if (ob->data.dwObjectID == 11372)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊项链(魔)");
			}
			else if (ob->data.dwObjectID == 11373)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄项链(魔)");
			}
			else if (ob->data.dwObjectID == 11374)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者项链(魔)");
			}
			else if (ob->data.dwObjectID == 11375)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神项链(魔)");
			}
			else if (ob->data.dwObjectID == 11376)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒项链(魔)");
			}
			else if (ob->data.dwObjectID == 11377)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰项链(魔)");
			}
			else if (ob->data.dwObjectID == 11378)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临项链(魔)");
			}
			else if (ob->data.dwObjectID == 11379)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
           	//soke 70级以上装备升级 - 戒指(物)
			if (ob->data.dwObjectID == 11407)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮戒指(物)");
			}
			else if (ob->data.dwObjectID == 11408)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神戒指(物)");
			}
			else if (ob->data.dwObjectID == 11409)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜物戒指(物)");
			}
			else if (ob->data.dwObjectID == 11410)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血物戒指(物)");
			}
			else if (ob->data.dwObjectID == 11411)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天物戒指(物)");
			}
			else if (ob->data.dwObjectID == 11412)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥戒指(物)");
			}
			else if (ob->data.dwObjectID == 11413)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武戒指(物)");
			}
			else if (ob->data.dwObjectID == 11414)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀戒指(物)");
			}
			else if (ob->data.dwObjectID == 11415)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎戒指(物)");
			}
			else if (ob->data.dwObjectID == 11416)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙戒指(物)");
			}
			else if (ob->data.dwObjectID == 11417)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇戒指(物)");
			}
			else if (ob->data.dwObjectID == 11418)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊戒指(物)");
			}
			else if (ob->data.dwObjectID == 11419)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤戒指(物)");
			}
			else if (ob->data.dwObjectID == 11420)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天戒指(物)");
			}
			else if (ob->data.dwObjectID == 11421)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕戒指(物)");
			}
			else if (ob->data.dwObjectID == 11422)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊戒指(物)");
			}
			else if (ob->data.dwObjectID == 11423)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄戒指(物)");
			}
			else if (ob->data.dwObjectID == 11424)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者戒指(物)");
			}
			else if (ob->data.dwObjectID == 11425)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神戒指(物)");
			}
			else if (ob->data.dwObjectID == 11426)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒戒指(物)");
			}
			else if (ob->data.dwObjectID == 11427)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰戒指(物)");
			}
			else if (ob->data.dwObjectID == 11428)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临戒指(物)");
			}
			else if (ob->data.dwObjectID == 11429)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
	        //soke 70级以上装备升级 - 戒指(魔)
			if (ob->data.dwObjectID == 11457)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 80;
				sprintf(ob->data.strName, "[未改造]血饮戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11458)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 90;
				sprintf(ob->data.strName, "[未改造]火神戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11459)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 100;
				sprintf(ob->data.strName, "[未改造]夜魔戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11460)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 110;
				sprintf(ob->data.strName, "[未改造]血魔戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11461)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]天魔戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11462)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]幽冥戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11463)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 135;
				sprintf(ob->data.strName, "[未改造]玄武戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11464)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]朱雀戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11465)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 145;
				sprintf(ob->data.strName, "[未改造]白虎戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11466)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]青龙戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11467)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 155;
				sprintf(ob->data.strName, "[未改造]紫薇戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11468)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 158;
				sprintf(ob->data.strName, "[未改造]至尊戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11469)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 165;
				sprintf(ob->data.strName, "[未改造]乾坤戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11470)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]炙天戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11471)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 175;
				sprintf(ob->data.strName, "[未改造]轩辕戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11472)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]天尊戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11473)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 185;
				sprintf(ob->data.strName, "[未改造]英雄戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11474)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]圣者戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11475)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 197;
				sprintf(ob->data.strName, "[未改造]战神戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11476)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 202;
				sprintf(ob->data.strName, "[未改造]永恒戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11477)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 205;
				sprintf(ob->data.strName, "[未改造]寒冰戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11478)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 210;
				sprintf(ob->data.strName, "[未改造]星临戒指(魔)");
			}
			else if (ob->data.dwObjectID == 11479)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}
			//soke 70级以上装备升级 - 马蹄铁
			if (ob->data.dwObjectID == 16101)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]完美的马蹄铁(二阶)");
			}
			else if (ob->data.dwObjectID == 16102)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]完美的马蹄铁(三阶)");
			}
			else if (ob->data.dwObjectID == 16103)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]完美的马蹄铁(四阶)");
			}
			else if (ob->data.dwObjectID == 16104)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]完美的马蹄铁(五阶)");
			}
			else if (ob->data.dwObjectID == 16105)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 160;
				sprintf(ob->data.strName, "[未改造]完美的马蹄铁(六阶)");
			}
			else if (ob->data.dwObjectID == 16106)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]完美的马蹄铁(七阶)");
			}
			else if (ob->data.dwObjectID == 16107)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]完美的马蹄铁(八阶)");
			}
			else if (ob->data.dwObjectID == 16108)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]完美的马蹄铁(九阶)");
			}
			else if (ob->data.dwObjectID == 16109)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 200;
				sprintf(ob->data.strName, "[未改造]完美的马蹄铁(十阶)");
			}
			else if (ob->data.dwObjectID == 16110)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}	
			//soke 70级以上马装备升级 - 马缰绳
			if (ob->data.dwObjectID == 16201)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]完美的马缰绳(二阶)");
			}
			else if (ob->data.dwObjectID == 16202)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]完美的马缰绳(三阶)");
			}
			else if (ob->data.dwObjectID == 16203)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]完美的马缰绳(四阶)");
			}
			else if (ob->data.dwObjectID == 16204)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]完美的马缰绳(五阶)");
			}
			else if (ob->data.dwObjectID == 16205)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 160;
				sprintf(ob->data.strName, "[未改造]完美的马缰绳(六阶)");
			}
			else if (ob->data.dwObjectID == 16206)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]完美的马缰绳(七阶)");
			}
			else if (ob->data.dwObjectID == 16207)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]完美的马缰绳(八阶)");
			}
			else if (ob->data.dwObjectID == 16208)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]完美的马缰绳(九阶)");
			}
			else if (ob->data.dwObjectID == 16209)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 200;
				sprintf(ob->data.strName, "[未改造]完美的马缰绳(十阶)");
			}
			else if (ob->data.dwObjectID == 16210)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}	

			//soke 70级以上马装备升级 - 御马鞍
			if (ob->data.dwObjectID == 16301)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]完美的御马鞍(二阶)");
			}
			else if (ob->data.dwObjectID == 16302)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]完美的御马鞍(三阶)");
			}
			else if (ob->data.dwObjectID == 16303)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]完美的御马鞍(四阶)");
			}
			else if (ob->data.dwObjectID == 16304)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]完美的御马鞍(五阶)");
			}
			else if (ob->data.dwObjectID == 16305)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 160;
				sprintf(ob->data.strName, "[未改造]完美的御马鞍(六阶)");
			}
			else if (ob->data.dwObjectID == 16306)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]完美的御马鞍(七阶)");
			}
			else if (ob->data.dwObjectID == 16307)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]完美的御马鞍(八阶)");
			}
			else if (ob->data.dwObjectID == 16308)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]完美的御马鞍(九阶)");
			}
			else if (ob->data.dwObjectID == 16309)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 200;
				sprintf(ob->data.strName, "[未改造]完美的御马鞍(十阶)");
			}
			else if (ob->data.dwObjectID == 16310)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}		
			
			//soke 70级以上马装备升级 - 护马甲
			if (ob->data.dwObjectID == 16401)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]完美的护马甲(二阶)");
			}
			else if (ob->data.dwObjectID == 16402)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]完美的护马甲(三阶)");
			}
			else if (ob->data.dwObjectID == 16403)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]完美的护马甲(四阶)");
			}
			else if (ob->data.dwObjectID == 16404)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]完美的护马甲(五阶)");
			}
			else if (ob->data.dwObjectID == 16405)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 160;
				sprintf(ob->data.strName, "[未改造]完美的护马甲(六阶)");
			}
			else if (ob->data.dwObjectID == 16406)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]完美的护马甲(七阶)");
			}
			else if (ob->data.dwObjectID == 16407)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]完美的护马甲(八阶)");
			}
			else if (ob->data.dwObjectID == 16408)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]完美的护马甲(九阶)");
			}
			else if (ob->data.dwObjectID == 16409)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 200;
				sprintf(ob->data.strName, "[未改造]完美的护马甲(十阶)");
			}
			else if (ob->data.dwObjectID == 16410)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}	

			//soke 70级以上马装备升级 - 马骑镫
			if (ob->data.dwObjectID == 16501)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 120;
				sprintf(ob->data.strName, "[未改造]完美的马骑镫(二阶)");
			}
			else if (ob->data.dwObjectID == 16502)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 130;
				sprintf(ob->data.strName, "[未改造]完美的马骑镫(三阶)");
			}
			else if (ob->data.dwObjectID == 16503)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 140;
				sprintf(ob->data.strName, "[未改造]完美的马骑镫(四阶)");
			}
			else if (ob->data.dwObjectID == 16504)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 150;
				sprintf(ob->data.strName, "[未改造]完美的马骑镫(五阶)");
			}
			else if (ob->data.dwObjectID == 16505)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 160;
				sprintf(ob->data.strName, "[未改造]完美的马骑镫(六阶)");
			}
			else if (ob->data.dwObjectID == 16506)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 170;
				sprintf(ob->data.strName, "[未改造]完美的马骑镫(七阶)");
			}
			else if (ob->data.dwObjectID == 16507)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 180;
				sprintf(ob->data.strName, "[未改造]完美的马骑镫(八阶)");
			}
			else if (ob->data.dwObjectID == 16508)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 190;
				sprintf(ob->data.strName, "[未改造]完美的马骑镫(九阶)");
			}
			else if (ob->data.dwObjectID == 16509)
			{
				ob->data.dwObjectID += 1; 
				ob->data.needlevel = 200;
				sprintf(ob->data.strName, "[未改造]完美的马骑镫(十阶)");
			}
			else if (ob->data.dwObjectID == 16510)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，当前装备已经达到升级上限");
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级失败，当前装备已经达到升级上限");
				return false;
			}	
			//Channel::sendSys(user, Cmd::INFO_TYPE_MSG, "装备升级功能优化中");
		} 

		 do_equip(ob, base);
		return true;
	}
	else 
	{
		//sky 不掉级
		int down = 0;
		for( int i=0;i<down;i++)
		{
			base = equipobjectbm.get(ob->data.dwObjectID+100000*(ob->data.retain7));
			if (base) do_downequip(ob, base);
		}
		return false;
	}
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品星星等级
 *
 * \param ob: 升星物品
 * \param base: 升星属性
 * \return 当前总是返回true
 */
bool Upgrade::do_upgrade(zObject* ob, zUpgradeObjectB* base)
{
	++ob->data.upgrade;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			// 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			// 物防
	COMPUTE( mdefence )				// 魔防
	COMPUTE( maxhp )			// 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品星星等级
 *
 * \param ob: 升星物品
 * \param base: 升星属性
 * \return 当前总是返回true
 */
bool Upgrade::do_downgrade(zObject* ob, zUpgradeObjectB* base)
{

	if (ob->data.upgrade > 0) 
	{
		--ob->data.upgrade;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( pdamage )				// 最小攻击力
		COMPUTE( maxpdamage )			// 最大攻击力
		COMPUTE( mdamage )			// 最小法术攻击力
		COMPUTE( maxmdamage )			// 最大法术攻击力
	
		COMPUTE( pdefence )			// 物防
		COMPUTE( mdefence )				// 魔防
		COMPUTE( maxhp )			// 最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品星星等级
 *
 * \param ob: 升星物品
 * \param base: 升星属性
 * \return 当前总是返回true
 */
bool Upgrade::do_2upgrade(zObject* ob, zUpgradeObject2B* base)
{
	//soke 历史最高升星
	if ((ob->data.upgrade+1)==1 && ob->data.xingshu <= 0)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==2 && ob->data.xingshu <= 1)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==3 && ob->data.xingshu <= 2)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==4 && ob->data.xingshu <= 3)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==5 && ob->data.xingshu <= 4)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==6 && ob->data.xingshu <= 5)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==7 && ob->data.xingshu <= 6)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==8 && ob->data.xingshu <= 7)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==9 && ob->data.xingshu <= 8)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==10 && ob->data.xingshu <= 9)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==11 && ob->data.xingshu <= 10)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==12 && ob->data.xingshu <= 11)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==13 && ob->data.xingshu <= 12)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==14 && ob->data.xingshu <= 13)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==15 && ob->data.xingshu <= 14)
	{
        ob->data.xingshu+=1;
	}

	++ob->data.upgrade;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p1damage )				// 最小攻击力
	COMPUTE( m1axpdamage )			// 最大攻击力
	COMPUTE( m1damage )			    // 最小法术攻击力
	COMPUTE( m1axmdamage )			// 最大法术攻击力

	COMPUTE( p1defence )			// 物防
	COMPUTE( m1defence )			// 魔防
	COMPUTE( m1axhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品星星等级
 *
 * \param ob: 升星物品
 * \param base: 升星属性
 * \return 当前总是返回true
 */
bool Upgrade::do_2downgrade(zObject* ob, zUpgradeObject2B* base)
{

	if (ob->data.upgrade > 0) 
	{
		--ob->data.upgrade;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p1damage )				// 最小攻击力
		COMPUTE( m1axpdamage )			// 最大攻击力
		COMPUTE( m1damage )			    // 最小法术攻击力
		COMPUTE( m1axmdamage )			// 最大法术攻击力
	
		COMPUTE( p1defence )			// 物防
		COMPUTE( m1defence )			// 魔防
		COMPUTE( m1axhp )			    // 最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品星星等级
 *
 * \param ob: 升星物品
 * \param base: 升星属性
 * \return 当前总是返回true
 */
bool Upgrade::pur_upgrade(zObject* ob, zUpgradePurpleB* base)
{
	//soke 历史最高升星
	if ((ob->data.upgrade+1)==1 && ob->data.xingshu <= 0)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==2 && ob->data.xingshu <= 1)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==3 && ob->data.xingshu <= 2)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==4 && ob->data.xingshu <= 3)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==5 && ob->data.xingshu <= 4)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==6 && ob->data.xingshu <= 5)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==7 && ob->data.xingshu <= 6)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==8 && ob->data.xingshu <= 7)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==9 && ob->data.xingshu <= 8)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==10 && ob->data.xingshu <= 9)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==11 && ob->data.xingshu <= 10)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==12 && ob->data.xingshu <= 11)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==13 && ob->data.xingshu <= 12)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==14 && ob->data.xingshu <= 13)
	{
        ob->data.xingshu+=1;
	}
	else if ((ob->data.upgrade+1)==15 && ob->data.xingshu <= 14)
	{
        ob->data.xingshu+=1;
	}

	++ob->data.upgrade;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p2damage )				// 最小攻击力
	COMPUTE( m2axpdamage )			// 最大攻击力
	COMPUTE( m2damage )			    // 最小法术攻击力
	COMPUTE( m2axmdamage )			// 最大法术攻击力

	COMPUTE( p2defence )			// 物防
	COMPUTE( m2defence )			// 魔防
	COMPUTE( m2axhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品星星等级
 *
 * \param ob: 升星物品
 * \param base: 升星属性
 * \return 当前总是返回true
 */
bool Upgrade::pur_downgrade(zObject* ob, zUpgradePurpleB* base)
{

	if (ob->data.upgrade > 0) 
	{
		--ob->data.upgrade;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p2damage )				// 最小攻击力
		COMPUTE( m2axpdamage )			// 最大攻击力
		COMPUTE( m2damage )			    // 最小法术攻击力
		COMPUTE( m2axmdamage )			// 最大法术攻击力
	
		COMPUTE( p2defence )			// 物防
		COMPUTE( m2defence )			// 魔防
		COMPUTE( m2axhp )			    // 最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品斗魂等级
 *
 * \param ob: 斗魂物品
 * \param base: 斗魂属性
 * \return 当前总是返回true
 */
bool Upgrade::douhun_upgrade(zObject* ob, zUdouhunObjectB* base)
{
	++ob->data.douhun;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p3damage )				// 斗魂最小攻击力
	COMPUTE( m3axpdamage )			// 斗魂最大攻击力
	COMPUTE( m3damage )			    // 斗魂最小法术攻击力
	COMPUTE( m3axmdamage )			// 斗魂最大法术攻击力

	COMPUTE( p3defence )			// 斗魂物防
	COMPUTE( m3defence )			// 斗魂魔防
	COMPUTE( m3axhp )			    // 斗魂最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品斗魂等级
 *
 * \param ob: 斗魂物品
 * \param base: 斗魂属性
 * \return 当前总是返回true
 */
bool Upgrade::douhun_downgrade(zObject* ob, zUdouhunObjectB* base)
{

	if (ob->data.douhun > 0) 
	{
		--ob->data.douhun;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p3damage )				// 斗魂最小攻击力
		COMPUTE( m3axpdamage )			// 斗魂最大攻击力
		COMPUTE( m3damage )			    // 斗魂最小法术攻击力
		COMPUTE( m3axmdamage )			// 斗魂最大法术攻击力
	
		COMPUTE( p3defence )			// 斗魂物防
		COMPUTE( m3defence )			// 斗魂魔防
		COMPUTE( m3axhp )			    // 斗魂最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品补天等级
 *
 * \param ob: 补天物品
 * \param base: 补天属性
 * \return 当前总是返回true
 */
bool Upgrade::butian_upgrade(zObject* ob, zUbutianObjectB* base)
{
	++ob->data.butian;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p6damage )				// 补天最小攻击力
	COMPUTE( m6axpdamage )			// 补天最大攻击力
	COMPUTE( m6damage )			    // 补天最小法术攻击力
	COMPUTE( m6axmdamage )			// 补天最大法术攻击力

	COMPUTE( p6defence )			// 补天物防
	COMPUTE( m6defence )			// 补天魔防
	COMPUTE( m6axhp )			    // 补天最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品补天等级
 *
 * \param ob: 补天物品
 * \param base: 补天属性
 * \return 当前总是返回true
 */
bool Upgrade::butian_downgrade(zObject* ob, zUbutianObjectB* base)
{

	if (ob->data.butian > 0) 
	{
		--ob->data.butian;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p6damage )				// 补天最小攻击力
		COMPUTE( m6axpdamage )			// 补天最大攻击力
		COMPUTE( m6damage )			    // 补天最小法术攻击力
		COMPUTE( m6axmdamage )			// 补天最大法术攻击力
	
		COMPUTE( p6defence )			// 补天物防
		COMPUTE( m6defence )			// 补天魔防
		COMPUTE( m6axhp )			    // 补天最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

/**     
 * \brief 提升物品等级
 *
 * \param ob: 等级物品
 * \param base: 龙星等级属性
 * \return 当前总是返回true
 */
bool Upgrade::do_dtrough_upgrade(zObject* ob, zUdtroughObjectB* base)
{
	++ob->data.dtrough;
	
	COMPUTE( p4damage )				// 龙槽*最小攻击力
	COMPUTE( m4axpdamage )			// 龙槽*最大攻击力
	COMPUTE( m4damage )			    // 龙槽*最小法术攻击力
	COMPUTE( m4axmdamage )			// 龙槽*最大法术攻击力

	COMPUTE( p4defence )			// 龙槽*物防
	COMPUTE( m4defence )			// 龙槽*魔防
	COMPUTE( m4axhp )			    // 龙槽*最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SDWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序

/**     
 * \brief 降低物品等级
 *
 * \param ob: 龙星等级物品
 * \param base: 等级属性
 * \return 当前总是返回true
 */
bool Upgrade::do_dtrough_downgrade(zObject* ob, zUdtroughObjectB* base)
{

	if (ob->data.dtrough > 0)
	{
		--ob->data.dtrough;
		
		COMPUTE( p4damage )				// 龙槽*最小攻击力
		COMPUTE( m4axpdamage )			// 龙槽*最大攻击力
		COMPUTE( m4damage )			    // 龙槽*最小法术攻击力
		COMPUTE( m4axmdamage )			// 龙槽*最大法术攻击力
	
		COMPUTE( p4defence )			// 龙槽*物防
		COMPUTE( m4defence )			// 龙槽*魔防
		COMPUTE( m4axhp )			    // 龙槽*最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

/**
* \brief 提升物品等级
*
* \param ob: 等级物品
* \param base: 龙星等级属性
* \return 当前总是返回true
*/
bool Upgrade::do_drastar_upgrade(zObject* ob, zUdrastarObjectB* base)
{
		++ob->data.drastar;

		COMPUTE(p5damage)			// 龙*最小攻击力
		COMPUTE(m5axpdamage)		// 龙*最大攻击力
		COMPUTE(m5damage)			// 龙*最小法术攻击力
		COMPUTE(m5axmdamage)		// 龙*最大法术攻击力

		COMPUTE(p5defence)			// 龙*物防
		COMPUTE(m5defence)			// 龙*魔防
		COMPUTE(m5axhp)			    // 龙*最大生命值

		return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SDWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序

/**
* \brief 降低物品等级
*
* \param ob: 龙星等级物品
* \param base: 等级属性
* \return 当前总是返回true
*/
bool Upgrade::do_drastar_downgrade(zObject* ob, zUdrastarObjectB* base)
{

	if (ob->data.drastar > 0)
	{
			--ob->data.drastar;

			COMPUTE(p5damage)			// 龙*最小攻击力
			COMPUTE(m5axpdamage)		// 龙*最大攻击力
			COMPUTE(m5damage)			// 龙*最小法术攻击力
			COMPUTE(m5axmdamage)		// 龙*最大法术攻击力

			COMPUTE(p5defence)			// 龙*物防
			COMPUTE(m5defence)			// 龙*魔防
			COMPUTE(m5axhp)			    // 龙*最大生命值
	}

	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

/**
* \brief 提升龙凤吟物品等级
*
* \param ob: 等级物品
* \param base: 龙凤吟等级属性
* \return 当前总是返回true
*/
bool Upgrade::do_longfengying_upgrade(zObject* ob, zUlongfengyingObjectB* base)
{
		++ob->data.retain24; //龙凤吟等级

		COMPUTE(p24damage)			// 龙*最小攻击力
		COMPUTE(m24axpdamage)		// 龙*最大攻击力
		COMPUTE(m24damage)			// 龙*最小法术攻击力
		COMPUTE(m24axmdamage)		// 龙*最大法术攻击力

		COMPUTE(p24defence)			// 龙*物防
		COMPUTE(m24defence)			// 龙*魔防
		COMPUTE(m24axhp)			    // 龙*最大生命值

		return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SDWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序

/**
* \brief 降低龙凤吟物品等级
*
* \param ob: 龙凤吟等级物品
* \param base: 等级属性
* \return 当前总是返回true
*/
bool Upgrade::do_longfengying_downgrade(zObject* ob, zUlongfengyingObjectB* base)
{

	if (ob->data.retain24 > 0)
	{
			--ob->data.retain24;

			COMPUTE(p24damage)			// 龙*最小攻击力
			COMPUTE(m24axpdamage)		// 龙*最大攻击力
			COMPUTE(m24damage)			// 龙*最小法术攻击力
			COMPUTE(m24axmdamage)		// 龙*最大法术攻击力

			COMPUTE(p24defence)			// 龙*物防
			COMPUTE(m24defence)			// 龙*魔防
			COMPUTE(m24axhp)			    // 龙*最大生命值
	}

	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品勋章等级
 *
 * \param ob: 勋章物品
 * \param base: 勋章属性
 * \return 当前总是返回true
 */

bool Upgrade::do_medal_upgrade(zObject* ob, zMedaltObjectB* base)
{
	++ob->data.dmedalt;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p7damage )				// 勋章最小攻击力
	COMPUTE( m7axpdamage )			// 勋章最大攻击力
	COMPUTE( m7damage )			    // 勋章最小法术攻击力
	COMPUTE( m7axmdamage )			// 勋章最大法术攻击力

	COMPUTE( p7defence )			// 勋章物防
	COMPUTE( m7defence )			// 勋章魔防
	COMPUTE( m7axhp )			    // 勋章最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品勋章等级
 *
 * \param ob: 勋章物品
 * \param base: 勋章属性
 * \return 当前总是返回true
 */
bool Upgrade::do_medal_downgrade(zObject* ob, zMedaltObjectB* base)
{

	if (ob->data.dmedalt > 0) 
	{
		--ob->data.dmedalt;	
		
		COMPUTE( p7damage )				// 勋章最小攻击力
		COMPUTE( m7axpdamage )			// 勋章最大攻击力
		COMPUTE( m7damage )			    // 勋章最小法术攻击力
		COMPUTE( m7axmdamage )			// 勋章最大法术攻击力
	
		COMPUTE( p7defence )			// 勋章物防
		COMPUTE( m7defence )			// 勋章魔防
		COMPUTE( m7axhp )			    // 勋章最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品寒冰等级
 *
 * \param ob: 寒冰物品
 * \param base: 寒冰属性
 * \return 当前总是返回true
 */
bool Upgrade::hbql_upgrade(zObject* ob, zUhbqlObjectB* base)
{
	++ob->data.retain1;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p8damage )				// 寒冰最小攻击力
	COMPUTE( m8axpdamage )			// 寒冰最大攻击力
	COMPUTE( m8damage )			    // 寒冰最小法术攻击力
	COMPUTE( m8axmdamage )			// 寒冰最大法术攻击力

	COMPUTE( p8defence )			// 寒冰物防
	COMPUTE( m8defence )			// 寒冰魔防
	COMPUTE( m8axhp )			    // 寒冰最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品寒冰等级
 *
 * \param ob: 寒冰物品
 * \param base: 寒冰属性
 * \return 当前总是返回true
 */
bool Upgrade::hbql_downgrade(zObject* ob, zUhbqlObjectB* base)
{

	if (ob->data.retain1 > 0) 
	{
		--ob->data.retain1;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p8damage )				// 寒冰最小攻击力
		COMPUTE( m8axpdamage )			// 寒冰最大攻击力
		COMPUTE( m8damage )			    // 寒冰最小法术攻击力
		COMPUTE( m8axmdamage )			// 寒冰最大法术攻击力
	
		COMPUTE( p8defence )			// 寒冰物防
		COMPUTE( m8defence )			// 寒冰魔防
		COMPUTE( m8axhp )			    // 寒冰最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品神器等级
 *
 * \param ob: 神器物品
 * \param base: 神器属性
 * \return 当前总是返回true
 */
bool Upgrade::do_shenqi_upgrade(zObject* ob, zUshenqiObjectB* base)
{
	++ob->data.retain3;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p9damage )				// 神器最小攻击力
	COMPUTE( m9axpdamage )			// 神器最大攻击力
	COMPUTE( m9damage )			    // 神器最小法术攻击力
	COMPUTE( m9axmdamage )			// 神器最大法术攻击力

	COMPUTE( p9defence )			// 神器物防
	COMPUTE( m9defence )			// 神器魔防
	COMPUTE( m9axhp )			    // 神器最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品神器等级
 *
 * \param ob: 神器物品
 * \param base: 神器属性
 * \return 当前总是返回true
 */
bool Upgrade::do_shenqi_downgrade(zObject* ob, zUshenqiObjectB* base)
{

	if (ob->data.retain3 > 0) 
	{
		--ob->data.retain3;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p9damage )				// 神器最小攻击力
		COMPUTE( m9axpdamage )			// 神器最大攻击力
		COMPUTE( m9damage )			    // 神器最小法术攻击力
		COMPUTE( m9axmdamage )			// 神器最大法术攻击力
	
		COMPUTE( p9defence )			// 神器物防
		COMPUTE( m9defence )			// 神器魔防
		COMPUTE( m9axhp )			    // 神器最大生命值
	}
	
	return true;	
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

/**     
 * \brief 提升物品马牌精炼等级
 *
 * \param ob: 马牌精炼物品
 * \param base: 马牌精炼属性
 * \return 当前总是返回true
 */
bool Upgrade::do_refine_upgrade(zObject* ob, zUrefineObjectB* base)
{
	++ob->data.itemrefine;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p10damage )				// 马牌精炼最小攻击力
	COMPUTE( m10axpdamage )			// 马牌精炼最大攻击力
	COMPUTE( m10damage )			    // 马牌精炼最小法术攻击力
	COMPUTE( m10axmdamage )			// 马牌精炼最大法术攻击力

	COMPUTE( p10defence )			// 马牌精炼物防
	COMPUTE( m10defence )			// 马牌精炼魔防
	COMPUTE( m10axhp )			    // 马牌精炼最大生命值
	
	return true;
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品马牌精炼等级
 *
 * \param ob: 马牌精炼物品
 * \param base: 马牌精炼属性
 * \return 当前总是返回true
 */
bool Upgrade::do_refine_downgrade(zObject* ob, zUrefineObjectB* base)
{

	if (ob->data.itemrefine > 0) 
	{
		--ob->data.itemrefine;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p10damage )				// 马牌精炼最小攻击力
		COMPUTE( m10axpdamage )			// 马牌精炼最大攻击力
		COMPUTE( m10damage )			    // 马牌精炼最小法术攻击力
		COMPUTE( m10axmdamage )			// 马牌精炼最大法术攻击力
	
		COMPUTE( p10defence )			// 马牌精炼物防
		COMPUTE( m10defence )			// 马牌精炼魔防
		COMPUTE( m10axhp )			    // 马牌精炼最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

/**     
 * \brief 提升物品装备璀璨镶嵌等级
 *
 * \param ob: 装备璀璨镶嵌物品
 * \param base: 装备璀璨镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_bright_upgrade(zObject* ob, zUbrightObjectB* base)
{
	++ob->data.itemkscuican;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p11damage )			// 装备璀璨镶嵌 最小攻击力
	COMPUTE( m11axpdamage )			// 装备璀璨镶嵌 最大攻击力
	COMPUTE( m11damage )		    // 装备璀璨镶嵌 最小法术攻击力
	COMPUTE( m11axmdamage )			// 装备璀璨镶嵌 最大法术攻击力

	COMPUTE( p11defence )			// 装备璀璨镶嵌 物防
	COMPUTE( m11defence )			// 装备璀璨镶嵌 魔防
	COMPUTE( m11axhp )			    // 装备璀璨镶嵌 最大生命值
	
	return true;
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品装备璀璨镶嵌等级
 *
 * \param ob: 装备璀璨镶嵌物品
 * \param base: 装备璀璨镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_bright_downgrade(zObject* ob, zUbrightObjectB* base)
{

	if (ob->data.itemkscuican > 0) 
	{
		--ob->data.itemkscuican;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p11damage )			// 装备璀璨镶嵌 最小攻击力
		COMPUTE( m11axpdamage )			// 装备璀璨镶嵌 最大攻击力
		COMPUTE( m11damage )			// 装备璀璨镶嵌 最小法术攻击力
		COMPUTE( m11axmdamage )			// 装备璀璨镶嵌 最大法术攻击力
	
		COMPUTE( p11defence )			// 装备璀璨镶嵌 物防
		COMPUTE( m11defence )			// 装备璀璨镶嵌 魔防
		COMPUTE( m11axhp )			    // 装备璀璨镶嵌 最大生命值
	}
	
	return true;	
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

/**     
 * \brief 提升物品装备神斧镶嵌等级
 *
 * \param ob: 装备神斧镶嵌物品
 * \param base: 装备神斧镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_xuanhua_upgrade(zObject* ob, zUxuanhuaObjectB* base)
{
	++ob->data.retain32;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p32damage )			// 装备神斧镶嵌 最小攻击力
	COMPUTE( m32axpdamage )			// 装备神斧镶嵌 最大攻击力
	COMPUTE( m32damage )		    // 装备神斧镶嵌 最小法术攻击力
	COMPUTE( m32axmdamage )			// 装备神斧镶嵌 最大法术攻击力

	COMPUTE( p32defence )			// 装备神斧镶嵌 物防
	COMPUTE( m32defence )			// 装备神斧镶嵌 魔防
	COMPUTE( m32axhp )			    // 装备神斧镶嵌 最大生命值
	
	return true;
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品装备神斧镶嵌等级
 *
 * \param ob: 装备神斧镶嵌物品
 * \param base: 装备神斧镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_xuanhua_downgrade(zObject* ob, zUxuanhuaObjectB* base)
{

	if (ob->data.retain32 > 0) 
	{
		--ob->data.retain32;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p32damage )			// 装备神斧镶嵌 最小攻击力
		COMPUTE( m32axpdamage )			// 装备神斧镶嵌 最大攻击力
		COMPUTE( m32damage )			// 装备神斧镶嵌 最小法术攻击力
		COMPUTE( m32axmdamage )			// 装备神斧镶嵌 最大法术攻击力
	
		COMPUTE( p32defence )			// 装备神斧镶嵌 物防
		COMPUTE( m32defence )			// 装备神斧镶嵌 魔防
		COMPUTE( m32axhp )			    // 装备神斧镶嵌 最大生命值
	}
	
	return true;	
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

/**     
 * \brief 提升物品装备黑曜镶嵌等级
 *
 * \param ob: 装备黑曜镶嵌物品
 * \param base: 装备黑曜镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_heiyao_upgrade(zObject* ob, zUheiyaoObjectB* base)
{
	++ob->data.retain33;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p33damage )			// 装备黑曜镶嵌 最小攻击力
	COMPUTE( m33axpdamage )			// 装备黑曜镶嵌 最大攻击力
	COMPUTE( m33damage )		    // 装备黑曜镶嵌 最小法术攻击力
	COMPUTE( m33axmdamage )			// 装备黑曜镶嵌 最大法术攻击力

	COMPUTE( p33defence )			// 装备黑曜镶嵌 物防
	COMPUTE( m33defence )			// 装备黑曜镶嵌 魔防
	COMPUTE( m33axhp )			    // 装备黑曜镶嵌 最大生命值
	
	return true;
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品装备黑曜镶嵌等级
 *
 * \param ob: 装备黑曜镶嵌物品
 * \param base: 装备黑曜镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_heiyao_downgrade(zObject* ob, zUheiyaoObjectB* base)
{

	if (ob->data.retain33 > 0) 
	{
		--ob->data.retain33;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p33damage )			// 装备黑曜镶嵌 最小攻击力
		COMPUTE( m33axpdamage )			// 装备黑曜镶嵌 最大攻击力
		COMPUTE( m33damage )			// 装备黑曜镶嵌 最小法术攻击力
		COMPUTE( m33axmdamage )			// 装备黑曜镶嵌 最大法术攻击力
	
		COMPUTE( p33defence )			// 装备黑曜镶嵌 物防
		COMPUTE( m33defence )			// 装备黑曜镶嵌 魔防
		COMPUTE( m33axhp )			    // 装备黑曜镶嵌 最大生命值
	}
	
	return true;	
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

/**     
 * \brief 提升物品装备精金镶嵌等级
 *
 * \param ob: 装备精金镶嵌物品
 * \param base: 装备精金镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_jingjin_upgrade(zObject* ob, zUjingjinObjectB* base)
{
	++ob->data.retain34;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p34damage )			// 装备精金镶嵌 最小攻击力
	COMPUTE( m34axpdamage )			// 装备精金镶嵌 最大攻击力
	COMPUTE( m34damage )		    // 装备精金镶嵌 最小法术攻击力
	COMPUTE( m34axmdamage )			// 装备精金镶嵌 最大法术攻击力

	COMPUTE( p34defence )			// 装备精金镶嵌 物防
	COMPUTE( m34defence )			// 装备精金镶嵌 魔防
	COMPUTE( m34axhp )			    // 装备精金镶嵌 最大生命值
	
	return true;
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品装备精金镶嵌等级
 *
 * \param ob: 装备精金镶嵌物品
 * \param base: 装备精金镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_jingjin_downgrade(zObject* ob, zUjingjinObjectB* base)
{

	if (ob->data.retain34 > 0) 
	{
		--ob->data.retain34;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p34damage )			// 装备精金镶嵌 最小攻击力
		COMPUTE( m34axpdamage )			// 装备精金镶嵌 最大攻击力
		COMPUTE( m34damage )			// 装备精金镶嵌 最小法术攻击力
		COMPUTE( m34axmdamage )			// 装备精金镶嵌 最大法术攻击力
	
		COMPUTE( p34defence )			// 装备精金镶嵌 物防
		COMPUTE( m34defence )			// 装备精金镶嵌 魔防
		COMPUTE( m34axhp )			    // 装备精金镶嵌 最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

/**     
 * \brief 提升物品金箍咒强化嵌等级
 *
 * \param ob: 装备金箍镶嵌物品
 * \param base: 装备金箍镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_jgzqh_upgrade(zObject* ob, zUjgzqhObjectB* base)
{
	++ob->data.syxqgrade21;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p49damage )			// 装备金箍镶嵌 最小攻击力
	COMPUTE( m49axpdamage )			// 装备金箍镶嵌 最大攻击力
	COMPUTE( m49damage )		    // 装备金箍镶嵌 最小法术攻击力
	COMPUTE( m49axmdamage )			// 装备金箍镶嵌 最大法术攻击力

	COMPUTE( p49defence )			// 装备金箍镶嵌 物防
	COMPUTE( m49defence )			// 装备金箍镶嵌 魔防
	COMPUTE( m49axhp )			    // 装备金箍镶嵌 最大生命值
	
	return true;
}
#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序
/**     
 * \brief 降低物品金箍咒强化等级
 *
 * \param ob: 装备金箍镶嵌物品
 * \param base: 装备金箍镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_jgzqh_downgrade(zObject* ob, zUjgzqhObjectB* base)
{

	if (ob->data.syxqgrade21 > 0) 
	{
		--ob->data.syxqgrade21;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p49damage )			// 装备金箍镶嵌 最小攻击力
		COMPUTE( m49axpdamage )			// 装备金箍镶嵌 最大攻击力
		COMPUTE( m49damage )			// 装备金箍镶嵌 最小法术攻击力
		COMPUTE( m49axmdamage )			// 装备金箍镶嵌 最大法术攻击力
	
		COMPUTE( p49defence )			// 装备金箍镶嵌 物防
		COMPUTE( m49defence )			// 装备金箍镶嵌 魔防
		COMPUTE( m49axhp )			    // 装备金箍镶嵌 最大生命值
	}
	
	return true;	
}
#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

/**     
 * \brief 提升物品金箍咒字纹嵌等级
 *
 * \param ob: 装备金箍镶嵌物品
 * \param base: 装备金箍镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_jgzzw_upgrade(zObject* ob, zUjgzzwObjectB* base)
{
	++ob->data.syxqgrade22;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p50damage )			// 装备金箍镶嵌 最小攻击力
	COMPUTE( m50axpdamage )			// 装备金箍镶嵌 最大攻击力
	COMPUTE( m50damage )		    // 装备金箍镶嵌 最小法术攻击力
	COMPUTE( m50axmdamage )			// 装备金箍镶嵌 最大法术攻击力

	COMPUTE( p50defence )			// 装备金箍镶嵌 物防
	COMPUTE( m50defence )			// 装备金箍镶嵌 魔防
	COMPUTE( m50axhp )			    // 装备金箍镶嵌 最大生命值
	
	return true;
}
#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序
/**     
 * \brief 降低物品金箍咒字纹等级
 *
 * \param ob: 装备金箍镶嵌物品
 * \param base: 装备金箍镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_jgzzw_downgrade(zObject* ob, zUjgzzwObjectB* base)
{

	if (ob->data.syxqgrade22 > 0) 
	{
		--ob->data.syxqgrade22;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p50damage )			// 装备金箍镶嵌 最小攻击力
		COMPUTE( m50axpdamage )			// 装备金箍镶嵌 最大攻击力
		COMPUTE( m50damage )			// 装备金箍镶嵌 最小法术攻击力
		COMPUTE( m50axmdamage )			// 装备金箍镶嵌 最大法术攻击力
	
		COMPUTE( p50defence )			// 装备金箍镶嵌 物防
		COMPUTE( m50defence )			// 装备金箍镶嵌 魔防
		COMPUTE( m50axhp )			    // 装备金箍镶嵌 最大生命值
	}
	
	return true;	
}
#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

/**     
 * \brief 提升物品金箍咒淬炼嵌等级
 *
 * \param ob: 装备金箍镶嵌物品
 * \param base: 装备金箍镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_jgzcl_upgrade(zObject* ob, zUjgzclObjectB* base)
{
	++ob->data.syxqgrade23;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p51damage )			// 装备金箍镶嵌 最小攻击力
	COMPUTE( m51axpdamage )			// 装备金箍镶嵌 最大攻击力
	COMPUTE( m51damage )		    // 装备金箍镶嵌 最小法术攻击力
	COMPUTE( m51axmdamage )			// 装备金箍镶嵌 最大法术攻击力

	COMPUTE( p51defence )			// 装备金箍镶嵌 物防
	COMPUTE( m51defence )			// 装备金箍镶嵌 魔防
	COMPUTE( m51axhp )			    // 装备金箍镶嵌 最大生命值
	
	return true;
}
#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序
/**     
 * \brief 降低物品金箍咒淬炼等级
 *
 * \param ob: 装备金箍镶嵌物品
 * \param base: 装备金箍镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_jgzcl_downgrade(zObject* ob, zUjgzclObjectB* base)
{

	if (ob->data.syxqgrade23 > 0) 
	{
		--ob->data.syxqgrade23;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p51damage )			// 装备金箍镶嵌 最小攻击力
		COMPUTE( m51axpdamage )			// 装备金箍镶嵌 最大攻击力
		COMPUTE( m51damage )			// 装备金箍镶嵌 最小法术攻击力
		COMPUTE( m51axmdamage )			// 装备金箍镶嵌 最大法术攻击力
	
		COMPUTE( p51defence )			// 装备金箍镶嵌 物防
		COMPUTE( m51defence )			// 装备金箍镶嵌 魔防
		COMPUTE( m51axhp )			    // 装备金箍镶嵌 最大生命值
	}
	
	return true;	
}
#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

/**     
 * \brief 提升物品寒冰神炼等级
 *
 * \param ob: 装备寒冰镶嵌物品
 * \param base: 装备寒冰镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_hbsl_upgrade(zObject* ob, zUhbslObjectB* base)
{
	++ob->data.syxqgrade24;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p52damage )			// 装备寒冰镶嵌 最小攻击力
	COMPUTE( m52axpdamage )			// 装备寒冰镶嵌 最大攻击力
	COMPUTE( m52damage )		    // 装备寒冰镶嵌 最小法术攻击力
	COMPUTE( m52axmdamage )			// 装备寒冰镶嵌 最大法术攻击力

	COMPUTE( p52defence )			// 装备寒冰镶嵌 物防
	COMPUTE( m52defence )			// 装备寒冰镶嵌 魔防
	COMPUTE( m52axhp )			    // 装备寒冰镶嵌 最大生命值
	
	return true;
}
#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序
/**     
 * \brief 降低物品寒冰神炼等级
 *
 * \param ob: 装备寒冰镶嵌物品
 * \param base: 装备寒冰镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_hbsl_downgrade(zObject* ob, zUhbslObjectB* base)
{

	if (ob->data.syxqgrade24 > 0) 
	{
		--ob->data.syxqgrade24;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p52damage )			// 装备寒冰镶嵌 最小攻击力
		COMPUTE( m52axpdamage )			// 装备寒冰镶嵌 最大攻击力
		COMPUTE( m52damage )			// 装备寒冰镶嵌 最小法术攻击力
		COMPUTE( m52axmdamage )			// 装备寒冰镶嵌 最大法术攻击力
	
		COMPUTE( p52defence )			// 装备寒冰镶嵌 物防
		COMPUTE( m52defence )			// 装备寒冰镶嵌 魔防
		COMPUTE( m52axhp )			    // 装备寒冰镶嵌 最大生命值
	}
	
	return true;	
}
#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

/**     
 * \brief 提升物品寒冰字纹等级
 *
 * \param ob: 装备寒冰镶嵌物品
 * \param base: 装备寒冰镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_hbzw_upgrade(zObject* ob, zUhbzwObjectB* base)
{
	++ob->data.syxqgrade25;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p53damage )			// 装备寒冰镶嵌 最小攻击力
	COMPUTE( m53axpdamage )			// 装备寒冰镶嵌 最大攻击力
	COMPUTE( m53damage )		    // 装备寒冰镶嵌 最小法术攻击力
	COMPUTE( m53axmdamage )			// 装备寒冰镶嵌 最大法术攻击力

	COMPUTE( p53defence )			// 装备寒冰镶嵌 物防
	COMPUTE( m53defence )			// 装备寒冰镶嵌 魔防
	COMPUTE( m53axhp )			    // 装备寒冰镶嵌 最大生命值
	
	return true;
}
#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序
/**     
 * \brief 降低物品寒冰字纹等级
 *
 * \param ob: 装备寒冰镶嵌物品
 * \param base: 装备寒冰镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_hbzw_downgrade(zObject* ob, zUhbzwObjectB* base)
{

	if (ob->data.syxqgrade25 > 0) 
	{
		--ob->data.syxqgrade25;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p53damage )			// 装备寒冰镶嵌 最小攻击力
		COMPUTE( m53axpdamage )			// 装备寒冰镶嵌 最大攻击力
		COMPUTE( m53damage )			// 装备寒冰镶嵌 最小法术攻击力
		COMPUTE( m53axmdamage )			// 装备寒冰镶嵌 最大法术攻击力
	
		COMPUTE( p53defence )			// 装备寒冰镶嵌 物防
		COMPUTE( m53defence )			// 装备寒冰镶嵌 魔防
		COMPUTE( m53axhp )			    // 装备寒冰镶嵌 最大生命值
	}
	
	return true;	
}
#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

/**     
 * \brief 提升物品寒冰纹章等级
 *
 * \param ob: 装备寒冰镶嵌物品
 * \param base: 装备寒冰镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_hbwz_upgrade(zObject* ob, zUhbwzObjectB* base)
{
	++ob->data.syxqgrade26;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p54damage )			// 装备寒冰镶嵌 最小攻击力
	COMPUTE( m54axpdamage )			// 装备寒冰镶嵌 最大攻击力
	COMPUTE( m54damage )		    // 装备寒冰镶嵌 最小法术攻击力
	COMPUTE( m54axmdamage )			// 装备寒冰镶嵌 最大法术攻击力

	COMPUTE( p54defence )			// 装备寒冰镶嵌 物防
	COMPUTE( m54defence )			// 装备寒冰镶嵌 魔防
	COMPUTE( m54axhp )			    // 装备寒冰镶嵌 最大生命值
	
	return true;
}
#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序
/**     
 * \brief 降低物品寒冰纹章等级
 *
 * \param ob: 装备寒冰镶嵌物品
 * \param base: 装备寒冰镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_hbwz_downgrade(zObject* ob, zUhbwzObjectB* base)
{

	if (ob->data.syxqgrade26 > 0) 
	{
		--ob->data.syxqgrade26;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p54damage )			// 装备寒冰镶嵌 最小攻击力
		COMPUTE( m54axpdamage )			// 装备寒冰镶嵌 最大攻击力
		COMPUTE( m54damage )			// 装备寒冰镶嵌 最小法术攻击力
		COMPUTE( m54axmdamage )			// 装备寒冰镶嵌 最大法术攻击力
	
		COMPUTE( p54defence )			// 装备寒冰镶嵌 物防
		COMPUTE( m54defence )			// 装备寒冰镶嵌 魔防
		COMPUTE( m54axhp )			    // 装备寒冰镶嵌 最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;

/**     
 * \brief 提升物品图鉴进阶等级
 *
 * \param ob: 图鉴进阶物品
 * \param base: 图鉴进阶属性
 * \return 当前总是返回true
 */
bool Upgrade::do_handbook_upgrade(zObject* ob, zUhandbookObjectB* base)
{
	++ob->data.handbooklvg;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p13damage )				// 图鉴进阶最小攻击力
	COMPUTE( m13axpdamage )			// 图鉴进阶最大攻击力
	COMPUTE( m13damage )			    // 图鉴进阶最小法术攻击力
	COMPUTE( m13axmdamage )			// 图鉴进阶最大法术攻击力

	COMPUTE( p13defence )			// 图鉴进阶物防
	COMPUTE( m13defence )			// 图鉴进阶魔防
	COMPUTE( m13axhp )			    // 图鉴进阶最大生命值
	
	return true;
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品图鉴进阶等级
 *
 * \param ob: 图鉴进阶物品
 * \param base: 图鉴进阶属性
 * \return 当前总是返回true
 */
bool Upgrade::do_handbook_downgrade(zObject* ob, zUhandbookObjectB* base)
{

	if (ob->data.handbooklvg > 0) 
	{
		--ob->data.handbooklvg;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p13damage )				// 图鉴进阶最小攻击力
		COMPUTE( m13axpdamage )			// 图鉴进阶最大攻击力
		COMPUTE( m13damage )			    // 图鉴进阶最小法术攻击力
		COMPUTE( m13axmdamage )			// 图鉴进阶最大法术攻击力
	
		COMPUTE( p13defence )			// 图鉴进阶物防
		COMPUTE( m13defence )			// 图鉴进阶魔防
		COMPUTE( m13axhp )			    // 图鉴进阶最大生命值
	}
	
	return true;	
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品十二星宿等级
 *
 * \param ob: 十二星宿物品
 * \param base: 十二星宿属性
 * \return 当前总是返回true
 */
bool Upgrade::do_SEXX_upgrade(zObject* ob, zUSEXXObjectB* base)
{
	++ob->data.retain21;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p21damage )				// 十二星宿最小攻击力
	COMPUTE( m21axpdamage )			// 十二星宿最大攻击力
	COMPUTE( m21damage )			    // 十二星宿最小法术攻击力
	COMPUTE( m21axmdamage )			// 十二星宿最大法术攻击力

	COMPUTE( p21defence )			// 十二星宿物防
	COMPUTE( m21defence )			// 十二星宿魔防
	COMPUTE( m21axhp )			    // 十二星宿最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品十二星宿等级
 *
 * \param ob: 十二星宿物品
 * \param base: 十二星宿属性
 * \return 当前总是返回true
 */
bool Upgrade::do_SEXX_downgrade(zObject* ob, zUSEXXObjectB* base)
{

	if (ob->data.retain21 > 0) 
	{
		--ob->data.retain21;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p21damage )				// 十二星宿最小攻击力
		COMPUTE( m21axpdamage )			// 十二星宿最大攻击力
		COMPUTE( m21damage )			    // 十二星宿最小法术攻击力
		COMPUTE( m21axmdamage )			// 十二星宿最大法术攻击力
	
		COMPUTE( p21defence )			// 十二星宿物防
		COMPUTE( m21defence )			// 十二星宿魔防
		COMPUTE( m21axhp )			    // 十二星宿最大生命值
	}
	
	return true;	
}



#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品法宝等级
 *
 * \param ob: 法宝物品
 * \param base: 法宝属性
 * \return 当前总是返回true
 */
bool Upgrade::do_fabao_upgrade(zObject* ob, zUfabaoObjectB* base)
{
	++ob->data.retain20;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p20damage )				// 法宝最小攻击力
	COMPUTE( m20axpdamage )			// 法宝最大攻击力
	COMPUTE( m20damage )			    // 法宝最小法术攻击力
	COMPUTE( m20axmdamage )			// 法宝最大法术攻击力

	COMPUTE( p20defence )			// 法宝物防
	COMPUTE( m20defence )			// 法宝魔防
	COMPUTE( m20axhp )			    // 法宝最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品法宝等级
 *
 * \param ob: 法宝物品
 * \param base: 法宝属性
 * \return 当前总是返回true
 */
bool Upgrade::do_fabao_downgrade(zObject* ob, zUfabaoObjectB* base)
{

	if (ob->data.retain20 > 0) 
	{
		--ob->data.retain20;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p20damage )				// 法宝最小攻击力
		COMPUTE( m20axpdamage )			// 法宝最大攻击力
		COMPUTE( m20damage )			    // 法宝最小法术攻击力
		COMPUTE( m20axmdamage )			// 法宝最大法术攻击力
	
		COMPUTE( p20defence )			// 法宝物防
		COMPUTE( m20defence )			// 法宝魔防
		COMPUTE( m20axhp )			    // 法宝最大生命值
	}
	
	return true;	
}



#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品元神等级
 *
 * \param ob: 元神物品
 * \param base: 元神属性
 * \return 当前总是返回true
 */
bool Upgrade::do_YUANSHEN_upgrade(zObject* ob, zUYUANSHENObjectB* base)
{
	++ob->data.retain22;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p22damage )				// 元神最小攻击力
	COMPUTE( m22axpdamage )			// 元神最大攻击力
	COMPUTE( m22damage )			    // 元神最小法术攻击力
	COMPUTE( m22axmdamage )			// 元神最大法术攻击力

	COMPUTE( p22defence )			// 元神物防
	COMPUTE( m22defence )			// 元神魔防
	COMPUTE( m22axhp )			    // 元神最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品元神等级
 *
 * \param ob: 元神物品
 * \param base: 元神属性
 * \return 当前总是返回true
 */
bool Upgrade::do_YUANSHEN_downgrade(zObject* ob, zUYUANSHENObjectB* base)
{

	if (ob->data.retain22 > 0) 
	{
		--ob->data.retain22;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p22damage )				// 元神最小攻击力
		COMPUTE( m22axpdamage )			// 元神最大攻击力
		COMPUTE( m22damage )			    // 元神最小法术攻击力
		COMPUTE( m22axmdamage )			// 元神最大法术攻击力
	
		COMPUTE( p22defence )			// 元神物防
		COMPUTE( m22defence )			// 元神魔防
		COMPUTE( m22axhp )			    // 元神最大生命值
	}
	
	return true;	
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品元神等级
 *
 * \param ob: 元神物品
 * \param base: 元神属性
 * \return 当前总是返回true
 */
bool Upgrade::do_YSBS_upgrade(zObject* ob, zUYSBSObjectB* base)
{
	++ob->data.retain23;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p23damage )				// 元神最小攻击力
	COMPUTE( m23axpdamage )			// 元神最大攻击力
	COMPUTE( m23damage )			    // 元神最小法术攻击力
	COMPUTE( m23axmdamage )			// 元神最大法术攻击力

	COMPUTE( p23defence )			// 元神物防
	COMPUTE( m23defence )			// 元神魔防
	COMPUTE( m23axhp )			    // 元神最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品元神等级
 *
 * \param ob: 元神物品
 * \param base: 元神属性
 * \return 当前总是返回true
 */
bool Upgrade::do_YSBS_downgrade(zObject* ob, zUYSBSObjectB* base)
{

	if (ob->data.retain23 > 0) 
	{
		--ob->data.retain23;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p23damage )				// 元神最小攻击力
		COMPUTE( m23axpdamage )			// 元神最大攻击力
		COMPUTE( m23damage )			    // 元神最小法术攻击力
		COMPUTE( m23axmdamage )			// 元神最大法术攻击力
	
		COMPUTE( p23defence )			// 元神物防
		COMPUTE( m23defence )			// 元神魔防
		COMPUTE( m23axhp )			    // 元神最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品翅膀等级
 *
 * \param ob: 翅膀物品
 * \param base: 翅膀属性
 * \return 当前总是返回true
 */
bool Upgrade::do_angel_upgrade(zObject* ob, zUangelObjectB* base)
{
	++ob->data.retain4;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p8damage )				// 翅膀最小攻击力
	COMPUTE( m8axpdamage )			// 翅膀最大攻击力
	COMPUTE( m8damage )			    // 翅膀最小法术攻击力
	COMPUTE( m8axmdamage )			// 翅膀最大法术攻击力

	COMPUTE( p8defence )			// 翅膀物防
	COMPUTE( m8defence )			// 翅膀魔防
	COMPUTE( m8axhp )			    // 翅膀最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品翅膀等级
 *
 * \param ob: 翅膀物品
 * \param base: 翅膀属性
 * \return 当前总是返回true
 */
bool Upgrade::do_angel_downgrade(zObject* ob, zUangelObjectB* base)
{

	if (ob->data.retain4 > 0) 
	{
		--ob->data.retain4;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p8damage )				// 翅膀最小攻击力
		COMPUTE( m8axpdamage )			// 翅膀最大攻击力
		COMPUTE( m8damage )			    // 翅膀最小法术攻击力
		COMPUTE( m8axmdamage )			// 翅膀最大法术攻击力
	
		COMPUTE( p8defence )			// 翅膀物防
		COMPUTE( m8defence )			// 翅膀魔防
		COMPUTE( m8axhp )			    // 翅膀最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品神兵等级
 *
 * \param ob: 神兵物品
 * \param base: 神兵属性
 * \return 当前总是返回true
 */

bool Upgrade::do_magical_upgrade(zObject* ob, zMagicalObjectB* base)
{
	++ob->data.retain5;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p7damage )				// 最小攻击力
	COMPUTE( m7axpdamage )			// 最大攻击力
	COMPUTE( m7damage )			    // 最小法术攻击力
	COMPUTE( m7axmdamage )			// 最大法术攻击力

	COMPUTE( p7defence )			// 物防
	COMPUTE( m7defence )			// 魔防
	COMPUTE( m7axhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品神兵等级
 *
 * \param ob: 神兵物品
 * \param base: 神兵属性
 * \return 当前总是返回true
 */
bool Upgrade::do_magical_downgrade(zObject* ob, zMagicalObjectB* base)
{

	if (ob->data.retain5 > 0) 
	{
		--ob->data.retain5;	
		
		COMPUTE( p7damage )				// 最小攻击力
		COMPUTE( m7axpdamage )			// 最大攻击力
		COMPUTE( m7damage )			    // 最小法术攻击力
		COMPUTE( m7axmdamage )			// 最大法术攻击力
	
		COMPUTE( p7defence )			// 物防
		COMPUTE( m7defence )			// 魔防
		COMPUTE( m7axhp )			    // 最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品神兽等级
 *
 * \param ob: 神兽物品
 * \param base: 神兽属性
 * \return 当前总是返回true
 */

bool Upgrade::do_mythical_upgrade(zObject* ob, zMythicalObjectB* base)
{
	++ob->data.retain6;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p7damage )				// 最小攻击力
	COMPUTE( m7axpdamage )			// 最大攻击力
	COMPUTE( m7damage )			    // 最小法术攻击力
	COMPUTE( m7axmdamage )			// 最大法术攻击力

	COMPUTE( p7defence )			// 物防
	COMPUTE( m7defence )			// 魔防
	COMPUTE( m7axhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品兽等级
 *
 * \param ob: 兽物品
 * \param base: 兽属性
 * \return 当前总是返回true
 */
bool Upgrade::do_mythical_downgrade(zObject* ob, zMythicalObjectB* base)
{

	if (ob->data.retain6 > 0) 
	{
		--ob->data.retain6;	
		
		COMPUTE( p7damage )				// 最小攻击力
		COMPUTE( m7axpdamage )			// 最大攻击力
		COMPUTE( m7damage )			    // 最小法术攻击力
		COMPUTE( m7axmdamage )			// 最大法术攻击力
	
		COMPUTE( p7defence )			// 物防
		COMPUTE( m7defence )			// 魔防
		COMPUTE( m7axhp )			    // 最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品等级
 *
 * \param ob: 升级物品
 * \param base: 升级属性
 * \return 当前总是返回true
 */
bool Upgrade::do_equip(zObject* ob, zEquipObjectB* base)
{
	if (!ob)
	{
		return false;
	}

	zSetObjectB *objbase = setobjectbm.get(ob->data.dwObjectID);
	if (base==NULL) return false;

	zFiveSetB* prop = fivesetbm.get(objbase->mark);
	if (!prop) return false;
		
	ob->data.kind |= 8;//五行套装,参看Object.h注释

	//soke
	//for (int i = 0;i < 5;i++)
	//{
	//	ob->data._five_props[i]=0;//灵魂锁链数据清零
	//}
	ob->data.dpdam = 0;
	ob->data.dmdam = 0;
	ob->data.bdam = 0;
	ob->data.rdam = 0;
	ob->data.ignoredef = 0;
	ob->data.addpdef = 0;
	ob->data.addmdef = 0;
	ob->data.addpdam = 0;
	ob->data.addmdam = 0;
	ob->data.addhp = 0;

	int i = 0;
	int tmpodds = 0;
	for (zSetObjectB::iterator it=objbase->sets.begin(); it!=objbase->sets.end(); ++it) 
	{
		//soke 灵魂锁链
		tmpodds = it->odds;
		if (tmpodds < 500 )
		{
		//	Zebra::logger->debug("it->odds == %d",tmpodds);
			tmpodds = 0;
			continue;
		}
		//Zebra::logger->debug("it->odds == %d",tmpodds);
		//Zebra::logger->debug("(ob->data.kind)&16==%d",(ob->data.kind)&16);
		if (tmpodds==500)
		{
			tmpodds = 0;
			if((ob->data.kind)&16)
			{
				Zebra::logger->debug("two soul");
				tmpodds = 10002;//紫装双灵魂
			}
		}
		//Zebra::logger->debug("it->odds == %d",tmpodds);
		if (zMisc::selectByTenTh(tmpodds)) 
		{
			for (std::vector<WORD>::iterator s_it=(*it).ids.begin(); s_it!=(*it).ids.end(); ++s_it) 
			{
				ob->data.fiveset[i++] = (*it).ids[0];
			}
			int set_prop[] = { 0, 0, 0, 0, 0};	
			int nSoulCount = 0;
			do 
			{
					int index = zMisc::randBetween(0,4);
				//	if (set_prop[index]) continue;
					if (set_prop[index-1]) continue;
					if( nSoulCount == 0)
					{
					//	ob->data.opt1=index;
						switch (index)
						{
						case 0:
							{
								ob->data.dpdam = zMisc::randBetween(prop->dpdam.min, prop->dpdam.max);
								i--;
							}
							break;
							//COMPUTE( dpdam ) //物理伤害减少%x
						case 1:
							{									
								ob->data.dmdam = zMisc::randBetween(prop->dmdam.min, prop->dmdam.max);
								i--;
							}
							break;
						//	COMPUTE( dmdam ) //法术伤害减少%x
						case 2:
							{									
								ob->data.bdam = zMisc::randBetween(prop->bdam.min, prop->bdam.max);
								i--;
							}
							break;
						//	COMPUTE( bdam ) //增加伤害x%
						case 3:
							{								
								ob->data.rdam = zMisc::randBetween(prop->rdam.min, prop->rdam.max);
								i--;
							}
							break;
						//	COMPUTE( rdam ) //伤害反射%x
						case 4:
							{								
								ob->data.ignoredef = zMisc::randBetween(prop->ignoredef.min, prop->ignoredef.max);
								i--;
							}
							break;
						//	COMPUTE( ignoredef ) //%x忽视目标防御

						default:
							break;		
						}

						if (!((ob->data.kind)&16)&&((ob->data.kind)&4))
						{
							break;
						}
					}
					else if ( nSoulCount == 1)
					{
					//	ob->data.opt3=index;
						switch (index)
						{
						case 0:
							{
								ob->data.addpdef = zMisc::randBetween(prop->addpdef.min, prop->addpdef.max);
								--i;
							}
							break;	
						case 1:
							{
								ob->data.addmdef = zMisc::randBetween(prop->addmdef.min, prop->addmdef.max);
								--i;
							}
							break;	
						case 2:
							{
								ob->data.addpdam = zMisc::randBetween(prop->addpdam.min, prop->addpdam.max);
								--i;
							}
							break;	
						case 3:
							{
								ob->data.addmdam = zMisc::randBetween(prop->addmdam.min, prop->addmdam.max);
								--i;
							}
							break;		
						case 4:
							{
								ob->data.addhp = zMisc::randBetween(prop->addhp.min, prop->addhp.max);
								--i;
							}
							break;
							
						default:
							break;		
						}
					}
					nSoulCount++;
				
			}while (i>0);
			
			break;	
		}	
	}
	
	fprintf(stderr,"生成灵魂成功 %s dwId = %u \n",ob->name,ob->data.dwObjectID);
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品等级
 *
 * \param ob: 升级物品
 * \param base: 升级属性
 * \return 当前总是返回true
 */
bool Upgrade::do_downequip(zObject* ob, zEquipObjectB* base)
{

	//if (ob->data.upgrade > 0) 
	//{
		//--ob->data.upgrade;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( pdamage )				// 最小攻击力
		COMPUTE( maxpdamage )			// 最大攻击力
		COMPUTE( mdamage )			// 最小法术攻击力
		COMPUTE( maxmdamage )			// 最大法术攻击力
	
		COMPUTE( pdefence )			// 物防
		COMPUTE( mdefence )				// 魔防
		COMPUTE( maxhp )			// 最大生命值
	//}
	
	return true;	
}

//sky 护身符功能
#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·力量值
 *
 * \param ob: 护身符·力量物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_liliang(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.str; //soke 单次加1
//soke 升星不改变需要的等级
//	++ob->data.needlevel;
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·力量值
 *
 * \param ob: 护身符·力量物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_liliang2(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.str+=2; //soke 单次加2
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·力量值
 *
 * \param ob: 护身符·力量物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_liliang3(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
    ++ob->data.str+=3; //soke 单次加3
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·力量值
 *
 * \param ob: 护身符·力量物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_liliang15(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.str+=15; //soke 单次加15
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·敏捷值
 *
 * \param ob: 护身符·敏捷物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_minjie(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.dex;  //soke 单次加1
//soke 升星不改变需要的等级
//	++ob->data.needlevel;
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·敏捷值
 *
 * \param ob: 护身符·敏捷物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_minjie2(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.dex+=2; //soke 单次加2
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·敏捷值
 *
 * \param ob: 护身符·敏捷物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_minjie3(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.dex+=3; //soke 单次加3
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·敏捷值
 *
 * \param ob: 护身符·敏捷物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_minjie15(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.dex+=15; //soke 单次加15
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·智力值
 *
 * \param ob: 护身符·智力物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_zhili(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.inte; //soke 单次加1
//soke 升星不改变需要的等级
//	++ob->data.needlevel;
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·智力值
 *
 * \param ob: 护身符·智力物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_zhili2(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.inte+=2; //soke 单次加2
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·智力值
 *
 * \param ob: 护身符·智力物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_zhili3(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.inte+=3; //soke 单次加3
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·智力值
 *
 * \param ob: 护身符·智力物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_zhili15(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.inte+=15; //soke 单次加15
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·体质值
 *
 * \param ob: 护身符·体质物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_tizhi(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.con; //soke 单次加1
//soke 升星不改变需要的等级
//	++ob->data.needlevel;
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·体质值
 *
 * \param ob: 护身符·体质物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_tizhi2(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.con+=2; //soke 单次加2
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·体质值
 *
 * \param ob: 护身符·体质物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_tizhi3(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.con+=3; //soke 单次加3
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·体质值
 *
 * \param ob: 护身符·体质物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_tizhi15(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.con+=15; //soke 单次加15
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·精神值
 *
 * \param ob: 护身符·精神物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_jingshen(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.spi; //soke 单次加1
//soke 升星不改变需要的等级
//	++ob->data.needlevel;
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·精神值
 *
 * \param ob: 护身符·精神物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_jingshen2(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.spi+=2; //soke 单次加2
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·精神值
 *
 * \param ob: 护身符·精神物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_jingshen3(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.spi+=3; //soke 单次加3
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升护身符·精神值
 *
 * \param ob: 护身符·精神物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_jingshen15(zObject* ob, zAmuletObjectB* base)
{
	++ob->data.upgrade;
	++ob->data.spi+=15; //soke 单次加15
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

/**     
 * \brief 提升护身符·力量(白)-(紫)
 *
 * \param ob: 护身符·力量物品
 * \param base: 值属性
 * \return 当前总是返回true
 */
bool Upgrade::do_lanupgrade(zObject* ob, zAmuletObjectB* base)
{
	ob->data.upgrade = 0;
	++ob->data.dwObjectID;
	
	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )			    // 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )			    // 物防
	COMPUTE( mdefence )			    // 魔防
	COMPUTE( maxhp )			    // 最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低护身符星星等级
 *
 * \param ob: 护身符物品
 * \param base: 护身符属性
 * \return 当前总是返回true
 */
bool Upgrade::hsf_downgrade(zObject* ob, zAmuletObjectB* base)
{

	if (ob->data.upgrade > 0) 
	{
		--ob->data.upgrade;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( pdamage )				// 最小攻击力
		COMPUTE( maxpdamage )			// 最大攻击力
		COMPUTE( mdamage )			    // 最小法术攻击力
		COMPUTE( maxmdamage )			// 最大法术攻击力
	
		COMPUTE( pdefence )			// 物防
		COMPUTE( mdefence )			// 魔防
		COMPUTE( maxhp )			    // 最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品神龙镶嵌等级
 *
 * \param ob: 神龙镶嵌物品
 * \param base: 神龙镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_longwanglv_upgrade(zObject* ob, zUlongwanglvObjectB* base)
{
	++ob->data.longwanglv;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p14damage )				// 神龙镶嵌最小攻击力
	COMPUTE( m14axpdamage )			// 神龙镶嵌最大攻击力
	COMPUTE( m14damage )			    // 神龙镶嵌最小法术攻击力
	COMPUTE( m14axmdamage )			// 神龙镶嵌最大法术攻击力

	COMPUTE( p14defence )			// 神龙镶嵌物防
	COMPUTE( m14defence )			// 神龙镶嵌魔防
	COMPUTE( m14axhp )			    // 神龙镶嵌最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品神龙镶嵌等级
 *
 * \param ob: 神龙镶嵌物品
 * \param base: 神龙镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_longwanglv_downgrade(zObject* ob, zUlongwanglvObjectB* base)
{

	if (ob->data.longwanglv > 0) 
	{
		--ob->data.longwanglv;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p14damage )				// 神龙镶嵌最小攻击力
		COMPUTE( m14axpdamage )			// 神龙镶嵌最大攻击力
		COMPUTE( m14damage )			    // 神龙镶嵌最小法术攻击力
		COMPUTE( m14axmdamage )			// 神龙镶嵌最大法术攻击力
	
		COMPUTE( p14defence )			// 神龙镶嵌物防
		COMPUTE( m14defence )			// 神龙镶嵌魔防
		COMPUTE( m14axhp )			    // 神龙镶嵌最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品逆天进阶等级
 *
 * \param ob: 逆天进阶物品
 * \param base: 逆天进阶属性
 * \return 当前总是返回true
 */
bool Upgrade::do_nitianlv_upgrade(zObject* ob, zUnitianlvObjectB* base)
{
	++ob->data.nitianlv;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p15damage )				// 逆天进阶最小攻击力
	COMPUTE( m15axpdamage )			// 逆天进阶最大攻击力
	COMPUTE( m15damage )			    // 逆天进阶最小法术攻击力
	COMPUTE( m15axmdamage )			// 逆天进阶最大法术攻击力

	COMPUTE( p15defence )			// 逆天进阶物防
	COMPUTE( m15defence )			// 逆天进阶魔防
	COMPUTE( m15axhp )			    // 逆天进阶最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品逆天进阶等级
 *
 * \param ob: 逆天进阶物品
 * \param base: 逆天进阶属性
 * \return 当前总是返回true
 */
bool Upgrade::do_nitianlv_downgrade(zObject* ob, zUnitianlvObjectB* base)
{

	if (ob->data.nitianlv > 0) 
	{
		--ob->data.nitianlv;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p15damage )				// 逆天进阶最小攻击力
		COMPUTE( m15axpdamage )			// 逆天进阶最大攻击力
		COMPUTE( m15damage )			    // 逆天进阶最小法术攻击力
		COMPUTE( m15axmdamage )			// 逆天进阶最大法术攻击力
	
		COMPUTE( p15defence )			// 逆天进阶物防
		COMPUTE( m15defence )			// 逆天进阶魔防
		COMPUTE( m15axhp )			    // 逆天进阶最大生命值
	}
	
	return true;	
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品神魂宝石镶嵌进阶等级
 *
 * \param ob: 神魂宝石镶嵌进阶物品
 * \param base: 神魂宝石镶嵌进阶属性
 * \return 当前总是返回true
 */
bool Upgrade::do_douzhansflv_upgrade(zObject* ob, zUdouzhansflvObjectB* base)
{
	++ob->data.douzhansflv;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p16damage )				// 神魂宝石镶嵌进阶最小攻击力
	COMPUTE( m16axpdamage )			// 神魂宝石镶嵌进阶最大攻击力
	COMPUTE( m16damage )			    // 神魂宝石镶嵌进阶最小法术攻击力
	COMPUTE( m16axmdamage )			// 神魂宝石镶嵌进阶最大法术攻击力

	COMPUTE( p16defence )			// 神魂宝石镶嵌进阶物防
	COMPUTE( m16defence )			// 神魂宝石镶嵌进阶魔防
	COMPUTE( m16axhp )			    // 神魂宝石镶嵌进阶最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品神魂宝石镶嵌进阶等级
 *
 * \param ob: 神魂宝石镶嵌进阶物品
 * \param base: 神魂宝石镶嵌进阶属性
 * \return 当前总是返回true
 */
bool Upgrade::do_douzhansflv_downgrade(zObject* ob, zUdouzhansflvObjectB* base)
{

	if (ob->data.douzhansflv > 0) 
	{
		--ob->data.douzhansflv;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p16damage )				// 神魂宝石镶嵌进阶最小攻击力
		COMPUTE( m16axpdamage )			// 神魂宝石镶嵌进阶最大攻击力
		COMPUTE( m16damage )			    // 神魂宝石镶嵌进阶最小法术攻击力
		COMPUTE( m16axmdamage )			// 神魂宝石镶嵌进阶最大法术攻击力
	
		COMPUTE( p16defence )			// 神魂宝石镶嵌进阶物防
		COMPUTE( m16defence )			// 神魂宝石镶嵌进阶魔防
		COMPUTE( m16axhp )			    // 神魂宝石镶嵌进阶最大生命值
	}
	
	return true;	
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品暗影宝石镶嵌等级
 *
 * \param ob: 暗影宝石镶嵌物品
 * \param base: 暗影宝石镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_aystonelv_upgrade(zObject* ob, zUaystonelvObjectB* base)
{
	++ob->data.aystonelv;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p17damage )				// 暗影宝石镶嵌最小攻击力
	COMPUTE( m17axpdamage )			// 暗影宝石镶嵌最大攻击力
	COMPUTE( m17damage )			    // 暗影宝石镶嵌最小法术攻击力
	COMPUTE( m17axmdamage )			// 暗影宝石镶嵌最大法术攻击力

	COMPUTE( p17defence )			// 暗影宝石镶嵌物防
	COMPUTE( m17defence )			// 暗影宝石镶嵌魔防
	COMPUTE( m17axhp )			    // 暗影宝石镶嵌最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品暗影宝石镶嵌等级
 *
 * \param ob: 暗影宝石镶嵌物品
 * \param base: 暗影宝石镶嵌属性
 * \return 当前总是返回true
 */
bool Upgrade::do_aystonelv_downgrade(zObject* ob, zUaystonelvObjectB* base)
{

	if (ob->data.aystonelv > 0) 
	{
		--ob->data.aystonelv;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p17damage )				// 暗影宝石镶嵌最小攻击力
		COMPUTE( m17axpdamage )			// 暗影宝石镶嵌最大攻击力
		COMPUTE( m17damage )			    // 暗影宝石镶嵌最小法术攻击力
		COMPUTE( m17axmdamage )			// 暗影宝石镶嵌最大法术攻击力
	
		COMPUTE( p17defence )			// 暗影宝石镶嵌物防
		COMPUTE( m17defence )			// 暗影宝石镶嵌魔防
		COMPUTE( m17axhp )			    // 暗影宝石镶嵌最大生命值
	}
	
	return true;	
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品尚方宝剑进阶等级
 *
 * \param ob: 尚方宝剑进阶物品
 * \param base: 尚方宝剑进阶属性
 * \return 当前总是返回true
 */
bool Upgrade::do_shangfang_upgrade(zObject* ob, zUshangfanglvObjectB* base)
{
	++ob->data.shangfanglv;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p18damage )				// 尚方宝剑进阶最小攻击力
	COMPUTE( m18axpdamage )			// 尚方宝剑进阶最大攻击力
	COMPUTE( m18damage )			    // 尚方宝剑进阶最小法术攻击力
	COMPUTE( m18axmdamage )			// 尚方宝剑进阶最大法术攻击力

	COMPUTE( p18defence )			// 尚方宝剑进阶物防
	COMPUTE( m18defence )			// 尚方宝剑进阶魔防
	COMPUTE( m18axhp )			    // 尚方宝剑进阶最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品尚方宝剑进阶等级
 *
 * \param ob: 尚方宝剑进阶物品
 * \param base: 尚方宝剑进阶属性
 * \return 当前总是返回true
 */
bool Upgrade::do_shangfang_downgrade(zObject* ob, zUshangfanglvObjectB* base)
{

	if (ob->data.shangfanglv > 0) 
	{
		--ob->data.shangfanglv;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p18damage )				// 尚方宝剑进阶最小攻击力
		COMPUTE( m18axpdamage )			// 尚方宝剑进阶最大攻击力
		COMPUTE( m18damage )			    // 尚方宝剑进阶最小法术攻击力
		COMPUTE( m18axmdamage )			// 尚方宝剑进阶最大法术攻击力
	
		COMPUTE( p18defence )			// 尚方宝剑进阶物防
		COMPUTE( m18defence )			// 尚方宝剑进阶魔防
		COMPUTE( m18axhp )			    // 尚方宝剑进阶最大生命值
	}
	
	return true;	
}


#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**     
 * \brief 提升物品紫金龙袍进阶等级
 *
 * \param ob: 紫金龙袍进阶物品
 * \param base: 紫金龙袍进阶属性
 * \return 当前总是返回true
 */
bool Upgrade::do_zijinlp_upgrade(zObject* ob, zUzijinlplvObjectB* base)
{
	++ob->data.zijinlplv;
    //soke 升星不改变需要的等级
    //++ob->data.needlevel;
	
	COMPUTE( p19damage )				// 紫金龙袍进阶最小攻击力
	COMPUTE( m19axpdamage )			// 紫金龙袍进阶最大攻击力
	COMPUTE( m19damage )			    // 紫金龙袍进阶最小法术攻击力
	COMPUTE( m19axmdamage )			// 紫金龙袍进阶最大法术攻击力

	COMPUTE( p19defence )			// 紫金龙袍进阶物防
	COMPUTE( m19defence )			// 紫金龙袍进阶魔防
	COMPUTE( m19axhp )			    // 紫金龙袍进阶最大生命值
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x -= base->x; \
if ((SWORD)ob->data.x < 0) ob->data.x = 0; //靠靠靠，不必要的预防，垃圾的代码，白痴的策划，无奈的程序


/**     
 * \brief 降低物品紫金龙袍进阶等级
 *
 * \param ob: 紫金龙袍进阶物品
 * \param base: 紫金龙袍进阶属性
 * \return 当前总是返回true
 */
bool Upgrade::do_zijinlp_downgrade(zObject* ob, zUzijinlplvObjectB* base)
{

	if (ob->data.zijinlplv > 0) 
	{
		--ob->data.zijinlplv;	
    //soke 即使升星失败也不降低等级
	//	--ob->data.needlevel;
		
		COMPUTE( p19damage )				// 紫金龙袍进阶最小攻击力
		COMPUTE( m19axpdamage )			// 紫金龙袍进阶最大攻击力
		COMPUTE( m19damage )			    // 紫金龙袍进阶最小法术攻击力
		COMPUTE( m19axmdamage )			// 紫金龙袍进阶最大法术攻击力
	
		COMPUTE( p19defence )			// 紫金龙袍进阶物防
		COMPUTE( m19defence )			// 紫金龙袍进阶魔防
		COMPUTE( m19axhp )			    // 紫金龙袍进阶最大生命值
	}
	
	return true;	
}

const int Decompose::_odds[] = 
			{	0, 		0, 		0, 		0, 		0, 		0, 		0, 		0, 		0, 		0, //white 0
		   		100,	80, 	50, 	2, 		1, 		1, 		0, 		0,  	0, 		0, //blue  1
		   		100,	80, 	50, 	5, 		2, 		1, 		1, 		1,  	0, 		0, //gold  2
		   		100,	100, 	60, 	30,		10,		5, 		2, 		1,  	1, 		1  //holy  3
		   	};

const int Decompose::_items[] = {540, 548, 556, 562, 576, 577, 662, 663, 664,666, 667, 668, 669, /*670, 671, 672, 673, 674, 675, 676, */ /*removed (TASK 731)*/677, 678, 679, 681, 683, 655, 685, 686, 0};

/**     
 * \brief 取得分解的物品类型
 *
 * \return 分解的物品类型
 */
int Decompose::index() const
{
	if (_ob->data.kind & 4) { //holy
		 return 3;
	}
	if (_ob->data.kind & 2) { //gold
		 return 2;
	}
	if (_ob->data.kind & 1) { //blue
		 return 1;
	}

	return 0;
}

/**     
 * \brief 取得分解物品获得原料的概率
 *
 * \return 获得原料概率
 */
int Decompose::chance() const
{
	double coefficient = 0;
	
	if (_ob->data.kind & 4) { //holy
		coefficient = 10;
	}else if (_ob->data.kind & 2) { //gold
		 coefficient = 1;
	}else if (_ob->data.kind & 1) { //blue
		 coefficient = 0.5;
	}
	
	int level = _ob->data.needlevel/20 + 1;
	return static_cast<int>(coefficient*level);
}

/**     
 * \brief 分解物品需要的金钱数量
 *
 * \return 需要金钱数量
 */
int Decompose::gold() const
{
	double coefficient = 0;
	
	if (_ob->data.kind & 4) { //holy
		coefficient = 20;
	}else if (_ob->data.kind & 2) { //gold
		 coefficient = 10;
	}else if (_ob->data.kind & 1) { //blue
		 coefficient = 5;
	}
	
	int level = _ob->data.needlevel/20 + 1;
	return static_cast<int>(coefficient*level);
}

/**     
 * \brief 奖励经验给分解物品的用户
 *
 * \param user: 分解物品的用户
 * \return 当前总是返回true
 */
bool Decompose::bonus_exp(SceneUser& user)
{
	int coefficient = 0;
	
	if (_ob->data.kind & 4) { //holy
		coefficient = 200;
	}else if (_ob->data.kind & 2) { //gold
		 coefficient = 50;
	}else if (_ob->data.kind & 1) { //blue
		 coefficient = 20;
	}
	
//	int level = _ob->data.needlevel/20 + 1;
//	int exp = 2*coefficient*level;

//	if (_ob->data.exp) exp += _ob->data.exp;

//	//add exp bonus	
//	user.addExp(exp);

	//soke 分解物品经验+10
	user.addExp(10);

	
	return true;
}

/**     
 * \brief 奖励原料给分解物品的用户
 *
 * \param user: 分解物品的用户
 * \return 当前总是返回true
 */
bool Decompose::bonus_items(SceneUser& user)
{
	int kind = index();
	if (kind) {
		zObjectB::material::stuffs_iterator it = _ob->base->need_material.stuffs.begin();
		
		while (it != _ob->base->need_material.stuffs.end()) {
			int count =  zMisc::randBetween(1, it->number / 2);
			if(index() == 3 && _ob->data.maker[0] && _ob->data.bind)
			{
				count =  zMisc::randBetween(1, it->number);
				if (strstr(_ob->data.strName, "完美的") != NULL)
				{
					count = it->number/2;
				}
			}
			while (count -- > 0) {
				int level = 0;
				if(index() >= 2 && zMisc::selectByPercent(20))
				{
					level = zMisc::randBetween(1, index()-1);
				}
				if(index() == 3 && _ob->data.maker[0] && _ob->data.bind)
				{
					level = 2;
					if (strstr(_ob->data.strName, "完美的") != NULL)
					{
						level = 3;
					}
				}

				if ( user.addObjectNum(it->id, 1, level, Cmd::INFO_TYPE_GAME) == 1) {
					//package full
					return true;
				}			
			}
			++it;
		}
	}

	return true;	
}

/**     
 * \brief 删除被分解物品
 *
 * 从用户包裹中删除被分解的物品
 *
 * \param user: 分解物品的用户
 * \return 当前总是返回true
 */
bool Decompose::remove_from(SceneUser& user)
{
	return Base::remove_object(user, _ob);
}

/**     
 * \brief 赠加技能点
 *
 * 按物品需要等级增加用户打造技能的技能点
 *
 * \param user: 打造的用户
 * \param ob: 打造的物品
 * \return 当前总是返回true
 */
bool EquipMaker::add_skill(SceneUser& user, zObjectB* ob)
{
	return true;
}

/**     
 * \brief 赠加打造用户的经验值
 *
 * \param user: 打造的用户
 * \param coefficient: 奖励系数
 * \return 当前总是返回true
 */
bool EquipMaker::add_exp(SceneUser& user, DWORD coefficient)
{
    //soke 修改打造经验 防止卡经验
	//DWORD exp = (DWORD)_odds.material_level*coefficient*_need;
	//if (exp < 10) exp = 10;
	////add exp bonus	
	//user.addExp(exp);
	
	//soke 只给10点经验
	user.addExp(10);

	return true;
}

/**     
 * \brief 技能检查
 *
 * 检查用户的技能是否满足打造该物品要求
 *
 * \param user: 打造的用户
 * \param ob: 打造的物品
 * \return 满足要求返回true,否则返回false
 */
bool EquipMaker::check_skill(SceneUser& user, zObjectB* ob)
{
	int id = ob->need_skill.id;
	if (!id) return false;
	int level = ob->need_skill.level;
	
	_odds.skill_level = level>1?-1:1;

	return true;
}

/**     
 * \brief 材料检查
 *
 * 检查用户包裹中的材料是否满足打造该物品要求
 *
 * \param ob: 打造的物品
 * \param list:材料列表
 * \param is_resource: NOT USED YET
 * \return 满足要求返回true,否则返回false
 */
bool EquipMaker::check_material(zObjectB* ob, const std::map<DWORD, DWORD>& list, bool is_resource)
{
	/*
	for (zObjectB::material::stuffs_iterator it= ob->need_material.stuffs.begin(); it!=ob->need_material.stuffs.end(); ++it) 
	{
		Zebra::logger->debug("打造%s需要材料%d", ob->name,  it->id);
	}
	// */
	
	if (list.size() != ob->need_material.stuffs.size()) return false;
	
	zObjectB::material::stuffs_iterator it = ob->need_material.stuffs.begin();
	while (it != ob->need_material.stuffs.end()) {
		std::map<DWORD, DWORD>::const_iterator l_it = list.find(it->id);
		if (l_it == list.end() || l_it->second != it->number) {
			return false;
		}			

		++it;
	}
	
	return true;
}

#if 0
void  EquipMaker::pre_level_of_material(zObjectB* base)
{
	int max_level = 0, min_level = 0xffff;
	zObjectB::material::stuffs_iterator it = base->need_material.stuffs.begin();
	for ( ; it!=base->need_material.stuffs.end(); ++it) {
		zObjectB* ob = objectbm.get(it->id);
		if (!ob /*|| ob->level == 1*/) continue;
		if (ob->level > max_level) {
			max_level = ob->level;
			_max_level_id = it->id;
		}
		if (ob->level < min_level) {
			min_level = ob->level;
			_min_level_id = it->id;
		}
	}
}
#endif

void  EquipMaker::pre_level_of_material(int id, int level)
{
	if (!_1_id) {
		_1_id = id;
		_1_level = level;
	}

	if (_1_id == id && level > _1_level) _1_level = level;

	if (!_2_id && id != _1_id) {
		_2_id = id;
		_2_level = level;
	}
	if (_2_id == id && level > _2_level) _2_level = level;	
}

/**     
 * \brief 计算材料等级
 *
 * 计算打造材料的最终等级
 *
 * \param id: 材料id
 * \param num: 材料数量
 * \param level:材料等级
 * \param base: 材料信息
 * \return 成功返回true,否则返回false
 */
bool EquipMaker::level_of_material(DWORD id, DWORD num, DWORD level, zObjectB* base)
{
	zObjectB::material::stuffs_iterator it = base->need_material.stuffs.begin();
	while ( it!= base->need_material.stuffs.end() && (it->id != id) ) ++it;
	if (it ==  base->need_material.stuffs.end() ) return false;

	float coff = 1.0;
	zObjectB* ob = objectbm.get(it->id);
	//if (ob && ob->level == 1) coff = 0.01;
	//打造公式修改
	if (ob && ob->level == 1) coff = 0.1;
	
	if ( ((int)id == _1_id && (int)level == _1_level) || ((int)id == _2_id && (int)level == _2_level) ) {
		_current += static_cast<double>( coff*5.0f*num*(level-it->level+1) );
		_base += static_cast<double>(coff*5.0f*num);
	}
	else 
	{
		_current += static_cast<double>( coff*5.0f*num*(level-it->level+1));
		_base += static_cast<double>(coff*5.0f*num);
	}
	
	return true;
}

/**     
 * \brief 打造材料
 *
 * \param base: 打造物品
 * \return 打造的物品
 */
zObject* EquipMaker::make_material(zObjectB* base)
{	
	zObject *ob = zObject::create(base, 1);
	return ob;
}

/**     
 * \brief 构造函数
 *
 * 初始化相关变量
 *
 * param user : 请求打造的用户
 *
 */   
EquipMaker::EquipMaker(SceneUser* user) : _current(0), _base(0), _make(user?true:false), _need(0), _1_id(0), _1_level(0), _2_id(0), _2_level(0)
{
	_odds.per = user?user->charstate.charm:0;
	_odds.luck = user?user->charstate.lucky:0;
}

/**     
 * \brief 奖励Hole
 *
 * \param ob: 打造物品
 * \return 无
 */
void EquipMaker::bonus_hole(zObject* ob)
{

	int bonus_hole = static_cast<int>(_odds.material_level -6);
	if (bonus_hole>0) {
		Hole::add_hole_num(ob, bonus_hole);
		Zebra::logger->debug("材料等级(%f), 奖励孔(%d)", _odds.material_level, bonus_hole);
	}
}


/**     
 * \brief 打造装备
 *
 * \param user: 打造用户
 * \param base: 打造物品
 * \param flag: 是否强制生成
 * \return 打造的物品
 */
zObject* EquipMaker::make(SceneUser* user, zObjectB* base, int flag)
{	
	_odds.material_level = _base?(_current/_base):0;	
	if(user)
		Zebra::logger->debug("[打造]用户(%s)材料等级(%s:%.10f)", user?user->name:"", base->name, _odds.material_level);

	int white = odds_of_white(base);

	if(user)
		Zebra::logger->debug("[打造]用户(%s)生成白色装备概率(%s:%f\%)", user?user->name:"", base->name, white*1.0);
	
	if ( zMisc::selectByPercent(white) || flag) 
	{
		zObject *ob = zObject::create(base, 1);
		if (!ob) return NULL;
		_need = ob->base->needlevel;

		    assign(user, ob, base, false, flag);
		
		return ob;
	}	

	return NULL;
}

/**     
 * \brief 打造转生装备
 *
 * \param user: 打造用户
 * \param base: 打造物品
 * \param flag: 是否强制生成
 * \return 打造的物品
 */
zObject* EquipMaker::zsmake(SceneUser* user, zObjectB* base, int flag)
{	
	_odds.material_level = _base?(_current/_base):0;	
	if(user)
		Zebra::logger->debug("[转生打造]用户(%s)材料等级(%s:%.10f)", user?user->name:"", base->name, _odds.material_level);

	int white = odds_of_white(base);

	if(user)
		Zebra::logger->debug("[转生打造]用户(%s)生成白色装备概率(%s:%f\%)", user?user->name:"", base->name, white*1.0);
	
	if ( zMisc::selectByPercent(white) || flag) 
	{
		zObject *ob = zObject::create(base, 1);
		ob->data.round = 1;
		if (!ob) return NULL;
		_need = ob->base->needlevel;

		    assign_zs(user, ob, base, false, flag);
		
		return ob;
	}	

	return NULL;
}

//soke 神圣绿装打造专用、ObjectManager.cpp 任务给绿装、flag是gm获取装备的标记
void EquipMaker::assign(SceneUser* user, zObject* ob, zObjectB* base, bool drop, int flag)
{
	if (!ob) return;
	
	ob->foundSocket();	

	int gold = odds_of_gold(base);
	if(user)
		Zebra::logger->debug("[打造]用户(%s)生成金色装备概率(%s:%f\%)", user?user->name:"", base->name, gold*0.01);

	int set = 0;
	
	if (zMisc::selectByTenTh(gold) || (flag & 2)) 
	{
		int holy = odds_of_holy(ob->base->holyrating);
		bool g_holy = zMisc::selectByTenTh(holy) || (flag & 4) ;

		int index = 0;

		zGoldObjectB *bob = NULL;
		if (!g_holy) 
		{
			int index = zMisc::randBetween(0, base->golds.size()-1);	
			if(drop)
				bob =(zGoldObjectB *) dropgoldobjectbm.get(base->golds[index]);
			else
				bob = goldobjectbm.get(base->golds[index]);
		}
		else 
		{
			int index = zMisc::randBetween(0, base->holys.size()-1);	
			if(drop)
				bob =(zGoldObjectB *) dropgoldobjectbm.get(base->holys[index]);
			else
				bob = goldobjectbm.get(base->holys[index]);
		}


		if (!bob) 
		{
			if(user)
				Zebra::logger->debug("[打造]用户(%s)金色装备表中找不到对应数据%d(%d)", user?user->name:"", base->golds[index], index);
			return;
		}

		if (!flag && user) strncpy(ob->data.maker, user->name, MAX_NAMESIZE);//打造者名字
        //以下计算生成装备的品质
		int props = 0;
		if (user) 
		{
			// :原来生成绿色装备概率大于3000%时,对特殊字段属性取上限,现在概率调成900%
			if (holy > 3000*100) 
			{
				props = 18;
			} 
			else if (holy > 900*100) 
			{
				props = 17;
			}
			else if (g_holy) 
			{
				props = 3;
			}
			else 
			{
				props = 2;
			}
		}
		if(props>=5)
		{
			int i=0;
			int size=base->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, base->golds.size()-1);			
			}
			zGoldObjectB* bob1=NULL;
			if(drop)
				bob1 = (zGoldObjectB *)dropgoldobjectbm.get(base->holys[i]);
			else
				bob1 = goldobjectbm.get(base->holys[i]);
			if(bob1)
			{
				assign_color(bob1, ob, 2,props,base,drop/*, props*//*金色装也不作个数限制了,可怜的请玉兄弟*/);
			}
			else
			{
				assign_color(bob, ob, 2,props,base,drop/*, props*//*金色装也不作个数限制了,可怜的请玉兄弟*/);
			}
		}
		else
		{
			assign_color(bob, ob, 2,props,base,drop/*, props*//*金色装也不作个数限制了,可怜的请玉兄弟*/);
		}

		if (bob->golds.size() > 0) 
		{
			index = zMisc::randBetween(0, bob->golds.size()-1);
			zGoldObjectB* n_bob=NULL;
			if(drop)
				n_bob = (zGoldObjectB *)dropgoldobjectbm.get(bob->golds[index]);
			else
				n_bob = goldobjectbm.get(bob->golds[index]);
			if (n_bob) 
			{
				assign_color(n_bob, ob, 2);
			}
		}
		if(user)
			Zebra::logger->debug("[打造]用户(%s)神圣物品产生几率%f\%", user?user->name:"", holy*0.01);

		if(g_holy) 
		{
			assign_holy(ob, holy);// 神圣装备
		}

		set = bob->five.sleightValue;

		}
		else 
		{
			int blue = odds_of_blue(base);
			if(user)
				Zebra::logger->debug("[打造]用户(%s)生成蓝色装备概率(%s:%f\%)", user?user->name:"", base->name, blue*0.01);

			if (zMisc::selectByTenTh(blue) || (flag & 1) ) 
			{
				int index = zMisc::randBetween(0, base->blues.size()-1);			
				zBlueObjectB *bob = blueobjectbm.get(base->blues[index]);
				if (!bob) return;
				if (!flag && user) strncpy(ob->data.maker, user->name, MAX_NAMESIZE);//打造者名字
				assign_color(bob, ob, 1);

				set = bob->five.sleightValue;
			}			
		}

		if(user)
			Zebra::logger->debug("[打造]用户(%s)生成五行套装概率(%s:%f\%)", user?user->name:"", base->name, set*0.01);	
		//soke 五行套装就是灵魂锁链。。。。。。如果所紫装 就生成两个灵魂	
		if ( (ob->data.fivetype != FIVE_NONE && zMisc::selectByTenTh(set)) || (flag & 8) || (ob->data.kind & 4) ) 
		{
			ob->data.fivetype = zMisc::randBetween(0, 4);
			assign_set(ob);	
		}

		bonus_hole(ob);
		fix(ob);

	}

	//soke 转生打造
void EquipMaker::assign_zs(SceneUser* user, zObject* ob, zObjectB* base, bool drop, int flag)
{
	if (!ob) return;

	ob->foundSocket();	

	int gold = odds_of_gold(base);
	if(user)
		Zebra::logger->debug("[转生打造]用户(%s)生成金色装备概率(%s:%f\%)", user?user->name:"", base->name, gold*0.01);

	int set = 0;

	if (zMisc::selectByTenTh(gold) || (flag & 2)) 
	{
		int holy = odds_of_holy(ob->base->holyrating);
		bool g_holy = zMisc::selectByTenTh(holy) || (flag & 4) ;

		int index = 0;

		zZhuanGoldObjectB *bob = NULL;
		if (!g_holy) 
		{
			int index = zMisc::randBetween(0, base->golds.size()-1);	
			if(drop)
				bob =(zZhuanGoldObjectB *) dropgoldobjectbm.get(base->golds[index]);
			else
				bob = zhuangoldobjectbm.get(base->golds[index]);
		}
		else 
		{
			int index = zMisc::randBetween(0, base->holys.size()-1);	
			if(drop)
				bob = (zZhuanGoldObjectB *)dropgoldobjectbm.get(base->holys[index]);
			else
				bob = zhuangoldobjectbm.get(base->holys[index]);
		}


		if (!bob) 
		{
			if(user)
				Zebra::logger->debug("[转生打造]用户(%s)金色装备表中找不到对应数据%d(%d)", user?user->name:"", base->golds[index], index);
			return;
		}

		if (!flag && user) strncpy(ob->data.maker, user->name, MAX_NAMESIZE);//打造者名字
		//以下计算生成装备的品质
		int props = 0;
		if (user) 
		{
			// :原来生成绿色装备概率大于3000%时,对特殊字段属性取上限,现在概率调成900%
			if (holy > 3000*100) 
			{
				props = 18;
			} 
			else if (holy > 900*100) 
			{
				props = 17;
			}
			else if (g_holy) 
			{
				props = 3;
			}
			else 
			{
				props = 2;
			}
		}
		if(props>=5)
		{
			int i=0;
			int size=base->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, base->golds.size()-1);			
			}
			zZhuanGoldObjectB *bob1=NULL;
			if(drop)
				bob1 = (zZhuanGoldObjectB *)dropgoldobjectbm.get(base->holys[i]);
			else
				bob1 = zhuangoldobjectbm.get(base->holys[i]);
			if(bob1)
			{
				assign_color(bob1, ob, 2,props,base,drop/*, props*//*金色装也不作个数限制了,可怜的请玉兄弟*/);
			}
			else
			{
				assign_color(bob, ob, 2,props,base,drop/*, props*//*金色装也不作个数限制了,可怜的请玉兄弟*/);
			}
		}
		else
		{
			assign_color(bob, ob, 2,props,base,drop/*, props*//*金色装也不作个数限制了,可怜的请玉兄弟*/);
		}

		if (bob->golds.size() > 0) 
		{
			index = zMisc::randBetween(0, bob->golds.size()-1);
			zZhuanGoldObjectB *n_bob=NULL;
			if(drop)
				n_bob = (zZhuanGoldObjectB *)dropgoldobjectbm.get(bob->golds[index]);
			else
				n_bob = zhuangoldobjectbm.get(bob->golds[index]);
			if (n_bob) 
			{
				assign_color(n_bob, ob, 2);
			}
		}
		if(user)
			Zebra::logger->debug("[转生打造]用户(%s)神圣物品产生几率%f\%", user?user->name:"", holy*0.01);

		if(g_holy) 
		{
			assign_holy(ob, holy);// 神圣装备
		}

		set = bob->five.sleightValue;

	}
	else 
	{
		int blue = odds_of_blue(base);
		if(user)
			Zebra::logger->debug("[转生打造]用户(%s)生成蓝色装备概率(%s:%f\%)", user?user->name:"", base->name, blue*0.01);

		if (zMisc::selectByTenTh(blue) || (flag & 1) ) 
		{
			int index = zMisc::randBetween(0, base->blues.size()-1);			
			zBlueObjectB *bob = blueobjectbm.get(base->blues[index]);
			if (!bob) return;
			if (!flag && user) strncpy(ob->data.maker, user->name, MAX_NAMESIZE);//打造者名字
			assign_color(bob, ob, 1);

			set = bob->five.sleightValue;
		}			
	}

	if(user)
		Zebra::logger->debug("[转生打造]用户(%s)生成五行套装概率(%s:%f\%)", user?user->name:"", base->name, set*0.01);	
	//soke 五行套装就是灵魂锁链。。。。。。如果所紫装 就生成两个灵魂	
	if ( (ob->data.fivetype != FIVE_NONE && zMisc::selectByTenTh(set)) || (flag & 8) || (ob->data.kind & 4) ) 
	{
		ob->data.fivetype = zMisc::randBetween(0, 4);
		assign_set(ob);	
	}

	bonus_hole(ob);
	fix(ob);

}

//soke 神圣祝福紫装打造专用、ObjectManager.cpp 任务给紫装 、flag是gm获取装备的标记
void EquipMaker::assign1(SceneUser* user, zObject* ob, zObjectB* base, bool drop, int flag)
{
	if (!ob) return;
	
	ob->foundSocket();	

	int gold = odds_of_gold(base);
	if(user)
		Zebra::logger->debug("[打造]用户(%s)生成金色装备概率(%s:%f\%)", user?user->name:"", base->name, gold*0.01);

	int set = 0;
	
	if (zMisc::selectByTenTh(gold) || (flag & 2)) 
	{
		int purple = odds_of_purple(ob->base->holyrating/10);
		bool g_purple = zMisc::selectByLakh(purple) || (flag & 16) ;

		int index = 0;

		zZhuanGoldObjectB *bob = NULL;
		if (g_purple) 	 
		{
			int index = zMisc::randBetween(0, base->holys.size()-1);	
			if(drop)
			    bob = (zZhuanGoldObjectB *)zhuangoldobjectbm.get(base->holys[index]);
			else
				bob = zhuangoldobjectbm.get(base->holys[index]);
		}


		if (!bob) 
		{
			if(user)
				Zebra::logger->debug("[打造]用户(%s)神圣祝福装备表中找不到对应数据%d(%d)", user?user->name:"", base->holys[index], index);
			return;
		}

		if (!flag && user) strncpy(ob->data.maker, user->name, MAX_NAMESIZE);//打造者名字
        //以下计算生成装备的品质
		int props = 0;
		if (user) 
		{
			if (purple > 3000*100) 
			{
				props = 18;
			} 
			else if (purple > 900*100) 
			{
				props = 17;
			}
			else if (g_purple) 
			{
				props = 3;
			}
			else 
			{
				props = 2;
			}
		}
		if(props>=5)
		{
			int i=0;
			int size=base->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, base->golds.size()-1);			
			}

			zZhuanGoldObjectB* bob1=NULL;
			if(drop)
				bob1 = (zZhuanGoldObjectB *)dropgoldobjectbm.get(base->holys[i]);
			else
				bob1 = zhuangoldobjectbm.get(base->holys[i]);
			if(bob1)
			{
				assign_color(bob1, ob, 2,props,base,drop/*, props*//*金色装也不作个数限制了,可怜的请玉兄弟*/);
			}
			else
			{
				assign_color(bob, ob, 2,props,base,drop/*, props*//*金色装也不作个数限制了,可怜的请玉兄弟*/);
			}
		}
		else
		{
			assign_color(bob, ob, 2,props,base,drop/*, props*//*金色装也不作个数限制了,可怜的请玉兄弟*/);
		}

		if(user)
			Zebra::logger->debug("[打造]用户(%s)神圣祝福物品产生几率%f\%", user?user->name:"", purple*0.01);

		if(g_purple) 
		{
            assign_purple(ob, purple);// 神圣装备
		}

		set = bob->five.sleightValue;
    }

		if(user)
			Zebra::logger->debug("[打造]用户(%s)生成五行套装概率(%s:%f\%)", user?user->name:"", base->name, set*0.01);	
		//soke 五行套装就是灵魂锁链。。。。。。如果所紫装 就生成两个灵魂	
		if ( (ob->data.fivetype != FIVE_NONE && zMisc::selectByTenTh(set)) || (flag & 8) || (ob->data.kind & 16) ) 
		{
			ob->data.fivetype = zMisc::randBetween(0, 4);
			assign_set(ob);	
		}

		bonus_hole(ob);
		fix(ob);
	}


//soke Scene.cpp中BOSS掉落装备，最高可掉落紫装
void EquipMaker::assign2(SceneUser* user, zObject* ob, zObjectB* base, bool drop, int flag)
{
	if (!ob) return;
	
	ob->foundSocket();	

	int gold = odds_of_gold(base)/10;

	int set = 0;
	
	if (zMisc::selectByTenTh(gold/8) || (flag & 2)) 
	{
		int holy = odds_of_holy(ob->base->holyrating/10); //soke 修改绿装掉落几率
		bool g_holy = zMisc::selectByTenTh(holy) || (flag & 4) ;
		int purple = odds_of_purple(ob->base->holyrating/12); //soke 修改紫装掉落几率
		bool g_purple = zMisc::selectByLakh(purple) || (flag & 16) ;

		int index = 0;

		zGoldObjectB *bob = NULL;
		if (!g_holy || !g_purple) 
		{
			int index = zMisc::randBetween(0, base->golds.size()-1);	
			if(drop)
				bob =(zGoldObjectB *) dropgoldobjectbm.get(base->golds[index]);
			else
				bob = goldobjectbm.get(base->golds[index]);
		}
		else
		{
			int index = zMisc::randBetween(0, base->holys.size()-1);	
			if(drop)
				bob = (zGoldObjectB *)dropgoldobjectbm.get(base->holys[index]);
			else
				bob = goldobjectbm.get(base->holys[index]);
		}

		if (!bob) 
		{
			return;
		}

		if (!flag && user) strncpy(ob->data.maker, user->name, MAX_NAMESIZE);//打造者名字
        //以下计算生成装备的品质
		int props = 0;
		if (user) 
		{
			// :原来生成绿色装备概率大于3000%时,对特殊字段属性取上限,现在概率调成900%
			if (holy > 3000*100) 
			{
				props = 18;
			} 
			else if (holy > 900*100) 
			{
				props = 17;
			}
			else if (g_holy) 
			{
				props = 3;
			}
			else 
			{
				props = 2;
			}
		}
		//如果是紫装 则100%完美 props就是用来计算品质的。
		if (g_purple)
		{
			props = 18;
		}
		if(props>=5)
		{
			int i=0;
			int size=base->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, base->golds.size()-1);			
			}
			zGoldObjectB* bob1=NULL;
			if(drop)
				bob1 = (zGoldObjectB *)dropgoldobjectbm.get(base->holys[i]);
			else
				bob1 = goldobjectbm.get(base->holys[i]);

			if(bob1)
			{
				assign_color(bob1, ob, 2,props,base,drop/*, props*//*金色装也不作个数限制了,可怜的请玉兄弟*/);
			}
			else
			{
				assign_color(bob, ob, 2,props,base,drop/*, props*//*金色装也不作个数限制了,可怜的请玉兄弟*/);
			}
		}
		else
		{
			assign_color(bob, ob, 2,props,base,drop/*, props*//*金色装也不作个数限制了,可怜的请玉兄弟*/);
		}

		if (bob->golds.size() > 0) 
		{
			index = zMisc::randBetween(0, bob->golds.size()-1);
			zGoldObjectB* n_bob=NULL;
			if(drop)
				n_bob = (zGoldObjectB *)dropgoldobjectbm.get(bob->golds[index]);
			else
				n_bob = goldobjectbm.get(bob->golds[index]);
			if (n_bob) 
			{
				assign_color(n_bob, ob, 2);
			}
		}

		if(g_holy) 
		{
                   assign_holy(ob, holy);// 神圣装备
		}
		//soke  紫装
		if(g_purple) 
		{
		   assign_purple(ob, purple);// 紫色装备
		}
		set = bob->five.sleightValue;

		}
		else 
		{
			int blue = odds_of_blue(base);

			if (zMisc::selectByTenTh(blue) || (flag & 1) ) 
			{
				int index = zMisc::randBetween(0, base->blues.size()-1);			
				zBlueObjectB *bob = blueobjectbm.get(base->blues[index]);
				if (!bob) return;
				if (!flag && user) strncpy(ob->data.maker, user->name, MAX_NAMESIZE);//打造者名字
				assign_color(bob, ob, 1);

				set = bob->five.sleightValue;
			}			
		}

		//soke 五行套装就是灵魂锁链。。。。。。如果所紫装 就生成两个灵魂	
		if ( (ob->data.fivetype != FIVE_NONE && zMisc::selectByTenTh(set)) || (flag & 8) || (ob->data.kind & 4)|| (ob->data.kind & 16) ) 
		{
			assign_set(ob);	
		}

		bonus_hole(ob);
		fix(ob);
	}


//soke 绿装 装备解绑
void EquipMaker::assign3(SceneUser* user, zObject* ob, zObjectB* base, bool drop, int flag)
{
	if (!ob) return;
	
	ob->foundSocket();	

	ob->data.needlevel = base->needlevel;			// 需要等级
	ob->data.maxhp = base->maxhp;					// 最大生命值
	ob->data.maxmp = base->maxmp;					// 最大法术值
	ob->data.maxsp = base->maxsp;					// 最大体力值
	ob->data.pdamage = base->pdamage;				// 最小攻击力
	ob->data.maxpdamage = base->maxpdamage;			// 最大攻击力
	ob->data.mdamage = base->mdamage;				// 最小法术攻击力
	ob->data.maxmdamage = base->maxmdamage;			// 最大法术攻击力
	ob->data.pdefence = base->pdefence;				// 物防
	ob->data.mdefence = base->mdefence;				// 魔防
	ob->data.damagebonus = base->damagebonus;			// 伤害加成
	ob->data.akspeed = base->akspeed;				// 攻击速度
	ob->data.mvspeed = base->mvspeed;				// 移动速度
	ob->data.atrating = base->atrating;				// 命中率
	ob->data.akdodge = base->akdodge;				// 躲避率
	ob->data.bang = base->bang;
	ob->data.dur = base->durability;
	ob->data.maxdur = base->durability;
	ob->data.price = base->price;
	ob->data.cardpoint = base->cardpoint;

	zGoldObjectB *bob = NULL;

	int gold = odds_of_gold(base);
	
	if (zMisc::selectByTenTh(gold) || (flag & 2)) 
	{
		int holy = odds_of_holy(ob->base->holyrating); //soke 修改绿装掉落几率
		bool g_holy = zMisc::selectByTenTh(holy) || (flag & 4) ;

		if (!g_holy) 
		{
			int index = zMisc::randBetween(0, base->golds.size()-1);	

			bob = goldobjectbm.get(base->golds[index]);
		 }
		 else
		 {
			int index = zMisc::randBetween(0, base->holys.size()-1);	

			bob = goldobjectbm.get(base->holys[index]);
		 }

        //以下计算生成装备的品质
		int props = 0;

			// :原来生成绿色装备概率大于3000%时,对特殊字段属性取上限,现在概率调成900%
			if (g_holy) 
			{
				props = 18;
			}
			else 
			{
				props = 2;
			}

		if (strstr(ob->data.strName,"完美的"))
		{
			props = 18;
		}
		if(props>=5)
		{
			int i=0;
			int size=base->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, base->golds.size()-1);			
			}
			zGoldObjectB* bob1=NULL;

			bob1 = goldobjectbm.get(base->holys[i]);

			if(bob1)
			{
				assign_bind(bob1, ob, 2,props,base);
			}
			else
			{
				assign_bind(bob, ob, 2,props,base);
			}
		}
		else
		{
			assign_bind(bob, ob, 2,props,base);
		}

		if(g_holy) 
		{
            assign_holy(ob, holy);// 神圣装备
		}

		//soke 五行套装就是灵魂锁链
		if ( (ob->data.fivetype != FIVE_NONE )|| ((ob->data.kind) & 8) || ((ob->data.kind) & 4))
		{
			assign_set(ob);	
		}

		bonus_hole(ob);
		fix(ob);
	}
}

//soke 紫装 装备解绑
void EquipMaker::assign4(SceneUser* user, zObject* ob, zObjectB* base, bool drop, int flag)
{
	if (!ob) return;
	
	ob->foundSocket();	

	ob->data.needlevel = base->needlevel;			// 需要等级
	ob->data.maxhp = base->maxhp;					// 最大生命值
	ob->data.maxmp = base->maxmp;					// 最大法术值
	ob->data.maxsp = base->maxsp;					// 最大体力值
	ob->data.pdamage = base->pdamage;				// 最小攻击力
	ob->data.maxpdamage = base->maxpdamage;			// 最大攻击力
	ob->data.mdamage = base->mdamage;				// 最小法术攻击力
	ob->data.maxmdamage = base->maxmdamage;			// 最大法术攻击力
	ob->data.pdefence = base->pdefence;				// 物防
	ob->data.mdefence = base->mdefence;				// 魔防
	ob->data.damagebonus = base->damagebonus;			// 伤害加成
	ob->data.akspeed = base->akspeed;				// 攻击速度
	ob->data.mvspeed = base->mvspeed;				// 移动速度
	ob->data.atrating = base->atrating;				// 命中率
	ob->data.akdodge = base->akdodge;				// 躲避率
	ob->data.bang = base->bang;
	ob->data.dur = base->durability;
	ob->data.maxdur = base->durability;
	ob->data.price = base->price;
	ob->data.cardpoint = base->cardpoint;

	zZhuanGoldObjectB *bob = NULL;

	int gold = odds_of_gold(base);
	
	if (zMisc::selectByTenTh(gold) || (flag & 2)) 
	{
		int purple = odds_of_purple(ob->base->holyrating/10); //soke 修改紫装掉落几率
		bool g_purple = zMisc::selectByLakh(purple) || (flag & 16) ;

		if (!g_purple) 
		{
			int index = zMisc::randBetween(0, base->golds.size()-1);	

			bob = zhuangoldobjectbm.get(base->golds[index]);
		 }
		 else
		 {
			int index = zMisc::randBetween(0, base->holys.size()-1);	

			bob = zhuangoldobjectbm.get(base->holys[index]);
		 }

        //以下计算生成装备的品质
		int props = 0;

		//如果是紫装 则100%完美 props就是用来计算品质的。
		if (g_purple)
		{
			props = 18;
		}
		else 
		{
			props = 2;
		}

		if (strstr(ob->data.strName,"完美的"))
		{
			props = 18;
		}
		if(props>=5)
		{
			int i=0;
			int size=base->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, base->golds.size()-1);			
			}

			zZhuanGoldObjectB* bob1=NULL;

			bob1 = zhuangoldobjectbm.get(base->holys[i]);

			if(bob1)
			{
				assign_bind2(bob1, ob, 2,props,base);
			}
			else
			{
				assign_bind2(bob, ob, 2,props,base);
			}
		}
		else
		{
			assign_bind2(bob, ob, 2,props,base);
		}

		//soke  紫装
		if(g_purple) 
		{
			assign_purple(ob, purple);// 紫色装备
		}

		//soke 五行套装就是灵魂锁链。。。。。。如果所紫装 就生成两个灵魂	
		if ( (ob->data.fivetype != FIVE_NONE )|| ((ob->data.kind) & 8)  || ((ob->data.kind) & 4) ||((ob->data.kind) & 16) )
		{
			assign_set(ob);	
		}

		bonus_hole(ob);
		fix(ob);
	}
}

void EquipMaker::fix(zObject* ob)
{
	if (ob->data.pdamage > ob->data.maxpdamage) 
	{
		std::swap(ob->data.pdamage, ob->data.maxpdamage);
	}

	if (ob->data.mdamage > ob->data.maxmdamage) 
	{
		std::swap(ob->data.mdamage, ob->data.maxmdamage);
	}


}

#undef COMPUTE
#define COMPUTE(x) ob->data.x = static_cast<DWORD>(ob->data.x*1.4);
/** 
 * \brief 产生神圣装备
 *
 * \param base: 打造物品
 * \return 成功返回true,否则返回false
 */
bool EquipMaker::assign_holy(zObject* ob, int holy)
{
//	zHolyObjectB *bob = holyobjectbm.get(ob->data.dwObjectID);
	//int index = zMisc::randBetween(0, ob->base->holys.size()-1);			
	zHolyObjectB *bob = holyobjectbm.get(1001/*ob->base->holys[index]*/);
	if (bob==NULL) return false;
	
	ob->data.kind |= 16;//神圣装备,参看Object.h注释
	ob->data.holy = bob->holy;
	
	int property = 1;

	COMPUTE( maxhp )					// 最大生命值
	COMPUTE( maxmp )					// 最大法术值
	COMPUTE( maxsp )					// 最大体力值

	COMPUTE( pdamage )				// 最小攻击力
	COMPUTE( maxpdamage )			// 最大攻击力
	COMPUTE( mdamage )				// 最小法术攻击力
	COMPUTE( maxmdamage )			// 最大法术攻击力

	COMPUTE( pdefence )				// 物防
	COMPUTE( mdefence )				// 魔防

	COMPUTE_L( damage )		// 增加伤害值x％
	COMPUTE_L(	fivepoint )		// 五行属性增加

	COMPUTE_L( hpr )		// 生命值恢复
	COMPUTE_L( mpr )		// 法术值恢复
	COMPUTE_L( spr )		// 体力值恢复

	COMPUTE_L( akspeed )	// 攻击速度
	COMPUTE_L( mvspeed )	// 移动速度
	
	COMPUTE_L( atrating )	// 命中率
	COMPUTE_L( akdodge )		// 闪避率

	COMPUTE_L( doublexp )	// %x双倍经验
	COMPUTE_L( mf )		//掉宝率
	
	BONUS_SKILL	                    // 技能加成    
	BONUS_SKILLS
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x = static_cast<DWORD>(ob->data.x*2.4);
/**     
 * \brief 产生神圣紫色装备
 *
 * \param base: 打造物品
 * \return 成功返回true,否则返回false
 */
 
bool EquipMaker::assign_purple(zObject* ob, int holy)
{
//	zHolyObjectB *bob = holyobjectbm.get(ob->data.dwObjectID);
	//int index = zMisc::randBetween(0, ob->base->holys.size()-1);			
	zHolyObjectB *bob = holyobjectbm.get(1001/*ob->base->holys[index]*/);
	if (bob==NULL) return false;
	
	ob->data.kind |= 16;//神圣装备,参看Object.h注释
	ob->data.holy = bob->holy;
	
	int property = 1;

	COMPUTE( maxhp )					// 最大生命值
		COMPUTE( maxmp )					// 最大法术值
		COMPUTE( maxsp )					// 最大体力值

		COMPUTE( pdamage )				// 最小攻击力
		COMPUTE( maxpdamage )			// 最大攻击力
		COMPUTE( mdamage )				// 最小法术攻击力
		COMPUTE( maxmdamage )			// 最大法术攻击力

		COMPUTE( pdefence )				// 物防
		COMPUTE( mdefence )				// 魔防

		COMPUTE_L( damage )		// 增加伤害值x％
		COMPUTE_L(	fivepoint )		// 五行属性增加

		COMPUTE_L( hpr )		// 生命值恢复
		COMPUTE_L( mpr )		// 法术值恢复
		COMPUTE_L( spr )		// 体力值恢复

		COMPUTE_L( akspeed )	// 攻击速度
		COMPUTE_L( mvspeed )	// 移动速度

		COMPUTE_L( atrating )	// 命中率
		COMPUTE_L( akdodge )		// 闪避率

	COMPUTE_L( doublexp )	        // %x双倍经验
	COMPUTE_L( mf )		            // 掉宝率
	
	BONUS_SKILL	                    // 技能加成    
	BONUS_SKILLS
	
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x = zMisc::randBetween(prop->x.min, prop->x.max); \
	set_prop[index] = 1; \
	--i; \
	break;
	
/**     
 * \brief 产生五行套装
 *
 * \param base: 打造物品
 * \return 成功返回true,否则返回false
 */
bool EquipMaker::assign_set(zObject* ob)
{
	if (!ob)
	{
		return false;
	}

	//soke 盾牌、护心镜、短枪、战旗等不给灵魂
/*	if(ob->base->kind == ItemType_Shield || ob->base->kind == ItemType_breastplate || ob->base->kind == ItemType_Handgun 
		|| (ob->base->kind >= ItemType_StaffFlag && ob->base->kind <= ItemType_SwordFlag))
	{
		return false;
	}*/

	zSetObjectB *base = setobjectbm.get(ob->data.dwObjectID);
	if (base==NULL) return false;

	zFiveSetB* prop = fivesetbm.get(base->mark);
	if (!prop) return false;
		
	ob->data.kind |= 8;//五行套装,参看Object.h注释

	//soke
	//for (int i = 0;i < 5;i++)
	//{
	//	ob->data._five_props[i]=0;//灵魂锁链数据清零
	//}
	ob->data.dpdam = 0;
	ob->data.dmdam = 0;
	ob->data.bdam = 0;
	ob->data.rdam = 0;
	ob->data.ignoredef = 0;
	ob->data.addpdef = 0;
	ob->data.addmdef = 0;
	ob->data.addpdam = 0;
	ob->data.addmdam = 0;
	ob->data.addhp = 0;

	int i = 0;
	int tmpodds = 0;
	for (zSetObjectB::iterator it=base->sets.begin(); it!=base->sets.end(); ++it) 
	{
		//soke 灵魂锁链
		tmpodds = it->odds;
		if (tmpodds < 500 )
		{
		//	Zebra::logger->debug("it->odds == %d",tmpodds);
			tmpodds = 0;
			continue;
		}
		//Zebra::logger->debug("it->odds == %d",tmpodds);
		//Zebra::logger->debug("(ob->data.kind)&16==%d",(ob->data.kind)&16);
		if (tmpodds==500)
		{
			tmpodds = 0;
			if((ob->data.kind)&16)
			{
				//Zebra::logger->debug("two soul");
				tmpodds = 10002;//紫装双灵魂
			}
		}
		//Zebra::logger->debug("it->odds == %d",tmpodds);
		if (zMisc::selectByTenTh(tmpodds)) 
		{
			for (std::vector<WORD>::iterator s_it=(*it).ids.begin(); s_it!=(*it).ids.end(); ++s_it) 
			{
				ob->data.fiveset[i++] = (*it).ids[0];
			}
			int set_prop[] = { 0, 0, 0, 0, 0};	
			int nSoulCount = 0;
			do 
			{
					int index = zMisc::randBetween(0,4);
				//	if (set_prop[index]) continue;
					if (set_prop[index-1]) continue;
					if( nSoulCount == 0)
					{
					//	ob->data.opt1=index;
						switch (index)
						{
						case 0:
							{
								ob->data.dpdam = zMisc::randBetween(prop->dpdam.min, prop->dpdam.max);
								i--;
							}
							break;
							//COMPUTE( dpdam ) //物理伤害减少%x
						case 1:
							{									
								ob->data.dmdam = zMisc::randBetween(prop->dmdam.min, prop->dmdam.max);
								i--;
							}
							break;
						//	COMPUTE( dmdam ) //法术伤害减少%x
						case 2:
							{									
								ob->data.bdam = zMisc::randBetween(prop->bdam.min, prop->bdam.max);
								i--;
							}
							break;
						//	COMPUTE( bdam ) //增加伤害x%
						case 3:
							{								
								ob->data.rdam = zMisc::randBetween(prop->rdam.min, prop->rdam.max);
								i--;
							}
							break;
						//	COMPUTE( rdam ) //伤害反射%x
						case 4:
							{								
								ob->data.ignoredef = zMisc::randBetween(prop->ignoredef.min, prop->ignoredef.max);
								i--;
							}
							break;
						//	COMPUTE( ignoredef ) //%x忽视目标防御

						default:
							break;		
						}

						if (!((ob->data.kind)&16)&&((ob->data.kind)&4))
						{
							break;
						}
					}
					else if ( nSoulCount == 1)
					{
					//	ob->data.opt3=index;
						switch (index)
						{
						case 0:
							{
								ob->data.addpdef = zMisc::randBetween(prop->addpdef.min, prop->addpdef.max);
								--i;
							}
							break;	
						case 1:
							{
								ob->data.addmdef = zMisc::randBetween(prop->addmdef.min, prop->addmdef.max);
								--i;
							}
							break;	
						case 2:
							{
								ob->data.addpdam = zMisc::randBetween(prop->addpdam.min, prop->addpdam.max);
								--i;
							}
							break;	
						case 3:
							{
								ob->data.addmdam = zMisc::randBetween(prop->addmdam.min, prop->addmdam.max);
								--i;
							}
							break;		
						case 4:
							{
								ob->data.addhp = zMisc::randBetween(prop->addhp.min, prop->addhp.max);
								--i;
							}
							break;
							
						default:
							break;		
						}
					}
					nSoulCount++;
				
			}while (i>0);
			
			break;	
		}	
	}
	
	//fprintf(stderr,"生成灵魂成功 %s dwId = %u \n",ob->name,ob->data.dwObjectID);
	return true;
}

/**     
 * \brief 计算打造是放入的几率宝石是否合法及对应的额外成功几率
 *
 * \param kind: 打造装备类型
 * \param id: 放入的几率宝石
 * \return 成功返回true,否则返回false
 */
bool EquipMaker::is_odds_gem(DWORD kind, DWORD id)
{

    //soke 衣服
	if (id == 730 && kind >= ItemType_ClothBody && kind <= ItemType_MetalBody ) 
	{
		_odds.odds_gem = 20;
		return true;
	}
	
	//soke 武器
	if (id == 731 && ( (kind >= ItemType_Blade && kind <= ItemType_Stick) || kind == ItemType_Dagger || kind == ItemType_NewSword || kind == ItemType_LongGun) )
	{
		_odds.odds_gem = 20;
		return true;
	}
	
	//soke 防具
	if (id == 732 && (kind == ItemType_Helm || kind == ItemType_Shield || kind == ItemType_Shoes || kind == ItemType_breastplate || kind == ItemType_Handgun || (kind >= ItemType_StaffFlag && kind <= ItemType_SwordFlag) || kind == ItemType_HorseRope || kind == ItemType_HorseSaddle) ) 
	{
		_odds.odds_gem = 20;
		return true;
	}
	
	//soke 首饰
	if (id == 733 && (kind == ItemType_Caestus || kind == ItemType_Caestus || kind == ItemType_Necklace || kind == ItemType_Fing || kind == ItemType_HorseShoe || kind == ItemType_HorseSafe || kind == ItemType_HorseIron) ) 
	{
		_odds.odds_gem = 20;
		return true;
	}
	
	return false;
}

/**     
 * \brief 生成白色装备概率
 *
 * \param ob: 打造物品
 * \return 生成概率
 */
int EquipMaker::odds_of_white(const zObjectB* ob)
{
//	return static_cast<int>((20*_odds.per/(_odds.per+4) + 20*_odds.luck/(_odds.luck+10) + 70*_odds.material_level + 10*_odds.skill_level));		
	return static_cast<int>((20*_odds.per/(_odds.per+4) + 20*_odds.luck/(_odds.luck+10) + 100*_odds.material_level + 10*_odds.skill_level));		

}

/**     
 * \brief 生成蓝色装备概率
 *
 * \param ob: 打造物品
 * \return 生成概率
 */
int EquipMaker::odds_of_blue(const zObjectB* ob)
{
	int odds = ob->bluerating;
	//if (_make) odds = static_cast<int>( pow(_odds.material_level, 5) + 0.1*ob->bluerating );
	if (_make) {
		odds = static_cast<int>( 100*pow(_odds.material_level+3, 3) /*+ 0.1*ob->bluerating*/ );
		//有个性的策划公式。。。
		//if (odds >= 9800) odds = 10000;
		odds += 200;
	}

	return odds;
}

/**     
 * \brief 生成金色装备概率
 *
 * \param ob: 打造物品
 * \return 生成概率
 */
int EquipMaker::odds_of_gold(const zObjectB* ob)
{
	int odds =  ob->goldrating;
	if (_make) {
		odds = static_cast<int>((4*pow(_odds.material_level, 7) /* + 0.1*ob->goldrating*/ ) );
		//有个性的策划公式。。。
		//if (odds >= 9800) odds = 10000;
		odds += 200;
	}

	return odds;
}

/**     
 * \brief 生成神圣装备概率
 *
 * \param ob: 打造物品
 * \return 生成概率
 */
int EquipMaker::odds_of_holy(int object)
{
	int odds = object;
	if (_make) 
	{
		odds = static_cast<int>( 0.01*pow(_odds.material_level, 10) /* + object + 120 */ );
		//有个性的策划公式。。。
		//if (odds >= 9800) odds = 10000;
		odds += 200;
	}	

	return odds;
}

/**     
 * \brief 生成神圣祝福紫色装备概率
 *
 * \param ob: 打造物品
 * \return 生成概率
 */
int EquipMaker::odds_of_purple(int object)
{
	int odds = object;
	if (_make) 
	{
		odds = static_cast<int>( 0.01*pow(_odds.material_level, 10) /* + object + 120 */ );
		//有个性的策划公式。。。
		//if (odds >= 9800) odds = 10000;
		odds += 200;
	}	

	return odds;
}

/**     
 * \brief 装备生成属性概率
 *
 * \param ob: 打造物品
 * \return 生成概率
 */
int EquipMaker::odds_of_property(int object, int property)
{
	return static_cast<int>(( (  3*_odds.per/(_odds.per+4) + 3*_odds.luck/(_odds.luck+10) + _odds.odds_gem + 5*(_odds.material_level) )/property) + object);
}


RebuildObject* RebuildObject::_instance = NULL;

/**     
 * \brief  单件模式,保证物品改造类的唯一实例
 *
 * \return 物品改造类的唯一实例
 */	
RebuildObject& RebuildObject::instance()
{
	static RebuildObject new_instance;
	return new_instance;
	
}	

/**     
 * \brief 合成普通物品
 *
 * \param user: 请求合成的用户
 * \param cmd: 合成指令
 * \return 合成成功返回true,否则返回false
 */
bool RebuildObject::compose(SceneUser& user, const Cmd::stPropertyUserCmd* cmd)
{
	Cmd::stComposeItemPropertyUserCmd * command = (Cmd::stComposeItemPropertyUserCmd *)cmd;

	ObjectPack* pack = user.packs.equip.pack(EquipPack::R_MAKE);
	if (command->location == 1) pack = user.packs.equip.pack(EquipPack::L_MAKE);
	//if (!pack || pack->object()->data.dur < 1 || user.tradeorder.in_trade(pack->object()) ) 
	if (!pack || user.tradeorder.in_trade(pack->object()) ) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求合成的包裹不存在，使用次数不够或在交易中", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : id(0), level(0), num(0), max_num(0), can_compose(true), compose_id(0)
		{ }
		
		bool exec(zObject* ob)
		{
			if (! ( (ob->base->recastlevel && ob->data.upgrade < ob->base->recastlevel) ||(ob->base->make && ob->base->make != 1) )) 
            {
				can_compose = false;
				return false;	
			}
			if (id && (ob->base->id != id || ob->data.upgrade != level)) 
            {
				can_compose = false;	
				return false;
			}
			
			if (!id) 
            {
				id = ob->base->id;
				level = ob->data.upgrade;
				max_num = ob->base->maxnum;
				compose_id = ob->base->make;
			}
			
			num += ob->data.dwNum;		
			
			return true;	
		}
		
		DWORD id;
		BYTE level;
		DWORD num;
		DWORD max_num;
		bool can_compose;
		WORD compose_id;
		
	};
	
	do_compose dc;
	pack->execEvery(dc);
	
	if (!dc.can_compose || !dc.num ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)请求合成不可合成物品", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}

	if ( command->per_num <3 || command->per_num > 5 || (dc.num % command->per_num) || (dc.num > command->per_num*dc.max_num) ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)请求合成物品数量错误", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	int expect_count = dc.num / command->per_num;

	zObjectB *base = objectbm.get(dc.id);
	
	DWORD gold = base->recastcost*expect_count; //base must exist
	if (!user.packs.checkMoney(gold)) 
    {
		return false;
	}
	
	if ( user.packs.removeMoney(gold,"普通合成") ) 
	{
        //soke 暂时无视月光宝盒的耐久
		//Base::refresh_pack(user, pack);
		
//		pack->clear(&user);
		user.packs.execEvery(pack, Type2Type<ClearPack>());
		
		int real_count = 0;

		int odds = 100;
		if (command->per_num == 3) 
        {
            odds=50;
        }
		else if (command->per_num == 4) 
        {
            odds=75;
        }

		do 
        {
			if (zMisc::selectByPercent(odds)) 
            {
				++real_count;
			}
		}while (--expect_count>0);

		if (!real_count) 
        {
			response(user, 1, COMPOSE);	
			return true;
		}

		zObject* o = NULL;
		if (dc.compose_id) 
        {
			base = objectbm.get(dc.compose_id);
			o = zObject::create(base, real_count, 0);
		}
        else 
        {
			//soke 控制合成材料的级别
			if(dc.level<6)
			{
				o = zObject::create(base, real_count, dc.level+1);
			}
		}
		
		if (o) 
        {
		    o->data.bind = 1; //soke 物品合成以后都是绑定的
			o->data.pos = stObjectLocation(pack->type(), pack->id(), 0, Cmd::MAKECELLTYPE_EQUIP);
			if (user.packs.addObject(o, false) ) 
            {
				Base::add_object(user, o);
				zObject::logger(o->createid,o->data.qwThisID,o->base->name,o->data.dwNum,o->data.dwNum,1,0,NULL,user.id,user.name,"普通合成",o->base,o->data.kind,o->data.upgrade);
				response(user, 0, COMPOSE);
				return true;
			}
		}
		
		response(user, 1, COMPOSE);			
		Zebra::logger->error("为用户(%d:%d:%s)添加合成物品时失败", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}
	
	return false;
}

/**     
 * \brief 合成魂魄石
 *
 * \param user: 请求合成的用户
 * \param cmd: 合成指令
 * \return 合成成功返回true,否则返回false
 */
bool RebuildObject::compose_soul_stone(SceneUser& user, const Cmd::stPropertyUserCmd* cmd)
{
	Cmd::stComposeSoulStonePropertyUserCmd * command = (Cmd::stComposeSoulStonePropertyUserCmd *)cmd;

	ObjectPack* pack = user.packs.equip.pack(EquipPack::R_MAKE);
	if (command->location == 1) pack = user.packs.equip.pack(EquipPack::L_MAKE);
	//if (!pack || pack->object()->data.dur < 1 || user.tradeorder.in_trade(pack->object()) ) 
	if (!pack || user.tradeorder.in_trade(pack->object()) ) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求绑定的包裹不存在，使用次数不够或在交易中", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : stone(NULL), c_ob(NULL), can_compose(true)
		{ }
		
		bool exec(zObject* ob)
		{
			if (ob->data.dwObjectID == 794) 
            {
				if (!stone) 
                {
					stone = ob;
				}
                else 
                {
					can_compose = false;
					return false;
				}
			}
			//soke 增加卫士、刺客、枪、战旗、马匹装备等装备可绑定
            else if ( ((ob->base->kind >= ItemType_ClothBody && ob->base->kind <=ItemType_Fing)|| (ob->base->kind >= ItemType_HorseShoe && ob->base->kind <=ItemType_HorseIron) 
		             || (ob->base->kind >= ItemType_Dagger && ob->base->kind <=ItemType_breastplate)|| (ob->base->kind >= ItemType_StaffFlag && ob->base->kind <=ItemType_SwordFlag)
					 || ob->base->kind == ItemType_LongGun || ob->base->kind == ItemType_Handgun) && !ob->data.bind) 
            {
				if (!c_ob) 
                {
					c_ob = ob;
				}
                else 
                {
					can_compose = false;
					return false;
				}	
			}
            else 
            {
				can_compose = false;
				return false;
			}
			
			return true;	
		}
		
		zObject* stone;
		zObject* c_ob;
		
		bool can_compose;
		
	};
	
	do_compose dc;
	pack->execEvery(dc);

	if (!dc.can_compose || !dc.stone || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)绑定装备时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	
	DWORD gold = 200;
	
	if (!user.packs.checkMoney(gold)) 
    {
		return false;
	}
	
	if ( user.packs.removeMoney(gold,"绑定装备") ) 
	{ 
        //soke 暂时无视月光宝盒的耐久
		//Base::refresh_pack(user, pack);
		
		dc.c_ob->data.bind = 1;
		/**
		 * \brief 这里的防御装备可能出现攻击力,攻击装备可能出现防御力,已经可策划确认过,如果改,fuck策划一辈子
		 *
		 */
		switch(dc.c_ob->base->kind)
		{
			case ItemType_ClothBody:
			case ItemType_FellBody:
			case ItemType_MetalBody:
			case ItemType_Shield:
			case ItemType_Helm:
			case ItemType_Caestus:
			case ItemType_Cuff:
			case ItemType_Shoes:
			case ItemType_Blade:
			case ItemType_Sword:
			case ItemType_Axe:
			case ItemType_Hammer:
			case ItemType_Crossbow:
			case ItemType_Staff:
			case ItemType_Fan:
			case ItemType_Stick:
			case ItemType_Necklace:
			case ItemType_Fing:
			case ItemType_HorseShoe:          //130代表坐骑马脚类
			case ItemType_HorseRope:          //131代表坐骑马绳类
			case ItemType_HorseSaddle:        //132代表坐骑马鞍类
			case ItemType_HorseSafe:          //133代表坐骑马甲类
			case ItemType_HorseIron:          //134代表坐骑马镫类
			case ItemType_Dagger:	          //136代表利刃
			case ItemType_NewSword:	          //137代表卫士剑
	        case ItemType_breastplate:	      //138代表护心镜
	        case ItemType_StaffFlag:		  //141代表战旗(法师)
	        case ItemType_CrossFlag:	      //142代表战旗(弓手)
	        case ItemType_Flag:	              //143代表战旗(仙术)
	        case ItemType_StickFlag:	      //144代表战旗(召唤)
	        case ItemType_BladeFlag:	      //145代表战旗(战士)
	        case ItemType_DaggerFlag:	      //146代表战旗(刺客)
	        case ItemType_SwordFlag:          //147代表战旗(卫士)
	        case ItemType_LongGun:            //155代表长枪
	        case ItemType_Handgun:            //156代表短枪
				{
					int index = 0;
					switch(dc.c_ob->base->kind)
					{
						case ItemType_ClothBody: //soke 防具
						case ItemType_FellBody:
						case ItemType_MetalBody:
						case ItemType_Shield:
						case ItemType_Helm:
						case ItemType_Caestus:
						case ItemType_Cuff:
						case ItemType_Shoes: //靴子
						case ItemType_HorseRope:          //131代表坐骑马绳类（防御）
			            case ItemType_HorseSaddle:        //132代表坐骑马鞍类（防御）
						case ItemType_breastplate:	      //138代表护心镜
						case ItemType_StaffFlag:		  //141代表战旗(法师)
	                    case ItemType_CrossFlag:	      //142代表战旗(弓手)
	                    case ItemType_Flag:	              //143代表战旗(仙术)
	                    case ItemType_StickFlag:	      //144代表战旗(召唤)
	                    case ItemType_BladeFlag:	      //145代表战旗(战士)
	                    case ItemType_DaggerFlag:	      //146代表战旗(刺客)
	                    case ItemType_SwordFlag:          //147代表战旗(卫士)
						case ItemType_Handgun:            //156代表短枪
						{
							if(!index)
							{
								//index = zMisc::randBetween(9, 10);
							}
						}
						break;
						case ItemType_Blade: //soke 物攻武器
						case ItemType_Sword:
						case ItemType_Axe:
						case ItemType_Hammer:
						case ItemType_Crossbow:
						{
							if(!index)
							{
								//index = 7;
							}
						}
						break;
						case ItemType_Staff: //soke 魔攻武器
						case ItemType_Fan:
						case ItemType_Stick:
						case ItemType_Dagger:	          //136代表利刃
						{
							if(!index)
							{
								//index = 8;
							}
						}
						break;
						case ItemType_Necklace: //项链  魔物双攻
						case ItemType_Fing:     //戒指
						case ItemType_HorseShoe:          //130代表坐骑马脚类（攻击）
			            case ItemType_HorseSafe:          //133代表坐骑马甲类（攻击）
			            case ItemType_HorseIron:          //134代表坐骑马镫类（攻击）
						case ItemType_NewSword:	          //137代表卫士剑
						case ItemType_LongGun:            //155代表长枪
						{
							if(!index)
							{
								if(dc.c_ob->data.maxpdamage)
								{
									//index = 7;
								}
								else if (dc.c_ob->data.maxmdamage)
								{
									//index = 8;
								}
								//index = zMisc::randBetween(7, 8);
							}
						}
						break;
						default:
						break;
					}
					//绑定现在100%得到属性
					/*
					   if ( zMisc::selectByPercent(75) ) 
					   {
					// */
					//int index = zMisc::randBetween(1, 10);
					switch(index)
					{
						/*
						   case 1:
						   {
						   dc.c_ob->data.str += zMisc::randBetween(1, 5);
						   }
						   break;
						   case 2:
						   {
						   dc.c_ob->data.inte += zMisc::randBetween(1, 5);
						   }
						   break;
						   case 3:
						   {
						   dc.c_ob->data.dex += zMisc::randBetween(1, 5);
						   }
						   break;
						   case 4:
						   {
						   dc.c_ob->data.spi += zMisc::randBetween(1, 5);
						   }
						   break;
						   case 5:
						   {
						   dc.c_ob->data.con += zMisc::randBetween(1, 5);
						   }
						   break;
						   case 6:
						   {
						   dc.c_ob->data.bang += 1;
						   }
						   break;
						// */
						case 7:
							{
								dc.c_ob->data.pdam += zMisc::randBetween(1, 3);
							}
							break;
						case 8:
							{
								dc.c_ob->data.mdam += zMisc::randBetween(1, 3);
							}
							break;
						case 9:
							{
								dc.c_ob->data.pdef += zMisc::randBetween(1, 5);
							}
							break;
						case 10:
							{
								dc.c_ob->data.mdef += zMisc::randBetween(1, 5);
							}
							break;
						default:
							break;
					}
					//}
				}
				break;
			default:
				break;
		}

		zObject* ob = zObject::create(dc.c_ob);
		user.packs.execEvery(pack, Type2Type<ClearPack>());
		if (ob) 
        {	
			ob->data.pos = stObjectLocation(pack->type(), pack->id(), 0, Cmd::MAKECELLTYPE_EQUIP);
			if (user.packs.addObject(ob, false)) 
            {
				Base::add_object(user, ob, false);
				response(user, 0, COMPOSE);
			}
		}
						
		return true;
	}

	return false;
}

/**     
 * \brief 升星物品
 *
 * \param user: 请求升星的用户
 * \param cmd: 升星指令
 * \return 升星成功返回true,否则返回false
 */
bool RebuildObject::upgrade(SceneUser& user, const Cmd::stPropertyUserCmd* cmd)
{
	Cmd::stUpgradeItemPropertyUserCmd * command = (Cmd::stUpgradeItemPropertyUserCmd *)cmd;
	ObjectPack* pack = user.packs.equip.pack(EquipPack::R_MAKE);
	if (command->location == 1) pack = user.packs.equip.pack(EquipPack::L_MAKE);
	//if (!pack || pack->object()->data.dur < 1 || user.tradeorder.in_trade(pack->object()) ) {
	if ( !pack || user.tradeorder.in_trade(pack->object()) ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)请求合成的包裹不存在，使用次数不够或在交易中", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true), need(0)
		{ }

		bool exec(zObject* ob)
		{

			if (!up_ob && ob->base->recast) 
            {
				up_ob = ob;
			}
            else 
            {
				if (!c_ob) 
                {
					c_ob = ob;
                    //count += ob->data.dwNum;
					count += 1;
					return true;
				}

				if ( ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
                {
                    //count += ob->data.dwNum;
					count += 1;
					return true;
				}
				can=false;
				//return false;
			}
			
			return true;	
		}

        //这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
            DWORD num = count >= 4 ? 4 : count;
			return 2*(num-1);
		}
		
		//需要升级的物品
		zObject* up_ob;
		//材料
		zObject* c_ob;
		zObject* odds_ob;
		int count;
		
		bool can;
		//升级所需银子
		DWORD need;

		//升星文件是否存在
		bool is_stuff()
		{
			zUpgradeObjectB *base = upgradeobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.upgrade+1));
			if(!base) return false;

			need = base->gold;
			return c_ob->data.dwObjectID == base->stuff;
		}

		bool ispurple_stuff()  //sky 紫
		{
			zUpgradePurpleB *base = upgradepurplebm.get(up_ob->data.dwObjectID+100000*(up_ob->data.upgrade+1));
			if(!base) return false;

			need = base->gold;
			return c_ob->data.dwObjectID == base->stuff;
		}

		bool amulet_stuff()
		{
			zAmuletObjectB *base = amuletobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.upgrade+1));
			if(!base) return false;

			need = base->gold;
			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{
			
			if ( (!(up_ob->data.kind & 4) ||(up_ob->data.kind & 16)) && up_ob->base->kind != ItemType_Amulets )
			{
				return count>=1 && up_ob && ispurple_stuff() && can;	
			}
			else if ( ((up_ob->data.kind & 4) || !(up_ob->data.kind & 16)) && up_ob->base->kind != ItemType_Amulets )
			{
				return count>=1 && up_ob && is_stuff() && can;
			}
			else if ( up_ob->base->kind == ItemType_Amulets )
			{
				return count>=1 && up_ob && amulet_stuff() && can;	
			}
			return false;
		}
	};
	
	do_compose dc;
	pack->execEvery(dc);
/* 	Zebra::logger->debug("dc.can_compose_sta");
	bool test = !(dc.c_ob->data.dwObjectID >= 2960 && dc.c_ob->data.dwObjectID <= 2974);
	Zebra::logger->debug("dc.can_compose_end"); */
	//if (!dc.can_compose() && !(dc.c_ob->data.dwObjectID >= 2960 && dc.c_ob->data.dwObjectID <= 2974) && dc.count>=1 && dc.up_ob && dc.can) 
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)绑定装备时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if (!dc.can_compose() && !(dc.c_ob->data.dwObjectID >= 2960 && dc.c_ob->data.dwObjectID <= 2974)) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级物品", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}


	if (!user.packs.checkMoney(dc.need)) 
    {
		return false;
	}
	
	if ( user.packs.removeMoney(dc.need,"升级") ) 
    {
        //ZULU 暂时无视月光宝盒的耐久
		//Base::refresh_pack(user, pack);
		
		bool ret = false;
		if (dc.up_ob->data.needlevel < 40)
		{
			if (dc.c_ob->data.dwObjectID == 681)
			{
				ret = Upgrade::upgrade(user,dc.up_ob,dc.odds());
			}else
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "当前需要升级宝石!");
				return false;
			}
		}
		//else if ( (!(dc.up_ob->data.kind & 4) ||(dc.up_ob->data.kind & 16)) && dc.up_ob->base->kind != ItemType_Amulets )
		else if ( (dc.up_ob->data.kind & 16) && dc.up_ob->base->kind != ItemType_Amulets )
		{
			if (dc.c_ob->data.dwObjectID == 799)
			{
				ret = Upgrade::purple_upgrade(user,dc.up_ob,dc.odds());
			}
			else if(dc.c_ob->data.dwObjectID >= 2960 && dc.c_ob->data.dwObjectID <= 2974)
			{
				int sjxingxing = dc.c_ob->data.dwObjectID - 2959 - dc.up_ob->data.upgrade;
				if(sjxingxing > 0)
				{
					while (sjxingxing > 0)
					{
						sjxingxing--;
						ret = Upgrade::purple_upgrade(user,dc.up_ob,100);
					}
				}
				else
				{
					Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "无法使用此升星石升星!");
					return false;
				}
			}
			else
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "当前需要无暇升级宝石!");
				return false;
			}
			
		}
		//else if ( ((dc.up_ob->data.kind & 4) || !(dc.up_ob->data.kind & 16)) && dc.up_ob->base->kind != ItemType_Amulets )
		else if ( (dc.up_ob->data.kind & 4)  && dc.up_ob->base->kind != ItemType_Amulets )
		{
			if (dc.c_ob->data.dwObjectID == 795)
			{
				ret = Upgrade::upgrade(user,dc.up_ob,dc.odds());
			}
			else if(dc.c_ob->data.dwObjectID >= 2960 && dc.c_ob->data.dwObjectID <= 2974)
			{
				int sjxingxing = dc.c_ob->data.dwObjectID - 2959 - dc.up_ob->data.upgrade;
				if(sjxingxing > 0)
				{
					while (sjxingxing > 0)
					{
						sjxingxing--;
						ret = Upgrade::upgrade(user,dc.up_ob,100);
					}
				}
				else
				{
					Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "无法使用此升星石升星!");
					return false;
				}
			}
			else
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "当前需要精致升级宝石!");
				return false;
			}
		}
		else if ( dc.up_ob->base->kind == ItemType_Amulets )
		{
			if (dc.c_ob->data.dwObjectID == 34567)
			{
				ret = Upgrade::hsf_upgrade(user,dc.up_ob,dc.odds());
			}else
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "当前需要无暇升级宝石!");
				return false;
			}
		}

		//绿装备升级
		//bool ret = Upgrade::upgrade(user, dc.up_ob, dc.odds());
		
		//pack->clear(&user, dc.up_ob);

		zObject* ob = zObject::create(dc.up_ob);
//		user.packs.clearPackage(pack);
		//user.packs.execEvery(pack, Type2Type<ClearPack>());
        struct MultiItemDelCB : public PackageCallback
        {
            MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
            { }

            bool exec(zObject* ob)
            {
				if ( 795 == ob->data.dwObjectID || 681 == ob->data.dwObjectID || 799 == ob->data.dwObjectID ||(ob->data.dwObjectID >= 2960 && ob->data.dwObjectID <= 2974)  || 34567 == ob->data.dwObjectID)
                {
                    delObj = ob;

                    /* static const DWORD MAX_DEL_COUNT = 4;
                    if ( pOwner && realDelCnt < MAX_DEL_COUNT )
                    {
                        DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

                        DWORD realDelNum = 0;
                        if ( delObj->data.dwNum > needDelNum )
                        {
                            realDelNum = needDelNum;
                            delObj->data.dwNum -= realDelNum;

                            Cmd::stRefCountObjectPropertyUserCmd send;
                            send.qwThisID = delObj->data.qwThisID;
                            send.dwNum = delObj->data.dwNum;
                            pOwner->sendCmdToMe(&send, sizeof(send));
                        }
                        else
                        {
                            realDelNum = delObj->data.dwNum;
                            zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"装备升星删除",delObj->base,delObj->data.kind,delObj->data.upgrade);
                            pOwner->packs.removeObject(delObj);
                        }
                        realDelCnt += realDelNum;
                    }
                    else
                    {
                        //已经扣除四颗，不用扣除了
                        return true;
                    } */
					if ( delObj->data.dwNum > 1 )
					{
						delObj->data.dwNum -= 1;

						Cmd::stRefCountObjectPropertyUserCmd send;
						send.qwThisID = delObj->data.qwThisID;
						send.dwNum = delObj->data.dwNum;
						pOwner->sendCmdToMe(&send, sizeof(send));
					}
					else
					{
						//zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"装备升星删除",delObj->base,delObj->data.kind,delObj->data.upgrade);
						pOwner->packs.removeObject(delObj);
					}
					return true;
                }
                else
                {
                    oldPos = ob->data.pos;       
                    pOwner->packs.removeObject(ob);
                }
                return true;	
            }

            zObject* up_ob;
            zObject* delObj;
            DWORD  realDelCnt; //实际删除的数量
            SceneUser* pOwner;
            stObjectLocation oldPos;
        };

        MultiItemDelCB del(&user);
        pack->execEvery(del);
        if ( ob ) 
        {	
			zObject *temp = NULL;
			pack->getObjectByZone(&temp, 0, 0);
			if (temp)
			{
				user.packs.moveObject(&user,temp,del.oldPos);
				Base::add_object(user, temp, false);
				//Zebra::logger->debug("中间有物品");
			}
			ob->data.pos = stObjectLocation(pack->type(), pack->id(), 0, Cmd::MAKECELLTYPE_EQUIP);
			if (user.packs.addObject(ob, false)) 
            {			
				//zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"升级生成",ob->base,ob->data.kind,ob->data.upgrade);
				Base::add_object(user, ob, false);
				response(user, ret?0:1, UPGRADE);
			}
		}
		
		return true;
	}

	return false;
}


/**     
 * \brief 装备打孔
 *
 * \param user: 请求打孔的用户
 * \param cmd: 打孔指令
 * \return 打孔成功返回true,否则返回false
 */
bool RebuildObject::hole(SceneUser& user, const Cmd::stPropertyUserCmd* cmd)
{
	Cmd::stHolePropertyUserCmd * command = (Cmd::stHolePropertyUserCmd *)cmd;
		
	zObject *up_ob = user.packs.uom.getObjectByThisID(command->up_id);
	if (!up_ob || !Hole::can_hole(up_ob)) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求对不能打孔装备打孔", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if (!check_npc(user, up_ob->base, NpcTrade::NPC_HOLE_OBJECT)) 
	{
		return false;
	}
	
	zObject* ob = user.packs.uom.getObjectByID(command->gem_id, command->gem_level, true);
	if (!ob || ob->data.dwObjectID != HOLE_SONTE_ID || Hole::get_hole_num(up_ob) == Hole::INVALID_NUM || ob->data.upgrade != (Hole::get_hole_num(up_ob)-1) ) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)对装备打孔时缺少宝石", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}

	DWORD gold = HOLE_MONEY*(1 << ob->data.upgrade);

	DWORD taxMoney = (DWORD)((gold*(user.scene->getTax())/100.0f)+0.5f); // 买东西收税
	gold = gold + taxMoney;
	Cmd::Session::t_taxAddCountry_SceneSession send;
	send.dwCountryID = user.scene->getCountryID();
	send.qwTaxMoney = taxMoney;
	sessionClient->sendCmd(&send, sizeof(send));

	if (!user.packs.checkMoney(gold)) 
	{
		return false;
	}
	
	if ( user.packs.removeMoney(gold,"打孔") ) 
	{
		Hole::add_hole(up_ob, ob->data.upgrade);
		user.reduceObjectNum(command->gem_id, 1, command->gem_level);
		add_object(user, up_ob, false);
		response(user, 0, HOLE);
		return true;

	}

	return false;
	
}

/**     
 * \brief 镶嵌物品
 *
 * \param user: 请求镶嵌的用户
 * \param cmd: 镶嵌指令
 * \return 镶嵌成功返回true,否则返回false
 */
bool RebuildObject::enchance(SceneUser& user, const Cmd::stPropertyUserCmd* cmd)
{
	Cmd::stEnchasePropertyUserCmd * command = (Cmd::stEnchasePropertyUserCmd *)cmd;

	ObjectPack* pack = user.packs.equip.pack(EquipPack::R_MAKE);
	if (command->location == 1) pack = user.packs.equip.pack(EquipPack::L_MAKE);
	//if (!pack || pack->object()->data.dur < 1 || user.tradeorder.in_trade(pack->object()) ) 
	if (!pack || user.tradeorder.in_trade(pack->object()) ) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求合成的包裹不存在，使用次数不够或在交易中", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	//static _obj_need_store[][]={{101,748},{101,748},{101,748},{101,748},
	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), s_ob(NULL), can(true)
		{ }
		
		bool exec(zObject* ob)
		{
			if (!c_ob && ob->data.dwObjectID == ENCHANCE_SONTE_ID) 
			{
				c_ob = ob;	
			}
			else if (!s_ob && ob->base->kind==ItemType_SOULSTONE) 
			{
				if(up_ob)
				{
					switch(ob->base->id)
					{
						//恋恋修改魂魄石
						case 680:
						{
							if ((up_ob->base->kind >= 101 && up_ob->base->kind <= 118) || (up_ob->base->kind >= 130 && up_ob->base->kind <= 145) || (up_ob->base->kind >= 155 && up_ob->base->kind <= 156))
							{
								s_ob = ob;
							}
						}
						break;
						case 748:
						{
							if ((up_ob->base->kind >= 101 && up_ob->base->kind <= 118) || (up_ob->base->kind >= 130 && up_ob->base->kind <= 145) || (up_ob->base->kind >= 155 && up_ob->base->kind <= 156))
							{
								s_ob = ob;
							}
						}
						break;
						case 749:
						{
							if ((up_ob->base->kind >= 101 && up_ob->base->kind <= 118) || (up_ob->base->kind >= 130 && up_ob->base->kind <= 145) || (up_ob->base->kind >= 155 && up_ob->base->kind <= 156))
							{
								s_ob = ob;
							}
						}
						break;
						case 750:
						{
							if ((up_ob->base->kind >= 101 && up_ob->base->kind <= 118) || (up_ob->base->kind >= 130 && up_ob->base->kind <= 145) || (up_ob->base->kind >= 155 && up_ob->base->kind <= 156))
							{
								s_ob = ob;
							}
						}
						break;
						case 751:
						{
							if ((up_ob->base->kind >= 101 && up_ob->base->kind <= 118) || (up_ob->base->kind >= 130 && up_ob->base->kind <= 145) || (up_ob->base->kind >= 155 && up_ob->base->kind <= 156))
							{
								s_ob = ob;
							}
						}
						break;
						default:
							{
								can = false;
								return false;
							}
							break;
					}
				}
                else
                {
					s_ob = ob;	
				}
			}
            else if (!up_ob) 
            {
				if(s_ob)
                {
					switch(ob->base->kind)
                    {
						case 101 ... 111:
						case 130 ... 140:
						case 155 ... 156:
							{
								if(s_ob->base->id == 680 || s_ob->base->id == 748 || s_ob->base->id == 749 || s_ob->base->id == 750 || s_ob->base->id == 751 || s_ob->base->id >= 101|| s_ob->base->id <= 110)
								{
									up_ob = ob;
								}
							}
							break;
						default:
							{
								can = false;
								return false;
							}
							break;
					}
				}
                else
                {
					up_ob = ob;
				}
			}
            else 
            {
				can = false;
				return false;
			}
			
			return true;	
		}
		
		zObject* up_ob;
		zObject* c_ob;
		zObject* s_ob;
		
		bool can;
		
		bool can_compose()
		{
			return can && up_ob && c_ob && s_ob && 
				Hole::get_hole_num(up_ob) != Hole::INVALID_NUM &&  Hole::get_empty_hole(up_ob) != Hole::INVALID_INDEX &&
				c_ob->data.upgrade == Hole::get_empty_hole(up_ob);
		}
	};
	
	do_compose dc;
	pack->execEvery(dc);

	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if (!dc.can_compose()) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)请求镶嵌不可镶嵌物品", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}

	DWORD gold = ENCHANCE_MONEY*(1 << dc.c_ob->data.upgrade);

	if (!user.packs.checkMoney(gold)) 
    {
		return false;
	}
	
	if ( user.packs.removeMoney(gold,"镶嵌") ) 
	{
        //soke 暂时无视月光宝盒的耐久
		//Base::refresh_pack(user, pack);
		
		Hole::put_hole(dc.up_ob, dc.c_ob->data.upgrade, dc.s_ob->data.dwObjectID);
		
		SoulStone::enchance(user, dc.up_ob, dc.s_ob);	
//	pack->clear(&user, dc.up_ob);
		zObject* ob = zObject::create(dc.up_ob);
//		user.packs.clearPackage(pack);
		user.packs.execEvery(pack, Type2Type<ClearPack>());
		if (ob) 
        {
			ob->data.pos = stObjectLocation(pack->type(), pack->id(), 0, Cmd::MAKECELLTYPE_EQUIP);
			if (user.packs.addObject(ob, false)) 
            {
				Base::add_object(user, ob, false);
				response(user, 0, ENCHANCE);
			}
		}		
		return true;
	}

	return false;
}

/**     
 * \brief 分解物品
 *
 * \param user: 请求分解的用户
 * \param cmd: 分解指令
 * \return 分解成功返回true,否则返回false
 */
bool RebuildObject::decompose(SceneUser& user, const Cmd::stPropertyUserCmd* cmd)
{
	Cmd::stDecomposePropertyUserCmd * command = (Cmd::stDecomposePropertyUserCmd *)cmd;
		
	zObject *up_ob = user.packs.uom.getObjectByThisID(command->up_id);
	if (!up_ob || up_ob->base->make != 1) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)请求分解不能分解装备", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}
	
	if (!check_space(user, 1, 1)) 
    {
		response(user, 1, DECOMPOSE);
		return false;
	}

	if (!check_npc(user, up_ob->base, NpcTrade::NPC_DECOMPOSE_OBJECT)) 
    {
		return false;
	}

	// 已绑定的物品需要验证是否已受保护
	if (up_ob->data.bind && user.isSafety(Cmd::SAFE_SPLIT_OBJECT)) return false;

	Decompose decompose(up_ob);

	DWORD gold = decompose.gold();

	DWORD taxMoney = (DWORD)((gold*(user.scene->getTax())/100.0f)+0.5f); // 买东西收税
	gold = gold + taxMoney;
	Cmd::Session::t_taxAddCountry_SceneSession send;
	send.dwCountryID = user.scene->getCountryID();
	send.qwTaxMoney = taxMoney;
	sessionClient->sendCmd(&send, sizeof(send));

	if (!user.packs.checkMoney(gold)) 
    {
		return false;
	}
	
	if ( user.packs.removeMoney(gold,"分解") ) 
	{
		decompose.bonus_exp(user);
	//	if (up_ob->data.maker[0] ) 
	//   {
	//		decompose.bonus_items(user);
	//	}
		decompose.remove_from(user);
		response(user, 0, DECOMPOSE);	
		return true;	
	}
	
	response(user, 1, DECOMPOSE);
	return false;
}


/**     
 * \brief 打造物品
 *
 * \param user: 请求打造的用户
 * \param cmd: 打造指令
 * \return 打造成功返回true,否则返回false
 */
bool RebuildObject::make(SceneUser& user, const Cmd::stPropertyUserCmd* cmd)
{
	Cmd::stFoundItemPropertyUserCmd *command = (Cmd::stFoundItemPropertyUserCmd *)cmd;

	zObjectB *up_ob = objectbm.get(command->up_id);
	
	if (!up_ob || up_ob->make != 1) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)请求打造不可打造物品", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;	
	}
	
	if (!check_space(user, up_ob->width, up_ob->height)) 
    {
		response(user, 1, MAKE);
		return false;
	}
	
	DWORD dwGold = up_ob->need_material.gold;

	DWORD taxMoney = (DWORD)((dwGold*(user.scene->getTax())/100.0f)+0.5f); // 买东西收税
	dwGold = dwGold + taxMoney;
	Cmd::Session::t_taxAddCountry_SceneSession send;
	send.dwCountryID = user.scene->getCountryID();
	send.qwTaxMoney = taxMoney;
	sessionClient->sendCmd(&send, sizeof(send));

	if (!user.packs.checkMoney(dwGold)) 
    {
		return false;
	}

	EquipMaker make(&user);
	std::map<DWORD, DWORD> list;
		
	int count = -1;
	int level = 0;
	while (++count < (int)command->count) make.pre_level_of_material(command->list[count].gem_id, command->list[count].gem_level);

	count = -1;	
	while ( ++count < (int)command->count) 
    {
		if (!user.packs.uom.exist(command->list[count].gem_id, command->list[count].gem_num, command->list[count].gem_level)) 
        {
			return false;
		}
		if ( make.is_odds_gem(up_ob->kind, command->list[count].gem_id) && command->list[count].gem_num == 1 ) continue;
		
		if (!level) level = command->list[count].gem_level;
		if ( up_ob->kind == ItemType_Resource && level != command->list[count].gem_level ) return false;
		
		if ( ! make.level_of_material(command->list[count].gem_id, command->list[count].gem_num, command->list[count].gem_level, up_ob) ) 
        {
			return false;
		}
		list[command->list[count].gem_id] += command->list[count].gem_num;
	}
		
	if ( !make.check_skill(user, up_ob) ) 
    {
		return false;
	}
	
	if (!make.check_material(up_ob, list)) 
    {
		return false;
	}
	
	count = -1;
	while ( ++count < (int)command->count) 
    {
		user.reduceObjectNum(command->list[count].gem_id, command->list[count].gem_num, command->list[count].gem_level);
	}	

	user.packs.removeMoney(dwGold,"打造");

	zObject *ob = NULL;
	if (up_ob->kind == ItemType_Resource) 
    {
		ob = make.make_material(up_ob);
	}
    if (check_npc(user, up_ob, NpcTrade::NPC_MAKE_OBJECT)) 
    {
		ob = make.make(&user, up_ob);
	}
	else if (check_npc(user, up_ob, NpcTrade::NPC_ZSMAKE_OBJECT)) 
	{
		ob = make.zsmake(&user, up_ob);
	}
	else
	{
		Zebra::logger->debug("用户%s打造%s失败,可能是npctrade填写有问题",user.name,up_ob->name);
		return false;
	}

	if(ob && user.packs.addObject(ob, true, AUTO_PACK)) 
    {
		//soke 打造装备和马匹装备绑定
		   if((ob->base->kind >= ItemType_ClothBody && ob->base->kind <= ItemType_Fing) 
		   || (ob->base->kind >= ItemType_HorseShoe && ob->base->kind <= ItemType_HorseIron)
		   || (ob->base->kind >= ItemType_Dagger && ob->base->kind <= ItemType_breastplate) 
		   || (ob->base->kind >= ItemType_StaffFlag && ob->base->kind <= ItemType_SwordFlag)
		   || (ob->base->kind == ItemType_BMW )
		   || (ob->base->kind == ItemType_LongGun) 
		   || (ob->base->kind == ItemType_Handgun)
		   || (ob->base->kind >= ItemType_ShangfangBaojian && ob->base->kind <= ItemType_Shenshou2))
		   {
			  ob->data.bind=1;
		   }
		zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"打造生成",ob->base,ob->data.kind,ob->data.upgrade);
	 	make.add_skill(user, up_ob);

		add_object(user, ob);

		int coefficient = 1;
		int status = 1;
		if (up_ob->kind != ItemType_Resource) 
        {
			if (ob->data.kind & 1) 
            { //blue
				coefficient = 2;
				status = 2;
			}
			if (ob->data.kind & 2) 
            { //gold
				coefficient = 15;
				status = 3;
			}
			if (ob->data.kind & 4) 
            { //holy
				coefficient = 30;
				status = 4;
			}
			//soke 紫装
			if (ob->data.kind & 16) 
			{ //purple
				coefficient = 60;
				status = 5;
			}
			make.add_exp(user, coefficient);
		}
			
		response(user, status+1, MAKE);
		if (up_ob->needlevel>=80)
		{
        //zjes 打造去掉功勋
			/*if ((int)user.charbase.exploit>(5*exploit_arg)) 
			{
				user.charbase.exploit -= (5*exploit_arg);
			}
			else 
			{
				user.charbase.exploit = 0;
			}*/
			Cmd::stAddUserMapScreenUserCmd cmd;
			user.full_t_MapUserData(cmd.data);
			user.sendCmdToMe(&cmd,sizeof(cmd));				     
			//Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "打造80级以上装备，减去5点功勋值");
		}
		return true;
	}
	
	response(user, 1, MAKE);


	return false;
}

bool RebuildObject::recast( SceneUser& user,zObjectB *base, const Cmd::stPropertyUserCmd* cmd )
{
    Cmd::ItemRecastPropertyUserCmd *rev = (Cmd::ItemRecastPropertyUserCmd *)cmd;
    if( !rev ) return false;


    SceneUser *pUser = reinterpret_cast<SceneUser *>(&user);
    if ( !pUser ) return true;

    switch ( rev->act )
    {
//装备助手开始/////////////////////////////////////////////////////////		
    case Cmd::RECAST_ITEM_COMPOSE11:  //装备祝福
        {
            return equipCompose11(user,base, rev);
        } 
        break;
    case Cmd::RECAST_ITEM_COMPOSE12:  //斗魂升级
        {
            return equipCompose12(user,base, rev);
        }
        break;
    case Cmd::RECAST_ITEM_COMPOSE13:  //神石镶嵌
        {
            return equipCompose13(user,base, rev);
        }
        break;
	case Cmd::RECAST_ITEM_COMPOSE14:  //装备栏激活
        {
            return equipCompose14(user,base, rev);
        }
        break;
	case Cmd::RECAST_ITEM_COMPOSE15:  //装备栏进阶
        {
            return equipCompose15(user,base, rev);
        }
        break;
	case Cmd::RECAST_ITEM_COMPOSE16:  //神器强化
        {
            return equipCompose16(user,base, rev);
        }
        break;	
	case Cmd::RECAST_ITEM_COMPOSE17:  //龙槽激活
        {
            return equipCompose17(user,base, rev);
        }
        break;
	case Cmd::RECAST_ITEM_COMPOSE18:  //龙槽解封
       {
           return equipCompose18(user,base, rev);
        }
        break;
	case Cmd::RECAST_ITEM_COMPOSE19:  //龙星升级
        {
           return equipCompose19(user,base, rev);
        }
        break;
	case Cmd::RECAST_ITEM_COMPOSE20:  //魂魄清除
        {
            return equipCompose20(user,base, rev);
        }
        break;
	case Cmd::RECAST_ITEM_COMPOSE21:  //装备升级
		{
			return equipCompose21(user,base, rev);
		} 
		break;
	case Cmd::RECAST_ITEM_COMPOSE22:  //马牌精炼
        {
            return equipCompose22(user,base, rev);
        }
        break;
	case Cmd::RECAST_ITEM_COMPOSE23:  //神龙宝宝镶嵌
        {
            return equipCompose23(user,base, rev);
        }
		break;	
	case Cmd::RECAST_ITEM_COMPOSE24:  //装备逆天进阶
        {
            return equipCompose24(user,base, rev);
        }
		break;
	case Cmd::RECAST_ITEM_COMPOSE25:  //神魂宝石进阶
        {
            return equipCompose25(user,base, rev);
        }
		break;
	case Cmd::RECAST_ITEM_COMPOSE26:  //暗影宝石镶嵌
        {
            return equipCompose26(user,base, rev);
        }
		break;	
	case Cmd::RECAST_ITEM_COMPOSE27:  //龙凤吟
        {
            return equipCompose27(user,base, rev);
        }
		break;
	case Cmd::RECAST_ITEM_COMPOSE28:  //轰天宝石镶嵌
        {
            return equipCompose28(user,base, rev);
        }
		break;	
	case Cmd::RECAST_ITEM_COMPOSE29:  //王者升级
        {
            return equipCompose29(user,base, rev);
        }
		break;
//附件助手开始/////////////////////////////////////////////////////////
	case Cmd::RECAST_ITEM_COMPOSE51:  //勋章升级
        {
            return equipCompose51(user,base, rev);
        }
        break;
	case Cmd::RECAST_ITEM_COMPOSE52:  //寒冰升级
        {
            return equipCompose52(user,base, rev);
        }
        break;
	case Cmd::RECAST_ITEM_COMPOSE53:  //翅膀升级
        {
            return equipCompose53(user,base, rev);
        }
        break;	
	case Cmd::RECAST_ITEM_COMPOSE54:  //神兵升级
        {
            return equipCompose54(user,base, rev);
        }
        break;
	case Cmd::RECAST_ITEM_COMPOSE55:  //神兽升级
        {
            return equipCompose55(user,base, rev);
        }
        break;	
	case Cmd::RECAST_ITEM_COMPOSE56:  //护身符璀璨宝石镶嵌
        {
            return equipCompose56(user,base, rev);
        }
		break;	
	case Cmd::RECAST_ITEM_COMPOSE57:  //护身符神兽宝石镶嵌
        {
            return equipCompose57(user,base, rev);
        }
		break;
	case Cmd::RECAST_ITEM_COMPOSE58:  //尚方宝剑升级
        {
            return equipCompose58(user,base, rev);
        }
		break;
	case Cmd::RECAST_ITEM_COMPOSE59:  //紫金龙袍升级
        {
            return equipCompose59(user,base, rev);
        }
		break;	
	case Cmd::RECAST_ITEM_COMPOSE61:  //神斧
        {
            return equipCompose61(user,base, rev);
        }
		break;	
	case Cmd::RECAST_ITEM_COMPOSE62:  //黑曜
        {
            return equipCompose62(user,base, rev);
        }
		break;	
	case Cmd::RECAST_ITEM_COMPOSE63:  //精金
        {
            return equipCompose63(user,base, rev);
        }
		break;	
	case Cmd::RECAST_ITEM_COMPOSE64:  //金箍咒强化
        {
            return equipCompose64(user,base, rev);
        }
		break;
	case Cmd::RECAST_ITEM_COMPOSE65:  //金箍咒字纹
        {
            return equipCompose65(user,base, rev);
        }
		break;
	case Cmd::RECAST_ITEM_COMPOSE66:  //金箍咒淬炼
        {
            return equipCompose66(user,base, rev);
        }
		break;
	case Cmd::RECAST_ITEM_COMPOSE67:  //寒冰神炼
        {
            return equipCompose67(user,base, rev);
        }
		break;
	case Cmd::RECAST_ITEM_COMPOSE68:  //寒冰字纹
        {
            return equipCompose68(user,base, rev);
        }
		break;
	case Cmd::RECAST_ITEM_COMPOSE69:  //寒冰纹章
        {
            return equipCompose69(user,base, rev);
        }
		break;	
//图鉴助手开始/////////////////////////////////////////////////////////
	case Cmd::RECAST_ITEM_COMPOSE101:  //百兽图鉴进阶
        {
            return equipCompose101(user,base, rev);
        }
		break;
	case Cmd::RECAST_ITEM_COMPOSE102:  //百兽图鉴签名
        {
            return equipCompose102(user,base, rev);
        }
		break;			
//法宝助手开始/////////////////////////////////////////////////////////
	case Cmd::RECAST_ITEM_COMPOSE103:  //法宝进阶
        {
            return equipCompose103(user,base, rev);
        }
		break;
	case Cmd::RECAST_ITEM_COMPOSE104:  //符石进阶
        {
            return equipCompose104(user,base, rev);
        }
		break;	
//十二星宿/////////////////////////////////////////////////////////
	case Cmd::RECAST_ITEM_COMPOSE60:  //十二星宿进阶
        {
            return equipCompose105(user,base, rev);
        }
		break;		
	case Cmd::RECAST_ITEM_COMPOSE106:  //元神进阶
        {
            return equipCompose106(user,base, rev);
        }
		break;	
	case Cmd::RECAST_ITEM_COMPOSE107:  //元神装备进阶
        {
            return equipCompose107(user,base, rev);
        }
		break;
	case Cmd::RECAST_ITEM_COMPOSE108:  //元神宝石镶嵌
        {
            return equipCompose108(user,base, rev);
        }
		break;		
    default:
        break;
        }
	return true;
}

//装备祝福
/*bool RebuildObject::equipCompose11( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
    if (!check_space(user, 1, 1)) 
    {
        Base::reply(user, 1, EQUIP_COMPOSE11, "包裹请保留一个空格");
        return false;
    }
    RecastPack* pack = &(user.packs.recastPack);
    if (!pack)
    {
        Zebra::logger->alarm("用户(%d:%d:%s)请求合成的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
        return false;
    }
    
    struct CheckSoulStone : public PackageCallback
    {
    public:
        CheckSoulStone() 
            : stone(NULL), eqpObj(NULL), canBind(true)
        {}

        bool exec(zObject* ob)
        {
			//soke 祝福石 1317
            if ( JBIND_SOUL_STONE_ID == ob->data.dwObjectID ) 
            {
                if (!stone) 
                {
                    stone = ob;
                }
                else 
                {
                    canBind = false;
                    return false;
                }
            }
            //soke 能祝福的条件 大于等于120级、绑定的绿装15星
			else if (ob->data.needlevel >= 120 || ob->data.bind || !ob->data.kind & 16 || (ob->data.kind & 4 || ob->data.upgrade >= 15) )
            {
                if (!eqpObj) 
                {
                    eqpObj = ob;
                }
                else 
                {
                    canBind = false;
                    return false;
                }	
            }
            else 
            {
                canBind = false;
                return false;
            }
            return true;	
        }

    public:
        zObject* stone;
        zObject* eqpObj;
        bool canBind;
    };

    CheckSoulStone chkStn;
    pack->execEvery(chkStn);

    //soke 条件判断
     if (!chkStn.stone || !chkStn.eqpObj || chkStn.eqpObj->data.needlevel < 70 
		 || !chkStn.eqpObj->data.bind || !chkStn.eqpObj->data.kind & 4 || chkStn.eqpObj->data.upgrade < 15 )  
    {
        Base::reply(user, 1, EQUIP_COMPOSE11, "装备祝福");
        Zebra::logger->alarm("用户(%d:%d:%s)请求对不能对该装备神圣祝福", user.charbase.accid, user.charbase.id, user.charbase.name);
      	Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备神圣祝福失败，要祝福的装备必须是120级以上绑定的15星绿装");
		return false;			
    }

    DWORD gold = 5000; //soke 需要工资50两
    if (!user.packs.checkSalary(gold) || !user.packs.removeSalary(gold,"装备神圣祝福") ) 
    {
        Base::reply(user, 1,EQUIP_COMPOSE11, "工资不足50两");
        return false;
    }

    if ((chkStn.eqpObj->data.kind & 4 || chkStn.eqpObj->data.upgrade >= 15) || !chkStn.eqpObj->data.kind & 16)
	{
		
	     EquipMaker maker(NULL);
	     base = objectbm.get(chkStn.eqpObj->data.dwObjectID);
		//Zebra::logger->debug("装备转生:%d 物品名:%s",chkStn.eqpObj->data.round,chkStn.eqpObj->name);
	     zObject* ob = zObject::create(base,1, 0); //soke base ,品质（颜色）,星星

		 if (ob)
		 {
			//祝福保留魂魄石和孔
			ob->data.round = chkStn.eqpObj->data.round;
            memcpy(ob->data.socket, chkStn.eqpObj->data.socket, sizeof(chkStn.eqpObj->data.socket));
	    	ob->data.xstr = chkStn.eqpObj->data.xstr;
	     	ob->data.xinte = chkStn.eqpObj->data.xinte;
	     	ob->data.xdex = chkStn.eqpObj->data.xdex;
		    ob->data.xspi = chkStn.eqpObj->data.xspi;
		    ob->data.xcon = chkStn.eqpObj->data.xcon;
			//祝福保留补天
	        if (chkStn.eqpObj->data.butian > 0)
	        {
		      int i = chkStn.eqpObj->data.butian;
		       while (i > 0)
		       {
				i--;
				Upgrade::bt_upgrade(user,ob,100000);
	           }
	        }
			
			if(chkStn.stone->data.dwNum > 1)
			{
				chkStn.stone->data.dwNum -= 1;
				Cmd::stRefCountObjectPropertyUserCmd send;
				send.qwThisID = chkStn.stone->data.qwThisID;
				send.dwNum = chkStn.stone->data.dwNum;
				user.sendCmdToMe(&send, sizeof(send));
				user.packs.removeObject(chkStn.eqpObj);
			}
			else
			{
				user.packs.execEvery(pack, Type2Type<ClearPack>());
			}

			maker.assign1(NULL, ob, base,false,22);//soke 16 品质
			ob->data.bind=1;
			strncpy(ob->data.maker, user.charbase.name, MAX_NAMESIZE);//打造者名字
			ob->data.pos = stObjectLocation(pack->type(), pack->id(), 0, Cmd::MAKECELLTYPE_EQUIP);

			if (user.packs.addObject(ob, false)) 
			{
				Base::add_object(user, ob, false);
				Base::reply(user, 0, EQUIP_COMPOSE11,"装备祝福成功");
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备祝福改造成功，花费工资50两");
			}
		}
	}

    return true;
}*/
//装备祝福
bool RebuildObject::equipCompose11(SceneUser &user, zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	RecastPack* pack = &(user.packs.recastPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备祝福的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct CheckSoulStone : public PackageCallback
	{
	public:
		CheckSoulStone() 
			: stone(NULL), eqpObj(NULL), canBind(true)
		{}

		bool exec(zObject* ob)
		{
			if ( 1317 == ob->data.dwObjectID ) 
			{
				if (!stone) 
				{
					stone = ob;
				}
				else 
				{
					canBind = false;
					return false;
				}
			}
		    //soke 能祝福的条件 大于等于120级、绑定的绿装15星
			else if (ob->data.needlevel >= 120 || ob->data.bind || !ob->data.kind & 16 || (ob->data.kind & 4 || ob->data.upgrade >= 15) )
            {
				if (!eqpObj) 
				{
					eqpObj = ob;
				}
				else 
				{
					canBind = false;
					return false;
				}	
			}
			else 
			{
				canBind = false;
				return false;
			}
			return true;	
		}

	public:
		zObject* stone;
		zObject* eqpObj;
		bool canBind;
	};

	CheckSoulStone chkStn;
	pack->execEvery(chkStn);

    //soke 条件判断
     if (!chkStn.stone || !chkStn.eqpObj || chkStn.eqpObj->data.needlevel < 160 
		 || !chkStn.eqpObj->data.bind || !chkStn.eqpObj->data.kind & 4 || chkStn.eqpObj->data.upgrade < 15 )  
    {
        Base::reply(user, 1, EQUIP_COMPOSE11, "装备祝福");
        Zebra::logger->alarm("用户(%d:%d:%s)请求对不能对该装备神圣祝福", user.charbase.accid, user.charbase.id, user.charbase.name);
      	Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备神圣祝福失败，要祝福的装备必须是160级以上绑定的15星绿装");
		return false;			
    }

	
	DWORD need = 50000; //soke 需要5锭
	if (!user.packs.checkMoney(need))
	{
		Base::reply(user, 1, EQUIP_COMPOSE11, "银子不足5锭");
		return false;
	}

	if (user.packs.removeMoney(need, "装备神圣祝福"))
	{
	    if ((chkStn.eqpObj->data.kind & 4 || chkStn.eqpObj->data.upgrade >= 15) || !chkStn.eqpObj->data.kind & 16)
	    {
	        EquipMaker maker(NULL);

	        base = objectbm.get(chkStn.eqpObj->data.dwObjectID);
		
	        zObject* new_ob = zObject::create(base);
		    if (new_ob)
		    {
			    new_ob->data.round = chkStn.eqpObj->data.round;
			    memcpy(new_ob->data.socket, chkStn.eqpObj->data.socket, sizeof(chkStn.eqpObj->data.socket));
	    	    new_ob->data.xstr = chkStn.eqpObj->data.xstr;
	     	    new_ob->data.xinte = chkStn.eqpObj->data.xinte;
	     	    new_ob->data.xdex = chkStn.eqpObj->data.xdex;
		        new_ob->data.xspi = chkStn.eqpObj->data.xspi;
		        new_ob->data.xcon = chkStn.eqpObj->data.xcon;
			   //补天
	            if (chkStn.eqpObj->data.butian > 0)
	            {
		            int i = chkStn.eqpObj->data.butian;
		            while (i > 0)
		            {
				        i--;
				        Upgrade::bt_upgrade(user,new_ob,100000);
	                }
	             }
                maker.assign1(NULL, new_ob, base,false,22);//soke 16 品质
			    new_ob->data.bind=1;
			    strncpy(new_ob->data.maker, user.charbase.name, MAX_NAMESIZE);//打造者名字	
            }
    	    zObject* ob = zObject::create(new_ob);
		    struct MultiItemDelCB : public PackageCallback
		    {
			    MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL), realDelCnt(0), pOwner(pUser)
			    { }

			    bool exec(zObject* ob)
			    {
				  if (ob->data.dwObjectID == 1317)
				  {
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if (pOwner && realDelCnt < MAX_DEL_COUNT)
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if (delObj->data.dwNum > needDelNum)
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				  }
				  else
				  {
					    oldPos = ob->data.pos;
					    pOwner->packs.removeObject(ob);
				  }
				  return true;
			    }
			    zObject* up_ob;
			    zObject* delObj;
			    DWORD  realDelCnt; //实际删除的数量
			    SceneUser* pOwner;
			    stObjectLocation oldPos;
		    };

		    MultiItemDelCB del(&user);
		    pack->execEvery(del);

		    if (ob) 
		    {	
			    ob->data.pos = del.oldPos;
			    if (user.packs.addObject(ob, false)) 
			    {
					EquipMaker modify(&user);
					//装备祝福完给他改造一下 妈的 by醉梦
					modify.modify_attri16(ob,base);
				    Base::add_object(user, ob, false);
				    Base::reply(user, 0, EQUIP_COMPOSE11,"装备神圣祝福成功");
			    }
		    }
		    return true;
	    }
		return false;	  
    }
	return false;
} 
//soke 斗魂升级
bool RebuildObject::equipCompose12( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
   RecastPack* pack = &(user.packs.recastPack);
    if (!pack)
    {
        Zebra::logger->alarm("用户(%d:%d:%s)请求升级斗魂的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
        return false;
    }

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}
				
				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 10*(num-1);
		}
		
		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;
		
		bool can;

		bool is_stuff()
		{
			zUdouhunObjectB *base = udouhunobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.douhun+1));
			if(!base) return false;
	
			return c_ob->data.dwObjectID == base->stuff;
		}
		
		bool can_compose()
		{
		   //soke 条件判断
		   if (up_ob->data.kind & 16)
		   {
		      return count>=1 && up_ob && is_stuff() && can;	
		   }
		   else
		      return false;
		}
	};
	
	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if (dc.up_ob->data.douhun >= 13) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级的装备斗魂已经大成", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE12, "          该装备斗魂已经大成");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级斗魂物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE12, "          该物品不能升级斗魂");
			return false;			
		}
	}

	DWORD need = 0;

	zUdouhunObjectB *uob = udouhunobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.douhun+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级斗魂");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE12, "               银子不足");
		return false;
	}
	
	if ( user.packs.removeMoney(need,"斗魂升级") ) 
	{	
		bool ret = Upgrade::dh_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
        {
            MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
            { }

            bool exec(zObject* ob)
            {
                if ( 3809 == ob->data.dwObjectID )
                {
                    delObj = ob;
                    static const DWORD MAX_DEL_COUNT = 1;
                    if ( pOwner && realDelCnt < MAX_DEL_COUNT )
                    {
                        DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

                        DWORD realDelNum = 0;
                        if ( delObj->data.dwNum > needDelNum )
                        {
                            realDelNum = needDelNum;
                            delObj->data.dwNum -= realDelNum;

                            Cmd::stRefCountObjectPropertyUserCmd send;
                            send.qwThisID = delObj->data.qwThisID;
                            send.dwNum = delObj->data.dwNum;
                            pOwner->sendCmdToMe(&send, sizeof(send));
                        }
                        else
                        {
                            realDelNum = delObj->data.dwNum;
                            zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"装备斗魂升级删除",delObj->base,delObj->data.kind,delObj->data.douhun);
                            pOwner->packs.removeObject(delObj);
                        }
                        realDelCnt += realDelNum;
                    }
                    else
                    {
                        //已经扣除四颗，不用扣除了
                        return true;
                    }
                }
                else
                {
                    oldPos = ob->data.pos;       
                    pOwner->packs.removeObject(ob);
                }
                return true;	
            }

            zObject* up_ob;
            zObject* delObj;
            DWORD  realDelCnt; //实际删除的数量
            SceneUser* pOwner;
            stObjectLocation oldPos;
        };

        MultiItemDelCB del(&user);
        pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;
			
			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"斗魂升级生成",ob->base,ob->data.kind,ob->data.douhun);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE12, ret?"             斗魂升级成功":"             斗魂升级失败");
			}
		}
		return true;
	}
	return false;
}

//soke 补天镶嵌
bool RebuildObject::equipCompose13( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	RecastPack* pack = &(user.packs.recastPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求镶嵌补天的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUbutianObjectB *base = ubutianobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.butian+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{
			//soke 条件判断
			if (up_ob->data.kind & 4 || up_ob->data.kind & 16)
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);

	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级补天时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if (dc.up_ob->data.butian >= 5) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求镶嵌补天的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE13, "          该装备补天等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可镶嵌补天物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE13, "          该物品不能镶嵌补天");
			return false;			
		}
	}

	DWORD need = 0;

	zUbutianObjectB *uob = ubutianobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.butian+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可镶嵌补天");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE13, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"补天镶嵌") ) 
	{
		bool ret = Upgrade::bt_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( (ob->data.dwObjectID >= 52000 && ob->data.dwObjectID <= 52034)||(ob->data.dwObjectID >= 52040 && ob->data.dwObjectID <= 52044)
					||(ob->data.dwObjectID >= 52050 && ob->data.dwObjectID <= 52069)||(ob->data.dwObjectID >= 52075 && ob->data.dwObjectID <= 52089) )
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"补天镶嵌删除",delObj->base,delObj->data.kind,delObj->data.butian);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"补天镶嵌生成",ob->base,ob->data.kind,ob->data.butian);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE13, ret?"             补天镶嵌成功":"             补天镶嵌失败");
			}
		}
		return true;
	}
	return false;
}

//soke 装备栏激活
bool RebuildObject::equipCompose14(SceneUser &user, zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	RecastPack* pack = &(user.packs.recastPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备栏激活的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct CheckSoulStone : public PackageCallback
	{
	public:
		CheckSoulStone() 
			: stone(NULL), eqpObj(NULL), canBind(true)
		{}

		bool exec(zObject* ob)
		{
			if ( 5310 == ob->data.dwObjectID ) 
			{
				if (!stone) 
				{
					stone = ob;
				}
				else 
				{
					canBind = false;
					return false;
				}
			}
			//soke 条件判断
			else if (ob->data.needlevel >= 70 || ob->data.bind || ((ob->data.kind & 16) || (ob->data.kind & 4))  )
			{
				if (!eqpObj) 
				{
					eqpObj = ob;
				}
				else 
				{
					canBind = false;
					return false;
				}	
			}
			else 
			{
				canBind = false;
				return false;
			}
			return true;	
		}

	public:
		zObject* stone;
		zObject* eqpObj;
		bool canBind;
	};

	CheckSoulStone chkStn;
	pack->execEvery(chkStn);

	//sky 限制条件
	if (!chkStn.stone || !chkStn.eqpObj || chkStn.eqpObj->data.needlevel < 70 || !chkStn.eqpObj->data.bind || !(chkStn.eqpObj->data.kind & 16 || chkStn.eqpObj->data.kind & 4)  ) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE14, "装备栏激活失败");
		Zebra::logger->alarm("用户(%d:%d:%s)请求对不能对该装备栏激活", user.charbase.accid, user.charbase.id, user.charbase.name);
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "70级以上绑定装备可以激活装备栏");
		return false;
	}

	if (chkStn.eqpObj->data.retain2 > 0)
	{
		Base::reply(user, 1, EQUIP_COMPOSE14, "          当前装备栏已经激活");
		return false;
	}

	DWORD need = 50000; //soke 需要5锭
	if (!user.packs.checkMoney(need))
	{
		Base::reply(user, 1, EQUIP_COMPOSE14, "               银子不足");
		return false;
	}

	if (user.packs.removeMoney(need, "装备栏激活"))
	{
		if (chkStn.eqpObj->data.retain2 < 1)
		{
			++chkStn.eqpObj->data.retain2;
		}

		zObject* ob = zObject::create(chkStn.eqpObj);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL), realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if (ob->data.dwObjectID == 5310)
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if (pOwner && realDelCnt < MAX_DEL_COUNT)
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if (delObj->data.dwNum > needDelNum)
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;
					pOwner->packs.removeObject(ob);
				}
				return true;
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);

		if (ob) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{
				Base::add_object(user, ob, false);
				Base::reply(user, 0, EQUIP_COMPOSE14,"装备栏激活成功");
			}
		}
		return true;
	}
	return false;
}

//soke 装备栏进阶
bool RebuildObject::equipCompose15(SceneUser &user, zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	RecastPack* pack = &(user.packs.recastPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备栏进阶的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct CheckSoulStone : public PackageCallback
	{
	public:
		CheckSoulStone() 
			: stone(NULL), eqpObj(NULL), canBind(true)
		{}

		bool exec(zObject* ob)
		{
			if ( 5311 == ob->data.dwObjectID ) 
			{
				if (!stone) 
				{
					stone = ob;
				}
				else 
				{
					canBind = false;
					return false;
				}
			}
			//soke 条件判断
			else if (ob->data.needlevel >= 70 || ob->data.bind || ((ob->data.kind & 16) || (ob->data.kind & 4))  )
			{
				if (!eqpObj) 
				{
					eqpObj = ob;
				}
				else 
				{
					canBind = false;
					return false;
				}	
			}
			else 
			{
				canBind = false;
				return false;
			}
			return true;	
		}

	public:
		zObject* stone;
		zObject* eqpObj;
		bool canBind;
	};

	CheckSoulStone chkStn;
	pack->execEvery(chkStn);

	if (!chkStn.stone || !chkStn.eqpObj ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级装备栏时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}

	if (chkStn.eqpObj->data.retain2 < 1)
	{
		Base::reply(user, 1, EQUIP_COMPOSE15, "          请先激活装备栏");
		return false;
	}

	if (chkStn.eqpObj->data.retain2 >= 6)
	{
		Base::reply(user, 1, EQUIP_COMPOSE15, "          当前装备栏等级已经最高");
		return false;
	}

	DWORD need = 50000; //soke 需要5锭
	if (!user.packs.checkMoney(need))
	{
		Base::reply(user, 1, EQUIP_COMPOSE15, "               银子不足");
		return false;
	}

	if (user.packs.removeMoney(need, "装备栏进阶"))
	{
		if (chkStn.eqpObj->data.retain2 < 6)
		{
			++chkStn.eqpObj->data.retain2;
		}

		zObject* ob = zObject::create(chkStn.eqpObj);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL), realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if (ob->data.dwObjectID == 5311)
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if (pOwner && realDelCnt < MAX_DEL_COUNT)
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if (delObj->data.dwNum > needDelNum)
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;
					pOwner->packs.removeObject(ob);
				}
				return true;
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);

		if (ob) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{
				Base::add_object(user, ob, false);
				Base::reply(user, 0, EQUIP_COMPOSE15,"装备栏进阶成功");
			}
		}
		return true;
	}
	return false;
}

//soke 神器进阶
bool RebuildObject::equipCompose16( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	RecastPack* pack = &(user.packs.recastPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求神器进阶的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUshenqiObjectB *base = ushenqiobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.retain3+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if (up_ob->data.kind & 16)
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);

	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级神器时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}

	if ( dc.up_ob->data.retain3 >= 7) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求神器进阶的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE16, "          神器进阶等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可神器进阶物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE16, "          该物品不能神器进阶");
			return false;			
		}
	}

	DWORD need = 0;

	zUshenqiObjectB *uob = ushenqiobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.retain3+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级神器进阶");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE16, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"神器进阶升级") ) 
	{
		bool ret = Upgrade::shenqi_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 3812 )
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"神器进阶删除",delObj->base,delObj->data.kind,delObj->data.retain3);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"神器进阶生成",ob->base,ob->data.kind,ob->data.retain3);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE16, ret?"             神器进阶成功":"             神器进阶失败");
			}
		}
		return true;
	}
	return false;
}

//soke 龙槽激活
bool RebuildObject::equipCompose17(SceneUser &user, zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev)
{
	RecastPack* pack = &(user.packs.recastPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求龙槽激活的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast)
			{
				up_ob = ob;
			}
			else
			{
				if (!c_ob)
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID)
				{
					count += ob->data.dwNum;
					return true;
				}

				can = false;
				return false;
			}
			return true;
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 10 * (num - 1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUdtroughObjectB *base = udtroughobjectbm.get(up_ob->data.dwObjectID + 100000 * (up_ob->data.dtrough + 1));
			if (!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{
			//sky 条件判断
		    if (up_ob->data.kind & 16 && up_ob->data.upgrade >= 15 && up_ob->data.needlevel >= 160)
		    {
			   return count >= 1 && up_ob && is_stuff() && can;
		    }
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);

	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)激活龙槽时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}

	if (dc.up_ob->data.dtrough >= 1)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求的装备龙槽激活的等级最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE17, "          该装备龙槽已经激活");
		return false;
	}
	else
	{
		if (!dc.can_compose())
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可激活龙槽物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE17, "          该物品不能激活龙槽");
			return false;
		}
	}

	DWORD need = 0;

	zUdtroughObjectB *uob = udtroughobjectbm.get(dc.up_ob->data.dwObjectID + 100000 * (dc.up_ob->data.dtrough + 1));
	if (uob)
	{
		need = uob->gold;
	}
	else
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不能激活龙槽");
		return false;
	}

	if (!user.packs.checkMoney(need))
	{
		Base::reply(user, 1, EQUIP_COMPOSE17, "               银子不足");
		return false;
	}

	if (user.packs.removeMoney(need, "龙槽激活"))
	{
		bool ret = Upgrade::dtrough_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL), realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if (ob->data.dwObjectID == 1140)
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if (pOwner && realDelCnt < MAX_DEL_COUNT)
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if (delObj->data.dwNum > needDelNum)
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid, delObj->data.qwThisID, delObj->data.strName, delObj->data.dwNum, delObj->data.dwNum, 0, pOwner->id, pOwner->name, 0, NULL, "装备龙槽激活删除", delObj->base, delObj->data.kind, delObj->data.dtrough);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;
					pOwner->packs.removeObject(ob);
				}
				return true;
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob)
		{
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false))
			{
				zObject::logger(ob->createid, ob->data.qwThisID, ob->data.strName, ob->data.dwNum, ob->data.dwNum, 1, 0, NULL, user.id, user.name, "龙槽激活生成", ob->base, ob->data.kind, ob->data.dtrough);
				Base::add_object(user, ob, false);
				Base::reply(user, ret ? 0 : 1, EQUIP_COMPOSE17, ret ? "             龙槽激活成功" : "             龙槽激活失败");
			}
		}
		return true;
	}
	return false;
}

//soke 龙槽解封
bool RebuildObject::equipCompose18(SceneUser &user, zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev)
{
	RecastPack* pack = &(user.packs.recastPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求龙槽解封的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast)
			{
				up_ob = ob;
			}
			else
			{
				if (!c_ob)
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID)
				{
					count += ob->data.dwNum;
					return true;
				}

				can = false;
				return false;
			}
			return true;
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 10 * (num - 1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUdtroughObjectB *base = udtroughobjectbm.get(up_ob->data.dwObjectID + 100000 * (up_ob->data.dtrough + 1));
			if (!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{
			//sky 条件判断 
			if (up_ob->data.kind & 16 && up_ob->data.upgrade >= 15 && up_ob->data.needlevel >= 160 && up_ob->data.dtrough >= 1)
		   {
			   return count >= 1 && up_ob && is_stuff() && can;
		   }
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);

	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)龙槽解封时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}

	if (dc.up_ob->data.dtrough >= 15)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求的装备龙槽解封的等级最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE18, "          该装备龙槽已经达到最高等级");
		return false;
	}
	else
	{
		if (!dc.can_compose())
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可解封龙槽物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE18, "          该物品不能解封龙槽");
			return false;
		}
	}

	DWORD need = 0;

	zUdtroughObjectB *uob = udtroughobjectbm.get(dc.up_ob->data.dwObjectID + 100000 * (dc.up_ob->data.dtrough + 1));
	if (uob)
	{
		need = uob->gold;
	}
	else
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不能解封龙槽");
		return false;
	}

	if (!user.packs.checkMoney(need))
	{
		Base::reply(user, 1, EQUIP_COMPOSE18, "               银子不足");
		return false;
	}

	if (user.packs.removeMoney(need, "龙槽解封"))
	{
		bool ret = Upgrade::dtrough_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL), realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if (ob->data.dwObjectID == 1141 ) //龙槽解封道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if (pOwner && realDelCnt < MAX_DEL_COUNT)
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if (delObj->data.dwNum > needDelNum)
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid, delObj->data.qwThisID, delObj->data.strName, delObj->data.dwNum, delObj->data.dwNum, 0, pOwner->id, pOwner->name, 0, NULL, "装备龙槽解封删除", delObj->base, delObj->data.kind, delObj->data.dtrough);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;
					pOwner->packs.removeObject(ob);
				}
				return true;
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob)
		{
			ob->data.pos = del.oldPos; //sky 解决装备不在中间吃装备BUG
			if (user.packs.addObject(ob, false))
			{
				zObject::logger(ob->createid, ob->data.qwThisID, ob->data.strName, ob->data.dwNum, ob->data.dwNum, 1, 0, NULL, user.id, user.name, "龙槽解封生成", ob->base, ob->data.kind, ob->data.dtrough);
				Base::add_object(user, ob, false);
				Base::reply(user, ret ? 0 : 1, EQUIP_COMPOSE18, ret ? "             龙槽解封成功" : "             龙槽解封失败");
			}
		}
		return true;
	}
	return false;
}

//soke 龙星提升
bool RebuildObject::equipCompose19( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
   RecastPack* pack = &(user.packs.recastPack);
    if (!pack)
    {
        Zebra::logger->alarm("用户(%d:%d:%s)请求升级龙星的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
        return false;
    }

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}
				
				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}
		
		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;
		
		bool can;

		bool is_stuff()
		{
			zUdrastarObjectB *base = udrastarobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.drastar+1));
			if(!base) return false;
	
			return c_ob->data.dwObjectID == base->stuff;
		}
		
		bool can_compose()
		{
		   //sky 紫装才能升级龙星
		   if (up_ob->data.kind & 16 && (up_ob->data.drastar < up_ob->data.dtrough))
		   {
		      return count>=1 && up_ob && is_stuff() && can;	
		   }
		   else
		      return false; //sky 不是紫装升级失败
		}
	};
	
	do_compose dc;
	pack->execEvery(dc);

	
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级龙星时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}

	if (dc.up_ob->data.drastar >= 15) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级的装备龙星已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE19, "          该装备龙星已经达到最高等级");
		return false;			
	}

	/*else if (dc.up_ob->data.drastar >= dc.up_ob->data.dtrough) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级的装备龙星已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		MoBase::reply(user, 1, EQUIP_COMPOSE19, "          该装备龙星已经达到最高等级");
		return false;			
	}*/
	else
	{
		if (!dc.can_compose()  && !(dc.c_ob->data.dwObjectID == 2975)) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级龙星物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE19, "          该物品不能升级龙星");
			return false;			
		}
	}

	if (dc.c_ob->data.dwObjectID == 2975)
	{
		for(int i=0;i<15;i++)
		{
			Upgrade::drastar_upgrade(user, dc.up_ob, 100);
		}
		
		zObject* ob = zObject::create(dc.up_ob);
		
		struct MultiItemDelCB : public PackageCallback
        {
            MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
            { }

            bool exec(zObject* ob)
            {
                if ( 2975 == ob->data.dwObjectID)
                {
                    delObj = ob;
                    static const DWORD MAX_DEL_COUNT = 1;
                    if ( pOwner && realDelCnt < MAX_DEL_COUNT )
                    {
                        DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

                        DWORD realDelNum = 0;
                        if ( delObj->data.dwNum > 1 )
                        {
                            realDelNum = 1;
                            delObj->data.dwNum -= 1;

                            Cmd::stRefCountObjectPropertyUserCmd send;
                            send.qwThisID = delObj->data.qwThisID;
                            send.dwNum = delObj->data.dwNum;
                            pOwner->sendCmdToMe(&send, sizeof(send));
                        }
                        else
                        {
                            realDelNum = delObj->data.dwNum;
                            zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"装备龙星升级删除",delObj->base,delObj->data.kind,delObj->data.drastar);
                            pOwner->packs.removeObject(delObj);
                        }
                        realDelCnt += realDelNum;
                    }
                    else
                    {
                        //已经扣除四颗，不用扣除了
                        return true;
                    }
                }
                else
                {
                    oldPos = ob->data.pos;       
                    pOwner->packs.removeObject(ob);
                }
                return true;	
            }

            zObject* up_ob;
            zObject* delObj;
            DWORD  realDelCnt; //实际删除的数量
            SceneUser* pOwner;
            stObjectLocation oldPos;
        };

        MultiItemDelCB del(&user);
        pack->execEvery(del);
		if (ob)
		{
			ob->data.pos = del.oldPos;
			if (user.packs.addObject(ob, false))
			{
				zObject::logger(ob->createid, ob->data.qwThisID, ob->data.strName, ob->data.dwNum, ob->data.dwNum, 1, 0, NULL, user.id, user.name, "龙星升级生成", ob->base, ob->data.kind, ob->data.drastar);
				Base::add_object(user, ob, false);
				Base::reply(user,  0, EQUIP_COMPOSE19, "             龙星升级成功");
			}
		}
		return true;
	}

	DWORD need = 0;

	zUdrastarObjectB *uob = udrastarobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.drastar+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级龙星");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE19, "               银子不足");
		return false;
	}
	
	if ( user.packs.removeMoney(need,"龙星升级") ) 
	{
		//Base::refresh_pack(user, pack);
		
		//算法
		bool ret;
		if(dc.up_ob->data.retain25 ==900)
		{
			dc.up_ob->data.retain26 = 900+zMisc::randBetween(0,500);
		}
		
		if(dc.up_ob->data.retain25 >= dc.up_ob->data.retain26 && dc.up_ob->data.retain25 >=900)
		{
			ret = Upgrade::drastar_upgrade(user, dc.up_ob,80);
		}
		else{
			if(dc.up_ob->data.drastar<=10)
			{
				ret = Upgrade::drastar_upgrade(user, dc.up_ob, 40);
			}
			else{
				if(dc.up_ob->data.drastar==11)
				{
					if(dc.up_ob->data.retain25>400)
					{
						ret = Upgrade::drastar_upgrade(user, dc.up_ob, zMisc::randBetween(20,40));
					}
					else{
						ret = Upgrade::drastar_upgrade(user, dc.up_ob, zMisc::randBetween(0,5));
					}
				}
				if(dc.up_ob->data.drastar==12)
				{
					if(dc.up_ob->data.retain25>550)
					{
						ret = Upgrade::drastar_upgrade(user, dc.up_ob, zMisc::randBetween(20,40));
					}
					else{
						ret = Upgrade::drastar_upgrade(user, dc.up_ob, zMisc::randBetween(0,5));
					}
				}
				if(dc.up_ob->data.drastar==13)
				{
					if(dc.up_ob->data.retain25>600)
					{
						ret = Upgrade::drastar_upgrade(user, dc.up_ob, zMisc::randBetween(20,40));
					}
					else{
						ret = Upgrade::drastar_upgrade(user, dc.up_ob, zMisc::randBetween(0,5));
					}
				}
				if(dc.up_ob->data.drastar==14)
				{
					if(dc.up_ob->data.retain25>900)
					{
						ret = Upgrade::drastar_upgrade(user, dc.up_ob, zMisc::randBetween(0,5));
					}
					else{
						ret = Upgrade::drastar_upgrade(user, dc.up_ob, 0);
					}
				}
			}
			
		}
		

		zObject* ob = zObject::create(dc.up_ob);
	//  user.packs.execEvery(pack, Type2Type<ClearPack>());
		struct MultiItemDelCB : public PackageCallback
        {
            MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
            { }

            bool exec(zObject* ob)
            {
                if ( 1153 == ob->data.dwObjectID/* || 681 == ob->data.dwObjectID*/ )
                {
                    delObj = ob;
                    static const DWORD MAX_DEL_COUNT = 1;
                    if ( pOwner && realDelCnt < MAX_DEL_COUNT )
                    {
                        DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

                        DWORD realDelNum = 0;
                        if ( delObj->data.dwNum > needDelNum )
                        {
                            realDelNum = needDelNum;
                            delObj->data.dwNum -= realDelNum;

                            Cmd::stRefCountObjectPropertyUserCmd send;
                            send.qwThisID = delObj->data.qwThisID;
                            send.dwNum = delObj->data.dwNum;
                            pOwner->sendCmdToMe(&send, sizeof(send));
                        }
                        else
                        {
                            realDelNum = delObj->data.dwNum;
                            zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"装备龙星升级删除",delObj->base,delObj->data.kind,delObj->data.drastar);
                            pOwner->packs.removeObject(delObj);
                        }
                        realDelCnt += realDelNum;
                    }
                    else
                    {
                        //已经扣除四颗，不用扣除了
                        return true;
                    }
                }
                else
                {
                    oldPos = ob->data.pos;       
                    pOwner->packs.removeObject(ob);
                }
                return true;	
            }

            zObject* up_ob;
            zObject* delObj;
            DWORD  realDelCnt; //实际删除的数量
            SceneUser* pOwner;
            stObjectLocation oldPos;
        };

        MultiItemDelCB del(&user);
        pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.retain25 +=1;
			ob->data.pos = del.oldPos;
			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"龙星升级生成",ob->base,ob->data.kind,ob->data.drastar);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE19, ret?"             龙星升级成功":"             龙星升级失败");
			}
		}
		return true;
	}
	return false;
}

//soke 魂魄清除
bool RebuildObject::equipCompose20(SceneUser &user, zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	RecastPack* pack = &(user.packs.recastPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求魂魄清除的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct CheckSoulStone : public PackageCallback
	{
	public:
		CheckSoulStone() 
			: stone(NULL), eqpObj(NULL), canBind(true)
		{}

		bool exec(zObject* ob)
		{
			if ( 2329 == ob->data.dwObjectID ) //魂魄清除材料
			{
				if (!stone) 
				{
					stone = ob;
				}
				else 
				{
					canBind = false;
					return false;
				}
			}
			//soke 条件判断
			else if (ob->data.needlevel >= 70 || ob->data.bind || ((ob->data.kind & 16) || (ob->data.kind & 4))  )
			{
				if (!eqpObj) 
				{
					eqpObj = ob;
				}
				else 
				{
					canBind = false;
					return false;
				}	
			}
			else 
			{
				canBind = false;
				return false;
			}
			return true;	
		}

	public:
		zObject* stone;
		zObject* eqpObj;
		bool canBind;
	};

	CheckSoulStone chkStn;
	pack->execEvery(chkStn);

	if (!chkStn.stone || !chkStn.eqpObj ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)魂魄清除时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}

	/*if (chkStn.eqpObj->data.retain2 < 1)
	{
		Base::reply(user, 1, EQUIP_COMPOSE20, "          请先激活装备栏");
		return false;
	}*/

	DWORD need = 50000; //soke 需要5锭
	if (!user.packs.checkMoney(need))
	{
		Base::reply(user, 1, EQUIP_COMPOSE20, "               银子不足");
		return false;
	}

	if (user.packs.removeMoney(need, "魂魄清除"))
	{

		for (int i=0;i<6;i++)
		{
			if (chkStn.eqpObj->data.socket[i]!=(DWORD)0)
			{
				chkStn.eqpObj->data.socket[i] =(DWORD)0;
				//memset(chkStn.eqpObj->data.socket,0,sizeof(chkStn.eqpObj->data.socket));

				chkStn.eqpObj->data.hpleech.odds = 0;
				chkStn.eqpObj->data.hpleech.effect = 0;
				chkStn.eqpObj->data.mpleech.odds = 0;
				chkStn.eqpObj->data.mpleech.effect = 0;
				chkStn.eqpObj->data.hptomp = 0; 
				chkStn.eqpObj->data.incgold = 0; 
				chkStn.eqpObj->data.doublexp = 0; 
				chkStn.eqpObj->data.mf = 0; 

				chkStn.eqpObj->data.poisondef = 0;
				chkStn.eqpObj->data.lulldef = 0;
				chkStn.eqpObj->data.reeldef = 0;
				chkStn.eqpObj->data.evildef = 0;
				chkStn.eqpObj->data.bitedef = 0;
				chkStn.eqpObj->data.chaosdef = 0;
				chkStn.eqpObj->data.colddef = 0;
				chkStn.eqpObj->data.petrifydef = 0;
				chkStn.eqpObj->data.blinddef = 0;
				chkStn.eqpObj->data.stabledef = 0;
				chkStn.eqpObj->data.slowdef = 0;
				chkStn.eqpObj->data.luredef = 0;

				chkStn.eqpObj->data.poison = 0;
				chkStn.eqpObj->data.lull = 0;
				chkStn.eqpObj->data.reel = 0;
				chkStn.eqpObj->data.evil = 0;
				chkStn.eqpObj->data.bite = 0;
				chkStn.eqpObj->data.chaos = 0;
				chkStn.eqpObj->data.cold = 0;
				chkStn.eqpObj->data.petrify = 0;
				chkStn.eqpObj->data.blind = 0;
				chkStn.eqpObj->data.stable = 0;
				chkStn.eqpObj->data.slow = 0;
				chkStn.eqpObj->data.lure = 0;
				chkStn.eqpObj->data.xstr = 0;
				chkStn.eqpObj->data.xinte = 0;
				chkStn.eqpObj->data.xdex = 0;
				chkStn.eqpObj->data.xspi = 0;
				chkStn.eqpObj->data.xcon = 0;
			}
			

		}

		zObject* ob = zObject::create(chkStn.eqpObj);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL), realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if (ob->data.dwObjectID == 2329) //魂魄清除材料
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if (pOwner && realDelCnt < MAX_DEL_COUNT)
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if (delObj->data.dwNum > needDelNum)
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;
					pOwner->packs.removeObject(ob);
				}
				return true;
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);

		if (ob) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{
				Base::add_object(user, ob, false);
				Base::reply(user, 0, EQUIP_COMPOSE20,"魂魄清除成功");

			}
		}
		return true;
	}
	return false;
}

//soke 装备升级
bool RebuildObject::equipCompose21(SceneUser &user, zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	RecastPack* pack = &(user.packs.recastPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备升级的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zEquipObjectB *base = equipobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.retain7+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{
			//soke 条件判断
			if (up_ob->data.kind & 4 || up_ob->data.kind & 16)
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if (dc.up_ob->data.needlevel >= 202) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备升级的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE21, "          该装备等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求不可升级物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			//Base::reply(user, 1, EQUIP_COMPOSE21, "          该物品不能升级");
			//return false;

		    //soke 70级以上装备升级 - 甲(男)
			if (dc.up_ob->data.dwObjectID == 10007)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10008)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10009)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10010)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10011)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10012)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10013)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10014)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10015)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10016)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10017)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10018)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10019)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10020)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10021)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10022)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10023)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10024)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10025)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10026)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10027)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10028)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10029)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 甲(女)
			if (dc.up_ob->data.dwObjectID == 10057)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10058)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10059)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10060)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10061)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10062)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10063)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10064)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10065)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10066)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10067)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10068)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10069)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10070)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10071)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10072)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10073)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10074)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10075)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10076)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10077)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10078)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10079)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}										

            //soke 70级以上装备升级 - 铠(男)
			if (dc.up_ob->data.dwObjectID == 10107)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10108)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10109)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10110)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10111)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10112)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10113)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10114)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10115)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10116)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10117)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10118)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10119)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10120)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10121)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10122)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10123)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10124)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10125)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10126)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10127)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10128)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10129)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}										

            //soke 70级以上装备升级 - 铠(女)
			if (dc.up_ob->data.dwObjectID == 10157)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10158)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10159)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10160)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10161)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10162)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10163)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10164)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10165)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10166)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10167)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10168)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10169)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10170)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10171)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10172)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10173)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10174)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10175)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10176)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10177)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10178)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10179)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}							

              //soke 70级以上装备升级 - 刀
			if (dc.up_ob->data.dwObjectID == 10207)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10208)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10209)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10210)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10211)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10212)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10213)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10214)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10215)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10216)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10217)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10218)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10219)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10220)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10221)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10222)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10223)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10224)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10225)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10226)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10227)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10228)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10229)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 仗
			if (dc.up_ob->data.dwObjectID == 10257)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10258)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10259)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10260)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10261)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10262)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10263)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10264)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10265)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10266)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10267)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10268)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10269)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10270)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10271)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10272)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10273)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10274)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10275)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10276)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10277)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10278)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10279)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 弓
			if (dc.up_ob->data.dwObjectID == 10307)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10308)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10309)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10310)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10311)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10312)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10313)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10314)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10315)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10316)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10317)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10318)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10319)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10320)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10321)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10322)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10323)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10324)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10325)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10326)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10327)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10328)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10329)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 扇
			if (dc.up_ob->data.dwObjectID == 10357)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10358)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10359)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10360)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10361)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10362)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10363)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10364)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10365)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10366)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10367)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10368)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10369)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10370)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10371)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10372)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10373)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10374)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10375)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10376)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10377)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10378)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10379)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 棍
			if (dc.up_ob->data.dwObjectID == 10407)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10408)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10409)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10410)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10411)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10412)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10413)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10414)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10415)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10416)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10417)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10418)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10419)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10420)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10421)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10422)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10423)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10424)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10425)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10426)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10427)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10428)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10429)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 剑(物)
			if (dc.up_ob->data.dwObjectID == 10457)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10458)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10459)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10460)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10461)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10462)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10463)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10464)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10465)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10466)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10467)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10468)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10469)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10470)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10471)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10472)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10473)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10474)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10475)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10476)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10477)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10478)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10479)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}	
			
			//soke 70级以上装备升级 - 剑(魔)
			if (dc.up_ob->data.dwObjectID == 10507)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10508)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10509)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10510)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10511)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10512)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10513)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10514)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10515)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10516)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10517)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10518)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10519)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10520)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10521)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10522)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10523)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10524)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10525)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10526)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10527)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10528)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10529)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}									

 		
            //soke 70级以上装备升级 - 刃(魔)
			if (dc.up_ob->data.dwObjectID == 10557)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10558)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10559)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10560)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10561)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10562)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10563)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10564)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10565)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10566)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10567)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10568)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10569)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10570)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10571)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10572)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10573)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10574)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10575)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10576)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10577)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10578)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10579)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 枪(物)
			if (dc.up_ob->data.dwObjectID == 10607)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10608)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10609)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10610)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10611)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10612)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10613)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10614)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10615)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10616)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10617)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10618)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10619)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10620)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10621)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10622)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10623)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10624)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10625)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10626)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10627)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10628)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10629)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 枪(魔)
			if (dc.up_ob->data.dwObjectID == 10657)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10658)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10659)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10660)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10661)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10662)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10663)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10664)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10665)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10666)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10667)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10668)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10669)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10670)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10671)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10672)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10673)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10674)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10675)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10676)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10677)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10678)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10679)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 盾
			if (dc.up_ob->data.dwObjectID == 10757)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10758)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10759)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10760)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10761)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10762)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10763)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10764)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10765)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10766)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10767)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10768)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10769)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10770)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10771)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10772)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10773)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10774)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10775)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10776)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10777)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10778)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10779)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}							

         	//soke 70级以上装备升级 - 战旗(法术)
			if (dc.up_ob->data.dwObjectID == 12007)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12008)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12009)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12010)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12011)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12012)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12013)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12014)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12015)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12016)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12017)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12018)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12019)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12020)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12021)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12022)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12023)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12024)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12025)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12026)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12027)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12028)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12029)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}

	        //soke 70级以上装备升级 - 战旗(弓手)
			if (dc.up_ob->data.dwObjectID == 12057)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12058)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12059)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12060)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12061)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12062)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12063)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12064)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12065)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12066)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12067)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12068)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12069)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12070)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12071)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12072)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12073)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12074)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12075)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12076)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12077)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12078)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12079)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}

           	//soke 70级以上装备升级 - 战旗(仙术)
			if (dc.up_ob->data.dwObjectID == 12107)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12108)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12109)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12110)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12111)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12112)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12113)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12114)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12115)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12116)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12117)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12118)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12119)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12120)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12121)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12122)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12123)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12124)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12125)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12126)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12127)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12128)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12129)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}

	        //soke 70级以上装备升级 - 战旗(召唤)
			if (dc.up_ob->data.dwObjectID == 12157)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12158)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12159)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12160)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12161)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12162)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12163)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12164)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12165)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12166)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12167)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12168)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12169)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12170)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12171)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12172)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12173)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12174)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12175)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12176)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12177)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12178)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12179)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}

           	//soke 70级以上装备升级 - 护心镜(物)
			if (dc.up_ob->data.dwObjectID == 12207)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12208)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12209)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12210)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12211)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12212)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12213)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12214)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12215)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12216)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12217)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12218)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12219)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12220)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12221)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12222)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12223)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12224)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12225)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12226)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12227)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12228)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12229)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}

	        //soke 70级以上装备升级 - 护心镜(魔)
			if (dc.up_ob->data.dwObjectID == 12257)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12258)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12259)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12260)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12261)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12262)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12263)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12264)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12265)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12266)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12267)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12268)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12269)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12270)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12271)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12272)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12273)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12274)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12275)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12276)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12277)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12278)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12279)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}

			//soke 70级以上装备升级 - 短枪(物)
			if (dc.up_ob->data.dwObjectID == 12307)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12308)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12309)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12310)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12311)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12312)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12313)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12314)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12315)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12316)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12317)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12318)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12319)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12320)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12321)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12322)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12323)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12324)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12325)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12326)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12327)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12328)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12329)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}						

            //soke 70级以上装备升级 - 短枪(魔)
			if (dc.up_ob->data.dwObjectID == 12357)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12358)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12359)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12360)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12361)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12362)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12363)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12364)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12365)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12366)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12367)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12368)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12369)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12370)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12371)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12372)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12373)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12374)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12375)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12376)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12377)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12378)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12379)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}

			//soke 70级以上装备升级 - 匕首(魔)
			if (dc.up_ob->data.dwObjectID == 12407)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12408)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12409)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12410)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12411)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12412)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12413)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12414)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12415)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12416)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12417)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12418)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12419)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12420)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12421)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12422)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12423)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12424)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12425)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12426)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12427)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12428)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 12429)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}	

            //soke 70级以上装备升级 - 头盔(魔)
			if (dc.up_ob->data.dwObjectID == 10907)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10908)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10909)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10910)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10911)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10912)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10913)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10914)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10915)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10916)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10917)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10918)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10919)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10920)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10921)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10922)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10923)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10924)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10925)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10926)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10927)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10928)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10929)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 头盔(物)
			if (dc.up_ob->data.dwObjectID == 10957)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10958)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10959)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10960)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10961)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10962)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10963)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10964)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10965)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10966)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10967)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10968)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10969)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10970)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10971)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10972)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10973)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10974)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10975)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10976)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10977)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10978)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 10979)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}							

  			//soke 70级以上装备升级 - 腰带(魔)
			if (dc.up_ob->data.dwObjectID == 11007)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11008)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11009)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11010)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11011)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11012)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11013)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11014)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11015)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11016)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11017)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11018)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11019)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11020)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11021)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11022)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11023)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11024)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11025)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11026)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11027)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11028)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11029)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 腰带(物)
			if (dc.up_ob->data.dwObjectID == 11057)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11058)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11059)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11060)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11061)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11062)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11063)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11064)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11065)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11066)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11067)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11068)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11069)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11070)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11071)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11072)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11073)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11074)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11075)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11076)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11077)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11078)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11079)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}				

            //soke 70级以上装备升级 - 护腕(魔)
			if (dc.up_ob->data.dwObjectID == 11107)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11108)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11109)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11110)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11111)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11112)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11113)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11114)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11115)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11116)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11117)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11118)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11119)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11120)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11121)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11122)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11123)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11124)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11125)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11126)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11127)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11128)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11129)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 护腕(物)
			if (dc.up_ob->data.dwObjectID == 11157)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11158)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11159)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11160)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11161)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11162)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11163)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11164)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11165)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11166)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11167)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11168)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11169)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11170)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11171)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11172)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11173)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11174)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11175)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11176)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11177)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11178)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11179)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}							

//soke 70级以上装备升级 - 靴(物)
			if (dc.up_ob->data.dwObjectID == 11207)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11208)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11209)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11210)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11211)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11212)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11213)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11214)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11215)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11216)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11217)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11218)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11219)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11220)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11221)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11222)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11223)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11224)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11225)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11226)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11227)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11228)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11229)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 靴(魔)
			if (dc.up_ob->data.dwObjectID == 11257)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11258)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11259)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11260)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11261)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11262)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11263)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11264)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11265)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11266)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11267)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11268)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11269)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11270)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11271)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11272)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11273)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11274)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11275)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11276)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11277)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11278)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11279)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}

            
            //soke 70级以上装备升级 - 项链(物)
			if (dc.up_ob->data.dwObjectID == 11307)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11308)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11309)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11310)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11311)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11312)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11313)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11314)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11315)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11316)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11317)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11318)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11319)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11320)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11321)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11322)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11323)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11324)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11325)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11326)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11327)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11328)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11329)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 项链(魔)
			if (dc.up_ob->data.dwObjectID == 11357)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11358)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11359)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11360)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11361)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11362)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11363)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11364)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11365)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11366)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11367)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11368)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11369)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11370)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11371)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11372)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11373)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11374)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11375)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11376)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11377)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11378)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11379)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 戒指(物)
			if (dc.up_ob->data.dwObjectID == 11407)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11408)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11409)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11410)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11411)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11412)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11413)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11414)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11415)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11416)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11417)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11418)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11419)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11420)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11421)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11422)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11423)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11424)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11425)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11426)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11427)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11428)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11429)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 戒指(魔)
			if (dc.up_ob->data.dwObjectID == 11457)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11458)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11459)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11460)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11461)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11462)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11463)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11464)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11465)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11466)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11467)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11468)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11469)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11470)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11471)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11472)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11473)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11474)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11475)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11476)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11477)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11478)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 11479)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}							

			//soke 70级以上装备升级 - 蹄铁
			if (dc.up_ob->data.dwObjectID == 16101)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16102)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16103)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16104)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16105)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16106)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16107)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16108)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16109)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			//soke 70级以上装备升级 - 缰绳
			if (dc.up_ob->data.dwObjectID == 16201)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16202)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16203)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16204)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16205)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16206)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16207)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16208)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16209)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			//soke 70级以上装备升级 - 金鞍
			if (dc.up_ob->data.dwObjectID == 16301)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(御马鞍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16302)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(御马鞍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16303)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(御马鞍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16304)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(御马鞍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16305)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(御马鞍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16306)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(御马鞍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16307)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(御马鞍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16308)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(御马鞍) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16309)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(御马鞍) x1");
				return false;
			}
			//soke 70级以上装备升级 - 护甲
			if (dc.up_ob->data.dwObjectID == 16401)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16402)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16403)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16404)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16405)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16406)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16407)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16408)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16409)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲) x1");
				return false;
			}
			//soke 70级以上装备升级 - 马镫
			if (dc.up_ob->data.dwObjectID == 16501)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马骑镫) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16502)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马骑镫) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16503)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马骑镫) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16504)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马骑镫) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16505)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马骑镫) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16506)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马骑镫) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16507)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马骑镫) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16508)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马骑镫) x1");
				return false;
			}
			else if (dc.up_ob->data.dwObjectID == 16509)
			{
				Base::reply(user, 1, EQUIP_COMPOSE21, " 当前坐骑装备升级所需： 坐骑装备升级图纸(马骑镫) x1");
				return false;
			}			

		}
	}

	DWORD need = 0;

	zEquipObjectB *uob = equipobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.retain7+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可装备升级");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE21, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"装备升级") ) 
	{
		bool ret = Upgrade::equip(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
                if ( ob->data.dwObjectID >= 2101 && ob->data.dwObjectID <=2125)//soke 主程序装备升级需求道具ID					
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"装备升级删除",delObj->base,delObj->data.kind,delObj->data.retain7);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备升级生成",ob->base,ob->data.kind,ob->data.retain7);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE21, ret?"             装备升级成功":"             装备升级失败");
				Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "装备%s升级成功，你可以改造装备了 ",ob->name);
				Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级提示：按Esc键→ 角色选择→ 确定→ 进入游戏→ 进行装备改造属性！ ");
				user.addExp(10);
			}
		}
		return true;
	}
	return false;
}

//soke 马牌精炼
bool RebuildObject::equipCompose22( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	RecastPack* pack = &(user.packs.recastPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求马牌精炼的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUrefineObjectB *base = urefineobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.itemrefine+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if((up_ob->data.kind & 16) || (up_ob->data.kind & 4))
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.itemrefine >= 2000) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求马牌精炼的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE22, "          马牌精炼等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可马牌精炼物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE22, "          该物品不能马牌精炼");
			return false;			
		}
	}

	DWORD need = 0;

	zUrefineObjectB *uob = urefineobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.itemrefine+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级马牌精炼");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE22, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"马牌精炼升级") ) 
	{
		bool ret = Upgrade::refine_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if (ob->data.dwObjectID >= 3843 && ob->data.dwObjectID <= 3844)  // 扣除精炼道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"马牌精炼删除",delObj->base,delObj->data.kind,delObj->data.itemrefine);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"马牌精炼生成",ob->base,ob->data.kind,ob->data.itemrefine);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE22, ret?"             马牌精炼成功":"             马牌精炼失败");
			}
		}
		return true;
	}
	return false;
}

//soke 神龙镶嵌
bool RebuildObject::equipCompose23( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	RecastPack* pack = &(user.packs.recastPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求镶嵌神龙宝宝的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUlongwanglvObjectB *base = ulongwanglvobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.longwanglv+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{
			//soke 条件判断
			if (up_ob->data.kind & 4 || up_ob->data.kind & 16)
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);

	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级神龙宝宝时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if (dc.up_ob->data.longwanglv >= 6) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求镶嵌神龙宝宝的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE23, "          该装备神龙宝宝等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可镶嵌神龙宝宝物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE23, "          该物品不能镶嵌神龙宝宝");
			return false;			
		}
	}

	DWORD need = 0;

	zUlongwanglvObjectB *uob = ulongwanglvobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.longwanglv+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可镶嵌神龙宝宝");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE23, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"神龙宝宝镶嵌") ) 
	{
		bool ret = Upgrade::longwang_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if (ob->data.dwObjectID >= 5301 && ob->data.dwObjectID <= 5307)
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"神龙宝宝镶嵌删除",delObj->base,delObj->data.kind,delObj->data.longwanglv);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"神龙宝宝镶嵌生成",ob->base,ob->data.kind,ob->data.longwanglv);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE23, ret?"             神龙宝宝镶嵌成功":"             神龙宝宝镶嵌失败");
			}
		}
		return true;
	}
	return false;
}

//soke 装备逆天进阶
bool RebuildObject::equipCompose24( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	RecastPack* pack = &(user.packs.recastPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备逆天进阶的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUnitianlvObjectB *base = unitianlvobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.nitianlv+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if((up_ob->data.kind & 16) || (up_ob->data.kind & 4))
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.nitianlv >= 100) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备逆天进阶的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE24, "          装备逆天进阶等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可装备逆天进阶物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE24, "          该物品不能装备逆天进阶");
			return false;			
		}
	}

	DWORD need = 0;

	zUnitianlvObjectB *uob = unitianlvobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.nitianlv+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级装备逆天进阶");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE24, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"装备逆天进阶升级") ) 
	{
		bool ret = Upgrade::nitian_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 5312 ) //扣除逆天进阶道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"装备逆天进阶删除",delObj->base,delObj->data.kind,delObj->data.nitianlv);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备逆天进阶生成",ob->base,ob->data.kind,ob->data.nitianlv);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE24, ret?"             装备逆天进阶成功":"             装备逆天进阶失败");
			}
		}
		return true;
	}
	return false;
}

//soke 装备神魂宝石镶嵌
bool RebuildObject::equipCompose25( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	RecastPack* pack = &(user.packs.recastPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备神魂宝石镶嵌的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUdouzhansflvObjectB *base = udouzhansflvobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.douzhansflv+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if((up_ob->data.kind & 16) || (up_ob->data.kind & 4))
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.douzhansflv >= 100) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备神魂宝石镶嵌的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE25, "          装备神魂宝石镶嵌等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可装备神魂宝石镶嵌物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE25, "          该物品不能装备神魂宝石镶嵌");
			return false;			
		}
	}

	DWORD need = 0;

	zUdouzhansflvObjectB *uob = udouzhansflvobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.douzhansflv+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级装备神魂宝石镶嵌");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE25, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"装备神魂宝石镶嵌升级") ) 
	{
		bool ret = Upgrade::douzhansf_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 5313  || ob->data.dwObjectID == 5317) //扣除神魂宝石镶嵌道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"装备神魂宝石镶嵌删除",delObj->base,delObj->data.kind,delObj->data.douzhansflv);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备神魂宝石镶嵌生成",ob->base,ob->data.kind,ob->data.douzhansflv);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE25, ret?"             装备神魂宝石镶嵌成功":"             装备神魂宝石镶嵌失败");
			}
		}
		return true;
	}
	return false;
}

//soke 装备暗影神石镶嵌
bool RebuildObject::equipCompose26( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	RecastPack* pack = &(user.packs.recastPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备暗影神石镶嵌的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUaystonelvObjectB *base = uaystonelvobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.aystonelv+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if((up_ob->data.kind & 16) || (up_ob->data.kind & 4))
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.aystonelv >= 100) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备暗影神石镶嵌的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE21, "          装备暗影神石镶嵌等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可装备暗影神石镶嵌物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE21, "          该物品不能装备暗影神石镶嵌");
			return false;			
		}
	}

	DWORD need = 0;

	zUaystonelvObjectB *uob = uaystonelvobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.aystonelv+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级装备暗影神石镶嵌");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE21, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"装备暗影神石镶嵌升级") ) 
	{
		bool ret = Upgrade::aystone_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 5314 ) //扣除暗影神石镶嵌道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"装备暗影神石镶嵌删除",delObj->base,delObj->data.kind,delObj->data.aystonelv);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备暗影神石镶嵌生成",ob->base,ob->data.kind,ob->data.aystonelv);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE21, ret?"             装备暗影神石镶嵌成功":"             装备暗影神石镶嵌失败");
			}
		}
		return true;
	}
	return false;
}



// 龙凤吟镶嵌
bool RebuildObject::equipCompose27( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{

   RecastPack* pack = &(user.packs.recastPack);
    if (!pack)
    {
        Zebra::logger->alarm("用户(%d:%d:%s)请求升级龙凤吟的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
        return false;
    }

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}
				
				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 10*(num-1);
		}
		
		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;
		
		bool can;

		bool is_stuff()
		{
			zUlongfengyingObjectB *base = ulongfengyingobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.retain24+1));
			if(!base) return false;
	
			return c_ob->data.dwObjectID == base->stuff;
		}
		
		bool can_compose()
		{
		   //sky 紫装才能升级龙凤吟
		   if (up_ob->data.kind & 16)  //16 紫装 4是绿装
		   {
		      return count>=1 && up_ob && is_stuff() && can;	
		   }
		   else
		      return false; //sky 不是紫装升级失败
		}
	};
	
	do_compose dc;
	pack->execEvery(dc);

	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级龙凤吟时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}

	if (dc.up_ob->data.retain24 >= 16) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级的装备龙凤吟已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE27, "          该装备龙凤吟已经达到最高等级");
		return false;			
	}
	else
	{
		if (!dc.can_compose() && !(dc.c_ob->data.dwObjectID == 2976))
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级龙凤吟物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE27, "          该物品不能升级龙凤吟");
			return false;
		}
	}
	
	if (dc.c_ob->data.dwObjectID == 2976)
	{
		for (int i = 0; i < 16; i++)
		{
			Upgrade::longfengying_upgrade(user, dc.up_ob, 100);
		}

		zObject *ob = zObject::create(dc.up_ob);

		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser *pUser) : up_ob(NULL), delObj(NULL), realDelCnt(0), pOwner(pUser)
			{
			}

			bool exec(zObject *ob)
			{
				if (2976 == ob->data.dwObjectID)
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if (pOwner && realDelCnt < MAX_DEL_COUNT)
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if (delObj->data.dwNum > 1)
						{
							realDelNum = 1;
							delObj->data.dwNum -= 1;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid, delObj->data.qwThisID, delObj->data.strName, delObj->data.dwNum, delObj->data.dwNum, 0, pOwner->id, pOwner->name, 0, NULL, "装备龙星升级删除", delObj->base, delObj->data.kind, delObj->data.drastar);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						// 已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;
					pOwner->packs.removeObject(ob);
				}
				return true;
			}

			zObject *up_ob;
			zObject *delObj;
			DWORD realDelCnt; // 实际删除的数量
			SceneUser *pOwner;
			stObjectLocation oldPos;
		};
		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob)
		{
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false))
			{
				zObject::logger(ob->createid, ob->data.qwThisID, ob->data.strName, ob->data.dwNum, ob->data.dwNum, 1, 0, NULL, user.id, user.name, "龙凤吟升级生成", ob->base, ob->data.kind, ob->data.retain24);
				Base::add_object(user, ob, false);
				Base::reply(user, 0, EQUIP_COMPOSE27, "             龙凤吟升级成功");
			}
		}
		return true;
	}

	DWORD need = 0;

	zUlongfengyingObjectB *uob = ulongfengyingobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.retain24+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级龙凤吟");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE27, "               银子不足");
		return false;
	}
	
	if ( user.packs.removeMoney(need,"龙凤吟升级") ) 
	{
		
		bool ret;
		//算法
		if(dc.up_ob->data.retain27 ==2800)
		{
			dc.up_ob->data.retain28 = 2800+zMisc::randBetween(300,600);
		}
		if(dc.up_ob->data.retain27 >= dc.up_ob->data.retain28 && dc.up_ob->data.retain27 >=2800)
		{
			ret = Upgrade::longfengying_upgrade(user, dc.up_ob,80);
		}
		else{
			if(dc.up_ob->data.retain24<=10)
			{
				ret = Upgrade::longfengying_upgrade(user, dc.up_ob, 40);
			}
			else{
				if(dc.up_ob->data.retain24==11)
				{
					if(dc.up_ob->data.retain27>1200)
					{
						ret = Upgrade::longfengying_upgrade(user, dc.up_ob, zMisc::randBetween(20,40));
					}
					else{
						ret = Upgrade::longfengying_upgrade(user, dc.up_ob, zMisc::randBetween(0,5));
					}
				}
				if(dc.up_ob->data.retain24==12)
				{
					if(dc.up_ob->data.retain27>1800)
					{
						ret = Upgrade::longfengying_upgrade(user, dc.up_ob, zMisc::randBetween(20,40));
					}
					else{
						ret = Upgrade::longfengying_upgrade(user, dc.up_ob, zMisc::randBetween(0,5));
					}
				}
				if(dc.up_ob->data.retain24==13)
				{
					if(dc.up_ob->data.retain27>2200)
					{
						ret = Upgrade::longfengying_upgrade(user, dc.up_ob, zMisc::randBetween(20,40));
					}
					else{
						ret = Upgrade::longfengying_upgrade(user, dc.up_ob, zMisc::randBetween(0,5));
					}
				}
				if(dc.up_ob->data.retain24==14)
				{
					if(dc.up_ob->data.retain27>2500)
					{
						ret = Upgrade::longfengying_upgrade(user, dc.up_ob, zMisc::randBetween(20,40));
					}
					else{
						ret = Upgrade::longfengying_upgrade(user, dc.up_ob, zMisc::randBetween(0,5));
					}
				}
				if(dc.up_ob->data.retain24==15)
				{
					if(dc.up_ob->data.retain27>2800)
					{
						ret = Upgrade::longfengying_upgrade(user, dc.up_ob, zMisc::randBetween(0,5));
					}
					else{
						ret = Upgrade::longfengying_upgrade(user, dc.up_ob, 1);
					}
				}
			}
		}
		dc.up_ob->data.retain27+=1;
		
	
		zObject* ob = zObject::create(dc.up_ob);

		struct MultiItemDelCB : public PackageCallback
        {
            MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
            { }

            bool exec(zObject* ob)
            {
                if ( 1150 == ob->data.dwObjectID )
                {
                    delObj = ob;
                    static const DWORD MAX_DEL_COUNT = 1;
                    if ( pOwner && realDelCnt < MAX_DEL_COUNT )
                    {
                        DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

                        DWORD realDelNum = 0;
                        if ( delObj->data.dwNum > needDelNum )
                        {
                            realDelNum = needDelNum;
                            delObj->data.dwNum -= realDelNum;

                            Cmd::stRefCountObjectPropertyUserCmd send;
                            send.qwThisID = delObj->data.qwThisID;
                            send.dwNum = delObj->data.dwNum;
                            pOwner->sendCmdToMe(&send, sizeof(send));
                        }
                        else
                        {
                            realDelNum = delObj->data.dwNum;
                            zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"装备龙凤吟升级删除",delObj->base,delObj->data.kind,delObj->data.retain24);
                            pOwner->packs.removeObject(delObj);
                        }
                        realDelCnt += realDelNum;
                    }
                    else
                    {
                        //已经扣除四颗，不用扣除了
                        return true;
                    }
                }
                else
                {
                    oldPos = ob->data.pos;       
                    pOwner->packs.removeObject(ob);
                }
                return true;	
            }

            zObject* up_ob;
            zObject* delObj;
            DWORD  realDelCnt; //实际删除的数量
            SceneUser* pOwner;
            stObjectLocation oldPos;
        };

        MultiItemDelCB del(&user);
        pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;
			
			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"龙凤吟升级生成",ob->base,ob->data.kind,ob->data.retain24);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE27, ret?"             龙凤吟升级成功":"             龙凤吟升级失败");
			}
		}
		return true;
	}
	return false;
}



// 轰天宝石镶嵌
bool RebuildObject::equipCompose28( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{

   RecastPack* pack = &(user.packs.recastPack);
    if (!pack)
    {
        Zebra::logger->alarm("用户(%d:%d:%s)请求升级轰天镶嵌的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
        return false;
    }

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}
				
				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 10*(num-1);
		}
		
		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;
		
		bool can;
		
		bool can_compose()
		{
		   //sky 紫装才能升级龙凤吟
		   if (up_ob->data.kind & 16)  //16 紫装 4是绿装
		   {
		      return count>=1 && up_ob  && can;	
		   }
		   else
		      return false; //sky 不是紫装升级失败
		}
	};
	
	do_compose dc;
	pack->execEvery(dc);

	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级轰天宝石时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}


	if (!dc.can_compose())
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级轰天宝石物品", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE28, "          该装备不能进行轰天镶嵌");
		return false;
	}

	bool ret = true;
	DWORD upnum = 0;
	DWORD delnum = 1;

	if ((dc.up_ob->base->kind >= 104 && dc.up_ob->base->kind <= 111) || dc.up_ob->base->kind == 136 || dc.up_ob->base->kind == 137 || dc.up_ob->base->kind == 155)
	{
		//武器
		if(dc.c_ob->data.dwObjectID == 120037 && dc.up_ob->data.hongtiannum1 <1)
		{
			dc.up_ob->data.hongtiannum1++;
		}
		else if (dc.c_ob->data.dwObjectID == 120046 && dc.up_ob->data.hongtiannum1 >=1)
		{
			if (dc.up_ob->data.hongtiannum1 >= 5) 
			{
				Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，该装备已完成轰天镶嵌");
				return false;			
			}
			upnum = dc.up_ob->data.hongtiannum1;
			if(upnum ==1)
			{
				delnum = 200;
			}
			else if(upnum ==2)
			{
				delnum = 400;
			}
			else if(upnum ==3)
			{
				delnum = 800;
			}
			else if(upnum ==4)
			{
				delnum = 1600;
			}
			if(dc.c_ob->data.dwNum < delnum)
			{
				char a[256];
				sprintf(a,"          失败，当前升级轰天需要 %d碎片",delnum);
				Base::reply(user, 1, EQUIP_COMPOSE28, a);
				return false;
			}
			dc.up_ob->data.hongtiannum1++;
			
		}
		else
		{
			Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，当前镶嵌需要 涅槃·轰天石[武器] 升级需要碎片");
			return false;
		}
	}
	else if (dc.up_ob->base->kind == 117 )//项链
	{	
		if(dc.c_ob->data.dwObjectID == 120040 && dc.up_ob->data.hongtiannum2 <1)
		{
			dc.up_ob->data.hongtiannum2++;
		}
		else if (dc.c_ob->data.dwObjectID == 120046 && dc.up_ob->data.hongtiannum2 >=1)
		{
			if (dc.up_ob->data.hongtiannum2 >= 5) 
			{
				Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，该装备已完成轰天镶嵌");
				return false;			
			}
			upnum = dc.up_ob->data.hongtiannum2;
				if(upnum ==1)
				{
					delnum = 200;
				}
				else if(upnum ==2)
				{
					delnum = 400;
				}
				else if(upnum ==3)
				{
					delnum = 800;
				}
				else if(upnum ==4)
				{
					delnum = 1600;
				}
			if(dc.c_ob->data.dwNum < delnum)
			{
				char a[256];
				sprintf(a,"          失败，当前升级轰天需要 %d碎片",delnum);
				Base::reply(user, 1, EQUIP_COMPOSE28, a);
				return false;
			}
			dc.up_ob->data.hongtiannum2++;
		}
		else
		{
			Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，当前镶嵌需要 涅槃·轰天印[项链] 升级需要碎片");
			return false;
		}
	}
	else if (dc.up_ob->base->kind == 115 )//护腕
	{
		if(dc.c_ob->data.dwObjectID == 120039 && dc.up_ob->data.hongtiannum3 <1)
		{
			dc.up_ob->data.hongtiannum3++;
		}
		else if (dc.c_ob->data.dwObjectID == 120046 && dc.up_ob->data.hongtiannum3 >=1)
		{
			if (dc.up_ob->data.hongtiannum3 >= 5) 
			{
				Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，该装备已完成轰天镶嵌");
				return false;			
			}
			upnum = dc.up_ob->data.hongtiannum3;
				if(upnum ==1)
			{
				delnum = 200;
			}
			else if(upnum ==2)
			{
				delnum = 400;
			}
			else if(upnum ==3)
			{
				delnum = 800;
			}
			else if(upnum ==4)
			{
				delnum = 1600;
			}
			if(dc.c_ob->data.dwNum < delnum)
			{
				char a[256];
				sprintf(a,"          失败，当前升级轰天需要 %d碎片",delnum);
				Base::reply(user, 1, EQUIP_COMPOSE28, a);
				return false;
			}
			dc.up_ob->data.hongtiannum3++;
		}
		else
		{
			Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，当前镶嵌需要 涅槃·轰天盾[护腕] 升级需要碎片");
			return false;
		}
	}
	else if (dc.up_ob->base->kind == 113 )//头盔
	{
		if(dc.c_ob->data.dwObjectID == 120041 && dc.up_ob->data.hongtiannum4 < 1)
		{
			dc.up_ob->data.hongtiannum4++;
		}
		else if (dc.c_ob->data.dwObjectID == 120046 && dc.up_ob->data.hongtiannum4 >=1)
		{
			if (dc.up_ob->data.hongtiannum4 >= 5) 
			{
				Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，该装备已完成轰天镶嵌");
				return false;			
			}
			upnum = dc.up_ob->data.hongtiannum4;
				if(upnum ==1)
			{
				delnum = 200;
			}
			else if(upnum ==2)
			{
				delnum = 400;
			}
			else if(upnum ==3)
			{
				delnum = 800;
			}
			else if(upnum ==4)
			{
				delnum = 1600;
			}
			if(dc.c_ob->data.dwNum < delnum)
			{
				char a[256];
				sprintf(a,"          失败，当前升级轰天需要 %d碎片",delnum);
				Base::reply(user, 1, EQUIP_COMPOSE28, a);
				return false;
			}
			dc.up_ob->data.hongtiannum4++;
		}
		else
		{
			Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，当前镶嵌需要 涅槃·轰天缨[头盔] 升级需要碎片");
			return false;
		}
	}
	else if (dc.up_ob->base->kind == 116 )//鞋子
	{
		if(dc.c_ob->data.dwObjectID == 120042 && dc.up_ob->data.hongtiannum5 <1)
		{
			dc.up_ob->data.hongtiannum5++;
		}
		else if (dc.c_ob->data.dwObjectID == 120046 && dc.up_ob->data.hongtiannum5 >=1)
		{
			if (dc.up_ob->data.hongtiannum5 >= 5) 
			{
				Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，该装备已完成轰天镶嵌");
				return false;			
			}
			upnum = dc.up_ob->data.hongtiannum5;
				if(upnum ==1)
			{
				delnum = 200;
			}
			else if(upnum ==2)
			{
				delnum = 400;
			}
			else if(upnum ==3)
			{
				delnum = 800;
			}
			else if(upnum ==4)
			{
				delnum = 1600;
			}
			if(dc.c_ob->data.dwNum < delnum)
			{
				char a[256];
				sprintf(a,"          失败，当前升级轰天需要 %d碎片",delnum);
				Base::reply(user, 1, EQUIP_COMPOSE28, a);
				return false;
			}
			dc.up_ob->data.hongtiannum5++;
		}
		else
		{
			Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，当前镶嵌需要 涅槃·轰天轮[鞋子] 升级需要碎片");
			return false;
		}
	}
	else if (dc.up_ob->base->kind == 118 )//戒指
	{
		if(dc.c_ob->data.dwObjectID == 120043 && dc.up_ob->data.hongtiannum6 <1)
		{
			dc.up_ob->data.hongtiannum6++;
		}
		else if (dc.c_ob->data.dwObjectID == 120046  && dc.up_ob->data.hongtiannum6 >= 1)
		{
			if (dc.up_ob->data.hongtiannum6 >= 5) 
			{
				Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，该装备已完成轰天镶嵌");
				return false;			
			}
			upnum = dc.up_ob->data.hongtiannum6;
				if(upnum ==1)
			{
				delnum = 200;
			}
			else if(upnum ==2)
			{
				delnum = 400;
			}
			else if(upnum ==3)
			{
				delnum = 800;
			}
			else if(upnum ==4)
			{
				delnum = 1600;
			}
			if(dc.c_ob->data.dwNum < delnum)
			{
				char a[256];
				sprintf(a,"          失败，当前升级轰天需要 %d碎片",delnum);
				Base::reply(user, 1, EQUIP_COMPOSE28, a);
				return false;
			}
			dc.up_ob->data.hongtiannum6++;
		}
		else
		{
			Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，当前镶嵌需要 涅槃·轰天缴[戒指] 升级需要碎片");
			return false;
		}
	}
	else if (dc.up_ob->base->kind == 114 )//腰带
	{
		if(dc.c_ob->data.dwObjectID == 120044 && dc.up_ob->data.hongtiannum7 < 1)
		{
			dc.up_ob->data.hongtiannum7++;
		}
		else if (dc.c_ob->data.dwObjectID == 120046 && dc.up_ob->data.hongtiannum7 >= 1)
		{
			if (dc.up_ob->data.hongtiannum7 >= 5) 
			{
				Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，该装备已完成轰天镶嵌");
				return false;			
			}
			upnum = dc.up_ob->data.hongtiannum7;
				if(upnum ==1)
			{
				delnum = 200;
			}
			else if(upnum ==2)
			{
				delnum = 400;
			}
			else if(upnum ==3)
			{
				delnum = 800;
			}
			else if(upnum ==4)
			{
				delnum = 1600;
			}
			if(dc.c_ob->data.dwNum < delnum)
			{
				char a[256];
				sprintf(a,"          失败，当前升级轰天需要 %d碎片",delnum);
				Base::reply(user, 1, EQUIP_COMPOSE28, a);
				return false;
			}
			dc.up_ob->data.hongtiannum7++;
		}
		else
		{
			Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，当前镶嵌需要 涅槃·轰天镜[腰带] 升级需要碎片");
			return false;
		}
	}
	else if (dc.up_ob->base->kind >= 101 && dc.up_ob->base->kind <= 103)//衣服
	{
		if(dc.c_ob->data.dwObjectID == 120045 && dc.up_ob->data.hongtiannum8 < 1)
		{
			dc.up_ob->data.hongtiannum8++;
		}
		else if (dc.c_ob->data.dwObjectID == 120046 && dc.up_ob->data.hongtiannum8 >= 1)
		{
			if (dc.up_ob->data.hongtiannum8 >= 5) 
			{
				Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，该装备已完成轰天镶嵌");
				return false;			
			}
			upnum = dc.up_ob->data.hongtiannum8;
				if(upnum ==1)
			{
				delnum = 200;
			}
			else if(upnum ==2)
			{
				delnum = 400;
			}
			else if(upnum ==3)
			{
				delnum = 800;
			}
			else if(upnum ==4)
			{
				delnum = 1600;
			}
			if(dc.c_ob->data.dwNum < delnum)
			{
				char a[256];
				sprintf(a,"          失败，当前升级轰天需要 %d碎片",delnum);
				Base::reply(user, 1, EQUIP_COMPOSE28, a);
				return false;
			}
			dc.up_ob->data.hongtiannum8++;
		}
		else
		{
			Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，当前镶嵌需要 涅槃·轰天绫[衣服] 升级需要碎片");
			return false;
		}
	}
	else if ((dc.up_ob->base->kind >= 137 && dc.up_ob->base->kind <= 138) || (dc.up_ob->base->kind >= 141 && dc.up_ob->base->kind <= 147) || dc.up_ob->base->kind == 156 )//副手
	{
		if(dc.c_ob->data.dwObjectID == 120038 && dc.up_ob->data.hongtiannum9 < 1)
		{
			dc.up_ob->data.hongtiannum9++;
		}
		else if (dc.c_ob->data.dwObjectID == 120046 && dc.up_ob->data.hongtiannum9 >= 1)
		{
			if (dc.up_ob->data.hongtiannum9 >= 5) 
			{
				Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，该装备已完成轰天镶嵌");
				return false;			
			}
			upnum = dc.up_ob->data.hongtiannum9;
			if(upnum ==1)
			{
				delnum = 200;
			}
			else if(upnum ==2)
			{
				delnum = 400;
			}
			else if(upnum ==3)
			{
				delnum = 800;
			}
			else if(upnum ==4)
			{
				delnum = 1600;
			}
			if(dc.c_ob->data.dwNum < delnum)
			{
				char a[256];
				sprintf(a,"          失败，当前升级轰天需要 %d碎片",delnum);
				Base::reply(user, 1, EQUIP_COMPOSE28, a);
				return false;
			}
			dc.up_ob->data.hongtiannum9++;
		}
		else
		{
			Base::reply(user, 1, EQUIP_COMPOSE28, "          失败，当前镶嵌需要 涅槃·轰天刃[副手]");
			return false;
		}
	}
	//todo 继续写就行了
	else
	{
		Base::reply(user, 1, EQUIP_COMPOSE28, "          此道具暂未开放轰天镶嵌");
		return false;
	}

	zObject *ob = zObject::create(dc.up_ob);

	struct MultiItemDelCB : public PackageCallback
	{
		MultiItemDelCB(SceneUser *pUser,DWORD num) : up_ob(NULL), delObj(NULL), realDelCnt(0), pOwner(pUser),xuyaonum(num)
		{
		}

		bool exec(zObject *ob)
		{
			if (( ob->data.dwObjectID>=120037 && ob->data.dwObjectID<=120045 ) || ob->data.dwObjectID==120046 )
			{
				delObj = ob;
				static const DWORD MAX_DEL_COUNT = xuyaonum;
				if (pOwner && realDelCnt < MAX_DEL_COUNT)
				{
					DWORD needDelNum = xuyaonum;

					DWORD realDelNum = 0;
					if (delObj->data.dwNum > needDelNum)
					{
						realDelNum = needDelNum;
						delObj->data.dwNum -= realDelNum;

						Cmd::stRefCountObjectPropertyUserCmd send;
						send.qwThisID = delObj->data.qwThisID;
						send.dwNum = delObj->data.dwNum;
						pOwner->sendCmdToMe(&send, sizeof(send));
					}
					else if (delObj->data.dwNum < needDelNum)
					{
						return false;
					}
					else
					{
						realDelNum = delObj->data.dwNum;
						pOwner->packs.removeObject(delObj);
					}
					realDelCnt += realDelNum;
				}
				else
				{
					// 已经扣除四颗，不用扣除了
					return true;
				}
			}
			else
			{
				oldPos = ob->data.pos;
				pOwner->packs.removeObject(ob);
			}
			return true;
		}

		zObject *up_ob;
		zObject *delObj;
		DWORD realDelCnt; // 实际删除的数量
		SceneUser *pOwner;
		DWORD xuyaonum; // 实际删除的数量
		stObjectLocation oldPos;
	};
	
	MultiItemDelCB del(&user,delnum);
	pack->execEvery(del);
	if (ob)
	{
		ob->data.pos = del.oldPos;

		if (user.packs.addObject(ob, false))
		{
			Base::add_object(user, ob, false);
			Base::reply(user, ret ? 0 : 1, EQUIP_COMPOSE28, ret ? "             轰天宝石镶嵌成功" : "             轰天宝石镶嵌失败");
		}
	}
	return true;
}


//soke 王者升级
bool RebuildObject::equipCompose29( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	RecastPack* pack = &(user.packs.recastPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求王者升级的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool can_compose()
		{

			//soke 条件判断
			if((up_ob->data.kind & 16) || (up_ob->data.kind & 4))
			{
				return count>=1 && up_ob && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);

	if ((dc.up_ob->data.dwObjectID >= 20381 && dc.up_ob->data.dwObjectID <= 29681)) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求王者升级的装备已经完成", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE29, "          此装备已升级至王者武器");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可装备暗影神石镶嵌物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE29, "          该物品不能进行王者武器升级");
			return false;			
		}
	}

	DWORD need = 50000;

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE29, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"装备暗影神石镶嵌升级") ) 
	{
		//升级
		if (!dc.up_ob)
		{
			Base::reply(user, 1, EQUIP_COMPOSE29, "          没有找到要升级的装备");
			return false; 
		}

		zObjectB *base = objectbm.get(dc.up_ob->data.dwObjectID);
		if (!dc.up_ob||!base)
		{
			Base::reply(user, 1, EQUIP_COMPOSE29, "          没有找到要升级的装备2");
			return false;
		}

		
		int up_id = dc.up_ob->data.dwObjectID + 10000;

		if(up_id == 0)
		{
			Base::reply(user, 1, EQUIP_COMPOSE29, "          当前装备已经不能在此升级");
			return false;
		}
		zObjectB *new_base = objectbm.get(up_id);
		new_base->goldrating = 10000;
		new_base->holyrating = 100000;
		if (!new_base)
		{
			Base::reply(user, 1, EQUIP_COMPOSE29, "          升级后的物品不存在");
			return false;
		}

		zObject *new_ob = zObject::create(new_base,1);

		if (dc.up_ob->data.kind & 16)
		{
			EquipMaker maker(NULL);;
			new_ob->data.round = dc.up_ob->data.round;
			new_ob->data.kind = 16;
			maker.modify_attri16(new_ob, new_base);
		}
		else
		{
			EquipMaker maker(NULL);
			new_ob->data.round = dc.up_ob->data.round;
			new_ob->data.kind = 4;
			maker.modify_attri(new_ob, new_base);
		}

		EquipMaker modify(&user);
		modify.modify_addmaker(new_ob,&user);
		modify.upgrade_inherit(user, new_ob, dc.up_ob);
		
		
		// user.packs.removeObject(dc.up_ob, true, true);

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 50129 ) //扣除王者碎片镶嵌道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 100;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = 100;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else if(delObj->data.dwNum < needDelNum )
						{
							return false;
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);

		
		
		Zebra::logger->debug("升级完成");
		if (new_ob ) 
		{	
			new_ob->data.pos = del.oldPos;

			if (user.packs.addObject(new_ob, false)) 
			{			
				Base::add_object(user, new_ob, false);
				Base::reply(user, 0, EQUIP_COMPOSE29, "             王者武器升级成功");
			}
		}
		return true;
	}
	return false;
}
//附件助手助手包裹功能开始////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//soke 段位勋章升级
bool RebuildObject::equipCompose51( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级段位勋章的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_sxtuff()
		{
			zMedaltObjectB *base = medaltobjectbm.get(up_ob->data.dwObjectID + 10000 * (up_ob->data.dmedalt + 1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can2_compose()
		{
			//soke 条件判断
			if ( up_ob->base->kind == ItemType_Xunzhang )
			{
				return count>=1 && up_ob && is_sxtuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级段位勋章时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.dwObjectID == 3831 && dc.up_ob->data.dmedalt > 999) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级段位勋章的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE51, "          该段位勋章等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can2_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级段位勋章物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE51, "          该物品不能升级段位勋章");
			return false;			
		}
	}

	DWORD need = 0;

	zMedaltObjectB *uob = medaltobjectbm.get(dc.up_ob->data.dwObjectID + 10000 * (dc.up_ob->data.dmedalt + 1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级段位勋章");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE51, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"段位勋章升级") ) 
	{
		bool ret = Upgrade::medal_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID >= 3832 && ob->data.dwObjectID <= 3837 ) //升级材料
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"段位勋章升级删除",delObj->base,delObj->data.kind,delObj->data.dmedalt);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if ( (ob->data.dwObjectID >= 3826 && ob->data.dwObjectID <= 3830) && ob->data.dmedalt > 100) 
			{
				++ob->data.dwObjectID;
				ob->data.dmedalt = 0;
            }

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"段位勋章升级生成",ob->base,ob->data.kind,ob->data.dmedalt);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE51, ret?"             段位勋章升级成功":"             段位勋章升级失败");
			}
		}
		return true;
	}
	return false;
}

//soke 寒冰麒麟升级
bool RebuildObject::equipCompose52( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级寒冰麒麟的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_sxtuff()
		{
			zUhbqlObjectB *base = uhbqlobjectbm.get(up_ob->data.dwObjectID + 10000 * (up_ob->data.retain1 + 1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{
			//soke 条件判断
			if ( up_ob->base->kind == ItemType_Hanbing )
			{
				return count>=1 && up_ob && is_sxtuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级寒冰麒麟时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.dwObjectID == 3865 && dc.up_ob->data.retain1 > 999) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级寒冰麒麟的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE52, "          该寒冰麒麟等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级寒冰麒麟物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE52, "          该物品不能升级寒冰麒麟");
			return false;			
		}
	}

	DWORD need = 0;

	zUhbqlObjectB *uob = uhbqlobjectbm.get(dc.up_ob->data.dwObjectID + 10000 * (dc.up_ob->data.retain1 + 1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级寒冰麒麟");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE52, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"寒冰麒麟升级") ) 
	{
		bool ret = Upgrade::hb_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID >= 3866 && ob->data.dwObjectID <= 3869 ) //升级材料
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"寒冰麒麟升级删除",delObj->base,delObj->data.kind,delObj->data.retain1);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if ( (ob->data.dwObjectID >= 3860 && ob->data.dwObjectID <= 3864) && ob->data.retain1 > 100) 
			{
				++ob->data.dwObjectID;
				ob->data.retain1 = 0;
            }

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"寒冰麒麟升级生成",ob->base,ob->data.kind,ob->data.retain1);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE52, ret?"             寒冰麒麟升级成功":"             寒冰麒麟升级失败");
			}
		}
		return true;
	}
	return false;
}

//soke 翅膀升级
bool RebuildObject::equipCompose53( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级天使之翼的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUangelObjectB *base = uangelobjectbm.get(up_ob->data.dwObjectID+10000*(up_ob->data.retain4+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if ( up_ob->base->kind == ItemType_Chibang )
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);

	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级翅膀时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.retain1 >= 1800) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级天使之翼的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE53, "          该天使之翼等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级天使之翼物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE53, "          该物品不能升级天使之翼");
			return false;			
		}
	}

	DWORD need = 0;

	zUangelObjectB *uob = uangelobjectbm.get(dc.up_ob->data.dwObjectID + 10000*(dc.up_ob->data.retain4+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级寒冰麒麟");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE53, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"天使之翼升级") ) 
	{
		bool ret = Upgrade::angel_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 3905 )
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"天使之翼升级删除",delObj->base,delObj->data.kind,delObj->data.retain4);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"天使之翼升级生成",ob->base,ob->data.kind,ob->data.retain4);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE53, ret?"             天使之翼升级成功":"             天使之翼升级失败");
			}
		}
		return true;
	}
	return false;
}

//soke 神兵升级
bool RebuildObject::equipCompose54( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级神兵的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zMagicalObjectB *base = magicalobjectbm.get(up_ob->data.dwObjectID+10000*(up_ob->data.retain5+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if ( up_ob->base->kind == ItemType_Shenbing0 || up_ob->base->kind == ItemType_Shenbing1 || up_ob->base->kind == ItemType_Shenbing2 )
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);

	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级神兵时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}

	if ( dc.up_ob->data.retain5 >= 10000) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级神兵的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE54, "          该神兵等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级神兵物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE54, "          该物品不能升级神兵");
			return false;			
		}
	}

	DWORD need = 0;

	zMagicalObjectB *uob = magicalobjectbm.get(dc.up_ob->data.dwObjectID + 10000*(dc.up_ob->data.retain5+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级神兵");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE54, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"神兵升级") ) 
	{
		bool ret = Upgrade::magical_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 35101 )
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"神兵升级删除",delObj->base,delObj->data.kind,delObj->data.retain5);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"神兵升级生成",ob->base,ob->data.kind,ob->data.retain5);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE54, ret?"             神兵升级成功":"             神兵升级失败");
			}
		}
		return true;
	}
	return false;
}

//soke 神兽升级
bool RebuildObject::equipCompose55( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级神兽的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zMythicalObjectB *base = mythicalobjectbm.get(up_ob->data.dwObjectID+10000*(up_ob->data.retain6+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if ( up_ob->base->kind == ItemType_Shenshou1 || up_ob->base->kind == ItemType_Shenshou2 )
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);

	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级神兽时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}

	if ( dc.up_ob->data.retain6 >= 10000) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级神兽的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE55, "          该神兽等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级神兽物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE55, "          该物品不能升级神兽");
			return false;			
		}
	}

	DWORD need = 0;

	zMythicalObjectB *uob = mythicalobjectbm.get(dc.up_ob->data.dwObjectID + 10000*(dc.up_ob->data.retain6+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级神兽");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE55, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"神兽升级") ) 
	{
		bool ret = Upgrade::mythical_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 35101 )
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"神兽升级删除",delObj->base,delObj->data.kind,delObj->data.retain6);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"神兽升级生成",ob->base,ob->data.kind,ob->data.retain6);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE55, ret?"             神兽升级成功":"             神兽升级失败");
			}
		}
		return true;
	}
	return false;
}

//soke 护身符璀璨宝石镶嵌
bool RebuildObject::equipCompose56( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备璀璨宝石镶嵌的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗护身符璀璨宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUbrightObjectB *base = ubrightobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.itemkscuican+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 只允许护身符条件判断
			if(up_ob->base->kind == 80)
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.itemkscuican >= 1000) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备璀璨属性宝石镶嵌的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE56, "         璀璨属性宝石镶嵌等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可装备璀璨属性宝石镶嵌物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE56, "          该物品不能璀璨属性宝石镶嵌");
			return false;			
		}
	}

	DWORD need = 0;

	zUbrightObjectB *uob = ubrightobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.itemkscuican+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可进行璀璨属性宝石镶嵌");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE56, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"护身符宝石镶嵌") ) 
	{
		bool ret = Upgrade::bright_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 3850 ) //扣除镶嵌道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"璀璨宝石镶嵌删除",delObj->base,delObj->data.kind,delObj->data.itemkscuican);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备镶嵌生成",ob->base,ob->data.kind,ob->data.itemkscuican);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE56, ret?"             装备镶嵌成功":"             装备镶嵌失败");
			}
		}
		return true;
	}
	return false;
}

//soke 护身符神兽宝石镶嵌
bool RebuildObject::equipCompose57( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求神兽宝石镶嵌的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct CheckSoulStone : public PackageCallback
	{
	public:
		CheckSoulStone() 
			: stone(NULL), aumletob(NULL), canBind(true)
		{}

		bool exec(zObject* ob)
		{
			if (ob->data.dwObjectID >= 3838 && ob->data.dwObjectID <= 3842) //myy 判定镶嵌道具
			{
				if (!stone) 
				{
					stone = ob;
				}
				else 
				{
					canBind = false;
					return false;
				}
			}
			//soke 条件判断
			else if(ob->base->kind == 80)
			{
				if (!aumletob) 
				{
					aumletob = ob;
				}
				else 
				{
					canBind = false;
					return false;
				}	
			}
			else 
			{
				canBind = false;
				return false;
			}
			return true;	
		}

	public:
		zObject* stone;
		zObject* aumletob;
		bool canBind;
	};

	CheckSoulStone chkStn;
	pack->execEvery(chkStn);

	//sky 限制条件
	if (!chkStn.stone || !chkStn.aumletob || !chkStn.aumletob->data.kind == 80 || !chkStn.aumletob->data.bind ) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE57, "护身符可镶嵌神兽宝石失败");
		Zebra::logger->alarm("用户(%d:%d:%s)请求对不能镶嵌神兽宝石的装备进行镶嵌", user.charbase.accid, user.charbase.id, user.charbase.name);
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "护身符可镶嵌神兽宝石");
		return false;
	}

	if (chkStn.aumletob->data.itemksAmulet >= 1000)
	{
		Base::reply(user, 1, EQUIP_COMPOSE57, "         当前镶嵌等级已经最高");
		return false;
	}

	DWORD need = 10000; //soke 需要1锭
	if (!user.packs.checkMoney(need))
	{
		Base::reply(user, 1, EQUIP_COMPOSE57, "               银子不足");
		return false;
	}
	//神兽宝石镶嵌属性 青龙 白虎 朱雀 玄武 麒麟
	if (user.packs.removeMoney(need, "神兽宝石镶嵌"))
	{
        if (chkStn.aumletob->data.itemksAmulet <= 1000)
		{
            if(chkStn.stone->data.dwObjectID == 3838)
			{
				chkStn.aumletob->data.p12damage += 5;
				chkStn.aumletob->data.m12axpdamage += 5;
			    ++chkStn.aumletob->data.itemksAmulet;
			}
            else if (chkStn.stone->data.dwObjectID == 3839)
			{
				chkStn.aumletob->data.m12damage += 5;
			    chkStn.aumletob->data.m12axmdamage += 5;
			    ++chkStn.aumletob->data.itemksAmulet;
			}
            else if (chkStn.stone->data.dwObjectID == 3840)
			{
				chkStn.aumletob->data.p12defence += 5;
			    ++chkStn.aumletob->data.itemksAmulet;	
			}
            else if (chkStn.stone->data.dwObjectID == 3841)
			{
				chkStn.aumletob->data.m12defence += 5;
			    ++chkStn.aumletob->data.itemksAmulet;
			}
		    else if (chkStn.stone->data.dwObjectID == 3842)
			{
				chkStn.aumletob->data.m12axhp += 50; //桃子修改麒麟石血量
				++chkStn.aumletob->data.itemksAmulet;
			} 
			else
			{
				return false;
			}
		}
		zObject* ob = zObject::create(chkStn.aumletob);
		  
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL), realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if (ob->data.dwObjectID >= 3838 && ob->data.dwObjectID <= 3842)  // myy 扣除道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if (pOwner && realDelCnt < MAX_DEL_COUNT)
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if (delObj->data.dwNum > needDelNum)
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;
					pOwner->packs.removeObject(ob);
				}
				return true;
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);

		if (ob) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{
				Base::add_object(user, ob, false);
				Base::reply(user, 0, EQUIP_COMPOSE57,"神兽宝石镶嵌成功");
			}
		}
		return true;
	}
	return false;
}

//soke 尚方宝剑进阶
bool RebuildObject::equipCompose58( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备尚方宝剑进阶的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUshangfanglvObjectB *base = ushangfanglvobjectbm.get(up_ob->data.dwObjectID+10000*(up_ob->data.shangfanglv+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if ( up_ob->base->kind == ItemType_ShangfangBaojian )
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.shangfanglv >= 1000) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备尚方宝剑进阶的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE58, "          装备尚方宝剑进阶等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可装备尚方宝剑进阶物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE58, "          该物品不能装备尚方宝剑进阶");
			return false;			
		}
	}

	DWORD need = 0;

	zUshangfanglvObjectB *uob = ushangfanglvobjectbm.get(dc.up_ob->data.dwObjectID + 10000*(dc.up_ob->data.shangfanglv+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级装备尚方宝剑进阶");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE58, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"装备尚方宝剑进阶升级") ) 
	{
		bool ret = Upgrade::shangfang_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 35101 ) //扣除尚方宝剑进阶道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"装备尚方宝剑进阶删除",delObj->base,delObj->data.kind,delObj->data.shangfanglv);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备尚方宝剑进阶生成",ob->base,ob->data.kind,ob->data.shangfanglv);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE58, ret?"             装备尚方宝剑进阶成功":"             装备尚方宝剑进阶失败");
			}
		}
		return true;
	}
	return false;
}

//soke 紫金龙袍进阶
bool RebuildObject::equipCompose59( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备紫金龙袍进阶的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUzijinlplvObjectB *base = uzijinlplvobjectbm.get(up_ob->data.dwObjectID+10000*(up_ob->data.zijinlplv+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if ( up_ob->base->kind == ItemType_ZijinLongpao )
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.zijinlplv >= 1000) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备紫金龙袍进阶的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE59, "          装备紫金龙袍进阶等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可装备紫金龙袍进阶物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE59, "          该物品不能装备紫金龙袍进阶");
			return false;			
		}
	}

	DWORD need = 0;

	zUzijinlplvObjectB *uob = uzijinlplvobjectbm.get(dc.up_ob->data.dwObjectID + 10000*(dc.up_ob->data.zijinlplv+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级装备紫金龙袍进阶");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE59, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"装备紫金龙袍进阶升级") ) 
	{
		bool ret = Upgrade::zijinlp_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 35101 ) //扣除紫金龙袍进阶道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"装备紫金龙袍进阶删除",delObj->base,delObj->data.kind,delObj->data.zijinlplv);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备紫金龙袍进阶生成",ob->base,ob->data.kind,ob->data.zijinlplv);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE59, ret?"             装备紫金龙袍进阶成功":"             装备紫金龙袍进阶失败");
			}
		}
		return true;
	}
	return false;
}


//soke 护身符神斧石镶嵌
bool RebuildObject::equipCompose61( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备神斧石镶嵌的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗护身符神斧石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUxuanhuaObjectB *base = uxuanhuaobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.retain32+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 只允许护身符条件判断
			if(up_ob->base->kind == 80)
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.retain32 >= 1000) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备神斧石镶嵌的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE61, "         神斧石镶嵌等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可装备神斧石镶嵌物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE61, "          该物品不能神斧石镶嵌");
			return false;			
		}
	}

	DWORD need = 0;

	zUxuanhuaObjectB *uob = uxuanhuaobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.retain32+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可进行神斧石镶嵌");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE61, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"护身符宝石镶嵌") ) 
	{
		bool ret = Upgrade::xuanhua_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 3851 ) //扣除镶嵌道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"神斧石镶嵌删除",delObj->base,delObj->data.kind,delObj->data.retain32);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备镶嵌生成",ob->base,ob->data.kind,ob->data.retain32);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE61, ret?"             神斧石镶嵌成功":"             神斧石镶嵌失败");
			}
		}
		return true;
	}
	return false;
}


//soke 护身符黑曜石镶嵌
bool RebuildObject::equipCompose62( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备黑曜石镶嵌的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗护身符黑曜石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUheiyaoObjectB *base = uheiyaoobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.retain33+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 只允许护身符条件判断
			if(up_ob->base->kind == 80)
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.retain33 >= 1000) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备黑曜石镶嵌的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE62, "         黑曜石镶嵌等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可装备黑曜石镶嵌物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE62, "          该物品不能黑曜石镶嵌");
			return false;			
		}
	}

	DWORD need = 0;

	zUheiyaoObjectB *uob = uheiyaoobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.retain33+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可进行黑曜石镶嵌");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE62, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"护身符宝石镶嵌") ) 
	{
		bool ret = Upgrade::heiyao_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 3852 ) //扣除镶嵌道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"黑曜石镶嵌删除",delObj->base,delObj->data.kind,delObj->data.retain33);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备镶嵌生成",ob->base,ob->data.kind,ob->data.retain33);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE62, ret?"             黑曜石镶嵌成功":"             黑曜石镶嵌失败");
			}
		}
		return true;
	}
	return false;
}


//soke 护身符精金石镶嵌
bool RebuildObject::equipCompose63( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备精金石镶嵌的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗护身符精金石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUjingjinObjectB *base = ujingjinobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.retain34+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 只允许护身符条件判断
			if(up_ob->base->kind == 80)
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.retain34 >= 1000) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求装备精金石镶嵌的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE63, "         精金石镶嵌等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可装备精金石镶嵌物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE63, "          该物品不能精金石镶嵌");
			return false;			
		}
	}

	DWORD need = 0;

	zUjingjinObjectB *uob = ujingjinobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.retain34+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可进行精金石镶嵌");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE63, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"护身符宝石镶嵌") ) 
	{
		bool ret = Upgrade::jingjin_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 3853 ) //扣除镶嵌道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"精金石镶嵌删除",delObj->base,delObj->data.kind,delObj->data.retain34);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备镶嵌生成",ob->base,ob->data.kind,ob->data.retain34);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE63, ret?"             精金石镶嵌成功":"             精金石镶嵌失败");
			}
		}
		return true;
	}
	return false;
}


//soke 金箍咒强化
bool RebuildObject::equipCompose64( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求金箍咒强化的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}
		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUjgzqhObjectB *base = ujgzqhobjectbm.get(up_ob->data.dwObjectID+10000*(up_ob->data.syxqgrade21+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}
    //-----------------------------------------
		bool can_compose()
		{
		   //sky 条件判断需要其他宝石都1000级方可进行强化
		   if(up_ob->base->kind == 90 && up_ob->data.dmedalt >= 500) //判断需求
		   {
		      return count>=1 && up_ob && is_stuff() && can;	
		   }
		   else
		      return false; //sky 不是其他宝石都1000级则升级失败
		}
	};
   //--------------------------------------------
	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)金箍咒强化时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.syxqgrade21 >= 1000) //次数上限
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求金箍咒强化的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE64, "         金箍咒强化等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可金箍咒强化的物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE64, "必须升级至金箍级才能进行强化哦！");
			return false;			
		}
	}

	DWORD need = 0;

	zUjgzqhObjectB *uob = ujgzqhobjectbm.get(dc.up_ob->data.dwObjectID + 10000*(dc.up_ob->data.syxqgrade21+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可进行金箍咒强化");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE64, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"金箍咒强化宝石镶嵌") ) 
	{
		bool ret = Upgrade::jgzqh_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 5351 ) //扣除镶嵌道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"金箍咒强化宝石删除",delObj->base,delObj->data.kind,delObj->data.syxqgrade21);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备镶嵌生成",ob->base,ob->data.kind,ob->data.syxqgrade21);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE64, ret?"             装备镶嵌成功":"             装备镶嵌失败");
			}
		}
		return true;
	}
	return false;
}
//soke 金箍咒字纹
bool RebuildObject::equipCompose65( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求金箍咒字纹的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}
		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUjgzzwObjectB *base = ujgzzwobjectbm.get(up_ob->data.dwObjectID+10000*(up_ob->data.syxqgrade22+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}
    //-----------------------------------------
		bool can_compose()
		{
		   //sky 条件判断需要其他宝石都1000级方可进行强化
		   if(up_ob->base->kind == 90 && up_ob->data.dmedalt >= 500) //判断需求
		   {
		      return count>=1 && up_ob && is_stuff() && can;	
		   }
		   else
		      return false; //sky 不是其他宝石都1000级则升级失败
		}
	};
   //--------------------------------------------
	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)金箍咒字纹时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.syxqgrade22 >= 1000) //次数上限
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求金箍咒字纹的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE65, "         金箍咒字纹等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可金箍咒字纹的物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE65, "必须升级至金箍级才能进行强化哦！");
			return false;			
		}
	}

	DWORD need = 0;

	zUjgzzwObjectB *uob = ujgzzwobjectbm.get(dc.up_ob->data.dwObjectID + 10000*(dc.up_ob->data.syxqgrade22+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可进行金箍咒字纹");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE65, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"金箍咒字纹宝石镶嵌") ) 
	{
		bool ret = Upgrade::jgzzw_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 5351 ) //扣除镶嵌道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"金箍咒字纹宝石删除",delObj->base,delObj->data.kind,delObj->data.syxqgrade22);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备镶嵌生成",ob->base,ob->data.kind,ob->data.syxqgrade22);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE65, ret?"             装备镶嵌成功":"             装备镶嵌失败");
			}
		}
		return true;
	}
	return false;
}
//soke 金箍咒淬炼
bool RebuildObject::equipCompose66( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求金箍咒淬炼的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}
		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUjgzclObjectB *base = ujgzclobjectbm.get(up_ob->data.dwObjectID+10000*(up_ob->data.syxqgrade23+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}
    //-----------------------------------------
		bool can_compose()
		{
		   //sky 条件判断需要其他宝石都1000级方可进行强化
		   if(up_ob->base->kind == 90 && up_ob->data.dmedalt >= 500 && up_ob->data.syxqgrade21 >= 1000) //判断需求
		   {
		      return count>=1 && up_ob && is_stuff() && can;	
		   }
		   else
		      return false; //sky 不是其他宝石都1000级则升级失败
		}
	};
   //--------------------------------------------
	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)金箍咒淬炼时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.syxqgrade23 >= 100) //次数上限
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求金箍咒淬炼的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE66, "         金箍咒淬炼等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可金箍咒淬炼的物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE66, "必须强化满级才能进行淬炼哦！");
			return false;			
		}
	}

	DWORD need = 0;

	zUjgzclObjectB *uob = ujgzclobjectbm.get(dc.up_ob->data.dwObjectID + 10000*(dc.up_ob->data.syxqgrade23+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可进行金箍咒淬炼");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE66, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"金箍咒淬炼宝石镶嵌") ) 
	{
		bool ret = Upgrade::jgzcl_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 5351 ) //扣除镶嵌道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"金箍咒淬炼宝石删除",delObj->base,delObj->data.kind,delObj->data.syxqgrade23);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备镶嵌生成",ob->base,ob->data.kind,ob->data.syxqgrade23);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE66, ret?"             装备镶嵌成功":"             装备镶嵌失败");
			}
		}
		return true;
	}
	return false;
}

//soke 寒冰神炼
bool RebuildObject::equipCompose67( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求寒冰神炼的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}
		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUhbslObjectB *base = uhbslobjectbm.get(up_ob->data.dwObjectID+10000*(up_ob->data.syxqgrade24+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}
    //-----------------------------------------
		bool can_compose()
		{
		   //sky 条件判断需要其他宝石都1000级方可进行强化
		   if(up_ob->data.dwObjectID ==3865 && up_ob->data.retain1 >= 701) //判断需求
		   {
		      return count>=1 && up_ob && is_stuff() && can;	
		   }
		   else
		      return false; //sky 不是其他宝石都1000级则升级失败
		}
	};
   //--------------------------------------------
	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)寒冰神炼时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}

	if ( dc.up_ob->data.syxqgrade24 >= 1000) //次数上限
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求寒冰神炼的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE67, "         寒冰神炼等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可寒冰神炼的物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE67, "必须升级至701阶以上才能进行神炼哦！");
			return false;			
		}
	}

	DWORD need = 0;

	zUhbslObjectB *uob = uhbslobjectbm.get(dc.up_ob->data.dwObjectID + 10000*(dc.up_ob->data.syxqgrade24+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可进行寒冰神炼");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE67, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"寒冰神炼宝石镶嵌") ) 
	{
		bool ret = Upgrade::hbsl_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 5356 ) //扣除镶嵌道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"寒冰神炼宝石删除",delObj->base,delObj->data.kind,delObj->data.syxqgrade24);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备镶嵌生成",ob->base,ob->data.kind,ob->data.syxqgrade24);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE67, ret?"             装备镶嵌成功":"             装备镶嵌失败");
			}
		}
		return true;
	}
	return false;
}
//soke 寒冰字纹
bool RebuildObject::equipCompose68( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求寒冰字纹的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}
		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUhbzwObjectB *base = uhbzwobjectbm.get(up_ob->data.dwObjectID+10000*(up_ob->data.syxqgrade25+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}
    //-----------------------------------------
		bool can_compose()
		{
		   //sky 条件判断需要其他宝石都1000级方可进行强化
		   if(up_ob->data.dwObjectID ==3865) //判断需求
		   {
		      return count>=1 && up_ob && is_stuff() && can;	
		   }
		   else
		      return false; //sky 不是其他宝石都1000级则升级失败
		}
	};
   //--------------------------------------------
	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)寒冰字纹时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.syxqgrade25 >= 1000) //次数上限
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求寒冰字纹的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE68, "         寒冰字纹等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可寒冰字纹的物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE68, "必须升级至五彩鎏光才能进行神炼哦！");
			return false;			
		}
	}

	DWORD need = 0;

	zUhbzwObjectB *uob = uhbzwobjectbm.get(dc.up_ob->data.dwObjectID + 10000*(dc.up_ob->data.syxqgrade25+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可进行寒冰字纹");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE68, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"寒冰字纹宝石镶嵌") ) 
	{
		bool ret = Upgrade::hbzw_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 5356 ) //扣除镶嵌道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"寒冰字纹宝石删除",delObj->base,delObj->data.kind,delObj->data.syxqgrade25);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备镶嵌生成",ob->base,ob->data.kind,ob->data.syxqgrade25);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE68, ret?"             装备镶嵌成功":"             装备镶嵌失败");
			}
		}
		return true;
	}
	return false;
}
//soke 寒冰纹章
bool RebuildObject::equipCompose69( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求寒冰纹章的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}
		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUhbwzObjectB *base = uhbwzobjectbm.get(up_ob->data.dwObjectID+10000*(up_ob->data.syxqgrade26+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}
    //-----------------------------------------
		bool can_compose()
		{
		   //sky 条件判断需要其他宝石都1000级方可进行强化
		   if(up_ob->data.dwObjectID ==3865 && up_ob->data.syxqgrade24 >= 701) //判断需求
		   {
		      return count>=1 && up_ob && is_stuff() && can;	
		   }
		   else
		      return false; //sky 不是其他宝石都1000级则升级失败
		}
	};
   //--------------------------------------------
	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)寒冰纹章时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.syxqgrade26 >= 100) //次数上限
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求寒冰纹章的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE69, "         寒冰纹章等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可寒冰纹章的物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE69, "必须升级至八阶及以上神炼才能进行寒冰纹章镶嵌哦！");
			return false;			
		}
	}

	DWORD need = 0;

	zUhbwzObjectB *uob = uhbwzobjectbm.get(dc.up_ob->data.dwObjectID + 10000*(dc.up_ob->data.syxqgrade26+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可进行寒冰纹章");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE69, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"寒冰纹章宝石镶嵌") ) 
	{
		bool ret = Upgrade::hbwz_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 5356 ) //扣除镶嵌道具
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"寒冰纹章宝石删除",delObj->base,delObj->data.kind,delObj->data.syxqgrade26);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"装备镶嵌生成",ob->base,ob->data.kind,ob->data.syxqgrade26);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE69, ret?"             装备镶嵌成功":"             装备镶嵌失败");
			}
		}
		return true;
	}
	return false;
}
//百兽图鉴助手包裹功能开始////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//百兽图鉴进阶
bool RebuildObject::equipCompose101( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	HandbookPack* pack = &(user.packs.handbookPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级图鉴进阶的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 2*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUhandbookObjectB *base = uhandbookobjectbm.get(up_ob->data.dwObjectID+10000*(up_ob->data.handbooklvg+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if ( up_ob->base->kind == ItemType_handbookitem1 || up_ob->base->kind == ItemType_handbookitem2 || up_ob->base->kind == ItemType_handbookitem3
			  || up_ob->base->kind == ItemType_handbookitem4 || up_ob->base->kind == ItemType_handbookitem5 || up_ob->base->kind == ItemType_handbookitem6
			  || up_ob->base->kind == ItemType_handbookitem7 || up_ob->base->kind == ItemType_handbookitem8 || up_ob->base->kind == ItemType_handbookitem9
			  || up_ob->base->kind == ItemType_handbookitem10)
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);

	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级图鉴进阶时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}

	if ( dc.up_ob->data.handbooklvg > 699) //桃子 图鉴强化次数700次
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级图鉴进阶的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE101, "          该图鉴进阶等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级图鉴进阶物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE101, "          该物品不能升级图鉴进阶");
			return false;			
		}
	}

	DWORD need = 0;

	zUhandbookObjectB *uob = uhandbookobjectbm.get(dc.up_ob->data.dwObjectID+10000*(dc.up_ob->data.handbooklvg+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级图鉴进阶");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE101, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"图鉴进阶升级") ) 
	{
		bool ret = Upgrade::handbook_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 3980 )
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"图鉴进阶升级删除",delObj->base,delObj->data.kind,delObj->data.handbooklvg);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"图鉴进阶升级生成",ob->base,ob->data.kind,ob->data.handbooklvg);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE101, ret?"             图鉴进阶升级成功":"             图鉴进阶升级失败");
			}
		}
		return true;
	}
	return false;
}
//图鉴签名
bool RebuildObject::equipCompose102( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	if (!check_space(user, 1, 1)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE102, "包裹请保留一个空格");
		return false;
	}
	HandbookPack* pack = &(user.packs.handbookPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求签名的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct CheckSoulStone : public PackageCallback
	{
	public:
		CheckSoulStone() 
		: stone(NULL), eqpObj(NULL)
		{}

		bool exec(zObject* ob)
		{
			//soke 星灵石
			if ( 548 == ob->data.dwObjectID ) 
			{
				if (!stone) 
				{
					stone = ob;
				}
				else 
				{
					return false;
				}
			}
			//soke 能签名的的条件
			else if (ob->data.needlevel >= 1 )
			{
				if (!eqpObj) 
				{
					eqpObj = ob;
				}
				else 
				{
					return false;
				}	
			}
			return true;	
		}

	public:
		zObject* stone;
		zObject* eqpObj;
	};

	CheckSoulStone chkStn;
	pack->execEvery(chkStn);

	//soke 条件判断
	if (!chkStn.stone || !chkStn.eqpObj )  
	{
		Base::reply(user, 1, EQUIP_COMPOSE102, "图鉴签名");
		Zebra::logger->alarm("用户(%d:%d:%s)请求对图鉴进行签名,条件不足，不能签名", user.charbase.accid, user.charbase.id, user.charbase.name);
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "签名失败");
		return false;			
	}
	
    DWORD needMoney = 50000;
    //扣钱
    if (!user.packs.checkMoney(needMoney) || !user.packs.removeMoney(needMoney, "图鉴签名") ) 
    {
        Base::reply(user, 1, EQUIP_COMPOSE102, "银子不足5锭");
        return false;
    }

	if (chkStn.eqpObj->data.needlevel >= 1)
	{
		EquipMaker maker(NULL);

	    base = objectbm.get(chkStn.eqpObj->data.dwObjectID);

		zObject* ob = zObject::create(base,1); 
	     
		if (ob)
		{			
			//图鉴进阶强化属性继承
	        if (chkStn.eqpObj->data.handbooklvg > 0)
	        {
		      int i = chkStn.eqpObj->data.handbooklvg;
		       while (i > 0)
		       {
				i--;
				Upgrade::handbook_upgrade(user,ob,10000);
	           }
	        }
			if(chkStn.stone->data.dwNum > 1)
			{
				chkStn.stone->data.dwNum -= 1;
				Cmd::stRefCountObjectPropertyUserCmd send;
				send.qwThisID = chkStn.stone->data.qwThisID;
				send.dwNum = chkStn.stone->data.dwNum;
				user.sendCmdToMe(&send, sizeof(send));
				user.packs.removeObject(chkStn.eqpObj);
			}
			else
			{
				user.packs.execEvery(pack, Type2Type<ClearPack>());
			}	
			ob->data.bind=1;
			strncpy(ob->data.maker, user.charbase.name, MAX_NAMESIZE);//打造者名字
			ob->data.pos = stObjectLocation(pack->type(), pack->id(), 0, Cmd::MAKECELLTYPE_EQUIP);

			if (user.packs.addObject(ob, false)) 
			{
				Base::add_object(user, ob, false);
				Base::reply(user, 0, EQUIP_COMPOSE102,"图鉴签名成功");
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "图鉴签名成功，花费银子5锭");
			}
		}
	}

	return true;
}

///soke 法宝升级
bool RebuildObject::equipCompose103( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	FabaoPack* pack = &(user.packs.fabaoPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级法宝的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 10*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUfabaoObjectB *base = ufabaoobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.retain20+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if ( up_ob->base->kind == ItemType_Fabao)
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.retain20 > 5000) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级法宝的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE103, "          该法宝等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级法宝物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE103, "          该物品不能升级法宝");
			return false;			
		}
	}

	DWORD need = 0;

	zUfabaoObjectB *uob = ufabaoobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.retain20+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级法宝");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE103, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"法宝升级") ) 
	{
		bool ret = Upgrade::fabao_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 20016 )
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"法宝升级删除",delObj->base,delObj->data.kind,delObj->data.retain20);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"法宝升级生成",ob->base,ob->data.kind,ob->data.retain20);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE103, ret?"             法宝升级成功":"             法宝升级失败");
			}
		}
		return true;
	}
	return false;
}




///soke 法宝符石升级
bool RebuildObject::equipCompose104( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	FabaoPack* pack = &(user.packs.fabaoPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级法宝符石的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 10*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUfabaoObjectB *base = ufabaoobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.retain20+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if ( up_ob->base->kind >= ItemType_Fabaofj1 && up_ob->base->kind <= ItemType_Fabaofj12)
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.retain20 > 1000) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级法宝符石的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE104, "          该法宝符石等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级法宝符石物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE104, "          该物品不能升级法宝符石");
			return false;			
		}
	}

	DWORD need = 0;

	zUfabaoObjectB *uob = ufabaoobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.retain20+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级法宝符石");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE104, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"法宝符石升级") ) 
	{
		bool ret = Upgrade::fabao_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 20017 )
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"法宝升级删除",delObj->base,delObj->data.kind,delObj->data.retain20);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"法宝符石升级生成",ob->base,ob->data.kind,ob->data.retain20);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE104, ret?"           法宝符石升级成功":"           法宝符石升级失败");
			}
		}
		return true;
	}
	return false;
}



///soke 十二星宿升级
bool RebuildObject::equipCompose105( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	AnnexPack* pack = &(user.packs.annexPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级十二星宿的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 10*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUSEXXObjectB *base = usexxobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.retain21+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if ( up_ob->base->kind >=ItemType_SEXX1 && up_ob->base->kind <= ItemType_SEXX12)
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.retain21 > 1000) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级十二星宿的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE105, "          该十二星宿等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级十二星宿物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE105, "          该物品不能升级十二星宿");
			return false;			
		}
	}

	DWORD need = 0;

	zUSEXXObjectB *uob = usexxobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.retain21+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级十二星宿");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE105, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"十二星宿升级") ) 
	{
		bool ret = Upgrade::SEXX_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 20032 )
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"十二星宿升级删除",delObj->base,delObj->data.kind,delObj->data.retain21);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"十二星宿升级生成",ob->base,ob->data.kind,ob->data.retain21);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE105, ret?"             十二星宿升级成功":"             十二星宿升级失败");
			}
		}
		return true;
	}
	return false;
}


///soke 元神升级
bool RebuildObject::equipCompose106( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	YuanshenPack* pack = &(user.packs.yuanshenPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级元神的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 10*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUYUANSHENObjectB *base = uYUANSHENobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.retain22+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if ( up_ob->base->kind ==ItemType_YUANSHEN)
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.retain22 > 5000) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级元神的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE106, "          该元神等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级元神物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE106, "          该物品不能升级元神");
			return false;			
		}
	}

	DWORD need = 0;

	zUYUANSHENObjectB *uob = uYUANSHENobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.retain22+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级元神");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE106, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"元神升级") ) 
	{
		bool ret = Upgrade::YUANSHEN_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 21108 )
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"元神升级删除",delObj->base,delObj->data.kind,delObj->data.retain22);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"元神升级生成",ob->base,ob->data.kind,ob->data.retain22);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE106, ret?"             元神升级成功":"             元神升级失败");
			}
		}
		return true;
	}
	return false;
}


///soke 元神装备升级
bool RebuildObject::equipCompose107( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	YuanshenPack* pack = &(user.packs.yuanshenPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级元神装备的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 10*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUYUANSHENObjectB *base = uYUANSHENobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.retain22+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if ( up_ob->base->kind >=ItemType_YUANSHENFJ1 && up_ob->base->kind <=ItemType_YUANSHENFJ6)
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.retain22 > 1000) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级元神装备的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE107, "          该元神装备等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级元神装备物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE107, "          该物品不能升级元神装备");
			return false;			
		}
	}

	DWORD need = 0;

	zUYUANSHENObjectB *uob = uYUANSHENobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.retain22+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可升级元神装备");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE107, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"元神装备升级") ) 
	{
		bool ret = Upgrade::YUANSHEN_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 21109 )
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"元神装备升级删除",delObj->base,delObj->data.kind,delObj->data.retain22);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"元神装备升级生成",ob->base,ob->data.kind,ob->data.retain22);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE107, ret?"             元神装备升级成功":"             元神装备升级失败");
			}
		}
		return true;
	}
	return false;
}


///soke 元神宝石镶嵌
bool RebuildObject::equipCompose108( SceneUser &user,zObjectB *base, const Cmd::ItemRecastPropertyUserCmd *rev )
{
	YuanshenPack* pack = &(user.packs.yuanshenPack);
	if (!pack)
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级元神镶嵌的包裹不存在", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;
	}

	struct do_compose : public PackageCallback
	{
		do_compose() : up_ob(NULL), c_ob(NULL), odds_ob(NULL), count(0), can(true)
		{ }

		bool exec(zObject* ob)
		{
			if (!up_ob && ob->base->recast) 
			{
				up_ob = ob;
			}
			else 
			{
				if (!c_ob) 
				{
					c_ob = ob;
					count += ob->data.dwNum;
					return true;
				}

				if (ob && ob->data.dwObjectID == c_ob->data.dwObjectID) 
				{
					count += ob->data.dwNum;
					return true;
				}

				can=false;
				return false;
			}
			return true;	
		}

		//这是每颗升级宝石额外增加的升星机率，最多可以叠加4颗
		int odds() const
		{
			DWORD num = count >= 1 ? 1 : count;
			return 10*(num-1);
		}

		zObject* up_ob;
		zObject* c_ob;
		zObject* odds_ob;
		int count;

		bool can;

		bool is_stuff()
		{
			zUYSBSObjectB *base = uYSBSobjectbm.get(up_ob->data.dwObjectID+100000*(up_ob->data.retain23+1));
			if(!base) return false;

			return c_ob->data.dwObjectID == base->stuff;
		}

		bool can_compose()
		{

			//soke 条件判断
			if ( up_ob->base->kind >=ItemType_YUANSHEN && up_ob->base->kind <=ItemType_YUANSHENFJ6)
			{
				return count>=1 && up_ob && is_stuff() && can;	
			}
			else
				return false;
		}
	};

	do_compose dc;
	pack->execEvery(dc);
	if (!dc.up_ob || !dc.c_ob ) 
    {
		Zebra::logger->alarm("用户(%d:%d:%s)升级时操作非法", user.charbase.accid, user.charbase.id, user.charbase.name);
		return false;			
	}
	if ( dc.up_ob->data.retain23 > 1000) 
	{
		Zebra::logger->alarm("用户(%d:%d:%s)请求升级元神镶嵌的等级已经最高", user.charbase.accid, user.charbase.id, user.charbase.name);
		Base::reply(user, 1, EQUIP_COMPOSE108, "          该元神宝石镶嵌等级已经最高");
		return false;			
	}
	else
	{
		if (!dc.can_compose()) 
		{
			Zebra::logger->alarm("用户(%d:%d:%s)请求升级不可升级元神镶嵌物品", user.charbase.accid, user.charbase.id, user.charbase.name);
			Base::reply(user, 1, EQUIP_COMPOSE108, "          该物品不能进行元神宝石镶嵌");
			return false;			
		}
	}

	DWORD need = 0;

	zUYSBSObjectB *uob = uYSBSobjectbm.get(dc.up_ob->data.dwObjectID + 100000*(dc.up_ob->data.retain23+1));
	if(uob)  
	{
		need = uob->gold;
	}
	else 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "该物品不可进行元神宝石镶嵌");	
		return false;
	}

	if (!user.packs.checkMoney(need)) 
	{
		Base::reply(user, 1, EQUIP_COMPOSE108, "               银子不足");
		return false;
	}

	if ( user.packs.removeMoney(need,"元神镶嵌升级") ) 
	{
		bool ret = Upgrade::YSBS_upgrade(user, dc.up_ob, dc.odds());

		zObject* ob = zObject::create(dc.up_ob);
		struct MultiItemDelCB : public PackageCallback
		{
			MultiItemDelCB(SceneUser* pUser) : up_ob(NULL), delObj(NULL),  realDelCnt(0), pOwner(pUser)
			{ }

			bool exec(zObject* ob)
			{
				if ( ob->data.dwObjectID == 21110 )
				{
					delObj = ob;
					static const DWORD MAX_DEL_COUNT = 1;
					if ( pOwner && realDelCnt < MAX_DEL_COUNT )
					{
						DWORD needDelNum = MAX_DEL_COUNT - realDelCnt;

						DWORD realDelNum = 0;
						if ( delObj->data.dwNum > needDelNum )
						{
							realDelNum = needDelNum;
							delObj->data.dwNum -= realDelNum;

							Cmd::stRefCountObjectPropertyUserCmd send;
							send.qwThisID = delObj->data.qwThisID;
							send.dwNum = delObj->data.dwNum;
							pOwner->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							realDelNum = delObj->data.dwNum;
							zObject::logger(delObj->createid,delObj->data.qwThisID,delObj->data.strName,delObj->data.dwNum,delObj->data.dwNum,0,pOwner->id,pOwner->name,0,NULL,"元神镶嵌升级删除",delObj->base,delObj->data.kind,delObj->data.retain23);
							pOwner->packs.removeObject(delObj);
						}
						realDelCnt += realDelNum;
					}
					else
					{
						//已经扣除四颗，不用扣除了
						return true;
					}
				}
				else
				{
					oldPos = ob->data.pos;       
					pOwner->packs.removeObject(ob);
				}
				return true;	
			}

			zObject* up_ob;
			zObject* delObj;
			DWORD  realDelCnt; //实际删除的数量
			SceneUser* pOwner;
			stObjectLocation oldPos;
		};

		MultiItemDelCB del(&user);
		pack->execEvery(del);
		if (ob ) 
		{	
			ob->data.pos = del.oldPos;

			if (user.packs.addObject(ob, false)) 
			{			
				zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,1,0,NULL,user.id,user.name,"元神镶嵌升级生成",ob->base,ob->data.kind,ob->data.retain23);
				Base::add_object(user, ob, false);
				Base::reply(user, ret?0:1, EQUIP_COMPOSE108, ret?"             元神宝石镶嵌成功":"             元神宝石镶嵌失败");
			}
		}
		return true;
	}
	return false;
}
//soke 改造开始////////////////////////////////////////////////////////////////////////
#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**
 * @brief 重新升星
 * @param 物品 {zObject*} ob
 * @return 是否成功 {*}
 */
bool EquipMaker::re_upgrade(zObject* ob)
{
	//soke 升级之前先清除升星的加成，不然会叠加
	ob->data.m1axhp = 0;				
	ob->data.p1damage = 0;				
	ob->data.m1axpdamage = 0;			
	ob->data.m1damage = 0;				
	ob->data.m1axmdamage = 0;			
	ob->data.p1defence = 0;	
	ob->data.m1defence = 0;


	/*Zebra::logger->debug("改造完开始升星 最大攻击力=%d,最小攻击力=%d,最大法攻=%d,最小法攻=%d,物防=%d,魔防=%d,生命=%d",
		                ob->data.maxpdamage,
						ob->data.pdamage,
						ob->data.maxmdamage,
						ob->data.mdamage,
						ob->data.pdefence,
						ob->data.mdefence,
						ob->data.maxhp);*/
	for (int i = 0;i < ob->data.upgrade;i++)
	{
		zUpgradeObject2B *base = upgradeobject2bm.get(ob->data.dwObjectID+100000*(i+1));
		if(!base) return false;

		COMPUTE( p1damage )				// 最小攻击力
		COMPUTE( m1axpdamage )			// 最大攻击力
		COMPUTE( m1damage )			    // 最小法术攻击力
		COMPUTE( m1axmdamage )			// 最大法术攻击力

		COMPUTE( p1defence )			    // 物防
		COMPUTE( m1defence )				// 魔防
		COMPUTE( m1axhp )			    // 最大生命值

		/*Zebra::logger->debug("第%d次升星 最大攻击力=%d,最小攻击力=%d,最大法攻=%d,最小法攻=%d,物防=%d,魔防=%d,生命=%d,基础属性=====最大攻击力=%d,最小攻击力=%d,最大法攻=%d,最小法攻=%d,物防=%d,魔防=%d,生命=%d",
		i+1,
		ob->data.maxpdamage,
		ob->data.pdamage,
		ob->data.maxmdamage,
		ob->data.mdamage,
		ob->data.pdefence,
		ob->data.mdefence,
		ob->data.maxhp,

		base->maxpdamage,
		base->pdamage,
		base->maxmdamage,
		base->mdamage,
		base->pdefence,
		base->mdefence,
		base->maxhp
		
		);*/
	}
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**
 * @brief 重新升星-装备升级绿装
 * @param 物品 {zObject*} ob
 * @return 是否成功 {*}
 */
bool EquipMaker::re2_upgrade(zObject* ob)
{
	ob->data.m1axhp = 0;				
	ob->data.p1damage = 0;				
	ob->data.m1axpdamage = 0;			
	ob->data.m1damage = 0;				
	ob->data.m1axmdamage = 0;			
	ob->data.p1defence = 0;	
	ob->data.m1defence = 0;

	/*Zebra::logger->debug("改造完开始升星 最大攻击力=%d,最小攻击力=%d,最大法攻=%d,最小法攻=%d,物防=%d,魔防=%d,生命=%d",
		                ob->data.maxpdamage,
						ob->data.pdamage,
						ob->data.maxmdamage,
						ob->data.mdamage,
						ob->data.pdefence,
						ob->data.mdefence,
						ob->data.maxhp);*/

	int down = ob->data.upgrade >= 100?ob->data.upgrade:1;
	for( int i=0;i<down;i++)
	{
		zUpgradeObject2B *base = upgradeobject2bm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
		if(!base) return false;
		ob->data.upgrade--;
		
		for (int t = 0;t < ob->data.upgrade;t++)
		{
			zUpgradeObject2B *base = upgradeobject2bm.get(ob->data.dwObjectID+100000*(t+1));

			COMPUTE( p1damage )				// 最小攻击力
			COMPUTE( m1axpdamage )			// 最大攻击力
			COMPUTE( m1damage )			    // 最小法术攻击力
			COMPUTE( m1axmdamage )			// 最大法术攻击力

			COMPUTE( p1defence )			    // 物防
			COMPUTE( m1defence )				// 魔防
			COMPUTE( m1axhp )			    // 最大生命值

			/*Zebra::logger->debug("第%d次升星 最大攻击力=%d,最小攻击力=%d,最大法攻=%d,最小法攻=%d,物防=%d,魔防=%d,生命=%d,基础属性=====最大攻击力=%d,最小攻击力=%d,最大法攻=%d,最小法攻=%d,物防=%d,魔防=%d,生命=%d",
			i+1,
			ob->data.maxpdamage,
			ob->data.pdamage,
			ob->data.maxmdamage,
			ob->data.mdamage,
			ob->data.pdefence,
			ob->data.mdefence,
			ob->data.maxhp,

			base->maxpdamage,
			base->pdamage,
			base->maxmdamage,
			base->mdamage,
			base->pdefence,
			base->mdefence,
			base->maxhp
			
			);*/
		}
	}
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
bool EquipMaker::repur_upgrade(zObject* ob)
{
	//soke 升级之前先清除升星的加成，不然会叠加
	ob->data.m2axhp = 0;				
	ob->data.p2damage = 0;				
	ob->data.m2axpdamage = 0;			
	ob->data.m2damage = 0;				
	ob->data.m2axmdamage = 0;			
	ob->data.p2defence = 0;	
	ob->data.m2defence = 0;

	/*Zebra::logger->debug("改造完开始升星 最大攻击力=%d,最小攻击力=%d,最大法攻=%d,最小法攻=%d,物防=%d,魔防=%d,生命=%d",
		                ob->data.maxpdamage,
						ob->data.pdamage,
						ob->data.maxmdamage,
						ob->data.mdamage,
						ob->data.pdefence,
						ob->data.mdefence,
						ob->data.maxhp);*/
	for (int i = 0;i < ob->data.upgrade;i++)
	{
		zUpgradePurpleB *base = upgradepurplebm.get(ob->data.dwObjectID+100000*(i+1));
		if(!base) return false;
		COMPUTE( p2damage )				// 最小攻击力
		COMPUTE( m2axpdamage )			// 最大攻击力
		COMPUTE( m2damage )			    // 最小法术攻击力
		COMPUTE( m2axmdamage )			// 最大法术攻击力

		COMPUTE( p2defence )			    // 物防
		COMPUTE( m2defence )				// 魔防
		COMPUTE( m2axhp )			    // 最大生命值

		/*Zebra::logger->debug("第%d次升星 最大攻击力=%d,最小攻击力=%d,最大法攻=%d,最小法攻=%d,物防=%d,魔防=%d,生命=%d,基础属性=====最大攻击力=%d,最小攻击力=%d,最大法攻=%d,最小法攻=%d,物防=%d,魔防=%d,生命=%d",
		i+1,
		ob->data.maxpdamage,
		ob->data.pdamage,
		ob->data.maxmdamage,
		ob->data.mdamage,
		ob->data.pdefence,
		ob->data.mdefence,
		ob->data.maxhp,

		base->maxpdamage,
		base->pdamage,
		base->maxmdamage,
		base->mdamage,
		base->pdefence,
		base->mdefence,
		base->maxhp
		
		);*/
	}
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x += base->x;
/**
 * @brief 重新升星-装备升级紫装
 * @param 物品 {zObject*} ob
 * @return 是否成功 {*}
 */
bool EquipMaker::repur2_upgrade(zObject* ob)
{
	//soke 升级之前先清除升星的加成，不然会叠加
	ob->data.m2axhp = 0;				
	ob->data.p2damage = 0;				
	ob->data.m2axpdamage = 0;			
	ob->data.m2damage = 0;				
	ob->data.m2axmdamage = 0;			
	ob->data.p2defence = 0;	
	ob->data.m2defence = 0;

	/*Zebra::logger->debug("改造完开始升星 最大攻击力=%d,最小攻击力=%d,最大法攻=%d,最小法攻=%d,物防=%d,魔防=%d,生命=%d",
		                ob->data.maxpdamage,
						ob->data.pdamage,
						ob->data.maxmdamage,
						ob->data.mdamage,
						ob->data.pdefence,
						ob->data.mdefence,
						ob->data.maxhp);*/


	//soke 装备升级掉1星
	int down = ob->data.upgrade >= 100?ob->data.upgrade:1;
	for( int i=0;i<down;i++)
	{
		zUpgradePurpleB *base = upgradepurplebm.get(ob->data.dwObjectID+100000*(ob->data.upgrade));
		if(!base) return false;
		ob->data.upgrade--;
		
		for (int t = 0;t < ob->data.upgrade;t++)
		{
			zUpgradePurpleB *base = upgradepurplebm.get(ob->data.dwObjectID+100000*(t+1));

			COMPUTE( p2damage )				// 最小攻击力
			COMPUTE( m2axpdamage )			// 最大攻击力
			COMPUTE( m2damage )			    // 最小法术攻击力
			COMPUTE( m2axmdamage )			// 最大法术攻击力

			COMPUTE( p2defence )			    // 物防
			COMPUTE( m2defence )				// 魔防
			COMPUTE( m2axhp )			    // 最大生命值

			/*Zebra::logger->debug("第%d次升星 最大攻击力=%d,最小攻击力=%d,最大法攻=%d,最小法攻=%d,物防=%d,魔防=%d,生命=%d,基础属性=====最大攻击力=%d,最小攻击力=%d,最大法攻=%d,最小法攻=%d,物防=%d,魔防=%d,生命=%d",
			i+1,
			ob->data.maxpdamage,
			ob->data.pdamage,
			ob->data.maxmdamage,
			ob->data.mdamage,
			ob->data.pdefence,
			ob->data.mdefence,
			ob->data.maxhp,

			base->maxpdamage,
			base->pdamage,
			base->maxmdamage,
			base->mdamage,
			base->pdefence,
			base->mdefence,
			base->maxhp
			
			);*/
		}
	}
	return true;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x = static_cast<WORD>(ob->data.x*1.4);

//soke 装备改造
bool RebuildObject::modify(SceneUser& user, const Cmd::stPropertyUserCmd* cmd)
{
	Cmd::stModifyPropertyUserCmd * command = (Cmd::stModifyPropertyUserCmd *)cmd;

	zObject *up_ob = user.packs.uom.getObjectByThisID(command->up_id);
	if (!up_ob)
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "改造失败，没有找到要改造的装备，您需要重新打开改造对话框进行改造");
		return false; 
	}
	zObjectB *base = objectbm.get(up_ob->data.dwObjectID);
	if (!up_ob||!base)
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "改造失败，没有找到要改造的装备");
		return false;
	}
	
	if (!up_ob || !EquipMaker::can_modify(up_ob)) 
	{
		Zebra::logger->debug("用户%s请求对不能对该装备改造", user.charbase.name);
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "改造失败，要改造的装备必须是70级并且绑定并且紫装或者绿装");
		return false;			
	}
	//if (!check_npc(user, up_ob->base, NpcTrade::NPC_UPDATE_OBJECT)) 
	//{
	//	return false;
	//}

	   //soke 效验NPC访问是否合法
	   

	zObject* ob = user.packs.uom.getObjectByID(command->gem_id, command->gem_level, true);
	if (!ob || ob->data.dwObjectID != MODIFYSTONE) 
	{
		Zebra::logger->debug("[改造]:用户%s对装备改造时缺少材料", user.charbase.name);
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "改造材料不足");
		return false;			
	}

	DWORD gold = MODIFY_MONEY;

	DWORD taxMoney = (DWORD)((gold*(user.scene->getTax())/100.0f)+0.5f); // 买东西收税
	gold = gold + taxMoney;
	Cmd::Session::t_taxAddCountry_SceneSession send;
	send.dwCountryID = user.scene->getCountryID();
	send.qwTaxMoney = taxMoney;
	sessionClient->sendCmd(&send, sizeof(send));

	if (!user.packs.checkMoney(gold)) 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "银子不足");
		return false;
	}

	EquipMaker modify(&user);
	if ( user.packs.removeMoney(gold,"改造") ) 
	{
		if (command->type==0)
		{
			if (check_npc(user, up_ob->base, NpcTrade::NPC_UPDATE_OBJECT)) //sky 改造
			{
				if ( up_ob->data.kind & 16 )
				{
					if(!modify.modify_attri16(up_ob,base))
					{
						Zebra::logger->debug("[紫装改造属性]:用户%s装备%s改造属性失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						//modify.re_upgrade(up_ob);
						Zebra::logger->debug("[紫装改造属性]:用户%s装备%s改造属性成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
				else
				{
					if(!modify.modify_attri(up_ob,base))
					{
						Zebra::logger->debug("[改造]:用户%s装备%s改造属性改造失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						//modify.re_upgrade(up_ob);
						Zebra::logger->debug("[改造属性]:用户%s装备%s改造属性成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
			}
			else if (check_npc(user, up_ob->base, NpcTrade::NPC_ZSUPDATE_OBJECT)) //sky 转生改造
			{
				if ( up_ob->data.kind & 16 )
				{
					if(!modify.modify_attri16(up_ob,base))
					{
						Zebra::logger->debug("[紫装转生改造属性]:用户%s装备%s改造属性失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						//modify.re_upgrade(up_ob);
						Zebra::logger->debug("[紫装转生改造属性]:用户%s装备%s改造属性成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
				else
				{
					if(!modify.modify_attri(up_ob,base))
					{
						Zebra::logger->debug("[转生改造]:用户%s装备%s改造属性改造失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						//modify.re_upgrade(up_ob);
						Zebra::logger->debug("[改造转生属性]:用户%s装备%s改造属性成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
			}
			else
			{
				Zebra::logger->debug("[改造]:用户%s改造装备%s失败,可能是npctrade填写有问题",user.charbase.name,up_ob->name);
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "您不能在此改造装备");
				return false;
			}
		} 
		else if (command->type==1)
		{
			if (check_npc(user, up_ob->base, NpcTrade::NPC_UPDATE_OBJECT)) //sky 改造
			{
				if ( up_ob->data.kind & 16 )
				{
					if(!modify.modify_rebind16(up_ob,base))
					{
						Zebra::logger->debug("[改造]:用户%s装备%s重新绑定属性改造失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
					Zebra::logger->debug("[重新绑定属性]:用户%s装备%s重新绑定属性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
				else
				{
					if(!modify.modify_rebind(up_ob,base))
					{
					   Zebra::logger->debug("[改造]:用户%s装备%s重新绑定属性改造失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
					   return false;
					}
					else
					Zebra::logger->debug("[重新绑定属性]:用户%s装备%s重新绑定属性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
			}
			else if (check_npc(user, up_ob->base, NpcTrade::NPC_ZSUPDATE_OBJECT)) //sky 转生改造
			{
				if ( up_ob->data.kind & 16 )
				{
					if(!modify.modify_rebind16(up_ob,base))
					{
						Zebra::logger->debug("[转生改造]:用户%s装备%s重新绑定属性改造失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						Zebra::logger->debug("[转生重新绑定属性]:用户%s装备%s重新绑定属性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
				else
				{
					if(!modify.modify_rebind(up_ob,base))
					{
						Zebra::logger->debug("[转生改造]:用户%s装备%s重新绑定属性改造失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						Zebra::logger->debug("[转生重新绑定属性]:用户%s装备%s重新绑定属性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
			}
			else
			{
				Zebra::logger->debug("[改造]:用户%s重新绑定%s失败,可能是npctrade填写有问题",user.charbase.name,up_ob->name);
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "您不能在此改造装备");
				return false;
			}
		}
		else if (command->type==2)
		{
			if (check_npc(user, up_ob->base, NpcTrade::NPC_UPDATE_OBJECT)) //sky 改造
			{
				if ( up_ob->data.maker[0]=='\0') 
				{
					Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "[改造]:%s 没有签名，请增加签名改造",up_ob->name);
					return false;
				}
				else
				{
					if(!modify.modify_remaker(up_ob,&user))
					{
					   Zebra::logger->debug("[改造]:用户%s装备%s修改签名改造失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
					   return false;
					}
					else
					Zebra::logger->debug("[修改签名]:用户%s装备%s修改签名改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
			}
			else if (check_npc(user, up_ob->base, NpcTrade::NPC_ZSUPDATE_OBJECT)) //sky 转生改造
			{
				if ( up_ob->data.maker[0]=='\0') 
				{
					Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "[转生改造]:%s 没有签名，请增加签名改造",up_ob->name);
					return false;
				}
				else
				{
					if(!modify.modify_remaker(up_ob,&user))
					{
						Zebra::logger->debug("[转生改造]:用户%s装备%s修改签名改造失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						Zebra::logger->debug("[转生修改签名]:用户%s装备%s修改签名改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
			}
			else
			{
				Zebra::logger->debug("[改造]:用户%s修改签名改造%s失败,可能是npctrade填写有问题",user.charbase.name,up_ob->name);
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "您不能在此改造装备");
				return false;
			}
		}
		else if (command->type==3)
		{
			if (check_npc(user, up_ob->base, NpcTrade::NPC_UPDATE_OBJECT)) //sky 改造
			{
				if ( up_ob->data.maker[0]=='\0') 
				{
					if(!modify.modify_addmaker(up_ob,&user))
					{
					   Zebra::logger->debug("[改造]:用户%s装备%s增加签名改造失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
					   return false;
					}
					else
					Zebra::logger->debug("[增加签名]:用户%s装备%s增加签名改造成功",user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
				else
				{
					Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "[改造]:%s 已有签名，不必重新增加签名",up_ob->name);
					return false;
				}
			}
			else if (check_npc(user, up_ob->base, NpcTrade::NPC_ZSUPDATE_OBJECT)) //sky 转生改造
			{
				if ( up_ob->data.maker[0]=='\0') 
				{
					if(!modify.modify_addmaker(up_ob,&user))
					{
						Zebra::logger->debug("[改造]:用户%s装备%s增加签名改造失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						Zebra::logger->debug("[增加签名]:用户%s装备%s增加签名改造成功",user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
				else
				{
					Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "[改造]:%s 已有签名，不必重新增加签名",up_ob->name);
					return false;
				}
			}
			else
			{
				Zebra::logger->debug("[改造]:用户%s签名改造%s失败,可能是npctrade填写有问题",user.charbase.name,up_ob->name);
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "您不能在此改造装备");
				return false;
			}
		}
		else if (command->type==4)
		{
			if (check_npc(user, up_ob->base, NpcTrade::NPC_UPDATE_OBJECT)) //sky 改造
			{
				if ( up_ob->data.kind & 16 || !up_ob->data.kind & 4)
				{
					if(!modify.modify_fivetype(up_ob))
					{
					   Zebra::logger->debug("[改造]:用户%s修改灵魂属性改造%s(紫)失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
					   return false;
					}
					else
					Zebra::logger->debug("[修改灵魂属性]:用户%s装备%s(紫)修改灵魂属性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
				else if ( up_ob->data.kind & 4 || !up_ob->data.kind & 16 )
				{
					if(!modify.modify_fivetype(up_ob))
					{
					   Zebra::logger->debug("[改造]:用户%s修改灵魂属性改造%s(绿装)失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
					   return false;
					}
					else
					Zebra::logger->debug("[修改灵魂属性]:用户%s装备%s(绿)修改灵魂属性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
			}
			else if (check_npc(user, up_ob->base, NpcTrade::NPC_ZSUPDATE_OBJECT)) //sky 转生改造
			{
				if ( up_ob->data.kind & 16 || !up_ob->data.kind & 4)
				{
					if(!modify.modify_fivetype(up_ob))
					{
						Zebra::logger->debug("[转生改造]:用户%s修改灵魂属性改造%s(紫)失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						Zebra::logger->debug("[转生修改灵魂属性]:用户%s装备%s(紫)修改灵魂属性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
				else if ( up_ob->data.kind & 4 || !up_ob->data.kind & 16 )
				{
					if(!modify.modify_fivetype(up_ob))
					{
						Zebra::logger->debug("[转生改造]:用户%s修改灵魂属性改造%s(绿装)失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						Zebra::logger->debug("[转生修改灵魂属性]:用户%s装备%s(绿)修改灵魂属性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
			}
			else
			{
				Zebra::logger->debug("[改造]:用户%s修改灵魂属性改造%s失败,可能是npctrade填写有问题",user.charbase.name,up_ob->name);
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "您不能在此改造装备");
				return false;
			}
		}
		else if (command->type==5)
		{
			if (check_npc(user, up_ob->base, NpcTrade::NPC_UPDATE_OBJECT)) //sky 改造
			{
				if ( up_ob->data.kind & 16 )
				{
					if(!modify.modify_soullinemd16(up_ob,base))
					{
					   Zebra::logger->debug("[改造]:用户%s保留灵魂锁链改造%s失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
					   return false;
					}
					else
					   //modify.re_upgrade(up_ob);
					   Zebra::logger->debug("[保留灵魂锁链改造]:用户%s装备%s保留灵魂锁链改造成功",user.charbase.name,up_ob->name);
					   Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					   user.addExp(10);
				}
				else
				{
					if(!modify.modify_soullinemd(up_ob,base))
					{
					   Zebra::logger->debug("[改造]:用户%s保留灵魂锁链改造%s失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
					   return false;
					}
					else
					   //modify.re_upgrade(up_ob);
					   Zebra::logger->debug("[保留灵魂锁链改造]:用户%s装备%s保留灵魂锁链改造成功", user.charbase.name,up_ob->name);
					   Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					   user.addExp(10);
				}
			}
			else if (check_npc(user, up_ob->base, NpcTrade::NPC_ZSUPDATE_OBJECT)) //sky 转生改造
			{
				if ( up_ob->data.kind & 16 )
				{
					if(!modify.modify_soullinemd16(up_ob,base))
					{
						Zebra::logger->debug("[转生改造]:用户%s保留灵魂锁链改造%s失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						//modify.re_upgrade(up_ob);
						Zebra::logger->debug("[转生保留灵魂锁链改造]:用户%s装备%s保留灵魂锁链改造成功",user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
				else
				{
					if(!modify.modify_soullinemd(up_ob,base))
					{
						Zebra::logger->debug("[转生改造]:用户%s保留灵魂锁链改造%s失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						//modify.re_upgrade(up_ob);
						Zebra::logger->debug("[转生保留灵魂锁链改造]:用户%s装备%s保留灵魂锁链改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
			}
			else
			{
				Zebra::logger->debug("[改造]:用户%s保留灵魂锁链改造%s失败,可能是npctrade填写有问题",user.charbase.name,up_ob->name);
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "您不能在此改造装备");
				return false;
			}
		}
		else if (command->type==6)
		{
			if (check_npc(user, up_ob->base, NpcTrade::NPC_UPDATE_OBJECT)) //sky 改造
			{
				if ( up_ob->data.kind & 16 )
				{
					if(!modify.modify_mdstwokilla16(up_ob,base))
					{
					   Zebra::logger->debug("[改造]:用户%s修改第二灵魂属性%s失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
					   return false;
					}
					else
					Zebra::logger->debug("[修改第二灵魂属性改造]:用户%s装备%s修改第二灵魂属性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
				else
				{
					Zebra::logger->debug("[改造]:非神圣祝福第二灵魂装备，不能在此改造");
					return true;	
				}
			}
			else if (check_npc(user, up_ob->base, NpcTrade::NPC_ZSUPDATE_OBJECT)) //sky 转生改造
			{
				if ( up_ob->data.kind & 16 )
				{
					if(!modify.modify_mdstwokilla16(up_ob,base))
					{
						Zebra::logger->debug("[转生改造]:用户%s修改第二灵魂属性%s失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						Zebra::logger->debug("[转生修改第二灵魂属性改造]:用户%s装备%s修改第二灵魂属性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
				else
				{
					Zebra::logger->debug("[转生改造]:非神圣祝福第二灵魂装备，不能在此改造");
					return true;	
				}
			}
			else
			{
				Zebra::logger->debug("[改造]:用户%s修改第二灵魂属性改造%s失败,可能是npctrade填写有问题",user.charbase.name,up_ob->name);
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "您不能在此改造装备");
				return false;
			}
		}
		else if (command->type==7)
		{
			if (check_npc(user, up_ob->base, NpcTrade::NPC_UPDATE_OBJECT)) //sky 改造
			{
				if ( up_ob->data.kind & 16 )
				{
					if(!modify.modify_mdskilla16(up_ob,base))
					{
					   Zebra::logger->debug("[改造]:用户%s修改技能属性%s失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
					   return false;
					}
					else
					Zebra::logger->debug("[修改技能属性改造]:用户%s装备%s修改技能属性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
				else 
				{
					if(!modify.modify_mdskilla(up_ob,base))
					{
					   Zebra::logger->debug("[改造]:用户%s修改技能属性%s失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
					   return false;
					}
					else
					Zebra::logger->debug("[修改技能属性改造]:用户%s装备%s修改技能属性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
			}
			else if (check_npc(user, up_ob->base, NpcTrade::NPC_ZSUPDATE_OBJECT)) //sky 转生改造
			{
				if ( up_ob->data.kind & 16 )
				{
					if(!modify.modify_mdskilla16(up_ob,base))
					{
						Zebra::logger->debug("[转生改造]:用户%s修改技能属性%s失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						Zebra::logger->debug("[转生修改技能属性改造]:用户%s装备%s修改技能属性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
				else 
				{
					if(!modify.modify_mdskilla(up_ob,base))
					{
						Zebra::logger->debug("[转生改造]:用户%s修改技能属性%s失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						Zebra::logger->debug("[转生修改技能属性改造]:用户%s装备%s修改技能属性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
			}
			else
			{
				Zebra::logger->debug("[改造]:用户%s修改技能属性改造%s失败,可能是npctrade填写有问题",user.charbase.name,up_ob->name);
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "您不能在此改造装备");
				return false;
			}
		}
		else if (command->type==8)
		{
			if (check_npc(user, up_ob->base, NpcTrade::NPC_UPDATE_OBJECT)) //sky 改造
			{
				if ( (up_ob->data.kind & 16 || up_ob->data.kind & 4) )
				{
					if(up_ob->data.round <= 0)
					{
					   up_ob->data.round =0;
					   up_ob->data.round = user.charbase.round;
					   Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					   user.addExp(10);
					}
					else
					{
					   Zebra::logger->debug("[改造]:当前装备已经是转生装备");
					   return false;
					}
				}
			}
			else
			{
				Zebra::logger->debug("[改造]:用户%s非转生装备%s改造转生装备失败,可能是npctrade填写有问题",user.charbase.name,up_ob->name);
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "您不能在此改造装备");
				return false;
			}
		}
		else if (command->type==9)
		{
			if (check_npc(user, up_ob->base, NpcTrade::NPC_UPDATE_OBJECT)) //sky 改造
			{
				if ( up_ob->data.kind & 16 )
				{
					if(!modify.modify_remakedz16(up_ob,base))
					{
						Zebra::logger->debug("[紫装马匹装备重新锻造]:用户%s马匹装备%s重新锻造改造失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
					Zebra::logger->debug("[紫装马匹装备重新锻造]:用户%s马匹装备%s重新锻造改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
				else
				{
					if(!modify.modify_remakedz(up_ob,base))
					{
					   Zebra::logger->debug("[马匹装备重新锻造]:用户%s马匹装备%s重新锻造改造失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
					   return false;
					}
					else
					Zebra::logger->debug("[马匹装备重新锻造]:用户%s马匹装备%s重新锻造性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
			}
			else if (check_npc(user, up_ob->base, NpcTrade::NPC_ZSUPDATE_OBJECT)) //sky 转生改造
			{
				if ( up_ob->data.kind & 16 )
				{
					if(!modify.modify_remakedz16(up_ob,base))
					{
						Zebra::logger->debug("[转生紫装马匹装备重新锻造]:用户%s马匹装备%s重新锻造改造失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						Zebra::logger->debug("转生[紫装马匹装备重新锻造]:用户%s马匹装备%s重新锻造改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
				else
				{
					if(!modify.modify_remakedz(up_ob,base))
					{
						Zebra::logger->debug("[转生马匹装备重新锻造]:用户%s马匹装备%s重新锻造改造失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						return false;
					}
					else
						Zebra::logger->debug("[转生马匹装备重新锻造]:用户%s马匹装备%s重新锻造性改造成功", user.charbase.name,up_ob->name);
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你获得了物品 %s ",up_ob->name);
					user.addExp(10);
				}
			}
			else
			{
				Zebra::logger->debug("[改造]:用户%s马匹装备%重新锻造改造失败,可能是npctrade填写有问题",user.charbase.name,up_ob->name);
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "您不能在此改造装备");
				return false;
			}
		}
		user.reduceObjectNum(command->gem_id, 2, command->gem_level);
		add_object(user, up_ob, false);
		response(user, 0, MODIFY);
	//	Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "恭喜你，改造装备成功！");
		return true;
	}

	return false;
}

//soke 是否可以改造
bool EquipMaker::can_modify(zObject * ob)
{
	if (ob->data.needlevel < 70 || !ob->data.bind || !((ob->data.kind & 16) || (ob->data.kind & 4))  )
	{
		return false;
	}
	return true;
}

//soke 紫装改造属性
int EquipMaker::modify_attri16(zObject* ob,zObjectB *objbase)
{
	ob->data.needlevel = objbase->needlevel;			// 需要等级

	ob->data.maxhp = objbase->maxhp;					// 最大生命值
	ob->data.maxmp = objbase->maxmp;					// 最大法术值
	ob->data.maxsp = objbase->maxsp;					// 最大体力值

	ob->data.pdamage = objbase->pdamage;				// 最小攻击力
	ob->data.maxpdamage = objbase->maxpdamage;			// 最大攻击力
	ob->data.mdamage = objbase->mdamage;				// 最小法术攻击力
	ob->data.maxmdamage = objbase->maxmdamage;			// 最大法术攻击力

	ob->data.pdefence = objbase->pdefence;				// 物防
	ob->data.mdefence = objbase->mdefence;				// 魔防
	ob->data.damagebonus = objbase->damagebonus;		// 伤害加成

	ob->data.akspeed = objbase->akspeed;				// 攻击速度
	ob->data.mvspeed = objbase->mvspeed;				// 移动速度
	ob->data.atrating = objbase->atrating;				// 命中率
	ob->data.akdodge = objbase->akdodge;				// 躲避率
	ob->data.bang = objbase->bang;
	ob->data.dur = objbase->durability;
	ob->data.maxdur = objbase->durability;
	ob->data.price = objbase->price;
	ob->data.cardpoint = objbase->cardpoint;

	zZhuanGoldObjectB* bob = NULL;

	int gold = odds_of_gold(objbase);
    if (zMisc::selectByTenTh(gold)||(ob->data.kind) & 2)
    {
		int holy = odds_of_holy(objbase->holyrating);
		int purple = odds_of_purple(objbase->holyrating/10);
		bool g_holy = zMisc::selectByTenTh(holy) || (ob->data.kind & 4);
		bool g_purple = zMisc::selectByMilh(purple) || (ob->data.kind & 16);
		if (g_holy)
		{
			int index = zMisc::randBetween(0, objbase->holys.size()-1);	
			bob = zhuangoldobjectbm.get(objbase->holys[index]);
		}
		int props = 0;
		if (holy > 3000*100) 
		{
			props = 18;
		} else if (holy > 900*100) 
		{
			props = 17;
		}
		else if (g_holy) 
		{
			props = 3;
		}
		else 
		{
			props = 2;
		}
		//如果是紫装 则100%完美 props就是用来计算品质的。
		if (g_purple)
		{
			props = 18;
		}
		if (strstr(ob->data.strName,"完美的"))
		{
			props = 18;
		}
		if(props>=5)
		{
			int i=0;
			int size=objbase->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, objbase->golds.size()-1);			
			}
			zZhuanGoldObjectB* bob1=NULL;
			bob1 = zhuangoldobjectbm.get(objbase->holys[i]);
			if(bob1)
			{
				modify_assign_color(bob1, ob, 2,props,objbase);
			}
			else
			{
				modify_assign_color(bob, ob, 2,props,objbase);
			}
		}
		else
		{
			modify_assign_color(bob, ob, 2,props,objbase);
		}
    //修复紫装多出1.4倍属性值问题
	//	if(g_holy) 
	//	{
	//		assign_holy(ob, holy);// 神圣装备
	//	}
		//soke  紫装
		if(g_purple) 
		{
			assign_purple(ob, holy);// 紫色装备
		}

		//soke 五行套装就是灵魂锁链。。。。。。如果所紫装 就生成两个灵魂	
		if ( (ob->data.fivetype != FIVE_NONE )|| ((ob->data.kind) & 8) || ((ob->data.kind) & 4) ||((ob->data.kind) & 16) )
		{
			assign_set(ob);	
		}

		bonus_hole(ob);
		fix(ob);
		return 1;

    }
	return 0;
}

//soke 绿装改造属性
int EquipMaker::modify_attri(zObject* ob,zObjectB *objbase)
{

	ob->data.needlevel = objbase->needlevel;			// 需要等级

	ob->data.maxhp = objbase->maxhp;					// 最大生命值
	ob->data.maxmp = objbase->maxmp;					// 最大法术值
	ob->data.maxsp = objbase->maxsp;					// 最大体力值

	ob->data.pdamage = objbase->pdamage;				// 最小攻击力
	ob->data.maxpdamage = objbase->maxpdamage;			// 最大攻击力
	ob->data.mdamage = objbase->mdamage;				// 最小法术攻击力
	ob->data.maxmdamage = objbase->maxmdamage;			// 最大法术攻击力

	ob->data.pdefence = objbase->pdefence;				// 物防
	ob->data.mdefence = objbase->mdefence;				// 魔防
	ob->data.damagebonus = objbase->damagebonus;		// 伤害加成

	ob->data.akspeed = objbase->akspeed;				// 攻击速度
	ob->data.mvspeed = objbase->mvspeed;				// 移动速度
	ob->data.atrating = objbase->atrating;				// 命中率
	ob->data.akdodge = objbase->akdodge;				// 躲避率
	ob->data.bang = objbase->bang;
	ob->data.dur = objbase->durability;
	ob->data.maxdur = objbase->durability;
	ob->data.price = objbase->price;
	ob->data.cardpoint = objbase->cardpoint;

	zGoldObjectB* bob = NULL;

	int gold = odds_of_gold(objbase);
    if (zMisc::selectByTenTh(gold)||(ob->data.kind) & 2)
    {
		int holy = odds_of_holy(objbase->holyrating);
		//int purple = odds_of_purple(objbase->holyrating/10);
		bool g_holy = zMisc::selectByTenTh(holy) || (ob->data.kind & 4);
		//bool g_purple = zMisc::selectByMilh(purple) || (ob->data.kind & 16);
		if (g_holy)
		{
			int index = zMisc::randBetween(0, objbase->holys.size()-1);	
			bob = goldobjectbm.get(objbase->holys[index]);
		}
		int props = 0;
		if (holy > 3000*100) 
		{
			props = 18;
		} 
		else if (holy > 900*100) 
		{
			props = 17;
		}
		else if (g_holy) 
		{
			props = 3;
		}
		else 
		{
			props = 2;
		}
		//如果是紫装 则100%完美 props就是用来计算品质的。
		/* if (g_purple)
		{
			props = 18;
		} */
		if (strstr(ob->data.strName,"完美的"))
		{
			props = 18;
		}
		if(props>=5)
		{
			int i=0;
			int size=objbase->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, objbase->golds.size()-1);			
			}
			zGoldObjectB* bob1=NULL;
			bob1 = goldobjectbm.get(objbase->holys[i]);
			if(bob1)
			{
				modify_assign_color(bob1, ob, 2,props,objbase);
			}
			else
			{
				modify_assign_color(bob, ob, 2,props,objbase);
			}
		}
		else
		{
			modify_assign_color(bob, ob, 2,props,objbase);
		}
		if(g_holy) 
		{
			assign_holy(ob, holy);// 神圣装备
		}
		//soke  紫装
		/* if(g_purple) 
		{
			assign_purple(ob, holy);// 紫色装备
		} */

		//soke 五行套装就是灵魂锁链。。。。。。如果所紫装 就生成两个灵魂	
		if ( (ob->data.fivetype != FIVE_NONE )|| ((ob->data.kind) & 8) || ((ob->data.kind) & 4) ||((ob->data.kind) & 16) )
		{
			assign_set(ob);	
		}

		bonus_hole(ob);
		fix(ob);
		return 1;

    }
	return 0;
}

//soke 紫装重新绑定
int EquipMaker::modify_rebind16(zObject* ob,zObjectB *objbase)
{
	zZhuanGoldObjectB* bob = NULL;
	int gold = odds_of_gold(objbase);
	if (zMisc::selectByTenTh(gold)||(ob->data.kind) & 2)
	{
		int holy = odds_of_holy(objbase->holyrating);
		int purple = odds_of_purple(objbase->holyrating/10);
		bool g_holy = zMisc::selectByTenTh(holy) || (ob->data.kind & 4);
		bool g_purple = zMisc::selectByMilh(purple) || (ob->data.kind & 16);
		if (g_holy)
		{
			int index = zMisc::randBetween(0, objbase->holys.size()-1);	
			bob = zhuangoldobjectbm.get(objbase->holys[index]);
		}
		int props = 0;
		if (holy > 3000*100) 
		{
			props = 18;
		} 
		else if (holy > 900*100) 
		{
			props = 17;
		}
		else if (g_holy) 
		{
			props = 3;
		}
		else 
		{
			props = 2;
		}
		//如果是紫装 则100%完美 props就是用来计算品质的。
		if (g_purple)
		{
			props = 18;
		}
		if (strstr(ob->data.strName,"卓越的"))
		{
			props = 18;
		}
		if(props>=5)
		{
			int i=0;
			int size=objbase->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, objbase->golds.size()-1);			
			}
			zZhuanGoldObjectB* bob1=NULL;
			bob1 = zhuangoldobjectbm.get(objbase->holys[i]);
			if(bob1)
			{
				modify_assign_bind(bob1, ob, 2,props,objbase);
			}
			else
			{
				modify_assign_bind(bob, ob, 2,props,objbase);
			}
		}
		else
		{
			modify_assign_bind(bob, ob, 2,props,objbase);
		}
		return 1;
	}
	return 0;
}

//soke 绿装重新绑定
int EquipMaker::modify_rebind(zObject* ob,zObjectB *objbase)
{
	zGoldObjectB* bob = NULL;
	int gold = odds_of_gold(objbase);
	if (zMisc::selectByTenTh(gold)||(ob->data.kind) & 2)
	{
		int holy = odds_of_holy(objbase->holyrating);
		int purple = odds_of_purple(objbase->holyrating/10);
		bool g_holy = zMisc::selectByTenTh(holy) || (ob->data.kind & 4);
		bool g_purple = zMisc::selectByMilh(purple) || (ob->data.kind & 16);
		if (g_holy)
		{
			int index = zMisc::randBetween(0, objbase->holys.size()-1);	
			bob = goldobjectbm.get(objbase->holys[index]);
		}
		int props = 0;
		if (holy > 3000*100) 
		{
			props = 18;
		} 
		else if (holy > 900*100) 
		{
			props = 17;
		}
		else if (g_holy) 
		{
			props = 3;
		}
		else 
		{
			props = 2;
		}
		//如果是紫装 则100%完美 props就是用来计算品质的。
		if (g_purple)
		{
			props = 18;
		}
		if (strstr(ob->data.strName,"完美的"))
		{
			props = 18;
		}
		if(props>=5)
		{
			int i=0;
			int size=objbase->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, objbase->golds.size()-1);			
			}
			zGoldObjectB* bob1=NULL;
			bob1 = goldobjectbm.get(objbase->holys[i]);
			if(bob1)
			{
				modify_assign_bind(bob1, ob, 2,props,objbase);
			}
			else
			{
				modify_assign_bind(bob, ob, 2,props,objbase);
			}
		}
		else
		{
			modify_assign_bind(bob, ob, 2,props,objbase);
		}
		return 1;
	}
	return 0;
}

//soke 重新签名
int EquipMaker::modify_remaker(zObject* ob,SceneUser* user)
{
	if (user) 
	{
		memset(ob->data.maker,0,MAX_NAMESIZE);
		strncpy(ob->data.maker, user->name, MAX_NAMESIZE);//打造者名字
	}
	return 1;
}

//soke 添加签名
int EquipMaker::modify_addmaker(zObject* ob,SceneUser* user)
{
	if (user) 
	{
		memset(ob->data.maker,0,MAX_NAMESIZE);
		strncpy(ob->data.maker, user->name, MAX_NAMESIZE);//打造者名字
	}
	return 1;
}

//soke 紫装修改灵魂属性
int EquipMaker::modify_soula16(zObject* ob,zObjectB *objbase)
{
	//soke 灵魂清零，不然只能物减改造
	ob->data.addpdef = 0;
	ob->data.addmdef = 0;
	ob->data.addpdam = 0;
	ob->data.addmdam = 0;
	ob->data.addhp = 0;
	
	if(ob->data.fivetype != FIVE_NONE)
	{
		ob->data.fivetype = zMisc::randBetween(0, 4);

			int index = zMisc::randBetween(0,4);

			switch (index)
			{
			case 0:
				{
					ob->data.addpdef = 60; 
				}
				break;	
			case 1:
				{
					ob->data.addmdef = 60;
				}
				break;	

			case 2:
				{
					ob->data.addpdam = 120;
				}
				break;	

			case 3:
				{
					ob->data.addmdam = 120;
				}
				break;	

			case 4:
				{
					ob->data.addhp = 100;
				}
				break;	

			default:
				break;		
			}
	}
	return 1;
}

//soke 绿装修改灵魂属性
int EquipMaker::modify_soula(zObject* ob,zObjectB *objbase)
{
	zSetObjectB *base = setobjectbm.get(ob->data.dwObjectID);
	if (base==NULL) return false;

	zFiveSetB* prop = fivesetbm.get(base->mark);
	if (!prop) return false;

	//醉梦 修复修改五行变灵魂

	//soke 灵魂清零，不然只能物减改造
	// ob->data.dpdam = 0;
	// ob->data.dmdam = 0;
	// ob->data.bdam = 0;
	// ob->data.rdam = 0;
	// ob->data.ignoredef = 0;

	if(ob->data.fivetype != FIVE_NONE)
	{
		ob->data.fivetype = zMisc::randBetween(0, 4);

			int index = zMisc::randBetween(0,4);

			switch (index)
			{
			case 0:
				{
					ob->data.dpdam = zMisc::randBetween(prop->dpdam.min, prop->dpdam.max);
				}
				break;	
			case 1:
				{
					ob->data.dmdam = zMisc::randBetween(prop->dmdam.min, prop->dmdam.max);
				}
				break;	

			case 2:
				{
					ob->data.bdam = zMisc::randBetween(prop->bdam.min, prop->bdam.max);
				}
				break;	

			case 3:
				{
					ob->data.rdam = zMisc::randBetween(prop->rdam.min, prop->rdam.max);
				}
				break;	

			case 4:
				{
					ob->data.ignoredef = zMisc::randBetween(prop->ignoredef.min, prop->rdam.max);
				}
				break;	

			default:
				break;		
			}
	}
	return 1;
}

// 修改五行属性
int EquipMaker::modify_fivetype(zObject* ob)
{
	if(ob->data.fivetype != FIVE_NONE)
	{
		ob->data.fivetype = zMisc::randBetween(0, 4);
		return 1;
	}
	return false;
}

//soke 修改第二灵魂属性
int EquipMaker::modify_mdstwokilla16(zObject* ob,zObjectB *objbase)
{
	//soke 灵魂清零，不然只能物减改造
	ob->data.addpdef = 0;
	ob->data.addmdef = 0;
	ob->data.addpdam = 0;
	ob->data.addmdam = 0;
	ob->data.addhp = 0;
	
	if(ob->data.fivetype != FIVE_NONE)
	{
		
		//醉梦 修改第二灵魂 取消五行更改
		//ob->data.fivetype = zMisc::randBetween(0, 4);

			int index = zMisc::randBetween(0,4);

			switch (index)
			{
			case 0:
				{
					ob->data.addpdef = 60; 
				}
				break;	
			case 1:
				{
					ob->data.addmdef = 60;
				}
				break;	

			case 2:
				{
					ob->data.addpdam = 120;
				}
				break;	

			case 3:
				{
					ob->data.addmdam = 120;
				}
				break;	

			case 4:
				{
					ob->data.addhp = 100;
				}
				break;	

			default:
				break;		
			}
	}
	return 1;
}

//soke 紫装保留灵魂锁链的改造
int EquipMaker::modify_soullinemd16(zObject* ob,zObjectB *objbase)
{
	ob->data.needlevel = objbase->needlevel;			// 需要等级

	ob->data.maxhp = objbase->maxhp;					// 最大生命值
	ob->data.maxmp = objbase->maxmp;					// 最大法术值
	ob->data.maxsp = objbase->maxsp;					// 最大体力值

	ob->data.pdamage = objbase->pdamage;				// 最小攻击力
	ob->data.maxpdamage = objbase->maxpdamage;			// 最大攻击力
	ob->data.mdamage = objbase->mdamage;				// 最小法术攻击力
	ob->data.maxmdamage = objbase->maxmdamage;			// 最大法术攻击力

	ob->data.pdefence = objbase->pdefence;				// 物防
	ob->data.mdefence = objbase->mdefence;				// 魔防
	ob->data.damagebonus = objbase->damagebonus;		// 伤害加成

	ob->data.akspeed = objbase->akspeed;				// 攻击速度
	ob->data.mvspeed = objbase->mvspeed;				// 移动速度
	ob->data.atrating = objbase->atrating;				// 命中率
	ob->data.akdodge = objbase->akdodge;				// 躲避率
	ob->data.bang = objbase->bang;
	ob->data.dur = objbase->durability;
	ob->data.maxdur = objbase->durability;
	ob->data.price = objbase->price;
	ob->data.cardpoint = objbase->cardpoint;

	zZhuanGoldObjectB* bob = NULL;
	if(ob->data.fivetype == FIVE_NONE)
		return 0;
	int gold = odds_of_gold(objbase);
	if (zMisc::selectByTenTh(gold)||(ob->data.kind) & 2)
	{
		int holy = odds_of_holy(objbase->holyrating);
		int purple = odds_of_purple(objbase->holyrating/10);
		bool g_holy = zMisc::selectByTenTh(holy) || (ob->data.kind & 4);
		bool g_purple = zMisc::selectByMilh(purple) || (ob->data.kind & 16);
		if (g_holy)
		{
			int index = zMisc::randBetween(0, objbase->holys.size()-1);	
			bob = zhuangoldobjectbm.get(objbase->holys[index]);
		}
		int props = 0;
		if (holy > 3000*100) 
		{
			props = 18;
		} else if (holy > 900*100) 
		{
			props = 17;
		}
		else if (g_holy) 
		{
			props = 3;
		}
		else 
		{
			props = 2;
		}
		//如果是紫装 则100%完美 props就是用来计算品质的。
		if (g_purple)
		{
			props = 18;
		}
		if (strstr(ob->data.strName,"卓越的"))
		{
			props = 18;
		}
		if(props>=5)
		{
			int i=0;
			int size=objbase->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, objbase->golds.size()-1);			
			}
			zZhuanGoldObjectB* bob1=NULL;
			bob1 = zhuangoldobjectbm.get(objbase->holys[i]);
			if(bob1)
			{
				modify_assign_color(bob1, ob, 2,props,objbase,false,false);
			}
			else
			{
				modify_assign_color(bob, ob, 2,props,objbase,false,false);
			}
		}
		else
		{
			modify_assign_color(bob, ob, 2,props,objbase,false,false);
		}
    //修复紫装多出1.4倍属性值问题
	//	if(g_holy) 
	//	{
	//		assign_holy(ob, holy);// 神圣装备
	//	}
		if(g_purple) 
		{
			assign_purple(ob, holy);// 紫色装备
		}

		return 1;
	}
	return 0;
}

//soke 绿装保留灵魂锁链的改造
int EquipMaker::modify_soullinemd(zObject* ob,zObjectB *objbase)
{
	ob->data.needlevel = objbase->needlevel;			// 需要等级

	ob->data.maxhp = objbase->maxhp;					// 最大生命值
	ob->data.maxmp = objbase->maxmp;					// 最大法术值
	ob->data.maxsp = objbase->maxsp;					// 最大体力值

	ob->data.pdamage = objbase->pdamage;				// 最小攻击力
	ob->data.maxpdamage = objbase->maxpdamage;			// 最大攻击力
	ob->data.mdamage = objbase->mdamage;				// 最小法术攻击力
	ob->data.maxmdamage = objbase->maxmdamage;			// 最大法术攻击力

	ob->data.pdefence = objbase->pdefence;				// 物防
	ob->data.mdefence = objbase->mdefence;				// 魔防
	ob->data.damagebonus = objbase->damagebonus;		// 伤害加成

	ob->data.akspeed = objbase->akspeed;				// 攻击速度
	ob->data.mvspeed = objbase->mvspeed;				// 移动速度
	ob->data.atrating = objbase->atrating;				// 命中率
	ob->data.akdodge = objbase->akdodge;				// 躲避率
	ob->data.bang = objbase->bang;
	ob->data.dur = objbase->durability;
	ob->data.maxdur = objbase->durability;
	ob->data.price = objbase->price;
	ob->data.cardpoint = objbase->cardpoint;

	zGoldObjectB* bob = NULL;
	if(ob->data.fivetype == FIVE_NONE)
		return 0;
	int gold = odds_of_gold(objbase);
	if (zMisc::selectByTenTh(gold)||(ob->data.kind) & 2)
	{
		int holy = odds_of_holy(objbase->holyrating);
		int purple = odds_of_purple(objbase->holyrating/10);
		bool g_holy = zMisc::selectByTenTh(holy) || (ob->data.kind & 4);
		bool g_purple = zMisc::selectByMilh(purple) || (ob->data.kind & 16);
		if (g_holy)
		{
			int index = zMisc::randBetween(0, objbase->holys.size()-1);	
			bob = goldobjectbm.get(objbase->holys[index]);
		}
		int props = 0;
		if (holy > 3000*100) 
		{
			props = 18;
		} else if (holy > 900*100) 
		{
			props = 17;
		}
		else if (g_holy) 
		{
			props = 3;
		}
		else 
		{
			props = 2;
		}
		//如果是紫装 则100%完美 props就是用来计算品质的。
		if (g_purple)
		{
			props = 18;
		}
		if (strstr(ob->data.strName,"完美的"))
		{
			props = 18;
		}
		if(props>=5)
		{
			int i=0;
			int size=objbase->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, objbase->golds.size()-1);			
			}
			zGoldObjectB* bob1=NULL;
			bob1 = goldobjectbm.get(objbase->holys[i]);
			if(bob1)
			{
				modify_assign_color(bob1, ob, 2,props,objbase,false,false);
			}
			else
			{
				modify_assign_color(bob, ob, 2,props,objbase,false,false);
			}
		}
		else
		{
			modify_assign_color(bob, ob, 2,props,objbase,false,false);
		}

		if(g_holy) 
		{
			assign_holy(ob, holy);// 神圣装备
		}
		/* if(g_purple) 
		{
			assign_purple(ob, holy);// 紫色装备
		} */

		return 1;
	}
	return 0;
}

//soke 紫装技能属性
int EquipMaker::modify_mdskilla16(zObject* ob,zObjectB *objbase)
{
	zZhuanGoldObjectB* bob = NULL;
	int gold = odds_of_gold(objbase);
	if (zMisc::selectByTenTh(gold)||(ob->data.kind) & 2)
	{
		int holy = odds_of_holy(objbase->holyrating);
		int purple = odds_of_purple(objbase->holyrating/10);
		bool g_holy = zMisc::selectByTenTh(holy) || (ob->data.kind & 4);
		bool g_purple = zMisc::selectByMilh(purple) || (ob->data.kind & 16);
		if (g_holy)
		{
			int index = zMisc::randBetween(0, objbase->holys.size()-1);	
			bob = zhuangoldobjectbm.get(objbase->holys[index]);
		}
		int props = 0;
		if (holy > 3000*100) 
		{
			props = 18;
		} 
		else if (holy > 900*100) 
		{
			props = 17;
		}
		else if (g_holy) 
		{
			props = 3;
		}
		else 
		{
			props = 2;
		}
		//如果是紫装 则100%完美 props就是用来计算品质的。
		if (g_purple)
		{
			props = 18;
		}
		if (strstr(ob->data.strName,"卓越的"))
		{
			props = 18;
		}
		if(props>=5)
		{
			int i=0;
			int size=objbase->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, objbase->golds.size()-1);			
			}
			zZhuanGoldObjectB* bob1=NULL;
			bob1 = zhuangoldobjectbm.get(objbase->holys[i]);
			if(bob1)
			{
				modify_assign_skill(bob1, ob, 2,props,objbase);
			}
			else
			{
				modify_assign_skill(bob, ob, 2,props,objbase);
			}
		}
		else
		{
			modify_assign_skill(bob, ob, 2,props,objbase);
		}
		return 1;
	}
	return 0;
}

//soke 绿装技能属性
int EquipMaker::modify_mdskilla(zObject* ob,zObjectB *objbase)
{
	zGoldObjectB* bob = NULL;
	int gold = odds_of_gold(objbase);
	if (zMisc::selectByTenTh(gold)||(ob->data.kind) & 2)
	{
		int holy = odds_of_holy(objbase->holyrating);
		int purple = odds_of_purple(objbase->holyrating/10);
		bool g_holy = zMisc::selectByTenTh(holy) || (ob->data.kind & 4);
		bool g_purple = zMisc::selectByMilh(purple) || (ob->data.kind & 16);
		if (g_holy)
		{
			int index = zMisc::randBetween(0, objbase->holys.size()-1);	
			bob = goldobjectbm.get(objbase->holys[index]);
		}
		int props = 0;
		if (holy > 3000*100) 
		{
			props = 18;
		} 
		else if (holy > 900*100) 
		{
			props = 17;
		}
		else if (g_holy) 
		{
			props = 3;
		}
		else 
		{
			props = 2;
		}
		//如果是紫装 则100%完美 props就是用来计算品质的。
		if (g_purple)
		{
			props = 18;
		}
		if (strstr(ob->data.strName,"完美的"))
		{
			props = 18;
		}
		if(props>=5)
		{
			int i=0;
			int size=objbase->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, objbase->golds.size()-1);			
			}
			zGoldObjectB* bob1=NULL;
			bob1 = goldobjectbm.get(objbase->holys[i]);
			if(bob1)
			{
				modify_assign_skill(bob1, ob, 2,props,objbase);
			}
			else
			{
				modify_assign_skill(bob, ob, 2,props,objbase);
			}
		}
		else
		{
			modify_assign_skill(bob, ob, 2,props,objbase);
		}
		return 1;
	}
	return 0;
}

//soke 紫色马匹装备重新锻造
int EquipMaker::modify_remakedz16(zObject* ob,zObjectB *objbase)
{
	if(ob->data.fivetype == FIVE_NONE)
	{
		ob->data.fivetype = zMisc::randBetween(0, 4);
	}

	zZhuanGoldObjectB* bob = NULL;
	int gold = odds_of_gold(objbase);
	if (zMisc::selectByTenTh(gold)||(ob->data.kind) & 2)
	{
		int holy = odds_of_holy(objbase->holyrating);
		int purple = odds_of_purple(objbase->holyrating/10);
		bool g_holy = zMisc::selectByTenTh(holy) || (ob->data.kind & 4);
		bool g_purple = zMisc::selectByMilh(purple) || (ob->data.kind & 16);
		if (g_holy)
		{
			int index = zMisc::randBetween(0, objbase->holys.size()-1);	
			bob = zhuangoldobjectbm.get(objbase->holys[index]);
		}
		int props = 0;
		if (holy > 3000*100) 
		{
			props = 18;
		} 
		else if (holy > 900*100) 
		{
			props = 17;
		}
		else if (g_holy) 
		{
			props = 3;
		}
		else 
		{
			props = 2;
		}
		//如果是紫装 则100%完美 props就是用来计算品质的。
		if (g_purple)
		{
			props = 18;
		}
		if (strstr(ob->data.strName,"卓越的"))
		{
			props = 18;
		}
		if(props>=5)
		{
			int i=0;
			int size=objbase->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, objbase->golds.size()-1);			
			}
			zZhuanGoldObjectB* bob1=NULL;
			bob1 = zhuangoldobjectbm.get(objbase->holys[i]);
			if(bob1)
			{
				modify_assign_makedz(bob1, ob, 2,props,objbase);
			}
			else
			{
				modify_assign_makedz(bob, ob, 2,props,objbase);
			}
		}
		else
		{
			modify_assign_makedz(bob, ob, 2,props,objbase);
		}
		return 1;
	}
	return 0;
}

//soke 绿色马匹装备重新锻造
int EquipMaker::modify_remakedz(zObject* ob,zObjectB *objbase)
{
	if(ob->data.fivetype == FIVE_NONE)
	{
		ob->data.fivetype = zMisc::randBetween(0, 4);
	}

	zGoldObjectB* bob = NULL;
	int gold = odds_of_gold(objbase);
	if (zMisc::selectByTenTh(gold)||(ob->data.kind) & 2)
	{
		int holy = odds_of_holy(objbase->holyrating);
		int purple = odds_of_purple(objbase->holyrating/10);
		bool g_holy = zMisc::selectByTenTh(holy) || (ob->data.kind & 4);
		bool g_purple = zMisc::selectByMilh(purple) || (ob->data.kind & 16);
		if (g_holy)
		{
			int index = zMisc::randBetween(0, objbase->holys.size()-1);	
			bob = goldobjectbm.get(objbase->holys[index]);
		}
		int props = 0;
		if (holy > 3000*100) 
		{
			props = 18;
		} 
		else if (holy > 900*100) 
		{
			props = 17;
		}
		else if (g_holy) 
		{
			props = 3;
		}
		else 
		{
			props = 2;
		}
		//如果是紫装 则100%完美 props就是用来计算品质的。
		if (g_purple)
		{
			props = 18;
		}
		if (strstr(ob->data.strName,"完美的"))
		{
			props = 18;
		}
		if(props>=5)
		{
			int i=0;
			int size=objbase->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, objbase->golds.size()-1);			
			}
			zGoldObjectB* bob1=NULL;
			bob1 = goldobjectbm.get(objbase->holys[i]);
			if(bob1)
			{
				modify_assign_makedz(bob1, ob, 2,props,objbase);
			}
			else
			{
				modify_assign_makedz(bob, ob, 2,props,objbase);
			}
		}
		else
		{
			modify_assign_makedz(bob, ob, 2,props,objbase);
		}
		return 1;
	}
	return 0;
}

#undef COMPUTE
#define COMPUTE(x) ob->data.x = static_cast<WORD>(ob->data.x*1.4);

//soke 神圣装备升级改造
bool RebuildObject::makelv(SceneUser& user, const Cmd::stPropertyUserCmd* cmd)
{
	Cmd::stFoundItemPropertyUserCmd * command = (Cmd::stFoundItemPropertyUserCmd *)cmd;

	zObject *up_ob = user.packs.uom.getObjectByThisID(command->up_id);
	if (!up_ob)
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "升级失败，没有找到要升级的装备，您需要重新打开对话框进行装备升级");
		return false; 
	}
	zObjectB *base = objectbm.get(up_ob->data.dwObjectID);
	if (!up_ob||!base)
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，没有找到要升级的装备");
		return false;
	}
	
	if (!up_ob || !EquipMaker::can_makelv(up_ob)) 
	{
		Zebra::logger->debug("用户%s请求对不能对该装备升级改造", user.charbase.name);
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "装备升级失败，要升级的装备必须是70级并且绑定的绿装或紫装");
		return false;			
	}

	if(command->count < 1)
		return false;

	if(command->count == 1 && up_ob->data.needlevel > 160)
		return false;

	   //soke 效验NPC访问是否合法
	  /* if (!check_npc(user, up_ob->base, NpcTrade::NPC_MAKE_OBJECT)) 
	   {
		   Zebra::logger->debug("[改造]:用户%s装备升级%s失败,可能是npctrade填写有问题",user.charbase.name,up_ob->name);
		   Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "您不能在此升级装备");
		   return false;
	   }*/
//	}

	int dxslevel = 0;
	if (up_ob->data.kind & 16)
		dxslevel = 5;
	else if (up_ob->data.kind & 4)
		dxslevel = 5;
	else
		return false;
	int count = -1;

	if (!user.packs.uom.exist(MAKELVTONE, MAKELVNUM, dxslevel)) 
			return false;
	while ( ++count < (int)command->count) 
    {
		if(command->list[count].gem_id == MAKELVTONE)
		{
			if(dxslevel != command->list[count].gem_level) return false;
		}else
		{
			int index  = base->need_material.stuffs.size() - 1;
			if(base->need_material.stuffs[index].id != command->list[count].gem_id || base->need_material.stuffs[index].level != command->list[count].gem_level)
				return false;
		}
		if (!user.packs.uom.exist(command->list[count].gem_id, command->list[count].gem_num, command->list[count].gem_level)) 
			return false;
	}

/* 	zObject* ob = user.packs.uom.getObjectByID(command->gem_id, command->gem_level, true);
	if (!ob || ob->data.dwObjectID != MAKELVTONE) 
	{
		Zebra::logger->debug("[升级改造]:用户%s对装备升级改造时缺少材料", user.charbase.name);
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "升级改造材料不足");
		return false;			
	} */

	DWORD gold = MAKELV_MONEY;

	DWORD taxMoney = (DWORD)((gold*(user.scene->getTax())/100.0f)+0.5f); // 买东西收税
	gold = gold + taxMoney;
	Cmd::Session::t_taxAddCountry_SceneSession send;
	send.dwCountryID = user.scene->getCountryID();
	send.qwTaxMoney = taxMoney;
	sessionClient->sendCmd(&send, sizeof(send));

	if (!user.packs.checkMoney(gold)) 
	{
		Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "银子不足");
		return false;
	}


	EquipMaker makelv(&user);
	if ( user.packs.removeMoney(gold,"改造") ) 
	{

					if (up_ob->data.needlevel >= 158 && up_ob->data.round == 0)
					{
						Zebra::logger->debug("[改造]:用户%s升级改造%s失败,装备需要转生后才可继续升级.", user.charbase.name,up_ob->name);
						Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "升级失败!当前装备需要转生后才可继续升级!");
						Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "升级失败!当前装备需要转生后才可继续升级!");
						return false;
					}
					
					int up_id = makelv.makelv_upgrade(up_ob);
					if(up_id == 0)
					{
						Zebra::logger->debug("[改造]:用户%s升级改造%s失败,可能是npctrade填写有问题", user.charbase.name,up_ob->name);
						Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "升级失败!当前装备已经不能在此升级!");
						return false;
					}
					zObjectB *new_base = objectbm.get(up_id);
					new_base->goldrating = 10000;
					new_base->holyrating = 100000;
					if (!new_base)
					{
						Zebra::logger->debug("[改造]:用户%s升级改造%s失败,可能是升级后的物品不存在", user.charbase.name,up_ob->name);
						return false;
					}
					
					Zebra::logger->debug("升级后ID:%d",new_base->id);
					zObject *new_ob = zObject::create(new_base,1);
					//zObject *new_ob;
					if(check_npc(user, up_ob->base, NpcTrade::NPC_MAKE_OBJECT) || check_npc(user, up_ob->base, NpcTrade::NPC_ZSMAKE_OBJECT))
					{
						if (up_ob->data.kind & 16)
						{
							EquipMaker maker(NULL);
							//makelv.assign1(NULL, new_ob, new_base,false,22);
							new_ob->data.round = up_ob->data.round;
							new_ob->data.kind = 16;
							maker.modify_attri16(new_ob,new_base);
							Zebra::logger->debug("升级紫装");
						}
						else
						{
							EquipMaker maker(NULL);
							//makelv.assign(NULL, new_ob, new_base,false,22);
							new_ob->data.round = up_ob->data.round;
							new_ob->data.kind = 4;
							maker.modify_attri(new_ob,new_base);
							Zebra::logger->debug("升级绿装");
						}
						makelv.modify_addmaker(new_ob,&user);
						makelv.upgrade_inherit(user,new_ob,up_ob);
						user.packs.removeObject(up_ob,true,true);
						user.packs.addObject(new_ob,true,1);
						Base::add_object(user, new_ob, false);
						Zebra::logger->debug("升级完成");
					}else
					{
						Zebra::logger->debug("[改造]:用户%s装备升级%s失败,可能是npctrade填写有问题",user.charbase.name,new_ob->name);
						Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "您不能在此升级装备");
						return false;
					}

/* 					if (check_npc(user, up_ob->base, NpcTrade::NPC_MAKE_OBJECT)) 
					{
						if (up_ob->data.kind & 16)
						{
							makelv.repur2_upgrade(up_ob);
						}
						else
						{
							makelv.re2_upgrade(up_ob);
						}
					}
					else if (check_npc(user, up_ob->base, NpcTrade::NPC_ZSMAKE_OBJECT)) 
					{
						if (up_ob->data.kind & 16)
						{
							//makelv.repur2_upgrade(up_ob);
							EquipMaker makelv2(NULL);
							makelv2.assign1(NULL, new_ob, new_base,false,22);
							user.packs.removeObject(up_ob,true,true);
							makelv2.modify_attri16(new_ob,new_base);
							user.packs.addObject(new_ob,true,1);
							Base::add_object(user, new_ob, false);
							Zebra::logger->debug("紫装升级");
						}
						else
						{
							Zebra::logger->debug("装备信息:%d,%d,%d,%d",up_ob->data.kind,up_ob->data.bind,up_ob->data.upgrade,up_ob->data.round);
							new_ob->data.kind = up_ob->data.kind; //设置品质4绿装
							//new_ob->data.kind = 16; //设置品质4绿装
							new_ob->data.bind = up_ob->data.bind;
							new_ob->data.upgrade = up_ob->data.upgrade;
							new_ob->data.round = up_ob->data.round;
							new_base->goldrating = 10000;
							new_base->holyrating = 100000;
							EquipMaker makelv2(NULL);
							//makelv.modify_addmaker(new_ob,&user);
							makelv2.modify_attri(new_ob,new_base);
							makelv2.modify_addmaker(new_ob,&user);
							makelv2.re_upgrade(new_ob);
							user.packs.removeObject(up_ob , true,true);
							user.packs.addObject(new_ob,true,1);
							add_object(user, new_ob, false);

						}
					}
					else
					{
						Zebra::logger->debug("[改造]:用户%s装备升级%s失败,可能是npctrade填写有问题",user.charbase.name,new_ob->name);
						Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "您不能在此升级装备");
						return false;
					}
 */				   //makelv.upgrade_attri(up_ob,base);
		           Zebra::logger->debug("[装备升级]:用户%s装备%s升级改造成功", user.charbase.name,new_ob->name);
			       Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "装备%s升级成功，你可以改造装备了 ",new_ob->name);
				   //Channel::sendSys(&user, Cmd::INFO_TYPE_MSG, "装备升级提示：按Esc键→ 角色选择→ 确定→ 进入游戏→ 进行装备改造属性！ ");
			       user.addExp(10);
				
		//扣除材料
		user.reduceObjectNum(MAKELVTONE, MAKELVNUM, dxslevel);
		int count = -1;
		while ( ++count < (int)command->count) 
		{
			if(command->list[count].gem_id == MAKELVTONE)
				continue;
			else
				user.reduceObjectNum(command->list[count].gem_id, command->list[count].gem_num, command->list[count].gem_level);
		}
		/* user.reduceObjectNum(command->gem_id, 4, command->gem_level);
		if(new_clid > 0)
			user.reduceObjectNum(new_clid, 30, 0);	 */
		//add_object(user, up_ob, false);
		response(user, 0, MAKELV);
	//	Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "恭喜你，升级装备成功！");
		return true;

	}

	return false;
}

//soke 是否可以升级改造
bool EquipMaker::can_makelv(zObject * ob)
{
	if (ob->data.needlevel < 70 || !ob->data.bind || !(ob->data.kind & 4 || ob->data.kind & 16)  )
	{
		return false;
	}
	return true;
}

//soke 装备升级改造
int EquipMaker::makelv_upgrade(zObject* ob)
{
	Zebra::logger->debug("开始升级装备ID:%d",ob->data.dwObjectID);
	if (ob->base->needlevel > 215)
	{
		return false;
	}
	if(ob->base->needlevel == 185 && ob->base->kind != 141 && ob->base->kind != 142 && ob->base->kind != 143 && ob->base->kind != 144 && ob->base->kind != 145)
	{	
		if (ob->base->kind < 117)
		{
			if (ob->base->kind >= 113 && ob->base->kind <= 116)
			{
				return ob->data.dwObjectID + 9002;
			}
			else
			{
				return ob->data.dwObjectID + 9001;
			}
		}
		else if(ob->base->kind == 117 || ob->base->kind == 118 || ob->base->kind == 136)
		{
			if (ob->base->kind == 117 || ob->base->kind == 118) //选定类型185以上加8002 不在类型类的加8001 刺客136为8001
			{
				return ob->data.dwObjectID + 8002;
			}
			else
			{
				return ob->data.dwObjectID + 8001;
			}
		}		
		else if(ob->base->kind == 137 || ob->base->kind == 138)
		{
			if (ob->base->kind == 137) //选定类型185以上加7002 不在类型类的加2 
			{
				return ob->data.dwObjectID + 7002;
			}
			else
			{
				return ob->data.dwObjectID + 2;
			}
		}			
	    }
	else if ((ob->base->kind >= 113 && ob->base->kind <= 118) || ob->base->kind == 137 || ob->base->kind == 155 || ob->base->kind == 138 || ob->base->kind == 156)
	{
		return ob->data.dwObjectID + 2;
	}
	else
	{
		return ob->data.dwObjectID + 1;
	}
	
	//ob->data.dwObjectID += 1;
	return 0;
}

// 装备升级继承属性
int EquipMaker::upgrade_inherit(SceneUser& user, zObject* new_ob, zObject* old_ob)
{
	new_ob->data.bind = old_ob->data.bind;
	//星星
	new_ob->data.upgrade = old_ob->data.upgrade;
	if(new_ob->data.upgrade > 0)
	{
		if (new_ob->data.kind & 16)
			repur_upgrade(new_ob); //修复升级掉星问题repur2_upgrade改为repur_upgrade
		else
			re_upgrade(new_ob);    //修复升级掉星问题re2_upgrade改为re_upgrade
		new_ob->data.xingshu = old_ob->data.xingshu;
	}	
	//魂魄石
	{
		memcpy(new_ob->data.socket, old_ob->data.socket, sizeof(old_ob->data.socket));
		new_ob->data.xstr = old_ob->data.xstr;
		new_ob->data.xinte = old_ob->data.xinte;
		new_ob->data.xdex = old_ob->data.xdex;
		new_ob->data.xspi = old_ob->data.xspi;
		new_ob->data.xcon = old_ob->data.xcon;
	}
	//灵魂属性
	{
		new_ob->data.fivetype = old_ob->data.fivetype;
		new_ob->data.fivepoint = old_ob->data.fivepoint;

		memcpy(new_ob->data._five_props, old_ob->data._five_props, sizeof(old_ob->data._five_props));
		new_ob->data.dpdam = old_ob->data.dpdam;
		new_ob->data.dmdam = old_ob->data.dmdam;
		new_ob->data.bdam = old_ob->data.bdam;
		new_ob->data.rdam = old_ob->data.rdam;
		new_ob->data.ignoredef = old_ob->data.ignoredef;

		new_ob->data.addpdef = old_ob->data.addpdef;
		new_ob->data.addmdef = old_ob->data.addmdef;
		new_ob->data.addpdam = old_ob->data.addpdam;
		new_ob->data.addmdam = old_ob->data.addmdam;
		new_ob->data.addhp = old_ob->data.addhp;
	}
	//增加攻击防御%
	{
		new_ob->data.pdam = old_ob->data.pdam;
		new_ob->data.pdef = old_ob->data.pdef;
		new_ob->data.mdam = old_ob->data.mdam;
		new_ob->data.mdef = old_ob->data.mdef;
	}

	if (new_ob->data.kind & 4)
	{
		//补天
		if (old_ob->data.butian > 0)
		{
			int i = old_ob->data.butian;
			while (i > 0)
			{
				i--;
				Upgrade::bt_upgrade(user,new_ob,100000);
			}
		}
	}

	if (new_ob->data.kind & 16)
	{
		//补天
		if (old_ob->data.butian > 0)
		{
			int i = old_ob->data.butian;
			while (i > 0)
			{
				i--;
				Upgrade::bt_upgrade(user,new_ob,100000);
			}
		}
		//斗魂
		if (old_ob->data.douhun > 0)
		{
			int i = old_ob->data.douhun;
			while (i > 0)
			{
				i--;
				Upgrade::dh_upgrade(user,new_ob,100000);
			}
		}
		//龙槽
		if (old_ob->data.dtrough > 0)
		{
			int i = old_ob->data.dtrough;
			while (i > 0)
			{
				i--;
				Upgrade::dtrough_upgrade(user,new_ob,100000);
			}
		}
		//龙星
		if (old_ob->data.drastar > 0)
		{
			int i = old_ob->data.drastar;
			while (i > 0)
			{
				i--;
				Upgrade::drastar_upgrade(user,new_ob,100000);
			}
		}
		//寒冰
		if (old_ob->data.retain1 > 0)
		{
			int i = old_ob->data.retain1;
			while (i > 0)
			{
				i--;
				Upgrade::hb_upgrade(user,new_ob,100000);
			}
		}
		//装备栏
		if (old_ob->data.retain2 > 0)
		{
			new_ob->data.retain2 = old_ob->data.retain2;
		}
		//神器
		if (old_ob->data.retain3 > 0)
		{
			int i = old_ob->data.retain3;
			while (i > 0)
			{
				i--;
				Upgrade::shenqi_upgrade(user,new_ob,100000);
			}
		}
		//精炼
		if (old_ob->data.itemrefine > 0)
		{
			int i = old_ob->data.itemrefine;
			while (i > 0)
			{
				i--;
				Upgrade::refine_upgrade(user,new_ob,100000);
			}
		}
		//神龙镶嵌
		if (old_ob->data.longwanglv > 0)
		{
			int i = old_ob->data.longwanglv;
			while (i > 0)
			{
				i--;
				Upgrade::longwang_upgrade(user,new_ob,100000);
			}
		}
		//逆天进阶
		if (old_ob->data.nitianlv > 0)
		{
			int i = old_ob->data.nitianlv;
			while (i > 0)
			{
				i--;
				Upgrade::nitian_upgrade(user,new_ob,100000);
			}
		}
		//神魂宝石(斗战圣佛)
		if (old_ob->data.douzhansflv > 0)
		{
			int i = old_ob->data.douzhansflv;
			while (i > 0)
			{
				i--;
				Upgrade::douzhansf_upgrade(user,new_ob,100000);
			}
		}
		//暗影宝石
		if (old_ob->data.aystonelv > 0)
		{
			int i = old_ob->data.aystonelv;
			while (i > 0)
			{
				i--;
				Upgrade::aystone_upgrade(user,new_ob,100000);
			}
		}
		//龙凤吟
		if (old_ob->data.retain24 > 0)
		{
			int i = old_ob->data.retain24;
			while (i > 0)
			{
				i--;
				Upgrade::longfengying_upgrade(user,new_ob,100000);
			}
		}

		//轰天继承
		if (old_ob->data.hongtiannum1 > 0)
		{
			new_ob->data.hongtiannum1 = old_ob->data.hongtiannum1;
		}
		if (old_ob->data.hongtiannum2 > 0)
		{
			new_ob->data.hongtiannum2 = old_ob->data.hongtiannum2;
		}
		if (old_ob->data.hongtiannum3 > 0)
		{
			new_ob->data.hongtiannum3 = old_ob->data.hongtiannum3;
		}
		if (old_ob->data.hongtiannum4 > 0)
		{
			new_ob->data.hongtiannum4 = old_ob->data.hongtiannum4;
		}
		if (old_ob->data.hongtiannum5 > 0)
		{
			new_ob->data.hongtiannum5 = old_ob->data.hongtiannum5;
		}
		if (old_ob->data.hongtiannum6 > 0)
		{
			new_ob->data.hongtiannum6 = old_ob->data.hongtiannum6;
		}
		if (old_ob->data.hongtiannum7 > 0)
		{
			new_ob->data.hongtiannum7 = old_ob->data.hongtiannum7;
		}
		if (old_ob->data.hongtiannum8 > 0)
		{
			new_ob->data.hongtiannum8 = old_ob->data.hongtiannum8;
		}
		if (old_ob->data.hongtiannum9 > 0)
		{
			new_ob->data.hongtiannum9 = old_ob->data.hongtiannum9;
		}
		
	}
	
	return 1;
}

//soke 装备升级改造
int EquipMaker::upgrade_attri(zObject* ob,zObjectB *objbase)
{
	//ob->data.needlevel = objbase->needlevel;			// 需要等级

	ob->data.maxhp = objbase->maxhp;					// 最大生命值
	ob->data.maxmp = objbase->maxmp;					// 最大法术值
	ob->data.maxsp = objbase->maxsp;					// 最大体力值

	ob->data.pdamage = objbase->pdamage;				// 最小攻击力
	ob->data.maxpdamage = objbase->maxpdamage;			// 最大攻击力
	ob->data.mdamage = objbase->mdamage;				// 最小法术攻击力
	ob->data.maxmdamage = objbase->maxmdamage;			// 最大法术攻击力

	ob->data.pdefence = objbase->pdefence;				// 物防
	ob->data.mdefence = objbase->mdefence;				// 魔防
	ob->data.damagebonus = objbase->damagebonus;		// 伤害加成

	ob->data.akspeed = objbase->akspeed;				// 攻击速度
	ob->data.mvspeed = objbase->mvspeed;				// 移动速度
	ob->data.atrating = objbase->atrating;				// 命中率
	ob->data.akdodge = objbase->akdodge;				// 躲避率
	ob->data.bang = objbase->bang;
	ob->data.dur = objbase->durability;
	ob->data.maxdur = objbase->durability;
	ob->data.price = objbase->price;
	ob->data.cardpoint = objbase->cardpoint;

	zZhuanGoldObjectB* bob = NULL;

	int gold = odds_of_gold(objbase);
    if (zMisc::selectByTenTh(gold)||(ob->data.kind) & 2)
    {
		int holy = odds_of_holy(objbase->holyrating);
		int purple = odds_of_purple(objbase->holyrating/10);
		bool g_holy = zMisc::selectByTenTh(holy) || (ob->data.kind & 4);
		bool g_purple = zMisc::selectByMilh(purple) || (ob->data.kind & 16);
		if (g_holy)
		{
			int index = zMisc::randBetween(0, objbase->holys.size()-1);	
			bob = zhuangoldobjectbm.get(objbase->holys[index]);
		}
		int props = 0;
		if (holy > 3000*100) 
		{
			props = 18;
		} 
		else if (holy > 900*100) 
		{
			props = 17;
		}
		else if (g_holy) 
		{
			props = 3;
		}
		else 
		{
			props = 2;
		}
		//如果是紫装 则100%完美 props就是用来计算品质的。
		if (g_purple)
		{
			props = 18;
		}
		//if (strstr(ob->data.strName,"完美的"))
		//{
		//	props = 18;
		//}
		if(props>=5)
		{
			int i=0;
			int size=objbase->blues.size();
			if(size >= 8)
			{
				i = zMisc::randBetween(0, 1)?3:7;			
			}
			else if(size >=4)
			{
				i=3;
			}
			else
			{
				i = zMisc::randBetween(0, objbase->golds.size()-1);			
			}
			zZhuanGoldObjectB* bob1=NULL;
			bob1 = zhuangoldobjectbm.get(objbase->holys[i]);
			if(bob1)
			{
				ungrade_assign_color(bob1, ob, 2,props,objbase);
			}
			else
			{
				ungrade_assign_color(bob, ob, 2,props,objbase);
			}
		}
		else
		{
			ungrade_assign_color(bob, ob, 2,props,objbase);
		}

		if(g_holy) 
		{
			assign_holy(ob, holy);// 神圣装备
		}
		//soke  紫装
		if(g_purple) 
		{
			assign_purple(ob, holy);// 紫色装备
		}

		//soke 五行套装就是灵魂锁链。。。。。。如果所紫装 就生成两个灵魂	
		if ( (ob->data.fivetype != FIVE_NONE )|| ((ob->data.kind) & 8) || ((ob->data.kind) & 4) ||((ob->data.kind) & 16) )
		{
			assign_set(ob);	
		}

		bonus_hole(ob);
		fix(ob);
		return 1;

    }
	return 0;
}

// 装备转换系统 by:醉梦
bool RebuildObject::doZhuanhuanCmd(SceneUser &user, const Cmd::stZhuanhuanUserCmd *ptCmd, unsigned int cmdLen)
{
	using namespace Cmd;
	switch (ptCmd->byParam) // 这里就是判断子命令了byParam
	{
	case ZHUANHUAN_START:
	{
		stZhuanhuanCmd *cmd = (stZhuanhuanCmd *)ptCmd;
		zObject *itemobj = user.packs.uom.getObjectByThisID(cmd->itemUID);
		

		if (itemobj)
		{
			if (itemobj->data.needlevel < 60)
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "60级以下装备无法进行装备转换");
				return true;
			}

			int newobjectid = 0;

			if (cmd->zhiye != -1 && cmd->type != -1) // 转成卫士 枪
			{
				int zhuankind = 0;

				switch (cmd->zhiye)
				{
				case 7: // 剑
				{
					if (itemobj->data.needlevel < 140)
					{
						Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "小于140级的道具无法转换成剑武器");
						return true;
					}
					zhuankind = 137;
				}
				break;
				case 8: // 枪
				{
					if (itemobj->data.needlevel < 190)
					{
						Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "小于190级的道具无法转换成长枪武器");
						return true;
					}
					zhuankind = 155;
				}
				break;
				}
				if (itemobj->data.dwObjectID > 20000) // 王者
				{
					for (int i = 20000; i <= 30000; i++)
					{
						zObjectB *new_base = objectbm.get(i);
						if (!new_base)
						{
							continue;
						}
						if (new_base->kind != zhuankind)
						{
							continue;
						}
						if (new_base->needlevel != itemobj->base->needlevel)
						{
							continue;
						}
						if (cmd->type == 1) // 物理
						{
							if (strstr(new_base->name, "(物)"))
							{
								newobjectid = new_base->id;
								break;
							}
						}
						else
						{ // 魔法
							if (strstr(new_base->name, "(魔)"))
							{
								newobjectid = new_base->id;
								break;
							}
						}
					}
				}
				else
				{
					for (int i = 10000; i <= 20000; i++)
					{
						zObjectB *new_base = objectbm.get(i);
						if (!new_base)
						{
							continue;
						}
						if (new_base->kind != zhuankind)
						{
							continue;
						}
						if (new_base->needlevel != itemobj->base->needlevel)
						{
							continue;
						}
						if (cmd->type == 1) // 物理
						{
							if (strstr(new_base->name, "(物)"))
							{
								newobjectid = new_base->id;
								break;
							}
						}
						else
						{ // 魔法
							if (strstr(new_base->name, "(魔)"))
							{
								newobjectid = new_base->id;
								break;
							}
						}
					}
				}

				if (newobjectid == 0)
				{
					Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "转换后的物品ID为0，错误");
					return false;
				}
				zObjectB *new_base = objectbm.get(newobjectid);
				if (!new_base)
				{
					Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "转换后的物品不存在");
					return false;
				};
				if (user.packs.checkGold(50000) && user.packs.removeGold(50000, "装备转换"))
				{
					new_base->goldrating = 10000;
					new_base->holyrating = 100000;

					zObject *new_ob = zObject::create(new_base, 1);

					if (itemobj->data.kind & 16)
					{
						EquipMaker maker(NULL);
						;
						new_ob->data.round = itemobj->data.round;
						new_ob->data.kind = 16;
						maker.modify_attri16(new_ob, new_base);
					}
					else
					{
						EquipMaker maker(NULL);
						new_ob->data.round = itemobj->data.round;
						new_ob->data.kind = 4;
						maker.modify_attri(new_ob, new_base);
					}

					EquipMaker modify(&user);
					modify.modify_addmaker(new_ob, &user);
					modify.upgrade_inherit(user, new_ob, itemobj);
					Zebra::logger->debug("升级完成");
					if (new_ob)
					{
						new_ob->data.pos = itemobj->data.pos;
						user.packs.removeObject(itemobj);
						if (user.packs.addObject(new_ob, false))
						{
							Base::add_object(user, new_ob, false);
						}
					}
				}
				else
				{
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你没有5锭金子，无法转换");
				}
			}
			else if (cmd->zhiye != -1 && cmd->type == -1) // 转成战士 法师 弓手 仙术 召唤 刺客
			{
				int zhuankind = 0;
				switch (cmd->zhiye)
				{
				case 1: // 刀
				{
					zhuankind = 104;
				}
				break;
				case 2: // 杖
				{
					zhuankind = 108;
				}
				break;
				case 3: // 弓
				{
					zhuankind = 109;
				}
				break;
				case 4: // 扇
				{
					zhuankind = 110;
				}
				break;
				case 5: // 棍
				{
					zhuankind = 111;
				}
				break;
				case 6: // 刃
				{
					if (itemobj->data.needlevel < 140)
					{
						Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "小于140级的道具无法转换成利刃武器");
						return true;
					}
					zhuankind = 136;
				}
				break;
				}

				if (itemobj->data.dwObjectID > 20000) // 王者
				{
					for (int i = 20000; i <= 30000; i++)
					{
						zObjectB *new_base = objectbm.get(i);
						if (!new_base)
						{
							continue;
						}
						if (new_base->kind != zhuankind)
						{
							continue;
						}
						if (new_base->needlevel != itemobj->base->needlevel)
						{
							continue;
						}
						newobjectid = new_base->id;
						break;
					}
				}
				else
				{
					for (int i = 10000; i <= 20000; i++)
					{
						zObjectB *new_base = objectbm.get(i);
						if (!new_base)
						{
							continue;
						}
						if (new_base->kind != zhuankind)
						{
							continue;
						}
						if (new_base->needlevel != itemobj->base->needlevel)
						{
							continue;
						}
						newobjectid = new_base->id;
						break;
					}
				}


				if (newobjectid == 0)
				{
					Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "转换后的物品ID为0，错误");
					return false;
				}
				zObjectB *new_base = objectbm.get(newobjectid);
				if (!new_base)
				{
					Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "转换后的物品不存在");
					return false;
				};
				if (user.packs.checkGold(50000) && user.packs.removeGold(50000, "装备转换"))
				{
					new_base->goldrating = 10000;
					new_base->holyrating = 100000;

					zObject *new_ob = zObject::create(new_base, 1);

					if (itemobj->data.kind & 16)
					{
						EquipMaker maker(NULL);
						;
						new_ob->data.round = itemobj->data.round;
						new_ob->data.kind = 16;
						maker.modify_attri16(new_ob, new_base);
					}
					else
					{
						EquipMaker maker(NULL);
						new_ob->data.round = itemobj->data.round;
						new_ob->data.kind = 4;
						maker.modify_attri(new_ob, new_base);
					}

					EquipMaker modify(&user);
					modify.modify_addmaker(new_ob, &user);
					modify.upgrade_inherit(user, new_ob, itemobj);
					Zebra::logger->debug("升级完成");
					if (new_ob)
					{
						new_ob->data.pos = itemobj->data.pos;
						user.packs.removeObject(itemobj);
						if (user.packs.addObject(new_ob, false))
						{
							Base::add_object(user, new_ob, false);
						}
					}
				}
				else
				{
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你没有5锭金子，无法转换");
				}
			}
			else if (cmd->zhiye == -1 && cmd->type != -1) // 其它装备 物魔
			{
				if (itemobj->base->kind == 102 )//甲
				{
					if (cmd->type == 1) // 转物理
					{
						newobjectid = itemobj->data.dwObjectID + 50;
					}
					else if (cmd->type == 2) // 转魔法
					{
						Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "此装备已经是魔法属性装备了");
						return true;
					}
				}
				else if (itemobj->base->kind == 103 )//凯
				{
					if (cmd->type == 1) // 转物理
					{
						Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "此装备已经是物理属性装备了");
						return true;
					}
					else if (cmd->type == 2) // 转魔法
					{
						newobjectid = itemobj->data.dwObjectID - 50;
					}
				}
				else if (itemobj->base->kind == 156)//短枪
				{
					if (itemobj->data.dwObjectID % 2 != 0) // 物理
					{
						if (cmd->type == 1) // 转物理
						{
							Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "此装备已经是物理属性装备了");
							return true;
							
						}
						else if (cmd->type == 2) // 转魔法
						{
							newobjectid = itemobj->data.dwObjectID + 1;
						}
					}
					else
					{
						if (cmd->type == 1) // 转物理
						{
							newobjectid = itemobj->data.dwObjectID - 1;
						}
						else if (cmd->type == 2) // 转魔法
						{
							Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "此装备已经是魔法属性装备了");
							return true;
						}
					}
				}
				else if (itemobj->base->kind == 138)//护心镜
				{
					if (itemobj->data.dwObjectID % 2 != 0) // 物理
					{
						if (cmd->type == 1) // 转物理
						{
							Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "此装备已经是物理属性装备了");
							return true;
							
						}
						else if (cmd->type == 2) // 转魔法
						{
							newobjectid = itemobj->data.dwObjectID + 1;
						}
					}
					else
					{
						if (cmd->type == 1) // 转物理
						{
							newobjectid = itemobj->data.dwObjectID - 1;
						}
						else if (cmd->type == 2) // 转魔法
						{
							Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "此装备已经是魔法属性装备了");
							return true;
						}
					}
				}
				else if (itemobj->base->kind == 113 || itemobj->base->kind == 114 || itemobj->base->kind == 115)
				{
					if (itemobj->data.dwObjectID % 2 != 0) // 魔法
					{
						if (cmd->type == 1) // 转物理
						{
							newobjectid = itemobj->data.dwObjectID + 1;
						}
						else if (cmd->type == 2) // 转魔法
						{
							Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "此装备已经是魔法属性装备了");
							return true;
						}
					}
					else
					{
						if (cmd->type == 1) // 转物理
						{
							Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "此装备已经是物理属性装备了");
							return true;
						}
						else if (cmd->type == 2) // 转魔法
						{
							newobjectid = itemobj->data.dwObjectID - 1;
						}
					}
				}
				else if (itemobj->base->kind == 116 || itemobj->base->kind == 117 || itemobj->base->kind == 118)
				{
					if (itemobj->data.dwObjectID % 2 != 0) // 物理
					{
						if (cmd->type == 1) // 转物理
						{
							Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "此装备已经是物理属性装备了");
							return true;
						}
						else if (cmd->type == 2) // 转魔法
						{
							newobjectid = itemobj->data.dwObjectID + 1;
						}
					}
					else
					{
						if (cmd->type == 1) // 转物理
						{
							newobjectid = itemobj->data.dwObjectID - 1;
						}
						else if (cmd->type == 2) // 转魔法
						{
							Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "此装备已经是魔法属性装备了");
							return true;
						}
					}
				}

				if (newobjectid == 0)
				{
					Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "转换后的物品ID为0，错误");
					return false;
				}
				zObjectB *new_base = objectbm.get(newobjectid);
				if (!new_base)
				{
					Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "转换后的物品不存在");
					return false;
				};
				if (user.packs.checkGold(50000) && user.packs.removeGold(50000, "装备转换"))
				{
					new_base->goldrating = 10000;
					new_base->holyrating = 100000;

					zObject *new_ob = zObject::create(new_base, 1);

					if (itemobj->data.kind & 16)
					{
						EquipMaker maker(NULL);
						;
						new_ob->data.round = itemobj->data.round;
						new_ob->data.kind = 16;
						maker.modify_attri16(new_ob, new_base);
					}
					else
					{
						EquipMaker maker(NULL);
						new_ob->data.round = itemobj->data.round;
						new_ob->data.kind = 4;
						maker.modify_attri(new_ob, new_base);
					}

					EquipMaker modify(&user);
					modify.modify_addmaker(new_ob, &user);
					modify.upgrade_inherit(user, new_ob, itemobj);
					Zebra::logger->debug("升级完成");
					if (new_ob)
					{
						new_ob->data.pos = itemobj->data.pos;
						user.packs.removeObject(itemobj);
						if (user.packs.addObject(new_ob, false))
						{
							Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "转换成功，扣除手续费5锭金");
							Base::add_object(user, new_ob, false);
						}
					}
				}
				else
				{
					Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "你没有5锭金子，无法转换");
				}
			}
			else
			{
				Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "未知错误，请联系GM");
			}
		}
		else
		{
			Channel::sendSys(&user, Cmd::INFO_TYPE_GAME, "装备让你吃了？无法转换联系GN");
		}
	}
	break;
	}
	return true;
}
