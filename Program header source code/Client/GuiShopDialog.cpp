/**
 *\file		GuiShopDialog.cpp
 *\version	$ID$
 *\author	$$$04公司
 *			$$$04公司@###.com
 *\date		2005-07-04 15:48:00
 *\brief	摆摊对话框
 *
 * 摆摊
 *
 */

#include "public.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include ".\GuiShopDialog.h"
#include "./RoleItem.h"
#include "./GuiItem.h"
#include "./GuiInputBox.h"
#include "./GuiMain.h"
#include "./GuiOthersShopDialog.h"
#include "./GameAppation.h"
#include "GameScene.h"
#include "MainCharacter.h"

#define SHOP_HEIGHT_GRID		6
#define SHOP_WIDTH_GRID			7

const int iShopDlgIdTable = 2;
const int iShopDlgIdOk = 3;
const int iShopDlgIdCancel = 4;

const stRectI sShopNameRect(106,93,290,111);
const stRectI sShopTitleRect(106,118,315,143);

#define SHOPSHOWBTN(id,bShow) {CGuiButton* pBtn = GetButton(id);if(pBtn) pBtn->SetVisible(bShow);}
/**
 * \brief 构造函数
 * 
 */
CGuiShopDialog::CGuiShopDialog(ShopType type)
{
	FUNCTION_BEGIN;

	//m_bCloseIsHide = true;
	//m_shopType = type;
	m_shopTable = NULL;
	
	FUNCTION_END;
}

/**
 * \brief 析构函数
 * 
 */
CGuiShopDialog::~CGuiShopDialog()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
 * \brief "创建消息"响应函数
 * 
 * \return void
 */
void CGuiShopDialog::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	m_shopTable = GetTable(iShopDlgIdTable);
	
	if( NULL == m_shopTable )
	{
		Engine_WriteLog("摆摊对话框格子控件id不正确！");
		Close();
		return ;
	}
	SHOPSHOWBTN(iShopDlgIdOk,true);
	SHOPSHOWBTN(iShopDlgIdCancel,false);
	m_shopTable->m_iTableType	= OBJECTCELLTYPE_MYSHOP;	
	m_shopTable->m_iTableID		= 0;

	FUNCTION_END;
}


/**
 * \brief GUI消息处理函数
 * 
 * \param nEvent : 消息句柄
 * \param nID : 控件id
 * \param pControl : 控件指针
 * \return true if succeed else false
 */
bool CGuiShopDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
		case iShopDlgIdOk:
			return OnIDOK();
			break;
		case iShopDlgIdCancel:
			return OnIDCancel();
			break;
		case GUI_CLOSEBOX_ID:
			{
				SetVisible(false);
				return true;
			}
			break;
		default:
			break;
		}
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

/**
* \brief 对话框关闭消息处理
* 
* 退出对话框、更新管理器对应的指针
*
* \return void
*/
void CGuiShopDialog::OnClose(void)
{
	FUNCTION_BEGIN;	

	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
	if(pMainRole && pMainRole->bIsShopStart() )
	{
		stFinishSellUserCmd cmd;
		SEND_USER_CMD(cmd);	
	}
	RemoveAllItem();
	GetGameGuiManager()->m_guiShop = NULL;

	FUNCTION_END;
}


HRESULT CGuiShopDialog::OnRender(float fElapsedTime)
{
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	RenderText();

	return hr;
}

bool CGuiShopDialog::RenderText()
{
	//显示文字(标题、提示)
	char szText[32];
	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
	if( pMainRole && !pMainRole->bIsShopStart() )
	{
		sprintf(szText,"摆摊前，右键可取消摊位里的物品");
		stRectI rect(sShopTitleRect);
		rect.OffsetRect(m_x,m_y);
		GetDevice()->DrawString(szText,rect,-1,DT_VCENTER|DT_CENTER);
	}
	else
	{
		sprintf(szText,"摆摊");
		stRectI rect(sShopNameRect);
		rect.OffsetRect(m_x,m_y);
		GetDevice()->DrawString(szText,rect,-1,DT_VCENTER|DT_CENTER);
	}
	return true;	

}

void CGuiShopDialog::OnMainRoleShopOpen()
{
	CGuiInputBox* pInput = GetGameGuiManager()->AddInputBox("请输入摆摊广告语,(30字以内)",eAction_StartShop);			
	if( pInput )
	{
		pInput->SetLengthOfText(60);
	}
	SHOPSHOWBTN(iShopDlgIdOk,false);
	SHOPSHOWBTN(iShopDlgIdCancel,true);
	SendAllItemToServer();
	SetVisible(false);
}

bool CGuiShopDialog::OnIDOK()
{
	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
	if(pMainRole)
	{	
		pMainRole->SetMainRoleShopAdv("");

		if ( pMainRole->bIsShopStart() ) 
		{
			//隐藏摊位界面
			SetVisible(false);
		}
		else
		{
			bool bAutoPk = GetGameApplication()->bClientSetted(enumAuto_Kill_Summon);
			if( bAutoPk )
			{
				GetGameGuiManager()->AddClientSystemMessage("正在自动打怪,无法摆摊");
				return false;
			}
			if ( pMainRole->IsMoving() )
			{
				GetGameGuiManager()->AddClientSystemMessage("正在移动,无法摆摊");
				return false;
			}
			if ( pMainRole->IsAttacking() )
			{
				GetGameGuiManager()->AddClientSystemMessage("正在攻击,无法摆摊");
				return false;
			}

			if ( pMainRole->IsFollowState() )
			{
				GetGameGuiManager()->AddClientSystemMessage("正在跟随,无法摆摊");
				return false;
			}
			if ( pMainRole->IsChangeFace() )
			{
				GetGameGuiManager()->AddClientSystemMessage("变形状态,无法摆摊");
				return false;
			}
			if ( pMainRole->IsCombinationState() )
			{
				GetGameGuiManager()->AddClientSystemMessage("合体状态,无法摆摊");
				return false;
			}
			
			//{
			//	GetGameGuiManager()->AddClientSystemMessage("你没有放入任何物品,无法摆摊");
			//	return false;
			//}				

			//发送开始摆摊消息(请求)
			stSellTradeUserCmd cmd;
			SEND_USER_CMD(cmd);

			if( GetGameGuiManager()->m_guiItem )
			{
				GetGameGuiManager()->m_guiItem->SetVisible(false);
			}
		}
	}
	return true;
}

bool CGuiShopDialog::OnIDCancel()
{
	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
	if(pMainRole)
	{	
		if( pMainRole->bIsShopStart() )
		{
			GetGameGuiManager()->AddMessageBox("你确定要收摊吗?",eAction_CloseShop,NULL,c_szMBOKCancel);
			return true;
		}		
	}
	Close();

	return true;
}

void CGuiShopDialog::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());
		m_pBtnClose->SetLocation(pt.x - 65,pt.y);
	}

	FUNCTION_END;
}


void CGuiShopDialog::SendAllItemToServer()
{
	/// 用户请求添加物品id列表
	char buffer[64*1024] = {0};
	stRequestAddObjectSellUserCmd *pCmd=(stRequestAddObjectSellUserCmd *)buffer;
	constructInPlace(pCmd);

	int num = 0;
	for( int i = 0; i < SHOP_WIDTH_GRID; ++i )
	{
		for(int j = 0; j < SHOP_HEIGHT_GRID; ++j )
		{
			CRoleItem* pItem = GetCellItem(m_shopTable,stPointI(i,j));
			if( pItem )
			{				
				pCmd->list[num].qwThisID = pItem->GetThisID();
				pCmd->list[num].x = i;
				pCmd->list[num].y = j;
				pCmd->list[num].price = pItem->GetPrice(ITEM_PRICE_SHOP);
				num ++;
			}
		}
	}
	pCmd->num = num;

	size_t cmdLen = sizeof( stRequestAddObjectSellUserCmd ) + num*sizeof(stRequestAddObjectSellUserCmd::tSellObject);

	Client_SendCommand( pCmd, cmdLen ,false);

	stSellTradeUserCmd cmd;
	//通知服务端物品增加完毕
	SEND_USER_CMD(cmd);
}

void CGuiShopDialog::RemoveAllItem()
{
	for( int i = 0; i < SHOP_WIDTH_GRID; ++i )
	{
		for(int j = 0; j < SHOP_HEIGHT_GRID; ++j )
		{
			CRoleItem* pItem = GetCellItem(m_shopTable,stPointI(i,j));
			if( pItem )
			{
				pItem->OnRemove();
				delete pItem;
			}
		}
	}
}

bool CGuiShopDialog::RemoveItem(int x,int y)
{
	CRoleItem* pItem = GetCellItem(m_shopTable,stPointI(x,y));
	if( pItem )
	{
		pItem->OnRemove();
		delete pItem;
		return true;
	}
	return false;
}


//判断pItem能不能放到摊位来，如果能放，可以放在pt位置
bool CGuiShopDialog::bItemCanput(CRoleItem* pItem,POINT &pt)
{
	if( NULL == pItem )
	{
		return false;
	}

	bool bFindPlace = false;
	for( int i = 0; i < SHOP_WIDTH_GRID; ++i )
	{
		for(int j = 0; j < SHOP_HEIGHT_GRID; ++j )
		{
			CRoleItem* p = GetCellItem(m_shopTable,stPointI(i,j));
			if( NULL == p && !bFindPlace )
			{
				pt = stPointI(i,j);
				bFindPlace = true;
			}
			if( p && pItem->GetThisID() == p->GetThisID() )
			{
				GetGameGuiManager()->AddClientSystemMessage("该物品已经添加过了!");
				return false;
			}
		}
	}
	if ( !bFindPlace )
	{
		GetGameGuiManager()->AddClientSystemMessage("没有空位了!");
	}
	return bFindPlace;
}

//把pItem的物品放一个镜像在摊位格子
bool CGuiShopDialog::AddItem(CRoleItem* pItem)
{
	stPointI ptGrid(0,0);
	bool bCanput = bItemCanput(pItem,ptGrid);
	if( bCanput )
	{
		stObjectLocation dst;
		dst.dwLocation = OBJECTCELLTYPE_MYSHOP;
		dst.dwTableID = 0;
		dst.x = ptGrid.x;
		dst.y = ptGrid.y;
		//pItem->AddShowLocation(dst,false,true);
		pItem->CopyTo(dst);

		CRoleItem* pImage = pItem->GetRoleItemImage();
		if( pImage )
		{
			pImage->SetShopPrice(pItem->GetPrice(ITEM_PRICE_SHOP));
		}
		//pItem->SetShopPrice(0);
		return true;
	}
	return false;
}

bool CGuiShopDialog::IsItemIn(DWORD dwThisID)
{
	for( int i = 0; i < SHOP_WIDTH_GRID; ++i )
	{
		for(int j = 0; j < SHOP_HEIGHT_GRID; ++j )
		{
			CRoleItem* pItem = GetCellItem(m_shopTable,stPointI(i,j));
			if( pItem && pItem->GetThisID() == dwThisID )
			{
				return true;
			}			
		}
	}
	return false;
}

bool CGuiShopDialog::RemoveItem(DWORD dwThisID)
{
	for( int i = 0; i < SHOP_WIDTH_GRID; ++i )
	{
		for(int j = 0; j < SHOP_HEIGHT_GRID; ++j )
		{
			CRoleItem* pItem = GetCellItem(m_shopTable,stPointI(i,j));
			if( pItem && pItem->GetThisID() == dwThisID )
			{
				pItem->OnRemove();
				delete pItem;
				return true;
			}			
		}
	}
	return false;
}

bool CGuiShopDialog::OnOpenShop( DWORD dwShopOwnerID )
{	
	if( dwShopOwnerID == GetScene()->GetMainCharacter()->GetID() )
	{
		//自己的摊位
		GetGameGuiManager()->AddShop();
		return true;
	}
	else 
	{
		if( NULL != GetGameGuiManager()->m_guiOthersShop )
		{
			if( GetGameGuiManager()->m_guiOthersShop->bCanClose() )
			{
				GetGameGuiManager()->m_guiOthersShop->Close();
			}
			else
			{
				//如果原来别人的摊位对话打开了不允许再打开
				return false;
			}			
		}
		CGuiOthersShopDialog* pDlg = GetGameGuiManager()->AddOthersShop();
		if ( NULL != pDlg )
		{
			pDlg->SetOwnerID(dwShopOwnerID);

			CCharacter* pChar = GetScene()->FindCharacterByID(dwShopOwnerID);
			if ( NULL != pChar )
			{
				pDlg->SetOwner( pChar->GetName() );
				stRequestSellInfoUserCmd cmd;
				cmd.temp_id = dwShopOwnerID;
				SEND_USER_CMD(cmd);
			}
		}

		return true;
	}		
	return false;
}