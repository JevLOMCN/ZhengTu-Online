/**
 * \file
 * \version	$Id: LiveSkill.h  $
 * \author	
 * \date	2005-03-09
 * \brief	实现生活技能
 * 
 */
 
#ifndef __LIVE_SKILL_H__
#define __LIVE_SKILL_H__

#include "zType.h"
#include "zNullCmd.h"
#include "zDatabaseManager.h"
#include "Command.h"

#include <strings.h> //for bzero in Command.h
#include <stdlib.h> //for random
#include <assert.h>

class SceneUser;
class zLiveSkillB;
class zObject;

struct LiveSkill
{
public:
	WORD id; //技能标识
	WORD level; //等级
	DWORD point; //等级后值，小点

	enum {
		DEFAULT_START_LEVEL	= 1, //生活技能默认开始等级
		WORKING_TIME = 5, //工作时间
		ADVANCE_WORK_BONUS = 500, //获得物品概率加成
		MIN_NEED_SP = 4, 
		MAX_NEED_SP = 8, 
		ADVANCE_LEVEL = 22, //进阶技能需要等级
		ODDS_BENCHMARK = 10000, 
	};

	enum {
		BASIC_WORK = 1,
		MAKE_WORK = 2,
		ADVANCE_WORK = 3,
	};

	int bonus(SceneUser* user, int points = 0);
	int bonus_items(SceneUser* user, zLiveSkillB* base_skill);
	static int bonus_exp(SceneUser* user, DWORD exp);
	
	static int consume_sp();
	
	const static int odds[];
};

#include <map>

class LiveSkillsManager;

class LiveSkills
{
public:
	LiveSkills();
	~LiveSkills();
		

	std::size_t count()
	{
		return skills.size();
	}
	
	void add(const LiveSkill& skill)
	{
		skills[skill.id] = skill;	
	}
	
	enum {
		START_SKILL_ID = 321,
	};
	
	int state() const
	{
		/*
		int state = skill_id - LiveSkills::START_SKILL_ID + Cmd::LIVE_SKILL_STATE_START + 1;

		//the following case should not be occured, just for safe
		state = (state<0)?0:state;
		state = (state>Cmd::MAX_STATE)?Cmd::MAX_STATE:state;
		*/
		int state = 0;
		switch (skill_id)
		{
			case 321:
				state = Cmd::USTATE_PLANTING;
				break;
			case 324:
				state = Cmd::USTATE_DIGGING;
				break;
			case 327:
				state = Cmd::USTATE_PROSPECTING;
				break;
			case 330:
				state = Cmd::USTATE_CUTTING;
				break;
			case 333:
				state = Cmd::USTATE_GATHERING;
				break;
			case 336:
				state = Cmd::USTATE_FISHING;
				break;
			case 337:
				state = Cmd::USTATE_HUNTING;
				break;
		}			
		
		return state;
	}
	
	//notice, unsafe function, ensure length of buf greater than 6
	int titles(BYTE* buf, int len) const
	{
		assert( len>=6 );
		memset(buf, 0, len);
		
		const_iterator it = skills.begin();
		while (it != skills.end()) {
			switch (it->second.id) 
			{
				case 323: //缝纫
					buf[0] = (BYTE) it->second.level;
					break;					
				case 326: //铁匠
					buf[1] = (BYTE) it->second.level;
					break;					
				case 329: //首饰
					buf[2] = (BYTE) it->second.level;
					break;					
				case 331: //木匠
					buf[3] = (BYTE) it->second.level;
					break;					
				case 335: //炼药
					buf[4] = (BYTE) it->second.level;
					break;					
				case 338: //烹饪
					buf[5] = (BYTE) it->second.level;
					break;					
			}			
			++it;	
		}
		
		return 0;
	}
	
	bool is_upgrade() const
	{
		bool result = true;
		
		switch (skill_id)
		{
			case 333:
				result = false;
				break;
			case 336:
				result = false;
				break;
			case 337:
				result = false;
				break;
		}
		
		return result;
	}
	
private:
	friend class LiveSkillsManager;
	friend class LiveSkill;
	friend class SceneUser;
	friend class EquipMaker;
	typedef std::map<WORD, LiveSkill> skill_t;
	typedef skill_t::iterator iterator;
	typedef skill_t::const_iterator const_iterator;
	
	skill_t skills; //用户当前获得技能
	unsigned long working_time; //用户上次工作时间
	WORD skill_id; //用户上次工作类别
	zObject* tool; //劳动工具
	/**
	 * \brief 依据技能id查找用户是否具有该技能
	 * \return 找到的技能，没有返回NULL
	 */
	LiveSkill* skill(WORD id);
	
};

class zObject;

#include <ext/hash_set>
#include <map>

class LiveSkillsManager
{
public:

	/**
	 * \brief 生活技能相关的消息处理函数
	 * 
	 */
	//static int execute(SceneUser* user, const Cmd::t_NullCmd *command, unsigned int len);

	/**
	 * \brief 劳动状态指令过滤
	 * 
	 */
	static bool command_filter(SceneUser* user, const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen);


	/**
	 * \brief 开始劳动处理函数
	 * 
	 */	
	int execute(SceneUser* user, zObject* ob);

	/**
	 * \brief 更新用户劳动状态
	 * 
	 */
	static bool update(SceneUser* user);
	
	/**
	 * \brief 读入某用户的生活技能信息
	 * 
	 */
	static int load(SceneUser* user, const LiveSkill& live_skill);

	/**
	 * \brief 按照特定格式存储用户生活技能信息
	 * \param user:要存档的用户
	 * \param dest:目标地址
	 * \return 封包后的缓冲区长度
	 */
	static int save(SceneUser* user, unsigned char* dest);	
	
	/**
	 * \brief 通知客户端某用户的生活技能信息，在SceneUser::sendInitToMe被调用
	 * 
	 */
	static void notify(SceneUser* user);

	/**
	 * \brief 实现sigleton模式
	 * 
	 */	
	static LiveSkillsManager& instance();

private:
	static LiveSkillsManager* instance_;
	
	LiveSkillsManager();
	~LiveSkillsManager();

	class Factory : public zDatabaseCallBack<zLiveSkillB> 
	{
	public:	
		typedef __gnu_cxx::hash_set<DWORD> hash_set;
		typedef hash_set::iterator iterator;

		hash_set identifies; //all possiable work tools
		std::map<DWORD, DWORD> skills; //
		
		bool exec(zLiveSkillB* entry)
		{
			//identifies.insert((entry->map_kind << 16) | (entry->weapon_kind ));
			if (entry->weapon_kind) {
				identifies.insert( entry->weapon_kind );
				skills[(entry->map_kind<<16) | entry->weapon_kind] = entry->skill_id;
			}
			return true;
		}
		
	};
	
	Factory factory;
};


#endif
