#include "public.h"

#include "Game.h"
#include "GameGuiManager.h"
#include "GameScene.h"
#include "MainCharacter.h"
#include "RoleItem.h"
#include "GuiNpcMake.h"
#include "GuiNpcDialog.h"
#include "MyTimer.h"

#include "DlgEquipAnnexpack.h"

namespace
{
	const int TAB_EQUIPANNEXPACK		= 4;

	const int TABLE_FOCUSITEM			= 80;

	const int STATIC_NEEDMONEY			= 117;//价格

	const int TABLE_MATERIAL1			= 120;

	const int STATIC_RESULTINFO			= 125;// 说明提示

	const int TIM_EQUIP_COMPOSE1 = 124;    //勋章升级
	const int TIM_EQUIP_COMPOSE2 = 130;    //寒冰升级
	const int TIM_EQUIP_COMPOSE3 = 131;    //翅膀升级
	const int TIM_EQUIP_COMPOSE4 = 132;   //神兵升级
	const int TIM_EQUIP_COMPOSE5 = 133;   //神兽升级
	const int TIM_EQUIP_COMPOSE6 = 134;    //护身符璀璨镶嵌
	const int TIM_EQUIP_COMPOSE7 = 135;    //护身符神兽宝石镶嵌
    const int TIM_EQUIP_COMPOSE8 = 136;    //尚方宝剑升级
    const int TIM_EQUIP_COMPOSE9 = 137;    //紫金龙袍升级
	const int TIM_EQUIP_COMPOSE10 = 138;    //十二星宿升级
	const int TIM_EQUIP_COMPOSE11 = 139;    //十二星宿升级
	const int TIM_EQUIP_COMPOSE12 = 140;    //十二星宿升级
	const int TIM_EQUIP_COMPOSE13 = 141;    //十二星宿升级
	const int TIM_EQUIP_COMPOSE14 = 142;   //金箍咒强化
	const int TIM_EQUIP_COMPOSE15 = 143;   //金箍咒字纹
	const int TIM_EQUIP_COMPOSE16 = 144;   //金箍咒淬炼
	const int TIM_EQUIP_COMPOSE17 = 145;   //寒冰神炼
	const int TIM_EQUIP_COMPOSE18 = 146;   //寒冰字纹
	const int TIM_EQUIP_COMPOSE19 = 147;   //寒冰纹章



	const int BUTTONGROUP_EQUIPANNEXPACK	= 11;

	const int MAX_PAGE_NUM				= 19;// tab的分页数

	const std::string STR_EQUIPANNEXPACK_TITLES[MAX_PAGE_NUM] =
	{
		std::string("勋章升级(205级)"),
		std::string("寒冰升级(190级)"),
		std::string("翅膀升级"),
		std::string("神兵升级"),
		std::string("神兽升级"),
		std::string("璀璨镶嵌(190级)"),
		std::string("神兽镶嵌(190级)"),
        std::string("尚方宝剑升级"),
		std::string("紫金龙袍升级"),		
		std::string("十二星宿升级"),	
		std::string("神斧镶嵌(190级)"),
		std::string("黑曜镶嵌(190级)"),
		std::string("精金镶嵌(190级)"),
		std::string("金箍咒强化(205级)"),
		std::string("金箍咒字纹(205级)"),
		std::string("金箍咒淬炼(205级)"),
		std::string("寒冰神炼(190级)"),
		std::string("寒冰字纹(190级)"),
		std::string("寒冰纹章(190级)"),
		
	};

	const std::string RES_PACK_NAME = std::string("data\\interfaces1.gl");
	const int RES_GROUP = 8;
	int PAGE_BG_MAP[MAX_PAGE_NUM] = {0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0}; // 页数和背景图片frame的映射

	const int MONEY_SIGNATURE = 20000; // 添加（修改）签名需要的金钱（文）
}

CDlgEquipAnnexpack::CDlgEquipAnnexpack()
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;
	m_eCurPage = EQUIPANNEXPACK_COMPOSE2;
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

CDlgEquipAnnexpack::~CDlgEquipAnnexpack()
{
	FUNCTION_BEGIN;

	SAFE_DELETE(m_pTimerProcess);

	FUNCTION_END;
}

void CDlgEquipAnnexpack::OnPutFocusItem(bool bPut)
{
	m_bPutFocusItem = bPut;
	this->RefreshPage();
}

void CDlgEquipAnnexpack::OnResult(stResponsePropertyUserCmd* pCmd)
{
	if (!pCmd)
	{
		return;
	}

	switch(pCmd->kind)
	{
	case EQUIP_COMPOSE51:   //勋章升级
	case EQUIP_COMPOSE52:   //寒冰升级
	case EQUIP_COMPOSE53:   //翅膀升级
	case EQUIP_COMPOSE54:   //神兵升级
	case EQUIP_COMPOSE55:   //神兽升级
	case EQUIP_COMPOSE56:   //护身符璀璨镶嵌
	case EQUIP_COMPOSE57:   //护身符神兽宝石镶嵌
 	case EQUIP_COMPOSE58:   //尚方宝剑升级
	case EQUIP_COMPOSE59:   //紫金龙袍升级   
	case EQUIP_COMPOSE60:   //十二星宿升级   
	case EQUIP_COMPOSE61:   //神斧   
	case EQUIP_COMPOSE62:   //黑曜
	case EQUIP_COMPOSE63:   //精金   
	case EQUIP_COMPOSE64:   //金箍咒强化
	case EQUIP_COMPOSE65:   //金箍咒字纹
	case EQUIP_COMPOSE66:   //金箍咒淬炼
	case EQUIP_COMPOSE67:   //寒冰神炼
	case EQUIP_COMPOSE68:   //寒冰字纹
	case EQUIP_COMPOSE69:   //寒冰纹章
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

void CDlgEquipAnnexpack::OnClose(void)
{
	GetGameGuiManager()->m_pDlgEquipAnnexpack = NULL;
}

void CDlgEquipAnnexpack::OnCreate()
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

	m_pTabEquipAnnexpack = this->GetTab(TAB_EQUIPANNEXPACK);
	if (m_pTabEquipAnnexpack)
	{
		m_pTabEquipAnnexpack->SetVisible(false);
		m_pTabEquipAnnexpack->SetCurItem(static_cast<int>(m_eCurPage));
	}

	m_pTableFocusItem = this->GetTable(TABLE_FOCUSITEM);
	m_pTableFocusItem->m_iTableType = OBJECTCELLTYPE_ANNEX;
	m_pTableFocusItem->m_iTableID = TABLE_FOCUSITEM;
	m_pTableFocusItem->m_EquipPosition = stPointI(0,MAKECELLTYPE_EQUIP);

	m_pTableMaterial1 = this->GetTable(TABLE_MATERIAL1);
	m_pTableMaterial1->m_iTableType = OBJECTCELLTYPE_ANNEX;
	m_pTableMaterial1->m_iTableID = TABLE_MATERIAL1;
	m_pTableMaterial1->m_EquipPosition = stPointI(0,MAKECELLTYPE_MATERIAL1);
	m_pTableMaterials[0] = m_pTableMaterial1;


	for (int i = 0; i < TOTAL_PAGE_NUM; ++i)
	{
		m_pBtnPages[i] = this->GetButton(BUTTONGROUP_EQUIPANNEXPACK + i);
	}

	m_pBtnPages[static_cast<int>(m_eCurPage)]->SetChecked(true);
	m_strTitle = STR_EQUIPANNEXPACK_TITLES[static_cast<int>(m_eCurPage)];
	m_pStaResult = this->GetStatic(STATIC_RESULTINFO);

	this->RefreshPage();
}

bool CDlgEquipAnnexpack::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
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
			if (nID >= BUTTONGROUP_EQUIPANNEXPACK && nID < BUTTONGROUP_EQUIPANNEXPACK + MAX_PAGE_NUM)
			{
				m_pStaResult->SetText("");
				int nIndex = nID - BUTTONGROUP_EQUIPANNEXPACK;
				m_pTabEquipAnnexpack->SetCurItem(nIndex);
				m_pBtnPages[nIndex]->SetChecked(true);
				m_eCurPage = static_cast<EQUIPANNEXPACK_PAGE>(nIndex);
				m_strTitle = STR_EQUIPANNEXPACK_TITLES[static_cast<int>(m_eCurPage)];

				this->RefreshPage();
			}

			if (nID == TIM_EQUIP_COMPOSE1) ///<勋章升级
			{
				if (this->CanComposeEquip1())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE51);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE2) ///<寒冰升级
			{
				if (this->CanComposeEquip2())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE52);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE3) ///<翅膀升级
			{
				if (this->CanComposeEquip3())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE53);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE4) ///<神兵升级
			{
				if (this->CanComposeEquip4())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE54);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE5) ///<神兽升级
			{
				if (this->CanComposeEquip5())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE55);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE6) ///<护身符璀璨镶嵌
			{
				if (this->CanComposeEquip6())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE56);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE7) ///<神兽宝石镶嵌
			{
				if (this->CanComposeEquip7())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE57);
				}
			}
            else if (nID == TIM_EQUIP_COMPOSE8) ///<尚方宝剑升级
			{
				if (this->CanComposeEquip8())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE58);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE9) ///<紫金龙袍升级
			{
				if (this->CanComposeEquip9())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE59);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE10) ///<十二星宿升级
			{
				if (this->CanComposeEquip10())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE60);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE11) ///<神斧
			{
				if (this->CanComposeEquip11())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE61);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE12) ///<黑曜
			{
				if (this->CanComposeEquip12())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE62);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE13) ///<精金
			{
				if (this->CanComposeEquip13())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE63);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE14) ///<3月19新增附件
			{
				if (this->CanComposeEquip14())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE64);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE15) ///<3月19新增附件
			{
				if (this->CanComposeEquip15())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE65);
				}
			}	
			else if (nID == TIM_EQUIP_COMPOSE16) ///<3月19新增附件
			{
				if (this->CanComposeEquip16())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE66);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE17) ///<3月19新增附件
			{
				if (this->CanComposeEquip17())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE67);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE18) ///<3月19新增附件
			{
				if (this->CanComposeEquip18())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE68);
				}
			}
			else if (nID == TIM_EQUIP_COMPOSE19) ///<3月19新增附件
			{
				if (this->CanComposeEquip19())
				{
				this->SendEquipAnnexpackMsg(RECAST_ITEM_COMPOSE69);
				}
			}
		}
		break;
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CDlgEquipAnnexpack::OnSetCloseLocation()
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

HRESULT CDlgEquipAnnexpack::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	DrawText(m_strTitle.c_str(), m_BackElements[0], &stRectI(395, 85, 627, 340));//soke 页面说明字体位置
	m_aniSuccess.Render(m_x+299,m_y-149);//soke 装特效位置



	return hr;

	FUNCTION_END;
}

//soke 勋章升级
bool CDlgEquipAnnexpack::CanComposeEquip1()
{
	if(GetScene()->GetMainCharacter()->GetLevel()<205)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		m_pStaResult->SetTextColor(dwColor);
		m_pStaResult->SetText("205级开放勋章功能");
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
		m_pStaResult->SetText("      勋章配合段位勋章经验丹可提升等级");
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
		if ( vecMaterialItems[i]->GetObjectID() >= 3832 && vecMaterialItems[i]->GetObjectID() <= 3837) //升级材料
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp>0)&&(1002 > vecMaterialItems[i]->GetObject()->dmedalt)) //等级
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
		if (!(pItemUp->GetObjectID() >= 3826 && pItemUp->GetObjectID() <= 3831))
		{
			m_pStaResult->SetText("            段位勋章才能在此升级");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				if ((pItemUp->GetObjectID() >= 3826 && pItemUp->GetObjectID() <= 3830) && pItemUp->GetObject()->dmedalt < 100 )
				{
					m_pStaResult->SetText("当前段位勋章升级需要 段位勋章经验丹");
					return false;
				}
				if ((pItemUp->GetObjectID() == 3831) && pItemUp->GetObject()->dmedalt < 1000 )
				{
					m_pStaResult->SetText("当前段位勋章升级需要 段位勋章经验丹");
					return false;
				}				
				else if ((pItemUp->GetObjectID() >= 3826 && pItemUp->GetObjectID() <= 3830) && pItemUp->GetObject()->dmedalt > 99 )
				{
					if (pItemUp->GetObjectID() == 3826)
					{
						m_pStaResult->SetText("当前段位勋章品质提升需要 段位勋章经验丹");
						return false;
					}
					else if (pItemUp->GetObjectID() == 3827)
					{
						m_pStaResult->SetText("当前段位勋章品质提升需要 段位勋章经验丹 ");
						return false;
					}
					else if (pItemUp->GetObjectID()== 3828)
					{
						m_pStaResult->SetText("当前段位勋章品质提升需要 段位勋章经验丹");
						return false;
					}
					else if (pItemUp->GetObjectID()== 3829)
					{
						m_pStaResult->SetText("当前段位勋章品质提升需要 段位勋章经验丹");
						return false;
					}
					else if (pItemUp->GetObjectID()== 3830)
					{
						m_pStaResult->SetText("当前段位勋章品质提升需要 段位勋章经验丹");
						return false;
					}					
				}	
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("            单件装备配合段位勋章经验丹升级");
		return false;
	}

	if (pItemUp->GetObjectID()== 3831 && pItemUp->GetObject()->dmedalt > 999)
	{
		m_pStaResult->SetText("       勋章等级已经最高，不能再升级");
		return false;
	}

	char szInfo[MAX_PATH] = {0};
	sprintf(szInfo, "消耗银子：1锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 寒冰升级
bool CDlgEquipAnnexpack::CanComposeEquip2()
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
		m_pStaResult->SetText("      寒冰麒麟配合麒麟丹可提升等级");
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
		if ( vecMaterialItems[i]->GetObjectID() >= 3866 && vecMaterialItems[i]->GetObjectID() <= 3869)
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp>0)&&(1001 > vecMaterialItems[i]->GetObject()->retain1))
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
		if (!(pItemUp->GetObjectID() >= 3860 && pItemUp->GetObjectID() <= 3865))
		{
			m_pStaResult->SetText("            寒冰麒麟才能在此升级");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				if ((pItemUp->GetObjectID() >= 3860 && pItemUp->GetObjectID() <= 3864) && pItemUp->GetObject()->retain1 < 100 )
				{
					m_pStaResult->SetText("当前寒冰麒麟升级需要 麒麟丹");
					return false;
				}
				if ((pItemUp->GetObjectID() == 3865) && pItemUp->GetObject()->retain1 < 1000 )
				{
					m_pStaResult->SetText("当前寒冰麒麟升级需要 麒麟丹");
					return false;
				}				
				else if ((pItemUp->GetObjectID() >= 3860 && pItemUp->GetObjectID() <= 3864) && pItemUp->GetObject()->retain1 > 99 )
				{
					if (pItemUp->GetObjectID() == 3860)
					{
						m_pStaResult->SetText("当前寒冰麒麟品质提升需要 麒麟之魄"); //白色
						return false;
					}
					else if (pItemUp->GetObjectID() == 3861)
					{
						m_pStaResult->SetText("当前寒冰麒麟品质提升需要 麒麟之魄"); //蓝色
						return false;
					}
					else if (pItemUp->GetObjectID()== 3862)
					{
						m_pStaResult->SetText("当前寒冰麒麟品质提升需要  麒麟之魄"); //黄色 
						return false;
					}
					else if (pItemUp->GetObjectID()== 3863)
					{
						m_pStaResult->SetText("当前寒冰麒麟品质提升需要  麒麟之魄"); //绿色 
						return false;
					}
					else if (pItemUp->GetObjectID()== 3864)
					{
						m_pStaResult->SetText("当前寒冰麒麟品质提升需要  麒麟之魄"); //紫色
						return false;
					}
				}	
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("            寒冰麒麟配合麒麟丹升级");
		return false;
	}

	if (pItemUp->GetObjectID()== 3865 && pItemUp->GetObject()->retain1 > 999)
	{
		m_pStaResult->SetText("            寒冰麒麟等级已经最高，不能再升级");
		return false;
	}

	char szInfo[MAX_PATH] = {0};
	sprintf(szInfo, "消耗银子：1锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 翅膀升级
bool CDlgEquipAnnexpack::CanComposeEquip3()
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
		m_pStaResult->SetText("      天使之翼配合神翼白羽可提升等级");
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
		if ( vecMaterialItems[i]->GetObjectID() == 3905)
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp>0)&&(1802 > vecMaterialItems[i]->GetObject()->retain4))
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
		if (!(pItemUp->GetObjectID() >= 3900 && pItemUp->GetObjectID() <= 3904))
		{
			m_pStaResult->SetText("            天使之翼才能在此升级");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				if ((pItemUp->GetObjectID() >= 3900 && pItemUp->GetObjectID() <= 3904) && pItemUp->GetObject()->retain4 < 1801 )
				{
					m_pStaResult->SetText("当前天使之翼升级需要 神翼白羽");
					return false;
				}
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("            天使之翼配合神翼白羽升级");
		return false;
	}

	if (pItemUp->GetObject()->retain4 >= 1800)
	{
		m_pStaResult->SetText("            天使之翼等级已经最高，不能再升级");
		return false;
	}

	char szInfo[MAX_PATH] = {0};
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 神兵升级
bool CDlgEquipAnnexpack::CanComposeEquip4()
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
		m_pStaResult->SetText("      神兵配合:龙皇之怒 可提升等级");
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
		if ( vecMaterialItems[i]->GetObjectID() == 35101)
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp>0)&&(10002	 > vecMaterialItems[i]->GetObject()->retain5))
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
		if (!(pItemUp->Get2ObjectID() == ItemType_Shenbing0 || pItemUp->Get2ObjectID() == ItemType_Shenbing1 ||pItemUp->Get2ObjectID() == ItemType_Shenbing2))
		{
			m_pStaResult->SetText("            神兵才能在此升级");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				if ( pItemUp->GetObject()->retain5 < 1001 )
				{
					m_pStaResult->SetText("当前神兵升级需要 龙皇之怒");
					return false;
				}
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("            神兵配合龙皇之怒升级");
		return false;
	}

	if (pItemUp->GetObject()->retain5 >= 1000)
	{
		m_pStaResult->SetText("            神兵等级已经最高，不能再升级");
		return false;
	}

	char szInfo[MAX_PATH] = {0};
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 神兽升级
bool CDlgEquipAnnexpack::CanComposeEquip5()
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
		m_pStaResult->SetText("      神兽配合:龙皇之怒 可提升等级");
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
		if ( vecMaterialItems[i]->GetObjectID() == 35101)
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp>0)&&(10002 > vecMaterialItems[i]->GetObject()->retain6))
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
		if (!(pItemUp->Get2ObjectID() == ItemType_Shenshou1 || pItemUp->Get2ObjectID() == ItemType_Shenshou2))
		{
			m_pStaResult->SetText("            神兽才能在此升级");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				if ( pItemUp->GetObject()->retain6 < 1001 )
				{
					m_pStaResult->SetText("当前神兽升级需要 龙皇之怒");
					return false;
				}
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("            神兽配合龙皇之怒升级");
		return false;
	}

	if (pItemUp->GetObject()->retain6 >= 1000)
	{
		m_pStaResult->SetText("            神兽等级已经最高，不能再升级");
		return false;
	}

	char szInfo[MAX_PATH] = {0};
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 装备璀璨镶嵌
bool CDlgEquipAnnexpack::CanComposeEquip6()
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
		m_pStaResult->SetText("装备获得璀璨属性需要:璀璨属性石");
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
		if (vecMaterialItems[i]->GetObjectID() == 3850) //ZM 璀璨石
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (1001 > vecMaterialItems[i]->GetObject()->itemkscuican))
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
		if (!(pItemUp->GetObject()->dwObjectID >= 18000 && pItemUp->GetObject()->dwObjectID <= 18025 ) )
		{
			m_pStaResult->SetText("只有护身符才能镶嵌璀璨宝石");
			return false;
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合璀璨属性石才能镶嵌至装备");
		return false;
	}

	if (pItemUp->GetObject()->itemkscuican >= 1000)
	{
		m_pStaResult->SetText("装备璀璨镶嵌等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 装备神兽宝石镶嵌
bool CDlgEquipAnnexpack::CanComposeEquip7()
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
		m_pStaResult->SetText("装备获得神兽宝石属性需要:神兽宝石\n神兽宝石种类：白虎石 青龙石 玄武石 朱雀石 麒麟石");
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
		if (vecMaterialItems[i]->GetObjectID() >= 3838 && vecMaterialItems[i]->GetObjectID() <= 3842) //ZM 神兽宝石
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (1001 > vecMaterialItems[i]->GetObject()->itemksAmulet))
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
		if (!(pItemUp->GetObject()->dwObjectID >= 18000 && pItemUp->GetObject()->dwObjectID <= 18025 ) )
		{
			m_pStaResult->SetText("只有护身符才能镶嵌神兽宝石");
			return false;
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合神兽宝石才能镶嵌至装备");
		return false;
	}
	if (pItemUp->GetObject()->itemksAmulet >= 1000)
	{
		m_pStaResult->SetText("装备神兽宝石镶嵌等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 尚方宝剑升级
bool CDlgEquipAnnexpack::CanComposeEquip8()
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
		m_pStaResult->SetText("      尚方宝剑配合:龙皇之怒 可提升等级");
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
		if ( vecMaterialItems[i]->GetObjectID() == 35101) //尚方宝剑升级材料
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp>0)&&(1002 > vecMaterialItems[i]->GetObject()->shangfanglv))
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
		if (!(pItemUp->GetObjectID() == 3932  ) && !(pItemUp->GetObjectID() == 4311  )&& !(pItemUp->GetObjectID() == 4312  )&& !(pItemUp->GetObjectID() == 4313  ))
		{
			m_pStaResult->SetText("            尚方宝剑才能在此升级");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				if ((pItemUp->GetObjectID() == 3932 ||pItemUp->GetObjectID() == 4311||pItemUp->GetObjectID() == 4312||pItemUp->GetObjectID() == 4313) && pItemUp->GetObject()->shangfanglv < 10001 )
				{
					m_pStaResult->SetText("当前物品升级需要 龙皇之怒 来升级");
					return false;
				}
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("            当前物品配合：龙皇之怒来升级");
		return false;
	}

	if (pItemUp->GetObject()->shangfanglv >= 1000)
	{
		m_pStaResult->SetText("            尚方宝剑等级已经最高，不能再升级");
		return false;
	}

	char szInfo[MAX_PATH] = {0};
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

//soke 紫金龙袍升级
bool CDlgEquipAnnexpack::CanComposeEquip9()
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
		m_pStaResult->SetText("      紫金龙袍配合:龙皇之怒 可提升等级");
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
		if ( vecMaterialItems[i]->GetObjectID() == 35101) //紫金龙袍升级材料
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp>0)&&(1001 > vecMaterialItems[i]->GetObject()->zijinlplv))
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
		if (!(pItemUp->GetObjectID() == 3933) &&!(pItemUp->GetObjectID() == 4321)&&!(pItemUp->GetObjectID() == 4322)&&!(pItemUp->GetObjectID() == 4321)&&!(pItemUp->GetObjectID() == 4323))
		{
			m_pStaResult->SetText("            紫金龙袍才能在此升级");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				if ((pItemUp->GetObjectID() == 3933 ||pItemUp->GetObjectID() == 4321||pItemUp->GetObjectID() == 4322||pItemUp->GetObjectID() == 4323 ) && pItemUp->GetObject()->zijinlplv < 10001 )
				{
					m_pStaResult->SetText("当前物品升级需要 龙皇之怒 来升级");
					return false;
				}
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("            当前物品配合：龙皇之怒来升级");
		return false;
	}

	if (pItemUp->GetObject()->zijinlplv >= 1000)
	{
		m_pStaResult->SetText("            紫金龙袍等级已经最高，不能再升级");
		return false;
	}

	char szInfo[MAX_PATH] = {0};
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}



//soke 十二星宿升级
bool CDlgEquipAnnexpack::CanComposeEquip10()
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
		m_pStaResult->SetText("      十二星宿配合:星宿石 可提升等级");
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
		if ( vecMaterialItems[i]->GetObjectID() == 20032) //星宿石
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp>0)&&(1001 > vecMaterialItems[i]->GetObject()->retain21))
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
		if (!(pItemUp->GetObjectID() >=20020 && pItemUp->GetObjectID()<=20031))
		{
			m_pStaResult->SetText("            十二星宿才能在此升级");
			return false;
		}
		else
		{
			if (nItemChange == 0)
			{
				if ((pItemUp->GetObjectID() >=20020 && pItemUp->GetObjectID()<=20031) && pItemUp->GetObject()->retain21 < 1001 )
				{
					m_pStaResult->SetText("当前物品升级需要 星宿石 来升级");
					return false;
				}
			}
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("            当前物品配合：星宿石来升级");
		return false;
	}

	if (pItemUp->GetObject()->zijinlplv >= 1000)
	{
		m_pStaResult->SetText("            十二星宿等级已经最高，不能再升级");
		return false;
	}

	char szInfo[MAX_PATH] = {0};
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}


//soke 装备神斧镶嵌
bool CDlgEquipAnnexpack::CanComposeEquip11()
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
		m_pStaResult->SetText("装备获得神斧属性需要:神斧石");
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
		if (vecMaterialItems[i]->GetObjectID() == 3851) //ZM 神斧石
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (1001 > vecMaterialItems[i]->GetObject()->retain32))
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
		if (!(pItemUp->GetObject()->dwObjectID >= 18000 && pItemUp->GetObject()->dwObjectID <= 18025 ) )
		{
			m_pStaResult->SetText("只有护身符才能镶嵌神斧石");
			return false;
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合神斧石才能镶嵌至装备");
		return false;
	}

	if (pItemUp->GetObject()->retain32 >= 1000)
	{
		m_pStaResult->SetText("装备神斧镶嵌等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}



//soke 装备黑曜镶嵌
bool CDlgEquipAnnexpack::CanComposeEquip12()
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
		m_pStaResult->SetText("装备获得黑曜属性需要:黑曜石");
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
		if (vecMaterialItems[i]->GetObjectID() == 3852) //ZM 黑曜石
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (1001 > vecMaterialItems[i]->GetObject()->retain33))
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
		if (!(pItemUp->GetObject()->dwObjectID >= 18000 && pItemUp->GetObject()->dwObjectID <= 18025 ) )
		{
			m_pStaResult->SetText("只有护身符才能镶嵌黑曜石");
			return false;
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合黑曜石才能镶嵌至装备");
		return false;
	}

	if (pItemUp->GetObject()->retain33 >= 1000)
	{
		m_pStaResult->SetText("装备黑曜镶嵌等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}


//soke 装备精金镶嵌
bool CDlgEquipAnnexpack::CanComposeEquip13()
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
		m_pStaResult->SetText("装备获得精金属性需要:精金石");
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
		if (vecMaterialItems[i]->GetObjectID() == 3853) //ZM 精金石
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (1001 > vecMaterialItems[i]->GetObject()->retain34))
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
		if (!(pItemUp->GetObject()->dwObjectID >= 18000 && pItemUp->GetObject()->dwObjectID <= 18025 ) )
		{
			m_pStaResult->SetText("只有护身符才能镶嵌精金石");
			return false;
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("单件装备配合精金石才能镶嵌至装备");
		return false;
	}

	if (pItemUp->GetObject()->retain34 >= 1000)
	{
		m_pStaResult->SetText("装备精金镶嵌等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：5锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}


//-----------------------------------------------------------------------------
//soke 附件金箍咒强化
bool CDlgEquipAnnexpack::CanComposeEquip14()
{
	if(GetScene()->GetMainCharacter()->GetLevel()<205)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		m_pStaResult->SetTextColor(dwColor);
		m_pStaResult->SetText("205级开放金箍功能");
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
		m_pStaResult->SetText("金箍咒强化需要：金箍强化石");
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
		if (vecMaterialItems[i]->GetObjectID() == 5351) //TZ 材料
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (1001 > vecMaterialItems[i]->GetObject()->syxqgrade21)) //强化次数+1
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
		if (!(pItemUp->GetObject()->dwObjectID == 3831)) //升级道具
		{
			m_pStaResult->SetText("　　　　 只有金箍级勋章才能强化");
			return false;
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("金箍配合金箍强化石才能强化");
		return false;
	}

	if (pItemUp->GetObject()->syxqgrade21 >= 1000) //强化次数
	{
		m_pStaResult->SetText("金箍强化等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：1锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}
//-----------------------------------------------------------------------------
//soke 附件金箍咒镶嵌字纹
bool CDlgEquipAnnexpack::CanComposeEquip15()
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
		m_pStaResult->SetText("金箍咒镶嵌字纹需要：金箍强化石");
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
		if (vecMaterialItems[i]->GetObjectID() == 5351) //TZ 材料
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (1001 > vecMaterialItems[i]->GetObject()->syxqgrade22)) //镶嵌字纹次数+1
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
		if (!(pItemUp->GetObject()->dwObjectID == 3831)) //升级道具
		{
			m_pStaResult->SetText("　　　　 只有金箍级勋章才能镶嵌字纹");
			return false;
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("金箍配合金箍强化石才能镶嵌字纹");
		return false;
	}

	if (pItemUp->GetObject()->syxqgrade22 >= 1000) //镶嵌字纹次数
	{
		m_pStaResult->SetText("金箍字纹镶嵌等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：1锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//soke 附件金箍咒淬炼
bool CDlgEquipAnnexpack::CanComposeEquip16()
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
		m_pStaResult->SetText("金箍咒淬炼需要：金箍强化石");
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
		if (vecMaterialItems[i]->GetObjectID() == 5351) //TZ 材料
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (1001 > vecMaterialItems[i]->GetObject()->syxqgrade23)) //淬炼次数+1
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
		if (!(pItemUp->GetObject()->dwObjectID == 3831)) //升级道具
		{
			m_pStaResult->SetText("　　　　 只有金箍级勋章才能淬炼");
			return false;
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("金箍配合金箍强化石才能淬炼");
		return false;
	}

	if (pItemUp->GetObject()->syxqgrade23 >= 1000) //淬炼次数
	{
		m_pStaResult->SetText("金箍淬炼等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：1锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//soke 附件寒冰神炼
bool CDlgEquipAnnexpack::CanComposeEquip17()
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
		m_pStaResult->SetText("寒冰神炼需要：寒冰纹章石");
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
		if (vecMaterialItems[i]->GetObjectID() == 5356) //TZ 材料
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (1001 > vecMaterialItems[i]->GetObject()->syxqgrade24)) //神炼次数+1
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
		if (!(pItemUp->GetObject()->dwObjectID == 3865)) //升级道具
		{
			m_pStaResult->SetText("只有五彩寒冰麒麟才能神炼");
			return false;
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("寒冰麒麟配合寒冰纹章石才能神炼");
		return false;
	}

	if (pItemUp->GetObject()->syxqgrade24 >= 1000) //神炼次数
	{
		m_pStaResult->SetText("寒冰麒麟神炼等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：1锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//soke 附件寒冰字纹
bool CDlgEquipAnnexpack::CanComposeEquip18()
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
		m_pStaResult->SetText("寒冰字纹需要：寒冰纹章石");
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
		if (vecMaterialItems[i]->GetObjectID() == 5356) //TZ 材料
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (1001 > vecMaterialItems[i]->GetObject()->syxqgrade25)) //字纹次数+1
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
		if (!(pItemUp->GetObject()->dwObjectID == 3865)) //升级道具
		{
			m_pStaResult->SetText("只有五彩寒冰麒麟才能字纹镶嵌");
			return false;
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("寒冰麒麟配合寒冰纹章石才能字纹镶嵌");
		return false;
	}

	if (pItemUp->GetObject()->syxqgrade25 >= 1000) //字纹次数
	{
		m_pStaResult->SetText("寒冰麒麟字纹等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：1锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//soke 附件寒冰纹章
bool CDlgEquipAnnexpack::CanComposeEquip19()
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
		m_pStaResult->SetText("寒冰纹章需要：寒冰纹章石");
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
		if (vecMaterialItems[i]->GetObjectID() == 5356) //TZ 材料
		{
			nItemChange++;
			continue;
		}

		if ((vecMaterialItems[i]->GetObjectBase()->dwUp > 0) && (101 > vecMaterialItems[i]->GetObject()->syxqgrade26)) //纹章次数+1
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
		if (!(pItemUp->GetObject()->dwObjectID == 3865)) //升级道具
		{
			m_pStaResult->SetText("只有五彩寒冰麒麟才能纹章镶嵌");
			return false;
		}
	}

	if ((nItemUp != 1) || (nItemChange < 1))
	{
		m_pStaResult->SetText("寒冰麒麟配合寒冰纹章石才能纹章镶嵌");
		return false;
	}

	if (pItemUp->GetObject()->syxqgrade26 >= 100) //纹章次数
	{
		m_pStaResult->SetText("寒冰麒麟纹章等级已经最高");
		return false;
	}

	char szInfo[MAX_PATH] = { 0 };
	sprintf(szInfo, "消耗银子：1锭");
	m_pStaNeedMoney->SetText(szInfo);

	return true;
}

CRoleItem* CDlgEquipAnnexpack::GetFocusItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableFocusItem->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem= static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}

CRoleItem* CDlgEquipAnnexpack::GetMaterialItem(int index)
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableMaterials[index]->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}

bool CDlgEquipAnnexpack::IsEquipItem(enumItemType eType)
{
	if ((eType >= ItemType_ClothBody && eType <= ItemType_Fing) 
    || (eType >= ItemType_HorseShoe && eType <= ItemType_HorseIron) 
    || (eType >= ItemType_Dagger && eType <= ItemType_breastplate) 
    || (eType >= 141 && eType <= 147) 
    || eType == 82 || eType == 88 || eType == 89
    || eType == 90)
	{
		return true;
	}

	return false;
}

void CDlgEquipAnnexpack::SendEquipAnnexpackMsg(RecastAction eAction)
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

		case RECAST_ITEM_COMPOSE51:
        case RECAST_ITEM_COMPOSE52:
        case RECAST_ITEM_COMPOSE53:
        case RECAST_ITEM_COMPOSE54:
        case RECAST_ITEM_COMPOSE55:
        case RECAST_ITEM_COMPOSE56:
        case RECAST_ITEM_COMPOSE57:
        case RECAST_ITEM_COMPOSE58:
        case RECAST_ITEM_COMPOSE59:
			cmd.itemUID = pRoleItem->GetObject()->qwThisID;
			break; 
		}

		SEND_USER_CMD(cmd);
	}
}

void CDlgEquipAnnexpack::SetBGImage()
{
	stResourceLocation rl;
	strcpy(rl.szFileName, RES_PACK_NAME.c_str());
	rl.group = RES_GROUP;
	rl.frame = PAGE_BG_MAP[static_cast<int>(m_eCurPage)];

	this->SetBackImage(rl);
}

void CDlgEquipAnnexpack::RefreshPage()
{
	switch (m_eCurPage)
	{

	case EQUIPANNEXPACK_COMPOSE1: // 勋章升级
		{
			this->RefreshEquipCompose1();
		}
		break;
	case EQUIPANNEXPACK_COMPOSE2: // 寒冰升级
		{
			this->RefreshEquipCompose2();
		}
		break;
	case EQUIPANNEXPACK_COMPOSE3: // 翅膀升级
		{
			this->RefreshEquipCompose3();
		}
		break;
	case EQUIPANNEXPACK_COMPOSE4: // 神兵升级
		{
			this->RefreshEquipCompose4();
		}
		break;
	case EQUIPANNEXPACK_COMPOSE5: // 神兽升级
		{
			this->RefreshEquipCompose5();
		}
		break;
	case EQUIPANNEXPACK_COMPOSE6:  //装备璀璨镶嵌
		{
			this->RefreshEquipCompose6();
		}
		break;	
	case EQUIPANNEXPACK_COMPOSE7:  //装备神兽宝石镶嵌
		{
			this->RefreshEquipCompose7();
		}
		break;
    case EQUIPANNEXPACK_COMPOSE8:  //尚方宝剑升级
		{
			this->RefreshEquipCompose8();
		}
		break;	
	case EQUIPANNEXPACK_COMPOSE9:  //紫金龙袍升级
		{
			this->RefreshEquipCompose9();
		}
		break; 
	case EQUIPANNEXPACK_COMPOSE10:  //十二星宿升级
		{
			this->RefreshEquipCompose10();
		}
		break;
	case EQUIPANNEXPACK_COMPOSE11:  //神斧
		{
			this->RefreshEquipCompose11();
		}
		break;
	case EQUIPANNEXPACK_COMPOSE12:  //黑曜
		{
			this->RefreshEquipCompose12();
		}
		break;
	case EQUIPANNEXPACK_COMPOSE13:  //精金
		{
			this->RefreshEquipCompose13();
		}
		break;  
	case EQUIPANNEXPACK_COMPOSE14:  //金箍咒强化
		{
			this->RefreshEquipCompose14();
		}
		break;	
	case EQUIPANNEXPACK_COMPOSE15:  //金箍咒字纹
		{
			this->RefreshEquipCompose15();
		}
		break;	
	case EQUIPANNEXPACK_COMPOSE16:  //金箍咒淬炼
		{
			this->RefreshEquipCompose16();
		}
		break;	
	case EQUIPANNEXPACK_COMPOSE17:  //寒冰神炼
		{
			this->RefreshEquipCompose17();
		}
		break;	
	case EQUIPANNEXPACK_COMPOSE18:  //寒冰字纹
		{
			this->RefreshEquipCompose18();
		}
		break;	
	case EQUIPANNEXPACK_COMPOSE19:  //寒冰纹章
		{
			this->RefreshEquipCompose19();
		}
		break;	  						
	}

	
	this->SetBGImage();
}

//soke 勋章升级
void CDlgEquipAnnexpack::RefreshEquipCompose1()
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

//soke 寒冰升级
void CDlgEquipAnnexpack::RefreshEquipCompose2()
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

//soke 翅膀升级
void CDlgEquipAnnexpack::RefreshEquipCompose3()
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

//soke 神兵升级
void CDlgEquipAnnexpack::RefreshEquipCompose4()
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

//soke 神兽升级
void CDlgEquipAnnexpack::RefreshEquipCompose5()
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

//soke 璀璨镶嵌
void CDlgEquipAnnexpack::RefreshEquipCompose6()
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
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}

//soke 神兽宝石镶嵌
void CDlgEquipAnnexpack::RefreshEquipCompose7()
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
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}

//soke 尚方宝剑升级
void CDlgEquipAnnexpack::RefreshEquipCompose8()
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
	  bool bEnable = this->CanComposeEquip8();
	  this->GetButton(TIM_EQUIP_COMPOSE8)->SetEnabled(bEnable);

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

//soke 紫金龙袍升级
void CDlgEquipAnnexpack::RefreshEquipCompose9()
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
	  bool bEnable = this->CanComposeEquip9();
	  this->GetButton(TIM_EQUIP_COMPOSE9)->SetEnabled(bEnable);

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


//soke 十二星宿升级
void CDlgEquipAnnexpack::RefreshEquipCompose10()
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
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}




//soke 神斧升级
void CDlgEquipAnnexpack::RefreshEquipCompose11()
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
		}
		else
		{
			m_pStaNeedMoney->SetText("");
		}
	}
}


//soke 黑曜升级
void CDlgEquipAnnexpack::RefreshEquipCompose12()
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


//soke 精金升级
void CDlgEquipAnnexpack::RefreshEquipCompose13()
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






void CDlgEquipAnnexpack::RefreshEquipCompose14()
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
void CDlgEquipAnnexpack::RefreshEquipCompose15()
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
void CDlgEquipAnnexpack::RefreshEquipCompose16()
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
void CDlgEquipAnnexpack::RefreshEquipCompose17()
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
void CDlgEquipAnnexpack::RefreshEquipCompose18()
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
void CDlgEquipAnnexpack::RefreshEquipCompose19()
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