/**
 *\file		GuiHairStyleDialog.cpp
 *\version	$ID$
 *\author	$$$04公司
 *			$$$04公司@###.com
 *\date		2005-07-06 11:19:00
 *\brief	换发型、发色对话框
 *
 * 跟发型师npc对话时显示的对话框，用于更换角色的发型和发色
 *
 */

#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include ".\GuiHairStyleDialog.h"
#include "./GameScene.h"
#include <vector>
#include "RoleItem.h"
#include "table.h"
#include "GameScene.h"
#include "MainCharacter.h"
///发型列表提示
static const int cHairStyleTextid = 2;

///发型列表id
static const int cChangeHairStyleListCtrlID = 3;

///发色列表提示
static const int cHairColorTextid = 4;

///发色列表id
static const int cChangeHairColorListCtrlID = 5;

///换发型的费用显示静态框id
static const int cMoneyOfChangeHairStyleID = 6;

///换发色的费用显示静态框
static const int cMoneyOfChangeHairColorID = 7;

#define _SetText(id,txt) CGuiStatic *p##id = GetStatic(id);if(p##id) p##id->SetText(txt);

///确定按钮id
static const int cIDOK = 20;

///取消按钮id
static const int cIDCancel = 21;



/**
 * \brief 构造函数
 * 
 */
CGuiHairStyleDialog::CGuiHairStyleDialog(void)
{
	FUNCTION_BEGIN;	

	m_pChangeHairStyleList = NULL;    
	m_pChangeHairColorList = NULL;

	m_pMainChar = NULL;

	FUNCTION_END;
}

/**
 * \brief 析构函数
 * 
 */
CGuiHairStyleDialog::~CGuiHairStyleDialog(void)
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}

/**
 * \brief "创建消息"响应函数
 * 
 * 初始化各个控件(Static，ListBox)
 * 
 * \return void
 */
void CGuiHairStyleDialog::OnCreate(void)
{
	FUNCTION_BEGIN;

	m_ptIcon.x = 36;
	m_ptIcon.y = 110;

	m_pChangeHairStyleList = GetListBox(cChangeHairStyleListCtrlID);    
	m_pChangeHairColorList = GetListBox(cChangeHairColorListCtrlID);    

	if(bControlInit())
	{
		m_pChangeHairStyleList->RemoveAllItems();
		m_pChangeHairColorList->RemoveAllItems();

		m_pMainChar = GetScene()->GetMainCharacter();
		if(m_pMainChar)
		{
			enumSex CharacterSex = m_pMainChar->GetSex();
			size_t hairType = m_pMainChar->GetHairType();
			if(CharacterSex == eSex_Male)//男性
			{
				for(tTableHairStyle::iterator it = g_tableHairStyle.begin();
					it != g_tableHairStyle.end() ;++it)
				{
					if(it->dwSex == 1 && it->dwID != hairType)
					{
						TRACE("AddItem[%s]\n",it->strName);
						m_pChangeHairStyleList->AddItem( it->strName,&(it->dwID) );
					}
				}
			}
			else if(CharacterSex == eSex_Female)//女的
			{
				for(tTableHairStyle::iterator it = g_tableHairStyle.begin();
					it != g_tableHairStyle.end() ;++it)
				{
					if( it->dwSex == 2 && it->dwID != hairType )
					{
						TRACE("AddItem[%s]\n",it->strName);
						m_pChangeHairStyleList->AddItem( it->strName,&(it->dwID) );
					}
				}
			}
		}

		for(tTableHairColor::iterator it = g_tableHairColor.begin();
			it != g_tableHairColor.end() ;++it)
		{
			if( it->dwColor !=  m_pMainChar->GetHairColor() )
			{
				m_pChangeHairColorList->AddItem( it->strName,&(it->dwID) );
			}
		}
	}

	CGuiDialog::OnCreate();
	FUNCTION_END;
}

/**
* \brief "关闭"消息响应函数
* 
* 退出对话框、更新管理器对应的指针、关闭npc对话框
* 
* \return void
*/
void CGuiHairStyleDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiHairStyleDlg = NULL;
	GetGameGuiManager()->OnNpcActionEnd();

	FUNCTION_END;
}


/**
 * \brief "确定"按钮响应消息
 * 
 * 计算角色的银子数目,如果钱够则发出改变发型(发色)命令，否则给出钱不够的提示
 * 
 * \return true if money enough else false
 */
bool CGuiHairStyleDialog::OnOK()
{
	FUNCTION_BEGIN;

	if(m_pMainChar)
	{
		int totalMoney = 0;
		DWORD dwStyle = m_pMainChar->GetHairType();
		DWORD dwColor = m_pMainChar->GetHairColor();

		size_t iMoney = m_pMainChar->GetMoney();
		
		int sindex = m_pChangeHairStyleList->GetCurItem();
		if(sindex >= 0 )
		{
			DWORD *pdwStyle = (DWORD *)m_pChangeHairStyleList->GetItemData(sindex);
			if(pdwStyle)	
			{
				dwStyle = *pdwStyle;
				HairStyle_t* p = GetHairStyle(dwStyle);
				if(p) totalMoney += p->dwMoney;
			}
		}

		int cindex = m_pChangeHairColorList->GetCurItem();
		if(cindex >= 0 )
		{
			DWORD *pdwColor = (DWORD *)m_pChangeHairColorList->GetItemData(cindex);
			if(pdwColor)	
			{
				dwColor = *pdwColor;
				HairColor_t* p = GetHairClor(dwColor);
				if(p)
				{
					dwColor = p->dwColor;
					totalMoney += p->dwMoney;
				}
			}
		}

		if(totalMoney == 0)
		{
			Close();
			return false;
		}
		if(iMoney < totalMoney)
		{
			//sorry your money is not enough
			GetGameGuiManager()->AddClientSystemMessage("对不起，你银子不够");
			return false;
		}
		else
		{
			stHairPropertyUserCmd hairCmd;
			hairCmd.byHairType = dwStyle;
			hairCmd.dwHairColor = dwColor;
			SEND_USER_CMD(hairCmd);
			Close();
			return true;
		}
	}
	return false;

	FUNCTION_END;
}

/**
* \brief GUI消息处理函数
* 
* 处理鼠标在listbox上移动的消息
* 
* \param nID : 控件id
* \return void
*/
void CGuiHairStyleDialog::OnMouseMoveList(UINT nID)
{
	FUNCTION_BEGIN;

	if(nID == cChangeHairColorListCtrlID)
	{
		if(m_pChangeHairColorList)
		{
			for(int i = 0;i < m_pChangeHairColorList->GetCount();++i)
			{
				m_pChangeHairColorList->SetItemColor(i,0,0xffffffff);
			}
		}
	}

	switch(nID)
	{
	case cChangeHairStyleListCtrlID:break;
	case cChangeHairColorListCtrlID:
		{
			int cel = m_pChangeHairColorList->GetMoveSel();
			if(cel >= 0)
			{
				DWORD *pdwColor = (DWORD *)m_pChangeHairColorList->GetItemData(cel);
				if(pdwColor)	
				{
					HairColor_t* p = GetHairClor(*pdwColor);
					if(p) m_pChangeHairColorList->SetItemColor(cel,0,p->dwColor);
				}
			}
		}
		break;
	}

	FUNCTION_END;
}

/**
 * \brief GUI消息处理函数
 * 
 * 分发列表框选择消息(EVENT_LISTBOX_SELECTION),处理按钮消息
 * 
 * \param nEvent : 消息句柄
 * \param nID : 控件id
 * \param pControl : 控件指针
 * \return true if succeed else false
 */
bool CGuiHairStyleDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	char szText[MAX_PATH];
	if(nEvent == EVENT_LISTBOX_SELECTION)
	{
		switch(nID)
		{
		case cChangeHairStyleListCtrlID:
			{
				static int oldIndex = -1;
				int index = m_pChangeHairStyleList->GetCurItem();
				if(index < 0 ) break;
				if(index == oldIndex)
				{
					m_pChangeHairStyleList->SelectItem(-1,false);
					_SetText(cMoneyOfChangeHairStyleID,"");
					oldIndex = -1;
					break;
				}
				oldIndex = index;

				DWORD *pdwStyle = (DWORD *)m_pChangeHairStyleList->GetItemData(index);
				if(pdwStyle)	
				{
					HairStyle_t* p = GetHairStyle(*pdwStyle);
					if(p)
					{
						sprintf(szText,"%s",Money2String(p->dwMoney));
						_SetText(cMoneyOfChangeHairStyleID,szText);
					}
				}
			}
			break;
		case cChangeHairColorListCtrlID:
			{
				static int oldIndex = -1;
				int index = m_pChangeHairColorList->GetCurItem();
				if(index < 0 ) break;
				if(index == oldIndex)
				{
					m_pChangeHairColorList->SelectItem(-1,false);
					_SetText(cMoneyOfChangeHairColorID,"");
					oldIndex = -1;
					break;
				}
				oldIndex = index;

				DWORD *pdwColor = (DWORD *)m_pChangeHairColorList->GetItemData(index);
				if(pdwColor)	
				{
					HairColor_t* p = GetHairClor(*pdwColor);
					if(p) 
					{
						sprintf(szText,"%s",Money2String(p->dwMoney));
						_SetText(cMoneyOfChangeHairColorID,szText);
					}
				}
			}
			break;
		}
	}
	else if(nEvent == EVENT_LISTBOX_MOVESELECTION)
	{
		OnMouseMoveList(nID);
		return true;
	}
	else if(nEvent == EVENT_BUTTON_CLICKED )
	{
		if( nID == cIDOK ) 
		{
			return OnOK();
		}
		else
		{
			Close();
		}
	}
	else if(nEvent == EVENT_CONTROL_MOUSELEAVE)
	{
		if(nID == cChangeHairStyleListCtrlID)
		{
			if(m_pChangeHairStyleList)
			{
				for(int i = 0;i < m_pChangeHairStyleList->GetCount();++i)
				{
					m_pChangeHairStyleList->SetItemColor(i,0,0xffffffff);
				}
			}
		}
		if(nID == cChangeHairColorListCtrlID)
		{
			if(m_pChangeHairColorList)
			{
				for(int i = 0;i < m_pChangeHairColorList->GetCount();++i)
				{
					m_pChangeHairColorList->SetItemColor(i,0,0xffffffff);
				}
			}
		}
	}
	
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CGuiHairStyleDialog::OnSetCloseLocation()
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
