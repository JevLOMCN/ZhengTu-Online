#pragma once
 
 
///////////////////////////////////////////////////////////////
// help funciton
int SeatToDirection(BYTE seat,BYTE selfseat);
int DirectionToSeat(BYTE direction,BYTE selfseat);

Card  IntToCard(int nCard);
int   CardToInt(Card card);
///////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
enum eDirection
{
	eDir_Left = 1,
	eDir_Down = 2,
	eDir_Right = 3,
};


enum ePlayerState
{
	ePlayerState_Preparing,           // 准备
	ePlayerState_DispenseCard = 0x01, // 发牌
	ePlayerState_ContestPoint = 0x02,  // 叫分
	ePlayerState_ExposureCard,         // 亮牌
	ePlayerState_Playing = 0x04,   // 出牌中
	ePlayerState_Waiting,        // 等待中(这个状态就是一直等,直到有这个动作完成或者空闲等待等到新动作来)

};

class CCard;
// 一般玩家
class CPlayer
{
public:
	CPlayer();
	virtual ~CPlayer();

	virtual void Draw(int nOffsetX,int nOffsetY);

	void SetDir(eDirection dir)
	{
		m_dir = dir;
	}

	eDirection  GetDir()
	{
		return m_dir;
	}

	void AddCard(int nCard);   // 加牌
	void RemoveCard(int nCard);   // 出牌

	int GetCards() // 得到当前手中的牌数
	{
		return m_vecCards.size();
	}

	
	std::vector<CCard *> & GetCardList()
	{
		return m_vecCards;
	}

	CCard * GetCard(int i)
	{
		if(i < 0 || i >= m_vecCards.size())
			return NULL;
		return m_vecCards[i];
	}

	void ClearCard();
	void SetCurNum(int nCurNum)
	{
		m_nCurNums = nCurNum;
	}
	int GetCurNum()
	{
		return m_nCurNums;
	}

	void SetName(const char * pName)
	{
		m_strName = pName;
	}

	virtual void DrawName(int nOffsetX,int nOffsetY);
private:
	eDirection  m_dir;
	
	int m_nCurNums;  // 当前手上拥有的牌数(根据规则客户端自己计算出)
           
	std::vector<CCard *> m_vecCards; //  每个玩家刚打出去的牌,牌id为1,2,3,4, .....13,14(小王),15(大王)

	std::string m_strName;
};


// 主角
class CMainPlayer : public CPlayer
{
public:
	CMainPlayer();
	~CMainPlayer();

 
	void Draw();

	CCard * GetPlayingCard(int i)
	{
		if(i < 0 || i >= m_vecPlayingCards.size())
			return NULL;
		return m_vecPlayingCards[i];
	}

	int GetPlayingCards() // 得到当前手中的牌数
	{
		return m_vecPlayingCards.size();
	}
	
	void AddPlayingCard(int nCard);   // 加牌
	void RemovePlayingCard(int nCard);   // 出牌

	std::vector<CCard *> & GetPlayingCardList()
	{
		return m_vecPlayingCards;
	}

	void SortPlayingCard(bool bUp);
	void ClearPlayingCard();

	CCard * GetLastPlayingCard();
	
private:
	std::vector<CCard *> m_vecPlayingCards; //  主角玩家拥有一堆牌,牌id为1,2,3,4, .....13,14(小王),15(大王)      

};
