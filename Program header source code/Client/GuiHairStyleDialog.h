/**
 *\file		GuiHairStyleDialog.h
 *\version	$ID$
 *\author	$$$04公司
 *			$$$04公司@###.com
 *\date		2005-07-06 09:35:00
 *\brief	换发型、发色对话框
 *
 * 跟发型师npc对话时显示的对话框，用于更换角色的发型和发色
 *
 */

#pragma once
#include "..\gui\include\guidialog.h"
#include "./GameScene.h"

#include <vector>

/**
 * \brief 换发型、发色对话框
 * 
 * 跟发型师npc对话时显示的对话框，用于更换角色的发型和发色
 * 
 */
class CGuiHairStyleDialog :
	public CGuiDialog
{
public:
	CGuiHairStyleDialog(void);
	~CGuiHairStyleDialog(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	bool OnOK();
	void OnClose();
	void OnSetCloseLocation();

	/**
	* \brief 控件是否初始化
	* 
	* 初始化各个控件(Static，ListBox)
	* 
	* \return true if 初始化
	*/
	bool bControlInit() const 
	{
		return (m_pChangeHairStyleList && 
			m_pChangeHairColorList);
	}

	void OnMouseMoveList(UINT nID);

private:	
	///发型列表
	CGuiListBox *m_pChangeHairStyleList;

	///发色列表
	CGuiListBox *m_pChangeHairColorList;

	///主角
	CMainCharacter* m_pMainChar;
};

