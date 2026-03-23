/**
 
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
class CGuiHeadListDialog :
	public CGuiDialog
{
public:
	CGuiHeadListDialog();
	~CGuiHeadListDialog();

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	bool OnOK();
	void OnClose();
	void OnSetCloseLocation();
	void OnMouseMoveList(UINT nID);

private:	
	// 发型列表
	CGuiListBox *m_pHeadList;
        CGuiImage * m_pDemoHead;
	// 主角
	CMainCharacter* m_pMainChar;

	void ChangeFace(DWORD dwHeadID);
};

