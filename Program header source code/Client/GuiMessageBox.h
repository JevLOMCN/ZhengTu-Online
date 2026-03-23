#pragma once

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CGuiMessageBox :
	public CGuiDialog
{
	enumGuiActionType  m_type;
	void*				m_pData;
	float             m_fTime;

	void OnTimeOut(float fElapsedTime);

public:
	CGuiMessageBox(enumGuiActionType type,void * pData);
	void OnClose(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	bool OnButtonClick(UINT nID);
	void ProcessRequestTrade(UINT nID);
	void ProcessSelfRequestTrade(UINT nID);
	void ProcessBuyItem(UINT nID);// SOKE ½ð×Ó
	void ProcessSalaryBuyItem(UINT nID); //soke ¹¤×Ê
	void ProcessTicketBuyItem(UINT nID); //soke »ý·Ö
	void ProcessChongzhiBuyItem(UINT nID); //soke ³äÖµµã
	void ProcessExitTeam(UINT nID);
	void ProcessAnswerTeam(UINT nID,DWORD dwMode);
	void DelUser(UINT nID);
	void TongQuery(UINT nID);
	void SeptQuery(UINT nID);
	void FriendQuery(UINT nID);
	void TeacherQuery(UINT nID);	
	void ExitSchool(UINT nID);
	void ExitTong(UINT nID);
	void ExitSept(UINT nID);
	void DelTong(UINT nID);
	void DelSept(UINT nID);
	void DelSchool(UINT nID);
	void DelFriend(UINT nID);
	void DelBad(UINT nID);
	void MarryTaskQuery(UINT nID);
	void TongAttack(UINT nID);
	void SeptAttack(UINT nID);
	void SchoolAttack(UINT nID);
	void TongBallot(UINT nID,int ballot);
	void SeptBallot(UINT nID,int ballot);
	void CompetitionNpc(UINT nID);
	void Competition(UINT nID);
	void BuyObjectFromOthersShop(UINT nID);
	void HonorToPk(UINT nID);
	void ActionCloseShop(UINT nID);
	void ActionConfirmRecvMailItem(UINT nID);
	void LeaveCountry(UINT nID);
	void AddCountry(UINT nID);
	void AddCountrySalary(UINT nID);
	void Add2Salary(UINT nID);
	void Add2Login(UINT nID);
	void Add2Vip(UINT nID);
	void AddFubenjh(UINT nID);

	void AddTongUser(UINT nID);
	void AddSeptUser(UINT nID);

	void ActionBecomeArmyGen(UINT nID);
	void ActionFabao(UINT nID);
	void ActionConsignCartoon(UINT nID);

	void ConfirmAllyCountry(UINT nID);
	void ConfirmCancelAllyCountry(UINT nID);

	void OnExitLordStrikeLobby(UINT nID);
	void OnExitLordStrikeRoom(UINT nID);
	void OnExitLordStrikeGame(UINT nID);

	HRESULT OnRender( float fElapsedTime );
	int	 GetActionType() { return m_type; }
};



/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CQueryManager
{
	typedef std::map<DWORD,DWORD>     QUERYLIST;

	QUERYLIST                m_queryList;

	DWORD    CreateIndex(byte eState,DWORD dwID);

public:
	CQueryManager(void){};
	~CQueryManager(void){};
	
	void ClearList();
	bool FindListData(byte eState,DWORD dwID);
	void DeleteListData(byte eState,DWORD dwID);

};

extern CQueryManager* GetQueryManager();