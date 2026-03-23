#include <stdlib.h>
#include <iostream>

#include "zMisc.h"
#include "Zebra.h"

namespace Zebra
{
	__thread unsigned int seedp = 0;

	volatile QWORD qwGameTime = 0;

	zLogger *logger = NULL;

	zProperties global;

	/**
	 * \brief 初始化一些全局变量
	 *
	 */
	static void initGlobal() __attribute__ ((constructor));
	void initGlobal()
	{
		global["threadPoolCapacity"] = "2048";
		global["server"] = "192.168.8.129";
		global["port"] = "10000";
		global["ifname"] = "eth0";
		global["mysql"] = "mysql://Zebra:Zebra@192.168.8.129:3306/TxGameDB";
		global["log"] = "debug";
		global["killpigtime"] = "0";
		global["killpigtime_hm"] = "0";
		global["pigtime"] = "0";
		global["phbgx"] = "0";
		global["pk1"] = "0";
		global["pk2"] = "0";
		global["pk3"] = "0";
		global["pk4"] = "0";
		global["pk5"] = "0";
		global["pk6"] = "0";
		global["pk7"] = "0";
		global["pk8"] = "0";
		global["pk9"] = "0";
		global["pk10"] = "0";
		global["pk11"] = "0";
		global["pk12"] = "0";
		global["pk13"] = "0";
		global["pk14"] = "0";
		global["pk15"] = "0";
		global["pk16"] = "0";
		global["pk17"] = "0";
		global["pk18"] = "0";
		global["pk19"] = "0";
		global["pk20"] = "0";
		global["pk21"] = "0";
		global["pk22"] = "0";
		global["pk23"] = "0";
		global["pk24"] = "0";
		global["pk25"] = "0";
		global["pk26"] = "0";
		global["pk27"] = "0";
		global["pk28"] = "0";
		global["pk29"] = "0";
		global["pk30"] = "0";
		global["pk31"] = "0";
		global["pk32"] = "0";
		global["BOSS_CHU"] = "0";
		global["BOSS_HAN"] = "0";

		global["BIKENUM1"] = "0";
		global["BIKENUM2"] = "0";
		global["BIKENUM3"] = "0";
		global["ZXCWC"] = "0";
		global["daoban"] = "0";
		global["MIGONGNUM1"] = "0";
		global["MIGONGNUM2"] = "0";
		global["MIGONGNUM3"] = "0";
		global["MIGONGWC"] = "0";
	}
	/**
	 * \brief 释放一些全局变量
	 *
	 */
	static void finalGlobal() __attribute__ ((destructor));
	void finalGlobal()
	{
		SAFE_DELETE(logger);
	}
};

