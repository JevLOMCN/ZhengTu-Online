#pragma once

#include <ostream>
#include <iostream>
#include <sstream>
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class iLog
{
protected:

	CLock m_lock;
	
public:
	 //*
	
	virtual void writeString(const char* value) = 0;

	template <class T >
		iLog & operator << ( T & value )
	{
		std::stringstream streamTemp;
		streamTemp << value;
		writeString(streamTemp.str().c_str());
		return (*this);
	}

	iLog & operator << ( const char* value )
	{
		writeString(value);
		return (*this);
	}
	//*/
	void flagTime()
	{
		char szTime[256];
		__time64_t time = _time64(NULL);
		struct tm* ptmTemp = _localtime64(&time);
		if (ptmTemp == NULL || !strftime(szTime, 256, "%H:%M, %B %d, %Y\n", ptmTemp))
			szTime[0] = '\0';
		(*this) << szTime ;
	}
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CConsoleLog : public iLog{
protected:
	void writeString(const char* value)
	{
		std::cout << value;
	}
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CFileLog : public iLog{
	char			m_szFileName[MAX_PATH];
	FILE*			m_stream;
protected:
public:
	~CFileLog()
	{
		close();
	}

	void writeString(const char* value)
	{
		if(m_stream)
		{
			CSafeLock lock(&m_lock);
			fwrite(value,strlen(value),1,m_stream);
		}
	}

	void close()
	{
		if(m_stream)
		{
			CSafeLock lock(&m_lock);
			fclose(m_stream);
			m_stream = NULL;
		}
	}
	CFileLog(const char* fileName = NULL)
		: m_stream(NULL)
	{
		create(fileName);
	}

	const char* GetFileName() { return m_szFileName;}
	
	bool create(const char* fileName = NULL)
	{
		char szFileName[MAX_PATH];
		if(fileName == NULL)
		{
			GetModuleFileName(NULL,szFileName,MAX_PATH);
			char* p = strrchr(szFileName,'.');
			if(p)
			{
				strcpy(p,".log");
			}
			fileName = szFileName;
		}
		strcpy(m_szFileName,fileName);
		m_stream = fopen(fileName,"w+");
		//flagTime();
		return true;
	}
};
