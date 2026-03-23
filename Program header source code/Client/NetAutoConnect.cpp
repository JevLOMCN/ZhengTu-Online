#include "public.h"
#include ".\gameappation.h"
#include ".\GameTime.h"
#include ".\gamestate.h"
#include ".\Game.h"
#include ".\NetAutoconnect.h"

#ifdef _AUTOPLAY
DWORD g_GameVersion = -1;
DWORD GetGameVersion()
{
	if(g_GameVersion == -1){
		CIni ini;
		ini.Open("config.ini");
		g_GameVersion = ini.GetInt("other","version",ZTGAME_VERSION);
		if(g_GameVersion<ZTGAME_VERSION)
			g_GameVersion = ZTGAME_VERSION;
	}
	return g_GameVersion;
}

void SaveGameVersion()
{
	CIni ini;
	ini.Open("config.ini");
	ini.SetInt("other","version",g_GameVersion);
	ini.Save("config.ini");
}

DWORD GetNewGameVersion(DWORD curVersion)
{
	int year,month,day;
	char szCurVersion[256];
	sprintf(szCurVersion,"%08d",curVersion);
	char szTemp[64];

	strncpy(szTemp,szCurVersion,4);
	szTemp[4] = 0;
	year = atol(szTemp);

	strncpy(szTemp,szCurVersion+4,2);
	szTemp[2] = 0;
	month = atol(szTemp);

	strncpy(szTemp,szCurVersion + 6,2);
	szTemp[2] = 0;
	day = atol(szTemp);

	__time64_t t;
	tm _t;
	memset(&_t,0,sizeof(_t));
	_t.tm_year = year - 1900;
	_t.tm_mon = month - 1;
	_t.tm_mday = day;
	t = _mktime64(&_t);
	t += (60 * 60 * 24);
	tm * pt = _localtime64(&t);
	if(pt){
		strftime(szCurVersion,sizeof(szCurVersion),"%Y%m%d",pt);
		return atol(szCurVersion);
	}
	return curVersion;
}

#endif

CNetAutoConnect * CNetAutoConnect::Instance()
{
	static CNetAutoConnect s_autonet;
	return &s_autonet;
}

CNetAutoConnect::CNetAutoConnect()
{ 
	m_dwStartActionTime = 0;
	m_State = eState_Null;
	m_bAutoEnable = false;
}

 
void CNetAutoConnect::TryBeginAutoConnect()
{
	m_State = eState_NeedConn;
      
	m_bAutoEnable = true;

        if(m_dwStartActionTime == 0 )
		m_dwStartActionTime = xtimeGetTime();
	else 
	{
		if( xtimeGetTime() - m_dwStartActionTime >= 1 * 60 * 1000)
		{
			m_dwStartActionTime = xtimeGetTime();
			m_State = eState_Connecting;
			___ConnectToGameServer();
			EndAutoConnect();
		}
	}
}

void CNetAutoConnect::EndAutoConnect()
{
 	if(GetClient()->GetState() == net::CSocket::eSocketState_Disconnect)
	{
		m_State =  eState_NeedConn;
	}
	else
	{
		m_State =  eState_Null;
		m_dwStartActionTime = 0;
	}
}

void CNetAutoConnect::___ConnectToGameServer()
{
	#ifdef _AUTOPLAY
		SaveGameVersion();
	#endif

	stServerReturnLoginSuccessCmd * cmd = &m_BackCmd;
	TRACE("帐号验证成功，准备连接到网关：[%s:%u]\n",cmd->pstrIP,cmd->wdPort);

	GetClient()->Destroy();

	if(!GetClient()->Connect(cmd->pstrIP,cmd->wdPort))
	{
		if ( GetGameApplication() )
			GetGameApplication()->UnInitLoginCloud();

		TRACE("连接到网关失败：[%s:%u]\n",cmd->pstrIP,cmd->wdPort);
		return;
	}

	GetGameTime()->m_bConnWay = false;
	BYTE* pKeyData = &cmd->key[cmd->keyOffset];

#ifdef _DEBUG
	TRACE("key : ");
	for(size_t i=0;i<8;++i)
	{
		TRACE("%d ",pKeyData[i]);
	}
	TRACE("\n");
#endif

	Client_SetKey(GetClient()->m_key,pKeyData,8);
	if(!g_bSinglePlayer)
	{
		GetClient()->m_bLoginRecv = false;
		GetClient()->m_bLoginSend = true;
	}

	//g_szServerKey = cmd->key;
	GetGameState()->SetState(eGameState_LoginOK);


	stUserVerifyVerCmd cmd0;
#ifdef _AUTOPLAY
	cmd0.version = GetGameVersion();
#else
	cmd0.version = ZTGAME_VERSION;
#endif
	SEND_USER_CMD(cmd0);
	 

	stPasswdLogonUserCmd cmd1;
	cmd1.dwUserID = cmd->dwUserID;
	cmd1.loginTempID = cmd->loginTempID;
	// modified 2005-8-4 10:07
	strncpy(cmd1.pstrName,CAccoutSaveInfo::GetSingleton()->GetUserID().c_str(),sizeof(cmd1.pstrName));
	cmd1.pstrName[sizeof(cmd1.pstrName)-1] = 0;
	strncpy(cmd1.pstrPassword,CAccoutSaveInfo::GetSingleton()->GetUserPass().c_str(),sizeof(cmd1.pstrPassword));
	cmd1.pstrPassword[sizeof(cmd1.pstrPassword)-1] = 0;

	SEND_USER_CMD(cmd1);
	 
	GetClient()->m_bLoginSend = false;

}

void CNetAutoConnect::LoginGameServer(stServerReturnLoginSuccessCmd * pCmd)
{

	stServerReturnLoginSuccessCmd* cmd = (stServerReturnLoginSuccessCmd*)pCmd;

	memcpy(&m_BackCmd,cmd,sizeof(stServerReturnLoginSuccessCmd));

	___ConnectToGameServer();
}

void CNetAutoConnect::AutoEnterGS(int charNo)
{
	if( !m_bAutoEnable) return ;

	assert(charNo == 0 || charNo == 1);

	stLoginSelectUserCmd cmd;
	memcpy(&cmd,&m_BackLoginCmd,sizeof(stLoginSelectUserCmd));
	cmd.charNo = charNo;

	SEND_USER_CMD(cmd);

	m_bAutoEnable = false;
}

void CNetAutoConnect::BackUpLoginSelectCmd(stLoginSelectUserCmd * pCmd)
{
       memcpy(&m_BackLoginCmd,pCmd,sizeof(stLoginSelectUserCmd));
}