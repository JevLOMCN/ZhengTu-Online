/**
 * \file
 * \version	$Id: Quest.cpp $
 * \author	
 * \date		
 * \brief		任务系统
 * 
 */

#include "Quest.h"

#include "QuestEvent.h"
#include "QuestTable.h"
#include "QuestAction.h"

#include "SceneUser.h"
#include "Zebra.h"

#include <algorithm>
#include <time.h>
#include "GuardNpc.h"
#include "Chat.h"
#include "NpcTrade.h"

#include <sstream>

#include "ScenesServer.h"
#include "luabind/luabind.hpp"
#include "script.h"

/**     
 * \brief  解析任务命令
 *
 * 处理客户端请求的任务指令,包括请求任务和放弃任务
 *      
 * \param user: 发出请求的用户
 * \param cmd: 任务指令内容
 * \param len: 任务指令长度
 * \return 处理成功返回true,否则返回false
 */
bool Quest::execute(SceneUser& user, Cmd::stQuestUserCmd* cmd, unsigned int len)
{
	if (user.tradeorder.hasBegin()) {
		user.tradeorder.cancel();
	}
	
	switch (cmd->byParam)	
	{
		case Cmd::REQUEST_QUEST_PARA :
			{			
				Cmd::stRequestQuestUserCmd *request=(Cmd::stRequestQuestUserCmd *)cmd;
				char target = request->target[0];
				DWORD event_id = atoi((char *)&request->target[1]);
				if (target == 'v') {
					OnVisit event(event_id, request->id, request->offset);
					EventTable::instance().execute(user, event);
				}else {
					OnUse event(event_id, request->id, request->offset);
					EventTable::instance().execute(user, event);
				}

				int index = 0;
				for (int i = 0; i < sizeof(request->target); i++)
				{
					if (request->target[i] >= '0' && request->target[i] <= '9')
					{
						index = i;
						break;
					}
				}
				DWORD event_obj_id = atoi((char *)&request->target[index]);
				//Zebra::logger->debug("执行函数(LUA):%s,%d,%d",request->target,request->offset,event_obj_id);
				if (ScriptQuest::get_instance().has(ScriptQuest::OBJ_USE, event_obj_id)) { 
					if (execute_script_event(&user,request->target, request->id, request->offset)) return true;
				}

				BYTE buf[zSocket::MAX_DATASIZE];
				Cmd::stVisitNpcTradeUserCmd *cmd=(Cmd::stVisitNpcTradeUserCmd *)buf;
				bzero(buf, sizeof(buf));
				constructInPlace(cmd);
				
				SceneNpc *sceneNpc = SceneNpcManager::getMe().getNpcByTempID(user.npc_dwNpcTempID);
				if (sceneNpc)
				{
//					if (execute_script_event(request->target, request->id, request->offset)) return true;
					if (request->offset > 0 && ScriptQuest::get_instance().has(ScriptQuest::NPC_VISIT, sceneNpc->id))
						execute_script_event(&user,request->target, sceneNpc, request->id, request->offset);
					
					int status;
					int len = user.quest_list.get_menu(cmd->menuTxt, status);
					
					if (status > 0 && NpcTrade::getInstance().getNpcMenu(sceneNpc->id, cmd->menuTxt+len))
					{
						cmd->byReturn = 1;
						user.sendCmdToMe(cmd, sizeof(Cmd::stVisitNpcTradeUserCmd) + strlen(cmd->menuTxt));
					}
				}/* else {
					
					
					if ((signed char)request->offset < 0) {
						execute_script_event(&user,request->target, sceneNpc, request->id, request->offset);
					}
				} */

				

				return true;

			}
			break;
		
		
		case Cmd::ABANDON_QUEST_PARA :
			{
				Cmd::stAbandonQuestUserCmd *request=(Cmd::stAbandonQuestUserCmd *)cmd;
				if (request->id >= 21000 && request->id <= 22000) 
				{
					return Channel::sendSys(&user, Cmd::INFO_TYPE_SYS, "该任务不能放弃!");
				}
				Quest::abandon(user, request->id);
				user.sendNineToMe(); //refresh quest state
				//user.sendMeToNine();
				/*Cmd::stMainUserDataUserCmd  userinfo;
				user->full_t_MainUserData(userinfo.data);
				user->sendCmdToMe(&userinfo,sizeof(userinfo));
				user->sendMeToNine();*/
				return true;
			}
		case Cmd::CART_CONTROL_QUEST_PARA:
			{
				if (user.guard)
				{
					user.guard->moveAction = !(user.guard->moveAction);
					if (user.guard->canMove())
						Zebra::logger->debug("%s 开始行走",user.guard->name);
					else
						Zebra::logger->debug("%s 停止行走",user.guard->name);
				}
				return true;
			}
			break;
		case Cmd::CHECK_VALID_QUEST_PARA:
			{
				EventManager<OnVisit>::instance().get_valid_quest(user); 
				return true;
			}
			break;
		case Cmd::REQ_VALID_QUEST_PARA:
			{
				EventManager<OnVisit>::instance().get_valid_quest_str(user); 
				return true;
			}
			break;
		default:
			break;
	}
	
	return false;
}

/**     
 * \brief  读取任务
 *
 * 从用户档案中读取任务列表
 *      
 * \param user: 发出请求的用户
 * \param dest: 任务档案
 * \return 读取的任务数目
 */
int Quest::load(SceneUser& user, unsigned char* dest , unsigned long &dest_size)
{
	return user.quest_list.load(dest,dest_size);	
}

/**     
 * \brief 存储任务
 *
 *存贮任务列表到用户档案中
 *      
 * \param user: 发出请求的用户
 * \param dest: 任务档案
 * \return 存储的二进制档案长度
 */
int Quest::save(SceneUser& user, unsigned char* dest)
{
	return user.quest_list.save(dest);
}

/**     
 * \brief 通知任务
 *
 *发送任务信息到用户
 *      
 * \param user: 发出请求的用户
 * \return 当前总是返回0
 */
int Quest::notify(SceneUser& user)
{
	return user.quest_list.notify(user);
}

/**     
 * \brief 放弃任务
 *
 *放弃一个任务
 *      
 * \param user: 发出请求的用户
 * \param id: 任务id
 * \return 成功返回0,失败返回-1
 */
int Quest::abandon(SceneUser& user, DWORD id)
{
	return user.quest_list.abandon(user, id);
}

const std::string Quest::FINISHED_NAME = "state";

/**     
 * \brief 设置时间
 *
 *设置一个任务的时间限制
 *      
 * \return 当前总是返回0
 */
int Vars::set_timer()
{
	_start_time = time(NULL);
	
	return 0;
}

int Vars::set_timer(int start)
{
	_start_time = start;
	
	return 0;
}

/**     
 * \brief 任务开始时间
 *
 *取得任务的开始时间
 *      
 * \return 任务开始时间
 */
int Vars::start_time() const
{
	return _start_time;
}

/**     
 * \brief 任务是否超时
 *
 *判断任务是否超过时间限制
 *      
 * \return 任务超过时间限制返回true,否则返回false
 */
bool Vars::is_timeout(int timeout) const
{
	if (_start_time && (timeout + _start_time > time(NULL)) ) 
	{
		return true;
	}
	
	return false;
}

/**     
 * \brief 存储时间
 *
 *存储任务的开始时间和时间限制
 *      
 * \param dest: 任务档案
 * \return 存储的二进制档案长度
 */
int Vars::save_timer(unsigned char* dest) const
{
	int len = 0;
	memcpy(dest+len, &_timeout, sizeof(_timeout));
	len += sizeof(_timeout);
	memcpy(dest+len, &_start_time, sizeof(_start_time));
	len += sizeof(_start_time);
	
	return len;
}

/**     
 * \brief 读取时间
 *
 *读取任务的开始时间和时间限制
 *      
 * \param dest: 任务档案
 * \return 读取的二进制档案长度
 */
int Vars::load_timer(unsigned char* dest)
{
	int len = 0;
	memcpy(&_timeout, (dest+len), sizeof(int));
	len += sizeof(int);
	memcpy(&_start_time, (dest+len), sizeof(int));
	len += sizeof(int);
	
	return len;
}

/**     
 * \brief 存储变量
 *
 *存储任务变量
 *      
 * \param dest: 任务档案
 * \return 存储的二进制档案长度
 */
int Vars::save(unsigned char* dest) const
{
	int length = save_timer(dest);
	
	int len = length + sizeof(int);
	int count = 0;
	for (const_var_iterator it=_vars.begin(); it!=_vars.end(); ++it) 
	{
		if (!it->second.is_tmp()) 
		{
			++count;
			int tmp = it->first.length();
			memcpy(dest+len, &tmp, sizeof(int));
			len += sizeof(int);
			memcpy(dest+len, it->first.c_str(), it->first.length());
			len += it->first.length();

			tmp = it->second.value().length();
			memcpy(dest+len, &tmp, sizeof(int));
			len += sizeof(int);
			memcpy(dest+len, it->second.value().c_str(), it->second.value().length());
			len += it->second.value().length();			
			
			//Zebra::logger->debug("存储变量(%s:%s)", it->first.c_str(), it->second.value().c_str());
		}
	}
	//store count
	memcpy(dest + length, &count, sizeof(int));	

	return len;
}

/**     
 * \brief 读取变量
 *
 *读取任务变量
 *      
 * \param dest: 任务档案
 * \return 读取的二进制档案长度
 */
int Vars::load(unsigned char* dest)
{
	int length = load_timer(dest);

	int* count = (int*)(dest + length);
	length += sizeof(int);

	while ( (*count)-- > 0) {
		int* len = (int*)(dest + length);
		length += sizeof(int);

		std::string name((char *)dest+length, *len);
		length += *len;
		
		len = (int *)(dest + length);
		length += sizeof(int);

		std::string value((char *)dest+length, *len);
		length += *len;
		
		_vars[name] = VAR(value);
		
		//Zebra::logger->debug("读取变量(%s:%s)", name.c_str(), value.c_str());
	}

	int states = state();
	if (states == Quest::FINISHED_NOT_SAVE || states == Quest::FINISHED_SAVE) update(1);
	
	return length;
}

/**     
 * \brief 任务是否需要更新
 *
 *判断是否需要通知客户端任务更新信息
 *      
 * \return 1表示需要通知客户端,0表示不需要
 */
int Vars::update() const
{
	return _update;
}

/**     
 * \brief 任务是否需要更新
 *
 *设置是否需要通知客户端任务更新信息
 *      
 * \param value: 是否需要通知客户端
 * \return 无
 */
void Vars::update(int value)
{
	_update = value;
}

/**     
 * \brief 通知任务变量
 *
 *发送所有任务变量信息到用户
 *      
 * \param user: 发出请求的用户
 * \return 当前总是返回0
 */
int Vars::notify(SceneUser& user) const
{
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::stQuestVarsUserCmd* vars = (Cmd::stQuestVarsUserCmd*)buf;
	constructInPlace(vars);
	vars->id = _quest_id;
	vars->count = _vars.size();
	
	int offset=0;
	for (const_var_iterator it=_vars.begin(); it!=_vars.end(); ++it) {
		if (! it->first.compare(Quest::FINISHED_NAME)) continue;
		if ( offset >= (int)(zSocket::MAX_DATASIZE - sizeof(Cmd::stQuestVarsUserCmd)
					- (Cmd::stQuestVarsUserCmd::MAX_NSIZE + Cmd::stQuestVarsUserCmd::MAX_VSIZE) ) ) {
			Zebra::logger->error("任务(%d)变量数太多", _quest_id);
			break;
		}
		//name
		strncpy((char*)vars->vars_list+offset, it->first.c_str(), Cmd::stQuestVarsUserCmd::MAX_NSIZE);
		offset += Cmd::stQuestVarsUserCmd::MAX_NSIZE;
		//value
		strncpy((char*)vars->vars_list+offset, it->second.value().c_str(), Cmd::stQuestVarsUserCmd::MAX_VSIZE);
		offset += Cmd::stQuestVarsUserCmd::MAX_VSIZE;
		//Zebra::logger->debug("var:(%s, %s)", it->first.c_str(), it->second.value().c_str());
	}
	
	user.sendCmdToMe(vars , sizeof(Cmd::stQuestVarsUserCmd)+ vars->count*sizeof(Cmd::stQuestVarsUserCmd::Var));
	
	return 0;	
}

/**     
 * \brief 通知任务变量
 *
 *发送特定任务变量信息到用户
 *      
 * \param user: 发出请求的用户
 * \param name: 变量名
 * \return 0 表示发送成功,-1表示失败
 */
int Vars::notify(SceneUser& user, const std::string& name) const
{	
	const_var_iterator it=_vars.find(name);
	if (it != _vars.end()) {
		BYTE buf[sizeof(Cmd::stQuestVarsUserCmd) + (Cmd::stQuestVarsUserCmd::MAX_NSIZE + Cmd::stQuestVarsUserCmd::MAX_VSIZE)];
		Cmd::stQuestVarsUserCmd* vars = (Cmd::stQuestVarsUserCmd*)buf;
		constructInPlace(vars);
		vars->id  = _quest_id;
		vars->count = 1;
		
		int offset = 0;
		//name
		strncpy((char *)vars->vars_list+offset, it->first.c_str(), Cmd::stQuestVarsUserCmd::MAX_NSIZE);
		offset += Cmd::stQuestVarsUserCmd::MAX_NSIZE;
		//value
		strncpy((char *)vars->vars_list+offset, it->second.value().c_str(), Cmd::stQuestVarsUserCmd::MAX_VSIZE);
		offset += Cmd::stQuestVarsUserCmd::MAX_VSIZE;
		user.sendCmdToMe(vars , sizeof(Cmd::stQuestVarsUserCmd)+ vars->count*sizeof(Cmd::stQuestVarsUserCmd::Var));
		//Zebra::logger->debug("var:(%s, %s)", it->first.c_str(), it->second.value().c_str());
		return 0;
	}
	
	
	return -1;	
}

std::string Vars::info() const
{
	std::ostringstream os;
	for (const_var_iterator it=_vars.begin(); it!=_vars.end(); ++it) {
		os << it->first << ":" << it->second.value() << "\t";
	}

	return os.str();
}

bool Vars::reserve()
{
	bool flag = false;
	for (var_iterator it=_vars.begin(); it!=_vars.end(); ) {
		const std::string& name = it->first;
		if (!strncmp(name.c_str(), "r", 1) || !strncmp(name.c_str(), "r_", 2)) {
			++it;
			flag = true;
		}else {
			_vars.erase(it++);
		}
	}

	return flag;
}

/**     
 * \brief 任务状态
 *
 *取得任务的当前状态
 *      
 * \return 任务状态
 */
int Vars::state() const
{
	const_var_iterator it = _vars.find(Quest::FINISHED_NAME);
	if ( it != _vars.end() ) {
		return atoi(it->second.value().c_str());
	}
	
	return 0;
}

#if 0
GlobalVar* GlobalVar::_instance = NULL;

/**     
 * \brief 单件模式,保证全局变量列表唯一
 *
 * \return 全局变量列表的唯一实例
 */
GlobalVar& GlobalVar::instance()
{
	if (!_instance) {
		static GlobalVar new_instance;
		_instance = &new_instance;
	}
	
	return *_instance;
}

/**     
 * \brief 查询全局变量
 *
 *在全局变量列表中查找特定id的一个变量
 *      
 * \param id: 任务id
 * \return 全局变量,没找到返回NULL
 */
Vars* GlobalVar::vars(DWORD id) const
{
	const_vars_iterator it = _vars.find(id);
	if 	(it != _vars.end() ) {
		return it->second;
	}
	
	return NULL;
}

/**     
 * \brief 添加全局变量
 *
 *在全局变量列表中添加一个特定id的变量,如果该变量已经存在,则更新变量的值
 *      
 * \param id: 任务id
 * \return 全局变量
 */
Vars* GlobalVar::add(DWORD id)
{
	Vars* v = vars(id);
	if (!v) v = new Vars(id);

	_vars[id] = v;

	return v;	
}
#endif

UserVar* UserVar::_instance = NULL;
int UserVar::SERVER_ID = 0;

/**     
 * \brief 单件模式,保证用户变量列表唯一
 *
 * \return 用户变量列表的唯一实例
 */
UserVar& UserVar::instance()
{
	if (!_instance) {
		static UserVar new_instance;
		_instance = &new_instance;
	}
	
	return *_instance;
}

/**     
 * \brief 查询用户变量
 *
 *在用户变量列表中查找特定的一个变量
 *      
 * \param id: 任务id
 * \param key: 根据用户信息hash出的一个key值,唯一识别一个ie用户
 * \return 用户变量,没找到返回NULL
 */
Vars* UserVar::vars(DWORD id, QWORD key) const
{
	const_vars_iterator it = _vars.find(id);
	if 	(it != _vars.end() ) {
		return it->second->vars(key);
	}
	
	return NULL;
}

/**     
 * \brief 添加用户变量
 *
 *在用户变量列表中添加特定的一个变量,如果变量已存在则更新
 *      
 * \param id: 任务id
 * \param key: 根据用户信息hash出的一个key值,唯一识别一个ie用户
 * \return 添加的用户变量
 */
Vars* UserVar::add(DWORD id, QWORD key)
{
	VAR* var = NULL; 
	const_vars_iterator it = _vars.find(id);
	if 	(it != _vars.end() ) {
		var = it->second;
	}
	
	if (var) {
		return var->add(id, key);
	}
	
	var = new VAR;
	Vars* v = var->add(id, key);
	_vars[id] = var;

	return v;	
}

bool UserVar::save() const
{
	std::ofstream of(_file.c_str(), std::ios::binary);

	char buf[MAX_BUF_SIZE*30];
	bzero(buf, sizeof(buf));

	int count = _vars.size();
	memcpy(buf, &count, sizeof(count));
	int len = sizeof(count);
		
	for (const_vars_iterator it=_vars.begin(); it!=_vars.end(); ++it) {
		QWORD id = it->first;
		memcpy(buf+len, &id, sizeof(id));
		len += sizeof(id);

		len += it->second->save((unsigned char*)buf+len);
		if (len >= (MAX_BUF_SIZE*30 - 1024))
		{
			Zebra::logger->fatal("存储全局变量时，缓冲区过短越界（%u, %u）", MAX_BUF_SIZE, len);
			of.write(buf, len);
			bzero(buf, sizeof(buf));
			len = 0;
		}
	}

	if (len > 0) of.write(buf, len);
			
	of.flush();
	of.close();
		
	return true;
}

bool UserVar::load()
{
	bool ret = false;
	std::ifstream inf(_file.c_str(), std::ios::binary);

	//获得文件大小
	inf.seekg(0, std::ios::end);
	int length = inf.tellg();
	inf.seekg(0, std::ios::beg);

	if (length > 0)
	{
		char *buf = new char[length];
		if (buf)
		{
			bzero(buf, length);

			inf.read(buf, sizeof(buf));
			int size =*((int*)buf);
			int len = sizeof(int);

			while (size-- > 0) {
				QWORD id = *((QWORD*)(buf+len));
				len += sizeof(id);
				VAR* vars = new VAR;
				len += vars->load((unsigned char*)buf+len);
				_vars[id] = vars;
			}
			SAFE_DELETE_VEC(buf);
			ret = true;
		}
		else
			Zebra::logger->fatal("加载任务变量文件分配内存失败：%u", length);
	}

	inf.close();
	return ret;
}

/**     
 * \brief 存储变量
 *
 *存储任务变量
 *      
 * \param dest: 任务档案
 * \return 存储的二进制档案长度
 */
int UserVar::VAR::save(unsigned char* dest) const
{
	int len = sizeof(int);
	int count = 0;
	for (const_var_iterator it=_vars.begin(); it!=_vars.end(); ++it) {
			++count;
			memcpy(dest+len, &(it->first), sizeof(it->first));
			len += sizeof(it->first);

			int tmp = it->second->save(dest);
			len += tmp;
	//Zebra::logger->debug("存储变量(%s:%s)", it->first.c_str(), it->second.value().c_str());
	}
	//store count
	memcpy(dest, &count, sizeof(int));	

	return len;
}

/**     
 * \brief 读取变量
 *
 * 读取任务变量
 *      
 * \param dest: 任务档案
 * \return 读取的二进制档案长度
 */
int UserVar::VAR::load(unsigned char* dest)
{
	int* count = (int*)(dest);
	int length = sizeof(int);

	while ( (*count)-- > 0) {
		QWORD id=(QWORD)(*(dest+length));
		length += sizeof(QWORD);
		Vars* vars= new Vars;
		length +=vars->load((unsigned char*)dest+length);
		_vars[id] = vars;
	}

	return length;
}

/**     
 * \brief 查询任务变量
 *
 *在任务列表中查找特定id的一个变量
 *      
 * \param id: 任务id
 * \return 任务变量,没找到返回NULL
 */
Vars* QuestList::vars(DWORD id) const
{
	const_quest_iterator it = _quests.find(id);
	if (it != _quests.end()) {
		return const_cast<Vars *>(&it->second);
	}

	return NULL;	
}

/**     
 * \brief 增加任务
 *
 *在任务列表中增加一个任务
 *      
 * \param id: 任务id
 * \param vars: 任务变量
 * \param user:  携带任务的用户
 * \param notify:  添加任务时是否通知客户端
 * \return 无
 */
void QuestList::add_quest(DWORD id, const Vars& vars, SceneUser& user, bool notify)
{
	_quests[id] = vars;
	if (!notify) return;
	
	const Quest* quest = QuestTable::instance().quest(id);
	if ( quest ) {

	//	Zebra::logger->debug("title=%s\ndescription=%s\n",quest->title().c_str(),quest->description().c_str());
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::stQuestInfoUserCmd* info = (Cmd::stQuestInfoUserCmd*)buf;
		constructInPlace(info);
		info->id = id;
		int len = (quest->title().length()<63)?quest->title().length():63;
		memcpy(info->name, quest->title().c_str(), len);
		info->name[len] = '\0';
		info->start = start_time(id);
		memcpy(info->info, quest->description().c_str(), quest->description().length());
		info->info[quest->description().length()] = '\0';
		user.sendCmdToMe(info , sizeof(Cmd::stQuestInfoUserCmd)+sizeof(BYTE)*(quest->description().length()+1));
	}	
}

/**     
 * \brief 任务数目
 *
 *取得任务列表中未完成的任务总数
 *      
 * \return 任务数量
 */
int QuestList::count() const
{
//	return _quests.size();
	int number = 0;
	for (const_quest_iterator it=_quests.begin(); it!=_quests.end(); ++it) {
		int state = it->second.state();
		if (it->first > 100000 ||state == Quest::FINISHED_NOT_SAVE || state == Quest::FINISHED_SAVE) continue;
		++number;
	}
	
	return number;
}

#define MARK "%%"
/**     
 * \brief 设置任务菜单
 *
 *设置npc的任务信息
 *      
  * \param menu: 菜单内容
 * \return 成功返回true,失败返回false
 */
bool QuestList::set_menu(const std::string& menu)
{
	if ( _menu.empty()  ) {
		if ( !_subs.empty() ) {
			std::ostringstream head;
			std::ostringstream tail;
			head << "function TaskDialog()";
			std::list<std::string>::iterator it;
			for (it=_subs.begin(); it!=_subs.end(); ++it) {
				std::string::size_type pos = (*it).find(MARK);
				if (pos != std::string::npos) {
					head << (*it).substr(0, pos);
					tail << (*it).substr(pos+strlen(MARK));
				}
			}
			head << "end";
			_menu = head.str() + tail.str();	
		}else {
			_menu = menu;
		}		

		return true;

	}
	
	return false;
}

/**     
 * \brief 设置任务菜单
 *
 *设置npc的任务信息
 *      
  * \param menu: 菜单内容
 * \return 成功返回true,失败返回false
 */
void QuestList::add_menu(const std::string& menu)
{
	_subs.push_back(menu);
}


#define HAS_TASK_DIALOG "function IsHasTask()\n\treturn true\nend\n"
#define NO_TASK_DIALOG "function IsHasTask()\n\treturn false\nend\nfunction TaskDialog()\nend\n"

/**     
 * \brief 查询任务菜单
 *
 *查询npc的任务信息
 *      
 * \param menu: 查询到的结果
 * \param status: 查询状态
 * \return 菜单的长度
 */
int QuestList::get_menu(char* menu, int& status)
{
	int len;
	if ( !_menu.empty() ) {
		len =  zSocket::MAX_DATASIZE - strlen(menu) - sizeof(Cmd::stVisitNpcTradeUserCmd) - strlen(HAS_TASK_DIALOG);
		len = ((int)_menu.length()>len)?len:_menu.length();
		strncpy(menu, _menu.c_str(), len);
		strncpy(menu+len, HAS_TASK_DIALOG, strlen(HAS_TASK_DIALOG));
		len += strlen(HAS_TASK_DIALOG);
		_menu.clear();
		_subs.clear();
		status = 1;
	}else {
		//client request this
		status = 0;
		len = strlen(NO_TASK_DIALOG);
		strncpy(menu, NO_TASK_DIALOG, len);
	}
	
	return len;
}

/**     
 * \brief 存储任务
 *
 *存贮任务列表到用户档案中
 *      
 * \param dest: 任务档案
 * \return 存储的二进制档案长度
 */
int QuestList::save(unsigned char* dest) const
{
	int len = 0;
	int tmp = _quests.size();
	memcpy(dest, &tmp, sizeof(int));
	len += sizeof(int);
	
	for (const_quest_iterator it=_quests.begin(); it!=_quests.end(); ++it) {
		memcpy(dest+len, &it->first, sizeof(DWORD));
		//Zebra::logger->debug("存储任务(%d)", it->first);
		len += sizeof(DWORD);
		len += it->second.save(dest+len);
	}
		
	return len;
}

/**     
 * \brief  读取任务
 *
 * 从用户档案中读取任务列表
 *      
 * \param dest: 任务档案
 * \param dest_size: 任务档案大小
 * \return 读取的任务数目
 */
int QuestList::load(unsigned char* dest , unsigned long &dest_size)
{
	unsigned char* data=dest;
	int* size = (int *)data;
	int count = *size;
	
	data += sizeof(int);
	
	while ((*size)-- > 0) {
		int* quest_id = (int *)data;
		data += sizeof(int);
		//Zebra::logger->debug("读取任务(%d)", *quest_id);
		
		Vars vars(*quest_id);
		data += vars.load(data);
		
		_quests[*quest_id] = vars;
	}
	
	dest_size =data - dest;
	return count;		
}

/**     
 * \brief 任务状态
 *
 *查询任务列表中的一个任务的状态
 *      
 * \param id: 任务id
 * \return 任务状态,没找到返回0
 */
int QuestList::state(DWORD id) const
{
	Vars* var = vars(id);
	if (!var) return 0;
	
	return var->state();
}

/**     
 * \brief 任务开始时间
 *
 *查询任务列表中的一个任务的开始时间
 *      
 * \param id: 任务id
 * \return 任务开始时间,没找到返回当前时间
 */
int QuestList::start_time(DWORD id) const
{
	Vars* var = vars(id);
	if (!var) return time(NULL);
	
	return var->start_time();
}

/**     
 * \brief 通知任务
 *
 *发送任务信息到用户
 *      
 * \param user: 发出请求的用户
 * \return 当前总是返回0
 */
int QuestList::notify(SceneUser& user) const
{
	for (const_quest_iterator it=_quests.begin(); it!=_quests.end(); ++it) 
	{
		const Quest* quest = QuestTable::instance().quest(it->first);
		if ( quest ) 
		{
			//finished request will not info client
			if (state(it->first) == Quest::FINISHED_SAVE || state(it->first) == Quest::FINISHED_NOT_SAVE) continue;
			
			BYTE buf[zSocket::MAX_DATASIZE];
			Cmd::stQuestInfoUserCmd* info = (Cmd::stQuestInfoUserCmd*)buf;
			constructInPlace(info);
			info->id = it->first;
			int len = (quest->title().length()<63)?quest->title().length():63;
			memcpy(info->name, quest->title().c_str(), len);
			info->name[len] = '\0';
			info->start = start_time(it->first);
			memcpy(info->info, quest->description().c_str(), quest->description().length());
			info->info[quest->description().length()] = '\0';
			user.sendCmdToMe(info , sizeof(Cmd::stQuestInfoUserCmd)+sizeof(BYTE)*(quest->description().length()+1));

		//	Zebra::logger->debug("QuestList::notify**********************\n description=%s",quest->description().c_str());
		//	Zebra::logger->debug("quest info(%s)", info->info);
			it->second.notify(user);
			
		}
		else 
		{			
			//Zebra::logger->error("用户(%d, %d)带有不存在的任务(%d)", user.accid, user.charbase.id, it->first);
		}
	}
	
	return 0;
}

/**     
 * \brief 放弃任务
 *
 *放弃一个任务
 *      
 * \param user: 发出请求的用户
 * \param id: 任务id
 * \param force: 是否强制放弃
 * \param destroy: 是否删除任务
 * \return 成功返回0,失败返回-1
 */
int QuestList::abandon(SceneUser& user, DWORD id, bool force, bool destroy)
{
	if (!force &&  (state(id)  ==  Quest::FINISHED_SAVE || state(id) == Quest::FINISHED_NOT_SAVE) ) 
	{
		return -1;
	}

	if (destroy) _quests.erase(id);
	
	Cmd::stAbandonQuestUserCmd ret;
	ret.id = id;
	user.sendCmdToMe(&ret, sizeof(ret));
	
	return 0;
}

/**     
 * \brief 放弃所有任务
 *
 *      
 * \param user: 发出请求的用户
 */

void QuestList::clear(SceneUser* pUser)
{
	for (quest_iterator it=_quests.begin(); it!=_quests.end(); it++) 
	{
		this->abandon(*pUser, it->first, true, false);
	}
	
	_quests.clear();
}

/**     
 * \brief 更新任务
 *
 *更新任务信息
 *      
 * \param user: 发出请求的用户
 * \param refresh: 是否需要刷新
 * \return 当前总是返回0
 */
int QuestList::update(SceneUser& user, bool refresh)
{
	for (quest_iterator it=_quests.begin(); it!=_quests.end();) 
	{
		int state = it->second.state();
		int update =  it->second.update();
		if ( state == Quest::FINISHED_NOT_SAVE && !update) 
		{
			DWORD id = it->first;
			int start = it->second.start_time();
			abandon(user, it->first, true, false);
			Vars vars(id);
			vars.set_timer(start);
			Op::Set<int> op;
			vars.set_value(op, Quest::FINISHED_NAME, Quest::FINISHED_NOT_SAVE, 0, &user);
			vars.update(1);  //set updated
			add_quest(id, vars, user, false);
			//it = old_it;
			//continue;
		}

		if (state == Quest::FINISHED_SAVE && !update) 
		{			

			DWORD id = it->first;
			abandon(user, it->first, true, false);
			Vars vars(id);
			Op::Set<int> op;
			vars.set_value(op, Quest::FINISHED_NAME, Quest::FINISHED_SAVE, 0 ,&user);
			vars.update(1);
			add_quest(id, vars, user, false);
		}
		
		++it;
	}

	if (refresh) 
	{
		//Zebra::logger->debug("任务状态变化，重新请求九屏数据!");

		DWORD leader_id = user.team.getLeader();
		SceneUser* leader = SceneUserManager::getMe().getUserByTempID(leader_id);
		if (leader) 
		{
			Team& team = const_cast<Team&>(leader->team.getTeam());

			team.rwlock.rdlock();
			std::vector<TeamMember>::iterator it = team.member.begin();
			for(; it!=team.member.end(); ++it) 
			{
				SceneUser* member = SceneUserManager::getMe().getUserByTempID(it->tempid);
				if (member) 
				{
					member->quest_list.update(*member, false);
					member->sendNineToMe();
				}
			}
			team.rwlock.unlock();
		}
		else 
		{
			user.sendNineToMe();			
		}

	}
	return 0;
}


std::string QuestList::info(int id) const
{
	std::ostringstream os;

	os << "人物任务列表:\n";
	for (const_quest_iterator it=_quests.begin(); it!=_quests.end(); ++it) {
		std::string name = "未知";
		const Quest* quest = QuestTable::instance().quest(it->first);
		if (quest) name = quest->title();
		os << "任务(" << name << ", " << it->first << ")" << "\n";
		os << it->second.info() << "\n";
	}

	return os.str();
}

