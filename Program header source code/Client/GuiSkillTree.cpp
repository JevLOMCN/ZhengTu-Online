#include "public.h"
#include "./GameGuiManager.h"
#include "./Game.h"
#include "./GameCursor.h"
#include "./GameScene.h"
#include ".\guiskill.h"
#include "./GuiMain.h"
#include "./GuiTeam.h"
#include "./RoleItem.h"
#include "./RoleSkill.h"
#include "./GameAccelKeyManager.h"
#include "./GuiPetCtrlDialog.h"

#define SKILLICONID_BASE 1000
#define SKILLIMAGEID_BASE 3000
#define SKILLIMAGEID_LEVEL 5000

#define SKILL_AFTER_ADJUST

static const stPointI ptOffset(60,278);
static const stPointI ptIconOffset(3,3);
static const stRectI	rcClip(10,258,410,580);

static int deltaPos = 0;
static int scrollOldPos = 0;

static const char* SkillSerialName[] = { "武术", "射术", "法术", "召唤", "仙术", "刺客", "卫士", "枪系", "阵法","天赋技能" };
static const char* SkillTreeName[] = {"魔刀", "", "", "",
									  "射日",	"", "", "",
									  "奥法",	"", "", "",
									  "阴阳师",	"", "", "",
									  "仙魔",	"", "", "",
									  "刺客",	"", "", "",
									  "卫士",	"", "", "",
									  "枪系",	"", "", "",
									  "阵法", "阵法", "阵法", "阵法", 
									  "天赋技能", "", "", "", };

/*static const int SkillTreeTypeImgID[SKILL_SERIAL_ANGEL][SKILL_TREE_TYPE_NUM] =
{
	{100, 101, 101, 101},
	{100, 102, 103,	102},
	{100, 104, 105, 106},
	{100, 107, 107, 107},
	{100, 108, 109, 109},
	{110, 110, 110, 110}
};*/

#ifndef SKILL_AFTER_ADJUST
//技能框架
static const int SkillTreeTypeImgID[SKILL_SERIAL_TFJN][SKILL_TREE_TYPE_NUM] =
{
	{108, -1, -1, -1},
	{108, -1, -1, -1},
	{108, -1, -1, -1},
	{108, -1, -1, -1},
	{108, -1, -1, -1},
	{108, -1, -1, -1},
	{108, -1, -1, -1},
	{108, -1, -1, -1},
	{900, 900, 900, 900},
	{900, 900, 900, 900},
};
static const int SkillTreeTypeImgHeight[SKILL_SERIAL_TFJN][SKILL_TREE_TYPE_NUM] = 
{
	{1502, -1, -1, -1},
	{1502, -1, -1, -1},
	{1502, -1, -1, -1},
	{1502, -1, -1, -1},
	{1502, -1, -1, -1},
	{1502, -1, -1, -1},
	{1502, -1, -1, -1},
	{1502, -1, -1, -1},
	{479, 479,  479,  479},
	{479, 479,  479,  479},
};

#else

static const int SkillTreeTypeImgID[SKILL_SERIAL_TFJN][SKILL_TREE_TYPE_NUM] =
{
	{100, 105, 105, 105},
	{101, 105, 106,	105},
	{102, 105, 105, 105},
	{103, 107, 107, 107},
	{104, 105, 105, 105},
	{108, -1, -1, -1},
	{108, -1, -1, -1},
	{108, -1, -1, -1},
	{900, 900, 900, 900},
	{900, 900, 900, 900},
};
static const int SkillTreeTypeImgHeight[SKILL_SERIAL_TFJN][SKILL_TREE_TYPE_NUM] = 
{
	{479, 768, 768, 768},
	{479, 768, 768, 768},
	{479, 768, 768, 768},
	{479, 896, 896, 896},
	{408, 768, 768, 768},
	{768, -1, -1, -1},
	{768, -1, -1, -1},
	{768, -1, -1, -1},
	{479, 479,  479,  479},
	{479, 479,  479,  479},
};

#endif

//extern const char* c_szSkillPack;// icon图包
//extern const char* c_szFacePack;// interface图包
//extern const char* c_szSkillIconFileName;	//icon pos ini.txt

const char* c_szSkillPack  = "data\\icons.gl";		   // 图包
const char* c_szFacePack  = "data\\Interfaces.gl";		// 图包
const char* c_szSkillIconFileName = "datas\\skillicon.ini";

CToolTips SkillTips;

const char* GetSkillSerialName( SkillBase_t* pSkill )
{
	if ( (pSkill->dwSerialType>=1) && (pSkill->dwSerialType<=SKILL_SERIAL_QIANG) ) //sky
		return SkillSerialName[pSkill->dwSerialType-1];

	return NULL;
}

const char* GetSkillTreeName( SkillBase_t* pSkill )
{
	if ( (pSkill->dwSerialType>=1) && (pSkill->dwSerialType<=SKILL_SERIAL_QIANG) //SKY
		&& (pSkill->dwTreeType>=1) && (pSkill->dwTreeType<=SKILL_TREE_TYPE_NUM) )
	{
		int index = (pSkill->dwSerialType-1) * SKILL_TREE_TYPE_NUM + pSkill->dwTreeType -1;
		return SkillTreeName[index];
	}

	return NULL;
}

bool IsTeamAttackState(const BYTE& state)
{
	if ( (state == USTATE_TEAM_ATTACK_BIRD_FLAG)
		|| (state == USTATE_TEAM_ATTACK_FLOW_CLOUD)
		|| (state == USTATE_STRENGTH_ATTACK_BLESS)
		|| (state == USTATE_TEAM_ATTACK_ONE_DIM) )
	{
		return true;
	}

	return false;
}

bool IsCharacterUseOnly(SkillBase_t* pSkill)
{
	if ( NULL == pSkill )
		return false;

	if ( pSkill->dwID == SKILL_CRAZY_ACTION )
	{
		return true;
	}

	return false;
}

struct stSkillTreeInfo{
	int xPos;
	int yPos;
};

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param szInfo : 描述
* \return 返回值的描述
*/
stSkillTreeInfo getSkillTreeInfo(const char* szInfo)
{
	FUNCTION_BEGIN;

	stSkillTreeInfo st={0,0};

	char *p,*p1;
	st.xPos = strtol(szInfo,&p,10);
	if(p && *p)
	{
		++p;
		st.yPos = strtol(p,&p1,10);
	}

	return st;

	FUNCTION_END;
}

CGuiSkill::CGuiSkill()
: m_curSerialType(SkillSerial_WuShu)
, m_curTreeType(0)
, m_bInitIcon(false)
, m_pTreeImg(NULL)
, m_activeSkillIcon(NULL)
, m_pScrollBar(NULL)
, m_curTreeUpItemPoint(0)
, m_curTreeTotalPoint(0)
, m_curSerialTotalPoint(0)
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;

	for( int i=0;i<SKILL_TREE_TYPE_NUM;i++ )
		m_pTreeName[i] = NULL;

	InitSerialTotalPoints();

	FUNCTION_END;
}

void CGuiSkill::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_BackElements.resize(2);

	CGuiElement* pEmt = new CGuiElement;
	pEmt->FontColor.Current = COLOR_ARGB(255,246,201,3);
	pEmt->TextureColor.Current = COLOR_ARGB(128,0,0,0);
	pEmt->dwTextFormat = DT_TOP | DT_LEFT;
	pEmt->iFont = 1;
	m_BackElements[0] = pEmt;

	pEmt = new CGuiElement;
	pEmt->FontColor.Current = COLOR_ARGB(255,43,30,2);
	pEmt->TextureColor.Current = COLOR_ARGB(128,0,0,0);
	pEmt->dwTextFormat = DT_TOP | DT_LEFT;
	pEmt->iFont = 1;
	m_BackElements[1] = pEmt;

	OnResetScreenSize();

	//make serial btn group
	GetButton(1)->SetButtonGroup(1);
	GetButton(2)->SetButtonGroup(1);
	GetButton(3)->SetButtonGroup(1);
	GetButton(4)->SetButtonGroup(1);
	GetButton(5)->SetButtonGroup(1);
	GetButton(6)->SetButtonGroup(1);
	GetButton(7)->SetButtonGroup(1);
	GetButton(8)->SetButtonGroup(1);
	GetButton(9)->SetButtonGroup(1);
	GetButton(24)->SetButtonGroup(1);
	GetButton(1)->SetChecked( true );

	GetButton(20)->SetButtonGroup(2);
	GetButton(21)->SetButtonGroup(2);
	GetButton(22)->SetButtonGroup(2);
	GetButton(23)->SetButtonGroup(2);
	GetButton(20)->SetChecked( true );

	m_pEdtPoints = GetStatic( 17 );
	m_pEdtPoints->SetLocation( m_pEdtPoints->GetLocation().x, m_pEdtPoints->GetLocation().y+100 );

	for( int i=0;i<SKILL_TREE_TYPE_NUM;i++ )
	{
		m_pTreeName[i] = GetImage(10+i);
		stRectI rcPos = GetButton(20+i)->GetBoundingBox();
		m_pTreeName[i]->SetLocation( rcPos.left + 5, rcPos.top );
	}

	//get tree img
	m_pTreeImg = GetImage( 18 );
	m_pTreeImg->SetLocation( ptOffset.x, ptOffset.y );

	//Get scroll bar
	m_pScrollBar = (CGuiScrollBar*)GetControl( 19, GUI_CONTROL_SCROLLBAR );
	m_pScrollBar->SetStyle(CGuiScrollBar::NOSKIN);

	if ( m_pScrollBar )
		scrollOldPos = m_pScrollBar->GetValue();

	InitRefresh();

	FUNCTION_END;

}

void CGuiSkill::InitRefresh()
{
	//Show/Hide skill tree type name
	RefreshTreeTypeName();
	
	m_bInitIcon = false;
	InitIcon();

	ResetTree( true );

	RefreshState();	

	UpdateRects();

	//init skill tree map
	InitSkillTreeList();
}

void CGuiSkill::InitSkillTreeList()
{
	m_listSkillTree.clear();

	for( int i=1;i<=SKILL_SERIAL_QIANG;i++ ) //SKY
	{
		for( int j=0;j<SKILL_TREE_TYPE_NUM;j++ )
		{
			stSkillTree skillTree;
			skillTree.dwID = (i-1)* SKILL_TREE_TYPE_NUM + j;
			sprintf( skillTree.strName, SkillTreeName[skillTree.dwID] );
			skillTree.dwPoints = 0;

			m_listSkillTree.push_back( skillTree );
		}
	}
}

void CGuiSkill::OnResetScreenSize()
{
	m_x = -20;
	m_y = -118 + 14;
}

void CGuiSkill::OnClose()
{
	FUNCTION_BEGIN;

	for( int i=0;i<SKILL_TREE_TYPE_NUM;i++ )
	{
		if ( m_pTreeName[i] )
			m_pTreeName[i] = NULL;
	}

	if( m_pEdtPoints )
		m_pEdtPoints = NULL;

	GetGameGuiManager()->m_guiSkill = NULL;

	FUNCTION_END;

}

void CGuiSkill::OnSetCloseLocation()
{
	CGuiDialog::OnSetCloseLocation();


	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x,pt.y+103);
	}
}

void CGuiSkill::UpdateRects()
{
	stRectI rect(326,257,340,581);

	//if(m_pScrollBar->IsVisible())
	//	rect.right -= m_nSBWidth + 10;
	//// Update the render rectangles
	//CuttRect9(rect,stPointI(0,0),stPointI(0,0),m_rcRender);
	//
	m_pScrollBar->SetLocation( rect.left, rect.top );
	m_pScrollBar->SetSize( 14, rect.Height() );
	//
	m_pScrollBar->SetPageSize(100);
	m_pScrollBar->SetTrackRange(0,500);
}

void CGuiSkill::InitCustomElements()
{
	FUNCTION_BEGIN;

	stResourceLocation rl;
	rl.SetFileName(GetGuiGraphicPackName());
	rl.group = 26;
	rl.frame = 3;

	stRectI rcTexture;
	CGuiElement Element;


	////------------------------------------
	//// Element for the 3D Border ScrollBar(滚动条)
	////------------------------------------
	//{
	//	stRectI rects[9];
	//	rl.group = 11;
	//	rl.frame = 11;
	//	stGuiCtrlBorderInfo * pInfo = GetCustomCtrlBorderInfo(GUI_CONTROL_SCROLLBAR);
	//	pInfo->rcTexture.SetRect(0,0,10,153);
	//	pInfo->ptLeftTop = stPointI(3,15);
	//	pInfo->ptRightBottom = stPointI(3,15);
	//	pInfo->ptBorderLeftTop = stPointI(3,-7);
	//	pInfo->ptBorderRightBottom = stPointI(3,-7);
	//	pInfo->CuttRect9(rects);
	//	m_nSBWidth = pInfo->rcTexture.Width();
	//	for(size_t i=0;i<count_of(rects);++i)
	//	{
	//		pInfo->element[i].SetTexture(&rl,&rects[i]);
	//		pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
	//		pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
	//	}
	//}

	//-------------------------------------
	// CGuiScrollBar - Up Arrow
	//-------------------------------------
	rcTexture.SetRect( 0, 212, 14, 230);
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 0, &Element );


	rcTexture.SetRect( 16, 212, 30, 230);
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 1, &Element );


	//-------------------------------------
	// CGuiScrollBar - Down Arrow
	//-------------------------------------
	rcTexture.SetRect( 0, 233,14, 251 );
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 2, &Element );

	rcTexture.SetRect( 16, 233,30, 251 );
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 3, &Element );

	//-------------------------------------
	// CGuiScrollBar - Button
	//-------------------------------------
	rcTexture.SetRect( 34, 209, 56, 231 );
	Element.SetTexture( &rl, &rcTexture );

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 4, &Element );

	FUNCTION_END;
}

void CGuiSkill::ResetIconPos()
{
	CIni ini;
	//ini.Open(NULL,"data\\datas\\skillicon.ini");	//"data\\datas.pak"
	ini.Open("data\\datas.pak",c_szSkillIconFileName);	//"data\\datas.pak"

	for(tTableSkillBase::iterator it = g_tableSkillBase.begin(); it != g_tableSkillBase.end(); ++it)
	{
		if( 1 )
		{
			char szText[20]={0};

			int serial = (*it).dwSerialType;

			int tree = (*it).dwTreeType;
			int id = (*it).dwID;

			//get tree index
			char szTreeMark[5] = {0};
			int treeIndex = ((*it).dwSerialType -1) * SKILL_TREE_TYPE_NUM + (*it).dwTreeType - 1;
			if ( (*it).dwSerialType == SKILL_SERIAL_SPECIAL)
				treeIndex = ((*it).dwSerialType -1) * SKILL_TREE_TYPE_NUM;
			if ( (*it).dwSerialType == 10)
				treeIndex = 36;

			if ( (treeIndex <0) || (treeIndex >100) )
				continue;
			sprintf( szTreeMark, "%d", treeIndex );

			stSkillTreeInfo st = getSkillTreeInfo(ini.GetString(szTreeMark,itoa((int)(*it).dwID,szText,10),"0,0").c_str());
			stPointI pt(st.xPos,st.yPos);

			if ((pt.x==0)&&(pt.y==0))
			{
				continue;
			}

			//We only see those skill whose level < OPEN_SKILL_MAX_LEVEL
			{
				SkillLevelBase_t* pSkillLevel = GetSkillLevelBase( (*it).dwID, 1 );
				if ( NULL == pSkillLevel )
					continue;

				if ( pSkillLevel->dwNeedSkill3Level >= OPEN_SKILL_MAX_LEVEL )
					continue;
			}

			if( m_mapSkillIcon[(*it).dwID].pImage[1] )
			{
				if ( (*it).dwSerialType == SKILL_SERIAL_SPECIAL )
				{
					m_mapSkillIcon[(*it).dwID].pImage[1]->SetLocation( pt.x+ptOffset.x+ptIconOffset.x-3, pt.y+ptOffset.y+ptIconOffset.y-3 );
					//m_mapSkillIcon[(*it).dwID].pImage[1]->SetVisible( true );
				}
				else
				{
					m_mapSkillIcon[(*it).dwID].pImage[1]->SetLocation( pt.x+ptOffset.x+30+ptIconOffset.x, pt.y+ptOffset.y+30+ptIconOffset.y );
					m_mapSkillIcon[(*it).dwID].pImage[1]->SetVisible( true );
				}				
			}

			if( m_mapSkillIcon[(*it).dwID].pImage[0] )
			{
				m_mapSkillIcon[(*it).dwID].pImage[0]->SetLocation( pt.x+ptOffset.x+ptIconOffset.x, pt.y+ptOffset.y+ptIconOffset.y );
				//m_mapSkillIcon[(*it).dwID].pImage[0]->SetVisible( true );
			}

			//get skill btn
			if ( m_mapSkillIcon[(*it).dwID].pButton )
			{
				m_mapSkillIcon[(*it).dwID].pButton->SetLocation( pt.x+ptOffset.x+ptIconOffset.x, pt.y+ptOffset.y+ptIconOffset.y );
				m_mapSkillIcon[(*it).dwID].pButton->SetVisible( true );
			}
		}
	}
}

void CGuiSkill::InitIcon()
{
	FUNCTION_BEGIN;

	if(m_bInitIcon) return;
	m_bInitIcon = true;

	CIni ini;
	//ini.Open(NULL,"data\\datas\\skillicon.ini");	//"data\\datas.pak"
	ini.Open("data\\datas.pak",c_szSkillIconFileName);	//"data\\datas.pak"

	DWORD dwState[] = { GUI_STATE_NORMAL,GUI_STATE_PRESSED,GUI_STATE_DISABLED };
	
	for(tTableSkillBase::iterator it = g_tableSkillBase.begin(); it != g_tableSkillBase.end(); ++it)
	{
		if( 1 )
		{
			char szText[20]={0};

			int serial = (*it).dwSerialType;

			if ( serial == SKILL_SERIAL_SPECIAL )
				int temp = 10;

			int tree = (*it).dwTreeType;
			int id = (*it).dwID;

			if ( id == 346 )
				int temp = 10;
			
			//get tree index
			char szTreeMark[5] = {0};
			int treeIndex = ((*it).dwSerialType -1) * SKILL_TREE_TYPE_NUM + (*it).dwTreeType - 1;
			if ( (*it).dwSerialType == SKILL_SERIAL_SPECIAL)
				treeIndex = ((*it).dwSerialType -1) * SKILL_TREE_TYPE_NUM;

			if ( (treeIndex <0) || (treeIndex >100) )
				continue;
			sprintf( szTreeMark, "%d", treeIndex );

			stSkillTreeInfo st = getSkillTreeInfo(ini.GetString(szTreeMark,itoa((int)(*it).dwID,szText,10),"0,0").c_str());
			stPointI pt(st.xPos,st.yPos);

			if ((pt.x==0)&&(pt.y==0))
			{
				continue;
			}

			//We only see those skill whose level < OPEN_SKILL_MAX_LEVEL
			{
				SkillLevelBase_t* pSkillLevel = GetSkillLevelBase( (*it).dwID, 1 );
				if ( NULL == pSkillLevel )
					continue;

				if ( pSkillLevel->dwNeedSkill3Level >= OPEN_SKILL_MAX_LEVEL )
					continue;
			}
			
			//get skill base
			m_mapSkillIcon[(*it).dwID].pSkillBase = GetSkillBase((*it).dwID);
			m_mapSkillIcon[(*it).dwID].pRoleSkill = NULL;

			//Get RECT img for special skill icon
			if ( (*it).dwSerialType == SKILL_SERIAL_SPECIAL )
			{
				m_mapSkillIcon[(*it).dwID].pImage[1] = GetImage((*it).dwID + SKILLIMAGEID_LEVEL);
				if(m_mapSkillIcon[(*it).dwID].pImage[1] == NULL)
				{
					CGuiImage* pImg;
					AddImage((*it).dwID + SKILLIMAGEID_LEVEL,pt.x-3+ptIconOffset.x,pt.y-3+ptIconOffset.y,stResourceLocation(c_szFacePack,26,4),
						NULL,-1,-1,-1,CGuiImage::FULLSIZE_IMAGE,&pImg);					

					m_mapSkillIcon[(*it).dwID].pImage[1] = pImg;

					//set clip rect
					m_mapSkillIcon[(*it).dwID].pImage[1]->SetClipRect( &rcClip );
				}
			}			
			
			
			//get skill btn
			m_mapSkillIcon[(*it).dwID].pButton = GetButton((*it).dwID + SKILLICONID_BASE);
			if(m_mapSkillIcon[(*it).dwID].pButton == NULL)
			{
				CGuiButton* pBtn;

				AddButton((*it).dwID + SKILLICONID_BASE,"",pt.x+ptOffset.x+ptIconOffset.x,pt.y+ptOffset.y+ptIconOffset.y,32,32,0,false,&pBtn);
				pBtn->SetButtonType(CGuiButton::ButtonType_Bitmap);

				pBtn->SetBitmap(stResourceLocation("data\\icons.gl",1,(*it).dwIcon),3,dwState);
				m_mapSkillIcon[(*it).dwID].pButton = pBtn;
				m_mapSkillIcon[(*it).dwID].pButton->SetUserData((void*)(*it).dwID);

				//set clip rect
				m_mapSkillIcon[(*it).dwID].pButton->SetClipRect( &rcClip );
			}
			m_mapSkillIcon[(*it).dwID].pButton->SetVisible(false);

			//get skill icon active sign
			m_mapSkillIcon[(*it).dwID].pImage[0] = GetImage((*it).dwID + SKILLIMAGEID_BASE);
			if(m_mapSkillIcon[(*it).dwID].pImage[0] == NULL)
			{
				CGuiImage* pImg;
				AddImage((*it).dwID + SKILLICONID_BASE,pt.x-3+ptIconOffset.x,pt.y-3+ptIconOffset.y,stResourceLocation(c_szFacePack,1,242),
					NULL,-1,-1,-1,CGuiImage::FULLSIZE_IMAGE,&pImg);


				m_mapSkillIcon[(*it).dwID].pImage[0] = pImg;

				//Set clip rect
				m_mapSkillIcon[(*it).dwID].pImage[0]->SetClipRect( &rcClip );
			}
			m_mapSkillIcon[(*it).dwID].pImage[0]->SetVisible(false);

			//get skill icon level circle
			if ( (*it).dwSerialType != SKILL_SERIAL_SPECIAL )
			{
				m_mapSkillIcon[(*it).dwID].pImage[1] = GetImage((*it).dwID + SKILLIMAGEID_LEVEL);
				if(m_mapSkillIcon[(*it).dwID].pImage[1] == NULL)
				{
					CGuiImage* pImg;
					//soke 技能加点数量
					AddImage((*it).dwID + SKILLIMAGEID_LEVEL,pt.x+ptOffset.x+30+ptIconOffset.x,pt.y+ptOffset.y+30+ptIconOffset.y,stResourceLocation(c_szFacePack,26,2),
							NULL,-1,-1,-1,CGuiImage::FULLSIZE_IMAGE,&pImg);

					m_mapSkillIcon[(*it).dwID].pImage[1] = pImg;

					//set clip rect
					m_mapSkillIcon[(*it).dwID].pImage[1]->SetClipRect( &rcClip );
				}
			}
			
			m_mapSkillIcon[(*it).dwID].pImage[1]->SetVisible(false);
			
			
		}
	}

	OnGuiEvent(EVENT_BUTTON_CLICKED, 1, NULL);
	OnGuiEvent(EVENT_BUTTON_CLICKED, 20, NULL);
	
	if ( GetScene()->GetMainCharacter() )
		SetPoints(GetScene()->GetMainCharacter()->GetSkillPoint());

	RefreshState();

	FUNCTION_END;

}

void CGuiSkill::DrawSkillDisableStatus()
{
	FUNCTION_BEGIN;

	for(tMapSkillIcon::iterator it = m_mapSkillIcon.begin(); it != m_mapSkillIcon.end(); ++it)
	{
		stSkillIcon& st = it->second;
		if( st.pRoleSkill && st.pButton->IsVisible())
		{
			stRectI rect = st.pButton->GetBoundingBox();
			//if ( rcClip.ContainRect( rect ) )
			if ( (rect.bottom > rcClip.top) && (rect.top < rcClip.bottom) )
			{
				rect &= rcClip;
				rect.OffsetRect(GetClientX(),GetClientY());
				GetScene()->GetMainCharacter()->DrawSkillDisableStatus( st.pRoleSkill->GetSkillBase(), &rect );
			}			
		}
	}

	FUNCTION_END;
}


HRESULT CGuiSkill::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;
	
	if ( m_pScrollBar )
	{
		deltaPos = m_pScrollBar->GetValue() - scrollOldPos;
		OnMouseWheel();
		scrollOldPos = m_pScrollBar->GetValue();
	}

	HRESULT hr =  CGuiDialog::OnRender(fElapsedTime);

	//Draw scroll bar
	if ( m_pScrollBar )
		m_pScrollBar->Render( fElapsedTime );

	DrawSkillDisableStatus();

	DrawSkillString(fElapsedTime);

	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	bool bSetTips = (/*!GetGuiManager()->GetToolTips() &&*/ (GetGuiManager()->GetMouseOverDlg() == this) && ContainsPoint( ptCursor ) );
	ptCursor.x -= GetClientX();
	ptCursor.y -= GetClientY();
	for(tMapSkillIcon::iterator it = m_mapSkillIcon.begin(); it != m_mapSkillIcon.end(); ++it)
	{
		stSkillIcon& st = it->second;
		if(st.pButton->IsVisible())
		{
			stRectI rect = st.pButton->GetBoundingBox();
			rect.OffsetRect(GetClientX(),GetClientY());
			if((st.pButton->ContainsPoint(ptCursor))&&!GetGuiManager()->GetToolTips()			//Mouse on
				&& ( bSetTips /*&& ((int)(GetScene()->GetMainCharacter()->GetSkillDisableStatus( st.pSkillBase )) > 0 )*/ ))
			{
				CToolTips* pTips = GetSkillToolTips(&st);

                //soke 增加技能图标拖动说明   
				if ( st.pRoleSkill != NULL && st.pSkillBase->dwType != SkillType_Passive)
				{
					pTips->SetCurColor(COLOR_ARGB(255,121,100,36) );
				//	m_ToolTips.SetCurColor( COLOR_ARGB(255,121,100,36) );
					pTips->AddText("\n按住鼠标左键可以拖动到下方的技能栏中",0);
				}
				pTips->Resize();
				
				pTips->RenderTopLeftPrefer(rect,GetDevice()->GetWidth(),GetDevice()->GetHeight());
				//GetGuiManager()->SetToolTips(pTips,pt);

			}
			if(st.pRoleSkill)
			{
				//soke 画出树中技能的等级
				if ( st.pSkillBase && (st.pSkillBase->dwSerialType == SKILL_SERIAL_SPECIAL) )
					continue;

				stPointI pt(rect.right -6,rect.bottom-6 );
				stRectI rcNumClip = rcClip;
				rcNumClip.left += m_x;
				rcNumClip.right += m_x;
				rcNumClip.top += m_y;
				rcNumClip.bottom += m_y;
				if ( rcNumClip.PtInRect( pt ) && rcNumClip.PtInRect( stPointI(pt.x+5, pt.y+5) ) )
				{
					char szPoint[8]; 
					sprintf(szPoint,"%u",st.pRoleSkill->GetTotalLevel());
					GetDevice()->DrawString(szPoint,pt);

					/*
					pt = stPointI(rect.right + 4,rect.top + (rect.bottom-rect.top-12)/2);
					GetDevice()->DrawString(st.szKey,pt);
					*/
				}
				
			}
		}
	}

	return hr;	

	FUNCTION_END;
}

bool CGuiSkill::OnGuiEvent( UINT nEvent,UINT nID,CGuiControl* pControl )
{
	FUNCTION_BEGIN;

	switch( nEvent )
	{
	case EVENT_RBUTTON_CLICKED:
		{
			if(GetScene()->GetMainCharacter()->GetSkillPoint())
			{
				tMapSkillIcon::iterator it = m_mapSkillIcon.find(nID - SKILLICONID_BASE);
				if( it != m_mapSkillIcon.end() )
				{
					stSkillIcon& st = it->second;

					//Special skill can only Level-up through skill book
					if ( st.pSkillBase && (st.pSkillBase->dwSerialType == SKILL_SERIAL_SPECIAL) )
						return true;

					if ( NULL == st.pRoleSkill )
					{
						//soke 学习前先判断下用户的职业
						if( GetScene()->GetMainCharacter()->GetProperty()->changejob != st.pSkillBase->dwSerialType && GetScene()->GetMainCharacter()->GetProperty()->changejob != 9 && st.pSkillBase->dwSerialType !=9)
						{
						//	Engine_WriteLogF("用户:[%s]企图学习非所属职业的技能:%s", pMapUserData->name, st.pSkillBase->pstrName);
							if(GetScene()->GetMainCharacter()->GetProperty()->changejob==0)
							{
							   GetGameGuiManager()->AddMessageBox( "你没有选择职业,不能学习职业技能！"); 
							}
							else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==1)
							{
							   GetGameGuiManager()->AddMessageBox( "你的技能职业是：武术,不能学习其它系职业技能！"); 
							}
							else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==2)
							{
							   GetGameGuiManager()->AddMessageBox( "你的技能职业是：射术,不能学习其它系职业技能！"); 
							}
							else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==3)
							{
							    GetGameGuiManager()->AddMessageBox( "你的技能职业是：法术,不能学习其它系职业技能！"); 
							}
							else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==4)
							{
							   GetGameGuiManager()->AddMessageBox( "你的技能职业是：召唤,不能学习其它系职业技能！"); 
							}
							else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==5)
							{
							   GetGameGuiManager()->AddMessageBox( "你的技能职业是：仙术,不能学习其它系职业技能！"); 
							}
							else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==6)
							{
								GetGameGuiManager()->AddMessageBox( "你的技能职业是：刺客,不能学习其它系职业技能！"); 
							}
							else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==7)
							{
								GetGameGuiManager()->AddMessageBox( "你的技能职业是：卫士,不能学习其它系职业技能！"); 
							}
							else if(GetScene()->GetMainCharacter()->GetProperty()->changejob==8)
							{
								GetGameGuiManager()->AddMessageBox( "你的技能职业是：枪系,不能学习其它系职业技能！"); 
							}
							else if(GetScene()->GetMainCharacter()->GetProperty()->changejob>9)
							{
							   GetGameGuiManager()->AddMessageBox( "你没有选择职业,不能学习职业技能！"); 
							}
							return false;
						}

						if ( st.pSkillBase )
						{
							char szMsg[256];
							sprintf( szMsg, "确定要学习%s吗？", st.pSkillBase->pstrName );
							GetGameGuiManager()->AddMessageBox( szMsg, eAction_SkillUp, st.pSkillBase, "确定|1|Enter|取消|2|Esc" );
							return true;
						}						
					}
					else
					{
						if ( 0 )		//st.pRoleSkill->GetLevel() >= MAX_SKILL_LEVEL
							GetGameGuiManager()->AddMessageBox( "此技能已达最高级，无法再进行提升", eAction_MaxSkillLevel );
						else
						{
							if ( GetScene()->GetMainCharacter()->IsCanStudySkill(st.pSkillBase, st.pRoleSkill->GetLevel()+1, true) )
							{
								stAddSkillPointPropertyUserCmd cmd;
								cmd.dwSkillID = nID - SKILLICONID_BASE;//(DWORD)pControl->GetUserData();
								SEND_USER_CMD(cmd);
								return true;
							}							
						}						
					}					
				}
			}			
		}
		break;
	case EVENT_BUTTON_DRAG_MOVE:
		if ( !GetGameCursor()->IsDrag()
			&& !(GetGameCursor()->GetCursorImageType() == eCursorSetAccess)
			&& !(GetGameCursor()->GetCursorImageType() == eCursorUseAccess)
			&& !(GetGameCursor()->GetCursorImageType() == eCursorFindAccess) )
		{
			tMapSkillIcon::iterator it = m_mapSkillIcon.find(nID - SKILLICONID_BASE);
			if ( it != m_mapSkillIcon.end() )
			{
				stSkillIcon* pST = &(it->second);
				if ( pST
					&& ( (pST->pRoleSkill && pST->pRoleSkill->GetLevel() > 0) && (pST->pSkillBase->dwType != 3) ) )
				{
					if ( (abs(pST->pButton->GetDragOffset().x) >= 5)
						|| (abs(pST->pButton->GetDragOffset().y) >= 5) )
					{
						stResourceLocation rl;
						int fNum = 0;
						if ( pST->pButton->GetBitmapInfo( rl, fNum ) )
						{
							//Get active skill icon
							SetActiveSkillIcon( pST, it->first );
							//Change mouse icon
							stPointI ptCursor(0,0);
							ptCursor.x += pST->pButton->GetWidth()/2;
							ptCursor.y += pST->pButton->GetHeight()/2;

							//GetGameCursor()->SetCursorImageType( eCursorSetAccess );
							GetGameCursor()->SetCursor( CCursor::CursorType_Drag, eCursorSetAccess );
							Engine_GetCursor()->SetImage( &rl, ptCursor );
						}
					}
				}
			}
		}		
		break;
	case EVENT_BUTTON_CLICKED:
		{
			//Get active skill icon to mouse
			//This state to set cur skill access key
			tMapSkillIcon::iterator it = m_mapSkillIcon.find(nID - SKILLICONID_BASE);
			if( it != m_mapSkillIcon.end() )
			{
				stSkillIcon* pST = &(it->second);
				if ( (GetGameCursor()->GetCursorImageType() == eCursorSetAccess)
						&& (pST == m_activeSkillIcon) )
				{
					SetActiveSkillIcon();

					//Restore mouse icon
					RestoreMouseIcon();
				}
				else
				{
					//use skill
					if ( pST->pSkillBase && pST->pRoleSkill && (pST->pRoleSkill->GetLevel() > 0) && (pST->pSkillBase->dwType != SkillType_Passive) )
					{
						if ( GetScene()->GetMainCharacter()
							&& (GetScene()->GetMainCharacter()->GetSkillDisableStatus( pST->pSkillBase ) < 1) )
						{
							GetGameGuiManager()->AddClientSystemMessage( "技能处于冷却中!" );
							break;
						}

						if ( GetGameCursor()->GetCursorImageType() == eCursorUseAccess )
						{
							if ( GetGameGuiManager()->m_guiTeam && GetGameGuiManager()->m_guiTeam->GetActiveSkill() )
							{
								//GetGameGuiManager()->m_guiTeam->UseSkillOnTeamMember( false );
								if ( pST->pSkillBase->dwID == GetGameGuiManager()->m_guiTeam->GetActiveSkill()->dwID )
									GetScene()->GetMainCharacter()->UseSkillOnCharacter( GetScene()->GetMainCharacter() );
							}
						}
						else
						{
							if ( IsCanSelectInHandSkill( pST->pSkillBase )
								&& ( !GetScene()->IsAnyAliveInterestNpc() /*|| (pST->pSkillBase->dwUseMethod & SkillUseMethod_BlueHand)*/ )
								/*&& !IsSelectFirstSkill( pST->pSkillBase )*/ )
							{
								//   My pet of type summon
								CPet* pPet = NULL;
								if ( pST->pSkillBase->dwID == SKILL_STRENGTHEN_SUMMON )
									pPet = GetScene()->FindPetByType( PET_TYPE_SUMMON );

								if ( pPet )
								{
									if ( GetGameGuiManager()->m_guiTeam )
										GetGameGuiManager()->m_guiTeam->m_pActiveSkill = pST->pSkillBase;
									GetScene()->GetMainCharacter()->UseSkillOnCharacter( pPet->GetNpc() );
								}
								else
								{
									if ( GetGameGuiManager()->m_guiTeam )
										GetGameGuiManager()->m_guiTeam->m_pActiveSkill = pST->pSkillBase;
									GetGameCursor()->SetCursor( CCursor::CursorType_Drag, eCursorUseAccess );
								}
							}
							else
							{
								GetScene()->GetMainCharacter()->OnSelectSkill( pST->pSkillBase->dwID );
								RestoreMouseIcon();
							}
						}						
					}
					/*if ( ( (pST->pRoleSkill && pST->pRoleSkill->GetLevel() > 0) && (pST->pSkillBase->dwType != SkillType_Passive) ) || g_bSinglePlayer )
					{
						stResourceLocation rl;
						int fNum = 0;
						if ( pST->pButton->GetBitmapInfo( rl, fNum ) )
						{
							//Get active skill icon
							SetActiveSkillIcon( pST, it->first );
							//Change mouse icon
							stPointI ptCursor(0,0);
							ptCursor.x += pST->pButton->GetWidth()/2;
							ptCursor.y += pST->pButton->GetHeight()/2;

							//GetGameCursor()->SetCursorImageType( eCursorSetAccess );
							GetGameCursor()->SetCursor( CCursor::CursorType_Drag, eCursorSetAccess );
							Engine_GetCursor()->SetImage( &rl, ptCursor );
						}
					}*/
				}				
				pST = NULL;
			}

			//Get serial btn event
			if ( ((nID>=1) && (nID<=9) ||  (nID==24) )&& (nID!=m_curSerialType) ) //sky
			{
				GetButton( nID )->SetChecked( true );
				m_curSerialType = nID;			
				if(nID == 24)
				{
					m_curSerialType = 10;	
				}	

				if (nID == 6 ||nID == 7||nID == 8|| nID == 9 || nID == 24 )		//Professional skill //sky 改了
				{
					m_curTreeType = 0;
					GetButton( 20 )->SetChecked( true );
					ShowLeftSideControl( false );
				}
				else
				{
					RefreshTreeTypeName();
					ShowLeftSideControl( true );
				}

				ResetTree( true );
				RefreshState( false );				

				return true;
			}
			//Get tree btn event
			if ( (nID>=20) && (nID<=23) && ((nID-20)!=m_curTreeType) )
			{
				GetButton(nID)->SetChecked( true );
				m_curTreeType = nID-20;

				ResetTree( true );
				RefreshState( false );				

				return true;
			}
		}
		break;
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiSkill::UpdateCurSerialTypeAccordingToWeaponInHand()
{
	if ( GetScene()->GetMainCharacter() == NULL )
		return;

	CMainCharacter* pMainData = GetScene()->GetMainCharacter();

	//Get weapon in hand
	ObjectBase_t* pObjWL = NULL,*pObjWR = NULL;

	pObjWL = GetObjectBase(pMainData->GetSculpt()->dwLeftHandID);
	pObjWR = GetObjectBase(pMainData->GetSculpt()->dwRightHandID);

	enumItemType weaponType = ItemType_None;
	if ( pObjWL )
		weaponType = (enumItemType)pObjWL->dwType;
	if ( pObjWR )
		weaponType = (enumItemType)pObjWR->dwType;
	
	/*ItemType_Arrow =21,
	ItemType_Blade =104,		    //104代表武术刀类武器
	ItemType_Sword =105,	        //105代表武术剑类武器
	ItemType_Axe =106,	           //106代表武术斧类武器
	ItemType_Hammer =107,	        //107代表武术斧类武器
	ItemType_Staff =108,		    //108代表法术杖类武器
	ItemType_Bow =109,	          //109代表箭术弓类武器
	ItemType_Fan =110,	           //110代表美女扇类
	ItemType_Stick =111,	        //111代表召唤棍类武器
	ItemType_Shield =112,	//112代表盾牌类*/
	
	switch( weaponType )
	{
	case ItemType_Blade:
	case ItemType_Sword:
	case ItemType_Axe:
	case ItemType_Hammer:
		m_curSerialType = 1;				
		break;
	case ItemType_Arrow:
	case ItemType_Bow:
		m_curSerialType = 2;
		break;
	case ItemType_Staff:
		m_curSerialType = 3;
		break;
	case ItemType_Stick:
		m_curSerialType = 4;
		break;
	case ItemType_Fan:
		m_curSerialType = 5;
		break;
	case ItemType_Dagger:
		m_curSerialType = 6; //sky 刺客
		break;
	case ItemType_NewSword:
		m_curSerialType = 7; //sky 卫士
		break;
	case ItemType_LongGun:
		m_curSerialType = 8; //sky 枪
		break;
	}

	if ( weaponType == ItemType_None )
		m_curSerialType = 1;
	
    RefreshCurSerialType();	
}

//
bool CGuiSkill::SerialHasAccordingWeapon( int serialType )
{
	if ( GetScene()->GetMainCharacter() == NULL )
		return false;

	CMainCharacter* pMainData = GetScene()->GetMainCharacter();

	//Get weapon in hand
	ObjectBase_t* pObjWL = NULL,*pObjWR = NULL;

	pObjWL = GetObjectBase(pMainData->GetSculpt()->dwLeftHandID);
	pObjWR = GetObjectBase(pMainData->GetSculpt()->dwRightHandID);

	enumItemType weaponType = ItemType_None;
	if ( pObjWL )
		weaponType = (enumItemType)pObjWL->dwType;
	if ( pObjWR )
		weaponType = (enumItemType)pObjWR->dwType;

	/*ItemType_Arrow =21,
	ItemType_Blade =104,		    //104代表武术刀类武器
	ItemType_Sword =105,	        //105代表武术剑类武器
	ItemType_Axe =106,	           //106代表武术斧类武器
	ItemType_Hammer =107,	        //107代表武术斧类武器
	ItemType_Staff =108,		    //108代表法术杖类武器
	ItemType_Bow =109,	          //109代表箭术弓类武器
	ItemType_Fan =110,	           //110代表美女扇类
	ItemType_Stick =111,	        //111代表召唤棍类武器
	ItemType_Shield =112,	//112代表盾牌类*/
	switch( serialType )
	{
	case 1:
		if ( (weaponType == ItemType_Blade) || (weaponType == ItemType_Sword)
			|| (weaponType == ItemType_Axe) || (weaponType == ItemType_Hammer) )
		{
			return true;
		}
		else
			return false;

		break;
	case 2:
		if ( (weaponType == ItemType_Arrow) || (weaponType == ItemType_Bow) )
		{
			return true;
		}
		else
			return false;

		break;
	case 3:
		if ( weaponType == ItemType_Staff )
			return true;
		else
			return false;

		break;
	case 4:
		if ( weaponType == ItemType_Stick )
			return true;
		else
			return false;

		break;
	case 5:
		if ( weaponType == ItemType_Fan )
			return true;
		else
			return false;
	case 6:
		if ( weaponType == ItemType_Dagger )
			return true;
		else
			return false;

		break;
	case 7:
		if ( weaponType == ItemType_NewSword )
			return true;
		else
			return false;

		break;
	case 8:
		if ( weaponType == ItemType_LongGun )
			return true;
		else
			return false;

		break;
	case 9:
		return false;

		break;
	}

	return false;
}

int CGuiSkill::UpdateCurSerialTypeAccordingToSkillPoints()
{
	int i;
	int points = 0;
	m_curSerialType = 1;
	
	//Update
	RefreshSkillTreeList();

	for( i=0;i<SKILL_SERIAL_QIANG;i++ ) //sky
	{
		if ( m_serialTotalPoints[i] > points )
		{
			m_curSerialType = i+1;
			points = m_serialTotalPoints[i];
		}
	}

	//To see if any two serial has same points
	for( i=0;i<SKILL_SERIAL_QIANG;i++ ) //sky
	{
		if ( (m_serialTotalPoints[i] == points) && (i != (m_curSerialType-1)) )
		{
			//See if according weapon in hand
			if ( SerialHasAccordingWeapon(i+1) )
			{
				m_curSerialType = i+1;
			}
		}
	}

	RefreshCurSerialType();

	return m_curSerialType;
}

void CGuiSkill::RefreshCurSerialType()
{
	if ( GetButton( m_curSerialType ) )
		GetButton( m_curSerialType )->SetChecked( true );

	RefreshTreeTypeName();
	ShowLeftSideControl( true );

	ResetTree( true );
	RefreshState( false );	
}

void CGuiSkill::ShowLeftSideControl( bool bShow /* = true */ )
{
	//Show or hide tree name
	for( int i=0;i<SKILL_TREE_TYPE_NUM;i++ )
	{
		if ( m_pTreeName[i] )
			m_pTreeName[i]->SetVisible( bShow );
	}
	//Show or hide tree switch btn
	for( int i=20;i<24;i++ )
	{
		GetButton( i )->SetVisible( bShow );
	}
}

bool CGuiSkill::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	//if(CGuiControl::HandleMouse(uMsg,pt,wParam,lParam))
	//	return true;

	if( !m_bEnabled || !m_bVisible )
		return false;

	//// First acquire focus
	//if( WM_LBUTTONDOWN == uMsg )
	//	if( !m_bHasFocus )
	//		m_pDialog->RequestFocus( this );

	/*if( m_pScrollBar->HandleMouse( uMsg, pt,wParam, lParam ) )
	{
		OnMouseWheel( m_pScrollBar->GetTrackPos() );
	}*/

	return false;
}

bool CGuiSkill::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;

	bool bHandled = false;

	switch(uMsg)
	{
	case WM_KEYDOWN:
		if(wParam >= VK_F1 && wParam <= VK_F12)
		{
			stSkillIcon* pSkillIcon = GetMouseOverIcon();
			if( pSkillIcon )
			{
				//SkillBase_t* pSkill = GetSkillBase(pSkillIcon->pButton->GetID());
				stGameAccelKey key;
				key.type = GameAccelKeyType_Skill;
				key.dwID = pSkillIcon->pButton->GetID() - SKILLICONID_BASE;
				key.szAccelKey[0] = wParam;
				key.szAccelKey[1] = 0;
				GetGameAccelKeyManager()->SetKey(key);
				RefreshState();
				GetGameAccelKeyManager()->Save();
				if(g_bSinglePlayer)
				{
					stAddUserSkillPropertyUserCmd cmd;
					cmd.dwExperience = 0;
					cmd.dwMaxExperience  = 1;
					cmd.dwSkillID = key.dwID;
					cmd.wdLevel = 1;
					GetClient()->PushCommand(&cmd,sizeof(cmd));
				}
				bHandled = true;
			}
		}
		//
		if ( wParam == VK_ESCAPE )
		{
			if ( m_activeSkillIcon || (GetGameCursor()->GetCursorImageType() == eCursorUseAccess) )
			{
				SetActiveSkillIcon();

				//Restore mouse icon
				RestoreMouseIcon();
				bHandled = true;
			}			
		}
		break;
	case WM_MOUSEWHEEL:
		{
			int delta = (short) HIWORD(wParam) / WHEEL_DELTA * 10;
			if ( m_pScrollBar )
				m_pScrollBar->SetValue( m_pScrollBar->GetValue() - delta );
		}
		bHandled = true;
		break;
	}

	if ( bHandled )
		return true;
	else
		return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);

	FUNCTION_END;
	
}

void CGuiSkill::OnMouseWheel()
{
	if ( deltaPos == 0 )
		return;

	//reset tree img pos
	stPointI pt = m_pTreeImg->GetLocation();
	pt.y -= deltaPos;
	m_pTreeImg->SetLocation( pt.x, pt.y );
	m_pTreeImg->SetVisible( true );

	//reset skill icon pos
	//reset the whole tree
	for(tMapSkillIcon::iterator it = m_mapSkillIcon.begin(); it != m_mapSkillIcon.end(); ++it)
	{
		bool bNeedMove = false;
		if ( it->second.pSkillBase )
		{
			bNeedMove = (it->second.pSkillBase->dwSerialType == m_curSerialType)
				&& ((it->second.pSkillBase->dwTreeType-1) == m_curTreeType);

			if ( it->second.pSkillBase->dwSerialType == SKILL_SERIAL_SPECIAL )
				bNeedMove = it->second.pSkillBase->dwSerialType == m_curSerialType;
		}

		if ( it->second.pSkillBase && it->second.pButton )
		{
			if ( bNeedMove )
			{
				pt = it->second.pButton->GetLocation();
				pt.y -= deltaPos;

				it->second.pButton->SetLocation( pt.x, pt.y );
				it->second.pButton->SetVisible( true );
			}		
		}

		if ( it->second.pSkillBase && it->second.pImage[0] )
		{
			if ( bNeedMove )
			{
				pt = it->second.pImage[0]->GetLocation();
				pt.y -= deltaPos;

				it->second.pImage[0]->SetLocation( pt.x, pt.y );
				//it->second.pImage[0]->SetVisible( true );
			}		
		}

		if ( it->second.pSkillBase && it->second.pImage[1] )
		{
			if ( bNeedMove )
			{
				pt = it->second.pImage[1]->GetLocation();
				pt.y -= deltaPos;

				it->second.pImage[1]->SetLocation( pt.x, pt.y );
				it->second.pImage[1]->SetVisible( true );
			}		
		}

		//For special skill
		if ( bNeedMove && (it->second.pSkillBase->dwSerialType == SKILL_SERIAL_SPECIAL) )
		{
			it->second.pButton->SetVisible( it->second.pRoleSkill != NULL );
			it->second.pImage[0]->SetVisible( false );
			it->second.pImage[1]->SetVisible( it->second.pRoleSkill != NULL );
		}		
	}
}

void CGuiSkill::SetPoints( int i )
{
	FUNCTION_BEGIN;
	
	m_pEdtPoints->SetText( avar("%u",i) );

	FUNCTION_END;
}

int CGuiSkill::GetCurTreeIndex()
{
	 return (m_curSerialType-1)*SKILL_TREE_TYPE_NUM + m_curTreeType;
}

int CGuiSkill::GetSkillTreeImgID()
{
	return SkillTreeTypeImgID[m_curSerialType-1][m_curTreeType];
}


void CGuiSkill::RefreshTreeTypeName()
{
	//Show cur serial && tree type name
	for( int i=0;i<SKILL_TREE_TYPE_NUM;i++ )
	{
		stResourceLocation st( "data\\interfaces.gl", 26, SKILL_TREE_NAME_IMG_START );
		//st.frame += (m_curSerialType-1)*(SKILL_TREE_TYPE_NUM+1) + i; //soke 1.04版使用
		st.frame += (m_curSerialType-1)*(SKILL_TREE_TYPE_NUM+1) + i; //soke 官方2016新的机制

		m_pTreeName[i]->SetImage( st );
	}	
}

void CGuiSkill::ResetTree( bool bResetPos )
{
	//Reset skill tree img
	stResourceLocation st( "data\\interfaces.gl", 26, SkillTreeTypeImgID[m_curSerialType-1][m_curTreeType] );
#ifndef SKILL_AFTER_ADJUST
	stRectI rect(0,0,252,1536);
	if ( SkillTreeTypeImgHeight[m_curSerialType-1][m_curTreeType] > 1024 )
		rect.bottom = 1536;
	else
		rect.bottom = 479;
#else
	stRectI rect(0,0,252,800);
	if ( SkillTreeTypeImgHeight[m_curSerialType-1][m_curTreeType] >= 700 )
		rect.bottom = 1536;
	else
		rect.bottom = 479;
#endif
	
	m_pTreeImg->SetImage( st, &rect );	//, &rect 
	m_pTreeImg->SetClipRect( &rcClip );
	m_pTreeImg->SetLocation( ptOffset.x, ptOffset.y );
	m_pTreeImg->SetVisible( true );
	if ( m_pScrollBar && bResetPos )
	{
		m_pScrollBar->SetRange( 0, SkillTreeTypeImgHeight[m_curSerialType-1][m_curTreeType] );	//m_pTreeImg->GetHeight()
		m_pScrollBar->SetValue( 0 );
		scrollOldPos = 0;

		ResetIconPos();
	}
}

void CGuiSkill::InitSerialTotalPoints()
{
	for( int i=0;i<SKILL_SERIAL_QIANG;i++ ) //sky
		m_serialTotalPoints[i] = 0;
}

void CGuiSkill::RefreshSkillTreeList()
{
	FUNCTION_BEGIN;
	
	if ( NULL == GetScene()->GetMainCharacter() )
		return;

	InitSerialTotalPoints();
	
	for( t_SkillTree::iterator itor = m_listSkillTree.begin(); itor != m_listSkillTree.end(); ++itor )
	{
		int serialType = (*itor).dwID / SKILL_TREE_TYPE_NUM;
		int treeType = (*itor).dwID - serialType * SKILL_TREE_TYPE_NUM;
		serialType += 1;		//based on 1

		(*itor).dwPoints = 0;

		for(tMapSkillIcon::iterator it = m_mapSkillIcon.begin(); it != m_mapSkillIcon.end(); ++it)
		{
			stSkillIcon& st = it->second;

			if (st.pRoleSkill==NULL)
				st.pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( it->first );

			{
				SkillBase_t* pSkill = it->second.pSkillBase;
				if ( pSkill )
				{
					if ( (pSkill->dwSerialType == serialType)
						&& ((pSkill->dwTreeType-1) == treeType) )
					{
						if ( st.pRoleSkill )
							(*itor).dwPoints += st.pRoleSkill->GetLevel();
					}

					//Get serial total points
					if ( pSkill->dwSerialType == serialType )
					{
						if ( st.pRoleSkill )
							m_serialTotalPoints[serialType-1] += st.pRoleSkill->GetLevel();
					}
				}					
			}
		}

		//TRACE( "\nSkill tree list---------ID=%u, NAME=%s, POINTS=%u", (*itor).dwID, (*itor).strName, (*itor).dwPoints );
	}
	

	FUNCTION_END;
}

int CGuiSkill::GetSerialSkillPoints( const int& serial, const int& tree, bool bIncUpItem /* = false */ )
{
	int serialPoints = 0;
	for( int k=1;k<=SKILL_TREE_TYPE_NUM;k++ )
	{
		if ( (k == 1) || (k == tree) )
			serialPoints += GetTreeSkillPoints( serial, k, bIncUpItem );
	}

	return serialPoints;
}

bool CGuiSkill::IsHasStudySpecialSkill( const enmSkillSpecialType& type )
{
	for(tMapSkillIcon::iterator it = m_mapSkillIcon.begin(); it != m_mapSkillIcon.end(); ++it)
	{
		stSkillIcon& st = it->second;

		if ( st.pRoleSkill )
		{
			SkillBase* pSkill = st.pSkillBase;
			if ( pSkill )
			{
				if ( (pSkill->dwSerialType == SKILL_SERIAL_SPECIAL)
					&& (pSkill->dwTreeType == type) )
				{
					return true;
				}
			}
		}		
	}

	return false;
}

bool CGuiSkill::IsCanStudySpecialSkill( const enmSkillSpecialType& type )
{
	switch( type )
	{
	case SKILL_SPECIAL_FAST:
		{
			if ( IsHasStudySpecialSkill( SKILL_SPECIAL_FLOW ) )
			{
				GetGameGuiManager()->AddClientSystemMessage( "你已经学习了诡流系阵法，不可以再学习奇兵系阵法!" );
				return false;
			}

			return true;
		}
		break;
	case SKILL_SPECIAL_FLOW:
		{
			if ( IsHasStudySpecialSkill( SKILL_SPECIAL_FAST ) )
			{
				GetGameGuiManager()->AddClientSystemMessage( "你已经学习了奇兵系阵法，不可以再学习诡流系阵法!" );
				return false;
			}

			return true;
		}
		break;
	case SKILL_SPECIAL_BOOK:
		return true;
		break;
	}

	return true;
}

int CGuiSkill::GetTreeSkillPoints( const int& serial, const int& tree, bool bIncUpItem /*= false*/ )
{
	FUNCTION_BEGIN;

	int totalPoints = 0;
	for(tMapSkillIcon::iterator it = m_mapSkillIcon.begin(); it != m_mapSkillIcon.end(); ++it)
	{
		stSkillIcon& st = it->second;

		if (st.pRoleSkill==NULL)
		{
			st.pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( it->first );

			if (st.pRoleSkill)
				st.pSkillBase = st.pRoleSkill->GetSkillBase();
		}

		{
			SkillBase_t* pSkill = it->second.pSkillBase;
			if ( pSkill )
			{
				if ( (it->second.pSkillBase->dwSerialType == serial)
					&& (it->second.pSkillBase->dwTreeType == tree) )
				{
					if ( st.pRoleSkill )
					{
						if ( bIncUpItem )
							totalPoints += st.pRoleSkill->GetLevel() + st.pRoleSkill->GetUpNum();
						else
							totalPoints += st.pRoleSkill->GetLevel();
					}						
				}	
			}					
		}
	}

	return totalPoints;

	FUNCTION_END;
}

void CGuiSkill::RefreshCurTreeSkillPoints()
{
	FUNCTION_BEGIN;

	if ( NULL == GetScene()->GetMainCharacter() )
		return;
	
	m_curTreeTotalPoint = GetTreeSkillPoints( m_curSerialType, m_curTreeType+1 );
	m_curTreeUpItemPoint = GetTreeSkillPoints( m_curSerialType, m_curTreeType+1, true ) - m_curTreeTotalPoint;
	m_curSerialTotalPoint = GetSerialSkillPoints( m_curSerialType, m_curTreeType+1 );

	GetScene()->GetMainCharacter()->SetCurSkillTreePoints( m_curTreeTotalPoint );

	FUNCTION_END;
}

void CGuiSkill::RefreshState(  bool bResetPos )
{
	FUNCTION_BEGIN;

	if ( NULL == GetScene()->GetMainCharacter() )
		return;
	
	/*TRACE( "!!!!!!!!!!!!!!!!!!!!!!!!!Before refresh state!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
	for(tTableSkillBase::iterator it = g_tableSkillBase.begin(); it != g_tableSkillBase.end(); ++it)
	{ 
		TRACE("skill:%u:%s,charTyp %u, five: %u, level: %u,\n",(*it).dwID,(*it).pstrName,(*it).dwSerialType,(*it).dwTreeType, (*it).dwLevel);
	}*/
	RefreshCurTreeSkillPoints();
	
	//reset the whole tree
	for(tMapSkillIcon::iterator it = m_mapSkillIcon.begin(); it != m_mapSkillIcon.end(); ++it)
	{
		stSkillIcon& st = it->second;

		if (st.pRoleSkill==NULL)
		{
			st.pRoleSkill = GetScene()->GetMainCharacter()->FindSkillByID( it->first );

			if (st.pRoleSkill)
				st.pSkillBase = st.pRoleSkill->GetSkillBase();
		}
		else
		{
			if ( NULL == GetScene()->GetMainCharacter()->FindSkillByID( it->first ) )
				st.pRoleSkill = NULL;
		}

		/*if ( st.pSkillBase->dwID == 151 )
			int tmp = 10;*/
		
		int upLevel = (st.pRoleSkill == NULL) ? 0 : st.pRoleSkill->GetLevel();
		st.pButton->SetEnabled((st.pRoleSkill!=NULL) || GetScene()->GetMainCharacter()->IsCanStudySkill(st.pSkillBase, ++upLevel) );
		//st.pButton->SetEnabled( true );
		
		//Only show role skill for special skill
		if ( st.pSkillBase && (st.pSkillBase->dwSerialType == SKILL_SERIAL_SPECIAL) )
		{
			st.pButton->SetVisible( NULL != st.pRoleSkill );
		}

		CAccelKey * pKey = GetGameAccelKeyManager()->GetKey(GameAccelKeyType_Skill,it->first);

		if(pKey)
		{
			strcpy(st.szKey,pKey->GetAccelKey().c_str());
		}
		else st.szKey[0] = 0;

		//refresh skill
		{
			DWORD dwSkillID = it->first;			

			SkillBase_t* pSkill = it->second.pSkillBase;
			if ( pSkill )
			{
				//TRACE("Skill:%s\n",pSkill->pstrName);
				int serial = it->second.pSkillBase->dwSerialType;
				int tree = (it->second.pSkillBase->dwTreeType-1);

				bool bShow = (it->second.pSkillBase->dwSerialType == m_curSerialType)
					&& ((it->second.pSkillBase->dwTreeType-1) == m_curTreeType);
				{
					if ( bShow )
						int temp = 10;
					it->second.pButton->SetVisible( bShow);
					it->second.pImage[1]->SetVisible( bShow );
					if ( (pSkill == GetScene()->GetMainCharacter()->m_pActiveSkill) && bShow )
						it->second.pImage[0]->SetVisible( true );
					else
						it->second.pImage[0]->SetVisible( false );
				}

				if ( it->second.pSkillBase->dwSerialType == SKILL_SERIAL_SPECIAL )
				{
					bShow = it->second.pSkillBase->dwSerialType == m_curSerialType;
					if ( bShow )
					{
						it->second.pButton->SetVisible( it->second.pRoleSkill != NULL );
						it->second.pImage[0]->SetVisible( false );
						it->second.pImage[1]->SetVisible( it->second.pRoleSkill != NULL );
					}
				}				
			}					
		}
	}	
	
	/*TRACE( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!After refresh state!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
	for(tTableSkillBase::iterator it = g_tableSkillBase.begin(); it != g_tableSkillBase.end(); ++it)
	{ 
		TRACE("skill:%u:%s,charTyp %u, five: %u, level: %u,\n",(*it).dwID,(*it).pstrName,(*it).dwSerialType,(*it).dwTreeType, (*it).dwLevel);
	}*/

	FUNCTION_END;
}

CToolTips* CGuiSkill::GetSkillToolTips( stSkillIcon* pSkillIcon )
{
	FUNCTION_BEGIN;

	//static CToolTips tips;
	SkillTips.Clear();

	//UpdateSkillToolTips(pSkillIcon->pSkillBase,SkillTips,pSkillIcon->pRoleSkill);
	if ( pSkillIcon->pRoleSkill )
		UpdateSkillToolTips(pSkillIcon->pRoleSkill->GetSkillBase(),SkillTips,pSkillIcon->pRoleSkill);
	else
		UpdateSkillToolTips(pSkillIcon->pSkillBase,SkillTips,pSkillIcon->pRoleSkill);

	return &SkillTips;

	FUNCTION_END;
}

void CGuiSkill::GetNeedPointsDesc( const int& serial, const int& tree, char* strDesc )
{
	if ( NULL == strDesc )
		return;
	
	//Valid serial & tree No.
	if ( (serial < 1) || (serial > SKILL_SERIAL_QIANG) ) //sky
		return;
	if ( (tree < 1) || (tree > SKILL_TREE_TYPE_NUM) )
		return;

	strDesc[0] = 0L;
	int baseIndex = (serial-1)*SKILL_TREE_TYPE_NUM;
	int treeIndex = (serial-1)*SKILL_TREE_TYPE_NUM + (tree-1);
	if ( 1 == tree )
		sprintf( strDesc, "%s", SkillTreeName[baseIndex] );
	else
		sprintf( strDesc, "%s + %s", SkillTreeName[baseIndex], SkillTreeName[treeIndex] );
}

void CGuiSkill::DrawSkillString( float fElapsedTime )
{
	FUNCTION_BEGIN;

	if ( m_curSerialType == SKILL_SERIAL_SPECIAL )
		return;

	//First draw tree total points
	stRectI rcTreePoints;
	
	rcTreePoints.top = ptOffset.y - 18;
	rcTreePoints.bottom = rcTreePoints.top + 25;

	rcTreePoints.left = ptOffset.x + 3;
	rcTreePoints.right = rcTreePoints.left + 120;
	DrawText( "前提点数之和：", m_BackElements[0], &rcTreePoints, FontEffect_Shadow);

	rcTreePoints.left = ptOffset.x + 130;
	rcTreePoints.right = rcTreePoints.left + 100;

	char szText[10]={0};
	itoa(m_curSerialTotalPoint, szText, 10);
	DrawText( szText, m_BackElements[0], &rcTreePoints, FontEffect_Shadow);

	//for(tMapSkillIcon::iterator it = m_mapSkillIcon.begin(); it != m_mapSkillIcon.end(); ++it)
	//{
	//	if (it->second.pButton->IsVisible())
	//	{
	//		stSkillIcon& st = it->second;
	//		stRectI      rectName(st.pButton->GetLocation().x + 50,st.pButton->GetLocation().y+7,
	//			st.pButton->GetLocation().x + 150,st.pButton->GetLocation().y+21);
	//		stRectI      rectAttack(st.pButton->GetLocation().x + 56,st.pButton->GetLocation().y+23,
	//			st.pButton->GetLocation().x + 156,st.pButton->GetLocation().y+48);
	//		st.pButton->m_pDialog->DrawText(st.pSkillBase->pstrName,m_BackElements[0],&rectName,FontEffect_Shadow);
	//		//st.pButton->m_pDialog->DrawText(st.pSkillBase->dwType==3?"被动":"主动",m_BackElements[1],&rectAttack);

	//		if (st.pRoleSkill)
	//		{
	//			stRectI  rect = st.pButton->GetBoundingBox();
	//			rect.OffsetRect(m_x,m_y);

	//			//GetScene()->GetMainCharacter()->DrawSkillDisableStatus( st.pSkillBase, &rect);
	//		}
	//	}
	//}

	FUNCTION_END;
}

CGuiSkill::stSkillIcon* CGuiSkill::GetMouseOverIcon()
{
	FUNCTION_BEGIN;

	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	ptCursor.x -= GetClientX();
	ptCursor.y -= GetClientY();
	for(tMapSkillIcon::iterator it = m_mapSkillIcon.begin(); it != m_mapSkillIcon.end(); ++it)
	{
		stSkillIcon& st = it->second;
		if(st.pButton->IsVisible())
		{
			stRectI rect = st.pButton->GetBoundingBox();
			rect.OffsetRect(GetClientX(),GetClientY());
			if(st.pButton->ContainsPoint(ptCursor))
			{
				return &st;
			}
		}
	}
	return NULL;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param id : 描述
* \return 返回值的描述
*/
CGuiSkill::stSkillIcon* CGuiSkill::GetSkillIconByID( const DWORD& id )
{
	FUNCTION_BEGIN;

	if ( m_mapSkillIcon.empty() )
		return NULL;

	for(tMapSkillIcon::iterator it = m_mapSkillIcon.begin(); it != m_mapSkillIcon.end(); ++it)
	{
		if ( id == it->first )
			return &( it->second );
	}

	return NULL;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
const char* CGuiSkill::GetActiveSkillIconTooltips()
{
	FUNCTION_BEGIN;

	if ( g_bSinglePlayer )
		return GetSkillIconTooltipsFromID( m_activeSkillID );
	else
	{
		//TBD
		return GetSkillIconTooltipsFromID( m_activeSkillID );
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
CToolTips* CGuiSkill::GetActiveSkillIconTooltipsFull()
{
	FUNCTION_BEGIN;

	if ( m_activeSkillIcon )
		return GetSkillToolTips(m_activeSkillIcon);//&(m_activeSkillIcon->tips);

	return NULL;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param id : 描述
* \param rl : 描述
* \return 返回值的描述
*/
bool CGuiSkill::GetSkillIconResourceFromID( const DWORD& id, stResourceLocation& rl )
{
	FUNCTION_BEGIN;

	if ( m_mapSkillIcon.empty() )
		return false;
	for(tMapSkillIcon::iterator it = m_mapSkillIcon.begin(); it != m_mapSkillIcon.end(); ++it)
	{
		if ( id == it->first )
		{
			int fNum = 0;
			if ( it->second.pButton->GetBitmapInfo( rl, fNum ) )
				return true;
		}
	}
	return false;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param id : 描述
* \return 返回值的描述
*/
const char* CGuiSkill::GetSkillIconTooltipsFromID( const DWORD& id )
{
	FUNCTION_BEGIN;

	if ( g_bSinglePlayer )
	{
		SkillBase_t* pSkill =  GetSkillBase( id );
		if ( pSkill )
			return pSkill->pstrName;

		return NULL;
	}
	else
	{
		//TBD
		SkillBase_t* pSkill =  GetSkillBase( id );
		if ( pSkill )
			return pSkill->pstrName;

		return NULL;
	}

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param id : 描述
* \return 返回值的描述
*/
CToolTips* CGuiSkill::GetSkillIconTooltipsFullFromID( const DWORD& id )
{
	FUNCTION_BEGIN;

	tMapSkillIcon::iterator it = m_mapSkillIcon.find( id );
	if( it != m_mapSkillIcon.end() )
		return GetSkillToolTips(&(it->second));//&(it->second.tips);		

	return NULL;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
DWORD CGuiSkill::GetActiveSkillIconID()
{
	FUNCTION_BEGIN;

	if ( m_activeSkillIcon )
		return m_activeSkillIcon->pButton->GetID() - SKILLICONID_BASE;
	else
		return -1;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param rl : 描述
* \return 返回值的描述
*/
//-------------------------------------------------------------------------------
bool CGuiSkill::GetActiveSkillIconResource( stResourceLocation& rl )
{
	FUNCTION_BEGIN;

	if ( m_activeSkillIcon )
	{
		int fNum = 0;
		if ( m_activeSkillIcon->pButton->GetBitmapInfo( rl, fNum ) )
			return true;
	}
	return false;	

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param rl : 描述
* \return 返回值的描述
*/
void CGuiSkill::SetActiveSkillIcon( stSkillIcon* pSkillIcon /* = NULL */, const DWORD& dwID /* = -1 */ )
{
	 m_activeSkillIcon = pSkillIcon;
	 m_activeSkillID = dwID;
}
