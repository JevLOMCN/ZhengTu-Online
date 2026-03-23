#pragma once
#include "..\gui\include\guidialog.h"

class CPet;
class CNpc;
class CGuiPetCtrlDialog :
	public CGuiDialog
{
public:
	CGuiPetCtrlDialog();
	CGuiPetCtrlDialog(petType type);
	~CGuiPetCtrlDialog(void);

	void OnClose(void);
	void OnCreate();
	void OnResetScreenSize();
	HRESULT OnRender( float fElapsedTime );
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	bool OnBtnClicked(int id);
	bool OnRBtnClicked(int id);

	void UpdateWindowPos();
	void UpdatePetInfo( petType type );
	void OnPetAttackNpc(CNpc* pNpc);

	void OnBeginSelectAttack();
	void OnEndSelectAttack();
	CPet* GetCurrentPet() { return m_pPet; }
private:
	void DrawHp(int hp,int maxhp,int maxhp_plus);
	void DrawName(const char* name,float fElapsedTime,DWORD color = -1);
	void ResetControlBitmap(int group);
private:
	IBitmaps*	m_pBmpHp;
	CPet*		m_pPet;
	CGuiButton* m_pSkillBtn[4];
	CToolTips   m_toolTips;

	float			leftOffset;

	static int m_siPetCtrlNum;
};
