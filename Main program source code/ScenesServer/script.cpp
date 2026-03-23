/**
 * \file
 * \version	$Id: script.cpp  $
 * \author	
 * \date	
 * \brief	脚本，新的任务引擎
 * 
 */
#include "script.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include <luabind/luabind.hpp>

//------------------------------------------------------
LuaScript::LuaScript() 
{
}

//------------------------------------------------------
LuaScript::~LuaScript()
{
}

//------------------------------------------------------
/**     
 * \brief 判断脚本是否被载入
 *
  * \return 已经载入返回true，否则返回false
 */   
bool LuaScript::isLoaded() 
{
	return mFileName.size() != 0;
}
	
//------------------------------------------------------
/**     
 * \brief 设置脚本文件
 *
 */   
void LuaScript::setData( const std::string& rData )
{
	mFileName = rData;
}

/**     
 * \brief 取得脚本文件名
 *
  * \return 脚本文件名称
 */   
//------------------------------------------------------
const std::string& LuaScript::getData() const 
{
	return mFileName;
}

#define IS_MASK_SET(val, mask) ((mask & val) == mask)

//------------------------------------------------------
/**     
 * \brief 构造函数，创建一个虚拟机上下文用于执行脚本
 * \param libs: 用于指定在虚拟机可用的lua库
 */   
LuaVM::LuaVM( DWORD libs ) 
{
	mLuaState = lua_open();
     #if LUA_VERSION_NUM >=501
	if (IS_MASK_SET(libs, LUALIB_BASE))
		lua_cpcall(mLuaState,luaopen_base,0);		
	if (IS_MASK_SET(libs, LUALIB_TABLE))
		lua_cpcall(mLuaState,luaopen_table,0);			
	if (IS_MASK_SET(libs, LUALIB_IO))
		lua_cpcall(mLuaState,luaopen_io,0);			
	if (IS_MASK_SET(libs, LUALIB_STRING))
		lua_cpcall(mLuaState,luaopen_string,0);		
	if (IS_MASK_SET(libs, LUALIB_MATH))
		lua_cpcall(mLuaState,luaopen_math,0);			
	if (IS_MASK_SET(libs, LUALIB_DEBUG))
		lua_cpcall(mLuaState,luaopen_debug,0);
	if (IS_MASK_SET(libs,LUALIB_PACKAGE))
		lua_cpcall(mLuaState,luaopen_package,0);
     #else
	if (IS_MASK_SET(libs, LUALIB_BASE))
		luaopen_base( mLuaState );
	if (IS_MASK_SET(libs, LUALIB_TABLE))
		luaopen_table( mLuaState );
	if (IS_MASK_SET(libs, LUALIB_IO))
		luaopen_io( mLuaState );
	if (IS_MASK_SET(libs, LUALIB_STRING))
		luaopen_string( mLuaState );
	if (IS_MASK_SET(libs, LUALIB_MATH))
		luaopen_math( mLuaState );
	if (IS_MASK_SET(libs, LUALIB_DEBUG))
		luaopen_debug( mLuaState );
     #endif
	luabind::open( mLuaState );
}

//------------------------------------------------------
/**     
 * \brief 析构函数，关闭虚拟机
 *
 */   
LuaVM::~LuaVM()
{
	if (mLuaState)
	{
		lua_close( mLuaState );
		mLuaState = 0;
	}
}

//------------------------------------------------------
/**     
 * \brief 执行一段脚本
 *
 * \param rData: 被执行的脚本内容
 */   
void LuaVM::execute( const std::string & rData )
{
	if (mLuaState)
	#if LUA_VERSION_NUM >=501
		luaL_dostring( mLuaState, rData.c_str() );
	#else
		lua_dostring( mLuaState, rData.c_str() );
	#endif
}

//------------------------------------------------------
//------------------------------------------------------
/**     
 * \brief 执行一段脚本
 *
 * \param pScript: 被执行的脚本
 */   
void LuaVM::execute( LuaScript* pScript )
{

	const std::string& rFileName = pScript->getData();
	#if LUA_VERSION_NUM >=501
	int ret = luaL_dofile( mLuaState, rFileName.c_str() );
	#else
	int ret = lua_dofile( mLuaState, rFileName.c_str() );
	#endif 
	
	if (ret==0)
	{
		Zebra::logger->info("加载lua文件成功");
	}else
	{
		Zebra::logger->info("FATAL ERROR:%s", lua_tostring(mLuaState, -1));
	}
}

ScriptingSystemLua* ScriptingSystemLua::_instance = NULL;

//------------------------------------------------------
/**     
 * \brief 单键模式，用于取得脚本系统接口
 *
 * \return 脚本系统接口
 */  
ScriptingSystemLua& ScriptingSystemLua::instance()
{
	if (!_instance) {
		_instance = new ScriptingSystemLua();
	}
	
	return *_instance;
}

//------------------------------------------------------
ScriptingSystemLua::ScriptingSystemLua()
{
}

//------------------------------------------------------
/**     
 * \brief 析构函数，清除已经创建的虚拟机
 *
 */  
ScriptingSystemLua::~ScriptingSystemLua()
{
  mVMs.clear();
}

//------------------------------------------------------
/**     
 * \brief 重新加载lua脚本
 *
 */  
void ScriptingSystemLua::reloadVM()
{
	mVMs.clear();
	LuaVM* vm = createVM();
	LuaScript* script = ScriptingSystemLua::instance().createScriptFromFile("newquest/quest.lua");
	Binder bind;
	bind.bind(vm);
	vm->execute(script);
	SAFE_DELETE(script);
}

//------------------------------------------------------
/**     
 * \brief 创建一个虚拟机
 *
 * \return 被创建的虚拟机
 */  
LuaVM* ScriptingSystemLua::createVM()
{
	LuaVM* pVM = new LuaVM( );
	mVMs.push_back( pVM );
	return pVM;
}

//------------------------------------------------------
/**     
 * \brief 取得一个已经被创建的虚拟机
 *
 * \param index: 要得到的虚拟机索引
 * \return 得到的虚拟机
 */  
LuaVM* ScriptingSystemLua::getVM(int index)
{
	return mVMs[index];
}

//------------------------------------------------------
//------------------------------------------------------
/**     
 * \brief 从给定的文件中创建一个脚本
 *
 * \param rFile: 文件名
 * \return 被创建的脚本
 */  
LuaScript* ScriptingSystemLua::createScriptFromFile( const std::string & rFile )
{
	LuaScript* pLuaScript = new LuaScript( );

	pLuaScript->setData( rFile );

	return pLuaScript;
}

ScriptQuest* ScriptQuest::_instance = NULL;

/**     
 * \brief 单键模式，用于取得系统中唯一的全局索引
 *
 * \return 脚本全局索引
 */  
ScriptQuest& ScriptQuest::get_instance()
{
	if (!_instance) {
		_instance = new ScriptQuest();
	}
	
	return *_instance;
}

/**     
 * \brief 在全局索引中添加一个项
 *
 * \param type : 事件类型
 * \param id : 事件id
 */  
void ScriptQuest::add(int type, int id)
{
	if (!has(type, id)) {
		int h = hash(type, id);
		_sq.insert(h);
	}
}

/**     
 * \brief 判断某事件是否在全局索引中存在
 *
 * \param type : 事件类型
 * \param id : 事件id
 * \return: 存在返回true，否则返回false
 */  
bool ScriptQuest::has(int type, int id) const
{
	return _sq.find(hash(type, id)) != _sq.end();
}

/**     
 * \brief hash函数，根据事件类型和id算出一个唯一值
 *
 * \param type : 事件类型
 * \param id : 事件id
 * \return : hash后得到的唯一值
 */  
int ScriptQuest::hash(int type, int id) const
{
	return ( (type & 0xff) << 24 ) | (id & 0x00ffffff);
}

#include <algorithm>

/**     
 * \brief 排序函数，加速以后的查找
 *
 */  
void ScriptQuest::sort()
{
	//std::stable_sort(_sq.begin(), _sq.end());	
}

/**     
 * \brief 取得全局索引
 *
 * \return : 全局索引
 */  
ScriptQuest& the_script()
{
	return ScriptQuest::get_instance();
}


#include "script_func.h"
#include "Chat.h"
#include "SceneUser.h"
#include "Quest.h"
#include "Scene.h"

extern SceneUser* current_user;

/**     
 * \brief 绑定一个虚拟机，使特定的接口对该虚拟机可用
 *
 * \param vm : 要绑定的虚拟机
 */  
void Binder::bind(LuaVM* vm)
{

#define _MODULE vm->getLuaState()

using namespace luabind;
module (_MODULE)
[
	class_<Channel>( "Channel" )
//		.def( "sys", (bool(Channel::*) (SceneUser*)) &Channel::add )
//		.def( "sys", (bool(Channel::*) (SceneUser*, int,  const char*, ...)) &Channel::sendSys )
];

/*
	INFO_TYPE_SYS	=	1,	/// 系统信息、GM信息
	INFO_TYPE_GAME,		/// 游戏信息
	INFO_TYPE_STATE,		/// 状态转换
	INFO_TYPE_FAIL,			/// 失败信息
	INFO_TYPE_EXP,		/// 特殊信息,获得经验、物品，在人物头上
	INFO_TYPE_MSG,		/// 弹出用户确认框的系统消息
	INFO_TYPE_KING,		/// 国王发出的聊天消息
	INFO_TYPE_CASTELLAN,	/// 城主发出的聊天消息
	INFO_TYPE_SCROLL	/// 屏幕上方滚动的系统信息
*/

module (_MODULE)
[
	def("me", &me),
	def("消息", &sys),
	def("世界消息",&all_sys),
	def("show_dialog", &show_dialog),
	def("show_obj_dialog", &show_obj_dialog),
	def("refresh_npc", &refresh_npc),
	def("the_script", &the_script),
	def("取全局变量", &get_GlobalVars),
	def("取随机数",&zMisc::randBetween),
	def("取百分之几率",&zMisc::selectByPercent),
	def("取万分之几率",&zMisc::selectByTenTh),
	def("取十万分之几率",&zMisc::selectByLakh),
	def("取百万分之几率",&zMisc::selectByMilh),
	//--------------------------------------------------------
	def("time", &get_time),
	def("difftime", &diff_time)


];

int s = Cmd::USTATE_START_QUEST;
int d = Cmd::USTATE_DOING_QUEST;
int f = Cmd::USTATE_FINISH_QUEST;

module (_MODULE)
[	
	class_<std::string>("String")
		,

	class_<t_Object>( "t_Object" )
		.def_readonly("qwThisID",&t_Object::qwThisID)
		.def_readonly("dwObjectID",&t_Object::dwObjectID)
		.def_readonly("strName",&t_Object::strName)
		.def_readwrite("maxhp",&t_Object::maxhp)
		.def_readwrite("maxmp",&t_Object::maxmp)
		.def_readwrite("maxsp",&t_Object::maxsp)
		.def_readwrite("pdamage",&t_Object::pdamage)
		.def_readwrite("maxpdamage",&t_Object::maxpdamage)
		.def_readwrite("mdamage",&t_Object::mdamage)
		.def_readwrite("maxmdamage",&t_Object::maxmdamage)
		.def_readwrite("pdefence",&t_Object::pdefence)
		.def_readwrite("mdefence",&t_Object::mdefence)
		.def_readwrite("damagebonus",&t_Object::damagebonus)
		.def_readwrite("akspeed",&t_Object::akspeed)
		.def_readwrite("mvspeed",&t_Object::mvspeed)
		.def_readwrite("atrating",&t_Object::atrating)
		.def_readwrite("akdodge",&t_Object::akdodge)
		.def_readwrite("fivepoint",&t_Object::fivepoint)
		.def_readwrite("fivetype",&t_Object::fivetype)
		.def_readwrite("bind",&t_Object::bind)
		,

	class_<zObjectB>( "ObjectB" )
		.def_readwrite("maxhp",&zObjectB::maxhp)
		.def_readwrite("maxmp",&zObjectB::maxmp)
		.def_readwrite("maxsp",&zObjectB::maxsp)
		.def_readwrite("pdamage",&zObjectB::pdamage)
		.def_readwrite("maxpdamage",&zObjectB::maxpdamage)
		.def_readwrite("mdamage",&zObjectB::mdamage)
		.def_readwrite("maxmdamage",&zObjectB::maxmdamage)
		.def_readwrite("pdefence",&zObjectB::pdefence)
		.def_readwrite("mdefence",&zObjectB::mdefence)
		.def_readwrite("damagebonus",&zObjectB::damagebonus)
		.def_readwrite("akspeed",&zObjectB::akspeed)
		.def_readwrite("mvspeed",&zObjectB::mvspeed)
		.def_readwrite("atrating",&zObjectB::atrating)
		.def_readwrite("akdodge",&zObjectB::akdodge)
		,

	class_<zObject>( "Object" )
		.def_readwrite("base",&zObject::base)
		.def_readwrite("data",&zObject::data)
		.def_readonly("name", (char*zObject::*)&zObject::name)
		//.def_readonly("scene", (Scene*SceneUser::*)&SceneUser::scene)
		,

	class_<Scene>("Scene")
		.def("country", &Scene::getCountryID)
		,
	
	class_<CharBase>( "CharBase" )
		.def_readonly("等级", &CharBase::level)
		.def_readwrite("五行等级", &CharBase::fivelevel)
		.def_readwrite("五行", &CharBase::fivetype)
		.def_readwrite("荣誉", &CharBase::honor)
		.def_readwrite("最大荣誉", &CharBase::maxhonor)
		.def_readonly("名称", &CharBase::name)		
		.def_readonly("当前地图", &CharBase::mapName)
		.def_readonly("国家", &CharBase::country)
		,

	class_<zPos>("Pos")
		.def(constructor<DWORD, DWORD>())
		,
		
	class_<SceneUser>( "SceneUser" )
		.def("升级", &SceneUser::upgrade )
		.def("增加经验", &add_exp)
		.def("瞬移", &SceneUser::goTo)
		.def("传送", &Gm::gomap)

		.def("增加物品", &SceneUser::addBindObjectNum)
		.def("减少物品", &SceneUser::reduceObjectNum)
		.def("检测物品", &have_ob)
		.def("获取物品", &get_ob)
		.def("删除物品", &del_ob)
		.def("包裹空间", &space)

		.def("检测银子", &check_money)
		.def("增加银子", &add_money)
		.def("减少银子", &remove_money)
		.def("检测金子", &check_Gold)
		.def("增加金子", &add_Gold)
		.def("减少金子", &remove_Gold)
		.def("检测工资", &check_Salary)
		.def("增加工资", &add_Salary)
		.def("减少工资", &remove_Salary)
		.def("检测积分", &check_Ticket)
		.def("增加积分", &add_Ticket)
		.def("减少积分", &remove_Ticket)
		.def("检测充值点", &check_Chongzhi)
		.def("增加充值点", &add_Chongzhi)
		.def("减少充值点", &remove_Chongzhi)

		.def("获取身上装备", &get_EquipNo)
		.def("输入框", &lua_InputBox)
		.def("制造装备", &make_object)

		.def_readwrite("角色信息", &SceneUser::charbase)
		.def_readwrite("任务", &SceneUser::quest_list)
		//.def_readwrite("全局变量", &GlobalVars::add_g)
		.def_readonly("地图", (Scene*SceneUser::*)&SceneUser::scene)
		//.def("爆物", &drop_object)
		//-----------------------------------------------
		.def("取家族变量", &get_familyvar)
		.def("取帮会变量", &get_tongvar)
		.def("取个人变量", &get_uservar)
		.def_readonly("帮会名", &SceneUser::unionName)
		.def_readonly("家族名", &SceneUser::septName)
		.def_readonly("头衔", &SceneUser::caption)
		.def_readonly("是否国王", &SceneUser::king)
		.def_readonly("是否帮主", &SceneUser::unionMaster)
		.def_readonly("是否族长", &SceneUser::septMaster)
		.def_readonly("家族声望", &SceneUser::dwSeptRepute)
		.def_readonly("家族等级", &SceneUser::dwSeptLevel)
		.def_readonly("帮会行动力", &SceneUser::dwUnionActionPoint)
		.def("取国家", &get_countryname)
		.def("增加帮会行动力",&add_actionpoint)
		,

	class_<zNpcB>("NpcBase")
		.def_readonly("level", &zNpcB::level)
		,

	//npc类
	class_<SceneNpc>( "SceneNpc" )
		.def_readonly("base", &SceneNpc::npc)
		.def("refresh", &refresh_status)
		.def("tempid", &npc_tempid)
		.def("id", &npc_id)
		.def("掉落物品", &drop_object)
		.def("召唤怪物", &summon_npc)
		,

	class_<Quest>( "Quest" )
		.enum_("constants")
		[
			value("START", s),
			value("DOING", d),
			value("FINISH", f)
		]
		,

	class_<Vars>("Vars")
		.def(constructor<DWORD>())
		.def("set", &set_var)
		.def("get", &get_var)
		.def("sets", &set_varS)
		.def("gets", &get_varS)
		.def("refresh",(int(Vars::*) (SceneUser&, const std::string&) const)  &Vars::notify)
		,

	class_<GlobalVars>("GlobalVars")
		.def("add_g", &GlobalVars::add_g)
		.def("add_f", &GlobalVars::add_f)
		.def("add_t", &GlobalVars::add_t)
		,
		
	class_<QuestList>("QuestList")
		.def("vars", &QuestList::vars)
		.def("add", &QuestList::add_quest)
		.def("refresh", &refresh_quest)
		,
		
	class_<ScriptQuest>("ScriptQuest")
		.enum_("constants")
		[
			value("NPC_VISIT", 1),
			value("NPC_KILL", 2),
			value("OBJ_USE", 4),
			value("OBJ_GET", 8),
			value("OBJ_DROP", 16)						
		]
		.def("add", &ScriptQuest::add)

];

}


