/**
 *\file		CGuiClearPointDlg.cpp
 *\version	$ID$
 *\author	
 *\date		2005-12-22 10:00:00
 *\brief	洗属性点对话框
 *
 *
 */
#include "public.h"
#include "./GameGuiManager.h"
#include "GuiClearPointDlg.h"
#include "RoleItem.h"
#include "../gui/include/GuiManager.h"

#include "GameScene.h"
#include "MainCharacter.h"

// roleitem.cpp call this function 
bool OnUseClearPointItem(CRoleItem  * pRoleItem)
{
	DWORD dwObjectBaseID = pRoleItem->GetObjectBase()->dwID;
	if( dwObjectBaseID == ID_CLEARPOINT_PROPERTY_JADE)
	{
		GetGameGuiManager()->AddClearPointDlg();
		if( GetGameGuiManager()->m_guiClearPoint)
			GetGameGuiManager()->m_guiClearPoint->SetObjectThisID(pRoleItem->GetThisID());
		return true;
	}
	if( dwObjectBaseID == ID_CLEARPOINT_MARROW_BIJOU ||
		dwObjectBaseID == ID_CLEARPOINT_MARROW_BIJOU_NEW ||
        dwObjectBaseID == ID_CLEARPOINT_TENDON_BIJOU ||
		dwObjectBaseID == ID_CLEARPOINT_TENDON_BIJOU_NEW ||
	    dwObjectBaseID == ID_CLEARPOINT_MARROW_BOOK || 
	    dwObjectBaseID == ID_CLEARPOINT_TENDON_BOOK
	    )
	{  // 使用洗点道具
		char szText[60] = {0};
		DWORD dwLevel  = GetScene()->GetMainCharacter()->GetLevel();
		if( ( (dwObjectBaseID == ID_CLEARPOINT_MARROW_BIJOU || dwObjectBaseID == ID_CLEARPOINT_MARROW_BIJOU_NEW || dwObjectBaseID == ID_CLEARPOINT_TENDON_BIJOU || dwObjectBaseID == ID_CLEARPOINT_TENDON_BIJOU_NEW) && dwLevel < 40)
			|| ( (dwObjectBaseID == ID_CLEARPOINT_MARROW_BOOK || dwObjectBaseID == ID_CLEARPOINT_TENDON_BOOK) && dwLevel >= 40) ) 
		{
			GetGameGuiManager()->AddClientSystemMessage("等级不符，不能使用");
			return false;
		}
		if( dwObjectBaseID == ID_CLEARPOINT_MARROW_BIJOU || dwObjectBaseID == ID_CLEARPOINT_MARROW_BIJOU_NEW || dwObjectBaseID == ID_CLEARPOINT_MARROW_BOOK) //洗髓经 | 洗髓宝珠
		{
			strncpy(szText,"你真的想清洗你的属性点吗？",sizeof(szText));
			szText[sizeof(szText)-1] = 0;
		}
		else  if( dwObjectBaseID == ID_CLEARPOINT_TENDON_BIJOU || dwObjectBaseID == ID_CLEARPOINT_TENDON_BIJOU_NEW || dwObjectBaseID == ID_CLEARPOINT_TENDON_BOOK) //易筋经 | 易筋宝珠
		{
			//soke 增加清洗职业说明
			strncpy(szText,"你真的想清洗你的技能点吗？",sizeof(szText));
			szText[sizeof(szText)-1] = 0;
		}
		if( 0 != strlen(szText) )
		{
			GetGameGuiManager()->AddMessageBox(szText,eAction_UseClearPoint,(void*)dwObjectBaseID,c_szMBOKCancel);
		}
		return true;
	}
	return false;
}
 
bool ClearPoint_CannotTrade(CRoleItem * pRoleItem)
{
	if( pRoleItem->GetObjectBase()->dwID == ID_CLEARPOINT_MARROW_BIJOU || pRoleItem->GetObjectBase()->dwID == ID_CLEARPOINT_TENDON_BIJOU)
	{
		char szTip[40] = {0};
		_snprintf(szTip,40,"对不起,%s不可交易!",pRoleItem->GetObjectBase()->strName);
		szTip[sizeof(szTip)-1] = 0;
		GetGameGuiManager()->AddClientSystemMessage(szTip);
		return true;
	}
	return false;	
}

bool ClearPoint_CannotMail(CRoleItem * pRoleItem)
{

	if(pRoleItem->GetObjectBase()->dwID ==  ID_CLEARPOINT_MARROW_BIJOU || 
           pRoleItem->GetObjectBase()->dwID == ID_CLEARPOINT_TENDON_BIJOU)
	{
		char szTip[40] = {0};
		_snprintf(szTip,sizeof(szTip),"对不起,%s不可摆摊",pRoleItem->GetObjectBase()->strName);
		szTip[sizeof(szTip)-1] = 0;
		GetGameGuiManager()->AddClientSystemMessage(szTip);
		return true;
	}
	return false;
}

// [注两种洗点道具也归结为ItemType_Quest]
bool ClearPoint_CannotTradeAndMail(DWORD dwObjectBaseID)
{
	if( dwObjectBaseID == ID_CLEARPOINT_MARROW_BIJOU 
		|| dwObjectBaseID == ID_CLEARPOINT_TENDON_BIJOU )
	{
		return true;
	}
	return false;
		
}
// maincharacter call this function 
void MSG_ClearPointOrSkill(DWORD dwObjectBaseID)
{
	switch(dwObjectBaseID)
	{
	case ID_CLEARPOINT_MARROW_BIJOU:
	case ID_CLEARPOINT_MARROW_BIJOU_NEW:
	case ID_CLEARPOINT_MARROW_BOOK:  
		{
			stClearPointPropertyUserCmd cmd;
			cmd.dwItemID = dwObjectBaseID;
			SEND_USER_CMD(cmd);
		}
		break;
	case ID_CLEARPOINT_TENDON_BIJOU:
	case ID_CLEARPOINT_TENDON_BIJOU_NEW:
	case ID_CLEARPOINT_TENDON_BOOK: 
		{
			stClearUserSkillPointPropertyUserCmd cmd;
			cmd.dwItemID = dwObjectBaseID;
			SEND_USER_CMD(cmd);
		}
		break;
	}
}


//-------------------------------------------------------
// button cmd id 
#define ID_BUTTON_STRENGTH          3   // 力量
#define ID_BUTTON_INTELLECTION      4   // 智力
#define ID_BUTTON_CELERITY          5   // 敏捷
#define ID_BUTTON_SPIRIT            6   // 精神
#define ID_BUTTON_BODY              7   // 体质


#define ID_BUTTON_CLOSE              2 
//-------------------------------------------------------

#define ClearPointDlg_ButtonSetEnable(id,bEnable)	{CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ pBtn->SetEnabled(bEnable);}}
#define ClearPointDlg_ButtonSetEnableDelay(id,bEnable)	{CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ pBtn->SetEnableDelay(bEnable,5500);}}
#define ClearPointDlg_TestBtnEnabled(id)		{CGuiButton* pBtn = GetButton(id);if( NULL == pBtn || !(pBtn->IsEnabled()) ){ return false;}}

#define ClearPointDlg_AllButtonSetEnableDelay(id1,id2,id3,id4,id5) \
	{\
	    ClearPointDlg_ButtonSetEnable(id1,false);\
	    ClearPointDlg_ButtonSetEnable(id2,false);\
            ClearPointDlg_ButtonSetEnable(id3,false);\
            ClearPointDlg_ButtonSetEnable(id4,false);\
            ClearPointDlg_ButtonSetEnable(id5,false);\
	    ClearPointDlg_ButtonSetEnableDelay(id1,true);\
	    ClearPointDlg_ButtonSetEnableDelay(id2,true);\
	    ClearPointDlg_ButtonSetEnableDelay(id3,true);\
	    ClearPointDlg_ButtonSetEnableDelay(id4,true);\
	    ClearPointDlg_ButtonSetEnableDelay(id5,true);\
	}

CGuiClearPointDlg::CGuiClearPointDlg()
: m_dwObjectThisID(0)
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiClearPointDlg::~CGuiClearPointDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuiClearPointDlg::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	GetButton(ID_BUTTON_CLOSE)->SetLocation(-534,-2);

	FUNCTION_END;
}

void CGuiClearPointDlg::InternalSendMsgToServer(BYTE byProperty)
{
	assert( byProperty == (BYTE)PROPERTY_CON || //体质
		byProperty == (BYTE)PROPERTY_STR ||//力量
		byProperty == (BYTE)PROPERTY_DEX ||//敏捷
		byProperty == (BYTE)PROPERTY_INT ||//智力
		byProperty == (BYTE)PROPERTY_MEN   //精神
		); 

	if(byProperty != (BYTE)PROPERTY_CON &&
		byProperty != (BYTE)PROPERTY_STR &&
		byProperty != (BYTE)PROPERTY_DEX &&
		byProperty != (BYTE)PROPERTY_INT &&
		byProperty != (BYTE)PROPERTY_MEN 
		)
		return;

	assert( m_dwObjectThisID != 0);
	if( m_dwObjectThisID == 0) return;

	stClearPointLimitPropertyUserCmd cmd;
	cmd.qwThisID = m_dwObjectThisID;
	cmd.byProperty = byProperty;
	SEND_USER_CMD(cmd);
}

#define ClearPoint_AfterBtnClick() \
      ClearPointDlg_AllButtonSetEnableDelay(ID_BUTTON_STRENGTH,ID_BUTTON_INTELLECTION,ID_BUTTON_CELERITY,ID_BUTTON_SPIRIT,ID_BUTTON_BODY);


bool CGuiClearPointDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
		case ID_BUTTON_STRENGTH://力量
			InternalSendMsgToServer(PROPERTY_STR);
			ClearPoint_AfterBtnClick();
			Close();
			break;
		case ID_BUTTON_INTELLECTION:  // 智力  
			InternalSendMsgToServer(PROPERTY_INT);
			ClearPoint_AfterBtnClick();
			Close();
			break;
		case ID_BUTTON_CELERITY:      // 敏捷    
			InternalSendMsgToServer(PROPERTY_DEX);
			ClearPoint_AfterBtnClick();
			Close();
			break;
		case ID_BUTTON_SPIRIT:       // 精神   
			InternalSendMsgToServer(PROPERTY_MEN);
			ClearPoint_AfterBtnClick();
			Close();
			break;
		case ID_BUTTON_BODY:// 体质
			InternalSendMsgToServer(PROPERTY_CON);
			ClearPoint_AfterBtnClick();
			Close();
			break;
		case ID_BUTTON_CLOSE:
			Close();
			break;
		}
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiClearPointDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiClearPoint = NULL;

	FUNCTION_END;
}
