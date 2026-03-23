#pragma once

enum enumDefaultChatTextColor
{
	Default_Color_Nine = COLOR_ARGB(255,255,255,255),
	Default_Color_Country = COLOR_ARGB(255,255,209,0),
	Default_Color_Erea = COLOR_ARGB(255,0,255,0),
	Default_Color_Whisper = COLOR_ARGB(255,255,72,24),
	Default_Color_Union = COLOR_ARGB(255,52,255,187),
	Default_Color_Team = COLOR_ARGB(255,152,244,23),
	Default_Color_Family = COLOR_ARGB(255,255,98,124),
	Default_Color_Private = COLOR_ARGB(255,0,127,255),
	Default_Color_Friend = COLOR_ARGB(255,0,127,255),
	Default_Color_School = COLOR_ARGB(255,255,180,255),
	Default_Color_World = COLOR_ARGB(255,255,240,160),
};



struct tChatUser
{
	std::string user;
	std::string	aliasName;

	tChatUser()
	{
		user = "";
		aliasName = "";
	}

	tChatUser(const char* name,const char* alias)
	{
		user =  name;
		aliasName = alias;
	}
};

class CGuiChatDialog;
class CGuiButton;
class CGuiEditBox;

class CChatChannel
{
public:
	typedef std::vector<tChatUser>			tChannelMemberList;
	typedef tChannelMemberList::iterator	tChannelMemberListIt;

	typedef std::vector<TiXmlElement*>		tChatMessageList;
	typedef tChatMessageList::iterator		tChatMessageListIt;

public:
	CChatChannel();
	CChatChannel(DWORD dwChatType);
	virtual ~CChatChannel();

	void	AddMember(const char* name,const char* alias = "");
	void	RemoveMember(const char* name);
	bool	IsUserExist(const char* name);
	void	InviteUser(const char* user);
	void	UpdateUserList();
	tChatUser&  GetChatUser(int index);
	const char* GetMember(int index);
	size_t  GetMemberCount(){return m_members.size();}

	virtual void	SetChannelID(DWORD id);
	DWORD	GetChannelID()			 { return m_id; }

	const char* GetChannelFileName() { return m_szFileName;	}

	void	SetChatDialog(CGuiChatDialog* pDialog){m_pChatDialog = pDialog;}

	void	CopyMessage(CChatChannel* pChannel);
	void	AddMsg(TiXmlElement* page);
	void	SendChatMsg(CGuiEditBox* pEditChat);
	size_t  GetMessageCount(){	return m_Msgs.size();}

	//
	//显示聊天对话框
	void	ShowChatDialog();
	void	HideChatDialog();

	//关闭频道
	void	CloseChannel();

	bool IsMine(){	return m_bMine;	}
	void SetMine( bool bMine ){	m_bMine = bMine;}

	bool IsPrivateChat();
	DWORD	GetChatType() { return m_dwChatType; }

	bool GetForbidden() { return m_bForbid; }
	void SetForbid(bool forbid) { m_bForbid = forbid; }

	bool IsChatDialogClosed();

#ifdef _DEBUG
	void	Trace();
#endif
protected:
	void	SaveMessage2Vector(TiXmlElement* pPage);
	void	WriteMsg2File(const char* pszFileName);
	void	ShowAllMessage();
	void	ReleaseMessage();

	void CloseDialog();
	CGuiChatDialog* AddPrivateChatDialog();
	CGuiChatDialog* AddFamilyChatDialog();
	CGuiChatDialog* AddUnionChatDialog();
protected:
	DWORD				m_dwChatType;
	CGuiChatDialog*		m_pChatDialog;
	DWORD				m_id;
	tChannelMemberList	m_members;
	tChatMessageList	m_Msgs;
	char				m_szFileName[MAX_PATH];
	bool				m_bMine;			//是否自己创建的聊天频道
	bool				m_bForbid;			//禁止接收消息
};


class CPrivateChatChannel: public CChatChannel
{
public:
	CPrivateChatChannel(const char* user);

	void SetChannelID(DWORD id)
	{
		m_id = 0;		
	}
};


struct tHideChatButton
{
	CGuiButton*		pButton;
	CChatChannel*	pChannel;

	tHideChatButton()
	{
		pButton		= NULL;
		pChannel	= NULL;
	}
};

struct tChatInvite
{
	DWORD			dwChannelID;
	DWORD			dwCharType;
	float			fKeepTime;
	float			fCurrTime;
	char			name[MAX_NAMESIZE];
	CChatChannel*	pChannel; //标志是邀请(NULL)还是消息提示

	tChatInvite()
	{
		dwChannelID = 0; 
		dwCharType = 1;
		pChannel = NULL;
		memset(name,0,MAX_NAMESIZE);
		fKeepTime = 0.0f;
		fCurrTime = 0.0f;
	}
};

class CGuiChatInvite;
class CGuiUserListDialog;

class CChatManager
{
public:
	typedef std::vector<CChatChannel*>		tListChatChannel;
	typedef tListChatChannel::iterator		tListChatChannelIt;

	typedef std::vector<tHideChatButton>	tListHideChatButton;
	typedef tListHideChatButton::iterator	tListHideChatButtonIt;

	typedef std::vector<tChatInvite*>		tListChatInvite;
	typedef tListChatInvite::iterator		tListChatInviteIt;

	typedef std::vector<UINT>				tListRemovedButtonId;
public:
	CChatManager();
	~CChatManager();

	//打开邀请对话框
	CGuiChatInvite* AddInviteDialog(tChatInvite* pInvite);

	//响应社会关系按钮,如果有没有显示过的消息,则打开对应的对话框显示之
	bool OnOpenChatDialog();

	//响应主界面上 IDC_PRIVATECHAT_BTN_BEG ~~ IDC_PRIVATECHAT_BTN_END 按钮事件
	bool OnReOpenChatDialog(UINT nID);

	//响应聊天对话框最小化按钮
	bool OnHideChatDialog(CChatChannel* pChannel);

	//添加私聊对话
	CChatChannel* GetPrivateChatChannel(const char* name);

	//直接跟玩家对话
	void PrivateChatWith(const char* name);

	//刷新社会关系按钮的状态
	void UpdateChatTipsButtonState();		

	//刷新隐藏对话框按钮的位置
	void UpdateHideChatButtonState();

	//
	bool OnAddFamilyChat();		//家族聊天
	bool OnAddUnionChat();		//帮会聊天

	void ShowChatDialogAllwith(DWORD id,const char* name);
	void ShowChannelDialog(DWORD id);
	void RemoveChannel(DWORD id);
	void RemoveChannel(CChatChannel* pChannel);
	void RemoveAllChannels();

	void OnChat(const char* name,CChatChannel* pChannel);

	//删除邀请
	void RemoveInvite(CChatChannel* pChannel);
	void RemoveInvite(tChatInvite*	pInvite);
	void RemoveAllInvite();		//删除邀请

	//在CGuiMain中的OnRender被调用,用以对所有邀请的保留时间进行刷新
	void UpdateAllInvite(float fElapsedTime);	

	//服务器消息处理函数
	void CreateChatChannel(stCreateChannelUserCmd* pCmd);	// 创建一个私聊频道
	void InviteChatChannel(stInvite_ChannelUserCmd* pCmd);	// 通知聊天者的私聊频道
	void JoinChatChannel(stJoin_ChannelUserCmd* pCmd);		// 加入一个私聊频道
	void LeaveChatChannel(stLeave_ChannelUserCmd* pCmd);	// 删除一个私聊频道中的聊天者
	bool AddPersonChatMessage(stChannelChatUserCmd* pCmd);	// 相应 CHAT_TYPE_PERSON 类型消息

	void	AddMessageTips(DWORD dwChannel,DWORD charType,CChatChannel* pChannel,const char* name);
	bool	IsChatDialogMinsized(CChatChannel* pChannel);

	void	HideChannelMinizedButton(CChatChannel* pChannel);

	
	static bool SendChatCmd(CGuiEditBox* pEditChat,DWORD id,DWORD type,const char* name);
	static void SetChatTitle(const stChannelChatUserCmd* & pCmd,TiXmlElement* pPage,DWORD dwColor=-1);
	static void FilterChatMessage(stChannelChatUserCmd* pCmd);

	static void FormatMsgTitle(const char* name,DWORD color,TiXmlElement* pPage);
	static void FormatMsgContent2XML(stChannelChatUserCmd* pCmd,TiXmlElement* pPage);
	static void FormatItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage);
	static void Format2ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage);
	static void Format3ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage);
	static void Format4ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage);
	static void Format5ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage);
	static void Format7ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage);
	static void Format8ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage);
	static void Format9ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage);
	static void Format10ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage);
	static void Format11ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage);
	static void Format12ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage);
	static void FormatCommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage);
	static void Format2CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage);
	static void Format3CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage);
	static void Format4CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage);
	static void Format5CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage);
	static void Format7CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage);
	static void Format8CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage);
	static void Format9CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage);
	static void Format10CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage);
	static void Format11CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage);
	static void Format12CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage);

#ifdef _DEBUG
	void	Trace();
#endif
	
	int GetHideButtonNum(){
		return m_HideBtns.size();
	}
private:
	bool	AddHideChatButton(CChatChannel* pChannel);
	UINT	GetUnUseButtonID();
	void	DeleteIdFromRemovedList(UINT id);
	
	
	CChatChannel*			GetOrAddChannelByID(DWORD id);
	CChatChannel*			GetOrAddFamilyChannel();
	CChatChannel*			GetOrAddUnionChannel();

	//
	CChatChannel*			FindChannelByID(DWORD id);
	CPrivateChatChannel*	FindPrivateChannelByName(const char* user);

	//
	tListChatInviteIt		FindInviteIt(DWORD channel,DWORD charType);
	tListChatInviteIt		FindInviteIt(CChatChannel* pChannel);
	tListChatInviteIt		FindInviteIt(tChatInvite*  pInvite);

	tListChatChannelIt		FindChannelItByID(DWORD id);
	tListChatChannelIt		FindChannelItByChannel(CChatChannel* pChannel);

	tListHideChatButtonIt	FindHideButtonItByButtonID(DWORD id);
	tListHideChatButtonIt	FindHideButtonItByChannelID(DWORD id);
	tListHideChatButtonIt	FindHideButtonByChannel(CChatChannel* pChannel);
private:
	tListChatChannel		m_Channels;		//聊天频道
	tListHideChatButton		m_HideBtns;		//聊天窗口最小化按钮
	tListRemovedButtonId	m_RemovedIDs;	//已经关闭的最小化按钮id列表
	tListChatInvite			m_Invites;		//聊天邀请

	
	CChatChannel*			m_pFamilyChannel;	//家族频道
	CChatChannel*			m_pUinonChannel;	//帮会频道
};

inline CChatManager& GetChatManager()
{
	static CChatManager cm;return cm;
}

const char* GetChatTypeName(DWORD dwType);
DWORD GetChatColor(DWORD dwType);


stPointI GetHideChatButtonPos(int i);