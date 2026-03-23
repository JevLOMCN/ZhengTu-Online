#pragma once
#include "npc.h"


class CNpc;
/**
 * 宠物,马匹是一种特殊的宠物(目前当作道具),马匹的操作不在这里进行
 */
class CPet :public CSceneObject
{
public:
	CPet( petType type );
	CPet();
	virtual ~CPet(void);

	void AutoChangeAI();
	void SetAI(petAIMode ai);
	void RequestData();
	void Rename(const char * szName);
	void DelSkill(int index);//0,1,2,3

	void SetAttrib(t_PetData data);
	void Update(stHpExpPetCmd* cmd);

	const char *GetName() const 
	{
		return m_data.name;
	}

	int GetPetLevel() const { if(!bDeleted) return m_data.lv; return 0; }
	const t_PetData* GetProperty() const { if(!bDeleted) return &m_data; return NULL; }
	petType GetPetType() const { return m_type; }

	void SetPetType(petType type) { m_type = type; }
	petAIMode GetPetAIMode() const { return (petAIMode)(m_data.ai); }

	void SetID(DWORD dwID) { m_dwID = dwID; }
	void SetDeleted( bool b ) { bDeleted = b; }	
	DWORD GetNpcID() const { return m_data.id; } //npcid
	NpcBase_t* GetNpcBase();
	static DWORD GetNameTextColor(petType type,DWORD level);
	DWORD GetNameTextColor();
	bool	bIsDelete() const {return bDeleted;}
	CNpc*	GetNpc();//这个函数可能返回不正确,因为当npc在9屏外就从npc列表中删除了，
	//所以宠物还在但宠物npc却已经被删除了，所以返回空	
protected:	
	t_PetData m_data;		//宠物的数据
	petType   m_type;		//宠物的类型

	bool bDeleted;			
};

inline bool IsSetAi(WORD ai,petAIMode mode)
{
	return ((ai & mode) != 0 );
}

extern const DWORD plusColor;