/**
 * \file      UdpSocket.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#pragma once
#include "socket.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CUdpSocket :
	public net::CSocket
{
public:
	CUdpSocket(void);
	~CUdpSocket(void);
};
