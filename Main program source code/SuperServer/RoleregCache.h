/**
 * \file
 * \version  $Id: RoleregCache.h  $
 * \author  
 * \date 
 * \brief 缓冲一些角色名称相关的指令
 */


#ifndef _RoleregCache_h_
#define _RoleregCache_h_

#include <list>

#include "zType.h"
#include "zMutex.h"
#include "zTime.h"
#include "SuperCommand.h"

class RoleregCache
{

	public:

		struct Data
		{
			WORD wdServerID;			/**< 服务器编号 */
			DWORD accid;				/**< 账号编号 */
			char name[MAX_NAMESIZE];	/**< 角色名称 */
			WORD state;					/**< 各种状态的位组合 */

			Data(const Cmd::Super::t_Charname_Gateway &cmd)
			{
				wdServerID = cmd.wdServerID;
				accid = cmd.accid;
				strcpy(name, cmd.name);
				state = cmd.state;
			}

			Data(const Data &data)
			{
				wdServerID = data.wdServerID;
				accid = data.accid;
				strcpy(name, data.name);
				state = data.state;
			}

			Data &operator=(const Data &data)
			{
				wdServerID = data.wdServerID;
				accid = data.accid;
				strcpy(name, data.name);
				state = data.state;
				return *this;
			}
		} __attribute__ ((packed));

		~RoleregCache() {};

		static RoleregCache &getInstance()
		{
			if (NULL == instance)
				instance = new RoleregCache();

			return *instance;
		}

		static void delInstance()
		{
			SAFE_DELETE(instance);
		}

		void add(const Cmd::Super::t_Charname_Gateway &cmd);
		void timeAction(const zTime &ct);

	private:

		RoleregCache() {};

		static RoleregCache *instance;

		zTime actionTimer;

		typedef std::list<Data> DataCache;
		zMutex mlock;
		DataCache datas;

};

#endif

