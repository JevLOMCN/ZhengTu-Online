#include "public.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include ".\NationalMapDialog.h"
#include ".\Task.h"
#include "GameScene.h"
#include "MainCharacter.h"

#define IDC_MAINLAND_COMBOX			1
#define IDC_SWITCH_NATIOANAL_BTN	3
#define IDC_SWITCH_WORLD_BTN		4
#define IDC_SMALL_BTN				5

#define IDC_BASE_BITMAP_BUTTON 10000

#define _ShowControl(id,bShow) {CGuiControl* pCtrl = GetControl(id);if(pCtrl){pCtrl->SetVisible(bShow);}}

#define NationalMap_Group_1024_768		6
#define NationalMap_Group_800_600		64

#define NationalMap_GetImage(id) if( GetDevice()->GetHeight() < 768 ) return stResourceLocation(pszImagePack,NationalMap_Group_800_600,id);return stResourceLocation(pszImagePack,NationalMap_Group_1024_768,id);

stResourceLocation CDistrict::GetButtonImage() const
{
	NationalMap_GetImage(iBtnImage);
}

stResourceLocation CProvince::GetBackGroundImage() const
{
	NationalMap_GetImage(iBackImage);
}
stResourceLocation CProvince::GetBorderImage() const
{
	NationalMap_GetImage(iBorderImage);
}
stResourceLocation CProvince::GetButtonImage() const
{
	NationalMap_GetImage(iBtnImage);
}
stResourceLocation CProvince::GetCoverImage() const
{
	NationalMap_GetImage(iCoverImage);
}

stResourceLocation CMainlandMap::GetBackGroundImage() const
{
	NationalMap_GetImage(iBackImage);
}
stResourceLocation CMainlandMap::GetBorderImage() const
{
	NationalMap_GetImage(iBorderImage);
}
stResourceLocation CMainlandMap::GetButtonImage() const
{
	NationalMap_GetImage(iBtnImage);
}

stResourceLocation CWorldMap::GetBackGroundImage() const
{
	NationalMap_GetImage(iBtnImage);
}
stResourceLocation CWorldMap::GetBorderImage() const
{
	NationalMap_GetImage(iBorderImage);	
}

CNationalMapDialog::CNationalMapDialog(void)
{
	//m_bCloseIsHide = true;
	m_pCurMainland = NULL;	//当前国家
	m_pCurProvince = NULL;	//当前州
	m_clientOffsetTopLeft = stPointI(0,0);
}

CNationalMapDialog::~CNationalMapDialog(void)
{
}

/**
* \brief "创建消息"响应函数
* 
* \return void
*/
void CNationalMapDialog::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	
	InitRect();

	bool bRes = m_worldMap.Init();

	if(bRes)
	{
		if ( !m_pCurMainland ) 
			m_pCurMainland = m_worldMap.GetMainlandByName(m_worldMap.GetDefaultMainland());
		if ( m_pCurMainland ) 
		{
			const char* pszDefaultPrivince = m_pCurMainland->GetDefaultProvince();
			m_pCurProvince = m_pCurMainland->GetProvinceByName(pszDefaultPrivince);
			if(m_pCurProvince) 
			{
				UpdateContents( enum_show_province );
				return ;
			}
			UpdateContents( enum_show_mainland );
			return ;
		}
		UpdateContents( enum_show_world );
		
		return ;
	}
	else
	{
		Engine_WriteLog("初始化地图信息文件 失败，世界地图无法显示\n");
		Close();
	}

	FUNCTION_END;
}


/**
* \brief GUI消息处理函数
* 
* \param nEvent : 消息句柄
* \param nID : 控件id
* \param pControl : 控件指针
* \return true if succeed else false
*/
bool CNationalMapDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:	if( OnBtnClick(nID) ) return true; break;
	case EVENT_RBUTTON_CLICKED: if( OnRButuonClick() ) return true;break;
	case EVENT_COMBOBOX_SELECTION_CHANGED:  if( OnComboBoxChange() ) return true;break;

	}


	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

bool CNationalMapDialog::OnClickMapItem(UINT id)
{
	FUNCTION_BEGIN;

	CGuiButton* pBtn = GetButton(id);
	if(pBtn)
	{
		const char* szText = pBtn->GetToopTips();
		if(!m_pCurMainland) 
			m_pCurMainland = m_worldMap.GetMainlandByName(m_worldMap.GetDefaultMainland());
		if( m_pCurMainland )
		{
			m_pCurProvince = m_pCurMainland->GetProvinceByName(szText);
			UpdateContents( enum_show_province );
			return true;
		}
	}
	return false;


	FUNCTION_END;
}

bool CNationalMapDialog::OnClickSmallBtn()
{
	FUNCTION_BEGIN;

	switch(m_curState)
	{
	case enum_show_world:
		break;
	case enum_show_mainland:
		UpdateContents( enum_show_world );
		return true;
		break;
	case enum_show_province:
		//禁止
		return true;
		UpdateContents( enum_show_mainland );
		return true;
		break;
	}

	return false;

	FUNCTION_END;
}

bool CNationalMapDialog::OnBtnClick(UINT id)
{
	FUNCTION_BEGIN;

	switch(id)
	{
	case IDC_SWITCH_NATIOANAL_BTN:	UpdateContents( enum_show_mainland );		return true;
	case IDC_SWITCH_WORLD_BTN:		UpdateContents( enum_show_world );			return true;
	case IDC_SMALL_BTN:				OnClickSmallBtn();							return true;
	case 6:
		{
			SetVisible(false);
			return true;
		}
		break;
	default:
		break;
	}

	//禁止点击地图区域按钮
	return true;

	//只有在世界地图页面才可以点
	if( m_curState == enum_show_province || 
		m_curState == enum_show_world ) return false;
	for(tListControlIt it = m_listCtrl.begin() ; it != m_listCtrl.end() ;++it)
	{
		if( (*it)->GetID() == id )
		{
			return OnClickMapItem(id);
			break;
		}
	}
	return false;

	FUNCTION_END;
}

bool CNationalMapDialog::OnRButuonClick()
{
	FUNCTION_BEGIN;

	//禁止原先右键点击地图区域按钮返回上一级地图的功能
	return true;

	switch(m_curState)
	{
		case enum_show_world:				break;
		case enum_show_mainland:	
		{
			UpdateContents(enum_show_world);
			return true;
		}
		break;
		case enum_show_province:
		{
			if(m_pCurProvince)
			{
				m_pCurMainland = m_pCurProvince->GetParentMap();
			}
			
			UpdateContents(enum_show_mainland);
			return true;
		}
		break;
	}
	return false;

	FUNCTION_END;
}

/**
* \brief 对话框关闭消息处理
* 
* 退出对话框、更新管理器对应的指针
*
* \return void
*/
void CNationalMapDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiNationalMap = NULL;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CNationalMapDialog::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x,pt.y - 90);
	}

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
HRESULT CNationalMapDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	return hr;

	FUNCTION_END;
}


void CNationalMapDialog::RemoveBitmapCtrl()
{
	FUNCTION_BEGIN;

	for(tListControlIt it = m_listCtrl.begin() ; it != m_listCtrl.end() ;++it)
	{
		//Debug_Int((*it)->GetID());
		RemoveControl( (CGuiControl *) (*it) );
	}
	m_listCtrl.clear();

	FUNCTION_END;
}

void CNationalMapDialog::AddMapImage(UINT id,stResourceLocation dst)
{
	if( dst.frame == -1 || dst.group < 0 ) return ;
	CGuiImage* pImage = NULL;
	if( FAILED( AddImage(id,0,0,dst,NULL,-1,1024,768,CGuiImage::STRETCH_IMAGE,&(pImage)) ) )
	{				
		return ;
	}
	if(pImage)
	{
		m_listCtrl.push_back(pImage);
	}
}

void CNationalMapDialog::AddMapButton(UINT id,stResourceLocation dst,const char* text,int framenum)
{
	if( dst.frame == -1 || dst.group < 0 || framenum < 0 ) return ;
	CGuiButton* pBtn = NULL;
	if(FAILED(AddButton(id,NULL,0,0,0,0,0,0,&(pBtn))))
	{				
		return ;
	}
	if(pBtn)
	{
		pBtn->SetButtonType(CGuiButton::ButtonType_Bitmap);
		pBtn->SetBitmap(dst,framenum);
		pBtn->SetToolTips( text );
		m_listCtrl.push_back(pBtn);
	}
}

inline void CNationalMapDialog::SetBackgroundImage(stResourceLocation dst)
{
	//CGuiImage* pImage = GetImage(0);
	//if(pImage)	pImage->SetImage(dst);
	SetBackImage(dst);
}

void CNationalMapDialog::ShowWorldMap()
{
	FUNCTION_BEGIN;

	m_pCurMainland = m_worldMap.GetMainlandByName(m_worldMap.GetDefaultMainland());
	int baseid = IDC_BASE_BITMAP_BUTTON;
	//显示背景
	SetBackgroundImage(m_worldMap.GetBackGroundImage());
	//AddMapImage(baseid++,m_worldMap.GetBackGroundImage());
	
	//显示边框
	AddMapImage(baseid++,m_worldMap.GetBorderImage());

	//更新固有控件的状态
	_ShowControl(IDC_SWITCH_NATIOANAL_BTN,true);
	_ShowControl(IDC_SWITCH_WORLD_BTN,false);
	_ShowControl(IDC_SMALL_BTN,false);
	
	CGuiComboBox* pCom = GetComboBox(IDC_MAINLAND_COMBOX);
	if(pCom)
	{
		pCom->RemoveAllItems();
	}
	//_ShowControl(IDC_PROVINCE_COMBOX,false);

	//显示子地图块
	CWorldMap::tListMainland* list = m_worldMap.GetMainlandList();
	if( list )
	{		
		for( CWorldMap::MainlandIt it = list->begin(); it != list->end(); ++it )
		{
			if( pCom && (*it)->bIsDo() ) pCom->AddItem((*it)->GetName(),NULL);
			AddMapButton(baseid++,(*it)->GetButtonImage(),(*it)->GetName(),(*it)->GetButtonFrameNum());
		}
	}

	FUNCTION_END;
}

void CNationalMapDialog::ShowMainlandMap()
{
	FUNCTION_BEGIN;

	CMainCharacter* pMainChar = GetScene()->GetMainCharacter();
	if(!pMainChar) return ;

	if( m_pCurMainland )
	{		
		int baseid = IDC_BASE_BITMAP_BUTTON;
		//显示背景
		SetBackgroundImage(m_pCurMainland->GetBackGroundImage());

		//显示边框
		//AddMapImage(baseid++,m_pCurMainland->GetBorderImage());

		//更新固有控件的状态
		_ShowControl(IDC_SWITCH_NATIOANAL_BTN,false);
		_ShowControl(IDC_SWITCH_WORLD_BTN,true);
		_ShowControl(IDC_SMALL_BTN,true);

		CTask* ptask = pMainChar->GetTaskList();

		CGuiComboBox* pCom = GetComboBox(IDC_MAINLAND_COMBOX);
		if(pCom)
		{
			pCom->SetVisible(true);
			pCom->RemoveAllItems();
		}	
		//显示子地图块
		CMainlandMap::tListProvince* list = m_pCurMainland->GetProvinceList();
		if( list )
		{		
			for( CMainlandMap::ProvinceIt it = list->begin(); it != list->end(); ++it )
			{
				int id = (*it)->GetTaskID();
				if(ptask && ( ptask->bIsTaskExist(id) || id == -1 ) )
				{
					if( pCom )
					{
						pCom->AddItem((*it)->GetName(),NULL);
					}
					AddMapButton(baseid++,(*it)->GetButtonImage(),(*it)->GetName(),(*it)->GetButtonFrameNum());
				}				
			}
		}	
	}


	FUNCTION_END;
}

void CNationalMapDialog::ShowProvinceMap()
{
	FUNCTION_BEGIN;

	CMainCharacter* pMainChar = GetScene()->GetMainCharacter();
	if(!pMainChar) return ;

	if( m_pCurProvince )
	{
		CTask* ptask = pMainChar->GetTaskList();
	
		int baseid = IDC_BASE_BITMAP_BUTTON;
		//显示背景
		SetBackgroundImage(m_pCurProvince->GetBackGroundImage());
	
		//显示边框
		//AddMapImage(baseid++,m_pCurProvince->GetBorderImage());

		//显示遮盖图片
		//AddMapImage(baseid++,m_pCurProvince->GetCoverImage());

		//更新固有控件的状态
		_ShowControl(IDC_SWITCH_NATIOANAL_BTN,false);
		_ShowControl(IDC_SWITCH_WORLD_BTN,false);
		_ShowControl(IDC_MAINLAND_COMBOX,false);
		_ShowControl(IDC_SMALL_BTN,true);

		//显示子地图块
		CProvince::tListDistrict* list = m_pCurProvince->GetDistrictList();
		if( list )
		{		
			for( CProvince::DistrictIt it = list->begin(); it != list->end(); ++it )
			{
				if(ptask && ptask->bIsTaskExist((*it)->GetTaskID())) continue;//have map
				AddMapButton(baseid++,(*it)->GetButtonImage(),(*it)->GetName(),(*it)->GetButtonFrameNum());
			}
		}		
	}

	FUNCTION_END;
}


void CNationalMapDialog::UpdateContents(STATE state)
{
	FUNCTION_BEGIN;

	RemoveBitmapCtrl();
	switch(state)
	{
	case enum_show_world:		ShowWorldMap();		break;
	case enum_show_mainland:	ShowMainlandMap();	break;
	case enum_show_province:	ShowProvinceMap();	break;
	}
	m_curState = state;

	FUNCTION_END;
}

bool CNationalMapDialog::OnComboBoxChange()
{
	FUNCTION_BEGIN;

	return true;
	CGuiComboBox* pCom = GetComboBox(IDC_MAINLAND_COMBOX);
	if(pCom)
	{
		int i = pCom->GetCurItem();
		const char* pszText = pCom->GetItemText(i);

		switch(m_curState)
		{
		case enum_show_world:		
			{
				m_pCurMainland = m_worldMap.GetMainlandByName(pszText);
				UpdateContents(enum_show_mainland);
				return true;
			}
			break;
		case enum_show_mainland:	
			{
				if(!m_pCurMainland) 
					m_pCurMainland = m_worldMap.GetMainlandByName(m_worldMap.GetDefaultMainland());
				if(m_pCurMainland)
				{
					m_pCurProvince = m_pCurMainland->GetProvinceByName(pszText);
					UpdateContents(enum_show_province);
					return true;
				}
			}
			break;
		case enum_show_province:break;
		}
	}	
	return false;

	FUNCTION_END;
}

/**
* \brief 重设对话框尺寸
* 
* 
* 
* \return 
*/
void CNationalMapDialog::OnResetScreenSize()
{
	FUNCTION_BEGIN;

	InitRect();
	UpdateContents(m_curState);

	FUNCTION_END;
}

/**
* \brief 
* 
* 
* 
* \return 
*/
void CNationalMapDialog::InitRect()
{
	FUNCTION_BEGIN;

	int width = GetDevice()->GetWidth();
	if( width <= 1024 )
	{
		m_x = m_y = 0;
	}
	else
	{
		m_x = (width - 1024)/ 2;
		m_y = (GetDevice()->GetHeight() - 768)/2;
	}
	if( width < 1024 )
	{
		GetButton(IDC_SWITCH_NATIOANAL_BTN)->SetBitmap( stResourceLocation(pszImagePack,64,30),3 );
		GetButton(IDC_SWITCH_WORLD_BTN)->SetBitmap( stResourceLocation(pszImagePack,64,27),3 );
		GetButton(IDC_SMALL_BTN)->SetBitmap( stResourceLocation(pszImagePack,64,33),3 );
		GetButton(6)->SetLocation(771,13);
	}
	else
	{
		GetButton(IDC_SWITCH_NATIOANAL_BTN)->SetBitmap( stResourceLocation(pszImagePack,6,30),3 );
		GetButton(IDC_SWITCH_WORLD_BTN)->SetBitmap( stResourceLocation(pszImagePack,6,27),3 );
		GetButton(IDC_SMALL_BTN)->SetBitmap( stResourceLocation(pszImagePack,6,33),3 );
		GetButton(6)->SetLocation(996,13);
	}

	FUNCTION_END;
}

void CNationalMapDialog::UpdateProvinceMap()
{
	FUNCTION_BEGIN;

	if(m_curState == enum_show_province)
	{
		UpdateContents(enum_show_province);
	}

	FUNCTION_END;
}