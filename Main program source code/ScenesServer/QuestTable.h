/**
 * \file
 * \version	$Id: QuestTable.h $
 * \author	
 * \date		
 * \brief		任务系统
 * 
 */

#ifndef __QUEST_TABLE_H__
#define __QUEST_TABLE_H__

#include "zType.h"

#include <string>
#include <map>

template <typename T>
class EventManager;

class SceneUser;

/**
 * \brief 事件驱动表格
 *
 *管理不同类型的事件列表,并提供统一的处理方式
 *
 */	
class EventTable
{
public:
	
	static EventTable& instance();

	/**     
	 * \brief  执行任务事件
	 *
	 * 调用相应的事件管理器执行该任务事件
	 *      
	 * \param user: 请求的用户
	 * \param event:任务事件
	 * \return 处理结果
	 */		
	template <typename E>
	int execute (SceneUser& user, E& event)
	{
		return EventManager<E>::instance().execute(user, event);
	}

	/**     
	 * \brief  执行任务事件
	 *
	 * 调用相应的事件管理器执行该任务事件
	 *      
	 * \param event:任务事件
	 * \return 处理结果
	 */		
	template <typename E>
	int execute (E& event)
	{
		return EventManager<E>::instance().execute(event);
	}

	bool init(const std::string& type, const std::string& file);
	
private:	

	EventTable();
	~EventTable();
	
	static EventTable* _instance;
};

#include "Quest.h"
#include <ext/hash_map>

/**
 * \brief 任务表格
 *
 *任务集合,存储各个任务的详细信息
 *
 */	
class QuestTable
{
public:
	static QuestTable& instance();
	
	bool init();
	
	const Quest* quest(DWORD id) const;
	
private:
	static const std::string _quest_file;
	
	QuestTable();

	~QuestTable();

	static QuestTable* _instance;	
	
	typedef __gnu_cxx::hash_map<DWORD, Quest> QUESTS;
	typedef QUESTS::iterator quest_iterator;
	typedef QUESTS::const_iterator const_quest_iterator;
	
	QUESTS _quests;
};

#endif



