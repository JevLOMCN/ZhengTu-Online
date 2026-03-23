/**
*\file		GuiItem.h
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    包裹对话框
*
* 
*
*/

#pragma once
#include "..\gui\include\guidialog.h"

#include <map>
//class CGuiEquip;
/**
 * \brief 包裹对话框
 * 
 * 包裹对话框
 * 
 */
class CGuiItem :
	public CGuiDialog
{
public:
	DWORD m_nMoney;
	DWORD m_nTicket;

	bool m_bWithStore;
	//CGuiEquip* m_pEquip;
	///显示玩家信息
	bool m_bShowEquip;

	///包裹1
	CGuiTable* m_pTableLeft;
	///包裹2
	CGuiTable* m_pTableRight;

	CGuiItem(void);
	void OnClose(void);
	void OnCreate();
	
	///包裹网格
	CGuiTable* m_pTable;
	//CGuiImage* m_pImage;
	
	CGuiEditBox* m_pGoldBox;

    //myy 积分显示框
	CGuiEditBox* m_pTicketBox;

	///银子显示框
	CGuiEditBox* m_pMoneyBox;

	class ColorMoneyBox
	{
		DWORD			m_nMoney;
		stRectI			m_rect;
		std::string		m_tail;
		int				m_iFont;

		CGuiStatic*		m_pEditBoxMoneyD;
		CGuiStatic*		m_pEditBoxMoneyN;
		CGuiStatic*		m_pEditBoxMoneyW;
		CGuiStatic*		m_pStatic_MoneyD;
		CGuiStatic*		m_pStatic_MoneyN;
		CGuiStatic*		m_pStatic_MoneyW;

		CGuiStatic*		m_pStaticTail;

	public:
		void Create(stRectI rect, CGuiDialog* pDialog);
		void SetValue(DWORD nMoney, std::string tail = "");
		DWORD GetValue() { return m_nMoney; }
	};
   //积分
   class ColorTicketBox
	{
		DWORD			m_nTicket;
		stRectI			m_rect;
		std::string		m_tail;
		int				m_iFont;

       
		CGuiStatic*		m_pEditBoxTicketD;
		CGuiStatic*		m_pStatic_TicketD;
		CGuiStatic*		m_pStaticName_TicketD;
    	CGuiStatic*		m_pStaticTail;

	public:
		void Create(stRectI rect, CGuiDialog* pDialog);
		void SetValue(DWORD nTicket, std::string tail = "");
		DWORD GetValue() { return m_nTicket; }
	};
   ///////////////////////////////////////////////////
    //充值点  
/*	
    class ColorChongzhiBox
	{
		DWORD			m_nChongzhi;
		stRectI			m_rect;
		std::string		m_tail;
		int				m_iFont;

       
		CGuiStatic*		m_pEditBoxChongzhiD;
		CGuiStatic*		m_pStatic_ChongzhiD;
		CGuiStatic*		m_pStaticName_ChongzhiD;
    	CGuiStatic*		m_pStaticTail;

	public:
		void Create(stRectI rect, CGuiDialog* pDialog);
		void SetValue(DWORD nChongzhi, std::string tail = "");
		DWORD GetValue() { return m_nChongzhi; }
	};
	*/
   ///////////////////////////////////////////////////  
	ColorMoneyBox	    m_ColorGoldBox;
	ColorMoneyBox	    m_ColorMoneyBox;
	ColorTicketBox	    m_ColorTicketBox;  //积分
   //ColorChongzhiBox   	m_ColorChongzhiBox; //充值点

	void SetCharType(int type);

	CGuiTable* & GetEquipTable(int equip_type,int x = 0);

	void SetMoney(DWORD nMoney);
	void SetGoldSalary(DWORD dwGold,DWORD dwSalary); //工资金子
	void SetTicket(DWORD dwTicket); //积分
    //void SetChongzhi(DWORD dwChongzhi); //充值点1
	DWORD GetMoney();
	DWORD GetTicket();
    //DWORD GetChongzhi(); //充值点1
	void OnInitScript();

	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void SetCharName(const char* name);
	void OnMoveMoney();

	void OnSetCloseLocation();

	HRESULT OnRender(float fElapsedTime);

	void bind_lua(lua_State* L);

	//soke 一键打开包裹
	//bool m_bShowAllBag;
};