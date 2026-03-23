#pragma once
#include "..\gui\include\guidialog.h"
#include "./MainCharacter.h"
#include "..\gui\include\GuiScrollBar.h"

#define SKILL_TREE_TYPE_NUM	4
#define SKILL_TREE_TYPE_IMG_START	100
#define SKILL_TREE_NAME_IMG_START	40

#define MAX_SKILL_LEVEL	7
#if defined _DEBUG || defined RELEASE_TEST
//soke 修改最高技能等级为200 不然不显示
	#define OPEN_SKILL_MAX_LEVEL	200
//	#define OPEN_SKILL_MAX_LEVEL	150
#else
//soke 修改最高技能等级为200 不然不显示
	#define OPEN_SKILL_MAX_LEVEL	200
//	#define OPEN_SKILL_MAX_LEVEL	120
#endif

/*enum enmSkill
{
	SKILL_RUSH_OUT		= 7,
	SKILL_THREATEN		= 10,
	SKILL_CHANGE_POS	= 220,
	SKILL_CHANGE_FACE	= 290,
	SKILL_ATTACK_FLOWER = 364,
};*/
enum enmSkill
{
	SKILL_RUSH_OUT_FAST		= 17,
	SKILL_THREATEN			= 64,
	SKILL_GAS_ARROW			= 74,
	SKILL_STRENGTH_MULTI_ARROW	= 84,
	SKILL_SHOOT_THROUGH		= 87,
	SKILL_CATCH_SUMMON_YE	= 99,
	SKILL_RELIVE_PET		= 103,
	SKILL_CATCH_SUMMON_LING = 107,
	SKILL_CATCH_SUMMON_MO	= 115,
	SKILL_CATCH_SUMMON_SENG	= 117,
	SKILL_FIRE_WALL			= 153,
	SKILL_STRENGTHEN_SUMMON	= 214,
	SKILL_CHANGE_POS		= 215,
	SKILL_COMBIN_BONE_PET	= 219,
	SKILL_EAT_GHOST			= 224,
	SKILL_EAT_DEATH			= 226,
	SKILL_COMBIN_BONE_GUARD	= 229,
	SKILL_DOOR_TO_DARK_WORLD	= 232,
	SKILL_COMBIN_SIX_GHOST		= 235,
	SKILL_COMBIN_GOD_FIGHTER	= 239,
	SKILL_COMBIN_GOD_LEADER	= 244,
	SKILL_COMBIN_XIAN_PET	= 249,
	SKILL_COMBIN_THREE_DIM	= 255,
	SKILL_COMBIN_STONE_PET	= 259,
	SKILL_COMBIN_WIND_PET	= 264,
	SKILL_COMBIN_IRON_PET	= 269,
	SKILL_COMBIN_FIRE_LEADER	= 275,
	SKILL_RELIVE_FRIEND		= 295,
	SKILL_CRAZY_ACTION		= 320,
	SKILL_DIFFUSE_COUNTRY	= 334,
	SKILL_HIDE_ME			= 341,
	SKILL_RUSH_OUT			= 346,		
	SKILL_CHANGE_FACE		= 350,
	SKILL_ATTACK_FLOWER		= 364,
	SKILL_COMBIN_BOW_ARROW	= 8016,
	SKILL_COMBIN_XIAN_PET_NORMAL_SKILL	= 8023,
	SKILL_COMBIN_WIND_PET_NORMAL_SKILL	= 8024,
	SKILL_EAT_GHOST_NORMAL_SKILL	= 8025,	
	SKILL_COMBIN_SIX_GHOST_NORMAL_SKILL		= 8027,
	SKILL_COMBIN_THREE_DIM_NORMAL_SKILL		= 8028,
	SKILL_COMBIN_FIRE_LEADER_NORMAL_SKILL	= 8029,
	SKILL_EMITE_ARROW		= 1000003,
};

enum enmSkillSerialType{
	SKILL_SERIAL_FIGHTER = 1,
	SKILL_SERIAL_SHOOTER,
	SKILL_SERIAL_MAGIC,
	SKILL_SERIAL_CALLER,
	SKILL_SERIAL_ANGEL,
	SKILL_SERIAL_CIKE,
	SKILL_SERIAL_WEISHI,
	SKILL_SERIAL_QIANG,
	SKILL_SERIAL_SPECIAL,
	SKILL_SERIAL_TFJN,
};

enum enmSkillSpecialType{
	SKILL_SPECIAL_FAST = 1,
	SKILL_SPECIAL_FLOW,
	SKILL_SPECIAL_BOOK,
};

extern const char* GetSkillSerialName(SkillBase_t* pSkill);
extern const char* GetSkillTreeName(SkillBase_t* pSkill);
extern bool IsTeamAttackState(const BYTE& state);
extern bool IsCharacterUseOnly(SkillBase_t* pSkill);

class CGuiSkill : public CGuiDialog
{
	int m_curSerialType;	//1~6
	int m_curTreeType;		//0~3

	bool m_bInitIcon;

	CGuiImage*	m_pTreeName[SKILL_TREE_TYPE_NUM];
	CGuiStatic*	m_pEdtPoints;
	CGuiImage* m_pTreeImg;

	CGuiScrollBar*	m_pScrollBar;

	int m_curTreeTotalPoint;
	int m_curTreeUpItemPoint;
	int m_curSerialTotalPoint;
	bool m_bFirstTimeIn;

	int m_serialTotalPoints[SKILL_SERIAL_TFJN]; //sky
public:

	struct stSkillIcon{
		CGuiButton* pButton;
		CGuiImage*  pImage[2];
		CRoleSkill* pRoleSkill;
		SkillBase_t* pSkillBase;
		char szKey[8];
		int serialType;
		int treeType;
		stSkillIcon() : pRoleSkill(NULL), serialType(-1), treeType(-1) {}
	};

	typedef std::map<DWORD, stSkillIcon> tMapSkillIcon;
	tMapSkillIcon m_mapSkillIcon;

	struct stSkillTree{
		DWORD dwID;
		char strName[MAX_NAMESIZE];
		DWORD dwPoints;
	};

	void SetNeedInitIcon(bool bNeed = true) { m_bInitIcon = bNeed; }
	
	//tree id = (m_curSerialType-1)*SKILL_TREE_TYPE_NUM + m_curTreeType
	typedef std::vector<stSkillTree> t_SkillTree;
	t_SkillTree m_listSkillTree;

	CGuiSkill(void);
	void OnClose(void);
	void OnCreate();
	void InitIcon();
	void InitSkillTreeList();
	void InitCustomElements();
	void OnResetScreenSize();

	HRESULT OnRender(float fElapsedTime);

	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnSetCloseLocation();
	void UpdateRects();

	//
	void RefreshSkillTreeList();
	t_SkillTree& GetSkillTreeList() { RefreshSkillTreeList(); return m_listSkillTree; }

	//
	void SetPoints(int i);
	int GetCurSerial() { return m_curSerialType; }
	void RefreshTreeTypeName();
	void OnMouseWheel();
	void ResetIconPos();
	void ResetTree( bool bResetPos );
	void RefreshCurTreeSkillPoints();
	int GetTreeSkillPoints(const int& serial, const int& tree, bool bIncUpItem = false);
	int GetSerialSkillPoints(const int& serial, const int& tree, bool bIncUpItem = false);
	void RefreshState( bool bResetPos = false );
	int GetCurTreeIndex();
	int GetSkillTreeImgID();
	stSkillIcon* GetMouseOverIcon();
	void ShowLeftSideControl(bool bShow = true);
	int GetCurTreeSkillPoints() { return m_curTreeTotalPoint; }
	int GetCurSerialSkillPoints() { return m_curSerialTotalPoint; }
	void GetNeedPointsDesc(const int& serial, const int& tree, char* strDesc);

	stSkillIcon* GetActiveSkillIcon() { return m_activeSkillIcon; }
	stSkillIcon* GetSkillIconByID(const DWORD& id);
	const char*	GetActiveSkillIconTooltips();
	CToolTips* GetActiveSkillIconTooltipsFull();
	bool GetSkillIconResourceFromID(const DWORD& id, stResourceLocation& rl );
	const char*	GetSkillIconTooltipsFromID(const DWORD& id);
	CToolTips* GetSkillIconTooltipsFullFromID(const DWORD& id);
	DWORD GetActiveSkillIconID();
	bool GetActiveSkillIconResource( stResourceLocation& rl );
	void SetActiveSkillIcon(stSkillIcon* pSkillIcon = NULL, const DWORD& dwID = -1);
	
	void    DrawSkillString(float fElapsedTime);
	void	DrawSkillDisableStatus();

	void InitRefresh();
	
	//Init serial according to weapon in hand
	void InitSerialTotalPoints();
	bool SerialHasAccordingWeapon(int serialType);
	void UpdateCurSerialTypeAccordingToWeaponInHand();
	int UpdateCurSerialTypeAccordingToSkillPoints();
	void RefreshCurSerialType();

	//Special skill use interface
	bool IsHasStudySpecialSkill(const enmSkillSpecialType& type);
	bool IsCanStudySpecialSkill(const enmSkillSpecialType& type);
private:
	stSkillIcon*	m_activeSkillIcon;
	DWORD			m_activeSkillID;

	CToolTips* GetSkillToolTips( stSkillIcon* pSkillIcon );
};