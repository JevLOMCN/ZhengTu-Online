/********************************************************************
	创建时间:	2004/10/31   14:30
	文件名: 	MemStream.h
	文件类型:	h
	作者:		$$$03公司
	
	用途:	  内存流
*********************************************************************/

// MemStream.h: interface for the MemStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMSTREAM_H__9DE3F9B6_9EA5_466D_A9F7_8A097D4B0B3D__INCLUDED_)
#define AFX_MEMSTREAM_H__9DE3F9B6_9EA5_466D_A9F7_8A097D4B0B3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Stream.h"

class ENGINE_API MemStream : public Stream  
{
	typedef Stream Parent;
	
protected:
	DWORD const cm_bufferSize;
	void*  m_pBufferBase;
	bool	m_bAutoFreeBuffer;
	DWORD m_instCaps;
	DWORD m_currentPosition;
	
public:
	MemStream(const DWORD  in_bufferSize,
		void*      io_pBuffer,
		const bool in_allowRead  = true,
		const bool in_allowWrite = true,
		bool in_autoFreeBuffer = false);
	~MemStream();
	
	// Mandatory overrides from Stream
protected:
	bool _read(void* out_pBuffer, const DWORD in_numBytes );
	bool _write(const void* in_pBuffer, const DWORD in_numBytes);
public:
	bool hasCapability(const Capability) const;
	DWORD  getPosition() const;
	bool setPosition(const DWORD in_newPosition);
	virtual BYTE* getBuffer(){ return (BYTE*)m_pBufferBase;}
	// Mandatory overrides from Stream
public:
	DWORD  getSize();
};

#endif // !defined(AFX_MEMSTREAM_H__9DE3F9B6_9EA5_466D_A9F7_8A097D4B0B3D__INCLUDED_)
