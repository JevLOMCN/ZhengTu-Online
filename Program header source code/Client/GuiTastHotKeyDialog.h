
#pragma once

#include "..\gui\include\guidialog.h"

/**
 * \brief 任务对话框
 * 
 * 任务对话框：显示任务、任务描述、任务难度
 * 
 */
class CGuiTastHotKeyDialog : public CGuiDialog
{
	float m_Ttime;
	float m_Ltime;
	IBitmaps*	m_pHotKeyBmp;
	int  IsHotKey;
public:
	void OnCreate();
	HRESULT OnRender(float fElapsedTime);
	CGuiTastHotKeyDialog();
	void SetVisibleTime(float t);
	void SetVisibleHotKey(int b);
};
