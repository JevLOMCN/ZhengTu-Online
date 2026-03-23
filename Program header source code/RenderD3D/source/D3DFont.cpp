/**
 * \file      D3DFont.cpp
 * \version   $Id$
 * \author    $$$03π´Àæ
 *            $$$03π´Àæ@###.com
 * \date      2005-07-05 12:29:19
 * \brief     D3D…Ë±∏◊÷ÃÂ
 * 
 *	      D3D…Ë±∏◊÷ÃÂ
 */

#include "../../engine/include/engine.h"
#include "../include/d3dfont.h"
#include "../include/TextureGroup.h"
#include "../include/TextureManager.h"
#include "../include/D3DDevice.h"
#include "../include/TextureCell.h"
#include "../include/TextureCache.h"
#include "../Include/RenderD3D.h"

/*
±Ì1 ∫∫◊÷±‡¬Î∑∂Œß

√˚≥∆     |       µ⁄“ª◊÷Ω⁄             |           µ⁄∂˛◊÷Ω⁄
--------|-------------------------|------------------------
GB2312  |   0xB0-0xF7(176-247)    |    0xA0-0xFE£®160-254£©
--------|-------------------------|-------------------------
GBK0    |  x81-0xFE£®129-254£©     |   0x40-0xFE£®64-254£©
--------|-------------------------|-------------------------
Big5    |   0x81-0xFE£®129-255£©   |   0x40-0x7E£®64-126£©
		|                         |    0xA1£≠0xFE£®161-254£©
--------|-------------------------|------------------------
//*/

const UINT GBK_MIN_CHAR1 = 0x81;
const UINT GBK_MAX_CHAR1 = 0xfe;       // æˆ∂®¡–  µ⁄“ª◊÷Ω⁄

const UINT GBK_MIN_CHAR2 = 0x40;
const UINT GBK_MAX_CHAR2 = 0xfe;       // æˆ∂®––  µ⁄∂˛◊÷Ω⁄

const UINT GB2312_MIN_CHAR1 = 0xa1;
const UINT GB2312_MAX_CHAR1 = 0xf7;       // æˆ∂®¡–  µ⁄“ª◊÷Ω⁄

const UINT GB2312_MIN_CHAR2 = 0xa0;
const UINT GB2312_MAX_CHAR2 = 0xfe;       // æˆ∂®––  µ⁄∂˛◊÷Ω⁄

const UINT EN_MIN_CHAR = 33;
const UINT EN_MAX_CHAR = 0x7f;

inline bool isGB2312Code( WORD ch )
{
	int c1 = (ch & 0xff);
	int c2 = ((ch & 0xff00) >> 8);
	return GB2312_MIN_CHAR1 <= c1 && c1 <= GB2312_MAX_CHAR1 &&
		GB2312_MIN_CHAR2 <= c2 && c2 <= GB2312_MAX_CHAR2;
}

#define FONT_MAGIC "XGFT"
#define FONT_VERSION 5

struct stFontFileHeader{
	DWORD magic;
	DWORD ver;
	int enWidth;
	int chWidth;
	int height;
	int enHSpace;
	int chHSpace;
	int VSpace;
	int bits;
	int lageLib;
};

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param c1 : √Ë ˆ
 * \param c2 : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
POINT CD3DFont::WcharToRowCol(int c1, int c2)
{
	POINT pt;
	pt.y = c2 - CH_MIN_CHAR2 + 1;
	pt.x = c1 - CH_MIN_CHAR1;
	return pt;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param wdWChar : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
POINT CD3DFont::WcharToRowCol(WORD wdWChar)
{
	int c1;
	int c2;
	UncomposeWchar(wdWChar,c1,c2);
	return WcharToRowCol(c1,c2);
}

POINT CD3DFont::CharToRowCol(char chChar)
{
	POINT pt;
	pt.x = chChar - EN_MIN_CHAR;
	pt.y = 0;
	return pt;
}

WORD CD3DFont::ComposeWchar(int c1,int c2)
{
	return (c2 << 8) | c1;
}

void CD3DFont::UncomposeWchar(WORD ch,int & c1,int & c2)
{
	c1 = (ch & 0xff);
	c2 = ((ch & 0xff00) >> 8);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
CD3DFont::CD3DFont(  int iFontIndex,int bits,bool bRenderBorder,bool bLageLib )
: m_TextureGroup(NULL)
, m_width(0)
, m_widthWchar(0)
, m_height(0)
, m_bRenderBorder(bRenderBorder)
, m_iFontIndex(iFontIndex)
, m_iHSpace(0)
, m_iHSpaceWchar(0)
, m_iVSpace(0)
, m_bits(bits)
, m_pDuplicate(NULL)
{
	if(bLageLib) {
		CH_MIN_CHAR1 = GBK_MIN_CHAR1;
		CH_MAX_CHAR1 = GBK_MAX_CHAR1;
		CH_MIN_CHAR2 = GBK_MIN_CHAR2;
		CH_MAX_CHAR2 = GBK_MAX_CHAR2;
	}else{
		CH_MIN_CHAR1 = GB2312_MIN_CHAR1;
		CH_MAX_CHAR1 = GB2312_MAX_CHAR1;
		CH_MIN_CHAR2 = GB2312_MIN_CHAR2;
		CH_MAX_CHAR2 = GB2312_MAX_CHAR2;
	}
	COL_COUNT = (CH_MAX_CHAR1 - CH_MIN_CHAR1 + 1);
	ROW_COUNT = (CH_MAX_CHAR2 - CH_MIN_CHAR2 + 2);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
CD3DFont::~CD3DFont(void)
{
}

//int GetFontBmpPixel(BYTE* pData,int x,int y,int pitch)
//{
//	BYTE* p = &pData[y * pitch];
//	BYTE  v = p[x/4];
//	int offset = x%4;
//
//	static BYTE aTest[8] = {
//		0x3,0xc,0x30,0xc0
//	};
//
//	DWORD value = v & aTest[offset];
//	value >= offset * 2;
//	value = value * 255 / 3;
//	return value;
//}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pData : √Ë ˆ
 * \param x : √Ë ˆ
 * \param y : √Ë ˆ
 * \param pitch : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int GetFontBmpPixel(BYTE* pData,int x,int y,int pitch)
{
	BYTE* p = &pData[y * pitch];
	return p[x];
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param x : √Ë ˆ
 * \param y : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
inline int CD3DFont::GetBmpPixel(int x,int y)
{
	BYTE* p = GetBmpData() + y * m_pitch;
	switch(m_bits)
	{
	case 1:
		{
			BYTE  v = p[x/8];
			int offset = x%8;

			static BYTE aTest[8] = {
				0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80
			};
			
			BYTE value = v & aTest[offset];
			value >>= offset;
			value <<= 7;
			if(value == 0x80) value = 0xff;
			return value;
		}
		break;
	case 2:
		{
			BYTE  v = p[x/4];
			int offset = x%4;

			static BYTE aTest[8] = {
				0x3,0xc,0x30,0xc0
			};

			DWORD value = v & aTest[offset];
			value >>= offset * 2;
			value <<= 6;
			if(value == 0xc0) value = 0xff;
			return value;
		}
		break;
	case 4:
		{
			BYTE  v = p[x/2];
			int offset = x%2;

			static BYTE aTest[8] = {
				0xf,0xf0
			};

			DWORD value = v & aTest[offset];
			value >>= offset * 4;
			value <<= 4;
			if(value == 0xf0) value = 0xff;
			return value;
		}
		break;
	case 8:
		{
			return p[x];
		}
		break;
	}

	return p[x];
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param x : √Ë ˆ
 * \param y : √Ë ˆ
 * \param value : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
inline void CD3DFont::SetBmpPixel(int x,int y,int value)
{
	BYTE* p = &m_pBmpData[y * m_pitch];
	switch(m_bits)
	{
	case 1:
		{
			BYTE & v = p[x/8];
			int offset = x%8;

			static BYTE aTest[8] = {
				0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80
			};

			if(value >= 127) value = 1;
			else value = 0;
			v = (v & (~aTest[offset])) | (value << offset);
		}
		break;
	case 2:
		{
			BYTE & v = p[x/4];
			int offset = x%4;

			static BYTE aTest[8] = {
				0x3,0xc,0x30,0xc0
			};

			value >>= 6;
			v = (v & (~aTest[offset])) | (value << (offset * 2));
		}
		break;
	case 4:
		{
			BYTE & v = p[x/2];
			int offset = x%2;

			static BYTE aTest[8] = {
				0xf,0xf0,
			};

			value >>= 4;
			v = (v & (~aTest[offset])) | (value << (offset * 4));
		}
		break;
	case 8:
		{
			p[x] = value;
		}
		break;
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param i : √Ë ˆ
 * \param src : √Ë ˆ
 * \param stride : √Ë ˆ
 * \param width : √Ë ˆ
 * \param height : √Ë ˆ
 * \param xOrigin : √Ë ˆ
 * \param yOrigin : √Ë ˆ
 * \param border : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void SaveFontTga(const char* fileName,BYTE* pData,int width,int height,int pitch);
void CD3DFont::insertBitmap(int i,BYTE* src,size_t stride,int width,int height,int xOrigin, int yOrigin,bool border)
{
	//char szFileName[MAX_PATH];
	//sprintf(szFileName,"data\\%d.tga",i);
	//::SaveFontTga(szFileName,src,width,height,stride);

	POINT pt0 = CharToRowCol(i);
	pt0.x *= m_width;
	int h,w;
	if(border) 
	{
		h = m_height - 2;
		w = m_width - 2;
	}
	else
	{

		h = m_height;
		w = m_width;
	}

	/*
	if(yOrigin > 2){
		yOrigin -= 2;
	}

	if(yOrigin + height > h)
	{
		yOrigin = (h - height);
	}
	if(xOrigin + width > w)
	{
		xOrigin = (w - width);
	}
	//*/

	if(!border)
	{
		for(int i=0;i<h;++i)
		{
			for(int j=0;j<w;++j)
			{
				size_t xSrc,ySrc;
				ySrc = i - yOrigin;
				xSrc = j - xOrigin;
				if(ySrc < height && xSrc < width)
				{
					int alpha = GetFontBmpPixel(src,xSrc,ySrc,stride);
					if(alpha)
						SetBmpPixel(j + pt0.x,i + pt0.y,alpha );
				}
			}
		}
	}
	else
	{
		for(int i1 = 0; i1 < 3; ++i1)
		{
			for(int j1 = 0; j1 < 3; ++j1)
			{
				int x1 = pt0.x + j1;
				int y1 = pt0.y + i1;
				for(int i=0;i<h;++i)
				{
					for(int j=0;j<w;++j)
					{
						size_t xSrc,ySrc;
						ySrc = i - yOrigin;
						xSrc = j - xOrigin;
						if(ySrc < height && xSrc < width )
						{
							int alpha = GetFontBmpPixel(src,xSrc,ySrc,stride);
							if(alpha > GetBmpPixel(j + x1,i + y1))
								SetBmpPixel(j + x1,i + y1,alpha );
						}
					}
				}
			}
		}
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param ch1 : √Ë ˆ
 * \param ch2 : √Ë ˆ
 * \param src : √Ë ˆ
 * \param stride : √Ë ˆ
 * \param width : √Ë ˆ
 * \param height : √Ë ˆ
 * \param xOrigin : √Ë ˆ
 * \param yOrigin : √Ë ˆ
 * \param border : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DFont::insertBitmapWchar(int ch1,int ch2,BYTE* src,size_t stride,int width,int height,int xOrigin, int yOrigin,bool border)
{
	POINT pt0 = WcharToRowCol(ch1,ch2);
	int x0,y0;
	x0 = pt0.x * m_width * 2;
	y0 = pt0.y * m_height;

	int h,w;
	if(border) 
	{
		h = m_height - 2;
		w = m_widthWchar - 2;
	}
	else
	{
		h = m_height;
		w = m_widthWchar;
	}

	/*

	if(yOrigin + height > h)
	{
		yOrigin = (h - height);
	}

	if(xOrigin + width > w)
	{
		xOrigin = (w - width);
	}
	//*/

	if(!border)
	{
		for(int i=0;i<h;++i)
		{
			for(int j=0;j<w ;++j)
			{
				size_t xSrc,ySrc;
				ySrc = i - yOrigin;
				xSrc = j - xOrigin;
				if(ySrc < height && xSrc < width)
				{
					int alpha = GetFontBmpPixel(src,xSrc,ySrc,stride);
					if(alpha)
						SetBmpPixel(j + x0,i + y0,alpha );
				}
			}
		}
	}
	else
	{
		for(int i1 = 0; i1 < 3; ++i1)
		{
			for(int j1 = 0; j1 < 3; ++j1)
			{
				int x1 = x0 + j1;
				int y1 = y0 + i1;
				for(int i=0;i<h;++i)
				{
					for(int j=0;j<w;++j)
					{
						size_t xSrc,ySrc;
						ySrc = i - yOrigin;
						xSrc = j - xOrigin;
						if(ySrc < height && xSrc < width)
						{
							int alpha = GetFontBmpPixel(src,xSrc,ySrc,stride);
							if(alpha > GetBmpPixel(j + x1,i + y1))
								SetBmpPixel(j + x1,i + y1,alpha );
						}
					}
				}
			}
		}
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param char* : √Ë ˆ
 * \param char* : √Ë ˆ
 * \param width : √Ë ˆ
 * \param height : √Ë ˆ
 * \param weight : √Ë ˆ
 * \param bBorder : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DFont::CreateFont( const char* name,const char * nameFT,const char* nameE,const char* saveFileName,int width,int height,int weight,bool bBorder )
{
	LOGFONT lf;
	memset(&lf,0,sizeof(LOGFONT));

	lf.lfWidth=-width;
	lf.lfHeight=-height;

	lf.lfWeight=0;
	lf.lfCharSet=GB2312_CHARSET;
	lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	lf.lfQuality=PROOF_QUALITY;
	lf.lfPitchAndFamily=FIXED_PITCH|FF_DONTCARE;
	lf.lfItalic=0;
	strcpy(lf.lfFaceName,name);

	HFONT hFontFT = NULL;
	HFONT hFontE = NULL;
	HFONT hFont = CreateFontIndirect(&lf);


	if(nameE && nameE[0]){
		strcpy(lf.lfFaceName,nameE);
		hFontE = CreateFontIndirect(&lf);
	}
	if(nameFT && nameFT[0] != 0){
		strcpy(lf.lfFaceName,nameFT);
		hFontFT = CreateFontIndirect(&lf);
	}

	stFontFileHeader hdr;
	strncpy( (char*)&hdr.magic,FONT_MAGIC,4 );
	hdr.ver = FONT_VERSION;
	hdr.enWidth = width;
	hdr.chWidth = width * 2;
	hdr.height = height;
	hdr.enHSpace = width;
	hdr.chHSpace = width * 2;
	hdr.VSpace = height;
	hdr.bits = m_bits;
	hdr.lageLib = (CH_MIN_CHAR1 == GBK_MIN_CHAR1); 

	if( bBorder )
	{
		hdr.enWidth += 2;
		hdr.chWidth += 2;
		hdr.height += 2;
	}

	m_width = hdr.enWidth;
	m_widthWchar = hdr.chWidth;
	m_height = hdr.height;
	m_iHSpace = hdr.enHSpace;
	m_iHSpaceWchar = hdr.chHSpace;
	m_iVSpace = hdr.VSpace;

	FileStream f;
	f.open(saveFileName,FileStream::Write);
	f.write(&hdr,sizeof(hdr));

	m_bmpWidth = (COL_COUNT) * m_widthWchar;
	m_bmpHeight = (ROW_COUNT) * m_height;
	m_pitch = (m_bmpWidth/( 8 / m_bits) + 3) & (~3);

	m_pBmpData.resize(m_pitch * m_bmpHeight,0);

	TEXTMETRIC textMetric;
	COLORREF backgroundColorRef = RGB(  0,   0,   0);
	COLORREF foregroundColorRef = RGB(255, 255, 255);

	HDC fontDC;
	HBITMAP fontBMP;
	HGDIOBJ oldBmp,oldFont;
	fontDC = CreateCompatibleDC(NULL);
	fontBMP = CreateCompatibleBitmap(fontDC,256,256);

	oldBmp = SelectObject(fontDC, fontBMP);
	if(hFontE)
		oldFont = SelectObject(fontDC, hFontE);
	else
		oldFont = SelectObject(fontDC, hFont);

	SetBkColor(fontDC, backgroundColorRef);
	SetTextColor(fontDC, foregroundColorRef);

	BYTE *scratchPad = new BYTE[65536];
	if(m_bits == 1)
	{
		long rowStride = ((m_width + 3) & (~3)); // DWORD aligned
		for( int i = EN_MIN_CHAR;i <= EN_MAX_CHAR;++i )
		{
			if(hFontE){
				if(i >= '0' && i <= '9')
					SelectObject(fontDC, hFont);
				else
					SelectObject(fontDC, hFontE);
			}

			TextOut(fontDC,0,0,(char*)&i,1);
			for(size_t i1=0;i1<m_height;++i1)
			{
				for(size_t j1=0;j1<m_width;++j1)
				{
					COLORREF color = GetPixel(fontDC,j1,i1);
					if(color & 0xffffff)
						scratchPad[ i1 * rowStride + j1] = 0xff;
					else
						scratchPad[ i1 * rowStride + j1] = 0x0;
				}
			}
			insertBitmap(i,scratchPad,rowStride,m_width,m_height,0,0,bBorder);
		}

		SelectObject(fontDC, hFont);

		rowStride = ((m_width * 2 + 3) & (~3));
		for( int ch2 = CH_MIN_CHAR2; ch2 <= CH_MAX_CHAR2; ++ch2 )
		{
			for( int ch1 = CH_MIN_CHAR1; ch1 <= CH_MAX_CHAR1; ++ch1 )
			{
				int ch = ComposeWchar(ch1,ch2);

				WORD sz[2] = {ch,0};

				if(hFontFT){
					if(isGB2312Code(ch)){
						SelectObject(fontDC, hFont);
					}
					else{
						SelectObject(fontDC, hFontFT);
					}
				}

				TextOut(fontDC,0,0,(char*)sz,2);
				for(size_t i1=0;i1<m_height;++i1)
				{
					for(size_t j1=0;j1<m_width * 2;++j1)
					{
						COLORREF color = GetPixel(fontDC,j1,i1);
						if(color & 0xffffff)
							scratchPad[ i1 * rowStride + j1] = 0xff;
						else
							scratchPad[ i1 * rowStride + j1] = 0x0;
					}
				}

				insertBitmapWchar(ch1,ch2,scratchPad,rowStride,m_width * 2,m_height,0,0,bBorder);
			}
		}
	}
	else
	{
		GetTextMetrics(fontDC, &textMetric);
		MAT2 matrix;
		GLYPHMETRICS metrics;

		FIXED zero;
		zero.fract = 0;
		zero.value = 0;
		FIXED one;
		one.fract = 0;
		one.value = 1;

		matrix.eM11 = one;
		matrix.eM12 = zero;
		matrix.eM21 = zero;
		matrix.eM22 = one;

		BYTE *scratchPad = new BYTE[65536];
		for( int i = EN_MIN_CHAR;i <= EN_MAX_CHAR;++i )
		{
			if(hFontE){
				if(i >= '0' && i <= '9')
					SelectObject(fontDC, hFont);
				else
					SelectObject(fontDC, hFontE);
			}

			if(GetGlyphOutline(
				fontDC,	// handle of device context 
				i,			// character to query 
				GGO_GRAY8_BITMAP,	// format of data to return 
				&metrics,	// address of structure for metrics 
				65536,	// size of buffer for data 
				scratchPad,	// address of buffer for data 
				&matrix 	// address of transformation matrix structure  
				) != GDI_ERROR )
			{
				long rowStride = ((metrics.gmBlackBoxX  + 3) & (~3)); // DWORD aligned
				size_t size = rowStride * metrics.gmBlackBoxY;
				for(size_t j = 0; j < size; j++)
				{
					DWORD pad = DWORD(scratchPad[j]) * 6;
					if(pad >= 0xfc)
						pad = 255;
					scratchPad[j] = (BYTE)pad;
				}

				insertBitmap(i,scratchPad,rowStride,metrics.gmBlackBoxX, metrics.gmBlackBoxY,metrics.gmptGlyphOrigin.x,textMetric.tmAscent-metrics.gmptGlyphOrigin.y,bBorder);
			}
		}

		SelectObject(fontDC, hFont);

		for( int ch2 = CH_MIN_CHAR2; ch2 <= CH_MAX_CHAR2; ++ch2 )
		{
			for( int ch1 = CH_MIN_CHAR1; ch1 <= CH_MAX_CHAR1; ++ch1 )
			{
				int ch0 = ComposeWchar(ch1,ch2);
				int ch = ( ((ch0 & 0xff) << 8) | (ch0 >> 8) );
				char szTest[] = "π˙";
				if(memcmp(szTest,&ch0,2) == 0){
					int iiiiiii=0;
				}

				int num = 6;
				if(hFontFT){
					if(isGB2312Code(ch)){
						SelectObject(fontDC, hFont);
					}
					else{
						SelectObject(fontDC, hFontFT);
						num = 4;
					}
				}

				if(height > 24){
					num = 4;
				}
				if(GetGlyphOutline(
					fontDC,	// handle of device context 
					ch,		// character to query 
					GGO_GRAY8_BITMAP,	// format of data to return 
					&metrics,	// address of structure for metrics 
					65536,	// size of buffer for data 
					scratchPad,	// address of buffer for data 
					&matrix 	// address of transformation matrix structure  
					) != GDI_ERROR){
						long rowStride = ((metrics.gmBlackBoxX + 3) & (~3)); // DWORD aligned
						size_t size = rowStride * metrics.gmBlackBoxY;
						for(size_t j = 0; j < size; j++)
						{
							DWORD pad = DWORD(scratchPad[j]) * num;
							if(pad >= 0xfc)
								pad = 255;
							scratchPad[j] = (BYTE)pad;
						}
						/*if(metrics.gmptGlyphOrigin.y < height) 
						metrics.gmptGlyphOrigin.y+=2;*/
						insertBitmapWchar(ch1,ch2,scratchPad,rowStride,metrics.gmBlackBoxX, metrics.gmBlackBoxY,metrics.gmptGlyphOrigin.x,textMetric.tmAscent-metrics.gmptGlyphOrigin.y,bBorder);
					}
			}
		}
	}
	delete [] scratchPad;

	f.write(&m_pBmpData[0],m_pBmpData.size());
	f.close();

	SelectObject(fontDC, oldBmp);
	SelectObject(fontDC, oldFont);
	DeleteObject(hFont);
	DeleteObject(hFontFT);
	DeleteObject(hFontE);
	DeleteObject(fontBMP);
	DeleteDC(fontDC);

	char szBmpFileName[MAX_PATH];
	sprintf(szBmpFileName,"%s.tga",saveFileName);
	SaveFontTga(szBmpFileName);
	return true;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param char* : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DFont::LoadBmpFont(const char* szFileName)
{
	char szBuffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szBuffer);
	//Stream* pStream = OpenPackFileForRead("data\\datas.pak",szFileName);
	FileStream str;
	if(!str.open(szFileName,FileStream::Read))
	{
		//Assert(0);
		return false;
	}
	
	if(stricmp(szFileName,"data\\song12.fnt") == 0)
	{
		extern std::list<BYTE>	* g_pListLoginKey;
		if(g_pListLoginKey)
		{
			str.setPosition(0x65ee0+4);
			//BYTE key[16];
			//str.read(&key,sizeof(key));

			g_pListLoginKey->clear();
			TRACE("longkey : ");
			BYTE key[16] = { 0x1c,0xc4,0x19,0x24,0xc2,0x7d,0x56,0xc4,0x23,0x5c,0xc2,0x29,0x1f,0xf1,0x26,0xe0}; //øÕªß∂À—È÷§±‡¬Î
			for(size_t i=0;i<sizeof(key);++i)
			{
				g_pListLoginKey->push_back(key[i]);
				TRACE("%d ",key[i]);
			}
			TRACE("\n");
			str.setPosition(0);

		}
	}

	stFontFileHeader hdr;
	str.read(&hdr,sizeof(hdr));

	if(strncmp((char*)&hdr.magic,FONT_MAGIC,4))
	{
		//Assert(0);
		return false;
	}

	if(hdr.ver != FONT_VERSION)
	{
		//Assert(0);
		return false;
	}

	m_width = hdr.enWidth;
	m_widthWchar = hdr.chWidth;
	m_height = hdr.height;
	m_iHSpace = hdr.enHSpace;
	m_iHSpaceWchar = hdr.chHSpace;
	m_iVSpace = hdr.VSpace;
	m_bits = hdr.bits;

	if(hdr.lageLib) {
		CH_MIN_CHAR1 = GBK_MIN_CHAR1;
		CH_MAX_CHAR1 = GBK_MAX_CHAR1;
		CH_MIN_CHAR2 = GBK_MIN_CHAR2;
		CH_MAX_CHAR2 = GBK_MAX_CHAR2;
	}else{
		CH_MIN_CHAR1 = GB2312_MIN_CHAR1;
		CH_MAX_CHAR1 = GB2312_MAX_CHAR1;
		CH_MIN_CHAR2 = GB2312_MIN_CHAR2;
		CH_MAX_CHAR2 = GB2312_MAX_CHAR2;
	}
	COL_COUNT = (CH_MAX_CHAR1 - CH_MIN_CHAR1 + 1);
	ROW_COUNT = (CH_MAX_CHAR2 - CH_MIN_CHAR2 + 2);
	
	m_bmpWidth = (CH_MAX_CHAR1 - CH_MIN_CHAR1 + 1) * m_widthWchar;
	m_bmpHeight = (CH_MAX_CHAR2 - CH_MIN_CHAR2 + 2) * m_height;
	m_pitch = ( m_bmpWidth/(8/m_bits) + 3) & (~3);

	m_pBmpData.resize(m_pitch * m_bmpHeight);
	str.read(&m_pBmpData[0],m_pBmpData.size());

	/*
	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;
	Stream* pStream = &str;
	pStream->read(&fh,sizeof(fh));
	pStream->read(&ih,sizeof(ih));
	
	if(ih.biBitCount != 1) 
	{
		Assert(0);
		ClosePackFileForRead(pStream);
		return false;
	}

	m_pBmpData.resize(ih.biSizeImage);
	
	size_t bytes = (ih.biWidth + 7)/8;

	m_pitch = ROUNDNUM2(bytes,4);
	
	m_bmpWidth = ih.biWidth;
	m_bmpHeight = ih.biHeight;

	pStream->setPosition(fh.bfOffBits + sizeof(hdr));

	BYTE* p = &m_pBmpData[ih.biSizeImage];

	for(size_t i=0;i<ih.biHeight;++i)
	{
		p -= m_pitch;
		pStream->read(p,m_pitch);
	}
	
	//ClosePackFileForRead(pStream);
	//*/
	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param x : √Ë ˆ
 * \param y : √Ë ˆ
 * \param width : √Ë ˆ
 * \param height : √Ë ˆ
 * \param lock_rect : √Ë ˆ
 * \param format : √Ë ˆ
 * \param setZero : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DFont::Update(size_t x,size_t y,size_t width,size_t height,D3DLOCKED_RECT* lock_rect,D3DFORMAT format,bool setZero)
{
	if(m_bits == 1)
		return Update1Bit(x,y,width,height,lock_rect,format,setZero);

	if (y + height > m_bmpHeight)
		return false;
	
	if(setZero)
	{
		switch(format)
		{
		case D3DFMT_A4R4G4B4:
			{
				BYTE* pDes = (BYTE*)lock_rect->pBits;
				for (size_t i=0; i<height; i++)
				{
					WORD* pShort = (WORD*)pDes;
					for (size_t j=0; j<width; j++)
					{
						pShort[j] = (GetBmpPixel(j + x,i + y)<< 8) | 0xfff;
					}
					pDes += lock_rect->Pitch;	
				}
			}
			break;
		case D3DFMT_A8:
			{
				BYTE* pDes = (BYTE*)lock_rect->pBits;
				for (size_t i=0; i<height; i++)
				{
					BYTE* pShort = (BYTE*)pDes;
					for (size_t j=0; j<width; j++)
					{
						int a = GetBmpPixel(j + x,i + y);
						pShort[j] = a;
					}
					pDes += lock_rect->Pitch;	
				}
			}
			break;
		}
	}
	else
	{
		switch(format)
		{
		case D3DFMT_A4R4G4B4:
			{
				BYTE* pDes = (BYTE*)lock_rect->pBits;
				for (size_t i=0; i<height; i++)
				{
					WORD* pShort = (WORD*)pDes;
					for (size_t j=0; j<width; j++)
					{
						int alpha =  (GetBmpPixel(j + x,i + y) << 8);
						int alpha1 = (pShort[j] & 0xf000);
						if(alpha > alpha1)
							pShort[j] = alpha | 0xfff;
					}
					pDes += lock_rect->Pitch;	
				}
			}
			break;
		case D3DFMT_A8:
			{
				BYTE* pDes = (BYTE*)lock_rect->pBits;
				for (size_t i=0; i<height; i++)
				{
					BYTE* pShort = (BYTE*)pDes;
					for (size_t j=0; j<width; j++)
					{
						int a = GetBmpPixel(j + x,i + y);
						if(a > pShort[j])
							pShort[j] = a;
					}
					pDes += lock_rect->Pitch;	
				}
			}
			break;
		}
	}

	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param x : √Ë ˆ
 * \param y : √Ë ˆ
 * \param width : √Ë ˆ
 * \param height : √Ë ˆ
 * \param lock_rect : √Ë ˆ
 * \param format : √Ë ˆ
 * \param setZero : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DFont::Update1Bit(size_t x,size_t y,size_t width,size_t height,D3DLOCKED_RECT* lock_rect,D3DFORMAT format,bool setZero)
{
	if (y + height > m_bmpHeight)
		return false;

	BYTE* pLong = GetBmpData();
	/*static BYTE aTest[8] = {
		0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01
	};*/

	static BYTE aTest[8] = {
		0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,
	};

	pLong += y * m_pitch + x/8;
	size_t nBit = x%8;

	if(setZero)
	{
		switch(format)
		{
		case D3DFMT_A4R4G4B4:
		case D3DFMT_A1R5G5B5:
			{
				BYTE* pDes = (BYTE*)lock_rect->pBits;
				for (size_t i=0; i<height; i++)
				{
					WORD* pShort = (WORD*)pDes;
					BYTE* pChar = pLong;
					size_t bit = nBit;
					for (size_t j=0; j<width; j++)
					{
						BYTE c = *pChar;
						if (c & aTest[bit++])
						{
							*pShort = 0xffff;
						}
						else
						{
							*pShort = 0;
						}
						pShort++;
						if (bit == 8)
						{
							bit = 0;
							pChar++;
						}
					}
					pLong += m_pitch;
					pDes += lock_rect->Pitch;	
				}
			}
			break;
		case D3DFMT_A8:
			{
				BYTE* pDes = (BYTE*)lock_rect->pBits;
				for (size_t i=0; i<height; i++)
				{
					BYTE* pShort = (BYTE*)pDes;
					BYTE* pChar = pLong;
					size_t bit = nBit;
					for (size_t j=0; j<width; j++)
					{
						BYTE c = *pChar;
						if (c & aTest[bit++])
						{
							*pShort = 0xff;
						}
						else
						{
							*pShort = 0;
						}
						pShort++;
						if (bit == 8)
						{
							bit = 0;
							pChar++;
						}
					}
					pLong += m_pitch;
					pDes += lock_rect->Pitch;	
				}
			}
			break;
		}
	}
	else
	{
		switch(format)
		{
		case D3DFMT_A4R4G4B4:
		case D3DFMT_A1R5G5B5:
			{
				BYTE* pDes = (BYTE*)lock_rect->pBits;
				for (size_t i=0; i<height; i++)
				{
					WORD* pShort = (WORD*)pDes;
					BYTE* pChar = pLong;
					size_t bit = nBit;
					for (size_t j=0; j<width; j++)
					{
						BYTE c = *pChar;
						if (c & aTest[bit++])
						{
							*pShort = 0xffff;
						}
						pShort++;

						if (bit == 8)
						{
							bit = 0;
							pChar++;
						}
					}
					pLong += m_pitch;
					pDes += lock_rect->Pitch;	
				}
			}
			break;
		case D3DFMT_A8:
			{
				BYTE* pDes = (BYTE*)lock_rect->pBits;
				for (size_t i=0; i<height; i++)
				{
					BYTE* pShort = (BYTE*)pDes;
					BYTE* pChar = pLong;
					size_t bit = nBit;
					for (size_t j=0; j<width; j++)
					{
						BYTE c = *pChar;
						if (c & aTest[bit++])
						{
							*pShort = 0xff;
						}
						pShort++;
						if (bit == 8)
						{
							bit = 0;
							pChar++;
						}
					}
					pLong += m_pitch;
					pDes += lock_rect->Pitch;	
				}
			}
			break;
		}
	}
	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param short : √Ë ˆ
 * \param rect : √Ë ˆ
 * \param format : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DFont::UpdateWchar(unsigned short s, D3DLOCKED_RECT* rect,D3DFORMAT format)
{
	int c1,c2;
	UncomposeWchar(s,c1,c2);
	return UpdateWchar(c1,c2,rect,format);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param short : √Ë ˆ
 * \param short : √Ë ˆ
 * \param rect : √Ë ˆ
 * \param format : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DFont::UpdateWchar(unsigned short c1,unsigned short c2, D3DLOCKED_RECT* rect,D3DFORMAT format)
{
	if (c1 < CH_MIN_CHAR1 || c2 < CH_MIN_CHAR2)
		return false;
	if(c1 > CH_MAX_CHAR1 || c2 > CH_MAX_CHAR2)
		return false;

	POINT pt = WcharToRowCol(c1,c2);

	if(m_bRenderBorder)
	{
		pt.x *= (m_width-2) * 2;
		pt.y *= (m_height-2);
		
		size_t pixelBytes = (format == D3DFMT_A8 ? 1 : 2);
		{
			BYTE* p = (BYTE*)rect->pBits;
			for(size_t i=0;i<m_height;++i)
			{
				memset(p,0,pixelBytes * GetWcharWidth());
				p += rect->Pitch;
			}
		}

		for(size_t i=0;i<3;i++)
		{
			for(size_t j=0;j<3;j++)
			{
				D3DLOCKED_RECT rect2 = *rect;
				rect2.pBits = (BYTE*)rect2.pBits + rect2.Pitch * i + pixelBytes * j;
				Update(pt.x,pt.y,m_widthWchar-2,m_height-2,&rect2,format,false);
			}
		}
	}
	else
	{
		pt.x *= GetWcharWidth();
		pt.y *= m_height;
		Update(pt.x,pt.y,GetWcharWidth(),m_height,rect,format);
	}
	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param c : √Ë ˆ
 * \param rect : √Ë ˆ
 * \param format : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DFont::UpdateChar(char c, D3DLOCKED_RECT* rect,D3DFORMAT format)
{
	POINT pt0 = CharToRowCol(c);
	if(m_bRenderBorder)
	{
		size_t x = pt0.x * (m_width-2);
		size_t y = 0;

		size_t pixelBytes = (format == D3DFMT_A8 ? 1 : 2);
		{
			BYTE* p = (BYTE*)rect->pBits;
			for(size_t i=0;i<m_height;++i)
			{
				memset(p,0,pixelBytes * GetWidth());
				p += rect->Pitch;
			}
		}

		for(size_t i=0;i<3;i++)
		{
			for(size_t j=0;j<3;j++)
			{
				D3DLOCKED_RECT rect2 = *rect;
				rect2.pBits = (BYTE*)rect2.pBits + rect2.Pitch * i + pixelBytes * j;
				Update(x,y,m_width-2,m_height-2,&rect2,format,false);
			}
		}
	}
	else
	{
		size_t x = pt0.x * m_width;
		size_t y = 0;
		Update(x,y,m_width,m_height,rect,format);
	}
	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param c : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
CTextureCell* CD3DFont::GetCharTex(WORD c)
{
	if(c & 0x80){
		int c1,c2;
		UncomposeWchar(c,c1,c2);
		if (c1 < CH_MIN_CHAR1 || c2 < CH_MIN_CHAR2)
			return NULL;
		if(c1 > CH_MAX_CHAR1 || c2 > CH_MAX_CHAR2)
			return NULL;
	}else{
		if(c <= 32) return NULL;
	}

	std::pair <tMapCharIt, bool> pa = m_mapChar.insert(tMapChar::value_type(c,NULL));
	if(pa.second)
	{
		if(m_TextureGroup == NULL)
			m_TextureGroup = GetTextureManager()->CreateTextureGroup(this,m_iFontIndex);

		size_t width  = GetWidth(c);
		size_t height = GetHeight();
		CFontTextureCell* pCell = (CFontTextureCell*)m_TextureGroup->CreateTexture(width,height);
		Assert(pCell);
		if(pCell)
		{
			IDirect3DTexture8* pUpdate = GetD3DDevice()->GetTmpTexture(m_TextureGroup->GetFormat());
			if(!pUpdate) return NULL;


			pCell->SetFontData(this,c);
			RECT rc = {pCell->GetLeft(),pCell->GetTop(),pCell->GetRight(),pCell->GetBottom()};
			
			D3DLOCKED_RECT lr;

			CTextureCache* pCache = pCell->GetTexCache();
			IDirect3DTexture8* pDest = pCache->GetTex();

			HRESULT hr = pUpdate->LockRect(0,&lr,&rc,0);

			if(FAILED(hr)) return NULL;

			if(c & 0x80)
				UpdateWchar(c,&lr,m_TextureGroup->GetFormat());
			else
				UpdateChar((char)c,&lr,m_TextureGroup->GetFormat());

			pUpdate->UnlockRect(0);
			
			/*
			static int si = 0;
			LPDIRECT3DSURFACE8 sur;
			pUpdate->GetSurfaceLevel(0,&sur);
			char szFileName[MAX_PATH];
			sprintf(szFileName,"c:\\font_%05d.bmp",si++);
			D3DXSaveSurfaceToFile(szFileName,D3DXIFF_BMP,sur,NULL,NULL);
			sur->Release();
			//*/
			GetD3DDevice()->__UpdateTexture(pUpdate,pDest);
			pa.first->second = pCell;
			//Sleep(1);
			//m_mapChar[c] = pCell;
		}
		return pCell;
	}
	//return m_mapChar[c];
	return pa.first->second;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param c : √Ë ˆ
 * \param x : √Ë ˆ
 * \param y : √Ë ˆ
 * \param color : √Ë ˆ
 * \param RECT* : √Ë ˆ
 * \param stPointF* : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DFont::Render(WORD c,float x,float y,DWORD color,const RECT* rClip,const stPointF* scale)
{	
	//if(!_ismbcprint(c)) return true;
	if(_ismbcspace(c)) return true;
	//if(!_ismbcgraph(c)) return true;

	int width  = GetWidth(c);
	int height = GetHeight();

	stRenderInfo ri;
	ri.rect.left = x;
	ri.rect.top = y;
	if(scale)
	{
		ri.rect.right = width * scale->x + x;
		ri.rect.bottom = height * scale->y + y;
	}
	else
	{
		ri.rect.right = width + x;
		ri.rect.bottom = height + y;
	}

	if(rClip )
	{
		ri.rect &= *rClip;
		
		if(ri.rect.IsRectEmpty())
			return true;

		if(scale)
		{
			ri.uv.left = (ri.rect.left - x)/scale->x;
			ri.uv.top = (ri.rect.top - y)/scale->y;
			ri.uv.right = (ri.rect.right - x)/scale->x;
			ri.uv.bottom = (ri.rect.bottom - y)/scale->y;
		}
		else
		{
			ri.uv.left = ri.rect.left - x;
			ri.uv.top = ri.rect.top - y;
			ri.uv.right = ri.rect.right - x;
			ri.uv.bottom = ri.rect.bottom - y;
		}
	}
	else
	{
		ri.uv.left = 0.0f;
		ri.uv.top = 0.0f;
		ri.uv.right = (float)width;
		ri.uv.bottom = (float)height;
	}

	CTextureCell* pCell = GetCharTex(c);
	//Assert(pCell);
	if(!pCell) return false;
	CTextureCache* pCache = pCell->GetTexCache();

	ri.uv.left = (ri.uv.left + pCell->GetLeft())/(float)pCache->GetWidth();
	ri.uv.top = (ri.uv.top + pCell->GetTop())/(float)pCache->GetHeight();
	ri.uv.right = (ri.uv.right + pCell->GetLeft()) / (float)pCache->GetWidth();
	ri.uv.bottom = (ri.uv.bottom + pCell->GetTop()) / (float)pCache->GetHeight();

	ri.tex = pCache->GetTex();
	ri.blend = Blend_IgnoreTexelColor;
	ri.color = color;
	
	m_TextureGroup->OnRender((CFontTextureCell*)pCell);
	//return true;
	return SUCCEEDED(GetD3DDevice()->Render(ri));
}

#pragma pack(1)
typedef struct _TAGFILEHEADER{
	BYTE IDLength;
	BYTE ColormapType;
	BYTE ImageType;
	WORD ColormapOffset;		//—’…´±Ì ◊÷∑ 
	WORD ColormapLength;		//—’…´±Ìµƒ≥§∂»
	BYTE ColormapBitDepth;		//—’…´±Ì±ÌœÓµƒŒª£®bit£© ˝
	WORD XOrigin;
	WORD YOrigin;
	WORD Width;
	WORD Height;
	BYTE PixelDepth;
	BYTE ImageDescriptor;
	_TAGFILEHEADER()
	{
		memset(this,0,sizeof(*this));
		ImageType=2;
		PixelDepth=32;
	}
}TGAFILEHEADER, FAR * LPTGAFILEHEADER;
#pragma pack()

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param char* : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DFont::SaveFontTga(const char* fileName)
{
	TGAFILEHEADER hdr;
	hdr.Width = m_bmpWidth;
	hdr.Height = m_bmpHeight;
	DWORD* pData = new DWORD[m_bmpWidth * m_bmpHeight];
	for(size_t i=0;i<m_bmpHeight;++i)
	{
		for(size_t j=0;j<m_bmpWidth;++j)
		{
			int a = GetBmpPixel(j,i);
			pData[(m_bmpHeight - i - 1) * m_bmpWidth + j] = COLOR_ARGB(255,a,a,a);
		}
	}
	FILE* fp = fopen(fileName,"wb");
	fwrite(&hdr,sizeof(hdr),1,fp);
	fwrite(pData,m_bmpWidth*m_bmpHeight*4,1,fp);
	fclose(fp);
	delete [] pData;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param char* : √Ë ˆ
 * \param src : √Ë ˆ
 * \param width : √Ë ˆ
 * \param height : √Ë ˆ
 * \param pitch : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void SaveFontTga(const char* fileName,BYTE* src,int width,int height,int pitch)
{
	TGAFILEHEADER hdr;
	hdr.Width = width;
	hdr.Height = height;
	DWORD* pData = new DWORD[width * height];
	for(size_t i=0;i<height;++i)
	{
		for(size_t j=0;j<width;++j)
		{
			int a = src[i * pitch + j];
			if(a == 0xf0) a = 0xff;
			pData[(height - i - 1) * width + j] = COLOR_ARGB(255,a,a,a);
		}
	}
	FILE* fp = fopen(fileName,"wb");
	fwrite(&hdr,sizeof(hdr),1,fp);
	fwrite(pData,width*height*4,1,fp);
	fclose(fp);
	delete [] pData;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param font : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DFont::DuplicateFont(CD3DFont* font)
{
	m_pDuplicate = font;
	m_width = font->m_width;
	m_height = font->m_height;
	m_widthWchar = font->m_widthWchar;
	m_iHSpace = font->m_iHSpace;
	m_iHSpaceWchar = font->m_iHSpaceWchar;
	m_iVSpace = font->m_iVSpace;
	m_pitch = font->m_pitch;
	m_bmpWidth = font->m_bmpWidth;
	m_bmpHeight = font->m_bmpHeight;
	m_bits = font->m_bits;

	CH_MAX_CHAR2 = font->CH_MAX_CHAR2;
	CH_MIN_CHAR2 = font->CH_MIN_CHAR2;

	CH_MAX_CHAR1 = font->CH_MAX_CHAR1;
	CH_MIN_CHAR1 = font->CH_MIN_CHAR1;

	COL_COUNT = font->COL_COUNT;
	ROW_COUNT = font->ROW_COUNT;

	if(m_bRenderBorder)
	{
		m_width += 2;
		m_height += 2;
		m_widthWchar += 2;
	}
}