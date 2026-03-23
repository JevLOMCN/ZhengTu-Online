#include "zLogger.h"
#include <stdarg.h>
#define getMessage(msg,msglen,pat)	\
do	\
{	\
	va_list ap;	\
	bzero(msg, msglen);	\
	va_start(ap, pat);		\
	vsnprintf(msg, msglen - 1, pat, ap);	\
	va_end(ap);	\
}while(false)
char message[1024];

/*
int main()
{
	zLogger *logger=new zLogger("WS");
	int size=0;
	while(size < 1000000)
	{
		char buf[]= "---------- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx ----------";
		for(int i=0 ; i < 100 ; i ++)
		{
			logger->debug("%s",buf);
		}
		size += 1;
		logger->debug("log´ÎÊý%d",size + 1);
		::usleep(1);
	}
	return true;
}
// */
#include <stdlib.h>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/ndc.h>

using namespace log4cxx;
using namespace log4cxx::helpers;

bool getLog(LoggerPtr rootLogger , const char * pattern, ...)
{
	getMessage(message,sizeof(message) , pattern);
	rootLogger->debug(message);/*(_T("debug message"));*/
	return true;
}
bool getLog1(LoggerPtr rootLogger , const char * pattern)
{
	//getMessage(message,sizeof(message) , pattern);
	//rootLogger->debug(message);/*(_T("debug message"));*/
	rootLogger->debug(pattern);/*(_T("debug message"));*/
	return true;
}
int main()
{
	bzero(message, sizeof(message));
	int result = EXIT_SUCCESS;
	try
	{
		BasicConfigurator::configure();
		int size=0;
		while(size < 1000000)
		{
			LoggerPtr rootLogger = Logger::getRootLogger();

			//NDC::push(_T("trivial context"));
			//getLog(rootLogger , "xxxxxxxxxxxxxx%d",size);
			//getLog1(rootLogger , "xxxxxxxxxxxxxx");

			rootLogger->debug(_T("debug message"));
			rootLogger->info(_T("info message"));
			rootLogger->warn(_T("warn message"));
			rootLogger->error(_T("error message"));
			rootLogger->fatal(_T("fatal message"));
			size ++;
		}
	}
	catch(Exception&)
	{
		result = EXIT_FAILURE;
	}

	return result;
}
