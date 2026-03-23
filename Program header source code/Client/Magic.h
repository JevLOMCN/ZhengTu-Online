#pragma once
#include "..\magic\sceneobject.h"
#include "../magic/iMagic.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CMagic :
	public CSceneObject
{
	iMagic * m_pMagic;
public:
	CMagic(void);
	~CMagic(void);
	iMagic * GetInterface() { return m_pMagic; }
	void SetInterface(iMagic* pMagic);
	void Release();
	void Run(long time_elasped);
	void Draw(POINT ptMapPixelOffset,DWORD dwColor);
	void Generate(tagListMagicGenerate & listMagic);
	void UpdatePosition();
	bool InScreen();
	POINT GetSortPos();
	stMoveStruct::SortEnum GetSortType(){ return m_pMagic->GetSort(); }
	void AddLight(POINT ptMapOffset);
};
