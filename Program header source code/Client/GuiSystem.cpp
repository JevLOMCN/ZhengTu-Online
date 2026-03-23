#include "public.h"
#include "./GameGuiManager.h"
#include ".\guisystem.h"
#include "./GameAppation.h"
#include "./GuiMain.h"
#include "./GuiHelp.h"
#include "./GuiMiniMap.h"
#include "./GuiChatOutput.h"
#include "../luabind/public.h"
#include "./Chat.h"
#include "GameScene.h"
#include "MainCharacter.h"

#define MSGSTRING_FOR_CHANGDISPLAYSETTINGS "你的桌面分辨率不适于游戏，你希望更改桌面分辨率更好的体验游戏吗？"

void CGuiSystem::bind_lua(lua_State* L)
{
	using namespace luabind;
	CGuiDialog::bind_lua(L);
	module(L)
	[
		class_<CGuiSystem,CGuiDialog>("CGuiSystem")
			.def("ApplaySetting",&CGuiSystem::ApplaySetting)
			.def("SetDefault",&CGuiSystem::SetDefault)
	];
	m_lua->SetGlobal("this",this);
}

/**
 * \brief 构造函数
 * 
 */
CGuiSystem::CGuiSystem(void)
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;

	FUNCTION_END;
}

/**
 * \brief "关闭"消息响应函数
 * 
 * 退出对话框、更新管理器对应的指针
 * 
 * \return void
 */
void CGuiSystem::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiSystem = NULL;

	FUNCTION_END;
}

/**
 * \brief "创建"消息响应函数
 * 
 * 获得系统配置信息，设置个控件的初始值
 * 
 * \return void
 */
void CGuiSystem::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	SetStaticColor(310,Default_Color_Nine);
	SetStaticColor(311,Default_Color_Country);
	SetStaticColor(312,Default_Color_Erea);
	SetStaticColor(313,Default_Color_Whisper);
	SetStaticColor(314,Default_Color_Union);
	SetStaticColor(315,Default_Color_Team);
	SetStaticColor(316,Default_Color_Family);
	SetStaticColor(317,Default_Color_Friend);
	SetStaticColor(318,Default_Color_School);
	SetStaticColor(319,Default_Color_World);

	GetTab(4)->SetCurItem(0);
	GetTab(4)->SetVisible(false);

	stGameConfig config = *((CGameAppation*)Engine_GetAppation())->GetConfig();
	SetServerData(config.serverData);

	FUNCTION_END;
}

HRESULT CGuiSystem::OnRender(float fElapsedTime)
{
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	return hr;
}

/**
 * \brief GUI消息处理函数
 * 
 * 分发Tab页面切换消息，处理按钮点击消息(颜色设置)
 * 
 * \param nEvent : 消息句柄
 * \param nID : 控件id
 * \param pControl : 控件指针
 * \return true if succeed else false
 */
bool CGuiSystem::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
			GetTab(4)->SetCurItem(nID - 10);
			GetButton(nID)->SetChecked(true);
			return true;
		case 2://默认
			SetDefault();
			return true;
			break;
		case 3://应用
			{
				if(GetCheckBox(104)->GetChecked())
				{
					// 选择窗口模式时才做这一步， 

					// 检测用户当前的桌面屏幕的分辨率信息

					// 分辨率应从注册表中取，当前分辨率可能改变已经不是用户的原始设定 

					DEVMODE modReg;
					memset(&modReg,0,sizeof(modReg));
					modReg.dmSize = sizeof(modReg);
					modReg.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
					EnumDisplaySettings(NULL,ENUM_REGISTRY_SETTINGS,&modReg);
					int w = modReg.dmPelsWidth;
					int h = modReg.dmPelsHeight;

					//检测当前用户选择分辨率信息
					int nScreenSizeType = GetComboBox(101)->GetCurItem();
					
					if (( w < m_aWindowSize[nScreenSizeType].cx ) ||( h < m_aWindowSize[nScreenSizeType].cy ))
					{
						GetGameGuiManager()->AddMessageBox(MSGSTRING_FOR_CHANGDISPLAYSETTINGS , eAction_ChangeDisplaySettings, NULL, c_szMBOKCancel );
					}
					else
					{
						ApplaySetting();
						Close();
					}
				}else{
					ApplaySetting();
					Close();
				}
				return true;
			}
			break;
		case 15:
			OnBestCompatible();
			return true;
		case 16:
			OnBestEffect();
			return true;
			break;
		case 310:
		case 311:
		case 312:
		case 313:
		case 314:
		case 315:
		case 316:
		case 317:
			{
				CGuiStatic *pStatic = GetStatic(nID);
				if(pStatic)
				{
					DWORD dwColor = pStatic->GetBackGroundColor();
					GetGameGuiManager()->AddColorDialog(eAction_Color_SystemSetting,dwColor,pStatic);
				}
				return true;
			}
			break;
		}
	}

	if( nEvent == EVENT_CHECKBOX_CHANGED)
	{
		switch(nID)
		{
		case 200:
			{
				UpdateSoundPannel();
				return true;
			}
			break;
		}
	}

	if( nEvent == EVENT_DIALOG_SHOW )
	{
		stGameConfig config = *((CGameAppation*)Engine_GetAppation())->GetConfig();
		SetConfig(config);
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

/**
 * \brief 应用设置
 * 
 * 获取控件显示的配置,更新系统配置(发送命令、保存到本地配置文件)
 *
 * \return void
 */
void CGuiSystem::ApplaySetting()
{
	FUNCTION_BEGIN;
	
	stGameConfig config = *((CGameAppation*)Engine_GetAppation())->GetConfig();	
	
	int nScreenSizeType = GetComboBox(101)->GetCurItem();
	int nIndex = GetComboBox(103)->GetCurItem();
	if(nScreenSizeType < 0	|| nScreenSizeType > 255) return ;
	int nColorByte = (int )(GetComboBox(103)->GetItemData(nIndex));
	if(nColorByte < 0		|| nColorByte > 255) return ;
	
	config.screenWidth = m_aWindowSize[nScreenSizeType].cx;
	config.screenHeight = m_aWindowSize[nScreenSizeType].cy;
	config.colorBits = nColorByte;
	config.bWindow = GetCheckBox(104)->GetChecked();
	config.bVertSync = GetCheckBox(105)->GetChecked();
//	config.bLightMap = false;
	config.bLightMap = GetCheckBox(106)->GetChecked(); //soke 支持开启光影效果
	config.bWeather = GetCheckBox(107)->GetChecked();
	//config.bMultiTex = GetCheckBox(110)->GetChecked();
	config.bRenderTarget = true;//GetCheckBox(109)->GetChecked();
	config.bHardwareCursor = GetCheckBox(108)->GetChecked();
	config.bUseDXT = GetCheckBox(109)->GetChecked();

	config.bMainSound = GetCheckBox(200)->GetChecked();
	config.bSceneSound = GetCheckBox(201)->GetChecked();
	config.bCircumstanceSound = GetCheckBox(202)->GetChecked();
	config.bBackgroundSound = GetCheckBox(203)->GetChecked();

	config.nMainSound = GetSlider(210)->GetValue();
	config.nSceneSound = GetSlider(211)->GetValue();
	config.nCircumstanceSound = GetSlider(212)->GetValue();
	config.nBackgroundSound = GetSlider(213)->GetValue();

	UpdateCheckCofig(300,config.serverData,USER_SETTING_CHAT_NINE);
	UpdateCheckCofig(301,config.serverData,USER_SETTING_CHAT_COUNTRY);
	UpdateCheckCofig(302,config.serverData,USER_SETTING_CHAT_AREA);
	UpdateCheckCofig(303,config.serverData,USER_SETTING_CHAT_WHISPER);
	UpdateCheckCofig(304,config.serverData,USER_SETTING_CHAT_UNION);
	UpdateCheckCofig(305,config.serverData,USER_SETTING_CHAT_TEAM);
	UpdateCheckCofig(306,config.serverData,USER_SETTING_CHAT_FAMILY);
	UpdateCheckCofig(307,config.serverData,USER_SETTING_CHAT_FRIEND);
	UpdateCheckCofig(308,config.serverData,USER_SETTING_CHAT_SCHOOL);
	UpdateCheckCofig(309,config.serverData,USER_SETTING_CHAT_WORLD);
    
	UpdateCheckCofig(400,config.serverData,USER_SETTING_TEAM);
	UpdateCheckCofig(401,config.serverData,USER_SETTING_TRADE);
	UpdateCheckCofig(506,config.serverData,USER_SETTING_AUTOFINDPATH);
	UpdateCheckCofig(403,config.serverData,USER_SETTING_FAMILY);
	UpdateCheckCofig(404,config.serverData,USER_SETTING_UNION);
	UpdateCheckCofig(405,config.serverData,USER_SETTING_FRIEND);

	UpdateCheckCofig(406,config.serverData,USER_SETTING_SCHOOL);
	UpdateCheckCofig(407,config.serverData,USER_SETTING_MINIGAME);

	UpdateCheckCofig(500,config.serverData,USER_SETTING_SHOW_HEADPORTRAIT);
	UpdateCheckCofig(501,config.serverData,USER_SETTING_SHOW_MIMIMAP);
	UpdateCheckCofig(503,config.serverData,USER_SETTING_SHOW_PLAYERNAME);
	UpdateCheckCofig(504,config.serverData,USER_SETTING_SHOW_HELPTIP);
	UpdateCheckCofig(505,config.serverData,USER_SETTING_SHOW_ALLMINIMAP);
	UpdateCheckCofig(507,config.serverData,USER_SETTING_SHOW_ITEM);
	UpdateCheckCofig(509,config.serverData,USER_SETTING_EFFECT);

	UpdateStaticColor(310,config.serverData,0);
	UpdateStaticColor(311,config.serverData,1);
	UpdateStaticColor(312,config.serverData,2);
	UpdateStaticColor(313,config.serverData,3);
	UpdateStaticColor(314,config.serverData,4);
	UpdateStaticColor(315,config.serverData,5);
	UpdateStaticColor(316,config.serverData,6);
	UpdateStaticColor(317,config.serverData,7);   

	UpdateCheckConfigClient(502,config,enumAuto_Kill_Summon);	//自动打怪放在客户端
	UpdateCheckConfigClient(508,config,enumAuto_Reconnect);	//断线重连放在客户端

	GetGameApplication()->SetServerConfigData(&(config.serverData));
	GetGameApplication()->SaveServerConfigData();

	((CGameAppation*)Engine_GetAppation())->SetConfig(&config);

	Engine_GetAppation()->ApplyConfig();

	Engine_GetAppation()->SaveConfig();

	//刷新帮助教程开关
	UpdateHelpTipSwitch();

	//刷新小地图开关
	UpdateMiniMapSwitch();

	//刷新天气开关
	if(config.bWeather && GetWeatherMgr())
	{
		GetWeatherMgr()->OpenWeatherEffect();
	}
	else
	{
		GetWeatherMgr()->CloseWeatherEffect();
	}

	FUNCTION_END;
}

void CGuiSystem::UpdateHelpTipSwitch()
{
	bool b = GetCheckBox(504)->GetChecked();	
	//刷新帮助教程按钮的显示
	if(GetGameGuiManager()->m_guiMain && !b)
	{
		GetGameGuiManager()->m_guiMain->ShowAllTipBtn(b);
	}
	//同时刷新帮助界面的教程开关
	if(GetGameGuiManager()->m_guiHelp)
	{
		//GetGameGuiManager()->m_guiHelp->UpdateHelpSwitchCheck(b);
	}
}

void CGuiSystem::UpdateMiniMapSwitch()
{
	if(GetGameGuiManager()->m_guiMiniMap)
	{
		bool b = GetCheckBox(501)->GetChecked();		
		GetGameGuiManager()->m_guiMiniMap->SetVisible(b);
	}
}

/**
 * \brief 设置控件默认值
 * 
 * 设置控件默认值
 * 
 * \return void
 */
void CGuiSystem::SetDefault()
{
	FUNCTION_BEGIN;

	stGameConfig config ;

	int index = GetTab(4)->GetCurItem();

	config.screenWidth = 1024;
	config.screenHeight = 768;
	config.bWindow = false;
	config.bLightMap = false;
	config.bWeather = false;
	config.bMultiTex = true;
	config.bRenderTarget = true;

	config.bMainSound = true;
	config.bSceneSound = true;
	config.bCircumstanceSound = true;
	config.bBackgroundSound = true;
	config.bHardwareCursor = true;

	config.bUseDXT = false;
	config.bMultiTex = GetDevice()->IsSupportFlag(IDevice::DEVICE_MULTITEX);

	config.nMainSound = 50;
	config.nSceneSound = 50;
	config.nCircumstanceSound = 50;
	config.nBackgroundSound = 50;

	config.SetSystemSet(USER_SETTING_TEAM);
	config.SetSystemSet(USER_SETTING_TRADE);
	config.SetSystemSet(USER_SETTING_FAMILY);
	config.SetSystemSet(USER_SETTING_UNION);
	config.SetSystemSet(USER_SETTING_AUTOFINDPATH);

	config.SetSystemSet(USER_SETTING_SHOW_HEADPORTRAIT);
	config.SetSystemSet(USER_SETTING_SHOW_MIMIMAP);
	config.ClearSystemSet(USER_SETTING_SHOW_PLAYERNAME);//默认不打开
	config.SetSystemSet(USER_SETTING_SHOW_HELPTIP);
	config.SetSystemSet(USER_SETTING_SHOW_ALLMINIMAP);	

	config.SetSystemSet(USER_SETTING_CHAT_NINE);
	config.SetSystemSet(USER_SETTING_CHAT_COUNTRY);
	config.SetSystemSet(USER_SETTING_CHAT_AREA);
	config.SetSystemSet(USER_SETTING_CHAT_WHISPER);
	config.SetSystemSet(USER_SETTING_CHAT_UNION);
	config.SetSystemSet(USER_SETTING_CHAT_TEAM);
	config.SetSystemSet(USER_SETTING_CHAT_FAMILY);
	config.SetSystemSet(USER_SETTING_CHAT_FRIEND);
	config.SetSystemSet(USER_SETTING_CHAT_SCHOOL);
	config.SetSystemSet(USER_SETTING_CHAT_WORLD);
	config.SetSystemSet(USER_SETTING_FRIEND);
	config.SetSystemSet(USER_SETTING_SCHOOL);
	config.SetSystemSet(USER_SETTING_SHOW_ITEM);
	config.SetSystemSet(USER_SETTING_MINIGAME);	
	config.SetSystemSet(USER_SETTING_EFFECT);	

	config.serverData.dwChatColor[0] = Default_Color_Nine;
	config.serverData.dwChatColor[1] = Default_Color_Country;
	config.serverData.dwChatColor[2] = Default_Color_Erea;
	config.serverData.dwChatColor[3] = Default_Color_Whisper;
	config.serverData.dwChatColor[4] = Default_Color_Union;
	config.serverData.dwChatColor[5] = Default_Color_Team;
	config.serverData.dwChatColor[6] = Default_Color_Family;
	config.serverData.dwChatColor[7] = Default_Color_Friend;
	
	SetConfig(config,index);
	 
	FUNCTION_END;
}

inline void CGuiSystem::UpdateCheckShown(int id,stSystemSettings setting,enumUserSetting us)
{
	bool bCheck = isset_state(setting.bySettings,us);
	CGuiCheckBox* pCheck = GetCheckBox(id);
	if(pCheck)
	{
		pCheck->SetChecked(bCheck);
	}
}

inline void CGuiSystem::UpdateCheckCofig(int id,stSystemSettings& setting,enumUserSetting us)
{
	CGuiCheckBox* pCheck = GetCheckBox(id);
	if(pCheck)
	{
		bool bCheck = pCheck->GetChecked(); 
		clear_state(setting.bySettings,us);	
		if(bCheck) set_state(setting.bySettings,us);	
	}
}


inline void CGuiSystem::UpdateCheckShownClient(int id,stGameConfig& cfg,enumClientSetting set)
{
	bool bCheck = cfg.bClientSetted(set);		
	CGuiCheckBox* pCheck = GetCheckBox(id);
	if(pCheck)
	{
		pCheck->SetChecked(bCheck);
	}
}

inline void CGuiSystem::UpdateCheckConfigClient(int id,stGameConfig& cfg,enumClientSetting set)
{
	CGuiCheckBox* pCheck = GetCheckBox(id);
	if(pCheck)
	{
		bool bCheck = pCheck->GetChecked();
		if ( set == enumAuto_Kill_Summon )
		{
			if ( cfg.bClientSetted(set) )
			{
				if ( !bCheck )
				{
					GetScene()->GetMainCharacter()->ClearPath();
					GetScene()->GetMainCharacter()->ClearRunEvent();
				}
			}
		}
		cfg.SetClientSet(set,bCheck);
	}
}

inline void CGuiSystem::UpdateStaticColor(int id,stSystemSettings& setting,int index)
{	
	CGuiStatic *p = GetStatic(id);
	if(p) setting.dwChatColor[index] = p->GetBackGroundColor();
}

inline void CGuiSystem::SetStaticColor(int id,DWORD color)
{
	CGuiStatic *p = GetStatic(id);
	if(p)
	{
		p->SetBackGroundColor(color);
	}
}

/**
 * \brief 根据系统设置设置控件值
 * 
 * 设置聊天显示颜色、组队允许等配置
 * 
 * \param serverData : 保存在服务器的设置
 * \return void
 */
void CGuiSystem::SetServerData(stSystemSettings serverData,int index)
{
	FUNCTION_BEGIN;

	if(index == -1 || index == 2)
	{
		UpdateCheckShown(300,serverData,USER_SETTING_CHAT_NINE);
		UpdateCheckShown(301,serverData,USER_SETTING_CHAT_COUNTRY);
		UpdateCheckShown(302,serverData,USER_SETTING_CHAT_AREA);
		UpdateCheckShown(303,serverData,USER_SETTING_CHAT_WHISPER);
		UpdateCheckShown(304,serverData,USER_SETTING_CHAT_UNION);
		UpdateCheckShown(305,serverData,USER_SETTING_CHAT_TEAM);
		UpdateCheckShown(306,serverData,USER_SETTING_CHAT_FAMILY);
		UpdateCheckShown(307,serverData,USER_SETTING_CHAT_FRIEND);
		UpdateCheckShown(308,serverData,USER_SETTING_CHAT_SCHOOL);
		UpdateCheckShown(309,serverData,USER_SETTING_CHAT_WORLD);

		SetStaticColor(310,serverData.dwChatColor[0]);
		SetStaticColor(311,serverData.dwChatColor[1]);
		SetStaticColor(312,serverData.dwChatColor[2]);
		SetStaticColor(313,serverData.dwChatColor[3]);
		SetStaticColor(314,serverData.dwChatColor[4]);
		SetStaticColor(315,serverData.dwChatColor[5]);
		SetStaticColor(316,serverData.dwChatColor[6]);
		SetStaticColor(317,serverData.dwChatColor[7]);  			
	}

	if(index == -1 || index == 3)
	{
		UpdateCheckShown(400,serverData,USER_SETTING_TEAM);
		UpdateCheckShown(401,serverData,USER_SETTING_TRADE);
		UpdateCheckShown(506,serverData,USER_SETTING_AUTOFINDPATH);
		UpdateCheckShown(403,serverData,USER_SETTING_FAMILY);
		UpdateCheckShown(404,serverData,USER_SETTING_UNION);
		UpdateCheckShown(405,serverData,USER_SETTING_FRIEND);
		UpdateCheckShown(406,serverData,USER_SETTING_SCHOOL);
		UpdateCheckShown(407,serverData,USER_SETTING_MINIGAME);		
	}
	if(index == -1 || index == 4)
	{
		UpdateCheckShown(500,serverData,USER_SETTING_SHOW_HEADPORTRAIT);
		UpdateCheckShown(501,serverData,USER_SETTING_SHOW_MIMIMAP);
		UpdateCheckShown(503,serverData,USER_SETTING_SHOW_PLAYERNAME);
		UpdateCheckShown(504,serverData,USER_SETTING_SHOW_HELPTIP);
		UpdateCheckShown(505,serverData,USER_SETTING_SHOW_ALLMINIMAP);
		UpdateCheckShown(507,serverData,USER_SETTING_SHOW_ITEM);	
		UpdateCheckShown(509,serverData,USER_SETTING_EFFECT);			

		UpdateCheckShownClient(502,*(GetGameApplication()->GetConfig()),enumAuto_Kill_Summon);

		UpdateCheckShownClient(508,*(GetGameApplication()->GetConfig()),enumAuto_Reconnect);
	}


	FUNCTION_END;
}

/**
 * \brief 设置各控件的显示值
 * 
 * 设置各控件的显示值(根据本地配置和服务器端的配置)
 * 
 * \return void
 */
void CGuiSystem::SetConfig(stGameConfig config,int index)
{
	FUNCTION_BEGIN;

	if(index == -1 || index == 0)
	{
		CGuiComboBox* pCtrlWindowSize = GetComboBox(101);
		pCtrlWindowSize->RemoveAllItems();
		CGuiComboBox* pCtrlColor = GetComboBox(103);
		pCtrlColor->RemoveAllItems();
		m_nWindowSizeCount = GetDevice()->GetWindowSizeList(m_aWindowSize,256);
		m_nColorBitsCount = GetDevice()->GetPixelBitsList( ( DWORD* )m_aColorBits,256);

		for(size_t i=0;i<m_nWindowSizeCount;++i)
		{
			char szStr[256];
			sprintf(szStr,"%d x %d",m_aWindowSize[i].cx,m_aWindowSize[i].cy);
			pCtrlWindowSize->AddItem(szStr,NULL);
		}

		for(size_t i=0;i<m_nColorBitsCount;++i)
		{
			char szStr[256];
			if(m_aColorBits[i] == 16 || m_aColorBits[i] == 32)
			{
				sprintf(szStr,"%d 位色",m_aColorBits[i]);
				pCtrlColor->AddItem(szStr,(void*)m_aColorBits[i]);
			}
		}

		int nScreenSizeType = 0;
		for(size_t i=0;i<m_nWindowSizeCount;++i)
		{
			if( config.screenWidth == m_aWindowSize[i].cx && config.screenHeight == m_aWindowSize[i].cy )
			{
				nScreenSizeType = i;
				break;
			}
		}

		int nColorMode = 0;
		for(int i = 0;i < pCtrlColor->GetItemCount();++i)
		{
			if ( (int)(pCtrlColor->GetItemData(i)) == config.colorBits )
			{
				nColorMode = i;
				break;
			}
		}

		GetComboBox(101)->SetCurItem(nScreenSizeType);
		GetComboBox(103)->SetCurItem(nColorMode);

		GetCheckBox(104)->SetChecked(config.bWindow);
		GetCheckBox(105)->SetChecked(config.bVertSync);
		GetCheckBox(106)->SetChecked(config.bLightMap);	//soke 支持开启光影效果
	//	GetCheckBox(106)->SetChecked(false);
	//	GetCheckBox(106)->SetEnabled(false);
		GetCheckBox(107)->SetChecked(config.bWeather);		
		GetCheckBox(108)->SetChecked(config.bHardwareCursor);
		GetCheckBox(109)->SetChecked(config.bUseDXT);
		GetCheckBox(109)->SetEnabled(GetDevice()->IsSupportFlag(IDevice::DEVICE_DXT));
	}

	if(index == -1 || index == 1)
	{
		GetCheckBox(200)->SetChecked(config.bMainSound);
		GetCheckBox(201)->SetChecked(config.bSceneSound);
		GetCheckBox(202)->SetChecked(config.bCircumstanceSound);
		GetCheckBox(203)->SetChecked(config.bBackgroundSound);

		GetSlider(210)->SetValue(config.nMainSound);
		GetSlider(211)->SetValue(config.nSceneSound);
		GetSlider(212)->SetValue(config.nCircumstanceSound);
		GetSlider(213)->SetValue(config.nBackgroundSound);

		UpdateSoundPannel();
	}
	SetServerData(config.serverData,index);

	FUNCTION_END;
}

void CGuiSystem::UpdateSoundPannel()
{
	FUNCTION_BEGIN;

	bool bSound = GetCheckBox(200)->GetChecked();
	GetCheckBox(201)->SetEnabled(bSound);
	GetCheckBox(202)->SetEnabled(bSound);
	GetCheckBox(203)->SetEnabled(bSound);
	GetSlider(210)->SetEnabled(bSound);
	GetSlider(211)->SetEnabled(bSound);
	GetSlider(212)->SetEnabled(bSound);
	GetSlider(213)->SetEnabled(bSound);

	FUNCTION_END;
}

void CGuiSystem::UpdateAutoPkSwitch()
{
	UpdateCheckShownClient(502,*(GetGameApplication()->GetConfig()),enumAuto_Kill_Summon);	
}

void CGuiSystem::UpdateAutoReconnectSwitch()
{
	UpdateCheckShownClient(508,*(GetGameApplication()->GetConfig()),enumAuto_Reconnect);	
}

/**
 * \brief 初始化脚本环境
 * 
 * 注册脚本，设置控件的显示值
 * 
 * \return void
 */
void CGuiSystem::OnInitScript()
{
	FUNCTION_BEGIN;
	stGameConfig config = *((CGameAppation*)Engine_GetAppation())->GetConfig();
	stGameConfig::bind_lua(GetScriptEngine());
	SetConfig(config);	

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CGuiSystem::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x - 5 ,pt.y-2);
	}

	FUNCTION_END;
}


// 最佳兼容
void CGuiSystem::OnBestCompatible()
{
	FUNCTION_BEGIN;
	stGameConfig config = *((CGameAppation*)Engine_GetAppation())->GetConfig();	

	config.screenWidth = 800;
	config.screenHeight = 600;
	config.colorBits = 16;

	config.bLightMap = false;
	config.bWeather = false;
	config.bUseDXT = false;
	config.bVertSync = false;

	SetConfig(config,0);
	FUNCTION_END;
}

// 最佳效果
void CGuiSystem::OnBestEffect()
{
	FUNCTION_BEGIN;
	stGameConfig config = *((CGameAppation*)Engine_GetAppation())->GetConfig();	

	config.bLightMap = true;
	config.bWeather = true;
	config.bUseDXT = true;
	config.bMultiTex = true;

	SetConfig(config,0);
	FUNCTION_END;
}