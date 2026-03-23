/**
 * \file      ZipSubStream.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ZIP包中子文件流
 * 
 *	      ZIP包中子文件流
 */

// ZipSubStream.h: interface for the ZipSubRStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZIPSUBSTREAM_H__46676B84_6257_44EA_B22F_4E61B067F62E__INCLUDED_)
#define AFX_ZIPSUBSTREAM_H__46676B84_6257_44EA_B22F_4E61B067F62E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct z_stream_s;

#include "FilterStream.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class ZipSubRStream : public FilterStream
{
	typedef FilterStream Parent;
protected:
	static const DWORD csm_streamCaps;
	static const DWORD csm_inputBufferSize;

	bool		m_bFullBuffer;
	Stream*		m_pStream;
	DWORD		m_uncompressedSize;
	DWORD		m_currentPosition;

	z_stream_s*  m_pZipStream;
	BYTE*        m_pInputBuffer;
	BYTE*		 m_pOutputBuffer;
	DWORD        m_originalSlavePosition;
	DWORD		 m_curSlavePosition;

	virtual DWORD fillBuffer(const DWORD in_attemptSize);
	
public:
	ZipSubRStream(bool bFullBuffer = false);
	virtual ~ZipSubRStream();
	
	// Overrides of NFilterStream
public:
	virtual bool    attachStream(Stream* io_pSlaveStream);
	virtual void    detachStream();
	Stream* getStream();
	
	void setUncompressedSize(const DWORD);
	
	// Mandatory overrides.  By default, these are simply passed to
	//  whatever is returned from getStream();
protected:
	bool _read(void* out_pBuffer, const DWORD in_numBytes);
public:
	bool hasCapability(const Capability) const;
	
	DWORD  getPosition() const;
	bool setPosition(const DWORD in_newPosition);

	DWORD  getSize();
	virtual BYTE* getBuffer(){ return m_bFullBuffer ? m_pOutputBuffer : NULL;}
	virtual BYTE* detachBuffer(){ 
		BYTE* pBuffer = m_pOutputBuffer;
		m_pOutputBuffer = NULL;
		detachStream();
		return pBuffer;
	}
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class ZipSubWStream : public FilterStream
{
	typedef FilterStream Parent;

	static const DWORD csm_streamCaps;
	static const DWORD csm_bufferSize;
	
	Stream*      m_pStream;
	z_stream_s*  m_pZipStream;
	
	DWORD          m_currPosition;  // Indicates number of _uncompressed_ bytes written
	
	BYTE*          m_pOutputBuffer;
	BYTE*          m_pInputBuffer;
	
public:
	ZipSubWStream();
	virtual ~ZipSubWStream();
	
	// Overrides of NFilterStream
public:
	bool    attachStream(Stream* io_pSlaveStream);
	void    detachStream();
	Stream* getStream();
	
	// Mandatory overrides.  By default, these are simply passed to
	//  whatever is returned from getStream();
protected:
	bool _read(void* out_pBuffer,  const DWORD in_numBytes);
	bool _write(const void* in_pBuffer, const DWORD in_numBytes);
public:
	bool hasCapability(const Capability) const;
	
	DWORD  getPosition() const;
	bool setPosition(const DWORD in_newPosition);
	
	DWORD  getSize();
};

#endif // !defined(AFX_ZIPSUBSTREAM_H__46676B84_6257_44EA_B22F_4E61B067F62E__INCLUDED_)
