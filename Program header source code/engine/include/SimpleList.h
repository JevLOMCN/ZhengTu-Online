/**
 * \file	SimpleList.h
 * \version $Id$
 * \author	$$$03公司
 *			$$$03公司@###.com
 * \date	2005年8月10日 13时19分
 * \brief	
 * 
 *			?
 */
#pragma once
#pragma warning(push)
#pragma warning( disable : 4355)
class ENGINE_API SimpleNode{
public:
	SimpleNode * prev;
	SimpleNode * next;
	SimpleNode() : prev(this) , next(this){}

	void init()
	{
		prev = next = this;
	}
	
	bool isLink()
	{
		return prev != this && next != this;
	}

	void unlink()
	{
		next->prev = prev;
		prev->next = next;
		next = prev = this;
	}

	// 链接在ptr后面
	void linkAfter(SimpleNode* ptr)
	{
		next = ptr->next;
		next->prev = this;
		prev = ptr;
		prev->next = this;
	}
	
	// 链接在ptr前面
	void linkBefore(SimpleNode* ptr)
	{
		prev = ptr->prev;
		prev->next = this;
		next = ptr;
		next->prev = this;
	}
};

class ENGINE_API SimpleList
{
	SimpleNode mStart,mEnd;
public:

	class reverse_iterator;
	class iterator{
		friend reverse_iterator;
		friend SimpleList;
		SimpleNode* itr;
	public:
		iterator operator = (iterator i_itr)
		{
			itr = i_itr.itr;
			return *this;
		}
		
		bool operator == (iterator it) const
		{
			return itr == it.itr;
		}

		bool operator != (iterator it) const
		{
			return itr != it.itr;
		}

		SimpleNode & operator * () const
		{
			return *itr;
		}
	
		SimpleNode * operator->() const
		{
			return itr;
		}

		iterator operator++()
		{	// preincrement
			itr = itr->next;
			return (*this);
		}

		iterator operator++(int)
		{
			iterator _Tmp = *this;
			++*this;
			return (_Tmp);
		}

		iterator& operator--()
		{	// predecrement
			itr = itr->prev;
			return (*this);
		}

		iterator operator--(int)
		{	// postdecrement
			iterator _Tmp = *this;
			--*this;
			return (_Tmp);
		}
	};

	class reverse_iterator{
		SimpleNode* itr;
		friend SimpleList;
	public:
		reverse_iterator operator = (reverse_iterator i_itr)
		{
			itr = i_itr.itr;
			return *this;
		}

		bool operator == (reverse_iterator it) const
		{
			return itr == it.itr;
		}

		bool operator != (reverse_iterator it) const
		{
			return itr != it.itr;
		}

		SimpleNode & operator * () const
		{
			return *itr;
		}

		SimpleNode * operator->() const
		{
			return itr;
		}

		reverse_iterator operator++()
		{	// preincrement
			itr = itr->prev;
			return (*this);
		}

		reverse_iterator operator++(int)
		{
			reverse_iterator _Tmp = *this;
			++*this;
			return (_Tmp);
		}

		reverse_iterator& operator--()
		{	// predecrement
			itr = itr->next;
			return (*this);
		}

		reverse_iterator operator--(int)
		{	// postdecrement
			reverse_iterator _Tmp = *this;
			--*this;
			return (_Tmp);
		}
		iterator base() const
		{
			iterator it;
			it.itr = itr;
			return it;
		}
	};

	SimpleList()
	{
		mEnd.next = mEnd.prev = &mStart;
		mStart.next = mStart.prev = &mEnd;
	}

	iterator begin()
	{ 
		iterator it;
		it.itr = mStart.next;
		return it; 
	}

	iterator end()
	{
		iterator it;
		it.itr = &mEnd;
		return it; 
	}
	
	reverse_iterator rbegin()
	{ 
		reverse_iterator it;
		it.itr = mEnd.prev;
		return it;
	}

	reverse_iterator rend()
	{ 
		reverse_iterator it;
		it.itr = &mStart;
		return it;
	}

	iterator erase(iterator it)
	{
		iterator tmp = it;
		++tmp;
		it->unlink();
		return tmp;
	}

	reverse_iterator erase(reverse_iterator it)
	{
		reverse_iterator tmp = it;
		++tmp;
		it->unlink();
		return tmp;
	}

	SimpleNode * front() { Assert(!empty()); return mStart.next;}
	SimpleNode * back(){ Assert(!empty()); return mEnd.prev;}

	void push_back(SimpleNode* node)
	{
		node->linkBefore(&mEnd);
	}

	void push_front(SimpleNode* node)
	{
		node->linkAfter(&mStart);
	}

	void pop_back(){ 
		Assert(!empty());
		mEnd.prev->unlink();
	}
	void pop_front(){
		Assert(!empty());
		mStart.next->unlink();
	}

	void clear(){
		while(!empty())
			pop_front();
	}

	bool empty(){ return mStart.next == &mEnd;}

	template< class _Ty >
	iterator erase(iterator it, type2type_t<_Ty>)
	{
		Assert(it != end());
		_Ty* p = static<_Ty*>(&(*it));
		iterator tmp = it;
		++tmp;
		it->unlink();
		delete p;
		return tmp;
	}

	template< class _Ty >
	reverse_iterator erase(reverse_iterator it, type2type_t<_Ty>)
	{
		Assert(it != rend());
		_Ty* p = static<_Ty*>(&(*it));
		reverse_iterator tmp = it;
		++tmp;
		it->unlink();
		delete p;
		return tmp;
	}

	template< class _Ty >
	void pop_back( type2type_t<_Ty> ){ 
		Assert(!empty());
		_Ty* p = static_cast< _Ty* >(mEnd.prev);
		mEnd.prev->unlink();
		delete p;
	}

	template< class _Ty >
	void pop_front( type2type_t<_Ty> ){
		Assert(!empty());
		_Ty* p = static_cast< _Ty* >(mStart.next);
		mStart.next->unlink();
		delete p;
	}

	template< class _Ty >
	void clear( type2type_t<_Ty> cType){
		while(!empty())
			pop_front(cType);
	}
};

#pragma warning(pop)