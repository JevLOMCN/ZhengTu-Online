#ifndef NOFREE_ALLOC_H
#define NOFREE_ALLOC_H

namespace std{
extern size_t gNoFreeAllocSize;
template<class _Ty, size_t blockNodeNum = 64>
class nofree_allocator{
public:
	typedef _Ty _FARQ *pointer;
	typedef _Ty _FARQ& reference;
	typedef const _Ty _FARQ *const_pointer;
	typedef const _Ty _FARQ& const_reference;

	typedef _SIZT size_type;
	typedef _PDFT difference_type;

private:
	struct stPool{
		_Ty		nodes[blockNodeNum];
		stPool* next;
	};
	
	stPool*		mPools;
	pointer		mCurPool;
	size_t		mCurAllocIndex;
public:

	nofree_allocator() : mPools(NULL) , mCurPool(NULL),mCurAllocIndex(-1)
	{	
		// construct default allocator (do nothing)
	}

	~nofree_allocator()
	{
		freeMemory();
	}

protected:
	void freeMemory()
	{
		while(mPools)
		{
			stPool* p = mPools;
			mPools = p->next;
			free(p);
			gNoFreeAllocSize -= sizeof(stPool);
		}
		mPools = NULL;
		mCurPool = NULL;
		mCurAllocIndex = -1;
	}

	nofree_allocator(const nofree_allocator<_Ty>&)
	{	// construct by copying (do nothing)
		assert(0);
	}

	void deallocate(pointer _Ptr, size_type size)
	{	// deallocate object at _Ptr, ignore size
		//operator delete(_Ptr);
		assert(0);
	}

	void destroy(pointer _Ptr)
	{	// destroy object at _Ptr
		_Ptr->~_Ty();
	}

	void addPoolBlock()
	{
		stPool* pool = (stPool*)malloc(sizeof(stPool));
		gNoFreeAllocSize += sizeof(stPool);
		//_IncLookasideAlloc(sizeof(stPool));
		pool->next = mPools;
		mPools = pool;
		mCurPool = pool->nodes;
		mCurAllocIndex = 0;
	}

	pointer allocate(size_type _Count)
	{	// allocate array of _Count elements
		if(mCurAllocIndex >= blockNodeNum) 
			addPoolBlock();

		return &mCurPool[mCurAllocIndex++];
	}
public:
	pointer alloc()
	{
		pointer _Ptr = allocate(1);
		constructInPlace(_Ptr);
		return _Ptr;
	}
	
	void freePointer(pointer _Ptr)
	{
		destroy(_Ptr);
	}
};

template<class _Ty, size_t blockNodeNum = 64>
class safe_nofree_allocator : public nofree_allocator< _Ty,blockNodeNum >
{
	CLock m_lock;
public:
	pointer alloc()
	{
		m_lock.Lock();
		pointer _Ptr = allocate(1);
		m_lock.Unlock();

		constructInPlace(_Ptr);
		return _Ptr;
	}

	void freePointer(pointer _Ptr)
	{
		destroy(_Ptr);
	}
}
;
};

#endif