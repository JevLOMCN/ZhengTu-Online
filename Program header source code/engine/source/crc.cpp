/**
 * \file      crc.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     CRC¼ÆËã
 * 
 *	      CRC¼ÆËã
 */

#include "../include/engine.h"
#include "../include/crc.h"

//-----------------------------------------------------------------------------
// simple crc function - generates lookup table on first call

static unsigned long crcTable[256];
static bool crcTableValid;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
static void calculateCRCTable()
{
   unsigned long val;

   for(long i = 0; i < 256; i++)
   {
      val = i;
      for(long j = 0; j < 8; j++)
      {
         if(val & 0x01)
            val = 0xedb88320 ^ (val >> 1);
         else
            val = val >> 1;
      }
      crcTable[i] = val;
   }
   crcTableValid = true;
}


//-----------------------------------------------------------------------------

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param buffer : ÃèÊö
 * \param len : ÃèÊö
 * \param crcVal : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
unsigned long calculateCRC(const void * buffer, long len, unsigned long crcVal )
{
   // check if need to generate the crc table
   if(!crcTableValid)
      calculateCRCTable();
   
   // now calculate the crc
   char * buf = (char*)buffer;
   for(long i = 0; i < len; i++)
      crcVal = crcTable[(crcVal ^ buf[i]) & 0xff] ^ (crcVal >> 8);
   return(crcVal);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param stream : ÃèÊö
 * \param crcVal : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//*
unsigned long calculateCRCStream(Stream *stream, unsigned long crcVal )
{
   // check if need to generate the crc table
   if(!crcTableValid)
      calculateCRCTable();
   
   // now calculate the crc
   stream->setPosition(0);
   long len = stream->getSize();
   BYTE buf[4096];

   long segCount = (len + 4095) / 4096;

   for(long j = 0; j < segCount; j++)
   {
      long slen = getMin(4096l, len - (j * 4096));
      stream->read(buf, slen);
      crcVal = calculateCRC(buf, slen, crcVal);
   }
   stream->setPosition(0);
   return(crcVal);
}
//*/
