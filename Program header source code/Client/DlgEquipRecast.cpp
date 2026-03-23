#include "public.h"

#include "Game.h"
#include "GameGuiManager.h"
#include "GameScene.h"
#include "MainCharacter.h"
#include "RoleItem.h"
#include "GuiNpcMake.h"
#include "GuiNpcDialog.h"
#include "MyTimer.h"

#include "DlgEquipRecast.h"

namespace
{
	const int TAB_EQUIPRECAST			= 4;
	const int TABLE_FOCUSITEM			= 80;

	const int STATIC_NEEDMONEY			= 117;//价格

	const int TABLE_MATERIAL1			= 120;

	const int STATIC_RESULTINFO			= 125;// 说明提示

    const int TIM_EQUIP_COMPOSE1 = 124;    //装备祝福
	const int TIM_EQUIP_COMPOSE2 = 130;    //斗魂升级
	const int TIM_EQUIP_COMPOSE3 = 131;    //神石镶嵌
	const int TIM_EQUIP_COMPOSE4 = 132;    //装备栏激活
	const int TIM_EQUIP_COMPOSE5 = 133;    //装备栏进阶
	const int TIM_EQUIP_COMPOSE6 = 134;    //神器强化
	const int TIM_EQUIP_COMPOSE7 = 135;   //龙槽激活
	const int TIM_EQUIP_COMPOSE8 = 136;   //龙槽解封
	const int TIM_EQUIP_COMPOSE9 = 137;   //龙星升级
	const int TIM_EQUIP_COMPOSE10 = 138;   //魂魄清除
	const int TIM_EQUIP_COMPOSE11 = 139;   //装备升级
	const int TIM_EQUIP_COMPOSE12 = 140;    //马牌精炼
	const int TIM_EQUIP_COMPOSE13 = 141;    //神龙镶嵌
	const int TIM_EQUIP_COMPOSE14 = 142;    //逆天进阶
	const int TIM_EQUIP_COMPOSE15 = 143;    //斗战套装进阶
	const int TIM_EQUIP_COMPOSE16 = 144;    //暗影进阶
	const int TIM_EQUIP_COMPOSE17 = 145;    //龙凤吟
	const int TIM_EQUIP_COMPOSE18 = 146;    //轰天宝石镶嵌
	const int TIM_EQUIP_COMPOSE19 = 147;    //王者升级

	const int BUTTONGROUP_EQUIPRECAST	= 11;

	const int MAX_PAGE_NUM				= 19;// tab的分页数
//	const int DISPLAY_TABLE_ROWNUM		= 2;// table显示的行数
//	const int DISPLAY_TABLE_COLNUM		= 5;// table现实的列数

	const std::string STR_EQUIPRECAST_TITLES[MAX_PAGE_NUM] =
	{
		std::string("装备祝福"),
		std::string("斗魂升级"),
		std::string("神石镶嵌(190级)"),
		std::string("装备栏激活"),
		std::string("装备栏进阶"),
		std::string("神器强化"),
		std::string("龙槽激活(197级)"),
		std::string("龙槽解封(197级)"),
		std::string("龙星升级(197级)"),
		std::string("魂魄清除(190级)"),
		std::string("装备升级"),
		std::string("马牌精炼"),
		std::string("神龙镶嵌"),
		std::string("逆天进阶"),
		std::string("字纹激活(200级)")	,
		std::string("暗影进阶")	,	
		std::string("龙凤吟镶(202级)"),		
		std::string("轰天镶嵌(190级)"),	
		std::string("无双武器(190级)")	,							

	};

	const std::string RES_PACK_NAME = std::string("data\\interfaces1.gl");
	const int RES_GROUP = 8;
	int PAGE_BG_MAP[MAX_PAGE_NUM] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0}; // 页数和背景图片frame的映射

	const int MONEY_SIGNATURE = 20000; // 添加（修改）签名需要的金钱（文）
}

CDlgEquipRecast::CDlgEquipRecast()
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;
	m_eCurPage = EQUIPRECAST_COMPOSE13;
//	m_nCurBaseValue = MAX_BASE_VALUE;
	m_pTimerProcess = NULL;
	m_bPutFocusItem = true;
	m_unSoulMountNeedMoney = 0;
	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
	{
		m_pTableMaterials[i] = NULL;
	}

	FUNCTION_END;
}

CDlgEquipRecast::~CDlgEquipRecast()
{
	FUNCTION_BEGIN;

	SAFE_DELETE(m_pTimerProcess);

	FUNCTION_END;
}

void CDlgEquipRecast::OnPutFocusItem(bool bPut)
{
	m_bPutFocusItem = bPut;
	this->RefreshPage();
}

void CDlgEquipRecast::OnResult(stResponsePropertyUserCmd* pCmd)
{
	if (!pCmd)
	{
		return;
	}

	switch(pCmd->kind)
	{
	case EQUIP_COMPOSE11:   //装备祝福
	case EQUIP_COMPOSE12:   //斗魂升级
	case EQUIP_COMPOSE13:   //神石镶嵌
	case EQUIP_COMPOSE14:   //装备栏激活
	case EQUIP_COMPOSE15:   //装备栏进阶
	case EQUIP_COMPOSE16:   //神器强化
	case EQUIP_COMPOSE17:   //龙槽激活
	case EQUIP_COMPOSE18:   //龙槽解封
	case EQUIP_COMPOSE19:   //龙星升级
	case EQUIP_COMPOSE20:   //魂魄清除
	case EQUIP_COMPOSE21:   //装备升级
	case EQUIP_COMPOSE22:   //马牌精炼
	case EQUIP_COMPOSE23:   //神龙镶嵌
	case EQUIP_COMPOSE24:   //逆天进阶
	case EQUIP_COMPOSE25:   //斗战圣佛进阶
	case EQUIP_COMPOSE26:   //暗影石镶嵌		
	case EQUIP_COMPOSE27:   //龙凤吟		
	case EQUIP_COMPOSE28:   //轰天宝石镶嵌		
	case EQUIP_COMPOSE29:   //王者升级	
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

void CDlgEquipRecast::OnClose(void)
{
	GetGameGuiManager()->m_pDlgEquipRecast = NULL;
}

void CDlgEquipRecast::OnCreate()
{
	CGuiDialog::OnCreate();

	m_BackElements.resize(1);

	CGuiElement* pEmt = new CGuiElement;
	pEmt->FontColor.Current = COLOR_ARGB(255,200,200,0); //soke 显示页面名称颜色
	pEmt->iFont = 2;
	pEmt->TextureColor.Current = COLOR_ARGB(128,0,0,0);
	pEmt->dwTextFormat = DT_CENTER | DT_CENTER;
	m_BackElements[0] = pEmt;

	m_pStaNeedMoney = this->GetStatic(STATIC_NEEDMONEY);

	m_pTabEquipRecast = this->GetTab(TAB_EQUIPRECAST);
	if (m_pTabEquipRecast)
	{
		m_pTabEquipRecast->SetVisible(false);
		m_pTabEquipRecast->SetCurItem(static_cast<int>(m_eCurPage));
	}

	m_pTableFocusItem = this->GetTable(TABLE_FOCUSITEM);
	m_pTableFocusItem->m_iTableType = OBJECTCELLTYPE_RECAST;
	m_pTableFocusItem->m_iTableID = TABLE_FOCUSITEM;
	m_pTableFocusItem->m_EquipPosition = stPointI(0,MAKECELLTYPE_EQUIP);

	m_pTableMaterial1 = this->GetTable(TABLE_MATERIAL1);
	m_pTableMaterial1->m_iTableType = OBJECTCELLTYPE_RECAST;
	m_pTableMaterial1->m_iTableID = TABLE_MATERIAL1;
	m_pTableMaterial1->m_EquipPosition = stPointI(0,MAKECELLTYPE_MATERIAL1);
	m_pTableMaterials[0] = m_pTableMaterial1;

	for (int i = 0; i < TOTAL_PAGE_NUM; ++i)
	{
		m_pBtnPages[i] = this->GetButton(BUTTONGROUP_EQUIPRECAST + i);
	}

	m_pBtnPages[static_cast<int>(m_eCurPage)]->SetChecked(true);
	m_strTitle = STR_EQUIPRECAST_TITLES[static_cast<int>(m_eCurPage)];
	m_pStaResult = this->GetStatic(STATIC_RESULTINFO);

	this->RefreshPage();
}

bool CDlgEquipRecast::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
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
			if (nID >= BUTTONGROUP_EQUIPRECAST && nID < BUTTONGROUP_EQUIPRECAST + MAX_PAGE_NUM)
			{
				m_pStaResult->SetText("");
				int nIndex = nID - BUTTONGROUP_EQUIPRECAST;
				m_pTabEquipRecast->SetCurItem(nIndex);
				m_pBtnPages[nIndex]->SetChecked(true);
				m_eCurPage = static_cast<EQUIPRECAST_PAGE>(nIndex);
				m_strTitle = STR_EQUIPRECAST_TITLES[static_cast<int>(m_eCurPage)];

				this->RefreshPage();
			}

			else if (nID == TIM_EQUIP_COMPOSE1) ///<装备祝福
			{
				if (this->CanComposeEquip1())
				{
				this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE11);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE2) ///<斗魂升级
			{
				if (this->CanComposeEquip2())
				{
				this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE12);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE3) ///<神石镶嵌
			{
				if (this->CanComposeEquip3())
				{
				this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE13);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE4) ///<装备栏激活
			{
				if (this->CanComposeEquip4())
				{
				this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE14);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE5) ///<装备栏进阶
			{
				if (this->CanComposeEquip5())
				{
				this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE15);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE6) ///<神器强化
			{
				if (this->CanComposeEquip6())
				{
				this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE16);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE7) ///<龙槽激活
			{
				if (this->CanComposeEquip7())
				{
				this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE17);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE8) ///<龙槽解封
			{
				if (this->CanComposeEquip8())
				{
				this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE18);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE9) ///<龙星升级
			{
				if (this->CanComposeEquip9())
				{
				this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE19);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE10) ///<魂魄清除
			{
				if (this->CanComposeEquip10())
				{
				this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE20);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE11) ///<装备升级
			{
				if (this->CanComposeEquip11())
				{
					this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE21);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE12) ///<马牌精炼
			{
				if (this->CanComposeEquip12())
				{
			    	this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE22);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE13) ///<神龙镶嵌
			{
				if (this->CanComposeEquip13())
				{
				    this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE23);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE14) ///<逆天装备进阶
			{
				if (this->CanComposeEquip14())
				{
				    this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE24);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE15) ///<斗战圣佛进阶
			{
				if (this->CanComposeEquip15())
				{
			    	this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE25);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE16) ///<暗影石
			{
				if (this->CanComposeEquip16())
				{
				    this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE26);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE17) ///<龙凤吟
			{
				if (this->CanComposeEquip17())
				{
				    this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE27);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE18) ///<轰天宝石镶嵌
			{
				if (this->CanComposeEquip18())
				{
				    this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE28);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE19) ///<王者升级
			{
				if (this->CanComposeEquip19())
				{
				    this->SendEquipRecastMsg(RECAST_ITEM_COMPOSE29);
				}
			}

		}
		break;
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CDlgEquipRecast::OnSetCloseLocation()
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

HRESULT CDlgEquipRecast::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	DrawText(m_strTitle.c_str(), m_BackElements[0], &stRectI(395, 85, 627, 340));//soke 页面说明字体位置
	m_aniSuccess.Render(m_x+299,m_y-149);//soke 装特效位置



	return hr;

	FUNCTION_END;
}

//soke 装备祝福
bool CDlgEquipRecast::CanComposeEquip1()
{
    std::vector<CRoleItem*> vecMaterialItems;
    CRoleItem* pRoleItem = this->GetFocusItem();
    if (pRoleItem)
    {
        vecMaterialItems.push_back(pRoleItem);
    }

    for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
        m_pStaResult->SetText("15星绿色装备可以祝福成紫装！");
        return false;
    }

    DWORD dwColor = D3DCOLOR_ARGB(255, 200, 200, 0);
    m_pStaResult->SetTextColor(dwColor);

    CRoleItem* pItemKa = NULL;
    int nItemKa = 0; //装备件数
    int nItemChange = 0; //需要的材料数量
    
    for (int i = 0; i < vecMaterialItems.size(); ++i)
    {
        if (vecMaterialItems[i]->GetObjectID() == 1317)
        {
            nItemChange++;
            continue;
        }

        if (this->IsEquipItem(vecMaterialItems[i]->GetItemType()))
        {
            ++nItemKa;
            pItemKa = vecMaterialItems[i];
            continue;
        }

        m_pStaResult->SetText("对不起，非装备类物品不能进行装备祝福！");
        return false;
    }

    if ((nItemKa + nItemChange) < vecMaterialItems.size())
    {
        m_pStaResult->SetText("对不起，非装备类物品不能完成装备祝福！");
        return false;
    }

    if (nItemKa == 1)
    {

       if ((pItemKa->GetObject()->kind & 16))
        {
            m_pStaResult->SetText("            当前装备不需要祝福");
            return false;
        }
        if (pItemKa->GetObject()->needlevel < 120)
        {
            m_pStaResult->SetText("            120级以下的装备不能进行祝福");
            return false;
        }
        else
        {
            if (nItemChange == 0)
            {
                m_pStaResult->SetText("            请放入祝福石,才能进行装备祝福！");
                return false;
            }
        }
    }

    if ((nItemKa != 1) || (nItemChange < 1))
    {
        m_pStaResult->SetText("必须放一件装备和一个颗祝福石进行装备祝福！");
        return false;
    }

    char szInfo[MAX_PATH] = {0};
    sprintf(szInfo, "消耗银子：5锭");
    m_pStaNeedMoney->SetText(szInfo);

    return true;
}

//soke 斗魂升级
bool CDlgEquipRecast::CanComposeEquip2()
{
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("      单件装备配合斗魂石可升级斗魂");
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
		if ((vecMaterialItems[i]->GetObjectID() >= 3809 && vecMaterialItems[i]->GetObjectID() <= 3810))	//斗魂材料	
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) &&(14 > vecMaterialItems[i]->GetObject()->douhun))
		{
			++nItemUp;
			pItemUp = vecMaterialItems[i];
			continue;
		}

		m_pStaResult->SetText("            存在非升级类道具");
		return false;
	}

	if ((nItemUp + nItemChange + nItemRate) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("            存在非升级类道具");
		return false;
	}

	if (nItemUp == 1)
	{
		if (!(pItemUp->GetObject()->kind & 16))
		{
			m_pStaResult->SetText("            只有70级以上的紫装才能升级斗魂");
			return false;
		}
		if (pItemUp->GetObject()->needlevel < 70)
		{
			m_pStaResult->SetText("            70级以下的装备不能升级斗魂");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				m_pStaResult->SetText("            70级及以上的装备需要斗魂丹来升级斗魂");
				return false;
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("            单件装备配合斗魂丹才能升级斗魂");
		return false;
	}

	if (pItemUp->GetObject()->douhun >= 15)
	{
		m_pStaResult->SetText("            斗魂等级已经最高，不能再升级");
		return false;
	}

	char szInfo[MAX_PATH] = {0};
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 补天镶嵌
bool CDlgEquipRecast::CanComposeEquip3()
{
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("      单件装备配合对应的补天石镶嵌");
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
		if ( (vecMaterialItems[i]->GetObjectID() >= 52000 && vecMaterialItems[i]->GetObjectID() <= 52005))
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (6 > vecMaterialItems[i]->GetObject()->butian))
		{
			++nItemUp;
			pItemUp = vecMaterialItems[i];
			continue;
		}

		m_pStaResult->SetText("            存在非镶嵌类道具");
		return false;
	}

	if ((nItemUp + nItemChange + nItemRate) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("            存在非镶嵌类道具");
		return false;
	}

	if (nItemUp == 1) //有装备
	{
		if (!(pItemUp->GetObject()->kind & 4 || pItemUp->GetObject()->kind & 16))
		{
			m_pStaResult->SetText("            只有70级以上的绿色或神圣祝福装备才能镶嵌");
			return false;
		}
		if (pItemUp->GetObject()->needlevel < 70)
		{
			m_pStaResult->SetText("            70级以下的装备不能镶嵌神石");
			return false;
		}
		else  
		{
			if (nItemChange == 0)
			{
				if (pItemUp->GetObject()->butian == 4)
				{
					 if (pItemUp->GetItemType() >= 101 && pItemUp->GetItemType() <= 118 || (pItemUp->GetItemType() >= 130 && pItemUp->GetItemType() <= 147 || pItemUp->GetItemType() == 155 || pItemUp->GetItemType() == 156))
					{
						m_pStaResult->SetText("        当前装备镶嵌需要弑天圣石");
						return false;
					}
				}
				else if (pItemUp->GetObject()->butian == 3)	
				{
					 if (pItemUp->GetItemType() >= 101 && pItemUp->GetItemType() <= 118 || (pItemUp->GetItemType() >= 130 && pItemUp->GetItemType() <= 147 || pItemUp->GetItemType() == 155 || pItemUp->GetItemType() == 156))
					{
						m_pStaResult->SetText("        当前装备镶嵌需要帝王神石");
						return false;
					}
				}
				else if (pItemUp->GetObject()->butian == 2)
				{
					 if (pItemUp->GetItemType() >= 101 && pItemUp->GetItemType() <= 118 || (pItemUp->GetItemType() >= 130 && pItemUp->GetItemType() <= 147 || pItemUp->GetItemType() == 155 || pItemUp->GetItemType() == 156))
					{
						m_pStaResult->SetText("        当前装备镶嵌需要补天神石");
						return false;
					}
				}
				else if (pItemUp->GetObject()->butian == 1)
				{
					 if (pItemUp->GetItemType() >= 101 && pItemUp->GetItemType() <= 118 || (pItemUp->GetItemType() >= 130 && pItemUp->GetItemType() <= 147 || pItemUp->GetItemType() == 155 || pItemUp->GetItemType() == 156))
					{
						m_pStaResult->SetText("        当前装备镶嵌需要补天灵石");
						return false;
					}
				}
				else if (pItemUp->GetObject()->butian == 0)
				{
					 if (pItemUp->GetItemType() >= 101 && pItemUp->GetItemType() <= 118 || (pItemUp->GetItemType() >= 130 && pItemUp->GetItemType() <= 147 || pItemUp->GetItemType() == 155 || pItemUp->GetItemType() == 156))
					{
						m_pStaResult->SetText("        当前装备镶嵌需要补天石");
						return false;
					}
				}
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("            单件装备配合对应的补天才能镶嵌");
		return false;
	}

	if (pItemUp->GetObject()->butian >= 5)
	{
		m_pStaResult->SetText("            补天等级已经最高，不能再镶嵌");
		return false;
	}

	char szInfo[MAX_PATH] = {0};
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 装备栏激活
bool CDlgEquipRecast::CanComposeEquip4()
{
	if(GetScene()->GetMainCharacter()->GetLevel()<197)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		m_pStaResult->SetTextColor(dwColor);
		m_pStaResult->SetText("197级开放装备栏激活功能");
		return false;
	}
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("神圣祝福装备配合装备栏属性石可激活装备栏");
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
		//sky 放入材料计数
		if (vecMaterialItems[i]->GetObjectID() == 5310 )
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) &&(3 >= vecMaterialItems[i]->GetObject()->retain2))
		{
			++nItemUp;
			pItemUp = vecMaterialItems[i];
			continue;
		}

		m_pStaResult->SetText("存在非激活类道具");
		return false;
	}

	//sky 放入物品数量判断
	if ((nItemUp + nItemChange + nItemRate ) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("请放入装备栏属性石");
		return false;
	}

	//sky 放入装备
	if (nItemUp == 1)
	{
		if (!(pItemUp->GetObject()->kind & 16))
		{
			m_pStaResult->SetText("70级以上神圣祝福装备才能激活装备栏");
			return false;
		}
		if (pItemUp->GetObject()->needlevel < 70)
		{
			m_pStaResult->SetText("70级以下的装备不能激活装备栏");
			return false;
		}
		else
		{
			//sky 根据装备栏等级判断需要的进阶材料
			if (pItemUp->GetObject()->retain2 <= 0 && nItemChange == 0)
			{
				m_pStaResult->SetText("当前装备需要装备栏属性石激活");
				return false;
			}
		}
	}

	//sky 没放入装备
	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("装备配合装备栏属性石可激活装备栏");
		return false;
	}

	//sky 最高等级
	if (pItemUp->GetObject()->retain2 > 0)
	{
		m_pStaResult->SetText("当前装备栏已经激活");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 装备栏进阶
bool CDlgEquipRecast::CanComposeEquip5()
{
	if(GetScene()->GetMainCharacter()->GetLevel()<197)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		m_pStaResult->SetTextColor(dwColor);
		m_pStaResult->SetText("197级开放装备栏进阶功能");
		return false;
	}
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("神圣祝福装备配合装备栏进阶石可提升装备栏等级");
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
		if (vecMaterialItems[i]->GetObjectID() == 5311) //装备栏进阶石
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) &&(8 > vecMaterialItems[i]->GetObject()->retain2))
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

	if (nItemUp == 1)
	{
		if (!(pItemUp->GetObject()->kind & 16) && pItemUp->GetObject()->retain2 < 1 )
		{
			m_pStaResult->SetText("70级以上已激活装备栏的神圣装备才能进阶装备栏");
			return false;
		}

		//sky 判断
		if (pItemUp->GetObject()->retain2 < 1 )
		{
			m_pStaResult->SetText("你的装备没有激活装备栏");
			return false;
		}

		if (pItemUp->GetObject()->needlevel < 70)
		{
			m_pStaResult->SetText("70级以下的装备不能进阶装备栏");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				m_pStaResult->SetText("70级及以上的装备需装备栏进阶石进阶");
				return false;
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合装备栏进阶石才能进阶");
		return false;
	}

	if (pItemUp->GetObject()->retain2 >= 6)
	{
		m_pStaResult->SetText("当前装备栏等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 神器强化
bool CDlgEquipRecast::CanComposeEquip6()
{
	if(GetScene()->GetMainCharacter()->GetLevel()<170)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		m_pStaResult->SetTextColor(dwColor);
		m_pStaResult->SetText("170级开放神器强化功能");
		return false;
	}

	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("神圣祝福装备需要神器碎片升级神器");
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
		if (vecMaterialItems[i]->GetObjectID() == 3812) //神器碎片
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) &&(7 > vecMaterialItems[i]->GetObject()->retain3))
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

	if (nItemUp == 1)
	{
		if (!(pItemUp->GetObject()->kind & 16) )
		{
			m_pStaResult->SetText("只有70级以上神圣祝福装备才能升级神器");
			return false;
		}


		if (pItemUp->GetObject()->needlevel < 70)
		{
			m_pStaResult->SetText("70级以下的装备不能升级神器");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				m_pStaResult->SetText("70级及以上的装备需神器碎片升级神器");
				return false;
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合神器碎片才能升级神器");
		return false;
	}

	if (pItemUp->GetObject()->retain3 >= 7)
	{
		m_pStaResult->SetText("神器等级已经最高，不能再升级");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 龙槽激活
bool CDlgEquipRecast::CanComposeEquip7()
{
	if(GetScene()->GetMainCharacter()->GetLevel()<197)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		m_pStaResult->SetTextColor(dwColor);
		m_pStaResult->SetText("197级开放龙星功能");
		return false;
	}
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("160级以上神圣祝福15星装备可以激活龙星");
		return false;
	}

	CRoleItem* pItemUp = NULL;
	int nItemUp = 0;
	int nItemC0 = 0; //龙星激活石

	DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
	m_pStaResult->SetTextColor(dwColor);

	for (int i = 0; i < vecMaterialItems.size(); ++i)
	{
		//sky 放入材料计数
		if (vecMaterialItems[i]->GetObjectID() == 1140 ) //龙星激活石
		{
			nItemC0++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) &&(2 >= vecMaterialItems[i]->GetObject()->dtrough))
		{
			++nItemUp;
			pItemUp = vecMaterialItems[i];
			continue;
		}

		m_pStaResult->SetText("存在非激活类道具");
		return false;
	}

	//sky 放入物品数量判断
	if ((nItemUp + nItemC0 ) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("请放入龙星激活石");
		return false;
	}

	//sky 放入装备
	if (nItemUp == 1)
	{
		if (!(pItemUp->GetObject()->kind & 16) && (pItemUp->GetObject()->upgrade < 15))
		{
			m_pStaResult->SetText("160级以上神圣祝福15星装备才能激活龙星");
			return false;
		}
		if (pItemUp->GetObject()->needlevel < 160)
		{
			m_pStaResult->SetText("160级以下的装备不能激活龙星");
			return false;
		}
		else
		{
			//sky 根据龙槽等级判断需要的解封材料
			if (pItemUp->GetObject()->dtrough <= 1 && nItemC0 == 0)
			{
				m_pStaResult->SetText("当前装备需要 龙星激活石 激活");
				return false;
			}
		}
	}

	//sky 没放入装备
	if ((nItemUp != 1) || ((nItemC0 ) < 1))
	{
		m_pStaResult->SetText("装备配合 龙星激活石 可激活 龙槽*1 ");
		return false;
	}

	//sky 龙槽最高等级
	if (pItemUp->GetObject()->dtrough > 0)
	{
		m_pStaResult->SetText("当前装备已经激活龙槽");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：1锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//sky 龙槽解封
bool CDlgEquipRecast::CanComposeEquip8()
{
	if(GetScene()->GetMainCharacter()->GetLevel()<197)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		m_pStaResult->SetTextColor(dwColor);
		m_pStaResult->SetText("197级开放龙星功能");
		return false;
	}
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("已激活1孔龙槽的装备可在此解封龙槽等级");
		return false;
	}

	CRoleItem* pItemUp = NULL;
	int nItemUp = 0;
	int nItemUPMAX = 0; //龙槽解封材料计数


	DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
	m_pStaResult->SetTextColor(dwColor);

	for (int i = 0; i < vecMaterialItems.size(); ++i)
	{
		//sky 放入材料计数
		if (vecMaterialItems[i]->GetObjectID() == 1141) //龙槽解封
		{
			nItemUPMAX++;
			continue;
		}	

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) &&(17 > vecMaterialItems[i]->GetObject()->dtrough))
		{
			++nItemUp;
			pItemUp = vecMaterialItems[i];
			continue;
		}

		m_pStaResult->SetText("存在非解封类道具");
		return false;
	}

	//sky 放入物品数量判断
	if ((nItemUp + nItemUPMAX) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("请放入对应的物品");
		return false;
	}

	//sky 放入装备
	if (nItemUp == 1)
	{
		if (!(pItemUp->GetObject()->kind & 16) && (pItemUp->GetObject()->upgrade < 15) && (pItemUp->GetObject()->dtrough >= 1))
		{
			m_pStaResult->SetText("160级以上神圣祝福15星装备已激活龙槽x1才能解封等级");
			return false;
		}
		//sky 龙星等级小于龙槽等级
		if (pItemUp->GetObject()->dtrough < 15 && (pItemUp->GetObject()->dtrough >= pItemUp->GetObject()->upgrade))
		{
			m_pStaResult->SetText("请提升装备星星等级");
			return false;
		}
		if (pItemUp->GetObject()->needlevel < 160)
		{
			m_pStaResult->SetText("160级以下的装备不能解封龙槽");
			return false;
		}
		else
		{
			//sky 根据龙槽等级判断需要的解封材料
			if (pItemUp->GetObject()->dtrough == 15)
			{
				m_pStaResult->SetText("当前装备不需要解封龙槽等级");
				return false;
			}
			else if (pItemUp->GetObject()->dtrough <= 14 && nItemUPMAX == 0)
			{
				m_pStaResult->SetText("当前装备解封需要龙槽解封石");
				return false;
			}		
		}
	}

	//sky 没放入装备
	if ((nItemUp != 1) || ((nItemUPMAX) < 1))
	{
		m_pStaResult->SetText("装备配合对应的解封石可解封龙槽等级");
		return false;
	}

	//sky 龙槽最高等级
	if (pItemUp->GetObject()->dtrough >= 15)
	{
		m_pStaResult->SetText("当前装备已经达到15龙槽");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：1锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//sky 龙星提升
bool CDlgEquipRecast::CanComposeEquip9()
{
	if(GetScene()->GetMainCharacter()->GetLevel()<197)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		m_pStaResult->SetTextColor(dwColor);
		m_pStaResult->SetText("197级开放龙星功能");
		return false;
	}
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("已激活龙槽的神圣祝福装备可以用龙星升级宝石提升等级");
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
		if (vecMaterialItems[i]->GetObjectID() == 1153 || vecMaterialItems[i]->GetObjectID() == 2975) //龙星升级宝石
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) &&(17 > vecMaterialItems[i]->GetObject()->drastar))
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

	if (nItemUp == 1)
	{
		if (!(pItemUp->GetObject()->kind & 16) && pItemUp->GetObject()->dtrough <= 0 )
		{
			m_pStaResult->SetText("只有160级以上已激活龙槽的紫装才能升级龙星等级");
			return false;
		}

		//sky 龙星等级小于龙槽等级  
		if (pItemUp->GetObject()->dtrough < 15 && (pItemUp->GetObject()->drastar >= pItemUp->GetObject()->dtrough))
		{
			m_pStaResult->SetText("请提升龙槽等级");
			return false;
		}

		if (pItemUp->GetObject()->needlevel < 160)
		{
			m_pStaResult->SetText("160级以下的装备不能升级");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				m_pStaResult->SetText("160级及以上的装备需龙星升级宝石升级");
				return false;
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合龙星升级宝石才能升级");
		return false;
	}

	if (pItemUp->GetObject()->drastar >= 15)
	{
		m_pStaResult->SetText("龙星等级已经最高，不能再升级");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：1锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 魂魄清除
bool CDlgEquipRecast::CanComposeEquip10()
{
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("      神圣装备配合魂魄原石可清除魂魄");
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
		if ( vecMaterialItems[i]->GetObjectID() == 2329)
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp>0))
		{
			++nItemUp;
			pItemUp = vecMaterialItems[i];
			continue;
		}

		m_pStaResult->SetText("            存在非清除类道具");
		return false;
	}

	if ((nItemUp + nItemChange + nItemRate) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("            存在非清除类道具");
		return false;
	}

	if (nItemUp == 1) //有装备
	{
		if (!(pItemUp->GetObject()->kind & 4 || pItemUp->GetObject()->kind & 16))
		{
			m_pStaResult->SetText("            神圣装备才能在此清除魂魄");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				m_pStaResult->SetText("当前装备清除魂魄需要 魂魄原石");
				return false;

			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("            神圣装备配合魂魄原石可清除魂魄");
		return false;
	}

	//for( int i=0;i<6;i++ )
	//{
		if (pItemUp->GetObject()->socket[6] == 0 )
		{
			m_pStaResult->SetText("            当前装备不需要清除魂魄");
			return false;
		}
	//}

	char szInfo[MAX_PATH] = {0};
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 装备升级
bool CDlgEquipRecast::CanComposeEquip11()
{
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("      70级以上装备配合 对应的升级图纸 材料可升级装备等级");
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
		if ( vecMaterialItems[i]->GetObjectID() >= 2101 && vecMaterialItems[i]->GetObjectID() <= 2125)//程序头装备升级需求道具ID
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (216 > vecMaterialItems[i]->GetObject()->needlevel))
		{
			++nItemUp;
			pItemUp = vecMaterialItems[i];
			continue;
		}

		m_pStaResult->SetText("            存在非升级类道具");
		return false;
	}

	if ((nItemUp + nItemChange + nItemRate) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("            存在非升级类道具");
		return false;
	}
	
	
	if (nItemUp == 1) //有装备
	{
		if (pItemUp->GetObject()->needlevel < 70)
		{
			m_pStaResult->SetText("            70级以下的装备不能升级");
			return false;
		}
		else  
		{
			
			if (nItemChange == 0)
			{
				 //soke 70级以上装备升级 - 甲(男)
			if (pItemUp->GetObject()->dwObjectID ==  10007)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10008)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10009)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10010)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10011)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10012)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10013)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10014)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10015)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10016)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10017)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10018)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10019)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10020)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10021)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10022)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10023)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10024)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10025)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10026)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10027)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10028)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10029)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 甲(女)
			if (pItemUp->GetObject()->dwObjectID ==  10057)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10058)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10059)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10060)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10061)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10062)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10063)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10064)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10065)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10066)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10067)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10068)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10069)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10070)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10071)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10072)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10073)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10074)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10075)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10076)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10077)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10078)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10079)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}										

            //soke 70级以上装备升级 - 铠(男)
			if (pItemUp->GetObject()->dwObjectID ==  10107)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10108)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10109)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10110)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10111)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10112)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10113)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10114)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10115)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10116)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10117)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10118)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10119)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10120)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10121)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10122)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10123)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10124)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10125)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10126)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10127)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10128)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10129)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}										

            //soke 70级以上装备升级 - 铠(女)
			if (pItemUp->GetObject()->dwObjectID ==  10157)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10158)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10159)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10160)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10161)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10162)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10163)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10164)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10165)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10166)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10167)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10168)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10169)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10170)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10171)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10172)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10173)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10174)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10175)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10176)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10177)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10178)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10179)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(衣服) x1");
				return false;
			}							

              //soke 70级以上装备升级 - 刀
			if (pItemUp->GetObject()->dwObjectID ==  10207)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10208)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10209)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10210)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10211)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10212)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10213)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10214)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10215)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10216)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10217)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10218)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10219)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10220)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10221)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10222)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10223)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10224)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10225)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10226)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10227)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10228)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10229)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刀) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 仗
			if (pItemUp->GetObject()->dwObjectID ==  10257)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10258)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10259)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10260)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10261)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10262)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10263)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10264)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10265)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10266)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10267)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10268)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10269)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10270)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10271)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10272)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10273)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10274)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10275)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10276)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10277)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10278)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10279)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(仗) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 弓
			if (pItemUp->GetObject()->dwObjectID ==  10307)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10308)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10309)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10310)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10311)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10312)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10313)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10314)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10315)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10316)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10317)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10318)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10319)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10320)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10321)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10322)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10323)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10324)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10325)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10326)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10327)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10328)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10329)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(弓) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 扇
			if (pItemUp->GetObject()->dwObjectID ==  10357)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10358)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10359)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10360)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10361)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10362)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10363)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10364)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10365)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10366)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10367)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10368)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10369)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10370)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10371)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10372)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10373)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10374)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10375)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10376)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10377)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10378)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10379)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(扇) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 棍
			if (pItemUp->GetObject()->dwObjectID ==  10407)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10408)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10409)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10410)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10411)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10412)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10413)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10414)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10415)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10416)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10417)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10418)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10419)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10420)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10421)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10422)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10423)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10424)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10425)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10426)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10427)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10428)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10429)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(棍) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 剑(物)
			if (pItemUp->GetObject()->dwObjectID ==  10457)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10458)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10459)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10460)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10461)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10462)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10463)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10464)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10465)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10466)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10467)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10468)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10469)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10470)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10471)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10472)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10473)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10474)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10475)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10476)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10477)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10478)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10479)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}	
			
			//soke 70级以上装备升级 - 剑(魔)
			if (pItemUp->GetObject()->dwObjectID ==  10507)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10508)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10509)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10510)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10511)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10512)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10513)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10514)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10515)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10516)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10517)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10518)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10519)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10520)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10521)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10522)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10523)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10524)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10525)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10526)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10527)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10528)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10529)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(剑) x1");
				return false;
			}									

 		
            //soke 70级以上装备升级 - 刃(魔)
			if (pItemUp->GetObject()->dwObjectID ==  10557)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10558)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10559)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10560)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10561)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10562)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10563)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10564)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10565)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10566)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10567)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10568)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10569)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10570)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10571)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10572)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10573)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10574)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10575)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10576)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10577)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10578)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10579)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(刃) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 枪(物)
			if (pItemUp->GetObject()->dwObjectID ==  10607)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10608)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10609)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10610)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10611)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10612)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10613)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10614)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10615)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10616)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10617)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10618)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10619)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10620)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10621)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10622)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10623)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10624)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10625)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10626)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10627)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10628)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10629)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 枪(魔)
			if (pItemUp->GetObject()->dwObjectID ==  10657)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10658)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10659)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10660)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10661)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10662)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10663)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10664)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10665)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10666)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10667)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10668)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10669)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10670)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10671)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10672)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10673)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10674)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10675)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10676)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10677)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10678)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10679)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(枪) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 盾
			if (pItemUp->GetObject()->dwObjectID ==  10757)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10758)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10759)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10760)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10761)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10762)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10763)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10764)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10765)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10766)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10767)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10768)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10769)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10770)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10771)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10772)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10773)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10774)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10775)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10776)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10777)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10778)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10779)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1");
				return false;
			}							

         	//soke 70级以上装备升级 - 战旗(法术)
			if (pItemUp->GetObject()->dwObjectID ==  12007)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12008)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12009)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12010)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12011)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12012)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12013)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12014)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12015)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12016)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12017)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12018)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12019)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12020)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12021)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12022)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12023)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12024)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12025)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12026)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12027)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12028)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12029)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}

	        //soke 70级以上装备升级 - 战旗(弓手)
			if (pItemUp->GetObject()->dwObjectID ==  12057)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12058)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12059)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12060)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12061)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12062)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12063)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12064)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12065)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12066)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12067)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12068)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12069)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12070)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12071)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12072)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12073)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12074)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12075)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12076)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12077)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12078)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12079)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}

           	//soke 70级以上装备升级 - 战旗(仙术)
			if (pItemUp->GetObject()->dwObjectID ==  12107)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12108)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12109)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12110)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12111)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12112)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12113)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12114)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12115)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12116)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12117)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12118)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12119)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12120)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12121)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12122)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12123)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12124)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12125)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12126)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12127)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12128)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12129)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}

	        //soke 70级以上装备升级 - 战旗(召唤)
			if (pItemUp->GetObject()->dwObjectID ==  12157)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12158)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12159)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12160)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12161)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12162)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12163)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12164)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12165)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12166)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12167)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12168)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12169)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12170)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12171)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12172)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12173)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12174)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12175)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12176)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12177)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12178)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12179)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}

           	//soke 70级以上装备升级 - 护心镜(物)
			if (pItemUp->GetObject()->dwObjectID ==  12207)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12208)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12209)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12210)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12211)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12212)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12213)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12214)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12215)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12216)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12217)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12218)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12219)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12220)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12221)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12222)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12223)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12224)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12225)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12226)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12227)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12228)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12229)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}

	        //soke 70级以上装备升级 - 护心镜(魔)
			if (pItemUp->GetObject()->dwObjectID ==  12257)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12258)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12259)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12260)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12261)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12262)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12263)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12264)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12265)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12266)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12267)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12268)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12269)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12270)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12271)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12272)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12273)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12274)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12275)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12276)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12277)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12278)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12279)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}

			//soke 70级以上装备升级 - 短枪(物)
			if (pItemUp->GetObject()->dwObjectID ==  12307)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12308)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12309)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12310)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12311)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12312)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12313)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12314)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12315)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12316)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12317)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12318)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12319)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12320)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12321)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12322)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12323)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12324)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12325)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12326)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12327)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12328)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12329)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}						

            //soke 70级以上装备升级 - 短枪(魔)
			if (pItemUp->GetObject()->dwObjectID ==  12357)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12358)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12359)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12360)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12361)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12362)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12363)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12364)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12365)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12366)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12367)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12368)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12369)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12370)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12371)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12372)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12373)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12374)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12375)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12376)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12377)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12378)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12379)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}

			//soke 70级以上装备升级 - 匕首(魔)
			if (pItemUp->GetObject()->dwObjectID ==  12407)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12408)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12409)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12410)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12411)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12412)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12413)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12414)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12415)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12416)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12417)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12418)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12419)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12420)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12421)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12422)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12423)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12424)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12425)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12426)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12427)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12428)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  12429)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(副手) x1"); 
				return false;
			}	

            //soke 70级以上装备升级 - 头盔(魔)
			if (pItemUp->GetObject()->dwObjectID ==  10907)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10908)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10909)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10910)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10911)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10912)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10913)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10914)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10915)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10916)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10917)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10918)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10919)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10920)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10921)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10922)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10923)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10924)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10925)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10926)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10927)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10928)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10929)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 头盔(物)
			if (pItemUp->GetObject()->dwObjectID ==  10957)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10958)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10959)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10960)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10961)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10962)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10963)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10964)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10965)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10966)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10967)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10968)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10969)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10970)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10971)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10972)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10973)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10974)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10975)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10976)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10977)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10978)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  10979)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(头盔) x1");
				return false;
			}							

  			//soke 70级以上装备升级 - 腰带(魔)
			if (pItemUp->GetObject()->dwObjectID ==  11007)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11008)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11009)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11010)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11011)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11012)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11013)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11014)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11015)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11016)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11017)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11018)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11019)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11020)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11021)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11022)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11023)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11024)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11025)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11026)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11027)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11028)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11029)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 腰带(物)
			if (pItemUp->GetObject()->dwObjectID ==  11057)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11058)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11059)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11060)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11061)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11062)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11063)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11064)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11065)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11066)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11067)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11068)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11069)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11070)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11071)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11072)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11073)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11074)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11075)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11076)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11077)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11078)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11079)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(腰带) x1");
				return false;
			}				

            //soke 70级以上装备升级 - 护腕(魔)
			if (pItemUp->GetObject()->dwObjectID ==  11107)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11108)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11109)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11110)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11111)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11112)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11113)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11114)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11115)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11116)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11117)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11118)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11119)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11120)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11121)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11122)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11123)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11124)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11125)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11126)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11127)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11128)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11129)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 护腕(物)
			if (pItemUp->GetObject()->dwObjectID ==  11157)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11158)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11159)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11160)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11161)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11162)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11163)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11164)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11165)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11166)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11167)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11168)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11169)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11170)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11171)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11172)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11173)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11174)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11175)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11176)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11177)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11178)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11179)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(护腕) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 靴(物)
			if (pItemUp->GetObject()->dwObjectID ==  11207)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11208)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11209)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11210)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11211)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11212)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11213)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11214)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11215)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11216)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11217)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11218)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11219)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11220)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11221)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11222)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11223)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11224)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11225)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11226)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11227)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11228)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11229)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 靴(魔)
			if (pItemUp->GetObject()->dwObjectID ==  11257)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11258)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11259)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11260)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11261)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11262)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11263)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11264)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11265)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11266)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11267)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11268)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11269)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11270)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11271)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11272)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11273)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11274)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11275)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11276)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11277)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11278)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11279)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(靴子) x1");
				return false;
			}

            
            //soke 70级以上装备升级 - 项链(物)
			if (pItemUp->GetObject()->dwObjectID ==  11307)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11308)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11309)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11310)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11311)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11312)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11313)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11314)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11315)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11316)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11317)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11318)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11319)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11320)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11321)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11322)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11323)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11324)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11325)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11326)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11327)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11328)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11329)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 项链(魔)
			if (pItemUp->GetObject()->dwObjectID ==  11357)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11358)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11359)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11360)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11361)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11362)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11363)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11364)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11365)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11366)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11367)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11368)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11369)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11370)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11371)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11372)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11373)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11374)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11375)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11376)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11377)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11378)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11379)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(项链) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 戒指(物)
			if (pItemUp->GetObject()->dwObjectID ==  11407)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11408)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11409)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11410)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11411)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11412)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11413)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11414)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11415)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11416)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11417)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11418)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11419)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11420)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11421)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11422)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11423)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11424)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11425)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11426)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11427)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11428)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11429)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}							

            //soke 70级以上装备升级 - 戒指(魔)
			if (pItemUp->GetObject()->dwObjectID ==  11457)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11458)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11459)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11460)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11461)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11462)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11463)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11464)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11465)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11466)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11467)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11468)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11469)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11470)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11471)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11472)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11473)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11474)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11475)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11476)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11477)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11478)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  11479)
			{
				m_pStaResult->SetText(" 当前装备升级所需： 装备升级图纸(戒指) x1");
				return false;
			}							

			//soke 70级以上装备升级 - 蹄铁
			if (pItemUp->GetObject()->dwObjectID ==  16101)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16102)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16103)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16104)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16105)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16106)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16107)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16108)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16109)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马蹄铁) x1");
				return false;
			}
			//soke 70级以上装备升级 - 缰绳
			if (pItemUp->GetObject()->dwObjectID ==  16201)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16202)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16203)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16204)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16205)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16206)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16207)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16208)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16209)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马缰绳) x1");
				return false;
			}
			//soke 70级以上装备升级 - 金鞍
			if (pItemUp->GetObject()->dwObjectID ==  16301)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马鞍） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16302)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马鞍） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16303)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马鞍） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16304)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马鞍） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16305)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马鞍） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16306)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马鞍） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16307)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马鞍） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16308)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马鞍） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16309)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马鞍） x1");
				return false;
			}
			//soke 70级以上装备升级 - 护甲
			if (pItemUp->GetObject()->dwObjectID ==  16401)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16402)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16403)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16404)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16405)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16406)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16407)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16408)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16409)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(护马甲） x1");
				return false;
			}
			//soke 70级以上装备升级 - 马镫
			if (pItemUp->GetObject()->dwObjectID ==  16501)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马镫） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16502)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马镫） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16503)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马镫） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16504)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马镫） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16505)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马镫） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16506)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马镫） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16507)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马镫） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16508)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马镫） x1");
				return false;
			}
			else if (pItemUp->GetObject()->dwObjectID ==  16509)
			{
				m_pStaResult->SetText(" 当前坐骑装备升级所需： 坐骑装备升级图纸(马镫） x1");
				return false;
			}	                                                           
				
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("            70级以上装备配合对应的材料可升级");
		return false;
	}

	if (pItemUp->GetObject()->needlevel >= 202)
	{
		m_pStaResult->SetText("           当前装备已经达到升级上限");
		return false;
	}

	char szInfo[MAX_PATH] = {0};
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 马牌精炼
bool CDlgEquipRecast::CanComposeEquip12()
{
	if(GetScene()->GetMainCharacter()->GetLevel()<170)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		m_pStaResult->SetTextColor(dwColor);
		m_pStaResult->SetText("170级开放马牌精炼功能");
		return false;
	}
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("马牌精炼装备附加属性需要:玄麟精魄");
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
        if ((vecMaterialItems[i]->GetObjectID() >= 3843 && vecMaterialItems[i]->GetObjectID() <= 3844))	 //tz 精炼石	
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0))
		{
			++nItemUp;
			pItemUp = vecMaterialItems[i];
			continue;
		}

		m_pStaResult->SetText("存在非精炼类道具");
		return false;
	}

	if ((nItemUp + nItemChange + nItemRate) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("存在非精炼类道具");
		return false;
	}

	if (nItemUp == 1)
	{
		if (!(pItemUp->GetObject()->dwObjectID == 15601) ) //tz特殊马牌马牌精炼
		{
			m_pStaResult->SetText("只有马牌才能进行马牌精炼");
			return false;
		}


		if (pItemUp->GetObject()->needlevel < 70)
		{
			m_pStaResult->SetText("70级以下的装备不能进行马牌精炼");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				m_pStaResult->SetText("马牌精炼需要：玄麟精魄");
				return false;
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合玄麟精魄才能精炼装备");
		return false;
	}

	if (pItemUp->GetObject()->itemrefine >= 2000)
	{
		m_pStaResult->SetText("马牌精炼等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	
	return true;
}

//soke 神龙镶嵌
bool CDlgEquipRecast::CanComposeEquip13()
{
	if(GetScene()->GetMainCharacter()->GetLevel()<197)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		m_pStaResult->SetTextColor(dwColor);
		m_pStaResult->SetText("197级开放神龙镶嵌功能");
		return false;
	}
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("      单件装备配合对应的神龙宝宝镶嵌");
		return false;
	}

	CRoleItem* pItemUp = NULL;
	int nItemUp = 0;
	int nItemChange = 0;
	int nItemRate = 0;

	DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
	m_pStaResult->SetTextColor(dwColor);

	for (int i = 0; i < vecMaterialItems.size(); ++i)
	{    //神龙镶嵌材料指定
		if ( (vecMaterialItems[i]->GetObjectID() >= 5301 && vecMaterialItems[i]->GetObjectID() <= 5307)) 
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (7 > vecMaterialItems[i]->GetObject()->longwanglv))
		{
			++nItemUp;
			pItemUp = vecMaterialItems[i];
			continue;
		}

		m_pStaResult->SetText("            存在非镶嵌类道具");
		return false;
	}

	if ((nItemUp + nItemChange + nItemRate) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("            存在非镶嵌类道具");
		return false;
	}

	if (nItemUp == 1) //有装备
	{
		if (!(pItemUp->GetObject()->kind & 4 || pItemUp->GetObject()->kind & 16))
		{
			m_pStaResult->SetText("            只有70级以上的绿色或神圣祝福装备才能镶嵌");
			return false;
		}
		if (pItemUp->GetObject()->needlevel < 70)
		{
			m_pStaResult->SetText("            70级以下的装备不能镶嵌神龙");
			return false;
		}
		else  
		{
			if (nItemChange == 0)
			{
				if (pItemUp->GetObject()->longwanglv == 5)
				{
					 if (pItemUp->GetItemType() >= 101 && pItemUp->GetItemType() <= 118 || (pItemUp->GetItemType() >= 130 && pItemUp->GetItemType() <= 147 || pItemUp->GetItemType() == 155 || pItemUp->GetItemType() == 156))
					{
						m_pStaResult->SetText("        当前装备镶嵌需要神巨龙宝宝");
						return false;
					}
				}
                if (pItemUp->GetObject()->longwanglv == 4)
				{
					 if (pItemUp->GetItemType() >= 101 && pItemUp->GetItemType() <= 118 || (pItemUp->GetItemType() >= 130 && pItemUp->GetItemType() <= 147 || pItemUp->GetItemType() == 155 || pItemUp->GetItemType() == 156))
					{
						m_pStaResult->SetText("        当前装备镶嵌需要神苍龙宝宝");
						return false;
					}
				}
				else if (pItemUp->GetObject()->longwanglv == 3)	
				{
					 if (pItemUp->GetItemType() >= 101 && pItemUp->GetItemType() <= 118 || (pItemUp->GetItemType() >= 130 && pItemUp->GetItemType() <= 147 || pItemUp->GetItemType() == 155 || pItemUp->GetItemType() == 156))
					{
						m_pStaResult->SetText("        当前装备镶嵌需要巨龙宝宝");
						return false;
					}
				}
				else if (pItemUp->GetObject()->longwanglv == 2)
				{
					 if (pItemUp->GetItemType() >= 101 && pItemUp->GetItemType() <= 118 || (pItemUp->GetItemType() >= 130 && pItemUp->GetItemType() <= 147 || pItemUp->GetItemType() == 155 || pItemUp->GetItemType() == 156))
					{
						m_pStaResult->SetText("        当前装备镶嵌需要苍龙宝宝");
						return false;
					}
				}
				else if (pItemUp->GetObject()->longwanglv == 1)
				{
					 if (pItemUp->GetItemType() >= 101 && pItemUp->GetItemType() <= 118 || (pItemUp->GetItemType() >= 130 && pItemUp->GetItemType() <= 147 || pItemUp->GetItemType() == 155 || pItemUp->GetItemType() == 156))
					{
						m_pStaResult->SetText("        当前装备镶嵌需要金龙宝宝");
						return false;
					}
				}
				else if (pItemUp->GetObject()->longwanglv == 0)
				{
					 if (pItemUp->GetItemType() >= 101 && pItemUp->GetItemType() <= 118 || (pItemUp->GetItemType() >= 130 && pItemUp->GetItemType() <= 147 || pItemUp->GetItemType() == 155 || pItemUp->GetItemType() == 156))
					{
						m_pStaResult->SetText("        当前装备镶嵌需要小龙宝宝");
						return false;
					}
				}
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("            单件装备配合对应的龙宝宝才能镶嵌");
		return false;
	}

	if (pItemUp->GetObject()->longwanglv >= 6)
	{
		m_pStaResult->SetText("            龙王降临等级已经最高，不能再镶嵌");
		return false;
	}

	char szInfo[MAX_PATH] = {0};
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 装备逆天进阶
bool CDlgEquipRecast::CanComposeEquip14()
{
	if(GetScene()->GetMainCharacter()->GetLevel()<197)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		m_pStaResult->SetTextColor(dwColor);
		m_pStaResult->SetText("197级开放逆天进阶功能");
		return false;
	}
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("装备附加逆天属性需要:逆天进阶石");
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
		if (vecMaterialItems[i]->GetObjectID() == 5312) //逆天进阶石
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (101 > vecMaterialItems[i]->GetObject()->nitianlv))
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

	if (nItemUp == 1)
	{
		if (!(pItemUp->GetObject()->kind & 16) )
		{
			m_pStaResult->SetText("只有70级以上神圣祝福装备才能进行装备逆天进阶");
			return false;
		}


		if (pItemUp->GetObject()->needlevel < 70)
		{
			m_pStaResult->SetText("70级以下的装备不能进行装备逆天进阶");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				m_pStaResult->SetText("70级及以上的装备逆天进阶需要：逆天进阶石");
				return false;
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合逆天进阶石才能逆天进阶装备");
		return false;
	}

	if (pItemUp->GetObject()->nitianlv >= 100)
	{
		m_pStaResult->SetText("装备逆天进阶等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 装备斗战圣佛套装进阶
bool CDlgEquipRecast::CanComposeEquip15()
{
	if(GetScene()->GetMainCharacter()->GetLevel()<200)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		m_pStaResult->SetTextColor(dwColor);
		m_pStaResult->SetText("200级开放字纹镶嵌功能");
		return false;
	}
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("装备激活斗战圣佛套装属性：神魂宝石");
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
		if (vecMaterialItems[i]->GetObjectID() == 5313 || vecMaterialItems[i]->GetObjectID() == 5317) //神魂宝石
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (101 > vecMaterialItems[i]->GetObject()->douzhansflv))
		{
			++nItemUp;
			pItemUp = vecMaterialItems[i];
			continue;
		}

		m_pStaResult->SetText("存在非进阶类道具");
		return false;
	}

	if ((nItemUp + nItemChange + nItemRate) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("存在非进阶类道具");
		return false;
	}

	if (nItemUp == 1)
	{
		if (!(pItemUp->GetObject()->kind & 16) )
		{
			m_pStaResult->SetText("只有70级以上神圣祝福装备才能进行神魂宝石激活斗战圣佛属性词条");
			return false;
		}


		if (pItemUp->GetObject()->needlevel < 70)
		{
			m_pStaResult->SetText("70级以下的装备不能进行 神魂宝石镶嵌 激活斗战圣佛属性词条");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				m_pStaResult->SetText("70级及以上的装备需要：神魂宝石 来激活斗战圣佛属性词条");
				return false;
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合神魂宝石才能激活装备附加属性");
		return false;
	}

	if (pItemUp->GetObject()->douzhansflv >= 100)
	{
		m_pStaResult->SetText("装备斗战圣佛词条等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 装备暗影宝石镶嵌
bool CDlgEquipRecast::CanComposeEquip16()
{
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("装备激活暗影属性需要:暗影宝石");
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
		if (vecMaterialItems[i]->GetObjectID() == 5314) //暗影宝石
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (101 > vecMaterialItems[i]->GetObject()->aystonelv))
		{
			++nItemUp;
			pItemUp = vecMaterialItems[i];
			continue;
		}

		m_pStaResult->SetText("存在非镶嵌类道具");
		return false;
	}

	if ((nItemUp + nItemChange + nItemRate) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("存在非镶嵌类道具");
		return false;
	}

	if (nItemUp == 1)
	{
		if (!(pItemUp->GetObject()->kind & 16) )
		{
			m_pStaResult->SetText("只有70级以上神圣祝福装备才能进行暗影宝石镶嵌");
			return false;
		}


		if (pItemUp->GetObject()->needlevel < 70)
		{
			m_pStaResult->SetText("70级以下的装备不能进行暗影宝石镶嵌");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				m_pStaResult->SetText("70级及以上的装备需要：暗影宝石来镶嵌");
				return false;
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合暗影宝石才能激活装备附加属性");
		return false;
	}

	if (pItemUp->GetObject()->aystonelv >= 100)
	{
		m_pStaResult->SetText("装备暗影属性等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}


//sky 龙凤吟镶嵌
bool CDlgEquipRecast::CanComposeEquip17()
{
	if(GetScene()->GetMainCharacter()->GetLevel()<202)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		m_pStaResult->SetTextColor(dwColor);
		m_pStaResult->SetText("202级开放龙凤吟功能");
		return false;
	}
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("单件装备配合龙凤石可以升级");
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
		if (vecMaterialItems[i]->GetObjectID() == 1150 || vecMaterialItems[i]->GetObjectID() == 2976) //龙星升级宝石
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) )
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

	if (nItemUp == 1)
	{
		if (!(pItemUp->GetObject()->kind & 16))
		{
			m_pStaResult->SetText("只有紫装才能升级龙凤吟等级");
			return false;
		}

		
			if (nItemChange == 0)
			{
				m_pStaResult->SetText("单件装备配合龙凤石才能升级");
				return false;
			}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合龙凤石才能升级");
		return false;
	}

	if (pItemUp->GetObject()->retain24 >= 16)
	{
		m_pStaResult->SetText("龙凤吟等级已经最高，不能再升级");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}


//sky 轰天宝石镶嵌
bool CDlgEquipRecast::CanComposeEquip18()
{
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("单件装备配合轰天宝石才能镶嵌");
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
		if ((vecMaterialItems[i]->GetObjectID() >= 120037 && vecMaterialItems[i]->GetObjectID() <= 120045)  || vecMaterialItems[i]->GetObjectID() == 120046) //轰天系列道具
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) )
		{
			++nItemUp;
			pItemUp = vecMaterialItems[i];
			continue;
		}

		m_pStaResult->SetText("存在非轰天镶嵌类道具");
		return false;
	}

	if ((nItemUp + nItemChange + nItemRate) < vecMaterialItems.size())
	{
		m_pStaResult->SetText("存在非轰天镶嵌类道具");
		return false;
	}

	if (nItemUp == 1)
	{
		if (!(pItemUp->GetObject()->kind & 16))
		{
			m_pStaResult->SetText("只有紫装才能镶嵌轰天宝石");
			return false;
		}

		
			if (nItemChange == 0)
			{
				m_pStaResult->SetText("单件装备配合轰天宝石才能镶嵌");
				return false;
			}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合轰天宝石才能镶嵌");
		return false;
	}

	

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "此镶嵌不需要银子");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}



//soke 王者升级
bool CDlgEquipRecast::CanComposeEquip19()
{
	int nMaterialNum = 0;
	std::vector<CRoleItem*> vecMaterialItems;
	CRoleItem* pRoleItem = this->GetFocusItem();
	if (pRoleItem)
	{
		vecMaterialItems.push_back(pRoleItem);
		nMaterialNum += vecMaterialItems.back()->GetObject()->dwNum;
	}

	for (int i = 0; i < TOTAL_MATERIAL_TABLE; ++i)
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
		m_pStaResult->SetText("升级王者武器需要:王者碎片");
		return false;
	}

	CRoleItem* pItemUp = NULL;
	int nItemUp = 0;
	int nItemChange = 0;
	int nItemRate = 0;
	int jswsnum =0;
	DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
	m_pStaResult->SetTextColor(dwColor);

	for (int i = 0; i < vecMaterialItems.size(); ++i)
	{
		if (vecMaterialItems[i]->GetObjectID() == 50129) //王者碎片
		{
			nItemChange++;
			jswsnum = vecMaterialItems[i]->GetObject()->dwNum;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && !(vecMaterialItems[i]->GetObjectBase()->dwID >= 20308 && vecMaterialItems[i]->GetObjectBase()->dwID <= 29681))
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

	if (nItemUp == 1)
	{
		if(pItemUp->GetObjectBase()->dwType != 155 && pItemUp->GetObjectBase()->dwType != 137 && pItemUp->GetObjectBase()->dwType != 136 && !(pItemUp->GetObjectBase()->dwType >= 104 && pItemUp->GetObjectBase()->dwType <= 111))
		{
			m_pStaResult->SetText("非武器类装备不能进行王者武器升级");
			return false;
		}
		
		if (pItemUp->GetObject()->needlevel < 140)
		{
			m_pStaResult->SetText("145级以下的装备不能进行王者武器升级");
			return false;
		}

		if(pItemUp->GetObjectBase()->dwType == 155 && pItemUp->GetObject()->needlevel < 190)//枪类型190级以下没有
		{
			m_pStaResult->SetText("190级以下的长枪装备不能进行王者武器升级");
			return false;
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合王者碎片才能升级王者武器");
		return false;
	}

	if(jswsnum <100)
	{
		m_pStaResult->SetText("100个王者武器升级石才可升级");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

CRoleItem* CDlgEquipRecast::GetFocusItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableFocusItem->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem= static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}

CRoleItem* CDlgEquipRecast::GetMaterialItem(int index)
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableMaterials[index]->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}

bool CDlgEquipRecast::IsEquipItem(enumItemType eType)
{
	if ((eType >= ItemType_ClothBody && eType <= ItemType_Fing) || (eType >= ItemType_HorseShoe && eType <= ItemType_HorseIron) || (eType >= ItemType_Dagger && eType <= ItemType_breastplate) || (eType >= 140 && eType <= 156) || eType == 82 || eType == 90)
	{
		return true;
	}

	return false;
}

void CDlgEquipRecast::SendEquipRecastMsg(RecastAction eAction)
{
	CGuiItemCell* pItemCell = m_pTableFocusItem->GetItemCell(0, 0);
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

		case RECAST_ITEM_COMPOSE11:
			cmd.itemUID = pRoleItem->GetObject()->qwThisID;
			break;
			
		}

		SEND_USER_CMD(cmd);
	}
}

void CDlgEquipRecast::SetBGImage()
{
	stResourceLocation rl;
	strcpy(rl.szFileName, RES_PACK_NAME.c_str());
	rl.group = RES_GROUP;
	rl.frame = PAGE_BG_MAP[static_cast<int>(m_eCurPage)];

	this->SetBackImage(rl);
}

void CDlgEquipRecast::RefreshPage()
{
	switch (m_eCurPage)
	{

	case EQUIPRECAST_COMPOSE11: // 装备祝福
		{
			this->RefreshEquipCompose1();
		}
		break;
	case EQUIPRECAST_COMPOSE12: // 斗魂升级
		{
			this->RefreshEquipCompose2();
		}
		break;
	case EQUIPRECAST_COMPOSE13: // 神石镶嵌
		{
			this->RefreshEquipCompose3();
		}
		break;
	case EQUIPRECAST_COMPOSE14: // 装备栏激活
		{
			this->RefreshEquipCompose4();
		}
		break;
	case EQUIPRECAST_COMPOSE15: // 装备栏进阶
		{
			this->RefreshEquipCompose5();
		}
		break;
	case EQUIPRECAST_COMPOSE16: // 神器强化
		{
			this->RefreshEquipCompose6();
		}
		break;
	case EQUIPRECAST_COMPOSE17: // 龙槽激活
		{
			this->RefreshEquipCompose7();
		}
		break;
	case EQUIPRECAST_COMPOSE18: // 龙槽解封
		{
			this->RefreshEquipCompose8();
		}
		break;
	case EQUIPRECAST_COMPOSE19: // 龙星升级
		{
			this->RefreshEquipCompose9();
		}
		break;
	case EQUIPRECAST_COMPOSE20: // 魂魄清除
		{
			this->RefreshEquipCompose10();
		}
	    break;
	case EQUIPRECAST_COMPOSE21: // 装备升级
		{
			this->RefreshEquipCompose11();
		}
		break;
	case EQUIPRECAST_COMPOSE22: // 马牌精炼
		{
			this->RefreshEquipCompose12();
		}
		break;
	case EQUIPRECAST_COMPOSE23:  //神龙宝宝镶嵌
		{
			this->RefreshEquipCompose13();
		}
		break;
	case EQUIPRECAST_COMPOSE24:  //逆天套装进阶
		{
			this->RefreshEquipCompose14();
		}
		break;	
	case EQUIPRECAST_COMPOSE25:  //斗战胜佛套装进阶
		{
			this->RefreshEquipCompose15();
		}
		break;
	case EQUIPRECAST_COMPOSE26:  //暗影宝石镶嵌
		{
			this->RefreshEquipCompose16();
		}
		break;
	case EQUIPRECAST_COMPOSE27:  //龙凤吟
		{
			this->RefreshEquipCompose17();
		}
		break;	
	case EQUIPRECAST_COMPOSE28:  //轰天宝石镶嵌
		{
			this->RefreshEquipCompose18();
		}
		break;	
	case EQUIPRECAST_COMPOSE29:  //王者升级
		{
			this->RefreshEquipCompose19();
		}
		break;									
	}
	
	this->SetBGImage();
}

//soke 装备祝福
void CDlgEquipRecast::RefreshEquipCompose1()
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
	  bool bEnable = this->CanComposeEquip1();
	  this->GetButton(TIM_EQUIP_COMPOSE1)->SetEnabled(bEnable);

		if (bEnable)
		{
			std::stringstream ss;
			ss << "需要银子：50两";
			m_pStaNeedMoney->SetText(ss.str().c_str());
			m_pStaResult->SetText("");
			m_pStaResult->SetText("70级以上的15星绿色绑定装备在我这里进行装备祝福。");
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}

//soke 斗魂升级
void CDlgEquipRecast::RefreshEquipCompose2()
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
	  bool bEnable = this->CanComposeEquip2();
	  this->GetButton(TIM_EQUIP_COMPOSE2)->SetEnabled(bEnable);

		if (bEnable)
		{
			std::stringstream ss;
			ss << "需要银子：5锭";
			m_pStaNeedMoney->SetText(ss.str().c_str());
			m_pStaResult->SetText("");
			//m_pStaResult->SetText("70级以上的15星绿色绑定装备在我这里进行装备祝福。");
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}

//soke 补天升级
void CDlgEquipRecast::RefreshEquipCompose3()
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
	  bool bEnable = this->CanComposeEquip3();
	  this->GetButton(TIM_EQUIP_COMPOSE3)->SetEnabled(bEnable);

		if (bEnable)
		{
			std::stringstream ss;
			ss << "需要银子：5锭";
			m_pStaNeedMoney->SetText(ss.str().c_str());
			m_pStaResult->SetText("");
			//m_pStaResult->SetText("70级以上的15星绿色绑定装备在我这里进行装备祝福。");
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}

//soke 装备栏激活
void CDlgEquipRecast::RefreshEquipCompose4()
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
	  bool bEnable = this->CanComposeEquip4();
	  this->GetButton(TIM_EQUIP_COMPOSE4)->SetEnabled(bEnable);

		if (bEnable)
		{
			std::stringstream ss;
			ss << "需要银子：5锭";
			m_pStaNeedMoney->SetText(ss.str().c_str());
			m_pStaResult->SetText("");
			//m_pStaResult->SetText("70级以上的15星绿色绑定装备在我这里进行装备祝福。");
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}

//soke 装备栏进阶
void CDlgEquipRecast::RefreshEquipCompose5()
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
	  bool bEnable = this->CanComposeEquip5();
	  this->GetButton(TIM_EQUIP_COMPOSE5)->SetEnabled(bEnable);

		if (bEnable)
		{
			std::stringstream ss;
			ss << "需要银子：5锭";
			m_pStaNeedMoney->SetText(ss.str().c_str());
			m_pStaResult->SetText("");
			//m_pStaResult->SetText("70级以上的15星绿色绑定装备在我这里进行装备祝福。");
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}

//soke 神器强化
void CDlgEquipRecast::RefreshEquipCompose6()
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
	  bool bEnable = this->CanComposeEquip6();
	  this->GetButton(TIM_EQUIP_COMPOSE6)->SetEnabled(bEnable);

		if (bEnable)
		{
			std::stringstream ss;
			ss << "需要银子：5锭";
			m_pStaNeedMoney->SetText(ss.str().c_str());
			m_pStaResult->SetText("");
			//m_pStaResult->SetText("70级以上的15星绿色绑定装备在我这里进行装备祝福。");
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}

//soke 龙槽激活
void CDlgEquipRecast::RefreshEquipCompose7()
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
	  bool bEnable = this->CanComposeEquip7();
	  this->GetButton(TIM_EQUIP_COMPOSE7)->SetEnabled(bEnable);

		if (bEnable)
		{
			std::stringstream ss;
			ss << "需要银子：5锭";
			m_pStaNeedMoney->SetText(ss.str().c_str());
			m_pStaResult->SetText("");
			//m_pStaResult->SetText("70级以上的15星绿色绑定装备在我这里进行装备祝福。");
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}

//soke 龙槽解封升级
void CDlgEquipRecast::RefreshEquipCompose8()
{

	  bool bEnable = this->CanComposeEquip8();
	  this->GetButton(TIM_EQUIP_COMPOSE8)->SetEnabled(bEnable);

		if (bEnable)
		{
			std::stringstream ss;
			ss << "需要银子：5锭";
			m_pStaNeedMoney->SetText(ss.str().c_str());
			m_pStaResult->SetText("");
			//m_pStaResult->SetText("70级以上的15星绿色绑定装备在我这里进行装备祝福。");
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}

}

//soke 龙星升级
void CDlgEquipRecast::RefreshEquipCompose9()
{

	  bool bEnable = this->CanComposeEquip9();
	  this->GetButton(TIM_EQUIP_COMPOSE9)->SetEnabled(bEnable);

		if (bEnable)
		{
			std::stringstream ss;
			ss << "需要银子：5锭";
			m_pStaNeedMoney->SetText(ss.str().c_str());
			m_pStaResult->SetText("");
			//m_pStaResult->SetText("70级以上的15星绿色绑定装备在我这里进行装备祝福。");
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}

}

//soke 魂魄清除
void CDlgEquipRecast::RefreshEquipCompose10()
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
	  bool bEnable = this->CanComposeEquip10();
	  this->GetButton(TIM_EQUIP_COMPOSE10)->SetEnabled(bEnable);

		if (bEnable)
		{
			std::stringstream ss;
			ss << "需要银子：5锭";
			m_pStaNeedMoney->SetText(ss.str().c_str());
			m_pStaResult->SetText("");
			//m_pStaResult->SetText("70级以上的15星绿色绑定装备在我这里进行装备祝福。");
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}

//soke 装备升级
void CDlgEquipRecast::RefreshEquipCompose11()
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
		bool bEnable = this->CanComposeEquip11();
		this->GetButton(TIM_EQUIP_COMPOSE11)->SetEnabled(bEnable);

		if (bEnable)
		{
			std::stringstream ss;
			ss << "需要银子：5锭";
			m_pStaNeedMoney->SetText(ss.str().c_str());
			m_pStaResult->SetText("");
			//m_pStaResult->SetText("70级以上的15星绿色绑定装备在我这里进行装备升级。");
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}

//soke 马牌精炼
void CDlgEquipRecast::RefreshEquipCompose12()
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
	  bool bEnable = this->CanComposeEquip12();
	  this->GetButton(TIM_EQUIP_COMPOSE12)->SetEnabled(bEnable);

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

//soke 神龙宝宝镶嵌
void CDlgEquipRecast::RefreshEquipCompose13()
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
	  bool bEnable = this->CanComposeEquip13();
	  this->GetButton(TIM_EQUIP_COMPOSE13)->SetEnabled(bEnable);

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

//soke 逆天装备进阶
void CDlgEquipRecast::RefreshEquipCompose14()
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
	  bool bEnable = this->CanComposeEquip14();
	  this->GetButton(TIM_EQUIP_COMPOSE14)->SetEnabled(bEnable);

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

//soke 斗战圣佛套装
void CDlgEquipRecast::RefreshEquipCompose15()
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
	  bool bEnable = this->CanComposeEquip15();
	  this->GetButton(TIM_EQUIP_COMPOSE15)->SetEnabled(bEnable);

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

//soke 暗影宝石镶嵌
void CDlgEquipRecast::RefreshEquipCompose16()
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
	  bool bEnable = this->CanComposeEquip16();
	  this->GetButton(TIM_EQUIP_COMPOSE16)->SetEnabled(bEnable);

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


//soke 龙凤吟镶嵌
void CDlgEquipRecast::RefreshEquipCompose17()
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
	  bool bEnable = this->CanComposeEquip17();
	  this->GetButton(TIM_EQUIP_COMPOSE17)->SetEnabled(bEnable);

		if (bEnable)
		{
			std::stringstream ss;
			ss << "需要银子：5锭";
			m_pStaNeedMoney->SetText(ss.str().c_str());
			m_pStaResult->SetText("");
			//m_pStaResult->SetText("70级以上的15星绿色绑定装备在我这里进行装备祝福。");
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}


//soke 轰天宝石镶嵌
void CDlgEquipRecast::RefreshEquipCompose18()
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
	  bool bEnable = this->CanComposeEquip18();
	  this->GetButton(TIM_EQUIP_COMPOSE18)->SetEnabled(bEnable);

		if (bEnable)
		{
			std::stringstream ss;
			ss << "此镶嵌不需要银子";
			m_pStaNeedMoney->SetText(ss.str().c_str());
			m_pStaResult->SetText("");
			//m_pStaResult->SetText("70级以上的15星绿色绑定装备在我这里进行装备祝福。");
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}

//soke 王者升级
void CDlgEquipRecast::RefreshEquipCompose19()
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
	  bool bEnable = this->CanComposeEquip19();
	  this->GetButton(TIM_EQUIP_COMPOSE19)->SetEnabled(bEnable);

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
