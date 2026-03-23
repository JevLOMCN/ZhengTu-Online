/********************************************************************
	创建时间:	2004/10/31   14:30
	文件名: 	MemStream.cpp
	文件类型:	cpp
	作者:		$$$03公司
	
	用途:	  内存流
*********************************************************************/

// MemStream.cpp: implementation of the MemStream class.
//
//////////////////////////////////////////////////////////////////////
#include "../include/engine.h"
#include "../include/MemStream.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MemStream::MemStream(const DWORD   in_bufferSize,
                     void*        io_pBuffer,
                     const bool   in_allowRead,
                     const bool   in_allowWrite,
					 bool in_autoFreeBuffer)
: cm_bufferSize(in_bufferSize),
m_pBufferBase(io_pBuffer),
m_bAutoFreeBuffer(in_autoFreeBuffer),
m_instCaps(0),
m_currentPosition(0)
{
	AssertFatal(io_pBuffer != NULL, "Invalid buffer pointer");
	AssertFatal(in_bufferSize > 0,  "Invalid buffer size");
	AssertFatal(in_allowRead || in_allowWrite, "Either write or read must be allowed");
	
	if (in_allowRead)
		m_instCaps |= Stream::StreamRead;
	if (in_allowWrite)
		m_instCaps |= Stream::StreamWrite;
	
	setStatus(Ok);
}

MemStream::~MemStream()
{
	close();
	if(m_bAutoFreeBuffer)
		delete [] m_pBufferBase;
	m_pBufferBase     = NULL;
	m_currentPosition = 0;
	
	setStatus(Closed);
}

bool MemStream::_read(void *out_pBuffer, const DWORD in_numBytes)
{
	AssertFatal(getStatus() != Closed, "Attempted read from a closed stream");
	
	if (in_numBytes == 0)
		return true;
	
	AssertFatal(out_pBuffer != NULL, "Invalid output buffer");
	
	if (hasCapability(StreamRead) == false) {
		//AssertWarn(false, "Reading is disallowed on this stream");
		setStatus(IllegalCall);
		return false;
	}
	
	bool success     = true;
	DWORD  actualBytes = in_numBytes;
	if ((m_currentPosition + in_numBytes) > cm_bufferSize) 
	{
		success = false;
		actualBytes = cm_bufferSize - m_currentPosition;
	}
	
	// Obtain a current pointer, and do the copy
	const void* pCurrent = (const void*)((const BYTE*)m_pBufferBase + m_currentPosition);
	memcpy(out_pBuffer, pCurrent, actualBytes);
	
	// Advance the stream position
	m_currentPosition += actualBytes;
	
	if (!success)
		setStatus(EOS);
	else
		setStatus(Ok);
	
	return success;
}

bool MemStream::_write(const void *in_pBuffer,const DWORD in_numBytes )
{
	AssertFatal(getStatus() != Closed, "Attempted write to a closed stream");
	
	if (in_numBytes == 0)
		return true;
	
	AssertFatal(in_pBuffer != NULL, "Invalid input buffer");
	
	if (hasCapability(StreamWrite) == false) {
		//AssertWarn(0, "Writing is disallowed on this stream");
		setStatus(IllegalCall);
		return false;
	}
	
	bool success     = true;
	DWORD  actualBytes = in_numBytes;
	if ((m_currentPosition + in_numBytes) > cm_bufferSize) {
		success = false;
		actualBytes = cm_bufferSize - m_currentPosition;
	}
	
	// Obtain a current pointer, and do the copy
	void* pCurrent = (void*)((BYTE*)m_pBufferBase + m_currentPosition);
	memcpy(pCurrent, in_pBuffer, actualBytes);
	
	// Advance the stream position
	m_currentPosition += actualBytes;
	
	if (m_currentPosition == cm_bufferSize)
		setStatus(EOS);
	else
		setStatus(Ok);
	
	return success;
}

DWORD MemStream::getSize()
{
	AssertFatal(getStatus() != Closed, "Stream not open, size undefined");
	
	return cm_bufferSize;
}

bool MemStream::hasCapability(const Capability in_cap) const
{
	// Closed streams can't do anything
	//
	if (getStatus() == Closed)
		return false;
	
	DWORD totalCaps = DWORD(Stream::StreamPosition) | m_instCaps;
	
	return (DWORD(in_cap) & totalCaps) != 0;
}

DWORD MemStream::getPosition() const
{
	AssertFatal(getStatus() != Closed, "Position of a closed stream is undefined");
	
	return m_currentPosition;
}

bool MemStream::setPosition(const DWORD in_newPosition)
{
	AssertFatal(getStatus() != Closed, "SetPosition of a closed stream is not allowed");
	AssertFatal(in_newPosition <= cm_bufferSize, "Invalid position");
	
	m_currentPosition = in_newPosition;
	if (m_currentPosition > cm_bufferSize) {
		// Never gets here in debug version, this is for the release builds...
		//
		setStatus(UnknownError);
		return false;
	} else if (m_currentPosition == cm_bufferSize) {
		setStatus(EOS);
	} else {
		setStatus(Ok);
	}
	
	return true;
}