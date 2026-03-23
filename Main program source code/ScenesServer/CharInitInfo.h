/**
 * \file
 * \version  $Id: CharInitInfo.h  $
 * \author 
 * \date
 * \brief 初始角色信息
 *
 * 
 */


#ifndef _CharInitInfo_h_
#define _CharInitInfo_h_

#include <vector>
#include <ext/hash_map>
#include "zType.h"
#include "zRWLock.h"

/**
 * \brief 新手装备结构
 *
 */
struct InitObject
{
	///
	DWORD id;
	char name[MAX_NAMESIZE+1];
	DWORD localeID;
	WORD  num;
	WORD  profession;

	InitObject()
	{
		id = 0;
		bzero(name, sizeof(name));
		localeID = 0;
		num = 0;
		profession = 0;
	}

	/**
	 * \brief 构造函数
	 *
	 *
	 * \param object: 新手装备
	 */
	InitObject(const InitObject &object)
	{
		id = object.id;
		bcopy(object.name, name, sizeof(name));
		localeID = object.localeID;
		num = object.num;
		profession = object.profession;
	}

};

typedef std::vector<InitObject> InitObjectVector;

/**
 * \brief 新手信息
 *
 */
class CharInitInfo
{

	public:

		~CharInitInfo()
		{
			final();
		}

		/**
		 * \brief 得到唯一实例
		 *
		 *
		 * \return 唯一实例
		 */
		static CharInitInfo &getInstance()
		{
			if (NULL == instance)
				instance = new CharInitInfo();

			return *instance;
		}

		/**
		 * \brief 删除唯一实例
		 *
		 *
		 */
		static void delInstance()
		{
			SAFE_DELETE(instance);
		}

		bool init();
		void get(const WORD profession, InitObjectVector &objs);
		void final();

	private:

		static CharInitInfo *instance;

		CharInitInfo() {};

		typedef __gnu_cxx::hash_multimap<WORD, InitObject> ObjectsContainer;
		zRWLock rwlock;
		ObjectsContainer objects;

};

#endif

