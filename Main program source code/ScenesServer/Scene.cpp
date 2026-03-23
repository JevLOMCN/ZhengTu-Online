/**
 * \file
 * \version  $Id: Scene.cpp  $
 * \author  
 * \date 
 * \brief 场景类的实现
 *
 * 
 */

#include <string>
#include <vector>
#include <set>

#include "Scene.h"
#include "LoadMap.h"
#include "Zebra.h"
#include "zRegex.h"
#include "zMisc.h"
#include "zDatabaseManager.h"
#include "SceneNpc.h"
#include "SceneNpcManager.h"
#include "TimeTick.h"
#include "Command.h"
#include "zString.h"
#include "zObject.h"
#include "ScenePk.h"
#include "SessionClient.h"
#include "SkillBase.h"
#include "Zebra.h"
#include "SceneManager.h"
#include "SceneTaskManager.h"
#include "RebuildObject.h"
#include "FuBenMgr.h" //副本

/**
 * \brief 构造函数
 *
 */
Scene::Scene(): _one_sec(1)
{
	userCount=0;
	countryID=0;
	function=0;
	backtoMapID=0;
	backtoCityMapID=0;
	foreignerBacktoMapID=0; 
	countryDareBackToMapID=0;
	commonCountryBacktoMapID=0; 
	commonUserBacktoMapID=0;
	inited=false;
	execGroup = 0;
	isUnionDare = false;
	isCountryFormalDare = false;
	//dwAttCountryID = 0;
	level =0;
	exprate=0.0f;
	winner_exp=false;
	pklevel=0;
	isEmperorDare = false;
	dwEmperorDareDef = 0;
    /////////////////////////////
	loongAltarNpcTmpID = 0;
	/////////////////////////////
    type = SCENE_TYPE_NONE;	
	//副本
    fbStartTime = 0;
    closeCountDownTime = 5;
    fbType = 0;
    fbMapID = 0;	
}

/**
 * \brief 析构函数
 *
 */
Scene::~Scene()
{
	final();
}

void Scene::freshGateScreenIndex(SceneUser *pUser, const DWORD screen)
{
	Cmd::Scene::t_fresh_ScreenIndex index;
	index.dwMapTempID = tempid;
	index.dwScreen = screen;
	index.dwUserTempID = pUser->tempid;
	pUser->gatetask->sendCmd(&index , sizeof(index));
}
void Scene::freshGateMapIndex() //副本新
{
    char Buf[zSocket::MAX_DATASIZE];
    bzero(Buf ,sizeof(Buf));
    Cmd::Scene::t_fresh_MapIndex *map = (Cmd::Scene::t_fresh_MapIndex *)Buf;
    constructInPlace(map);
    struct GateMapAddExec : public SceneCallBack
    {
        Cmd::Scene::t_fresh_MapIndex *_data;
        GateMapAddExec(Cmd::Scene::t_fresh_MapIndex *data):_data(data){_data->dwSize=0;} 
        bool exec(Scene *scene)
        {
            if(scene)
            {
                _data->mps[_data->dwSize].maptempid=scene->tempid;
                _data->mps[_data->dwSize].mapx=scene->getScreenX();
                _data->mps[_data->dwSize].mapy=scene->getScreenY();
                _data->dwSize++; 
            }
            return true; 
        }
    };
    GateMapAddExec getMapInfo(map);
    getMapInfo.exec(this);
    // SceneManager::getInstance().execEveryScene(exec);
    //pUser->gatetask->sendCmd(map , sizeof(Cmd::Scene::t_fresh_MapIndex) + map->dwSize * sizeof(Cmd::Scene::MapIndexXY));
    SceneTaskManager::getInstance().broadcastCmd(map , sizeof(Cmd::Scene::t_fresh_MapIndex) + map->dwSize * sizeof(Cmd::Scene::MapIndexXY) );
}
/**
 * \brief 获取地图所属国家名字
 * \return 地图所属国家名字
 */
const char * Scene::getCountryName() const
{
	return SceneManager::getInstance().getCountryNameByCountryID(countryID);
}

/**
 * \brief 为丢物品查找一个空余点
 * \param pos 中心点
 * \param findedPos 查找到的点
 * \return 查找是否成功
 */
bool Scene::findPosForObject(const zPos &pos, zPos &findedPos)
{
	int side = 0;
	int direct = zMisc::randBetween(0, 7);
	int clockwise = zMisc::selectByPercent(50) ? -1 : 1;
	int count = 0;
	findedPos = pos;
	while (checkObjectBlock(findedPos))
	{
		if (side > 14)
		{
			return false;
		}
		getNextPos(side, direct, pos, clockwise, findedPos);
		if (++count>=28*28) break;
	}
	setObjectBlock(findedPos);
	return true;
}

/**
 * \brief 为人物查找一个空余点
 * \param pos 中心点
 * \param findedPos 查找到的点
 * \return 查找是否成功
 */
bool Scene::findPosForUser(const zPos &pos, zPos &findedPos)
{
	int side = 0;
	int direct = zMisc::randBetween(0, 7);
	int clockwise = zMisc::selectByPercent(50) ? -1 : 1;
	int count = 0;
	findedPos = pos;
	while (checkBlock(findedPos))
	{
		if (side > 14)
		{
			return false;
		}
		getNextPos(side, direct, pos, clockwise, findedPos);
		if (++count>=28*28) return false;
	}
	setBlock(findedPos);
	//Zebra::logger->debug("%s, count = %u", __PRETTY_FUNCTION__, count);
	return true;
}

/**
 * \brief 根据坐标，获取站在这个坐标点上面的人物
 * \param pos 坐标点
 * \param bState 是否限定找到的玩家的状态
 * \param byState 玩家的状态
 * \return 玩家
 */
SceneUser *Scene::getSceneUserByPos(const zPos &pos, const bool bState, const zSceneEntry::SceneEntryState byState)
{
	return (SceneUser *)getSceneEntryByPos(zSceneEntry::SceneEntry_Player, pos, bState, byState);
}

/**
  * \brief 通过玩家ID获取场景对象
  *
  * \param userid 玩家ID
  *
  * \return 玩家实例
  *
  */
SceneUser *Scene::getUserByID(DWORD userid)
{
	SceneUser *ret=SceneUserManager::getMe().getUserByID(userid);
	if(ret)
	{
		if(ret->scene!=this) ret=NULL;
	}
	return ret;
}

/**
 * \brief 根据临时id得到玩家指针
 *
 *
 * \param usertempid 玩家临时id
 * \return 玩家指针，失败返回0
 */
SceneUser *Scene::getUserByTempID(DWORD usertempid)
{
	SceneUser *ret=SceneUserManager::getMe().getUserByTempID(usertempid);
	if(ret)
	{
		if(ret->scene!=this) ret=NULL;
	}
	return ret;
}

/**
 * \brief 根据名字得到玩家指针
 *
 *
 * \param username 玩家名字
 * \return 玩家指针，失败返回0
 */
SceneUser *Scene::getUserByName(const char *username)
{
	SceneUser *ret=SceneUserManager::getMe().getUserByName(username);
	if(ret)
	{
		if(ret->scene!=this) ret=NULL;
	}
	return ret;
}

/**
 * \brief 根据坐标，获取站在这个坐标点上面的Npc
 * \param pos 坐标点
 * \param bState 是否限定npc所处的状态
 * \param byState npc的所处的状态
 * \return Npc
 */
SceneNpc *Scene::getSceneNpcByPos(const zPos &pos, const bool bState, const zSceneEntry::SceneEntryState byState)
{
	return (SceneNpc *)getSceneEntryByPos(zSceneEntry::SceneEntry_NPC, pos, bState, byState);
}

/**
 * \brief 根据临时id得到NPC指针
 *
 *
 * \param npctempid 玩家临时id
 * \return NPC指针，失败返回0
 */
SceneNpc *Scene::getNpcByTempID(DWORD npctempid)
{
	SceneNpc *ret=SceneNpcManager::getMe().getNpcByTempID(npctempid);
	if(ret)
	{
		if(ret->scene!=this) ret=NULL;
	}
	return ret;
}

struct AICallback : public zSceneEntryCallBack
{
	const zRTime &ctv;
	MonkeyNpcs delNpc;
	AICallback(const zRTime &ctv) : ctv(ctv) {}
	/**
	 * \brief 回调函数
	 * \param entry 地图物件，这里全部是npc
	 * \return 回调是否成功
	 */
	bool exec(zSceneEntry *entry)
	{
		My_FunctionTime_wrapper(__PRETTY_FUNCTION__);
		SceneNpc *npc = (SceneNpc *)entry;
		if (npc->needClear())
			delNpc.insert(npc);
		else
			npc->action(ctv);
		return true;
	}
};
struct GetAffectScreen : public zSceneEntryCallBack
{
	DWORD group;
	/**
	 * \brief 构造函数
	 * \param scene 场景
	 */
	GetAffectScreen(DWORD g) :group(g) {};
	/**
	 * \brief 回调函数
	 * \return 回调是否成功
	 */
	bool exec(zSceneEntry *entry)
	{

		My_FunctionTime_wrapper(__PRETTY_FUNCTION__);
		SceneUser *pUser = (SceneUser *)entry;
		if(group == pUser->tempid%MAX_NPC_GROUP && pUser->scene)
			pUser->refreshMe();

		return true;
	}
};

bool Scene::SceneEntryAction(const zRTime& ctv, const DWORD group)
{
	My_FunctionTime_wrapper(__PRETTY_FUNCTION__);
	////FunctionTimes(0,__FUNCTION__);
	if(_one_sec(ctv))
	{
		//攻城
		processRush();
		//更新地表物品
		updateSceneObject();
		if(this->getRealMapID() == 190 && userCount)
		{
			struct tm tm_1;
			time_t timValue = time(NULL);
			tm_1=*localtime(&timValue);
			/*
			if((tm_1.tm_hour%2) && (tm_1.tm_min >= 55))
			{
			}
			else if(((tm_1.tm_hour%2) == 0) && tm_1.tm_min == 30)
			// */
			if(((tm_1.tm_hour%2) == 0) && tm_1.tm_min == 30 && tm_1.tm_sec < 3)
			{
				//SceneUserManager::getMe().removeUserToHuangcheng(this);
			}
		}
	}

	//首先收集受影响的屏编号，对地图上所有的角色恢复
	GetAffectScreen affectScreen(group);
	execAllOfScene(zSceneEntry::SceneEntry_Player, affectScreen);
	if(userCount>400)
	{
		SceneTaskManager::getInstance().execEvery();
	}
	AICallback callback(ctv);
	execAllOfEffectNpcScreen(group,callback);
	//回收待删除的npc
	if (!callback.delNpc.empty())
	{
		//FunctionTime func_alltime(0,__PRETTY_FUNCTION__,"回收动态npc需要的时间" , 32);
		for(MonkeyNpcs::iterator it=callback.delNpc.begin();it!=callback.delNpc.end();++it)
		{
			SceneNpc *npc = *it;

			//通知客户端删除NPC
			Cmd::stRemoveMapNpcMapScreenUserCmd removeMapNpc;
			removeMapNpc.dwMapNpcDataPosition = npc->tempid;
			sendCmdToNine(npc->getPosI(), &removeMapNpc, sizeof(removeMapNpc));
			//从地图和管理器中删除这个NPC
			removeNpc(npc);
			SceneNpcManager::getMe().removeSceneNpc(npc);
			SceneNpcManager::getMe().removeSpecialNpc(npc);
			//Zebra::logger->trace("%s, 删除npc %s(%u)", __FUNCTION__, npc->name, npc->tempid);
			SAFE_DELETE(npc);
		}
	}
	static int allcount=0;
	allcount += userCount;
	if(allcount >= 100)
	{
		allcount=0;
		return true;
	}
	return false;
}

/**
 * \brief 根据坐标，获取站在这个坐标点上面的物品
 * \param pos 坐标点
 * \return 物品
 */
zSceneObject *Scene::getSceneObjectByPos(const zPos &pos)
{
	return (zSceneObject *)getSceneEntryByPos(zSceneEntry::SceneEntry_Object, pos);
}

/**
 * \brief 在地图上面移除某一个物品
 * \param so 地图物品
 */
void Scene::removeObject(zSceneObject *so)
{
	if (removeSceneEntry(so))
	{
		clearObjectBlock(so->getPos());
	}
}

/**
 * \brief 在地图上面移除某一个Npc
 * \param sn 地图Npc
 */
void Scene::removeNpc(SceneNpc *sn)
{
	if (removeSceneEntry(sn)
			&& sn->getState() == zSceneEntry::SceneEntry_Normal)
	{
		clearBlock(sn->getPos());
	}
}

/**
 * \brief 检查坐标所在区域的类型
 * \param pos 待检查的坐标
 * \param type 需要检查的区域类型
 * \return 是否成功
 */
bool Scene::checkZoneType(const zPos &pos, const int type) const
{
	int ret = ZoneTypeDef::ZONE_NONE;
	for(ZoneTypeDefVector::const_iterator it = zoneTypeDef.begin(); it != zoneTypeDef.end(); it++)
	{
		if ((*it).region.isIn(pos))
		{
			ret |= (*it).type;
		}
	}
	return ZoneTypeDef::ZONE_NONE != (ret & type);
}

/**
 * \brief 获得坐标所在区域的类型
 * \param pos 待检查的坐标
 * \return 区域类型
 */
int Scene::getZoneType(const zPos &pos) const
{
	int ret = ZoneTypeDef::ZONE_NONE;
	for(ZoneTypeDefVector::const_iterator it = zoneTypeDef.begin(); it != zoneTypeDef.end(); it++)
	{
		if ((*it).region.isIn(pos))
		{
			ret |= (*it).type;
		}
	}
	return ret;
}

/**
 * \brief 在地图指定类型的区域里面随机产生坐标
 * \param type 区域类型
 * \param pos 产生的坐标
 * \return 是否成功
 */
bool Scene::randzPosByZoneType(const int type, zPos &pos) const
{
	if (ZoneTypeDef::ZONE_NONE == type) {
#if 0
		int i=0;
		do {
			pos.x = zMisc::randBetween(1, width() -1);
			pos.y = zMisc::randBetween(1, height() -1);
		} while (checkBlock(pos, TILE_BLOCK) && ++i<10000);

		Zebra::logger->debug("%s, i = %u", __PRETTY_FUNCTION__, i);
		return true;
#else
		return randPosByRegion(_index, pos);
#endif
	}

	for(ZoneTypeDefVector::const_iterator it = zoneTypeDef.begin(); it != zoneTypeDef.end(); it++)
	{
		if (ZoneTypeDef::ZONE_NONE != ((*it).type & type))
		{
			return randPosByRegion((*it).region.index, pos);
		}
	}
	return false;
}



/**
 * \brief 玩家切换地图
 *
 *
 * \param pUser 玩家指针
 * \param deathBackto 是否是死亡回城
 * \param ignoreWar 忽略跳转中的国战处理部分
 * \return 0 跳场景失败，1 在本服跳场景， 2跨服跳场景
 */
int Scene::changeMap(SceneUser *pUser , bool deathBackto, bool ignoreWar)
{
	if(!pUser)
	{
		return 0;
	}
	//TODO
	//在别的地图寻找重生地
	//Scene *toscene=SceneManager::getInstance().getSceneByName(backtoMapName);
	Scene *toscene=NULL;

	if(deathBackto)
	{
		if(pUser->charbase.country == this->countryID)
		{
			toscene=SceneManager::getInstance().getSceneByID(backtoMapID);
		}
		else if(pUser->deathBackToMapID)
		{
			toscene=SceneManager::getInstance().getSceneByID(pUser->deathBackToMapID);
		}
		else
		{
			if(this->getCountryID() == 6)
			{
				if (commonCountryBacktoMapID)
				{
					toscene=SceneManager::getInstance().getSceneByID((this->countryID<<16) + commonCountryBacktoMapID);
				}
				else
				{
					toscene=SceneManager::getInstance().getSceneByID((pUser->charbase.country<<16) + 
							foreignerBacktoMapID);
				}
			}
			else
			{
				if(pUser->charbase.country == 6)
				{
					toscene=SceneManager::getInstance().getSceneByID((pUser->charbase.country << 16 ) + commonUserBacktoMapID);
				}
				else
				{
					toscene=SceneManager::getInstance().getSceneByID((pUser->charbase.country << 16 ) + foreignerBacktoMapID);
				}
			}
		}
	}
	else
	{
		toscene=SceneManager::getInstance().getSceneByID(backtoCityMapID);
	}

	if(toscene)
	{
		if ((!toscene->checkUserLevel(pUser))&&(pUser->scene!=toscene)) return true;

		zPos newpos;
		newpos.x=0,newpos.y=0;
		if(pUser->charbase.country == this->countryID)
		{
			if(toscene->randzPosByZoneType(ZoneTypeDef::ZONE_RELIVE , newpos))
			{
				pUser->changeMap(toscene,newpos);
				return 1;
			}
			else
			{
				return 1;
			}
		}
		else
		{
			if (pUser->isSpecWar(Cmd::COUNTRY_FORMAL_DARE) && pUser->scene->getCountryDare() && !ignoreWar)
			{
				if (toscene->randzPosByZoneType(ZoneTypeDef::ZONE_PRIVATE_RELIVE , newpos))
				{
					pUser->changeMap(toscene, newpos);
					return 1;
				}
				else
				{
					return 1;
				}
			}
			else
			{
				if(toscene->randzPosByZoneType(ZoneTypeDef::ZONE_RELIVE, newpos))
				{
					pUser->changeMap(toscene,newpos);
					return 1;
				}
				else
				{
					return 1;
				}
			}
		}
	}
	else
	{
		Cmd::Session::t_changeScene_SceneSession cmd;
		cmd.id = pUser->id;
		cmd.temp_id = pUser->tempid;
		cmd.x = 0;
		cmd.y = 0;
		bzero(cmd.map_file ,sizeof(cmd.map_file));
		bzero(cmd.map_name ,sizeof(cmd.map_name));
		if(deathBackto)
		{
			if(pUser->deathBackToMapID)
			{
				cmd.map_id = pUser->deathBackToMapID;
			}
			else
			{
				if (this->getCountryID() == 6 && !this->commonCountryBacktoMapID)
				{
					cmd.map_id = (pUser->charbase.country<<16) + this->foreignerBacktoMapID;
				}
				else
				{
					cmd.map_id = this->backtoMapID;
				}
			}
		}
		else
		{
			cmd.map_id = this->backtoCityMapID;
		}
		//bcopy(backtoMapName , cmd.map_name , sizeof(cmd.map_name));
		sessionClient->sendCmd(&cmd, sizeof(cmd));
		std::string mapname;
		SceneManager::MapMap_iter map_iter = SceneManager::getInstance().map_info.find(cmd.map_id & 0X0000FFFF);
		SceneManager::CountryMap_iter country_iter = SceneManager::getInstance().country_info.find(cmd.map_id >> 16);
		if(map_iter != SceneManager::getInstance().map_info.end() && 
				country_iter != SceneManager::getInstance().country_info.end())
		{
			mapname = country_iter->second.name;
			mapname+= "·";
			mapname+= map_iter->second.name;
			strncpy(pUser->wait_gomap_name,mapname.c_str(),MAX_NAMESIZE);
			Zebra::logger->debug("在别的场景寻找重生地,地图名称:%s",pUser->wait_gomap_name);
		}
		else
		{
			Zebra::logger->debug("在别的场景寻找重生地,地图ID:%d",cmd.map_id);
		}
		return 2;
	}
	// */
	return 0;
}

/**
 * \brief 在地图指定类型的区域里面随机产生坐标
 * \param type 区域类型
 * \param pos 产生的坐标
 * \param orign 原始坐标,用来找最近本类型区域
 * \return 是否成功
 */
bool Scene::randzPosByZoneType(const int type, zPos &pos , const zPos orign)
{
	if (ZoneTypeDef::ZONE_NONE == type) {
		return randPosByRegion(_index, pos);
	}

	ZoneTypeDef *near = NULL;
	//soke 遍历地图中所有特殊类型区域
	for(ZoneTypeDefVector::iterator it = zoneTypeDef.begin(); it != zoneTypeDef.end(); it++)
	{
		//soke 查找和要查询的类型匹配的区域	
		if (ZoneTypeDef::ZONE_NONE != ((*it).type & type))
		{
			//soke 比较并保存最近的该特殊类型区域的坐标
			if (NULL == near
					|| abs((*it).pos.x - orign.x) + abs((*it).pos.y - orign.y) < abs(near->pos.x - orign.x) + abs(near->pos.y - orign.y))
			{
				near = &(*it);
			}
		}
	}
	if(near)
	{
		return randPosByRegion(near->region.index, pos);
	}
	return false;
}

/**
 * \brief 在某位置一定范围内随机找一点
 * 重试10次，10次内找不到则放弃返回false
 *
 *
 * \param center 中心位置
 * \param pos 返回找到的位置
 * \param rectx 范围宽
 * \param recty 范围高
 * \return 查找是否成功
 */
bool Scene::randzPosOnRect(const zPos &center , zPos &pos , WORD rectx , WORD recty) const
{
	int randx = 0;
	int randy = 0;
	int times = 10;
	while(times)
	{
		times --;
		randx = zMisc::randBetween(1 , rectx);
		randy = zMisc::randBetween(1 , recty);
		pos.x = center.x + randx - (rectx + 1)/2;
		pos.y = center.y + randy - (recty + 1)/2;
		if(pos.x < this->width() || pos.y < this->height()) 
		{
			return true;
		}
	}
	return false;
}

/**
 * \brief 清除地图，释放地图资源
 *
 */
void Scene::final()
{
	if(inited)
	{
		allTiles.clear();
		sceneWH.x=0;
		sceneWH.y=0;
	}
}

/**
 * \brief 初始化地图
 * \param countryid 国家id
 * \param mapid 地图id
 */
bool Scene::init(DWORD countryid , DWORD mapid)
{
	if(inited)
	{
		Zebra::logger->error("重复初始化");
		return inited;
	}
	//std::string s;
	/*
	zXMLParser parser;
	parser.initStr(xmlConfig);
	xmlNodePtr root=parser.getRootNode("map");
	// */
	std::string mapname;
	std::string countryname;
	std::string filename;
	SceneManager::MapMap_iter map_iter = SceneManager::getInstance().map_info.find(mapid);
	if(map_iter == SceneManager::getInstance().map_info.end())
	{
		Zebra::logger->error("得到地图名称失败");
		return inited;
	}
	SceneManager::CountryMap_iter country_iter = SceneManager::getInstance().country_info.find(countryid);
	if(country_iter == SceneManager::getInstance().country_info.end())
	{
		Zebra::logger->error("得到国家信息失败");
		return inited;
	}

	mapname = country_iter->second.name;
	mapname+= "·";
	mapname+= map_iter->second.name;
	strncpy(this->name,mapname.c_str(),MAX_NAMESIZE);
	char temp[20];
	bzero(temp , sizeof(temp));
	sprintf(temp , "%d" , country_iter->second.id);
	fileName = temp;
	fileName+= ".";
	fileName+= map_iter->second.filename;
	this->id = (country_iter->second.id << 16) + map_iter->second.id;
	this->countryID = countryid;
	this->function = map_iter->second.function;
	this->level = map_iter->second.level;
	this->exprate = map_iter->second.exprate/100.0f;
	 

	//如果该地图没有重生点
	if(map_iter->second.backto)
	{
		this->backtoMapID = (country_iter->second.id << 16) + map_iter->second.backto;
	}
	//外国人来到本国的重生地图
	if(map_iter->second.foreignbackto)
	{
		this->foreignerBacktoMapID = /*(country_iter->second.id << 16) + */map_iter->second.foreignbackto;
	}
	//在公共国死亡重生地图
	if(map_iter->second.commoncountrybackto)
	{
		this->commonCountryBacktoMapID = /*(country_iter->second.id << 16) + */map_iter->second.commoncountrybackto;
	}
	//无国家人在外国死亡重生地图
	if(map_iter->second.commonuserbackto)
	{
		this->commonUserBacktoMapID = /*(country_iter->second.id << 16) + */map_iter->second.commonuserbackto;
	}
	//如果该地图不是主城
	if(map_iter->second.backtoCity)
	{
		this->backtoCityMapID = (country_iter->second.id << 16) + map_iter->second.backtoCity;
	}

	/// 国战战场死亡后，攻方死亡重生地
	if (map_iter->second.countrydarebackto)
	{
		this->countryDareBackToMapID = map_iter->second.countrydarebackto;
	}

	if (map_iter->second.countrydefbackto)
	{
		this->countryDefBackToMapID = map_iter->second.countrydefbackto;
	}
	if (map_iter->second.pklevel)
	{
		this->pklevel = map_iter->second.pklevel;
	}
	
	//如果该地图不是国战目的地
	if(map_iter->second.backtodare)
	{
		this->backtoDareMapID = map_iter->second.backtodare;
	}
	if(!loadMapFile())return inited;
	screenx = (sceneWH.x+SCREEN_WIDTH -1)/SCREEN_WIDTH;
	screeny = (sceneWH.y+SCREEN_HEIGHT -1)/SCREEN_HEIGHT;
	screenMax=screenx*screeny;
	setSceneWH(sceneWH, screenx, screeny, screenMax);

	//预先建立地图非阻挡点索引
	{
		zPos tempPos;
		for(tempPos.x = 0; tempPos.x < sceneWH.x; tempPos.x++)
		{
			for(tempPos.y = 0; tempPos.y < sceneWH.y; tempPos.y++)
			{
				if (!checkBlock(tempPos, TILE_BLOCK))
					_index.push_back(tempPos);
			}
		}
	}

	for(ZoneTypeDefVector::iterator it = zoneTypeDef.begin(); it != zoneTypeDef.end(); it++)
	{
		ZoneTypeDef &zone = *it;
		zPosRevaluate(zone.pos);
		/*
		Zebra::logger->debug("zonedef: %u, %u, %u, %u, %u, %u",
				zone.pos.x, zone.pos.y, zone.width, zone.height, zone.type, zone.initstate);
				// */
		initRegion(zone.region, zone.pos, zone.width, zone.height);
	}

	//从脚本文件里取得共享脚本id和npcid的对应
	std::map<int,int> scriptMap;
	zXMLParser xp;
	if (!xp.initFile(Zebra::global["mapdir"] + "NpcAIScript.xml"))
	{
		Zebra::logger->error("不能读取npc脚本文件 %s", (Zebra::global["mapdir"] + "NpcAIScript.xml").c_str());
	}
	xmlNodePtr infoNode = xp.getRootNode("info");
	if (infoNode)
	{
		xmlNodePtr scriptNode = xp.getChildNode(infoNode, NULL);
		while (scriptNode)
		{
			int shared;//是否共享脚本
			xp.getNodePropNum(scriptNode, "shared", &shared, sizeof(shared));
			if (shared)
			{
				int sID;//脚本id
				int nID;//npcid
				xp.getNodePropNum(scriptNode, "id", &sID, sizeof(sID));
				xp.getNodePropNum(scriptNode, "npcid", &nID, sizeof(nID));
				scriptMap[nID] = sID;
			}
			scriptNode = xp.getNextNode(scriptNode, NULL);
		}
	}

	for(NpcDefineVector::iterator it = npcDefine.begin(); it != npcDefine.end(); it++)
	{
		t_NpcDefine *define = &(*it);
		zPosRevaluate(define->pos);
		/*
		Zebra::logger->debug("define: %u, %s, %u, %u, %u, %u, %u, %u, %u",
				define->id, define->name, define->pos.x, define->pos.y,
				define->width, define->height, define->num, define->interval, define->initstate);
				// */
		initRegion(define->region, define->pos, define->width, define->height);
		
		//检查是否有共享脚本
		if ((0==define->scriptID)&&(scriptMap.end()!=scriptMap.find(define->id)))
		{
			define->scriptID = scriptMap[define->id];
			//Zebra::logger->debug("给 %u 设置共享脚本 %u 号", define->id, define->scriptID);
		}
		
		if (!initByNpcDefine(define))
		{
			Zebra::logger->debug("初始化NPC失败");
			return false;
		}
	}

	countryTax = 0; ///将本地图的国家税率初始化成0；
	inited=true;
	return inited;
}

/**
 * \brief 初始化一个矩形范围，建立可移动点索引
 * \param reg 一个矩形范围
 * \param pos 矩形范围的左上点坐标
 * \param width 矩形范围的宽
 * \param height 矩形范围的高
 * \return 初始化是否成功
 */
void Scene::initRegion(zRegion &reg, const zPos &pos, const WORD width, const WORD height)
{
	zPos tempPos;
	reg.s = pos;
	reg.e.x = pos.x + width;
	reg.e.y = pos.y + height;
	zPosRevaluate(reg.e);
	reg.c.x = reg.s.x + (reg.e.x - reg.s.x) / 2;
	reg.c.y = reg.s.y + (reg.e.y - reg.s.y) / 2;
	for(tempPos.x = reg.s.x; tempPos.x < reg.e.x; tempPos.x++)
	{
		for(tempPos.y = reg.s.y; tempPos.y < reg.e.y; tempPos.y++)
		{
			if (!checkBlock(tempPos, TILE_BLOCK))
				reg.index.push_back(tempPos);
		}
	}
}

/**
 * \brief 在一个矩形范围内随机获取一个坐标
 * \param reg 一个矩形范围
 * \param pos 输出坐标
 * \return 获取坐标是否成功
 */
bool Scene::randPosByRegion(const zPosIndex &index, zPos &pos) const
{
	if (index.empty())
		return false;
	else
	{
		int r = zMisc::randBetween(0, index.size() - 1);
		int i=0;
		while (checkBlock(index[r]) && i++<5)
			r = zMisc::randBetween(0, index.size() - 1);
			
		pos = index[r];
		return true;
	}
}

/**
 * \brief 初始化一个Npc
 * \param sceneNpc 需要初始化的Npc
 * \param init_region Npc初始坐标范围
 */
void Scene::initNpc(SceneNpc *sceneNpc, zRegion *init_region, zPos myPos)
{
	zPos pos;
	SceneNpcManager::getMe().addSceneNpc(sceneNpc);
	if (sceneNpc->isSpecialNpc())
		SceneNpcManager::getMe().addSpecialNpc(sceneNpc);
	if (myPos.x != 0 && myPos.y !=0)
	{
		if (!refreshNpc(sceneNpc,myPos))
		{
			if(randPosByRegion(init_region == NULL ? sceneNpc->define->region.index : init_region->index, pos))
				refreshNpc(sceneNpc,pos);
			//else
			//	Zebra::logger->debug("%s 查找空格失败 %s:(%u,%u)", sceneNpc->name, name, pos.x, pos.y);
		}
	}
	else
	{
		if(randPosByRegion(init_region == NULL ? sceneNpc->define->region.index : init_region->index, pos))
			refreshNpc(sceneNpc,pos);
		//else
		//	Zebra::logger->debug("%s 查找空格失败 %s:(%u,%u)", sceneNpc->name, name, pos.x, pos.y);
	}
}

/**
 * \brief 刷新地图上的NPC坐标,没有就添加,并且清除先前的阻挡
 * \param sceneNpc 需要刷新的Npc
 * \param pos 刷新的坐标
 * \return 成功返回true,否则返回false
 */
bool Scene::refreshNpc(SceneNpc *sceneNpc,const zPos & pos)
{
	if (sceneNpc)
	{
		zPos oldPos=sceneNpc->getPos();
		bool newnpc=sceneNpc->hasInScene();
		if(sceneNpc->define->initstate == zSceneEntry::SceneEntry_Normal)
		{
			//初始化为普通状态
			if (refresh(sceneNpc, pos))
			{
				switch (sceneNpc->npc->kind)
				{
					case NPC_TYPE_TOTEM:
					case NPC_TYPE_TRAP:
						if(!newnpc) clearBlock(oldPos);
						break;
					default:
						{
							//查找非阻挡点成功
							setBlock(pos);
							//清除以前阻挡
							if(!newnpc) clearBlock(oldPos);
						}
						break;
				}
				sceneNpc->setState(zSceneEntry::SceneEntry_Normal);
				return true;
			}
		}
		else
		{
			//初始化为隐藏状态
			if(refresh(sceneNpc, pos))
			{
				//清除以前阻挡
				if(!newnpc) clearBlock(oldPos);
				return true;
			}
		}
	}
	return false;
}

/**
 * \brief 召唤npc
 * 召唤的都是动态npc，需要手动删除
 *
 * \param define npc定义
 * \param pos 目标位置
 * \param base npc基本信息
 * \return 召唤出npc的数量
 */
int Scene::summonNpc(const t_NpcDefine &define, const zPos &pos, zNpcB *base)
{
	zRegion init_region;
	initRegion(init_region, pos, SCREEN_WIDTH, SCREEN_HEIGHT);

	//Zebra::logger->debug("*******%u, %s, %u, %u, %u", define.id, define.name, define.pos.x, define.pos.y, define.num);
	unsigned int count = 0;
	for(WORD i = 0; i < define.num; i++)
	{
		t_NpcDefine *pDefine = new t_NpcDefine(define);
		if (pDefine)
		{
			//Zebra::logger->debug("%u, %s, %u, %u, %u", pDefine->id, pDefine->name, pDefine->pos.x, pDefine->pos.y, pDefine->num);
			SceneNpc *sceneNpc = new SceneNpc(this, base, pDefine, SceneNpc::GANG);
			if (sceneNpc)
			{
				initNpc(sceneNpc, &init_region);
				if (sceneNpc->getState() == zSceneEntry::SceneEntry_Normal)
				{
					Cmd::stAddMapNpcMapScreenUserCmd addNpc;
					//天下第一
					sceneNpc->full_t_MapNpcData(addNpc.data);
					sendCmdToNine(sceneNpc->getPosI(), &addNpc, sizeof(addNpc));
					Cmd::stRTMagicPosUserCmd ret;
					sceneNpc->full_stRTMagicPosUserCmd(ret);
					sendCmdToNine(sceneNpc->getPosI(), &ret,sizeof(ret));
				}
				count++;
			}
			else
			{
				Zebra::logger->fatal("%s:SceneNpc分配内存失败", __FUNCTION__);
				SAFE_DELETE(pDefine);
			}
		}
		else
		{
			Zebra::logger->fatal("%s:t_NpcDefine分配内存失败", __FUNCTION__);
		}
	}
	return count;
}

/**
 * \brief 根据定义初始化所有的Npc
 * \param pDefine Npc定义数据
 * \return 初始化是否成功
 */
bool Scene::initByNpcDefine(const t_NpcDefine *pDefine)
{
	zNpcB *base = npcbm.get(pDefine->id);

	bool retval = true;
	if (base)
	{
		for(DWORD i = 0; i < pDefine->num; i++)
		{
			SceneNpc *sceneNpc = new SceneNpc(this, base, pDefine, SceneNpc::STATIC);
			if (sceneNpc)
			{
				initNpc(sceneNpc, NULL);
				if (!pDefine->petList.empty()||!pDefine->dieList.empty())
				{
					sceneNpc->setState(zSceneEntry::SceneEntry_Death);
					clearBlock(sceneNpc->getPos());
					sceneNpc->setMoveTime(SceneTimeTick::currentTime, 10000);
				}
				/*
				switch (base->kind)
				{
					case NPC_TYPE_HUMAN:
					case NPC_TYPE_NORMAL:
					case NPC_TYPE_BBOSS:
					case NPC_TYPE_LBOSS:
					case NPC_TYPE_PBOSS:
					case NPC_TYPE_BACKBONE:
					case NPC_TYPE_GOLD:
					case NPC_TYPE_BACKBONEBUG:
					case NPC_TYPE_ABERRANCE:
					case NPC_TYPE_GUARD:
						{
							sceneNpc->setState(zSceneEntry::SceneEntry_Death);
							clearBlock(sceneNpc->getPos());
							sceneNpc->setMoveTime(SceneTimeTick::currentTime, 10000);
						}
						break;
					default:
						break;
				}
				*/
			}
			else
				retval = false;
		}
	}
	else
		retval = false;
	return retval;
}

/**
 * \brief 添加跳转点
 * \return 添加跳转点是否成功
 */
bool Scene::initWayPoint(zXMLParser *parser,const xmlNodePtr node , DWORD countryid)
{
	WayPoint wp;
	if(wp.init(parser,node , countryid) && wpm.addWayPoint(wp))
	{
		for(int i=0;i<wp.pointC;i++)
			setBlock(wp.point[i], TILE_DOOR);
		return true;
	}
	return false;
}

/**
 * \brief 读取地图以及其配置文件
 *
 * 
 * \return 读取是否成功
 */
bool Scene::loadMapFile()
{
	if (!LoadMap((Zebra::global["mapdir"] + getRealFileName()+".mps").c_str(),allTiles,sceneWH.x,sceneWH.y))
	{
		Zebra::logger->error("加载 %s 失败",(Zebra::global["mapdir"] + getRealFileName()+".mps").c_str());
		return false;
	}

	zXMLParser xml;
	if (!xml.initFile(Zebra::global["mapdir"] + getRealFileName()+".xml"))
	{
		Zebra::logger->error("不能读取场景配置文件 %s", (Zebra::global["mapdir"] + getRealFileName()+".xml").c_str());
		return false;
	}

	xmlNodePtr root = xml.getRootNode("map");
	if (root)
	{
		xmlNodePtr node = xml.getChildNode(root, NULL);
		while(node)
		{
			//Zebra::logger->debug(":::::::::::::::::::::: %s", node->name);
			if (strcmp((char *)node->name, "zonedef") == 0)
			{
				ZoneTypeDef zone;
				xml.getNodePropNum(node, "x", &zone.pos.x, sizeof(zone.pos.x));
				xml.getNodePropNum(node, "y", &zone.pos.y, sizeof(zone.pos.y));
				xml.getNodePropNum(node, "width", &zone.width, sizeof(zone.width));
				xml.getNodePropNum(node, "height", &zone.height, sizeof(zone.height));
				xml.getNodePropNum(node, "type", &zone.type, sizeof(zone.type));
				char tempChar[32];
				bzero(tempChar, sizeof(tempChar));
				xml.getNodePropStr(node, "initstate", tempChar, sizeof(tempChar));
				if (0 == strcasecmp(tempChar, "hide"))
					zone.initstate = zSceneEntry::SceneEntry_Hide;
				else
					zone.initstate = zSceneEntry::SceneEntry_Normal;
				zone.state = zone.initstate;
				//Zebra::logger->debug("zonedef: x = %u, y = %u, width = %u, height = %u, type = %u, initstate = %u", zone.pos.x, zone.pos.y, zone.width, zone.height, zone.type, zone.initstate);
				zoneTypeDef.push_back(zone);
			}
			else if (strcmp((char *)node->name, "npc") == 0)
			{
				t_NpcDefine define;
				xml.getNodePropNum(node, "id", &define.id, sizeof(define.id));
				//xml.getNodePropStr(node, "name", &define.name, sizeof(define.name));
				xml.getNodePropNum(node, "x", &define.pos.x, sizeof(define.pos.x));
				xml.getNodePropNum(node, "y", &define.pos.y, sizeof(define.pos.y));
				xml.getNodePropNum(node, "width", &define.width, sizeof(define.width));
				xml.getNodePropNum(node, "height", &define.height, sizeof(define.height));
				xml.getNodePropNum(node, "num", &define.num, sizeof(define.num));
				xml.getNodePropNum(node, "interval", &define.interval, sizeof(define.interval));
				xml.getNodePropNum(node, "rush", &define.rushID, sizeof(define.rushID));
				xml.getNodePropNum(node, "rushrate", &define.rushRate, sizeof(define.rushRate));
				xml.getNodePropNum(node, "delay", &define.rushDelay, sizeof(define.rushDelay));
				xml.getNodePropNum(node,"script",&define.scriptID,sizeof(define.scriptID));	//巡逻说话等AI设置
				//xml.getNodePropNum(node, "summonid", &define.summonID, sizeof(define.summonID));
				//xml.getNodePropNum(node, "summonnum", &define.summonNum, sizeof(define.summonNum));
				char tempChar[512];
				bzero(tempChar, sizeof(tempChar));
				xml.getNodePropStr(node, "pet", tempChar, sizeof(tempChar));
				if (strcmp(tempChar, ""))
					define.fillNpcMap(tempChar, define.petList);

				bzero(tempChar, sizeof(tempChar));
				xml.getNodePropStr(node, "summon", tempChar, sizeof(tempChar));
				if (strcmp(tempChar, ""))
					define.fillNpcMap(tempChar, define.summonList);

				bzero(tempChar, sizeof(tempChar));
				xml.getNodePropStr(node, "deathsummon", tempChar, sizeof(tempChar));
				if (strcmp(tempChar, ""))
					define.fillNpcMap(tempChar, define.deathSummonList);

				bzero(tempChar, sizeof(tempChar));
				xml.getNodePropStr(node, "die", tempChar, sizeof(tempChar));
				if (strcmp(tempChar, ""))
				{
					std::vector<std::string> vs;
					std::vector<std::string> sub_vs;
					vs.clear();
					Zebra::stringtok(vs, tempChar, ";");
					for (DWORD i=0; i<vs.size(); i++)
					{
						sub_vs.clear();
						Zebra::stringtok(sub_vs, vs[i].c_str(), "-");
						if (sub_vs.size()==3)
							define.dieList.push_back(std::make_pair(atoi(sub_vs[0].c_str()), zPos(atoi(sub_vs[1].c_str()), atoi(sub_vs[2].c_str()))));
					}
				}

				bzero(tempChar, sizeof(tempChar));
				xml.getNodePropStr(node, "initstate", tempChar, sizeof(tempChar));
				if (0 == strcasecmp(tempChar, "hide"))
					define.initstate = zSceneEntry::SceneEntry_Hide;
				else
					if (!define.petList.empty()||!define.dieList.empty())
						define.initstate = zSceneEntry::SceneEntry_Death;
					else
						define.initstate = zSceneEntry::SceneEntry_Normal;

				if (npcbm.get(define.id))
				{
					npcDefine.push_back(define);
				}
				else
				{
					if(strlen(define.name)!=0)
						Zebra::logger->warn("基本数据表格中没有这个Npc %s", define.name);
				}
			}
			else if (strcmp((char *)node->name, "waypoint") == 0)
			{
				if(!initWayPoint(&xml,node,countryID))
				{
					Zebra::logger->error("解析跳转点错误");
					return false;
				}
			}
			else if (strcmp((char *)node->name, "fixedrush") == 0)
			{
				char tempChar[32];
				bzero(tempChar, sizeof(tempChar));
				xml.getNodePropNum(node, "id", &fixedRush.id, sizeof(fixedRush.id));
				if (fixedRush.id)
				{
					fixedRush.nextTime = SceneTimeTick::currentTime.sec() + 10;

					struct tm tm;
					xml.getNodePropStr(node, "allStart", tempChar, sizeof(tempChar));
					if(strptime(tempChar,"%Y%m%d %H:%M:%S", &tm)!=NULL)
					{
						time_t t=mktime(&tm);
						if(t!=(time_t)-1) fixedRush.allStart = t;
					}
					bzero(tempChar, sizeof(tempChar));
					xml.getNodePropStr(node, "allEnd", tempChar, sizeof(tempChar));
					if(strptime(tempChar,"%Y%m%d %H:%M:%S", &tm)!=NULL)
					{
						time_t t=mktime(&tm);
						if(t!=(time_t)-1) fixedRush.allEnd = t;
					}

					bzero(tempChar, sizeof(tempChar));
					xml.getNodePropStr(node, "startTime", tempChar, sizeof(tempChar));
					strptime(tempChar,"%H:%M:%S", &fixedRush.startTime);

					bzero(tempChar, sizeof(tempChar));
					xml.getNodePropStr(node, "endTime", tempChar, sizeof(tempChar));
					strptime(tempChar,"%H:%M:%S", &fixedRush.endTime);

					xml.getNodePropNum(node, "weekDay", &fixedRush.weekDay, sizeof(fixedRush.weekDay));
					xml.getNodePropNum(node, "delay", &fixedRush.delay, sizeof(fixedRush.delay));
				}
			}
			/*
			else if (strcmp((char *)node->name, "params") == 0)
			{
				char tempChar[32];
				bzero(tempChar, sizeof(tempChar));
				xml.getNodePropNum(node, "id", &fixedRush.id, sizeof(fixedRush.id));
				if (fixedRush.id)
				{
					fixedRush.nextTime = SceneTimeTick::currentTime.sec() + 10;

					struct tm tm;
					xml.getNodePropStr(node, "start", tempChar, sizeof(tempChar));
					if(strptime(tempChar,"%Y%m%d %H:%M:%S", &tm)!=NULL)
					{
						time_t t=mktime(&tm);
						if(t!=(time_t)-1) fixedRush.startTime = t;
					}
					xml.getNodePropStr(node, "end", tempChar, sizeof(tempChar));
					if(strptime(tempChar,"%Y%m%d %H:%M:%S", &tm)!=NULL)
					{
						time_t t=mktime(&tm);
						if(t!=(time_t)-1) fixedRush.endTime = t;
					}

					xml.getNodePropNum(node, "startHour", &fixedRush.startHour, sizeof(fixedRush.startHour));
					xml.getNodePropNum(node, "endHour", &fixedRush.endHour, sizeof(fixedRush.endHour));
				}
			}
			*/
			node = xml.getNextNode(node, NULL);
		}
	}
	else
	{
		Zebra::logger->warn("加载地图配置%s失败", (Zebra::global["mapdir"] + fileName+".xml").c_str());
		return false;
	}

	return true;
}

/**
 * \brief 发送指令到地图屏索引中所有玩家的回调函数
 *
 */
struct SceneSendToEveryOne : public zSceneEntryCallBack
{
	const void *cmd;		/// 待发送的指令
	const int len;	/// 待发送指令的大小
	SceneSendToEveryOne(const void *cmd, const int len) : cmd(cmd), len(len) {};
	/**
	 * \brief 回调函数
	 * \param entry 地图物件，这里是玩家
	 * \return 回调是否成功
	 */
	bool exec(zSceneEntry *entry)
	{
		((SceneUser *)entry)->sendCmdToMe(cmd,len);
		return true;
	}
};

/**
 * \brief 发送指令到本地图的所有用户
 * \param pstrCmd 待发送的指令
 * \param nCmdLen 待发送指令的长度
 * \param locker 操作过程中是否对地图索引进行加锁操作
 * \return 广播指令是否成功
 */
bool Scene::sendCmdToScene(const void *pstrCmd, const int nCmdLen)
{
	/// whj 
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::Scene::t_User_ForwardMap *sendCmd=(Cmd::Scene::t_User_ForwardMap *)buf;
	constructInPlace(sendCmd);
	sendCmd->maptempid=tempid;
	sendCmd->size=nCmdLen;
	bcopy(pstrCmd,sendCmd->data,nCmdLen);
	SceneTaskManager::getInstance().broadcastCmd(sendCmd,sizeof(Cmd::Scene::t_User_ForwardMap)+nCmdLen); 
	return true;
}
/**
 * \brief 发送指令到某一屏周围共九屏的所有用户
 * \param posi 中心屏
 * \param pstrCmd 待发送的指令
 * \param nCmdLen 待发送指令的长度
 * \return 广播指令是否成功
 */
bool Scene::sendCmdToNine(const zPosI posi, const void *pstrCmd, const int nCmdLen)
{
#if 1
	/// whj 网关发送9屏测试
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::Scene::t_Nine_ForwardScene *sendCmd=(Cmd::Scene::t_Nine_ForwardScene *)buf;
	constructInPlace(sendCmd);
	sendCmd->maptempid=tempid;
	sendCmd->screen=posi;
	sendCmd->size=nCmdLen;
	bcopy(pstrCmd,sendCmd->data,nCmdLen);
	SceneTaskManager::getInstance().broadcastCmd(sendCmd,sizeof(Cmd::Scene::t_Nine_ForwardScene)+nCmdLen); 
	return true;
#else
	SceneSendToEveryOne sendeveryone(pstrCmd, nCmdLen);
	const zPosIVector &pv = getNineScreen(posi);
	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		//Zebra::logger->debug("%s: %u", __FUNCTION__, *it);
		execAllOfScreen(zSceneEntry::SceneEntry_Player,*it,sendeveryone);
	}
	return true;
#endif
}


/**
 * \brief 发送指令到地图屏索引中所有玩家的回调函数
 *
 */
struct SceneSendToWatchTrap : public zSceneEntryCallBack
{
	const void *cmd;		/// 待发送的指令
	const int len;	/// 待发送指令的大小
	SceneSendToWatchTrap(const void *cmd, const int len) : cmd(cmd), len(len) {};
	/**
	 * \brief 回调函数
	 * \param entry 地图物件，这里是玩家
	 * \return 回调是否成功
	 */
	bool exec(zSceneEntry *entry)
	{
		if (((SceneUser *)entry)->watchTrap)((SceneUser *)entry)->sendCmdToMe(cmd,len);
		return true;
	}
};
/**
 * \brief 发送指令到某一屏周围共九屏的所有具有陷阱侦察能力的用户
 * \param posi 中心屏
 * \param pstrCmd 待发送的指令
 * \param nCmdLen 待发送指令的长度
 * \return 广播指令是否成功
 */
bool Scene::sendCmdToWatchTrap(const zPosI posi, const void *pstrCmd, const int nCmdLen)
{
	SceneSendToWatchTrap sendeveryone(pstrCmd, nCmdLen);
	const zPosIVector &pv = getNineScreen(posi);
	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		//Zebra::logger->debug("%s: %u", __FUNCTION__, *it);
		execAllOfScreen(zSceneEntry::SceneEntry_Player,*it,sendeveryone);
	}
	return true;
}

/**
 * \brief 发送指令到地图屏索引中所有玩家的回调函数
 *
 */
struct SceneSendToUnWatchTrap : public zSceneEntryCallBack
{
	const void *cmd;		/// 待发送的指令
	const int len;	/// 待发送指令的大小
	SceneSendToUnWatchTrap(const void *cmd, const int len) : cmd(cmd), len(len) {};
	/**
	 * \brief 回调函数
	 * \param entry 地图物件，这里是玩家
	 * \return 回调是否成功
	 */
	bool exec(zSceneEntry *entry)
	{
		if (!((SceneUser *)entry)->watchTrap)((SceneUser *)entry)->sendCmdToMe(cmd,len);
		return true;
	}
};
/**
 * \brief 发送指令到某一屏周围共九屏的所有具有陷阱侦察能力的用户
 * \param posi 中心屏
 * \param pstrCmd 待发送的指令
 * \param nCmdLen 待发送指令的长度
 * \return 广播指令是否成功
 */
bool Scene::sendCmdToNineUnWatch(const zPosI posi, const void *pstrCmd, const int nCmdLen)
{
	SceneSendToUnWatchTrap sendeveryone(pstrCmd, nCmdLen);
	const zPosIVector &pv = getNineScreen(posi);
	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		//Zebra::logger->debug("%s: %u", __FUNCTION__, *it);
		execAllOfScreen(zSceneEntry::SceneEntry_Player,*it,sendeveryone);
	}
	return true;
}

/**
 * \brief 发送指令到地图屏索引中所有玩家的回调函数
 *
 */

struct findPosInNine : public zSceneEntryCallBack
{
	const zPos &_pos;		///施法者中心坐标
	zPosVector &_range;	///坐标点列表
	findPosInNine(const zPos &pos, zPosVector &range) : _pos(pos), _range(range)
	{
		_range.reserve( 9 * SCREEN_WIDTH * SCREEN_HEIGHT);
	}
	/**
	 * \brief 回调函数
	 * \param entry 地图物件，这里是玩家
	 * \return 回调是否成功
	 */
	bool exec(zSceneEntry *entry)
	{
		if ((abs(_pos.x - entry->getPos().x) <=SCREEN_WIDTH) && (abs(_pos.y - entry->getPos().y) <=SCREEN_HEIGHT))
		{
			_range.push_back(entry->getPos());
		}
		return true;
	}
};

/**
 * \brief 查找九屏内的所有角色的坐标
 * \param posi 中心屏
 * \return 广播指令是否成功
 */
bool Scene::findEntryPosInNine(const zPos &vpos, zPosI vposi, zPosVector &range)
{
	findPosInNine findpos(vpos, range);
	const zPosIVector &pv = getNineScreen(vposi);
	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		//Zebra::logger->debug("%s: %u", __FUNCTION__, *it);
		execAllOfScreen(zSceneEntry::SceneEntry_Player,*it,findpos);
	}
	return true;
}

/**
 * \brief 查找九屏内的所有角色或NPC的坐标
 * \param posi 中心屏
 * \return 广播指令是否成功
 */
bool Scene::findEntryPosInOne(const zPos &vpos, zPosI vposi, zPosVector &range)
{
	findPosInNine findpos(vpos, range);
	const zPosIVector &pv = getScreenByRange(vpos, 19);
	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		execAllOfScreen(zSceneEntry::SceneEntry_Player,*it,findpos);
		execAllOfScreen(zSceneEntry::SceneEntry_NPC,*it,findpos);
	}
	return true;
}

/**
 * \brief 发送指令到地图屏索引中所有玩家的回调函数
 *
 */
struct SendToEveryOneExceptMe : public zSceneEntryCallBack
{
	const zSceneEntry *me;
	const void *cmd;		/// 待发送的指令
	const int len;	/// 待发送指令的大小
	SendToEveryOneExceptMe(const zSceneEntry *pEntry, const void *cmd, const int len) : me(pEntry), cmd(cmd), len(len){};
	/**
	 * \brief 回调函数
	 * \param entry 地图物件，这里是玩家
	 * \return 回调是否成功
	 */
	bool exec(zSceneEntry *entry)
	{
		if (entry != me) ((SceneUser *)entry)->sendCmdToMe(cmd,len);
		return true;
	}
};

/**
 * \brief 发送指令到某一屏周围共九屏的所有用户，除了对象本身
 * \param pEntry 对象本身
 * \param posi 中心屏
 * \param pstrCmd 待发送的指令
 * \param nCmdLen 待发送指令的长度
 * \return 广播指令是否成功
 */
bool Scene::sendCmdToNineExceptMe(zSceneEntry *pEntry, const zPosI posi, const void *pstrCmd, const int nCmdLen)
{
#if 1
	/// whj 网关发送9屏测试
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::Scene::t_Nine_ExceptMe_ForwardScene *sendCmd=(Cmd::Scene::t_Nine_ExceptMe_ForwardScene *)buf;
	constructInPlace(sendCmd);
	sendCmd->maptempid=tempid;
	sendCmd->screen=posi;
	sendCmd->exceptme_id=pEntry->id;
	sendCmd->size=nCmdLen;
	bcopy(pstrCmd,sendCmd->data,nCmdLen);
	SceneTaskManager::getInstance().broadcastCmd(sendCmd,sizeof(Cmd::Scene::t_Nine_ExceptMe_ForwardScene)+nCmdLen); 
	return true;
#else
	SendToEveryOneExceptMe sendeveryone(pEntry, pstrCmd, nCmdLen);
	const zPosIVector &pv = getNineScreen(posi);
	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		//Zebra::logger->debug("%s: %u", __FUNCTION__, *it);
		execAllOfScreen(zSceneEntry::SceneEntry_Player,*it,sendeveryone);
	}
	return true;
#endif
}

/**
 * \brief 广播指令到正向3屏或者5屏的所有用户
 * \param posi 中心屏
 * \param direct 方向
 * \param pstrCmd 待发送的指令
 * \param nCmdLen 待发送指令的长度
 * \return 广播指令是否成功
 */
bool Scene::sendCmdToDirect(const zPosI posi, const int direct, const void *pstrCmd, const int nCmdLen)
{
#if 1
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::Scene::t_Nine_dir_ForwardScene *sendCmd=(Cmd::Scene::t_Nine_dir_ForwardScene *)buf;
	constructInPlace(sendCmd);
	sendCmd->maptempid=tempid;
	sendCmd->screen=posi;
	sendCmd->dir=direct;
	sendCmd->size=nCmdLen;
	bcopy(pstrCmd,sendCmd->data,nCmdLen);
	SceneTaskManager::getInstance().broadcastCmd(sendCmd,sizeof(Cmd::Scene::t_Nine_dir_ForwardScene)+nCmdLen); 
	return true;
#else
	SceneSendToEveryOne sendeveryone(pstrCmd, nCmdLen);
	const zPosIVector &pv = getDirectScreen(posi, direct);
	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		//Zebra::logger->debug("%s: %u", __FUNCTION__, *it);
		execAllOfScreen(zSceneEntry::SceneEntry_Player,*it,sendeveryone);
	}
	return true;
#endif
}

/**
 * \brief 广播指令到反向3屏或者5屏的所有用户
 * \param posi 中心屏
 * \param direct 方向
 * \param pstrCmd 待发送的指令
 * \param nCmdLen 待发送指令的长度
 * \return 广播指令是否成功
 */
bool Scene::sendCmdToReverseDirect(const zPosI posi, const int direct, const void *pstrCmd, const int nCmdLen)
{
#if 1
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::Scene::t_Nine_rdir_ForwardScene *sendCmd=(Cmd::Scene::t_Nine_rdir_ForwardScene *)buf;
	constructInPlace(sendCmd);
	sendCmd->maptempid=tempid;
	sendCmd->screen=posi;
	sendCmd->dir=direct;
	sendCmd->size=nCmdLen;
	bcopy(pstrCmd,sendCmd->data,nCmdLen);
	SceneTaskManager::getInstance().broadcastCmd(sendCmd,sizeof(Cmd::Scene::t_Nine_rdir_ForwardScene)+nCmdLen); 
	return true;
#else
	SceneSendToEveryOne sendeveryone(pstrCmd, nCmdLen);
	const zPosIVector &pv = getReverseDirectScreen(posi, direct);
	for(zPosIVector::const_iterator it = pv.begin(); it != pv.end(); it++)
	{
		//Zebra::logger->debug("%s: %u", __FUNCTION__, *it);
		execAllOfScreen(zSceneEntry::SceneEntry_Player,*it,sendeveryone);
	}
	return true;
#endif
}

/**
 * \brief 在地图上面移除玩家
 * \param so 玩家
 */
void Scene::removeUser(SceneUser *so)
{
	if (removeSceneEntry(so))
	{
		clearBlock(so->getPos());
	}
	//SceneUserManager::getMe().removeUser(so);
}

/**
 * \brief 把物品添加到地上，并通知周围9屏的玩家
 * \param ob 物品
 * \param pos 坐标
 * \param dwID 物品的主人
 * \return 添加物品是否成功
 */
bool Scene::addObject(zObject *ob,const zPos &pos,const unsigned long overdue_msecs,const unsigned long dwID, int protime)
{
	//Zebra::logger->debug("掉落物品");
	bool retval = false;
	zSceneObject *so = zSceneObject::create(ob, SceneTimeTick::currentTime);
	if(overdue_msecs)
	{
		zRTime ct = SceneTimeTick::currentTime;
		ct.addDelay(overdue_msecs);
		so->setOverDueTime(ct);
	}
	if (so)
	{
		zPos findedPos;
		zPosI posi;
		if(findPosForObject(pos, findedPos))
		{
			if (refresh(so, findedPos))
			{
				//设置保护
				if(dwID)
					so->setOwner(dwID, protime);
				zPos2zPosI(findedPos, posi);
				stObjectLocation pos(Cmd::OBJECTCELLTYPE_NONE, 0, findedPos.x, findedPos.y);
				so->getObject()->data.pos = pos;
/*				
				so->getObject()->data.pos.dwLocation=Cmd::OBJECTCELLTYPE_COMMON;
				so->getObject()->data.pos.x = findedPos.x;
				so->getObject()->data.pos.y = findedPos.y;
*/
				Cmd::stAddMapObjectMapScreenUserCmd add;
				if(so->getObject()->base->id == 672 || so->getObject()->base->id == 673)
				{
					add.data.dwOwner = 0;
					add.action = Cmd::OBJECTACTION_UPDATE;
				}
				else
				{
					add.action = Cmd::OBJECTACTION_DROP;
					SceneUser *pUser = SceneUserManager::getMe().getUserByID(so->getOwner());
					if(pUser)
					{
						add.data.dwOwner = pUser->tempid;
					}
					else
					{
						add.data.dwOwner = 0;
					}
				}
				add.data.dwMapObjectTempID = so->getObject()->data.qwThisID;
				add.data.dwObjectID = so->getObject()->base->id;
				strncpy(add.data.pstrName, so->getObject()->data.strName, MAX_NAMESIZE);
				add.data.x = so->getPos().x;
				add.data.y = so->getPos().y;
				add.data.wdNumber = so->getObject()->data.dwNum;
				add.data.wdLevel = so->getObject()->base->level;
				add.data.upgrade = so->getObject()->data.upgrade;
				add.data.kind = so->getObject()->data.kind;				
				sendCmdToNine(posi, &add, sizeof(add));
				retval = true;
			}
			else
				clearObjectBlock(findedPos);
		}
		if (!retval)
		{
//			so->clear();
			SAFE_DELETE(so);
		}
	}
	return retval;
}
/**
 * \brief 把物品添加到地上，并通知周围9屏的玩家
 * \param ob 需要添加的物品的基本数据
 * \param num 物品数量
 * \param pos 坐标
 * \param dwID 物品的主人
 * \param npc_id  魂魄石对应的npc id
 * \param Protection  物品保护时间开关 false关 true开
 * \param protime 物品保护时间 
 * \return 添加物品是否成功
 */
bool Scene::addObject(zObjectB *ob, const int num, const zPos &pos,const unsigned long dwID, int npc_id ,bool Protection , int protime)
{
	bool retval = false;
	//设置物品保护时间/秒
	int timetion = protime;
	zObject *o = zObject::create(ob,num);
	if (o)
	{
		//掉落物品也可能产生非白色装备
		//o->found(NULL,true);
		EquipMaker maker(NULL);
	//	maker.assign(NULL, o, o->base,true);
		maker.assign2(NULL, o, o->base,true); //soke 修改BOSS有几率掉落紫装

	    if(Protection == true)	
	    {
            timetion = 30; //设置物品保护时间/秒
	    }
	    else
	    {
	        timetion = 0;   //设置物品保护时间/秒
	    }
		

	    ///if ((npc_id>=21501 && npc_id<=21511) || (npc_id>=26501 && npc_id<=26511) || (npc_id>=30051 && npc_id<=30102))
	    ///{
	    /// protime=30; //设置物品保护时间/秒
	    ///}

		if (addObject(o, pos, 0, dwID, timetion))
		{
			zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,o->data.dwNum,1,0,NULL,this->id,this->name,"场景生成",o->base,o->data.kind,o->data.upgrade);
			retval = true;
		}
		//removed by lqy, addobject will clean this object auto
/*		
		else
			SAFE_DELETE(o);
*/
	}
	return retval;
}



//国家归属
/**
 * \brief 把物品添加到地上，并通知周围9屏的玩家
 * \param ob 物品
 * \param pos 坐标
 * \param dwID 物品的主人
 * \return 添加物品是否成功
 */
bool Scene::addObject2(zObject *ob,const zPos &pos,const unsigned long overdue_msecs,const unsigned long dwID, int protime)
{
	//Zebra::logger->debug("掉落物品");
	bool retval = false;
	zSceneObject *so = zSceneObject::create(ob, SceneTimeTick::currentTime);
	if(overdue_msecs)
	{
		zRTime ct = SceneTimeTick::currentTime;
		ct.addDelay(overdue_msecs);
		so->setOverDueTime(ct);
	}
	if (so)
	{
		zPos findedPos;
		zPosI posi;
		if(findPosForObject(pos, findedPos))
		{
			if (refresh(so, findedPos))
			{
				//设置保护
				if(dwID)
				{
					// so->setOwner(dwID, protime);
					SceneUser *pUsers = SceneUserManager::getMe().getUserByID(so->getOwner());
					if(pUsers)
					{
						so->setGuoOwner(pUsers->charbase.country, protime);
					}
				}
				

				zPos2zPosI(findedPos, posi);
				stObjectLocation pos(Cmd::OBJECTCELLTYPE_NONE, 0, findedPos.x, findedPos.y);
				so->getObject()->data.pos = pos;
/*				
				so->getObject()->data.pos.dwLocation=Cmd::OBJECTCELLTYPE_COMMON;
				so->getObject()->data.pos.x = findedPos.x;
				so->getObject()->data.pos.y = findedPos.y;
*/
				Cmd::stAddMapObjectMapScreenUserCmd add;
				if(so->getObject()->base->id == 672 || so->getObject()->base->id == 673)
				{
					add.data.dwOwner = 0;
					add.action = Cmd::OBJECTACTION_UPDATE;
				}
				else
				{
					add.action = Cmd::OBJECTACTION_DROP;
					SceneUser *pUser = SceneUserManager::getMe().getUserByID(dwID);
					if(pUser)
					{
						add.data.dwOwner = 0;
						add.data.GuoOwner = pUser->charbase.country;
					}
					else
					{
						add.data.dwOwner = 0;
					}
				}
				add.data.dwMapObjectTempID = so->getObject()->data.qwThisID;
				add.data.dwObjectID = so->getObject()->base->id;
				strncpy(add.data.pstrName, so->getObject()->data.strName, MAX_NAMESIZE);
				add.data.x = so->getPos().x;
				add.data.y = so->getPos().y;
				add.data.wdNumber = so->getObject()->data.dwNum;
				add.data.wdLevel = so->getObject()->base->level;
				add.data.upgrade = so->getObject()->data.upgrade;
				add.data.kind = so->getObject()->data.kind;				
				sendCmdToNine(posi, &add, sizeof(add));
				retval = true;
			}
			else
				clearObjectBlock(findedPos);
		}
		if (!retval)
		{
//			so->clear();
			SAFE_DELETE(so);
		}
	}
	return retval;
}
//国家归属
/**
 * \brief 把物品添加到地上，并通知周围9屏的玩家
 * \param ob 需要添加的物品的基本数据
 * \param num 物品数量
 * \param pos 坐标
 * \param dwID 物品的主人
 * \param npc_id  魂魄石对应的npc id
 * \param Protection  物品保护时间开关 false关 true开
 * \param protime 物品保护时间 
 * \return 添加物品是否成功
 */
bool Scene::addObject2(zObjectB *ob, const int num, const zPos &pos,const unsigned long dwID, int npc_id ,bool Protection , int protime)
{
	bool retval = false;
	//设置物品保护时间/秒
	int timetion = protime;
	zObject *o = zObject::create(ob,num);
	if (o)
	{
		//掉落物品也可能产生非白色装备
		//o->found(NULL,true);
		EquipMaker maker(NULL);
	//	maker.assign(NULL, o, o->base,true);
		maker.assign2(NULL, o, o->base,true); //soke 修改BOSS有几率掉落紫装

	    if(Protection == true)	
	    {
            timetion = 30; //设置物品保护时间/秒
	    }
	    else
	    {
	        timetion = 0;   //设置物品保护时间/秒
	    }
		

	    ///if ((npc_id>=21501 && npc_id<=21511) || (npc_id>=26501 && npc_id<=26511) || (npc_id>=30051 && npc_id<=30102))
	    ///{
	    /// protime=30; //设置物品保护时间/秒
	    ///}

		if (addObject2(o, pos, 0, dwID, timetion))
		{
			zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,o->data.dwNum,1,0,NULL,this->id,this->name,"场景生成",o->base,o->data.kind,o->data.upgrade);
			retval = true;
		}
		//removed by lqy, addobject will clean this object auto
/*		
		else
			SAFE_DELETE(o);
*/
	}
	return retval;
}
/**
 * \brief 逆时针转圈
 * \param side 半径
 * \param X 初始坐标X
 * \param Y 初始坐标Y
 * \param CX 生成坐标X
 * \param CY 生成坐标Y
 */
void Scene::runCircle_anti_clockwise(
		const int side,
		const DWORD X,
		const DWORD Y,
		DWORD &CX,
		DWORD &CY) const
{
	if (CX + side == X)
	{
		//左边线上
		if (Y + side == CY)
		{
			//左下顶点
			CX++;
		}
		else
		{
			CY++;
		}
	}
	else if (Y + side == CY)
	{
		//下边线上
		if (X + side == CX)
		{
			//右下顶点
			CY--;
		}
		else
		{
			CX++;
		}
	}
	else if (X + side == CX)
	{
		//右边线上
		if (CY + side == Y)
		{
			//右上顶点
			CX--;
		}
		else
		{
			CY--;
		}
	}
	else if (CY + side == Y)
	{
		//上面线上
		if (CX + side == X)
		{
			//左上顶点
			CY++;
		}
		else
		{
			CX--;
		}
	}
	else
		Zebra::logger->trace("Scene::runCircle_anti_clockwise(): 错误的参数 side=%u, X=%u Y=%u CX=%u CY=%u", side, X, Y, CX, CY);
}

/**
 * \brief 顺时针转圈
 * \param side 半径
 * \param X 初始坐标X
 * \param Y 初始坐标Y
 * \param CX 生成坐标X
 * \param CY 生成坐标Y
 */
void Scene::runCircle_clockwise(
		const int side,
		const DWORD X,
		const DWORD Y,
		DWORD &CX,
		DWORD &CY) const
{
	if (CX + side == X)
	{
		//左边线上
		if (CY + side == Y)
		{
			//左上顶点
			CX++;
		}
		else
		{
			CY--;
		}
	}
	else if (Y + side == CY)
	{
		//下边线上
		if (CX + side == X)
		{
			//左下顶点
			CY--;
		}
		else
		{
			CX--;
		}
	}
	else if (X + side == CX)
	{
		//右边线上
		if (Y + side == CY)
		{
			//右下顶点
			CX--;
		}
		else
		{
			CY++;
		}
	}
	else if (CY + side == Y)
	{
		//上面线上
		if (CX + side == X)
		{
			//右上顶点
			CY++;
		}
		else
		{
			CX++;
		}
	}
	else
		Zebra::logger->trace("Scene::runCircle_clockwise(): 错误的参数 side=%u, X=%u Y=%u CX=%u CY=%u", side, X, Y, CX, CY);
}

/**
 * \brief 找出一个坐标指定方向上的相邻位置
 * \param dir 方向
 * \param orgPos 初始坐标
 * \return 找到的坐标
 */
void Scene::getNextPos(const zPos &orgPos, const int dir, zPos &newPos) const
{
	newPos = orgPos;
	switch (dir)
	{
		case 0:
			newPos.y = newPos.y>1?newPos.y-1:0;
			break;
		case 1:
			newPos.x = newPos.x+1>sceneWH.x?sceneWH.x:newPos.x+1;
			newPos.y = newPos.y>1?newPos.y-1:0;
			break;
		case 2:
			newPos.x = newPos.x+1>sceneWH.x?sceneWH.x:newPos.x+1;
			break;
		case 3:
			newPos.x = newPos.x+1>sceneWH.x?sceneWH.x:newPos.x+1;
			newPos.y = newPos.y+1>sceneWH.y?sceneWH.y:newPos.y+1;
			break;
		case 4:
			newPos.y = newPos.y+1>sceneWH.y?sceneWH.y:newPos.y+1;
			break;
		case 5:
			newPos.x = newPos.x>1?newPos.x-1:0;
			newPos.y = newPos.y+1>sceneWH.y?sceneWH.y:newPos.y+1;
			break;
		case 6:
			newPos.x = newPos.x>1?newPos.x-1:0;
			break;
		case 7:
			newPos.x = newPos.x>1?newPos.x-1:0;
			newPos.y = newPos.y>1?newPos.y-1:0;
			break;
		default:
			break;
	}
}

/**
 * \brief 以side为半径，以orgPos为原点，找地图位置
 * \param side 半径
 * \param direct 方向
 * \param orgPos 初始坐标
 * \param clockwise 顺时针还是逆时针
 * \param crtPos 得到的坐标
 * \return 查找是否成功
 */
bool Scene::getNextPos(
		int &side,
		const int direct,
		const zPos &orgPos,
		const int clockwise,
		zPos &crtPos) const
{
	const int walk_adjust[][2]= { {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, 0} };

	//检查中心点是否超出地图边界
	if (!zPosValidate(orgPos))
	{
		crtPos.x = 0;
		crtPos.y = 0;
		++side; //added by lqy, fix infinite loop bug, see  Scene::findPosForUser please!
		return false;
	}

	do {
		//到终点
		if (side == 0)
		{
			side++;
			crtPos.x = orgPos.x + walk_adjust[direct][0] * side;
			crtPos.y = orgPos.y + walk_adjust[direct][1] * side;
		}
		else
		{
			//转圈
			if (clockwise == 1)
			{
				//逆时针
				runCircle_anti_clockwise(side, orgPos.x, orgPos.y, crtPos.x, crtPos.y);
			}
			else if (clockwise == -1)
			{
				//顺时针
				runCircle_clockwise(side, orgPos.x, orgPos.y, crtPos.x, crtPos.y);
			}
			else
			{
				Zebra::logger->trace("Scene::getNextPos(): 错误的参数 clockwise=%u", clockwise);

				//按照顺时针处理
				runCircle_clockwise(side, orgPos.x, orgPos.y, crtPos.x, crtPos.y);
			}
			if ((crtPos.x == orgPos.x + walk_adjust[direct][0] * side
						&& crtPos.y == orgPos.y + walk_adjust[direct][1] * side))
			{
				//绕半径一周完毕
				side++;
				crtPos.x = orgPos.x + walk_adjust[direct][0] * side;
				crtPos.y = orgPos.y + walk_adjust[direct][1] * side;
			}
		}

		//边界检查
		if ((crtPos.x & 0x80000000) == 0 
				&& (crtPos.y & 0x80000000) == 0
				&& zPosValidate(crtPos))
		{
			break;
		}
		else
		{
			//跳过超出边界的点
			Zebra::logger->error("中心点(%u, %u), 超出边界点(%u, %u)", orgPos.x, orgPos.y, crtPos.x, crtPos.y);
		}
	} while(true);

	return true;
}

/**
 * \brief 更新地图物品的回调函数
 *
 */
struct UpdateSceneObjectCallBack : public zSceneEntryCallBack
{
	/**
	 * \brief 场景指针
	 *
	 */
	Scene *scene;
#ifdef _POOL_ALLOC_	
	typedef std::set<zSceneObject *, std::less<zSceneObject *>, __gnu_cxx::__pool_alloc<zSceneObject *> > set;
#else
	typedef std::set<zSceneObject *, std::less<zSceneObject *> > set;
#endif

	set delObject;
	/**
	 * \brief 构造函数
	 * \param scene 场景
	 */
	UpdateSceneObjectCallBack(Scene *scene) : scene(scene) {};
	/**
	 * \brief 回调函数
	 * \param entry 地图物件，这里全部是地图物品
	 * \return 回调是否成功
	 */
	bool exec(zSceneEntry *entry)
	{
		zSceneObject *so = (zSceneObject *)entry;
		zObject *o = so->getObject();
		if (NULL == o)
		{
			delObject.insert(so);
			return true;
		}
		if(o->base->id != 673 && o->base->id != 674)
		{
			if(so->checkProtectOverdue(SceneTimeTick::currentTime))
			{
				Cmd::stClearObjectOwnerMapScreenUserCmd  ret;
				ret.dwMapObjectTempID=so->id;
				scene->sendCmdToNine(so->getPosI(),&ret,sizeof(ret));
			}
		}
#ifndef _DEBUGLOG
		if (so->checkOverdue(SceneTimeTick::currentTime))
#endif
		{
			delObject.insert(so);
		}

		return true;
	}
	void clearObject()
	{
		Cmd::stRemoveMapObjectMapScreenUserCmd re;
		for(set::iterator it = delObject.begin(); it != delObject.end(); ++it)
		{
			zSceneObject *so = *it;
			zObject *o = so->getObject();
			if (o)
				zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,o->data.dwNum,0,0,NULL,0,NULL,"场景回收",NULL,0,0);
			re.dwMapObjectTempID=so->id;
			scene->sendCmdToNine(so->getPosI(),&re,sizeof(re));

			//实际删除物品
			scene->removeObject(so);
			SAFE_DELETE(so);
		}
	}
};

/**
 * \brief 更新地图上面的物品，例如更新物品保护属性、物品消失等
 *
 */
void Scene::updateSceneObject()
{
	My_FunctionTime_wrapper(__PRETTY_FUNCTION__);
	UpdateSceneObjectCallBack usocb(this);

	execAllOfScene(zSceneEntry::SceneEntry_Object, usocb);
	usocb.clearObject();
}

/**
 * \brief 删除地图物品的回调函数
 */
struct RemoveSceneObjectCallBack : public zSceneEntryCallBack
{
	Scene *scene;
#ifdef _POOL_ALLOC_	
	typedef std::set<zSceneObject *, std::less<zSceneObject *>, __gnu_cxx::__pool_alloc<zSceneObject *> > set;
#else
	typedef std::set<zSceneObject *, std::less<zSceneObject *> > set;
#endif	
	set delObject;
	/**
	 * \brief 构造函数
	 */
	RemoveSceneObjectCallBack(Scene *scene) : scene(scene) {};
	/**
	 * \brief 回调函数
	 * \param entry 地图物件，这里全部是地图物品
	 * \return 回调是否成功
	 */
	bool exec(zSceneEntry *entry)
	{
		zSceneObject *so = (zSceneObject *)entry;
		delObject.insert(so);
		return true;
	}
	void clearObject()
	{
		Cmd::stRemoveMapObjectMapScreenUserCmd re;
		for(set::iterator it = delObject.begin(); it != delObject.end(); ++it)
		{
			zSceneObject *so = *it;
			zObject *o = so->getObject();
			if (o)
				zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,o->data.dwNum,0,0,NULL,0,NULL,"场景回收",NULL,0,0);
			re.dwMapObjectTempID=so->id;
			scene->sendCmdToNine(so->getPosI(),&re,sizeof(re));

			//实际删除物品
			scene->removeObject(so);
			SAFE_DELETE(so);
		}
	}
};

/**
 * \brief 卸载地图时卸载所有地图上得物品
 *
 */
void Scene::removeSceneObjectInOneScene()
{
	RemoveSceneObjectCallBack usocb(this);

	execAllOfScene(zSceneEntry::SceneEntry_Object, usocb);
	usocb.clearObject();
}

/**
 * \brief 保存静态场景
 *
 *
 */
bool StaticScene::save()
{
	return true;
}

/////////////////////////////////////
// 	子类继承
/////////////////////////////////////
/**
 * \brief 构造函数
 *
 */
StaticScene::StaticScene():Scene()
{
}

/**
 * \brief 析构函数
 *
 *
 */
StaticScene::~StaticScene()
{
}
//----------------------------------动态图开始-----------------------------------------------------------------------------

/**
 * \brief 保存动态场景
 *
 *
 */
bool DynamicScene::save()
{
	//TODO  保存到数据库
	return true;
}

/**
 * \brief 构造函数
 *
 *
 */
DynamicScene::DynamicScene():Scene()
{
}

/**
 * \brief 析构函数
 *
 *
 */
DynamicScene::~DynamicScene()
{
	save();
}

//-------------------------------------动态图结束--------------------------------------------------------------------------
/**
 * \brief 往场景添加一个攻城对象
 *
 *
 * \param rush 对象指针
 * \return 是否添加成功
 */
bool Scene::addRush(Rush *rush)
{
	rushList.push_back(rush);
	//Zebra::logger->debug("添加Rush , 场景 %s 有 %u 个Rush", name, rushList.size());
	return true;
}

/**
 * \brief 遍历检查攻城是否结束，结束则删除该对象
 *
 *
 */
void Scene::processRush()
{
	My_FunctionTime_wrapper(__PRETTY_FUNCTION__);
	std::list<Rush *> del;
	for (std::list<Rush *>::iterator it=rushList.begin(); it!=rushList.end(); it++)
	{
		(*it)->process();
		if ((*it)->isEnd())
		{
			del.push_back((*it));
			Zebra::logger->debug("%s 删除Rush %s", name, (*it)->rushName);
		}
	}
	if (!del.empty())
	{
		for (std::list<Rush *>::iterator it=del.begin(); it!=del.end(); it++)
		{
			rushList.remove(*it);
			Rush *ru = *it;
			SAFE_DELETE(ru);
		}
		//Zebra::logger->debug("删除Rush , 场景 %s 有 %u 个Rush", name, rushList.size());
	}

	DWORD sec = SceneTimeTick::currentTime.sec();
	//固定刷怪
	if (fixedRush.id)
	{
		if (sec+30>=fixedRush.nextTime && sec<fixedRush.nextTime)
		{
			if (!rushList.empty())
			{
				Zebra::logger->debug("%s 清除所有怪物攻城", name);
				for (std::list<Rush *>::iterator it=rushList.begin(); it!=rushList.end(); it++)
					(*it)->terminate();
			}
		}
		else if (sec>=fixedRush.nextTime || rushList.empty())
		{
			if (fixedRush.allStart && sec<fixedRush.allStart) return;
			if (fixedRush.allEnd && sec>fixedRush.allEnd) return;
			struct tm tv1;
			time_t timValue = time(NULL);
			zRTime::getLocalTime(tv1, timValue);
			if (fixedRush.weekDay && tv1.tm_wday!=fixedRush.weekDay) return;
			if (fixedRush.startTime.tm_hour)
			{
				if (tv1.tm_hour<fixedRush.startTime.tm_hour) return;
				if (tv1.tm_hour==fixedRush.startTime.tm_hour && tv1.tm_min<fixedRush.startTime.tm_min) return;
			}
			if (fixedRush.endTime.tm_hour)
			{
				if (tv1.tm_hour>fixedRush.endTime.tm_hour) return;
				if (tv1.tm_hour==fixedRush.endTime.tm_hour && tv1.tm_min>fixedRush.endTime.tm_min) return;
			}

			Rush * rush = new Rush(fixedRush.id, fixedRush.delay, countryID);
			if (rush)
			{
				if (rush->init(this))
				{
					fixedRush.nextTime = sec;
					fixedRush.nextTime += rush->rushDelay+rush->lasttime+30;
					Zebra::logger->error("%s 初始化怪物攻城 id=%u countryID=%u delay=%u", name, fixedRush.id, countryID, fixedRush.delay);
					return;
				}
				else
					Zebra::logger->error("%s 初始化怪物攻城失败 id=%u countryID=%u delay=%u", name, fixedRush.id, countryID, fixedRush.delay);

				SAFE_DELETE(rush);
				return;
			}
		}
	}
	/*

	DWORD sec = SceneTimeTick::currentTime.sec();
	//固定刷怪
	if (nextRushTime && sec>=1145592000 && sec<=1146024000)
	{
		if (sec+30>=fixedRush.nextTime && sec<fixedRush.nextTime)
		{
			if (!rushList.empty())
			{
				Zebra::logger->debug("%s 清除所有怪物攻城", name);
				for (std::list<Rush *>::iterator it=rushList.begin(); it!=rushList.end(); it++)
					(*it)->terminate();
			}
		}
		else if (sec>=fixedRush.nextTime || rushList.empty())
		{
			if (fixedRush.startTime && sec<fixedRush.startTime) return;
			if (fixedRush.endTime && sec>fixedRush.endTime) return;
			struct tm tv1;
			time_t timValue = time(NULL);
			zRTime::getLocalTime(tv1, timValue);
			if (fixedRush.startHour && tv1.tm_hour<fixedRush.startHour) return;
			if (fixedRush.endHour && tv1.tm_hour>fixedRush.endHour) return;

			Rush * rush = new Rush(fixedRush.id, 0, countryID);
			if (rush)
			{
				if (rush->init(this))
				{
					fixedRush.nextTime = sec;
					fixedRush.nextTime += rush->lasttime+30;
					return;
				}
				else
					Zebra::logger->error("%s 初始化怪物攻城失败 id=%u countryID=%u", name, atoi(params["fixedRush"].c_str()), countryID);

				SAFE_DELETE(rush);
				return;
			}
		}
	}
	*/
}

bool Scene::checkUserLevel(SceneUser *pUser)
{
	if (!pUser) return true;
	if (this->level >0)
	{
		if (pUser->charbase.level < this->level)
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "%s不对等级低于%d级的玩家开放！",this->name, this->level);
			Zebra::logger->trace("[GOMAP]玩家%s等级不够跳到地图[%s]失败!" , pUser->name, this->name);
			return false;
		}
		//soke 增加地图ID 1111 限制2名玩家
		if (this->getRealMapID() == 1111 && this->countUser()>=2)
		{
			Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "[%s]已经有2名玩家，请稍后再试！",this->name);
			Zebra::logger->trace("[1VS1]地图[%s]已经存在2名玩家，玩家%s跳转失败!" , this->name, pUser->name);
			return false;
		}
	}
	return true;
}


struct SecGenCallback : public zSceneEntryCallBack
{
	SecGenCallback(){}
	/**
	 * \brief 回调函数
	 * \param entry 地图物件，这里全部是npc
	 * \return 回调是否成功
	 */
	bool exec(zSceneEntry *entry)
	{
		SceneNpc *npc = (SceneNpc *)entry;
		if (npc->getState()==zSceneEntry::SceneEntry_Death)
		{
			npc->reliveTime = SceneTimeTick::currentTime;

			Zebra::logger->debug("%s 国战结束，复活禁卫队长(%u,%u)", npc->scene->name, npc->getPos().x, npc->getPos().y);
		}
		return true;
	}
};

void Scene::reliveSecGen()
{
	SecGenCallback c;
	execAllOfScene_npc(COUNTRY_SEC_GEN, c);
}

bool Scene::clone( Scene *fromScn )
{
    if ( !fromScn )
    {
        return false;
    }
    

	//==================================================================================================醉梦修复动态地图
    this->id = fromScn->id;
    strncpy(this->name, fromScn->name, MAX_NAMESIZE);
    
    this->setRunState(SCENE_RUN_STATE_NORMAL);

    this->bossMap = fromScn->bossMap;
    ///当前神龙坛Npc临时ID
    this->loongAltarNpcTmpID = fromScn->loongAltarNpcTmpID;
    ///战胜国经验加成标志 
    this->winner_exp = fromScn->winner_exp;
    /// 国战战场死亡后，攻方死亡复活地
    this->countryDareBackToMapID = fromScn->countryDareBackToMapID;

    //练功点地图
    this->fieldMapName = fromScn->fieldMapName;
    //主城地图
    this->mainMapName = fromScn->mainMapName;

    //增值地图
    this->incMapName = fromScn->incMapName;
    
    this->params.clear();
    this->params = fromScn->params;
    
    ///给npc分组处理
    this->execGroup = fromScn->execGroup;
    /// 帮会夺城战进行标志
    this->isUnionDare = fromScn->isUnionDare;
    /// 该场景所属帮会
    this->dwHoldUnionID = fromScn->dwHoldUnionID;
    /// 该场景占领者国家ID
    this->dwHoldCountryID = fromScn->dwHoldCountryID;

    /// 正式国战正在该场景进行的标志
    this->isCountryFormalDare = fromScn->isCountryFormalDare;
    /// 皇城战正在该场景进行的标志
    this->isEmperorDare = fromScn->isEmperorDare;
    /// 皇城战的守方
    this->dwEmperorDareDef = fromScn->dwEmperorDareDef;
    ///本场景的攻城列表
    this->rushList.clear();
    this->rushList = fromScn->rushList;
    ///一秒定时器
    this->_one_sec = fromScn->_one_sec;
    ///已经初始化
    this->inited = fromScn->inited;
    ///所有的地图格子
    this->allTiles.clear();
    this->allTiles = fromScn->allTiles;

    ///场景中各种区域的定义
    this->zoneTypeDef.clear();
    this->zoneTypeDef = fromScn->zoneTypeDef;

    ///本场景用户数 初始化为0
    this->userCount = 0;
    ///如果本地图没有重生区时需要跳转到的地图
    this->backtoMapID = fromScn->backtoMapID;
    ///回到主城的地图id
    this->backtoCityMapID = fromScn->backtoCityMapID;
    ///回到国战目的地
    this->backtoDareMapID = fromScn->backtoDareMapID;
    /// 外国人死亡后应该回到的地图id(没有国家信息)
    this->foreignerBacktoMapID = fromScn->foreignerBacktoMapID;
    /// 在公共国死亡后应该回到的地图id(没有国家信息)
    this->commonCountryBacktoMapID = fromScn->commonCountryBacktoMapID;
    /// 无国籍人在外国死亡重生地
    this->commonUserBacktoMapID = fromScn->commonUserBacktoMapID;
    /// 国战战场死亡后，守方死亡复活地
    this->countryDefBackToMapID = fromScn->countryDefBackToMapID;
    ///地图特殊说明
    this->function = fromScn->function;
    /// 可相互pk的等级
    this->pklevel = fromScn->pklevel;
    ///国家id
    this->countryID = fromScn->countryID;
    ///场景对应的文件名
    this->fileName = fromScn->fileName;

    ///路点管理器
    this->wpm = fromScn->wpm;
    ///本地图收取税费
    this->countryTax = fromScn->countryTax;
    ///本地图允许进入的最小玩家等级
    this->level = fromScn->level;
    ///场景地图加成
    this->exprate = fromScn->exprate;

    //init SceneEntryIndex
    this->sceneWH = fromScn->sceneWH;
	this->screenx = fromScn->screenx;
	this->screeny = fromScn->screeny;
	this->screenMax = screenx*screeny;
	setSceneWH(sceneWH, screenx, screeny, screenMax);
 
    /// 非阻挡点索引
    this->_index.clear();
    this->_index = fromScn->_index;

    for(ZoneTypeDefVector::iterator it = zoneTypeDef.begin(); it != zoneTypeDef.end(); it++)
    {
        ZoneTypeDef &zone = *it;
        zPosRevaluate(zone.pos);
        initRegion(zone.region, zone.pos, zone.width, zone.height);
    }

    ///所有的npc定义
    this->npcDefine.clear();
    this->npcDefine = fromScn->npcDefine;
    this->scriptMap = fromScn->scriptMap;

	for(NpcDefineVector::iterator it = npcDefine.begin(); it != npcDefine.end(); it++)
	{
		t_NpcDefine *define = &(*it);
		zPosRevaluate(define->pos);
		initRegion(define->region, define->pos, define->width, define->height);
		
		//检查是否有共享脚本
		if ( (0 == define->scriptID) && (scriptMap.end() != scriptMap.find(define->id)) )
		{
			define->scriptID = scriptMap[define->id];
		}
		
		if (!initByNpcDefine(define))
		{
            Zebra::logger->error("初始化NPC(id:%d,name:%s)失败", define->id, define->name);
		}
	}

    //副本相关变量
    this->fbType = fromScn->fbType;
    this->fbMapID = fromScn->fbMapID;
    this->fbStartTime = SceneTimeTick::currentTime.sec();
    this->closeCountDownTime = fromScn->closeCountDownTime;

    return true;
}

/**
 * \brief 把玩家送回复活区
 *
 */
struct KickoffUserCallBack : public zSceneEntryCallBack
{
	
	//场景指针
	Scene *scene;

	KickoffUserCallBack(Scene *scene) : scene(scene) {};
	/**
	 * \brief 回调函数
	 * \param entry 
	 * \return 回调是否成功
	 */
	bool exec(zSceneEntry *entry)
	{
        SceneUser  *pUser = (SceneUser *)entry;
        if ( pUser && pUser->scene && pUser->scene == this->scene )
        {
            if ( scene->isDynamic() )
            {
                Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "%s时间结束，你被传送出了副本", scene->name);
                pUser->reliveReady(NULL, false);
            }
        }
		return true;
	}
};

void Scene::kickoffAllSceneUser()
{
	My_FunctionTime_wrapper(__PRETTY_FUNCTION__);
    KickoffUserCallBack koucb(this);
	execAllOfScene(zSceneEntry::SceneEntry_Player, koucb);
}

void Scene::fbTimer()
{
    if ( !isDynamic() ) return ;
    
    DWORD curTime = SceneTimeTick::currentTime.sec();
    switch ( getRunState() )
    {
    case SCENE_RUN_STATE_NORMAL:
        {
            if ( fbStartTime + FuBenMgr::getMe().getFBLastTime( fbType ) < curTime )
            {
                //kick all user
                if ( countUser() )
                {
                 //   std::ostringstream oss;
                 //   oss << "楚国·王城 pos=320,480";
                 //   SceneUserManager::getMe().removeUserToWhere( this, oss.str().c_str() );
                    kickoffAllSceneUser();
                }
                setRunState( SCENE_RUN_STATE_READY_CLOSE );
            }
        }
        break;
    case SCENE_RUN_STATE_READY_CLOSE:
        {
#ifdef  _WQH_DEBUG

            setRunState( SCENE_RUN_STATE_CLOSE );
#else
            closeCountDownTime -= 60;
            if ( closeCountDownTime <= 0)
            {
                setRunState( SCENE_RUN_STATE_CLOSE );
            }
#endif
        }
        break;
    case SCENE_RUN_STATE_CLOSE:
        {
            //卸载地图
            Cmd::Session::t_unloadScene_SceneSession ret;
            //ret.map_id = SceneManager::getInstance().buildMapID(countryid , mapid);
            ret.map_id = this->id;
            sessionClient->sendCmd(&ret, sizeof(ret));
        }
        break;
    case SCENE_RUN_STATE_UNLOAD:
        {
            //卸载地图
            Cmd::Session::t_unloadScene_SceneSession ret;
            //ret.map_id = SceneManager::getInstance().buildMapID(countryid , mapid);
            ret.map_id = this->id;
            sessionClient->sendCmd(&ret, sizeof(ret));
        }
        break;
    default:
        break;
    }
}
