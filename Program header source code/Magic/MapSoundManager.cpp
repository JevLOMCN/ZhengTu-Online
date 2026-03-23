#include "../engine/include/engine.h"
#include ".\mapsoundmanager.h"
#include "./Map.h"
#include "./MapScene.h"


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
/*stMapSceneSound::~stMapSceneSound()
{
	if(sound)
	{
		GetSoundManager()->DestroySound(sound);
		sound = NULL;
	}
}*/

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CMapSoundManager* GetMapSoundManager()
{
	static CMapSoundManager sm;
	return &sm;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param time_elapsed : 描述
 * \return 返回值的描述
 */
void stMapSceneSound::Run( float time_elapsed )
{
	const int maxDis = 20;
	if(!GetSoundManager()->IsEnableOfType(SoundType_Circumstance))
		return;

	if( Scene_GetDis(GetMapScene()->GetCenterGridPos(),pt) > maxDis )
	{
		sound.Destroy(true);
		nextPlayTime = 0 ;	//下一次再重新来
	}
	else if( xtimeGetTime() > nextPlayTime )
	{
		///////////////////////////////////////////////////////////////////////
		DWORD dwGameHour = GetMapScene()->GetGameHourTime();
		if( (lastCheckTime == -1) || (dwGameHour - lastCheckTime) >= 1 )  // 由于单位是小时，所以每个1小时才检测一次时间范围,这样不浪费时间
		{
			// 先判断时间范围时候好
			BOOL bOkToPlay = TRUE;
			if( pMusic && pMusic->vecSegments.size() != 0)   // 0 表示没有时段限制
			{
				int i;for(i  = 0 ; i < pMusic->vecSegments.size(); i ++)
				{
					if( pMusic->vecSegments[i].dwStart <= dwGameHour && dwGameHour <= pMusic->vecSegments[i].dwEnd)
					{
						bOkToPlay = TRUE;
						break;
					}
				}
				if( i == pMusic->vecSegments.size()) {
					bOkToPlay = FALSE;
				}
			}

			if( !bOkToPlay )  return;

			lastCheckTime = dwGameHour;
		}
		
        // 仍然在这个时间段，OK,继续

		//if( !sound.IsPlaying())
		{
			if(pMusic->file == 10006)
				int iii = 0;
			char szSound[MAX_PATH];
			sprintf(szSound,"%d",pMusic->file);
			//sprintf(szSound,"sound\\scene\\%d.wav",pMusic->file);

			// 注:由于PlaySceneSound()里面还有一个距离判断，所以这个调用不一定成功，所以后面设置nextPlayTime时，必须判断是否播放成功
			PlaySceneSound(&sound,stPointI(pt.x * GRID_WIDTH + GRID_WIDTH/2,pt.y * GRID_HEIGHT + GRID_HEIGHT/2),stPointI(0,0),szSound,pMusic->maxInterval == 0,GRID_WIDTH,GRID_WIDTH * 1000);
		}

		if(pMusic->maxInterval > 0)
		{
			// 如果没有播放成功，就设置nextPlayingTime,就会断声很严重
			//if( sound.IsPlaying())
			{
				nextPlayTime = xtimeGetTime() + rand() % (pMusic->maxInterval-pMusic->minInterval) + pMusic->minInterval;	
			}
		}
		else
		{
			// 循环播放
			//if( sound.IsPlaying())
			{
				nextPlayTime = 0xffffffff;
			}
			
		}
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CMapSoundManager::CMapSoundManager( void )
{
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CMapSoundManager::~CMapSoundManager( void )
{
	Release();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMapSoundManager::Init()
{
	TiXmlDocument xml;
	if(!xml.LoadFile("data\\MapAddinInfo.xml"))
	{
		Engine_WriteLogF("解析 MapAddinInfo.xml 失败！\n");
		return;
	}
	TiXmlElement* map = xml.FirstChildElement("map");
	if(!map) return ;
	TiXmlElement* scenemusic = map->FirstChildElement("scenemusic");
	if(!scenemusic) return;
	TiXmlElement* item = scenemusic->FirstChildElement("item");
	while(item)
	{
		int MinInterval;
		int MaxInterval;
		int id;
		int file;
		if(item->QueryIntAttribute("id",&id) == TIXML_SUCCESS)
		{
			if(item->QueryIntAttribute("MinInterval",&MinInterval) != TIXML_SUCCESS)
				MinInterval = 0;
			if(item->QueryIntAttribute("MaxInterval",&MaxInterval) != TIXML_SUCCESS)
				MaxInterval = 0;
			if(item->QueryIntAttribute("file",&file) != TIXML_SUCCESS)
				file = 0;
			stMapSceneSoundInfo music;
			music.minInterval = MinInterval;
			music.maxInterval = MaxInterval;
			music.file = file;

                        //////////////////////////////////////////////////////////

			TiXmlElement * hour = item->FirstChildElement("hour");
			while( hour)
			{
				int start,end;
				if( hour->QueryIntAttribute("start",&start) != TIXML_SUCCESS)
					start = 0;
				if( hour->QueryIntAttribute("end",&end) != TIXML_SUCCESS)
					end = 0;
				if( start != 0 || end != 0)
				{
					stMapSceneSoundTimeSegment  segment;
					segment.dwStart = start ;
					segment.dwEnd   = end ;
					music.vecSegments.push_back(segment);
				}
				hour = hour->NextSiblingElement("hour");
			}
			//////////////////////////////////////////////////////////

			m_mapMusic[id] = music;
		}
		item = item->NextSiblingElement("item");
	}
}
