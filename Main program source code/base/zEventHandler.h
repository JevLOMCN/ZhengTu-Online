/**
 * \file	zEventHandler.h
 * \version  	$Id$
 * \author  	
 * \date 	
 * \brief 	定义FSM事件处理的接口
 *
 * 
 */

#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

class Event;

template<typename T>
class EventHandler
{
	public: 
		typedef int  (T::*Action)(Event*);
		Action handler;
		T* instance;
		
		explicit EventHandler(T* value)
		{       
			instance = value;
		}       

		virtual ~EventHandler(){};

		virtual void action(Event* ev) 
		{       
			(instance->*handler)(ev);
		}       
};

#endif
