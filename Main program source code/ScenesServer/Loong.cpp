/**
 * \file   Loong.cpp	
 * \brief  太庙资源掠夺
 * \author WQH
 * \date   2010年7月10日
 * 
 */
#include <math.h>
#include "SceneUser.h"
#include "SceneManager.h"
#include "zMisc.h"
#include "Chat.h"
#include "SceneNpcManager.h"
#include "CountryTech.h"

bool SceneUser::doLoongUserCmd(const Cmd::LoongUserCmd *rev, unsigned int cmdLen)
{
    if( NULL == rev ) return false;
    using namespace  Cmd;
    switch(rev->byParam)
    {
    case REQ_FETCH_LOONGPEARL_PARA: //soke 玩家请求从太庙中夺取砖块
        {
            //ReqFetchLoongPearlLoongUserCmd* cmd = (ReqFetchLoongPearlLoongUserCmd *)rev;
            
            fetchPearlFromAltar();
            return true;
        }
        break;
    case REQ_PUT_LOONGPEARL_PARA: //soke 玩家请求把砖块放入太庙
        {
            //ReqPutLoongPearlLoongUserCmd* cmd = (ReqPutLoongPearlLoongUserCmd *)rev;
            putPearlIntoAltar();
            return true;
        }
        break;
    case REQ_LOONGALTAR_LIST_PARA: //soke 玩家请求世界太庙砖块列表
        {
            //ReqAltarListLoongUserCmd* cmd = (ReqAltarListLoongUserCmd *)rev;
            //never reach here
            return true;
            CountryTechM::getMe().sendLoongAltarList(this);
        }
        break;
	case REQ_GENGHUAN_LOONGPEARL_PARA: //soke 玩家请求从太庙中更换砖块
        {
            fetch2PearlFromAltar();
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

bool SceneUser::fetchPearlFromAltar() //soke 从太庙中夺取砖块
{
    if ( ! checkLoongPearlAct() ) //soke 等级检测
        return false;

    DWORD toCounID = scene->getCountryID();
    if ( toCounID == charbase.country )
    {
        Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "到外国太庙才能接任务！");
		return false;
    }

    if ( getFetchedPearlType() > 0 )//soke 砖块数量
    {
        Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你身上已有砖块，请回本国交任务！");
        return false;
    }
 
    DWORD curPearlNum = CountryTechM::getMe().getAltarPearlNumByCountryID(toCounID);
    if ( curPearlNum <= 0 )
    {
        Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "太庙资源已经所剩无几了！");
        return false;
    }
    else
    {
        CountryTechM::getMe().fetchPearlFromAltar(toCounID);
    }

    DWORD randNum = zMisc::randBetween(1,100);
    DWORD odds[] = {0, 55, 80, 90, 97, 100}; // odds[i]-odds[i-1] 为生成不同品质砖块的几率
    DWORD pearlType = Cmd::LOONG_PEARL_NONE;
    for ( int i = 0; i <5; ++i )
    {
        if ( randNum >= odds[i] && randNum <= odds[i+1] )
        {
            pearlType = i+1;
            break;
        }
    }

    cm.addCounterDay(LOONG_PEARL_COUNT); //soke 每天夺取的太庙资源计数
    cm.addCounter(LOONG_PEARL_TYPE, pearlType);  //soke 当前夺取的太庙资源品质
    Cmd::ReplyFetchLoongPearlLoongUserCmd send;  //soke 回复玩家夺取砖块的结果
    send.pearlType = pearlType;   
	sendCmdToMe(&send,sizeof(send));

    // this->setUState(getUStateByPearlType(pearlType));
    // 更新客户端状态
    showCurrentEffect( getUStateByPearlType(pearlType), true );


    ///////////////////////////////开始网管醉梦教网管这段代码加上是换砖图标2024-11-14 00:35:37/////////////////////////////////
    char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = getUStateByPearlType(pearlType);
	srs->states[0].result=0;
	srs->states[0].time=0XFFFF;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
    ///////////////////////////////结束网管/////////////////////////////////

    std::string  info;
    getTypeStrByPearlType(pearlType, info);
    Channel::sendSys(this , Cmd::INFO_TYPE_GAME, "成功拆除太庙资源(%s)",info.c_str());

    std::string fromCoun = SceneManager::getInstance().getCountryNameByCountryID(charbase.country);
    std::string toCoun = SceneManager::getInstance().getCountryNameByCountryID(toCounID);
    DWORD nowPearlNum = CountryTechM::getMe().getAltarPearlNumByCountryID(toCounID);
    //发国家公告 
	Channel::sendCountryInfo( scene->getCountryID(), Cmd::INFO_TYPE_GAME, "我国防守不利，%s %s 掠夺了我国一块太庙砖块，我国太庙砖块现减少为%d块！",fromCoun.c_str(), name, nowPearlNum );

	Zebra::logger->debug("%s的玩家%s 夺取了%s的一块太庙资源,%s太庙资源降到了%d块", fromCoun.c_str(),name, toCoun.c_str(), toCoun.c_str(), nowPearlNum );

    return true;
}

bool SceneUser::fetch2PearlFromAltar() //soke 从太庙中更换砖块
{
    if ( ! checkLoongPearlAct() ) //soke 等级检测
        return false;

    DWORD toCounID = scene->getCountryID();
    if ( toCounID == charbase.country )
    {
        Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "到外国太庙才能更换资源！");
		return false;
    }

    if ( getFetchedPearlType() <= 0 )//soke 砖块数量检查
    {
        Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你身上没有砖块，请拆除太庙后再来更换吧！");
        return false;
    }
 
    DWORD curPearlNum = CountryTechM::getMe().getAltarPearlNumByCountryID(toCounID);
    if ( curPearlNum <= 0 )
    {
        Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "太庙资源已经所剩无几，不能更换太庙资源！");
        return false;
    }

    if ( getFetchedPearlType() != 0 )
    {
        DWORD pearlType = getFetchedPearlType();
		showCurrentEffect( getUStateByPearlType(pearlType), false ); //soke 关闭特效
		 
        cm.clearCounter(LOONG_PEARL_TYPE); //soke 清除当前夺取的太庙资源品质

    }

    DWORD randNum = zMisc::randBetween(1,100);
    DWORD odds[] = {0, 55, 80, 90, 97, 100}; // odds[i]-odds[i-1] 为生成不同品质砖块的几率
    DWORD pearlType = Cmd::LOONG_PEARL_NONE;
    for ( int i = 0; i <5; ++i )
    {
        if ( randNum >= odds[i] && randNum <= odds[i+1] )
        {
            pearlType = i+1;
            break;
        }
    }

    cm.addCounterDay(LOONG2_PEARL_COUNT);        //soke 每天更换的太庙资源计数
    cm.addCounter(LOONG_PEARL_TYPE, pearlType);  //soke 当前更换的太庙资源品质
    Cmd::ReplyFetchLoongPearlLoongUserCmd send;  //soke 回复玩家更换砖块的结果
    send.pearlType = pearlType;   
	sendCmdToMe(&send,sizeof(send));

    // this->setUState(getUStateByPearlType(pearlType));
    // 更新客户端状态
    showCurrentEffect( getUStateByPearlType(pearlType), true );
     ///////////////////////////////开始网管醉梦教网管这段代码加上是换砖图标2024-11-14 00:35:37/////////////////////////////////
    char Buf[200]; 
	bzero(Buf , sizeof(Buf));
	Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
	constructInPlace(srs);
	srs->byType = Cmd::MAPDATATYPE_USER;
	srs->dwTempID = this->tempid;
	srs->size=1;
	srs->states[0].state = getUStateByPearlType(pearlType);
	srs->states[0].result=0;
	srs->states[0].time=0XFFFF;
	this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
			sizeof(srs->states[0]) * srs->size);
	//selected_lock.lock();
	SelectedSet_iterator iter = selected.begin();
	for(; iter != selected.end() ;)
	{
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
		if(pUser)
		{
			if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
			{
				pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
						sizeof(srs->states[0]) * srs->size);
				iter ++ ;
				continue;
			}
		}
		SelectedSet_iterator iter_del = iter;
		iter_del ++;
		selected.erase(iter);
		iter = iter_del;
	}
    ///////////////////////////////结束网管/////////////////////////////////
    std::string  info;
    getTypeStrByPearlType(pearlType, info);
    Channel::sendSys(this , Cmd::INFO_TYPE_GAME, "成功换取太庙资源(%s)",info.c_str());

    return true;
}

DWORD SceneUser::getUStateByPearlType( DWORD type )
{
    using namespace Cmd;
    DWORD ustate = 0;
    switch ( type )
    {
    case LOONG_PEARL_WHITE:
        ustate = USTATE_LOONGPEARL_WHITE;
    	break;
    case LOONG_PEARL_BLUE:
        ustate = USTATE_LOONGPEARL_BLUE;
    	break;
    case LOONG_PEARL_YELLOW:
        ustate = USTATE_LOONGPEARL_YELLOW;
    	break;
    case LOONG_PEARL_GREEN:
        ustate = USTATE_LOONGPEARL_GREEN;
    	break;
    case LOONG_PEARL_PURPLE:
        ustate = USTATE_LOONGPEARL_PURPLE;
    	break;
    default:
    	break;
    }
    return ustate;
}

void SceneUser::getTypeStrByPearlType( DWORD type, std::string& tpStr )
{
    using namespace Cmd;
    switch ( type )
    {
    case LOONG_PEARL_WHITE:
        tpStr = "白";
    	break;
    case LOONG_PEARL_BLUE:
        tpStr = "蓝";
    	break;
    case LOONG_PEARL_YELLOW:
        tpStr = "黄";
    	break;
    case LOONG_PEARL_GREEN:
        tpStr = "绿";
    	break;
    case LOONG_PEARL_PURPLE:
        tpStr = "紫";
    	break;
    default:
    	break;
    }
}

bool SceneUser::putPearlIntoAltar()
{
    if ( !checkLoongPearlAct() )
    {
        return false;
    }

    DWORD pearlType = getFetchedPearlType();
    if ( 0 == pearlType )
    {
        Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你没有接过拆除太庙任务，请先去外国太庙接任务！");
        return false;
    }

    DWORD toCounID = scene->getCountryID();
    if ( toCounID != charbase.country )
    {
        Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "在本国太庙才能交任务！");
		return false;
    }

    CountryTechM::getMe().putPearlIntoAltar(toCounID);

    showCurrentEffect( getUStateByPearlType(pearlType), false ); //soke 关闭特效
    doPearlReward(pearlType);             //soke 处理砖块奖励
    cm.clearCounter(LOONG_PEARL_TYPE); //soke 清除当前夺取的太庙资源品质
    return true;
}

//soke 建设太庙资源
void SceneUser::doPearlReward( DWORD pearlType )
{
	struct tm tm_1;
	time_t timValue = time(NULL);
	zRTime::getLocalTime(tm_1, timValue); 

	if (tm_1.tm_hour==0 && tm_1.tm_min<5)
	{
		Channel::sendSys(this, Cmd::INFO_TYPE_MSG, "太庙系统重新加载中,请稍后再试!");
//		return false;
	}
	else
	{
       if ( pearlType && getFetchedPearlNum() <= 500)  //每天夺取的太庙资源次数
       {
		   if ( charbase.taimCount < 3)
		   {
              //soke （品质*10000*等级*等级）/2
		      /* QWORD exp = (pearlType*10000 * charbase.level * charbase.level)/2;
           
              addExp(exp);
		      DWORD gold = pearlType*10000;
		      charbase.taimCount++;
		      packs.addGold(gold,"建设国家太庙奖励");
              Channel::sendSys(this , Cmd::INFO_TYPE_GAME, "成功建设国家太庙，获得了海量经验%d,金子%d锭", exp, pearlType); */
                charbase.taimCount++;
                switch ( pearlType )
                {
                case Cmd::LOONG_PEARL_WHITE:
                    addBindObjectNum(202314,1,0,2,true);
                    break;
                case Cmd::LOONG_PEARL_BLUE:
                    addBindObjectNum(202315,1,0,2,true);
                    break;
                case Cmd::LOONG_PEARL_YELLOW:
                    addBindObjectNum(202316,1,0,2,true);
                    break;
                case Cmd::LOONG_PEARL_GREEN:
                    addBindObjectNum(202317,1,0,2,true);
                    break;
                case Cmd::LOONG_PEARL_PURPLE:
                    addBindObjectNum(202318,1,0,2,true);
                    break;
                default:
                    break;
                }
                Channel::sendSys(this , Cmd::INFO_TYPE_GAME, "成功建设国家太庙，获得了大量奖励."); 
		   }
		   else
		   {
              addExp(1);
              Channel::sendSys(this , Cmd::INFO_TYPE_GAME, "成功建设国家太庙，获得经验+1");
		   }
       }

	    std::string  info;
        getTypeStrByPearlType(pearlType, info);

	    Channel::sendSys(this , Cmd::INFO_TYPE_MSG, "成功建设太庙资源(%s)",info.c_str());

        DWORD nowPearlNum = CountryTechM::getMe().getAltarPearlNumByCountryID(charbase.country);


       //发国家公告 
	   Channel::sendCountryInfo(scene->getCountryID(), Cmd::INFO_TYPE_GAME, "真嚣张！我国 %s 成功去敌国掠夺了一块太庙资源(%s)，我国太庙资源现增长为%d块！",name, /*toCoun.c_str(),*/info.c_str(), nowPearlNum);
	}

}


void SceneUser::doLoongEffWhenLogin()
{
    DWORD pearlType = getFetchedPearlType();

    if ( pearlType > 0 )
    {
        // this->setUState(getUStateByPearlType(pearlType));
        showCurrentEffect( getUStateByPearlType(pearlType), true );
        ///////////////////////////////开始网管醉梦教网管太庙下线再上无图标 加上这段2024-11-14 00:53:47/////////////////////////////////
        char Buf[200]; 
        bzero(Buf , sizeof(Buf));
        Cmd::stSelectReturnStatesPropertyUserCmd *srs=(Cmd::stSelectReturnStatesPropertyUserCmd*)Buf;
        constructInPlace(srs);
        srs->byType = Cmd::MAPDATATYPE_USER;
        srs->dwTempID = this->tempid;
        srs->size=1;
        srs->states[0].state = getUStateByPearlType(pearlType);
        srs->states[0].result=0;
        srs->states[0].time=0XFFFF;
        this->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
                sizeof(srs->states[0]) * srs->size);
        //selected_lock.lock();
        SelectedSet_iterator iter = selected.begin();
        for(; iter != selected.end() ;)
        {
            SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(*iter);
            if(pUser)
            {
                if(this->scene->checkTwoPosIInNine(this->getPosI() , pUser->getPosI()))
                {
                    pUser->sendCmdToMe(srs , sizeof(Cmd::stSelectReturnStatesPropertyUserCmd) + 
                            sizeof(srs->states[0]) * srs->size);
                    iter ++ ;
                    continue;
                }
            }
            SelectedSet_iterator iter_del = iter;
            iter_del ++;
            selected.erase(iter);
            iter = iter_del;
        }
        ///////////////////////////////结束网管/////////////////////////////////
    }
}

bool SceneUser::checkLoongPearlAct()
{
    DWORD needLv = 90;
    if ( charbase.level < needLv )
    {
        Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你的等级小于%d级，不能做太庙任务！", needLv );
		return false;
    }
	SceneNpc * n = SceneNpcManager::getMe().getNpcByTempID(npc_dwNpcTempID);
	if (!n)
	{
		Zebra::logger->debug("[太庙]%s 夺取太庙资源时，找不到该npc tempID=%u", name, npc_dwNpcTempID);
        Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "无法获取太庙资源……");
		return false;
	}
    if ( !CountryTechM::getMe().isLoongAltarNpc(n->id) )
    {
        Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "必须找到太庙才能掠夺太庙资源……");
		return false;
    }
    if ( abs(pos.x- n->getPos().x) > (SCREEN_WIDTH >> 1) || abs(pos.y-n->getPos().y) > (SCREEN_HEIGHT >> 1))  
    {
        Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,  "你必须去太庙附近！");	
        return false;
    }
    return true;
}

