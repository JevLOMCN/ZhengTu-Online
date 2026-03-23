#include "public.h"
#include "../../RenderD3D/include/d3dfont.h"


/*
表1 汉字编码范围

名称     |       第一字节             |           第二字节
--------|-------------------------|------------------------
GB2312  |   0xB0-0xF7(176-247)    |    0xA0-0xFE（160-254）
--------|-------------------------|-------------------------
GBK0    |  x81-0xFE（129-254）     |   0x40-0xFE（64-254）
--------|-------------------------|-------------------------
Big5    |   0x81-0xFE（129-255）   |   0x40-0x7E（64-126）
|                         |    0xA1－0xFE（161-254）
--------|-------------------------|------------------------
//*/

const UINT GBK_MIN_CHAR1 = 0x81;
const UINT GBK_MAX_CHAR1 = 0xfe;       // 决定列  第一字节

const UINT GBK_MIN_CHAR2 = 0x40;
const UINT GBK_MAX_CHAR2 = 0xfe;       // 决定行  第二字节

const UINT GB2312_MIN_CHAR1 = 0xa1;
const UINT GB2312_MAX_CHAR1 = 0xf7;       // 决定列  第一字节

const UINT GB2312_MIN_CHAR2 = 0xa0;
const UINT GB2312_MAX_CHAR2 = 0xfe;       // 决定行  第二字节

const UINT EN_MIN_CHAR = 33;
const UINT EN_MAX_CHAR = 0x7f;

inline bool isGB2312Code( WORD ch )
{
	int c1 = (ch & 0xff);
	int c2 = ((ch & 0xff00) >> 8);
	return GB2312_MIN_CHAR1 <= c1 && c1 <= GB2312_MAX_CHAR1 &&
		GB2312_MIN_CHAR2 <= c2 && c2 <= GB2312_MAX_CHAR2;
}

inline WORD ComposeWchar(int c1,int c2)
{
	return (c2 << 8) | c1;
}

inline void UncomposeWchar(WORD ch,int & c1,int & c2)
{
	c1 = (ch & 0xff);
	c2 = ((ch & 0xff00) >> 8);
}

inline bool isSamePoint(stPointI pt1, stPointI pt2, int rate)
{
	if ( (abs(pt1.x-pt2.x) < rate) && (abs(pt1.y-pt2.y) < rate) )
	{
		return true;
	}

	return false;
}


//Get font point list form a char
bool GetFontPointList(const WORD& cName, stPointI* pPointList, int& numPoint)
{
	char name[32] = "新宋体";
	char nameE[32] = "Arial";
	char nameFT[32] = "Arial";
	int width = 10;
	int height = 20;
	LOGFONT lf;
	memset(&lf,0,sizeof(LOGFONT));

	numPoint = 0;

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
	
	//Get font
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

	//TEXTMETRIC textMetric;
	COLORREF backgroundColorRef = RGB(  0,   0,   0);
	COLORREF foregroundColorRef = RGB(255, 255, 255);

	//Get font DC
	HDC fontDC;
	HBITMAP fontBMP;
	HGDIOBJ oldBmp, oldFont;
    	
	fontDC = CreateCompatibleDC(NULL);
	fontBMP = CreateCompatibleBitmap(fontDC,256,256);
	
	oldBmp = SelectObject(fontDC, fontBMP);
	if(hFontE)
		oldFont = SelectObject(fontDC, hFontE);
	else
		oldFont = SelectObject(fontDC, hFont);

	//
	SetBkColor(fontDC, backgroundColorRef);
	SetTextColor(fontDC, foregroundColorRef);

	//Get english font
	if ( (cName>=EN_MIN_CHAR) && (cName<=EN_MAX_CHAR) )
	{
		if(hFontE)
		{
			if(cName >= '0' && cName <= '9')
				SelectObject(fontDC, hFont);
			else
				SelectObject(fontDC, hFontE);
		}

		TextOut(fontDC,0,0,(char*)&cName,1);
		numPoint = 0;
		for(size_t i1=0;i1<height;++i1)
		{
			for(size_t j1=0;j1<width;++j1)
			{
				COLORREF color = GetPixel(fontDC,j1,i1);
				if(color & 0xffffff)
				{
					pPointList[numPoint++] = stPointI( j1, i1 );
				}
			}
		}
	}
	else		//Get chinese font
	{
		int c1,c2;
		UncomposeWchar( cName, c1, c2 );
		WORD wName = ComposeWchar( c2, c1 );
		WORD sz[2] = {wName,0};

		if( hFontFT )
		{
			if( isGB2312Code( wName ) )
				SelectObject(fontDC, hFont);
			else
				SelectObject(fontDC, hFontFT);
		}
		else
			SelectObject( fontDC, hFont );

		TextOut(fontDC,0,0,(char*)sz,2);
		if ( 0 )
		{
			float cRate = 0.02f;
			int space = (int)(width * cRate);
			stPointI ptLast(0,0);
			for(size_t i1=0;i1<height;++i1)
			{
				for(size_t j1=0;j1<width*2;++j1)
				{
					COLORREF color = GetPixel(fontDC,j1,i1);
					if(color & 0xffffff)
					{
						if ( !isSamePoint( stPointI(j1,i1), ptLast, space ) )
						{
							pPointList[numPoint++] = stPointI( j1, i1 );
							ptLast = stPointI(j1,i1);
						}					
					}
				}
			}
		}
		else
		{
			for(size_t i1=0;i1<height;++i1)
			{
				for(size_t j1=0;j1<width*2;++j1)
				{
					COLORREF color = GetPixel(fontDC,j1,i1);
					if(color & 0xffffff)
					{
						pPointList[numPoint++] = stPointI( j1, i1 );
					}
				}
			}
		}
	}

	//Make center at point(0,0)
	stRectI rcBoundingBox;
	for( int i=0;i<numPoint;i++ )
	{
		if ( i == 0 )
		{
			rcBoundingBox.left = pPointList[0].x;
			rcBoundingBox.right = pPointList[0].x;
			rcBoundingBox.top = pPointList[0].y;
			rcBoundingBox.bottom = pPointList[0].y;
		}
		else
		{
			if ( pPointList[i].x < rcBoundingBox.left )
				rcBoundingBox.left = pPointList[i].x;
			if ( pPointList[i].x > rcBoundingBox.right )
				rcBoundingBox.right = pPointList[i].x;
			if ( pPointList[i].y < rcBoundingBox.top )
				rcBoundingBox.top = pPointList[i].y;
			if ( pPointList[i].y > rcBoundingBox.bottom )
				rcBoundingBox.bottom = pPointList[i].y;
		}		
	}
	
	int nWidth = ( rcBoundingBox.left + rcBoundingBox.right ) / 2;
	int nHeight = ( rcBoundingBox.top + rcBoundingBox.bottom ) / 2;

	//Reset pos of point list
	for( int i=0;i<numPoint;i++ )
	{
		pPointList[i] -= stPointI(nWidth, nHeight);
	}
	
	
	//Release res
	SelectObject(fontDC, oldBmp);
	SelectObject(fontDC, oldFont);
	DeleteObject(hFont);
	DeleteObject(hFontFT);
	DeleteObject(hFontE);
	DeleteObject(fontBMP);
	DeleteDC(fontDC);
	return numPoint > 0;
}