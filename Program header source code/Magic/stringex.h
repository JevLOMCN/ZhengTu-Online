#ifdef MAGIC_EDITOR

#ifndef _STRINGEX_H
#define _STRINGEX_H
#define GDK_API 
//-------------------------------------------------------------------------
// 功能: 字符串替换
//-------------------------------------------------------------------------
GDK_API void str_replace(const char* pszSrc,char* pszDst,const char* pszOld,const char* pszNew);

//-------------------------------------------------------------------------
// 功能: 字符串替换
//-------------------------------------------------------------------------
GDK_API void str_replace(char* pszSrc,const char* pszOld,const char* pszNew);

//-------------------------------------------------------------------------
// 功能: 字符串替换
//-------------------------------------------------------------------------
GDK_API void str_replace(char* pszSrc,char cOld,char cNew);

//-------------------------------------------------------------------------
// 功能: 字符串替换
//-------------------------------------------------------------------------
GDK_API void str_replace(char* pszSrc,wchar_t cOld,wchar_t cNew);

//-------------------------------------------------------------------------
// 功能: 删除字符
//-------------------------------------------------------------------------
GDK_API void str_del(char* pszSrc,char c);

//-------------------------------------------------------------------------
// 功能: 删除字符
//-------------------------------------------------------------------------
GDK_API void str_del(char* pszSrc,wchar_t c);

//-------------------------------------------------------------------------
// 功能: 判断是否是一个空白字符串
//-------------------------------------------------------------------------
GDK_API BOOL str_isempty(const char* psz);

//-------------------------------------------------------------------------
// 功能: 安全字符串拷贝, 防止越界, 防止半个汉字拷贝
//-------------------------------------------------------------------------
GDK_API char* str_cpy(char* pszDest,const char* pszSrc,int len/* 目标长度(包括最后的'\0')*/);

GDK_API void str_trim_left(char* psz);

GDK_API void str_trim_right(char* psz);

GDK_API void str_cat(char* strDest,const char* strSource,size_t count);

template <int i>
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class tString{
	char szString[i];
public:
	typedef tString<i> _Myt;
	//operator char* () {return szString;}
	operator const char* () const {return szString;}
	tString()
	{
		szString[0] = 0;
	}
	tString(const char* psz)
	{
		(*this) = psz;
	}
	_Myt & operator = (const char* psz) 
	{
		strncpy(szString,psz,i);
		szString[i-1] = 0;
		return (*this);
	}
	BOOL operator == (const char* psz) const 
	{
		return (strcmp(szString,psz) == 0);
	}
	BOOL operator != (const char* psz) const
	{
		return strcmp(szString,psz);
	}
	_Myt & operator +=(const char* psz)
	{
		str_cat(szString,psz,i);
		return (*this);
	}
	
	_Myt & operator += (char ch)
	{
		size_t len = strlen(szString);
		if(len < i-1)
		{
			szString[len] = ch;
			szString[len+1] = 0;
		}
		return (*this);
	}
	
	BOOL operator > (const char* psz)
	{
		return strcmp(szString,psz) > 0;
	}

	BOOL operator >= (const char* psz)
	{
		return strcmp(szString,psz) >= 0;
	}

	BOOL operator < (const char* psz)
	{
		return strcmp(szString,psz) < 0;
	}

	BOOL operator <= (const char* psz)
	{
		return strcmp(szString,psz) <= 0;
	}

	size_t length() const
	{
		return strlen(szString);
	}
	void set(char ch,size_t count)
	{
		count = (count < (i-1) ? count : (i-1));
		memset(szString,ch,count);
		szString[count] = 0;
	}
	void set(const char* sz,size_t count)
	{
		count = (count < (i-1) ? count : (i-1));
		memcpy(szString,sz,count);
		szString[count] = 0;
	}

	void set(size_t index,const char* psz)
	{
		strcpy(&szString[index],psz);
	}

	void setAt(size_t index,char ch)
	{
		assert(index < i);
		szString[index] = ch;
	}

	void Delete(size_t index,size_t count)
	{
		strcpy(&szString[index],&szString[index + count]);
	}
	
	void Format(const char* szFmt,...)
	{
		va_list ap;
		va_start( ap, szFmt );
		_vsnprintf(szString,i,szFmt,ap);
		szString[i-1] = 0;
		va_end  ( ap );
	}
};

#endif

#endif