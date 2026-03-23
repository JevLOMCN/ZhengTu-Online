#include "public.h"
#include "./Chat.h"
#include ".\character.h"
#include "./Country.h"
#include "./CharacterProperty.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./GuiPrivateChat.h"
#include "./GuiChatInvite.h"
#include "./GuiWorldConn.h"
#include "./GuiMain.h"
#include "./Relation.h"
#include "./GuiChatSelectButton.h"
#include "./GuiPrivateChatSelect.h"
#include "./RoleItem.h"
#include "../engine/include/crc.h"
#include "../engine//include/PhraseFilter.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "./GameAppation.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "../xml_parse/XmlParseEx.h"

const char* g_szChatTypeName[] = {
	"",                            // 0 
		"【私】",                      // 1  
		"【轻】",                      // 2
		"【队】",                      // 3 
		"【好】",                      // 4 
		"【GM】",                      // 5   
		"【系】",                      // 6
		"【帮】",                      // 7
		"【弹】",                      // 8 
		"【个】",                      // 9 
		"【密】",                      // 10
		"【密】",                      // 11
		"【国】",                      // 12
		"【区】",                      // 13  
		"【家】",                      // 14
		"","","","","","", "","","","","",                     // 15 16 17 18 19 20 21 22 23 24 25 
		"【世】",                      // 26
		"【师】",                      // 27
		"","","","","","",                                 // 28 29 30 31 32 33 34 与command的enumChatType下对应
		"【系】",                      // 34

};

const char* GetChatTypeName(DWORD dwType)
{
	FUNCTION_BEGIN;
	if(dwType >= count_of(g_szChatTypeName))
		return g_szChatTypeName[0];
	return g_szChatTypeName[dwType];

	FUNCTION_END;
}


DWORD GetChatColor(DWORD dwType)
{
	FUNCTION_BEGIN;

	stGameConfig config = *((CGameAppation*)Engine_GetAppation())->GetConfig();

	DWORD dwDefaultColor = COLOR_ARGB(255,255,255,255);
	switch(dwType)
	{
	case CHAT_TYPE_PRIVATE:						/// 私聊频道
		return dwDefaultColor;
	case CHAT_TYPE_NINE:						/// 轻聊频道
		return config.serverData.dwChatColor[0];
	case CHAT_TYPE_TEAM:						/// 队伍频道
		return config.serverData.dwChatColor[5];
	case CHAT_TYPE_FRIEND:						/// 好友频道
		return config.serverData.dwChatColor[7];
	case CHAT_TYPE_WHISPER:						/// 悄悄话
		return config.serverData.dwChatColor[3];
	case CHAT_TYPE_WHISPERTOME:                 /// 悄悄话
		return config.serverData.dwChatColor[3];
	case CHAT_TYPE_GM:							/// GM聊频道
		return dwDefaultColor;
	case CHAT_TYPE_SYSTEM:						/// 系统频道
		return COLOR_ARGB(255,250,0,0);
	case CHAT_TYPE_UNION:						/// 帮会频道
		return config.serverData.dwChatColor[4];
	case CHAT_TYPE_POP:							/// 弹出式系统提示
		return dwDefaultColor;
	case CHAT_TYPE_PERSON:						/// 个人频道
		return dwDefaultColor;
	case CHAT_TYPE_COUNTRY:						/// 国家频道
		return config.serverData.dwChatColor[1];
	case CHAT_TYPE_AREA:						/// 地区频道
		return config.serverData.dwChatColor[2];
	case CHAT_TYPE_OVERMAN:						/// 师门频道		//TBD
		return Default_Color_School;
	case CHAT_TYPE_FAMILY:						/// 家族频道
		return config.serverData.dwChatColor[6];
	case CHAT_TYPE_FRIEND_AFFICHE:				/// 好友公告
		return dwDefaultColor;
	case CHAT_TYPE_UNION_AFFICHE:				/// 帮会公告
		return dwDefaultColor;
	case CHAT_TYPE_OVERMAN_AFFICHE:				/// 师门公告
		return dwDefaultColor;
	case CHAT_TYPE_FAMILY_AFFICHE:				/// 家族公告
		return dwDefaultColor;
	case CHAT_TYPE_FRIEND_PRIVATE:				/// 好友私聊
		return dwDefaultColor;
	case CHAT_TYPE_UNION_PRIVATE:				/// 帮会私聊
		return dwDefaultColor;
	case CHAT_TYPE_OVERMAN_PRIVATE:				/// 师门私聊
		return dwDefaultColor;
	case CHAT_TYPE_FAMILY_PRIVATE:				/// 家族私聊
		return dwDefaultColor;
	case CHAT_TYPE_WORLD:						/// 世界频道
		return Default_Color_World;		
	case CHAT_TYPE_AUTO:
		return COLOR_ARGB(255,255,72,24);
	case CHAT_TYPE_SYTEM:						///soke 系统公告频道
		return COLOR_ARGB(255,250,0,0);

	}
	return dwDefaultColor;
	FUNCTION_END;
}



#define IDC_HIDECHAT_BTN_BEG   5000
#define IDC_HIDECHAT_BTN_END   9999

/*------------------- chat manager ------------------*/
CChatChannel::CChatChannel()
{
	FUNCTION_BEGIN;
	SetChannelID(0);
	m_pChatDialog = NULL;
	m_bMine = false;
	m_dwChatType = CHAT_TYPE_PERSON;
	m_bForbid = false;
}

CChatChannel::CChatChannel(DWORD dwChatType)
{
	FUNCTION_BEGIN;
	SetChannelID(0);
	m_pChatDialog = NULL;
	m_bMine = false;
	m_dwChatType = dwChatType;
	m_bForbid = false;
}

CChatChannel::~CChatChannel()
{
	FUNCTION_BEGIN;
	//退出该频道时删除文件
	if( IsFileExist( m_szFileName ) )
	{
		DeleteFile( m_szFileName );
	}
	ReleaseMessage();

	if( m_pChatDialog )
	{
		m_pChatDialog->Close();
		m_pChatDialog = NULL;
	}
	if ( m_id != 0 )		//不是私聊
	{
		//发送关闭聊天频道消息
		stLeave_ChannelUserCmd cmd;
		strncpy(cmd.name,GetMember(0),MAX_NAMESIZE);
		cmd.dwChannelID = m_id;
		SEND_USER_CMD(cmd);
	}

	//
	CGuiChatInvateDialog::OnChannelClosed(this);
	CGuiWorldConn::OnChannelClosed(this);
}

void CChatChannel::SetChannelID(DWORD id)
{
	FUNCTION_BEGIN;
	m_id = id;

	//避免多开时误加入别人的信息
	char szTemp[MAX_PATH];
	_snprintf(szTemp,MAX_PATH,"%s%d",GetScene()->GetMainCharacter()->GetName(),m_id);
	szTemp[MAX_PATH - 1] = 0;

	DWORD crc = calculateCRC(szTemp,strlen(szTemp)+1);
	_snprintf(m_szFileName,MAX_PATH,"data\\chat\\%u_%u.xml",m_dwChatType,crc);
	m_szFileName[MAX_PATH - 1] = 0;

	//如果发现该频道以前创建过文件则删除之
	if( IsFileExist( m_szFileName ) )
	{
		DeleteFile( m_szFileName );
	}	
}

void CChatChannel::AddMember(const char* name,const char* alias)
{
	FUNCTION_BEGIN;

	if( !IsUserExist(name) )
	{
		tChatUser cu(name,alias);
		m_members.push_back(cu);
	}
}


void CChatChannel::InviteUser(const char* user)
{
	FUNCTION_BEGIN;

	if( IsUserExist(user) )
	{
		return ;
	}
	if ( m_id != 0 )
	{
		if( GetMemberCount() < 20 )
		{
			stInvite_ChannelUserCmd cmd;
			cmd.dwChannelID = GetChannelID();
			cmd.dwCharType = GetScene()->GetMainCharacter()->GetType();
			strncpy(cmd.name,user,MAX_NAMESIZE);
			SEND_USER_CMD(cmd);
		}
		else
		{
			GetGameGuiManager()->AddClientSystemMessage("本频道人数已满20人,不能再添加了");
		}
	}
	else
	{
		stCreateChannelUserCmd cmd;
		strncpy(cmd.name,GetMember(1),MAX_NAMESIZE);
		strncpy(cmd.name2,user,MAX_NAMESIZE);
		SEND_USER_CMD(cmd);
	}
}

//删除成员
void CChatChannel::RemoveMember(const char* name)
{
	FUNCTION_BEGIN;
	for ( tChannelMemberListIt it = m_members.begin(); 
		it != m_members.end(); ++it )
	{
		tChatUser& cu = *it;
		if( cu.user == name )
		{
			m_members.erase( it );
			return ;
		}
	}
}

bool CChatChannel::IsUserExist(const char* name)
{
	FUNCTION_BEGIN;
	for ( tChannelMemberListIt it = m_members.begin(); 
		it != m_members.end(); ++it )
	{
		tChatUser& cu = *it;
		if( cu.user == name )
		{			
			return true;
		}
	}
	return false;
}

const char* CChatChannel::GetMember(int index)
{
	return GetChatUser(index).user.c_str();
}

tChatUser&  CChatChannel::GetChatUser(int index)
{
	FUNCTION_BEGIN;
	Assert( index >= 0 && index < m_members.size() );
	return m_members[index];
}

//刷新成员列表
void	CChatChannel::UpdateUserList()
{
	FUNCTION_BEGIN;
	if( m_pChatDialog )
	{
		m_pChatDialog->RemoveAllUser();
		for( size_t i = 0;i < m_members.size() ;++i)
		{
			std::string str = m_members[i].user;
			DWORD color = COLOR_ARGB(255,255,255,255);
			if( m_members[i].aliasName.size() > 1 )
			{
				color = COLOR_ARGB(255,0,255,0);
				//str = str + " ( " + m_members[i].aliasName + " )";
			}
			m_pChatDialog->AddChatUser( str.c_str(),color );
		}
	}
}

//发送聊天信息消息到服务器
void CChatChannel::SendChatMsg(CGuiEditBox* pEditChat)
{
	FUNCTION_BEGIN;

	if( NULL == pEditChat )
	{
		return ;
	}

	const char* pszName = GetScene()->GetMainCharacter()->GetName();
	if ( m_dwChatType != CHAT_TYPE_UNION &&
		 m_dwChatType != CHAT_TYPE_FAMILY )
	{
		//记录自己说的话
		DWORD color = COLOR_ARGB(255,242,140,33);
		TiXmlElement page("p");
		CChatManager::FormatMsgTitle(pszName,color,&page);
		CChatManager::FormatCommMsgContent(pEditChat->GetText(),color,&page);
		AddMsg(&page);
	}

	const char* psz = (m_dwChatType==CHAT_TYPE_PRIVATE)?GetMember(1):pszName;
	CChatManager::SendChatCmd(pEditChat,m_id,m_dwChatType,psz);
}

void CChatChannel::AddMsg(TiXmlElement* page)
{
	FUNCTION_BEGIN;
	SaveMessage2Vector(page);
	if( NULL != m_pChatDialog )
	{
		GetChatManager().RemoveInvite(this);
		if( !m_bForbid )
		{
			m_pChatDialog->AddMessage(page);
		}		
	}
}

void CChatChannel::CopyMessage(CChatChannel* pChannel)
{
	FUNCTION_BEGIN;
	if( NULL == pChannel )
	{
		Assert(0);
		return;
	}
	const char* pszFileName = pChannel->GetChannelFileName();
	if( IsFileExist( pszFileName ) )
	{
		CopyFile(pszFileName,m_szFileName,false);
	}
	pChannel->WriteMsg2File(m_szFileName);
}

void CChatChannel::SaveMessage2Vector(TiXmlElement* pPage)
{
	FUNCTION_BEGIN;
	TiXmlElement* p = new TiXmlElement(*pPage);
	if ( NULL != p )
	{
		m_Msgs.push_back( p );
		if( m_Msgs.size() > 10 )
		{
			WriteMsg2File( m_szFileName );
		}
	}			
}

void CChatChannel::WriteMsg2File(const char* pszFileName)
{
	FUNCTION_BEGIN;

	if(m_Msgs.empty())
	{
		return ;
	}

	TiXmlDocument doc;
	if(!doc.LoadFile(pszFileName))
	{
		doc.InsertEndChild(TiXmlDeclaration("1.0","gb2312",""));
	}
	TiXmlElement* chat = XML_FindAndAddChildNode(&doc,"body");
	if( NULL != chat )
	{
		for ( int i = 0; i < m_Msgs.size(); i++ )
		{
			TiXmlElement* page = m_Msgs[i];
			chat->InsertEndChild( *page );
		}
	}
	if( !doc.SaveFile(pszFileName) )
	{
		Assert(0);
		TRACE("SaveFile:%s error\n",pszFileName);		
	}	
	else
	{
		//保存成功则清空内存中的消息
		ReleaseMessage();
	}	
}

void CChatChannel::ShowAllMessage()
{
	FUNCTION_BEGIN;
	if( m_pChatDialog )
	{
		//先装在文件中的
		m_pChatDialog->LoadChatMsg( m_szFileName );

		//再显示内存中的
		for ( int i = 0; i < m_Msgs.size(); i++ )
		{
			TiXmlElement* page = m_Msgs[i];
			m_pChatDialog->AddMessage(page);
		}

		//把内存中的消息写到文件中
		WriteMsg2File( m_szFileName );		
	}
}

void CChatChannel::ReleaseMessage()
{
	FreePointerList<tChatMessageList>(m_Msgs);
}

void CChatChannel::CloseDialog()
{
	if( m_pChatDialog )
	{
		m_pChatDialog->Close();
		m_pChatDialog = NULL;
	}
}

CGuiChatDialog* CChatChannel::AddPrivateChatDialog()
{
	CloseDialog();

	if( NULL == m_pChatDialog )
	{
		m_pChatDialog = new CGuiPrivateChat(this);
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\PrivateChatDialog.xml",m_pChatDialog);
		if( FAILED( hr ) ) 
		{
			SAFE_DELETE(m_pChatDialog);
			return m_pChatDialog;	//NULL
		}
	}
	return m_pChatDialog;
}

CGuiChatDialog* CChatChannel::AddFamilyChatDialog()
{
	CloseDialog();

	if( NULL == m_pChatDialog )
	{
		m_pChatDialog = new CGuiFUChatDialog(CHAT_TYPE_FAMILY,this);
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\familyunionchat.xml",m_pChatDialog);
		if( FAILED( hr ) ) 
		{
			SAFE_DELETE(m_pChatDialog);
			return m_pChatDialog;	//NULL
		}
	}
	GetRelation().AddMember2ChatChannel( this,CRelation::RELATION_FAMILY);
	return m_pChatDialog;
}

CGuiChatDialog* CChatChannel::AddUnionChatDialog()
{
	CloseDialog();

	if( NULL == m_pChatDialog )
	{
		m_pChatDialog = new CGuiFUChatDialog(CHAT_TYPE_UNION,this);
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\familyunionchat.xml",m_pChatDialog);
		if( FAILED( hr ) ) 
		{
			SAFE_DELETE(m_pChatDialog);
			return m_pChatDialog;	//NULL
		}
	}
	GetRelation().AddMember2ChatChannel( this,CRelation::RELATION_UNION);
	return m_pChatDialog;
}

void CChatChannel::ShowChatDialog()
{
	FUNCTION_BEGIN;

	switch( m_dwChatType )
	{
	case CHAT_TYPE_PRIVATE : m_pChatDialog = AddPrivateChatDialog();break;
	case CHAT_TYPE_FAMILY  : m_pChatDialog = AddFamilyChatDialog();break;
	case CHAT_TYPE_UNION   : m_pChatDialog = AddUnionChatDialog();break;
	default				   : m_pChatDialog = AddPrivateChatDialog();break;
	}

	if( NULL != m_pChatDialog )
	{
		UpdateUserList();	//同时刷新成员列表
		ShowAllMessage();	//显示所有的消息
		m_pChatDialog->UpdateDialogTitle();
		m_pChatDialog->SetVisible( true );
		m_pChatDialog->ScrollToTail();
		GetChatManager().RemoveInvite(this);
		GetChatManager().HideChannelMinizedButton(this);
	}
	else
	{
		AssertFatal(0,"privatechat dialog error!");
	}
}

void CChatChannel::HideChatDialog()	//其实是close
{
	if( NULL != m_pChatDialog )
	{
		//把内存中的消息写到文件中
		WriteMsg2File( m_szFileName );	
		m_pChatDialog->Close();	
	}
}

//关闭该聊天频道
void CChatChannel::CloseChannel()
{
	FUNCTION_BEGIN;
	if ( m_id != 0 )		//不是私聊
	{
		//发送关闭聊天频道消息
		stLeave_ChannelUserCmd cmd;
		strncpy(cmd.name,GetScene()->GetMainCharacter()->GetName(),MAX_NAMESIZE);
		cmd.dwChannelID = m_id;
		SEND_USER_CMD(cmd);
	}

	//通知管理器删除该频道
	GetChatManager().RemoveChannel( this );
}


bool CChatChannel::IsPrivateChat()
{
	if( m_dwChatType == CHAT_TYPE_PERSON || m_dwChatType == CHAT_TYPE_PRIVATE )
		return true;
	else
		return false;
}

bool CChatChannel::IsChatDialogClosed()
{
	if( !m_pChatDialog || !m_pChatDialog->IsVisible() )
	{
		return true;
	}
	return false;
}

//私聊(普通)
CPrivateChatChannel::CPrivateChatChannel(const char* user)
{
	FUNCTION_BEGIN;
	m_id = 0;

	m_pChatDialog = NULL;

	AddMember( GetScene()->GetMainCharacter()->GetName() );
	AddMember( user );

	//避免多开时误加入别人的信息
	char szTemp[MAX_PATH];
	_snprintf(szTemp,MAX_PATH,"%s__%s",GetScene()->GetMainCharacter()->GetName(),user);
	szTemp[MAX_PATH - 1] = 0;

	DWORD crc = calculateCRC(szTemp,strlen(szTemp)+1);
	_snprintf(m_szFileName,MAX_PATH,"data\\privatechat\\%u.xml",crc);
	m_szFileName[MAX_PATH - 1] = 0;

	//如果发现该频道以前创建过文件则删除之
	if( IsFileExist( m_szFileName ) )
	{
		DeleteFile( m_szFileName );
	}
	m_dwChatType = CHAT_TYPE_PRIVATE;
	m_bForbid = false;
	m_bMine = false;
}


//////////////////////////////////////////////////////////////////////////

CChatManager::CChatManager()
{
	FUNCTION_BEGIN;
	::CreateDirectory("data\\chat",NULL);
	::CreateDirectory("data\\privatechat",NULL);

	m_pFamilyChannel = NULL;
	m_pUinonChannel = NULL;
}

CChatManager::~CChatManager()
{
	FUNCTION_BEGIN;
	RemoveAllChannels();
}

//添加私聊对话
CChatChannel* CChatManager::GetPrivateChatChannel( const char* name )
{
	FUNCTION_BEGIN;

	CChatChannel* pChannel = FindPrivateChannelByName( name );
	if( NULL == pChannel )
	{
		//如果该频道内有3人以上的成员则可以同其中的某一个成员私聊
		pChannel = new CPrivateChatChannel( name );
		m_Channels.push_back( pChannel );
	}
	Assert(pChannel);
	return pChannel;
}

//直接跟玩家对话
void CChatManager::PrivateChatWith(const char* name)
{
	FUNCTION_BEGIN;
	if( 0 == strcmp(name,GetScene()->GetMainCharacter()->GetName()) )
	{
		return ;
	}

	CGuiChatInvateDialog::AddPrivateChatName(name);

	CChatChannel* pChannel = GetPrivateChatChannel( name );
	if ( NULL != pChannel )
	{
		pChannel->ShowChatDialog();
	}	
}

//
CChatChannel* CChatManager::GetOrAddChannelByID(DWORD id)
{
	FUNCTION_BEGIN;
	CChatChannel* pChannel = FindChannelByID( id );
	if( NULL == pChannel )		
	{
		pChannel = new CChatChannel();
		pChannel->SetChannelID(id);
		m_Channels.push_back( pChannel );
	}
	return pChannel;
}

CChatChannel* CChatManager::GetOrAddFamilyChannel()
{
	if( NULL == m_pFamilyChannel )
	{
		m_pFamilyChannel = new CChatChannel(CHAT_TYPE_FAMILY);
	}
	return m_pFamilyChannel;
}

CChatChannel* CChatManager::GetOrAddUnionChannel()
{
	if( NULL == m_pUinonChannel )
	{
		m_pUinonChannel = new CChatChannel(CHAT_TYPE_UNION);
	}
	return m_pUinonChannel;
}

// 创建一个私聊频道
void CChatManager::CreateChatChannel(stCreateChannelUserCmd* pCmd)
{
	FUNCTION_BEGIN;
	CChatChannel* pChannel = GetOrAddChannelByID( pCmd->dwChannelID );
	Assert(pChannel);
	pChannel->AddMember(GetScene()->GetMainCharacter()->GetName());
	pChannel->AddMember(pCmd->name);
	pChannel->SetMine(true);
	CChatChannel* pPrivateChannel = FindPrivateChannelByName(pCmd->name);
	if( pPrivateChannel )
	{
		//复制原来的消息到新的频道
		pChannel->CopyMessage(pPrivateChannel);
		pPrivateChannel->CloseChannel();
	}
	pChannel->ShowChatDialog();
}


void CChatManager::AddMessageTips(DWORD dwChannel,DWORD charType,CChatChannel* pChannel,const char* name)
{

	FUNCTION_BEGIN;
	tChatInvite* pInvite = NULL;
	tListChatInviteIt it = m_Invites.end();
	if( NULL == pChannel )
	{ 
		it = FindInviteIt(dwChannel,charType);	
	}
	else
	{
		it = FindInviteIt(pChannel);
	}
	if( it == m_Invites.end() )
	{
		pInvite = new tChatInvite();
		m_Invites.push_back( pInvite );
	}
	else
	{
		pInvite = (*it);
	}
	pInvite->dwChannelID = dwChannel;
	pInvite->dwCharType = charType;
	pInvite->fCurrTime = 0;
	pInvite->fKeepTime = 6000;
	pInvite->pChannel = pChannel;
	if( NULL != name )
	{
		strncpy(pInvite->name,name,MAX_NAMESIZE);
	}		
	UpdateChatTipsButtonState();
}

void CChatManager::RemoveInvite(CChatChannel* pChannel)
{
	tListChatInviteIt it = FindInviteIt(pChannel);
	if( it != m_Invites.end() )
	{
		delete (tChatInvite*)(*it);
		m_Invites.erase(it);
		UpdateChatTipsButtonState();
	}
}

void CChatManager::RemoveInvite(tChatInvite* pInvite)
{
	tListChatInviteIt it = FindInviteIt(pInvite);
	if( it != m_Invites.end() )
	{
		delete (tChatInvite*)(*it);
		m_Invites.erase(it);
		UpdateChatTipsButtonState();
	}
}

CChatManager::tListChatInviteIt CChatManager::FindInviteIt(CChatChannel* pChannel)
{
	FUNCTION_BEGIN;
	for( tListChatInviteIt it = m_Invites.begin();
		it != m_Invites.end(); ++it )
	{
		tChatInvite* pInvite = (*it);
		if( pInvite->pChannel == pChannel )
		{
			return it;
		}
	}
	return m_Invites.end();
}

CChatManager::tListChatInviteIt CChatManager::FindInviteIt(tChatInvite*  pInvite)
{
	FUNCTION_BEGIN;
	for( tListChatInviteIt it = m_Invites.begin();
		it != m_Invites.end(); ++it )
	{
		if( *it == pInvite )
		{
			return it;
		}
	}
	return m_Invites.end();
}


CChatManager::tListChatInviteIt CChatManager::FindInviteIt(DWORD channel,DWORD charType)
{
	FUNCTION_BEGIN;
	for( tListChatInviteIt it = m_Invites.begin();
		it != m_Invites.end(); ++it )
	{
		tChatInvite* pInvite = (*it);
		if( pInvite->dwChannelID == channel && 
			pInvite->dwCharType == charType )
		{
			return it;
		}
	}
	return m_Invites.end();
}

void CChatManager::HideChannelMinizedButton(CChatChannel* pChannel)
{
	tListHideChatButtonIt it = FindHideButtonByChannel( pChannel );
	if ( (it != m_HideBtns.end()) &&
		(NULL != it->pButton) )
	{
		it->pButton->SetVisible( false );
		UpdateHideChatButtonState();
	}	
}

// 通知聊天者的私聊频道
void CChatManager::InviteChatChannel(stInvite_ChannelUserCmd* pCmd)
{
	FUNCTION_BEGIN;
	AddMessageTips(pCmd->dwChannelID,pCmd->dwCharType,NULL,pCmd->name);
}

// 加入一个私聊频道
void CChatManager::JoinChatChannel(stJoin_ChannelUserCmd* pCmd)
{
	FUNCTION_BEGIN;
	CChatChannel* pChannel = GetOrAddChannelByID( pCmd->dwChannelID );
	AssertFatal( pChannel,"not find channel and create fail.." );
	pChannel->AddMember(pCmd->name);
	pChannel->UpdateUserList();
	pChannel->ShowChatDialog();
}

// 删除一个私聊频道中的聊天者
void CChatManager::LeaveChatChannel(stLeave_ChannelUserCmd* pCmd)
{
	FUNCTION_BEGIN;
	CChatChannel* pChannel = FindChannelByID( pCmd->dwChannelID );
	if( NULL != pChannel )
	{
		pChannel->RemoveMember( pCmd->name );
		pChannel->UpdateUserList();
		if( 0 == strcmp(GetScene()->GetMainCharacter()->GetName(),pCmd->name) )
		{
			RemoveChannel( pChannel );
		}
	}
}

void CChatManager::UpdateChatTipsButtonState()
{
	FUNCTION_BEGIN;
	CGuiButton* pBtn = NULL;
	if( GetGameGuiManager()->m_guiMain )
	{
		pBtn = GetGameGuiManager()->m_guiMain->m_BtnPrivateChat;
	}
	if( NULL != pBtn )
	{
		if( !m_Invites.empty() )
		{
			tChatInvite* pInvite = m_Invites[0];
			int group = 40;
			int frame = pInvite->dwCharType;
			char tips[32] = "邀请(O)";
			if( pInvite->pChannel && !pInvite->pChannel->IsPrivateChat() )
			{
				group = 11;
				frame = 103;
				if( pInvite->pChannel->GetChatType() == CHAT_TYPE_FAMILY )
				{
					strncpy(tips,"家族聊天(O)",32);
				}
				else if( pInvite->pChannel->GetChatType() == CHAT_TYPE_UNION )
				{
					strncpy(tips,"帮会聊天(O)",32);
				}				
			}
			//给出有新信息提示
			stResourceLocation st("data\\interfaces.gl",group,frame);
			pBtn->SetBitmap(st,1);
			if( GetDevice()->GetWidth() == 800 )
				pBtn->SetLocation(690,563);
			else
				pBtn->SetLocation(736,730);
			pBtn->SetToolTips(tips);
		}
		else
		{
			//soke 没有私聊消息,照旧显示社会关系图标
			int group = (GetDevice()->GetWidth() == 800)?101:1;
			stResourceLocation st("data\\interfaces.gl",group,13);
			pBtn->SetBitmap(st,3); 
			pBtn->SetLocation(0,0);
			pBtn->SetToolTips("社会(O)");
		}
	}
}

CChatManager::tListChatChannelIt CChatManager::FindChannelItByID(DWORD id)
{
	FUNCTION_BEGIN;
	for ( tListChatChannelIt it = m_Channels.begin(); 
		it != m_Channels.end(); ++it )
	{
		if ( (*it)->GetChannelID() == id )
		{
			return it;
		}
	}
	return m_Channels.end();
}

CChatManager::tListChatChannelIt CChatManager::FindChannelItByChannel(CChatChannel* pChannel)
{
	FUNCTION_BEGIN;
	for ( tListChatChannelIt it = m_Channels.begin(); 
		it != m_Channels.end(); ++it )
	{
		CChatChannel* p = (*it);
		if( pChannel == p )
		{
			return it;
		}
	}
	return m_Channels.end();
}

CPrivateChatChannel* CChatManager::FindPrivateChannelByName(const char* user)
{
	FUNCTION_BEGIN;

	for ( tListChatChannelIt it = m_Channels.begin(); 
		it != m_Channels.end(); ++it )
	{
		CChatChannel* pChannel = (*it);
		if( pChannel->IsUserExist(user) &&		//存在该用户
			pChannel->GetChannelID() == 0 )		//频道id == 0 才是私聊对话
		{
			return static_cast<CPrivateChatChannel*>(pChannel);
		}
	}
	return NULL;
}

CChatChannel* CChatManager::FindChannelByID(DWORD id)
{
	FUNCTION_BEGIN;
	tListChatChannelIt it = FindChannelItByID( id );
	if( m_Channels.end() != it )
	{
		return (CChatChannel*)(*it);
	}
	return NULL;
}

CChatManager::tListHideChatButtonIt CChatManager::FindHideButtonItByButtonID(DWORD id)
{
	FUNCTION_BEGIN;
	for ( tListHideChatButtonIt it = m_HideBtns.begin(); 
		it != m_HideBtns.end(); ++it )
	{
		tHideChatButton& tHcb = *it;
		if( (NULL != tHcb.pButton) && 
			(id == tHcb.pButton->GetID()) )
		{
			return it;
		}
	}
	return m_HideBtns.end();
}

CChatManager::tListHideChatButtonIt CChatManager::FindHideButtonItByChannelID(DWORD id)
{
	FUNCTION_BEGIN;
	for ( tListHideChatButtonIt it = m_HideBtns.begin(); 
		it != m_HideBtns.end(); ++it )
	{
		tHideChatButton& tHcb = *it;
		if ( (NULL != tHcb.pChannel) &&
			(id == tHcb.pChannel->GetChannelID()) )
		{
			return it;
		}
	}
	return m_HideBtns.end();
}

CChatManager::tListHideChatButtonIt CChatManager::FindHideButtonByChannel(CChatChannel* pChannel)
{
	FUNCTION_BEGIN;
	for ( tListHideChatButtonIt it = m_HideBtns.begin(); 
		it != m_HideBtns.end(); ++it )
	{
		tHideChatButton& tHcb = *it;
		if( tHcb.pChannel == pChannel )
		{
			return it;
		}
	}
	return m_HideBtns.end();
}

void CChatManager::RemoveChannel(DWORD id)
{
	FUNCTION_BEGIN;
	tListHideChatButtonIt itBtn = FindHideButtonItByChannelID( id );
	if ( m_HideBtns.end() != itBtn )
	{
		//记录删除过的按钮
		m_RemovedIDs.push_back( itBtn->pButton->GetID() );

		//删除按钮
		CGuiMain* pMainDlg = GetGameGuiManager()->m_guiMain;
		if( pMainDlg )
		{
			pMainDlg->RemoveControl( itBtn->pButton );
		}
		m_HideBtns.erase( itBtn );
	}

	tListChatChannelIt itChannel = FindChannelItByID( id );
	if( m_Channels.end() != itChannel )
	{	
		delete (CChatChannel*)(*itChannel);
		m_Channels.erase( itChannel );
	}
}

void CChatManager::RemoveChannel(CChatChannel* pChannel)
{
	tListHideChatButtonIt itBtn = FindHideButtonByChannel( pChannel );
	if ( m_HideBtns.end() != itBtn )
	{
		//记录删除过的按钮
		m_RemovedIDs.push_back( itBtn->pButton->GetID() );

		//删除按钮
		CGuiMain* pMainDlg = GetGameGuiManager()->m_guiMain;
		if( pMainDlg )
		{
			pMainDlg->RemoveControl( itBtn->pButton );
		}
		m_HideBtns.erase( itBtn );
	}

	for ( tListChatChannelIt it = m_Channels.begin(); 
		it != m_Channels.end(); ++it )
	{
		CChatChannel* p = (*it);
		if( pChannel == p )
		{
			delete p;
			m_Channels.erase( it );
			return ;
		}
	}
}

void CChatManager::RemoveAllInvite()
{
	FUNCTION_BEGIN;
	FreePointerList<tListChatInvite>(m_Invites);
	UpdateChatTipsButtonState();
}

void CChatManager::ShowChannelDialog(DWORD id)
{
	FUNCTION_BEGIN;
	CChatChannel* pChannel = FindChannelByID( id );
	if( pChannel )
	{
		pChannel->ShowChatDialog();
	}
}

void CChatManager::ShowChatDialogAllwith(DWORD id,const char* name)
{
	FUNCTION_BEGIN;
	CChatChannel* pChannel = GetOrAddChannelByID( id );
	pChannel->AddMember(GetScene()->GetMainCharacter()->GetName());
	pChannel->AddMember(name);
	pChannel->ShowChatDialog();
}

void CChatManager::RemoveAllChannels()
{
	FUNCTION_BEGIN;
	FreePointerList<tListChatChannel>(m_Channels);
	RemoveAllInvite();
	SAFE_DELETE(m_pFamilyChannel);
	SAFE_DELETE(m_pUinonChannel);
	m_HideBtns.clear();
}

//响应社会关系按钮,如果有没有显示过的消息,则打开对应的对话框显示之
bool CChatManager::OnOpenChatDialog()
{
	FUNCTION_BEGIN;
	tListChatInviteIt it = m_Invites.begin();
	if( it != m_Invites.end() )
	{
		tChatInvite* pInvite = (*it);
		if( NULL != pInvite->pChannel )
		{
			pInvite->pChannel->ShowChatDialog();
		}
		else
		{
			AddInviteDialog( pInvite );
		}
		return true;
	}
	return false;
}

//打开邀请对话框
CGuiChatInvite* CChatManager::AddInviteDialog(tChatInvite* pInvite)
{
	FUNCTION_BEGIN;
	Assert( pInvite );
	char szTitle[MAX_PATH];
	sprintf(szTitle,"%s 邀请你聊天！",pInvite->name);
	CGuiChatInvite* pDlg = new CGuiChatInvite(pInvite);
	HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\ChatReplyDialog.xml",pDlg);
	if( FAILED( hr ) ) 
	{
		delete pDlg;
		return NULL;
	}
	pDlg->SetTitleInfo( szTitle );
	return pDlg;
}

//响应主界面上 IDC_PRIVATECHAT_BTN_BEG ~~ IDC_PRIVATECHAT_BTN_END 按钮事件
bool CChatManager::OnReOpenChatDialog(UINT nID)
{
	FUNCTION_BEGIN;
	CGuiMain* pMainDlg = GetGameGuiManager()->m_guiMain;

	if( pMainDlg && 
		nID >= IDC_HIDECHAT_BTN_BEG && 
		nID < IDC_HIDECHAT_BTN_END )
	{
		tListHideChatButtonIt it = FindHideButtonItByButtonID(nID);
		if( it != m_HideBtns.end() )
		{
			tHideChatButton& tHcb = *it;
			if( NULL != tHcb.pChannel )
			{
				tHcb.pChannel->ShowChatDialog();	//显示对应的频道
			}
			if( NULL != tHcb.pButton )
			{
				tHcb.pButton->SetVisible( false );	//隐藏对应的按钮
			}
			UpdateHideChatButtonState();
		}		
		return true;
	}	
	return false;
}


//响应聊天对话框最小化按钮
bool CChatManager::OnHideChatDialog(CChatChannel* pChannel)
{
	FUNCTION_BEGIN;
	Assert(pChannel);
	if ( pChannel )
	{
		tListHideChatButtonIt it = FindHideButtonByChannel( pChannel );
		if ( it == m_HideBtns.end() )
		{
			//把该频道放入隐藏频道vector
			bool bRes = AddHideChatButton( pChannel );		
			if( !bRes )
			{
				return false;
			}
		}
		else
		{
			if ( NULL != it->pButton )
			{
				it->pButton->SetVisible( true );
			}
		}
		pChannel->HideChatDialog();
		//刷新隐藏频道按钮
		UpdateHideChatButtonState();		
		return true;
	}
	return false;
}

// i: 第几个按钮
stPointI GetHideChatButtonPos(int i)
{   //soke 修改最小化显示位置
	int nHeight=7,nWidth=9,nIcon=32,nNum=7;
	//stPointI ptStart(990,200);
	stPointI ptStart(1155,240);
	if (GetDevice()->GetWidth() == 800)
	{
		ptStart.x = 766;
	}
	else
	{
		//ptStart.x = 990;
		ptStart.x = 1155;
	}
	stPointI stPos = ptStart;
	stPos += stPointI(i/nNum*(nIcon+nHeight)*(-1),i%nNum*(nIcon+nWidth));
	return stPos;
}
//刷新隐藏对话框按钮的位置
void CChatManager::UpdateHideChatButtonState()
{
	FUNCTION_BEGIN;
	/*int nHeight=7,nWidth=9,nIcon=32,nNum=7;
	stPointI ptStart(990,200);
	if (GetDevice()->GetWidth() == 800)
	{
		ptStart.x = 766;
	}
	else
	{
		ptStart.x = 990;
	}*/
	for ( int i = 0; i < m_HideBtns.size(); i++ )
	{
		tHideChatButton& tHcb = m_HideBtns[i];
		if( NULL != tHcb.pButton && tHcb.pButton->IsVisible() )
		{
			int flash = 100;
			int notflash = 104;
			if( tHcb.pChannel && !tHcb.pChannel->IsPrivateChat() )
			{
				//如果不是私聊则显示群聊图标
				flash = 101;
				notflash = 102;
			}
			tHcb.pButton->SetButtonType(CGuiButton::ButtonType_Bitmap);
			if( tHcb.pChannel && (tHcb.pChannel->GetMessageCount() > 0) )
			{				
				tHcb.pButton->SetBitmap(stResourceLocation("data\\Interfaces.gl",11,flash),1);	//闪
			}
			else
			{
				tHcb.pButton->SetBitmap(stResourceLocation("data\\Interfaces.gl",11,notflash),1);	//不闪
			}			

			/*stPointI stPos = ptStart;
			stPos += stPointI(i/nNum*(nIcon+nHeight)*(-1),i%nNum*(nIcon+nWidth));*/
			stPointI stPos = GetHideChatButtonPos(i);
			tHcb.pButton->SetLocation(stPos.x,stPos.y);
			if( tHcb.pChannel )
			{
				switch( tHcb.pChannel->GetChatType() )
				{
				case CHAT_TYPE_UNION:
					tHcb.pButton->SetToolTips("帮会频道");
					break;
				case CHAT_TYPE_FAMILY: 
					tHcb.pButton->SetToolTips("家族频道");
					break;
				case CHAT_TYPE_PRIVATE:
					{
						char sz[64];
						_snprintf(sz,64,"私聊频道--%s",tHcb.pChannel->GetMember(1));
						sz[63] = 0;
						tHcb.pButton->SetToolTips(sz);
					}					
					break;
				case CHAT_TYPE_PERSON:
					tHcb.pButton->SetToolTips("群聊频道");
					break;
				}				
			}
		}		
	}
 
	CLordStrikeManager::GetSingleton()->UpdateHideBtn();
}

//在CGuiMain中的OnRender被调用,用以对所有邀请的保留时间进行刷新
void CChatManager::UpdateAllInvite(float fElapsedTime)
{
	FUNCTION_BEGIN;
	for ( tListChatInviteIt it = m_Invites.begin();
		it != m_Invites.end(); ++it )
	{
		tChatInvite* pInvite = (*it);
		pInvite->fCurrTime += fElapsedTime;
		if( pInvite->fCurrTime > pInvite->fKeepTime)
		{
			delete pInvite;
			it = m_Invites.erase( it ) - 1;		//删除所有超时的邀请
			UpdateChatTipsButtonState();
		}
	}
}

//得到未用的按钮id
UINT CChatManager::GetUnUseButtonID()
{
	FUNCTION_BEGIN;
	if( m_HideBtns.empty() )
	{
		return IDC_HIDECHAT_BTN_BEG;
	}
	else if ( m_RemovedIDs.size() > 0 )
	{
		std::vector<UINT>::iterator it = m_RemovedIDs.begin();
		return *it;
	}
	else
	{
		UINT iMax = IDC_HIDECHAT_BTN_BEG;
		for ( int i = 0; i < m_HideBtns.size(); i++ )
		{
			tHideChatButton& tHcb = m_HideBtns[i];
			iMax = max( iMax,tHcb.pButton->GetID() );
		}
		return iMax + 1;
	}
}

void CChatManager::DeleteIdFromRemovedList(UINT id)
{
	FUNCTION_BEGIN;
	for ( std::vector<UINT>::iterator it = m_RemovedIDs.begin();
		it != m_RemovedIDs.end(); ++it )
	{
		if( *it == id )
		{
			m_RemovedIDs.erase( it );
			return ;
		}
	}
}

//增加一个频道隐藏按钮
bool CChatManager::AddHideChatButton(CChatChannel* pChannel)
{
	FUNCTION_BEGIN;
	Assert(pChannel);
	tHideChatButton tHcb;
	CGuiMain* pMainDlg = GetGameGuiManager()->m_guiMain;
	if( pMainDlg && pChannel )
	{
		CGuiButton* pButton = NULL;
		UINT id = GetUnUseButtonID();

		//增加有效性判断id
		if( id >= IDC_HIDECHAT_BTN_BEG && 
			id < IDC_HIDECHAT_BTN_END )
		{
			if ( FAILED( pMainDlg->AddButton(id,NULL,0,0,0,0,0,0,&(pButton)) ) )
			{
				return false;
			}
			if( pButton )
			{
				tHcb.pButton = pButton;
				tHcb.pChannel = pChannel;
				m_HideBtns.push_back( tHcb );

				//成功构造新的按钮时从历史列表中删除该id
				DeleteIdFromRemovedList( id );
				return true;
			}		
		}
	}
	return false;
}

bool CChatManager::IsChatDialogMinsized(CChatChannel* pChannel)
{
	FUNCTION_BEGIN;
	for ( tListHideChatButtonIt it = m_HideBtns.begin(); 
		it != m_HideBtns.end(); ++it )
	{
		tHideChatButton& tHcb = *it;
		if( tHcb.pChannel == pChannel )
		{
			return true;
		}
	}
	return false;
}

void CChatManager::OnChat(const char* name,CChatChannel* pChannel)
{
	FUNCTION_BEGIN;
	CChatChannel* pCh = pChannel;
	if( NULL != pCh )
	{
		pCh->InviteUser(name);
	}
	else
	{
		PrivateChatWith(name);
	}
}

bool CChatManager::AddPersonChatMessage(stChannelChatUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	/*
	返回值的作用是作为是否往guioutput窗口继续添加消息的判断条件
	如果返回true则不加了
	*/
	bool bRes = false;
	CChatChannel* pChannel = NULL;
	switch( pCmd->dwType )
	{
	case CHAT_TYPE_PRIVATE:
	case CHAT_TYPE_FRIEND_PRIVATE:
	case CHAT_TYPE_UNION_PRIVATE:
	case CHAT_TYPE_OVERMAN_PRIVATE:
	case CHAT_TYPE_FAMILY_PRIVATE:
		{
			if( pCmd->dwChannelID == 0 )
			{
				pChannel = GetPrivateChatChannel(pCmd->pstrName);
			}
			else
			{
				pChannel = GetOrAddChannelByID( pCmd->dwChannelID );
			}
			CGuiChatInvateDialog::AddPrivateChatName(pCmd->pstrName);
			bRes = true;
		}
		break;
	case CHAT_TYPE_PERSON:
		{
			pChannel = FindChannelByID(pCmd->dwChannelID);
			CGuiChatInvateDialog::AddPrivateChatName(pCmd->pstrName);
			bRes = true;
		}
		break;
	case CHAT_TYPE_FAMILY:
		{
			pChannel = GetOrAddFamilyChannel();
		}
		break;
	case CHAT_TYPE_UNION:
		{
			pChannel = GetOrAddUnionChannel();
		}
		break;
	default:break;
	}

	if( NULL != pChannel && !pChannel->GetForbidden() )
	{
		FilterChatMessage(pCmd);
		TiXmlElement page("p");
		DWORD dwColor = GetChatColor(pCmd->dwType);
		FormatMsgTitle(pCmd->pstrName,dwColor,&page);
		FormatMsgContent2XML(pCmd,&page);
		pChannel->AddMsg(&page);

		bool bIsDialogMinsize = GetChatManager().IsChatDialogMinsized(pChannel);
		if( bIsDialogMinsize )
		{
			//聊天对话框如果是最小化了，则在隐藏按钮上给出提示
			GetChatManager().UpdateHideChatButtonState();
		}
		else if( pChannel->IsChatDialogClosed() && pChannel->IsPrivateChat() )	//私聊情况下才给出提示
		{
			GetChatManager().AddMessageTips(pChannel->GetChannelID(),pCmd->dwCharType,pChannel,NULL);
		}
		return bRes ;
	}
	return false;
}

bool CChatManager::OnAddFamilyChat()
{
	m_pFamilyChannel = GetOrAddFamilyChannel();
	if( m_pFamilyChannel )
	{	
		m_pFamilyChannel->ShowChatDialog();
		return true;
	}
	return false;
}

bool CChatManager::OnAddUnionChat()		//帮会聊天
{
	m_pUinonChannel = GetOrAddUnionChannel();
	if( m_pUinonChannel )
	{		
		m_pUinonChannel->ShowChatDialog();
		return true;
	}
	return false;
}

bool CChatManager::SendChatCmd(CGuiEditBox* pEditChat,DWORD id,DWORD type,const char* name)
{
	if ( NULL == pEditChat )
		return false;	
	pEditChat->ResetCurChatHistory();

	const char* pszText = pEditChat->GetText();

	char szText[512]={0};
	strncpy(szText,pszText,512);
	str_trim_left((char *)szText);
	str_trim_right((char *)szText);
	if(strlen(szText) == 0) return false;



	//First we save chat history
	if ( pEditChat->IsNeedSaveHistoryInfo() )
		pEditChat->SaveCurContent();

	if( IsChatTypeClosed (type) )
	{
		GetGameGuiManager()->AddClientSystemMessage("该频道已经被你屏蔽了,要开启频道请到系统设置-聊天页面");
		return false;
	}


	char szCharText[512]={0};
	if( !G_PhraseFilterChat(szText,szCharText,MAX_CHATINFO) )
	{
		strncpy(szCharText,szText,MAX_CHATINFO);
	}

	//Get msg content
	void* p = NULL;
	BYTE numItem = 0;
	if ( pEditChat->GetStyle() & CGuiEditBox::GUI_ES_MULTIMEDIA )
	{
		//Update chat edit box
		pEditChat->UpdateMediaListBeforeSend();
		numItem = pEditChat->GetMediaList().size();
		p = malloc( sizeof( stChannelChatUserCmd ) + numItem*sizeof(stTradeObject) );
	}
	else
	{
		p = malloc( sizeof( stChannelChatUserCmd ) );
	}

	char szChatName[MAX_NAMESIZE]={0};

	strncpy(szChatName,name,sizeof(szChatName));

	DWORD iChatType = type;

	{
		std::string  strChat = pszText;
		if ((strChat.find("/")==0)&&(strChat.find("/",1)!=1))
		{
			int nPos = strChat.find(" ");
			if (nPos!=-1)
			{
				strncpy(szChatName,strChat.substr(1,nPos-1).c_str(),sizeof(szChatName));


				char pstrName[MAX_NAMESIZE];
				memset(pstrName,0,sizeof(pstrName));
				if( G_PhraseFilterName(szChatName,pstrName,MAX_NAMESIZE))
				{
					strncpy(szChatName,pstrName,MAX_NAMESIZE);
				}
			
				CGuiPrivateChatSelect::AddWhisperChatUser(szChatName);

				strncpy(szCharText,strChat.substr(nPos+1).c_str(),sizeof(szCharText));

				if( strlen(szCharText) > 0 )
				{
					iChatType = CHAT_TYPE_WHISPER;
				}
				else
				{
					return false;
				}
			}
		}
	}

	stChannelChatUserCmd* cmd = (stChannelChatUserCmd*) p;

	cmd->byCmd = CHAT_USERCMD;
	cmd->byParam = ALL_CHAT_USERCMD_PARAMETER;
	cmd->dwChannelID = id;
	cmd->dwSysInfoType = 0;
	cmd->dwFromID = GetScene()->GetMainCharacter()->GetID();
	cmd->dwType = iChatType;
	strncpy(cmd->pstrName,szChatName,sizeof(cmd->pstrName));
	strncpy(cmd->pstrChat,szCharText,sizeof(cmd->pstrChat));

	//we del continue block of multimedia special char and only left one
	str_del_sp( cmd->pstrChat, (char)0xfe );

	if ( pEditChat->GetStyle() & CGuiEditBox::GUI_ES_MULTIMEDIA ) 
	{
		if ( numItem > 0 )			//Add item info if there have any
		{
			CGuiEditBox::mapMediaNameData::iterator	itor;
			CGuiEditBox::mapMediaNameData	mediaList = pEditChat->GetMediaList();
			int i=0;
			for( itor=mediaList.begin();itor!=mediaList.end();itor++ )
			{
				cmd->tobject_array[i].dwObjectTempID = itor->second.mNumber;
				//When we send chat to ourself we init all the struct
				if ( iChatType == CHAT_TYPE_WHISPER )
				{
					CRoleItem* pRoleItem = GetScene()->GetMainCharacter()->FindItemByID( itor->second.mNumber );
					if ( pRoleItem )
					{
						cmd->tobject_array[i].dwObjectID = pRoleItem->GetObjectID();
						cmd->tobject_array[i].kind = pRoleItem->GetObject()->kind;
						cmd->tobject_array[i].strName[0] = 0L;
						sprintf( cmd->tobject_array[i].strName, pRoleItem->GetName() );
						cmd->tobject_array[i].upgrade = pRoleItem->GetObject()->upgrade;
					}				
				}
				i++ ;
			}
		}
	}

	cmd->size = numItem;

	size_t cmdLen = sizeof( stChannelChatUserCmd ) + numItem*sizeof(stTradeObject);

	Client_SendCommand( cmd, cmdLen ,false);

	if (iChatType == CHAT_TYPE_WHISPER)
	{
		cmd->dwType = CHAT_TYPE_WHISPERTOME;

		char buffer[64*1024];
		stServerChannelChatUserCmd *pCmd=(stServerChannelChatUserCmd *)buffer;
		constructInPlace(pCmd);
		pCmd->dwType = cmd->dwType;
		pCmd->dwSysInfoType = cmd->dwSysInfoType;
		pCmd->dwCharType = cmd->dwCharType;
		pCmd->dwChannelID = cmd->dwChannelID;
		pCmd->dwFromID = cmd->dwFromID;
		pCmd->dwChatTime = cmd->dwChatTime;
		pCmd->size = cmd->size;

		int nameLen=strlen(cmd->pstrName)+1;
		int chatLen=strlen(cmd->pstrChat)+1;
		if(nameLen>1)
			strncpy(pCmd->info,cmd->pstrName,nameLen);
		if(chatLen>1)
			memcpy(pCmd->info+nameLen,cmd->pstrChat,chatLen);
		else    
			memset(pCmd->info+nameLen,0,1);
		char *temp =pCmd->info + nameLen + chatLen;
		if(cmd->size)
		{       
			memcpy(temp,cmd->tobject_array,cmd->size * sizeof(stTradeObject));
		}       
		cmdLen = cmdLen - (MAX_NAMESIZE - nameLen) - (MAX_CHATINFO - chatLen);
		GetClient()->PushCommand(pCmd,cmdLen);
	}

	free( p );
	pEditChat->SetText("");
	pEditChat->ClearMediaList();

	return true;

	FUNCTION_END;
}

void CChatManager::SetChatTitle(const stChannelChatUserCmd* & pCmd,TiXmlElement* pPage,DWORD dwColor)
{
	FUNCTION_BEGIN;
	if (pPage==NULL)
		return;

	TiXmlElement* n;
	char szTitle[64];

	n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
	_snprintf( szTitle,sizeof( szTitle ),"%s ",GetChatTypeName(pCmd->dwType));
	szTitle[sizeof(szTitle)-1] = 0;
	XML_SetColorAttribute(n,"color",dwColor);
	XML_SetNodeText(n,szTitle);

	switch(pCmd->dwType)
	{
	case CHAT_TYPE_WHISPER:
		{
			n = pPage->InsertEndChild(TiXmlElement("name"))->ToElement();
			_snprintf( szTitle,sizeof( szTitle ),"%s",pCmd->pstrName);
			szTitle[sizeof(szTitle)-1] = 0;
			XML_SetColorAttribute(n,"color",dwColor);
			XML_SetNodeText(n,szTitle);

			n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
			XML_SetColorAttribute(n,"color",dwColor);
			XML_SetNodeText(n," 悄悄对你说:");
		}
		break;
	case CHAT_TYPE_WHISPERTOME:
		{
			n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
			XML_SetColorAttribute(n,"color",dwColor);
			XML_SetNodeText(n,"你悄悄对 ");

			n = pPage->InsertEndChild(TiXmlElement("name"))->ToElement();
			_snprintf( szTitle,sizeof( szTitle ),"%s",pCmd->pstrName);
			szTitle[sizeof(szTitle)-1] = 0;
			XML_SetColorAttribute(n,"color",dwColor);
			XML_SetNodeText(n,szTitle);

			n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
			XML_SetColorAttribute(n,"color",dwColor);
			XML_SetNodeText(n,"  说:");
		}
		break;
	case CHAT_TYPE_WORLD:	//世界频道
		{
			if( pCmd->dwSysInfoType == INFO_TYPE_EMPEROR )
			{
			  //soke 2016 - 3 - 15 男生皇帝说话标志
			  if( pCmd->dwCharType==1)
			  {
				DWORD color = COLOR_ARGB(255,70,70,255);
				n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
				XML_SetColorAttribute(n,"color",color);
				XML_SetNodeText(n,"★皇帝★ ");
			  }
			  if( pCmd->dwCharType==2)
			  {
				DWORD color = COLOR_ARGB(255,70,70,255);
				n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
				XML_SetColorAttribute(n,"color",color);
				XML_SetNodeText(n,"★女皇★ ");
			  }
			}

	
				
		//	//soke 2016 - 3 -15 男生国家说话标志
		//	if( pCmd->dwCharType==1 )
		//	{
		//		DWORD color = COLOR_ARGB(255,0,124,194);
		//		n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		//		XML_SetColorAttribute(n,"color",color);
		//		XML_SetNodeText(n,"♂");
		//	}
			//soke 2016 - 3 -15 女生国家说话标志
			if( pCmd->dwCharType==2 )
			{
				DWORD color = COLOR_ARGB(255,247,130,223);
				n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
				XML_SetColorAttribute(n,"color",color);
				XML_SetNodeText(n,"♀");
			}

			//soke 世界说话内容
			FormatMsgTitle(pCmd->pstrName,dwColor,pPage);
		}
		break;
	case CHAT_TYPE_COUNTRY:
		{
			DWORD color = dwColor;
			switch( pCmd->dwSysInfoType )
			{
			case INFO_TYPE_KING:
				{
				  //soke 2016 - 3 - 15 国家国王说话标志
			      if( pCmd->dwCharType==1)
			      {
					color = COLOR_ARGB(255,255,128,64);

					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★国王★ ");
					//soke 2016 - 3 - 15 如果是国战期间，同步屏幕中间紫色公告
					if(CCountry::Instance()->IsCountryWarState())
					{
					  GetGameGuiManager()->AddClientTaskMessage(pCmd->pstrChat,COLOR_ARGB(255,135,24,198));
					}
				  }
				  
				  //soke 2016 - 3 - 15 国家女王说话标志
				  if( pCmd->dwCharType==2)
				  {
					color = COLOR_ARGB(255,255,0,255);
					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★女王★ ");
					//soke 如果是国战期间，同步屏幕中间紫色公告
					if(CCountry::Instance()->IsCountryWarState())
					{
					  GetGameGuiManager()->AddClientTaskMessage(pCmd->pstrChat,COLOR_ARGB(255,135,24,198));
					}
				  }
				}
				break;
			case INFO_TYPE_CONSORT:
				{
				  //soke 男生王夫说话标志
			      if( pCmd->dwCharType==1)
				  {
					color = COLOR_ARGB(255,214,146,223);
					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★王夫★ ");
				  }
				  //soke 女生王后说话标志
				  if( pCmd->dwCharType==2)
				  {
					color = COLOR_ARGB(255,255,0,255);
					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★王后★ ");
				  }
				}
				break;
			case INFO_TYPE_CONSORZ:
				{
				  //soke 男生皇夫说话标志
			      if( pCmd->dwCharType==1)
				  {
					color = COLOR_ARGB(255,214,146,223);
					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★皇夫★ ");
				  }
				  //soke 女生皇后说话标志
				  if( pCmd->dwCharType==2)
				  {
					color = COLOR_ARGB(255,255,0,255);
					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★皇后★ ");
				  }
				}
				break;
			case INFO_TYPE_CASTELLAN:
				{
					color = COLOR_ARGB(255,70,70,255);
					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★城主★ ");
				}
				break;
			case INFO_TYPE_EMPEROR:
				{
				  //soke 2016 - 3 - 15 男生皇帝说话标志
			      if( pCmd->dwCharType==1)
				  {
					color = COLOR_ARGB(255,70,70,255);
					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★皇帝★ ");
				  }
				   //soke 2016 - 3 - 15 男生女皇说话标志
				  if( pCmd->dwCharType==2)
				  {
					color = COLOR_ARGB(255,255,0,255);
					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★女皇★ ");
				  }
				}
				break;
			case INFO_TYPE_CATCHER://soke 2016 - 3 - 15 捕头（左）
				{
					color = COLOR_ARGB(255,70,70,255);
					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★捕头★ ");
				}
				break;
			case INFO_TYPE_CATCHEX://soke 2016 - 3 - 15 捕头（右）
				{
					color = COLOR_ARGB(255,70,70,255);
					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★捕头★ ");
				}
				break;
			case INFO_TYPE_CENSOR://soke 2016 - 3 - 15 御史大夫（左）
				{
					color = COLOR_ARGB(255,70,70,255);
					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★御史大夫★ ");
				}
				break;
			case INFO_TYPE_CENSOS://soke 2016 - 3 - 15 御史大夫（右）
				{
					color = COLOR_ARGB(255,70,70,255);
					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★御史大夫★ ");
				}
				break;
			case INFO_TYPE_YUANSHUAI://soke 2016 - 3 - 15 元帅
				{
					color = COLOR_ARGB(255,70,70,255);
					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★元帅★ ");
				}
				break;
			case INFO_TYPE_ZAIXIANG://soke 2016 - 3 - 15 宰相
				{
					color = COLOR_ARGB(255,70,70,255);
					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★宰相★ ");
				}
				break;
			case INFO_TYPE_ZTBB://soke 2017 - 1 - 10 征途宝贝的称号
				{
				  //soke 女生征途宝贝说话标志
				  if( pCmd->dwCharType==2)
				  {
					color = COLOR_ARGB(255,255,0,255);
					n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
					XML_SetColorAttribute(n,"color",color);
					XML_SetNodeText(n,"★征途宝贝★ ");
				  }
				}
				break;
			default:
				break;
			}

			//soke 2016 - 3 - 15 男生国家说话标志
		//	if( pCmd->dwCharType==1 )
		//	{
		//		DWORD color = COLOR_ARGB(255,0,124,194);
		//		n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		//		XML_SetColorAttribute(n,"color",color);
		//		XML_SetNodeText(n,"♂");
		//	}
			//soke 2016 - 3 - 15 女生国家说话标志
			if( pCmd->dwCharType==2 )
			{
				DWORD color = COLOR_ARGB(255,247,130,223);
				n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
				XML_SetColorAttribute(n,"color",color);
				XML_SetNodeText(n,"♀");
			}

			//soke 说话的内容
			FormatMsgTitle(pCmd->pstrName,dwColor,pPage);
		}
		break;
	case CHAT_TYPE_AUTO:
		{
			n = pPage->InsertEndChild(TiXmlElement("name"))->ToElement();
			_snprintf( szTitle,sizeof( szTitle ),"%s",pCmd->pstrName);
			szTitle[sizeof(szTitle)-1] = 0;
			XML_SetColorAttribute(n,"color",dwColor);
			XML_SetNodeText(n,szTitle);
			n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
			XML_SetColorAttribute(n,"color",dwColor);
			XML_SetNodeText(n," 悄悄对你说:");
		}
		break;
	default:
		{
			FormatMsgTitle(pCmd->pstrName,dwColor,pPage);
		}
		break;
	}	

	FUNCTION_END;
}


void CChatManager::FormatMsgTitle(const char* name,DWORD color,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	if (pPage==NULL)
		return;
	TiXmlElement*n = pPage->InsertEndChild(TiXmlElement("name"))->ToElement();
	XML_SetColorAttribute(n,"color",color);
	XML_SetNodeText(n,name);

	n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
	XML_SetColorAttribute(n,"color",color);
	XML_SetNodeText(n,":");
}

//soke 玩家世界说话内容
void CChatManager::FormatCommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	if (pPage==NULL)
		return;
	char szCharMsg[512];
	strncpy(szCharMsg,strChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	//delete media
	str_del_sp_full( szCharMsg, (char)( ITEM_SPECIAL_CHAR >> 8 ) );

	TiXmlElement* n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
	XML_SetColorAttribute(n,"color",color);
	XML_SetNodeText(n,szCharMsg);
}

//soke 宋国玩家世界说话
void CChatManager::Format2CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	if (pPage==NULL)
		return;
	char szCharMsg[512];
	strncpy(szCharMsg,strChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	//delete media
	str_del_sp_full( szCharMsg, (char)( ITEM_SPECIAL_CHAR >> 8 ) );

	TiXmlElement* n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
	XML_SetColorAttribute(n,"color",color);
	XML_SetNodeText(n,szCharMsg);

	//soke 世界说话国家判断（不是发送物品链接显示）
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(n,"color",color);
		XML_SetNodeText(n,"[宋国玩家]");
	}
}

//soke 魏国玩家世界说话
void CChatManager::Format3CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	if (pPage==NULL)
		return;
	char szCharMsg[512];
	strncpy(szCharMsg,strChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	//delete media
	str_del_sp_full( szCharMsg, (char)( ITEM_SPECIAL_CHAR >> 8 ) );

	TiXmlElement* n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
	XML_SetColorAttribute(n,"color",color);
	XML_SetNodeText(n,szCharMsg);

	//soke 世界说话国家判断（不是发送物品链接显示）
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(n,"color",color);
		XML_SetNodeText(n,"[魏国玩家]");
	}
}

//soke 齐国玩家世界说话
void CChatManager::Format4CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	if (pPage==NULL)
		return;
	char szCharMsg[512];
	strncpy(szCharMsg,strChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	//delete media
	str_del_sp_full( szCharMsg, (char)( ITEM_SPECIAL_CHAR >> 8 ) );

	TiXmlElement* n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
	XML_SetColorAttribute(n,"color",color);
	XML_SetNodeText(n,szCharMsg);

	//soke 世界说话国家判断（不是发送物品链接显示）
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(n,"color",color);
		XML_SetNodeText(n,"[齐国玩家]");
	}
}

//soke 燕国玩家世界说话
void CChatManager::Format5CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	if (pPage==NULL)
		return;
	char szCharMsg[512];
	strncpy(szCharMsg,strChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	//delete media
	str_del_sp_full( szCharMsg, (char)( ITEM_SPECIAL_CHAR >> 8 ) );

	TiXmlElement* n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
	XML_SetColorAttribute(n,"color",color);
	XML_SetNodeText(n,szCharMsg);

	//soke 世界说话国家判断（不是发送物品链接显示）
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(n,"color",color);
		XML_SetNodeText(n,"[燕国玩家]");
	}
}
//soke 唐国玩家世界说话
void CChatManager::Format7CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	if (pPage==NULL)
		return;
	char szCharMsg[512];
	strncpy(szCharMsg,strChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	//delete media
	str_del_sp_full( szCharMsg, (char)( ITEM_SPECIAL_CHAR >> 8 ) );

	TiXmlElement* n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
	XML_SetColorAttribute(n,"color",color);
	XML_SetNodeText(n,szCharMsg);

	//soke 世界说话国家判断（不是发送物品链接显示）
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(n,"color",color);
		XML_SetNodeText(n,"[唐国玩家]");
	}
}

//soke 汉国玩家世界说话
void CChatManager::Format8CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	if (pPage==NULL)
		return;
	char szCharMsg[512];
	strncpy(szCharMsg,strChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;
	//delete media
	str_del_sp_full( szCharMsg, (char)( ITEM_SPECIAL_CHAR >> 8 ) );

	TiXmlElement* n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
	XML_SetColorAttribute(n,"color",color);
	XML_SetNodeText(n,szCharMsg);

	//soke 世界说话国家判断（不是发送物品链接显示）
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(n,"color",color);
		XML_SetNodeText(n,"[汉国玩家]");
	}
}
//soke 吴国玩家世界说话
void CChatManager::Format9CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	if (pPage==NULL)
		return;
	char szCharMsg[512];
	strncpy(szCharMsg,strChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	//delete media
	str_del_sp_full( szCharMsg, (char)( ITEM_SPECIAL_CHAR >> 8 ) );

	TiXmlElement* n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
	XML_SetColorAttribute(n,"color",color);
	XML_SetNodeText(n,szCharMsg);

	//soke 世界说话国家判断（不是发送物品链接显示）
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(n,"color",color);
		XML_SetNodeText(n,"[吴国玩家]");
	}
}

//soke 赵国玩家世界说话
void CChatManager::Format10CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	if (pPage==NULL)
		return;
	char szCharMsg[512];
	strncpy(szCharMsg,strChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	//delete media
	str_del_sp_full( szCharMsg, (char)( ITEM_SPECIAL_CHAR >> 8 ) );

	TiXmlElement* n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
	XML_SetColorAttribute(n,"color",color);
	XML_SetNodeText(n,szCharMsg);

	//soke 世界说话国家判断（不是发送物品链接显示）
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(n,"color",color);
		XML_SetNodeText(n,"[赵国玩家]");
	}
}

//soke 周国玩家世界说话
void CChatManager::Format11CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	if (pPage==NULL)
		return;
	char szCharMsg[512];
	strncpy(szCharMsg,strChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	//delete media
	str_del_sp_full( szCharMsg, (char)( ITEM_SPECIAL_CHAR >> 8 ) );

	TiXmlElement* n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
	XML_SetColorAttribute(n,"color",color);
	XML_SetNodeText(n,szCharMsg);

	//soke 世界说话国家判断（不是发送物品链接显示）
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(n,"color",color);
		XML_SetNodeText(n,"[周国玩家]");
	}
}

//soke 楚国玩家世界说话
void CChatManager::Format12CommMsgContent(const char* strChat,DWORD color,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	if (pPage==NULL)
		return;
	char szCharMsg[512];
	strncpy(szCharMsg,strChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	//delete media
	str_del_sp_full( szCharMsg, (char)( ITEM_SPECIAL_CHAR >> 8 ) );

	TiXmlElement* n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
	XML_SetColorAttribute(n,"color",color);
	XML_SetNodeText(n,szCharMsg);

	//soke 世界说话国家判断（不是发送物品链接显示）
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		n = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(n,"color",color);
		XML_SetNodeText(n,"[楚国玩家]");
	}
}

void CChatManager::FormatMsgContent2XML(stChannelChatUserCmd* pCmd,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;
	
	DWORD dwColor = GetChatColor(pCmd->dwType);

	//soke 如果是正常的说话
	if( pCmd->size <= 0)
	{
	  if( pCmd->dwSysInfoType == INFO_TYPE_SONGGUO ) //soke 宋国玩家世界说话
	  {
		CChatManager::Format2CommMsgContent(pCmd->pstrChat,dwColor,pPage);
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_WEIGUO )  //soke 魏国玩家世界说话
	  {
		CChatManager::Format3CommMsgContent(pCmd->pstrChat,dwColor,pPage);
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_QIGUO )   //soke 齐国玩家世界说话
	  {
		CChatManager::Format4CommMsgContent(pCmd->pstrChat,dwColor,pPage);
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_YANGUO )  //soke 燕国玩家世界说话
	  {
		CChatManager::Format5CommMsgContent(pCmd->pstrChat,dwColor,pPage);
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_TANGGUO ) //soke 唐国玩家世界说话
	  {
		CChatManager::Format7CommMsgContent(pCmd->pstrChat,dwColor,pPage);
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_HANGUO )  //soke 汉国玩家世界说话
	  {
		CChatManager::Format8CommMsgContent(pCmd->pstrChat,dwColor,pPage);
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_WUGUO )   //soke 吴国玩家世界说话
	  {
		CChatManager::Format9CommMsgContent(pCmd->pstrChat,dwColor,pPage);
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_ZHAOGUO ) //soke 赵国玩家世界说话
	  {
		CChatManager::Format10CommMsgContent(pCmd->pstrChat,dwColor,pPage);
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_ZHOUGUO ) //soke 周国玩家世界说话
	  {
		CChatManager::Format11CommMsgContent(pCmd->pstrChat,dwColor,pPage);
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_CHUGUO )  //soke 楚国玩家世界说话
	  {
		CChatManager::Format12CommMsgContent(pCmd->pstrChat,dwColor,pPage);
	  }
	  else
	  {
		CChatManager::FormatCommMsgContent(pCmd->pstrChat,dwColor,pPage);
	  }
	}
	else
	{
	  if( pCmd->dwSysInfoType == INFO_TYPE_SONGGUO ) //soke 宋国玩家世界说话
	  {
		CChatManager::Format2ItemMsgContent(pCmd,pPage); 
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_WEIGUO )  //soke 魏国玩家世界说话
	  {
		CChatManager::Format3ItemMsgContent(pCmd,pPage); 
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_QIGUO )   //soke 齐国玩家世界说话
	  {
		CChatManager::Format4ItemMsgContent(pCmd,pPage); 
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_YANGUO )  //soke 燕国玩家世界说话
	  {
		CChatManager::Format5ItemMsgContent(pCmd,pPage); 
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_TANGGUO ) //soke 唐国玩家世界说话
	  {
		CChatManager::Format7ItemMsgContent(pCmd,pPage); 
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_HANGUO )  //soke 汉国玩家世界说话
	  {
		CChatManager::Format8ItemMsgContent(pCmd,pPage); 
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_WUGUO )   //soke 吴国玩家世界说话
	  {
		CChatManager::Format9ItemMsgContent(pCmd,pPage); 
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_ZHAOGUO ) //soke 赵国玩家世界说话
	  {
		CChatManager::Format10ItemMsgContent(pCmd,pPage); 
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_ZHOUGUO ) //soke 周国玩家世界说话
	  {
		CChatManager::Format11ItemMsgContent(pCmd,pPage); 
	  }
	  else if( pCmd->dwSysInfoType == INFO_TYPE_CHUGUO )  //soke 楚国玩家世界说话
	  {
		CChatManager::Format12ItemMsgContent(pCmd,pPage); 
	  }
	  else
	  {
		CChatManager::FormatItemMsgContent(pCmd,pPage); //soke 如果是带物品链接的说话
	  }
	}
}

//soke 玩家世界喊话（带物品链接）
void CChatManager::FormatItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	DWORD dwColor = GetChatColor(pCmd->dwType);
	char szCharMsg[512];
	strncpy(szCharMsg,pCmd->pstrChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	// then we get chat content
	char mediaSP[2];
	mediaSP[0] = (char)( ITEM_SPECIAL_CHAR >> 8 );
	mediaSP[1] = 0;			

	char* pszStr = pCmd->pstrChat;
	int numItem = 0;
	int startPos = 0;
	int endPos = 0;
	while ( endPos < strlen(szCharMsg) )
	{
		if ( (BYTE)szCharMsg[endPos] == (ITEM_SPECIAL_CHAR >> 8) )
		{
			//Add common msg before item
			if ( endPos > startPos )
			{
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), (endPos-startPos) );
				szChat[endPos-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
			}
			//Add an item
			if ( numItem < pCmd->size )
			{
				TiXmlElement* item = pPage->InsertEndChild( TiXmlElement( "shell" ) )->ToElement();
				item->SetAttribute( "obj", pCmd->tobject_array[numItem].dwObjectTempID );

				ObjectBase_t* pObject = GetObjectBase( pCmd->tobject_array[numItem].dwObjectID );
				if ( pObject )
				{
					if ( 0 )		//Use object base name
						TiXmlText* itemName = item->InsertEndChild( TiXmlText( pObject->strName ) )->ToText();
					else			//Use server given name
					{
						int gradeType = 0;		//0 nograde
						//1 等
						//2 ★
						int gradeVale = pCmd->tobject_array[numItem].upgrade;
						if ( gradeVale > c_maxItemUpgrade )
							gradeVale = c_maxItemUpgrade;

						char upgradeStar[MAX_NAMESIZE];
						upgradeStar[0] = 0L;	
						for( int k=0;k<gradeVale;k++ )
							strcat( upgradeStar, "★" );
						upgradeStar[strlen(upgradeStar)] = 0L;

						//Get grade type
						if ( pCmd->tobject_array[numItem].kind & 4 )
							gradeType = 2;
						else
						{
							if ( ( (pObject->dwType == ItemType_Resource)
								|| (pObject->dwType == ItemType_SOULSTONE) 
								|| (pObject->dwType == ItemType_LevelUp) )
								&& ( (pObject->dwID == 506 )
								|| (pObject->dwID == 507)
								|| (pObject->dwID == 516)
								|| (pObject->dwID == 517)
								|| (pObject->dwID == 526)
								|| (pObject->dwID == 527)
								|| (pObject->dwID == 536)
								|| (pObject->dwID == 537)
								|| (pObject->dwID == 546)
								|| (pObject->dwID == 547)
								|| (pObject->dwID == 548)
								|| (pObject->dwID == 550)
								|| (pObject->dwID == 561)
								|| (pObject->dwID == 562)
								|| (pObject->dwID == 563)
								|| (pObject->dwID == 564)
								|| (pObject->dwID == 565)
								|| (pObject->dwID == 852)
								|| (pObject->dwID == 877)
								|| (pObject->dwID == 957)) )
							{
								gradeType = 1;
							}
							else
							{
								if ( ((pObject->dwID >= 677) && (pObject->dwType <= 683) && (pObject->dwID != 682))
									|| ((pObject->dwID >= 748) && (pObject->dwType <= 751)) )
								{
									if ( (pObject->dwID == 678) || (pObject->dwID == 679) )
										gradeType = 1;
								}
								else
									gradeType = 2;
							}
						}

						//This time we don't show grade
						gradeType = 0;

						//Split item name
						char* pName;
						char namePartOne[MAX_NAMESIZE];
						char namePartTwo[MAX_NAMESIZE];
						char nameFull[MAX_PATH];
						nameFull[0] = 0L;								

						pName = strpbrk( pCmd->tobject_array[numItem].strName, "0" );
						if ( pName )
						{
							sprintf( namePartOne, ++pName );
							int len = strlen(pCmd->tobject_array[numItem].strName)-strlen(namePartOne)-1;
							strncpy( namePartTwo, pCmd->tobject_array[numItem].strName, len );
							namePartTwo[len] = 0;
						}
						else
						{
							sprintf( namePartTwo, pCmd->tobject_array[numItem].strName );
							sprintf( namePartOne, "" );
						}
						//End split item name

						if ( strlen(namePartOne) > 0 )
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}												
							}
							else
							{
								sprintf(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}

							strcat(nameFull, "(" );
							strcat(nameFull, namePartTwo);
							strcat(nameFull, ")");												
						}
						else
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
							else
							{
								sprintf(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
						}

						TiXmlText* itemName = item->InsertEndChild( TiXmlText( nameFull ) )->ToText();
					}								
				}
				else
					TiXmlText* itemName = item->InsertEndChild( TiXmlText( "道具交易" ) )->ToText();

				numItem++ ;
			}

			pszStr += ++endPos - startPos;
			startPos = endPos;
		}
		else
		{
			if ( numItem >= pCmd->size )
			{
				//Add the last chat part
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), ( strlen(szCharMsg)-startPos ) );
				szChat[strlen(szCharMsg)-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
				break;
			}
			else
			{
				endPos++ ;
			}
		}
	}
}

//soke 宋国玩家世界喊话（带物品链接）
void CChatManager::Format2ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	DWORD dwColor = GetChatColor(pCmd->dwType);
	char szCharMsg[512];
	strncpy(szCharMsg,pCmd->pstrChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	// then we get chat content
	char mediaSP[2];
	mediaSP[0] = (char)( ITEM_SPECIAL_CHAR >> 8 );
	mediaSP[1] = 0;			

	char* pszStr = pCmd->pstrChat;
	int numItem = 0;
	int startPos = 0;
	int endPos = 0;
	while ( endPos < strlen(szCharMsg) )
	{
		if ( (BYTE)szCharMsg[endPos] == (ITEM_SPECIAL_CHAR >> 8) )
		{
			//Add common msg before item
			if ( endPos > startPos )
			{
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), (endPos-startPos) );
				szChat[endPos-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
			}
			//Add an item
			if ( numItem < pCmd->size )
			{
				TiXmlElement* item = pPage->InsertEndChild( TiXmlElement( "shell" ) )->ToElement();
				item->SetAttribute( "obj", pCmd->tobject_array[numItem].dwObjectTempID );

				ObjectBase_t* pObject = GetObjectBase( pCmd->tobject_array[numItem].dwObjectID );
				if ( pObject )
				{
					if ( 0 )		//Use object base name
						TiXmlText* itemName = item->InsertEndChild( TiXmlText( pObject->strName ) )->ToText();
					else			//Use server given name
					{
						int gradeType = 0;		//0 nograde
						//1 等
						//2 ★
						int gradeVale = pCmd->tobject_array[numItem].upgrade;
						if ( gradeVale > c_maxItemUpgrade )
							gradeVale = c_maxItemUpgrade;

						char upgradeStar[MAX_NAMESIZE];
						upgradeStar[0] = 0L;	
						for( int k=0;k<gradeVale;k++ )
							strcat( upgradeStar, "★" );
						upgradeStar[strlen(upgradeStar)] = 0L;

						//Get grade type
						if ( pCmd->tobject_array[numItem].kind & 4 )
							gradeType = 2;
						else
						{
							if ( ( (pObject->dwType == ItemType_Resource)
								|| (pObject->dwType == ItemType_SOULSTONE) 
								|| (pObject->dwType == ItemType_LevelUp) )
								&& ( (pObject->dwID == 506 )
								|| (pObject->dwID == 507)
								|| (pObject->dwID == 516)
								|| (pObject->dwID == 517)
								|| (pObject->dwID == 526)
								|| (pObject->dwID == 527)
								|| (pObject->dwID == 536)
								|| (pObject->dwID == 537)
								|| (pObject->dwID == 546)
								|| (pObject->dwID == 547)
								|| (pObject->dwID == 548)
								|| (pObject->dwID == 550)
								|| (pObject->dwID == 561)
								|| (pObject->dwID == 562)
								|| (pObject->dwID == 563)
								|| (pObject->dwID == 564)
								|| (pObject->dwID == 565)
								|| (pObject->dwID == 852)
								|| (pObject->dwID == 877)
								|| (pObject->dwID == 957)) )
							{
								gradeType = 1;
							}
							else
							{
								if ( ((pObject->dwID >= 677) && (pObject->dwType <= 683) && (pObject->dwID != 682))
									|| ((pObject->dwID >= 748) && (pObject->dwType <= 751)) )
								{
									if ( (pObject->dwID == 678) || (pObject->dwID == 679) )
										gradeType = 1;
								}
								else
									gradeType = 2;
							}
						}

						//This time we don't show grade
						gradeType = 0;

						//Split item name
						char* pName;
						char namePartOne[MAX_NAMESIZE];
						char namePartTwo[MAX_NAMESIZE];
						char nameFull[MAX_PATH];
						nameFull[0] = 0L;								

						pName = strpbrk( pCmd->tobject_array[numItem].strName, "0" );
						if ( pName )
						{
							sprintf( namePartOne, ++pName );
							int len = strlen(pCmd->tobject_array[numItem].strName)-strlen(namePartOne)-1;
							strncpy( namePartTwo, pCmd->tobject_array[numItem].strName, len );
							namePartTwo[len] = 0;
						}
						else
						{
							sprintf( namePartTwo, pCmd->tobject_array[numItem].strName );
							sprintf( namePartOne, "" );
						}
						//End split item name

						if ( strlen(namePartOne) > 0 )
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}												
							}
							else
							{
								sprintf(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}

							strcat(nameFull, "(" );
							strcat(nameFull, namePartTwo);
							strcat(nameFull, ")");												
						}
						else
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
							else
							{
								sprintf(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
						}

						TiXmlText* itemName = item->InsertEndChild( TiXmlText( nameFull ) )->ToText();
					}								
				}
				else
					TiXmlText* itemName = item->InsertEndChild( TiXmlText( "道具交易" ) )->ToText();

				numItem++ ;
			}

			pszStr += ++endPos - startPos;
			startPos = endPos;
		}
		else
		{
			if ( numItem >= pCmd->size )
			{
				//Add the last chat part
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), ( strlen(szCharMsg)-startPos ) );
				szChat[strlen(szCharMsg)-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
				break;
			}
			else
			{
				endPos++ ;
			}
		}
	}

	//soke 世界说话国家判断
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		TiXmlElement* strPart = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(strPart,"color",color);
		XML_SetNodeText(strPart,"[宋国玩家]");
	}
}

//soke 魏国玩家世界喊话（带物品链接）
void CChatManager::Format3ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	DWORD dwColor = GetChatColor(pCmd->dwType);
	char szCharMsg[512];
	strncpy(szCharMsg,pCmd->pstrChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	// then we get chat content
	char mediaSP[2];
	mediaSP[0] = (char)( ITEM_SPECIAL_CHAR >> 8 );
	mediaSP[1] = 0;			

	char* pszStr = pCmd->pstrChat;
	int numItem = 0;
	int startPos = 0;
	int endPos = 0;
	while ( endPos < strlen(szCharMsg) )
	{
		if ( (BYTE)szCharMsg[endPos] == (ITEM_SPECIAL_CHAR >> 8) )
		{
			//Add common msg before item
			if ( endPos > startPos )
			{
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), (endPos-startPos) );
				szChat[endPos-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
			}
			//Add an item
			if ( numItem < pCmd->size )
			{
				TiXmlElement* item = pPage->InsertEndChild( TiXmlElement( "shell" ) )->ToElement();
				item->SetAttribute( "obj", pCmd->tobject_array[numItem].dwObjectTempID );

				ObjectBase_t* pObject = GetObjectBase( pCmd->tobject_array[numItem].dwObjectID );
				if ( pObject )
				{
					if ( 0 )		//Use object base name
						TiXmlText* itemName = item->InsertEndChild( TiXmlText( pObject->strName ) )->ToText();
					else			//Use server given name
					{
						int gradeType = 0;		//0 nograde
						//1 等
						//2 ★
						int gradeVale = pCmd->tobject_array[numItem].upgrade;
						if ( gradeVale > c_maxItemUpgrade )
							gradeVale = c_maxItemUpgrade;

						char upgradeStar[MAX_NAMESIZE];
						upgradeStar[0] = 0L;	
						for( int k=0;k<gradeVale;k++ )
							strcat( upgradeStar, "★" );
						upgradeStar[strlen(upgradeStar)] = 0L;

						//Get grade type
						if ( pCmd->tobject_array[numItem].kind & 4 )
							gradeType = 2;
						else
						{
							if ( ( (pObject->dwType == ItemType_Resource)
								|| (pObject->dwType == ItemType_SOULSTONE) 
								|| (pObject->dwType == ItemType_LevelUp) )
								&& ( (pObject->dwID == 506 )
								|| (pObject->dwID == 507)
								|| (pObject->dwID == 516)
								|| (pObject->dwID == 517)
								|| (pObject->dwID == 526)
								|| (pObject->dwID == 527)
								|| (pObject->dwID == 536)
								|| (pObject->dwID == 537)
								|| (pObject->dwID == 546)
								|| (pObject->dwID == 547)
								|| (pObject->dwID == 548)
								|| (pObject->dwID == 550)
								|| (pObject->dwID == 561)
								|| (pObject->dwID == 562)
								|| (pObject->dwID == 563)
								|| (pObject->dwID == 564)
								|| (pObject->dwID == 565)
								|| (pObject->dwID == 852)
								|| (pObject->dwID == 877)
								|| (pObject->dwID == 957) ) )
							{
								gradeType = 1;
							}
							else
							{
								if ( ((pObject->dwID >= 677) && (pObject->dwType <= 683) && (pObject->dwID != 682))
									|| ((pObject->dwID >= 748) && (pObject->dwType <= 751)) )
								{
									if ( (pObject->dwID == 678) || (pObject->dwID == 679) )
										gradeType = 1;
								}
								else
									gradeType = 2;
							}
						}

						//This time we don't show grade
						gradeType = 0;

						//Split item name
						char* pName;
						char namePartOne[MAX_NAMESIZE];
						char namePartTwo[MAX_NAMESIZE];
						char nameFull[MAX_PATH];
						nameFull[0] = 0L;								

						pName = strpbrk( pCmd->tobject_array[numItem].strName, "0" );
						if ( pName )
						{
							sprintf( namePartOne, ++pName );
							int len = strlen(pCmd->tobject_array[numItem].strName)-strlen(namePartOne)-1;
							strncpy( namePartTwo, pCmd->tobject_array[numItem].strName, len );
							namePartTwo[len] = 0;
						}
						else
						{
							sprintf( namePartTwo, pCmd->tobject_array[numItem].strName );
							sprintf( namePartOne, "" );
						}
						//End split item name

						if ( strlen(namePartOne) > 0 )
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}												
							}
							else
							{
								sprintf(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}

							strcat(nameFull, "(" );
							strcat(nameFull, namePartTwo);
							strcat(nameFull, ")");												
						}
						else
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
							else
							{
								sprintf(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
						}

						TiXmlText* itemName = item->InsertEndChild( TiXmlText( nameFull ) )->ToText();
					}								
				}
				else
					TiXmlText* itemName = item->InsertEndChild( TiXmlText( "道具交易" ) )->ToText();

				numItem++ ;
			}

			pszStr += ++endPos - startPos;
			startPos = endPos;
		}
		else
		{
			if ( numItem >= pCmd->size )
			{
				//Add the last chat part
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), ( strlen(szCharMsg)-startPos ) );
				szChat[strlen(szCharMsg)-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
				break;
			}
			else
			{
				endPos++ ;
			}
		}
	}

	//soke 世界说话国家判断
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		TiXmlElement* strPart = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(strPart,"color",color);
		XML_SetNodeText(strPart,"[魏国玩家]");
	}
}

//soke 齐国玩家世界喊话（带物品链接）
void CChatManager::Format4ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	DWORD dwColor = GetChatColor(pCmd->dwType);
	char szCharMsg[512];
	strncpy(szCharMsg,pCmd->pstrChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	// then we get chat content
	char mediaSP[2];
	mediaSP[0] = (char)( ITEM_SPECIAL_CHAR >> 8 );
	mediaSP[1] = 0;			

	char* pszStr = pCmd->pstrChat;
	int numItem = 0;
	int startPos = 0;
	int endPos = 0;
	while ( endPos < strlen(szCharMsg) )
	{
		if ( (BYTE)szCharMsg[endPos] == (ITEM_SPECIAL_CHAR >> 8) )
		{
			//Add common msg before item
			if ( endPos > startPos )
			{
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), (endPos-startPos) );
				szChat[endPos-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
			}
			//Add an item
			if ( numItem < pCmd->size )
			{
				TiXmlElement* item = pPage->InsertEndChild( TiXmlElement( "shell" ) )->ToElement();
				item->SetAttribute( "obj", pCmd->tobject_array[numItem].dwObjectTempID );

				ObjectBase_t* pObject = GetObjectBase( pCmd->tobject_array[numItem].dwObjectID );
				if ( pObject )
				{
					if ( 0 )		//Use object base name
						TiXmlText* itemName = item->InsertEndChild( TiXmlText( pObject->strName ) )->ToText();
					else			//Use server given name
					{
						int gradeType = 0;		//0 nograde
						//1 等
						//2 ★
						int gradeVale = pCmd->tobject_array[numItem].upgrade;
						if ( gradeVale > c_maxItemUpgrade )
							gradeVale = c_maxItemUpgrade;

						char upgradeStar[MAX_NAMESIZE];
						upgradeStar[0] = 0L;	
						for( int k=0;k<gradeVale;k++ )
							strcat( upgradeStar, "★" );
						upgradeStar[strlen(upgradeStar)] = 0L;

						//Get grade type
						if ( pCmd->tobject_array[numItem].kind & 4 )
							gradeType = 2;
						else
						{
							if ( ( (pObject->dwType == ItemType_Resource)
								|| (pObject->dwType == ItemType_SOULSTONE) 
								|| (pObject->dwType == ItemType_LevelUp) )
								&& ( (pObject->dwID == 506 )
								|| (pObject->dwID == 507)
								|| (pObject->dwID == 516)
								|| (pObject->dwID == 517)
								|| (pObject->dwID == 526)
								|| (pObject->dwID == 527)
								|| (pObject->dwID == 536)
								|| (pObject->dwID == 537)
								|| (pObject->dwID == 546)
								|| (pObject->dwID == 547)
								|| (pObject->dwID == 548)
								|| (pObject->dwID == 550)
								|| (pObject->dwID == 561)
								|| (pObject->dwID == 562)
								|| (pObject->dwID == 563)
								|| (pObject->dwID == 564)
								|| (pObject->dwID == 565)
								|| (pObject->dwID == 852)
								|| (pObject->dwID == 877)
								|| (pObject->dwID == 957) ) )
							{
								gradeType = 1;
							}
							else
							{
								if ( ((pObject->dwID >= 677) && (pObject->dwType <= 683) && (pObject->dwID != 682))
									|| ((pObject->dwID >= 748) && (pObject->dwType <= 751)) )
								{
									if ( (pObject->dwID == 678) || (pObject->dwID == 679) )
										gradeType = 1;
								}
								else
									gradeType = 2;
							}
						}

						//This time we don't show grade
						gradeType = 0;

						//Split item name
						char* pName;
						char namePartOne[MAX_NAMESIZE];
						char namePartTwo[MAX_NAMESIZE];
						char nameFull[MAX_PATH];
						nameFull[0] = 0L;								

						pName = strpbrk( pCmd->tobject_array[numItem].strName, "0" );
						if ( pName )
						{
							sprintf( namePartOne, ++pName );
							int len = strlen(pCmd->tobject_array[numItem].strName)-strlen(namePartOne)-1;
							strncpy( namePartTwo, pCmd->tobject_array[numItem].strName, len );
							namePartTwo[len] = 0;
						}
						else
						{
							sprintf( namePartTwo, pCmd->tobject_array[numItem].strName );
							sprintf( namePartOne, "" );
						}
						//End split item name

						if ( strlen(namePartOne) > 0 )
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}												
							}
							else
							{
								sprintf(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}

							strcat(nameFull, "(" );
							strcat(nameFull, namePartTwo);
							strcat(nameFull, ")");												
						}
						else
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
							else
							{
								sprintf(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
						}

						TiXmlText* itemName = item->InsertEndChild( TiXmlText( nameFull ) )->ToText();
					}								
				}
				else
					TiXmlText* itemName = item->InsertEndChild( TiXmlText( "道具交易" ) )->ToText();

				numItem++ ;
			}

			pszStr += ++endPos - startPos;
			startPos = endPos;
		}
		else
		{
			if ( numItem >= pCmd->size )
			{
				//Add the last chat part
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), ( strlen(szCharMsg)-startPos ) );
				szChat[strlen(szCharMsg)-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
				break;
			}
			else
			{
				endPos++ ;
			}
		}
	}

	//soke 世界说话国家判断
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		TiXmlElement* strPart = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(strPart,"color",color);
		XML_SetNodeText(strPart,"[齐国玩家]");
	}
}

//soke 燕国玩家世界喊话（带物品链接）
void CChatManager::Format5ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	DWORD dwColor = GetChatColor(pCmd->dwType);
	char szCharMsg[512];
	strncpy(szCharMsg,pCmd->pstrChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	// then we get chat content
	char mediaSP[2];
	mediaSP[0] = (char)( ITEM_SPECIAL_CHAR >> 8 );
	mediaSP[1] = 0;			

	char* pszStr = pCmd->pstrChat;
	int numItem = 0;
	int startPos = 0;
	int endPos = 0;
	while ( endPos < strlen(szCharMsg) )
	{
		if ( (BYTE)szCharMsg[endPos] == (ITEM_SPECIAL_CHAR >> 8) )
		{
			//Add common msg before item
			if ( endPos > startPos )
			{
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), (endPos-startPos) );
				szChat[endPos-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
			}
			//Add an item
			if ( numItem < pCmd->size )
			{
				TiXmlElement* item = pPage->InsertEndChild( TiXmlElement( "shell" ) )->ToElement();
				item->SetAttribute( "obj", pCmd->tobject_array[numItem].dwObjectTempID );

				ObjectBase_t* pObject = GetObjectBase( pCmd->tobject_array[numItem].dwObjectID );
				if ( pObject )
				{
					if ( 0 )		//Use object base name
						TiXmlText* itemName = item->InsertEndChild( TiXmlText( pObject->strName ) )->ToText();
					else			//Use server given name
					{
						int gradeType = 0;		//0 nograde
						//1 等
						//2 ★
						int gradeVale = pCmd->tobject_array[numItem].upgrade;
						if ( gradeVale > c_maxItemUpgrade )
							gradeVale = c_maxItemUpgrade;

						char upgradeStar[MAX_NAMESIZE];
						upgradeStar[0] = 0L;	
						for( int k=0;k<gradeVale;k++ )
							strcat( upgradeStar, "★" );
						upgradeStar[strlen(upgradeStar)] = 0L;

						//Get grade type
						if ( pCmd->tobject_array[numItem].kind & 4 )
							gradeType = 2;
						else
						{
							if ( ( (pObject->dwType == ItemType_Resource)
								|| (pObject->dwType == ItemType_SOULSTONE) 
								|| (pObject->dwType == ItemType_LevelUp) )
								&& ( (pObject->dwID == 506 )
								|| (pObject->dwID == 507)
								|| (pObject->dwID == 516)
								|| (pObject->dwID == 517)
								|| (pObject->dwID == 526)
								|| (pObject->dwID == 527)
								|| (pObject->dwID == 536)
								|| (pObject->dwID == 537)
								|| (pObject->dwID == 546)
								|| (pObject->dwID == 547)
								|| (pObject->dwID == 548)
								|| (pObject->dwID == 550)
								|| (pObject->dwID == 561)
								|| (pObject->dwID == 562)
								|| (pObject->dwID == 563)
								|| (pObject->dwID == 564)
								|| (pObject->dwID == 565)
								|| (pObject->dwID == 852)
								|| (pObject->dwID == 877)
								|| (pObject->dwID == 957) ) )
							{
								gradeType = 1;
							}
							else
							{
								if ( ((pObject->dwID >= 677) && (pObject->dwType <= 683) && (pObject->dwID != 682))
									|| ((pObject->dwID >= 748) && (pObject->dwType <= 751)) )
								{
									if ( (pObject->dwID == 678) || (pObject->dwID == 679) )
										gradeType = 1;
								}
								else
									gradeType = 2;
							}
						}

						//This time we don't show grade
						gradeType = 0;

						//Split item name
						char* pName;
						char namePartOne[MAX_NAMESIZE];
						char namePartTwo[MAX_NAMESIZE];
						char nameFull[MAX_PATH];
						nameFull[0] = 0L;								

						pName = strpbrk( pCmd->tobject_array[numItem].strName, "0" );
						if ( pName )
						{
							sprintf( namePartOne, ++pName );
							int len = strlen(pCmd->tobject_array[numItem].strName)-strlen(namePartOne)-1;
							strncpy( namePartTwo, pCmd->tobject_array[numItem].strName, len );
							namePartTwo[len] = 0;
						}
						else
						{
							sprintf( namePartTwo, pCmd->tobject_array[numItem].strName );
							sprintf( namePartOne, "" );
						}
						//End split item name

						if ( strlen(namePartOne) > 0 )
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}												
							}
							else
							{
								sprintf(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}

							strcat(nameFull, "(" );
							strcat(nameFull, namePartTwo);
							strcat(nameFull, ")");												
						}
						else
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
							else
							{
								sprintf(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
						}

						TiXmlText* itemName = item->InsertEndChild( TiXmlText( nameFull ) )->ToText();
					}								
				}
				else
					TiXmlText* itemName = item->InsertEndChild( TiXmlText( "道具交易" ) )->ToText();

				numItem++ ;
			}

			pszStr += ++endPos - startPos;
			startPos = endPos;
		}
		else
		{
			if ( numItem >= pCmd->size )
			{
				//Add the last chat part
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), ( strlen(szCharMsg)-startPos ) );
				szChat[strlen(szCharMsg)-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
				break;
			}
			else
			{
				endPos++ ;
			}
		}
	}

	//soke 世界说话国家判断
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		TiXmlElement* strPart = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(strPart,"color",color);
		XML_SetNodeText(strPart,"[燕国玩家]");
	}
}

//soke 唐国玩家世界喊话（带物品链接）
void CChatManager::Format7ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	DWORD dwColor = GetChatColor(pCmd->dwType);
	char szCharMsg[512];
	strncpy(szCharMsg,pCmd->pstrChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	// then we get chat content
	char mediaSP[2];
	mediaSP[0] = (char)( ITEM_SPECIAL_CHAR >> 8 );
	mediaSP[1] = 0;			

	char* pszStr = pCmd->pstrChat;
	int numItem = 0;
	int startPos = 0;
	int endPos = 0;
	while ( endPos < strlen(szCharMsg) )
	{
		if ( (BYTE)szCharMsg[endPos] == (ITEM_SPECIAL_CHAR >> 8) )
		{
			//Add common msg before item
			if ( endPos > startPos )
			{
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), (endPos-startPos) );
				szChat[endPos-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
			}
			//Add an item
			if ( numItem < pCmd->size )
			{
				TiXmlElement* item = pPage->InsertEndChild( TiXmlElement( "shell" ) )->ToElement();
				item->SetAttribute( "obj", pCmd->tobject_array[numItem].dwObjectTempID );

				ObjectBase_t* pObject = GetObjectBase( pCmd->tobject_array[numItem].dwObjectID );
				if ( pObject )
				{
					if ( 0 )		//Use object base name
						TiXmlText* itemName = item->InsertEndChild( TiXmlText( pObject->strName ) )->ToText();
					else			//Use server given name
					{
						int gradeType = 0;		//0 nograde
						//1 等
						//2 ★
						int gradeVale = pCmd->tobject_array[numItem].upgrade;
						if ( gradeVale > c_maxItemUpgrade )
							gradeVale = c_maxItemUpgrade;

						char upgradeStar[MAX_NAMESIZE];
						upgradeStar[0] = 0L;	
						for( int k=0;k<gradeVale;k++ )
							strcat( upgradeStar, "★" );
						upgradeStar[strlen(upgradeStar)] = 0L;

						//Get grade type
						if ( pCmd->tobject_array[numItem].kind & 4 )
							gradeType = 2;
						else
						{
							if ( ( (pObject->dwType == ItemType_Resource)
								|| (pObject->dwType == ItemType_SOULSTONE) 
								|| (pObject->dwType == ItemType_LevelUp) )
								&& ( (pObject->dwID == 506 )
								|| (pObject->dwID == 507)
								|| (pObject->dwID == 516)
								|| (pObject->dwID == 517)
								|| (pObject->dwID == 526)
								|| (pObject->dwID == 527)
								|| (pObject->dwID == 536)
								|| (pObject->dwID == 537)
								|| (pObject->dwID == 546)
								|| (pObject->dwID == 547)
								|| (pObject->dwID == 548)
								|| (pObject->dwID == 550)
								|| (pObject->dwID == 561)
								|| (pObject->dwID == 562)
								|| (pObject->dwID == 563)
								|| (pObject->dwID == 564)
								|| (pObject->dwID == 565)
								|| (pObject->dwID == 852)
								|| (pObject->dwID == 877)
								|| (pObject->dwID == 957) ) )
							{
								gradeType = 1;
							}
							else
							{
								if ( ((pObject->dwID >= 677) && (pObject->dwType <= 683) && (pObject->dwID != 682))
									|| ((pObject->dwID >= 748) && (pObject->dwType <= 751)) )
								{
									if ( (pObject->dwID == 678) || (pObject->dwID == 679) )
										gradeType = 1;
								}
								else
									gradeType = 2;
							}
						}

						//This time we don't show grade
						gradeType = 0;

						//Split item name
						char* pName;
						char namePartOne[MAX_NAMESIZE];
						char namePartTwo[MAX_NAMESIZE];
						char nameFull[MAX_PATH];
						nameFull[0] = 0L;								

						pName = strpbrk( pCmd->tobject_array[numItem].strName, "0" );
						if ( pName )
						{
							sprintf( namePartOne, ++pName );
							int len = strlen(pCmd->tobject_array[numItem].strName)-strlen(namePartOne)-1;
							strncpy( namePartTwo, pCmd->tobject_array[numItem].strName, len );
							namePartTwo[len] = 0;
						}
						else
						{
							sprintf( namePartTwo, pCmd->tobject_array[numItem].strName );
							sprintf( namePartOne, "" );
						}
						//End split item name

						if ( strlen(namePartOne) > 0 )
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}												
							}
							else
							{
								sprintf(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}

							strcat(nameFull, "(" );
							strcat(nameFull, namePartTwo);
							strcat(nameFull, ")");												
						}
						else
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
							else
							{
								sprintf(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
						}

						TiXmlText* itemName = item->InsertEndChild( TiXmlText( nameFull ) )->ToText();
					}								
				}
				else
					TiXmlText* itemName = item->InsertEndChild( TiXmlText( "道具交易" ) )->ToText();

				numItem++ ;
			}

			pszStr += ++endPos - startPos;
			startPos = endPos;
		}
		else
		{
			if ( numItem >= pCmd->size )
			{
				//Add the last chat part
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), ( strlen(szCharMsg)-startPos ) );
				szChat[strlen(szCharMsg)-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
				break;
			}
			else
			{
				endPos++ ;
			}
		}
	}

	//soke 世界说话国家判断
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		TiXmlElement* strPart = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(strPart,"color",color);
		XML_SetNodeText(strPart,"[唐国玩家]");
	}
}

//soke 汉国玩家世界喊话（带物品链接）
void CChatManager::Format8ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	DWORD dwColor = GetChatColor(pCmd->dwType);
	char szCharMsg[512];
	strncpy(szCharMsg,pCmd->pstrChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	// then we get chat content
	char mediaSP[2];
	mediaSP[0] = (char)( ITEM_SPECIAL_CHAR >> 8 );
	mediaSP[1] = 0;			

	char* pszStr = pCmd->pstrChat;
	int numItem = 0;
	int startPos = 0;
	int endPos = 0;
	while ( endPos < strlen(szCharMsg) )
	{
		if ( (BYTE)szCharMsg[endPos] == (ITEM_SPECIAL_CHAR >> 8) )
		{
			//Add common msg before item
			if ( endPos > startPos )
			{
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), (endPos-startPos) );
				szChat[endPos-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
			}
			//Add an item
			if ( numItem < pCmd->size )
			{
				TiXmlElement* item = pPage->InsertEndChild( TiXmlElement( "shell" ) )->ToElement();
				item->SetAttribute( "obj", pCmd->tobject_array[numItem].dwObjectTempID );

				ObjectBase_t* pObject = GetObjectBase( pCmd->tobject_array[numItem].dwObjectID );
				if ( pObject )
				{
					if ( 0 )		//Use object base name
						TiXmlText* itemName = item->InsertEndChild( TiXmlText( pObject->strName ) )->ToText();
					else			//Use server given name
					{
						int gradeType = 0;		//0 nograde
						//1 等
						//2 ★
						int gradeVale = pCmd->tobject_array[numItem].upgrade;
						if ( gradeVale > c_maxItemUpgrade )
							gradeVale = c_maxItemUpgrade;

						char upgradeStar[MAX_NAMESIZE];
						upgradeStar[0] = 0L;	
						for( int k=0;k<gradeVale;k++ )
							strcat( upgradeStar, "★" );
						upgradeStar[strlen(upgradeStar)] = 0L;

						//Get grade type
						if ( pCmd->tobject_array[numItem].kind & 4 )
							gradeType = 2;
						else
						{
							if ( ( (pObject->dwType == ItemType_Resource)
								|| (pObject->dwType == ItemType_SOULSTONE) 
								|| (pObject->dwType == ItemType_LevelUp) )
								&& ( (pObject->dwID == 506 )
								|| (pObject->dwID == 507)
								|| (pObject->dwID == 516)
								|| (pObject->dwID == 517)
								|| (pObject->dwID == 526)
								|| (pObject->dwID == 527)
								|| (pObject->dwID == 536)
								|| (pObject->dwID == 537)
								|| (pObject->dwID == 546)
								|| (pObject->dwID == 547)
								|| (pObject->dwID == 548)
								|| (pObject->dwID == 550)
								|| (pObject->dwID == 561)
								|| (pObject->dwID == 562)
								|| (pObject->dwID == 563)
								|| (pObject->dwID == 564)
								|| (pObject->dwID == 565)
								|| (pObject->dwID == 852)
								|| (pObject->dwID == 877)
								|| (pObject->dwID == 957) ) )
							{
								gradeType = 1;
							}
							else
							{
								if ( ((pObject->dwID >= 677) && (pObject->dwType <= 683) && (pObject->dwID != 682))
									|| ((pObject->dwID >= 748) && (pObject->dwType <= 751)) )
								{
									if ( (pObject->dwID == 678) || (pObject->dwID == 679) )
										gradeType = 1;
								}
								else
									gradeType = 2;
							}
						}

						//This time we don't show grade
						gradeType = 0;

						//Split item name
						char* pName;
						char namePartOne[MAX_NAMESIZE];
						char namePartTwo[MAX_NAMESIZE];
						char nameFull[MAX_PATH];
						nameFull[0] = 0L;								

						pName = strpbrk( pCmd->tobject_array[numItem].strName, "0" );
						if ( pName )
						{
							sprintf( namePartOne, ++pName );
							int len = strlen(pCmd->tobject_array[numItem].strName)-strlen(namePartOne)-1;
							strncpy( namePartTwo, pCmd->tobject_array[numItem].strName, len );
							namePartTwo[len] = 0;
						}
						else
						{
							sprintf( namePartTwo, pCmd->tobject_array[numItem].strName );
							sprintf( namePartOne, "" );
						}
						//End split item name

						if ( strlen(namePartOne) > 0 )
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}												
							}
							else
							{
								sprintf(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}

							strcat(nameFull, "(" );
							strcat(nameFull, namePartTwo);
							strcat(nameFull, ")");												
						}
						else
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
							else
							{
								sprintf(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
						}

						TiXmlText* itemName = item->InsertEndChild( TiXmlText( nameFull ) )->ToText();
					}								
				}
				else
					TiXmlText* itemName = item->InsertEndChild( TiXmlText( "道具交易" ) )->ToText();

				numItem++ ;
			}

			pszStr += ++endPos - startPos;
			startPos = endPos;
		}
		else
		{
			if ( numItem >= pCmd->size )
			{
				//Add the last chat part
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), ( strlen(szCharMsg)-startPos ) );
				szChat[strlen(szCharMsg)-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
				break;
			}
			else
			{
				endPos++ ;
			}
		}
	}

	//soke 世界说话国家判断
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		TiXmlElement* strPart = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(strPart,"color",color);
		XML_SetNodeText(strPart,"[汉国玩家]");
	}
}

//soke 吴国玩家世界喊话（带物品链接）
void CChatManager::Format9ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	DWORD dwColor = GetChatColor(pCmd->dwType);
	char szCharMsg[512];
	strncpy(szCharMsg,pCmd->pstrChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	// then we get chat content
	char mediaSP[2];
	mediaSP[0] = (char)( ITEM_SPECIAL_CHAR >> 8 );
	mediaSP[1] = 0;			

	char* pszStr = pCmd->pstrChat;
	int numItem = 0;
	int startPos = 0;
	int endPos = 0;
	while ( endPos < strlen(szCharMsg) )
	{
		if ( (BYTE)szCharMsg[endPos] == (ITEM_SPECIAL_CHAR >> 8) )
		{
			//Add common msg before item
			if ( endPos > startPos )
			{
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), (endPos-startPos) );
				szChat[endPos-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
			}
			//Add an item
			if ( numItem < pCmd->size )
			{
				TiXmlElement* item = pPage->InsertEndChild( TiXmlElement( "shell" ) )->ToElement();
				item->SetAttribute( "obj", pCmd->tobject_array[numItem].dwObjectTempID );

				ObjectBase_t* pObject = GetObjectBase( pCmd->tobject_array[numItem].dwObjectID );
				if ( pObject )
				{
					if ( 0 )		//Use object base name
						TiXmlText* itemName = item->InsertEndChild( TiXmlText( pObject->strName ) )->ToText();
					else			//Use server given name
					{
						int gradeType = 0;		//0 nograde
						//1 等
						//2 ★
						int gradeVale = pCmd->tobject_array[numItem].upgrade;
						if ( gradeVale > c_maxItemUpgrade )
							gradeVale = c_maxItemUpgrade;

						char upgradeStar[MAX_NAMESIZE];
						upgradeStar[0] = 0L;	
						for( int k=0;k<gradeVale;k++ )
							strcat( upgradeStar, "★" );
						upgradeStar[strlen(upgradeStar)] = 0L;

						//Get grade type
						if ( pCmd->tobject_array[numItem].kind & 4 )
							gradeType = 2;
						else
						{
							if ( ( (pObject->dwType == ItemType_Resource)
								|| (pObject->dwType == ItemType_SOULSTONE) 
								|| (pObject->dwType == ItemType_LevelUp) )
								&& ( (pObject->dwID == 506 )
								|| (pObject->dwID == 507)
								|| (pObject->dwID == 516)
								|| (pObject->dwID == 517)
								|| (pObject->dwID == 526)
								|| (pObject->dwID == 527)
								|| (pObject->dwID == 536)
								|| (pObject->dwID == 537)
								|| (pObject->dwID == 546)
								|| (pObject->dwID == 547)
								|| (pObject->dwID == 548)
								|| (pObject->dwID == 550)
								|| (pObject->dwID == 561)
								|| (pObject->dwID == 562)
								|| (pObject->dwID == 563)
								|| (pObject->dwID == 564)
								|| (pObject->dwID == 565)
								|| (pObject->dwID == 852)
								|| (pObject->dwID == 877)
								|| (pObject->dwID == 957) ) )
							{
								gradeType = 1;
							}
							else
							{
								if ( ((pObject->dwID >= 677) && (pObject->dwType <= 683) && (pObject->dwID != 682))
									|| ((pObject->dwID >= 748) && (pObject->dwType <= 751)) )
								{
									if ( (pObject->dwID == 678) || (pObject->dwID == 679) )
										gradeType = 1;
								}
								else
									gradeType = 2;
							}
						}

						//This time we don't show grade
						gradeType = 0;

						//Split item name
						char* pName;
						char namePartOne[MAX_NAMESIZE];
						char namePartTwo[MAX_NAMESIZE];
						char nameFull[MAX_PATH];
						nameFull[0] = 0L;								

						pName = strpbrk( pCmd->tobject_array[numItem].strName, "0" );
						if ( pName )
						{
							sprintf( namePartOne, ++pName );
							int len = strlen(pCmd->tobject_array[numItem].strName)-strlen(namePartOne)-1;
							strncpy( namePartTwo, pCmd->tobject_array[numItem].strName, len );
							namePartTwo[len] = 0;
						}
						else
						{
							sprintf( namePartTwo, pCmd->tobject_array[numItem].strName );
							sprintf( namePartOne, "" );
						}
						//End split item name

						if ( strlen(namePartOne) > 0 )
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}												
							}
							else
							{
								sprintf(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}

							strcat(nameFull, "(" );
							strcat(nameFull, namePartTwo);
							strcat(nameFull, ")");												
						}
						else
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
							else
							{
								sprintf(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
						}

						TiXmlText* itemName = item->InsertEndChild( TiXmlText( nameFull ) )->ToText();
					}								
				}
				else
					TiXmlText* itemName = item->InsertEndChild( TiXmlText( "道具交易" ) )->ToText();

				numItem++ ;
			}

			pszStr += ++endPos - startPos;
			startPos = endPos;
		}
		else
		{
			if ( numItem >= pCmd->size )
			{
				//Add the last chat part
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), ( strlen(szCharMsg)-startPos ) );
				szChat[strlen(szCharMsg)-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
				break;
			}
			else
			{
				endPos++ ;
			}
		}
	}

	//soke 世界说话国家判断
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		TiXmlElement* strPart = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(strPart,"color",color);
		XML_SetNodeText(strPart,"[吴国玩家]");
	}
}

//soke 赵国玩家世界喊话（带物品链接）
void CChatManager::Format10ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	DWORD dwColor = GetChatColor(pCmd->dwType);
	char szCharMsg[512];
	strncpy(szCharMsg,pCmd->pstrChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	// then we get chat content
	char mediaSP[2];
	mediaSP[0] = (char)( ITEM_SPECIAL_CHAR >> 8 );
	mediaSP[1] = 0;			

	char* pszStr = pCmd->pstrChat;
	int numItem = 0;
	int startPos = 0;
	int endPos = 0;
	while ( endPos < strlen(szCharMsg) )
	{
		if ( (BYTE)szCharMsg[endPos] == (ITEM_SPECIAL_CHAR >> 8) )
		{
			//Add common msg before item
			if ( endPos > startPos )
			{
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), (endPos-startPos) );
				szChat[endPos-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
			}
			//Add an item
			if ( numItem < pCmd->size )
			{
				TiXmlElement* item = pPage->InsertEndChild( TiXmlElement( "shell" ) )->ToElement();
				item->SetAttribute( "obj", pCmd->tobject_array[numItem].dwObjectTempID );

				ObjectBase_t* pObject = GetObjectBase( pCmd->tobject_array[numItem].dwObjectID );
				if ( pObject )
				{
					if ( 0 )		//Use object base name
						TiXmlText* itemName = item->InsertEndChild( TiXmlText( pObject->strName ) )->ToText();
					else			//Use server given name
					{
						int gradeType = 0;		//0 nograde
						//1 等
						//2 ★
						int gradeVale = pCmd->tobject_array[numItem].upgrade;
						if ( gradeVale > c_maxItemUpgrade )
							gradeVale = c_maxItemUpgrade;

						char upgradeStar[MAX_NAMESIZE];
						upgradeStar[0] = 0L;	
						for( int k=0;k<gradeVale;k++ )
							strcat( upgradeStar, "★" );
						upgradeStar[strlen(upgradeStar)] = 0L;

						//Get grade type
						if ( pCmd->tobject_array[numItem].kind & 4 )
							gradeType = 2;
						else
						{
							if ( ( (pObject->dwType == ItemType_Resource)
								|| (pObject->dwType == ItemType_SOULSTONE) 
								|| (pObject->dwType == ItemType_LevelUp) )
								&& ( (pObject->dwID == 506 )
								|| (pObject->dwID == 507)
								|| (pObject->dwID == 516)
								|| (pObject->dwID == 517)
								|| (pObject->dwID == 526)
								|| (pObject->dwID == 527)
								|| (pObject->dwID == 536)
								|| (pObject->dwID == 537)
								|| (pObject->dwID == 546)
								|| (pObject->dwID == 547)
								|| (pObject->dwID == 548)
								|| (pObject->dwID == 550)
								|| (pObject->dwID == 561)
								|| (pObject->dwID == 562)
								|| (pObject->dwID == 563)
								|| (pObject->dwID == 564)
								|| (pObject->dwID == 565)
								|| (pObject->dwID == 852)
								|| (pObject->dwID == 877)
								|| (pObject->dwID == 957) ) )
							{
								gradeType = 1;
							}
							else
							{
								if ( ((pObject->dwID >= 677) && (pObject->dwType <= 683) && (pObject->dwID != 682))
									|| ((pObject->dwID >= 748) && (pObject->dwType <= 751)) )
								{
									if ( (pObject->dwID == 678) || (pObject->dwID == 679) )
										gradeType = 1;
								}
								else
									gradeType = 2;
							}
						}

						//This time we don't show grade
						gradeType = 0;

						//Split item name
						char* pName;
						char namePartOne[MAX_NAMESIZE];
						char namePartTwo[MAX_NAMESIZE];
						char nameFull[MAX_PATH];
						nameFull[0] = 0L;								

						pName = strpbrk( pCmd->tobject_array[numItem].strName, "0" );
						if ( pName )
						{
							sprintf( namePartOne, ++pName );
							int len = strlen(pCmd->tobject_array[numItem].strName)-strlen(namePartOne)-1;
							strncpy( namePartTwo, pCmd->tobject_array[numItem].strName, len );
							namePartTwo[len] = 0;
						}
						else
						{
							sprintf( namePartTwo, pCmd->tobject_array[numItem].strName );
							sprintf( namePartOne, "" );
						}
						//End split item name

						if ( strlen(namePartOne) > 0 )
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}												
							}
							else
							{
								sprintf(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}

							strcat(nameFull, "(" );
							strcat(nameFull, namePartTwo);
							strcat(nameFull, ")");												
						}
						else
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
							else
							{
								sprintf(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
						}

						TiXmlText* itemName = item->InsertEndChild( TiXmlText( nameFull ) )->ToText();
					}								
				}
				else
					TiXmlText* itemName = item->InsertEndChild( TiXmlText( "道具交易" ) )->ToText();

				numItem++ ;
			}

			pszStr += ++endPos - startPos;
			startPos = endPos;
		}
		else
		{
			if ( numItem >= pCmd->size )
			{
				//Add the last chat part
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), ( strlen(szCharMsg)-startPos ) );
				szChat[strlen(szCharMsg)-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
				break;
			}
			else
			{
				endPos++ ;
			}
		}
	}

	//soke 世界说话国家判断
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		TiXmlElement* strPart = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(strPart,"color",color);
		XML_SetNodeText(strPart,"[赵国玩家]");
	}
}

//soke 周国玩家世界喊话（带物品链接）
void CChatManager::Format11ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	DWORD dwColor = GetChatColor(pCmd->dwType);
	char szCharMsg[512];
	strncpy(szCharMsg,pCmd->pstrChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	// then we get chat content
	char mediaSP[2];
	mediaSP[0] = (char)( ITEM_SPECIAL_CHAR >> 8 );
	mediaSP[1] = 0;			

	char* pszStr = pCmd->pstrChat;
	int numItem = 0;
	int startPos = 0;
	int endPos = 0;
	while ( endPos < strlen(szCharMsg) )
	{
		if ( (BYTE)szCharMsg[endPos] == (ITEM_SPECIAL_CHAR >> 8) )
		{
			//Add common msg before item
			if ( endPos > startPos )
			{
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), (endPos-startPos) );
				szChat[endPos-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
			}
			//Add an item
			if ( numItem < pCmd->size )
			{
				TiXmlElement* item = pPage->InsertEndChild( TiXmlElement( "shell" ) )->ToElement();
				item->SetAttribute( "obj", pCmd->tobject_array[numItem].dwObjectTempID );

				ObjectBase_t* pObject = GetObjectBase( pCmd->tobject_array[numItem].dwObjectID );
				if ( pObject )
				{
					if ( 0 )		//Use object base name
						TiXmlText* itemName = item->InsertEndChild( TiXmlText( pObject->strName ) )->ToText();
					else			//Use server given name
					{
						int gradeType = 0;		//0 nograde
						//1 等
						//2 ★
						int gradeVale = pCmd->tobject_array[numItem].upgrade;
						if ( gradeVale > c_maxItemUpgrade )
							gradeVale = c_maxItemUpgrade;

						char upgradeStar[MAX_NAMESIZE];
						upgradeStar[0] = 0L;	
						for( int k=0;k<gradeVale;k++ )
							strcat( upgradeStar, "★" );
						upgradeStar[strlen(upgradeStar)] = 0L;

						//Get grade type
						if ( pCmd->tobject_array[numItem].kind & 4 )
							gradeType = 2;
						else
						{
							if ( ( (pObject->dwType == ItemType_Resource)
								|| (pObject->dwType == ItemType_SOULSTONE) 
								|| (pObject->dwType == ItemType_LevelUp) )
								&& ( (pObject->dwID == 506 )
								|| (pObject->dwID == 507)
								|| (pObject->dwID == 516)
								|| (pObject->dwID == 517)
								|| (pObject->dwID == 526)
								|| (pObject->dwID == 527)
								|| (pObject->dwID == 536)
								|| (pObject->dwID == 537)
								|| (pObject->dwID == 546)
								|| (pObject->dwID == 547)
								|| (pObject->dwID == 548)
								|| (pObject->dwID == 550)
								|| (pObject->dwID == 561)
								|| (pObject->dwID == 562)
								|| (pObject->dwID == 563)
								|| (pObject->dwID == 564)
								|| (pObject->dwID == 565)
								|| (pObject->dwID == 852)
								|| (pObject->dwID == 877)
								|| (pObject->dwID == 957) ) )
							{
								gradeType = 1;
							}
							else
							{
								if ( ((pObject->dwID >= 677) && (pObject->dwType <= 683) && (pObject->dwID != 682))
									|| ((pObject->dwID >= 748) && (pObject->dwType <= 751)) )
								{
									if ( (pObject->dwID == 678) || (pObject->dwID == 679) )
										gradeType = 1;
								}
								else
									gradeType = 2;
							}
						}

						//This time we don't show grade
						gradeType = 0;

						//Split item name
						char* pName;
						char namePartOne[MAX_NAMESIZE];
						char namePartTwo[MAX_NAMESIZE];
						char nameFull[MAX_PATH];
						nameFull[0] = 0L;								

						pName = strpbrk( pCmd->tobject_array[numItem].strName, "0" );
						if ( pName )
						{
							sprintf( namePartOne, ++pName );
							int len = strlen(pCmd->tobject_array[numItem].strName)-strlen(namePartOne)-1;
							strncpy( namePartTwo, pCmd->tobject_array[numItem].strName, len );
							namePartTwo[len] = 0;
						}
						else
						{
							sprintf( namePartTwo, pCmd->tobject_array[numItem].strName );
							sprintf( namePartOne, "" );
						}
						//End split item name

						if ( strlen(namePartOne) > 0 )
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}												
							}
							else
							{
								sprintf(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}

							strcat(nameFull, "(" );
							strcat(nameFull, namePartTwo);
							strcat(nameFull, ")");												
						}
						else
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
							else
							{
								sprintf(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
						}

						TiXmlText* itemName = item->InsertEndChild( TiXmlText( nameFull ) )->ToText();
					}								
				}
				else
					TiXmlText* itemName = item->InsertEndChild( TiXmlText( "道具交易" ) )->ToText();

				numItem++ ;
			}

			pszStr += ++endPos - startPos;
			startPos = endPos;
		}
		else
		{
			if ( numItem >= pCmd->size )
			{
				//Add the last chat part
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), ( strlen(szCharMsg)-startPos ) );
				szChat[strlen(szCharMsg)-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
				break;
			}
			else
			{
				endPos++ ;
			}
		}
	}

	//soke 世界说话国家判断
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		TiXmlElement* strPart = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(strPart,"color",color);
		XML_SetNodeText(strPart,"[周国玩家]");
	}
}

//soke 楚国玩家世界喊话（带物品链接）
void CChatManager::Format12ItemMsgContent(stChannelChatUserCmd* pCmd,TiXmlElement* pPage)
{
	FUNCTION_BEGIN;

	DWORD dwColor = GetChatColor(pCmd->dwType);
	char szCharMsg[512];
	strncpy(szCharMsg,pCmd->pstrChat,sizeof(szCharMsg));
	szCharMsg[sizeof(szCharMsg)-1] = 0;

	// then we get chat content
	char mediaSP[2];
	mediaSP[0] = (char)( ITEM_SPECIAL_CHAR >> 8 );
	mediaSP[1] = 0;			

	char* pszStr = pCmd->pstrChat;
	int numItem = 0;
	int startPos = 0;
	int endPos = 0;
	while ( endPos < strlen(szCharMsg) )
	{
		if ( (BYTE)szCharMsg[endPos] == (ITEM_SPECIAL_CHAR >> 8) )
		{
			//Add common msg before item
			if ( endPos > startPos )
			{
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), (endPos-startPos) );
				szChat[endPos-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
			}
			//Add an item
			if ( numItem < pCmd->size )
			{
				TiXmlElement* item = pPage->InsertEndChild( TiXmlElement( "shell" ) )->ToElement();
				item->SetAttribute( "obj", pCmd->tobject_array[numItem].dwObjectTempID );

				ObjectBase_t* pObject = GetObjectBase( pCmd->tobject_array[numItem].dwObjectID );
				if ( pObject )
				{
					if ( 0 )		//Use object base name
						TiXmlText* itemName = item->InsertEndChild( TiXmlText( pObject->strName ) )->ToText();
					else			//Use server given name
					{
						int gradeType = 0;		//0 nograde
						//1 等
						//2 ★
						int gradeVale = pCmd->tobject_array[numItem].upgrade;
						if ( gradeVale > c_maxItemUpgrade )
							gradeVale = c_maxItemUpgrade;

						char upgradeStar[MAX_NAMESIZE];
						upgradeStar[0] = 0L;	
						for( int k=0;k<gradeVale;k++ )
							strcat( upgradeStar, "★" );
						upgradeStar[strlen(upgradeStar)] = 0L;

						//Get grade type
						if ( pCmd->tobject_array[numItem].kind & 4 )
							gradeType = 2;
						else
						{
							if ( ( (pObject->dwType == ItemType_Resource)
								|| (pObject->dwType == ItemType_SOULSTONE) 
								|| (pObject->dwType == ItemType_LevelUp) )
								&& ( (pObject->dwID == 506 )
								|| (pObject->dwID == 507)
								|| (pObject->dwID == 516)
								|| (pObject->dwID == 517)
								|| (pObject->dwID == 526)
								|| (pObject->dwID == 527)
								|| (pObject->dwID == 536)
								|| (pObject->dwID == 537)
								|| (pObject->dwID == 546)
								|| (pObject->dwID == 547)
								|| (pObject->dwID == 548)
								|| (pObject->dwID == 550)
								|| (pObject->dwID == 561)
								|| (pObject->dwID == 562)
								|| (pObject->dwID == 563)
								|| (pObject->dwID == 564)
								|| (pObject->dwID == 565)
								|| (pObject->dwID == 852)
								|| (pObject->dwID == 877)
								|| (pObject->dwID == 957) ) )
							{
								gradeType = 1;
							}
							else
							{
								if ( ((pObject->dwID >= 677) && (pObject->dwType <= 683) && (pObject->dwID != 682))
									|| ((pObject->dwID >= 748) && (pObject->dwType <= 751)) )
								{
									if ( (pObject->dwID == 678) || (pObject->dwID == 679) )
										gradeType = 1;
								}
								else
									gradeType = 2;
							}
						}

						//This time we don't show grade
						gradeType = 0;

						//Split item name
						char* pName;
						char namePartOne[MAX_NAMESIZE];
						char namePartTwo[MAX_NAMESIZE];
						char nameFull[MAX_PATH];
						nameFull[0] = 0L;								

						pName = strpbrk( pCmd->tobject_array[numItem].strName, "0" );
						if ( pName )
						{
							sprintf( namePartOne, ++pName );
							int len = strlen(pCmd->tobject_array[numItem].strName)-strlen(namePartOne)-1;
							strncpy( namePartTwo, pCmd->tobject_array[numItem].strName, len );
							namePartTwo[len] = 0;
						}
						else
						{
							sprintf( namePartTwo, pCmd->tobject_array[numItem].strName );
							sprintf( namePartOne, "" );
						}
						//End split item name

						if ( strlen(namePartOne) > 0 )
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}												
							}
							else
							{
								sprintf(nameFull, namePartOne);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}

							strcat(nameFull, "(" );
							strcat(nameFull, namePartTwo);
							strcat(nameFull, ")");												
						}
						else
						{
							if ( strlen(nameFull) > 0 )
							{
								strcat(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
							else
							{
								sprintf(nameFull, namePartTwo);
								switch( gradeType )
								{
								case 0:
									break;
								case 1:
									strcat(nameFull, upgradeName[gradeVale]);
									break;
								case 2:
									strcat(nameFull, upgradeStar);
									break;
								}
							}
						}

						TiXmlText* itemName = item->InsertEndChild( TiXmlText( nameFull ) )->ToText();
					}								
				}
				else
					TiXmlText* itemName = item->InsertEndChild( TiXmlText( "道具交易" ) )->ToText();

				numItem++ ;
			}

			pszStr += ++endPos - startPos;
			startPos = endPos;
		}
		else
		{
			if ( numItem >= pCmd->size )
			{
				//Add the last chat part
				char szChat[MAX_CHATINFO];
				strncpy( szChat, (szCharMsg+startPos), ( strlen(szCharMsg)-startPos ) );
				szChat[strlen(szCharMsg)-startPos] = 0;
				TiXmlElement* strPart = pPage->InsertEndChild( TiXmlElement( "n" ) )->ToElement();
				XML_SetColorAttribute(strPart,"color",dwColor);
				XML_SetNodeText( strPart, szChat );
				break;
			}
			else
			{
				endPos++ ;
			}
		}
	}

	//soke 世界说话国家判断
	if (GetScene()->GetMainCharacter()->GetProperty()->country)
	{
		DWORD color = COLOR_ARGB(255,255,0,0);
		TiXmlElement* strPart = pPage->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(strPart,"color",color);
		XML_SetNodeText(strPart,"[楚国玩家]");
	}
}

void CChatManager::FilterChatMessage(stChannelChatUserCmd* pCmd)
{
	FUNCTION_BEGIN;
	char pstrName[MAX_NAMESIZE];
	char pstrChat[MAX_CHATINFO];
	memset(pstrName,0,sizeof(pstrName));
	memset(pstrChat,0,sizeof(pstrChat));
	if( G_PhraseFilterName(pCmd->pstrName,pstrName,MAX_NAMESIZE))
	{
		strncpy(pCmd->pstrName,pstrName,MAX_NAMESIZE);
	}
	if( G_PhraseFilterChat(pCmd->pstrChat,pstrChat,MAX_CHATINFO))
	{
		strncpy(pCmd->pstrChat,pstrChat,MAX_CHATINFO);
	}
}

#ifdef _DEBUG
void CChatChannel::Trace()
{
	Engine_WriteLogF("Channel:[id:%d]\n",m_id);
	for( size_t i = 0; i < m_members.size(); i++ )
	{
		Engine_WriteLogF("member:[%d,%s]\n",i,m_members[i].user.c_str());
	}	
}
#endif

#ifdef _DEBUG
void CChatManager::Trace()
{
	Engine_WriteLogF("%s m_Channels.size() = %d\n",__FUNCTION__,m_Channels.size());
	for ( tListChatChannelIt it = m_Channels.begin(); 
		it != m_Channels.end(); ++it )
	{
		CChatChannel* pChannel = (*it);
		pChannel->Trace();
	}
}
#endif