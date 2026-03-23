/**
 *\file		GuiItemDetailInfo.h
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:13:00 中国标准时间
 *\brief	道具详细信息界面
 *
 * 
 *
*/

#pragma once

#include "..\gui\include\guidialog.h"

#define ITEM_DETAIL_INFO_COUNT	3

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiItemDetailInfo :
	public CGuiDialog
{
	struct stExchangeItem
	{
		char		owner[MAX_NAMESIZE];
		t_Object	object;
		CToolTips	infoDetail;
		stPointI	ptInfo;
		
		stExchangeItem(const t_Object& obj, const char* ownerName)
		{
			object = obj;
			sprintf( owner, ownerName );
			ptInfo = stPointI(0,0);
			infoDetail.SetAlignType( GUI_ALIGN_LEFT );
			infoDetail.Clear();
		}

		void Render(int x, int y)
		{
			infoDetail.Render( x+ptInfo.x, y+ptInfo.y );
		}
	};
	typedef std::deque<stExchangeItem>		aItemDetailInfo;
	typedef std::deque<stExchangeItem>::iterator	aItemDetailInfoIt;
	aItemDetailInfo m_aItemDetailList;

public:
	CGuiItemDetailInfo(void);
	void OnClose(void);
	void OnCreate();
	void OnResetScreenSize();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void RenderBackground(float fElapsedTime);
	HRESULT OnRender(float fElapsedTime);

	void SetActiveItem( const t_Object& object, const char* ownerName );

	void UpdateItemDetailInfo();
};