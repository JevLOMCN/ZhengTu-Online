#ifndef _GUI_SELECT_SEPTDARE_TYPE_DLG
#define _GUI_SELECT_SEPTDARE_TYPE_DLG

/**
* \brief 重生对话框
* 
* 主角死亡后通过本对话框选择重生方式
* 
*/
class CGuiSelectSeptDareTypeDlg :
	public CGuiDialog
{
public:
	stActiveDareCmd m_cmdSeptDare;
public:
	CGuiSelectSeptDareTypeDlg(void);
	~CGuiSelectSeptDareTypeDlg();

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
};

#endif