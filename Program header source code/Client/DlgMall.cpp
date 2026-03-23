#include "public.h"

#include "GuiWare.h"
#include "MallDataMgr.h"
#include "GameScene.h"
#include "RoleItem.h"
#include "MainCharacter.h"


#include "DlgMall.h"

namespace
{
	const int BUTTON_CLOSE				= 1; //关闭按钮
	const int BUTTON_ACCOUNTRECHARGE	= 2; //充值按钮
	const int BUTTON_PAGEUP				= 3; //上一页
	const int BUTTON_PAGEDOWN			= 4; //下一页
	const int LISTBOX_WAREREGION		= 5; //引导页面
	const int STATIC_CURPAGE			= 6; //翻页1/2那个
	const int STATIC_CHONGZHI			= 7; //剩余点数

	const int GUIWARE_BASE_ID			= 11;
	const int GUIWARE_BASE_ID_BUTTON	= 14;
	const int GUIWARE_CONTROLNUM		= 5;

	const std::string STR_WAREREGION[WAREREGION_COUNT] =
	{
		std::string("商品限购区"),
	    //std::string("附件材料区"),
		//std::string("补天热卖区"),
		//std::string("坐骑系列区"),
		//std::string("天赋秘籍区"),
		//std::string("其他商品区"),
	};
}

CDlgMall::CDlgMall()
{
	FUNCTION_BEGIN;

	for (int i = 0; i < MAX_WARE_PER_PAGE; ++i)
	{
		m_pGuiWare[i] = NULL;
	}

	m_eCurRegion = WAREREGION_DISCOUNT;
	m_nCurPage = 1;

	FUNCTION_END;
}

CDlgMall::~CDlgMall()
{
	FUNCTION_BEGIN;

	for (int i = 0; i < MAX_WARE_PER_PAGE; ++i)
	{
		SAFE_DELETE(m_pGuiWare[i]);
	}

	FUNCTION_END;
}

void CDlgMall::Refresh()
{
	FUNCTION_BEGIN;

	this->ReleaseWares();

	this->InitWares();

	// 刷新当前页号显示
	if (m_pStaCurPage)
	{
		unsigned int unTotalPage = this->GetMaxPage();

		std::stringstream ss;
		ss << m_nCurPage << "/" << unTotalPage;
		m_pStaCurPage->SetText(ss.str().c_str());
	}

	FUNCTION_END
}

void CDlgMall::OnClose(void)
{
	FUNCTION_BEGIN;

	this->ReleaseWares();
	GetGameGuiManager()->m_pDlgMall = NULL;

	FUNCTION_END
}

void CDlgMall::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_pListBoxWareRegion = this->GetListBox(LISTBOX_WAREREGION);
	if (m_pListBoxWareRegion)
	{
		for (int i = 0; i < WAREREGION_COUNT; ++i)
		{
			m_pListBoxWareRegion->AddItem("", NULL);
			m_pListBoxWareRegion->SetItemText(i, 0, STR_WAREREGION[i].c_str(), -1, GuiListBoxColFmt_Center);
		}
		m_pListBoxWareRegion->SetCurItem(0);
		m_pListBoxWareRegion->SetTextHeight(27);
	}

	for (int i = 0; i < MAX_WARE_PER_PAGE; ++i)
	{
		CGuiImage* pImage = this->GetImage(GUIWARE_BASE_ID + GUIWARE_CONTROLNUM * i);
		CGuiButton* pBtn = this->GetButton(GUIWARE_BASE_ID + GUIWARE_CONTROLNUM * i + 3);
		CGuiStatic* pSta1 = this->GetStatic(GUIWARE_BASE_ID + GUIWARE_CONTROLNUM * i + 1);
		CGuiStatic* pSta2 = this->GetStatic(GUIWARE_BASE_ID + GUIWARE_CONTROLNUM * i + 4);
		CGuiTable* pTable = this->GetTable(GUIWARE_BASE_ID + GUIWARE_CONTROLNUM * i + 2);
		pTable->m_iTableType = OBJECTCELLTYPE_MALL;
		pTable->m_EquipPosition = stPointI(0, i);
		m_pGuiWare[i] = new CGuiWare(pImage, pBtn, pSta1, pSta2, pTable);
	}

	m_pStaCurPage = this->GetStatic(STATIC_CURPAGE);
	m_pStaChongzhi = this->GetStatic(STATIC_CHONGZHI); //充值点

	FUNCTION_END;
}

bool CDlgMall::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch (nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			int nIndex = (nID - GUIWARE_BASE_ID_BUTTON) / 5;
			int nMode = (nID - GUIWARE_BASE_ID_BUTTON) % 5;
			if (nMode == 0 && nIndex >= 0 && nIndex <= 14) // 新商城点击购买物品按钮 10个就写9个
			{
				int nSerialNum = (m_nCurPage - 1) * MAX_WARE_PER_PAGE + nIndex;
				DWORD dwID = GetMallDataMgr().GetWareID(m_eCurRegion, nSerialNum);

				stBuyObjectMallUserCmd cmd;
				cmd.dwObjectID = dwID;
				cmd.dwNum = 1;
				cmd.itemLevel = 0;

				SEND_USER_CMD(cmd);
			}

			switch (nID)
			{
			case BUTTON_CLOSE:
				this->Close();
				break;
			case BUTTON_ACCOUNTRECHARGE:
				break;
			case BUTTON_PAGEUP:
				{
					unsigned int unNum = GetMallDataMgr().GetWaresNum(m_eCurRegion);
					unsigned int unTotalPage = this->GetMaxPage();
					if (m_nCurPage > 1)
					{
						--m_nCurPage;
					}

					this->Refresh();
				}
				break;
			case BUTTON_PAGEDOWN:
				{
					unsigned int unNum = GetMallDataMgr().GetWaresNum(m_eCurRegion);
					unsigned int unTotalPage = this->GetMaxPage();
					if (m_nCurPage < unTotalPage)
					{
						++m_nCurPage;
					}

					this->Refresh();
				}
				break;
			}
		}
		break;
	// case EVENT_LISTBOX_SELECTION:
	// 	{
	// 		if (LISTBOX_WAREREGION == nID)
	// 		{
	// 			int nIndex = m_pListBoxWareRegion->GetCurItem();
	// 			if (nIndex != static_cast<int>(m_eCurRegion))
	// 			{
	// 				m_eCurRegion = static_cast<WAREREGION_CLASS>(nIndex);
	// 				m_nCurPage = 1;
	// 				this->Refresh();
	// 			}
	// 		}
	// 	}
	// 	break;
	// 		case 101: // 增加新商城新按钮
	// 		{
	// 			m_pListBoxWareRegion->SelectItem(5,true);
	// 		}
	// 		break;			
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

HRESULT CDlgMall::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	// 刷新当前金钱数
	DWORD dwChongzhi = GetScene()->GetMainCharacter()->GetProperty()->chongzhi; //充值点
	m_pStaChongzhi->SetText(Chongzhi2String(dwChongzhi));

	return hr;

	FUNCTION_END;
}

unsigned int CDlgMall::GetMaxPage()
{
	unsigned int unTotalPage = 0;
	unsigned int unNum = GetMallDataMgr().GetWaresNum(m_eCurRegion);
	if (unNum != 0)
	{
		unTotalPage = (unNum - 1) / MAX_WARE_PER_PAGE + 1;
	}
	
	return unTotalPage;
}

void CDlgMall::InitWares()
{
	unsigned int unNum = GetMallDataMgr().GetWaresNum(m_eCurRegion);
	unsigned int unTotalPage = this->GetMaxPage();

	unsigned int unStart = (m_nCurPage - 1) * MAX_WARE_PER_PAGE;
	unsigned int unEnd = unNum;
	if (unEnd > (unStart + MAX_WARE_PER_PAGE))
	{
		unEnd = unStart + MAX_WARE_PER_PAGE;
	}

	for (int i = 0; i < MAX_WARE_PER_PAGE; ++i)
	{
		if ((unStart + i) < unEnd)
		{
			ObjectBase_t* pObject = GetObjectBase(GetMallDataMgr().GetWareID(m_eCurRegion, unStart + i));
			if (pObject)
			{
				m_pGuiWare[i]->SetPrice(Chongzhi2String(pObject->price));
				m_pGuiWare[i]->SetName(pObject->strName);
				m_pGuiWare[i]->Show(true);

				stAddObjectPropertyUserCmd cmd;
				cmd.byActionType = EQUIPACTION_INIT;
				memset(&cmd.object,0,sizeof(cmd.object));
				strncpy(cmd.object.strName,pObject->strName,sizeof(cmd.object.strName));
				cmd.object.dwObjectID = pObject->dwID;
				cmd.object.qwThisID = INVALID_THISID;
				cmd.object.pos.dwLocation = OBJECTCELLTYPE_MALL;
				cmd.object.pos.dwTableID = 0;
				cmd.object.pos.x = 0;
				cmd.object.pos.y = i;
				cmd.object.color = -1;
				cmd.object.dwNum = 1;
				GetScene()->GetMainCharacter()->AddItem(&cmd);

				continue;
			}
		}
		m_pGuiWare[i]->Show(false);
	}
}

void CDlgMall::ReleaseWares()
{
	for (int i = 0; i < MAX_WARE_PER_PAGE; ++i)
	{
		GetScene()->GetMainCharacter()->RemoveItem(OBJECTCELLTYPE_MALL, stPointI(0, i));
	}
}

