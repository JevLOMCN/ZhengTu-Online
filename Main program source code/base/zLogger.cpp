/**
 * \file
 * \version  $Id: zLogger.cpp  $
 * \author  
 * \date 
 * \brief Zebra项目日志系统定义文件
 *
 */

#include "zLogger.h"
#include "zTime.h"
#include <stdarg.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/helpers/dateformat.h>
#include <log4cxx/net/syslogappender.h>
#include <sys/stat.h>
#include <time.h>

using namespace log4cxx::net;

const LevelPtr zLogger::zLevel::LEVELALARM(new Level(ALARM_INT,_T("ALARM"),3));
const LevelPtr zLogger::zLevel::LEVELIFFY(new Level(IFFY_INT,_T("IFFY"),3));
const LevelPtr zLogger::zLevel::LEVELTRACE(new Level(TRACE_INT,_T("TRACE"),3));
const LevelPtr zLogger::zLevel::LEVELGBUG(new Level(GBUG_INT,_T("GBUG"),3));

const zLogger::zLevel *  zLogger::zLevel::OFF=new zLevel(Level::OFF);
const zLogger::zLevel *  zLogger::zLevel::FATAL=new zLevel(Level::FATAL);
const zLogger::zLevel *  zLogger::zLevel::ALARM=new zLevel(LEVELALARM);
const zLogger::zLevel *  zLogger::zLevel::ERROR=new zLevel(Level::ERROR);
const zLogger::zLevel *  zLogger::zLevel::IFFY=new zLevel(LEVELIFFY);
const zLogger::zLevel *  zLogger::zLevel::WARN=new zLevel(Level::WARN);
const zLogger::zLevel *  zLogger::zLevel::TRACE=new zLevel(LEVELTRACE);
const zLogger::zLevel *  zLogger::zLevel::INFO=new zLevel(Level::INFO);
const zLogger::zLevel *  zLogger::zLevel::GBUG=new zLevel(LEVELGBUG);
const zLogger::zLevel *  zLogger::zLevel::DEBUG=new zLevel(Level::DEBUG);
const zLogger::zLevel *  zLogger::zLevel::ALL=new zLevel(Level::ALL);

/**
 * \brief zLevel构造函数
 * \param  level 等级数字，类内部定义
 */
zLogger::zLevel::zLevel(LevelPtr level):zlevel(level)
{
}


TimeZonePtr zLogger::zLoggerLocalFileAppender::tz(TimeZone::getDefault());

/**
 * \brief 构造一个本地文件Appender 
 */
zLogger::zLoggerLocalFileAppender::zLoggerLocalFileAppender()
{
}

/**
 * \brief 析构时，回收DateFormat内存
 */
zLogger::zLoggerLocalFileAppender::~zLoggerLocalFileAppender()
{
	SAFE_DELETE(df);
}

/**
 * \brief 设置时区
 *
 * \param timeZone 时区字符串 
 */
void zLogger::zLoggerLocalFileAppender::setTimeZone(const String &timeZone)
{
	tz=TimeZone::getTimeZone(timeZone);
}

/**
 * \brief 激活所设置的选项
 */
void zLogger::zLoggerLocalFileAppender::activateOptions()
{
	//设置时区和预定文件
	rc.setTimeZone(tz);
	String ss;
	zRTime::getLocalTZ(ss);
	char tzstr[64];
	strncpy(tzstr,"TZ=",3);
	strncpy(tzstr+3,ss.c_str(),60);
	DailyRollingFileAppender::activateOptions();
	//由于DailyRollingFileAppender::activateOptions()会修改时区环境变量，故执行完后再修改回来 
	putenv(tzstr);
	tzset();
	if (!datePattern.empty() && !fileName.empty())
	{
		SAFE_DELETE(df);
		df=new helpers::DateFormat(datePattern,tz);

		int64_t lastModified = 0;
		struct stat fileStats;
		if (::stat(T2A(fileName.c_str()), &fileStats) == 0)
			lastModified = (int64_t)fileStats.st_mtime * 1000; 

		scheduledFilename = fileName + df->format(lastModified);
	} 
}

/**
 * \brief 构造一个zLogger 
 *
 * \param  name zLogger的名字，将会出现在输出的日志中的每一行
 */
zLogger::zLogger(const String &name)
{
	///std::cout << __PRETTY_FUNCTION__ << std::endl;
	bzero(message, sizeof(message));
	logger = Logger::getLogger(name);
	logger->setLevel(Level::DEBUG);

	addConsoleLog();
}

/**
 * \brief 析构函数
 */
zLogger::~zLogger()
{
}

/**
 * \brief 得到Logger的名字，它出现在每条日志信息中
 * \return	Logger名字
 */
const String & zLogger::getName()
{
	return logger->getName();
}

/**
 * \brief 设置Logger的名字，它出现在每条日志信息中
 * \param 要被设置的名字
 */
void zLogger::setName(const String & setName)
{
    logger = Logger::getLogger(setName);
}
/**
 * \brief 添加控制台输出Log
 * \return	成功返回true，否则返回false 
 */
bool zLogger::addConsoleLog()
{
	PatternLayoutPtr cpl = new PatternLayout("\%d{\%y\%m\%d-\%H:\%M:\%S }%c %5p: %m%n");
	std::string s;
	cpl->setTimeZone(zRTime::getLocalTZ(s));
	cpl->activateOptions();

	ConsoleAppenderPtr carp = new ConsoleAppender(cpl);
	carp->setName("console");

	logger->addAppender(carp);
	return true;
}

/**
 * \brief 移除控制台Log输出
 */
void zLogger::removeConsoleLog()
{
	AppenderPtr ap=logger->getAppender("console");
	logger->removeAppender(ap);
	ap->close();
}

/**
 * \brief 加一个本地文件Log输出
 *
 * \param file 要输出的文件名，Logger会自动地添加时间后缀 
 * \return 成功返回true，否则返回false
 */
bool zLogger::addLocalFileLog(const String &file)
{
	PatternLayoutPtr fpl = new PatternLayout("\%d{\%y\%m\%d-\%H:\%M:\%S }%c %5p: %m%n");
	String s;
	fpl->setTimeZone(zRTime::getLocalTZ(s));
	fpl->activateOptions();

	zLoggerLocalFileAppender * farp = new  zLoggerLocalFileAppender();
	farp->setDatePattern(".\%y\%m\%d-\%H");
	farp->setTimeZone(s);
	farp->setFile(file);
	farp->setLayout(fpl);
	farp->activateOptions();
	farp->setName("localfile:"+file);

	logger->addAppender(farp);
	return true;
}

/**
 * \brief 移出一个本地文件Log输出
 * \param file 要移除的Log文件名 
 */
void zLogger::removeLocalFileLog(const String &file)
{
	AppenderPtr ap=logger->getAppender("localfile:"+file);
	logger->removeAppender(ap);
	ap->close();
}

/**
 * \brief 添加一个Syslog输出
 * \param host Syslog服务器地址,默认为本主机
 * \return 成功返回true，否则返回false
 */
bool zLogger::addSysLog(const String & host="127.0.0.1")
{
	PatternLayoutPtr pls = new PatternLayout("%c %5p: %m%n");
	SyslogAppenderPtr sarp=new SyslogAppender(pls,SyslogAppender::getFacility("user"));
	sarp->setName(host);
	sarp->setSyslogHost (host);
	sarp->activateOptions();
	logger->addAppender(sarp);
	return true;
}

/**
 * \brief 移除一个Syslog输出
 * \param host 要移除的Syslog服务器地址,默认为本主机
 */
void zLogger::removeSysLog(const String & host="127.0.0.1")
{
	AppenderPtr ap=logger->getAppender(host);
	logger->removeAppender(ap);
	ap->close();
}

#define getMessage(msg,msglen,pat)	\
do	\
{	\
	va_list ap;	\
	bzero(msg, msglen);	\
	va_start(ap, pat);		\
	vsnprintf(msg, msglen - 1, pat, ap);	\
	va_end(ap);	\
}while(false)

/**
 * \brief 设置写日志等级
 * \param  zLevelPtr 日志等级.参见 #zLogger::zLevel
 */
void zLogger::setLevel(const zLevel * zLevelPtr)
{
	logger->setLevel(zLevelPtr->zlevel);
}

/**
 * \brief 设置写日志等级
 * \param  level 日志等级
 */
void zLogger::setLevel(const std::string &level)
{
	if ("off" == level)
		setLevel(zLevel::OFF);
	else if ("fatal" == level)
		setLevel(zLevel::FATAL);
	else if ("alarm" == level)
		setLevel(zLevel::ALARM);
	else if ("error" == level)
		setLevel(zLevel::ERROR);
	else if ("iffy" == level)
		setLevel(zLevel::IFFY);
	else if ("warn" == level)
		setLevel(zLevel::WARN);
	else if ("trace" == level)
		setLevel(zLevel::TRACE);
	else if ("info" == level)
		setLevel(zLevel::INFO);
	else if ("gbug" == level)
		setLevel(zLevel::GBUG);
	else if ("debug" == level)
		setLevel(zLevel::DEBUG);
	else if ("all" == level)
		setLevel(zLevel::ALL);
}

/**
 * \brief 写日志
 * \param  zLevelPtr 日志等级参见 #zLogger::zLevel
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLogger::log(const zLevel * zLevelPtr,const char * pattern, ...)
{
	msgMut.lock();
	getMessage(message,MSGBUF_MAX,pattern);

	logger->log(zLevelPtr->zlevel,message);
	msgMut.unlock();
	return true;
}

/**
 * \brief 强制写日志,不受日志等级限制
 * \param  zLevelPtr 日志等级参见 #zLogger::zLevel
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLogger::forceLog(const zLevel * zLevelPtr,const char * pattern, ...)
{
	msgMut.lock();
	getMessage(message,MSGBUF_MAX,pattern);

	logger->forcedLog(zLevelPtr->zlevel,message);
	msgMut.unlock();
	return true;
}

/**
 * \brief 写fatal程序日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLogger::fatal(const char * pattern, ...)
{
	msgMut.lock();
	getMessage(message,MSGBUF_MAX,pattern);

	logger->fatal(message);
	msgMut.unlock();
	return true;
}

/**
 * \brief 写error程序日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLogger::error(const char * pattern, ...)
{
	msgMut.lock();
	getMessage(message,MSGBUF_MAX,pattern);

	logger->error(message);
	msgMut.unlock();
	return true;
}

/**
 * \brief 写error程序日志
 * \param  buf 输出内容
 * \return 成功返回true，否则返回false
 */
bool zLogger::error_out(const char * buf)
{
	msgMut.lock();
	logger->error(buf);
	msgMut.unlock();
	return true;
}

/**
 * \brief 写warn程序日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLogger::warn(const char * pattern, ...)
{
	msgMut.lock();
	getMessage(message,MSGBUF_MAX,pattern);

	logger->warn(message);
	msgMut.unlock();
	return true;
}

/**
 * \brief 写info程序日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLogger::info(const char * pattern, ...)
{
	msgMut.lock();
	getMessage(message,MSGBUF_MAX,pattern);

	logger->info(message);
	msgMut.unlock();
	return true;
}

/**
 * \brief 写debug程序日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLogger::debug(const char * pattern, ...)
{
	msgMut.lock();
	getMessage(message,MSGBUF_MAX,pattern);

	logger->debug(message);
	msgMut.unlock();
	return true;
}

/**
 * \brief 写alarm游戏日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLogger::alarm(const char * pattern, ...)
{
	msgMut.lock();
	getMessage(message,MSGBUF_MAX,pattern);

	logger->log(zLevel::ALARM->zlevel,message);
	msgMut.unlock();
	return true;
}

/**
 * \brief 写iffy游戏日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLogger::iffy(const char * pattern, ...)
{
	msgMut.lock();
	getMessage(message,MSGBUF_MAX,pattern);

	logger->log(zLevel::IFFY->zlevel,message);
	msgMut.unlock();
	return true;
}

/**
 * \brief 写trace游戏日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLogger::trace(const char * pattern, ...)
{
	msgMut.lock();
	getMessage(message,MSGBUF_MAX,pattern);

	logger->log(zLevel::TRACE->zlevel,message);
	msgMut.unlock();
	return true;
}

/**
 * \brief 写gbug游戏日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLogger::gbug(const char * pattern, ...)
{
	msgMut.lock();
	getMessage(message,MSGBUF_MAX,pattern);

	logger->log(zLevel::GBUG->zlevel,message);
	msgMut.unlock();
	return true;
}
