#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./RoleItem.h"
#include "./MainCharacter.h"
#include "./GuiNpcDialog.h"
#include "./GameState.h"
#include "./GuiSystem.h"

#include ".\guicompetition.h"
#include "../Media/Music.h"



struct stRandValue{
	float fValue;
	int   nMaxNum;
	int   nMinNum;
};

stRandValue randValueList[] = 
{
	{1000,99,99},
	{14,95,86},
	{13,85,76},
	{12,75,66},
	{11,65,56},
	{10,55,50},
	{9,49,45},
	{8,44,40},
	{7,39,35},
	{6,34,30},
	{5,29,25},
	{4,24,20},
	{3,19,15},
	{2,14,10},
	{1,9,5},
	{0,4,1},
};

struct stNumPic{
	float fValue;
	int   nGroup;
	int   nFrame;
};

stNumPic numPicList[] = 
{
	{1,45,100},
	{2,45,101},
	{3,45,102},
	{4,45,103},
	{5,45,104},
	{6,45,105},
	{7,45,106},
	{8,45,107},
	{9,45,108},
	{10,45,109},
};

int getRandValue(float value){
	static int count = sizeof(randValueList)/sizeof(stRandValue);
	for (int i=count-1;i>=0;i--)
	{
		if (value<randValueList[i].fValue)
		{
			int pos = (i==0)?i:i-1;
			return GETRANDNUM(randValueList[i].nMinNum,randValueList[i].nMaxNum);
		}
	}
	return 0;
}

stNumPic* GetPic(float value)
{
	static int count = sizeof(numPicList)/sizeof(stNumPic);
	for (int i=0;i<count;i++)
	{
		if (value<numPicList[i].fValue)
		{
			return &(numPicList[i]);
		}
	}
	return NULL;
}

CGuiCompetition::CGuiCompetition(byte eState)
:m_eState(eState)
,m_fReadTime(10.0f)
,m_fAnswerTime(10.0f)
,m_fWaitTime(5.0f)
,m_fStart(0.0f)
,m_nQuizCount(0)
,m_nQuizCountLeave(0)
,m_lastPic(0)
{
	GetGameState()->SetStateItem(eGameStateItem_Quiz);
	memset(&m_quizInfo,0,sizeof(stQuiz));
}

void CGuiCompetition::OnCreate()
{
	CGuiDialog::OnCreate();

	bool bAutoPk = GetGameApplication()->bClientSetted(enumAuto_Kill_Summon);
	if( bAutoPk )
	{
		GetGameApplication()->SetClientSet(enumAuto_Kill_Summon,false);
	}
	if( GetGameGuiManager()->m_guiSystem )
	{
		GetGameGuiManager()->m_guiSystem->UpdateAutoPkSwitch();
	}	
	m_BackElements.resize(4);
	CGuiElement* pEmt = new CGuiElement;
	pEmt->FontColor.Current = COLOR_ARGB(255,232,218,3);  
	pEmt->TextureColor.Current = COLOR_ARGB(128,0,0,0);
	pEmt->dwTextFormat = DT_TOP | DT_RIGHT;
	pEmt->iFont = 2;
	m_BackElements[0] = pEmt;

	pEmt = new CGuiElement;
	pEmt->FontColor.Current = COLOR_ARGB(255,232,218,3);
	pEmt->TextureColor.Current = COLOR_ARGB(128,0,0,0);
	pEmt->dwTextFormat = DT_TOP | DT_LEFT;
	pEmt->iFont = 2;
	m_BackElements[1] = pEmt;

	pEmt = new CGuiElement;
	pEmt->FontColor.Current = COLOR_ARGB(255,1,209,239);
	pEmt->TextureColor.Current = COLOR_ARGB(128,0,0,0);
	pEmt->dwTextFormat = DT_TOP | DT_LEFT;
	pEmt->iFont = 2;
	m_BackElements[2] = pEmt;

	pEmt = new CGuiElement;
	pEmt->FontColor.Current = COLOR_ARGB(255,151,239,1);
	pEmt->TextureColor.Current = COLOR_ARGB(128,0,0,0);
	pEmt->dwTextFormat = DT_TOP | DT_LEFT;
	pEmt->iFont = 2;
	m_BackElements[3] = pEmt;


	if (GetDevice()->GetWidth()<1024)
	{
		m_x = (GetDevice()->GetWidth() - 800)/2;
		m_y = (GetDevice()->GetHeight() - 600)/2;
	}
	else
	{
		m_x = (GetDevice()->GetWidth() - 1024)/2;
		m_y = (GetDevice()->GetHeight() - 768)/2;
	}

	InitData();

	strncpy(m_szBackMusicFile,GetGameBackMusic()->GetCurPlayingFile(),sizeof(m_szBackMusicFile));
	PlayBackMusic("dati.mp3");


	/*if (GetButton(200))
		GetButton(200)->SetEnabled(false);*/

}

void CGuiCompetition::OnClose(void)
{
	GetGameGuiManager()->m_guiCompetition = NULL;

	stQuizExit cmd;
	SEND_USER_CMD(cmd);
	
	if (m_eState==CGuiNpcDialog::eCompetitionNpc)
	{
		if (GetGameGuiManager()->m_guiNpcDialog)
		{
			GetGameState()->SetStateItem(eGameStateItem_NpcDialog);
			GetGameGuiManager()->OnNpcActionEnd();
		}
		else
			GetGameState()->SetStateItem(eGameStateItem_Normal);
	}
	else
		GetGameState()->SetStateItem(eGameStateItem_Normal);

	PlayBackMusic(m_szBackMusicFile);
}

void CGuiCompetition::RenderBackground(float fElapsedTime)
{
	stRectI rect(0,0,GetDevice()->GetWidth(),GetDevice()->GetHeight());

	//GetDevice()->FillRect(&rect,0xff000000);
	return CGuiDialog::RenderBackground(fElapsedTime);
}

void CGuiCompetition::InitData()
{
	GetButton(10)->SetButtonGroup(1);
	GetButton(11)->SetButtonGroup(1);
	GetButton(12)->SetButtonGroup(1);
	GetButton(13)->SetButtonGroup(1);
	GetButton(14)->SetButtonGroup(1);
	GetButton(15)->SetButtonGroup(1);

	GetButton(10)->SetChecked(false);
	GetButton(11)->SetChecked(false);
	GetButton(12)->SetChecked(false);
	GetButton(13)->SetChecked(false);
	GetButton(14)->SetChecked(false);
	GetButton(15)->SetChecked(false);

	/*GetStatic(3)->SetText("");
	GetStatic(4)->SetText("");
	GetStatic(30)->SetText("");
	GetStatic(31)->SetText("");
	GetStatic(32)->SetText("");
	GetStatic(33)->SetText("");*/

	switch(m_eState) {
	case CGuiNpcDialog::eCompetitionNpc:
		{
			
		}
		break;
	case CGuiNpcDialog::eCompetition:
		{
			GetButton(20)->SetButtonGroup(2);
			GetButton(21)->SetButtonGroup(2);
			GetButton(22)->SetButtonGroup(2);

			if (GetButton(20)->GetChecked())
				GetButton(20)->SetEnabled(false);

			if (GetButton(21)->GetChecked())
				GetButton(21)->SetEnabled(false);

			if (GetButton(22)->GetChecked())
				GetButton(22)->SetEnabled(false);
		}
		break;
	}
}

byte CGuiCompetition::GetLuck()
{
	if (m_eState==CGuiNpcDialog::eCompetition)
	{
		if (GetButton(20)->IsEnabled()&&GetButton(20)->GetChecked())
			return 1;
		
		if (GetButton(21)->IsEnabled()&&GetButton(21)->GetChecked())
			return 1;

		if (GetButton(22)->IsEnabled()&&GetButton(22)->GetChecked())
			return 1;
	}

	return 0;
}

void CGuiCompetition::SetQuizParam(stQuizParam* pCmd)
{
	m_fReadTime = pCmd->byReadQuestionTime;
	m_fAnswerTime = pCmd->byAnswerQuestionTime;
	m_fWaitTime = pCmd->byCalcTime;
	m_fStart = pCmd->byStartTime;
	m_nQuizCount = pCmd->bySubjectNumber;
	m_nQuizCountLeave = pCmd->bySubjectNumber;
	
	m_quizInfo.fPassTime = 0.0f;
	m_quizInfo.fSpareTime = m_fStart;
	m_quizInfo.eState = eStart;

	

	if (m_eState==CGuiNpcDialog::eCompetition)
	{
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl");

		rl.group = 45;
		rl.frame = 57;
		m_aniMake.Create(&rl);
		m_aniMake.SetSpeed(250);
		m_aniMake.SetLoopPlay(false);
	}

}

void CGuiCompetition::SetQuestion(stQuestionQuiz* pCmd)
{
	char szText[1024]={0};
	char* pStr=NULL;

	InitData();

	

	m_aniMake.SetStop(true);

	if (m_eState==CGuiNpcDialog::eCompetition)
	{
		GetImage(100)->SetVisible(false);
	}

	m_quizInfo.eState = eRead;
	m_quizInfo.dwID = pCmd->dwID;
	m_quizInfo.byType = m_eState==CGuiNpcDialog::eCompetition?QUIZ_WORLD:QUIZ_PERSONAL;
	m_quizInfo.fSpareTime = pCmd->bySpareTime;
	m_quizInfo.fPassTime = 0.0f;
	m_quizInfo.fReadTime = max(0.0f , m_quizInfo.fSpareTime - m_fAnswerTime - m_fWaitTime);
	m_quizInfo.fAnswerTime = max(0.0f , m_quizInfo.fSpareTime - m_quizInfo.fReadTime - m_fWaitTime);
	m_quizInfo.fWaitTime = max(0.0f , m_quizInfo.fSpareTime - m_quizInfo.fReadTime - m_quizInfo.fAnswerTime);
	m_quizInfo.nNoAnswer = 100;
	m_quizInfo.dwTotal = pCmd->dwTotal;

	sprintf(m_quizInfo.szID,"%d、",m_quizInfo.dwID);

	pStr=pCmd->subject;
	sprintf(m_quizInfo.szTitle,"%s",pStr);
	
	pStr+=(strlen(pStr)+1);
	sprintf(m_quizInfo.szAnswer1,"%s",pStr);
	
	pStr+=(strlen(pStr)+1);
	sprintf(m_quizInfo.szAnswer2,"%s",pStr);
	
	pStr+=(strlen(pStr)+1);
	sprintf(m_quizInfo.szAnswer3,"%s",pStr);
	
	pStr+=(strlen(pStr)+1);
	sprintf(m_quizInfo.szAnswer4,"%s",pStr);

	pStr+=(strlen(pStr)+1);
	sprintf(m_quizInfo.szAnswer5,"%s",pStr);

	pStr+=(strlen(pStr)+1);
	sprintf(m_quizInfo.szAnswer6,"%s",pStr);
	
	if (m_nQuizCountLeave!=0)
		m_nQuizCountLeave--;
	
	m_lastTime = 0;
}

void CGuiCompetition::SetQuizCurScore(stQuizCurScore* pCmd)
{
	m_quizInfo.nGrace = pCmd->dwGrace;
	m_quizInfo.nScore = pCmd->dwScore;
}

void CGuiCompetition::SetResult(stAnswerReturnQuiz* pCmd)
{
	m_quizInfo.eState = eResult;
	switch(pCmd->byAnswerStatus) {
	case 0: //回答正确
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces.gl");

			rl.group = 45;
			rl.frame = 51;
			m_aniMake.Create(&rl);
			m_aniMake.SetLoopPlay(false);
			PlayUISound(10300);
		}
		break;
	case 1: //回答错误
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces.gl");

			rl.group = 45;
			rl.frame = 50;
			m_aniMake.Create(&rl);
			m_aniMake.SetLoopPlay(false);
			PlayUISound(10301);
		}
		break;
	default:
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces.gl");

			rl.group = 45;
			rl.frame = 52;
			m_aniMake.Create(&rl);
			m_aniMake.SetLoopPlay(false);
			PlayUISound(10302);
		}
		break;
	}
}

void CGuiCompetition::SetQuizList(stQuizPosList* pCmd)
{
	if (pCmd->dwSize==0)
		return;

	static int namewidth = GetDevice()->GetWidth()<1024?16:20;

	CGuiListBox* pListBox = NULL;

	pListBox = GetListBox(5);
	if (pListBox==NULL)
		return;
	
	m_quizInfo.nRank = pCmd->rank_list[0].rank;
	m_quizInfo.nScore = pCmd->rank_list[0].score;
	m_quizInfo.nGrace = pCmd->rank_list[0].grace;


	pListBox->RemoveAllItems();
	for (int i=1;i<pCmd->dwSize;i++)
	{
		pListBox->AddItem("",NULL);
		pListBox->SetItemText(pListBox->GetCount()-1,0,avar("%s(%s)",pCmd->rank_list[i].name,
			pCmd->rank_list[i].countryName),namewidth);
		
		pListBox->SetItemText(pListBox->GetCount()-1,1,avar("%d",pCmd->rank_list[i].score));

	}
}

bool CGuiCompetition::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent) {
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 200:
			case 2:
				OnGuiEvent(EVENT_BUTTON_CLICKED,-1,NULL);
				break;
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				{
					if (m_quizInfo.eState==eAnswer)
					{
						GetButton(nID)->SetChecked(true);
						m_quizInfo.eState = eWait;

						stAnswerQuiz cmd;
						cmd.dwID = m_quizInfo.dwID;
						cmd.byType = m_quizInfo.byType;
						cmd.answerID = nID - 9;
						cmd.byLuck = GetLuck();
						cmd.dwScore = (int)(m_quizInfo.fReadTime + m_quizInfo.fAnswerTime - m_quizInfo.fPassTime)+1;

						SEND_USER_CMD(cmd);


						switch(m_eState) {
						case CGuiNpcDialog::eCompetition:
							{
								
							}
							break;

						case CGuiNpcDialog::eCompetitionNpc:
							{
								m_quizInfo.fSpareTime = m_quizInfo.fPassTime + m_quizInfo.fWaitTime;
								m_quizInfo.fAnswerTime = m_quizInfo.fPassTime - m_quizInfo.fReadTime;
								
							}
							break;
						}


						if(g_bSinglePlayer)
						{
							
							stAnswerReturnQuiz _cmd;
							_cmd.byAnswerStatus = 1;
							GetClient()->PushCommand(&_cmd,sizeof(stAnswerReturnQuiz));

							switch(m_eState) {
							case CGuiNpcDialog::eCompetition:
								{
									stQuizPosList cmd;
									cmd.dwSize = 5;
									cmd.rank_list[0].rank = 20;
									cmd.rank_list[0].score = 12000;

									sprintf(cmd.rank_list[1].countryName,"宋国");
									sprintf(cmd.rank_list[1].name,"zzf1");
									cmd.rank_list[1].rank = 1;
									cmd.rank_list[1].score = 50000;

									sprintf(cmd.rank_list[2].countryName,"宋国");
									sprintf(cmd.rank_list[2].name,"zzf2");
									cmd.rank_list[2].rank = 2;
									cmd.rank_list[2].score = 40000;

									sprintf(cmd.rank_list[3].countryName,"宋国");
									sprintf(cmd.rank_list[3].name,"zzf3");
									cmd.rank_list[3].rank = 3;
									cmd.rank_list[3].score = 30000;

									sprintf(cmd.rank_list[4].countryName,"宋国");
									sprintf(cmd.rank_list[4].name,"zzf4");
									cmd.rank_list[4].rank = 4;
									cmd.rank_list[4].score = 20000;

									GetClient()->PushCommand(&cmd,sizeof(stQuizPosList));
								}
								break;
							case CGuiNpcDialog::eCompetitionNpc:
								{
									stQuizCurScore cmd;
									cmd.dwScore = 1000;
									cmd.dwGrace = 30;

									GetClient()->PushCommand(&cmd,sizeof(stQuizCurScore));
								}
								break;
							}
							
						}
					}
				}
				break;
			case 20:
			case 21:
			case 22:
				{
					if ((m_quizInfo.eState==eAnswer)||(m_quizInfo.eState==eRead))
					{
						if (GetButton(nID)->GetChecked())
							GetButton(nID)->SetChecked(false);
						else
							GetButton(nID)->SetChecked(true);
					}
				}
				break;
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

void  CGuiCompetition::SetAnswer(bool bAnswer)
{
	if (GetButton(10)->GetChecked()==false)
		GetButton(10)->SetEnabled(bAnswer);
	if (GetButton(11)->GetChecked()==false)
		GetButton(11)->SetEnabled(bAnswer);
	if (GetButton(12)->GetChecked()==false)
		GetButton(12)->SetEnabled(bAnswer);
	if (GetButton(13)->GetChecked()==false)
		GetButton(13)->SetEnabled(bAnswer);
	if (GetButton(14)->GetChecked()==false)
		GetButton(14)->SetEnabled(bAnswer);
	if (GetButton(15)->GetChecked()==false)
		GetButton(15)->SetEnabled(bAnswer);


}

void  CGuiCompetition::QuitCompetition()
{
	if (m_eState==CGuiNpcDialog::eCompetitionNpc)
	{
		Close();
		return;
	}

	if (m_quizInfo.eState != eQuit)
	{
		SetQuit();
	}
	else
	{
		Close();
	}
}

void  CGuiCompetition::SetQuit()
{
	m_quizInfo.eState = eQuit;

	stResourceLocation rl;
	rl.SetFileName("data\\interfaces.gl");

	rl.group = 45;
	rl.frame = 56;
	m_aniMake.Create(&rl);
	m_aniMake.SetSpeed(300);
	m_aniMake.SetLoopPlay(false);
}

HRESULT CGuiCompetition::OnRender(float fElapsedTime)
{
	bool bPlaySound = false;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	
	stPointF pt = GetDevice()->GetFontScale();
	GetDevice()->SetFontScale(0.75,0.75);

	if ((m_quizInfo.fSpareTime>m_quizInfo.fPassTime)||(m_quizInfo.eState==eQuit))
		m_quizInfo.fPassTime+=fElapsedTime;


	if (m_quizInfo.eState==eRead)
	{
		if (m_quizInfo.fReadTime<m_quizInfo.fPassTime)
		{
			m_quizInfo.eState = eAnswer;
		}
	}

	if (m_quizInfo.eState==eAnswer)
	{
		if (m_quizInfo.fReadTime+m_quizInfo.fAnswerTime<m_quizInfo.fPassTime)
		{
			m_quizInfo.eState = eWait;

			stResourceLocation rl;
			rl.SetFileName("data\\interfaces.gl");

			rl.group = 45;
			rl.frame = 52;
			m_aniMake.Create(&rl);
			m_aniMake.SetLoopPlay(false);
			PlayUISound(10302);
		}
	}

	switch(m_quizInfo.eState) {
	case eStart:
		{
			//DrawText(m_eState==CGuiNpcDialog::eCompetition?"开始竞赛时间：":"开始问答时间：",m_BackElements[3],&(stRectI(125,535,290,570)));
			SetAnswer(false);
			if (m_eState==CGuiNpcDialog::eCompetition)
			{
				stNumPic* p=GetPic(m_quizInfo.fSpareTime -m_quizInfo.fPassTime);
				if (p)
				{
					if(m_lastPic != p->nFrame)
					{
						m_lastPic = p->nFrame;
						PlayUISound(10304);
					}
					GetImage(100)->SetImage(stResourceLocation("data\\interfaces.gl",p->nGroup,p->nFrame));
				}
				else
					GetImage(100)->SetImage(stResourceLocation("data\\interfaces.gl",-1,-1));
			}
		}
		break;

	case eRead:
		{
			SetAnswer(false);
		}
		break;

	case eAnswer:
		{
			SetAnswer(true);
		}
		break;

	case eWait:
		{
			SetAnswer(false);
		}
		break;
	}


	if (GetDevice()->GetWidth()<1024)
	{
		DrawText(m_quizInfo.szID,m_BackElements[0],&(stRectI(115,170,164,200)));
		DrawText(m_quizInfo.szTitle,m_BackElements[1],&(stRectI(157,170,500,240)));
		DrawText(m_quizInfo.szAnswer1,m_BackElements[1],&(stRectI(175,256,500,276)));
		DrawText(m_quizInfo.szAnswer2,m_BackElements[1],&(stRectI(175,289,500,309)));
		DrawText(m_quizInfo.szAnswer3,m_BackElements[1],&(stRectI(175,323,500,343)));
		DrawText(m_quizInfo.szAnswer4,m_BackElements[1],&(stRectI(175,356,500,376)));
		DrawText(m_quizInfo.szAnswer5,m_BackElements[1],&(stRectI(175,389,500,409)));
		DrawText(m_quizInfo.szAnswer6,m_BackElements[1],&(stRectI(175,424,500,444)));


		switch(m_eState) {
		case CGuiNpcDialog::eCompetition:
			DrawText("竞赛剩余题数：",m_BackElements[3],&(stRectI(305,460,530,480)));
			break;
		case CGuiNpcDialog::eCompetitionNpc:
			DrawText("问答剩余题数：",m_BackElements[3],&(stRectI(305,460,530,480)));
			break;
		}

		

		switch(m_quizInfo.eState) {
		case eStart:
			{
				//DrawText(avar("%d",(int)(m_quizInfo.fSpareTime -m_quizInfo.fPassTime)),m_BackElements[3],&(stRectI(280,532,350,570)));


			}
			break;

		case eRead:
			{
				DrawText(avar("%d",(int)(m_quizInfo.fReadTime -m_quizInfo.fPassTime)),m_BackElements[3],&(stRectI(253,460,300,480)));
				DrawText("现在是出题时间，请认真看题！",m_BackElements[2],&(stRectI(130,150,500,170)));
				DrawText("阅题剩余时间：",m_BackElements[3],&(stRectI(110,460,290,480)));
			}
			break;

		case eAnswer:
			{
				int nTmp = (int)(m_quizInfo.fReadTime + m_quizInfo.fAnswerTime - m_quizInfo.fPassTime);
				if (m_lastTime != nTmp)
				{
					m_lastTime = nTmp;
					bPlaySound = true;
				}
				DrawText(avar("%d",nTmp),m_BackElements[3],&(stRectI(253,460,300,480)));
				DrawText("现在是答题时间，请认真答题！",m_BackElements[2],&(stRectI(130,150,500,170)));
				DrawText("答题剩余时间：",m_BackElements[3],&(stRectI(110,460,290,480)));
			}
			break;

		case eQuit:
		case eWait:
			{
				int nTime = (int)(m_quizInfo.fReadTime + m_quizInfo.fAnswerTime + m_quizInfo.fWaitTime  - m_quizInfo.fPassTime);
				nTime = nTime<0?0:nTime;
				//if ((m_eState==CGuiNpcDialog::eCompetition)&&((m_quizInfo.fReadTime + m_quizInfo.fAnswerTime - m_quizInfo.fPassTime)>0.0f))
				//{
				//	int randNum = getRandValue(m_quizInfo.fReadTime + m_quizInfo.fAnswerTime - m_quizInfo.fPassTime);
				//	if (randNum<m_quizInfo.nNoAnswer)
				//		m_quizInfo.nNoAnswer = randNum;

				//	DrawText(avar("%d",(int)(m_quizInfo.dwTotal*m_quizInfo.nNoAnswer/100)),m_BackElements[3],&(stRectI(280,600,350,630)));
				//}
				//else
				DrawText(avar("%d",nTime),m_BackElements[3],&(stRectI(253,460,300,480)));

				//if ((m_eState==CGuiNpcDialog::eCompetition)&&((m_quizInfo.fReadTime + m_quizInfo.fAnswerTime - m_quizInfo.fPassTime)>0.0f))
				//	DrawText("未答题的人数：",m_BackElements[3],&(stRectI(125,603,290,633)));
				//else
				DrawText("下题出题时间：",m_BackElements[3],&(stRectI(110,460,290,480)));
			}
			break;
		}

		DrawText(avar("%d",m_nQuizCountLeave),m_BackElements[3],&(stRectI(450,460,550,480)));
	

		switch(m_eState) {
		case CGuiNpcDialog::eCompetition:
			DrawText(avar("%d",m_quizInfo.nScore),m_BackElements[3],&(stRectI(695,490,750,510)));
			DrawText(avar("%d",m_quizInfo.nRank),m_BackElements[3],&(stRectI(695,511,750,530)));
			DrawText(avar("%d",m_quizInfo.nGrace),m_BackElements[3],&(stRectI(695,532,750,550)));
			break;
		case CGuiNpcDialog::eCompetitionNpc:
			DrawText(avar("%d",m_quizInfo.nScore),m_BackElements[3],&(stRectI(700,413,780,435)));
			DrawText(avar("%d",m_quizInfo.nGrace),m_BackElements[3],&(stRectI(700,437,780,458)));
			DrawText(avar("%d",m_nQuizCount),m_BackElements[1],&(stRectI(684,201,707,220)));
			break;
		}

		GetDevice()->SetFontScale(pt.x,pt.y);

	}
	else
	{
		DrawText(m_quizInfo.szID,m_BackElements[0],&(stRectI(115,195,164,220)));
		DrawText(m_quizInfo.szTitle,m_BackElements[1],&(stRectI(157,195,650,320)));
		DrawText(m_quizInfo.szAnswer1,m_BackElements[1],&(stRectI(195,333,650,363)));
		DrawText(m_quizInfo.szAnswer2,m_BackElements[1],&(stRectI(195,377,650,407)));
		DrawText(m_quizInfo.szAnswer3,m_BackElements[1],&(stRectI(195,420,650,450)));
		DrawText(m_quizInfo.szAnswer4,m_BackElements[1],&(stRectI(195,464,650,494)));
		DrawText(m_quizInfo.szAnswer5,m_BackElements[1],&(stRectI(195,506,650,536)));
		DrawText(m_quizInfo.szAnswer6,m_BackElements[1],&(stRectI(195,550,650,580)));


		switch(m_quizInfo.eState) {
		case eStart:
			{
				//DrawText(m_eState==CGuiNpcDialog::eCompetition?"开始竞赛时间：":"开始问答时间：",m_BackElements[3],&(stRectI(125,535,290,570)));
			}
			break;

		case eRead:
			{
				DrawText("现在是出题时间，请认真看题！",m_BackElements[2],&(stRectI(140,170,650,190)));
				DrawText("阅题剩余时间：",m_BackElements[3],&(stRectI(125,603,290,633)));
			}
			break;

		case eAnswer:
			{
				DrawText("现在是答题时间，请认真答题！",m_BackElements[2],&(stRectI(140,170,650,190)));
				DrawText("答题剩余时间：",m_BackElements[3],&(stRectI(125,603,290,633)));
			}
			break;

		case eWait:
			{
				//if ((m_eState==CGuiNpcDialog::eCompetition)&&((m_quizInfo.fReadTime + m_quizInfo.fAnswerTime - m_quizInfo.fPassTime)>0.0f))
				//	DrawText("未答题的人数：",m_BackElements[3],&(stRectI(125,603,290,633)));
				//else
				DrawText("下题出题时间：",m_BackElements[3],&(stRectI(125,603,290,633)));
			}
			break;
		}

		switch(m_eState) {
		case CGuiNpcDialog::eCompetition:
			DrawText("竞赛剩余题数：",m_BackElements[3],&(stRectI(380,603,540,633)));
			break;
		case CGuiNpcDialog::eCompetitionNpc:
			DrawText("问答剩余题数：",m_BackElements[3],&(stRectI(380,603,540,633)));
			break;
		}

		GetDevice()->SetFontScale(pt.x,pt.y);

		switch(m_quizInfo.eState) {
		case eStart:
			{
				//DrawText(avar("%d",(int)(m_quizInfo.fSpareTime -m_quizInfo.fPassTime)),m_BackElements[3],&(stRectI(280,532,350,570)));
				if (m_eState==CGuiNpcDialog::eCompetition)
				{
					stNumPic* p=GetPic(m_quizInfo.fSpareTime -m_quizInfo.fPassTime);
					if (p)
					{
						if(m_lastPic != p->nFrame)
						{
							m_lastPic = p->nFrame;
							PlayUISound(10304);
						}
						GetImage(100)->SetImage(stResourceLocation("data\\interfaces.gl",p->nGroup,p->nFrame));
					}
					else
						GetImage(100)->SetImage(stResourceLocation("data\\interfaces.gl",-1,-1));
				}

			}
			break;

		case eRead:
			{
				DrawText(avar("%d",(int)(m_quizInfo.fReadTime -m_quizInfo.fPassTime)),m_BackElements[3],&(stRectI(280,600,350,630)));
			}
			break;

		case eAnswer:
			{
				int nTmp = (int)(m_quizInfo.fReadTime + m_quizInfo.fAnswerTime - m_quizInfo.fPassTime);
				if (m_lastTime != nTmp)
				{
					m_lastTime = nTmp;
					bPlaySound = true;
				}
				DrawText(avar("%d",nTmp),m_BackElements[3],&(stRectI(280,600,350,630)));
			}
			break;

		case eQuit:
		case eWait:
			{
				int nTime = (int)(m_quizInfo.fReadTime + m_quizInfo.fAnswerTime + m_quizInfo.fWaitTime  - m_quizInfo.fPassTime);
				nTime = nTime<0?0:nTime;
				//if ((m_eState==CGuiNpcDialog::eCompetition)&&((m_quizInfo.fReadTime + m_quizInfo.fAnswerTime - m_quizInfo.fPassTime)>0.0f))
				//{
				//	int randNum = getRandValue(m_quizInfo.fReadTime + m_quizInfo.fAnswerTime - m_quizInfo.fPassTime);
				//	if (randNum<m_quizInfo.nNoAnswer)
				//		m_quizInfo.nNoAnswer = randNum;

				//	DrawText(avar("%d",(int)(m_quizInfo.dwTotal*m_quizInfo.nNoAnswer/100)),m_BackElements[3],&(stRectI(280,600,350,630)));
				//}
				//else
				DrawText(avar("%d",nTime),m_BackElements[3],&(stRectI(280,600,350,630)));
			}
			break;
		}

		DrawText(avar("%d",m_nQuizCountLeave),m_BackElements[3],&(stRectI(540,600,650,630)));

		switch(m_eState) {
		case CGuiNpcDialog::eCompetition:
			DrawText(avar("%d",m_quizInfo.nScore),m_BackElements[3],&(stRectI(900,632,980,670)));
			DrawText(avar("%d",m_quizInfo.nRank),m_BackElements[3],&(stRectI(900,661,980,700)));
			DrawText(avar("%d",m_quizInfo.nGrace),m_BackElements[3],&(stRectI(900,691,980,730)));
			break;
		case CGuiNpcDialog::eCompetitionNpc:
			DrawText(avar("%d",m_quizInfo.nScore),m_BackElements[3],&(stRectI(900,493,980,520)));
			DrawText(avar("%d",m_quizInfo.nGrace),m_BackElements[3],&(stRectI(900,525,980,550)));
			DrawText(avar("%d",m_nQuizCount),m_BackElements[1],&(stRectI(882,230,920,260)));
			break;
		}

	}



	

	static int x = (GetDevice()->GetWidth() - 1024)/2;
	static int y = (GetDevice()->GetHeight() - 768)/2;
	
	m_aniMake.Render(x,y);

	if( m_quizInfo.eState == eResult)
	{
		if (m_aniMake.GetImages()&&(m_aniMake.IsLoopPlay()==false)&&(m_aniMake.IsPlaying()==false))
		{
			switch(m_aniMake.GetImages()->GetFrame()) 
			{
			case 50:  // answer error 
				{
					stResourceLocation rl;
					rl.SetFileName("data\\interfaces.gl");

					rl.group = 45;
					rl.frame = 53;
					m_aniMake.Create(&rl);
					m_aniMake.SetLoopPlay(true);
				}
				break;
			case 51: // answer ok
				{
					stResourceLocation rl;
					rl.SetFileName("data\\interfaces.gl");

					rl.group = 45;
					rl.frame = 54;
					m_aniMake.Create(&rl);
					m_aniMake.SetLoopPlay(true);
				}
				break;
			case 52: // no answer 
				{
					stResourceLocation rl;
					rl.SetFileName("data\\interfaces.gl");

					rl.group = 45;
					rl.frame = 55;
					m_aniMake.Create(&rl);
					m_aniMake.SetLoopPlay(true);
				}
				break;
			case 57: // wait 
				{
					stResourceLocation rl;
					rl.SetFileName("data\\interfaces.gl");

					rl.group = 45;
					rl.frame = 58;
					m_aniMake.Create(&rl);
					m_aniMake.SetLoopPlay(true);
				}
				break;
			}
		}// end switch 
	}
	

	if ((bPlaySound)&&(m_quizInfo.eState==eAnswer))
		PlayUISound(10303);

	if ((m_quizInfo.eState==eQuit)&&(m_quizInfo.fReadTime + m_quizInfo.fAnswerTime + m_quizInfo.fWaitTime + 60.0f - m_quizInfo.fPassTime<0.0f))
	{
		Close();
	}
	
	return hr;
}