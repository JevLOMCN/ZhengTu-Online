/**
 * \file
 * \version  $Id: SceneManager.cpp $
 * \author  
 * \date 
 * \brief SceneManager的实现
 *
 * 
 */

#include "SceneManager.h"
#include "zXMLParser.h"
#include "Zebra.h"
#include "ScenesServer.h"
#include "SessionClient.h"
#include "FuBenMgr.h" //副本
using namespace Cmd::Session;
using namespace Zebra;
///SceneManager的唯一实例
SceneManager *SceneManager::sm(new SceneManager());

/**
 * \brief 生成一个唯一ID
 *
 * \param tempid 输出：取得的ID
 * \return 是否成功
 */
bool SceneManager::getUniqeID(DWORD &tempid)
{
	tempid=sceneUniqeID->get();
	//Zebra::logger->debug("得到scenetempid = %ld",tempid);
	return (tempid!=sceneUniqeID->invalid());
}

/**
 * \brief 释放唯一ID
 *
 * \param tempid 要释放的ID
 */
void SceneManager::putUniqeID(const DWORD &tempid)
{
	sceneUniqeID->put(tempid);
	//Zebra::logger->debug("回收scenetempid = %ld",tempid);
}

/**
 * \brief 构造函数
 *
 */
SceneManager::SceneManager()
{
	inited=false;
	newzone=false; 
}

/**
 * \brief 析构函数
 *
 */
SceneManager::~SceneManager()
{
	final();
}

/**
 * \brief 得到SceneManager的指针
 * 如果指针为0则进行初始化
 *
 */
SceneManager & SceneManager::getInstance()
{
	if(sm==NULL)
		sm=new SceneManager();
		return *sm;
}

/**
 * \brief 删除SceneManager
 *
 */
void SceneManager::delInstance()
{
	if(sm!=NULL)
	{
		sm->final();
		SAFE_DELETE(sm);
	}
}

/**
 * \brief 释放所有已经加载的地图
 *
 */
void SceneManager::final()
{
	if(inited)
	{
		inited=false;
		unloadAllScene();
	}
}

/**
 * \brief 初始化
 * 加载所有地图
 *
 */
bool SceneManager::init()
{
	if(inited) return inited;

	//为每个场景服务器生成不相交叉的场景临时ID分配器,最小的从10000开始,每个有49998个ID可用
	DWORD firstTempID=10000+(ScenesService::getInstance().getServerID()%100)*50000;
	sceneUniqeID=new zUniqueDWORDID(firstTempID,firstTempID+49998);

	// 初始化所有地图
	zXMLParser parser=zXMLParser();
	std::string scenesinfofile=Zebra::global["sceneinfofile"];
	if(scenesinfofile=="")
		scenesinfofile="scenesinfo.xml";
	if(parser.initFile(scenesinfofile.c_str()))
	{
		xmlNodePtr root=parser.getRootNode("ScenesInfo");
		xmlNodePtr countryNode=parser.getChildNode(root,"countryinfo");
		if(countryNode)
		{
			xmlNodePtr subnode = parser.getChildNode(countryNode , "country");
			while(subnode)
			{
				if(strcmp((char*)subnode->name , "country") == 0)
				{
					CountryInfo info;
					bzero(&info , sizeof(info));
					parser.getNodePropNum(subnode , "id" , &info.id , sizeof(info.id));
					parser.getNodePropStr(subnode , "name" , info.name , sizeof(info.name));
					parser.getNodePropNum(subnode , "mapID" , &info.mapid , sizeof(info.mapid));
					parser.getNodePropNum(subnode , "function" , &info.function , sizeof(info.function));
					Zebra::logger->debug("加载国家名称(%u , %s , %u , %u)" , info.id , info.name,info.mapid,info.function);
					country_info.insert(CountryMap_value_type(info.id , info));
				}
				subnode = parser.getNextNode(subnode, NULL);
			}
		}
		xmlNodePtr mapNode=parser.getChildNode(root,"mapinfo");
		if(mapNode)
		{
			xmlNodePtr subnode = parser.getChildNode(mapNode , "map");
			while(subnode)
			{
				if(strcmp((char*)subnode->name , "map") == 0)
				{
					MapInfo info;
					bzero(&info , sizeof(info));
					parser.getNodePropNum(subnode , "mapID" , &info.id , sizeof(info.id));
					parser.getNodePropStr(subnode , "name" , info.name , sizeof(info.name));
					parser.getNodePropStr(subnode , "fileName" , info.filename , sizeof(info.filename));
					parser.getNodePropNum(subnode , "backto" , &info.backto , sizeof(info.backto));
					parser.getNodePropNum(subnode , "backtocity" , &info.backtoCity , sizeof(info.backtoCity));
					parser.getNodePropNum(subnode , "foreignerbackto" , &info.foreignbackto , sizeof(info.foreignbackto));
					parser.getNodePropNum(subnode, "countrydarebackto", &info.countrydarebackto, 
							sizeof(info.countrydarebackto));
					
					parser.getNodePropNum(subnode, "countrydefbackto", &info.countrydefbackto, 
							sizeof(info.countrydefbackto));
					parser.getNodePropNum(subnode, "pklevel", &info.pklevel, 
							sizeof(info.pklevel));



					parser.getNodePropNum(subnode , "commoncountrybackto" , &info.commoncountrybackto , sizeof(info.commoncountrybackto));
					parser.getNodePropNum(subnode , "commonuserbackto" , &info.commonuserbackto , sizeof(info.commonuserbackto));
					parser.getNodePropNum(subnode , "backtodare" , &info.backtodare , sizeof(info.backtodare));
					parser.getNodePropNum(subnode , "function" , &info.function , sizeof(info.function));
					 
					parser.getNodePropNum(subnode , "level" , &info.level , sizeof(info.level));
					parser.getNodePropNum(subnode , "exprate" , &info.exprate , sizeof(info.exprate));
					Zebra::logger->debug("加载地图名称(%u,%s,%s,%u,%u)",
							info.id,info.name, info.filename , info.backto,info.backtoCity);
					map_info.insert(MapMap_value_type(info.id , info));
				}
				subnode = parser.getNextNode(subnode, NULL);
			}
		}
		xmlNodePtr serverNode=parser.getChildNode(root,"server");
		while (serverNode) {
			int id = 0;
			parser.getNodePropNum(serverNode, "id", &id, sizeof(id));
			if (ScenesService::getInstance().getServerID() == id) {
				int mapCount=0;
				xmlNodePtr countryNode=parser.getChildNode(serverNode,"country");
				while(countryNode)
				{
					DWORD countryid=0;
					parser.getNodePropNum(countryNode, "id", &countryid, sizeof(countryid));
					xmlNodePtr mapNode=parser.getChildNode(countryNode,"map");
					while(mapNode)
					{
						//TODO 判断是否可以加载这个地图
						//加载地图
						//std::string s;
						//parser.dump(mapNode,s);
						DWORD mapid=0;
						if(!parser.getNodePropNum(mapNode,"mapID",&mapid,sizeof(mapid)))
						{
							Zebra::logger->error("得到地图编号失败");
							return inited;
						}
						Scene *loaded=loadScene(Scene::STATIC, countryid , mapid);
						// 注册地图
						if(loaded)
						{
							Zebra::logger->info("加载%s(%ld,%ld)成功",loaded->name,loaded->id,loaded->tempid);
							t_regScene_SceneSession regscene;
							regscene.dwID=loaded->id;
							//Zebra::logger->info("[地图真实ID]:%d", loaded->id&0x0FFF);
							regscene.dwTempID=loaded->tempid;
							strncpy(regscene.byName,loaded->name,MAX_NAMESIZE);
							strncpy(regscene.fileName, loaded->getFileName(), MAX_NAMESIZE);
							regscene.dwCountryID = countryid;
							regscene.byLevel = loaded->getLevel();
							sessionClient->sendCmd(&regscene,sizeof(regscene));
							mapCount++;
						}
						else
						{
							return inited;
						}
						mapNode=parser.getNextNode(mapNode,"map");
					}
					countryNode=parser.getNextNode(countryNode,"country");
				}
				Zebra::logger->info("ScenesServer id=%d 加载 %d 张地图。", id, mapCount);
			}
			serverNode=parser.getNextNode(serverNode,"server");
		}
		inited=true;
	}
	else
		Zebra::logger->alarm("SceneManager 解析配置文件失败。 %s", scenesinfofile.c_str());
	//刷新玉如意需要的地图信息
	freshEverySceneField(); 
	return inited;
}

/**
 * \brief 根据名字的到场景指针的回调
 * 方法是遍历所有scene并比较名字
 * 
 */
class GetSceneByFileName:public SceneCallBack
{
	public:
		///找到的scene指针
		Scene *ret;
		///要找的场景名字
		const char *name;

		/**
		 * \brief 构造函数
		 *
		 * 
		 * \param name 要查找的场景名字
		 * \return 
		 */
		GetSceneByFileName(const char *name) : ret(NULL), name(name) {};

		/**
		 * \brief  执行查找的方法
		 *
		 *
		 * \param scene 场景指针
		 * \return 是否继续查找
		 */
		bool exec(Scene *scene)
		{
			if(strncmp(scene->getFileName(),name,MAX_NAMESIZE)==0)
			{
				ret=scene;
				return false;
			}
			else
				return true;
		}
};

/**
 * \brief 根据文件名字找到场景指针
 *
 * \param name 要找的场景名字
 * \return 要找的场景指针，失败返回0
 */
Scene * SceneManager::getSceneByFileName( const char * name)
{
	GetSceneByFileName gsfn(name);
	execEveryScene(gsfn);
	return gsfn.ret;
}

/**
 * \brief 根据名字找到场景指针
 *
 * \param name 要找的场景名字
 * \return 要找的场景指针，失败返回0
 */
Scene * SceneManager::getSceneByName( const char * name)
{
	rwlock.rdlock();
	Scene *ret =(Scene *)getEntryByName(name);
	rwlock.unlock();
	return ret;
}

/**
 * \brief 根据领事ID找到场景指针
 *
 * \param tempid 要找的场景的临时id
 * \return 要找的场景指针，失败返回0
 */
Scene * SceneManager::getSceneByTempID( DWORD tempid)
{
	rwlock.rdlock();
	Scene * ret = (Scene *)getEntryByTempID(tempid);
	rwlock.unlock();
	return ret;
}

/**
 * \brief 根据id找到场景指针
 *
 * \param id 要找的场景id
 * \return 要找的场景指针，失败返回0
 */
Scene * SceneManager::getSceneByID( DWORD id)
{
	rwlock.rdlock();
	Scene * ret = (Scene *)getEntryByID(id);
	rwlock.unlock();
	return ret;
}

/**
 * \brief 加载一个地图
 *
 * \param type 地图类型，静态/动态
 * \param countryid 国家id
 * \param mapid 地图id
 * \param fbType 动态地图情况下，副本类型
 * \param fbID   动态地图情况下，副本序号
 * \return 新加载的场景指针
 */
Scene * SceneManager::loadScene( Scene::SceneType type,DWORD countryid , DWORD mapid, DWORD fbType /*= 0 */ )
{
	zEntry *s=NULL;
	switch(type)
	{
		case Scene::STATIC:
			{
				s=new StaticScene();
			}
			break;
		case Scene::SCENE_TYPE_DYNAMIC:
			{
				s=new DynamicScene();
			}
			break;
		default:
			{ 
				Zebra::logger->error("未知场景类型");
				return false;
			}
	}
    ((Scene *)s)->type = type;
	rwlock.wrlock();
	bool ret=((Scene *)s)->init(countryid , mapid);
	if(ret)
	{
        if( ((Scene *)s)->isDynamic() )
        {
            //此处要产生一个Unique ID,只是为了加入SceneManager
            getUniqeID( s->id );
            //此处要产生一个Unique name,只是为了加入SceneManager
            std::stringstream oss;
            oss << s->name << s->id;
            bzero(s->name, MAX_NAMESIZE);
            strncpy(s->name, oss.str().c_str(), MAX_NAMESIZE);

            ((Scene *)s)->setFBType(fbType);
        }
		ret=addEntry(s);
		if(!ret)
			Zebra::logger->error("添加 %s 失败!",s->name);
	}
	else
		Zebra::logger->error("初始化 %s 失败!",s->name);
	rwlock.unlock();
	if(!ret)
	{
		SAFE_DELETE(s);
	}
	return (Scene *)s;
}


Scene * SceneManager::cloneScene( Scene::SceneType type, DWORD countryID, DWORD mapID, DWORD fbType )
{
     zEntry *s = NULL;
     switch(type)
     {
     case Scene::STATIC: //静态地图
         {
             s = new StaticScene();
         }
         break;
     case Scene::SCENE_TYPE_DYNAMIC: //动态地图
         {
             s = new DynamicScene();
         }
         break;
     default:
         {
             Zebra::logger->error("未知场景类型");
             return false;
         }
     }
    
    Scene  *srcScn = getSceneByID( buildMapID(countryID , mapID) );
    if ( !srcScn )
    {
        Zebra::logger->error("克隆场景没有母版");
        return NULL;
    }
    else
    {
        Scene* dstScn = (Scene *)s;
        dstScn->clone(srcScn);
        dstScn->type = type;

        if( dstScn->isDynamic() )
        {
            //此处要产生一个Unique ID,只是为了加入SceneManager
            getUniqeID( dstScn->id );
            //此处要产生一个Unique name,只是为了加入SceneManager
            std::stringstream oss;
            oss << dstScn->name << dstScn->id;
            bzero(dstScn->name, MAX_NAMESIZE);
            strncpy(dstScn->name, oss.str().c_str(), MAX_NAMESIZE);
            
            dstScn->setFBType(fbType);
        }
    }
    bool isAdd = addEntry(s);
	if( !isAdd )
	{
        Zebra::logger->error("添加 %s 失败!", s->name);
        SAFE_DELETE(s);
        return NULL;
    }
	return (Scene *)s;
}



/**
 * \brief 加载一个地图
 *
 * \param type 地图类型，静态/动态
 * \param countryid 国家id
 * \param mapid 地图id
 * \param fbType 动态地图情况下，副本类型
 * \param fbID   动态地图情况下，副本序号
 * \return 新加载的场景指针
 */
Scene * SceneManager::loadScene2( Scene::SceneType type,DWORD septid,DWORD countryid , DWORD mapid, DWORD fbType /*= 0 */ )
{
	zEntry *s=NULL;
	switch(type)
	{
		case Scene::STATIC:
			{
				s=new StaticScene();
			}
			break;
		case Scene::SCENE_TYPE_DYNAMIC:
			{
				s=new DynamicScene();
			}
			break;
		default:
			{ 
				Zebra::logger->error("未知场景类型");
				return false;
			}
	}
    ((Scene *)s)->type = type;
	rwlock.wrlock();
	bool ret=((Scene *)s)->init(countryid , mapid);
	if(ret)
	{
        if( ((Scene *)s)->isDynamic() )
        {
            //此处要产生一个Unique ID,只是为了加入SceneManager
            getUniqeID( s->id );
            //此处要产生一个Unique name,只是为了加入SceneManager
            std::stringstream oss;
            oss << s->name << septid;
            bzero(s->name, MAX_NAMESIZE);
            strncpy(s->name, oss.str().c_str(), MAX_NAMESIZE);

            ((Scene *)s)->setFBType(fbType);
        }
		ret=addEntry(s);
		if(!ret)
			Zebra::logger->error("添加 %s 失败!",s->name);
	}
	else
		Zebra::logger->error("初始化 %s 失败!",s->name);
	rwlock.unlock();
	if(!ret)
	{
		SAFE_DELETE(s);
	}
	return (Scene *)s;
}


Scene * SceneManager::cloneScene2( Scene::SceneType type,DWORD septid, DWORD countryID, DWORD mapID, DWORD fbType )
{
     zEntry *s = NULL;
     switch(type)
     {
     case Scene::STATIC: //静态地图
         {
             s = new StaticScene();
         }
         break;
     case Scene::SCENE_TYPE_DYNAMIC: //动态地图
         {
             s = new DynamicScene();
         }
         break;
     default:
         {
             Zebra::logger->error("未知场景类型");
             return false;
         }
     }
    
    Scene  *srcScn = getSceneByID( buildMapID(countryID , mapID) );
    if ( !srcScn )
    {
        Zebra::logger->error("克隆场景没有母版");
        return NULL;
    }
    else
    {
        Scene* dstScn = (Scene *)s;
        dstScn->clone(srcScn);
        dstScn->type = type;

        if( dstScn->isDynamic() )
        {
            //此处要产生一个Unique ID,只是为了加入SceneManager
            getUniqeID( dstScn->id );
            //此处要产生一个Unique name,只是为了加入SceneManager
            std::stringstream oss;
            oss << dstScn->name << septid;
            bzero(dstScn->name, MAX_NAMESIZE);
            strncpy(dstScn->name, oss.str().c_str(), MAX_NAMESIZE);
            
            dstScn->setFBType(fbType);
        }
    }
    bool isAdd = addEntry(s);
	if( !isAdd )
	{
        Zebra::logger->error("添加 %s 失败!", s->name);
        SAFE_DELETE(s);
        return NULL;
    }
	return (Scene *)s;
}




/**
 * \brief 加载一个地图
 *
 * \param type 地图类型，静态/动态
 * \param countryid 国家id
 * \param mapid 地图id
 * \param fbType 动态地图情况下，副本类型
 * \param fbID   动态地图情况下，副本序号
 * \return 新加载的场景指针
 */
Scene * SceneManager::loadScene3( Scene::SceneType type,DWORD charid,DWORD countryid , DWORD mapid, DWORD fbType /*= 0 */ )
{
	zEntry *s=NULL;
	switch(type)
	{
		case Scene::STATIC:
			{
				s=new StaticScene();
			}
			break;
		case Scene::SCENE_TYPE_DYNAMIC:
			{
				s=new DynamicScene();
			}
			break;
		default:
			{ 
				Zebra::logger->error("未知场景类型");
				return false;
			}
	}
    ((Scene *)s)->type = type;
	rwlock.wrlock();
	bool ret=((Scene *)s)->init(countryid , mapid);
	if(ret)
	{
        if( ((Scene *)s)->isDynamic() )
        {
            //此处要产生一个Unique ID,只是为了加入SceneManager
            getUniqeID( s->id );
            //此处要产生一个Unique name,只是为了加入SceneManager
            std::stringstream oss;
            oss << s->name << charid;
            bzero(s->name, MAX_NAMESIZE);
            strncpy(s->name, oss.str().c_str(), MAX_NAMESIZE);

            ((Scene *)s)->setFBType(fbType);
        }
		ret=addEntry(s);
		if(!ret)
			Zebra::logger->error("添加 %s 失败!",s->name);
	}
	else
		Zebra::logger->error("初始化 %s 失败!",s->name);
	rwlock.unlock();
	if(!ret)
	{
		SAFE_DELETE(s);
	}
	return (Scene *)s;
}


Scene * SceneManager::cloneScene3( Scene::SceneType type,DWORD charid, DWORD countryID, DWORD mapID, DWORD fbType )
{
     zEntry *s = NULL;
     switch(type)
     {
     case Scene::STATIC: //静态地图
         {
             s = new StaticScene();
         }
         break;
     case Scene::SCENE_TYPE_DYNAMIC: //动态地图
         {
             s = new DynamicScene();
         }
         break;
     default:
         {
             Zebra::logger->error("未知场景类型");
             return false;
         }
     }
    
    Scene  *srcScn = getSceneByID( buildMapID(countryID , mapID) );
    if ( !srcScn )
    {
        Zebra::logger->error("克隆场景没有母版");
        return NULL;
    }
    else
    {
        Scene* dstScn = (Scene *)s;
        dstScn->clone(srcScn);
        dstScn->type = type;

        if( dstScn->isDynamic() )
        {
            //此处要产生一个Unique ID,只是为了加入SceneManager
            getUniqeID( dstScn->id );
            //此处要产生一个Unique name,只是为了加入SceneManager
            std::stringstream oss;
            oss << dstScn->name << charid;
            bzero(dstScn->name, MAX_NAMESIZE);
            strncpy(dstScn->name, oss.str().c_str(), MAX_NAMESIZE);
            
            dstScn->setFBType(fbType);
        }
    }
    bool isAdd = addEntry(s);
	if( !isAdd )
	{
        Zebra::logger->error("添加 %s 失败!", s->name);
        SAFE_DELETE(s);
        return NULL;
    }
	return (Scene *)s;
}

/**
 * \brief 根据名字卸载一张地图
 *
 * \param name 要卸载的地图名字
 */
void SceneManager::unloadScene(std::string &name)
{
	zEntry * ret=NULL;
	rwlock.wrlock();
	if(zEntryName::find(name.c_str(),ret))
		removeEntry(ret);
	SAFE_DELETE(ret);
	rwlock.unlock();
	Zebra::logger->debug(__PRETTY_FUNCTION__);
}

/**
 * \brief 根据场景指针卸载一张地图
 *
 * \param scene 要卸载的地图指针
 */
void SceneManager::unloadScene(Scene * &scene)
{
	if(scene==NULL) return;
	rwlock.wrlock();
    removeEntry((zEntry *)scene);

    SAFE_DELETE(scene);
    rwlock.unlock();
    Zebra::logger->debug(__PRETTY_FUNCTION__);
}

/**
 * \brief 卸载全部地图
 *
 */
void SceneManager::unloadAllScene()
{
	zEntry * ret=NULL;
	rwlock.wrlock();
	while(zEntryName::findOne(ret))
	{
		removeEntry(ret);
		SAFE_DELETE(ret);
	}
	rwlock.unlock();
	Zebra::logger->debug(__PRETTY_FUNCTION__);
}

/**
 * \brief 检查并删除设置为remove的场景
 * 方法是遍历所有场景，如果设置了remove标志，则清除所有npc和物件，然后删除场景
 * 该方法在场景主循环中执行
 *
 */
void SceneManager::checkUnloadOneScene()
{
	for(zEntryTempID::hashmap::iterator it=zEntryTempID::ets.begin();it!=zEntryTempID::ets.end();it++)
	{
		Scene *scene = (Scene *)it->second;
		if(scene->getRunState() == SCENE_RUN_STATE_REMOVE)
		{
			Zebra::logger->debug("卸载场景%s",scene->name);
			SceneNpcManager::getMe().removeNpcInOneScene(scene);
			scene->removeSceneObjectInOneScene();
			unloadScene(scene);
			return ;
		}
	}
}
///////////////////////////////////////////////////////////////////////
struct EveryMapExec : public execEntry<SceneManager::MapInfo>
{
	Scene *_scene;
	EveryMapExec(Scene *s):_scene(s)
	{
	}
	bool exec(SceneManager::MapInfo *info)
	{
		if(info->function & 0x2)
		{
			char buf[MAX_NAMESIZE];
			bzero(buf,sizeof(buf));
			if(SceneManager::getInstance().buildMapName(_scene->getCountryID(),info->name,buf))
			{
				_scene->addMainMapName(buf);
			}
		}
		if(info->function & 0x20)
		{
			char buf[MAX_NAMESIZE];
			bzero(buf,sizeof(buf));
			if(SceneManager::getInstance().buildMapName(_scene->getCountryID(),info->name,buf))
			{
				_scene->addIncMapName(buf);
			}
		}
		return true;
	}
};
void SceneManager::freshEverySceneField()
{
	struct getAllMapExec :public SceneCallBack
	{
		Scene *_old_scene;
		getAllMapExec(Scene *s):_old_scene(s)
		{
		}
		bool exec(Scene *scene)
		{
			//Zebra::logger->debug("%d,%d,%d,%s",scene->getCountryID(),_old_scene->getCountryID(),scene->isMainCity(),scene->getFileName());
			if(/*scene != _old_scene && */_old_scene->isMainCity() && scene->getCountryID() == _old_scene->getCountryID() && scene->isField() && _old_scene->getWayPoint(scene->getFileName()))
			{
				_old_scene->addFieldMapName(scene->name);
			}
			return true;
		}
	};
	struct EverySceneExec :public SceneCallBack
	{
		bool exec(Scene *scene)
		{
			scene->clearMainMapName();
			EveryMapExec exec1(scene); 
			SceneManager::getInstance().execEveryMap(exec1);
			scene->clearFieldMapName();
			getAllMapExec exec(scene);
			SceneManager::getInstance().execEveryScene(exec);
			return true;
		}
	};
	EverySceneExec exec; 
	SceneManager::getInstance().execEveryScene(exec);
}
/**
 * \brief 对每个场景执行回调函数
 *
 * \param callback 要执行的回调函数
 */
void SceneManager::execEveryScene(SceneCallBack &callback)
{
	My_FunctionTime_wrapper(__PRETTY_FUNCTION__);
	for(zEntryTempID::hashmap::iterator it=zEntryTempID::ets.begin();it!=zEntryTempID::ets.end();it++)
	{
		if(!callback.exec((Scene *)it->second))
		{
			//mlock.unlock();
			return;
		}
	}
}


/**
 * \brief 根据国家名字得到国家id
 *
 * \param name 要得到id的国家名字
 * \return 找到的id，失败返回0
 */
DWORD SceneManager::getCountryIDByCountryName(const char *name)
{
	SceneManager::CountryMap_iter country_iter = SceneManager::getInstance().country_info.begin();
	for(; country_iter != SceneManager::getInstance().country_info.end() ; country_iter ++)
	{
		if(strcmp(name , country_iter->second.name) == 0)
		{
			return country_iter->first;
		}
	}
	return 0;
}

/**
 * \brief 对每个国家配置执行回调函数
 *
 * \param callback 要执行的回调函数
 */
/*
void SceneManager::execEveryMap(MapCallBack &callback)
{
	SceneManager::MapMap_iter map_iter = SceneManager::getInstance().map_info.begin();
	for(; map_iter != SceneManager::getInstance().map_info.end() ; map_iter ++)
	{
		callback.exec(map_iter->second);
	}
}
// */
/**
 * \brief 根据国家id得到国家名字
 *
 * \param id 要找的国家id
 * \return 找到的国家名字，失败返回0
 */
const char * SceneManager::getCountryNameByCountryID(DWORD id)
{
	SceneManager::CountryMap_iter country_iter = SceneManager::getInstance().country_info.begin();
	for(; country_iter != SceneManager::getInstance().country_info.end() ; country_iter ++)
	{
		if(country_iter->first == id)
		{
			return country_iter->second.name;
		}
	}
	return 0;
}

/**
 * \brief 根据地图名字得到地图id
 *
 * \param name 要找的地图名字
 * \return 找到的id，失败返回0
 */
DWORD SceneManager::getMapIDByMapName(const char *name)
{
	const char *p = strstr(name, "·");
	if (p)
		p += strlen("·");
	else
		p = name;

	SceneManager::MapMap_iter map_iter = SceneManager::getInstance().map_info.begin();
	for(; map_iter != SceneManager::getInstance().map_info.end() ; map_iter ++)
	{
		if(strcmp(p , map_iter->second.name) == 0)
		{
			return map_iter->first;
		}
	}
	return 0;
}

/**
 * \brief 根据国家id和地图id组成场景name
 *
 */
bool SceneManager::buildMapName(DWORD countryid , DWORD mapid,char *out)
{
	const char *c = getCountryNameByCountryID(countryid);
	const char *m = map_info[mapid].name;
	if(c && m)
	{
		sprintf(out,"%s·%s",c,m);
		return true;
	}
	return false;
}
 

/**
 * \brief 根据国家id和地图name组成场景name
 *
 */
bool SceneManager::buildMapName(DWORD countryid , const char *in ,char *out)
{
	const char *c = getCountryNameByCountryID(countryid);
	if(c && in)
	{
		sprintf(out,"%s·%s",c,in);
		return true;
	}
	return false;
}
/**
 * \brief 根据国家id和地图id组成场景id
 *
 */
DWORD SceneManager::buildMapID(DWORD countryid , DWORD mapid)
{
	return (countryid << 16) + mapid;
}
bool SceneManager::isNewZoneConfig()
{
	return newzone && (!newzon_vec.empty());
}
void SceneManager::setNewZoneConfig(bool type)
{
	newzone=type;
	if(newzone==false)
	{
		newzon_vec.clear();
	}
}
void SceneManager::addNewZonePos(DWORD x,DWORD y)
{
	newzon_vec.push_back(std::make_pair(x,y));
}
SceneManager::NewZoneVec &SceneManager::queryNewZonePos()
{
	return newzon_vec;
}
bool SceneManager::randzPosNewZone(Scene *intoScene,zPos &findedPos)
{
	bool founded=false;
	int i=0;
	while(!founded && i < (int)newzon_vec.size())
	{
		int which = zMisc::randBetween(0, newzon_vec.size() - 1);
		zPos initPos;
		int x = zMisc::randBetween(0, 10);
		int y = zMisc::randBetween(0, 10);
		initPos.x =newzon_vec[which].first + 5 - x;
		initPos.y =newzon_vec[which].second + 5 - y;
		founded = intoScene->findPosForUser(initPos, findedPos);
	}
	return founded;
}
/**
 * \brief 副本场景的回调函数
 */
class FBSceneCallBack : public SceneCallBack
{
public:

    FBSceneCallBack(){}

    bool exec(Scene *scene)
    {
        if( scene && scene->isDynamic() )
        {
            scene->fbTimer();
        }
        return true;
    }
};

void SceneManager::timer()
{
    FBSceneCallBack fbcb;
    execEveryScene( fbcb );
    return ;
}

Scene* SceneManager::openFuBenScene(DWORD charid, DWORD fbType, bool isClone /*= false*/ )
{
    //向会话注册的每一个地图的id,tempid,name都要求是唯一的
    DWORD mapID = FuBenMgr::getMe().getFBMapID(fbType);
    DWORD counID = FuBenMgr::getMe().getFBCounID(fbType);
    DWORD fbCnt = this->getCurOpenFBCount(fbType);

    Scene *scn = NULL;
    if ( isClone )
    {
        scn = this->cloneScene3(Scene::SCENE_TYPE_DYNAMIC, charid,counID, mapID, fbType);
    }
    else
    {
        scn = this->loadScene3(Scene::SCENE_TYPE_DYNAMIC, charid,counID, mapID, fbType);
    }

    if ( scn && scn->isDynamic() )
    {
        scn->freshGateMapIndex();

        using namespace Cmd::Session;
        t_regScene_SceneSession regscene;
        regscene.dwID = scn->id;
        regscene.dwTempID = scn->tempid;
        strncpy(regscene.byName,scn->name,MAX_NAMESIZE);
        strncpy(regscene.fileName, scn->getFileName(), MAX_NAMESIZE);
        regscene.dwCountryID = counID;
        regscene.byLevel = scn->getLevel();
        sessionClient->sendCmd(&regscene,sizeof(regscene));
        Zebra::logger->info("[副本]新开启 %s (mapID：%d,countryID：%d),目前同类型副本个数%d", scn->name, mapID, counID, fbCnt);

        return scn;
    }
    return NULL;
}

//家族地图

Scene* SceneManager::openJiaZuScene(DWORD septid, DWORD fbType, bool isClone /*= false*/ )
{
    //向会话注册的每一个地图的id,tempid,name都要求是唯一的
    DWORD mapID = FuBenMgr::getMe().getFBMapID(fbType);
    DWORD counID = FuBenMgr::getMe().getFBCounID(fbType);
    DWORD fbCnt = this->getCurOpenFBCount(fbType);

    Scene *scn = NULL;
    if ( isClone )
    {
        scn = this->cloneScene2(Scene::SCENE_TYPE_DYNAMIC, septid,counID, mapID, fbType);
    }
    else
    {
        scn = this->loadScene2(Scene::SCENE_TYPE_DYNAMIC, septid,counID, mapID, fbType);
    }

    if ( scn && scn->isDynamic() )
    {
        scn->freshGateMapIndex();

        using namespace Cmd::Session;
        t_regScene_SceneSession regscene;
        regscene.dwID = scn->id;
        regscene.dwTempID = scn->tempid;
        strncpy(regscene.byName,scn->name,MAX_NAMESIZE);
        strncpy(regscene.fileName, scn->getFileName(), MAX_NAMESIZE);
        regscene.dwCountryID = counID;
        regscene.byLevel = scn->getLevel();
        sessionClient->sendCmd(&regscene,sizeof(regscene));
      

        return scn;
    }
    return NULL;
}

DWORD SceneManager::getCurOpenFBCount( DWORD fbType )
{
    struct GetFBCountCallBack : public SceneCallBack
    {
        GetFBCountCallBack(DWORD fbType)
        {
            this->count = 0;
            this->fbType = fbType;
        }

        bool exec(Scene *scene)
        {
            if( scene && scene->isDynamic() && fbType && fbType == scene->getFBType())
            {
                count++;
            }
            return true;
        }
        DWORD fbType;
        DWORD count;
    };
    
    GetFBCountCallBack getFBCnt(fbType);
    execEveryScene( getFBCnt );
    return getFBCnt.count;
}



