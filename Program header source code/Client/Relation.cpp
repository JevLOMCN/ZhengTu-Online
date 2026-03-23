#include "public.h"
#include ".\Relation.h"
#include "./GameGuiManager.h"
#include "./GuiWorldConn.h"
#include "./Country.h"
#include "./Chat.h"
#include "GameScene.h"
#include "MainCharacter.h"

tRelation::tRelation( const char* pszName,DWORD c,bool online,const char* alias )
{
	strncpy(name,pszName,MAX_NAMESIZE);
	strncpy(aliasName,alias,MAX_NAMESIZE);
	country = c;
	bOnline = online;	
}
//////////////////////////////////////////////////////////////////////////


CRelation::CRelation(void)
{
	FUNCTION_BEGIN;
}

CRelation::~CRelation(void)
{
	FUNCTION_BEGIN;
}

bool CRelation::AddRelation(const char* name,DWORD id,bool bOnline,const char* alias,CRelation::RelationShip rs)	//关系类型
{	
	FUNCTION_BEGIN;
	if( FindRelationItByName(name,rs) == m_List[rs].end() )
	{
		tRelation t(name,id,bOnline,alias);
//#ifdef _DEBUG	
//		TRACE("%s (%d)	%s,%d,%s, %d\n",__FILE__,__LINE__,name,id,bOnline?"true":"false",rs );
//#endif
		m_List[rs].push_back(t);
		return true;
	}
	return false;
}

bool CRelation::RemoveRelation(const char* name,RelationShip rs)
{
	FUNCTION_BEGIN;

	tRelationListIt it = FindRelationItByName(name,rs);
	if( it != m_List[rs].end() )
	{
		m_List[rs].erase( it );
		return true;
	}
	return false;
}

CRelation::tRelationListIt CRelation::FindRelationItByName(const char* name,RelationShip rs)
{
	FUNCTION_BEGIN;
	for( tRelationListIt it = m_List[rs].begin(); 
		 it != m_List[rs].end(); ++it )
	{
		tRelation& r = *it;
		if( strcmp(r.name,name) == 0 )
		{
			return it;
		}
	}
	return m_List[rs].end();
}

bool CRelation::UpdateItOnline(const char* name,RelationShip rs,bool online)
{
	FUNCTION_BEGIN;

	for( tRelationListIt it = m_List[rs].begin(); 
		it != m_List[rs].end(); ++it )
	{
		tRelation& r = *it;
		if( strcmp(r.name,name) == 0 )
		{
			r.bOnline = online;
			return true;
		}
	}

	return false;
}

bool CRelation::ParseRelationMessage(stNullUserCmd* pCmd,size_t size)
{
	FUNCTION_BEGIN;
	switch(pCmd->byCmdType)
	{
	case UNION_USERCMD:
		{
			return ParseTongMessage(pCmd,size);
		}
		break;
	case RELATION_USERCMD:
		{
			return ParseFriendMessage(pCmd,size);
		}
		break;
	case SEPT_USERCMD:
		{
			return ParseFamilyMessage(pCmd,size);
		}
		break;
	}
	return false;	
}

bool CRelation::ParseTongMessage(stNullUserCmd* pCmd,size_t size)
{
	FUNCTION_BEGIN;
	DWORD dwCountry = GetScene()->GetMainCharacter()->GetCountry();
	switch(pCmd->byParam)
	{
	case BROADCAST_MEMBER_INFO_PARA:
		{
			stBroadcastMemberInfo* pCmdInfo=(stBroadcastMemberInfo*)pCmd;
			switch( pCmdInfo->byStatus )
			{
			case UNION_MEMBER_STATUS_ONLINE:	//上线
				{
					return UpdateItOnline(pCmdInfo->name,RELATION_UNION,true);
				}
				break;
			case UNION_MEMBER_STATUS_OFFLINE:	//下线
				{
					return UpdateItOnline(pCmdInfo->name,RELATION_UNION,false);
				}
				break;
			case UNION_MEMBER_STATUS_NEWMEMBER:	//新增
				{
					const char* pszAlias = (pCmdInfo->bySeptMaster!=0)?"帮主":"";
					return AddRelation(pCmdInfo->name,dwCountry,true,pszAlias,RELATION_UNION);
				}
				break;
			case UNION_MEMBER_STATUS_FIRE:		//开除
				{
					return RemoveRelation(pCmdInfo->name,RELATION_UNION);
				}
				break;
			}
		}
		break;
	case ALL_UNION_MEMBER_PARA:
		{
			stAllUnionMemberCmd* pCmdInfo=(stAllUnionMemberCmd*)pCmd;
			
			for (int i = 0;i < pCmdInfo->size; i++ )
			{
				stUnionRember& m = pCmdInfo->memberList[i];
				const char* pszAlias = (m.bySeptMaster!=0)?"帮主":"";
				AddRelation(m.memberName,dwCountry,m.byOnline != 0,pszAlias,RELATION_UNION);	//帮会的应该都是同一国家的
			}
			return true;
		}
		break;
	}
	return false;
}

bool CRelation::ParseFriendMessage(stNullUserCmd* pCmd,size_t size)
{
	FUNCTION_BEGIN;
	switch(pCmd->byParam)
	{
	case RELATION_STATUS_PARA:
		{
			stRelationStatusCmd* pCmdInfo=(stRelationStatusCmd*)pCmd;
			switch( pCmdInfo->byState )
			{
			case RELATION_ONLINE:		//上线
				{
					return UpdateItOnline(pCmdInfo->name,RELATION_FRIEND,true);
				}
				break;
			case RELATION_OFFLINE:		//离线
				{
					return UpdateItOnline(pCmdInfo->name,RELATION_FRIEND,false);
				}
				break;
			case RELATION_ADD:			//增加
				{
					return AddRelation(pCmdInfo->name,pCmdInfo->country,1,"",RELATION_FRIEND);
				}
				break;
			case RELATION_REMOVE:		//开除
				{
					return RemoveRelation(pCmdInfo->name,RELATION_FRIEND);
				}
				break;
			}
		}
		break;
	case RELATION_SEND_RELATIONLIST_PARA:
		{
			stSendRelationListCmd* pCmdInfo=(stSendRelationListCmd*)pCmd;
			for (int i = 0;i < pCmdInfo->size; i++ )
			{
				stRelation& r = pCmdInfo->member[i];
				AddRelation(r.name,r.country,r.online != 0,"",RELATION_FRIEND);
			}
			return true;
		}
		break;
	}
	return true;
}

bool CRelation::ParseFamilyMessage(stNullUserCmd* pCmd,size_t size)
{
	FUNCTION_BEGIN;

	DWORD dwCountry = GetScene()->GetMainCharacter()->GetCountry();
	switch(pCmd->byParam)
	{
	case BROADCAST_SEPT_MEMBER_INFO_PARA:  //更新家族成员状态及信息
		{
			stBroadcastSeptMemberInfo* pCmdInfo=(stBroadcastSeptMemberInfo*)pCmd;
			switch(pCmdInfo->byStatus) 
			{
			case SEPT_MEMBER_STATUS_ONLINE:  // 家族成员上线
				{
					return UpdateItOnline(pCmdInfo->name,RELATION_FAMILY,true);
				}
				break;
			case SEPT_MEMBER_STATUS_OFFLINE:  // 家族成员下线
				{
					return UpdateItOnline(pCmdInfo->name,RELATION_FAMILY,false);
				}
				break;
			case SEPT_MEMBER_STATUS_FIRE:    // 家族成员被开除
				{
					return RemoveRelation(pCmdInfo->name,RELATION_FAMILY);
				}
				break;
			case SEPT_MEMBER_STATUS_NEWMEMBER: // 新增加家族成员
				{
					return AddRelation(pCmdInfo->name,dwCountry,true,pCmdInfo->aliasName,RELATION_FAMILY);
				}
				break;
			}
		}
		break;
	case ALL_SEPT_MEMBER_PARA:   //成员列表
		{
			stAllSeptMemberCmd* pCmdInfo=(stAllSeptMemberCmd*)pCmd;
			for (int i = 0;i < pCmdInfo->size; i++ )
			{
				stSeptRember& s = pCmdInfo->memberList[i];
				AddRelation(s.memberName,dwCountry,s.byOnline != 0,s.memberAliasName,RELATION_FAMILY);
			}
		}
		break;
	}
	return true;
}

void CRelation::Release()
{
	FUNCTION_BEGIN;
	for( int i = 0 ; i < MAX_RELATIONSHIP; i++ )
	{
		m_List[i].clear();
	}
}

bool CRelation::ShowRelationOnListBox(CGuiListBox* pListBox,tRelationList& list)
{
	FUNCTION_BEGIN;
	Assert(pListBox);
	pListBox->RemoveAllItems();

	int i = 0;
	for( tRelationListIt it = list.begin(); 
		 it != list.end(); ++it )
	{
		tRelation& r = *it;
		if( r.bOnline )	//列出所有玩家的列表（在线和不在线）
		{
			pListBox->AddItem("",NULL);
			pListBox->SetItemText(i,0,r.name,22,GuiListBoxColFmt_Center);
			char szCountry[MAX_NAMESIZE];	
			BOOL bRes = CCountry::Instance()->GetCountryName(r.country,szCountry);
			if( !bRes )
			{
				strncpy(szCountry,GetScene()->GetMainCharacter()->GetCountryName(),MAX_NAMESIZE);
			}
			pListBox->SetItemText(i,1,szCountry,16,GuiListBoxColFmt_Center);
			++i;
		}
	}

	return true;
}

bool CRelation::OnUpdate(CGuiListBox* pListBox,RelationShip rs)
{
	FUNCTION_BEGIN;
	return ShowRelationOnListBox(pListBox,m_List[rs]);
}	

bool CRelation::AddMember2ChatChannel(CChatChannel* pChannel,RelationShip rs)
{
	FUNCTION_BEGIN;
	Assert(pChannel);

	for( tRelationListIt it = m_List[rs].begin(); 
		it != m_List[rs].end(); ++it )
	{
		tRelation& r = *it;
		pChannel->AddMember( r.name,r.aliasName );
	}
	return true;
}