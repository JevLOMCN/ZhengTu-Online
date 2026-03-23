#pragma once
#include "..\include\guidialog.h"


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiMenu :
	public CGuiDialog
{
	CGuiDialog* m_pOwner;
public:
	enum enumMenuStyle{
		MenuStyle_Separate,
		MenuStyle_Command,
		MenuStyle_Popup,
	};
private:
	struct stMenuItem{
		DWORD id;
		enumMenuStyle style;
		bool enabled;
		bool checked;
		tString<64> text;
		stRectI rect;
		bool useCustomerElement;

		stMenuItem()
		{
			useCustomerElement = false;
		}
	};
	std::vector<stMenuItem> m_aItem;

	static CGuiElement s_aBorderElement[9];
	static CGuiElement s_aItemElement[9];
	static CGuiElement s_aItemSelectElement[9];
	static CGuiElement s_aSpaceElement[9];
	static CGuiElement s_aTextElement[4];
	static CGuiElement s_aCheckElement[9];
public:
	CGuiMenu(void);
	~CGuiMenu(void);

	void SetOwner(CGuiDialog* pOwner){ m_pOwner = pOwner;}
	CGuiDialog* GetOwner(){ return m_pOwner;}

	void AddMenuItem(int cmd,const char* pszText,enumMenuStyle style = MenuStyle_Command,bool bEnabled = true);
	void SetMenuItem(int index,int cmd,const char* pszText,enumMenuStyle style = MenuStyle_Command,bool bEnabled = true);
	void SetMenuItemUseCustomeElement(int index, CGuiElement& customerElement);
	int GetMenuIndex(int cmd);
	void SetMenuEnabled(int index,bool bEnabled);
	void SetMenuItemChecked(int index,bool bChecked);	//只设置index为bChecked
	void SetMenuItemChecked(int index);					//设置index为Checked同时，取消其他的Checked
	bool SetMenuItemEnbale(int cmd,bool bEnable);
	bool SetMenuItemCheck(int cmd,bool bCheck);			//只设置cmd为bChecked
	void SetMenuItemCheck(int cmd);						//设置cmd为Checked同时，取消其他的Checked

	void ClearAllCheck();
	void SetAllCheck();
	void UpdateRects();
	void OnCreate();
	HRESULT OnRender(float fElapsedTime);
	void RenderItem(int index);
	void BeginPopup();
	void Popup();
	static void InitDefaultElements();
	bool MsgProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};
