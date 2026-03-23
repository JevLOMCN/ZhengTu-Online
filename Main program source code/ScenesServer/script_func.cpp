/**
 * \file
 * \version	$Id: script_func.cpp $
 * \author	
 * \date	
 * \brief	脚本辅助函数
 * 
 */

#include "script_func.h"
#include "SceneUser.h"

SceneUser* current_user = NULL;


/**     
 * \brief 取得当前用户
 *
 * \return: 当前用户
 */  
SceneUser* me()
{
	return current_user;
}

/**     
 * \brief 设定当前用户
 *
 * \param user: 目标用户
 */  
void set_me(SceneUser* user)
{
	current_user = user;
}

#include "Chat.h"


/**     
 * \brief 脚本接口，传送信息到用户窗口
 *
 * \param target : 目标用户
 * \param type : 讯息类型，see enumSysInfoType from Command.h for details
 * \param msg: 信息内容
 * \return: 传送成功返回true，否则返回false
 */  
bool sys(SceneUser* target, int type, const char* msg)
{
	return Channel::sendSys(target, type, msg);
}

#include "Command.h"
#include "SceneNpc.h"
#include "NpcTrade.h"

/**     
 * \brief 脚本接口，弹出对话框
 *
 * \param npc : 对话的npc
 * \param menu : 对话框内容
 */  
void show_dialog(SceneNpc* npc, const char* menu)
{
	BYTE buf[4096];
	Cmd::stVisitNpcTradeUserCmd *cmd=(Cmd::stVisitNpcTradeUserCmd *)buf;
	memset(buf, 0, sizeof(buf));
	constructInPlace(cmd);
	
	strncpy(cmd->menuTxt, menu, strlen(menu));	
	if (npc && NpcTrade::getInstance().getNpcMenu(npc->id, cmd->menuTxt+strlen(menu))) {
		//intended to be blank
	}
	cmd->byReturn = 1;
	
	if (current_user) {
		current_user->sendCmdToMe(cmd, sizeof(Cmd::stVisitNpcTradeUserCmd) + strlen(cmd->menuTxt));
	}
}

#include "Quest.h"
#include "QuestAction.h"

/**     
 * \brief 脚本接口，设定一个变量值
 *
 * \param vars : 目标变量
 * \param name : 变量名称
 * \param value: 变量值
 */ 
void set_var(Vars* vars, const char* name, int value)
{
	Op::Set<int> op;
	vars->set_value(op, name, value, 0, current_user);
	
}

/**     
 * \brief 脚本接口，设定一个变量值
 *
 * \param vars : 目标变量
 * \param name : 变量名称
 * \param value: 变量值
 */ 
void set_varS(Vars* vars, const char* name, const char * value)
{
	Op::Set< std::string> op;
	std::string s=value;
	vars->set_value(op, name, s, 0, current_user);
}

/**     
 * \brief 脚本接口，取得一个变量值
 *
 * \param vars : 目标变量
 * \param name : 变量名称
 * \return: 取得的变量值，若变量不存在返回0
 */ 
int get_var(Vars* vars, const char* name)
{
	int value;
	if (vars->get_value(name, value)) return value;
	
	return 0;
}

/**     
 * \brief 脚本接口，取得一个变量值
 *
 * \param vars : 目标变量
 * \param name : 变量名称
 * \return: 取得的变量值，若变量不存在返回0
 */ 
const char *  get_varS(Vars* vars, const char* name)
{
	std::string value;
	if (vars->get_value(name, value)) return value.c_str();
	
	return 0;
}

/**     
 * \brief 脚本接口，刷新npc状态
 *
 * \param npc : 目标npc
 */ 
void refresh_status(SceneNpc* npc)
{
	npc->set_quest_status(current_user);

	Cmd::stAddMapNpcMapScreenUserCmd cmd;
	npc->full_t_MapNpcData(cmd.data);
	current_user->sendCmdToMe(&cmd, sizeof(Cmd::stAddMapNpcMapScreenUserCmd));
}

/**     
 * \brief 脚本接口，刷新npc状态
 *
 * \param npc : 目标npc的tempid
 */ 
void refresh_npc(int id)
{
	SceneNpc *sceneNpc = SceneNpcManager::getMe().getNpcByTempID((DWORD)id);	
	if (sceneNpc) refresh_status(sceneNpc);

}

/**     
 * \brief 脚本接口，取得某npc的tempid
 *
 * \param npc : 目标npc
 *\return : 该npc的tempid
 */ 
int npc_tempid(SceneNpc* npc)
{
	return npc->tempid;
}

/**     
 * \brief 脚本接口，取得某npc的id
 *
 * \param npc : 目标npc
 *\return : 该npc的id
 */ 
int npc_id(SceneNpc* npc)
{
	return npc->id;
}

/**     
 * \brief 脚本接口，刷新任务信息
 *
 * \param npc : 目标任务id
 */ 
void refresh_quest(int id)
{
	Cmd::stAbandonQuestUserCmd ret;
	ret.id = id;
	current_user->sendCmdToMe(&ret, sizeof(ret));
}

#define U_ID 98765

/**     
 * \brief 脚本接口，取得全局变量，若变量不存在，则新建
 *
 *\return : 得到的全局变量
 */ 
Vars* GlobalVars::add_g()
{
	return GlobalVar::instance().add(U_ID);
}

/**     
 * \brief 脚本接口，取得帮会变量，若变量不存在，则新建
 *
 *\return : 得到的帮会变量
 */ 
Vars* GlobalVars::add_t()
{
	return TongVar::instance().add(current_user->charbase.unionid);
}

/**     
 * \brief 脚本接口，取得家族变量，若变量不存在，则新建
 *
 *\return : 得到的家族变量
 */ 
Vars* GlobalVars::add_f()
{
	return FamilyVar::instance().add(current_user->charbase.septid);
}

/**     
 * \brief 脚本接口，判断用户身上带有的金钱是否满足要求
 *
 * \param user: 目标用户
 * \param money: 需要的金钱
 * \return : 满足条件返回ture，否则返回false
 */ 
bool check_money(SceneUser* user, int money)
{
	return user->packs.checkMoney(money);
}

/**     
 * \brief 脚本接口，较少用户身上带有的金钱数量
 *
 * \param user: 目标用户
 * \param money: 需要的金钱
 * \return : 较少成功返回ture，否则返回false
 */ 
bool remove_money(SceneUser* user, int money)
{
	return user->packs.removeMoney(money, "任务");
}

/**     
 * \brief 脚本接口，增加用户身上带有的金钱数量
 *
 * \param user: 目标用户
 * \param money: 增加的金钱
 */ 
void add_money(SceneUser* user, int money)
{
	user->packs.addMoney(money, "任务");
}



/**     
 * \brief 脚本接口，判断用户身上是否带有某物品
 *
 * \param user: 目标用户
 * \param id: 物品的objectid
 * \param number: 要求的数量
 * \param level: 物品等级或类型
 * \return 包裹中物品数量大于等于给定的数量返回true，否则返回false
 */ 
bool have_ob(SceneUser* user, int id, int num, int level ,int type)
{
	return user->packs.uom.exist(id, num, level ,type);
}

/**     
 * \brief 脚本接口，判断用户身上是否带有某物品
 *
 * \param user: 目标用户
 * \param id: 物品的objectid
 * \param level: 物品类型
 * \return 包裹中有物品返回物品thisid，否则返回0
 */ 
unsigned int get_ob(SceneUser* user, int id, int level )
{
	return user->packs.uom.exist(id, 1, level ,1);
}

/**     
 * \brief 脚本接口，删除物品
 *
 * \param user: 目标用户
 * \param id: 物品的thisid
 * \return 删除是否成功
 */ 
bool del_ob(SceneUser* user, unsigned int id)
{
	zObject *delObj=user->packs.uom.getObjectByThisID(id);
	return user->packs.removeObject(delObj , true,true);
}

/**     
 * \brief 脚本接口，取得用户包裹中的剩余空间
 *
 * \param user: 目标用户
 * \return 用户包裹所拥有的剩余空间
 */ 
int  space(SceneUser* user)
{
	return user->packs.uom.space(user);
}

/**
 * \brief 脚本接口，取得指定用户家族变量，若变量不存在，则新建
 *
 *\return : 得到的用户家族变量
 */
Vars * get_familyvar(SceneUser* user, int dummy)
{
	return FamilyVar::instance().add(user->charbase.septid);
}

/**
 * \brief 脚本接口，取得指定用户变量，若变量不存在，则新建
 *
 *\return : 得到的用户变量
 */
Vars * get_uservar(SceneUser* user, int dummy)
{
	return UsersVar::instance().add(((QWORD)user->charbase.accid << 32) |user->charbase.id);
}

/**
 * \brief 脚本接口，取得指定用户行会变量，若变量不存在，则新建
 *
 *\return : 得到的用户行会变量
 */
Vars * get_tongvar(SceneUser* user, int dummy)
{
	return TongVar::instance().add(user->charbase.unionid);
}

void add_exp(SceneUser* user,DWORD num, bool addPet, DWORD dwTempID, BYTE byType, bool addCartoon)
{
	if(user)
	{
		user->addExp(num,addPet,dwTempID,byType,addCartoon);
		Zebra::logger->debug("[得到环式任务经验][%s(%u)][%ld]", user->charbase.name, user->id, num);
	}
}

/**     
 * \brief 脚本接口，取得系统当前时间
 *
 * \return 系统当前时间
 */ 
int get_time()
{
	tzset();
	return time(NULL)-timezone;
}

/**     
 * \brief 脚本接口，取得两个时间的差
 *
 * \param t1 : 待比较时间1
 * \param t2 : 待比较时间2
 * \return 两个时间的差
 */ 
double diff_time(int t1, int t2)
{
	return difftime(t1, t2);
}

//cccccccccccccccccccccc
/**
 * @brief 脚本接口，发送全服消息
 * @param 消息类型 {DWORD} infoType
 * @param 消息内容 {char*} msg
 * @return 传送成功返回true，否则返回false
 */
bool all_sys(DWORD infoType, const char* msg)
{
	return Channel::sendAllInfo(infoType, msg);
}

/**
 * @brief 脚本接口，弹出物品对话框
 * @param 对话内容 {char*} menu
 * @return {*}
 */
void show_obj_dialog(const char* menu)
{
	BYTE buf[4096];
	Cmd::stVisitNpcTradeUserCmd *cmd=(Cmd::stVisitNpcTradeUserCmd *)buf;
	memset(buf, 0, sizeof(buf));
	constructInPlace(cmd);
	
	strncpy(cmd->menuTxt, menu, strlen(menu));	

	cmd->byReturn = 1;
	
	if (current_user) {
		current_user->sendCmdToMe(cmd, sizeof(Cmd::stVisitNpcTradeUserCmd) + strlen(cmd->menuTxt));
	}
}

/**
 * @brief 掉落绿色装备
 * @param 掉落的怪物 {SceneNpc*} npc
 * @param 物品ID {int} id
 * @param 物品等级 {int} xingxing
 * @return {*}
 */
void drop_object(SceneNpc* npc, int id,DWORD num,int kind,int upgrade)
{	
		SceneUser* user = me();
		zObjectB *base = objectbm.get(id);
		if (base)
		{
			if (num>(DWORD)base->maxnum) num = base->maxnum;
			zObject *o = zObject::create(base,num);
			if (o)
			{	o->data.upgrade = upgrade;
				if(o->base->make == 1)
				{
					base->goldrating = 10000;
					base->holyrating = 100000;
					o->data.kind = kind; //设置品质4绿装
					EquipMaker maker(NULL);
					maker.modify_attri(o,base);
					maker.re_upgrade(o);
				}
				user->scene->addObject(o, npc->getPos(), 0, user->id, 30);
			}
		}
		//Zebra::logger->debug("当前场景:%s", user->scene->getFileName());		
}

/**
 * @brief 脚本接口，击杀怪物后召唤怪物
 * @param 击杀的怪物 {SceneNpc*} npc
 * @param 召唤的怪物 {int} npcid
 * @return {*}
 */
void summon_npc(SceneNpc* npc, int npcid)
{
	zNpcB *base = npcbm.get(npcid);
	t_NpcDefine define;
	zPos pos = npc->getPos();
	define.id = base->id;
	strcpy(define.name, base->name);
	define.pos = npc->getPos();
	define.width = SCREEN_WIDTH;
	define.height = SCREEN_HEIGHT;
	define.num = 1;
	define.interval = 30;
	define.initstate = zSceneEntry::SceneEntry_Normal;

	npc->scene->zPosRevaluate(define.pos);
	define.pos -= zPosHalfScreen;
	npc->scene->zPosRevaluate(define.pos);
	pos -= zPosHalfScreen;
	npc->scene->zPosRevaluate(pos);
	npc->scene->initRegion(define.region, define.pos, define.width, define.height);
	npc->scene->summonNpc(define, pos, base);
}

/**
 * @brief 脚本接口，获取身上穿戴的装备
 * @param 玩家 {SceneUser*} user
 * @param 装备格子 {int} id
 * @return {*}
 */
zObject *get_EquipNo(SceneUser* user,int id)
{
	return user->packs.equip.getObjectByEquipPos(id);
}

/**
 * @brief 脚本接口，弹出输入框
 * @param 玩家 {SceneUser*} user
 * @param 回调函数 {char*} funname
 * @param 提示信息 {char*} text
 * @return {*}
 */
void lua_InputBox(SceneUser* user,const char* funname,const char* text,const char* funcparam)
{
	Cmd::stLuaInputBoxUserCmd send; 
	//strcpy(send.funName,funname);
	if (strlen(funname)+strlen(funcparam)+2 < sizeof(send.funName))
	{
		sprintf(send.funName,"%s-%s",funname,funcparam);
	}else{
		strcpy(send.funName,funname);
	}
	strcpy(send.text,text);
	user->sendCmdToMe(&send,sizeof(send));
}

/**
 * @brief 获取角色国家名
 * @param 角色 {SceneUser*} user
 * @return 国家名 {string}
 */

std::string get_countryname(SceneUser* user)
{
	SceneManager::CountryMap_iter src_pos = SceneManager::getInstance().country_info.find(user->charbase.country);
	return src_pos->second.name;
}

/**
 * @brief 检查金子是否足够
 * @param 角色 {SceneUser*} user
 * @param 数量 {int} money
 * @return 足够返回真 {*}
 */
bool check_Gold(SceneUser* user, int money)
{
	return user->packs.checkGold(money);
}

/**
 * @brief 扣除金子
 * @param 角色 {SceneUser*} user
 * @param 数量 {int} money
 * @return 成功返回真 {*}
 */
bool remove_Gold(SceneUser* user, int money)
{
	return user->packs.removeGold(money, "Lua");
}

/**
 * @brief 增加金子
 * @param 角色 {SceneUser*} user
 * @param 数量 {int} money
 * @return 成功返回真 {*}
 */
void add_Gold(SceneUser* user, int money)
{
	user->packs.addGold(money, "Lua");
}

/**
 * @brief 检查工资是否足够
 * @param 角色 {SceneUser*} user
 * @param 数量 {int} money
 * @return 足够返回真 {*}
 */
bool check_Salary(SceneUser* user, int money)
{
	return user->packs.checkSalary(money);
}

/**
 * @brief 扣除工资
 * @param 角色 {SceneUser*} user
 * @param 数量 {int} money
 * @return 成功返回真 {*}
 */
bool remove_Salary(SceneUser* user, int money)
{
	return user->packs.removeSalary(money, "Lua");
}

/**
 * @brief 增加工资
 * @param 角色 {SceneUser*} user
 * @param 数量 {int} money
 * @return 成功返回真 {*}
 */
void add_Salary(SceneUser* user, int money)
{
	user->packs.addSalary(money, "Lua");
}
/**
 * @brief 检查积分是否足够
 * @param 角色 {SceneUser*} user
 * @param 数量 {int} money
 * @return 足够返回真 {*}
 */
bool check_Ticket(SceneUser* user, int money)
{
	return user->packs.checkTicket(money);
}

/**
 * @brief 扣除积分
 * @param 角色 {SceneUser*} user
 * @param 数量 {int} money
 * @return 成功返回真 {*}
 */
bool remove_Ticket(SceneUser* user, int money)
{
	return user->packs.removeTicket(money, "Lua");
}

/**
 * @brief 增加积分
 * @param 角色 {SceneUser*} user
 * @param 数量 {int} money
 * @return 成功返回真 {*}
 */
void add_Ticket(SceneUser* user, int money)
{
	user->packs.addTicket(money, "Lua");
}
//////////////////充值点开始///////////////////////////////
/**
 * @brief 检查充值点是否足够
 * @param 角色 {SceneUser*} user
 * @param 数量 {int} money
 * @return 足够返回真 {*}
 */
bool check_Chongzhi(SceneUser* user, int money)
{
	return user->packs.checkChongzhi(money);
}

/**
 * @brief 扣除充值点
 * @param 角色 {SceneUser*} user
 * @param 数量 {int} money
 * @return 成功返回真 {*}
 */
bool remove_Chongzhi(SceneUser* user, int money)
{
	return user->packs.removeChongzhi(money, "Lua");
}

/**
 * @brief 增加充值点
 * @param 角色 {SceneUser*} user
 * @param 数量 {int} money
 * @return 成功返回真 {*}
 */
void add_Chongzhi(SceneUser* user, int money)
{
	user->packs.addChongzhi(money, "Lua");
}
/////////////////充值点结束///////////////////////////////
/**
 * @brief 获取全局变量
 * @return {*}
 */
Vars* get_GlobalVars()
{
	return GlobalVar::instance().add(U_ID);
}

void make_object(SceneUser* user, luabind::object table)
{
	if(luabind::type(table["编号"]) == LUA_TNIL)
		return;
	int obid = luabind::object_cast<int>(table["编号"]);
	zObjectB *base = objectbm.get(obid);
	if (base)
	{
		zObject *o = zObject::create(base,1);
		if (o)
		{	
			if(o->base->make == 1)
			{	
				base->goldrating = 10000;
				base->holyrating = 100000;
				if(luabind::type(table["品质"]) != LUA_TNIL)
				{
					int kind = luabind::object_cast<int>(table["品质"]);
					o->data.kind = kind; //设置品质4绿装
				}
				EquipMaker maker(NULL);
				maker.modify_attri(o,base);
				maker.modify_addmaker(o,user);
				o->data.bind = 1;
				if(luabind::type(table["星星"]) != LUA_TNIL)
				{
					int upgrade = luabind::object_cast<int>(table["星星"]);
					o->data.upgrade = upgrade;
					if(o->data.upgrade > 0)
					{
						if (o->data.kind & 16)
							maker.repur_upgrade(o);
						else
							maker.re_upgrade(o);
					}
				}

				if(luabind::type(table["五行属性"]) != LUA_TNIL)
				{
					int fivetype = luabind::object_cast<int>(table["五行属性"]);
					o->data.fivetype =  fivetype < 5 ? fivetype : 5;
				}
				if(luabind::type(table["灵魂物减"]) != LUA_TNIL)
				{
					o->data.dpdam = 0;
					o->data.dmdam = 0;
					o->data.bdam = 0;
					o->data.rdam = 0;
					o->data.ignoredef = 0;
					int dpdam = luabind::object_cast<int>(table["灵魂物减"]);
					o->data.dpdam =  dpdam;
				}
				if(luabind::type(table["灵魂魔减"]) != LUA_TNIL)
				{
					o->data.dpdam = 0;
					o->data.dmdam = 0;
					o->data.bdam = 0;
					o->data.rdam = 0;
					o->data.ignoredef = 0;
					int dmdam = luabind::object_cast<int>(table["灵魂魔减"]);
					o->data.dmdam =  dmdam;
				}
				if(luabind::type(table["灵魂增伤"]) != LUA_TNIL)
				{
					o->data.dpdam = 0;
					o->data.dmdam = 0;
					o->data.bdam = 0;
					o->data.rdam = 0;
					o->data.ignoredef = 0;
					int bdam = luabind::object_cast<int>(table["灵魂增伤"]);
					o->data.bdam =  bdam;
				}
				if(luabind::type(table["灵魂反射"]) != LUA_TNIL)
				{
					o->data.dpdam = 0;
					o->data.dmdam = 0;
					o->data.bdam = 0;
					o->data.rdam = 0;
					o->data.ignoredef = 0;
					int rdam = luabind::object_cast<int>(table["灵魂反射"]);
					o->data.rdam =  rdam;
				}
				if(luabind::type(table["灵魂忽视"]) != LUA_TNIL)
				{
					o->data.dpdam = 0;
					o->data.dmdam = 0;
					o->data.bdam = 0;
					o->data.rdam = 0;
					o->data.ignoredef = 0;
					int ignoredef = luabind::object_cast<int>(table["灵魂忽视"]);
					o->data.ignoredef =  ignoredef;
				}
				if (o->data.kind & 16)
				{
					if(luabind::type(table["二灵魂物防"]) != LUA_TNIL)
					{
						o->data.addpdef = 0;
						o->data.addmdef = 0;
						o->data.addpdam = 0;
						o->data.addmdam = 0;
						o->data.addhp = 0;
						int addpdef = luabind::object_cast<int>(table["二灵魂物防"]);
						o->data.addpdef =  addpdef;
					}
					if(luabind::type(table["二灵魂魔防"]) != LUA_TNIL)
					{
						o->data.addpdef = 0;
						o->data.addmdef = 0;
						o->data.addpdam = 0;
						o->data.addmdam = 0;
						o->data.addhp = 0;
						int addmdef = luabind::object_cast<int>(table["二灵魂魔防"]);
						o->data.addmdef =  addmdef;
					}
					if(luabind::type(table["二灵魂物攻"]) != LUA_TNIL)
					{
						o->data.addpdef = 0;
						o->data.addmdef = 0;
						o->data.addpdam = 0;
						o->data.addmdam = 0;
						o->data.addhp = 0;
						int addpdam = luabind::object_cast<int>(table["二灵魂物攻"]);
						o->data.addpdam =  addpdam;
					}
					if(luabind::type(table["二灵魂魔攻"]) != LUA_TNIL)
					{
						o->data.addpdef = 0;
						o->data.addmdef = 0;
						o->data.addpdam = 0;
						o->data.addmdam = 0;
						o->data.addhp = 0;
						int addmdam = luabind::object_cast<int>(table["二灵魂魔攻"]);
						o->data.addmdam =  addmdam;
					}
					if(luabind::type(table["二灵魂生命"]) != LUA_TNIL)
					{
						o->data.addpdef = 0;
						o->data.addmdef = 0;
						o->data.addpdam = 0;
						o->data.addmdam = 0;
						o->data.addhp = 0;
						int addhp = luabind::object_cast<int>(table["二灵魂生命"]);
						o->data.addhp =  addhp;
					}
				}
				//user->packs.addObject(o,true,1);
				user->packs.addObject(o,true,AUTO_PACK); //领取扩展主包袱和乾坤袋
				Base::add_object(*user,o);
			}
		}
	}
}

void add_actionpoint(SceneUser* user,int num)
{
	Cmd::Session::t_ChangeAP  cmd;
	cmd.id = user->charbase.unionid;
	cmd.point = num;
	sessionClient->sendCmd(&cmd, sizeof(cmd));
}
