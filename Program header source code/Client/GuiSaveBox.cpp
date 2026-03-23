#include "public.h"
#include "./GameGuiManager.h"
#include ".\GuiSaveBox.h"
#include "./table.h"
#include "./RoleItem.h"
#include "./GuiInputBox.h"
#include "./GameState.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiEquip.h"
#include "./GuiUserInfo.h"
#include "./GuiItem.h"
#include "safetyuser.h"

#define IDC_SHOWEQUIP 5
#define IDC_HIDEEQUIP 6
#define IDC_CONNIMAGE 6

#define IDC_BOX1_TABLE			1
#define IDC_BOX2_TABLE			2
#define IDC_BOX3_TABLE			3

#define IDC_BOX_TAB				4

#define IDC_CONTINUEHIRE_BTN1	5
#define IDC_CONTINUEHIRE_BTN2	6
#define IDC_CONTINUEHIRE_BTN3	7

#define IDC_BOX1_BTN			8
#define IDC_BOX2_BTN			9
#define IDC_BOX3_BTN			10

//#define IDC_LAST_DAYS1			11
//#define IDC_LAST_DAYS2			12
//#define IDC_LAST_DAYS3			13

/**
 * \brief 简短描述 构造函数
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CGuiSavingBoxDialog::CGuiSavingBoxDialog(void)
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;

	FUNCTION_END;
}

/**
 * \brief 简短描述 窗体创建
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiSavingBoxDialog::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_pTable[0] = GetTable(IDC_BOX1_TABLE);
	m_pTable[0]->m_iTableType = OBJECTCELLTYPE_STORE;
	m_pTable[0]->m_iTableID = IDC_BOX1_TABLE;

	m_pTable[1] = GetTable(IDC_BOX2_TABLE);
	m_pTable[1]->m_iTableType = OBJECTCELLTYPE_STORE;
	m_pTable[1]->m_iTableID = IDC_BOX2_TABLE;

	m_pTable[2] = GetTable(IDC_BOX3_TABLE);
	m_pTable[2]->m_iTableType = OBJECTCELLTYPE_STORE;
	m_pTable[2]->m_iTableID = IDC_BOX3_TABLE;

	AddControl2TabByID(IDC_BOX_TAB,0,IDC_BOX1_TABLE);
	//AddControl2TabByID(IDC_BOX_TAB,0,IDC_CONTINUEHIRE_BTN1);
	//AddControl2TabByID(IDC_BOX_TAB,0,IDC_LAST_DAYS1);

	AddControl2TabByID(IDC_BOX_TAB,1,IDC_BOX2_TABLE);
	//AddControl2TabByID(IDC_BOX_TAB,1,IDC_CONTINUEHIRE_BTN2);
	//AddControl2TabByID(IDC_BOX_TAB,1,IDC_LAST_DAYS2);

	AddControl2TabByID(IDC_BOX_TAB,2,IDC_BOX3_TABLE);
	//AddControl2TabByID(IDC_BOX_TAB,2,IDC_CONTINUEHIRE_BTN3);
	//AddControl2TabByID(IDC_BOX_TAB,2,IDC_LAST_DAYS3);

	GetTab(IDC_BOX_TAB)->SetVisible(false);

	GetButton(IDC_BOX1_BTN)->SetButtonGroup(1);
	GetButton(IDC_BOX2_BTN)->SetButtonGroup(1);
	GetButton(IDC_BOX3_BTN)->SetButtonGroup(1);

	bool bRes = ResetContents();
	//if(bRes) Close();

	OnGuiEvent(EVENT_BUTTON_CLICKED,IDC_BOX1_BTN,GetButton(IDC_BOX1_BTN));

	FUNCTION_END;
}

/**
 * \brief 简短描述 窗体关闭
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiSavingBoxDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiSavingBoxDlg=NULL;

	GetGameGuiManager()->OnNpcActionEnd();

	FUNCTION_END;
}

/**
 * \brief 简短描述 设置关闭按钮位置
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiSavingBoxDialog::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();


	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x - 65,pt.y);
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述 响应窗体事件
 * 
 * 详细描述
 * 
 * \param nEvent : 描述 事件类型
 * \param nID : 描述 被触发的对象id
 * \param pControl : 描述 被触发的对象指针
 * \return 返回值的描述 响应是否成功
 */
bool CGuiSavingBoxDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID)
			{
			//case IDC_CONTINUEHIRE_BTN1:
			//case IDC_CONTINUEHIRE_BTN2:
			//case IDC_CONTINUEHIRE_BTN3:
			//	GetGameGuiManager()->AddSaveBoxContinueDlg(0);
			//	break;
			case IDC_BOX1_BTN:
				{
					GetTab(IDC_BOX_TAB)->SetCurItem(0);
					GetButton(IDC_BOX1_BTN)->SetChecked(true);
					return true;
				}
				break;
			case IDC_BOX2_BTN:
				{
					GetTab(IDC_BOX_TAB)->SetCurItem(1);
					GetButton(IDC_BOX2_BTN)->SetChecked(true);
					return true;
				}
				break;
			case IDC_BOX3_BTN:
				{
					GetTab(IDC_BOX_TAB)->SetCurItem(2);
					GetButton(IDC_BOX3_BTN)->SetChecked(true);
					return true;
				}
				break;
			case GUI_CLOSEBOX_ID:
				{
					if(GetGameGuiManager()->m_guiItem)
					{
						GetGameGuiManager()->m_guiItem->SetVisible(false);
					}
					GetGameGuiManager()->OnNpcActionEnd();
					SetVisible(false);
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
//
//inline void CGuiSavingBoxDialog::SetDay(int id,int day)
//{
//	//Debug_Int(id);
//	//Debug_Int(day);
//	if( day < 1 ) day = 0 ;	
//	CGuiStatic* p = GetStatic(id);
//	if(p)
//	{
//		p->SetText(avar("还可用%d天",day));
//		if(day > 7)
//		{
//			p->SetTextColor(COLOR_ARGB(255,0,255,0));
//		}
//		else
//			p->SetTextColor(COLOR_ARGB(255,237,27,35));
//	}
//}

bool CGuiSavingBoxDialog::ResetContents()
{
	FUNCTION_BEGIN;

#define _EnableButton(id,b) {CGuiButton* pBtn = GetButton(id); if(pBtn) pBtn->SetEnabled(b);}
	CMainCharacter *pMainChar = GetScene()->GetMainCharacter();
	if(pMainChar)
	{
		int savingbox = pMainChar->GetRealSavingBoxNum();

		if( CSafetyUser::Instance()->IsSafe23Pack() )
		{
			_EnableButton(IDC_BOX3_BTN,false)
			_EnableButton(IDC_BOX2_BTN,false)
			if( savingbox > 1)
				GetGameGuiManager()->AddClientSystemMessage("从第二三个包裹中拿出物品,需要验证!");
		}
		else 
		{
			if( savingbox > 2 && !CSafetyUser::Instance()->IsSafe23Pack())
				_EnableButton(IDC_BOX3_BTN,true)
			else 
			_EnableButton(IDC_BOX3_BTN,false)

			if( savingbox > 1 && !CSafetyUser::Instance()->IsSafe23Pack()) 
				_EnableButton(IDC_BOX2_BTN,true)
			else
			_EnableButton(IDC_BOX2_BTN,false)
		}
		

		if( savingbox > 0 ) _EnableButton(IDC_BOX1_BTN,true)
		else _EnableButton(IDC_BOX1_BTN,false)
		
		if(savingbox == 0) 
		{
			Close();
			return false;
		}
		else 
		{
			//t_SavingBox* p = pMainChar->GetSavingBox();
			//if(p)
			//{
			//	SetDay(IDC_LAST_DAYS1,p->days[0]);
			//	SetDay(IDC_LAST_DAYS2,p->days[1]);
			//	SetDay(IDC_LAST_DAYS3,p->days[2]);
			//}
			OnGuiEvent(EVENT_BUTTON_CLICKED,IDC_BOX1_BTN,GetButton(IDC_BOX1_BTN));
			return true;
		}
	}
	return false;

	FUNCTION_END;
}

CGuiTable* CGuiSavingBoxDialog::GetBoxTable(const stObjectLocation & pos)
{
	FUNCTION_BEGIN;

	switch(pos.dwTableID)
	{
	case 0:
	case IDC_BOX1_TABLE:
		return GetTableByPosY(pos);
	case IDC_BOX2_TABLE:
		return m_pTable[1];
	case IDC_BOX3_TABLE:
		return m_pTable[2];
	}
	return NULL;


	FUNCTION_END;
}

inline CGuiTable* CGuiSavingBoxDialog::GetTableByPosY(const stObjectLocation pos)
{
	int index = GetStoreTableIndex(pos);
	if ( index < 0 || index > 2 ) 
		return NULL;
	else 
		return m_pTable[index];
}

HRESULT CGuiSavingBoxDialog::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;
	//Render dialog
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	CMainCharacter *pMainChar = GetScene()->GetMainCharacter();
	if(pMainChar)
	{
		int oldFont = GetDevice()->SetFont(1);
		stRectI rc(100,115,270,145);
		rc.OffsetRect(m_x,m_y);
		GetDevice()->DrawString(pMainChar->GetName(),rc,-1,DT_CENTER);
		GetDevice()->SetFont(oldFont);
	}

	return hr;

	FUNCTION_END;
}
