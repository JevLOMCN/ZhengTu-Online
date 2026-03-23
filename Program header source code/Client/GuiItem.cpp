/**
 *\file		GuiItem.cpp
 *\version	$Id$
 *\author	$$$01公司
 *\			$$$01公司@###.com
 *\date		2006-7-19  21:11:00 中国标准时间
 *\brief	    包裹对话框
 *
 *
 *
 */

#include "public.h"
#include "./GameGuiManager.h"
#include ".\guiitem.h"
#include "./table.h"
#include "./RoleItem.h"
#include "./GuiMoneyInputBox.h"
#include "./GuiTrade.h"
#include "./GameState.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiEquip.h"
#include "./GuiUserInfo.h"
#include "./GuiNpcMake.h"
#include "../luabind/public.h"
#include "./GuiNpcTrade.h"
#include "GuiMain.h"
#include ".\guifloatcontrol.h"
#include "./GuiPetDialog.h"
#include "./GuiShopDialog.h"
#include "./DlgEquiphandbook.h" //soke 百兽图鉴

#define IDC_SHOWEQUIP 5
#define IDC_HIDEEQUIP 6
#define IDC_SHUISHEN 8
#define IDC_CONNIMAGE 6
#define IDC_CONGNYJZL 308

// soke 包裹一键整理
bool SortItem(CRoleItem *p, CRoleItem *p2)
{
	return (p->GetPrice(ITEM_PRICE_SELL) > p2->GetPrice(ITEM_PRICE_SELL));
}

void CGuiItem::bind_lua(lua_State *L)
{
	using namespace luabind;
	CGuiDialog::bind_lua(L);
	module(L)
		[class_<CGuiItem, CGuiDialog>("CGuiItem")
			 .def("OnMoveMoney", CGuiItem::OnMoveMoney)];
	m_lua->SetGlobal("this", this);
}

/**
 * \brief 构造函数
 *
 * 构造函数
 *
 * \return 无
 */
CGuiItem::CGuiItem(void)
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;
	//	m_pEquip = NULL;
	m_bShowEquip = false;
	m_clientOffsetTopLeft = stPointI(0, 0);
	m_clientOffsetRightBottom = stPointI(0, 0);

	FUNCTION_END;
}

// soke 价格 锭 两 文 重定义颜色显示
void CGuiItem::ColorMoneyBox::Create(stRectI rect, CGuiDialog *pDialog)
{
	m_rect = rect;

	m_iFont = 1;

	int iFont = GetDevice()->SetFont(m_iFont);
	SIZE textSize = GetDevice()->GetStringSize(INGOT_TXT, -1, 20);
	GetDevice()->SetFont(iFont);

	auto H = rect.Height();
	auto X = rect.left;
	auto Y = rect.top + (H - textSize.cy) / 2;

	pDialog->AddStatic(pDialog->GenerateControlID(), NULL, X, Y, 100, H, false, &m_pEditBoxMoneyD);
	pDialog->AddStatic(pDialog->GenerateControlID(), NULL, X, Y, 100, H, false, &m_pStatic_MoneyD);

	pDialog->AddStatic(pDialog->GenerateControlID(), NULL, X, Y, 100, H, false, &m_pEditBoxMoneyN);
	pDialog->AddStatic(pDialog->GenerateControlID(), NULL, X, Y, 100, H, false, &m_pStatic_MoneyN);

	pDialog->AddStatic(pDialog->GenerateControlID(), NULL, X, Y, 100, H, false, &m_pEditBoxMoneyW);
	pDialog->AddStatic(pDialog->GenerateControlID(), NULL, X, Y, 100, H, false, &m_pStatic_MoneyW);

	pDialog->AddStatic(pDialog->GenerateControlID(), NULL, X, Y, 100, H, false, &m_pStaticTail);

	// m_pEditBoxMoneyD->SetEnabled(false);
	// m_pEditBoxMoneyN->SetEnabled(false);
	// m_pEditBoxMoneyW->SetEnabled(false);
	m_pStatic_MoneyD->SetTextColor(COLOR_ARGB(255, 0, 255, 137));
	m_pStatic_MoneyN->SetTextColor(COLOR_ARGB(255, 255, 222, 3));

	m_pStatic_MoneyD->SetText(INGOT_TXT);
	m_pStatic_MoneyN->SetText(TAEL_TXT);
	m_pStatic_MoneyW->SetText(CENT_TXT);

	CGuiControl *controls[] = {
		m_pEditBoxMoneyD, m_pStatic_MoneyD,
		m_pEditBoxMoneyN, m_pStatic_MoneyN,
		m_pEditBoxMoneyW, m_pStatic_MoneyW,
		m_pStaticTail};
	for (int i = 0; i < count_of(controls); ++i)
		controls[i]->SetFont(m_iFont);

	m_nMoney = 0;
	m_tail = "";
	SetValue(m_nMoney, m_tail);
}

void CGuiItem::ColorMoneyBox::SetValue(DWORD nMoney, std::string tail)
{

	m_nMoney = nMoney;
	m_tail = tail;
	m_pStaticTail->SetText(m_tail.c_str());

	DWORD ingot = GETMONEYINGOT(m_nMoney);
	DWORD tael = GETMONEYTAEL(m_nMoney);
	DWORD cent = GETMONEYCENT(m_nMoney);

	char buf[20];
	sprintf(buf, "%u", ingot);
	m_pEditBoxMoneyD->SetText(buf);

	sprintf(buf, "%u", tael);
	m_pEditBoxMoneyN->SetText(buf);

	sprintf(buf, "%u", cent);
	m_pEditBoxMoneyW->SetText(buf);

	m_pEditBoxMoneyD->SetVisible(ingot != 0);
	m_pStatic_MoneyD->SetVisible(ingot != 0);
	m_pEditBoxMoneyN->SetVisible(tael != 0);
	m_pStatic_MoneyN->SetVisible(tael != 0);
	m_pEditBoxMoneyW->SetVisible(cent != 0 || nMoney == 0);
	m_pStatic_MoneyW->SetVisible(cent != 0 || nMoney == 0);

	CGuiControl *controls[] = {
		m_pEditBoxMoneyD, m_pStatic_MoneyD,
		m_pEditBoxMoneyN, m_pStatic_MoneyN,
		m_pEditBoxMoneyW, m_pStatic_MoneyW,
		m_pStaticTail};

	int iFont = GetDevice()->SetFont(m_iFont);
	int x = m_pEditBoxMoneyD->GetX();
	for (int i = 0; i < count_of(controls); ++i)
	{
		if (!controls[i]->IsVisible())
			continue;

		stRectI rect = controls[i]->GetBoundingBox();

		controls[i]->SetLocation(x, rect.top);
		controls[i]->SetSize(GetDevice()->GetStringSize(controls[i]->GetText(), -1, 100).cx, rect.Height());

		x += controls[i]->GetWidth();
	}
	GetDevice()->SetFont(iFont);
}

//soke 积分价格点 重定义颜色显示包袱
void CGuiItem::ColorTicketBox::Create(stRectI rect, CGuiDialog* pDialog)
{
	m_rect = rect;

	m_iFont = 1;

	int iFont = GetDevice()->SetFont(m_iFont);
	SIZE textSize = GetDevice()->GetStringSize(TICKET_TXT, -1, 20);
	GetDevice()->SetFont(iFont);

	auto H = rect.Height();
	auto X = rect.left;
	auto Y = rect.top + (H - textSize.cy) / 2;

	pDialog->AddStatic(pDialog->GenerateControlID(), NULL, X, Y, 150, H, false, &m_pEditBoxTicketD);
	pDialog->AddStatic(pDialog->GenerateControlID(), NULL, X, Y, 150, H, false, &m_pStatic_TicketD);
	pDialog->AddStatic(pDialog->GenerateControlID(), NULL, X, Y, 150, H, false, &m_pStaticName_TicketD);

	pDialog->AddStatic(pDialog->GenerateControlID(), NULL, X, Y, 150, H, false, &m_pStaticTail);

	//m_pEditBoxMoneyD->SetEnabled(false);
	//m_pEditBoxMoneyN->SetEnabled(false);
	//m_pEditBoxMoneyW->SetEnabled(false);
	m_pStaticName_TicketD->SetTextColor(COLOR_ARGB(255, 255, 222, 3)); 
	m_pStatic_TicketD->SetTextColor(COLOR_ARGB(255, 255, 222, 3)); 
   
	m_pStatic_TicketD->SetText(TICKET_TXT);
	m_pStaticName_TicketD->SetText(TICKETNAME_TXT);

	CGuiControl* controls[] = {
		m_pEditBoxTicketD,	m_pStatic_TicketD, m_pStaticName_TicketD,
		m_pStaticTail
	};
	for (int i = 0; i < count_of(controls); ++i)
		controls[i]->SetFont(m_iFont);

	m_nTicket = 0;
	m_tail = "";
	SetValue(m_nTicket, m_tail);
}


void CGuiItem::ColorTicketBox::SetValue(DWORD nTicket, std::string tail)
{

	m_nTicket = nTicket;
	m_tail = tail;
	m_pStaticTail->SetText(m_tail.c_str());

	DWORD ingot = GETTICKETENT(m_nTicket);

	char buf[20];
	sprintf(buf, "%u", ingot);
	m_pEditBoxTicketD->SetText(buf);

    
	m_pEditBoxTicketD->SetVisible(ingot != 0 || nTicket == 0);
	m_pStatic_TicketD->SetVisible(ingot != 0 || nTicket == 0);
	m_pStaticName_TicketD->SetVisible(ingot != 0 || nTicket == 0);


	CGuiControl* controls[] = {
		m_pEditBoxTicketD, m_pStatic_TicketD, m_pStaticName_TicketD, 
		m_pStaticTail
	};

	int iFont = GetDevice()->SetFont(m_iFont);
	int x = m_pEditBoxTicketD->GetX();
	for (int i = 0; i < count_of(controls); ++i)
	{
		if(!controls[i]->IsVisible()) continue;

		stRectI rect = controls[i]->GetBoundingBox();

		controls[i]->SetLocation(x, rect.top);
		controls[i]->SetSize(GetDevice()->GetStringSize(controls[i]->GetText(), -1, 200).cx, rect.Height());

		x += controls[i]->GetWidth();
	}
	GetDevice()->SetFont(iFont);
}
///////////////////////////////////////////////////////////////////////
//充值点
/*
void CGuiItem::ColorChongzhiBox::SetValue(DWORD nChongzhi, std::string tail)
{

	m_nChongzhi = nChongzhi;
	m_tail = tail;
	m_pStaticTail->SetText(m_tail.c_str());

	DWORD ingot = GETCHONGZHIENT(m_nChongzhi);

	char buf[20];
	sprintf(buf, "%u", ingot);
	m_pEditBoxChongzhiD->SetText(buf);

    
	m_pEditBoxChongzhiD->SetVisible(ingot != 0 || nChongzhi == 0);
	m_pStatic_ChongzhiD->SetVisible(ingot != 0 || nChongzhi == 0);
	m_pStaticName_ChongzhiD->SetVisible(ingot != 0 || nChongzhi == 0);


	CGuiControl* controls[] = {
		m_pEditBoxChongzhiD, m_pStatic_ChongzhiD, m_pStaticName_ChongzhiD, 
		m_pStaticTail
	};

	int iFont = GetDevice()->SetFont(m_iFont);
	int x = m_pEditBoxChongzhiD->GetX();
	for (int i = 0; i < count_of(controls); ++i)
	{
		if(!controls[i]->IsVisible()) continue;

		stRectI rect = controls[i]->GetBoundingBox();

		controls[i]->SetLocation(x, rect.top);
		controls[i]->SetSize(GetDevice()->GetStringSize(controls[i]->GetText(), -1, 200).cx, rect.Height());

		x += controls[i]->GetWidth();
	}
	GetDevice()->SetFont(iFont);
}*/
//////////////////////////////////////////////////////////////////////////////
/**
 * \brief 创建对话框
 *
 * 创建对话框
 *
 * \return 无
 */
void CGuiItem::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	SetControlEnabled(GUI_CONTROL_EDITBOX, false);

	//	m_pImage = GetImage(1);
	m_pTable = GetTable(2);
	m_pTable->m_iTableType = OBJECTCELLTYPE_COMMON;
	m_pTable->m_iTableID = 0;

	m_pMoneyBox = GetEditBox(11);
	//	m_pMoneyBox = GetEditBox(3); //soke 银子
	m_pMoneyBox->SetEnabled(false);

	m_pGoldBox = GetEditBox(20); // soke 金子
	m_pGoldBox->SetEnabled(false);

	m_pTicketBox = GetEditBox(30); //soke 积分
	m_pTicketBox->SetEnabled(false);

	m_ColorGoldBox.Create(m_pGoldBox->GetBoundingBox(), this);
	m_ColorMoneyBox.Create(m_pMoneyBox->GetBoundingBox(), this);
	m_ColorTicketBox.Create(m_pTicketBox->GetBoundingBox(), this);  //积分显示包袱页面

	GetButton(IDC_HIDEEQUIP)->SetVisible(false);
	GetButton(IDC_SHOWEQUIP)->SetVisible(false);
	GetButton(IDC_SHUISHEN)->SetVisible(true);
	GetButton(100)->SetVisible(true);
	//	m_pMoneyBox->SetVisible(false);
	//	GetEditBox(11)->SetEnabled(false);
	//  GetButton(12)->SetEnabled(false);
	m_bShowEquip = false;

	m_pTableLeft = GetTable(300);
	m_pTableLeft->m_iTableType = OBJECTCELLTYPE_EQUIP;
	m_pTableLeft->m_EquipPosition = stPointI(EQUIPCELLTYPE_LEFT, EQUIPCELLTYPE_PACKAGE);

	m_pTableRight = GetTable(301);
	m_pTableRight->m_iTableType = OBJECTCELLTYPE_EQUIP;
	m_pTableRight->m_EquipPosition = stPointI(EQUIPCELLTYPE_RIGHT, EQUIPCELLTYPE_PACKAGE);
	// m_pImage = GetImage(7);
	// m_pImage->SetVisible(false);

	FUNCTION_END;
}

/**
 * \brief 设置关闭按钮
 *
 * 设置关闭按钮
 *
 * \return 无
 */
void CGuiItem::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if (m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x - 64, pt.y - 2);
	}

	/**
	 * \brief 关闭对话框
	 *
	 * 关闭对话框
	 *
	 * \return 无
	 */
	FUNCTION_END;
}
void CGuiItem::OnClose(void)
{
	FUNCTION_BEGIN;
	//	CGuiDialog::OnClose();
	GetGameGuiManager()->m_guiItem = NULL;

	FUNCTION_END;
}

/**
 * \brief 设置玩家角色
 *
 * 设置玩家角色
 *
 * \param type : 角色类型
 * \return 无
 */
void CGuiItem::SetCharType(int type)
{
	FUNCTION_BEGIN;

	if (GetGameGuiManager()->m_guiUserInfo)
		GetGameGuiManager()->m_guiUserInfo->SetCharType(type);

	FUNCTION_END;
}

/**
 * \brief 设置银子
 *
 * 设置银子
 *
 * \param dwMoney : 银子数
 * \return 无
 */
void CGuiItem::SetMoney(DWORD nMoney)
{
	FUNCTION_BEGIN;

	m_nMoney = nMoney;

	m_ColorMoneyBox.SetValue(nMoney);
	//	m_ColorMoneyBox.SetValue(nMoney, "");

	FUNCTION_END;
}

void CGuiItem::SetGoldSalary(DWORD dwGold, DWORD dwSalary)
{
	m_ColorGoldBox.SetValue(dwGold);
	//	m_ColorGoldBox.SetValue(dwGold, "");
}

//充值点
/*
void CGuiItem::SetChongzhi(DWORD dwChongzhi)
{
	m_ColorChongzhiBox.SetValue(dwChongzhi);
	//	m_ColorGoldBox.SetValue(dwChongzhi, "");
}
*/
void CGuiItem::SetTicket(DWORD dwTicket)
{
	m_ColorTicketBox.SetValue(dwTicket);
	//	 m_ColorTicketBox.SetValue(dwTicket, "");
}

/**
 * \brief 得到包裹银子数
 *
 * 得到包裹银子数
 *
 * \return 无
 */
DWORD CGuiItem::GetMoney()
{
	FUNCTION_BEGIN;

	return m_nMoney;
	//	return String2Money(m_pMoneyBox->GetText());

	FUNCTION_END;
}

/**
 * \brief 点击银子按钮事件
 *
 * 点击银子按钮事件
 *
 * \return 无
 */
void CGuiItem::OnMoveMoney()
{
	FUNCTION_BEGIN;

	if (g_pMoneyInputBox)
		return;

	const char *pText;
	DWORD dwLocationDst;
	switch (GetGameState()->GetStateItem())
	{
	case eGameStateItem_TradeRun:
		pText = "要交易多少银子？";
		dwLocationDst = OBJECTCELLTYPE_TRADE;
		break;
	default:
		return;
		pText = "要扔下多少银子？";
		dwLocationDst = OBJECTCELLTYPE_NONE;
		break;
	}

	static stSplitItemData st;

	st.qwThisID = 0;						  // pSrcItem->GetThisID();
	st.dwCurNum = m_ColorMoneyBox.GetValue(); // String2Money(m_pMoneyBox->GetText());
	//	st.dwCurNum = String2Money(GetEditBox(11)->GetText());
	st.dst.dwLocation = dwLocationDst;
	st.dst.dwTableID = 0;
	st.dst.x = -1;
	st.dst.y = -1;

	CGuiMoneyInputBox *pBox = GetGameGuiManager()->AddMoneyInputBox(pText, eAction_SplitItem, &st);
	// pBox->SetNumber(0,pSrcItem->GetCount());
	CRoleItem::s_bCanMoveItem = false;
	g_pMoneyInputBox = (void *)pBox;

	FUNCTION_END;
}

/**
 * \brief 设置脚本函数
 *
 * 设置脚本函数
 *
 * \return 无
 */
void CGuiItem::OnInitScript()
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}

/**
 * \brief 对话框消息处理
 *
 * 对话框消息处理
 *
 * \param nEvent : 消息类型
 * \param nID : 控件ID
 * \param pControl : 控件指针
 * \return 消息返回值
 */
bool CGuiItem::OnGuiEvent(UINT nEvent, UINT nID, CGuiControl *pControl)
{
	FUNCTION_BEGIN;

	switch (nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		switch (nID)
		{
			// soke 一键打开所有包裹
			/*	case 303:
					{
						m_bShowAllBag = ! m_bShowAllBag;

						GetButton( 303 )->SetChecked( ! m_bShowAllBag );
					}
					break;*/
		case 10:
		{
			SetVisible(false);
		}
		break;
		case 365://随身改造
		{
			pControl->SetVisible(false);
			GetButton(365)->SetVisible(true);
			{
				stVisitNpcTradeUserCmd cmd;
				cmd.dwNpcTempID = 100000004; // 65535为随身商店的特殊ID`用于服务端特殊处理
				SEND_USER_CMD(cmd);
			}
			m_bShowEquip = false;
		}
		break;
		case 364://随身打造
		{
			pControl->SetVisible(false);
			GetButton(364)->SetVisible(true);
			{
				stVisitNpcTradeUserCmd cmd;
				cmd.dwNpcTempID = 100000003; // 65535为随身商店的特殊ID`用于服务端特殊处理
				SEND_USER_CMD(cmd);
			}
			m_bShowEquip = false;
		}
		break;
		case 363://随身仓库
		{
			pControl->SetVisible(false);
			GetButton(363)->SetVisible(true);
			{
				stVisitNpcTradeUserCmd cmd;
				cmd.dwNpcTempID = 100000002; // 65535为随身商店的特殊ID`用于服务端特殊处理
				SEND_USER_CMD(cmd);
			}
			m_bShowEquip = false;
		}
		break;
		case 362://装备转换
		{
			if (GetGameGuiManager()->m_guiZhuanhuan && ((CGuiDialog *)GetGameGuiManager()->m_guiZhuanhuan)->IsVisible())
			{
				((CGuiDialog *)GetGameGuiManager()->m_guiZhuanhuan)->SetVisible(false);
			}
			else
			{

				GetGameGuiManager()->AddZhuanhuan();
			}
		}
		break;
		case 361://回收系统
		{
			if (GetGameGuiManager()->m_guiHuishou && ((CGuiDialog *)GetGameGuiManager()->m_guiHuishou)->IsVisible())
			{
				((CGuiDialog *)GetGameGuiManager()->m_guiHuishou)->SetVisible(false);
			}
			else
			{

				GetGameGuiManager()->AddHuishou();
			}
		}
		break;
		case 360://自动使用
		{
			if (GetGameGuiManager()->m_guiAutoUse && ((CGuiDialog *)GetGameGuiManager()->m_guiAutoUse)->IsVisible())
			{
				((CGuiDialog *)GetGameGuiManager()->m_guiAutoUse)->SetVisible(false);
			}
			else
			{

				GetGameGuiManager()->AddAutoUse();
			}
		}
		break;
		// soke 包裹一键整理
		case IDC_CONGNYJZL:
		{
			CMainCharacter *p = GetScene()->GetMainCharacter();
			std::list<CRoleItem *> List2;
			std::list<CRoleItem *>::iterator it2;
			std::list<CRoleItem *> List3;

			for (std::vector<CRoleItem *>::iterator it = p->m_listItem.begin(); it != p->m_listItem.end(); ++it)
			{
				CRoleItem *pI = *it;

				// soke 包裹一键整理 不是装备格子、不是买卖格子、不是仓库格子、不是包裹1、2的格子(2.4.5.8.7.10.11.12.13.14.15.16.17.18.19)
				if (pI->GetLocation().dwLocation != 2 && pI->GetLocation().dwLocation != 4 && pI->GetLocation().dwLocation != 8 && pI->GetLocation().dwLocation != 5 && pI->GetLocation().dwLocation != 7 && pI->GetLocation().dwLocation != 10 && pI->GetLocation().dwLocation != 11 && pI->GetLocation().dwLocation != 12 && pI->GetLocation().dwLocation != 13 && pI->GetLocation().dwLocation != 14 && pI->GetLocation().dwLocation != 15 && pI->GetLocation().dwLocation != 16 && pI->GetLocation().dwLocation != 17 && pI->GetLocation().dwLocation != 18 && pI->GetLocation().dwLocation != 19 && pI->GetLocation().dwLocation != 20 && pI->GetLocation().dwLocation != 21 && strcmp(pI->GetName(), "银子") != 0)
				{
					if (pI->GetEquipType() > 0) // soke 装备位置 > 0
					{
						List2.push_back(pI);
					}
					else
					{
						List3.push_back(pI);
					}
				}

				p = p;
			}

			// Sort
			List2.sort(&SortItem);
			List3.sort(&SortItem);

			for (it2 = List3.begin(); it2 != List3.end(); ++it2)
			{
				List2.push_back(*it2);
			}

			int x = 0;
			int y = 0;
			stSwapAllObjectPropertyUserCmd cmd;
			int n = 5;
			stObjectLocation L;

			L.dwLocation = 1;
			L.dwTableID = 0;

			cmd.nNum = 0;

			for (it2 = List2.begin(); it2 != List2.end(); ++it2)
			{
				L.x = x;
				L.y = y;

				cmd.qwThisID[cmd.nNum] = (*it2)->GetThisID();
				cmd.dst[cmd.nNum] = L;

				x += 1;

				if (x > 6)
				{
					x = 0;
					y += 1;
				}

				cmd.nNum += 1;
			}

			if (cmd.nNum > 0 && cmd.nNum <= 99)
			{
				SEND_USER_CMD(cmd);
			}
		}
		break;
		case 309: // 随身摆摊
		{
			CMainCharacter *pMainRole = GetScene()->GetMainCharacter();

			if (GetGameGuiManager()->m_guiShop && GetGameGuiManager()->m_guiShop->IsVisible())
			{
				GetGameGuiManager()->m_guiShop->SetVisible(false);
			}
			else if (pMainRole && pMainRole->bIsShopStart())
			{
				GetGameGuiManager()->AddShop();
			}
			else
			{
				CGuiShopDialog *pShopDlg = GetGameGuiManager()->AddShop();
				if (pShopDlg)
				{
					stRectI rcClient = pShopDlg->GetClipRect();
					int x = GetDevice()->GetWidth() / 2;
					int y = 100;
					pShopDlg->SetLocation(x, y);
				}

				GetGameGuiManager()->AddItem();

				if (GetGameGuiManager()->m_guiItem)
				{
					stRectI rcClient = GetGameGuiManager()->m_guiItem->GetClipRect();
					int x = GetDevice()->GetWidth() / 2 - rcClient.Width();
					int y = 100;
					GetGameGuiManager()->m_guiItem->SetLocation(x, y);
				}
			}
		}
		break;
			/*	case 13://包裹月光宝盒控件
					{
						if (GetGameGuiManager()->m_guiMakeTo1&&!GetGameGuiManager()->m_guiMakeTo1->IsVisible())
						{
							GetGameGuiManager()->m_guiMakeTo1->SetVisible(true);
							GetGameGuiManager()->m_guiMakeTo1->UpdateData(true);//wen 2010 9 10
						}
					}
					break;*/
		case IDC_SHOWEQUIP:
			pControl->SetVisible(false);
			GetButton(IDC_HIDEEQUIP)->SetVisible(true);
			{
				stGuiAnimationInfo ani;

				ani.dwColorDst = m_dwColor;
				ani.eAniComplete = GuiAniEvent_None;
				ani.fTime = 0.2f;
			}

			m_bShowEquip = true;
			return true;
		case IDC_HIDEEQUIP:
			pControl->SetVisible(false);
			GetButton(IDC_SHOWEQUIP)->SetVisible(true);
			{
				//	m_pEquip->SetColor(COLOR_ARGB(0,255,255,255));
				stGuiAnimationInfo ani;

				ani.dwColorDst = COLOR_ARGB(50, 255, 255, 255);
				ani.eAniComplete = GuiAniEvent_Hide;
				ani.fTime = 0.2f;
			}

		// m_pImage->SetVisible(false);
		// soke 随身商店
		case IDC_SHUISHEN:
			pControl->SetVisible(false);
			GetButton(IDC_SHUISHEN)->SetVisible(true);
			{
				/*CNpc*  pNpc = GetScene()->FindNpcByID( 103900 );
				if(pNpc)
					GetScene()->GetMainCharacter()->CallNpc(pNpc);*/

				stVisitNpcTradeUserCmd cmd;
				cmd.dwNpcTempID = 100000000; // 65535为随身商店的特殊ID`用于服务端特殊处理
				SEND_USER_CMD(cmd);
			}
			m_bShowEquip = false;
			return true;
		}
		break;
	case EVENT_DIALOG_HIDE:
		return true;
	case EVENT_DIALOG_SHOW:
		return true;
	case EVENT_DIALOG_MOVE:
	{
	}
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent, nID, pControl);

	FUNCTION_END;
}

/**
 * \brief 得到网格
 *
 * 得到网格
 *
 * \param equip_type : 网格类型
 * \param x : 网格位置
 * \return 得到网格
 */
CGuiTable *&CGuiItem::GetEquipTable(int equip_type, int x)
{
	FUNCTION_BEGIN;

	return GetGameGuiManager()->m_guiUserInfo->GetEquipTable(equip_type, x);

	FUNCTION_END;
}

/**
 * \brief 设置角色名字
 *
 * 设置角色名字
 *
 * \param name : 角色名字
 * \return 无
 */
void CGuiItem::SetCharName(const char *name)
{
	FUNCTION_BEGIN;

	///	m_pEquip->SetCharName(name);

	FUNCTION_END;
}

/**
 * \brief 刷新对话框
 *
 * 刷新对话框
 *
 * \param fElapsedTime : 刷新时间
 * \return 处理结果
 */
HRESULT CGuiItem::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	switch (GetGameState()->GetStateItem())
	{
	case eGameStateItem_TradeRun:
		if (GetGameGuiManager()->m_guiTrade == NULL)
			break;

		m_ColorMoneyBox.SetValue(m_nMoney - GetGameGuiManager()->m_guiTrade->GetMoney(OBJECTCELLTYPE_TRADE));
		//		GetEditBox(11)->SetText(Money2String(String2Money(m_pMoneyBox->GetText()) - GetGameGuiManager()->m_guiTrade->GetMoney(OBJECTCELLTYPE_TRADE)));
		break;
	default:
		//		GetEditBox(11)->SetText(m_pMoneyBox->GetText());
		break;
	}
	//	GetEditBox(11)->SetEnabled(false);
	//	GetEditBox(11)->SetVisible(true);

	return CGuiDialog::OnRender(fElapsedTime);

	FUNCTION_END;
}