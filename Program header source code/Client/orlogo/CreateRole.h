#pragma once
#include ".\gameappation.h"
#include "..\gui\include\guidialog.h"

#include "./SpriteObj.h"

#define PROFESSION_1_DESC     "侠客：身材魁梧，注重近身格斗技能。"
#define PROFESSION_2_DESC     "侠女：身材健美，专著于修炼武功。"

enum enumRoleSel
{
	eRoleSel_ONE = 0,
	eRoleSel_TWO = 1,
	eRoleSel_NULL = 2,
};

#define __MAX_CHARINFO__ 2

class CGameRoleDemo
{
public:
	///////////////////////////////////////////////////////////////////////
	IBitmaps*    pDemoBk;
	int      nBmpWidth ;
	int      nBmpHeight;

	RECT         rcClip; 
	void         RenderBlock(POINT ptDlg);
	void         DrawBlock(int x,int y,int index,POINT ptDlg);

	stRectI      rcShow;
	///////////////////////////////////////////////////////////////////////


	float        fRunTime;
	float        fTotalTime;

	int          nFramePerDir;
	bool         bUpdateFrame;

	int          nMaxDir;
	int          nCurDir;
	int          nCurDirFrame;

	float          fCharacterSpeed;
	float        fGrassSpeed;

	bool         bInited;

	float          x,y;// 人物位置

	CGameRoleDemo();
	void init();
	void RenderBk(POINT pt);
	void Update(float fElaspedTime);

	void ChangeCharacter(const stResourceLocation & rlBody,const stResourceLocation & rlHair,bool bDrawBody,bool bDrawHair);
	void RenderCharacter(int x,int y ,DWORD clrHair);

	void ChangeHair(const stResourceLocation & rlHair,bool bDrawHair);

	void UpdateFrameAsReverseTime();
	void UpdateFrameAsTime();

	CAnimation       m_DemoBody;
	CAnimation       m_DemoHair;

	bool            bRenderBody;
	bool           bRenderHair;

};

struct stRole 
{
	enum enumRoleState
	{
		eRoleState_NULL,                   // 角色空标志 
		eRoleState_DONE,                   // 角色已有标志
	};
	char  name[MAX_NAMESIZE + 1];	           // 角色名称
	WORD  type;			           // 职业类型
	WORD  conntryId;                           // 国家ID
	char  countryName[MAX_NAMESIZE+1];         /// 国家名称
	WORD  level;                               // 等级

	//WORD  five;                              // 五行
	WORD  oldtype;                             // 因为现在type已经被head用，而头发类型仍然要用，所以保留

	WORD  head;                                // 当前头像信息

	WORD  hairstyle;                           // 发型

	DWORD  clrHair;                             // 头发颜色

	enumRoleState  state;                      // 角色有否

	DWORD        dwLightColor;                       // 光照颜色

	DWORD        dwBitMask;

	// 动画播放状态
	enum enumAnimationState
	{
		eAni_State1 = 0,	        
		eAni_State2,	          
		eAni_State3,		         
		eAni_Num, 
	};
	enumAnimationState DstAniState;            // 目标运动状态
	enumAnimationState SrcAniState;            // 当前运动状态


	stRole()
	{
		init();
	}
	void init();	
};


class CGuiDelRoleDialog : public CGuiDialog 
{
public:
	CGuiDelRoleDialog();

	void    OnCreate(void);
	bool    OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);
	bool    MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void    OnResetScreenSize();

	virtual void OnClose(void);
};

class CGuiRoleSelDialog : public CGuiDialog 
{
public:

	CGuiStatic* m_pLabelRoleName[2];             // 角色名称Label
	CGuiStatic* m_pLabelRoleNation[2];           // 国家ID
	CGuiStatic* m_pLabelRoleLevel[2];            // 等级     

	CCustomTexture  * pTexture;
	struct _Ani
	{     
		CAnimation m_ani;         
		bool bAddLight;
		bool bCreated ; 
		//bool bReserver;                     // 是否倒播
		bool bStop;
		_Ani()
		{
			init();
		}
		void init()
		{
			bCreated = false;
			bStop = false;
			bAddLight = false;
		
		}
	};
	_Ani m_roleAni[2];

	void UpdateAniState();

	// GUI
	void ShowCreateAndDeleteBtn(bool bShow);

	void UpdateRoleSelDemoAni();

	void CreatePPSound();
	void DestroyPPSound();
	void CreateWeaponSound();
	void DestroyWeaponSound();

	void OnCompleteAniEvent();

private:	
	CSound           * m_pSound;
	CSound           * m_pWeaponSound;
	CCustomTexture* m_pPassport;                 // 验证码
	CGuiEditBox*   m_pEditPass;                  // 输入验证码框

	CGuiImage * m_pImageUpBord;

	int     m_iMouseOn;
	void    MouseOn(bool bMouseOn,int index);

	void    DrawCharacterAni();
	void    RenderTextEffect(float fElapsedTime);
	void    RenderPassPort();
public:
	CGuiRoleSelDialog();

	void    ReLoad();

	void    OnCreate(void);
	bool    OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);
	bool    MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void OnClose(void);
	virtual BOOL ContainsPoint(POINT ptScreen);
	DWORD   OnHiTest(POINT ptScreen);
	void    RenderBackground(float fElapsedTime);

	void RenderUpBorder(float fElapsedTime);

	void EnableCreateBtn(bool bEnable);
};


//---------------------------------------------------------------------
//　头像 
struct szCharInfo
{
	WORD  type;			    // 职业类型
	int   group;                        //
	std::vector<int>   nVecFrame;       // for 4 character head info 
};
//---------------------------------------------------------------------

class CGuiRoleCreateDialog: public CGuiDialog
{
	WORD              m_moveCountry;
	CGuiImage*        m_pImageTag;              // omitted 
	CGuiImage*        m_pImageConText;	    // omitted 
	CGuiImage*        m_pImageCountryName;	    // omitted 

	CGuiImage*      m_pImage;	            // 角色园圈图片显示 

	std::vector<szCharInfo> m_vecCharInfo;

	CCreateRoleTextEffect  m_AniRoleDesc;
public:
	CGuiEditBox*    m_pEditRoleName;	    // 角色名


	CGuiComboBox    * m_pHairCombox;
	CGuiComboBox    * m_pCountryCombox;

	CGuiImage * m_pImageRole;

	CGuiButton * m_pButtonMen;
	CGuiButton * m_pButtonWomen;

	WORD  m_EditingFace;                         // 当前正在编辑的Face，不是发型

        CGameRoleDemo    m_RoleDemo;

	void RefreshHeadIcon();
	void RefreshHairCombox();
	void RefreshCountryList();

	void   FinishCreate();
	int    GetCurSelHair();
	void   UpdateHair();
	// Hairlist 
	std::vector<int>        m_stHairList;

	CGuiSlider *m_pRSlider;
	CGuiSlider *m_pGSlider;
	CGuiSlider *m_pBSlider;

	DWORD m_curColorHair;

	void             UpdateCreateDemoAni();

	void             init();
	void             SetButtonState();
	void             RenderButtonText();

	void             PopoutWindow();
public:
	CGuiRoleCreateDialog();
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender( float fElapsedTime ); 
	virtual void OnClose(void);
	virtual BOOL ContainsPoint(POINT ptScreen);
	void    RenderBackground(float fElapsedTime);
	void InitCustomElements();
	void OnCompleteAniEvent();

	void ReLoad();
	void UpdateData();

	///////////////////////////////////////////////////////
	void InitCharInfo();
	WORD GetNextHead(WORD type,int nCurHead);
	WORD GetPrevHead(WORD type,int nCurHead);
	WORD GetTheFirstHead(WORD type);
	int GetCurGroup(WORD type);

	void CreateAniRoleDesc();
	void ReleaseAniDesc(){m_AniRoleDesc.Release();};

};


class CCreateRoleManager
{
public:
	// Two Role 
	stRole         m_role[__MAX_CHARINFO__];
	enumRoleSel    m_eSelRoleID;                   // 当前已经选中的角色
	enumRoleSel    m_eCreateingRoleID;             // 当前正在创建的角色

	bool           m_bSingle;        // 是否只有一个角色
	// Create
	bool		m_bInCreating ;
	stRole          m_DemoRole;   

	enumRoleSel      FindRoleByName(const char *pName);

	void            InitRole();
	// Two Dialog 
	CGuiRoleCreateDialog*   m_guiRoleCreate;
	CGuiRoleSelDialog*      m_guiRoleSel;          // 弹出

	// DelRole Dialog
	CGuiDelRoleDialog *     m_guiDelRole;


	void SetCreateingCountryID(int nCountry){ m_role[m_eSelRoleID].conntryId = nCountry;}
	void SetCreateingHairType(int nHairtype){m_role[m_eSelRoleID].hairstyle = nHairtype;}

public:
	CCreateRoleManager(void);
	~CCreateRoleManager(void);

	CGuiRoleCreateDialog*   AddRoleCreate();
	CGuiRoleSelDialog*     AddRoleSel();

	CGuiDelRoleDialog * AddDelRole();

	stRole* GetCreateingRole();
	stRole * GetCurSelRole();

	void           init();
	void           Release();
	void           GetUserInfo(SelectUserInfo * info);
	void           DelUser();
	void           CreateUser();
	bool           SetSelRoleID(enumRoleSel newSelRoleID);
	int            GetHairTypeIndex(const int* pHairType,size_t typenum);

	void           CreateUserSuccess();
	void           SendCreateUserCommand();

};

extern CCreateRoleManager* GetCreateRoleManager();
extern void CreateRole(SelectUserInfo * info = NULL);
extern void ReturntoLogin();
extern void LoadLoginRes();
extern void ClearResEntryGame();
extern void ResetLoginDevice();
