#include "public.h"
#include "miniusercommand.h"
#include "GameGuiManager.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "LSScoreDlg.h"

//-------------------------------------------------------------------------------------------
// CGuiLordStrikeSortDlg
//-------------------------------------------------------------------------------------------
#define ID_GUILSSCORE_LISTBOX        2 
#define ID_GUILSSCORE_BUTTON_OK      10   // ok
 

CGuiLSScoreDlg::CGuiLSScoreDlg()
{
	m_bSmDialog = TRUE;
}

void CGuiLSScoreDlg::OnClose(void)
{
	FUNCTION_BEGIN;
	CLordStrikeManager::GetSingleton()->m_guiLSScoreDlg = NULL;
	FUNCTION_END;
}

void CGuiLSScoreDlg::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

#ifdef _DEBUG
	/*UpdataList("testw11111",100,200,300,400);
	UpdataList("testw11111",100,200,300,400);
	UpdataList("testw11111",100,200,300,400);
	UpdataList("testw11111",100,200,300,400);
	UpdataList("testw11111",100,200,300,400);
	UpdataList("testw11111",100,200,300,400);*/
#endif 
	m_x = (GetDevice()->GetWidth() - GetWidth())/2;
	m_y = (GetDevice()->GetHeight() - GetHeight())/2;

	FUNCTION_END;
}

void CGuiLSScoreDlg::UpdataList(std::string strText,int nSingleScore,int nSingleMoney,int nTotalScore,int nTotalMoeny)
{

	CGuiListBox * pListBox = GetListBox(ID_GUILSSCORE_LISTBOX);
	if( !pListBox )		
		return;
	pListBox->AddItem("",NULL);
	pListBox->SetItemText(pListBox->GetCount()-1,0,strText.c_str(),15);


	char szChat[30] = {0};
	_snprintf(szChat,30,"%d",nSingleScore);
	szChat[29] = 0;
	pListBox->SetItemText(pListBox->GetCount()-1,1,szChat,10);

	_snprintf(szChat,30,"%d",nSingleMoney);
	szChat[29] = 0;
	pListBox->SetItemText(pListBox->GetCount()-1,2,szChat,10);


	_snprintf(szChat,30,"%d",nTotalScore);
	szChat[29] = 0;
	pListBox->SetItemText(pListBox->GetCount()-1,3,szChat,10);

	_snprintf(szChat,30,"%d",nTotalMoeny);
	szChat[29] = 0;
	pListBox->SetItemText(pListBox->GetCount()-1,4,szChat,10);


}


void CGuiLSScoreDlg::UpdataList(DWORD nUserID,const MiniGameScore & SingleScore,const MiniGameScore & TotalScore)
{
	CGuiListBox * pListBox = GetListBox(ID_GUILSSCORE_LISTBOX);
	if( !pListBox )		
		return;
	pListBox->AddItem("",NULL);

	std::string strName;
	CLordStrikeManager::GetSingleton()->GetLobby()->GetCurRommInfo().GetUserName(nUserID,strName);
	pListBox->SetItemText(pListBox->GetCount()-1,0,strName.c_str(),15);

	char szChat[30] = {0};
	_snprintf(szChat,30,"%d",SingleScore.score);
	szChat[29] = 0;
	pListBox->SetItemText(pListBox->GetCount()-1,1,szChat,10);

 	_snprintf(szChat,30,"%d",SingleScore.money);
	szChat[29] = 0;
	pListBox->SetItemText(pListBox->GetCount()-1,2,szChat,10);


	_snprintf(szChat,30,"%d",TotalScore.score);
	szChat[29] = 0;
	pListBox->SetItemText(pListBox->GetCount()-1,3,szChat,10);

	_snprintf(szChat,30,"%d",TotalScore.money);
	szChat[29] = 0;
	pListBox->SetItemText(pListBox->GetCount()-1,4,szChat,10);
}

bool CGuiLSScoreDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) 
			{
			case ID_GUILSSCORE_BUTTON_OK:	
				{ 
					Close();
				} 
				break;
			}
		}
		break;

	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}


///////////////////////////////////////////////////////////////////////////////////

CGuiLordStrikeSortDlg::CGuiLordStrikeSortDlg()
{
	m_bSmDialog = TRUE;
}

void CGuiLordStrikeSortDlg::OnClose(void)
{
	FUNCTION_BEGIN;
	CLordStrikeManager::GetSingleton()->m_guiSortDlg = NULL;
	FUNCTION_END;
}

void CGuiLordStrikeSortDlg::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	m_x = (GetDevice()->GetWidth() - GetWidth())/2;
	m_y = (GetDevice()->GetHeight() - GetHeight())/2;

#ifdef _DEBUG
	/* std::list<MiniUserData> & sortlist = CLordStrikeManager::GetSingleton()->GetLobby()->GetSortList();
	 for(int i = 0 ; i < 100; i++)
	 {
		 MiniUserData user;
		 strcpy(user.name,"飞洒大发送颠覆");
		 user.score.win = 100;
		 user.score.lose = 10;
		 user.score.draw = 99;
		 user.score.score = 10000;
		 sortlist.push_back(user);
	 }
	 UpdateList();*/
#endif 

	FUNCTION_END;
}

void CGuiLordStrikeSortDlg::UpdateList()
{

	CGuiListBox * pListBox = GetListBox(2);
	if( !pListBox )		
		return;

	 pListBox->RemoveAllItems();
	 std::list<MiniUserData> & sortlist = CLordStrikeManager::GetSingleton()->GetLobby()->GetSortList();
	 std::list<MiniUserData>::iterator iter = sortlist.begin();
	 int isort =1 ; 
	 for(;iter != sortlist.end() && isort <= 100 ; ++iter,isort++)
	 {
		 MiniUserData & user = *iter;
		 pListBox->AddItem("",NULL);

		 char szSort[20];
		 _snprintf(szSort,20,"%d",isort);
		 szSort[19] = 0;
		 pListBox->SetItemText(pListBox->GetCount()-1,0,szSort,10);  // 名次

		 pListBox->SetItemText(pListBox->GetCount()-1,1,user.name,15);  // 玩家

		 char szTest[20];
		 _snprintf(szTest,20,"%d",user.score.win);
		 szTest[19] = 0;
		 pListBox->SetItemText(pListBox->GetCount()-1,2,szTest,10);  // ">赢

		 _snprintf(szTest,20,"%d",user.score.lose);
		 szTest[19] = 0;
		 pListBox->SetItemText(pListBox->GetCount()-1,3,szTest,10);  // 败</

	
		 _snprintf(szTest,20,"%d",user.score.draw);
		 szTest[19] = 0;
		 pListBox->SetItemText(pListBox->GetCount()-1,4,szTest,10);  // ">">平

	
		 _snprintf(szTest,20,"%d",user.score.score);
		 szTest[19] = 0;
		 pListBox->SetItemText(pListBox->GetCount()-1,5,szTest,10);  // 总分
	 }
}

bool CGuiLordStrikeSortDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) 
			{
			case 10:	
				{ 
					Close();
				} 
				break;
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}



