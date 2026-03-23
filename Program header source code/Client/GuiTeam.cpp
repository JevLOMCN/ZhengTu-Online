#include "public.h"
#include "./GameGuiManager.h"
#include "./Game.h"
#include ".\guiteam.h"
#include "./GameScene.h"
#include "./Character.h"
#include "./MainCharacter.h"
#include "./GameAppation.h"
#include "./GuiMain.h"
#include "./GuiFloatAccessKey.h"
#include "./GameCursor.h"
#include "GuiWorldConn.h"

#define IDC_ADDMEMBER	    100
#define IDC_REMOVEMEMBER   101
#define IDC_EXITTEAM		    102
#define IDC_ALLOWTEAM	    103
#define IDC_TEAMLISTBOX	     3		
#define IDC_CHARLISTBOX	     4


#define __REMOTE__CAN__MAKE__TEAM__    // 远程组队

enum enmTEAMCMD
{
	CMD_DELETEONE,
	CMD_ADDFRIEND,
	CMD_NUM
};


//------------------------------------------------------------------
static char* szCmdName[CMD_NUM] = {"解除邀请","加为好友"};
static bool cmdCanUse[CMD_NUM] = { false,true };
char* teamMemberName[6] ={"","","","","",""};       
int  g_CurMemerID = -1;

struct STATUS_TO_COLOR
{
        enumTeamStatus status;
	DWORD dwCoverClr;
	//DWORD dwTextClr;
	char szDesc[50];
};
static STATUS_TO_COLOR c_status_clr[3]   = 
{
	{ DIED,    D3DCOLOR_ARGB(255,255,255,255),/* D3DCOLOR_ARGB(255,255,14,82)  ,*/"死亡" },     // 死亡
	{ OFFLINE, D3DCOLOR_ARGB(255,164,166,164),      /*D3DCOLOR_ARGB(255,173,234,41), */"离线" },      // 离线
	{ AWAY,	   D3DCOLOR_ARGB(255,255,194,58),	  /* D3DCOLOR_ARGB(255,255,255,255),*/"远离,距离你九屏之外" },      //九屏之外
};


static stPointI s_ptTeamExpAssign;
static stPointI s_ptTeamObjAssign;
//------------------------------------------------------------------


CTeamInfo* GetTeamInfo()
{
	static CTeamInfo g_TeamInfo;  //static 到这里，只有用到才会分配内存 
	return &g_TeamInfo;
}

CTeamInfo::CTeamInfo(void)
: m_pLeaderAttackNpc( NULL )
{

}

/**
 * \brief 简短描述 构造函数
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CGuiTeam::CGuiTeam(void)
: m_dwRefreshTime(0)
, m_dwTeramID(0)
, m_dwRefreshHPTime(0)
, m_pTeamFlag(NULL)
, m_dwTeamMode(TEAM_MAX)
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;

	m_bSmDialog = true;

	///////////////////
	rlbk.SetFileName("data\\interfaces.gl");
	rlbk.group = 24;	
	rlbk.frame = 3;
	m_pTeamBkIcon = GetDevice()->FindBitmaps(&rlbk);     // 白圈
	//////////////////

	rlhead.SetFileName("data\\interfaces.gl");
	rlhead.group = 24;	 
	rlhead.frame = 1;
	m_pTeamHeadIcon = GetDevice()->FindBitmaps(&rlhead);     // 五角星
	///////////////////////
	
	rldied.SetFileName("data\\interfaces1.gl");
	rldied.group = 1;	 
	rldied.frame = 2;
	m_pTeamDiedIcon = GetDevice()->FindBitmaps(&rldied);     // 死亡
	///////////////////////
		
	rloff.SetFileName("data\\interfaces1.gl");
	rloff.group = 1;	 
	rloff.frame = 1;
	m_pTeamOffIcon = GetDevice()->FindBitmaps(&rloff);     // 离线
	///////////////////////
			
	rlayaw.SetFileName("data\\interfaces1.gl");
	rlayaw.group = 1;	 
	rlayaw.frame = 0;
	m_pTeamAyawIcon = GetDevice()->FindBitmaps(&rlayaw);     // 远离
	///////////////////////
	
	///////////////////////
	rl1.SetFileName("data\\interfaces.gl");
	rl1.group = 33;	 //soke 组队显示的头像库


	m_pActiveSkill = NULL;

	m_iRightX = 0;

	m_pTeamExpAssignByLevel = NULL;
	m_pTeamExpAssignBySpecial = NULL ;
	m_pTeamItemAssignByFreedom = NULL;
	m_pTeamItemAssignByPoll = NULL;


	m_dwCurExpAssign = TEAM_EXP_MODE_NORMAL;
	m_dwCurObjAssign = TEAM_OBJ_MODE_NORMAL;


	m_ptOffsetModeFlag.x = 5;
	m_ptOffsetModeFlag.y = 124;

 

	s_ptTeamExpAssign.x = m_ptOffsetModeFlag.x;
	s_ptTeamExpAssign.y = m_ptOffsetModeFlag.y + 15;

	s_ptTeamObjAssign.x = m_ptOffsetModeFlag.x;
	s_ptTeamObjAssign.y = m_ptOffsetModeFlag.y + 30;

 
	FUNCTION_END;
}

/**
 * \brief 简短描述 窗口关闭
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiTeam::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiTeam = NULL;

	if ( m_menu )
	{
		m_menu->Close();
		m_menu = NULL;
	}

	if (  m_pActiveSkill )
		m_pActiveSkill = NULL;

	FUNCTION_END;
}

/**
 * \brief 简短描述 窗口创建
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiTeam::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

    //soke 组队显示的底图
	rl.SetFileName("data\\interfaces.gl");
	rl.group = 24;	 
	rl.frame = 0;
	m_pTeamIcon = GetDevice()->FindBitmaps(&rl);        // 底图（整圆）
	
	stResourceLocation rl1;
	rl1.SetFileName("data\\interfaces.gl");
	rl1.group = 22;	 
	rl1.frame = 21;

	RefreshButtonState();
	SetVisible(false);


	///////////////////////////////////////////////////////////////////////////
	//Get the two bar
	IBitmapList* pBmpList = GetDevice()->FindResource(GetGuiGraphicPackName());

	m_aBar.resize(0);
	stBarShowInfo bar;
	m_aBar.reserve(2);
	bar.pBmp = pBmpList->LoadFrame(24,4); //soke 血
	bar.rcClip = bar.pBmp->GetClipRect();
	bar.type = BarTypeVert;
	m_aBar.push_back(bar);

	bar.pBmp = pBmpList->LoadFrame(24,5); //soke 蓝
	bar.rcClip = bar.pBmp->GetClipRect();
	bar.type = BarTypeVert;
	m_aBar.push_back(bar);
	///////////////////////////////////////////////////////////////////////////


	/////////////fancui
	m_menu = GetGuiManager()->CreateMenu( this );
	for ( int i=0;i<CMD_NUM;i++ )
		m_menu->AddMenuItem( i, szCmdName[i], CGuiMenu::MenuStyle_Command, cmdCanUse[i] );
	SetPopupMenu( m_menu );
	//end of fancui



       // ----------------------------------------------------------
	stResourceLocation rlTemp;
	rlTemp.SetFileName(GetGuiGraphicPackName());
	rlTemp.group = 24;
	rlTemp.frame = 9;  // 0009 经验等级.tga
	m_pTeamExpAssignByLevel = GetDevice()->FindBitmaps(&rlTemp);


	rlTemp.SetFileName(GetGuiGraphicPackName());
	rlTemp.group = 24;
	rlTemp.frame = 10;  //  0010经验特殊.tga
	m_pTeamExpAssignBySpecial = GetDevice()->FindBitmaps(&rlTemp);


	rlTemp.SetFileName(GetGuiGraphicPackName());
	rlTemp.group = 24;
	rlTemp.frame = 11;  //  0011物品自由.tga
	m_pTeamItemAssignByFreedom = GetDevice()->FindBitmaps(&rlTemp);

	rlTemp.SetFileName(GetGuiGraphicPackName());
	rlTemp.group = 24;
	rlTemp.frame = 12;  //  0012物品轮流.tga
	m_pTeamItemAssignByPoll = GetDevice()->FindBitmaps(&rlTemp);


	IBitmap * pTempBitmap = NULL;
	if(m_pTeamExpAssignByLevel) 
	{
		pTempBitmap  = m_pTeamExpAssignByLevel->GetBitmap(0);
		if( pTempBitmap )
			m_rcClipTeamExpAssignByLevel = pTempBitmap->GetClipRect();
	}
	if( m_pTeamExpAssignBySpecial)
	{
		pTempBitmap = m_pTeamExpAssignBySpecial->GetBitmap(0);
		if( pTempBitmap)
			m_rcClipTeamExpAssignBySpecial = pTempBitmap->GetClipRect();
	}

	if( m_pTeamItemAssignByFreedom)
	{
		pTempBitmap = m_pTeamItemAssignByFreedom->GetBitmap(0);
		if( pTempBitmap)
			m_rcClipTeamItemAssignByFreedom = pTempBitmap->GetClipRect();
	}

	if( m_pTeamItemAssignByPoll)
	{
		pTempBitmap = m_pTeamItemAssignByPoll->GetBitmap(0);
		if( pTempBitmap)
			m_rcClipTeamItemAssignByPoll = pTempBitmap->GetClipRect();
	}
	//----------------------------------------------------------------------


	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiTeam::RefreshButtonState()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}


/**
* \brief 绘制选中NPC的生命、魔法、体力等
* 
* 
* 
* \param bar : 相应显示条
* \param iCur : 当前值
* \param iMax : 最大值
* \param index : 种类
* \return 
*/
void CGuiTeam::RenderBar( CGuiTeam::stBarShowInfo& bar,size_t iCur,size_t iMax,int index ,int OffsetX,int OffsetY)
{
	FUNCTION_BEGIN;

	if(!bar.pBmp) return;

	if ( g_bSinglePlayer )
	{
		iCur = 100;
		iMax = 200;
	}


	stRectI rc = bar.rcClip;
	stPointI pt(rc.left,rc.top);
	if( iMax > 0 )
	{
		if(bar.type == BarTypeVert)
		{
			rc.top = bar.rcClip.bottom - (long)(bar.rcClip.Height() * iCur/iMax);
			pt.y = rc.top;
		}
		else
		{
			rc.right = bar.rcClip.left + bar.rcClip.Width() * iCur/iMax;
			pt.x = rc.left;
		}

		pt.x += m_x;
		pt.y += m_y;

		bar.pBmp->Render( pt.x + OffsetX, pt.y + OffsetY, &rc, NULL );
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
void CGuiTeam::OnInitScript()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiTeam::RefreshCharacter()
{
	FUNCTION_BEGIN;

	SetFrameAllocator fa;
	std::vector< CCharacter*, std::stack_allocator<CCharacter*> > aCharacter;

	GetScene()->GetObjectOrderByDistance(aCharacter,type2type_t<CCharacter>());

	char szCurSel[MAX_NAMESIZE];
	szCurSel[0] = 0;
	FUNCTION_END;
}

/**
 * \brief 简短描述 刷新血条
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiTeam::RefreshHP()
{
	FUNCTION_BEGIN;

	//if( xtimeGetTime() - m_dwRefreshHPTime > 1000 )
	{
		m_dwRefreshHPTime = xtimeGetTime();
		for(tListTeamMemvber::iterator it = m_aTeamMember.begin() ; it != m_aTeamMember.end(); ++it)
		{
			RefreshHP(&(*it).data);
		}
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述 刷新血条
 * 
 * 详细描述
 * 
 * \param pData : 描述
 * \return 返回值的描述
 */
void CGuiTeam::RefreshHP(stTeamMemberData * pData)
{
	FUNCTION_BEGIN;
					
	CCharacter* pChar = GetScene()->FindCharacterByID((*pData).dwTempID);
	if( pChar && (pChar != GetScene()->GetMainCharacter()) )
	{
		static int hp = (*pData).dwHealth;

		//触发"队员头像"事件
		/*if(bIsUserUnSetted(USER_SETTING_HELP_MEMBERHEAD))
		{
			if( hp < (*pData).dwHealth )
				ShowHelpTip(USER_SETTING_HELP_MEMBERHEAD);
			hp = (*pData).dwHealth;
		}*/

		//if(hp > (*pData).wdHealth)
		{
			// 2005-8-12 13:20 depricated , 会改变正常的值
			/*pChar->SetHP( (*pData).wdHealth );
			pChar->SetMaxHP( (*pData).wdMaxHealth);
			pChar->SetMP((*pData).wdMp);
			pChar->SetMaxMP((*pData).wdMaxMp);*/
		}		
	}

	FUNCTION_END;
}



/**
* \brief
*      根据队员名字来查找一个玩家,如果是蒙面人，则根据临时id再找一边
* 详细描述: 
* \param  pszName : 人名
* \return 
*/
CCharacter* CGuiTeam::FindCharacterByName(const char* pszName,DWORD dwTempID)
{
	FUNCTION_BEGIN;

	CCharacter * pCharacter = NULL;
	pCharacter = GetScene()->FindCharacterByName(pszName);
	if( pCharacter == NULL )
	{            
	    // 可能是蒙面人，名字被改变,用TempID
	    // 蒙面人是队伍中的名字没有变，但游戏中名字已改
             pCharacter = GetScene()->FindCharacterByID(dwTempID);
	   
	}
	return pCharacter;

	FUNCTION_END;
}


void CGuiTeam::OnResetScreenSize()
{
	FUNCTION_BEGIN;

	SetLocation(0,0);

	FUNCTION_END;
}

DWORD CGuiTeam::OnHiTest(POINT ptScreen)
{
	FUNCTION_BEGIN;

	return CGuiDialog::OnHiTest(ptScreen);

	FUNCTION_END;
}
/**
 * \brief 简短描述 判断右键点击的点是否含有颜色值
 * 
 * 详细描述 通过颜色值的判断来确定所点击的点是否在头像图标上
 * 
 * \param ptScreen : 描述 屏幕上的点
 * \return 返回值的描述 真表示点在头像上
 */
BOOL CGuiTeam::ContainsPoint( POINT ptScreen )
{
	FUNCTION_BEGIN;
	
	if(CGuiDialog::ContainsPoint(ptScreen)) return TRUE;

	if(m_pTeamIcon && m_pTeamIcon->GetBitmapCount() &&m_aTeamMember.size())
	{

		IBitmap * pBmp = m_pTeamIcon->GetBitmap(0);

		RECT rc;
		rc.left=0;
		rc.right=200;
		rc.top=0;
		rc.bottom=200;
		int j=0;
		for(int i=0;i<m_aTeamMember.size();i++)
		{
			//if(FindCharacterByName(m_aTeamMember[i].data.pstrName,m_aTeamMember[i].data.dwTempID)==GetScene()->GetMainCharacter())
			//
#ifdef _DEBUG
		//	const char * pOwnerName = GetScene()->GetMainCharacter()->GetName();
		//	int  id = GetScene()->GetMainCharacter()->GetID();
		
#endif _DEBUG
			if( strcmp(m_aTeamMember[i].data.pstrName,GetScene()->GetMainCharacter()->GetName()) == 0 )
			{
				j=1;
			}
			else
			{
				
				stPointI pt(0,30+81*(i-j));

				pt = ptScreen - pt;
				//TRACE("ptScreen:(%d,%d)\n",ptScreen.x,ptScreen.y);
				//TRACE("pt:(%d,%d)\n",pt.x,pt.y);
				RECT rcClip = pBmp->GetClipRect();
				//TRACE("rcClip:(%d,%d,%d,%d)\n",rcClip.left,rcClip.top,rcClip.right,rcClip.bottom); 
				if(pBmp->IsAt(pt,false)) 
					return TRUE;
					
			}
		}
	}
	return FALSE;

	FUNCTION_END;
}

int CGuiTeam::GetMouseOnMemberNo()
{
	FUNCTION_BEGIN;

	stPointI pt = Engine_GetCursor()->GetPosition();
	int j=0;
	for( int i=0;i<m_aTeamMember.size();i++ )
	{
		if( strcmp(m_aTeamMember[i].data.pstrName,GetScene()->GetMainCharacter()->GetName()) == 0 )
		{
			j=1;
			continue;
		}
		stPointI ptCursor = Engine_GetCursor()->GetPosition();
		stRectI rcClip;
		rcClip.left=16;
		rcClip.right=70;
		rcClip.top = 130+81*(i-j);
		rcClip.bottom =rcClip.top+40;
		//Engine_WriteLogF("矩形 宽:%d,高:%d\n",rcClip.Width(),rcClip.Height());
		if ( rcClip.PtInRect( ptCursor ) )
			return i;
	}

	return -1;

	FUNCTION_END;
}

CCharacter* CGuiTeam::GetSelMember()
{
	FUNCTION_BEGIN;
	
	//return GetScene()->FindCharacterByName(m_aTeamMember[m_dwSelMemberID].data.pstrName);
	return GetScene()->FindCharacterByID( m_dwSelMemberID );

	FUNCTION_END;
}

CCharacter* CGuiTeam::GetMouseOnMember()
{
	FUNCTION_BEGIN;

	int id = GetMouseOnMemberNo();

	if ( id != -1 )
	{
		return FindCharacterByName(m_aTeamMember[id].data.pstrName,m_aTeamMember[id].data.dwTempID);
	}
		

	return NULL;

	FUNCTION_END;
}

/**
 * \brief 简短描述 渲染函数
 * 
 * 详细描述 绘制组队界面
 * 
 * \param fElapsedTime : 描述 绘制的时间间隔
 * \return 返回值的描述 绘制是否成功描述
 */
HRESULT CGuiTeam::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	
	//soke 使用新的底图了，这个就取消了
	GetImage( 0 )->SetVisible( false );
		
	if(m_pTeamIcon&&m_aTeamMember.size())
	{
		RECT rc;
		rc.left=0;
		rc.right=200;
		rc.top=0;
		rc.bottom=200;
		int j=0;

		bool bSelfTeamer = true;
		for(int i=0;i<m_aTeamMember.size();i++)
		{
			//CCharacter * pCharacter = FindCharacterByName(m_aTeamMember[i].data.pstrName,m_aTeamMember[i].data.dwTempID) ; 
						
			if( GetScene()->GetMainCharacter()->GetID() == m_aTeamMember[i].data.dwTempID)
			{
				j=1; // 玩家自己是不显示的，所以跳过这个栏位1
				continue;
			}
			else
			{				
				//soke 调整新的底图位置
				//m_pTeamIcon->RenderAni(0,81*(i-j) ,xtimeGetTime(),&rc,0,-1,Blend_Null);  // 全圆底
				m_pTeamIcon->RenderAni( 11,30 + 81*(i-j) ,xtimeGetTime(),&rc,0,-1,Blend_Null);  // 全圆底
				rl1.frame = m_aTeamMember[i].data.wdFace;
				m_pTeamIcon1 = GetDevice()->FindBitmaps(&rl1);  
				assert(m_pTeamIcon1);

				stPointF scale(0.6f,0.6f);
				if( m_pTeamIcon1)
				{
					m_pTeamIcon1->RenderAni(34  ,130+81*(i-j),xtimeGetTime(),NULL,&scale,-1,Blend_Null);// 头像
				}
				else
				{
					#if defined _DEBUG || defined RELEASE_TEST
						Engine_WriteLogF("对于队员脸型不对,收到face:%d,队员名字为:%s\n",m_aTeamMember[i].data.wdFace,m_aTeamMember[i].data.pstrName);
					#endif
				}

				
				


				//soke 使用新的血条、蓝
				//RenderBar(m_aBar[0],m_aTeamMember[i].data.dwHealth,m_aTeamMember[i].data.dwMaxHealth,0,0,81*(i-j));
				//RenderBar(m_aBar[1],m_aTeamMember[i].data.dwMp,m_aTeamMember[i].data.dwMaxMp,1,0,81*(i-j));
				RenderBar(m_aBar[0],m_aTeamMember[i].data.dwHealth,m_aTeamMember[i].data.dwMaxHealth,0,11,30 + 81*(i-j));
				RenderBar(m_aBar[1],m_aTeamMember[i].data.dwMp,m_aTeamMember[i].data.dwMaxMp,1,11,30 + 81*(i-j));
                              
                //soke 队员状态
				FrameRefreashTeamerStatus(m_aTeamMember[i]);
		        assert(m_aTeamMember[i].status >= DIED && m_aTeamMember[i].status <= NORMAL);
				enumTeamStatus status = m_aTeamMember[i].status;
		//		if( NORMAL != status )
		//		{
		//			m_pTeamBKIcon->RenderAni(11  ,90+81*(i-j),xtimeGetTime(),&rc,0, c_status_clr[status].dwCoverClr,Blend_Null); //白圈
		//		}
		
		        if( DIED == status )
				{
					m_pTeamDiedIcon->RenderAni(34  ,130+81*(i-j),xtimeGetTime(),&rc,0, c_status_clr[status].dwCoverClr,Blend_Null); //soke 死亡
				}
				if( OFFLINE == status )
				{
					m_pTeamOffIcon->RenderAni(34  ,130+81*(i-j),xtimeGetTime(),&rc,0, c_status_clr[status].dwCoverClr,Blend_Null); //soke 离线
				}
				if( AWAY == status )
				{
					m_pTeamAyawIcon->RenderAni(34  ,130+81*(i-j),xtimeGetTime(),&rc,0, c_status_clr[status].dwCoverClr,Blend_Null); //soke 远离
				}
	
				SetVisible(true);
				teamMemberName[i-j]=m_aTeamMember[i].data.pstrName;
				if(IsTeamHead2(m_aTeamMember[i].data.dwTempID))
				{
					bSelfTeamer = false;
				}
				stPointI ptCursor = Engine_GetCursor()->GetPosition();
				stRectI rcClip;
				rcClip.left=23;
				rcClip.right=63;
				rcClip.top = 130+81*(i-j);
				rcClip.bottom =rcClip.top+40;
				bool bMouseOver = rcClip.PtInRect( ptCursor );

				if(bMouseOver)
				{
					g_CurMemerID = m_aTeamMember[i].data.dwTempID;
					if( NORMAL != status )
					{
						CToolTips tips1;
						tips1.SetBkColor( D3DCOLOR_ARGB(128,0,0,0) );
						tips1.SetAlignType( GUI_ALIGN_LEFT );
						tips1.SetBorderColor( 0 );
						tips1.SetLineGaps( 3 );
						tips1.SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
						tips1.AddText( m_aTeamMember[i].data.pstrName);
						tips1.AddText(c_status_clr[status].szDesc);
						tips1.Resize();
						stPointI ptTips1;
						ptTips1.x = rcClip.right;
						ptTips1.y = rcClip.top+20;		
						tips1.Render( ptTips1.x, ptTips1.y );	
					}
					else
					{
						CToolTips tips;
						tips.SetBkColor( D3DCOLOR_ARGB(128,0,0,0) );
						tips.SetAlignType( GUI_ALIGN_LEFT );
						tips.SetBorderColor( 0 );
						tips.SetLineGaps( 3 );
						tips.SetCurColor( D3DCOLOR_ARGB(255,255,255,255) );
						tips.AddText( m_aTeamMember[i].data.pstrName);
						tips.SetCurColor( D3DCOLOR_ARGB(255,152,244,23) );
						char szValue[80];
						sprintf(szValue,"\n生命值：%u/%u",m_aTeamMember[i].data.dwHealth,m_aTeamMember[i].data.dwMaxHealth);
						tips.AddText(szValue);
						sprintf(szValue,"\n法术值：%u/%u",m_aTeamMember[i].data.dwMp,m_aTeamMember[i].data.dwMaxMp);
						tips.AddText(szValue);

						
						tips.Resize();

						stPointI ptTips;
						ptTips.x = rcClip.right;
						ptTips.y = rcClip.top;		
						tips.Render( ptTips.x, ptTips.y );	

					}
					//status tip 
                                 }

		}//end for(int i=0;i<m_aTeamMember.size();i++)

		if( !bSelfTeamer )
		{
			m_pTeamHeadIcon->RenderAni(m_ptOffsetModeFlag.x,m_ptOffsetModeFlag.y,xtimeGetTime(),-1,Blend_Null);
			if( m_dwTeamMode == TEAM_NORMAL)
			{
				if(m_dwCurExpAssign == TEAM_EXP_MODE_NORMAL)
				{
					if( m_pTeamExpAssignByLevel ) 
						m_pTeamExpAssignByLevel->RenderAni(s_ptTeamExpAssign.x,s_ptTeamExpAssign.y,xtimeGetTime(),-1,Blend_Null);
				}
				else if (m_dwCurExpAssign == TEAM_EXP_MODE_SPECIAL)
				{
					if(m_pTeamExpAssignBySpecial) 
						m_pTeamExpAssignBySpecial->RenderAni(s_ptTeamExpAssign.x,s_ptTeamExpAssign.y,xtimeGetTime(),-1,Blend_Null);
				}

				if(m_dwCurObjAssign == TEAM_OBJ_MODE_NORMAL)
				{
					if(m_pTeamItemAssignByFreedom) 
						m_pTeamItemAssignByFreedom->RenderAni(s_ptTeamObjAssign.x,s_ptTeamObjAssign.y,xtimeGetTime(),-1,Blend_Null);
				}
				else if(m_dwCurObjAssign == TEAM_OBJ_MODE_SPECIAL)
				{
					if(m_pTeamItemAssignByPoll) 
						m_pTeamItemAssignByPoll->RenderAni(s_ptTeamObjAssign.x,s_ptTeamObjAssign.y,xtimeGetTime(),-1,Blend_Null);
				}
			
			}
		}

		RenderTeamTip();
	}
	}
	else
	{
		SetVisible(false);
	}

	if(xtimeGetTime() - m_dwRefreshTime > 2000)
	{
		m_dwRefreshTime = xtimeGetTime();
		RefreshCharacter();

	}

	

	return hr;

	FUNCTION_END;
}


/**
 * \brief 简短描述 GUI事件响应
 * 
 * 详细描述 在界面上触发的事件在此响应
 * 
 * \param nEvent : 描述 事件类型
 * \param nID : 描述  被触发的对象id
 * \param pControl : 描述 被触发的对象指针
 * \return 返回值的描述 响应是否成功
 */
bool CGuiTeam::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	switch( nEvent )
	{
	case EVENT_DIALOG_COMMAND:
		{
			switch( nID )
			{
			case CMD_DELETEONE:
				{
					if(IsTeamHead(GetScene()->GetMainCharacter()->GetID()))
					{
						stRemoveTeamMemberUserCmd cmd;
						cmd.dwTeamID = m_dwTeramID;
						cmd.dwTempID  = g_CurMemerID ;
						SEND_USER_CMD(cmd);
					}
					else
					{
						GetGameGuiManager()->AddClientSystemMessage("您不是队长，没有剔人权限！");
					}
					return true;
				}
				break;
			case CMD_ADDFRIEND:
				{
					if( g_CurMemerID != -1)
					{
						CCharacter * pCharacter = GetScene()->FindCharacterByID(g_CurMemerID);
						if (pCharacter)
							AddFriend(pCharacter->GetName());
						else
						{
							GetGameGuiManager()->AddClientSystemMessage("该玩家不在九屏之内");
						}
					}
					return true;
				}
				break;
			}
		}		
		break;	
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}


void CGuiTeam::FrameRefreashTeamerStatus(stTeamMember & member)
{
	FUNCTION_BEGIN;
			 
	CCharacter * pCharacter = GetScene()->FindCharacterByID(member.data.dwTempID);
	if( pCharacter )
	{
		if( pCharacter->IsDie())
			member.status = DIED;
		else
			member.status = NORMAL;
	}
	else 
	{
		if(member.data.dwHealth <= 0 )
			member.status = DIED;
		else if(member.status != OFFLINE)
			member.status = AWAY;
	}                  
	
	 FUNCTION_END;
}

// 响应服务器的队员下线通知
void CGuiTeam::OnTeamerOffLine(stTeamMemberOfflineUserCmd * pCmd)
{
	FUNCTION_BEGIN;
	int id = pCmd->dwTempID;
	TRACE("队员%d离线了\n",id);
	tListTeamMemvber::iterator it = FindMemberIt(id);
	if(it != m_aTeamMember.end())
	{
		(*it).status = OFFLINE;
	}
	FUNCTION_END;
}
/**
 * \brief 简短描述 招收队员
 * 
 * 详细描述 
 * 
 * \param pData : 描述 所招队员的信息
 * \return 返回值的描述
 */
void CGuiTeam::AddTeamMember(stAddTeamMemberUserCmd* pData)
{
	FUNCTION_BEGIN;

	m_dwTeramID = pData->dwTeamID;
	tListTeamMemvber::iterator it = FindMemberIt(pData->data.pstrName);
	if(it != m_aTeamMember.end())
	{      
		(*it).data = pData->data;
		(*it).status = AWAY ; //default status 
	}
	else
	{       // Add a new teamer 
		stTeamMember member;
		member.data = pData->data;

		member.szMapName[0] = 0;
		member.x = -1;
		member.y = -1;
 
		member.status = AWAY; // default status 
		m_aTeamMember.push_back(member);
	}
	if(pData->data.byHead)
		m_dwHeadID = pData->data.dwTempID;

	RefreshButtonState();
	RefreshHP(&pData->data);

	m_iRightX = 0;
	if(m_pTeamIcon && !m_aTeamMember.empty()) 
	{
		IBitmap * pBitmap = m_pTeamIcon->GetBitmap(0);
		RECT rcClip = pBitmap->GetClipRect();
		m_iRightX = rcClip.right;
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述 剔除成员数据
 * 
 * 详细描述
 * 
 * \param pszName : 描述 所剔除的成员临时id,不用名字是为了避免蒙面人状态找不到
 * \return 返回值的描述
 */
void CGuiTeam::RemoveTeamMemberData(DWORD dwTempID)
{
	FUNCTION_BEGIN;

	stTeamMemberData delData;
	delData.dwTempID = dwTempID;
	delData.dwHealth = 0;
	delData.dwMaxHealth = 0;
	RefreshHP(&delData);

	FUNCTION_END;
}

/**
 * \brief 简短描述 剔除成员
 * 
 * 详细描述
 * 
 * \param pszName : 描述 所剔除的成员名字
 * \return 返回值的描述
 */
void CGuiTeam::RemoveTeamMember(DWORD dwTempID)
{
	FUNCTION_BEGIN;

	RemoveTeamMemberData(dwTempID);

	tListTeamMemvber::iterator it = FindMemberIt(dwTempID);
        Assert(it != m_aTeamMember.end());


	if(it == m_aTeamMember.end()) return ;

	// Modified 2005-8-9 17:47,
	// 原来是SimpleRemoveFromArray()之后调用it,已经是后面一个member.name了，哈哈
	stTeamMember & member = *it;
	char szMsg[60];
	sprintf(szMsg,"%s离开队伍了",member.data.pstrName);

	SimpleRemoveFromArray(m_aTeamMember,it - m_aTeamMember.begin());

	RefreshButtonState();

	if(m_aTeamMember.size() <= 1 || GetScene()->GetMainCharacter()->GetID() == dwTempID )
	{
		BreakTeam();
	}

	RefreshButtonState();


	GetGameGuiManager()->AddClientSystemMessage(szMsg);


	FUNCTION_END;
}

/**
 * \brief 简短描述 解散队伍
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiTeam::BreakTeam()
{
	FUNCTION_BEGIN;

	for(size_t i=0;i<m_aTeamMember.size();++i)
	{
		RemoveTeamMemberData(m_aTeamMember[i].data.dwTempID);
	}

	m_dwTeamMode = TEAM_MAX;
	m_aTeamMember.resize(0);
	m_dwTeramID = 0;
	m_dwHeadID = 0;
	RefreshButtonState();

	m_iRightX = 0;

	FUNCTION_END;
} 

/**
 * \brief 简短描述 队伍的系统设置
 * 
 * 详细描述
 * 
 * \param pData : 描述
 * \return 返回值的描述 
 */
void CGuiTeam::SetTeamSettings(stSystemSettings* pData)
{
	FUNCTION_BEGIN;

	GetCheckBox(IDC_ALLOWTEAM)->SetChecked( isset_state(pData->bySettings,USER_SETTING_TEAM) );

	FUNCTION_END;
}

/**
 * \brief 简短描述  获取队伍位置
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiTeam::RequestTeamPosition()
{
	FUNCTION_BEGIN;

	stRequestTeamMemberPosition cmd;
	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

/**
 * \brief 简短描述  设置队伍位置
 * 
 * 详细描述
 * 
 * \param pCmd : 描述
 * \return 返回值的描述 
 */
void CGuiTeam::SetTeamPosition(stReturnTeamMemberPosition* pCmd)
{
	FUNCTION_BEGIN;

	for(size_t i=0;i<pCmd->size;++i)
	{
		tListTeamMemvber::iterator it = FindMemberIt(pCmd->memberPos[i].szName);
		if(it != m_aTeamMember.end())
		{
			strncpy((*it).szMapName,pCmd->memberPos[i].szMapName,MAX_NAMESIZE);
			(*it).szMapName[MAX_NAMESIZE-1] = 0;
			(*it).x = pCmd->memberPos[i].x;
			(*it).y = pCmd->memberPos[i].y;
		}
	}


	FUNCTION_END;
}
/**
 * \brief 简短描述 处理鼠标消息
 * 
 * 详细描述
 * 
 * \param uMsg : 描述
 * \param pt : 描述
 * \param wParam : 描述
 * \param lParam : 描述
 * \return 返回值的描述
 */
bool CGuiTeam::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONDOWN:
		break;
	}
	return false;
	FUNCTION_END;
}
/**
 * \brief 简短描述 请求对方加入
 * 
 * 详细描述
 * 
 * \param pszName : 描述
 * \return 返回值的描述
 */
void CGuiTeam::ToAddTeamMember(const char* pszName,DWORD dwMode)
{
	FUNCTION_BEGIN;

	if( dwMode < TEAM_NORMAL && dwMode > TEAM_HONOR)
	{
		TRACE("无效模式\n");
		return;
	}

	m_dwTeamMode = dwMode;
	if(FindMember(pszName))
	{
		GetGameGuiManager()->AddMessageBox("该玩家已经在队伍中了！");
		return ;
	}

	CCharacter* pChar = GetScene()->FindCharacterByName(pszName);
	if(pChar && pChar == GetScene()->GetMainCharacter())
	{
		GetGameGuiManager()->AddMessageBox("开玩笑吧，不能与自己组队的！");
		return ;
	}
        
#ifdef __REMOTE__CAN__MAKE__TEAM__
	stRequestNameTeamUserCmd cmd;
	strncpy(cmd.byAnswerUserName,pszName,sizeof(cmd.byAnswerUserName));
	cmd.dwTeamMode = dwMode;
	SEND_USER_CMD(cmd);
#else 
	if(!pChar)
	{
		GetGameGuiManager()->AddMessageBox("该玩家不在你的附近！");
		return ;
	}
	stRequestTeamUserCmd cmd;
	cmd.dwAnswerUserID = pChar->GetID();
	cmd.dwTeamMode = dwMode;
	SEND_USER_CMD(cmd);
#endif

	GetGameGuiManager()->AddClientSystemMessage("正在等待对方接受您的邀请！");

	FUNCTION_END;
}

void CGuiTeam::ToAddTeamMember(DWORD dwTempID,DWORD dwMode)
{
	FUNCTION_BEGIN;

	if(FindMember(dwTempID))
	{
		GetGameGuiManager()->AddMessageBox("该玩家已经在队伍中了！");
		return ;
	}

	if( dwMode < TEAM_NORMAL && dwMode > TEAM_HONOR)
	{
		TRACE("无效模式\n");
		return;
	}

	m_dwTeamMode = dwMode;

	CCharacter* pChar = GetScene()->FindCharacterByID(dwTempID);
	if(pChar && pChar == GetScene()->GetMainCharacter())
	{
		GetGameGuiManager()->AddMessageBox("开玩笑吧，不能与自己组队的！");
		return ;
	}
#ifdef __REMOTE__CAN__MAKE__TEAM__
	if(!pChar)
		return;
	stRequestNameTeamUserCmd cmd;
	strncpy(cmd.byAnswerUserName,pChar->GetName(),sizeof(cmd.byAnswerUserName));
	cmd.dwTeamMode = dwMode;
	SEND_USER_CMD(cmd);
#else
	if(!pChar)
	{
		GetGameGuiManager()->AddMessageBox("该玩家不在你的附近！");
		return ;
	}
	stRequestTeamUserCmd cmd;
	cmd.dwAnswerUserID = pChar->GetID();
	cmd.dwTeamMode = dwMode;
	SEND_USER_CMD(cmd);
#endif
	
	GetGameGuiManager()->AddClientSystemMessage("正在等待对方接受您的邀请！");

	FUNCTION_END;
}

//Return team member num among limited distance
//Include self
int CGuiTeam::GetTeamMemberAmongRange( const int& dis )
{
	FUNCTION_BEGIN;
	
	if ( !IsTeam() )
		return 0;
	
	int num = 0;
	stPointI ptCenter = GetScene()->GetMainCharacter()->GetGridPos();
	for(tListTeamMemvber::iterator it = m_aTeamMember.begin() ; it != m_aTeamMember.end(); ++it)
	{
		CCharacter* pChar = GetScene()->FindCharacterByID( (*it).data.dwTempID );
		if ( pChar )
		{
			//Distance
			int distance = Scene_GetDis( ptCenter, pChar->GetGridPos() );
			if ( distance <= dis )
				num++ ;
		}		
	}

	return num;

	FUNCTION_END;
}

void CGuiTeam::UseSkillOnTeamMember( bool bHas /* = true */ )
{
	FUNCTION_BEGIN;

	//Get sel member id
	CCharacter* pCharacter = NULL;
	if ( bHas )
		pCharacter = GetSelMember();
	else
		pCharacter = GetMouseOnMember();

	//use skill for our member
	//if ( GetGameCursor()->IsDrag() && (GetGameCursor()->GetCursorImageType() == eCursorUseAccess) )
	{
		if ( pCharacter )
		{
			//if( GetScene()->GetMainCharacter()->bIsShopStart() )
			//{
			//	GetGameGuiManager()->AddClientSystemMessage("摆摊情况下不可以攻击");
			//	return ;
			//}

			CMainCharacter::stRunEvent runEvent;
			runEvent.id = pCharacter->GetID();
			runEvent.target = CMainCharacter::eTargetType_Character;
			runEvent.pSkill = m_pActiveSkill;
			runEvent.pt = pCharacter->GetPos();
			runEvent.speed = 0;
			runEvent.event = CMainCharacter::eEventType_Attack;

			GetScene()->GetMainCharacter()->SetRunEvent( runEvent );

			RestoreMouseIcon();
		}		
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param hWnd : 描述
 * \param uMsg : 描述
 * \param wParam : 描述
 * \param lParam : 描述
 * \return 返回值的描述
 */
bool CGuiTeam::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
		{
			if ( !GetGameCursor()->IsDrag() && !(GetGameCursor()->GetCursorImageType() == eCursorUseAccess) )
			{
				int id = GetMouseOnMemberNo();
				if ( -1 != id )
				{		
					TRACE("teamer %d was selected:id\n");
					CCharacter * pCharacter = GetScene()->FindCharacterByID(m_aTeamMember[id].data.dwTempID);
					if( pCharacter )
					{
						///////////////////////////////////////////////////////
                                                GetScene()->GetMainCharacter()->HandleLButtonDownEventOnSceneObject(pCharacter);
						///////////////////////////////////////////////////////
						stResourceLocation rl = rl1;
						rl.frame = GetCharTypeInfo(pCharacter->GetType()).equippic+20;
						//rl.frame = m_aTeamMember[id].data.wdFace;
						//GetGameCursor()->SetCursor( CCursor::CursorType_Drag, eCursorFindAccess );
						//Engine_GetCursor()->SetImage( &rl, stPointI(30,30) );
						//m_dwSelMemberID = pCharacter->GetID();
						m_dwSelMemberID = -1;

						///////////////////////////////////////////////////////

					}
					return true;
				}
			}
			else if ( GetGameCursor()->IsDrag() && (GetGameCursor()->GetCursorImageType() == eCursorUseAccess) )
				return true;
		}
		break;
	case WM_LBUTTONUP:
		{
			if ( GetGameCursor()->IsDrag() && (GetGameCursor()->GetCursorImageType() == eCursorUseAccess) )
			{
				UseSkillOnTeamMember( false );
			}
		}
		break;
#ifdef _DEBUG
	case WM_KEYDOWN:
			switch(wParam)
			{
				case '1':
					for(int i = 0 ; i< 6; i++)
					{
						stAddTeamMemberUserCmd cmd;
						if( i == 0)
							cmd.data.byHead = 1;
						else 
							cmd.data.byHead = 0;
						cmd.data.dwTempID = 222+i;
						char szPar[60];
						sprintf(szPar,"test%d",i);
						strcpy(cmd.data.pstrName,szPar);
						cmd.data.wdFace = i+1;
						cmd.data.dwHealth = 100;
						cmd.data.dwMaxHealth = 100;
						cmd.data.dwMaxMp = 100;
						cmd.data.dwMp = 100;
						cmd.dwTeamID = 1;
						AddTeamMember(&cmd);
					}					
					break;
			}
			break;
#endif
	}


	return CGuiDialog::MsgProc( hWnd, uMsg, wParam, lParam );


	FUNCTION_END;
}
/**
 * \brief 简短描述 响应右键菜单
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
bool CGuiTeam::OnContextMenu()
{
	FUNCTION_BEGIN;

	if( m_menu )
	{
		int id  = GetMouseOnMemberNo();
		if( id == -1 ) 
			return false;
		//g_CurMemerID = id;
		if(IsTeamHead( GetScene()->GetMainCharacter()->GetID() ) ) 
		{
			m_menu->SetMenuItem( 0, CMD_DELETEONE, "解除邀请",CGuiMenu::MenuStyle_Command,true);
		}
		else
		{
			m_menu->SetMenuItem( 0, CMD_DELETEONE, "解除邀请",CGuiMenu::MenuStyle_Command,false);
		}
		m_menu->Popup();
		return true;
		
	}
	return false;

	FUNCTION_END;
}
/**
 * \brief 简短描述 离开队伍
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiTeam::LeaveTeam()
{
	FUNCTION_BEGIN;

	if( m_aTeamMember.size() <= 0 ) return;

	const char * pszMessage = "是否真的要离开队伍？";
	GetGameGuiManager()->AddMessageBox(pszMessage,eAction_ExitTeam,(void*)m_dwTeramID,c_szMBOKCancel);

	FUNCTION_END;
}

// clinet send message to server 
// 设置经验分配方式
void CGuiTeam::SetExpAssign(DWORD dwAssign)
{
	assert( dwAssign == TEAM_EXP_MODE_NORMAL || dwAssign == TEAM_EXP_MODE_SPECIAL);
	if( dwAssign != TEAM_EXP_MODE_NORMAL && dwAssign != TEAM_EXP_MODE_SPECIAL) return;

	if( m_aTeamMember.size() <= 0 || !IsTeamHead(GetScene()->GetMainCharacter()->GetID() ) )
		return;

	if(m_dwTeamMode != TEAM_NORMAL) return;

	stExpModeTeamMemberUserCmd cmd;
	cmd.byType =  dwAssign; 
	m_dwCurExpAssign = dwAssign;
	SEND_USER_CMD(cmd);

}

// 设置物品分配方式
void CGuiTeam::SetPickupItemAssing(DWORD dwAssign)
{
	assert( dwAssign == TEAM_OBJ_MODE_NORMAL || dwAssign == TEAM_OBJ_MODE_SPECIAL);
	if( dwAssign != TEAM_OBJ_MODE_NORMAL && dwAssign != TEAM_OBJ_MODE_SPECIAL) return;

	if( m_aTeamMember.size() <= 0 || !IsTeamHead(GetScene()->GetMainCharacter()->GetID() ) )
		return; 

	if(m_dwTeamMode != TEAM_NORMAL) return;

	stObjModeTeamMemberUserCmd cmd;
	cmd.byType =  dwAssign;
	m_dwCurObjAssign = dwAssign;
	SEND_USER_CMD(cmd);
}

void CGuiTeam::RenderTeamTip( )
{
	FUNCTION_BEGIN;


	if(m_aTeamMember.size() <= 0)  return;
	if(m_dwTeamMode != TEAM_NORMAL) return;

	stRectI rcTest = m_rcClipTeamExpAssignByLevel;
	rcTest.OffsetRect(s_ptTeamExpAssign.x ,s_ptTeamExpAssign.y);

	bool bShowTip = rcTest.PtInRect( Engine_GetCursor()->GetPosition() );

	if(  bShowTip)
	{
		CToolTips tips;
		tips.SetBkColor( D3DCOLOR_ARGB(128,0,0,0) );
		tips.SetBorderColor( 0 );
		tips.SetAlignType( GUI_ALIGN_LEFT );
		tips.SetLineGaps( 3 );
		tips.SetCurColor( D3DCOLOR_ARGB(255,152,244,23) );
		char szValue[80];
		if( m_dwCurExpAssign ==  TEAM_EXP_MODE_NORMAL)
			strcpy(szValue,"等级分配:按个人等级占整个团队\n等级的百分比来分配经验");
		else  
			strcpy(szValue,"特殊分配:个人杀怪获得100%经验，\n其他每个队员获得10%经验");
		szValue[79] = 0;
		tips.AddText(szValue); 
		tips.Resize();
		tips.Render( rcTest.right, rcTest.bottom );
	}	

	rcTest = m_rcClipTeamItemAssignByFreedom;
	rcTest.OffsetRect(s_ptTeamObjAssign.x,s_ptTeamObjAssign.y);
	bShowTip = rcTest.PtInRect( Engine_GetCursor()->GetPosition() );

	if(  bShowTip)
	{
		CToolTips tips;
		tips.SetBkColor( D3DCOLOR_ARGB(128,0,0,0) );
		tips.SetBorderColor( 0 );
		tips.SetAlignType( GUI_ALIGN_LEFT );
		tips.SetLineGaps( 3 );
		tips.SetCurColor( D3DCOLOR_ARGB(255,152,244,23) );
		char szValue[80];
		if( m_dwCurObjAssign == TEAM_OBJ_MODE_NORMAL)
			strcpy(szValue,"自由拾取:物品无分配，可自由拾取");
		else  
			strcpy(szValue,"轮流拾取:物品轮流分配，分配的物品可拾取");
		szValue[79] = 0;
		tips.AddText(szValue); 
		tips.Resize();
		tips.Render( rcTest.right, rcTest.bottom );
	}	

	FUNCTION_END;
}

//echo server message 
void	CGuiTeam::SetExpMode(stExpModeTeamMemberUserCmd * pCmd)
{
	assert( pCmd->byType == TEAM_EXP_MODE_NORMAL || pCmd->byType == TEAM_EXP_MODE_SPECIAL);
	if( pCmd->byType != TEAM_EXP_MODE_NORMAL && pCmd->byType != TEAM_EXP_MODE_SPECIAL) return;

	m_dwCurExpAssign = pCmd->byType;

}
void	CGuiTeam::SetItemMode(stObjModeTeamMemberUserCmd * pCmd)
{
	assert( pCmd->byType == TEAM_OBJ_MODE_NORMAL || pCmd->byType == TEAM_OBJ_MODE_SPECIAL);
	if( pCmd->byType != TEAM_OBJ_MODE_NORMAL && pCmd->byType != TEAM_OBJ_MODE_SPECIAL) return;

	m_dwCurObjAssign = pCmd->byType;

}

void CGuiTeam::SetTeamMode(stTeamModeUserCmd * pCmd)
{
	assert( pCmd->byType == TEAM_NORMAL || pCmd->byType == TEAM_HONOR);
	if( pCmd->byType != TEAM_NORMAL && pCmd->byType != TEAM_HONOR) return;
	m_dwTeamMode = pCmd->byType;
}


void CGuiTeam::RefreshTeamMember(const stFreshTeamMemberUserCmd * pCmd)
{
	tListTeamMemvber::iterator Iter_Member = FindMemberIt(pCmd->dwTempID);
	if( Iter_Member != m_aTeamMember.end())
	{
                stTeamMember & Ok_Member = *Iter_Member;
		Ok_Member.data.dwMaxHealth = pCmd->dwMaxHealth;
		Ok_Member.data.dwHealth = pCmd->dwHealth;
		Ok_Member.data.dwMp = pCmd->dwMp;
		Ok_Member.data.dwMaxMp = pCmd->dwMaxMp;

	}
	else 
	{
#if defined _DEBUG || defined RELEASE_TEST
		Engine_WriteLog("收到服务器一个无效TempID的stFreshTeamMemberUserCmd消息");
#endif 
	}
}

bool CGuiTeam::AddMember(const char* name,DWORD type)
{
	CGuiTeam* pDlg = GetGameGuiManager()->AddTeam();
	if(pDlg)
	{
		pDlg->ToAddTeamMember(name,type);
		return true;
	}
	return false;
}

bool CGuiTeam::AddMember(DWORD dwTempID,DWORD type)
{
	CGuiTeam* pDlg = GetGameGuiManager()->AddTeam();
	if(pDlg)
	{
		pDlg->ToAddTeamMember(dwTempID,type);
		return true;
	}
	return false;
}

void CGuiTeam::BackUpAnswerUserName(const char * pCmdUserName)
{
	strncpy(m_szBackupName,pCmdUserName,MAX_NAMESIZE);
	m_szBackupName[MAX_NAMESIZE] = 0;
}
