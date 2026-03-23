#ifndef _CARTOON_H_
#define _CARTOON_H_

#include "ScenePet.h"
#include "SessionCommand.h"

class CartoonPet : public ScenePet
{
	private:
		DWORD cartoonID;
		Cmd::t_CartoonData cartoonData;

		BYTE expRate;//释放经验的速率
		Timer _5_sec;
		DWORD _5_sec_count;

		void releaseExp();
	public:
		void releaseExp(DWORD);

		CartoonPet(Scene *scene, zNpcB *npc, const t_NpcDefine *define, const SceneNpcType type, const SceneEntryType entrytype = SceneEntry_NPC, zNpcB *abase=NULL);

		~CartoonPet(){};

		Cmd::t_CartoonData& getCartoonData();
		void setCartoonData(Cmd::t_CartoonData&);

		void setMaster(SceneEntryPk *);
		void setMaster(DWORD, DWORD);
		void setCartoonID(DWORD);
		DWORD getCartoonID(){return cartoonID;}
		void setExpRate(BYTE);
		void recoverSp(DWORD);
		void setName(char *);

		void putAway(Cmd::Session::saveType saveType);
		void drawExp();
		bool normalAction();
		void sendData();
		void sendHpExp();
		bool addExp(DWORD);
		void levelUp();

		DWORD getLevel() const;
		bool isAdopted();

		void save(Cmd::Session::saveType saveType);
		DWORD save(BYTE * dest);

		void delMyself();
};
#endif
