/**
 * \file      ToolTips.cpp
 * \version   $Id$
 * \author    $$$03公司@###.com
 *           $$$03公司@###.com
 * \date      2005-07-05 17:02:57
 * \brief     工具提示
 * 
 *	      工具提示
 */

#include "../../engine/include/engine.h"
#include "../include/tooltips.h"
#include "../../engine/include/stack_alloc.h"
#include "../../engine/include/stringex.h"
#include <typeinfo>
#include "../../gui/include/guiTypes.h"


const stPointI s_ptTipsBorderGrpLeftTop(9,9);
const stPointI s_ptTipsBorderGrpRightBot(9,9);

static IBitmaps*	s_pBmp[9];
static stRectI		s_borderRect[9];
const int c_nDefTipFont = 1;
const int c_nDefTipFontnew = -1;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x : 描述
 * \param y : 描述
 * \return 返回值的描述
 */
void CToolTips::stTipTextCell::Render(int x,int y)
{
	int iOldFont = GetDevice()->SetFont(iFont);
	GetDevice()->DrawString(szText.c_str(),stPointI(x,y),color,eFontEffect,colorShadow);
	GetDevice()->SetFont(iOldFont);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x : 描述
 * \param y : 描述
 * \return 返回值的描述
 */
void CToolTips::stTipImageCell::Render(int x,int y)
{
	image.Render(x,y);
}

//龙槽1坐标调整
void CToolTips::st3TipImageCell::Render(int x,int y)
{
	image.Render(x-5,y);
}

//龙槽2坐标调整
void CToolTips::st4TipImageCell::Render(int x,int y)
{
	image.Render(x-5,y);
}

//有龙槽星星1调整
void CToolTips::st5TipImageCell::Render(int x,int y)
{
	image.Render(x+3,y);
}

//有龙槽星星2调整
void CToolTips::st6TipImageCell::Render(int x,int y)
{
	image.Render(x+3,y);
}

//角色套装界面线条调整
void CToolTips::st10TipImageCell::Render(int x,int y)
{
	image.Render(x,y);
}
void CToolTips::stTipImageCellss::Render(int x,int y)
{
	image.Render(x+xx,y+yy);
}
//醉梦甩出动态参数 定义x和y 他妈的 搞那么多Render麻不麻烦？直接弄成传值 妈的 必须实现徒弟的梦想 高仿官服
void CToolTips::stTipImageCells::Render(int x,int y)
{
	image.Render(x+xx,y+yy);
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x : 描述
 * \param y : 描述
 * \return 返回值的描述
 */
void CToolTips::stTipLine::Render(int x,int y)
{
	for(size_t i=0;i<cells.size();++i)
	{
		cells[i]->Render(x,y);
		x += cells[i]->width;
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CToolTips::CToolTips(void)
: m_curColor(0xff000000)
, m_curColorShadow(0xff000000)
, m_dwBorderColor(0xff000000)
//, m_curFontEffect(FontEffect_Border)
, m_curFontEffect(FontEffect_None)
, m_width(0)
, m_height(0)
, m_dwBkColor(COLOR_ARGB(180,0,0,0))
{
	m_lines.reserve(32);
	m_alignType = GUI_ALIGN_HCENTER;

	m_gaps = 0;
	m_bShowBorderTex = true;
	m_bUseCustomeBkColor = false;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CToolTips::~CToolTips(void)
{
	Clear();
}

void CToolTips::Clear()
{ 
	for(size_t i=0;i<m_lines.size();++i)
	{
		delete m_lines[i];
	}
	m_lines.resize(0);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CToolTips::Resize()
{
	m_width = 0;
	m_height = 0;
	for(size_t i=0;i<m_lines.size();++i)
	{
		if(m_lines[i]->width > m_width) m_width = m_lines[i]->width;
		m_height += m_lines[i]->height;
	}

	m_height += m_lines.size() * m_gaps;

	//Add tips border texture size
	m_width += s_ptTipsBorderGrpLeftTop.x+s_ptTipsBorderGrpRightBot.x;
	m_height += s_ptTipsBorderGrpLeftTop.y+s_ptTipsBorderGrpRightBot.y;

	//union back color
	m_dwBkColor = COLOR_ARGB(210,0,0,0);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CToolTips::InitBorderTexture()
{
   //soke 装备、物品等边框
	stResourceLocation rl;
	rl.SetFileName( GetGuiGraphicPackName() );
	rl.group = 0;
	rl.frame = 27;

	stRectI rcTexture;
	stRectI rects[9];

	rcTexture.SetRect(0,0,37,38);

	CuttRect9(rcTexture,s_ptTipsBorderGrpLeftTop,s_ptTipsBorderGrpRightBot,rects);
	for(size_t i=0;i<9;++i)
	{
		s_pBmp[i] = GetDevice()->FindBitmaps( &rl );
		s_borderRect[i] = rects[i];
	}
}
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pszSrc : 描述
 * \param pszDest : 描述
 * \param nLineLen : 描述
 * \return 返回值的描述
 */
const char* str_split_begin(const char* pszSrc,char* pszDest,int nLineLen)
{
	size_t i;
	i =0;

	char * pstrDst = pszDest;

	while(*pszSrc)
	{
		if((*pszSrc) & 0x80)
		{
			if(!(*(pszSrc+1))) break;

			*pstrDst ++ = *pszSrc++;
			*pstrDst ++ = *pszSrc++;
			i+=2;

			if(i >= nLineLen)
			{
				// 换行
				break;
			}
		}
		else
		{
			size_t iCharNum;
			if(IsNewLine(pszSrc,iCharNum))
			{
				// 换行
				pszSrc += iCharNum;
				break;
			}
			else
			{
				*pstrDst ++ = *pszSrc++;
				++i;
				if( i >= nLineLen)
				{
					// 换行
					break;
				}
			}
		}
	}
	*pstrDst = 0;
	return pszSrc;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pszSrc : 描述
 * \param pszDest : 描述
 * \param nLineLen : 描述
 * \return 返回值的描述
 */
 //ZM 装备属性展示页面宽度
const char* str_split_begin1(const char *pszSrc, char *pszDest,int nLineLen)
{
	size_t i;
	i =0;

	char *pstrDst = pszDest;

	while(*pszSrc)
	{
		if((*pszSrc) & 0x80)
		{
			if(!(*(pszSrc+1))) break;

			*pstrDst ++ = *pszSrc++;
			*pstrDst ++ = *pszSrc++;
			i+=2;

			if(i >= nLineLen)
			{
				// 换行
			  //break;
			}
		}
		else
		{
			size_t iCharNum;
			if(IsNewLine(pszSrc,iCharNum))
			{
				// 换行
				pszSrc += iCharNum;
				//break;
			}
			else
			{
				*pstrDst ++ = *pszSrc++;
				++i;
				if( i >= nLineLen)
				{
					// 换行
					//break;
				}
			}
		}
	}
	*pstrDst = 0;
	return pszSrc;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pszText : 描述
 * \param iFont : 描述
 * \return 返回值的描述
 */
void CToolTips::AddText(const char* pszText,int iFont)
{
	size_t icharnum = 0;
	stTipLine* pLine = NULL;
	if(m_lines.empty() || IsNewLine(pszText,icharnum))
	{
		m_lines.push_back(new stTipLine);
		pszText += icharnum;
	}
	pLine = m_lines.back();

	const int maxWidth=40;
	const int iFontWidth = 6;
	//*
	SetFrameAllocator fa;
	std::vector<std::stack_string,std::stack_allocator<std::stack_string> > vString;

	int len1 = maxWidth - pLine->width/iFontWidth;
	if(len1 > 0)
	{
		char szBuf[256];
		pszText = str_split_begin(pszText,szBuf,len1);
		vString.push_back(szBuf);
	}
	else
	{
		m_lines.push_back(new stTipLine);
		pLine = m_lines.back();
	}

	str_split(pszText,vString, maxWidth );		//	200/6 

	for(size_t i=0;i<vString.size();++i)
	{
		if(pLine == NULL)
		{
			m_lines.push_back(new stTipLine);
			pLine = m_lines.back();
		}
		pLine->AddText(vString[i].c_str(),iFont,m_curColor,m_curColorShadow,m_curFontEffect);
		pLine = NULL;
	}
	//*/
}


void CToolTips::AddText1(const char* pszText,int iFont)
{
	size_t icharnum = 0;
	stTipLine* pLine = NULL;
	if(m_lines.empty() || IsNewLine(pszText,icharnum))
	{
		m_lines.push_back(new stTipLine);
		pszText += icharnum;
	}
	pLine = m_lines.back();

	const int maxWidth=40;
	const int iFontWidth = 6;
	//*
	SetFrameAllocator fa;
	std::vector<std::stack_string,std::stack_allocator<std::stack_string> > vString;

	int len1 = maxWidth - pLine->width/iFontWidth;
	if(len1 > 0)
	{
		char szBuf[255]; //ZM 装备属性展示页面宽度
		pszText = str_split_begin1(pszText,szBuf,len1);
		vString.push_back(szBuf);
	}
	else
	{
		m_lines.push_back(new stTipLine);
		pLine = m_lines.back();
	}

	str_split(pszText,vString, maxWidth );		//	200/6 

	for(size_t i=0;i<vString.size();++i)
	{
		if(pLine == NULL)
		{
			m_lines.push_back(new stTipLine);
			pLine = m_lines.back();
		}
		pLine->AddText1(vString[i].c_str(),iFont,m_curColor,m_curColorShadow,m_curFontEffect);
		pLine = NULL;
	}
	//*/
}

void CToolTips::AddText2(const char* pszText,int iFont)
{
	size_t icharnum = 0;
	stTipLine* pLine = NULL;
	if(m_lines.empty() || IsNewLine(pszText,icharnum))
	{
		m_lines.push_back(new stTipLine);
		pszText += icharnum;
	}
	pLine = m_lines.back();

	const int maxWidth=40;
	const int iFontWidth = 6;
	//*
	SetFrameAllocator fa;
	std::vector<std::stack_string,std::stack_allocator<std::stack_string> > vString;

	int len1 = maxWidth - pLine->width/iFontWidth;
	if(len1 > 0)
	{
		char szBuf[255]; //ZM 装备属性展示页面宽度
		pszText = str_split_begin1(pszText,szBuf,len1);
		vString.push_back(szBuf);
	}
	else
	{
		m_lines.push_back(new stTipLine);
		pLine = m_lines.back();
	}

	str_split(pszText,vString, maxWidth );		//	200/6 

	for(size_t i=0;i<vString.size();++i)
	{
		if(pLine == NULL)
		{
			m_lines.push_back(new stTipLine);
			pLine = m_lines.back();
		}
		pLine->AddText2(vString[i].c_str(),iFont,m_curColor,m_curColorShadow,m_curFontEffect);
		pLine = NULL;
	}
	//*/
}


//桃子新增框
void CToolTips::AddText3(const char* pszText,int iFont)
{
	size_t icharnum = 0;
	stTipLine* pLine = NULL;
	if(m_lines.empty() || IsNewLine(pszText,icharnum))
	{
		m_lines.push_back(new stTipLine);
		pszText += icharnum;
	}
	pLine = m_lines.back();

	const int maxWidth=40;
	const int iFontWidth = 6;
	//*
	SetFrameAllocator fa;
	std::vector<std::stack_string,std::stack_allocator<std::stack_string> > vString;

	int len1 = maxWidth - pLine->width/iFontWidth;
	if(len1 > 0)
	{
		char szBuf[255]; //MYY 装备属性展示页面宽度
		pszText = str_split_begin1(pszText,szBuf,len1);
		vString.push_back(szBuf);
	}
	else
	{
		m_lines.push_back(new stTipLine);
		pLine = m_lines.back();
	}

	str_split(pszText,vString, maxWidth );		//	200/6 

	for(size_t i=0;i<vString.size();++i)
	{
		if(pLine == NULL)
		{
			m_lines.push_back(new stTipLine);
			pLine = m_lines.back();
		}
		pLine->AddText3(vString[i].c_str(),iFont,m_curColor,m_curColorShadow,m_curFontEffect);
		pLine = NULL;
	}
	//*/
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param rl : 描述
 * \param bNextLine : 描述
 * \return 返回值的描述
 */
void CToolTips::AddAnimation(const stResourceLocation *rl,bool bNextLine)
{
	if(bNextLine || m_lines.empty())
	{
		m_lines.push_back(new stTipLine);
	}
	stTipLine* pLine = m_lines.back();
	pLine->AddAnimation(rl);
}
void CToolTips::AddAnimations(const stResourceLocation *rl,int x,int y,int xs,int ys)
{
	if(m_lines.empty())
	{
		m_lines.push_back(new stTipLine);
	}
	stTipLine* pLine = m_lines.back();
	pLine->AddAnimations(rl,x,y,xs,ys);
}
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param rl : 描述
 * \param bNextLine : 描述
 * \return 返回值的描述
 */
//soke 2016-5-3 实现特效图不占高度
void CToolTips::AddAnimation1(const stResourceLocation *rl,bool bNextLine)
{
	if(bNextLine || m_lines.empty())
	{
		m_lines.push_back(new stTipLine);
	}
	stTipLine* pLine = m_lines.back();

	pLine->AddAnimation1(rl);
}
void CToolTips::AddAnimation1s(const stResourceLocation *rl,int x,int y,int xs,int ys)
{
	if(m_lines.empty())
	{
		m_lines.push_back(new stTipLine);
	}
	stTipLine* pLine = m_lines.back();

	pLine->AddAnimation1s(rl,x,y,xs,ys);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param rl : 描述
 * \param bNextLine : 描述
 * \return 返回值的描述
 */
//soke 2016-5-3 实现特效图不占高度、宽度
void CToolTips::AddAnimation2(const stResourceLocation *rl,bool bNextLine)
{
	if(bNextLine || m_lines.empty())
	{
		m_lines.push_back(new stTipLine);
	}
	stTipLine* pLine = m_lines.back();

	pLine->AddAnimation2(rl);
}

//sky 特效图不占高度、宽度
void CToolTips::AddAnimation3(const stResourceLocation *rl,bool bNextLine)
{
	if(bNextLine || m_lines.empty())
	{
		m_lines.push_back(new stTipLine);
	}
	stTipLine* pLine = m_lines.back();

	pLine->AddAnimation3(rl);
}

//sky 特效图不占高度、宽度
void CToolTips::AddAnimation4(const stResourceLocation *rl,bool bNextLine)
{
	if(bNextLine || m_lines.empty())
	{
		m_lines.push_back(new stTipLine);
	}
	stTipLine* pLine = m_lines.back();

	pLine->AddAnimation4(rl);
}

//sky 特效图不占高度、宽度
void CToolTips::AddAnimation5(const stResourceLocation *rl,bool bNextLine)
{
	if(bNextLine || m_lines.empty())
	{
		m_lines.push_back(new stTipLine);
	}
	stTipLine* pLine = m_lines.back();

	pLine->AddAnimation5(rl);
}

//sky 特效图不占高度、宽度
void CToolTips::AddAnimation6(const stResourceLocation *rl,bool bNextLine)
{
	if(bNextLine || m_lines.empty())
	{
		m_lines.push_back(new stTipLine);
	}
	stTipLine* pLine = m_lines.back();

	pLine->AddAnimation6(rl);
}

//ZM 装备套装效果 线条高度 宽度
void CToolTips::AddAnimation10(const stResourceLocation *rl,bool bNextLine)
{
	if(bNextLine || m_lines.empty())
	{
		m_lines.push_back(new stTipLine);
	}
	stTipLine* pLine = m_lines.back();
	pLine->AddAnimation10(rl);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x : 描述
 * \param y : 描述
 * \return 返回值的描述
 */
void CToolTips::RenderBorderTextuer( int x,int y )
{
	stRectI rcBack( x, y, x+m_width, y+m_height );
	stRectI rects[9];
	CuttRect9(rcBack,s_ptTipsBorderGrpLeftTop,s_ptTipsBorderGrpRightBot,rects);
	for(size_t i=1;i<9;++i)
	{
		PatchRenderImage(rects[i],s_pBmp[i],&s_borderRect[i],-1,Blend_Null,s_PatchAlignFormat[i]);
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param rcBBox : 描述
 * \param devWidth : 描述
 * \param devHeight : 描述
 * \return 返回值的描述
 */
//-------------------------------------------------------------------------------
//Give tips some EQ
//First make sure tips full visible
//Then we make sure tips not on item
//
//	reBBox	: control bounding box(left,top,right,bottom) ,related to screen pixel pos
//	devWidth : limited range width
//	devHeight : limited range height
//
//-------------------------------------------------------------------------------
void CToolTips::RenderTopLeftPrefer( stRectI rcBBox, int devWidth, int devHeight )
{
	//Get tips position
	int x,y;
	x = rcBBox.left - m_width;
	y = rcBBox.top - m_height;

	//Adjust to valid
	if( x < 0 )
		x = rcBBox.right;
	if( x + m_width > devWidth )		//Make sure visible
		x = devWidth -m_width;

	if( y < 0 )
		y = rcBBox.bottom;
	if ( y + m_height > devHeight )		//make sure visible
		y = devHeight - m_height;

	//Call render
	//Render( x, y );

	//union back color
	m_dwBkColor = COLOR_ARGB(210,0,0,0);
	if ( NULL == GetGuiManager()->GetToolTips() )
		this->x = x;
		this->y = y;
		GetGuiManager()->SetToolTips( this, stPointI(x,y) );
}



/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x : 描述
 * \param y : 描述
 * \return 返回值的描述
 */
void CToolTips::Render(int x,int y)
{
	//First render border texture
	if ( m_bShowBorderTex )
		RenderBorderTextuer( x, y );

	//union back color
	if ( !m_bUseCustomeBkColor )
     	//soke 装备、物品等说明透明度
		m_dwBkColor = COLOR_ARGB(180,0,0,0);

    //soke 调节透明度边框大小、高度
	stRectI bkRect( x-5+s_ptTipsBorderGrpLeftTop.x,
					y-5+s_ptTipsBorderGrpLeftTop.y,
					x + m_width + 5 - s_ptTipsBorderGrpLeftTop.x,
					y + m_height + 5 - s_ptTipsBorderGrpLeftTop.y );
	
	GetDevice()->FillRect(&bkRect, m_dwBkColor);		//

	if( m_dwBorderColor & 0x00000000 )		//0xff000000
		GetDevice()->DrawRectangle(&bkRect,m_dwBorderColor);

	x += s_ptTipsBorderGrpLeftTop.x;
	y += s_ptTipsBorderGrpLeftTop.y;

	for(size_t i=0;i<m_lines.size();++i)
	{
		stTipLine& line = *m_lines[i];
		stPointI pt(x + (GetWidth() - s_ptTipsBorderGrpLeftTop.x - line.width)/2, y);
		if ( m_alignType & GUI_ALIGN_LEFT )
			pt.x = x;
		if ( m_alignType & GUI_ALIGN_RIGHT )
			pt.x = x + (GetWidth() - line.width);
		
		line.Render(pt.x,pt.y);
		y+= line.height + m_gaps;
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
const char* CToolTips::GetText()
{
	if(m_lines.empty()) return NULL;
	return m_lines[0]->GetText();
}