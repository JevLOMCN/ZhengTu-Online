
#include <malloc.h>
#include "../include/engine.h"
#include <string.h>
#include <ctype.h>
#include <mbstring.h>
#include "../include/stringex.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszSrc : ÃèÊö
 * \param pszDst : ÃèÊö
 * \param pszOld : ÃèÊö
 * \param pszNew : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-------------------------------------------------------------------------
// ¹¦ÄÜ: ×Ö·û´®Ìæ»»
//-------------------------------------------------------------------------
void str_replace(const char* pszSrc,char* pszDst,const char* pszOld,const char* pszNew)
{
	const char* p,*q;
	char *q1;

	int len_old = (int)strlen(pszOld);
	int len_new = (int)strlen(pszNew);

	int l;
	q = pszSrc;
	q1 = pszDst;
	for(;;)
	{
		p = strstr(q,pszOld);
		if(p == NULL)
		{
			strcpy(q1,q);
			break;
		}
		else
		{
			l = (int)(p-q);
			memcpy(q1,q,l);
			q1 += l;
			memcpy(q1,pszNew,len_new);
			q1 += len_new;
			q = p + len_old;
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszSrc : ÃèÊö
 * \param pszOld : ÃèÊö
 * \param pszNew : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-------------------------------------------------------------------------
// ¹¦ÄÜ: ×Ö·û´®Ìæ»»
//-------------------------------------------------------------------------
void str_replace(char* pszSrc,const char* pszOld,const char* pszNew)
{
	int len = (int)strlen(pszSrc);
	char * psz = (char*)_alloca(len*2);

	str_replace(pszSrc,psz,pszOld,pszNew);

	strcpy(pszSrc,psz);

}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszSrc : ÃèÊö
 * \param cOld : ÃèÊö
 * \param cNew : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-------------------------------------------------------------------------
// ¹¦ÄÜ: ×Ö·û´®Ìæ»»
//-------------------------------------------------------------------------
void str_replace(char* pszSrc,char cOld,char cNew)
{
	while(*pszSrc)
	{
		if((*pszSrc) & 0x80)
		{
			// ºº×Ö
			if(!(*(pszSrc+1))) break;

			pszSrc+=2;
		}
		else
		{
			if(*pszSrc == cOld)
				*pszSrc = cNew;
			pszSrc++;
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszSrc : ÃèÊö
 * \param cOld : ÃèÊö
 * \param cNew : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-------------------------------------------------------------------------
// ¹¦ÄÜ: ×Ö·û´®Ìæ»»
//-------------------------------------------------------------------------
void str_replace(char* pszSrc,wchar_t cOld,wchar_t cNew)
{
	while(*pszSrc)
	{
		if((*pszSrc) & 0x80)
		{
			// ºº×Ö
			if(!(*(pszSrc+1))) break;
			if((*(wchar_t*)pszSrc) == cOld)
			{
				(*(wchar_t*)pszSrc) = cNew;
			}
			pszSrc+=2;
		}
		else
		{
			pszSrc++;
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszSrc : ÃèÊö
 * \param c : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-------------------------------------------------------------------------
// ¹¦ÄÜ: É¾³ı×Ö·û
//-------------------------------------------------------------------------
static void str_del(char* pszSrc,char c)
{
	while(*pszSrc)
	{
		if((*pszSrc) & 0x80)
		{
			// ºº×Ö
			if(!(*(pszSrc+1))) break;

			pszSrc+=2;
		}
		else
		{
			if((*pszSrc) == c)
			{
				strcpy(pszSrc,pszSrc+1);
			}
			else
			{
				pszSrc++;
			}
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszSrc : ÃèÊö
 * \param c : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-------------------------------------------------------------------------
// ¹¦ÄÜ: É¾³ı×Ö·û
//-------------------------------------------------------------------------
static void str_del(char* pszSrc,wchar_t c)
{
	while(*pszSrc)
	{
		if((*pszSrc) & 0x80)
		{
			// ºº×Ö

			if(!(*(pszSrc+1))) break;

			if((*(wchar_t*)pszSrc) == c)
			{
				strcpy(pszSrc,pszSrc+2);
			}
			else
			{
				pszSrc+=2;
			}
		}
		else
		{
			pszSrc++;
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszSrc : ÃèÊö
 * \param c : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void str_del(char* pszSrc,int c)
{
	if(c & 0x80)
		str_del(pszSrc,(wchar_t)c);
	else
		str_del(pszSrc,(char)c);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszSrc : ÃèÊö
 * \param c : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-------------------------------------------------------------------------
// Function: Delete continue block of multimedia special chars
//			 Add only left one for each block
//			 For example:	From :"sdÎÒµÄf£¤£¤£¤£¤s»¨¶äfs£¤£¤sdfs×æ¹úµÄ´óºÃºÓÉ½df£¤£¤£¤£¤£¤£¤£¤£¤fgÃ´Àîfgf£¤£¤"
//							To   :"sdÎÒµÄf£¤s»¨¶äfs£¤sdfs×æ¹úµÄ´óºÃºÓÉ½df£¤fgÃ´Àîfgf£¤"
//							Note :£¤ means the multimedia special char
//-------------------------------------------------------------------------
void str_del_sp( char* pszSrc, char c )
{
	char* pszStr = pszSrc;
	while ( *pszStr )
	{
		if ( *pszStr == c )
		{
			//we get one
			if ( !(*(pszStr+1)) )
			{
				strcpy( pszSrc, pszStr );
				break;
			}

			if ( *(pszStr+1) == c )
			{
				pszStr++ ;
			}
			else
			{
				strcpy( pszSrc++, pszStr );
				pszStr = pszSrc;
			}

		}
		else
		{
			if ( (*pszStr) & 0x80 )
			{
				//we get chinese
				if ( !(*(pszStr+1)) )	break;
				pszStr += 2;

			}
			else
			{
				pszStr++ ;
			}
			pszSrc = pszStr;
		}		
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszSrc : ÃèÊö
 * \param c : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//Same as up func except that left no sp char after finished
void str_del_sp_full( char* pszSrc, char c )
{
	char* pszStr = pszSrc;
	while ( *pszStr )
	{
		if ( *pszStr == c )
		{
			//we get one
			if ( !(*(pszStr+1)) )
			{
				strcpy( pszSrc, pszStr+1 );
				break;
			}			
			else
			{
				if ( *(pszStr+1) == c )
				{
					pszStr++ ;
				}
				else
				{
					strcpy( pszSrc, ++pszStr );
					pszStr = pszSrc;
				}

			}

		}
		else
		{
			if ( (*pszStr) & 0x80 )
			{
				//we get chinese
				if ( !(*(pszStr+1)) )	break;
				pszStr += 2;

			}
			else
			{
				pszStr++ ;
			}
			pszSrc = pszStr;
		}		
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param psz : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-------------------------------------------------------------------------
// ¹¦ÄÜ: ÅĞ¶ÏÊÇ·ñÊÇÒ»¸ö¿Õ°××Ö·û´®
//-------------------------------------------------------------------------
BOOL str_isempty(const char* psz)
{
	if(psz == NULL) return TRUE;
	while(*psz)
	{
		if((*psz) & 0x80)
		{
			if(!(*(psz+1))) break;

			if(!_ismbcspace(*((WORD*)psz))) break;
			psz+=2;
		}
		else
		{
			if(!isspace(*psz)) break;
			psz++;
		}
	}
	return (*psz == 0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param psz : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void str_trim_right(char* psz)
{

	char* p = psz;
	char* p1 = NULL;
	while(*p)
	{
		if((*p) & 0x80)
		{
			if(!(*(p+1))) break;
			if(_ismbcspace(*((WORD*)p)))
			{
				if(p1 == NULL)
					p1 = p;
			}
			else
			{
				if(p1)p1 = NULL;
			}
			p+=2;
		}
		else
		{
			if(isspace(*p))
			{
				if(p1 == NULL)
					p1 = p;
			}
			else
			{
				if(p1)p1 = NULL;
			}
			p++;
		}
	}
	if(p1) *p1 = 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param psz : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void str_trim_left(char* psz)
{
	//	char* pszNew = new char[strlen(psz) + 1];
	char* p = psz;
	while(*p)
	{
		if((*p) & 0x80)
		{
			if(!(*(p+1))) break;

			if(_ismbcspace(*((WORD*)p)))
			{
			}
			else
			{
				break;
			}
			p+=2;
		}
		else
		{
			if(isspace(*p))
			{
			}
			else
			{
				break;
			}
			p++;
		}
	}
	strcpy(psz,p);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszSrc : ÃèÊö
 * \param pszDst : ÃèÊö
 * \param nDstBufLen : ÃèÊö
 * \param nLineLen : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void str_split(const char* pszSrc,char* pszDst,size_t nDstBufLen,size_t nLineLen)
{
	size_t i;
	i =0;
	while(*pszSrc)
	{
		if((*pszSrc) & 0x80)
		{
			if(!(*(pszSrc+1))) break;

			if(i+1 >= nLineLen)
			{
				// »»ĞĞ
				if(nDstBufLen < 2) break;
				*pszDst++ = '\n';
				nDstBufLen--;
				i = 0;
			}

			if(nDstBufLen<3) break;
		
			*pszDst++ = *pszSrc++;
			*pszDst++ = *pszSrc++;
			nDstBufLen-=2;
			i+=2;
		}
		else
		{
			if(i >= nLineLen)
			{
				// »»ĞĞ
				if(nDstBufLen < 2) break;
				*pszDst++ = '\n';
				nDstBufLen--;
				i = 0;
			}

			if(nDstBufLen < 2) break;
			*pszDst++ = *pszSrc++;
			nDstBufLen--;
			i++;
		}
	}
	*pszDst = 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszString : ÃèÊö
 * \param endptr : ÃèÊö
 * \param base : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
long str_getnextl(const char* pszString,char** endptr,int base)
{
	char* p;
	long l = strtol(pszString,&p,10);
	char* result = p;
	while( *p )
	{
		++p;
		result = p;
		strtol(result,&p,base);
		if(result != p) break;
	}

	if(endptr)
		*endptr = result;

	return l;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param wcstr : ÃèÊö
 * \param mbstr : ÃèÊö
 * \param count : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
size_t my_mbstowcs(wchar_t *wcstr,
				   const char *mbstr,
				   size_t count )
{
	size_t i = 0;
	size_t ret = 0;
	while(i < count && mbstr[i])
	{
		if(mbstr[i] & 0x80)
		{
			if( i+1 >= count || mbstr[i + 1] == 0) break;
			BYTE* p = (BYTE*)&wcstr[ret++];
			p[0] = mbstr[i+1];
			p[1] = mbstr[i];
			i+=2;
		}
		else
		{
			wcstr[ret++] = (WORD)(BYTE)mbstr[i];
			i++;
		}
	}
	wcstr[ret] = 0;
	return ret;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param mbstr : ÃèÊö
 * \param wcstr : ÃèÊö
 * \param count : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
size_t my_wcstombs(char *mbstr,
				   const wchar_t *wcstr,
				   size_t count )
{
	size_t i = 0;
	size_t ret = 0;
	while( i < count && wcstr[i])
	{
		if(wcstr[i] & 0xff00)
		{
			BYTE* p = (BYTE*)&wcstr[i];
			mbstr[ret++] = p[1];
			mbstr[ret++] = p[0];
			i++;
		}
		else
		{
			mbstr[ret++] = (BYTE)(WORD)wcstr[i];
			i++;
		}
	}
	mbstr[ret] = 0;
	return ret;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param str : ÃèÊö
 * \param pData : ÃèÊö
 * \param count : ÃèÊö
 * \param color : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
#ifndef _NO_GRAPHICS
void InitStringData( const char *str,DWORD* pData,size_t count,DWORD color )
{
	size_t i = 0;
	size_t ix = 0;
	while( i < count && str[i] )
	{
		int ch;
		if(str[i] & 0x80)
		{
			if(str[i+1] == 0) break;
			ch = *(WORD*)&str[i];
			i+=2;
		}
		else
		{
			ch = str[i];
			i++;
		}

		stPicCharInfo* p = GetDevice()->GetPicCharInfo(ch);
		if(p)
		{
			pData[ix++] = xtimeGetTime()-rand()%1024;
		}
		else
		{
			pData[ix++] = color;
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param wcstr : ÃèÊö
 * \param pData : ÃèÊö
 * \param count : ÃèÊö
 * \param color : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void InitStringData( const wchar_t *wcstr,DWORD* pData,size_t count,DWORD color )
{
	size_t i = 0;
	while( i < count && wcstr[i] )
	{
		BYTE mbstr[2];
		if( wcstr[i] & 0xff00 )
		{
			BYTE* p = (BYTE*)&wcstr[i];
			mbstr[0] = p[1];
			mbstr[1] = p[0];
		}
		else
		{
			mbstr[0] = (BYTE)(WORD)wcstr[i];
			mbstr[1] = 0;
		}
		stPicCharInfo* p = GetDevice()->GetPicCharInfo(*(WORD*)mbstr);
		if(p)
		{
			pData[i] = xtimeGetTime()-rand()%1024;
		}
		else
		{
			pData[i] = color;
		}
		++i;
	}
}
#endif

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ch : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
WORD my_mbstowcs(WORD ch)
{
	BYTE wcstr[2];
	if(ch & 0x80)
	{
		
		BYTE* p = (BYTE*)&ch;
		wcstr[0] = p[1];
		wcstr[1] = p[0];
	}
	else
	{
		wcstr[0] = (BYTE)ch;
		wcstr[1] = 0;
	}
	return *(WORD*)wcstr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ch : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
WORD my_wcstombs(WORD ch)
{
	BYTE mbstr[2];
	if( ch & 0xff00 )
	{
		BYTE* p = (BYTE*)&ch;
		mbstr[0] = p[1];
		mbstr[1] = p[0];
	}
	else
	{
		mbstr[0] = (BYTE)(WORD)ch;
		mbstr[1] = 0;
	}
	return *(WORD*)mbstr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param str : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int GetCharacterCount(const char * str)
{
	size_t i = 0;
	size_t ret = 0;
	while(str[i])
	{
		if(str[i] & 0x80)
		{
			if( str[i + 1] == 0) break;
			ret++;
			i+=2;
		}
		else
		{
			ret++;
			i++;
		}
	}
	return ret;
}