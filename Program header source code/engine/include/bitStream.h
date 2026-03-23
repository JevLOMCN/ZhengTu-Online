//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _BITSTREAM_H_
#define _BITSTREAM_H_

//-------------------------------------- Some caveats when using this class:
//                                        - Get/setPosition semantics are changed
//                                         to indicate bit position rather than
//                                         byte position.
//
#include "Stream.h"
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class BitStream : public Stream
{
protected:
   BYTE *dataPtr;
   long  bitNum;
   long  bufSize;
   bool error;
   long  maxReadBitNum;
   long  maxWriteBitNum;
   char *stringBuffer;
   bool mCompressRelative;
   //Point3F mCompressPoint;

   friend class HuffmanProcessor;
public:
   //static BitStream *getPacketStream(DWORD writeSize = 0);
   //static void sendPacketStream(const NetAddress *addr);

   void setBuffer(void *bufPtr, long bufSize, long maxSize = 0);
   BYTE*  getBuffer() { return dataPtr; }
   BYTE*  getBytePtr();
   
   DWORD getReadByteSize();

   long  getCurPos() const;
   void setCurPos(const DWORD);

   BitStream(void *bufPtr, long bufSize, long maxWriteSize = -1) { setBuffer(bufPtr, bufSize,maxWriteSize); stringBuffer = NULL; }
   void clear();
   
   void setStringBuffer(char buffer[256]);
   void writeInt(long value, long bitCount);
   long  readInt(long bitCount);

   void writeSignedInt(long value, long bitCount);
   long  readSignedInt(long bitCount);

   void writeRangedU32(DWORD value, DWORD rangeStart, DWORD rangeEnd);
   DWORD  readRangedU32(DWORD rangeStart, DWORD rangeEnd);

   // read and write floats... floats are 0 to 1 inclusive, signed floats are -1 to 1 inclusive

   float  readFloat(long bitCount);
   float  readSignedFloat(long bitCount);

   void writeFloat(float f, long bitCount);
   void writeSignedFloat(float f, long bitCount);

   //void writeClassId(DWORD classId, DWORD classType, DWORD classGroup);
   //long readClassId(DWORD classType, DWORD classGroup); // returns -1 if the class type is out of range

   /*
   // writes a normalized vector
   void writeNormalVector(const Point3F& vec, long bitCount);
   void readNormalVector(Point3F *vec, long bitCount);

   void clearCompressionPoint();
   void setCompressionPoint(const Point3F& p);

   // Matching calls to these compression methods must, of course,
   // have matching scale values.
   void writeCompressedPoint(const Point3F& p,float scale = 0.01f);
   void readCompressedPoint(Point3F* p,float scale = 0.01f);

   // Uses the above method to reduce the precision of a normal vector so the server can
   //  determine exactly what is on the client.  (Pre-dumbing the vector before sending
   //  to the client can result in precision errors...)
   static Point3F dumbDownNormal(const Point3F& vec, long bitCount);   


   // writes a normalized vector using alternate method
   void writeNormalVector(const Point3F& vec, long angleBitCount, long zBitCount);
   void readNormalVector(Point3F *vec, long angleBitCount, long zBitCount);
   
   // writes an affine transform (full precision version)
   void writeAffineTransform(const MatrixF&);
   void readAffineTransform(MatrixF*);

   //*/

   void writeBits(const void *bitPtr, long bitCount);
   void readBits(void *bitPtr, long bitCount);
   bool writeFlag(bool val);
   bool readFlag();

   void setBit(long bitCount, bool set);
   bool testBit(long bitCount);

   bool isFull() { return bitNum > (bufSize << 3); }
   bool isValid() { return !error; }

   bool _read (void* d,const DWORD size);
   bool _write(const void* d,const DWORD size);

   void readString(char stringBuf[256]);
   void writeString(const char *stringBuf, long maxLen=255);

   bool hasCapability(const Capability) const { return true; }
   DWORD  getPosition() const;
   bool setPosition(const DWORD in_newPosition);
   DWORD  getStreamSize();
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class ResizeBitStream : public BitStream
{
   DWORD mMinSpace;
public:
   ResizeBitStream(DWORD minSpace = 1500, DWORD initialSize = 0);
   void validate();
   ~ResizeBitStream();
};

//------------------------------------------------------------------------------
//-------------------------------------- INLINES
//
inline long BitStream::getCurPos() const
{
   return bitNum;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param in_position : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void BitStream::setCurPos(const DWORD in_position)
{
   AssertFatal(in_position < (DWORD)(bufSize << 3), "Out of range bitposition");
   bitNum = long(in_position);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline bool BitStream::readFlag()
{
   if(bitNum > maxReadBitNum)
   {
      error = true;
      AssertFatal(false, "Out of range read");
      return false;
   }
   long mask = 1 << (bitNum & 0x7);
   bool ret = (*(dataPtr + (bitNum >> 3)) & mask) != 0;
   bitNum++;
   return ret;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param value : ÃèÊö
 * \param rangeStart : ÃèÊö
 * \param rangeEnd : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void BitStream::writeRangedU32(DWORD value, DWORD rangeStart, DWORD rangeEnd)
{
   AssertFatal(value >= rangeStart && value <= rangeEnd, "Out of bounds value!");
   AssertFatal(rangeEnd >= rangeStart, "error, end of range less than start");

   DWORD rangeSize = rangeEnd - rangeStart + 1;
   DWORD rangeBits = GetRangeBits(rangeSize);

   writeInt(long(value - rangeStart), long(rangeBits));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param rangeStart : ÃèÊö
 * \param rangeEnd : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline DWORD BitStream::readRangedU32(DWORD rangeStart, DWORD rangeEnd)
{
   AssertFatal(rangeEnd >= rangeStart, "error, end of range less than start");

   DWORD rangeSize = rangeEnd - rangeStart + 1;
   DWORD rangeBits = GetRangeBits(rangeSize);

   DWORD val = DWORD(readInt(long(rangeBits)));
   return val + rangeStart;
}

#endif //_BITSTREAM_H_
