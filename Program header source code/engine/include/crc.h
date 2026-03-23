/**
 * \file      crc.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     CRC计算
 * 
 *	      CRC计算
 */


#ifndef _CRC_H_
#define _CRC_H_

#define INITIAL_CRC_VALUE 0xffffffff

class Stream;

unsigned long calculateCRC(const void * buffer, long len, unsigned long crcVal = INITIAL_CRC_VALUE);
unsigned long calculateCRCStream(Stream *stream, unsigned long crcVal = INITIAL_CRC_VALUE);

#endif

