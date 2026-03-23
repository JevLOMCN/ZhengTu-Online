
#include "public.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include "./DlgEquiphandbook.h"
#include "../gui/include/GuiManager.h"
#include "./Game.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "SafetyUser.h"
#include "./guiuserinfo.h"
#include "./GameScene.h"
#include "./RoleItem.h"
#include "./MainCharacter.h"
#include ".\GuiCartoonAddCentDialog.h"
#include "./character.h"
#include "./CreateRole.h"
#include "./GuiWorldConn.h"
#include "./GuiItem.h"
#include "./GameGuiFloatItem.h"
#include "./Country.h"
#include "Guimain.h"
#include "../engine/include/BxmpTextout.h"
#include "./GameCursor.h"
#include "./GameState.h"

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

#define  AddShowTitle(str,total,base,unit) \
	{\
		m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));\
		char szTxt[16]={0};\
		m_ToolTips.AddText(str);\
		m_ToolTips.AddText(" = ");\
		m_ToolTips.AddText(_itoa(base,szTxt,10));\
		m_ToolTips.AddText(unit);\
		if (total!=base){\
			m_ToolTips.SetCurColor(GETSHOWCOLOR(total,base));\
			m_ToolTips.AddText(total>base?" + ":" - ");\
			m_ToolTips.AddText(_itoa(abs((int)(total-base)),szTxt,10)); \
			m_ToolTips.AddText(unit);\
			m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));}\
	}\

CDlgEquiphandbook::CDlgEquiphandbook(bool bMainUser)
:m_bMainUser(bMainUser)
,m_bEquipe(false)
,m_bClothes(false)

{
	FUNCTION_BEGIN;

    memset(&x_UserData,0,sizeof(x_UserData));

	FUNCTION_END;
}
 
CDlgEquiphandbook::~CDlgEquiphandbook()
{
	FUNCTION_BEGIN;

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
void CDlgEquiphandbook::SetCharType(int type,void* pData)
{
	FUNCTION_BEGIN;

	if (GetGameGuiManager()->m_guiUserInfo)
		GetGameGuiManager()->m_guiUserInfo->SetCharType(type);


    SetUserData(pData);

	FUNCTION_END;
}


void CDlgEquiphandbook::SetUserData(void* pData)
{
	if (m_bMainUser)
	{
		x_UserData.eMainUserData = GetScene()->GetMainCharacter()->GetProperty();
		x_UserData.eMapUserData = GetScene()->GetMainCharacter()->GetProData();
	}	

}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param fElapsedTime : 描述
 * \return 返回值的描述
 */
HRESULT CDlgEquiphandbook::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	GetButton( 10 )->SetLocation( 765, 22 ); //关闭按钮
	GetButton( 5 )->SetLocation( 133, 46 ); //百兽图鉴按钮
	GetButton( 888 )->SetLocation( 582, 46 ); //百兽图鉴助手按钮


	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);


	int iOldFont = GetDevice()->SetFont(1);

	//GetDevice()->DrawString(x_UserData.pMapUserData->name,rect,ColorBlend(GetNameColor(x_UserData.pMapUserData->goodness),0xffffffff),DT_VCENTER|DT_CENTER);

	switch(m_pTab->GetCurItem())
	{
	    case 0: //soke 装备页面
		{
		   ShowUserInfoText1();	
		   ShowEquipByName();	  
		}
		break;	
	}

	TipsRender();

	GetDevice()->SetFont(iOldFont);
	
	return hr;

	FUNCTION_END;
}


void CDlgEquiphandbook::RenderBook(float fElapsedTime)
{
	int iOldFont = GetDevice()->SetFont(1);

	CMainCharacter *pMainChar = GetScene()->GetMainCharacter();

	GetDevice()->SetFont(iOldFont);
}

/**
* \brief "创建消息"响应函数
* 
* \return void
*/
void CDlgEquiphandbook::OnCreate(void)
{
	FUNCTION_BEGIN;
	

	CGuiDialog::OnCreate();

	byte nTableType = OBJECTCELLTYPE_EQUIP;


	GetEquipTable(EQUIPCELLTYPE_TUJIAN1) = GetTable(1301);
	GetEquipTable(EQUIPCELLTYPE_TUJIAN1)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_TUJIAN1)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_TUJIAN1);

	GetEquipTable(EQUIPCELLTYPE_TUJIAN2) = GetTable(1302);
	GetEquipTable(EQUIPCELLTYPE_TUJIAN2)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_TUJIAN2)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_TUJIAN2);

	GetEquipTable(EQUIPCELLTYPE_TUJIAN3) = GetTable(1303);
	GetEquipTable(EQUIPCELLTYPE_TUJIAN3)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_TUJIAN3)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_TUJIAN3);

	GetEquipTable(EQUIPCELLTYPE_TUJIAN4) = GetTable(1304);
	GetEquipTable(EQUIPCELLTYPE_TUJIAN4)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_TUJIAN4)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_TUJIAN4);

	GetEquipTable(EQUIPCELLTYPE_TUJIAN5) = GetTable(1305);
	GetEquipTable(EQUIPCELLTYPE_TUJIAN5)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_TUJIAN5)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_TUJIAN5);

	GetEquipTable(EQUIPCELLTYPE_TUJIAN6) = GetTable(1306);
	GetEquipTable(EQUIPCELLTYPE_TUJIAN6)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_TUJIAN6)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_TUJIAN6);	

	GetEquipTable(EQUIPCELLTYPE_TUJIAN7) = GetTable(1307);
	GetEquipTable(EQUIPCELLTYPE_TUJIAN7)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_TUJIAN7)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_TUJIAN7);

	GetEquipTable(EQUIPCELLTYPE_TUJIAN8) = GetTable(1308);
	GetEquipTable(EQUIPCELLTYPE_TUJIAN8)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_TUJIAN8)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_TUJIAN8);

	GetEquipTable(EQUIPCELLTYPE_TUJIAN9) = GetTable(1309);
	GetEquipTable(EQUIPCELLTYPE_TUJIAN9)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_TUJIAN9)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_TUJIAN9);

	GetEquipTable(EQUIPCELLTYPE_TUJIAN10) = GetTable(1310);
	GetEquipTable(EQUIPCELLTYPE_TUJIAN10)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_TUJIAN10)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_TUJIAN10);
	m_ptIcon.x = 33;
	m_ptIcon.y = 90;
            	

	GetButton(5)->SetButtonGroup(1);

    //soke 如果不是观察,关闭
	if( m_bMainUser )
	{

		//GetButton(1030)->SetVisible(false);
	}
	//soke 是观察就关闭 
	else
	{
	
	//	GetButton(1300)->SetVisible(false);
	}

	m_pTab = GetTab(7);
	AssertCreateCtrl( m_pTab,"GUI配置脚本出错" );
	m_pTab->AddItem("",NULL); //0  

	
    // bright moon 百兽图鉴
	m_pTab->AddControl(0,(CGuiControl*)GetTable(1301));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(1302));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(1303));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(1304));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(1305));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(1306));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(1307));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(1308));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(1309));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(1310));
	m_pTab->AddControl(0,(CGuiControl*)GetImage(101));
	m_pTab->AddControl(0,(CGuiControl*)GetImage(102)); 
	m_pTab->AddControl(0,(CGuiControl*)GetImage(103));
	m_pTab->AddControl(0,(CGuiControl*)GetImage(104)); 
	m_pTab->AddControl(0,(CGuiControl*)GetImage(105)); 
	m_pTab->AddControl(0,(CGuiControl*)GetImage(106)); 
	m_pTab->AddControl(0,(CGuiControl*)GetImage(107)); 
	m_pTab->AddControl(0,(CGuiControl*)GetImage(108)); 
	m_pTab->AddControl(0,(CGuiControl*)GetImage(109));	
	m_pTab->AddControl(0,(CGuiControl*)GetImage(110)); 
	m_pTab->AddControl(0,(CGuiControl*)GetImage(151));
	m_pTab->AddControl(0,(CGuiControl*)GetImage(152)); 
	m_pTab->AddControl(0,(CGuiControl*)GetImage(153));
	m_pTab->AddControl(0,(CGuiControl*)GetImage(154)); 
	m_pTab->AddControl(0,(CGuiControl*)GetImage(155)); 
	m_pTab->AddControl(0,(CGuiControl*)GetImage(156)); 
	m_pTab->AddControl(0,(CGuiControl*)GetImage(157)); 
	m_pTab->AddControl(0,(CGuiControl*)GetImage(158)); 
	m_pTab->AddControl(0,(CGuiControl*)GetImage(159));	
	m_pTab->AddControl(0,(CGuiControl*)GetImage(160)); 



	m_pTab->AddControl(0,GetControl(15));
	m_pTab->AddControl(0,GetControl(16));
	m_pTab->AddControl(0,GetControl(17));
	m_pTab->AddControl(0,GetControl(18));
	m_pTab->AddControl(0,GetControl(19));
	m_pTab->AddControl(0,GetControl(25));
	m_pTab->AddControl(0,GetControl(26));
	m_pTab->AddControl(0,GetControl(27));
	m_pTab->AddControl(0,GetControl(28));
	m_pTab->AddControl(0,GetControl(29));
	m_pTab->AddControl(0,GetControl(30));


	m_pTab->SetVisible(false);

	OnGuiEvent(EVENT_BUTTON_CLICKED,5,GetButton(5));



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
CGuiTable* & CDlgEquiphandbook::GetEquipTable(int equip_type,int x )
{
	FUNCTION_BEGIN;

	return GetGameGuiManager()->m_guiUserInfo->GetEquipTable(equip_type,x);

	FUNCTION_END;
}

CRoleItem* CDlgEquiphandbook::GetEquipItem( int equip_type,int x /* = 0 */ )
{
	CRoleItem* pRoleItem = NULL;

	CGuiTable* pTable = GetEquipTable(equip_type, x);
	if (pTable)
	{
		if (pTable->m_listItem.size()>0)
		{
			pRoleItem =(CRoleItem*) pTable->m_listItem[0]->m_pItemData;
		}
	}

	return pRoleItem;
}
/**
* \brief GUI消息处理函数
* 
* \param nEvent : 消息句柄
* \param nID : 控件id
* \param pControl : 控件指针
* \return true if succeed else false
*/

bool CDlgEquiphandbook::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	
	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
			case 5:
				m_pTab->SetCurItem(0);
				GetButton(5)->SetChecked(true);
				SetBackImage(stResourceLocation("data\\interfaces2.gl",106,61));//soke 百兽图鉴界面
			break;			
			case 10:
			{
				SetVisible(false);
			}
			break;
			case 888: //图鉴助手
			{
				GetGameGuiManager()->AddDlgEquipbookRecast();
			}
			break;	
			default:	
			return true;

		}
		
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}



/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CDlgEquiphandbook::RefreshButtonState()
{
	FUNCTION_BEGIN;

	int curTab = GetTab(7)->GetCurItem();
	CMainCharacter *pMainChar = GetScene()->GetMainCharacter();
	if(! pMainChar) return ;
	switch(curTab)
	{
	case 0: //第一个页面
		break;
	}

}

void CDlgEquiphandbook::TipsRender()
{
	


}

//桃子 图鉴面板强化加成
void CDlgEquiphandbook::ShowUserInfoText1()
{
	unsigned int handbook1 = 0;	
	unsigned int handbook2 = 0; 
	unsigned int handbook3 = 0; 
	unsigned int handbook4 = 0;	
	unsigned int handbook5 = 0;	
	unsigned int handbook6 = 0;	
	unsigned int handbook7 = 0;	
	unsigned int handbook8 = 0;	
	unsigned int handbook9 = 0;	
	unsigned int handbook10 = 0;	

	unsigned int fyhandbook1 = 0;	
	unsigned int fyhandbook2 = 0; 
	unsigned int fyhandbook3 = 0; 
	unsigned int fyhandbook4 = 0;	
	unsigned int fyhandbook5 = 0;	
	unsigned int fyhandbook6 = 0;	
	unsigned int fyhandbook7 = 0;	
	unsigned int fyhandbook8 = 0;	
	unsigned int fyhandbook9 = 0;	
	unsigned int fyhandbook10 = 0;

	unsigned int handbookhp1 = 0;	
	unsigned int handbookhp2 = 0; 
	unsigned int handbookhp3 = 0; 
	unsigned int handbookhp4 = 0;	
	unsigned int handbookhp5 = 0;	
	unsigned int handbookhp6 = 0;
	unsigned int handbookhp7 = 0;
	unsigned int handbookhp8 = 0;
	unsigned int handbookhp9 = 0;
	unsigned int handbookhp10 = 0;

	CRoleItem* phandbook1 = GetEquipItem(EQUIPCELLTYPE_TUJIAN1);

    if ((phandbook1) && (phandbook1->GetObjectBase()->dwType==ItemType_handbookitem1))	
	{ 
		phandbook1 = NULL;	

		if(phandbook1 == NULL)
		{
		   handbook1 = 500;
		   fyhandbook1 = 300;
		   handbookhp1 = 2000;
		}  
    }

    CRoleItem* phandbook2 = GetEquipItem(EQUIPCELLTYPE_TUJIAN2);

    if ((phandbook2) && (phandbook2->GetObjectBase()->dwType==ItemType_handbookitem2))	
	{ 
		phandbook2 = NULL;

		if(phandbook2 == NULL)
		{
		   handbook2 = 500;
		   fyhandbook2 = 300;
		   handbookhp2 = 2000;
		}	
	}

    CRoleItem* phandbook3 = GetEquipItem(EQUIPCELLTYPE_TUJIAN3);


    if ((phandbook3) && (phandbook3->GetObjectBase()->dwType==ItemType_handbookitem3))	
	{ 
	    phandbook3 = NULL;

		if(phandbook3 == NULL)
		{
		   handbook3 = 500;
		   fyhandbook3 = 300;
		   handbookhp3 = 2000;
		}	
	}
  
  	CRoleItem* phandbook4 = GetEquipItem(EQUIPCELLTYPE_TUJIAN4);

    if ((phandbook4) && (phandbook4->GetObjectBase()->dwType==ItemType_handbookitem4))	
	{ 
		phandbook4 = NULL;

		if(phandbook4 == NULL)
		{
		   handbook4 = 500;
		   fyhandbook4 = 300;
		   handbookhp4 = 2000;
		}	
	}

	CRoleItem* phandbook5 = GetEquipItem(EQUIPCELLTYPE_TUJIAN5);

    if ((phandbook5) && (phandbook5->GetObjectBase()->dwType==ItemType_handbookitem5))	
	{ 
		phandbook5 = NULL;

		if(phandbook5 == NULL)
		{
		   handbook5 = 500;
		   fyhandbook5 = 300;
		   handbookhp5 = 2000;
		}
	}

	CRoleItem* phandbook6 = GetEquipItem(EQUIPCELLTYPE_TUJIAN6);
  
    if ((phandbook6) && (phandbook6->GetObjectBase()->dwType==ItemType_handbookitem6))	
	{ 
		phandbook6 = NULL;

		if(phandbook6 == NULL) 
		{
		   handbook6 = 500;
		   fyhandbook6 = 300;
		   handbookhp6 = 2000;
		}	 
	}

	CRoleItem* phandbook7 = GetEquipItem(EQUIPCELLTYPE_TUJIAN7);
  
    if ((phandbook7) && (phandbook7->GetObjectBase()->dwType==ItemType_handbookitem7))	
	{ 
		phandbook7 = NULL;

		if(phandbook7 == NULL) 
		{
		   handbook7 = 500;
		   fyhandbook7 = 300;
		   handbookhp7 = 2000;
		}	 
	}

	CRoleItem* phandbook8 = GetEquipItem(EQUIPCELLTYPE_TUJIAN8);
  
    if ((phandbook8) && (phandbook8->GetObjectBase()->dwType==ItemType_handbookitem8))	
	{ 
		phandbook8 = NULL;

		if(phandbook8 == NULL) 
		{
		   handbook8 = 500;
		   fyhandbook8 = 300;
		   handbookhp8 = 2000;
		}	 
	}

	CRoleItem* phandbook9 = GetEquipItem(EQUIPCELLTYPE_TUJIAN9);
  
    if ((phandbook9) && (phandbook9->GetObjectBase()->dwType==ItemType_handbookitem9))	
	{ 
		phandbook9 = NULL;

		if(phandbook9 == NULL) 
		{
		   handbook9 = 500;
		   fyhandbook9 = 300;
		   handbookhp9 = 2000;
		}	 
	}

	CRoleItem* phandbook10 = GetEquipItem(EQUIPCELLTYPE_TUJIAN10);
  
    if ((phandbook10) && (phandbook10->GetObjectBase()->dwType==ItemType_handbookitem10))	
	{ 
		phandbook10 = NULL;

		if(phandbook10 == NULL) 
		{
		   handbook10 = 500;
		   fyhandbook10 = 300;
		   handbookhp10 = 2000;
		}	 
	}
   
	unsigned int itemPkvalue = handbook1 + handbook2 + handbook3 + handbook4 + handbook5 + handbook6 + handbook7 + handbook8 + handbook9 + handbook10;

    unsigned int fyitemPkvalue = fyhandbook1 + fyhandbook2 + fyhandbook3 + fyhandbook4 + fyhandbook5 + fyhandbook6 + fyhandbook7 + fyhandbook8 + fyhandbook9 + fyhandbook10;

	unsigned int itemHpvalue = handbookhp1 + handbookhp2 + handbookhp3 + handbookhp4 + handbookhp5 + handbookhp6 + handbookhp7 + handbookhp8 + handbookhp9 + handbookhp10;

	GetDevice()->SetFont(0); //字体大小
	stPointI ptText ;

    ptText = stPointI(200,420) + GetLocation();
    GetDevice()->DrawString(avar("+ %u",itemPkvalue + GetScene()->GetMainCharacter()->GetProperty()->m13axpdamage),ptText,0xFF00FF00);//图鉴增加物攻

	ptText = stPointI(200,450) + GetLocation();
    GetDevice()->DrawString(avar("+ %u",fyitemPkvalue + GetScene()->GetMainCharacter()->GetProperty()->p13defence),ptText,0xFF00FF00);	//图鉴增加物防

	ptText = stPointI(200,480) + GetLocation();
    GetDevice()->DrawString(avar("+ %u",itemHpvalue + GetScene()->GetMainCharacter()->GetProperty()->m13axhp),ptText,0xFF00FF00);	//图鉴增加血量

	ptText = stPointI(308,420) + GetLocation();
    GetDevice()->DrawString(avar("+ %u",itemPkvalue + GetScene()->GetMainCharacter()->GetProperty()->m13axmdamage),ptText,0xFF00FF00);	//图鉴增加魔攻

	ptText = stPointI(308,450) + GetLocation();
    GetDevice()->DrawString(avar("+ %u",fyitemPkvalue + GetScene()->GetMainCharacter()->GetProperty()->m13defence),ptText,0xFF00FF00);	//图鉴增加魔防

}


void CDlgEquiphandbook::RefreshActiveItem()
{

	CRoleItem*  pItem = NULL;

	m_listActiveItem.clear();

	m_strSetMakerName[0] = 0L;
	
	stObjectLocation location;

	location.dwLocation = OBJECTCELLTYPE_EQUIP;
	location.dwTableID = 0;
	location.x = 0;

	location.y = EQUIPCELLTYPE_TUJIAN1;
 	//图鉴1
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		GetGameGuiManager()->m_guiUserInfo->SetEquipByName(pItem);
	
	}

	location.y = EQUIPCELLTYPE_TUJIAN2;
 	//图鉴2
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		GetGameGuiManager()->m_guiUserInfo->SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_TUJIAN3;
 	//图鉴3
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		GetGameGuiManager()->m_guiUserInfo->SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_TUJIAN4;
 	//图鉴4
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		GetGameGuiManager()->m_guiUserInfo->SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_TUJIAN5;
 	//图鉴5
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		GetGameGuiManager()->m_guiUserInfo->SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_TUJIAN6;
 	//图鉴6
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		GetGameGuiManager()->m_guiUserInfo->SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_TUJIAN7;
 	//图鉴7
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		GetGameGuiManager()->m_guiUserInfo->SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_TUJIAN8;
 	//图鉴8
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		GetGameGuiManager()->m_guiUserInfo->SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_TUJIAN9;
 	//图鉴9
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		GetGameGuiManager()->m_guiUserInfo->SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_TUJIAN10;
 	//图鉴10
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		GetGameGuiManager()->m_guiUserInfo->SetEquipByName(pItem);
	}
	
}

void CDlgEquiphandbook::InsertActiveItem(CRoleItem* pItem)
{
	if (IsActiveItem(pItem)==false)
	{
		m_listActiveItem.push_back(pItem);
	}
}

bool CDlgEquiphandbook::IsActiveItem(CRoleItem* pItem)
{
	for (int i=0;i<m_listActiveItem.size();i++)
	{
		if ((m_listActiveItem[i]==pItem)&&IsShowActiveItem(pItem))
			return true;
	}

	return false;
}

void CDlgEquiphandbook::SetEquipByName( CRoleItem* pItem, bool bMainCharacter /* = true */ )
{
	if (pItem==NULL)
		return;

	 CGuiFloatItem* pFloatItem = GetGuiManager()->GetFloatItem();
	 


}

#define Tips_AddNum(szFormat,nNum)\
	if(nNum){\
	_snprintf(szTemp,sizeof(szTemp),szFormat,nNum);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText(szTemp,0);\
	}\

#define Tips_AddString(szText) \
	if( (szText)[0] ) {\
	\
	_snprintf(szTemp,sizeof(szTemp),"%s",szText);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText(szTemp,0);\
	}\

void CDlgEquiphandbook::ShowEquipByName()
{
	IBitmaps* pBmp=NULL;

}


char* CDlgEquiphandbook::GetSetMakerName()
{
	if ( strlen( m_strSetMakerName ) <= 0 )
		return NULL;

	return m_strSetMakerName;
}




/**
* \brief 对话框关闭消息处理
* 
* 退出对话框、更新管理器对应的指针
*
* \return void
*/
void CDlgEquiphandbook::OnClose(void)
{
	FUNCTION_BEGIN;
	
	GetGameGuiManager()->m_pDlgEquiphandbook = NULL;

	FUNCTION_END;
}
