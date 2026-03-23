#pragma once
#include "stream.h"


class MemMapFileHandle;
class MemMapFileStream :
	public Stream
{
	MemMapFileHandle * mHandle;
	DWORD			   mPosition;
	bool			   mFullMap;
public:

	MemMapFileStream( void );
	MemMapFileStream(const char* filename,bool bFullMap);
	~MemMapFileStream( void );

	bool open(const char* filename,bool bFullMap);
	void close();
	bool _read(void *o_pBuffer,const DWORD i_numBytes );
	bool _write(const void *,const DWORD);
	DWORD  getPosition() const;
	bool setPosition(const DWORD in_newPosition);
	DWORD  getSize();
	bool hasCapability(const Capability) const;
	BYTE* getBuffer();
};
