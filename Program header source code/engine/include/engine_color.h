#ifndef _ENGINE_COLOR_H
#define _ENGINE_COLOR_H

#pragma warning(disable :4018)

#define	COLOR_A4R4G4B4( a, r, g, b )	( (WORD)( (((a) & 0xf0) << 8) | (((r) & 0xf0) << 4 ) | ((g) & 0xf0)  | (((b) & 0xf0) >> 4) ) )
#define COLOR_R5G6B5( r, g, b )		( (WORD)( ( ((r) & 0xf8) << 8 ) | ( ((g) & 0xfc) << 3 ) | ( ((b) & 0xf8) >> 3 ) ) )
#define COLOR_ARGB( a, r, g, b )	( (DWORD)( ( ((a) & 0xff) << 24 ) | ( ((r) & 0xff) << 16 ) | ( ((g) & 0xff)<< 8  ) | ( ((b) & 0xff)  ) ) )
#define COLOR_ACOLOR(a,color) ( (DWORD)( ( ((a) & 0xff) << 24) | (color & 0x00ffffff) ) )
#define A4R4G4B42A8R8G8B8(x) ( (DWORD)(  (((x) & 0xf000) << 16) | (((x) & 0xf00) << 12) | (((x) & 0xf0) << 8) | (((x) & 0xf) << 4) ) )
#define R5G6B52X8R8G8B8(x) ( (DWORD)( 0xff000000 | (((x) & 0xf800) << 8) | (((x) & 0x7e0) << 5) | (((x) & 0x1f) << 3) ))
#define R5G5B52X8R8G8B8(x) ( (DWORD)( 0xff000000 | (((x) & 0x7c00) << 9) | (((x) & 0x3e0) << 6) | (((x) & 0x1f) << 3) ))

#define A8R8G8B82A4R4G4B4(x) ((WORD) ( (((x) & 0xf0000000) >> 16) | (((x) & 0xf00000) >> 12) | (((x) & 0xf000) >> 8) | (((x) & 0xf0) >> 4) ))
#define X8R8G8B82R5G6B5(x) ((WORD) ( (((x) & 0xf80000) >> 16) | (((x) & 0xfc00) >> 13) | (((x) & 0xf8) >> 3) ) )

#define X_COLOR_DIV(color,value) ( ( ((color) + ((value)>>1)) + ((color) + ((value)>>1)) >> ((value)+1) ) >> ((value) + 1) )

#define COLOR_GET_ALPHA( x ) ( ( ((x) & 0xff000000)>>24))
#define COLOR_GET_R( x ) ( ( ((x) & 0xff0000)>>16))
#define COLOR_GET_G( x ) ( ( ((x) & 0xff00)>>8))
#define COLOR_GET_B( x ) ( ( ((x) & 0xff)))

#define COLOR565_GET_R( x ) ( ((x) & 0xf800) >> 8)
#define COLOR565_GET_G( x ) ( ((x) & 0x07e0) >> 3)
#define COLOR565_GET_B( x ) ( ((x) & 0x001f) << 3)

#define COLOR555_GET_R( x ) ( ((x) & 0x7c00) >> 7)
#define COLOR555_GET_G( x ) ( ((x) & 0x03e0) >> 2)
#define COLOR555_GET_B( x ) ( ((x) & 0x001f) << 3)


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszString : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline DWORD StringToColor(const char* pszString)
{
	DWORD color;
	if(strchr(pszString,','))
	{
		int a,r,g,b;
		char* p;
		a = str_getnextl(pszString,&p,10);
		r = str_getnextl(p,&p,10);
		g = str_getnextl(p,&p,10);
		b = str_getnextl(p,&p,10);
		color = COLOR_ARGB(a,r,g,b);
	}
	else
	{
		color = strtoul(pszString,NULL,16);
	}
	return color;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param color : ÃèÊö
 * \param pszBuffer : ÃèÊö
 * \param size : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void ColorToString(DWORD color,char* pszBuffer,size_t size)
{
	_snprintf(pszBuffer,size,"%u,%u,%u,%u",COLOR_GET_ALPHA(color),COLOR_GET_R(color),COLOR_GET_G(color),COLOR_GET_B(color));
	if(size) pszBuffer[size-1]=0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param color1 : ÃèÊö
 * \param color2 : ÃèÊö
 * \param s : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
extern DWORD _ColorLerp(DWORD color1,DWORD color2,DWORD color3);
inline DWORD ColorLerp(DWORD color1,DWORD color2,DWORD s)
{
	s =  (s << 24) | (s << 16) | ( s << 8) | s;
	return _ColorLerp(color1,color2,s);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param color1 : ÃèÊö
 * \param color2 : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline DWORD ColorBlend(DWORD color1,DWORD color2)
{
	extern DWORD _ColorBlend(DWORD color1,DWORD color2);
	if(color1 == 0xffffffff) return color2;
	if(color1 == 0) return 0;
	if(color2 == 0xffffffff) return color1;
	if(color2 == 0) return 0;
	return _ColorBlend(color1,color2);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline DWORD GetRandColor32()
{
	int a,r,g,b;
	a = rand()%256;
	r = rand()%256;
	g = rand()%256;
	b = rand()%256;
	return COLOR_ARGB(a,r,g,b);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline DWORD GetRandColor24()
{
	int a,r,g,b;
	a = 255;
	r = rand()%256;
	g = rand()%256;
	b = rand()%256;
	return COLOR_ARGB(a,r,g,b);
}

#endif