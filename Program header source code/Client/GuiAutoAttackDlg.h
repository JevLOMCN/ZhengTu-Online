#pragma once
#include "..\gui\include\guidialog.h"
#include "./GameAppation.h"
/**
 * 自动挂机界面  star100515
 * 
 */

struct sAutoSkill
{
	DWORD dID;
	bool  bUse;
	char  cName[40];
};

class CGuiAutoAttack :
	public CGuiDialog
{
public:
	CGuiAutoAttack();
	~CGuiAutoAttack();

	bool    m_fBegin_State;
	bool    m_fBegin_Attack;
	bool    m_fBegin_Origin;
	bool    m_fBegin_Range;
	bool    m_fAuto_UseMagic;
	bool    m_fAuto_InTeam;
	bool    m_fAuto_PicUp;
	bool    m_fAuto_PicUp_Setup;
	bool	m_fProtection_HP;
	bool	m_fProtection_MP;
	bool	m_fProtection_HP_20have;
	bool	m_fProtection_MP_20have;
//////////star100604
	bool    m_fBegin_DS_Aid;
	bool    m_fAuto_DS_Resume;
	bool    m_fAuto_DS_TeamerResurrection;
	bool    m_fAuto_DS_Stealth;

	int     m_iSitdown_PosX;
	int     m_iSitdown_PosY;
	CGuiEditBox* m_pEditBox_Sitdown_PutX;		
	CGuiEditBox* m_pEditBox_Sitdown_PutY;		

	bool   m_fAniToWait;
	int    m_iAni;
//////////end
	CGuiEditBox* m_pEditBox_PutX;		
	CGuiEditBox* m_pEditBox_PutY;		
	CGuiEditBox* m_pEditBox_Range;			
	CGuiEditBox* m_pEditBox_NeedToProtect_HP;		
	CGuiEditBox* m_pEditBox_NeedToProtect_MP;		
	CGuiListBox* m_pSkillList;		

	int		m_iRange_PosX ;
	int		m_iRange_PosY ;
	int		m_iRange_Range;
	int		m_iNeedToProtect_HP ;
	int		m_iNeedToProtect_MP ;
	int		m_iComboBox_HP;
	int		m_iComboBox_MP;
	WORD   m_dwNeedMedicine_HP[7];
	WORD   m_dwNeedMedicine_MP[6];

	std::vector<sAutoSkill>   m_PassiveSkill;
	void    OnCreate(void);
	//void    OnClose(void);

	bool	OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
    void    OnSetCloseLocation();

	HRESULT OnRender(float fElapsedTime);
	VOID    OnRender_AutoAtt(float fElapsedTime);
	void    SetConfig(stGameConfig config,int index = -1);
	void    SetConfig_Skill(stGameConfig config,int index = -1);

	//sbt
	bool    OnBegin_State();
	bool    OnBegin_Attack();
	bool    OnBegin_AttOrigin();
	bool    OnBegin_AttRange();
	bool    OnAutoUseMagic();
	bool    OnAutoInTeam();
	bool    OnAutoPicUp();
	bool    OnAutoPicUp_Setup();
	bool	OnfProtection_HP();
	bool	OnfProtection_MP();
	bool	OnfProtection_HP_20have();
	bool	OnfProtection_MP_20have();
	bool    OnAddListSkill();
	bool    OnDelListSkill();

	bool	OnSaveButtenCleck();

	bool    OnIsGoaway();
    bool    OnBegin_AttRange_Setup();
	void    OnProtection_Life();

/////////////////star100604
	bool    OnBegin_DS_Aid();
	bool    OnAuto_DS_Resume();
	bool    OnAuto_DS_TeamerResurrection();
	bool    OnAuto_DS_Stealth();

	bool    OnAuto_Sitdown();
	bool    OnAuto_GotoSitdown();
	bool    OnAuto_SetPosSit();
/////////////////end
	bool    OnBegin(bool);//////////star100716
	
private:

	CGuiTab* m_tab;
	
	CGuiImage* m_pImage[2];
};

