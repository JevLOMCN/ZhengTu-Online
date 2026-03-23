/********************************************************************
	创建时间:	2004/10/31   15:12
	文件名: 	ResizeFilterStream.cpp
	文件类型:	cpp
	作者:		$$$03公司
	
	用途:	  包容流，套在另一个流上的流,按另一种方式输出，可保持另一个流的状态不变
*********************************************************************/

// ResizeFilterStream.cpp: implementation of the ResizeFilterStream class.
//
//////////////////////////////////////////////////////////////////////
#include "../include/engine.h"
#include "../include/ResizeFilterStream.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ResizeFilterStream::ResizeFilterStream()
: m_pStream(NULL),
m_startOffset(0),
m_streamLen(0),
m_currOffset(0)
{
	//
}

ResizeFilterStream::~ResizeFilterStream()
{
	detachStream();
}

bool ResizeFilterStream::attachStream(Stream* io_pSlaveStream)
{
	AssertFatal(io_pSlaveStream != NULL, "NULL Slave stream?");
	
	m_pStream     = io_pSlaveStream;
	m_startOffset = 0;
	m_streamLen   = m_pStream->getSize();
	m_currOffset  = 0;
	setStatus(EOS);
	return true;
}

void ResizeFilterStream::detachStream()
{
	m_pStream     = NULL;
	m_startOffset = 0;
	m_streamLen   = 0;
	m_currOffset  = 0;
	setStatus(Closed);
}

Stream* ResizeFilterStream::getStream()
{
	return m_pStream;
}

bool ResizeFilterStream::setStreamOffset(const DWORD in_startOffset, const DWORD in_streamLen)
{
	AssertFatal(m_pStream != NULL, "stream not attached!");
	if (m_pStream == NULL)
		return false;
	
	DWORD start  = in_startOffset;
	DWORD end    = in_startOffset + in_streamLen;
	DWORD actual = m_pStream->getSize();
	
	if (start >= actual || end > actual)
		return false;
	
	m_startOffset = start;
	m_streamLen   = in_streamLen;
	m_currOffset  = 0;
	
	if (m_streamLen != 0)
		setStatus(Ok);
	else
		setStatus(EOS);
	
	return true;
}

DWORD ResizeFilterStream::getPosition() const
{
	AssertFatal(m_pStream != NULL, "Error, stream not attached");
	if (m_pStream == NULL)
		return 0;
	
	return m_currOffset;
}

bool ResizeFilterStream::setPosition(const DWORD in_newPosition)
{
	AssertFatal(m_pStream != NULL, "Error, stream not attached");
	if (m_pStream == NULL)
		return false;
	
	if (in_newPosition < m_streamLen) {
		m_currOffset = in_newPosition;
		return true;
	} else {
		m_currOffset = m_streamLen;
		return false;
	}
}

DWORD ResizeFilterStream::getSize()
{
	AssertFatal(m_pStream != NULL, "Error, stream not attached");
	
	return m_streamLen;
}

bool ResizeFilterStream::_read( void* out_pBuffer,const DWORD in_numBytes)
{
	AssertFatal(m_pStream != NULL, "Error, stream not attached");
	
	if (in_numBytes == 0)
		return true;
	
	AssertFatal(out_pBuffer != NULL, "Invalid output buffer");
	if (getStatus() == Closed) {
		AssertFatal(false, "Attempted read from closed stream");
		return false;
	}
	
	DWORD savePosition = m_pStream->getPosition();
	if (m_pStream->setPosition(m_startOffset + m_currOffset) == false)
		return false;
	
	DWORD actualSize = in_numBytes;
	DWORD position   = m_startOffset + m_currOffset;
	if (in_numBytes + position > m_startOffset + m_streamLen)
		actualSize = m_streamLen - (position - m_startOffset);
	
	if (actualSize == 0) {
		setStatus(EOS);
		return false;
	}
	
	bool success = m_pStream->read(out_pBuffer,actualSize );
	m_currOffset += actualSize;
	
	setStatus(m_pStream->getStatus());
	
	m_pStream->setPosition(savePosition);
	return success;
}

BYTE* ResizeFilterStream::getBuffer()
{
	BYTE* pAddress = m_pStream->getBuffer();
	if(!pAddress) return NULL;
	return pAddress + m_startOffset;
}