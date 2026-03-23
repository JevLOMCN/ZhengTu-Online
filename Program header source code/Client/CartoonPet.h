#ifndef _CARTOON_PET_H
#define _CARTOON_PET_H


class CCartoonPet 
{
public:
	CCartoonPet();
	~CCartoonPet();

	void SetAttribute(stAddCartoonCmd* cmd)
	{
		m_data = cmd->data;
		m_dwCartoonID = cmd->cartoonID;
		m_bIsMine = cmd->isMine;
	}
	void Update(stHpExpCartoonCmd* cmd)
	{
		m_data.sp = cmd->sp;
		m_data.exp = cmd->exp;
	}

	const char* GetName() const { return m_data.name; }
	t_CartoonData& GetData() { return m_data; }	
	DWORD		GetCartoonID() const { return m_dwCartoonID; }
	bool		IsMine() const { return m_bIsMine; }
	void		Rename(const char* newname);
	NpcBase_t*  GetNpcBase();
	BYTE		GetState() const { return m_data.state; }

	bool		IsCanFollow();		//可以跟随
	bool		IsCanPickup();		//可以收起
	bool		IsCanStartTraining();	//可以开始修炼
	bool		IsCanEndTraining();		//可以终止修炼
	bool		IsCanFeed();			//可以喂养
	bool		IsCanAddCent();			//可以充值
	bool		IsCanFetchExp();		//可以提取经验值
	bool		IsCanStartAutoRepair();	//可以自动修理
	bool		IsCanEndAutoRepari();	//可以终止自动修理
	bool		IsHaveNext();
	bool		IsHavePre();
	CCartoonPet* GetNextCartoon();
	CCartoonPet* GetPreCartoon();

	static	void BuyCartoonPet(DWORD dwNpcID);		//购买宠物
	static  void CartoonFollow(DWORD dwCartoonID);	//跟随
	static  void CartoonLetOut(DWORD dwCartoonID);	//开始修炼
	static  void AdoptCartoon(DWORD dwCartoonID);	//领养别人的宠物
	static  void ReturnCartoon(DWORD dwCartoonID);	//把别人的宠物还回去
	static  void GetBackCartoon();					//把自己的宠物要回来(终止修炼)
	static  void DrawExpCartoon(DWORD dwCartoonID);	//提取经验值
	static	void RequestWaitCartoonList();			//请求领养宠物列表
	static	void AddCent(DWORD dwCartoonID,DWORD time);			//给宠物充值
	static  void AutoRepair(DWORD dwCartoonID,BYTE repair);		//自动修理 开启/关闭

	static const char* CartoonState2String(BYTE state,CCartoonPet* pCartoon = NULL);
	static const char* CartoonState2String(CCartoonPet* pCartoon);

	static bool  IsCanConsignCartoon();	//是否可以指定领养者

private:
	int GetIndexInCartoonPetList();
private:
	t_CartoonData	m_data;
	DWORD			m_dwCartoonID;
	bool			m_bIsMine;
};

class CCartoonPetList
{
public:
	typedef std::vector<CCartoonPet*>	tListCartoonPet;
	typedef tListCartoonPet::iterator	tListCartoonPetIt;

	CCartoonPetList();
	~CCartoonPetList();

	void Release();
	void AddCartoon(stAddCartoonCmd* cmd);
	void RemoveCartoon(stRemoveCartoonCmd* cmd);
	void UpdateCartoonPet(stHpExpCartoonCmd* cmd);
	CCartoonPet* GetCartoon( DWORD dwCartoonID );
	CCartoonPet* GetCartoonByNpcID( DWORD dwNpcID );
	void Rename( DWORD dwCartoonID,const char* newname );

	int  GetCanUseCellsNum();	//返回替身宝宝包裹可用的格子数

	CCartoonPet* GetAt(int pos);
	CCartoonPet* operator [](int index);

	bool IsHaveLetoutCartoonPet();		//是否有修炼的宠物
	bool IsHaveFollowCartoonPet();		//是否有跟随的宠物
	bool IsHaveAdoptedCartoonPet();		//是否有被领养的宠物
	
#ifdef _DEBUG
	void TraceList();
#endif

	tListCartoonPetIt	GetCartoonPetIt( DWORD dwCartoonID );

	size_t	GetSize() const { return m_listCartoon.size(); }
	bool    empty() const { return m_listCartoon.empty(); }
private:
	tListCartoonPet		m_listCartoon;
};

//等待领养的宠物列表
class CWaitingCartoonPetList
{
public:
	struct t_WaitingCartoonPet	
	{
		DWORD	cartoonID;					//宝宝id
		DWORD	npcID;						//NPC ID
		char	name[MAX_NAMESIZE];			//名字
		char	masterName[MAX_NAMESIZE];	//主人名字
		BYTE	state;						//状态
		DWORD	time;						//挂机剩余时间(秒)
		NpcBase_t*	pNpcBase;				//npcbase
	};

	typedef std::vector<t_WaitingCartoonPet*>	tListWaitingCartoonPet;
	typedef tListWaitingCartoonPet::iterator	tListWaitingCartoonPetIt;

	CWaitingCartoonPetList();
	~CWaitingCartoonPetList();

	void	Release();

	tListWaitingCartoonPet& GetList() { return m_listWaitingCartoonPet; } 

	void	AddCartoonPet(stAddWaitingCartoonCmd* cmd);
	void	RemoveCartoonPet(stRemoveWaitingCartoonCmd* cmd);	

	t_WaitingCartoonPet* GetCartoonPet( DWORD dwCartoonID );
	tListWaitingCartoonPetIt GetCartoonPetIt( DWORD dwCartoonID );
	BYTE	GetCartoonPetState( DWORD dwCartoonID );

	bool    IsCanAdoptCartoonPet();
	bool	IsHaveAdoptedCartoonPet();
#ifdef _DEBUG
	void TraceList();
#endif


private:
	tListWaitingCartoonPet m_listWaitingCartoonPet;
};

#endif