/**
 * \file      ToolTips.h
 * \version   $Id$
 * \author    $$$03公司@###.com
 *           $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     工具提示
 * 
 *	      工具提示
 */

#pragma once

#include "../../engine/include/Animation.h"

extern const int c_nDefTipFont;
extern const int c_nDefTipFontnew;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CToolTips
{
	class stTipCellBase
	{
	public:
		int		width;
		int		height;
		virtual void Render(int x,int y) = 0;
		virtual ~stTipCellBase(){}
		virtual const char* GetText(){ return NULL;}
	};
	
	class stTipTextCell : public stTipCellBase
	{
	public:
		DWORD			color;
		DWORD			colorShadow;
		enumFontEffect	eFontEffect;
		std::string		szText;
		int				iFont;
		stTipTextCell(const char* pszText,int i_iFont,DWORD dwColor = -1,DWORD dwColorShadow=0xff000000,enumFontEffect fontEffect = FontEffect_Border)
			: iFont(i_iFont),eFontEffect(fontEffect) , color(dwColor) , colorShadow(dwColorShadow)
		{
			szText = pszText;
			width = szText.length() * GetDevice()->GetFontWidth(iFont);
			height = GetDevice()->GetFontHeight(iFont);
		}

		void Render(int x,int y);
		const char* GetText(){ return szText.c_str();}
		~stTipTextCell(){}
	};

	class stTipImageCell : public stTipCellBase
	{
	public:
		CAnimation image;
		
		void Render(int x,int y);

		stTipImageCell(const stResourceLocation* rl)
		{
			image.Create(rl);
			width = image.GetWidth();
			height = image.GetHeight();
		}
		~stTipImageCell(){}
	};
	class stTipImageCellss : public stTipCellBase
	{
	public:
		CAnimation image;
		int xx;
		int yy;
		void Render(int x,int y);

		stTipImageCellss(const stResourceLocation* rl)
		{
			image.Create(rl);
			width = image.GetWidth();
			height = image.GetHeight();
		}
		~stTipImageCellss(){}
	};
	class stTipImageCells : public stTipCellBase
	{
	public:
		CAnimation image;
		int xx;
		int yy;
		
		void Render(int x,int y);

		stTipImageCells(const stResourceLocation* rl)
		{
			image.Create(rl);
			width = image.GetWidth();
			height = image.GetHeight();
		}
		~stTipImageCells(){}
	};
	
	//sky 龙槽1距离调整
	class st3TipImageCell : public stTipCellBase
	{
	public:
		CAnimation image;
		void Render(int x,int y);

		st3TipImageCell(const stResourceLocation* rl)
		{
			image.Create(rl);
			width = image.GetWidth()-9;
			height = image.GetHeight();
		}
		~st3TipImageCell(){}
	};
	
	//sky 龙槽2距离调整
	class st4TipImageCell : public stTipCellBase
	{
	public:
		CAnimation image;
		
		void Render(int x,int y);

		st4TipImageCell(const stResourceLocation* rl)
		{
			image.Create(rl);
			width = image.GetWidth()-6;//龙槽间距
			height = image.GetHeight();
		}
		~st4TipImageCell(){}
	};
	
	//sky 星星1距离调整
	class st5TipImageCell : public stTipCellBase
	{
	public:
		CAnimation image;
		
		void Render(int x,int y);

		st5TipImageCell(const stResourceLocation* rl)
		{
			image.Create(rl);
			width = image.GetWidth()+9; //星星间距
			height = image.GetHeight();
		}
		~st5TipImageCell(){}
	};
	
	//sky 星星2距离调整
	class st6TipImageCell : public stTipCellBase
	{
	public:
		CAnimation image;
		
		void Render(int x,int y);

		st6TipImageCell(const stResourceLocation* rl)
		{
			image.Create(rl);
			width = image.GetWidth()+10; //星星间距
			height = image.GetHeight();
		}
		~st6TipImageCell(){}
	};

	//myy 线条调整高度
	class st10TipImageCell : public stTipCellBase
	{
	public:
		CAnimation image;
		
		void Render(int x,int y);

		st10TipImageCell(const stResourceLocation* rl)
		{
			image.Create(rl);
			width = image.GetWidth(); 
			height = image.GetHeight()-28; //线条高度间距
		}
		~st10TipImageCell(){}
	};

	class stTipLine{
	public:
		std::vector<stTipCellBase*> cells;
		int width,height;

		const char* GetText()
		{
			if(cells.empty()) return NULL;
			return cells[0]->GetText();
		}

		void AddText(const char* pszText,int iFont,DWORD color,DWORD colorShadow,enumFontEffect eFontEffect)
		{
			stTipTextCell* p = new stTipTextCell(pszText,iFont,color,colorShadow,eFontEffect);
			cells.push_back(p);
			if(p->height > height) height = p->height;
			width += p->width;
		}

		void AddText1(const char* pszText,int iFont,DWORD color,DWORD colorShadow,enumFontEffect eFontEffect)
		{
			stTipTextCell* p = new stTipTextCell(pszText,iFont,color,colorShadow,eFontEffect);
			cells.push_back(p);
			if(p->height > height) height = p->height;
			width += p->width;
		}
		
		void AddText2(const char* pszText,int iFont,DWORD color,DWORD colorShadow,enumFontEffect eFontEffect)
		{
			stTipTextCell* p = new stTipTextCell(pszText,iFont,color,colorShadow,eFontEffect);
			cells.push_back(p);
			if(p->height > height) height = p->height;
			width += p->width;
		}
		//桃子增加新框页面
		void AddText3(const char* pszText,int iFont,DWORD color,DWORD colorShadow,enumFontEffect eFontEffect)
		{
			stTipTextCell* p = new stTipTextCell(pszText,iFont,color,colorShadow,eFontEffect);
			cells.push_back(p);
			//if(p->height > height) height = p->height;
			//width += p->width;
		}	
		
		void AddAnimation(const stResourceLocation* rl)
		{
			stTipImageCell* p = new stTipImageCell(rl);
			cells.push_back(p);
			if(p->height > height) height = p->height;
			width += p->width;
		}

		//x,y为图片坐标偏移量 xs,ys占地偏移量
		void AddAnimations(const stResourceLocation* rl,int x,int y,int xs,int ys)
		{
			stTipImageCells* p = new stTipImageCells(rl);
			p->xx=x;  //骂了隔壁 甩动态出来 太费劲了
			p->yy=y;
			cells.push_back(p);
			if(p->height > height) height = p->height+ys;
			width += p->width+xs;
		}

		//soke 实现特效图片不占高度（例如：interfaces、3、250）
		void AddAnimation1(const stResourceLocation* rl)
		{
			stTipImageCell* p = new stTipImageCell(rl);
			cells.push_back(p);
		//	if(p->height > height) height = p->height;
			width += p->width;
		}

		//soke 实现特效图片不占高度（例如：interfaces、3、250）
		void AddAnimation1s(const stResourceLocation* rl,int x,int y,int xs,int ys)
		{
			stTipImageCellss* p = new stTipImageCellss(rl);
			p->xx=x;  //骂了隔壁 甩动态出来 太费劲了
			p->yy=y;
			cells.push_back(p);
			// if(p->height > height) height = p->height+xs;
			// width += p->width+xs;
		}

		//soke 实现特效图片不占高度/宽度（例如：interfaces、3、250）
		void AddAnimation2(const stResourceLocation* rl)
		{
			stTipImageCell* p = new stTipImageCell(rl);
			cells.push_back(p);
		//	if(p->height > height) height = p->height;
		//	width += p->width;
		}
		
		//sky 龙槽1
		void AddAnimation3(const stResourceLocation* rl)
		{
			st3TipImageCell* p = new st3TipImageCell(rl);
			cells.push_back(p);
		    if(p->height > height) height = p->height-28; //28
			width = p->width;
		}
		
		//sky 龙槽2
		void AddAnimation4(const stResourceLocation* rl)
		{
			st4TipImageCell* p = new st4TipImageCell(rl);
			cells.push_back(p);
			if(p->height > height) height = p->height-28; // 龙槽高度28
			//if(p->height > height) height = p->height;
			width += p->width;
		}
		
		//sky 有龙槽星星1
		void AddAnimation5(const stResourceLocation* rl)
		{
			st5TipImageCell* p = new st5TipImageCell(rl);
			cells.push_back(p);
			if(p->height > height) height = p->height;
			width += p->width;
		}
		
			//sky 有龙槽星星2
		void AddAnimation6(const stResourceLocation* rl)
		{
			st6TipImageCell* p = new st6TipImageCell(rl);
			cells.push_back(p);
			if(p->height > height) height = p->height;
			width += p->width;
		}

			//ZM 角色套装线条高度调整
		void AddAnimation10(const stResourceLocation* rl)
		{
		    st10TipImageCell* p = new st10TipImageCell(rl);
		    cells.push_back(p);
			if(p->height > height) height = p->height;
			width += p->width;
		}	

		stTipLine() : width(0) , height(0){}

		~stTipLine()
		{
			for(size_t i=0;i<cells.size();++i)
			{
				delete cells[i];
			}
			cells.clear();
		}
		void Render(int x,int y);
	};

	std::vector<stTipLine*> m_lines;
	DWORD m_curColor;
	DWORD m_curColorShadow;
	DWORD m_dwBorderColor;
	enumFontEffect m_curFontEffect;
	DWORD	m_alignType;

	DWORD m_dwBkColor;

	int m_gaps;		//gaps between line

	int m_width;
	int m_height;

	
	
	bool	m_bShowBorderTex;
	bool	m_bUseCustomeBkColor;

public:
	
	CToolTips(void);
	~CToolTips(void);
	int x;
	int y;
	void Clear();
	bool Empty() { return m_lines.empty(); }
	DWORD SetCurColor(DWORD color){ DWORD tmp = color; m_curColor = color; return tmp;}
	DWORD SetCurShadowColor(DWORD color) { DWORD tmp = m_curColorShadow; m_curColorShadow = color; return tmp;}
	enumFontEffect SetCurFontEffect(enumFontEffect e) { enumFontEffect eTmp = m_curFontEffect; m_curFontEffect = e; return eTmp;}
	void SetBkColor(DWORD color) { m_dwBkColor = color;}
	void SetAlignType(DWORD alignType) { m_alignType = alignType; }
	void SetBorderColor(DWORD color ) { m_dwBorderColor = color; }
	void SetBorderTexVisible(bool bShow = true) { m_bShowBorderTex = bShow; }
	void SetUseCustomeBkColor(bool bCustom = true) { m_bUseCustomeBkColor = bCustom; }
	void Resize();	

	void SetLineGaps(int gaps) { m_gaps = gaps; }
	int GetLineGaps() { return m_gaps; }

	int GetWidth(){ return m_width;}
	int GetHeight(){ return m_height;}
	DWORD GetAlignType() { return m_alignType; }
	DWORD GetCurColor() { return m_curColor; }
	
	//醉梦正常装备框
	static void InitBorderTexture();

	void AddText(const char* pszText,int iFont = c_nDefTipFont);
	void AddText2(const char* pszText,int iFont = c_nDefTipFontnew);
	void AddText1(const char* pszText,int iFont = c_nDefTipFont);
	void AddText3(const char* pszText,int iFont = c_nDefTipFont); //桃子新增框
	void AddAnimation(const stResourceLocation *rl,bool bNextLine);
	void AddAnimations(const stResourceLocation *rl,int x,int y,int xs,int ys);
	void AddAnimation1s(const stResourceLocation *rl,int x,int y,int xs,int ys);
	void AddAnimation1(const stResourceLocation *rl,bool bNextLine);
	void AddAnimation2(const stResourceLocation *rl,bool bNextLine);
	void AddAnimation3(const stResourceLocation *rl,bool bNextLine);
	void AddAnimation4(const stResourceLocation *rl,bool bNextLine);
	void AddAnimation5(const stResourceLocation *rl,bool bNextLine);
	void AddAnimation6(const stResourceLocation *rl,bool bNextLine);
	void AddAnimation10(const stResourceLocation *rl,bool bNextLine);
	void RenderBorderTextuer(int x,int y);
	void Render(int x,int y);
	void RenderTopLeftPrefer(stRectI rcBBox, int devWidth, int devHeight);
	const char* GetText();

};
