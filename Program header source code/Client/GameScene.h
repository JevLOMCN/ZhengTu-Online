#pragma once
#include "..\magic\mapscene.h"
#include <algorithm>
#include <functional>

//#include "Item.h"
//#include "MainCharacter.h"
//#include "Build.h"
#include "./Npc.h"

//*
//class CNpc;
class CCharacter;
class CMagic;
class CBuild;
class CItem;
class CMainCharacter;
//class CPet;

#include "../magic/SceneObject.h"
//*/
#include "../Magic/ObjectBase.h"
#include "../Magic/public.h"
#include "../Magic/iObj.h"
#include "./Pet.h"
#include "./Horse.h"
#include "./Auction.h"
#include "./CartoonPet.h"
#include "./SceneMovie.h"

struct stMapFunctionNpc {
	DWORD   dwID;
	char    szName[MAX_NAMESIZE];
	POINT   ptPos;
};

enum enumWetherType
{
	WETHER_T_RAIN,
	WETHER_T_SNOW,
	WETHER_T_CLOUD,
	WETHER_T_NUM
};
static const char* wetherName[] = { "rain", "snow", "cloud" };

typedef std::vector<stMapFunctionNpc>	tListMapNpc;

typedef std::vector<stPointI> POINT_LIST;
POINT_LIST GetPointList(int dir,const stPointF & screen,int num_arrow = 3,int nspace = 50);
POINT_LIST GetPointListSingleArrow(const stPointI& dir, const stPointF& screen, int num_obj = 3);

template<class tObjectType>
struct stFindObjectByID{
	DWORD dwFindID;
	stFindObjectByID(DWORD dwID)
		: dwFindID(dwID)
	{
	}
	bool operator () (tObjectType * pObject)
	{
		return ((CSceneObject*)pObject)->GetID() == dwFindID;
	}
};

template<class tObjectType>
struct stFindObjectByName{
	const char* m_pszName;
	stFindObjectByName(const char* pszName)
		: m_pszName(pszName)
	{
	}

	bool operator () (tObjectType * pObject)
	{
		return strcmp( ((CSceneObject*)pObject)->GetName(),m_pszName) == 0;
	}
};

template<class tObjectType>
struct stDeleteObject{
	void operator () (tObjectType * pObject)
	{
		delete pObject;
	}
};


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param aScreenID[9] : 描述
 * \param iScreenID : 描述
 * \return 返回值的描述
 */
inline bool IsHaveScreenID(int aScreenID[9],int iScreenID)
{
	for(size_t i=0;i<9;++i)
	{
		if(aScreenID[i] == iScreenID) return true;
	}
	return false;
}

extern int  compareObjectDistance(const void* p1,const void * p2);

extern const int PathZoneSize;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGameScene :
	public CMapScene
{
public:
	

	struct stPathZone{
		DWORD	 flags;// 跟周围八格的连通性

		DWORD GetFlags(POINT ptOffset)
		{
			ptOffset.x += 1;
			ptOffset.y += 1;
			int num = (ptOffset.y << 2) | ptOffset.x;
			return 1 << num;
		}

		bool IsBlock(POINT ptOffset)
		{
			return (flags & GetFlags(ptOffset)) != 0;
		}

		void SetBlock(POINT ptOffset,bool bBlock)
		{
			if(bBlock)
				flags |= GetFlags(ptOffset);
			else
				flags &= (~GetFlags(ptOffset));
		}

	};

	int			m_nPathZoneWidth;
	int			m_nPathZoneHeight;
	std::vector<stPathZone> m_aPathZone;

	enum enmRequestCmdType
	{
		RequestCmd_UnionName,
		RequestCmd_SeptName,
		RequestCmd_Num
	};
	struct tRequestUnionSeptName{
		DWORD	dwID;
		enmRequestCmdType type;
	};
	typedef std::deque<tRequestUnionSeptName>	tListRequestCmd;
	tListRequestCmd	m_queueRequestUnionSeptNameCmd;
	void AddRequestUnionSeptNameCmd(const enmRequestCmdType& type, const DWORD& dwID)
	{
		tRequestUnionSeptName cmd;
		cmd.dwID = dwID;
		cmd.type = type;
		m_queueRequestUnionSeptNameCmd.push_back( cmd );
	}
	void RunQueueUnionSept();
	
	POINT		GridPosToPathZonePos(POINT pos)
	{
		pos.x/=PathZoneSize;
		pos.y/=PathZoneSize;
		return pos;
	}

	POINT		PathZonePosToGridPos(POINT pos)
	{
		pos.x = pos.x * PathZoneSize + PathZoneSize/2;
		pos.y = pos.y * PathZoneSize + PathZoneSize/2;
		
		if(pos.x < 0) pos.x = 0;
		else if(pos.x >= GetWidth()) pos.x = GetWidth()-1;

		if(pos.y < 0) pos.y = 0;
		else if(pos.y >= GetHeight()) pos.y = GetHeight()-1;

		return pos;
	}

	void InitPathZone();

	stPathZone GetPathZone(POINT ptZone)
	{
		return m_aPathZone[ptZone.y * m_nPathZoneWidth + ptZone.x];
	}

	struct stBackMusic
	{
		int iMinInternal,iMaxInternal;
		int priority;
		char szBackMusic[64];
		stBackMusic()	{
			szBackMusic[0] = 0;
			priority = 0;
		}
		void Init(){
			szBackMusic[0] = 0;
			iMinInternal = 0;
			iMaxInternal = 0;
		}
	};	

	struct stZoneInfo{
		char szName[64];
		stRectI rect;
		stBackMusic music;
	};

	//Weather info
	struct stZoneWetherInfo
	{
		int timeStart;
		int timeEnd;
		BYTE type;
		int level;
		char szBackMusic[64];
	};

	// exit(城门出口)
	struct stExitInfo
	{
		char szName[64];
		int x;
		int y;
	};

	void PlayUISound(DWORD dwSoundID);
	bool m_bSendLoadOK;

	bool GetObjectImageLocation(int id,stResourceLocation * rl);
	void SetObjectTooltips(int id,CToolTips& toolTips);
private:
	stBackMusic m_BackMusic;

	std::map<DWORD,stZoneInfo> m_zoneInfo;
	//Exits map
	std::map<DWORD,stExitInfo> m_ExitInfo;
	//Wether map
	typedef std::vector<stZoneWetherInfo>	tZoneWetherInfo;
	std::map<DWORD,tZoneWetherInfo> m_zoneWetherInfo;
	void LoadMapZoneInfo();
	void LoadMapNpc(stMapScreenSizeDataUserCmd * pCmd);
	void InitZoneInfo();

	bool m_bDarkScene;

	bool m_bStartSpringComingEffect;
	bool m_bGMStartSpringComingEffect;
	time_t	 m_OffsetSpringComingTime;
public:
	CGameScene(void);
	~CGameScene(void);

	bool IsSpringEffect(){return m_bStartSpringComingEffect;};
	int GetMainCharacterDir();

	stBackMusic* GetBackMusic();
	
	std::map<DWORD,stZoneInfo> & GetZoneInfoList() { return m_zoneInfo;}
	
	stZoneInfo* GetMapZoneInfo(int id){
		std::map<DWORD,stZoneInfo>::iterator it = m_zoneInfo.find(id);
		if(it == m_zoneInfo.end()) return NULL;
		return &it->second;
	}

	//Exits  
	std::map<DWORD,stExitInfo> & GetExitInfo(){return m_ExitInfo;}
	//Wether interface
	std::map<DWORD,tZoneWetherInfo> & GetZoneWetherInfoList() { return m_zoneWetherInfo; }
	
	tZoneWetherInfo* GetMapZoneWetherInfo(int id)
	{
		std::map<DWORD,tZoneWetherInfo>::iterator it = m_zoneWetherInfo.find( id );
		if ( it == m_zoneWetherInfo.end() )
		{
			if ( m_zoneWetherInfo.find( 255 ) != m_zoneWetherInfo.end() )
				return &(m_zoneWetherInfo[255]);
			else
				return NULL;
		}

		return &(it->second);
	}
	stZoneWetherInfo* GetMapZoneWetherInfo(int id, int time)
	{
		tZoneWetherInfo* wether = GetMapZoneWetherInfo( id );

		if ( NULL == wether )
			return NULL;
		
		std::vector<stZoneWetherInfo>::iterator	itor;
		for( itor = wether->begin();itor !=wether->end();itor++ )
		{
			if ( (time >= itor->timeStart) && (time <= itor->timeEnd) )
				return &(*itor);
		}

		return NULL;
	}

	bool IsAnyAliveInterestNpc();


	bool IsAreaNameChange(POINT prevGrid,POINT ptCurGrid);
	bool IsAreaBackMusicChange(POINT prevGrid,POINT ptCurGrid);
	const char *GetAreaName(POINT ptCurGrid);
	stBackMusic * GetAreaBackMusic(POINT ptCurGrid);

	int m_aScreenID[9];
	bool IsHaveScreenID(int iScreenID)
	{
		return ::IsHaveScreenID(m_aScreenID,iScreenID);
	}
	char m_szMapName[64];
	char m_szCountryName[MAX_NAMESIZE];

public:
	void OnDisplayModeChange();
	typedef std::vector< CNpc* >			tListNpc;
	typedef std::vector< CCharacter* >		tListCharacter;
	typedef std::vector< CMagic* >			tListMagic;
	typedef std::vector< CItem* >			tListItem;
	typedef std::vector< CBuild* >			tListBuild;
	typedef std::vector< CPet* >			tListPet;
	typedef std::vector< CSceneMovie* >    tListMovie; // add by snip3r

	tListNpc		m_listNpc;
	tListCharacter	m_listCharacter;
	tListMagic		m_listMagic;
	tListItem		m_listItem;
	tListBuild		m_listBuild;
	tListMapNpc		m_listFunctionNpc;
	tListPet		m_listPet;

	//////////////////////////////////////////////////////////////////////////
	
	tListMovie		m_listMovie; // add by snip3r
	bool			b_MovieMode; // add by snip3r
	DWORD			m_dwCurTime; // add by snip3r

	CCartoonPetList	m_listCartoon;		//替身宠物列表
	CCartoonPetList* GetCartoonPetList() { return &m_listCartoon; }	

	CWaitingCartoonPetList	m_listWaitCartoon;		//等待领养的替身宠物
	CWaitingCartoonPetList* GetWaitCartoonPetList() { return &m_listWaitCartoon; }

	/*std::lookaside_allocator<CNpc> m_allocNpc;
	std::lookaside_allocator<CCharacter> m_allocCharacter;
	std::lookaside_allocator<CMagic> m_allocMagic;
	std::lookaside_allocator<CItem> m_allocItem;*/

	CMainCharacter*	m_pMainCharacter;
	CNpc*		m_pInterestNpc;
	stInterestNpcBaseInfo m_InterestNpcBaseInfo;

	bool		m_IsHaveSendRoralCityWarMsg;
	time_t		m_ServerTime;

	CAuction		m_Auction;
	CAuction* GetAuction() { return &m_Auction; }

	//DWORD GetMapCountryID();
	const char* GetMapName() { return m_szMapName;}
	const char* GetMapCountryName() { return m_szCountryName; }
	DWORD  GetMapCountryID();
	

	CMainCharacter* GetMainCharacter() { return m_pMainCharacter;}
	
	bool IsCurMapIsMiddleCountry();

	bool IsRoralCityWarTime();

	void UnSelectInterestNpc();
	void AddOrRemoveAllDynamicObstacle(bool bAdd = true);

	tListNpc & GetSceneObjectList(type2type_t<CNpc>)
	{
		return  m_listNpc;
	}

	tListCharacter & GetSceneObjectList(type2type_t<CCharacter>)
	{
		return  m_listCharacter;
	}

	tListMagic & GetSceneObjectList(type2type_t<CMagic>)
	{
		return  m_listMagic;
	}

	tListItem & GetSceneObjectList(type2type_t<CItem>)
	{
		return  m_listItem;
	}

	tListBuild & GetSceneObjectList(type2type_t<CBuild>)
	{
		return  m_listBuild;
	}

	tListPet & GetSceneObjectList(type2type_t<CPet>)
	{
		return  m_listPet;
	}
	
	template < class tObjectType ,class tItObjectType >
	tObjectType* ObjectItToObjectPointer(type2type_t<tObjectType>,tItObjectType it)
	{
		std::vector< tObjectType* >& list = GetSceneObjectList(type2type_t<tObjectType>());
		if(it == list.end()) return NULL;
		return (*it);
	}
	
	template < class tObjectType,class tItObjectType >
		tItObjectType FindObjectItByID(type2type_t<tObjectType>,type2type_t<tItObjectType>,DWORD dwID)
	{
		std::vector<tObjectType * >& list = GetSceneObjectList(type2type_t<tObjectType>());
		return std::find_if(list.begin(),list.end(),stFindObjectByID<tObjectType>(dwID));
	}

	template < class tObjectType,class tItObjectType >
		tItObjectType FindObjectItByName(type2type_t<tObjectType>,type2type_t<tItObjectType>,const char* pszName)
	{
		std::vector<tObjectType* >& list = GetSceneObjectList(type2type_t<tObjectType>());
		return std::find_if(list.begin(),list.end(),stFindObjectByName<tObjectType>(pszName));
	}

	//*
	template< class tObjectType>
	tObjectType * FindObjectByID(DWORD dwID)
	{
		std::vector<tObjectType* >::iterator it = FindObjectItByID(type2type_t<tObjectType>(),
			type2type_t<std::vector<tObjectType* >::iterator>(),dwID);
		return ObjectItToObjectPointer(type2type_t<tObjectType>(), it);
	}
	template< class tObjectType>
		tObjectType * FindObjectByName(const char* pszName)
	{
		std::vector<tObjectType* >::iterator it = FindObjectItByName(type2type_t<tObjectType>(),
			type2type_t<std::vector<tObjectType* >::iterator>(),pszName);
		return ObjectItToObjectPointer(type2type_t<tObjectType>(), it);
	}
	//*/

	tListNpc::iterator FindNpcItByID(DWORD dwID)
	{
		return FindObjectItByID(type2type_t<CNpc>(),type2type_t<tListNpc::iterator>(),dwID);
	}

	tListCharacter::iterator FindCharacterItByID(DWORD dwID)
	{
		return FindObjectItByID(type2type_t<CCharacter>(),type2type_t<tListCharacter::iterator>(),dwID);
	}
	tListCharacter::iterator FindCharacterItByName(const char* pszName)
	{
		return FindObjectItByName(type2type_t<CCharacter>(),type2type_t<tListCharacter::iterator>(),pszName);
	}
	tListItem::iterator FindItemItByID(DWORD dwID)
	{
		return FindObjectItByID(type2type_t<CItem>(),type2type_t<tListItem::iterator>(),dwID);
	}
	tListBuild::iterator FindBuildItByID(DWORD dwID)
	{
		return FindObjectItByID(type2type_t<CBuild>(),type2type_t<tListBuild::iterator>(),dwID);
	}
	tListPet::iterator FindPetItByID(DWORD dwID)
	{
		return FindObjectItByID(type2type_t<CPet>(),type2type_t<tListPet::iterator>(),dwID);
	}

	CNpc* FindNpcByID(DWORD dwID) { return FindObjectByID<CNpc>(dwID); }
	DWORD FindOneCanMoveNpcRandom();
	CNpc* FindNearbySummon(DWORD dwID = -1);
	CCharacter* FindNearbyCharacter();
	CCharacter* FindCharacterByID(DWORD dwID){  return FindObjectByID<CCharacter>(dwID); }
	CItem* FindItemByID(DWORD dwID) { return FindObjectByID<CItem>(dwID); }
	CBuild* FindBuildByID(DWORD dwID) { return FindObjectByID<CBuild>(dwID); }
	CPet* FindPetByID(DWORD dwID) { return FindObjectByID<CPet>(dwID); }
	CCharacter* FindCharacterByName(const char* pszName){ return FindObjectByName<CCharacter>(pszName);}
	CNpc* FindNpcByName(const char* pszName){ return FindObjectByName<CNpc>(pszName);}
	CNpc* FindNpcAndCharacter(DWORD dwID,bool bNpc);

	bool IsMainCharacterHasCartAmong9Screen();

	//
	CPet* FindPetByType(petType type)
	{
		for(tListPet::iterator it = m_listPet.begin();
			it != m_listPet.end(); ++it )
		{
			CPet* p = (*it);
			if( p->GetPetType() == type )
				return p;
		}
		return NULL;
	}

	DWORD GetPetNpcTypeIdByType(petType type)
	{
		CPet* pet = FindPetByType( type );
		if ( NULL == pet )
			return 0;

		CNpc* pNpc = pet->GetNpc();
		if ( NULL == pNpc )
			return 0;
		
		return pNpc->GetTypeID();		
	}

	bool bIsHaveSummon()
	{
		return FindPetByType(PET_TYPE_SUMMON) != NULL;
	}

	/*tListCharacter::iterator FindCharacterItByGridPos(POINT ptGrid)
	{
		for( tListCharacter::iterator it = m_listCharacter.begin(); 
			it != m_listCharacter.end(); ++it )
		{
			if(((CSceneObject*)(*it))->GetGridPos() == ptGrid)
				return it;
		}
		return m_listCharacter.end();
	}

	CCharacter* FindCharacterByGridPos(POINT ptGrid)
	{
		return ObjectItToObjectPointer(type2type_t<CCharacter>(),FindCharacterItByGridPos(ptGrid));
	}*/

	bool Create();
	void ReleaseScene();
	bool InitMap(stMapScreenSizeDataUserCmd * pCmd);
	void Destroy();

	void UpdateFindPathRect();

	CNpc* AddNpc(t_MapNpcData* pData);
	CNpc* AddNpc2(char* name,t_MapNpcData* pData);
	CNpc* AddCharacter(t_MapUserData* pData);
	bool AddMainCharacter(t_MainUserData* pData);
	bool AddItem(t_MapObjectData* pData,bool bDrop);
	bool AddBuild(t_MapBuildingData* pData);
	CPet* AddPet(petType type,DWORD dwID);

	bool AddMagic(iMagic* piMagic);
	void AddMagic(size_t magicType, stPointI pos, int nSetLife = -1, int table = 0);
	bool RemoveNpc(stRemoveMapNpcMapScreenUserCmd* pCmd);
	bool RemoveCharacter(stRemoveUserMapScreenUserCmd* pCmd);
	bool RemoveItem(stRemoveMapObjectMapScreenUserCmd* pCmd);
	bool RemoveBuild(stRemoveMapBuildingMapScreenUserCmd* pCmd);
	bool RemovePet(stDelPetPetCmd* pCmd);

	// add by snip3r
	void AddMovie( CSceneMovie* pImg );
	void RemoveAllMovie();
	void RunMovie(long time_elapsed);
	void StartMovieMode();
	void EndMovieMode();
	// add end

	bool IsDarkScene();

	CNpc * FindAndRequestNpc(DWORD dwID);
	CCharacter* FindAndRequestCharacter(DWORD dwID);

	void Run(long time_eslasped);
	void Draw(DWORD dwColor);
	
	void RemoveScreenData(int iScreenID);
	void RequestScreenData(int * pScreenID,size_t size);
	void Update9ScreenRect();

	//soke 显示玫瑰花（红）特效
	void AddRoseRoseIOU();

	//soke 显示烟花特效
	void AddRoseRoseIOX();

	//soke 天降洪福
	void AddTianjiang();

	void Center();

	void BuildRun(long time_eslasped);
	void ItemRun(long time_eslasped);
	void NpcRun(long time_eslasped);
	void CharacterRun(long time_eslasped);
	void MagicRun(long time_eslasped,tagListMagicGenerate& listGenerateMagic);
	void OnEvent(std::list<stEvent>& listEvent,iObj* pObj);
	void OnEvent(stEvent event,iObj* pObj);
	void OnEventEx(std::vector<stEventEx>& listEvent);

	void Sort();
	bool MsgProc(UINT nMsg,WPARAM wParam,LPARAM lParam);
	void UpdateMouseOverObject();

	void GetFullScreenCharacter(tStackVectorICharacter & list,iCharacter* pExcept,bool bGetNpc);
	//void GetCharacter(RangeEnum eRange, tStackVectorICharacter & list, POINT ptStart, size_t dir, iCharacter* pExcept = NULL);
	//void GetCharacter(tStackVectorICharacter& list, POINT ptStart, size_t dir, int mul, iCharacter* pExcept = NULL);

	template <class tObjectType>
	tObjectType * GetObjectNearGrid(POINT ptGrid)
	{
		int min = 9999;
		tObjectType * pObject = NULL;
		std::vector<tObjectType* > & list = GetSceneObjectList(type2type_t<tObjectType>());
		for( std::vector<tObjectType* >::iterator it = list.begin();it != list.end(); ++it )
		{
			if (!(*it)->IsDie())
			{
				stPointI pt2 = (*it)->GetGridPos();
				int dis = Scene_GetGridDis(ptGrid,pt2);
				if (dis < min)
				{
					min = dis;
					pObject = (*it);
				}
			}
		}
		return pObject;
	}

	template <class tObjectType, class tVec >
		void GetObjectAtGridRect(const RECT & rc,tVec & retval)
	{
		std::vector<tObjectType* > & list = GetSceneObjectList(type2type_t<tObjectType>());
		for( std::vector<tObjectType* >::iterator it = list.begin();it != list.end(); ++it )
		{
			if (!((CSceneObject*)(*it))->IsDie())
			{
				stPointI pt2 = ((CSceneObject*)(*it))->GetGridPos();
				if(pt2.x >= rc.left && pt2.x < rc.right && pt2.y >= rc.top && pt2.y < rc.bottom){
					retval.push_back( *it );
				}
			}
		}
	}

	template <class tObjectType>
	tObjectType * GetObjectAtGrid(POINT pt)
	{
		std::vector<tObjectType* > & list = GetSceneObjectList(type2type_t<tObjectType>());
		for( std::vector<tObjectType* >::iterator it = list.begin();it != list.end(); ++it )
		{
			if (!((CSceneObject*)(*it))->IsDie())
			{
				stPointI pt2 = ((CSceneObject*)(*it))->GetGridPos();
				if(pt2 == pt) return (*it);
			}
		}
		return NULL;
	}

	template <class tObjectType>
	tObjectType* GetObjectAtPixel(POINT pt)
	{
		std::vector<tObjectType* > & list = GetSceneObjectList(type2type_t<tObjectType>());
		for( std::vector<tObjectType* >::iterator it = list.begin();it != list.end(); ++it )
		{
			if (!(*it)->IsDie())
			{
				if(((CSceneObject*)(*it))->IsAtPixelPos(pt,false))
					return (*it);
			}
		}

		for( std::vector<tObjectType* >::iterator it = list.begin();it != list.end(); ++it )
		{
			if (!(*it)->IsDie())
			{
				if(((CSceneObject*)(*it))->IsAtPixelPos(pt,true))
					return (*it);
			}
		}
		return NULL;

	}

	template <class tObjectType>
		tObjectType* GetObjectAtScreenPixel(POINT pt)
	{
		stPointI pt1 = pt  + GetPixelOffset();
		return GetObjectAtPixel<tObjectType>(pt1);
	}

	CNpc* GetCharacterByPos(POINT ptPos);

	iCharacter* GetICharacterByPos(POINT ptGrid);

	void OnAllCharPostion(stAllMapScreenUserCmd* pCmd);
	void RequestScreenPic(int * pScreenID,size_t size);	

	template<class tObject,class tVectorObject>
	void GetObjectOrderByDistance(tVectorObject & aObjects,type2type_t<tObject>)
	{
		std::vector<tObject* > & listObject = GetSceneObjectList(type2type_t<tObject>());

		aObjects.reserve(listObject.size());
		aObjects.resize(0);
		for(size_t i=0; i < listObject.size(); ++i)
		{
			if(listObject[i] != (CSceneObject*)m_pMainCharacter)
				aObjects.push_back(listObject[i]);
		}

		qsort(&aObjects[0],aObjects.size(),sizeof(tObject*),compareObjectDistance);
		//std::sort(aObjects.begin(),aObjects.end(),compareObjectDistance<tObject>());
	}
	
	bool RefreshLightInfo();
	bool UpdateMouseOverObject(tListDrawObjects & objects);
	bool UpdateMouseOverTextNpc(tListCharacter& characters);

	DWORD  GetGameHourTime();

	CNpc* GetNearByNpc(enumNpcTypeItem* npcs,int xOffset = 2,int yOffset = 2);
	bool IsFunctionalNpcNear();
	bool IsBugbearNpcNear();

	bool IsGridHasAnyObject(const stPointI& ptGrid, bool bAbsPos = false);
	bool IsGridHasAnyEnemy(const stPointI& ptGrid, bool bAbsPos = false);

	POINT GetCenterGridPos();

	void CalculateDrawRect();

	std::string GetTaskVariable(DWORD taskID,const char* taskVarName);
	void MessageBox(const char* szText);
	void RemvoeMagicByCharacter(CNpc* pNpc);

	void UpdateCountryStar(DWORD dwCountry,DWORD dwStar);
	void UpdateCountryWarFlag();
	
	void StartSpringComingEffect(const time_t& sTime);
	void RunSpringComingEffect(long time_eslasped);
	void GMStartSpringComingEffect();

	void RunSpringWelcomeEffect();

	bool IsAntiDarkeCountryWarTime();

	bool IsCallDutyCanUse(); 

#ifdef _WYC_OPT_SER_
	//add by wyc 在得到服务器发回的帮派,家族名字字符串后,对该unionid,septid的角色的相应属性赋值
	void SetCharacterUnionNameByID(DWORD dwUnionID,const char* UnionName);
	void SetCharacterSeptNameByID(DWORD dwSeptID,const char* SeptName);
#endif
	
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
inline CGameScene * GetScene()
{
	extern CGameScene * g_pGameScene;
	return g_pGameScene;
}


