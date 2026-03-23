/********************************************************************
	创建时间:	2004/10/31   14:35
	文件名: 	Stream.h
	文件类型:	h
	作者:		$$$03公司
	
	用途:	  流基类
*********************************************************************/

// Stream.h: interface for the Stream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STREAM_H__D368B43E_1695_4299_992B_46F31F8845D3__INCLUDED_)
#define AFX_STREAM_H__D368B43E_1695_4299_992B_46F31F8845D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#include "color.h"


class ENGINE_API Stream
{
public:
	enum Status {
      Ok = 0,           ///< Ok!
      IOError,          ///< Read or Write error
      EOS,              ///< End of Stream reached (mostly for reads)
      IllegalCall,      ///< An unsupported operation used.  Always w/ accompanied by AssertWarn
      Closed,           ///< Tried to operate on a closed stream (or detached filter)
      UnknownError      ///< Catchall
   };

   enum Capability {
      StreamWrite    = 1<<0, ///< Can this stream write?
      StreamRead     = 1<<1, ///< Can this stream read?
      StreamPosition = 1<<2  ///< Can this stream position?
   };
private:
	Status m_streamStatus;
public:
	/// Checks to see if this stream has the capability of a given function
	virtual bool hasCapability(const Capability) const = 0;
	
	/// Gets the position in the stream
	virtual DWORD  getPosition() const                      = 0;
	/// Sets the position of the stream.  Returns if the new position is valid or not
	virtual bool setPosition(const DWORD in_newPosition) = 0;
	/// Gets the size of the stream
	virtual DWORD  getSize() = 0;
protected:
	void setStatus(const Status in_newStatus) { m_streamStatus = in_newStatus; }

	virtual bool _read(void* out_pBuffer,const DWORD in_numBytes  )      = 0;
	virtual bool _write(const void* in_pBuffer,const DWORD in_numBytes ) = 0;
public:
	Stream::Status getStatus() const { return m_streamStatus; }
	static const TCHAR* getStatusString(const Status in_status);

	bool read(void* out_pBuffer, const DWORD in_numBytes ) {
		return _read(out_pBuffer,in_numBytes );
	}
	bool write(const void* in_pBuffer,const DWORD in_numBytes ) {
		return _write(in_pBuffer,in_numBytes );
	}
	Stream();
	virtual ~Stream();
	virtual void close(){};
/*
	Stream & operator << (const TCHAR ch)
	{
		write(sizeof(TCHAR),&ch);
		return (*this);
	}
	
	Stream & operator >> (TCHAR & ch)
	{
		read(sizeof(TCHAR),&ch);
		return (*this);
	}

	Stream & operator << (const BYTE ch)
	{
		write(sizeof(BYTE),&ch);
		return (*this);
	}

	Stream & operator >> (BYTE &ch)
	{
		read(sizeof(BYTE),&ch);
		return (*this);
	}

	Stream & operator << (const long ch)
	{
		write(sizeof(long),&ch);
		return (*this);
	}
	
	Stream & operator >> (long &ch)
	{
		read(sizeof(long),&ch);
		return (*this);
	}

	Stream & operator << (const DWORD ch)
	{
		write(sizeof(DWORD),&ch);
		return (*this);
	}
	
	Stream & operator >> (DWORD &ch)
	{
		read(sizeof(DWORD),&ch);
		return (*this);
	}

	Stream & operator << (const long ch)
	{
		write(sizeof(long),&ch);
		return (*this);
	}
	
	Stream & operator >> (long &ch)
	{
		read(sizeof(long),&ch);
		return (*this);
	}

	Stream & operator << (const unsigned long ch)
	{
		write(sizeof(unsigned long),&ch);
		return (*this);
	}
	
	Stream & operator >> (unsigned long &ch)
	{
		read(sizeof(unsigned long),&ch);
		return (*this);
	}

	Stream & operator << (const short ch)
	{
		write(sizeof(short),&ch);
		return (*this);
	}
	
	Stream & operator >> (short &ch)
	{
		read(sizeof(short),&ch);
		return (*this);
	}
	
	Stream & operator << (const unsigned short ch)
	{
		write(sizeof(unsigned short),&ch);
		return (*this);
	}
	
	Stream & operator >> (unsigned short &ch)
	{
		read(sizeof(unsigned short),&ch);
		return (*this);
	}

	Stream & operator << (const TCHAR* psz)
	{
		write((DWORD)strlen(psz),psz);
		return (*this);
	}

//*/
	bool write (char ch)
	{
		return write(&ch,sizeof(char));
	}
	
	bool read (char & ch)
	{
		return read(&ch,sizeof(char));
	}

	bool write (BYTE ch)
	{
		return write(&ch,sizeof(BYTE));
	}

	bool read (BYTE &ch)
	{
		return read(&ch,sizeof(BYTE));
	}

	bool write (long ch)
	{
		return write(&ch,sizeof(long));
	}
	
	bool read (long &ch)
	{
		return read(&ch,sizeof(long));
	}

	bool write (DWORD ch)
	{
		return write(&ch,sizeof(DWORD));
	}
	
	bool read (DWORD &ch)
	{
		return read(&ch,sizeof(DWORD));
	}	

	bool write (short ch)
	{
		return write(&ch,sizeof(short));
	}
	
	bool read (short &ch)
	{
		return read(&ch,sizeof(short));
	}
	
	bool write (unsigned short ch)
	{
		return write(&ch,sizeof(unsigned short));
	}
	
	bool read (unsigned short &ch)
	{
		return read(&ch,sizeof(unsigned short));
	}

	bool write (const char* psz)
	{
		return write(psz,(DWORD)strlen(psz) * sizeof(char));
	}

	bool write (const wchar_t* psz)
	{
		return write(psz,(DWORD)wcslen(psz) * sizeof(wchar_t));
	}
	virtual BYTE* getBuffer(){ return NULL;}
	virtual BYTE* detachBuffer(){ return NULL;}
	BYTE*	getPointer(){
		BYTE* pBuffer = getBuffer();
		if(pBuffer) return pBuffer + getPosition();
		return NULL;
	}
	void setPositionRelative(long pos)
	{
		setPosition((long)getPosition() + pos);
	}
};

#endif // !defined(AFX_STREAM_H__D368B43E_1695_4299_992B_46F31F8845D3__INCLUDED_)
