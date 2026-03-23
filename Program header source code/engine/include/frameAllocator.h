/**
 * \file      frameAllocator.h
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     Õ»·ÖÅäÆ÷
 * 
 *	      Õ»·ÖÅäÆ÷
 */
#ifndef _FRAMEALLOCATOR_H_
#define _FRAMEALLOCATOR_H_

#include <assert.h>
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class FrameAllocator
{
public:
   static unsigned char*   smBuffer;
   static unsigned long   smHighWaterMark;
   static unsigned long   smWaterMark;
  public:
   inline static void init(const unsigned long frameSize);
   inline static void destroy();

   inline static void _free(void* p);
   inline static void* alloc(unsigned long allocSize);
   inline static void* allocNoAlign(unsigned long allocSize);
   inline static void setWaterMark(const unsigned long);
   inline static unsigned long  getWaterMark();
   inline static unsigned long  getHighWaterMark();
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class SetFrameAllocator{
	unsigned long m_dwWaterMark;
public:
	SetFrameAllocator()
	{
		m_dwWaterMark = FrameAllocator::getWaterMark();
	}
	~SetFrameAllocator()
	{
		FrameAllocator::setWaterMark(m_dwWaterMark);
	}
};

class KeepFrameAllocator{
	void* mPtr;
public:
	KeepFrameAllocator()
	{
		mPtr = NULL;
	}
	~KeepFrameAllocator()
	{
		if(mPtr) FrameAllocator::_free(mPtr);
	}
	
	void _free()
	{
		if(mPtr) 
		{
			FrameAllocator::_free(mPtr);
			mPtr = NULL;
		}
	}

	void* alloc(size_t size)
	{
		_free();
		mPtr = FrameAllocator::alloc(size);
		return mPtr;
	}
};

#if defined(_DEBUG)
extern unsigned long sgMaxFrameAllocation;
unsigned long getMaxFrameAllocation();
#endif

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param frameSize : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void FrameAllocator::init(const unsigned long frameSize)
{
   assert(smBuffer == NULL && "Error, already initialized");
   smBuffer = (unsigned char*)malloc(frameSize);
   smWaterMark = 0;
   smHighWaterMark = frameSize;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void FrameAllocator::destroy()
{
   assert(smBuffer != NULL && "Error, not initialized");
   
   free(smBuffer);
   smBuffer = NULL;
   smWaterMark = 0;
   smHighWaterMark = 0;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param allocSize : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void* FrameAllocator::alloc(unsigned long allocSize)
{
  // allocSize = ((allocSize + 0xF) & ~0xF);
   assert(smBuffer != NULL && "Error, no buffer!");
   if(smWaterMark + allocSize <= smHighWaterMark)
   {
		//assert(smWaterMark + allocSize <= smHighWaterMark && "Error alloc too large, increase frame size!");

		unsigned char* p = &smBuffer[smWaterMark];
		smWaterMark += allocSize;

		#if defined(_DEBUG)
		if (smWaterMark > sgMaxFrameAllocation)
			sgMaxFrameAllocation = smWaterMark;
		#endif
		return p;
   }
   else
   {
	   return malloc(allocSize);
   }
}


void* FrameAllocator::allocNoAlign(unsigned long allocSize)
{
	return alloc(allocSize);
}

void FrameAllocator::_free(void* p)
{
	if( (BYTE*)p >= (BYTE*)smBuffer && (BYTE*) p < (BYTE*)smBuffer + smHighWaterMark)
	{
	}
	else
	{
		free(p);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param waterMark : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void FrameAllocator::setWaterMark(const unsigned long waterMark)
{
   assert(waterMark < smHighWaterMark && "Error, invalid waterMark");

   smWaterMark = waterMark;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
unsigned long FrameAllocator::getWaterMark()
{
   return smWaterMark;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
unsigned long FrameAllocator::getHighWaterMark()
{
   return smHighWaterMark;
}


#endif  // _H_FRAMEALLOCATOR_
