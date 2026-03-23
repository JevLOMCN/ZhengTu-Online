#pragma once
#pragma pack(1)
typedef struct _TAGFILEHEADER{
	BYTE IDLength;
    BYTE ColormapType;
    BYTE ImageType;
	WORD ColormapOffset;		//颜色表首址 
	WORD ColormapLength;		//颜色表的长度
    BYTE ColormapBitDepth;		//颜色表表项的位（bit）数
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

class CGraphicsFileData
{
private:
public:
	TGAFILEHEADER m_FileHeader;
	unsigned short GetWidth(){return m_FileHeader.Width;}
	unsigned short GetHeight(){return m_FileHeader.Height;}

	typedef struct _stFileData{
		BYTE B;
		BYTE G;
		BYTE R;
		BYTE A;
	}stFileData;
 
	union {
		stFileData* m_pstFileData;
		unsigned char *m_pFileData;
	};
    //int hxw;
    //int size_in_bytes;
    //int bits_per_pixel;
    int m_BytesPerPixel;
    //unsigned char *endptr;

	void LoadTGAFile(const char* );
	void LoadTGAFile(CArchive& ar);

	CGraphicsFileData(void);
	~CGraphicsFileData(void);

	void Draw(HDC hDC, RECT * pRect, COLORREF ct);
	HBITMAP GetBitmap(COLORREF ct);
#if 0
	void SaveAsTex(LPCTSTR lpszFileName, int type);
	void SaveAsTex(CArchive& ar, int type);
#endif
	void LoadTGAFile(BYTE* pBuf);
	POINT GetOffset(void);
	BOOL m_bAlpha;
	void SetColorKey(DWORD dwColor);
	void Filter(DWORD dwColor);
};
