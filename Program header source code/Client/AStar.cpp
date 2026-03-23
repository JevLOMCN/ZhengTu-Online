/********************************************************************
	created:	
	created:	
	filename: 	
	file path:	
	file base:	astar
	file ext:	cpp
	author:		$$$03公司
	
	purpose:	
*********************************************************************/

#include "public.h"
#include "astar.h"
#include <memory.h>
#include "../engine/include/stack_alloc.h"
#include <algorithm>
bool gFindMode3 = false;

bool g_AStarFast = false;
const int catNum = 14;
const int rightNum = 10;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x : 描述
 * \return 返回值的描述
 */
inline int ABS(int x) {
	//FUNCTION_BEGIN;
 return (x < 0 ? -x : x);
	//FUNCTION_END;
}
inline int MAX(int x ,int y) {
	//FUNCTION_BEGIN;
 return (x > y ? x : y);
	//FUNCTION_END;
}

bool gFindPathDisMode2 = false;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x1 : 描述
 * \param y1 : 描述
 * \param x2 : 描述
 * \param y2 : 描述
 * \return 返回值的描述
 */
inline int GetDis(int x1,int y1,int x2,int y2)
{
	return MAX(ABS(x2-x1) , ABS(y2-y1));
	//return (int)sqrtf((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
	//return (ABS(x2-x1) + ABS(y2-y1)) * rightNum;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x1 : 描述
 * \param y1 : 描述
 * \param x2 : 描述
 * \param y2 : 描述
 * \return 返回值的描述
 */
inline int GetDis2(int x1,int y1,int x2,int y2)
{
	//return MAX(ABS(x2-x1) , ABS(y2-y1));
	return (ABS(x2-x1) + ABS(y2-y1));
}

inline int GetDis3(int x1,int y1,int x2,int y2)
{
	return MAX(ABS(x2-x1) , ABS(y2-y1));
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x1 : 描述
 * \param y1 : 描述
 * \param x2 : 描述
 * \param y2 : 描述
 * \return 返回值的描述
 */
inline int IsLine(int x1,int y1,int x2,int y2)
{
	//FUNCTION_BEGIN;

	if(x1 == x2) return 1;
	if(y1 == y2) return 1;
	return (ABS(y1 - y2) == ABS(x1 - x2));

	//FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x1 : 描述
 * \param y1 : 描述
 * \param x2 : 描述
 * \param y2 : 描述
 * \param x3 : 描述
 * \param y3 : 描述
 * \return 返回值的描述
 */
inline int IsLine(int x1,int y1,int x2,int y2,int x3,int y3)
{
	//FUNCTION_BEGIN;

	if(x1 == x2 && x1 == x3) return 1;
	if(y1 == y2 && y1 == y3) return 1;
	//return ( (y2 - y1)/(y3 - y2) == (x2 - x1)/(x3 - x2) );
	return ( (y2 - y1) * (x3 - x2) == (x2 - x1) * (y3 - y2) );
	//FUNCTION_END;
}




/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x1 : 描述
 * \param y1 : 描述
 * \param x2 : 描述
 * \param y2 : 描述
 * \return 返回值的描述
 */
//判断两个坐标点是否可以通过
int CAStar::FreeTile(int x1,int y1,int x2,int y2)
{
	//FUNCTION_BEGIN;
	if(m_fn_freeTile2)
	{
		//return 1;
		return m_fn_freeTile2(x1,y1,x2,y2);
	}

	int dx,dy;

	if(x2 > x1) dx = 1;
	else if(x2 < x1) dx = -1;
	else	dx = 0;

	if(y2 > y1) dy = 1;
	else if(y2 < y1) dy = -1;
	else	dy = 0;

	while(1)
	{
		x1 += dx;
		y1 += dy;

		if((x1 != x2 || y1 != y2) && x1 == m_dst_x && y1 == m_dst_y)
			return 0;

		if(!m_fn_FreeTile(x1,y1)) return 0;

		if(x1 == x2 && y1 == y2) return 1;
	}

	//FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CAStar::CAStar(void)
{
	//FUNCTION_BEGIN;
	m_eMode = eFindPathMode1;

	m_latest_node = NULL;
	/*m_Open=NULL;
	m_Close=NULL;*/
	m_iStep = 1;
	m_fn_malloc = malloc;
	m_fn_free	= free;
	m_fn_freeTile2 = NULL;

	m_TmpPath.reserve(256);

	m_Open.reserve(1024);
	//m_Close.reserve(1024);

	m_Path.c.resize(1024);
	m_Path.c.resize(0);
	m_Stack.reserve(1024);
	m_dst_x = 0;
	m_dst_y = 0;
	m_fn_FreeTile = NULL;
	m_PathPreNode.x = -1;
	m_PathPreNode.y = -1;
	m_iMaxFindDis = 40;

	//FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CAStar::~CAStar(void)
{
	//FUNCTION_BEGIN;

	FreeNodes();
	FreePath();

	//FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param srcx : 描述
 * \param srcy : 描述
 * \param dstx : 描述
 * \param dsty : 描述
 * \return 返回值的描述
 */
int CAStar::GetNewDstPoint(int srcx,int srcy,int &dstx,int &dsty)
{
	//FUNCTION_BEGIN;

	extern stRectI s_rcFindPathRect;
	if(m_fn_FreeTile(dstx,dsty)) return 1;
	
	int dis0 = GetDis2(srcx,srcy,dstx,dsty);
	if(dis0 > m_iMaxFindDis)
	{
		dstx = srcx + (dstx - srcx) * m_iMaxFindDis/dis0;
		dsty = srcy + (dsty - srcy) * m_iMaxFindDis/dis0;

	}
	//return 1;
	dis0 = GetDis2(srcx,srcy,dstx,dsty);

	int mask = 0;
	if(srcx > dstx)
	{
		mask |= DIR_RIGHT;
	}
	else if(srcx < dstx)
	{
		mask |= DIR_LEFT;
	}

	if(srcy > dsty)
	{
		mask |= DIR_BOTTOM;
	}
	else if(srcy < dsty)
	{
		mask |= DIR_UP;
	}

	if(mask == 0) return 0;

	for(int i=1;i<=dis0;i++)
	{
		int min_dis = 0x7fffffff;
		int x0,y0;
		int x,y;
		int c;
		int dis;

		if(mask & DIR_RIGHT)
		{
			x = dstx+i;
			y = dsty-i;
			c = dsty+i;
			for(;y<=c;y++)
			{
				if(m_fn_FreeTile(x,y))
				{
					if((dis = GetDis(x,y,dstx,dsty) /*+ GetDis(x,y,srcx,srcy)*/) < min_dis)
					{
						min_dis = dis;
						x0 = x;
						y0 = y;
					}
				}
			}
		}

		if(mask & DIR_LEFT)
		{
			x = dstx-i;
			y = dsty-i;
			c = dsty+i;
			for(;y<=c;y++)
			{
				if(m_fn_FreeTile(x,y))
				{
					if((dis = GetDis(x,y,dstx,dsty)/* + GetDis(x,y,srcx,srcy)*/) < min_dis)
					{
						min_dis = dis;
						x0 = x;
						y0 = y;
					}
				}
			}
		}

		if(mask & DIR_UP)
		{
			x = dstx-i;
			y = dsty-i;
			c = dstx+i;
			for(;x<=c;x++)
			{
				if(m_fn_FreeTile(x,y))
				{
					if((dis = GetDis(x,y,dstx,dsty) /*+ GetDis(x,y,srcx,srcy)*/) < min_dis)
					{
						min_dis = dis;
						x0 = x;
						y0 = y;
					}
				}
			}
		}

		if(mask & DIR_BOTTOM)
		{
			x = dstx-i;
			y = dsty+i;
			c = dstx+i;
			for(;x<=c;x++)
			{
				if(m_fn_FreeTile(x,y))
				{
					if((dis = GetDis(x,y,dstx,dsty) /*+ GetDis(x,y,srcx,srcy)*/) < min_dis)
					{
						min_dis = dis;
						x0 = x;
						y0 = y;
					}
				}
			}
		}
		if(min_dis != 0x7fffffff)
		{
			dstx = x0;
			dsty = y0;
			return 1;
		}
	}
	return 0;

	//FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
/*
 *	
 Best_First_Search()
{
	//FUNCTION_BEGIN;

　Open = [起始节点];
　Closed = [];
　while (Open表非空)
　{
　　从Open中取得一个节点X，并从OPEN表中删除。
　　if (X是目标节点)
　　{
　　　求得路径PATH；
　　　返回路径PATH；
　　}
　　for (每一个X的子节点Y)
　　{
　　　if (Y不在OPEN表和CLOSE表中)
　　　{
　　　　求Y的估价值；
　　　　并将Y插入OPEN表中；
　　　}
　　　//还没有排序
　　　else if (Y在OPEN表中)
　　　{
　　　　if (Y的估价值小于OPEN表的估价值)
　　　　　更新OPEN表中的估价值；
　　　}
　　　else //Y在CLOSE表中
　　　{
　　　　if (Y的估价值小于CLOSE表的估价值)
　　　　{
　　　　　更新CLOSE表中的估价值；
　　　　　从CLOSE表中移出节点，并放入OPEN表中；
　　　　}
　　　}
　　　将X节点插入CLOSE表中；
　　　按照估价值将OPEN表中的节点排序；
　　}//end for
　}//end while

	//FUNCTION_END;
}//end func

*/
bool gbGetNewPoint = true;
// 寻路...
void CAStar::FindPath(int srcx, int srcy, int dstx, int dsty)
{
	//FUNCTION_BEGIN;

	FreePath();

	m_dst_x = dstx;
	m_dst_y = dsty;

	if(gbGetNewPoint)
		if(m_fn_FreeTile && !GetNewDstPoint(srcx,srcy,m_dst_x,m_dst_y))
			return;

	m_latest_node	=	NULL;
	m_src_x = srcx;
	m_src_y = srcy;

	NODE *Node, *BestNode;
	int TileNumDest;
	// 目标结点编号
	TileNumDest = ((m_dst_y<<16)|m_dst_x);

	m_Open.resize(0);
	m_mapOpen.clear();
	//m_Close.resize(0);
	m_mapClose.clear();
	//m_Open=NULL;		// 开放的列表(未走过的结点)
	//m_Close=NULL;		// 关闭的列表(已经走过的结点)

	Node=( NODE* )m_fn_malloc(sizeof( NODE ));
	Node->g = 0;
	Node->h = GetDis(dstx,dsty,srcx,srcy);
	Node->f = Node->g + Node->h;
	Node->x = m_src_x;
	Node->y = m_src_y;
	Node->Parent = NULL;
	memset(&Node->Child,0,sizeof(Node->Child));

	InsertToOpen(Node);

	DWORD time = xtimeGetTime();

	int run = 0;
	// -------
	// 寻路过程
	// -------
	for (;;)
	{
		// 从开放表中取出头一个结点(评估值最小),并放入关闭表中
		NODE* tmpNode=ReturnBestNode();
		
		if(tmpNode==NULL) break;

		BestNode=tmpNode;

		// 如果是目标结点则结束
		if (BestNode->NodeNum == TileNumDest)
			break;
		
		// 取出BestNode的相邻的所有子结点,计算评估值,并排序插入开放表中
		if(m_eMode == eFindPathMode1)
			GenerateSuccessors(BestNode);
		else
			GenerateSuccessors2(BestNode);
		if(++run > 100)
		{
			if(xtimeGetTime() - time > 1600)
			{
				break;
			}
			run = 0;
		}
	}
	//--------
	//--------

	if(!m_mapClose.empty())
	{
		NODE* tmp = m_latest_node;//m_Close->NodePtr;
		static std::vector<stPointI> tmpPath;
		tmpPath.resize(0);
		while(tmp)
		{
			tmpPath.push_back(stPointI());
			tmpPath.back().x = tmp->x;
			tmpPath.back().y = tmp->y;
			tmp=tmp->Parent;
		}

		int end = 0;

		if( m_eMode == eFindPathMode1 )
		{
			for(int i = tmpPath.size()-1;i >= end;i--)
			{
				InsertToPath1(tmpPath[i]);
			}
		}
		else
		{
			if(m_iStep == 1)
			{
				for(int i = tmpPath.size()-1;i >= end;i--)
				{
					InsertToPath1(tmpPath[i]);
				}
			}
			else
			{
				for(int i = tmpPath.size()-1;i >= end;i--)
				{
					InsertToPath(tmpPath[i]);
				}
				EndPath();
			}
		}
	}
	FreeNodes();

	m_dst_x = dstx;
	m_dst_y = dsty;

	//FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
// -----------------------------------------------------
// 从开放表中取出评估值最小的结点(就是第一个结点),并放入关闭表中
// -----------------------------------------------------
CAStar::NODE* CAStar::ReturnBestNode(void)
{
	//FUNCTION_BEGIN;

	if(m_Open.empty())
		return NULL;

	std::pop_heap(m_Open.begin(),m_Open.end());
	NODE* nod = m_Open.back().nod;
	m_Open.pop_back();
	m_mapOpen.erase(m_mapOpen.find(nod->NodeNum));

	if(m_latest_node == NULL || m_latest_node->h > nod->h) 
		m_latest_node = nod;

	m_mapClose[nod->NodeNum] = nod;

	return nod;
	/*
	LIST *tmp;

	if(m_Open == NULL) return NULL;
	// Pick Node with lowest f, in this case it's the first node in list
	// because we sort the m_Open list wrt lowest f. Call it BESTNODE.

	tmp = m_Open;   // point to first node on m_Open
	m_Open = tmp->NextPtr;    // Make m_Open point to nextnode or NULL.

	// Next take BESTNODE (or temp in this case) and put it on CLOSED

	if(m_latest_node == NULL || m_latest_node->h > tmp->NodePtr->h) m_latest_node = tmp->NodePtr;
	tmp->NextPtr = m_Close;
	m_Close = tmp;

	return(tmp->NodePtr);*/

	//FUNCTION_END;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param BestNode : 描述
 * \return 返回值的描述
 */
// 取出BestNode的相邻的所有子结点,计算评估值,并排序插入开放表中
void CAStar::GenerateSuccessors2(CAStar::NODE* BestNode)
{
	//FUNCTION_BEGIN;

	int x, y;
	const int iStep = 1;

	// Upper-Left
	if ( m_fn_FreeTile( x = BestNode->x - iStep, y = BestNode->y - iStep) )
	{
		GenerateSucc(BestNode,x,y);
	}

	// Upper
	if ( m_fn_FreeTile(x=BestNode->x, y=BestNode->y - iStep) )
	{
		GenerateSucc(BestNode,x,y);
	}

	// Upper-Right
	if ( m_fn_FreeTile(x=BestNode->x + iStep, y=BestNode->y - iStep) )
	{
		GenerateSucc(BestNode,x,y);
	}

	// Right
	if ( m_fn_FreeTile(x=BestNode->x + iStep, y=BestNode->y) )
	{
		GenerateSucc(BestNode,x,y);
	}

	// Lower-Right
	if ( m_fn_FreeTile(x=BestNode->x + iStep, y=BestNode->y + iStep) )
	{
		GenerateSucc(BestNode,x,y);
	}

	// Lower
	if ( m_fn_FreeTile(x=BestNode->x, y=BestNode->y + iStep) )
	{
		GenerateSucc(BestNode,x,y);
	}

	// Lower-Left
	if ( m_fn_FreeTile(x=BestNode->x - iStep, y=BestNode->y + iStep) )
	{
		GenerateSucc(BestNode,x,y);
	}

	// Left
	if ( m_fn_FreeTile(x=BestNode->x - iStep, y=BestNode->y) )
	{
		GenerateSucc(BestNode,x,y);
	}

	//FUNCTION_END;
}

static int curStep;
static int curNum;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param BestNode : 描述
 * \return 返回值的描述
 */
// 取出BestNode的相邻的所有子结点,计算评估值,并排序插入开放表中
void CAStar::GenerateSuccessors(CAStar::NODE* BestNode)
{
	if(GetDis3(BestNode->x,BestNode->y,m_dst_x,m_dst_y) >= m_iStep)
	{
		int x, y;
		int iStep = m_iStep;
		int dis0 = GetDis3(BestNode->x,BestNode->y,m_dst_x,m_dst_y);
		bool bFreeTile = false;
		curStep = iStep;

		if(g_AStarFast){
			if( FreeTile( BestNode->x,BestNode->y,x = BestNode->x - iStep, y = BestNode->y - iStep) )
			{
				curNum = catNum;
				if(GenerateSucc(BestNode,x,y))
					bFreeTile = true;
			}

			// Upper
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x, y=BestNode->y - iStep) )
			{
				curNum = rightNum;
				if(GenerateSucc(BestNode,x,y))
					bFreeTile = true;
			}

			// Upper-Right
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x + iStep, y=BestNode->y - iStep) )
			{
				curNum = catNum;
				if(GenerateSucc(BestNode,x,y))
					bFreeTile = true;
			}

			// Right
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x + iStep, y=BestNode->y) )
			{
				curNum = rightNum;
				if(GenerateSucc(BestNode,x,y))
					bFreeTile = true;
			}

			// Lower-Right
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x + iStep, y=BestNode->y + iStep) )
			{
				curNum = catNum;
				if(GenerateSucc(BestNode,x,y))
					bFreeTile = true;
			}

			// Lower
			if (FreeTile(BestNode->x ,BestNode->y,x=BestNode->x, y=BestNode->y + iStep) )
			{
				curNum = rightNum;
				if(GenerateSucc(BestNode,x,y))
					bFreeTile = true;
			}

			// Lower-Left
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x - iStep, y=BestNode->y + iStep) )
			{
				curNum = catNum;
				if(GenerateSucc(BestNode,x,y))
					bFreeTile = true;
			}

			// Left
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x - iStep, y=BestNode->y) )
			{
				curNum = rightNum;
				if(GenerateSucc(BestNode,x,y))
					bFreeTile = true;
			}
		}else{
			if( FreeTile( BestNode->x,BestNode->y,x = BestNode->x - iStep, y = BestNode->y - iStep) )
			{
				curNum = catNum;
				if(GenerateSucc(BestNode,x,y) && GetDis3(x,y,m_dst_x,m_dst_y) < dis0 )
					bFreeTile = true;
			}

			// Upper
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x, y=BestNode->y - iStep) )
			{
				curNum = rightNum;
				if(GenerateSucc(BestNode,x,y) && GetDis3(x,y,m_dst_x,m_dst_y) < dis0 )
					bFreeTile = true;
			}

			// Upper-Right
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x + iStep, y=BestNode->y - iStep) )
			{
				curNum = catNum;
				if(GenerateSucc(BestNode,x,y) && GetDis3(x,y,m_dst_x,m_dst_y) < dis0 )
					bFreeTile = true;
			}

			// Right
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x + iStep, y=BestNode->y) )
			{
				curNum = rightNum;
				if(GenerateSucc(BestNode,x,y) && GetDis3(x,y,m_dst_x,m_dst_y) < dis0 )
					bFreeTile = true;
			}

			// Lower-Right
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x + iStep, y=BestNode->y + iStep) )
			{
				curNum = catNum;
				if(GenerateSucc(BestNode,x,y) && GetDis3(x,y,m_dst_x,m_dst_y) < dis0 )
					bFreeTile = true;
			}

			// Lower
			if (FreeTile(BestNode->x ,BestNode->y,x=BestNode->x, y=BestNode->y + iStep) )
			{
				curNum = rightNum;
				if(GenerateSucc(BestNode,x,y) && GetDis3(x,y,m_dst_x,m_dst_y) < dis0 )
					bFreeTile = true;
			}

			// Lower-Left
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x - iStep, y=BestNode->y + iStep) )
			{
				curNum = catNum;
				if(GenerateSucc(BestNode,x,y) && GetDis3(x,y,m_dst_x,m_dst_y) < dis0 )
					bFreeTile = true;
			}

			// Left
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x - iStep, y=BestNode->y) )
			{
				curNum = rightNum;
				if(GenerateSucc(BestNode,x,y) && GetDis3(x,y,m_dst_x,m_dst_y) < dis0 )
					bFreeTile = true;
			}
		}

		if(!bFreeTile)
		{
			iStep = 1;
			curStep = iStep;
			if( FreeTile( BestNode->x,BestNode->y,x = BestNode->x - iStep, y = BestNode->y - iStep) )
			{
				curNum = catNum;
				GenerateSucc(BestNode,x,y);
			}

			// Upper
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x, y=BestNode->y - iStep) )
			{
				curNum = rightNum;
				GenerateSucc(BestNode,x,y);
			}

			// Upper-Right
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x + iStep, y=BestNode->y - iStep) )
			{
				curNum = catNum;
				GenerateSucc(BestNode,x,y);
			}

			// Right
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x + iStep, y=BestNode->y) )
			{
				curNum = rightNum;
				GenerateSucc(BestNode,x,y);
			}

			// Lower-Right
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x + iStep, y=BestNode->y + iStep) )
			{
				curNum = catNum;
				GenerateSucc(BestNode,x,y);
			}

			// Lower
			if (FreeTile(BestNode->x ,BestNode->y,x=BestNode->x, y=BestNode->y + iStep) )
			{
				curNum = rightNum;
				GenerateSucc(BestNode,x,y);
			}

			// Lower-Left
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x - iStep, y=BestNode->y + iStep) )
			{
				curNum = catNum;
				GenerateSucc(BestNode,x,y);
			}

			// Left
			if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x - iStep, y=BestNode->y) )
			{
				curNum = rightNum;
				GenerateSucc(BestNode,x,y);
			}
		}
	}
	else
	{
		int x, y;
		int iStep = 1;
		curStep = iStep;
		if( FreeTile( BestNode->x,BestNode->y,x = BestNode->x - iStep, y = BestNode->y - iStep) )
		{
			curNum = catNum;
			GenerateSucc(BestNode,x,y);
		}

		// Upper
		if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x, y=BestNode->y - iStep) )
		{
			curNum = rightNum;
			GenerateSucc(BestNode,x,y);
		}

		// Upper-Right
		if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x + iStep, y=BestNode->y - iStep) )
		{
			curNum = catNum;
			GenerateSucc(BestNode,x,y);
		}

		// Right
		if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x + iStep, y=BestNode->y) )
		{
			curNum = rightNum;
			GenerateSucc(BestNode,x,y);
		}

		// Lower-Right
		if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x + iStep, y=BestNode->y + iStep) )
		{
			curNum = catNum;
			GenerateSucc(BestNode,x,y);
		}

		// Lower
		if (FreeTile(BestNode->x ,BestNode->y,x=BestNode->x, y=BestNode->y + iStep) )
		{
			curNum = rightNum;
			GenerateSucc(BestNode,x,y);
		}

		// Lower-Left
		if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x - iStep, y=BestNode->y + iStep) )
		{
			curNum = catNum;
			GenerateSucc(BestNode,x,y);
		}

		// Left
		if (FreeTile(BestNode->x,BestNode->y,x=BestNode->x - iStep, y=BestNode->y) )
		{
			curNum = rightNum;
			GenerateSucc(BestNode,x,y);
		}
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param BestNode : 描述
 * \param x : 描述
 * \param y : 描述
 * \return 返回值的描述
 */
// 取出BestNode的相邻的一个子结点,计算评估值,并按顺序插入开放表中
bool CAStar::GenerateSucc(CAStar::NODE *BestNode,int x, int y)
{
	//FUNCTION_BEGIN;

	int g, TileNumS, c = 0;
	NODE *Successor;


	//计算新的评估值
	if(g_AStarFast)
		g = BestNode->g + m_iStep;// g(Successor)=g(BestNode)+cost of getting from BestNode to Successor
	else
		g = BestNode->g + curStep;

	TileNumS = ((y<<16)|x);		// identification purposes 结点编号

	bool retval = false;
	if ( ( Successor = CheckOPEN(TileNumS)) != NULL ) // if equal to NULL then not in m_Open list, else it returns the Node in Old
	{
		//这个(x,y)点在开放表中
		// 如果评估值更小,则更新结点

		if ( g < Successor->g )  // if our new g value is < Old's then reset Old's parent to point to BestNode
		{
			Successor->Parent = BestNode;
			Successor->g = g;
			Successor->f = g + Successor->h;
			//if(!g_AStarFast)
				std::make_heap(m_Open.begin(),m_Open.end());
		}
	}
	else if ( (Successor=CheckCLOSE(TileNumS)) != NULL ) // if equal to NULL then not in m_Open list, else it returns the Node in Old
	{
		//如果(x,y)点在关闭表中
		// 如果评估值更小,则更新结点与周围结点的评估值
		if ( g < Successor->g )  // if our new g value is < Old's then reset Old's parent to point to BestNode
		{
			Successor->Parent = BestNode;
			Successor->g = g;
			Successor->f = g + Successor->h;

			//更新与(x,y)点相联的所有已经走过的结点的评估值
			PropagateDown(Successor);  // Since we changed the g value of Old, we need
			// to propagate this new value downwards, i.e.
			// do a Depth-First traversal of the tree!
		}
	}
	else
	{
		//子结点既不在开放表,也不在关闭表中,则建立子结点计算评估值,插入开放表中
		Successor = ( NODE* )m_fn_malloc(sizeof( NODE ));
		Successor->Parent = BestNode;
		Successor->g = g;
		Successor->h = GetDis(x,y,m_dst_x,m_dst_y);  // should do sqrt(), but since we don't really
		
		Successor->f = g+Successor->h;     // care about the distance but just which branch looks
		Successor->x = x;                 // better this should suffice. Anyayz it's faster.
		Successor->y = y;
		Successor->NodeNum = TileNumS;
		memset(&Successor->Child,0,sizeof(Successor->Child));
		InsertToOpen(Successor);     // Insert Successor on m_Open list wrt f

		retval = true;
	}

	
	// 把节点链接到 BastNode
	int nChildNum = (m_eMode == eFindPathMode1 ? 16 : 8);
	for( c = 0; c < nChildNum; c++)
	{
		if( BestNode->Child[c] == NULL ) // Add Old to the list of BestNode's Children (or Successors).
		{
			BestNode->Child[c] = Successor;
			break;
		}
	}

	return retval;
	//FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param Old : 描述
 * \return 返回值的描述
 */
//更新与 Old 相联的所有已经走过的结点的评估值
void CAStar::PropagateDown(CAStar::NODE* Old)
{
	//FUNCTION_BEGIN;

	int c,g;
	NODE* Child,*Father;
	g=Old->g;

	int cChildNum = (m_eMode == eFindPathMode1 ? 16 : 8);
	for( c=0;c < cChildNum; c++ )
	{
		if((Child = Old->Child[c]) == NULL)
			break;

		int new_g = g+1;

		if(new_g < Child->g)
		{
			Child->g = new_g;
			Child->f = Child->g+Child->h;
			Child->Parent = Old;

			// 把这一层的节点压入栈中(临时的)
			Push(m_Stack,Child);
		}
	}
	
	// 更新每一层的估价值
	while( !m_Stack.empty() )
	{
		Father = Pop(m_Stack);

		int cChildNum = (m_eMode == eFindPathMode1 ? 16 : 8);
		for( c=0; c < cChildNum; c++ )
		{
			if((Child = Father->Child[c]) == NULL)
				break;
			
			int new_g = Father->g+1;

			if(new_g < Child->g)
			{
				Child->g = new_g;
				Child->f = Child->g+Child->h;
				Child->Parent = Father;
				// 把这一层的节点压入栈中(临时的)
				Push(m_Stack,Child);
			}
		}
	}

	//FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param p : 描述
 * \param Node : 描述
 * \return 返回值的描述
 */
// 把结点 Node 压入栈中
void CAStar::Push(STACK & p,CAStar::NODE * Node)
{
	//FUNCTION_BEGIN;

	p.push_back(Node);

	//FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param p : 描述
 * \return 返回值的描述
 */
// 从栈顶弹出结点
CAStar::NODE* CAStar::Pop(STACK & p)
{
	//FUNCTION_BEGIN;

	NODE* Node = p.back();
	STACK::iterator it =p.end();
	--it;
	p.erase(it);
	return Node;

	//FUNCTION_END;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param Successor : 描述
 * \return 返回值的描述
 */
//把Successor插入开放表中,并排序
void CAStar::InsertToOpen(CAStar::NODE* Successor)
{
	//FUNCTION_BEGIN;

	m_Open.push_back(Successor);
	std::push_heap(m_Open.begin(),m_Open.end());
	m_mapOpen[Successor->NodeNum] = Successor;

	/*LIST* tmp = (LIST*)m_fn_malloc(sizeof(LIST));
	tmp->NodePtr = Successor;
	tmp->NextPtr = NULL;

	int f;

	f=Successor->f;

	LIST* prev,*cur;

	prev=NULL;
	cur=m_Open;

	while((cur != NULL) && (cur->NodePtr->f < f))
	{
		prev=cur;
		cur=cur->NextPtr;
	}

	tmp->NextPtr = cur;

	if(prev)
		prev->NextPtr = tmp;
	else
		m_Open = tmp;*/

	//FUNCTION_END;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param tilenum : 描述
 * \return 返回值的描述
 */
//从关闭表中找指定结点
CAStar::NODE* CAStar::CheckCLOSE(int tilenum)
{
	//FUNCTION_BEGIN;

	tMapNodes::iterator it = m_mapClose.find(tilenum);
	if(it == m_mapClose.end())
		return NULL;
	return it->second;

	/*LIST* tmp;
	tmp=m_Close;
	while(tmp!=NULL)
	{
		if(tmp->NodePtr->NodeNum == tilenum)
			return tmp->NodePtr;
		tmp=tmp->NextPtr;
	}
	return NULL;*/

	//FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param tilenum : 描述
 * \return 返回值的描述
 */
//从开放表中找指定结点
CAStar::NODE* CAStar::CheckOPEN(int tilenum)
{
	//FUNCTION_BEGIN;
	
	tMapNodes::iterator it = m_mapOpen.find(tilenum);
	if(it == m_mapOpen.end())
		return NULL;
	return it->second;

	/*LIST* tmp;
	tmp=m_Open;
	while(tmp!=NULL)
	{
		if(tmp->NodePtr->NodeNum == tilenum)
			return tmp->NodePtr;
		tmp=tmp->NextPtr;
	}
	return NULL;*/

	//FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CAStar::FreeNodes(void)
{
	//FUNCTION_BEGIN;

	for(std::vector<OPEN_NODE>::iterator it = m_Open.begin(); it != m_Open.end(); ++it){
		m_fn_free(it->nod);
	}
	m_Open.resize(0);
	m_mapOpen.clear();

	for(tMapNodes::iterator it = m_mapClose.begin(); it != m_mapClose.end(); ++it){
		m_fn_free(it->second);
	}
	m_mapClose.clear();

	/*LIST *Node,*OldNode;

	Node = m_Open;
	while ( Node != NULL )
	{
		OldNode = Node;
		Node = Node->NextPtr;
		m_fn_free(OldNode->NodePtr);
		m_fn_free(OldNode);
	}
	m_Open=NULL;

	Node = m_Close;
	while ( Node != NULL )
	{
		OldNode = Node;
		Node = Node->NextPtr;
		m_fn_free(OldNode->NodePtr);
		m_fn_free(OldNode);
	}
	m_Close=NULL;*/

	//FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CAStar::FreePath(void)
{
	//FUNCTION_BEGIN;

	m_Stack.resize(0);
	
	while(!m_Path.empty()) 
		m_Path.pop();
	m_TmpPath.resize(0);
	m_PathPreNode.x = -1;
	m_PathPreNode.y = -1;

	//FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param node : 描述
 * \return 返回值的描述
 */
void CAStar::InsertToPath1(stPointI & node)
{
	//FUNCTION_BEGIN;

	m_Path.push(node);

	//FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param node : 描述
 * \return 返回值的描述
 */
void CAStar::InsertToPath(stPointI & node)
{
	//FUNCTION_BEGIN;

	///*
	if(m_Path.empty())
	{
		m_Path.push(node);
		return;
	}

	
	if(m_TmpPath.empty())
	{
		m_TmpPath.push_back(node);
		return;
	}

	stPointI & begin = m_Path.back();

	stPointI & end = m_TmpPath.back();

	if(IsLine(begin.x,begin.y,end.x,end.y,node.x,node.y))
	{
		if(m_TmpPath.size() == m_iStep-1)
		{
			m_TmpPath.resize(0);
			m_Path.push(node);
		}
		else
		{
			m_TmpPath.push_back(node);
		}
		return;
	}

	for( size_t i=0;i<m_TmpPath.size();i++ )
	{
		m_Path.push(m_TmpPath[i]);
	}

	m_TmpPath.resize(0);
	m_Path.push(node);
	//*/

	//FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CAStar::EndPath()
{
	//FUNCTION_BEGIN;

	///*
	for(size_t i=0;i<m_TmpPath.size();i++)
	{
		m_Path.push(m_TmpPath[i]);
	}
	m_TmpPath.resize(0);
	//*/

	//FUNCTION_END;
}

void CAStar::PrintfPath()
{
	for(size_t i=0; i<m_Path.c.size();++i){
		Engine_WriteLogF("PathNode(%d,%d)\n",m_Path.c[i].x,m_Path.c[i].y);
	}
}