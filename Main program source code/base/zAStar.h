/**
 * \file
 * \version  $Id: zAStar.h  $
 * \author  
 * \date 
 * \brief A*寻路算法
 */


#ifndef _zAStar_h_
#define _zAStar_h_

#include <list>
#include <vector>
#include <algorithm>

#include "zSceneEntry.h"

/**
 * \brief A*寻路算法模板
 * 其中step表示步长，radius表示搜索半径
 */
template <int step = 1, int radius = 12>
class zAStar
{

	private:

		/**
		 * \brief 路径坐标点
		 */
		struct zPathPoint
		{
			/**
			 * \brief 坐标
			 */
			zPos pos;
			/**
			 * \brief 当前距离
			 */
			int cc;
			/**
			 * \brief 路径上一个结点指针
			 */
			zPathPoint *father;
		};

		/**
		 * \brief 路径头
		 */
		struct zPathQueue
		{
			/**
			 * \brief 路径节点头指针
			 */
			zPathPoint *node;
			/**
			 * \brief 路径消耗距离
			 */
			int cost;
			/**
			 * \brief 构造函数
			 * \param node 初始化的路径节点头指针
			 * \param cost 当前消耗距离
			 */
			zPathQueue(zPathPoint *node, int cost)
			{
				this->node = node;
				this->cost = cost;
			}
			/**
			 * \brief 拷贝构造函数
			 * \param queue 待拷贝的源数据
			 */
			zPathQueue(const zPathQueue &queue)
			{
				node = queue.node;
				cost = queue.cost;
			}
			/**
			 * \brief 赋值操作符号
			 * \param queue 待赋值的源数据
			 * \return 返回结构的引用
			 */
			zPathQueue & operator= (const zPathQueue &queue)
			{
				node = queue.node;
				cost = queue.cost;
				return *this;
			}
		};

		/**
		 * \brief 定义所有路径的链表
		 */
#ifdef _POOL_ALLOC_		
		typedef std::list<zPathQueue, __gnu_cxx::__pool_alloc<zPathQueue> > zPathQueueHead;
#else
		typedef std::list<zPathQueue> zPathQueueHead;
#endif

		typedef typename zPathQueueHead::iterator iterator;
		typedef typename zPathQueueHead::reference reference;

		/**
		 * \brief 估价函数
		 * \param midPos 中间临时坐标点
		 * \param endPos 最终坐标点
		 * \return 估算出的两点之间的距离
		 */
		int judge(const zPos &midPos, const zPos &endPos)
		{
			int distance = abs(midPos.x - endPos.x) + abs(midPos.y - endPos.y);
			return distance;
		}

		/**
		 * \brief 进入路径队列
		 * \param queueHead 路径队列头
		 * \param pPoint 把路径节点添加到路径中
		 * \param currentCost 路径的估算距离
		 */
		void enter_queue(zPathQueueHead &queueHead, zPathPoint *pPoint, int currentCost)
		{
			zPathQueue pNew(pPoint, currentCost);
			if (!queueHead.empty())
			{
				for(iterator it = queueHead.begin(); it != queueHead.end(); it++)
				{
					//队列按cost由小到大的顺序排列
					if ((*it).cost > currentCost)
					{
						queueHead.insert(it, pNew);
						return;
					}
				}
			}
			queueHead.push_back(pNew);
		}

		/**
		 * \brief 从路径链表中弹出最近距离
		 * \param queueHead 路径队列头
		 * \return 弹出的最近路径
		 */
		zPathPoint *exit_queue(zPathQueueHead &queueHead)
		{
			zPathPoint *ret = NULL;
			if (!queueHead.empty())
			{
				reference ref = queueHead.front();
				ret = ref.node;
				queueHead.pop_front();
			}
			return ret;
		}

	public:

		/**
		 * \brief 寻路过程中判断中间点是否可达目的地
		 *
		 *	return (scene->zPosShortRange(tempPos, destPos, radius)
		 *			&& (!scene->checkBlock(tempPos) //目标点可达，或者是最终目标点
		 *				|| tempPos == destPos));
		 *
		 * \param tempPos 寻路过程的中间点
		 * \param destPos 目的点坐标
		 * \param radius 寻路范围，超出范围的视为目的地不可达
		 * \return 返回是否可到达目的地
		 */
		virtual bool moveable(const zPos &tempPos, const zPos &destPos, const int radius = radius) = 0;
		/**
		 * \brief 物件向某一个方向移动
		 * \param direct 方向
		 * \param step 表示步长
		 * \return 移动是否成功
		 */
		virtual bool move(const int direct, const int step = step) = 0;
		/**
		 * \brief 使物件向某一个点移动
		 * 带寻路算法的移动
		 * \param srcPos 起点坐标
		 * \param destPos 目的地坐标
		 * \return 移动是否成功
		 */
		bool gotoFindPath(const zPos &srcPos, const zPos &destPos);
		/**
		 * \brief Npc向某一个点移动
		 * \param srcPos 起点坐标
		 * \param destPos 目的地坐标
		 * \return 移动是否成功
		 */
		bool goTo(const zPos &srcPos, const zPos &destPos);
		/**
		 * \brief Npc随机向某一个方向移动
		 * \param direct 随机方向
		 * \return 移动是否成功
		 */
		bool shiftMove(const int direct);

};

template<int step, int radius>
bool zAStar<step, radius>::gotoFindPath(const zPos &srcPos, const zPos &destPos)
{
	//DisMap是以destPos为中心的边长为2 * radius + 1 的正方形
	const int width = (2 * radius + 1);
	const int height = (2 * radius + 1);
	const int MaxNum = width * height;
	//把所有路径距离初始化为最大值
	std::vector<int, __gnu_cxx::__pool_alloc<int> > pDisMap(MaxNum, MaxNum);
	std::vector<zPathPoint, __gnu_cxx::__pool_alloc<zPathPoint> > stack(MaxNum * 8 + 1);//在堆栈中分配内存
	zPathQueueHead queueHead;

	//从开始坐标进行计算
	zPathPoint *root = &stack[MaxNum * 8];
	root->pos = srcPos;
	root->cc = 0;
	root->father = NULL;
	enter_queue(queueHead, root, root->cc + judge(root->pos, destPos));

	int Count = 0;
	//无论如何,循环超过MaxNum次则放弃
	while(Count < MaxNum)
	{
		root = exit_queue(queueHead);
		if (NULL == root)
		{
			//目标点不可达
			return false;
		}

		if (root->pos == destPos)
		{
			//找到到达目的地的路径
			break;
		}

		const zAdjust adjust[8] =
		{
			{	1 * step,	0 * step	},
			{	0 * step,	-1 * step	},
			{	0 * step,	1 * step	},
			{	-1 * step,	0 * step	},
			{	1 * step,	-1 * step	},
			{	-1 * step,	-1 * step	},
			{	-1 * step,	1 * step	},
			{	1 * step,	1 * step	}
		};
		for(int i = 0; i < 8; i++)
		{
			//分别对周围8个格点进行计算路径
			bool bCanWalk = true;
			zPos tempPos = root->pos;
			tempPos += adjust[i];

			if (moveable(tempPos, destPos))
			{
				//对路径进行回溯
				zPathPoint *p = root;
				while(p)
				{
					if(p->pos == tempPos)
					{
						//发现坐标点已经在回溯路径中，不能向前走
						bCanWalk = false;
						break;
					}
					p = p->father;
				}

				//如果路径回溯成功，表示这个点是可行走的
				if (bCanWalk)
				{
					int cost = root->cc + 1;
					int index = (tempPos.y - destPos.y + radius) * width + (tempPos.x - destPos.x + radius);
					if (index >= 0
							&& index < MaxNum
							&& cost < pDisMap[index])
					{
						//这条路径比上次计算的路径还要短，需要加入到最短路径队列中
						pDisMap[index] = cost;
						zPathPoint *pNewEntry = &stack[Count * 8 + i];
						pNewEntry->pos = tempPos;
						pNewEntry->cc = cost;
						pNewEntry->father = root;
						enter_queue(queueHead, pNewEntry, pNewEntry->cc + judge(pNewEntry->pos, destPos));
					}
				}
			}
		}

		Count++;
	}

	if (Count < MaxNum)
	{
		//最终路径在PointHead中,但只走一步
		while(root)
		{
			//倒数第二个节点
			if(root->father != NULL
					&& root->father->father == NULL)
			{
				return move(srcPos.getDirect(root->pos));
			}

			root = root->father;
		}
	}

	return false;
}

template<int step, int radius>
inline bool zAStar<step, radius>::goTo(const zPos &srcPos, const zPos &destPos)
{
	int direct = srcPos.getDirect(destPos);

	if (!move(direct)) {
		int r = zMisc::randBetween(0, 1);
		int deep = 0;
		while(deep < 3) {
			switch(r) {
				case 0://顺时针
					direct++;
					break;
				case 1://逆时针
					direct += 7;
					break;
			}
			direct %= 8;
			if (move(direct))
				return true;
			deep++;
		}
	}

	return false;
}

template<int step, int radius>
inline bool zAStar<step, radius>::shiftMove(const int direct)
{
	return move(direct);
}

#endif

