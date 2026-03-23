/********************************************************************
	创建时间:	2004/10/31   15:12
	文件名: 	ResizeFilterStream.h
	文件类型:	h
	作者:		$$$03公司
	
	用途:	  包容流，套在另一个流上的流,按另一种方式输出，可保持另一个流的状态不变
*********************************************************************/

// ResizeFilterStream.h: interface for the ResizeFilterStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESIZEFILTERSTREAM_H__9DA0B010_D1AC_4760_BD8A_DF77AE4CE835__INCLUDED_)
#define AFX_RESIZEFILTERSTREAM_H__9DA0B010_D1AC_4760_BD8A_DF77AE4CE835__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilterStream.h"

// 套在另一个流上的流,可保持另一个流的状态不变
class ENGINE_API ResizeFilterStream : public FilterStream
{
	typedef FilterStream Parent;
	
	Stream* m_pStream;
	DWORD     m_startOffset;
	DWORD     m_streamLen;
	DWORD     m_currOffset;
	
public:
	ResizeFilterStream();
	~ResizeFilterStream();
	
	bool    attachStream(Stream* io_pSlaveStream);
	void    detachStream();
	Stream* getStream();
	
	bool setStreamOffset(const DWORD in_startOffset,
		const DWORD in_streamLen);
	
	// Mandatory overrides.
protected:
	bool _read( void* out_pBuffer, const DWORD in_numBytes);
public:
	DWORD  getPosition() const;
	bool setPosition(const DWORD in_newPosition);
	
	DWORD  getSize();
	BYTE* getBuffer();
};

#endif // !defined(AFX_RESIZEFILTERSTREAM_H__9DA0B010_D1AC_4760_BD8A_DF77AE4CE835__INCLUDED_)
