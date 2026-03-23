#include "public.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include ".\lordstrikelobbydlg.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "../xml_parse/XmlParseEx.h"

//#define _M_DEBUG
///////////////////////////////////////////////////////////////////////////////////////////////////
#define ID_TREE          2
#define ID_BUTTON_CLOSE  3
#define ID_BUTTON_OPTION 4
#define ID_BUTTON_EXIT   5
#define ID_MLTEXTBOX     6

#define ID_BUTTON_CHANGE_DOUZI 7
#define ID_BUTTON_MIN          8 


CGuiLordStrikeLobbyDlg::CGuiLordStrikeLobbyDlg()
: m_pTree(NULL)
{

}

CGuiLordStrikeLobbyDlg::~CGuiLordStrikeLobbyDlg()
{

}

void CGuiLordStrikeLobbyDlg::OnCreate(void)
{
	CGuiDialog::OnCreate();
	m_pTree = (CGuiTree*)GetControl(ID_TREE,GUI_CONTROL_LISTBOX);
	assert(m_pTree!=NULL);
	CGuiButton * pButton = GetButton(ID_BUTTON_OPTION);
	if(pButton)
	{
		pButton->SetEnabled(FALSE);
	}
	
	m_x = (GetDevice()->GetWidth() - GetWidth())/2;
	m_y = (GetDevice()->GetHeight() - GetHeight())/2;
	 
	LoadDescFromFile();


	pButton=GetButton(ID_BUTTON_CHANGE_DOUZI);
	if( pButton)
	{
		pButton->SetEnabled(false);
	}

}

void CGuiLordStrikeLobbyDlg::LoadDescFromFile()
{
	std::string strFileName = "datas\\lordstrike.xml";
	TiXmlDocument doc;
	Stream* pStream = OpenPackFileForRead("data\\datas.pak",strFileName.c_str());
	if(!pStream)
	{
		Engine_WriteLog("装载 lordstrike.xml 失败\n");
		return;
	}
	if(!LoadXMLFromStream(pStream,doc))
	{
		ClosePackFileForRead(pStream);
		Engine_WriteLog("解析 lordstrike.xml 失败\n");
		return;
	}
	TiXmlElement* ddz = doc.FirstChildElement("ddz");
	if(ddz)
	{
		TiXmlElement* Text = ddz->FirstChildElement("Text");
		while(Text)
		{
			const char* pword = Text->Attribute("word");
			if( pword  )
			{
				CGuiMLTextBox * pMLTExtBox = GetMLTextBox(ID_MLTEXTBOX);
				if(pMLTExtBox)
					pMLTExtBox->SetText(pword);
				break;
			}
		}
	}
	ClosePackFileForRead(pStream);

 
}

bool CGuiLordStrikeLobbyDlg::FindRoom(std::string strText,stRoom & outroom)
{
	for(std::list<stTreeNode>::iterator iter = m_TreeData.begin(); iter != m_TreeData.end(); ++iter)
	{
		stTreeNode  & treenode = *iter;
		for(std::list<stSpecialGame>::iterator iterGame = treenode.mListGame.begin();iterGame != treenode.mListGame.end();++iterGame)
		{
			stSpecialGame & game = *iterGame;
			for(std::list<stRoom>::iterator iterRoom = game.mListRoom.begin(); iterRoom != game.mListRoom.end(); iterRoom++)
			{
				const stRoom & room = *iterRoom;
				if(room.strRoomName == strText)
				{
					outroom = room;
                                        return true;
				}
			}

		}
	}
	return false;
}


int CGuiLordStrikeLobbyDlg::GetRoomNeedMoney(MiniRoomID roomid)
{
	stRoom room;
	if( FindRoom (roomid,room) )
	{
	    return room.nNeedMoney;
	}
	return 0;
}

bool CGuiLordStrikeLobbyDlg::FindRoom(MiniRoomID roomid,stRoom & outroom)
{
	for(std::list<stTreeNode>::iterator iter = m_TreeData.begin(); iter != m_TreeData.end(); ++iter)
	{
		stTreeNode  & treenode = *iter;
		for(std::list<stSpecialGame>::iterator iterGame = treenode.mListGame.begin();iterGame != treenode.mListGame.end();++iterGame)
		{
			stSpecialGame & game = *iterGame;
			for(std::list<stRoom>::iterator iterRoom = game.mListRoom.begin(); iterRoom != game.mListRoom.end(); iterRoom++)
			{
				stRoom & room = *iterRoom;
				if(room.roomid == roomid)
				{
					outroom = room;
					return true;
				}
			}
		}
	}
	return false;
}



bool CGuiLordStrikeLobbyDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID)
			{
			case ID_BUTTON_CLOSE:
			case ID_BUTTON_EXIT:
				{
					if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
					{
						GetGameGuiManager()->AddMessageBox("你现在有正在进行的游戏，请问你是否决定退出?",eAction_ExitLordStrikeLobbyAndGame,NULL, c_szMBOKCancel);	
					}
					else
					{
						GetGameGuiManager()->AddMessageBox("确定退出小游戏吗?",eAction_ExitLordStrikeLobby,NULL, c_szMBOKCancel);	
					}
					
				}
				break;
			case ID_BUTTON_CHANGE_DOUZI:
				{
					CLordStrikeManager::GetSingleton()->AddLordStrikeChangeMoneyDlg();
				}
				break;
			case ID_BUTTON_MIN:
				{
                                         CLordStrikeManager::GetSingleton()->ShowAllDialog(false);
					 CLordStrikeManager::GetSingleton()->UpdateHideBtn();
				}
				break;
			}
						
		}
		break;
	case EVENT_LISTBOX_ITEM_DBLCLK:
		{
			TreeItem * pItem = m_pTree->GetCurTreeItem();
			std::string strRoomName =  pItem->strText;
			const char * pstr = strRoomName.c_str();
			char * pchar = strstr(pstr,"(");
			if(pchar)
				*pchar = 0;
			std::string roomname = pstr;
			stRoom room;
			if( FindRoom(roomname,room))
			{
				DWORD dwNowDouzi = CLordStrikeManager::GetSingleton()->GetMainUserInfo().userdata.score.money;
				if(room.nNeedMoney > dwNowDouzi)
				{
					char szLevel[20]={0};
					if(room.type == eNormal)
					{
						strncpy(szLevel,"试炼场",20);
					}
					else if(room.type == eJunior)
					{
						strncpy(szLevel,"初级炼丹房",20);
					}
					else if(room.type == eMiddle)
					{
						strncpy(szLevel,"中级炼丹房",20);
					}
					char szError[128]={0};
					_snprintf(szError,128,"对不起，你的仙丹储备不足%d粒,不能进入%s!",room.nNeedMoney,szLevel);
					GameMessageBox(szError);
					break;
				}
				stEnterRoomCommonMiniGameCmd cmd;
				cmd.roomID = room.roomid;
				SEND_USER_CMD(cmd);
			}
			else 
			{
				//assert(0);
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

void CGuiLordStrikeLobbyDlg::OnClose(void)
{
	// 释放所有结构，退出Lobby	 
	CLordStrikeManager::GetSingleton()->m_guiLordStrikeLobby = NULL;
	stLogoutCommonMiniGameCmd cmd;
	SEND_USER_CMD(cmd);
}

std::string GetDescOfType(BYTE type)
{
	if(type == DOUDIZHU)
		return "斗地主";
	else if(type == CAISHUZI)
		return "猜数字";
	else 
		return "";
}

void CGuiLordStrikeLobbyDlg::RefreshRoom()
{
	//struct MiniRoomData
	//{
	//	DWORD roomID;  // 房间id
	//	WORD gameNum;  // 桌子数
	//	WORD oneGameUserNum; // 一个桌子最多多少人
	//	DWORD userNum;    // 当前房间用户数 《 oneGameUserNum * gameNum
	//};

	m_TreeData.clear();

	std::list<MiniRoomData> & roomlist = CLordStrikeManager::GetSingleton()->GetLobby()->GetRoomList();

     
	 stTreeNode node1;
	 node1.type = eGameType_Card;   // 牌类
	 node1.strType = "牌类游戏";

	 stSpecialGame stLordGame;
	 stLordGame.strGameName = GetDescOfType(DOUDIZHU);  // 斗地主

	
	 int iCommonRoomNum = 0;
	 int iJuniorRoomNum = 0;
	 int iMiddleRoomNum = 0;
	 int iSeniorRoomNum = 0;
	 for(std::list<MiniRoomData>::iterator iter = roomlist.begin() ; iter != roomlist.end(); iter++)
	 {
                  const MiniRoomData & room = *iter;
		  
		  // 游戏类型
		  if(room.roomID.type == DOUDIZHU)
		  {
			  stRoom _room;
			   char szRoomName[30];
			  /* 根据程序的建议
				   先补充房间名称设定规则
				   初级房间：1<= 基数 <10
				   中级房间：10<= 基数 < 50
				   (在此对程序的同志表示感谢！！！) */
			   if( room.money == 0)
			   {
				   iCommonRoomNum++;
				   _room.type = eNormal;
				   _snprintf(szRoomName,30,"试炼场%d",iCommonRoomNum);
			   }
			   else if( room.money < 10)//1.	初级炼丹房进入条件：仙宿丹 >= 30    （30两）
			   {
				   iJuniorRoomNum++;
				   _room.type = eJunior;
				   _snprintf(szRoomName,30,"初级炼丹房%d",iJuniorRoomNum);
			   }
			   else if(room.money < 50)
			   {
				   iMiddleRoomNum++;
				   _room.type = eMiddle;
				   _snprintf(szRoomName,30,"中级炼丹房%d",iMiddleRoomNum);
			   }
			   else 
			   {
				   iSeniorRoomNum++;
				   _room.type = eSenior;
				   _snprintf(szRoomName,30,"高级炼丹房%d",iSeniorRoomNum);
			   }
			  szRoomName[29] = 0;
		
			  _room.strRoomName = szRoomName;
			  _room.nCapatily =  room.oneGameUserNum * room.gameNum;
			  _room.nResideNum = room.userNum;
			  _room.roomid = room.roomID;
			  _room.nMaxTable = room.gameNum;
			  _room.nNeedMoney = room.money * 30;  // 条件是积分 >= 基数 * 30

			  stLordGame.mListRoom.push_back(_room);	  
		  }
	 }

	 // 写死了...
	 node1.mListGame.push_back(stLordGame);
	 m_TreeData.push_back(node1);

	 
	 /////////////////////////////////////////////////////
	if(!m_pTree)
		return;

	m_pTree->RemoveAllItems();

	DWORD clr = COLOR_ARGB(255,255,255,255);

	std :: list<stTreeNode>::const_iterator iterGameType;
	for(iterGameType =  m_TreeData.begin(); iterGameType != m_TreeData.end();  ++iterGameType)
	{
		const stTreeNode & node = *iterGameType;
		TreeItem * pGameType = m_pTree->AddChildItem(NULL,node.strType.c_str(),clr);
		if(pGameType)
		{
			std :: list<stSpecialGame>::const_iterator iterSpecialGame;
			for(iterSpecialGame = node.mListGame.begin(); iterSpecialGame != node.mListGame.end(); ++iterSpecialGame)
			{
				TreeItem * pSGame;
				const stSpecialGame & sgame=  *iterSpecialGame;
				pSGame = m_pTree->AddChildItem(pGameType,sgame.strGameName.c_str(),clr);

				std :: list<stRoom>::const_iterator iterRoom;
				for(iterRoom = sgame.mListRoom.begin(); iterRoom != sgame.mListRoom.end(); ++iterRoom)
				{
					const stRoom & room = *iterRoom;
					char szRoomDesc[40];
					_snprintf(szRoomDesc,39,"%s(%d/%d人)",room.strRoomName.c_str(),room.nResideNum,room.nCapatily);
					szRoomDesc[39] = 0;
					m_pTree->AddChildItem(pSGame,szRoomDesc,clr);
				}
			}
		}		
	}
}
