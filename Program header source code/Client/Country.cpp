/*
    Author: 
    国家相关信息，包含：
          国家列表，
	  对应星级列表，
	  税率,
	  国库,
	  设置国战的对战状态（头顶显示"国战",播放国战音乐）
*/
#include "public.h"
#include "./Country.h"
#include "./GameScene.h"
#include "./Game.h"
#include "../Media/Music.h"

////////////////////////////////////////////////////////////////
// 用static禁止extern这个变量,以便只能通过Singeleton获取
static   CCountry  s_country;
CCountry* CCountry::Singleton = 0;
////////////////////////////////////////////////////////////

#define INVALID_COUNTRY_ID 0xFFFFFFFF

CCountry::CCountry()
{
	FUNCTION_BEGIN;
	Assert(0 == Singleton);
	Singleton = this;

	m_vecCountryList.clear();
	m_strKing.clear();    //soke 清除国家官员 - 国王

	m_strCatcher.clear();   //soke 2016 - 3 - 11 清除国家官员 - 捕头（左）
	m_strCatchex.clear();   //soke 2016 - 3 - 11 清除国家官员 - 捕头（右）
	m_strCensor.clear();    //soke 2016 - 3 - 11 清除国家官员 - 御史大夫（左）
	m_strCensos.clear();    //soke 2016 - 3 - 11 清除国家官员 - 御史大夫（右）
	m_strYuanshuai.clear(); //soke 2016 - 3 - 11 清除国家官员 - 元帅
	m_strZaixiang.clear();  //soke 2016 - 3 - 11 清除国家官员 - 宰相

	m_bIsEmperor = false;

	Reset();

	m_dwAllyCountry = INVALID_COUNTRY_ID;
	m_dwDefaultCountryID  = INVALID_COUNTRY_ID;

	FUNCTION_END;
}


CCountry::~CCountry()
{
	FUNCTION_BEGIN;
	Singleton = 0;
	FUNCTION_END;
}

void CCountry::Reset()
{
	FUNCTION_BEGIN;

	m_dwTax = 0;
	
	m_vecCountryStarList.clear();

	m_object.dwCountry = 0;
 
	 
	m_object.qwGold = 0;
	 
	m_object.qwMaterial = 0;
 
	m_object.qwStock = 0;

	m_WarInfo.Reset();

	FUNCTION_END;
}

//--------------------------------------------------------------------
//   得到盟国国家id
//--------------------------------------------------------------------
BOOL CCountry::GetAllyCountry(DWORD & dwOutterAllyCountry)
{
        if(m_dwAllyCountry == INVALID_COUNTRY_ID)
		return FALSE;
	else 
	{
		dwOutterAllyCountry = m_dwAllyCountry;
		return TRUE;
	}
}

//-------------------------------------------------------------------
// 是否盟国而且主角处于国战状态
//-------------------------------------------------------------------
BOOL CCountry::IsAllyCountryAndInWar(DWORD dwCountryID)
{
	return (m_dwAllyCountry == dwCountryID) && (m_WarInfo.IsAnyCountryInWar());
}

//-------------------------------------------------------------------
// 指定国家是否盟国
// Note:
//  pCountryName 可以是"{宋国xxx}"或者"宋国"等任意包含字符串
//-------------------------------------------------------------------
BOOL CCountry::IsAllyCountryAsName(const char * pCountryNameWrapper)
{
	if(m_dwAllyCountry == INVALID_COUNTRY_ID)
		return FALSE;

	for(std::vector<Country_Info>::iterator it = m_vecCountryList.begin(); it != m_vecCountryList.end() ;++it)
	{
		if ( strstr( pCountryNameWrapper,(*it).pstrName) != 0 )
		{
			if( (*it).id == m_dwAllyCountry) 
				return TRUE;
			else 
				return FALSE;
		}
	}
	return FALSE;
}

//--------------------------------------------------------------------
//    服务器设置盟国id
//--------------------------------------------------------------------
void CCountry::SetAllyCountry(const stUpdateCountryAlly * pCmd)
{
	m_dwAllyCountry = pCmd->dwAllyCountryID;   
	if( m_dwAllyCountry == 0)  // 服务器无盟国用0,客户端用INVALID_COUNTRY_ID
		m_dwAllyCountry = INVALID_COUNTRY_ID;
}

//--------------------------------------------------------------------
//    得到盟国国家信息
//--------------------------------------------------------------------
void CCountry::QuestAllyCountryInfo()
{
	stReqCountryAllyInfo  cmd;
	SEND_USER_CMD(cmd);
		
}
//--------------------------------------------------------------------
//    服务器设置盟国信息
//--------------------------------------------------------------------
void CCountry::SetAllyCountryInfo(const stRtnCountryAllyInfo  * pCmd)
{
       m_dwAllyCountry =  pCmd->dwAllyCountryID;
       m_dwFriendDegree = pCmd->dwFriendDegree;
}


//--------------------------------------------------------------------
//   从服务器接收到国家列表
//--------------------------------------------------------------------
void  CCountry::SetCountryList(stCountryInfoUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	m_vecCountryList.clear();

	for (int i=0;i<pCmd->size;++i)
	{
		stCountry st;
		st.id = pCmd->countryinfo[i].id;
		if(i == 0)
			m_dwDefaultCountryID = st.id;
		st.enableRegister = pCmd->countryinfo[i].enableRegister;
		st.enableLogin = pCmd->countryinfo[i].enableLogin;
		strncpy(st.pstrName,pCmd->countryinfo[i].pstrName,sizeof(st.pstrName));
		m_vecCountryList.push_back(st);
	}

	FUNCTION_END;
}

//--------------------------------------------------------------------
//    得到默认国家id
//--------------------------------------------------------------------
BOOL CCountry::GetDefaultCountryID(DWORD & dwOutter)
{
	FUNCTION_BEGIN;

	if( INVALID_COUNTRY_ID == m_dwDefaultCountryID )
		return FALSE;
	else 
	{
		dwOutter  = m_dwDefaultCountryID;
		return TRUE;
	}

	FUNCTION_END;
}

//--------------------------------------------------------------------
//    对应国家id是否能登陆
//--------------------------------------------------------------------
BOOL CCountry::IsCountryCanLogin(const char *pCountryName)
{
	FUNCTION_BEGIN;

	for(std::vector<Country_Info>::iterator it = m_vecCountryList.begin();
		it != m_vecCountryList.end() ;++it)
	{
		if ( strcmp( (*it).pstrName, pCountryName ) == 0 )
		{
			if(it->enableLogin)
				return TRUE;
			else 
				return FALSE;
		}
	}
	return FALSE;

	FUNCTION_END;
}

//--------------------------------------------------------------------
//    查找对应国家id的国家名
//--------------------------------------------------------------------
BOOL CCountry::GetCountryName(DWORD dwCountryId,char * pOutter)
{
	FUNCTION_BEGIN;

	assert(pOutter != NULL);

	for(std::vector<Country_Info>::iterator it = m_vecCountryList.begin();
		it != m_vecCountryList.end() ;++it)
	{
		if(it->id == dwCountryId )
		{
			strcpy(pOutter,it->pstrName);
			return TRUE;
		}
	}
	return FALSE;
        
	FUNCTION_END;
}
 

//------------------------------------------------------------------------
//   指定id国家的国家名是否是pInner 
//------------------------------------------------------------------------
BOOL CCountry::CheckCountry( DWORD dwCountryID,const char * pInner )
{
	FUNCTION_BEGIN;


	std::vector<Country_Info>::iterator itor;
	for( itor=m_vecCountryList.begin(); itor!=m_vecCountryList.end(); ++itor )
	{
		if ( (*itor).id == dwCountryID )
		{
			if ( strcmp( (*itor).pstrName, pInner ) == 0 )
				return TRUE;
			else
				return FALSE;
		}		
	}

	return FALSE;

	FUNCTION_END;
}

//------------------------------------------------------------------------
//     指定名字的国家是否在国家列表中
//------------------------------------------------------------------------
BOOL CCountry::ContainsCountry(const char * pInner)
{
	FUNCTION_BEGIN;

	std::vector<Country_Info>::iterator itor;
	for( itor=m_vecCountryList.begin(); itor!=m_vecCountryList.end(); ++itor )
	{
		if ( strcmp( (*itor).pstrName, pInner ) == 0 )
				return TRUE;
			
	}
	
	return FALSE;

	FUNCTION_END;
}

//------------------------------------------------------------------------
//    得到指定国家名的id
//------------------------------------------------------------------------
DWORD  CCountry::GetCountryID(const char * pInner)
{
	FUNCTION_BEGIN;

	std::vector<Country_Info>::iterator itor;
	for( itor=m_vecCountryList.begin(); itor!=m_vecCountryList.end(); ++itor )
	{
		if ( strcmp( (*itor).pstrName, pInner ) == 0 )
			return (*itor).id;
	}
	// 国家列表里没有，为中立国家(服务器端写死，为6)
	return 6;

	FUNCTION_END;
}


//---------------------------------------------------------------------------
//    功能: 更新国家星级列表
//    注意: 当某个国家星级有变时也用这个消息，所以函数中不要先清空列表后再收
//---------------------------------------------------------------------------
void CCountry::UpdateCountryStarList(stUpdateCountryStarCmd * pCmd)
{
        FUNCTION_BEGIN;

	if( pCmd )
	{
		for (int i=0;i<pCmd->dwSize;i++)
		{
			COUNTRYSTARLIST::iterator iter;
			for(iter = m_vecCountryStarList.begin() ; iter != m_vecCountryStarList.end() ; ++iter )
			{
				if( iter->dwCountry == pCmd->data[i].dwCountry)
				{
					iter->dwStar = pCmd->data[i].dwStar;
					GetScene()->UpdateCountryStar(pCmd->data[i].dwCountry,pCmd->data[i].dwStar);
					break;
				}
			}
			if( iter == m_vecCountryStarList.end())
			{
				stCountryStar star;
				star.dwCountry = pCmd->data[i].dwCountry;
				star.dwStar = pCmd->data[i].dwStar;
				m_vecCountryStarList.push_back(star);
				GetScene()->UpdateCountryStar(pCmd->data[i].dwCountry,pCmd->data[i].dwStar);
			}			
		}
	}   

	FUNCTION_END;
}

//---------------------------------------------------------------------------
//     得到指定国家的星级
//---------------------------------------------------------------------------
BOOL CCountry::GetCountryStar(DWORD dwCountryID,DWORD & dwOutter)
{
	FUNCTION_BEGIN;

	COUNTRYSTARLIST::iterator iter;
	for(iter = m_vecCountryStarList.begin() ; iter != m_vecCountryStarList.end() ; ++iter )
	{
		if( iter->dwCountry == dwCountryID)
		{
		        dwOutter = iter->dwStar;
			return TRUE;
		}
	}

	return FALSE;

	FUNCTION_END;
}

//---------------------------------------------------------------------------
//     设置国家仺库信息
//---------------------------------------------------------------------------
void CCountry::SetObject(stFiskCountryUserCmd * cmd)
{
	FUNCTION_BEGIN;

	m_object.dwCountry = cmd->dwCountry;	 
	m_object.qwGold = cmd->qwGold;
	m_object.qwStock = cmd->qwStock;// 原料
	m_object.qwMaterial = cmd->qwMaterial;// 物资

	FUNCTION_END;
}

//---------------------------------------------------------------------------
//     设置主角所在国家的国王名字
//---------------------------------------------------------------------------
void CCountry::SetKing(stUpdateCountryKingUserCmd * pCmd)
{
	FUNCTION_BEGIN;

        m_strKing = pCmd->kingName;
		m_bIsEmperor = (pCmd->isEmperor == 1);

	FUNCTION_END;
}

//---------------------------------------------------------------------------
//soke 设置主角所在国家的捕头（左）名字
//---------------------------------------------------------------------------
void CCountry::SetCatcher(stUpdateCountryCatcherUserCmd * pCmd)
{
	FUNCTION_BEGIN;

        m_strCatcher = pCmd->catcherName;
	//	m_bIsEmperor = (pCmd->isEmperor == 1);

	FUNCTION_END;
}

//---------------------------------------------------------------------------
//soke 设置主角所在国家的捕头（右）名字
//---------------------------------------------------------------------------
void CCountry::SetCatchex(stUpdateCountryCatchexUserCmd * pCmd)
{
	FUNCTION_BEGIN;

        m_strCatchex = pCmd->catchexName;
	//	m_bIsEmperor = (pCmd->isEmperor == 1);

	FUNCTION_END;
}

//---------------------------------------------------------------------------
//soke 设置主角所在国家的御史大夫（左）名字
//---------------------------------------------------------------------------
void CCountry::SetCensor(stUpdateCountryCensorUserCmd * pCmd)
{
	FUNCTION_BEGIN;

        m_strCensor = pCmd->censorName;
	//	m_bIsEmperor = (pCmd->isEmperor == 1);

	FUNCTION_END;
}

//---------------------------------------------------------------------------
//soke 设置主角所在国家的御史大夫（右）名字
//---------------------------------------------------------------------------
void CCountry::SetCensos(stUpdateCountryCensosUserCmd * pCmd)
{
	FUNCTION_BEGIN;

        m_strCensos = pCmd->censosName;
	//	m_bIsEmperor = (pCmd->isEmperor == 1);

	FUNCTION_END;
}

//---------------------------------------------------------------------------
//soke 设置主角所在国家的元帅名字
//---------------------------------------------------------------------------
void CCountry::SetYuanshuai(stUpdateCountryYuanshuaiUserCmd * pCmd)
{
	FUNCTION_BEGIN;

        m_strYuanshuai = pCmd->yuanshuaiName;
	//	m_bIsEmperor = (pCmd->isEmperor == 1);

	FUNCTION_END;
}

//---------------------------------------------------------------------------
//soke 设置主角所在国家的宰相名字
//---------------------------------------------------------------------------
void CCountry::SetZaixiang(stUpdateCountryZaixiangUserCmd * pCmd)
{
	FUNCTION_BEGIN;

        m_strZaixiang = pCmd->zaixiangName;
	//	m_bIsEmperor = (pCmd->isEmperor == 1);

	FUNCTION_END;
}

//---------------------------------------------------------------------------
//     请求当场场景所在国家的税率
//---------------------------------------------------------------------------
void  CCountry::RuquestTax()
{
	FUNCTION_BEGIN;

	stTaxCountryUserCmd cmd;
	cmd.dwCountry  = GetCountryID(GetScene()->GetMapCountryName());
	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

//---------------------------------------------------------------------------
//     请求当场场景所在国家的国库信息
//---------------------------------------------------------------------------
void CCountry::RequestCountrySaveBox()
{
	FUNCTION_BEGIN;

	stFiskCountryUserCmd cmd;
	cmd.dwCountry = GetCountryID(GetScene()->GetMapCountryName());
	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

//---------------------------------------------------------------------------
//     设置国战的对立国家状态,播放国战音乐
//---------------------------------------------------------------------------
void CCountry::UpdateCountryWarFlag(stSetCountryDareUserCmd * cmd)
{
	FUNCTION_BEGIN;

	m_WarInfo.UpdateCountryWarFlag(cmd);
	GetScene()->UpdateCountryWarFlag();

	if( cmd->byStatus)
	{
		// 国战开始了
		PlayBackMusic("CWarStart.mp3",0,0,-1,true); //

		// 国战开始音乐播放完后,开始播放国家背景音乐
		PlayBackMusic("CWarBj.mp3",0,0,0,false);
	}
	else
	{
		// 国战结束了
		if( cmd->byResult)
			// 成功			
			PlayBackMusic("CWarOk.MP3",0,0,-1);  
		else 
			// 失败
			PlayBackMusic("CWarNo.MP3",0,0,-1);
	}

	FUNCTION_END;
}

//---------------------------------------------------
// 主角所在国家是否国战中
//---------------------------------------------------
BOOL  CCountry::IsCountryWarState()
{
	stDareCountryInfo & dareinfo = CCountry::Instance()->GetCountryWarInfo();
	if(dareinfo.IsAnyCountryInWar())
		return TRUE;
	else 
		return FALSE;
	
	return true;
}

//---------------------------------------------------
//---------------------------------------------------
void  CCountry::SetSeptSort(const stRtnSeptSort* pCmd)
{
	FUNCTION_BEGIN;

 	if( pCmd->byType == COUNTRY_SEPT_SORT)
	{
		m_vecNativeSeptSortInfo.clear();
		for (int i=0;i<pCmd->dwSize;++i)
		{
			stSeptSortInfo st;

			strncpy(st.septName,pCmd->data[i].septName,sizeof(st.septName));
			st.dwRepute =  pCmd->data[i].dwRepute;
			st.dwOrder =  pCmd->data[i].dwOrder;
			st.dwCountryID =  pCmd->data[i].dwCountryID;

			m_vecNativeSeptSortInfo.push_back(st);
		}
	}
	else if( pCmd->byType == WORLD_SEPT_SORT)
	{
		m_vecWorldSeptSortInfo.clear();
		for (int i=0;i<pCmd->dwSize;++i)
		{
			stSeptSortInfo st;

			strncpy(st.septName,pCmd->data[i].septName,sizeof(st.septName));
			st.dwRepute =  pCmd->data[i].dwRepute;
			st.dwOrder =  pCmd->data[i].dwOrder;
			st.dwCountryID =  pCmd->data[i].dwCountryID;

			m_vecWorldSeptSortInfo.push_back(st);
		} 
	}	

	FUNCTION_END;
}
void  CCountry::SetUnionSort(const stRtnUnionSort * pCmd)
{
	FUNCTION_BEGIN;

	if( pCmd->byType == COUNTRY_UNION_SORT)
	{

		m_vecNativeUnionSortInfo.clear();
		for (int i=0;i<pCmd->dwSize;++i)
		{
			stUnionSortInfo st;

			strncpy(st.unionName,pCmd->data[i].unionName,sizeof(st.unionName));
			st.dwMana =  pCmd->data[i].dwMana;
			st.dwOrder =  pCmd->data[i].dwOrder;
			st.dwCountryID =  pCmd->data[i].dwCountryID;

			m_vecNativeUnionSortInfo.push_back(st);
		}
	}
	else if( pCmd->byType == WORLD_UNION_SORT)
	{
		m_vecWorldUnionSortInfo.clear();
		for (int i=0;i<pCmd->dwSize;++i)
		{
			stUnionSortInfo st;

			strncpy(st.unionName,pCmd->data[i].unionName,sizeof(st.unionName));
			st.dwMana =  pCmd->data[i].dwMana;
			st.dwOrder =  pCmd->data[i].dwOrder;
			st.dwCountryID =  pCmd->data[i].dwCountryID;

			m_vecWorldUnionSortInfo.push_back(st);
		} 
	}	

	FUNCTION_END;
}