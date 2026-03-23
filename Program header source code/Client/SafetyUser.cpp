#include "public.h"
#include "SafetyUser.h"
#include "GameGuiManager.h"
#include "guiMiniMap.h"

////////////////////////////////////////////////////////////////
// 用static禁止extern这个变量,以便只能通过Singeleton获取
static   CSafetyUser  s_safetyuser;
CSafetyUser* CSafetyUser::Singleton = 0;
////////////////////////////////////////////////////////////
CSafetyUser::CSafetyUser()
{
	Singleton = this;

  m_guiOptionSafetyDlg = NULL;
  m_guiNumDlg = NULL;

   Reset();
}

CSafetyUser::~CSafetyUser()
{}


void CSafetyUser::Reset()
{
    m_state = (BYTE)-1; // -1 表关闭
    set_state(&m_safesetup,SAFE_SPLIT_OBJECT);
	set_state(&m_safesetup,SAFE_THIRD_PACK);
	set_state(&m_safesetup,SAFE_GOLD_STOCK);
}

void CSafetyUser::SetStateInfo(BYTE state,BYTE safesetup)
{
	m_state = state;
	m_safesetup = safesetup;

    if(GetGameGuiManager()->m_guiMiniMap)
		GetGameGuiManager()->m_guiMiniMap->UpdateSafetyUser();
}


void CSafetyUser::OnBtnClick()
{
    if(IsStateClose())
	{
           GetGameGuiManager()->AddMessageBox("确定打开密码保护吗?",eAction_OpenSafety,NULL,c_szMBOKCancel);
	}
	else if( IsStateSafetyOpen())
	{
			 OptionSafety();
	}
	else if (IsStateTmpClose())
	{
		GetGameGuiManager()->AddClientSystemMessage("你处于暂时未保护状态,要打开必须重新选角色进一次!");
	}
}

void CSafetyUser::OpenSafety()
{
#ifdef _DEBUG
	assert(IsStateClose());
#endif 
     if(IsStateSafetyOpen())
		 return;
	 if( IsStateTmpClose())
		 return;

	 AddNumDlg(eUsage_OpenSafety);

}

void CSafetyUser::OptionSafety()
{
#ifdef _DEBUG
	 assert(IsStateSafetyOpen());
#endif 

	 if( IsStateTmpClose())
		 return;

	 if( IsStateClose())
		 return;

	 AddOptionSafeDlg();
}

 

// 设置安全保护项
CGuiOptionSafetyDlg *  CSafetyUser::AddOptionSafeDlg()
{
	if(!m_guiOptionSafetyDlg)
	{
		CGuiOptionSafetyDlg * pDlg = new CGuiOptionSafetyDlg;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\optionsafetydlg.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiOptionSafetyDlg = pDlg;
	}
	m_guiOptionSafetyDlg->SetVisible(true);
	m_guiOptionSafetyDlg->SetEnabled(true);

	return m_guiOptionSafetyDlg;

}

CGuiNumdDlg * CSafetyUser::AddNumDlg(const eNumDlg_Usage & m_Usage)
{
	
		if(!m_guiNumDlg)
		{
			CGuiNumdDlg * pDlg = new CGuiNumdDlg(m_Usage);
			HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\numdlg.xml",pDlg);
			if(FAILED(hr)) 
			{
				delete pDlg;
				return NULL;
			}
			m_guiNumDlg = pDlg;
		}
		m_guiNumDlg->SetVisible(true);
		m_guiNumDlg->SetEnabled(true);

		return m_guiNumDlg;
}



////////////////////////////////////////////////////////////////////////////
bool ParseSafetyMessage(stNullUserCmd* pCmd,size_t size)
{
	FUNCTION_BEGIN;

	switch(pCmd->byParam)
	{
	case NOTIFY_SAFETY_PARA:  // when client login,the server send this message to notify the client 
		{
			stNotifySafetyUserCmd * __pCmd = (stNotifySafetyUserCmd *)pCmd;
			if( __pCmd)
			{
				CSafetyUser::Instance()->SetStateInfo(__pCmd->byState,__pCmd->safe_setup);
			}
		}
		break;
	}
	return false;
	FUNCTION_END;
}



////////////////////////////////////////////////////////////////////////////////////
#define ID_OPTION_SAFETYDLG_OK      10
#define ID_OPTION_SAFETYDLG_TEMPCLOSE  11

#define ID_OPTION_CHECK_1  1
#define ID_OPTION_CHECK_2  2
#define ID_OPTION_CHECK_3  3

CGuiOptionSafetyDlg::CGuiOptionSafetyDlg()
{
	m_bSmDialog = true;
}
CGuiOptionSafetyDlg::~CGuiOptionSafetyDlg()
{

}

void CGuiOptionSafetyDlg::OnCreate(void)
{
	CGuiDialog::OnCreate();

	CGuiCheckBox * pCheckBox;
	
	pCheckBox = GetCheckBox(ID_OPTION_CHECK_1);
	if(pCheckBox)
	{
			pCheckBox->SetChecked(CSafetyUser::Instance()->IsSafeDecomposeObject());
	}

	pCheckBox = GetCheckBox(ID_OPTION_CHECK_2);
	if(pCheckBox)
	{
		pCheckBox->SetChecked(CSafetyUser::Instance()->IsSafe23Pack());
	}

	pCheckBox = GetCheckBox(ID_OPTION_CHECK_3);
	if(pCheckBox)
	{
		pCheckBox->SetChecked(CSafetyUser::Instance()->IsSafeGoldStock());
	}

	
}

bool CGuiOptionSafetyDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		switch(nID)
		{
		case ID_OPTION_SAFETYDLG_OK:
			{
				CSafetyUser::Instance()->AddNumDlg(eUsage_OptionSafety);
			}
			break;
		case ID_OPTION_SAFETYDLG_TEMPCLOSE:
			{
              //CSafetyUser::Instance()->AddNumDlg(eUsage_TmpClose);
				Close(); //soke 修改直接关闭
			}
			break;
		}
		break;
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

void CGuiOptionSafetyDlg::OnClose(void)
{
	CSafetyUser::Instance()->m_guiOptionSafetyDlg = NULL;
}

HRESULT CGuiOptionSafetyDlg::OnRender(float fElapsedTime)
{
	return CGuiDialog::OnRender(fElapsedTime);
}

bool CGuiOptionSafetyDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{  
	return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);
}


void CGuiOptionSafetyDlg::FinishOptionSafety()
{
	
	stSetSafetyDetailUserCmd  cmd; 
	cmd.pwd = m_dwPass;
	cmd.safe_setup = 0;

    CGuiCheckBox * pCheckBox =  GetCheckBox(ID_OPTION_CHECK_1);
	if( pCheckBox)
	{
		if( pCheckBox->GetChecked())
		{
	          set_state(&cmd.safe_setup,SAFE_SPLIT_OBJECT);
		}
	}
	pCheckBox =  GetCheckBox(ID_OPTION_CHECK_2);
	if( pCheckBox)
	{
		if( pCheckBox->GetChecked())
		{
			set_state(&cmd.safe_setup,SAFE_THIRD_PACK);
		}
	}
	pCheckBox =  GetCheckBox(ID_OPTION_CHECK_3);
	if( pCheckBox)
	{
		if( pCheckBox->GetChecked())
		{
			set_state(&cmd.safe_setup,SAFE_GOLD_STOCK);
		}
	}
	SEND_USER_CMD(cmd);

	Close();
}




void CGuiOptionSafetyDlg::FinishTempClose()
{
	stSetTempUnSafetyUserCmd  cmd;
	cmd.pwd = m_dwPass;
	SEND_USER_CMD(cmd);

	Close();
}


////////////////////////////////////////////////////////////////////////////
#define ID_NUM_DL_OK   11
#define ID_NUM_DLG_OK  20
#define ID_NUM_DLG_CANCLE 21 

#define ID_NUM_NUM_1    1
#define ID_NUM_NUM_10   10

#define ID_STATIC_PASS 31


CGuiNumdDlg::CGuiNumdDlg(const  eNumDlg_Usage & usage )
{
	m_bSmDialog = true;
	m_Usage = usage;
}


void CGuiNumdDlg::OnClose(void)
{
	CSafetyUser::Instance()->m_guiNumDlg = NULL;
}


void CGuiNumdDlg::OnCreate()
{
	CGuiDialog::OnCreate();
	m_strNum ="";
}


HRESULT CGuiNumdDlg::OnRender(float fElapsedTime)
{
	if( !IsVisible()) return S_OK;

	CGuiDialog::OnRender(fElapsedTime);

	return S_OK;
}

bool CGuiNumdDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam  )
{
	return CGuiDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

bool CGuiNumdDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			if(nID >= ID_NUM_NUM_1 && nID <= ID_NUM_NUM_10)
			{
				int nNum = nID;
				if(nID == ID_NUM_NUM_10)
					nNum = 0;

				char szText[5];
				_snprintf(szText,5,"%d",nNum);
                szText[4] = 0;

				m_strNum += szText;

				CGuiStatic * pStatic = GetStatic(ID_STATIC_PASS);
				if( pStatic )
				{
					pStatic->SetText(m_strNum.c_str());
				}

				break;
			}
			switch(nID)
			{
			case ID_NUM_DL_OK:
				{
					if (m_strNum.size()>0)
					{
						int n = m_strNum.size()-1;
						m_strNum=m_strNum.substr(0,n);
						CGuiStatic * pStatic = GetStatic(ID_STATIC_PASS);
						if( pStatic )
						{
							pStatic->SetText(m_strNum.c_str());
						}
					}
				}
				break;
			case ID_NUM_DLG_OK:
				{
                     Close();
					 DWORD dwPass = atoi(m_strNum.c_str());
					 
					 if( m_Usage == eUsage_OptionSafety)
					 {
						 CSafetyUser::Instance()->m_guiOptionSafetyDlg->m_dwPass = dwPass;
						 CSafetyUser::Instance()->m_guiOptionSafetyDlg->FinishOptionSafety();
					 }
					 else if(m_Usage==eUsage_TmpClose)
					 {
					//	CSafetyUser::Instance()->m_guiOptionSafetyDlg->m_dwPass = dwPass;
					//	CSafetyUser::Instance()->m_guiOptionSafetyDlg->FinishTempClose();

						 //soke 修改成新的 这样在菜单里临时取消不会报错
						 stSetTempUnSafetyUserCmd  cmd;
	                     cmd.pwd = dwPass;
	                     SEND_USER_CMD(cmd);

					 }
					 else if( m_Usage == eUsage_OpenSafety)
					 {
						 stSetSafetyUserCmd cmd;
						 cmd.pwd = dwPass;
						 cmd.state = 1;
						 SEND_USER_CMD(cmd);
					 }					
				}
				break;
			case ID_NUM_DLG_CANCLE:
				{
					Close();
				}
				break;
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}



