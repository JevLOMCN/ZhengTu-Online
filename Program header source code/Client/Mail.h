#pragma once

#include "./command.h"

struct stMailTitle
{
	DWORD id;
	BYTE state;	//1 未读 2 读
	char fromName[MAX_NAMESIZE];
	bool accessory;
	DWORD endTime;
	BYTE type; //0:普通 1：系统
};
typedef std::vector<stMailTitle> tMailList;

class CMailManager
{
public:
	CMailManager(void);
	~CMailManager(void);

	void RequestMailIitle();
	void OpenMail(DWORD dwMailID);
	bool ParseMailTitle(stAddListMail* cmd);
	bool ParseMailContent(stContentMail* cmd);
	bool DeleteMailItem(DWORD dwMailID);
	bool RecvMailItem(DWORD dwMailID);
	bool DeleteMail(DWORD dwMailID);
	void SetNewMailNotify(bool bIsNewMail = true);
	void OnRemoveMail(DWORD dwMailID);
	void SendBackMail(DWORD dwMailId);

	bool IsSystemMail(DWORD dwMailID);

	tMailList::iterator GetMailTitle(DWORD dwMailId);

	const char* GetMailFromName(DWORD dwMailId);
	BYTE GetMailState(DWORD dwMailId);

	tMailList* GetMailList() { return &m_listMail; }

	bool bIsHaveNewMail() { return m_bIsHaveNewMail; }
	void SortMailListByEndTime();
#ifdef _DEBUG
	DWORD GetMailTest();
	void OpenMailByIndex(int index);
	void ShowMailList();
#endif
private:
	tMailList	m_listMail;
	bool		m_bIsHaveNewMail;//是否有新邮件
};
