/**
 * \file
 * \version  $Id: CountryTech.h $
 * \author   
 * \date 
 * \brief 国家科技管理器
 *
 * 
 */

#ifndef _COUNTRY_TECH
#define _COUNTRY_TECH
#include "zTime.h"
#include <time.h>       
#include <sys/time.h> 
#include <vector>
#include <map>
#include "zSingleton.h"
#include "zRWLock.h"
#include "SessionCommand.h"
#include "zSceneEntry.h"

class Scene;
class SceneUser;

//soke 神圣太庙ID
static  const DWORD SENIOR_LOONG_ALTAR_ID = 5245;
//高级太庙ID
static  const DWORD LOONG_ALTAR_ID = 5246;
//初级太庙ID
static  const DWORD SHABBY_LOONG_ALTAR_ID = 5247;
//废弃太庙ID
static  const DWORD BASE_LOONG_ALTAR_ID = 5248;
//太庙pos
//static  const zPos  LOONG_ALTAR_POS = zPos(750,554);
static zPos  LOONG_ALTAR_POS = zPos(760,548);

/*
//高级神龙坛ID
static  const DWORD SENIOR_LOONG_ALTAR_ID = 800001;
//神龙坛ID
static  const DWORD LOONG_ALTAR_ID = 800002;
//残破神龙坛ID
static  const DWORD SHABBY_LOONG_ALTAR_ID = 800003;
//神龙坛底座ID
static  const DWORD BASE_LOONG_ALTAR_ID = 800004;
//神龙坛pos
static  const zPos  LOONG_ALTAR_POS = zPos(316,485);*/
////////////////////////////////////////////////////////

class CTech
{
	public:
		CTech()
		{
			dwType = 0;
			dwLevel = 0;
		}
		
		~CTech(){};
		void init(Cmd::Session::_techItem* rec);
		bool canProduce(DWORD dwObjectLevel);

		DWORD dwType;
		DWORD dwLevel;
};

class CountryTech
{
	public:
		CountryTech(){};
		~CountryTech(){};
		typedef std::map<DWORD, CTech*> CTechMap;

		void init(Cmd::Session::t_updateTech_SceneSession* rev);
		/// 科技索引
		CTechMap techIndex;
		CTech* getTech(DWORD dwType);
		void   addTech(DWORD dwType, CTech* pTech);
		DWORD id;

		zRWLock rwlock;
};

////////////////////////////////////////////////////
/**
 * \brief  每个国家都有自己的太庙
 * 
 */
class LoongAltar
{
public:
    LoongAltar()
    {
        countryID = 0;
        level = 0;
        pearlNum = 0;
    };
    ~LoongAltar(){};

    DWORD countryID;
    DWORD level;
    DWORD pearlNum;
};
/////////////////////////////////////////////////////

class CountryTechM : public Singleton<CountryTechM>
{
	friend class SingletonFactory<CountryTechM>;
	public:
		~CountryTechM(){}

		/**
		 * \brief 初始化
		 *
		 * 初始化该场景所有国家的国家科技信息
		 *
		 */
		void init();
		CountryTech* getCountryTech(DWORD dwCountryID);
		void   addCountryTech(DWORD dwCountryID);
		bool canProduce(DWORD dwCountry, DWORD dwObjectType, DWORD dwObjectLevel);
	
        ////////////////////////////////////////////////////
		///添加太庙
        void addLoongAltar(DWORD countryID, DWORD num);
        ///根据砖块数量召唤太庙
        bool summonLoongAltar(DWORD countryID, DWORD num, bool needReplace);
        ///得到太庙砖块珠数目
        DWORD getAltarPearlNumByCountryID(DWORD countryID);
        ///把砖块放入太庙
        void putPearlIntoAltar(DWORD countryID);
        ///从太庙中取出砖块
        void fetchPearlFromAltar(DWORD countryID);
        
        ///根据砖块数量得到太庙NPC id
        DWORD getAltarNpcIDByPearlNum(DWORD num);
        /**
         * \brief  检查砖块数量变化是否要替换太庙NPC的形象
         * \param oldNum 太庙原来的砖块数量
         * \param newNum 太庙新的砖块数量
         * \param isNotify 是否发国家公告
         */
        bool checkNeedReplaceAltar(DWORD countryID, DWORD oldNum, DWORD newNum, bool isNotify);
        ///发送太庙列表
        void sendLoongAltarList(SceneUser* pUser);
        ///移动砖块动作枚举
        enum MovePearlAction
        {
            MOVE_PEARL_ACTION_NONE   = 0,
            MOVE_PEARL_ACTION_PUT    = 1,           //取出 
            MOVE_PEARL_ACTION_FETCH  = 2,         //放入
        };
        void doMovePearl(DWORD countryID, MovePearlAction act);
        bool isLoongAltarNpc(DWORD npcID) { return npcID >= SENIOR_LOONG_ALTAR_ID && npcID <= BASE_LOONG_ALTAR_ID; }
        ///太庙特效显示
        bool showAltarEff(Scene * scene, DWORD pearlNum, MovePearlAction act);
	/////////////////////////////////////////////////////////
	protected:
		typedef std::map<DWORD, CountryTech*> CountryTechMap;
		/// 科技索引
		CountryTechMap countryTechIndex;
		CountryTechM(){}
		std::map<DWORD, DWORD> TechMap;
		zRWLock rwlock;

        ////////////////////////////////////////////////////
		typedef std::map<DWORD, LoongAltar> CountryAltarMap;   //<countryID, LoongAltar>
        typedef CountryAltarMap::iterator   CountryAltarMapIt;
        CountryAltarMap      counAltarList;
		////////////////////////////////////////////////////
};

#endif

