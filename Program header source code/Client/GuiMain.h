/**
 *\file		GuiMain.h
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2006-7-19  21:11:00 中国标准时间
 *\brief	游戏主界面
 *
 * 
 *
*/

#pragma once
#include "..\gui\include\guidialog.h"
#include "./GuiButtonList.h"

#include <vector>

// 2010
static const stPointF skillIconScale(0.8f, 0.8f);

static const stPointF itemIconScale(1.0f, 1.0f);

extern int g_nMainAccessBtnLeft ;
extern int g_nMainAccessBtnTop ;	
extern int g_MainAccessBtnSpace;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGMTextEffect;
class CGuiMain :
	public CGuiDialog
{
	CAnimation     m_aniMain;
	CAnimation     m_lianzhan1;
	CAnimation     m_lianzhan2;
	CAnimation     m_lianzhan3;
	CAnimation     m_lianzhan4;
	CAnimation     m_lianzhan5;
	CAnimation     m_lianzhan6;
	CAnimation     m_lianzhan7;
	CAnimation     m_lianzhan8;
	CAnimation     m_lianzhan9;
	CAnimation     m_lianzhan10;
	CAnimation     m_lianzhan11;

	CAnimation     m_jingcai;

	CAnimation     gundongbeijing3;
	int            m_showX;
	int            m_showY;
	int            m_showX4;
	int            m_showY4;


	//快捷图标隐藏
	bool yincang;
	//TAB索敌索引
	int tabNum;
	// 图标裁减区
	stRectI      m_rcIconClip;

	enumChatType m_iChatType;
	int m_iScreenWidth;
	//std::vector<IBitmap*> m_pBmpForeground;
	///步行按钮
	CGuiButton* m_BtnWalk;
	///跑步按钮
	CGuiButton* m_BtnRun;
	///攻击模式按钮
	//CGuiButton*	m_BtnAttackMode;
	///三值显示条
	CGuiImage*	m_imgThreeValue;
	
	///快捷键提示
	CToolTips*	m_pBtnAccessKeyToolTips[MAIN_ACCESS_KEY_NUM];
	///快捷键提示是否需要更新
	bool		m_bAccessKeyToolTipsNeedUpdate[MAIN_ACCESS_KEY_NUM];
	///当前快捷键组
	int	m_curAccessKeyGroup;
	///鼠标所指快捷键id
	int m_mouseOverAccessKeyNo;
	//////////////////////////////////////////////////////////////////////////
	int m_curEditAccessKeyNo;
	///经验条提示
	CToolTips	m_ExpTips;
	//
	
	///pk 模式选择按钮
	//CGuiButton* m_pBtnPkMode[PKMODE_NUM];
	CGuiButtonList m_aPkModeBtnList;
	CGuiMenu*	m_menu;
	//CGuiMenu*	m_cartMenu;
	enum enumBarType{
		BarTypeVert,
		BarTypeHoriz,
	};

	struct stBarShowInfo{
		stRectI rcClip;
		enumBarType type; 
		IBitmap* pBmp;
	};
	
	

	std::vector<stBarShowInfo> m_aBar;
	void RenderBar(CGuiMain::stBarShowInfo& bar,uint64_t iCur,uint64_t iMax,int index);
	void UpdateChatBtn();
	
	struct stStateInfo
	{
		bool bMainCharacter;
		bool bSkill;
		DWORD dwID;
		IBitmaps* pIcon;
		stPointI ptPos;
		DWORD time;
		DWORD startTime;
		DWORD rate;
		bool bFlashState;
		float fFlashRate;
		D3DXCOLOR color;

		stStateInfo()
		{
			color = -1;
			bFlashState = false;
			fFlashRate = 0.12f;
		}

		void PlayFlash(float fRate = 0.12f)
		{
			bFlashState = true;
			fFlashRate = fRate;
			color = -1;
		}

		void StopFlash()
		{
			bFlashState = false;
			color = -1;
		}
	};

	struct stStateTips
	{
		char	strName[32];
		char	strDesc[512];
	};

	std::vector<stStateInfo> m_aStateInfo;
	CToolTips	m_stateToolTips;

	BOOL ContainsPoint(POINT  ptScreen);
public:
	//zh
	std::vector<std::string> m_BossList;
	//////////////////////////////////////////////////////////////////////////

	///快捷键按钮
	CGuiButton* m_pBtnAccessKey[MAIN_ACCESS_KEY_NUM];
	//Skill access btn active sign
	IBitmaps* m_activeSkillSign;

	IBitmaps* m_vipSign;
	IBitmaps* m2_vipSign;

	//Item num image on access key
	IBitmap* m_itemNumImg[10];

	/// 鼠标是否在背景图片的有效区域
	bool	m_bMouseInBackground;

	// bool isBoxAni;
public:
	enum enumActiveAccessType{
		ActiveAccessSkill,
		ActiveAccessItem,
        ActiveAccessEmotion,
		ActiveAccessEquipment,
		ActiveAccessTaoZhuang,
		ActiveAccessAttaPak,
		ActiveAccessNums,
	};
	enum enumMessageType
	{
		TaskMessage = 1,
		ZoneMessage = 2,
		ExpMessage = 3,
		SystemMessage = 4,
		ScrollMessage = 5,
		LuckAttackMessage = 6,
		HsMessage = 7,//soke 黄
		LsMessage = 8,//soke 绿
		ZsMessage = 9,//soke 紫
		CsMessage = 10,//soke 橙
		FhMessage = 11,//soke 红（系统专用）
		FsMessage = 12,//soke 桔红-结婚专用
		ShMessage = 13,//soke 粉红-送花
		XShMessage = 14,//soke 粉红-送花
		ZfMessage = 15,//soke 深红-好友膜拜
		TYMessage = 16,//soke 通用
	};

	int password1;
	int password2;
	int password3;
	int password4;
	//连斩
	CGuiStatic* m_lianzhaninfo;
	//功勋竞猜
	CGuiStatic* m_gxbossinfo;
	long lianzhantime;
	int lianzhannum;

	long long pigtime;
	long long killpigtime;
	long long killpigtime_hm;
	CGuiButton* m_BtnPrivateChat;
	///包裹1
	CGuiTable* m_pTableLeft;
	///包裹2
	CGuiTable* m_pTableRight;
public:
	void SetChatType(enumChatType iChatType);
	CGuiMain(void);
	~CGuiMain();
	void OnCreate();
	void OnClose(void);
	void password(int i);//敬请期待密码
	void OnInitScript();
	void InitIcon();
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void SendChat(CGuiEditBox* pEditChat);
	void RenderBackground(float fElapsedTime);
	HRESULT RenderAccessKeyNotes(float fElapsedTime);
	HRESULT RenderAccessKeyBtnToolTips(float fElapsedTime);
	HRESULT RenderAccessKeyBtnDisableStatus(float fElapsedTime);
	void OnResetScreenSize();
	//TAB索敌
	void OnTabSuoDi();
	void set2ActiveIcon(CGuiButton* btn){m2_ActiveIcon=btn;}
	CGuiButton* get2ActiveIcon(){ return m2_ActiveIcon; }

	CToolTips* GetAttPakTooltips(int nIndex);

	void changeAttPak(int nIndex);

	bool GetResourceLocation(DWORD iAconID,stResourceLocation & rl);
	
	CGuiButton* m2_ActiveIcon; 

	HRESULT OnRender( float fElapsedTime );
	void InitCustomElements();
	DWORD OnHiTest(POINT ptScreen);
	//cart menu
	void ShowCartMenu( bool bPlay );
	void CloseCartMenu();
	void PlayAni(stResourceLocation& rl,int nSpeed,int x,int y,bool bLoop=false);
	void PlayLZ(int num);
	void PlayJC(int num);
	//State about
	DWORD	GetStateIconID(DWORD state);
	void	GetStateDesc(DWORD state, stStateTips& desc);
	int		GetMainCharacterStateIconNum();
	int		GetActiveNpcStateIconNum();
	void AddOneState(bool bMainCharacter, bool bSkill, DWORD dwID, DWORD time, DWORD rate);
	void DeleteOneState(bool bMainCharacter, bool bSkill, DWORD dwID);
	bool DeleteInterestNpcStateList();
	bool DeleteAllState(bool bMainCharacter);
	void RenderStateList( float fElapsedTime );
	void RenderStateTooltips(DWORD curTime);
	void UpdateStateList(DWORD curTime);
	void UpdateStateTooltips( stPointI& ptPos, DWORD curTime );
	stStateInfo* GetMouseOverStateInfo();
	//

	//Set access key
	bool SetAccessKey( const int& keyID );
	bool IsAnyActiveForAccessKeySetting( enumActiveAccessType& type );
	BYTE GetCurAccessKeyGroup() { return m_curAccessKeyGroup; }
	bool SetActiveAccessKeyGroup(BYTE group);
	void OnAccessBtnSelectSkill( const int& btnID, const int& skillId );
	bool OnFindAccessSkill();
	void EditAccessKeyBtn(const DWORD& id);
	
	void RefreshKeyGroupState( const int& group = 0 );
	void UpdateKeyTooltips( const BYTE& btnID );
	void ChangeCurKeyGroup( bool bDown = true );

	void UpdateAccessKeyBtnIcon();
	int  GetMouseOverAccessKeyNo();
	int	 GetCurAccessKeyNo() { return m_mouseOverAccessKeyNo; }
	void DeleteMouseOverAccessKey();
	void DeleteAccessKey(enumGameAccelKeyType type, DWORD& id);
	void ReplaceAccessKey(enumGameAccelKeyType type, const DWORD& id, const DWORD& newID);
	void RemoveAccessKeyBtnInfo( BYTE key );

	//Change attack mode tips
	void ChangeAttackModeBtnTips();
	void ChangeActivePkModeBtn();
	void TurnOffPkModeList();
	void OnOffPkModeList(int curMode);
    bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnOffWalkRunBtn();
	void SwitchWalk();
	bool OnContextMenu();
	CGuiButton* InsertPrivateButton(int nPos);

	//Get chat type
	int GetChatType() { return m_iChatType; }


	void DrawScreenCenterMsg();
	void DrawScreenRightBottomMsg(stPointI& pt);
	void AddMessage(const char* msg,enumMessageType type,DWORD color = -1);  //soke 黄

	void AddTaskTimeMessage(DWORD dwTaskID,const char* title,DWORD dwTime);
	void DrawTaskTimeMessage( const POINT& pt );
	void RemoveTaskTimeMessage(DWORD dwTaskID);
	void __SetEmotionAccessBtn(stResourceLocation & rl,int keyID);

	void AddGMTextSprite(const char * pchar,DWORD clr);
	void AddHGMTextMessage(const char * pchar,DWORD clr); //soke （黄）
	void AddGMTextMessage(const char * pchar,DWORD clr); //soke （绿）
	void AddZGMTextMessage(const char * pchar,DWORD clr); //soke （紫）
	void AddCGMTextMessage(const char * pchar,DWORD clr); //soke （橙）
	void AddXGMTextMessage(const char * pchar,DWORD clr); //soke （红）
	void AddJGMTextMessage(const char * pchar,DWORD clr); //soke （桔红-结婚）
	void AddSGMTextMessage(const char * pchar,DWORD clr); //soke （粉红-送花）
	void AddXSGMTextMessage(const char * pchar, DWORD clr); //soke （蓝色-送蓝色妖姬）
	void AddYGMTextMessage(const char * pchar,DWORD clr); //soke （深红-好友膜拜）
	void AddTYTextMessage(const char * pchar,DWORD clr); //soke （通用）
	
	//sky 好帮手
	void AddbangsBtn(bool nShow = true);

	//升级属性点还有剩余提示按钮相关,无则创建,并按照nShow操作
	void AddshuxBtn(bool nShow = true);
	
	//战车小图标
	void AddZhanche(bool nShow = true);

	//升级技能点还有剩余提示按钮相关,无则创建,并按照nShow操作
	void AddjinengBtn(bool nShow = true);
	
private:
	std::auto_ptr<CGMTextEffect> m_pTextSprite;
	std::auto_ptr<CGMTextEffect> mh_pTextSprite;//soke （黄）
	std::auto_ptr<CGMTextEffect> mt_pTextSprite;//soke （绿）
	std::auto_ptr<CGMTextEffect> mz_pTextSprite;//soke （紫）
	std::auto_ptr<CGMTextEffect> mf_pTextSprite;//soke （橙）
	std::auto_ptr<CGMTextEffect> mx_pTextSprite;//soke （红）
	std::auto_ptr<CGMTextEffect> mj_pTextSprite;//soke （桔红-结婚专用）
	std::auto_ptr<CGMTextEffect> ms_pTextSprite;//soke （粉红-送花）
	std::auto_ptr<CGMTextEffect> xms_pTextSprite;//soke （蓝色-送蓝色妖姬）
	std::auto_ptr<CGMTextEffect> my_pTextSprite;//soke （深红-好友膜拜）
	std::auto_ptr<CGMTextEffect> my_pTextSprite2;//soke （深红-好友膜拜）
	struct stHelpButton
	{
		CGuiButton		*pBtn;
		enumUserSetting type;
		DWORD			dwStartTime;
		int				iBtnId;

		stHelpButton()
		{
			pBtn = NULL;
			type = USER_SETTING_TEAM;//忽略
			dwStartTime = xtimeGetTime();
		}
		void Hide()
		{
			if(pBtn) 
			{
				pBtn->SetToolTips("");
				pBtn->SetVisible(false);
			}
		}
	};

	typedef std::vector<stHelpButton *> stHelpButtons;
	stHelpButtons m_helpBtns;

	stHelpButton *m_pbangsBtn;//sky 好帮手控件
	inline stHelpButton *GetHidebangsBtn(){ return m_pbangsBtn; }

	stHelpButton *m_pshuxBtn;//升级属性点还有剩余提示
	inline stHelpButton *GetHideshuxBtn(){return m_pshuxBtn;}

	stHelpButton *m_pjinengBtn;//升级技能点还有剩余提示
	inline stHelpButton *GetHidejinengBtn(){return m_pjinengBtn;}


	//战车小图标
	struct stZhancheButton
	{
		CGuiButton		*pBtn;
		int				iBtnId;

		stZhancheButton()
		{
			pBtn = NULL;
		}
		void Hide()
		{
			if(pBtn) 
			{
				pBtn->SetToolTips("");
				pBtn->SetVisible(false);
			}
		}
	};

	stZhancheButton *m_pZhancheBtn;//战车小图标
	inline stZhancheButton *GetHideZhancheBtn(){return m_pZhancheBtn;}


private:
	inline int GetNextHelpBtnId();
	inline stHelpButton *GetHideBtn( );
	bool bIsTypeExist(enumUserSetting type)
	{
		for(stHelpButtons::iterator it = m_helpBtns.begin();
			it != m_helpBtns.end(); ++it)
		{
			if((*it)->type == type) return true;
		}
		return false;
	}
	
public:
	void UpdateHelpBtnState();
	void ShowHelpTipBtn(enumUserSetting type);
	void OnHelpTipBtn(enumUserSetting type);
	void ShowAllTipBtn(bool bShow = true);
	size_t GetShownHelpTipNum();
	BOOL IsInRect(POINT ptScreen);


private:
	struct stScreenMessage
	{
		std::string strMsg;
		DWORD dwShowTime;
		DWORD dwTotalShowTime;
		DWORD dwColor;
		stPointF scale;
		enumMessageType type;
		int	  alpha;

		stScreenMessage()
		{
			type = TaskMessage;
			strMsg = "";
			dwShowTime = 0;
			dwColor = -1;
			scale = stPointF(1.0f,1.0f);
			dwTotalShowTime = 10000;
		}
	};
	//任务消息
	typedef std::vector<stScreenMessage> TaskMsgList;
	typedef TaskMsgList::iterator TaskMsgListIt;
	TaskMsgList m_taskMsgList;

	//经验值消息
	struct stExpMessage
	{
		char msg[MAX_CHATINFO];
		DWORD dwColor;
		DWORD dwTime;
		enumMessageType type;
	};
	typedef std::vector<stExpMessage> ExpMessageList;
	typedef ExpMessageList::iterator ExpMessageListIt;
	ExpMessageList m_expMsgList;

	struct stTaskTimeMessage
	{
		DWORD		dwTaskID;
		std::string	strTaskTitle;
		DWORD		dwTaskTime;
		DWORD		dwBeginTime;
	};
	typedef std::vector<stTaskTimeMessage>	TaskTimeMsgList;
	typedef TaskTimeMsgList::iterator		TaskTimeMsgListIt;
	TaskTimeMsgList	m_listTaskTime;

	//
	void __AddMessage(const char* msg,enumMessageType type,DWORD color);  //soke 黄

	/////////////////////////////////////
	IBitmaps * m_pTeamHeadIcon;
	IBitmaps * m_pTeamExpAssignByLevel;
	IBitmaps * m_pTeamExpAssignBySpecial;
	IBitmaps * m_pTeamItemAssignByFreedom;
	IBitmaps * m_pTeamItemAssignByPoll;
	stRectI    m_rcClipTeamExpAssignByLevel;
	stRectI    m_rcClipTeamExpAssignBySpecial;
	stRectI    m_rcClipTeamItemAssignByFreedom;
	stRectI    m_rcClipTeamItemAssignByPoll;

	///////////////////////////////////////////////
	// tooltips
	void     RenderTeamFlag();
	void     RenderTeamTip();
	bool     m_bShowTeamToolTip;
	//////////////////////////////////////////////
public:
	bool isHasStateSkill(DWORD dwID);

public:
	//系统公告 in Client
	bool initSysMessage(const char* pszFileName);	//
	bool m_bEnabledSysmessage;						//1=on,0=off;
	void OnUpdateSysmessage(float fElapsedTime);
	//系统公告相关变量
	struct stSysMessage 
	{
		std::string starttime;
		std::string endtime;
		int order;
		int login;
		int interval;
		int states;//本轮怎么发的状态
		DWORD dwCurTime;
		int j;
		std::vector<std::string> pmessgae;
		stSysMessage()
		{
			starttime ="000";
			endtime ="111";
			order = 0;
			login = 0;
			interval = -1;
			states = 0;//0=没有发过，1＝发过，下回继续，2＝只发一下，3＝永久不再发
			dwCurTime = 0;
			j = 0;
		}
	};
	std::vector<stSysMessage> m_SysMessage;
	//
public:
	bool CanPrintCenter;

	// 2010 3
	bool m_bShowSystemMessage;
};



inline bool bIsUserUnSetted(enumUserSetting type);
inline void ShowHelpTip(enumUserSetting type);
inline void OnTipHelp(enumUserSetting type,bool bCheck = true);

extern bool SendAttackMode(int curMode);
extern bool ChangeAttackMode(int mode);
extern bool SaveBackBufferToFile();
extern void OnOffNearbyPlayer();
extern void OnSitDown();
extern void OnOffAutoAttackSummon();
extern void OnRide();
extern void OnOffSysDialog();
extern void OnOffAutoAttackDlg(); /////////star100515
extern void OnOffAutoAttack(bool,WORD wTxtShow = 0);/////////star100515
extern void OnRunSiRenBieShuDialog(); //云天别墅
extern void OnUserClickButton1(); //敬请期待
extern void OnUserClickButton2(); //敬请期待
extern void OnUserClickButton3(); //敬请期待
extern void OnUserClickButton4(); //敬请期待
