/********************************************************************
	创建时间:	2004/10/31   14:27
	文件名: 	FileStream.cpp
	文件类型:	cpp
	作者:		$$$03公司
	
	用途:	磁盘文件流
*********************************************************************/

// FileStream.cpp: implementation of the FileStream class.
//
//////////////////////////////////////////////////////////////////////
#include "../include/engine.h"
#include "../include/FileStream.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FileStream::FileStream()
{
	m_bOwnBuf = true;
	m_pBuffer = NULL;
	m_nBufSize = 0;
	init();
}

FileStream::FileStream(const TCHAR *i_pFilename, AccessMode i_openMode,size_t fileBufferSize)
{
	m_bOwnBuf = true;
	m_pBuffer = NULL;
	m_nBufSize = 0;
	init();
	open(i_pFilename,i_openMode,fileBufferSize);
}

FileStream::~FileStream()
{
	close();
	if(m_bOwnBuf) free(m_pBuffer);
}

void FileStream::clearBuffer()
{
	m_nBuffHead = BUFFER_INVALID;
	m_nBuffPos  = 0;
	m_nBuffTail = 0;
	m_bDirty = false;
	m_bEOF = false;
}

void FileStream::init()
{
	m_nStreamCaps = 0;
	Stream::setStatus(Closed);
	clearBuffer();
}

void FileStream::setBuf(BYTE* pBuf,DWORD nBufSize)
{
	if (m_bDirty) flush();
	clearBuffer();
	if(m_bOwnBuf) free(m_pBuffer);
	m_pBuffer = pBuf;
	m_nBufSize = nBufSize;
	m_bOwnBuf = false;
}

bool FileStream::open(const TCHAR *i_pFilename, AccessMode i_openMode,size_t fileBufferSize)
{
	assert(NULL != i_pFilename);
	
	if(!m_pBuffer && fileBufferSize)
	{
		m_pBuffer = (BYTE*)malloc(fileBufferSize);
		m_bOwnBuf = true;
	}
	m_nBufSize = fileBufferSize;
	// make sure the file stream's state is clean
	clearBuffer();
	
	if (File::Ok == m_File.open(i_pFilename, (File::AccessMode)i_openMode))
	{
		setStatus();
		switch (i_openMode)
		{
		case Read:
            m_nStreamCaps = DWORD(StreamRead) |
				DWORD(StreamPosition);
            break;
		case Write:
		case WriteAppend:
            m_nStreamCaps = DWORD(StreamWrite) |
				DWORD(StreamPosition);
            break;
		case ReadWrite:
            m_nStreamCaps = DWORD(StreamRead)  |
				DWORD(StreamWrite) |
				DWORD(StreamPosition);
            break;
		default:
			
            AssertFatal(false,"FileStream::open: bad access mode");
		}
	}
	else
	{
		setStatus();
		return(false);
	}   
	return(true);
}

//-----------------------------------------------------------------------------
void FileStream::close()
{
	if (Closed == getStatus())
		return;
	
	// make sure nothing in the buffer differs from what is on disk
	if (m_bDirty)
		flush();
	// and close the file
	File::Status closeResult = m_File.close();
	
	assert(File::Closed == closeResult);
	
	// clear the file stream's state
	init();
}



//-----------------------------------------------------------------------------
void FileStream::calcBlockHead(const DWORD i_position, DWORD *o_blockHead)
{
	assert(NULL != o_blockHead);
	
	*o_blockHead = i_position/m_nBufSize * m_nBufSize;
}

//-----------------------------------------------------------------------------
void FileStream::calcBlockBounds(const DWORD i_position, DWORD *o_blockHead, DWORD *o_blockTail)
{
	assert(NULL != o_blockHead);
	assert(NULL != o_blockTail);
	
	*o_blockHead = i_position/m_nBufSize * m_nBufSize;
	*o_blockTail = *o_blockHead + m_nBufSize - 1;
}

//-----------------------------------------------------------------------------
void FileStream::setStatus()
{
	switch (m_File.getStatus())
	{
	case File::Ok:
		Stream::setStatus(Ok);
		break;
	case File::IOError:
		Stream::setStatus(IOError);
		break;
	case File::EOS:
		Stream::setStatus(EOS);
		break;
	case File::IllegalCall:
		Stream::setStatus(IllegalCall);
		break;
	case File::Closed:
		Stream::setStatus(Closed);
		break;
	case File::UnknownError:
		Stream::setStatus(UnknownError);
		break;
	default:
		AssertFatal(false, "FileStream::setStatus: invalid error mode");
	}
}

bool FileStream::fillBuffer(const DWORD i_startPosition)
{
	assert(0 != m_nStreamCaps);
	assert(false == m_bDirty);
	
	// make sure start position and file pointer jive
	if (i_startPosition != m_File.getPosition())
	{
		m_File.setPosition(i_startPosition);
		if (File::Ok != m_File.getStatus() && File::EOS != m_File.getStatus())
		{
			setStatus();
			return(false);
		}
		else
			// update buffer pointer
			m_nBuffPos = i_startPosition;
	}
	
	// check if file pointer is at end-of-file
	if (EOS == getStatus())
	{
		// invalidate the buffer
		m_nBuffHead = BUFFER_INVALID;
		// set the status to end-of-stream
		m_bEOF = true;
	}
	// otherwise
	else
	{
		DWORD bytesRead = 0;
		DWORD blockHead;
		// locate bounds of buffer containing current position
		calcBlockHead(m_nBuffPos, &blockHead);
		// read as much as possible from input file
		m_File.read((TCHAR *)m_pBuffer + (i_startPosition - blockHead),m_nBufSize - (i_startPosition - blockHead),  &bytesRead);
		setStatus();
		if (Ok == getStatus() || EOS == getStatus())
		{
			// update buffer pointers
			m_nBuffHead = i_startPosition;
			m_nBuffPos = i_startPosition;
			m_nBuffTail = i_startPosition + bytesRead - 1;
			// update end-of-file status
			if (0 != bytesRead && EOS == getStatus())
			{
				Stream::setStatus(Ok);
				m_bEOF = true;
			}
		}
		else
		{
			m_nBuffHead = BUFFER_INVALID;
			return(false);
		}
	}
	return(true);
}

//-----------------------------------------------------------------------------
bool FileStream::flush()
{

	if(m_pBuffer == NULL) 
		return true;

	assert(false == m_bDirty || BUFFER_INVALID != m_nBuffHead);
	
	// if the buffer is dirty
	if (m_bDirty)
	{
		assert(hasCapability(StreamWrite));
		// align the file pointer to the buffer head
		if (m_nBuffHead != m_File.getPosition())
		{
			m_File.setPosition(m_nBuffHead);
			if (File::Ok != m_File.getStatus() && File::EOS != m_File.getStatus())
				return(false);
		}
		// write contents of the buffer to disk
		DWORD blockHead;
		calcBlockHead(m_nBuffHead, &blockHead);	// 计算缓冲区的起始位置
		m_File.write((TCHAR *)m_pBuffer + (m_nBuffHead - blockHead),m_nBuffTail - m_nBuffHead + 1 );
		// and update the file stream's state
		setStatus();
		if (EOS == getStatus())
			m_bEOF = true;
		
		if (Ok == getStatus() || EOS == getStatus())
			// and update the status of the buffer
			m_bDirty = false;
		else
			return(false);
	}
	return(true);
}

bool FileStream::_read(void *o_pBuffer,const DWORD i_numBytes )
{
	assert(0 != m_nStreamCaps);
	assert(NULL != o_pBuffer || i_numBytes == 0);
    
	if (false == hasCapability(Stream::StreamRead))
	{
		AssertFatal( false, "FileStream::_read: file stream lacks capability");
		Stream::setStatus(IllegalCall);
		return(false);
	}
    
	// exit on pre-existing errors
	if (Ok != getStatus())
		return(false);
	
	// if a request of non-zero length was made
	if (0 != i_numBytes)
	{
		if(m_pBuffer == NULL)
		{
			/*
			 *	没有缓冲的情况
			 */
			DWORD readSize;
			return File::Ok == m_File.read(o_pBuffer,i_numBytes,&readSize);
		}

		BYTE *pDst = (BYTE *)o_pBuffer;
		DWORD readSize;
		DWORD remaining = i_numBytes;
		DWORD bytesRead;
		DWORD blockHead;
		DWORD blockTail;
		
		// check if the buffer has some data in it
		if (BUFFER_INVALID != m_nBuffHead)
		{
			// copy as much as possible from the buffer into the destination
			readSize = ((m_nBuffTail + 1) >= m_nBuffPos) ? (m_nBuffTail + 1 - m_nBuffPos) : 0;
			readSize = min(readSize, remaining);
			calcBlockHead(m_nBuffPos, &blockHead);
			memcpy(pDst, m_pBuffer + (m_nBuffPos - blockHead), readSize);
			// reduce the remaining amount to read
			remaining -= readSize;
			// advance the buffer pointers
			m_nBuffPos += readSize;
			pDst += readSize;
			
			if (m_nBuffPos > m_nBuffTail && remaining != 0)
			{
				// 把未写入的缓冲数据写入
				flush();
				m_nBuffHead = BUFFER_INVALID;
				if (m_bEOF)	Stream::setStatus(EOS);
			}
		}
		
		// if the request wasn't satisfied by the buffer and the file has more data
		if (false == m_bEOF && 0 < remaining)
		{
			// flush the buffer if its dirty, since we now need to go to disk
			if (m_bDirty)	flush();   // 把未写入的缓冲数据写入
			
			// make sure we know the current read location in the underlying file
			m_nBuffPos = m_File.getPosition();
			calcBlockBounds(m_nBuffPos, &blockHead, &blockTail);
			
			// check if the data to be read falls within a single block
			if ((m_nBuffPos + remaining) <= blockTail)
			{
				// 如果剩余要读的数据在一个块内
				// fill the buffer from disk
				if (fillBuffer(m_nBuffPos))
				{
					// copy as much as possible from the buffer to the destination
					remaining = min(remaining, m_nBuffTail - m_nBuffPos + 1);
					memcpy(pDst, m_pBuffer + (m_nBuffPos - blockHead), remaining);
					// advance the buffer pointer
					m_nBuffPos += remaining;
				}
				else
					return(false);
			}
			// otherwise the remaining spans multiple blocks
			else
			{
				clearBuffer();
				// read from disk directly into the destination
				m_File.read((TCHAR *)pDst,remaining,  &bytesRead);
				setStatus();
				// check to make sure we read as much as expected
				if (Ok == getStatus() || EOS == getStatus())
				{
					// if not, update the end-of-file status
					if (0 != bytesRead && EOS == getStatus())
					{
						Stream::setStatus(Ok);
						m_bEOF = true;
					}
				}
				else
					return(false);
			}
		}
	}
	return(true);
}

//-----------------------------------------------------------------------------
bool FileStream::_write(const void *i_pBuffer, const DWORD i_numBytes)
{
	AssertFatal(0 != m_nStreamCaps, "FileStream::_write: the stream isn't open");
	AssertFatal(NULL != i_pBuffer || i_numBytes == 0, "FileStream::_write: NULL source buffer pointer on non-zero write request");
    
	if (false == hasCapability(Stream::StreamWrite))
	{
		AssertFatal(false, "FileStream::_write: file stream lacks capability");
		Stream::setStatus(IllegalCall);
		return(false);
	}
    
	// exit on pre-existing errors
	if (Ok != getStatus() && EOS != getStatus())
		return(false);
	
	// if a request of non-zero length was made
	if (0 != i_numBytes)
	{
		if(m_pBuffer == NULL)
		{
			/*
			*	没有缓冲的情况
			*/
			DWORD writeSize;
			return File::Ok == m_File.write(i_pBuffer,i_numBytes,&writeSize);
		}

		BYTE *pSrc = (BYTE *)i_pBuffer;
		DWORD writeSize;
		DWORD remaining = i_numBytes;
		DWORD bytesWrit;
		DWORD blockHead;
		DWORD blockTail;
		
		// check if the buffer is valid
		if (BUFFER_INVALID != m_nBuffHead)
		{
			// copy as much as possible from the source to the buffer
			calcBlockBounds(m_nBuffHead, &blockHead, &blockTail);
			writeSize = (m_nBuffPos > blockTail) ? 0 : blockTail - m_nBuffPos + 1;
			writeSize = min(writeSize, remaining);
			
			AssertFatal(0 == writeSize || (m_nBuffPos - blockHead) < m_nBufSize, "FileStream::_write: out of bounds buffer position");
			memcpy(m_pBuffer + (m_nBuffPos - blockHead), pSrc, writeSize);
			// reduce the remaining amount to be written
			remaining -= writeSize;
			// advance the buffer pointers
			m_nBuffPos += writeSize;
			m_nBuffTail = max(m_nBuffTail, m_nBuffPos - 1);
			pSrc += writeSize;
			// mark the buffer dirty
			if (0 < writeSize)
				m_bDirty = true;
		}
		
		// if the request wasn't satisfied by the buffer
		if (0 < remaining)
		{
			// flush the buffer if its dirty, since we now need to go to disk
			if (m_bDirty) flush(); // 把未写入的缓冲数据写入
			
			// make sure we know the current write location in the underlying file
			m_nBuffPos = m_File.getPosition();
			calcBlockBounds(m_nBuffPos, &blockHead, &blockTail);
			
			// check if the data to be written falls within a single block
			if ((m_nBuffPos + remaining) <= blockTail)
			{
				// 如果剩余要写的数据在一个块内
				// write the data to the buffer
				memcpy(m_pBuffer + (m_nBuffPos - blockHead), pSrc, remaining);
				// update the buffer pointers
				m_nBuffHead = m_nBuffPos;
				m_nBuffPos += remaining;
				m_nBuffTail = m_nBuffPos - 1;
				// mark the buffer dirty
				m_bDirty = true;
			}
			// otherwise the remaining spans multiple blocks
			else
			{
				clearBuffer();
				// write to disk directly from the source
				m_File.write((TCHAR *)pSrc, remaining, &bytesWrit);
				setStatus();
				return(Ok == getStatus() || EOS == getStatus());
			}
		}
	}
	return(true);
}

//-----------------------------------------------------------------------------
bool FileStream::hasCapability(const Capability i_cap) const
{
	return(0 != (DWORD(i_cap) & m_nStreamCaps));
}

DWORD FileStream::getPosition() const
{
	AssertFatal(0 != m_nStreamCaps, "FileStream::getPosition: the stream isn't open");
	AssertFatal(hasCapability(StreamPosition), "FileStream::getPosition(): lacks positioning capability");
	
	// return the position inside the buffer if its valid, otherwise return the underlying file position
	return((BUFFER_INVALID != m_nBuffHead) ? m_nBuffPos : m_File.getPosition());
}

//-----------------------------------------------------------------------------
bool FileStream::setPosition(const DWORD i_newPosition)
{
	AssertFatal(0 != m_nStreamCaps, "FileStream::setPosition: the stream isn't open");
	AssertFatal(true == hasCapability(StreamPosition), "FileStream::setPosition: lacks positioning capability");
	if(m_pBuffer == NULL)
	{
		/*
		*	没有缓冲的情况
		*/
		m_File.setPosition(i_newPosition);
		setStatus();
		return true;

	}

	// if the buffer is valid, test the new position against the bounds of the buffer
	if ((BUFFER_INVALID != m_nBuffHead) && (i_newPosition >= m_nBuffHead) && (i_newPosition <= m_nBuffTail))
	{
		// set the position and return
		m_nBuffPos = i_newPosition;
		return(true);
	}
	// otherwise the new position lies in some block not in memory
	else
	{
		// flush the buffer if its dirty
		if (m_bDirty) flush();
		// and clear out the state of the file stream
		clearBuffer();
		
		// and set the new position
		m_File.setPosition((DWORD)i_newPosition);
		// update the stream to reflect the file's state
		setStatus();
		// taking end-of-file into consideration
		if ((DWORD)EOS == (DWORD)(m_File.getStatus()))
			m_bEOF = true;
		// and return good states
		return(Ok == getStatus() || EOS == getStatus());
	}
}

//-----------------------------------------------------------------------------
DWORD FileStream::getSize()
{
	//AssertWarn(0 != mStreamCaps, "FileStream::getSize(): the stream isn't open");
	AssertFatal((BUFFER_INVALID != m_nBuffHead && true == m_bDirty) || false == m_bDirty, "FileStream::getSize(): buffer must be valid if its dirty");
	
	// the stream size may not match the size on-disk if its been written to...
	if (m_bDirty && m_pBuffer)
		return(max(m_File.getSize(), m_nBuffTail + 1));
	// otherwise just get the size on disk...
	else
		return(m_File.getSize());
}