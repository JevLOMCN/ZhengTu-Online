/**
 * \file
 * \version	$Id: QuestEvent.h  $
 * \author
 * \date	2005-03-31
 * \brief	任务系统
 * 
 */

#ifndef __QUEST_EVENT_H__
#define __QUEST_EVENT_H__

#include "Zebra.h"
#include "zXMLParser.h"

#include <string>
#include <ext/hash_map>
#include <vector>

class SceneUser;
class Vars;
class Condition;
class Action;

/**
 * \brief 触发事件
 *
 * 封装了对任务脚本触发的事件的处理
 *
 */
class Event
{
public:
	enum {
		START_QUEST = 0,
		QUESTING = 1,
	};
	
	Event();
	~Event();


	int execute();
	int execute(SceneUser& user);
	int execute(SceneUser& user, DWORD quest_id, DWORD offset);
	
	
	int state(SceneUser& user);

	/**     
	 * \brief   取得事件id
	 *      
	 * \return 事件id
	 */
	DWORD id() const
	{
		return _id;
	}
	
	bool parse(const std::string& file_name);

	const char * npc_name() const
	{
		return _npc_name;
	}
private:
	
	class FreeMemory 
	{
	public:
		template <typename T>
		void operator () (T target)
		{
			delete target;	
		}	
	};

	class Embranchment
	{
	public:
		Embranchment();

		~Embranchment();

		bool is_valid(SceneUser* user, const Vars* vars);
		int do_it(SceneUser* user, Vars* vars);

		void add(Condition* condition);
		void add(Action* action);
		bool active() const;
		void active(bool flag);
	private:
		bool _active;
		std::vector<Condition *> _conditions;
		std::vector<Action *> _actions;
	};
	
	typedef __gnu_cxx::hash_map<DWORD, Embranchment *> EMBRANCHMENT; //embranchment_id
	typedef EMBRANCHMENT::iterator embranchment_iterator;
	typedef EMBRANCHMENT::const_iterator const_embranchment_iterator;	
	
	//EMBRANCHMENT _embranchments;
	
	//we use the higest bit to indicate if it is a new quest
	template <typename T>
	class LessIgnoreHB : public std::binary_function<T, T, bool>
	{
	public:
		bool operator() (T preceding, T posterior) const
		{
			preceding &= ~(0x1 << 31);
			posterior &= ~(0x1 << 31);
			
			return preceding < posterior;
		}
	};
	
	typedef std::map<DWORD, EMBRANCHMENT *, LessIgnoreHB<DWORD> > QUESTS; //quest_id
	typedef QUESTS::iterator quest_iterator;
	
	QUESTS _quests;
	
	DWORD _id;
	char _npc_name[MAX_NAMESIZE];
};

/**
 * \brief 事件触发器
 *
 *触发事件模板,提供对不同触发事件类型的统一处理
 *
 */
template<typename T>
class EventTrigger
{
public:
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 */    
	EventTrigger(DWORD id) : _id(id)
	{ }

	/**     
	 * \brief   取得事件id
	 *      
	 * \return 事件id
	 */		
	DWORD id() const
	{
		return _id;
	}
	
	/**     
	 * \brief   取得事件目录
	 *      
	 * \return 事件目录
	 */
	static const std::string& dir()
	{
		return T::DIR;
	}
	
private:
	DWORD _id;		
};

namespace Trigger {
	/**
	 * \brief 使用触发事件
	 *
	 *在使用物品时被触发
	 *
	 */	
	class Use
	{
	public:
		static const std::string DIR;
	};
	
	/**
	 * \brief 杀死触发事件
	 *
	 *在杀死NPC时被触发
	 *
	 */	
	class Kill
	{
	public:
		static const std::string DIR;
	};

	/**
	 * \brief 杀死触发事件
	 *
	 *在杀死NPC时被触发
	 *
	 */	
	class KillByLevel
	{
	public:
		static const std::string DIR;
	};
	
	/**
	 * \brief 杀死触发事件(亲自杀的)
	 *
	 *在杀死NPC时被触发
	 *
	 */	
	class KillBySelf
	{
	public:
		static const std::string DIR;
	};

	/**
	 * \brief 拾取触发事件
	 *
	 *在拾取物品时被触发
	 *
	 */	
	class Get
	{
	public:	
		static const std::string DIR;
	};
		
	/**
	 * \brief 访问触发事件
	 *
	 *在访问NPC时被触发
	 *
	 */	
	class Visit
	{
	public:	
		static const std::string DIR;
	};

	/**
	 * \brief 死亡触发事件
	 *
	 *在用户死亡时被触发
	 *
	 */	
	class Die
	{
	public:	
		static const std::string DIR;
	};

	/**
	 * \brief 骑马触发事件
	 *
	 *在用户骑马时被触发
	 *
	 */	
	class Ride
	{
	public:	
		static const std::string DIR;
	};

	/**
	 * \brief 退出触发事件
	 *
	 *在用户退出时被触发
	 *
	 */	
	class Quit
	{
	public:	
		static const std::string DIR;
	};

	/**
	 * \brief 定时触发事件
	 *
	 *需要定时处理的任务
	 *
	 */	
	class Timer
	{
	public:	
		static const std::string DIR;
	};
	
	/**
	 * \brief 丢弃触发事件
	 *
	 *在用户丢弃物品时被触发
	 *
	 */	
	class Drop
	{
	public:	
		static const std::string DIR;
	};

	/**
	 * \brief 进入游戏触发事件
	 *
	 *在用户进入游戏时被触发
	 *
	 */	
	class Enter
	{
	public:	
		static const std::string DIR;
	};

	/**
	 * \brief 其他触发事件
	 *
	 */	
	class Other
	{
	public:	
		static const std::string DIR;
	};	
}


//typedef EventTrigger<Trigger::Use> OnUse;
typedef EventTrigger<Trigger::Kill> OnKill;
typedef EventTrigger<Trigger::KillByLevel> OnKillByLevel;
typedef EventTrigger<Trigger::KillBySelf> OnKillBySelf;
typedef EventTrigger<Trigger::Get> OnGet;
//typedef EventTrigger<Trigger::Visit> OnVisit;
typedef EventTrigger<Trigger::Die> OnDie;
typedef EventTrigger<Trigger::Ride> OnRide;
typedef EventTrigger<Trigger::Quit> OnQuit;
typedef EventTrigger<Trigger::Timer> OnTimer;
typedef EventTrigger<Trigger::Drop> OnDrop;
typedef EventTrigger<Trigger::Enter> OnEnter;
typedef EventTrigger<Trigger::Other> OnOther;

/**
 * \brief 访问触发事件
 *
 *在访问NPC时被触发
 *
 */	
template <typename T>
class EventTrigger2 : public EventTrigger<T> 
{
public:	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *  
	 * param id : 访问的npc id
	 * param quest_id : 任务id
	 * param offset : 任务分支 
	 */    
	EventTrigger2(DWORD id, DWORD quest_id = 0, DWORD offset = 0) : EventTrigger<T>(id), _quest_id(quest_id), _offset(offset)
	{ }

	/**     
	 * \brief   取得事件对应的任务id
	 *      
	 * \return 任务id
	 */	
	DWORD quest_id() const
	{
		return _quest_id;
	}
	
	/**     
	 * \brief   取得事件对应的任务分支
	 *      
	 * \return 任务分支
	 */	
	DWORD offset() const
	{
		return _offset;
	}
		
private:
	DWORD _quest_id;
	DWORD _offset;
};

typedef EventTrigger2<Trigger::Use> OnUse;
typedef EventTrigger2<Trigger::Visit> OnVisit;

/**
 * \brief 事件管理器
 *
 *管理不同类型的事件列表,并提供统一的处理方式
 *
 */	
template <typename E>
class EventManager
{
public:
	typedef EventManager<E> self_t;

	/**     
	 * \brief  单件模式,保证事件管理器的唯一实例
	 *      
	 * \return 事件管理器的唯一实例
	 */		
	static self_t& instance() 
	{
		static self_t new_instance;			
		return new_instance;
	}

	/**     
	 * \brief  执行任务
	 *
	 * 查找对应的任务事件并执行
	 *      
	 * \param user: 请求的用户
	 * \param event:任务事件
	 * \return 处理结果
	 */	
	int execute(SceneUser& user, E& event)
	{
		__gnu_cxx::hash_map<DWORD, Event>::iterator it = _events.find(event.id());
		if (it!=_events.end()) {
			return it->second.execute(user);
		}
		
		//return Action::SUCCESS;
		return 0;
	}

	/**     
	 * \brief  执行任务
	 *
	 * 查找对应的任务事件并执行
	 *      
	 * \param event:任务事件
	 * \return 处理结果
	 */		
	int execute(E& event);

	/**     
	 * \brief  任务状态
	 *
	 * 取得用户某任务的状态
	 *      
	 * \param user: 请求的用户
	 * \param event: 查询的任务
	 * \return 任务状态
	 */	
	int state(SceneUser& user, E& event);

	/**     
	 * \brief  解析任务脚本
	 *
	 * 解析任务脚本,生成对应的事件结构
	 *      
	 * \param file_name: 文件名
	 * \return true表示解析脚本成功,false表示解析脚本失败
	 */  	
	bool parse(const std::string& file_name) 
	{
		Event event;
		
		std::string name = Zebra::global["questdir"]+E::dir()+file_name;
		if ( !event.parse(name) ) {
			return false;
		}
		
		_events[event.id()] = event;
		return true;
	}
	
	void get_valid_quest(SceneUser &user, bool=false);
	void get_valid_quest_str(SceneUser &user);
private:
	__gnu_cxx::hash_map<DWORD, Event> _events;	

	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *  
	 */    
	EventManager()
	{ }

	/**     
	 * \brief 析构函数
	 *
	 */
	~EventManager()
	{ }
	
	static EventManager<E>* _instance;
};

template<typename E>
EventManager<E>* EventManager<E>::_instance = NULL;

/**     
 * \brief  执行任务
 *
 * 模板偏特化,执行特定的访问类型触发任务事件
 *      
 * \param user: 请求的用户
 * \param event:任务事件
 * \return 处理结果
 */	
template<>
inline int EventManager<OnVisit>::execute(SceneUser& user, OnVisit& event)
{
	__gnu_cxx::hash_map<DWORD, Event>::iterator it = _events.find(event.id());
	if (it!=_events.end()) {
		if ( event.quest_id() && event.offset() ) {
			return it->second.execute(user, event.quest_id(), event.offset());
		}else {
			return it->second.execute(user);	
		}
	}
	
	return 0;
}

template<>
inline int EventManager<OnUse>::execute(SceneUser& user, OnUse& event)
{
	__gnu_cxx::hash_map<DWORD, Event>::iterator it = _events.find(event.id());
	if (it!=_events.end()) {
		if ( event.quest_id() && event.offset() ) {
			return it->second.execute(user, event.quest_id(), event.offset());
		}else {
			return it->second.execute(user);	
		}
	}
	
	return 0;
}

/**     
 * \brief  任务状态
 *
 * 取得用户某任务的状态
 *      
 * \param user: 请求的用户
 * \param event: 查询的任务
 * \return 任务状态
 */	
template<>
inline int EventManager<OnVisit>::state(SceneUser& user, OnVisit& event)
{
	__gnu_cxx::hash_map<DWORD, Event>::iterator it = _events.find(event.id());
	if (it!=_events.end()) {
		return it->second.state(user);	
	}
	
	return -1;
}

/**     
 * \brief  执行任务
 *
 * 模板偏特化,执行特定的定时触发任务事件
 *      
 * \param event:任务事件
 * \return 处理结果
 */	
template<>
inline int EventManager<OnTimer>::execute(OnTimer& event)
{
	__gnu_cxx::hash_map<DWORD, Event>::iterator it = _events.find(event.id());
	if (it!=_events.end()) {
		return it->second.execute();	
	}
	
	return 0;
}

#endif
