#include "./public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include ".\guiequip.h"
#include "./table.h"
#include "./RoleItem.h"
#include "./GameState.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiItem.h"

#define IDC_CONN_IMG 2

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiEquip::CGuiEquip(void)
{
	FUNCTION_BEGIN;

	m_szCharName[0] = 0;

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiEquip::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->RemoveEquip(this);

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param b : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiEquip::SetConnVisible(bool b)
{
	FUNCTION_BEGIN;

	m_pConnImg->SetVisible(b);

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiEquip::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_pConnImg = GetImage(IDC_CONN_IMG);

	/*GetEquipTable(EQUIPCELLTYPE_NONE) = NULL;
	GetEquipTable(EQUIPCELLTYPE_NONE)->m_iTableType = OBJECTCELLTYPE_EQUIP;
	GetEquipTable(EQUIPCELLTYPE_NONE)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_NONE);*/

	GetEquipTable(EQUIPCELLTYPE_HELM) = GetTable(100);
	GetEquipTable(EQUIPCELLTYPE_HELM)->m_iTableType = OBJECTCELLTYPE_EQUIP;
	GetEquipTable(EQUIPCELLTYPE_HELM)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_HELM);

	GetEquipTable(EQUIPCELLTYPE_NECKLACE) = GetTable(101);
	GetEquipTable(EQUIPCELLTYPE_NECKLACE)->m_iTableType = OBJECTCELLTYPE_EQUIP;
	GetEquipTable(EQUIPCELLTYPE_NECKLACE)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_NECKLACE);

	GetEquipTable(EQUIPCELLTYPE_GLOVES,EQUIPCELLTYPE_RIGHT) = GetTable(102);
	GetEquipTable(EQUIPCELLTYPE_GLOVES,EQUIPCELLTYPE_RIGHT)->m_iTableType = OBJECTCELLTYPE_EQUIP;
	GetEquipTable(EQUIPCELLTYPE_GLOVES,EQUIPCELLTYPE_RIGHT)->m_EquipPosition = stPointI(EQUIPCELLTYPE_RIGHT,EQUIPCELLTYPE_GLOVES);

	GetEquipTable(EQUIPCELLTYPE_GLOVES,EQUIPCELLTYPE_LEFT) = GetTable(103);
	GetEquipTable(EQUIPCELLTYPE_GLOVES,EQUIPCELLTYPE_LEFT)->m_iTableType = OBJECTCELLTYPE_EQUIP;
	GetEquipTable(EQUIPCELLTYPE_GLOVES,EQUIPCELLTYPE_LEFT)->m_EquipPosition = stPointI(EQUIPCELLTYPE_LEFT,EQUIPCELLTYPE_GLOVES);

	GetEquipTable(EQUIPCELLTYPE_RING,EQUIPCELLTYPE_RIGHT) = GetTable(104);
	GetEquipTable(EQUIPCELLTYPE_RING,EQUIPCELLTYPE_RIGHT)->m_iTableType = OBJECTCELLTYPE_EQUIP;
	GetEquipTable(EQUIPCELLTYPE_RING,EQUIPCELLTYPE_RIGHT)->m_EquipPosition = stPointI(EQUIPCELLTYPE_RIGHT,EQUIPCELLTYPE_RING);

	GetEquipTable(EQUIPCELLTYPE_RING,EQUIPCELLTYPE_LEFT) = GetTable(105);
	GetEquipTable(EQUIPCELLTYPE_RING,EQUIPCELLTYPE_LEFT)->m_iTableType = OBJECTCELLTYPE_EQUIP;
	GetEquipTable(EQUIPCELLTYPE_RING,EQUIPCELLTYPE_LEFT)->m_EquipPosition = stPointI(EQUIPCELLTYPE_LEFT,EQUIPCELLTYPE_RING);

	GetEquipTable(EQUIPCELLTYPE_SHOES) = GetTable(106);
	GetEquipTable(EQUIPCELLTYPE_SHOES)->m_iTableType = OBJECTCELLTYPE_EQUIP;
	GetEquipTable(EQUIPCELLTYPE_SHOES)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SHOES);

	GetEquipTable(EQUIPCELLTYPE_BELT) = GetTable(107);
	GetEquipTable(EQUIPCELLTYPE_BELT)->m_iTableType = OBJECTCELLTYPE_EQUIP;
	GetEquipTable(EQUIPCELLTYPE_BELT)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_BELT);

	GetEquipTable(EQUIPCELLTYPE_BODY) = GetTable(108);
	GetEquipTable(EQUIPCELLTYPE_BODY)->m_iTableType = OBJECTCELLTYPE_EQUIP;
	GetEquipTable(EQUIPCELLTYPE_BODY)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_BODY);

	GetEquipTable(EQUIPCELLTYPE_HANDR) = GetTable(109);
	GetEquipTable(EQUIPCELLTYPE_HANDR)->m_iTableType = OBJECTCELLTYPE_EQUIP;
	GetEquipTable(EQUIPCELLTYPE_HANDR)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_HANDR);

	GetEquipTable(EQUIPCELLTYPE_HANDL) = GetTable(110);
	GetEquipTable(EQUIPCELLTYPE_HANDL)->m_iTableType = OBJECTCELLTYPE_EQUIP;
	GetEquipTable(EQUIPCELLTYPE_HANDL)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_HANDL);

	/*GetEquipTable(EQUIPCELLTYPE_BODY)->m_cellOffset = stPointI(23,36);
	GetEquipTable(EQUIPCELLTYPE_HANDR)->m_cellOffset = stPointI(23,36);
	GetEquipTable(EQUIPCELLTYPE_HANDL)->m_cellOffset = stPointI(23,36);*/

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param type : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
	FUNCTION_END;
}
/*
void CGuiEquip::SetCharType(int type)
{
	FUNCTION_BEGIN;

	const stCharTypeInfo& info = GetCharTypeInfo(type);


	GetBackImage()->SetImage(stResourceLocation(GetGuiGraphicPackName(),12,info.background));

	stResourceLocation rl;
	rl.SetFileName(c_szItemGrpPack);
	rl.group = c_nGroupItemEquip;
	rl.frame = info.equippic;
	m_aniBody[0].Create(&rl);
	rl.group = c_nGroupItemEquipPattern;
	m_aniBody[1].Create(&rl);
	rl.group = c_nGroupItemEquipPattern2;
	m_aniBody[2].Create(&rl);

	FUNCTION_END;
}
*/

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiEquip::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
	CGuiTable* pTable;

	pTable = GetEquipTable(EQUIPCELLTYPE_BODY);

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	stRectI rect(74,7,181,26);
	rect.OffsetRect(m_x,m_y);
	GetDevice()->DrawString(GetCharName(),rect,ColorBlend(m_dwColor,0xffffffff),DT_VCENTER|DT_CENTER);

	
	return hr;

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pTable : ÃèÊö
 * \param ptRender : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
/*
void CGuiEquip::BodyRender(CGuiTable* pTable,stPointI ptRender)
{
	FUNCTION_BEGIN;

	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();

	if(pTable->IsVisible())
	{
		if(!pTable->GetItemCell(0,0))
		{
			for(size_t i=0;i<count_of(m_aniBody);++i)
			{
				stPointI pt = pTable->GetLocation();
				pt.x += pTable->m_cellOffset.x;
				pt.y += pTable->m_cellOffset.y;
				pt.x += ptRender.x;
				pt.y += ptRender.y;
				m_aniBody[i].Render(pt.x,pt.y);
			}
		}
	}

	if(pMainRole->GetHairType() != 0)
	{
		const stCharTypeInfo& info = GetCharTypeInfo(pMainRole->GetType());
		stResourceLocation rl("data\\items.gl",info.equipHairPic,pMainRole->GetHairType());
		IBitmaps* pBmps = GetDevice()->FindBitmaps(&rl);
		if(pBmps)
		{
			stPointI pt = pTable->GetLocation();
			pt.x += pTable->m_cellOffset.x;
			pt.y += pTable->m_cellOffset.y;
			pt.x += ptRender.x;
			pt.y += ptRender.y;
			pBmps->RenderAni(pt.x,pt.y,timeGetTime(),pMainRole->GetHairColor());
		}
	}

	FUNCTION_END;
}
*/

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pData : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiEquip::SetEquipData(void* pData)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nEvent : ÃèÊö
 * \param nID : ÃèÊö
 * \param pControl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiEquip::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_DIALOG_MOVE:
		if(m_pConnImg->IsVisible())
		{
			CGuiDialog* pDlg = GetGameGuiManager()->m_guiItem;
			pDlg->SetLocation(GetX() + GetWidth(),GetY() + (GetHeight() - pDlg->GetHeight())/2);
		}
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}