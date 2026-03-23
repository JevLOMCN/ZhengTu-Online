/**
 *\file		GuiClothColorDialog.h
 *\version	$ID$
 *\author	$$$04公司
 *			$$$04公司@###.com
 *\date		2005-07-06 11:18:00
 *\brief	服装换色对话框
 *
 * 跟服装师npc对话时显示的对话框，用于更换角色服装的颜色，移动slider时，角色的服装跟着变色。
 *
 * 确定则扣钱-换色，取消则退出，回到原来的颜色，
 *
 */

#pragma once
#include "..\gui\include\guidialog.h"
#include "./GameScene.h"

/**
 * \brief 服装换色对话框
 * 
 * 跟服装师npc对话时显示的对话框，用于更换角色服装的颜色，移动slider时，角色的服装跟着变色。
 *
 * 确定则扣钱-换色，取消则退出，回到原来的颜色，
 * 
 */
class CGuiClothColorDialog :
	public CGuiDialog
{
public:
	CGuiClothColorDialog(void);
	~CGuiClothColorDialog(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnSetCloseLocation();

	bool OnSliderMove(UINT nID);

	void SetDefaultColor(DWORD color);
	HRESULT OnRender(float fElapsedTime);
	void OnClose();
private:
	void SetColorOnColorWnd(DWORD color);
	void SetColorOnSlider(DWORD color);
private:
	///R 分量Slider
	CGuiSlider *m_pRSlider;

	///G 分量Slider
	CGuiSlider *m_pGSlider;

	///B 分量Slider
	CGuiSlider *m_pBSlider;
	
	///颜色显示区域
	CGuiStatic *m_pColorStatic;

	///当前显示的颜色
	DWORD m_dwColor;

	///打开对话框时角色衣服的颜色
	DWORD m_dwOldBodyColor;
};
