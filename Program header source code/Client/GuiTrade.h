/**
*\file		GuiTrade.h
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    玩家交易界面对话框
*
* 
*
*/

#pragma once
#include "..\gui\include\guidialog.h"

struct stBeginTradeUserCmd;

/**
 * \brief 玩家交易界面对话框
 * 
 * 玩家交易界面对话框
 * 
 */
class CGuiTrade :
	public CGuiDialog
{
	///交易存放的临时ID
	DWORD m_dwOtherID;

	std::lookaside_allocator<CRoleItem> m_allocRoleItem;
	std::vector<CRoleItem*> m_listItem;
	


public:
	///交易存放道具的网格
	CGuiTable* m_pTables[2];
	///交易存放银子的编辑框
	CGuiEditBox* m_pEdtMoney[2];
	///交易选择银子的按钮
	CGuiButton*	 m_pBtnMoney[2];
	///控件按钮
	CGuiButton*	 m_pBtnCommit[2];
	///是否显示交易对话框
	bool		 m_bShowOther;
	///交易提示控件
	CGuiStatic*	 m_pTargetName;
	///设置控件位置
	int			 m_nDy;

	bool        m_bCommit;

	CGuiTrade(void);
	
	void OnClose(void);
	void OnCreate();
	void InitData();

	CGuiTable* & GetItemTable(int ID)
	{
		int ix = ID - OBJECTCELLTYPE_TRADE;
		Assert(ix < 2);
		return m_pTables[ix];
	}

	CGuiEditBox* & GetMoneyEdit(int ID)
	{
		int ix = ID - OBJECTCELLTYPE_TRADE;
		Assert(ix < 2);
		return m_pEdtMoney[ix];
	}

	CGuiButton* & GetMoneyButton(int ID)
	{
		int ix = ID - OBJECTCELLTYPE_TRADE;
		Assert(ix < 2);
		return m_pBtnMoney[ix];
	}

	CGuiButton* & GetCommitButton(int ID)
	{
		int ix = ID - OBJECTCELLTYPE_TRADE;
		Assert(ix < 2);
		return m_pBtnCommit[ix];
	}

	void BeginTrade(stBeginTradeUserCmd* pCmd);
	void OtherCommitTrade();
	void CancelTrade();
	void FinalTrade();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnItemChange();

	void SetMoney(DWORD dwLocation,DWORD dwMoney);
	DWORD GetMoney(DWORD dwLocation);
	void OnInitScript();

	void InitIcon();
	void OnSetCloseLocation();

	void InsertObject(t_Object* pObject);
	void DeleteObject(DWORD thisID);

	bool IsEdit();
};

extern void AddTrade(const char* szName);