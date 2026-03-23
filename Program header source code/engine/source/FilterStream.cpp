/********************************************************************
	创建时间:	2004/10/31   14:28
	文件名: 	FilterStream.cpp
	文件类型:	cpp
	作者:		$$$03公司
	
	用途:	  包容流，附加在某一流上，按另一种方式输出或输入
*********************************************************************/

// FilterStream.cpp: implementation of the FilterStream class.
//
//////////////////////////////////////////////////////////////////////
#include "../include/engine.h"
#include "../include/FilterStream.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool FilterStream::_read(void* out_pBuffer,const DWORD in_numBytes )
{
	AssertFatal(getStream() != NULL, "Error no stream to pass to");
	
	bool success = getStream()->read(out_pBuffer,in_numBytes );
	
	setStatus(getStream()->getStatus());
	return success;
}


bool FilterStream::_write(const void*,const DWORD )
{
	AssertFatal(false, "No writing allowed to filter");
	return false;
}

bool FilterStream::hasCapability(const Capability in_streamCap) const
{
	// Fool the compiler.  We know better...
	FilterStream* ncThis = const_cast<FilterStream*>(this);
	AssertFatal(ncThis->getStream() != NULL, "Error no stream to pass to");
	
	return ncThis->getStream()->hasCapability(in_streamCap);
}

DWORD FilterStream::getPosition() const
{
	// Fool the compiler.  We know better...
	FilterStream* ncThis = const_cast<FilterStream*>(this);
	AssertFatal(ncThis->getStream() != NULL, "Error no stream to pass to");
	
	return ncThis->getStream()->getPosition();
}

bool FilterStream::setPosition(const DWORD in_newPosition)
{
	AssertFatal(getStream() != NULL, "Error no stream to pass to");
	
	return getStream()->setPosition(in_newPosition);
}

DWORD FilterStream::getSize()
{
	AssertFatal(getStream() != NULL, "Error no stream to pass to");
	
	return getStream()->getSize();
}
