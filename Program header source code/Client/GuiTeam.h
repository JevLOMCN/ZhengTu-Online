#pragma once
#include "..\gui\include\guidialog.h"
#include "./Character.h"
#include "GameScene.h"
#include "MainCharacter.h"

#define  TEAMHEADCOLOR    D3DCOLOR_ARGB( 255, 39, 90, 0 )

struct stReturnTeamMemberPosition;

enum enumTeamStatus
{
	DIED = 0,
	OFFLINE = 1,
	AWAY=2,   //default 
	NORMAL,
};
struct stTeamMember {
	stTeamMemberData data;
	char szMapName[MAX_NAMESIZE];
	int x,y;
	enumTeamStatus status;
};

/**
 * \brief 简短描述 :     组队界面
 * 
 * 详细描述 :
 *	    主界面中的“组队”按钮 将被 替换为“附近玩家”按钮；
 *         队伍上限人数从原来的10人减少到6人，队伍成员的头像将按图中的形式 表现在玩家的界面上 
*          队长的头像边上将有 “五角星” 图标.
 *        头像上的颜色 代表其的“HP”状态 绿色代表剩余的“HP”值，红色代表损耗的“HP”值；
 *        人物死亡或掉线 头像变成灰色；人物离开队伍则移除头像。
 * 
 */
class CCharacter;
class CNpc;

class CTeamInfo
{
	CNpc* m_pLeaderAttackNpc;
public:
	CTeamInfo(void);
	void SetLeaderAttackNpc(CNpc* pNpc) { m_pLeaderAttackNpc = pNpc; }

	CNpc* GetLeaderAttackNpc() { return m_pLeaderAttackNpc; }
};

CTeamInfo* GetTeamInfo();

class CGuiTeam :
	public CGuiDialog
{
private:
	DWORD m_dwSelMemberID;

	void RenderTeamTip();
	
public : 
	DWORD		 m_dwCurExpAssign;
	DWORD		 m_dwCurObjAssign;
	void             SetPickupItemAssing(DWORD dwAssign);
	void		 SetExpAssign(DWORD dwAssign);
        void		 SetExpMode(stExpModeTeamMemberUserCmd * pCmd); 
	void		 SetItemMode(stObjModeTeamMemberUserCmd * pCmd);
	void             SetTeamMode(stTeamModeUserCmd * pCmd);

	
	typedef std::vector<stTeamMember> tListTeamMemvber;
	tListTeamMemvber m_aTeamMember; //存储队员信息的数组
	DWORD         m_dwHeadID;       // 队长的ID信息
 
	DWORD m_dwRefreshTime;                   // 绘制刷新时间间隔
	DWORD m_dwRefreshHPTime;             //  绘制血条刷新时间间隔
	
	SkillBase_t* m_pActiveSkill;

	int      m_iRightX;
	int      GetRightX(){return m_iRightX;}

	CGuiTeam(void);
	void OnCreate();
	void OnClose(void);
	void OnInitScript();
	HRESULT OnRender(float fElapsedTime);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool OnContextMenu();
	BOOL ContainsPoint( POINT ptScreen );
	void RefreshCharacter();
	void OnResetScreenSize();
	DWORD OnHiTest(POINT ptScreen);
	bool IsTeam()
	{
		return m_dwTeramID != 0;
	}
	bool IsTeamHead2(DWORD dwTempID)
	{
		return IsTeam() && dwTempID == m_dwHeadID;
	}

	tListTeamMemvber::iterator FindMemberIt(const char* pszName)
	{
		for(tListTeamMemvber::iterator it = m_aTeamMember.begin() ; it != m_aTeamMember.end(); ++it)
		{
			if(strcmp((*it).data.pstrName,pszName) == 0)
			{
				return it;
			}
		}
		return m_aTeamMember.end();
	}

	tListTeamMemvber::iterator FindMemberIt(DWORD dwTempID)
	{
		for(tListTeamMemvber::iterator it = m_aTeamMember.begin() ; it != m_aTeamMember.end(); ++it)
		{
			if( (*it).data.dwTempID == dwTempID )
			{
				return it;
			}
		}
		return m_aTeamMember.end();
	}


	bool IsTeamHead(DWORD dwTempID)
	{
		for(tListTeamMemvber::iterator it = m_aTeamMember.begin() ; it != m_aTeamMember.end(); ++it)
		{
			if( (*it).data.dwTempID == dwTempID )
			{
				CCharacter * pChar = GetScene()->FindCharacterByID(dwTempID);
				if( pChar && pChar->GetTeamState() == TEAM_STATE_LEADER)
					return true;
				break;
			}
		}
		return false;
	}

	stTeamMemberData* FindMember(const char* pszName)
	{
		tListTeamMemvber::iterator it = FindMemberIt(pszName);
		if(it == m_aTeamMember.end()) return NULL;
		return &(*it).data;
	}
	stTeamMemberData* FindMember(DWORD dwTempID)
	{
		tListTeamMemvber::iterator it = FindMemberIt(dwTempID);
		if(it == m_aTeamMember.end()) return NULL;
		return &(*it).data;
	}

	int GetTeamMemberAmongRange(const int& dis);

	void AddTeamMember(stAddTeamMemberUserCmd* pData);
	void RemoveTeamMember(DWORD dwTempID);

	void OnTeamerOffLine(stTeamMemberOfflineUserCmd * pCmd);

	void RefreshButtonState();
	void BreakTeam();
	void RefreshHP();
	void RefreshHP(stTeamMemberData * pData);
	void RemoveTeamMemberData(DWORD dwTempID);
	void SetTeamSettings(stSystemSettings* pData);
	void RequestTeamPosition();
	void SetTeamPosition(stReturnTeamMemberPosition* pCmd);
	
	void ToAddTeamMember(const char* pszName,DWORD dwMode = TEAM_NORMAL);
	void ToAddTeamMember(DWORD dwTempID,DWORD dwMode);

	void LeaveTeam();
	

	CCharacter* GetMouseOnMember();
	CCharacter* GetSelMember();
	int	GetMouseOnMemberNo();
	DWORD GetSelMemberID() { return m_dwSelMemberID; }
	void UseSkillOnTeamMember( bool bHas = true);
	SkillBase_t* GetActiveSkill() { return m_pActiveSkill; }

	CCharacter* FindCharacterByName(const char* pszName,DWORD dwTempID);
	DWORD m_dwTeamMode;

	//////////////////////////////////////////////////////////////////
	void RefreshTeamMember(const stFreshTeamMemberUserCmd * pCmd);

	static bool AddMember(const char* name,DWORD type);
	static bool AddMember(DWORD dwTempID,DWORD type);

private:

	void FrameRefreashTeamerStatus(stTeamMember & member);
	// Bar 
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
	void RenderBar(CGuiTeam::stBarShowInfo& bar,size_t iCur,size_t iMax,int index,int OffsetX,int OffsetY);
	//////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////
	IBitmaps * m_pTeamExpAssignByLevel;
	IBitmaps * m_pTeamExpAssignBySpecial;
	IBitmaps * m_pTeamItemAssignByFreedom;
	IBitmaps * m_pTeamItemAssignByPoll;
	stRectI    m_rcClipTeamExpAssignByLevel;
	stRectI    m_rcClipTeamExpAssignBySpecial;
	stRectI    m_rcClipTeamItemAssignByFreedom;
	stRectI    m_rcClipTeamItemAssignByPoll;

	stResourceLocation rl;				//队员头像的外圈图标
	stResourceLocation rl1;				//队员死亡或掉线后头像上覆盖的灰色图片
	stResourceLocation rlbk;			//组队界面的背景图片（只包含一个圆圈）
	stResourceLocation rlhead;			//队员的头像图标
	stResourceLocation rldied;			//soke 死亡
	stResourceLocation rloff;			//soke 离线
	stResourceLocation rlayaw;			//soke 远离

	stPointI m_ptOffsetModeFlag;
	//////////////////////////////////////////////////////////////////

	DWORD		 m_dwTeramID;			//队伍的ID

	IBitmaps*	 m_pTeamIcon;			//队员头像的外圈图标
	IBitmaps*	 m_pTeamIcon1;			//队员死亡或掉线后头像上覆盖的灰色图片
	IBitmaps*	 m_pTeamBkIcon;		//组队界面的背景图片（只包含一个圆圈）
	IBitmaps*	 m_pTeamHeadIcon;		//队长的五角星图标
	IBitmaps*	 m_pTeamDiedIcon;		//soke 死亡 
	IBitmaps*	 m_pTeamOffIcon;		//soke 离线
	IBitmaps*	 m_pTeamAyawIcon;		//soke 远离

	IBitmaps*        m_pTeamFlag;           // 组队模式标志
	/////////////////////////////////////////////////////////////////


	CGuiMenu*	m_menu;				//点击队员的头像图标弹出的右键菜单


private:
	char	m_szBackupName[MAX_NAMESIZE+1];
public:
        void BackUpAnswerUserName(const char * pCmdUserName);
	const char * GetBackUpAnswerUserName(){return m_szBackupName;};
};
