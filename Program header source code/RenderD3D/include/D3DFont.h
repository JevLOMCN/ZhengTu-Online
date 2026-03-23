/**
 * \file      D3DFont.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2005-07-05 12:29:19
 * \brief     D3D设备字体
 * 
 *	      D3D设备字体
 */

#pragma once

#include <hash_map>

class CTextureCell;
class CFontTextureCell;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CFontTextureGroup;

#pragma pack(1)
struct stCharInfo{
	WORD ch;
	/*BYTE offsetX;
	BYTE offsetY;
	BYTE width;
	BYTE height;*/
	DWORD dataOffset;
};
#pragma pack()

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CD3DFont
{
protected:

	int CH_MAX_CHAR2;
	int CH_MIN_CHAR2;

	int CH_MAX_CHAR1;
	int CH_MIN_CHAR1;

	int COL_COUNT;
	int ROW_COUNT;

	std::vector<BYTE> m_pBmpData;

	//typedef std::vector<stCharInfo> tCharInfos;
	//tCharInfos m_charInfos;

	CD3DFont* m_pDuplicate;
	int m_width;
	int m_widthWchar;
	int m_height;

	// 字符之间的间距
	int m_iHSpace;
	int m_iHSpaceWchar;
	int m_iVSpace;

	size_t m_pitch;
	int m_bmpWidth;
	int m_bmpHeight;
	CFontTextureGroup * m_TextureGroup;
	typedef std::map<DWORD,CTextureCell*> tMapChar;
	typedef tMapChar::iterator tMapCharIt;
	tMapChar m_mapChar;
	//CTextureCell* m_mapChar[0xffff];
	int m_iFontIndex;
	int m_bits;

	BYTE* GetBmpData() { return (m_pDuplicate ? m_pDuplicate->GetBmpData() : &m_pBmpData[0]);}
	POINT WcharToRowCol(int c1 /*高位*/, int c2/*低位*/);
	POINT WcharToRowCol(WORD wdWChar);
	POINT CharToRowCol(char chChar);
	WORD ComposeWchar(int c1,int c2);
	void UncomposeWchar(WORD ch,int & c1,int & c2);
public:
	bool m_bRenderBorder;
	CD3DFont(int iFontIndex,int bits,bool bRenderBorder,bool bLageLib);
	~CD3DFont(void);

	int GetHSpace() { return m_iHSpace; }
	int GetHSpaceWchar() { return m_iHSpaceWchar; }
	int GetVSpace() { return m_iVSpace;}

	D3DFORMAT GetFontFormat();
	CFontTextureGroup* GetTextureGroup() { return m_TextureGroup; }

	CTextureCell* GetCharTex(WORD c);

	void DeleteCharTex(WORD c)
	{
		//*
		if(m_mapChar.empty()) return ;
		tMapCharIt it = m_mapChar.find(c);
		if(it != m_mapChar.end())
		{
			m_mapChar.erase(it);
		}
		else
		{
			Assert(0);
		}
		//*/
	}

	bool LoadBmpFont(const char* szFileName);

	bool Update1Bit(size_t x,size_t y,size_t width,size_t height,D3DLOCKED_RECT* lock_rect,D3DFORMAT format,bool setZero = true);
	bool Update(size_t x,size_t y,size_t width,size_t height,D3DLOCKED_RECT* lock_rect,D3DFORMAT format,bool setZero = true);

	bool UpdateWchar(unsigned short s, D3DLOCKED_RECT* rect,D3DFORMAT format);
	bool UpdateWchar(unsigned short c1,unsigned short c2, D3DLOCKED_RECT* rect,D3DFORMAT format);
	bool UpdateChar(char c, D3DLOCKED_RECT* rect,D3DFORMAT format);

	int GetWidth(){ return m_width; }
	int GetHeight(){ return m_height; }
	int GetWcharWidth(){ return m_widthWchar;}
	int GetWidth(int ch){ return ((ch & 0x80) ? GetWcharWidth() : GetWidth());}
	
	bool Render(WORD c,float x,float y,DWORD color,const RECT* rClip=NULL,const stPointF* scale = NULL);

	bool CreateFont(const char* name,const char* nameFT,const char* nameE,const char* saveFileName,int width,int height,int weight,bool bBorder);
	void insertBitmap(int i,BYTE* src,size_t stride,int width,int height,int xOrigin, int yOrigin,bool border);
	void insertBitmapWchar(int ch1,int ch2,BYTE* src,size_t stride,int width,int height,int xOrigin, int yOrigin,bool border);
	int GetBmpPixel(int x,int y);
	void SetBmpPixel(int x,int y,int value);
	void SaveFontTga(const char* fileName);
	void DuplicateFont(CD3DFont* font);
};
