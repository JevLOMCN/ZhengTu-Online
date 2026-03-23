#ifndef _SIMPLE_ALLOCATOR_H
#define _SIMPLE_ALLOCATOR_H

template <class _Ty, size_t blockNodeNum = 64>
struct simpleAllocator{
	typedef _Ty  * pointer;
	typedef _SIZT size_type;
	typedef _PDFT difference_type;

	void deallocate(pointer _Ptr, size_type size)
	{	// deallocate object at _Ptr, ignore size
		//operator delete(_Ptr);
		assert(size == 1);
		free(_Ptr);
	}

	void destroy(pointer _Ptr)
	{	// destroy object at _Ptr
		_Ptr->~_Ty();
	}

#ifdef _DEBUG

	pointer allocate(size_type _Count,const char* fileName,int line)
	{	// allocate array of _Count elements
		assert(_Count == 1);
		return (pointer)_malloc_dbg(sizeof(_Ty),_NORMAL_BLOCK,fileName,line);
	}

#else

	pointer allocate(size_type _Count)
	{	// allocate array of _Count elements
		return (pointer)malloc(sizeof(_Ty));
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
#endif