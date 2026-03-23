#include "public.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include ".\GuiOthersShopDialog.h"
#include "./RoleItem.h"
#include "./GuiItem.h"
#include "./GuiInputBox.h"
#include "./GuiMain.h"

#define OTHERS_SHOP_HEIGHT_GRID			6
#define OTHERS_SHOP_WIDTH_GRID			7

const static stRectI g_sShopNameRect(106,93,290,111);

CGuiOthersShopDialog::CGuiOthersShopDialog(void)
{
	m_bCanClose = false;
	m_dwBeginTime = 0;
	m_dwOwnerID = 0;
	memset(m_szOwnerName,0,MAX_NAMESIZE);
}

CGuiOthersShopDialog::~CGuiOthersShopDialog(void)
{
	RemoveAllItem();
}

void CGuiOthersShopDialog::OnCreate(void)
{
	CGuiDialog::OnCreate();

	m_shopTable = GetTable(2);

	if( NULL == m_shopTable )
	{
		Engine_WriteLog("摊位othersshop对话框格子控件id不正确！");
		Close();
		return ;
	}
	m_shopTable->AddFlag(eTip_ShowPrice);
	m_shopTable->m_iTableType	= OBJECTCELLTYPE_OTHERSSHOP;
	m_shopTable->m_iTableID		= 0;
	m_dwBeginTime = xtimeGetTime();
}

bool CGuiOthersShopDialog::bCanClose()
{
	return (xtimeGetTime() - m_dwBeginTime > 3000 || m_bCanClose);
}

bool CGuiOthersShopDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED && nID == GUI_CONTROL_CLOSEBOX )
	{
		if ( bCanClose() )
		{
			Close();
		}
		else
		{
			return true;
		}
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiOthersShopDialog::OnClose(void)
{
	RemoveAllItem();
	GetGameGuiManager()->m_guiOthersShop = NULL;
}

HRESULT CGuiOthersShopDialog::OnRender(float fElapsedTime)
{
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	stRectI rect(g_sShopNameRect);
	rect.OffsetRect(m_x,m_y);
	GetDevice()->DrawString(m_szOwnerName,rect,-1,DT_VCENTER|DT_CENTER);
	
	return hr;
}

void CGuiOthersShopDialog::OnSetCloseLocation()
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

bool CGuiOthersShopDialog::AddItem(t_Object* pObj,int x,int y,int price)
{
	CRoleItem* pItem = GetCellItem(m_shopTable,stPointI(x,y));
	if( pItem )
	{
		pItem->OnRemove();
		delete pItem;
	}
	pItem = new CRoleItem();
	pObj->pos.dwLocation = OBJECTCELLTYPE_OTHERSSHOP;
	pObj->pos.dwTableID = 0;
	pObj->pos.x = x;
	pObj->pos.y = y;
	pItem->SetObject(pObj);
	pItem->SetShopPrice(price);
	pItem->OnUpdate(-1);
	return true;
}

void CGuiOthersShopDialog::RemoveAllItem()
{
	for( int i = 0; i < OTHERS_SHOP_WIDTH_GRID; ++i )
	{
		for(int j = 0; j < OTHERS_SHOP_HEIGHT_GRID; ++j )
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


void CGuiOthersShopDialog::RemoveItem(int x,int y)
{
	CRoleItem* pItem = GetCellItem(m_shopTable,stPointI(x,y));
	if( pItem )
	{
		pItem->OnRemove();
		delete pItem;
	}
}

DWORD CGuiOthersShopDialog::GetOwnerID()
{
	return m_dwOwnerID;
}

void CGuiOthersShopDialog::SetOwner(const char* pszOwner)
{
	_snprintf(m_szOwnerName,MAX_NAMESIZE,"%s 的摊位",pszOwner);
	m_szOwnerName[MAX_NAMESIZE - 1] = 0;
}

void CGuiOthersShopDialog::RequestBuyItem(DWORD dwThisID)
{
	stRequestSellBuyUserCmd cmd;
	cmd.temp_id = m_dwOwnerID;
	cmd.object_id = dwThisID;
	SEND_USER_CMD(cmd);

	//发送请求后就删除该物品
	for( int i = 0; i < OTHERS_SHOP_WIDTH_GRID; ++i )
	{
		for(int j = 0; j < OTHERS_SHOP_HEIGHT_GRID; ++j )
		{
			CRoleItem* pItem = GetCellItem(m_shopTable,stPointI(i,j));
			if( pItem && pItem->GetThisID() == dwThisID )
			{
				pItem->OnRemove();
				delete pItem;
			}
		}
	}
}

void CGuiOthersShopDialog::ParseAddObjectCmd(stAddObjectSellUserCmd* pCmd)
{
	if( GetGameGuiManager()->m_guiOthersShop )
	{
		GetGameGuiManager()->m_guiOthersShop->AddItem(&(pCmd->object),pCmd->x,pCmd->y,pCmd->price);
	}
}

void CGuiOthersShopDialog::ParseAddObjectListCmd(stAddObjectSellListUserCmd* pCmd)
{
	CGuiOthersShopDialog* pOthersShop = GetGameGuiManager()->m_guiOthersShop;
	if( pOthersShop )
	{
		for( int i = 0; i < pCmd->num; i++ )
		{
			pOthersShop->AddItem( &(pCmd->list[i].object),pCmd->list[i].x,pCmd->list[i].y,pCmd->list[i].price );
		}
	}
}