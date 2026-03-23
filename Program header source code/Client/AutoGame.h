#ifndef _AUTO_GAME_H
#define _AUTO_GAME_H

struct stAutoGameConfig
{
	DWORD minHP;
	float minHPPercent;
	bool  useMinHPPercent;			// 按百分比判断HP
	bool  autoAddHP;
	bool  autoAddMP;
	bool  autoPickupObject;			// 自动检物品
	std::set<DWORD> setCullObject;	// 不检的物品ID
	std::set<DWORD> setCullNPC;		// 不打的怪物
	std::vector<DWORD> stateSkills; // 状态技能
	HMODULE hConfigDll;
	stAutoGameConfig();
	~stAutoGameConfig();

	typedef void (* t_fnShowConfigDlg)(HWND hParent);
	typedef void (* t_fnLoadNpcTables)(BYTE* pData);
	typedef void (* t_fnLoadSkillTables)(BYTE* pData);
	typedef void (* t_fnLoadSkillLevelTables)(BYTE* pData);
	typedef void (* t_fnLoadConfig)();
	typedef void (* t_fnSetStateSkill)(DWORD* pSkillID,DWORD num);
	typedef bool (* t_fnReSetSkillData)(SkillBase* pSkill,DWORD dwLevel);
	t_fnShowConfigDlg fnShowConfigDlg;
	t_fnLoadNpcTables fnLoadNpcTables;
	t_fnLoadSkillTables fnLoadSkillTables;
	t_fnLoadSkillLevelTables fnLoadSkillLevelTables;
	t_fnLoadConfig	  fnLoadConfig;
	t_fnSetStateSkill fnSetStateSkill;
	t_fnReSetSkillData fnReSetSkillData;
	void loadConfig();
	void showConfigWnd();
};

extern stAutoGameConfig gAutoGameConfig;
void enableAutoGame(bool enable);
#endif