/**
 * \file      StreamQueue.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     队列流，经过优化的，在一块连续的内存上
 * 
 *	      队列流，经过优化的，在一块连续的内存上
 */

#pragma once

#include <deque>
#include <queue>

struct stMsg
{
	char* buffer;
	int size;
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CStreamQueue
{
public:
#if defined _DEBUG || defined RELEASE_TEST
	static size_t sAllocSize;
#endif
private:
	char* m_buffer;
	size_t m_header;
	size_t m_tail;
	size_t m_size;
	size_t m_initSize;
	std::queue<stMsg> m_queue;
	void Resize(size_t size);

	void AddSize(size_t size)
	{
		if(size < m_initSize) size = m_initSize;
		Resize(m_size + size);
	}

	bool IsFull(size_t size)
	{
		if(m_tail == m_header && m_header != 0) return true;

		if(m_tail == m_size && m_header == 0) return true;

		if(m_tail >= m_header)
		{
			if(m_size - m_tail < size)
			{
				if(m_header < size)
				{
					return true;
				}
			}
		}
		else if(m_header - m_tail < size)
		{
			return true;
		}
		return false;
	}

public:

	enum enumError{
		eErrorOk = 0,
		eErrorNoData,
		eErrorMsgSize,
	};
	typedef std::deque<stMsg>::iterator iterator;

	iterator begin(){ return m_queue.c.begin();}

	iterator end(){ return m_queue.c.end();}

	CStreamQueue();

	CStreamQueue(size_t bufferSize);

	~CStreamQueue();

	size_t getMemorySize()
	{
		return m_size;
	}

	void Init(size_t bufferSize);
	void Clear()
	{
		m_header = 0;
		m_tail = 0;
		while(!m_queue.empty())m_queue.pop();
	}

	bool Empty(){ return m_queue.empty();}

	void* Push(size_t size)
	{

		stMsg msg;
		if(IsFull(size)) AddSize(size);

		if(m_tail >= m_header)
		{
			if(m_size - m_tail < size)
			{
				m_tail = 0;
			}
		}

		msg.buffer = m_buffer + m_tail;
		msg.size = size;
		m_tail += size;

		m_queue.push(msg);
		return msg.buffer;
	}

	void Push(void* pData,size_t size)
	{
		void * p = Push(size);
		if(p)
		{
			memcpy(p,pData,size);
		}
	}

	size_t GetSize(){ return m_queue.size(); }

	void* GetFront(){ return m_queue.front().buffer;}

	int Pop(stMsg& msg,size_t maxBufferSize = 0xffffffff)
	{
		if(m_queue.empty()) return eErrorNoData;
		msg = m_queue.front();
		if((size_t)msg.size > maxBufferSize) return eErrorMsgSize;

		m_queue.pop();
		m_header += msg.size;

		if(m_header == m_tail)
		{
			m_header = 0;
			m_tail = 0;
		}

		if(m_queue.size() == 0 || m_header + m_buffer != m_queue.front().buffer)
		{
			m_header = 0;
		}

		if(m_header == m_tail)
		{
			assert(m_queue.size() == 0);
		}
		else
		{
			assert(m_queue.size() != 0);
		}

		if(m_queue.size() == 0)
		{
			assert(m_header == m_tail);
		}
		else
		{
			assert(m_header != m_tail);
		}

		return eErrorOk;
	}

	bool Pop()
	{
		stMsg msg;
		return Pop(msg) != 0;
	}
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CSafeStreamQueue : public CStreamQueue
{
	CLock m_lock;
public:
	CSafeStreamQueue(size_t bufferSize) : CStreamQueue(bufferSize){}
	CSafeStreamQueue(){}
	void Lock()
	{
		m_lock.Lock();
	}

	void Unlock()
	{
		m_lock.Unlock();
	}
};
