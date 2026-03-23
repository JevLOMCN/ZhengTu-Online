#pragma once
#include "..\gui\include\guidialog.h"
#include "./SpriteObj.h"
#include "./GameScene.h"

class CRenderTargetTexture;
class CCustomTexture;


///轻聊显示
class CNameTip
{
public:
	CNameTip();
	void Render(POINT pt);
	void DrawText(const char *czText,POINT pt)
	{
		AddText(czText);
		Render(pt);
	}
	void AddText(const char *cText,bool bAsLineSymbol = false);
	int GetTipHeight() 
	{
		return m_height;
	}
	void Close() {m_strTextVec.clear();}

	void CalculateCutRect();

	typedef std::vector<std::string> strVec;
	///文字（多行）
	strVec m_strTextVec;

private:
	///矩形框图片
	IBitmaps*	m_pBmpRect;	
	///矩形框分割成的9个小矩形
	stRectI m_bmpRect[9];
	///渲染区域分割成的11个小矩形
	stRectI m_renderRect[9];

	///矩形框大小
	int m_width,m_height;
	///行高
	int m_cellHeight;
	///文字颜色
	DWORD m_dwColor;		
	///横边切割宽度
	static const int m_scWidthIncise;
	///纵边切割宽度
	static const int m_scHeightIncise;
};


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
struct stInterestNpc : public stMapFunctionNpc {
	DWORD    dwType;
};
typedef  std::vector<stInterestNpc> tInterestNpc;

///////////////////////////////////////////////////////////////
typedef std::vector<CCharacter*>	tListCharacter;
typedef std::vector<CCharacter*>::iterator IterList;
///////////////////////////////////////////////////////////////
enum PING_STATUS
{
	PING_EQUO_SMALL = 0,
	PING_EQUO_NORMAL,  
	PING_EQUO_BIG,       
};

struct  stPing
{
	PING_STATUS m_Ping;
	char  szTip[128];
	DWORD clr;
};

class CGuiMenu;
class CGuiMiniMap :
	public CGuiDialog
{
	float	m_nScale;
	stRectI m_rectAlphaUV;
	CGuiButton* m_BtnHide;
	CGuiButton* m_BtnShow;
	CGuiButton* m_BtnLargeMap;
	CGuiImage*	m_imgCircle;
	CGuiImage*	m2_imgCircle;
	BOOL ContainsPoint(POINT  ptScreen);

	////////////////////////////////////////////////////////////
	// 
	stPointF  m_ptBmpScale;
	////////////////////////////////////////////////////////////
public:

	CGuiMiniMap(void);
    ~CGuiMiniMap();

	void OnClose(void);
	void OnCreate();
	void OnInitScript();
	bool LoadMiniMap(const char* szMapName2);
	void ReleaseMiniMap();
	void ReleaseMiniMap1();
	void ReleaseMiniMap6();
	bool UpdateRenderBmp();
	bool InitDevice();
	void UnInitDevice();
	bool InitBackAlpha();
	HRESULT OnRender( float fElapsedTime );
	void RenderBackground(float fElapsedTime);
	void Update();
	void UpdateLocation(bool bReAlign);
	void SetScale(float fScale) { m_nScale = fScale;}
	void OnOffMiniMap();
	void OnOffLageMap();
	void OnLargeMiniMap();
	void OnSmallMiniMap();
	void OnAddPrivateChat();
	void OnAddGMMsg();
	void OnAutoReply();
	void OnEveryTaskDlg();
	
	void OnClickSafetyUser();
	//void OnClickMall(); //新商城
	void OnClickHeroListBoard();
	
	void OnClickLanding();

	void UpdateSafetyUser();

	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	BOOL IsInRect(POINT ptScreen);
	void OnResetScreenSize();

	void ShowMiniMap(bool bShow);

	void EchoPing();

	void OnAutoReplyMenuCmd( int nID );

	void SetAutoReplyButtonToolTips(const char* tips);
	//
	bool	OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
private:
        ///////////////////////////////////////////////////////////////
	// Ping 
	void SetPing(const PING_STATUS & ping );
	PING_STATUS m_Ping;

	// character *
	tListCharacter  m_listTeam;
	tListCharacter  m_Family;
	tListCharacter  m_OtherNation;
	tListCharacter  m_AllyNation;
	tListCharacter  m_Friend;
	tListCharacter  m_Tong;
	tListCharacter  m_PkOver; 
	tListCharacter  m_General;
	tListCharacter  m_Couple;
	tListCharacter  m_UnKnownPerson;
private:
	//////////////////////////////////////////////////////////////
	//  城门出口
	void UpdateExitFlag();
	struct szExitFlag
	{
		int x;
		int y;
		float theta;
		char szName[64];
	};
	std::vector<szExitFlag>         m_ExitInfo;
	IBitmaps*                       m_pExitIcon;
	RECT                            m_rcExitClip;
	CToolTips   *                   m_pExitToolTips;

	////////////////////////////////////////////////////////////
	IBitmaps*                       m_pNpcIcon;  // 用于显示各种npc

	CGuiButton*			m_NewMailBtn;
	CGuiButton*			m_MailBtn; // 邮箱按钮
	CGuiButton*			m2_MailBtn;// 邮箱特效

	CMiniMapTextEffect		m_TextSprite;
    void                            InitTextEffect();
	stRectI                         m_rcText;
	stRectI                         m_rcTextLeft;
	stRectI                         m_rcTextRight;

	CGuiMenu*	m_AutoReplyMenu;
public:

	IBitmap*			m_pMemBmp;
	IBitmap*			m_pMemBmp1;
	IBitmap*			m_pMemBmp6;
	IBitmaps*			m_pTeamIcon;
	IBitmaps*			m_pMainRoleIcon;
	IBitmaps*			m_pTaskNpcIcon;
	IBitmaps*			m_pPreTaskNpcIcon;
	IBitmaps*           m_pCoupleCharacterIcon;
	
	//IDirect3DSurface8* m_pMemBmp;
	CRenderTargetTexture* m_pRenderBmp;
	CCustomTexture*	m_pBackAlpha;
	
	DWORD m_dwUpdateTime;
	DWORD m_dwUpdateCMDTime;
	std::vector<DWORD> m_vecTimeStamp;
	
	bool m_bRenderTarget;
	bool m_bUpdateSuccess;

	DWORD m_dwStartTimeMiniBtn;
	stResourceLocation m_rlMiniBtn;
	/////////////////////////////////////////////////////////////
	// NPC显示
	tInterestNpc      m_vecIntersetNpc;
	void ResetMapNpc(tListMapNpc& listMapNpc);
	void     RenderTip();

 	void bind_lua(lua_State* L);

	void InitCustomElements();

};


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiLageMap : public CGuiDialog
{
	void OnClose();
	stRectI m_bmpRect;
	stPointF m_scale;
	/*struct stMZoneInfo{
		char szName[256];
		stRectI rect;
	};
	std::map<DWORD,stMZoneInfo> m_zones;*/

	DWORD     m_dwLastUpdateTime;
	void      RequestTeamerPos();

public:
	void InitRect();
	CGuiLageMap();
	void GetZoneInfo();
	IBitmap*			m_pBmp;
	IBitmaps*			m_pTeamIcon;
	IBitmaps*			m_pMainRoleIcon;
	//IBitmaps*			m_pPreTaskNpcIcon;

	IBitmaps*                       m_pFuncNpcIcon;
	IBitmaps*                       m_pCoupleCharacterIcon;
	
	tInterestNpc    *                m_pVecInterestNpc;
	tListCharacter    *             m_pVecCoupleCharacter;

	void OnCreate();
	void RenderBackground(float fElapsedTime);
	void OnResetScreenSize();
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	
};

