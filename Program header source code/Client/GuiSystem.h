#pragma once
#include "..\gui\include\guidialog.h"
#include "./GameAppation.h"
/**
 * \brief 系统配置对话框
 * 
 */
class CGuiSystem :
	public CGuiDialog
{
public:
	CGuiSystem(void);
	void OnClose(void);
	void OnInitScript();
	void OnCreate();
	void ApplaySetting();
	void SetDefault();
	void OnBestEffect();
	void OnBestCompatible();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnSetCloseLocation();
	void SetServerData(stSystemSettings serverData,int index = -1);
	void SetConfig(stGameConfig config,int index = -1);

	HRESULT OnRender(float fElapsedTime);

	void UpdateAutoPkSwitch();
	void UpdateAutoReconnectSwitch();

	void bind_lua(lua_State* L);
private:
	void UpdateSoundPannel();
	void UpdateHelpTipSwitch();
	void UpdateMiniMapSwitch();

	inline void UpdateCheckShown(int id,stSystemSettings setting,enumUserSetting us);
	inline void UpdateCheckCofig(int id,stSystemSettings& setting,enumUserSetting us);

	inline void UpdateCheckShownClient(int id,stGameConfig& cfg,enumClientSetting set);
	inline void UpdateCheckConfigClient(int id,stGameConfig& cfg,enumClientSetting set);

	inline void SetStaticColor(int id,DWORD color);
	inline void UpdateStaticColor(int id,stSystemSettings& setting,int index);
private:
	///窗口大小
	SIZE m_aWindowSize[256];

	///窗口大小类型的数目
	size_t m_nWindowSizeCount;

	///颜色质量
	DWORD m_aColorBits[256];

	///颜色质量类型的数目
	size_t m_nColorBitsCount;
};
