#ifndef _STRINGEX_H
#define _STRINGEX_H

//-------------------------------------------------------------------------
// 功能: 字符串替换
//-------------------------------------------------------------------------
void str_replace(const char* pszSrc,char* pszDst,const char* pszOld,const char* pszNew);

//-------------------------------------------------------------------------
// 功能: 字符串替换
//-------------------------------------------------------------------------
void str_replace(char* pszSrc,const char* pszOld,const char* pszNew);

//-------------------------------------------------------------------------
// 功能: 字符串替换
//-------------------------------------------------------------------------
void str_replace(char* pszSrc,char cOld,char cNew);

//-------------------------------------------------------------------------
// 功能: 字符串替换
//-------------------------------------------------------------------------
void str_replace(char* pszSrc,wchar_t cOld,wchar_t cNew);

//-------------------------------------------------------------------------
// 功能: 删除字符
//-------------------------------------------------------------------------
void str_del(char* pszSrc,int c);

//-------------------------------------------------------------------------
// Function: Delete continue block of multimedia special chars
//			 Add only left one for each block
//			 For example:	From :"sd我的f￥￥￥￥s花朵fs￥￥sdfs祖国的大好河山df￥￥￥￥￥￥￥￥fg么李fgf￥￥"
//							To   :"sd我的f￥s花朵fs￥sdfs祖国的大好河山df￥fg么李fgf￥"
//							Note :￥ means the multimedia special char
//-------------------------------------------------------------------------
void str_del_sp(char* pszSrc, char c);
void str_del_sp_full(char* pszSrc, char c);		//Same as up fun except that left no sp char after finished

//-------------------------------------------------------------------------
// 功能: 判断是否是一个空白字符串
//-------------------------------------------------------------------------
BOOL str_isempty(const char* psz);

void str_trim_left(char* psz);

void str_trim_right(char* psz);

// 将单行字符串按长度分成多行
void str_split(const char* pszSrc,char* pszDst,size_t nDstBufLen,size_t nLineLen);

size_t my_mbstowcs(wchar_t *wcstr,
				   const char *mbstr,
				   size_t count );
size_t my_wcstombs(char *mbstr,
				   const wchar_t *wcstr,
				   size_t count );
void InitStringData(const wchar_t *wcstr,DWORD* pData,size_t count,DWORD color);
void InitStringData( const char *str,DWORD* pData,size_t count,DWORD color );
WORD my_mbstowcs(WORD ch);
WORD my_wcstombs(WORD ch);
int GetCharacterCount(const char * str);

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pszSrc : 描述
 * \param listString : 描述
 * \param nLineLen : 描述
 * \return 返回值的描述
 */
// 将单行字符串按长度分成多行,如果nLineLen为-1则按换行符来拆分
template <class tVecString >
void str_split(const char* pszSrc,tVecString &listString,size_t nLineLen)
{
	size_t i;
	i =0;
	
	tVecString::value_type * pstrDst = NULL;
	listString.reserve(8);


	while(*pszSrc)
	{
		if((*pszSrc) & 0x80)
		{
			if(!(*(pszSrc+1))) break;

			if(!pstrDst)
			{
				listString.resize(listString.size()+1);
				pstrDst = &listString.back();
				pstrDst->reserve(256);
			}

			pstrDst->push_back(*pszSrc++);
			pstrDst->push_back(*pszSrc++);
			i+=2;

			if(i >= nLineLen)
			{
				// 换行
				Assert(pstrDst);
				pstrDst->push_back(0);
				pstrDst = NULL;
				i = 0;
			}
		}
		else
		{
			if(!pstrDst)
			{
				listString.resize(listString.size()+1);
				pstrDst = &listString.back();
				pstrDst->reserve(256);
			}

			size_t iCharNum;
			if(IsNewLine(pszSrc,iCharNum))
			{
				// 换行
				pstrDst->push_back(0);
				pstrDst = NULL;
				i = 0;
				pszSrc += iCharNum;
			}
			else
			{
				pstrDst->push_back(*pszSrc++);
				i++;

				if( i >= nLineLen)
				{
					// 换行
					Assert(pstrDst);
					pstrDst->push_back(0);
					pstrDst = NULL;
					i = 0;
				}
			}

		}
	}
	if(pstrDst) pstrDst->push_back(0);
}

extern long str_getnextl(const char* pszString,char** endptr,int base);

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param vRet : 描述
 * \param pszString : 描述
 * \param ch : 描述
 * \return 返回值的描述
 */
template < 
class Type, 
class Allocator  
>
void _parse_str(std::vector<Type,Allocator> & vRet,const char* pszString,char ch)
{
	vRet.resize(0);
	const char* p1,*p2;
	p1 = pszString;
	for(;;)
	{
		while(*p1 != 0 && *p1 == ch)
		{
			p1++;
		}

		if(*p1 == 0) break;

		p2 = p1;

		while(*p2 != 0 && *p2!=ch)
		{
			p2++;
		}

		vRet.resize(vRet.size()+1);
		Type & str = vRet.back();
		str.resize(p2 - p1);
		memcpy(&str[0],p1,p2-p1);
		str[p2-p1] = 0;
		if(*p2 == 0) break;

		p1 = p2+1;
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param vRet : 描述
 * \param pszString : 描述
 * \return 返回值的描述
 */
template <class tVectorInt>
void _parse_str_num(tVectorInt & vRet,const char* pszString)
{
	vRet.resize(0);
	const char* p1,*p2;
	p1 = pszString;
	for(;;)
	{
		while(*p1 != 0 && !( '0' <= *p1 && *p1 <= '9'))
		{
			p1++;
		}

		if(*p1 == 0) break;

		p2 = p1;

		while(*p2 != 0 && ( '0' <= *p2 && *p2 <= '9'))
		{
			p2++;
		}

		char szTmp[256];
		int len = getMin(p2-p1,255);
		memcpy(szTmp,p1,len);
		szTmp[len] = 0;

		vRet.push_back(atol(szTmp));

		if(*p2 == 0) break;

		p1 = p2+1;
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param vRet : 描述
 * \param pszString : 描述
 * \return 返回值的描述
 */
template <class tVectorInt>
void _parse_str_num_sign(tVectorInt & vRet,const char* pszString)
{
	vRet.resize(0);
	const char* p1,*p2;
	p1 = pszString;
	for(;;)
	{
		while(*p1 != 0 && !( ( '0' <= *p1 && *p1 <= '9') || *p1 == '-'))
		{
			p1++;
		}


		if(*p1 == 0) break;

		p2 = p1;
		if(*p2 == '-') ++p2;
		if(*p2 == 0) break;

		while(*p2 != 0 && ( '0' <= *p2 && *p2 <= '9' ))
		{
			p2++;
		}

		char szTmp[256];
		int len = getMin(p2-p1,255);
		memcpy(szTmp,p1,len);
		szTmp[len] = 0;

		vRet.push_back(atol(szTmp));

		if(*p2 == 0) break;

		p1 = p2+1;
	}
}

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
		strncat(szString,psz,i);
		szString[i-1] = 0;
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