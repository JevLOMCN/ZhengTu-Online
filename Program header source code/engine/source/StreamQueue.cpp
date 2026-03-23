#include "../include/engine.h"
#include "../include/StreamQueue.h"

#if defined _DEBUG || defined RELEASE_TEST
size_t CStreamQueue::sAllocSize = 0;
static CLock& getAllocLock()
{
	static CLock lock;
	return lock;
}
#define sAllocLock getAllocLock()
#endif

CStreamQueue::CStreamQueue()
: m_header(0)
, m_tail(0)
, m_initSize(0)
, m_size(0)
, m_buffer(NULL)
{}

CStreamQueue::CStreamQueue(size_t bufferSize)
: m_header(0)
, m_tail(0)
, m_initSize(bufferSize)
, m_size(bufferSize)
{
#if defined _DEBUG || defined RELEASE_TEST
	sAllocLock.Lock();
	sAllocSize += bufferSize;
	sAllocLock.Unlock();
#endif
	m_buffer = new char [ bufferSize];
}

CStreamQueue::~CStreamQueue()
{
	delete [] m_buffer;
#if defined _DEBUG || defined RELEASE_TEST
	sAllocLock.Lock();
	sAllocSize -= m_size;
	sAllocLock.Unlock();
#endif
}

void CStreamQueue::Init(size_t bufferSize)
{
#if defined _DEBUG || defined RELEASE_TEST
	sAllocLock.Lock();
	sAllocSize -= m_size;
	sAllocSize += bufferSize;
	sAllocLock.Unlock();
#endif
	m_header	=	0;
	m_tail		=	0;
	m_initSize	=	bufferSize;
	m_size		=	bufferSize;
	delete [] m_buffer;
	m_buffer = new char [bufferSize];
}

void CStreamQueue::Resize(size_t size)
{
	char * buf = new char[size];
	char* p = buf;
	for(size_t i=0;i<m_queue.c.size();++i)
	{
		memcpy(p,m_queue.c[i].buffer,m_queue.c[i].size);
		m_queue.c[i].buffer = p;
		p += m_queue.c[i].size;
	}
	m_header = 0;
	m_tail = p-buf;
	delete [] m_buffer;
	m_buffer = buf;
#if defined _DEBUG || defined RELEASE_TEST
	sAllocLock.Lock();
	sAllocSize -= m_size;
	sAllocSize += size;
	sAllocLock.Unlock();
#endif
	m_size = size;
}