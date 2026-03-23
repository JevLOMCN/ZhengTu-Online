

/**
*\file		GuiTicketTrade.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    NPC交易对话框
*
* 
*
*/

#include "public.h"
#include "./GameGuiManager.h"
#include ".\guiTickettrade.h"
//#include <boost/regex.hpp>
#include <ostream>
#include "../Engine/include/stack_alloc.h"
#include "../Engine/include/stringex.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GameState.h"
#include "./RoleItem.h"
#include "./GuiNpcDialog.h"
#include "./GuiItem.h"
#include "./GameCursor.h"
#include "./GameAccelKeyManager.h"

//#include "../greta/regexpr2.h"
//using namespace regex;

/**
 * \brief 构造函数
 * 
 * 构造函数
 * 
 * \return 无
 */
CGuiTicketTrade::CGuiTicketTrade(void)
: m_dwRepair(CGuiNpcDialog::eTicket)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
 * \brief 对话框关闭
 * 
 * 对话框关闭
 * 
 * \return 无
 */
void CGuiTicketTrade::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameCursor()->SetCursor( CCursor::CursorType_Normal, eCursorNormal);
	
	if (GetScene()->GetMainCharacter()!=NULL)
	{
		GetScene()->GetMainCharacter()->RemoveItemByLocationType(OBJECTCELLTYPE_SELL);
		GetScene()->GetMainCharacter()->RemoveItemTipFlagByLocationType(OBJECTCELLTYPE_COMMON,ITEMTIPS_FLAG_PRICE);
		GetScene()->GetMainCharacter()->RemoveItemTipFlagByLocationType(OBJECTCELLTYPE_EQUIP,ITEMTIPS_FLAG_PRICE);
		GetScene()->GetMainCharacter()->RemoveItemTipFlagByLocationType(OBJECTCELLTYPE_PACKAGE,ITEMTIPS_FLAG_PRICE);
		GetScene()->GetMainCharacter()->RemoveItemTipFlagByLocationType(OBJECTCELLTYPE_MAKE,ITEMTIPS_FLAG_PRICE);
	}

	GetGameGuiManager()->m_guiTicketTrade = NULL;
	GetGameGuiManager()->m_guiTicketTrade = NULL;
	GetGameState()->SetStateItem(eGameStateItem_NpcDialog);
	if(GetGameGuiManager()->m_guiItem)
	{
		GetGameGuiManager()->m_guiItem->SetVisible(false);
	}
	GetGameGuiManager()->OnNpcActionEnd();

	

	FUNCTION_END;
}

/**
 * \brief 创建对话框
 * 
 * 创建对话框
 * 
 * \return 无
 */
void CGuiTicketTrade::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	m_pLabel = GetStatic(1);
	m_pLabel->SetTextColor(COLOR_ARGB(255,0,255,0)); //soke 标题颜色
	m_pTable = GetTable(2);
	m_pTab = GetTab(3);

	m2_pStaTicket = GetEditBox(1002); //soke 积分显示
	m2_pStaTicket->SetEnabled(false);

	m_pTable->m_iTableType = OBJECTCELLTYPE_SELL;
	m_strPageInfo.clear();
	//m_pTable->SetEnabled(false);
	
	RefreshRepairMoney();

	Engine_WriteLogF("创建商人窗口完毕！");

	FUNCTION_END;
}

//soke 增加窗口支持积分显示
HRESULT CGuiTicketTrade::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	// 刷新当前积分数
	DWORD dwTicket = GetScene()->GetMainCharacter()->GetProperty()->ticket;
	m2_pStaTicket->SetText(Ticket2String(dwTicket));

	return hr;

	FUNCTION_END;
}

/**
 * \brief 加入买物品的内容
 * 
 * 加入买物品的内容
 * 
 * \param pObject : 物品指针
 * \return 无
 */
void CGuiTicketTrade::AddBuyObject(ObjectBase_t* pObject,int nNum,int nLevel)
{
	FUNCTION_BEGIN;

	POINT ptCell = m_pTable->AppendItemPosition(pObject->cx,pObject->cy);

	stAddObjectPropertyUserCmd cmd;
	cmd.byActionType = EQUIPACTION_INIT;
	memset(&cmd.object,0,sizeof(cmd.object));
	strncpy(cmd.object.strName,pObject->strName,sizeof(cmd.object.strName));
	cmd.object.dwObjectID = pObject->dwID;
	cmd.object.qwThisID = INVALID_THISID;
	cmd.object.pos.dwLocation = OBJECTCELLTYPE_SELL;
	cmd.object.pos.dwTableID = 0;
	cmd.object.pos.x = ptCell.x;
	cmd.object.pos.y = ptCell.y;
	cmd.object.color = -1;
	cmd.object.dwNum = nNum;
	cmd.object.upgrade = nLevel;
	GetScene()->GetMainCharacter()->AddItem(&cmd);


	FUNCTION_END;
}

/**
 * \brief 解析服务器端的脚本
 * 
 * 解析服务器端的脚本
 * 
 * \param aObjects : 保存物品内容的缓冲
 * \param pszString : 脚本内容
 * \return 无
 */
#define  NumMark            "*"
#define  SeriateMark        "-"
template < class tVecObject >
void ParseTradeString( tVecObject & aObjects,const char* pszString )
{
	FUNCTION_BEGIN;

	SetFrameAllocator fa;
	std::vector< std::stack_string, std::stack_allocator<std::stack_string> > aParams1;

	_parse_str( aParams1, pszString , ';' );
	for(size_t i=0;i<aParams1.size();++i)
	{
		const char* pszTemp = aParams1[i].c_str();
		SetFrameAllocator fa;
		std::vector< std::stack_string, std::stack_allocator<std::stack_string> > results;
		std::vector< std::stack_string, std::stack_allocator<std::stack_string> > results1;
		_parse_str(results,pszTemp,'[');
		if(results.size() == 2)
		{
			if(results[1].size())
				results[1].resize(results[1].size()-1);

			pszTemp = results[1].c_str();
			std::vector<int,std::stack_allocator<int> > aParams2;
			if(stricmp("id",results[0].c_str() ) == 0)
			{
				// 按 ID 查找物品
				_parse_str(results1,results[1].c_str(),',');

				for( size_t k = 0; k < results1.size(); ++k )
				{
					//int bNum = results1[k].find(NumMark);
					int bNum = 0;
					bool found = false;
					const char *objstr = results1[k].c_str();
					for(int i = 0;i<strlen(objstr);i++)
					{
						if(objstr[i] == '*')
						{
							bNum++;
							found = true;
						}
					}
					if(!found)
						bNum = -1;
					//int bSeriate = results1[k].find(SeriateMark);
					_parse_str_num(aParams2,results1[k].c_str());

					//soke 高级杂货
					int nNum = 1;
					int nLevel = 0;
					int nMaxId = 0;
					if (bNum == 2)
					{
						nLevel = aParams2[aParams2.size()-1];
						nNum = aParams2[aParams2.size()-2];
						nMaxId = aParams2[aParams2.size()-3];
						bNum = -1;

					}
					else
					{
						nNum = (bNum == -1)?1:aParams2[aParams2.size()-1];
						nMaxId = (bNum == -1)?aParams2[aParams2.size()-1]:aParams2[aParams2.size()-2];
					}

					for (size_t h=aParams2[0];h<=nMaxId;++h)
					{
						ObjectBase_t* pObject = GetObjectBase(h);
						if(pObject)
						{
							stTicketBuyObject st;
							st.pObjectBase = pObject;
							st.nNum = nNum;
							st.nLevel = nLevel;
							aObjects.push_back(st);
						}
					}
				}

			}
			else if(stricmp("type",results[0].c_str() ) == 0 )
			{
				// 按 类型 查找物品
				_parse_str(results1,results[1].c_str(),',');

				for( size_t j = 0; j < results1.size(); ++j )
				{
					int bNum = results1[j].find(NumMark);
					_parse_str_num(aParams2,results1[j].c_str());
					int nNum = (bNum == -1)?1:aParams2[aParams2.size()-1];
					int nMaxId = (bNum == -1)?aParams2[aParams2.size()-1]:aParams2[aParams2.size()-2];

					for( size_t k = aParams2[0]; k <= nMaxId; ++k )
					{
						for(size_t h = 0 ; h < g_tableObjectBase.size();++h)
						{
							if(g_tableObjectBase[h].dwType == k)
							{
								stTicketBuyObject st;
								st.pObjectBase = &g_tableObjectBase[h];
								st.nNum = nNum;
								aObjects.push_back(st);
							}
						}
					}
				}

			}
		}
	}
	Engine_WriteLogF("解析服务端XML脚本完毕！");

	FUNCTION_END;
}


/**
 * \brief 加入买物品的页
 * 
 * 加入买物品的页
 * 
 * \param pageName : 页名
 * \param pszBuyParam : 脚本内容
 * \return 无
 */
void CGuiTicketTrade::AddBuyPage(const char* pageName,const char* pszBuyParam)
{
	FUNCTION_BEGIN;

	m_strPageInfo.push_back(pszBuyParam);
	m_pTab->AddItem(pageName,(void*)(m_strPageInfo.size()-1));

	FUNCTION_END;
}

void CGuiTicketTrade::SetPage(int nPage)
{
	if ((nPage>=0)&&(nPage<m_pTab->GetItemCount()))
	{
		m_pTab->SetCurItem(nPage);
		OnGuiEvent(EVENT_TAB_SELECTION_CHANGED,3,NULL);
	}
}

/**
* \brief 解析脚本内容
* 
* 解析脚本内容
* 
* \param params : 脚本内容
* \return 无
*/
void CGuiTicketTrade::SetParam(std::vector<std::string> & params)
{
	FUNCTION_BEGIN;

	int index = 0;
	if( params.size() > index )
	{
		SetFrameAllocator fa;
		std::vector<std::stack_string,std::stack_allocator<std::stack_string> > aStrBuyPage;
		_parse_str( aStrBuyPage,params[index].c_str(), '|' );

		for( size_t ix = 0; ix < aStrBuyPage.size(); ix ++ )
		{
			// 对每个页的描述进行解析
			SetFrameAllocator fa;
			std::vector<std::stack_string,std::stack_allocator<std::stack_string> > aStr;
			_parse_str(aStr,aStrBuyPage[ix].c_str(),'{');
			if(aStr.size() == 2)
			{
				if(aStr[1].size())
					aStr[1].resize(aStr[1].size()-1);

				AddBuyPage(aStr[0].c_str(),aStr[1].c_str());
			}
		}
		OnGuiEvent(EVENT_TAB_SELECTION_CHANGED,3,NULL);
	}
	index++;

	if(params.size() > index)
	{
		ParseTradeString(m_aSellObject,params[index].c_str());
	}
	index++;

	if(m_dwRepair!=CGuiNpcDialog::eTicket)
	{
		if(params.size() > index)
		{
			ParseTradeString(m_aRepairObject,params[index].c_str());
		}
		index++;
	}

	if(params.size() > index )
	{
		m_pLabel->SetText(params[index].c_str());
	}
	else
	{
		m_pLabel->SetText("");
	}
	index++;
	GetGameState()->SetStateItem(eGameStateItem_NpcTrade);
	GetScene()->GetMainCharacter()->AddItemTipFlagByLocationType(OBJECTCELLTYPE_SELL,ITEMTIPS_FLAG_PRICE);
	GetScene()->GetMainCharacter()->AddItemTipFlagByLocationType(OBJECTCELLTYPE_COMMON,ITEMTIPS_FLAG_PRICE);
	GetScene()->GetMainCharacter()->AddItemTipFlagByLocationType(OBJECTCELLTYPE_EQUIP,ITEMTIPS_FLAG_PRICE);
	GetScene()->GetMainCharacter()->AddItemTipFlagByLocationType(OBJECTCELLTYPE_PACKAGE,ITEMTIPS_FLAG_PRICE);
	GetScene()->GetMainCharacter()->AddItemTipFlagByLocationType(OBJECTCELLTYPE_MAKE,ITEMTIPS_FLAG_PRICE);

	FUNCTION_END;
}

/**
* \brief 检查是否是可购买的物品
* 
* 检查是否是可购买的物品
* 
* \param params : 物品ID
* \return 检查结果
*/
bool CGuiTicketTrade::IsBuyObject(DWORD dwObjectID)
{
	FUNCTION_BEGIN;

	for(size_t i=0;i<m_aBuyObject.size();++i)
	{
		if(m_aBuyObject[i].pObjectBase->dwID == dwObjectID)
			return true;
	}
	return false;

	FUNCTION_END;
}

/**
* \brief 检查是否是可卖的物品
* 
* 检查是否是可卖的物品
* 
* \param params : 物品ID
* \return 检查结果
*/
bool CGuiTicketTrade::IsSellObject(DWORD dwObjectID)
{
	FUNCTION_BEGIN;

	for(size_t i=0;i<m_aSellObject.size();++i)
	{
		if(m_aSellObject[i].pObjectBase->dwID == dwObjectID)
			return true;
	}
	return false;

	FUNCTION_END;
}

/**
* \brief 检查是否是修理的物品
* 
* 检查是否是修理的物品
* 
* \param params : 物品ID
* \return 检查结果
*/
bool CGuiTicketTrade::IsRepairObject(DWORD dwObjectID)
{
	FUNCTION_BEGIN;

	return false;

	FUNCTION_END;
}

/**
* \brief 刷新全部维修费用
* 
* 刷新全部维修费用
* 
* \return 无
*/
void CGuiTicketTrade::RefreshRepairMoney()
{
	FUNCTION_BEGIN;

	CGuiButton* pButton = GetButton(101);
	if (!pButton)
		return;

	pButton->GetToolTips()->Clear();
	char szTxt[MAX_PATH]={0};
	DWORD nRepairMoney = GetScene()->GetMainCharacter()->GetAllItemRepairMoney(m_dwRepair);
	//sprintf(szTxt,"全部修理费用：%s",GetMoneyShowStr(nRepairMoney));
	DWORD color;

	if (m_dwRepair==CGuiNpcDialog::eTradeRepairByGold)
	{
		sprintf(szTxt,"%s 金子",Money2String(nRepairMoney));
		color = GetScene()->GetMainCharacter()->GetProperty()->gold>=nRepairMoney?COLOR_ARGB(255,0,255,0):COLOR_ARGB(255,255,0,0);
	}
	else
	{
		sprintf(szTxt,"%s",Money2String(nRepairMoney));
		color = GetScene()->GetMainCharacter()->GetMoney()>=nRepairMoney?COLOR_ARGB(255,0,255,0):COLOR_ARGB(255,255,0,0);
	}
	
	pButton->GetToolTips()->SetCurColor(color);
	pButton->GetToolTips()->AddText(szTxt);
	pButton->GetToolTips()->Resize();

	FUNCTION_END;
}


/**
* \brief 消息处理函数
* 
* 消息处理函数
* 
* \param nEvent : 消息类别
* \param nID : 控件ID
* \param pControl : 控件指针
* \return 消息处理结果
*/
bool CGuiTicketTrade::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_TAB_SELECTION_CHANGED:
		if (nID == 3)
		{
			int index =(int) m_pTab->GetItemData(m_pTab->GetCurItem());
			GetScene()->GetMainCharacter()->RemoveItemByLocationType(OBJECTCELLTYPE_SELL);
			GetScene()->GetMainCharacter()->RemoveItemTipFlagByLocationType(OBJECTCELLTYPE_COMMON,ITEMTIPS_FLAG_PRICE);
			m_aBuyObject.clear();
			m_pTable->ClearAll();
			
			ParseTradeString(m_aBuyObject,m_strPageInfo[index].c_str());
			for(size_t i=0; i < m_aBuyObject.size(); ++i)
			{
			    //soke 增加品质
				AddBuyObject(m_aBuyObject[i].pObjectBase,m_aBuyObject[i].nNum,m_aBuyObject[i].nLevel);
			}
		}
		break;
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 100:
				{
					GetGameCursor()->SetCursor( CCursor::CursorType_Drag, eCursorRepair);
					//Engine_GetCursor()->SetImage(stResourceLocation("icon", ptCursor );
				}
				break;
			case 101:
				{
					if (m_dwRepair==CGuiNpcDialog::eTradeRepairByGold)
					{
						stRepairObjectGoldNpcTradeUserCmd cmd;
						cmd.id = 0;
						SEND_USER_CMD(cmd);
					}
					else
					{
						stRepairObjectNpcTradeUserCmd cmd;
						cmd.id = 0;
						cmd.gem_id = 0;
						SEND_USER_CMD(cmd);
					}
				}
				break;
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}