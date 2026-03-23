//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------
#include "../engine.h"
#include "./bitStream.h"
#include <vector>
//static BitStream gPacketStream(NULL, 0);
//static BYTE gPacketBuffer[MaxPacketDataSize];

// bitstream utility functions

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param buffer[256] : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::setStringBuffer(char buffer[256])
{
   stringBuffer = buffer;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param writeSize : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
/*
BitStream *BitStream::getPacketStream(DWORD writeSize)
{
   if(!writeSize)
      writeSize = MaxPacketDataSize;
      
   gPacketStream.setBuffer(gPacketBuffer, writeSize, MaxPacketDataSize);
   gPacketStream.setPosition(0);
   
   return &gPacketStream;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param addr : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::sendPacketStream(const NetAddress *addr)
{
   Net::sendto(addr, gPacketStream.getBuffer(), gPacketStream.getPosition());
}

//*/
// FIXMEFIXMEFIXME MATH
                  
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param a : ÃèÊö
 * \param b : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline bool IsEqual(float a, float b) { return a == b; }

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
ResizeBitStream::ResizeBitStream(DWORD minSpace, DWORD initialSize) : BitStream(NULL, 0, 0)
{
   mMinSpace = minSpace;
   if(!initialSize)
      initialSize = minSpace * 2;
   BYTE *buf = (BYTE *) malloc(initialSize);
   setBuffer(buf, initialSize, initialSize);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
ResizeBitStream::~ResizeBitStream()
{
   free(dataPtr);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void ResizeBitStream::validate()
{
   if(getPosition() + mMinSpace > bufSize)
   {
      bufSize = getPosition() + mMinSpace * 2;
      dataPtr = (BYTE *) realloc(dataPtr, bufSize);
      
      maxReadBitNum = bufSize << 3;
      maxWriteBitNum = bufSize << 3;
   }
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class HuffmanProcessor
{
   static const DWORD csm_charFreqs[256];
   bool   m_tablesBuilt;

   void buildTables();

   struct HuffNode {
      DWORD pop;

      short index0;
      short index1;
   };
   struct HuffLeaf {
      DWORD pop;

      BYTE  numBits;
      BYTE  symbol;
      DWORD code;   // no code should be longer than 32 bits.
   };
   // We have to be a bit careful with these, mSince they are pointers...
   struct HuffWrap {
      HuffNode* pNode;
      HuffLeaf* pLeaf;

     public:
      HuffWrap() : pNode(NULL), pLeaf(NULL) { }

      void set(HuffLeaf* in_leaf) { pNode = NULL; pLeaf = in_leaf; }
      void set(HuffNode* in_node) { pLeaf = NULL; pNode = in_node; }

      DWORD getPop() { if (pNode) return pNode->pop; else return pLeaf->pop; }
   };

   std::vector<HuffNode> m_huffNodes;
   std::vector<HuffLeaf> m_huffLeaves;

   short determineIndex(HuffWrap&);

   void generateCodes(BitStream&, long, long);

  public:
   HuffmanProcessor() : m_tablesBuilt(false) { }

   static HuffmanProcessor g_huffProcessor;

   bool readHuffBuffer(BitStream* pStream, char* out_pBuffer);
   bool writeHuffBuffer(BitStream* pStream, const char* out_pBuffer, long maxLen);
};

HuffmanProcessor HuffmanProcessor::g_huffProcessor;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bufPtr : ÃèÊö
 * \param size : ÃèÊö
 * \param maxSize : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::setBuffer(void *bufPtr, long size, long maxSize)
{
   dataPtr = (BYTE *) bufPtr;
   bitNum = 0;
   bufSize = size;
   maxReadBitNum = size << 3;
   if(maxSize < 0)
      maxSize = size;
   maxWriteBitNum = maxSize << 3;
   error = false;
   mCompressRelative = false;
}

DWORD BitStream::getPosition() const
{
   return (bitNum + 7) >> 3;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pos : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool BitStream::setPosition(const DWORD pos)
{
   bitNum = pos << 3;
   return (true);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
DWORD BitStream::getStreamSize()
{
   AssertFatal(false, "Ambiguous call on BitStream: bytes or bits?");
   return 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
BYTE *BitStream::getBytePtr()
{
   return dataPtr + getPosition();
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
DWORD BitStream::getReadByteSize()
{
   return (maxReadBitNum >> 3) - getPosition();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::clear()
{
   memset(dataPtr, 0, bufSize);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param classId : ÃèÊö
 * \param classType : ÃèÊö
 * \param classGroup : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
/*
void BitStream::writeClassId(DWORD classId, DWORD classType, DWORD classGroup)
{
   AssertFatal(classType < NetClassTypesCount, "Out of range class type.");
   AssertFatal(classId < AbstractClassRep::NetClassCount[classGroup][classType], "Out of range class id.");
   writeInt(classId, AbstractClassRep::NetClassBitSize[classGroup][classType]);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param classType : ÃèÊö
 * \param classGroup : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
long BitStream::readClassId(DWORD classType, DWORD classGroup)
{
   AssertFatal(classType < NetClassTypesCount, "Out of range class type.");
   long ret = readInt(AbstractClassRep::NetClassBitSize[classGroup][classType]);
   if(ret > AbstractClassRep::NetClassCount[classGroup][classType])
      return -1;
   return ret;
}
//*/

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bitPtr : ÃèÊö
 * \param bitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::writeBits(const void *bitPtr,long bitCount )
{
   if(!bitCount)
      return;

   if(bitCount + bitNum > maxWriteBitNum)
   {
      error = true;
      AssertFatal(false, "Out of range write");
      return;
   }
   const BYTE *ptr = (BYTE *) bitPtr;
   BYTE *stPtr = dataPtr + (bitNum >> 3);
   BYTE *endPtr = dataPtr + ((bitCount + bitNum - 1) >> 3);

   long upShift  = bitNum & 0x7;
   long downShift= 8 - upShift;
   BYTE lastMask  = 0xFF >> (7 - ((bitNum + bitCount - 1) & 0x7));
   BYTE startMask = 0xFF >> downShift;

   BYTE curB = *ptr++;
   *stPtr = (curB << upShift) | (*stPtr & startMask);

   stPtr++;
   while(stPtr <= endPtr)
   {
      BYTE nextB = *ptr++;
      *stPtr++ = (curB >> downShift) | (nextB << upShift);
      curB = nextB;
   }
   *endPtr &= lastMask;
   
   bitNum += bitCount;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bitCount : ÃèÊö
 * \param set : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::setBit(long bitCount, bool set)
{
   if(set)
      *(dataPtr + (bitCount >> 3)) |= (1 << (bitCount & 0x7));
   else
      *(dataPtr + (bitCount >> 3)) &= ~(1 << (bitCount & 0x7));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool BitStream::testBit(long bitCount)
{
   return (*(dataPtr + (bitCount >> 3)) & (1 << (bitCount & 0x7))) != 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param val : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool BitStream::writeFlag(bool val)
{
   if(bitNum + 1 > maxWriteBitNum)
   {
      error = true;
      AssertFatal(false, "Out of range write");
      return false;
   }
   if(val)
      *(dataPtr + (bitNum >> 3)) |= (1 << (bitNum & 0x7));
   else
      *(dataPtr + (bitNum >> 3)) &= ~(1 << (bitNum & 0x7));
   bitNum++;
   return (val);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bitPtr : ÃèÊö
 * \param bitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::readBits(void *bitPtr,long bitCount )
{
   if(!bitCount)
      return;
   if(bitCount + bitNum > maxReadBitNum)
   {
      error = true;
      //AssertFatal(false, "Out of range read");
      AssertFatal(false, "Out of range read");
      return;
   }
   BYTE *stPtr = dataPtr + (bitNum >> 3);
   long byteCount = (bitCount + 7) >> 3;

   BYTE *ptr = (BYTE *) bitPtr;

   long downShift = bitNum & 0x7;
   long upShift = 8 - downShift;

   BYTE curB = *stPtr;
   while(byteCount--)
   {
      BYTE nextB = *++stPtr;
      *ptr++ = (curB >> downShift) | (nextB << upShift);
      curB = nextB;
   }

   bitNum += bitCount;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dataPtr : ÃèÊö
 * \param size : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool BitStream::_read(void *dataPtr, const DWORD size)
{
   readBits(dataPtr, size << 3);
   return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dataPtr : ÃèÊö
 * \param size : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool BitStream::_write(const void *dataPtr,const DWORD size)
{
   writeBits(dataPtr,size << 3 );
   return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
long BitStream::readInt(long bitCount)
{
   long ret = 0;
   readBits(&ret, bitCount);
   ret = convertLEndianToHost(ret);
   if(bitCount == 32)
      return ret;
   else
      ret &= (1 << bitCount) - 1;
   return ret;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param val : ÃèÊö
 * \param bitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::writeInt(long val, long bitCount)
{
   val = convertHostToLEndian(val);
   writeBits(bitCount, &val);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param f : ÃèÊö
 * \param bitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::writeFloat(float f, long bitCount)
{
   writeInt((long)(f * ((1 << bitCount) - 1)), bitCount);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
float BitStream::readFloat(long bitCount)
{
   return readInt(bitCount) / float((1 << bitCount) - 1);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param f : ÃèÊö
 * \param bitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::writeSignedFloat(float f, long bitCount)
{
   writeInt((long)(((f + 1) * .5) * ((1 << bitCount) - 1)), bitCount);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
float BitStream::readSignedFloat(long bitCount)
{
   return readInt(bitCount) * 2 / float((1 << bitCount) - 1) - 1.0f;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param value : ÃèÊö
 * \param bitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::writeSignedInt(long value, long bitCount)
{
   if(writeFlag(value < 0))
      writeInt(-value, bitCount - 1);
   else
      writeInt(value, bitCount - 1);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
long BitStream::readSignedInt(long bitCount)
{
   if(readFlag())
      return -readInt(bitCount - 1);
   else
      return readInt(bitCount - 1);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param vec : ÃèÊö
 * \param bitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
/*
void BitStream::writeNormalVector(const Point3F& vec, long bitCount)
{
   float phi   = mAtan(vec.x, vec.y) / M_PI;
   float theta = mAtan(vec.z, mSqrt(vec.x*vec.x + vec.y*vec.y)) / (M_PI/2.0);

   writeSignedFloat(phi, bitCount+1);
   writeSignedFloat(theta, bitCount);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param vec : ÃèÊö
 * \param bitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::readNormalVector(Point3F *vec, long bitCount)
{
   float phi   = readSignedFloat(bitCount+1) * M_PI;
   float theta = readSignedFloat(bitCount) * (M_PI/2.0);

   vec->x = mSin(phi)*mCos(theta);
   vec->y = mCos(phi)*mCos(theta);
   vec->z = mSin(theta);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param vec : ÃèÊö
 * \param bitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
Point3F BitStream::dumbDownNormal(const Point3F& vec, long bitCount)
{
   BYTE buffer[128];
   BitStream temp(buffer, 128);

   temp.writeNormalVector(vec, bitCount);
   temp.setCurPos(0);

   Point3F ret;
   temp.readNormalVector(&ret, bitCount);
   return ret;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param vec : ÃèÊö
 * \param angleBitCount : ÃèÊö
 * \param zBitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::writeNormalVector(const Point3F& vec, long angleBitCount, long zBitCount)
{
   writeSignedFloat( vec.z, zBitCount );
   
   // don't need to write x and y if they are both zero, which we can assess
   // by checking for |z| == 1
   if(!IsEqual(mFabs(vec.z), 1.0f))
   {
      writeSignedFloat( mAtan(vec.x,vec.y) / M_2PI, angleBitCount );
   }
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param vec : ÃèÊö
 * \param angleBitCount : ÃèÊö
 * \param zBitCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::readNormalVector(Point3F * vec, long angleBitCount, long zBitCount)
{
   vec->z = readSignedFloat(zBitCount);
   
   // check to see if |z| == 1.  If so, then we don't read x and y (they're zero)
   if(!IsEqual(mFabs(vec->z), 1.0f))
   {
      float angle = M_2PI * readSignedFloat(angleBitCount);
   
      float mult = mSqrt(1.0f - vec->z * vec->z);
      vec->x = mult * mCos(angle);
      vec->y = mult * mSin(angle);
   }
   else
   {
      // no need to read, we know
      vec->x = 0.0f;
      vec->y = 0.0f;
   }
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param matrix : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::writeAffineTransform(const MatrixF& matrix)
{
//   AssertFatal(matrix.isAffine() == true,
//               "BitStream::writeAffineTransform: Error, must write only affine transforms!");

   Point3F pos;
   matrix.getColumn(3, &pos);
   mathWrite(*this, pos);

   QuatF q(matrix);
   q.normalize();
   write(q.x);
   write(q.y);
   write(q.z);
   writeFlag(q.w < 0.0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param matrix : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::readAffineTransform(MatrixF* matrix)
{
   Point3F pos;
   QuatF   q;

   mathRead(*this, &pos);
   read(&q.x);
   read(&q.y);
   read(&q.z);
   q.w = mSqrt(1.0 - getMin(float(((q.x * q.x) + (q.y * q.y) + (q.z * q.z))), 1.f));
   if (readFlag())
      q.w = -q.w;

   q.setMatrix(matrix);
   matrix->setColumn(3, pos);
//   AssertFatal(matrix->isAffine() == true,
//               "BitStream::readAffineTransform: Error, transform should be affine after this function!");
}

//*/

//----------------------------------------------------------------------------

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::clearCompressionPoint()
{
   mCompressRelative = false;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param p : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
/*
void BitStream::setCompressionPoint(const Point3F& p)
{
   mCompressRelative = true;
   mCompressPoint = p;
}

static DWORD gBitCounts[4] = {
   16, 18, 20, 32
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param p : ÃèÊö
 * \param scale : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::writeCompressedPoint(const Point3F& p,float scale)
{
   // Same # of bits for all axis
   Point3F vec;
   float invScale = 1 / scale;
   DWORD type;
   if(mCompressRelative)
   {
      vec = p - mCompressPoint;
      float dist = vec.len() * invScale;
      if(dist < (1 << 15))
         type = 0;
      else if(dist < (1 << 17))
         type = 1;
      else if(dist < (1 << 19))
         type = 2;
      else
         type = 3;
   }
   else
      type = 3;

   writeInt(type, 2);

   if (type != 3)
   {
      type = gBitCounts[type];
      writeSignedInt(long(vec.x * invScale),type);
      writeSignedInt(long(vec.y * invScale),type);
      writeSignedInt(long(vec.z * invScale),type);
   }
   else
   {
      write(p.x);
      write(p.y);
      write(p.z);
   }
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param p : ÃèÊö
 * \param scale : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::readCompressedPoint(Point3F* p,float scale)
{
   // Same # of bits for all axis
   DWORD type = readInt(2);

   if(type == 3)
   {
      read(&p->x);
      read(&p->y);
      read(&p->z);
   }
   else
   {
      type = gBitCounts[type];
      p->x = readSignedInt(type);
      p->y = readSignedInt(type);
      p->z = readSignedInt(type);

      p->x = mCompressPoint.x + p->x * scale;
      p->y = mCompressPoint.y + p->y * scale;
      p->z = mCompressPoint.z + p->z * scale;
   }
}

//*/

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param buf[256] : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::readString(char buf[256])
{
   if(stringBuffer)
   {
      if(readFlag())
      {
         long offset = readInt(8);
         HuffmanProcessor::g_huffProcessor.readHuffBuffer(this, stringBuffer + offset);
         dStrcpy(buf, stringBuffer);
         return;
      }
   }
   HuffmanProcessor::g_huffProcessor.readHuffBuffer(this, buf);
   if(stringBuffer)
      dStrcpy(stringBuffer, buf);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param string : ÃèÊö
 * \param maxLen : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void BitStream::writeString(const char *string, long maxLen)
{
   if(!string)
      string = "";
   if(stringBuffer)
   {
      long j;
      for(j = 0; j < maxLen && stringBuffer[j] == string[j] && string[j];j++)
         ;
      dStrncpy(stringBuffer, string, maxLen);
      stringBuffer[maxLen] = 0;

      if(writeFlag(j > 2))
      {
         writeInt(j, 8);
         HuffmanProcessor::g_huffProcessor.writeHuffBuffer(this, string + j, maxLen - j);
         return;
      }
   }
   HuffmanProcessor::g_huffProcessor.writeHuffBuffer(this, string, maxLen);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void HuffmanProcessor::buildTables()
{
   AssertFatal(m_tablesBuilt == false, "Cannot build tables twice!");
   m_tablesBuilt = true;

   long i;

   // First, construct the array of wraps...
   //
   m_huffLeaves.setSize(256);
   m_huffNodes.reserve(256);
   m_huffNodes.increment();
   for (i = 0; i < 256; i++) {
      HuffLeaf& rLeaf = m_huffLeaves[i];

      rLeaf.pop    = csm_charFreqs[i] + 1;
      rLeaf.symbol = BYTE(i);

      memset(&rLeaf.code, 0, sizeof(rLeaf.code));
      rLeaf.numBits = 0;
   }

   long currWraps = 256;
   HuffWrap* pWrap = new HuffWrap[256];
   for (i = 0; i < 256; i++) {
      pWrap[i].set(&m_huffLeaves[i]);
   }

   while (currWraps != 1) {
      DWORD min1 = 0xfffffffe, min2 = 0xffffffff;
      long index1 = -1, index2 = -1;

      for (i = 0; i < currWraps; i++) {
         if (pWrap[i].getPop() < min1) {
            min2   = min1;
            index2 = index1;

            min1   = pWrap[i].getPop();
            index1 = i;
         } else if (pWrap[i].getPop() < min2) {
            min2   = pWrap[i].getPop();
            index2 = i;
         }
      }
      AssertFatal(index1 != -1 && index2 != -1 && index1 != index2, "hrph");
      
      // Create a node for this...
      m_huffNodes.increment();
      HuffNode& rNode = m_huffNodes.last();
      rNode.pop    = pWrap[index1].getPop() + pWrap[index2].getPop();
      rNode.index0 = determineIndex(pWrap[index1]);
      rNode.index1 = determineIndex(pWrap[index2]);

      long mergeIndex = index1 > index2 ? index2 : index1;
      long nukeIndex  = index1 > index2 ? index1 : index2;
      pWrap[mergeIndex].set(&rNode);

      if (index2 != (currWraps - 1)) {
         pWrap[nukeIndex] = pWrap[currWraps - 1];
      }
      currWraps--;
   }
   AssertFatal(currWraps == 1, "wrong wraps?");
   AssertFatal(pWrap[0].pNode != NULL && pWrap[0].pLeaf == NULL, "Wrong wrap type!");

   // Ok, now we have one wrap, which is a node.  we need to make sure that this
   //  is the first node in the node list.
   m_huffNodes[0] = *(pWrap[0].pNode);
   delete [] pWrap;

   DWORD code = 0;
   BitStream bs(&code, 4);

   generateCodes(bs, 0, 0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param rBS : ÃèÊö
 * \param index : ÃèÊö
 * \param depth : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void HuffmanProcessor::generateCodes(BitStream& rBS, long index, long depth)
{
   if (index < 0) {
      // leaf node, copy the code in, and back out...
      HuffLeaf& rLeaf = m_huffLeaves[-(index + 1)];

      dMemcpy(&rLeaf.code, rBS.dataPtr, sizeof(rLeaf.code));
      rLeaf.numBits = depth;
   } else {
      HuffNode& rNode = m_huffNodes[index];

      long pos = rBS.getCurPos();

      rBS.writeFlag(false);
      generateCodes(rBS, rNode.index0, depth + 1);

      rBS.setCurPos(pos);
      rBS.writeFlag(true);
      generateCodes(rBS, rNode.index1, depth + 1);

      rBS.setCurPos(pos);      
   }
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param rWrap : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
short HuffmanProcessor::determineIndex(HuffWrap& rWrap)
{
   if (rWrap.pLeaf != NULL) {
      AssertFatal(rWrap.pNode == NULL, "Got a non-NULL pNode in a HuffWrap with a non-NULL leaf.");

      return -((rWrap.pLeaf - m_huffLeaves.address()) + 1);
   } else {
      AssertFatal(rWrap.pNode != NULL, "Got a NULL pNode in a HuffWrap with a NULL leaf.");

      return rWrap.pNode - m_huffNodes.address();
   }
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pStream : ÃèÊö
 * \param out_pBuffer : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool HuffmanProcessor::readHuffBuffer(BitStream* pStream, char* out_pBuffer)
{
   if (m_tablesBuilt == false)
      buildTables();

   if (pStream->readFlag()) {
      long len = pStream->readInt(8);
      for (long i = 0; i < len; i++) {
         long index = 0;
         while (true) {
            if (index >= 0) {
               if (pStream->readFlag() == true) {
                  index = m_huffNodes[index].index1;
               } else {
                  index = m_huffNodes[index].index0;
               }
            } else {
               out_pBuffer[i] = m_huffLeaves[-(index+1)].symbol;
               break;
            }
         }
      }
      out_pBuffer[len] = '\0';
      return true;
   } else {
      // Uncompressed string...
      DWORD len = pStream->readInt(8);
      pStream->read(len, out_pBuffer);
      out_pBuffer[len] = '\0';
      return true;
   }
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pStream : ÃèÊö
 * \param out_pBuffer : ÃèÊö
 * \param maxLen : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool HuffmanProcessor::writeHuffBuffer(BitStream* pStream, const char* out_pBuffer, long maxLen)
{
   if (out_pBuffer == NULL) {
      pStream->writeFlag(false);
      pStream->writeInt(0, 8);
      return true;
   }

   if (m_tablesBuilt == false)
      buildTables();

   long len = out_pBuffer ? dStrlen(out_pBuffer) : 0;
   AssertWarn(len <= 255, "String TOO long for writeString");
   AssertWarn(len <= 255, out_pBuffer);
   if (len > maxLen)
      len = maxLen;

   long numBits = 0;
   long i;
   for (i = 0; i < len; i++)
      numBits += m_huffLeaves[(unsigned char)out_pBuffer[i]].numBits;

   if (numBits >= (len * 8)) {
      pStream->writeFlag(false);
      pStream->writeInt(len, 8);
      pStream->write(len, out_pBuffer);
   } else {
      pStream->writeFlag(true);
      pStream->writeInt(len, 8);
      for (i = 0; i < len; i++) {
         HuffLeaf& rLeaf = m_huffLeaves[((unsigned char)out_pBuffer[i])];
         pStream->writeBits(rLeaf.numBits, &rLeaf.code);
      }
   }

   return true;
}

const DWORD HuffmanProcessor::csm_charFreqs[256] = {
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
329   ,
21    ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
2809  ,
68    ,
0     ,
27    ,
0     ,
58    ,
3     ,
62    ,
4     ,
7     ,
0     ,
0     ,
15    ,
65    ,
554   ,
3     ,
394   ,
404   ,
189   ,
117   ,
30    ,
51    ,
27    ,
15    ,
34    ,
32    ,
80    ,
1     ,
142   ,
3     ,
142   ,
39    ,
0     ,
144   ,
125   ,
44    ,
122   ,
275   ,
70    ,
135   ,
61    ,
127   ,
8     ,
12    ,
113   ,
246   ,
122   ,
36    ,
185   ,
1     ,
149   ,
309   ,
335   ,
12    ,
11    ,
14    ,
54    ,
151   ,
0     ,
0     ,
2     ,
0     ,
0     ,
211   ,
0     ,
2090  ,
344   ,
736   ,
993   ,
2872  ,
701   ,
605   ,
646   ,
1552  ,
328   ,
305   ,
1240  ,
735   ,
1533  ,
1713  ,
562   ,
3     ,
1775  ,
1149  ,
1469  ,
979   ,
407   ,
553   ,
59    ,
279   ,
31    ,
0     ,
0     ,
0     ,
68    ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     
};

