

#include "../include/engine.h"
#include "../include/spell.h"


#define MAKE_UPPER(c)		(c >= 'a' && c <= 'z')?(c-32):c;
#define MAKE_WORD(w,b1,b2)	{BYTE* p = (BYTE*)&w;p[0]=b2;p[1]=b1;}

CSpell::CSpell(void)
{
}

CSpell::~CSpell(void)
{
	UnLoadSpellMap();
}

void CSpell::LoadSpellMap()
{
	Stream* fin = OpenPackFileForRead("data\\datas.pak","datas\\spell.hzx");
	if ( fin )
	{
		int i = 0;
		while( i++ < 480 )
		{
			tSpellItem t;
			if( fin->read(t.pszSpell,tSpellItem::SPELL_LENGTH) )
			{
				size_t iLen = 0;
				if( fin->read( &iLen,sizeof(iLen) ) )
				{
					if( iLen != 0 && iLen < 1024 )
					{
						t.iSize = iLen;
						t.pszHz = new char[iLen];						
						if ( fin->read(t.pszHz,iLen) )
						{
							m_List.push_back(t);
						}
						else
						{
							delete t.pszHz;
							break; 
						}
					}
				}else break; // end if( iLen != 0 && iLen < 512 )
			}else break; // end if ( 8 == fread(t.pszSpell,1,8,fin) )
		}
		ClosePackFileForRead(fin);
	}
}

void CSpell::UnLoadSpellMap()
{
	for( tListSpellIt it = m_List.begin(); it != m_List.end(); ++it )
	{
		tSpellItem& t = (*it);
		delete t.pszHz;
	}
	m_List.clear();
}

const char* CSpell::GetAllSpell(WORD wChar)
{
	if( m_List.empty() )
	{
		LoadSpellMap();
	}
	for( tListSpellIt it = m_List.begin(); it != m_List.end(); ++it )
	{
		tSpellItem& t = (*it);
		for( int i = 0; i < t.iSize; i+=2 )
		{
			WORD c = 0;
			MAKE_WORD(c,t.pszHz[i],t.pszHz[i+1]);
			if( c == wChar )
			{
				return t.pszSpell;
			}
		}
	}
	return "";
}

//一级汉字或特殊符号
char CSpell::GetSpellI(WORD wChar)
{
	if ( wChar >= 'a' && wChar <= 'z' ) return (char)(wChar - 32);
	if ( wChar >= 'A' && wChar <= 'Z' ) return (char)(wChar);
	if ( wChar >= '0' && wChar <= '9' ) return (char)(wChar);

	if(wChar >= 'Ａ' && wChar <= 'Ｚ') 
		return (char)(wChar - 'Ａ' + 'A'); 

	if(wChar >= 'ａ' && wChar <= 'ｚ') 
		return (char)(wChar - 'ａ' + 'A'); 

	if(wChar >= '０' && wChar <= '９') 
		return (char)(wChar - '０' + '0');

	int aCode[] = {45217,45253,45761,46318,46826,47010,47297,47614,48119,48119,49062,49324,49896,50371,50614,50622,50906,51387,51446,52218,52698,52698,52698,52980,53689,54481};

	for( int i = 0; i < 26; i ++ )
	{
		int max = 55290;
		if(i != 25) max = aCode[i+1];
		if( aCode[i] <= wChar && wChar < max )
		{
			return (char)(65+i);
		}
	}
	return '*';
}

//二级汉字
char CSpell::GetSpellII(WORD wChar)
{
	const char* pszSpell = GetAllSpell(wChar);
	if( pszSpell && strlen(pszSpell) > 0 )
	{
		return pszSpell[0];
	}
	return '*';
}

char CSpell::GetSpell(WORD wChar)
{
	if( wChar < 55290 )	//01-09区为特殊符号。 //16-55区为一级汉字，按拼音排序。 
	{
		return GetSpellI(wChar);
	}
	else				//56-87区为二级汉字，按部首/笔画排序。
	{
		return GetSpellII(wChar);
	}
}

char CSpell::GetFirstSpell(const char* psz)
{
	if( psz && strlen(psz) > 0 )
	{
		wchar_t ret;
		if( (psz[0] & 0x80) && strlen(psz) > 1 )
		{
			MAKE_WORD(ret,psz[0],psz[1]);
		}
		else 
		{
			ret = psz[0];
		}	
		return GetSpell(ret);
	}
	return '*';
}

bool Compare2StringBySpell(const char* psz1,const char* psz2)
{
	size_t i = 0, j = 0;
	while( psz1[i] && psz2[j] )
	{	
		char c1 = psz1[i];
		char c2 = psz2[j];

		if ( c1 & 0x80 )
		{
			if ( psz1[i+1] == 0 ) return false;
			wchar_t ret;
			MAKE_WORD(ret,c1,psz1[i+1]);
			c1 = GetSpellManager().GetSpell(ret);
			i += 2;
		}
		else i ++;

		if ( c2 & 0x80 )
		{
			if ( psz2[j+1] == 0 ) return true;

			wchar_t ret;
			MAKE_WORD(ret,c2,psz2[j+1]);
			c2 = GetSpellManager().GetSpell(ret);
			j += 2;
		}
		else j ++;

		char upper1 = MAKE_UPPER(c1);
		char upper2 = MAKE_UPPER(c2);

		if ( upper1 != upper2 )	return upper1 > upper2;
	}
	return psz1[i] != 0;
}

