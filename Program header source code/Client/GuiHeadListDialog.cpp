
#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GuiHeadListDialog.h"
#include "./GameScene.h"
#include <vector>
#include "RoleItem.h"
#include "table.h"
#include "GameScene.h"
#include "MainCharacter.h"
///////////////////////////////////////////////////////////////////////////////////////////////////

#define HEAD_LIST_BOX     1        // 头像列表
#define IMAGE_HEAD        2        // 头像演示图片
#define STATIC_MONEY      3        // 银子说明 
#define BUTTON_OK         4        // 确定
#define BUTTON_CANCEL     5        // 取消

#define _SetText(id,txt) CGuiStatic *p##id = GetStatic(id);if(p##id) p##id->SetText(txt); 

///////////////////////////////////////////////////////////////////////////////////////////////////

CGuiHeadListDialog::CGuiHeadListDialog()
{
	FUNCTION_BEGIN;	

	m_pHeadList = NULL;    

	m_pDemoHead = NULL;

	m_pMainChar = NULL;

	FUNCTION_END;
}

CGuiHeadListDialog::~CGuiHeadListDialog(void)
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}


void CGuiHeadListDialog::OnCreate(void)
{
	FUNCTION_BEGIN;

	m_ptIcon.x = 36;
	m_ptIcon.y = 110;

	m_pHeadList = GetListBox(HEAD_LIST_BOX);    
	m_pHeadList->RemoveAllItems();
	m_pMainChar = GetScene()->GetMainCharacter();

	m_pDemoHead = GetImage(IMAGE_HEAD);

	enumSex CharacterSex = m_pMainChar->GetSex();
	DWORD curFace = (DWORD)m_pMainChar->GetFace();
        if( g_bSinglePlayer)
	{
		for(tTableHeadList::iterator it = g_tableHeadList.begin();
			it != g_tableHeadList.end() ;++it)
		{
			m_pHeadList->AddItem( it->strName,&(it->dwID) );
		 
		}
	}
	else
	{

		if(CharacterSex == eSex_Male)//男性
		{
			for(tTableHeadList::iterator it = g_tableHeadList.begin();
				it != g_tableHeadList.end() ;++it)
			{
				if(it->dwSex == 1 && it->dwID != curFace)
				{
					TRACE("AddItem[%s]\n",it->strName);
					m_pHeadList->AddItem( it->strName,&(it->dwID) );
				}
			}
		}
		else if(CharacterSex == eSex_Female)//女的
		{
			for(tTableHeadList::iterator it = g_tableHeadList.begin();
				it != g_tableHeadList.end() ;++it)
			{
				if( it->dwSex == 2 && it->dwID != curFace )
				{
					TRACE("AddItem[%s]\n",it->strName);
					m_pHeadList->AddItem( it->strName,&(it->dwID) );
				}
			}
		}
	}


	CGuiDialog::OnCreate();
	FUNCTION_END;
}


void CGuiHeadListDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiHeadListDlg = NULL;
	GetGameGuiManager()->OnNpcActionEnd();

	FUNCTION_END;
}


/**
 * \brief "确定"按钮响应消息
 * 
 * 计算角色的银子数目,如果钱够则发出改变头像命令，否则给出钱不够的提示
 * 
 * \return true if money enough else false
 */
bool CGuiHeadListDialog::OnOK()
{
	FUNCTION_BEGIN;

	if(m_pMainChar)
	{
		DWORD dwNeedMoney = 0;

		//DWORD dwStyle = m_pMainChar->GetHairType();
		DWORD dwFace = m_pMainChar->GetFace();
		DWORD dwOwnMoney = m_pMainChar->GetMoney();
		DWORD dwID = -1;

		int nCurIndex = m_pHeadList->GetCurItem();
		if( nCurIndex >= 0 )
		{
			DWORD *pID = (DWORD *)m_pHeadList->GetItemData(nCurIndex);
			if(pID)	
			{

			        DWORD dwID = *pID;
				HeadList_t* p = GetHeadInfo(dwID);
				dwFace = p->dwHeadID;
				if(p) dwNeedMoney += p->dwMoney;
			}
		}

		if(dwNeedMoney == 0)
		{
			Close();
			return false;
		}
		if(dwOwnMoney < dwNeedMoney)
		{
			//sorry your money is not enough
			GetGameGuiManager()->AddClientSystemMessage("不要骗我了，你根本没银子整容！");
			return false;
		}
		else
		{
			stFacePropertyUserCmd cmd;
			cmd.dwFace = dwFace;
			SEND_USER_CMD(cmd);
			//stHairPropertyUserCmd hairCmd;
			//hairCmd.byHairType = dwStyle;
			//hairCmd.dwHairColor = dwColor;
			//SEND_USER_CMD(hairCmd);
			Close();
			return true;
		}
	}
	return false;

	FUNCTION_END;
}

bool CGuiHeadListDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	char szText[MAX_PATH];
	if(nEvent == EVENT_LISTBOX_SELECTION)
	{
		switch(nID)
		{
		case HEAD_LIST_BOX:
			{
				static int oldIndex = -1;
				int index = m_pHeadList->GetCurItem();
				if(index < 0 ) break;
				if(index == oldIndex)
				{
					m_pHeadList->SelectItem(-1,false);
					_SetText(STATIC_MONEY,"");
					oldIndex = -1;
					DWORD curFace = (DWORD)m_pMainChar->GetFace();
					m_pDemoHead->SetVisible(false);
					break;
				}
				oldIndex = index;

				DWORD *pID = (DWORD *)m_pHeadList->GetItemData(index);
				if(pID)	
				{
					HeadList_t* p = GetHeadInfo(*pID);
					if(p)
					{
						sprintf(szText,"%s",Money2String(p->dwMoney));
						_SetText(STATIC_MONEY,szText);
						ChangeFace(p->dwHeadID);
					}
				}
			}
			break;
		}
	}
	else if(nEvent == EVENT_BUTTON_CLICKED )
	{
		if( nID == BUTTON_OK ) 
		{
			return OnOK();
		}
		else if( nID == BUTTON_CANCEL )
		{
			Close();
		}
	}
	
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiHeadListDialog::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());
		m_pBtnClose->SetLocation(pt.x,pt.y + 24);
	}

	FUNCTION_END;
}

void CGuiHeadListDialog::ChangeFace(DWORD dwHeadID)
{
	FUNCTION_BEGIN;
      
	const int c_group = 33;
	IBitmapList * pBmpList = GetDevice()->FindResource("data\\Interfaces.gl");
	size_t framecount = pBmpList->GetFrameCount(c_group);
     
	if( pBmpList->IsExist(c_group,dwHeadID))
	{
		const stResourceLocation rl("data\\Interfaces.gl",c_group,dwHeadID);
		m_pDemoHead->SetImage(rl);
		m_pDemoHead->SetVisible(true);
	}
	else
		m_pDemoHead->SetVisible(false);
 	 
	FUNCTION_END;
}
