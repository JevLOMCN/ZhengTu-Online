/**
 *\file		GuiReliveDlg.h
 *\version	$ID$
 *\author	$$$04公司
 *			$$$04公司@###.com
 *\date		2005-07-06 11:32:00
 *\brief	重生对话框
 *
 *主角死亡后通过本对话框选择重生方式
 *
 */

#ifndef _GUI_RELIVE_DLG
#define _GUI_RELIVE_DLG

/**
 * \brief 重生对话框
 * 
 * 主角死亡后通过本对话框选择重生方式
 * 
 */
class CGuiReliveDlg :
	public CGuiDialog
{	
public:
	CGuiReliveDlg();
	CGuiReliveDlg(DWORD dwID,DWORD dwDeadType = 0);
	~CGuiReliveDlg();

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
private:
	int	 GetReliveMoney();
	int	 GetReliveMaxey(); //soke 原地满状态复活
	///主角的临时id
	DWORD m_dwUserTempID;
	DWORD m_dwDeadType;
};
#endif