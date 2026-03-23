/**
 * \file      lookaside_alloc.h
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ?
 * 
 *	      ?
 */

#ifndef _LOOKASIDE_ALLOC_H
#define _LOOKASIDE_ALLOC_H

#include "Lock.h"
#include <assert.h>
//#include "../../xsharelib/public.h"

namespace std{

template<class _Ty>
struct lookaside_node{
	typedef lookaside_node< _Ty > _Myt;
	_Ty data;
	_Myt* next;
};

extern size_t gLookasideAllocSize;
//allocator
/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
*/
template<class _Ty, size_t blockNodeNum = 64>
class lookaside_allocator
{	// generic allocator for objects of class _Ty

	typedef lookaside_node< _Ty > _MyNode;

	_MyNode * m_freePool;

	struct stPool{
		_MyNode nodes[blockNodeNum];
		stPool* next;
	};
	stPool*		mPools;

public:

	typedef _Ty _FARQ *pointer;
	typedef _Ty _FARQ& reference;
	typedef const _Ty _FARQ *const_pointer;
	typedef const _Ty _FARQ& const_reference;

	typedef _SIZT size_type;
	typedef _PDFT difference_type;

	lookaside_allocator() : m_freePool(NULL) , mPools(NULL)
	{	// construct default allocator (do nothing)
	}

	~lookaside_allocator()
	{
		freeMemory();
	}

	void freeMemory()
	{
		while(mPools)
		{
			stPool* p = mPools;
			mPools = p->next;
			free(p);
			gLookasideAllocSize -= sizeof(stPool);
		}
		m_freePool = NULL;
	}

	lookaside_allocator(const lookaside_allocator<_Ty>&) : m_list(NULL)
	{	// construct by copying (do nothing)
		assert(0);
	}

	void deallocate(pointer _Ptr, size_type size)
	{	// deallocate object at _Ptr, ignore size
		//operator delete(_Ptr);
		assert(size == 1);
		_MyNode* pNode = (_MyNode*)_Ptr;
		pNode->next = m_freePool;
		m_freePool = pNode;
	}

	void destroy(pointer _Ptr)
	{	// destroy object at _Ptr
		_Ptr->~_Ty();
	}

#ifdef _DEBUG
	void addPoolBlock(const char* fileName,int line)
	{
		stPool* pool = (stPool*)_malloc_dbg(sizeof(stPool),_NORMAL_BLOCK,fileName,line);
		gLookasideAllocSize += sizeof(stPool);
		//_IncLookasideAlloc(sizeof(stPool));
		pool->next = mPools;
		mPools = pool;
		_MyNode* p = &pool->nodes[0];
		for(size_t i=0 ; i < blockNodeNum - 1;++i)
		{
			p[i].next = &p[i+1];
		}
		p[blockNodeNum-1].next = NULL;
		m_freePool = p;
	}

	pointer allocate(size_type _Count,const char* fileName,int line)
	{	// allocate array of _Count elements
		assert(_Count == 1);
		if(!m_freePool) 
			addPoolBlock(fileName,line);

		_MyNode* pNode = m_freePool;
		m_freePool = m_freePool->next;
		pNode->next = NULL;
		return &pNode->data;
	}

#else

	void addPoolBlock()
	{
		stPool* pool = (stPool*)malloc(sizeof(stPool));
		gLookasideAllocSize += sizeof(stPool);
		//_IncLookasideAlloc(sizeof(stPool));
		pool->next = mPools;
		mPools = pool;
		_MyNode* p = &pool->nodes[0];
		for(size_t i=0 ; i < blockNodeNum - 1;++i)
		{
			p[i].next = &p[i+1];
		}
		p[blockNodeNum-1].next = NULL;
		m_freePool = p;
	}

	pointer allocate(size_type _Count)
	{	// allocate array of _Count elements
		if(!m_freePool) 
			addPoolBlock();

		_MyNode* pNode = m_freePool;
		m_freePool = m_freePool->next;
		pNode->next = NULL;
		return &pNode->data;
	}
#endif

#ifdef _DEBUG

	pointer alloc(const char* fileName,int line)
	{
		pointer _Ptr = allocate(1,fileName,line);
		constructInPlace(_Ptr);
		return _Ptr;
	}

	template < class _TParam>
		pointer alloc(_TParam param,const char* fileName,int line)
	{
		pointer _Ptr = allocate(1,fileName,line);
		constructInPlace(_Ptr,param);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2>
		pointer alloc(_TParam1 param1,_TParam2 param2,const char* fileName,int line)
	{
		pointer _Ptr = allocate(1,fileName,line);
		constructInPlace(_Ptr,param1,param2);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2,class _TParam3>
		pointer alloc(_TParam1 param1,_TParam2 param2,_TParam3 param3,const char* fileName,int line)
	{
		pointer _Ptr = allocate(1,fileName,line);
		constructInPlace(_Ptr,param1,param2,param3);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2,class _TParam3,class _TParam4>
		pointer alloc(_TParam1 param1,_TParam2 param2,_TParam3 param3,_TParam4 param4,const char* fileName,int line)
	{
		pointer _Ptr = allocate(1,fileName,line);
		constructInPlace(_Ptr,param1,param2,param3,param4);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2,class _TParam3,class _TParam4,class _TParam5 >
		pointer alloc(_TParam1 param1,_TParam2 param2,_TParam3 param3,_TParam4 param4,_TParam5 param5,const char* fileName,int line)
	{
		pointer _Ptr = allocate(1,fileName,line);
		constructInPlace(_Ptr,param1,param2,param3,param4,param5);
		return _Ptr;
	}

#else

	pointer alloc()
	{
		pointer _Ptr = allocate(1);
		constructInPlace(_Ptr);
		return _Ptr;
	}

	template < class _TParam>
		pointer alloc(_TParam param)
	{
		pointer _Ptr = allocate(1);
		constructInPlace(_Ptr,param);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2>
		pointer alloc(_TParam1 param1,_TParam2 param2)
	{
		pointer _Ptr = allocate(1);
		constructInPlace(_Ptr,param1,param2);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2,class _TParam3>
		pointer alloc(_TParam1 param1,_TParam2 param2,_TParam3 param3)
	{
		pointer _Ptr = allocate(1);
		constructInPlace(_Ptr,param1,param2,param3);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2,class _TParam3,class _TParam4>
		pointer alloc(_TParam1 param1,_TParam2 param2,_TParam3 param3,_TParam4 param4)
	{
		pointer _Ptr = allocate(1);
		constructInPlace(_Ptr,param1,param2,param3,param4);
		return _Ptr;
	}

	template < class _TParam1, class _TParam2,class _TParam3,class _TParam4,class _TParam5 >
		pointer alloc(_TParam1 param1,_TParam2 param2,_TParam3 param3,_TParam4 param4,_TParam5 param5)
	{
		pointer _Ptr = allocate(1);
		constructInPlace(_Ptr,param1,param2,param3,param4,param5);
		return _Ptr;
	}
#endif

	void freePointer(pointer _Ptr)
	{
		destroy(_Ptr);
		deallocate(_Ptr,1);
	}
};

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
*/
template<class _Ty, size_t blockNodeNum = 64>
class safe_lookaside_allocator : public lookaside_allocator< _Ty,blockNodeNum >
{
	CLock m_lock;
public:

#ifdef _DEBUG
	pointer alloc(const char* fileName,int line)
	{
		m_lock.Lock();
		pointer _Ptr = allocate(1,fileName,line);
		m_lock.Unlock();

		constructInPlace(_Ptr);
		return _Ptr;
	}

	template < class _TParam>
		pointer alloc(_TParam param,const char* fileName,int line)
	{
		m_lock.Lock();
		pointer _Ptr = allocate(1,fileName,line);
		m_lock.Unlock();

		constructInPlace(_Ptr,param);
		return _Ptr;
	}
#else
	pointer alloc()
	{
		m_lock.Lock();
		pointer _Ptr = allocate(1);
		m_lock.Unlock();

		constructInPlace(_Ptr);
		return _Ptr;
	}

	template < class _TParam>
		pointer alloc(_TParam param)
	{
		m_lock.Lock();
		pointer _Ptr = allocate(1);
		m_lock.Unlock();

		constructInPlace(_Ptr,param);
		return _Ptr;
	}
#endif

	void freePointer(pointer _Ptr)
	{
		destroy(_Ptr);
		m_lock.Lock();
		deallocate(_Ptr,1);
		m_lock.Unlock();
	}
};
};

#ifdef _DEBUG
#define LOOKASIDE_ALLOC(lookasideAllocator) (lookasideAllocator).alloc(__FILE__,__LINE__)
#define LOOKASIDE_PALLOC(lookasideAllocator,param) (lookasideAllocator).alloc(param,__FILE__,__LINE__)
#define LOOKASIDE_PALLOC2(lookasideAllocator,param1,param2) (lookasideAllocator).alloc(param1,param2,__FILE__,__LINE__)
#define LOOKASIDE_PALLOC3(lookasideAllocator,param1,param2,param3) (lookasideAllocator).alloc(param1,param2,param3,__FILE__,__LINE__)
#define LOOKASIDE_PALLOC4(lookasideAllocator,param1,param2,param3,param4) (lookasideAllocator).alloc(param1,param2,param3,param4,__FILE__,__LINE__)
#define LOOKASIDE_PALLOC5(lookasideAllocator,param1,param2,param3,param4,param5) (lookasideAllocator).alloc(param1,param2,param3,param4,param5,__FILE__,__LINE__)
#else
#define LOOKASIDE_ALLOC(lookasideAllocator) (lookasideAllocator).alloc()
#define LOOKASIDE_PALLOC(lookasideAllocator,param) (lookasideAllocator).alloc(param)
#define LOOKASIDE_PALLOC2(lookasideAllocator,param1,param2) (lookasideAllocator).alloc(param1,param2)
#define LOOKASIDE_PALLOC3(lookasideAllocator,param1,param2,param3) (lookasideAllocator).alloc(param1,param2,param3)
#define LOOKASIDE_PALLOC4(lookasideAllocator,param1,param2,param3,param4) (lookasideAllocator).alloc(param1,param2,param3,param4)
#define LOOKASIDE_PALLOC5(lookasideAllocator,param1,param2,param3,param4,param5) (lookasideAllocator).alloc(param1,param2,param3,param4,param5)
#endif

#endif