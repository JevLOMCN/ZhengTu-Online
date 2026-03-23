/**
 * \file
 * \version  $Id: zDatabase.cpp  $
 * \author  
 * \date 
 * \brief Zebra游戏中所有基本数据结构的声明定义
 */

#include "zDatabase.h"
#include "zDatabaseManager.h"

void NpcCarryObject::lostGreen(NpcLostObject &nlo, int value, int value1, int value2, int vcharm, int vlucky)
{
	//mlock.lock();
	if (vcharm>1000) vcharm=1000;
	if (vlucky>1000) vlucky=1000;
	for(std::vector<CarryObject>::const_iterator it = cov.begin(); it != cov.end(); it++)
	{
		//Zebra::logger->debug("%u, %u, %u, %u", (*it).id, (*it).rate, (*it).minnum, (*it).maxnum);
		zObjectB *ob = objectbm.get((*it).id);
		if (ob)
		{
			if (ob->kind>=101 && ob->kind <=118)
			{
				nlo.push_back(*it);
			}
			else
			{
				switch((*it).id)
				{
					case 665:
					{
						int vrate = (int)(((*it).rate/value)*(1+value1/100.0f)*(1+value2/100.0f)*(1+vcharm/1000.0f)*(1+vlucky/1000.0f));
						if (zMisc::selectByTenTh(vrate))
						{
							nlo.push_back(*it);
						}
					}
					break;
					default:
					{
						int vrate = (int)(((*it).rate/value)*(1+value1/100.0f)*(1+vcharm/1000.0f)*(1+vlucky/1000.0f));
						if (zMisc::selectByTenTh(vrate))
						{
							nlo.push_back(*it);
						}
					}
					break;
				}
			}
		}
	}
	//mlock.unlock();
}
