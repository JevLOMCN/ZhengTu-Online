/**
 * \file      frameAllocator.cpp
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     栈分配器
 * 
 *	      栈分配器
 */

#include "../include/engine.h"
#include "../include/frameAllocator.h"

BYTE*   FrameAllocator::smBuffer = NULL;
DWORD   FrameAllocator::smWaterMark = 0;
DWORD   FrameAllocator::smHighWaterMark = 0;

#ifdef _DEBUG
DWORD sgMaxFrameAllocation = 0;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
DWORD getMaxFrameAllocation()
{
	return sgMaxFrameAllocation;
}
#endif