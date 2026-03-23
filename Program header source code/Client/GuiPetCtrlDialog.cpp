#include "public.h"
#include "../gui/include/guiTypes.h"
#include ".\gameguimanager.h"
#include "Game.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GameState.h"
#include ".\GuiPetCtrlDialog.h"
#include "./GuiChatSelect.h"
#include "./GuiSkill.h"
#include "./GameCursor.h"
#include "./GuiMain.h"
#include "./GameAccelKeyManager.h"

#define	IDC_PET_NAME_LABEL	2
#define IDC_FOLLOW_BTN		11
#define IDC_STAND_BTN		12
#define IDC_SELATTACK_BTN	13
#define IDC_SKILL2_BTN		14
#define IDC_SKILL3_BTN		15
#define IDC_SKILL4_BTN		16
#define IDC_ATTACK_BTN		17
#define IDC_PASSIVE_BTN		18
#define IDC_NOATK_BTN		19


int CGuiPetCtrlDialog::m_siPetCtrlNum = 0;

#define PetCtrl_Group_1024_768	25
#define PetCtrl_Group_800_600	125

#define PetCtrl_SetButtonBitmap(id,group,frame) {CGuiButton* p = GetButton(id);if(p) {p->SetBitmap(stResourceLocation(GetGuiGraphicPackName(),group,frame),3);}}
#define PetCtrl_AddGroupButton(id,group)		{CGuiButton *p = GetButton(id);if(p) p->SetButtonGroup(group);}

CGuiPetCtrlDialog::CGuiPetCtrlDialog()
{
	FUNCTION_BEGIN;

	m_pBmpHp = NULL;
	m_pPet = NULL;

	m_siPetCtrlNum ++;

	leftOffset = 0.0f;
	
	FUNCTION_END;
}

CGuiPetCtrlDialog::CGuiPetCtrlDialog(petType type)
{
	FUNCTION_BEGIN;

	m_pBmpHp = NULL;
	m_pPet = GetScene()->FindPetByType( type );
	m_siPetCtrlNum ++;
	leftOffset = 0.0f;

	FUNCTION_END;
}

CGuiPetCtrlDialog::~CGuiPetCtrlDialog(void)
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}


/**
* \brief 关闭对话框
* 
* 
* 
* \return 
*/
void CGuiPetCtrlDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	m_siPetCtrlNum -- ;

	if( m_pPet )
	{
		GetGameGuiManager()->m_guiPetFloatDlg = NULL;
		//if( m_pPet->GetPetType() == PET_TYPE_PET)
		//{
		//	GetGameGuiManager()->m_guiPetFloatDlg = NULL;
		//}
		//if( m_pPet->GetPetType() == PET_TYPE_SUMMON)
		//{
		//	GetGameGuiManager()->m_guiSummonFloatDlg = NULL;
		//}

		//if ( GetGameGuiManager()->m_guiPetFloatDlg ) 
		//{
		//	GetGameGuiManager()->m_guiPetFloatDlg->UpdateWindowPos();
		//}
		//if ( GetGameGuiManager()->m_guiSummonFloatDlg ) 
		//{
		//	GetGameGuiManager()->m_guiSummonFloatDlg->UpdateWindowPos();
		//}
	}

	FUNCTION_END;
}

/**
* \brief 创建对话框
* 
* 
* 
* \return 
*/
void CGuiPetCtrlDialog::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();



	m_pSkillBtn[0] = GetButton(14);
	m_pSkillBtn[1] = GetButton(15);
	m_pSkillBtn[2] = GetButton(16);
	m_pSkillBtn[3] = NULL;

	int y = GetDevice()->GetHeight();
	m_y = y - 120;

	PetCtrl_AddGroupButton(IDC_FOLLOW_BTN,1);
	PetCtrl_AddGroupButton(IDC_STAND_BTN,1);
	PetCtrl_AddGroupButton(IDC_ATTACK_BTN,2);
	PetCtrl_AddGroupButton(IDC_PASSIVE_BTN,2);
	PetCtrl_AddGroupButton(IDC_NOATK_BTN,2);

	OnResetScreenSize();


	FUNCTION_END;
}



/**
* \brief 
* 
* 
* 
* \param type : 
* \return 
*/
void CGuiPetCtrlDialog::UpdatePetInfo( petType type )
{
	FUNCTION_BEGIN;

	m_pPet = GetScene()->FindPetByType( type );
	if( m_pPet )
	{
		const t_PetData* pData = m_pPet->GetProperty();		
		if(pData) 
		{
			//Debug_Int(pData->ai);
#define PetCtrl_SetButtonCheck(id) {GetButton(id)->SetChecked(true);}
#define PetCtrl_SetAiButton(mode,id) {bool b= IsSetAi(pData->ai,mode);if(b) PetCtrl_SetButtonCheck(id)}
	
			{
				CGuiStatic* p = GetStatic(IDC_PET_NAME_LABEL);
				if(p)
				{
					CGuiElement Element;
					DWORD color = m_pPet->GetNameTextColor();
					DWORD dwTextFormat = DT_CENTER | DT_VCENTER ;
					Element.SetFont(1,color,dwTextFormat);
					Element.FontColor.States[ GUI_STATE_NORMAL ] = color;
					Debug_Color(color);
					p->SetElement(0,&Element);
					p->SetText(pData->name);
				}
			}
			PetCtrl_SetAiButton(PETAI_MOVE_FOLLOW,IDC_FOLLOW_BTN);
			PetCtrl_SetAiButton(PETAI_MOVE_STAND,IDC_STAND_BTN);
			PetCtrl_SetAiButton(PETAI_ATK_ACTIVE,IDC_ATTACK_BTN);
			PetCtrl_SetAiButton(PETAI_ATK_PASSIVE,IDC_PASSIVE_BTN);
			PetCtrl_SetAiButton(PETAI_ATK_NOATK,IDC_NOATK_BTN);
			//显示技能图标
			stResourceLocation rl;
			for( int i = 0 ;i < 3; ++i)
			{
				DWORD dwSkill = pData->skills[i];

				if ( NULL == GetGameGuiManager()->m_guiSkill )
				{
					GetGameGuiManager()->AddSkill();
					if ( GetGameGuiManager()->m_guiSkill )
					{
						GetGameGuiManager()->m_guiSkill->SetVisible( false );
					}
				}
				CGuiSkill* pskill = GetGameGuiManager()->m_guiSkill;

				if ( pskill->GetSkillIconResourceFromID( dwSkill, rl ) )
				{
					TRACE("dwSkill %d %s,%d,%d [i=%d] \n",dwSkill,rl.szFileName,rl.group,rl.frame,i);
					m_pSkillBtn[i]->SetToolTips( pskill->GetSkillIconTooltipsFromID(dwSkill) );
					m_pSkillBtn[i]->SetBitmapScale( skillIconScale );
					m_pSkillBtn[i]->SetBitmap( rl, 3 );
					m_pSkillBtn[i]->SetEnabled(true);
				}
				else
				{
					TRACE("dwSkill %d [i=%d] \n",dwSkill,i);
					m_pSkillBtn[i]->SetToolTips("");
					m_pSkillBtn[i]->SetBitmap( rl, 0 );
					m_pSkillBtn[i]->SetEnabled(false);
				}
			}
		}
	}
	else
	{
		Close();
	}
	
	FUNCTION_END;
}


/**
* \brief 对话框控件事件处理
* 
* 
* 
* \param nEvent : 
* \param nID : 
* \param pControl : 
* \return 
*/
bool CGuiPetCtrlDialog::OnGuiEvent( UINT nEvent,UINT nID,CGuiControl* pControl )
{
	FUNCTION_BEGIN;

	switch( nEvent )
	{
	case EVENT_RBUTTON_CLICKED:
		if(OnRBtnClicked(nID)) return true;
		break;
	case EVENT_BUTTON_CLICKED:
		if(OnBtnClicked(nID)) return true;
		break;
	default:
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiPetCtrlDialog::OnBeginSelectAttack()
{
	FUNCTION_BEGIN;

	if( GetGameCursor()->GetCursorImageType() == eCursorPetAttack )
	//宠物攻击
	{
		OnEndSelectAttack();
	}
	else
	{
		stResourceLocation rl("data\\cursor.gl",0,7);
		//统一称为宠物攻击
		GetGameCursor()->SetCursor( CCursor::CursorType_Select, eCursorPetAttack );
		GetGameCursor()->SetImage( &rl, stPointI(1,1) );
		GetButton(IDC_SELATTACK_BTN)->SetChecked(true);
	}

	FUNCTION_END;
}

void CGuiPetCtrlDialog::OnEndSelectAttack()
{
	GetGameCursor()->SetCursorTypeEx(eCursorNormal) ;
	//光标换成普通光标
	GetGameCursor()->SetCursor(CCursor::CursorType_Normal,eCursorNormal);
	GetButton(IDC_SELATTACK_BTN)->SetChecked(false);
}

void CGuiPetCtrlDialog::OnPetAttackNpc(CNpc* pNpc)
{
	FUNCTION_BEGIN;
	bool bRes = false;
	if(pNpc && m_pPet)
	{
		if( !pNpc->IsMyPet() && !pNpc->IsMainRole() )
		{
			stSetTargetPetCmd cmd;
			cmd.id = pNpc->GetID();
			cmd.type = m_pPet->GetPetType();
			cmd.targetType = 1;		//代表npc
			if(pNpc->IsCharacter()) 
			{
				cmd.targetType = 0;	//代表玩家
			}
			SEND_USER_CMD(cmd);
			bRes = true;
		}
	}
	OnEndSelectAttack();

	if(!bRes)
	{
		GetGameGuiManager()->AddClientSystemMessage("无效攻击目标");
	}

	FUNCTION_END;
}

bool CGuiPetCtrlDialog::OnRBtnClicked(int id)
{
	FUNCTION_BEGIN;
	if( !m_pPet ) return false;

	const t_PetData* pData = m_pPet->GetProperty();
	if( !pData ) return false;
	switch(id)
	{
	case IDC_SKILL2_BTN:
		m_pPet->DelSkill(1);
		return true;
		break;
	case IDC_SKILL3_BTN:
		m_pPet->DelSkill(2);
		return true;
		break;
	case IDC_SKILL4_BTN:
		m_pPet->DelSkill(3);
		return true;
		break;
	}
	return false;

	FUNCTION_END;
}

bool CGuiPetCtrlDialog::OnBtnClicked(int id)
{
	FUNCTION_BEGIN;
	if( !m_pPet ) return true;

	if ( isset_state(GetScene()->GetMainCharacter()->GetState(),USTATE_CRAZY_ACTION)
		|| isset_state(GetScene()->GetMainCharacter()->GetState(),USTATE_CRAZY) )
	{
		return true;
	}

	//if( GetScene()->GetMainCharacter()->GetState() )

	const t_PetData* pData = m_pPet->GetProperty();
	if( !pData ) return true;
	switch(id)
	{
	case IDC_FOLLOW_BTN:	
		GetButton(IDC_FOLLOW_BTN)->SetChecked(true);
		m_pPet->SetAI( PETAI_MOVE_FOLLOW );
		return true;
		break;
	case IDC_STAND_BTN:
		GetButton(IDC_STAND_BTN)->SetChecked(true);
		m_pPet->SetAI( PETAI_MOVE_STAND );
		return true;
		break;
	case IDC_ATTACK_BTN:
		GetButton(IDC_ATTACK_BTN)->SetChecked(true);
		m_pPet->SetAI( PETAI_ATK_ACTIVE );
		return true;
		break;
	case IDC_PASSIVE_BTN:
		GetButton(IDC_PASSIVE_BTN)->SetChecked(true);
		m_pPet->SetAI( PETAI_ATK_PASSIVE );
		return true;
		break;
	case IDC_NOATK_BTN:
		GetButton(IDC_NOATK_BTN)->SetChecked(true);
		m_pPet->SetAI( PETAI_ATK_NOATK );
		return true;
		break;
	case IDC_SELATTACK_BTN:
		OnBeginSelectAttack();
		return true;
		break;
	case IDC_PET_NAME_LABEL:
		{
			CNpc* pNpc = m_pPet->GetNpc();
			if( pNpc )
			{
				//When cursor state is use access
				if ( GetGameCursor()->IsDrag() && (GetGameCursor()->GetCursorImageType() == eCursorUseAccess) )
				{
					GetScene()->GetMainCharacter()->UseSkillOnCharacter( pNpc );
					RestoreMouseIcon();
					return true;
				}
			}
			return true;
		}
		break;
	}
	return true;
	FUNCTION_END;
}

HRESULT CGuiPetCtrlDialog::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;
	//Render dialog
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	if(m_pPet)
	{
		const t_PetData *pData = m_pPet->GetProperty();
		if(pData)
		{
			DrawHp(pData->hp,pData->maxhp,pData->maxhp_plus);
			//DrawName(pData->name,fElapsedTime,m_pPet->GetNameTextColor());
		}
	}

	return hr;

	FUNCTION_END;
}

void CGuiPetCtrlDialog::DrawHp(int hp,int maxhp,int maxhp_plus)
{
	FUNCTION_BEGIN;

	if( maxhp <= 0 ) return ;

	stRectI rc(124,15,236,23);	
	rc.OffsetRect(m_x,m_y);
	if(m_pBmpHp)
	{
		stPointF scale((float) hp / (maxhp+maxhp_plus),1.0f);
		m_pBmpHp->Render(0,rc.left,rc.top,NULL,&scale,-1,Blend_Null);
	}	

	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	if(rc.PtInRect(ptCursor))
	{
		m_toolTips.Clear();
		m_toolTips.SetBkColor( D3DCOLOR_ARGB(128,0,0,0) );
		m_toolTips.SetAlignType( GUI_ALIGN_LEFT );
		m_toolTips.SetBorderColor( 0 );
		m_toolTips.SetLineGaps( 3 );
		m_toolTips.SetCurColor( -1 );
		m_toolTips.AddText(avar("当前生命值:%d",hp));
		m_toolTips.AddText(avar("\n最大生命值:%d",maxhp));
		if(maxhp_plus>0)
		{
			m_toolTips.SetCurColor(plusColor);
			m_toolTips.AddText(" + ");
			m_toolTips.AddText(avar("%d",maxhp_plus));
			m_toolTips.SetCurColor( -1 );
		}
		m_toolTips.Resize();
		m_toolTips.RenderTopLeftPrefer( rc, GetDevice()->GetWidth(), GetDevice()->GetHeight() );		
	}

	FUNCTION_END;
}

void CGuiPetCtrlDialog::DrawName(const char* name,float fElapsedTime,DWORD color)
{
	FUNCTION_BEGIN;

	int oldFont = GetDevice()->SetFont(1);
	stRectI rc(12,4,110,24);
	rc.OffsetRect(m_x,m_y);
	GetDevice()->DrawString(name,rc,color,DT_CENTER,FontEffect_Border);
	GetDevice()->SetFont(oldFont);

	FUNCTION_END;
}


void CGuiPetCtrlDialog::UpdateWindowPos()
{
	FUNCTION_BEGIN;

	//if( m_pPet ) 
	//{
	//	petType type = m_pPet->GetPetType();

	//	int height = GetDevice()->GetHeight();
	//	int x = m_x;
	//	//int y = m_y;

	//	//if( m_y > height - 170 )
	//	{
	//		//y = height - 170;
	//		//if(GetGameGuiManager()->m_guiChatSelect)
	//		//{
	//		//	stPointI pt = GetGameGuiManager()->m_guiChatSelect->GetLocation();
	//		//	y = 613 + pt.y;
	//		//}

	//		if( m_siPetCtrlNum == 2)//如果有2个对话框，则分开摆放
	//		{
	//			if( type == PET_TYPE_PET )//宠物
	//			{
	//				x = GetDevice()->GetWidth() / 2 + 45;	//右边
	//			}
	//			else if( type == PET_TYPE_SUMMON ) //召唤兽
	//			{
	//				x = GetDevice()->GetWidth() / 2 - 405; //左边
	//			}
	//			else
	//				return ;
	//		}
	//		else if( m_siPetCtrlNum == 1 )//如果只有一个，则默认摆在左边
	//		{
	//			x = GetDevice()->GetWidth() / 2 - 405; //左边
	//		}

	//		SetLocation(x,y);
	//	}
	//}

	FUNCTION_END;
}

/**
* \brief 重设对话框尺寸
* 
* 
* 
* \return 
*/
void CGuiPetCtrlDialog::OnResetScreenSize()
{
	FUNCTION_BEGIN;

	if(GetGameGuiManager()->m_guiMain)
	{
		int x = GetGameGuiManager()->m_guiMain->GetClientX();
		int y = GetGameGuiManager()->m_guiMain->GetClientY();
		stRectI rcClip = GetGameGuiManager()->m_guiMain->GetClipRect();
		rcClip.OffsetRect(x,y);

		if( GetDevice()->GetHeight() < 768 )
		{
			ResetControlBitmap(PetCtrl_Group_800_600);
			m_x = 0 ;
			m_y = rcClip.top  - 65 ;
		}
		else
		{
			ResetControlBitmap(PetCtrl_Group_1024_768);
			m_x = rcClip.left + 22 ;
			m_y = rcClip.top  - 65 ;
		}
	}

	FUNCTION_END;
}

void CGuiPetCtrlDialog::ResetControlBitmap(int group)
{
	stResourceLocation rl;
	rl.SetFileName( GetGuiGraphicPackName() );
	rl.group = group;
	rl.frame = 18;
	m_pBmpHp = GetDevice()->FindBitmaps( &rl );	
	SetBackImage(stResourceLocation(GetGuiGraphicPackName(),group,17));

	PetCtrl_SetButtonBitmap(IDC_FOLLOW_BTN,group,19);
	PetCtrl_SetButtonBitmap(IDC_STAND_BTN,group,31);
	PetCtrl_SetButtonBitmap(IDC_SELATTACK_BTN,group,38);
	PetCtrl_SetButtonBitmap(IDC_ATTACK_BTN,group,28);
	PetCtrl_SetButtonBitmap(IDC_PASSIVE_BTN,group,22);
	PetCtrl_SetButtonBitmap(IDC_NOATK_BTN,group,25);
	if(m_pPet)
	{
		UpdatePetInfo(m_pPet->GetPetType());
	}
}