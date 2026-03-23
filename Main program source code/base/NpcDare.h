/**
 * \file
 * \version  $Id: NpcDare.h $
 * \author  
 * \date 
 * \brief 定义NPC争夺的公共信息
 *
 */


#ifndef _NPCDARE_H_
#define _NPCDARE_H_

#include "zType.h"

#pragma pack(1)

namespace NpcDareDef
{

	const DWORD CREATE_NPCDARE_NEED_ITEM = 738; // 发起对战需要的道具 地羽令

	struct NpcDareRecord {
		DWORD dwCountry;			/// 国家
		DWORD dwMapID;				/// 地图ID
		DWORD dwNpcID;				/// NPC id
		DWORD dwPosX;				/// npc的 x 坐标
		DWORD dwPosY;				/// npc的 y 坐标
		DWORD dwHoldSeptID;			/// 目前该npc的所有家族
		DWORD dwDareSeptID;			/// 目前该npc的挑战家族
		DWORD dwGold;				/// 结余税金
	} __attribute__ ((packed));

}

#pragma pack()
#endif





