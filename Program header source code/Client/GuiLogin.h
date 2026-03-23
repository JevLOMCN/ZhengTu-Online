/**
*\file		GuiLogin.h
*\version	$Id$
*\author	
*\brief	    登陆对话框
*
* 
*
*/

#pragma once
#include "..\gui\include\guidialog.h"


class CPixelContext
{
	static BOOL m_bFirst;
	static BOOL m_bIs800By600;
	static BOOL IsLogin800By600Internal();
	static stPointF m_Scale;
public:
	static BOOL IsLogin800By600();
	static stPointF & GetRenderScale();
	static void ClearFirstFlag();
};

//class CTextEffect
//{
//public:
//	CGuiControl * m_pControl;
//	BOOL  bIsPlaying;
//	POINT ptSrcPos,ptDstPos;
//	POINT ptMid;
//	float   fRunTime;
//	float   fTotalTime;
//	int dx,dy;
//	BOOL    bLoopAni;
//	CTextEffect();
//	void Release();
//	void BeginAni(const float & TotalTime,POINT ptDst,bool bLoop = false);
//	void UpdateAni(float fElaspedTime);
//};


class CLogoAni
{
public:
	CLogoAni();
	void  CreateAnimation();
	void  RenderAnimation(POINT pt);
	void RenderBackAnimation(POINT pt);
	void Release();
private:

	CAnimation m_AniFloor;		// 地板
	CAnimation m_AniPP;		// 瀑布
	CAnimation m_AniBannerHorse;	// 旗马
	CAnimation m_AniMenHorse;	//人马

	BOOL m_bCreateFloorOk; 
	BOOL m_bCreatePPOk;
	BOOL m_bCreateBannerHorseOk;
	BOOL m_bCreateMenHorseOk;

	CAnimation m_AniBack;
	BOOL m_bBackAnimation;

	BOOL bInited;
};

extern CLogoAni * GetLogoAni();

class CGuiLogin :
	public CGuiDialog
{
	BOOL m_bLogining;
public:
	
	CGuiLogin( void );

	BOOL IsLogining(){return m_bLogining;};
	void SetLogining(BOOL bLogining){m_bLogining = bLogining;};
	
	void OnCreate();
	void OnClose();
	//bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnInitScript();
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void RenderBackground(float fElapsedTime);
	HRESULT OnRender(float fElapsedTime);

	void ClearPassText();
	void SavePlayerName();

	bool IsSavePlayerName();

	void AppendPassWord(char szPass);

	void OnResetScreenSize();

	void ClickOnLoginButton();

	void OnBackClicked();

	void ResetOpenKeyBoardStatus();
private:
	void SetPassEditFocus();

	BOOL m_bKeyBoardOpen;

	
};

extern void LoginSafeSendCommand();
extern BOOL  s_bLoginEcho; 