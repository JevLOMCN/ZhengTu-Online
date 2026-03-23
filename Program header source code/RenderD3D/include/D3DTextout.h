/**
 * \file      D3DTextout.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2005-07-05 12:29:19
 * \brief     D3D设备文本输出
 * 
 *	      D3D设备文本输出
 */

#pragma once

#include "../../engine/include/frameAllocator.h"
#include "../../Engine/include/stack_alloc.h"
class CD3DFont;

struct stFontInfo{
	char fontName[64];
	char fontNameFT[64];//繁体字体名
	char fontNameE[64];// 英文字体
	char pszFontFile[64];
	char pszBFontFile[64];
	int	width;
	int height;
	int weight;
	int bits;
	bool bLageLib;
	bool bNoBorderFont;
};

extern const stFontInfo c_fontInfo[];
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CD3DTextout
{
public:
	static const int Font_Count;

private:

	struct stRenderFontItem{
		int ichar;
		stPicCharInfo* pPic;
		DWORD time;
		DWORD color;
		stPointF pt;
	};

	typedef std::vector<stRenderFontItem,std::stack_allocator<stRenderFontItem> > t_aRenderFontItem;

	struct stLineInfo{
		stPointF pt1;
		stPointF pt2;
	};

	struct stFont{
		CD3DFont* pNormal;
		CD3DFont* pBorder;
	};

	typedef std::vector<stLineInfo,std::stack_allocator<stLineInfo> > t_aRenderLine;

	std::vector<stFont> m_aFont;

	int		  m_nCurFont;
	int	  m_nHSpace;	// 水平间距
	int	  m_nVSpace;	// 行距
	int	  m_nTabSize;	// tab 字符的宽度
	stPointF m_scale;

	stPicCharInfo* m_pPicCharInfo;
	size_t		   m_nPicCharCount;
public:
	CD3DTextout(void);
	~CD3DTextout(void);

	void GetPicCharInfo(stPicCharInfo** ppPicCharInfo,size_t * count) 
	{ 
		*ppPicCharInfo = m_pPicCharInfo; 
		*count = m_nPicCharCount;
	}

	void SetPicCharInfo(stPicCharInfo* p,size_t nPicCharInfoCount) 
	{ 
		m_pPicCharInfo = p; 
		m_nPicCharCount = nPicCharInfoCount;
	}

	CD3DFont* GetCurFont(){ return m_aFont[m_nCurFont].pNormal;}

	SIZE GetStringSize(const char* lpszString, size_t count,size_t maxWidth,bool bSingleLine = false,enumFontEffect fontEffect = FontEffect_None,t_aRenderFontItem* pRenderList = NULL,t_aRenderLine* pLineList = NULL,DWORD* pData = NULL,DWORD color = -1);

	void GetStringRect(const char* lpszString, size_t count,RECT & layoutRect,DWORD dwTextFormat = DT_TOP | DT_LEFT ,enumFontEffect fontEffect = FontEffect_None,t_aRenderFontItem* pRenderList = NULL,t_aRenderLine* pLineList = NULL,DWORD* pData= NULL,DWORD color = -1);
	void GetStringRect2(const char* lpszString, size_t count,RECT & layoutRect,RECT & totalRect,DWORD dwTextFormat = DT_TOP | DT_LEFT ,enumFontEffect fontEffect = FontEffect_None,t_aRenderFontItem* pRenderList = NULL,t_aRenderLine* pLineList = NULL,DWORD* pData = NULL,DWORD color = -1);

	void DrawString(const char* lpszString , size_t count,const RECT &layoutRect ,DWORD clrText = 0xffffffff,DWORD dwTextFormat = DT_TOP | DT_LEFT,enumFontEffect fontEffect = FontEffect_None,DWORD clrShadow = 0xa0101010,DWORD* pData = NULL);
	void DrawString(const WCHAR* lpszString , size_t count,const RECT &layoutRect ,DWORD clrText = 0xffffffff,DWORD dwTextFormat = DT_TOP | DT_LEFT,enumFontEffect fontEffect = FontEffect_None,DWORD clrShadow = 0xa0101010,DWORD* pData = NULL);

	int	SetHSpace(int n) { int old = m_nHSpace; m_nHSpace = n; return old; }
	int	SetVSpace(int n) { int old = m_nVSpace; m_nVSpace = n; return old; }
	int	SetTabSize(int n) { int old = m_nTabSize; m_nTabSize = n; return old; }

	float GetFontWidth(int iFont = -1);
	float GetFontWcharWidth(int iFont = -1);
	float GetFontHeight(int iFont = -1);
	float GetAFontWidth(int ichar,int iFont = -1);
	stPicCharInfo* GetPicCharInfo(int ichar);
	int SetFont(int iFont);

	void Init();

	void SetScale(float cx,float cy)
	{
		m_scale.x = cx;
		m_scale.y = cy;
	}
	stPointF GetScale(){ return m_scale;}
};