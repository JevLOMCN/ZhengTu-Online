/**
 * \file      PackSubStream.cpp
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     Pak文件包中的子文件流，支持加密压缩
 * 
 *	      Pak文件包中的子文件流，支持加密压缩
 */

#include "../include/engine.h"
#include "../include//packsubstream.h"
#include "../../EncDec/include/EncDec.h"
#include <vector>
#include "../../Zlib/Zlib.h"


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
PackSubRStream::PackSubRStream(DWORD dwFlags,bool bFullBuffer)
: ZipSubRStream(bFullBuffer)
, m_dwFlags(dwFlags)
, m_dwBufferPosition(0)
, m_dwBufferSize(0)
, m_pDesKey(NULL)
{
	Assert( (m_dwFlags& PackAggregate::flagCompress) ||  (m_dwFlags & PackAggregate::flagEncrypt) );
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
PackSubRStream::~PackSubRStream(void)
{
}
const DWORD csm_inputBufferSize = 4096;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param io_pSlaveStream : 描述
 * \return 返回值的描述
 */
//--------------------------------------
bool PackSubRStream::attachStream(Stream* io_pSlaveStream)
{
   AssertFatal(io_pSlaveStream != NULL, "NULL Slave stream?");
   AssertFatal(m_pStream == NULL,       "Already attached!");

   m_pStream          = io_pSlaveStream;
   m_originalSlavePosition = io_pSlaveStream->getPosition();
   m_curSlavePosition = m_originalSlavePosition;
   //m_uncompressedSize = 0;
   m_currentPosition  = 0;
   m_dwBufferPosition = 0;
   m_dwBufferSize = 0;
   // Initialize zipStream state...


   if(isCompress())
   {

		if(m_bFullBuffer){
			if(m_pInputBuffer)
				delete [] m_pInputBuffer;

			BYTE inBuffer[::csm_inputBufferSize];
			m_pInputBuffer = inBuffer;

			DWORD buffSize = fillBuffer(csm_inputBufferSize);

			m_pZipStream   = new z_stream_s;
			m_pZipStream->zalloc = Z_NULL;
			m_pZipStream->zfree  = Z_NULL;
			m_pZipStream->opaque = Z_NULL;

			m_pZipStream->next_in  = m_pInputBuffer;
			m_pZipStream->avail_in = buffSize;
			m_pZipStream->total_in = 0;
			inflateInit2(m_pZipStream, -MAX_WBITS);

			setStatus(Ok);

			size_t uSize = isEncrypt() ? ((m_uncompressedSize + 7) & (~7)) : m_uncompressedSize;

			if(m_pOutputBuffer)
			{
				delete [] m_pOutputBuffer;
				m_pOutputBuffer = NULL;
			}

			BYTE* pOutputBuffer = new BYTE[uSize];

			_read(pOutputBuffer,m_uncompressedSize);

			m_pInputBuffer = NULL;
			m_pOutputBuffer = pOutputBuffer;

			setPosition(0);
		}else{
			if(!m_pInputBuffer)
				m_pInputBuffer = new BYTE[csm_inputBufferSize];

			DWORD buffSize = fillBuffer(csm_inputBufferSize);

			m_pZipStream   = new z_stream_s;
			m_pZipStream->zalloc = Z_NULL;
			m_pZipStream->zfree  = Z_NULL;
			m_pZipStream->opaque = Z_NULL;

			m_pZipStream->next_in  = m_pInputBuffer;
			m_pZipStream->avail_in = buffSize;
			m_pZipStream->total_in = 0;
			inflateInit2(m_pZipStream, -MAX_WBITS);
		}
   }else{
	   if(isEncrypt()){
		   if(m_bFullBuffer){
			   if(m_pInputBuffer)
				   delete [] m_pInputBuffer;
			   size_t uSize = ((m_uncompressedSize + 7) & (~7));
			   m_pInputBuffer = new BYTE[uSize];
			   DWORD bufferSize = fillBuffer(uSize);
			   m_pOutputBuffer = m_pInputBuffer;
			   m_pInputBuffer = NULL;
		   }else{
			   if(!m_pInputBuffer)
				   m_pInputBuffer = new BYTE[csm_inputBufferSize];

			   DWORD buffSize = fillBuffer(csm_inputBufferSize);
		   }
	   }
   }

   setStatus(Ok);
   return true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param in_attemptSize : 描述
 * \return 返回值的描述
 */
//报错点
DWORD PackSubRStream::fillBuffer(const DWORD in_attemptSize)
{
	
	DWORD size = ZipSubRStream::fillBuffer(in_attemptSize);
	if(isEncrypt())
	{
		Assert((size & 7) == 0);
		Assert(m_pDesKey);
		DecryptData(m_pInputBuffer,size,m_pDesKey,m_pDesKey + 8,m_pDesKey + 16);
	}
	m_dwBufferSize = size;
	return size;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param out_pBuffer : 描述
 * \param in_numBytes : 描述
 * \return 返回值的描述
 */
bool PackSubRStream::_read(void *out_pBuffer, const DWORD in_numBytes)
{
	if(m_pOutputBuffer)
	{
		size_t curPos = getPosition();
		size_t toRead = min(in_numBytes,m_uncompressedSize-curPos);
		memcpy(out_pBuffer,m_pOutputBuffer+curPos,toRead);
		setPosition(curPos + toRead);
		return true;
	}

	if(this->isCompress())
		return ZipSubRStream::_read(out_pBuffer,in_numBytes);
	else
	{
		if (in_numBytes == 0)
		return true;

		AssertFatal(out_pBuffer != NULL, "NULL output buffer");
		if (getStatus() == Closed) {
			AssertFatal(false, "Attempted read from closed stream");
			return false;
		}

		DWORD readBytes = 0;
		while(readBytes < in_numBytes)
		{
			DWORD buffPosition;
			DWORD readSize;

			if(m_dwBufferPosition + m_dwBufferSize <= m_currentPosition)
			{
				m_dwBufferPosition = m_currentPosition;
				fillBuffer(m_bFullBuffer ? m_uncompressedSize : csm_inputBufferSize);
			}

			buffPosition = m_currentPosition - m_dwBufferPosition;
			readSize = min((in_numBytes-readBytes),m_dwBufferSize - buffPosition);
			
			if(readSize == 0) break;

			memcpy((BYTE*)out_pBuffer + readBytes,m_pInputBuffer + buffPosition,readSize);
			readBytes += readSize;
			m_currentPosition += readSize;

			if(m_currentPosition >= getSize()) 
				return readBytes == in_numBytes;
		}
		return true;
	}
}