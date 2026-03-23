#include "public.h"
#include "Game.h"
#include ".\gameappation.h"
#include "../gui/include/guiTypes.h"
#include "../Media/SoundManager.h"
#include "../engine/include/Fps.h"
#include "./resource.h"
#include "./GameScene.h"
#include "./gameState.h"
#include "./GameGuiManager.h"
#include "./GameHookThread.h"
#include "./GuiMiniMap.h"
#include "./GuiTeam.h"
#include "./GuiMain.h"
#include "./GuiLogin.h"
#include "../Media/Music.h"
#include "../xml_parse/XmlParseEx.h"
#include "./GameCursor.h"
#include "./ErrorDlg.h"
#include ".\guisystem.h"
#include "./LoadingDlg.h"
#include "./GameTime.h"
#include "./AutoGame.h"
#include "./KillProcess.h"
#include ".\NetAutoconnect.h"
#include "../engine/include/nofree_alloc.h"
#include "../luabind/public.h"
#include "./DumpError.h"
#include "./FindDLL.h"
#include "miniusercommand.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "./GuiAutoAttackDlg.h" //////////star100515
#include "TimerDisplayMgr.h"


#pragma comment( lib, "ijl15l.lib" )
#ifdef _USE_SHARE_MEMORY_PAGE
#include "..\MakeExe\SpyProcess\Communication.h"
#endif
CGameAppation theApp;

const char * c_szWindowTitle = "公测版1.0 — %s";
const char * c_szWindowClass = "xx_client";

bool g_bSitDownHotKeyActive = true;

void stGameConfig::bind_lua(lua_State* L)
{
	using namespace luabind;
	module(L)
	[
		class_<stGameConfig>("stGameConfig")
			.def(constructor<>())
	];
}
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CGameAppation* GetGameApplication()
{
	FUNCTION_BEGIN;

	return & theApp;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
stGameConfig::stGameConfig()
{
	FUNCTION_BEGIN;

	strcpy(szLoginAddress,"127.0.0.1");
	strcpy(szLoginPort,"7000");
	memset(serverData.bySettings,0,sizeof(serverData.bySettings));	
	memset(clientData,0,sizeof(clientData));

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CGameAppation::CGameAppation(void)
: m_pParticleSysMgr(NULL)
, m_pWeatherMgr(NULL)
{
	FUNCTION_BEGIN;
#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );	
#endif

	extern const char* c_szFileName;

	CIni ini(c_szFileName);
	sprintf(m_szWndName,c_szWindowTitle,ini.GetString("Server","servername","").c_str());
	
	strcpy(m_szWndClassName,c_szWindowClass);
	m_hIcon = LoadIcon(m_hInstance,MAKEINTRESOURCE(IDI_Main));

	m_bESCHook = false;//false 代表不hook ESC,只在1-15类型的窗口存在的时候hook
	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CGameAppation::~CGameAppation(void)
{
	FUNCTION_BEGIN;

	SAFE_DELETE( m_pParticleSysMgr );
	SAFE_DELETE( m_pWeatherMgr );

	UnInitLoginCloud();

	//_CrtDumpMemoryLeaks();
	FUNCTION_END;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
const char* c_szFileName = ".\\config.ini";
void CGameAppation::LoadConfig()
{
	FUNCTION_BEGIN;

	CIni ini(c_szFileName);
#if defined _DEBUG || defined RELEASE_TEST
	extern bool gEngineLimitFPS;
	gEngineLimitFPS = ini.GetBool("video","limitFPS",true);
#endif
	m_Config.screenWidth = ini.GetInt("video","ScreenWidth",1024);
	m_Config.screenHeight = ini.GetInt("video","ScreenHeight",768);

	m_Config.bWindow = ini.GetBool("video","Window",false);
	m_Config.colorBits = ini.GetInt("video","ColorBits",32);
	m_Config.bVertSync = ini.GetBool("video","VertSync",false);
	//soke 开启光影效果
	m_Config.bLightMap = ini.GetBool("video","LightMap",false);
	m_Config.bWeather =  ini.GetBool("video","Weather",false);
	m_Config.bMultiTex = ini.GetBool("video","MultiTex",true);
	m_Config.bRenderTarget = true;//ini.GetBool("video","RenderTarget",true);
	m_Config.bHardwareCursor = ini.GetBool("video","HardwareCursor",true);
	m_Config.bUseDXT = ini.GetBool("video","UseDXT",false);

	m_Config.bMainSound = ini.GetBool("audio","sound",true);
	m_Config.bSceneSound = ini.GetBool("audio","scenesound",true);
	m_Config.bCircumstanceSound = ini.GetBool("audio","circumstancesound",true);
	m_Config.bBackgroundSound = ini.GetBool("audio","backgroundsound",true);

	m_Config.nMainSound = ini.GetInt("audio","soundvolume",50);
	m_Config.nSceneSound = ini.GetInt("audio","scenevolume",50);
	m_Config.nCircumstanceSound = ini.GetInt("audio","circumstancevolume",50);
	m_Config.nBackgroundSound = ini.GetInt("audio","backvolume",50);

	//断线重连
	bool bAutoReconnect = ini.GetBool("Server","autoreconnect",false);
	m_Config.SetClientSet(enumAuto_Reconnect,bAutoReconnect);

	//自动打怪
	//bool bAutoKillSummon = ini.GetBool("other","AutoPK",false);
	m_Config.SetClientSet(enumAuto_Kill_Summon,false);
	extern bool g_bSinglePlayer;

#if defined _DEBUG || defined RELEASE_TEST
	g_bSinglePlayer = ini.GetBool("other","single",false);
#endif

	ini.GetString(m_Config.szInitMapName,256,"other","InitMap","fenghuangcheng-main");
	m_Config.ptInit.x = ini.GetInt("other","InitX",100);
	m_Config.ptInit.y = ini.GetInt("other","InitY",100);

	strncpy(m_Config.szLoginAddress,ini.GetString("Server","loginAddress","192.168.2.11").c_str(),sizeof(m_Config.szLoginAddress));
	strncpy(m_Config.szLoginPort,ini.GetString("Server","loginPort","7000").c_str(),sizeof(m_Config.szLoginPort));

	//ini.GetString(m_Config.szRegUrl,256,"Server","regurl","http://192.168.2.11/reg.html");
	//ini.GetString(m_Config.szModifyPasswdUrl,256,"Server","ModifyPasswdUrl","http://192.168.2.11/changepwd.html");

	m_Config.szLoginAddress[sizeof(m_Config.szLoginAddress)-1] = 0;
	m_Config.nZone = ini.GetInt("Server","zone",1);
	//m_Config.nLoginPort = ini.GetInt("server","loginPort",7000);

	ini.GetString(m_Config.szPlayerName,MAX_NAMESIZE,"other","Player","");
	m_Config.szPlayerName[MAX_NAMESIZE] = 0;

#ifdef _AUTOPLAY
	bool b1 = ini.GetBool("other","autogame",false);
	enableAutoGame(b1);
#endif

//soke begin 2015.11.17
//	CIni ini1(".\\patchsetup.ini");

//	ini1.GetString(m_Config.szRegUrl,256,"Setup","regurl","http://192.168.2.11/reg.html");
//	ini1.GetString(m_Config.szModifyPasswdUrl,256,"Setup","ModifyPasswdUrl","http://192.168.2.11/changepwd.html");
//	ini1.GetString(m_Config.szBugAddr,256,"Setup","BugAddr","game.ztgame.comp");
//	ini1.GetString(m_Config.szBugObj,256,"Setup","BugObj","/bugreport.php");
//	ini1.GetString(m_Config.szHomeURL,256,"Setup","home","http://www.ztgame.com.cn");
//soke end

	if(__argc == 5){
		strncpy(m_Config.szLoginAddress,__targv[2],sizeof(m_Config.szLoginAddress));
		m_Config.szLoginAddress[sizeof(m_Config.szLoginAddress)-1] = 0;

		strncpy(m_Config.szLoginPort,__targv[3],sizeof(m_Config.szLoginPort));
		m_Config.szLoginPort[sizeof(m_Config.szLoginPort)-1] = 0;

		m_Config.nZone = atol(__targv[4]);
	}
	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGameAppation::SaveConfig()
{
	FUNCTION_BEGIN;

	CIni ini(c_szFileName);

	ini.SetInt("video","ScreenWidth",m_Config.screenWidth);
	ini.SetInt("video","ScreenHeight",m_Config.screenHeight);
	ini.SetInt("video","Window",m_Config.bWindow);
	ini.SetInt("video","ColorBits",m_Config.colorBits);
	ini.SetInt("video","VertSync",m_Config.bVertSync);
	ini.SetInt("video","LightMap",m_Config.bLightMap);
	ini.SetInt("video","Weather",m_Config.bWeather);
	ini.SetInt("video","MultiTex",m_Config.bMultiTex);
	ini.SetInt("video","RenderTarget",m_Config.bRenderTarget);
	ini.SetInt("video","HardwareCursor",m_Config.bHardwareCursor);
	ini.SetInt("video","UseDXT",m_Config.bUseDXT);

	ini.SetInt("audio","sound",m_Config.bMainSound);
	ini.SetInt("audio","scenesound",m_Config.bSceneSound);
	ini.SetInt("audio","circumstancesound",m_Config.bCircumstanceSound);
	ini.SetInt("audio","backgroundsound",m_Config.bBackgroundSound);

	ini.SetInt("audio","soundvolume",m_Config.nMainSound);
	ini.SetInt("audio","scenevolume",m_Config.nSceneSound);
	ini.SetInt("audio","circumstancevolume",m_Config.nCircumstanceSound);
	ini.SetInt("audio","backvolume",m_Config.nBackgroundSound);

	ini.SetString("other","Player",m_Config.szPlayerName);
	ini.SetInt("Server","autoreconnect",m_Config.bClientSetted(enumAuto_Reconnect));
	ini.Save(c_szFileName);


	FUNCTION_END;
}

void CGameAppation::UnInitLoginCloud()
{
	if ( m_loginCloud )
	{
		m_loginCloud->Stop();
		SAFE_DELETE( m_loginCloud );
	}

	if ( m_pParticleSysMgr )
	{
		m_pParticleSysMgr->UnLoadOneParticleSystem( "LoginEffect\\Sun" );
		m_pParticleSysMgr->UnLoadOneParticleSystem( "LoginEffect\\SunRay" );
		m_pParticleSysMgr->UnLoadOneParticleSystem( "LoginEffect\\ZT00" );
		m_pParticleSysMgr->UnLoadOneParticleSystem( "LoginEffect\\ZT01" );
		m_pParticleSysMgr->UnLoadOneParticleSystem( "LoginEffect\\ZT02" );
		m_pParticleSysMgr->UnLoadOneParticleSystem( "LoginEffect\\ZT03" );
		m_pParticleSysMgr->UnLoadOneParticleSystem( "LoginEffect\\ZT04" );
		m_pParticleSysMgr->UnLoadOneParticleSystem( "LoginEffect\\ZT05" );

	}
}

HRESULT CGameAppation::InitLoginCloud()
{
	FUNCTION_BEGIN;

	return S_OK;
	
	//Load login ZT word
	if ( m_pParticleSysMgr )
	{
		float scaleRate = 0.3f;
		float totalTime = 11.0f;
		float startTime = 0.0f;
		float suppressTime = 0.0f;
		D3DXVECTOR3 vPos;
		vPos.x = GetDevice()->GetWidth() - 80;
		vPos.y = 25;
		vPos.z = 0;
		if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\ZT00" ) )
		{
			m_pParticleSysMgr->LoadOneParticleSystem( "LoginEffect\\ZT00", vPos );
			if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\ZT00" ) )
				return E_FAIL;
			m_pParticleSysMgr->GetActiveParticleSystem()->Play();
			m_pParticleSysMgr->GetActiveParticleSystem()->UseCharacterOffset( false );
			suppressTime = m_pParticleSysMgr->GetActiveParticleSystem()->GetSuppressTimeEnd()
				- m_pParticleSysMgr->GetActiveParticleSystem()->GetSuppressTimeStart();
			startTime = m_pParticleSysMgr->GetActiveParticleSystem()->GetLifeBorn()
				+ m_pParticleSysMgr->GetActiveParticleSystem()->GetSuppressTimeEnd();
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSysLife( totalTime );
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSizeScale( scaleRate, scaleRate*2.0f );
		}

		if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\ZT01" ) )
		{
			vPos.x += 80*scaleRate;
			vPos.y -= 40*scaleRate;
			m_pParticleSysMgr->LoadOneParticleSystem( "LoginEffect\\ZT01", vPos );
			if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\ZT01" ) )
				return E_FAIL;
			m_pParticleSysMgr->GetActiveParticleSystem()->Play();
			m_pParticleSysMgr->GetActiveParticleSystem()->UseCharacterOffset( false );
			//m_pParticleSysMgr->GetActiveParticleSystem()->SetStartTime( startTime - suppressTime );
			//m_pParticleSysMgr->GetActiveParticleSystem()->SetSuppressTimeStart( startTime - suppressTime );
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSuppressTimeEnd( startTime );
			suppressTime = m_pParticleSysMgr->GetActiveParticleSystem()->GetSuppressTimeEnd()
				- m_pParticleSysMgr->GetActiveParticleSystem()->GetSuppressTimeStart();
			startTime += m_pParticleSysMgr->GetActiveParticleSystem()->GetLifeBorn();
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSysLife( totalTime );
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSizeScale( scaleRate, scaleRate*2.0f );
		}
				
		if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\ZT02" ) )
		{
			vPos.x -= 130*scaleRate;
			vPos.y += 160*scaleRate;
			m_pParticleSysMgr->LoadOneParticleSystem( "LoginEffect\\ZT02", vPos );
			if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\ZT02" ) )
				return E_FAIL;
			m_pParticleSysMgr->GetActiveParticleSystem()->Play();
			m_pParticleSysMgr->GetActiveParticleSystem()->UseCharacterOffset( false );
			//m_pParticleSysMgr->GetActiveParticleSystem()->SetStartTime( startTime - suppressTime );
			//m_pParticleSysMgr->GetActiveParticleSystem()->SetSuppressTimeStart( startTime - suppressTime );
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSuppressTimeEnd( startTime );
			suppressTime = m_pParticleSysMgr->GetActiveParticleSystem()->GetSuppressTimeEnd()
				- m_pParticleSysMgr->GetActiveParticleSystem()->GetSuppressTimeStart();
			startTime += m_pParticleSysMgr->GetActiveParticleSystem()->GetLifeBorn() + 0.2;
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSysLife( totalTime );
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSizeScale( scaleRate, scaleRate*2.0f );
			
		}
		
		//T word
		if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\ZT03" ) )
		{
			vPos.x += 160*scaleRate;
			vPos.y -= 70*scaleRate;
			m_pParticleSysMgr->LoadOneParticleSystem( "LoginEffect\\ZT03", vPos );
			if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\ZT03" ) )
				return E_FAIL;
			m_pParticleSysMgr->GetActiveParticleSystem()->Play();
			m_pParticleSysMgr->GetActiveParticleSystem()->UseCharacterOffset( false );
			//m_pParticleSysMgr->GetActiveParticleSystem()->SetStartTime( startTime - suppressTime );
			//m_pParticleSysMgr->GetActiveParticleSystem()->SetSuppressTimeStart( startTime - suppressTime );
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSuppressTimeEnd( startTime );
			suppressTime = m_pParticleSysMgr->GetActiveParticleSystem()->GetSuppressTimeEnd()
				- m_pParticleSysMgr->GetActiveParticleSystem()->GetSuppressTimeStart();
			startTime += m_pParticleSysMgr->GetActiveParticleSystem()->GetLifeBorn();
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSysLife( totalTime );
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSizeScale( scaleRate, scaleRate*2.0f );
		}

		if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\ZT04" ) )
		{
			vPos.x += 55*scaleRate;
			vPos.y -= 85*scaleRate;
			m_pParticleSysMgr->LoadOneParticleSystem( "LoginEffect\\ZT04", vPos );
			if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\ZT04" ) )
				return E_FAIL;
			m_pParticleSysMgr->GetActiveParticleSystem()->Play();
			m_pParticleSysMgr->GetActiveParticleSystem()->UseCharacterOffset( false );
			//m_pParticleSysMgr->GetActiveParticleSystem()->SetStartTime( startTime - suppressTime );
			//m_pParticleSysMgr->GetActiveParticleSystem()->SetSuppressTimeStart( startTime - suppressTime );
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSuppressTimeEnd( startTime );
			suppressTime = m_pParticleSysMgr->GetActiveParticleSystem()->GetSuppressTimeEnd()
				- m_pParticleSysMgr->GetActiveParticleSystem()->GetSuppressTimeStart();
			startTime += m_pParticleSysMgr->GetActiveParticleSystem()->GetLifeBorn();
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSysLife( totalTime );
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSizeScale( scaleRate, scaleRate*2.0f );
		}
		
		if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\ZT05" ) )
		{
			vPos.x -= 60*scaleRate;
			vPos.y += 150*scaleRate;
			m_pParticleSysMgr->LoadOneParticleSystem( "LoginEffect\\ZT05", vPos );
			if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\ZT05" ) )
				return E_FAIL;
			m_pParticleSysMgr->GetActiveParticleSystem()->Play();
			m_pParticleSysMgr->GetActiveParticleSystem()->UseCharacterOffset( false );
			//m_pParticleSysMgr->GetActiveParticleSystem()->SetStartTime( startTime - suppressTime );
			//m_pParticleSysMgr->GetActiveParticleSystem()->SetSuppressTimeStart( startTime - suppressTime );
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSuppressTimeEnd( startTime );
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSysLife( totalTime );
			m_pParticleSysMgr->GetActiveParticleSystem()->SetSizeScale( scaleRate, scaleRate*2.0f );
		}
	}
	

	//Load sun effect
	if ( m_pParticleSysMgr )
	{
		D3DXVECTOR3 vPos;
		vPos.x = GetDevice()->GetWidth() - 150;
		vPos.y = 130;
		vPos.z = 0;

		if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\Sun" ) )
		{
			m_pParticleSysMgr->LoadOneParticleSystem( "LoginEffect\\Sun", vPos );
			if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\Sun" ) )
				return E_FAIL;
			m_pParticleSysMgr->GetActiveParticleSystem()->UseCharacterOffset( false );
			m_pParticleSysMgr->GetActiveParticleSystem()->SetLoopPlay( false );
			m_pParticleSysMgr->GetActiveParticleSystem()->Play();		//m_pParticleSysMgr->GetActiveParticleSystem()->GetSysLife()
		}

		if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\SunRay" ) )
		{
			m_pParticleSysMgr->LoadOneParticleSystem( "LoginEffect\\SunRay",vPos );
			if ( NULL == m_pParticleSysMgr->FindParticleSys( "LoginEffect\\SunRay" ) )
				return E_FAIL;
			m_pParticleSysMgr->GetActiveParticleSystem()->UseCharacterOffset( false );
			m_pParticleSysMgr->GetActiveParticleSystem()->Play();
		}
	}

	return S_OK;

	m_loginCloud = new CEffectCloud;
	m_loginCloud->SetCloudVolume( 0 );
	m_loginCloud->SetColor( 0xffffffff );
	m_loginCloud->SetDensity( 150 );
	m_loginCloud->SetUpdateFrequency( 200 );
	m_loginCloud->SetPosition( GetDevice()->GetWidth()-1024, 0 );
	m_loginCloud->SetScaleRate( 4.0f );

	if ( m_loginCloud->InitDevice() )
		m_loginCloud->Play();
	else
		return E_FAIL;
	
	return S_OK;

	FUNCTION_END;
}

/**
 * \brief 初始化粒子系统设备
 * 
 * 
 * 
 * \return 成功返回S_OK 否则返回E_FAIL
 */
HRESULT CGameAppation::InitParticleDevice()
{
	FUNCTION_BEGIN;

	if ( m_pParticleSysMgr )
		return m_pParticleSysMgr->InitAllParticleSysDevice();

	return E_FAIL;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGameAppation::ApplyConfig()
{
	FUNCTION_BEGIN;

	GetSoundManager()->SetEnable(m_Config.bMainSound);
	GetSoundManager()->SetVolume(m_Config.nMainSound/100.f);

	GetSoundManager()->SetEnableOfType(SoundType_Scene,m_Config.bSceneSound);
	GetSoundManager()->SetEnableOfType(SoundType_Circumstance,m_Config.bCircumstanceSound);
	GetSoundManager()->SetEnableOfType(SoundType_Music,m_Config.bBackgroundSound);

	GetSoundManager()->SetVolumeOfType(SoundType_Scene,m_Config.nSceneSound/100.f);
	GetSoundManager()->SetVolumeOfType(SoundType_Circumstance,m_Config.nCircumstanceSound/100.f);
	GetSoundManager()->SetVolumeOfType(SoundType_Music,m_Config.nBackgroundSound/100.f);
	
	CAppation::ApplyConfig();

	Engine_GetCursor()->ResetCursor();
	FUNCTION_END;
}

class CLogingDlg{
public:
	CLogingDlg()
	{
		ShowLoadingDlg();
	}
	void Close()
	{
		CloseLoadingDlg();
	}

	~CLogingDlg()
	{
		CloseLoadingDlg();
	}
};
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
#define SOUND_MESSAGE (WM_USER + 207)
static CThread g_hookThread;
bool CGameAppation::InitInstance()
{
	FUNCTION_BEGIN;


	extern CGameScene* g_pGameScene;
	extern CClient* g_pClient;
	g_pGameScene = new CGameScene;
	g_pClient = new CClient;

	CLogingDlg loginDlg;

	LoadConfig();

	CHECK_DEBUGGER_INIT

	if(!CAppation::InitInstance())
	{
		return false;
	}

	//Engine_WriteLog("CAppation::InitInstance() succeeded\n");

	GetSoundManager()->Create(m_hWnd,SOUND_MESSAGE);
	GetSoundManager()->SetFilePack("data\\sound.pak");

	GetSoundManager()->SetEnable(m_Config.bMainSound);
	GetSoundManager()->SetVolume(m_Config.nMainSound/100.f);

	GetSoundManager()->SetEnableOfType(SoundType_Scene,m_Config.bSceneSound);
	GetSoundManager()->SetEnableOfType(SoundType_Circumstance,m_Config.bCircumstanceSound);
	GetSoundManager()->SetEnableOfType(SoundType_Music,m_Config.bBackgroundSound);

	GetSoundManager()->SetVolumeOfType(SoundType_Scene,m_Config.nSceneSound/100.f);
	GetSoundManager()->SetVolumeOfType(SoundType_Circumstance,m_Config.nCircumstanceSound/100.f);
	GetSoundManager()->SetVolumeOfType(SoundType_Music,m_Config.nBackgroundSound/100.f);

	//Engine_WriteLog("CParticleSysManager create begin......\n");
	#ifdef PARTICLE_SYSTEM
		m_pParticleSysMgr = new CParticleSysManager;
		m_pParticleSysMgr->SetUsageType( PSM_USE_GAME );
	#endif

	#ifdef WEATHER_EFFECT
		m_pWeatherMgr = new CWeatherMgr;
	#endif

    GetDevice()->FindResource("data\\footprint.gl",0,Usage_Interface);
	GetDevice()->FindResource("data\\interfaces.gl",0,Usage_Interface);
	GetDevice()->FindResource("data\\interfaces2.gl",0,Usage_Interface);
	GetDevice()->FindResource("data\\interfaces3.gl",0,Usage_Interface);
	GetDevice()->FindResource("data\\interfaces4.gl",0,Usage_Interface);
	GetDevice()->FindResource("data\\interfaces5.gl",0,Usage_Interface);
	GetDevice()->FindResource("data\\interfaces6.gl",0,Usage_Interface);
	GetDevice()->FindResource("data\\interfaces7.gl",0,Usage_Interface);
	GetDevice()->FindResource("data\\interfaces_cz.gl",0,Usage_Interface);
	GetDevice()->FindResource("data\\interfaces1.gl",0,Usage_Interface);
	GetDevice()->FindResource("data\\items.gl",IBitmapList::flag_AlwaysOpen ,Usage_Interface);
	GetDevice()->FindResource("data\\cursor.gl",0,Usage_Interface);
	GetDevice()->FindResource("data\\icons.gl",0,Usage_Interface);
	GetDevice()->FindResource("data\\login.gl",0,Usage_Interface);
	GetDevice()->FindResource("data\\logo.gl",0,Usage_Interface);
	GetDevice()->FindResource("data\\other.gl",IBitmapList::flag_AlwaysOpen ,Usage_Interface);
	GetDevice()->FindResource("data\\Minimap.gl",0 ,Usage_Interface);
	GetDevice()->FindResource("data\\MallUI.gl",0,Usage_Interface); //商城新页面读取pak
	GetGameGuiManager()->InitCreate( );
	GetGameGuiManager()->Init( );
	GetGameCursor( )->SetCursor(CCursor::CursorType_Normal,eCursorNormal);
	if(!CreateGameSystem())	return false;	

	LoadFaceInfo();
	GetDevice()->SetPicCharInfo(&m_aPicCharInfo[0],m_aPicCharInfo.size());

	loginDlg.Close();

	ShowWindow(m_hWnd,SW_SHOW);
	UpdateWindow(m_hWnd);

	/*if(!SetTimer(m_hWnd,1,2000,NULL))
	{
#if defined _DEBUG || defined RELEASE_TEST
		Engine_WriteLog("设置定时器消息失败！\n");
#endif
	}*/

	//Engine_WriteLog("程序初始化完成\n");

	//SetTimer(m_hWnd,1,2000,NULL);
	void BeginHookThread();
	BeginHookThread();
	return true;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
int  CGameAppation::ExitInstance()
{
	FUNCTION_BEGIN;

#if defined _DEBUG || defined RELEASE_TEST || defined _AUTOPLAY
#else
   //GotoURL(m_Config.szHomeURL,SW_SHOWMAXIMIZED);	
#endif

	void EndHookThread();
	EndHookThread();
	UninitGame();

	DestroyGameSystem();

	GetSoundManager()->Destroy();

	extern CGameScene* g_pGameScene;
	extern CClient* g_pClient;

	SAFE_DELETE(g_pGameScene);
	SAFE_DELETE(g_pClient);
	return CAppation::ExitInstance();

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param uMsg : 描述
 * \param wParam : 描述
 * \param lParam : 描述
 * \return 返回值的描述
 */
LRESULT CGameAppation::MsgProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	FUNCTION_BEGIN;

	GetDevice()->MsgProc(uMsg,wParam,lParam);

	CHECK_DEBUGGER_INIT

	LPARAM OldLParam = lParam;
	if(uMsg == WM_MOUSEMOVE && (wParam & MK_LBUTTON)){
		POINT mousePoint;
		mousePoint.x = (short)(lParam & 0xffff);
		mousePoint.y = (short)(lParam >> 16);
		if(mousePoint.x < 0)
			mousePoint.x = 0;
		if(mousePoint.y < 0)
			mousePoint.y = 0;
		if(mousePoint.x >= GetDevice()->GetWndWidth())
			mousePoint.x = GetDevice()->GetWndWidth() - 1;
		if(mousePoint.y >= GetDevice()->GetWndHeight())
			mousePoint.y = GetDevice()->GetWndHeight() - 1;
		if(lParam != MAKELPARAM(mousePoint.x,mousePoint.y)){
			lParam = MAKELPARAM(mousePoint.x,mousePoint.y);
			ClientToScreen(GetDevice()->GetHWnd(),&mousePoint);
			SetCursorPos(mousePoint.x,mousePoint.y);
		}
	}
	switch(uMsg)
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
		{
			POINT mousePoint;
			mousePoint.x = (short)(lParam & 0xffff);
			mousePoint.y = (short)(lParam >> 16);
			if(mousePoint.x<0)
			{
				int iiiii=0;
			}
			GetDevice()->ConverMousePoint(mousePoint);
			lParam = ( ((DWORD)(short)mousePoint.y) << 16) | ( (WORD)(short)mousePoint.x );
		}
		break;
	}

	//Begin yours
	if ( (uMsg == WM_KEYUP) && (wParam == 'D') )
		g_bSitDownHotKeyActive = true;

	if( uMsg == WM_KEYDOWN)
		int iii = 0;
	if ((uMsg == WM_KEYUP)&&(wParam == VK_SNAPSHOT))
		return SaveBackBufferToFile();

	if(Engine_GetCursor()->MsgProc(uMsg,wParam,lParam))
		return 1;
	if(GetSoundManager()->MsgProc(uMsg,wParam,lParam))
		return 1;

	//ESC 控制1-15类型窗口同时关闭
	//m_bESCHook = false;//false 代表不hook ESC,只在1-15类型的窗口存在的时候hook
	if ( (uMsg == WM_KEYDOWN) && (wParam == VK_ESCAPE) && m_bESCHook)
	{		
		GetGameGuiManager()->CloseAll1_15TypeWnd();
		return 1;
	}

	if(GetGuiManager()->MsgProc(m_hWnd,uMsg,wParam,lParam))
	{
		if(GetKeyState(VK_MENU) >= 0) 
			return 1;
	}	

	if(GetGameState()->IsState(eGameState_Run))
	{
		if(GetScene()->MsgProc(uMsg,wParam,lParam))
			return 1;
	} 

	switch(uMsg)
	{
	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
		GetGameBackMusic()->SetPause(wParam == WA_INACTIVE);		
		break;
	case WM_TIMER:
		if(wParam == 1)
		{
			CTimer::UpdateCpuFrequency();
		}
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		break;
#ifdef _USE_SHARE_MEMORY_PAGE
	case WM_COPYDATA:
		{
		       PCOPYDATASTRUCT   pcds = (PCOPYDATASTRUCT)lParam;
		       switch(pcds->dwData)
		       {
		       case ACTION_FIND_WG:
			       {
				       const ResultData *pData  = (const ResultData * )pcds->dwData;
				       int ii = 0;
			       }
		       }
		}
		break;
#endif
#ifdef _AUTOPLAY
	case WM_KEYDOWN:
		{
			if(wParam == VK_F12){
				gAutoGameConfig.showConfigWnd();
			}
		}
		break;
#endif
	}
	return CAppation::MsgProc(uMsg,wParam,OldLParam);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param fElapsedTime : 描述
 * \return 返回值的描述
 */
void CGameAppation::RefreshFrame(float fElapsedTime)
{
	FUNCTION_BEGIN;

	GetDevice()->Run();
	Engine_GetCursor()->Run(fElapsedTime);
	GetSoundManager()->Run( fElapsedTime );
	GetGameBackMusic()->Run(fElapsedTime);

	HandleCommand();

	switch(GetClient()->GetState())
	{
	case net::CSocket::eSocketState_None:
		break;
	case net::CSocket::eSocketState_Connection:
		break;
	case net::CSocket::eSocketState_Disconnect:	// 断线了或用户退出游戏
		switch(GetGameState()->GetState())
		{
		case eGameState_Null:
			break;
		case eGameState_Login:
		case eGameState_LoginOK:
			{// 登陆时异常中断
				GetGameState()->SetState(eGameState_Null);
				bool bSelfDisconnect = GetClient()->IsSelfDisconnect();

				// 回到登陆界面,重新尝试连接
				GetClient()->m_bLoginRecv = true;
				GetClient()->m_bLoginSend = true;
#ifndef _LOGIN_SERVER_MANUAL
				ConnectionLoginServer();
#endif 

				extern void ReturntoLogin();
				ReturntoLogin();

				CGuiMessageBox * pMsgBox = NULL;
				if(!bSelfDisconnect)
				{ // 只有非主动退出才提示

					static DWORD itemdata = 5000;
				 
					if(g_pszLoginErrorMessage)
						pMsgBox = GetGameGuiManager()->AddMessageBox(g_pszLoginErrorMessage,eAction_ConnectLoginFailed,(void *)itemdata);
					else
						pMsgBox = GetGameGuiManager()->AddMessageBox("与服务器失去连接！",eAction_ConnectLoginFailed,(void *)itemdata);

					GetGuiManager()->RequestFocus((CGuiDialog *)pMsgBox);
				}
				g_pszLoginErrorMessage = NULL;
			}
			break;
		case eGameState_Run:
			{   
#ifdef ___AUTO___NET___CONNECT___
				// 游戏中非正常退出，返回到登陆界面
				if(CNetAutoConnect::Instance()->IsNeedConnOrConning())
				{
                                         CNetAutoConnect::Instance()->TryBeginAutoConnect();
				}
				else
				{
					bool bSelfDisconnect = GetClient()->IsSelfDisconnect();
					if(!bSelfDisconnect)
					{
						//if(GetSystem()->m_bAutoConnect)
						{
							CNetAutoConnect::Instance()->TryBeginAutoConnect();
						}						
					}
					else
					{
						UninitGame();
						InitGame();
						if(!bSelfDisconnect)
						{// 只有非主动退出才提示
							if( GetGameState()->IsState(eGameState_Login)) 
							{ // 已经重新连接上,仅仅提示一下
								GameMessageBox("与服务器失去连接！");
							}
							else
							{ // 断线重新连接也失败，则弹出一个将关闭的框
								GetGameGuiManager()->AddMessageBox("与服务器失去连接！",eAction_ConnectLoginFailed); 
							}					
						}
						GetGameState()->SetState(eGameState_Null);
					}			
				}					
#else
				bool bSelfDisconnect = GetClient()->IsSelfDisconnect();
				UninitGame();
				InitGame();
				if(!bSelfDisconnect)
				{// 只有非主动退出才提示
					if( GetGameState()->IsState(eGameState_Login)) 
					{ // 已经重新连接上,仅仅提示一下
						GameMessageBox("与服务器失去连接！");
					}
					else
					{ // 断线重新连接也失败，则弹出一个将关闭的框
						GetGameGuiManager()->AddMessageBox("与服务器失去连接！",eAction_ConnectLoginFailed); 
					}					
				}
				GetGameState()->SetState(eGameState_Null);		
#endif 
			}
			break;
		}
		break;
	}

	if(GetGameState()->IsState(eGameState_Run) && GetScene()->GetWidth())
	{
		static DWORD s_fTime = 0;
		DWORD fCurTime = xtimeGetTime();

		if(s_fTime != 0)
		{
			DWORD fTime = (fCurTime - s_fTime);
			s_fTime = fCurTime;
			GetScene()->Run( fTime);
		}
		else
		{
			s_fTime = xtimeGetTime();
			GetScene()->Run( 0 );
		}
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
bool CGameAppation::AforeDraw()
{
	FUNCTION_BEGIN;
	if(/*(!GetDevice()->IsActive() && GetDevice()->GetFlags() & IDevice::DEVICE_FULLSCREEN) || */IsIconic(m_hWnd))
		return false;

	if(GetGameState()->IsState(eGameState_Run))
	{
		CLordStrikeManager::GetSingleton()->OnUpdate();

		if(GetGameGuiManager()->m_guiMiniMap)
			GetGameGuiManager()->m_guiMiniMap->Update();

		if ( !GetScene()->IsDarkScene() )
		{
			GetScene()->m_bLightMap = false;
			if(GetLightMap()->IsEnabled() && GetScene()->RefreshLightInfo())
			{
				GetLightMap()->BeginLight();
				GetScene()->AddLight();
				GetLightMap()->GenerateLightMap(GetScene()->GetLightColor(),GetDevice()->GetWidth(),GetDevice()->GetHeight());
			}
		}
		else
		{
			GetScene()->m_bLightMap = true;
			if ( !GetLightMap()->IsEnabled() )
			{
				GetLightMap()->SetEnabled( true );
			}
			GetLightMap()->BeginLight();
			
			CMainCharacter* pMainCharacter = GetScene()->GetMainCharacter();
			if ( pMainCharacter && pMainCharacter->GetInterface() )
			{
				stPointI ptMapOffset = GetScene()->GetPixelOffset();
				ptMapOffset.y += 40;
				pMainCharacter->GetInterface()->AddLight( ptMapOffset );
				GetLightMap()->AddLight(pMainCharacter->GetPos()-ptMapOffset,700,-1);
			}
			GetLightMap()->GenerateLightMap(0xff000000,GetDevice()->GetWidth(),GetDevice()->GetHeight());
		}
	}

	#ifdef PARTICLE_SYSTEM
		if ( m_pParticleSysMgr )
			m_pParticleSysMgr->Update( Engine_GetTime() );
	#endif

	#ifdef WEATHER_EFFECT
		if ( m_pWeatherMgr )
			m_pWeatherMgr->Update();
	#endif

	//update login cloud effect
	if ( GetGameGuiManager()->m_guiLogin && GetGameGuiManager()->m_guiLogin->IsVisible() )
	{
		if ( GetGameApplication()->m_loginCloud )
			GetGameApplication()->m_loginCloud->Update();
	}

	return CAppation::AforeDraw();

	FUNCTION_END;
}

void CGameAppation::DrawWeatheAndParticleEffect(float fElapsedTime)
{
	FUNCTION_BEGIN;

	#ifdef PARTICLE_SYSTEM
		if ( m_pParticleSysMgr )
			m_pParticleSysMgr->Render();
	#endif

	#ifdef WEATHER_EFFECT
		if ( m_pWeatherMgr )
			m_pWeatherMgr->Render( fElapsedTime );
	#endif

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param fElapsedTime : 描述
 * \return 返回值的描述
 */
void CGameAppation::Draw(float fElapsedTime)
{
	FUNCTION_BEGIN;
	
	GetServerKillProcess()->OnRender(fElapsedTime);

	

	//sky 刷新小帮手控件
    GetGameGuiManager()->m_guiMain->AddbangsBtn(true);
	
	GetGameGuiManager()->m_guiAutoAttackDlg->OnRender_AutoAtt(fElapsedTime);////////star100515


	if(GetGameState()->IsState(eGameState_Run))
	{
		if(GetScene()->GetWidth())
			GetScene()->Draw(0xffffffff);

		if(!GetScene()->m_bSendLoadOK)
		{
			// 发送地图装载ＯＫ消息
			static int s_iFrame = 0;
			if(s_iFrame > 30) 
			{
				stLoadMapOKDataUserCmd cmd;
				SEND_USER_CMD(cmd);
				GetScene()->m_bSendLoadOK = true;
				s_iFrame = 0;
			}
			else
			{
				s_iFrame ++;
			}
		}

		DrawWeatheAndParticleEffect( fElapsedTime );

		//Render gui
		GetGuiManager()->Render(fElapsedTime);

		// Render timers
		GetTimerDisplayMgr().ShowTimers(fElapsedTime);
	}
	else
	{
		//Render gui
		GetGuiManager()->Render(fElapsedTime);
	}

#if defined _DEBUG || defined RELEASE_TEST
	extern size_t	gSocketRecvNum;
	extern size_t  gSocketSendNum;
	
	static size_t sRecvNum0 = 0;
	static size_t sSendNum0 = 0;
	static DWORD sSocketTime0 = 0;
	static size_t sCurRecvNumPreSecond = 0;
	static size_t sCurSendNumPreSecond = 0;
	static float sConnectTotalTime = 0.0f;
	static size_t nAveRecvNumPreSecond = 0;
	static size_t nAveSendNumPreSecond = 0;
	DWORD curTime = xtimeGetTime();
	
	if(GetClient()->GetState() == net::CSocket::eSocketState_Connection){
		sConnectTotalTime += fElapsedTime;
		nAveRecvNumPreSecond = gSocketRecvNum / sConnectTotalTime;
		nAveSendNumPreSecond = gSocketSendNum / sConnectTotalTime;
	}

	if(curTime - sSocketTime0 > 1000){
		sSocketTime0 = curTime;
		sCurRecvNumPreSecond = (gSocketRecvNum - sRecvNum0);
		sCurSendNumPreSecond = (gSocketSendNum - sSendNum0);
		sRecvNum0 = gSocketRecvNum;
		sSendNum0 = gSocketSendNum;
	}

	stPointI pt = Engine_GetCursor()->GetPosition();
	char szTmp[256];
	if(GetDevice()->GetWidth() > 800)
	{
		sprintf(
			//szTmp,"%d:%d,FPS:%d/%d,三角形数:%d,显存:%dK/%dK,内存:%dK,LAS %dK,SQS %dK,NFAS %dK",
			szTmp,"%d:%d,FPS:%d/%d,三角形数:%d,显存:%dK/%dK,内存:%dK,LAS %dK,SQS %dK",
			//szTmp,"%d:%d,FPS:%.2f/%g,三角形数:%d,显存:%.2f/%.2f,内存:%.2f",
			pt.x,pt.y,
			Engine_GetFps()->GetFPS(),Engine_GetFps()->GetLimitFPS(),
			GetDevice()->GetTriCount(),
			GetDevice()->GetUseTextureMemSize()/1024,GetDevice()->GetTotalTextureMemSize()/1024,
			GetDevice()->GetUseResMemorySize()/1024,
			std::gLookasideAllocSize/1024,
			//GetClient()->getStreamQueueMemorySize()/1024
			CStreamQueue::sAllocSize/1024/*,
			std::gNoFreeAllocSize/1024*/
			);
		pt.x = 300;
	}else{
		sprintf(
			szTmp,"%d:%d,FPS:%d/%d,三角形数:%d,显存:%dK/%dK,内存:%dK",
			//szTmp,"%d:%d,FPS:%.2f/%g,三角形数:%d,显存:%.2f/%.2f,内存:%.2f",
			pt.x,pt.y,
			Engine_GetFps()->GetFPS(),Engine_GetFps()->GetLimitFPS(),
			GetDevice()->GetTriCount(),
			GetDevice()->GetUseTextureMemSize()/1024,GetDevice()->GetTotalTextureMemSize()/1024,
			GetDevice()->GetUseResMemorySize()/1024
			);
		pt.x = 200;
	}
	pt.y = 5;
	GetDevice()->DrawString(szTmp,pt,0xffffffff,FontEffect_Border);

	if(GetDevice()->GetWidth() > 800){
		sprintf(szTmp,"发送流量 %.3fkbit/%.3fkbit，接收流量 %.3fkbit/%.3fkbit",
			sCurSendNumPreSecond * (8 /1000.0f),nAveSendNumPreSecond * (8 /1000.0f),
			sCurRecvNumPreSecond * (8 /1000.0f),nAveRecvNumPreSecond * (8 /1000.0f));

		pt.x = 300;
		pt.y = 5 + 13;
		GetDevice()->DrawString(szTmp,pt,0xffffffff,FontEffect_Border);
	}
#endif

#ifdef PARTICLE_SYSTEM
		if ( 0 )		//GetParticleSysMgr()
		{ 
			char szParticleNum[100];
			sprintf( szParticleNum, "Current Alive Particle Num = %d", GetParticleSysMgr()->GetTotalParticleNum() );
			GetDevice()->DrawString( szParticleNum, stPointI(450,15), 0xffff0000, FontEffect_Border );
		}
#endif
#ifndef _AUTOPLAY
	GetClientKillProcess()->OnRender(fElapsedTime);
#endif
	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGameAppation::Exit()
{
	FUNCTION_BEGIN;

	PostMessage(m_hWnd,WM_CLOSE,0,0);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pT : 描述
 * \param size : 描述
 * \return 返回值的描述
 */
template <typename T>
std::string Array_2_String(T *pT,size_t size)
{
	if(!pT) return "";
	std::string strRes = "";
	for(int i = 0;i < size; ++i)
	{
		char buf[10];
		sprintf(buf,"%x,",pT[i]);
		strRes += buf;
	}
	return strRes;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param serverData : 描述
 * \return 返回值的描述
 */
void CGameAppation::SetServerConfigData( stSystemSettings * serverData )
{
	FUNCTION_BEGIN;

	m_Config.serverData = *serverData;

	if(GetGameGuiManager()->m_guiSystem)
	{
		GetGameGuiManager()->m_guiSystem->SetServerData(m_Config.serverData);
	}

	//如果关闭了小地图就不显示,打开了才显示
	bool bShowMiniMap = m_Config.bSystemSetted(USER_SETTING_SHOW_MIMIMAP);
	if(GetGameGuiManager()->m_guiMiniMap)
	{
		GetGameGuiManager()->m_guiMiniMap->SetVisible(bShowMiniMap);
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGameAppation::SaveServerConfigData()
{
	FUNCTION_BEGIN;

	m_Config.UpdateSystemSettingOnServer();

	FUNCTION_END;
}

extern void DecryptHeaderData(BYTE* inData,size_t len,const BYTE* key1,const BYTE * key2,const BYTE* key3);

inline HANDLE InitAppMutex()
{
	const char * szMutexName = "xx_client";
	HANDLE hMutex = CreateMutex(NULL,FALSE,szMutexName);
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND  hWnd = FindWindow(c_szWindowClass,c_szWindowTitle);
		if(hWnd)
			SetForegroundWindow(hWnd);
		CloseHandle(hMutex);
		return NULL;
	}
	return hMutex;
}

inline bool InitAppCommandLine()
{
	if(__argc != 5 && __argc != 2) 
		return false;
	return true;
}

// 2010 8
bool AddLine( char* p )
{
	std::string s = p;
	int i = 0;
	char c = 92;
	std::string s2 = "";

	s2 += c;

	while( 1 )
	{
		i = ( int )s.find( s2, i );

		if( i < 0 )
		{
			break;
		}

		s.insert( i, s2 );
		i += 2;
	}

	s += s2;

	sprintf( p, s.c_str() );

	return true;
}

bool EditReg()
{
	char cPath[ MAX_PATH + 1 ] = "";

	::GetCurrentDirectory( MAX_PATH, cPath );

	AddLine( cPath );

	std::string s = "";
	char c = 34;

	FILE* p = fopen( "data/ztgame.reg", "w" );

	s += c;
	s += cPath;
	s += "\\ztgame.dat";
	s += c;
	s += "=";
	s += c;
	s += "WIN95";
	s += c;

	std::string s2 = "Windows Registry Editor Version 5.00";
	
	s2 += "\n";
	s2 += "[HKEY_CURRENT_USER\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers]";
	s2 += "\n";
	s2 += s;

	fwrite( s2.c_str(), s2.size(), 1, p );

	fclose( p );

	s = "regedit /s ";
	s += cPath;
	s += "data/ztgame.reg";

	::WinExec( s.c_str(), SW_HIDE );

	return true;
}

int  main( int argc, char *argv[] )
{

	// 2010 8
	//EditReg();

	CHECK_DEBUGGER_INIT;
//#if defined _DEBUG || defined RELEASE_TEST || defined _AUTOPLAY
	
	if(IsDebuggerPresent())
		return GameMain(GetModuleHandle(NULL),NULL,NULL);

	__try
	{
		return GameMain(GetModuleHandle(NULL),NULL,NULL);
	}
	__except( SimpleReportError( GetExceptionInformation(),GetExceptionCode()) )
	{
		return -1;
	}

//#else

	//CHECK_DEBUGGER_INIT;
	//
	//HANDLE hMutex;
	//
	//if(InitAppCommandLine() == false)
	//	return 0;

	//if(NULL == (hMutex = InitAppMutex()))
	//	return 0;

	//if(IsDebuggerPresent())
	//	return 0;


	//__try{
	//	GameMain(GetModuleHandle(NULL),NULL,NULL);
	//}
	//__except(SendReportError(GetExceptionInformation(),GetExceptionCode()))
	//{
	//}

	//CloseHandle(hMutex);

	//return 0;
//#endif
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param hInstance : 描述
 * \param hPrevInstance : 描述
 * \param lpCmdLine : 描述
 * \param nCmdShow					 : 描述
 * \return 返回值的描述
 */
int __stdcall WinMain(
					   HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPSTR lpCmdLine,
					   int nCmdShow
					   )
{
	CHECK_DEBUGGER_INIT
	return main(0,NULL);
	//return GameMain(hInstance,lpCmdLine,nCmdShow);

}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGameAppation::LoadFaceInfo()
{
	FUNCTION_BEGIN;

	static std::vector<stPicCharInfo> aPic;
	TiXmlDocument doc;
	//////////////////////////////////////////////////
	Stream* pStream = OpenPackFileForRead("data\\datas.pak","datas\\face.xml");
	if(!pStream)
	{
		Engine_WriteLog("装载 face.xml 失败\n");
		return ;
	}
	if(!LoadXMLFromStream(pStream,doc))
	{
		ClosePackFileForRead(pStream);
		Engine_WriteLog("解析 face.xml 失败\n");
		return;
	}
	//doc.LoadFile("data\\face.xml");  //soke 原版
	//////////////////////////////////////////////////////
	TiXmlElement* faces = doc.FirstChildElement("faces");
	if(!faces) return;
	TiXmlElement* face = faces->FirstChildElement("face");

	stResourceLocation rl;
	rl.SetFileName("data\\icons.gl");
	rl.group = 2;
	while(face)
	{
		stPicCharInfo st;
		int id,pic;
		TiXmlElement* esc = face->FirstChildElement("esc");
		if(face->QueryIntAttribute("id",&id) != TIXML_SUCCESS)
			continue;
		if(face->QueryIntAttribute("pic",&pic) != TIXML_SUCCESS)
			continue;
		rl.frame = pic;
		st.pBmps = GetDevice()->FindBitmaps(&rl);
		if(st.pBmps && st.pBmps->GetFrameCount())
		{
			st.size.cx = st.pBmps->GetBitmap(0)->GetWidth();
			st.size.cy = st.pBmps->GetBitmap(0)->GetHeight();
		}
		else
		{
			st.size.cx = 12;
			st.size.cy = 12;
		}
		st.scale = stPointF(1,1);
		if(id >= m_aPicCharInfo.size())
			m_aPicCharInfo.resize(id+1);
		m_aPicCharInfo[id] = st;
		while(esc)
		{
			const char* value = XML_GetNodeText(esc);
			if(value)
			{
				//m_mapPicChar[value] = id;
				m_aPicChar.push_back(stPicCharEsc());
				strncpy(m_aPicChar.back().esc,value,32);
				m_aPicChar.back().esc[31] = 0;
				m_aPicChar.back().id = id;
			}
			esc = esc->NextSiblingElement("esc");
		}
		face = face->NextSiblingElement("face");
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param str : 描述
 * \param ch : 描述
 * \param charNum : 描述
 * \return 返回值的描述
 */
bool CGameAppation::ConverPicChar(const char* str,WORD& ch,int & charNum)
{
	FUNCTION_BEGIN;

	int len1 = strlen(str);
	for(size_t i=0;i<m_aPicChar.size();++i)
	{
		stPicCharEsc & st = m_aPicChar[i];
		int len = strlen(st.esc);
		if( len1 >= len )
		{
			const char* p = str + len1 - len;
			if( strcmp(st.esc,p) == 0 )
			{
				if( p-1 >= str && *(p-1) == '/'&& *p == '/' )
				{
					return false;
				}
				ch = st.id + c_nPicCharBase;
				WCHAR wsz1[256];
				charNum = my_mbstowcs(wsz1,st.esc,-1);
				return true;
			}
		}
	}
	return false;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param str : 描述
 * \param strContext : 描述
 * \param strSeparator : 描述
 * \param index : 描述
 * \return 返回值的描述
 */
bool CGameAppation::SearchPicChar(char* str,char* strContext,const char* strSeparator,int index)
{
	FUNCTION_BEGIN;

	bool bFind=false;
	if (strContext==NULL||str==NULL||strSeparator==NULL)
		return bFind;
	

	for (int i=0;i<m_aPicChar.size();i++)
	{
		if (m_aPicChar[i].id==index)
		{
			if (bFind)
			{
				strncat(strContext,strSeparator,strlen(strSeparator));
				strncat(strContext,m_aPicChar[i].esc,strlen(m_aPicChar[i].esc));
			}
			else
			{
				wcsncpy((wchar_t *)str,(const wchar_t *)m_aPicChar[i].esc,sizeof(str));
				wcsncpy((wchar_t *)strContext,(const wchar_t *)m_aPicChar[i].esc,sizeof(strContext));
				bFind=true;
			}
		}		
	}
	return bFind;

	FUNCTION_END;
}

const char* CGameAppation::GetAutoReplyText( enumAutoReplyType type )
{
	if( type == AutoReply_UserDefine )
	{
		return m_Config.szAutoReply;
	}
	else if( type < AutoReply_UserDefine )
	{

		int size = sizeof(g_sAutoReplyCmd)/sizeof(stAutoReply);

		for ( int i = 0; i < size; i++ )
		{
			if( g_sAutoReplyCmd[i].type == type )
			{
				return g_sAutoReplyCmd[i].szReplyText;
			}
		}
	}
	return NULL;
}

void CGameAppation::SetAutoReplyType( enumAutoReplyType type )
{
	m_Config.autoReplyType = type;
	stAutoReplyUserCmd cmd;
	strncpy(cmd.text,GetAutoReplyText( type ),MAX_CHATINFO);
	SEND_USER_CMD( cmd );
}

