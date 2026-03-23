/**
 * \file
 * \version  $Id: SceneNpcManager.h  $
 * \author  
 * \date 
 * \brief Npc管理容器
 *
 * 
 */

#ifndef _SceneNpcManager_h_
#define _SceneNpcManager_h_

#include "SceneNpc.h"
#include "Chat.h"
#include "TimeTick.h"
#include "zEntryManager.h"

/**
 * \brief 对每个特殊npc执行的回调
 *
 */
struct specialNpcCallBack
{
	public:
		virtual bool exec(SceneNpc *npc)=0;
		virtual ~specialNpcCallBack(){};
};

/**
 * \brief npc管理器
 *
 */
class SceneNpcManager : public zEntryManager< zEntryTempID >
{
	public: 

		bool init();
		bool addSceneNpc(SceneNpc *sceneNpc);
		bool addSpecialNpc(SceneNpc *, bool=false);
		void removeSceneNpc(SceneNpc *sceneNpc);

		SceneNpc *getNpcByTempID(DWORD tempid);
		static SceneNpcManager &getMe();
		static void destroyMe();
		/**
		 * \brief 对每个npc执行回调函数
		 *
		 *
		 * \param exec 回调函数
		 * \return 是否继续执行
		 */
		template <class YourNpcEntry>
		bool execEveryNpc(execEntry<YourNpcEntry> &exec)
		{
			rwlock.rdlock();
			bool ret=execEveryEntry<>(exec);
			rwlock.unlock();
			return ret;
		}

		/**
		 * \brief 删除符合条件的npc
		 *
		 * \param pred 判断条件
		 */
		template <class YourNpcEntry>
		void removeNpc_if(removeEntry_Pred<YourNpcEntry> &pred)
		{
			rwlock.wrlock();
			removeEntry_if<>(pred);
			rwlock.unlock();
		}

		/**
		 * \brief 删除一场景内的npc
		 *
		 *
		 * \param scene 场景
		 */
		void removeNpcInOneScene(Scene *scene);
		void SpecialAI();
		/**
		 * \brief 对每个特殊npc执行回吊调函数
		 *
		 *
		 * \param callback 回调函数
		 * \return 
		 */
		void execAllSpecialNpc(specialNpcCallBack &callback)
		{
			rwlock.wrlock();
			for (MonkeyNpcs::iterator it=specialNpc.begin(); it!=specialNpc.end(); it++)
			{
				if (!callback.exec(*it))
				{
					rwlock.unlock();
					return;
				}
			}
			rwlock.unlock();
		}

		MonkeyNpcs &getSepcialNpc()
		{
			return specialNpc;
		}
		bool getNpcCommonChat(DWORD type, char * content);
		void removeSpecialNpc(SceneNpc *);
		void closeFunctionNpc();
	private:
		///specialNpc是指即使没有玩家在附近也要做处理的NPC
		///包括宠物、boss、任务NPC
		MonkeyNpcs specialNpc;

		SceneNpcManager();
		~SceneNpcManager();

		///SceneNpcManager的唯一实例
		static SceneNpcManager *snm;
		///读写锁
		zRWLock rwlock;

		bool getUniqeID(DWORD &tempid) { return true; }       
		void putUniqeID(const DWORD &tempid) {}

		///npc随机说话的内容
		std::map<DWORD, std::vector<std::string> > NpcCommonChatTable;
		///npc随机说话的概率
		std::map<DWORD, int> NpcCommonChatRate;

		bool loadNpcCommonChatTable();
};

#endif

