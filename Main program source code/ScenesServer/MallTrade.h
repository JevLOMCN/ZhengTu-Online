/**
 *  * \file
 *   * \version  $Id: MallTrade.h $
 *    * \author  
 *     * \date 
 *      * \brief 商城随身商店
 *       *
 *        * 
 *         */
//yikey
#include "zType.h"
#include "zRWLock.h"
#include "zDatabase.h"

class MallTrade
{

	public:
		struct	MallItem
		{
			DWORD	id;
			WORD	kind;
			WORD	action;

			MallItem()
			{
				id = 0;
				kind = 0;
				action = 0;
			}
		};

		~MallTrade()
		{
			final();
		}

		static MallTrade &getInstance()
		{
			if(NULL == instance)
				instance = new MallTrade();
			
			return *instance;
		}

		static void delInstance()
		{
			SAFE_DELETE(instance);
		}

		bool init();
		bool verifyMallTrade(const MallItem &mallitem);

	private:
		MallTrade() {};

		static MallTrade *instance;

		typedef __gnu_cxx::hash_multimap<DWORD,MallItem> MallItemMultimap;
		MallItemMultimap	mallitems;
		
		void final();

		zRWLock rwlock;
};
