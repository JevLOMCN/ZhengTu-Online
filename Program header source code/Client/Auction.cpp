#include "public.h"
#include ".\Auction.h"
#include "GameGuiManager.h"
#include "GuiAuctionDialog.h"

CAuction::CAuction(void)
{
	//m_bIsHaveNextPage = false;
}

CAuction::~CAuction(void)
{
	Release();
}

void CAuction::Release()
{
	FUNCTION_BEGIN;

	FreePointerList<tListAuctionItem>(m_BrowseList);
	FreePointerList<tListAuctionItem>(m_RoupList);
	FreePointerList<tListAuctionItem>(m_AuctionList);
	FUNCTION_END;
}

bool CAuction::UpdateItem(const stAddListAuction* cmd )
{

	FUNCTION_BEGIN;
	bool bRes = false;

	if ( cmd->list == 1 )
	{
		return UpdateItem(m_BrowseList,cmd,0);
	}
	if ( cmd->list == 2 )
	{
		return UpdateItem(m_RoupList,cmd,1);
	}
	if ( cmd->list == 3 )
	{
		return UpdateItem(m_AuctionList,cmd,2);
	}
	if ( cmd->list == 4 )
	{

	}


	return true;

	return bRes;
	FUNCTION_END;
}

bool CAuction::UpdateItem( tListAuctionItem& list,const stAddListAuction* cmd,int page )
{
	FUNCTION_BEGIN;
	tListAuctionItemIt it = GetItem( cmd->auctionID,list );
	if( it != list.end() )
	{
		(*it)->minMoney = cmd->minMoney;
		(*it)->minGold = cmd->minGold;
		(*it)->endTime = cmd->endTime;

		if ( GetGameGuiManager()->m_guiAuctionDlg )
		{
			GetGameGuiManager()->m_guiAuctionDlg->UpdateAuctionItem( page,(*it) );		
		}
		return true;
	}
	return false;
	FUNCTION_END;
}

bool CAuction::ParseAddListAuctionCmd(stAddListAuction* cmd)
{
	FUNCTION_BEGIN;
	if( NULL == cmd ) return false;

	int iRes = AddItem(cmd);
	if( iRes == 2 ) //表示增加了
	{
		if( GetGameGuiManager()->m_guiAuctionDlg )
		{
			if( cmd->list == 1 || cmd->list == 4 )
			{
				GetGameGuiManager()->m_guiAuctionDlg->UpdateList(m_BrowseList,eAuction_Browse);		
			}
			if( cmd->list == 2 || cmd->list == 4 )
			{
				GetGameGuiManager()->m_guiAuctionDlg->UpdateList(m_RoupList,eAuction_Roup);	
			}
			if( cmd->list == 3 || cmd->list == 4 )
			{
				GetGameGuiManager()->m_guiAuctionDlg->UpdateList(m_AuctionList,eAuction_Auction);	
			}
		}
	}

	return true;

	FUNCTION_END;
}

bool CAuction::ParseRemoveListAuctionCmd(stRemoveListAuction* cmd)
{
	FUNCTION_BEGIN;
	if( NULL == cmd ) return false;
	bool bRes = false;

	if ( GetGameGuiManager()->m_guiAuctionDlg )
	{
		if( cmd->list == 1 || cmd->list == 4 )
		{
			RemoveItem(cmd->auctionID,m_BrowseList);
			GetGameGuiManager()->m_guiAuctionDlg->RemoveAuctionItem(cmd->auctionID,eAuction_Browse);
		}
		if( cmd->list == 2 || cmd->list == 4 )
		{
			RemoveItem(cmd->auctionID,m_RoupList);
			GetGameGuiManager()->m_guiAuctionDlg->RemoveAuctionItem(cmd->auctionID,eAuction_Roup);
		}
		if( cmd->list == 3 || cmd->list == 4 )
		{
			RemoveItem(cmd->auctionID,m_AuctionList);
			GetGameGuiManager()->m_guiAuctionDlg->RemoveAuctionItem(cmd->auctionID,eAuction_Auction);
		}
	}
	return false;
	FUNCTION_END;
}

bool CAuction::RemoveItem( DWORD dwId,tListAuctionItem& list )
{
	FUNCTION_BEGIN;
	for ( tListAuctionItemIt it = list.begin(); it != list.end(); ++it )
	{
		if( (*it)->auctionID == dwId )
		{
			delete (*it);
			list.erase( it );
			return true;
		}
	}

	return true;
	FUNCTION_END;
}

tListAuctionItemIt CAuction::GetItem(DWORD dwId,tListAuctionItem& list)
{
	FUNCTION_BEGIN;
	for ( tListAuctionItemIt it = list.begin(); it != list.end(); ++it )
	{
		if( (*it)->auctionID == dwId )
		{
			return it;
		}
	}
	return list.end();
	FUNCTION_END;
}

bool CAuction::ClearListByType(enumAuctionListType type)
{
	FUNCTION_BEGIN;
	switch( type )
	{
	case eAuction_Browse:	FreePointerList<tListAuctionItem>(m_BrowseList);	break;
	case eAuction_Roup:		FreePointerList<tListAuctionItem>(m_RoupList);		break;
	case eAuction_Auction:	FreePointerList<tListAuctionItem>(m_AuctionList);	break;
	default:
		return false;
		break;
	}
	return true;
	FUNCTION_END;
}

stAuctionItem* CAuction::GetItem(DWORD dwId,enumAuctionListType type)
{
	FUNCTION_BEGIN;

	if ( type == eAuction_Browse )
	{
		for ( tListAuctionItemIt it = m_BrowseList.begin(); it != m_BrowseList.end(); ++it )
		{
			if( (*it)->auctionID == dwId )
			{
				return (*it);
			}
		}
	}
	if ( type == eAuction_Roup )
	{
		for ( tListAuctionItemIt it = m_RoupList.begin(); it != m_RoupList.end(); ++it )
		{
			if( (*it)->auctionID == dwId )
			{
				return (*it);
			}
		}
	}
	if ( type == eAuction_Auction )
	{
		for ( tListAuctionItemIt it = m_AuctionList.begin(); it != m_AuctionList.end(); ++it )
		{
			if( (*it)->auctionID == dwId )
			{
				return (*it);
			}
		}
	}
	return NULL;
	FUNCTION_END;
}

BYTE CAuction::GetItemBidType(DWORD dwId,enumAuctionListType type)
{
	stAuctionItem* pItem = GetItem(dwId,type);
	if( pItem )
	{
		return pItem->bidType;
	}
	return AUCTION_BID_TYPE_MONEY;
}

int	 CAuction::AddItem(const stAddListAuction* cmd)
{
	FUNCTION_BEGIN;
	if ( cmd->list == 1 )
	{
		return AddItem(cmd,m_BrowseList);
	}
	if ( cmd->list == 2 )
	{
		return AddItem(cmd,m_RoupList);
	}
	if ( cmd->list == 3 )
	{
		return AddItem(cmd,m_AuctionList);
	}
	if( cmd->list == 4 )
	{
		AddItem(cmd,m_BrowseList);
		AddItem(cmd,m_RoupList);
		AddItem(cmd,m_AuctionList);
		return 1;
	}
	return 0;
	FUNCTION_END;
}

int	 CAuction::AddItem(const stAddListAuction* cmd,tListAuctionItem& list)
{
	FUNCTION_BEGIN;
	Debug_Int( list.size() );

	//到浏览页面去查找该id的物品,如果找到则同时更新它的价格
	stAuctionItem* pItem = GetItem(cmd->auctionID,eAuction_Browse);	
	if( pItem )
	{
		pItem->minMoney = cmd->minMoney;
		pItem->minGold = cmd->minGold;
		pItem->endTime = cmd->endTime;
		pItem->mine = cmd->mine;
		pItem->max = cmd->max;
		GetGameGuiManager()->m_guiAuctionDlg->UpdateAuctionItem( 0,pItem );	
		if( cmd->list == 1 )
		{
			return 1;
		}
	}

	tListAuctionItemIt it = GetItem(cmd->auctionID,list);
	if ( it != list.end() )
	{
		//找到就刷新
		(*it)->minMoney = cmd->minMoney;
		(*it)->minGold = cmd->minGold;
		(*it)->endTime = cmd->endTime;
		(*it)->mine = cmd->mine;	
		(*it)->max = cmd->max;

		if ( GetGameGuiManager()->m_guiAuctionDlg )
		{
			GetGameGuiManager()->m_guiAuctionDlg->UpdateAuctionItem( cmd->list - 1,(*it) );	
		}
		return 1;
	}
	else
	{
		//没有找到就增加
		stAuctionItem* pItem = new stAuctionItem();
		if( NULL == pItem )
		{
			return 0;
		}
		pItem->SetAttrib(cmd);
		pItem->max = cmd->max;
		list.push_back( pItem );

		if( cmd->list == 1 || cmd->list == 4 )
		{
			//if( list.size() >= 9 )
			//{
			//	m_bIsHaveNextPage = true;
			//}
			//对浏览页面特殊处理
			if( list.size() > 10 )
			{
				tListAuctionItemIt it = list.begin();
				delete (*it);
				list.erase(it);
			}
		}
		return 2;
	}
	return 0;
	FUNCTION_END;
}

template<bool bUp>
bool ComAuctionItByNeedLevel(stAuctionItem*& it1,stAuctionItem*& it2)
{
	if ( bUp )
	{
		return it1->item.needlevel >= it2->item.needlevel;
	}
	return it1->item.needlevel <= it2->item.needlevel;
}

template<bool bUp>
bool ComAuctionItByQuality(stAuctionItem*& it1,stAuctionItem*& it2)
{
	if ( bUp )
	{
		return (it1->item.kind >= it2->item.kind);
	}
	return (it1->item.kind <= it2->item.kind);	
}

template<bool bUp>
bool ComAuctionItByOwner(stAuctionItem*& it1,stAuctionItem*& it2)
{
	if ( bUp )
	{
		return (strcmp( it1->owner,it2->owner) >= 0);
	}
	return (strcmp( it1->owner,it2->owner) <= 0);
}

template<bool bUp>
bool ComAuctionItByTime(stAuctionItem*& it1,stAuctionItem*& it2)
{
	if ( bUp )
	{
		return it1->endTime >= it2->endTime;
	}
	return it1->endTime <= it2->endTime;
}

template<bool bUp>
bool ComAuctionItByPrice(stAuctionItem*& it1,stAuctionItem*& it2)
{
	if ( bUp )
	{
		return it1->minMoney >= it2->minMoney;
	}
	return it1->minMoney <= it2->minMoney;
}

void CAuction::Sort(enumAuctionListType type,eSortType sort,bool bUp)
{
	tListAuctionItem* pList = NULL;
	if ( type == eAuction_Browse )
	{
		pList = &m_BrowseList;
	}
	if ( type == eAuction_Roup )
	{
		pList = &m_RoupList;
	}
	if ( type == eAuction_Auction )
	{
		pList = &m_AuctionList;
	}

	if( bUp )
	{
		switch( sort )
		{
		case eSort_By_NeedLevel:std::sort(pList->begin(),pList->end(),ComAuctionItByNeedLevel<true>);break;
		case eSort_By_Quality:	std::sort(pList->begin(),pList->end(),ComAuctionItByQuality<true>);break;
		case eSort_By_Owner:	std::sort(pList->begin(),pList->end(),ComAuctionItByOwner<true>);break;
		case eSort_By_LastTime:	std::sort(pList->begin(),pList->end(),ComAuctionItByTime<true>);break;
		case eSort_By_Price:	std::sort(pList->begin(),pList->end(),ComAuctionItByPrice<true>);break;
		default:break;
		}
	}
	else
	{
		switch( sort )
		{
		case eSort_By_NeedLevel:std::sort(pList->begin(),pList->end(),ComAuctionItByNeedLevel<false>);break;
		case eSort_By_Quality:	std::sort(pList->begin(),pList->end(),ComAuctionItByQuality<false>);break;
		case eSort_By_Owner:	std::sort(pList->begin(),pList->end(),ComAuctionItByOwner<false>);break;
		case eSort_By_LastTime:	std::sort(pList->begin(),pList->end(),ComAuctionItByTime<false>);break;
		case eSort_By_Price:	std::sort(pList->begin(),pList->end(),ComAuctionItByPrice<false>);break;
		default:break;
		}
	}
}
