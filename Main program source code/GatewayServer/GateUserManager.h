/**
 * \file
 * \version  $Id: GateUserManager.h  $
 * \author  
 * \date
 * \brief 定义网关用户管理类
 */

#ifndef _GATEUSERMANAGER_H_
#define _GATEUSERMANAGER_H_
#include "GateUser.h"
#include "zUserManager.h"
#include "zUniqueID.h"
/**
 * \brief GateUser以帐号ID为key值的指针容器，需要继承使用
 */
class GateUserAccountID:protected LimitHash<DWORD,GateUser *>
{
	protected:

		/**
		 * \brief 将GateUser加入容器中
		 * \param e 要加入的GateUser
		 * \return 成功返回true,否则返回false
		 */
		bool push(GateUser * &e)
		{
			GateUser *account;
			return (!find(e->accid,account) && insert(e->accid,e));
		}

	public:
		GateUserAccountID() {}
		virtual ~GateUserAccountID() {}
		/**
		 * \brief 通过帐号ID得到GateUser
		 * \param accid 要得到GateUser的帐号ID
		 * \return 返回GateUser指针,未找到返回NULL
		 */
		virtual GateUser * getUserByAccID(DWORD accid) =0;
		/**
		 * \brief 通过帐号ID删除GateUser，仅从帐号容器中移除
		 * \param accid 要删除的GateUser的帐号ID
		 */
		virtual void removeUserOnlyByAccID(DWORD accid) =0;
		/**
		 * \brief 通过帐号ID添加GateUser，仅添加到帐号容器中
		 * \param user 要添加的GateUser
		 */
		virtual bool addUserOnlyByAccID(GateUser *user) =0;
};

/**
 * \brief 网关用户管理器
 *
 */
class GateUserManager:public zUserManager,protected GateUserAccountID
{
	private:
		bool inited;
		zUniqueDWORDID *userUniqeID;
		static GateUserManager *gum;

		GateUserManager();
		~GateUserManager();
		bool getUniqeID(DWORD &tempid);
		void putUniqeID(const DWORD &tempid);
	public:
		static GateUserManager *getInstance();
		static void delInstance();
		bool init();
		void final();
		GateUser * getUserByAccID(DWORD accid);
		void removeUserOnlyByAccID(DWORD accid);
		bool addUserOnlyByAccID(GateUser *user);
		void removeUserBySceneClient(SceneClient *scene);
		void removeAllUser();
		bool addCountryUser(GateUser *user);
		void removeCountryUser(GateUser *user);
		template <class YourNpcEntry>
		void execAllOfCountry(const DWORD country,execEntry<YourNpcEntry> &callback);
		void sendCmdToCountry(const DWORD country , const void *pstrCmd, const unsigned int nCmdLen);
	private:
		typedef std::set<GateUser *> GateUser_SET;
		typedef GateUser_SET::iterator GateUser_SET_iter;
		typedef __gnu_cxx::hash_map<DWORD, GateUser_SET> CountryUserMap;
		typedef CountryUserMap::iterator CountryUserMap_iter;
		CountryUserMap countryindex;
};
/* 
class RecycleUserManager:public zUserManager
{
	private:
		RecycleUserManager()
		{
		}
		~RecycleUserManager()
		{
		}
	public:
		static RecycleUserManager *getInstance();
		static void delInstance();
	private:
		static RecycleUserManager *instance;
};
// */
#endif
