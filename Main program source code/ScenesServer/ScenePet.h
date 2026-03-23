/**
 * \file
 * \version  $Id: ScenePet.h  $
 * \author 
 * \date 2005年07月22日 17时45分50秒 CST
 * \brief 宠物类定义
 *
 * 
 */

#ifndef _SCENE_PET_H_
#define _SCENE_PET_H_

#include "SceneNpc.h"

struct petBonus
{
	DWORD type;
	WORD atkB;
	WORD defB;
	WORD hpB;

	/*
	petBonus()
	{
		type = 0;
		atkB = 0;
		defB = 0;
		hpB = 0;
	}
	petBonus(DWORD p, WORD a, WORD d, WORD h)
	{
		type = p;
		atkB = a;
		defB = d;
		hpB = h;
	}
	petBonus(petBonus & pb)
	{
		type = pb.type;
		atkB = pb.atkB;
		defB = pb.defB;
		hpB = pb.hpB;
	}
	*/
};

class ScenePet : public SceneNpc
{
	private:
		/*
		   static std::map<DWORD,petBonus> bonusTable;
	*/

	///主人
	//SceneEntryPk * master;
	DWORD masterID;
	DWORD masterType;

	DWORD delCount;

	///宠物类型
	Cmd::petType type;
	///宠物的行动模式
	//WORD petAI;

	///等级
	//DWORD level;

	///是否因为离主人太远而提升了速度
	bool speedUpOffMaster;
	int isUserMasterEnemy(SceneEntryPk *);
public:

	//bool needSave;
	ScenePet(Scene* scene,zNpcB *npc, const t_NpcDefine *define, const SceneNpcType type, const SceneEntryType entrytype=SceneEntry_NPC, zNpcB *abase=NULL);

	Cmd::t_PetData petData;

	void setPetAI(Cmd::petAIMode);
	WORD getPetAI();
	int isEnemy(SceneEntryPk *, bool = false, bool good = false);
	SceneEntryPk * chooseEnemy(SceneEntryPk_vec &);
	bool isRedNamed(bool allRedMode=true);
	void returnToRegion();
	void full_PetDataStruct(Cmd::t_PetData &);
	virtual bool moveToMaster();
	bool randomMove();
	bool isActive();
	bool masterIsAlive;
	virtual bool canMove();
	bool canFight();
	bool runOffEnemy(SceneEntryPk_vec&);

	bool checkMasterTarget(SceneEntryPk *&entry);
	void setMaster(SceneEntryPk *);
	void setMaster(DWORD, DWORD);
	SceneEntryPk * getMaster();
	DWORD getMasterID() const{return masterID;}
	SceneEntryPk * getTopMaster();
	void clearMaster();

	Cmd::petType getPetType();
	void setPetType(Cmd::petType);
	void petDeath();
	virtual void sendData();
	virtual void sendHpExp();
	virtual DWORD getLevel() const;
	virtual bool addExp(DWORD);
	void addPetExp(DWORD);

	bool isPkZone(SceneEntryPk *other=NULL);
	bool recover();

	//给GuardNpc继承用
	virtual void reset(){}
	virtual void check(){};
	virtual void on_death(SceneEntryPk* att){}
	virtual void setAppendDamage(DWORD mindamage, DWORD maxdamage);
	DWORD getMasterMana();
	void changeAndRefreshHMS(bool lock=true, bool sendData=true);

	virtual DWORD getMaxHP();
	virtual DWORD getBaseMaxHP();
	virtual DWORD getMinMDamage();
	virtual DWORD getMaxMDamage(); 
	virtual DWORD getMinPDamage();
	virtual DWORD getMaxPDamage();
	virtual DWORD getMinMDefence();
	virtual DWORD getMaxMDefence();
	virtual DWORD getMinPDefence();
	virtual DWORD getMaxPDefence();

	virtual void levelUp();
	void getAbilityByLevel(DWORD);

	virtual bool normalAction(){return SceneNpc::normalAction();}

	void full_t_MapPetData(Cmd::t_MapPetData &data);

	virtual void sendMeToNine();
	virtual void sendPetDataToNine();

	virtual void delMyself();
};

#endif
