/**
 * \file   GameConfigMgr.h	
 * \brief  场景服务器中负责管理游戏中的各种功能和活动的配置 
 * \author wanqihu
 * \date   2010年7月30日
 * 
 */
#ifndef GAME_CONFIG_MGR_H_
#define GAME_CONFIG_MGR_H_


#include "zXMLParser.h"


class GameConfigMgr
{
public:
    ~GameConfigMgr();

    static GameConfigMgr& getMe();

    bool init();

    bool Reload();

    void InitWenQuanNode(xmlNodePtr node);
protected:

private:
    GameConfigMgr();

    static GameConfigMgr* pInst;
};







#endif

