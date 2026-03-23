
#include "GameConfigMgr.h"
#include "Zebra.h"

#include <iostream>

GameConfigMgr* GameConfigMgr::pInst = NULL;




GameConfigMgr::GameConfigMgr()
{


}


GameConfigMgr::~GameConfigMgr()
{
    if ( pInst )
    {
        delete pInst;
    }
}

GameConfigMgr& GameConfigMgr::getMe()
{
    if(NULL == pInst)
    {
        pInst = new GameConfigMgr();
    }
    return *pInst;
}

bool GameConfigMgr::init()
{
    zXMLParser xml;
    std::string cfgFile("Config/GameConfig.xml");
    if ( !xml.initFile(cfgFile.c_str()) )
    {
        Zebra::logger->error("不能读取场景服务器游戏配置文件 %s", cfgFile.c_str() );
        return false;
    }

    xmlNodePtr root = xml.getRootNode("Config");
    if( !root ) 
    {
        Zebra::logger->warn("加载场景服务器游戏配置%s失败", cfgFile.c_str() );
        return false;
    }

    xmlNodePtr node = xml.getChildNode(root, NULL);
    //每增加一个功能，按下面的结构解析结点
    while(node)
    {
        if ( 0 == strcmp((char *)node->name, "wenquan") )
        {
            InitWenQuanNode(node);
        }
        else if (strcmp((char *)node->name, "npc") == 0)
        {
        }
        else if (strcmp((char *)node->name, "waypoint") == 0)
        {
        }
        else if (strcmp((char *)node->name, "fixedrush") == 0)
        {
        }
        node = xml.getNextNode(node, NULL);
    }


	Zebra::logger->info("初始化场景服务器游戏配置成功");
    return true;
}

void GameConfigMgr::InitWenQuanNode( xmlNodePtr node )
{


    return ;
}


