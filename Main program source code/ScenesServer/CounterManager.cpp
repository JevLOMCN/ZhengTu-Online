/**
 *  ============================================================================
 * \file	   		 CounterManager.cpp
 * \author		     WANQIHU , <wanqihu@gmail.com>
 * \date   		     2010年5月19日
 * \version  	     
 * \brief    		 计数器，持久化在在数据库中
 *  
 *  ============================================================================
 */
#include "CounterManager.h"

#include "TimeTick.h"

QWORD CounterManager::getCounter( DWORD id )
{
	CounterMapIter it = counterMap.find(id);
	if ( it != counterMap.end() )
	{
		DWORD     curTime = SceneTimeTick::currentTime.sec(); 
		zTime     now(curTime);
		zTime	  saveTime(it->second.saveTime);

		it->second.saveTime = curTime;

        bool needReset = false;
		switch(it->second.type)
		{
		case COUNTER_CHANGE_NEVER:
			{
			}break;
		case COUNTER_CHANGE_DAY:
			{
				if ( now.getYDay() != saveTime.getYDay() )
				{
                    needReset = true;
				}
			}break;
		case COUNTER_CHANGE_WEEK:
			{
				if ( now.getYWeek() != saveTime.getYWeek() )
				{
                    needReset = true;
				}
			}break;
		case COUNTER_CHANGE_MONTH:
			{
				if ( now.getMonth() != saveTime.getMonth() )
				{
                    needReset = true;
				}

			}break;
		case COUNTER_CHANGE_YEAR:
			{
				if ( now.getYear() != saveTime.getYear() )
				{
                    needReset = true;
				}
			}break;
		default:
			break;
		}
        if ( needReset )
        {
            it->second.num = 0;
            it->second.data1 = 0;
            it->second.data2 = 0;
        }
		return it->second.num;
	}
	return 0;
}

QWORD CounterManager::getCounter( DWORD id, DWORD &data1, DWORD &data2 )
{
	CounterMapIter it = counterMap.find(id);
	if ( it != counterMap.end() )
	{
		DWORD     curTime = SceneTimeTick::currentTime.sec(); 
		zTime     now(curTime);
		zTime	  saveTime(it->second.saveTime);

		it->second.saveTime = curTime;

        bool needReset = false;
		switch(it->second.type)
		{
		case COUNTER_CHANGE_NEVER:
			{
			}break;
		case COUNTER_CHANGE_DAY:
			{
				if ( now.getYDay() != saveTime.getYDay() )
				{
                    needReset = true;
				}
			}break;
		case COUNTER_CHANGE_WEEK:
			{
				if ( now.getYWeek() != saveTime.getYWeek() )
				{
                    needReset = true;
				}
			}break;
		case COUNTER_CHANGE_MONTH:
			{
				if ( now.getMonth() != saveTime.getMonth() )
				{
                    needReset = true;
				}

			}break;
		case COUNTER_CHANGE_YEAR:
			{
				if ( now.getYear() != saveTime.getYear() )
				{
                    needReset = true;
				}
			}break;
		default:
			break;
		}
        if ( needReset )
        {
            it->second.num = 0;
            it->second.data1 = 0;
            it->second.data2 = 0;
        }
        data1 = it->second.data1;
        data2 = it->second.data2;
		return it->second.num;
	}
	return 0;
}

bool CounterManager::addCounter( DWORD id, QWORD num /*= 1*/, DWORD type /*= COUNTER_CHANGE_NEVER*/, DWORD data1 /*= 0*/, DWORD data2 /*= 0*/)
{
	CounterMapIter it = counterMap.find(id);
	if ( it != counterMap.end() )
	{
        DWORD dt1 = 0;
        DWORD dt2 = 0;
        getCounter(id, dt1, dt2);
		it->second.num += num;
		it->second.data1 += data1;
		it->second.data2 += data2;
		it->second.saveTime = SceneTimeTick::currentTime.sec();
		if ( it->second.type != type )
		{
			it->second.type = type;
		}
	}
	else
	{
		CounterMember memb;
		memb.version = COUNTER_MEMB_VER_1;
		memb.id = id;
		memb.type = type;
		memb.num = num;
		memb.saveTime = SceneTimeTick::currentTime.sec();
        memb.data1 = data1;
        memb.data2 = data2;
		counterMap[id] = memb;
	}
	return true;
}

bool CounterManager::clearCounter( DWORD id )
{
	CounterMapIter it = counterMap.find(id);
	if ( it != counterMap.end() )
	{
		it->second.num = 0;
        it->second.data1 = 0;
        it->second.data2 = 0;
		it->second.saveTime = SceneTimeTick::currentTime.sec();
	}
	return true;
}


bool CounterManager::addCounterDay( DWORD id, QWORD num /*= 1*/, DWORD data1 /*= 0*/, DWORD data2 /*= 0*/ )
{
	return addCounter(id, num, COUNTER_CHANGE_DAY, data1, data2);
}

bool CounterManager::addCounterWeek( DWORD id, QWORD num /*= 1*/, DWORD data1 /*= 0*/, DWORD data2 /*= 0*/ )
{
	return addCounter(id, num, COUNTER_CHANGE_WEEK, data1, data2);
}

bool CounterManager::addCounterMonth( DWORD id, QWORD num /*= 1*/, DWORD data1 /*= 0*/, DWORD data2 /*= 0*/ )
{
	return addCounter(id, num, COUNTER_CHANGE_MONTH, data1, data2);
}

bool CounterManager::addCounterYear( DWORD id, QWORD num /*= 1*/, DWORD data1 /*= 0*/, DWORD data2 /*= 0*/ )
{
	return addCounter(id, num, COUNTER_CHANGE_YEAR, data1, data2);
}

DWORD CounterManager::save( char* to )
{
    DWORD size = 0;
    DWORD count = counterMap.size();
    *(DWORD*)to = count;
    
    size += sizeof(DWORD);
    for (CounterMapIter it = counterMap.begin(); it != counterMap.end(); ++it )
    {
        bcopy(&(it->second), to+size, sizeof(CounterMember) );
        size += sizeof(CounterMember);
    }
    return size;
}

bool CounterManager::load( char* from )
{
    DWORD size = 0;
	DWORD count = *(DWORD*)from;
    
    size += sizeof(DWORD);
    while (count--)
    {
		CounterMember memb;
        bcopy(from+size, &memb, sizeof(CounterMember));
        size += sizeof(CounterMember);
		counterMap[memb.id] = memb;
    }
	return size;
}


