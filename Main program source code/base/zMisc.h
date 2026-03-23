/**
 * \file
 * \version  $Id: zMisc.h  $
 * \author  
 * \date 
 * \brief 封装一些常用函数
 *
 * 
 */

#ifndef _ZMISC_H_
#define _ZMISC_H_

#include <string>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <ctime>
#include <vector>
#include <queue>
#include <dirent.h>
#include <unistd.h>
#include "zType.h"
#include "zMutex.h"

struct odds_t
{
	unsigned int upNum;
	unsigned int downNum;
};

enum ServerType {
	UNKNOWNSERVER	=	0,		/**< 未知服务器类型 */
	SUPERSERVER	=	1,		/**< 服务器管理器 */
	LOGINSERVER	=	10,		/**< 登陆服务器 */
	RECORDSERVER	=	11,		/**< 档案服务器 */
	BILLSERVER	=	12,		/**< 计费服务器 */
	SESSIONSERVER	=	20,		/**< Session服务器 */
	SCENESSERVER	=	21,		/**< 场景服务器 */
	GATEWAYSERVER	=	22,		/**< 网关服务器 */
	MINISERVER	=	23,		/**< 小游戏服务器 */
	MAX_SERVERTYPE				/**< 服务器类型的最大编号 */
};

namespace Misc
{
	template <typename V>
	class Parse
	{
	public:
		V* operator () (const std::string& down, const std::string& separator_down)
		{
			std::string::size_type pos = 0;
			if  ( (pos = down.find(separator_down)) != std::string::npos ) {
				
				std::string first_element = down.substr(0, pos);
				std::string second_element = down.substr(pos+separator_down.length());
				return new V(first_element, second_element);
			}
			
			return NULL;
		}
	};
	
	template <typename V>
	class Parse3
	{
	public:
		V* operator () (const std::string& down, const std::string& separator_down)
		{
			std::string::size_type pos = 0;
			if  ( (pos = down.find(separator_down)) != std::string::npos ) {
				
				std::string first_element = down.substr(0, pos);
				std::string::size_type npos = 0;
				if ( (npos = down.find(separator_down, pos+separator_down.length())) != std::string::npos) {
					std::string second_element = down.substr(pos+separator_down.length(), npos-pos);
					std::string third_element = down.substr(npos+separator_down.length());
					return new V(first_element, second_element, third_element);
				}
			}
			
			return NULL;
		}
	};

	/**
	 * \brief  分隔由二级分隔符分隔的字符串
	 * \param list 待分隔的字符串
	 * \param dest 存储分隔结果，必须满足特定的语义要求
	 * \param separator_up 一级分隔符
	 * \param separator_down 二级分隔符		 
	 * \author liqingyu 
	 */
	template <template <typename> class P = Parse>
	class Split
	{
	public:

		template <typename T>
		void operator() (const std::string& list, T& dest, const std::string& separator_up = ";", const std::string& separator_down = ",")
		{	
			typedef typename T::value_type value_type;
			typedef typename T::pointer pointer;

			std::string::size_type lpos = 0;
			std::string::size_type pos = 0;
			P<value_type> p;
			
			
			while ( ( lpos = list.find(separator_up, pos)) != std::string::npos) {
				/*
				std::string down = list.substr(pos, lpos - pos);
				std::string::size_type dpos = 0;
				if  ( (dpos = down.find(separator_down)) != std::string::npos ) {
					
					std::string first_element = down.substr(0, dpos);
					std::string second_element = down.substr(dpos+separator_down.length());
					dest.push_back(typename T::value_type(first_element, second_element));
				}
				pos = lpos+1;
				*/
				std::string down = list.substr(pos, lpos - pos);
				pointer v = p(down, separator_down);
				if (v) {
					dest.push_back(*v);
					SAFE_DELETE(v);
				}
				pos = lpos+1;
			}

			std::string down = list.substr(pos, lpos - pos);
			pointer v = p(down, separator_down);
			if (v) {
				dest.push_back(*v);
				SAFE_DELETE(v);
			}
		}
	};
}

namespace Zebra
{
	// rand number generator's seed
	extern __thread unsigned int seedp;
};

class zMisc
{
	public:	
		//从字符串中查找第pos(从零开始)个数字，如果未找到返回defValue
		template <typename T>
		static WORD getAllNum(const char *s,std::vector<T> & data)
		{
			size_t i;
			int count = 0;
			if (s == NULL) return count;
			bool preIsD = false;
			for (i = 0; i < strlen(s); i++)
			{
				if (isdigit(*(s + i)))
				{
					if (!preIsD)
					{
						count++;
						data.push_back(atoi(s+i));
					}
					preIsD = true;
				}
				else
					preIsD = false;
			}
			return count;
		}

		//随机产生min~max之间的数字，包裹min和max
		static int randBetween(int min,int max)
		{
			if(min==max)
				return min;
			else if (min > max)
				return max + (int) (((double) min - (double)max + 1.0) * rand_r(&Zebra::seedp) / (RAND_MAX + 1.0));
			else
				return min + (int) (((double) max - (double)min + 1.0) * rand_r(&Zebra::seedp) / (RAND_MAX + 1.0));
		}

		//获取几分之的几率
		static bool selectByOdds(const unsigned int upNum, const unsigned int downNum)
		{
			unsigned int m_rand;
			if (downNum < 1) return false;
			if (upNum < 1) return false;
			if (upNum > downNum - 1) return true;
			m_rand = 1 + (unsigned int) ((double)downNum * rand_r(&Zebra::seedp) / (RAND_MAX + 1.0));
			return (m_rand <= upNum);
		}

		//获取几分之几的几率
		static bool selectByt_Odds(const odds_t &odds)
		{
			return selectByOdds(odds.upNum, odds.downNum);
		}

		//获取百分之的几率
		static bool selectByPercent(const unsigned int percent)
		{
			return selectByOdds(percent, 100);
		}

		//获取万分之的几率
		static bool selectByTenTh(const unsigned int tenth)
		{
			return selectByOdds(tenth, 10000);
		}

		//获取十万分之的几率
		static bool selectByLakh(const unsigned int lakh)
		{
			return selectByOdds(lakh, 100000);
		}

		//soke 获取百万分之的几率
		static bool selectByMilh(const unsigned int lakh)
		{
			return selectByOdds(lakh, 2000000);
		}

		//获取亿分之之的几率
		static bool selectByOneHM(const unsigned int lakh)
		{
			return selectByOdds(lakh, 100000000);
		}

		//获取当前时间字符串，需要给定格式
		static void getCurrentTimeString(char *buffer, const int bufferlen, const char *format)
		{
			time_t now;
			struct tm tmnow;
			time(&now);
			localtime_r(&now, &tmnow);
			strftime(buffer, bufferlen, format, &tmnow);
		}

		static char *getTimeString(time_t t,char *buffer, const int bufferlen)
		{
			return getTimeString(t,buffer,bufferlen,"%C/%m/%d %T");
		}

		static char *getTimeString(time_t t,char *buffer, const int bufferlen, const char *format)
		{
			struct tm tmnow;
			localtime_r(&t, &tmnow);
			strftime(buffer, bufferlen, format, &tmnow);
			return buffer;
		}
		
		/**
		 * \brief 删除目录树
		 * \author fqnewman
		 */
		static bool rmDirTree(std::string path)
		{
			struct dirent *record;
			DIR* tDir = opendir(path.c_str());
			if (tDir == NULL) return false;

			while(NULL != (record = readdir(tDir)))
			{
				if (record->d_type == DT_DIR)
				{
					if (strcmp(record->d_name,".") == 0 ||	strcmp(record->d_name,"..") == 0) continue;

					if (!rmDirTree((path+"/"+record->d_name).c_str())) return false;
				}
				else
				{
					if (unlink((path+"/"+record->d_name).c_str()) != 0) return false;
				}
		}
		closedir(tDir);
		if (rmdir(path.c_str())!=0)return false;
		return true;
	}
};

template <typename T>
struct singleton_default
{
	private:
		singleton_default();

	public:
		typedef T object_type;

		static object_type & instance()
		{
			return obj;
		}

		static object_type obj;
};
template <typename T>
typename singleton_default<T>::object_type singleton_default<T>::obj;

//手动调用构造函数，不分配内存
	template<class _T1> 
inline	void constructInPlace(_T1  *_Ptr)
{
	new (static_cast<void*>(_Ptr)) _T1();
}
/// 声明变长指令
#define BUFFER_CMD(cmd,name,len) char buffer##name[len];\
				cmd *name=(cmd *)buffer##name;constructInPlace(name);

#include <ext/pool_allocator.h>
#include <ext/mt_allocator.h>
typedef std::pair<unsigned int , unsigned char *> CmdPair;
template <int QueueSize=102400>
class MsgQueue
{
	public:
		MsgQueue()
		{
			queueRead=0;
			queueWrite=0;
		}
		~MsgQueue()
		{
			clear();
		}
		typedef std::pair<volatile bool , CmdPair > CmdQueue;
		CmdPair *get()
		{
			CmdPair *ret=NULL;
			if(cmdQueue[queueRead].first)
			{
				ret=&cmdQueue[queueRead].second;
			}
			return ret;
		}
		void erase()
		{
				//SAFE_DELETE_VEC(cmdQueue[queueRead].second.second);
				__mt_alloc.deallocate(cmdQueue[queueRead].second.second, cmdQueue[queueRead].second.first);
				cmdQueue[queueRead].first=false;
				queueRead = (++queueRead)%QueueSize;
		}
		bool put(const void *ptNullCmd, const unsigned int cmdLen)
		{
			//unsigned char *buf = new unsigned char[cmdLen];
			unsigned char *buf = __mt_alloc.allocate(cmdLen);
			if(buf)
			{
				bcopy(ptNullCmd , buf , cmdLen);
				if(!putQueueToArray() && !cmdQueue[queueWrite].first)
				{
					cmdQueue[queueWrite].second.first = cmdLen;
					cmdQueue[queueWrite].second.second = buf;
					cmdQueue[queueWrite].first=true;
					queueWrite = (++queueWrite)%QueueSize;
					return true;
				}
				else
				{
					queueCmd.push(std::make_pair(cmdLen , buf));
				}
				return true;
			}
			return false;

		}
	private:
		void clear()
		{
			while(putQueueToArray())
			{
				while(get())
				{
					erase();
				}
			}
			while(get())
			{
				erase();
			}
		}
		bool putQueueToArray()
		{
			bool isLeft=false;
			while(!queueCmd.empty())
			{
				if(!cmdQueue[queueWrite].first)
				{
					cmdQueue[queueWrite].second = queueCmd.front();
					cmdQueue[queueWrite].first=true;
					queueWrite = (++queueWrite)%QueueSize;
					queueCmd.pop();
				}
				else
				{
					isLeft = true; 
					break;
				}
			}
			return isLeft;
		}
		__gnu_cxx::__mt_alloc<unsigned char> __mt_alloc;
		CmdQueue cmdQueue[QueueSize];
#ifdef _POOL_ALLOC_		
		std::queue<CmdPair, std::deque<CmdPair, __gnu_cxx::__pool_alloc<CmdPair> > > queueCmd;
#else		
		std::queue<CmdPair, std::deque<CmdPair> > queueCmd;
#endif		
		unsigned int queueWrite;
		unsigned int queueRead;
};
template < typename T>
T **make_2d_array(const int x,const int y , T *value=NULL,const int width=0)
{
	T **t;
	t = new T*[x];
	if(value)
	{
		for(int i=0 ; i < x ; i++)
		{
			t[i] = new T[y];
			for(int j=0 ; j < y ; j++)
			{
				t[i][j]=value[j*width + i];
			}
		}
	}
	else
	{
		for(int i=0 ; i < x ; i++)
		{
			*t = new T[y];
		}
	}
	return t;
}
template < typename T>
void free_2d_array(T **&t ,const int x,const int y)
{
	for(int i=0 ; i < x ; i++)
	{
			SAFE_DELETE_VEC(t[i]);
	}
	SAFE_DELETE_VEC(t);
}


#endif

