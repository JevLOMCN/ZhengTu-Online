/********************************************************************
	创建时间:	2004/10/31   14:28
	文件名: 	FileStream.h
	文件类型:	h
	作者:		$$$03公司
	
	用途:	  磁盘文件流
*********************************************************************/

// FileStream.h: interface for the FileStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILESTREAM_H__FF9D2A68_20F5_4A74_BACE_D2F00B8A8AEB__INCLUDED_)
#define AFX_FILESTREAM_H__FF9D2A68_20F5_4A74_BACE_D2F00B8A8AEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stream.h"
#include "File.h"

class ENGINE_API FileStream  : public Stream
{
public:
	enum AccessMode
	{
		Read = File::Read,
		Write = File::Write,
		ReadWrite = File::ReadWrite,
		WriteAppend = File::WriteAppend
	};
	enum
	{
		BUFFER_SIZE = 8 * 1024,         // this can be changed to anything appropriate
		BUFFER_INVALID = 0xffffffff      // file offsets must all be less than this
	};
	
private:
	File	m_File;                         // file being streamed
	DWORD	m_nStreamCaps;                   // dependent on access mode
	BYTE*	m_pBuffer;
	DWORD	m_nBufSize;
	bool	m_bOwnBuf;
	DWORD	m_nBuffHead;                     // 未写入的数据起始位置 first valid position of buffer (from start-of-file)
	DWORD	m_nBuffPos;                      // 文件位置 next read or write will occur here
	DWORD	m_nBuffTail;                     // 未写入的数据结束位置 last valid position in buffer (inclusive)
	bool	m_bDirty;                        // whether buffer has been written to
	bool	m_bEOF;                          // whether disk reads have reached the end-of-file
	
public:
	FileStream();
	~FileStream();
	FileStream(const TCHAR *i_pFilename, AccessMode i_openMode,size_t fileBufferSize = BUFFER_SIZE);
	void setBuf(BYTE* pBuf,DWORD nBufSize);
	bool open(const TCHAR *i_pFilename, AccessMode i_openMode,size_t fileBufferSize = BUFFER_SIZE);
	void close();
	bool flush();
	bool fillBuffer(const DWORD i_startPosition);
	void init();
	void calcBlockHead(const DWORD i_position, DWORD *o_blockHead);
	void calcBlockBounds(const DWORD i_position, DWORD *o_blockHead, DWORD *o_blockTail);
	bool hasCapability(const Capability i_cap) const;
protected:
	bool _read(void *o_pBufferconst ,const DWORD i_numBytes );
	bool _write(const void *i_pBuffer,const DWORD i_numBytes );
	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	void setStatus();
	void clearBuffer();
public:
	DWORD getPosition() const;
	
	//-----------------------------------------------------------------------------
	bool setPosition(const DWORD i_newPosition);
	
	//-----------------------------------------------------------------------------
	DWORD getSize();
};

#endif // !defined(AFX_FILESTREAM_H__FF9D2A68_20F5_4A74_BACE_D2F00B8A8AEB__INCLUDED_)
