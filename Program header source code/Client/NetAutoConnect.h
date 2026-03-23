#pragma once


#define ZTGAME_VERSION  20250101

#ifdef _DEBUG
	#ifndef _AUTOPLAY
	#define _AUTOPLAY
	#endif
#endif

#ifdef _AUTOPLAY
	extern DWORD g_GameVersion;
	extern DWORD GetGameVersion();
	extern void SaveGameVersion();
	extern DWORD GetNewGameVersion(DWORD curVersion);
#endif

enum AutoConnect_State
{
	eState_NeedConn,
	eState_Connecting,
	eState_Null,
};

class CNetAutoConnect
{
public:
	CNetAutoConnect();

	void TryBeginAutoConnect();
	void EndAutoConnect();


	void LoginGameServer(stServerReturnLoginSuccessCmd * pCmd);

	bool IsNeedConnOrConning(){return m_State == eState_NeedConn || m_State == eState_Connecting;};

	static CNetAutoConnect * Instance();
	void   AutoEnterGS(int charNo);

	void BackUpLoginSelectCmd(stLoginSelectUserCmd * pCmd);


	std::string m_strAccount;

private:
	void ___ConnectToGameServer();

	DWORD m_dwStartActionTime;
protected:
	stServerReturnLoginSuccessCmd  m_BackCmd;
	stLoginSelectUserCmd	       m_BackLoginCmd;
	AutoConnect_State m_State;

	bool m_bAutoEnable;
};
