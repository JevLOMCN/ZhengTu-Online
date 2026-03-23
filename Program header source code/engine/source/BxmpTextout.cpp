/**
 *\file		BxmpTextout.cpp
 *\version	$ID$
 *\author	$$$04公司
 *			$$$04公司@###.com
 *\date		2005-07-06 11:50:00
 *\brief	图片字的显示类
 *
 * 图片字，用于在pk 时显示伤害值、经验值等
 *
 */

#include "../include/engine.h"
#include "..\include\bxmptextout.h"
#include "../../xml_parse/XmlParseEx.h"

/**
* \brief 得到当前CBxmpTextout对象指针
* 
* \return 当前CBxmpTextout静态对象指针
*/
CBxmpTextout* GetBxmpTextout()
{
	static CBxmpTextout  sm;
	return &sm;
}

/**
 * \brief 构造函数
 * 
 * 装载索引表(data\datas.pak-->datas\\BmpText1.xml)
 *
 */
CBxmpTextout::CBxmpTextout(void)
{
	SetResourceLocation("data\\Interfaces1.gl",3);

	TiXmlDocument doc;
	Stream* pStream = OpenPackFileForRead("data\\datas.pak","datas\\BmpText1.xml");
	if(!pStream)
	{
		Engine_WriteLog("装载 BmpText1.xml 失败\n");
		return ;
	}
	if(!LoadXMLFromStream(pStream,doc))
	{
		ClosePackFileForRead(pStream);
		Engine_WriteLog("解析 BmpText1.xml 失败\n");
		return;
	}
	wchar_t wString[256];
	TiXmlElement* BmpTexts = doc.FirstChildElement("BmpTexts");
	if(BmpTexts)
	{
		TiXmlElement* Text = BmpTexts->FirstChildElement("Text");
		while(Text)
		{
			const char* word = Text->Attribute("word");		
			if(strlen(word) > 4)	continue;

			memset(wString,0,sizeof(wchar_t) * strlen(word));
			size_t size = my_mbstowcs(wString,word,strlen(word));
			
			int bmp = -1;
			if(Text->QueryIntAttribute("bmp",&bmp) != TIXML_SUCCESS)
				bmp = -1;

			if( bmp >= 0 && strlen(word) > 0)
			{				
				SetCharPicture(wString[0],bmp);
			}		

			Text = Text->NextSiblingElement("Text");
		}
	}
	ClosePackFileForRead(pStream);
}

/**
 * \brief 析构函数
 * 
 */
CBxmpTextout::~CBxmpTextout(void)
{
}


/**
* \brief 设置图资源的位置
* 
* \param fileName : 图包文件名
* \param group : 图片字所在的组
* \return void
*/
void CBxmpTextout::SetResourceLocation(const char* fileName,int group)
{
	m_rl.SetFileName(fileName);
	m_rl.group = group;
}

/**
* \brief 设置字符的图片
* 
* \param ch : 字符
* \param frame : 字符对应的图片id
* \return void
*/
void CBxmpTextout::SetCharPicture(WORD ch,int frame)
{
	m_mapCharIndex.insert(CharIndexMap::value_type(ch,frame));
}


/**
 * \brief 显示图片字
 * 
 * 以pt为中心,显示颜色为color,伸缩比例为scale的长度为count的字符串pszString
 *
 * 如果字符不存在，则显示原先的字符。
 * 
 * \param pt : 中心点
 * \param pszString : 字符串
 * \param count : 字符串长度
 * \param color : 颜色
 * \param scale : 伸缩比
 * \return void
 */
 //soke 添加的新字体
void CBxmpTextout::DrawString(POINT pt,const char* pszString,int count,DWORD color,stPointF * scale, BlendType eBlendType)
{
	if(m_rl.IsEmpty()) return ;
	wchar_t wString[256];
	memset(wString,0,sizeof(wchar_t) * count);
	size_t size = my_mbstowcs(wString,pszString,count);

	SIZE s = GetStringSize(pszString,count);

	POINT tmpt;

	stPointF *scaletemp = scale;

	stPointF p(1,1);
	if(!scale)
	{
		scaletemp = &p;
	}
	tmpt.x = pt.x - (long)(s.cx/2 * scaletemp->x);
	tmpt.y = pt.y - (long)(s.cy/2 * scaletemp->y);

	for(size_t i = 0;i < size;i++)
	{
		if(i >= count) continue ;
		WORD w = wString[i];
		CharIndexMap::iterator it = m_mapCharIndex.find(w); 
		if(it == m_mapCharIndex.end())
		{
			GetDevice()->DrawString(&w,1,tmpt,color);
			tmpt.x += 10;
			continue;
		}
		m_rl.frame = it->second;
		IBitmaps*	pImage = GetDevice()->FindBitmaps(&m_rl);
		if(!pImage) 
		{
			GetDevice()->DrawString(&w,1,tmpt,color);
			tmpt.x += 10;
			continue;
		}
		IBitmap *pBitmap = pImage->GetBitmap(0);
		if(!pBitmap)
		{
			GetDevice()->DrawString(&w,1,tmpt,color);
			tmpt.x += 10;
			continue;
		}
		stRectI rRect = pBitmap->GetClipRect();
		
		// 2010 4
		//pBitmap->Render(tmpt.x,tmpt.y,NULL,scaletemp,color,Blend_IgnoreTexelColor);
		pBitmap->Render(tmpt.x,tmpt.y,NULL,scaletemp,color,eBlendType);


	

		tmpt.x += (long)(rRect.Width() * scaletemp->x);
	}
}

/**
* \brief 获取字符串的绘制区域大小
* 
* \param pszString : 字符串
* \param count : 字符串长度
* \return 字符串的绘制区域大小
*/
SIZE CBxmpTextout::GetStringSize(const char* pszString,int count)
{
	SIZE RectSize = SIZE();
	RectSize.cy = 0;
	if(m_rl.IsEmpty()) return SIZE();
	wchar_t wString[256];
	memset(wString,0,sizeof(wchar_t) * count);
	size_t size = my_mbstowcs(wString,pszString,count);
	for(size_t i = 0;i < size;i++)
	{
		if(i >= count) continue ;
		WORD w = wString[i];
		CharIndexMap::iterator it = m_mapCharIndex.find(w); 
		if(it == m_mapCharIndex.end())
		{
			RectSize.cx += 10;
			continue;
		}
		m_rl.frame = it->second;
		IBitmaps*	pImage = GetDevice()->FindBitmaps(&m_rl);
		if(!pImage)
		{
			RectSize.cx += 10;
			continue;
		}
		IBitmap *pBitmap = pImage->GetBitmap(0);
		if(!pBitmap)
		{
			RectSize.cx += 10;
			continue;
		}
		stRectI rRect = pBitmap->GetClipRect();
		RectSize.cy = (rRect.Width() > RectSize.cy)?rRect.Width():RectSize.cy;
		RectSize.cx += rRect.Width();
	}
	return RectSize;
}