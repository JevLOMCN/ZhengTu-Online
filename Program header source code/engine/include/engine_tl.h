/**
 * \file      engine_tl.h
 * \version   $Id$
 * \author    许朝
 *            xuzhaoh@tom.com
 * \date      2005-07-05 12:28:31
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#ifndef _ENGINE_TL_H
#define _ENGINE_TL_H

class CList;
#include <vector>

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CSimpleNode {	// 叶点
	friend class CList ;
	CSimpleNode	*_NEXT;								// 指向下一节点的指针
	CSimpleNode	*_PREV;								// 指向前一个节点的指针
	CList		* pList;
public:
	virtual	~CSimpleNode(){};
	static void DeleteANode(CSimpleNode* p);

	CSimpleNode() : _NEXT(NULL) , _PREV(NULL) , pList(NULL) {}

	CSimpleNode	*	_Next	( ){ return _NEXT;}					// 指向下一个节点
	CSimpleNode	*	_Prev	( ){ return _PREV;}					// 指向上一个节点
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CList 
{	// 链表头
protected:
	CSimpleNode *_BEGIN;						//	叶头
	CSimpleNode *_END;							//	叶尾

	size_t		m_iTotalNode;					//	链表中总共的叶点数量
	bool _Unlink(CSimpleNode* pNode);
public:
	CList ()
		: _BEGIN(NULL) , _END(NULL) , m_iTotalNode(0)
	{}

	~CList (){ _Clear();}

	void	_Insert		( CSimpleNode *pCNODE,CSimpleNode* pNewNODE);
	void	_Add		( CSimpleNode *pCNODE );	//	得到的是唯一ID
	bool	_Delete		( CSimpleNode *pCNODE );
	bool	_ToEnd		( CSimpleNode *pCNODE );

	CSimpleNode	*_Begin		( ) { return _BEGIN;};
	CSimpleNode	*_End		( ) { return _END;};

	size_t	GetCount() { return m_iTotalNode;}

	bool _IsLink(CSimpleNode* pNode)
	{
		return (pNode->pList == this);
	}

	bool _ToBegin(CSimpleNode* pNode);
	void _AddToBegin(CSimpleNode* pNODE);
	void _Clear(void);

};

// 堆
template <class _Ty>
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class  TSTACK {	
	std::vector<_Ty> m_ppNode;
public:

	TSTACK( size_t iBufferSize )
	{
		m_ppNode.reserve(iBufferSize);
	}
	
	void* address()
	{
		assert( 0 < _Count());
		if(0 >= _Count()) return NULL;
		return &m_ppNode[0];
	}

	_Ty operator [] (size_t i)
	{
		assert( i < _Count());
		if(i >= _Count()) return NULL;
		return m_ppNode[i];
	}

	_Ty GetAt(size_t i)
	{
		return (*this)[i];
	}

	void _Push( _Ty  pNODE )
	{
		m_ppNode.push_back(pNODE);
	}

	_Ty _Pop()
	{
		assert(_Count() == 0);
		if(_Count() == 0)
			return NULL;

		std::vector<_Ty>::iterator it = m_ppNode.end();
		--it;
		_Ty pNode  = *it;
		m_ppNode.erase(it);
		return pNode;
	}

	void	_Clear	()
	{
		m_ppNode.resize(0);
	}

	size_t	_Count(){return m_ppNode.size();}
 };

#endif