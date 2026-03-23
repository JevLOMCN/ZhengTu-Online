/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2023-05-17 23:25:21
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2023-06-09 20:40:02
 * @FilePath: \Server\ScenesServer\script_func.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/**
 * \file
 * \version	$Id: script_func.h  $
 * \author	
 * \date	
 * \brief	脚本辅助函数
 * 
 */

#ifndef __QUEST_SCRIPT_FUNC_H__
#define __QUEST_SCRIPT_FUNC_H__

#include <string>
#include <vector>
#include "RebuildObject.h"
#include <luabind/luabind.hpp>

class SceneUser;
class SceneNpc;

SceneUser* me();
void set_me(SceneUser* user);

bool sys(SceneUser* target, int type, const char* msg);

bool all_sys(DWORD infoType, const char* msg);

void show_dialog(SceneNpc* npc, const char* menu);

void show_obj_dialog(const char* menu);

class Vars;

void set_var(Vars* vars, const char* name, int value);
void set_varS(Vars* vars, const char* name, const char * value);

int get_var(Vars* vars, const char* name);
const char * get_varS(Vars* vars, const char* name);

void refresh_status(SceneNpc* npc);

void refresh_npc(int id);

int npc_tempid(SceneNpc* npc);

int npc_id(SceneNpc* npc);

void refresh_quest(int id);



/** brief 提供对脚本中全局变量的支持
	
*/
class GlobalVars
{
public:
	static Vars* add_g();
	static Vars* add_t();
	static Vars* add_f();
};

bool check_money(SceneUser* user, int money);
bool remove_money(SceneUser* user, int money);
void add_money(SceneUser* user, int money);

bool have_ob(SceneUser* user, int id, int num, int level,int type);
unsigned int get_ob(SceneUser* user, int id, int level );
bool del_ob(SceneUser* user, unsigned int id);
int  space(SceneUser* user);

Vars * get_familyvar(SceneUser* user, int dummy);
Vars * get_uservar(SceneUser* user, int dummy);
Vars * get_tongvar(SceneUser* user, int dummy);

void add_exp(SceneUser* user,unsigned int num, bool addPet, unsigned int dwTempID, unsigned char byType, bool addCartoon);
int get_time();
double diff_time(int, int);

//掉落物品
void drop_object(SceneNpc* npc, int id,DWORD num=1,int kind=0,int upgrade=0);
//召唤怪物
void summon_npc(SceneNpc* npc,int npcid);
//获取身上的装备
zObject *get_EquipNo(SceneUser* user,int id);
//脚本调用输入框
void lua_InputBox(SceneUser* user,const char* funname,const char* text,const char* funcparam="");
//获取国家名
std::string get_countryname(SceneUser* user);
//制造装备
void make_object(SceneUser* user, luabind::object table);
void add_actionpoint(SceneUser* user,int num);
//金子
bool check_Gold(SceneUser* user, int money);
bool remove_Gold(SceneUser* user, int money);
void add_Gold(SceneUser* user, int money);

//工资
bool check_Salary(SceneUser* user, int money);
bool remove_Salary(SceneUser* user, int money);
void add_Salary(SceneUser* user, int money);

//积分
bool check_Ticket(SceneUser* user, int money);
bool remove_Ticket(SceneUser* user, int money);
void add_Ticket(SceneUser* user, int money);

//充值点
bool check_Chongzhi(SceneUser* user, int money);
bool remove_Chongzhi(SceneUser* user, int money);
void add_Chongzhi(SceneUser* user, int money);

Vars* get_GlobalVars();

#endif
