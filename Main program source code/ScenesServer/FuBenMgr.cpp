#include "FuBenMgr.h"
#include "Zebra.h"

#include <iostream>
#include <sstream>

#include "SceneUser.h"
#include "SceneManager.h"
#include "Scene.h"
#include "SessionClient.h"
#include "TimeTick.h"
#include "SceneUserManager.h"
#include "SessionClient.h"
#include "ScenesServer.h"

FuBenMgr* FuBenMgr::pInst = NULL;


FuBenMgr::FuBenMgr()
{
}

FuBenMgr::~FuBenMgr()
{
    if ( pInst )
    {
        delete pInst;
    }
}

FuBenMgr& FuBenMgr::getMe()
{
    if(NULL == pInst)
    {
        pInst = new FuBenMgr();
    }
    return *pInst;
}

bool FuBenMgr::init()
{
    fubenCfgList.clear();
    zXMLParser xml;
    std::string cfgFile("Config/FuBenConfig.xml");
    if ( !xml.initFile(cfgFile.c_str()) )
    {
        Zebra::logger->error("无法读取副本配置文件 %s", cfgFile.c_str() );
        return false;
    }

    xmlNodePtr root = xml.getRootNode("Config");
    if( !root ) 
    {
        Zebra::logger->error("加载副本配置%s失败", cfgFile.c_str() );
        return false;
    }

    xmlNodePtr fbNode = xml.getChildNode(root, "FuBen");
    //每增加一个功能，按下面的结构解析结点
    while(fbNode)
    {
        if(strcmp((char*)fbNode->name , "FuBen") == 0)
        {
            FuBenCfgInfo info;
            bzero(&info , sizeof(info));
            xml.getNodePropNum(fbNode , "FBID" , &info.fbID , sizeof(info.fbID));
            xml.getNodePropNum(fbNode , "FBType" , &info.fbType , sizeof(info.fbType));
            xml.getNodePropNum(fbNode , "FBMapID" , &info.fbMapID , sizeof(info.fbMapID));
            xml.getNodePropNum(fbNode , "FBCounID" , &info.fbCounID , sizeof(info.fbCounID));
            xml.getNodePropStr(fbNode , "FBName" , &info.fbName , sizeof(info.fbName));
            xml.getNodePropTime(fbNode, "StartTime", info.startTime);
            xml.getNodePropNum(fbNode , "LastTime" , &info.lastTime , sizeof(info.lastTime));
            xml.getNodePropNum(fbNode , "MaxOpenNumPerDay" , &info.maxOpenNumPerDay , sizeof(info.maxOpenNumPerDay));
            xml.getNodePropNum(fbNode , "MaxEnterNumPerDay" , &info.maxEnterNumPerDay , sizeof(info.maxEnterNumPerDay));
            xml.getNodePropNum(fbNode , "MaxUserCount" , &info.maxUserCount , sizeof(info.maxUserCount));
            xml.getNodePropNum(fbNode , "MaxFBCount", &info.maxFBCount, sizeof(info.maxFBCount));
            xml.getNodePropNum(fbNode , "IsNeedTeam", &info.isNeedTeam, sizeof(info.isNeedTeam));
            xml.getNodePropNum(fbNode , "MinTeamMembNum", &info.minTeamMembNum, sizeof(info.minTeamMembNum));
            xml.getNodePropNum(fbNode , "UserMinLvLimit", &info.userMinLvLimit, sizeof(info.userMinLvLimit));
            xml.getNodePropNum(fbNode , "UserMaxLvLimit", &info.userMaxLvLimit, sizeof(info.userMaxLvLimit));

            Zebra::logger->debug("加载副本配置(ID:%u,Type:%u,MapID:%d)",  info.fbID, info.fbType, info.fbMapID);
            fubenCfgList[info.fbID] = info;
        }
        fbNode = xml.getNextNode(fbNode, "FuBen");
    }
	Zebra::logger->info("加载场景服务器副本配置成功");
    return true;
}


DWORD FuBenMgr::getFBMapID( DWORD fbType )
{
    DWORD mapID = 0;
    FuBenCfgMapIt it = fubenCfgList.find(fbType);
    if ( it != fubenCfgList.end() )
    {
        mapID = it->second.fbMapID;
    }
    return mapID;
}

DWORD FuBenMgr::getFBCounID( DWORD fbType )
{
    DWORD counID = 0;
    FuBenCfgMapIt it = fubenCfgList.find(fbType);
    if ( it != fubenCfgList.end() )
    {
        counID = it->second.fbCounID;
    }
    return counID;
}

DWORD FuBenMgr::getFBLastTime( DWORD fbType )
{
    DWORD lastTime = 0;
    FuBenCfgMapIt it = fubenCfgList.find(fbType);
    if ( it != fubenCfgList.end() )
    {
        lastTime = it->second.lastTime;
    }
    return lastTime;
}

DWORD FuBenMgr::getMaxEnterCount( DWORD fbType )
{
    DWORD enterCnt = 0;
    FuBenCfgMapIt it = fubenCfgList.find(fbType);
    if ( it != fubenCfgList.end() )
    {
        enterCnt = it->second.maxEnterNumPerDay;
    }
    return enterCnt;
}

bool FuBenMgr::checkNeedTeam( DWORD fbType )
{
    FuBenCfgMapIt it = fubenCfgList.find(fbType);
    if ( it != fubenCfgList.end() )
    {
        return it->second.isNeedTeam;
    }
    return false;
}

bool FuBenMgr::checkValid( SceneUser *pUser, DWORD fbType, Cmd::FBActionType actType )
{
    if ( !pUser || !fbType || !actType )
    {
        return false;
    }
    
    if ( pUser->getState() == SceneUser::SceneEntry_Death )
    {
        return false;
    }

    if (pUser->guard)
    {
        Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你还有护送任务，完成后再来吧");
        return false;
    }

    if( pUser->tradeorder.hasBegin() || pUser->privatestore.step() ) 
    {
        Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "交易或摆摊中无法开启或进入副本");
        return true;
    }

    FuBenCfgMapIt it = fubenCfgList.find(fbType);
    if ( it == fubenCfgList.end() )
    {
        Zebra::logger->error("[副本系统]配置文件中没有%d类型的副本",  fbType);
        return false;
    }
    else
    {
        DWORD curTime = SceneTimeTick::currentTime.sec();
        if ( curTime < it->second.startTime )
        {
            Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "副本尚未到开放时间，请留意官方公告");
            return false;
        }
        
        if ( pUser->charbase.level < it->second.userMinLvLimit || pUser->charbase.level > it->second.userMaxLvLimit )
        {
            Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你的等级不符");
            return false;
        }

        if ( it->second.isNeedTeam )
        {
            if ( !pUser->team.IsTeamed() )
            {
                Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "%s副本需要组队前往", it->second.fbName );
                return false;
            }

            if ( Cmd::FB_ACTION_TYPE_OPEN == actType )
            {
                if ( ! pUser->isTeamLeader() )
                {
                    Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "%s副本需要队长才能开启",it->second.fbName);
                    return false;
                }

                if ( static_cast<DWORD>(pUser->team.getSize()) < it->second.minTeamMembNum )
                {
                    Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "%s副本需要队伍中至少有%个人才能开启",it->second.minTeamMembNum);
                    return false;
                }
            }
        }

        if ( Cmd::FB_ACTION_TYPE_OPEN == actType )
        {
            if ( SceneManager::getInstance().getCurOpenFBCount(fbType) > it->second.maxFBCount )
            {
                Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "当前副本数量已经达到最大，请稍后再来");
                return false;
            }

            if ( pUser->getFBOpenCount(fbType) > it->second.maxOpenNumPerDay )
            {
                Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "你今天开启副本次数已经达到最大,无法开启");
                return false;
            }
        }
        else if (  Cmd::FB_ACTION_TYPE_ENTER == actType)
        {
            if ( pUser->getFBEnterCount(fbType) > it->second.maxEnterNumPerDay )
            {
                Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "你今天进入副本次数已经达到最大, 无法进入");
                return false;
            }
        }
    }
    return true;
}


bool FuBenMgr::checkUserValid( SceneUser *pUser, DWORD fbType, Cmd::FBActionType actType )
{
    if ( !pUser || !fbType || !actType )
    {
        return false;
    }
    
    if ( pUser->getState() == SceneUser::SceneEntry_Death )
    {
        return false;
    }

    if (pUser->guard)
    {
        Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你还有护送任务，完成后再来吧");
        return false;
    }

    if( pUser->tradeorder.hasBegin() || pUser->privatestore.step() ) 
    {
        Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "交易或摆摊中无法开启或进入副本");
        return true;
    }

    FuBenCfgMapIt it = fubenCfgList.find(fbType);
    if ( it == fubenCfgList.end() )
    {
        Zebra::logger->error("[副本系统]配置文件中没有%d类型的副本",  fbType);
        return false;
    }
    else
    {
        DWORD curTime = SceneTimeTick::currentTime.sec();
        if ( curTime < it->second.startTime )
        {
            Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "副本尚未到开放时间，请留意官方公告");
            return false;
        }

        if ( pUser->charbase.level < it->second.userMinLvLimit || pUser->charbase.level > it->second.userMaxLvLimit )
        {
            Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你的等级不符");
            return false;
        }

        //仅开启才判断是否组队
        if ( Cmd::FB_ACTION_TYPE_OPEN == actType )
        {
            if ( it->second.isNeedTeam )
            {
                if ( !pUser->team.IsTeamed() )
                {
                    Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "%s副本需要组队前往", it->second.fbName );
                    return false;
                }

                if ( ! pUser->isTeamLeader() )
                {
                    Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "%s副本需要队长才能开启",it->second.fbName);
                    return false;
                }

                if ( static_cast<DWORD>(pUser->team.getSize()) < it->second.minTeamMembNum )
                {
                    Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "%s副本需要队伍中至少有%个人才能开启",it->second.minTeamMembNum);
                    return false;
                }
            }

            if ( pUser->getFBOpenCount(fbType) > it->second.maxOpenNumPerDay )
            {
                Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "你今天开启副本次数已经达到最大,无法开启");
                return false;
            }
        }
        else if (  Cmd::FB_ACTION_TYPE_ENTER == actType)
        {
            if ( pUser->getFBEnterCount(fbType) > it->second.maxEnterNumPerDay )
            {
                Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "你今天进入副本次数已经达到最大, 无法进入");
                return false;
            }
        }
    }
    return true;
}


bool FuBenMgr::checkLeaderInFB( SceneUser *pUser, DWORD fbType )
{
    if( !pUser || !pUser->scene ) return false;

    SceneUser *leader = SceneUserManager::getMe().getUserByTempID(pUser->team.getLeader());
    if( leader && leader->scene && pUser->id != leader->id && pUser->scene != leader->scene ) 
    {
        if ( leader->scene->isDynamic() )
        {
            Scene *toScene = leader->scene;
            if( !toScene ) return false;
        
            if ( fbType == toScene->getFBType() )
            {
                return true;
            }
        }
    }
    return false;
}

bool FuBenMgr::reqOpenFuBen( SceneUser *pUser, DWORD fbType )
{
    bool canOpen = FuBenMgr::getMe().checkUserValid(pUser, fbType, Cmd::FB_ACTION_TYPE_OPEN);
    if ( !canOpen )
    {
        Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "条件不符，无法开启副本");
        return false;
    }
    
    DWORD fbScnID = 1;
    DWORD openCnt = 0;
    DWORD enterCnt = 0;
    pUser->getFuBenInfo(fbType, fbScnID, openCnt, enterCnt);

    using namespace  Cmd::Session;
    Req_OpenEnterFuBen_SceneSession send;
    send.actType = Cmd::FB_ACTION_TYPE_OPEN;
    send.userID = pUser->id;
    send.reqSvrID = ScenesService::getInstance().getServerID();
    send.fbType = fbType;
    send.lastFBScnID = fbScnID;
    send.counID = getFBCounID(fbType);
    send.mapID = getFBMapID(fbType);
    sessionClient->sendCmd(&send, sizeof(send));
    return true;
}

bool FuBenMgr::doReqOpenFuBen( Cmd::Session::Req_OpenEnterFuBen_SceneSession *rev )
{
    if ( !rev )
    {
        return false;
    }
    
    Scene *findScn = SceneManager::getInstance().getSceneByID( (rev->counID << 16) + rev->mapID );
    if ( !findScn ) return false;

    DWORD fbType = rev->fbType;

    using namespace Cmd::Session;
    Reply_OpenEnterFuBen_SceneSession send;
    send.actType = Cmd::FB_ACTION_TYPE_OPEN;
    send.userID = rev->userID;
    send.fbType = fbType;
    send.reqSvrID = rev->reqSvrID;
    send.replySvrID = ScenesService::getInstance().getServerID();
        
    Scene *fbScn = NULL;
    char fbName[64];
    sprintf(fbName, "中立区·云天别墅%d", rev->userID);
    Scene *toScn  = SceneManager::getInstance().getSceneByName(fbName);
    if ( !toScn )
    { 
        fbScn = SceneManager::getInstance().openFuBenScene(rev->userID,fbType,true);
        if ( !fbScn )
        {
            send.retVal = REQFB_NO_SYS_BUSY;
        }
        else
        {
            send.retVal = REQFB_YES_OPEN_NEW;
        }
    }
    else if (toScn->isRunning() && fbType == toScn->getFBType())
    {
        send.retVal = REQFB_EXIST_RUN;
    }
    else if (toScn->isClosing())
    {
        send.retVal = REQFB_EXIST_CLOSING;
        send.closeCDTime = toScn->getCloseCDTime();
    }
    else if (toScn->isUnloading())
    {
        send.retVal = REQFB_EXIST_UNLOAD;
    }
         
    sessionClient->sendCmd(&send, sizeof(send));
    return true;
}

bool FuBenMgr::doReplyOpenFuBen( Cmd::Session::Reply_OpenEnterFuBen_SceneSession *rev )
{
    if ( !rev || !rev->userID )
    {
        return false;
    }
    if ( rev->reqSvrID != ScenesService::getInstance().getServerID() )
    {
        return false;
    }

    SceneUser *pUser = SceneUserManager::getMe().getUserByID(rev->userID);
    if ( !pUser )
    {
        return false;
    }
    
    using namespace Cmd::Session;
    switch (rev->retVal)
    {
    case REQFB_NO_OVER_MAXCNT:
        {
            Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "当前副本数量已经达到最大，请稍后再来");
            return false;
        }
    	break;
    case REQFB_NO_SYS_BUSY:
        {
            Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "系统繁忙，请稍后再来");
            return false;
        }
        break;
    case REQFB_YES_OPEN_NEW:
        {
            if ( checkNeedTeam( rev->fbType) )
            {
                struct TeamGoMap : public TeamMemExec
                {
                    SceneUser *leader;
                    DWORD      fbType;
                    DWORD      fbScnID;
                    std::string     fbName;
                    zPos       toPos;
                    TeamGoMap(SceneUser *ldr, DWORD fbType, DWORD fbScnID, const char* fbName, zPos pos)
                        :leader(ldr), fbType(fbType), fbScnID(fbScnID), fbName(fbName), toPos(pos)
                    {
                    }

                    bool exec(TeamMember &member)
                    {
                        SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(member.tempid);
                        if( !pUser ) return true;
                        if( pUser->tradeorder.hasBegin() || pUser->privatestore.step() 
                            || pUser->getState() == SceneUser::SceneEntry_Death
                            || pUser->guard )
                        {
                            return true;
                        }

                        bool canEnter = FuBenMgr::getMe().checkUserValid(pUser, fbType, Cmd::FB_ACTION_TYPE_ENTER);
                        if ( !canEnter )
                        {
                            Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你不满足进入副本的条件");
                            return false;
                        }

                        if ( pUser->isTeamLeader() )
                            pUser->setFuBenInfo( fbType, fbScnID, Cmd::FB_ACTION_TYPE_OPEN );
                        else
                            pUser->setFuBenInfo( fbType, fbScnID, Cmd::FB_ACTION_TYPE_ENTER );
                        
                        zPos  tmpPos = toPos.getRandPosByRadius(2);
                        // std::ostringstream os;
                        // os << "name=" << fbName <<" pos=" << tmpPos.x << "," << tmpPos.y;
                        // if ( Gm::gomap(pUser, os.str().c_str()) )
                        // {
                        //     Channel::sendSys(pUser, Cmd::INFO_TYPE_EXP, "%s开启了副本。你被传送入了【%s】", leader->name, fbName.c_str() );
                        //     Zebra::logger->info("[副本]%s(ID:%d)进入了 %s(Type:%d)", pUser->name, pUser->id, fbName.c_str(), fbType);
                        // }
                        std::ostringstream os;
                        os << "name=" << "中立区·云天别墅" << pUser->id <<" pos=" << 32 << "," << 104;
                        if ( Gm::gomap(pUser, os.str().c_str()) )
                        {
                            Channel::sendSys( pUser, Cmd::INFO_TYPE_EXP, "你进入了你的云天别墅");
                            Zebra::logger->info("[副本]%s(ID:%d)进入了 %s(Type:%d)", pUser->name, pUser->id, fbName.c_str(), fbType);
                        }
                        return true;
                    }
                };

                if( pUser->isTeamLeader() )
                {
                    TeamGoMap exec( pUser, rev->fbType, rev->fbScnID, rev->fbName, rev->dstPos );
                    pUser->team.execEveryOne(exec);
                    Channel::sendSys(pUser, Cmd::INFO_TYPE_EXP, "你成功开启了【%s】", rev->fbName);
                    Zebra::logger->info("[副本]%s(ID:%d)开启了 %s(Type:%d)", pUser->name, pUser->id, rev->fbName, rev->fbType);
                }   
            }
            else
            {
                zPos  toPos = rev->dstPos.getRandPosByRadius(2);
                // std::ostringstream os;
                // os << "name=" << rev->fbName <<" pos=" << toPos.x << "," << toPos.y;
                // if ( Gm::gomap(pUser, os.str().c_str()) )
                // {
                //     pUser->setFuBenInfo( rev->fbType, rev->fbScnID, Cmd::FB_ACTION_TYPE_OPEN );
                //     Channel::sendSys( pUser, Cmd::INFO_TYPE_EXP, "%s开启了副本。你被传送入了【%s】",pUser->name, rev->fbName );
                //     Zebra::logger->info("[副本]%s(ID:%d)进入了 %s(Type:%d)", pUser->name, pUser->id, rev->fbName, rev->fbType);
                // }
                std::ostringstream os;
                os << "name=" << "中立区·云天别墅" << pUser->id <<" pos=" << 32 << "," << 104;
                if ( Gm::gomap(pUser, os.str().c_str()) )
                {
                    pUser->setFuBenInfo( rev->fbType, rev->fbScnID, Cmd::FB_ACTION_TYPE_ENTER );
                    Channel::sendSys( pUser, Cmd::INFO_TYPE_EXP, "你进入了你的云天别墅");
                    Zebra::logger->info("[副本]%s(ID:%d)进入了 %s(Type:%d)", pUser->name, pUser->id, rev->fbName, rev->fbType);
                }
            } 
        }
        break;
    case REQFB_EXIST_RUN:
        {
            // Channel::sendSys(pUser, Cmd::INFO_TYPE_MSG, "此副本已经开启，无需再次开启。");
            reqEnterFuBen(pUser, rev->fbType);
            return true;
        }
        break;
    case REQFB_EXIST_CLOSING:
        {
            Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "该副本正在关闭,请%d分钟后再来", rev->closeCDTime);
            return true;
        }
        break;
    case REQFB_EXIST_UNLOAD:
        {
            Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "该副本正在卸载,请稍后再来");
            return true;
        }
        break;
    default:
    	break;
    }
    return true;
}




//家族地图
bool FuBenMgr::reqOpenJiaZu( SceneUser *pUser , DWORD fbType )
{
    if(!pUser)
    {
        return false;
    }
    if ( pUser->getState() == SceneUser::SceneEntry_Death )
    {
        return false;
    }

    if (pUser->guard)
    {
        Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你还有护送任务，完成后再来吧");
        return false;
    }

    if( pUser->tradeorder.hasBegin() || pUser->privatestore.step() ) 
    {
        Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "交易或摆摊中无法开启或进入家族地图");
        return true;
    }

    DWORD fbScnID = 1;
    DWORD openCnt = 0;
    DWORD enterCnt = 0;
    pUser->getFuBenInfo(fbType, fbScnID, openCnt, enterCnt);

    using namespace  Cmd::Session;
    Req_OpenEnterJiaZu_SceneSession send;
    send.actType = Cmd::FB_ACTION_TYPE_OPEN;
    send.userID = pUser->id;
    send.reqSvrID = ScenesService::getInstance().getServerID();
    send.fbType = fbType;
    send.lastFBScnID = fbScnID;
    send.counID = getFBCounID(fbType);
    send.mapID = getFBMapID(fbType);
    sessionClient->sendCmd(&send, sizeof(send));
   
    return true;
}

//家族地图
bool FuBenMgr::doReqOpenJiaZu( Cmd::Session::Req_OpenEnterJiaZu_SceneSession *rev )
{
    if ( !rev )
    {
        return false;
    }
    SceneUser *pUser = SceneUserManager::getMe().getUserByID(rev->userID);
    if ( !pUser ) return false;

    if(pUser->charbase.septid == 0)
    {
        Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你还未加入家族，无法进入家族地图");
        return false;
    }

    Scene *findScn = SceneManager::getInstance().getSceneByID( (rev->counID << 16) + rev->mapID );
    if ( !findScn ) return false;

    DWORD fbType = rev->fbType;

    using namespace Cmd::Session;
    Reply_OpenEnterJiaZu_SceneSession send;
    send.actType = Cmd::FB_ACTION_TYPE_OPEN;
    send.userID = rev->userID;
    send.fbType = fbType;
    send.reqSvrID = rev->reqSvrID;
    send.replySvrID = ScenesService::getInstance().getServerID();
        
    Scene *fbScn = NULL;

    char fbName[64];
    sprintf(fbName, "中立区·家族地图%d", pUser->charbase.septid);
    Scene *toScn  = SceneManager::getInstance().getSceneByName(fbName);
    if (!toScn)
    {
        fbScn = SceneManager::getInstance().openJiaZuScene(pUser->charbase.septid, fbType, true);
        if (!fbScn)
        {
            send.retVal = REQFB_NO_SYS_BUSY;
        }
        else
        {
            send.retVal = REQFB_YES_OPEN_NEW;
        }
    }
    else if (toScn->isRunning() && fbType == toScn->getFBType())
    {
        send.retVal = REQFB_EXIST_RUN;
    }
    else if (toScn->isClosing())
    {
        send.retVal = REQFB_EXIST_CLOSING;
        send.closeCDTime = toScn->getCloseCDTime();
    }
    else if (toScn->isUnloading())
    {
        send.retVal = REQFB_EXIST_UNLOAD;
    }

    sessionClient->sendCmd(&send, sizeof(send));
    return true;
}


bool FuBenMgr::reqEnterFuBen( SceneUser *pUser, DWORD fbType )
{
    bool canEnter = FuBenMgr::getMe().checkUserValid(pUser, fbType, Cmd::FB_ACTION_TYPE_ENTER);
    if ( !canEnter )
    {
        Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "条件不符，无法进入副本");
        return false;
    }

    //以下是本服内开启副本
    DWORD fbScnID = 0;
    DWORD openCnt = 0;
    DWORD enterCnt = 0;
    pUser->getFuBenInfo(fbType, fbScnID, openCnt, enterCnt);

    using namespace  Cmd::Session;
    Req_OpenEnterFuBen_SceneSession send;
    send.actType = Cmd::FB_ACTION_TYPE_ENTER;
    send.userID = pUser->id;
    send.reqSvrID = ScenesService::getInstance().getServerID();
    send.fbType = fbType;
    send.lastFBScnID = fbScnID;
    send.counID = getFBCounID(fbType);
    send.mapID = getFBMapID(fbType);
    sessionClient->sendCmd(&send, sizeof(send));
    return true;
}

bool FuBenMgr::doReqEnterFuBen( Cmd::Session::Req_OpenEnterFuBen_SceneSession *rev )
{
    if ( !rev || !rev->fbType || !rev->userID )
    {
        return false;
    }
    
    Scene *findScn = SceneManager::getInstance().getSceneByID( (rev->counID << 16) + rev->mapID );
    if ( !findScn ) return false;

    DWORD fbType = rev->fbType;
    DWORD fbScnID = rev->lastFBScnID;

    using namespace Cmd::Session;
    Reply_OpenEnterFuBen_SceneSession send;
    send.actType = Cmd::FB_ACTION_TYPE_ENTER;
    send.userID = rev->userID;
    send.fbType = fbType;
    send.reqSvrID = rev->reqSvrID;
    send.replySvrID = ScenesService::getInstance().getServerID();

    // if ( fbScnID )
    // {
    //     Scene *toScn = SceneManager::getInstance().getSceneByID( fbScnID );
    //     if ( !toScn )
    //     { 
    //         send.retVal = REQFB_NEED_REOPEN;
    //     }
    //     else if ( toScn->isClosing() )
    //     {
    //         send.retVal = REQFB_EXIST_CLOSING;
    //         send.closeCDTime = toScn->getCloseCDTime();
    //     }
    //     else if ( toScn->isUnloading() )
    //     {
    //         send.retVal = REQFB_EXIST_UNLOAD;
    //     }
    //     else if ( toScn->isRunning() &&  fbType == toScn->getFBType() )
    //     {
    //         send.fbScnID = toScn->id;
    //         bcopy( toScn->name, send.fbName, sizeof(send.fbName) );
    //         send.retVal = REQFB_CAN_ENTER;
    //         const WayPoint *wp = toScn->getRandWayPoint(); 
    //         if( wp )
    //         {
    //             const zPos *pos = wp->getRandPoint();
    //             if ( pos )  send.dstPos = *pos;
    //         }
    //     }
    // }
    // else
    // {
    //     send.retVal = REQFB_NEED_REOPEN;
    // }

    Scene *fbScn = NULL;
    char fbName[64];
    sprintf(fbName, "中立区·云天别墅%d", rev->userID);
    Scene *toScn = SceneManager::getInstance().getSceneByName(fbName);
    if (!toScn)
    {
        send.retVal = REQFB_NEED_REOPEN;
    }
    else if (toScn->isClosing())
    {
        send.retVal = REQFB_EXIST_CLOSING;
        send.closeCDTime = toScn->getCloseCDTime();
    }
    else if (toScn->isUnloading())
    {
        send.retVal = REQFB_EXIST_UNLOAD;
    }
    else if (toScn->isRunning() && fbType == toScn->getFBType())
    {
        send.fbScnID = toScn->id;
        bcopy(toScn->name, send.fbName, sizeof(send.fbName));
        send.retVal = REQFB_CAN_ENTER;
        const WayPoint *wp = toScn->getRandWayPoint();
        if (wp)
        {
            const zPos *pos = wp->getRandPoint();
            if (pos)
                send.dstPos = *pos;
        }
    }
    else{
        send.retVal = REQFB_NEED_REOPEN;
    }

    sessionClient->sendCmd(&send, sizeof(send));
    return true;
}

bool FuBenMgr::doReplyEnterFuBen( Cmd::Session::Reply_OpenEnterFuBen_SceneSession *rev )
{
    if ( !rev || !rev->reqSvrID || !rev->userID )
    {
        return false;
    }
    if ( rev->reqSvrID != ScenesService::getInstance().getServerID() )
    {
        return false;
    }

    SceneUser *pUser = SceneUserManager::getMe().getUserByID(rev->userID);
    if ( !pUser )
    {
        return false;
    }
    
    using namespace Cmd::Session;
    switch (rev->retVal)
    {
    case REQFB_NEED_REOPEN:
        {
            Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你还没有开启副本");
            return false;
        }
    	break;
    case REQFB_NO_SYS_BUSY:
        {
            Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "系统繁忙，请稍后再来");
            return false;
        }
        break;
    case REQFB_EXIST_CLOSING:
        {
            Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "该副本正在关闭,请%d分钟后再来", rev->closeCDTime);
            return true;
        }
        break;
    case REQFB_EXIST_UNLOAD:
        {
            Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "该副本正在卸载,请稍后再来");
            return true;
        }
        break;
    case REQFB_CAN_ENTER:
        {
            // zPos  toPos = rev->dstPos.getRandPosByRadius(2);
            // std::ostringstream os;
            // os << "name=" << rev->fbName <<" pos=" << toPos.x << "," << toPos.y;
            // if ( Gm::gomap(pUser, os.str().c_str()) )
            // {
            //     pUser->setFuBenInfo( rev->fbType, rev->fbScnID, Cmd::FB_ACTION_TYPE_ENTER );
            //     Channel::sendSys( pUser, Cmd::INFO_TYPE_EXP, "你进入了【%s】", rev->fbName );
            //     Zebra::logger->info("[副本]%s(ID:%d)进入了 %s(Type:%d)", pUser->name, pUser->id, rev->fbName, rev->fbType);
            // }
            std::ostringstream os;
            os << "name=" << "中立区·云天别墅" << pUser->id <<" pos=" << 32 << "," << 104;
            if ( Gm::gomap(pUser, os.str().c_str()) )
            {
                pUser->setFuBenInfo( rev->fbType, rev->fbScnID, Cmd::FB_ACTION_TYPE_ENTER );
                Channel::sendSys( pUser, Cmd::INFO_TYPE_EXP, "你进入了你的云天别墅");
                Zebra::logger->info("[副本]%s(ID:%d)进入了 %s(Type:%d)", pUser->name, pUser->id, rev->fbName, rev->fbType);
            }
        }
        break;
    default:
    	break;
    }
    return true;
}


//家族地图
bool FuBenMgr::doReplyEnterJiaZu( Cmd::Session::Reply_OpenEnterJiaZu_SceneSession *rev )
{
    if ( !rev || !rev->reqSvrID || !rev->userID )
    {
        return false;
    }
    if ( rev->reqSvrID != ScenesService::getInstance().getServerID() )
    {
        return false;
    }

    SceneUser *pUser = SceneUserManager::getMe().getUserByID(rev->userID);
    if ( !pUser )
    {
        return false;
    }
    
    using namespace Cmd::Session;
    switch (rev->retVal)
    {
    case REQFB_NO_SYS_BUSY:
        {
            Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "系统繁忙，请稍后再来");
            return false;
        }
        break;
    case REQFB_EXIST_CLOSING:
        {
            Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "该副本正在关闭,请%d分钟后再来", rev->closeCDTime);
            return true;
        }
        break;
    case REQFB_EXIST_UNLOAD:
        {
            Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "该副本正在卸载,请稍后再来");
            return true;
        }
        break;
    default:
        {
            std::ostringstream os;
            os << "name=" << "中立区·家族地图" << pUser->charbase.septid <<" pos=" << 31 << "," << 37;
            if ( Gm::gomap(pUser, os.str().c_str()) )
            {
                pUser->setFuBenInfo( rev->fbType, rev->fbScnID, Cmd::FB_ACTION_TYPE_ENTER );
                Channel::sendSys( pUser, Cmd::INFO_TYPE_EXP, "你进入了你的家族地图");
                Zebra::logger->info("[副本]%s(ID:%d)进入了 %s(Type:%d)", pUser->name, pUser->id, rev->fbName, rev->fbType);
            }
        }
    	break;
    }
    return true;
}

bool SceneUser::doXinFuBenCmd(const Cmd::FuBenUserCmd *rev, unsigned int cmdLen)
{
    if (NULL == rev)
        return false;
    using namespace Cmd;
    switch (rev->byParam)
    {
    case FUBEN_INFO:
    {
        Cmd::stFubenReturnInfoCmd res;
        res.fba1 = charbase.fba1;
        res.fba2 = charbase.fba2;
        res.fba3 = charbase.fba3;
        res.fba4 = charbase.fba4;
        res.fba5 = charbase.fba5;
        this->sendCmdToMe(&res, sizeof(res));
        return true;
    }
    break;
    case FUBEN_IN:
    {
        stFubenInCmd *pCmd = (stFubenInCmd *)rev;
        int type = pCmd->type;
        int select = pCmd->select;
        if(type==0)
        { 
            //右上角的BOSS副本地图
            if(select==1) //顺序1
            {
                struct tm tv1;
		   	    time_t timValue = time(NULL);
			    zRTime::getLocalTime(tv1, timValue);
				//if (tv1.tm_hour>=10 &&tv1.tm_hour<=12) 
                if (tv1.tm_hour>=20 &&tv1.tm_hour<21) //20点-21点
				{
                    Gm::gomap(this, "name=中立区·镇妖塔一层 pos=91,86");
					return true;
				}
                else{
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，当前不在副本开放时间内！");
                    return false;
                }
            }
            if(select==2) //顺序2
            {
                struct tm tv1;
		   	    time_t timValue = time(NULL);
			    zRTime::getLocalTime(tv1, timValue);
				//if (tv1.tm_hour>=12 &&tv1.tm_hour<=23) 
                if (tv1.tm_hour>=21 &&tv1.tm_hour<22) //21点-22点
				{
                  Gm::gomap(this, "name=中立区·封龙之地 pos=23,66");
					return true;
				}
                else{
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，当前不在副本开放时间内！");
                    return false;
                }
            }
            if(select==3) //顺序3
            {
                struct tm tv1;
		   	    time_t timValue = time(NULL);
			    zRTime::getLocalTime(tv1, timValue);
			
				if (tv1.tm_hour>=18 &&tv1.tm_hour<24) 
				{
                    Gm::gomap(this, "name=中立区·财神宝地 pos=36,31");
					return true;
				}
                else{
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，当前不在副本开放时间内！");
                    return false;
                }
            }
        }

        //右上角竞技副本地图
        else if(type==1)
        {
            if(select==1) //顺序1
            {
                struct tm tv1;
		   	    time_t timValue = time(NULL);
			    zRTime::getLocalTime(tv1, timValue);
			
				if (tv1.tm_hour>=20 &&tv1.tm_hour<=21) //竞技比赛 时间地图 你自己改就行
				{
                    Gm::gomap(this, "name=中立区·封龙之地副本 pos=154,136");
					return true;
				}
                else{
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，当前不在活动开放时间内！");
                    return false;
                }
            }
        }

        //右上角材料副本地图
        else if(type==2) //这里 type 代表三个分页 2就是第三个
        {
            if(select==1)// 这里 1234 代表四个副本
            {
                if(this->charbase.fba1<=0)
                {
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，您的副本次数不足！");
                    return false;
                }
               
			
                    Gm::gomap(this, "name=中立区·附件地图 pos=36,31");
					return true;
		
            }
             if(select==2)//
            {
                  if(this->charbase.fba2<=0)
                {
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，您的副本次数不足！");
                    return false;
                }
                 Gm::gomap(this, "name=中立区·元神地图 pos=36,31");
					return true;
            }
             if(select==3)//
            {
                  if(this->charbase.fba3<=0)
                {
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，您的副本次数不足！");
                    return false;
                }
                 Gm::gomap(this, "name=中立区·法宝地图 pos=36,31");
					return true;
            }
             if(select==4)//
            {
                  if(this->charbase.fba4<=0)
                {
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，您的副本次数不足！");
                    return false;
                }
                Gm::gomap(this, "name=中立区·生肖地图 pos=36,31");
				return true;
            }
            
        }
        
        return true;
    }
    break;
    case FUBEN_BUY:
    {
        stFubenBuyCmd *pCmd = (stFubenBuyCmd *)rev;
        int select = pCmd->select;

        if (charbase.charvip >= 10 && charbase.charvip < 30) // v2-v3
        {
            if (charbase.vipfb + 1 > 1)
            {
                Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您今日的VIP购买次数已全部使用！");
                return false;
            }
            else
            {
                if (packs.checkGold(20000) && packs.removeGold(20000, "副本次数购买"))
                {
                    if (select == 1)
                    {
                        charbase.fba1 += 1;
                    }
                    else if (select == 2)
                    {
                        charbase.fba2 += 1;
                    }
                    else if (select == 3)
                    {
                        charbase.fba3 += 1;
                    }
                    else if (select == 4)
                    {
                        charbase.fba4 += 1;
                    }
                    else if (select == 5)
                    {
                        charbase.fba5 += 1;
                    }
                    charbase.vipfb += 1;
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "购买成功，副本次数已增加！");
                    Cmd::stFubenReturnInfoCmd res;
                    res.fba1 = charbase.fba1;
                    res.fba2 = charbase.fba2;
                    res.fba3 = charbase.fba3;
                    res.fba4 = charbase.fba4;
                    res.fba5 = charbase.fba5;
                    this->sendCmdToMe(&res, sizeof(res));
                    return true;
                }
                else
                {
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您的金子不足！");
                    return false;
                }
            }
        }
        else if (charbase.charvip >= 30 && charbase.charvip < 50)
        { // v4-v5
            if (charbase.vipfb + 1 > 2)
            {
                Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您今日的VIP购买次数已全部使用！");
                return false;
            }
            else
            {
                if (packs.checkGold(20000) && packs.removeGold(20000, "副本次数购买"))
                {
                    if (select == 1)
                    {
                        charbase.fba1 += 1;
                    }
                    else if (select == 2)
                    {
                        charbase.fba2 += 1;
                    }
                    else if (select == 3)
                    {
                        charbase.fba3 += 1;
                    }
                    else if (select == 4)
                    {
                        charbase.fba4 += 1;
                    }
                    else if (select == 5)
                    {
                        charbase.fba5 += 1;
                    }
                    charbase.vipfb += 1;
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "购买成功，副本次数已增加！");
                    Cmd::stFubenReturnInfoCmd res;
                    res.fba1 = charbase.fba1;
                    res.fba2 = charbase.fba2;
                    res.fba3 = charbase.fba3;
                    res.fba4 = charbase.fba4;
                    res.fba5 = charbase.fba5;
                    this->sendCmdToMe(&res, sizeof(res));
                    return true;
                }
                else
                {
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您的金子不足！");
                    return false;
                }
            }
        }
        else if (charbase.charvip >= 50 && charbase.charvip < 70)
        { // v6-v7
            if (charbase.vipfb + 1 > 3)
            {
                Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您今日的VIP购买次数已全部使用！");
                return false;
            }
            else
            {
                if (packs.checkGold(20000) && packs.removeGold(20000, "副本次数购买"))
                {
                   if (select == 1)
                    {
                        charbase.fba1 += 1;
                    }
                    else if (select == 2)
                    {
                        charbase.fba2 += 1;
                    }
                    else if (select == 3)
                    {
                        charbase.fba3 += 1;
                    }
                    else if (select == 4)
                    {
                        charbase.fba4 += 1;
                    }
                    else if (select == 5)
                    {
                        charbase.fba5 += 1;
                    }
                    charbase.vipfb += 1;
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "购买成功，副本次数已增加！");
                    Cmd::stFubenReturnInfoCmd res;
                    res.fba1 = charbase.fba1;
                    res.fba2 = charbase.fba2;
                    res.fba3 = charbase.fba3;
                    res.fba4 = charbase.fba4;
                    res.fba5 = charbase.fba5;
                    this->sendCmdToMe(&res, sizeof(res));
                    return true;
                }
                else
                {
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您的金子不足！");
                    return false;
                }
            }
        }
        else if (charbase.charvip >= 70 && charbase.charvip < 90)
        { // v8-v9
            if (charbase.vipfb + 1 > 4)
            {
                Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您今日的VIP购买次数已全部使用！");
                return false;
            }
            else
            {
                if (packs.checkGold(20000) && packs.removeGold(20000, "副本次数购买"))
                {
                    if (select == 1)
                    {
                        charbase.fba1 += 1;
                    }
                    else if (select == 2)
                    {
                        charbase.fba2 += 1;
                    }
                    else if (select == 3)
                    {
                        charbase.fba3 += 1;
                    }
                    else if (select == 4)
                    {
                        charbase.fba4 += 1;
                    }
                    else if (select == 5)
                    {
                        charbase.fba5 += 1;
                    }
                    charbase.vipfb += 1;
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "购买成功，副本次数已增加！");
                    Cmd::stFubenReturnInfoCmd res;
                    res.fba1 = charbase.fba1;
                    res.fba2 = charbase.fba2;
                    res.fba3 = charbase.fba3;
                    res.fba4 = charbase.fba4;
                    res.fba5 = charbase.fba5;
                    this->sendCmdToMe(&res, sizeof(res));
                    return true;
                }
                else
                {
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您的金子不足！");
                    return false;
                }
            }
        }
        else if (charbase.charvip >= 90)
        { // v10
            if (charbase.vipfb + 1 > 5)
            {
                Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您今日的VIP购买次数已全部使用！");
                return false;
            }
            else
            {
                if (packs.checkGold(20000) && packs.removeGold(20000, "副本次数购买"))
                {
                   if (select == 1)
                    {
                        charbase.fba1 += 1;
                    }
                    else if (select == 2)
                    {
                        charbase.fba2 += 1;
                    }
                    else if (select == 3)
                    {
                        charbase.fba3 += 1;
                    }
                    else if (select == 4)
                    {
                        charbase.fba4 += 1;
                    }
                    else if (select == 5)
                    {
                        charbase.fba5 += 1;
                    }
                    charbase.vipfb += 1;
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "购买成功，副本次数已增加！");
                    Cmd::stFubenReturnInfoCmd res;
                    res.fba1 = charbase.fba1;
                    res.fba2 = charbase.fba2;
                    res.fba3 = charbase.fba3;
                    res.fba4 = charbase.fba4;
                    res.fba5 = charbase.fba5;
                    this->sendCmdToMe(&res, sizeof(res));
                    return true;
                }
                else
                {
                    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您的金子不足！");
                    return false;
                }
            }
        }
        else
        { // 非
            Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，您的VIP等级不足以购买额外副本次数！");
            return false;
        }
        return true;
    }
    break;
    default:
        break;
    }
    return true;
}

bool SceneUser::doFuBenCmd( const Cmd::FuBenUserCmd *rev, unsigned int cmdLen )
{
    if( NULL == rev ) return false;
    using namespace  Cmd;
    switch(rev->byParam)
    {
    case REQ_OPEN_FUBEN_PARA:
        {
            ReqOpenFuBenUserCmd *pCmd = (ReqOpenFuBenUserCmd *)rev;
            bool ret = FuBenMgr::getMe().reqOpenFuBen( this, pCmd->fbType );
            return ret;
        }
        break;
    case REQ_ENTER_FUBEN_PARA:
        {
            ReqEnterFuBenUserCmd *pCmd = (ReqEnterFuBenUserCmd *)rev;
            bool ret = FuBenMgr::getMe().reqEnterFuBen(this,pCmd->fbType);           
            return ret;
        }
        break;
    case REQ_OPEN_JIAZU_PARA://家族地图
        {
            ReqOpenJiaZuUserCmd *pCmd = (ReqOpenJiaZuUserCmd *)rev;
            bool ret = FuBenMgr::getMe().reqOpenJiaZu(this,8);           
            return ret;
        }
        break;
    default:
        break;
    }
    return true;
}


bool SceneUser::openFuBen( DWORD fbType )
{
    bool canOpen = FuBenMgr::getMe().checkValid(this, fbType, Cmd::FB_ACTION_TYPE_OPEN);
    if ( !canOpen )
    {
        Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "无法开启副本");
        return false;
    }

    //以下是本服内开启副本
    DWORD fbScnID = 0;
    DWORD openCnt = 0;
    DWORD enterCnt = 0;
    getFuBenInfo(fbType, fbScnID, openCnt, enterCnt);

    using namespace  Cmd::Session;
    Req_OpenEnterFuBen_SceneSession send;
    send.userID = this->id;
    send.reqSvrID = ScenesService::getInstance().getServerID();
    send.fbType = fbType;
    send.lastFBScnID = fbScnID;

    sessionClient->sendCmd(&send, sizeof(send));
    return true;


    Scene *fbScn = NULL;
    if ( fbScnID )
    {
        Scene *toScn = SceneManager::getInstance().getSceneByID( fbScnID );
        if ( !toScn )
        { 
            //Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "开启副本。先前开启的副本已经关闭");
            fbScn = SceneManager::getInstance().openFuBenScene(this->id,fbType);
        }
        else if ( toScn->isRunning() &&  fbType == toScn->getFBType() )
        {
           // Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "此副本已经开启，无需再次开启。");
            FuBenMgr::getMe().reqEnterFuBen( this, fbType);
            return false;
        }
        else if ( toScn->isClosing() )
        {
            Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "该副本正在关闭,请%d分钟后再来", toScn->getCloseCDTime() );
            return false;
        }
    }
    else
    {
        fbScn = SceneManager::getInstance().openFuBenScene(this->id,fbType);
    }
    
    if ( !fbScn )
    {
        Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "系统繁忙！请稍后再试试");
        return false;
    }
    else if ( fbScn->isClosing() )
    {
        Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "该副本正在关闭,请%d分钟后再来", fbScn->getCloseCDTime() );
        return false;
    }
    else if ( fbScn->isRunning() )
    {
        Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "你开启了【%s】", fbScn->name);
        if( isTeamLeader() )
        {
            struct TeamGoMap : public TeamMemExec
            {
                SceneUser *leader;
                Scene     *toScn;
                TeamGoMap(SceneUser *ldr, Scene *scn)
                    :leader(ldr), toScn(scn)
                {
                }

                bool exec(TeamMember &member)
                {
                    SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(member.tempid);
                    if( !pUser ) return true;
                    if( pUser->tradeorder.hasBegin() || pUser->privatestore.step() 
                        || pUser->getState() == SceneUser::SceneEntry_Death
                        || pUser->guard )
                    {
                        return true;
                    }
                    
                    bool canEnter = FuBenMgr::getMe().checkValid(pUser, toScn->getFBType(), Cmd::FB_ACTION_TYPE_ENTER);
                    if ( !canEnter )
                    {
                        Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "你不满足进入副本的条件");
                        return false;
                    }

                    if ( pUser->isTeamLeader() )
                    {
                        pUser->setFuBenInfo( toScn, Cmd::FB_ACTION_TYPE_OPEN );
                    }
                    else
                    {
                        pUser->setFuBenInfo( toScn, Cmd::FB_ACTION_TYPE_ENTER );
                    }

                    std::ostringstream os;
                    os << "name=" << toScn->name;
                    if ( Gm::gomap(pUser, os.str().c_str()) )
                    {
                        Channel::sendSys(pUser, Cmd::INFO_TYPE_EXP, "%s开启了副本。你被传送入了【%s】", leader->name, toScn->name);
                        Zebra::logger->info("[副本]%s(ID:%d)进入了 %s(Type:%d)", pUser->name, pUser->id, toScn->name, toScn->getFBType());
                    }
                    return true;
                }
            };

            TeamGoMap exec( this, fbScn );
            this->team.execEveryOne(exec);
            Zebra::logger->info("[副本]%s(ID:%d)开启了 %s(Type:%d) 副本场景", name, id, fbScn->name, fbType);
        }
        
        return true;
    }
    return false;
}

bool SceneUser::enterFuBen( DWORD fbType )
{
    bool canEnter = FuBenMgr::getMe().checkValid(this, fbType, Cmd::FB_ACTION_TYPE_ENTER);
    if ( !canEnter )
    {
        Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "无法进入副本");
        return false;
    }
    
    DWORD fbScnID = 0;
    DWORD openCnt = 0;
    DWORD enterCnt = 0;
    getFuBenInfo(fbType, fbScnID, openCnt, enterCnt);

    //tempID
    Scene *toScn = SceneManager::getInstance().getSceneByID( fbScnID );
    if ( !toScn )
    { 
        Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "副本已经关闭，你可以重新开启");
        return false;
    }
    else if ( toScn->isClosing() )
    {
        Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "该副本正在关闭");
        return false;
    }
    else if ( toScn->isRunning() &&  fbType == toScn->getFBType() )
    {
        if ( isTeamLeader() )
        {
            if( openCnt && enterCnt )
            {
                std::ostringstream where;
                where << "name=" << toScn->name;
                if ( Gm::gomap(this, where.str().c_str()) )
                {
                    setFuBenInfo(toScn, Cmd::FB_ACTION_TYPE_ENTER);
                    Zebra::logger->info("[副本]%s(ID:%d)进入了 %s(Type:%d) 副本场景", name, id, toScn->name, fbType);
                }
            }
            else
            {
                Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你还未开启此副本");
                return false;
            }
        }
        else
        {
            std::ostringstream where;
            where << "name=" << toScn->name;
            if ( Gm::gomap(this, where.str().c_str()) )
            {
                setFuBenInfo(toScn, Cmd::FB_ACTION_TYPE_ENTER);
                Zebra::logger->info("[副本]%s(ID:%d)进入了 %s(Type:%d) 副本场景", name, id, toScn->name, fbType);
            }
        }
    }
    return true;
}

bool SceneUser::setFuBenInfo( Scene *fbScn, Cmd::FBActionType actType )
{
    if ( !fbScn ) return false;
    DWORD counterID = getFBCounterID( fbScn->getFBType() );

    switch (actType)
    {
    case Cmd::FB_ACTION_TYPE_OPEN:
        {
            DWORD openCnt = 1;
            DWORD enterCnt = 1;
            if ( cm.getCounter(counterID) != fbScn->id )
            {
                cm.clearCounter(counterID);
                cm.addCounterDay(counterID, fbScn->id, openCnt, enterCnt);
            }
            else
            {
                cm.addCounterDay(counterID, 0, openCnt, enterCnt);
            }
        }
    	break;
    case Cmd::FB_ACTION_TYPE_ENTER:
        {
            DWORD openCnt = 0;
            DWORD enterCnt = 1;
            if ( cm.getCounter(counterID) != fbScn->id )
            {
                cm.clearCounter(counterID);
                cm.addCounterDay(counterID, fbScn->id, openCnt, enterCnt);
            }
            else
            {
                cm.addCounterDay(counterID, 0, openCnt, enterCnt);
            }
        }
    	break;
    default:
    	break;
    }
    return true;
}

bool SceneUser::setFuBenInfo( DWORD fbType, DWORD fbScnID, Cmd::FBActionType actType )
{
    if ( !fbType || !fbScnID ) return false;
    DWORD counterID = getFBCounterID( fbType );

    switch (actType)
    {
    case Cmd::FB_ACTION_TYPE_OPEN:
        {
            DWORD openCnt = 1;
            DWORD enterCnt = 1;
            if ( cm.getCounter(counterID) != fbScnID )
            {
                cm.clearCounter(counterID);
                cm.addCounterDay(counterID, fbScnID, openCnt, enterCnt);
            }
            else
            {
                cm.addCounterDay(counterID, 0, openCnt, enterCnt);
            }
        }
    	break;
    case Cmd::FB_ACTION_TYPE_ENTER:
        {
            DWORD openCnt = 0;
            DWORD enterCnt = 1;
            if ( cm.getCounter(counterID) != fbScnID )
            {
                cm.clearCounter(counterID);
                cm.addCounterDay(counterID, fbScnID, openCnt, enterCnt);
            }
            else
            {
                cm.addCounterDay(counterID, 0, openCnt, enterCnt);
            }
        }
    	break;
    default:
    	break;
    }
    return true;
}

bool SceneUser::getFuBenInfo( DWORD fbType, DWORD &fbScnID, DWORD &openCnt, DWORD &enterCnt )
{
    DWORD counterID = getFBCounterID( fbType );
    fbScnID = cm.getCounter(counterID, openCnt, enterCnt);
    return true;
}





