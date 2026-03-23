#ifndef _NetType_h_
#define _NetType_h_

enum NetType
{
	NetType_near = 0,	//近程路由，电信区连电信服务器，网通区连网通服务器
	NetType_far = 1		//远端路由，电信区连网通服务器，网通区连电信服务器
};

#endif

