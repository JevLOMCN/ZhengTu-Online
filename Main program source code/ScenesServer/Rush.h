/**
 * \file 
 * \version  $Id: Rush.h  $
 * \author  
 * \date 
 * \brief 处理怪物攻城的类，解析脚本并根据脚本控制攻城boss
 *
 * 
 */

#ifndef _RUSH_H_
#define _RUSH_H_
#include "SceneNpc.h"
#include "SceneDefine.h"
#include <vector>

/**
 * \brief 处理怪物攻城的类，解析脚本并根据脚本控制攻城boss
 *
 */
class Rush
{
	private:
		//DWORD summonCount;

		/**
		 * \brief boss 的定义，用于召唤boss
		 * 可以定义多个boss，召唤时随机选一个
		 *
		 */
		struct bossDefine
		{
			///boss编号，对应npc表
			DWORD id;
			///出生位置
			zPos pos;
			///出生范围
			int region;
			///数量
			int num;
			///脚本ID
			DWORD script;
		};
		/**
		 * \brief 仆人定义
		 *  仆人是boss召唤出来的小怪，协助bos攻城
		 *  仆人不受Rush类控制，行为方式与普通怪物相同
		 *  Rush类只保存它们的指针以便在攻城结束后删除
		 *
		 */
		struct servantDefine
		{
			///仆人id，对应npc表
			DWORD id;
			///一次召唤的数量
			int num;
			///召唤的几率
			int rate;
			///召唤间隔
			int interval;
		};
		///存放多个boss，但是只召唤一个
		std::vector<bossDefine> bossVector;
		///存放多个仆人定义
		std::vector<servantDefine> servantVector;
		///召唤出来仆人的指针，用于删除
		std::list<SceneNpc *> servants;

		/**
		 * \brief 动作定义，每个阶段有一个动作
		 *
		 */
		enum rushAction
		{
			///普通
			RUSH_NORMAL,
			///攻城之前的复活阶段
			RUSH_RELIVE,
			///移动
			RUSH_MOVETO,
			///攻击
			RUSH_ATTACK,
			///召唤仆人
			RUSH_SUMMON,
			///召唤跟随boss往城里冲的仆人
			RUSH_SUMMON_RUSH,
			///打开召唤
			RUSH_SUMMON_ON,
			///关闭召唤，关闭后不会再召唤仆人
			RUSH_SUMMON_OFF,
			///清除攻城怪物，包括boss和仆人
			RUSH_CLEAR,
			///给boss加满hp/sp/mp
			RUSH_RECOVER,
			///召唤跟随boss的宠物
			RUSH_SUMMON_PET,
			///世界广播
			RUSH_BROADCAST,
			///攻城结束
			RUSH_END
		};
		/**
		 * \brief 阶段定义，攻城的阶段
		 * Rush类的目的就是控制boss按照攻城阶段一步步进行
		 *
		 */
		struct phaseDefine
		{
			///阶段动作
			rushAction action;
			///位置，在每个阶段意义略不相同
			///移动时表示目的，攻击时表示活动范围中心位置
			zPos pos;
			///范围 移动时表示到达目标的判定距离，攻击时表示活动范围大小
			int region;
			///持续时间 一个阶段的持续时间，到时会触发onPhaseEnd事件
			int lasttime;
			///boss的说话，每阶段开头说话
			char say[MAX_CHATINFO];
			///3个参数
			DWORD x,y,z;
		};
		///存放阶段列表
		std::vector<phaseDefine> phaseVector;
		///当前的阶段索引
		unsigned int curPhase;
		///阶段结束时间
		zRTime nextPhaseTime;
		///攻城结束的时间
		zRTime endTime;
		
		///攻城脚本的编号
		DWORD id;
		///目标国家的编号
		DWORD countryID;

		///攻城boss的id
		DWORD bossID;
		///boss指针
		SceneNpc * boss;

		///下次可召唤的时间
		zRTime summonTime;
		///上次循环时boss的hp
		DWORD lastBossHp;
		///召唤仆人的开关
		bool canSummon;
		///攻城时间结束或者boss死亡后过多久清除所有攻城npc
		int clearDelay;
		///是否结束
		bool end;
		///发起时的提示语
		char text[128];
		std::string startText;
		std::string endText;

		///召唤仆人的范围，位置是以boss的当前位置为中心
		static const int summon_servant_region = 4;
		bool checkSummonTime();
		
		bool loadRushData(DWORD rushID, DWORD rushDelay, DWORD countryID);
		void onPhaseTimeOver();
		void onTimeOver();
		void setEndTime(int delay);
		bool checkEndTime();
		void setPhaseTime(const int delay);
		bool checkPhaseTime();

		void enterNextPhase();
		bool summonBoss();
		bool summonServant();
		bool summonPet();
		void deleteBoss();
		void clearServants();

		rushAction parseAction(const char *);
	public:
		///攻城总持续时间
		int lasttime;
		///多长时间后开始攻城，也就是boss的复活时间
		DWORD rushDelay;
		///本次攻城的名字
		char rushName[MAX_NAMESIZE];
		///目标地图名字
		char mapName[MAX_NAMESIZE];
		///boss的名字
		char bossName[MAX_NAMESIZE];

		Rush(DWORD rushID, DWORD rushDelay, DWORD countryID);
		bool init(Scene * =0);
		~Rush();
		void process();
		bool isEnd();

		void terminate();
};
#endif
