#include "zTime.h"
#include "zTimer.h"
#include <unistd.h>
#include <iostream>
#include "CharBase.h"
#include "Object.h"

class myTask:public zTimer::zTimerTask
{
	private:
	void run()
	{
		zRTime tv;
		std::cout<< __PRETTY_FUNCTION__ << "  " << tv.msecs() <<  " " << get() <<std::endl;
	}

	public:
		unsigned long long get()
		{
			return nextExecTime.msecs() ;
		}
};
class myTask1:public zTimer::zTimerTask
{
	private:
	void run()
	{
		zRTime tv;
		std::cout<< __PRETTY_FUNCTION__ << "  " << tv.msecs() <<  " " << get() <<std::endl;
	}

	public:
		int get()
		{
			return nextExecTime.msecs() ;
		}
};

//zTimer *tr;
typedef void (*FUNC)();
struct SkillExp
{
	char *data;
	FUNC f;
	static SkillExp SE[];
	static void funcc(){};
};
#define pre(s) #s , s
SkillExp SkillExp::SE[] = 
{
	{"dd" , funcc},
	{pre(funcc) }
};
//#define aa
//#define STD(s) std:: ##s
int main(int argc,char *argv[])
{
	char cBuf[10];
	bzero(cBuf , 10);
	cBuf[9] = 3;
	std::cout << cBuf[9] <<std::endl;
	char cBuf_1[10];
	bzero(cBuf_1 , 10);
	bcopy(cBuf , cBuf_1 , 10);
	std::cout << cBuf_1[9] <<std::endl;
	std::cout << SkillExp::SE[1].data << std::endl;
	std::cout<< "CharBase:" << sizeof(CharBase) <<std::endl;

	std::cout<< "t_Object:" << sizeof(t_Object) <<std::endl;
	std::cout<< "time_t:" << sizeof(time_t) <<std::endl;
	Zebra::logger=new zLogger("timetest");

	zTimer *tr = new zTimer();
	/*
	zTime ztime=zTime::initzTime(1111111);
	for(int i=0;i<5;i++)
	{
		std::cout<< ztime.getzTime()<<std::endl;
		sleep(1);
	}
	// */

	myTask *task=new myTask();
	myTask1 *task1=new myTask1();

	if( *task >  *task1)
	{
	}
	zRTime tv;

	if(tr->scheduleAtRate(task,tv,10000))
		std::cout<< "ok" <<std::endl;
	else
		std::cout<< "ko" <<std::endl;
	/*if(tr->scheduleAtDelay(task,tv,2000))
		std::cout<< "ok" <<std::endl;
	else
		std::cout<< "ko" <<std::endl;*/
	//tr->scheduleAtDelay(task1,tv,200);
	if(tr->schedule(task1,10000))
		std::cout<< "ok" <<std::endl;
	else
		std::cout<< "ko" <<std::endl;

	
	
	while(true)
		usleep(1000000);
	return 0;
}
