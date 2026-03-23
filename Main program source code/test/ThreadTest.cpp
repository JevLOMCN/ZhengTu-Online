/**
 * \file
 * \version  $Id: ThreadTest.cpp  $
 * \author 
 * \date 
 * \brief ≤‚ ‘œﬂ≥Ã
 *
 * 
 */

#include <iostream>
#include <sstream>
#include <string>

#include "zThread.h"
#include "Zebra.h"

class Thread : public zThread
{

	public:

		Thread(const std::string &name) : zThread(name, false) {};

		virtual void run()
		{
			while(!isFinal())
				zThread::sleep(2);
		};

};

int main(int argc, char **argv)
{
	Thread *pThread = NULL;
	int i = 0;

	do
	{
		std::ostringstream name;
		name << "Thread" << "[" << i++ << "]";
		pThread = new Thread(name.str());
	}
	while(pThread->start());
	
	return EXIT_SUCCESS;
}
