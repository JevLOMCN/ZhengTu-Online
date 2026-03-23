#pragma once

//it is used for multipul thread!
#include <queue>
#include <stack>
#include <vector>
#include "lock.h"

template<typename T>
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class cSafeQueue
{
public:
	CLock m_lock;

	virtual void Push(T p) {CSafeLock sl(&m_lock);
							m_queue.push(p);};

	virtual  bool Pop(T& p) {		
							CSafeLock sl(&m_lock);
							if (m_queue.empty())
								return false;
							p = m_queue.front();
							m_queue.pop();
							return true;};
	void Clear(){while(!m_queue.empty())m_queue.pop();}

	std::queue<T> m_queue;

	typedef bool (__cdecl *fnIf )(T & var);

	void remove_if(  fnIf pFn ) 
	{
		CSafeLock sl( &m_lock );
		for( std::deque<T>::iterator it = m_queue.c.begin(); it != m_queue.c.end(); )
		{
			if( pFn(*it) )
			{
				it = m_queue.c.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

};

template<typename T>
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class cSafeStack
{
public:
	CLock m_lock;

	virtual void Push(T p) {CSafeLock sl(&m_lock);
							m_stack.push_back(p);};

	virtual bool Pop(T& p) {		
							CSafeLock sl(&m_lock);
							if (m_stack.empty())
								return false;
							p = m_stack.back();
							m_stack.pop_back();
							return true;};

	void Clear(){ CSafeLock sl(&m_lock); m_stack.clear();}
	std::deque<T> m_stack;
	bool Empty(){ CSafeLock sl(&m_lock); return m_stack.empty();}

	typedef bool (__cdecl *fnIf )(T & var);

	void remove_if( fnIf pFn ) 
	{
		CSafeLock sl( &m_lock );
		for( std::deque<T>::iterator it = m_stack.begin(); it != m_stack.end(); )
		{
			if( pFn(*it) )
			{
				it = m_stack.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

};


template<typename T>
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class safe_queue
{
	std::queue<T> m_queue;
public:
	CMyLock m_lock;

	virtual void Push(T p) {
		CSafeLock lk(&m_lock);
		m_queue.push(p);};

	virtual  bool Pop(T& p) {		
		CSafeLock lk(&m_lock);
		if (m_queue.empty())
			return false;
		p = m_queue.front();
		m_queue.pop();
		return true;};
		void Clear(){while(!m_queue.empty())m_queue.pop();}

};
