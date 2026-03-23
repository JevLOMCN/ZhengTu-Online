
#include "public.h"
#include "./Game.h"
//#include "../gui/include/guiTypes.h"
//#include "./GameGuiManager.h"
#include ".\guipassworddlg.h"

 

/************************************* begin define CGuiPasswordDlg *************************************/

static const int	iPasswordDlgIdOk = 5;
static const int	iPasswordDlgIdCancel = 4;

CGuiPasswordDlg::CGuiPasswordDlg(enumPasswordType type,const char* pszLabel,void * pData)
: m_type(type)
, m_pData(pData)
{
	FUNCTION_BEGIN;

	m_Title = pszLabel;
	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiPasswordDlg::~CGuiPasswordDlg(void)
{
}


void CGuiPasswordDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiPasswordDlg = NULL;

	FUNCTION_END;
}

bool CGuiPasswordDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID)
			{
			case iPasswordDlgIdOk:
				return OnIDOK();
				break;
			case iPasswordDlgIdCancel:
				return OnIDCancel();
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}


HRESULT CGuiPasswordDlg::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	return hr;

	FUNCTION_END;
}

bool CGuiPasswordDlg::OnIDOK()
{
	switch(m_type)
	{
	case ePassword_SavingBox:
		{
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

bool CGuiPasswordDlg::OnIDCancel()
{
	return true;
}
/************************************* end define CGuiPasswordDlg *************************************/


/********************************* begin define CGuiModifyPasswordDlg *********************************/
static const int	iModifyPasswordDlgUpIdOk = 5;
static const int	iModifyPasswordDlgUpIdCancel = 4;
static const int	iModifyPasswordDlgBottomIdOk = 7;
static const int	iModifyPasswordDlgBottomIdCancel = 6;

CGuiModifyPasswordDlg::CGuiModifyPasswordDlg(enumPasswordType type,const char* pszLabel1,const char* pszLabel2,void * pData)
: m_type(type)
, m_pData(pData)
{
	FUNCTION_BEGIN;

	m_Title1 = pszLabel1;
	m_Title2 = pszLabel2;
	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiModifyPasswordDlg::~CGuiModifyPasswordDlg(void)
{
}


void CGuiModifyPasswordDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiModifyPasswordDlg = NULL;

	FUNCTION_END;
}

bool CGuiModifyPasswordDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID)
			{
			case iModifyPasswordDlgUpIdOk:
				return OnUpIDOK();
				break;
			case iModifyPasswordDlgBottomIdOk:
				return OnBottomIDOK();
				break;
			case iModifyPasswordDlgUpIdCancel:
				return OnUpIDCancel();
				break;
			case iModifyPasswordDlgBottomIdCancel:
				return OnBottomIDCancel();
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}


HRESULT CGuiModifyPasswordDlg::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	return hr;

	FUNCTION_END;
}

bool CGuiModifyPasswordDlg::OnUpIDOK()
{
	Close();
	return true;
}

bool CGuiModifyPasswordDlg::OnUpIDCancel()
{
	Close();
	return true;
}

bool CGuiModifyPasswordDlg::OnBottomIDOK()
{
	Close();
	return true;
}

bool CGuiModifyPasswordDlg::OnBottomIDCancel()
{
	Close();
	return true;
}

/********************************* end define CGuiModifyPasswordDlg *********************************/