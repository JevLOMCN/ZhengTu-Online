/**
*\file		GuiBallot.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    投票对话框
*
* 
*
*/


#include "public.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiWorldConn.h"
#include ".\guiballot.h"
#include "./GuiNpcDialog.h"
/**
 * \brief 投票构造函数
 * 
 * 投票构造函数
 * 
 * \param eState : 投票类别
 * \return 无
 */
CGuiBallot::CGuiBallot(byte eState)
{
	FUNCTION_BEGIN;
	m_eState = eState;
	m_nSelect = -1;
	m_bIsBallot = true;
	bMaster = false;
	FUNCTION_END;
}


/**
 * \brief 关闭对话框
 * 
 * 关闭对话框
 * 
 * \return 无
 */
void CGuiBallot::OnClose(void)
{
	FUNCTION_BEGIN;
	
	GetGameGuiManager()->m_guiBallot = NULL;
	GetGameGuiManager()->OnNpcActionEnd();
	FUNCTION_END;
}

/**
 * \brief 对话框创建
 * 
 * 对话框创建
 * 
 * \return 无
 */
void CGuiBallot::OnCreate()
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();

	m_pEditBox = GetMLEditBox(8);
	m_pEditBox->SetEnabled(false);
	
	m_pListBox = GetListBox(7);

	GetButton(3)->SetEnabled(false);
	GetButton(4)->SetEnabled(false);

	m_BallotList.clear();

	switch(m_eState) {
	case eAction_Tong:
		{
			stVoteListUnionCmd cmd;
			SEND_USER_CMD(cmd);
		}
		break;

	case eAction_Sept:
		{
			stVoteListSeptCmd cmd;
			SEND_USER_CMD(cmd);
		}
		break;
	}
	

	FUNCTION_END;
}

void CGuiBallot::OnSetCloseLocation()
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

/**
 * \brief 刷新投票列表
 * 
 * 刷新投票列表
 * 
 * \param name : 投票对象的负责人
 * \return 无
 */
void CGuiBallot::RefreshListBox(char* name)
{
	FUNCTION_BEGIN;
	
	bMaster = false;
	m_nSelect = -1;
	m_pListBox->RemoveAllItems();
	
	switch(m_eState) {
	case eAction_Tong:
		{
			if (GetGameGuiManager()->m_guiWorldConn)
			{
				if (strlen(GetGameGuiManager()->m_guiWorldConn->m_TongInfo.unionName)>0)
					m_bIsBallot = false;
				else
					m_bIsBallot = true;

			}

			m_pListBox->AddItem("",NULL,FONTCOLOR);
			m_pListBox->SetItemText(0,0,"帮会名",12);
			m_pListBox->SetItemText(0,1,"帮主",12);
			m_pListBox->SetItemText(0,2,"签名数",12);
		}
		break;
	case eAction_Sept:
		{
			if (GetGameGuiManager()->m_guiWorldConn)
			{
				if (strlen(GetGameGuiManager()->m_guiWorldConn->m_SeptInfo.septName)>0)
					m_bIsBallot = false;
				else
					m_bIsBallot = true;

			}

			m_pListBox->AddItem("",NULL,FONTCOLOR);
			m_pListBox->SetItemText(0,0,"家族名",12);
			m_pListBox->SetItemText(0,1,"族长",12);
			m_pListBox->SetItemText(0,2,"签名数",12);
		}
		break;
	}
	
	
	for (int i=0;i<m_BallotList.size();i++)
	{
		m_pListBox->AddItem("",&(m_BallotList[i]),FONTCOLOR);
		
		char szTxt[8]={0};

		m_pListBox->SetItemText(m_pListBox->GetItemCount()-1,0,m_BallotList[i].szBallot,12);
		m_pListBox->SetItemText(m_pListBox->GetItemCount()-1,1,m_BallotList[i].szName,12);
		m_pListBox->SetItemText(m_pListBox->GetItemCount()-1,2,
			_itoa(m_BallotList[i].nBallot,szTxt,10),12);

		if (strncmp(m_BallotList[i].szBallot,name,sizeof(m_BallotList[i].szBallot))==0)
		{
			m_nSelect = m_pListBox->GetItemCount()-1;

			if (strncmp(GetScene()->GetMainCharacter()->GetName(),m_BallotList[i].szName,MAX_NAMESIZE)==0)
				bMaster = true;
		}
	}
	
	if (m_nSelect!=-1)
	{
		GetButton(3)->SetEnabled(false);
		/*if (bMaster)
			GetButton(4)->SetEnabled(false);
		else*/
			GetButton(4)->SetEnabled(true);

		m_pListBox->SetCurItem(m_nSelect);
	}
	else
	{
		GetButton(3)->SetEnabled(false);
		GetButton(4)->SetEnabled(false);
		
		m_pEditBox->SetText("");
		//m_pListBox->SetCurItem(0);
	}

	FUNCTION_END;
}

/**
 * \brief 投票或弃票处理
 * 
 * 投票或弃票处理
 * 
 * \param name : 投票对象名字
 * \param nSuccess : 是否成功
 * \return 无
 */
void CGuiBallot::Ballot(char* name,int nSuccess)
{
	for (int i=0;i<m_BallotList.size();i++)
	{
		if (strncmp(m_BallotList[i].szBallot,name,sizeof(m_BallotList[i].szBallot))==0)
		{
			switch(nSuccess) {
			case 2:
				m_BallotList[i].nBallot--;
				RefreshListBox("");
				return;
			case 1:
				m_BallotList[i].nBallot++;
				RefreshListBox(name);
				return;
			}
		}
	}
}

/**
 * \brief 投票消息处理
 * 
 * 投票消息处理
 * 
 * \param nEvent : 消息类型
 * \param nID : 消息控件ID
 * \param pControl : 消息控件指针
 * \return 消息处理结果
 */
bool CGuiBallot::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
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
					stBallot* pBallot = (stBallot*)m_pListBox->GetItemData(m_pListBox->GetCurItem());
					if (pBallot)
					{
						switch(m_eState) {
						case eAction_Tong:
							{
								pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("是否签名支持 %s 帮会？",m_pListBox->GetItemText(m_pListBox->GetCurItem())),
									eAction_TongBallot_OK,(void*)pBallot,c_szMBOKCancel);

							}
							break;
						case eAction_Sept:
							{
								pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("是否签名支持 %s 家族的票？",m_pListBox->GetItemText(m_pListBox->GetCurItem())),
									eAction_SeptBallot_OK,(void*)pBallot,c_szMBOKCancel);

							}
							break;
						}
						if (GetGameGuiManager()->m_guiNpcDialog)
							GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = (void*) pDlg;
					}
				}
				break;
			case 4:
				{
					stBallot* pBallot = (stBallot*)m_pListBox->GetItemData(m_pListBox->GetCurItem());
					if (pBallot)
					{
						switch(m_eState) {
						case eAction_Tong:
							{
								if (bMaster)
									pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("是否要解散 %s 帮会？",m_pListBox->GetItemText(m_pListBox->GetCurItem())),
										eAction_TongBallot_CANCEL,(void*)pBallot,c_szMBOKCancel);
								else
									pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("是否要取消签名放弃 %s 帮会？",m_pListBox->GetItemText(m_pListBox->GetCurItem())),
										eAction_TongBallot_CANCEL,(void*)pBallot,c_szMBOKCancel);

							}
							break;
						case eAction_Sept:
							{
								if (bMaster)
									pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("是否要解散 %s 家族？",m_pListBox->GetItemText(m_pListBox->GetCurItem())),
										eAction_SeptBallot_CANCEL,(void*)pBallot,c_szMBOKCancel);
								else
									pDlg =(CGuiDialog*) GetGameGuiManager()->AddMessageBox(avar("是否要取消签名放弃 %s 家族？",m_pListBox->GetItemText(m_pListBox->GetCurItem())),
										eAction_SeptBallot_CANCEL,(void*)pBallot,c_szMBOKCancel);

							}
							break;
						}
						if (GetGameGuiManager()->m_guiNpcDialog)
							GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = (void*) pDlg;
					}
				}
				break;
			}
		}
		break;

	case EVENT_LISTBOX_SELECTION:
		{
			CGuiListBox* pListBox = (CGuiListBox*) pControl;
			if (m_nSelect!=-1)
			{
				if (m_nSelect!=pListBox->GetCurItem())
					pListBox->SetCurItem(m_nSelect);
			}
			else
			{
				if (pListBox->GetItemData(pListBox->GetCurItem())!=NULL)
				{
					stBallot* pBallot = (stBallot*)pListBox->GetItemData(pListBox->GetCurItem());

					m_pEditBox->SetText(pBallot->szContext);
					
					if (m_bIsBallot)
						GetButton(3)->SetEnabled(true);
					else
						GetButton(3)->SetEnabled(false);
				}
				else
				{
					GetButton(3)->SetEnabled(false);
				}
			}
		}
		break;

	case EVENT_LISTBOX_MOVESELECTION:
		{
			CGuiListBox* pListBox = (CGuiListBox*) pControl;

			if (pListBox->GetMoveSel()!=-1)
			{
				if (pListBox->GetItemData(pListBox->GetMoveSel())!=NULL)
				{
					stBallot* pBallot = (stBallot*)pListBox->GetItemData(pListBox->GetMoveSel());

					m_pEditBox->SetText(pBallot->szContext);
				}
				else
					m_pEditBox->SetText("");
			}
			else
			{
				if (pListBox->GetItemData(pListBox->GetCurItem())!=NULL)
				{
					stBallot* pBallot = (stBallot*)pListBox->GetItemData(pListBox->GetCurItem());

					m_pEditBox->SetText(pBallot->szContext);
				}
				else
					m_pEditBox->SetText("");
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}