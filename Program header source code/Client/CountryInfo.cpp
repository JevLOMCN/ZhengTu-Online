/*
     All the gui about country was inclued in this file. 
     Each Gui own a corresponding data class which was global,whilst gui was deleted 
     when you close the dialog.

     GuiDialog Kinds of country includes:
		CGuiModifyRevenueBox    :  modify the revenue 
		CGuiCountrySavingBoxDlg :  materials which were contributed will be save here
                CGuiCountryTechDlg      :  technique includes studying,ballot,researcher.
                CGuiCountry             :  browse the country     '
		CGuiQueryGenDlg  : Query the experience and ability of gen 
*/

#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./CountryInfo.h"
#include "./GuiItem.h"
#include "./Country.h"
#include "./GuiWorldConn.h"
#include "./RoleItem.h"
#include "./GameGuiFloatItem.h"
#include "table.h"
#include "GameScene.h"
#include "MainCharacter.h"
#include "./GuiNpcDialog.h"

#define   SAFE_CHECK_COUNTRY_MSG(pMsg)  assert(pMsg!=NULL);  if(NULL == pMsg)  return;
           
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//         查询大将军王的能力和经验
/////////////////////////////////////////////////////////////////////

#define  ID_QUERY_STATIC_Level		1
#define  ID_QUERY_PROCESS			2
#define  ID_QUERY_STATIC_EXP			3
#define  ID_QUERY_BUTTON_CLOSE         4  

CGuiQueryGenDlg::CGuiQueryGenDlg(void)
: m_pExpProcess(NULL)
, m_pLevelStatic(NULL)
, m_pExpStatic(NULL)
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiQueryGenDlg::~CGuiQueryGenDlg(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;	
}



void CGuiQueryGenDlg::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_pExpProcess = GetProcess(ID_QUERY_PROCESS);
	m_pExpStatic = GetStatic(ID_QUERY_STATIC_EXP);
	m_pLevelStatic = GetStatic(ID_QUERY_STATIC_Level);
 
	SetStyle(GUI_WS_TOPMOST | GUI_WS_MODAL);

	CCountryConn::Instance()->RequestForGenInfo();

	UpdataUI();

	FUNCTION_END;
}


bool CGuiQueryGenDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case ID_QUERY_BUTTON_CLOSE:  
			{
				Close();
			}
			return true;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiQueryGenDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	CCountryConn::Instance()->m_guiGenInfoDlg = NULL;
	GetGameGuiManager()->OnNpcActionEnd();

	FUNCTION_END;
}

void CGuiQueryGenDlg::UpdataUI()
{
	FUNCTION_BEGIN;

	const stGenInfo & info = CCountryConn::Instance()->GetGenInfo();
	if(info.maxExp == 0)
	{
		m_pExpProcess->SetRange(0,1);
	}
	else 
		m_pExpProcess->SetRange(0,info.maxExp);

	m_pExpProcess->SetValue(info.exp);
		
	char szExp[50] = {0};
	_snprintf(szExp,50,"%d/%d",info.exp,info.maxExp);
	szExp[49] = 0;
	m_pExpStatic->SetText(szExp);

	char szLevel[50] = {0};
	_snprintf(szLevel,50,"大将军等级  %d 级",info.level);
	szLevel[49] = 0;
	m_pLevelStatic->SetText(szLevel);

	
	FUNCTION_END;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//         国王调整税率的界面
/////////////////////////////////////////////////////////////////////

#define  ID_STATIC      1
#define  ID_SLIDER      2
#define  ID_BTN_OK      3
#define  ID_BTN_CANCEL  4 

CGuiModifyRevenueBox::CGuiModifyRevenueBox(void)
: m_pCurRevenueSlider(NULL)
, m_pCurRevenueStatic(NULL)
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiModifyRevenueBox::~CGuiModifyRevenueBox(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;	
}

void CGuiModifyRevenueBox::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_pCurRevenueSlider = GetSlider(ID_SLIDER);
	m_pCurRevenueSlider->SetValue(CCountry::Instance()->GetTax());

	m_pCurRevenueStatic = GetStatic(ID_STATIC);

	char szLabel[20] = {0};
	sprintf(szLabel,"当前税:%d",CCountry::Instance()->GetTax());
	strcat(szLabel,"%");
	szLabel[MAX_PATH-1] = 0;

	m_pCurRevenueStatic->SetText(szLabel);


	SetStyle(GUI_WS_TOPMOST | GUI_WS_MODAL);

	FUNCTION_END;
}


bool CGuiModifyRevenueBox::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case ID_BTN_OK:  
			{
				CCountry::Instance()->SetTax(m_pCurRevenueSlider->GetValue());

				// send to server 
				stSetTaxCountryUserCmd cmd;
				cmd.dwCountry = GetScene()->GetMainCharacter()->GetCountry();
				cmd.byTax = CCountry::Instance()->GetTax();
				SEND_USER_CMD(cmd);

				if(GetGameGuiManager()->m_guiNpcDialog)
				{
					GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;
				}
				Close();

				return true;
			}
		case ID_BTN_CANCEL:
			Close();
			return true;
		}
	}


	if( nEvent == EVENT_SLIDER_VALUE_CHANGED)
	{
		if ( nID == ID_SLIDER )
		{
			char szLabel[20] = {0};
			sprintf(szLabel,"当前税:%d",m_pCurRevenueSlider->GetValue());
			strcat(szLabel,"%");
			szLabel[MAX_PATH-1] = 0;
		        m_pCurRevenueStatic->SetText(szLabel);
		}
		return true;
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiModifyRevenueBox::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiModifyRevenueBox = NULL;
	if(GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

	FUNCTION_END;
}

void CGuiModifyRevenueBox::UpdateCountryMoney()
{
	FUNCTION_BEGIN;

	char szLabel[20] = {0};
	sprintf(szLabel,"当前税:%d",CCountry::Instance()->GetTax());
	strcat(szLabel,"%");
	szLabel[MAX_PATH-1] = 0;
	m_pCurRevenueStatic->SetText(szLabel);
	m_pCurRevenueSlider->SetValue(CCountry::Instance()->GetTax());

	FUNCTION_END;
}


//////////////////////////////////////////////////////////////////////
//         捐献到国家仺库的界面
//////////////////////////////////////////////////////////////////////

#define IDC_COUNTRY_SAVEBOX_TABLE 3
#define IDC_COUNTRY_BUTTON_OK     5 
#define IDC_COUNTRY_BUTTON_CANCEL 6 

#define IDC_COUNTRY_STATIC_POINT 4

CGuiCountrySavingBoxDlg::CGuiCountrySavingBoxDlg(void)
: m_pStatic(NULL)
{
	FUNCTION_BEGIN;

	//m_bSmDialog = true;

	FUNCTION_END;
}


void CGuiCountrySavingBoxDlg::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_pTable = GetTable(IDC_COUNTRY_SAVEBOX_TABLE);
	m_pTable->m_iTableType = OBJECTCELLTYPE_COUNTRY_SAVEBOX;
	m_pTable->m_iTableID = IDC_COUNTRY_SAVEBOX_TABLE;


	m_pStatic = GetStatic(IDC_COUNTRY_STATIC_POINT);

	m_ptIcon.x = 33;
	m_ptIcon.y = 90;


	FUNCTION_END;
}

void CGuiCountrySavingBoxDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiCountrySaveingBoxDlg=NULL;

	if(GetGameGuiManager()->m_guiItem)
	{
		GetGameGuiManager()->m_guiItem->SetVisible(false);
	}

	GetGameGuiManager()->OnNpcActionEnd();

	FUNCTION_END;
}

 
void CGuiCountrySavingBoxDlg::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{	
		stPointI pt(m_pBtnClose->GetLocation());
		m_pBtnClose->SetLocation(pt.x ,pt.y + 4);
	}

	FUNCTION_END;
}

//---------------------------------------------------------------------------
//     从格子中拿起一个物品，如果有的话
//---------------------------------------------------------------------------
void CGuiCountrySavingBoxDlg::PickUpRoleItem()
{
	FUNCTION_BEGIN;

	// 取消这个镜象
	CGuiItemCell* pOldCell = m_pTable->GetItemCell(0,0);
	if( !pOldCell ) return;
	CRoleItem * pOldItem = NULL;
	
	pOldItem = (CRoleItem *)pOldCell->m_pItemData;
	if( !pOldItem) return;
	pOldItem->MoveBackOldLocation();
	
	// 新添一个镜象到鼠标
	CGuiItemCell* pTrueCell = NULL;
	int count = 0;
	pOldItem->GetTrueCell(pTrueCell,count);

	stObjectLocation dst;
	dst.dwLocation = OBJECTCELLTYPE_MOUSE;
	dst.dwTableID = 0;
	dst.x = 0;
	dst.y = 0;
	pOldItem->AddShowLocation(pTrueCell,count,dst,true,true);

	FUNCTION_END;
}

//-------------------------------------------------------------------
//      放下一个物品到格子中
//-------------------------------------------------------------------
bool CGuiCountrySavingBoxDlg::PutDownRoleItem(CGuiItemCell * pSrcItemCell)
{
	FUNCTION_BEGIN;
    
	if( !pSrcItemCell ) return false;
	CRoleItem * pSrcItem =(CRoleItem *)pSrcItemCell->m_pItemData;
	if( !pSrcItem )  return false;

	// 如果原来已有,则取消格子中现有的镜象
	CGuiItemCell* pOldCell = m_pTable->GetItemCell(0,0);
	if(pOldCell)
	{
		CRoleItem * pOldItem = (CRoleItem *)pOldCell->m_pItemData;
		pOldItem->MoveBackOldLocation();
		/*pOldItem->OnRemoveCell(pOldCell);
		if(pSrcItemCell != pSrcTrueCell)
		{
			pSrcItemCell->m_pTable->RemoveItem(pSrcItemCell->GetPos().x,pSrcItemCell->GetPos().y,true);
		}*/
	}

	// 要放下的物品少了一个镜象 
	pSrcItem->MoveBackOldLocation();
	
	/*pSrcItem->OnRemoveCell(pSrcItemCell);
	if(pSrcItemCell != pSrcTrueCell)
	{
		pSrcItemCell->m_pTable->RemoveItem(pSrcItemCell->GetPos().x,pSrcItemCell->GetPos().y,true);
	}
	*/

	// 设置新的镜象 
	stObjectLocation dst;
	dst.dwLocation = OBJECTCELLTYPE_COUNTRY_SAVEBOX;
	dst.dwTableID = IDC_COUNTRY_SAVEBOX_TABLE;
	dst.x = 0;
	dst.y = 0;
	CGuiItemCell* pSrcTrueCell = NULL;
	int count1 = 0;
	pSrcItem->GetTrueCell(pSrcTrueCell,count1);
	pSrcItem->AddShowLocation(pSrcTrueCell,count1,dst,true,true);

	return true;

	FUNCTION_END;
}

//---------------------------------------------------------------------------
//      左键点击在格子上，如果鼠标上有物品则放下，否则拿起格子中原有的
//      物品，如果有的话.
//---------------------------------------------------------------------------
void CGuiCountrySavingBoxDlg::OnLeftClickedOnItem()
{
	FUNCTION_BEGIN;

	CGuiFloatItem* pFloatItem = GetGuiManager()->GetFloatItem();
	if ( pFloatItem ) 
	{
		CGuiItemCell* pSrcItemCell = (CGuiItemCell*)pFloatItem->GetItem();

		if( !pSrcItemCell) return;
		CRoleItem * pSrcItem =(CRoleItem *)pSrcItemCell->m_pItemData;
		if( !pSrcItem )  return;

               
		CountryMaterial_t materail;
		if( !CanCountributeToCountry(pSrcItem,materail) ) return;
		if( !PutDownRoleItem(pSrcItemCell)) return;

		DWORD dwPoint = GetCountributePoint(pSrcItem,materail);
                if( m_pStatic) 
		{
			float fExpolit = 0.00;
			if (materail.dwMaterialType == 3)
			{
				fExpolit = (float)dwPoint*2;
			}
			else 
				fExpolit = (float)((float)dwPoint/50);

			char szPoint[256] = {0};
			// _snprintf(szPoint,sizeof(szPoint),"该原料将给国家增加 %.2f 单位原料,\n你将获得%.2f个人功勋值\n",...) ;; 不行，你将获得%.2f个人功勋值显示不出来，去掉\n就可以
			if( pSrcItem->GetObjectBase()->dwType == ItemType_Resource)
				_snprintf(szPoint,sizeof(szPoint),"该原料将给国家增加 %.2f 单位原料,你将获得%.2f个人功勋值\n",(float)(dwPoint),fExpolit);
			else 
				_snprintf(szPoint,sizeof(szPoint),"该物资将给国家增加 %.2f 单位物资,你将获得%.2f个人功勋值\n",(float)(dwPoint),fExpolit);
			szPoint[sizeof(szPoint)-1] = 0;

			m_pStatic->SetText(szPoint);
		}
	}
	else
	{	 
		m_pStatic->SetText("");
		PickUpRoleItem();
	}

	FUNCTION_END;
}


//---------------------------------------------------------------------------------------------
// 函数:
//    CanCountributeToCountry
//    getMaterialKind
//    GetCountributePoint
//    从服务器借鉴，要跟服务器保持一致
//    
//---------------------------------------------------------------------------------------------
bool CGuiCountrySavingBoxDlg::CanCountributeToCountry(CRoleItem * pRoleItem,CountryMaterial_t & outter)
{
	FUNCTION_BEGIN;

	if( !pRoleItem )  return false;

	for(tTableCountryMaterialList::iterator it = g_CountryMatrialList.begin();
		it != g_CountryMatrialList.end() ;++it)
	{
		const CountryMaterial_t & material = *it;
		if( (enumItemType)material.dwObjectType == pRoleItem->GetItemType() && material.dwObjectID == pRoleItem->GetObjectID() )
		{
			outter = *it;
			return true;
		}
	}
	
	GetGameGuiManager()->AddClientSystemMessage("你不能捐赠这件物品！");
	
	return false;	   


	FUNCTION_END;
}

bool CGuiCountrySavingBoxDlg::getMaterialKind(CRoleItem * pRoleItem,DWORD & dwOutter)
{
	if( !pRoleItem) return false;

	DWORD ret = 0;
	
	if( pRoleItem->GetItemType() == ItemType_Resource)
	{
		DWORD dwObjectID = pRoleItem->GetObjectID();

		if ( dwObjectID == 501 || dwObjectID == 502 || dwObjectID == 506 || dwObjectID == 507)
		{                       
			ret = 1;        
		}                               
		else if (dwObjectID == 511 || dwObjectID == 512 || dwObjectID == 516 || dwObjectID == 517)
		{
			ret = 2;                
		}                               
		else if (dwObjectID == 521 || dwObjectID == 522 || dwObjectID == 526 || dwObjectID == 527)
		{
			ret = 3;
		}
		else if (dwObjectID == 531 || dwObjectID == 532 || dwObjectID == 536 || dwObjectID == 537)
		{
			ret = 4;
		}
		else if (dwObjectID == 541 || dwObjectID == 542 || dwObjectID == 546 || dwObjectID == 547)
		{
			ret = 5;
		}
		else if (dwObjectID == 554 || dwObjectID == 555 || dwObjectID == 559 || dwObjectID == 560)
		{
			ret = 6;
		}
	}
	 
        dwOutter = ret;

	return true;
}

DWORD CGuiCountrySavingBoxDlg::GetCountributePoint(CRoleItem * pRoleItem,const CountryMaterial_t & country_materail)
{
	FUNCTION_BEGIN;

	if( !pRoleItem )  return 0;

        DWORD dwMaterailLevel ;
	if( !getMaterialKind(pRoleItem,dwMaterailLevel) ) return 0;  

	t_Object* pObject = pRoleItem->GetObject();
	DWORD dwValue = 0;
	if (dwMaterailLevel>0){
		
		if (country_materail.dwMaterialType == 1)
		{//打造出来的高等级材料
			dwValue = (DWORD)pow(5,pObject->upgrade+1) * pObject->dwNum;
		}
		else
		{//普通材料
			dwValue = pObject->dwNum;
		}
	}
	else
	{
		if (country_materail.dwMaterialType == 3)//贡品
		{
			dwValue = pObject->dwNum;
		}
		else
		{
			dwValue = (pObject->price*pObject->dwNum)/100;
		}
	}

	return dwValue;

	FUNCTION_END;
}

bool CGuiCountrySavingBoxDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID)
			{
			 
			case GUI_CLOSEBOX_ID:
				{
					if(GetGameGuiManager()->m_guiItem)
					{
						GetGameGuiManager()->m_guiItem->SetVisible(false);
					}
					GetGameGuiManager()->OnNpcActionEnd();
					SetVisible(false);
					
					MoveBackToPackage();



					Close();

					return true;
				}
				break;
			case IDC_COUNTRY_BUTTON_OK:
				{ 
					CGuiItemCell* pCell = m_pTable->GetItemCell(0,0); 
					if(pCell) 
					{
						CRoleItem * pRoleItem = (CRoleItem *)pCell->m_pItemData;	

						if( pRoleItem)
						{
							pRoleItem->MoveBackOldLocation();

							stContributeCountryMaterialCmd cmd;
							cmd.itemID = (DWORD)pRoleItem->GetThisID();
							SEND_USER_CMD(cmd);

							
						}						
						 
					}
					Close();
				}
				break;
			case IDC_COUNTRY_BUTTON_CANCEL:
				{
					MoveBackToPackage();
					Close();
				}
				break;
			}
		}

		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}
 
void CGuiCountrySavingBoxDlg::MoveBackToPackage()
{
	FUNCTION_BEGIN;

	CGuiItemCell* pCell = m_pTable->GetItemCell(0,0); 
	if(pCell) 
	{
		CRoleItem * pRoleItem = (CRoleItem *)pCell->m_pItemData;	

		if( pRoleItem)
		{
			pRoleItem->MoveBackOldLocation();
		}				
	}

	FUNCTION_END;
}


//////////////////////////////////////////////////////////////////////
//         国家科技数据
/////////////////////////////////////////////////////////////////////
static CCountryTech s_countrytech; 
CCountryTech* CCountryTech::Singleton = 0;


CCountryTech::CCountryTech()
{
	FUNCTION_BEGIN;

	Assert(0 == Singleton);
	Singleton = this;

	FUNCTION_END;
}

CCountryTech::~CCountryTech()
{
	FUNCTION_BEGIN;

	Singleton = 0;
	Release();

	FUNCTION_END;
}

void CCountryTech::Release()
{
	FUNCTION_BEGIN;

	FreePointerList<tVecStudyingItem>(m_vecStudyingItem);
	FreePointerList<tVecBallotItem>(m_vecBallotItem);
	FreePointerList<tVecResearcher>(m_vecReasearchItem);
	FreePointerList<tVecTechniqueItem>(m_vecWaitingTech);

	FUNCTION_END;
}

void CCountryTech::Debug()
{
#if 0 
	for( int i = 0 ; i< 30; i++)
	{
		stStudying * pTemp1 = new stStudying;
		stTechBallot * pTemp2 = new stTechBallot;
		stWaitingTech * pTemp3 = new stWaitingTech;

	/*	pTemp1->dwFinished = i;
		pTemp2->dwBallot = i;

		char szTest[20] = {0};
		sprintf(szTest,"研究员@@%d",i);
		strcpy(pTemp1->szWaitingTech,szTest);
		pTemp3->vecResearcher.push_back(stResearcher::_String(szTest));

		sprintf(szTest,"研究员##%d",i);
		pTemp3->vecResearcher.push_back(stResearcher::_String(szTest));

		sprintf(szTest,"研究员!!!!%d",i);
		pTemp3->vecResearcher.push_back(stResearcher::_String(szTest));
		
		memset(szTest,20,sizeof(char));
		sprintf(szTest,"科技%d",i);
		strcpy(pTemp1->szTech,szTest);
		strcpy(pTemp2->szTech,szTest);
		strcpy(pTemp3->szTech,szTest);
		pTemp2->bChecked = false;*/

	//	m_vecStudyingItem.push_back(pTemp1);		
	//	m_vecBallotItem.push_back(pTemp2);
	//	m_vecReasearchItem.push_back(pTemp3);
	}
#endif 
	
}
 
//---------------------------------------------------------------
// 从服务器接收投票列表
//---------------------------------------------------------------
void CCountryTech::SetVoteList(const stReturnVoteListUserCmd * pCmd)
{
	FUNCTION_BEGIN;

	SAFE_CHECK_COUNTRY_MSG(pCmd);

	FreePointerList<tVecBallotItem>(m_vecBallotItem);


	m_dwVoteID = pCmd->dwVoteID;
	m_byVoteType = pCmd->byType;

	for (int i=0;i<pCmd->dwSize;++i)
	{
		stTechBallot * st = new stTechBallot;
		if( !st) continue;
		st->bChecked = false;
		st->dwOption = pCmd->data[i].dwOption;
		st->dwBallot = pCmd->data[i].dwBallot;
		st->strTech = pCmd->data[i].szOptionDesc;
		m_vecBallotItem.push_back(st);
	}

	if( GetGameGuiManager()->m_guiCountryTechDlg && GetGameGuiManager()->m_guiCountryTechDlg->IsVisible())
	{
		GetGameGuiManager()->m_guiCountryTechDlg->RefreshCurControl();
	}

	FUNCTION_END;
}

//---------------------------------------------------------------
// 从服务器接收待选官员列表
//---------------------------------------------------------------
void CCountryTech::SetResearcherList(const stRtnWaitOfficialUserCmd * pCmd)
{
	FUNCTION_BEGIN;
        
	SAFE_CHECK_COUNTRY_MSG(pCmd);

	FreePointerList<tVecResearcher>(m_vecReasearchItem);

	for (int i=0;i<pCmd->dwSize;++i)
	{
		_stWaitOfficialItem * st = new _stWaitOfficialItem;
		if( !st) continue;
		st->dwCharID = pCmd->data[i].dwCharID;
		st->strResearcher = pCmd->data[i].szName;
		 
		m_vecReasearchItem.push_back(st);
	}

	if( GetGameGuiManager()->m_guiCountryTechDlg && GetGameGuiManager()->m_guiCountryTechDlg->IsVisible())
	{
		GetGameGuiManager()->m_guiCountryTechDlg->RefreshCurControl();
	}

	FUNCTION_END;
}
//---------------------------------------------------------------
// 根据id得到候选研究员的名字
//---------------------------------------------------------------
bool CCountryTech::GetWaitOfficalName(DWORD dwCharID,std::string & strOutter)
{

	for(size_t i = 0 ; i < m_vecReasearchItem.size() ; ++i)
	{
		const _stWaitOfficialItem * item = m_vecReasearchItem[i];
		if(item)
		{
			if(item->dwCharID == dwCharID)
			{
				strOutter = item->strResearcher;
				return true;
			}
		}
	}
	return false;
}
//---------------------------------------------------------------
// 国王指定研究员时候，不能指定一个研究员同时为所有等待被指派研究员的多项科技的研究员
// ,也不能指定正在研究中的科技研究员
//---------------------------------------------------------------

CCountryTech::eResult_SpecifyForWatingTech  CCountryTech::CanSpecifyForWaitingTech()
{
    FUNCTION_BEGIN;

    bool bAllNotSpecified = true;

    for(size_t i = 0 ; i < m_vecWaitingTech.size(); ++i )
    {
          const stWaitingTech * pTech1 = m_vecWaitingTech[i];
	  if( pTech1->dwCharID == 0 )
		  continue;
	  bAllNotSpecified = false;
	  for(size_t j = i+1 ; j < m_vecWaitingTech.size() ; j++)
	  {
		  const stWaitingTech * pTech2 = m_vecWaitingTech[j];
		  if( pTech2->dwCharID == 0 )
			  continue;
		  if( pTech1->dwCharID == pTech2->dwCharID )
			  return eResult_HasSomeRepeat;
	  }
	  for(size_t k = 0 ; k< m_vecStudyingItem.size(); ++k)
	  {
		  const stStudying * pStudying = m_vecStudyingItem[k];
		  if(pStudying)
		  {
			  std::string strName;
			  if( GetWaitOfficalName(pTech1->dwCharID,strName))
			  {
				  if(0 == strcmp(pStudying->strResearcher.c_str(),strName.c_str()))
				  {
                                           return  eResult_HasSomeExistResearcher;
				  }
			  }
		  }
	  }
    } 
    if( bAllNotSpecified )
    {
	    return eResult_AllNotSpecified;
    }
    else 
    {
	    return eResult_Succeeded;
    }

    FUNCTION_END;
}

//---------------------------------------------------------------
// 当科技人员来升级时，要判断这个科技人员是否研究的是科技dwTechID
//---------------------------------------------------------------
bool  CCountryTech::IsResearcherLegal(DWORD dwTechID)
{
	FUNCTION_BEGIN;
	for( size_t i =  0 ;  i< m_vecStudyingItem.size() ; ++i )
	{
                  const  stStudying * pTemp  = m_vecStudyingItem[i];
		  if( pTemp )
		  {
			  if( pTemp->dwTechOption == dwTechID 
				  && 0 == strcmp(pTemp->strResearcher.c_str(),GetScene()->GetMainCharacter()->GetName())
		            )
			    return true;
		  }
	}
	return false;
	FUNCTION_END;
}
//---------------------------------------------------------------
// 从服务器接收  [正在进行升级]  科技列表
//---------------------------------------------------------------
void CCountryTech::SetActiveTechList(const stRtnTechUserCmd * pCmd)
{
	FUNCTION_BEGIN;

	SAFE_CHECK_COUNTRY_MSG(pCmd);

	if( pCmd->dwType !=  ACTIVE_TECH_TYPE)  return;

	FreePointerList<tVecStudyingItem>(m_vecStudyingItem);


	/*  科技等级	对应装备等级	需要国库资金(两)	需要捐献度	需要矿
	1		80	0			0		0
	2		90	320000			80000		160000
	3		100	640000			160000		320000
	4		110	1280000			320000		640000
	5		120	2560000			640000		1280000
	6		130	5120000			1280000		2560000
	7		140	10240000		2560000		5120000
	8		150	20480000		5120000		10240000
	*/

	stStudying::stTipMap   c_TipMap[] = 
	{
		{1,5000,	2000,	2000},
		{2,10000,	4000,  4000},
		{3,15000,	6000, 6000},
		{4,20000,	8000, 8000},
		{5,25000,	10000, 10000},
		{6,30000,	12000,12000},
		{7,35000,	14000,14000},
		{8,40000,	16000,16000},
	};
	
	for (int i=0 ; i < pCmd->dwSize ; ++i )
	{
		stStudying * st= new stStudying;

		// tech id(option)
		st->dwTechOption = pCmd->data[i].dwOption;          

		// tech name 
		st->strTechName  = pCmd->data[i].szOptionDesc;

		// tech szResearcher 
		st->strResearcher =  pCmd->data[i].szResearchName;

		// tech finished 
		st->dwFinished = pCmd->data[i].dwProgress;

		// tooltips 
		DWORD dwNextLevel = pCmd->data[i].dwLevel + 1;
		if( dwNextLevel >=1 && dwNextLevel <= count_of(c_TipMap))
		{
			st->stTip.dwNeedContribute = c_TipMap[dwNextLevel-1].dwNeedContribute * 0.02;
			st->stTip.dwNeedMaterial = c_TipMap[dwNextLevel-1].dwNeedMaterial * 0.02;
			st->stTip.dwNeedMoney = c_TipMap[dwNextLevel-1].dwNeedMoney * 0.02;
		}

		st->stTip.dwNextLevel = dwNextLevel;

		m_vecStudyingItem.push_back(st);
	}	

	if( GetGameGuiManager()->m_guiCountryTechDlg && GetGameGuiManager()->m_guiCountryTechDlg->IsVisible())
	{
		GetGameGuiManager()->m_guiCountryTechDlg->RefreshCurControl();
	}

        FUNCTION_END;
}

//---------------------------------------------------------------
// 从服务器接收   [投票选出的科技，等待国王选择官员]  科技列表
//---------------------------------------------------------------
void CCountryTech::SetWatingTechList(const stRtnTechUserCmd * pCmd)
{
	FUNCTION_BEGIN;

	SAFE_CHECK_COUNTRY_MSG(pCmd);

	if( pCmd->dwType !=  WAIT_TECH_TYPE)  return;

	FreePointerList<tVecTechniqueItem>(m_vecWaitingTech);

	for (int i=0;i<pCmd->dwSize;++i)
	{
		stWaitingTech * st = new stWaitingTech;
		if( !st) continue;

		// tech id 
		st->dwTechOption  = pCmd->data[i].dwOption;

		// tech desc 
		st->strTechName = pCmd->data[i].szOptionDesc;

		m_vecWaitingTech.push_back(st);
	}

	if( GetGameGuiManager()->m_guiCountryTechDlg && GetGameGuiManager()->m_guiCountryTechDlg->IsVisible())
	{
		GetGameGuiManager()->m_guiCountryTechDlg->RefreshCurControl();
	}

	FUNCTION_END;
}

void CCountryTech::EchoKingSetResearcherEvent( const stConfirmSearcherUserCmd * pCmd )
{
	FUNCTION_BEGIN;

	//for(size_t i = 0 ;  i<  m_vecWaitingTech.size() ; i++)
	//{
	//	const stWaitingTech *  pTemp = m_vecWaitingTech[i];
 //               if(pTemp && pTemp->dwTechOption == pCmd->dwTechID)
	//	{
	//		char szTip[100];
	//		_snprintf(szTip,sizeof(szTip),"国王指派你为%s的研究员，你同意吗?",pTemp->strTechName.c_str());
	//		szTip[sizeof(szTip)-1] = 0;
	//		GetGameGuiManager()->AddMessageBox(szTip,eAction_BecomeResearcher,(void *)pTemp->dwTechOption,c_szMBOKCancel);
	//		break;
	//	}
	//}
	char szTip[100];
	_snprintf(szTip,sizeof(szTip),"国王指派你为研究员，你同意吗?");
	szTip[sizeof(szTip)-1] = 0;
	GetGameGuiManager()->AddMessageBox(szTip,eAction_BecomeResearcher,(void *)pCmd->dwTechID,c_szMBOKCancel);

	FUNCTION_END;
}

//----------------------------------------------------------------------------



// bollot msg request and process 
void RequestBallotFromServer()
{
	stRequestVoteListUserCmd cmd;
	cmd.byType = TECH_VOTE;
	SEND_USER_CMD(cmd);
}

void ConfirmBallotToServer(DWORD dwVoteOption)
{
	stCommitVoteUserCmd cmd;
	cmd.dwVoteID = CCountryTech::Instance()->GetVoteID();
	cmd.dwOption = dwVoteOption;
	SEND_USER_CMD(cmd);
}
// researcher msg request and process
void RequestResearcherFromServer(const DWORD dwTechId,const DWORD dwCharID)
{
	stSetTechSearchUserCmd cmd;
	cmd.dwOption = dwTechId;
	cmd.dwCharID = dwCharID;
	SEND_USER_CMD(cmd);
}

// tech 
void RequestActiveTech()  // 正在进行升级
{
	stReqTechUserCmd cmd;
	cmd.dwType = ACTIVE_TECH_TYPE;
	SEND_USER_CMD(cmd);
}


//king sepecify researcher 
void RequestWaitingTech() //投票选出的科技，等待国王选择官员
{
	stReqTechUserCmd cmd;
	cmd.dwType = WAIT_TECH_TYPE;
	SEND_USER_CMD(cmd);
}
void RequestResearcherList()  // 请求研究员列表
{
	stReqWaitOfficialUserCmd cmd;
	SEND_USER_CMD(cmd);
}

// studying 
void RequestUpdateTech(const DWORD dwTechID)
{
	stUpTechDegreeUserCmd cmd;
	cmd.dwOption =dwTechID;
	SEND_USER_CMD(cmd);
}


//////////////////////////////////////////////////////////////////
//         国家科技界面（包含进行中研究，投票，指定研究员)
//////////////////////////////////////////////////////////////////

#define IDC_COUNTRY_TECH_TAB 2 
#define IDC_COUNTRY_TAB_STUDYING  0     // 切页标示
#define IDC_COUNTRY_TAB_BALLOT    1
#define IDC_COUNTRY_TAB_RESEARCHER 2 

#define IDC_BTNID_STUDYING   3
#define IDC_BTNID_BALLOT     4
#define IDC_BTNID_RESEARCHER 5  

#define ID_BTN_CHECK_START		1000
#define ID_PROGRESS_START		2000
#define ID_BTN_UPDATE_START		3000
#define ID_COMBOX_RESERACHER_START	4000

#define ID_LISTBOX_STUDYING   101
#define ID_LISTBOX_BALLOT     201 
#define ID_LISTBOX_RESEARCHER 301

// ballot 
#define ID_BTN_BALLOT_OK            202
#define ID_BTN_BALLOT_CANCEL        203 

// researcher
#define ID_BTN_RESEARCHER_OK        302
#define ID_BTN_RESEARCHER_CANCEL    303

//close
#define ID_BTN_CLOSE    10


// group 
#define  ID_GROUP_TAB_TECH     1 // tab button 
#define  ID_GROUP_VOTE_BUTTON  2 // All vote button include in a group 

//////////////////////////////////////////////////////////////////////////
struct stCountryCmdItem
{
	enum enumCMD
	{
		CMD_KING_CANCEL_RESEARCHER,  //国王取消研究员
		CMD_NUM
	};
	enumCMD		cmd;
	std::string     strCmd;
	bool            bCanUse;
};

static stCountryCmdItem s_country_cmdItem[] = 
{
	{ stCountryCmdItem::CMD_KING_CANCEL_RESEARCHER,		"取消研究员",            true },
};
//////////////////////////////////////////////////////////////////////////
CGuiCountryTechDlg::CGuiCountryTechDlg(void)
: m_pTab(NULL)
, m_pListBoxStudying(NULL)
, m_pListBoxBallot(NULL)
, m_pListBoxResearcher(NULL)
, m_iLastFstBallotShowItem(0)
, m_iLastFstStudyingShowItem(0)
, m_iLastFstResearcherShowItem(0)
, m_pLastSelectBallot(NULL)
, m_menu(NULL)
{
	FUNCTION_BEGIN;

	m_vecUpgradeBtn.clear();
	m_vecProgressBtn.clear();
	m_vecCheckBtn.clear();
	m_vecCombox.clear();

	FUNCTION_END;
}

CGuiCountryTechDlg::~CGuiCountryTechDlg(void)
{
	FUNCTION_BEGIN;

	 Release();

	FUNCTION_END;
}

void CGuiCountryTechDlg::Release()
{
	FUNCTION_BEGIN;
	
	m_vecUpgradeBtn.clear();
	m_vecCheckBtn.clear();
	m_vecProgressBtn.clear();
	m_vecCombox.clear();
	
	FUNCTION_END;
}


void CGuiCountryTechDlg::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	
	m_pTab = GetTab(IDC_COUNTRY_TECH_TAB);
	m_pTab->SetVisible(false);

	
	//CCountryTech::Instance()->Debug();

	m_pListBoxStudying = GetListBox(ID_LISTBOX_STUDYING);
	m_pListBoxBallot  = GetListBox(ID_LISTBOX_BALLOT);
	m_pListBoxResearcher = GetListBox(ID_LISTBOX_RESEARCHER);

	GetButton(IDC_BTNID_STUDYING)->SetButtonGroup(ID_GROUP_TAB_TECH);
	GetButton(IDC_BTNID_BALLOT)->SetButtonGroup(ID_GROUP_TAB_TECH);
	GetButton(IDC_BTNID_RESEARCHER)->SetButtonGroup(ID_GROUP_TAB_TECH);

	Refresh_Studying_ListBox();
	Refresh_Ballot_ListBox();
	Refresh_Researcher_ListBox();
	
	m_bFirstTab = true;


	stPointI __pt = m_pListBoxStudying->GetLocation();
	m_rcListBox.left = __pt.x ;
	m_rcListBox.top = __pt.y ;
        m_rcListBox.right = m_rcListBox.left + m_pListBoxStudying->GetWidth();
	m_rcListBox.bottom = m_rcListBox.top + m_pListBoxStudying->GetHeight();
	

	//////////////////////////////////////////////////////////////////////////////
	m_menu = GetGuiManager()->CreateMenu( this );
	for ( int i = 0 ; i < count_of(s_country_cmdItem) ; i++ )
	{
		m_menu->AddMenuItem( i, s_country_cmdItem[i].strCmd.c_str(), CGuiMenu::MenuStyle_Command, s_country_cmdItem[i].bCanUse );
	}
	SetPopupMenu( m_menu );
	//////////////////////////////////////////////////////////////////////////////

	FUNCTION_END;
}

bool CGuiCountryTechDlg::OnContextMenu()
{
	FUNCTION_BEGIN;

	if( m_menu )
	{
		if( m_pTab->GetCurItem() != STUDYING)
			return false;

		if( !GetScene()->GetMainCharacter()->IsKing()) 
			return false;

		stPointI ptCursor = Engine_GetCursor()->GetPosition();
		stPointI _ptCursor;
		_ptCursor.x = ptCursor.x - m_x;
		_ptCursor.y = ptCursor.y - m_y;

		int iFirstShowItem = m_pListBoxStudying->GetFirstShowItem();
		int iLastShowItem  = m_pListBoxStudying->GetLastShowItem();
		if( m_rcListBox.PtInRect(_ptCursor))
		{
			int iii = 0; 
		}
		stRectI rcTemp = m_rcListBox;
		rcTemp.bottom = rcTemp.top + m_pListBoxStudying->GetTextHeight();
		for(int i = iFirstShowItem;i <= iLastShowItem;i++)
		{
			if( rcTemp.PtInRect(_ptCursor) )
			{
				m_pListBoxStudying->SetCurItem(i);
				m_menu->Popup();
				return true;
			}
			rcTemp.OffsetRect(0,m_pListBoxStudying->GetTextHeight());
		}		
	}
	return false;

	FUNCTION_END;
}


void CGuiCountryTechDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiCountryTechDlg=NULL;
	GetGameGuiManager()->OnNpcActionEnd();

	FUNCTION_END;
}


void CGuiCountryTechDlg::OnSetCloseLocation()
{
	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());
		m_pBtnClose->SetLocation(pt.x,pt.y+24);
	}
}

void CGuiCountryTechDlg::SetSelTab(const TAB_FLAG & nTab)
{
	FUNCTION_BEGIN;
	
	assert(nTab >= STUDYING && nTab <= RESEARCHER);
	if( nTab < STUDYING || nTab > RESEARCHER)  return;

	if( m_pTab->GetCurItem() == nTab && !m_bFirstTab)  return;
        m_bFirstTab = false;
	if(nTab == STUDYING) // 科技人员来升级
	{ 
		RequestActiveTech();  // 请求正在升级中的科技

		ShowBollotCtrl(false);
		ShowStudyingCtrl(false);
		ShowResearchCtrl(false);
		m_pTab->SetCurItem(STUDYING);
		OnGuiEvent(EVENT_TAB_SELECTION_CHANGED,IDC_BTNID_STUDYING,NULL);
		GetButton(IDC_BTNID_STUDYING)->SetChecked(true);
		GetButton(IDC_BTNID_BALLOT)->SetEnabled(false);
		GetButton(IDC_BTNID_STUDYING)->SetEnabled(true);
		GetButton(IDC_BTNID_RESEARCHER)->SetEnabled(false);
		
	}
	else if(nTab == BALLOT)   // 国民投票
	{
		RequestBallotFromServer();

		ShowBollotCtrl(false);
		ShowStudyingCtrl(false);
		ShowResearchCtrl(false);
		m_pTab->SetCurItem(BALLOT);
		OnGuiEvent(EVENT_TAB_SELECTION_CHANGED,IDC_BTNID_BALLOT,NULL);
		GetButton(IDC_BTNID_BALLOT)->SetChecked(true);
		GetButton(IDC_BTNID_BALLOT)->SetEnabled(true);
		GetButton(IDC_BTNID_STUDYING)->SetEnabled(false);
		GetButton(IDC_BTNID_RESEARCHER)->SetEnabled(false);

	}
	else if(nTab == RESEARCHER)  // 国王来指定研究员
	{     
	        RequestWaitingTech();
		RequestResearcherList();
	 
		ShowBollotCtrl(false);
		ShowStudyingCtrl(false);
		ShowResearchCtrl(false);
                m_pTab->SetCurItem(RESEARCHER);
		OnGuiEvent(EVENT_TAB_SELECTION_CHANGED,IDC_BTNID_RESEARCHER,NULL);
		GetButton(IDC_BTNID_RESEARCHER)->SetChecked(true);
		GetButton(IDC_BTNID_BALLOT)->SetEnabled(false);
		GetButton(IDC_BTNID_STUDYING)->SetEnabled(false);
		GetButton(IDC_BTNID_RESEARCHER)->SetEnabled(true);
	}

	FUNCTION_END;
}

bool CGuiCountryTechDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			// Ballot 
			if( nID >= ID_BTN_CHECK_START && nID <= ID_BTN_CHECK_START + 100)
			{  
                             int nIndex = nID - ID_BTN_CHECK_START;

			     Assert(nIndex >= 0 && nIndex < m_vecCheckBtn.size());
			     if( nIndex < 0 && nIndex >= m_vecCheckBtn.size())
				     break;

			     CGuiButton * pBtn = m_vecCheckBtn[nIndex];
			     bool bChecked = pBtn->GetChecked();
			     bChecked = !bChecked;
			     pBtn->SetChecked(bChecked);

			     // just change the temporary data
			      int nFirtShowItem = m_pListBoxBallot->GetFirstShowItem();
			     int nCurItem = nFirtShowItem + nIndex;
			     m_pListBoxBallot->SetCurItem(nCurItem);
			     stTechBallot * pBallot =  (stTechBallot *)m_pListBoxBallot->GetItemData(nCurItem);
			     pBallot->bChecked = bChecked;
                             
			     if( m_pLastSelectBallot != pBallot)
			     {
				     if( m_pLastSelectBallot)
						m_pLastSelectBallot->bChecked = false;
				     m_pLastSelectBallot = pBallot;
			     }
			}
			// progress
			else if( nID >= ID_BTN_UPDATE_START && nID <= ID_BTN_UPDATE_START + 100 )
			{
				int nIndex = nID - ID_BTN_UPDATE_START;

				Assert(nIndex >= 0 && nIndex < m_vecUpgradeBtn.size());
				if( nIndex < 0 && nIndex >= m_vecUpgradeBtn.size())
					break;

				Assert(nIndex >= 0 && nIndex < m_vecProgressBtn.size());
				if( nIndex < 0 && nIndex >= m_vecProgressBtn.size())
					break;

				CGuiButton * pBtn = m_vecUpgradeBtn[nIndex];
				CGuiProcess * pProgress = m_vecProgressBtn[nIndex];


				int nFirtShowItem = m_pListBoxStudying->GetFirstShowItem();
				int nCurItem = nFirtShowItem + nIndex;
				m_pListBoxStudying->SetCurItem(nCurItem);
				stStudying * pStudying =  (stStudying *)m_pListBoxStudying->GetItemData(nCurItem);

				if( pStudying && CCountryTech::Instance()->IsResearcherLegal(pStudying->dwTechOption))
					RequestUpdateTech(pStudying->dwTechOption);

				//pStudying->dwFinished +=5; 

				//pProgress->SetPos(pStudying->dwFinished); 
				
			}
			else if(nID == IDC_BTNID_STUDYING){
				SetSelTab(STUDYING);
			}
			else if(nID == IDC_BTNID_BALLOT){
				SetSelTab(BALLOT);
			}
			else if(nID == IDC_BTNID_RESEARCHER){
				SetSelTab(RESEARCHER);
			}
			else if( nID == ID_BTN_CLOSE)
			{
				//OnGuiEvent(EVENT_BUTTON_CLICKED,GUI_CLOSEBOX_ID,NULL);
				__Close__Release();
			}
 			else if(nID == ID_BTN_BALLOT_OK)   //  I am demotic and confirm bollots
			{ 
                                ConfirmBallot();   

				__Close__Release();
			}
			else if( nID == ID_BTN_BALLOT_CANCEL)  // I am demotic and I cancel bollots
			{
                               __Close__Release();
			}
			else if(nID == ID_BTN_RESEARCHER_OK) // I'm King, and now I confirm these researchers;
			{
				CCountryTech::eResult_SpecifyForWatingTech result = CCountryTech::Instance()->CanSpecifyForWaitingTech();
				if( result == CCountryTech::eResult_AllNotSpecified)
				{
					GetGameGuiManager()->AddMessageBox("你还没有指定任一科技的研究员！");
				}
				else if( result == CCountryTech::eResult_Succeeded )
				{
					ConfirmResearcher();
					__Close__Release();
				}
				else if( result == CCountryTech::eResult_HasSomeRepeat)
				{
					GetGameGuiManager()->AddMessageBox("不能指定一个研究员同时研究多项科技！");
				}
				else if( result == CCountryTech::eResult_HasSomeExistResearcher)
				{
					GetGameGuiManager()->AddMessageBox("不能指定正在研究中的研究员来研究新科技！");
				}
				
			}
			else if( nID == ID_BTN_RESEARCHER_CANCEL ) // I'am King, and now I don't want to change the researcher or assign some new ones.
			{
				__Close__Release();
			}
		}
		break;
	case EVENT_TAB_SELECTION_CHANGED:
		{
			RefreshCheckBtn();
			RefreshProgressBtn();
			RefreshUpdateBtn();
			RefreshResearcherCombox();
		}
		break;
	case EVENT_COMBOBOX_SELECTION_CHANGED:
		{	
			// researcher  
			if( nID >= ID_COMBOX_RESERACHER_START && nID <= ID_COMBOX_RESERACHER_START + 100)
			{  
				int nIndex = nID - ID_COMBOX_RESERACHER_START;

				Assert(nIndex >= 0 && nIndex < m_vecCombox.size());
				if( nIndex < 0 && nIndex >= m_vecCombox.size())
					break;

				CGuiComboBox * pCombox = m_vecCombox[nIndex];
			 

				// just change the temporary data
				int nFirtShowItem = m_pListBoxResearcher->GetFirstShowItem();
				int nCurItem = nFirtShowItem + nIndex;
				m_pListBoxResearcher->SetCurItem(nCurItem);
				stWaitingTech * pWating =  (stWaitingTech *)m_pListBoxResearcher->GetItemData(nCurItem);
				nCurItem = pCombox->GetCurItem();
				if( nCurItem >= 0 && nCurItem < pCombox->GetItemCount() )
					pWating->dwCharID = (DWORD)pCombox->GetItemData(nCurItem);
				 
			}
                       
		}
		break;
	//EVENT_LISTBOX_ITEM_CLICK
	case EVENT_LISTBOX_SELECTION:
		{
			if( m_pTab->GetCurItem() != BALLOT)
				break;
			int nFirtShowItem = m_pListBoxBallot->GetFirstShowItem();
			int nCurItem = m_pListBoxBallot->GetCurItem();
			int nIndex = nCurItem - nFirtShowItem;
			nIndex +=ID_BTN_CHECK_START;
			OnGuiEvent(EVENT_BUTTON_CLICKED,nIndex,NULL);

		}
		break;
	case EVENT_DIALOG_COMMAND:
		{
			switch( nID )
			{
			case stCountryCmdItem::CMD_KING_CANCEL_RESEARCHER:
				{
					assert(STUDYING == m_pTab->GetCurItem());
					if( m_pTab->GetCurItem() != STUDYING)
						break;

					int nCurItem = m_pListBoxStudying->GetCurItem();
					if( nCurItem == -1) 
						break;

					stStudying * pStudying =  (stStudying *)m_pListBoxStudying->GetItemData(nCurItem);

					if( pStudying)
					{
						stCancelTechSearchUserCmd  cmd;
						cmd.dwOption = pStudying->dwTechOption;
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

void CGuiCountryTechDlg::__Close__Release()
{
	Close();
	CCountryTech::Instance()->Release();
}

void CGuiCountryTechDlg::InitCustomElements()
{
	FUNCTION_BEGIN;

	//------------------------------------------------
	stResourceLocation rl;
	rl.SetFileName(GetGuiGraphicPackName());

	rl.group = 59;
	rl.frame = 30;
	IBitmap * pBitmap = GetDevice()->FindBitmap(&rl);
	m_UpdateBtnSize.cx = pBitmap->GetWidth();
	m_UpdateBtnSize.cy = pBitmap->GetHeight();

	rl.group = 59;
	rl.frame = 28;
	pBitmap = GetDevice()->FindBitmap(&rl);
	m_ProgressSize.cx = pBitmap->GetWidth();
	m_ProgressSize.cy = pBitmap->GetHeight();

	
	//------------------------------------------------

	
	stRectI rcTexture;
	stRectI rects[9];
	
 	//------------------------------------
	// Element for the 3D Border Process(进度条)
	//------------------------------------
 
	stGuiCtrlBorderInfo * pInfo = GetDefaultCtrlBorderInfo(GUI_CONTROL_PROCESS);

	pInfo->rcTexture.SetRect(0,0,m_ProgressSize.cx-1,m_ProgressSize.cy-1);
	pInfo->ptLeftTop = CGuiProcess::s_borderTopLeft;
	pInfo->ptRightBottom = CGuiProcess::s_borderRightBottom;
	pInfo->ptBorderLeftTop = stPointI(8,3);
	pInfo->ptBorderRightBottom = stPointI(4,2);
	pInfo->CuttRect9(rects);
	for(size_t i=0;i<count_of(rects);++i)
	{
		pInfo->element[i].SetTexture(&rl,&rects[i]);
		pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
		pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
	}

	CGuiElement Element;
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 200, 200, 200);
	Element.FontColor.Current = COLOR_ARGB(255,255,255,255);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_PROCESS, 0, &Element );

	FUNCTION_END;
}


HRESULT CGuiCountryTechDlg::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;
	 
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	if( m_pTab )
	{
		if( m_pTab->GetCurItem() == IDC_COUNTRY_TAB_BALLOT && m_pListBoxBallot->GetFirstShowItem() != m_iLastFstBallotShowItem)
		{
                      RefreshCheckBtn();    
		      m_iLastFstBallotShowItem = m_pListBoxBallot->GetFirstShowItem();
		}
		if( m_pTab->GetCurItem() == IDC_COUNTRY_TAB_STUDYING && m_pListBoxStudying->GetFirstShowItem() != m_iLastFstStudyingShowItem)
		{
                      RefreshProgressBtn();
		      RefreshUpdateBtn();
                      m_iLastFstStudyingShowItem = m_pListBoxStudying->GetFirstShowItem();
		}
		if( m_pTab->GetCurItem() == IDC_COUNTRY_TAB_RESEARCHER && m_pListBoxResearcher->GetFirstShowItem() != m_iLastFstResearcherShowItem)
		{
			RefreshResearcherCombox();
			m_iLastFstResearcherShowItem = m_pListBoxResearcher->GetFirstShowItem();
		}
	}

	// tooltips 
	RenderTechToolTip();

	return hr;

	FUNCTION_END;
}

void CGuiCountryTechDlg::RenderTechToolTip()
{
	FUNCTION_BEGIN;

	if( GetGuiManager()->GetMouseOverDlg() != this)
		return;

	if( m_pTab->GetCurItem() != IDC_COUNTRY_TAB_STUDYING )
		return;

	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	stPointI _ptCursor;
	_ptCursor.x = ptCursor.x - m_x;
	_ptCursor.y = ptCursor.y - m_y;

	int iFirstShowItem = m_pListBoxStudying->GetFirstShowItem();
	int iLastShowItem  = m_pListBoxStudying->GetLastShowItem();
	if( m_rcListBox.PtInRect(_ptCursor))
	{
		int iii = 0; 
	}
	stRectI rcTemp = m_rcListBox;
	rcTemp.bottom = rcTemp.top + m_pListBoxStudying->GetTextHeight();
	for(int i = iFirstShowItem;i <= iLastShowItem;i++)
	{
		if( rcTemp.PtInRect(_ptCursor) )
		{
			stStudying * pStudying = (stStudying *)m_pListBoxStudying->GetItemData(i);

			m_ToolTip.Clear();
			m_ToolTip.SetAlignType( GUI_ALIGN_LEFT );
			m_ToolTip.SetLineGaps( 3 );
			m_ToolTip.SetBorderColor(0);
			m_ToolTip.SetBkColor(COLOR_ARGB(128,0,0,0));
			m_ToolTip.SetCurColor(COLOR_ARGB(255,255,255,255));

			char szTemp[512];
			_snprintf(szTemp,sizeof(szTemp),"本次研究%d级\n需要国库资金　%d 两",pStudying->stTip.dwNextLevel,pStudying->stTip.dwNeedMoney);
			szTemp[sizeof(szTemp)-1] = 0;
			m_ToolTip.AddText(szTemp,0);

			_snprintf(szTemp,sizeof(szTemp)," 需要物资　%d ",pStudying->stTip.dwNeedContribute);
			szTemp[sizeof(szTemp)-1] = 0;
			m_ToolTip.AddText(szTemp,0);

			_snprintf(szTemp,sizeof(szTemp)," 需要原料　%d ",pStudying->stTip.dwNeedMaterial);
			szTemp[sizeof(szTemp)-1] = 0;
			m_ToolTip.AddText(szTemp,0);

			m_ToolTip.Resize();
			
			stPointI ptRender;
			ptRender.x = ptCursor.x - m_ToolTip.GetWidth();
			ptRender.y = ptCursor.y - m_ToolTip.GetHeight();

			m_ToolTip.Render(ptRender.x,ptRender.y);

			break;
		}
		rcTemp.OffsetRect(0,m_pListBoxStudying->GetTextHeight());

	}


	FUNCTION_END;
}


void CGuiCountryTechDlg::RefreshCurControl()
{
	FUNCTION_BEGIN;
	if( m_pTab && m_pTab->GetCurItem() == IDC_COUNTRY_TAB_BALLOT)
	{
		 Refresh_Ballot_ListBox();
		 RefreshCheckBtn();
	}
	else if( m_pTab && m_pTab->GetCurItem() == IDC_COUNTRY_TAB_STUDYING)
	{
		Refresh_Studying_ListBox();
		RefreshProgressBtn();
		RefreshUpdateBtn();
	}
	else if( m_pTab && m_pTab->GetCurItem() == IDC_COUNTRY_TAB_RESEARCHER)
	{
		Refresh_Researcher_ListBox();
		RefreshResearcherCombox();
	}
	FUNCTION_END;
}
void CGuiCountryTechDlg::Refresh_Studying_ListBox()
{
	FUNCTION_BEGIN;
	m_pListBoxStudying->RemoveAllItems();
	for( int i = 0 ; i < CCountryTech::Instance()->m_vecStudyingItem.size() ; i++)
	{
		stStudying * pTemp = CCountryTech::Instance()->m_vecStudyingItem[i];
		m_pListBoxStudying->AddItem("",pTemp);
		m_pListBoxStudying->SetItemText2(m_pListBoxStudying->GetItemCount()-1,0,pTemp->strTechName.c_str(),164);
		m_pListBoxStudying->SetItemText2(m_pListBoxStudying->GetItemCount()-1,1,pTemp->strResearcher.c_str(),164);
	}
	FUNCTION_END;
}
void CGuiCountryTechDlg::Refresh_Ballot_ListBox()
{
	FUNCTION_BEGIN;
	m_pListBoxBallot->RemoveAllItems();
	for( int i = 0 ; i < CCountryTech::Instance()->m_vecBallotItem.size() ; i++)
	{
		stTechBallot * pTemp1 = CCountryTech::Instance()->m_vecBallotItem[i];
		m_pListBoxBallot->AddItem("",pTemp1);
		m_pListBoxBallot->SetItemText2(m_pListBoxBallot->GetItemCount()-1,0,pTemp1->strTech.c_str(),250);

		char szBallot[20] = {0};
		sprintf(szBallot,"%d",pTemp1->dwBallot);
		m_pListBoxBallot->SetItemText2(m_pListBoxBallot->GetItemCount()-1,1,szBallot,206);
	}
	FUNCTION_END;

}
void CGuiCountryTechDlg::Refresh_Researcher_ListBox()
{
	FUNCTION_BEGIN;

	m_pListBoxResearcher->RemoveAllItems();
	for( int i = 0 ; i < CCountryTech::Instance()->m_vecWaitingTech.size() ; i++)
	{
		stWaitingTech * pTemp2 = CCountryTech::Instance()->m_vecWaitingTech[i];
		m_pListBoxResearcher->AddItem("",pTemp2);
		m_pListBoxResearcher->SetItemText2(m_pListBoxResearcher->GetItemCount()-1,0,pTemp2->strTechName.c_str(),312);
	}
        
	FUNCTION_END;
}


bool CGuiCountryTechDlg::MsgProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	FUNCTION_BEGIN;
	
	switch(uMsg)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_RETURN:
			//if( m_pListBoxBallot )
			//{
			//	m_pListBoxBallot->FinishEdit();
			//}
			break;
		}
		break;
	}
	return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);

	FUNCTION_END;
}


void CGuiCountryTechDlg::ShowBollotCtrl(bool bShow)
{
	FUNCTION_BEGIN;

	for(int i = 0 ; i < m_vecCheckBtn.size() ; i++)
	{
		CGuiButton * pBtn = m_vecCheckBtn[i];
		pBtn->SetVisible(bShow);
		pBtn->SetEnabled(bShow);
	}
	
	FUNCTION_END;
}

void CGuiCountryTechDlg::ShowStudyingCtrl(bool bShow)
{
	FUNCTION_BEGIN;

	 
	for(int i = 0 ; i < m_vecProgressBtn.size() ; i++)
	{
		CGuiProcess * pProcess = m_vecProgressBtn[i];
		pProcess->SetVisible(bShow);
		pProcess->SetEnabled(bShow);
	}

	for(i = 0 ; i < m_vecUpgradeBtn.size();i++)
	{
		CGuiButton * pBtn = m_vecUpgradeBtn[i];
		pBtn->SetVisible(bShow);
		pBtn->SetEnabled(bShow);
	}


	FUNCTION_END;
}

void CGuiCountryTechDlg::ShowResearchCtrl(bool bShow)
{
	FUNCTION_BEGIN;

	for(int i = 0 ; i < m_vecCombox.size() ; i++)
	{
		CGuiComboBox * pCombox = m_vecCombox[i];
		pCombox->SetVisible(bShow);
		pCombox->SetEnabled(bShow);
	}
	
	FUNCTION_END;
}

//---------------------------------------------------------------------------
// 刷新投票的CheckButton
//---------------------------------------------------------------------------
void CGuiCountryTechDlg::RefreshCheckBtn()
{
	FUNCTION_BEGIN;

	if( m_pTab->GetCurItem() == IDC_COUNTRY_TAB_BALLOT)
	{
	    if( m_pListBoxBallot->GetItemCount() <= 0)
			return;

            int nStarted = m_pListBoxBallot->GetFirstShowItem();
	    int nEnd = m_pListBoxBallot->GetLastShowItem();


	    int nShowItem = nEnd - nStarted + 1;
	    if( m_vecCheckBtn.size() < nShowItem )
	    {  // Need increase some new buttons 
		    size_t nDiff = nShowItem - m_vecCheckBtn.size();
		    DWORD nStartID = 0;
		    if(m_vecCheckBtn.size() == 0 )
			    nStartID=  ID_BTN_CHECK_START;
		    else
		    {
			    CGuiButton * pLastBtn = m_vecCheckBtn.back();
			    nStartID = pLastBtn->GetID();
		    }
		    DWORD dwState[] = {GUI_STATE_NORMAL,GUI_STATE_MOUSEOVER,GUI_STATE_PRESSED};
		    for(size_t i = 0 ; i< nDiff;i++)
		    {    			    
			    stPointI pt(0,0);
			    CGuiButton* pBtn;
			    AddButton(nStartID + i, "" , pt.x , pt.y , 32 , 32 , 0 , false , &pBtn );
			    pBtn->SetButtonType( CGuiButton::ButtonType_Bitmap );
			    pBtn->SetBitmap( stResourceLocation(GetGuiGraphicPackName(),59,50) , 3 , dwState);
                            m_vecCheckBtn.push_back(pBtn);

			    pBtn->SetButtonGroup(ID_GROUP_VOTE_BUTTON);
		    }
	    }
	    else if( m_vecCheckBtn.size() > nShowItem)
	    {  // Some  Buttons need not show which was layed after  
                    size_t _diff = m_vecCheckBtn.size() - nShowItem;
		    for(size_t i = 0 ; i < _diff ; i++)
		    {
			    m_vecCheckBtn[ nShowItem + i]->SetVisible(false);
			    m_vecCheckBtn[ nShowItem + i]->SetEnabled(false);
		    }
	    }

	    // Now let's setvisible() and setenable the suitable checkbutton as the listbox's items
            for(size_t i = 0 ; i< nShowItem ; i++)
	    {
		    stTechBallot *pBallot = (stTechBallot *)m_pListBoxBallot->GetItemData(nStarted+i);
                    m_vecCheckBtn[i]->SetChecked(pBallot->bChecked);
		    m_vecCheckBtn[i]->SetLocation(530,61 + i * GetDevice()->GetFontHeight(1));
		    m_vecCheckBtn[i]->SetVisible(true);
		    m_vecCheckBtn[i]->SetEnabled(true);
	    }
	}
 
	FUNCTION_END;
}

//---------------------------------------------------------------------------
// 刷新研究员切页的完成度
//---------------------------------------------------------------------------
void CGuiCountryTechDlg::RefreshProgressBtn()
{
	FUNCTION_BEGIN;

	if( m_pTab->GetCurItem() == IDC_COUNTRY_TAB_STUDYING)
	{
		if( m_pListBoxStudying->GetItemCount() <= 0)
			return;

		int nStarted = m_pListBoxStudying->GetFirstShowItem();
		int nEnd = m_pListBoxStudying->GetLastShowItem();

		int nShowItem = nEnd - nStarted + 1;
		if( m_vecProgressBtn.size() < nShowItem )
		{  // Need increase some new buttons 
			size_t nDiff = nShowItem - m_vecProgressBtn.size();
			DWORD nStartID = 0;
			if(m_vecProgressBtn.size() == 0 )
				nStartID=  ID_PROGRESS_START;
			else
			{
				CGuiProcess * pLastBtn = m_vecProgressBtn.back();
				nStartID = pLastBtn->GetID();
			}
			for(size_t i = 0 ; i< nDiff;i++)
			{    			    
				stPointI pt(0,0);
				CGuiProcess* pProgress;
				AddProcess(nStartID + i,pt.x , pt.y , m_ProgressSize.cx , m_ProgressSize.cy , 0 ,100,0 ,false , &pProgress );
				pProgress->SetProcessBitmap(stResourceLocation(GetGuiGraphicPackName(),59,29));
				m_vecProgressBtn.push_back(pProgress);
			}
		}
		else if( m_vecProgressBtn.size() > nShowItem)
		{  // Some  Buttons need not show which was layed after  
			size_t _diff = m_vecProgressBtn.size() - nShowItem;
			for(size_t i = 0 ; i < _diff ; i++)
			{
				m_vecProgressBtn[ nShowItem + i]->SetVisible(false);
				m_vecProgressBtn[ nShowItem + i]->SetEnabled(false);
			}
		}

		// Now let's setvisible() and setenable the suitable checkbutton as the listbox's items
		for(size_t i = 0 ; i< nShowItem ; i++)
		{
			stStudying *pStudying = (stStudying *)m_pListBoxStudying->GetItemData(nStarted+i);
			m_vecProgressBtn[i]->SetPos(pStudying->dwFinished);
			char szProgress[50] = {0};
			_snprintf(szProgress,sizeof(szProgress),"%d",pStudying->dwFinished);
			strcat(szProgress,"%");
			szProgress[sizeof(szProgress)-1] = 0;
			m_vecProgressBtn[i]->SetText(szProgress);

			m_vecProgressBtn[i]->SetLocation(374,60 + 5+ i * GetDevice()->GetFontHeight(1));
			m_vecProgressBtn[i]->SetVisible(true);
			m_vecProgressBtn[i]->SetEnabled(true);
		}
	}

	FUNCTION_END;
}

//---------------------------------------------------------------------------
// 刷新研究员切页的升级按钮个数
//---------------------------------------------------------------------------
void CGuiCountryTechDlg::RefreshUpdateBtn()
{
	FUNCTION_BEGIN;

	if( m_pTab->GetCurItem() == IDC_COUNTRY_TAB_STUDYING)
	{
		if( m_pListBoxStudying->GetItemCount() <= 0 ) 
			return ;

		int nStarted = m_pListBoxStudying->GetFirstShowItem();
		int nEnd = m_pListBoxStudying->GetLastShowItem();

		int nShowItem = nEnd - nStarted + 1;
		if( m_vecUpgradeBtn.size() < nShowItem )
		{  // Need increase some new buttons 
			size_t nDiff = nShowItem - m_vecUpgradeBtn.size();
			DWORD nStartID = 0;
			if(m_vecUpgradeBtn.size() == 0 )
				nStartID=  ID_BTN_UPDATE_START;
			else
			{
				CGuiButton * pLastBtn = m_vecUpgradeBtn.back();
				nStartID = pLastBtn->GetID();
			}
			DWORD dwState[] = {GUI_STATE_NORMAL,GUI_STATE_MOUSEOVER,GUI_STATE_PRESSED};
			for(size_t i = 0 ; i< nDiff;i++)
			{    			    
				stPointI pt(0,0);
				CGuiButton* pBtn;
				AddButton(nStartID + i, "" , pt.x , pt.y , m_UpdateBtnSize.cx , m_UpdateBtnSize.cy , 0 , false , &pBtn );
				pBtn->SetButtonType( CGuiButton::ButtonType_Bitmap );
				pBtn->SetBitmap( stResourceLocation(GetGuiGraphicPackName(),59,30) , 3 , dwState);
				m_vecUpgradeBtn.push_back(pBtn);
			}
		}
		else if( m_vecUpgradeBtn.size() > nShowItem)
		{  // Some  Buttons need not show which was layed after  
			size_t _diff = m_vecUpgradeBtn.size() - nShowItem;
			for(size_t i = 0 ; i < _diff ; i++)
			{
				m_vecUpgradeBtn[ nShowItem + i]->SetVisible(false);
				m_vecUpgradeBtn[ nShowItem + i]->SetEnabled(false);
			}
		}

		// Now let's setvisible() and setenable the suitable checkbutton as the listbox's items
		for(size_t i = 0 ; i< nShowItem ; i++)
		{
			stStudying *pStudying = (stStudying *)m_pListBoxStudying->GetItemData(nStarted+i);
			m_vecUpgradeBtn[i]->SetLocation( 557,60+ 2+ i * GetDevice()->GetFontHeight(1));
			m_vecUpgradeBtn[i]->SetVisible(true);
			m_vecUpgradeBtn[i]->SetEnabled(true);

			if( pStudying && CCountryTech::Instance()->IsResearcherLegal(pStudying->dwTechOption))
				m_vecUpgradeBtn[i]->SetEnabled(true);
			else 
				m_vecUpgradeBtn[i]->SetEnabled(false);

		}
	}

	FUNCTION_END;
}


//---------------------------------------------------------------------------
// 刷新进行中研究切页的升级按钮个数
//---------------------------------------------------------------------------
void CGuiCountryTechDlg::RefreshResearcherCombox()
{
	FUNCTION_BEGIN;

	if( m_pTab->GetCurItem() == IDC_COUNTRY_TAB_RESEARCHER)
	{
		if( m_pListBoxResearcher->GetItemCount() <= 0) 
			return;

		int nStarted = m_pListBoxResearcher->GetFirstShowItem();
		int nEnd = m_pListBoxResearcher->GetLastShowItem();

		int nShowItem = nEnd - nStarted + 1;
		if( m_vecCombox.size() < nShowItem )
		{  // Need increase some new combox 
			size_t nDiff = nShowItem - m_vecCombox.size();
			DWORD nStartID = 0;
			if(m_vecCombox.size() == 0 )
				nStartID=  ID_COMBOX_RESERACHER_START;
			else
			{
				CGuiComboBox * pLastCombox = m_vecCombox.back();
				nStartID = pLastCombox->GetID();
			}
			for(size_t i = 0 ; i< nDiff;i++)
			{    			    
				stPointI pt(0,0);
				CGuiComboBox* pCombox;
				AddComboBox(nStartID + i, pt.x , pt.y , m_ComboxSize.cx , m_ComboxSize.cy , 0 , false , &pCombox );

				

				m_vecCombox.push_back(pCombox);
			}
		}
		else if( m_vecCombox.size() > nShowItem)
		{  // Some  Buttons need not show which was layed after  
			size_t _diff = m_vecCombox.size() - nShowItem;
			for(size_t i = 0 ; i < _diff ; i++)
			{
				m_vecCombox[ nShowItem + i]->SetVisible(false);
				m_vecCombox[ nShowItem + i]->SetEnabled(false);
			}
		}

		// Now let's setvisible() and setenable the suitable checkbutton as the listbox's items
		for(size_t i = 0 ; i< nShowItem ; i++)
		{
			stWaitingTech *pWaitingTech = (stWaitingTech *)m_pListBoxResearcher->GetItemData(nStarted+i);
			
			
			if( pWaitingTech)
			{
				m_vecCombox[i]->RemoveAllItems();
				m_vecCombox[i]->AddItem("",NULL);
				int nSelItem = 0;
				for(size_t iSub = 0; iSub < CCountryTech::Instance()->m_vecReasearchItem.size() ;  ++iSub)
				{
					const _stWaitOfficialItem * _st = CCountryTech::Instance()->m_vecReasearchItem[iSub];
					m_vecCombox[i]->AddItem(_st->strResearcher.c_str(),(void *)_st->dwCharID);
					if( _st->dwCharID == pWaitingTech->dwCharID)
						nSelItem = m_vecCombox[i]->GetItemCount()-1;
				}
				m_vecCombox[i]->SetCurItem(nSelItem);
				 
			}

			m_vecCombox[i]->SetLocation( 380,60+ 3 + i * GetDevice()->GetFontHeight(1));
			m_vecCombox[i]->SetSize(200,GetDevice()->GetFontHeight(1)+4);
			m_vecCombox[i]->SetVisible(true);
			m_vecCombox[i]->SetEnabled(true);
		}
	}

	FUNCTION_END;
}


void CGuiCountryTechDlg::ConfirmBallot()
{
        FUNCTION_BEGIN;
       

	for( size_t i = 0 ; i <  CCountryTech::Instance()->m_vecBallotItem.size(); i++ )
	{  
		 
		stTechBallot * pBallot = CCountryTech::Instance()->m_vecBallotItem[i];
	 
		if( pBallot && pBallot->bChecked)
		{
			ConfirmBallotToServer(pBallot->dwOption);
			return;			
		}

	}

	FUNCTION_END;
}

void CGuiCountryTechDlg::ConfirmResearcher()
{
	FUNCTION_BEGIN;

	for( size_t i = 0 ; i <  CCountryTech::Instance()->m_vecWaitingTech.size(); i++ )
	{  

		stWaitingTech * pWating = CCountryTech::Instance()->m_vecWaitingTech[i];

		if( pWating && pWating->dwCharID != 0)
		{
			RequestResearcherFromServer(pWating->dwTechOption,pWating->dwCharID);		
		}

	}
 
	FUNCTION_END;
}

//-----------------------------------------------------------------------
//     国家数据
//-----------------------------------------------------------------------
static CCountryConn g_CountryConn;
CCountryConn* CCountryConn::Singleton = 0;

CCountryConn::CCountryConn()
: m_guiAntiDareCountryDlg(NULL)
, m_guiGenInfoDlg(NULL)
, m_guiOccupierDlg(NULL)
{
	FUNCTION_BEGIN;

	Assert(0 == Singleton);
	Singleton = this;
	Release();

	FUNCTION_END;
}
CCountryConn::~CCountryConn()
{
	FUNCTION_BEGIN;

	Singleton = 0;
	Release();

	FUNCTION_END;
}


void CCountryConn::RequestForGenInfo()
{
		stReqGenCountryCmd cmd;
		SEND_USER_CMD(cmd);
}

void CCountryConn::EchoForGenInfo(const stRetGenCountryCmd  * pCmd)
{
        SAFE_CHECK_COUNTRY_MSG(pCmd)
	m_GenInfo.exp = pCmd->exp;
	m_GenInfo.maxExp = pCmd->maxExp;
	m_GenInfo.level = pCmd->level;
	if(m_guiGenInfoDlg)
		m_guiGenInfoDlg->UpdataUI();
}

void CCountryConn::ResetQuest() //soke 应该是国王发布任务
{
	m_bQuestForKingAffiched = false;
}

void CCountryConn::Release()
{
	FUNCTION_BEGIN;

	m_vecFinishedTech.clear();
#ifdef _UN_USED
	m_vecMember.clear();
#endif
	FreePointerList<tVecArmyBaseInfo>(m_vecCountryArmyBaseInfo);
	m_HashMapArmyItem.clear();
	
	FUNCTION_END
}

void CCountryConn::AddKingAfficle(const stCountryNoteCmd * pcmd)
{
	FUNCTION_BEGIN;
	if(pcmd)
	{
		size_t len = strlen(pcmd->note);
		if(len >=255)
			len = 255;
		char * pTemp = (char *)_alloca(len+1);
		_snprintf(pTemp,len,"%s",pcmd->note);
		*(pTemp+len) = 0;
		m_strKingAfficle = pTemp;
		if( GetGameGuiManager()->m_guiCountry && GetGameGuiManager()->m_guiCountry->IsVisible()){
			GetGameGuiManager()->m_guiCountry->RefreshAffiche();
		}
	}
	FUNCTION_END;
}

bool CCountryConn::GetKingAfficle(std::string & strOut)
{
	 if(m_bQuestForKingAffiched)
	 {
		 strOut = m_strKingAfficle;
		 return true;
	 }
	 else 
	 {
		 return false;
	 }
}
void CCountryConn::RequestForKingAffiche()
{
	 
	stReqCountryNoteCmd  cmd;
	SEND_USER_CMD(cmd);
	m_bQuestForKingAffiched = true;
	 
}

void CCountryConn::Debug(const Occupier_Usage & usage)
{
#if 0 
	m_ListOccupierTong.clear();
	m_ListOccupierSept.clear();
	m_ListKingSort.clear();

	for( int i = 0 ; i< 30; i++)
	{

		stOccupierTongItem item;
		char szBuffer[10];
		itoa(i,szBuffer,10);
		std::string strTest = szBuffer;
			
		item.strCityName = strTest + "王城";
		item.strTongName = strTest + "天下会";
		m_ListOccupierTong.push_back(item);	
			 
		stOccupierSeptItem _item;
		_item.strNpcPos  = strTest + "王城学校(123,444)";
		_item.strSeptName = strTest + "白痴家族";
		m_ListOccupierSept.push_back(_item);

		stKingSortList __item;
		__item.strCountryName = strTest + "宋国";
		__item.strName = strTest + "国王";
		__item.strTotalTime = strTest +  "时间";
		m_ListKingSort.push_back(__item);
        }

	CCountryConn::Instance()->AddOccupierDlg(usage);
#endif
}


void CCountryConn::SetKingSortList(const stRtnKingListCmd * pCmd)
{
     if(!pCmd)  return;

     m_ListKingSort.clear();

     for(size_t i = 0 ; i < pCmd->size ; ++i)
     {
          const _KingListItem & item =  pCmd->data[i];
	  stKingSortList __item;
	  char szCountryName[MAX_NAMESIZE+1];
	  if(CCountry::Instance()->GetCountryName(item.country_id,szCountryName))
	  {
		  szCountryName[MAX_NAMESIZE] = 0;
		  __item.strCountryName = szCountryName;
	  }
	  else 
	  {
		__item.strCountryName = "";	
	  }
	  __item.strName = item.king_name;

	  char szOnLine[21];
	  _snprintf(szOnLine,20,"%d",item.online_time);
	  szOnLine[20] = 0;

	  __item.strTotalTime = szOnLine;
	  m_ListKingSort.push_back(__item);
     }
  
     AddOccupierDlg(Usage_KingSort);
}

void CCountryConn::SetCityOwnerList(const stRtnCityOwnerListCmd * pCmd)
{
      if(!pCmd) return;

      m_ListOccupierTong.clear();

        
      for(size_t i = 0 ; i < pCmd->size; ++i)
      {
	      stOccupierTongItem __item;

	      char szName[MAX_NAMESIZE+1];
	      strncpy(szName,pCmd->list[i].cityName,MAX_NAMESIZE);
	      szName[MAX_NAMESIZE] = 0;
	      __item.strCityName = szName;

	      strncpy(szName,pCmd->list[i].unionName,MAX_NAMESIZE);
	      szName[MAX_NAMESIZE] = 0;
	      __item.strTongName = szName;

	      m_ListOccupierTong.push_back(__item);

      }

       AddOccupierDlg(Usage_OccupierTong);
}

 
void CCountryConn::SetNpcOwnerList(const stRtnNpcOwnerListCmd * pCmd)
{
	if(!pCmd) return;
 
	for(size_t i = 0 ; i < pCmd->size; ++i)
	{

		stOccupierSeptItem _item;

		char szName[MAX_NAMESIZE+1];
		strncpy(szName,pCmd->list[i].mapName,MAX_NAMESIZE);
		szName[MAX_NAMESIZE] = 0;

		char szFinal[MAX_NAMESIZE+21];
		_snprintf(szFinal,MAX_NAMESIZE+20,"%s(%d,%d)",szName,pCmd->list[i].x,pCmd->list[i].y);
		szFinal[MAX_NAMESIZE+20] = 0;

		_item.strNpcPos  = szFinal;
		
		strncpy(szName,pCmd->list[i].septName,MAX_NAMESIZE);
		szName[MAX_NAMESIZE] = 0;
		_item.strSeptName = szName;

		m_ListOccupierSept.push_back(_item);
	}


	AddOccupierDlg(Usage_OccupierSept);
}

//-----------------------------------------------------------------------------------
//  Request the technique list which has been finished from server which will be 
//  showed in the country gui. 
//-----------------------------------------------------------------------------------
void RequestFinishedTechList()
{
	stReqTechUserCmd cmd;
	cmd.dwType = FINISH_TECH_TYPE;  
	SEND_USER_CMD(cmd);

}


void RequestCountryArmyList()
{
	stReqArmyListUserCmd cmd;
	cmd.byType = COUNTRY_ARMY_LIST;
	SEND_USER_CMD(cmd);
}


void CCountryConn::SetDeaultTechList()
{
	SetFrameAllocator fa;

	
	

	size_t len = sizeof(stRtnTechUserCmd) + 14 * sizeof(stTechItem);

	stRtnTechUserCmd *cmd = (stRtnTechUserCmd*)FrameAllocator::alloc(len);

	constructInPlace(cmd);

	cmd->dwType = FINISH_TECH_TYPE;
	cmd->dwSize= 14;
       
	const char techName[][14] = 
	{
		{"布料类服装"},
		{"皮甲类服装"},
		{"铠甲类服装"},
		{"刀盾类"},
		{"弓类"},
		{"棍类"},
		{"杖类"},
		{"扇类"},
		{"项链类"},
		{"戒指类"},
		{"护腕类"},
		{"腰带类"},
		{"鞋子类"},
		{"头盔类"},
	};

	for(size_t i = 1 ; i <= 14; ++i)
	{
                  cmd->data[i-1].dwOption =i;  // tech id
		  cmd->data[i-1].dwLevel = 1; // level 1
		  strncpy(cmd->data[i-1].szOptionDesc,techName[i-1],sizeof(cmd->data[i-1].szOptionDesc));
	}

	SetFinishedTechList(cmd);

	
}

//--------------------------------------------------------------------------------
// 从服务器copy过来的，所以要跟服务器保持一致,注意不要用全局map,浪费内存
/*
科技等级	对应装备等级	
1		80	
2		90	
3		100	
4		110	
5		120	
6		130	
7		140	
8		150	



衣服：1、布料类服装，2、皮甲类服装，3、铠甲类服装   
	[服装分男女，但是类型相同。]
	
武器：4、刀盾类，5、弓类，6、棍类，7、杖类，8、扇类
	[其中刀盾类包括：刀、剑、锤、斧、盾牌]

首饰：9、项链类，10、戒指类，11、护腕类

防护：12、腰带类，13、鞋子类，14、头盔类


*/
//--------------------------------------------------------------------------------
void CCountryConn::SetFinishedTechList(const stRtnTechUserCmd* pCmd)
{
	
	assert(pCmd && pCmd->dwType == FINISH_TECH_TYPE);

	if( !pCmd || pCmd->dwType != FINISH_TECH_TYPE) return;
	
	//m_vecFinishedTech.clear();

	struct MAP_TECHLEVEL_EQUIPLEVEL
	{
		DWORD dwTechLevel;    // 科技等级
		DWORD dwEquipLevel;   // 对应装备等级
	};

	const MAP_TECHLEVEL_EQUIPLEVEL c_MapEquipLevel[] =
	{
		{1,80},
		{2,90},	
		{3,100},	
		{4,110},	
		{5,120},	
		{6,130},	
		{7,140},	
		{8,150},		
	};
	struct MAP_TECHID_EQUIPNAME
	{
		DWORD dwTechOption;
		std::string strEquipName;
	};
	const MAP_TECHID_EQUIPNAME c_MapEquipName[] = 
	{
		{1,"布料类服装"},
		{2,"皮甲类服装"},
		{3,"铠甲类服装"},
		{4,"刀盾类"},
		{5,"弓类"},
		{6,"棍类"},
		{7,"杖类"},
		{8,"扇类"},
		{9,"项链类"},
		{10,"戒指类"},
		{11,"护腕类"},
		{12,"腰带类"},
		{13,"鞋子类"},
		{14,"头盔类"},
	};


	for (int i=0 ; i < pCmd->dwSize ; ++i )
	{
	
		CCountry_stTech st;

		// tech id(option)
		st.dwOption = pCmd->data[i].dwOption;          

		// tech name 
                st.strTech  = pCmd->data[i].szOptionDesc;

		// tech level 
                st.dwTechLevel = pCmd->data[i].dwLevel;


		// equipName 
		for( size_t __i = 0 ; __i < count_of(c_MapEquipName); __i++ )
		{
			if( c_MapEquipName[__i].dwTechOption == st.dwOption)
			{
				st.strEquip = c_MapEquipName[__i].strEquipName;
				break;
			}
		}
		assert(__i != count_of(c_MapEquipName));

		// equiplevel
		for( size_t _i = 0 ; _i < count_of(c_MapEquipLevel); _i++)
		{
			if( c_MapEquipLevel[_i].dwTechLevel == st.dwTechLevel)
			{
                                st.dwEquipLevel = c_MapEquipLevel[_i].dwEquipLevel;
				break;
			}
		}
		assert(_i != count_of(c_MapEquipLevel));
		if( _i == count_of(c_MapEquipLevel))   st.dwEquipLevel = 0; 


		for(CCountryConn_Iterator iter = m_vecFinishedTech.begin() ; iter != m_vecFinishedTech.end() ; )
		{
			const CCountry_stTech & temp = *iter;
			if( temp.dwOption == st.dwOption)
			{
				m_vecFinishedTech.erase(iter);
			}
			else 
				++iter;
		}
		m_vecFinishedTech.push_back(st);
	}

///////////////////////////////////////////////////////////
//soke 取消国家科技
//	if( GetGameGuiManager()->m_guiCountry && GetGameGuiManager()->m_guiCountry->IsVisible())
//	{
//		GetGameGuiManager()->m_guiCountry->Refresh_ListBox_Tech();
//	}
///////////////////////////////////////////////////////////

}

void CCountryConn::SetCountryArmyList(const stRtnArmyListUserCmd* pCmd)
{
	if( pCmd->byType == COUNTRY_ARMY_LIST)// 用于国家对话框，显示全国军队列表
	{
		 
		FreePointerList<tVecArmyBaseInfo>(m_vecCountryArmyBaseInfo);

		for(size_t i = 0; i < pCmd->dwSize; ++i)
		{
			stArmyBaseInfo * st = new stArmyBaseInfo;
			if( !st) continue;

			st->dwArmyID = pCmd->data[i].dwArmyID;
			strcpy(st->cityname,pCmd->data[i].cityname);
			strcpy(st->genname,pCmd->data[i].genname);
			strcpy(st->name,pCmd->data[i].name);
			st->dwCapNum = pCmd->data[i].dwCapNum;

			m_vecCountryArmyBaseInfo.push_back(st);
		}

	}

	if( GetGameGuiManager()->m_guiCountry)
		GetGameGuiManager()->m_guiCountry->Refresh_ListBox_Army();
}

void CCountryConn::SetCountryArmyItem(const stRtnArmySpecUserCmd * pCmd)
{
	for(c_ArmyBaseInfoIterator iter = m_vecCountryArmyBaseInfo.begin();iter!=m_vecCountryArmyBaseInfo.end();++iter)
	{
                   const stArmyBaseInfo * pInfo = *iter;
		   if(pInfo && strcmp(pInfo->name,pCmd->name) == 0)
		   {
			     stRtnArmyItem & Item = m_HashMapArmyItem[pInfo->name];
			     Item.strCity = pCmd->cityname;
			     Item.strGen = pCmd->genname;
			     Item.strName = pCmd->name;
			     Item.vecBase.clear();
			     for(size_t i = 0 ; i < pCmd->dwSize; ++i)
			     {
				     _stCaptionBase base;
				     base.dwCharID  = pCmd->data[i].dwCharID;
				     base.strCaptionName  = pCmd->data[i].name;
				     Item.vecBase.push_back(base);
			     }
                             break;
		   }
	}
	if( GetGameGuiManager()->m_guiCountry)
		GetGameGuiManager()->m_guiCountry->Refresh_ListBox_ArmyItem();
}

/********************************************************************************
//********************************************************************************
//				国家界面
//********************************************************************************
********************************************************************************/

//-----------------------------------------------------------------------------------
#define   ID_COUNTRY_BUTTON_CLOSE     2 

#define   ID_COUNTRY_LISTBOX_TECH     10   

#ifdef _UN_USED
	#define   ID_COUNTRY_LISTBOX_MEMBER   11
#else
	#define   ID_COUNTRY_MLEDITBOX_KING_SEND_AFFICLE 11
#endif

#define   ID_COUNTRY_LISTBOX_ARMY     12 //soke 军队
#define   ID_COUNTRY_LISTBOX_ARMYITEM     13 //soke 军队成员

#define   ID_COUNTRY_STATIC_COUNTRY   15 //soke 国家名
#define   ID_COUNTRY_STATIC_KING      16 //soke 国王
#define   ID_COUNTRY_STATIC_HONOR     17 //soke 荣誉
#define   ID_COUNTRY_STATIC_TAX       18 //soke 今日税率

#define   ID_COUNTRY_BUTTON_FORUM     20
#define   ID_COUNTRY_BUTTON_FINANCE   21 
#define   ID_COUNTRY_BUTTON_MILITARY  22
#define   ID_COUNTRY_BUTTON_DIPLOMACY 23

#define ID_COUNTRY_BUTTON_KING_SEND_AFFICLE 24 //soke 国家公告
#define   ID_COUNTRY_STATIC_XKING      25 //soke 2016 - 3 - 11 国家官员表 - 国王
#define   ID_COUNTRY_STATIC_YUANSHUAI  26 //soke 2016 - 3 - 11 国家官员表 - 元帅
#define   ID_COUNTRY_STATIC_ZAIXIANG   27 //soke 2016 - 3 - 11 国家官员表 - 宰相
#define   ID_COUNTRY_STATIC_CATCHER    28 //soke 2016 - 3 - 11 国家官员表 - 捕头（左上）
#define   ID_COUNTRY_STATIC_CENSOR     29 //soke 2016 - 3 - 11 国家官员表 - 御史大夫（右上）
#define   ID_COUNTRY_STATIC_CATCHEX    30 //soke 2016 - 3 - 11 国家官员表 - 捕头（左下）
#define   ID_COUNTRY_STATIC_CENSOS     31 //soke 2016 - 3 - 11 国家官员表 - 御史大夫（右下）


//-----------------------------------------------------------------------------------

CGuiCountry::CGuiCountry()
: m_pListBoxTech(NULL)
#ifdef _UN_USED
, m_pListBoxMember(NULL)
#else
, m_pMLEditBoxAffiche(NULL)
#endif
, m_pListBoxArmy(NULL)
, m_dwBtnIDClicked(ID_COUNTRY_BUTTON_COUNTRY)
, m_pStaticCountry(NULL)
, m_pStaticKing(NULL)    //soke 国家官员 - 国王
, m_pStaticHonor(NULL)
, m_pStaticTax(NULL)
, m_pStaticCatcher(NULL)   //soke 2016 - 3 - 11 国家官员 - 捕头（左上）
, m_pStaticCatchex(NULL)   //soke 2016 - 3 - 11 国家官员 - 捕头（左下）
, m_pStaticCensor(NULL)    //soke 2016 - 3 - 11 国家官员 - 御史大夫（右上）
, m_pStaticCensos(NULL)    //soke 2016 - 3 - 11 国家官员 - 御史大夫（右下）
, m_pStaticYuanshuai(NULL) //soke 2016 - 3 - 11 国家官员 - 元帅
, m_pStaticZaixiang(NULL)  //soke 2016 - 3 - 11 国家官员 - 宰相
{
	FUNCTION_BEGIN;
	m_bCloseIsHide = true;
	FUNCTION_END;
}

CGuiCountry::~CGuiCountry()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

void CGuiCountry::OnClose(void)
{
	FUNCTION_BEGIN;

	CCountryConn::Instance()->Release();
	GetGameGuiManager()->m_guiCountry = NULL;


	FUNCTION_END;
}


void CGuiCountry::OnCreate()
{
	FUNCTION_BEGIN;

	

	m_pListBoxTech   = GetListBox(ID_COUNTRY_LISTBOX_TECH)	 ;
#ifdef __UN_USED
	m_pListBoxMember = GetListBox(ID_COUNTRY_LISTBOX_MEMBER) ;
#else 
	m_pMLEditBoxAffiche = GetMLEditBox(ID_COUNTRY_MLEDITBOX_KING_SEND_AFFICLE);
#endif

	m_pListBoxArmy   = GetListBox(ID_COUNTRY_LISTBOX_ARMY)	 ;
	m_pListBoxArmyItem = GetListBox(ID_COUNTRY_LISTBOX_ARMYITEM);

	GetButton(ID_COUNTRY_BUTTON_TONG)->SetButtonGroup(1);
	GetButton(ID_COUNTRY_BUTTON_FRIEND)->SetButtonGroup(1);
	GetButton(ID_COUNTRY_BUTTON_SEPT)->SetButtonGroup(1);
	GetButton(ID_COUNTRY_BUTTON_SCHOOL)->SetButtonGroup(1);
	GetButton(ID_COUNTRY_BUTTON_COUNTRY)->SetButtonGroup(1);

	

//	CCountryConn::Instance()->Debug();

	CCountryConn::Instance()->SetDeaultTechList();

////////////////////////////////////////////////
//soke 取消国家科技
//	Refresh_ListBox_Tech();
////////////////////////////////////////////////
#ifdef __UN_USED
	Refresh_ListBox_Member();
#else
	if(GetScene()->GetMainCharacter()->IsKing())
	{
		m_pMLEditBoxAffiche->SetStyle(0x2b);
	}
	else
	{
		m_pMLEditBoxAffiche->SetStyle(0x3b); //readonly
	}
	std::string strAffiche;
	if( CCountryConn::Instance()->GetKingAfficle(strAffiche))
	{
		m_pMLEditBoxAffiche->SetText(strAffiche.c_str());
	}
	else 
	{
		CCountryConn::Instance()->RequestForKingAffiche();
	}
	
#endif
	Refresh_ListBox_Army();

	
	m_pStaticCountry = GetStatic(ID_COUNTRY_STATIC_COUNTRY);
	m_pStaticKing    = GetStatic(ID_COUNTRY_STATIC_KING);
	m_pStaticHonor   = GetStatic(ID_COUNTRY_STATIC_HONOR);
	m_pStaticTax     = GetStatic(ID_COUNTRY_STATIC_TAX);
	m_pStaticXking    = GetStatic(ID_COUNTRY_STATIC_XKING);     //soke 2016 - 3 - 11 国家官员表 - 国王
	m_pStaticCatcher  = GetStatic(ID_COUNTRY_STATIC_CATCHER);   //soke 2016 - 3 - 11 国家官员表 - 捕头（左上）
	m_pStaticCatchex  = GetStatic(ID_COUNTRY_STATIC_CATCHEX);   //soke 2016 - 3 - 11 国家官员表 - 捕头（左下）
	m_pStaticCensor = GetStatic(ID_COUNTRY_STATIC_CENSOR);      //soke 2016 - 3 - 11 国家官员表 - 御史大夫（右上）
	m_pStaticCensos = GetStatic(ID_COUNTRY_STATIC_CENSOS);      //soke 2016 - 3 - 11 国家官员表 - 御史大夫（右下）
	m_pStaticYuanshuai= GetStatic(ID_COUNTRY_STATIC_YUANSHUAI); //soke 2016 - 3 - 11 国家官员表 - 元帅
	m_pStaticZaixiang = GetStatic(ID_COUNTRY_STATIC_ZAIXIANG);  //soke 2016 - 3 - 11 国家官员表 - 宰相
	GetButton(ID_COUNTRY_BUTTON_FORUM)->SetEnabled(false);
	GetButton(ID_COUNTRY_BUTTON_FINANCE)->SetEnabled(false);
	GetButton(ID_COUNTRY_BUTTON_MILITARY)->SetEnabled(false);
	GetButton(ID_COUNTRY_BUTTON_DIPLOMACY)->SetEnabled(false);
 
	m_pStaticCountry->SetText(GetScene()->GetMainCharacter()->GetCountryName());
	m_pStaticKing->SetText(CCountry::Instance()->GetKing().c_str());
	
	//soke 2016 - 3 - 11 国家官员表 - 国王
	m_pStaticCountry->SetText(GetScene()->GetMainCharacter()->GetCountryName());
	m_pStaticXking->SetText(CCountry::Instance()->GetKing().c_str());

	//soke 2016 - 3 - 11 国家官员表 - 捕头（左上）
	m_pStaticCountry->SetText(GetScene()->GetMainCharacter()->GetCountryName());
	m_pStaticCatcher->SetText(CCountry::Instance()->GetCatcher().c_str());

	//soke 2016 - 3 - 11 国家官员表 - 捕头（左下）
	m_pStaticCountry->SetText(GetScene()->GetMainCharacter()->GetCountryName());
	m_pStaticCatchex->SetText(CCountry::Instance()->GetCatchex().c_str());

	//soke 2016 - 3 - 11 国家官员表 - 御史大夫（右上）
	m_pStaticCountry->SetText(GetScene()->GetMainCharacter()->GetCountryName());
	m_pStaticCensor->SetText(CCountry::Instance()->GetCensor().c_str());

	//soke 2016 - 3 - 11 国家官员表 - 御史大夫（右下）
	m_pStaticCountry->SetText(GetScene()->GetMainCharacter()->GetCountryName());
	m_pStaticCensos->SetText(CCountry::Instance()->GetCensos().c_str());

	//soke 国家官员表 - 元帅
	m_pStaticCountry->SetText(GetScene()->GetMainCharacter()->GetCountryName());
	m_pStaticYuanshuai->SetText(CCountry::Instance()->GetYuanshuai().c_str());

	//soke 国家官员表 - 宰相
	m_pStaticCountry->SetText(GetScene()->GetMainCharacter()->GetCountryName());
	m_pStaticZaixiang->SetText(CCountry::Instance()->GetZaixiang().c_str());
	///////////////////////////////////////////////////////////
	
	DWORD dwStarColor;
	char szShow[256]={0};
	extern bool ShowCountryStar(DWORD star,char* pStr,DWORD& color);
	if (ShowCountryStar(GetScene()->GetMainCharacter()->GetCountryStar(),szShow,dwStarColor)){
		m_pStaticHonor->SetTextColor(dwStarColor);
		m_pStaticHonor->SetText(szShow);
	}

	memset(szShow,0,256);
	_snprintf(szShow,255,"%d",CCountry::Instance()->GetTax());
	strcat(szShow,"%");
	szShow[255] = 0;
	m_pStaticTax->SetText(szShow);

	FUNCTION_END;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///////////////////////////////////////////////////////////
//soke 取消国家科技
/*
void    CGuiCountry::Refresh_ListBox_Tech()
{
	FUNCTION_BEGIN;

        m_pListBoxTech->RemoveAllItems();

 	for( size_t i = 0 ; i < CCountryConn::Instance()->m_vecFinishedTech.size() ; i++)
	{
		CCountry_stTech & Temp = CCountryConn::Instance()->m_vecFinishedTech[i];

		m_pListBoxTech->AddItem("",&Temp);
		int append_index = m_pListBoxTech->GetItemCount()-1;

		m_pListBoxTech->SetItemText2(append_index,0,Temp.strTech.c_str(),175);

		char szLevel[30];
		sprintf(szLevel,"%d级",Temp.dwTechLevel);
                m_pListBoxTech->SetItemText2(append_index,1,szLevel,190);

		m_pListBoxTech->SetItemText2(append_index,2,Temp.strEquip.c_str(),196);

		sprintf(szLevel,"可打造%d等级以下装备",Temp.dwEquipLevel);
		m_pListBoxTech->SetItemText2(append_index,3,szLevel,161);

	}

	FUNCTION_END;
}*/
////////////////////////////////////////////////////////////


#ifdef __UN_USED
void CGuiCountry::Refresh_ListBox_Member()
{
	FUNCTION_BEGIN;

	m_pListBoxMember->RemoveAllItems();

	for( int i = 0 ; i < CCountryConn::Instance()->m_vecMember.size() ; i++)
	{
		CCountry_stMemeber & Temp = CCountryConn::Instance()->m_vecMember[i];

	
		m_pListBoxMember->AddItem("",&Temp);

		int append_index = m_pListBoxMember->GetItemCount()-1;
		m_pListBoxMember->SetItemText2(append_index,0,Temp.strName.c_str(),140);

	
		m_pListBoxMember->SetItemText2(append_index,1,Temp.strTitle.c_str(),130);
	}

	FUNCTION_END;
}
#endif 


void CGuiCountry::Refresh_ListBox_Army()
{
	FUNCTION_BEGIN;

	m_pListBoxArmy->RemoveAllItems();

	for( int i = 0 ; i < CCountryConn::Instance()->m_vecCountryArmyBaseInfo.size() ; i++)
	{
		stArmyBaseInfo * pTemp = CCountryConn::Instance()->m_vecCountryArmyBaseInfo[i];

		m_pListBoxArmy->AddItem("",pTemp);

		int append_index = m_pListBoxArmy->GetItemCount()-1;

		m_pListBoxArmy->SetItemText2(append_index,0,pTemp->name,90);

		m_pListBoxArmy->SetItemText2(append_index,1,pTemp->cityname,100);

		m_pListBoxArmy->SetItemText2(append_index,2,pTemp->genname,112);

		char szText[20];
		sprintf(szText,"%d",pTemp->dwCapNum);
		m_pListBoxArmy->SetItemText2(append_index,3,szText,89,GuiListBoxColFmt_Center);

	}

	FUNCTION_END;
}

void CGuiCountry::Refresh_ListBox_ArmyItem()
{
	FUNCTION_BEGIN;


	int nCurItem = m_pListBoxArmy->GetCurItem();
	if(nCurItem !=-1)
	{
		const stArmyBaseInfo * pInfo  = (const stArmyBaseInfo *)m_pListBoxArmy->GetItemData(nCurItem);
		if(pInfo)
		{
			stRtnArmyItem & Item = CCountryConn::Instance()->m_HashMapArmyItem[pInfo->name];
			Item.strCity = pInfo->cityname;
			Item.strGen = pInfo->genname;
			Item.strName = pInfo->name;
			m_pListBoxArmyItem->RemoveAllItems();

			size_t i = 0 ;
			while(i < Item.vecBase.size())
			{
				m_pListBoxArmyItem->AddItem("",NULL);
				int append_index = m_pListBoxArmyItem->GetItemCount()-1;
				std::string strCaption;
				strCaption = Item.vecBase[i].strCaptionName;
				if(strcmp(Item.strGen.c_str(),strCaption.c_str()) == 0 )  // Is he or her the Gen Of Army? ^.^
					strCaption +="(将军)";
				m_pListBoxArmyItem->SetItemText2(append_index,0,strCaption.c_str(),380); 
				i++;
			}
		}

	}

	FUNCTION_END;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CGuiCountry::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;

	return CGuiDialog::OnRender(fElapsedTime);

	FUNCTION_END;
}

#define CGuiCountry_ButtonSetEnable(id,bEnable)	{CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ pBtn->SetEnabled(bEnable);}}
#define CGuiCountry_ButtonSetEnableDelay(id,bEnable)	{CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ pBtn->SetEnableDelay(bEnable,5500);}}
#define CGuiCountry_CallButtonSetEnableDelay(id) \
{\
	CGuiCountry_ButtonSetEnable(id,false);\
	CGuiCountry_ButtonSetEnableDelay(id,true);\
}


bool CGuiCountry::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
        FUNCTION_BEGIN;
	
	switch(nEvent)
	{
	case EVENT_LISTBOX_SELECTION:
		{
			if(nID == ID_COUNTRY_LISTBOX_ARMY)
			{
				int nCurSel = m_pListBoxArmy->GetCurItem();
				if(nCurSel!=-1)
				{
					const stArmyBaseInfo * pBaseInfo = (const stArmyBaseInfo *)m_pListBoxArmy->GetItemData(nCurSel);
					if(pBaseInfo)
					{

						const tHashMapArmyItem &  armyItemList = CCountryConn::Instance()->m_HashMapArmyItem;
						for(c_HashMapArmyItemIteator citer = armyItemList.begin(); citer != armyItemList.end() ;++citer)
						{
							const stRtnArmyItem & item = (*citer).second;

							if(  stricmp(item.strName.c_str(),pBaseInfo->name) == 0)
							{
								   Refresh_ListBox_ArmyItem(); 
                                                                    return true;
							}
						}
						if( citer == armyItemList.end())
						{
							stReqArmySpecUserCmd cmd;
							cmd.dwArmyID = pBaseInfo->dwArmyID;
							SEND_USER_CMD(cmd);
						}
					
					}
				}
			}
		}
		break;
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) 
			{
					case ID_COUNTRY_BUTTON_CLOSE:  // close 
						{
							//OnGuiEvent(EVENT_BUTTON_CLICKED,-1,NULL);
							if( GetGameGuiManager()->m_guiWorldConn)
								GetGameGuiManager()->m_guiWorldConn->OnGuiEvent(EVENT_BUTTON_CLICKED,3,NULL);
						}
						break;
					case ID_COUNTRY_BUTTON_TONG:
					case ID_COUNTRY_BUTTON_FRIEND:
					case ID_COUNTRY_BUTTON_SEPT:
					case ID_COUNTRY_BUTTON_SCHOOL:
						{
							SwitchToWorldConnDlg(nID);
						}
						break;
					case ID_COUNTRY_BUTTON_COUNTRY:
						{
							GetButton(nID)->SetChecked(true);
						}
						break;
					case ID_COUNTRY_BUTTON_FORUM:
						{
						}	 
						break;
					case ID_COUNTRY_BUTTON_FINANCE:
						{
						}
						break;
					case ID_COUNTRY_BUTTON_MILITARY:
						{
						}
						break;
					case ID_COUNTRY_BUTTON_DIPLOMACY:
						{
						}
						break;
					case ID_COUNTRY_BUTTON_KING_SEND_AFFICLE:
						{
							if(m_pMLEditBoxAffiche)
							{
								stCountryNoteCmd cmd;
								_snprintf(cmd.note,sizeof(cmd.note),"%s",m_pMLEditBoxAffiche->GetText());
								cmd.note[sizeof(cmd.note)-1] = 0;
								SEND_USER_CMD(cmd);

								CGuiCountry_CallButtonSetEnableDelay(ID_COUNTRY_BUTTON_KING_SEND_AFFICLE);
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



void CGuiCountry::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());
		m_pBtnClose->SetLocation(pt.x,pt.y+26);
	}

	FUNCTION_END;
}

void CGuiCountry::RefreshAffiche()
{
        FUNCTION_BEGIN;
	std::string strAffiche;
	if(CCountryConn::Instance()->GetKingAfficle(strAffiche))
	{
		if(m_pMLEditBoxAffiche)
		{
			m_pMLEditBoxAffiche->SetText(strAffiche.c_str());

		}

	}
	FUNCTION_END;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CGuiCountry::SwitchToWorldConnDlg(DWORD dwBtnID)
{
	FUNCTION_BEGIN;

	m_dwBtnIDClicked = dwBtnID;
	GetButton(dwBtnID)->SetChecked(true);

	if( GetGameGuiManager()->m_guiWorldConn )
	{
		this->SetVisible(false);
		this->SetEnabled(false);
		POINT pt = this->GetLocation();

		GetGameGuiManager()->m_guiWorldConn->SetLocation(pt.x,pt.y);
		GetGameGuiManager()->m_guiWorldConn->SetVisible(true);
		GetGameGuiManager()->m_guiWorldConn->SetEnabled(true);
		GetGameGuiManager()->m_guiWorldConn->SwitchFromCountryDlg();
		
	}
	
	FUNCTION_END;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CGuiCountry::SwitchFromWorldConnDlg()
{
	FUNCTION_BEGIN;

	if( GetGameGuiManager()->m_guiWorldConn )
	{
		GetGameGuiManager()->m_guiWorldConn->SetVisible(false);
		GetGameGuiManager()->m_guiWorldConn->SetEnabled(false);

		stPointI pt = GetGameGuiManager()->m_guiWorldConn->GetLocation();
		this->SetLocation(pt.x,pt.y);
		this->SetVisible(true);
		this->SetEnabled(true);
		GetButton(ID_COUNTRY_BUTTON_COUNTRY)->SetChecked(TRUE);
		 
		// NetWork 
		RequestFinishedTechList();
		RequestCountryArmyList();
	}

	FUNCTION_END;
}

//--------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
CArmyManager * CArmyManager::m_Singleton = 0;

CArmyManager::CArmyManager()
{
	m_guiCreateArmyDlg = NULL;
	m_guiArmyListDlg = NULL;
	m_guiArmyItemDlg = NULL;
	assert(m_Singleton == 0);
	m_Singleton = this;
	m_pAddArmyCaptainMsg = NULL;

	m_HashMapArmyItem.clear();
	m_vecCityArmyBaseInfo.clear();
}

CArmyManager::~CArmyManager()
{
	SafeRelease();
}

void CArmyManager::SafeRelease()
{
	if(m_Singleton) 
		delete m_Singleton;
	m_Singleton = NULL;
}

CArmyManager * CArmyManager::GetSingleton()
{
	if( m_Singleton == 0)
	{
		m_Singleton = new CArmyManager;
		//if( !m_Singleton)
                //      throw("memory not enough!")
	}
	return m_Singleton;
}

void CArmyManager::QuestCityArmyList()
{
	stReqArmyListUserCmd cmd;
	cmd.byType = CITY_ARMY_LIST;
	SEND_USER_CMD(cmd);
}

void CArmyManager::SetWaitGenList(const stRtnWaitGenUserCmd* pCmd)
{
	SAFE_CHECK_COUNTRY_MSG(pCmd);

	FreePointerList<tVecWaitGen>(m_vecWaitGen);

	for (size_t i=0;i<pCmd->dwSize;++i)
	{
		stWaitGenItem * st = new stWaitGenItem;
		if( !st) continue;

		st->dwCharID = pCmd->data[i].dwCharID;
		strcpy(st->szName,pCmd->data[i].szName);
		st->szName[sizeof(st->szName)-1] = 0;

		m_vecWaitGen.push_back(st);
	}

	if( m_guiCreateArmyDlg && m_guiCreateArmyDlg->IsVisible())
	{
		m_guiCreateArmyDlg->RefreashGenList();
	}

}

void CArmyManager::SetCityArmyList(const stRtnArmyListUserCmd* pCmd)
{
	if( pCmd->byType == CITY_ARMY_LIST)// 用于NPC对话，显示本城军队列表
	{

		FreePointerList<tVecArmyBaseInfo>(m_vecCityArmyBaseInfo);

		for(size_t i = 0; i < pCmd->dwSize; ++i)
		{
			stArmyBaseInfo * st = new stArmyBaseInfo;
			if( !st) continue;

			st->dwArmyID = pCmd->data[i].dwArmyID;
			strcpy(st->cityname,pCmd->data[i].cityname);
			strcpy(st->genname,pCmd->data[i].genname);
			strcpy(st->name,pCmd->data[i].name);
			st->dwCapNum = pCmd->data[i].dwCapNum;

			m_vecCityArmyBaseInfo.push_back(st);
		}

	}
	this->AddArmyListDlg();
	m_guiArmyListDlg->AddArmyList();
}

void CArmyManager::QuestCityArmyItem(DWORD dwArmyID)
{
	stReqArmySpecUserCmd cmd;
	cmd.dwArmyID = dwArmyID;
	SEND_USER_CMD(cmd);
}


void CArmyManager::SetCityArmyItem(const stRtnArmySpecUserCmd * pCmd)
{
      std::string strName = pCmd->name;
      stRtnArmyItem & Value = m_HashMapArmyItem[strName];
      Value.strName =  pCmd->name;  // 军队名
      Value.strName =  pCmd->cityname;  // 城市名
      Value.strGen = pCmd->genname; //将军名
     
      Value.vecBase.clear();
      for(size_t i = 0 ; i < pCmd->dwSize; ++i)
      {
               _stCaptionBase base;
	       base.dwCharID  = pCmd->data[i].dwCharID;
	       base.strCaptionName  = pCmd->data[i].name;
	       Value.vecBase.push_back(base);
      }

      if( m_guiArmyItemDlg && m_guiArmyItemDlg->IsVisible())
      {
	      m_guiArmyItemDlg->AddData(&Value);
      }
}

const stArmyBaseInfo *  CArmyManager::GetArmyItem(DWORD ArmyItem)
{
 
	for( c_ArmyBaseInfoIterator iter = m_vecCityArmyBaseInfo.begin() ; iter != m_vecCityArmyBaseInfo.end() ; ++iter)
	{
	   const stArmyBaseInfo * pBaseInfo = *iter;
	   if( pBaseInfo->dwArmyID == ArmyItem)
	   {
                  return pBaseInfo;
	   }
	}
	return NULL;
}

void CArmyManager::CopyAddArmyCaptionUserCmd(const stAddArmyCaptainUserCmd * pCmd)
{
     ClearAddArmyCaptionUserCmd();
      m_pAddArmyCaptainMsg = new stAddArmyCaptainUserCmd;
      if( m_pAddArmyCaptainMsg)
	      memcpy(m_pAddArmyCaptainMsg,pCmd,sizeof(stAddArmyCaptainUserCmd));
}

void CArmyManager::ClearAddArmyCaptionUserCmd()
{
	if( m_pAddArmyCaptainMsg)
		delete m_pAddArmyCaptainMsg;
	m_pAddArmyCaptainMsg = NULL;
}
void CArmyManager::DiscreteArmy()
{
	AddArmyListDlg();
	
	m_guiArmyListDlg->AddArmyList();
}


const stRtnArmyItem  *  CArmyManager::GetArmyDetailItem(const char * ArmyName)
{
	const tHashMapArmyItem &  armyItemList = CArmyManager::GetSingleton()->GetCityArmyItemList();
	for(c_HashMapArmyItemIteator citer = armyItemList.begin(); citer != armyItemList.end() ;++citer)
	{
		const stRtnArmyItem & item = (*citer).second;

		if(  stricmp(item.strName.c_str(),ArmyName) == 0)
		{
                      return &item;
		}
	}
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define ID_EDITBOX_ID    2 
#define ID_COMBOX_ID     3 
#define ID_BUTTON_OK	 5
#define ID_BUTTON_CANCEL 6

CGuiCreateArmy::CGuiCreateArmy()
{
	m_pComboxGenName = NULL;
	m_pEditBoxArmyName = NULL;
	m_bSmDialog = true;
}

CGuiCreateArmy::~CGuiCreateArmy()
{

}

void Net_Army_QuestGenList()
{
	stReqWaitGenUserCmd cmd;
	SEND_USER_CMD(cmd);

	if(g_bSinglePlayer)
	{
		size_t size = sizeof(stRtnWaitGenUserCmd)+3*sizeof(stWaitGenItem);
		char* pchar = new char[size];
		stRtnWaitGenUserCmd * cmd = (stRtnWaitGenUserCmd *)pchar;  
		cmd->dwSize = 3;
		cmd->data[0].dwCharID = 1001;
		strcpy(cmd->data[0].szName,"将军1");

		cmd->data[1].dwCharID = 2001;
		strcpy(cmd->data[1].szName,"将军2");

		cmd->data[2].dwCharID = 3001;
		strcpy(cmd->data[2].szName,"将军3");

		GetClient()->PushCommand(cmd,size);

		delete []pchar;
	}
}



void CGuiCreateArmy::OnCreate()
{
	CGuiDialog::OnCreate();

	Net_Army_QuestGenList();

	m_pEditBoxArmyName = GetEditBox(ID_EDITBOX_ID);
	m_pComboxGenName = GetComboBox(ID_COMBOX_ID);      // The gen name of Army 
}

void CGuiCreateArmy::OnClose()
{
	CArmyManager::GetSingleton()->m_guiCreateArmyDlg = NULL;
}

bool CGuiCreateArmy::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		  case ID_BUTTON_OK:
			{
				OnInputEnter();
				Close();
				return true;
			}
			  break;
		  case ID_BUTTON_CANCEL:
			  {
				  Close();
				  return true;
			  }
			  break;

		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

bool CGuiCreateArmy::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case VK_RETURN:
				//OnInputEnter();
				break;
			}
		}
		break;
	}
	return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);
}

void CGuiCreateArmy::OnInputEnter()
{
	char * pText = (char *)m_pEditBoxArmyName->GetText();
	str_trim_left(pText);
	str_trim_right(pText);
	if( 0 == strlen(pText))
		return;

	pText = (char *)m_pComboxGenName->GetText();
	str_trim_left(pText);
	str_trim_right(pText);
	if( 0 == strlen(pText))
		return;

	stCreateArmyUserCmd cmd;
	_snprintf(cmd.armyName,sizeof(cmd.armyName)-1,m_pEditBoxArmyName->GetText());
	cmd.armyName[sizeof(cmd.armyName)-1]  = 0;
	_snprintf(cmd.genName,sizeof(cmd.genName),m_pComboxGenName->GetText());

	SEND_USER_CMD(cmd);
}

void CGuiCreateArmy::RefreashGenList()
{
	m_pComboxGenName->RemoveAllItems();

	const tVecWaitGen& GenList = CArmyManager::GetSingleton()->GetWaitGenList();
	for( c_WaitGenIterator iter = GenList.begin(); iter != GenList.end() ; ++iter)
	{
		const stWaitGenItem *  pItem = *iter;
		m_pComboxGenName->AddItem(pItem->szName,(void *)pItem->dwCharID);
	}

}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


struct stArmyCmdItem
{
	enum enumCMD
	{
		CMD_DISCRETAMY, //解散军队
		CMD_CHANGEARMYNAME,
	//	CMD_RECRUITARMYER,
		CMD_VIEWARMYER,
		CMD_NUM,
	};
	enumCMD		cmd;
	std::string     strCmd;
	bool            bCanUse;
};



static stArmyCmdItem s_army_cmdItem[] = 
{
	{ stArmyCmdItem::CMD_DISCRETAMY,		"解散军队",            true },
	{ stArmyCmdItem::CMD_CHANGEARMYNAME,		"更改军队名称",            true },
	//{ stArmyCmdItem::CMD_RECRUITARMYER,		"招收队长",            true },
	{ stArmyCmdItem::CMD_VIEWARMYER,		"查看军队详细信息",            true },
};

CGuiArmyListDlg::CGuiArmyListDlg(void)
{
	FUNCTION_BEGIN;
	m_pList = NULL;
	m_bSmDialog = true;
	m_menu = NULL;
	FUNCTION_END;
}

CGuiArmyListDlg::~CGuiArmyListDlg(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}


void CGuiArmyListDlg::OnCreate(void)
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();

	GetStatic(1)->SetText("军队列表:");
	m_pList = GetListBox(2);

	stPointI __pt = m_pList->GetLocation();
	m_rcListBox.left = __pt.x ;
	m_rcListBox.top = __pt.y ;
	m_rcListBox.right = m_rcListBox.left + m_pList->GetWidth();
	m_rcListBox.bottom = m_rcListBox.top + m_pList->GetHeight();
	//////////////////////////////////////////////////////////////////////////////
	m_menu = GetGuiManager()->CreateMenu( this );
	for ( int i = 0 ; i < count_of(s_army_cmdItem) ; i++ )
	{
		m_menu->AddMenuItem( i, s_army_cmdItem[i].strCmd.c_str(), CGuiMenu::MenuStyle_Command, s_army_cmdItem[i].bCanUse );
	}
	SetPopupMenu( m_menu );
	//GetButton(3)->SetVisible(false);
	GetButton(4)->SetVisible(false);
	//GetButton(3)->SetEnabled(false);
	GetButton(4)->SetEnabled(false);
	//////////////////////////////////////////////////////////////////////////////

	FUNCTION_END;
}
bool CGuiArmyListDlg::OnContextMenu()
{
	FUNCTION_BEGIN;

	if( m_menu )
	{

		if( m_pList->GetCurItem() == -1)
		{
			GetGameGuiManager()->AddClientSystemMessage("请先选择一个队长!");
			return false;
		}

		/*stPointI ptCursor = Engine_GetCursor()->GetPosition();
		stPointI _ptCursor;
		_ptCursor.x = ptCursor.x - m_x;
		_ptCursor.y = ptCursor.y - m_y;

		int iFirstShowItem = m_pList->GetFirstShowItem();
		int iLastShowItem  = m_pList->GetLastShowItem();
		if( m_rcListBox.PtInRect(_ptCursor))
		{
			int iii = 0; 
		}*/
		/*stRectI rcTemp = m_rcListBox;
		rcTemp.bottom = rcTemp.top + m_pList->GetTextHeight();
		for(int i = iFirstShowItem;i <= iLastShowItem;i++)
		{
			if( rcTemp.PtInRect(_ptCursor) )
			{
				m_pList->SetCurItem(i);*/
				m_menu->Popup();
				return true;
			/*}
			rcTemp.OffsetRect(0,m_pList->GetTextHeight());
		}		*/
	}
	return false;

	FUNCTION_END;
}

void CGuiArmyListDlg::AddArmyList()
{
	
	const tVecArmyBaseInfo & vecBaseInfo = CArmyManager::GetSingleton()->GetCityArmyBaseInfo();
	for(c_ArmyBaseInfoIterator iter = vecBaseInfo.begin() ;iter != vecBaseInfo.end();++iter)
	{
		const  stArmyBaseInfo * pInfo = *iter;
		AddListItem(pInfo->name,(void *)pInfo->dwArmyID);
	}		 
}

bool CGuiArmyListDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case 3: return OnIDOk();
		case 4: return OnIDCancel();
		}
	}
	if( nEvent == EVENT_DIALOG_COMMAND)
	{
			switch( nID )
			{
			case stArmyCmdItem::CMD_DISCRETAMY: //解散军队
				{
					stRemoveArmyUserCmd cmd;
					cmd.dwArmyID = (DWORD)GetCurSelectItemData();
					SEND_USER_CMD(cmd);
					Close();
				}
				break;
			case stArmyCmdItem::CMD_CHANGEARMYNAME://更改军队名称
				{
				       if(GetScene()->GetMainCharacter()->IsCityLeader() || GetScene()->GetMainCharacter()->IsKing())
					       GetGameGuiManager()->AddInputBox("更改军队名称为:",eAction_ChangeArmyName,(void *)GetCurSelectItemData());
				       else 
					       GetGameGuiManager()->AddClientSystemMessage("对不起,你不是城主!");

				}
				break;
			//case stArmyCmdItem::CMD_RECRUITARMYER://招收队长
			//	{
			//		if( !GetScene()->GetMainCharacter()->IsGen()) 
			//		{
			//			GetGameGuiManager()->AddClientSystemMessage("对不起,你不是将军!");
			//			break;
			//		}

			//		DWORD dwArmyID = (DWORD)GetCurSelectItemData();
			//		const stArmyBaseInfo *  pInfo = CArmyManager::GetSingleton()->GetArmyItem(dwArmyID);
			//		if(pInfo){
			//			GetGameGuiManager()->AddInputBox("招收队长:",eAction_RecruitArmy,(void *)pInfo);
			//		}
			//	}
			//	break;
			case stArmyCmdItem::CMD_VIEWARMYER://查看军队详细信息
				{
					CArmyManager::GetSingleton()->AddArmyItemDlg();
					DWORD dwArmyID = (DWORD)GetCurSelectItemData();
					const stArmyBaseInfo *  pInfo = CArmyManager::GetSingleton()->GetArmyItem(dwArmyID);
					const stRtnArmyItem  *  pItem = CArmyManager::GetSingleton()->GetArmyDetailItem(pInfo->name);
					if( !pItem )
					{
						DWORD dwArmyID = (DWORD)GetCurSelectItemData();
						CArmyManager::GetSingleton()->QuestCityArmyItem(dwArmyID);
					}
					else 
					{
						CArmyManager::GetSingleton()->m_guiArmyItemDlg->AddData(pItem);
					}
					
				}
				break;
			}
	}
		
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiArmyListDlg::OnClose(void)
{
	FUNCTION_BEGIN;
	CArmyManager::GetSingleton()->m_guiArmyListDlg = NULL;
	FUNCTION_END;
}

HRESULT CGuiArmyListDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	return hr;
	FUNCTION_END;
}

void CGuiArmyListDlg::OnSetCloseLocation()
{
}

bool CGuiArmyListDlg::OnIDOk()
{
	FUNCTION_BEGIN;
	
	Close();
	return true;
	FUNCTION_END;
}

bool CGuiArmyListDlg::OnIDCancel()
{
	FUNCTION_BEGIN;
	Close();
	return true;
	FUNCTION_END;
}

void CGuiArmyListDlg::SetText(const char* pszText)
{
	FUNCTION_BEGIN;
	_snprintf(m_strText,MAX_PATH,pszText);
	m_strText[MAX_PATH - 1] = 0;
	FUNCTION_END;
}

void CGuiArmyListDlg::AddListItem(const char *szText, void *pData  ,DWORD color ,IBitmaps* icon)
{
	FUNCTION_BEGIN;
	if( m_pList )
	{
		m_pList->AddItem(szText,pData,color);
	}
	FUNCTION_END;
}

int CGuiArmyListDlg::GetCurSelectItem()
{
	FUNCTION_BEGIN;
	if( m_pList )
	{
		return  m_pList->GetCurItem();
	}
	return -1;
	FUNCTION_END;
}

const char* CGuiArmyListDlg::GetCurItemText()
{
	FUNCTION_BEGIN;
	if( m_pList )
	{
		int curIndex = GetCurSelectItem();
		if( curIndex >= 0 && curIndex < m_pList->GetCount() )
		{
			return  m_pList->GetItemText(curIndex);
		}
	}
	return NULL;
	FUNCTION_END;
}

void* CGuiArmyListDlg::GetCurSelectItemData()
{
	FUNCTION_BEGIN;
	if( m_pList )
	{
		int curIndex = GetCurSelectItem();
		if( curIndex >= 0 && curIndex < m_pList->GetCount() )
		{
			return  m_pList->GetItemData(curIndex);
		}		
	}
	return NULL;
	FUNCTION_END;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct stArmyCmdItem2
{
	enum enumCMD
	{
		CMD_FUCKARMYER,
		CMD_NUM,
	};
	enumCMD		cmd;
	std::string     strCmd;
	bool            bCanUse;
};


static stArmyCmdItem2 s_army2item_cmdItem[] = 
{
	{ stArmyCmdItem2::CMD_FUCKARMYER,		"开除队长",            true },
};


CGuiArmyItemDlg::CGuiArmyItemDlg(void)
{
	FUNCTION_BEGIN;
	m_pList = NULL;
	m_bSmDialog = true;
	m_menu = NULL;
	FUNCTION_END;
}

CGuiArmyItemDlg::~CGuiArmyItemDlg(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}


void CGuiArmyItemDlg::OnCreate(void)
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();

	GetStatic(1)->SetText("该军队详细信息:");
	m_pList = GetListBox(2);

	stPointI __pt = m_pList->GetLocation();
	m_rcListBox.left = __pt.x ;
	m_rcListBox.top = __pt.y ;
	m_rcListBox.right = m_rcListBox.left + m_pList->GetWidth();
	m_rcListBox.bottom = m_rcListBox.top + m_pList->GetHeight();
	//////////////////////////////////////////////////////////////////////////////
	m_menu = GetGuiManager()->CreateMenu( this );
	for ( int i = 0 ; i < count_of(s_army2item_cmdItem) ; i++ )
	{
		m_menu->AddMenuItem( i, s_army2item_cmdItem[i].strCmd.c_str(), CGuiMenu::MenuStyle_Command, s_army2item_cmdItem[i].bCanUse );
	}
	SetPopupMenu( m_menu );
	//GetButton(3)->SetVisible(false);
	GetButton(4)->SetVisible(false);
	//GetButton(3)->SetEnabled(false);
	GetButton(4)->SetEnabled(false);
	//////////////////////////////////////////////////////////////////////////////

	FUNCTION_END;
}

bool CGuiArmyItemDlg::OnContextMenu()
{
	FUNCTION_BEGIN;

	if( m_menu )
	{

		if( m_pList->GetCurItem() == -1)
		{
			GetGameGuiManager()->AddClientSystemMessage("请先选择一个队长!");
			return false;
		}
		if( !GetScene()->GetMainCharacter()->IsGen()) 
		{
			GetGameGuiManager()->AddClientSystemMessage("对不起,你不是将军!");
			return false;
		}


	/*	stPointI ptCursor = Engine_GetCursor()->GetPosition();
		stPointI _ptCursor;
		_ptCursor.x = ptCursor.x - m_x;
		_ptCursor.y = ptCursor.y - m_y;

		int iFirstShowItem = m_pList->GetFirstShowItem();
		int iLastShowItem  = m_pList->GetLastShowItem();
		if( m_rcListBox.PtInRect(_ptCursor))
		{
			int iii = 0; 
		}
		stRectI rcTemp = m_rcListBox;
		rcTemp.bottom = rcTemp.top + m_pList->GetTextHeight();
		for(int i = iFirstShowItem;i <= iLastShowItem;i++)
		//{
			if( rcTemp.PtInRect(_ptCursor) )
			{
				m_pList->SetCurItem(i);*/
				m_menu->Popup();
				return true;
			/*}
			rcTemp.OffsetRect(0,m_pList->GetTextHeight());*/
		//}		
	}
	return false;

	FUNCTION_END;
}

bool CGuiArmyItemDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case 3: return OnIDOk();
		case 4: return OnIDCancel();
		}
	}
	if( nEvent == EVENT_DIALOG_COMMAND)
	{
		switch( nID )
		{
		case stArmyCmdItem2::CMD_FUCKARMYER://开除队长
			{ 
				if( m_pList )
				{
					void * pData = GetCurSelectItemData();
					if( NULL != pData )
					{
						void * pData = GetCurSelectItemData();
						if( NULL != pData )
						{
							stFireArmyCaptainUserCmd cmd;
							if(0 == strcmp(GetCurItemText(),GetScene()->GetMainCharacter()->GetName()))
							{
								GetGameGuiManager()->AddClientSystemMessage("不能开除自己!");
								return true;
							}
							else{
							}
							cmd.dwUserID = (DWORD)pData;
							SEND_USER_CMD(cmd);
						}		
					}		
				}
				Close();
			}
			break;
		}
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiArmyItemDlg::OnClose(void)
{
	FUNCTION_BEGIN;
	CArmyManager::GetSingleton()->m_guiArmyItemDlg = NULL;
	FUNCTION_END;
}

HRESULT CGuiArmyItemDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	return hr;
	FUNCTION_END;
}

void CGuiArmyItemDlg::OnSetCloseLocation()
{
}

bool CGuiArmyItemDlg::OnIDOk()
{
	FUNCTION_BEGIN;
	Close();
	return true;
	FUNCTION_END;
}

bool CGuiArmyItemDlg::OnIDCancel()
{
	FUNCTION_BEGIN;
	Close();
	return true;
	FUNCTION_END;
}

void CGuiArmyItemDlg::SetText(const char* pszText)
{
	FUNCTION_BEGIN;
	_snprintf(m_strText,MAX_PATH,pszText);
	m_strText[MAX_PATH - 1] = 0;
	FUNCTION_END;
}

void CGuiArmyItemDlg::AddListItem(const char *szText, void *pData  ,DWORD color ,IBitmaps* icon)
{
	FUNCTION_BEGIN;
	if( m_pList )
	{
		m_pList->AddItem(szText,pData,color);
	}
	FUNCTION_END;
}


void CGuiArmyItemDlg::AddData(const stRtnArmyItem * pItem)
{
      m_pList->RemoveAllItems();
      for(size_t i = 0 ; i < pItem->vecBase.size() ; i++)
      {
	      const _stCaptionBase & base =pItem->vecBase[i];
	     AddListItem(base.strCaptionName.c_str(),(void *)base.dwCharID);
      }
}

int CGuiArmyItemDlg::GetCurSelectItem()
{
	FUNCTION_BEGIN;
	if( m_pList )
	{
		return  m_pList->GetCurItem();
	}
	return -1;
	FUNCTION_END;
}

const char* CGuiArmyItemDlg::GetCurItemText()
{
	FUNCTION_BEGIN;
	if( m_pList )
	{
		int curIndex = GetCurSelectItem();
		if( curIndex >= 0 && curIndex < m_pList->GetCount() )
		{
			return  m_pList->GetItemText(curIndex);
		}
	}
	return NULL;
	FUNCTION_END;
}

void* CGuiArmyItemDlg::GetCurSelectItemData()
{
	FUNCTION_BEGIN;
	if( m_pList )
	{
		int curIndex = GetCurSelectItem();
		if( curIndex >= 0 && curIndex < m_pList->GetCount() )
		{
			return  m_pList->GetItemData(curIndex);
		}		
	}
	return NULL;
	FUNCTION_END;
}

CGuiCreateArmy *  CArmyManager::AddCreateArmyGuiDlg()
{
	FUNCTION_BEGIN;

	if(!m_guiCreateArmyDlg)
	{
		CGuiCreateArmy * pDlg = new CGuiCreateArmy;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\CreateArmyDialog.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiCreateArmyDlg = pDlg;
	}
	m_guiCreateArmyDlg->SetVisible(true);
	m_guiCreateArmyDlg->SetEnabled(true);

	return m_guiCreateArmyDlg;

	FUNCTION_END;
}


CGuiArmyListDlg *  CArmyManager::AddArmyListDlg()
{
	FUNCTION_BEGIN;

	if(!m_guiArmyListDlg)
	{
		CGuiArmyListDlg * pDlg = new CGuiArmyListDlg;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\selectdialog.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiArmyListDlg = pDlg;
	}
	m_guiArmyListDlg->SetVisible(true);
	m_guiArmyListDlg->SetEnabled(true);

	return m_guiArmyListDlg;

	FUNCTION_END;
}

CGuiArmyItemDlg*  CArmyManager::AddArmyItemDlg()
{	
	if(!m_guiArmyItemDlg)
	{
		CGuiArmyItemDlg * pDlg = new CGuiArmyItemDlg;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\selectdialog.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiArmyItemDlg = pDlg;
	}
	m_guiArmyItemDlg->SetVisible(true);
	m_guiArmyItemDlg->SetEnabled(true);

	return m_guiArmyItemDlg;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CGuiAntiDareCountry *  CCountryConn::AddAntiDareCountryDlg(const  COUNTRYLIST_USAGE & usage)
{
	FUNCTION_BEGIN;

	if(!m_guiAntiDareCountryDlg)
	{
		CGuiAntiDareCountry * pDlg = new CGuiAntiDareCountry(usage);
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\antidarecountry.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiAntiDareCountryDlg = pDlg;
	}
	m_guiAntiDareCountryDlg->SetVisible(true);
	m_guiAntiDareCountryDlg->SetEnabled(true);

	return m_guiAntiDareCountryDlg;

	FUNCTION_END;
}

CGuiAntiDareCountry::CGuiAntiDareCountry(const COUNTRYLIST_USAGE & usage)
:m_pListBox(NULL)
,m_dlgUsage(usage)
{
	m_bSmDialog = TRUE;
}

void CGuiAntiDareCountry::OnClose(void)
{
	FUNCTION_BEGIN;

	CCountryConn::Instance()->m_guiAntiDareCountryDlg = NULL;
	GetGameGuiManager()->OnNpcActionEnd();
	//GetGameGuiManager()->EndNpcDialog();

	FUNCTION_END;
}

void CGuiAntiDareCountry::OnCreate()
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();


	m_pListBox = GetListBox(2);

	CGuiStatic  * pStatic = GetStatic(1);
	if( pStatic )
	{
		if( m_dlgUsage == enumAntiDareCountry)
		{
			pStatic->SetText("申请国战反攻:");
		}
		else if( m_dlgUsage == enumAllyCountry )
		{
			pStatic->SetText("缔结盟约:"); 
		}
	}
	
	if (m_pListBox)
	{
		m_pListBox->RemoveAllItems();

		const CCountry::COUNTRYLIST  & countrylist = CCountry::Instance()->GetCountryList();

		CCountry::COUNTRYLIST::const_iterator it;
		for( it = countrylist.begin(); it != countrylist.end(); ++it )
		{
			if(it->id == GetScene()->GetMainCharacter()->GetCountry())
			{
				//if (GetStatic(1)) GetStatic(1)->SetText(it->pstrName);
			}
			else
			{
				if( strstr(it->pstrName,"中立") != 0 )
					continue;
				m_pListBox->AddItem("",(void*)it->id);
				m_pListBox->SetItemText(m_pListBox->GetCount()-1,0,"  ",2);
				m_pListBox->SetItemText(m_pListBox->GetCount()-1,1,it->pstrName);
			}
		}
	}


	FUNCTION_END;
}

bool CGuiAntiDareCountry::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;


	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
		
			switch(nID) 
			{
			case 3:
				{
					if (m_pListBox)
					{
						if( m_dlgUsage == enumAntiDareCountry)
						{
							stAntiDareCountryFormalCmd cmd;
							cmd.dwCountryID = (DWORD) m_pListBox->GetItemData(m_pListBox->GetCurItem());
							SEND_USER_CMD(cmd);
						}
						else if( m_dlgUsage == enumAllyCountry )
						{
							stReqCountryAlly  cmd;
							cmd.dwAllyCountryID = (DWORD) m_pListBox->GetItemData(m_pListBox->GetCurItem());
							cmd.byStatus = QUESTION_COUNTRY_ALLY;
							SEND_USER_CMD(cmd);        
						}
					}
					Close();
				
				}
				break;
			case 4:
				{
					Close();
				}
				break;
			}
		}
		break;

	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiAntiDareCountry::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt( m_pBtnClose->GetLocation() );

		m_pBtnClose->SetLocation(pt.x,pt.y + 24);
	}

	FUNCTION_END;
}

CGuiQueryGenDlg *  CCountryConn::AddGenInfoDlg()
{
	FUNCTION_BEGIN;

	if(!m_guiGenInfoDlg)
	{
		CGuiQueryGenDlg * pDlg = new CGuiQueryGenDlg();
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\querygen.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiGenInfoDlg = pDlg;
	}
	m_guiGenInfoDlg->SetVisible(true);
	m_guiGenInfoDlg->SetEnabled(true);

	return m_guiGenInfoDlg;

	FUNCTION_END;
}

 
//-------------------------------------------------------------------------------------------
// CGuiQueryOccupierDlg
//-------------------------------------------------------------------------------------------
#define ID_QUERYOCCUPIER_LISTBOX        2 
#define ID_QUERYOCCUPIER_HEADER_ITEM_1  3  
#define ID_QUERYOCCUPIER_HEADER_ITEM_2  4
#define ID_QUERYOCCUPIER_HEADER_ITEM_3  5
#define ID_QUERYOCCUPIER_BUTTON_OK      10


CGuiQueryOccupierDlg::CGuiQueryOccupierDlg(const Occupier_Usage & usage)
: m_Usage(usage)
{
	//m_bSmDialog = TRUE;
}

void CGuiQueryOccupierDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	CCountryConn::Instance()->m_guiOccupierDlg = NULL;
	GetGameGuiManager()->OnNpcActionEnd(); 

	FUNCTION_END;
}

void CGuiQueryOccupierDlg::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	
	UpdataList();

	FUNCTION_END;
}



void CGuiQueryOccupierDlg::UpdataList()
{

	//struct stColumnInfo
	//{
	//	int nColWidthAsFont;  // 单位为字体的大小，像素大小 = nColWidthAsFont * GetFontWidth(*);
	//	std::string strCaption; 
	//};

	CGuiStatic  * pStaticHead1 = GetStatic(ID_QUERYOCCUPIER_HEADER_ITEM_1);
	CGuiStatic  * pStaticHead2  = GetStatic(ID_QUERYOCCUPIER_HEADER_ITEM_2);
	CGuiStatic  * pStaticHead3  = GetStatic(ID_QUERYOCCUPIER_HEADER_ITEM_3);
	if( ! (pStaticHead1 && pStaticHead2 && pStaticHead3) )
		return;

	CGuiListBox * pListBox = GetListBox(ID_QUERYOCCUPIER_LISTBOX);
	if( !pListBox )
		return;
	pListBox->RemoveAllItems();

	CGuiButton * pButton = GetButton(ID_QUERYOCCUPIER_BUTTON_OK);
	if( !pButton)
		return;

	if( m_Usage == Usage_OccupierTong)
	{
		SetSize(364,430);
		pListBox->SetSize(300,300);
		pButton->SetLocation(80,314);

		pStaticHead1->SetText("城市");
		pStaticHead2->SetText("占领帮会");
		pStaticHead3->SetVisible(false);

		pStaticHead1->SetLocation(-24,2);
		pStaticHead2->SetLocation(130,2);

		const ListOccupierTong  & __TongList = CCountryConn::Instance()->m_ListOccupierTong;
		ConstIter_OccupierTong it;
		for( it = __TongList.begin(); it != __TongList.end(); ++it )
		{  
			const stOccupierTongItem & item = *it;
			pListBox->AddItem("",NULL);
			pListBox->SetItemText(pListBox->GetCount()-1,0,item.strCityName.c_str(),20);
			pListBox->SetItemText(pListBox->GetCount()-1,1,item.strTongName.c_str(),20);

		}
	}
	else if(m_Usage == Usage_OccupierSept)
	{
		SetSize(464,430);
		pListBox->SetSize(400,300);
		pButton->SetLocation(116,314);

		pStaticHead1->SetText("NPC");
		pStaticHead2->SetText("占领家族");
		pStaticHead3->SetVisible(false);

		pStaticHead1->SetLocation(-24,2);
		pStaticHead2->SetLocation(176,2);

		const ListOccupierSept  & __SeptList = CCountryConn::Instance()->m_ListOccupierSept;
		ConstIter_OccupierSept it;
		for( it = __SeptList.begin(); it != __SeptList.end(); ++it )
		{  
			const stOccupierSeptItem & item = *it;
			pListBox->AddItem("",NULL);
			pListBox->SetItemText(pListBox->GetCount()-1,0,item.strNpcPos.c_str(),25);
			pListBox->SetItemText(pListBox->GetCount()-1,1,item.strSeptName.c_str(),20);
		}
	}
	else if(m_Usage == Usage_KingSort)
	{
		SetSize(364,430);
		pListBox->SetSize(300,300);
		pButton->SetLocation(80,314);

		pStaticHead1->SetText("姓名");
		pStaticHead2->SetText("国家");
		pStaticHead3->SetText("在位时间");
		pStaticHead3->SetVisible(true);


		pStaticHead1->SetLocation(-24,2);
		pStaticHead2->SetLocation(76,2);
		pStaticHead3->SetLocation(180,2);

		const ListKingSort &__KingSortList = CCountryConn::Instance()->m_ListKingSort;
		ConstIter_KingSort it;
		for( it= __KingSortList.begin(); it != __KingSortList.end(); ++it)
		{
			const stKingSortList & item = *it;
			pListBox->AddItem("",NULL);
			pListBox->SetItemText(pListBox->GetCount()-1,0,item.strName.c_str(),13);
			pListBox->SetItemText(pListBox->GetCount()-1,1,item.strCountryName.c_str(),13);
			pListBox->SetItemText(pListBox->GetCount()-1,2,item.strTotalTime.c_str(),13);
		}

	}
}

bool CGuiQueryOccupierDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) 
			{
				case ID_QUERYOCCUPIER_BUTTON_OK:	
					{
						if(GetGameGuiManager()->m_guiNpcDialog)
							GetGameGuiManager()->m_guiNpcDialog->OnActionEnd();
						Close();
					} 
					break;
			}
		}
		break;

	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiQueryOccupierDlg::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	//if(m_pBtnClose)
	//{
	//	stPointI pt( m_pBtnClose->GetLocation() );
	//	/*m_pBtnClose->SetLocation(pt.x,pt.y + 3);*/
	//}

	FUNCTION_END;
}

// 查询占领者
CGuiQueryOccupierDlg *  CCountryConn::AddOccupierDlg(Occupier_Usage type)
{
	FUNCTION_BEGIN;

	if(!m_guiOccupierDlg)
	{
		CGuiQueryOccupierDlg * pDlg = new CGuiQueryOccupierDlg(type);
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\queryoccupierdlg.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiOccupierDlg = pDlg;
	}
	m_guiOccupierDlg->SetVisible(true);
	m_guiOccupierDlg->SetEnabled(true);



	if(GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->OnActionBegin();

	return m_guiOccupierDlg;

	FUNCTION_END;
}

