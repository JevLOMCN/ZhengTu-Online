#ifndef _SCENEMANAGER_H_
#define _SCENEMANAGER_H_

#include "Scene.h"
#include "zSceneManager.h"
#include "zUniqueID.h"

/**
 * \brief 对每个scene执行的回调基类
 *
 */
struct SceneCallBack
{
	virtual bool exec(Scene *scene)=0;
	virtual ~SceneCallBack(){};
};

/**
 * \brief 场景管理器
 *
 */
class SceneManager:public zSceneManager
{
	public:
		typedef std::vector<std::pair<DWORD,DWORD> > NewZoneVec;
		typedef NewZoneVec::iterator newZoneVec_iter;
	private:
		///唯一实例
		static SceneManager * sm;
		///唯一ID分配器
		zUniqueDWORDID *sceneUniqeID;
		///是否已初始化
		bool inited;

		SceneManager();
		~SceneManager();

		bool getUniqeID(DWORD &tempid);
		void putUniqeID(const DWORD &tempid);
		bool newzone;
		NewZoneVec newzon_vec; 

	public:
		static SceneManager &getInstance();
		static void delInstance();
		bool init();
		void final();

		NewZoneVec &queryNewZonePos();
		bool isNewZoneConfig();
		bool randzPosNewZone(Scene *intoScene,zPos &findedPos);
		void addNewZonePos(DWORD x,DWORD y); 
		void setNewZoneConfig(bool type);
		Scene * getSceneByFileName( const char * name);
		Scene * getSceneByName( const char * name);
		Scene * getSceneByTempID( DWORD tempid);
		Scene * getSceneByID( DWORD id);
        Scene * loadScene(Scene::SceneType type,DWORD countryid , DWORD mapid, DWORD fbType = 0 ); //副本
        /**
        * \brief  克隆一个场景，用在多个相同的副本生成 
        */
        Scene * cloneScene(Scene::SceneType type, DWORD countryID, DWORD mapID, DWORD fbType); //副本

		Scene * loadScene2(Scene::SceneType type,DWORD septid,DWORD countryid , DWORD mapid, DWORD fbType = 0 ); //家族地图
        /**
        * \brief  克隆一个场景，用在多个相同的副本生成 
        */
        Scene * cloneScene2(Scene::SceneType type,DWORD septid, DWORD countryID, DWORD mapID, DWORD fbType); //家族地图

		Scene * loadScene3(Scene::SceneType type,DWORD charid,DWORD countryid , DWORD mapid, DWORD fbType = 0 ); //家族地图
        /**
        * \brief  克隆一个场景，用在多个相同的副本生成 
        */
        Scene * cloneScene3(Scene::SceneType type,DWORD charid, DWORD countryID, DWORD mapID, DWORD fbType); //家族地图

		void unloadScene(std::string &name);
		void unloadScene(Scene * &scene);
		void unloadAllScene();

		void freshEverySceneField();
		void execEveryScene(SceneCallBack &callback);
		//void execEveryMap(MapCallBack &callback);
		template <class YourEntry>
		void execEveryMap(execEntry<YourEntry> &callback)
		{
			SceneManager::MapMap_iter map_iter = SceneManager::getInstance().map_info.begin();
			for(; map_iter != SceneManager::getInstance().map_info.end() ; map_iter ++)
			{
				callback.exec(&map_iter->second);
			}
		}
		/*
		template <class YourEntry>
		void execEveryCountryMap(DWORD country_id , execEntry<YourEntry> &exec)
		{
			for(CountryMap_iter iter = country_info.begin() ; iter != country_info.end() ; iter ++)
			{
				MapMap_value_type iter_map = map_info.find(iter->second.mapid);
				if(iter_map != map_info.end())
				{
					exec.exec(iter_map->second);
				}
			}
		}
		// */
		void checkUnloadOneScene();

		/**
		 * \brief 国家信息
		 *
		 */
		struct CountryInfo
		{
			///编号
			DWORD id;
			///名字
			char name[MAX_NAMESIZE];
			///所在的地图名字
			DWORD mapid;
			///国家功能标识字段
			DWORD function;
		};
		/**
		 * \brief 地图信息
		 *
		 */
		struct MapInfo
		{
			///编号
			DWORD id;
			///名字
			char name[MAX_NAMESIZE];
			///对应的地图文件名
			char filename[MAX_NAMESIZE];
			///玩家在该地图死后回到的地图
			DWORD backto;
			///玩家在该地图死后回到的城市
			DWORD backtoCity;
			///玩家在外国地图死后回到的城市
			DWORD foreignbackto;
			///玩家在公共国地图死后回到的城市
			DWORD commoncountrybackto;
			///无国家人在外国地图死后回到的城市
			DWORD commonuserbackto;
			///国战目的地
			DWORD backtodare;
			///国战期间，在国战战场（目前在王城）死亡后，攻方复活城市
			DWORD countrydarebackto;
			///国战期间，在国战战场（目前在王城）死亡后, 守方复活城市
			DWORD countrydefbackto;
			///可相互pk的等级
			DWORD pklevel;
			///表示该地图某些功能是否可用，骑马等
			DWORD function;
			///表示该地图允许的level级别的玩家进入
			BYTE level;
			///表示该地图的经验加成(使用时/100求出比率)
			BYTE exprate;
		};
		typedef std::map<DWORD , CountryInfo> CountryMap;
		typedef CountryMap::iterator CountryMap_iter;
		typedef CountryMap::value_type CountryMap_value_type;
		///国家信息和ID的映射
		CountryMap country_info;
		DWORD getCountryIDByCountryName(const char *name);
		const char * getCountryNameByCountryID(DWORD);
		DWORD getMapIDByMapName(const char *name);
		const char *getMapMapNameByMapID(DWORD mapid);
		DWORD buildMapID(DWORD countryid , DWORD mapid);
		bool buildMapName(DWORD countryid , const char *in ,char *out);
		bool buildMapName(DWORD countryid, DWORD mapid, char *out);
		typedef std::map<DWORD , MapInfo> MapMap;
		typedef MapMap::value_type MapMap_value_type;
		typedef MapMap::iterator MapMap_iter;
		///地图信息和ID的映射
		MapMap map_info;
        ///获取当前开启的副本数目 
        DWORD getCurOpenFBCount(DWORD fbType); //副本   
        /**
        * \brief 开启新的副本场景 
        * \param isClone 是否克隆现有的母版场景
        */
        Scene* openFuBenScene(DWORD cahrid,DWORD fbType, bool isClone = false); //副本

		/**
        * \brief 开启新的家族地图
        * \param isClone 是否克隆现有的母版场景
        */
        Scene* openJiaZuScene(DWORD septid,DWORD fbType, bool isClone = false); //副本
 
        void timer();
};
#endif
