/**
 * \file
 * \version  $Id: datetime.cpp  $
 * \author  
 * \date 
 * \brief 
 *
 * 
 */
#include <vector>
#include <iostream>
#include <string>

#include <sys/time.h>
#include "zTime.h"
#include "zTCPClientTask.h"

int main()
{
	time_t input_time = 0;
		
	std::cout << "size:" << sizeof(zTCPClientTask) << std::endl;
	struct tm tv1;
	input_time = time(NULL) - 24*60*60;
	zRTime::getLocalTime(tv1, input_time);
	std::cout << tv1.tm_wday << std::endl;

	while(1)
	{
		input_time=0;
		std::cout << "input time:";
		std::cin >> input_time;
		
		if (input_time==0)
		{
			break;
		}
		else
		{
			struct tm tv1;
			zRTime::getLocalTime(tv1, input_time);
			std::cout << (tv1.tm_year+1900) << ":" << (tv1.tm_mon+1) << ":" << tv1.tm_mday <<
			" " << tv1.tm_hour << ":" << tv1.tm_min << ":" <<  tv1.tm_sec << std::endl;
		}
	}
	
	return 1;
}





