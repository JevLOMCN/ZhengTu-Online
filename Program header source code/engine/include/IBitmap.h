/**
 * \file      IBitmap.h
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     Í¼Æ¬²Ù×÷½Ó¿Ú
 * 
 *	      Í¼Æ¬²Ù×÷½Ó¿Ú
 */

#pragma once

enum BlendType{
	Blend_Null,			   //src:srcAlpha               dest:invSrcAlpha
	Blend_Add,			   //src:one                    dest:invSrcColor
	Blend_Add2,			   //src:one                    dest:one
	Blend_AlphaAdd,		   //src:srcAlpha               dest:one
	Blend_AlphaAdd2,	   //src:one                    dest:invSrcAlpha
	Blend_Bright,		   //src:srcAlpha               dest:invSrcAlpha       srcColor:tex*diffuse*2
	Blend_IgnoreTexelColor,//src:srcAlpha               dest:invSrcAlpha       srcColor:diffuse
	Blend_ColorAdd,		   //src:srcAlpha               dest:invSrcAlpha       srcColor:tex+diffuse
	Blend_AlphaAdd3,	   //src:srcColor	            dest:invSrcColor
	Blend_AlphaSub,        //src:srcAlpha               dest:invSrcAlpha       srcAlpha:tex-diffuse
	Blend_Monochrome,      //src:
	Blend_AlphaAdd4,       //src:srcAlpha               dest:invSrcAlpha       srcAlpha:tex+diffuse
	Blend_MulAlpha,        //src:zone                   dest:srcAlpha
	Blend_MulInvAlpha,     //src:zone                   dest:invAlpha
	Blend_Add3,			   //src:one                    dest:srcColor
	Blend_MulInvColor,	   //src:zero                   dest:invsrccolor
	Blend_AlphaAdd5,       //src:srcAlpha               dest:one               srcColor:diffuse
	Blend_MulColor,		   //src:zero                   dest:srcColor
};	

enum enumGrpType
{
	eGrpTypeRle = 0,
	eGrpTypeTex = 1,
	eGrpTypeJpeg= 2,
	eGrpTypeTexA4 = 3,
	eGrpTypeTexRle = 4,
	eGrpTypeTexA4Rle = 5,
	eGrpTypeTexColorRle = 6,
};

enum enumPixelFormat
{
	ePixUnknown			= 0,
	ePixPal8			,
	ePixPal8NotAlpha	,
	ePixJpegRGB			,
	ePixJpegARGB		,
	ePix4444			,
	ePix565				,
	ePixDXT1_5551		,
	ePixDXT1			,
	ePixDXT2			,
	ePixDXT3			,
	ePixDXT4			,
	ePixDXT5			,
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param format : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline size_t GetTexBlkSize(D3DFORMAT format)
{
	switch(format)
	{
	case D3DFMT_DXT1:
		return 8;
	case D3DFMT_DXT3:
		return 16;
	}
	return 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param e : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline D3DFORMAT GrpGetD3DFormat(enumPixelFormat e)
{
	switch(e)
	{
	case ePixDXT1:
	case ePixDXT1_5551:
		return D3DFMT_DXT1;
	case ePixDXT2:
		return D3DFMT_DXT2;
	case ePixDXT3:
		return D3DFMT_DXT3;
	case ePixDXT4:
		return D3DFMT_DXT4;
	case ePixDXT5:
		return D3DFMT_DXT5;
	case ePixPal8NotAlpha:
	case ePixJpegRGB:
		return D3DFMT_R5G6B5;
	}
	return D3DFMT_A4R4G4B4;
}

struct stGrpImageBlock{
	DWORD offset;
	WORD  left;
	WORD  top;
	WORD  width;
	WORD  height;
};

const DWORD MAX_TEXTUREWIDTH = 256;

//#define BLOCK_FLAGE_NORMAL  0
//#define BLOCK_FLAGE_NULL	(1 << 0)
//#define BLOCK_FLAGE_ZIP		(1 << 1)

enum enumAlphaType{
	AlphaType_None = 0,
	AlphaType_AllBlack,	// È«ºÚ
	AlphaType_AllWhite,	// È«°×
	AlphaType_OneOnly, // Ö»ÓĞÒ»ÖÖÖµ
};

//	Í¼Ïñ¿éĞÅÏ¢
struct stGrpBlock
{
	BYTE	byGrpType;				// Í¼Æ¬Ñ¹ËõÀàĞÍ	

	union{
		BYTE	byPixelFormat;		// ÏñËØ¸ñÊ½
		BYTE	byAlphaType;
	};

	union{
		BYTE	byAlpha;			//
		WORD	reserve;			// ±£Áô×Ö
	};


	WORD	wdWidth;				// Í¼Ïñ¿í¶È
	WORD	wdHeight;				// Í¼Ïñ¸ß¶È

	WORD	wdClipLeft;
	WORD	wdClipTop;

	WORD	wdClipWidth;
	WORD	wdClipHeight;
	DWORD	dwDataOffset;
	/*DWORD	dwCRC;*/
};
#pragma pack(1)
struct stRgb
{
	union{
		struct{
			BYTE b;
			BYTE g;
			BYTE r;
			BYTE a;
		};
		DWORD color;
	};

	bool operator == (const stRgb & v) { return color == v.color;}
	stRgb& operator = (DWORD v) { color = v; return (*this);}

};
#pragma pack()

extern const int c_GrpVersion;
extern const int c_GrpVersionOld;
#define GRP_FILE_MAGIC ("XZGL")

//*
struct stGrpFileHeaderOld
{
	DWORD	dwMagic;	// 'XZGL'
	WORD	wdVersion;	
	WORD	wdDirCount; // Ä¿Â¼Êı
	DWORD	dwHeaderDataOffset;
	DWORD	dwFlags;	//
};
//*/

struct stGLHeader{
	DWORD	dwMagic;		//'XZGL'
	DWORD	dwVersion;		// c_GrpVersion
	DWORD	dwPictureNum;	// Í¼Æ¬Êı
	DWORD	dwHeaderDataOffset;
};

enum PlayType{
	Play_Normal,
	Play_Random,
};

struct stPackAniHeader
{
	WORD version;
	BYTE blend;
	BYTE playType;
	WORD wdBitmapCount;
	WORD wdFrameCount;
	DWORD wdAniSpeed;
	DWORD intervalTime;
	DWORD dwColor;
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param info : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void InitPackAniInfo(stPackAniHeader* info)
{
	memset(info,0,sizeof(stPackAniHeader));
	info->wdBitmapCount = 1;
	info->wdFrameCount = 1;
	info->dwColor = 0xffffffff;
}

struct stGrpAniLocation{
	DWORD	dwFileOffset;			// ¶¯»­ÎÄ¼şÎ»ÖÃ£¬Ïà¶ÔÊı¾İÇøµÄÎ»ÖÃ
	DWORD	dwFileSize;				// ¶¯»­ÎÄ¼ş´óĞ¡
};

//struct stPackAniHeaderEx : public stPackAniHeader
//{
//	DWORD	dwFileOffset;			// ¶¯»­ÎÄ¼şÎ»ÖÃ£¬Ïà¶ÔÊı¾İÇøµÄÎ»ÖÃ
//	DWORD	dwFileSize;				// ¶¯»­ÎÄ¼ş´óĞ¡
//};

struct stGrpAniInfo{
	stGrpAniLocation location;
	//DWORD	interspaceSize;			// ËùÓĞ¿Õ¼ä´óĞ¡£¬°üÀ¨Î´ÓÃµÄ¿Õ¼ä
	//DWORD	dwCRC;
};

struct stGrpAniInfoOld{
	stGrpAniLocation location;
	DWORD	interspaceSize;			// ËùÓĞ¿Õ¼ä´óĞ¡£¬°üÀ¨Î´ÓÃµÄ¿Õ¼ä
	DWORD	dwCRC;
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dwWidth : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline DWORD GetTailWidth(DWORD dwWidth)
{
	dwWidth &= (MAX_TEXTUREWIDTH-1);
	if(dwWidth) return dwWidth;
	return MAX_TEXTUREWIDTH;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dwWidth : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline DWORD GetTailPow2(DWORD dwWidth)
{
	return GetPow2(GetTailWidth(dwWidth));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dwWidth : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline DWORD GetWidth4(DWORD dwWidth)
{
	return ROUNDNUM2(dwWidth,4);
}

enum LoadEnum
{
	Load_NotLoad,
	Load_Loaded,
	Load_PreLoad,
	Load_PreLoad0,	//
	Load_PreLoad1,	//in the queue
	Load_MapView,
	Load_LoadError,	//×°ÔØ³ö´í
};

struct stRleDecodeByteToByte{
};

struct stRleDecodeWordToWord{
};

struct stRleDecode4444To8888{
};

struct stRleDecode565To888{
};

struct stRleDecodeIndexTo8888{
};

struct stRleDecodeAlphaTo8888{
};

struct stRleDecodeAlphaTo4444{
};

struct stRleDecodeIndexTo4444{
};

struct stRleDecodeIndexTo565{
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param value : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeSet(stRleDecodeIndexTo4444, WORD * pDst,BYTE value,size_t count,void* param)
{
	stRgb* pal = (stRgb*)param;
	for(size_t i=0;i<count;++i)
	{
		pDst[i] = COLOR_A4R4G4B4(255,pal[value].r,pal[value].g,pal[value].b);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param value : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeSet(stRleDecodeIndexTo565, WORD * pDst,BYTE value,size_t count,void* param)
{
	stRgb* pal = (stRgb*)param;
	for(size_t i=0;i<count;++i)
	{
		pDst[i] = COLOR_R5G6B5(pal[value].r,pal[value].g,pal[value].b);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pData : ÃèÊö
 * \param value : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeSet(stRleDecodeByteToByte, BYTE* pData,BYTE value,size_t count,void* param)
{
	memset(pData,value,count);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pData : ÃèÊö
 * \param value : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeSet(stRleDecodeWordToWord, WORD* pData,WORD value,size_t count,void* param)
{
	wmemset((wchar_t*)pData,(wchar_t)value,count);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param value : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeSet(stRleDecodeIndexTo8888, DWORD * pDst,BYTE value,size_t count,void* param)
{
	for(size_t i=0;i<count;++i)
	{
		pDst[i] = ((DWORD*)param)[value];
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param value : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeSet(stRleDecode565To888, DWORD* pDst,WORD value,size_t count,void* param)
{
	for(size_t i=0;i<count;++i)
	{
		pDst[i] = R5G6B52X8R8G8B8(value);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param value : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeSet(stRleDecode4444To8888, DWORD* pDst,WORD value,size_t count,void* param)
{
	for(size_t i=0;i<count;++i)
	{
		pDst[i] = A4R4G4B42A8R8G8B8(value);
	}
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param value : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeSet(stRleDecodeAlphaTo8888,DWORD * pDst,BYTE value,size_t count,void* param)
{
	for(size_t i=0;i<count;++i)
	{
		pDst[i] &= (0x00ffffff | ((DWORD)value << 24));
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param value : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeSet(stRleDecodeAlphaTo4444,WORD * pDst,BYTE value,size_t count,void* param)
{
	for(size_t i=0;i<count;++i)
	{
		pDst[i] &= (0x0fff | ( (WORD(value) & 0xf0) << 8));
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param pSrc : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeCpy(stRleDecodeByteToByte,BYTE * pDst,BYTE* pSrc,size_t count,void* param)
{
	memcpy(pDst,pSrc,count);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param pSrc : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeCpy(stRleDecodeWordToWord, WORD * pDst,WORD* pSrc,size_t count,void* param)
{
	memcpy(pDst,pSrc,count*2);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param pSrc : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeCpy(stRleDecodeIndexTo8888,DWORD * pDst,BYTE* pSrc,size_t count,void* param)
{
	for(size_t i=0;i<count;++i)
	{
		pDst[i] = ((DWORD*)param)[*pSrc++];
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param pSrc : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeCpy(stRleDecodeAlphaTo8888,DWORD * pDst,BYTE* pSrc,size_t count,void* param)
{
	for(size_t i=0;i<count;++i)
	{
		pDst[i] &= (0x00ffffff | (DWORD(*pSrc++) << 24));
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param pSrc : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeCpy(stRleDecode565To888,DWORD* pDst,WORD* pSrc,size_t count,void* param)
{
	for(size_t i=0;i<count;++i)
	{
		pDst[i] = R5G6B52X8R8G8B8(pSrc[i]);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param pSrc : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeCpy(stRleDecode4444To8888,DWORD* pDst,WORD* pSrc,size_t count,void* param)
{
	for(size_t i=0;i<count;++i)
	{
		pDst[i] = A4R4G4B42A8R8G8B8(pSrc[i]);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param pSrc : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeCpy(stRleDecodeAlphaTo4444,WORD * pDst,BYTE* pSrc,size_t count,void* param)
{
	for(size_t i=0;i<count;++i)
	{
		pDst[i] &= (0x0fff | ( (WORD(*pSrc++) & 0xf0) << 8));
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param pSrc : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeCpy(stRleDecodeIndexTo4444, WORD * pDst,BYTE *pSrc,size_t count,void* param)
{
	stRgb* pal = (stRgb*)param;
	for(size_t i=0;i<count;++i)
	{
		pDst[i] = COLOR_A4R4G4B4(255,pal[pSrc[i]].r,pal[pSrc[i]].g,pal[pSrc[i]].b);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDst : ÃèÊö
 * \param pSrc : ÃèÊö
 * \param count : ÃèÊö
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline void RLE_DecodeCpy(stRleDecodeIndexTo565, WORD * pDst,BYTE *pSrc,size_t count,void* param)
{
	stRgb* pal = (stRgb*)param;
	for(size_t i=0;i<count;++i)
	{
		pDst[i] = COLOR_R5G6B5(pal[pSrc[i]].r,pal[pSrc[i]].g,pal[pSrc[i]].b);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pData : ÃèÊö
 * \param width : ÃèÊö
 * \param pRetData : ÃèÊö
 * \param param : ÃèÊö
 * \param st : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
// RLE ½âÑ¹ËõÒ»ĞĞÏñËØ
template < class TSrc,class TDst,typename DecodeT >
void RLE_DecodeALine(TSrc* & pData,size_t width,TDst* pRetData,void* param ,DecodeT st)
{
	const size_t c1 = sizeof(TSrc)*8 - 1;
	const size_t c2 = sizeof(TSrc)*8;
	const size_t max_line = (1<<c1);
	const size_t mask_rle = (1<<c1);
	for(size_t i=0;i<width;)
	{
		size_t flag= *pData++;
		size_t len = (flag & (~mask_rle))+1;
		if(flag & mask_rle)
		{
			TSrc value = *pData++;
			RLE_DecodeSet(st,pRetData,value,len,param);
			//my_memset(pRetData,value,len);
			pRetData += len;
			i += len;
		}
		else
		{
			RLE_DecodeCpy(st,pRetData,pData,len,param);
			//memcpy(pRetData,pData,len*sizeof(T));
			pRetData += len;
			pData += len;
			i+=len;
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pData : ÃèÊö
 * \param width : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
// RLE ½âÑ¹ËõÒ»ĞĞÏñËØ
template < class TSrc>
void RLE_ForwardALine(TSrc* & pData,size_t width)
{
	const size_t c1 = sizeof(TSrc)*8 - 1;
	const size_t c2 = sizeof(TSrc)*8;
	const size_t max_line = (1<<c1);
	const size_t mask_rle = (1<<c1);
	for(size_t i=0;i<width;)
	{
		size_t flag= *pData++;
		size_t len = (flag & (~mask_rle))+1;
		if(flag & mask_rle)
		{
			pData++;
			i += len;
		}
		else
		{
			pData += len;
			i+=len;
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pData : ÃèÊö
 * \param width : ÃèÊö
 * \param x : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
template < class TSrc>
TSrc RLE_GetValue(TSrc* & pData,size_t width,int x)
{
	const size_t c1 = sizeof(TSrc)*8 - 1;
	const size_t c2 = sizeof(TSrc)*8;
	const size_t max_line = (1<<c1);
	const size_t mask_rle = (1<<c1);
	for(size_t i=0;i<width;)
	{
		size_t flag= *pData++;
		size_t len = (flag & (~mask_rle))+1;
		if(flag & mask_rle)
		{
			if( i + len > x) return *pData;
			pData++;
			i += len;
		}
		else
		{
			if( i + len > x) return pData[x-i];
			pData += len;
			i+=len;
		}
	}
	return 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pData : ÃèÊö
 * \param width : ÃèÊö
 * \param x : ÃèÊö
 * \param y : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
template < class TSrc>
TSrc RLE_GetValue(TSrc* & pData,size_t width,int x,int y)
{
	for(int i=0;i<y;++i)
		RLE_ForwardALine(pData,width);
	return RLE_GetValue(pData,width,x);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param width : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline int GetGrpBlockNum(int width)
{
	return (width + MAX_TEXTUREWIDTH-1)/ MAX_TEXTUREWIDTH;
}
struct stMapFileData;
class IBitmapList;
class IBitmaps;
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class IBitmap
{
	DWORD m_dwLastRenderTime;
protected:

	//int m_iTailWidth;
	//int m_iTailHeight;

	//stRgb*			m_pal;
	BYTE*			m_pDataBase;
	BYTE*			m_pData;
	stMapFileData*  m_pMapFileData;

	bool _MapData();

	//LoadEnum		eLoad;
	WORD			eLoad;
	WORD			m_dwFrameFrame;

	//DWORD			m_dwMarkTime;
	IBitmaps*	  m_pBitmaps;
public:
	bool	IsAlphaAllWhite(){ return m_block.byAlphaType == AlphaType_AllWhite;}
	bool	IsAlphaAllBlack(){ return m_block.byAlphaType == AlphaType_AllBlack;}

	enum {
		errorSize = 0xffffffff,
	};

	stGrpBlock	m_block;

	BYTE*			GetDataBase() { return m_pDataBase; }
	BYTE*			GetData() { return m_pData; }
	//stRgb*			GetPal()  { return m_pal; }
	stGrpBlock*	GetInfo() { return &m_block; }

	int GetTailWidth() { return ::GetTailWidth(m_block.wdClipWidth); }
	int GetTailHeight(){ return ::GetTailWidth(m_block.wdClipHeight);}
	//virtual bool _LoadBitmap(CAtlFile& file) = 0;
	//virtual bool _MapDataLoad(BYTE* pMapData) = 0;
	
	void SetMapFileData(stMapFileData* p);
	stMapFileData* GetMapFileData(){return m_pMapFileData;}

	void SetLoad(LoadEnum e){eLoad = e;}
	bool IsLoad(){return eLoad == Load_Loaded;};			//ÒÑ¾­¶ÁºÃÁË£¿
	bool IsPreload(){return eLoad == Load_PreLoad;};			//·ÅÔÚ¶ÓÁĞÀï¶Á¡£
	bool IsNotLoad(){return eLoad == Load_NotLoad;}
	bool IsLoadError(){return eLoad == Load_LoadError;}

	DWORD GetLastRenderTime(){ return m_dwLastRenderTime;}
	void  SetLastRenderTime(DWORD dwTime);

	virtual size_t ReleaseMemory();

	IBitmap(void);
	~IBitmap(void);

	virtual void Init(IBitmaps* pBitmaps,DWORD wdFrameFrame) = 0;
	virtual bool Render(int x,int y,DWORD color = 0xffffffff,BlendType blend = Blend_Null) = 0;
	virtual bool Render(int x,int y,const RECT* rect,const stPointF * scale,DWORD color = 0xffffffff,BlendType blend = Blend_Null) = 0;

	virtual int GetAlpha(POINT pt) = 0;

	bool IsEmpty(){ return m_block.wdHeight == 0 || m_block.wdWidth == 0;}
	bool IsNull(){ return m_block.wdClipWidth == 0 || m_block.wdClipHeight == 0;}
	bool IsLoadData(){ return m_pData != NULL;}

	int GetWidth(){ return m_block.wdWidth;}
	int GetHeight(){ return m_block.wdHeight;}

	int GetClipWidth(){ return m_block.wdClipWidth;}
	int GetClipHeight(){ return m_block.wdClipHeight;}

	int GetWidthBlockNum(){ return GetGrpBlockNum(m_block.wdClipWidth); }
	int GetHeightBlockNum() { return GetGrpBlockNum(m_block.wdClipHeight); }
	
	SIZE GetClipSize(){ SIZE sz = {m_block.wdClipWidth,m_block.wdClipHeight}; return sz;}
	POINT GetClipOffset(){ POINT pt = { m_block.wdClipLeft,m_block.wdClipTop}; return pt;}
	RECT GetClipRect(){ RECT rc = {m_block.wdClipLeft,m_block.wdClipTop,m_block.wdClipLeft + m_block.wdClipWidth,m_block.wdClipTop + m_block.wdClipHeight}; return rc;}

	BYTE* GetRleBlock(int blockX,int blockY);
	//SIZE GetBlockSize(int blockX,int blockY);
	POINT GetBlockByPos(int x,int y)
	{
		POINT pt;
		x -= (int)m_block.wdClipLeft;
		y -= (int)m_block.wdClipTop;
		if(x < 0 || x >= m_block.wdClipWidth || y < 0 || y >= m_block.wdClipHeight)
		{
			pt.x = -1;
			pt.y = -1;
			return pt;
		}
		pt.x = x / MAX_TEXTUREWIDTH;
		pt.y = y / MAX_TEXTUREWIDTH;
		return pt;
	}

	POINT GetOffsetPos(int x,int y)
	{
		POINT pt = {x - (int)m_block.wdClipLeft,y - (int)m_block.wdClipTop};
		if(pt.x < 0 || pt.x >= m_block.wdClipWidth || pt.y < 0 || pt.y >= m_block.wdClipHeight)
		{
			pt.x = -1;
			pt.y = -1;
		}
		return pt;
	}

	bool IsInClipRect(POINT pt){ return ( (pt.x >= (int)m_block.wdClipLeft)
											&& (pt.x < (int)(m_block.wdClipLeft + m_block.wdClipWidth))
											&& (pt.y >= (int)m_block.wdClipTop)
											&& (pt.y < (int)(m_block.wdClipTop + m_block.wdClipHeight)) );}

	bool IsAt(POINT pt,bool bJustInRect) { if(bJustInRect) return IsInClipRect(pt); return GetAlpha(pt) >= 16; }
	bool IsAt(POINT pt,bool bMirror,bool bJustInRect) { if(bJustInRect) return IsInClipRect(pt); return GetAlpha(pt) > 16; }

	int GetPitch() 
	{ 
		if(m_block.byGrpType == eGrpTypeJpeg) 
			return ROUNDNUM2(m_block.wdClipWidth * 2,4);
		if(m_block.byGrpType == eGrpTypeTex) 
			return GetWidth4(m_block.wdClipWidth) * GetTexBlkSize(GrpGetD3DFormat((enumPixelFormat)m_block.byPixelFormat)) / 4;
		return 0;
	}

	bool LoadBitmap();

	bool IsBreak(IBitmap* p,const POINT &ptOffset,int radius)
	{
		RECT rc1 = GetClipRect();
		RECT rc2 = p->GetClipRect();
		::OffsetRect(&rc2,ptOffset.x,ptOffset.y);
		RECT rc;
		if(!IntersectRect(&rc,&rc1,&rc2)) return false;

		return rc.right-rc.left < radius && rc.bottom-rc.top < radius;

	}
	
	const char* GetPackName();
	int GetFrameFrame(){ return m_dwFrameFrame;}

	IBitmapList* GetBitmapList();
	int GetGroup();
	int GetFrame();
	void CalculateBlockClipRect(int blockX,int blockY,RECT* pRect);
	SIZE CalculateBlockClipSize(int blockX,int blockY);

	DWORD GetDataOffset();
	DWORD GetDataSize();

	virtual bool GetBitmapData(void* pData,int dataPatch) = 0;
};
