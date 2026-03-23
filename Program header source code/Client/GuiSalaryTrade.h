/**
*\file		GuiSalaryTrade.h
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    NPC交易对话框
*
* 
*
*/

#pragma once
#include "..\gui\include\guidialog.h"

/**
 * \brief NPC交易对话框
 * 
 * NPC交易对话框
 * 
 */
struct stSalaryBuyObject
{
	ObjectBase_t*  pObjectBase;
	int           nNum;
	//soke  商店材料等级
	int           nLevel;

	stSalaryBuyObject()
	{
		pObjectBase = NULL;
		nNum = 1;
	}
};

class CGuiSalaryTrade :
	public CGuiDialog
{
	///买物品数据
	std::vector<stSalaryBuyObject> m_aBuyObject;  
	///卖物品数据
	std::vector<stSalaryBuyObject> m_aSellObject;
	///修理物品数据
	std::vector<stSalaryBuyObject> m_aRepairObject;
	///是否可修理
	DWORD m_dwRepair;
	///多选页
	CGuiTab*                 m_pTab;
	///多选页标题
	std::vector<std::string>   m_strPageInfo;
public:
	///标题框控件
	CGuiStatic* m_pLabel;
	CGuiStatic*	 m_pStaMoney;
	///工资显示框
	CGuiEditBox* m2_pStaMoney;

	///买物品显示网格
	CGuiTable*	m_pTable;
	CGuiSalaryTrade(void);
	void OnClose(void);
	void OnCreate();
	HRESULT OnRender(float fElapsedTime);
	void SetParam(std::vector<std::string> & params);
	void AddBuyObject(ObjectBase_t* pObject,int nNum,int nLevel);
	DWORD IsRepair(){ return m_dwRepair;}
	void SetRepair(DWORD dwRepair){ m_dwRepair = dwRepair;}
	bool IsBuyObject(DWORD dwObjectID);
	bool IsSellObject(DWORD dwObjectID);
	bool IsRepairObject(DWORD dwObjectID);
	void AddBuyPage(const char* pageName,const char* pszBuyParam);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void RefreshRepairMoney();
	void SetPage(int nPage);
};
