#include "zLogger.h"
#include "Zebra.h"
#include "zUrl.h"
#include "zJpeg.h"
#include "zTime.h"
#include <map>
#include <ext/hash_map>

int main(int argc,char *argv[])
{
	Zebra::logger=new zLogger("test");
	const int max_size = 30000;
	const int recount = 100;
	for(int j = 0; j < 3; j++)
	{
		{
			__gnu_cxx::hash_map<int, int> map;
			for(int i = 0; i < max_size; i++)
				map[i] = i;
			FunctionTime func_time(0,__PRETTY_FUNCTION__,"hash_map" , 32);
			for(int i = 0; i < max_size * recount; i++)
				map.find(i);
		}
		{
			__gnu_cxx::hash_map<int, int> map(max_size);
			for(int i = 0; i < max_size; i++)
				map[i] = i;
			FunctionTime func_time(0,__PRETTY_FUNCTION__,"hash_map(max_size)" , 32);
			for(int i = 0; i < max_size * recount; i++)
				map.find(i);
		}
		{
			std::map<int, int> map;
			for(int i = 0; i < max_size; i++)
				map[i] = i;
			FunctionTime func_time(0,__PRETTY_FUNCTION__,"map" , 32);
			for(int i = 0; i < max_size * recount; i++)
				map.find(i);
		}
	}
	Zebra::logger->addConsoleLog();
	//*
	zLogger *zlog=new zLogger("log");
	zlog->addConsoleLog();
	zlog->addLocalFileLog("Zebra.log");
	//zlog->addSysLog();
	zlog->debug("sdfsfdsfds");
	zlog->log(zLogger::zLevel::GBUG,"%d()%s",1,"sdfsfdsfds");
	zlog->warn("%d()%s",1,"sdfsfdsfds");
	zlog->log(zLogger::zLevel::FATAL,"%d()%s",1,"sdfsfdsfds");
	zlog->log(zLogger::zLevel::INFO,"%d()%s",1,"sdfsfdsfds");

	char in[] = "&ÄãºÃ&";
	int out_len;
	char *out = Zebra::url_encode(in, strlen(in), &out_len);
	zlog->log(zLogger::zLevel::GBUG,"%s", out);
	out_len = Zebra::url_decode(out, strlen(out));
	zlog->log(zLogger::zLevel::GBUG,"%s", out);

	/*
	for(int i = 0; i < 10; i++)
	{
		char buffer[7];
		int size;
		void *ret = Zebra::jpegPassport(buffer, sizeof(buffer), &size);

		char filename[32];
		bzero(filename, sizeof(filename));
		snprintf(filename, sizeof(filename) - 1, "test-%02u.jpeg", i);
		FILE *out = fopen(filename, "wb");
		if (out)
		{
			fwrite(ret, size, 1, out);
			fclose(out);
		}
		free(ret);
	}*/

	zlog->removeConsoleLog();
	delete zlog;
	// */
	return 0;
}
