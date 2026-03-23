/**
 * \file	zStateMachine.h
 * \version  	$Id$
 * \author  	
 * \date 	
 * \brief 	FSM基类
 *
 * 
 */

#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H
#include <string>
#include <map>
#include "zType.h"
#include "zState.h"


template<typename T>
class StateMachine
{   
	public:
		StateMachine()
		{
			act_state = NULL;
		}
		
		virtual ~StateMachine()
		{
			StateIter st;

			for(st=states.begin(); st!=states.end(); st++)
			{
				SAFE_DELETE(st->second);
			}
		}

		/**
		 * \brief 分派事件
		 *
		 *
		 * \param ev 事件指针 
		 */
		virtual void dispatch_event(Event* ev)
		{
			if (act_state)
			{
				std::cout << "machine:dispatch_event" << std::endl;
				act_state->dispatch_event(ev);
			}
		}
		
		/**
		 * \brief 通过state_machine.xml初始化状态机,子类,必须实现的方法
		 *
		 * \param name 状态机名称, 用于在state_machine.xml查找对应的配置
		 */
		virtual void init(const std::string& name)
		{
			State<T>* over_state  = new State<T>;
			this->add_state("over", over_state);
		}
		
		/**
		 * \brief 重载[],通过状态名,获取一个状态
		 *
		 *
		 * \param name 状态名 
		 * \return 指定状态的指针, 如果指定状态不存在,返回NULL
		 */
		virtual State<T>* operator[](std::string name)
		{
			StateIter st;
			st = states.find(name);
			if ( st == states.end() )
				return 0; // 未找到,返回NULL 
			return st->second; // 找到,返回对应的状态指针
		}

		/**
		 * \brief 设置当前的活跃状态 
		 *
		 * \param 状态名
		 * \return 设置成功,返回TRUE,否则,返回FALSE
		 */
		bool set_act(std::string statename = "default")
		{
			State<T> *st = (*this)[statename];
			if(st == 0)
				return false;

			act_state = st;
			return true;
		}
		
		/**
		 * \brief 得到当前的活跃状态 
		 *
		 * \return 返回当前状态
		 */
		State<T>* get_act()
		{
			return act_state;
		}

		bool is_over()
		{
			if (act_state && act_state->name=="over") return true;
			return false;
		}

	protected:
		
		State<T>* act_state;//当前状态 
	
		typedef std::map<std::string, State<T>* > StateMap;
		typedef typename StateMap::iterator StateIter;
			
		/// 状态表
		StateMap states;

		/**
		 * \brief 添加一个新状态 
		 *
		 *
		 * \param name 状态名
		 * \param new_state 新的状态
		 * \return 
		 */
		void add_state(const std::string& name, State<T>* new_state)
		{
			//std::pair<StateIter, bool> retval = states.insert(StateMap::value_type(name, new_state));
			new_state->name = name;
			states.insert(typename StateMap::value_type(name, new_state));
			//return retval.second;
		}

		std::string name;
};

#endif
