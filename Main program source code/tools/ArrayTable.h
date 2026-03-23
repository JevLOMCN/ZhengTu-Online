#ifndef _ARRAYTABLE_H_
#define _ARRAYTABLE_H_
#include "zType.h"
#include "zRWLock.h"
#include "zSceneEntry.h"
#include "zProperties.h"
#include <vector>
#include <map>

#include "zXMLParser.h"
#include "zebraclientService.h"



class ArrayTable
{
	public:
		typedef std::vector<std::string> str_vec;
		typedef str_vec::iterator str_vec_iterator;
		typedef str_vec::const_iterator str_vec_const_iterator;
		typedef std::map<DWORD , str_vec> vec_map;
		typedef vec_map::iterator vec_map_iterator;
		typedef vec_map::value_type vec_map_value_type;
		typedef std::vector<DWORD> t_vec;
		typedef t_vec::iterator t_vec_iterator;
		typedef t_vec::const_iterator t_vec_const_iterator;
		typedef t_vec::value_type t_vec_value_type;
		zProperties global;
		~ArrayTable()
		{
			final();
		}
		static ArrayTable *getInstance();
		static void delInstance();
		DWORD getSize()
		{
			return array.size();
		}
		DWORD get(DWORD which);
		bool init(unsigned int type);
		void final()
		{
			array.clear();
		}
		str_vec getOperation(DWORD timer)
		{
			str_vec v;
			vec_map::iterator iter = operation.find(timer);
			if(iter != operation.end())
			{
				v = iter->second;
			}
			return v;
		}
		void clearOperation();
	// */
	private:
		static ArrayTable *instance;
		ArrayTable(){}
		vec_map operation;
		t_vec array;
		zRWLock rwlock;
};



#endif
