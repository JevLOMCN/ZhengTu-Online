#include "public.h"
#include "../Magic/iAnimation.h"
#include ".\character.h"
#include "./GameGuiManager.h"
#include "./GuiItem.h"
#include "./GuiSkill.h"
#include "./GuiMiniMap.h"
#include "./GuiMain.h"
#include "./GuiFloatAccessKey.h"
#include "./GuiOtherAccessKey.h"
#include "./Character.h"
#include "./GameScene.h"
#include "./GameCursor.h"
#include "./GuiUserInfo.h"
#include "./GuiTeam.h"
#include "./GuiWorldConn.h"
#include "./Game.h"
#include "./RoleItem.h"
#include "./CharacterProperty.h"
#include "./GuiShopDialog.h"
#include "./GuiOthersShopDialog.h"
#include "./Country.h"
#include "./GameAppation.h"

extern bool g_bAutoFight;

#define HAND_ATTACK_SOUND 20008
#define HAND_HIT_SOUND	 10008
#define BOW_EMPTY_ATTACK_SOUND 20008
#define BOW_EMPTY_HIT_SOUND	10008

#define KEEP_SHOW_TIME  8000
#define START_HIDE_TIME 7000

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */

bool ShowCountryStar(DWORD star,char* pStr,DWORD& color)
{
	FUNCTION_BEGIN;

	static char szText[64];
	
	szText[0]=0;
	
	if (pStr==NULL) return false;
	if (star==0) return false;
	if (star>36) star = 36;
	star--;
	int nRate = star / 9;
	int nMod = star % 9;


	for (int i=0;i<=nMod;i++)
		strncat(szText,"★",sizeof(szText));
	
	switch(nRate) {
	case 0:
		color = COLOR_ARGB(255,255,255,255);
		break;
	case 1:
		color = COLOR_ARGB(255,0,86,234);
		break;
	case 2:
		color = COLOR_ARGB(255,255,222,3);
		break;
	case 3:
		color = COLOR_ARGB(255,0,255,137);
		break;
	}
	
	
	sprintf(pStr,"%s",szText);
	return true;

	FUNCTION_END;
}

stBitmapShow::stBitmapShow()
{
	rl.SetFileName("data\\icons.gl");
	rl.group = 7;
	pBitmps = NULL;
	alpha = 0;
	rcClip.SetRect(0,0,0,0);
}

inline bool stBitmapShow::IsNeedShow()
{
	if( pBitmps==NULL || alpha == 0 ) 
		return false;
	else 
		return true;
}

void stBitmapShow::StartShow()
{
	dwStartTime = xtimeGetTime();
	alpha = 255;
}

void stBitmapShow::SetFrame(int nFrame)
{
	if( nFrame == rl.frame) return; // must be in the same group,else will make a error 
	rl.frame = nFrame;
	pBitmps = GetDevice()->FindBitmaps(&rl);
	if( pBitmps)
	{
		IBitmap * pBitmap =  pBitmps->GetBitmap(pBitmps->GetFrameCount()/2);
		if( pBitmap)
		{
			rcClip = pBitmap->GetClipRect();
		}
	}	
}

void stBitmapShow::Run()
{
	if(IsNeedShow())
	{
		DWORD curtime = xtimeGetTime();
		DWORD diff = curtime - dwStartTime;
		if( diff > KEEP_SHOW_TIME )
		{
			//pBitmps  = NULL;
			alpha = 0 ;
			return ;
		}
		int alpha = 255;
		if(diff > START_HIDE_TIME) 
			alpha = (KEEP_SHOW_TIME - curtime ) *255 /( KEEP_SHOW_TIME - START_HIDE_TIME);
	}
}

void stBitmapShow::Render(const POINT & pt)
{
	if(IsNeedShow())
	{	
		stPointI pt2 = pt;
		pt2.x -= (rcClip.left + rcClip.Width()/2);
		pt2.y -= (rcClip.top+rcClip.Height());
		pBitmps->RenderAni(pt2.x,pt2.y,xtimeGetTime(),NULL,NULL,COLOR_ARGB(alpha,255,255,255),Blend_Null);  
	}
}


DWORD GetNameColor(DWORD dwGoodNess)
{
	FUNCTION_BEGIN;

	DWORD dwColor = -1;

	if (dwGoodNess & GOODNESS_ATT)
	{
		dwColor=COLOR_ARGB(255,63,43,0);
	}
	else
	{
		DWORD dw=0x0000FFFF;
		short goodness=(short)(dwGoodNess & dw);
		switch(goodness) {
		case GOODNESS_2_1:
		case GOODNESS_2_2:
			dwColor=COLOR_ARGB(255,255,255,255);
			break;
		default:
			{
				if (goodness<(short)(GOODNESS_1 & 0x0000FFFF))
					dwColor=COLOR_ARGB(255,3,106,288);
				else if (goodness<(short)(GOODNESS_2_1 & dw))
					dwColor=COLOR_ARGB(255,36,203,173);
				else if (goodness<=(short)(GOODNESS_3 & dw))
					dwColor=COLOR_ARGB(255,255,155,21);
				else if (goodness<=(short)(GOODNESS_4 & dw))
					dwColor=COLOR_ARGB(255,225,97,21);
				else if (goodness<=(short)(GOODNESS_5 & dw))
					dwColor=COLOR_ARGB(255,200,33,58);
				else 
					dwColor=COLOR_ARGB(255,225,18,24);
			}
			break;
		}
	}

	return dwColor;

	FUNCTION_END;
}


CCharacter::CCharacter(void)
: m_tiletype(-1)
, m_ptPrevGrid(-1,-1)
, m_dwStar(0)
, m_bShowWarFlag(FALSE)
{
	FUNCTION_BEGIN;

	m_dwSceneObjectType |= OBJECT_CHARACTER;
	memset(&m_data,0,sizeof(m_data));
	m_data.sculpt.dwBodyID = -1;
	m_data.sculpt.dwBodyKind = -1;
	memset(m_countryName,0,MAX_NAMESIZE);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CCharacter::~CCharacter(void)
{
	FUNCTION_BEGIN;

	Release();

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CCharacter::Release()
{
	FUNCTION_BEGIN;

	//If this our interest 
	if ( GetScene()->m_pInterestNpc && ( GetScene()->m_pInterestNpc->IsCharacter() ) )
	{
		if ( GetScene()->m_pInterestNpc == this )
		{
			GetScene()->UnSelectInterestNpc();
			if ( GetGameGuiManager()->m_guiCharacterProperty )
				GetGameGuiManager()->m_guiCharacterProperty->SetVisible( false );
		}
	}
	
	if( GetScene()->m_pMouseOverObject == this){
		GetScene()->m_pMouseOverObject = NULL;
	}

	//Delete this from Sept && Tong Name request list
	//g_UserLocalInfo.DeleteCharacter( this );

	memset(&m_data,0,sizeof(m_data));
	CNpc::Release();

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pCmd : 描述
 * \return 返回值的描述
 */
bool CCharacter::SetAttrib(::t_MapUserData* pCmd)
{
	FUNCTION_BEGIN;

#ifdef _WYC_OPT_SER_
	//优化:服务器传递用户信息,帮派等只传id号,客户端做一个对应关系
	//为了避免程序结构大范围改变,采取收到消息后,先拷贝到一个临时结构,然后再这个结构里面赋值
	wyc::t_MapUserData temp;
	bool isFindid2String_union = false;
	bool isFindid2String_sept = false;
	memcpy(&temp,pCmd,sizeof(temp));
	
	strcpy(temp.unionName,"");
	strcpy(temp.septName,"");

	if ( pCmd->dwUnionID != 0 )
	{
		g_UserLocalInfo.m_pIter = g_UserLocalInfo.m_mapUnionName.find(pCmd->dwUnionID);
		if (g_UserLocalInfo.m_pIter == g_UserLocalInfo.m_mapUnionName.end())
		{
			//从request id map 里面确定是否请求过
			g_UserLocalInfo.m_pRequestIter = g_UserLocalInfo.curRequesetUnionId.find(pCmd->dwUnionID);
			if (g_UserLocalInfo.m_pRequestIter == g_UserLocalInfo.curRequesetUnionId.end())
			{
				//向服务器请求帮会名称,并加入请求id队,防止多次向服务器请求
				GetScene()->AddRequestUnionSeptNameCmd( CGameScene::RequestCmd_UnionName, temp.dwUnionID );
				/*stReqUnionNameCmd cmd;
				cmd.dwUnionID = temp.dwUnionID;
				SEND_USER_CMD( cmd );*/

				vRequestCharacter	vCharacter;
				vCharacter.clear();
				vCharacter.push_back( GetID() );
				g_UserLocalInfo.curRequesetUnionId.insert( D_Character_Pair ( temp.dwUnionID , vCharacter) );
				TRACE("--wyc__向服务器请求帮会名称__wyc--\n");
			}
			else
			{
				vRequestCharacter& vCharacter = (*(g_UserLocalInfo.m_pRequestIter)).second;
				vRequestCharacter::iterator iter;
				for( iter=vCharacter.begin();iter!=vCharacter.end();++iter )
				{
					if ( (*iter) == GetID() )
						break;
				}
				if ( iter == vCharacter.end() )
					vCharacter.push_back( GetID() );
			}
		}
		else
		{
			strcpy(temp.unionName,g_UserLocalInfo.m_mapUnionName.find(pCmd->dwUnionID)->second.c_str());
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	if ( pCmd->dwSeptID != 0 )
	{
		g_UserLocalInfo.m_pIter = g_UserLocalInfo.m_mapSpetName.find(pCmd->dwSeptID);
		if (g_UserLocalInfo.m_pIter == g_UserLocalInfo.m_mapSpetName.end())
		{
			//从request id map 里面确定是否请求过
			g_UserLocalInfo.m_pRequestIter = g_UserLocalInfo.curRequesetSpetId.find(pCmd->dwSeptID);
			if (g_UserLocalInfo.m_pRequestIter == g_UserLocalInfo.curRequesetSpetId.end())
			{
				//向服务器请求家族名称,并加入请求id队,防止多次向服务器请求
				GetScene()->AddRequestUnionSeptNameCmd( CGameScene::RequestCmd_SeptName, temp.dwSeptID );
				/*stReqSeptNameCmd cmd;
				cmd.dwSeptID = temp.dwSeptID;
				SEND_USER_CMD( cmd );*/
				vRequestCharacter	vCharacter;
				vCharacter.clear();
				vCharacter.push_back( GetID() );
				g_UserLocalInfo.curRequesetSpetId.insert( D_Character_Pair ( temp.dwSeptID , vCharacter) );
				TRACE("--wyc__向服务器请求家族名称__wyc--\n");
			}
			else
			{
				vRequestCharacter& vCharacter = (*(g_UserLocalInfo.m_pRequestIter)).second;
				vRequestCharacter::iterator iter;
				for( iter=vCharacter.begin();iter!=vCharacter.end();++iter )
				{
					if ( (*iter) == GetID() )
						break;
				}
				if ( iter == vCharacter.end() )
					vCharacter.push_back( GetID() );
			}
		}
		else
		{
			strcpy(temp.septName,g_UserLocalInfo.m_mapSpetName.find(pCmd->dwSeptID)->second.c_str());
		}
	}
			
	//////////////////////////////////////////////////////////////////////////
	pCmd = &temp;
    //////////////////////////////////////////////////////////////////////////
#endif

	bool bShopStart = bIsShopStart() ;

	bool bNeedRide = isset_state(pCmd->state,USTATE_RIDE) && !IsRide();
	bool bUpdateImage = (0 != memcmp(&m_data.sculpt,&pCmd->sculpt,sizeof(t_MapUserSculpt)) || bNeedRide) && !IsChangeFace() && !IsCombinationState();
	//bool bUpdateState = (memcmp(m_data.state,pCmd->state,sizeof(m_data.state)) != 0);
	bool bUpdateState = (memcmp(m_data.state,pCmd->state,sizeof(m_data.state)) != 0) || 
		pCmd->dwBodyColorCustom != m_data.dwBodyColorCustom ||
		pCmd->dwHairRGB != m_data.dwHairRGB;

	if( false == bIsShopStart() )
	{
		if( GetGameGuiManager()->m_guiOthersShop )
		{
			if ( GetID() == GetGameGuiManager()->m_guiOthersShop->GetOwnerID() )
			{
				GetGameGuiManager()->m_guiOthersShop->Close();
			}
		}
	}

	bool bIsMainRole = IsMainRole();

	if ( bIsMainRole && (m_data.movespeed < pCmd->movespeed) )
	{
		if ( GetScene()->GetMainCharacter() )
			GetScene()->GetMainCharacter()->ClearRushOutState();
	}

	//Wether need update caused by weapon equipment color change
	bool bNeedUpdateItemByEquipColor = (pCmd->dwBodyColorSystem != m_data.dwBodyColorSystem)
									||  (pCmd->dwLeftWeaponColor != m_data.dwLeftWeaponColor)
									||  (pCmd->dwRightWeaponColor != m_data.dwRightWeaponColor)
									||  (pCmd->dwBodyColorCustom != m_data.dwBodyColorCustom);
	
	//We don't use level for main character
	int oldLevel = m_data.level;
	//m_data = *pCmd;
	m_data = temp;

	if ( bIsMainRole )
	{
		m_data.level = oldLevel;
	}

	//Modify exploit value form server
	//Scale 0.01
	m_data.exploit = pCmd->exploit / 100;

	if( bIsMainRole )
	{
		if( GetGameGuiManager()->m_guiMain)
			GetGameGuiManager()->m_guiMain->InitIcon();
	}
	else
	{     
		if(  GetGameGuiManager()->m_guiCharacterProperty && GetGameGuiManager()->m_guiCharacterProperty->IsVisible() && GetScene()->m_pInterestNpc && GetScene()->m_pInterestNpc->GetID() == this->GetID())
		{
			GetGameGuiManager()->m_guiCharacterProperty->InitIcon();
		}
	}

	SetName(pCmd->name);
	m_dwID = pCmd->dwUserTempID;
	m_bDeleted = false;
	AfterSetAttrib(bUpdateImage,bUpdateState);

	//设置摊位的名字
	if( bIsShopStart() )
	{
		char shopname[MAX_PATH];
		memset(shopname,0,MAX_PATH);
		sprintf(shopname,"%s 的摊位",GetName());
		m_ShopNameTip.SetShopName(shopname);
	}
	
	memset(m_countryName,0,MAX_NAMESIZE);

	CCountry::Instance()->GetCountryName(GetCountry(),m_countryName);

	if( bIsMainRole )
	{
		//触发"消除红名"事件
		if(bIsUserUnSetted(USER_SETTING_HELP_CLEARREDNAME))
		{
			DWORD goodness[4]={GOODNESS_4,GOODNESS_5,GOODNESS_6,GOODNESS_7};

			for(int i = 0;i <4;++i) 
			{
				if(pCmd->goodness == goodness[i] )
				{
					ShowHelpTip(USER_SETTING_HELP_CLEARREDNAME);
					break;
				}
			}			
		}
		CAccoutSaveInfo::GetSingleton()->SetUserName(pCmd->name);


		GetGameGuiManager()->InitMainCharacterInfo();
		if ( GetGameGuiManager()->m_guiSkill )
			GetGameGuiManager()->m_guiSkill->InitIcon();
		GetGameGuiManager()->m_guiItem->SetCharName(GetName());

		// Init access key status
		if ( GetGameGuiManager()->m_guiMain )
			GetGameGuiManager()->m_guiMain->RefreshKeyGroupState( GetScene()->GetMainCharacter()->GetPreferAccessGroup() );
		if ( GetGameGuiManager()->m_guiFloatAcessKey )
			GetGameGuiManager()->m_guiFloatAcessKey->RefreshKeyGroupState();
		if ( GetGameGuiManager()->m_guiOtherAccessKey )
			GetGameGuiManager()->m_guiOtherAccessKey->RefreshKeyGroupState();

		//Update item property just when equipment color changed
		if ( GetScene()->GetMainCharacter() )
		{
			if ( bNeedUpdateItemByEquipColor )
			{
				DWORD flag = CRoleItem::UPDATE_FLAG_IMAGE;

				GetScene()->GetMainCharacter()->UpdateAllItems( flag );
			}	
			GetScene()->GetMainCharacter()->ResetHorseOwnStatus();
		}
	}
	if( FALSE ==  CCountry::Instance()->GetCountryStar(GetCountry(),m_dwStar) )
	{
	       m_dwStar =  0 ;
	}

	CCountry::stDareCountryInfo & dareinfo = CCountry::Instance()->GetCountryWarInfo();
	m_bShowWarFlag =  dareinfo.IsCountryInWar(GetCountry());
        if( !m_bShowWarFlag )
	{
		if( GetScene()->GetMainCharacter()->GetCountry() == GetCountry() )
		{//与主角同国
			m_bShowWarFlag = dareinfo.IsAnyCountryInWar();
		}
	}
	return true;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwColor : 描述
 * \return 返回值的描述
 */
void CCharacter::SetBodyCustomColor(DWORD dwColor)
{
	m_pCharacter->ChangeColor(Ani_Part_BodyCustomColor,dwColor);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwColor : 描述
 * \return 返回值的描述
 */
void CCharacter::SetBodySystemColor(DWORD dwColor)
{
	m_pCharacter->ChangeColor(Ani_Part_BodySystemColor,dwColor);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param bUpdateImage : 描述
 * \param bUpdateState : 描述
 * \return 返回值的描述
 */
void CCharacter::AfterSetAttrib(bool bUpdateImage,bool bUpdateState)
{
	FUNCTION_BEGIN;

	CNpc::AfterSetAttrib(bUpdateImage,bUpdateState);
	m_pCharacter->SetSex(GetSex()); 
	
	m_pCharacter->ChangeColor(Ani_Part_BodySystemColor,GetBodySystemColor());
	m_pCharacter->ChangeColor(Ani_Part_BodyCustomColor,GetBodyCustomColor());
	m_pCharacter->ChangeColor(Ani_Part_Hair,GetHairColor());
	m_pCharacter->ChangeColor(Ani_Part_LeftWeaponColor,GetLeftWeaponColor());
	m_pCharacter->ChangeColor(Ani_Part_RightWeaponColor,GetRightWeaponColor());

	if(IsMainRole())
	{
		GetGameGuiManager()->m_guiItem->SetCharType(GetType());
		GetGameGuiManager()->m_guiUserInfo->RefreshBodyColor(GetBodyCustomColor());
	}

	m_pCharacter->SetSex(GetSex());

	if ( IsCombinationState() )
		GetInterface()->SetMoveSpeed( (DWORD)m_data.movespeed/2 );
	else
		GetInterface()->SetMoveSpeed( (DWORD)m_data.movespeed );

	if ( IsMainRole() )
		TRACE( "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$Server change move speed to =%d$$$$$$$$$$$$$$$$$$$$$$$$$\n", m_data.movespeed ); 

	GetInterface()->SetAttackSpeed(m_data.attackspeed/100.0f);

	GetInterface()->SetMagicSpeed(m_data.attackspeed/100.0f);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
bool CCharacter::ResetState()
{
	CNpc::ResetState();
	return true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
bool CCharacter::ResetImage()
{
	FUNCTION_BEGIN;

	int dwPicHairID	=	0;
	int dwPicBodyID	=	0;
	int dwPicWeaponLID=	0;
	int dwPicWeaponRID=	0;

	int dwPicWeaponLColorID = 0;
	int dwPicWeaponRColorID = 0;
	int dwPicBodyColorID = 0;

	//服装
	ObjectBase_t* pBody = GetObjectBase(m_data.sculpt.dwBodyID);
	if( pBody )
	{
		dwPicBodyID = pBody->dwActionPic;
		if(pBody->dwChannelNum != -1)
			dwPicBodyColorID = pBody->dwChannelNum;
		else
			dwPicBodyColorID = dwPicBodyID;
	}

	//处理弓箭左手武器
/*	ObjectBase_t* pWeaponL = GetObjectBase(m_data.sculpt.dwLeftHandID);
	if(pWeaponL)
	{
		Engine_WriteLogF("装备来了%s\n",pWeaponL->strName);
		
		if(GetScene()->GetMainCharacter()->GetProperty()->hanbing == 1)
		{
			if(pWeaponL->dwType == ItemType_Bow ) //sky 弓寒冰外形
			{
				dwPicWeaponLID = 63;
				if(pWeaponL->dwChannelNum != -1)
				     dwPicWeaponLColorID = pWeaponL->dwChannelNum;	
			}
		}
		else
		{
			if(pWeaponL->dwType == ItemType_Shield || pWeaponL->dwType == ItemType_Bow)
		    {
			   dwPicWeaponLID = pWeaponL->dwActionPic;
			   if(pWeaponL->dwChannelNum != -1)
				     dwPicWeaponLColorID = pWeaponL->dwChannelNum;
		    }
		}
	}*/

	//左手武器
	ObjectBase_t* pWeaponL = GetObjectBase(m_data.sculpt.dwLeftHandID);
	if(pWeaponL)
	{
		Engine_WriteLogF("装备来了%s\n",pWeaponL->strName);
		if(pWeaponL->dwType == ItemType_Shield)
		{
			dwPicWeaponLID = pWeaponL->dwActionPic;
			if(pWeaponL->dwChannelNum != -1)
				dwPicWeaponLColorID = pWeaponL->dwChannelNum;
		}
	}

	//右手武器
	ObjectBase_t* pWeaponR = GetObjectBase(m_data.sculpt.dwRightHandID);
	if(pWeaponR)
	{
		if(pWeaponR->dwType == ItemType_Dagger) //sky 利刃
		{
			Engine_WriteLogF("装备来了%s\n",pWeaponR->strName);
			//服装
			dwPicBodyID = (eSex_Male == this->GetSex()) ? 171: 172;
			if(pWeaponR->dwChannelNum != -1)
				dwPicBodyColorID = pWeaponR->dwChannelNum;
			else
				dwPicBodyColorID = dwPicBodyID;
		}

		if(GetProData()->hanbing == 1)
		{
			if(pWeaponR->dwType == ItemType_Bow ) //sky 处理弓箭弓
			{
				dwPicWeaponLID = 63;
				if(pWeaponR->dwChannelNum != -1)
					dwPicWeaponLColorID = pWeaponR->dwChannelNum;	
			}
			else if(pWeaponR->dwType == 155) //sky 枪
			{
				dwPicWeaponRID = 304;
				if(pWeaponR->dwChannelNum != -1)
					dwPicWeaponRColorID = pWeaponR->dwChannelNum;
			}
			else if(pWeaponR->dwType == 137) //sky 剑
			{
				dwPicWeaponRID = 303;
				if(pWeaponR->dwChannelNum != -1)
					dwPicWeaponRColorID = pWeaponR->dwChannelNum;
			}
			else if(pWeaponR->dwType == 136) //sky 利刃
			{
				dwPicWeaponRID = 245; //sky 没有寒冰的
				if(pWeaponR->dwChannelNum != -1)
					dwPicWeaponRColorID = pWeaponR->dwChannelNum;
			}
			else if(pWeaponR->dwType == 111) //sky 棍
			{
				dwPicWeaponRID = 307;
				if(pWeaponR->dwChannelNum != -1)
					dwPicWeaponRColorID = pWeaponR->dwChannelNum;
			}
			else if(pWeaponR->dwType == 110) //sky 扇
			{
				dwPicWeaponRID = 305;
				if(pWeaponR->dwChannelNum != -1)
					dwPicWeaponRColorID = pWeaponR->dwChannelNum;
			}
			else if(pWeaponR->dwType == 108) //sky 杖
			{
				dwPicWeaponRID = 306;
				if(pWeaponR->dwChannelNum != -1)
					dwPicWeaponRColorID = pWeaponR->dwChannelNum;
			}
			else if(pWeaponR->dwType == 106 || pWeaponR->dwType == 107) //sky 斧
			{
				dwPicWeaponRID = 302;
				if(pWeaponR->dwChannelNum != -1)
					dwPicWeaponRColorID = pWeaponR->dwChannelNum;
			}
			else if(pWeaponR->dwType == 105) //sky 剑
			{
				dwPicWeaponRID = 303;
				if(pWeaponR->dwChannelNum != -1)
					dwPicWeaponRColorID = pWeaponR->dwChannelNum;
			}
			else if(pWeaponR->dwType == 104 ) //sky 刀
			{
				dwPicWeaponRID = 301;
				if(pWeaponR->dwChannelNum != -1)
					dwPicWeaponRColorID = pWeaponR->dwChannelNum;
			}	
		}
/*处理弓箭	醉梦	
        else
		{
			if(pWeaponR->dwType != ItemType_Bow)
            {
				dwPicWeaponRID = pWeaponR->dwActionPic;
				if(pWeaponR->dwChannelNum != -1)
				   dwPicWeaponRColorID = pWeaponR->dwChannelNum;
			}
		}
	}*/
		else
		{
			if(pWeaponR->dwType == ItemType_Bow)
			{
				dwPicWeaponLID = pWeaponR->dwActionPic;
				if(pWeaponR->dwChannelNum != -1)
					dwPicWeaponLColorID = pWeaponR->dwChannelNum;
			}
			else
			{
				dwPicWeaponRID = pWeaponR->dwActionPic;
				if(pWeaponR->dwChannelNum != -1)
					dwPicWeaponRColorID = pWeaponR->dwChannelNum;
			}
		}
	}

 	if (pWeaponR)
 		m_pCharacter->SetArrowInRightHand( pWeaponR->dwType == ItemType_Arrow || pWeaponR->dwType == ItemType_CrossFlag );//sky 箭支和弓手战旗装备后外形动作正常
 	else
 		m_pCharacter->SetArrowInRightHand( false );

   if( dwPicBodyID == 0 )
    {
	    dwPicBodyID = GetCharTypeInfo(GetType()).actionpic;
    }

    ObjectBase_t* pBodykind = GetObjectBase(m_data.sculpt.dwBodyID);
	int PicbodydwID = NULL; //是否穿戴装备ID判定
	int Picbodykind = NULL; //是否穿戴装备类型判定
	
	ObjectBase_t* pWeaponRkind = GetObjectBase(m_data.sculpt.dwRightHandID);
	int PicWeaponRkind = NULL; //是否穿戴装备类型判定
	if(pWeaponRkind)  PicWeaponRkind = pWeaponRkind->dwType;
	
    //限制125类型和刺客武器还有个别时装不带发型外部
	if(pBodykind) 
	{
		Picbodykind = pBodykind->dwType;
		PicbodydwID = pBodykind->dwID;

		if(Picbodykind == 125 ){ }
		else if(PicbodydwID == 10030 || PicbodydwID == 19080 || PicbodydwID == 19130 || PicbodydwID == 19180 || PicbodydwID == 19230 || PicbodydwID == 19280){ }
		else if(PicWeaponRkind == 136 ){ }
		else
		{	
			dwPicHairID = m_data.sculpt.dwHairID;
			HairStyle_t* p = GetHairStyle(m_data.sculpt.dwHairID);
			if(p) dwPicHairID = p->dwActionPicID;
		}
    }
	else
	{
		dwPicHairID = m_data.sculpt.dwHairID;
		HairStyle_t* p = GetHairStyle(m_data.sculpt.dwHairID);
		if(p) dwPicHairID = p->dwActionPicID;
	}

	//修复指定时装id 不显示武器与盾牌动作外观【西游时装专用】
	if((PicbodydwID >= 200158 && PicbodydwID <= 200162) ||(PicbodydwID >= 210158 && PicbodydwID <= 210162))
	{
		dwPicWeaponLID = 0;
		dwPicWeaponLColorID = 0;
		  dwPicWeaponRID = 0;
		dwPicWeaponRColorID = 0;
	}


	static std::vector<int> aID;

	aID.resize(Ani_Part_Num);

#ifdef _HAS_WEAPON_SHADOW
	aID[Ani_Part_LeftWeaponShadow] = dwPicWeaponLID;
	aID[Ani_Part_RightWeaponShadow] = dwPicWeaponRID;
#endif

	aID[Ani_Part_LeftWeapon] = dwPicWeaponLID;
	aID[Ani_Part_RightWeapon] = dwPicWeaponRID;

#ifdef _HAS_WEAPON_COLOR
	aID[Ani_Part_LeftWeaponColor] = dwPicWeaponLColorID;
	aID[Ani_Part_RightWeaponColor] = dwPicWeaponRColorID;
#endif

	aID[Ani_Part_Body] = dwPicBodyID;
	aID[Ani_Part_BodySystemColor] = dwPicBodyColorID;
	aID[Ani_Part_BodyCustomColor] = dwPicBodyID;
	aID[Ani_Part_Hair] = dwPicHairID;

	m_pCharacter->CreateCharacter(0,aID,false);

	if(m_pCharacter->IsWait())
	{
		//m_pCharacter->TurnTo(m_data.);
	}
	OnEquipChanged( );
	//In case of is ride state
	if(!m_bDied)
	{
		bool bRide = isset_state(GetState(),USTATE_RIDE);
		if( bRide )
			OnRide(bRide);
	}

	return true;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param time_elasped : 描述
 * \return 返回值的描述
 */
void CCharacter::Run(long time_elasped)
{
	FUNCTION_BEGIN;

	CNpc::Run(time_elasped);
	if(m_ptPrevGrid != m_ptGrid)
	{
		OnGridPosChange(m_ptPrevGrid,m_ptGrid);
		m_ptPrevGrid = m_ptGrid;
	}
    m_BitmapShow.Run();

	POINT dir = m_pCharacter->GetMoveDir();
	m_AniArraySwim.Turn(dir);
	m_AniArraySwimStand.Turn(dir);
	m_AniArrayFly.Turn(dir);

	m_AniArraySwim.Run(time_elasped);
	m_AniArraySwimStand.Run(time_elasped);
	//soke 挖宝
	POINT dir5 = m_pCharacter->GetMoveDir();
	m_AniArraySwim5.Turn(dir5);
	m_AniArraySwimStand5.Turn(dir5);
	m_AniArraySwim5.Run(time_elasped);
	m_AniArraySwimStand5.Run(time_elasped);
	
	m_AniArrayFly.Run(time_elasped);
	
	//跳舞动作
	m_AniArrayDance.Turn(dir);
	m_AniArrayDance.Run(time_elasped);
	
	//跳舞发型
	m2_AniArrayDance.Turn(dir);
	m2_AniArrayDance.Run(time_elasped);
	
	//跳舞衣服填充
	m3_AniArrayDance.Turn(dir);
	m3_AniArrayDance.Run(time_elasped);

	
	//自行车动作 by醉梦
	POINT dir6 = m_pCharacter->GetMoveDir();
	m_AniArrayBike.Turn(dir6);
	m_AniArrayBikeStand.Turn(dir6);
	m_AniArrayBike.Run(time_elasped);
	m_AniArrayBikeStand.Run(time_elasped);

	//自行车动作 by醉梦
	POINT dir7 = m_pCharacter->GetMoveDir();
	m_AniArrayBike2.Turn(dir7);
	m_AniArrayBikeStand2.Turn(dir7);
	m_AniArrayBike2.Run(time_elasped);
	m_AniArrayBikeStand2.Run(time_elasped);

	//战车
	POINT dir8 = m_pCharacter->GetMoveDir();
	m_AniArrayZC.Turn(dir8);
	m_AniArrayZCStand.Turn(dir8);
	m_AniArrayZC.Run(time_elasped);
	m_AniArrayZCStand.Run(time_elasped);


	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ptMapPixelOffset : 描述
 * \param dwColor : 描述
 * \return 返回值的描述
 */
void CCharacter::Draw(POINT ptMapPixelOffset,DWORD dwColor)
{
	FUNCTION_BEGIN;
	
	if (isset_state(this->GetState(), USTATE_SPA))
	{
		this->SetInSpaState(true);
		POINT pt = GetPos() - ptMapPixelOffset;
		POINT posCenter = m_pCharacter->GetDefaultCenter();

		if (this->IsMoving())
		{
			if(isset_state(GetState(),USTATE_SUOGUDAN))
			{
				posCenter.x += 130;
				posCenter.y += 145;
				m_AniArraySwim.DrawAtScreen2(0, pt, posCenter);
			}
			else{
				m_AniArraySwim.DrawAtScreen(0, pt, posCenter);
			}
			
		}
		else
		{
			if(isset_state(GetState(),USTATE_SUOGUDAN))
			{
				posCenter.x += 130;
				posCenter.y += 145;
				m_AniArraySwimStand.DrawAtScreen2(0, pt, posCenter);
			}
			else{
				m_AniArraySwimStand.DrawAtScreen(0, pt, posCenter);
			}
			
		}

		m_iDrawTop = GetImageHeight();

		this->DrawEffect(ptMapPixelOffset);
	}
	else if (isset_state(this->GetState(), USTATE_USER_FLY))
	{
		this->SetInFlyState(true);

		POINT pt = GetPos() - ptMapPixelOffset;
		POINT posCenter = m_pCharacter->GetDefaultCenter();
		m_AniArrayFly.DrawAtScreen(0, pt, posCenter);

		ptMapPixelOffset.y += 65;
		CNpc::Draw(ptMapPixelOffset,dwColor);
	}
    else if (isset_state(this->GetState(), USTATE_USER_WABAO)) //soke 挖宝状态
	{
		this->SetInSpaState5(true);
		POINT pt = GetPos() - ptMapPixelOffset;
		POINT posCenter = m_pCharacter->GetDefaultCenter();

		if (this->IsMoving())
		{
			//soke 挖宝时就关闭自动打怪
		    GetGameApplication()->SetClientSet(enumAuto_Kill_Summon,false);
		    if ( GetScene()->GetMainCharacter() )
		    {
			    GetScene()->GetMainCharacter()->ClearPath();
			    GetScene()->GetMainCharacter()->ClearRunEvent();
		    }
		    //GetGameGuiManager()->AddClientSystemMessage("挖宝时不能自动打怪");
		    g_bAutoFight = false;

			
			if(isset_state(GetState(),USTATE_SUOGUDAN))
			{
				posCenter.x += 130;
				posCenter.y += 145;
				m_AniArraySwim5.DrawAtScreen2(0, pt, posCenter);
			}
			else{
				m_AniArraySwim5.DrawAtScreen(0, pt, posCenter);
			}
		} 
		else
		{
			if(isset_state(GetState(),USTATE_SUOGUDAN))
			{
				posCenter.x += 130;
				posCenter.y += 145;
				m_AniArraySwimStand5.DrawAtScreen2(0, pt, posCenter);
			}
			else{
				m_AniArraySwimStand5.DrawAtScreen(0, pt, posCenter);
			}
			
		}

		m_iDrawTop = GetImageHeight();

		this->DrawEffect(ptMapPixelOffset);
	}
	
	else if (isset_state(this->GetState(), USTATE_USER_DANCE)) //跳舞
	{
		this->SetInDanceState(true);
		this->Set2InDanceState(true); //发型
		this->Set3InDanceState(true); //服装填充
		POINT pt = GetPos() - ptMapPixelOffset;
		POINT posCenter = m_pCharacter->GetDefaultCenter();

		if(isset_state(GetState(),USTATE_SUOGUDAN))
		{
			posCenter.x += 130;
			posCenter.y += 145;
			m_AniArrayDance.DrawAtScreen2(0, pt, posCenter);
			m2_AniArrayDance.DrawAtScreen2(0, pt, posCenter);
			m3_AniArrayDance.DrawAtScreen2(0, pt, posCenter);
		}
		else{
			m_AniArrayDance.DrawAtScreen(0, pt, posCenter);
			m2_AniArrayDance.DrawAtScreen(0, pt, posCenter);
			m3_AniArrayDance.DrawAtScreen(0, pt, posCenter);
		}
		

		m_iDrawTop = GetImageHeight();
	}
	else if (isset_state(this->GetState(), USTATE_USER_BIKE)) //自行车
	{
		this->SetInBikeState(true);
		this->SetInBikeState2(true);
		POINT pt = GetPos() - ptMapPixelOffset;
		POINT posCenter = m_pCharacter->GetDefaultCenter();

		if (this->IsMoving())
		{
			if(isset_state(GetState(),USTATE_SUOGUDAN))
			{
				posCenter.x += 130;
				posCenter.y += 145;
				m_AniArrayBike.DrawAtScreen2(0, pt, posCenter);
				m_AniArrayBike2.DrawAtScreen2(0, pt, posCenter);
			}
			else{
				m_AniArrayBike.DrawAtScreen(0, pt, posCenter);
				m_AniArrayBike2.DrawAtScreen(0, pt, posCenter);
			}
			
		}
		else
		{
			if(isset_state(GetState(),USTATE_SUOGUDAN))
			{
				posCenter.x += 130;
				posCenter.y += 145;
				m_AniArrayBikeStand.DrawAtScreen2(0, pt, posCenter);
				m_AniArrayBikeStand2.DrawAtScreen2(0, pt, posCenter);
			}
			else{
				m_AniArrayBikeStand.DrawAtScreen(0, pt, posCenter);
				m_AniArrayBikeStand2.DrawAtScreen(0, pt, posCenter);
			}
			
		}

		m_iDrawTop = GetImageHeight();

		this->DrawEffect(ptMapPixelOffset);
	}
	else if (isset_state(this->GetState(), USTATE_USER_ZC)) //战车
	{
		this->SetInZCState(true);
		POINT pt = GetPos() - ptMapPixelOffset;
		POINT posCenter = m_pCharacter->GetDefaultCenter();
		if (this->IsMoving())
		{
			if(isset_state(GetState(),USTATE_SUOGUDAN))
			{
				posCenter.x += 130;
				posCenter.y += 145;
				m_AniArrayZC.DrawAtScreen2(0, pt, posCenter);
			}
			else{
				m_AniArrayZC.DrawAtScreen(0, pt, posCenter);
			}
		}
		else
		{
			if(isset_state(GetState(),USTATE_SUOGUDAN))
			{
				posCenter.x += 130;
				posCenter.y += 145;
				m_AniArrayZCStand.DrawAtScreen2(0, pt, posCenter);
			}
			else{
				m_AniArrayZCStand.DrawAtScreen(0, pt, posCenter);
			}
			
		}
		m_iDrawTop = GetImageHeight();

		this->DrawEffect(ptMapPixelOffset);
	}
	else if (m_pCharacter)
	{
		this->SetInSpaState(false);
		this->SetInSpaState5(false);
		this->SetInFlyState(false);

		this->SetInZCState(false);

		this->SetInDanceState(false);
		this->Set2InDanceState(false);
		this->Set3InDanceState(false);
		if(isset_state(GetState(),USTATE_SUOGUDAN))
		{
			CNpc::Draw2(ptMapPixelOffset,dwColor);
		}
		else{
			CNpc::Draw(ptMapPixelOffset,dwColor);
		}
		
		
	}

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	描述: 		  能否被主角攻击
*/
bool CCharacter::IsCanAttack()
{
	FUNCTION_BEGIN;
	
	if ( this == GetScene()->GetMainCharacter() )
		return false;

	//Decide if can be attack by Main Character according to cur Attack Mode
	switch( GetScene()->GetMainCharacter()->GetPKMode() )
	{
	case PKMODE_NORMAL:		//和平模式:此种模式下对所有目标人物进行攻击不会造成伤害
		return false;
		break;
	case PKMODE_ENTIRE:		//全体模式:此种模式下对所有目标人物进行攻击会造成伤害
		break;
	case PKMODE_TEAM:		//组队模式:此种模式下只对非队友目标人物攻击才能造成伤害
		{
			if ( GetGameGuiManager()->m_guiTeam )
			{
				if ( GetGameGuiManager()->m_guiTeam->FindMember( GetID() ) )		//Is team member	GetName()
					return false;
			}
		}
		break;
	case PKMODE_TONG:		//帮会模式:此种模式下只对非同一帮会目标人物攻击才能造成伤害
		{
			if ( GetGameGuiManager()->m_guiWorldConn )
			{
				if ( GetGameGuiManager()->m_guiWorldConn->FindTongMember( GetName() ) )		// Is tong member
					return false;
			}
		}
		break;
	case PKMODE_SEPT:		//家族模式:此模式下只对非同一家族目标人物攻击才能造成伤害
		{
			if ( GetGameGuiManager()->m_guiWorldConn )
			{
				if ( GetGameGuiManager()->m_guiWorldConn->FindSeptMember( GetName() ) )		//Is sept member
					return false;
			}
		}
		break;
	//case PKMODE_SCHOOL:		//师门模式:此模式下只对非同一师门目标人物攻击才能造成伤害
	//	{
	//		if ( GetGameGuiManager()->m_guiWorldConn )
	//		{
	//			if ( GetGameGuiManager()->m_guiWorldConn->FindTeacherMember( GetName() ) )		//Is teacher member
	//				return false;
	//		}
	//	}
	//	break;
	case PKMODE_COUNTRY:	//国家模式:此种模式下只对非同一国家目标人物攻击才能造成伤害
		{               //  而且国战期间对盟国人也不能攻击
			if ( GetCountry() == GetScene()->GetMainCharacter()->GetCountry() || CCountry::Instance()->IsAllyCountryAndInWar(GetCountry()) )
				return false;
		}
		break;
	//case PKMODE_CHALLENGE:	//挑战模式
	//	{
	//		//TBD
	//	}
	//	break;
	case PKMODE_GOODNESS:
		{
			DWORD dw=0x0000FFFF;
			short goodness=(short)(m_data.goodness & dw);
			switch(goodness) {
			case GOODNESS_2_1:
			case GOODNESS_2_2:
				break;
			default:
				break;
			}

			//TBD
			return true;
		}
		break;
	case PKMODE_ALLY:
		{
			if ( (GetCountry() == GetScene()->GetMainCharacter()->GetCountry())
				|| (CCountry::Instance()->IsAllyCountry(GetCountry())) )
			{
				return false;
			}
		}		
		break;
	}

	return true;

	FUNCTION_END;
}

bool CCharacter::IsMyFriend()
{
	FUNCTION_BEGIN;

	if ( this == GetScene()->GetMainCharacter() )
		return true;

	//Is team member
	if ( GetGameGuiManager()->m_guiTeam && GetGameGuiManager()->m_guiTeam->FindMember( GetID() ) )
		return true;

	//Decide if can be attack by Main Character according to cur Attack Mode
	switch( GetScene()->GetMainCharacter()->GetPKMode() )
	{
	case PKMODE_NORMAL:		//和平模式:此种模式下对所有目标人物进行攻击不会造成伤害
		break;
	case PKMODE_ENTIRE:		//全体模式:此种模式下对所有目标人物进行攻击会造成伤害
		break;
	case PKMODE_TEAM:		//组队模式:此种模式下只对非队友目标人物攻击才能造成伤害
		break;
	case PKMODE_TONG:		//帮会模式:此种模式下只对非同一帮会目标人物攻击才能造成伤害
		{
			if ( GetGameGuiManager()->m_guiWorldConn )
			{
				if ( GetGameGuiManager()->m_guiWorldConn->FindTongMember( GetName() ) )		// Is tong member
					return true;
			}
		}
		break;
	case PKMODE_SEPT:		//家族模式:此模式下只对非同一家族目标人物攻击才能造成伤害
		{
			if ( GetGameGuiManager()->m_guiWorldConn )
			{
				if ( GetGameGuiManager()->m_guiWorldConn->FindSeptMember( GetName() ) )		//Is sept member
					return true;
			}
		}
		break;
		//case PKMODE_SCHOOL:		//师门模式:此模式下只对非同一师门目标人物攻击才能造成伤害
		//	{
		//		if ( GetGameGuiManager()->m_guiWorldConn )
		//		{
		//			if ( GetGameGuiManager()->m_guiWorldConn->FindTeacherMember( GetName() ) )		//Is teacher member
		//				return false;
		//		}
		//	}
		//	break;
	case PKMODE_COUNTRY:	//国家模式:此种模式下只对非同一国家目标人物攻击才能造成伤害
		{               //  而且国战期间对盟国人也不能攻击
			if ( GetCountry() == GetScene()->GetMainCharacter()->GetCountry() || CCountry::Instance()->IsAllyCountryAndInWar(GetCountry()) )
				return true;
		}
		break;
		//case PKMODE_CHALLENGE:	//挑战模式
		//	{
		//		//TBD
		//	}
		//	break;
	case PKMODE_GOODNESS:
		break;
	case PKMODE_ALLY:
		{
			if ( (GetCountry() == GetScene()->GetMainCharacter()->GetCountry())
				|| (CCountry::Instance()->IsAllyCountry(GetCountry())) )
			{
				return true;
			}
		}		
		break;
	}

	return false;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	描述: 		  能否攻击别人
*/
bool CCharacter::CanAttack()
{
	FUNCTION_BEGIN;

	return true;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
const char* CCharacter::GetTypeName()
{
	FUNCTION_BEGIN;

	return GetCharTypeInfo(GetType()).name;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
DWORD CCharacter::GetBodySystemColor() 
{
	FUNCTION_BEGIN;

	ObjectBase_t* pBody = GetObjectBase(m_data.sculpt.dwBodyID);
	if(pBody) return pBody->dwColor;
	return -1;
	//return m_data.dwBodyColorSystem | 0xff000000;

	FUNCTION_END;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
DWORD CCharacter::GetGoodnessAfterAdjust()
{
	FUNCTION_BEGIN;

	if( (m_data.goodness & 0x0000FFFF) == GOODNESS_7 )
	{
		return GOODNESS_7;
	}

	DWORD good = 0x0000FFFF & m_data.goodness;
	if(good < -60)
	{
		good = GOODNESS_0;
	}
	else if(good < 0 && good >= -60) 
	{
		good = GOODNESS_1;
	}
	else if(good == 0)
	{
		good = GOODNESS_2_1;
	}
	else if(good > 0 && good <= 60)
	{
		good = GOODNESS_3;
	}
	else if(good > 60 && good <= 120) 
	{
		good = GOODNESS_4;
	}
	else if(good > 120 && good <= 180) 
	{
		good = GOODNESS_5;
	}
	else if(good > 180 && good < 10000)
	{
		good = GOODNESS_6;
	}

	return good;

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
DWORD CCharacter::GetNameColor()
{
	FUNCTION_BEGIN;

	DWORD dwColor = -1;

	if (m_data.goodness & GOODNESS_ATT)
	{
		dwColor=COLOR_ARGB(255,63,43,0);
	}
	else
	{
		DWORD dw=0x0000FFFF;
		short goodness=(short)(m_data.goodness & dw);
		switch(goodness) {
		case GOODNESS_2_1:
		case GOODNESS_2_2:
			dwColor=COLOR_ARGB(255,255,255,255);
			break;
		default:
			{
				if (goodness<(short)(GOODNESS_1 & 0x0000FFFF))
					dwColor=COLOR_ARGB(255,3,106,288);
				else if (goodness<(short)(GOODNESS_2_1 & dw))
					dwColor=COLOR_ARGB(255,36,203,173);
				else if (goodness<(short)(GOODNESS_3 & dw))
					dwColor=COLOR_ARGB(255,255,155,21);
				else if (goodness<(short)(GOODNESS_4 & dw))
					dwColor=COLOR_ARGB(255,225,97,21);
				else if (goodness<(short)(GOODNESS_5 & dw))
					dwColor=COLOR_ARGB(255,200,33,58);
				else 
					dwColor=COLOR_ARGB(255,225,18,24);
			}
			break;
		}
	}

	return dwColor;

	FUNCTION_END;
}
*/

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ptMapPixelOffset : 描述
 * \param dwColor : 描述
 * \return 返回值的描述
 */
bool CCharacter::IsUnKnowPerson()
{
	FUNCTION_BEGIN;
	#define  UNKNOWPERSON      "蒙面人"

	if (strcmp(GetName(),UNKNOWPERSON)==0)
		return true;

	return false;

	FUNCTION_END;

}

//CAnimation vipImage;
void CCharacter::DrawName(POINT ptMapPixelOffset,DWORD dwColor)
{
	FUNCTION_BEGIN;

	DWORD dwTitleColor=COLOR_ARGB(255,255,255,255);
	DWORD dwMasterColor=COLOR_ARGB(255,30,150,43);

	dwColor = GetNameColor(m_data.goodness);

    //soke PK善恶保护名字颜色改变
	if(isset_state(GetState(),USTATE_PK))
	{
		dwColor = COLOR_ARGB(255,0,255,137);
	}

	  //soke 战车名字改变
	if(isset_state(GetState(),USTATE_USER_ZC))
	{
		dwColor = COLOR_ARGB(255,59,149,253);
	}
	//soke VIP1玩家颜色改变
	//if(isset_state(GetState(),USTATE_VIP1) && isset_state(GetState(),USTATE_VIP2) && isset_state(GetState(),USTATE_VIP3) && isset_state(GetState(),USTATE_VIP4) && isset_state(GetState(),USTATE_VIP5))
	//{
	//	dwColor = COLOR_ARGB(255,184,0,237);
	//}
	//static CAnimation vipImage;

	int vipllist[] = {USTATE_VIP1,USTATE_VIP2,USTATE_VIP3,USTATE_VIP4,USTATE_VIP5,USTATE_VIP6,USTATE_VIP7,USTATE_VIP8,USTATE_VIP9,USTATE_VIP10,USTATE_VIP11,USTATE_VIP12,USTATE_VIP13,USTATE_VIP14,USTATE_VIP15,USTATE_VIP16,USTATE_VIP17,USTATE_VIP18,USTATE_VIP19,USTATE_VIP20};
	//Engine_WriteLogF("vipllist[1]:%d\n",vipllist[1]);
	IBitmaps* temppImages = vipImage.GetImages();
	int tempframe = 0;
	for (int i = sizeof(vipllist) / sizeof(int); i > 0; i--)
	{
		if(isset_state(GetState(),vipllist[i-1]))
		{
			if ( temppImages == NULL || temppImages->GetFrame() != i-1 )
			{
				tempframe = i;
			}
			break;
		}
	}
	
	if(tempframe>0)//soke VIP特效名字左边特效
	{
		stResourceLocation rl;
		rl.SetFileName( "data\\magic7.gl" );
		switch (tempframe)
		{
			case 1:
			{
				rl.group = 1603;						
				rl.frame = 21;
			}
			break;
			case 2:
			{
				rl.group = 1603;						
				rl.frame = 22;
			}
			break;
			case 3:
			{
				rl.group = 1603;						
				rl.frame = 23;
			}
			break;
			case 4:
			{
				rl.group = 1603;						
				rl.frame = 24;
			}
			break;
			case 5:
			{
				rl.group = 1603;						
				rl.frame = 25;
			}
			break;
			case 6:
			{
				rl.group = 803;						
				rl.frame = 1;
			}
			break;
			case 7:
			{
				rl.group = 803;						
				rl.frame = 2;
			}
			break;
			case 8:
			{
				rl.group = 803;						
				rl.frame = 3;
			}
			break;
			case 9:
			{
				rl.group = 803;						
				rl.frame = 4;
			}
			break;
			case 10:
			{
				rl.group = 803;						
				rl.frame = 5;
			}
			break;
			case 11:
			{
				rl.SetFileName( "data\\magic9.gl" );
				rl.group = 911;						
				rl.frame = 11;
			}
			break;
			case 12:
			{
				rl.SetFileName( "data\\magic9.gl" );
				rl.group = 911;						
				rl.frame = 12;
			}
			break;
			case 13:
			{
				rl.SetFileName( "data\\magic9.gl" );
				rl.group = 911;						
				rl.frame = 13;
			}
			break;
			case 14:
			{
				rl.SetFileName( "data\\magic9.gl" );
				rl.group = 911;						
				rl.frame = 14;
			}
			break;
		
			case 15:
			{
				rl.SetFileName( "data\\magic9.gl" );
				rl.group = 911;						
				rl.frame = 15;
			}
			break;
			case 16:
			{
				rl.SetFileName( "data\\magic9.gl" );
				rl.group = 911;						
				rl.frame = 16;
			}
			break;
			case 17:
			{
				rl.SetFileName( "data\\magic9.gl" );
				rl.group = 911;						
				rl.frame = 17;
			}
			break;
			case 18:
			{
				rl.SetFileName( "data\\magic9.gl" );
				rl.group = 911;						
				rl.frame = 18;
			}
			break;
			case 19:
			{
				rl.SetFileName( "data\\magic9.gl" );
				rl.group = 911;						
				rl.frame = 19;
			}
			break;
			case 20:
			{
				rl.SetFileName( "data\\magic9.gl" );
				rl.group = 911;						
				rl.frame = 20;
			}
			break;
			
		}
	
		
		vipImage.Create( &rl );
	}


	//段位经验图标 s
	int duanweilist[] = {USTATE_DUANWEI1,USTATE_DUANWEI2,USTATE_DUANWEI3,USTATE_DUANWEI4,USTATE_DUANWEI5,USTATE_DUANWEI6,USTATE_DUANWEI7,USTATE_DUANWEI8,USTATE_DUANWEI9,USTATE_DUANWEI10};
	//Engine_WriteLogF("vipllist[1]:%d\n",vipllist[1]);
	IBitmaps* duanwei_temppImage = duanwei.GetImages();
	int duanwei_tempframe = 0;
	for (int i = sizeof(duanweilist) / sizeof(int); i > 0; i--)
	{
		if(isset_state(GetState(),duanweilist[i-1]))
		{
			if ( duanwei_temppImage == NULL || duanwei_temppImage->GetFrame() != i-1 )
			{
				duanwei_tempframe = i;
			}
			break;
		}
	}
	
	if(duanwei_tempframe>0)//soke VIP特效名字左边特效 还没找到特效 这里 先用VIP代替 段位图标
	{
		stResourceLocation rl;
		rl.SetFileName( "data\\magic7.gl" );
		switch (duanwei_tempframe)
		{
			case 1:
			{
				rl.group = 261;						
				rl.frame = 210;
			}
			break;
			case 2:
			{
				rl.group = 261;						
				rl.frame = 211;
			}
			break;
			case 3:
			{
				rl.group = 261;						
				rl.frame = 212;
			}
			break;
			case 4:
			{
				rl.group = 261;						
				rl.frame = 213;
			}
			break;
			case 5:
			{
				rl.group = 261;						
				rl.frame = 214;
			}
			break;
			case 6:
			{
				rl.group = 261;						
				rl.frame = 215;
			}
			break;
			case 7:
			{
				rl.group = 261;						
				rl.frame = 216;
			}
			break;
			case 8:
			{
				rl.group = 261;						
				rl.frame = 217;
			}
			break;
			case 9:
			{
				rl.group = 261;						
				rl.frame = 218;
			}
			break;
			case 10:
			{
				rl.group = 261;						
				rl.frame = 219;
			}
			break;
		}
	
		
		duanwei.Create( &rl );
	}



	IBitmaps* temppImages2 = touxian.GetImages();
	int tempframe2 = 0;
	for (int i = 16; i > 0; i--)
	{
		if(GetProData()->touxianlevel == i)
		{
			if ( temppImages2 == NULL || temppImages2->GetFrame() != i+1 )
			{
				tempframe2 = i+1;
			}
			break;
		}
	}
	if(tempframe2>0)//soke 头衔特效
	{
		stResourceLocation rl;
		rl.SetFileName( "data\\interfaces2.gl" );
		rl.group = 65;						
		rl.frame = tempframe2;
		touxian.Create( &rl );
	}
	/* if(IsKing())
	{
		if (( this == GetScene()->GetMainCharacter()) )
			GetGameGuiManager()->m_guiMain->AddOneState( true, false, 1347, 10000, 0);
	}	 */

	if (GetScene()->GetMainCharacter()->GetProperty()->country!=GetProData()->country)
	{
        //soke 不是一个国家的颜色
		if( !CCountry::Instance()->IsAllyCountry(GetProData()->country))
		{
			dwColor = COLOR_ARGB(255,236,0,140);
		}
		else
		{ //盟国用户在小地图及平时人名显示采用粉色
			dwColor = COLOR_ARGB(255,255,128,192);
		}
	}
	
	
	if(isset_state(GetState(),USTATE_WAR))
	{
		dwColor = COLOR_ARGB(255,80,66,159);
	}

	if(GetName())
	{
		stPointI pt0 = GetPos() - ptMapPixelOffset;
		//int height = GetImageHeight();
		//缩骨
		if(isset_state(GetState(),USTATE_SUOGUDAN))
		{
			pt0.y +=50;	
			if(isset_state(GetState(),USTATE_SITDOWN))
			{
				pt0.y -=30;	
			}

		}
		
		int nLine = 0;

		int iFontHeight = GetDevice()->GetFontHeight();
		
		char szTmp[256]={0};
		char szShow[256]={0};

		stRectI rcText(pt0.x - 500,pt0.y - m_iDrawTop,pt0.x + 500,0xffff);
		
		stRectI rcSwitchText = rcText;

		

#if 0//_DEBUG

		if (IsUnKnowPerson() )
		{
			dwColor = COLOR_ARGB(255,255,255,255);
			rcText.top-=iFontHeight;
			GetDevice()->DrawString(GetName(),rcText,dwColor,DT_TOP | DT_CENTER,FontEffect_Border);
		}
		else if ( IsChangeFace() )
		{	
			NpcBase_t* pNpcBase = GetNpcBase( m_proChangingFace.m_dwChangingFaceID );
			if ( pNpcBase )
				sprintf( szShow, pNpcBase->strName );
			else
				sprintf( szShow, "" );
			
			rcText.top-=iFontHeight;
			GetDevice()->DrawString(szShow,rcText,dwColor,DT_TOP | DT_CENTER,FontEffect_Border);
		}
		else
		{
			int nStrLen = 0;
			//stRectI rectApp;

			if (GetScene()->m_pMouseOverObject == this)
			{

				if (GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_GRACENAME ))
				{
					nStrLen = strlen(GetGraceName());
					if (nStrLen>0)
					{
						/*rcText.top-=iFontHeight;
						rcText.left = pt0.x-(nStrLen*8+40)/2;
						rcText.right = pt0.x+(nStrLen*8+40)/2;
						rectApp=rcText;
						rectApp.left += 40;
						sprintf(szShow,"称号 ");
						GetDevice()->DrawString(szShow,rcText,dwTitleColor,DT_TOP | DT_LEFT,FontEffect_Border);*/
						rcText.top-=iFontHeight;

						sprintf(szShow,"%s",GetGraceName());
						GetDevice()->DrawString(szShow,rcText,dwTitleColor,DT_TOP | DT_CENTER,FontEffect_Border);
					}
				}
				
				if (GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_OFFICENAME ))
				{
					nStrLen = strlen(GetOfficeName());
					if (nStrLen>0)
					{
						/*rcText.top-=iFontHeight;
						rcText.left = pt0.x-(nStrLen*8+40)/2;
						rcText.right = pt0.x+(nStrLen*8+40)/2;
						rectApp=rcText;
						rectApp.left += 40;
						sprintf(szShow,"官职 ");
						GetDevice()->DrawString(szShow,rcText,dwTitleColor,DT_TOP | DT_LEFT,FontEffect_Border);*/
						rcText.top-=iFontHeight;

						sprintf(szShow,"%s",GetOfficeName());
						GetDevice()->DrawString(szShow,rcText,dwTitleColor,DT_TOP | DT_CENTER,FontEffect_Border);
					}
					
				}

				
				if (GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_SEPTNAME ))
				{
					nStrLen = strlen(GetSeptName());
					if (nStrLen>0)
					{
						rcText.top-=iFontHeight;

						sprintf(szShow,"%s 【家】",GetSeptName());
						GetDevice()->DrawString(szShow,rcText,dwTitleColor,DT_TOP | DT_CENTER,FontEffect_Border);
					}
				}


				if (GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_UNIONNAME ))
				{
					nStrLen = strlen(GetUnionName());
					if (nStrLen>0)
					{
						rcText.top-=iFontHeight;
						
						sprintf(szShow,"%s 【帮】",GetUnionName());
						GetDevice()->DrawString(szShow,rcText,dwTitleColor,DT_TOP | DT_CENTER,FontEffect_Border);
					}
				}
			
			}

			//if (GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_COUNTRYNAME ))
			//{
			//	rcText.top-=iFontHeight;
			//	rcText.left=pt0.x - 500;
			//	rcText.right =pt0.x + 500;
			//	sprintf(szShow,"【%s】",GetCountryName());
			//	GetDevice()->DrawString(szShow,rcText,dwColor,DT_TOP | DT_CENTER,FontEffect_Border);

			//}
			DWORD dwStarColor;
			if (ShowCountryStar(m_dwStar,szShow,dwStarColor))
			{
				rcText.top-=iFontHeight;
				GetDevice()->DrawString(szShow,rcText,dwStarColor,DT_TOP | DT_CENTER,FontEffect_Border);
			}
			
		
			rcText.top-=iFontHeight;
			sprintf(szShow,"%s 【%s】(%u)(%u,%u)",GetName(),GetCountryName(),GetID(),GetGridPos().x,GetGridPos().y);
			GetDevice()->DrawString(szShow,rcText,dwColor,DT_TOP | DT_CENTER,FontEffect_Border);
			
			nStrLen = strlen(GetProData()->caption);
			if (nStrLen>0)
			{
				rcText.top-=iFontHeight;

				sprintf(szShow,"%s",GetProData()->caption);
				GetDevice()->DrawString(szShow,rcText,dwMasterColor,DT_TOP | DT_CENTER,FontEffect_Border);

			}
		
		}
#else

		if ( IsChangeFace() )
		{
			NpcBase_t* pNpcBase = GetNpcBase( m_proChangingFace.m_dwChangingFaceID );
			if ( pNpcBase )
				sprintf( szShow, "%s(%u级)", pNpcBase->strName, m_proChangingFace.m_dwChangingFaceLevel );
			else
				sprintf( szShow, "" );

			rcText.top-=iFontHeight;
			m_iDrawTop += iFontHeight;
			GetDevice()->DrawString(szShow,rcText,-1,DT_TOP | DT_CENTER,FontEffect_Border);
		}
		else if (IsUnKnowPerson() )
		{
			if (m_data.goodness & GOODNESS_ATT)
				dwColor=COLOR_ARGB(255,63,43,0);
			else
				dwColor = COLOR_ARGB(255,255,255,255);

			rcText.top-=iFontHeight;
			m_iDrawTop += iFontHeight;
			GetDevice()->DrawString(GetName(),rcText,dwColor,DT_TOP | DT_CENTER,FontEffect_Border);
		}		
		else
		{
			int nStrLen = 0;
			//stRectI rectApp;

			DWORD dwStarColor;
			if (ShowCountryStar(m_dwStar,szShow,dwStarColor))//五星（国家星星）
			{
				bool isShow5 = true; 
				if (GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_PLAYERNAME ))
				{
					if (GetProData()->country == GetScene()->GetMainCharacter()->GetProperty()->country)
					{
						if (GetScene()->m_pMouseOverObject == this)
						{
							isShow5 = true;
						}
						else
						{
							isShow5 = false;
						}	
					}
					else
					{
						isShow5 = true;
					}
				}
				else
				{
					isShow5 = true;
				}
				//////////////////////////////////////////////////////////////////////////
				if (isShow5)
				{
					rcText.top-=iFontHeight;
					m_iDrawTop += iFontHeight;
					GetDevice()->DrawString(szShow,rcText,dwStarColor,DT_TOP | DT_CENTER,FontEffect_Border);
				}				
			}

			static CAnimation image;
			if ( NULL == image.GetImages() )
			{
				stResourceLocation rl;
				rl.SetFileName( "data\\interfaces.gl" );
				rl.group = 4;						
				rl.frame = 16;
				image.Create( &rl );
			}			

			rcText.top-=iFontHeight;

			m_iDrawTop += iFontHeight;
		#ifdef _DEBUG 
			//soke DUG模式下显示的名字
			if (GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_PLAYERNAME ))
			{
				if (GetProData()->country == GetScene()->GetMainCharacter()->GetProperty()->country)
				{
					if (GetScene()->m_pMouseOverObject == this)
					{
						sprintf( szShow,"%s 【%s】(%d)",GetName(),GetCountryName(),GetID() );
					}
					else
					{
						sprintf( szShow,"%s(%d)",GetName(),GetID() );
					}	
				}
				else
				{
					sprintf( szShow,"%s 【%s】(%d)",GetName(),GetCountryName(),GetID() );
				}
			}
			else
			{
				sprintf( szShow,"%s 【%s】(%d)",GetName(),GetCountryName(),GetID() );
			}						
		#else
			//soke 正常显示的玩家名称
			if (GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_PLAYERNAME ))
			{
				if (GetProData()->country == GetScene()->GetMainCharacter()->GetProperty()->country)
				{
					if(isset_state(GetState(),USTATE_USER_ZC))
					{
						sprintf( szShow,"家族战车 【%s】",GetCountryName());
					}
					else if (GetScene()->m_pMouseOverObject == this)
					{
						
						sprintf( szShow,"%s 【%s】",GetName(),GetCountryName());
					}
					else
					{
						sprintf( szShow,"%s",GetName());
					}	
				}
				else
				{
					sprintf( szShow,"%s 【%s】",GetName(),GetCountryName());
				}
			}
			else
			{
				sprintf( szShow,"%s 【%s】",GetName(),GetCountryName());
			}		
		#endif
			int templen = 0;
			if ( CCountry::Instance()->IsEmperorAt()
				&& CCountry::Instance()->CheckCountry(GetCountry(), GetScene()->GetMapCountryName()) )
			{
				stPointI pt;
				pt.x = rcText.left + (rcText.Width() - strlen(szShow)*GetDevice()->GetFontWidth()) / 2 - image.GetWidth() - 2;
				pt.y = rcText.top;
				if(isset_state(GetState(),USTATE_SUOGUDAN))
				{
					pt.y+=50;
				}
				image.Render( pt.x, pt.y );
				templen = image.GetWidth();
			}
			GetDevice()->DrawString(szShow,rcText,dwColor,DT_TOP | DT_CENTER,FontEffect_Border);

			if (touxian.GetImages() != NULL)
			{
				stPointI pt;
				pt.x = rcText.left + (rcText.Width() - strlen(szShow)*GetDevice()->GetFontWidth()) / 2 - touxian.GetWidth()  - templen;
				pt.y = rcText.top;
				touxian.Render( pt.x-2, pt.y-2 );
			} 

			if (duanwei.GetImages() != NULL)
			{
				stPointI pt;
				pt.x = rcText.left +(rcText.Width() + strlen(szShow)*GetDevice()->GetFontWidth()) / 2 ;
				pt.y = rcText.top;
				duanwei.Render( pt.x-(-60), pt.y-10 );
			} 

			if (vipImage.GetImages() != NULL)
			{
				IBitmaps* temppImages = vipImage.GetImages();
				tempframe = temppImages->GetFrame();
				if(tempframe == 11)
				{
					stPointI pt;
					pt.x = rcText.left +(rcText.Width() + strlen(szShow)*GetDevice()->GetFontWidth()) / 2 ;
					pt.y = rcText.top;
					vipImage.Render( pt.x-5,  pt.y-10 );
				}
				else if(tempframe == 12)
				{
					stPointI pt;
					pt.x = rcText.left +(rcText.Width() + strlen(szShow)*GetDevice()->GetFontWidth()) / 2 ;
					pt.y = rcText.top;
					vipImage.Render( pt.x-5,  pt.y-10 );
				}
				else if(tempframe == 13)
				{
					stPointI pt;
					pt.x = rcText.left +(rcText.Width() + strlen(szShow)*GetDevice()->GetFontWidth()) / 2 ;
					pt.y = rcText.top;
					vipImage.Render( pt.x-5,  pt.y-10 );
				}
				else if(tempframe == 14)
				{
					stPointI pt;
					pt.x = rcText.left +(rcText.Width() + strlen(szShow)*GetDevice()->GetFontWidth()) / 2 ;
					pt.y = rcText.top;
					vipImage.Render( pt.x-5,  pt.y-10 );
				}
				else if(tempframe == 15)
				{
					stPointI pt;
					pt.x = rcText.left +(rcText.Width() + strlen(szShow)*GetDevice()->GetFontWidth()) / 2 ;
					pt.y = rcText.top;
					vipImage.Render( pt.x-5,  pt.y-10 );
				}
				else if(tempframe == 16)
				{
					stPointI pt;
					pt.x = rcText.left +(rcText.Width() + strlen(szShow)*GetDevice()->GetFontWidth()) / 2 ;
					pt.y = rcText.top;
					vipImage.Render( pt.x-5,  pt.y-10 );
				}
				else if(tempframe == 17)
				{
					stPointI pt;
					pt.x = rcText.left +(rcText.Width() + strlen(szShow)*GetDevice()->GetFontWidth()) / 2 ;
					pt.y = rcText.top;
					vipImage.Render( pt.x-5,  pt.y-10 );
				}
				else if(tempframe == 18)
				{
					stPointI pt;
					pt.x = rcText.left +(rcText.Width() + strlen(szShow)*GetDevice()->GetFontWidth()) / 2 ;
					pt.y = rcText.top;
					vipImage.Render( pt.x-5,  pt.y-10 );
				}
				else if(tempframe == 19)
				{
					stPointI pt;
					pt.x = rcText.left +(rcText.Width() + strlen(szShow)*GetDevice()->GetFontWidth()) / 2 ;
					pt.y = rcText.top;
					vipImage.Render( pt.x-5,  pt.y-10 );
				}
				else if(tempframe == 20)
				{
					stPointI pt;
					pt.x = rcText.left +(rcText.Width() + strlen(szShow)*GetDevice()->GetFontWidth()) / 2 ;
					pt.y = rcText.top;
					vipImage.Render( pt.x-5,  pt.y-10 );
				}
				else{
					stPointI pt;
					pt.x = rcText.left +(rcText.Width() + strlen(szShow)*GetDevice()->GetFontWidth()) / 2 ;
					pt.y = rcText.top;
					vipImage.Render( pt.x-170, pt.y-110 );
				}
			
				
			} 

			nStrLen = strlen(GetProData()->caption);
			if (nStrLen>0)
			{
				rcText.top-=iFontHeight;
				m_iDrawTop += iFontHeight;
				sprintf(szShow,"%s",GetProData()->caption);
				GetDevice()->DrawString(szShow,rcText,dwMasterColor,DT_TOP | DT_CENTER,FontEffect_Border);

			}
			else
			{
				 //1为队长，2为将军
				if( GetProData()->dwArmyState == 1) // 为队长
				{
					rcText.top-=iFontHeight;
					m_iDrawTop += iFontHeight;
					sprintf(szShow,"%s","队长");
					GetDevice()->DrawString(szShow,rcText,dwMasterColor,DT_TOP | DT_CENTER,FontEffect_Border);
    
				}
				else if(GetProData()->dwArmyState == 2) // 为将军
				{
					rcText.top-=iFontHeight;
					m_iDrawTop += iFontHeight;
					sprintf(szShow,"%s","将军");
					GetDevice()->DrawString(szShow,rcText,dwMasterColor,DT_TOP | DT_CENTER,FontEffect_Border);

				}
			}

			
			
			if (GetScene()->m_pMouseOverObject == this)
			{
				rcSwitchText.top-=iFontHeight;

				if (GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_UNIONNAME ))
				{
					nStrLen = strlen(GetUnionName());
					if (nStrLen>0)
					{
						rcSwitchText.top+=iFontHeight;

						sprintf(szShow,"%s 【帮】",GetUnionName());
						GetDevice()->DrawString(szShow,rcSwitchText,dwTitleColor,DT_TOP | DT_CENTER,FontEffect_Border);
					}
				}

				if (GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_SEPTNAME ))
				{
					nStrLen = strlen(GetSeptName());
					if (nStrLen>0)
					{
						rcSwitchText.top+=iFontHeight;

						sprintf(szShow,"%s 【家】",GetSeptName());
						GetDevice()->DrawString(szShow,rcSwitchText,dwTitleColor,DT_TOP | DT_CENTER,FontEffect_Border);
					}
				}


				if (GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_OFFICENAME ))
				{
					nStrLen = strlen(GetOfficeName());
					if (nStrLen>0)
					{
						/*rcText.top-=iFontHeight;
						rcText.left = pt0.x-(nStrLen*8+40)/2;
						rcText.right = pt0.x+(nStrLen*8+40)/2;
						rectApp=rcText;
						rectApp.left += 40;
						sprintf(szShow,"官职 ");
						GetDevice()->DrawString(szShow,rcText,dwTitleColor,DT_TOP | DT_LEFT,FontEffect_Border);*/
						rcSwitchText.top+=iFontHeight;

						sprintf(szShow,"%s",GetOfficeName());
						GetDevice()->DrawString(szShow,rcSwitchText,dwTitleColor,DT_TOP | DT_CENTER,FontEffect_Border);
					}

				}


				if (GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_GRACENAME ))
				{
					nStrLen = strlen(GetGraceName());
					if (nStrLen>0)
					{
						/*rcText.top-=iFontHeight;
						rcText.left = pt0.x-(nStrLen*8+40)/2;
						rcText.right = pt0.x+(nStrLen*8+40)/2;
						rectApp=rcText;
						rectApp.left += 40;
						sprintf(szShow,"称号 ");
						GetDevice()->DrawString(szShow,rcText,dwTitleColor,DT_TOP | DT_LEFT,FontEffect_Border);*/
						rcSwitchText.top+=iFontHeight;

						sprintf(szShow,"%s",GetGraceName());
						GetDevice()->DrawString(szShow,rcSwitchText,dwTitleColor,DT_TOP | DT_CENTER,FontEffect_Border);
					}
				}

			}


		}		
#endif
	}

	FUNCTION_END;
}



void CCharacter::DrawCountryWarFlag(stPointI ptMapPixelOffset)
{
	FUNCTION_BEGIN;

        if( m_bShowWarFlag )
	{
		stPointI ptChar = GetPos();
		ptChar -= ptMapPixelOffset;
		ptChar.y -= m_iDrawTop;
		
		int iFontHeight = GetDevice()->GetFontHeight();

		DWORD dwColor = GetNameColor(m_data.goodness);

		if (GetScene()->GetMainCharacter()->GetProperty()->country!=GetProData()->country)
		{
			if( !CCountry::Instance()->IsAllyCountry(GetProData()->country))
			{
				if( CCountry::Instance()->IsDareCountryInWar(GetProData()->country))
				{
					dwColor = COLOR_ARGB(255,80,66,159);
				}	
				else 
				{
					dwColor = COLOR_ARGB(255,236,0,140);
				}
			}
		}

		if(isset_state(GetState(),USTATE_SUOGUDAN))
		{
			ptChar.y +=50;	
			if(isset_state(GetState(),USTATE_SITDOWN))
			{
				ptChar.y -=30;	
			}

		}
		stRectI rcText(ptChar.x - 20,ptChar.y - iFontHeight,ptChar.x + 20,ptChar.y);
		GetDevice()->DrawString("国战",rcText,dwColor,DT_TOP | DT_CENTER,FontEffect_Border);
	}

	//Draw emperor dare flag
	if ( GetScene()->GetMainCharacter() && GetScene()->GetMainCharacter()->GetEmperorDareZoneInfo().bNeedDraw )
	{
		stPointI ptChar = GetPos();
		ptChar -= ptMapPixelOffset;
		ptChar.y -= m_iDrawTop;

		int iFontHeight = GetDevice()->GetFontHeight();

		DWORD dwColor = GetNameColor(m_data.goodness);

		//Adjust color according to main character's dare type
		CMainCharacter::stEmperorDareZone& info = GetScene()->GetMainCharacter()->GetEmperorDareZoneInfo();
		if ( info.dwDefID != GetCountry() )
			dwColor = COLOR_ARGB(255,236,0,140);
		
		if(isset_state(GetState(),USTATE_SUOGUDAN))
		{
			ptChar.y +=50;	
			if(isset_state(GetState(),USTATE_SITDOWN))
			{
				ptChar.y -=30;	
			}

		}
		stRectI rcText(ptChar.x - 20,ptChar.y - iFontHeight,ptChar.x + 20,ptChar.y);
		GetDevice()->DrawString("皇城战",rcText,dwColor,DT_TOP | DT_CENTER,FontEffect_Border);
	}
	//Draw emperor dare flag
	if ( GetScene()->GetMainCharacter() && isset_state(GetState(),USTATE_USER_ZC))
	{
		stPointI ptChar = GetPos();
		ptChar -= ptMapPixelOffset;
		ptChar.y -= m_iDrawTop;

		int iFontHeight = GetDevice()->GetFontHeight();

		DWORD dwColor = GetNameColor(m_data.goodness);

		//Adjust color according to main character's dare type
		CMainCharacter::stEmperorDareZone& info = GetScene()->GetMainCharacter()->GetEmperorDareZoneInfo();
		if ( info.dwDefID != GetCountry() )
			dwColor = COLOR_ARGB(255,0,255,0);

		if(isset_state(GetState(),USTATE_SUOGUDAN))
		{
			ptChar.y +=50;	
			if(isset_state(GetState(),USTATE_SITDOWN))
			{
				ptChar.y -=30;	
			}

		}	
		stRectI rcText(ptChar.x - 20,ptChar.y - iFontHeight,ptChar.x + 20,ptChar.y);
		GetDevice()->DrawString(" ★★★ ",rcText,dwColor,DT_TOP | DT_CENTER,FontEffect_Border);
	}

	FUNCTION_END;
}

void CCharacter::DrawEmotionIcon(stPointI ptMapPixelOffset)
{
        FUNCTION_BEGIN;

	if(m_BitmapShow.IsNeedShow())
	{
		stPointI ptChar = GetPos();
		ptChar -= ptMapPixelOffset;
		ptChar.y -= m_iDrawTop;
		m_BitmapShow.Render(ptChar);
		m_iDrawTop -= (m_BitmapShow.rcClip.top+m_BitmapShow.rcClip.Height());
	}		

	FUNCTION_END;
}
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pSkill : 描述
 * \return 返回值的描述
 */
int CCharacter::ServerSkillToClientSkill(SkillBase_t* pSkill)
{
	FUNCTION_BEGIN;

	if( pSkill )
	{
		switch(pSkill->dwType)
		{
		case SkillType_Attack:
		case SkillType_StateAttack:
		case SkillType_Active:
			return pSkill->dwPic;
			break;
		}
	}
	return -1;

	FUNCTION_END;
}


/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  获得击中（近身攻击）的声音
*/
int CCharacter::GetHitSound()
{
	FUNCTION_BEGIN;

	int sound = HAND_HIT_SOUND;

	ObjectBase_t* pLObject = GetObjectBase(GetLeftWeaponID());
	ObjectBase_t* pRObject = GetObjectBase(GetRightWeaponID());

	if(pLObject && pLObject->dwType == ItemType_Bow)
	{
		if(pRObject)
			return pLObject->hitSound;
		else
			return BOW_EMPTY_HIT_SOUND;
	}

	if(pRObject)
	{
		sound = pRObject->hitSound;
	}
	return sound;

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  获得挥动武器的声音
*/
int CCharacter::GetAttackSound()
{
	FUNCTION_BEGIN;

	int sound = HAND_ATTACK_SOUND;
	ObjectBase_t* pLObject = GetObjectBase(GetLeftWeaponID());
	ObjectBase_t* pRObject = GetObjectBase(GetRightWeaponID());
	if( pLObject && pLObject->dwType == ItemType_Bow )
	{
		if(pRObject)
			return pLObject->attackSound;
		else 
			return BOW_EMPTY_ATTACK_SOUND;
	}

	if(pRObject)
	{
		sound = pRObject->attackSound;
	}

	return sound;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CCharacter::OnEquipChanged()
{
	FUNCTION_BEGIN;

	int sound = GetAttackSound();
	if(sound)
	{
		char szSound[MAX_PATH];
		sprintf(szSound,"%d",sound);
		m_pCharacter->SetSound(Ani_Attack,szSound);
		m_pCharacter->SetSound(Ani_Attack2,szSound);
		m_pCharacter->SetSound(Ani_Attack3,szSound);
		m_pCharacter->SetSound(Ani_Ride_Attack,szSound);
		m_pCharacter->SetSound(Ani_Ride_Bow,szSound);
		m_pCharacter->SetSound(Ani_Bow,szSound);
	}
	else
	{		
		m_pCharacter->ResetSound(Ani_Attack);
		m_pCharacter->ResetSound(Ani_Attack2);
		m_pCharacter->ResetSound(Ani_Attack3);
		m_pCharacter->ResetSound(Ani_Ride_Attack);

		m_pCharacter->ResetSound(Ani_Bow);
		m_pCharacter->ResetSound(Ani_Ride_Bow);
	}
	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pCmd : 描述
 * \return 返回值的描述
 */
bool CCharacter::HandleCommand(stNullUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	switch(pCmd->byCmdType)
	{
	case MAPSCREEN_USERCMD:
		switch(pCmd->byParam)
		{
		case REFRESHSTATE_MAPSCREEN_USERCMD_PARA:			// 刷新角色状态
			{
				stRefreshStateMapScreenUserCmd* p = (stRefreshStateMapScreenUserCmd*)pCmd;
				memcpy(m_data.state,p->state,sizeof(m_data.state));
				AfterSetAttrib(false,true);
				return true;
			}
			break;
		}
		break;
	}
	
	return CNpc::HandleCommand(pCmd);

	FUNCTION_END;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ptMapOffset : 描述
 * \return 返回值的描述
 */
void CCharacter::AddLight(POINT ptMapOffset)
{
	FUNCTION_BEGIN;

	
	m_pCharacter->AddLight(ptMapOffset);
	/*if(GetGameGuiManager()->m_guiTeam->FindMember(GetName()))
	{
	}*/
	if( GetMaxHP() && GetHP() )
	{
		// 是队友
		GetLightMap()->AddLight(GetPos()-ptMapOffset,GetScene()->GetLightRadius()/2,-1);
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
bool CCharacter::OnSetCursor()
{
	FUNCTION_BEGIN;

	if ( (GetGameCursor()->GetCursorImageType() == eCursorUseAccess)
		||( GetGameCursor()->GetCursorImageType() == eCursorShowUser )
		||( GetGameCursor()->GetCursorImageType() == eCursorFollowUser )
		||( GetGameCursor()->GetCursorImageType() == eCursorTrade )
		||( GetGameCursor()->GetCursorImageType() == eCursorPetAttack)
		||( GetGameCursor()->GetCursorTypeEx() == eCursorGetMailItem) )
		return false;

	if(IsCanAttack())
	{
		GetGameCursor()->SetCursor(CCursor::CursorType_Normal,eCursorAttack);
		return true;
	}
	return false;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
enumItemType CCharacter::GetRightWeaponType()
{
	FUNCTION_BEGIN;

	ObjectBase_t* pObject = GetObjectBase( GetRightWeaponID() );
	if(!pObject) return ItemType_None;
	return (enumItemType)pObject->dwType;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
enumItemType CCharacter::GetLeftWeaponType()
{
	FUNCTION_BEGIN;

	ObjectBase_t* pObject = GetObjectBase( GetLeftWeaponID() );
	if(!pObject) return ItemType_None;
	return (enumItemType)pObject->dwType;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param prevGrid : 描述
 * \param ptCurGrid : 描述
 * \return 返回值的描述
 */
void CCharacter::OnGridPosChange(POINT prevGrid,POINT ptCurGrid)
{
	FUNCTION_BEGIN;

	stGroundMusic* music = NULL;
	int curTileType = GetScene()->GetMapTileType(ptCurGrid);
	if( curTileType != m_tiletype )
	{
		m_tiletype = curTileType;
		music = GetScene()->GetMapTileMusic(curTileType);
		if( music )
		{
			//Engine_WriteLogF("进入地表类型 %d \n",music->)
			//得到地表的属性，设置声音。

			stSoundFx fx;
			if( music->walk.size() > 1 )
			{
				fx.nNum = music->walk[1];
				fx.nStartID = music->walk[0];
				m_pCharacter->SetSoundFx(Ani_Walk,fx);
			}

			if( music->run.size() > 1 )
			{
				fx.nNum = music->run[1];
				fx.nStartID = music->run[0];
				m_pCharacter->SetSoundFx(Ani_Run,fx);
			}
		}
	}

	FUNCTION_END;
}

const char* CCharacter::GetGraceName()
{
	return GetCivilianName(m_data.grace);
}
const char* CCharacter::GetOfficeName()
{
	return GetKnightName(m_data.exploit);
}

bool CCharacter::bIsShopStart()
{
	return isset_state(GetState(),USTATE_PRIVATE_STORE);
	return CNpc::bIsShopStart();
}

bool CCharacter::IsSeptLeader()
{
	FUNCTION_BEGIN;
	
	if ( NULL == GetGameGuiManager()->m_guiWorldConn )
		return false;

	CGuiWorldConn* pDlg = GetGameGuiManager()->m_guiWorldConn;
	if ( strlen( pDlg->m_SeptInfo.master ) == 0 )
		return false;

	if ( strcmp( pDlg->m_SeptInfo.master, GetName() ) != 0 )
		return false;

	return true;

	FUNCTION_END;
}

bool CCharacter::IsUnionLeader()
{
	FUNCTION_BEGIN;

	if ( NULL == GetGameGuiManager()->m_guiWorldConn )
		return false;

	CGuiWorldConn* pDlg = GetGameGuiManager()->m_guiWorldConn;
	if ( strlen( pDlg->m_TongInfo.master ) == 0 )
		return false;

	if ( strcmp( pDlg->m_TongInfo.master, GetName() ) != 0 )
		return false;

	return true;

	FUNCTION_END;
}

//soke 国王/女王领取薪资标识
bool CCharacter::IsKing()
{
	FUNCTION_BEGIN;

	if ( IsEmperor() )
		return true;

	char caption [MAX_NAMESIZE] = {0};
	strcpy(caption,CCharacter::m_data.caption);
	str_trim_right(caption);
	str_trim_left(caption);

	size_t len = strlen(caption);
	if( len < 4)  return false;

	char * pLast4 = (char * )(caption + (len - 4) * sizeof(char));
	if( (0 == strcmp(pLast4,"国王"))
		|| (0 == strcmp(pLast4,"女王")))
		return true;
	return false;

	FUNCTION_END;
}

//soke 皇帝/女皇领取薪资标识
bool CCharacter::IsEmperor()
{
	FUNCTION_BEGIN;

	char caption [MAX_NAMESIZE] = {0};
	strcpy(caption,CCharacter::m_data.caption);
	str_trim_right(caption);
	str_trim_left(caption);

	size_t len = strlen(caption);
	if( len < 4)  return false;

	char * pLast4 = (char * )(caption + (len - 4) * sizeof(char));
	if( ( (0 == strcmp(pLast4,"皇帝")) && (GetType() & 1) )
		|| ( (0 == strcmp(pLast4,"女皇")) && (GetType() & 2) ) )
		return true;

	return false;

	FUNCTION_END;
}

//soke 元帅领取薪资标识
bool CCharacter::IsYuanshuai()
{
	FUNCTION_BEGIN;

	char caption [MAX_NAMESIZE] = {0};
	strcpy(caption,CCharacter::m_data.caption);
	str_trim_right(caption);
	str_trim_left(caption);

	size_t len = strlen(caption);
	if( len < 4)  return false;

	char * pLast4 = (char * )(caption + (len - 4) * sizeof(char));
	if( 0 == strcmp(pLast4,"元帅"))
		return true;
	return false;

	FUNCTION_END;
}

//soke 宰相领取薪资标识
bool CCharacter::IsZaixiang()
{
	FUNCTION_BEGIN;

	char caption [MAX_NAMESIZE] = {0};
	strcpy(caption,CCharacter::m_data.caption);
	str_trim_right(caption);
	str_trim_left(caption);

	size_t len = strlen(caption);
	if( len < 4)  return false;

	char * pLast4 = (char * )(caption + (len - 4) * sizeof(char));
	if( 0 == strcmp(pLast4,"宰相"))
		return true;
	return false;

	FUNCTION_END;
}

//soke 捕头（左）领取薪资标识
bool CCharacter::IsCatcher()
{
	FUNCTION_BEGIN;

	char caption [MAX_NAMESIZE] = {0};
	strcpy(caption,CCharacter::m_data.caption);
	str_trim_right(caption);
	str_trim_left(caption);

	size_t len = strlen(caption);
	if( len < 4)  return false;

	char * pLast4 = (char * )(caption + (len - 4) * sizeof(char));
	if( 0 == strcmp(pLast4,"捕头"))
		return true;
	return false;

	FUNCTION_END;
}

//soke 御史大夫（左）领取薪资标识
bool CCharacter::IsDiplomat()
{
	FUNCTION_BEGIN;

	char caption [MAX_NAMESIZE] = {0};
	strcpy(caption,CCharacter::m_data.caption);
	str_trim_right(caption);
	str_trim_left(caption);

	//soke len 字节长度
	size_t len = strlen(caption);
	if( len < 8)  return false;

	char * pLast8 = (char * )(caption + (len - 8) * sizeof(char));
	if( 0 == strcmp(pLast8,"御史大夫"))
		return true;
	return false;

	FUNCTION_END;
}

//soke 捕头（右）领取薪资标识
bool CCharacter::IsCatchex()
{
	FUNCTION_BEGIN;

	char caption [MAX_NAMESIZE] = {0};
	strcpy(caption,CCharacter::m_data.caption);
	str_trim_right(caption);
	str_trim_left(caption);

	size_t len = strlen(caption);
	if( len < 4)  return false;

	char * pLast4 = (char * )(caption + (len - 4) * sizeof(char));
	if( 0 == strcmp(pLast4,"捕头"))
		return true;
	return false;

	FUNCTION_END;
}

//soke 御史大夫（右）领取薪资标识
bool CCharacter::IsDiplomax()
{
	FUNCTION_BEGIN;

	char caption [MAX_NAMESIZE] = {0};
	strcpy(caption,CCharacter::m_data.caption);
	str_trim_right(caption);
	str_trim_left(caption);

	size_t len = strlen(caption);
	if( len < 8)  return false;

	char * pLast8 = (char * )(caption + (len - 8) * sizeof(char));
	if( 0 == strcmp(pLast8,"御史大夫"))
		return true;
	return false;

	FUNCTION_END;
}

//soke 城主领取薪资标识
bool CCharacter::IsCityLeader()
{
	FUNCTION_BEGIN;

	char caption [MAX_NAMESIZE] = {0};
	strcpy(caption,CCharacter::m_data.caption);
	str_trim_right(caption);
	str_trim_left(caption);

	size_t len = strlen(caption);
	if( len < 4)  return false;

	char * pLast4 = (char * )(caption + (len - 4) * sizeof(char));
	if( 0 == strcmp(pLast4,"城主"))
		return true;
	return false;

	FUNCTION_END;
}

bool CCharacter::IsGen()
{
	FUNCTION_BEGIN;

	 return  m_data.dwArmyState == 2;
	
	 FUNCTION_END;
}
bool CCharacter::IsArmyer()
{
	return m_data.dwArmyState != 0;
}

size_t CCharacter::GetLevel()
{
	return m_data.level;
}

const char* CCharacter::GetCountryName()
{	
	return m_countryName;
}

DWORD CCharacter::GetTeamState()
{
	return m_data.dwTeamState;
}

bool CCharacter::IsEquipedPickResourceTool()
{
	if ( !IsMainRole() )
		return false;
//取消采集手套 注释掉 
//soke 不取消不能自动打怪
	//if (GetRightWeaponID() != 876)
	//	return false;

	//// Is dur ok
	//CRoleItem* pItem = GetScene()->GetMainCharacter()->FindItemByLocationConstID( 876, OBJECTCELLTYPE_EQUIP );
	//if ( NULL == pItem )
	//	return false;

	//if ( pItem->GetObject()->dur <= 0 )
	//	return false;
//取消采集手套 注释掉完
	return true;
}

DWORD CCharacter::GetLeftWeaponColor() 
{
	//左手武器
	ObjectBase_t* pWeaponL = GetObjectBase(m_data.sculpt.dwLeftHandID);	
	if(pWeaponL)
		return pWeaponL->dwColor;
	return 0;
}

DWORD CCharacter::GetRightWeaponColor()
{
	//左手武器
	ObjectBase_t* pWeaponR = GetObjectBase(m_data.sculpt.dwRightHandID);
	if(pWeaponR)
		return pWeaponR->dwColor;
	return 0;
}

bool CCharacter::IsDragonTigerState()
{
	BYTE* byState = GetState();
	if ( isset_state(byState, USTATE_TOGETHER_WITH_DRAGON)
		|| isset_state(byState, USTATE_TOGETHER_WITH_TIGER) )
	{
		return true;
	}

	return false;
}

int CCharacter::GetRideID()
{
	return m_data.sculpt.dwHorseID > 0 ? m_data.sculpt.dwHorseID : CNpc::GetRideID(); 
}

void CCharacter::SetGoodness(DWORD dwGoodness)
{
	m_data.goodness = dwGoodness;
}

void CCharacter::SetInSpaState(bool bSwim)
{
	int nAniSwim = m_AniArraySwim.GetAnimationCount();
	int nAniSwimStand = m_AniArraySwimStand.GetAnimationCount();
	if (bSwim && (nAniSwim == 0) && (nAniSwimStand == 0))
	{
		iAnimation* pAniSwim = Magic_CreateAnimation();
		iAnimation* pAniSwimStand = Magic_CreateAnimation();

		if (pAniSwim && pAniSwimStand)
		{
			stAnimationStruct aniSwim;
			stAnimationStruct aniSwimStand;
			//控制图片ID
			aniSwimStand.nGroup = 0;
			aniSwim.nGroup = 1;
			aniSwimStand.nGroupCount = aniSwim.nGroupCount = 1;
			aniSwimStand.nAnispeed = 260;
			aniSwim.nAnispeed = 260;
			aniSwimStand.nDir = aniSwim.nDir = 8;
			strcpy(aniSwim.strAnimation, "swim");
			strcpy(aniSwimStand.strAnimation, "swimstand");

            //soke 人物温泉ID
			size_t nPack = 0;
			nPack = (eSex_Male == this->GetSex()) ? 43: 44;
			pAniSwim->CreateAnimation("Interfaces1", nPack, 0, aniSwim, Usage_Character);
			pAniSwimStand->CreateAnimation("Interfaces1", nPack, 0, aniSwimStand, Usage_Character);

			m_AniArraySwim.Add(pAniSwim);
			m_AniArraySwimStand.Add(pAniSwimStand);
		}
	}
	else if(m_AniArraySwim.GetAnimationCount() > 0 || m_AniArraySwimStand.GetAnimationCount() > 0)
	{
		m_AniArraySwim.FinalRelease();
		m_AniArraySwimStand.FinalRelease();
	}
}

//人物飞行控制
//soke 挖宝
void CCharacter::SetInSpaState5(bool bSwim)
{
	int nAniSwim = m_AniArraySwim5.GetAnimationCount();
	int nAniSwimStand = m_AniArraySwimStand5.GetAnimationCount();
	if (bSwim && (nAniSwim == 0) && (nAniSwimStand == 0))
	{
		iAnimation* pAniSwim = Magic_CreateAnimation();
		iAnimation* pAniSwimStand = Magic_CreateAnimation();

		if (pAniSwim && pAniSwimStand)
		{
			stAnimationStruct aniSwim;
			stAnimationStruct aniSwimStand;
			//soke 控制图片ID
			aniSwimStand.nGroup = 0; //soke 挖宝站立
			aniSwim.nGroup = 1;      //soke 挖宝游泳
			aniSwimStand.nGroupCount = aniSwim.nGroupCount = 1;
			aniSwimStand.nAnispeed = 190; //soke 站立速度
			aniSwim.nAnispeed = 60;       //soke 游泳速度
			aniSwimStand.nDir = aniSwim.nDir = 8; //soke 方向
			strcpy(aniSwim.strAnimation, "swim");
			strcpy(aniSwimStand.strAnimation, "swimstand");

            //soke 角色外形grop位置 1785:1786
			size_t nPack = 0;
			nPack = (eSex_Male == this->GetSex()) ? 45: 46; //soke 男:女
			pAniSwim->CreateAnimation("Interfaces1", nPack, 0, aniSwim, Usage_Character);
			pAniSwimStand->CreateAnimation("Interfaces1", nPack, 0, aniSwimStand, Usage_Character);

			m_AniArraySwim5.Add(pAniSwim);
			m_AniArraySwimStand5.Add(pAniSwimStand);
		}
	}
	else if(m_AniArraySwim5.GetAnimationCount() > 0 || m_AniArraySwimStand5.GetAnimationCount() > 0)
	{
		m_AniArraySwim5.FinalRelease();
		m_AniArraySwimStand5.FinalRelease();
	}
}

//soke 飞行配置
void CCharacter::SetInFlyState(bool bFly)
{
	int nAniFly = m_AniArrayFly.GetAnimationCount();
	if (bFly && (nAniFly == 0))
	{
		iAnimation* pAniFly = Magic_CreateAnimation();
		if (pAniFly)
		{
			stAnimationStruct aniFly;
			aniFly.nGroup = 16;
			aniFly.nAnispeed = 60; //速度
			aniFly.nGroupCount = 1;
			aniFly.nDir = 8;
			strcpy(aniFly.strAnimation, "fly");

            //soke npc位置grop
			size_t nPack = 42;
			if (pAniFly->CreateAnimation("Interfaces1", nPack, 0, aniFly, Usage_Character))
			{
				m_AniArrayFly.Add(pAniFly);
			}
		}
	}
	else if(m_AniArraySwim.GetAnimationCount() > 0 || m_AniArraySwimStand.GetAnimationCount() > 0)
	{
		m_AniArrayFly.FinalRelease();
	}
}



//soke 自行车配置
void CCharacter::SetInBikeState(bool bBike)
{
	int nAniSwim = m_AniArrayBike.GetAnimationCount();
	int nAniSwimStand = m_AniArrayBikeStand.GetAnimationCount();
	if (bBike && (nAniSwim == 0) && (nAniSwimStand == 0))
	{
		iAnimation* pAniSwim = Magic_CreateAnimation();
		iAnimation* pAniSwimStand = Magic_CreateAnimation();

		if (pAniSwim && pAniSwimStand)
		{
			stAnimationStruct aniSwim;
			stAnimationStruct aniSwimStand;
			//soke 控制图片ID
			aniSwimStand.nGroup = 14; //soke 挖宝站立
			aniSwim.nGroup = 16;      //soke 挖宝游泳
			aniSwimStand.nGroupCount = aniSwim.nGroupCount = 1;
			aniSwimStand.nAnispeed = 190; //soke 站立速度
			aniSwim.nAnispeed = 60;       //soke 游泳速度
			aniSwimStand.nDir = aniSwim.nDir = 8; //soke 方向
			strcpy(aniSwim.strAnimation, "Bike");
			strcpy(aniSwimStand.strAnimation, "BikeStand");

            //soke 角色外形grop位置 1785:1786
			size_t nPack = 0;
			nPack = (eSex_Male == this->GetSex()) ? 146: 147; //soke 男:女
			pAniSwim->CreateAnimation("pattern\\body23", nPack, 16, aniSwim, Usage_Character);
			pAniSwimStand->CreateAnimation("pattern\\body23", nPack, 14, aniSwimStand, Usage_Character);

			m_AniArrayBike.Add(pAniSwim);
			m_AniArrayBikeStand.Add(pAniSwimStand);
			
		}
	}
	else if(m_AniArrayBike.GetAnimationCount() > 0 || m_AniArrayBikeStand.GetAnimationCount() > 0)
	{
		m_AniArrayBike.FinalRelease();
		m_AniArrayBikeStand.FinalRelease();
	}
}

void CCharacter::SetInBikeState2(bool bBike)
{
	int nAniSwim = m_AniArrayBike2.GetAnimationCount();
	int nAniSwimStand = m_AniArrayBikeStand2.GetAnimationCount();
	if (bBike && (nAniSwim == 0) && (nAniSwimStand == 0))
	{
		iAnimation* pAniSwim = Magic_CreateAnimation();
		iAnimation* pAniSwimStand = Magic_CreateAnimation();

		if (pAniSwim && pAniSwimStand)
		{
			stAnimationStruct aniSwim;
			stAnimationStruct aniSwimStand;
			//soke 控制图片ID
			aniSwimStand.nGroup = 14; //soke 挖宝站立
			aniSwim.nGroup = 16;      //soke 挖宝游泳
			aniSwimStand.nGroupCount = aniSwim.nGroupCount = 1;
			aniSwimStand.nAnispeed = 190; //soke 站立速度
			aniSwim.nAnispeed = 60;       //soke 游泳速度
			aniSwimStand.nDir = aniSwim.nDir = 8; //soke 方向
			strcpy(aniSwim.strAnimation, "Bike2");
			strcpy(aniSwimStand.strAnimation, "BikeStand2");

            //soke 角色外形grop位置 1785:1786
			size_t nPack = 0;
			pAniSwim->CreateAnimation("npc20", 701, 16, aniSwim, Usage_Character);
			pAniSwimStand->CreateAnimation("npc20", 701, 14, aniSwimStand, Usage_Character);

			m_AniArrayBike2.Add(pAniSwim);
			m_AniArrayBikeStand2.Add(pAniSwimStand);
			
		}
	}
	else if(m_AniArrayBike2.GetAnimationCount() > 0 || m_AniArrayBikeStand2.GetAnimationCount() > 0)
	{
		m_AniArrayBike2.FinalRelease();
		m_AniArrayBikeStand2.FinalRelease();
	}
}


//soke 战车配置
void CCharacter::SetInZCState(bool bZc)
{
	int nAniSwim = m_AniArrayZC.GetAnimationCount();
	int nAniSwimStand = m_AniArrayZCStand.GetAnimationCount();
	if (bZc && (nAniSwim == 0) && (nAniSwimStand == 0))
	{
		iAnimation* pAniSwim = Magic_CreateAnimation();
		iAnimation* pAniSwimStand = Magic_CreateAnimation();

		if (pAniSwim && pAniSwimStand)
		{
			stAnimationStruct aniSwim;
			stAnimationStruct aniSwimStand;
			//soke 控制图片ID
			aniSwimStand.nGroup = 0; //soke 挖宝站立
			aniSwim.nGroup = 1;      //soke 挖宝游泳
			aniSwimStand.nGroupCount = aniSwim.nGroupCount = 1;
			aniSwimStand.nAnispeed = 30; //soke 站立速度
			aniSwim.nAnispeed = 30;       //soke 游泳速度
			aniSwimStand.nDir = aniSwim.nDir = 8; //soke 方向
			strcpy(aniSwim.strAnimation, "Zc");
			strcpy(aniSwimStand.strAnimation, "ZcStand2");

            //soke 角色外形grop位置 1785:1786
			size_t nPack = 0;
			pAniSwim->CreateAnimation("npc14", 529, 0, aniSwim, Usage_Character);
			pAniSwimStand->CreateAnimation("npc14", 529, 0, aniSwimStand, Usage_Character);

			m_AniArrayZC.Add(pAniSwim);
			m_AniArrayZCStand.Add(pAniSwimStand);
			
		}
	}
	else if(m_AniArrayZC.GetAnimationCount() > 0 || m_AniArrayZCStand.GetAnimationCount() > 0)
	{
		m_AniArrayZC.FinalRelease();
		m_AniArrayZCStand.FinalRelease();
	}
}


void CCharacter::SetInDanceState(bool bDance)
{
	int nAniDance = m_AniArrayDance.GetAnimationCount();
	if (bDance && (nAniDance == 0))
	{
		iAnimation* pAniDance = Magic_CreateAnimation();
		if (pAniDance)
		{
			stAnimationStruct aniDance;
			aniDance.nGroup = 0;
			aniDance.nAnispeed = 160;
			aniDance.nGroupCount = 1;
			aniDance.nDir = 8;
			strcpy(aniDance.strAnimation, "dance");

			size_t nPack = (eSex_Male == this->GetSex()) ? 200: 201;
			if (pAniDance->CreateAnimation("body4", nPack, 0, aniDance, Usage_Character))
			{
				m_AniArrayDance.Add(pAniDance);
			}
		}
	}
	else if(m_AniArrayDance.GetAnimationCount() > 0)
	{
		m_AniArrayDance.FinalRelease();
	}
}

//跳舞发型
void CCharacter::Set2InDanceState(bool bDance)
{
	int nAniDance = m2_AniArrayDance.GetAnimationCount();
	if (bDance && (nAniDance == 0))
	{
		iAnimation* pAniDance = Magic_CreateAnimation();
		if (pAniDance)
		{
			stAnimationStruct aniDance;
			aniDance.nGroup = 0;
			aniDance.nAnispeed = 160;
			aniDance.nGroupCount = 1;
			aniDance.nDir = 8;
			strcpy(aniDance.strAnimation, "dance2");

			size_t nPack = (eSex_Male == this->GetSex()) ? 60: 61;
			if (pAniDance->CreateAnimation("Hair", nPack, 0, aniDance, Usage_Character))
			{
				m2_AniArrayDance.Add(pAniDance);
			}
		}
	}
	else if(m2_AniArrayDance.GetAnimationCount() > 0)
	{
		m2_AniArrayDance.FinalRelease();
	}
}

//跳舞衣服填充
void CCharacter::Set3InDanceState(bool bDance)
{
	int nAniDance = m3_AniArrayDance.GetAnimationCount();
	if (bDance && (nAniDance == 0))
	{
		iAnimation* pAniDance = Magic_CreateAnimation();
		if (pAniDance)
		{
			stAnimationStruct aniDance;
			aniDance.nGroup = 0;
			aniDance.nAnispeed = 160;
			aniDance.nGroupCount = 1;
			aniDance.nDir = 8;
			strcpy(aniDance.strAnimation, "dance3");

			size_t nPack = (eSex_Male == this->GetSex()) ? 200: 201;
			if (pAniDance->CreateAnimation("pattern\\body22", nPack, 0, aniDance, Usage_Character))
			{
				m3_AniArrayDance.Add(pAniDance);
			}
		}
	}
	else if(m3_AniArrayDance.GetAnimationCount() > 0)
	{
		m3_AniArrayDance.FinalRelease();
	}
}
