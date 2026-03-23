#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include ".\GuiKingSelectDialog.h"

namespace util
{
	std::vector<std::string> string_split(const std::string& str);

	POINT parsePos(const std::string& strPos);
}

namespace util
{
	std::vector<std::string> string_split(const std::string str, const std::string delim)
	{
		std::vector<std::string> ret;

		size_t last = 0;
		size_t index = str.find_first_of(delim, last);
		while (index != std::string::npos)
		{
			ret.push_back(str.substr(last, index - last));
			last = index + 1;
			index = str.find_first_of(delim, last);
		}
		if (index - last > 0)
		{
			ret.push_back(str.substr(last, index - last));
		}

		return ret;
	}

	POINT parsePos(const std::string& strPos)
	{
		POINT pt = {0,0};

		std::vector<std::string> vPos = string_split(strPos, " ,");
		if(vPos.size()==2)
		{
			pt.x = atoi(vPos[0].c_str());
			pt.y = atoi(vPos[1].c_str());
		}

		return pt;
	}
}

std::vector<std::string>	CGuiKingSelectDialog::m_vForeigner;

CGuiKingSelectDialog::CGuiKingSelectDialog(void)
{
	FUNCTION_BEGIN;
	mx_pList = NULL;
	m_bSmDialog = true;
	FUNCTION_END;
}

CGuiKingSelectDialog::~CGuiKingSelectDialog(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}


void CGuiKingSelectDialog::OnCreate(void)
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();

	mx_pList = GetListBox(8);
	m_editboxPos	= GetEditBox(5);
	m_listboxForeigner	= GetListBox(101);

	for(int i = 0; i < m_vForeigner.size(); i ++)
		AddForeigner(m_vForeigner[i]);

	FUNCTION_END;
}

bool CGuiKingSelectDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
			case 6: return OnIDOk();
			case 7: return OnIDCancel();
			
		}
	}
	if( nEvent == EVENT_LISTBOX_ITEM_DBLCLK )
	{
		if( nID == 8)
		{
			return OnIDOk();
/*			void * pData = GetCurSelectItemData();
			if( NULL != pData )
			{
				DWORD dwThisId = (DWORD)(pData);
				SetFrameAllocator fa;
				stScroll2MapPropertyUserCmd * p = (stScroll2MapPropertyUserCmd*)FrameAllocator::alloc(sizeof(stScroll2MapPropertyUserCmd) + MAX_NAMESIZE);
				constructInPlace(p);
				p->qwThisID = dwThisId;
				p->size = 1;
				strncpy(p->mapname[0].strMapName,GetCurItemText(),MAX_NAMESIZE);
				Client_SendCommand( p, sizeof( stScroll2MapPropertyUserCmd ) + MAX_NAMESIZE,false );
				Close();
			}	*/	
		}
		if(nID == 101)
		{
			int index = m_listboxForeigner->GetCurItem();

			if(index >= 0 && index < m_listboxForeigner->GetCount())
			{
				std::string str = m_listboxForeigner->GetItemText(index);

				std::vector<std::string> vStr = util::string_split(str, " ,");
				if(vStr.size() == 3)
				{
					//stGotoForeignerPosCmd * p = (stGotoForeignerPosCmd*)FrameAllocator::alloc(sizeof(stGotoForeignerPosCmd));
					//constructInPlace(p);

					//strncpy(p->szMapName, vStr[0].c_str(), MAX_NAMESIZE);
					//p->xPos = atoi(vStr[1].c_str());
					//p->yPos = atoi(vStr[2].c_str());				

					//Client_SendCommand( p, sizeof( stGotoForeignerPosCmd ), false );
					SetFrameAllocator fa;
					stScroll2MapPropertyUserCmd * p = (stScroll2MapPropertyUserCmd*)FrameAllocator::alloc(sizeof(stScroll2MapPropertyUserCmd) + MAX_NAMESIZE);
					constructInPlace(p);
					p->qwThisID = m_dwThisId;
					p->size = 1;

					strncpy(p->mapname[0].strMapName, vStr[0].c_str(), MAX_NAMESIZE);
					p->xPos = atoi(vStr[1].c_str());
					p->yPos = atoi(vStr[2].c_str());				

					Client_SendCommand( p, sizeof( stScroll2MapPropertyUserCmd ) + MAX_NAMESIZE,false );

				}

				Close();
			}
			
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiKingSelectDialog::OnClose(void)
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->m_guiKingSelectDialog = NULL;
	FUNCTION_END;
}

HRESULT CGuiKingSelectDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	return hr;
	FUNCTION_END;
}

void CGuiKingSelectDialog::OnSetCloseLocation()
{
}

bool CGuiKingSelectDialog::OnIDOk()
{
	FUNCTION_BEGIN;
	if( mx_pList )
	{
		void * pData = GetCurSelectItemData();
		if( NULL != pData )
		{
			DWORD dwThisId = (DWORD)(pData);
			SetFrameAllocator fa;
			stScroll2MapPropertyUserCmd * p = (stScroll2MapPropertyUserCmd*)FrameAllocator::alloc(sizeof(stScroll2MapPropertyUserCmd) + MAX_NAMESIZE);
			constructInPlace(p);
			p->qwThisID = dwThisId;
			p->size = 1;

			POINT pt = util::parsePos(m_editboxPos->GetText());
			p->xPos = pt.x;
			p->yPos = pt.y;

			strncpy(p->mapname[0].strMapName,GetCurItemText(),MAX_NAMESIZE);
			Client_SendCommand( p, sizeof( stScroll2MapPropertyUserCmd ) + MAX_NAMESIZE,false );
		}		
	}
	Close();
	return true;
	FUNCTION_END;
}

bool CGuiKingSelectDialog::OnIDCancel()
{
	FUNCTION_BEGIN;
	Close();
	return true;
	FUNCTION_END;
}

void CGuiKingSelectDialog::SetText(const char* pszText)
{
	FUNCTION_BEGIN;
	_snprintf(m_strText,MAX_PATH,pszText);
	m_strText[MAX_PATH - 1] = 0;
	FUNCTION_END;
}

void CGuiKingSelectDialog::ClearList()
{
	mx_pList->RemoveAllItems();
}
 
void CGuiKingSelectDialog::AddListItem(const char *szText, void *pData  ,DWORD color ,IBitmaps* icon)
{
	FUNCTION_BEGIN;
	if( mx_pList )
	{
		mx_pList->AddItem(szText,pData,color);
	}
	FUNCTION_END;
}

void CGuiKingSelectDialog::ClearListForeigner()
{
	m_listboxForeigner->RemoveAllItems();
}

void CGuiKingSelectDialog::AddForeigner(const std::string& text)
{
	FUNCTION_BEGIN;
	if( m_listboxForeigner )
	{
		m_listboxForeigner->AddItem(text.c_str(), NULL);
	}
	FUNCTION_END;
}


int CGuiKingSelectDialog::GetCurSelectItem()
{
	FUNCTION_BEGIN;
	if( mx_pList )
	{
		return  mx_pList->GetCurItem();
	}
	return -1;
	FUNCTION_END;
}

const char* CGuiKingSelectDialog::GetCurItemText()
{
	FUNCTION_BEGIN;
	if( mx_pList )
	{
		int curIndex = GetCurSelectItem();
		if( curIndex >= 0 && curIndex < mx_pList->GetCount() )
		{
			return  mx_pList->GetItemText(curIndex);
		}
	}
	return NULL;
	FUNCTION_END;
}

void* CGuiKingSelectDialog::GetCurSelectItemData()
{
	FUNCTION_BEGIN;
	if( mx_pList )
	{
		int curIndex = GetCurSelectItem();
		if( curIndex >= 0 && curIndex < mx_pList->GetCount() )
		{
			return  mx_pList->GetItemData(curIndex);
		}		
	}
	return NULL;
	FUNCTION_END;
}