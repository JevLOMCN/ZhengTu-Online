
#pragma once
//#include "GuiDialog.h"
//#include "maincharacter.h"
#include "..\gui\include\guidialog.h"

/**
 * \brief 任务对话框
 * 
 * 任务对话框：显示任务、任务描述、任务难度
 * 
 */
class CGuiTastCueDialog : public CGuiDialog
{
	float m_Ttime;
	float m_Ltime;
	IBitmaps*	m_pHotKeyBmp;
	int  IsHotKey;
	int  m_Alpha;
public:
	void OnCreate();
	HRESULT OnRender(float fElapsedTime);
	CGuiTastCueDialog();
	void SetVisibleTime(float t);
	void SetVisibleHotKey(int b);
};
