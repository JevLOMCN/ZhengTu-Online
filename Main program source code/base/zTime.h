/**
 * \file
 * \version  $Id: zTime.h 5751 z $
 * \author  
 * \date 
 * \brief 时间定义
 *
 * 
 */

#ifndef _ZTIME_H_
#define _ZTIME_H_

#include <time.h>
#include <sys/time.h>
#include <sstream>

#include "zType.h"
#include "zMutex.h"
#include "zMisc.h"

/**
 * \brief 真实时间类,对timeval结构简单封装,提供一些常用时间函数
 * 时间精度精确到毫秒，
 * 关于timeval请man gettimeofday
 */
class zRTime
{

	private:

		/**
		 * \brief 真实时间换算为毫秒
		 *
		 */
		unsigned long long _msecs;

		/**
		 * \brief 得到当前真实时间
		 *
		 * \return 真实时间，单位毫秒
		 */
		unsigned long long _now()
		{
			unsigned long long retval = 0LL;
			struct timeval tv;
			gettimeofday(&tv,NULL);
			retval = tv.tv_sec;
			retval *= 1000;
			retval += tv.tv_usec / 1000;
			return retval;
		}

		/**
		 * \brief 得到当前真实时间延迟后的时间
		 * \param delay 延迟，可以为负数，单位毫秒
		 */
		void nowByDelay(int delay)
		{
			_msecs = _now();
			addDelay(delay);
		}

	public:

		/**
		 * \brief 构造函数
		 *
		 * \param delay 相对于现在时间的延时，单位毫秒
		 */
		zRTime(const int delay = 0)
		{
			nowByDelay(delay);
		}

		/**
		 * \brief 拷贝构造函数
		 *
		 * \param rt 拷贝的引用
		 */
		zRTime(const zRTime &rt)
		{
			_msecs = rt._msecs;
		}

		/**
		 * \brief 获取当前时间
		 *
		 */
		void now()
		{
			_msecs = _now();
		}

		/**
		 * \brief 返回秒数
		 *
		 * \return 秒数
		 */
		unsigned long sec() const
		{
			return _msecs / 1000;
		}

		/**
		 * \brief 返回毫秒数
		 *
		 * \return 毫秒数
		 */
		unsigned long msec() const
		{
			return _msecs % 1000;
		}

		/**
		 * \brief 返回总共的毫秒数
		 *
		 * \return 总共的毫秒数
		 */
		unsigned long long msecs() const
		{
			return _msecs;
		}

		/**
		 * \brief 返回总共的毫秒数
		 *
		 * \return 总共的毫秒数
		 */
		void setmsecs(unsigned long long data)
		{
			_msecs = data;
		}

		/**
		 * \brief 加延迟偏移量
		 *
		 * \param delay 延迟，可以为负数，单位毫秒
		 */
		void addDelay(int delay)
		{
			_msecs += delay;
		}

		/**
		 * \brief 重载=运算符号
		 *
		 * \param rt 拷贝的引用
		 * \return 自身引用
		 */
		zRTime & operator= (const zRTime &rt)
		{
			_msecs = rt._msecs;
			return *this;
		}

		/**
		 * \brief 重构+操作符
		 *
		 */
		const zRTime & operator+ (const zRTime &rt)
		{
			_msecs += rt._msecs;
			return *this;
		}

		/**
		 * \brief 重构-操作符
		 *
		 */
		const zRTime & operator- (const zRTime &rt)
		{
			_msecs -= rt._msecs;
			return *this;
		}

		/**
		 * \brief 重构>操作符，比较zRTime结构大小
		 *
		 */
		bool operator > (const zRTime &rt) const
		{
			return _msecs > rt._msecs;
		}

		/**
		 * \brief 重构>=操作符，比较zRTime结构大小
		 *
		 */
		bool operator >= (const zRTime &rt) const
		{
			return _msecs >= rt._msecs;
		}

		/**
		 * \brief 重构<操作符，比较zRTime结构大小
		 *
		 */
		bool operator < (const zRTime &rt) const
		{
			return _msecs < rt._msecs;
		}

		/**
		 * \brief 重构<=操作符，比较zRTime结构大小
		 *
		 */
		bool operator <= (const zRTime &rt) const
		{
			return _msecs <= rt._msecs;
		}

		/**
		 * \brief 重构==操作符，比较zRTime结构是否相等
		 *
		 */
		bool operator == (const zRTime &rt) const
		{
			return _msecs == rt._msecs;
		}

		/**
		 * \brief 计时器消逝的时间，单位毫秒
		 * \param rt 当前时间
		 * \return 计时器消逝的时间，单位毫秒
		 */
		unsigned long long elapse(const zRTime &rt) const
		{
			if (rt._msecs > _msecs)
				return (rt._msecs - _msecs);
			else
				return 0LL;
		}


		///////////////////////////////////////////////////
		/**
		 * \brief 计时器消逝的时间，单位秒
		 * \param  一个以秒计数的时间
		 * \return 计时器消逝的时间，单位秒
		 */
		unsigned long long elapse(const DWORD&  tickSecs) const
		{
			if ( sec() > tickSecs)
				return ( sec() - tickSecs);
			else
				return 0LL;
		}
		///////////////////////////////////////////////////
		static std::string & getLocalTZ(std::string & s);
		static void getLocalTime(struct tm & tv1, time_t timValue)
		{
			timValue +=8*60*60;
			tv1 = *gmtime(&timValue);
		}

};

/**
 * \brief 时间类,对struct tm结构简单封装
 */

class zTime
{

	public:

		/**
		 * \brief 构造函数
		 */
		zTime()
		{
			time(&secs);
			zRTime::getLocalTime(tv, secs);
		}

		/**
		 * \brief 拷贝构造函数
		 */
		zTime(const zTime &ct)
		{
			secs = ct.secs;
			zRTime::getLocalTime(tv, secs);
		}


        /**
        * \brief 拷贝构造函数
        */
        zTime(const DWORD  secs_)
        {
            secs = secs_;
            zRTime::getLocalTime(tv, secs);
        }

		/**
		 * \brief 获取当前时间
		 */
		void now()
		{
			time(&secs);
			zRTime::getLocalTime(tv, secs);
		}

		/**
		 * \brief 返回存储的时间
		 * \return 时间，秒
		 */
		time_t sec() const
		{
			return secs;
		}

		/**
		 * \brief 重载=运算符号
		 * \param rt 拷贝的引用
		 * \return 自身引用
		 */
		zTime & operator= (const zTime &rt)
		{
			secs = rt.secs;
			return *this;
		}

		/**
		 * \brief 重构+操作符
		 */
		const zTime & operator+ (const zTime &rt)
		{
			secs += rt.secs;
			return *this;
		}

		/**
		 * \brief 重构-操作符
		 */
		const zTime & operator- (const zTime &rt)
		{
			secs -= rt.secs;
			return *this;
		}

		/**
		 * \brief 重构-操作符
		 */
		const zTime & operator-= (const time_t s)
		{
			secs -= s;
			return *this;
		}

		/**
		 * \brief 重构>操作符，比较zTime结构大小
		 */
		bool operator > (const zTime &rt) const
		{
			return secs > rt.secs;
		}

		/**
		 * \brief 重构>=操作符，比较zTime结构大小
		 */
		bool operator >= (const zTime &rt) const
		{
			return secs >= rt.secs;
		}

		/**
		 * \brief 重构<操作符，比较zTime结构大小
		 */
		bool operator < (const zTime &rt) const
		{
			return secs < rt.secs;
		}

		/**
		 * \brief 重构<=操作符，比较zTime结构大小
		 */
		bool operator <= (const zTime &rt) const
		{
			return secs <= rt.secs;
		}

		/**
		 * \brief 重构==操作符，比较zTime结构是否相等
		 */
		bool operator == (const zTime &rt) const
		{
			return secs == rt.secs;
		}

		/**
		 * \brief 计时器消逝的时间，单位秒
		 * \param rt 当前时间
		 * \return 计时器消逝的时间，单位秒
		 */
		time_t elapse(const zTime &rt) const
		{
			if (rt.secs > secs)
				return (rt.secs - secs);
			else
				return 0;
		}

		/**
		 * \brief 计时器消逝的时间，单位秒
		 * \return 计时器消逝的时间，单位秒
		 */
		time_t elapse() const
		{
			zTime rt;
			return (rt.secs - secs);
		}

		/**
		 * \brief 得到当前分钟，范围0-59点
		 *
		 * \return 
		 */
		int getSec()
		{
			return tv.tm_sec;
		}
	
		/**
		 * \brief 得到当前分钟，范围0-59点
		 *
		 * \return 
		 */
		int getMin()
		{
			return tv.tm_min;
		}
		
		/**
		 * \brief 得到当前小时，范围0-23点
		 *
		 * \return 
		 */
		int getHour()
		{
			return tv.tm_hour;
		}
		
		/**
		 * \brief 得到天数，范围1-31
		 *
		 * \return 
		 */
		int getMDay()
		{
			return tv.tm_mday;
		}

		/**
		 * \brief 得到当前星期几，范围1-7
		 *
		 * \return 
		 */
		int getWDay()
		{
			return tv.tm_wday;
		}

        /**
		 * \brief 得到一年中的天序号，范围0-365
		 *
		 * \return 
		 */
		int getYDay()
		{
			return tv.tm_yday;
		}

        /**
        * \brief 得到第一年中的周序号，范围1-53
        *  未经验证的算法
        * \return 
        */
        int getYWeek()
        {
            // WEEK_OF_YEAR start
            // Compute the week of the year.  For the Gregorian calendar, valid week
            // numbers run from 1 to 52 or 53, depending on the year, the first day
            // of the week, and the minimal days in the first week.  For other
            // calendars, the valid range may be different -- it depends on the year
            // length.  Days at the start of the year may fall into the last week of
            // the previous year; days at the end of the year may fall into the
            // first week of the next year.  ASSUME that the year length is less than
            // 7000 days.

            DWORD nDay = 0;
            DWORD nWeek = 0;
            DWORD nYear = tv.tm_year;
            for(int i = 1; i <= tv.tm_mon+1; i ++)
            {
                switch(i)
                {
                case 1:
                case 3:
                case 5:
                case 7:
                case 8:
                case 10:
                case 12:
                    nDay += 31;
                    break;
                case 4:
                case 6:
                case 9:
                case 11:
                    nDay += 30;
                    break;
                case 2:
                    if(nYear % 4 == 0 && nYear % 100 != 0 || nYear % 400 == 0)
                    {
                        nDay += 29;
                    }
                    else
                    {
                        nDay += 28;
                    }
                    break;
                default:
                    break;
                }
               
            }
            nWeek = nDay / 7 + 1; 
            return static_cast<int>(nWeek);
        }

		/**
		 * \brief 得到当前月份，范围1-12
		 *
		 * \return 
		 */
		int getMonth()
		{
			return tv.tm_mon+1;
		}
		
		/**
		 * \brief 得到当前年份
		 *
		 * \return 
		 */
		int getYear()
		{
			return tv.tm_year+1900;
		}	

	private:

		/**
		 * \brief 存储时间，单位秒
		 */
		time_t secs;
		
		/**
		 * \brief tm结构，方便访问
		 */
		struct tm tv;


};

class Timer
{
	public:
		Timer(const float how_long , const int delay=0) : _long((int)(how_long*1000)), _timer(delay*1000)
		{

		}
		Timer(const float how_long , const zRTime cur) : _long((int)(how_long*1000)), _timer(cur)
		{
			_timer.addDelay(_long);
		}
		void next(const zRTime &cur)
		{
			_timer=cur;
			_timer.addDelay(_long);
		} 
		bool operator() (const zRTime& current)
		{
			if (_timer <= current) {
				_timer = current;
				_timer.addDelay(_long);
				return true;
			}

			return false;
		}
	private:
		int _long;
		zRTime _timer;
};

//时间间隔具有随机性
class RandTimer
{
	public:
#define next_time(_long) (_long / 2 + zMisc::randBetween(0, _long))
		RandTimer(const float how_long , const int delay=0) : _long((int)(how_long*1000)), _timer(delay*1000)
		{

		}
		RandTimer(const float how_long , const zRTime cur) : _long((int)(how_long*1000)), _timer(cur)
		{
			_timer.addDelay(next_time(_long));
		}
		void next(const zRTime &cur)
		{
			_timer=cur;
			_timer.addDelay(next_time(_long));
		} 
		bool operator() (const zRTime& current)
		{
			if (_timer <= current) {
				_timer = current;
				_timer.addDelay(next_time(_long));
				return true;
			}

			return false;
		}
	private:
		int _long;
		zRTime _timer;
};

struct FunctionInterval
{
	struct timeval _tv_1;
	struct timeval _tv_2;
	const int _need_log;
	const char *_fun_name;
	FunctionInterval(const int interval):_need_log(interval)
	{
		_tv_1.tv_sec=-1;
		_tv_1.tv_usec=-1;
	}
	void interval(const char *func=NULL);
};
struct FunctionTime
{
	private:
	struct timeval _tv_1;
	struct timeval _tv_2;
	const int _need_log;
	const char *_fun_name;
	const char *_dis;
	const int _dis_len;
	public:
	FunctionTime(const int interval,const char *func=NULL,const char *dis=NULL,const int dis_len=16):_need_log(interval),_fun_name(func),_dis(dis),_dis_len(dis_len)
	{
		gettimeofday(&_tv_1,NULL);
	}
	~FunctionTime();
};
struct FunctionTimes
{
	struct Times
	{
		//Times();
		Times():_log_timer(600),_times(0),_total_time(0)
		{
			bzero(_dis,sizeof(_dis));
		}
		Timer _log_timer;
		char _dis[256];
		int _times;
		int _total_time;
		zMutex _mutex;
	};
	private:
	static Times _times[256]; 
	int _which;
	struct timeval _tv_1;
	struct timeval _tv_2;
	public:
	FunctionTimes(const int which , const char *dis);
	~FunctionTimes();
};
/*
struct CmdAnalysis
{
	CmdAnalysis(const char *disc,DWORD time_secs):_log_timer(time_secs)
	{
		bzero(_disc,sizeof(disc));
		strncpy(_disc,disc,sizeof(_disc)-1);
		bzero(_data,sizeof(_data));
		_switch=false;
	}
	struct
	{
		DWORD num;
		DWORD size;
	}_data[256][256] ;
	zMutex _mutex;
	Timer _log_timer;
	char _disc[256];
	bool _switch;//开关
	void add(const BYTE &cmd, const BYTE &para , const DWORD &size)
	{
		if(!_switch)
		{
			return ;
		}
		_mutex.lock(); 
		_data[cmd][para].num++;
		_data[cmd][para].size +=size;
		zRTime ct;
		if(_log_timer(ct))
		{
			for(int i = 0 ; i < 256 ; i ++)
			{
				for(int j = 0 ; j < 256 ; j ++)
				{
					if(_data[i][j].num)
						Zebra::logger->debug("%s:%d,%d,%d,%d",_disc,i,j,_data[i][j].num,_data[i][j].size);
				}
			}
			bzero(_data,sizeof(_data));
		}
		_mutex.unlock(); 
	}
};
// */
/*
struct FunctionTimes
{
	private:
		int _times;
		Timer _timer;
	public:
		FunctionTimes():_times(0),_timer(60)
		{
		}
		void operator();
		
};
// */
#endif
