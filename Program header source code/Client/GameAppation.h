#pragma once

#include "..\engine\include\appation.h"
#include "./command.h"
#include "./EffectCloud.h"
#include "./EffectRain.h"

#include "./Particle/ParticleSysMgr.h"
#include "./WeatherMgr.h"

#define    SCREENWIDTH_LOGIN    1024
#define    SCREENHEIGHT_LOGIN   768
// ==================================================================================
extern bool g_bSitDownHotKeyActive;

// ==================================================================================
//用于保存在本地配置文件中的配置选项
enum enumClientSetting
{
	enumAuto_Reply,			//自动回复
	enumAuto_Reconnect,		//自动断线重连
	enumAuto_Kill_Summon,	//自动打怪
};
// ==================================================================================
enum enumAutoReplyType
{
	AutoReply_ComebackSoon,
	AutoReply_BusyNow,
	AutoReply_MeetingNow,
	AutoReply_PhoneNow,
	AutoReply_EatingNow,
	AutoReply_NotHere,
	AutoReply_RestingNow,
	AutoReply_UserDefine,
	AutoReply_Null,
	AutoReply_Switch,
};
// ==================================================================================
struct stAutoReply
{
	char				szReplyText[MAX_PATH];
	enumAutoReplyType	type;
};
// ==================================================================================
static stAutoReply g_sAutoReplyCmd[] = 
{
	{"马上回来",AutoReply_ComebackSoon},
	{"现在忙",AutoReply_BusyNow},
	{"正在会议中",AutoReply_MeetingNow},
	{"正在接听电话",AutoReply_PhoneNow},
	{"外出就餐",AutoReply_EatingNow},
	{"不在电脑旁",AutoReply_NotHere},
	{"休息中,请勿打扰",AutoReply_RestingNow},
	{"自定义",AutoReply_UserDefine},
};
struct lua_State;
// ==================================================================================
// stGameConfig
// ==================================================================================
struct stGameConfig : public stConfigBase
{
	char	szInitMapName[256];
	POINT	ptInit;
	char	szRegUrl[256];
	char	szModifyPasswdUrl[256];
	char	szBugAddr[256];
	char	szBugObj[256];
	char	szHomeURL[256];
	int		nZone;
	char	szLoginAddress[256];
	char	szLoginPort[256];
	stSystemSettings serverData;
	char    szPlayerName[MAX_NAMESIZE+1];

	enumAutoReplyType autoReplyType;
	char	szAutoReply[MAX_PATH];
	BYTE	clientData[8];

	//游戏配置
	stGameConfig();

	bool    bClientSetted(enumClientSetting set)
	{
		return isset_state(clientData,set);
	}
	void    SetClientSet(enumClientSetting set,bool bSetted = true )
	{
		if( bSetted )
		{
			set_state(clientData,set);
		}
		else
		{
			clear_state(clientData,set);
		}
	}

	void SetAutoReplayText( const char* text )
	{
		_snprintf(szAutoReply,MAX_PATH,text);
		szAutoReply[MAX_PATH - 1] = 0;
	}

	bool	bSystemSetted(enumUserSetting userset)
	{	
		return isset_state(serverData.bySettings,userset); 
	}
	void	SetSystemSet(enumUserSetting userset)
	{
		set_state(serverData.bySettings,userset); 		
	}

	void	ClearSystemSet(enumUserSetting userset)
	{
		clear_state(serverData.bySettings,userset); 		
	}

	void	SetSystemSetOnServer(enumUserSetting userset)
	{
		SetSystemSet(userset);
		UpdateSystemSettingOnServer();
	}

	void	ClearSystemSetOnServer(enumUserSetting userset)
	{
		ClearSystemSet(userset);
		UpdateSystemSettingOnServer();
	}

	void UpdateSystemSettingOnServer()
	{	
		stSystemSettingsUserCmd cmd;		
		cmd.data = serverData;
		SEND_USER_CMD(cmd);
	}

	static void bind_lua(lua_State* L);
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGameAppation :
	public CAppation
{
	stGameConfig m_Config;

	
	std::vector<stPicCharInfo> m_aPicCharInfo;
	struct stPicCharEsc{
		char esc[32];
		DWORD id;
	};
	std::vector<stPicCharEsc> m_aPicChar;
	///粒子系统管理器	
	CParticleSysManager*	m_pParticleSysMgr;
	//天气管理器
	CWeatherMgr*			m_pWeatherMgr;
public:
	bool m_bESCHook; //用于hook"ESC按键"的功能

public:
	CEffectCloud*		m_loginCloud;

public:
	CGameAppation(void);
	~CGameAppation(void);

	stConfigBase* GetConfigBase(){ return & m_Config; }
	
	HRESULT InitParticleDevice();
	HRESULT InitLoginCloud();
	void UnInitLoginCloud();

	bool InitInstance();
	int  ExitInstance();
	LRESULT MsgProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
	void RefreshFrame(float fElapsedTime);
	void Draw(float fElapsedTime);
	void DrawWeatheAndParticleEffect(float fElapsedTime);
	bool AforeDraw();

	void Exit();

	void LoadConfig();
	void SaveConfig();
	void SetConfig(stGameConfig* config)
	{
		m_Config = *config;
	}
	stGameConfig* GetConfig(){ return &m_Config;}
	void ApplyConfig();

	//放在服务器的系统设置
	bool	bSystemSetted(enumUserSetting userset){	return m_Config.bSystemSetted(userset); }
	void	SetSystemSet(enumUserSetting userset){ m_Config.SetSystemSet(userset); }
	void	ClearSystemSet(enumUserSetting userset) { m_Config.ClearSystemSet(userset); }
	void	SetSystemSetOnServer(enumUserSetting userset) { m_Config.SetSystemSetOnServer(userset); }
	void	ClearSystemSetOnServer(enumUserSetting userset) { m_Config.ClearSystemSetOnServer(userset); }

	//放在本地的系统设置
	bool    bClientSetted(enumClientSetting set){return m_Config.bClientSetted(set);}
	void    SetClientSet(enumClientSetting set,bool bSetted = true ){m_Config.SetClientSet(set,bSetted);}

	const char* GetAutoReplyText( enumAutoReplyType type );
	void SetAutoReplyType( enumAutoReplyType type );
	
	void SetServerConfigData( stSystemSettings * serverData );
	void SaveServerConfigData();
	stSystemSettings GetServerConfigData(){ return m_Config.serverData;}
	void LoadFaceInfo();

	bool ConverPicChar(const char* str,WORD& ch,int & charNum);
	bool SearchPicChar(char* str,char* strContext,const char* strSeparator,int index);
	int  GetPicCount(){ return m_aPicCharInfo.size();}
};

CGameAppation* GetGameApplication();
