/**
 * \file
 * \version	$Id: Visitor.h  $
 * \author	
 * \date	2005-05-24
 * \brief	∑¿÷πŒﬁ–ß∑√Œ 
 * 
 */

#ifndef __VISITOR_H__
#define __VISITOR_H__

#include "zThread.h"

#include <time.h>
#include <list>

template<typename Visitor>
class VisitorThread : public zThread
{
public:
	typedef VisitorThread<Visitor> self_t;
	
	static self_t& instance() 
	{
		static self_t instance;
		return instance;
	}
	
	void run()
	{
		while(!isFinal())
		{
			zThread::sleep(1);
		
			time_t current = time(NULL);
			
			zMutex_scope_lock lock(_mutex);
			
			for (typename std::list<Visitor*>::iterator it=_elements.begin(); it!=_elements.end(); ) {
				if ( (*it)->deleted(current) ) {
					Visitor* visitor = *it;
					SAFE_DELETE(visitor);
					it = _elements.erase(it);
				}else {
					++it;
				}
			}
		}
	}
	
	void add(Visitor* v)
	{
		zMutex_scope_lock lock(_mutex);
		_elements.push_back(v);
	}
	
	~VisitorThread() { }	
private:

	zMutex _mutex;
	std::list<Visitor*> _elements;
	
	VisitorThread() { }
};

template <typename Base, template <typename> class VThread = VisitorThread, int delay = 20>
class Visitor : public Base
{
public:
	typedef VThread<Visitor> Thread;

	template<typename Para>
	Visitor(Para p) : Base(p), _deleted(false)
	{ }
		
	bool deleted(time_t current) const
	{
		return _deleted && ((current - _destroy_time) > delay);
	}
	
	void destroy() 
	{
		if (!_deleted) {
			Base::destroy();
			_deleted = true;
			_destroy_time = time(NULL);
			Thread::instance().add(this);
		}
	}
		
private:
	friend class VThread<Visitor>;
	
	~Visitor()
	{
//		Zebra::logger->debug("%s(%x) really deleted in visitor", this->name, this);
	}

	bool _deleted;
	
	time_t _destroy_time;	

};

#endif
