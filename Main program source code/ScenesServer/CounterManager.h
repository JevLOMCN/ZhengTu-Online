/**
 *  ============================================================================
 * \file	   		 CounterManager.h
 * \author		     WANQIHU , <wanqihu@gmail.com>
 * \date   		     2010年5月19日  
 * \version  	     
 * \brief    		 计数器管理器 
 * \desc             使用说明
 *                    1 此管理器可以持久化N个数据（1=<N<=3, 如一个三元组<t1,t2,t3>)到一段二进制缓冲区中。目前用在SceneUser中
 *                    2 数据存储单位时间（如天、月）后可重置清零  
 *  ============================================================================
 */
#ifndef COUNTERMANAGER_H_
#define COUNTERMANAGER_H_

#include "zType.h"

#include <map>

#pragma  pack(1)

///计数器切换类型枚举
enum CounterChangeType
{
	COUNTER_CHANGE_NONE		= 0,  ///< 切换类型
	COUNTER_CHANGE_NEVER	= 1,  ///< 切换类型, 不切换
	COUNTER_CHANGE_DAY		= 2,  ///< 切换类型，按天切换，过天计数器清零
	COUNTER_CHANGE_WEEK		= 3,  ///< 切换类型，按周切换，过周计数器清零
	COUNTER_CHANGE_MONTH	= 4,  ///< 切换类型，按月切换，过月计数器清零
	COUNTER_CHANGE_YEAR		= 5,  ///< 切换类型，按年切换，过年计数器清零

};

/// 计数器成员数据结构
struct CounterMember
{
	BYTE	version;     ///< 版本号
	DWORD	type;		 ///< 计数器切换类型
	DWORD	id;		     ///< 计数器ID
	QWORD	num;		 ///< 计数器保存的值
	DWORD	saveTime;	 ///< 计数器保存的时间
	DWORD	data1;	     ///< 计数器作其它用的数据
	DWORD	data2;	     ///< 计数器作其它用的数据
    CounterMember()
    {
        version = 0;
        type = 0;
        id = 0;
        num = 0;
        saveTime = 0;
        data1 = 0;
        data2 = 0;
    }
};

/// 计数器成员结构版本号
enum CounterMemberVersion
{
	COUNTER_MEMB_VER_1 = 0,   //第一个版本
};

///计数器成员类型枚举
enum CounterMemberType
{
	COUNTER_MEMBTYPE_NONE       = 0,            ///<无
	TODAY_GAME_TIME             = 1,            ///<今日游戏时间
    LOONG_PEARL_COUNT           = 2,            ///<每天夺取的太庙资源计数
    LOONG_PEARL_TYPE            = 3,            ///<当前夺取的太庙资源品质
    SPA_TIME_IN_WANGCHENG       = 4,            ///<今日王城温泉时间计数,以秒计
	LOONG2_PEARL_COUNT          = 5,            ///<当前换取的太庙资源品质
   
    FUBEN_TEYP_1                = 6,            ///<副本类型，在记数器中的记录数据包括：当前进入副本TempID,开启次数data1,进入次数data2
    FUBEN_TEYP_2                = 7,            ///<副本类型，在记数器中的记录数据包括：当前进入副本TempID,开启次数data1,进入次数data2
    FUBEN_TEYP_3                = 8,            ///<副本类型，在记数器中的记录数据包括：当前进入副本TempID,开启次数data1,进入次数data2
    FUBEN_TEYP_4                = 9,            ///<副本类型，在记数器中的记录数据包括：当前进入副本TempID,开启次数data1,进入次数data2
    FUBEN_TEYP_5                = 10,           ///<副本类型，在记数器中的记录数据包括：当前进入副本TempID,开启次数data1,进入次数data2
    FUBEN_TEYP_6                = 11,           ///<副本类型，在记数器中的记录数据包括：当前进入副本TempID,开启次数data1,进入次数data2
    FUBEN_TEYP_7                = 12,           ///<副本类型，在记数器中的记录数据包括：当前进入副本TempID,开启次数data1,进入次数data2
    FUBEN_TEYP_8                = 13,           ///<副本类型，在记数器中的记录数据包括：当前进入副本TempID,开启次数data1,进入次数data2
    FUBEN_TEYP_9                = 14,           ///<副本类型，在记数器中的记录数据包括：当前进入副本TempID,开启次数data1,进入次数data2
    FUBEN_TEYP_10               = 15,           ///<副本类型，在记数器中的记录数据包括：当前进入副本TempID,开启次数data1,进入次数data2
    FUBEN_TEYP_11               = 16,           ///<副本类型，在记数器中的记录数据包括：当前进入副本TempID,开启次数data1,进入次数data2
    FUBEN_TEYP_12               = 17,           ///<副本类型，在记数器中的记录数据包括：当前进入副本TempID,开启次数data1,进入次数data2
    FUBEN_TEYP_13               = 18,           ///<副本类型，在记数器中的记录数据包括：当前进入副本TempID,开启次数data1,进入次数data2
    FUBEN_TEYP_14               = 19,           ///<副本类型，在记数器中的记录数据包括：当前进入副本TempID,开启次数data1,进入次数data2
    FUBEN_TEYP_15               = 20,           ///<副本类型，在记数器中的记录数据包括：当前进入副本TempID,开启次数data1,进入次数data2
	JIAZU_TIME_IN_WANGCHENG     = 21,            ///<今日家族地图打坐时间计数,以秒计
    COUNTER_MEMBER_MAX_SIZE,
};




#pragma pack() // 取消1字节对齐方




#include "zNoncopyable.h"

class CounterManager : public zNoncopyable 
{
public:
    CounterManager() {};
    ~CounterManager(){};
	
	/**
	 * \brief  从管理器中取出指定id的计数值保存的值 
	 *
	 */
	QWORD getCounter(DWORD id);

	/**
	 * \brief  从管理器中取出指定id的计数值保存的值,并取出data1和data2的值
     * \param  [in] id 指定的计数器id
     * \param  [out] data1 返回指定的计数器数据结构中保存的data1
     * \param  [out] data2 返回指定的计数器数据结构中保存的data2
	 */
	QWORD getCounter(DWORD id, DWORD &data1, DWORD &data2);

	/**
	 * \brief  向管理器中增加一个计数器，默认为不切换类型,增加的数据包括data1和data2
	 * 调用者应该注意，如调用AddCountWeek之后再调用AddCountryDay对同一ID进行操作，计数器成员类型会随之做出改变
	 */
	bool addCounter(DWORD id, QWORD num = 1, DWORD type = COUNTER_CHANGE_NEVER, DWORD data1 = 0, DWORD data2 = 0);

	/**
	 * \brief  对计数器保存的值进行清零操作 
	 *
	 */
	bool clearCounter(DWORD id);

	/**
	 * \brief  对计数器保存的值进行减操作 
	 *
	 */
	bool subCounter(DWORD id, QWORD num = 1);

	/**
	 * \brief  向管理器中增加一个以天切换的计数器
	 *
	 */
    bool addCounterDay(DWORD id, QWORD num = 1, DWORD data1 = 0, DWORD data2 = 0);

	/**
	 * \brief  向管理器中增加一个以周切换的计数器 目前未实现
	 *
	 */
    bool addCounterWeek(DWORD id, QWORD num = 1, DWORD data1 = 0, DWORD data2 = 0);

	/**
	 * \brief  向管理器中增加一个以月切换的计数器
	 *
	 */
    bool addCounterMonth(DWORD id, QWORD num = 1, DWORD data1 = 0, DWORD data2 = 0);

	/**
	 * \brief  向管理器中增加一个以年切换的计数器
	 *
	 */
    bool addCounterYear(DWORD id, QWORD num = 1, DWORD data1 = 0, DWORD data2 = 0);

	/**
	 * \brief  将管理器中的所有计数器存储至二进制缓冲区 
	 *
	 */
	DWORD save(char* to);
	/**
	 * \brief  从二进制中缓冲区加载计数器 
	 *
	 */
	bool  load(char* from);

private:
	typedef std::map<DWORD, CounterMember> CounterMap;
	typedef CounterMap::iterator		   CounterMapIter;

	CounterMap			counterMap;
};



#endif    // COUNTERMANAGER_H_



