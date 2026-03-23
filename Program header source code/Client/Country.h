#pragma once 


class CCountry
{
public:
	// constructor 
	explicit CCountry();

	// destructor 
	~CCountry();

	/// get instance pointer
	static CCountry* Instance();

        
	/////////////////////////////////////////////////////////////////////////
        // When entering game again("again" means logout game to role selection,
	// and login game again,but not login after exit ),call reset() ,because
	//  this is a a global variable 
	void Reset();

	/////////////////////////////// countrylist///////////////////////////////
	// get 
	int GetCountrySize();

	// query 
	BOOL GetCountryName(DWORD dwCountryId,char * pOutter);

	BOOL CheckCountry( DWORD dwCountryID,const char * pInner );

	BOOL ContainsCountry(const char * pInner);
        

	typedef std::vector<Country_Info>  COUNTRYLIST;
	typedef Country_Info  stCountry;
	const COUNTRYLIST  & GetCountryList();

	DWORD  GetCountryID(const char * pInner);
	void  SetCountryList(stCountryInfoUserCmd* pCmd);

	BOOL IsCountryCanLogin(const char *pCountryName);

	/////////////////////////////// countrystartlist///////////////////////////////////
	typedef std::vector<stCountryStar> COUNTRYSTARLIST;
        
	// get 
	const COUNTRYSTARLIST & GetCountryStartList();
	void  UpdateCountryStarList(stUpdateCountryStarCmd * pCmd);

	BOOL  GetCountryStar(DWORD dwCountryID,DWORD & dwOutter);

	
	////////////////////////////////savebox//////////////////////////////////////
	struct stCountryObject
	{
		DWORD dwCountry;
		DWORD qwGold;    // 当前国库银子数
		DWORD qwStock;   // 原料
		DWORD qwMaterial; // 物资
	};
 	 
	stCountryObject & GetObject();
	void SetObject(stFiskCountryUserCmd * cmd);


	/////////////////////////////tax/////////////////////////////////////////////
	void SetTax(DWORD  dwTax);
	DWORD GetTax();

	void  RuquestTax();
	void RequestCountrySaveBox();
	DWORD GetRevenue(float fMoney);


	/////////////////////////////////////////////////////////////////////////////
	///////////////////////////countrywarflag///////////////////////////////////
	struct stCountryWarFlag
	{
		DWORD dwDareCountry; // 敌对国家ID
		BYTE  byStatus; // 1为进入国战，0为退出国战   
		BYTE  byResult; // 对战结果，1为胜方，0为失败方,当byStatus为0时，该字段有意义
		stCountryWarFlag()
		{
			dwDareCountry = 0;
			byStatus  = 0;   
			byResult = 0;
		}
		stCountryWarFlag(const stCountryWarFlag & war)
		{
			dwDareCountry = war.dwDareCountry;
			byStatus = war.byStatus;
			byResult = war.byResult;
		}
	};	 
	struct stDareCountryInfo
	{
		std::vector<stCountryWarFlag> vecDareCountryWarFlag;

		void UpdateCountryWarFlag(stSetCountryDareUserCmd * cmd)
		{ 
			for(size_t i = 0 ; i < vecDareCountryWarFlag.size(); ++i)
			{
				stCountryWarFlag & war = vecDareCountryWarFlag[i];
				if( war.dwDareCountry == cmd->dwCountry)
				{        
					war.byResult = cmd->byResult;
					war.byStatus = cmd->byStatus;
					return;
				}
			}
			stCountryWarFlag __war;
			__war.byResult = cmd->byResult;
			__war.dwDareCountry = cmd->dwCountry;
			__war.byStatus = cmd->byStatus;
			vecDareCountryWarFlag.push_back(__war);
		}

		bool GetDareCountryInfo(DWORD dwCountry,stCountryWarFlag & warflag)
		{
			for(size_t i = 0; i < vecDareCountryWarFlag.size(); ++i)
			{
				if(vecDareCountryWarFlag[i].dwDareCountry == dwCountry)
				{
					warflag = vecDareCountryWarFlag[i];
					return true;
				}
			}
			return false;
		}

		bool IsCountryInWar(DWORD dwCountry)
		{
			stCountryWarFlag warflag;
			if( GetDareCountryInfo(dwCountry,warflag))
			{
				return (warflag.byStatus == (BYTE)1);			
			}
			return false;
		}

		bool IsAnyCountryInWar()
		{
			for(size_t i = 0; i < vecDareCountryWarFlag.size(); ++i)
			{
				if(vecDareCountryWarFlag[i].byStatus == (BYTE)1)
				{
					return true;
				}
			}
			return false;
		}

		void Reset()
		{
			vecDareCountryWarFlag.clear();
		}
	};
	void UpdateCountryWarFlag(stSetCountryDareUserCmd * cmd);
	stDareCountryInfo & GetCountryWarInfo()
	{
		return m_WarInfo;
	};
        BOOL  IsCountryWarState();
	DWORD IsDareCountryInWar(DWORD dwCountry) 
	{
	     return m_WarInfo.IsCountryInWar(dwCountry);
	};
	/////////////////////////////////////////////////////////////////////////////


	//king  更新国家国王
	std::string & GetKing();
	//soke  更新国家-捕头(左)
	std::string & GetCatcher();
	//soke  更新国家-捕头(右)
	std::string & GetCatchex();
	//soke 2016 - 3 - 11  更新国家-御史大夫(左)
	std::string & GetCensor();
	//soke 2016 - 3 - 11  更新国家-御史大夫(右)
	std::string & GetCensos();
	//soke 2016 - 3 - 11  更新国家-元帅
	std::string & GetYuanshuai();
	//soke  更新国家-宰相
	std::string & GetZaixiang();

	//soke 设置主角所在国家的国王名字
	void  SetKing(stUpdateCountryKingUserCmd * pCmd);
	//soke 设置主角所在国家的捕头（左）名字
	void  SetCatcher(stUpdateCountryCatcherUserCmd * pCmd);
	//soke 设置主角所在国家的捕头（右）名字
	void  SetCatchex(stUpdateCountryCatchexUserCmd * pCmd);
	//soke 2016 - 3 - 11 设置主角所在国家的御史大夫(左)名字
	void  SetCensor(stUpdateCountryCensorUserCmd * pCmd);
	//soke 2016 - 3 - 11 设置主角所在国家的御史大夫(右)名字
	void  SetCensos(stUpdateCountryCensosUserCmd * pCmd);
	//soke 2016 - 3 - 11 设置主角所在国家的元帅名字
	void  SetYuanshuai(stUpdateCountryYuanshuaiUserCmd * pCmd);
	//soke 设置主角所在国家的宰相名字
	void  SetZaixiang(stUpdateCountryZaixiangUserCmd * pCmd);

	bool	IsEmperorAt() { return m_bIsEmperor; }
      

	// ally country 
	BOOL GetAllyCountry(DWORD & dwOutterAllyCountry);
	void SetAllyCountry(const stUpdateCountryAlly * pCmd);

	void QuestAllyCountryInfo();
	void SetAllyCountryInfo(const stRtnCountryAllyInfo  * pCmd);

	DWORD GetAllyCountryFriendDegree()
	{
		return m_dwFriendDegree;
	};
	BOOL IsAllyCountryAndInWar(DWORD dwCountry);
	BOOL IsAllyCountry(DWORD dwCountry){return (m_dwAllyCountry == dwCountry);};
	BOOL IsAllyCountryAsName(const char * pCountryNameWrapper);

	BOOL GetDefaultCountryID(DWORD & dwOutter);

protected:
	static CCountry* Singleton;
private:
	
	// Countrylist
	COUNTRYLIST     m_vecCountryList;
	DWORD               m_dwDefaultCountryID;

	// Countrystarlist
	COUNTRYSTARLIST m_vecCountryStarList;


	// save box 
	stCountryObject m_object;

	// tax 
	DWORD m_dwTax;

	// country war 
	stDareCountryInfo  m_WarInfo;

	std::string  m_strKing;    //soke 国家官员 - 国王

	std::string  m_strCatcher;   //soke 2016 - 3 - 11 国家官员 - 捕头（左）
	std::string  m_strCatchex;   //soke 2016 - 3 - 11 国家官员 - 捕头（右）
	std::string  m_strCensor;    //soke 2016 - 3 - 11 国家官员 - 御史大夫（左）
	std::string  m_strCensos;    //soke 2016 - 3 - 11 国家官员 - 御史大夫（右）
	std::string  m_strYuanshuai; //soke 2016 - 3 - 11 国家官员 - 元帅
	std::string  m_strZaixiang;  //soke 2016 - 3 - 11 国家官员 - 宰相

	bool	m_bIsEmperor;

	DWORD m_dwAllyCountry;
	DWORD m_dwFriendDegree;

public:
	typedef std::vector<stSeptSortInfo> SEPTSORTINFO;
	typedef std::vector<stUnionSortInfo> UNIONSORTINFO;
private:
	// family sort list 	
	SEPTSORTINFO m_vecNativeSeptSortInfo;
	SEPTSORTINFO m_vecWorldSeptSortInfo;
        // sept sort list
	UNIONSORTINFO m_vecNativeUnionSortInfo;
	UNIONSORTINFO m_vecWorldUnionSortInfo;
public:
	void  SetSeptSort(const stRtnSeptSort* pCmd);
	void  SetUnionSort(const stRtnUnionSort * pcmd);
	SEPTSORTINFO & GetSeptNativeSort(){return m_vecNativeSeptSortInfo;};
	SEPTSORTINFO & GetSeptWorldSort(){return m_vecWorldSeptSortInfo;};

	UNIONSORTINFO &GetUnionNativeSort(){return m_vecNativeUnionSortInfo;};
	UNIONSORTINFO &GetUnionWorldSort(){return m_vecWorldUnionSortInfo;};

};

inline 
CCountry::stCountryObject & CCountry::GetObject()
{
	return m_object;
}

inline  
CCountry* CCountry::Instance()
{
	Assert(Singleton != 0);
	return Singleton;
}

inline
int  CCountry::GetCountrySize()
{
       return m_vecCountryList.size();
}

inline 
const CCountry::COUNTRYLIST  & CCountry::GetCountryList()
{
      return m_vecCountryList;
}

inline 
const CCountry::COUNTRYSTARLIST & CCountry::GetCountryStartList()
{
      return m_vecCountryStarList;
}


inline 
void CCountry::SetTax(DWORD  dwTax)
{
	m_dwTax = dwTax;
}

inline 
DWORD CCountry::GetTax()
{
	return m_dwTax;
}

inline
DWORD CCountry::GetRevenue(float fMoney)
{
	return  (DWORD)(fMoney * m_dwTax/100 + 0.5 );
}

//soke 国家官员 - 国王
inline 	
std::string & CCountry::GetKing()
{
	return m_strKing;
}

//soke 国家官员 - 捕头（左）
inline 	
std::string & CCountry::GetCatcher()
{
	return m_strCatcher;
}

//soke 国家官员 - 捕头（右）
inline 	
std::string & CCountry::GetCatchex()
{
	return m_strCatchex;
}

//soke 2016 - 3 - 11 国家官员 - 御史大夫（左）
inline 	
std::string & CCountry::GetCensor()
{
	return m_strCensor;
}

//soke 国家官员 - 御史大夫（右）
inline 	
std::string & CCountry::GetCensos()
{
	return m_strCensos;
}

//soke 国家官员 - 元帅
inline 	
std::string & CCountry::GetYuanshuai()
{
	return m_strYuanshuai;
}

//soke 国家官员 - 宰相
inline 	
std::string & CCountry::GetZaixiang()
{
	return m_strZaixiang;
}
