
 

#include "public.h"
#include "./GameGuiManager.h"
#include "./GuiBossInfo.h"
#include "./GameScene.h"
#include "./character.h"
#include "./MainCharacter.h"
#include "./CreateRole.h"
#include "./GuiWorldConn.h"
#include "./GuiItem.h"
#include "./RoleItem.h"
#include "./GameGuiFloatItem.h"
#include "./Country.h"


void CGuiBossInfoDialog::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
	  m_pBtnClose->SetLocation( 329 + 116 + 160, 12 + 1 );
	}

	FUNCTION_END;
}

void CGuiBossInfoDialog::OnCreate()
{
	FUNCTION_BEGIN;

	//CGuiDialog::OnCreate();
	m_pListBox=NULL;
	m_pListBox = GetListBox(12);

	FUNCTION_END;
}

//soke 暂时用不到这个了
void CGuiBossInfoDialog::OnClose()
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiBossInfoDialog = NULL;


	FUNCTION_END;
}

void CGuiBossInfoDialog::OnResetScreenSize()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnResetScreenSize();

	m_x = ( GetDevice()->GetWidth() - GetImage( 0 )->GetWidth() ) / 2;
	m_y = ( GetDevice()->GetHeight() - GetImage( 0 )->GetHeight() ) / 2;

	FUNCTION_END;
}

bool CGuiBossInfoDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID)
			{
			case 23:
				{
					break;
				}
			case 24:
				{
					SetVisible(false);
					break;
				}
			}

			break;
		}
	}
	
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void  CGuiBossInfoDialog::SetInfo(char* c)
{
	if( m_pListBox != NULL )
	{
		m_pListBox->RemoveAllItems();
		m_pListBox->AddItem( c, 0 );

	}
}