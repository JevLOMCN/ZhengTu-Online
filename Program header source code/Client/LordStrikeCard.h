#pragma once

 

// ≈∆¿‡
class CCard
{
public:
	CCard();
	~CCard();

	// card id
	int GetCardID() const 
	{
		return m_nCardID; 
	}
	void SetCardID(int nCardID);

	//bool operator==(const CCard &c) const
	//{
	//	 int nCardID = c.GetCardID() % 100;
	//	 if( GetCardID() == nCardID)
	//		 return true;
	//	 return false;
	//}
	//bool operator<(const CCard &c) const
	//{
	//	int nCarID1 = c.GetCardID() % 100;
	//	// must first decide the 14 and 15,else 13 + 1 == 14,so this 14
	//	// is the last or the add
	//	if(nCarID1 == 14 || nCarID1 == 15)
	//		nCarID1 += 2;
	//	if(nCarID1 == 1 || nCarID1 == 2)
	//		nCarID1 += 13;

	//	int nCarID2 = GetCardID();
	//	if(nCarID2 == 14 || nCarID2 == 15)
	//		nCarID2 += 2;
	//	if(nCarID2 == 1 || nCarID2 == 2)
	//		nCarID2 += 13;

	//	
	//	return nCarID2 < nCarID1;
	//}
	//bool operator>(const CCard &c) const
	//{
	//	int nCarID1 = c.GetCardID() % 100;
	//	// must first decide the 14 and 15,else 13 + 1 == 14,so this 14
	//	// is the last or the add
	//	if(nCarID1 == 14 || nCarID1 == 15)
	//		nCarID1 += 2;
	//	if(nCarID1 == 1 || nCarID1 == 2)
	//		nCarID1 += 13;

	//	int nCarID2 = GetCardID();
	//	if(nCarID2 == 14 || nCarID2 == 15)
	//		nCarID2 += 2;
	//	if(nCarID2 == 1 || nCarID2 == 2)
	//		nCarID2 += 13;


	//	return nCarID2 > nCarID1;
	//}

	// operator = 
	CCard& operator = (const CCard& xCard);

	// resource location
	void SetResource(const stResourceLocation &rl);
	const stResourceLocation &  GetResource(){
		return m_rl;
	}

	// color-blend
	void SetColor(DWORD dwColor){
		m_CurrentColor = dwColor;
	}
	void SetBlendType(BlendType type){
		m_BlendType = type;
	}
	
        // render 
	void Render(int x,int y);
	void Render(int x,int y,const RECT* rcClip,const stPointF* scale);

	void Render(int x,int y,DWORD color);
	void Render(int x,int y,const RECT* rcClip,const stPointF* scale,DWORD color);

	// size-info 
	int GetWidth();
	int GetHeight();
	int GetAlpha(POINT pt);



	// GetBitmap()
	IBitmap* GetImage(){ return m_pImage;}

	bool IsNullImage();

	void SetSelectState(bool bSelect)
	{
		m_bSelected = bSelect;
	}
	bool IsSelectState()
	{
		return m_bSelected;
	}
private:
	bool m_bSelected;
	int                  m_nCardID;
	stResourceLocation   m_rl;

	IBitmap* m_pImage;
	D3DXCOLOR m_CurrentColor;
	BlendType m_BlendType;

};

template <bool bUp>
class CardCompare
{
public:
	int operator()(const CCard * pCard1,const CCard * pCard2)
	{
		int nNum1 = pCard1->GetCardID()%100;
		
		if( nNum1 == 14 || nNum1 == 15)
			nNum1 += 2;
		if(nNum1 == 1 || nNum1 == 2)
			nNum1 +=13;
		int nNum2 = pCard2->GetCardID()%100;
	
		if( nNum2 == 14 || nNum2 == 15)
			nNum2 += 2;
		if(nNum2 == 1 || nNum2 == 2)
			nNum2 +=13;	

		if(bUp)
			return nNum1 > nNum2;
		else 
			return nNum1 < nNum2;
	}
};