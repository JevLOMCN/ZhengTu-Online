#include "./public.h"
#include "./GameGuiManager.h"
#include ".\guinpcsave.h"
#include "./table.h"
#include "./RoleItem.h"
#include "./GuiInputBox.h"
#include "./GameState.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiEquip.h"
#include "./GuiUserInfo.h"
#include "./GuiItem.h"

#define IDC_SHOWEQUIP 5
#define IDC_HIDEEQUIP 6
#define IDC_CONNIMAGE 6

#define IDC_BOX1_TABLE			1
#define IDC_BOX2_TABLE			2
#define IDC_BOX3_TABLE			3

#define IDC_BOX_TAB				4
#define IDC_CONTINUEHIRE_BTN	5
#define IDC_BOX1_BTN			6
#define IDC_BOX2_BTN			7
#define IDC_BOX3_BTN			8
#define IDC_LAST_DAYS			9

/**
 * \brief ¼ò¶ÌÃèÊö ¹¹Ôìº¯Êý
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiSavingBoxDialog::CGuiSavingBoxDialog(void)
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö ´°Ìå´´½¨
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiSavingBoxDialog::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

#ifdef __yjt_saving_box
	m_pTable[0] = GetTable(IDC_BOX1_TABLE);
	m_pTable[0]->m_iTableType = OBJECTCELLTYPE_STORE_BOX1;
	m_pTable[0]->m_iTableID = IDC_BOX1_TABLE;

	m_pTable[1] = GetTable(IDC_BOX2_TABLE);
	m_pTable[1]->m_iTableType = OBJECTCELLTYPE_STORE_BOX2;
	m_pTable[1]->m_iTableID = IDC_BOX2_TABLE;

	m_pTable[2] = GetTable(IDC_BOX3_TABLE);
	m_pTable[2]->m_iTableType = OBJECTCELLTYPE_STORE_BOX3;
	m_pTable[2]->m_iTableID = IDC_BOX3_TABLE;
#else
	m_pTable[0] = GetTable(IDC_BOX1_TABLE);
	m_pTable[0]->m_iTableType = OBJECTCELLTYPE_STORE;
	m_pTable[0]->m_iTableID = IDC_BOX1_TABLE;

	m_pTable[1] = GetTable(IDC_BOX2_TABLE);
	m_pTable[1]->m_iTableType = OBJECTCELLTYPE_STORE;
	m_pTable[1]->m_iTableID = IDC_BOX2_TABLE;

	m_pTable[2] = GetTable(IDC_BOX3_TABLE);
	m_pTable[2]->m_iTableType = OBJECTCELLTYPE_STORE;
	m_pTable[2]->m_iTableID = IDC_BOX3_TABLE;
#endif

	AddControl2TabByID(IDC_BOX_TAB,0,IDC_BOX1_TABLE);
	AddControl2TabByID(IDC_BOX_TAB,1,IDC_BOX2_TABLE);
	AddControl2TabByID(IDC_BOX_TAB,2,IDC_BOX3_TABLE);

	GetTab(IDC_BOX_TAB)->SetVisible(false);

	GetButton(IDC_BOX1_BTN)->SetButtonGroup(1);
	GetButton(IDC_BOX2_BTN)->SetButtonGroup(1);
	GetButton(IDC_BOX3_BTN)->SetButtonGroup(1);

	OnGuiEvent(EVENT_BUTTON_CLICKED,IDC_BOX1_BTN,GetButton(IDC_BOX1_BTN));

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö ´°Ìå¹Ø±Õ
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiSavingBoxDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiSavingBoxDlg=NULL;


/**
 * \brief ¼ò¶ÌÃèÊö ÉèÖÃ¹Ø±Õ°´Å¥Î»ÖÃ
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
	FUNCTION_END;
}
void CGuiSavingBoxDialog::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();


	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x-113,pt.y);
	}

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö ÉèÖÃ½ðÇ®
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dwMoney : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiSavingBoxDialog::SetMoney(DWORD dwMoney)
{
	FUNCTION_BEGIN;

	char szMoney[32];
	sprintf(szMoney,"%u",dwMoney);
	//m_pMoneyBox->SetText(szMoney);

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö »ñÈ¡Ç®±Ò
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
DWORD CGuiSavingBoxDialog::GetMoney()
{
	FUNCTION_BEGIN;

	return 10;

	//return strtoul(m_pMoneyBox->GetText(),NULL,10);

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiSavingBoxDialog::OnMoveMoney()
{
	FUNCTION_BEGIN;



	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö ³õÊ¼»¯½Å±¾
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiSavingBoxDialog::OnInitScript()
{
	FUNCTION_BEGIN;

	GetScriptEngine()->Register<CGuiSavingBoxDialog>();

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö ÏìÓ¦´°ÌåÊÂ¼þ
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nEvent : ÃèÊö ÊÂ¼þÀàÐÍ
 * \param nID : ÃèÊö ±»´¥·¢µÄ¶ÔÏóid
 * \param pControl : ÃèÊö ±»´¥·¢µÄ¶ÔÏóÖ¸Õë
 * \return ·µ»ØÖµµÄÃèÊö ÏìÓ¦ÊÇ·ñ³É¹¦
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
			case IDC_CONTINUEHIRE_BTN:
				break;
			case IDC_BOX1_BTN:
				{
					GetTab(IDC_BOX_TAB)->SetCurItem(0);
					GetButton(IDC_BOX1_BTN)->SetChecked(true);
				}
				break;
			case IDC_BOX2_BTN:
				{
					GetTab(IDC_BOX_TAB)->SetCurItem(1);
					GetButton(IDC_BOX2_BTN)->SetChecked(true);
				}
				break;
			case IDC_BOX3_BTN:
				{
					GetTab(IDC_BOX_TAB)->SetCurItem(2);
					GetButton(IDC_BOX3_BTN)->SetChecked(true);
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