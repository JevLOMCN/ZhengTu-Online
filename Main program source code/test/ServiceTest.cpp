/**
 * \file
 * \version  $Id: ServiceTest.cpp  $
 * \author 
 * \date 
 * \brief 测试类zService
 *
 * 
 */

#include <iostream>
#include <string>
#include <ext/numeric>

#include "zService.h"
#include "Zebra.h"
#include "zThread.h"

class TestService : public zService
{

	public:

		static TestService &getInstance()
		{
			if (NULL == instance)
				instance = new TestService("服务程序");

			return *instance;
		}

		/**
		 * \brief 释放类的唯一实例
		 *
		 */
		static void delInstance()
		{
			if (instance)
			{
				delete instance;
				instance = NULL;
			}
		}

	private:

		static TestService *instance;

		TestService(const std::string &name) : zService(name) {};

		bool init();
		bool serviceCallback();
		void final();

};

TestService *TestService::instance = NULL;

bool TestService::init()
{
	if (!zService::init())
		return false;

	Zebra::logger->debug(__PRETTY_FUNCTION__);

	return true;
}

bool TestService::serviceCallback()
{
	static int i = 0;
	zThread::sleep(1);
	Zebra::logger->debug("%s\t%u", __PRETTY_FUNCTION__, i);
	/*std::string s = "key = This is just a test.";
	std::vector<std::string> v;
	Zebra::stringtok(v, s, " =", 1);
	for(std::vector<std::string>::const_iterator it = v.begin(); it != v.end(); it++)
		std::cout << (*it) << std::endl;

	std::cout << std::endl;
	zProperties prop;
	prop.loadFromFile("test.props");
	prop.dump(std::cout);*/

	std::vector<int> V(10);
	__gnu_cxx::iota(V.begin(), V.end(), 7);
	std::copy(V.begin(), V.end(), std::ostream_iterator<int>(std::cout, " "));
	std::cout << std::endl; 

	std::vector<int> r(100);
	std::generate(r.begin(), r.end(), std::rand);
	std::copy(r.begin(), r.end(), std::ostream_iterator<int>(std::cout, " "));
	std::cout << std::endl; 

	return true;
}

void TestService::final()
{
	Zebra::logger->debug("%s", __PRETTY_FUNCTION__);
}

int main(int argc, char **argv)
{
	Zebra::logger=new zLogger();

	TestService::getInstance().main();
	TestService::delInstance();

	return EXIT_SUCCESS;
}

