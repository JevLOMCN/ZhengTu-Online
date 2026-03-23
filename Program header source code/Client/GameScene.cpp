#include "public.h"
#include ".\gamescene.h"
#include "./Character.h"
#include "./MainCharacter.h"
#include "./Magic.h"
#include "./Item.h"
#include "./Build.h"
#include "../Magic/FootprintManager.h"
#include "./GameCursor.h"
#include "../gui/include/guiTypes.h"
#include "./GameTime.h"
#include "./GameGuiManager.h"
#include "./GameState.h"
#include "./GuiMain.h"
#include "./GuiMiniMap.h"
#include "./GuiWorldConn.h"
#include "../Magic/MapSurf.h"
#include "./Game.h"
#include "../RenderD3D/include/RenderD3D.h"
#include "../RenderD3D/include/D3DDevice.h"
#include "./particle/ParticleSysMgr.h"
#include "./WeatherMgr.h"
#include "../xml_parse/XmlParseEx.h"
#include "./RoleItem.h"
#include ".\Pet.h"
#include "./GuiPetCtrlDialog.h"
#include "./GuiPetDialog.h"
#include "./GuiTeam.h"
#include "./AStar.h"
#include "./AutoGame.h"
#include "./Country.h"
#include "../Media/Music.h"
#include <fstream>
#include "../magic/magic_sound.h"
#include "./Relation.h"
#include "./GameAppation.h"
#include "Maincharacter.h"
#include "./DlgEquiphandbook.h" //soke 百兽图鉴新增
#include <sys/timeb.h>

CGameScene * g_pGameScene = NULL;

POINT_LIST vShootGridList;

const int PathZoneSize = 7;

void DrawShootGridList()
{
#ifdef _DEBUG
	/*
	if ( vShootGridList.empty() )
		return;
	//Draw line
	stPointI lastGrid(0,0);
	stPointI ptGrid;
	std::vector<stPointI>::iterator itor;

	for( itor=vShootGridList.begin();itor!=vShootGridList.end();itor++ )
	{
		ptGrid = *itor;
		stPointI ptStart;
		stPointI ptEnd;
		ptStart.x = lastGrid.x * GRID_WIDTH - GRID_WIDTH/2 + GetDevice()->GetWidth()/2;
		ptStart.y = lastGrid.y * GRID_HEIGHT - GRID_HEIGHT/2 + GetDevice()->GetHeight()/2;
		ptEnd.x = ptGrid.x * GRID_WIDTH - GRID_WIDTH/2 + GetDevice()->GetWidth()/2;
		ptEnd.y = ptGrid.y * GRID_HEIGHT - GRID_HEIGHT/2 + GetDevice()->GetHeight()/2;
		
		GetDevice()->DrawLine( &ptStart, &ptEnd,	D3DCOLOR_ARGB(255,255,0,0) );

		lastGrid = ptGrid;
	}
	*/
#endif
}
bool IsInteriorMap(const std::string & strMapName)
{
	std::basic_string <char>::size_type _dot_pos = strMapName.find('.',0);
	std::string strInner = strMapName.substr(0,_dot_pos);

	for( size_t i = 0;i < g_vecInteriorMapName.size() ;++i)
	{
		const std::string  & strList = g_vecInteriorMapName[i];
		std::basic_string <char>::size_type _dot_pos_2 = strList.find('.',0);
		std::string _strList = strMapName.substr(0,_dot_pos_2);
		if( 0 == stricmp(strInner.c_str(),_strList.c_str()))
		{
			return true;
		}
	}	
	return false;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param p1 : 描述
 * \param p2 : 描述
 * \return 返回值的描述
 */
int  compareObjectDistance(const void* p1,const void * p2)
{
	FUNCTION_BEGIN;

	return Scene_GetDis( ((CSceneObject*)GetScene()->GetMainCharacter())->GetGridPos(),(*(CSceneObject**)p1)->GetGridPos() )
		- Scene_GetDis( ((CSceneObject*)GetScene()->GetMainCharacter())->GetGridPos(),(*(CSceneObject**)p2)->GetGridPos() );

	FUNCTION_END;
}

int  compareObjectName(const void* p1,const void * p2)
{
	FUNCTION_BEGIN;

	return _stricmp(((stMapFunctionNpc*)p1)->szName, ((stMapFunctionNpc*)p2)->szName );
 
	FUNCTION_END;
}

POINT_LIST GetPointListSingleArrow(const stPointI& dir, const stPointF& screen, int num_obj)
{
	FUNCTION_BEGIN;

	vShootGridList.clear();
	
	// 假定人物位置为(0,0)
	//float ftheta = 22.5f*PI/180.f * dir;
	stPointF fDir;
	fDir.x = dir.x;
	fDir.y = dir.y;
	float angleSin = fDir.x / sqrt((fDir.x)*(fDir.x) + (fDir.y)*(fDir.y));
	float angleCos = fDir.y / 2 / sqrt((fDir.x)*(fDir.x) + (fDir.y)*(fDir.y));
	POINT_LIST _list;
	int nWidthGrids = 2 * (screen.x/2 - GRID_WIDTH/2 +GRID_WIDTH-1) / GRID_WIDTH;
	int nHeightGrids  = 2 * (screen.y/2 - GRID_HEIGHT/2 + GRID_HEIGHT-1) / GRID_HEIGHT;

	const float DIFF_SAMPLE = 15;  // 直线采样点像素间隔
	float c_radius = 1.42 * screen.x / 2;
	{
		stPointF ptOffset;//箭支相对人物的偏移
		ptOffset.x = 0.0f;
		ptOffset.y = 0.0f;
		stPointI lastGrid(0,0);
		for( int _i = 1 ; _i<c_radius / DIFF_SAMPLE ; _i++) // 每隔DIFF_SAMPLE采样检测一次
		{

			/*float _offset_x = ptOffset.x + _i * DIFF_SAMPLE* sin(ftheta);
			float _offset_y = ptOffset.y - _i * DIFF_SAMPLE * cos(ftheta);*/
			float _offset_x = ptOffset.x + _i * DIFF_SAMPLE* angleSin;
			float _offset_y = ptOffset.y + _i * DIFF_SAMPLE * angleCos;
			stPointI ptGrid;

			for( int i=1;i<2;i++ )
			{
				ptGrid.x = (_offset_x + (i-1) * GRID_WIDTH / 2)  / GRID_WIDTH;

				ptGrid.y = (_offset_y + (i-1) * GRID_HEIGHT / 2) / GRID_HEIGHT;


				if( abs(ptGrid.x) >= (nWidthGrids/2 + 1) || abs(ptGrid.y) >= (nHeightGrids/2 +1)) // exceed the screen
					break;

				if( ptGrid.x ==  lastGrid.x && ptGrid.y == lastGrid.y)
					continue;

				vShootGridList.push_back( ptGrid );

				lastGrid = ptGrid;
				//TRACE( "Search grid range x=%d y=%d====================================\n", ptGrid.x+GetScene()->GetMainCharacter()->GetGridPos().x, ptGrid.y+GetScene()->GetMainCharacter()->GetGridPos().y );
				if( GetScene()->IsGridHasAnyObject(ptGrid,false))
				{
					//TRACE( "Search grid has object x=%d y=%dVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV\n", ptGrid.x+GetScene()->GetMainCharacter()->GetGridPos().x, ptGrid.y+GetScene()->GetMainCharacter()->GetGridPos().y );

					if( _list.end() == std::find(_list.begin(),_list.end(),ptGrid))
						_list.push_back(ptGrid);

					if ( _list.size() >= num_obj )
						break; //这只箭射到了
				}
			}
			
		}
	}

	return _list;
	
	FUNCTION_END;
}

POINT_LIST GetPointList(int dir,const stPointF & screen,int num_arrow,int nspace)
{
	FUNCTION_BEGIN;

	// 假定人物位置为(0,0)
	float ftheta = 22.5f*PI/180.f * dir;	
	POINT_LIST _list;
	int nWidthGrids = 2 * (screen.x/2 - GRID_WIDTH/2 +GRID_WIDTH-1) / GRID_WIDTH;
	int nHeightGrids  = 2 * (screen.y/2 - GRID_HEIGHT/2 + GRID_HEIGHT-1) / GRID_HEIGHT;

	const float DIFF_SAMPLE = 15;  // 直线采样点像素间隔
	float c_radius = 1.42 * screen.x / 2;
	int i;
	for( i = 0 ; i< num_arrow ; i++)     // 对每根箭的轨迹(目前只支持直线)
	{	
		stPointF ptOffset;//每根箭支相对人物的偏移
			ptOffset.x = - (num_arrow - 1 - 2*i) / 2 * nspace * cos(ftheta);
			ptOffset.y = - (num_arrow - 1 - 2*i) / 2 * nspace * sin(ftheta);
		stPointI lastGrid(0,0);
		for( int _i = 1 ; _i<c_radius / DIFF_SAMPLE ; _i++) // 每隔DIFF_SAMPLE采样检测一次
		{
		
			float _offset_x = ptOffset.x + _i * DIFF_SAMPLE* sin(ftheta);
			float _offset_y = ptOffset.y - _i * DIFF_SAMPLE * cos(ftheta);
			stPointI ptGrid;
			if( _offset_x >= 0)
				ptGrid.x = (_offset_x + GRID_WIDTH * 1/2 )  / GRID_WIDTH;
			else
				ptGrid.x = (_offset_x - GRID_WIDTH * 1/2 )  / GRID_WIDTH;
			
			if( _offset_y >= 0 )
				ptGrid.y = (_offset_y + GRID_HEIGHT * 1/2 ) / GRID_HEIGHT;
			else 
				ptGrid.y = (_offset_y - GRID_HEIGHT * 1/2 ) / GRID_HEIGHT;


			if( ptGrid.x >= abs(nWidthGrids/2 + 1) || ptGrid.y >= abs(nHeightGrids/2 +1)) // exceed the screen
				break;		        


			if( ptGrid.x ==  lastGrid.x && ptGrid.y == lastGrid.y)
				continue;
			lastGrid = ptGrid;
			//TRACE( "Search grid range x=%d y=%d====================================\n", ptGrid.x+GetScene()->GetMainCharacter()->GetGridPos().x, ptGrid.y+GetScene()->GetMainCharacter()->GetGridPos().y );
			if( GetScene()->IsGridHasAnyObject(ptGrid,false))
			{
				//TRACE( "Search grid has object x=%d y=%dVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV\n", ptGrid.x+GetScene()->GetMainCharacter()->GetGridPos().x, ptGrid.y+GetScene()->GetMainCharacter()->GetGridPos().y );
				
				if( _list.end() == std::find(_list.begin(),_list.end(),ptGrid))
					_list.push_back(ptGrid);
				break; //这只箭射到了
			}
		}
	}
	return _list;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CGameScene::CGameScene(void)
: m_pMainCharacter(NULL)
, m_pInterestNpc(NULL)
, m_bSendLoadOK(true)
, m_bDarkScene(false)
, m_bStartSpringComingEffect(false)
, m_bGMStartSpringComingEffect(false)
, m_OffsetSpringComingTime(0)
{
	FUNCTION_BEGIN;

	memset(&m_aScreenID,0xff,sizeof(m_aScreenID));
	m_szMapName[0] = 0;
	m_szCountryName[0] = 0;

	m_InterestNpcBaseInfo.tmpID = 0;
	b_MovieMode = false;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
CGameScene::~CGameScene(void)
{
	FUNCTION_BEGIN;

	Destroy();	

	FUNCTION_END;
}

struct stDeleteCharacter{
	/*std::lookaside_allocator<CCharacter> * m_alloc;
	stDeleteCharacter(std::lookaside_allocator<CCharacter> * pAlloc)
	: m_alloc(pAlloc) {}*/
	void operator () (CCharacter * pObject)
	{
		if(pObject != GetScene()->m_pMainCharacter)
			delete pObject;
		//m_alloc->freePointer(pObject);
	}
};

template<class tObjectType>
struct stDeleteSceneObject{
	/*std::lookaside_allocator<tObjectType> * m_alloc;
	stDeleteSceneObject(std::lookaside_allocator<tObjectType> * pAlloc)
	: m_alloc(pAlloc) {}*/
	void operator () (tObjectType * pObject)
	{
		delete pObject;
		//m_alloc->freePointer(pObject);
	}
};

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CGameScene::ReleaseScene()
{
	FUNCTION_BEGIN;

	std::for_each(m_listNpc.begin(),m_listNpc.end(),stDeleteSceneObject<CNpc>(/*&m_allocNpc*/));
	std::for_each(m_listCharacter.begin(),m_listCharacter.end(),stDeleteCharacter(/*&m_allocCharacter*/));
	std::for_each(m_listItem.begin(),m_listItem.end(),stDeleteSceneObject<CItem>(/*&m_allocItem*/));
	std::for_each(m_listMagic.begin(),m_listMagic.end(),stDeleteSceneObject<CMagic>(/*&m_allocMagic*/));

	std::for_each(m_listBuild.begin(),m_listBuild.end(),stDeleteObject<CBuild>());
	m_listNpc.resize(0);
	m_listCharacter.resize(0);
	m_listItem.resize(0);
	m_listBuild.resize(0);
	m_listMagic.resize(0);
	memset(&m_aScreenID,0xff,sizeof(m_aScreenID));

	CMapScene::ReleaseScene();

	if ( m_pMainCharacter )
	{
		m_pMainCharacter->m_ptPrevGrid = stPointI(-1,-1);

		//Clear follow state
		m_pMainCharacter->ClearRunEvent();
		m_pMainCharacter->ClearPath();
	}

	m_bSendLoadOK = true;
	FUNCTION_END;
}

///**
// * \brief 简短描述
// * 
// * 详细描述
// * 
// * \param dwID : 描述
// * \return 返回值的描述
// */
///*
//CGameScene::tListNpc::iterator CGameScene::FindNpcItByID(DWORD dwID)
//{
//	FUNCTION_BEGIN;
//
//return std::find_if(m_listNpc.begin(),m_listNpc.end(),stFindObjectByID(dwID));
//
//	FUNCTION_END;
//}
//
///**
// * \brief 简短描述
// * 
// * 详细描述
// * 
// * \param dwID : 描述
// * \return 返回值的描述
// */
//CGameScene::tListCharacter::iterator CGameScene::FindCharacterItByID(DWORD dwID)
//{
//	FUNCTION_BEGIN;
//
//return std::find_if(m_listCharacter.begin(),m_listCharacter.end(),stFindObjectByID(dwID));
//
//	FUNCTION_END;
//}
//
///**
// * \brief 简短描述
// * 
// * 详细描述
// * 
// * \param dwID : 描述
// * \return 返回值的描述
// */
//CGameScene::tListItem::iterator CGameScene::FindItemItByID(DWORD dwID)
//{
//	FUNCTION_BEGIN;
//
//return std::find_if(m_listItem.begin(),m_listItem.end(),stFindObjectByID(dwID));
//
//	FUNCTION_END;
//}
//
///**
// * \brief 简短描述
// * 
// * 详细描述
// * 
// * \param dwID : 描述
// * \return 返回值的描述
// */
//CGameScene::tListBuild::iterator CGameScene::FindBuildItByID(DWORD dwID)
//{
//	FUNCTION_BEGIN;
//
//return std::find_if(m_listBuild.begin(),m_listBuild.end(),stFindObjectByID(dwID));
//
//	FUNCTION_END;
//}
////*/
bool CGameScene::Create()
{
	FUNCTION_BEGIN;

	m_bSendLoadOK = true;
	InitGraphicPack();
	//醉梦注释报错点	
	LoadMapTileInfo();
	m_pMainCharacter = new CMainCharacter;
	memset(&m_aScreenID,0xff,sizeof(m_aScreenID));

	m_bGMStartSpringComingEffect = false;

	return false;

	FUNCTION_END;
}

void CGameScene::UnSelectInterestNpc()
{
	if ( m_pInterestNpc )
	{
		//Send unselect msg to server
		stSelectPropertyUserCmd cmd;
		cmd.byType = m_pInterestNpc->IsCharacter() ? MAPDATATYPE_USER : MAPDATATYPE_NPC;
		cmd.dwTempID = -1;
		cmd.byOldType = m_pInterestNpc->IsCharacter() ? MAPDATATYPE_USER : MAPDATATYPE_NPC;
		cmd.dwOldTempID = m_pInterestNpc->GetID();

		SEND_USER_CMD( cmd );
	}

	m_InterestNpcBaseInfo.Init();
	m_pInterestNpc = NULL;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CGameScene::LoadMapZoneInfo()
{
	FUNCTION_BEGIN;
	m_BackMusic.Init();
	m_zoneInfo.clear();
	m_zoneWetherInfo.clear();
	m_ExitInfo.clear();
	TiXmlDocument doc;
	Stream* pStream = OpenPackFileForRead("data\\datas.pak","datas\\mapsinfo.xml");
	//Stream* pStream = OpenPackFileForRead(NULL,"data\\datas\\mapsinfo.xml");
	if(!pStream)
	{
		Engine_WriteLog("装载 mapsinfo.xml 失败\n");
		return ;
	}
	//if(!doc.LoadFile("data\\mapsinfo.xml"))
	if(!LoadXMLFromStream(pStream,doc))
	{
		ClosePackFileForRead(pStream);
		Engine_WriteLog("解析 mapsinfo.xml 失败\n");
		return;
	}
	TiXmlElement* maps = doc.FirstChildElement("maps");
	if(maps)
	{
		TiXmlElement* map = maps->FirstChildElement("map");
		while(map)
		{
			char blankBackMusic[1];
			sprintf( blankBackMusic, "" );
			const char* name = map->Attribute("name");
			const char* backmusic = map->Attribute("backmusic");
			if( name && strcmp(name,m_szFileName) == 0 )
			{
				if ( backmusic )
					strncpy(m_BackMusic.szBackMusic,backmusic,sizeof(m_BackMusic.szBackMusic));
				else
				{
					backmusic = blankBackMusic;
					sprintf( m_BackMusic.szBackMusic, "" );
				}
				m_BackMusic.szBackMusic[sizeof(m_BackMusic.szBackMusic)-1] = 0;

				if(map->QueryIntAttribute("MinInterval",&m_BackMusic.iMinInternal) != TIXML_SUCCESS)
					m_BackMusic.iMinInternal = 0;
				if(map->QueryIntAttribute("MaxInterval",&m_BackMusic.iMaxInternal) != TIXML_SUCCESS)
					m_BackMusic.iMinInternal = 0;

				TiXmlElement* zones = map->FirstChildElement("zones");
				if(zones)
				{
					TiXmlElement* zone = zones->FirstChildElement("zone");
					while(zone)
					{
						int id;
						const char* name = zone->Attribute("name");
						const char* backmusic1 = zone->Attribute("backmusic");
						if(backmusic1 == NULL || backmusic1[0] == 0)
							backmusic1 = backmusic;
						if(zone->QueryIntAttribute("id",&id) == TIXML_SUCCESS)
						{
							stZoneInfo & st = m_zoneInfo[id];

							strncpy(st.szName,name,sizeof(st.szName));
							st.szName[sizeof(st.szName)-1] = 0;

							strncpy(st.music.szBackMusic,backmusic1,sizeof(st.music.szBackMusic));
							st.music.szBackMusic[sizeof(st.music.szBackMusic)-1] = 0;
							st.rect = stRectI(0,0,0,0);

							if(zone->QueryIntAttribute("Priority",(int *)&st.music.priority) != TIXML_SUCCESS)
								st.music.priority = 0;
							if(zone->QueryIntAttribute("MinInterval",&st.music.iMinInternal) != TIXML_SUCCESS)
								st.music.iMinInternal = 0;
							if(zone->QueryIntAttribute("MaxInterval",&st.music.iMaxInternal) != TIXML_SUCCESS)
								st.music.iMaxInternal = 0;

							//Get zone wether info
							TiXmlElement* wether = zone->FirstChildElement( "weather" );
							if(wether)
							{
								tZoneWetherInfo& vInfo = m_zoneWetherInfo[id];
								TiXmlElement* date = wether->FirstChildElement( "date" );
								while(date)
								{
									int timeStart;
									int timeEnd;
									if( (date->QueryIntAttribute( "start", &timeStart )  == TIXML_SUCCESS)
										&& (date->QueryIntAttribute( "end", &timeEnd ) == TIXML_SUCCESS) )
									{
										for( int i=0;i<WETHER_T_NUM;i++ )
										{
											TiXmlElement* wt = date->FirstChildElement( wetherName[i] );
											if (wt)
											{
												int level;
												if ( wt->QueryIntAttribute( "level", &level ) == TIXML_SUCCESS )
												{
													//Yes we get an wether
													stZoneWetherInfo st;
													st.level = level;
													st.timeEnd = timeEnd;
													st.timeStart = timeStart;
													st.type = i;

													const char* weatherMusic = wt->Attribute( "backmusic" );
													if ( (weatherMusic == NULL) || (weatherMusic[0] == 0) )
														sprintf( st.szBackMusic, "nomusic" );
													else
														sprintf( st.szBackMusic, weatherMusic );

													vInfo.push_back( st );
												}
											}
										}										
									}

									date = date->NextSiblingElement( "date" );
								}
							}
						}			

						zone = zone->NextSiblingElement("zone");
					}
				}


				// Get the exit(城门出口)
				TiXmlElement * exits = map->FirstChildElement("exits");
				if( exits )
				{
					TiXmlElement * exit = exits->FirstChildElement("exit");
					while(exit)
					{
						int id,x,y;
						const char * name = exit->Attribute("name");
						if(exit->QueryIntAttribute("id",&id) == TIXML_SUCCESS &&
							exit->QueryIntAttribute("x",&x) == TIXML_SUCCESS &&
							exit->QueryIntAttribute("y",&y) == TIXML_SUCCESS)
						{
							stExitInfo & element = m_ExitInfo[id];
							strncpy(element.szName,name,sizeof(element.szName));
							element.szName[sizeof(element.szName)-1] = 0;
							element.x = x;
							element.y = y;
						}
						exit = exit->NextSiblingElement("exit");

					}
				}


				//Get Weather info
				TiXmlElement* wether = map->FirstChildElement( "weather" );
				if(wether)
				{
					tZoneWetherInfo vInfo;
					TiXmlElement* date = wether->FirstChildElement( "date" );
					while(date)
					{
						int timeStart;
						int timeEnd;
						if( (date->QueryIntAttribute( "start", &timeStart )  == TIXML_SUCCESS)
							&& (date->QueryIntAttribute( "end", &timeEnd ) == TIXML_SUCCESS) )
						{
							for( int i=0;i<WETHER_T_NUM;i++ )
							{
								TiXmlElement* wt = date->FirstChildElement( wetherName[i] );
								if (wt)
								{
									int level;
									if ( wt->QueryIntAttribute( "level", &level ) == TIXML_SUCCESS )
									{
										//Yes we get an wether
										stZoneWetherInfo st;
										st.level = level;
										st.timeEnd = timeEnd;
										st.timeStart = timeStart;
										st.type = i;

										const char* weatherMusic = wt->Attribute( "backmusic" );
										if ( (weatherMusic == NULL) || (weatherMusic[0] == 0) )
											sprintf( st.szBackMusic, "nomusic" );
										else
											sprintf( st.szBackMusic, weatherMusic );

										vInfo.push_back( st );
									}
								}
							}										
						}

						date = date->NextSiblingElement( "date" );
					}

					//Set this wether info to those zone who has not any wether info yet
					std::map<DWORD,stZoneInfo>::iterator it;
					for( it = m_zoneInfo.begin();it != m_zoneInfo.end();it++ )
					{
						for( tZoneWetherInfo::iterator itor=vInfo.begin();itor!=vInfo.end();itor++ )
						{
							m_zoneWetherInfo[it->first].push_back( *itor );
						}
						//tZoneWetherInfo* wether = GetMapZoneWetherInfo( it->first );
						//if ( NULL == wether )		//not yet has wether
						//	m_zoneWetherInfo[it->first] = vInfo;
					}

					//Add free zone as one whole zone and set same weather info
					for( tZoneWetherInfo::iterator itor=vInfo.begin();itor!=vInfo.end();itor++ )
					{
						m_zoneWetherInfo[255].push_back( *itor );
					}
				}

				//Here we just check the map data
				/*std::map<DWORD,tZoneWetherInfo>::iterator it;
				for( it=m_zoneWetherInfo.begin();it!=m_zoneWetherInfo.end();it++ )
				{
				int id = it->first;
				stZoneWetherInfo st
				tZoneWetherInfo* info = &(it->second);
				std::vector<stZoneWetherInfo>::iterator itor;
				for( itor=info->begin();itor!=info->end();itor++ )
				st = *itor;
				}*/
				//end check
				break;
			}

			map = map->NextSiblingElement("map");
		}
	}
	ClosePackFileForRead(pStream);


	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
CGameScene::stBackMusic * CGameScene::GetBackMusic() 
{
	FUNCTION_BEGIN;

	return &m_BackMusic;

	FUNCTION_END;
}
/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CGameScene::InitZoneInfo()
{
	FUNCTION_BEGIN;

	int width = GetWidth();
	int height = GetHeight();
	int i;for(i = 0; i < height; ++i)
	{
		for(int j=0; j < width; ++j)
		{
			int zoneID = m_map.GetTile(j,i)->zone;
			CGameScene::stZoneInfo* pInfo = GetMapZoneInfo(zoneID);
			if(pInfo)
			{
				stRectI rect(j,i,j+1,i+1);
				std::map<DWORD,stZoneInfo>::iterator it = m_zoneInfo.find(zoneID);
				if(it != m_zoneInfo.end())
				{
					stZoneInfo & zoneInfo = it->second;
					if( zoneInfo.rect.IsRectEmpty())
						zoneInfo.rect = rect;
					else 
						zoneInfo.rect |= rect;
				}
			}
		}
	}

	FUNCTION_END;
}

void CGameScene::MessageBox(const char* szText)
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->AddMessageBox(szText);
	FUNCTION_END;
}


bool CGameScene::IsCurMapIsMiddleCountry()
{
	FUNCTION_BEGIN;

	if( CCountry::Instance()->ContainsCountry(m_szCountryName))
		return false;

	return true;

	FUNCTION_END;
}

bool CGameScene::IsAnyAliveInterestNpc()
{
	if ( NULL == m_pInterestNpc )
		return false;

	if ( m_pInterestNpc->IsDie() )
		return false;

	return true;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pCmd : 描述
* \return 返回值的描述
*/
bool IsDarkScene(const char* szMapName)
{
	const char* szDarkMapName[] = { "gumu", "", "" };
	for( int i=0;i<count_of(szDarkMapName);i++ )
	{
		if ( stricmp( szMapName, szDarkMapName[i] ) == 0 )
			return true;
	}

	return false;
}

bool CGameScene::InitMap(stMapScreenSizeDataUserCmd * pCmd)
{
	FUNCTION_BEGIN;

	strncpy(m_szMapName,pCmd->pstrMapName,sizeof(m_szMapName));
	m_szMapName[sizeof(m_szMapName)-1] = 0;

	strncpy( m_szCountryName, pCmd->pstrCountryName, sizeof(m_szCountryName) );
	m_szCountryName[sizeof(m_szCountryName)-1] = 0;

	//Wether this is a dark scene
	m_bDarkScene = ::IsDarkScene( pCmd->pstrFilename );

	m_IsHaveSendRoralCityWarMsg = false;

	//Stop weather effect
	GetWeatherMgr()->ChangeWeather( WETHER_T_NUM, -1, "" );

	//Load map
	DWORD t1 = xtimeGetTime();
	if(!LoadMap(pCmd->pstrFilename))
	{
		char szMsg[256];
		_snprintf(szMsg,256,"未找到 %s 对应的地图文件。",pCmd->pstrMapName);
		szMsg[255] = 0;
		MessageBox(szMsg);
		return false;
	}

#if defined _DEBUG || defined RELEASE_TEST
	Engine_WriteLogF("LoadMap time %d\n",xtimeGetTime() - t1);
#endif

	m_pMainCharacter->ClearCmdQueue();
	m_pMainCharacter->SetEmperorDareZone( false, 0 );

	m_pMainCharacter->UpdateGridPosition(stPointI(pCmd->mainRoleX,pCmd->mainRoleX));

	t1 = xtimeGetTime();

	if(!GetScene()->IsDarkScene())
	{
		GetGameGuiManager()->m_guiMiniMap->LoadMiniMap(pCmd->pstrFilename);

#if defined _DEBUG || defined RELEASE_TEST
		Engine_WriteLogF("LoadMiniMap time %d\n",xtimeGetTime() - t1);
#endif

		GetGameGuiManager()->m_guiMiniMap->InitDevice();
		//Open lightmap effect
		GetLightMap()->SetEnabled( false );

		GetGameGuiManager()->m_guiMiniMap->ShowMiniMap( true );

	}
	else
	{
		GetGameGuiManager()->m_guiMiniMap->ShowMiniMap( false );		
	}

	if(GetGameGuiManager()->m_guiLageMap)
		GetGameGuiManager()->m_guiLageMap->Close();



	GetGameState()->SetState(eGameState_Run);
	GetGameState()->SetStateItem(eGameStateItem_Normal);

	t1 = xtimeGetTime();
	LoadMapZoneInfo();
	LoadMapNpc(pCmd);

#if defined _DEBUG || defined RELEASE_TEST
	Engine_WriteLogF("LoadMapZoneInfo time %d\n",xtimeGetTime() - t1);
#endif

	if(!CCountry::Instance()->IsCountryWarState() && !IsSpringEffect())
	{
		CGameScene::stBackMusic * pBackMusic = GetBackMusic();
		if( pBackMusic )
			PlayBackMusic(pBackMusic->szBackMusic,c_dwMusicFadeTime,pBackMusic->iMinInternal,pBackMusic->iMaxInternal);
		else
			PlayBackMusic("");
	}else{
		PlayBackMusic("");
	}

	t1 = xtimeGetTime();
	InitZoneInfo();

#if defined _DEBUG || defined RELEASE_TEST
	Engine_WriteLogF("InitMapZoneInfo time %d\n",xtimeGetTime() - t1);
#endif

	CCountry::Instance()->RuquestTax();
	//InitPathZone();
	m_bSendLoadOK = false;

	SetInteriorMap(IsInteriorMap(m_szFileName));

	return true;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CGameScene::Destroy()
{
	FUNCTION_BEGIN;

	ReleaseScene();
	delete m_pMainCharacter;
	m_pMainCharacter = NULL;

	m_pInterestNpc = NULL;

	//destroy all weather effect
	if ( GetWeatherMgr() )
		GetWeatherMgr()->ReleaseAllWeather();

	//不能放在ReleaseScence里面，因为每次加载地图时要清掉其他的数据，就把服务端发来的
	//马的信息给删掉了，
	FreePointerList<tListPet>(m_listPet);

	GetCartoonPetList()->Release();			//替身宠物列表清空
	GetWaitCartoonPetList()->Release();		//等待领养的替身宠物列表清空
	GetRelation().Release();

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CGameScene::UpdateFindPathRect()
{
	FUNCTION_BEGIN;

	POINT pt = m_pMainCharacter->GetGridPos();

	m_rcFindPathRect.left = pt.x - m_pMainCharacter->GetMaxFindDis();
	m_rcFindPathRect.top  = pt.y - m_pMainCharacter->GetMaxFindDis();
	m_rcFindPathRect.right = pt.x + m_pMainCharacter->GetMaxFindDis();
	m_rcFindPathRect.bottom = pt.y + m_pMainCharacter->GetMaxFindDis();
	stRectI rc(0,0,m_map.GetWidth(),m_map.GetHeight());
	m_rcFindPathRect &= rc;

	FUNCTION_END;
}
//
//CCharacter* CGameScene::FindCharacterByID(DWORD dwID) 
//{
//	//if(dwID == m_pMainCharacter->GetID()) return m_pMainCharacter;
//	return CharacterItToCharacter(FindCharacterItByID(dwID)); 
//}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pCmd : 描述
* \return 返回值的描述
*/
void CGameScene::LoadMapNpc(stMapScreenSizeDataUserCmd * pCmd)
{
	FUNCTION_BEGIN;

	stMapFunctionNpc  st;
	NpcBase_t*        pData = NULL;
	m_listFunctionNpc.clear();
	for (int i=0;i<pCmd->npc_count;i++)
	{
		st.dwID = pCmd->npc_list[i].id;
		pData = GetNpcBase(st.dwID);
		if (pData)
			strncpy(st.szName,pData->strName,sizeof(st.szName));
		st.ptPos.x = pCmd->npc_list[i].x;
		st.ptPos.y = pCmd->npc_list[i].y;
		m_listFunctionNpc.push_back(st);
	}

	qsort(&m_listFunctionNpc[0],m_listFunctionNpc.size(),sizeof(stMapFunctionNpc),compareObjectName);

	if (GetGameGuiManager()->m_guiNearby)
		GetGameGuiManager()->m_guiNearby->ResetMapNpc(m_listFunctionNpc);

	if( GetGameGuiManager()->m_guiMiniMap)
		GetGameGuiManager()->m_guiMiniMap->ResetMapNpc(m_listFunctionNpc);

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pData : 描述
* \return 返回值的描述
*/
CNpc* CGameScene::AddNpc(t_MapNpcData* pData)
{
	FUNCTION_BEGIN;

	CNpc* pNpc = FindNpcByID(pData->dwMapNpcDataPosition);

	if(pNpc) 
	{
		stAddMapNpcMapScreenUserCmd cmd;
		cmd.data = *pData;
		pNpc->PushCommand(&cmd,sizeof(cmd));
	}
	else
	{
		pNpc = new CNpc;
		//pNpc = LOOKASIDE_ALLOC(m_allocNpc);
		pNpc->SetAttrib(pData);

		//soke 2016 - 3 - 27 增加NPC的特效显示
		if (pNpc->GetTypeID() == 142||pNpc->GetTypeID() ==143
		    ||pNpc->GetTypeID() == 149||pNpc->GetTypeID() ==150
			||pNpc->GetTypeID() == 158||pNpc->GetTypeID() ==301
			||pNpc->GetTypeID() == 302||pNpc->GetTypeID() ==496
			||pNpc->GetTypeID() == 513||pNpc->GetTypeID() ==843
			||pNpc->GetTypeID() == 844||pNpc->GetTypeID() ==845
			||pNpc->GetTypeID() == 846||pNpc->GetTypeID() ==847
			||pNpc->GetTypeID() == 5001||pNpc->GetTypeID() ==5020
            ||pNpc->GetTypeID() == 5093||pNpc->GetTypeID() ==5122
			||pNpc->GetTypeID() == 5131||pNpc->GetTypeID() ==5094
			||pNpc->GetTypeID() == 5099||pNpc->GetTypeID() ==5096
			||pNpc->GetTypeID() == 5150||pNpc->GetTypeID() ==5155
			||pNpc->GetTypeID() ==1023) //是否为车夫
		{
			pNpc->AddEffect("PearlFrontchefu1");
		}
		if (pNpc->GetTypeID() == 887) //是否为福利姐姐
		{
			pNpc->AddEffect("PearlFrontfuli1");
		}
		if (pNpc->GetTypeID() == 99||pNpc->GetTypeID() == 884) //是否为高级杂货
		{
			pNpc->AddEffect("PearlFrontgaoji1");
		}
		if (pNpc->GetTypeID() == 361||pNpc->GetTypeID() == 854) //是否为高级钱庄老板
		{
			pNpc->AddEffect("PearlFrontlaoban1");
		}
		if (pNpc->GetTypeID() == 213||pNpc->GetTypeID() == 214
			||pNpc->GetTypeID() == 215||pNpc->GetTypeID() == 216) //是否为武器锻造师
		{
			pNpc->AddEffect("PearlFrontwuqiduanzao1");
		}
		if (pNpc->GetTypeID() == 217||pNpc->GetTypeID() == 218) //是否为首饰锻造师
		{
			pNpc->AddEffect("PearlFrontshoushiduanzao1");
		}
		if (pNpc->GetTypeID() == 211||pNpc->GetTypeID() == 212) //是否为防具锻造师
		{
			pNpc->AddEffect("PearlFrontfangjuduanzao1");
		}
		if (pNpc->GetTypeID() == 5577) //是否为装备助手
		{
			pNpc->AddEffect("PearlFrontzhuangbeizhushou1");
		}
		if (pNpc->GetTypeID() == 310) //是否为拍卖管理员
		{
			pNpc->AddEffect("PearlFrontpaimai1");
		}
		if (pNpc->GetTypeID() == 93) //是否为邮差
		{
			pNpc->AddEffect("PearlFrontyouchai1");
		}
		if (pNpc->GetTypeID() == 5108) //是否为流浪者
		{
			pNpc->AddEffect("PearlFrontliulang1");
		}
		if (pNpc->GetTypeID() == 227||pNpc->GetTypeID() == 7005) //是否为大臣（王城）、大臣（凤凰城）
		{
			pNpc->AddEffect("PearlFrontdachen1");
		}
		if (pNpc->GetTypeID() == 803||pNpc->GetTypeID() == 1058
			||pNpc->GetTypeID() == 5105) //是否为杂货商
		{
			pNpc->AddEffect("PearlFrontzahuoshang1");
		}
		if (pNpc->GetTypeID() == 1 ||pNpc->GetTypeID() == 5164) //是否为仓库
		{
			pNpc->AddEffect("PearlFrontcangku1");
		}
		if (pNpc->GetTypeID() == 892) //是否为兵马大元帅
		{
			pNpc->AddEffect("PearlFrontbingma1");
		}
		if (pNpc->GetTypeID() == 55102) //增加21 32类型显示采集特效
		{
			pNpc->AddEffect("Caiji1");
		}		
		if (pNpc->GetTypeID() == 5245||pNpc->GetTypeID() == 5246||pNpc->GetTypeID() == 5247||pNpc->GetTypeID() == 5248)
		{
			pNpc->AddEffect("LoongPearlFrontLeft1");
			pNpc->AddEffect("LoongPearlFrontRight1");
			pNpc->AddEffect("LoongPearlBackLeft1");
			pNpc->AddEffect("LoongPearlBackRight1");
			pNpc->AddEffect("LoongPearlMiddle1");
		}
		m_listNpc.push_back(pNpc);
	}


	return pNpc;

	FUNCTION_END;
}

CNpc* CGameScene::AddNpc2(char* name,t_MapNpcData* pData)
{
	FUNCTION_BEGIN;

	CNpc* pNpc = FindNpcByID(pData->dwMapNpcDataPosition);

	if(pNpc) 
	{
		stAddMapNpcMapScreenUserCmd cmd;
		cmd.data = *pData;
		pNpc->PushCommand(&cmd,sizeof(cmd));
	}
	else
	{
		pNpc = new CNpc;
		//pNpc = LOOKASIDE_ALLOC(m_allocNpc);
		pNpc->SetAttrib(pData);

		//soke 2016 - 3 - 27 增加NPC的特效显示
		if (pNpc->GetTypeID() == 142||pNpc->GetTypeID() ==143
		    ||pNpc->GetTypeID() == 149||pNpc->GetTypeID() ==150
			||pNpc->GetTypeID() == 158||pNpc->GetTypeID() ==301
			||pNpc->GetTypeID() == 302||pNpc->GetTypeID() ==496
			||pNpc->GetTypeID() == 513||pNpc->GetTypeID() ==843
			||pNpc->GetTypeID() == 844||pNpc->GetTypeID() ==845
			||pNpc->GetTypeID() == 846||pNpc->GetTypeID() ==847
			||pNpc->GetTypeID() == 5001||pNpc->GetTypeID() ==5020
            ||pNpc->GetTypeID() == 5093||pNpc->GetTypeID() ==5122
			||pNpc->GetTypeID() == 5131||pNpc->GetTypeID() ==5094
			||pNpc->GetTypeID() == 5099||pNpc->GetTypeID() ==5096
			||pNpc->GetTypeID() == 5150||pNpc->GetTypeID() ==5155
			||pNpc->GetTypeID() ==1023) //是否为车夫
		{
			pNpc->AddEffect("PearlFrontchefu1");
		}
		if (pNpc->GetTypeID() == 887) //是否为福利姐姐
		{
			pNpc->AddEffect("PearlFrontfuli1");
		}
		if (pNpc->GetTypeID() == 99||pNpc->GetTypeID() == 884) //是否为高级杂货
		{
			pNpc->AddEffect("PearlFrontgaoji1");
		}
		if (pNpc->GetTypeID() == 361||pNpc->GetTypeID() == 854) //是否为高级钱庄老板
		{
			pNpc->AddEffect("PearlFrontlaoban1");
		}
		if (pNpc->GetTypeID() == 213||pNpc->GetTypeID() == 214
			||pNpc->GetTypeID() == 215||pNpc->GetTypeID() == 216) //是否为武器锻造师
		{
			pNpc->AddEffect("PearlFrontwuqiduanzao1");
		}
		if (pNpc->GetTypeID() == 217||pNpc->GetTypeID() == 218) //是否为首饰锻造师
		{
			pNpc->AddEffect("PearlFrontshoushiduanzao1");
		}
		if (pNpc->GetTypeID() == 211||pNpc->GetTypeID() == 212) //是否为防具锻造师
		{
			pNpc->AddEffect("PearlFrontfangjuduanzao1");
		}
		if (pNpc->GetTypeID() == 5577) //是否为装备助手
		{
			pNpc->AddEffect("PearlFrontzhuangbeizhushou1");
		}
		if (pNpc->GetTypeID() == 310) //是否为拍卖管理员
		{
			pNpc->AddEffect("PearlFrontpaimai1");
		}
		if (pNpc->GetTypeID() == 93) //是否为邮差
		{
			pNpc->AddEffect("PearlFrontyouchai1");
		}
		if (pNpc->GetTypeID() == 5108) //是否为流浪者
		{
			pNpc->AddEffect("PearlFrontliulang1");
		}
		if (pNpc->GetTypeID() == 227||pNpc->GetTypeID() == 7005) //是否为大臣（王城）、大臣（凤凰城）
		{
			pNpc->AddEffect("PearlFrontdachen1");
		}
		if (pNpc->GetTypeID() == 803||pNpc->GetTypeID() == 1058
			||pNpc->GetTypeID() == 5105) //是否为杂货商
		{
			pNpc->AddEffect("PearlFrontzahuoshang1");
		}
		if (pNpc->GetTypeID() == 1 ||pNpc->GetTypeID() == 5164) //是否为仓库
		{
			pNpc->AddEffect("PearlFrontcangku1");
		}
		if (pNpc->GetTypeID() == 892) //是否为兵马大元帅
		{
			pNpc->AddEffect("PearlFrontbingma1");
		}
		if (pNpc->GetTypeID() == 55102) //增加21 32类型显示采集特效
		{
			pNpc->AddEffect("Caiji1");
		}		
		if (pNpc->GetTypeID() == 5245||pNpc->GetTypeID() == 5246||pNpc->GetTypeID() == 5247||pNpc->GetTypeID() == 5248)
		{
			pNpc->AddEffect("LoongPearlFrontLeft1");
			pNpc->AddEffect("LoongPearlFrontRight1");
			pNpc->AddEffect("LoongPearlBackLeft1");
			pNpc->AddEffect("LoongPearlBackRight1");
			pNpc->AddEffect("LoongPearlMiddle1");
		}
		// if (pNpc->GetTypeID() == 5146) //增加21 32类型显示采集特效
		// {
		// 	char names[MAX_NAMESIZE+1];
		// 	sprintf(names,"本服天下第一：%s",name);
		// 	pNpc->SetName(names);
		// }

		m_listNpc.push_back(pNpc);
	}


	return pNpc;

	FUNCTION_END;
}


/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pData : 描述
* \return 返回值的描述
*/
bool CGameScene::AddMainCharacter(t_MainUserData* pData)
{
	FUNCTION_BEGIN;

	CCharacter* pCharacter = FindCharacterByID(pData->dwUserTempID);
	if(pCharacter)
	{
		//Assert(pCharacter == m_pMainCharacter);
		Assert(pCharacter->GetID() == pData->dwUserTempID);
		m_pMainCharacter->m_dwID = pData->dwUserTempID;

		stMainUserDataUserCmd cmd;
		cmd.data = *pData;
		return pCharacter->PushCommand(&cmd,sizeof(cmd));
	}
	else
	{
		ReLoadSkillTable();
		pCharacter = m_pMainCharacter;
		m_pMainCharacter->SetAttrib(pData);
		m_listCharacter.push_back(pCharacter);
		return true;
	}

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pData : 描述
* \return 返回值的描述
*/
CNpc* CGameScene::AddCharacter(t_MapUserData* pData)
{
	FUNCTION_BEGIN;

	if (pData == NULL)
		return NULL;

	if ( m_pMainCharacter->m_dwID == -1 )
		return NULL;

	CCharacter* pCharacter = FindCharacterByID(pData->dwUserTempID);
	if(pCharacter) 
	{
		stAddUserMapScreenUserCmd cmd;
		//cmd.data = *pData;
		memcpy(&(cmd.data),pData,sizeof(cmd.data));
		pCharacter->PushCommand(&cmd,sizeof(cmd));
	}
	else
	{
		if(pData->dwUserTempID == m_pMainCharacter->m_dwID)
		{
			pCharacter = m_pMainCharacter;
		}
		else
		{
			//Add new character
			pCharacter = new CCharacter;
		}

		pCharacter->SetAttrib(pData);

		//if is changing face
		if ( isset_state( pCharacter->GetState(), USTATE_CHANGEFACE )
			|| isset_state( pCharacter->GetState(), USTATE_COMMON_TEMP_USE)
			|| isset_state( pCharacter->GetState(), USTATE_ZHUTOU))//猪头术
		{
			pCharacter->ChangeFace( pData->dwChangeFaceID );
		}
		//end change face
		m_listCharacter.push_back(pCharacter);
	}
	return (CNpc*)pCharacter;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pData : 描述
* \return 返回值的描述
*/
bool CGameScene::AddItem(t_MapObjectData* pData,bool bDrop)
{
	FUNCTION_BEGIN;

	CItem* pItem = FindItemByID(pData->dwMapObjectTempID);
	if(!pItem)
	{
		pItem = new CItem;
		//pItem = LOOKASIDE_ALLOC(m_allocItem);
		m_listItem.push_back(pItem);
		pItem->m_CreateTime = xtimeGetTime();
	}
	pItem->SetAttrib(pData,bDrop);

	//When we are being auto PK mode we try pickup this item
	if ( GetGameApplication()->bClientSetted( enumAuto_Kill_Summon ) )
	{
		if ( GetMainCharacter() && pItem && 0
			&& ( (pItem->GetKind() > 0) || (pItem->GetItemObjectType() == ItemType_SOULSTONE) ) )
		{
			GetMainCharacter()->TryPickUpItem( pItem );
		}
	}

	return true;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pData : 描述
* \return 返回值的描述
*/
CPet* CGameScene::AddPet(petType type,DWORD dwID)
{
	FUNCTION_BEGIN;

	//TRACE("AddPet:[%d][%d]\n",type ,dwID);
	CPet* pPet = FindPetByType( type );
	if(!pPet)
	{
		if( type == PET_TYPE_RIDE )
		{
			//pPet = LOOKASIDE_ALLOC(m_allocHorse);
			Debug_String("Add Horse");
			pPet = new CHorse();
			m_listPet.push_back(pPet);
		}
		else
		{
			//pPet = LOOKASIDE_ALLOC(m_allocPet);
			pPet = new CPet();
			m_listPet.push_back(pPet);
		}		
	}
	pPet->SetPetType( type );
	pPet->SetID(dwID);
	pPet->SetDeleted(false);

	return pPet;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pData : 描述
 * \return 返回值的描述
 */
bool CGameScene::AddBuild(t_MapBuildingData* pData)
{
	FUNCTION_BEGIN;

	return false;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pCmd : 描述
 * \return 返回值的描述
 */
bool CGameScene::RemoveNpc(stRemoveMapNpcMapScreenUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	CNpc* pNpc = FindNpcByID(pCmd->dwMapNpcDataPosition);
	if(pNpc)
	{
		Engine_WriteLogF("删除 NPC %d\n",pCmd->dwMapNpcDataPosition);
		return pNpc->PushCommand(pCmd,sizeof(*pCmd));
	}else{
		Engine_WriteLogF("[REMOVE] 未找到NPC %d\n",pCmd->dwMapNpcDataPosition);
	}
	return false;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pCmd : 描述
 * \return 返回值的描述
 */
bool CGameScene::RemoveCharacter(stRemoveUserMapScreenUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	CCharacter* pCharacter = FindCharacterByID(pCmd->dwUserTempID);
	if(pCharacter)
	{
#ifdef _DEBUG
		//Engine_WriteLogF("RemoveCharacter ID:%d,pos(%d,%d)\n",pCharacter->GetID(),pCharacter->GetGridPos().x,pCharacter->GetGridPos().y);
#endif
		if( pCharacter->IsMainRole() )
		{
			GetGameGuiManager()->EndNpcDialog();
			//Assert(0);
#if defined _DEBUG || defined RELEASE_TEST
			Engine_WriteLogF("===>警告：服务器删除主角,ID:%d\n",pCmd->dwUserTempID);
#endif
			return true;
		}

		//We delete this character from list when destruction
		//g_UserLocalInfo.DeleteCharacter( pCharacter );

		return pCharacter->PushCommand(pCmd,sizeof(*pCmd));
	}

	return false;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pCmd : 描述
 * \return 返回值的描述
 */
bool CGameScene::RemoveItem(stRemoveMapObjectMapScreenUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	
	CItem* pItem = FindItemByID(pCmd->dwMapObjectTempID);
	if(pItem) 
	{
		//TRACE("删除物品 %s:%u\n",pItem->GetName(),pItem->GetID());
		pItem->OnRemove();	
		return true;
	}
	return false;

	FUNCTION_END;
}


/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pCmd : 描述
* \return 返回值的描述
*/
bool CGameScene::RemovePet(stDelPetPetCmd* pCmd)
{
	FUNCTION_BEGIN;

	petType type = (petType)(pCmd->type);
	CPet* pPet = FindPetByType( type );
	if( pPet ) 
	{		
		pPet->SetDeleted( true );

		if(GetGameGuiManager()->m_guiPetDlg)
			GetGameGuiManager()->m_guiPetDlg->UpdateContents();

		if( type == PET_TYPE_RIDE )
		{
			Debug_String("delete horse");
		}
		
		if( type == PET_TYPE_PET || type == PET_TYPE_SUMMON)
		{
			//如果pPet不为空，说明该宠物肯定存在，如果该宠物存在，则FloatDlg肯定存在！
			if(GetGameGuiManager()->m_guiPetFloatDlg)
				GetGameGuiManager()->m_guiPetFloatDlg->Close();
		}		

		//if( type == PET_TYPE_PET )
		//{
		//	if(GetGameGuiManager()->m_guiPetFloatDlg)
		//		GetGameGuiManager()->m_guiPetFloatDlg->Close();
		//	return true;
		//}
		//if( type == PET_TYPE_SUMMON )
		//{
		//	if(GetGameGuiManager()->m_guiSummonFloatDlg)
		//		GetGameGuiManager()->m_guiSummonFloatDlg->Close();
		//	return true;
		//}
		return true;
	}
	return false;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pCmd : 描述
 * \return 返回值的描述
 */
bool CGameScene::RemoveBuild(stRemoveMapBuildingMapScreenUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	return false;

	FUNCTION_END;
}

bool IsCanChangeFaceToNpc(NpcBase_t* pNpc)
{
	if ( NULL == pNpc )
		return NULL;
	
	//return ( (pNpc->dwType == NpcTypeItem_Human )
	//	|| ( pNpc->dwType == NpcTypeItem_Normal )	
	//	|| ( pNpc->dwType == NpcTypeItem_Boss )
	//	|| ( pNpc->dwType == NpcTypeItem_SmBoss )
	//	|| ( pNpc->dwType == NpcTypeItem_Extract )
	//	/*|| ( pNpc->dwType == NpcTypeItem_Pet )*/
	//	|| ( pNpc->dwType == NpcTypeItem_ExtractBugbear )
	//	|| ( pNpc->dwType == NpcTypeItem_Beckon ) );

	return pNpc->dwType == NpcTypeItem_Normal;
}

CNpc* CGameScene::FindNearbySummon( DWORD dwID /* = -1 */ )
{
	CNpc* pNpc = NULL;

	if ( m_listNpc.size() > 0 )
	{
		int dis = 100;
		for( tListNpc::iterator itor=m_listNpc.begin(); itor!=m_listNpc.end(); itor++ )
		{
			if ( (*itor)->IsNpc() && (*itor)->IsNpcType( NPC_TYPE_ATTACK ) && !(*itor)->IsDie()
				&& ( (*itor)->IsCanAutoAttackNpc() )
				&& !( m_pInterestNpc && ((*itor) == m_pInterestNpc) && (m_InterestNpcBaseInfo.nAttackFailNum>=5)) )
			{

#ifdef _AUTOPLAY
				// 过滤不打的怪物
				if(gAutoGameConfig.setCullNPC.find((*itor)->GetTypeID()) != gAutoGameConfig.setCullNPC.end())
					continue;
#endif
				if ( dwID == -1 )
				{
					stPointI ptObs;
					if ( (Scene_GetDis(GetScene()->GetMainCharacter()->GetGridPos(), (*itor)->GetGridPos()) < dis)
						/*&& (GetScene()->GetTileFlagsNum(GetScene()->GetMainCharacter()->GetGridPos(), (*itor)->GetGridPos(), ptObs,TILE_BLOCK) < 1 )*/
						&& (GetScene()->GetTileFlagsNum(GetScene()->GetMainCharacter()->GetGridPos(), (*itor)->GetGridPos(), ptObs,TILE_MAGIC_BLOCK) < 1) )
					{
						dis = Scene_GetDis( GetScene()->GetMainCharacter()->GetGridPos(), (*itor)->GetGridPos() );
						pNpc = (*itor);
					}
				}
				else
				{
					
					bool bWWW = false;
					if ( dwID == NpcTypeItem_Resource )
					{
						if ( (*itor)->GetNpcTypeItem() == dwID )
							bWWW = true;
					}
					else
					{
						if ( (*itor)->GetTypeID() == dwID )
							bWWW = true;
					}

					if( bWWW )
					{
						stPointI ptObs;
						if ( (Scene_GetDis(GetScene()->GetMainCharacter()->GetGridPos(), (*itor)->GetGridPos()) < dis)
							/*&& (GetScene()->GetTileFlagsNum(GetScene()->GetMainCharacter()->GetGridPos(), (*itor)->GetGridPos(), ptObs,TILE_BLOCK) < 1 )*/
							&& ( (GetScene()->GetTileFlagsNum(GetScene()->GetMainCharacter()->GetGridPos(), (*itor)->GetGridPos(), ptObs,TILE_MAGIC_BLOCK) < 1) || !pNpc) )
						{
							dis = Scene_GetDis( GetScene()->GetMainCharacter()->GetGridPos(), (*itor)->GetGridPos() );
							pNpc = (*itor);
						}
					}
				}
			}			
		}
	}

	return pNpc;
}


CCharacter* CGameScene::FindNearbyCharacter()
{
	CCharacter* pChar = NULL;

	if ( m_listCharacter.size() > 0 )
	{
		int dis = 100;
		for( tListCharacter::iterator itor=m_listCharacter.begin(); itor!=m_listCharacter.end(); itor++ )
		{
			if ( (*itor)->IsCharacter() && !(*itor)->IsMainRole() )
			{
				stPointI ptObs;
				if ( (Scene_GetDis(GetScene()->GetMainCharacter()->GetGridPos(), (*itor)->GetGridPos()) < dis)
					/*&& (GetScene()->GetTileFlagsNum(GetScene()->GetMainCharacter()->GetGridPos(), (*itor)->GetGridPos(), ptObs,TILE_BLOCK) < 1 )*/
					&& (GetScene()->GetTileFlagsNum(GetScene()->GetMainCharacter()->GetGridPos(), (*itor)->GetGridPos(), ptObs,TILE_MAGIC_BLOCK) < 1) )
				{
					dis = Scene_GetDis( GetScene()->GetMainCharacter()->GetGridPos(), (*itor)->GetGridPos() );
					pChar = (*itor);
				}
			}
		}
	}

	return pChar;
}

bool CGameScene::IsMainCharacterHasCartAmong9Screen()
{
	if ( m_listNpc.size() > 0 )
	{
		tListNpc::iterator itor;

		for( itor=m_listNpc.begin(); itor!=m_listNpc.end();itor++ )
		{
			if ( (*itor)->IsMyCart() )
				return true;	
		}
	}

	return false;
}

DWORD CGameScene::FindOneCanMoveNpcRandom()
{
	//First we find from nearby
	if ( m_listNpc.size() > 0 )
	{
		tListNpc::iterator itor;

		int num = 0;
		for( itor=m_listNpc.begin(); itor!=m_listNpc.end();itor++ )
		{
			if ( (*itor)->IsCanChangeFaceTo() )
				num++ ;
		}

		int pos = (float)rand()/RAND_MAX * (num-1);
		int i=0;
		for( itor=m_listNpc.begin(); itor!=m_listNpc.end();itor++ )
		{
			if ( (*itor)->IsCanChangeFaceTo() )
			{
				if ( i == pos )
					return (*itor)->GetTypeID();

				i++ ;
			}			
		}
	}

	//Then we find from table
	{
		if ( g_tableNpcBase.empty() )
			return -1;
		else
		{
			tTableNpcBase::iterator itor;

			int num = 0;
			for( itor = g_tableNpcBase.begin(); itor!=g_tableNpcBase.end(); itor++ )
			{
				if ( IsCanChangeFaceToNpc( &(*itor) ) )
					num++ ;
			}

			if ( num == 0 )
				return -1;

			int pos = (float)rand()/RAND_MAX * (num-1);
			int i=0;
			for( itor = g_tableNpcBase.begin(); itor!=g_tableNpcBase.end(); itor++ )
			{
				if ( IsCanChangeFaceToNpc( &(*itor) ) )
				{
					if ( ( (i == pos) ) )
						return itor->dwID;

					i++ ;
				}				
			}
		}
	}	
	
	return 10001;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwID : 描述
 * \return 返回值的描述
 */
CNpc* CGameScene::FindAndRequestNpc(DWORD dwID)
{
	FUNCTION_BEGIN;

	Assert(0);
	CNpc* pNpc = FindNpcByID(dwID);
	if(pNpc) return pNpc;

	/*SetFrameAllocator fa;

	WaitCanSendCommand(MAPSCREEN_USERCMD);

	size_t len = sizeof(stRequestMapNpcDataMapScreenUserCmd) + sizeof(DWORD);

	stRequestMapNpcDataMapScreenUserCmd *cmd = (stRequestMapNpcDataMapScreenUserCmd*)FrameAllocator::alloc(len);

	constructInPlace(cmd);
	cmd->size	=	1;
	*((DWORD*)(cmd+1))	=	dwID;

	Client_SendCommand(cmd,len,false);*/
	//Engine_WriteLogF("向服务器请一个NPC %u\n",dwID);
	return NULL;

	FUNCTION_END;
}

 
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwID : 描述
 * \return 返回值的描述
 */
CCharacter* CGameScene::FindAndRequestCharacter(DWORD dwID)
{
	FUNCTION_BEGIN;
	Assert(0);
	CCharacter* pCharacter = FindCharacterByID(dwID);
	if(pCharacter) return pCharacter;

	/*SetFrameAllocator fa;

	WaitCanSendCommand(MAPSCREEN_USERCMD);

	size_t len = sizeof(stRequestUserDataMapScreenUserCmd) + sizeof(DWORD);

	stRequestUserDataMapScreenUserCmd *cmd = (stRequestUserDataMapScreenUserCmd*)FrameAllocator::alloc(len);
	constructInPlace(cmd);
	cmd->size	=	1;
	*((DWORD*)(cmd+1))	=	dwID;

	Client_SendCommand(cmd,len,false);*/
	//Engine_WriteLogF("向服务器请一个人物 %u\n",dwID);
	return NULL;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGameScene::Center()
{
	FUNCTION_BEGIN;

	stPointI pt = m_pMainCharacter->GetPos();
	m_ptPixelOffset.x = pt.x - m_iPixelWidth/2;
	m_ptPixelOffset.y = pt.y - m_iPixelHeight/2;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param list : 描述
 * \param pExcept : 描述
 * \param bGetNpc : 描述
 * \return 返回值的描述
 */
void CGameScene::GetFullScreenCharacter(tStackVectorICharacter & list,iCharacter* pExcept,bool bGetNpc)
{
	FUNCTION_BEGIN;

	if(bGetNpc)
	{
		for(tListNpc::iterator it = m_listNpc.begin(); it != m_listNpc.end(); ++it)
		{
			if((*it)->GetInterface() != pExcept)
				list.push_back((*it)->GetInterface());
		}
	}

	for(tListCharacter::iterator it = m_listCharacter.begin(); it != m_listCharacter.end(); ++it)
	{
		if((*it)->GetInterface() != pExcept)
			list.push_back((*it)->GetInterface());
	}

	FUNCTION_END;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param time_eslasped : 描述
 * \return 返回值的描述
 */
void CGameScene::BuildRun(long time_eslasped)
{
	FUNCTION_BEGIN;

	for(tListBuild::iterator it = m_listBuild.begin();it != m_listBuild.end();)
	{
		if((*it)->CanRemove())
		{
			delete (*it);
			//it = m_listBuild.erase(it);
			SimpleRemoveFromArray(m_listBuild,it - m_listBuild.begin());
		}
		else
		{
			(*it)->Run(time_eslasped);
			if((*it)->InScreen())
				m_aDrawObjects.push_back(*it);
			++it;
		}
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param time_eslasped : 描述
 * \return 返回值的描述
 */
void CGameScene::ItemRun(long time_eslasped)
{
	FUNCTION_BEGIN;

	for(tListItem::iterator it = m_listItem.begin();it != m_listItem.end();)
	{
		if((*it)->CanRemove())
		{
			delete (*it);
			it = m_listItem.erase(it);
		}
		else
		{
			(*it)->Run(time_eslasped);
			if((*it)->InScreen())
				m_aDrawObjects.push_back(*it);
			++it;
		}
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param time_eslasped : 描述
 * \return 返回值的描述
 */
void CGameScene::NpcRun(long time_eslasped)
{
	FUNCTION_BEGIN;

	for(tListNpc::iterator it = m_listNpc.begin();it != m_listNpc.end();)
	{
		if((*it)->CanRemove())
		{
			delete (*it);
			it = m_listNpc.erase(it);
		}
		else
		{
			(*it)->Run(time_eslasped);
			if((*it)->InScreen())
			{
				if((*it)->IsAshes())	m_aDrawLowest.push_back(*it);
				else				m_aDrawObjects.push_back(*it);
			}
			++it;
		}
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param time_eslasped : 描述
 * \return 返回值的描述
 */
void CGameScene::CharacterRun(long time_eslasped)
{
	FUNCTION_BEGIN;
	for(tListCharacter::iterator it = m_listCharacter.begin();it != m_listCharacter.end();)
	{
		if((*it)->CanRemove())
		{
			delete (*it);
			//m_allocCharacter.freePointer(*it);
			//SimpleRemoveFromArray(m_listCharacter,it - m_listCharacter.begin());
			it = m_listCharacter.erase(it); 
		}
		else
		{
			(*it)->Run(time_eslasped);
			if((*it)->InScreen())
			{
				if((*it)->IsAshes())	m_aDrawLowest.push_back(*it);
				else				m_aDrawObjects.push_back(*it);
			}
			++it;
		}
	}
	FUNCTION_END;
}

void CGameScene::RemvoeMagicByCharacter(CNpc* pNpc)
{
	for(tListMagic::iterator it = m_listMagic.begin();it != m_listMagic.end();)
	{
		CMagic* pMagic = *it;
		if(pMagic->GetInterface() && 
			(pMagic->GetInterface()->GetSender() == pNpc->GetInterface() || pMagic->GetInterface()->GetTarget() == pNpc->GetInterface()) )
		{
			delete (*it);
			//m_allocMagic.freePointer(*it);
			it = m_listMagic.erase(it);
		}
		else ++it;
	}
}
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param time_eslasped : 描述
 * \param listGenerateMagic : 描述
 * \return 返回值的描述
 */
void CGameScene::MagicRun(long time_eslasped,tagListMagicGenerate& listGenerateMagic)
{
	FUNCTION_BEGIN;

	for(tListMagic::iterator it = m_listMagic.begin();it != m_listMagic.end();)
	{
		if((*it)->CanRemove())
		{
			delete (*it);
			it = m_listMagic.erase(it);
		}
		else
		{
#ifdef _DEBUG
			iMagic * pMagic  = (iMagic *)*it;
#endif 

			(*it)->Run(time_eslasped);
			(*it)->Generate(listGenerateMagic);
			if((*it)->CanRemove())
			{
				delete (*it);
				it = m_listMagic.erase(it);
			}
			else
			{
				if((*it)->InScreen())
				{
					switch((*it)->GetSortType())
					{
					case stMoveDataStruct::Sort_Lowest:
						m_aDrawLowest.push_back(*it);
						break;
					case stMoveDataStruct::Sort_Frontest:
						m_aDrawFrontest.push_back(*it);
						break;
					case stMoveDataStruct::Sort_AboveAll:
						m_aDrawAboveAll.push_back(*it);
						break;
					default:

						m_aDrawObjects.push_back(*it);
						break;
					}
				}

				++it;
			}
		}
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param event : 描述
 * \param pObj : 描述
 * \return 返回值的描述
 */
void CGameScene::OnEvent(stEvent event,iObj* pObj)
{
	FUNCTION_BEGIN;

	SetFrameAllocator fa;
	switch(event.eEvent) 
	{
	case stEvent::Event_Hit:
		{
			RangeEnum eRange = (RangeEnum)(event.nEventID);
			int magicHit = Magic_GetMagicID(event.szEventData);

			tStackVectorICharacter listCharacter;
			listCharacter.reserve(512);
			iCharacter* pSendCharacter = NULL;
			switch(pObj->GetObjType())
			{
			case  iObj::eObjType_Character:
				pSendCharacter = (iCharacter*)pObj;
				break;
			case iObj::eObjType_Magic:
				pSendCharacter = ((iMagic*)pObj)->GetSender();
				break;
			}
			GetCharacter(eRange,listCharacter,pObj->GetPos(),pObj->GetDir(),pSendCharacter);

			for (tStackVectorICharacter::iterator itC = listCharacter.begin(); itC != listCharacter.end(); ++itC)
			{
				iCharacter* pCh = *itC;
				CNpc* pNpc = (pCh->GetObjType() == iObj::eObjType_Character ? (CNpc*)pCh->GetData() : NULL);

				if (pNpc == NULL)
				{
					Assert(0);

					pCh->Hurt();
					if (magicHit >= 0)
					{
						pCh->SetMagicEffect(magicHit,0,0);
					}
				}
				else
				{
					if ( !pNpc->IsCharacter())
					{
						if( pNpc->IsNpcType(NPC_TYPE_FUNCTION) || pNpc->IsNpcType(NPC_TYPE_TRANSPORT)) 
							continue;
					}

					if (pSendCharacter)
					{
						CNpc* pSender = (CNpc*)pSendCharacter->GetData();
						if (pNpc->OnMagicHurt(pSender))
						{
							if (magicHit > 0)
								pCh->SetMagicEffect(magicHit,0,0);
						}
					}
					else
					{
						if (pNpc->OnMagicHurt(NULL))
						{
							if (magicHit > 0)
								pCh->SetMagicEffect(magicHit,0,0);
						}
					}
				}
			}
		}
		break;
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param listEvent : 描述
 * \return 返回值的描述
 */
void CGameScene::OnEventEx(std::vector<stEventEx>& listEvent)
{
	FUNCTION_BEGIN;

	for (std::vector<stEventEx>::iterator it2 = listEvent.begin(); it2 != listEvent.end(); ++it2)
	{
		OnEvent((*it2),(*it2).pSender);
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param listEvent : 描述
 * \param pObj : 描述
 * \return 返回值的描述
 */
void CGameScene::OnEvent(std::list<stEvent>& listEvent,iObj* pObj)
{
	FUNCTION_BEGIN;

	iCharacter* pSrcCh = (pObj->GetObjType() == iObj::eObjType_Character ? (iCharacter*)pObj : NULL);
	for (std::list<stEvent>::iterator it2 = listEvent.begin(); it2 != listEvent.end(); ++it2)
	{
		OnEvent((*it2),pObj);
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pCmd : 描述
 * \return 返回值的描述
 */
void CGameScene::OnAllCharPostion(stAllMapScreenUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	//DWORD tStartTime = xtimeGetTime();

	SetFrameAllocator fa;

	std::vector<DWORD ,std::stack_allocator<DWORD> > vecNpcID;
	std::vector<DWORD ,std::stack_allocator<DWORD> > vecUserID;

	vecNpcID.reserve(1024);
	vecUserID.reserve(1024);
	
	BYTE* pCmdData = (BYTE*)(&pCmd->mdih);
	
	for(;;)
	{
		MapData_ItemHeader* ihdr = (MapData_ItemHeader*)pCmdData;
		if(ihdr->size == 0) break;
		TRACE("OnAllCharPostion type:%d,num:%d\n",ihdr->type,ihdr->size);
		stNpcTempID* pNpcPos = (stNpcTempID*)(ihdr+1);
		stNpcTempID* end = pNpcPos + ihdr->size;
		pCmdData = (BYTE*)end;
		switch( ihdr->type )
		{
		case MAPDATATYPE_NPC:
			for(tListNpc::iterator it = m_listNpc.begin(); it != m_listNpc.end(); ++it)
			{
				if(!(*it)->IsDie() && !(*it)->IsDieWaitingMagic())
					(*it)->m_bDeleted = true;
			}
			for( ; pNpcPos != end ; ++pNpcPos )
			{
				DWORD id = pNpcPos->dwTempID;
				CNpc* pNpc = NULL;

				tListNpc::iterator it;
				for( it = m_listNpc.begin(); it != m_listNpc.end(); ++it )
				{
					if( (*it)->GetID() == id )
					{
						(*it)->m_bDeleted = false;
						pNpc = (*it);
						break;
					}
				}
				
				if( pNpc == NULL )
				{
					vecNpcID.push_back(id);
				}
				else
				{
					/*stPointI ptGrid (pNpcPos->x,pNpcPos->y);
					if(pNpc->GetGridPos() != ptGrid)
					{
						stRTMagicPosUserCmd  cmd;
						cmd.pos = *pNpcPos;
						cmd.byTarget = MAPDATATYPE_NPC;
						pNpc->PushCommand(&cmd,sizeof(cmd));
					}*/
				}
			}
			break;
		case MAPDATATYPE_USER:
			for(tListCharacter::iterator it = m_listCharacter.begin(); it != m_listCharacter.end(); ++it)
			{
				if(!(*it)->IsDie() && !(*it)->IsDieWaitingMagic() && !(*it)->IsMainRole() )
					(*it)->m_bDeleted = true;
			}
			for( ; pNpcPos != end ; ++pNpcPos )
			{
				DWORD id = pNpcPos->dwTempID;
				CNpc* pNpc = NULL;
				tListCharacter::iterator it;
				for( it = m_listCharacter.begin(); it != m_listCharacter.end(); ++it )
				{
					if( (*it)->GetID() == id )
					{
						(*it)->m_bDeleted = false;
						pNpc = (*it);
						break;
					}
				}

				if( pNpc == NULL )
				{
					vecUserID.push_back(id);
				}
				else
				{
					/*stPointI ptGrid (pNpcPos->x,pNpcPos->y);
					if(pNpc->GetGridPos() != ptGrid)
					{
						stRTMagicPosUserCmd  cmd;
						cmd.pos = *pNpcPos;
						cmd.byTarget = MAPDATATYPE_USER;
						pNpc->PushCommand(&cmd,sizeof(cmd));
					}*/
				}
			}
			break;
		}
	}

	//size_t len;
	if(vecNpcID.size())
	{
		/*len = sizeof(stRequestMapNpcDataMapScreenUserCmd) + sizeof(DWORD) * vecNpcID.size();
		stRequestMapNpcDataMapScreenUserCmd *cmd = (stRequestMapNpcDataMapScreenUserCmd*)FrameAllocator::alloc(len);
		constructInPlace(cmd);
		cmd->size = vecNpcID.size();
		DWORD* pID = (DWORD*)(cmd + 1);
		memcpy(pID,&vecNpcID[0],vecNpcID.size() * sizeof(DWORD));
		WaitCanSendCommand(MAPSCREEN_USERCMD);
		Client_SendCommand(cmd,len,false);*/

		Engine_WriteLogF("向服务器请NPC 数 %u\n",vecNpcID.size());
	}

	if(vecUserID.size())
	{
		/*len = sizeof(stRequestUserDataMapScreenUserCmd) + sizeof(DWORD) * vecUserID.size();
		stRequestUserDataMapScreenUserCmd *cmd = (stRequestUserDataMapScreenUserCmd*)FrameAllocator::alloc(len);
		constructInPlace(cmd);
		cmd->size = vecUserID.size();
		DWORD* pID = (DWORD*)(cmd + 1);
		memcpy(pID,&vecUserID[0],vecUserID.size() * sizeof(DWORD));
		WaitCanSendCommand(MAPSCREEN_USERCMD);
		Client_SendCommand(cmd,len,false);*/

		Engine_WriteLogF("向服务器请求人物数 %u\n",vecUserID.size());
	}

	//TRACE( "\nOn all character time = %u",xtimeGetTime() - tStartTime );

	FUNCTION_END;
}

void CGameScene::CalculateDrawRect()
{
	//if ( GetScene()->GetMainCharacter() && isset_state(GetScene()->GetMainCharacter()->GetState(), USTATE_BLIND) )
	if ( 0 )
	{
		stPointI ptMapPixelOffset = GetPixelOffset();
		stPointI ptCenterGrid = GetScene()->GetMainCharacter()->GetGridPos();
		stPointI ptPixelOffset = GridCenterToPixelPos( ptCenterGrid );
		
		const float blendRange = 1.0f;
		// 绘制区域
		m_rcDrawPixelRect.left = getMax( 0L, (long)(ptPixelOffset.x-GRID_WIDTH*blendRange) );
		m_rcDrawPixelRect.right = getMin( (long)m_map.GetWidth()*GRID_WIDTH, (long)(ptPixelOffset.x+GRID_WIDTH*blendRange) );
		m_rcDrawPixelRect.top = getMax( 0L, (long)(ptPixelOffset.y-GRID_HEIGHT*blendRange) );
		m_rcDrawPixelRect.bottom = getMin( (long)m_map.GetHeight()*GRID_HEIGHT, (long)(ptPixelOffset.y+GRID_HEIGHT*blendRange) );

		// 以网格尺寸对齐的绘制区域
		m_rcDrawRect.left = getMax( 0L , m_rcDrawPixelRect.left / GRID_WIDTH );
		m_rcDrawRect.top = getMax( 0L , m_rcDrawPixelRect.right / GRID_HEIGHT );
		m_rcDrawRect.right = getMin( (long)m_map.GetWidth() , (m_rcDrawPixelRect.top + GRID_WIDTH - 1)/GRID_WIDTH );
		m_rcDrawRect.bottom = getMin( (long)m_map.GetHeight() , (m_rcDrawPixelRect.bottom + GRID_HEIGHT - 1)/GRID_HEIGHT );
	}
	else
		CMapScene::CalculateDrawRect();
}

void CGameScene::AddOrRemoveAllDynamicObstacle( bool bAdd /* = true */ )
{
	// Clear all character & npc dynamic obstacle
	for(tListNpc::iterator it = m_listNpc.begin(); it != m_listNpc.end(); ++it)
	{
		if ( bAdd )
			(*it)->AddDynamicObstacle( (*it)->GetGridPos() );
		else
			(*it)->RemoveDynamicObstacle();
	}

	for(tListCharacter::iterator it = m_listCharacter.begin() ; it != m_listCharacter.end(); ++it)
	{
		if ( bAdd )
			(*it)->AddDynamicObstacle( (*it)->GetGridPos() );
		else
			(*it)->RemoveDynamicObstacle();
	}
}

void CGameScene::RunSpringWelcomeEffect()
{
	static bool bPlayedWelcomeEffect = false;

	if ( !m_bSendLoadOK )
		return;

	//We just play this effect among special time range

	if ( !bPlayedWelcomeEffect )
	{
		//Get sys time   after server valid
		tm* localTime;
		time_t times;
		time(&times);
		times -= m_OffsetSpringComingTime;
		localTime = localtime( (const time_t*)&times );
		if ( localTime->tm_year+1900 < 2006 )
			return;

#ifdef _DEBUG
		if ( localTime->tm_mon > 1 )
			return;
		if ( (localTime->tm_mon == 0) && (localTime->tm_mday < 26) )
			return;
		if ( (localTime->tm_mon == 1) && (localTime->tm_mday > 3) )
			return;
#else
		//活动时间	1-26 到 2-3

		if ( localTime->tm_mon > 1 )
			return;
		if ( (localTime->tm_mon == 0) && (localTime->tm_mday < 26) )
			return;
		if ( (localTime->tm_mon == 1) && (localTime->tm_mday > 3) )
			return;
#endif
		

		//Play effect
		{
			AddMagic( 628, GetMainCharacter()->GetInterface()->GetPos() );

			bPlayedWelcomeEffect = true;
		}
	}
}

void CGameScene::GMStartSpringComingEffect()
{
	m_bGMStartSpringComingEffect = true;

	time_t lTime;
	time( &lTime );
	StartSpringComingEffect( lTime );	
}

void CGameScene::StartSpringComingEffect( const time_t& sTime )
{
	//Get local time
	time_t times;
	time(&times);

	//Server time offset
	m_OffsetSpringComingTime = times - sTime;

	//Start times
	m_bStartSpringComingEffect = true;
}

bool CGameScene::IsRoralCityWarTime()
{

#ifdef  _DEBUG
	return true;
#else
	tm* serverTime;
	serverTime = localtime( (const time_t*)&m_ServerTime );

	if ( (serverTime->tm_mday % 2) > 0 )
		return false;
	if ( serverTime->tm_hour < 20 )
		return false;
	if ( serverTime->tm_hour > 22 )
		return false;
	if ( (serverTime->tm_hour == 20) && (serverTime->tm_min < 40) )
		return false;
	if ( (serverTime->tm_hour == 22) && (serverTime->tm_min > 40) )
		return false;

	return true;
#endif 

}

bool CGameScene::IsCallDutyCanUse()
{
	return true;
}

void CGameScene::RunSpringComingEffect( long time_eslasped )
{
	return;
	if ( !m_bStartSpringComingEffect )
		return;
    
	static bool bReallyStart = false;
	if ( !bReallyStart )
	{
#ifdef _DEBUG
		if ( m_bGMStartSpringComingEffect )
			bReallyStart = true;
		else
		{
			tm* localTime;
			time_t times;
			time(&times);
			times -= m_OffsetSpringComingTime;
			localTime = localtime( (const time_t*)&times );
			if ( localTime->tm_year+1900 < 2006 )
				return;
			if ( localTime->tm_mon < 0 )
				return;
			if ( localTime->tm_mday < 28 )
				return;
			if ( localTime->tm_hour < 23 )
				return;
			if ( localTime->tm_min < 59 )
				return;
			if ( localTime->tm_sec < 45 )
				return;

			bReallyStart = true;
		}
#else
	   //Run spring time
		if ( m_bGMStartSpringComingEffect )
			bReallyStart = true;
		else
		{
			tm* localTime;
			time_t times;
			time(&times);
			times -= m_OffsetSpringComingTime;
			localTime = localtime( (const time_t*)&times );
			if ( localTime->tm_year+1900 != 2006 )
				return;
			if ( localTime->tm_mon != 0 )
				return;
			if ( localTime->tm_mday != 28 )
				return;
			if ( localTime->tm_hour != 23 )
				return;
			if ( localTime->tm_min != 59 )
				return;
			if ( localTime->tm_sec < 45 )
				return;

			bReallyStart = true;
		}		
#endif

		
		
	}

	if ( !bReallyStart )
		return;
	//

	//Really start spring coming effect
	const int cStartTime = 15;
	static time = xtimeGetTime();
	static lastBackTime = cStartTime;

	int nBackTime = xtimeGetTime() - time;
	nBackTime /= 1000;
	nBackTime = cStartTime - nBackTime;

	if ( nBackTime > 10 )
		return;

	if( nBackTime == 10)
	{
		PlayBackMusic("DJS.mp3",0,0,-1); //
		PlayBackMusic("NWJX.mp3",0,0,0,false);
	}

	if ( (nBackTime<=10) && (nBackTime>=0) )
	{
		//Play firework
		D3DXVECTOR3 vEffect;
		vEffect.x = GetDevice()->GetWidth() / 2;
		vEffect.y = GetDevice()->GetHeight() / 2;
		vEffect.z = 0;
		if ( lastBackTime != nBackTime )
		{
			char strFireName[MAX_NAMESIZE];
			sprintf( strFireName, "Spring\\BackTimeFireWork%d", nBackTime );
			if ( GetParticleSysMgr() )
			{
				GetParticleSysMgr()->LoadOneParticleSystem( strFireName, vEffect );
				CParticleSystem* pSys = GetParticleSysMgr()->GetActiveParticleSystem();
				if ( pSys )
				{
					pSys->UseCharacterOffset( false );
					pSys->Play();
					pSys->SetLoopPlay( false );
					pSys->SetSysLife( 2.0f );	//one second
					pSys->SetEmitRate( pSys->GetEmiterRate() * 10 );
					pSys->ResetBombStartTime( 0.6f );
					pSys->SetSizeScale( 10.0f, 1.0f );
				}
			}
			lastBackTime = nBackTime;
		}
	}

	/*CParticleSystem* pFireWork = NULL;
	if ( nBackTime == 0 )
	{
		//Pre load firework
		if ( GetParticleSysMgr() )
		{
			//Randomly get one pos
			D3DXVECTOR3 vEffect;
			vEffect.x = rand() % GetDevice()->GetWidth();
			vEffect.y = rand() % GetDevice()->GetHeight();
			vEffect.z = 0;

			GetParticleSysMgr()->LoadOneParticleSystem( "Spring\\SpringFireWork0", vEffect );
			pFireWork = GetParticleSysMgr()->GetActiveParticleSystem();
			pFireWork->UseCharacterOffset( false );			
			pFireWork->SetLoopPlay( false );
		}
	}

	if ( NULL == pFireWork )
		return;*/
	if ( nBackTime > 0 )
		return;

	//Add text
	static bool bPlayed = false;
	if ( !bPlayed && (nBackTime == 0) )
	{
		AddMagic( 628, GetMainCharacter()->GetInterface()->GetPos() );
		AddMagic( 634, GetMainCharacter()->GetInterface()->GetPos() );
		bPlayed = true;
	}

	//When spring comes we random play firework for 3 minutes
#ifdef _DEBUG
	if ( nBackTime > -3 * 60 )
#else
	if ( nBackTime > -3 * 60 )
#endif	
	{
		//Randomly play firework
		int numSys = rand() % 7;
		static bool bNeedPlay = true;
		if ( !bNeedPlay )
		{
			if ( (rand() % 1000)  < 100 )
				bNeedPlay = true;
		}
		bNeedPlay = false;

		if ( bNeedPlay )
		{
			if ( 0 )
			{
 				if ( GetParticleSysMgr() )
				{
					//Randomly get one pos
					D3DXVECTOR3 vEffect;
					vEffect.x = rand() % GetDevice()->GetWidth();
					vEffect.y = rand() % GetDevice()->GetHeight();
					vEffect.z = 0;

					char strName[MAX_NAMESIZE];
					int sysIndex = rand() % 10;
					if ( sysIndex < 5 )
						sysIndex = 0;
					else if ( sysIndex < 8 )
						sysIndex = 1;
					else if ( sysIndex < 9 )
						sysIndex = 2;
					else
						sysIndex = 3;
					sprintf( strName, "Spring\\SpringFireWork%d", sysIndex );
					GetParticleSysMgr()->LoadOneParticleSystem( strName, vEffect );
					CParticleSystem* pSys = GetParticleSysMgr()->GetActiveParticleSystem();
					if ( pSys )
					{
						pSys->UseCharacterOffset( false );

						//Set sys life
						float sysLife = pSys->GetSysLife();
						if ( sysIndex == 0 )
							sysLife *= 1.5f;
						pSys->Play( sysLife );
						pSys->SetLoopPlay( false );
						//Set size scale
						if ( sysIndex == 0 )
						{
							float sizeScale = abs(rand()%10 - rand()%5) / 5;
							if ( sizeScale < 0.5f )
								sizeScale = 0.5f;
							pSys->SetSizeScale( pSys->GetSizeScale() * sizeScale, 1.0f );
							//Random Set texture
							static const int tTexture[] = {3, 4, 15, 4};
							pSys->SetTexture( tTexture[rand() % 3] );
						}
						
						//Set suppress end time
						//pSys->SetSuppressTimeEnd( pSys->GetSuppressTimeEnd() * sizeScale );
						
						//Set color
						pSys->SetStartColorRed( (float)(rand() % 255)/255.0f );
						pSys->SetEndColorRed( (float)(rand() % 255)/255.0f );
						pSys->SetStartColorGreen( (float)(rand() % 255)/255.0f );
						pSys->SetEndColorGreen( (float)(rand() % 255)/255.0f );
						pSys->SetStartColorBlue( (float)(rand() % 255)/255.0f );
						pSys->SetEndColorBlue( (float)(rand() % 255)/255.0f );
						pSys->SetColorVarRed( (float)(rand() % 50)/255.0f );
						pSys->SetColorVarGreen( (float)(rand() % 50)/255.0f );
						pSys->SetColorVarBlue( (float)(rand() % 50)/255.0f );

						//Set move dir for type index=1
						if ( (sysIndex == 1) && pSys->GetMovement() )
						{
							CMovementLine* move = static_cast<CMovementLine*>(pSys->GetMovement());
							move->SetAngle( rand() % 360 );
						}
					}
				}
			}
			else
			{
				if ( 0 )
				{
					//Add effect form artist
					//Randomly get one pos
					for( int i=0;i<numSys;i++ )
					{
						stPointI ptPos = GetMainCharacter()->GetInterface()->GetPos();
						ptPos.x += RANDOM_NUM * GetDevice()->GetWidth()/2;
						ptPos.y += RANDOM_NUM * GetDevice()->GetHeight()/2;

						static const DWORD fireList[] = {626, 627, 630};
						DWORD dwFireID = ( rand() % 3 );
						dwFireID = fireList[dwFireID];

						AddMagic( dwFireID, ptPos );
					}	
				}
			}
			bNeedPlay = false;
		}
	}
	else
	{
        m_bStartSpringComingEffect = false;

	} 
}
//soke 国战反攻时间
bool CGameScene::IsAntiDarkeCountryWarTime()
{
	FUNCTION_BEGIN;

	tm* localTime;
	time_t times;
	time(&times);
	times -= m_OffsetSpringComingTime;
	localTime = localtime( (const time_t*)&times );
	
	
	//int tm_sec;     /* seconds after the minute - [0,59] */
	//int tm_min;     /* minutes after the hour - [0,59] */
	//int tm_hour;    /* hours since midnight - [0,23] */

	if( (localTime->tm_hour == 22 && localTime->tm_min >= 40)
		|| (localTime->tm_hour == 23 &&  localTime->tm_min <= 10) )
	{
		return true;
	}
	return false;
	  
	FUNCTION_END;
}
 
//soke 这个显示的玫瑰花（红）特效
void CGameScene::AddRoseRoseIOU()
{
	D3DXVECTOR3 vEffect;
	vEffect.x = GetDevice()->GetWidth() / 2;
	vEffect.y = GetDevice()->GetHeight() / 3;
	vEffect.y = 0;
	vEffect.z = 0;

	GetParticleSysMgr()->LoadOneParticleSystem( "Spring\\RoseRoseIOU", vEffect ); //桃子这种路径都在datas\particle\sampleeffect\spring里面
	CParticleSystem* pSys = GetParticleSysMgr()->GetActiveParticleSystem();
	if ( pSys )
	{
		pSys->UseCharacterOffset( false );

		//Set sys life						
		pSys->Play( 60 ); //soke 效果持续时间/秒
		pSys->SetLoopPlay( false );

		pSys->SetSuppressTimeStart( 0.0f );
		pSys->SetSuppressTimeEnd( 0.0f );
	}
}


//soke 天降洪福
void CGameScene::AddTianjiang()
{
	D3DXVECTOR3 vEffect;
	vEffect.x = GetDevice()->GetWidth() / 2;
	vEffect.y = GetDevice()->GetHeight() / 3;
	vEffect.y = 0;
	vEffect.z = 0;

	GetParticleSysMgr()->LoadOneParticleSystem( "Spring\\tianjianglonglu", vEffect ); //桃子这种路径都在datas\particle\sampleeffect\spring里面 网管改天降洪福特效为玫瑰花2024-6-7 10:32:26
	CParticleSystem* pSys = GetParticleSysMgr()->GetActiveParticleSystem();
	if ( pSys )
	{
		pSys->UseCharacterOffset( false );
		

		//Set sys life						
		pSys->Play( 30 ); //soke 效果持续时间/秒
		pSys->SetLoopPlay( false );

		pSys->SetSuppressTimeStart( 0.0f );
		pSys->SetSuppressTimeEnd( 0.0f );
	}
}
/////////////////////////////////////////////////////////////////////////////
//soke 这里显示的烟花特效
void CGameScene::AddRoseRoseIOX()
{
	D3DXVECTOR3 vEffect;
	vEffect.x = GetDevice()->GetWidth() / 2;
	vEffect.y = GetDevice()->GetHeight() / 3;
	vEffect.y = 0;
	vEffect.z = 0;

	GetParticleSysMgr()->LoadOneParticleSystem( "Spring\\SpringFireWork0", vEffect ); //soke 烟花特效
	CParticleSystem* pSys = GetParticleSysMgr()->GetActiveParticleSystem();
	if ( pSys )
	{
		pSys->UseCharacterOffset( false );

		//Set sys life						
		pSys->Play( 30 ); //soke 效果持续时间/秒
		pSys->SetLoopPlay( false );

		pSys->SetSuppressTimeStart( 0.0f );
		pSys->SetSuppressTimeEnd( 0.0f );
	}
}
//////////////////////////////////////////////////////////////////////////////

// add by snip3r
void CGameScene::AddMovie(CSceneMovie* pImg)
{
	m_listMovie.push_back(pImg);
}

void CGameScene::RemoveAllMovie()
{
	std::for_each(m_listMovie.begin(),m_listMovie.end(),stDeleteObject<CSceneMovie>());
}

void CGameScene::RunMovie(long time_elapsed)
{
	CGameScene::tListMovie::iterator itr = m_listMovie.begin();
	if ( m_dwCurTime == 0 )
	{
		for ( ; itr != m_listMovie.end(); ++itr )
		{
			(*itr)->Draw(m_dwCurTime);
		}
		m_dwCurTime += time_elapsed;
	}
	else
	{
		m_dwCurTime += time_elapsed;
		for ( ; itr != m_listMovie.end(); ++itr )
		{
			(*itr)->Draw(m_dwCurTime);
		}
	}
}

void CGameScene::StartMovieMode()
{
	b_MovieMode = true;
	m_dwCurTime = 0;
}

void CGameScene::EndMovieMode()
{
	b_MovieMode = false;
}
// end added by snip3r

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param time_eslasped : 描述
 * \return 返回值的描述
 */
void CGameScene::Run(long time_eslasped)
{
	FUNCTION_BEGIN;

	GetGameTime()->Run();

	RunSpringComingEffect( time_eslasped );
	RunSpringWelcomeEffect();

	RunQueueUnionSept();

	if ( GetGameTime()->IsDayNumChanged() )
	{
		if ( GetScene()->GetMainCharacter() )
			GetScene()->GetMainCharacter()->OnDayNumChange();
	}

	CMapScene::Run(time_eslasped);

	m_aDrawObjects.resize(0);
	m_aDrawLowest.resize(0);
	m_aDrawFrontest.resize(0);
	m_aDrawAboveAll.resize(0);
	Center();
	CalculateDrawRect();

	SetFrameAllocator fa;
	tagListMagicGenerate listMagic;
	listMagic.reserve(1024);
	BuildRun(time_eslasped);
	ItemRun(time_eslasped);
	MagicRun(time_eslasped,listMagic);
	NpcRun(time_eslasped);
	CharacterRun(time_eslasped);

	Sort();

	//这里写连斩系统 去判断 gui里面的时间 是否 大于 现在时间 如果大于 则 去刷新秒数 否则 人头置0 by醉梦
	time_t cur_time = time(NULL);
	if(GetGameGuiManager()->m_guiMain->lianzhantime>cur_time)
	{
		int fen = (GetGameGuiManager()->m_guiMain->lianzhantime-cur_time)/60;
		int miao = (GetGameGuiManager()->m_guiMain->lianzhantime-cur_time)- (60*fen);
		char text[256];
		sprintf(text,"当前连斩人数：%d人 剩余持续时间: %d分%d秒",GetGameGuiManager()->m_guiMain->lianzhannum,fen,miao);
		GetGameGuiManager()->m_guiMain->m_lianzhaninfo->SetText(text);
	}
	else{
		//置空
		GetGameGuiManager()->m_guiMain->m_lianzhaninfo->SetText("");
	}

	if(0 == strcmp(GetScene()->GetMapName(),"中立区·功勋符地图"))
	{
		timeb t;
		ftime(&t);
		time_t times;
		time(&times);
		times -= m_OffsetSpringComingTime;

		if(GetGameGuiManager()->m_guiMain->killpigtime !=0)
		{
			
			if(times - GetGameGuiManager()->m_guiMain->killpigtime/1000 >=10)
			{
				long long t1 = GetGameGuiManager()->m_guiMain->killpigtime/1000 + 60;
				if(t1>times)
				{
					int fen = (t1-times)/60;
					int miao = (t1-times)- (60*fen);
					char text[256];
					sprintf(text,"BOSS复活还剩%d分%d秒",fen,miao);
					GetGameGuiManager()->m_guiMain->m_gxbossinfo->SetText(text);
				}
				else{
					char text[256];
					sprintf(text,"BOSS已复活,击杀BOSS即可开奖");
					GetGameGuiManager()->m_guiMain->m_gxbossinfo->SetText(text);
				}
			}
			else{
				
				int fen = ((GetGameGuiManager()->m_guiMain->killpigtime/1000 - GetGameGuiManager()->m_guiMain->pigtime))/60;
				int miao = ((GetGameGuiManager()->m_guiMain->killpigtime/1000 - GetGameGuiManager()->m_guiMain->pigtime))- (60*fen);
				char text[256];
				if(GetGameGuiManager()->m_guiMain->killpigtime%1000>=100)
				{
					sprintf(text,"BOSS %d:%d:%d",fen,miao,(GetGameGuiManager()->m_guiMain->killpigtime%1000)%100);
				}
				else{
					sprintf(text,"BOSS %d:%d:%d",fen,miao,(GetGameGuiManager()->m_guiMain->killpigtime%1000));
				}
				GetGameGuiManager()->m_guiMain->m_gxbossinfo->SetText(text);
				
			}
		}
		else{
			if(times - GetGameGuiManager()->m_guiMain->pigtime <=120)
			{
				int fen = ((times - GetGameGuiManager()->m_guiMain->pigtime))/60;
				int miao = ((times - GetGameGuiManager()->m_guiMain->pigtime))- (60*fen);
				char text[256];
				if(t.millitm>=100)
				{
					sprintf(text,"BOSS %d:%d:%d",fen,miao,(t.millitm/10));
				}
				else{
					sprintf(text,"BOSS %d:%d:0%d",fen,miao,t.millitm/10);
				}
				GetGameGuiManager()->m_guiMain->m_gxbossinfo->SetText(text);
			}
			else{
				//置空
				char text[256];
				sprintf(text,"BOSS已复活,击杀BOSS即可开奖");
				GetGameGuiManager()->m_guiMain->m_gxbossinfo->SetText(text);
			}				
			
		}
	}
	else{
		GetGameGuiManager()->m_guiMain->m_gxbossinfo->SetText("");
	}
	

	UpdateMouseOverObject();

	//刷新鼠标选中的是哪个角色头顶的文字
	UpdateMouseOverTextNpc(m_listCharacter);

	Center();

	CalculateDrawRect();


	typedef std::vector< CNpc*,std::stack_allocator<CNpc*> > tagListNpc;

	tagListNpc listCharactor;
	listCharactor.reserve(1024);

	// 魔法事件
	for(tListMagic::iterator it = m_listMagic.begin(); it != m_listMagic.end(); ++it)
	{
		iMagic* p = (*it)->GetInterface();
		std::list<stEvent>& listEvent = p->GetEventList();
		OnEvent(listEvent,p);
		p->ClearEventList();
	}

	// 将所有人物，NPC 放入列表中
	for(tListNpc::iterator it = m_listNpc.begin(); it != m_listNpc.end(); ++it)
	{
		listCharactor.push_back(*it);
	}

	for(tListCharacter::iterator it = m_listCharacter.begin() ; it != m_listCharacter.end(); ++it)
	{
		listCharactor.push_back(*it);
	}

	for(tagListNpc::iterator it = listCharactor.begin(); it != listCharactor.end(); ++it)
	{
		iCharacter* p = (*it)->GetInterface();
		std::vector<stEventEx>& listEvent = p->GetEventList();
		OnEventEx(listEvent);
		p->ClearEventList();
	}

	/********* 碰撞检查 *************/
	for(tListMagic::iterator it = m_listMagic.begin(); it != m_listMagic.end(); ++it)
	{
		iMagic* p = (*it)->GetInterface();
		if (!p->IsBreak(stBlockStruct::Block_Character) && !p->IsBreak(stBlockStruct::Block_Self))
			continue;

		POINT pt = p->GetPos();
		POINT ptGrid = (*it)->GetGridPos();

		// 检测与目标的碰撞
		iCharacter* pDes = p->GetTarget();
		if (pDes)
		{
			if (p->IsBreak(pDes))
			{
				p->OnBreak(stBlockStruct::Block_Character,listMagic,pDes);
				CNpc* pNpc = (CNpc* )pDes->GetData();
				if (pNpc)
				{
					iCharacter* p2 = p->GetSender();
					CNpc* pSender = (CNpc* )p2->GetData();
					//pNpc may be deleted...

					// 由于碰撞产生伤害
					// pNpc->OnMagicHurt(pSender);
				}
				continue;
			}
		}

		// 检测与所有人物的碰撞
		for (tagListNpc::iterator it = listCharactor.begin();
			it != listCharactor.end();
			++it)
		{
			CNpc* pCh = *it;
			if (pCh->GetInterface()->IsDie())
				continue;

			if (p->IsBreak(pCh->GetInterface()))
			{
				if (
					(pCh->GetInterface() == p->GetSender() && p->IsBreak(stBlockStruct::Block_Self))
					||(pCh->GetInterface() != p->GetSender() && p->IsBreak(stBlockStruct::Block_Character))
					)
				{
					if(p->OnBreak(stBlockStruct::Block_Character,listMagic,pCh->GetInterface()))
					{
						iCharacter* p2 = p->GetSender();
						if( p2 )
						{
							CNpc* pSender = (CNpc* )p2->GetData();
							(*it)->OnMagicHurt(pSender);
						}                               						
					}
					break;
				}
			}
		}

		// 检测与障碍的碰撞
		if ( p->IsBreak(NULL) )
		{
			p->OnBreak(stBlockStruct::Block_Obstacle,listMagic,NULL);
		}
	}
	/******************/

	// 碰撞所产生的魔法
	for (tagListMagicGenerate::iterator it = listMagic.begin(); it != listMagic.end(); ++it)
	{
		iMagic* p = *it;
		AddMagic(p);
		p->Release();
	}

	static DWORD s_time = 0;
	//if(xtimeGetTime() - s_time > 50)
	{
		CSceneSound::SetListenerPosition(m_pMainCharacter->GetPos());
		//CSceneSound::SetListenerVelocity(m_pMainCharacter->GetInterface()->GetMoveSpeed());
		GetSoundManager()->CommitDeferredSettings();
		s_time = xtimeGetTime();
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param piMagic : 描述
 * \return 返回值的描述
 */
bool CGameScene::AddMagic(iMagic* piMagic)
{
	FUNCTION_BEGIN;

	CMagic* pMagic = new CMagic;
	//CMagic* pMagic = LOOKASIDE_ALLOC(m_allocMagic);
	pMagic->SetInterface(piMagic);
	m_listMagic.push_back(pMagic);
	return true;

	FUNCTION_END;
}

void CGameScene::AddMagic( size_t magicType, stPointI pos, int nSetLife /* = -1*/, int table /* = 0 */ )
{
	FUNCTION_BEGIN;

	//Add magic effect to scene
	iMagic* pMagic = Magic_CreateMagic( magicType, table );
	if ( !pMagic )
		return;
	pMagic->CreateMagic( false );
	if ( nSetLife != -1 )
		pMagic->SetLife( nSetLife ); 	
	pMagic->SetPos( pos );

	AddMagic( pMagic );

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param p1 : 描述
 * \param p2 : 描述
 * \return 返回值的描述
 */
inline bool compObj(CSceneObject* p1,CSceneObject* p2)
{
	FUNCTION_BEGIN;

	stPointI pt = p1->GetSortPos();
	int l1 = pt.x + (pt.y << 16);
	pt = p2->GetSortPos();
	int l2 = pt.x + (pt.y << 16);
	return l1 < l2;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGameScene::Sort()
{
	FUNCTION_BEGIN;

	std::sort(m_aDrawObjects.begin(),m_aDrawObjects.end(),compObj);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwColor : 描述
 * \return 返回值的描述
 */
void CGameScene::Draw(DWORD dwColor)
{
	FUNCTION_BEGIN;

	if ( GetScene()->GetMainCharacter()
		&& (isset_state(GetScene()->GetMainCharacter()->GetState(), USTATE_BLIND) || isset_state(GetScene()->GetMainCharacter()->GetState(), USTATE_CRAZY_ATTACK)) )
	{
		GetD3DDevice()->Clear( 0x00 );
		return;
	}

	OnOffMagicEffect( GetGameApplication()->bSystemSetted( USER_SETTING_EFFECT ) );

	DrawTerrain(dwColor);

	DrawTerrainEffect(dwColor);
	DrawSmTerrain(dwColor);
	DrawMapLowestObjects(dwColor);
	GetMapSurfManager()->Draw(GetPixelOffset());
#ifdef _DEBUG
	DrawObstacle(COLOR_ARGB(128,255,255,255));
#endif
	stPointI ptOffset = GetPixelOffset();

	GetFootprintManager().Draw(ptOffset);
	DrawLowestObjects(dwColor);

	std::for_each(m_listNpc.begin(),m_listNpc.end(),std::bind2nd(std::mem_fun1<void,CNpc,POINT>(&CNpc::DrawMagicLowest),ptOffset));
	std::for_each(m_listCharacter.begin(),m_listCharacter.end(),std::bind2nd(std::mem_fun1<void,CNpc,POINT>(&CNpc::DrawMagicLowest),ptOffset));
	
	//When current weather is rain, we render "shui hua" here
	if ( GetParticleSysMgr() )
	{
		CParticleSystem* pRainGround = GetParticleSysMgr()->FindParticleSys( "RainGround" );
		if ( pRainGround )
			// change 
			pRainGround->RenderUseEngineMethod( NULL );
			//pRainGround->RenderUseEngineMethod( GetD3DDevice()->GetDevice() );
	}	

	DrawObjects(dwColor);

	DrawCoverObject(dwColor);

	std::for_each(m_listNpc.begin(),m_listNpc.end(),std::bind2nd(std::mem_fun1<void,CNpc,POINT>(&CNpc::DrawMagicFrontest),ptOffset));
	std::for_each(m_listCharacter.begin(),m_listCharacter.end(),std::bind2nd(std::mem_fun1<void,CNpc,POINT>(&CNpc::DrawMagicFrontest),ptOffset));

	if(GetLightMap()->IsEnabled() && m_bLightMap)
	{
		stRectI rcLight(0,0,GetDevice()->GetWidth(),GetDevice()->GetHeight());
		GetLightMap()->Draw(&rcLight);
	}
	
	DrawFrontestObjects(dwColor);

	//Draw above all magic effect
	//This time only firework effect
	DrawAboveAllObjects(dwColor);   	
	
	std::for_each(m_listNpc.begin(),m_listNpc.end(),std::bind2nd(std::mem_fun1<void,CNpc,POINT>(&CNpc::DrawMagicAboveAll),ptOffset));
	std::for_each(m_listCharacter.begin(),m_listCharacter.end(),std::bind2nd(std::mem_fun1<void,CNpc,POINT>(&CNpc::DrawMagicAboveAll),ptOffset));

	int iFont = GetDevice()->SetFont(1);
	// 画文字
	
	std::for_each(m_aDrawLowest.begin(),m_aDrawLowest.end(),std::bind2nd(std::mem_fun1<void,CSceneObject,POINT>(&CSceneObject::DrawBack),ptOffset));
	std::for_each(m_aDrawLowest.begin(),m_aDrawLowest.end(),std::bind2nd(std::mem_fun1<void,CSceneObject,POINT>(&CSceneObject::DrawText),ptOffset));

	std::for_each(m_aDrawObjects.begin(),m_aDrawObjects.end(),std::bind2nd(std::mem_fun1<void,CSceneObject,POINT>(&CSceneObject::DrawBack),ptOffset));
	std::for_each(m_aDrawObjects.begin(),m_aDrawObjects.end(),std::bind2nd(std::mem_fun1<void,CSceneObject,POINT>(&CSceneObject::DrawText),ptOffset));

	GetDevice()->SetFont(iFont);

	//This is just a test
/*#ifdef _DEBUG
	IBitmaps* pBmp=NULL;
	pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",4,30)));
	if ( pBmp )
	{
		stRectI	rcSkill;
		rcSkill.left = GetDevice()->GetWidth() / 2 + 100;
		rcSkill.top = GetDevice()->GetHeight() / 2;
		pBmp->RenderAni( rcSkill.left, rcSkill.top, xtimeGetTime(), -1 );
	} 
#endif*/

	//End test

	//Add By David For 2006-3-9
	#ifdef _DEBUG
		GetMainCharacter()->RenderGridLine( );
		DrawShootGridList();
	#endif


	//#ifdef _DEBUG
	//GetDevice()->DrawString(GetGameTime()->GetTimeString(),stPointI(100,0),0xffffffff,FontEffect_Border);
	//#endif
	

	FUNCTION_END;
}



/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param iScreenID : 描述
 * \param list : 描述
 * \return 返回值的描述
 */
template < class tListObject >
void RemoveScreenObject(int iScreenID,tListObject & list)
{
	FUNCTION_BEGIN;

	for(tListObject::iterator it = list.begin(); it != list.end(); ++it)
	{
		stPointI pt = (*it)->GetGridPos();
		if(pt.x == -1 || pt.y == -1) continue;
		int id = GetScene()->GridPosToScreenID(pt);
		if(id == iScreenID)
		{
			(*it)->m_bDeleted = true;
#ifdef _DEBUG
			//Engine_WriteLogF("RemoveSceneObject, id:%d,pos(%d,%d)\n",(*it)->GetID(),pt.x,pt.y);
#endif
		}
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param iScreenID : 描述
 * \return 返回值的描述
 */
void CGameScene::RemoveScreenData(int iScreenID)
{
	FUNCTION_BEGIN;

	/*RemoveScreenObject(iScreenID,m_listNpc);
	RemoveScreenObject(iScreenID,m_listCharacter);*/
	RemoveScreenObject(iScreenID,m_listBuild);
	RemoveScreenObject(iScreenID,m_listItem);
	RemoveScreenObject(iScreenID,m_listMagic);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pScreenID : 描述
 * \param size : 描述
 * \return 返回值的描述
 */
void CGameScene::RequestScreenData(int * pScreenID,size_t size)
{
	FUNCTION_BEGIN;

	/*
	if(size == 0) return;

	WaitCanSendCommand(MAPSCREEN_USERCMD);

	size_t len = sizeof(stRequestMapDataMapScreenUserCmd) + sizeof(DWORD) * size;

	SetFrameAllocator fa;

	stRequestMapDataMapScreenUserCmd *pCmd = (stRequestMapDataMapScreenUserCmd*) FrameAllocator::alloc(len);

	pCmd->byCmdType			=	MAPSCREEN_USERCMD;
	pCmd->byParameterType	=	REQUESTMAPDATA_MAPSCREEN_USERCMD_PARA;
	pCmd->size				=	size;
	memcpy(pCmd + 1,pScreenID,sizeof(DWORD)*size);

	GetClient()->SendCommand(pCmd,len);

	RequestScreenPic(pScreenID,size);
	//*/

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pScreenID : 描述
 * \param size : 描述
 * \return 返回值的描述
 */
void CGameScene::RequestScreenPic(int * pScreenID,size_t size)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGameScene::Update9ScreenRect()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nMsg : 描述
 * \param wParam : 描述
 * \param lParam : 描述
 * \return 返回值的描述
 */
bool CGameScene::MsgProc(UINT nMsg,WPARAM wParam,LPARAM lParam)
{
	FUNCTION_BEGIN;

	if( nMsg == WM_LBUTTONDOWN || 
		nMsg == WM_RBUTTONDOWN || 
		nMsg == WM_MBUTTONDOWN ||
		nMsg == WM_LBUTTONUP ||
		nMsg == WM_RBUTTONUP ||
		nMsg == WM_MBUTTONUP 
		)
	{
		CGuiDialog* pMouseDlg = GetGuiManager()->GetMouseOverDlg();

		if( pMouseDlg && !pMouseDlg->IsSystemDialog()	) 
			return false;
	}

	return m_pMainCharacter->MsgProc(nMsg,wParam,lParam);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param objects : 描述
 * \return 返回值的描述
 */
bool CGameScene::UpdateMouseOverObject(CGameScene::tListDrawObjects & objects)
{

	FUNCTION_BEGIN;

	bool bAltDown = (GetKeyState(VK_MENU) < 0);
	bool bShiftDown = (GetKeyState(VK_SHIFT) < 0);
	stPointI ptMouse = Engine_GetCursor()->GetPosition();

	bool bJusetInRect = false;
	/*
	* 这里以前写得太垃圾了。 2006.3.10 3:18 重新编写 
	*/
_search:
	{
		int iii=0;
		SetFrameAllocator fa;
		std::vector<CSceneObject*,std::stack_allocator<CSceneObject*> > sels;
		std::vector<CSceneObject*,std::stack_allocator<CSceneObject*> > funcNpcSels;
		for(tListDrawObjects::reverse_iterator it = objects.rbegin(); it != objects.rend();++it)
		{
			if( (*it)->IsSceneObjectType(OBJECT_ITEM) )
			{
				DWORD dwId = ((CItem*)(*it))->GetBaseID();
				//对于移动卷轴的通道特殊处理
				if( (dwId == c_nHuoYunFu) ||
					(dwId == c_nXianTianFu) ||
					(dwId == c_nXuanYuanFu) )
					continue;
			}
			CSceneObject* pFuncNpc = NULL;
			if( (*it) != m_pMainCharacter && (*it)->IsVisible() && (*it)->IsEnabled() && (*it)->IsAtScreenPixelPos(ptMouse,bJusetInRect) )
			{
				if(bAltDown){
					// Alt 键按住时优先级：Item , FuncNpc, Other
					if((*it)->IsSceneObjectType(OBJECT_ITEM)){
						m_pMouseOverObject = (*it);
						if( (*it)->OnSetCursor() )
							return true;
					}else{
						if( (*it)->IsSceneObjectType(OBJECT_NPC) && ((CNpc*)(*it))->IsNpcType(NPC_TYPE_FUNCTION) )
							funcNpcSels.push_back(*it);
						else
							sels.push_back(*it);
					}
				}else{
					m_pMouseOverObject = (*it);
					if( (*it)->OnSetCursor() )
						return true;
				}
			}
		}

		for(size_t i=0; i<funcNpcSels.size();++i){
			m_pMouseOverObject = funcNpcSels[i];
			if( funcNpcSels[i]->OnSetCursor() )
				return true;
		}

		for(size_t i=0; i<sels.size();++i){
			m_pMouseOverObject = sels[i];
			if( sels[i]->OnSetCursor() )
				return true;
		}

	}

	// 从上层开始搜索,扩大选择灵敏度
	if(!bJusetInRect)
	{
		bJusetInRect = true;
		goto _search;
	}

	FUNCTION_END;

	return false;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGameScene::UpdateMouseOverObject()
{
	FUNCTION_BEGIN;

	m_pMouseOverObject = NULL;
	/*if(!GetGameState()->IsStateItem(eGameStateItem_Normal))
	return;*/
	if (!GetGameCursor()->IsNormal() 
		&& !(GetGameCursor()->GetCursorImageType() == eCursorShowUser) 
		&& !(GetGameCursor()->GetCursorImageType() == eCursorFollowUser) 
		&& !(GetGameCursor()->GetCursorImageType() == eCursorUseAccess) 
		&& !(GetGameCursor()->GetCursorImageType() == eCursorTrade)
		&& !(GetGameCursor()->GetCursorImageType() == eCursorFreeMove)
		&& !(GetGameCursor()->GetCursorImageType() == eCursorPetAttack)
		&& !(GetGameCursor()->GetCursorTypeEx() == eCursorGetMailItem))
		return;

	CGuiDialog* pMouseDlg = GetGuiManager()->GetMouseOverDlg();

	if( pMouseDlg && !pMouseDlg->IsSystemDialog()	) 
	{
		if ( (GetGameCursor()->GetCursorImageType() != eCursorTrade)
			&& (GetGameCursor()->GetCursorImageType() != eCursorShowUser) 
			&& (GetGameCursor()->GetCursorImageType() != eCursorFollowUser) 
			&& (GetGameCursor()->GetCursorImageType() != eCursorSetAccess) 
			&& (GetGameCursor()->GetCursorImageType() != eCursorUseAccess)
			&& (GetGameCursor()->GetCursorImageType() != eCursorPetAttack)
			&& (GetGameCursor()->GetCursorTypeEx() != eCursorGetMailItem))
		{
			GetGameCursor()->SetCursor(CCursor::CursorType_Normal,eCursorNormal);
			return;
		}		
	}

	bool bAltDown = (GetKeyState(VK_MENU) < 0);
	stPointI ptMouse = Engine_GetCursor()->GetPosition();

	/*if ( GetKeyState(VK_UP) < 0 )*/  //Just a test
	if(UpdateMouseOverObject(m_aDrawObjects)) return;
	if(UpdateMouseOverObject(m_aDrawLowest)) return;

	if(m_pMainCharacter && m_pMainCharacter->IsAtScreenPixelPos(ptMouse,true))
	{
		m_pMouseOverObject = m_pMainCharacter;
		if(m_pMainCharacter->OnSetCursor())
			return;
	}

	if ( (GetGameCursor()->GetCursorImageType() != eCursorTrade) 
		&& (GetGameCursor()->GetCursorImageType() != eCursorShowUser) 
		&& (GetGameCursor()->GetCursorImageType() != eCursorFollowUser) 
		&& (GetGameCursor()->GetCursorImageType() != eCursorSetAccess) 
		&& (GetGameCursor()->GetCursorImageType() != eCursorUseAccess)
		&& (GetGameCursor()->GetCursorImageType() != eCursorFreeMove)
		&& (GetGameCursor()->GetCursorImageType() != eCursorPetAttack)
		&& (GetGameCursor()->GetCursorTypeEx() != eCursorGetMailItem) )
		GetGameCursor()->SetCursor(CCursor::CursorType_Normal,eCursorNormal);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwID : 描述
 * \param bNpc : 描述
 * \return 返回值的描述
 */
CNpc* CGameScene::FindNpcAndCharacter(DWORD dwID,bool bNpc)
{
	FUNCTION_BEGIN;

	if(bNpc) return FindNpcByID(dwID);
	return static_cast<CNpc*>(FindCharacterByID(dwID));

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGameScene::OnDisplayModeChange()
{
	FUNCTION_BEGIN;

	CMapScene::OnDisplayModeChange();
	GetGuiManager()->OnScreenSize();
	/*if(GetGameGuiManager()->m_guiMain)
		GetGameGuiManager()->m_guiMain->ResetScreenWidth();*/
	if ( GetWeatherMgr() )
		GetWeatherMgr()->ResetLiveRange( stRectI( 0, 0, GetDevice()->GetWidth(), GetDevice()->GetHeight() ) );

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ptGrid : 描述
 * \return 返回值的描述
 */
iCharacter* CGameScene::GetICharacterByPos(POINT ptGrid)
{
	FUNCTION_BEGIN;

	CNpc* pNpc = GetCharacterByPos(ptGrid);
	if(pNpc && !pNpc->IsDie()) return pNpc->GetInterface();
	return NULL;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ptPos : 描述
 * \return 返回值的描述
 */
CNpc* CGameScene::GetCharacterByPos(POINT ptPos)
{
	FUNCTION_BEGIN;

	for(tListNpc::iterator it = m_listNpc.begin(); it != m_listNpc.end(); ++it)
	{
		stPointI pt = (*it)->GetPos();
		if(abs(ptPos.x - pt.x) < GRID_WIDTH && abs(ptPos.y - pt.y) < GRID_HEIGHT)
			return (*it);
	}
	for(tListCharacter::iterator it = m_listCharacter.begin(); it != m_listCharacter.end(); ++it)
	{
		stPointI pt = (*it)->GetPos();
		if(abs(ptPos.x - pt.x) < GRID_WIDTH && abs(ptPos.y - pt.y) < GRID_HEIGHT)
			return (*it);
	}
	return NULL;

	FUNCTION_END;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
bool CGameScene::RefreshLightInfo()
{
	FUNCTION_BEGIN;

	const D3DXCOLOR cr12(1.0f,1.0f,1.0f,1.0f);
	static D3DXCOLOR cr0;

	static bool s_bInitCr0 = false;

	int c_nLightStart = 8;
	int c_nLightEnd = 16;
	//6--18全部亮
	int c_nDayStart = 6;
	int c_nDayEnd = 18;
	//6--18是白天，其余是晚上
	int c_nMinRadius = 600;
	int c_nMaxRadius = 1200;

	QWORD t0;
	QWORD curTime = GetGameTime()->m_qwSecond;
	stGameDateTime stTime = GetGameTime()->m_time;

	if (stTime.tm_hour >= c_nLightStart && stTime.tm_hour < c_nLightEnd)
	{
		m_bLightMap = true;
		m_dwLightColor = -1;
		return true;
		//return false;
	}

	if(!s_bInitCr0)
	{
		const int maxC(0x60),minC(0x40);
		const float f = 1.0f/255.0f;
		float fc = f * ( (rand()%(maxC-minC)) + minC );
		cr0.r = fc;
		cr0.g = fc;
		cr0.b = fc * 1.5f;
		cr0.a = 1.0f;
		s_bInitCr0 = true;
	}
	if(stTime.tm_hour == 12)
	{
		s_bInitCr0 = false;
	}

	long dt = 12 * 60 * 60;
	if(stTime.tm_hour < 12)
	{
		stTime.tm_hour = 0;
		stTime.tm_min = 0;
		stTime.tm_sec = 0;
		t0 = timeGetTimeSecond(&stTime);
		//m_dwLightColor = ColorLerp( cr0,cr12,(long)(curTime-t0) * 255/(long)(t1-t0) );
		D3DXCOLOR color;
		D3DXColorLerp(&color,&(cr0),&(cr12),(float)(curTime-t0)/(float)(dt));
		m_dwLightColor = color;
		m_nLightRadius = (long)(curTime-t0) * (c_nMaxRadius-c_nMinRadius)/(long)(dt) + c_nMinRadius;	
	}
	else
	{

		stTime.tm_hour = 12;
		stTime.tm_min = 0;
		stTime.tm_sec = 0;
		t0 = timeGetTimeSecond(&stTime);
		//m_dwLightColor = ColorLerp( cr12,cr0,(long)(curTime-t0) * 255/(long)(t1-t0) );
		D3DXCOLOR color;
		D3DXColorLerp(&color,&(cr12),&(cr0),(float)(curTime-t0)/(float)(dt));
		m_dwLightColor = color;
		m_nLightRadius = (long)(curTime-t0) * (c_nMinRadius-c_nMaxRadius)/(long)(dt) + c_nMaxRadius;	
	}
	//TRACE("LightMap r:%d,c:%x ,dt:%d\n",m_nLightRadius,m_dwLightColor,(long)(curTime - t0));
	m_bLightMap = true;
	return true;

	FUNCTION_END;
}

bool CGameScene::IsDarkScene()
{
	return m_bDarkScene;
}
 
/**
 * \brief 判断地图名称是否变化
 * 
 * 判断prevGrid和ptCurGrid的区域名字是否相同
 *
 * \param prevGrid : 前一个格子
 * \param ptCurGrid : 现在的格子
 * \return 名称相同true else false
 */
bool CGameScene::IsAreaNameChange(POINT prevGrid,POINT ptCurGrid)
{
	FUNCTION_BEGIN;

	if ( (prevGrid.x == -1)
		|| (prevGrid.y == -1) )
	{
		return true;
	}
		

	int prevZoneID = GetZoneID(prevGrid);
	int curZoneID = GetZoneID(ptCurGrid);
	stZoneInfo* preZoneInfo = GetMapZoneInfo(prevZoneID);
	stZoneInfo* curZoneInfo = GetMapZoneInfo(curZoneID);

	return (preZoneInfo != curZoneInfo);

	FUNCTION_END;
}

/**
 * \brief 判断背景音乐是否发生变化
 * 
 * 判断prevGrid和ptCurGrid的区域背景音乐是否相同
 * 
 * \param prevGrid : 前一个格子
 * \param ptCurGrid : 现在的格子
 * \return 音乐相同true else false
 */
bool CGameScene::IsAreaBackMusicChange(POINT prevGrid,POINT ptCurGrid)
{
	FUNCTION_BEGIN;	

	int prevZoneID = GetZoneID(prevGrid);
	int curZoneID = GetZoneID(ptCurGrid);
	stZoneInfo* preZoneInfo = GetMapZoneInfo(prevZoneID);
	stZoneInfo* curZoneInfo = GetMapZoneInfo(curZoneID);

	const char* preName = preZoneInfo ? preZoneInfo->music.szBackMusic : "";
	const char* curName = curZoneInfo ? curZoneInfo->music.szBackMusic : "";
	return stricmp(preName,curName) != 0;

	FUNCTION_END;
}

/**
 * \brief 获得格子所在区域名
 * 
 * \param ptCurGrid : 格子
 * \return 区域名,如果格子不在地图里return NULL
 */
const char *CGameScene::GetAreaName(POINT ptCurGrid)
{
	FUNCTION_BEGIN;

	int curZoneID = GetZoneID(ptCurGrid);
	stZoneInfo* curZoneInfo = GetMapZoneInfo(curZoneID);
	if(curZoneInfo) return curZoneInfo->szName;
	else return NULL;

	FUNCTION_END;
}

/**
 * \brief 获得格子所在区域的背景音乐
 * 
 * \param ptCurGrid : 格子
 * \return 背景音乐文件名,如果格子不在地图里return NULL
 */
CGameScene::stBackMusic * CGameScene::GetAreaBackMusic(POINT ptCurGrid)
{
	FUNCTION_BEGIN;

	int curZoneID = GetZoneID(ptCurGrid);
	stZoneInfo* curZoneInfo = GetMapZoneInfo(curZoneID);
	if(curZoneInfo) return &curZoneInfo->music;
	else return NULL;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwSoundID : 描述
 * \return 返回值的描述
 */
void CGameScene::PlayUISound(DWORD dwSoundID)
{
	FUNCTION_BEGIN;
	::PlayUISound(dwSoundID);
	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
DWORD  CGameScene::GetGameHourTime()
{
	FUNCTION_BEGIN;
	return GetGameTime()->m_time.tm_hour;
	FUNCTION_END;
}


bool CGameScene::GetObjectImageLocation(int id,stResourceLocation * rl)
{
	ObjectBase_t* pObject = GetObjectBase(id);
	if(pObject)
	{
		*rl = ::GetObjectImageLocation(pObject);
		return true;
	}
	return  false;
}

void CGameScene::SetObjectTooltips(int id,CToolTips& toolTips)
{
	ObjectBase_t* pObject = GetObjectBase(id);
	if(pObject)
	{
		UpdateObjectToolTips(pObject,toolTips,1); //sky
	}
}

static enumNpcTypeItem scBugbearNpcs[] = {
		NpcTypeItem_Human,				//0
		NpcTypeItem_Normal,				//1
		NpcTypeItem_Boss,				//2
		NpcTypeItem_SmBoss,				//3
		NpcTypeItem_Extract,			//4
		NpcTypeItem_Gold,				//5
		NpcTypeItem_Pet,				//9
		NpcTypeItem_ExtractBugbear,		//10
		NpcTypeItem_Beckon,				//11
		NpcTypeItem_ExGold,				//31
		NpcTypeItem_RedBoss,            //33

		NpcTypeItem_None				//end
};

static enumNpcTypeItem scFunctionalNpcs[] = {
		NpcTypeItem_Trade,				//6
		NpcTypeItem_Task,				//7
		NpcTypeItem_Enlisted,			//8
		NpcTypeItem_SavingBox,			//15
		NpcTypeItem_MailMan,
		NpcTypeItem_Auction,
		NpcTypeItem_None				//end
};

bool bIsNpcTypeIn(enumNpcTypeItem type,enumNpcTypeItem *Npcs)
{
	while(*Npcs != NpcTypeItem_None)
	{
		if(type == *Npcs) return true;
		Npcs++;
	}
	return false;
}

bool CGameScene::IsBugbearNpcNear()
{
	if(m_pMainCharacter)
	{
		stPointI curMainCharPos = m_pMainCharacter->GetGridPos();
		
		for(tListNpc::iterator it = m_listNpc.begin(); it != m_listNpc.end(); ++it)
		{
			CNpc *pNpc = (*it);
			if(bIsNpcTypeIn(pNpc->GetNpcTypeItem(),scBugbearNpcs) )
			{
				stPointI pos = pNpc->GetGridPos();
				stPointI pt1 = pos - curMainCharPos;
				if( abs(pt1.x) <= 2 && abs(pt1.y) <= 2 )
				{		
					TRACE("%s\t",pNpc->GetName());
					TRACE("Npc:%d,%d\t,Mc:%d,%d\n",pos.x,pos.y,curMainCharPos.x,curMainCharPos.y);
					TRACE("pt1:%d,%d\t \n",pt1.x,pt1.y);
					return true;
				}
			}
		}
	}
	return false;
}

//CNpc* CGameScene::GetNearByNpc(enumNpcTypeItem* npcs,int xOffset,int yOffset)
//{
//	if(m_pMainCharacter)
//	{
//		stPointI curMainCharPos = m_pMainCharacter->GetGridPos();
//		for(tListNpc::iterator it = m_listNpc.begin(); it != m_listNpc.end(); ++it)
//		{
//			CNpc *pNpc = (*it);
//			if( bIsNpcTypeIn( pNpc->GetNpcTypeItem(),npcs ) )
//			{
//				stPointI pos = pNpc->GetGridPos();
//				stPointI pt1 = pos - curMainCharPos;
//
//				if( abs(pt1.x) <= xOffset  && abs(pt1.y) <= yOffset  )
//				{
//					return pNpc;
//				}
//			}
//		}
//	}
//	return NULL;
//}

bool CGameScene::IsFunctionalNpcNear()
{
	if(m_pMainCharacter)
	{
		stPointI curMainCharPos = m_pMainCharacter->GetGridPos();
		
		for(tListNpc::iterator it = m_listNpc.begin(); it != m_listNpc.end(); ++it)
		{
			CNpc *pNpc = (*it);
			if(bIsNpcTypeIn(pNpc->GetNpcTypeItem(),scFunctionalNpcs) )
			{
				stPointI pos = pNpc->GetGridPos();
				stPointI pt1 = pos - curMainCharPos;
				//TRACE("Npc:%d,%d\t,Mc:%d,%d\n",pos.x,pos.y,curMainCharPos.x,curMainCharPos.y);
				//TRACE("pt1:%d,%d\t \n",pt1.x,pt1.y);

				if( abs(pt1.x) <= 2  && abs(pt1.y) <= 2  )
				{
					//TRACE("%s\t",pNpc->GetName());
					//TRACE("Npc:%d,%d\t,Mc:%d,%d\n",pos.x,pos.y,curMainCharPos.x,curMainCharPos.y);
					//TRACE("pt1:%d,%d\t \n",pt1.x,pt1.y);

					return true;
				}
			}
		}
	}
	return false;
}

POINT CGameScene::GetCenterGridPos()
{
	return m_pMainCharacter->GetGridPos();
}

bool CGameScene::IsGridHasAnyObject( const stPointI& ptGrid, bool bAbsPos /*=false*/ )
{
	stPointI realPos = bAbsPos ? ptGrid : ptGrid+GetMainCharacter()->GetGridPos();
	/*CSceneObject* pObject = GetScene()->GetObjectAtGrid<CCharacter>(realPos);
	if ( NULL == pObject )
		pObject = GetScene()->GetObjectAtGrid<CNpc>(realPos);
	if ( pObject && !pObject->IsDie() )
		return true;*/
	CNpc* pNpc = GetCharacterByPos( GridCenterToPixelPos( realPos ) );
	if ( pNpc && !pNpc->IsDie() )
		return true;
	

	return false;
}

bool CGameScene::IsGridHasAnyEnemy( const stPointI& ptGrid, bool bAbsPos /* = false */ )
{
	stPointI realPos = bAbsPos ? ptGrid : ptGrid+GetMainCharacter()->GetGridPos();
	CSceneObject* pObject = GetScene()->GetObjectAtGrid<CCharacter>(realPos);
	if ( NULL == pObject )
		pObject = GetScene()->GetObjectAtGrid<CNpc>(realPos);

	if ( pObject )
	{
		CNpc* pNpc = static_cast<CNpc*>(pObject);

		if ( pNpc->IsMainRole() )
			return false;

		if ( pNpc->IsDie() )
			return false;

		if ( GetScene()->GetMainCharacter()->GetPKMode() == PKMODE_NORMAL )
			return false;

		if ( GetScene()->GetMainCharacter()->GetPKMode() == PKMODE_TEAM )
		{
			if ( GetGameGuiManager()->m_guiTeam )
			{
				if ( GetGameGuiManager()->m_guiTeam->FindMember( pNpc->GetID() ) )
					return false;
				else
					return true;
			}
			else
				return true;		
		}		

		if ( pNpc->IsPet() )
		{
			if ( !( (GetScene()->GetMainCharacter()->GetPKMode() == PKMODE_COUNTRY)
				|| (GetScene()->GetMainCharacter()->GetPKMode() == PKMODE_ENTIRE) ) )
			{
				return true;
			}
			else
				return false;
		}

		return true;
	}

	return false;
}

/**
 * \brief  获取一个任务变量的值
 *
 *	获取一个任务变量的值
 *
 * \param taskID : 任务ＩＤ
 * \param taskVarName : 任务变量名
 *
 * \return  任务变量值
 *
 */
std::string CGameScene::GetTaskVariable(DWORD taskID,const char* taskVarName)
{
	return m_pMainCharacter->GetTaskList()->GetTaskValue(taskID,taskVarName);
}

/**
 * \brief  返回主角的当前方向
 *
 *	返回主角的当前方向
 *
 *
 * \return  返回主角的当前方向
 *
 */
int CGameScene::GetMainCharacterDir()
{
	return m_pMainCharacter->GetInterface()->GetDir();
}


static void* __cdecl s_fn_malloc( size_t size )
{
	return FrameAllocator::alloc(size);
}

static void __cdecl s_fn_free( void *memblock )
{
}

static stRectI s_pathRect;

static int __stdcall  s_fn_FreeTile(int x,int y)
{
	if(s_pathRect.PtInRect(stPointI(x,y)))
		return !GetScene()->IsStaticObstacle(stPointI(x,y));
	return false;
}


static POINT GetNewFreePoint(POINT pt)
{
	int count = PathZoneSize/2;

	for(int i=0; i < count;++i)
	{
		int minX = getMax(pt.x - i,(long)0);
		int maxX = getMin(pt.x + i,(long)(GetScene()->GetWidth() - 1));
		int minY = getMax(pt.y - i,(long)0);
		int maxY = getMin(pt.y + i,(long)(GetScene()->GetHeight() - 1));

		int x,y;
		y = minY;
		for(x = minX; x <= maxX; ++x)
		{
			if(!GetScene()->IsStaticObstacle(stPointI(x,y)))
				return stPointI(x,y);
		}

		if(maxY != minY)
		{
			y = maxY;
			for(x = minX; x <= maxX; ++x)
			{
				if(!GetScene()->IsStaticObstacle(stPointI(x,y)))
					return stPointI(x,y);
			}
		}

		x = minX;
		for(y = minY + 1; y <= maxY - 1; ++y)
		{	
			if(!GetScene()->IsStaticObstacle(stPointI(x,y)))
				return stPointI(x,y);
		}

		if(maxX != minX)
		{
			x = maxX;
			for(y = minY + 1; y <= maxY - 1; ++y)
			{	
				if(!GetScene()->IsStaticObstacle(stPointI(x,y)))
					return stPointI(x,y);
			}
		}

	}
	return pt;
}

#define SETZONEBLOCKBYPREVZONE(offsetX,offsetY)\
{\
	stPointI prev = stPointI(j + (offsetX), i + (offsetY) );\
	if(rectTotalZone.PtInRect(prev))\
		zone.SetBlock(stPointI(offsetX,offsetY), GetPathZone(prev).IsBlock(stPointI(-(offsetX),-(offsetY))) );\
}

#define TESTZONEBLOCK(offsetX,offsetY)\
{\
	SetFrameAllocator fa;\
	stRectI rc1;\
	rc1.left = j * PathZoneSize;\
	rc1.top = i * PathZoneSize;\
	rc1.right = rc1.left + PathZoneSize;\
	rc1.bottom = rc1.top + PathZoneSize;\
\
	stRectI rc2;\
	rc2.left = (j + offsetX) * PathZoneSize;\
	rc2.top = (i + offsetY) * PathZoneSize;\
	rc2.right = rc2.left + PathZoneSize;\
	rc2.bottom = rc2.top + PathZoneSize;\
\
	s_pathRect.left = getMin(rc1.left,rc2.left);\
	s_pathRect.top  = getMin(rc1.top,rc2.top);\
	s_pathRect.right = s_pathRect.left + PathZoneSize * 2 + 1;\
	s_pathRect.bottom = s_pathRect.top + PathZoneSize * 2 + 1;\
\
	s_pathRect &= rectMap;\
\
	stPointI ptSrc( rc1.left + PathZoneSize/2 , rc1.top + PathZoneSize/2 );\
	stPointI ptDst( rc2.left + PathZoneSize/2 , rc2.top + PathZoneSize/2 );\
	if(ptSrc.x >= GetWidth()) ptSrc.x = GetWidth()-1;\
	if(ptSrc.y >= GetHeight()) ptSrc.y = GetHeight() -1;\
	if(ptDst.x >= GetWidth()) ptDst.x = GetWidth()-1;\
	if(ptDst.y >= GetHeight()) ptDst.y = GetHeight()-1;\
	ptSrc = GetNewFreePoint(ptSrc);\
	ptDst = GetNewFreePoint(ptDst);\
	if(!IsStaticObstacle(ptSrc) && !IsStaticObstacle(ptDst))\
	{\
\
		if(ptSrc.x < 0) ptSrc.x = 0;\
		else if(ptSrc.x >= GetWidth()) ptSrc.x = GetWidth()-1;\
		if(ptSrc.y < 0) ptSrc.y = 0;\
		else if(ptSrc.y >= GetHeight()) ptSrc.y = GetHeight()-1;\
\
		if(ptDst.x < 0) ptDst.x = 0;\
		else if(ptDst.x >= GetWidth()) ptDst.x = GetWidth()-1;\
		if(ptDst.y < 0) ptDst.y = 0;\
		else if(ptDst.y >= GetHeight()) ptDst.y = GetHeight()-1;\
\
		astar.FindPath(ptSrc.x, ptSrc.y, ptDst.x, ptDst.y);\
		if(astar.IsHavePath())\
		{\
			stPointI lastPos(astar.LastNodeGetX(),astar.LastNodeGetY());\
\
			int dis = Scene_GetDis(ptDst,lastPos);\
			zone.SetBlock(stPointI(offsetX,offsetY),dis > PathZoneSize/2);\
		}\
	}\
}


void CGameScene::InitPathZone()
{
	m_nPathZoneWidth = (GetWidth() + PathZoneSize - 1)/PathZoneSize;
	m_nPathZoneHeight = (GetHeight() + PathZoneSize - 1)/PathZoneSize;

	stRectI rectMap(0,0,GetWidth(),GetHeight());
	stRectI rectTotalZone(0,0,m_nPathZoneWidth,m_nPathZoneHeight);

	extern bool gbGetNewPoint;
	CAStar astar;
	gbGetNewPoint = false;
	astar.SetMallocFn(s_fn_malloc,s_fn_free);
	astar.SetFreeTile(s_fn_FreeTile);
	astar.SetStep(1);
	m_aPathZone.resize(m_nPathZoneWidth * m_nPathZoneHeight);
	for( int i = 0; i < m_nPathZoneHeight;++i )
	{
		for( int j=0; j < m_nPathZoneWidth; ++j )
		{
			stPathZone & zone = m_aPathZone[m_nPathZoneWidth * i + j];
			stPointI prev;

			zone.flags = -1;

			/*SETZONEBLOCKBYPREVZONE(-1,-1);
			SETZONEBLOCKBYPREVZONE(0,-1);
			SETZONEBLOCKBYPREVZONE(1,-1);
			SETZONEBLOCKBYPREVZONE(-1,0);*/


			TESTZONEBLOCK(-1,-1);
			TESTZONEBLOCK(0,-1);
			TESTZONEBLOCK(1,-1);
			TESTZONEBLOCK(-1,0);

			TESTZONEBLOCK(1,0);
			TESTZONEBLOCK(-1,1);
			TESTZONEBLOCK(0,1);
			TESTZONEBLOCK(1,1);

			//stRectI rc;
			//rc.left = j * PathZoneSize;
			//rc.top = i * PathZoneSize;
			//rc.right = rc.left  + PathZoneSize;
			//rc.bottom = rc.top + PathZoneSize;

			//rc &= rectMap;

			//int blockNum = 0;

			//for(int y = rc.top; y < rc.bottom;++y)
			//{
			//	for(int x = rc.left; x < rc.right; ++x)
			//	{
			//		if(IsStaticObstacle(stPointI(x,y)))
			//			blockNum++;
			//	}
			//}

			//int totalGridNum = rc.Width() * rc.Height();
			//if( blockNum > 0)//totalGridNum * 50/100 )
			//{
			//	zone.flags = 1;
			//}
			//else
			//{
			//	zone.flags = 0;
			//}
			
		}
	}
	gbGetNewPoint = true;
}

bool CGameScene::UpdateMouseOverTextNpc(tListCharacter& characters)
{
	FUNCTION_BEGIN;

	m_pMouseOverTextObject = NULL;
	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	for(tListCharacter::iterator it = characters.begin(); it != characters.end(); ++it)
	{
		stRectI rcText = (*it)->GetShopTextRect();
		if((*it)->bIsShopStart() && rcText.PtInRect(ptCursor))
		{
			//TRACE("GetShopTextRect:%d,%d,%d,%d\n",rcText.left,rcText.top,rcText.right,rcText.bottom);
			m_pMouseOverTextObject = (*it);
			return true;
		}
	}
	return false;

	FUNCTION_END;
}

CNpc* CGameScene::GetNearByNpc(enumNpcTypeItem* npcs,int xOffset,int yOffset)
{
	FUNCTION_BEGIN;
	if(m_pMainCharacter)
	{
		stPointI curMainCharPos = m_pMainCharacter->GetGridPos();
		for(tListNpc::iterator it = m_listNpc.begin(); it != m_listNpc.end(); ++it)
		{
			CNpc *pNpc = (*it);
			if( bIsNpcTypeIn( pNpc->GetNpcTypeItem(),npcs ) )
			{
				stPointI pos = pNpc->GetGridPos();
				stPointI pt1 = pos - curMainCharPos;
				if( abs(pt1.x) <= xOffset  && abs(pt1.y) <= yOffset  )
				{
					return pNpc;
				}
			}
		}
	}
	return NULL;
	FUNCTION_END;
}

void CGameScene::UpdateCountryStar(DWORD dwCountry,DWORD dwStar)
{
	FUNCTION_BEGIN;

	tListCharacter::iterator iter;
	for( iter = m_listCharacter.begin(); iter != m_listCharacter.end(); ++iter)
	{
		CCharacter * pTemp = *iter;
		if(pTemp->GetCountry() == dwCountry)
		{
			pTemp->SetCountryStar(dwStar);
		}
	}
	

	FUNCTION_END;
}


void CGameScene::UpdateCountryWarFlag()
{ 
	FUNCTION_BEGIN;

	CCountry::stDareCountryInfo & DareInfo = CCountry::Instance()->GetCountryWarInfo();

	tListCharacter::iterator iter;
	for( iter = m_listCharacter.begin(); iter != m_listCharacter.end(); ++iter)
	{
		CCharacter * pTemp = *iter;
		DWORD __dwCountry = pTemp->GetCountry();
		
		CCountry::stCountryWarFlag warflag;
                if(DareInfo.GetDareCountryInfo(__dwCountry,warflag))
		{// 敌对国
			pTemp->SetShowWarFlag( (warflag.byStatus == (BYTE)1) );
		}
		else if(__dwCountry == GetMainCharacter()->GetCountry() )
		{//　与主角同国
			bool bAnyDare = DareInfo.IsAnyCountryInWar();
			if(bAnyDare)
			    pTemp->SetShowWarFlag(bAnyDare);
		}
	}


	FUNCTION_END;
}

void CGameScene::RunQueueUnionSept()
{
	int step = 250;
	static DWORD lastTimeSendCmd = timeGetTime() - step;
	if ( m_queueRequestUnionSeptNameCmd.empty() )
		return;

	if ( (timeGetTime()-lastTimeSendCmd) >= step )		//It's time to send one cmd
	{
		tRequestUnionSeptName tCmd = m_queueRequestUnionSeptNameCmd.front();
		switch( tCmd.type )
		{
		case RequestCmd_UnionName:
			{
				stReqUnionNameCmd cmd;
				cmd.dwUnionID = tCmd.dwID;
				SEND_USER_CMD( cmd );

				//Pop this cmd
				m_queueRequestUnionSeptNameCmd.pop_front();
			}
			break;
		case RequestCmd_SeptName:
			{
				stReqSeptNameCmd cmd;
				cmd.dwSeptID = tCmd.dwID;
				SEND_USER_CMD( cmd );

				m_queueRequestUnionSeptNameCmd.pop_front();
			}
			break;
		}

		lastTimeSendCmd = timeGetTime();
	}
}

DWORD  CGameScene::GetMapCountryID()
{
	return CCountry::Instance()->GetCountryID(m_szCountryName);
}

#ifdef _WYC_OPT_SER_
//add by wyc
void CGameScene::SetCharacterUnionNameByID(DWORD dwUnionID,const char* UnionName)
{
	FUNCTION_BEGIN;
	/*tListCharacter::iterator iter;
	for( iter = m_listCharacter.begin(); iter != m_listCharacter.end(); ++iter)
	{
		CCharacter * pTemp = *iter;
		if (pTemp->GetProData()->dwUnionID == dwUnionID) {
			strcpy(pTemp->GetProData()->unionName,UnionName);
		}
	}*/
	g_UserLocalInfo.m_pRequestIter = g_UserLocalInfo.curRequesetUnionId.find( dwUnionID );
	if ( g_UserLocalInfo.m_pRequestIter != g_UserLocalInfo.curRequesetUnionId.end() )
	{
		vRequestCharacter& vCharacter = (g_UserLocalInfo.m_pRequestIter)->second;
		vRequestCharacter::iterator iter;
		for( iter=vCharacter.begin(); iter!=vCharacter.end(); ++iter )
		{
			CCharacter* pChar = GetScene()->FindCharacterByID( (*iter) );
			if ( pChar && (pChar->GetProData()->dwUnionID == dwUnionID) )
				strcpy( pChar->GetProData()->unionName, UnionName );
		}

		//Delete from map
		g_UserLocalInfo.curRequesetUnionId.erase( dwUnionID );
	}

	FUNCTION_END;
}

void CGameScene::SetCharacterSeptNameByID(DWORD dwSeptID,const char* SeptName)
{
	FUNCTION_BEGIN;
	/*tListCharacter::iterator iter;
	for( iter = m_listCharacter.begin(); iter != m_listCharacter.end(); ++iter)
	{
		CCharacter * pTemp = *iter;
		if (pTemp->GetProData()->dwSeptID == dwSeptID) {
			strcpy(pTemp->GetProData()->septName,SeptName);
		}
	}*/
	g_UserLocalInfo.m_pRequestIter = g_UserLocalInfo.curRequesetSpetId.find( dwSeptID );
	if ( g_UserLocalInfo.m_pRequestIter != g_UserLocalInfo.curRequesetSpetId.end() )
	{
		vRequestCharacter& vCharacter = (g_UserLocalInfo.m_pRequestIter)->second;
		vRequestCharacter::iterator iter;
		for( iter=vCharacter.begin(); iter!=vCharacter.end(); ++iter )
		{
			CCharacter* pChar = GetScene()->FindCharacterByID( (*iter) );
			if ( pChar && (pChar->GetProData()->dwSeptID == dwSeptID) )
				strcpy( pChar->GetProData()->septName, SeptName );
		}

		//Delete from map
		g_UserLocalInfo.curRequesetSpetId.erase( dwSeptID );
	}
	FUNCTION_END;
}
#endif









