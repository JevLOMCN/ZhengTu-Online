/**
 * \file	zState.h
 * \version  	$Id$
 * \author  	
 * \date 	
 * \brief 	定义有限状态机的状态接口
 *
 * 
 */

#ifndef _STATE_H
#define _STATE_H
#include <vector>
#include <string>
#include <map>

#include "zEventHandler.h"
#include "zEvent.h"
//#include "zStateMachine.h"
template<typename T> class StateMachine;

//template<typename T, typename EventHandler = EventHandler<T> >
template<typename T>
class State
{
	public:

		/**
		 * \brief 状态名称 
		 *
		 */
		std::string name;

		/**
		 * \brief State构造函数 
		 *
		 *
		 * \param timeout 该状态超过该值,则过期
		 * \return 
		 */
		State(int timeout=0)
		{
		}


		/**
		 * \brief 设置状态机指针 
		 *
		 * \param m 拥有该状态的状态机指针
		 */
		void set_machine(StateMachine<T>* m)
		{
			this->machine = m;
		}

		/**
		 * \brief 加入事件处理函数 
		 *
		 *
		 * \param ename 事件名称
		 * \param eh 事件处理的Wrap类
		 * \return 添加成功,返回TRUE, 否则, 返回FALSE
		 */
		bool addHandler(const std::string& ename, EventHandler<T> eh)
		{
			std::pair<EventIter, bool> retval = handlers.insert(typename EventMap::value_type(ename, eh));
			return retval.second;
		}

		/**
		 * \brief 分派事件 
		 *
		 *
		 * \param ev 事件指针
		 * \return 事件被处理返回TRUE, 没有被处理返回FALSE
		 */
		bool dispatch_event(Event* ev)
		{
			EventIter et = handlers.find(ev->get_name());	
			if (et!=handlers.end())
			{
				// 执行当前状态收到该事件时的事件处理函数
				EventHandler<T> eh = (EventHandler<T>)et->second;
				eh.action(ev);
				
				// 跳转到下一状态
				TnsIter ti = transitions.find(ev->get_name());
				machine->set_act(ti->second);
#ifdef _ZJW_DEBUG				
				std::cout << "cur_state:[" << this->name.c_str() << "] " 
					<< "receive event:[" << ev->get_name() 	<< "] "
					<< "nextstate:[" << ti->second << "]" << std::endl;
#endif				
				return true;
			}
			else
			{
				std::cout << "state:dispatch_event:no find event" << std::endl;
			}

			return false;
		}
		

		/**
		 * \brief 添加状态转换表项
		 *
		 *
		 * \param evname 事件名称
		 * \param eh  事件处理函数
		 * \param nextstatename 下一状态名称,默认为over状态
		 * \return 添加成功返回TRUE, 否则为FALSE 
		 */
		bool add_transition(const std::string& evname, EventHandler<T> eh, 
				const std::string& nextstatename="over")
		{
			std::pair< TnsIter, bool > pr;

			pr = transitions.insert(typename TnsMap::value_type(evname, nextstatename));

			if (pr.second)
			{
				pr.second = addHandler(evname, eh);
			}

			return pr.second;
		}


		/**
		 * \brief 判断该状态是否超时 
		 *
		 * \return 超时返回TRUE, 否则为FALSE 
		 */
		bool is_timeout()
		{
			
		}

	protected:
		typedef std::map<std::string, EventHandler<T> > EventMap;
		typedef typename EventMap::iterator EventIter;
		EventMap handlers;

	
		typedef std::map<std::string, std::string> TnsMap;
		typedef typename TnsMap::iterator TnsIter;
		TnsMap transitions;
		StateMachine<T>* machine;
		int timeout;
};

#endif
