/**
 * \file      engine_tl.cpp
 * \version   $Id$
 * \author    Ðí³¯
 *            xuzhaoh@tom.com
 * \date      2005-07-05 12:28:31
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#include "../include/engine.h"
#include "../include/engine_tl.h"


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pCNODE : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//////////////////////////////////////////////////////////////////////////////////////////////
//	_Add( CSimpleNode *pCNODE )
//	Ôö¼ÓÒ»¸öÒ¶µã
//////////////////////////////////////////////////////////////////////////////////////////////
void CList ::_Add( CSimpleNode *pCNODE )
{
	assert(pCNODE);

	if ( _BEGIN == NULL && _END == NULL )	// Í·Ò»¸ö±»²åÈëµÄ½Úµã
	{
		_BEGIN	=	pCNODE;
		_END	=	pCNODE;
		pCNODE->_PREV = NULL;
		pCNODE->_NEXT = NULL;
	} 
	else 
	{
		_Insert( _END,pCNODE);
	}
	pCNODE->pList = this;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pNODE : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CList ::_AddToBegin(CSimpleNode* pNODE)
{
	if ( _BEGIN == NULL && _END == NULL )	// Í·Ò»¸ö±»²åÈëµÄ½Úµã
	{
		_BEGIN	=	pNODE;
		_END	=	pNODE;
		pNODE->_PREV = NULL;
		pNODE->_NEXT = NULL;
		
	} 
	else
	{
		pNODE->_PREV=NULL;
		pNODE->_NEXT=_BEGIN;
		_BEGIN->_PREV=pNODE;
		_BEGIN = pNODE;
	}
	pNODE->pList = this;
	m_iTotalNode++;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pCNODE : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//////////////////////////////////////////////////////////////////////////////////////////////
//	_ToEnd( CSimpleNode *pCNODE )
//	½«µ±Ç°½Úµã·ÅÖÃµ½×îºóµÄÎ»ÖÃ
//////////////////////////////////////////////////////////////////////////////////////////////
bool CList ::_ToEnd( CSimpleNode *pCNODE )
{
	if(pCNODE == NULL) return false;

	_Unlink(pCNODE);
	_Add( pCNODE );

	return	true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pNode : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CList ::_ToBegin(CSimpleNode* pNode)
{
	if(pNode == NULL) return false;

	if(_BEGIN == pNode) return true;

	_Unlink(pNode);
	pNode->_NEXT=_BEGIN;
	_BEGIN->_PREV=pNode;
	_BEGIN=pNode;
	m_iTotalNode++;
	return true;

}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pNode : ÃèÊö
 * \param pNewNode : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void	CList ::_Insert	( CSimpleNode *pNode,CSimpleNode* pNewNode)
{
	assert(pNode && pNewNode);

	pNewNode->_NEXT =	pNode->_NEXT;
	pNewNode->_PREV =	pNode;

	if ( pNode->_NEXT )
	{
		assert(pNode != _END);
		pNode->_NEXT->_PREV	= pNewNode;
	} 
	else 
	{
		assert(pNode == _END);
		_END = pNewNode;
	}
	pNode->_NEXT = pNewNode;
	pNewNode->pList = this;

	m_iTotalNode ++;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pNode : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CList ::_Unlink(CSimpleNode* pNode)
{
	assert(pNode);
	
	if(pNode->_PREV)
	{
		assert(_BEGIN != pNode);
		pNode->_PREV->_NEXT = pNode->_NEXT;
	}
	else
	{
		assert(_BEGIN == pNode);
		_BEGIN = pNode->_NEXT;
	}

	if(pNode->_NEXT)
	{
		assert(pNode != _END);
		pNode->_NEXT->_PREV = pNode->_PREV;
	}
	else
	{
		assert(pNode == _END);
		_END = pNode->_PREV;
	}

	pNode->_NEXT = NULL;
	pNode->_PREV = NULL;

	pNode->pList = NULL;
	m_iTotalNode --;

	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pCNODE : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//////////////////////////////////////////////////////////////////////////////////////////////
//	_Delete( CSimpleNode *pCNODE )
//	É¾³ýÒ»¸öÒ¶µã
//////////////////////////////////////////////////////////////////////////////////////////////
bool CList ::_Delete( CSimpleNode *pCNODE )
{
	_Unlink(pCNODE);
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CList ::_Clear(void)
{
	CSimpleNode *pNode	=	NULL;
	CSimpleNode *pNext	=	NULL;

	for( pNode = _BEGIN; pNode != NULL;  )	// É¾³ý
	{
		pNext	=	pNode	-> _NEXT;
		_Delete(pNode);
		pNode	=	pNext;
	}
	_BEGIN = NULL;
	_END = NULL;
	m_iTotalNode = 0;
}