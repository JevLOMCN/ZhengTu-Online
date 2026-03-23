/**
 * \file
 * \version  $Id: zEntryManager.h  $
 * \author  
 * \date 
 * \brief entry管理器定义文件
 */

#ifndef _ZENTRYMANAGER_H_
#define _ZENTRYMANAGER_H_
#include "zMisc.h"
#include "zEntry.h"
#include <ext/hash_map>
#include "Zebra.h"

/**
 * \brief key值等值比较,目前支持 (DWORD , char *)，两种类型
 */
template <class keyT>
struct my_key_equal : public std::binary_function<keyT, keyT, bool>
{
	inline bool operator()(const keyT s1, const keyT s2) const;
};

/**
 * \brief 模板偏特化
 * 对字符串进行比较
 */
template<>
inline bool my_key_equal<const char *>::operator()(const char * s1, const char * s2) const
{
	return strcmp(s1, s2) == 0;
}

/**
 * \brief 模板偏特化
 * 对整数进行比较
 */
template<>
inline bool my_key_equal<DWORD>::operator()(const DWORD s1, const DWORD s2) const
{
	return s1  == s2;
}

/**
 * \brief 有限桶Hash管理模板,非线程安全
 *
 * 目前支持两种key类型(DWORD , char *),value类型不作限制,但此类型要可copy的。
 * \param keyT key类型(DWORD , char *)
 * \param valueT value类型
 */
template <class keyT,class valueT>
class LimitHash:private zNoncopyable
{
	protected:

		/**
		 * \brief hash_map容器
		 */
		typedef __gnu_cxx::hash_map<keyT, valueT, __gnu_cxx::hash<keyT>, my_key_equal<keyT> > hashmap;
		typedef typename hashmap::iterator iter;
		typedef typename hashmap::const_iterator const_iter;
		hashmap ets;

		/**
		 * \brief 插入数据，如果原来存在相同key值的数据，原来数据将会被替换
		 * \param key key值
		 * \param value 要插入的数据
		 * \return 成功返回true，否则返回false
		 */
		inline bool insert(const keyT &key,valueT &value)
		{
			ets[key]=value;
			return true;
		}

		/**
		 * \brief 根据key值查找并得到数据
		 * \param key 要寻找的key值
		 * \param value 返回结果将放入此处,未找到将不会改变此值
		 * \return 查找到返回true，未找到返回false
		 */
		inline bool find(const keyT &key,valueT &value) const
		{
			const_iter it = ets.find(key);
			if(it != ets.end())
			{
				value = it->second;
				return true;
			}
			else
				return false;
		}

		/**
		 * \brief 查找并得到一个数据
		 * \param value 返回结果将放入此处,未找到将不会改变此值
		 * \return 查找到返回true，未找到返回false
		 */
		inline bool findOne(valueT &value) const
		{
			if(!ets.empty())
			{
				value=ets.begin()->second;
				return true;
			}
			return false;
		}

		/**
		 * \brief 构造函数
		 *
		 */
		LimitHash()
		{
		}

		/**
		 * \brief 析构函数,清除所有数据
		 */
		~LimitHash()
		{
			clear();
		}

		/**
		 * \brief 移除数据
		 * \param key 要移除的key值
		 */
		inline void remove(const keyT &key)
		{
			ets.erase(key);
		}

		/**
		 * \brief 清除所有数据
		 */
		inline void clear()
		{
			ets.clear();
		}

		/**
		 * \brief 统计数据个数
		 */
		inline unsigned int size() const
		{
			return ets.size();
		}

		/**
		 * \brief 判断容器是否为空
		 */
		inline bool empty() const
		{
			return ets.empty();
		}
};

/**
 * \brief 有限桶MultiHash管理模板,非线程安全
 *
 * 目前支持两种key类型(DWORD , char *),value类型不作限制,但此类型要可copy的。
 * \param keyT key类型(DWORD , char *)
 * \param valueT value类型
 */
template <class keyT,class valueT>
class MultiHash:private zNoncopyable
{
	protected:

		/**
		 * \brief hash_multimap容器
		 */
		typedef __gnu_cxx::hash_multimap<keyT, valueT, __gnu_cxx::hash<keyT>, my_key_equal<keyT> > hashmap;
		typedef typename hashmap::iterator iter;
		typedef typename hashmap::const_iterator const_iter;
		hashmap ets;

		/**
		 * \brief 插入数据，如果原来存在相同key值的数据，原来数据将会被替换
		 * \param key key值
		 * \param value 要插入的数据
		 * \return 成功返回true，否则返回false
		 */
		inline bool insert(const keyT &key,valueT &value)
		{
			ets.insert(std::pair<keyT, valueT>(key, value));
			return true;
		}

		/**
		 * \brief 构造函数
		 *
		 */
		MultiHash()
		{
		}

		/**
		 * \brief 析构函数,清除所有数据
		 */
		~MultiHash()
		{
			clear();
		}

		/**
		 * \brief 清除所有数据
		 */
		inline void clear()
		{
			ets.clear();
		}

		/**
		 * \brief 统计数据个数
		 */
		inline unsigned int size() const
		{
			return ets.size();
		}

		/**
		 * \brief 判断容器是否为空
		 */
		inline bool empty() const
		{
			return ets.empty();
		}
};

/**
 * \brief Entry以临时ID为key值的指针容器，需要继承使用
 */
class zEntryTempID:public LimitHash<DWORD,zEntry *>
{
	protected:

		zEntryTempID() {}
		virtual ~zEntryTempID() {}

		/**
		 * \brief 将Entry加入容器中,tempid重复添加失败
		 * \param e 要加入的Entry
		 * \return 成功返回true,否则返回false
		 */
		inline bool push(zEntry * e)
		{
			if(e!=NULL && getUniqeID(e->tempid))
			{
				zEntry *temp;
				if(!find(e->tempid,temp))
				{
					if(insert(e->tempid,e))
						return true;
				}
				putUniqeID(e->tempid);
			}
			return false;
		}

		/**
		 * \brief 移除Entry
		 * \param e 要移除的Entry
		 */
		inline void remove(zEntry * e)
		{
			if(e!=NULL)
			{
				putUniqeID(e->tempid);
				LimitHash<DWORD,zEntry *>::remove(e->tempid);
			}
		}

		/**
		 * \brief 通过临时ID得到Entry
		 * \param tempid 要得到Entry的临时ID
		 * \return 返回Entry指针,未找到返回NULL
		 */
		inline zEntry * getEntryByTempID(const DWORD tempid) const
		{
			zEntry *ret=NULL;
			LimitHash<DWORD,zEntry *>::find(tempid,ret);
			return ret;
		}

		/**
		 * \brief 得到一个临时ID
		 * \param tempid 存放要得到的临时ID
		 * \return 得到返回true,否则返回false
		 */
		virtual bool getUniqeID(DWORD &tempid) =0;
		/**
		 * \brief 放回一个临时ID
		 * \param tempid 要放回的临时ID
		 */
		virtual void putUniqeID(const DWORD &tempid) =0;
};

/**
 * \brief Entry以ID为key值的指针容器，需要继承使用
 */
class zEntryID:public LimitHash<DWORD,zEntry *>
{
	protected:
		/**
		 * \brief 将Entry加入容器中
		 * \param e 要加入的Entry
		 * \return 成功返回true,否则返回false
		 */
		inline bool push(zEntry * &e)
		{
			zEntry *temp;
			if(!find(e->id,temp))
				return insert(e->id,e);
			else
				return false;
		}

		/**
		 * \brief 移除Entry
		 * \param e 要移除的Entry
		 */
		inline void remove(zEntry * e)
		{
			if(e!=NULL)
			{
				LimitHash<DWORD,zEntry *>::remove(e->id);
			}
		}

		/**
		 * \brief 通过ID得到Entry
		 * \param id 要得到Entry的ID
		 * \return 返回Entry指针,未找到返回NULL
		 */
		inline zEntry * getEntryByID(const DWORD id) const
		{
			zEntry *ret=NULL;
			LimitHash<DWORD,zEntry *>::find(id,ret);
			return ret;
		}
};

/**
 * \brief Entry以名字为key值的指针容器，需要继承使用
 */
class zEntryName:public LimitHash<const char *,zEntry *>
{
	protected:
		/**
		 * \brief 将Entry加入容器中,如果容器中有相同key值的添加失败
		 * \param e 要加入的Entry
		 * \return 成功返回true,否则返回false
		 */
		inline bool push(zEntry * &e)
		{
			zEntry *temp;
			if(!find(e->name,temp))
				return insert(e->name,e);
			else
				return false;
		}

		/**
		 * \brief 移除Entry
		 * \param e 要移除的Entry
		 */
		inline void remove(zEntry * e)
		{
			if(e!=NULL)
			{
				LimitHash<const char *,zEntry *>::remove(e->name);
			}
		}
		
		/**
		 * \brief 通过名字得到Entry
		 * \param name 要得到Entry的名字
		 * \return 返回Entry指针,未找到返回NULL
		 */
		inline zEntry * getEntryByName( const char * name) const
		{
			zEntry *ret=NULL;
			LimitHash<const char *,zEntry *>::find(name,ret);
			return ret;
		}

		/**
		 * \brief 通过名字得到Entry
		 * \param name 要得到Entry的名字
		 * \return 返回Entry指针,未找到返回NULL
		 */
		inline zEntry * getEntryByName(const std::string  &name) const
		{
			return getEntryByName(name.c_str());
		}
};

/**
 * \brief Entry以名字为key值的指针容器，需要继承使用
 */
class zMultiEntryName:public MultiHash<const char *,zEntry *>
{
	protected:
		/**
		 * \brief 将Entry加入容器中,如果容器中有相同key值的添加失败
		 * \param e 要加入的Entry
		 * \return 成功返回true,否则返回false
		 */
		inline bool push(zEntry * &e)
		{
			return insert(e->name,e);
		}

		/**
		 * \brief 将Entry从容器中移除
		 * \param e 需要移除的Entry
		 */
		inline void remove(zEntry * &e)
		{
			__gnu_cxx::pair<iter,iter> its = ets.equal_range(e->name);
			for(iter it = its.first; it != its.second; it++)
			{
				if (it->second == e)
				{
					ets.erase(it);
					return;
				}
			}
		}

		/**
		 * \brief 根据key值查找并得到数据
		 * \param name 要寻找的name值
		 * \param e 返回结果将放入此处,未找到将不会改变此值
		 * \param r 如果有多项匹配，是否随机选择
		 * \return 查找到返回true，未找到返回false
		 */
		inline bool find(const char * &name,zEntry * &e,const bool r=false) const
		{
			int rd = ets.count(name);
			if(rd > 0)
			{
				int mrd = 0, j = 0;
				if (r)
					zMisc::randBetween(0, rd - 1);
				__gnu_cxx::pair<const_iter,const_iter> its = ets.equal_range(name);
				for(const_iter it = its.first; it != its.second && j < rd; it++, j++)
				{
					if (mrd == j)
					{
						e = it->second;
						return true;
					}
				}
			}
			return false;
		}

};

template<int i>
class zEntryNone
{
	protected:
		inline bool push(zEntry * &e) { return true; }
		inline void remove(zEntry * &e) { }
		inline void clear(){}
};

/**
 * \brief Entry处理接口,由<code>zEntryManager::execEveryEntry</code>使用
 */
template <class YourEntry>
struct execEntry
{
	virtual bool exec(YourEntry *entry) =0;
	virtual ~execEntry(){}
};

/**
 * \brief Entry删除条件接口,由<code>zEntryManager::removeEntry_if</code>使用
 */
template <class YourEntry>
struct removeEntry_Pred
{
	/**
	 * \brief 被删除的entry存储在这里
	 */
	std::vector<YourEntry *> removed;
	/**
	 * \brief 测试是否要删除的entry,需要实现
	 * \param 要被测试的entry
	 */
	virtual bool isIt(YourEntry *entry) =0;
	/**
	 * \brief 析构函数
	 */
	virtual ~removeEntry_Pred(){}
};

/**
 * \brief Entry管理器接口,用户应该根据不同使用情况继承它
 */

template<typename e1,typename e2=zEntryNone<1>, typename e3=zEntryNone<2> >
class zEntryManager:protected e1,protected e2,protected e3
{
	protected:

		/**
		 * \brief 添加Entry,对于重复索引的Entry添加失败
		 * \param e 被添加的 Entry指针
		 * \return 成功返回true，否则返回false 
		 */
		inline bool addEntry(zEntry * e)
		{
			if(e1::push(e))
			{
				if(e2::push(e))
				{
					if(e3::push(e))
						return true;
					else
					{
						e2::remove(e);
						e1::remove(e);
					}
				}
				else
					e1::remove(e);
			}
			return false;
		}

		/**
		 * \brief 删除Entry
		 * \param e 被删除的Entry指针
		 */
		inline void removeEntry(zEntry * e)
		{
			e1::remove(e);
			e2::remove(e);
			e3::remove(e);
		}

		/**
		 * \brief 虚析构函数
		 */
		~zEntryManager() { };

		/**
		 * \brief 统计管理器中Entry的个数
		 * \return 返回Entry个数
		 */
		inline int size() const
		{
			return e1::size();
		}

		/**
		 * \brief 判断容器是否为空
		 */
		inline bool empty() const
		{
			return e1::empty();
		}

		/**
		 * \brief 清除所有Entry
		 */
		inline void clear()
		{
			e1::clear();
			e2::clear();
			e3::clear();
		}

		/**
		 * \brief 对每个Entry进行处理
		 * 当处理某个Entry返回false时立即打断处理返回
		 * \param eee 处理接口
		 * \return 如果全部执行完毕返回true,否则返回false
		 */
		template <class YourEntry>
		inline bool execEveryEntry(execEntry<YourEntry> &eee)
		{
			typedef typename e1::iter my_iter;
			for(my_iter it=e1::ets.begin();it!=e1::ets.end();it++)
			{
				if(!eee.exec((YourEntry *)it->second))
					return false;
			}
			return true;
		}

		/**
		 * \brief 删除满足条件的Entry
		 * \param pred 测试条件接口
		 */
		template <class YourEntry>
		inline void removeEntry_if(removeEntry_Pred<YourEntry> &pred)
		{
			typedef typename e1::iter my_iter;
			my_iter it=e1::ets.begin();
			while(it!=e1::ets.end())
			{
				if(pred.isIt((YourEntry *)it->second))
				{
					pred.removed.push_back((YourEntry *)it->second);
				}
				it++;
			}

			for(unsigned int i=0;i<pred.removed.size();i++)
			{
				removeEntry(pred.removed[i]);
			}
		}
};
#endif
