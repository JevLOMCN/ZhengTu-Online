/********************************************************************
	创建时间:	2004/10/31   14:30
	文件名: 	FilterStream.h
	文件类型:	h
	作者:		$$$03公司
	
	用途:	  包容流，附加在某一流上，按另一种方式输出或输入
*********************************************************************/

// FilterStream.h: interface for the FilterStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTERSTREAM_H__F4C63E51_8F60_4753_B11C_8FF9E3148D03__INCLUDED_)
#define AFX_FILTERSTREAM_H__F4C63E51_8F60_4753_B11C_8FF9E3148D03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Stream.h"

class ENGINE_API FilterStream : public Stream
{
public:
	virtual ~FilterStream(){};
	
	virtual bool    attachStream(Stream* io_pSlaveStream) = 0;
	virtual void    detachStream()                        = 0;
	virtual Stream* getStream()                           = 0;
	
	// Mandatory overrides.  By default, these are simply passed to
	//  whatever is returned from getStream();
protected:
	bool _read(void* out_pBuffer, const DWORD in_numBytes );
	bool _write(const void* in_pBuffer, const DWORD in_numBytes);
public:
	bool hasCapability(const Capability) const;
	
	DWORD  getPosition() const;
	bool setPosition(const DWORD in_newPosition);
	DWORD  getSize();
};

#endif // !defined(AFX_FILTERSTREAM_H__F4C63E51_8F60_4753_B11C_8FF9E3148D03__INCLUDED_)
