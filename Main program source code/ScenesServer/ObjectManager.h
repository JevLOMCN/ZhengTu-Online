#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_
#include <ext/pool_allocator.h>
#include "zType.h"
#include "Object.h"
#include "zEntryManager.h"
#include "zUniqueID.h"

class zObject;
class SceneUser;

class GlobalObjectIndex:private zEntryManager< zEntryID >
{
	private:
		static GlobalObjectIndex *onlyme;
		zMutex mlock;

		GlobalObjectIndex();
		~GlobalObjectIndex();
	public:
		static GlobalObjectIndex *getInstance();
		static void delInstance();
		void removeObject(DWORD thisid);
		bool addObject(zObject * o);
};

extern GlobalObjectIndex *const goi;

class UserObjectCompare
{
	public:
		virtual bool isIt(zObject * object) =0;
};

class UserObjectExec
{
	public:
		virtual bool exec(zObject * object) =0;
};

class UserObjectM:private zEntryManager< zEntryID >
{
	public:
#ifdef _POOL_ALLOC_		
		typedef std::vector<DWORD, __gnu_cxx::__pool_alloc<DWORD> > ObjID_vec;
		typedef std::vector<zObject *, __gnu_cxx::__pool_alloc<zObject *> > Obj_vec;
#else
		typedef std::vector<DWORD > ObjID_vec;
		typedef std::vector<zObject *> Obj_vec;
#endif

		UserObjectM();
		~UserObjectM();
		zObject * getObjectByThisID( DWORD thisid);
		zObject *getObjectByPos(const stObjectLocation &dst);
		void removeObjectByThisID(DWORD thisid);
		void removeObject(zObject * o);
		bool addObject(zObject * o);
		zObject *getObject(UserObjectCompare &comp);
		void execEvery(UserObjectExec &exec);
		
		DWORD exist(DWORD id, DWORD number, BYTE upgrade = 0, BYTE type = 0 ) const;
		int space(const SceneUser* user) const;

		zObject* getObjectByID(DWORD id, BYTE upgrade = 0, bool not_need_space = false) const;
		int reduceObjectNum(SceneUser* user, DWORD id, DWORD number, zObject*& update_ob, ObjID_vec& del_obs, BYTE upgrade = 0);		
		int addObjectNum(SceneUser* user, DWORD id, DWORD number, zObject* & orig_ob, Obj_vec& new_obs, BYTE upgrade);
		//soke ∞Û∂®ŒÔ∆∑
		int addBindObjectNum(SceneUser* user, DWORD id, DWORD number, zObject* & orig_ob, Obj_vec& new_obs, BYTE upgrade);
		int addBlueObjectNum(SceneUser* user, DWORD id, DWORD number, zObject* & orig_ob, Obj_vec& new_obs, BYTE upgrade);
		int addGoldObjectNum(SceneUser* user, DWORD id, DWORD number, zObject* & orig_ob, Obj_vec& new_obs, BYTE upgrade);
		int addGreenObjectNum(SceneUser* user, DWORD id, DWORD number, zObject* & orig_ob, Obj_vec& new_obs, BYTE upgrade);
		int addPurpleObjectNum(SceneUser* user, DWORD id, DWORD number, zObject* & orig_ob, Obj_vec& new_obs, BYTE upgrade);
};

#endif
