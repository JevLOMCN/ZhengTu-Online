/**
 * \file
 * \version  $Id: BillCache.h  $
 * \author  
 * \date 
 * \brief 缓冲请求交易序列号
 * 缓冲请求交易序列，以便确认一个完整的交易事务
 */


#ifndef _BillCache_h_
#define _BillCache_h_

#include <cstdio>
#include <cstdlib>
#include <string>
#include <ext/hash_map>

#include "zType.h"
#include "zTime.h"
#include "zMutex.h"
#include "BillCallback.h"

/**
 * \brief 缺省计费超时时间
 */
#define DEFAULT_BILL_TIMEOUT 60

class BillCache : public SingletonBase<BillCache>
{

	public:

		BillData *add(BillData *bd, const char *gameZone_str);
		BillData *get(const char *tid);
		void remove(const char *tid);
		void update(const zTime &ct);

	private:

		friend class SingletonBase<BillCache>;
		BillCache();
		~BillCache();

		unsigned int SerialNumber;

		struct eqstr
		{
			bool operator()(const char* s1, const char* s2) const
			{
				return strcmp(s1, s2) == 0;
			}
		};

		typedef __gnu_cxx::hash_map<const char *, BillData *, __gnu_cxx::hash<const char *>, eqstr> CacheContainer;
		typedef CacheContainer::iterator iter;
		typedef CacheContainer::const_iterator const_iter;
		typedef CacheContainer::value_type value_type;
		CacheContainer cache;
		zMutex mlock;

};

#endif

