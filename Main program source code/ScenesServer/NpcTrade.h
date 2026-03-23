/**
 * \file
 * \version  $Id: NpcTrade.h $
 * \author  
 * \date 
 * \brief Npc买卖对话框
 *
 * 
 */


#ifndef _NpcTrade_h_
#define _NpcTrade_h_

#include "zType.h"
#include "zRWLock.h"
#include "zDatabase.h"

/**
 * \brief NPC买卖
 *
 */
class NpcTrade
{

	public:

		enum
		{
			NPC_BUY_OBJECT		= 1,	///买
			NPC_SELL_OBJECT		= 2,	///卖
			NPC_REPAIR_OBJECT	= 4,	///修理
			NPC_MAKE_OBJECT		= 8,	///打造
			NPC_UPDATE_OBJECT	= 16,	///改造
			NPC_MERGE_OBJECT	= 32, 	///合成
			NPC_ENCHANCE_OBJECT = 64,  //镶嵌
			NPC_MERGE_SOUL_OBJECT = 128,  //魂魄合成
			NPC_HOLE_OBJECT = 256,  //打孔
			NPC_STORE_OBJECT = 512,  //仓库
			NPC_DECOMPOSE_OBJECT = 1024,  //分解
			NPC_RECAST_OBJECT      = 1048,  //随身改造
			NPC_ZSMAKE_OBJECT = 2048,
	        NPC_ZSUPDATE_OBJECT = 4096,
		};

		struct NpcItem
		{
			DWORD id;					///物品编号
			WORD  kind;					///物品类型
			WORD  lowLevel;				///最低等级
			WORD  level;				///最高等级
			WORD  itemlevel;			///购买物品的等级
			WORD  action;				///动作类型
			NpcItem()
			{
				id = 0;
				kind = 0;
				lowLevel = 0;
				level = 0;
				itemlevel = 0;
				action = 0;
			}
		};

		~NpcTrade()
		{
			final();
		}

		/**
		 * \brief 得到唯一实例
		 *
		 *
		 * \return npc买卖系统
		 */
		static NpcTrade &getInstance()
		{
			if (NULL == instance)
				instance = new NpcTrade();

			return *instance;
		}

		/**
		 * \brief 卸载唯一实例
		 *
		 */
		static void delInstance()
		{
			SAFE_DELETE(instance);
		}

		bool init();
		bool getNpcMenu(const DWORD npcid, char *menuTxt);
		bool verifyNpcAction(const DWORD npcid, const NpcItem &item);

	private:

		NpcTrade() {};

		void final();

		static NpcTrade *instance;

		typedef __gnu_cxx::hash_multimap<DWORD, NpcItem> NpcItemMultiMap;

		/**
		 * \brief npc对话框
		 *
		 */
		struct NpcDialog
		{
			DWORD npcid;			///Npc编号
			char menu[6144];		///菜单内容
			NpcItemMultiMap items;	///物品动作
			NpcDialog()
			{
				npcid = 0;
				bzero(menu, sizeof(menu));
			}
			NpcDialog(const NpcDialog& nd)
			{
				npcid = nd.npcid;
				bcopy(nd.menu, menu, sizeof(menu));
				for(NpcItemMultiMap::const_iterator it = nd.items.begin(); it != nd.items.end(); it++)
				{
					items.insert(*it);
				}
			}
		};

		typedef __gnu_cxx::hash_multimap<DWORD, NpcDialog> NpcDialogMultiMap;

		NpcDialogMultiMap dialogs;
		zRWLock rwlock;

};

#endif

