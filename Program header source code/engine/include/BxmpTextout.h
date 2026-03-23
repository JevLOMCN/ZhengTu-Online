/**
*\file		BxmpTextout.h
*\version	$ID$
*\author	$$$04公司
*			$$$04公司@###.com
*\date		2005-07-06 11:50:00
*\brief	图片字的显示类
*
* 图片字，用于在pk 时显示伤害值、经验值等
*
*/

#pragma once

/**
 * \brief 图片字显示
 * 
 * 图片字，用于在pk 时显示伤害值、经验值等
 * 
 */
class CBxmpTextout
{
public:
	CBxmpTextout(void);
	~CBxmpTextout(void);

	void SetResourceLocation(const char* fileName,int group);
	void SetCharPicture(WORD ch,int frame);	
	// 2010 3 添加新字体
	void DrawString(POINT pt,const char* pszString,int count,DWORD color,stPointF * scale = NULL, BlendType eBlendType = Blend_IgnoreTexelColor);
	SIZE GetStringSize(const char* pszString,int count);

private:
	typedef std::map<WORD,int> CharIndexMap;

	///字符(WORD) 图片对应表
	CharIndexMap	m_mapCharIndex;

	///图片资源位置(存在同一个group里面)
	stResourceLocation	m_rl;
};

CBxmpTextout* GetBxmpTextout();