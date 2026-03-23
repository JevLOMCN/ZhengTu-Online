#include "public.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./RoleItem.h"

#include ".\guipackage.h"

#include "guiItem.h"

CGuiPackage::CGuiPackage(CRoleItem* pRoleItem)
:m_pRoleItem(pRoleItem)
{
	m_bCloseIsHide = true;
}

void CGuiPackage::OnCreate()
{
	CGuiDialog::OnCreate();

	this->SetCaptionText(m_pRoleItem->GetObjectName());

	m_pTable = GetTable(2);

	m_pTable->m_iTableType = OBJECTCELLTYPE_PACKAGE;
	m_pTable->m_iTableID = m_pRoleItem->GetThisID();
	m_pTable->m_EquipPosition = stPointI(m_pRoleItem->GetLocation().x,m_pRoleItem->GetLocation().y);
	
	m_pTable->m_showRowCount = m_pRoleItem->GetObjectBase()->maxmdam;
	m_pTable->ResetSize(m_pRoleItem->GetObjectBase()->maxpdam,
		m_pRoleItem->GetObjectBase()->maxmdam);

	// 2010 8	
	//GetStatic(10)->SetText(m_pRoleItem->GetName());

	switch(m_pRoleItem->GetObjectBase()->dwID) 
	{ //soke 额外包裹打开的图片乾坤袋
	case 662:
		SetBackImage(stResourceLocation("data\\interfaces1.gl",26,2));
		break;
	case 663:
		SetBackImage(stResourceLocation("data\\interfaces1.gl",26,3));
		break;
	case 664:
		SetBackImage(stResourceLocation("data\\interfaces1.gl",26,40));
		break;
	case 810:
		SetBackImage(stResourceLocation("data\\interfaces1.gl",26,41));
		break;
	case 811:
		SetBackImage(stResourceLocation("data\\interfaces1.gl",26,1));
		break;
	}

}

bool CGuiPackage::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
			case 4:
			{
			}
			break;
		}
		SetVisible(false);
		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiPackage::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();


	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

        //soke 包裹关闭窗口
		m_pBtnClose->SetLocation(pt.x,pt.y-86);

		//switch(m_pRoleItem->GetObjectBase()->dwID) 
		//{
		//case 662:
		//	m_pBtnClose->SetLocation(pt.x-63,pt.y);
		//	break;
		//case 663:
		//	m_pBtnClose->SetLocation(pt.x-63,pt.y);
		//	break;
		//case 664:
		//	m_pBtnClose->SetLocation(pt.x-63,pt.y);
		//	break;
		//case 810:
		//	m_pBtnClose->SetLocation(pt.x-63,pt.y);
		//	break;
		//}
	}


	FUNCTION_END;
}

HRESULT CGuiPackage::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	GetButton( 4 )->SetLocation( 270, 5 );

	if( ! ::GetGameGuiManager()->m_guiItem->IsVisible() )
	{
		this->SetVisible( false );
	}

	HRESULT h = CGuiDialog::OnRender(fElapsedTime);

	// Title 这里是包裹的名字位置
	int iOldFont = GetDevice()->SetFont(1);
	int w = -82;
	int H = 32 - 4;
	stRectI rect( 210 + w, 12 + H, 370 + w, 30 + H );

	rect.OffsetRect(m_x,m_y);
	GetDevice()->DrawString( m_pRoleItem->GetName(), rect );
	GetDevice()->SetFont( iOldFont );

	return h;
	
	FUNCTION_END;
}

void CGuiPackage::OnClose(void)
{
	if (!m_bCloseIsHide)
	{
		switch(m_pTable->m_EquipPosition.x) 
		{
		case EQUIPCELLTYPE_LEFT:
			GetGameGuiManager()->m_guiPackage1 = NULL;
			break;
		case EQUIPCELLTYPE_RIGHT:
			GetGameGuiManager()->m_guiPackage2 = NULL;
			break;
		}
	}
}
