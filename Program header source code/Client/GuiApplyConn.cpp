
#include "public.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiWorldConn.h"
#include ".\guiapplyconn.h"

CGuiApplyConn::CGuiApplyConn(void)
{
	m_bSmDialog = true;

	//m_bCloseIsHide = true;
}


void CGuiApplyConn::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiApplyConn = NULL;
	GetGameGuiManager()->OnNpcActionEnd();

	m_SeptList.clear();
	m_TongList.clear();
	FUNCTION_END;
}

/**
* \brief 对话框创建
* 
* 对话框创建
* 
* \return 无
*/
void CGuiApplyConn::OnCreate()
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();

	m_pListBox = GetListBox(7);

	GetButton(5)->SetButtonGroup(1);
	GetButton(6)->SetButtonGroup(1);
	//m_BallotList.clear();

	
	OnGuiEvent(EVENT_BUTTON_CLICKED,5,NULL);


	stListSeptCmd  cmd1;
	SEND_USER_CMD(cmd1);


	stListUnionCmd  cmd2;
	SEND_USER_CMD(cmd2);

	FUNCTION_END;
}

void CGuiApplyConn::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt( m_pBtnClose->GetLocation() );

		m_pBtnClose->SetLocation(pt.x,pt.y + 24);
	}

	FUNCTION_END;
}


void CGuiApplyConn::RefreshListBox()
{
	m_pListBox->RemoveAllItems();
	


	if (GetButton(5)->GetChecked()==true)
	{
		for (int i=0;i<m_SeptList.size();i++)
		{
			m_pListBox->AddItem("",&(m_SeptList[i]),FONTCOLOR);

			m_pListBox->SetItemText(m_pListBox->GetItemCount()-1,0,m_SeptList[i].szApplyConn,20);
			m_pListBox->SetItemText(m_pListBox->GetItemCount()-1,1,m_SeptList[i].szName);
		}
	}
	else
	{
		for (int i=0;i<m_TongList.size();i++)
		{
			m_pListBox->AddItem("",&(m_TongList[i]),FONTCOLOR);

			m_pListBox->SetItemText(m_pListBox->GetItemCount()-1,0,m_TongList[i].szApplyConn,20);
			m_pListBox->SetItemText(m_pListBox->GetItemCount()-1,1,m_TongList[i].szName);
		}
	}

	
}

bool CGuiApplyConn::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	CGuiDialog* pDlg = NULL;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 3:
				{
					stApplyConn* pApplyConn = (stApplyConn*)m_pListBox->GetItemData(m_pListBox->GetCurItem());
					if (pApplyConn==NULL)
					{
						GameMessageBox("请先选中你要加入的组织！");
						break;
					}

					if (GetButton(5)->GetChecked()==true)
					{
						if (strlen(GetGameGuiManager()->m_guiWorldConn->m_SeptInfo.septName)>0)
						{
							GameMessageBox("你已有家族，无法申请加入其他的家族！");
							break;
						}

						stRequestJoinSeptCmd  cmd;
						strncpy(cmd.name,pApplyConn->szName,sizeof(cmd.name));
						cmd.status = REQUEST_JOIN_OK;

						SEND_USER_CMD(cmd);

					}
					else
					{
						if (strlen(GetGameGuiManager()->m_guiWorldConn->m_TongInfo.unionName)>0)
						{
							GameMessageBox("你已有帮会，无法申请加入其他的帮会！");
							break;
						}

						stRequestJoinUnionCmd  cmd;
						strncpy(cmd.name,pApplyConn->szName,sizeof(cmd.name));
						cmd.status = REQUEST_JOIN_OK;

						SEND_USER_CMD(cmd);
					}
				}
			case 4:
				OnGuiEvent(EVENT_BUTTON_CLICKED,-1,NULL);
				break;
			case 5:
				GetButton(5)->SetChecked(true);
				SetBackImage(stResourceLocation("data\\interfaces.gl",54,50));
				RefreshListBox();
				break;
			case 6:
				GetButton(6)->SetChecked(true);
				SetBackImage(stResourceLocation("data\\interfaces.gl",54,51));
				RefreshListBox();
				break;
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

