/**
 * \file      GuiImage.h
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUIÍ¼Ïñ¿Ø¼þ
 * 
 *	      GUIÍ¼Ïñ¿Ø¼þ
 */

#pragma once

#include "GuiStatic.h"
#include "../../engine/include/Animation.h"


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CGuiImage : public CGuiControl
{
public:
	enum enumUSE_TYPE
	{
		USE_TYPE_COMMON,
		USE_TYPE_HOLE,
		USE_TYPE_DIAMOND,
		USE_TYPE_NUMS,
	};
protected:

	struct stImgInfo{
		stRectI clip;
		stRectI rect;
		bool bHide;		//True to hide i.e.
		enumUSE_TYPE useType;	//see enumUSE_TYPE
		CAnimation image;

		stImgInfo()
		{
			bHide = false;
			useType = USE_TYPE_COMMON;
		}
	};
	std::vector<stImgInfo> m_Images;
	std::string m_strText;
	DWORD			m_TextColor;
	DWORD			m_dwTextShadowColor;
	DWORD			m_dwTextFormat;
	enumFontEffect	m_eFontEffect;
	//stRectI			m_rcClip;
public:
	bool			m_bIsBack;
	enum STYLE{
		STRETCH_IMAGE = 1, 
		FULLSIZE_IMAGE = 2,
	};

	CGuiImage(CGuiDialog *pDialog = NULL);
	~CGuiImage(void);
	
	void AddAnimation(const stResourceLocation & rl,DWORD color = -1,const RECT * clip = NULL,const RECT * rect = NULL, const bool bHide = false, const enumUSE_TYPE& useType = USE_TYPE_COMMON );
	void RemoveAnimation(int index);
	void Clear();
	bool ContainsPoint( POINT pt );
	HRESULT SetImage(const stResourceLocation & rl,const RECT * rc = NULL,DWORD color = -1, const bool bHide = false, const enumUSE_TYPE& useType = USE_TYPE_COMMON );
	void Render( float fElapsedTime );
	CAnimation * GetImage(int index = 0);

	void SetTextColor(DWORD color) { m_TextColor = color;}
	void SetTextShadowColor(DWORD color) { m_dwTextShadowColor = color;}
	void SetTextFormat(DWORD format) { m_dwTextFormat = format;}
	void SetTextEffect(enumFontEffect e){ m_eFontEffect = e;}
	void SetText(const char* szText) { m_strText = szText;}

	//Show or hide image by type
	void ShowImageByType( const enumUSE_TYPE& useType = enumUSE_TYPE::USE_TYPE_NUMS, const bool& bHide = false );

	const char* GetImgPackName();
	int GetImgGroup();
	int GetImgFrame();

	void SetColor(DWORD color);
	DWORD GetColor();

	bool IsNullImage(int index);
	void SetImageColor(int index,DWORD color);
	void SetImageBlend(int index,BlendType type);
};
