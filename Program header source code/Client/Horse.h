#pragma once
#include "pet.h"

class CHorse :
	public CPet
{
public:
	CHorse(void);
	virtual ~CHorse(void);

	void SetAttrib(const t_HorseData& data);
	const t_HorseData* GetHorseProperty() const { if(!bDeleted) return &m_horsedata; return NULL; }
	int   GetHorseLevel() const {if(!bDeleted) return m_horsedata.lv;return 0;}
	horseState GetHorseState() const { return m_horsedata.state; }
	void Follow();
	void Putup();
protected:
	t_HorseData m_horsedata;
};
