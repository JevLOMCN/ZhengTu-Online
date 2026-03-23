#pragma once

#include "./command.h"

struct stAuctionItem
{
	DWORD	auctionID;
	DWORD	minMoney;
	DWORD	maxMoney;
	DWORD	minGold;
	DWORD	maxGold;
	DWORD	endTime;
	DWORD	dwTimeStamp;
	bool	mine;				//是否我的价格最高
	char	owner[MAX_NAMESIZE];//拍卖者
	DWORD	max;				//总共多少个
	BYTE	bidType;			//金子拍卖还是银子拍卖
	t_Object item;

	stAuctionItem()
	{
		auctionID = 0;
		minMoney = 0;
		maxMoney = 0;
		minGold = 0;
		maxGold = 0;
		bzero(&item, sizeof(item));
		mine = false;
		bzero(owner, sizeof(owner));
		dwTimeStamp = 0;
		bidType = 0;
	}

	void SetAttrib(const stAddListAuction* cmd)
	{
		auctionID = cmd->auctionID;
		minMoney = cmd->minMoney;
		maxMoney = cmd->maxMoney;
		minGold = cmd->minGold;
		maxGold = cmd->maxGold;
		item = cmd->item;
		mine = cmd->mine;
		endTime = cmd->endTime;
		bidType = cmd->bidType;

		_snprintf(owner,MAX_NAMESIZE,cmd->owner);
		owner[MAX_NAMESIZE-1] = 0;
		dwTimeStamp = xtimeGetTime();
	}
};

typedef std::vector<stAuctionItem*> tListAuctionItem;
typedef tListAuctionItem::iterator	tListAuctionItemIt;

enum enumAuctionListType
{
	eAuction_Browse,
	eAuction_Roup,
	eAuction_Auction,
	eAuction_All
};

enum eSortType
{
	eSort_By_NeedLevel,
	eSort_By_Quality,
	eSort_By_Owner,
	eSort_By_LastTime,
	eSort_By_Price
};

class CAuction
{
public:
	CAuction(void);
	~CAuction(void);

	void Release();
	bool ParseAddListAuctionCmd(stAddListAuction* cmd);
	bool ParseRemoveListAuctionCmd(stRemoveListAuction* cmd);
	bool RemoveItem( DWORD dwId,tListAuctionItem& list );	
	bool ClearListByType(enumAuctionListType type);
	void Sort(enumAuctionListType type,eSortType sort,bool bUp);

	//bool m_bIsHaveNextPage;

	tListAuctionItem	m_BrowseList;	//浏览
	tListAuctionItem	m_RoupList;		//竞标
	tListAuctionItem	m_AuctionList;	//拍卖

	stAuctionItem* GetItem(DWORD dwId,enumAuctionListType type);
	BYTE GetItemBidType(DWORD dwId,enumAuctionListType type);
	tListAuctionItemIt GetItem(DWORD dwId, tListAuctionItem& list);
private:
	int	 AddItem(const stAddListAuction* cmd);
	int	 AddItem(const stAddListAuction* cmd,tListAuctionItem& list);
	bool UpdateItem(const stAddListAuction* cmd );
	bool UpdateItem( tListAuctionItem& list,const stAddListAuction* cmd,int page );	
};
