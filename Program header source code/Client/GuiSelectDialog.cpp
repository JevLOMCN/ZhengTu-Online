#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include ".\GuiSelectDialog.h"

CGuiSelectDialog::CGuiSelectDialog(void)
{
	FUNCTION_BEGIN;
	m_pList = NULL;
	m_bSmDialog = true;
	FUNCTION_END;
}

CGuiSelectDialog::~CGuiSelectDialog(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}


void CGuiSelectDialog::OnCreate(void)
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();

	m_pList = GetListBox(2);
	FUNCTION_END;
}

bool CGuiSelectDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case 3: return OnIDOk();
		case 4: return OnIDCancel();
		}
	}
	if( nEvent == EVENT_LISTBOX_ITEM_DBLCLK )
	{
		if( nID == 2)
		{
			void * pData = GetCurSelectItemData();
			if( NULL != pData )
			{
				DWORD dwThisId = (DWORD)(pData);
				SetFrameAllocator fa;
				stScrollMapPropertyUserCmd * p = (stScrollMapPropertyUserCmd*)FrameAllocator::alloc(sizeof(stScrollMapPropertyUserCmd) + MAX_NAMESIZE);
				constructInPlace(p);
				p->qwThisID = dwThisId;
				p->size = 1;
				strncpy(p->mapname[0].strMapName,GetCurItemText(),MAX_NAMESIZE);
				Client_SendCommand( p, sizeof( stScrollMapPropertyUserCmd ) + MAX_NAMESIZE,false );
				Close();
			}		
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiSelectDialog::OnClose(void)
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->m_guiSelectDialog = NULL;
	FUNCTION_END;
}

HRESULT CGuiSelectDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	return hr;
	FUNCTION_END;
}

void CGuiSelectDialog::OnSetCloseLocation()
{
}

bool CGuiSelectDialog::OnIDOk()
{
	FUNCTION_BEGIN;
	if( m_pList )
	{
		void * pData = GetCurSelectItemData();
		if( NULL != pData )
		{
			DWORD dwThisId = (DWORD)(pData);
			SetFrameAllocator fa;
			stScrollMapPropertyUserCmd * p = (stScrollMapPropertyUserCmd*)FrameAllocator::alloc(sizeof(stScrollMapPropertyUserCmd) + MAX_NAMESIZE);
			constructInPlace(p);
			p->qwThisID = dwThisId;
			p->size = 1;
			strncpy(p->mapname[0].strMapName,GetCurItemText(),MAX_NAMESIZE);
			Client_SendCommand( p, sizeof( stScrollMapPropertyUserCmd ) + MAX_NAMESIZE,false );
		}		
	}
	Close();
	return true;
	FUNCTION_END;
}

bool CGuiSelectDialog::OnIDCancel()
{
	FUNCTION_BEGIN;
	Close();
	return true;
	FUNCTION_END;
}

void CGuiSelectDialog::SetText(const char* pszText)
{
	FUNCTION_BEGIN;
	_snprintf(m_strText,MAX_PATH,pszText);
	m_strText[MAX_PATH - 1] = 0;
	FUNCTION_END;
}

void CGuiSelectDialog::AddListItem(const char *szText, void *pData  ,DWORD color ,IBitmaps* icon)
{
	FUNCTION_BEGIN;
	if( m_pList )
	{
		m_pList->AddItem(szText,pData,color);
	}
	FUNCTION_END;
}

int CGuiSelectDialog::GetCurSelectItem()
{
	FUNCTION_BEGIN;
	if( m_pList )
	{
		return  m_pList->GetCurItem();
	}
	return -1;
	FUNCTION_END;
}

const char* CGuiSelectDialog::GetCurItemText()
{
	FUNCTION_BEGIN;
	if( m_pList )
	{
		int curIndex = GetCurSelectItem();
		if( curIndex >= 0 && curIndex < m_pList->GetCount() )
		{
			return  m_pList->GetItemText(curIndex);
		}
	}
	return NULL;
	FUNCTION_END;
}

void* CGuiSelectDialog::GetCurSelectItemData()
{
	FUNCTION_BEGIN;
	if( m_pList )
	{
		int curIndex = GetCurSelectItem();
		if( curIndex >= 0 && curIndex < m_pList->GetCount() )
		{
			return  m_pList->GetItemData(curIndex);
		}		
	}
	return NULL;
	FUNCTION_END;
}