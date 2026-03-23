/**
 * \file      stack_alloc.h
 * \version   $Id$
 * \author    $$$03π´Àæ
 *            $$$03π´Àæ@###.com
 * \date      2006-7-19  21:11:00
 * \brief     STL’ª∑÷≈‰∆˜
 * 
 *	      STL’ª∑÷≈‰∆˜
 */

#ifndef _X_STACK_ALLOC_H
#define _X_STACK_ALLOC_H

#include "frameAllocator.h"
#include <xmemory>
#include <string>

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param _Count : √Ë ˆ
 * \param  : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
_STD_BEGIN
template<class _Ty> inline
_Ty _FARQ *_StackAllocate(_SIZT _Count, _Ty _FARQ *)
{	// allocate storage for _Count elements of type _Ty
	return ((_Ty _FARQ *)FrameAllocator::alloc((DWORD)_Count * sizeof (_Ty)));
}
//allocator
template<class _Ty>
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class stack_allocator
	: public _Allocator_base<_Ty>
{	// generic allocator for objects of class _Ty
public:
	typedef _Allocator_base<_Ty> _Mybase;
	typedef typename _Mybase::value_type value_type;


	typedef value_type _FARQ *pointer;
	typedef value_type _FARQ& reference;
	typedef const value_type _FARQ *const_pointer;
	typedef const value_type _FARQ& const_reference;

	typedef _SIZT size_type;
	typedef _PDFT difference_type;

	template<class _Other>
	struct rebind
	{	// convert an allocator<_Ty> to an allocator <_Other>
		typedef stack_allocator<_Other> other;
	};

	pointer address(reference _Val) const
	{	// return address of mutable _Val
		return (&_Val);
	}

	const_pointer address(const_reference _Val) const
	{	// return address of nonmutable _Val
		return (&_Val);
	}

	stack_allocator()
	{	// construct default allocator (do nothing)
	}

	stack_allocator(const stack_allocator<_Ty>&)
	{	// construct by copying (do nothing)
	}

	template<class _Other>
		stack_allocator(const stack_allocator<_Other>&)
	{	// construct from a related allocator (do nothing)
	}

	template<class _Other>
		stack_allocator<_Ty>& operator=(const stack_allocator<_Other>&)
	{	// assign from a related allocator (do nothing)
		return (*this);
	}

	void deallocate(pointer _Ptr, size_type)
	{	// deallocate object at _Ptr, ignore size
		//operator delete(_Ptr);
	}

	pointer allocate(size_type _Count)
	{	// allocate array of _Count elements
		return (_StackAllocate(_Count, (pointer)0));
	}

	pointer allocate(size_type _Count, const void _FARQ *)
	{	// allocate array of _Count elements, ignore hint
		return (allocate(_Count));
	}

	void construct(pointer _Ptr, const _Ty& _Val)
	{	// construct object at _Ptr with value _Val
		_Construct(_Ptr, _Val);
	}

	void destroy(pointer _Ptr)
	{	// destroy object at _Ptr
		_Destroy(_Ptr);
	}

	_SIZT max_size() const
	{	// estimate maximum array size
		_SIZT _Count = (_SIZT)(-1) / sizeof (_Ty);
		return (0 < _Count ? _Count : 1);
	}
};

typedef basic_string<char, char_traits<char>, stack_allocator<char> >	stack_string;
typedef basic_string<wchar_t, char_traits<wchar_t>,	stack_allocator<wchar_t> > stack_wstring;

_STD_END
#endif