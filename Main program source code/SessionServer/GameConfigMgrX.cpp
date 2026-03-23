#include "GameConfigMgrX.h"
#include "Zebra.h"

#include <iostream>

GameConfigMgrX* GameConfigMgrX::pInst = NULL;




GameConfigMgrX::GameConfigMgrX()
{


}


GameConfigMgrX::~GameConfigMgrX()
{
    if ( pInst )
    {
        delete pInst;
    }
}

GameConfigMgrX& GameConfigMgrX::getMe()
{
    if(NULL == pInst)
    {
        pInst = new GameConfigMgrX();
    }
    return *pInst;
}

bool GameConfigMgrX::init()
{
    zXMLParser xml;
    std::string cfgFile("Config/GameConfig.xml");
    if ( !xml.initFile(cfgFile.c_str()) )
    {
        Zebra::logger->error("不能读取会话服务器游戏配置文件 %s", cfgFile.c_str() );
        return false;
    }

    xmlNodePtr root = xml.getRootNode("Config");
    if( !root ) 
    {
        Zebra::logger->warn("加载会话服务器游戏配置%s失败", cfgFile.c_str() );
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
	Zebra::logger->info("初始化会话服务器游戏配置成功");
    return true;
}

void GameConfigMgrX::InitWenQuanNode( xmlNodePtr node )
{


    return ;
}










