#pragma once

#include <vector>

///////////////////////////////////////////////////////////////////////
// 查询大将军王的经验和能力
class CGuiQueryGenDlg : public CGuiDialog
{
public:
	CGuiQueryGenDlg(void);
	~CGuiQueryGenDlg(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void); 

	void UpdataUI();

private:
	CGuiProcess*     m_pExpProcess;
	CGuiStatic*	m_pLevelStatic;
	CGuiStatic*	m_pExpStatic;

};



///////////////////////////////////////////////////////////////////////
// 调税界面
class CGuiModifyRevenueBox : public CGuiDialog
{
public:
	CGuiModifyRevenueBox(void);
	~CGuiModifyRevenueBox(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void); 

	void UpdateCountryMoney();
private:
	CGuiSlider* m_pCurRevenueSlider;
	CGuiStatic* m_pCurRevenueStatic;
};



//////////////////////////////////////////////////////////////////////////
// 国家倉库界面
class CGuiCountrySavingBoxDlg :
	public CGuiDialog
{
public:

	CGuiCountrySavingBoxDlg(void);
	void OnClose(void);
	void OnCreate();

	///存放物品的格子
	CGuiTable* m_pTable;

	void OnSetCloseLocation();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	void OnLeftClickedOnItem();

private:
	bool PutDownRoleItem(CGuiItemCell * pSrcCell);
	void PickUpRoleItem();

        // Get some info from table 
	DWORD GetCountributePoint(CRoleItem * pRoleItem,const CountryMaterial_t & country_materail);
	bool  getMaterialKind(CRoleItem * pRoleItem,DWORD & dwOutter);
	bool  CanCountributeToCountry(CRoleItem * pRoleItem,CountryMaterial_t & outter);

	void  MoveBackToPackage();

	CGuiStatic  * m_pStatic;

};

///////////////////////////////////////////////////////////////////////
// 国家科技界面


//-----------------------------------------------------
//  进行中研究
//-----------------------------------------------------
#define MAX_TECH_NAME 20
#define MAX_TECHER_NAME 20

struct stStudying          
{
	DWORD dwTechOption;  // 科技id
	std::string strTechName;// 科技名
	std::string strResearcher;// 研究员
	DWORD dwFinished; // 完成度　
	struct stTipMap
	{
		DWORD dwNextLevel;
		DWORD dwNeedMoney;
		DWORD dwNeedContribute;
		DWORD dwNeedMaterial;
	};
	stTipMap stTip;

};
typedef std::vector<stStudying*> tVecStudyingItem;
typedef std::vector<CGuiButton *> tVecUpdateBtn;
typedef std::vector<CGuiProcess *> tVecProgressBtn;


//-----------------------------------------------------
// 投票
//-----------------------------------------------------
struct stTechBallot
{
	std::string strTech;                // 科技名
        DWORD dwBallot;                            // 投票数　
	bool bChecked;                             // 我投这票吗
	DWORD dwOption; // 选项编号

	stTechBallot()
	{
		dwBallot = 0 ; 
		bChecked = false;
	}
};
typedef std::vector<stTechBallot*>   tVecBallotItem;
typedef std::vector<CGuiButton *> tVecCheckBtn;

//-----------------------------------------------------
// 研究员
//-----------------------------------------------------
struct _stWaitOfficialItem
{
	DWORD dwCharID;
	std::string strResearcher;
};

// all the  researcher candidators
typedef std::vector<_stWaitOfficialItem *> tVecResearcher;       

struct stWaitingTech
{
	DWORD dwTechOption; // tech id 
	std::string strTechName; // tech name 
	DWORD dwCharID;      // who will be specified 

	stWaitingTech(){
		dwCharID = 0 ;
	}
};

// all techniques which includes the studying and waiting to be studied 
typedef std::vector<stWaitingTech *> tVecTechniqueItem;    
typedef std::vector<CGuiComboBox *> tVecCombox;
//////////////////////////////////////////////////////////////////////
class CGuiAntiDareCountry;
class CCountryTech
{
public:
	/// constructor
	CCountryTech(void);
	 /// destructor
	~CCountryTech(void);

	/// Debug
	void Debug();

	/// get instance pointer
	static CCountryTech* Instance();

	
	

	//tVecTechniqueItem  m_vecTechWaitingItem;  // All the technique
	

	// studying 
	tVecStudyingItem  m_vecStudyingItem;     // 正在进行中研究
	void SetActiveTechList(const stRtnTechUserCmd * cmd);

	// ballot  
	tVecBallotItem    m_vecBallotItem;	 // 投票
	void SetVoteList(const stReturnVoteListUserCmd * cmd);

	// researcher 
	tVecResearcher     m_vecReasearchItem;  // All the researcher 
	tVecTechniqueItem  m_vecWaitingTech;  // 所有等待被指派研究员的科技列表
	void SetWatingTechList(const stRtnTechUserCmd * cmd);
	void SetResearcherList(const stRtnWaitOfficialUserCmd *cmd);
	bool GetWaitOfficalName(DWORD dwCharID,std::string & strOutter);
        
	void EchoKingSetResearcherEvent( const stConfirmSearcherUserCmd * pCmd );
	

	DWORD GetVoteID(){return m_dwVoteID;};


	void Release();

	// check researcher 
	bool  IsResearcherLegal(DWORD dwTechID);

	// two or more iswaitingteches  have been specified the same researcher
	enum eResult_SpecifyForWatingTech 
	{
		eResult_AllNotSpecified,  // 
		eResult_HasSomeRepeat,
		eResult_HasSomeExistResearcher,
		eResult_Succeeded,
	};

	eResult_SpecifyForWatingTech  CanSpecifyForWaitingTech();

protected:
	static CCountryTech* Singleton;

	//////////////////////////////////////////////////////////
	// ballot 
	DWORD  m_dwVoteID;  // 本次投票id
	BYTE   m_byVoteType;

};
inline  CCountryTech* CCountryTech::Instance()
{
	Assert(Singleton != 0);
	return Singleton;
}

///////////////////////////////////////////////////////////////////////

class CGuiCountryTechDlg : 
	public CGuiDialog
{
public:
	CGuiCountryTechDlg(void);
	~CGuiCountryTechDlg(void);


	void OnClose(void);
	void OnCreate();
	HRESULT OnRender( float fElapsedTime );
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	tVecUpdateBtn     m_vecUpgradeBtn;		 // 升级按钮组
	tVecProgressBtn   m_vecProgressBtn;              // 进度条按钮组
	tVecCheckBtn      m_vecCheckBtn;		 // 投票按钮组
	tVecCombox        m_vecCombox;                   // 指定研究员按钮组

	void Release();

	void OnSetCloseLocation();

	
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	enum  TAB_FLAG
	{
		STUDYING = 0,    // 科技人员升级科技
		BALLOT,      // 国民投票
		RESEARCHER,  // 国王指定研究员		
	};
	void SetSelTab(const TAB_FLAG & nTab);

	void   InitCustomElements();
	bool OnContextMenu();

        // Interface 
	void RefreshCurControl();

protected:

	// studying 
	void Refresh_Studying_ListBox();
	void RefreshProgressBtn();
	void RefreshUpdateBtn();

	// ballot 
	void RefreshCheckBtn();
	void Refresh_Ballot_ListBox();

        // researcher 	
	void Refresh_Researcher_ListBox();
	void RefreshResearcherCombox();

private:
	
	CGuiTab * m_pTab;

	CGuiListBox * m_pListBoxStudying;
	CGuiListBox * m_pListBoxBallot;
	CGuiListBox * m_pListBoxResearcher;

	int      m_iLastFstBallotShowItem;
	int      m_iLastFstStudyingShowItem;
	int      m_iLastFstResearcherShowItem;

	void ShowBollotCtrl(bool bShow);
	void ShowStudyingCtrl(bool bShow);
	void ShowResearchCtrl(bool bShow);

        SIZE  m_ProgressSize;
	SIZE  m_UpdateBtnSize;
	SIZE  m_ComboxSize;

	void  ConfirmBallot();
	void  ConfirmResearcher();

	void __Close__Release();


	bool  m_bFirstTab;

	///////////////////////////////////////////////
	// tooltips 
	CToolTips m_ToolTip;
	stRectI   m_rcListBox;
	void RenderTechToolTip();

	stTechBallot * m_pLastSelectBallot;// just for convenience,because one only can ballot one! [check]

	////////////////////////////////////////////////
	CGuiMenu*	m_menu;
};



///////////////////////////////////////////////////////////////////////
struct  CCountry_stTech
{
	DWORD  dwOption;
	std::string strTech;       // 科技名称
	DWORD       dwTechLevel;       // 科技等级
	std::string strEquip;      // 对应装备
	DWORD       dwEquipLevel;   // 装备等级
};

struct CCountry_stMemeber
{
	std::string strName;      // 官员
	std::string strTitle;     // 头衔
};


typedef std::vector<CCountry_stTech>		CCountryConn_CountryTech;
typedef std::vector<CCountry_stTech>::iterator  CCountryConn_Iterator;
typedef std::vector<CCountry_stMemeber>		CCountryConn_CountryMember;

typedef std::vector<stArmyBaseInfo*> tVecArmyBaseInfo;
typedef std::vector<stArmyBaseInfo*>::const_iterator c_ArmyBaseInfoIterator;


// 返回军队详细信息
struct _stCaptionBase
{
	std::string strCaptionName;
	DWORD dwCharID;
};
struct stRtnArmyItem
{
	std::string strName;// 军队名
	std::string strCity; //  城市名
	std::string strGen; // 将军名
	std::vector<_stCaptionBase> vecBase;  
};
typedef stdext::hash_map<std::string,stRtnArmyItem> tHashMapArmyItem;
typedef stdext::hash_map<std::string,stRtnArmyItem>::const_iterator c_HashMapArmyItemIteator;


struct stGenInfo
{
	DWORD level;
	DWORD exp;
	DWORD maxExp;
	stGenInfo()
	{
                  level = 0;
		  exp = 0;
		  maxExp =0;
	};
};
enum COUNTRYLIST_USAGE 
{
	enumAntiDareCountry,  // 申请国战反攻
	enumAllyCountry,   // 国家联盟
};

class CGuiQueryOccupierDlg;
enum  Occupier_Usage
{ 
	Usage_OccupierTong = 1,
	Usage_OccupierSept = 2,
	Usage_KingSort = 3,
};   

//////////////////////////////////////////////////////////
struct stOccupierTongItem          // 占领查询·帮会列表
{
	std::string strCityName;   // 城市
	std::string strTongName;   // 占领帮会
};

struct stOccupierSeptItem          // 占领查询·家族列表
{
	std::string strNpcPos;     // ＮＰＣ坐标
	std::string strSeptName;   // 占领家族
};

struct stKingSortList
{
	std::string strName;        // 姓名 
	std::string strCountryName; // 国家
	std::string strTotalTime;   // 在位时间 
};

typedef std::list<stOccupierTongItem> ListOccupierTong;
typedef std::list<stOccupierSeptItem> ListOccupierSept;
typedef std::list<stKingSortList> ListKingSort;
typedef std::list<stOccupierTongItem>::iterator  Iter_OccupierTong;
typedef std::list<stOccupierTongItem>::const_iterator  ConstIter_OccupierTong;
typedef std::list<stOccupierSeptItem>::iterator  Iter_OccupierSept;
typedef std::list<stOccupierSeptItem>::const_iterator  ConstIter_OccupierSept;
typedef std::list<stKingSortList>::iterator  Iter_KingSort;
typedef std::list<stKingSortList>::const_iterator  ConstIter_KingSort;


class CCountryConn
{
public:
	CCountryConn();
	~CCountryConn();

	void Debug(const Occupier_Usage & usage);

	CCountryConn_CountryTech      m_vecFinishedTech;
	CCountryConn_CountryMember    m_vecMember;
	tVecArmyBaseInfo m_vecCountryArmyBaseInfo;
	tHashMapArmyItem   m_HashMapArmyItem;

	
	// get singleton pointer 
	static CCountryConn* Instance();

	// network 
	void SetFinishedTechList(const stRtnTechUserCmd* pCmd);

	// init 
	void SetDeaultTechList();

	void Release();

	// country army list 
	 void  SetCountryArmyList(const stRtnArmyListUserCmd* cmd);
	void SetCountryArmyItem(const stRtnArmySpecUserCmd * pCmd);


	CGuiAntiDareCountry  * AddAntiDareCountryDlg(const COUNTRYLIST_USAGE & usage);
	CGuiAntiDareCountry * m_guiAntiDareCountryDlg;

	// afficle
	void AddKingAfficle(const stCountryNoteCmd * pcmd);
	bool GetKingAfficle(std::string & strOut);


	void RequestForKingAffiche();
	void ResetQuest();


	///////////////////////////////////////////////////////////
	void RequestForGenInfo();
	void EchoForGenInfo(const stRetGenCountryCmd  * pCmd);
	CGuiQueryGenDlg  * AddGenInfoDlg();
	CGuiQueryGenDlg * m_guiGenInfoDlg;
        const stGenInfo & GetGenInfo()
	{
		return m_GenInfo;
	}
	///////////////////////////////////////////////////////////
	
	ListOccupierTong   m_ListOccupierTong;
	ListOccupierSept   m_ListOccupierSept;
	ListKingSort       m_ListKingSort;     

	void SetKingSortList(const stRtnKingListCmd * pCmd);
	void SetCityOwnerList(const stRtnCityOwnerListCmd * pcmd);
	void SetNpcOwnerList(const stRtnNpcOwnerListCmd * pCmd);
		 
	CGuiQueryOccupierDlg *  AddOccupierDlg(Occupier_Usage nType);
	CGuiQueryOccupierDlg *  m_guiOccupierDlg;
	//////////////////////////////////////////////////////////


protected:
	// singleton 
	static CCountryConn* Singleton;

	std::string m_strKingAfficle;
	bool m_bQuestForKingAffiched;


	stGenInfo m_GenInfo;
};

inline  CCountryConn* CCountryConn::Instance()
{
	Assert(Singleton != 0);
	return Singleton;
}


//--------------------------------------------------------------------------------
// 国家界面
//--------------------------------------------------------------------------------
#define   ID_COUNTRY_BUTTON_TONG      3
#define   ID_COUNTRY_BUTTON_FRIEND    4 
#define   ID_COUNTRY_BUTTON_SEPT      5 
#define   ID_COUNTRY_BUTTON_SCHOOL    6 
#define   ID_COUNTRY_BUTTON_COUNTRY   7 

class CGuiCountry : 
	public CGuiDialog
{
public:
	CGuiCountry(void);
	~CGuiCountry(void);

	void OnClose(void);
	void OnCreate();
	HRESULT OnRender( float fElapsedTime );
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	void OnSetCloseLocation();

	void RefreshAffiche();
private:
	CGuiListBox * m_pListBoxTech;          // 科技
#ifdef __UN_USED
	CGuiListBox * m_pListBoxMember;        // 官员
#else
	CGuiMLEditBox * m_pMLEditBoxAffiche;       // Country Affiche
#endif
	CGuiListBox * m_pListBoxArmy;          // 军队
	CGuiListBox * m_pListBoxArmyItem;          // 军队列表


	CGuiStatic * m_pStaticCountry;         // 国家
	CGuiStatic * m_pStaticKing;            // 国王
	CGuiStatic * m_pStaticHonor;           // 今日荣毅
	CGuiStatic * m_pStaticTax;             // 今日税率
	CGuiStatic * m_pStaticXking;           //soke 2016 - 3 - 11 国家官员表 - 国王
	CGuiStatic * m_pStaticCatcher;         //soke 2016 - 3 - 11 国家官员表 - 捕头（左上）
	CGuiStatic * m_pStaticCatchex;         //soke 2016 - 3 - 11 国家官员表 - 捕头（左下）
	CGuiStatic * m_pStaticCensor;          //soke 2016 - 3 - 11 国家官员表 - 御史大夫（右上）
	CGuiStatic * m_pStaticCensos;          //soke 2016 - 3 - 11 国家官员表 - 御史大夫（右下）
	CGuiStatic * m_pStaticYuanshuai;       //soke 2016 - 3 - 11 国家官员表 - 元帅
	CGuiStatic * m_pStaticZaixiang;        //soke 2016 - 3 - 11 国家官员表 - 宰相

	
public:
	//////////////////////////////////////////////////////////////////////
	// Refresh ListBox 
//	void Refresh_ListBox_Tech(); //soke 国家科技
#ifdef __UN_USED
	void Refresh_ListBox_Member();
#endif
	void Refresh_ListBox_Army();
	void Refresh_ListBox_ArmyItem();

public:
	//////////////////////////////////////////////////////////////////////
	// For CWorldConnDialog 
	void SwitchToWorldConnDlg(DWORD dwBtnID);
	void SwitchFromWorldConnDlg();

	BOOL IsTongClicked();
	BOOL IsFriendClicked();
	BOOL IsSeptClicked();
	BOOL IsSchoolClicked();
	//////////////////////////////////////////////////////////////////////


private:
	DWORD  m_dwBtnIDClicked;

};

inline 
BOOL CGuiCountry::IsTongClicked()
{
	return ID_COUNTRY_BUTTON_TONG == m_dwBtnIDClicked;
}

inline 
BOOL CGuiCountry::IsFriendClicked()
{
	return ID_COUNTRY_BUTTON_FRIEND == m_dwBtnIDClicked;
}

inline 
BOOL CGuiCountry::IsSeptClicked()
{
	return ID_COUNTRY_BUTTON_SEPT == m_dwBtnIDClicked;
}

inline
BOOL CGuiCountry::IsSchoolClicked()
{
	return ID_COUNTRY_BUTTON_SCHOOL == m_dwBtnIDClicked;
}


//--------------------------------------------------------------------------------
// 创建军队输入框
//--------------------------------------------------------------------------------
class CGuiCreateArmy :
	public CGuiDialog
{
	CGuiEditBox* m_pEditBoxArmyName;   // The Name of Army 
	CGuiComboBox * m_pComboxGenName;      // The gen name of Army 
public:
	CGuiCreateArmy();
	~CGuiCreateArmy();

	void OnClose(void);
	void OnCreate();
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	void RefreashGenList();

private:
	void OnInputEnter();

};

//--------------------------------------------------------------------------------
// 军队列表
//--------------------------------------------------------------------------------
class CGuiArmyListDlg :
	public CGuiDialog
{
public:
	CGuiArmyListDlg(void);
	~CGuiArmyListDlg(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	void OnSetCloseLocation();

	bool OnIDOk();
	bool OnIDCancel();

	void SetText(const char* pszText);
	void AddListItem(const char *szText, void *pData  ,DWORD color  = -1,IBitmaps* icon = NULL);
	int GetCurSelectItem();
	void* GetCurSelectItemData();
	const char* GetCurItemText();
	bool OnContextMenu();

	CGuiListBox	 *	m_pList;		//列表
	char			m_strText[MAX_PATH];	//题头

	void  AddArmyList();
protected:
	////////////////////////////////////////////////
	CGuiMenu*	m_menu;
	stRectI   m_rcListBox;
};


//--------------------------------------------------------------------------------
// 军队人员列表
//--------------------------------------------------------------------------------
class CGuiArmyItemDlg :
	public CGuiDialog
{
public:
	CGuiArmyItemDlg(void);
	~CGuiArmyItemDlg(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	void OnSetCloseLocation();

	bool OnIDOk();
	bool OnIDCancel();

	void SetText(const char* pszText);
	void AddListItem(const char *szText, void *pData  ,DWORD color  = -1,IBitmaps* icon = NULL);
	int GetCurSelectItem();
	void* GetCurSelectItemData();
	const char* GetCurItemText();
	bool OnContextMenu();

	CGuiListBox	 *	m_pList;		//列表
	char			m_strText[MAX_PATH];	//题头

	void AddData(const stRtnArmyItem * pItem);

protected:
	////////////////////////////////////////////////
	CGuiMenu*	m_menu;
	stRectI   m_rcListBox;
};
/////////////////////////////////////////////////////////////////////////////////////////////

typedef std::vector<stWaitGenItem*>   tVecWaitGen;
typedef std::vector<stWaitGenItem*>::iterator WaitGenIterator;
typedef std::vector<stWaitGenItem*>::const_iterator c_WaitGenIterator;

class CArmyManager
{
	tVecWaitGen  m_vecWaitGen;
	
	tVecArmyBaseInfo m_vecCityArmyBaseInfo;
	

	tHashMapArmyItem   m_HashMapArmyItem;

	static CArmyManager * m_Singleton; 

public:
	CArmyManager();
	~CArmyManager();

	static CArmyManager * GetSingleton();
	void SafeRelease();

	 // gui dialog 
	 CGuiCreateArmy *  AddCreateArmyGuiDlg();
	 CGuiArmyListDlg*  AddArmyListDlg();
	 CGuiArmyItemDlg * AddArmyItemDlg();
	 CGuiCreateArmy * m_guiCreateArmyDlg;
	 CGuiArmyListDlg * m_guiArmyListDlg;
	 CGuiArmyItemDlg * m_guiArmyItemDlg;

	 // wait gen list 
	 const tVecWaitGen & GetWaitGenList(){return m_vecWaitGen;};
	 void SetWaitGenList(const stRtnWaitGenUserCmd* pCmd);

	 // army baseinfo 
	 // army list 
	 void QuestCityArmyList();
	 const tVecArmyBaseInfo & GetCityArmyBaseInfo(){return m_vecCityArmyBaseInfo;};
	 void SetCityArmyList(const stRtnArmyListUserCmd* pCmd);

	

	 // city army item 
	 void QuestCityArmyItem(DWORD dwArmyID);
	 void SetCityArmyItem(const stRtnArmySpecUserCmd * pCmd);
	const tHashMapArmyItem &  GetCityArmyItemList(){return m_HashMapArmyItem;};
	 const stArmyBaseInfo *  GetArmyItem(DWORD ArmyItem);
	 const stRtnArmyItem  *  GetArmyDetailItem(const char * ArmyName);
 

	//DiscreteArmy
	void DiscreteArmy();

	// msg copy
	stAddArmyCaptainUserCmd * m_pAddArmyCaptainMsg;
	void ClearAddArmyCaptionUserCmd();
	void CopyAddArmyCaptionUserCmd(const stAddArmyCaptainUserCmd * pCmd);
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
class CGuiAntiDareCountry :
	public CGuiDialog
{
	COUNTRYLIST_USAGE  m_dlgUsage;

	CGuiListBox*   m_pListBox;

public:
	CGuiAntiDareCountry(const COUNTRYLIST_USAGE & usage);


	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	void OnSetCloseLocation();
};

//--------------------------------------------------------------------------------
// 查询占领者帮会和家族
//--------------------------------------------------------------------------------
class CGuiQueryOccupierDlg : public CGuiDialog
{	
private:
	Occupier_Usage m_Usage;

public:
	CGuiQueryOccupierDlg(const Occupier_Usage & usage);

	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl * pControl);

	void OnSetCloseLocation();

	void UpdataList();
};


