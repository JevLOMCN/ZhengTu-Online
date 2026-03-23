#include "public.h"
#include "miniusercommand.h"
#include "GameGuiManager.h"
#include "../Media/SoundManager.h"
#include "Game.h"
#include ".\lordstrikeroomdlg.h"
#include "LordStrikeGame.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "country.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define ID_ROOM_SCROLLBAR   1 
#define ID_ROOM_BUTTON_CLOSE 4
#define ID_ROOM_BUTTON_EXIT  7

#define ID_BUTTON_LOBBY 3 

#define ID_ROOM_BUTTON_SORT 5 

#define ID_BUTTON_OPTION  6

#define ID_BUTTON_ROOM_CHANGE_DZ 30 
// ScrollBar 
static int	g_nLeftVertDeltaPos	= 0;
static int	g_nOldLeftVertScrollPos = 0;



#define ID_BUTTON_START_TABLE_ID   1000

#define TABLE_OFFSET_X 200
#define TABLE_OFFSET_Y 150
#define TABLE_GAP_X    300   
#define TABLE_GAP_Y    200

//#define _DEBUG_M 
//#define _DEBUG_M__MM
//////////////////////////////////////////////////////////////////////////
CGuiLordStrikeRoomDlg::CGuiLordStrikeRoomDlg()
: m_pLeftVertScrollBar(NULL)
{
}

CGuiLordStrikeRoomDlg::~CGuiLordStrikeRoomDlg()
{
}


void CGuiLordStrikeRoomDlg::OnCreate(void)
{
	CGuiLordStrikeBaseDlg::OnCreate();


	m_pLeftVertScrollBar = (CGuiScrollBar *)GetControl(ID_ROOM_SCROLLBAR,GUI_CONTROL_SCROLLBAR);
	assert(m_pLeftVertScrollBar!=NULL);

 
	UpdateSrollRects();

	InitTableButtons();

	CGuiButton * pBtn = GetButton(ID_BUTTON_OPTION);
	if(pBtn)
	{
		pBtn->SetEnabled(false);
	}

#ifdef _DEBUG_M
	UpdateTableButtons();
#endif 


	m_dwCreateTime = xtimeGetTime();
	m_bOrderReset = true;
	//SetZOrder(GUI_DIALOG_TOPMOST);
	
	pBtn=GetButton(ID_BUTTON_ROOM_CHANGE_DZ);
	if( pBtn){
		pBtn->SetEnabled(false);
	}



}

void CGuiLordStrikeRoomDlg::NewTable(int nMaxTable)
{
	m_VecTable.clear();
	for(int i = 0 ; i < nMaxTable; i++)
	{
		stTableInfo info;
		info.tableid = i+1;//房间从1~maxid
		info.VecUserData.clear();
		m_VecTable.push_back(info);
	}

	UpdateTableButtons();

	////////////////////////////////////////////////////////////////////////////////////////
	// 再根据桌子列表刷新滚动条和按钮
	if ( m_pLeftVertScrollBar )
	{
		//m_pLeftVertScrollBar->SetStyle(CGuiScrollBar::NOSKIN);
		int rows = (m_VecTable.size() + ID_COL_TABLE_NUM-1) / ID_COL_TABLE_NUM;
		m_pLeftVertScrollBar->SetRange( 0, rows);
		m_pLeftVertScrollBar->SetValue( 0 );
		g_nOldLeftVertScrollPos = 0;
		m_pLeftVertScrollBar->SetPageSize(ID_ROW_TABLE_NUM);
		m_pLeftVertScrollBar->SetTrackRange(0,rows);
	}
}

void CGuiLordStrikeRoomDlg::InitTableButtons()
{
	for(size_t i = 0 ; i < ID_COL_TABLE_NUM * ID_ROW_TABLE_NUM; ++i)
	{
		// 从 中　左上　到　左下　到　下　到　右下　到　右上　到　上

		m_ButtonInfo[i].pTableButton = __AddButton(ID_BUTTON_START_TABLE_ID + i*50 + 0);

		m_ButtonInfo[i].pLeftButton1 =  __AddButton(ID_BUTTON_START_TABLE_ID + i*50 + 1);  // Left Button1 
		m_ButtonInfo[i].pLeftStatic1 =  __AddStatic(ID_BUTTON_START_TABLE_ID + i*50 + 2);  // Left static1
		m_ButtonInfo[i].pLeftImage1  =  __AddImage(ID_BUTTON_START_TABLE_ID  + i*50 + 3); //left iamge1　
		m_ButtonInfo[i].pLeftPrepareImage1  =  __AddImage(ID_BUTTON_START_TABLE_ID  + i*50 + 4); //left iamge1　


		m_ButtonInfo[i].pDownButton =  __AddButton(ID_BUTTON_START_TABLE_ID + i*50 + 5);  // down Button1 
		m_ButtonInfo[i].pDownStatic =  __AddStatic(ID_BUTTON_START_TABLE_ID + i*50 + 6);  // down static1
		m_ButtonInfo[i].pDownImage  =  __AddImage(ID_BUTTON_START_TABLE_ID  + i*50 + 7);  //down iamge1
		m_ButtonInfo[i].pDownPrepareImage  =  __AddImage(ID_BUTTON_START_TABLE_ID  + i*50 + 8); //left iamge1　


		m_ButtonInfo[i].pRightButton1 =  __AddButton(ID_BUTTON_START_TABLE_ID + i*50 + 9); 
		m_ButtonInfo[i].pRightStatic1 =  __AddStatic(ID_BUTTON_START_TABLE_ID + i*50 + 10);  
		m_ButtonInfo[i].pRightImage1  =  __AddImage(ID_BUTTON_START_TABLE_ID  + i*50 + 11);
		m_ButtonInfo[i].pRightPrepareImage1  =  __AddImage(ID_BUTTON_START_TABLE_ID  + i*50 + 12); //left iamge1　


		m_ButtonInfo[i].pTableIDStatic  =  __AddStatic(ID_BUTTON_START_TABLE_ID  + i*50 + 13);
	}
}


void CGuiLordStrikeRoomDlg::OnTableButtonClick(UINT nID)
{
       int nIndexTableOffset = (nID -  ID_BUTTON_START_TABLE_ID) / 50;
       int nLeftIndex = (nID - ID_BUTTON_START_TABLE_ID) % 50;

       if(nLeftIndex == 0 )
       {// 点在桌子上，不处里
	       return;
       }

       if( nLeftIndex != 1 &&
	   nLeftIndex != 5 &&
	   nLeftIndex != 9)
	       return;
     
	int nCurLine = m_pLeftVertScrollBar->GetValue();
	int nCurTableIndex = nCurLine  * ID_COL_TABLE_NUM;
	nCurTableIndex += nIndexTableOffset;
	assert(nCurTableIndex < m_VecTable.size());

	stTableInfo & tableinfo = m_VecTable[nCurTableIndex];
        
        int nDirection = (nLeftIndex+4-1)/4; // 这是按钮和方位的索引关系

	assert(nDirection == 1 || nDirection == 2 || nDirection == 3);
        int nIndex = tableinfo.GetIndex(nDirection);
	if(nIndex == -1 )
	{// 没有人，坐下
		stEnterGameCommonMiniGameCmd cmd;
		MiniRoomID id = CLordStrikeManager::GetSingleton()->GetLobby()->GetCurRommInfo().id;
		id.game = nCurTableIndex+1;
	        id.seat = nDirection;
		cmd.seatID = id;
		SEND_USER_CMD(cmd);
		// 等待确认登陆成功消息

		CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo().mSeatList.clear();
		for(int i = 0 ; i < m_VecTable.size() ; i++)
		{
			stTableInfo info = m_VecTable[i];
			if( info.tableid == nCurTableIndex+1)
			{
				for(int j = 0 ; j < info.vecSeat.size() ; j++)
				{
					CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo().mSeatList.push_back(info.vecSeat[j]);
				}
			}
		}
	}
}


void CGuiLordStrikeRoomDlg::ShowTableButtons(bool bShow)
{
	for(size_t i = 0 ; i < ID_COL_TABLE_NUM * ID_ROW_TABLE_NUM; ++i)
	{
		stButtonInfo & btngroup = m_ButtonInfo[i];

		btngroup.pLeftButton1->SetVisible(bShow);
		btngroup.pLeftStatic1->SetVisible(bShow);
		btngroup.pLeftImage1->SetVisible(bShow);
		btngroup.pLeftPrepareImage1->SetVisible(bShow);

		btngroup.pDownButton->SetVisible(bShow);
		btngroup.pDownStatic->SetVisible(bShow);
		btngroup.pDownImage->SetVisible(bShow);
		btngroup.pDownPrepareImage->SetVisible(bShow);

		btngroup.pTableButton->SetVisible(bShow);
		btngroup.pTableIDStatic->SetVisible(bShow);

		btngroup.pRightButton1->SetVisible(bShow);
		btngroup.pRightStatic1->SetVisible(bShow);
		btngroup.pRightImage1->SetVisible(bShow);
		btngroup.pRightPrepareImage1->SetVisible(bShow);
	}
}

void CGuiLordStrikeRoomDlg::ClearAllTable()
{
	m_VecTable.clear();
}

// 
void CGuiLordStrikeRoomDlg::UpdateTable()
{
	FUNCTION_BEGIN;


	// 先把桌子上的所有人物去掉，重新从大厅数据里面获取
	for(size_t i = 0; i < m_VecTable.size(); ++i)
	{
		stTableInfo & info = m_VecTable[i];
		for(size_t j = 0 ; j < info.VecUserData.size() ;++j)
		{
			info.VecUserData.clear();
		}			
		for(j = 0; j < info.vecSeat.size(); j++)
			info.vecSeat.clear();
	}

	// 先将玩家列表转成桌子列表
	stRoomInfo & roominfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetCurRommInfo();
	std::list<MiniUserData>::iterator iter = roominfo.mListUserData.begin();

	for(iter = roominfo.mListUserData.begin();iter != roominfo.mListUserData.end();++iter)
	{
		MiniUserData & user = *iter;
		DWORD tableid = user.pos.game;

		// 记录指针到自己的结构里面
		for(int i = 0; i < m_VecTable.size(); ++i)
		{
			stTableInfo & info = m_VecTable[i];
			if(info.tableid == tableid)
			{
				info.VecUserData.push_back(&user);
				break;
			}
		}
		if(i== m_VecTable.size())
		{//不在任何桌子
			continue;
		}
		//assert( i != m_VecTable.size() );
	}
	std::vector<MiniSeatData >::iterator iter2;
	for(iter2 = roominfo.mSeatList.begin();iter2 != roominfo.mSeatList.end();++iter2)
	{
		MiniSeatData & seatdata = *iter2;
		DWORD tableid = seatdata.seatID.game;

		// 记录指针到自己的结构里面
		for(int i = 0; i < m_VecTable.size(); ++i)
		{
			stTableInfo & info = m_VecTable[i];
			if(info.tableid == tableid)
			{
				info.vecSeat.push_back(&seatdata);
				break;
			}
		}
		if(i== m_VecTable.size())
		{//不在任何桌子
			continue;
		}
		//assert( i != m_VecTable.size() );
	}

	if(IsVisible())
	{
		ShowTableButtons(false);
		UpdateTableButtons();
		UpdateListBox();
	}	

	FUNCTION_END;
}

void CGuiLordStrikeRoomDlg::UpdatePlayerScore(stGameResultCommonMiniGameCmd * cmd)
{	
	stRoomInfo & roomInfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetCurRommInfo();
	std::list<MiniUserData>::iterator iter = roomInfo.mListUserData.begin();
	for(;iter != roomInfo.mListUserData.end();++iter)
	{
		MiniUserData & user = *iter;
		if(user.id == cmd->userID)
		{
			user.score += cmd->score;
			break;
		}
	}
	UpdateListBox();
}

void CGuiLordStrikeRoomDlg::UpdateListBox()
{
	CGuiListBox * pListBox = GetListBox(ID_LISTBOX_PLAYER);
	assert(pListBox != NULL);
	if(!pListBox)
		return;

	pListBox->RemoveAllItems();
	stRoomInfo & roomInfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetCurRommInfo();

	std::list<MiniUserData>::iterator iter = roomInfo.mListUserData.begin();
	for(; iter != roomInfo.mListUserData.end(); ++iter)
	{
		MiniUserData & info = *iter;
		pListBox->AddItem("",&info);
		int append_index = pListBox->GetItemCount()-1;

		
		std::string str = info.name;
		std::string str2;
		if(str.length() > 10)
			str2 = str.substr(0,10) + "...";
		else 
			str2 = str;
		pListBox->SetItemText2(append_index,0,str2.c_str(),99,GuiListBoxColFmt_Left) ; // ??é???

		//pListBox->SetItemText2(append_index,0,info.name,99,GuiListBoxColFmt_Center) ; // 角色名

		pListBox->SetItemText2(append_index,1,"-",75,GuiListBoxColFmt_Center);  // 等级

		char szTest[40]={0};
		_snprintf(szTest,40,"%d",info.score.score);
		szTest[39] = 0;
		pListBox->SetItemText2(append_index,2,szTest,100,GuiListBoxColFmt_Center);  /// 积分

		pListBox->SetItemText2(append_index,3,"-",65,GuiListBoxColFmt_Center); ///排名

		_snprintf(szTest,40,"%d",info.score.win);
		szTest[39] = 0;
		pListBox->SetItemText2(append_index,4,szTest,46,GuiListBoxColFmt_Center); ///胜


		_snprintf(szTest,40,"%d",info.score.lose);
		szTest[39] = 0;
		pListBox->SetItemText2(append_index,5,szTest,49,GuiListBoxColFmt_Center); ///负

		_snprintf(szTest,40,"%d",info.score.draw);
		szTest[39] = 0;
		pListBox->SetItemText2(append_index,6,szTest,52,GuiListBoxColFmt_Center); ///和

		char szOut[80] = {0};
		if(!CCountry::Instance()->GetCountryName(info.countryID,szOut))
			szOut[0] = 0;
		pListBox->SetItemText2(append_index,7,szOut,70,GuiListBoxColFmt_Center); ///国家

		pListBox->SetItemText2(append_index,8,"",119,GuiListBoxColFmt_Center); ///官职
	}	
}

void CGuiLordStrikeRoomDlg::UpdateTableButtons()
{
	int nCurLine = m_pLeftVertScrollBar->GetValue();
	int nCurIconIndex = nCurLine  * ID_COL_TABLE_NUM;


	int j = nCurIconIndex;
	int i = 0 ; 
	while( j < m_VecTable.size()  &&  j < (nCurIconIndex + ID_COL_TABLE_NUM * ID_ROW_TABLE_NUM) && (i < ID_COL_TABLE_NUM * ID_ROW_TABLE_NUM))
	{	 
		stTableInfo & tableinfo = m_VecTable[j];

		stButtonInfo * pButtonInfo = &m_ButtonInfo[i];

		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");
		rl.group = 132;	

		///////////////////////////////////////////////////////////////////////////////
		// TableButton 
		if(!tableinfo.IsTableStartState()){// 还没有开始的桌子
			rl.frame = 18;
		}
		else {
			rl.frame = 17;
		}
                int x = TABLE_OFFSET_X + i%ID_COL_TABLE_NUM * TABLE_GAP_X;
		int y = TABLE_OFFSET_Y + i/ID_COL_TABLE_NUM * TABLE_GAP_Y;
		
		stPointF scale1(1.0f,1.0f);
		stPointF scale2(0.6f,0.6f);
		__SetButtonBitmap(pButtonInfo->pTableButton,rl,x,y,scale1);
		
		///////////////////////////////////////////////////////////////////////////////
		// Table ID 
		char szTableID[20];
		_snprintf(szTableID,20,"%d号",tableinfo.tableid);
		szTableID[19] = 0;
		__SetStatic(pButtonInfo->pTableIDStatic,szTableID,x-80,y-40);


		///////////////////////////////////////////////////////////////////////////////
		// pLeft
		x = TABLE_OFFSET_X + i%ID_COL_TABLE_NUM * TABLE_GAP_X - 42;
		y = TABLE_OFFSET_Y + i/ID_COL_TABLE_NUM * TABLE_GAP_Y;

		int nIndex = tableinfo.GetLeftIndex();

		MiniUserState state = MUS_NOTPLAY;
		if(nIndex != -1)
			state  = tableinfo.GetUserState(1);
		if( nIndex != -1 &&  (state == MUS_SEAT || state == MUS_READY || MUS_PLAY == state))
		{		
			MiniUserData *pUserData = tableinfo.VecUserData[nIndex];  // 桌子上的玩家列表
			rl.group = 33;
			if(pUserData && pUserData->face >= 1 && pUserData->face <= 28)
			{
				rl.frame = pUserData->face;
			}
			else 
				rl.frame = 1;
			
			__SetButtonBitmap(pButtonInfo->pLeftButton1,rl,x,y,scale2);	
		}
		else if(tableinfo.IsSeatClosed(1))
		{
			rl.group = 132;
			rl.frame = 20;	
			__SetButtonBitmap(pButtonInfo->pLeftButton1,rl,x,y,scale1);
		}
		else 
		{
			rl.group = 132;
			rl.frame = 19;	 
			__SetButtonBitmap(pButtonInfo->pLeftButton1,rl,x,y,scale1);
		}
		if(nIndex != -1)
		{
			MiniUserData *pUserData = tableinfo.VecUserData[nIndex];  // 桌子上的玩家列表
			if( pUserData )
				__SetStatic(pButtonInfo->pLeftStatic1,pUserData->name,x-25,y-15);
		//		__SetStatic(pButtonInfo->pLeftStatic1,"符至阿道夫",x-25,y-15);
			if(pUserData  )
			{
				BYTE state = pUserData->state;
				if(state == MUS_SEAT || state == MUS_READY || MUS_PLAY == state)
				{
					rl.group = 132;
					rl.frame = 22;
					__SetImage(pButtonInfo->pLeftPrepareImage1,rl,x+40,y);
				}
			}
		}
 
		//////////////////////////////////////////////////////////////////////////////
		// pDownbutton
		x = TABLE_OFFSET_X + i%ID_COL_TABLE_NUM * TABLE_GAP_X+22;
		y = TABLE_OFFSET_Y + i/ID_COL_TABLE_NUM * TABLE_GAP_Y+80;
		nIndex = tableinfo.GetDownIndex();
		state = MUS_NOTPLAY;
		if(nIndex != -1)
			state  = tableinfo.GetUserState(2);
		if( nIndex != -1 &&  (state == MUS_SEAT || state == MUS_READY || MUS_PLAY == state))
		{		
			rl.group = 33;
			MiniUserData *pUserData = tableinfo.VecUserData[nIndex];  // 桌子上的玩家列表
			if(pUserData && pUserData->face >= 1 && pUserData->face <= 28)
			{
				rl.frame = pUserData->face;
			}
			else 
				rl.frame = 1;
			__SetButtonBitmap(pButtonInfo->pDownButton,rl,x,y,scale2);
		}
		else if(tableinfo.IsSeatClosed(2))
		{
			rl.group = 132;
			rl.frame = 20;	
			__SetButtonBitmap(pButtonInfo->pDownButton,rl,x,y,scale1);
		}
		else
		{
			rl.group = 132;
			rl.frame = 19;	
			__SetButtonBitmap(pButtonInfo->pDownButton,rl,x,y,scale1);
		}
		
		if(nIndex != -1)
		{
			MiniUserData *pUserData = tableinfo.VecUserData[nIndex];  // 桌子上的玩家列表
			if(pUserData)
				__SetStatic(pButtonInfo->pDownStatic,pUserData->name,x-25,y+40);
				//__SetStatic(pButtonInfo->pDownStatic,"符至阿道夫",x-25,y+40);
			if(pUserData  )
			{
				BYTE state = pUserData->state;
				if(state == MUS_SEAT || state == MUS_READY || MUS_PLAY == state)
				{
					rl.group = 132;
					rl.frame = 22;
					__SetImage(pButtonInfo->pDownPrepareImage,rl,x,y-20);
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////////
		// right up  
		nIndex = tableinfo.GetRightIndex();
		x = TABLE_OFFSET_X + i%ID_COL_TABLE_NUM * TABLE_GAP_X+84;
		y = TABLE_OFFSET_Y + i/ID_COL_TABLE_NUM * TABLE_GAP_Y;
		state = MUS_NOTPLAY;
		if(nIndex != -1)
			state  = tableinfo.GetUserState(3);
		if( nIndex != -1 && (state == MUS_SEAT || state == MUS_READY || MUS_PLAY == state))
		{		
			rl.group = 33;
			MiniUserData *pUserData = tableinfo.VecUserData[nIndex];  // 桌子上的玩家列表
			if(pUserData && pUserData->face >= 1 && pUserData->face <= 28)
			{
				rl.frame = pUserData->face;
			}
			else 
				rl.frame = 1;
			__SetButtonBitmap(pButtonInfo->pRightButton1,rl,x,y,scale2);	
		}
		else if(tableinfo.IsSeatClosed(3))
		{
			rl.group = 132;
			rl.frame = 20;	
			__SetButtonBitmap(pButtonInfo->pRightButton1,rl,x,y,scale1);
		}
		else
		{
			rl.group = 132;
			rl.frame = 19;	
			__SetButtonBitmap(pButtonInfo->pRightButton1,rl,x,y,scale1);
		}
		if(nIndex != -1)
		{
			MiniUserData *pUserData = tableinfo.VecUserData[nIndex];  // 桌子上的玩家列表
			if(pUserData)
				__SetStatic(pButtonInfo->pRightStatic1,pUserData->name,x-25,y-15);
				//__SetStatic(pButtonInfo->pRightStatic1,"桌子上的",x-25,y-15);
			if(pUserData  )
			{	
				BYTE state = pUserData->state;
				if(state == MUS_SEAT || state == MUS_READY || MUS_PLAY == state)
				{
					rl.group = 132;
					rl.frame = 22;
					__SetImage(pButtonInfo->pRightPrepareImage1,rl,x-20,y);
				}
			}
		}
		j++;
		i++;
	}
}


bool CGuiLordStrikeRoomDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			if( nID >= ID_BUTTON_START_TABLE_ID)
			{
				OnTableButtonClick(nID);
				break;
			}
			switch(nID)
			{
			case ID_ROOM_BUTTON_SORT:
				{
					stReqTopCommonMiniGameCmd cmd;
					SEND_USER_CMD(cmd);
					LSDlg_ButtonSetEnable(ID_ROOM_BUTTON_SORT,false);
					LSDlg_ButtonSetEnableDelay(ID_ROOM_BUTTON_SORT,true);
				}
				break;
			case ID_ROOM_BUTTON_CLOSE:
				{
					CLordStrikeManager::GetSingleton()->GetLobby()->MeLeaveRoom();
					//GetGameGuiManager()->AddMessageBox("确定退出房间吗?",eAction_ExitLordStrikeRoom,NULL, c_szMBOKCancel);					
				}
				break;
			case ID_ROOM_BUTTON_EXIT:
				{
					 GetGameGuiManager()->AddMessageBox("确定退出整个游戏吗?",eAction_ExitLordStrikeGameFromRoom,NULL, c_szMBOKCancel);
				}
				break;
			case ID_BUTTON_LOBBY:
				{
					CLordStrikeManager::GetSingleton()->GetLobby()->MeLeaveRoom();
				}
				break;
			case ID_BUTTON_ROOM_CHANGE_DZ:
				{
					CLordStrikeManager::GetSingleton()->AddLordStrikeChangeMoneyDlg();
				}
				break;
			}			 
		}
		break;
	}
	return  CGuiLordStrikeBaseDlg::OnGuiEvent(nEvent,nID,pControl);
}

void CGuiLordStrikeRoomDlg::OnClose()
{
	CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom = NULL;
}

void CGuiLordStrikeRoomDlg::InitScrollBar()
{
	FUNCTION_BEGIN;

	//Get scroll bar,must first call InitIcon() to set the scrollpos 
	m_pLeftVertScrollBar = (CGuiScrollBar*)GetControl( ID_ROOM_SCROLLBAR, GUI_CONTROL_SCROLLBAR );
	if ( m_pLeftVertScrollBar )
	{
		m_pLeftVertScrollBar->SetStyle(CGuiScrollBar::NOSKIN);
		m_pLeftVertScrollBar->SetValue( 0 );
		g_nOldLeftVertScrollPos = 0;
	}

	CGuiLordStrikeBaseDlg::InitScrollBar();

	FUNCTION_END;
}

void CGuiLordStrikeRoomDlg::UpdateSrollRects()
{
	FUNCTION_BEGIN;

	if( m_pLeftVertScrollBar )
	{
		stRectI rect(697,92,712,708);
		m_pLeftVertScrollBar->SetLocation( rect.left, rect.top );
		m_pLeftVertScrollBar->SetSize( 16, rect.Height() );
	}

	CGuiLordStrikeBaseDlg::UpdateSrollRects();

	FUNCTION_END;
}

bool CGuiLordStrikeRoomDlg::MsgProc(HWND hWnd,UINT uMsg,WPARAM wParam, LPARAM lParam)
{

	if( uMsg == WM_MOUSEWHEEL)
	{
		int delta = (short) HIWORD(wParam) / WHEEL_DELTA * 10;
		if ( m_pLeftVertScrollBar )
		{
			m_pLeftVertScrollBar->SetValue( m_pLeftVertScrollBar->GetValue() - delta );
			return true;
		}
	}
	return CGuiLordStrikeBaseDlg::MsgProc(hWnd,uMsg,wParam,lParam);
              
}

void CGuiLordStrikeRoomDlg::OnLeftMouseWheel()
{
	FUNCTION_BEGIN;

	if ( abs(g_nLeftVertDeltaPos) >= 1)
	{
		if( m_pLeftVertScrollBar )
		{
			UpdateTable();
			g_nOldLeftVertScrollPos = m_pLeftVertScrollBar->GetValue();
		}
	}


	FUNCTION_END;
}

HRESULT CGuiLordStrikeRoomDlg::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;

	if ( m_pLeftVertScrollBar ){
		g_nLeftVertDeltaPos = m_pLeftVertScrollBar->GetValue() - g_nOldLeftVertScrollPos;
	}

	OnLeftMouseWheel();

	HRESULT hr = CGuiLordStrikeBaseDlg::OnRender(fElapsedTime);

	//Draw scroll bar
	if ( m_pLeftVertScrollBar )
		m_pLeftVertScrollBar->Render( fElapsedTime );

	/*if( m_bOrderReset )
	{
		if( (int)(xtimeGetTime() - m_dwCreateTime) >= 1000 )
		{
			m_bOrderReset = false;
			SetZOrder(GUI_DIALOG_TOP);
		}
	}*/
	
	return hr;	

	FUNCTION_END;
}

void CGuiLordStrikeRoomDlg::SendChat(std::string strChat)
{
	stChatCommonMiniGameCmd cmd;
	cmd.userID = 0 ;// noused ?
	cmd.pos = CLordStrikeManager::GetSingleton()->GetLobby()->GetCurRommInfo().id;
	cmd.type = MCT_NORMAL;
	_snprintf(cmd.content,64,"%s",strChat.c_str());
	cmd.content[63] = 0;
	SEND_USER_CMD(cmd);
}
