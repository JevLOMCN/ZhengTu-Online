#include "public.h"
#include "miniusercommand.h"
#include "GameGuiManager.h"
#include "../Media/SoundManager.h"
#include "Game.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include ".\lordstrikecard.h"
#include ".\lordstrikePlayer.h"
#include "LordStrikeGame.h"
 


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
CPlayer::CPlayer()
{
	m_nCurNums = 0;
}

CPlayer::~CPlayer()
{
	ClearCard();
}


void CPlayer::ClearCard()
{
	for(int i = 0 ; i < m_vecCards.size() ; i++)
	{
		CCard * pCard = m_vecCards[i];
		delete pCard;
	}
	m_vecCards.clear();
}


void CPlayer::DrawName(int nOffsetX,int nOffsetY)
{
	// role name 
	int iFont = GetDevice()->SetFont(1); 

	int nlen = GetDevice()->GetFontWidth(1) * m_strName.length();

	stRectI rcMsg;
	rcMsg.top = nOffsetY;
	rcMsg.bottom = rcMsg.top + 60; 
	int dir = GetDir();
	if(dir == 1 || dir == 2 )
	{
		rcMsg.left = nOffsetX;
		rcMsg.right =  rcMsg.left + nlen;
	}
	else
	{
		rcMsg.right = nOffsetX;
		rcMsg.left = rcMsg.right - nlen;
	}
	GetDevice()->DrawString(m_strName.c_str(),m_strName.length(),rcMsg,COLOR_ARGB(255,74,233,29),DT_CENTER,FontEffect_Border);

	GetDevice()->SetFont(iFont);
}

void CPlayer::Draw(int nOffsetX,int nOffsetY)
{
	/*int iFont = GetDevice()->SetFont(2); 
	
	char szText[30] = {0};
	_snprintf(szText,30,"牌数%d",GetCurNum());
	szText[29] = 0;
        int nlen = GetDevice()->GetFontWidth(2) * strlen(szText);
	 
	stRectI rcMsg;
	rcMsg.top = nOffsetY;
	rcMsg.bottom = rcMsg.top + 60; 
	int dir = GetDir();
	if(dir == 1 || dir == 2 )
	{
		rcMsg.left = nOffsetX;
		rcMsg.right =  rcMsg.left + nlen;
	}
	else
	{
		rcMsg.right = nOffsetX;
		rcMsg.left = rcMsg.right - nlen;
	}

	GetDevice()->DrawString(szText,strlen(szText),rcMsg,COLOR_ARGB(255,74,233,29),
			DT_CENTER,
			FontEffect_Border);
	GetDevice()->SetFont(iFont);*/
}

void CPlayer::AddCard(int nCard)   // 加牌
{
	std::vector<CCard *> & cardlist = GetCardList();
	CCard * pCard = new CCard;
	pCard->SetCardID(nCard);
	cardlist.push_back(pCard);
}

void CPlayer::RemoveCard(int nCard)   // 出牌
{
	std :: vector<CCard *> & cardlist = GetCardList();
	std :: vector<CCard *>::iterator iter;
	for(iter = cardlist.begin();iter != cardlist.end() ;)
	{
		CCard * pCard = *iter;
		if(pCard)
		{
			if(pCard->GetCardID() == nCard)
			{
				delete pCard;
				cardlist.erase(iter);
				return;
			}
			else 
				++iter;
		}
	}
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
CMainPlayer::CMainPlayer()
{
}

CMainPlayer::~CMainPlayer()
{
	CLordStrikeManager::GetSingleton()->GetGame()->ClearMainPlayer();
}

void CMainPlayer::AddPlayingCard(int nCard)   // 加牌
{
	std::vector<CCard *> & cardlist = m_vecPlayingCards;
	CCard * pCard = new CCard;
	pCard->SetCardID(nCard);
	cardlist.push_back(pCard);
}

void CMainPlayer::RemovePlayingCard(int nCard)   // 出牌
{
	std :: vector<CCard *> & cardlist = m_vecPlayingCards;
	std :: vector<CCard *>::iterator iter;
	for(iter = cardlist.begin();iter != cardlist.end() ;)
	{
		CCard * pCard = *iter;
		if(pCard)
		{
			if(pCard->GetCardID() == nCard)
			{
				delete pCard;
				cardlist.erase(iter);
				return;
			}
			else 
				++iter;
		}
	}
}

void CMainPlayer::SortPlayingCard(bool bUp)
{
	std::vector<CCard *> & cardlist = m_vecPlayingCards;
	if(bUp)
	    std::sort(cardlist.begin(),cardlist.end(),CardCompare<true>());
	else
	    std::sort(cardlist.begin(),cardlist.end(),CardCompare<false>());

}

void CMainPlayer::ClearPlayingCard()
{
	for(int i = 0 ; i < m_vecPlayingCards.size() ; i++)
	{
		CCard * pCard = m_vecPlayingCards[i];
		delete pCard;
	}
	m_vecPlayingCards.clear();
}

CCard * CMainPlayer::GetLastPlayingCard()
{
	if(m_vecPlayingCards.size() > 0)
	{
		CCard * pard = m_vecPlayingCards[m_vecPlayingCards.size()-1];
		if(pard)
			return pard;
	}
	return 0;
}


int SeatToDirection(BYTE seat,BYTE selfseat)
{
	int ndirect =  (2 - selfseat) + seat;
	if(ndirect > 3)
		ndirect -= 3;
	if(ndirect <= 0)
		ndirect +=3;
	return ndirect;
}

int DirectionToSeat(BYTE direction,BYTE selfseat)
{
	int nseat =  (selfseat - 2) + direction;
	if(nseat > 3)
		nseat -= 3;
	if(nseat <=0)
		nseat += 3;
	return nseat;
}

// 图片编号规律: 100 *　花色　＋  牌大小(1~15)
// 花色: 
// 0:黑桃
// 1:红心
// 2:梅花
// 3:方块

//14:小王
//15:大王

//enum Suit
//{
//Diamond = 0x10,  // 方块
//Club = 0x20,        // 梅花
//Heart = 0x30,      // 红心
//Spade = 0x40,     // 黑头 
//Joker = 0x50      // 小王
//};

Card  IntToCard(int nCard)
{
	Card::Suit s;
	unsigned char n;

	n = nCard%100;

	if(nCard == 14 || nCard == 15)
	{
		s = Card::Joker;
		n = nCard;
		return Card(s,n);
	}
	else if(nCard/100 == 0 ) //黑桃  
	{
		s = Card::Spade;
		if(n == 1 || n == 2) 
		{
			n = 11 + n; // 12 ,13
		}
		else 
		{
			n = n - 2;  // 3到K是从1到11
		}
	}
	else if(nCard/100 == 1)// 1:红心
	{
		s = Card::Heart;
		if(n == 1 || n == 2) 
		{
			n = 11 + n; // 12 ,13
		}
		else 
		{
			n = n - 2;  // 3到K是从1到11
		}
	}
	else if(nCard/100 == 2) // 2:梅花
	{
		s = Card::Club;
		if(n == 1 || n == 2) 
		{
			n = 11 + n; // 12 ,13
		}
		else 
		{
			n = n - 2;  // 3到K是从1到11
		}
	}
	else if(nCard/100 == 3) //3:方块
	{
		s = Card::Diamond;
		if(n == 1 || n == 2) 
		{
			n = 11 + n; // 12 ,13
		}
		else 
		{
			n = n - 2;  // 3到K是从1到11
		}
	}
	return Card(s,n);
}

int CardToInt(Card card)
{
	unsigned char suit = card.suit();
	unsigned char nubmer = card.number();
	// 图片编号规律: 100 *　花色　＋  牌大小(1~15)
	int n1,n2;

	//Diamond = 0x10,  // 方块
	//Club = 0x20,        // 梅花
	//Heart = 0x30,      // 红心
	//Spade = 0x40,     // 黑头 
	//Joker = 0x50      // 小王

	// 0:黑桃
	// 1:红心
	// 2:梅花
	// 3:方块

	if(suit == Card::Diamond)
		n1 = 3;
	else if(suit == Card::Club)
		n1 = 2;
	else if(suit == Card::Heart)
		n1 = 1;
	else if(suit == Card::Spade)
		n1= 0;
	else if(suit == Card::Joker)
		n1= 0;


	if(nubmer>=1 && nubmer <=11)
	{// 3 ~ k
		n2 = nubmer+2;
	}
	else if(nubmer == 12 || nubmer == 13)
	{// 1 2 
		n2 = nubmer-11;
	}
	else if(nubmer == 14 || nubmer== 15)
	{// joker 
		n2 = nubmer;
	}
	return n1 * 100 + n2;
}


