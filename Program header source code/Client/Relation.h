#pragma once

struct tRelation
{
	char	name[MAX_NAMESIZE];
	char	aliasName[MAX_NAMESIZE];
	DWORD	country;
	bool	bOnline;
	

	tRelation( const char* pszName,DWORD c,bool online,const char* alias );
};

class CGuiListBox;
class CChatChannel;

class CRelation
{
public:
	typedef std::vector<tRelation>		tRelationList;
	typedef tRelationList::iterator		tRelationListIt;
public:
	CRelation(void);	
	~CRelation(void);

	enum RelationShip
	{
		RELATION_FAMILY,		//家族
		RELATION_FRIEND,		//好友
		RELATION_UNION,			//帮会
		MAX_RELATIONSHIP
	};

	bool AddRelation(const char* name,DWORD id,bool bOnline,const char* alias,RelationShip rs);	// add 关系类型
	bool RemoveRelation(const char* name,RelationShip rs);						// remode

	bool ParseRelationMessage(stNullUserCmd* pCmd,size_t size);
	void Release();

	bool AddMember2ChatChannel(CChatChannel* pChannel,RelationShip rs);
	bool OnUpdate(CGuiListBox* pListBox,RelationShip rs);
private:
	tRelationListIt FindRelationItByName(const char* name,RelationShip rs);

	bool UpdateItOnline(const char* name,RelationShip rs,bool online);

	bool ParseTongMessage(stNullUserCmd* pCmd,size_t size);
	bool ParseFriendMessage(stNullUserCmd* pCmd,size_t size);
	bool ParseFamilyMessage(stNullUserCmd* pCmd,size_t size);

	bool ShowRelationOnListBox(CGuiListBox* pListBox,tRelationList& list);
private:
	tRelationList	m_List[MAX_RELATIONSHIP];		//家族
};

inline CRelation&	GetRelation()
{
	static CRelation r;return r;
}
