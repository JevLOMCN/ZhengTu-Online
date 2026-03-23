#include "public.h"
#include ".\mail.h"
#include "./GameGuiManager.h"
#include "./GuiRecvMailDialog.h"

CMailManager::CMailManager(void)
{
	m_bIsHaveNewMail = false;
}

CMailManager::~CMailManager(void)
{
}

void CMailManager::RequestMailIitle()
{
	stGetListMail cmd;
	SEND_USER_CMD(cmd);
}

bool CMailManager::ParseMailTitle(stAddListMail* cmd)
{
	stMailTitle mt;
	mt.id = cmd->id;
	mt.state = cmd->state;
	strcpy(mt.fromName,cmd->fromName);
	Debug_String(mt.fromName);
	mt.accessory = cmd->accessory;
	mt.endTime = cmd->endTime;
	mt.type = cmd->type;

//#ifdef _DEBUG
//	AddClientSystemMessageF("%s %d %s\n",mt.fromName,mt.id,(mt.type==0)?"普通":"系统");
//#endif

	bool bIsIdExist = false;
	for( tMailList::iterator it = m_listMail.begin(); it != m_listMail.end(); ++it )
	{
		if( it->id == mt.id )
		{
			bIsIdExist = true;
			break;
		}
	}
	if( !bIsIdExist )
	{
		m_listMail.push_back( mt );
	}
	
	SortMailListByEndTime();

	if( GetGameGuiManager()->m_guiRecvMailDlg )
	{
		GetGameGuiManager()->m_guiRecvMailDlg->UpdateMailList();
	}	

	return true;
}

void CMailManager::SendBackMail(DWORD dwMailId)
{
	stTurnBackMail cmd;
	cmd.mailID = dwMailId;
	SEND_USER_CMD(cmd);
}

bool CMailManager::ParseMailContent(stContentMail* cmd)
{
	if( NULL == cmd ) return false;

	for( tMailList::iterator it = m_listMail.begin(); it != m_listMail.end(); ++it )
	{
		if( it->id == cmd->mailID )
		{
			it->state = 2;
		}
	}
	if( GetGameGuiManager()->m_guiRecvMailDlg )
	{
		GetGameGuiManager()->m_guiRecvMailDlg->ParseMailContent(cmd);
	}

//#ifdef _DEBUG
//	char Msg[512];
//	_snprintf(Msg,512,"stContentMail:[mailID:%d][title:%s][accessory:%d][text:%s][sendMoney:%d][recvMoney:%d][Item:%s,%d]",
//		cmd->mailID,cmd->title,cmd->accessory,cmd->text,cmd->sendMoney,cmd->recvMoney,cmd->item.strName,cmd->item.dwNum);
//	GetGameGuiManager()->AddClientSystemMessage(Msg);
//	Engine_WriteLog(Msg);
//#endif

	return true;
}


void CMailManager::OpenMail(DWORD dwMailID)
{
	stOpenMail cmd;
	cmd.mailID = dwMailID;
	SEND_USER_CMD(cmd);

//#ifdef _DEBUG
//	char Msg[MAX_PATH];
//	_snprintf( Msg,MAX_PATH,"Send stOpenMail [mailID:%d]",dwMailID );
//	GetGameGuiManager()->AddClientSystemMessage(Msg);
//	Engine_WriteLog(Msg);
//#endif
}

tMailList::iterator CMailManager::GetMailTitle(DWORD dwMailId)
{
	for( tMailList::iterator it = m_listMail.begin(); it != m_listMail.end(); ++it )
	{
		if( it->id == dwMailId )
		{
			return it;
		}
	}
	return m_listMail.end();
}

bool CMailManager::DeleteMailItem(DWORD dwMailID)
//删除邮件的附件
{
	for( tMailList::iterator it = m_listMail.begin(); it != m_listMail.end(); ++it )
	{
		if( it->id == dwMailID )
		{
			it->accessory = false;
			if( GetGameGuiManager()->m_guiRecvMailDlg )
			{
				GetGameGuiManager()->m_guiRecvMailDlg->UpdateMailList();
			}
		}
	}
//#ifdef _DEBUG
//	char Msg[MAX_PATH];
//	sprintf(Msg,"删除邮件附件:%d\n",dwMailID);
//	Engine_WriteLog(Msg);
//	GetGameGuiManager()->AddClientSystemMessage(Msg);
//#endif
	return true;
}

bool CMailManager::RecvMailItem(DWORD dwMailID)
//收邮件附件
{
	stGetItemMail cmd;
	cmd.mailID = dwMailID;
	SEND_USER_CMD(cmd);
//#ifdef _DEBUG
//	char Msg[MAX_PATH];
//	sprintf(Msg,"收邮件附件:%d\n",dwMailID);
//	Engine_WriteLog(Msg);
//	GetGameGuiManager()->AddClientSystemMessage(Msg);
//#endif
	return true;
}

void CMailManager::OnRemoveMail(DWORD dwMailID)
{
	for( tMailList::iterator it = m_listMail.begin(); it != m_listMail.end(); ++it )
	{
		if( it->id == dwMailID )
		{
			m_listMail.erase( it );

			if( GetGameGuiManager()->m_guiRecvMailDlg )
			{
				GetGameGuiManager()->m_guiRecvMailDlg->UpdateMailList();
			}
			return ;
		}
	}
}

bool CMailManager::DeleteMail(DWORD dwMailID)
//删除邮件
{
	stDelMail cmd;
	cmd.mailID = dwMailID;
	SEND_USER_CMD(cmd);
//#ifdef _DEBUG
//	char Msg[MAX_PATH];
//	sprintf(Msg,"删除邮件:%d\n",dwMailID);
//	Engine_WriteLog(Msg);
//	GetGameGuiManager()->AddClientSystemMessage(Msg);
//#endif
	return true;
}

void CMailManager::SetNewMailNotify(bool bIsNewMail)
{
	Debug_Bool(bIsNewMail);
	m_bIsHaveNewMail = bIsNewMail;
}

const char* CMailManager::GetMailFromName(DWORD dwMailId)
{
	for( tMailList::iterator it = m_listMail.begin(); it != m_listMail.end(); ++it )
	{
		if( it->id == dwMailId )
		{
			return it->fromName;
		}
	}
	return NULL;
}

template <class tRecord>
bool DataLessByEndTime(tRecord& data1,tRecord & data2)
{
	FUNCTION_BEGIN;

	return data1.endTime >= data2.endTime;

	FUNCTION_END;
}

void CMailManager::SortMailListByEndTime()
{
	FUNCTION_BEGIN;
	std::sort(m_listMail.begin(),m_listMail.end(),DataLessByEndTime<stMailTitle>);
	FUNCTION_END;
}

BYTE CMailManager::GetMailState(DWORD dwMailId)
{
	for( tMailList::iterator it = m_listMail.begin(); it != m_listMail.end(); ++it )
	{
		if( it->id == dwMailId )
		{
			return it->state;
		}
	}
	return 0;
}

bool CMailManager::IsSystemMail(DWORD dwMailID)
{
	tMailList::iterator it = GetMailTitle(dwMailID);
	if( it != m_listMail.end() )
	{
		stMailTitle& mt = *it;
		return mt.type == 1;
	}
	return false;
}

#ifdef _DEBUG
DWORD CMailManager::GetMailTest()
{
	if( m_listMail.empty() ) return 0;
	static int i = 0;
	DWORD dwID = m_listMail[i].id;
	if( i < m_listMail.size() ) i ++;
	if( i == m_listMail.size() ) i = 0;
	return dwID;
}

void CMailManager::OpenMailByIndex(int index)
{
	if ( index >= 0 && index < m_listMail.size() )
	{
		OpenMail( m_listMail[index].id );
	}
}

void CMailManager::ShowMailList()
{
	int i = 0;
	for( tMailList::iterator it = m_listMail.begin(); it != m_listMail.end(); ++it )
	{
		char Msg[MAX_PATH];
		sprintf( Msg,"%d. %d,%s,%d,%s",i,it->id,it->fromName,it->endTime,((it->accessory)?"true":"false") );
		Engine_WriteLog(Msg);
		GetGameGuiManager()->AddClientSystemMessage(Msg);
		++i;
	}
}
#endif