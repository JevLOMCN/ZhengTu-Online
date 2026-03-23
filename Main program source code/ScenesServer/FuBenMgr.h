/**
 * \file   FuBenMgr.h	
 * \brief  副本管理器  管理副本配置文件
 * \author wanqihu
 * \date   2010年9月3日
 * 
 */
#ifndef FUBEN_MGR_H
#define FUBEN_MGR_H

#include "zType.h"
#include "zXMLParser.h"
#include "Scene.h"
#include "SessionCommand.h"

#include <map>
#include <vector>
#include <list>

class SceneUser;

///副本配置信息
struct FuBenCfgInfo
{
    DWORD    fbID;                      //副本ID
    DWORD    fbType;		            //副本类型 一种副本对应一种类型
    DWORD    fbMapID;                   //副本对应的mapID
    DWORD    fbCounID;                  //副本对应的CounID 国家ID 区或线
    char     fbName[MAX_NAMESIZE];      //副本的名字 32字节，约8个汉字
    DWORD    startTime;                 //开放时间 ,格式： 20100905 12:00:00
    DWORD    lastTime;                  //持续时间 ，单位：秒。 默认两个小时。
    DWORD    maxOpenNumPerDay;          //每天可开启的最大次数。默认为3次。
    DWORD    maxEnterNumPerDay;         //每天可进入的最大次数。默认为3次。
    DWORD    maxUserCount;              //最大用户数量
    DWORD    maxFBCount;                //最大副本数量，同时可开启的最大副本数量，默认40.
    DWORD    isNeedTeam;                //是否需要组队前往 0,无需组队。 1，需要组队
    DWORD    minTeamMembNum;            //队伍成员数量下限 
    DWORD    userMinLvLimit;            //玩家最低等级限制
    DWORD    userMaxLvLimit;            //玩家最高等级限制
};


class FuBenMgr
{
public:
    ~FuBenMgr();

    static FuBenMgr& getMe();
    bool init();
    DWORD getFBMapID(DWORD fbType);
    DWORD getFBCounID(DWORD fbType);
    DWORD getFBLastTime(DWORD fbType);
    DWORD getMaxEnterCount(DWORD fbType);
    bool  checkNeedTeam(DWORD fbType);

    ///检查副本有效性 单服副本使用
    bool checkValid(SceneUser *pUser, DWORD fbType, Cmd::FBActionType actType);

    ///检查副本有效性 跨服副本使用
    bool checkUserValid(SceneUser *pUser, DWORD fbType, Cmd::FBActionType actType);

    bool checkLeaderInFB(SceneUser *pUser, DWORD fbType);
    
    bool reqOpenFuBen(SceneUser *pUser, DWORD fbType);
    bool doReqOpenFuBen(Cmd::Session::Req_OpenEnterFuBen_SceneSession *rev);
    bool doReplyOpenFuBen(Cmd::Session::Reply_OpenEnterFuBen_SceneSession *rev);

    bool reqEnterFuBen(SceneUser *pUser, DWORD fbType);
    bool doReqEnterFuBen(Cmd::Session::Req_OpenEnterFuBen_SceneSession *rev);
    bool doReplyEnterFuBen(Cmd::Session::Reply_OpenEnterFuBen_SceneSession *rev);

    //家族地图
    bool reqOpenJiaZu(SceneUser *pUser, DWORD fbType);
    bool doReqOpenJiaZu(Cmd::Session::Req_OpenEnterJiaZu_SceneSession *rev);
    bool doReplyEnterJiaZu(Cmd::Session::Reply_OpenEnterJiaZu_SceneSession *rev);
private:
    FuBenMgr();
    static FuBenMgr* pInst;

    typedef std::map<DWORD, FuBenCfgInfo>              FuBenCfgMap;        //<type, >
    typedef std::map<DWORD, FuBenCfgInfo>::iterator    FuBenCfgMapIt;
    ///副本配置列表
    FuBenCfgMap    fubenCfgList;
};

#endif

