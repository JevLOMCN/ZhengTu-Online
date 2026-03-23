#include "public.h"
#include "../gui/include/guiTypes.h"
#include ".\gameguimanager.h"
#include "Game.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include ".\GuiPetPackDialog.h"
#include "./CartoonPet.h"

const int CGuiPetPackDialog::m_iPetPackTableID = 2;


CGuiPetPackDialog::CGuiPetPackDialog(void)
{
	m_pPetPackTable = NULL;
	memset(m_byCellState,0,sizeof(m_byCellState));
}

CGuiPetPackDialog::~CGuiPetPackDialog(void)
{
	m_pPetPackTable = NULL;
}

void CGuiPetPackDialog::OnClose(void)
{
	GetGameGuiManager()->m_guiPetPackDlg = NULL;
	m_pPetPackTable = NULL;
}

void CGuiPetPackDialog::OnCreate()
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();
	m_pPetPackTable = GetTable(m_iPetPackTableID);
	AssertCreateCtrl(m_pPetPackTable,"替身宝宝包裹对话框脚本错误");
	m_pPetPackTable->m_iTableType = OBJECTCELLTYPE_PET;
	m_pPetPackTable->m_iTableID = 1;

	UpdateCanUseCells();

	FUNCTION_END;
}

HRESULT CGuiPetPackDialog::OnRender( float fElapsedTime )
{
	HRESULT hr = CGuiDialog::OnRender( fElapsedTime );

//#ifdef DEBUG
//	stPointI pt = m_pPetPackTable->GetLocation();
//	int h = m_pPetPackTable->GetHeight();
//	int w = m_pPetPackTable->GetWidth();
//	stRectI rc(pt.x,pt.y,pt.x + w,pt.y + h);
//	rc.OffsetRect(m_x,m_y);
//	GetDevice()->DrawRectangle(&rc,0xff00ff00);
//
	stPointI pt = m_pPetPackTable->GetLocation();
	for(int i = 0; i < 10; i++)
	{
		for(int j = 0; j < 8; j++ )
		{
			stRectI r(pt.x,pt.y,pt.x+38,pt.y+38);
			r.OffsetRect(m_x+j*38,m_y+i*38);
			if(isset_state(m_byCellState+i,j))
			{
				GetDevice()->FillRect(&r,0x0f00ff00);
			}
			else
			{
				GetDevice()->FillRect(&r,0x0fff0000);
			}
		}
	}

	return hr;
}

bool CGuiPetPackDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	if( EVENT_BUTTON_CLICKED == nEvent && nID == 3 )
	{
		OnClickOK();
		return true;
	}
	if( EVENT_BUTTON_CLICKED == nEvent && nID == -1 )
	{
		SetVisible(false);
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

void CGuiPetPackDialog::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());
		m_pBtnClose->SetLocation(pt.x - 25,pt.y);
	}

	FUNCTION_END;
}

void CGuiPetPackDialog::OnClickOK()
{
	SetVisible(false);
	stSellAllCartoonCmd cmd;
	SEND_USER_CMD(cmd);
}

void CGuiPetPackDialog::UpdateCanUseCells()
{
	int num = GetScene()->GetCartoonPetList()->GetCanUseCellsNum();	

	int row = num / 10;
	if( row > 0 )
	{
		memset(m_byCellState,0xff,row);
	}
	for(int t = 0; t < (num % 8); ++t)
	{
		set_state(m_byCellState+row,t);
	}	
}

bool CGuiPetPackDialog::bIsCellCanUse(int x,int y)
{
	if( y > 9 || x > 7 ) return false;
	return isset_state(m_byCellState+y,x);
}

bool CGuiPetPackDialog::bIsCellCanUse(POINT pt)
{
	return bIsCellCanUse(pt.x,pt.y);
}

bool CGuiPetPackDialog::OnTableEvent(CGuiTable* pTable,stPointI pt,UINT uMsg)
{
	if( uMsg != WM_LBUTTONDOWN )
	{
		return false;
	}
	if( GetGameGuiManager()->m_guiPetPackDlg )
	{
		// 计算目标格子	
		stPointI ptDestPixel = pt;
		ptDestPixel.x -= pTable->GetX();
		ptDestPixel.y -= pTable->GetY();
		stPointI ptDestGrid = pTable->GetGridPosByPixelPos(ptDestPixel);

		if( GetGameGuiManager()->m_guiPetPackDlg->bIsCellCanUse(ptDestGrid) )
		{
			return true;
		}
	}

	return false;
}