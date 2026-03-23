/**
 * \file   GameConfigMgrX.h	
 * \brief  会话服务器中负责管理游戏中的各种功能和活动的配置,场景也有一个对应的管理器
 * \author wanqihu
 * \date   2010年7月30日
 * 
 */
#ifndef GAME_CONFIG_MGRX_H_
#define GAME_CONFIG_MGRX_H_



#include "zXMLParser.h"


class GameConfigMgrX
{
public:
    ~GameConfigMgrX();

    static GameConfigMgrX& getMe();

    bool init();

    bool reload();

    void InitWenQuanNode(xmlNodePtr node);
protected:

private:
    GameConfigMgrX();

    static GameConfigMgrX* pInst;
};


#endif


