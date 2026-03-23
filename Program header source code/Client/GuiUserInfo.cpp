/**
*\file		GuiUserInfo.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    人物属性界面对话框
*
* 
*
*/
 

#include "public.h"
#include "./GameGuiManager.h"
#include "./guiuserinfo.h"
#include "./GameScene.h"
#include "./character.h"
#include "./MainCharacter.h"
#include "./CreateRole.h"
#include "./GuiWorldConn.h"
#include "./GuiItem.h"
#include "./RoleItem.h"
#include "./GameGuiFloatItem.h"
#include "./Country.h"
#include "Guimain.h"
#include "../engine/include/BxmpTextout.h"
#include "./DlgEquipRecast.h"//装备助手
#include "./DlgEquipAnnexpack.h" //附件助手
#include "./GuiPetDialog.h"
#include "./GameCursor.h"
#include "./GameState.h"
#include "./DlgEquiphandbook.h"  //soke 百兽图鉴
#include "./DlgEquipbookRecast.h" //soke 百兽图鉴助手
#include "./DlgEquipFabaoRecast.h" //soke 法宝助手
#include "./GuiTouxian.h" //soke 头衔
#include "./GuiChenghao.h" //soke 称号
#include "./GuiBieshu.h" //soke 云天别墅
#include "./GuiMagicBoxDlg.h" //时装魔盒
#include "Game.h"
#include "./GuiSkill.h"

extern DWORD dwState[];
#define IDC_ADDPOINT_BASE 200
#define IDC_FIVEPOINT_BASE 300
#define IDC_FIVEPOINT_RECOVERY 400


#define UserInfo_SetStaticText(id,txt)			{CGuiStatic *p##id = GetStatic(id);if(p##id) p##id->SetText(txt);}
#define UserInfo_SetHorseText(id,txt,d)			{sprintf(szText,"%s%u",txt,pData->d);UserInfo_SetStaticText(id,szText);}
#define UserInfo_SetHorseTextPercent(id,txt,d)	{sprintf(szText,"%s%u%%",txt,pData->d);UserInfo_SetStaticText(id,szText);}
#define UserInfo_SetHorseExt(txt,mem)			{if( pData->mem > 0 )	{UserInfo_SetHorseText(nID,txt,mem); nID++;}}
#define UserInfo_SetHorseExtPercent(txt,mem)	{if( pData->mem > 0 )	{UserInfo_SetHorseTextPercent(nID,txt,mem); nID++;}}
#define UserInfo_SetHorseImage(id,img)			{CGuiImage* p = GetImage(id);if(p) p->SetImage(stResourceLocation("data\\icons.gl",5,img));}


#define IsKeyPressed(key) (GetAsyncKeyState(key) & 0x8000)
struct stPropTxt 
{
	byte  nButtonId;
	byte  nProp;
	char  szTxt[64];
};

const stPropTxt  g_PropTxt[]=
{
	{1,PROPERTY_CON,"增加力量可提高角色的物理攻击力,同时提高物理防御力和生命值上限"},
	{3,PROPERTY_STR,"增加智力可提高角色的魔法攻击力,同时提高魔法防御力和法术值上限"},
	{2,PROPERTY_DEX,"增加敏捷可提高角色的物理防御力,同时提高魔法防御力"},
	{4,PROPERTY_INT,"增加精神可提高角色的法术值上限,同时提高魔法防御力"},
	{0,PROPERTY_MEN,"增加体质可提高角色的生命值上限,同时提高物理防御力和魔法防御力"}
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param five : 描述
 * \return 返回值的描述
 */
const char* GetPropTxt(byte nButtonId)
{
	FUNCTION_BEGIN;
	static int size=sizeof(g_PropTxt)/sizeof(stPropTxt);


	for (int i=0;i<size;i++)
	{
		if (nButtonId==g_PropTxt[i].nButtonId)
			return g_PropTxt[i].szTxt;
	}
	
	return NULL;

	FUNCTION_END;
}



/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CGuiUserInfo::CGuiUserInfo(bool bMainUser)
:m_bMainUser(bMainUser)
,m_bShowBody(true)
,m_bEquipe(false)
,m_bClothes(false)
,m_LevelNum(0)
,m_equipByMakerType(EquipByMaker_T_NONE)
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;
	memset(&m_OneKeyEquip, 0, sizeof(m_OneKeyEquip));
	memset(&m_UserData,0,sizeof(m_UserData));

	m_vEquipByMaker.clear();

	m_listActiveItem.clear();
	
	m_ActiveIcon = NULL;
	
	FUNCTION_END;
}

void CGuiUserInfo::SetUserData(void* pData)
{
	if (m_bMainUser)
	{
		m_UserData.pMainUserData = GetScene()->GetMainCharacter()->GetProperty();
		m_UserData.pMapUserData = GetScene()->GetMainCharacter()->GetProData();
		m_UserData.pMapUserData->dwBodyColorSystem = GetScene()->GetMainCharacter()->GetBodySystemColor();

		/*stLevelDegreeDataUserCmd cmd;
		SEND_USER_CMD(cmd);*/
	}
	else
	{
		m_bEquipe = false;
		m_bClothes = false;
		m_equipByMakerType = EquipByMaker_T_NONE;
		m_listActiveItem.clear();

		//Clear EquipByMaker list
		for( int i=0;i<m_vEquipByMaker.size();i++ )
		{
			m_vEquipByMaker[i].Init();
		}
		m_vEquipByMaker.clear();

		//Init set maker name
		m_strSetMakerName[0] = 0L;

		m_UserData.pMainUserData = &(m_UserData.tMainUserData);
		m_UserData.pMapUserData = &(m_UserData.tMapUserData);
		if (pData==NULL)
			return;
		
		stSelectReturnMainUserPropertyUserCmd*  pCmd = (stSelectReturnMainUserPropertyUserCmd*) pData;
		memcpy(&(m_UserData.tMainUserData),&(pCmd->mainuser_data),sizeof(m_UserData.tMainUserData));
		memcpy(&(m_UserData.tMapUserData),&(pCmd->mapuser_data),sizeof(m_UserData.tMapUserData));

		//Modify exploit value from server
		//Scale 0.01
		m_UserData.tMapUserData.exploit = pCmd->mapuser_data.exploit / 100;

		ObjectBase_t* pObject = GetObjectBase(m_UserData.tMapUserData.sculpt.dwBodyID);
		if(pObject) m_UserData.tMapUserData.dwBodyColorSystem = pObject->dwColor;

		for (int i=0;i<m_listItem.size();i++)
		{
			m_listItem[i]->OnRemove();
			m_allocRoleItem.freePointer(m_listItem[i]);
		}
		m_listItem.clear();

		SetUserImage(EQUIPCELLTYPE_BODY);
		SetUserImage(EQUIPCELLTYPE_HANDR);
		SetUserImage(EQUIPCELLTYPE_HANDL);


		for (int i=0;i<pCmd->dwSize;i++)
		{
			CRoleItem* pItem = LOOKASIDE_ALLOC(m_allocRoleItem);
			stAddObjectPropertyUserCmd  cmd;
			memcpy(&(cmd.object),&(pCmd->object_data[i].object),sizeof(t_Object));

			cmd.object.pos.dwLocation = OBJECTCELLTYPE_EQUIPSHOW;
			//cmd.object.pos.x = 0;
			//cmd.object.pos.y = pCmd->object_data[i].byWhere;

			if( !pItem->SetAttrib(&cmd) )
			{
				m_allocRoleItem.freePointer(pItem);
				return;
			}
			m_listItem.push_back(pItem);
			
			//Refresh player equipment type
			SetEquipByName( pItem, false );
		}

		//Refresh play item list for five-type-link
		if ( !m_listItem.empty() )
		{
			for( std::vector<CRoleItem*>::iterator itor=m_listItem.begin();itor!=m_listItem.end();++itor )
			{
				ActiveItem( (*itor), false );
			}
		}
	}
	InitIcon();

	//请求云天别墅信息
	stBieShuInfoCmd cmd; //sky 请求指令
	SEND_USER_CMD(cmd);

	//请求时装魔盒信息
	CMainCharacter *pMainRole = (CMainCharacter *)GetScene()->GetMainCharacter();
	stGetShizhuangPropertyUserCmd cmd2;
	if (pMainRole)
		cmd2.dwOldTempID = pMainRole->GetProperty()->userid;
	else
		cmd2.dwOldTempID = 0;
	SEND_USER_CMD(cmd2);
}
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiUserInfo::OnClose(void)
{
	FUNCTION_BEGIN;

	for (int i=0;i<m_listItem.size();i++)
	{
		m_listItem[i]->OnRemove();
		m_allocRoleItem.freePointer(m_listItem[i]);
	}

	if (m_bMainUser)
		GetGameGuiManager()->m_guiUserInfo = NULL;
	else
		GetGameGuiManager()->m_guiUserInfoShow = NULL;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiUserInfo::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();


	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x,pt.y+4);
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
void CGuiUserInfo::InitIcon()
{
	FUNCTION_BEGIN;

	if (m_UserData.pMapUserData==NULL)
		return;

	stResourceLocation rl(GetGuiGraphicPackName(),33,m_UserData.pMapUserData->face);
	SetIcon(&rl);

	m_ptIcon.x = 33;
	m_ptIcon.y = 90;

	FUNCTION_END;
}

void CGuiUserInfo::ControlImage(byte pos)
{
//纸娃娃
	CRoleItem* pRoleItem = NULL;
	CGuiImage* pImage = NULL;


	switch(pos) 
	{
	case EQUIPCELLTYPE_FASHION:
	case EQUIPCELLTYPE_BODY:
		{

			pRoleItem = GetShowClothes();
			pImage = m_pImage[0];
			if (pRoleItem&&(pRoleItem->GetLocation().y==EQUIPCELLTYPE_FASHION)&&(pRoleItem->GetCellCount()>1))
			{
				CRoleItem* pEquipItem = GetEquipItem(EQUIPCELLTYPE_BODY);
				if (pEquipItem)
					pRoleItem = pEquipItem;
			}
			if (pRoleItem)
				SetImage(pImage,pos,pRoleItem->GetObject());
		}
		break;
	case EQUIPCELLTYPE_HANDL:
		pRoleItem = GetEquipItem(EQUIPCELLTYPE_HANDL);
		pImage = m_pImage[1];
		break;
	case EQUIPCELLTYPE_HANDR:
		pRoleItem = GetEquipItem(EQUIPCELLTYPE_HANDR);
		pImage = m_pImage[2];
		break;
	case EQUIPCELLTYPE_PFAMULET:
		pRoleItem = GetEquipItem(EQUIPCELLTYPE_PFAMULET);
		pImage = m_pImage[3];
		break;
	case EQUIPCELLTYPE_CBAMULET:
		pRoleItem = GetEquipItem(EQUIPCELLTYPE_CBAMULET);
		pImage = m_pImage[4];
		break;
	}

	if ((pImage!=NULL)&&(pRoleItem!=NULL))
	{
		if (m_pTab->GetCurItem()!=0)
		{
			pImage->SetVisible(false);
			return;
		}
		else
			pImage->SetVisible(true);
			m_pImage[1]->SetVisible(false); //修复指定时装id C界面不显示武器与盾牌【西游时装专用】
			m_pImage[2]->SetVisible(false); //修复指定时装id C界面不显示武器与盾牌【西游时装专用】
			m_pImage[3]->SetVisible(true); 
			m_pImage[4]->SetVisible(true); 
		if (pRoleItem->GetCellCount()>1)
		{
			pImage->SetVisible(false);
			if ((pos==EQUIPCELLTYPE_BODY)||(pos==EQUIPCELLTYPE_FASHION))
				m_bShowBody = true;
		}
	}

}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiUserInfo::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	byte nTableType = m_bMainUser?OBJECTCELLTYPE_EQUIP:OBJECTCELLTYPE_EQUIPSHOW;


	//头盔
	GetEquipTable(EQUIPCELLTYPE_HELM) = GetTable(100);
	GetEquipTable(EQUIPCELLTYPE_HELM)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_HELM)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_HELM);

	//项链
	GetEquipTable(EQUIPCELLTYPE_NECKLACE) = GetTable(101);
	GetEquipTable(EQUIPCELLTYPE_NECKLACE)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_NECKLACE)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_NECKLACE);

	//护腕 R
	GetEquipTable(EQUIPCELLTYPE_GLOVES,EQUIPCELLTYPE_RIGHT) = GetTable(102);
	GetEquipTable(EQUIPCELLTYPE_GLOVES,EQUIPCELLTYPE_RIGHT)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_GLOVES,EQUIPCELLTYPE_RIGHT)->m_EquipPosition = stPointI(EQUIPCELLTYPE_RIGHT,EQUIPCELLTYPE_GLOVES);
	//护腕 L
	GetEquipTable(EQUIPCELLTYPE_GLOVES,EQUIPCELLTYPE_LEFT) = GetTable(103);
	GetEquipTable(EQUIPCELLTYPE_GLOVES,EQUIPCELLTYPE_LEFT)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_GLOVES,EQUIPCELLTYPE_LEFT)->m_EquipPosition = stPointI(EQUIPCELLTYPE_LEFT,EQUIPCELLTYPE_GLOVES);
	//戒指 R
	GetEquipTable(EQUIPCELLTYPE_RING,EQUIPCELLTYPE_RIGHT) = GetTable(104);
	GetEquipTable(EQUIPCELLTYPE_RING,EQUIPCELLTYPE_RIGHT)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_RING,EQUIPCELLTYPE_RIGHT)->m_EquipPosition = stPointI(EQUIPCELLTYPE_RIGHT,EQUIPCELLTYPE_RING);
	//戒指 L
	GetEquipTable(EQUIPCELLTYPE_RING,EQUIPCELLTYPE_LEFT) = GetTable(105);
	GetEquipTable(EQUIPCELLTYPE_RING,EQUIPCELLTYPE_LEFT)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_RING,EQUIPCELLTYPE_LEFT)->m_EquipPosition = stPointI(EQUIPCELLTYPE_LEFT,EQUIPCELLTYPE_RING);
	//鞋子
	GetEquipTable(EQUIPCELLTYPE_SHOES) = GetTable(106);
	GetEquipTable(EQUIPCELLTYPE_SHOES)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SHOES)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SHOES);
	//腰带
	GetEquipTable(EQUIPCELLTYPE_BELT) = GetTable(107);
	GetEquipTable(EQUIPCELLTYPE_BELT)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_BELT)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_BELT);
	//衣服
	GetEquipTable(EQUIPCELLTYPE_BODY) = GetTable(108);
	GetEquipTable(EQUIPCELLTYPE_BODY)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_BODY)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_BODY);
	//武器 右
	GetEquipTable(EQUIPCELLTYPE_HANDR) = GetTable(109);
	GetEquipTable(EQUIPCELLTYPE_HANDR)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_HANDR)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_HANDR);
	// 武器 左
	GetEquipTable(EQUIPCELLTYPE_HANDL) = GetTable(110);
	GetEquipTable(EQUIPCELLTYPE_HANDL)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_HANDL)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_HANDL);
	//饰品R
	GetEquipTable(EQUIPCELLTYPE_ADORN,EQUIPCELLTYPE_LEFT) = GetTable(111);
	GetEquipTable(EQUIPCELLTYPE_ADORN,EQUIPCELLTYPE_LEFT)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_ADORN,EQUIPCELLTYPE_LEFT)->m_EquipPosition = stPointI(EQUIPCELLTYPE_LEFT,EQUIPCELLTYPE_ADORN);

	//饰品L
	GetEquipTable(EQUIPCELLTYPE_ADORN,EQUIPCELLTYPE_RIGHT) = GetTable(112);
	GetEquipTable(EQUIPCELLTYPE_ADORN,EQUIPCELLTYPE_RIGHT)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_ADORN,EQUIPCELLTYPE_RIGHT)->m_EquipPosition = stPointI(EQUIPCELLTYPE_RIGHT,EQUIPCELLTYPE_ADORN);
	
	//soke 新时装
	GetEquipTable(EQUIPCELLTYPE_FASHION) = GetTable(113);
	GetEquipTable(EQUIPCELLTYPE_FASHION)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_FASHION)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_FASHION);

	//soke 护身符
	GetEquipTable(EQUIPCELLTYPE_AMULET) = GetTable(114);
	GetEquipTable(EQUIPCELLTYPE_AMULET)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_AMULET)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_AMULET);

 	//soke 勋章
	GetEquipTable(EQUIPCELLTYPE_XZAMULET) = GetTable(115);
	GetEquipTable(EQUIPCELLTYPE_XZAMULET)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_XZAMULET)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_XZAMULET);

	//soke 翅膀
	GetEquipTable(EQUIPCELLTYPE_CBAMULET) = GetTable(116);
	GetEquipTable(EQUIPCELLTYPE_CBAMULET)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_CBAMULET)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_CBAMULET);

	//soke 神兵
	GetEquipTable(EQUIPCELLTYPE_SBAMULET) = GetTable(117);
	GetEquipTable(EQUIPCELLTYPE_SBAMULET)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SBAMULET)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SBAMULET);

	//soke 神兽
	GetEquipTable(EQUIPCELLTYPE_SSAMULET) = GetTable(118);
	GetEquipTable(EQUIPCELLTYPE_SSAMULET)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SSAMULET)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SSAMULET);

	//上方宝剑
	GetEquipTable(EQUIPCELLTYPE_SHANGFANGBAOJIAN) = GetTable(119);
	GetEquipTable(EQUIPCELLTYPE_SHANGFANGBAOJIAN)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SHANGFANGBAOJIAN)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SHANGFANGBAOJIAN);

	//紫金龙袍
	GetEquipTable(EQUIPCELLTYPE_ZIJINLONGPAO) = GetTable(120);
	GetEquipTable(EQUIPCELLTYPE_ZIJINLONGPAO)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_ZIJINLONGPAO)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_ZIJINLONGPAO);

	//披风
	GetEquipTable(EQUIPCELLTYPE_PFAMULET) = GetTable(121);
	GetEquipTable(EQUIPCELLTYPE_PFAMULET)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_PFAMULET)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_PFAMULET);
 	
	//法宝
	//soke 法宝
	GetEquipTable(EQUIPCELLTYPE_FABAO) = GetTable(1508);
	GetEquipTable(EQUIPCELLTYPE_FABAO)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_FABAO)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_FABAO);
	//soke 法宝附件1
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ1) = GetTable(1520);
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ1)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ1)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_FABAOFJ1);
	//soke 法宝附件2
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ2) = GetTable(1521);
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ2)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ2)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_FABAOFJ2);
	//soke 法宝附件3
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ3) = GetTable(1522);
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ3)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ3)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_FABAOFJ3);
	//soke 法宝附件4
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ4) = GetTable(1523);
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ4)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ4)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_FABAOFJ4);
	//soke 法宝附件5
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ5) = GetTable(1524);
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ5)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ5)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_FABAOFJ5);
	//soke 法宝附件6
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ6) = GetTable(1525);
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ6)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ6)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_FABAOFJ6);
	//soke 法宝附件7
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ7) = GetTable(1600);
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ7)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ7)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_FABAOFJ7);
	//soke 法宝附件8
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ8) = GetTable(1601);
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ8)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ8)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_FABAOFJ8);
	//soke 法宝附件9
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ9) = GetTable(1602);
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ9)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ9)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_FABAOFJ9);
	//soke 法宝附件10
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ10) = GetTable(1603);
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ10)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ10)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_FABAOFJ10);
	//soke 法宝附件11
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ11) = GetTable(1604);
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ11)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ11)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_FABAOFJ11);
	//soke 法宝附件12
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ12) = GetTable(1605);
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ12)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_FABAOFJ12)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_FABAOFJ12);


	
	//soke 十二星宿附件1
	GetEquipTable(EQUIPCELLTYPE_SEXX1) = GetTable(2000);
	GetEquipTable(EQUIPCELLTYPE_SEXX1)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SEXX1)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SEXX1);
	//soke 十二星宿附件2
	GetEquipTable(EQUIPCELLTYPE_SEXX2) = GetTable(2001);
	GetEquipTable(EQUIPCELLTYPE_SEXX2)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SEXX2)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SEXX2);
	//soke 十二星宿附件3
	GetEquipTable(EQUIPCELLTYPE_SEXX3) = GetTable(2002);
	GetEquipTable(EQUIPCELLTYPE_SEXX3)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SEXX3)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SEXX3);
	//soke 十二星宿附件4
	GetEquipTable(EQUIPCELLTYPE_SEXX4) = GetTable(2003);
	GetEquipTable(EQUIPCELLTYPE_SEXX4)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SEXX4)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SEXX4);
	//soke 十二星宿附件5
	GetEquipTable(EQUIPCELLTYPE_SEXX5) = GetTable(2004);
	GetEquipTable(EQUIPCELLTYPE_SEXX5)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SEXX5)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SEXX5);
	//soke 十二星宿附件6
	GetEquipTable(EQUIPCELLTYPE_SEXX6) = GetTable(2005);
	GetEquipTable(EQUIPCELLTYPE_SEXX6)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SEXX6)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SEXX6);
	//soke 十二星宿附件7
	GetEquipTable(EQUIPCELLTYPE_SEXX7) = GetTable(2006);
	GetEquipTable(EQUIPCELLTYPE_SEXX7)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SEXX7)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SEXX7);
	//soke 十二星宿附件8
	GetEquipTable(EQUIPCELLTYPE_SEXX8) = GetTable(2007);
	GetEquipTable(EQUIPCELLTYPE_SEXX8)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SEXX8)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SEXX8);
	//soke 十二星宿附件9
	GetEquipTable(EQUIPCELLTYPE_SEXX9) = GetTable(2008);
	GetEquipTable(EQUIPCELLTYPE_SEXX9)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SEXX9)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SEXX9);
	//soke 十二星宿附件10
	GetEquipTable(EQUIPCELLTYPE_SEXX10) = GetTable(2009);
	GetEquipTable(EQUIPCELLTYPE_SEXX10)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SEXX10)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SEXX10);
	//soke 十二星宿附件11
	GetEquipTable(EQUIPCELLTYPE_SEXX11) = GetTable(2010);
	GetEquipTable(EQUIPCELLTYPE_SEXX11)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SEXX11)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SEXX11);
	//soke 十二星宿附件12
	GetEquipTable(EQUIPCELLTYPE_SEXX12) = GetTable(2011);
	GetEquipTable(EQUIPCELLTYPE_SEXX12)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_SEXX12)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_SEXX12);

	//soke 元神
	GetEquipTable(EQUIPCELLTYPE_YUANSHEN) = GetTable(1201);
	GetEquipTable(EQUIPCELLTYPE_YUANSHEN)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_YUANSHEN)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_YUANSHEN);
	//soke 元神附件1
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ1) = GetTable(1202);
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ1)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ1)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_YUANSHENFJ1);
	//soke 元神附件2
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ2) = GetTable(1203);
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ2)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ2)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_YUANSHENFJ2);
	//soke 元神附件3
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ3) = GetTable(1204);
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ3)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ3)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_YUANSHENFJ3);
	//soke 元神附件4
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ4) = GetTable(1205);
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ4)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ4)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_YUANSHENFJ4);
	//soke 元神附件5
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ5) = GetTable(1206);
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ5)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ5)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_YUANSHENFJ5);
	//soke 元神附件6
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ6) = GetTable(1207);
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ6)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_YUANSHENFJ6)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_YUANSHENFJ6);


	//soke 马脚
	GetEquipTable(EQUIPCELLTYPE_HORSESHOE) = GetTable(215);
	GetEquipTable(EQUIPCELLTYPE_HORSESHOE)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_HORSESHOE)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_HORSESHOE);

	//soke 马绳
	GetEquipTable(EQUIPCELLTYPE_HORSEROPE) = GetTable(216);
	GetEquipTable(EQUIPCELLTYPE_HORSEROPE)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_HORSEROPE)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_HORSEROPE);

	//soke 马鞍
	GetEquipTable(EQUIPCELLTYPE_HORSESADDLE) = GetTable(217);
	GetEquipTable(EQUIPCELLTYPE_HORSESADDLE)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_HORSESADDLE)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_HORSESADDLE);

	//soke 马甲
	GetEquipTable(EQUIPCELLTYPE_HORSESAFE) = GetTable(218);
	GetEquipTable(EQUIPCELLTYPE_HORSESAFE)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_HORSESAFE)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_HORSESAFE);

	//soke 马镫
	GetEquipTable(EQUIPCELLTYPE_HORSEIRON) = GetTable(219);
	GetEquipTable(EQUIPCELLTYPE_HORSEIRON)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_HORSEIRON)->m_EquipPosition = stPointI(0,EQUIPCELLTYPE_HORSEIRON);

	//soke 马牌
	GetEquipTable(EQUIPCELLTYPE_HORSEFASHION) = GetTable(220);
	GetEquipTable(EQUIPCELLTYPE_HORSEFASHION)->m_iTableType = nTableType;
	GetEquipTable(EQUIPCELLTYPE_HORSEFASHION)->m_EquipPosition = stPointI(2,EQUIPCELLTYPE_HORSEFASHION);	
	
	m_pImage[0]=GetImage(2);
	m_pImage[1]=GetImage(3);
	m_pImage[2]=GetImage(4);
	m_pImage[3]=GetImage(5);
	m_pImage[4]=GetImage(6);

	GetButton(5)->SetButtonGroup(1);
	GetButton(6)->SetButtonGroup(1);	
	GetButton(8)->SetButtonGroup(1);
	GetButton(9)->SetButtonGroup(1);
	GetButton(1023)->SetButtonGroup(1);

    //soke 如果不是观察,关闭坐骑显示
	if( m_bMainUser )
	{
		GetButton(8)->SetVisible(false);
	}
	//soke 是观察就关闭 属性和一键换装
	else
	{
		GetButton(6)->SetVisible(false);
		GetButton(9)->SetVisible(false);
	}

	m_pTab = GetTab(7);
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);	
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);
	
	
	//13件装备
	m_pTab->AddControl(0,(CGuiControl*)GetTable(100));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(101));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(102));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(103));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(104));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(105));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(106));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(107));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(108));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(109));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(110));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(111));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(112));
	m_pTab->AddControl(0,(CGuiControl*)GetTable(113)); //soke 时装
	m_pTab->AddControl(0,(CGuiControl*)GetTable(114)); //soke 护身符
	m_pTab->AddControl(0,(CGuiControl*)GetTable(115)); //soke 勋章
	m_pTab->AddControl(0,(CGuiControl*)GetTable(116)); //soke 翅膀
	m_pTab->AddControl(0,(CGuiControl*)GetTable(117)); //soke 神兵
	m_pTab->AddControl(0,(CGuiControl*)GetTable(118)); //soke 神兽
	m_pTab->AddControl(0,(CGuiControl*)GetTable(119)); //soke 上方宝剑
	m_pTab->AddControl(0,(CGuiControl*)GetTable(120)); //soke 紫金龙袍
	m_pTab->AddControl(0,(CGuiControl*)GetTable(121)); //soke 披风
	m_pTab->AddControl(0,(CGuiControl*)GetImage(2));
	m_pTab->AddControl(0,(CGuiControl*)GetImage(3));
	m_pTab->AddControl(0,(CGuiControl*)GetImage(4));
	m_pTab->AddControl(0,(CGuiControl*)GetImage(5));
	m_pTab->AddControl(0,(CGuiControl*)GetImage(6));
	m_pTab->AddControl(0,(CGuiControl*)GetButton(2500));//元神
	m_pTab->AddControl(0,(CGuiControl*)GetButton(2502));//星宿
	m_pTab->AddControl(0,(CGuiControl*)GetButton(2501));//五附件
	 //soke 如果不是观察,
	if( m_bMainUser )
	{
		
	m_pTab->AddControl(0,(CGuiControl*)GetButton(126)); //装备助手	
	m_pTab->AddControl(0,(CGuiControl*)GetButton(1800)); //附件助手	
	// m_pTab->AddControl(0,(CGuiControl*)GetButton(1801)); //儿女
  	m_pTab->AddControl(0,(CGuiControl*)GetButton(1300)); //百兽图鉴
	m_pTab->AddControl(0,(CGuiControl*)GetButton(128)); //翅膀封印
	m_pTab->AddControl(0,(CGuiControl*)GetButton(131)); //寒冰麒麟 
	m_pTab->AddControl(0,(CGuiControl*)GetButton(8888));//云天别墅
	m_pTab->AddControl(0,(CGuiControl*)GetButton(8890));//时装魔盒
	m_pTab->AddControl(0,(CGuiControl*)GetButton(8889));//修仙系统
	m_pTab->AddControl(0,(CGuiControl*)GetButton(8891));//能力修炼
	
	
	
	}
	else
	{
	GetButton(126)->SetVisible(false);    // 装备助手关闭
	GetButton(1800)->SetVisible(false);    // 附件助手关闭
	// GetButton(1801)->SetVisible(false);    // 儿女关闭
	GetButton(1300)->SetVisible(false);    // 百兽图鉴关闭
	GetButton(128)->SetVisible(false);    // 翅膀封印关闭
	GetButton(131)->SetVisible(false);    // 寒冰麒麟关闭

	GetButton(8888)->SetVisible(false);    // 云天别墅关闭
	GetButton(8890)->SetVisible(false);    // 时装魔盒关闭
	GetButton(8891)->SetVisible(false);    // 能力修炼关闭
	GetButton(8889)->SetVisible(false);    // 修仙系统关闭
	}


	m_pTab->AddControl(0,(CGuiControl*)GetImage(152)); //soke 时装
	m_pTab->AddControl(0,(CGuiControl*)GetImage(153)); //soke 护身符
	m_pTab->AddControl(0,(CGuiControl*)GetImage(154)); //soke 勋章
	m_pTab->AddControl(0,(CGuiControl*)GetImage(155)); //soke 翅膀
	m_pTab->AddControl(0,(CGuiControl*)GetImage(156)); //soke 神兵
	m_pTab->AddControl(0,(CGuiControl*)GetImage(157)); //soke 神兽
	m_pTab->AddControl(0,(CGuiControl*)GetImage(158)); //soke 神兵
	m_pTab->AddControl(0,(CGuiControl*)GetImage(159)); //soke 神兽
	
	
	m_pTab->AddControl(1,(CGuiControl*)GetButton(200));
	m_pTab->AddControl(1,(CGuiControl*)GetButton(201));
	m_pTab->AddControl(1,(CGuiControl*)GetButton(202));
	m_pTab->AddControl(1,(CGuiControl*)GetButton(203));
	m_pTab->AddControl(1,(CGuiControl*)GetButton(204));


	m_pTab->AddControl(1,(CGuiControl*)GetButton(20));
	m_pTab->AddControl(1,(CGuiControl*)GetButton(21));
	m_pTab->AddControl(1,(CGuiControl*)GetButton(22));
	m_pTab->AddControl(1,(CGuiControl*)GetButton(23));
	m_pTab->AddControl(1,(CGuiControl*)GetButton(24));

    m_pTab->AddControl(1,GetControl(601));
	m_pTab->AddControl(1,GetControl(602));
	m_pTab->AddControl(1,GetControl(603));
	m_pTab->AddControl(1,GetControl(604));
	m_pTab->AddControl(1,GetControl(605));
	m_pTab->AddControl(1,GetControl(606));
	m_pTab->AddControl(1,GetControl(607));
	m_pTab->AddControl(1,GetControl(608));
	m_pTab->AddControl(1,GetControl(609));
	m_pTab->AddControl(1,GetControl(610));
	m_pTab->AddControl(1,GetControl(611));
	m_pTab->AddControl(1,GetControl(612));
	m_pTab->AddControl(1,GetControl(613));
	m_pTab->AddControl(1,GetControl(614));
	m_pTab->AddControl(1,GetControl(615));
	m_pTab->AddControl(1,GetControl(616));
	m_pTab->AddControl(1,GetControl(617));
	m_pTab->AddControl(1,GetControl(618));
	m_pTab->AddControl(1,GetControl(619));
	m_pTab->AddControl(1,GetControl(620));
	m_pTab->AddControl(1,GetControl(621));
	m_pTab->AddControl(1,GetControl(622));
	m_pTab->AddControl(1,GetControl(623));
	m_pTab->AddControl(1,GetControl(624));
	m_pTab->AddControl(1,GetControl(625));
	m_pTab->AddControl(1,GetControl(626));

	//头衔称号
	m_pTab->AddControl(1,GetButton(1721));
	//封印系统
	m_pTab->AddControl(1,GetButton(1722));
	m_pTab->AddControl(1,GetButton(1821));
	// m_pTab->AddControl(1,(CGuiControl*)GetButton(2500));//元神

	//soke 坐骑被观察显示的
	m_pTab->AddControl(2,(CGuiControl*)GetTable(215));
	m_pTab->AddControl(2,(CGuiControl*)GetTable(216));
	m_pTab->AddControl(2,(CGuiControl*)GetTable(217));
	m_pTab->AddControl(2,(CGuiControl*)GetTable(218));
	m_pTab->AddControl(2,(CGuiControl*)GetTable(219));
	m_pTab->AddControl(2,(CGuiControl*)GetTable(220));
	m_pTab->AddControl(2,GetControl(300));
	m_pTab->AddControl(2,GetControl(301));
	m_pTab->AddControl(2,GetControl(302));
	m_pTab->AddControl(2,GetControl(303));
	m_pTab->AddControl(2,GetControl(304));
	m_pTab->AddControl(2,GetControl(305));
	m_pTab->AddControl(2,GetControl(306));
	m_pTab->AddControl(2,GetControl(307));
	m_pTab->AddControl(2,GetControl(308));
	m_pTab->AddControl(2,GetControl(309));
	m_pTab->AddControl(2,GetControl(314));
	m_pTab->AddControl(2,GetControl(315));
	m_pTab->AddControl(2,GetControl(316));
	m_pTab->AddControl(2,GetControl(400));
	// m_pTab->AddControl(2,(CGuiControl*)GetButton(2500));//元神

	//soke 一键换装
	m_pTab->AddControl(3,GetControl(11));
	m_pTab->AddControl(3,GetControl(12));
	m_pTab->AddControl(3,GetControl(13));
	m_pTab->AddControl(3,(CGuiControl*)GetComboBox(14));
	m_pTab->AddControl(3,(CGuiControl*)GetButton(15));
	m_pTab->AddControl(3,(CGuiControl*)GetButton(16));
	m_pTab->AddControl(3,(CGuiControl*)GetButton(17));
	m_pTab->AddControl(3,(CGuiControl*)GetButton(18));
	m_pTab->AddControl(3,(CGuiControl*)GetButton(19));
	// m_pTab->AddControl(3,(CGuiControl*)GetImage(153)); //soke 时装
	// m_pTab->AddControl(3,(CGuiControl*)GetImage(154)); //soke 护身符
	// m_pTab->AddControl(3,(CGuiControl*)GetImage(155)); //soke 勋章	
	// m_pTab->AddControl(3,(CGuiControl*)GetImage(156)); //soke 翅膀	
	m_pTab->AddControl(3,(CGuiControl*)GetImage(702));
	m_pTab->AddControl(3,(CGuiControl*)GetImage(703));
	m_pTab->AddControl(3,(CGuiControl*)GetImage(704));
	m_pTab->AddControl(3,(CGuiControl*)GetImage(705));
	m_pTab->AddControl(3,(CGuiControl*)GetImage(706));
	m_pTab->AddControl(3,(CGuiControl*)GetImage(707));
	m_pTab->AddControl(3,(CGuiControl*)GetImage(708));
	m_pTab->AddControl(3,(CGuiControl*)GetImage(709));

	m_pTab->AddControl(3,(CGuiControl*)GetTable(500));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(501));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(502));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(503));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(504));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(505));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(506));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(507));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(508));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(509));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(510));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(511));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(512));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(513));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(514));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(515));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(516));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(517));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(518));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(519));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(520));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(521));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(522));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(523));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(524));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(525));
	m_pTab->AddControl(3,(CGuiControl*)GetTable(526));
	// m_pTab->AddControl(3,(CGuiControl*)GetButton(2500));//元神
	
	m_pBtn_Skill = GetButton(1613);
	m_pTab->AddControl(3, (CGuiControl *)m_pBtn_Skill);
	
	m_pBtn_Skill->GetBitmapInfo(m_rl_Skill, m_frameNum_Skill);
	

	//soke 签名档
	m_pTab->AddControl(4,GetControl(1000));
	m_pTab->AddControl(4,GetControl(1001));
	m_pTab->AddControl(4,GetControl(1002));
	m_pTab->AddControl(4,GetControl(1003));
	m_pTab->AddControl(4,GetControl(1004));
	m_pTab->AddControl(4,GetControl(1005));
	m_pTab->AddControl(4,GetControl(1008));
	m_pTab->AddControl(4,GetControl(1011));
	m_pTab->AddControl(4,GetControl(1012));
	m_pTab->AddControl(4,GetControl(1013));
	//m_pTab->AddControl(4,GetControl(1015));
	m_pTab->AddControl(4,(CGuiControl*)GetComboBox(1019));
	m_pTab->AddControl(4,(CGuiControl*)GetComboBox(1020));
	m_pTab->AddControl(4,(CGuiControl*)GetComboBox(1021));
	m_pTab->AddControl(4,(CGuiControl*)GetComboBox(1103));
	m_pTab->AddControl(4,GetControl(1105));
	m_pTab->AddControl(4,GetControl(1107));
	m_pTab->AddControl(4,GetControl(1108));
	// m_pTab->AddControl(4,(CGuiControl*)GetButton(2500));//元神
//	m_pTab->AddControl(4,GetControl(1110));
//	m_pTab->AddControl(4,GetControl(1111));
//	m_pTab->AddControl(4,(CGuiControl*)GetImage(1112));

	//soke 法宝
	m_pTab->AddControl(5,(CGuiControl*)GetImage(1700));
	m_pTab->AddControl(5,(CGuiControl*)GetImage(1701));
	m_pTab->AddControl(5,GetControl(1702));
	m_pTab->AddControl(5,GetControl(1703));
	m_pTab->AddControl(5,GetControl(1704));
	m_pTab->AddControl(5,GetControl(1705));
	m_pTab->AddControl(5,GetControl(1706));
	m_pTab->AddControl(5,GetControl(1707));
	m_pTab->AddControl(5,GetControl(1708));
	m_pTab->AddControl(5,GetControl(1709));

	m_pTab->AddControl(5,GetControl(1710));
	m_pTab->AddControl(5,GetControl(1711));
	m_pTab->AddControl(5,GetControl(1712));
	m_pTab->AddControl(5,GetControl(1713));
	m_pTab->AddControl(5,GetControl(1714));

	m_pTab->AddControl(5,GetControl(1715));
	m_pTab->AddControl(5,GetControl(1716));
	m_pTab->AddControl(5,GetControl(1717));
	m_pTab->AddControl(5,GetControl(1718));
	m_pTab->AddControl(5,GetControl(1719));
	m_pTab->AddControl(5,GetControl(1802));
	m_pTab->AddControl(5,GetControl(1803));
	
	m_pTab->AddControl(5,(CGuiControl*)GetButton(1503));
	m_pTab->AddControl(5,(CGuiControl*)GetButton(1504));
	m_pTab->AddControl(5,(CGuiControl*)GetButton(1505));
	m_pTab->AddControl(5,(CGuiControl*)GetButton(1506));
	m_pTab->AddControl(5,(CGuiControl*)GetButton(1510));//展示法宝


	
	

	m_pTab->AddControl(5,(CGuiControl*)GetButton(1550));
	m_pTab->AddControl(5,(CGuiControl*)GetTable(1508));
	m_pTab->AddControl(5,(CGuiControl*)GetButton(1559));
	m_pTab->AddControl(5,(CGuiControl*)GetTable(1520));
	m_pTab->AddControl(5,(CGuiControl*)GetTable(1521));
	m_pTab->AddControl(5,(CGuiControl*)GetTable(1522));
	m_pTab->AddControl(5,(CGuiControl*)GetTable(1523));
	m_pTab->AddControl(5,(CGuiControl*)GetTable(1524));
	m_pTab->AddControl(5,(CGuiControl*)GetTable(1525));

	

	m_pTab->AddControl(5,(CGuiControl*)GetTable(1600));
	m_pTab->AddControl(5,(CGuiControl*)GetTable(1601));
	m_pTab->AddControl(5,(CGuiControl*)GetTable(1602));
	m_pTab->AddControl(5,(CGuiControl*)GetTable(1603));
	m_pTab->AddControl(5,(CGuiControl*)GetTable(1604));
	m_pTab->AddControl(5,(CGuiControl*)GetTable(1605));
	m_pTab->AddControl(5,(CGuiControl*)GetButton(1611));
	m_pTab->AddControl(5,(CGuiControl*)GetButton(1612));
	m_pTab->AddControl(5,(CGuiControl*)GetImage(1613));

	m_pTab->AddControl(5,(CGuiControl*)GetProcess(1800));
	m_pTab->AddControl(5,(CGuiControl*)GetProcess(1801));
	


	//十二星宿
	m_pTab->AddControl(6,(CGuiControl*)GetTable(2000));
	m_pTab->AddControl(6,(CGuiControl*)GetTable(2001));
	m_pTab->AddControl(6,(CGuiControl*)GetTable(2002));
	m_pTab->AddControl(6,(CGuiControl*)GetTable(2003));
	m_pTab->AddControl(6,(CGuiControl*)GetTable(2004));
	m_pTab->AddControl(6,(CGuiControl*)GetTable(2005));
	m_pTab->AddControl(6,(CGuiControl*)GetTable(2006));
	m_pTab->AddControl(6,(CGuiControl*)GetTable(2007));
	m_pTab->AddControl(6,(CGuiControl*)GetTable(2008));
	m_pTab->AddControl(6,(CGuiControl*)GetTable(2009));
	m_pTab->AddControl(6,(CGuiControl*)GetTable(2010));
	m_pTab->AddControl(6,(CGuiControl*)GetTable(2011));
	m_pTab->AddControl(6,(CGuiControl*)GetButton(2012));//元神
	m_pTab->AddControl(6,(CGuiControl*)GetButton(2500));//元神
	// m_pTab->AddControl(6,(CGuiControl*)GetButton(2502));//五附件
	m_pTab->AddControl(6,(CGuiControl*)GetButton(2501));//五附件
	GetButton(2012)->SetBitmap(stResourceLocation("data\\Interfaces3.gl",103,1),1);
	stPointF scale(0.8f, 0.8f);
	GetButton(2012)->SetBitmapScale(scale);
	GetButton(1505)->SetEnabled(false);
	GetButton(1506)->SetEnabled(false);



	//元神
	m_pTab->AddControl(7,(CGuiControl*)GetTable(1201));
	m_pTab->AddControl(7,(CGuiControl*)GetTable(1202));
	m_pTab->AddControl(7,(CGuiControl*)GetTable(1203));
	m_pTab->AddControl(7,(CGuiControl*)GetTable(1204));
	m_pTab->AddControl(7,(CGuiControl*)GetTable(1205));
	m_pTab->AddControl(7,(CGuiControl*)GetTable(1206));
	m_pTab->AddControl(7,(CGuiControl*)GetTable(1207));
	m_pTab->AddControl(7,(CGuiControl*)GetButton(1208));
	m_pTab->AddControl(7,(CGuiControl*)GetStatic(1210));
	m_pTab->AddControl(7,(CGuiControl*)GetStatic(1211));
	m_pTab->AddControl(7,(CGuiControl*)GetStatic(1212));
	m_pTab->AddControl(7,(CGuiControl*)GetStatic(1213));
	m_pTab->AddControl(7,(CGuiControl*)GetStatic(1214));
	m_pTab->AddControl(7,(CGuiControl*)GetButton(1216));
	
	
	
	m_pTab->SetVisible(false);

	fabao_show = false;

	OnGuiEvent(EVENT_BUTTON_CLICKED,5,GetButton(5));

	SetTitleList();

	//一键换装冷却
	onekey_time=0;
	hanbing_state = 0;
	//sky 马匹属性控件
	m_pHorseListBox =  GetListBox(400);

	FUNCTION_END;
}

bool CGuiUserInfo::IsTonic()
{
	FUNCTION_BEGIN;

	CRoleItem* pItem = GetEquipItem( EQUIPCELLTYPE_ADORN, EQUIPCELLTYPE_LEFT );
	if ( pItem && (pItem->GetObjectBase()->dwType!=ItemType_Tonic) )
		pItem = NULL;

	if ( NULL == pItem )
	{
		pItem = GetEquipItem( EQUIPCELLTYPE_ADORN, EQUIPCELLTYPE_RIGHT );
		if ( pItem && (pItem->GetObjectBase()->dwType!=ItemType_Tonic) )
			pItem = NULL;
	}

	return pItem != NULL;

	/*CGuiTable* pTable = GetEquipTable(EQUIPCELLTYPE_ADORN);
	if ((pTable)&&(pTable->m_listItem.size()>0))
	{
		CGuiItemCell* pCell = pTable->m_listItem[0];
		if (pCell&&pCell->m_pItemData)
		{
			CRoleItem* pItem = (CRoleItem*)pCell->m_pItemData;
			if (pItem->GetObjectBase()->dwType==ItemType_Tonic)
			{
				return true;
			}
		}
	}
	return false;*/

	FUNCTION_END;
}



void CGuiUserInfo::SetTitleList()
{
	m_titleList.clear();
	m_equipList.clear();
	CIni ini;
	ini.Open("data\\datas.pak","datas\\userinfo.ini");

	char szTxt1[10]={0};
	char szTxt2[10]={0};

	for (int i=1;i<=2;i++)
	{
		for (int j=1;j<=26;j++)
		{
			std::vector<UINT, std::stack_allocator<UINT> > info;
			_parse_str_num(info,ini.GetString(_itoa(i,szTxt1,10),_itoa(j,szTxt2,10),"0,0,0,0,0").c_str());

			if (info.size()==5)
			{
				stTitle  st;
				st.key = i*100+j;
				st.rect.SetRect(info[0],info[1],info[2],info[3]);
				st.index = info[4];
				
				if (st.rect!=stRectI(0,0,0,0))
					m_titleList.push_back(st);
			}
		}
	}

	for (int i=1;i<=3;i++)
	{
		std::vector<UINT, std::stack_allocator<UINT> > info;
		_parse_str_num(info,ini.GetString("3",_itoa(i,szTxt2,10),"0,0,0,0,0").c_str());

		if (info.size()==5)
		{
			stTitle  st;
			st.key = i;
			st.rect.SetRect(info[0],info[1],info[2],info[3]);
			st.index = info[4];

			if (st.rect!=stRectI(0,0,0,0))
				m_equipList.push_back(st);
		}
	}
}

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

void CGuiUserInfo::TipsRender()
{
	static int key = 0;

	if (m_bMainUser==false)
		return;

	if (GetGuiManager()->GetMouseOverDlg()!=this)
		return;

	stPointI pt(Engine_GetCursor()->GetPosition());
	pt -= GetLocation();

	for (int i=0;i<m_titleList.size();i++)
	{
		if (m_titleList[i].index!=m_pTab->GetCurItem())
			continue;

		if (m_titleList[i].rect.PtInRect(pt))
		{
			{
				m_ToolTips.Clear();
				m_ToolTips.SetBorderColor(0);
				m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
				m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));

				switch(m_titleList[i].key) 
				{
				case 101:
				case 201:
					AddShowTitle("当前尚未分配的属性点数",m_UserData.pMainUserData->points,m_UserData.pMainUserData->points,"");
					break;
				case 102:
				case 202:
					AddShowTitle("当前力量",m_UserData.pMainUserData->wdStr,m_UserData.pMainUserData->wdStdStr,"");
					break;
				case 103:
				case 203:
					AddShowTitle("当前智力",m_UserData.pMainUserData->wdInt,m_UserData.pMainUserData->wdStdInt,"");
					break;
				case 104:
				case 204:
					AddShowTitle("当前敏捷",m_UserData.pMainUserData->wdDex,m_UserData.pMainUserData->wdStdDex,"");
					break;
				case 105:
				case 205:
					AddShowTitle("当前精神",m_UserData.pMainUserData->wdMen,m_UserData.pMainUserData->wdStdMen,"");
					break;
				case 106:
				case 206:
					AddShowTitle("当前体质",m_UserData.pMainUserData->wdCon,m_UserData.pMainUserData->wdStdCon,"");
					break;
				case 107:
				case 114:
				case 207:
				case 214:
					AddShowTitle("最小物理攻击力",m_UserData.pMainUserData->pdamage,m_UserData.pMainUserData->stdpdamage,"");
					AddShowTitle("\n最大物理攻击力",m_UserData.pMainUserData->maxpdamage,m_UserData.pMainUserData->stdpdamage,"");
					break;
				case 108:
				case 115:
				case 208:
				case 215:
					AddShowTitle("最小魔法攻击力",m_UserData.pMainUserData->mdamage,m_UserData.pMainUserData->stdmdamage,"");
					AddShowTitle("\n最大魔法攻击力",m_UserData.pMainUserData->maxmdamage,m_UserData.pMainUserData->stdmdamage,"");
					break;
				case 109:
				case 116:
				case 209:
				case 216:
					AddShowTitle("物理防御力",m_UserData.pMainUserData->pdefence,m_UserData.pMainUserData->stdpdefence,"");
					break;
				case 110:
				case 117:
				case 210:
				case 217:
					AddShowTitle("魔法防御力",m_UserData.pMainUserData->mdefence,m_UserData.pMainUserData->stdmdefence,"");
					break;
				case 113:
				case 213:
					AddShowTitle("生命值",m_UserData.pMainUserData->maxhp,m_UserData.pMainUserData->maxhp,"");
					break;
				case 118:
				case 218:
					m_ToolTips.AddText("对无属性或相克属性的角色和怪物有攻击加成");
					break;
				case 119:
				case 219:
					m_ToolTips.AddText("职业决定人物的装备和技能");
					break;
				case 120:
				case 220:
					m_ToolTips.AddText("国家评定人物官位的标准值");
					break;
				case 121:
				case 221:
					m_ToolTips.AddText("可影响人物称号的值");
					break;
				case 122:
				case 222:
					m_ToolTips.AddText("可以影响打造成功率,攻击命中率等的属性");
					break;
				case 123:
					m_ToolTips.AddText("荣誉值可换取双倍经验道具或洗PK值");
					break;
				case 223:
					m_ToolTips.AddText(avar("当前荣誉值 %u ,最大荣誉值 %u ",m_UserData.pMainUserData->honor,m_UserData.pMainUserData->maxhonor));
					break;
				case 125:
				case 225:
					m_ToolTips.AddText("您的角色等级在当前服务器所处的名次\n5000名以后不参与排名");
					break;
				case 126:
				case 226:
					//m_ToolTips.AddText(avar("您当前的功勋值为 %d ",m_UserData.pMapUserData->exploit));
					return;
					break;
				case 124:
				case 224:
					{
						DWORD goodness = m_UserData.pMapUserData->goodness & GOODNESS_ATT ? 0 : m_UserData.pMapUserData->goodness;
						DWORD dwNameColor = GetNameColor(goodness);
						if (dwNameColor==-1)
							m_ToolTips.AddText("当前角色的善恶程度");
						else
						{
							m_ToolTips.AddText("您由");
							m_ToolTips.SetCurColor(dwNameColor);
							m_ToolTips.AddText(avar("[%s]",GetPkModeName(goodness))); 
							m_ToolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
							m_ToolTips.AddText(avar("恢复至[普通]还剩%d分钟",abs((short)(0xffff&goodness))));
						}
					}
					break;
				}

				m_ToolTips.Resize();
			}

			stRectI rect = m_titleList[i].rect;
			rect.OffsetRect(GetX(),GetY());

			m_ToolTips.RenderTopLeftPrefer(rect,GetDevice()->GetWidth(),GetDevice()->GetHeight());
			return;
		}
	}
}

void CGuiUserInfo::RefreshGoodnessOnXiXinGeMianState()
{
	DWORD goodness = m_UserData.pMapUserData->goodness & GOODNESS_ATT ? 0 : m_UserData.pMapUserData->goodness;
	if ( goodness == 0 )
		return;

	const int cStep = 60 * 1000;
	int nUnit = 1;
	static DWORD tLastTime = xtimeGetTime() - cStep;
	bool bXiXinGeMianState = false;
	if ( m_bMainUser )
	{
		if ( GetScene()->GetMainCharacter() && isset_state( GetScene()->GetMainCharacter()->GetState(), USTATE_XIXINGEMIA ) )
			bXiXinGeMianState = true;			
	}
	else
	{
		if ( GetScene()->m_pInterestNpc && GetScene()->m_pInterestNpc->IsCharacter() && isset_state( GetScene()->m_pInterestNpc->GetState(), USTATE_XIXINGEMIA ) )
			bXiXinGeMianState = true;	
	}

	if ( bXiXinGeMianState )
		nUnit = 3;

	//Reduce 
	if ( (xtimeGetTime() - tLastTime) >= cStep )
	{
		if ( m_UserData.pMapUserData->goodness >= nUnit )
			m_UserData.pMapUserData->goodness -= nUnit;
		else
			m_UserData.pMapUserData->goodness = 0;
		tLastTime = xtimeGetTime();
	}
}

//sky 一键换装命名字显示
void CGuiUserInfo::RefreshTaoZzhuangNameState()
{
	CGuiComboBox* pCombo = GetComboBox(14);
	int nIndex = pCombo->GetCurItem();

	CGuiButton* btn2 = GetButton(11);
	{
		char szText[256]={};
		char stemp[128]={};

		strcpy(stemp,m_OneKeyEquip[nIndex].sName);

		if(stemp[1]='.')
			strncpy(szText,&stemp[2],2);
		if(stemp[2]='.')
			strncpy(szText,&stemp[3],2);	

		if(!strlen(szText))
		{
			sprintf(szText, "%d", nIndex + 1);
		}
        btn2->SetTextColor(COLOR_ARGB(255,255,0,0));
		btn2->SetText(szText);
		btn2->SetFont(2);
	}
}


/**
* \brief 对话框消息处理
* 
* 
* 
* \param hWnd : 
* \param uMsg : 
* \param wParam : 
* \param lParam : 
* \return 
*/
bool CGuiUserInfo::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;

	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		{
			if (m_bMainUser)
			{
				stPointI pt(Engine_GetCursor()->GetPosition());
				pt -= GetLocation();

				for (int i=0;i<m_equipList.size();i++)
				{
					if (m_equipList[i].index!=m_pTab->GetCurItem())
						continue;

					if (m_equipList[i].rect.PtInRect(pt))
					{
						switch(m_equipList[i].key) 
						{
						case 1:  //身体装备
							{

								
								CRoleItem* pRoleItem = NULL;
								if (GetGuiFloatItem()->GetItem())
									pRoleItem = (CRoleItem*)(GetGuiFloatItem()->GetItem()->m_pItemData);
								if (pRoleItem==NULL)								
									pRoleItem = GetShowClothes();
								if (pRoleItem)
								{
									switch(pRoleItem->GetLocation().y) 
									{
									case EQUIPCELLTYPE_BODY:
										ItemTableEvent(GetEquipTable(EQUIPCELLTYPE_BODY),WM_LBUTTONDOWN,stPointI(350,250),0,0);
										break;
									//soke 时装改了
									/*case EQUIPCELLTYPE_ADORN:
										{
											switch( pRoleItem->GetLocation().x )
											{
											case EQUIPCELLTYPE_LEFT:
												ItemTableEvent(GetEquipTable(EQUIPCELLTYPE_ADORN,EQUIPCELLTYPE_LEFT),WM_LBUTTONDOWN,stPointI(60,175),0,0);
												break;
											case EQUIPCELLTYPE_RIGHT:
												ItemTableEvent(GetEquipTable(EQUIPCELLTYPE_ADORN,EQUIPCELLTYPE_RIGHT),WM_LBUTTONDOWN,stPointI(320,140),0,0);
												break;
											}	
										}										
										break;
									default:
										if (pRoleItem->GetItemType()==ItemType_FashionBody || pRoleItem->GetItemType()==ItemType_HighFashionBody)
										{
											switch( pRoleItem->GetLocation().x )
											{
											case EQUIPCELLTYPE_LEFT:
												ItemTableEvent(GetEquipTable(EQUIPCELLTYPE_ADORN,EQUIPCELLTYPE_LEFT),WM_LBUTTONDOWN,stPointI(60,175),0,0);
												break;
											case EQUIPCELLTYPE_RIGHT:
												ItemTableEvent(GetEquipTable(EQUIPCELLTYPE_ADORN,EQUIPCELLTYPE_RIGHT),WM_LBUTTONDOWN,stPointI(320,140),0,0);
												break;
											}
										}
										else
											ItemTableEvent(GetEquipTable(EQUIPCELLTYPE_BODY),WM_LBUTTONDOWN,stPointI(350,250),0,0);
										break;*/
									//soke 时装
									case EQUIPCELLTYPE_FASHION:
										{
												ItemTableEvent(GetEquipTable(EQUIPCELLTYPE_FASHION),WM_LBUTTONDOWN,stPointI(320,140),0,0);
										}
										break;
									default:
										if (pRoleItem->GetItemType()==ItemType_FashionBody || pRoleItem->GetItemType()==ItemType_HighFashionBody || pRoleItem->GetItemType()==ItemType_XHighFashionBody)
										{
                                            switch(pRoleItem->GetLocation().y) 
									        {
                                                case EQUIPCELLTYPE_FASHION:
												ItemTableEvent(GetEquipTable(EQUIPCELLTYPE_FASHION),WM_LBUTTONDOWN,stPointI(320,140),0,0);
	                                            break;
											}
										}
										else
											ItemTableEvent(GetEquipTable(EQUIPCELLTYPE_BODY),WM_LBUTTONDOWN,stPointI(350,250),0,0);
										break;
									}
								}
							}
							break;
						case 2:  //右手装备
							ItemTableEvent(GetEquipTable(EQUIPCELLTYPE_HANDR),WM_LBUTTONDOWN,stPointI(60,210),0,0);
							break;
						case 3:  //左手装备
							ItemTableEvent(GetEquipTable(EQUIPCELLTYPE_HANDL),WM_LBUTTONDOWN,stPointI(60,250),0,0);
							break;
						}
					}
				}
			}
		}
		break;
	}
	return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
/*
void CGuiUserInfo::InitCustomElements()
{
	FUNCTION_BEGIN;

	//-------------------------------------
	// CGuiButton - Button 小的
	//-------------------------------------
	const char* szGrpFileName = "data\\interfaces.gl";
	stResourceLocation rl;
	rl.SetFileName(GetGuiGraphicPackName());
	rl.group = 3;
	rl.frame = 1;
	
	
	stRectI rcTexture;
	CGuiElement Element;

	rcTexture.SetRect( 0, 0, 19, 20 );
	Element.SetTexture( &rl, &rcTexture );
	Element.SetFont( 0 );
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.TextureColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);

	Element.FontColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.FontColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.FontColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.FontColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	// Assign the Element
	SetCustomElement( GUI_CONTROL_BUTTON, 0, &Element );

	rcTexture.SetRect( 19, 0, 19 * 2, 20 );
	Element.SetTexture( &rl, &rcTexture );
	Element.SetFont( 0 );
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_MOUSEOVER ] = 0;
	Element.TextureColor.States[ GUI_STATE_FOCUS ] = 0;

	Element.FontColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.FontColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.FontColor.States[ GUI_STATE_MOUSEOVER ] = 0;
	Element.FontColor.States[ GUI_STATE_FOCUS ] = 0;
	// Assign the Element
	SetCustomElement( GUI_CONTROL_BUTTON, 1, &Element );

	rcTexture.SetRect( 19 * 2, 0, 19 * 3, 20 );
	Element.SetTexture( &rl, &rcTexture );
	Element.SetFont( 0 );
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = 0;
	Element.TextureColor.States[ GUI_STATE_MOUSEOVER ] = 0;
	Element.TextureColor.States[ GUI_STATE_FOCUS ] = 0;
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 255, 255, 255);

	Element.FontColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.FontColor.States[ GUI_STATE_PRESSED ] = 0;
	Element.FontColor.States[ GUI_STATE_MOUSEOVER ] = 0;
	Element.FontColor.States[ GUI_STATE_FOCUS ] = 0;
	Element.FontColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	// Assign the Element
	SetCustomElement( GUI_CONTROL_BUTTON, 2, &Element );
	

	FUNCTION_END;
}
*/



/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param fElapsedTime : 描述
 * \return 返回值的描述
 */
HRESULT CGuiUserInfo::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;



	// GetButton( 25 )->SetLocation( 361, 96 ); //关闭按钮

	GetButton( 128 )->SetLocation( 300,380 );//soke 翅膀内形按钮位置
  //GetButton( 131 )->SetLocation( 50,415 );//soke 寒冰外形按钮位置
    GetButton( 131 )->SetLocation( 300,340 );//soke 寒冰外形按钮位置
	bool bShowBody = m_bShowBody;

	ControlImage(EQUIPCELLTYPE_BODY);
	ControlImage(EQUIPCELLTYPE_HANDL);
	ControlImage(EQUIPCELLTYPE_HANDR);

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);


	int iOldFont = GetDevice()->SetFont(1);

	// 名字
	stRectI rect(113,101,351,117);
	rect.OffsetRect(m_x,m_y);
	GetDevice()->DrawString(m_UserData.pMapUserData->name,rect,ColorBlend(GetNameColor(m_UserData.pMapUserData->goodness),0xffffffff),DT_VCENTER|DT_CENTER);


	//寒冰麒麟处理
	if(m_UserData.pMainUserData->hanbing == 1)
	{
		hanbing_state=1;
	}
	else{
		hanbing_state=0;
	}
	
	switch(m_pTab->GetCurItem())
	{
	case 0: //soke 装备页面
		{
			ShowUserInfoText1();
			m_pImage[3]->Render(fElapsedTime);
			m_pImage[4]->Render(fElapsedTime);
			m_pImage[0]->Render(fElapsedTime);
			
			BodyRender(GetLocation());

			m_pImage[1]->Render(fElapsedTime);
			m_pImage[2]->Render(fElapsedTime);
			

			ShowEquipByName();

			ShowActiveFiveEffect();
		}
		break;
	case 1: //soke 属性页面
		{
			ShowUserInfoText2();
		}
		break;
	case 3: //soke 换装页面
		{
			ShowUserInfoText3();
		}
		break;
	case 4: //soke 签名档
		{
			ShowUserInfoText4();
		}
		break;
	}

	//云天别墅
	CToolTips *pToolTips = GetButton(8888)->GetToolTips();
	CMainCharacter *pMainChar = GetScene()->GetMainCharacter();
	if (GetGameGuiManager()->m_guiBieShu->bieshu_level==0)
	{
		
		GetButton(8888)->SetBitmap(stResourceLocation("data\\interfaces5.gl", 3, 1), 1);
		char msg[256];
		pToolTips->Clear();
		pToolTips->SetBkColor(D3DCOLOR_ARGB(128, 0, 0, 0));
		pToolTips->SetAlignType(GUI_ALIGN_LEFT);
		pToolTips->SetBorderColor(0);
		pToolTips->SetLineGaps(3);
		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
	
		pToolTips->AddText("私人领地·云天别墅");
		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips->AddText("\n激活状态:",0);
		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		pToolTips->AddText("未激活\n\n",0);

		stResourceLocation rl;
		rl.SetFileName("data\\interfaces5.gl");
		rl.group = 3;
		rl.frame = 90;
		pToolTips->AddAnimation(&rl, false);

		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips->AddText("\n别墅等级Lv:",0);
		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "%d", GetGameGuiManager()->m_guiBieShu->bieshu_level);
		pToolTips->AddText(msg,0);

		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips->AddText("\n人物攻击:",0);
		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "+%d", GetGameGuiManager()->m_guiBieShu->bieshu_gong);
		pToolTips->AddText(msg,0);

		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips->AddText("\n人物防御:",0);
		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "+%d", GetGameGuiManager()->m_guiBieShu->bieshu_fang);
		pToolTips->AddText(msg,0);

		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips->AddText("\n生命值上限:",0);
		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "+%d", GetGameGuiManager()->m_guiBieShu->bieshu_hp);
		pToolTips->AddText(msg,0);

		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips->AddText("\n专属私人领地·云天别墅，地图内每20分钟刷新一只橙色BOSS，每15分钟刷新5只绿色BOSS",0);

		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		pToolTips->AddText("\n进入方式:世外桃源·别墅管理员 处进入专属自己的领地别墅",0);
		pToolTips->AddText("\n别墅等级越高提供给人物加成越高",0);

		pToolTips->Resize();
	}
	else
	{
		GetButton(8888)->SetBitmap(stResourceLocation("data\\interfaces5.gl", 3, 2), 1);
		char msg[256];
		pToolTips->Clear();
		pToolTips->SetBkColor(D3DCOLOR_ARGB(128, 0, 0, 0));
		pToolTips->SetAlignType(GUI_ALIGN_LEFT);
		pToolTips->SetBorderColor(0);
		pToolTips->SetLineGaps(3);
		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
	
		pToolTips->AddText("私人领地·云天别墅");
		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips->AddText("\n激活状态:",0);
		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		pToolTips->AddText("已激活\n\n",0);

		stResourceLocation rl;
		rl.SetFileName("data\\interfaces5.gl");
		rl.group = 3;
		rl.frame = 90;
		pToolTips->AddAnimation(&rl, false);

		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips->AddText("\n别墅等级Lv:",0);
		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "%d", GetGameGuiManager()->m_guiBieShu->bieshu_level);
		pToolTips->AddText(msg,0);

		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips->AddText("\n人物攻击:",0);
		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "+%d", GetGameGuiManager()->m_guiBieShu->bieshu_gong);
		pToolTips->AddText(msg,0);

		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips->AddText("\n人物防御:",0);
		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "+%d", GetGameGuiManager()->m_guiBieShu->bieshu_fang);
		pToolTips->AddText(msg,0);

		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips->AddText("\n生命值上限:",0);
		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "+%d", GetGameGuiManager()->m_guiBieShu->bieshu_hp);
		pToolTips->AddText(msg,0);

		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips->AddText("\n专属私人领地·云天别墅，地图内每20分钟刷新一只橙色BOSS，每15分钟刷新5只绿色BOSS",0);

		pToolTips->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		pToolTips->AddText("\n进入方式:世外桃源·别墅管理员 处进入专属自己的领地别墅",0);
		pToolTips->AddText("\n别墅等级越高提供给人物加成越高",0);

		pToolTips->Resize();
	}



	//时装魔盒
	CToolTips *pToolTips2 = GetButton(8890)->GetToolTips();
	if (GetGameGuiManager()->m_guiMagicBox)
	{
		

		char msg[256];
		pToolTips2->Clear();
		pToolTips2->SetBkColor(D3DCOLOR_ARGB(128, 0, 0, 0));
		pToolTips2->SetAlignType(GUI_ALIGN_LEFT);
		pToolTips2->SetBorderColor(0);
		pToolTips2->SetLineGaps(3);
		
		if( GetGameGuiManager()->m_guiMagicBox->mohelevel >=0 &&  GetGameGuiManager()->m_guiMagicBox->mohelevel<=2)
		{
			pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
			pToolTips2->AddText("时装魔盒·出类拔萃");
		}
		else if( GetGameGuiManager()->m_guiMagicBox->mohelevel >=3 &&  GetGameGuiManager()->m_guiMagicBox->mohelevel<=5)
		{
			pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 0));
			pToolTips2->AddText("时装魔盒·惊鸿一瞥");
		}
		else if( GetGameGuiManager()->m_guiMagicBox->mohelevel >=6 &&  GetGameGuiManager()->m_guiMagicBox->mohelevel<=8)
		{
			pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 0,191,255));
			pToolTips2->AddText("时装魔盒·姿容典雅");
		}
		else if( GetGameGuiManager()->m_guiMagicBox->mohelevel >=9 &&  GetGameGuiManager()->m_guiMagicBox->mohelevel<=11)
		{
			pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 221,160,221));
			pToolTips2->AddText("时装魔盒·翩翩如玉");
		}
		else if( GetGameGuiManager()->m_guiMagicBox->mohelevel >=12 &&  GetGameGuiManager()->m_guiMagicBox->mohelevel<=14)
		{
			pToolTips2->SetCurColor(D3DCOLOR_ARGB(255,255,0,0));
			pToolTips2->AddText("时装魔盒·惊若翩鸿");
		}
		else if( GetGameGuiManager()->m_guiMagicBox->mohelevel >=15)
		{
			pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 255,69,0));
			pToolTips2->AddText("时装魔盒·风华绝代");
		}
		
		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips2->AddText("\n时装拥有数量：",0);
		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "[%d] 套\n", GetGameGuiManager()->m_guiMagicBox->shizhuangshu);
		pToolTips2->AddText(msg,0);
		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips2->AddText("\n披风拥有数量：",0);
		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "[%d] 套\n", GetGameGuiManager()->m_guiMagicBox->pifengshu);
		pToolTips2->AddText(msg,0);
		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips2->AddText("\n翅膀拥有数量：",0);
		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "[%d] 套\n", GetGameGuiManager()->m_guiMagicBox->chibangshu);
		pToolTips2->AddText(msg,0);
		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips2->AddText("\n坐骑拥有数量：",0);
		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "[%d] 套\n\n", GetGameGuiManager()->m_guiMagicBox->zuoqishu);
		pToolTips2->AddText(msg,0);

		stResourceLocation r2;
		r2.SetFileName("data\\interfaces.gl");
		r2.group = 289;
		r2.frame = 17;
		pToolTips2->AddAnimation(&r2, false);
		pToolTips2->SetCurColor(COLOR_ARGB(255, 234, 191, 140));
		pToolTips2->AddText(" 魔盒属性 ",c_nDefTipFont);
		r2.SetFileName("data\\interfaces.gl");
		r2.group = 289;
		r2.frame = 18;
		pToolTips2->AddAnimation(&r2, false);

		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips2->AddText("\n魔盒等级Lv：",0);
		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "%d级", GetGameGuiManager()->m_guiMagicBox->mohelevel);
		pToolTips2->AddText(msg,0);

		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips2->AddText("\n当前经验:",0);
		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "%d/%d", GetGameGuiManager()->m_guiMagicBox->moheexp, GetGameGuiManager()->m_guiMagicBox->mohemaxexp);
		pToolTips2->AddText(msg,0);


		

		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips2->AddText("\n增加人物物攻：",0);
		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "+%d", GetGameGuiManager()->m_guiMagicBox->num1);
		pToolTips2->AddText(msg,0);

		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips2->AddText("\n增加人物魔攻：",0);
		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "+%d", GetGameGuiManager()->m_guiMagicBox->num2);
		pToolTips2->AddText(msg,0);

		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips2->AddText("\n增加人物物防：",0);
		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "+%d", GetGameGuiManager()->m_guiMagicBox->num3);
		pToolTips2->AddText(msg,0);

		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips2->AddText("\n增加人物魔防：",0);
		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "+%d", GetGameGuiManager()->m_guiMagicBox->num4);
		pToolTips2->AddText(msg,0);

		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips2->AddText("\n增加人物生命：",0);
		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		sprintf(msg, "+%d", GetGameGuiManager()->m_guiMagicBox->num5);
		pToolTips2->AddText(msg,0);

		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		pToolTips2->AddText("\n时装魔盒系统：每激活或升级一套时装即可增加魔盒经验，魔盒最高等级20级	",0);

		pToolTips2->SetCurColor(D3DCOLOR_ARGB(255, 0, 255, 0));
		pToolTips2->AddText("\n时装魔盒等级越高提供给人物加成越高",0);

		pToolTips2->Resize();
	}


	TipsRender();

	GetDevice()->SetFont(iOldFont);
	
	m_bShowBody = bShowBody;

	RefreshGoodnessOnXiXinGeMianState();

	RefreshTaoZzhuangNameState();
	
	/* 	//soke 籍贯
	CGuiComboBox* pguanji = GetComboBox(1019);
	if(pguanji)
	{
		for(int i=1;i<2;i++)
		{
			char szStr[256];
			sprintf(szStr,"北京市",i);
			pguanji->AddItem(szStr,NULL);
		}
		for(int i=2;i<3;i++)
		{
			char szStr[256];
			sprintf(szStr,"重庆市",i);
			pguanji->AddItem(szStr,NULL);
		}
	}

	//soke 地区
	CGuiComboBox* pdiqu = GetComboBox(1020);
	if(pdiqu)
	{
		for(int i=1;i<2;i++)
		{
			char szStr[256];
			sprintf(szStr,"海淀",i);
			pdiqu->AddItem(szStr,NULL);
		}
	}
	//soke 年龄
	CGuiComboBox* pnianling = GetComboBox(1021);
	if(pnianling)
	{
		for(int i=0;i<1;i++)
		{
			char szStr[256];
			sprintf(szStr,"保密",i);
			pnianling->AddItem(szStr,NULL);
		}
		for(int i=1;i<101;i++)
		{
			char szStr[256];
			sprintf(szStr,"%d",i);
			pnianling->AddItem(szStr,NULL);
		}
	} */

	return hr;

	FUNCTION_END;
}



void CGuiUserInfo::ShowActiveFiveEffect()
{
	if ( m_bMainUser )
		return;

	if ( m_listActiveItem.empty() )
		return;

	for( std::vector<CRoleItem*>::iterator itor=m_listActiveItem.begin();itor!=m_listActiveItem.end();++itor )
	{
		CRoleItem* pItem = (*itor);

		if ( pItem && (pItem->GetObject()->fiveset[0] == 0) )
			continue;

		IBitmaps* pBmp=NULL;

		stRectI tableRect = stRectI(-1000,-1000,0,0);
		CGuiTable* pTable = GetEquipTable( pItem->GetLocation().y, pItem->GetLocation().x );
		if ( pTable )
		{
			tableRect = pTable->m_itemRect;
			pTable->m_pDialog->ClientToScreen( &tableRect );
		}

		switch(pItem->GetObject()->fivetype)
		{
		case FIVE_METAL:
			{
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,100)));
				if (pBmp)
				{
					pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
				}
			}
			break;
		case FIVE_WOOD:
			{
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,101)));
				if (pBmp)
				{
					pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
				}
			}
			break;
		case FIVE_SOIL:
			{
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,102)));
				if (pBmp)
				{
					pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
				}
			}
			break;
		case FIVE_WATER:
			{
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,103)));
				if (pBmp)
				{
					pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
				}
			}
			break;
		case FIVE_FIRE:
			{
				pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,104)));
				if (pBmp)
				{
					pBmp->RenderAni(tableRect.left-112,tableRect.top-112,xtimeGetTime(),-1,Blend_Null);
				}
			}
			break;
		}
	}
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

// myy 新增不限制字符串长度
#define Tips_AddNum1(szFormat,nNum)\
	if(nNum){\
	_snprintf(szTemp,sizeof(szTemp),szFormat,nNum);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText1(szTemp,0);\
	}\

#define Tips_AddAllNum(szFormat,nNum)\
{\
	_snprintf(szTemp,sizeof(szTemp),szFormat,nNum);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText(szTemp,0);\
}\

#define Tips_AddNum2(szFormat,nNum1,nNum2)\
	if(nNum1 || nNum2){\
	_snprintf(szTemp,sizeof(szTemp),szFormat,nNum1,nNum2);\
	szTemp[sizeof(szTemp)-1]=0;\
	m_ToolTips.AddText(szTemp,0);\
	}\


void CGuiUserInfo::ShowEquipByName()
{
	IBitmaps* pBmp=NULL;



	int frame = -1;
	//soke 显示>=10件装备特效
	if ( m_equipByMakerType >= EquipByMaker_T_WONDER_BLUE )
	{
		switch( m_equipByMakerType )
		{
		case EquipByMaker_T_WONDER_BLUE:
			frame = 201;
			break;
		case EquipByMaker_T_WONDER_GOLD:
			frame = 200;
			break;
		case EquipByMaker_T_WONDER_HOLY:
			frame = 204;
			break;
		case EquipByMaker_T_WONDER_PURPLE:
			frame = 204;
			break;
		}
	}
	// //soke 显示<10件装备特效
	// else if ( m_equipByMakerType == EquipByMaker_T_BUG_PURPLE )
	// {
	// 	switch( m_equipByMakerType )
	// 	{
	// 	case EquipByMaker_T_BUG_PURPLE:
	// 		frame = 204;
	// 		break;
	// 	//恋恋 注释紫色光效
	// 	}
	// }

	if ( frame > -1 )
	{
		//Add special effect
		pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,frame)));
		if ( pBmp )
		{
			pBmp->RenderAni(m_x,m_y,xtimeGetTime(),-1,Blend_Null);
		}
	}

	//sky 一件斗魂10星特效
	if (m3_equipByMakerType == EquipByMaker001_T_WONDER_PURPLE)
	{
		pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces1.gl",7,2)));
		if (pBmp)
		{
			pBmp->RenderAni(m_x+35,m_y+123,xtimeGetTime(),-1,Blend_Null);
		}
	}

	/*
	if (m_equipByName.numHoly>=10)
	{
		pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,202)));
		if (pBmp)
		{
			pBmp->RenderAni(m_x,m_y,xtimeGetTime(),-1,Blend_Null);
		}
	}
	else if(m_equipByName.numHoly>=6)
	{

	}
	else if(m_equipByName.numGlod>=10)
	{
		pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,200)));
		if (pBmp)
		{
			pBmp->RenderAni(m_x,m_y,xtimeGetTime(),-1,Blend_Null);
		}

	}
	else if(m_equipByName.numGlod>=6)
	{

	}
	else if(m_equipByName.numBlue>=10)
	{
		pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,201)));
		if (pBmp)
		{
			pBmp->RenderAni(m_x,m_y,xtimeGetTime(),-1,Blend_Null);
		}

	}
	else if(m_equipByName.numBlue>=6)
	{

	}  */

	//if (pBmp!=NULL)
	if( m_equipByMakerType > EquipByMaker_T_NONE || m1_equipByMakerType > EquipByMaker1_T_NONE || m2_equipByMakerType > EquipByMaker2_T_NONE 
	|| m3_equipByMakerType > EquipByMaker3_T_NONE || m21_equipByMakerType > EquipByMaker21_T_NONE || m22_equipByMakerType > EquipByMaker22_T_NONE  
	|| m11_equipByMakerType > EquipByMaker11_T_NONE || m4_equipByMakerType > EquipByMaker4_T_NONE || m5_equipByMakerType > EquipByMaker5_T_NONE
	|| m6_equipByMakerType > EquipByMaker6_T_NONE || m7_equipByMakerType > EquipByMaker7_T_NONE || m24_equipByMakerType > EquipByMaker24_T_NONE)
	{
		if (GetGuiManager()->GetMouseOverDlg()==this)
		{
			stPointI pt(Engine_GetCursor()->GetPosition());
			pt -= GetLocation();

			for (int i=0;i<m_equipList.size();i++)
			{
				if (m_equipList[i].index!=m_pTab->GetCurItem())
					continue;

				if (m_equipList[i].rect.PtInRect(pt))
				{
					switch(m_equipList[i].key) 
					{
					case 1:  //身体装备
						{
							if ( (m_equipByMakerType > EquipByMaker_T_NONE) && (m_equipByMakerType <= EquipByMaker_T_WONDER_PURPLE) )
							{
                                stRectI rect = m_equipList[i].rect;
								rect.OffsetRect(GetX()+30,GetY());
								m_ToolTips.Clear();
								m_ToolTips.SetBorderColor(0);
								stResourceLocation imguserinfo;
			                    imguserinfo.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo.group = 400;
		                        imguserinfo.frame = 1;
		                        m_ToolTips.AddAnimation( &imguserinfo, false );
								m_ToolTips.AddText("\n");
								m_ToolTips.SetAlignType(GUI_ALIGN_LEFT);
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,255,100,25));

								static const int value[8][4] = { {2,4,4,4},{2,4,4,4},{2,4,4,4},{5,8,8,8},{5,8,8,8},{10,8,8,8},{10,10,12,12},{15,10,15,15} };
								static const char* strSetName[] = { "地之侠义", "地之神佑", "地之圣威", "天之英雄", "天之屠魔", "天之圣尊", "神之圣尊", "魔之至尊" };

								char szTemp[512];
								m_ToolTips.AddText( strSetName[m_equipByMakerType] );
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
								Tips_AddNum1( "\n 双攻 +%u%% ", value[m_equipByMakerType][0] );
								Tips_AddNum1( " 双防 +%u%% ", value[m_equipByMakerType][1] );
								Tips_AddNum1( " 生命值 +%u%% ", value[m_equipByMakerType][2] );
								Tips_AddNum1( " 法术值 +%u%% ", value[m_equipByMakerType][3] );
								m_ToolTips.AddText( "\n\n" );
								stResourceLocation imguserinfo1;
			                    imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo1.group = 400;
		                        imguserinfo1.frame = 2;
		                        m_ToolTips.AddAnimation( &imguserinfo1, false );
								m_ToolTips.Resize();
								m_ToolTips.RenderTopLeftPrefer(rect,GetDevice()->GetWidth(),GetDevice()->GetHeight());		

							}
                            //绿装15星套装效果、绿装套装效果
							if ( (m1_equipByMakerType > EquipByMaker1_T_NONE) && (m1_equipByMakerType <= EquipByMaker1_T_WONDER_PURPLE) )
							{ 
								m_ToolTips.SetBorderColor(0);
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,255,100,25));
							 
							    static const int value[2][4] = { {12,12,12,12},{12,12,12,12} };
								static const char* strSetName[] = { "15星神圣套装效果(10件)", "神圣绿装套装效果(10件)" };

								char szTemp[512];
								
								m_ToolTips.AddText( strSetName[m1_equipByMakerType] );
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
								Tips_AddNum1( "\n 双攻 +%u%% ", value[m1_equipByMakerType][0] );
								Tips_AddNum1( " 双防 +%u%% ", value[m1_equipByMakerType][1] );
								Tips_AddNum1( " 生命值 +%u%% ", value[m1_equipByMakerType][2] );
								Tips_AddNum1( " 法术值 +%u%% ", value[m1_equipByMakerType][3] );
								m_ToolTips.AddText( "\n\n" );
								stResourceLocation imguserinfo1;
			                    imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo1.group = 400;
		                        imguserinfo1.frame = 2;
		                        m_ToolTips.AddAnimation( &imguserinfo1, false );
								m_ToolTips.Resize();
							}
                            //soke 紫装星星套装显示效果
							if ( (m2_equipByMakerType > EquipByMaker3_T_WONDER_PURPLE) && (m2_equipByMakerType <= EquipByMaker15_T_WONDER_PURPLE) )
							{   
								m_ToolTips.SetBorderColor(0);
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,255,100,25));
							 
							    static const int value[8][3] = { {1000,500,10000},{5000,2500,50000},{8000,4000,80000},{15000,7500,150000},{30000,15000,300000},{50000,25000,500000},{70000,35000,700000},{100000,50000,1000000} };
								static const char* strSetName[] = { "3星绿装套装效果(10件)", "5-7星套装效果(10件)", "8-9星套装效果(10件)", "10-11星套装效果(10件)", "12星套装效果(10件)", "13星套装效果(10件)", "14星套装效果(10件)", "15星套装效果(10件)" };

								char szTemp[512];

								m_ToolTips.AddText( strSetName[m2_equipByMakerType] );
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
								Tips_AddNum1( "\n 双攻 +%u% ", value[m2_equipByMakerType][0] );
								Tips_AddNum1( " 双防 +%u% ", value[m2_equipByMakerType][1] );
								Tips_AddNum1( " 生命值 +%u% ", value[m2_equipByMakerType][2] );
								
								// //星套压制系统
								// static const char* jianshang[] = { "受到来自3星套以下的伤害减伤2%", "受到来自5星套以下的伤害减伤4%", "受到来自8星套以下的伤害减伤6%", "受到来自10星套以下的伤害减伤8%", "受到来自12星套以下的伤害减伤10%", "受到来自13星套以下的伤害减伤12%", "受到来自14星套以下的伤害减伤15%", "受到来自15星套以下的伤害减伤20%" };
								// static const char* zengshang[] = { "攻击3星套以下的玩家伤害增伤2%", "攻击5星套以下的玩家伤害增伤4%", "攻击8星套以下的玩家伤害增伤6%", "攻击10星套以下的玩家伤害增伤8%", "攻击12星套以下的玩家伤害增伤10%", "攻击13星套以下的玩家伤害增伤12%", "攻击14星套以下的玩家伤害增伤15%", "攻击15星套以下的玩家伤害增伤20%" };

							
								m_ToolTips.AddText( "\n\n" );
								stResourceLocation imguserinfo1;
			                    imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo1.group = 400;
		                        imguserinfo1.frame = 2;
		                        m_ToolTips.AddAnimation( &imguserinfo1, false );
								m_ToolTips.Resize();
							}
                            // //soke 第二灵魂套装效果
							// if ( (m21_equipByMakerType > EquipByMaker21_T_NONE) && (m21_equipByMakerType <= EquipByMaker25_T_WONDER_PURPLE) )
							// {
							// 	m_ToolTips.SetBorderColor(0);
							// 	m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
							// 	m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));

							// 	static const int value[5][1] = { {200000},{200000},{200000},{200000},{200000} };
							// 	static const char* strSetName[] = { "绿装第二灵魂相同套装效果(8件)", "绿装第二灵魂相同套装效果(8件)", "绿装第二灵魂相同套装效果(8件)", "绿装第二灵魂相同套装效果(8件)", "绿装第二灵魂相同套装效果(8件)" };
							   
                            //     char szTemp[512];
							// 	m_ToolTips.AddText( strSetName[m21_equipByMakerType] );
                            //     m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
							// 	m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
							// 	Tips_AddNum1( "\n 额外增加生命值上限 %u% ", value[m21_equipByMakerType][0] );
							// 	 m_ToolTips.AddText("\n");
							//     stResourceLocation imguserinfo1;
			                //     imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                //     imguserinfo1.group = 400;
		                    //     imguserinfo1.frame = 2;
		                    //     m_ToolTips.AddAnimation( &imguserinfo1, false );
							// 	m_ToolTips.Resize();
							// }
                            //soke 装备等级套装效果
					    	if ( (m11_equipByMakerType > EquipByMaker11_T_NONE) && (m11_equipByMakerType <= EquipByMaker220_T_WONDER_PURPLE) )
							{   
								m_ToolTips.SetBorderColor(0);
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,255));
							 
							    static const int value[12][3] = { {2000,1000,20000},{4000,2000,40000},{6000,3000,60000},{8000,4000,80000},{10000,5000,100000},{12000,6000,120000},{14000,7000,140000},{16000,8000,160000},{18000,9000,180000},{20000,10000,200000},{48000,24000,480000},{30000,15000,300000} };
								static const char* strSetName[] = { "乾坤套装效果(10件)", "炙天套装效果(10件)", "轩辕套装效果(10件)", "卓越套装效果(10件)", "传说套装效果(10件)", "天尊套装效果(10件)", "英雄套装效果(10件)", "圣者套装效果(10件)", "战神套装效果(10件)", "帝王套装效果(10件)", "弑天套装效果(10件)", "永恒套装效果(10件) "};

								char szTemp[512];
								
								m_ToolTips.AddText( strSetName[m11_equipByMakerType] );
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
								Tips_AddNum1( "\n 双攻 +%u% ", value[m11_equipByMakerType][0] );
								Tips_AddNum1( " 双防 +%u% ", value[m11_equipByMakerType][1] );
								Tips_AddNum1( " 生命值 +%u% ", value[m11_equipByMakerType][2] );
								m_ToolTips.AddText( "\n\n" );				
								stResourceLocation imguserinfo1;
			                    imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo1.group = 400;
		                        imguserinfo1.frame = 2;
		                        m_ToolTips.AddAnimation10( &imguserinfo1, false );
								m_ToolTips.Resize();
							}
                            //soke 斗魂套装显示效果
							if ( (m3_equipByMakerType > EquipByMaker3_T_NONE) && (m3_equipByMakerType <= EquipByMaker104_T_WONDER_PURPLE) )
							{  
								m_ToolTips.SetBorderColor(0);
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,255,100,25));
							 
							   static const int value[5][3] = { {3000,2500,30000},{6000,5000,60000},{9000,7500,90000},{20000,15000,180000},{50000,30000,500000} };
							   static const char* strSetName[] = { "全套3星斗魂套装效果(10件)","全套6星斗魂套装效果(10件)","全套9星斗魂套装效果(10件)","全套12星斗魂套装效果(10件)","全套15星斗魂套装效果(10件)" };

								char szTemp[512];
								
								m_ToolTips.AddText( strSetName[m3_equipByMakerType] );
                                m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
								Tips_AddNum1( "\n 双攻 +%u% ", value[m3_equipByMakerType][0] );
								Tips_AddNum1( " 双防 +%u% ", value[m3_equipByMakerType][1] );
								Tips_AddNum1( " 生命值 +%u% ", value[m3_equipByMakerType][2] );
								m_ToolTips.AddText("\n\n");
								//m_ToolTips.AddText("\n ");
								m_ToolTips.Resize();
                                stResourceLocation imguserinfo1;
			                    imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo1.group = 400;
		                        imguserinfo1.frame = 2;
		                        m_ToolTips.AddAnimation( &imguserinfo1, false );
							}
                            //soke 龙星套装显示效果
							if ( (m4_equipByMakerType > EquipByMaker4_T_NONE) && (m4_equipByMakerType <= EquipByMaker415_T_WONDER_PURPLE) )
							{  
								m_ToolTips.SetBorderColor(0);
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,255,100,25));
							 
							   static const int value[5][3] = { {5000,2500,50000},{10000,5000,100000},{30000,15000,300000},{50000,25000,500000},{100000,50000,1000000} };
								static const char* strSetName[] = {"5星龙星套装效果(10件)", "8星龙星套装效果(10件)", "11星龙星套装效果(10件)","13星龙星套装效果(10件)", "15星龙星套装效果(10件)" };

								char szTemp[512];
								
								m_ToolTips.AddText( strSetName[m4_equipByMakerType] );
                                m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
								Tips_AddNum1( "\n 双攻 +%u% ", value[m4_equipByMakerType][0] );
								Tips_AddNum1( " 双防 +%u% ", value[m4_equipByMakerType][1] );
								Tips_AddNum1( " 生命值 +%u% ", value[m4_equipByMakerType][2] );
								m_ToolTips.AddText("\n\n");
								m_ToolTips.Resize();
                                stResourceLocation imguserinfo1;
			                    imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo1.group = 400;
		                        imguserinfo1.frame = 2;
		                        m_ToolTips.AddAnimation( &imguserinfo1, false );
							}
					        //soke 补天套装显示效果
							if ( (m7_equipByMakerType > EquipByMaker7_T_NONE) && (m7_equipByMakerType <= EquipByMaker75_T_WONDER_PURPLE) )
							{  
								m_ToolTips.SetBorderColor(0);
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,255,100,25));

								static const int value[5][3] = { {3,2,3},{6,5,6},{10,8,10},{15,12,15},{24,20,24}};
								static const char* strSetName[] = {"补天石套装效果(10件)", "补天灵石套装效果(10件)", "补天神石套装效果(10件)","补天仙石套装效果(10件)", "补天圣石套装效果(10件)" };

								char szTemp[512];

								m_ToolTips.AddText( strSetName[m7_equipByMakerType] );
                                m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
								Tips_AddNum1( "\n 双攻 +%u%% ", value[m7_equipByMakerType][0] );
								Tips_AddNum1( " 双防 +%u%% ", value[m7_equipByMakerType][1] );
								Tips_AddNum1( " 生命法术 +%u%% ", value[m7_equipByMakerType][2] );
								m_ToolTips.AddText("\n\n");
								//m_ToolTips.AddText("\n ");
								m_ToolTips.Resize();
                                stResourceLocation imguserinfo1;
			                    imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo1.group = 400;
		                        imguserinfo1.frame = 2;
		                        m_ToolTips.AddAnimation( &imguserinfo1, false );
							}
						    //soke 装备栏套装显示效果
							if ( (m5_equipByMakerType > EquipByMaker5_T_NONE) && (m5_equipByMakerType <= EquipByMaker56_T_WONDER_PURPLE) )
							{   
								m_ToolTips.SetBorderColor(0);
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,255,100,25));

								static const int value[5][3] = { {5000,5000,50000},{10000,10000,100000},{15000,15000,150000},{20000,20000,200000},{30000,25000,300000}};
								static const char* strSetName[] = {"装备栏蓝色祝福(10件)", "装备栏黄色祝福(10件)", "装备栏绿色祝福(10件)","装备栏紫色祝福(10件)", "装备栏完美紫祝福(10件)" };

								char szTemp[512];

								m_ToolTips.AddText( strSetName[m5_equipByMakerType] );
                                m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
								Tips_AddNum1( "\n 双攻 +%u% ", value[m5_equipByMakerType][0] );
								Tips_AddNum1( " 双防 +%u% ", value[m5_equipByMakerType][1] );
								Tips_AddNum1( " 生命值 +%u% ", value[m5_equipByMakerType][2] );
								m_ToolTips.AddText("\n\n");
								//m_ToolTips.AddText("\n ");
								m_ToolTips.Resize();
                                stResourceLocation imguserinfo1;
			                    imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo1.group = 400;
		                        imguserinfo1.frame = 2;
		                        m_ToolTips.AddAnimation( &imguserinfo1, false );
							}
							//soke 神龙宝宝镶嵌套装显示效果
							if ( (m12_equipByMakerType > EquipByMaker12_T_NONE) && (m12_equipByMakerType <= EquipByMaker12_T_SHENLONG_LV6) )
							{   
								m_ToolTips.SetBorderColor(0);
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,255,100,25));
							 
							    static const int value[6][3] = { {3000,2500,30000},{6000,5000,60000},{9000,7500,90000},{12000,10000,120000},{15000,12500,150000},{50000,30000,500000} };
								static const char* strSetName[] = { "小龙降临套装效果(10件)", "金龙降临套装效果(10件)", "苍龙降临套装效果(10件)", "巨龙降临套装效果(10件)", "神苍龙降临套装效果(10件)", "神巨龙降临套装效果(10件)" };

								char szTemp[512];

								m_ToolTips.AddText( strSetName[m12_equipByMakerType] );
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
								Tips_AddNum1( "\n 双攻 +%u% ", value[m12_equipByMakerType][0] );
								Tips_AddNum1( " 双防 +%u% ", value[m12_equipByMakerType][1] );
								Tips_AddNum1( " 生命值 +%u% ", value[m12_equipByMakerType][2] );
								m_ToolTips.AddText( "\n\n" );
								stResourceLocation imguserinfo1;
			                    imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo1.group = 400;
		                        imguserinfo1.frame = 2;
		                        m_ToolTips.AddAnimation( &imguserinfo1, false );
								m_ToolTips.Resize();
							}
							//soke 逆天进阶套装显示效果
							if ( (m13_equipByMakerType > EquipByMaker13_T_NONE) && (m13_equipByMakerType <= EquipByMaker13_T_NITIAN_LV5) )
							{   
								m_ToolTips.SetBorderColor(0);
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,255,100,25));
							 
							    static const int value[5][3] = { {3000,2500,30000},{6000,5000,60000},{9000,7500,90000},{12000,10000,120000},{30000,15000,300000}};
								static const char* strSetName[] = { "15阶逆天套装效果(10件)", "30阶逆天套装效果(10件)", "50阶逆天套装效果(10件)", "80阶逆天套装效果(10件)", "100阶逆天套装效果(10件)" };

								char szTemp[512];

								m_ToolTips.AddText( strSetName[m13_equipByMakerType] );
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
								Tips_AddNum1( "\n 双攻 +%u% ", value[m13_equipByMakerType][0] );
								Tips_AddNum1( " 双防 +%u% ", value[m13_equipByMakerType][1] );
								Tips_AddNum1( " 生命值 +%u% ", value[m13_equipByMakerType][2] );
								m_ToolTips.AddText( "\n\n" );
								stResourceLocation imguserinfo1;
			                    imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo1.group = 400;
		                        imguserinfo1.frame = 2;
		                        m_ToolTips.AddAnimation( &imguserinfo1, false );
								m_ToolTips.Resize();
							}
                            //soke 神魂宝石套装显示效果
							if ( (m14_equipByMakerType > EquipByMaker14_T_NONE) && (m14_equipByMakerType <= EquipByMaker14_T_DOUZHANSF_LV4) )
							{   
								m_ToolTips.SetBorderColor(0);
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,255,100,25));
							 
							    static const int value[4][3] = { {3000,2500,30000},{6000,5000,60000},{9000,7500,90000},{30000,15000,300000} };
								static const char* strSetName[] = { "25级神魂宝石套装效果(10件)", "50级神魂宝石套装效果(10件)", "75级神魂宝石套装效果(10件)", "100级神魂宝石套装效果(10件)" };

								char szTemp[512];

								m_ToolTips.AddText( strSetName[m14_equipByMakerType] );
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
								Tips_AddNum1( "\n 双攻 +%u% ", value[m14_equipByMakerType][0] );
								Tips_AddNum1( " 双防 +%u% ", value[m14_equipByMakerType][1] );
								Tips_AddNum1( " 生命值 +%u% ", value[m14_equipByMakerType][2] );
								m_ToolTips.AddText( "\n\n" );
								stResourceLocation imguserinfo1;
			                    imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo1.group = 400;
		                        imguserinfo1.frame = 2;
		                        m_ToolTips.AddAnimation( &imguserinfo1, false );
								m_ToolTips.Resize();
							}
							//soke 暗影宝石套装显示效果
							if ( (m15_equipByMakerType > EquipByMaker15_T_NONE) && (m15_equipByMakerType <= EquipByMaker15_T_AYSTONE_LV5) )
							{   
								m_ToolTips.SetBorderColor(0);
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,255,100,25));
							 
							    static const int value[5][3] = { {3000,2500,30000},{6000,5000,60000},{9000,7500,90000},{12000,10000,120000},{30000,15000,300000} };
								static const char* strSetName[] = { "20阶暗影宝石套装效果(10件)", "40阶暗影宝石套装效果(10件)", "60阶暗影宝石套装效果(10件)", "80阶暗影宝石套装效果(10件)", "100阶暗影宝石套装效果(10件)" };

								char szTemp[512];

								m_ToolTips.AddText( strSetName[m15_equipByMakerType] );
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
								Tips_AddNum1( "\n 双攻 +%u% ", value[m15_equipByMakerType][0] );
								Tips_AddNum1( " 双防 +%u% ", value[m15_equipByMakerType][1] );
								Tips_AddNum1( " 生命值 +%u% ", value[m15_equipByMakerType][2] );
								m_ToolTips.AddText( "\n\n" );
								stResourceLocation imguserinfo1;
			                    imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo1.group = 400;
		                        imguserinfo1.frame = 2;
		                        m_ToolTips.AddAnimation( &imguserinfo1, false );
								m_ToolTips.Resize();
							}
                            //soke 神器套装显示效果
							if ( (m6_equipByMakerType > EquipByMaker6_T_NONE) && (m6_equipByMakerType <= EquipByMaker67_T_WONDER_PURPLE))
							{  
								m_ToolTips.SetBorderColor(0);
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,255,100,25));

								static const int value[7][3] = { {3200,3200,32000},{6400,6400,64000},{12800,12800,128000},{19200,19200,192000},{25600,25600,256000},{32000,32000,320000},{50000,30000,500000} };
								static const char* strSetName[] = {"一阶神器套装效果(10件)", "二阶神器套装效果(10件)", "三阶神器套装效果(10件)","四阶神器套装效果(10件)", "五阶神器套装效果(10件)", "六阶神器套装效果(10件)", "七阶神器套装效果(10件)" };

								char szTemp[512];

								m_ToolTips.AddText( strSetName[m6_equipByMakerType] );
                                m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
								Tips_AddNum1( "\n 双攻 +%u% ", value[m6_equipByMakerType][0] );
								Tips_AddNum1( " 双防 +%u% ", value[m6_equipByMakerType][1] );
								Tips_AddNum1( " 生命值 +%u% ", value[m6_equipByMakerType][2] );
								//m_ToolTips.AddText("\n ");
								m_ToolTips.Resize();
								m_ToolTips.AddText("\n\n");
                                stResourceLocation imguserinfo1;
			                    imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo1.group = 400;
		                        imguserinfo1.frame = 2;
		                        m_ToolTips.AddAnimation( &imguserinfo1, false );
							}
					    	//百兽图鉴套装展示
							if ( (m22_equipByMakerType > EquipByMaker22_T_NONE) && (m22_equipByMakerType <= EquipByMaker22_T_HANDBOOK_RED))
							{
                                m_ToolTips.SetBorderColor(0);
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
							 
							    static const int value[7][3] = { {2000,1000,20000},{4000,2000,40000},{6000,3000,60000},{8000,4000,80000},{10000,5000,100000},{13000,7000,130000} ,{16000,9000,160000} };
								static const char* strSetName[] = { "白色图鉴套装效果(10件)", "蓝色图鉴套装效果(10件)", "黄色图鉴套装效果(10件)", "绿色图鉴套装效果(10件)", "紫色图鉴套装效果(10件)", "橙色图鉴套装效果(10件)", "红色图鉴套装效果(10件)" };

								char szTemp[512];
								
								m_ToolTips.AddText( strSetName[m22_equipByMakerType] );
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
								Tips_AddNum1( "\n 双攻 +%u% ", value[m22_equipByMakerType][0] );
								Tips_AddNum1( " 双防 +%u% ", value[m22_equipByMakerType][1] );
								Tips_AddNum1( " 生命值 +%u% ", value[m22_equipByMakerType][2] );
								m_ToolTips.AddText( "\n\n" );				
								stResourceLocation imguserinfo1;
			                    imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo1.group = 400;
		                        imguserinfo1.frame = 2;
		                        m_ToolTips.AddAnimation10( &imguserinfo1, false );
								m_ToolTips.Resize();
							}

							//龙凤吟套装展示
							if ( (m24_equipByMakerType > EquipByMaker24_T_NONE) && (m24_equipByMakerType <= EquipByMaker24_T_LFY_LV5))
							{
                                m_ToolTips.SetBorderColor(0);
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,0,255,0));
							 
							    static const int value[5][3] = { {160000,20000,300000},{200000,30000,400000},{240000,50000,600000},{280000,60000,800000},{320000,100000,1200000} };
								static const char* strSetName[] = { "八阶龙凤吟套装效果(10件)", "十阶龙凤吟套装效果(10件)", "十二阶龙凤吟套装效果(10件)", "十四阶龙凤吟套装效果(10件)", "十六阶龙凤吟套装效果(10件)"};

								char szTemp[512];
								
								m_ToolTips.AddText( strSetName[m24_equipByMakerType] );
								m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
								m_ToolTips.SetCurColor(COLOR_ARGB(255,244,249,168));
								Tips_AddNum1( "\n 双攻 +%u% ", value[m24_equipByMakerType][0] );
							
								m_ToolTips.AddText( "\n\n" );				
								stResourceLocation imguserinfo1;
			                    imguserinfo1.SetFileName( "data\\interfaces2.gl");
			                    imguserinfo1.group = 400;
		                        imguserinfo1.frame = 2;
		                        m_ToolTips.AddAnimation10( &imguserinfo1, false );
								m_ToolTips.Resize();
							}

							//soke 套装说明
							m_ToolTips.SetBorderColor(0);
							m_ToolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
							m_ToolTips.SetCurColor(COLOR_ARGB(255,255,0,0));
							// m_ToolTips.AddText2( "\n\n此效果持续至2024年12月31日"); //AddText2为小字体
							m_ToolTips.SetCurColor(COLOR_ARGB(255,86,255,0));				
							m_ToolTips.AddText("\n盾牌、战旗不计入套装系统");
							m_ToolTips.Resize();							
						}
						break;
					case 2:  //右手装备
						{

						}
						break;
					case 3:  //左手装备
						{

						}
						break;
					}
					break;
				}
			}
		}
	}
}



void CGuiUserInfo::ShowUserInfoText4()
{
	stPointI ptText ;
	int dy = 0;

	//if (m_bMainUser==false)
	//	return;
	
	// 未上传
	GetDevice()->SetFont(0);
	stRectI rect(90,127,351,144);
	rect.OffsetRect(m_x,m_y);
	GetDevice()->DrawString(avar("未上传",m_UserData.pMainUserData->level ),rect,0xffffffff,DT_VCENTER|DT_CENTER);

	GetDevice()->SetFont(0);

	ptText = stPointI(278,162) + GetLocation();

    //soke 人气
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->moods),ptText);

	dy=30;
	ptText.y += dy;

	// 鲜花
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->folwers),ptText);
	
	ptText.y += dy;

	// 爱心
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->flloves),ptText);

}

void CGuiUserInfo::ShowUserInfoText2()
{
	stPointI ptText ;
	int dy = 0;

	if (m_bMainUser==false)
		return;
	
    //soke 转生设置
	if(m_UserData.pMainUserData->round > 0)
	{
		if(m_UserData.pMainUserData->round == 1)
	    { 
	       stRectI rect(65,130,351,144);
	       rect.OffsetRect(m_x,m_y);
	       GetDevice()->DrawString(avar("【一转】"),rect,0xffefc400,DT_VCENTER|DT_CENTER);
		}
		else if(m_UserData.pMainUserData->round == 2)
	    { 
	       stRectI rect(65,130,351,144);
	       rect.OffsetRect(m_x,m_y);
	       GetDevice()->DrawString(avar("【二转】"),rect,0xffefc400,DT_VCENTER|DT_CENTER);
		}
		else if(m_UserData.pMainUserData->round == 3)
	    { 
	       stRectI rect(65,130,351,144);
	       rect.OffsetRect(m_x,m_y);
	       GetDevice()->DrawString(avar("【三转】"),rect,0xffefc400,DT_VCENTER|DT_CENTER);
		}
		else if(m_UserData.pMainUserData->round == 4)
	    { 
	       stRectI rect(65,130,351,144);
	       rect.OffsetRect(m_x,m_y);
	       GetDevice()->DrawString(avar("【四转】"),rect,0xffefc400,DT_VCENTER|DT_CENTER);
		}
		else if(m_UserData.pMainUserData->round == 5)
	    { 
	       stRectI rect(65,130,351,144);
	       rect.OffsetRect(m_x,m_y);
	       GetDevice()->DrawString(avar("【五转】"),rect,0xffefc400,DT_VCENTER|DT_CENTER);
		}
		else if(m_UserData.pMainUserData->round == 6)
	    { 
	       stRectI rect(65,130,351,144);
	       rect.OffsetRect(m_x,m_y);
	       GetDevice()->DrawString(avar("【六转】"),rect,0xffefc400,DT_VCENTER|DT_CENTER);
		}
		else if(m_UserData.pMainUserData->round == 7)
	    { 
	       stRectI rect(65,130,351,144);
	       rect.OffsetRect(m_x,m_y);
	       GetDevice()->DrawString(avar("【七转】"),rect,0xffefc400,DT_VCENTER|DT_CENTER);
		}
		else if(m_UserData.pMainUserData->round == 8)
	    { 
	       stRectI rect(65,130,351,144);
	       rect.OffsetRect(m_x,m_y);
	       GetDevice()->DrawString(avar("【八转】" ),rect,0xffefc400,DT_VCENTER|DT_CENTER);
		}
		else if(m_UserData.pMainUserData->round == 9)
	    { 
	       stRectI rect(65,130,351,144);
	       rect.OffsetRect(m_x,m_y);
	       GetDevice()->DrawString(avar("【九转】"),rect,0xffefc400,DT_VCENTER|DT_CENTER);
		}
		else if(m_UserData.pMainUserData->round >= 10)
	    { 
	       stRectI rect(65,130,351,144);
	       rect.OffsetRect(m_x,m_y);
	       GetDevice()->DrawString(avar("【十转】"),rect,0xffefc400,DT_VCENTER|DT_CENTER);
		}
	    // 等级 
	    stRectI rect1(164,130,351,144);
	    rect1.OffsetRect(m_x,m_y);
	    GetDevice()->DrawString(avar("%u级",m_UserData.pMainUserData->level ),rect1,0xffffffff,DT_VCENTER|DT_CENTER);
	}
	else
	{
		// 等级 
	    stRectI rect(113,130,351,144);
	    rect.OffsetRect(m_x,m_y);
	    GetDevice()->DrawString(avar("%u级",m_UserData.pMainUserData->level ),rect,0xffffffff,DT_VCENTER|DT_CENTER);
	}


	GetDevice()->SetFont(0);

	
	ptText = stPointI(124,167) + GetLocation();
	
	char szCountryName[MAX_NAMESIZE] = {0};
	CCountry::Instance()->GetCountryName(m_UserData.pMapUserData->country,szCountryName);
	GetDevice()->DrawString(avar("【%s】",szCountryName),ptText);

	dy=24;
	ptText.y += dy;
	GetDevice()->DrawString(GetGameGuiManager()->m_guiWorldConn->m_TongInfo.unionName,ptText);

	ptText.y += dy;
	GetDevice()->DrawString(GetGameGuiManager()->m_guiWorldConn->m_SeptInfo.septName,ptText);

	ptText.y += dy;
	//m_UserData.pMapUserData->exploit = 1000;
	//sprintf(m_UserData.pMapUserData->caption,"城主");
	GetDevice()->DrawString(GetKnightName(m_UserData.pMapUserData->exploit),ptText);

	ptText.y += dy;
	/*m_UserData.pMapUserData->grace = 1000;*/
	GetDevice()->DrawString(GetCivilianName(m_UserData.pMapUserData->grace),ptText);
	
	//m_UserData.pMapUserData->star = 0;

	ptText.y += dy;
	DWORD goodness = m_UserData.pMapUserData->goodness & GOODNESS_ATT ? 0 : m_UserData.pMapUserData->goodness;
	GetDevice()->DrawString(GetPkModeName(goodness),ptText,GetNameColor(goodness));


	

	ptText = stPointI(270,166) + GetLocation();

	//GetDevice()->DrawString(avar("%u",640 + (640 - m_UserData.pMapUserData->movespeed)),ptText);
	if (m_UserData.pMainUserData->fivetype<FIVE_NONE)
		GetDevice()->DrawString(c_szFiveName[m_UserData.pMainUserData->fivetype],ptText);

	dy=24;
	ptText.y += dy;
	
	if ( m_UserData.pMainUserData->changejob > 9 )
    {
	    GetDevice()->DrawString(avar("未知"),ptText,0xffffffff );
	}
	else if (m_UserData.pMainUserData->changejob == 9)
	{
		GetDevice()->DrawString(avar("全职业"),ptText,0xffb800ed );
	}
	else if (m_UserData.pMainUserData->changejob == 8)
	{
		GetDevice()->DrawString(avar("枪系"),ptText,0xff00ff00 );
	}
	else if (m_UserData.pMainUserData->changejob == 7)
	{
		GetDevice()->DrawString(avar("卫士"),ptText,0xff00ff00 );
	}
	else if (m_UserData.pMainUserData->changejob == 6)
	{
		GetDevice()->DrawString(avar("刺客"),ptText,0xff00ff00 );
	}
	else if (m_UserData.pMainUserData->changejob == 5)
	{
		GetDevice()->DrawString(avar("天仙"),ptText,0xff00ff00 );
	}
	else if (m_UserData.pMainUserData->changejob == 4)
	{
		GetDevice()->DrawString(avar("召唤"),ptText,0xff00ff00 );
	}
	else if (m_UserData.pMainUserData->changejob == 3)
	{
		GetDevice()->DrawString(avar("法师"),ptText,0xff00ff00 );
	}
	else if (m_UserData.pMainUserData->changejob == 2)
	{
		GetDevice()->DrawString(avar("弓手"),ptText,0xff00ff00 );
	}
	else if (m_UserData.pMainUserData->changejob == 1)
	{
		GetDevice()->DrawString(avar("战士"),ptText,0xff00ff00 );
	}
	else if (m_UserData.pMainUserData->changejob == 0)
	{
		GetDevice()->DrawString(avar("无职业"),ptText,0xffffffff );
	}
	
//	if (m_UserData.pMainUserData->fivetype<FIVE_NONE)
//		GetDevice()->DrawString(avar("%s %d级",c_szFiveToOtherName[m_UserData.pMainUserData->fivetype],m_UserData.pMainUserData->fivepoint),ptText);

    ptText = stPointI(260,191) + GetLocation(); 
	ptText.y += dy;
	GetDevice()->DrawString(avar("%u",m_UserData.pMapUserData->exploit),ptText);

	ptText.y += dy;
	GetDevice()->DrawString(avar("%u",m_UserData.pMapUserData->grace),ptText);

	ptText.y += dy;
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->lucky),ptText);

	ptText.y += dy;
	GetDevice()->DrawString(avar("%u / %u",m_UserData.pMainUserData->honor,m_UserData.pMainUserData->maxhonor),ptText);




	ptText = stPointI(145,333) + GetLocation();

	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->points),ptText,0xffffffff );

    ptText = stPointI(125,339) + GetLocation();
	dy=20;
	//ptText.x=138;
	ptText.y += dy;
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->wdStr),ptText,GETSHOWCOLOR(m_UserData.pMainUserData->wdStr,m_UserData.pMainUserData->wdStdStr));

	ptText.y += dy;
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->wdInt),ptText,GETSHOWCOLOR(m_UserData.pMainUserData->wdInt,m_UserData.pMainUserData->wdStdInt));

	ptText.y += dy;
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->wdDex),ptText,GETSHOWCOLOR(m_UserData.pMainUserData->wdDex,m_UserData.pMainUserData->wdStdDex));

	ptText.y += dy;
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->wdMen),ptText,GETSHOWCOLOR(m_UserData.pMainUserData->wdMen,m_UserData.pMainUserData->wdStdMen));

	ptText.y += dy;
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->wdCon),ptText,GETSHOWCOLOR(m_UserData.pMainUserData->wdCon,m_UserData.pMainUserData->wdStdCon));

	ptText.y += dy;
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->bang),ptText,GETSHOWCOLOR(m_UserData.pMainUserData->bang,m_UserData.pMainUserData->stdbang) );


	ptText = stPointI(270,333) + GetLocation();

	if ((m_LevelNum!=0)&&(m_LevelNum!=(WORD)-1))
		GetDevice()->DrawString(avar("%u",m_LevelNum),ptText,0xffffffff );

	ptText = stPointI(270,338) + GetLocation();
	dy = 20;
	ptText.x -= 30;
	ptText.y += dy;

	//soke 如果是魔攻职业，那么属性界面的物理攻击灰色显示
	if (m_UserData.pMainUserData->changejob==0 || m_UserData.pMainUserData->changejob==3 || m_UserData.pMainUserData->changejob==4 || m_UserData.pMainUserData->changejob==5|| m_UserData.pMainUserData->changejob==6)
	{
	    // 物理攻击
	    GetDevice()->DrawString(avar("%u - %u",m_UserData.pMainUserData->pdamage,m_UserData.pMainUserData->maxpdamage),ptText,GETSHOWCOLOR2(m_UserData.pMainUserData->pdamage,m_UserData.pMainUserData->stdpdamage));
	}
	else
	{
		// 物理攻击
	    GetDevice()->DrawString(avar("%u - %u",m_UserData.pMainUserData->pdamage,m_UserData.pMainUserData->maxpdamage),ptText,GETSHOWCOLOR(m_UserData.pMainUserData->pdamage,m_UserData.pMainUserData->stdpdamage));
	}
	ptText.y += dy;
	//soke 如果是物攻职业，那么属性界面的魔法攻击灰色显示
	if (m_UserData.pMainUserData->changejob==0 || m_UserData.pMainUserData->changejob==1 || m_UserData.pMainUserData->changejob==2)
	{
	   // 魔法攻击
	   GetDevice()->DrawString(avar("%u - %u",m_UserData.pMainUserData->mdamage,m_UserData.pMainUserData->maxmdamage),ptText,GETSHOWCOLOR2(m_UserData.pMainUserData->mdamage,m_UserData.pMainUserData->stdmdamage));
	}
	else
	{
	   // 魔法攻击
	   GetDevice()->DrawString(avar("%u - %u",m_UserData.pMainUserData->mdamage,m_UserData.pMainUserData->maxmdamage),ptText,GETSHOWCOLOR(m_UserData.pMainUserData->mdamage,m_UserData.pMainUserData->stdmdamage));
	}
	ptText.y += dy;

	// 物理防御
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->pdefence),ptText,GETSHOWCOLOR(m_UserData.pMainUserData->pdefence,m_UserData.pMainUserData->stdpdefence));
	ptText.y += dy;
	// 魔法防御
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->mdefence),ptText,GETSHOWCOLOR(m_UserData.pMainUserData->mdefence,m_UserData.pMainUserData->stdmdefence));
	ptText.y += dy;
	// 生命
	if ( m_UserData.pMainUserData->maxhp < 10000)
    {
	   GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->maxhp),ptText,0xffffffff );
	}
	else
	{
	   GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->maxhp),ptText,0xff00ff00 );
	}

	ptText = stPointI(250,338) + GetLocation();
	dy = 121;
	//ptText.x -= 30;
	ptText.y += dy;

	// 战力
	char zhanli[256];
	int nZhanli = m_UserData.pMainUserData->maxpdamage + m_UserData.pMainUserData->maxmdamage + m_UserData.pMainUserData->pdefence + m_UserData.pMainUserData->mdefence;
	int nZhanli2 = nZhanli/18;

	if ( nZhanli2 <= 1)
    {
	    sprintf( zhanli, " 0" );
	}
	else if ( nZhanli2 > 1 )
    {
	    sprintf( zhanli, " %d", nZhanli2 );
	}

	stPointF CommScale(0.8f,0.8f);

	GetBxmpTextout()->DrawString(ptText,zhanli,strlen(zhanli),0xffffffff,&CommScale, Blend_Null);

	/*
	// 生命值
	GetDevice()->DrawString(avar("%u/%u",pMainChar->GetHP(),pMainChar->GetMaxHP()),ptText);
	ptText.y += dy;
	// 法术值
	GetDevice()->DrawString(avar("%u/%u",pMainChar->GetMP(),pMainChar->GetMaxMP()),ptText);
	ptText.y += dy;
	// 体力值
	GetDevice()->DrawString(avar("%u/%u",pMainChar->GetSP(),pMainChar->GetMaxSP()),ptText);
	ptText.y += dy;

	// 命中率
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->attackrating),ptText);
	ptText.y += 12;
	// 躲避率
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->attackdodge),ptText );
	ptText.y += 12;
	*/
	
}

//sky 换装界面选中的框
void CGuiUserInfo::ShowUserInfoText3()
{
	//Draw skill select sign
	CGuiButton* btn = GetButton(11);
	if(btn)
	{
		if ( m_usedIndex == getActiveIndex())
		{
			btn->SetBitmap(stResourceLocation("data\\Interfaces.gl",3,67),3);
		}
		else
		{
			btn->SetBitmap(stResourceLocation("data\\Interfaces.gl",3,64),3);
		}
	}
}

void CGuiUserInfo::ShowUserInfoText1()
{
	stPointI ptText ;

	
	if (m_bMainUser==false)
		return;

	GetDevice()->SetFont(0);
	ptText = stPointI(100,454) + GetLocation();

	//soke  如果是魔攻职业，那么装备界面的物理攻击灰色显示
	if (m_UserData.pMainUserData->changejob==0 || m_UserData.pMainUserData->changejob==3 || m_UserData.pMainUserData->changejob==4 || m_UserData.pMainUserData->changejob==5 || m_UserData.pMainUserData->changejob==6)
	{
	   // 物理攻击
	   GetDevice()->DrawString(avar("%u - %u",m_UserData.pMainUserData->pdamage,m_UserData.pMainUserData->maxpdamage),ptText,GETSHOWCOLOR2(m_UserData.pMainUserData->pdamage,m_UserData.pMainUserData->stdpdamage));
	
	}
	else
	{
	   // 物理攻击
	   GetDevice()->DrawString(avar("%u - %u",m_UserData.pMainUserData->pdamage,m_UserData.pMainUserData->maxpdamage),ptText,GETSHOWCOLOR(m_UserData.pMainUserData->pdamage,m_UserData.pMainUserData->stdpdamage));
	}
	ptText.y += 16;
	//soke 如果是物攻职业，那么装备界面的物魔法击灰色显示
	if (m_UserData.pMainUserData->changejob==0 || m_UserData.pMainUserData->changejob==1 || m_UserData.pMainUserData->changejob==2)
	{
	   // 魔法攻击
	   GetDevice()->DrawString(avar("%u - %u",m_UserData.pMainUserData->mdamage,m_UserData.pMainUserData->maxmdamage),ptText,GETSHOWCOLOR2(m_UserData.pMainUserData->mdamage,m_UserData.pMainUserData->stdmdamage));
    }
	else
	{
	   // 魔法攻击
	   GetDevice()->DrawString(avar("%u - %u",m_UserData.pMainUserData->mdamage,m_UserData.pMainUserData->maxmdamage),ptText,GETSHOWCOLOR(m_UserData.pMainUserData->mdamage,m_UserData.pMainUserData->stdmdamage));
    }

	ptText = stPointI(275,454) + GetLocation();
	
	// 物理防御
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->pdefence),ptText,GETSHOWCOLOR(m_UserData.pMainUserData->pdefence,m_UserData.pMainUserData->stdpdefence));
	ptText.y += 16;
	// 魔法防御
	GetDevice()->DrawString(avar("%u",m_UserData.pMainUserData->mdefence),ptText,GETSHOWCOLOR(m_UserData.pMainUserData->mdefence,m_UserData.pMainUserData->stdmdefence));

}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pTable : 描述
 * \param ptRender : 描述
 * \return 返回值的描述
 */
void CGuiUserInfo::BodyRender(stPointI ptRender)
{
	FUNCTION_BEGIN;
	m_pImage[1]->SetVisible(true); //修复指定时装id C界面不显示武器与盾牌【西游时装专用】
	m_pImage[2]->SetVisible(true); //修复指定时装id C界面不显示武器与盾牌【西游时装专用】
	m_pImage[3]->SetVisible(true);
	m_pImage[4]->SetVisible(true);
	static stPointI pos(0,0);
	
	//if(pTable->IsVisible())
	{
		//if(!pTable->GetItemCell(0,0))
		if(m_bShowBody)
		{
			for(size_t i=0;i<count_of(m_aniBody);++i)
			{
				{
					if(i == 1) 
						m_aniBody[i].SetColor(m_UserData.pMapUserData->dwBodyColorSystem);
					else if(i==2)
						m_aniBody[i].SetColor(m_UserData.pMapUserData->dwBodyColorCustom | 0xff000000);
					else m_aniBody[i].SetColor(-1);
					m_aniBody[i].SetBlendType(Blend_Null);
				}
				stPointI pt = pos;
				pt.x += ptRender.x;
				pt.y += ptRender.y;
				//m_aniBody[i].SetColor();
				m_aniBody[i].Render(pt.x,pt.y);
			}
		}
	}
    //修复指定时装id C界面不显示武器与盾牌【西游时装专用】
	if((m_UserData.pMapUserData->sculpt.dwBodyID >= 200158 && m_UserData.pMapUserData->sculpt.dwBodyID <= 200162) || (m_UserData.pMapUserData->sculpt.dwBodyID >= 210158 && m_UserData.pMapUserData->sculpt.dwBodyID <= 210162))
		{  
			m_pImage[1]->SetVisible(false);
			m_pImage[2]->SetVisible(false);
		}

    //soke 125类型时装不显示纸娃娃头发
	if(m_UserData.pMapUserData->sculpt.dwBodyKind != ItemType_XHighFashionBody)
	{
	    if(!(m_UserData.pMapUserData->sculpt.dwBodyID == 10030 || m_UserData.pMapUserData->sculpt.dwBodyID == 19080 || m_UserData.pMapUserData->sculpt.dwBodyID == 19130 
		|| m_UserData.pMapUserData->sculpt.dwBodyID == 19180 || m_UserData.pMapUserData->sculpt.dwBodyID == 19230 || m_UserData.pMapUserData->sculpt.dwBodyID == 19280))
        {  
			if(m_UserData.pMapUserData->sculpt.dwHairID != 0)
	        {
		         const stCharTypeInfo& info = GetCharTypeInfo(m_UserData.pMapUserData->type);
		         HairStyle_t* p = GetHairStyle(m_UserData.pMapUserData->sculpt.dwHairID);
		         if(p)
		         {
		   	         stResourceLocation rl("data\\items.gl",info.equipHairPic,p->dwEquipPicID);
			         IBitmaps* pBmps = GetDevice()->FindBitmaps(&rl);
			        if(pBmps)
			        {
			     	     stPointI pt = pos;
			     	     pt.x += ptRender.x;
			        	 pt.y += ptRender.y;
			     	     pBmps->RenderAni(pt.x,pt.y,xtimeGetTime(),m_UserData.pMapUserData->dwHairRGB | 0xff000000);
			        }
		        }
	        }
		}
		else
		{

		}	
	}
	else 
	{
		

	}

	FUNCTION_END;
}


bool CGuiUserInfo::IsActiveItem(CRoleItem* pItem)
{
	for (int i=0;i<m_listActiveItem.size();i++)
	{
		if ((m_listActiveItem[i]==pItem)&&IsShowActiveItem(pItem))
			return true;
	}

	return false;
}


void CGuiUserInfo::SetEquipByName( CRoleItem* pItem, bool bMainCharacter /* = true */ )
{
	if (pItem==NULL)
		return;

	CGuiFloatItem* pFloatItem = GetGuiManager()->GetFloatItem();
	
	//soke 盾牌、箭支、护心镜、短枪、战旗不计入套装系统
	if ((pItem->GetItemType()==ItemType_Shield)||(pItem->GetItemType()==ItemType_Arrow)||(pItem->GetItemType()==ItemType_breastplate)||(pItem->GetItemType()==ItemType_Handgun)||(pItem->GetItemType()>=ItemType_StaffFlag && pItem->GetItemType()<=ItemType_SwordFlag))
		return;

	if ( NULL == m_UserData.pMapUserData )
		return;
	if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ( pItem->GetObject()->bind == 1 )
		{
			if ( pItem->GetObject()->kind & 16 )
			{
				(*itor).numSetList.numPurple++ ;
			}
			else if ( pItem->GetObject()->kind & 4 )
			{
				(*itor).numSetList.numHoly++ ;
			}
			else if ( pItem->GetObject()->kind & 2 )
			{
				(*itor).numSetList.numGlod++ ;
			}
			else if ( pItem->GetObject()->kind & 1 )
			{
				(*itor).numSetList.numBlue++ ;
			}
		}

		enmEuipByMakerType typeWonder = EquipByMaker_T_NONE;
		enmEuipByMakerType typeBug = EquipByMaker_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;
            if ( (*itor).numSetList.numPurple >= 10)
			{
				if ( typeWonder < EquipByMaker_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numPurple >= 2 )
			{ 
				if ( typeBug < EquipByMaker_T_BUG_PURPLE )
				{
					typeBug = EquipByMaker_T_BUG_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numHoly >= 10)
			{
				if ( typeWonder < EquipByMaker_T_WONDER_HOLY )
				{
					typeWonder = EquipByMaker_T_WONDER_HOLY;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numHoly >= 6 )
			{
				if ( typeBug < EquipByMaker_T_BUG_HOLY )
				{
					typeBug = EquipByMaker_T_BUG_HOLY;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numGlod >= 10)
			{
				if ( typeWonder < EquipByMaker_T_WONDER_GOLD )
				{
					typeWonder = EquipByMaker_T_WONDER_GOLD;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numGlod >= 6 )
			{
				if ( typeBug < EquipByMaker_T_BUG_GOLD )
				{
					typeBug = EquipByMaker_T_BUG_GOLD;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numBlue >= 10)
			{
				if ( typeWonder < EquipByMaker_T_WONDER_BLUE )
				{
					typeWonder = EquipByMaker_T_WONDER_BLUE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numBlue >= 6 )
			{
				if ( typeBug < EquipByMaker_T_BUG_BLUE )
				{
					typeBug = EquipByMaker_T_BUG_BLUE;
					bGetNewSet = true;
				}
			}

			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker_T_NONE )
			m_equipByMakerType = typeWonder;			
		else if ( typeBug > EquipByMaker_T_NONE )
			m_equipByMakerType = typeBug;
		else
			m_equipByMakerType = EquipByMaker_T_NONE;
	}

	//soke 是15星绿装、紫装显示
	if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ( pItem->GetObject()->bind == 1 )
		{
			if ( (pItem->GetObject()->kind & 4 || !(pItem->GetObject()->kind & 16)) && pItem->GetObject()->upgrade >= 15 )
			{
				(*itor).numSetList.numGreen++ ;
			}
			if ( pItem->GetObject()->kind & 16 )
			{
				(*itor).numSetList.num2Purple++ ;
			}
		}

		enm1EuipByMakerType typeWonder = EquipByMaker1_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;

			if ( (*itor).numSetList.num2Purple >= 10)
			{
				if ( typeWonder < EquipByMaker1_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker1_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numGreen >= 10)
			{
				if ( typeWonder < EquipByMaker1_T_WONDER_GREEN )
				{
					typeWonder = EquipByMaker1_T_WONDER_GREEN;
					bGetNewSet = true;
				}
			}
			
			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker1_T_NONE )
			m1_equipByMakerType = typeWonder;			
		else
			m1_equipByMakerType = EquipByMaker1_T_NONE;
	}

	//soke  装备套装显示
	if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ( pItem->GetObject()->bind == 1 )
		{
			if ( (pItem->GetObject()->kind & 4 || pItem->GetObject()->kind & 16) && pItem->GetObject()->needlevel >=220 ) //桃子 永恒 等级显示限制
			{
				(*itor).numSetList.num220Purple++ ;
			}
			if ( (pItem->GetObject()->kind & 4 || pItem->GetObject()->kind & 16) && pItem->GetObject()->needlevel >=215 ) //桃子 弑天 等级显示限制
			{
				(*itor).numSetList.num215Purple++ ;
			}
			if ( (pItem->GetObject()->kind & 4 || pItem->GetObject()->kind & 16) && pItem->GetObject()->needlevel >=210 ) //桃子 帝王 等级显示限制
			{
				(*itor).numSetList.num210Purple++ ;
			}
			if ( (pItem->GetObject()->kind & 4 || pItem->GetObject()->kind & 16) && pItem->GetObject()->needlevel >=205 ) //桃子 战神 等级显示限制
			{
				(*itor).numSetList.num205Purple++ ;
			}
			if ( (pItem->GetObject()->kind & 4 || pItem->GetObject()->kind & 16) && pItem->GetObject()->needlevel >=202 ) //桃子 圣者 等级显示限制
			{
				(*itor).numSetList.num202Purple++ ;
			}			
			if ( (pItem->GetObject()->kind & 4 || pItem->GetObject()->kind & 16) && pItem->GetObject()->needlevel >=197 ) //桃子 英雄 等级显示限制
			{
				(*itor).numSetList.num197Purple++ ;
			}
			if ( (pItem->GetObject()->kind & 4 || pItem->GetObject()->kind & 16) && pItem->GetObject()->needlevel >=190 ) //桃子 天尊 等级显示限制
			{
				(*itor).numSetList.num190Purple++ ;
			}
			if ( (pItem->GetObject()->kind & 4 || pItem->GetObject()->kind & 16) && pItem->GetObject()->needlevel >=185 ) //桃子 传说 等级显示限制
			{
				(*itor).numSetList.num185Purple++ ;
			}
			if ( (pItem->GetObject()->kind & 4 || pItem->GetObject()->kind & 16) && pItem->GetObject()->needlevel >=180 ) //桃子 卓越 等级显示限制
			{
				(*itor).numSetList.num180Purple++ ;
			}
			if ( (pItem->GetObject()->kind & 4 || pItem->GetObject()->kind & 16) && pItem->GetObject()->needlevel >=175 ) //桃子 轩辕 等级显示限制
			{
				(*itor).numSetList.num175Purple++ ;
			}
			if ( (pItem->GetObject()->kind & 4 || pItem->GetObject()->kind & 16) && pItem->GetObject()->needlevel >=170 ) //桃子 炙天 等级显示限制
			{
				(*itor).numSetList.num170Purple++ ;
			}
			if ( (pItem->GetObject()->kind & 4 || pItem->GetObject()->kind & 16) && pItem->GetObject()->needlevel >=165 ) //桃子 乾坤套 等级显示限制
			{
				(*itor).numSetList.num165Purple++ ;
			}
		}

		enm11EuipByMakerType typeWonder = EquipByMaker11_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;

		
			if ( (*itor).numSetList.num220Purple >= 10) //桃子 永恒装备10件
			{
				if ( typeWonder < EquipByMaker220_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker220_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num215Purple >= 10) //桃子 弑天装备10件
			{
				if ( typeWonder < EquipByMaker215_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker215_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}			
			else if ( (*itor).numSetList.num210Purple >= 10) //桃子 帝王装备10件
			{
				if ( typeWonder < EquipByMaker210_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker210_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}			
			else if ( (*itor).numSetList.num205Purple >= 10) //桃子 战神装备10件
			{
				if ( typeWonder < EquipByMaker205_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker205_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num202Purple >= 10) //桃子 圣者装备10件
			{
				if ( typeWonder < EquipByMaker202_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker202_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}			
			else if ( (*itor).numSetList.num197Purple >= 10) //桃子 英雄装备10件
			{
				if ( typeWonder < EquipByMaker197_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker197_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num190Purple >= 10) //桃子 天尊装备10件
			{
				if ( typeWonder < EquipByMaker190_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker190_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num185Purple >= 10) //桃子 传说装备10件
			{
				if ( typeWonder < EquipByMaker185_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker185_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num180Purple >= 10) //桃子 卓越装备10件
			{
				if ( typeWonder < EquipByMaker180_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker180_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num175Purple >= 10) //桃子 轩辕装备10件
			{
				if ( typeWonder < EquipByMaker175_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker175_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num170Purple >= 10) //桃子 炙天装备10件
			{
				if ( typeWonder < EquipByMaker170_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker170_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num165Purple >= 10) //桃子 乾坤装备10件
			{
				if ( typeWonder < EquipByMaker165_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker165_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			
			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker11_T_NONE )
			m11_equipByMakerType = typeWonder;			
		else
			m11_equipByMakerType = EquipByMaker11_T_NONE;
	}

	//soke 星星套装系统
	if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ( pItem->GetObject()->kind & 16 && pItem->GetObject()->bind == 1 && !(pItem->Get2ObjectID() >= 130 && pItem->Get2ObjectID() <= 134) )
		{
			if ( pItem->GetObject()->upgrade >= 15 )
			{
				(*itor).numSetList.num15Purple++ ;
			}
			if ( pItem->GetObject()->upgrade >= 14 )
			{
				(*itor).numSetList.num14Purple++ ;
			}
			if ( pItem->GetObject()->upgrade >= 13 )
			{
				(*itor).numSetList.num13Purple++ ;
			}
			if ( pItem->GetObject()->upgrade >= 12 )
			{
				(*itor).numSetList.num12Purple++ ;
			}
			if ( pItem->GetObject()->upgrade >= 10 )
			{
				(*itor).numSetList.num10Purple++ ;
			}
			if ( pItem->GetObject()->upgrade >= 8 )
			{
				(*itor).numSetList.num8Purple++ ;
			}
			if ( pItem->GetObject()->upgrade >= 5 )
			{
				(*itor).numSetList.num5Purple++ ;
			}
			if ( pItem->GetObject()->upgrade >= 3 )
			{
				(*itor).numSetList.num3Purple++ ;
			}	
		}

		enm2EuipByMakerType typeWonder = EquipByMaker2_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;

			if ( (*itor).numSetList.num15Purple >= 10)
			{
				if ( typeWonder < EquipByMaker15_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker15_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num14Purple >= 10)
			{
				if ( typeWonder < EquipByMaker14_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker14_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num13Purple >= 10)
			{
				if ( typeWonder < EquipByMaker13_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker13_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num12Purple >= 10)
			{
				if ( typeWonder < EquipByMaker12_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker12_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num10Purple >= 10)
			{
				if ( typeWonder < EquipByMaker10_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker10_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num8Purple >= 10)
			{
				if ( typeWonder < EquipByMaker8_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker8_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num5Purple >= 10)
			{
				if ( typeWonder < EquipByMaker5_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker5_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
            else if ( (*itor).numSetList.num3Purple >= 10)
			{
				if ( typeWonder < EquipByMaker3_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker3_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
	
			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker2_T_NONE )
			m2_equipByMakerType = typeWonder;			
		else
			m2_equipByMakerType = EquipByMaker2_T_NONE;
	}

	//soke 第二灵魂系统
	if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ( pItem->GetObject()->kind & 16 && pItem->GetObject()->bind == 1 )
		{
			if ( pItem->GetObject()->addpdef == 60 ) //物理防御力增强
			{
				(*itor).numSetList.num21Purple++ ;
			}
			else if ( pItem->GetObject()->addmdef == 60 ) //魔法防御力增强
			{
				(*itor).numSetList.num22Purple++ ;
			}
			else if ( pItem->GetObject()->addpdam == 120 ) //增加物理攻击力
			{
				(*itor).numSetList.num23Purple++ ;
			}
			else if ( pItem->GetObject()->addmdam == 120 ) //增加魔法攻击力
			{
				(*itor).numSetList.num24Purple++ ;
			}
			else if ( pItem->GetObject()->addhp = 100 ) //增加生命值
			{
				(*itor).numSetList.num25Purple++ ;
			}
		}

		enm21EuipByMakerType typeWonder = EquipByMaker21_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;

			if ( (*itor).numSetList.num21Purple >= 8) //物理防御力增强
			{
				if ( typeWonder < EquipByMaker21_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker21_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num22Purple >= 8) //魔法防御力增强
			{
				if ( typeWonder < EquipByMaker22_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker22_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num23Purple >= 8) //增加物理攻击力
			{
				if ( typeWonder < EquipByMaker23_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker23_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num24Purple >= 8) //增加魔法攻击力
			{
				if ( typeWonder < EquipByMaker24_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker24_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num25Purple >= 8) //增加生命值
			{
				if ( typeWonder < EquipByMaker25_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker25_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
	
			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker21_T_NONE )
			m21_equipByMakerType = typeWonder;			
		else
			m21_equipByMakerType = EquipByMaker21_T_NONE;
	}

		//soke 斗魂套装系统
	if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ( pItem->GetObject()->kind & 16 && pItem->GetObject()->bind == 1 && !(pItem->Get2ObjectID() >= 130 && pItem->Get2ObjectID() <= 134) )
		{
			if ( pItem->GetObject()->douhun >= 15 )    // myy 斗魂满足3-15星判断 显示
			{
				(*itor).numSetList.num104Purple++ ;
			}
			if ( pItem->GetObject()->douhun >= 12 ) 
		 	{
		 		(*itor).numSetList.num103Purple++ ;
		 	}
			if ( pItem->GetObject()->douhun >= 9 )
		 	{
		 		(*itor).numSetList.num102Purple++ ;
		 	}
			if ( pItem->GetObject()->douhun >= 6 )
		 	{
		 		(*itor).numSetList.num101Purple++ ;
		 	}
		 	if ( pItem->GetObject()->douhun >= 3 )
		 	{
		 		(*itor).numSetList.num100Purple++ ;
		 	}
			if ( pItem->GetObject()->douhun >= 1 )
		 	{
		 		(*itor).numSetList.num001Purple++ ;
		 	}
		}

		enm3EuipByMakerType typeWonder = EquipByMaker3_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;

			if ( (*itor).numSetList.num104Purple >= 10)
			{
				if ( typeWonder < EquipByMaker104_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker104_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num103Purple >= 10)
			{
				if ( typeWonder < EquipByMaker103_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker103_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num102Purple >= 10)
			{
				if ( typeWonder < EquipByMaker102_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker102_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num101Purple >= 10)
			{
				if ( typeWonder < EquipByMaker101_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker101_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.num100Purple >= 10)
			{
				if ( typeWonder < EquipByMaker100_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker100_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			
			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker3_T_NONE )
			m3_equipByMakerType = typeWonder;			
		else
			m3_equipByMakerType = EquipByMaker3_T_NONE;
	}

	//soke 龙星套装系统
	if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ( pItem->GetObject()->kind & 16 && pItem->GetObject()->bind == 1 && !(pItem->Get2ObjectID() >= 130 && pItem->Get2ObjectID() <= 134) )
		{
			if ( pItem->GetObject()->drastar >= 15 )
			{
				(*itor).numSetList.numx15Purple++ ;
			}
			if ( pItem->GetObject()->drastar >= 13 )
			{
				(*itor).numSetList.numx13Purple++ ;
			}
			if ( pItem->GetObject()->drastar >= 11 )
			{
				(*itor).numSetList.numx11Purple++ ;
			}
			if ( pItem->GetObject()->drastar >= 8 )
			{
				(*itor).numSetList.numx8Purple++ ;
			}
			if ( pItem->GetObject()->drastar >= 5 )
			{
				(*itor).numSetList.numx5Purple++ ;
			}	
		}

		enm4EuipByMakerType typeWonder = EquipByMaker4_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;

			if ( (*itor).numSetList.numx15Purple >= 10)
			{
				if ( typeWonder < EquipByMaker415_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker415_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numx13Purple >= 10)
			{
				if ( typeWonder < EquipByMaker413_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker413_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numx11Purple >= 10)
			{
				if ( typeWonder < EquipByMaker411_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker411_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numx8Purple >= 10)
			{
				if ( typeWonder < EquipByMaker48_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker48_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numx5Purple >= 10)
			{
				if ( typeWonder < EquipByMaker45_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker45_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
	
			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker4_T_NONE )
			m4_equipByMakerType = typeWonder;			
		else
			m4_equipByMakerType = EquipByMaker4_T_NONE;
	}

	//soke 补天套装系统
	if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ( pItem->GetObject()->kind & 16 && pItem->GetObject()->bind == 1 && !(pItem->Get2ObjectID() >= 130 && pItem->Get2ObjectID() <= 134) )
		{
			if ( pItem->GetObject()->butian >= 5 )
			{
				(*itor).numSetList.numbt5Purple++ ;
			}
			if ( pItem->GetObject()->butian >= 4 )
			{
				(*itor).numSetList.numbt4Purple++ ;
			}
			if ( pItem->GetObject()->butian >= 3 )
			{
				(*itor).numSetList.numbt3Purple++ ;
			}
			if ( pItem->GetObject()->butian >= 2 )
			{
				(*itor).numSetList.numbt2Purple++ ;
			}
			if ( pItem->GetObject()->butian >= 1 )
			{
				(*itor).numSetList.numbt1Purple++ ;
			}	
		}

		enm7EuipByMakerType typeWonder = EquipByMaker7_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;

			if ( (*itor).numSetList.numbt5Purple >= 10)
			{
				if ( typeWonder < EquipByMaker75_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker75_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numbt4Purple >= 10)
			{
				if ( typeWonder < EquipByMaker74_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker74_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numbt3Purple >= 10)
			{
				if ( typeWonder < EquipByMaker73_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker73_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numbt2Purple >= 10)
			{
				if ( typeWonder < EquipByMaker72_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker72_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numbt1Purple >= 10)
			{
				if ( typeWonder < EquipByMaker71_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker71_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}

			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker7_T_NONE )
			m7_equipByMakerType = typeWonder;			
		else
			m7_equipByMakerType = EquipByMaker7_T_NONE;
	}

	//soke 装备栏套装系统
	if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ( pItem->GetObject()->kind & 16 && pItem->GetObject()->bind == 1 && !(pItem->Get2ObjectID() >= 130 && pItem->Get2ObjectID() <= 134) )
		{
			if ( pItem->GetObject()->retain2 >= 6 )
			{
				(*itor).numSetList.numzb6Purple++ ;
			}
			if ( pItem->GetObject()->retain2 >= 5 )
			{
				(*itor).numSetList.numzb5Purple++ ;
			}
			if ( pItem->GetObject()->retain2 >= 4 )
			{
				(*itor).numSetList.numzb4Purple++ ;
			}
			if ( pItem->GetObject()->retain2 >= 3 )
			{
				(*itor).numSetList.numzb3Purple++ ;
			}
			if ( pItem->GetObject()->retain2 >= 2 )
			{
				(*itor).numSetList.numzb2Purple++ ;
			}	
		}

		enm5EuipByMakerType typeWonder = EquipByMaker5_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;

			if ( (*itor).numSetList.numzb6Purple >= 10)
			{
				if ( typeWonder < EquipByMaker56_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker56_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numzb5Purple >= 10)
			{
				if ( typeWonder < EquipByMaker55_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker55_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numzb4Purple >= 10)
			{
				if ( typeWonder < EquipByMaker54_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker54_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numzb3Purple >= 10)
			{
				if ( typeWonder < EquipByMaker53_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker53_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numzb2Purple >= 10)
			{
				if ( typeWonder < EquipByMaker52_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker52_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}

			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker5_T_NONE )
			m5_equipByMakerType = typeWonder;			
		else
			m5_equipByMakerType = EquipByMaker5_T_NONE;
	}
//soke 神龙装备镶嵌套装套装系统
	if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ( pItem->GetObject()->kind & 16 && pItem->GetObject()->bind == 1 && !(pItem->Get2ObjectID() >= 130 && pItem->Get2ObjectID() <= 134) )
		{
			if ( pItem->GetObject()->longwanglv >= 6 )
			{
				(*itor).numSetList.numshenlong6++ ;
			}
            if ( pItem->GetObject()->longwanglv >= 5 )
			{
				(*itor).numSetList.numshenlong5++ ;
			}
			if ( pItem->GetObject()->longwanglv >= 4 )
			{
				(*itor).numSetList.numshenlong4++ ;
			}
			if ( pItem->GetObject()->longwanglv >= 3 )
			{
				(*itor).numSetList.numshenlong3++ ;
			}
			if ( pItem->GetObject()->longwanglv >= 2 )
			{
				(*itor).numSetList.numshenlong2++ ;
			}
			if ( pItem->GetObject()->longwanglv >= 1 )
			{
				(*itor).numSetList.numshenlong1++ ;
			}	
		}

		enm12EuipByMakerType typeWonder = EquipByMaker12_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;

			if ( (*itor).numSetList.numshenlong6 >= 10)
			{
				if ( typeWonder < EquipByMaker12_T_SHENLONG_LV6 )
				{
					typeWonder = EquipByMaker12_T_SHENLONG_LV6;
					bGetNewSet = true;
				}
			}
            else if ( (*itor).numSetList.numshenlong5 >= 10)
			{
				if ( typeWonder < EquipByMaker12_T_SHENLONG_LV5 )
				{
					typeWonder = EquipByMaker12_T_SHENLONG_LV5;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numshenlong4 >= 10)
			{
				if ( typeWonder < EquipByMaker12_T_SHENLONG_LV4 )
				{
					typeWonder = EquipByMaker12_T_SHENLONG_LV4;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numshenlong3 >= 10)
			{
				if ( typeWonder < EquipByMaker12_T_SHENLONG_LV3 )
				{
					typeWonder = EquipByMaker12_T_SHENLONG_LV3;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numshenlong2 >= 10)
			{
				if ( typeWonder < EquipByMaker12_T_SHENLONG_LV2 )
				{
					typeWonder = EquipByMaker12_T_SHENLONG_LV2;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numshenlong1 >= 10)
			{
				if ( typeWonder < EquipByMaker12_T_SHENLONG_LV1 )
				{
					typeWonder = EquipByMaker12_T_SHENLONG_LV1;
					bGetNewSet = true;
				}
			}

			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker12_T_NONE )
			m12_equipByMakerType = typeWonder;			
		else
			m12_equipByMakerType = EquipByMaker12_T_NONE;
	}
	//soke 逆天套装套装系统
	if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ( pItem->GetObject()->kind & 16 && pItem->GetObject()->bind == 1 && !(pItem->Get2ObjectID() >= 130 && pItem->Get2ObjectID() <= 134) )
		{
			if ( pItem->GetObject()->nitianlv >= 100 )
			{
				(*itor).numSetList.numnitian5++ ;
			}
			if ( pItem->GetObject()->nitianlv >= 80 )
			{
				(*itor).numSetList.numnitian4++ ;
			}
			if ( pItem->GetObject()->nitianlv >= 50 )
			{
				(*itor).numSetList.numnitian3++ ;
			}
			if ( pItem->GetObject()->nitianlv >= 30 )
			{
				(*itor).numSetList.numnitian2++ ;
			}
			if ( pItem->GetObject()->nitianlv >= 15 )
			{
				(*itor).numSetList.numnitian1++ ;
			}	
		}

		enm13EuipByMakerType typeWonder = EquipByMaker13_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;

			if ( (*itor).numSetList.numnitian5 >= 10)
			{
				if ( typeWonder < EquipByMaker13_T_NITIAN_LV5 )
				{
					typeWonder = EquipByMaker13_T_NITIAN_LV5;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numnitian4 >= 10)
			{
				if ( typeWonder < EquipByMaker13_T_NITIAN_LV4 )
				{
					typeWonder = EquipByMaker13_T_NITIAN_LV4;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numnitian3 >= 10)
			{
				if ( typeWonder < EquipByMaker13_T_NITIAN_LV3 )
				{
					typeWonder = EquipByMaker13_T_NITIAN_LV3;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numnitian2 >= 10)
			{
				if ( typeWonder < EquipByMaker13_T_NITIAN_LV2 )
				{
					typeWonder = EquipByMaker13_T_NITIAN_LV2;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numnitian1 >= 10)
			{
				if ( typeWonder < EquipByMaker13_T_NITIAN_LV1 )
				{
					typeWonder = EquipByMaker13_T_NITIAN_LV1;
					bGetNewSet = true;
				}
			}

			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker13_T_NONE )
			m13_equipByMakerType = typeWonder;			
		else
			m13_equipByMakerType = EquipByMaker13_T_NONE;
	}
//soke 神龙装备镶嵌套装套装系统
	if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ( pItem->GetObject()->kind & 16 && pItem->GetObject()->bind == 1 && !(pItem->Get2ObjectID() >= 130 && pItem->Get2ObjectID() <= 134) )
		{
			if ( pItem->GetObject()->douzhansflv >= 100 )
			{
				(*itor).numSetList.numdouzhansf4++ ;
			}
			if ( pItem->GetObject()->douzhansflv >= 75 )
			{
				(*itor).numSetList.numdouzhansf3++ ;
			}
			if ( pItem->GetObject()->douzhansflv >= 50 )
			{
				(*itor).numSetList.numdouzhansf2++ ;
			}
			if ( pItem->GetObject()->douzhansflv >= 25 )
			{
				(*itor).numSetList.numdouzhansf1++ ;
			}	
		}

		enm14EuipByMakerType typeWonder = EquipByMaker14_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;

			if ( (*itor).numSetList.numdouzhansf4 >= 10)
			{
				if ( typeWonder < EquipByMaker14_T_DOUZHANSF_LV4 )
				{
					typeWonder = EquipByMaker14_T_DOUZHANSF_LV4;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numdouzhansf3 >= 10)
			{
				if ( typeWonder < EquipByMaker14_T_DOUZHANSF_LV3 )
				{
					typeWonder = EquipByMaker14_T_DOUZHANSF_LV3;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numdouzhansf2 >= 10)
			{
				if ( typeWonder < EquipByMaker14_T_DOUZHANSF_LV2 )
				{
					typeWonder = EquipByMaker14_T_DOUZHANSF_LV2;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numdouzhansf1 >= 10)
			{
				if ( typeWonder < EquipByMaker14_T_DOUZHANSF_LV1 )
				{
					typeWonder = EquipByMaker14_T_DOUZHANSF_LV1;
					bGetNewSet = true;
				}
			}

			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker14_T_NONE )
			m14_equipByMakerType = typeWonder;			
		else
			m14_equipByMakerType = EquipByMaker14_T_NONE;
	}
	//soke 暗影宝石套装套装系统
	if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ( pItem->GetObject()->kind & 16 && pItem->GetObject()->bind == 1 && !(pItem->Get2ObjectID() >= 130 && pItem->Get2ObjectID() <= 134) )
		{
			if ( pItem->GetObject()->aystonelv >= 100 )
			{
				(*itor).numSetList.numaystone5++ ;
			}
			if ( pItem->GetObject()->aystonelv >= 80 )
			{
				(*itor).numSetList.numaystone4++ ;
			}
			if ( pItem->GetObject()->aystonelv >= 60 )
			{
				(*itor).numSetList.numaystone3++ ;
			}
			if ( pItem->GetObject()->aystonelv >= 40 )
			{
				(*itor).numSetList.numaystone2++ ;
			}
			if ( pItem->GetObject()->aystonelv >= 20 )
			{
				(*itor).numSetList.numaystone1++ ;
			}	
		}

		enm15EuipByMakerType typeWonder = EquipByMaker15_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;

			if ( (*itor).numSetList.numaystone5 >= 10)
			{
				if ( typeWonder < EquipByMaker15_T_AYSTONE_LV5 )
				{
					typeWonder = EquipByMaker15_T_AYSTONE_LV5;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numaystone4 >= 10)
			{
				if ( typeWonder < EquipByMaker15_T_AYSTONE_LV4 )
				{
					typeWonder = EquipByMaker15_T_AYSTONE_LV4;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numaystone3 >= 10)
			{
				if ( typeWonder < EquipByMaker15_T_AYSTONE_LV3 )
				{
					typeWonder = EquipByMaker15_T_AYSTONE_LV3;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numaystone2 >= 10)
			{
				if ( typeWonder < EquipByMaker15_T_AYSTONE_LV2 )
				{
					typeWonder = EquipByMaker15_T_AYSTONE_LV2;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numaystone1 >= 10)
			{
				if ( typeWonder < EquipByMaker15_T_AYSTONE_LV1 )
				{
					typeWonder = EquipByMaker15_T_AYSTONE_LV1;
					bGetNewSet = true;
				}
			}

			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker15_T_NONE )
			m15_equipByMakerType = typeWonder;			
		else
			m15_equipByMakerType = EquipByMaker15_T_NONE;
	}

	//soke 龙凤吟套装套装系统
	if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ( pItem->GetObject()->kind & 16 && pItem->GetObject()->bind == 1 && !(pItem->Get2ObjectID() >= 130 && pItem->Get2ObjectID() <= 134) )
		{
			if ( pItem->GetObject()->retain24 >= 16 )
			{
				(*itor).numSetList.lfy5++ ;
			}
			if ( pItem->GetObject()->retain24 >= 14 )
			{
				(*itor).numSetList.lfy4++ ;
			}
			if ( pItem->GetObject()->retain24 >= 12 )
			{
				(*itor).numSetList.lfy3++ ;
			}
			if ( pItem->GetObject()->retain24 >= 10 )
			{
				(*itor).numSetList.lfy2++ ;
			}
			if ( pItem->GetObject()->retain24 >= 8 )
			{
				(*itor).numSetList.lfy1++ ;
			}	
		}

		enm24EuipByMakerType typeWonder = EquipByMaker24_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;

			if ( (*itor).numSetList.lfy5 >= 10)
			{
				if ( typeWonder < EquipByMaker24_T_LFY_LV5 )
				{
					typeWonder = EquipByMaker24_T_LFY_LV5;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.lfy4 >= 10)
			{
				if ( typeWonder < EquipByMaker24_T_LFY_LV4 )
				{
					typeWonder = EquipByMaker24_T_LFY_LV4;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.lfy3 >= 10)
			{
				if ( typeWonder < EquipByMaker24_T_LFY_LV3 )
				{
					typeWonder = EquipByMaker24_T_LFY_LV3;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.lfy2 >= 10)
			{
				if ( typeWonder < EquipByMaker24_T_LFY_LV2 )
				{
					typeWonder = EquipByMaker24_T_LFY_LV2;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.lfy1 >= 10)
			{
				if ( typeWonder < EquipByMaker24_T_LFY_LV1 )
				{
					typeWonder = EquipByMaker24_T_LFY_LV1;
					bGetNewSet = true;
				}
			}

			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker24_T_NONE )
			m24_equipByMakerType = typeWonder;			
		else
			m24_equipByMakerType = EquipByMaker24_T_NONE;
	}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//soke 百兽图鉴套装系统
   if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ((pItem->Get2ObjectID() >= 221 && pItem->Get2ObjectID() <= 230)) //图鉴套装激活规则
		{
			if ( pItem->GetObject()->handbooklvg >= 601 )//红色5星图鉴10件 计算数值为 10个图鉴同时穿戴并且强化601次条件激活
			{
				(*itor).numSetList.handbook_red++ ;
			}				
			if ( pItem->GetObject()->handbooklvg >= 501 )//橙色5星图鉴10件 计算数值为 10个图鉴同时穿戴并且强化501次条件激活
			{
				(*itor).numSetList.handbook_orange++ ;
			}			
			if ( pItem->GetObject()->handbooklvg >= 401 )//紫色5星图鉴10件 计算数值为 10个图鉴同时穿戴并且强化401次条件激活
			{
				(*itor).numSetList.handbook_purple++ ;
			}
			if ( pItem->GetObject()->handbooklvg >= 301 )//绿色4星图鉴10件 计算数值为 10个图鉴同时穿戴并且强化301次条件激活
			{
				(*itor).numSetList.handbook_green++ ;
			}
			if ( pItem->GetObject()->handbooklvg >= 201 ) //黄色3星图鉴10件 计算数值为 10个图鉴同时穿戴并且强化201次条件激活
			{
				(*itor).numSetList.handbook_yloer++ ;
			}
			if ( pItem->GetObject()->handbooklvg >= 101 )//蓝色2星图鉴10件 计算数值为 10个图鉴同时穿戴并且强化101次条件激活
			{
				(*itor).numSetList.handbook_bule++ ;
			}
			if ( pItem->GetObject()->handbooklvg >= 1 ) //白色1星图鉴10件 计算数值为 10个图鉴同时穿戴并且强化1次条件激活
			{
				(*itor).numSetList.handbook_white++ ;
			}	
		}

		enm22EuipByMakerType typeWonder = EquipByMaker22_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;

			if ( (*itor).numSetList.handbook_red >= 10) 
			{
				if ( typeWonder < EquipByMaker22_T_HANDBOOK_RED )
				{
					typeWonder = EquipByMaker22_T_HANDBOOK_RED;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.handbook_orange >= 10)
			{
				if ( typeWonder < EquipByMaker22_T_HANDBOOK_ORANGE)
				{
					typeWonder = EquipByMaker22_T_HANDBOOK_ORANGE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.handbook_purple >= 10)
			{
				if ( typeWonder < EquipByMaker22_T_HANDBOOK_PURPLE )
				{
					typeWonder = EquipByMaker22_T_HANDBOOK_PURPLE;
					bGetNewSet = true;
				}
			}						
			else if ( (*itor).numSetList.handbook_green >= 10)
			{
				if ( typeWonder < EquipByMaker22_T_HANDBOOK_GREEN )
				{
					typeWonder = EquipByMaker22_T_HANDBOOK_GREEN;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.handbook_yloer >= 10)
			{
				if ( typeWonder < EquipByMaker22_T_HANDBOOK_YLOER )
				{
					typeWonder = EquipByMaker22_T_HANDBOOK_YLOER;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.handbook_bule >= 10)
			{
				if ( typeWonder < EquipByMaker22_T_HANDBOOK_BULE )
				{
					typeWonder = EquipByMaker22_T_HANDBOOK_BULE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.handbook_white >= 10) 
			{
				if ( typeWonder < EquipByMaker22_T_HANDBOOK_WHITE )
				{
					typeWonder = EquipByMaker22_T_HANDBOOK_WHITE;
					bGetNewSet = true;
				}
			}

			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker22_T_NONE )
			m22_equipByMakerType = typeWonder;			
		else
			m22_equipByMakerType = EquipByMaker22_T_NONE;
	}
//////////////////////////////////////////////////////////////////////////////////////////////////////
	//soke 神器套装系统
	if ( strlen(pItem->GetObject()->maker) > 0 )
	{
		std::vector<stEquipByMaker>::iterator itor;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			if ( strcmp((*itor).strMakerName, pItem->GetObject()->maker) == 0 )
				break;
		}

		if ( itor == m_vEquipByMaker.end() )		//Add as new
		{
			stEquipByMaker byMaker;
			byMaker.Init();
			sprintf( byMaker.strMakerName, pItem->GetObject()->maker );
			m_vEquipByMaker.push_back( byMaker );
			itor = m_vEquipByMaker.end() -1;			
		}

		//Set set num
		if ( pItem->GetObject()->kind & 16 && pItem->GetObject()->bind == 1 && !(pItem->Get2ObjectID() >= 130 && pItem->Get2ObjectID() <= 134) )
		{
			if ( pItem->GetObject()->retain3 >= 7 )
			{
				(*itor).numSetList.numsq7Purple++ ;
			}
			if ( pItem->GetObject()->retain3 >= 6 )
			{
				(*itor).numSetList.numsq6Purple++ ;
			}
			if ( pItem->GetObject()->retain3 >= 5 )
			{
				(*itor).numSetList.numsq5Purple++ ;
			}
			if ( pItem->GetObject()->retain3 >= 4 )
			{
				(*itor).numSetList.numsq4Purple++ ;
			}
			if ( pItem->GetObject()->retain3 >= 3 )
			{
				(*itor).numSetList.numsq3Purple++ ;
			}
			if ( pItem->GetObject()->retain3 >= 2 )
			{
				(*itor).numSetList.numsq2Purple++ ;
			}
			if ( pItem->GetObject()->retain3 >= 1 )
			{
				(*itor).numSetList.numsq1Purple++ ;
			}	
		}

		enm6EuipByMakerType typeWonder = EquipByMaker6_T_NONE;
		for( itor = m_vEquipByMaker.begin();itor!=m_vEquipByMaker.end();itor++ )
		{
			bool bGetNewSet = false;

			if ( (*itor).numSetList.numsq7Purple >= 10)
			{
				if ( typeWonder < EquipByMaker67_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker67_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numsq6Purple >= 10)
			{
				if ( typeWonder < EquipByMaker66_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker66_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numsq5Purple >= 10)
			{
				if ( typeWonder < EquipByMaker65_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker65_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numsq4Purple >= 10)
			{
				if ( typeWonder < EquipByMaker64_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker64_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numsq3Purple >= 10)
			{
				if ( typeWonder < EquipByMaker63_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker63_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numsq2Purple >= 10)
			{
				if ( typeWonder < EquipByMaker62_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker62_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}
			else if ( (*itor).numSetList.numsq1Purple >= 10)
			{
				if ( typeWonder < EquipByMaker61_T_WONDER_PURPLE )
				{
					typeWonder = EquipByMaker61_T_WONDER_PURPLE;
					bGetNewSet = true;
				}
			}

			//Remember set maker
			if ( bGetNewSet )
			{
				m_strSetMakerName[0] = 0L;
				strcpy( m_strSetMakerName, (*itor).strMakerName );
			}
		}

		if ( typeWonder > EquipByMaker6_T_NONE )
			m6_equipByMakerType = typeWonder;			
		else
			m6_equipByMakerType = EquipByMaker6_T_NONE;
	}
}

char* CGuiUserInfo::GetSetMakerName()
{
	if ( strlen( m_strSetMakerName ) <= 0 )
		return NULL;

	return m_strSetMakerName;
}

BYTE CGuiUserInfo::GetSetEquipType()
{
	switch( m_equipByMakerType )
	{
	case EquipByMaker_T_NONE:
		return 0;
		break;
	case EquipByMaker_T_BUG_BLUE:
	case EquipByMaker_T_WONDER_BLUE:
		return 1;
		break;
	case EquipByMaker_T_BUG_GOLD:
	case EquipByMaker_T_WONDER_GOLD:
		return 2;
		break;
	case EquipByMaker_T_BUG_HOLY:
	case EquipByMaker_T_WONDER_HOLY:
		return 4;
		break;
	case EquipByMaker_T_BUG_PURPLE:
	case EquipByMaker_T_WONDER_PURPLE:
		return 16;
		break;
	}
	return 0;

    switch( m1_equipByMakerType )
	{
	case EquipByMaker1_T_NONE:
		return 0;
		break;
	case EquipByMaker1_T_WONDER_GREEN:
	    return 4;
		break;
	case EquipByMaker1_T_WONDER_PURPLE:
		return 16;
		break;
	}
	return 0;
	
	switch( m11_equipByMakerType )
	{
	case EquipByMaker11_T_NONE:
		return 0;
		break;
	case EquipByMaker165_T_WONDER_PURPLE:
	case EquipByMaker170_T_WONDER_PURPLE:
	case EquipByMaker175_T_WONDER_PURPLE:
	case EquipByMaker180_T_WONDER_PURPLE:
	case EquipByMaker185_T_WONDER_PURPLE:
	case EquipByMaker190_T_WONDER_PURPLE:
	case EquipByMaker197_T_WONDER_PURPLE:
	case EquipByMaker202_T_WONDER_PURPLE:
	case EquipByMaker205_T_WONDER_PURPLE:
	case EquipByMaker210_T_WONDER_PURPLE:
	case EquipByMaker215_T_WONDER_PURPLE:
	case EquipByMaker220_T_WONDER_PURPLE:	
		return 16;
		break;
	}
	return 0;
	
	switch( m2_equipByMakerType )
	{
	case EquipByMaker2_T_NONE:
		return 0;
		break;
	case EquipByMaker3_T_WONDER_PURPLE:
	case EquipByMaker5_T_WONDER_PURPLE:
	case EquipByMaker8_T_WONDER_PURPLE:
	case EquipByMaker10_T_WONDER_PURPLE:
	case EquipByMaker12_T_WONDER_PURPLE:
	case EquipByMaker13_T_WONDER_PURPLE:
	case EquipByMaker15_T_WONDER_PURPLE:
		return 16;
		break;
	}
	return 0;

	switch( m3_equipByMakerType )
	{
	case EquipByMaker3_T_NONE:
		return 0;
		break;
	case EquipByMaker100_T_WONDER_PURPLE:
	case EquipByMaker101_T_WONDER_PURPLE:
	case EquipByMaker102_T_WONDER_PURPLE:
	case EquipByMaker103_T_WONDER_PURPLE:
	case EquipByMaker104_T_WONDER_PURPLE:
		return 16;
		break;
	}
	return 0;
	
	switch( m21_equipByMakerType )
	{
	case EquipByMaker21_T_NONE:
		return 0;
		break;
	case EquipByMaker21_T_WONDER_PURPLE:
	case EquipByMaker22_T_WONDER_PURPLE:
	case EquipByMaker23_T_WONDER_PURPLE:
	case EquipByMaker24_T_WONDER_PURPLE:
	case EquipByMaker25_T_WONDER_PURPLE:
		return 16;
		break;
	}

	return 0;

    switch( m12_equipByMakerType )
	{
	case EquipByMaker12_T_NONE:
		return 0;
		break;
	case EquipByMaker12_T_SHENLONG_LV1:
	case EquipByMaker12_T_SHENLONG_LV2:
	case EquipByMaker12_T_SHENLONG_LV3:
	case EquipByMaker12_T_SHENLONG_LV4:
	case EquipByMaker12_T_SHENLONG_LV5:
	case EquipByMaker12_T_SHENLONG_LV6:
    	return 16;
		break;
	}

	return 0;	

	switch( m13_equipByMakerType )
	{
	case EquipByMaker13_T_NONE:
		return 0;
		break;
	case EquipByMaker13_T_NITIAN_LV1:
	case EquipByMaker13_T_NITIAN_LV2:
	case EquipByMaker13_T_NITIAN_LV3:
	case EquipByMaker13_T_NITIAN_LV4:
	case EquipByMaker13_T_NITIAN_LV5:
    	return 16;
		break;
	}

	return 0;

	switch( m14_equipByMakerType )
	{
	case EquipByMaker14_T_NONE:
		return 0;
		break;
	case EquipByMaker14_T_DOUZHANSF_LV1:
	case EquipByMaker14_T_DOUZHANSF_LV2:
	case EquipByMaker14_T_DOUZHANSF_LV3:
	case EquipByMaker14_T_DOUZHANSF_LV4:
    	return 16;
		break;
	}

	return 0;

	switch( m15_equipByMakerType )
	{
	case EquipByMaker15_T_NONE:
		return 0;
		break;
	case EquipByMaker15_T_AYSTONE_LV1:
	case EquipByMaker15_T_AYSTONE_LV2:
	case EquipByMaker15_T_AYSTONE_LV3:
	case EquipByMaker15_T_AYSTONE_LV4:
	case EquipByMaker15_T_AYSTONE_LV5:
    	return 16;
		break;
	}

	return 0;		
//图鉴套装
	switch( m22_equipByMakerType )
	{
	case EquipByMaker22_T_NONE:
		return 0;
		break;
	case EquipByMaker22_T_HANDBOOK_WHITE:
	case EquipByMaker22_T_HANDBOOK_BULE:
	case EquipByMaker22_T_HANDBOOK_YLOER:
	case EquipByMaker22_T_HANDBOOK_GREEN:
	case EquipByMaker22_T_HANDBOOK_PURPLE:
	case EquipByMaker22_T_HANDBOOK_ORANGE:
	case EquipByMaker22_T_HANDBOOK_RED:
    	return 16;
		break;
	}

	return 0;	
}

void CGuiUserInfo::RefreshActiveItem()
{

	CRoleItem*  pItem = NULL;

	m_listActiveItem.clear();

	//Clear EquipByMaker list
	for( int i=0;i<m_vEquipByMaker.size();i++ )
	{
		m_vEquipByMaker[i].Init();
	}
	m_vEquipByMaker.clear();
	m_strSetMakerName[0] = 0L;
	
	stObjectLocation location;

	location.dwLocation = OBJECTCELLTYPE_EQUIP;
	location.dwTableID = 0;
	location.x = 0;
	location.y = EQUIPCELLTYPE_HELM;
 	//头盔
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}
	
	location.y = EQUIPCELLTYPE_BODY;
 	//服装 
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_HANDR;
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_HANDL;
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_NECKLACE;
 	//项链
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_BELT;
 	//腰带
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_SHOES;
 	//鞋子
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.x = EQUIPCELLTYPE_LEFT;
	location.y = EQUIPCELLTYPE_ADORN;
 	//装饰品
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.x = EQUIPCELLTYPE_RIGHT;
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.x = EQUIPCELLTYPE_LEFT;
	location.y = EQUIPCELLTYPE_GLOVES;
 	//手镯
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.x = EQUIPCELLTYPE_RIGHT;
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.x = EQUIPCELLTYPE_LEFT;
	location.y = EQUIPCELLTYPE_RING;
 	//戒指
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.x = EQUIPCELLTYPE_RIGHT;
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

		location.y = EQUIPCELLTYPE_FASHION;
 	//时装
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

    location.y = EQUIPCELLTYPE_AMULET;
	//护身符
 	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
 	if (pItem!=NULL)
 	{
 		ActiveItem(pItem);
		SetEquipByName(pItem);
 	}

	location.y=EQUIPCELLTYPE_HORSESHOE;
 	//马脚
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y=EQUIPCELLTYPE_HORSEROPE;
 	//马绳
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y=EQUIPCELLTYPE_HORSESADDLE;
 	//马鞍
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y=EQUIPCELLTYPE_HORSESAFE;
 	//马甲
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y=EQUIPCELLTYPE_HORSEIRON;
 	//马镫
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_HORSEFASHION;
 	//马匹圣石
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	//法宝
	location.y = EQUIPCELLTYPE_FABAO;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_FABAOFJ1;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_FABAOFJ2;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_FABAOFJ3;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_FABAOFJ4;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_FABAOFJ5;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_FABAOFJ6;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_FABAOFJ7;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_FABAOFJ8;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}
	

	location.y = EQUIPCELLTYPE_FABAOFJ9;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_FABAOFJ10;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_FABAOFJ11;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}
	
	location.y = EQUIPCELLTYPE_FABAOFJ12;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}


	//十二星宿
	location.y = EQUIPCELLTYPE_SEXX1;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_SEXX2;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_SEXX3;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_SEXX4;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_SEXX5;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_SEXX6;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_SEXX7;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_SEXX8;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}
	

	location.y = EQUIPCELLTYPE_SEXX9;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_SEXX10;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_SEXX11;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}
	
	location.y = EQUIPCELLTYPE_SEXX12;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	
	//元神
	location.y = EQUIPCELLTYPE_YUANSHEN;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_YUANSHENFJ1;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_YUANSHENFJ2;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_YUANSHENFJ3;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_YUANSHENFJ4;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_YUANSHENFJ5;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_YUANSHENFJ6;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}


	location.y = EQUIPCELLTYPE_TUJIAN1;
 	//图鉴
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_TUJIAN2;
 	//图鉴
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	
	location.y = EQUIPCELLTYPE_TUJIAN3;
 	//图鉴
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}	

	
	location.y = EQUIPCELLTYPE_TUJIAN4;
 	//图鉴
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}


	location.y = EQUIPCELLTYPE_TUJIAN5;
 	//图鉴
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	
	location.y = EQUIPCELLTYPE_TUJIAN6;
 	//图鉴
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	
	location.y = EQUIPCELLTYPE_TUJIAN7;
 	//图鉴
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	
	location.y = EQUIPCELLTYPE_TUJIAN8;
 	//图鉴
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	
	location.y = EQUIPCELLTYPE_TUJIAN9;
 	//图鉴
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	
	location.y = EQUIPCELLTYPE_TUJIAN10;
 	//图鉴
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

/* 	location.y = EQUIPCELLTYPE_CBAMULET;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_XZAMULET;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_SBAMULET;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	}

	location.y = EQUIPCELLTYPE_SSAMULET;
	//
	pItem = GetScene()->GetMainCharacter()->FindItemByLocation(location);
	if (pItem!=NULL)
	{
		ActiveItem(pItem);
		SetEquipByName(pItem);
	} */
}

void CGuiUserInfo::InsertActiveItem(CRoleItem* pItem)
{
	if (IsActiveItem(pItem)==false)
	{
		m_listActiveItem.push_back(pItem);
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param type : 描述
 * \return 返回值的描述
 */
void CGuiUserInfo::SetCharType(int type,void* pData)
{
	FUNCTION_BEGIN;

	const stCharTypeInfo& info = GetCharTypeInfo(type);


	//GetBackImage()->SetImage(stResourceLocation(GetGuiGraphicPackName(),12,info.background));

	stResourceLocation rl;
	rl.SetFileName(c_szItemGrpPack);
	rl.group = c_nGroupItemEquip;

	rl.frame = info.equippic;
	m_aniBody[0].Create(&rl);
	rl.group = c_nGroupItemEquipPattern;
	m_aniBody[1].Create(&rl);
	rl.group = c_nGroupItemEquipPattern2;
	m_aniBody[2].Create(&rl);
	
	SetUserData(pData);

	

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiUserInfo::RefreshButtonState()
{
	FUNCTION_BEGIN;

//
	//以前这里有一个bug，当用别的页面退出再打开时，属性加点按钮就在不该出现的页面出现了
	int curTab = GetTab(7)->GetCurItem();
	CMainCharacter *pMainChar = GetScene()->GetMainCharacter();
	if(! pMainChar) return ;
	switch(curTab)
	{
	case 0://属性
		InitIcon();
		break;
	case 1:
		{
			static bool bInit = false;
			bool bEnable = pMainChar->GetPoint() > 0;
			for(size_t i=0;i<5;++i)
			{
				GetButton(IDC_ADDPOINT_BASE + i)->SetEnabled(bEnable);
				GetButton(IDC_ADDPOINT_BASE + i)->SetVisible(m_bMainUser);
			}

			GetButton(20)->SetVisible(m_bMainUser);
			GetButton(21)->SetVisible(m_bMainUser);
			GetButton(22)->SetVisible(m_bMainUser);
			GetButton(23)->SetVisible(m_bMainUser);
			GetButton(24)->SetVisible(m_bMainUser);

			for(size_t i=0;i<5;++i)
			{
				GetButton(IDC_ADDPOINT_BASE + i)->GetToolTips()->Clear();
				GetButton(IDC_ADDPOINT_BASE + i)->GetToolTips()->SetBkColor(COLOR_ARGB(128,0,0,0));
				GetButton(IDC_ADDPOINT_BASE + i)->GetToolTips()->SetCurColor(COLOR_ARGB(255,255,255,255));
				char chProTxt[256];
				sprintf( chProTxt , "%s,%s",GetPropTxt(i),"当您剩余属性点总数大于等于50时,可以使用快速加点功能" );
			//	sprintf( chProTxt , "%s,%s",GetPropTxt(i),"Ctrl+鼠标左键可以使用快速加点功能" );
				GetButton(IDC_ADDPOINT_BASE + i)->GetToolTips()->AddText(chProTxt);
				GetButton(IDC_ADDPOINT_BASE + i)->GetToolTips()->Resize();
			}
			GetButton(IDC_ADDPOINT_BASE + 1)->SetTipsShowType(GUI_ALIGN_LEFTBUTTOM);
			bInit = true;
			InitIcon();
		}
		break;
	}
//

	
	//if(pMainChar)
	//{
	//	/*

	//	for(size_t i=0;i<5;++i)
	//	{
	//		GetButton(IDC_FIVEPOINT_RECOVERY + i)->GetToolTips()->Clear();
	//	}
	//	
	//	char szText[MAX_PATH]={0};
	//
	//	// 金抵抗
	//	sprintf(szText,CURRFIVECONTEXT,"金",GetScene()->GetMainCharacter()->GetProperty()->metalresistance);
	//	GetButton(IDC_FIVEPOINT_RECOVERY + 0)->GetToolTips()->AddText(szText);

	//	// 木抵抗
	//	sprintf(szText,CURRFIVECONTEXT,"木",GetScene()->GetMainCharacter()->GetProperty()->woodresistance);
	//	GetButton(IDC_FIVEPOINT_RECOVERY + 1)->GetToolTips()->AddText(szText);

	//	// 水抵抗
	//	sprintf(szText,CURRFIVECONTEXT,"水",GetScene()->GetMainCharacter()->GetProperty()->waterresistance);
	//	GetButton(IDC_FIVEPOINT_RECOVERY + 2)->GetToolTips()->AddText(szText);


	//	// 火抵抗
	//	sprintf(szText,CURRFIVECONTEXT,"火",GetScene()->GetMainCharacter()->GetProperty()->fireresistance);
	//	GetButton(IDC_FIVEPOINT_RECOVERY + 3)->GetToolTips()->AddText(szText);


	//	// 土抵抗
	//	sprintf(szText,CURRFIVECONTEXT,"土",GetScene()->GetMainCharacter()->GetProperty()->soilresistance);
	//	GetButton(IDC_FIVEPOINT_RECOVERY + 4)->GetToolTips()->AddText(szText);

	//	for(size_t i=0;i<5;++i)
	//	{
	//		GetButton(IDC_FIVEPOINT_RECOVERY + i)->GetToolTips()->Resize();
	//	}

	//	*/
	//}

	FUNCTION_END;
}

//soke 自由加点
void CGuiUserInfo::AutomaticProperty( int nPosition )
{
	if ( GetScene()->GetMainCharacter() && GetScene()->GetMainCharacter()->GetProperty() )
	{
		int nSize = GetScene()->GetMainCharacter()->GetProperty()->points / 3;
		SendAddUserProperty( PROPERTY_CON , nSize );
		nSize = GetScene()->GetMainCharacter()->GetProperty()->points - nSize;
		switch ( nPosition )
		{
		case 0:
			{
				SendAddUserProperty( PROPERTY_CON , nSize );
			}
			break; 
		case 1:
			{
				SendAddUserProperty( PROPERTY_STR , nSize );
			}
			break; 
		case 2:
			{
				SendAddUserProperty( PROPERTY_DEX , nSize );
			}
			break;
		case 3:
			{
				SendAddUserProperty( PROPERTY_INT , nSize );
			}
			break;
		case 4:
			{
				SendAddUserProperty( PROPERTY_MEN , nSize );
			}
			break;
		}
	}
}

void CGuiUserInfo::RefreshShowState()
{
	GetButton(20)->SetChecked(GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_COUNTRYNAME ));
	GetButton(21)->SetChecked(GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_UNIONNAME ));
	GetButton(22)->SetChecked(GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_SEPTNAME ));
	GetButton(23)->SetChecked(GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_OFFICENAME ));
	GetButton(24)->SetChecked(GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_GRACENAME ));
}

void CGuiUserInfo::RefreshGetBoxGeMianState()
{
	int nIndex;
	CGuiComboBox* pCombo = GetComboBox(14);
	if (pCombo)
	{   
		static int m_cur_index=0;
		m_cur_index=pCombo->GetCurItem();
		pCombo->RemoveAllItems();

		for (int i = 0; i < 10; i++) //修改一键换装10套
		{
			char szStr[256];
			sprintf(szStr, "%s", m_OneKeyEquip[i].sName);
			if(!strlen(szStr))
			{
				sprintf(szStr, "第%d套", i + 1);
			}
			pCombo->AddItem(szStr, NULL);

		}
		pCombo->SetCurItem(m_cur_index);

		nIndex=m_cur_index;
	}
}


void CGuiUserInfo::RefreshGetBoxQianming()
{
	//soke 籍贯
	CGuiComboBox* pguanji = GetComboBox(1019);
	if(pguanji)
	{
		pguanji->RemoveAllItems();
		for(int i=1;i<2;i++)
		{
			char szStr[256];
			sprintf(szStr,"北京市",i);
			pguanji->AddItem(szStr,NULL);
		}
		for(int i=2;i<3;i++)
		{
			char szStr[256];
			sprintf(szStr,"重庆市",i);
			pguanji->AddItem(szStr,NULL);
		}
	}

	//soke 地区
	CGuiComboBox* pdiqu = GetComboBox(1020);
	if(pdiqu)
	{
		pdiqu->RemoveAllItems();
		for(int i=1;i<2;i++)
		{
			char szStr[256];
			sprintf(szStr,"海淀",i);
			pdiqu->AddItem(szStr,NULL);
		}
	}
	//soke 年龄
	CGuiComboBox* pnianling = GetComboBox(1021);
	if(pnianling)
	{
		pnianling->RemoveAllItems();
		for(int i=0;i<1;i++)
		{
			char szStr[256];
			sprintf(szStr,"保密",i);
			pnianling->AddItem(szStr,NULL);
		}
		for(int i=1;i<101;i++)
		{
			char szStr[256];
			sprintf(szStr,"%d",i);
			pnianling->AddItem(szStr,NULL);
		}
	} 
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nEvent : 描述
 * \param nID : 描述
 * \param pControl : 描述
 * \return 返回值的描述
 */

CRoleItem* CGuiUserInfo::GetShowClothes()
{
    //soke 时装 	        
	//soke 如果身上有衣服、时装、直接显示时装外形，如果没有衣服，时装外形则不显示
	CRoleItem* pRoleItem = GetEquipItem(EQUIPCELLTYPE_FASHION);

    if ((pRoleItem) && (pRoleItem->GetObjectBase()->dwType==ItemType_SHighFashionBody))
    {
		pRoleItem = NULL;

        if ( pRoleItem == NULL)
		pRoleItem = GetEquipItem(EQUIPCELLTYPE_BODY); 
    }
	else
	{
		if ((pRoleItem) && ((pRoleItem->GetObjectBase()->dwType!=ItemType_FashionBody) || (pRoleItem->GetObjectBase()->dwType!=ItemType_HighFashionBody)
		 || (pRoleItem->GetObjectBase()->dwType!=ItemType_XHighFashionBody) || (pRoleItem->GetObjectBase()->dwType!=ItemType_SHighFashionBody)) )
	    pRoleItem = NULL;

        if ( pRoleItem == NULL)
	    {  

	       if (pRoleItem = GetEquipItem(EQUIPCELLTYPE_BODY))
	        {
		       pRoleItem = GetEquipItem(EQUIPCELLTYPE_FASHION);
	        }  
	    }	

		if (pRoleItem == NULL)
		pRoleItem = GetEquipItem(EQUIPCELLTYPE_BODY); 

	}
	return  pRoleItem;
}

bool CGuiUserInfo::IsAdornObject(DWORD dwObjectID)
{
	CRoleItem* pRoleItem = GetEquipItem(EQUIPCELLTYPE_ADORN, EQUIPCELLTYPE_LEFT);

	if ((pRoleItem)&&(pRoleItem->GetObjectBase()->dwID!=dwObjectID))
		pRoleItem = NULL;
	
	if ( pRoleItem == NULL )
	{
		pRoleItem = GetEquipItem( EQUIPCELLTYPE_ADORN, EQUIPCELLTYPE_RIGHT );
		if ((pRoleItem)&&(pRoleItem->GetObjectBase()->dwID!=dwObjectID))
			pRoleItem = NULL;
	}

	return pRoleItem != NULL;
}

bool CGuiUserInfo::IsAdornObjectByType(DWORD  dwType)
{
	CRoleItem* pRoleItem = GetEquipItem(EQUIPCELLTYPE_ADORN, EQUIPCELLTYPE_LEFT);

	if ((pRoleItem)&&(pRoleItem->GetObjectBase()->dwType != dwType))
		pRoleItem = NULL;

	if ( pRoleItem == NULL )
	{
		pRoleItem = GetEquipItem( EQUIPCELLTYPE_ADORN, EQUIPCELLTYPE_RIGHT );
		if ((pRoleItem)&&(pRoleItem->GetObjectBase()->dwType != dwType))
			pRoleItem = NULL;
	}

	return pRoleItem != NULL;
}

//sky 换装tip显示
CToolTips* CGuiUserInfo::GetTaoZhuangTooltips(int nIndex)
{
	FUNCTION_BEGIN;

	static CToolTips tips;

	tips.Clear();

	tips.SetBkColor(COLOR_ARGB(128,0,0,0));
	tips.SetAlignType( GUI_ALIGN_LEFT );
	tips.SetLineGaps( 3 );
	tips.SetCurColor(COLOR_ARGB(255,255,255,255));

	std::string strText = getTaoZhuang_Name(nIndex);
	tips.AddText(strText.c_str());
	tips.Resize(); 
	return &tips;

	FUNCTION_END;
}
std::string CGuiUserInfo::getTaoZhuang_Name(int index)
{
	std::string strText = m_OneKeyEquip[index].sName;

	if(strText.empty())
	{
		char szTemp[512];
		_snprintf(szTemp,sizeof(szTemp),"第%d套",index + 1);
		szTemp[sizeof(szTemp)-1]=0;

		strText = szTemp;
	}
	return strText;
}

std::string CGuiUserInfo::getTaoZhuang_SName(int index)
{
	std::string strText = m_OneKeyEquip[index].sName;

	if(strText.empty())
	{
		char szTemp[512];
		_snprintf(szTemp,sizeof(szTemp),"  %d",index + 1);
		szTemp[sizeof(szTemp)-1]=0;

		strText = szTemp;
	}
	if(index == 9)
	   return strText.substr(3, 3); //substr(0, 2);
	else
		return strText.substr(2, 2); //substr(0, 2);
}

int CGuiUserInfo::getActiveIndex()
{
	CGuiComboBox* pCombo = GetComboBox(14);
	return pCombo->GetCurItem();
}
void CGuiUserInfo::equipTaoZhuang(int nIndex)
{
	CGuiComboBox* pCombo = GetComboBox(14);
	pCombo->SetCurItem(nIndex);

	on_equipTaoZhuang(nIndex);
}

void CGuiUserInfo::on_equipTaoZhuang(int nIndex)
{
	// //一键换装冷却
	// time_t times;
	// time(&times);

	// if(times - onekey_time >=1)
	// {
	// 	onekey_time = times;
		for(int i = 0;i<27;i++)
		{
				CRoleItem* pItem = GetScene()->GetMainCharacter()->FindItemByID( m_OneKeyEquip[nIndex].IdTable[i] );
				if ( pItem )
				{
					// if(pItem->GetLocation().dwLocation == OBJECTCELLTYPE_ANNEX || pItem->GetLocation().dwLocation == OBJECTCELLTYPE_RECAST )
					// {
					// 	continue;
					// }
					if (i == 2 || i == 4)
						EquipItem(pItem, 1);
					else if (i == 3 || i == 5)
						EquipItem(pItem, 0);
					else
						EquipItem(pItem, -1);
				}
		}	
		setSkillIconID(m_OneKeyEquip[nIndex].dwSkillIconID);

		if(CGuiSkill::stSkillIcon *pST = GetGameGuiManager()->m_guiSkill->GetSkillIconByID(m_dwSkillIconID))
			GetScene()->GetMainCharacter()->OnSelectSkill( pST->pSkillBase->dwID );

		// 鼠标点击的边框
		m_usedIndex = nIndex;
	// }
	// else{
	// 	GetGameGuiManager()->AddClientSystemMessage( "一键换装速度过快！" );
	// }

}

//sky 技能ID
void CGuiUserInfo::setSkillIconID(DWORD dwID)
{
	stResourceLocation rl;
	if(GetGameGuiManager()->m_guiSkill->GetSkillIconResourceFromID(dwID, rl))
	{
		// m_pBtn_Skill->SetBitmapScale(skillIconScale);
		m_pBtn_Skill->SetBitmap(rl, 3, dwState);
	}
	else
	{
		m_pBtn_Skill->SetBitmap(m_rl_Skill, 1, dwState);
	}

	m_dwSkillIconID = dwID;
}

void CGuiUserInfo::ParseAccessBtnClickEvent(UINT nEvent, UINT nID)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch (nID)
		{
		case 1613: // 技能框
			if (GetGameGuiManager()->m_guiUserInfo)
			{
				if ((GetGameCursor()->IsDrag() && ((GetGameCursor()->GetCursorImageType() == eCursorSetAccess) || (GetGameCursor()->GetCursorImageType() == eCursorDictate))) || GetGuiManager()->GetFloatItem())
					SetAccessKey(nID);
			}

			break;
		}
	}
	if(nEvent == EVENT_RBUTTON_CLICKED)
	{
		if(m_pBtn_Skill->IsMouseOver())
		{
			setSkillIconID();
		}
	}


	FUNCTION_END;
}


bool CGuiUserInfo::SetAccessKey(const int &keyID)
{
	FUNCTION_BEGIN;

	if (NULL == m_pBtn_Skill)
	{
		RestoreMouseIcon();
		return false;
	}

	CGuiMain::enumActiveAccessType type;
	if (!(GetGameGuiManager()->m_guiMain->IsAnyActiveForAccessKeySetting(type)))
	{
		RestoreMouseIcon();
		return false;
	}
	// m_pBtnAccessKey_Skill->SetLocation( g_nMainAccessBtnLeft+keyID*g_MainAccessBtnSpace, g_nMainAccessBtnTop );
	m_pBtn_Skill->SetText("");

	stResourceLocation rl;
	switch (type)
	{
	case CGuiMain::ActiveAccessSkill:
	{
		if (GetGameGuiManager()->m_guiSkill->GetActiveSkillIconResource(rl))
		{
			setSkillIconID(GetGameGuiManager()->m_guiSkill->GetActiveSkillIconID());

		    GetGameGuiManager()->m_guiSkill->SetActiveSkillIcon(); //解决拖拽第一次是技能BUG
			RestoreMouseIcon();

			return true;
		}
	}
	break;
	default:
		break;
	}

	RestoreMouseIcon();
	return false;

	FUNCTION_END;
}

bool CGuiUserInfo::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if (nEvent == EVENT_BUTTON_CLICKED || nEvent == EVENT_RBUTTON_CLICKED)
		ParseAccessBtnClickEvent(nEvent, nID);

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) 
{
			case 5:
				m_pTab->SetCurItem(0);
				GetButton(5)->SetChecked(true);
				SetBackImage(stResourceLocation("data\\interfaces3.gl",60,0));//soke 角色界面
				// SetBackImage(stResourceLocation("data\\interfaces.gl",3,0));//soke 角色界面
				RefreshButtonState();
				GetButton( 25 )->SetLocation( 361, 96 );   //soke 有新的邮件
				break;
			case 6:
				m_pTab->SetCurItem(1);
				GetButton(6)->SetChecked(true);
				SetBackImage(stResourceLocation("data\\interfaces.gl",3,57));//soke 属性界面
				RefreshButtonState();
				GetButton( 25 )->SetLocation( 361, 96 );   //soke 有新的邮件
				break;
			case 8:
				{
					m_pTab->SetCurItem(2);
					GetButton(8)->SetChecked(true);
					SetBackImage(stResourceLocation("data\\interfaces.gl",25,0)); //他人观看马匹装备界面
					GetButton( 25 )->SetLocation( 361, 96 );   //soke 有新的邮件
				}
				break;
			case 9:
				{
					m_pTab->SetCurItem(3);
					GetButton(9)->SetChecked(true);
					SetBackImage(stResourceLocation("data\\interfaces.gl",3,600)); //31 soke 一键换装
					GetButton( 25 )->SetLocation( 361, 96 );   //soke 有新的邮件
					
				    RefreshGetBoxGeMianState();
				}
				break;
			case 1023: //soke 签名档
				{
					m_pTab->SetCurItem(4);
					GetButton(1023)->SetChecked(true);
					SetBackImage(stResourceLocation("data\\interfaces1.gl",27,23));
					RefreshGetBoxQianming();
					GetButton( 25 )->SetLocation( 361, 96 );   //soke 有新的邮件
				}
				break;
			case 1500: //法宝
				{
					if(GetScene()->GetMainCharacter()->GetLevel()<190)
					{
						GameMessageBox( "190级开放法宝功能" );
						return false;
					}
					m_pTab->SetCurItem(5);
					SetBackImage(stResourceLocation("data\\interfaces.gl",80,53)); //31
					// m_pBackImage->SetLocation(22,80);
					GetButton( 25 )->SetLocation( 662, 96 );  
					
				}
				break;
			case 1510: //展示法宝
				{
					// if(fabao_show)
					// {
					// 	GetGameGuiManager()->AddMessageBox("是否关闭法宝展示?",eAction_FabaoShow,NULL,"确定|2|Enter|取消|3|Esc");
					// 	return false;
					// }
					GetGameGuiManager()->AddMessageBox("是否开启法宝展示?",eAction_FabaoShow,NULL,"确定|1|Enter|取消|3|Esc");
				}
				break;
			case 1504://法宝帮助
				{
					if(GetGameGuiManager()->m_pDlgEquipFabaoRecast && ((CGuiDialog*)GetGameGuiManager()->m_pDlgEquipFabaoRecast)->IsVisible())
					{
						((CGuiDialog*)GetGameGuiManager()->m_pDlgEquipFabaoRecast)->SetVisible(false);
					}
					else
					{
						GetGameGuiManager()->AddDlgEquipFabaoRecast();
					}
				}
				break;
			case 1216://元神助手
				{	
					if(GetGameGuiManager()->m_pDlgEquipYuanshenRecast && ((CGuiDialog*)GetGameGuiManager()->m_pDlgEquipYuanshenRecast)->IsVisible())
					{
						((CGuiDialog*)GetGameGuiManager()->m_pDlgEquipYuanshenRecast)->SetVisible(false);
					}
					else
					{
						GetGameGuiManager()->AddDlgEquipYuanshenRecast();
					}
				}
				break;
			case 1821://头衔
				{
					if(GetGameGuiManager()->m_guiTouxian && ((CGuiDialog*)GetGameGuiManager()->m_guiTouxian)->IsVisible())
					{
						((CGuiDialog*)GetGameGuiManager()->m_guiTouxian)->SetVisible(false);
					}
					else
					{
						GetGameGuiManager()->AddTouxian();
					}
				}
				break;
			case 1721://称号
				{
					if(GetGameGuiManager()->m_guiChenghao && ((CGuiDialog*)GetGameGuiManager()->m_guiChenghao)->IsVisible())
					{
						((CGuiDialog*)GetGameGuiManager()->m_guiChenghao)->Close();
					}
					else
					{
						GetGameGuiManager()->AddChenghao();
					}
				}
				break;
			case 2501://五附件点击
				{	
					GetGameGuiManager()->Addjianzhong();
				}
				break;
			case 2502://星宿点击
				{	
					SetBackImage(stResourceLocation("data\\interfaces3.gl",60,0));//soke 角色界面
					m_pTab->SetCurItem(6);
				}
				break;
			case 2500://元神点击  这里 case 2500  这个 2500 指的是 gui的xml文件 里面的 编号2500控件 的点击效果
				{	
					if(GetScene()->GetMainCharacter()->GetLevel()<190)
					{
						GameMessageBox( "190级开放元神功能" );
						return false;
					}
					m_pTab->SetCurItem(7);
					SetBackImage(stResourceLocation("data\\interfaces3.gl",13,73));//soke 角色界面
					GetButton( 25 )->SetLocation( 505, 96 );   //soke 有新的邮件
					}
				break;
			case 8888://云天别墅点击
				{	
					if(GetGameGuiManager()->m_guiBieShu && ((CGuiDialog*)GetGameGuiManager()->m_guiBieShu)->IsVisible())
					{
						((CGuiDialog*)GetGameGuiManager()->m_guiBieShu)->SetVisible(false);
					}
					else
					{
					
						GetGameGuiManager()->AddBieShu();
					}
				}
				break;
			case 8890://时装魔盒点击
				{	
					if(GetGameGuiManager()->m_guiMagicBox && ((CGuiDialog*)GetGameGuiManager()->m_guiMagicBox)->IsVisible())
					{
						((CGuiDialog*)GetGameGuiManager()->m_guiMagicBox)->SetVisible(false);
					}
					else
					{
					
						GetGameGuiManager()->AddMagicBox();
					}
				}
				break;
			case 8889://山河图点击
				{	
					if(GetGameGuiManager()->m_guiXiuxian && ((CGuiDialog*)GetGameGuiManager()->m_guiXiuxian)->IsVisible())
					{
						((CGuiDialog*)GetGameGuiManager()->m_guiXiuxian)->SetVisible(false);
					}
					else
					{
					
						GetGameGuiManager()->AddXiuxian();
					}
				}
				break;
			case 8891://能力能力修炼点击
				{	
					if(GetGameGuiManager()->m_guiXiulian && ((CGuiDialog*)GetGameGuiManager()->m_guiXiulian)->IsVisible())
					{
						((CGuiDialog*)GetGameGuiManager()->m_guiXiulian)->SetVisible(false);
					}
					else
					{
					
						GetGameGuiManager()->AddXiulian();
					}
				}
				break;	
			case 1722://封印系统
				{
					if(GetGameGuiManager()->m_guiFengyin && ((CGuiDialog*)GetGameGuiManager()->m_guiFengyin)->IsVisible())
					{
						((CGuiDialog*)GetGameGuiManager()->m_guiFengyin)->Close();
					}
					else
					{
						GetGameGuiManager()->AddFengyin();
					}
				}
				break;		
			case 15:
				{
			GetGameGuiManager()->AddInputBox("请输入套装名字？", eAction_RenameEquip, 0);
			CGuiComboBox* pCombo = GetComboBox(14);
			if (pCombo)
			{
					//sky 同步guilnputBox命名
				pComboItem = pCombo;

			}
		}
		break;
		case 16: //清除设置（清除	本套换装的设置）
		{         
			//清除指令
			for (int i = 0; i < 27; i++)
			{
				CRoleItem* pItem = GetCellItem(GetTable(i + 500), stPointI(0, 0));
				if (pItem)
				{
					pItem->OnRemove();
					delete pItem;
					pItem = NULL;
				}
			}
			setSkillIconID();
		}
		break;
		case 17: // 一键换装  读取身上装备
		{

			for (int i = 0; i < 27; i++)
			{
				//sky 
				CRoleItem* pItem = GetCellItem(GetTable(i + 500), stPointI(0, 0));
				if (pItem)
				{
					//pItem->ClearItemImage();
					pItem->OnRemove();
					delete pItem;
					pItem = NULL;
				}
			}
			for (int i = 0; i < 17; i++)
			{
				//sky 
				//CRoleItem* pItem = GetCellItem(GetTable(i + 500), stPointI(0, 0));
				//if (pItem)
				//{
				//	//pItem->ClearItemImage();
				//	pItem->OnRemove();
				//	delete pItem;
				//	pItem = NULL;
				//}

				//sky 读取身上装备
				CRoleItem* pItem2 = GetCellItem(GetTable(i + 100), stPointI(0, 0));
				if (pItem2)
				{
					pItem2->CopyTo(GetTable(i + 500));

				}
			}
			for (int i = 17; i < 21; i++)
			{
				//sky 
				//CRoleItem* pItem = GetCellItem(GetTable(i + 500), stPointI(0, 0));
				//if (pItem)
				//{
				//	//pItem->ClearItemImage();
				//	pItem->OnRemove();
				//	delete pItem;
				//	pItem = NULL;
				//}

				//sky 读取身上装备
				CRoleItem* pItem2 = GetCellItem(GetTable(i + 100), stPointI(0, 0));
				if (pItem2)
				{
					pItem2->CopyTo(GetTable(i + 506));

				}
			}

			for (int i = 21; i < 27; i++)
			{
				////sky 
				//CRoleItem* pItem = GetCellItem(GetTable(i + 500), stPointI(0, 0));
				//if (pItem)
				//{
				//	//pItem->ClearItemImage();
				//	pItem->OnRemove();
				//	delete pItem;
				//	pItem = NULL;
				//}

				//sky 读取身上装备
				CRoleItem* pItem2 = GetCellItem(GetGameGuiManager()->m_guiPetDlg->GetTable(i-2 + 200), stPointI(0, 0));
				if (pItem2)
				{
					pItem2->CopyTo(GetTable(i + 500));

				}
			}
			
			setSkillIconID();

		}
		break;
		case 18: // 换装
		{
			CGuiComboBox *pCombo = GetComboBox(14);
			int nIndex = pCombo->GetCurItem();
			on_equipTaoZhuang(nIndex);
		}
		break;
		case 19: // 一键换装 保存
		{
			stOneKeyEquipPropertyUserCmd cmd;

			CGuiComboBox* pCombo = GetComboBox(14);
			if (pCombo)
			{
				CMainCharacter* pMainRole = (CMainCharacter*)GetScene()->GetMainCharacter();
				strncpy(cmd.one_key_equip.sName, pCombo->GetText(), 256);
				cmd.one_key_equip.index = pCombo->GetCurItem();
				cmd.dwOldTempID = pMainRole->GetProperty()->userid;
			}
			else
			{
				cmd.one_key_equip.index = -1;
			}
			for (int i = 0; i < 27; i++)
			{
				CRoleItem* pItem = GetCellItem(GetTable(i + 500), stPointI(0, 0));

				if (pItem)
				{
					//CGuiTable* table500 = GetTable(500 + i);
					cmd.one_key_equip.IdTable[i] = pItem->GetThisID();
				}
				else
				{
					cmd.one_key_equip.IdTable[i] = 0;
				}
			}
			cmd.one_key_equip.dwSkillIconID = m_dwSkillIconID;
			SEND_USER_CMD(cmd);

            //sky 保存更新套装信息
			stGetOneKeyEquipPropertyUserCmd cmd1;
			CMainCharacter* pMainRole = (CMainCharacter*)GetScene()->GetMainCharacter();
			if (pMainRole)
				cmd1.dwOldTempID = cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
			else
				cmd1.dwOldTempID = 0;
			SEND_USER_CMD(cmd1);
		}
		break;
			case 25:
				{
					SetVisible(false);
				}
				break;
			case 126: //soke 装备引导
				{
					GetGameGuiManager()->AddDlgEquipRecast();
				}	
				break;
			case 1800: //soke 附件引导
				{
					GetGameGuiManager()->AddDlgEquipAnnexpack();
				}	
				break;	
			case 1801: //soke 儿女
				{
					GetGameGuiManager()->AddBaby();
				}	
				break;	
			case 1300: //soke 百兽图鉴
				{
					GetGameGuiManager()->AddDlgEquiphandbook();
				}
				break;
 				case 128: //soke 翅膀封印
				{
					if ( GetScene()->GetMainCharacter() && isset_state( GetScene()->GetMainCharacter()->GetState(), USTATE_ULTRA_CHIBANG_QI ) )
					{
						CRoleItem* pItem = GetEquipItem(EQUIPCELLTYPE_XZAMULET);
						if(pItem->GetObject()->dwObjectID !=3831 && pItem)
						{
							GameMessageBox( "只有勋章品质等级在紧箍咒以上才可开放外形" );
							break;
						}
						if(pItem->GetObject()->dmedalt < 1000)
						{
							GameMessageBox( "只有勋章等级达到1000阶才可开放外形" );
							break;
						}
						// if ( GetScene()->GetMainCharacter()->GetProperty()->angel == 1)
						// {
						// 	GetGameGuiManager()->AddMessageBox("你确定要关闭神兵跟随吗？",eAction_Chibang2,NULL, "确定|1|Enter|取消|2|Esc");
						// }
						// else
						// {
							GetGameGuiManager()->AddMessageBox("你确定要开启神兵跟随吗？",eAction_Chibang,NULL, "确定|1|Enter|取消|2|Esc");
						// }
					}
					else
					{
						GameMessageBox( "未检测到紧箍咒！" );
					}
				}
				break;
			case 131: //soke 寒冰麒麟
				{
					if ( GetScene()->GetMainCharacter() && isset_state( GetScene()->GetMainCharacter()->GetState(), USTATE_ULTRA_HANBING_QI ) )
					{
						
						if ( GetScene()->GetMainCharacter()->GetProperty()->hanbing == 1)
						{
							GetGameGuiManager()->AddMessageBox("你确定要关闭寒冰武器外形吗？",eAction_Hanbing2,NULL, "确定|1|Enter|取消|2|Esc");
						}
						else
						{
							CRoleItem* pItem = GetEquipItem(EQUIPCELLTYPE_ADORN);
							if(pItem)
							{
								if(pItem->GetObject()->dwObjectID != 3865 ||  pItem->GetObject()->retain1 < 99 && pItem)
								{
									GameMessageBox( "只有寒冰麒麟满阶才可开放外形" );
									break;
								}
								GetGameGuiManager()->AddMessageBox("你确定要开启寒冰武器外形吗？",eAction_Hanbing,NULL, "确定|1|Enter|取消|2|Esc");
							}
							CRoleItem* pItem2 = GetEquipItem(14);
							if(pItem2)
							{
								if(pItem2->GetObject()->dwObjectID != 3865 ||  pItem2->GetObject()->retain1 < 99 && pItem2)
								{
									GameMessageBox( "只有寒冰麒麟满阶才可开放外形" );
									break;
								}
								GetGameGuiManager()->AddMessageBox("你确定要开启寒冰武器外形吗？",eAction_Hanbing,NULL, "确定|1|Enter|取消|2|Esc");
							}
						}
					}
					else
					{
						GameMessageBox( "未检测到寒冰麒麟！" );
					}
				}
				break;
			case 20:
				GetButton(nID)->SetChecked(!(GetButton(nID)->GetChecked()));
				if(GetButton(nID)->GetChecked()) 
					GetGameApplication()->SetSystemSetOnServer(USER_SETTING_SHOW_COUNTRYNAME);
				else 
					GetGameApplication()->ClearSystemSetOnServer(USER_SETTING_SHOW_COUNTRYNAME);
				break;
			case 21:
				GetButton(nID)->SetChecked(!(GetButton(nID)->GetChecked()));
				if(GetButton(nID)->GetChecked())
					GetGameApplication()->SetSystemSetOnServer(USER_SETTING_SHOW_UNIONNAME);
				else 
					GetGameApplication()->ClearSystemSetOnServer(USER_SETTING_SHOW_UNIONNAME);
				break;
			case 22:
				GetButton(nID)->SetChecked(!(GetButton(nID)->GetChecked()));
				if(GetButton(nID)->GetChecked())
					GetGameApplication()->SetSystemSetOnServer(USER_SETTING_SHOW_SEPTNAME);
				else 
					GetGameApplication()->ClearSystemSetOnServer(USER_SETTING_SHOW_SEPTNAME);
				break;
			case 23:
				GetButton(nID)->SetChecked(!(GetButton(nID)->GetChecked()));
				if(GetButton(nID)->GetChecked())  
					GetGameApplication()->SetSystemSetOnServer(USER_SETTING_SHOW_OFFICENAME);
				else 
					GetGameApplication()->ClearSystemSetOnServer(USER_SETTING_SHOW_OFFICENAME);
				break;
			case 24:
				GetButton(nID)->SetChecked(!(GetButton(nID)->GetChecked()));
				if(GetButton(nID)->GetChecked())  
					GetGameApplication()->SetSystemSetOnServer(USER_SETTING_SHOW_GRACENAME);
				else 
					GetGameApplication()->ClearSystemSetOnServer(USER_SETTING_SHOW_GRACENAME);
				break;
			default:
			if (m_bMainUser)
			{
				stPointI pt(Engine_GetCursor()->GetPosition());
				pt -= GetLocation();
				static int size=sizeof(g_PropTxt)/sizeof(stPropTxt);
				for ( int i = 0 ; i < size ; i ++ )
				{
					stRectI rc;
					rc.left = GetButton( IDC_ADDPOINT_BASE + i )->GetX();
					rc.top = GetButton( IDC_ADDPOINT_BASE + i )->GetY()-8; //soke 属性加点位置改了，这里也要改 不然会错乱
					rc.right = rc.left + GetButton( IDC_ADDPOINT_BASE + i )->GetWidth();
					rc.bottom = rc.top + GetButton( IDC_ADDPOINT_BASE + i )->GetHeight();
					if ( rc.PtInRect( pt ) )
					{
					  if(m_UserData.pMainUserData->points > 50)
					  {
						if(nID == IDC_ADDPOINT_BASE)
						{
							GetGameGuiManager()->AddInputBox( "请输入您想加在体质上的点数" , eAction_Add_Property , (void*)0);
						}
						else if( nID == IDC_ADDPOINT_BASE + 1)
						{
							GetGameGuiManager()->AddInputBox( "请输入您想加在力量上的点数" , eAction_Add_Property , (void*)1);
						}
						else if( nID == IDC_ADDPOINT_BASE + 2)
						{
							GetGameGuiManager()->AddInputBox( "请输入您想加在敏捷上的点数" , eAction_Add_Property , (void*)2);
						}
						else if( nID == IDC_ADDPOINT_BASE + 3)
						{
							GetGameGuiManager()->AddInputBox( "请输入您想加在智力上的点数" , eAction_Add_Property , (void*)3);
						}
						else if( nID == IDC_ADDPOINT_BASE + 4)
						{
							GetGameGuiManager()->AddInputBox( "请输入您想加在精神上的点数" , eAction_Add_Property , (void*)4);
						}
					//	if ( IsKeyPressed( VK_LCONTROL ) )
					//	{
						//	GetGameGuiManager()->AddInputBox( "请输入您想加在敏捷上的点数" , eAction_Add_Property , (void*)i);
					//		GetGameGuiManager()->AddInputBox( "请输入您想分配属性的点数" , eAction_Add_Property , (void*)i);
					//	}						
					  }
					  else
					  {
						 stDistributePointPropertyUserCmd cmd;
						 cmd.type = i;
						 SEND_USER_CMD(cmd);
						 return true;
						 Close(); //桃子 加完属性关闭加属性页面
					  }
					}
				}
			}
			}
			
		}
		break;
       case EVENT_BUTTON_DRAG_MOVE:
			{
				if ( !GetGameCursor()->IsDrag()
					&& !(GetGameCursor()->GetCursorImageType() == eCursorSetAccess)
					&& !(GetGameCursor()->GetCursorImageType() == eCursorUseAccess)
					&& !(GetGameCursor()->GetCursorImageType() == eCursorFindAccess) )
				{
					if(nID == 11)
					{
						CGuiButton* btn = GetButton(nID);
						if(btn)
						{
							btn->SetBitmap(stResourceLocation("data\\Interfaces.gl",3,64),3);
						}

						stResourceLocation rl;
						int fNum = 0;
						if ( btn->GetBitmapInfo( rl, fNum ) )
						{
							//Get active skill icon
							setActiveIcon( btn );

							//Change mouse icon
							stPointI ptCursor(0,0);
							ptCursor.x += btn->GetWidth()/2;
							ptCursor.y += btn->GetHeight()/2;

							//GetGameCursor()->SetCursorImageType( eCursorSetAccess );
							GetGameCursor()->SetCursor( CCursor::CursorType_Drag, eCursorSetAccess );
							Engine_GetCursor()->SetImage( &rl, ptCursor );
						}
					}
				}
			}
			break;
	case EVENT_COMBOBOX_SELECTION_CHANGED: // 一键换装下拉
		{

			CGuiComboBox* pCombo = GetComboBox(14);
			int nIndex = pCombo->GetCurItem();

			CGuiControl* pTitle = GetControl(13);
			char szText2[100]={};
			sprintf(szText2,"本套快捷键: Shift + %u", nIndex + 1);
			pTitle->SetText(szText2);

			for(int i = 0;i<27;i++)
			{
				if( GetTable(i+500))
				{
					CRoleItem* pItem = ::GetCellItem(GetTable(i+500),stPointI(0,0));
					if( pItem )
					{
						//如果想再copy一遍则直接返回
						//pItem->ClearItemImage();
						pItem->OnRemove();
						delete pItem;
						pItem = NULL;
					}
				}
				CRoleItem* pItem = GetScene()->GetMainCharacter()->FindItemByID(m_OneKeyEquip[nIndex].IdTable[i]);
				if(pItem)
				{
					pItem->CopyTo(GetTable(i+500));
				}
			}
		setSkillIconID(m_OneKeyEquip[nIndex].dwSkillIconID); //sky 换装技能
		}
		break;

	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiUserInfo::UpdateFabao(void)
{
		fbsx1=0;
		fbsx2=0;
		fbsx3=0;
		fbsx4=0;
		fbsx5=0;
		fblevel=0;
		fbfjlevel=0;
		//法宝属性
		CRoleItem* pItem = GetEquipItem(EQUIPCELLTYPE_FABAO);
		if(pItem)
		{
			fbsx1+= pItem->GetObject()->maxpdamage+pItem->GetObject()->m20axpdamage;
			fbsx2+= pItem->GetObject()->maxmdamage+pItem->GetObject()->m20axmdamage;
			fbsx3+= pItem->GetObject()->pdefence+pItem->GetObject()->p20defence;
			fbsx4+= pItem->GetObject()->mdefence+pItem->GetObject()->m20defence;
			fbsx5+= pItem->GetObject()->maxhp+pItem->GetObject()->m20axhp;
			fblevel+=pItem->GetObject()->retain20;
		}
		CRoleItem* pItem2 = GetEquipItem(EQUIPCELLTYPE_FABAOFJ1);
		if(pItem2)
		{
			fbsx1+= pItem2->GetObject()->maxpdamage+pItem2->GetObject()->m20axpdamage;
			fbsx2+= pItem2->GetObject()->maxmdamage+pItem2->GetObject()->m20axmdamage;
			fbsx3+= pItem2->GetObject()->pdefence+pItem2->GetObject()->p20defence;
			fbsx4+= pItem2->GetObject()->mdefence+pItem2->GetObject()->m20defence;
			fbsx5+= pItem2->GetObject()->maxhp+pItem2->GetObject()->m20axhp;
			fbfjlevel+=pItem2->GetObject()->retain20;
		}
		CRoleItem* pItem3 = GetEquipItem(EQUIPCELLTYPE_FABAOFJ2);
		if(pItem3)
		{
			fbsx1+= pItem3->GetObject()->maxpdamage+pItem3->GetObject()->m20axpdamage;
			fbsx2+= pItem3->GetObject()->maxmdamage+pItem3->GetObject()->m20axmdamage;
			fbsx3+= pItem3->GetObject()->pdefence+pItem3->GetObject()->p20defence;
			fbsx4+= pItem3->GetObject()->mdefence+pItem3->GetObject()->m20defence;
			fbsx5+= pItem3->GetObject()->maxhp+pItem3->GetObject()->m20axhp;
			fbfjlevel+=pItem3->GetObject()->retain20;
		}
		CRoleItem* pItem4 = GetEquipItem(EQUIPCELLTYPE_FABAOFJ3);
		if(pItem4)
		{
			fbsx1+= pItem4->GetObject()->maxpdamage+pItem4->GetObject()->m20axpdamage;
			fbsx2+= pItem4->GetObject()->maxmdamage+pItem4->GetObject()->m20axmdamage;
			fbsx3+= pItem4->GetObject()->pdefence+pItem4->GetObject()->p20defence;
			fbsx4+= pItem4->GetObject()->mdefence+pItem4->GetObject()->m20defence;
			fbsx5+= pItem4->GetObject()->maxhp+pItem4->GetObject()->m20axhp;
			fbfjlevel+=pItem4->GetObject()->retain20;
		}
		CRoleItem* pItem5 = GetEquipItem(EQUIPCELLTYPE_FABAOFJ4);
		if(pItem5)
		{
			fbsx1+= pItem5->GetObject()->maxpdamage+pItem5->GetObject()->m20axpdamage;
			fbsx2+= pItem5->GetObject()->maxmdamage+pItem5->GetObject()->m20axmdamage;
			fbsx3+= pItem5->GetObject()->pdefence+pItem5->GetObject()->p20defence;
			fbsx4+= pItem5->GetObject()->mdefence+pItem5->GetObject()->m20defence;
			fbsx5+= pItem5->GetObject()->maxhp+pItem5->GetObject()->m20axhp;
			fbfjlevel+=pItem5->GetObject()->retain20;
		}
		CRoleItem* pItem6 = GetEquipItem(EQUIPCELLTYPE_FABAOFJ5);
		if(pItem6)
		{
			fbsx1+= pItem6->GetObject()->maxpdamage+pItem6->GetObject()->m20axpdamage;
			fbsx2+= pItem6->GetObject()->maxmdamage+pItem6->GetObject()->m20axmdamage;
			fbsx3+= pItem6->GetObject()->pdefence+pItem6->GetObject()->p20defence;
			fbsx4+= pItem6->GetObject()->mdefence+pItem6->GetObject()->m20defence;
			fbsx5+= pItem6->GetObject()->maxhp+pItem6->GetObject()->m20axhp;
			fbfjlevel+=pItem6->GetObject()->retain20;
		}
		CRoleItem* pItem7 = GetEquipItem(EQUIPCELLTYPE_FABAOFJ6);
		if(pItem7)
		{
			fbsx1+= pItem7->GetObject()->maxpdamage+pItem7->GetObject()->m20axpdamage;
			fbsx2+= pItem7->GetObject()->maxmdamage+pItem7->GetObject()->m20axmdamage;
			fbsx3+= pItem7->GetObject()->pdefence+pItem7->GetObject()->p20defence;
			fbsx4+= pItem7->GetObject()->mdefence+pItem7->GetObject()->m20defence;
			fbsx5+= pItem7->GetObject()->maxhp+pItem7->GetObject()->m20axhp;
			fbfjlevel+=pItem7->GetObject()->retain20;
		}
		CRoleItem* pItem8 = GetEquipItem(EQUIPCELLTYPE_FABAOFJ7);
		if(pItem8)
		{
			fbsx1+= pItem8->GetObject()->maxpdamage+pItem8->GetObject()->m20axpdamage;
			fbsx2+= pItem8->GetObject()->maxmdamage+pItem8->GetObject()->m20axmdamage;
			fbsx3+= pItem8->GetObject()->pdefence+pItem8->GetObject()->p20defence;
			fbsx4+= pItem8->GetObject()->mdefence+pItem8->GetObject()->m20defence;
			fbsx5+= pItem8->GetObject()->maxhp+pItem8->GetObject()->m20axhp;
			fbfjlevel+=pItem8->GetObject()->retain20;
		}
		CRoleItem* pItem9 = GetEquipItem(EQUIPCELLTYPE_FABAOFJ8);
		if(pItem9)
		{
			fbsx1+= pItem9->GetObject()->maxpdamage+pItem9->GetObject()->m20axpdamage;
			fbsx2+= pItem9->GetObject()->maxmdamage+pItem9->GetObject()->m20axmdamage;
			fbsx3+= pItem9->GetObject()->pdefence+pItem9->GetObject()->p20defence;
			fbsx4+= pItem9->GetObject()->mdefence+pItem9->GetObject()->m20defence;
			fbsx5+= pItem9->GetObject()->maxhp+pItem9->GetObject()->m20axhp;
			fbfjlevel+=pItem9->GetObject()->retain20;
		}
		CRoleItem* pItem10 = GetEquipItem(EQUIPCELLTYPE_FABAOFJ9);
		if(pItem10)
		{
			fbsx1+= pItem10->GetObject()->maxpdamage+pItem10->GetObject()->m20axpdamage;
			fbsx2+= pItem10->GetObject()->maxmdamage+pItem10->GetObject()->m20axmdamage;
			fbsx3+= pItem10->GetObject()->pdefence+pItem10->GetObject()->p20defence;
			fbsx4+= pItem10->GetObject()->mdefence+pItem10->GetObject()->m20defence;
			fbsx5+= pItem10->GetObject()->maxhp+pItem10->GetObject()->m20axhp;
			fbfjlevel+=pItem10->GetObject()->retain20;
		}
		CRoleItem* pItem11 = GetEquipItem(EQUIPCELLTYPE_FABAOFJ10);
		if(pItem11)
		{
			fbsx1+= pItem11->GetObject()->maxpdamage+pItem11->GetObject()->m20axpdamage;
			fbsx2+= pItem11->GetObject()->maxmdamage+pItem11->GetObject()->m20axmdamage;
			fbsx3+= pItem11->GetObject()->pdefence+pItem11->GetObject()->p20defence;
			fbsx4+= pItem11->GetObject()->mdefence+pItem11->GetObject()->m20defence;
			fbsx5+= pItem11->GetObject()->maxhp+pItem11->GetObject()->m20axhp;
			fbfjlevel+=pItem11->GetObject()->retain20;
		}
		CRoleItem* pItem12 = GetEquipItem(EQUIPCELLTYPE_FABAOFJ11);
		if(pItem12)
		{
			fbsx1+= pItem12->GetObject()->maxpdamage+pItem12->GetObject()->m20axpdamage;
			fbsx2+= pItem12->GetObject()->maxmdamage+pItem12->GetObject()->m20axmdamage;
			fbsx3+= pItem12->GetObject()->pdefence+pItem12->GetObject()->p20defence;
			fbsx4+= pItem12->GetObject()->mdefence+pItem12->GetObject()->m20defence;
			fbsx5+= pItem12->GetObject()->maxhp+pItem12->GetObject()->m20axhp;
			fbfjlevel+=pItem12->GetObject()->retain20;
		}
		CRoleItem* pItem13 = GetEquipItem(EQUIPCELLTYPE_FABAOFJ12);
		if(pItem13)
		{
			fbsx1+= pItem13->GetObject()->maxpdamage+pItem13->GetObject()->m20axpdamage;
			fbsx2+= pItem13->GetObject()->maxmdamage+pItem13->GetObject()->m20axmdamage;
			fbsx3+= pItem13->GetObject()->pdefence+pItem13->GetObject()->p20defence;
			fbsx4+= pItem13->GetObject()->mdefence+pItem13->GetObject()->m20defence;
			fbsx5+= pItem13->GetObject()->maxhp+pItem13->GetObject()->m20axhp;
			fbfjlevel+=pItem13->GetObject()->retain20;
		}
}

// 展示法宝
bool CGuiUserInfo::ShowFabao(bool type)
{
	// if (!type)
	// {
	// 	fabao_show = false;
	// 	stPutFabaoPetCmd cmds;
	// 	SEND_USER_CMD(cmds);
	// 	return false;
	// }
	CRoleItem *pItem = GetEquipItem(EQUIPCELLTYPE_FABAO);
	if (pItem)
	{
		stCallFabaoPetCmd cmd;
		if (pItem->GetObject()->dwObjectID == 20000) // 混天绫
		{
			cmd.id = 20000;
			fabao_show = true;
			SEND_USER_CMD(cmd);
		}
		else if (pItem->GetObject()->dwObjectID == 20001) // 混天绫
		{
			cmd.id = 20001;
			fabao_show = true;
			SEND_USER_CMD(cmd);
		}
		else if (pItem->GetObject()->dwObjectID == 20002) // 混天绫
		{
			cmd.id = 20002;
			fabao_show = true;
			SEND_USER_CMD(cmd);
		}
		else if (pItem->GetObject()->dwObjectID == 20003) // 混天绫
		{
			cmd.id = 20003;
			fabao_show = true;
			SEND_USER_CMD(cmd);
		}
	}
	else
	{
		GameMessageBox("未检测到佩戴法宝！");
	}
	return true;
}

//法宝
void CGuiUserInfo::OnPutFocusItem(int index,CRoleItem* pRoleItem)
{
	UpdateFabao();
	
			CRoleItem* pItem = GetEquipItem(EQUIPCELLTYPE_FABAO);
			if(pItem)
			{
				char msg[256];
				if(pItem->GetObject()->dwObjectID == 20000)//混天绫
				{
					sprintf(msg,"混天绫 %u阶\n",pItem->GetObject()->retain20);
					GetStatic(1707)->SetText(msg);
					sprintf(msg,"%u\n",pItem->GetObject()->needlevel);
					GetStatic(1707)->SetTextColor(COLOR_ARGB(255,255,0,0));
					GetStatic(1708)->SetText(msg);
					GetStatic(1709)->SetText("永生");
					sprintf(msg,"增加物理攻击 %u\n",fbsx1);
					GetStatic(1710)->SetText(msg);
					sprintf(msg,"增加魔法攻击 %u\n",fbsx2);
					GetStatic(1711)->SetText(msg);
					sprintf(msg,"增加物理防御 %u\n",fbsx3);
					GetStatic(1712)->SetText(msg);
					sprintf(msg,"增加魔法防御 %u\n",fbsx4);
					GetStatic(1713)->SetText(msg);
					sprintf(msg,"增加生命上限 %u\n",fbsx5);
					GetStatic(1714)->SetText(msg);
					GetButton(1550)->SetBitmap(stResourceLocation("data\\interfaces.gl",80,201),1); 

				}
				else if(pItem->GetObject()->dwObjectID == 20001)
				{
					sprintf(msg,"金箍棒 %u阶\n",pItem->GetObject()->retain20);
					GetStatic(1707)->SetText(msg);
					sprintf(msg,"%u\n",pItem->GetObject()->needlevel);
					GetStatic(1707)->SetTextColor(COLOR_ARGB(255,255,0,0));
					GetStatic(1708)->SetText(msg);
					GetStatic(1709)->SetText("永生");
					sprintf(msg,"增加物理攻击 %u\n",fbsx1);
					GetStatic(1710)->SetText(msg);
					sprintf(msg,"增加魔法攻击 %u\n",fbsx2);
					GetStatic(1711)->SetText(msg);
					sprintf(msg,"增加物理防御 %u\n",fbsx3);
					GetStatic(1712)->SetText(msg);
					sprintf(msg,"增加魔法防御 %u\n",fbsx4);
					GetStatic(1713)->SetText(msg);
					sprintf(msg,"增加生命上限 %u\n",fbsx5);
					GetStatic(1714)->SetText(msg);
					GetButton(1550)->SetBitmap(stResourceLocation("data\\interfaces.gl",80,203),1); 
				}
				else if(pItem->GetObject()->dwObjectID == 20002)
				{
					sprintf(msg,"金刚圈 %u阶\n",pItem->GetObject()->retain20);
					GetStatic(1707)->SetText(msg);
					sprintf(msg,"%u\n",pItem->GetObject()->needlevel);
					GetStatic(1707)->SetTextColor(COLOR_ARGB(255,255,0,0));
					GetStatic(1708)->SetText(msg);
					GetStatic(1709)->SetText("永生");
					sprintf(msg,"增加物理攻击 %u\n",fbsx1);
					GetStatic(1710)->SetText(msg);
					sprintf(msg,"增加魔法攻击 %u\n",fbsx2);
					GetStatic(1711)->SetText(msg);
					sprintf(msg,"增加物理防御 %u\n",fbsx3);
					GetStatic(1712)->SetText(msg);
					sprintf(msg,"增加魔法防御 %u\n",fbsx4);
					GetStatic(1713)->SetText(msg);
					sprintf(msg,"增加生命上限 %u\n",fbsx5);
					GetStatic(1714)->SetText(msg);
					GetButton(1550)->SetBitmap(stResourceLocation("data\\interfaces.gl",80,202),1); 
				}
				else if(pItem->GetObject()->dwObjectID == 20003)
				{
					sprintf(msg,"八卦镜 %u阶\n",pItem->GetObject()->retain20);
					GetStatic(1707)->SetText(msg);
					GetStatic(1707)->SetTextColor(COLOR_ARGB(255,255,0,0));
					sprintf(msg,"%u\n",pItem->GetObject()->needlevel);
					GetStatic(1708)->SetText(msg);
					GetStatic(1709)->SetText("永生");
					sprintf(msg,"增加物理攻击 %u\n",fbsx1);
					GetStatic(1710)->SetText(msg);
					sprintf(msg,"增加魔法攻击 %u\n",fbsx2);
					GetStatic(1711)->SetText(msg);
					sprintf(msg,"增加物理防御 %u\n",fbsx3);
					GetStatic(1712)->SetText(msg);
					sprintf(msg,"增加魔法防御 %u\n",fbsx4);
					GetStatic(1713)->SetText(msg);
					sprintf(msg,"增加生命上限 %u\n",fbsx5);
					GetStatic(1714)->SetText(msg);
					GetButton(1550)->SetBitmap(stResourceLocation("data\\interfaces.gl",80,204),1); 
					
				}
			}
			else{
				char msg[256];
				GetStatic(1707)->SetText("暂未装备法宝");
				GetStatic(1708)->SetText("");	
				GetStatic(1709)->SetText("");
				sprintf(msg,"增加物理攻击 %u\n",fbsx1);
				GetStatic(1710)->SetText(msg);
				sprintf(msg,"增加魔法攻击 %u\n",fbsx2);
				GetStatic(1711)->SetText(msg);
				sprintf(msg,"增加物理防御 %u\n",fbsx3);
				GetStatic(1712)->SetText(msg);
				sprintf(msg,"增加魔法防御 %u\n",fbsx4);
				GetStatic(1713)->SetText(msg);
				sprintf(msg,"增加生命上限 %u\n",fbsx5);
				GetStatic(1714)->SetText(msg);
				GetButton(1550)->SetBitmap(stResourceLocation("data\\interfaces.gl",80,999),1); 
			}	

			char msg[256];
			sprintf(msg,"%u/5000\n",fblevel);
			GetStatic(1802)->SetText(msg);	
			sprintf(msg,"%u/12000\n",fbfjlevel);
			GetStatic(1803)->SetText(msg);	

			GetProcess(1800)->SetPos(fblevel);
			GetProcess(1801)->SetPos(fbfjlevel);
			
			

}




void CGuiUserInfo::UpdateYuanshen(void)
{
		yssx1=0;
		yssx2=0;
		yssx3=0;
		yssx4=0;
		yssx5=0;
		yslevel=0;
		ysfjlevel=0;
		//法宝属性
		CRoleItem* pItem = GetEquipItem(EQUIPCELLTYPE_YUANSHEN);
		if(pItem)
		{
			yssx1+= pItem->GetObject()->maxpdamage+pItem->GetObject()->m22axpdamage + pItem->GetObject()->m23axpdamage;
			yssx2+= pItem->GetObject()->maxmdamage+pItem->GetObject()->m22axmdamage + pItem->GetObject()->m23axmdamage;
			yssx3+= pItem->GetObject()->pdefence+pItem->GetObject()->p22defence + pItem->GetObject()->p23defence;
			yssx4+= pItem->GetObject()->mdefence+pItem->GetObject()->m22defence + pItem->GetObject()->m23defence;
			yssx5+= pItem->GetObject()->maxhp+pItem->GetObject()->m22axhp + pItem->GetObject()->m23axhp;
			yslevel+=pItem->GetObject()->retain22;
		}
		CRoleItem* pItem2 = GetEquipItem(EQUIPCELLTYPE_YUANSHENFJ1);
		if(pItem2)
		{
			yssx1+= pItem2->GetObject()->maxpdamage+pItem2->GetObject()->m22axpdamage + pItem2->GetObject()->m23axpdamage;
			yssx2+= pItem2->GetObject()->maxmdamage+pItem2->GetObject()->m22axmdamage + pItem2->GetObject()->m23axmdamage;
			yssx3+= pItem2->GetObject()->pdefence+pItem2->GetObject()->p22defence + pItem2->GetObject()->p23defence;
			yssx4+= pItem2->GetObject()->mdefence+pItem2->GetObject()->m22defence + pItem2->GetObject()->m23defence;
			yssx5+= pItem2->GetObject()->maxhp+pItem2->GetObject()->m22axhp + pItem2->GetObject()->m23axhp;
			ysfjlevel+=pItem2->GetObject()->retain22;
		}
		CRoleItem* pItem3 = GetEquipItem(EQUIPCELLTYPE_YUANSHENFJ2);
		if(pItem3)
		{
			yssx1+= pItem3->GetObject()->maxpdamage+pItem3->GetObject()->m22axpdamage + pItem3->GetObject()->m23axpdamage;
			yssx2+= pItem3->GetObject()->maxmdamage+pItem3->GetObject()->m22axmdamage + pItem3->GetObject()->m23axmdamage;
			yssx3+= pItem3->GetObject()->pdefence+pItem3->GetObject()->p22defence + pItem3->GetObject()->p23defence;
			yssx4+= pItem3->GetObject()->mdefence+pItem3->GetObject()->m22defence + pItem3->GetObject()->m23defence;
			yssx5+= pItem3->GetObject()->maxhp+pItem3->GetObject()->m22axhp + pItem3->GetObject()->m23axhp;
			ysfjlevel+=pItem3->GetObject()->retain22;
		}
		CRoleItem* pItem4 = GetEquipItem(EQUIPCELLTYPE_YUANSHENFJ3);
		if(pItem4)
		{
			yssx1+= pItem4->GetObject()->maxpdamage+pItem4->GetObject()->m22axpdamage + pItem4->GetObject()->m23axpdamage;
			yssx2+= pItem4->GetObject()->maxmdamage+pItem4->GetObject()->m22axmdamage + pItem4->GetObject()->m23axmdamage;
			yssx3+= pItem4->GetObject()->pdefence+pItem4->GetObject()->p22defence + pItem4->GetObject()->p23defence;
			yssx4+= pItem4->GetObject()->mdefence+pItem4->GetObject()->m22defence + pItem4->GetObject()->m23defence;
			yssx5+= pItem4->GetObject()->maxhp+pItem4->GetObject()->m22axhp + pItem4->GetObject()->m23axhp;
			ysfjlevel+=pItem4->GetObject()->retain22;
		}
		CRoleItem* pItem5 = GetEquipItem(EQUIPCELLTYPE_YUANSHENFJ4);
		if(pItem5)
		{
			yssx1+= pItem5->GetObject()->maxpdamage+pItem5->GetObject()->m22axpdamage + pItem5->GetObject()->m23axpdamage;
			yssx2+= pItem5->GetObject()->maxmdamage+pItem5->GetObject()->m22axmdamage + pItem5->GetObject()->m23axmdamage;
			yssx3+= pItem5->GetObject()->pdefence+pItem5->GetObject()->p22defence + pItem5->GetObject()->p23defence;
			yssx4+= pItem5->GetObject()->mdefence+pItem5->GetObject()->m22defence + pItem5->GetObject()->m23defence;
			yssx5+= pItem5->GetObject()->maxhp+pItem5->GetObject()->m22axhp + pItem5->GetObject()->m23axhp;
			ysfjlevel+=pItem5->GetObject()->retain22;
		}
		CRoleItem* pItem6 = GetEquipItem(EQUIPCELLTYPE_YUANSHENFJ5);
		if(pItem6)
		{
			yssx1+= pItem6->GetObject()->maxpdamage+pItem6->GetObject()->m22axpdamage + pItem6->GetObject()->m23axpdamage;
			yssx2+= pItem6->GetObject()->maxmdamage+pItem6->GetObject()->m22axmdamage + pItem6->GetObject()->m23axmdamage;
			yssx3+= pItem6->GetObject()->pdefence+pItem6->GetObject()->p22defence + pItem6->GetObject()->p23defence;
			yssx4+= pItem6->GetObject()->mdefence+pItem6->GetObject()->m22defence + pItem6->GetObject()->m23defence;
			yssx5+= pItem6->GetObject()->maxhp+pItem6->GetObject()->m22axhp + pItem6->GetObject()->m23axhp;
			ysfjlevel+=pItem6->GetObject()->retain22;
		}
		CRoleItem* pItem7 = GetEquipItem(EQUIPCELLTYPE_YUANSHENFJ6);
		if(pItem7)
		{
			yssx1+= pItem7->GetObject()->maxpdamage+pItem7->GetObject()->m22axpdamage + pItem7->GetObject()->m23axpdamage;
			yssx2+= pItem7->GetObject()->maxmdamage+pItem7->GetObject()->m22axmdamage + pItem7->GetObject()->m23axmdamage;
			yssx3+= pItem7->GetObject()->pdefence+pItem7->GetObject()->p22defence + pItem7->GetObject()->p23defence;
			yssx4+= pItem7->GetObject()->mdefence+pItem7->GetObject()->m22defence + pItem7->GetObject()->m23defence;
			yssx5+= pItem7->GetObject()->maxhp+pItem7->GetObject()->m22axhp + pItem7->GetObject()->m23axhp;
			ysfjlevel+=pItem7->GetObject()->retain22;
		}
		
}

//元神
void CGuiUserInfo::OnPutFocusItem2(int index,CRoleItem* pRoleItem)
{

		UpdateYuanshen();
		CRoleItem* pItem = GetEquipItem(EQUIPCELLTYPE_YUANSHEN);
		if(pItem)
		{
			if(pItem->GetObject()->retain22>0 && pItem->GetObject()->retain22<1000 )
			{			
				GetButton(1208)->SetBitmap(stResourceLocation("data\\interfaces3.gl",102,3),1); 		
			}
			else if(pItem->GetObject()->retain22>=1000 && pItem->GetObject()->retain22<2000 )
			{			
				GetButton(1208)->SetBitmap(stResourceLocation("data\\interfaces3.gl",102,4),1); 		
			}
			else if(pItem->GetObject()->retain22>=2000 && pItem->GetObject()->retain22<3000 )
			{			
				GetButton(1208)->SetBitmap(stResourceLocation("data\\interfaces3.gl",102,5),1); 		
			}
			else if(pItem->GetObject()->retain22>=3000 && pItem->GetObject()->retain22<4000 )
			{			
				GetButton(1208)->SetBitmap(stResourceLocation("data\\interfaces3.gl",102,6),1); 		
			}
			else if(pItem->GetObject()->retain22>=4000)
			{			
				GetButton(1208)->SetBitmap(stResourceLocation("data\\interfaces3.gl",102,7),1); 		
			}
			else{
				GetButton(1208)->SetBitmap(stResourceLocation("data\\interfaces3.gl",102,2),1); 
			}		
		}
		else{
			GetButton(1208)->SetBitmap(stResourceLocation("data\\interfaces3.gl",102,999),1); 
		}
			char msg[256];		
			sprintf(msg,"元神增加物理攻击 %u\n",yssx1);
			GetStatic(1210)->SetText(msg);
			sprintf(msg,"元神增加魔法攻击 %u\n",yssx2);
			GetStatic(1211)->SetText(msg);
			sprintf(msg,"元神增加物理防御 %u\n",yssx3);
			GetStatic(1212)->SetText(msg);
			sprintf(msg,"元神增加魔法防御 %u\n",yssx4);
			GetStatic(1213)->SetText(msg);
			sprintf(msg,"元神增加生命上限 %u\n",yssx5);
			GetStatic(1214)->SetText(msg);

}


//soke 自由加点
void CGuiUserInfo::SendAddUserProperty( WORD Wdtype , int nCount )
{
	stChangeUserPropertyUserCmd cmd;
	cmd.dwCount = nCount;
	cmd.dwType = Wdtype;
	SEND_USER_CMD(cmd);
}

void CGuiUserInfo::SetUserBody(byte EquipPosition,t_Object* pObject)
{
	switch(EquipPosition) 
	{
	case EQUIPCELLTYPE_BODY:
		{
			if (pObject == NULL)
				m_bEquipe = false;
			else
				m_bEquipe = true;

			if (m_bClothes==false)
				SetImage(m_pImage[0],EquipPosition,pObject);
		}
		break;
	 case EQUIPCELLTYPE_FASHION: //soke 时装
		{
			if ((pObject == NULL)||(pObject&&( GetObjectBase(pObject->dwObjectID)->dwType!=ItemType_FashionBody || GetObjectBase(pObject->dwObjectID)->dwType!=ItemType_HighFashionBody || GetObjectBase(pObject->dwObjectID)->dwType!=ItemType_XHighFashionBody || GetObjectBase(pObject->dwObjectID)->dwType!=ItemType_SHighFashionBody )))
				m_bClothes = false;
			else
				m_bClothes = true;

			if (m_bClothes)
				SetImage(m_pImage[0],EquipPosition,pObject);
			else if (m_bEquipe)
				SetImage(m_pImage[0],EquipPosition,GetEquipItem(EQUIPCELLTYPE_BODY)->GetObject());
			else
				SetImage(m_pImage[0],EquipPosition,NULL);
		}
		break;
	}

	if ((m_bEquipe==false)&&(m_bClothes==false))
		m_bShowBody = true;
	else
		m_bShowBody = false;
	
}

void CGuiUserInfo::SetUserImage(byte EquipPosition,t_Object* pObject)
{
	
    // 纸娃娃
	switch(EquipPosition) 
	{
		
	case EQUIPCELLTYPE_FASHION:
	case EQUIPCELLTYPE_BODY:
		SetUserBody(EquipPosition,pObject);
		break;
	case EQUIPCELLTYPE_HANDL:
		{
			SetImage(m_pImage[1],EquipPosition,pObject);
		}	
		break;
	case EQUIPCELLTYPE_HANDR:
		{
			//soke 武器可以显示动态图 
			
			SetImage(m_pImage[2],EquipPosition,pObject);
		}	
		break;
	case EQUIPCELLTYPE_PFAMULET:
		{
			//soke 披风可以显示动态图 
			SetImage(m_pImage[3],EquipPosition,pObject);
		}	
		break;
	case EQUIPCELLTYPE_CBAMULET://翅膀
		{
			//soke 翅膀可以显示动态图 
			SetImage(m_pImage[4],EquipPosition,pObject);
		}
		break;
	}

}

void CGuiUserInfo::SetImage(CGuiImage* pImage,byte EquipPosition,t_Object* pObject)
{

	if (pImage== NULL)
		return;
	
	pImage->Clear();

	if (pObject==NULL)
		return;
	
	ObjectBase_t* pObjectBase = GetObjectBase(pObject->dwObjectID);

	if (pObjectBase==NULL)
		return;
		
	//寒冰处理
	stResourceLocation rl0;
	if( m_bMainUser )
	{
		rl0 = GetObjectBasePicLocation(pObjectBase);
	}
	else{
		if(hanbing_state==1)
		{
			rl0 = GetObjectBasePicLocation(pObjectBase);
			
		}
		else{
			rl0 = GetObjectBasePicLocation2(pObjectBase);
			
		}
	}
	
	
	
	stResourceLocation rl1 = GetObjectBasePatternPicLocation(pObjectBase);
	stResourceLocation rl2 = GetObjectBasePattern2PicLocation(pObjectBase);
	
	if(!rl0.IsEmpty())
	{
		pImage->AddAnimation(rl0);
	}
	

	if(!rl1.IsEmpty())
	{	
		pImage->AddAnimation(rl1,  pObjectBase->dwColor);		//color
			
	}

	if(!rl2.IsEmpty())
	{
		pImage->AddAnimation(rl2,  pObject->color);		//color
			
	}


}

bool CGuiUserInfo::IsEquipedAutoHpItem( bool bMainUser /* = true */ ) //装备道具自动回血
{

	if ( bMainUser && !m_bMainUser )
		return false;

	if ( !bMainUser && m_bMainUser )
		return false;

	CRoleItem* pItem = GetEquipItem( EQUIPCELLTYPE_ADORN, EQUIPCELLTYPE_LEFT );
	if ( pItem && (pItem->GetObjectID() >= 18000) )
		pItem = NULL;

	if ( NULL == pItem )
	{
		pItem = GetEquipItem( EQUIPCELLTYPE_ADORN, EQUIPCELLTYPE_RIGHT );
		if ( pItem && (pItem->GetObjectID() >= 18000) )
			pItem = NULL;
	}

	return pItem != NULL;
}

CRoleItem* CGuiUserInfo::GetEquipItem( int equip_type,int x /* = 0 */ )
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

void CGuiUserInfo::RefreshBodyColor(DWORD color)
{
	CRoleItem* pRoleItem = GetShowClothes();

	if (pRoleItem)
	{
		t_Object* pObject = pRoleItem->GetObject();
		if (pObject)
		{
			pObject->color = color;
			SetUserImage(pRoleItem->GetLocation().y,pObject);
		}
	}

}

CRoleItem* CGuiUserInfo::FindItemInEquipListByFiveTypeConstID( const DWORD& dwID, const DWORD& fiveType )
{
	if ( m_bMainUser )
		return NULL;

	for(std::vector<CRoleItem*>::iterator itor=m_listItem.begin();itor!=m_listItem.end();++itor)
	{
		t_Object* obj = (*itor)->GetObject();
		if ( (obj->dwObjectID == dwID) && (obj->fivetype == fiveType) )
		{
			return (*itor);
			break;
		}
	}

	return NULL;
}


void CGuiUserInfo::SetHorseData( t_HorseData* pData )
{
	if( pData )
	{
		//
		char szText[32];
		NpcBase_t* pNpcBase = GetNpcBase(pData->id);
		if( pNpcBase )
		{
			UserInfo_SetHorseImage(314,pNpcBase->dwImage);
		}
		
		UserInfo_SetStaticText(315,pData->name);
		UserInfo_SetHorseText(316,"等级:",lv);

		HorseProperty hp[20];
		m_vecHorseProperty.clear();

        #define HorsePropertySet(index, kk, vv, pp)	{if (vv > 0) {hp[index].name = kk; hp[index].value=vv; hp[index].isPer=pp; m_vecHorseProperty.push_back(hp[index]);}}

		HorsePropertySet(0, "增加主人力量",  pData->str, false)
		HorsePropertySet(1,"增加主人智力", pData->intel, false)
		HorsePropertySet(2,"增加主人敏捷", pData->dex, false)
		HorsePropertySet(3,"增加主人精神", pData->men, false)
		HorsePropertySet(4,"增加主人体质", pData->con, false)
		HorsePropertySet(5,"增加主人速度", pData->speed, false)
		HorsePropertySet(6,"增加主人物攻", pData->pdam, true)
		HorsePropertySet(7,"增加主人物防", pData->pdef, true)
		HorsePropertySet(8,"增加主人魔攻", pData->mdam, true)
		HorsePropertySet(9,"增加主人魔防", pData->mdef, true)
		HorsePropertySet(10,"增加主人最大生命值", pData->maxhp, false)
		HorsePropertySet(11,"增加主人最大魔法值", pData->maxmp, false)
		HorsePropertySet(12,"增加主人抗减速", pData->slowRes, false)
		HorsePropertySet(13,"增加主人抗失明", pData->blindRes, false)
		HorsePropertySet(14,"增加主人抗石化", pData->petrifyRes, false) 
		HorsePropertySet(15,"增加主人抗冰冻", pData->freezeRes, false)
		HorsePropertySet(16,"增加主人抗混乱", pData->chaosRes, false)
		HorsePropertySet(17,"增加主人抗晕眩", pData->faintRes, false)
		HorsePropertySet(18,"增加主人抗麻痹", pData->lullRes, false)
		HorsePropertySet(19,"增加主人抗毒", pData->poisonRes, false)

		m_pHorseListBox->RemoveAllItems();

		int num = 0;
		for( size_t i = 0 ; i < m_vecHorseProperty.size(); i++)
		{
			int width = 175;
			int index = 0;
			if (num++ % 2 == 0) 
			{
				m_pHorseListBox->AddItem("",&(m_vecHorseProperty[i]));
			} 
			else 
			{
				index = 1;
				width = 190;
			}

			int appendIndex = m_pHorseListBox->GetItemCount()-1;
			std::stringstream ssText;
			std::stringstream ssText2;
			if (m_vecHorseProperty[i].isPer) 
			{
				ssText <<m_vecHorseProperty[i].name << m_vecHorseProperty[i].value << "%";
			} 
			else 
			{
				ssText << m_vecHorseProperty[i].name << m_vecHorseProperty[i].value;
			}

			m_pHorseListBox->SetItemText2(appendIndex, index, ssText.str().c_str(), width); 

		}
		return ;		
	}
	else
	{
		//
		UserInfo_SetHorseImage(314,-1);
		UserInfo_SetStaticText(315,"");
		UserInfo_SetStaticText(316,"");
		
		//sky 清除列表
		m_pHorseListBox->RemoveAllItems();
	}
}
