#ifndef _GUI_SELECT_MARRY_TYPE_DLG
#define _GUI_SELECT_MARRY_TYPE_DLG

/**
* \brief 重生对话框
* 
* 主角死亡后通过本对话框选择重生方式
* 
*/
class CGuiSelectMarryTypeDlg :
	public CGuiDialog
{	
public:
	CGuiSelectMarryTypeDlg(void);
	~CGuiSelectMarryTypeDlg();

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
};

#endif