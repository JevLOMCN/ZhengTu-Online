/**
 * \file      PackSubStream.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     Pak文件包中的子文件流，支持加密压缩
 * 
 *	      Pak文件包中的子文件流，支持加密压缩
 */

#pragma once

#include "filterstream.h"
#include "ZipSubStream.h"
#include ".\packaggregate.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class PackSubRStream :
	public ZipSubRStream
{
	DWORD m_dwFlags;
	DWORD m_dwBufferPosition;
	DWORD m_dwBufferSize;
	const BYTE* m_pDesKey;
	DWORD fillBuffer(const DWORD in_attemptSize);
public:
	PackSubRStream(DWORD dwFlags,bool bFullBuffer = false);
	~PackSubRStream(void);

	bool isEncrypt() { return (m_dwFlags & PackAggregate::flagEncrypt) != 0 ;}
	bool isCompress(){ return (m_dwFlags & PackAggregate::flagCompress) != 0 ;}

	bool    attachStream(Stream* io_pSlaveStream);

	bool _read(void *out_pBuffer, const DWORD in_numBytes);

	void SetDesKey(const BYTE* pKey){
		m_pDesKey = pKey;
	}
	
	virtual BYTE* getBuffer(){ 
		if(!isEncrypt())
			return ZipSubRStream::getBuffer();
		return NULL;
	}

	virtual BYTE* detachBuffer(){ 
		if(!isEncrypt())
			return ZipSubRStream::detachBuffer();
		return NULL;
	}
};
