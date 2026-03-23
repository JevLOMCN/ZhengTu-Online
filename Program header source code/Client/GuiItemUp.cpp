
#include "./public.h"
#include "./GameGuiManager.h"
#include ".\guiitemup.h"
#include "./table.h"
#include "./RoleItem.h"
#include "./GuiInputBox.h"
#include "./GameState.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiNpcDialog.h"
#include "./GuiItem.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiItemUp::CGuiItemUp(bool bChange)
: m_bChange(false)
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;
	m_bChange=bChange;

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiItemUp::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	return CGuiDialog::OnRender(fElapsedTime);

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
	FUNCTION_END;
}
void CGuiItemUp::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	/*
	if (m_bChange)
	{
		m_pLabel1 = GetStatic(11);
		m_pTable1 = GetTable(8);
		m_pTable1->m_iTableType = OBJECTCELLTYPE_COMPOSE;

	}
	else
	{
		m_pLabel2 = GetStatic(7);
		m_pTable2 = GetTable(2);
		m_pTable2->m_iTableType = OBJECTCELLTYPE_UPGRADE;
		m_pTable2->m_EquipPosition = stPointI(0,UPGRADECELLTYPE_UPER);
		m_pTable3 = GetTable(4);
		m_pTable3->m_iTableType = OBJECTCELLTYPE_UPGRADE;
		m_pTable3->m_EquipPosition = stPointI(0,UPGRADECELLTYPE_JEWEL1);
		m_pTable4 = GetTable(12);
		m_pTable4->m_iTableType = OBJECTCELLTYPE_UPGRADE;
		m_pTable4->m_EquipPosition = stPointI(0,UPGRADECELLTYPE_JEWEL2);
	}
	*/

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiItemUp::OnClose(void)
{
	FUNCTION_BEGIN;

	
	if (m_bChange)
	{
		
		//GetGameGuiManager()->m_guiItemChange=NULL;
		
	}
	else
	{
		//GetGameGuiManager()->m_guiItemUp=NULL;
	}

	FUNCTION_END;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param params : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiItemUp::SetParam(std::vector<std::string> & params)
{
	FUNCTION_BEGIN;

	if (m_bChange)
	{
		if(params.size() > 1)
		{
			m_pLabel1->SetText(params[1].c_str());
		}
		else
		{
			m_pLabel1->SetText("");
		}
	}
	else
	{
		if(params.size() > 1)
		{
			m_pLabel2->SetText(params[1].c_str());
		}
		else
		{
			m_pLabel2->SetText("");
		}

	}

	
	GetGameState()->SetStateItem(eGameStateItem_Up);


	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nEvent : ÃèÊö
 * \param nID : ÃèÊö
 * \param pControl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiItemUp::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	stNullUserCmd  stUpCmd={0};
	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		switch(nID)
		{
		case 5:	// Éı¼¶
			{
				stUpgradeItemPropertyUserCmd stUpCmd;
				SEND_USER_CMD(stUpCmd);
			}
			break;
		case 9: //ºÏ³É
			{
				/*
				stComposeItemPropertyUserCmd stComposeCmd;
				if (m_pTable1)
				{
					if (m_pTable1->m_listItem.size()>0)
					{
						CRoleItem* pItem=(CRoleItem*)m_pTable1->m_listItem[0]->m_pItemData;
						stComposeCmd.dwObjectID=pItem->GetObjectID()+1;
					}	
					else
					{
						stComposeCmd.dwObjectID=0;
					}
				}
				SEND_USER_CMD(stComposeCmd);
				*/
			}
			break;
		case -1:
		case 6:	// ÍË³ö
		case 10:
			{

				
				GetGameGuiManager()->OnNpcActionEnd();
			}
			break;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}