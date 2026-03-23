/********************************************************************
	created:	
	created:	
	filename: 	
	file path:	
	file base:	astar
	file ext:	h
	author:		$$$03公司
	
	purpose:	
*********************************************************************/
 
#pragma once

#include <stdio.h>
#include <malloc.h>
#include <vector>
#include <queue>

//#define USE_FINDPATH_MODE2


#ifndef BIT
#define BIT(x) (1<<(x))
#endif
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CAStar
{
	enum{
		DIR_UP			= BIT(0),
		DIR_UPRIGHT		= BIT(1),
		DIR_RIGHT		= BIT(2),
		DIR_BOTTOMRIGHT = BIT(3),
		DIR_BOTTOM		= BIT(4),
		DIR_BOTTOMLEFT	= BIT(5),
		DIR_LEFT		= BIT(6),
		DIR_UPLEFT		= BIT(7),
	};
public:
	enum enumFindPathMode{
		eFindPathMode1,// 一格或多格搜索
		eFindPathMode2,// 一格一格的搜索
	};
	
	enumFindPathMode m_eMode;
	
	void SetFindPathMode(enumFindPathMode mode)
	{
		m_eMode = mode;
	}
	
	enumFindPathMode GetFindPathMode()
	{
		return m_eMode;
	}

	typedef void* (__cdecl *fn_malloc)( size_t size );
	typedef void (__cdecl *fn_free)( void *memblock );
	typedef int (__stdcall * fn_FreeTile)(int x,int y);
	typedef int (__stdcall * fn_FreeTile2)(int x1,int y1,int x2,int y2);
	enum enumNodeTable{
		NoneTable,
		OpenTable,
		CloseTable,
	};

	struct NODE {     // node structure
		long f;			// 结点的预计代价值
		long h;			// 结点到目标结点的启发值
		long g;			// 源结点到本结点的实际代价
		union{
			struct{
				short x, y;
			};
			int NodeNum;// 结点编号
		};
		NODE *Parent;	
		NODE *Child[16]; // 周围8个结点
		//enumNodeTable tab;
	};

	struct OPEN_NODE{
		NODE* nod;
		bool operator < (const OPEN_NODE n)const {
			return nod->f > n.nod->f;
		}
		OPEN_NODE(){}
		OPEN_NODE(NODE* _nod):nod(_nod){}
	};

	
	struct stPointI{
		long x,y;
	};
	typedef std::vector<NODE*> STACK;
	typedef std::queue<stPointI> PATH;
	typedef std::vector<stPointI> PATHTMP;
private:
	PATH    m_Path; // pointer to the best path
	typedef std::map<int,NODE*> tMapNodes;
	tMapNodes m_mapOpen;
	tMapNodes m_mapClose;

	std::vector<OPEN_NODE> m_Open;
	//std::vector<NODE*> m_Close;
	//LIST*   m_Close;
	//LIST*   m_Open;
	int		m_iStep;//步幅
	STACK	m_Stack;
	PATHTMP	m_TmpPath;
	// 内存分配函数
	fn_malloc m_fn_malloc;
	fn_free	  m_fn_free;
	fn_FreeTile m_fn_FreeTile;
	fn_FreeTile2 m_fn_freeTile2;

	int		m_src_x,m_src_y;
	int		m_dst_x,m_dst_y;
	NODE*	m_latest_node;
	stPointI m_PathPreNode;
	int		m_iMaxFindDis;
public:
	int GetMaxFindDis(){ return m_iMaxFindDis;}
	void SetMaxFindDis(int i){ m_iMaxFindDis = i;}
	int GetPathPreNodeX(){ return m_PathPreNode.x;}
	int GetPathPreNodeY(){ return m_PathPreNode.y;}

	int GetDstX(){ return m_dst_x;}
	int GetDstY(){ return m_dst_y;}
	int GetStep(){ return m_iStep;}
	void SetMallocFn(fn_malloc p_fn_malloc,fn_free p_fn_free)
	{
		m_fn_malloc = p_fn_malloc;
		m_fn_free	= p_fn_free;
	}
	
	void SetFreeTile(fn_FreeTile p_fn_FreeTile)
	{
		m_fn_FreeTile = p_fn_FreeTile;
	}

	void SetFreeTile2(fn_FreeTile2 p_fn)
	{
		m_fn_freeTile2 = p_fn;
	}

	//判断两个坐标点是否可以通过
	int FreeTile(int x1,int y1,int x2,int y2);

	CAStar(void);
	~CAStar(void);

	void			SetStep(int iStep){m_iStep = iStep;}
	CAStar::NODE*	ReturnBestNode(void);
	void FindPath(int srcx, int srcy, int dstx, int dsty);
	void GenerateSuccessors(NODE* BestNode);
	void GenerateSuccessors2(NODE* BestNode);
	bool GenerateSucc(NODE *BestNode,int x, int y);
	void PropagateDown(CAStar::NODE* Old);
	void InsertToOpen(CAStar::NODE* Successor);
	CAStar::NODE* CheckOPEN(int tilenum);
	CAStar::NODE* CheckCLOSE(int tilenum);
	void FreeNodes(void);

	void PathNextNode(void) { m_PathPreNode = m_Path.front();m_Path.pop();}

	int NodeGetX(void)      { return m_Path.front().x; }
	int NodeGetY(void)      { return m_Path.front().y; }
	int LastNodeGetX()		{ return m_Path.back().x;}
	int LastNodeGetY()		{ return m_Path.back().y;}
	void RePrePathNode()
	{ 
		if(m_PathPreNode.x != -1 && m_PathPreNode.y != -1)
		{
			m_Path.c.push_front(m_PathPreNode);
		}
	}
	void FreePathTail()
	{
		while(m_Path.c.size() > 1)
			m_Path.c.pop_back();
	}
	int IsHavePath()		{ return !m_Path.c.empty();}

	void PrintfPath();
	void FreePath(void);
	// 把结点 Node 压入栈中
	static void Push(STACK & p,NODE * Node);

	// 从栈顶弹出结点
	static NODE* Pop(STACK & p);

	void InsertToPath(stPointI & node);
	void InsertToPath1(stPointI & node);
	void EndPath();

	int GetNewDstPoint(int srcx,int srcy,int &dstx,int &dsty);
	
};
