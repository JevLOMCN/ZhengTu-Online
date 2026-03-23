#include "public.h"

#include "Game.h"
#include "GameGuiManager.h"
#include "GameScene.h"
#include "MainCharacter.h"
#include "RoleItem.h"
#include "GuiNpcMake.h"
#include "GuiNpcDialog.h"
#include "MyTimer.h"

#include "DlgEquipbookRecast.h"

namespace
{
	const int TAB_EQUIPBOOKRECAST		= 4;
    
	const int TABLE_FOCUSITEM			= 80; //装备格子

	const int STATIC_NEEDMONEY			= 117;//价格

	const int TABLE_MATERIAL1			= 120; //材料格子
	
	const int STATIC_RESULTINFO			= 125;// 说明提示

    const int TIM_EQUIP_COMPOSE101 = 124;    //图鉴进阶
	const int TIM_EQUIP_COMPOSE102 = 130;    //图鉴签名



	const int BUTTONGROUP_EQUIPBOOKRECAST	= 11;

	const int MAX_PAGE_NUM				= 2;// tab的分页数


	const std::string STR_EQUIPBOOKRECAST_TITLES[MAX_PAGE_NUM] = 
	{
		std::string("强化进阶"),
		std::string("图鉴签名"),

	};

	const std::string RES_PACK_NAME = std::string("data\\interfaces2.gl");
	const int RES_GROUP = 374;
	int PAGE_BG_MAP[MAX_PAGE_NUM] = {82,82}; // 页数和背景图片frame的映射

	const int MONEY_SIGNATURE = 20000; // 添加（修改）签名需要的金钱（文）
}

CDlgEquipbookRecast::CDlgEquipbookRecast()
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;
	m_eCurPage = EQUIPBOOKRECAST_COMPOSE101;
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

CDlgEquipbookRecast::~CDlgEquipbookRecast()
{
	FUNCTION_BEGIN;

	SAFE_DELETE(m_pTimerProcess);

	FUNCTION_END;
}

void CDlgEquipbookRecast::OnPutFocusItembook(bool bPut)
{
	m_bPutFocusItem = bPut;
	this->RefreshPage();
}

void CDlgEquipbookRecast::OnResult(stResponsePropertyUserCmd* pCmd)
{
	if (!pCmd)
	{
		return;
	}

	switch(pCmd->kind)
	{
	case EQUIP_COMPOSE101:   //图鉴进阶
	case EQUIP_COMPOSE102:   //图鉴签名
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
				m_aniSuccess.SetSpeed(25);
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

void CDlgEquipbookRecast::OnClose(void)
{
	GetGameGuiManager()->m_pDlgEquipbookRecast = NULL;
}

void CDlgEquipbookRecast::OnCreate()
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

	m_pTabEquipbookRecast = this->GetTab(TAB_EQUIPBOOKRECAST);
	if (m_pTabEquipbookRecast)
	{
		m_pTabEquipbookRecast->SetVisible(false);
		m_pTabEquipbookRecast->SetCurItem(static_cast<int>(m_eCurPage));
	}

	m_pTableSFocusItem = this->GetTable(TABLE_FOCUSITEM);
	m_pTableSFocusItem->m_iTableType = 	OBJECTCELLTYPE_HANDBOOK;
	m_pTableSFocusItem->m_iTableID = TABLE_FOCUSITEM;
	m_pTableSFocusItem->m_EquipPosition = stPointI(0,MAKECELLTYPE_EQUIP);

	m_pTableSMaterial1 = this->GetTable(TABLE_MATERIAL1);
	m_pTableSMaterial1->m_iTableType = OBJECTCELLTYPE_HANDBOOK;
	m_pTableSMaterial1->m_iTableID = TABLE_MATERIAL1;
	m_pTableSMaterial1->m_EquipPosition = stPointI(0,MAKECELLTYPE_MATERIAL1);
	m_pTableSMaterials[0] = m_pTableSMaterial1;


	for (int i = 0; i < TOTAL_PAGE_NUM; ++i)
	{
		m_pBtnPages[i] = this->GetButton(BUTTONGROUP_EQUIPBOOKRECAST + i);
	}

	m_pBtnPages[static_cast<int>(m_eCurPage)]->SetChecked(true);
	m_strTitle = STR_EQUIPBOOKRECAST_TITLES[static_cast<int>(m_eCurPage)];
	m_pStaResult = this->GetStatic(STATIC_RESULTINFO);

	this->RefreshPage();
}

bool CDlgEquipbookRecast::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
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
			if (nID >= BUTTONGROUP_EQUIPBOOKRECAST && nID < BUTTONGROUP_EQUIPBOOKRECAST + MAX_PAGE_NUM)
			{
				m_pStaResult->SetText("");
				int nIndex = nID - BUTTONGROUP_EQUIPBOOKRECAST;
				m_pTabEquipbookRecast->SetCurItem(nIndex);
				m_pBtnPages[nIndex]->SetChecked(true);
				m_eCurPage = static_cast<EQUIPBOOKRECAST_PAGE>(nIndex);
				m_strTitle = STR_EQUIPBOOKRECAST_TITLES[static_cast<int>(m_eCurPage)];

				this->RefreshPage();
			}

			else if (nID == TIM_EQUIP_COMPOSE101) ///<图鉴进阶
			{
				if (this->CanComposeEquip101())
				{
				this->SendEquipbookRecastMsg(RECAST_ITEM_COMPOSE101);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE102) ///<图鉴签名
			{
				if (this->CanComposeEquip102())
				{
				this->SendEquipbookRecastMsg(RECAST_ITEM_COMPOSE102); 
				}
			}
		}
		break;
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CDlgEquipbookRecast::OnSetCloseLocation()
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

HRESULT CDlgEquipbookRecast::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	DrawText(m_strTitle.c_str(), m_BackElements[0], &stRectI(100, 92, 145, 30));//soke 页面说明字体位置
	m_aniSuccess.Render(m_x-29,m_y-226);//soke 装特效位置



	return hr;

	FUNCTION_END;
}

//soke 图鉴进阶
bool CDlgEquipbookRecast::CanComposeEquip101()
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
		m_pStaResult->SetText("图鉴配合怪物灵魂石可进阶图鉴");
		return false;
	}

	CRoleItem* pItemUp = NULL;
	int nItemUp = 0;
	int nItemChange = 0;
	int nItemRate = 0;

	DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
	m_pStaResult->SetTextColor(dwColor);

	for (int i = 0; i < vecMaterialItems.size(); ++i)
	{
		if (vecMaterialItems[i]->GetObjectID() == 3980) //图鉴进阶材料
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) &&(701 > vecMaterialItems[i]->GetObject()->handbooklvg)) //图鉴强化上限
		{
			++nItemUp;
			pItemUp = vecMaterialItems[i];
			continue;
		}

		m_pStaResult->SetText("存在非升级类道具");
		return false;
	}

	if ((nItemUp + nItemChange + nItemRate) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("存在非升级类道具");
		return false;
	}


	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合怪物灵魂石才能进阶");
		return false;
	}

	if (pItemUp->GetObject()->handbooklvg >= 700) //图鉴次数上限
	{
		m_pStaResult->SetText("当前图鉴等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：1锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 图鉴签名
bool CDlgEquipbookRecast::CanComposeEquip102()
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
		m_pStaResult->SetText("图鉴装备签名");
		return false;
	}

	DWORD dwColor = D3DCOLOR_ARGB(255, 200, 200, 0);
	m_pStaResult->SetTextColor(dwColor);

	CRoleItem* pItemKa = NULL;
	int nItemKa = 0; //装备件数
	int nItemChange = 0; //需要的材料数量
	
	for (int i = 0; i < vecMaterialItems.size(); ++i)
	{
		if (vecMaterialItems[i]->GetObjectID() == 548  ) 
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
			m_pStaResult->SetText("对不起，非图鉴类物品不能进行签名！");
			return false;
		}
	}

	if ((nItemKa + nItemChange) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("对不起，非装备类物品不能完成图鉴签名！");
		return false;
	}


	if ((nItemKa != 1) || (nItemChange != 1))
	{
		m_pStaResult->SetText("必须放一件装备和一颗星灵石进行图鉴签名！");
		return false;
	}
	
	
	if (m_MakeData.CheckMoney(50000) == false) //soke 需要银子
	{
		m_pStaResult->SetText("对不起，你银子不足5锭，不能装备签名！");
		return false;
	}

	return true;
}


CRoleItem* CDlgEquipbookRecast::GetFocusItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableSFocusItem->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem= static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}

CRoleItem* CDlgEquipbookRecast::GetMaterialItem(int index)
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableSMaterials[index]->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}

bool CDlgEquipbookRecast::IsEquipItem(enumItemType eType)
{
	if ((eType >= 221 && eType <= 230))
	{
		return true;
	}

	return false;
}

void CDlgEquipbookRecast::SendEquipbookRecastMsg(RecastAction eAction)
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

		case RECAST_ITEM_COMPOSE101:
			cmd.itemUID = pRoleItem->GetObject()->qwThisID;
			break;
		}

		SEND_USER_CMD(cmd);
	}
}

void CDlgEquipbookRecast::SetBGImage()
{
	stResourceLocation rl;
	strcpy(rl.szFileName, RES_PACK_NAME.c_str());
	rl.group = RES_GROUP;
	rl.frame = PAGE_BG_MAP[static_cast<int>(m_eCurPage)];

	this->SetBackImage(rl);
}

void CDlgEquipbookRecast::RefreshPage()
{
	switch (m_eCurPage)
	{

	case EQUIPBOOKRECAST_COMPOSE101: // 图鉴进阶
		{
			this->RefreshEquipCompose101();
		}
		break;
	case EQUIPBOOKRECAST_COMPOSE102: // 图鉴签名
		{
			this->RefreshEquipCompose102();
		}
		break;	
	this->SetBGImage();
	}
}

//soke 图鉴进阶
void CDlgEquipbookRecast::RefreshEquipCompose101()
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
void CDlgEquipbookRecast::RefreshEquipCompose102()
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



