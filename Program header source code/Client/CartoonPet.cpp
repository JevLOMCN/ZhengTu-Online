#include "public.h"
#include ".\CartoonPet.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./GuiPetDialog.h"
#include "./GuiAdoptCartoonDialog.h"
#include ".\GuiPetPackDialog.h"

CCartoonPet::CCartoonPet() 
{ 
	m_bIsMine = true; 
}

CCartoonPet::~CCartoonPet()
{
}

void CCartoonPet::Rename(const char* newname)
{
	if( newname && strlen(newname) > 0 )
	{
		_snprintf(m_data.name,32,newname);
		m_data.name[31] = 0;
		stChangeNameCartoonCmd cmd;
		cmd.cartoonID = GetCartoonID();
		_snprintf(cmd.name,32,newname);
		cmd.name[31] = 0;
		SEND_USER_CMD( cmd );
	}
}

NpcBase_t*  CCartoonPet::GetNpcBase()
{
	return ::GetNpcBase( m_data.npcID );
}

bool CCartoonPet::IsHaveNext()
{
	int index = GetIndexInCartoonPetList();
	return ( index != -1 && index < ( GetScene()->GetCartoonPetList()->GetSize() - 1) );
}

bool CCartoonPet::IsHavePre()
{
	int index = GetIndexInCartoonPetList();
	return ( index > 0 );
}

CCartoonPet* CCartoonPet::GetNextCartoon()
{
	int index = GetIndexInCartoonPetList();
	return GetScene()->GetCartoonPetList()->GetAt( index + 1 );
}

CCartoonPet* CCartoonPet::GetPreCartoon()
{
	int index = GetIndexInCartoonPetList();
	return GetScene()->GetCartoonPetList()->GetAt( index - 1 );
}

int CCartoonPet::GetIndexInCartoonPetList()
{
	for(int i = 0; i < GetScene()->GetCartoonPetList()->GetSize(); ++i )
	{
		CCartoonPet* pCartoon = GetScene()->GetCartoonPetList()->GetAt( i );
		if( pCartoon && pCartoon->GetCartoonID() == GetCartoonID() )
		{
			return i;
			break;
		}
	}
	return -1;
}

void CCartoonPet::BuyCartoonPet(DWORD dwNpcID)		//购买宠物
{
	stBuyCartoonCmd cmd;
	cmd.npcID = dwNpcID;
	SEND_USER_CMD( cmd );
//#ifdef _DEBUG
//	char szTemp[64];
//	_snprintf(szTemp,64,"stBuyCartoonCmd:%d",dwNpcID);
//	szTemp[63] = 0;
//	GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
//#endif
}

void CCartoonPet::CartoonFollow(DWORD dwCartoonID)	//跟随(id)/取消跟随(id == 0)
{
	// dwCartoonID == 0 取消跟随
	stFollowCartoonCmd cmd;
	cmd.cartoonID = dwCartoonID;
	SEND_USER_CMD( cmd );
//#ifdef _DEBUG
//	char szTemp[64];
//	_snprintf(szTemp,64,"stFollowCartoonCmd:%d",dwCartoonID);
//	szTemp[63] = 0;
//	GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
//#endif
}

void CCartoonPet::CartoonLetOut(DWORD dwCartoonID)	//放出宠物 == 开始修炼
{
	stLetOutCartoonCmd cmd;
	cmd.cartoonID = dwCartoonID;
	SEND_USER_CMD( cmd );
//#ifdef _DEBUG
//	char szTemp[64];
//	_snprintf(szTemp,64,"stLetOutCartoonCmd:%d",dwCartoonID);
//	szTemp[63] = 0;
//	GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
//#endif
}

void CCartoonPet::AdoptCartoon(DWORD dwCartoonID)	//领养宠物
{	
	BYTE byState = GetScene()->GetWaitCartoonPetList()->GetCartoonPetState(dwCartoonID);
	if( byState == CARTOON_STATE_WAITING )
	{
		stAdoptCartoonCmd cmd;
		cmd.cartoonID = dwCartoonID;
		SEND_USER_CMD( cmd );
//#ifdef _DEBUG
//		//char szTemp[64];
//		//_snprintf(szTemp,64,"stAdoptCartoonCmd:%d",dwCartoonID);
//		//szTemp[63] = 0;
//		//GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
//
//		//CWaitingCartoonPetList::t_WaitingCartoonPet* pWc = GetScene()->GetWaitCartoonPetList()->GetCartoonPet(dwCartoonID);
//		//if( pWc )
//		//{
//		//	pWc->state = CARTOON_STATE_ADOPTED;
//		//}
//		////刷新列表
//		//if ( GetGameGuiManager()->m_guiAdoptCartoonDlg ) 
//		//{
//		//	GetGameGuiManager()->m_guiAdoptCartoonDlg->UpdateCartoonPetList();
//		//}
//#endif
	}
}

void CCartoonPet::ReturnCartoon(DWORD dwCartoonID)					//交还宠物
{
	BYTE byState = GetScene()->GetWaitCartoonPetList()->GetCartoonPetState(dwCartoonID);
	if( byState == CARTOON_STATE_ADOPTED )
	{
		stReturnCartoonCmd cmd;
		cmd.cartoonID = dwCartoonID;
		SEND_USER_CMD( cmd );
	//#ifdef _DEBUG
	//	char szTemp[64];
	//	_snprintf(szTemp,64,"stReturnCartoonCmd:%d",dwCartoonID);
	//	szTemp[63] = 0;
	//	GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
	//#endif
	}
}

void CCartoonPet::GetBackCartoon()
{
	stGetBackCartoonCmd cmd;
	SEND_USER_CMD( cmd );
//#ifdef _DEBUG
//	GetGameGuiManager()->AddClientSystemMessage("stGetBackCartoonCmd",COLOR_ARGB(255,0,255,0));
//#endif
}

void CCartoonPet::DrawExpCartoon(DWORD dwCartoonID)	//提取经验值
{
	stDrawExpCartoonCmd cmd;
	cmd.cartoonID = dwCartoonID;
	SEND_USER_CMD( cmd );
//#ifdef _DEBUG
//	char szTemp[64];
//	_snprintf(szTemp,64,"stDrawExpCartoonCmd:%d",dwCartoonID);
//	szTemp[63] = 0;
//	GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
//#endif
}

void CCartoonPet::RequestWaitCartoonList()			//请求领养宠物列表
{
	stRequestListCartoonCmd cmd;
	SEND_USER_CMD( cmd );
//#ifdef _DEBUG
//	GetGameGuiManager()->AddClientSystemMessage("stRequestListCartoonCmd",COLOR_ARGB(255,0,255,0));
//#endif
}

void CCartoonPet::AddCent(DWORD dwCartoonID,DWORD time)			//给宠物充值
{
	stChargeCartoonCmd cmd;
	cmd.cartoonID = dwCartoonID;
	cmd.time = time;
	SEND_USER_CMD( cmd );
//#ifdef _DEBUG
//	char szTemp[64];
//	_snprintf(szTemp,64,"stChargeCartoonCmd: [id:%d][time:%d]",dwCartoonID,time);
//	szTemp[63] = 0;
//	GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
//#endif
}

void CCartoonPet::AutoRepair(DWORD dwCartoonID,BYTE repair)		//自动修理 开启/关
{
	stRepairCartoonCmd cmd;
	cmd.cartoonID = dwCartoonID;
	cmd.repair = repair;
	SEND_USER_CMD( cmd );
//
//#ifdef _DEBUG	
//	AddClientSystemMessageF("stRepairCartoonCmd: [id:%d][repair:%d]",dwCartoonID,repair);
//#endif
}

bool CCartoonPet::IsCanFollow()		//可以跟随
{
	bool bHaveFollow = GetScene()->GetCartoonPetList()->IsHaveFollowCartoonPet();
	return ( !bHaveFollow && ( m_data.state == CARTOON_STATE_PUTAWAY ) );
}

bool CCartoonPet::IsCanPickup()		//可以收起
{
	return m_data.state == CARTOON_STATE_FOLLOW;
}

bool CCartoonPet::IsCanStartTraining()	//可以开始修炼
{
	bool bHaveLetout = GetScene()->GetCartoonPetList()->IsHaveLetoutCartoonPet();
	return ( !bHaveLetout  && ( m_data.state == CARTOON_STATE_PUTAWAY && m_data.time > 0 ));
}

bool CCartoonPet::IsCanEndTraining()	//可以终止修炼
{
	return (m_data.state == CARTOON_STATE_ADOPTED || m_data.state == CARTOON_STATE_WAITING ); 
}

bool CCartoonPet::IsCanFeed()			//可以喂养
{
	return false;
}

bool CCartoonPet::IsCanAddCent()		//可以充值
{
	return (m_data.state != CARTOON_STATE_ADOPTED && m_data.state != CARTOON_STATE_WAITING ); 
}

bool CCartoonPet::IsCanFetchExp()		//可以提取经验值
{
	return (m_data.state != CARTOON_STATE_ADOPTED && m_data.state != CARTOON_STATE_WAITING && m_data.addExp > 0 ); 
}

bool CCartoonPet::IsCanStartAutoRepair()	//可以自动修理
{
	return (m_data.state == CARTOON_STATE_FOLLOW && m_data.repair == 0);
}

bool CCartoonPet::IsCanEndAutoRepari()		//可以终止自动修理
{
	return (m_data.state == CARTOON_STATE_FOLLOW && m_data.repair == 1);
}

const char* CCartoonPet::CartoonState2String(BYTE state,CCartoonPet* pCartoon)	//宠物状态转换为字符串
{
	static char CartoonState[][32] = {"收起","跟随主人","等待领养","被领养"};
	if( state < 4 )
	{
		if(state == 3 && pCartoon )
		{
			_snprintf(CartoonState[3],32,"被领养 (%s)",pCartoon->GetData().adopter);
			CartoonState[3][31] = 0;
			return CartoonState[3];
		}
		return CartoonState[state];
	}
	return NULL;
}

const char* CCartoonPet::CartoonState2String(CCartoonPet* pCartoon)
{
	if(pCartoon)
		return CCartoonPet::CartoonState2String(pCartoon->GetState(),pCartoon);
	return NULL;
}


CCartoonPetList::CCartoonPetList()
{
}

CCartoonPetList::~CCartoonPetList() 
{
	Release();
}

void CCartoonPetList::Release() 
{
	FreePointerList<tListCartoonPet>(m_listCartoon);
}

void CCartoonPetList::AddCartoon(stAddCartoonCmd* cmd)		//添加宠物
{
	tListCartoonPetIt it = GetCartoonPetIt( cmd->cartoonID );
	if( it != m_listCartoon.end() ) 
	{
		CCartoonPet* pCartoon = (*it);
		pCartoon->SetAttribute( cmd );
	}
	else
	{
		CCartoonPet* pCartoon = new CCartoonPet();
		pCartoon->SetAttribute( cmd );
		m_listCartoon.push_back( pCartoon );
	}

	if( GetGameGuiManager()->m_guiPetDlg )
	{
		GetGameGuiManager()->m_guiPetDlg->OnUpdateCartoonPet( cmd->cartoonID );
	}

	if( GetGameGuiManager()->m_guiPetPackDlg )
	{
		GetGameGuiManager()->m_guiPetPackDlg->UpdateCanUseCells();
	}

//#ifdef _DEBUG
//	char szTemp[512];	
//	_snprintf(szTemp,512,"AddCartoon [isMine:%s][id:%d] [name:%s] [npc:%d] [master id:%d,name:%s] [lv:%d] [exp:%d,addexp:%d] [state:%d] [adopter:%d] [time:%d] \n",
//		(cmd->isMine)?"true":"false",
//		cmd->cartoonID,
//		cmd->data.name,
//		cmd->data.npcID,
//		cmd->data.masterID,
//		cmd->data.masterName,
//		cmd->data.lv,
//		cmd->data.exp,
//		cmd->data.addExp,
//		cmd->data.state,
//		cmd->data.adopter,
//		cmd->data.time);
//	szTemp[511] = 0;
//	TRACE(szTemp);
//	GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
//#endif
}

void CCartoonPetList::RemoveCartoon(stRemoveCartoonCmd* cmd)	
{
//#ifdef _DEBUG
//	char szTemp[64];
//	_snprintf(szTemp,64,"RemoveCartoon:%d",cmd->cartoonID);
//	szTemp[63] = 0;
//	TRACE(szTemp);
//	GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
//#endif
	tListCartoonPetIt it = GetCartoonPetIt( cmd->cartoonID );
	if( it != m_listCartoon.end() ) 
	{
		if( GetGameGuiManager()->m_guiPetDlg )
		{
			GetGameGuiManager()->m_guiPetDlg->OnRemoveCartoonPet( cmd->cartoonID );
		}
		CCartoonPet* pCartoon = (*it);
		delete pCartoon;
		m_listCartoon.erase( it );
	}
	if( GetGameGuiManager()->m_guiPetDlg )
	{
		GetGameGuiManager()->m_guiPetDlg->OnUpdateCartoonPet( -1 );
	}
	if( GetGameGuiManager()->m_guiPetPackDlg )
	{
		GetGameGuiManager()->m_guiPetPackDlg->UpdateCanUseCells();
	}
}

void CCartoonPetList::UpdateCartoonPet(stHpExpCartoonCmd* cmd)
{
	tListCartoonPetIt it = GetCartoonPetIt( cmd->cartoonID );
	if( it != m_listCartoon.end() ) 
	{
		CCartoonPet* pCartoon = (*it);
		pCartoon->Update( cmd );
		if( GetGameGuiManager()->m_guiPetDlg )
		{
			GetGameGuiManager()->m_guiPetDlg->OnUpdateCartoonPet( cmd->cartoonID );
		}
//#ifdef _DEBUG
//		char szTemp[64];
//		_snprintf(szTemp,64,"stHpExpCartoonCmd:%d [sp:%d][exp:%d]",cmd->cartoonID,cmd->sp,cmd->exp);
//		szTemp[63] = 0;
//		TRACE(szTemp);
//		GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
//#endif
		return ;
	}
//#ifdef _DEBUG
//	char szTemp[64];
//	_snprintf(szTemp,64,"stHpExpCartoonCmd:%d [sp:%d][exp:%d] not find this id",cmd->cartoonID,cmd->sp,cmd->exp);
//	szTemp[63] = 0;
//	TRACE(szTemp);
//	GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
//#endif
}

CCartoonPet* CCartoonPetList::GetCartoon( DWORD dwCartoonID )
{
	tListCartoonPetIt it = GetCartoonPetIt( dwCartoonID );
	if( it != m_listCartoon.end() ) 
	{
		return (*it);
	}
	return NULL;
}

CCartoonPet* CCartoonPetList::GetCartoonByNpcID( DWORD dwNpcID )
{
	Debug_Int(dwNpcID);
	for( tListCartoonPetIt it = m_listCartoon.begin(); it != m_listCartoon.end(); ++it )
	{
		Debug_Int((*it)->GetData().npcID);
		if( (*it)->GetData().npcID == dwNpcID )
		{
			return (*it);
		}
	}
	return NULL;
}

void CCartoonPetList::Rename( DWORD dwCartoonID,const char* newname )
{
	CCartoonPet* pCartoon = GetCartoon( dwCartoonID );
	if( pCartoon )
	{
		pCartoon->Rename( newname );
	}
}

CCartoonPet* CCartoonPetList::operator [](int index)
{
	if( index >= m_listCartoon.size() || index < 0 )
	{
		return NULL;
	}
	return (CCartoonPet*)(m_listCartoon[index]);
}

CCartoonPet* CCartoonPetList::GetAt(int pos)
{
	if( pos >= m_listCartoon.size() || pos < 0 )
	{
		return NULL;
	}
	return m_listCartoon[pos];
}

bool CCartoonPetList::IsHaveLetoutCartoonPet()		//是否有开始修炼的宠物
{
	for( tListCartoonPetIt it = m_listCartoon.begin(); it != m_listCartoon.end(); ++it )
	{
		CCartoonPet* pCartoon = (*it);
		if( pCartoon->GetState() == CARTOON_STATE_WAITING ||
			pCartoon->GetState() == CARTOON_STATE_ADOPTED )
		{
			return true;
		}
	}
	return false;
}

bool CCartoonPetList::IsHaveFollowCartoonPet()	//是否有跟随的宠物
{
	for( tListCartoonPetIt it = m_listCartoon.begin(); it != m_listCartoon.end(); ++it )
	{
		CCartoonPet* pCartoon = (*it);
		if( pCartoon->GetState() == CARTOON_STATE_FOLLOW )
		{
			return true;
		}
	}
	return false;
}

bool CCartoonPetList::IsHaveAdoptedCartoonPet()		//是否有被领养的宠物
{
	for( tListCartoonPetIt it = m_listCartoon.begin(); it != m_listCartoon.end(); ++it )
	{
		CCartoonPet* pCartoon = (*it);
		if( pCartoon->GetState() == CARTOON_STATE_ADOPTED )
		{
			return true;
		}
	}
	return false;
}

CCartoonPetList::tListCartoonPetIt	CCartoonPetList::GetCartoonPetIt( DWORD dwCartoonID )
{
	for( tListCartoonPetIt it = m_listCartoon.begin(); it != m_listCartoon.end(); ++it )
	{
		if( (*it)->GetCartoonID() == dwCartoonID ) return it;
	}
	return m_listCartoon.end();
}

int  CCartoonPetList::GetCanUseCellsNum()	//返回替身宝宝包裹可用的格子数
{
	int iNum = 0;
	for( tListCartoonPetIt it = m_listCartoon.begin(); it != m_listCartoon.end(); ++it )
	{
		CCartoonPet* pCartoon = (*it);
		iNum += pCartoon->GetData().lv;
	}
	int num = m_listCartoon.size() + iNum/10;
	return num;
}

#ifdef _DEBUG
void CCartoonPetList::TraceList()
{
	char szTemp[512];
	for( tListCartoonPetIt it = m_listCartoon.begin(); it != m_listCartoon.end(); ++it )
	{
		CCartoonPet* pCartoon = (*it);
		const char* psz = CCartoonPet::CartoonState2String(pCartoon);
		_snprintf(szTemp,512,"[id:%d] [name:%s] [npc:%d] [master id:%d,name:%s] [lv:%d] [exp:%d,addexp:%d] [state:%s] [adopter:%d] [time:%d] \n",
			pCartoon->GetCartoonID(),
			pCartoon->GetName(),
			pCartoon->GetData().npcID,
			pCartoon->GetData().masterID,
			pCartoon->GetData().masterName,
			pCartoon->GetData().lv,
			pCartoon->GetData().exp,
			pCartoon->GetData().addExp,
			psz,
			pCartoon->GetData().adopter,
			pCartoon->GetData().time);
		szTemp[511] = 0;
		TRACE(szTemp);
		GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
	}
}
#endif


//等带领养的宠物列表
CWaitingCartoonPetList::CWaitingCartoonPetList()
{
}

CWaitingCartoonPetList::~CWaitingCartoonPetList()
{
	Release();
}

void CWaitingCartoonPetList::Release()
{
	FreePointerList<tListWaitingCartoonPet>(m_listWaitingCartoonPet);
}

CWaitingCartoonPetList::t_WaitingCartoonPet* CWaitingCartoonPetList::GetCartoonPet( DWORD dwCartoonID )
{
	tListWaitingCartoonPetIt it = GetCartoonPetIt( dwCartoonID );
	if( it != m_listWaitingCartoonPet.end() )
	{
		return (*it);
	}
	return NULL;
}

void CWaitingCartoonPetList::AddCartoonPet(stAddWaitingCartoonCmd* cmd)
{
	t_WaitingCartoonPet* pWcp = GetCartoonPet( cmd->cartoonID );
	if( NULL == pWcp )
	{
		pWcp = new t_WaitingCartoonPet;
		m_listWaitingCartoonPet.push_back(pWcp);
	}
	pWcp->cartoonID = cmd->cartoonID;
	strncpy(pWcp->masterName,cmd->masterName,sizeof(pWcp->masterName));
	strncpy(pWcp->name,cmd->name,sizeof(pWcp->name));
	pWcp->state = cmd->state;
	pWcp->time = cmd->time;
	pWcp->pNpcBase = GetNpcBase( cmd->npcID );

	//刷新列表
	if ( GetGameGuiManager()->m_guiAdoptCartoonDlg ) 
	{
		GetGameGuiManager()->m_guiAdoptCartoonDlg->UpdateCartoonPetList();
	}

//#ifdef _DEBUG
//	char szTemp[MAX_PATH];
//	const char* psz = CCartoonPet::CartoonState2String(cmd->state);
//	_snprintf(szTemp,MAX_PATH,"stAddWaitingCartoonCmd:[id:%d,npcid:%d] [name:%s] [master name:%s] [state:%s] [time:%d] [pNpcBase:%d]\n",
//		cmd->cartoonID,
//		cmd->npcID,
//		cmd->name,
//		cmd->masterName,
//		psz,
//		cmd->time,
//		pWcp->pNpcBase);
//	szTemp[MAX_PATH - 1] = 0;
//	TRACE(szTemp);
//	GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
//
//	if( false == (( cmd->state == CARTOON_STATE_WAITING) || ( cmd->state == CARTOON_STATE_ADOPTED) ) )
//	{
//		char szTemp[MAX_PATH];
//		const char* psz = CCartoonPet::CartoonState2String(cmd->state);
//		_snprintf(szTemp,MAX_PATH,"宠物的状态设置错误 状态:%s\n",psz );
//		szTemp[MAX_PATH-1] = 0;
//		AssertFatal(0,szTemp);
//	}
//#endif
}

CWaitingCartoonPetList::tListWaitingCartoonPetIt CWaitingCartoonPetList::GetCartoonPetIt( DWORD dwCartoonID )
{
	for( tListWaitingCartoonPetIt it = m_listWaitingCartoonPet.begin(); it != m_listWaitingCartoonPet.end(); ++it )
	{
		if( (*it)->cartoonID == dwCartoonID ) return it;
	}
	return m_listWaitingCartoonPet.end();
}

BYTE	CWaitingCartoonPetList::GetCartoonPetState( DWORD dwCartoonID )
{
	t_WaitingCartoonPet* pCartoonPet = GetCartoonPet( dwCartoonID );
	if( NULL != pCartoonPet )
	{
		return pCartoonPet->state;
	}
	return -1;
}

bool	CWaitingCartoonPetList::IsHaveAdoptedCartoonPet()
{
	for( tListWaitingCartoonPetIt it = m_listWaitingCartoonPet.begin(); it != m_listWaitingCartoonPet.end(); ++it )
	{
		if( (*it)->state == CARTOON_STATE_ADOPTED )
		{
			return true;
		}
	}
	return false;
}

bool    CWaitingCartoonPetList::IsCanAdoptCartoonPet()
{
	int num = 0;
	for( tListWaitingCartoonPetIt it = m_listWaitingCartoonPet.begin(); it != m_listWaitingCartoonPet.end(); ++it )
	{
		if( (*it)->state == CARTOON_STATE_ADOPTED )
		{
			num ++;
			//return true;
		}
	}
	return  num < 5 ;	
}

void CWaitingCartoonPetList::RemoveCartoonPet(stRemoveWaitingCartoonCmd* cmd)
{
	tListWaitingCartoonPetIt it = GetCartoonPetIt( cmd->cartoonID );
	if( it != m_listWaitingCartoonPet.end() )
	{
		t_WaitingCartoonPet* pWcp = (*it);
		delete pWcp;
		m_listWaitingCartoonPet.erase( it );
	}

	//刷新列表
	if ( GetGameGuiManager()->m_guiAdoptCartoonDlg ) 
	{
		GetGameGuiManager()->m_guiAdoptCartoonDlg->UpdateCartoonPetList();
	}

//#ifdef _DEBUG
//	char szTemp[MAX_PATH];
//	_snprintf(szTemp,MAX_PATH,"stRemoveWaitingCartoonCmd:[id:%d] \n",cmd->cartoonID);
//	szTemp[MAX_PATH - 1] = 0;
//	GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
//#endif
}


#ifdef _DEBUG
void CWaitingCartoonPetList::TraceList()
{
	char szTemp[MAX_PATH];
	for( tListWaitingCartoonPetIt it = m_listWaitingCartoonPet.begin(); it != m_listWaitingCartoonPet.end(); ++it )
	{
		t_WaitingCartoonPet* pWcp = (*it);
		const char* psz = CCartoonPet::CartoonState2String(pWcp->state);
		_snprintf(szTemp,MAX_PATH,"[id:%d] [name:%s] [master name:%s] [state:%s] [time:%d] \n",
			pWcp->cartoonID,
			pWcp->name,
			pWcp->masterName,
			psz,
			pWcp->time);
		szTemp[MAX_PATH - 1] = 0;
		TRACE(szTemp);
		GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
	}
}
#endif
