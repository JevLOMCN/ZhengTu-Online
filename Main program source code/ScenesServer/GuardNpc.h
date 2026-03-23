/**
 * \file
 * \version	$Id: GuardNpc.h  $
 * \author	
 * \date		2005-06-15
 * \brief		护镖
 * 
 */

#ifndef __GUARD_NPC_H__
#define __GUARD_NPC_H__

#include "ScenePet.h"

#include <map>

class SceneUser;

/**
 * \brief 镖车
 *
 * 该类封装了对特殊NPC镖车的抽象
 *
 */
class GuardNpc : public ScenePet
{
public:	
	GuardNpc(Scene *scene, zNpcB *npc, const t_NpcDefine *define, const SceneNpcType type, const SceneEntryType entrytype = SceneEntry_NPC, zNpcB *abase=NULL);
	
	~GuardNpc();
	
	void owner(SceneUser* user);	
	SceneUser* owner();
	
	void gold(int money);
	int gold() const;
	
	void exp(int experience);
	int exp() const;

	void dest(const zPos& pos);
	void map(const std::string& name);
	
	void reset();
	void check();

	void on_death(SceneEntryPk*);

	//bool masterIsAlive;
	bool canMove();
	bool moveToMaster();
	DWORD save(BYTE * dest);

	bool isSeptGuard;
private:
	void on_reached();
	
	//SceneUser* _owner;
	std::string _name;
	std::string _map;
			
	int _exp;
	DWORD _gold;
	zPos _dest;
	
	int _status;
	
	zRTime _time;	

	DWORD getRobGold();
	//char ownerName[MAX_NAMESIZE];
};

#endif
