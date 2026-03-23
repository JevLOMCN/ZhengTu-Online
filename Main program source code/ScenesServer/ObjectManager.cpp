#include "ObjectManager.h"
#include "Zebra.h"
#include "zMisc.h"
#include "zObject.h"

GlobalObjectIndex *GlobalObjectIndex::onlyme=NULL;

GlobalObjectIndex *const goi=GlobalObjectIndex::getInstance();

GlobalObjectIndex *GlobalObjectIndex::getInstance()
{
	if(onlyme==NULL)
		onlyme= new GlobalObjectIndex();
	return onlyme;
}

void GlobalObjectIndex::delInstance()
{
	SAFE_DELETE(onlyme);
}

GlobalObjectIndex::~GlobalObjectIndex()
{
	clear();
}

GlobalObjectIndex::GlobalObjectIndex()
{
}

bool GlobalObjectIndex::addObject(zObject * o)
{
	bool bret=false;
	if(o)
	{
		mlock.lock();
		zObject *ret =(zObject *)getEntryByID(o->id);
		if(ret)//物品重复处理
		{
			if(ret==o)
			{
				bret=true;
				Zebra::logger->debug("全局物品管理器中发现有重复添加物品(%s,%d)",ret->data.strName,ret->data.qwThisID);
			}
			else if(ret->createtime==o->createtime && ret->data.dwObjectID==o->data.dwObjectID && ret->data.qwThisID == o->data.qwThisID)
			{
				Zebra::logger->alarm("复制物品:%s 创建时间:%llu",ret->name,ret->createid);
				ret=false;
			}
			else
			{
				Zebra::logger->debug("id冲突%s(%d)", ret->name, ret->id);
				do {
					//重新生成ID
					o->generateThisID();
					bret = addEntry((zEntry *)o);
				} while (!bret);
				//if (!bret) Zebra::logger->fatal("添加物品到物品表失败1");
			}
		}
		else
		{
			bret=addEntry((zEntry *)o);
			if(!bret) Zebra::logger->fatal("添加物品到物品表失败");
		}
		mlock.unlock();
	}else {
		Zebra::logger->fatal("添加非法物品");
	}
	
	return bret;
}

void GlobalObjectIndex::removeObject(DWORD thisid)
{
	mlock.lock();
	zEntry *e=getEntryByID(thisid);
	if(e)
		removeEntry(e);
	mlock.unlock();
}

UserObjectM::UserObjectM()
{
}

UserObjectM::~UserObjectM()
{
}

zObject * UserObjectM::getObjectByThisID( DWORD thisid)
{
	return (zObject *)getEntryByID(thisid);
}

class UserObjectComparePos:public UserObjectCompare 
{
	public:
		stObjectLocation dst;

		bool isIt(zObject *object)
		{
			return true;
		}
};

zObject *UserObjectM::getObjectByPos(const stObjectLocation &dst)
{
	UserObjectComparePos comp;
	comp.dst=dst;
	return getObject(comp);
}

void UserObjectM::removeObjectByThisID(DWORD thisid)
{
	zEntry *e=getEntryByID(thisid);
	if(e)
		removeEntry(e);
}

void UserObjectM::removeObject(zObject * o)
{
	removeEntry(o);
}

bool UserObjectM::addObject(zObject * o)
{
	return addEntry(o);
}

zObject *UserObjectM::getObject(UserObjectCompare &comp)
{
	for(hashmap::iterator it=ets.begin();it!=ets.end();it++)
	{
		if (((zObject *)it->second)->data.pos.loc()!=Cmd::OBJECTCELLTYPE_COMMON &&
				((zObject *)it->second)->data.pos.loc()!=Cmd::OBJECTCELLTYPE_PACKAGE) continue;

		if(comp.isIt((zObject *)it->second))
			return (zObject *)it->second;
	}
	return NULL;
}

void UserObjectM::execEvery(UserObjectExec &exec)
{
//	Obj_vec a, b;
	Obj_vec a;
	for(hashmap::iterator it=ets.begin();it!=ets.end();it++)
	{

		zObject* tmp = (zObject *)it->second;
		//			Zebra::logger->debug("物品%s(%d, %d, %d, %d)", tmp->name, tmp->data.pos.dwLocation, tmp->data.pos.dwTableID, tmp->data.pos.x, tmp->data.pos.y);
		if (tmp->data.pos.tab()) {
			a.push_back(tmp);
			continue;
		}

		if(!exec.exec((zObject *)it->second))
			return;
	}
	
	Obj_vec::iterator it;
	for (it=a.begin(); it!=a.end(); ++it) {
//		Zebra::logger->debug("物品%s(%d, %d, %d)", (*it)->name, (*it)->data.pos.dwLocation, (*it)->data.pos.dwTableID, (*it)->data.pos.x, (*it)->data.pos.y);		
		if(!exec.exec(*it))
			return;
	}

}

/**
 * \brief 判断包裹中某物品是否具有指定的数量
 * moved from MainPack for support multi packages
 * \param id: 物品的objectid
 * \param number: 要求的数量
 * \param upgrade: 物品等级 ，或物品类型，根据type参数判断
 * \param type: 比较类型0为物品品等级，1为物品类型
 * \author lqy
 * \return 包裹中物品数量大于等于给定的数量返回某一个物品的thisid，否则返回0
 */
DWORD UserObjectM::exist(DWORD id, DWORD number, BYTE upgrade , BYTE type) const
{
	DWORD num = 0;
	
  //soke 遍历包袱开始比较材料
  for(hashmap::const_iterator it=ets.begin();it!=ets.end();it++)  
  {
    zObject* ob = (zObject *)it->second;

		if (ob->data.pos.loc()!=Cmd::OBJECTCELLTYPE_COMMON && ob->data.pos.loc()!=Cmd::OBJECTCELLTYPE_PACKAGE) continue;

		if ( ob->data.dwObjectID == id &&
				((ob->data.upgrade == upgrade && type==0) ||
				 (( ob->data.kind == upgrade || (ob->data.kind & upgrade)) && type==1))
		   )
		{
			num += 	ob->data.dwNum;
			if (num >= number) return ob->data.qwThisID;
		}		
	}
	
	return 0;
}

#include "SceneUser.h"

int UserObjectM::space(const SceneUser* user) const
{
#if 0
	int free = 0;
	free = user->packs.main.get_width()*user->packs.main.get_height();
	if (user->packs.equip.left) 
	{
		free += user->packs.equip.left->get_width()*user->packs.equip.left->get_height();		
	}
	if (user->packs.equip.right) 
	{
		free += user->packs.equip.right->get_width()*user->packs.equip.right->get_height();		
	}
	
	for(hashmap::const_iterator it=ets.begin();it!=ets.end();it++) 
	{
		zObject* ob = (zObject *)it->second;
		if (ob->data.pos.dwLocation!=Cmd::OBJECTCELLTYPE_COMMON && ob->data.pos.dwLocation!=Cmd::OBJECTCELLTYPE_PACKAGE) continue;
		if (ob->base->kind == ItemType_Money) continue;
		//Zebra::logger->debug("space: %s, %d", ob->name, ob->data.pos.dwLocation);
		--free;
	}
#endif
	int free = user->packs.main.space();
	if (user->packs.equip.pack(EquipPack::L_PACK) && user->packs.equip.pack(EquipPack::L_PACK)->can_input()) 
	{
		free += user->packs.equip.pack(EquipPack::L_PACK)->space();		
	}
	if (user->packs.equip.pack(EquipPack::R_PACK) && user->packs.equip.pack(EquipPack::R_PACK)->can_input()) 
	{
		free += user->packs.equip.pack(EquipPack::R_PACK)->space();		
	}

	return free;
}

/**
 * \brief 根据物品id在包裹中查找没有到达最大数量的物品,如果包裹中有多个相同ID物品,则按在包裹中的顺序返回
 * \param id: 物品的objectid
 * \param upgrade: 物品等级
 * \param not_need_space: 是否需要有剩余空间
 * \author lqy
 * \return 找到则返回该物品实例,否则返回NULL
 */
zObject* UserObjectM::getObjectByID(DWORD id, BYTE upgrade, bool not_need_space) const
{
	for(hashmap::const_iterator it=ets.begin();it!=ets.end();it++)	
	{

		zObject* ob = (zObject *)it->second;
		if (ob->data.pos.loc()!=Cmd::OBJECTCELLTYPE_COMMON && ob->data.pos.loc()!=Cmd::OBJECTCELLTYPE_PACKAGE) continue;

		if ( (ob->data.dwObjectID == id) && (ob->data.upgrade== upgrade) && ( not_need_space || ob->data.dwNum < ob->base->maxnum) ) 
		{
			return ob;	
		}		
	}

	return NULL;
}

#include "RebuildObject.h"
/**
 * \brief 根据物品id调整物品在包裹中的数量,如果大于该物品最大数量则创建一个新物品
 * \param user: 要改变包裹的用户
 * \param id: 物品的objectid
 * \param number: 增加的数量
 * \param orig_ob: 数量被改变的物品,如果没有物品数量被改变,该参数被忽略
 * \param new_obs: 新创建的物品的列表,如果没有物品被创建,该参数被忽略
 * \param upgrade: 物品等级
 * \author lqy
 * \return 失败返回-1, 没有物品被创建返回0, 包裹满返回1, 成功添加所有创建的物品返回2
 */
int UserObjectM::addObjectNum(SceneUser* user, DWORD id, DWORD number, zObject* & orig_ob, Obj_vec& new_obs, BYTE upgrade)
{
	//check if exist this object id
	zObjectB* o = objectbm.get(id);
	if (!o) 
	{
		return -1;
	}
	
	DWORD capacity = o->maxnum;
	
	//check if we have a instance of this object which havn't reached max number
	zObject* ob = getObjectByID(id, upgrade);
	if (ob) 
	{
		number += ob->data.dwNum;
		if (capacity >= number) 
		{ //have enough space to put this 
			ob->data.dwNum = number;			
			orig_ob = ob;
			return 0;
		}
		else 
		{ //space isn't enough, set max number first, then create new object
			ob->data.dwNum = capacity;
			number -= capacity;
			orig_ob = ob;
		}
	}
	
	while ((int)number > 0) 
	{
		zObject *tmp = zObject::create(o, (number>=capacity)?capacity:number, upgrade);
		if (tmp) 
		{
			// tmp->data.bind=1;
			EquipMaker maker(NULL);
			maker.assign(NULL, tmp, tmp->base);
			EquipMaker modify(NULL);
			modify.re_upgrade(tmp); //myy 任务给绿装刷新装备星星属性
			
			if (user->packs.addObject(tmp, true, AUTO_PACK)) 
			{
				//soke 特定类型需要绑定
                //  tmp->checkXinBind();

			zObject::logger(tmp->createid,tmp->data.qwThisID,tmp->data.strName,tmp->data.dwNum,tmp->data.dwNum,1,0,NULL,user->id,user->name,"新增",tmp->base,tmp->data.kind,tmp->data.upgrade);
				new_obs.push_back(tmp);		
			}
			else
			{
				//maybe package is full, stop process, but still return true
				zObject::destroy(tmp);	
				return 1;
			}
		}
		number -= capacity;
	}
	
	return 2;
}

/**
 * \brief 根据物品id调整物品在包裹中的数量,如果大于该物品最大数量则创建一个新物品并且绑定(只用于任务接口请勿随便调用)
 * \param user: 要改变包裹的用户
 * \param id: 物品的objectid
 * \param number: 增加的数量
 * \param orig_ob: 数量被改变的物品,如果没有物品数量被改变,该参数被忽略
 * \param new_obs: 新创建的物品的列表,如果没有物品被创建,该参数被忽略
 * \param upgrade: 物品等级
 * \author lqy
 * \return 失败返回-1, 没有物品被创建返回0, 包裹满返回1, 成功添加所有创建的物品返回2
 */
//soke 增加绑定物品
int UserObjectM::addBindObjectNum(SceneUser* user, DWORD id, DWORD number, zObject* & orig_ob, Obj_vec& new_obs, BYTE upgrade)
{
	//check if exist this object id
	zObjectB* o = objectbm.get(id);
	if (!o) {
		return -1;
	}
	
	DWORD capacity = o->maxnum;
	
	//check if we have a instance of this object which havn't reached max number
	zObject* ob = getObjectByID(id, upgrade);
	if (ob) {
		number += ob->data.dwNum;
		if (capacity >= number) { //have enough space to put this 
			ob->data.dwNum = number;			
			orig_ob = ob;
			return 0;
		}else { //space isn't enough, set max number first, then create new object
			ob->data.dwNum = capacity;
			number -= capacity;
			orig_ob = ob;
		}
	}
	
	while ((int)number > 0) 
	{
		zObject *tmp = zObject::create(o, (number>=capacity)?capacity:number, upgrade);
		if (tmp) 
		{
			EquipMaker maker(NULL);
			maker.assign(NULL, tmp, tmp->base);
			EquipMaker modify(NULL);
			modify.re_upgrade(tmp); //myy 任务给绿装刷新装备星星属性
			
			if (user->packs.addObject(tmp, true, AUTO_PACK)) 
			{
                tmp->data.bind=1;
				zObject::logger(tmp->createid,tmp->data.qwThisID,tmp->data.strName,tmp->data.dwNum,tmp->data.dwNum,1,0,NULL,user->id,user->name,"新增",tmp->base,tmp->data.kind,tmp->data.upgrade);
				new_obs.push_back(tmp);		
			}
			else 
			{
				//maybe package is full, stop process, but still return true
				zObject::destroy(tmp);	
				return 1;
			}
		}
		number -= capacity;
	}
	
	return 2;
}

/**
 * \brief 根据物品id调整物品在包裹中的数量,如果大于该物品最大数量则创建一个新物品并且绑定(只用于任务接口请勿随便调用)
 * \param user: 要改变包裹的用户
 * \param id: 物品的objectid
 * \param number: 增加的数量
 * \param orig_ob: 数量被改变的物品,如果没有物品数量被改变,该参数被忽略
 * \param new_obs: 新创建的物品的列表,如果没有物品被创建,该参数被忽略
 * \param upgrade: 物品等级
 * \author lqy
 * \return 失败返回-1, 没有物品被创建返回0, 包裹满返回1, 成功添加所有创建的物品返回2
 */
int UserObjectM::addBlueObjectNum(SceneUser* user, DWORD id, DWORD number, zObject* & orig_ob, Obj_vec& new_obs, BYTE upgrade)
{
	//check if exist this object id
	zObjectB* o = objectbm.get(id);
	if (!o) {
		return -1;
	}
	
	DWORD capacity = o->maxnum;
	
	//check if we have a instance of this object which havn't reached max number
	zObject* ob = getObjectByID(id, upgrade);
	if (ob) {
		number += ob->data.dwNum;
		if (capacity >= number) { //have enough space to put this 
			ob->data.dwNum = number;			
			orig_ob = ob;
			return 0;
		}else { //space isn't enough, set max number first, then create new object
			ob->data.dwNum = capacity;
			number -= capacity;
			orig_ob = ob;
		}
	}
	
	while ((int)number > 0) 
	{
		zObject *tmp = zObject::create(o, (number>=capacity)?capacity:number, upgrade);
		if (tmp) 
		{
			tmp->data.bind=1;
			EquipMaker maker(NULL);
			maker.assign(NULL, tmp, tmp->base, true, 1);
			EquipMaker modify(NULL);
			modify.re_upgrade(tmp); //myy 任务给绿装刷新装备星星属性
			
			if (user->packs.addObject(tmp, true, AUTO_PACK)) 
			{
				zObject::logger(tmp->createid,tmp->data.qwThisID,tmp->data.strName,tmp->data.dwNum,tmp->data.dwNum,1,0,NULL,user->id,user->name,"新增",tmp->base,tmp->data.kind,tmp->data.upgrade);
				new_obs.push_back(tmp);		
			}
			else 
			{
				//maybe package is full, stop process, but still return true
				zObject::destroy(tmp);	
				return 1;
			}
		}
		number -= capacity;
	}
	
	return 2;
}

/**
 * \brief 根据物品id调整物品在包裹中的数量,如果大于该物品最大数量则创建一个新物品并且绑定(只用于任务接口请勿随便调用)
 * \param user: 要改变包裹的用户
 * \param id: 物品的objectid
 * \param number: 增加的数量
 * \param orig_ob: 数量被改变的物品,如果没有物品数量被改变,该参数被忽略
 * \param new_obs: 新创建的物品的列表,如果没有物品被创建,该参数被忽略
 * \param upgrade: 物品等级
 * \author lqy
 * \return 失败返回-1, 没有物品被创建返回0, 包裹满返回1, 成功添加所有创建的物品返回2
 */
int UserObjectM::addGoldObjectNum(SceneUser* user, DWORD id, DWORD number, zObject* & orig_ob, Obj_vec& new_obs, BYTE upgrade)
{
	//check if exist this object id
	zObjectB* o = objectbm.get(id);
	if (!o) {
		return -1;
	}
	
	DWORD capacity = o->maxnum;
	
	//check if we have a instance of this object which havn't reached max number
	zObject* ob = getObjectByID(id, upgrade);
	if (ob) {
		number += ob->data.dwNum;
		if (capacity >= number) { //have enough space to put this 
			ob->data.dwNum = number;			
			orig_ob = ob;
			return 0;
		}else { //space isn't enough, set max number first, then create new object
			ob->data.dwNum = capacity;
			number -= capacity;
			orig_ob = ob;
		}
	}
	
	while ((int)number > 0) 
	{
		zObject *tmp = zObject::create(o, (number>=capacity)?capacity:number, upgrade);
		if (tmp) 
		{
			tmp->data.bind=1;
			EquipMaker maker(NULL);
			maker.assign(NULL, tmp, tmp->base, true, 2);
			EquipMaker modify(NULL);
			modify.re_upgrade(tmp); //myy 任务给绿装刷新装备星星属性
			
			if (user->packs.addObject(tmp, true, AUTO_PACK)) 
			{
				zObject::logger(tmp->createid,tmp->data.qwThisID,tmp->data.strName,tmp->data.dwNum,tmp->data.dwNum,1,0,NULL,user->id,user->name,"新增",tmp->base,tmp->data.kind,tmp->data.upgrade);
				new_obs.push_back(tmp);		
			}
			else 
			{
				//maybe package is full, stop process, but still return true
				zObject::destroy(tmp);	
				return 1;
			}
		}
		number -= capacity;
	}
	
	return 2;
}

/**
 * \brief 根据物品id调整物品在包裹中的数量,如果大于该物品最大数量则创建一个新物品并且绑定(只用于任务接口请勿随便调用)
 * \param user: 要改变包裹的用户
 * \param id: 物品的objectid
 * \param number: 增加的数量
 * \param orig_ob: 数量被改变的物品,如果没有物品数量被改变,该参数被忽略
 * \param new_obs: 新创建的物品的列表,如果没有物品被创建,该参数被忽略
 * \param upgrade: 物品等级
 * \author lqy
 * \return 失败返回-1, 没有物品被创建返回0, 包裹满返回1, 成功添加所有创建的物品返回2
 */
int UserObjectM::addGreenObjectNum(SceneUser* user, DWORD id, DWORD number, zObject* & orig_ob, Obj_vec& new_obs, BYTE upgrade)
{
	//check if exist this object id
	zObjectB* o = objectbm.get(id);
	if (!o) {
		return -1;
	}
	
	DWORD capacity = o->maxnum;
	
	//check if we have a instance of this object which havn't reached max number
	zObject* ob = getObjectByID(id, upgrade);
	if (ob) {
		number += ob->data.dwNum;
		if (capacity >= number) { //have enough space to put this 
			ob->data.dwNum = number;			
			orig_ob = ob;
			return 0;
		}else { //space isn't enough, set max number first, then create new object
			ob->data.dwNum = capacity;
			number -= capacity;
			orig_ob = ob;
		}
	}
	
	while ((int)number > 0) {
		zObject *tmp = zObject::create(o, (number>=capacity)?capacity:number, upgrade);
		if (tmp) {
			tmp->data.bind=1;
			EquipMaker maker(NULL);
			maker.assign(NULL, tmp, tmp->base, true, 6);
			EquipMaker modify(NULL);
			modify.re_upgrade(tmp); //myy 任务给绿装刷新装备星星属性
			
			if (user->packs.addObject(tmp, true, AUTO_PACK)) {
				zObject::logger(tmp->createid,tmp->data.qwThisID,tmp->data.strName,tmp->data.dwNum,tmp->data.dwNum,1,0,NULL,user->id,user->name,"新增",tmp->base,tmp->data.kind,tmp->data.upgrade);
				new_obs.push_back(tmp);		
			}else {
				//maybe package is full, stop process, but still return true
				zObject::destroy(tmp);	
				return 1;
			}
		}
		number -= capacity;
	}
	
	return 2;
}

/**
* \brief 根据物品id调整物品在包裹中的数量,如果大于该物品最大数量则创建一个新物品并且绑定(只用于任务接口请勿随便调用)
* \param user: 要改变包裹的用户
* \param id: 物品的objectid
* \param number: 增加的数量
* \param orig_ob: 数量被改变的物品,如果没有物品数量被改变,该参数被忽略
* \param new_obs: 新创建的物品的列表,如果没有物品被创建,该参数被忽略
* \param upgrade: 物品等级
* \author lqy
* \return 失败返回-1, 没有物品被创建返回0, 包裹满返回1, 成功添加所有创建的物品返回2
*/
int UserObjectM::addPurpleObjectNum(SceneUser* user, DWORD id, DWORD number, zObject* & orig_ob, Obj_vec& new_obs, BYTE upgrade)
{
	//check if exist this object id
	zObjectB* o = objectbm.get(id);
	if (!o) {
		return -1;
	}

	DWORD capacity = o->maxnum;

	//check if we have a instance of this object which havn't reached max number
	zObject* ob = getObjectByID(id, upgrade);
	if (ob) {
		number += ob->data.dwNum;
		if (capacity >= number) { //have enough space to put this 
			ob->data.dwNum = number;			
			orig_ob = ob;
			return 0;
		}else { //space isn't enough, set max number first, then create new object
			ob->data.dwNum = capacity;
			number -= capacity;
			orig_ob = ob;
		}
	}
	
	while ((int)number > 0) 
	{
		zObject *tmp = zObject::create(o, (number>=capacity)?capacity:number, upgrade);
		if (tmp) 
		{
			tmp->data.bind=1;
			EquipMaker maker(NULL);
			maker.assign1(NULL, tmp, tmp->base, true, 22); //soke 任务给的紫装是双灵魂
			EquipMaker modify(NULL);
			modify.repur_upgrade(tmp); //myy 任务给紫装刷新装备星星属性
			
			if (user->packs.addObject(tmp, true, AUTO_PACK)) 
			{
				zObject::logger(tmp->createid,tmp->data.qwThisID,tmp->data.strName,tmp->data.dwNum,tmp->data.dwNum,1,0,NULL,user->id,user->name,"新增",tmp->base,tmp->data.kind,tmp->data.upgrade);
				new_obs.push_back(tmp);		
			}
			else 
			{
				//maybe package is full, stop process, but still return true
				zObject::destroy(tmp);	
				return 1;
			}
		}
		number -= capacity;
	}
	
	return 2;
}

/**
 * \brief 根据物品id调整物品在包裹中的数量
 * \param user: 要改变包裹的用户
 * \param id: 物品的objectid
 * \param number: 减少的数量
 * \param update_ob: 数量被改变的物品,如果没有物品数量被改变,该参数被忽略
 * \param del_obs: 被删除的物品列表
 * \param upgrade: 物品等级
 * \author lqy
 * \return 失败返回-1, 否则返回0
 */
int UserObjectM::reduceObjectNum(SceneUser* user, DWORD id, DWORD number, zObject*& update_ob, ObjID_vec& del_obs, BYTE upgrade)
{
	//check if exist this object id
	zObjectB* o = objectbm.get(id);
	if (!o) 
	{
		return -1;
	}
	
	for(hashmap::iterator it=ets.begin();it!=ets.end();)
	{
		zObject* ob = (zObject *)it->second;
		if (ob->data.pos.loc()!=Cmd::OBJECTCELLTYPE_COMMON && ob->data.pos.loc()!=Cmd::OBJECTCELLTYPE_PACKAGE) 
		{
			++it;
			continue;
		}
		if ( ob->data.dwObjectID == id && ob->data.upgrade == upgrade ) 
		{
			if (number >= ob->data.dwNum) 
			{
				hashmap::iterator tmp = it;
				++tmp;
				number -= ob->data.dwNum;
				del_obs.push_back(ob->data.qwThisID);
				user->packs.removeObject(ob, false, true); //delete but not notify 删除但不通知
				it = tmp;
				continue;
			}
			else 
			{
				ob->data.dwNum -= number;
				number = 0;
				update_ob = ob;
				return 0;
			}
		}
		++it;

	}
	
	return number?-1:0;	
}
