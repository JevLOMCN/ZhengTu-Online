#include "public.h"

#include "Game.h"
#include "GameGuiManager.h"
#include "GameScene.h"
#include "MainCharacter.h"
#include "RoleItem.h"
#include "GuiNpcMake.h"
#include "GuiNpcDialog.h"
#include "MyTimer.h"

#include "DlgEquipFabaoRecast.h"

namespace
{
	const int TAB_EQUIPFABAORECAST		= 4;
    
	const int TABLE_FOCUSITEM			= 80; //装备格子

	const int STATIC_NEEDMONEY			= 117;//价格

	const int TABLE_MATERIAL1			= 120; //材料格子
	
	const int STATIC_RESULTINFO			= 125;// 说明提示

    const int TIM_EQUIP_COMPOSE101 = 124;    //图鉴进阶
	const int TIM_EQUIP_COMPOSE102 = 130;    //图鉴签名



	const int BUTTONGROUP_EQUIPFABAORECAST	= 11;

	const int MAX_PAGE_NUM				= 2;// tab的分页数


	const std::string STR_EQUIPFABAORECAST_TITLES[MAX_PAGE_NUM] = 
	{
		std::string("法宝进阶"),
		std::string("符石进阶"),

	};

	const std::string RES_PACK_NAME = std::string("data\\interfaces3.gl");
	const int RES_GROUP = 374;
	int PAGE_BG_MAP[MAX_PAGE_NUM] = {82,82}; // 页数和背景图片frame的映射

	const int MONEY_SIGNATURE = 20000; // 添加（修改）签名需要的金钱（文）
}

CDlgEquipFabaoRecast::CDlgEquipFabaoRecast()
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;
	m_eCurPage = EQUIPFABAORECAST_COMPOSE101;
//	m_nCurBaseValue = MAX_BASE_VALUE;
	m_pTimerProcess = NULL;
	m_bPutFocusItem = true;
	m_unSoulMountNeedMoney = 0;
	for (int i = 0; i < TOTAL_SMATERIAL_TABLE; ++i)
	{
		m_pTableSMaterials[i] = NULL;
	}

	FUNCTION_END;
}

CDlgEquipFabaoRecast::~CDlgEquipFabaoRecast()
{
	FUNCTION_BEGIN;

	SAFE_DELETE(m_pTimerProcess);

	FUNCTION_END;
}

void CDlgEquipFabaoRecast::OnPutFocusItembook(bool bPut)
{
	m_bPutFocusItem = bPut;
	this->RefreshPage();
}

void CDlgEquipFabaoRecast::OnResult(stResponsePropertyUserCmd* pCmd)
{
	if (!pCmd)
	{
		return;
	}

	switch(pCmd->kind)
	{
	case EQUIP_COMPOSE103:   //法宝进阶
	case EQUIP_COMPOSE104:   //法宝符石进阶
     //新增在这里新增
		{
			DWORD dwColor = 0;
			if (pCmd->status == 0) //	123
			{
				stResourceLocation rl;
				rl.SetFileName("data\\Interfaces.gl");
				rl.group = 31;
				rl.frame = 51;
				m_aniSuccess.Create(&rl);
				m_aniSuccess.SetSpeed(65);
				m_aniSuccess.SetLoopPlay(false);
				PlayUISound(10005);

				dwColor = D3DCOLOR_ARGB(255,0,255,0);
			}
			else // 失败
			{
				stResourceLocation rl;
				rl.SetFileName("data\\Interfaces.gl");
				rl.group = 31;
				rl.frame = 54;
				m_aniSuccess.Create(&rl);
				m_aniSuccess.SetLoopPlay(false);

				dwColor = D3DCOLOR_ARGB(255,255,0,0);
			}
			m_pStaResult->SetTextColor(dwColor);
			m_pStaResult->SetText(pCmd->info);
		}
		break;
	}
}

void CDlgEquipFabaoRecast::OnClose(void)
{
	GetGameGuiManager()->m_pDlgEquipFabaoRecast = NULL;
}

void CDlgEquipFabaoRecast::OnCreate()
{
	CGuiDialog::OnCreate();

	m_BackElements.resize(1);

	CGuiElement* pEmt = new CGuiElement;
	pEmt->FontColor.Current = COLOR_ARGB(255,200,200,0); //soke 显示页面名称颜色
	pEmt->iFont = 1;
	pEmt->TextureColor.Current = COLOR_ARGB(128,0,0,0);
	pEmt->dwTextFormat = DT_CENTER | DT_CENTER;
	m_BackElements[0] = pEmt;

	m_pStaNeedMoney = this->GetStatic(STATIC_NEEDMONEY);

	m_pTabEquipFabaoRecast = this->GetTab(TAB_EQUIPFABAORECAST);
	if (m_pTabEquipFabaoRecast)
	{
		m_pTabEquipFabaoRecast->SetVisible(false);
		m_pTabEquipFabaoRecast->SetCurItem(static_cast<int>(m_eCurPage));
	}

	m_pTableSFocusItem = this->GetTable(TABLE_FOCUSITEM);
	m_pTableSFocusItem->m_iTableType = 	OBJECTCELLTYPE_FABAO;
	m_pTableSFocusItem->m_iTableID = TABLE_FOCUSITEM;
	m_pTableSFocusItem->m_EquipPosition = stPointI(0,MAKECELLTYPE_EQUIP);

	m_pTableSMaterial1 = this->GetTable(TABLE_MATERIAL1);
	m_pTableSMaterial1->m_iTableType = OBJECTCELLTYPE_FABAO;
	m_pTableSMaterial1->m_iTableID = TABLE_MATERIAL1;
	m_pTableSMaterial1->m_EquipPosition = stPointI(0,MAKECELLTYPE_MATERIAL1);
	m_pTableSMaterials[0] = m_pTableSMaterial1;


	for (int i = 0; i < TOTAL_PAGE_NUM; ++i)
	{
		m_pBtnPages[i] = this->GetButton(BUTTONGROUP_EQUIPFABAORECAST + i);
	}

	m_pBtnPages[static_cast<int>(m_eCurPage)]->SetChecked(true);
	m_strTitle = STR_EQUIPFABAORECAST_TITLES[static_cast<int>(m_eCurPage)];
	m_pStaResult = this->GetStatic(STATIC_RESULTINFO);

	this->RefreshPage();
}

bool CDlgEquipFabaoRecast::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch (nEvent)
	{
	case EVENT_DIALOG_SHOW:
		{
			m_pBtnPages[static_cast<int>(m_eCurPage)]->SetChecked(true);
			if (m_pStaResult)
			{
				m_pStaResult->SetText("");
			}
			this->RefreshPage();
		}
		break;
	case EVENT_BUTTON_CLICKED:
		{
			switch (nID)
			{
				case 2: //sky 关闭
				{
					SetVisible(false);
				}
				break;
			}
			if (nID >= BUTTONGROUP_EQUIPFABAORECAST && nID < BUTTONGROUP_EQUIPFABAORECAST + MAX_PAGE_NUM)
			{
				m_pStaResult->SetText("");
				int nIndex = nID - BUTTONGROUP_EQUIPFABAORECAST;
				m_pTabEquipFabaoRecast->SetCurItem(nIndex);
				m_pBtnPages[nIndex]->SetChecked(true);
				m_eCurPage = static_cast<EQUIPFABAORECAST_PAGE>(nIndex);
				m_strTitle = STR_EQUIPFABAORECAST_TITLES[static_cast<int>(m_eCurPage)];

				this->RefreshPage();
			}

			else if (nID == TIM_EQUIP_COMPOSE101) ///<法宝进阶
			{
				if (this->CanComposeEquip101())
				{
				this->SendEquipFabaoRecastMsg(RECAST_ITEM_COMPOSE103);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE102) ///<法宝符石进阶
			{
				if (this->CanComposeEquip102())
				{
				this->SendEquipFabaoRecastMsg(RECAST_ITEM_COMPOSE104); 
				}
			}
		}
		break;
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CDlgEquipFabaoRecast::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());
		m_pBtnClose->SetLocation(pt.x-22, pt.y-38);
	}

	FUNCTION_END;
}

HRESULT CDlgEquipFabaoRecast::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	DrawText(m_strTitle.c_str(), m_BackElements[0], &stRectI(10, 40, 330, 340));//soke 页面说明字体位置
	m_aniSuccess.Render(m_x-103,m_y-250);//soke 装特效位置



	return hr;

	FUNCTION_END;
}

//soke 图鉴进阶
bool CDlgEquipFabaoRecast::CanComposeEquip101()
{
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_SMATERIAL_TABLE; ++i)
	{
		pRoleItem = this->GetMaterialItem(i);
		if (pRoleItem)
		{
			vecMaterialItems.push_back(pRoleItem);
			nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
		}
	}

	if (vecMaterialItems.size() == 0)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 255);
		m_pStaResult->SetTextColor(dwColor);
		m_pStaResult->SetText("单件法宝配合玉竹筒可进阶");
		return false;
	}

	CRoleItem* pItemKa = NULL;
	int nItemKa = 0; //装备件数
	int nItemChange = 0; //需要的材料数量

	DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
	m_pStaResult->SetTextColor(dwColor);

	
	for (int i = 0; i < vecMaterialItems.size(); ++i)
	{
		if (vecMaterialItems[i]->GetObjectID() == 20016  ) 
		{
			nItemChange++;
			continue;
		}

		int bFind = false;
		if (this->IsEquipItem(vecMaterialItems[i]->GetItemType()))
		{
			nItemKa++;
			pItemKa = vecMaterialItems[i];

			bFind = true;
		}

		if (bFind == false)
		{
			m_pStaResult->SetText("对不起，非法宝类物品不能进阶！");
			return false;
		}
	}

	if ((nItemKa + nItemChange) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("对不起，非法宝类物品不能进阶！");
		return false;
	}


	if ((nItemKa != 1) || (nItemChange != 1))
	{
		m_pStaResult->SetText("必须放一件法宝类和玉竹筒进阶！");
		return false;
	}
	
	
	if (m_MakeData.CheckMoney(50000) == false) //soke 需要银子
	{
		m_pStaResult->SetText("对不起，你银子不足5锭，不能装备签名！");
		return false;
	}


	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 图鉴签名
bool CDlgEquipFabaoRecast::CanComposeEquip102()
{
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
	}

	for (int i = 0; i < TOTAL_SMATERIAL_TABLE; ++i)
	{
		pRoleItem = this->GetMaterialItem(i);
		if (pRoleItem)
		{
			vecMaterialItems.push_back(pRoleItem);
		}
	}

	if (vecMaterialItems.size() == 0)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 0, 255, 0);
		m_pStaResult->SetTextColor(dwColor);
		m_pStaResult->SetText("法宝符石进阶");
		return false;
	}

	DWORD dwColor = D3DCOLOR_ARGB(255, 200, 200, 0);
	m_pStaResult->SetTextColor(dwColor);

	CRoleItem* pItemKa = NULL;
	int nItemKa = 0; //装备件数
	int nItemChange = 0; //需要的材料数量
	
	for (int i = 0; i < vecMaterialItems.size(); ++i)
	{
		if (vecMaterialItems[i]->GetObjectID() == 20017  ) 
		{
			nItemChange++;
			continue;
		}

		int bFind = false;
		if (this->IsEquipItem2(vecMaterialItems[i]->GetItemType()))
		{
			nItemKa++;
			pItemKa = vecMaterialItems[i];

			bFind = true;
		}

		if (bFind == false)
		{
			m_pStaResult->SetText("对不起，非法宝符石类物品不能进阶！");
			return false;
		}
	}

	if ((nItemKa + nItemChange) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("对不起，非法宝符石类物品不能进阶！");
		return false;
	}


	if ((nItemKa != 1) || (nItemChange != 1))
	{
		m_pStaResult->SetText("必须放一件法宝符石和紫金砂进阶！");
		return false;
	}
	
	
	if (m_MakeData.CheckMoney(50000) == false) //soke 需要银子
	{
		m_pStaResult->SetText("对不起，你银子不足5锭，不能装备签名！");
		return false;
	}

	return true;
}


CRoleItem* CDlgEquipFabaoRecast::GetFocusItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableSFocusItem->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem= static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}

CRoleItem* CDlgEquipFabaoRecast::GetMaterialItem(int index)
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableSMaterials[index]->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}

bool CDlgEquipFabaoRecast::IsEquipItem(enumItemType eType)
{
	if (eType ==ItemType_Fabao)
	{
		return true;
	}

	return false;
}

bool CDlgEquipFabaoRecast::IsEquipItem2(enumItemType eType)
{
	if ((eType >= ItemType_Fabaofj1 && eType <= ItemType_Fabaofj12))
	{
		return true;
	}

	return false;
}

void CDlgEquipFabaoRecast::SendEquipFabaoRecastMsg(RecastAction eAction)
{
	CGuiItemCell* pItemCell = m_pTableSFocusItem->GetItemCell(0, 0);
	if (!pItemCell)
	{
		return;
	}

	CRoleItem* pRoleItem= static_cast<CRoleItem*>(pItemCell->m_pItemData);
	if (pRoleItem)
	{
		ItemRecastPropertyUserCmd cmd;
		cmd.act = eAction;
		switch (eAction)
		{

		case RECAST_ITEM_COMPOSE103:
			cmd.itemUID = pRoleItem->GetObject()->qwThisID;
			break;
		}

		SEND_USER_CMD(cmd);
	}
}

void CDlgEquipFabaoRecast::SetBGImage()
{
	stResourceLocation rl;
	strcpy(rl.szFileName, RES_PACK_NAME.c_str());
	rl.group = RES_GROUP;
	rl.frame = PAGE_BG_MAP[static_cast<int>(m_eCurPage)];

	this->SetBackImage(rl);
}

void CDlgEquipFabaoRecast::RefreshPage()
{
	switch (m_eCurPage)
	{

	case EQUIPFABAORECAST_COMPOSE101: // 法宝进阶
		{
			this->RefreshEquipCompose101();
		}
		break;
	case EQUIPFABAORECAST_COMPOSE102: // 法宝符石进阶
		{
			this->RefreshEquipCompose102();
		}
		break;	
	this->SetBGImage();
	}
}

//soke 图鉴进阶
void CDlgEquipFabaoRecast::RefreshEquipCompose101()
{
	bool bShow = false;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem && m_bPutFocusItem)
	{
		bShow = true;
	}

	m_pStaNeedMoney->SetVisible(bShow);
	if (bShow)
	{
	  bool bEnable = this->CanComposeEquip101();
	  this->GetButton(TIM_EQUIP_COMPOSE101)->SetEnabled(bEnable);

		if (bEnable)
		{
			std::stringstream ss;
			ss << "需要银子：1锭";
			m_pStaNeedMoney->SetText(ss.str().c_str());
			m_pStaResult->SetText("");
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}
//soke 图鉴签名
void CDlgEquipFabaoRecast::RefreshEquipCompose102()
{
	bool bShow = false;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem && m_bPutFocusItem)
	{
		bShow = true;
	}

	m_pStaNeedMoney->SetVisible(bShow);
	if (bShow)
	{
	    bool bEnable = this->CanComposeEquip102();
	    this->GetButton(TIM_EQUIP_COMPOSE102)->SetEnabled(bEnable);

	  	if (bEnable)
		{
			std::stringstream ss;
			ss << "需要银子：5锭";
			m_pStaNeedMoney->SetText(ss.str().c_str());
			m_pStaResult->SetText("");
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}



