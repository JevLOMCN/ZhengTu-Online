/**
*\file		GuiWorldConn.h
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2005-7-5   17:12:00 中国标准时间
*\brief	    社会关系界面
*
* 
*
*/

#pragma once
#include "..\gui\include\guidialog.h"
#include "./GameScene.h"

#define   SCROLLTAB           10


#define   TONG                0  //帮会
#define   TongBack            300
#define   TongList            301
#define   TongButton1         302
#define   TongButton2         303
#define   TongStatic          310
#define   TongButton3         304
#define   TongButton4         305
#define   TongButton5         306
#define   TongButton6         307
#define   TongButton7         308

#define   TongButton8         309
#define   TongButton9         310
#define   TongButton10        311
#define   TongButton11        312
#define   TongButton12        313
#define   TongButton13        314
#define   TongButton100       380

#define   TongEditBoard       320
//
#define   TongButtonW         350
//

#define   SEPT                1  //家族
#define   SeptBack            200
#define   SeptList            201
#define   SeptButton1         202
#define   SeptButton2         203
#define   SeptStatic          206
#define   SeptButton3         204
#define   SeptButton4         205
#define   SeptEditBoard       220

//
#define   SeptButton5         207
#define   SeptButtonW         250

#define   SeptButton6         208 
#define   SeptButton7         209 
#define   SeptButton8         210 
#define   SeptButton9         211
#define   SeptButton10        212
#define   SeptButton11        213
#define   SeptButton100       280
//

#define   SCHOOL               2  //师门
#define   SchoolBack            400
#define   SchoolList            401
#define   SchoolButton1         402
#define   SchoolButton2         403
#define   SchoolStatic          406
#define   SchoolButton3         404
#define   SchoolButton4         405
#define   SchoolEditBoard       420

//
#define   SchoolButton6         406
#define   SchoolButton5         407
#define   SchoolButtonW         450

#define   SchoolButton100         480
//


#define   FRIEND             3  //好友
#define   FriendBack          100
#define   FriendList          101
#define   FriendButton1       104
#define   FriendButton2       105
#define   FriendButton3       106
#define   FriendButton4       107
#define   FriendButton5       108
#define   FriendButton6       109
#define   FriendButton7       110

//
#define   FriendButton8       109
#define   FriendButton9       110
#define   FriendButton10      111
#define   FriendButton11      112 
#define   FriendButton12      113 

#define   FriendButton13      114
#define   FriendButton14      115
#define   FriendButton15      116
#define   FriendButton100      180
#define   FriendButton101      181		//委托替身
//


#define   NEARBY              4   //附近玩家
#define   NearbyBack          500
#define   NearbyList          501
#define   NearbyButton1       504
#define   NearbyButton2       505
#define   NearbyButton3       506
#define   NearbyButton4       507
#define   NearbyButton5       508
#define   NearbyButton6       509

//
#define   NearbyButton7       511
#define   NearbyButton8       512
#define   NearbyButton9       510
#define   NearbyButton10      581
#define   NearbyButton100     580
//

#define   NEARBYNPC            5  //附近NPC
#define   NearbyNpcBack        600
#define   NearbyNpcList        601
#define   NearbyNpcButton1     604
#define   NearbyNpcButton2     605
#define   NearbyNpcButton3     606
#define   NearbyNpcButton4     607
#define   NearbyNpcButton5     608
#define   NearbyNpcButton6     609


#define   BADUSER              6   //黑名单
#define   BadUserBack          700
#define   BadUserList          701
#define   BadUserButton1       704
#define   BadUserButton2       705
#define   BadUserButton3       706
#define   BadUserButton4       707
#define   BadUserButton5       708
#define   BadUserButton6       709
#define   BadUserButton7       710
#define   BadUserButton8       711
#define   BadUserButton9       712
#define   BadUserButton100      780


#define   ENEMYUSER           7   //仇人
#define   ENEMYBack          800
#define   ENEMYList          801
#define   ENEMYButton1       804
#define   ENEMYButton2       805
#define   ENEMYButton3       806
#define   ENEMYButton4       807
#define   ENEMYButton5       808
#define   ENEMYButton6       809
#define   ENEMYButton7       810
#define   ENEMYButton8       811

#define   ENEMYButton100     880
#define   ENEMYButton101	 881

///////////////////////////////////////////////////////
#define   OTHER                    9
#define   OTHER_ADDFRIEND          900
#define   OTHER_MAKEGROUP_NORMAL   901
#define   OTHER_MAKEGROUP_HONOR    902 
///////////////////////////////////////////////////////


////////////////////////////////////////////////////////
#define   FRIENDBUTTON       1002   
#define   SEPTBUTTON         1003  
#define   TONGBUTTON         1000  
#define   NEARYBYBUTTON       1001  
#define   TEACHERBUTTON       1004 
#define    ID_BUTTON_COUNTRY     1005 

////////////////////////////////////////////////////////

#define   FRIENDPAGE          102      
#define   BADPAGE             103
#define   ENEMYPAGE           120
#define		CARTOON_PET			901


#define   NEARBYPAGE          503      
#define   NEARBYNPCPAGE        502


#define   LEVEL1              3*60
#define   LEVEL2              25*60
#define   LEVEL3              55*60


#define   TITLECOLOR           0xffff0000
#define   SUBCOLOR             0xff00ffff
#define   ITEMONLINECOLOR       0xffffff00
#define   ITEMOFFLINECOLOR      0xffBEBDBD
#define   FRIENDCOLOR          0xff00ff00

// used for select  
#define   RELATE_CONSORT      11  

#define   PLAYERNAMEWIDTH      20
#define   TONGMASTER          "会 长"
#define   TONGCOMM            "成 员"
#define   SEPTMASTER          "族 长"
#define   SEPTCOMM            "族 人"
#define   CONSORT             "夫 妻"
#define   FRIENDS             "好 友"
#define   BADPLAYER           "黑名单"
#define   SCHOOLMASTER        "师 尊"
#define   SCHOOLGRADE         "第%d代弟子"
#define   TEACHERMASTER        "师 傅"
#define   BROTHERH            "师 兄"
#define   BROTHERL            "师 弟"
#define   PRENTICEDESC         "徒 弟"
#define   TIMESTR             "[%d-%d-%d %d:%d:%d]"


struct stWorldConnGrade {
	BYTE     conn;
	char     szConn[8];
	char     szGrade1[16];
	char     szGrade2[16];
	char     szGrade3[16];
	char     szGrade4[16];
};

struct stPopMenu {
	BYTE    menuType;
	WORD    menuId;
	char    menuName[MAX_NAMESIZE];
	BYTE    menuStyle;
};

struct stSchool {
	DWORD    classID;
	DWORD    line;
	byte     online;
	WORD     level;

	char	 name[MAX_NAMESIZE];
	WORD     occupation;							// 职业类型
	stSchool()
	{
		classID=0;
		line=0;
		name[0]=0;
		online=0;
		level=0;
	}
};

struct stShoolStatus {
	char schoolName[MAX_NAMESIZE]; // 门派名字
	BYTE byStatus; //	SCHOOL_STATUS_NONE,	SCHOOL_STATUS_TEACHER,	SCHOOL_STATUS_SCHOOL
	DWORD dwMasterTempID;
};

struct stTabInfo {
	CGuiMenu*    pMenu;
	CGuiListBox* pListBox;
	byte        ePrivateType;
	byte        eBoardType;
};

struct stNpcInfo {
	std::string   name;
	stPointI      pt;
};

struct stBoardInfo {
	bool              update;
	std::string       strText;
};

struct stBoardShowInfo {
	enumGuiActionType  action;
	bool              bEdit;
	byte              type;
	char              strCaption[32];
	char              strTitle[64];
};

struct stTeacherMemberEx : public stTeacherMember
{
	int  nLine;
};

enum enumRoleState{
	eOnLine,
	eOffLine,
	eOnlineClew,
	eOffLineClew
};
struct stRolePic
{
	//byte          role;
	enumRoleState  state;
	int           group;
	//int           frame;
};

//typedef std::multimap<std::string,stUnionRember>  TONGORDERBYLIST;
struct stTongShow
{
	char szSeptName[MAX_NAMESIZE];
	std::vector<stUnionRember*> tongList;
};
typedef std::vector<stTongShow>                 TONGSHOWLIST;
typedef std::map<std::string,stUnionRember>     TONGLIST;
typedef std::map<std::string,stSeptRember>      SEPTLIST;
typedef std::map<std::string,stRelation>       RELATIONLIST;
typedef std::list<stSchool>                   SCHOOLLIST; 
typedef std::map<std::string,stTeacherMemberEx> TEACHERLLIST;
typedef std::map<UINT,float>                  RATELIST;
typedef std::map<byte,stTabInfo>               TABINFOLIST;
typedef std::vector<std::string>               NEARLYLIST;
typedef std::vector<stNpcInfo>                 NEARLYNPCLIST;
typedef std::map<BYTE,stBoardInfo>             BOARDINFOLIST;       

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiWorldConn :
	public CGuiDialog
{
	enum enumSelect{
		eSelectNone,
		eSelectMine,
		eSelectRember,
	};
	//TONGORDERBYLIST    m_TongOrderByList;   //排序好的帮会成员列表
	TONGSHOWLIST       m_TongShowList;
	TONGLIST           m_TongList;          //帮会成员信息
	SEPTLIST           m_SeptList;          //家族成员信息
	RELATIONLIST       m_RelationList;       //好友成员信息
	RELATIONLIST       m_ConsortList;        //夫妻信息
	RELATIONLIST       m_BadList;           //黑名单成员信息
	RELATIONLIST       m_EnemyList;           //仇人成员信息
	SCHOOLLIST         m_SchoolList;        //门派成员信息
	TEACHERLLIST       m_TeacherList;       //师门成员信息
	NEARLYLIST         m_NearlyList;        //附近玩家
	TABINFOLIST        m_TabList;         //菜单列表
	NEARLYNPCLIST      m_NearlyNpcList;     //附近NPC

	RATELIST         m_rateList;       
	BOARDINFOLIST     m_boardList;


	bool			m_bStartSize;
	SIZE			m_curDlgSize;
	char			m_szSelectRember[MAX_NAMESIZE];
	CChatChannel*	m_pChannel;

	CGuiScrollTab*  m_pScrollTab;
	CGuiMenu*       m_pTongMenu;
	CGuiMenu*       m_pSeptMenu;
	CGuiMenu*       m_pRelationMenu;
	CGuiMenu*       m_pSchoolMenu;
	CGuiMenu*       m_pNearlyMenu;
	CGuiMenu*       m_pNearlyNpcMenu;
	CGuiMenu*       m_pBadUserMenu;
	CGuiMenu*       m_pEnemyMenu;

	//
	CGuiMenu*	m_pOthermenu;

	CGuiMLEditBox*  m_pTongEditBoard;
	CGuiMLEditBox*  m_pSeptEditBoard;
	CGuiMLEditBox*  m_pSchoolEditBoard;

	void*          m_pSelData;
	bool           m_bShowOffline;
	bool           m_bPlaySound;
	

	float        getRate(CGuiControl* pControl);
	void         chat(const char* name);
	void         gotoNpc();
	enumSelect    getSelectType(const char* name);
	void          RemoveAllItem(CGuiListBox* pListBox);
	void*         ResetSelectRember(CGuiListBox* pListBox,char* szBuffer);
	CGuiMenu*     GetCurrMenu();
	CGuiListBox*   GetCurrListBox();
	byte          GetCurrPrivateType();
	byte          GetCurrBoardType();
	void          checkBoard();
	void         readBoard(enumGuiActionType  action);
	void         writeBoard(enumGuiActionType  action);
	bool         IsSelectItem();

	void         InitTong();
	void         InsertItem(stUnionRember* pItem);
	void         DeleteItem(stUnionRember* pItem);
	void         ClearItemData();
	void         ResetTong();
	void         RefreshAllTong(stVoteListUnionCmd* pCmd);
	void         BallotTong(stVoteUnionCmd* pCmd);
	bool         IsTeamer(const char* name,bool bShow=true);

	void         InitSept();
	void         ResetSept();
	void         RefreshAllSept(stVoteListSeptCmd* pCmd);
	void         BallotSept(stVoteSeptCmd* pCmd);

	void         InitRational();
	void         ResetRational();
	RELATIONLIST& GetList(byte id);
	stRelation*   GetRationalPlayer(char* name,byte type=255);
	void         DeleteRationalPlayer(char* name,byte type=255);
	void         AddRationalPlayer(stRelation* st);

	void         InitSchool();
	void         ResetSchool();
	void         InsertSchoolList(stSchoolMember* pMber);
	void         InsertSchoolList(stSchool& st,SCHOOLLIST::iterator pointer);
	SCHOOLLIST::iterator   FindSchoolByName(DWORD classID,char* szName);
	SCHOOLLIST::iterator   FindSchoolByName(char* szName);

	void         InitBadUser();
	void         ResetBadUser();

	void         InitEnemy();
	void         ResetEnemy();

	void         InitMapNpc();


	bool         TongMessage(stNullUserCmd* pCmd,size_t size);
	bool         SeptMessage(stNullUserCmd* pCmd,size_t size);
	bool         RationalMessage(stNullUserCmd* pCmd,size_t size);
	bool         SchoolMessage(stNullUserCmd* pCmd,size_t size);
	void         RefreshCharacter();
	//void         RefreshNpc();

	void         SendBoard(const char* pStr,byte eState);

	void         UpdateMakeGroupMenuState(CGuiMenu * pMenu,int * pBtn,int BtnSize);
private:
	 CToolTips  m_ToolTips;
public:
	CGuiWorldConn(void);
	~CGuiWorldConn(void);

	void SetChatChannel(CChatChannel*	pChannel)
	{
		m_pChannel = pChannel;
	}
	CChatChannel* GetChatChannel() { return m_pChannel; }

	void OnCreate(void);
	HRESULT OnRender(float fElapsedTime);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void InitCustomElements();
	void RenderBackground(float fElapsedTime);
	bool OnContextMenu();
	void OnSetCloseLocation();

	bool ParseMessage(stNullUserCmd* pCmd,size_t size);
	void AddBoard(stChannelChatUserCmd* pCmd);
	void AddChater();
	void RefreshCurrData(byte state);

	void ActiveTabItem(BYTE Index);
	int GetActiveTabItem();
	bool GetIsShowOffLine(){ return m_bShowOffline;}

	void         ResetMapNpc(tListMapNpc& listMapNpc);
	void         RefreshIsOffLine();
	
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	stUnionBaseInfoCmd   m_TongInfo;           //帮会信息
	stShoolStatus        m_SchoolInfo;        //门派信息
	stSeptBaseInfoCmd    m_SeptInfo;          //家族信息
	BYTE           m_TongPower[2];  

	stUnionRember*	FindTongMember( const char* pszName );
	stSeptRember*	FindSeptMember( const char* pszName );
	stTeacherMemberEx*	FindTeacherMember( const char* pszName );

	int GetTongSeptMasterNum();

	BOOL   IsBad(const char * szName)
	{
		std::string strKey = szName;
		return ! (m_BadList.end() == m_BadList.find(strKey));
	}

	bool  IsMyRelation(const char* szName,BYTE state);

	void SwitchFromCountryDlg();
	void SwitchFromAdoptCartoonDlg(UINT uID);

	static void OnChannelClosed(CChatChannel* pChannel);
};

void AddFriend(const char* name);
void AddBad(const char* name);
void AddTong(const char* name);
void AddPrentice(const char* name);
void AddSept(const char* name);

void RequestSept();
void RequestTong();
void RequestSchool();
void RequestMarry();

void ShutDownSept();
void ShutDownTong();
void ShutDownSchool();

int GetMaxSeptNumByTongMana(const int& dwMana);
stPointI GetResource(byte role,enumRoleState state);