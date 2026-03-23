/********************************************************************
	created:	2006/01/17
	created:	17:1:2006   16:21
	filename: 	m:\codes\NewGame\b\Client\AutoGame.cpp
	file path:	m:\codes\NewGame\b\Client
	file base:	AutoGame
	file ext:	cpp
	author:		$$$03公司
	
	purpose:	自动游戏功能
*********************************************************************/

#include "public.h"
#include "Game.h"
#include "./MainCharacter.h"
#include "./AutoGame.h"
#include "./RoleItem.h"
#include "./GameScene.h"
#include "./Item.h"
#include "../engine/include/stack_alloc.h"
#include "./GameGuiManager.h"
#include "./GuiTeam.h"
#include "./GuiUserInfo.h"
#include "./RoleSkill.h"
#include "./GuiMain.h"
#include "./GameAppation.h"
#include "./GuiAutoAttackDlg.h" ///////////star100515
#include "./GameGuiManager.h"      ///////////star100722
#include "./GuiPetCtrlDialog.h"
class CGuiPetCtrlDialog;
template <class tVectorInt>
void _parse_str_num_set(tVectorInt & vRet,const char* pszString)
{
	vRet.clear();
	const char* p1,*p2;
	p1 = pszString;
	for(;;)
	{
		while(*p1 != 0 && !( '0' <= *p1 && *p1 <= '9'))
		{
			p1++;
		}

		if(*p1 == 0) break;

		p2 = p1;

		while(*p2 != 0 && ( '0' <= *p2 && *p2 <= '9'))
		{
			p2++;
		}

		char szTmp[256];
		int len = getMin(p2-p1,255);
		memcpy(szTmp,p1,len);
		szTmp[len] = 0;

		vRet.insert(atol(szTmp));

		if(*p2 == 0) break;

		p1 = p2+1;
	}
}

typedef void (*t_fnReLoadConfig)();

static void ReLoadConfig()
{
	gAutoGameConfig.loadConfig();
}

typedef void (*t_fnSetReLoadConfiFun)( t_fnReLoadConfig fn);

stAutoGameConfig gAutoGameConfig;
stAutoGameConfig::stAutoGameConfig()
{
	minHP = 0;
	minHPPercent = 0.25f;
	useMinHPPercent = true;
	autoAddHP = true;
	autoAddMP = true;
	autoPickupObject = true;
#ifdef _AUTOPLAY
	hConfigDll = LoadLibrary("AutoPlayPrj.dll");
	if(hConfigDll){
		fnShowConfigDlg = (t_fnShowConfigDlg)GetProcAddress(hConfigDll,"@ShowConfigDlg$qpv");
		fnLoadNpcTables = (t_fnLoadNpcTables)GetProcAddress(hConfigDll,"@LoadNpcTables$qpuc");
		fnLoadSkillTables = (t_fnLoadSkillTables)GetProcAddress(hConfigDll,"@LoadSkillTables$qpuc");
		fnLoadSkillLevelTables = (t_fnLoadSkillLevelTables)GetProcAddress(hConfigDll,"@LoadSkillLevelTables$qpuc");
		t_fnSetReLoadConfiFun fnReLoadConfig = (t_fnSetReLoadConfiFun)GetProcAddress(hConfigDll,"@SetReLoadConfiFun$qpqv$v");
		fnLoadConfig = (t_fnLoadConfig)GetProcAddress(hConfigDll,"@LoadConfig$qv");
		fnSetStateSkill = (t_fnSetStateSkill)GetProcAddress(hConfigDll,"@SetStateSkill$qpulul");
		fnReSetSkillData = (t_fnReSetSkillData)GetProcAddress(hConfigDll,"@ReSetSkillData$qp9SkillBaseul");
		if(fnReLoadConfig)
			fnReLoadConfig(ReLoadConfig);
	}else{

		fnShowConfigDlg = NULL;
		fnLoadNpcTables = NULL;
		fnLoadSkillTables = NULL;
		fnLoadSkillLevelTables = NULL;
		fnLoadConfig = NULL;
		fnSetStateSkill = NULL;
		fnReSetSkillData = NULL;

		Engine_WriteLog("LoadLibrary AutoPlayPrj.dll failed!\n");
	}
#endif
}

stAutoGameConfig::~stAutoGameConfig()
{
#ifdef _AUTOPLAY
	if(hConfigDll){
		fnShowConfigDlg = NULL;
		FreeLibrary(hConfigDll);
	}
#endif
}

void stAutoGameConfig::loadConfig()
{
#ifdef _AUTOPLAY
	minHPPercent = 0.5f;
	minHP = 100;
	useMinHPPercent = 1;
	setCullNPC.clear();
	stateSkills.clear();
	TiXmlDocument xml;
	if(xml.LoadFile("AutoPlay.cfg")){
		TiXmlElement* config = xml.RootElement();
		if(config){
			TiXmlElement* base = config->FirstChildElement("base");
			if(base){
				int num;
				if(TIXML_SUCCESS == base->QueryIntAttribute("MinHPP",&num))
					minHPPercent= num/100.0f;
				if(TIXML_SUCCESS == base->QueryIntAttribute("MinHPV",&num))
					minHP = num;
				if(TIXML_SUCCESS == base->QueryIntAttribute("AddHPByPer",&num))
					useMinHPPercent = ( num == 0 ? false : true );
			}
			TiXmlElement * attack = config->FirstChildElement("attack");
			if(attack){
				TiXmlElement * noAttackNpc = attack->FirstChildElement("noAttackNpc");
				if(noAttackNpc){
					std::string str = XML_GetNodeText(noAttackNpc);
					_parse_str_num_set(setCullNPC,str.c_str());
				}
				TiXmlElement * stateSkill = attack->FirstChildElement("stateSkills");
				if(stateSkill){
					std::string str = XML_GetNodeText(stateSkill);
					_parse_str_num(stateSkills,str.c_str());
				}
			}
		}
	}
#endif
}

void CMainCharacter::AutoAddHP(int minHP)
{
#ifdef _AUTOPLAY
	struct stObjectInfo{
		DWORD objectID;
		DWORD hp;
	};
	stObjectInfo aObj[] = {
		{581,750},//大还丹 补充生命值750点
		{580,450},//小还丹 补充生命值450点
		{584,400},//仙宿丹 每秒恢复生命值400点,持续10秒
		{579,200},//铜还丹 补充生命值200点
		{583,200},//九华丹 每秒恢复生命值200点,持续10秒
		{578,100},//铁还丹 补充生命值100点
		{582,100},//还魂丹 每秒恢复生命值100点,持续10秒
	};

	if(minHP > GetMaxHP())
		return;
	if(GetHP() < minHP){
		for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it){
			CRoleItem* pItem = (*it);
			if( pItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON 
				&& pItem->GetObjectID() == 655)
			{
					//生命之源
					CALL_USEITEM(pItem->GetThisID());
					return;
			}
		}

		bool bFind = false;
		int dHP = GetMaxHP() - minHP;
		for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it){
			CRoleItem* pItem = (*it);
			if( pItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON 
				&& pItem->GetObjectID() == 655){// 可以按需补的药水
				CALL_USEITEM(pItem->GetThisID());
				return;
			}
		}

		for(size_t i=0; i<count_of(aObj); ++i){
			if(dHP >= aObj[i].hp){
				for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it){
					CRoleItem* pItem = (*it);
					if( pItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON 
						&& pItem->GetObjectID() == aObj[i].objectID){
							CALL_USEITEM(pItem->GetThisID());
							break;
						}
				}
			}
		}

		for(int i=count_of(aObj)-1; i>=0; --i){
			for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it){
				CRoleItem* pItem = (*it);
				if( pItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON 
					&& pItem->GetObjectID() == aObj[i].objectID){
						CALL_USEITEM(pItem->GetThisID());
						break;
					}
			}
		}
	}
#endif
}

void CMainCharacter::AutoAddMP(int minMP)
{
#ifdef _AUTOPLAY
	struct stObjectInfo{
		DWORD objectID;
		DWORD mp;
	};
	stObjectInfo aObj[] = {
		{588,300},//灵玉露 补充法力值300点
		{587,100},//定心露 补充法力值150点
		{591,150},//丹凤露 每秒恢复法力值150点,持续10秒
		{590,100},//百花露 每秒恢复法力值100点,持续10秒
		{591,75}, //天星露 每秒恢复法力值75点,持续10秒
		{586,70}, //安神露 补充法力值70点
		{585,30}, //甘草露 补充法力值30点
	};

	if(minMP > GetMaxMP())
		return;
	if(GetMP() < minMP){
		for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it){
			CRoleItem* pItem = (*it);
			if( pItem->GetLocation().dwLocation == OBJECTCELLTYPE_EQUIP 
				&& pItem->GetObjectID() == 793){
					//装备了法力之源，服务器自动补
					return;
			}
		}

		bool bFind = false;
		int dMP = GetMaxMP() - minMP;

		for(size_t i=0; i<count_of(aObj); ++i){
			if(dMP >= aObj[i].mp){
				for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it){
					CRoleItem* pItem = (*it);
					if( pItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON 
						&& pItem->GetObjectID() == aObj[i].objectID){
						CALL_USEITEM(pItem->GetThisID());
						break;
					}
				}
			}
		}

		for(int i=count_of(aObj)-1; i>=0; --i){
			for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it){
				CRoleItem* pItem = (*it);
				if( pItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON 
					&& pItem->GetObjectID() == aObj[i].objectID){
						CALL_USEITEM(pItem->GetThisID());
						break;
					}
			}
		}
	}
#endif
}

static int cmpItemByPos(const void *p1, const void *p2) 
{
	CItem* item1 = *(CItem**)p1;
	CItem* item2 = *(CItem**)p2;
	int dis1 = Scene_GetDis(GetScene()->GetMainCharacter()->GetGridPos(),item1->GetGridPos());
	int dis2 = Scene_GetDis(GetScene()->GetMainCharacter()->GetGridPos(),item2->GetGridPos());
	return dis1 - dis2;
}

typedef std::vector<CItem*,std::stack_allocator<CItem*> > tTmpItemVec;
bool CMainCharacter::RunAutoPickup()
{
//#ifdef _AUTOPLAY
	//if(GetGameGuiManager()->IsPackgetFull())
	//	return false;

	stPointI ptGrid = GetGridPos();
	stRectI rc(ptGrid.x - 8,ptGrid.y - 8,ptGrid.x + 9,ptGrid.y + 9);
	SetFrameAllocator fa;
	tTmpItemVec aItem;
	GetScene()->GetObjectAtGridRect<CItem,tTmpItemVec >(rc,(tTmpItemVec&)aItem);
	qsort(&aItem[0],aItem.size(),sizeof(CItem*),cmpItemByPos);
	bool retval = false;
	size_t pickupCount = 0;
	DWORD curTime = xtimeGetTime();
	for(size_t i=0; i<aItem.size(); ++i)
	{
		CItem* pItem = aItem[i];
		DWORD dwBaseID = pItem->GetBaseID();
		if( (dwBaseID != c_nHuoYunFu) &&
			(dwBaseID != c_nXianTianFu) &&
			(dwBaseID != c_nXuanYuanFu) ) 
		{
			DWORD dwOwner = pItem->GetOwner();
			bool bCanPickFoOwner = true;
			if( dwOwner != 0 && dwOwner != GetID())
			{
				bCanPickFoOwner = 
					GetGameGuiManager()->m_guiTeam && 
					NULL != GetGameGuiManager()->m_guiTeam->FindMember(GetID())
					&& GetGameGuiManager()->m_guiTeam->m_dwCurObjAssign == TEAM_OBJ_MODE_NORMAL;
				if(!bCanPickFoOwner)
					continue;
			}
		}
		if(gAutoGameConfig.setCullObject.find(pItem->GetBaseID()) == gAutoGameConfig.setCullObject.end())
		{
			if(curTime - pItem->m_LastSendPickCmdTime > 1 && curTime - pItem->m_LastTryPickTime > 1){
				if( Scene_GetDis(GetGridPos(),pItem->GetGridPos()) > 1 ){
					//TryPickUpItem(pItem);
					////RunPickUpEvent();
					PickUpItem(pItem);////////star100810
					retval = true;
					break;
				}else{
					if(pickupCount > 0)
						Sleep(1);
					PickUpItem(pItem);
				}
			}
		}
	}
	return retval;
//#else
//	return false;
//#endif
}

bool CMainCharacter::RunAutoWork()
{
#ifdef _AUTOPLAY
	DWORD curTime = xtimeGetTime();
	for(std::vector<DWORD>::iterator it = gAutoGameConfig.stateSkills.begin(); it != gAutoGameConfig.stateSkills.end(); ++it){
		SkillBase_t* pSkill = GetSkillBase(*it);
		if(pSkill){
			if(!GetGameGuiManager()->m_guiMain->isHasStateSkill(pSkill->dwID) && GetSkillDisableStatus(pSkill) > 0.99f){
				if(GetMP() < pSkill->dwMP){
					AutoAddMP(pSkill->dwMP);
					return true;
				}
				CRoleSkill* pRoleSkill = FindSkillByID(*it);
				if(pRoleSkill){
					m_RunEvent.id = GetID();
					m_RunEvent.target = eTargetType_Character;
					m_RunEvent.pSkill = pSkill;
					m_RunEvent.pt = GetPos();
					m_RunEvent.speed = GetMoveSpeed();
					m_RunEvent.event = eEventType_Attack;
					return true;
				}
			}
		}
	}
	return false;
#else
	return false;
#endif
}
//////////////////star100515
bool CMainCharacter::RunAutoWorkSkill()
{
	DWORD curTime = xtimeGetTime();
	for(std::vector<sAutoSkill>::iterator iPter = GetGameGuiManager()->m_guiAutoAttackDlg->m_PassiveSkill.begin(); 
		iPter != GetGameGuiManager()->m_guiAutoAttackDlg->m_PassiveSkill.end(); ++iPter)
	{
		if( iPter->bUse)
		{
			SkillBase_t* pSkill = GetSkillBase(iPter->dID);
			if(pSkill)
			{
				float iii = GetSkillDisableStatus(pSkill);
				if(!GetGameGuiManager()->m_guiMain->isHasStateSkill(pSkill->dwID) && GetSkillDisableStatus(pSkill) > 0.99f)
				{
					if(GetMP() < pSkill->dwMP)
						return false;
					CRoleSkill* pRoleSkill = FindSkillByID(iPter->dID);

					if( pSkill->dwID == 209 || pSkill->dwID == 212 || pSkill->dwID == 217)
					{
						if( GetGameGuiManager()->m_guiPetFloatDlg !=NULL )
						{
							const t_PetData *pData = GetGameGuiManager()->m_guiPetFloatDlg->GetCurrentPet()->GetProperty();
							if(pData != NULL)
							if( pData->hp >  5 )
							continue;
						}
					}
					if(pRoleSkill)
					{
						m_RunEvent.id = GetID();
						m_RunEvent.target = eTargetType_Character;
						m_RunEvent.pSkill = pSkill;
						m_RunEvent.pt = GetPos();
						m_RunEvent.speed = GetMoveSpeed();
						m_RunEvent.event = eEventType_Attack;
						return true;
					}
				}
			}
		}
	}
	return true;
}
//////////////////end

void enableAutoGame(bool enable)
{
#ifdef _AUTOPLAY
	if(enable){
		gAutoGameConfig.minHP = 0;
		gAutoGameConfig.minHPPercent = 0.2f;
		gAutoGameConfig.useMinHPPercent = true;
		gAutoGameConfig.autoAddHP = true;
		gAutoGameConfig.autoAddMP = true;
		gAutoGameConfig.autoPickupObject = true;
		gAutoGameConfig.loadConfig();
	}else{
		gAutoGameConfig.minHP = 0;
		gAutoGameConfig.minHPPercent = 0.2f;
		gAutoGameConfig.useMinHPPercent = false;
		gAutoGameConfig.autoAddHP = false;
		gAutoGameConfig.autoAddMP = false;
		gAutoGameConfig.autoPickupObject = false;
	}
#endif
}

void CMainCharacter::AutoAddHpBySpecialEquip()
{
	//Confirm if equiped 655
	if ( NULL == GetGameGuiManager()->m_guiUserInfo )
		return;

	if ( !GetGameGuiManager()->m_guiUserInfo->IsEquipedAutoHpItem() )
		return;

	//Let's Start
	struct stObjectInfo{
		DWORD objectID;
		DWORD hp;
	};
	stObjectInfo aObj[] = {
		{581,750},
		{580,450},
		{579,200},
		{578,100},
	};
	stObjectInfo bObj[] = {
		{582,100},
		{583,200},
		{584,400},
	};

	int minHpA = (int)(GetMaxHP() * 0.5f);
	int minHpB = (int)(GetMaxHP() * 0.7f);
	if ( GetHP() > minHpB )
		return;

	//First use big hp item
	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it){
		CRoleItem* pItem = (*it);
		if( pItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON 
			&& ( pItem->GetItemType() == ItemType_FillHP ) )
		{
			//生命之源
			CALL_USEITEM(pItem->GetThisID());
			return;
		}
	}

	//First add item that can last
	if ( GetHP() > minHpA )
	{
		int dHpB = GetMaxHP() - GetHP();
		for( size_t i=0;i<count_of(bObj);++i ){
			if ( bObj[i].hp * 4 >= dHpB ){
				for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it){
					CRoleItem* pItem = (*it);
					if( pItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON 
						&& pItem->GetObjectID() == bObj[i].objectID){
							CALL_USEITEM(pItem->GetThisID());
							return;
							break;
						}
				}
			}
		}
	}	
		
	//Then use item that do one time
	int dHpA = GetMaxHP() - GetHP();
	for(size_t i=0; i<count_of(aObj); ++i){
		if(dHpA >= aObj[i].hp){
			for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it){
				CRoleItem* pItem = (*it);
				if( pItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON 
					&& pItem->GetObjectID() == aObj[i].objectID){
						CALL_USEITEM(pItem->GetThisID());
						break;
					}
			}
		}
	}	
}

bool CMainCharacter::AutoEquipFillItem( const int& xPos, const int& type )
{
	if ( type < 0 )
		return false;

	for(tListItem::iterator it = m_listItem.begin(); it != m_listItem.end(); ++ it)
	{
		CRoleItem* pItem = (*it);
		if ( (pItem->GetItemType() == type)
			&& (pItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON)
			&& (pItem->GetItemDur() >= 60 ) )
		{
			//Yes we find one
			if ( GetGameGuiManager()->m_guiUserInfo == NULL )
				return false;

			CGuiTable* pTable = GetGameGuiManager()->m_guiUserInfo->GetEquipTable(EQUIPCELLTYPE_ADORN, xPos );

			if ( NULL == pTable )
				return false;

			// 交换物品
			stObjectLocation dst;

			PlayItemSound(pItem->GetObjectBase()->itmeSound);
			dst.dwLocation = pTable->m_iTableType;
			dst.dwTableID = pTable->m_iTableID;

			dst.x = pTable->m_EquipPosition.x;
			dst.y = pTable->m_EquipPosition.y;

			pItem->MoveTo(dst);

			return true;
			break;
		}
	}

	return false;
}

void stAutoGameConfig::showConfigWnd()
{
#ifdef _AUTOPLAY
	if(fnShowConfigDlg)
		fnShowConfigDlg(GetGameApplication()->m_hWnd);
	if(fnSetStateSkill){
		CMainCharacter* pMain = GetScene()->GetMainCharacter();
		if(pMain){
			std::vector<DWORD> ids;
			for(CMainCharacter::tListSkill::iterator it = pMain->m_listSkill.begin(); it != pMain->m_listSkill.end(); ++it){
				SkillBase_t * p = (*it)->GetSkillBase();
				if( (p->dwType == SkillType_StateAttack || p->dwType == SkillType_State) && (p->dwTarget & SkillTarget_Self) ){
					ids.push_back(p->dwID);
				}
			}
			fnSetStateSkill(&ids[0],ids.size());
		}
	}
	if(fnLoadConfig)
		fnLoadConfig();
#endif
}