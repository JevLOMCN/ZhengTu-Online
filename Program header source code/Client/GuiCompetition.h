#pragma once
#include "..\gui\include\guidialog.h"


#define GETRANDNUM(minNum,maxNum)  minNum + rand() * (maxNum - minNum) / RAND_MAX

class CGuiCompetition :
	public CGuiDialog
{
	float m_fStart;
	float m_fReadTime;    //读题时间
	float m_fAnswerTime;  //答题时间
	float m_fWaitTime;    //等待时间
	BYTE  m_nQuizCount;   //总的题目数
	BYTE  m_nQuizCountLeave; //剩余题目数

	int   m_lastPic;
	int   m_lastTime;
	

	enum eQuizState{
		eStart,
		eRead,
		eAnswer,
		eWait,
		eResult,  // 服务器返回结果
		eQuit,
	};
	struct stQuiz{
		int   nScore;     // 得分
		int   nRank; // 玩家排名
		int   nGrace; //文采
		int   nNoAnswer;
		DWORD dwTotal;  // 总人数


		DWORD dwID;     // 题目序号
		BYTE  byType; // 竞赛ID
		eQuizState  eState; //当前可否回答
		float fSpareTime; //剩余时间
		float fPassTime;  //经过时间
		float fReadTime;    //读题时间
		float fAnswerTime;  //答题时间
		float fWaitTime;    //等待时间

		char  szID[16];
		char  szTitle[512];
		char  szAnswer1[128];
		char  szAnswer2[128];
		char  szAnswer3[128];
		char  szAnswer4[128];
		char  szAnswer5[128];
		char  szAnswer6[128];
	};
	byte     m_eState;
	stQuiz   m_quizInfo;
	char     m_szBackMusicFile[MAX_PATH];

	CAnimation     m_aniMake;

	void     InitData();
	byte     GetLuck();
	void     SetAnswer(bool bAnswer);
	void     SetQuit();
	byte     GetState(){
		  return m_quizInfo.eState;
	}

public:
	CGuiCompetition(byte eState);
	void OnCreate();
	void OnClose(void);
	void RenderBackground(float fElapsedTime);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);
	
	void SetQuestion(stQuestionQuiz* pCmd);
	void SetQuizList(stQuizPosList* pCmd);
	void SetQuizCurScore(stQuizCurScore* pCmd);
	void SetQuizParam(stQuizParam* pCmd);
	void SetResult(stAnswerReturnQuiz* pCmd);
	void QuitCompetition();
	
};
