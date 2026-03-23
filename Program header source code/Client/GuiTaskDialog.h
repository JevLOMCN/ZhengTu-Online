/**
 *\file		GuiTaskDialog.h
 *\version	$ID$
 *\author	$$$04公司
 *			$$$04公司@###.com
 *\date		2006-7-19  21:11:00
 * \brief 任务对话框
 * 
 * 任务对话框：显示任务、任务描述、任务难度
 * 
 */

#pragma once
#include "guinpcdialog.h"
#include "maincharacter.h"

/**
 * \brief 任务对话框
 * 
 * 任务对话框：显示任务、任务描述、任务难度
 * 
 */
class CGuiTaskDialog : public CGuiDialog
{
public:
	CGuiTaskDialog();
	void	OnClose();
	void	OnCreate();
	bool	ParseTaskMessage(stNullUserCmd* pCmd,size_t size);
	bool	OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void	OnSetCloseLocation();
	HRESULT OnRender(float fElapsedTime);

	void	UpdateTaskContent();
	static void	RequestCmd(std::vector<std::string> & params);
	void	LookupTaskInfo();
	void	ShowLastTask(DWORD id);

	void	UpdateTaskKing(const char* text);
private:
	inline CTask* GetTaskList();
	void SetLevel(int iLevel,bool bMainTask = true);
	inline int bIsTaskIdShown(DWORD dwTaskID,CGuiListBox* list);

	void   ShowTaskText(DWORD dwTaskID,CGuiMLTextBox* ml);
	DWORD  GetSelectTaskID(CGuiListBox* list);
	bool   CancelTask();

	bool	OnBtnClick(int nID);
	bool	OnListBoxSelChange(int nID);

	inline void   ShowMainTaskStars(bool bHard,bool bShow,int num = 5,int lv=0);
	inline void   ShowSwitchTaskStars(bool bHard,bool bShow,int num = 5,int lv=0);
	
private:
	CGuiTab*	m_guiTab;
	/*  主线任务 */
	///任务难度(ease stars)
	CGuiButton *m_MainTaskEasyStars[5];
	///任务难度(hard stars)
	CGuiButton *m_MainTaskHardStars[5];
	///任务列表
	CGuiListBox	 *	m_pMainTaskList;
	///任务描述显示
	CGuiMLTextBox *	m_mlMainTaskTextBox;

	/*  支线任务 */
	///任务难度(ease stars)
	CGuiButton *m_SwitchTaskEasyStars[5];
	///任务难度(hard stars)
	CGuiButton *m_SwitchTaskHardStars[5];
	///任务列表
	CGuiListBox	 *	m_pSwitchTaskList;
	///任务描述显示
	CGuiMLTextBox *	m_mlSwitchTaskTextBox;

	//检索任务王
	CGuiMLTextBox *	m_mlTaskKingTextBox;

	DWORD m_dwCurShownTask;
};
