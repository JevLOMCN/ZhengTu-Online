/**
*\file		GuiTop.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    经脉对话框
*
* 
*
*/
#include "public.h"
#include "../gui/include/GuiManager.h"	
#include "./GameGuiManager.h"
#include "./Game.h"
#include "./table.h"
#include "./RoleItem.h"
#include "./GuiTianxia.h"
#include <sys/timeb.h>
#include "./GameScene.h"
#include "Maincharacter.h"
///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiTianxiaDlg::CGuiTianxiaDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiTianxiaDlg::~CGuiTianxiaDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}
HRESULT CGuiTianxiaDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	time_t cur_time = time(NULL);
	struct tm* server_time = localtime(&cur_time);

	static char szTime[256];
	if( server_time->tm_hour<22)
	{
		int fen = 0;
		fen = (21-server_time->tm_hour)*60 + 60-server_time->tm_min;
		sprintf(szTime, "距离比赛开始还剩 %d分钟",fen+10);
		GetStatic(999)->SetText(szTime);
	}
	else{
		if( server_time->tm_hour == 22)
		{
			if(server_time->tm_min <18)
			{
				int fen = 0;
				fen = 18 - server_time->tm_min;
				sprintf(szTime, "距离海选始开始剩 %d分钟",fen);
				GetStatic(999)->SetText(szTime);
			}
			else if(server_time->tm_min <24)
			{
				int fen = 0;
				fen = 24 - server_time->tm_min;
				sprintf(szTime, "距离复选开始还剩 %d分钟",fen);
				GetStatic(999)->SetText(szTime);
			}
			else if(server_time->tm_min <32)
			{
				int fen = 0;
				fen = 32 - server_time->tm_min;
				sprintf(szTime, "距离半决赛开始还剩 %d分钟",fen);
				GetStatic(999)->SetText(szTime);
			}
			else if(server_time->tm_min <40)
			{
				int fen = 0;
				fen = 40 - server_time->tm_min;
				sprintf(szTime, "距离决赛开始还剩 %d分钟",fen);
				GetStatic(999)->SetText(szTime);
			}

			else if(server_time->tm_min >=40)
			{
				sprintf(szTime, "  今日比赛已全部结束");
				GetStatic(999)->SetText(szTime);
			}
		}
	}
	return hr;

	FUNCTION_END;
}
void CGuiTianxiaDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();

	



	FUNCTION_END;
}


bool CGuiTianxiaDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
			case 1:
			{
				SetVisible(false);
			}	
			break;
			case 1800:
			{
				stTianxiaBaomingCmd cmd; //sky 请求指令
				SEND_USER_CMD(cmd);
			}
			break;
		}		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiTianxiaDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiTianxia = NULL;

	FUNCTION_END;
}

//天下第一刷新
void CGuiTianxiaDlg::shuaxin(void)
{
	FUNCTION_BEGIN;

	for(int i=0;i<32;i++)
	{
		if(tianxia[i].userid != 0)
		{
			GetStatic(100+i+1)->SetText(tianxia[i].username);
			GetStatic(100+i+1)->SetTextColor(COLOR_ARGB(255,0,255,0));
		}
	}	

	if(tianxia[0].saidian >=1)
	{
		GetStatic(201)->SetText(tianxia[0].username);
		GetStatic(201)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1001)->SetImage(stResourceLocation("data\\interfaces3.gl",113,18));
		
	}
	else if(tianxia[1].saidian >=1)
	{
		GetStatic(201)->SetText(tianxia[1].username);
		GetStatic(201)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1002)->SetImage(stResourceLocation("data\\interfaces3.gl",113,19));
	}

	if(tianxia[2].saidian >=1)
	{
		GetStatic(203)->SetText(tianxia[2].username);
		GetStatic(203)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1003)->SetImage(stResourceLocation("data\\interfaces3.gl",113,26));
	}
	else if(tianxia[3].saidian >=1)
	{
		GetStatic(203)->SetText(tianxia[3].username);
		GetStatic(203)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1004)->SetImage(stResourceLocation("data\\interfaces3.gl",113,27));
	}

	if(tianxia[4].saidian >=1)
	{
		GetStatic(202)->SetText(tianxia[4].username);
		GetStatic(202)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1005)->SetImage(stResourceLocation("data\\interfaces3.gl",113,18));
	}
	else if(tianxia[5].saidian >=1)
	{
		GetStatic(202)->SetText(tianxia[5].username);
		GetStatic(202)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1006)->SetImage(stResourceLocation("data\\interfaces3.gl",113,19));
	}

	if(tianxia[6].saidian >=1)
	{
		GetStatic(204)->SetText(tianxia[6].username);
		GetStatic(204)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1007)->SetImage(stResourceLocation("data\\interfaces3.gl",113,26));
	}
	else if(tianxia[7].saidian >=1)
	{
		GetStatic(204)->SetText(tianxia[7].username);
		GetStatic(204)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1008)->SetImage(stResourceLocation("data\\interfaces3.gl",113,27));
	}

	if(tianxia[8].saidian >=1)
	{
		GetStatic(205)->SetText(tianxia[8].username);
		GetStatic(205)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1009)->SetImage(stResourceLocation("data\\interfaces3.gl",113,18));
	}
	else if(tianxia[9].saidian >=1)
	{
		GetStatic(205)->SetText(tianxia[9].username);
		GetStatic(205)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1010)->SetImage(stResourceLocation("data\\interfaces3.gl",113,19));
	}

	if(tianxia[10].saidian >=1)
	{
		GetStatic(207)->SetText(tianxia[10].username);
		GetStatic(207)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1011)->SetImage(stResourceLocation("data\\interfaces3.gl",113,26));
	}
	else if(tianxia[11].saidian >=1)
	{
		GetStatic(207)->SetText(tianxia[11].username);
		GetStatic(207)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1012)->SetImage(stResourceLocation("data\\interfaces3.gl",113,27));
	}

	if(tianxia[12].saidian >=1)
	{
		GetStatic(206)->SetText(tianxia[12].username);
		GetStatic(206)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1013)->SetImage(stResourceLocation("data\\interfaces3.gl",113,18));
	}
	else if(tianxia[13].saidian >=1)
	{
		GetStatic(206)->SetText(tianxia[13].username);
		GetStatic(206)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1014)->SetImage(stResourceLocation("data\\interfaces3.gl",113,19));
	}

	if(tianxia[14].saidian >=1)
	{
		GetStatic(208)->SetText(tianxia[14].username);
		GetStatic(208)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1015)->SetImage(stResourceLocation("data\\interfaces3.gl",113,26));
	}
	else if(tianxia[15].saidian >=1)
	{
		GetStatic(208)->SetText(tianxia[15].username);
		GetStatic(208)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1016)->SetImage(stResourceLocation("data\\interfaces3.gl",113,27));
	}


	if(tianxia[16].saidian >=1)
	{
		GetStatic(209)->SetText(tianxia[16].username);
		GetStatic(209)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1017)->SetImage(stResourceLocation("data\\interfaces3.gl",113,18));
	}
	else if(tianxia[17].saidian >=1)
	{
		GetStatic(209)->SetText(tianxia[17].username);
		GetStatic(209)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1018)->SetImage(stResourceLocation("data\\interfaces3.gl",113,19));
	}

	if(tianxia[18].saidian >=1)
	{
		GetStatic(211)->SetText(tianxia[18].username);
		GetStatic(211)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1019)->SetImage(stResourceLocation("data\\interfaces3.gl",113,26));
	}
	else if(tianxia[19].saidian >=1)
	{
		GetStatic(211)->SetText(tianxia[19].username);
		GetStatic(211)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1020)->SetImage(stResourceLocation("data\\interfaces3.gl",113,27));
	}

	if(tianxia[20].saidian >=1)
	{
		GetStatic(210)->SetText(tianxia[20].username);
		GetStatic(210)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1021)->SetImage(stResourceLocation("data\\interfaces3.gl",113,18));
	}
	else if(tianxia[21].saidian >=1)
	{
		GetStatic(210)->SetText(tianxia[21].username);
		GetStatic(210)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1022)->SetImage(stResourceLocation("data\\interfaces3.gl",113,19));
	}

	if(tianxia[22].saidian >=1)
	{
		GetStatic(212)->SetText(tianxia[22].username);
		GetStatic(212)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1023)->SetImage(stResourceLocation("data\\interfaces3.gl",113,26));
	}
	else if(tianxia[23].saidian >=1)
	{
		GetStatic(212)->SetText(tianxia[23].username);
		GetStatic(212)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1024)->SetImage(stResourceLocation("data\\interfaces3.gl",113,27));
	}

	if(tianxia[24].saidian >=1)
	{
		GetStatic(213)->SetText(tianxia[24].username);
		GetStatic(213)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1025)->SetImage(stResourceLocation("data\\interfaces3.gl",113,18));
	}
	else if(tianxia[25].saidian >=1)
	{
		GetStatic(213)->SetText(tianxia[25].username);
		GetStatic(213)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1026)->SetImage(stResourceLocation("data\\interfaces3.gl",113,19));
	}

	if(tianxia[26].saidian >=1)
	{
		GetStatic(215)->SetText(tianxia[26].username);
		GetStatic(215)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1027)->SetImage(stResourceLocation("data\\interfaces3.gl",113,26));
	}
	else if(tianxia[27].saidian >=1)
	{
		GetStatic(215)->SetText(tianxia[27].username);
		GetStatic(215)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1028)->SetImage(stResourceLocation("data\\interfaces3.gl",113,27));
	}

	if(tianxia[28].saidian >=1)
	{
		GetStatic(214)->SetText(tianxia[28].username);
		GetStatic(214)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1029)->SetImage(stResourceLocation("data\\interfaces3.gl",113,18));
	}
	else if(tianxia[29].saidian >=1)
	{
		GetStatic(214)->SetText(tianxia[29].username);
		GetStatic(214)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1030)->SetImage(stResourceLocation("data\\interfaces3.gl",113,19));
	}

	if(tianxia[30].saidian >=1)
	{
		GetStatic(216)->SetText(tianxia[30].username);
		GetStatic(216)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1031)->SetImage(stResourceLocation("data\\interfaces3.gl",113,26));
	}
	else if(tianxia[31].saidian >=1)
	{
		GetStatic(216)->SetText(tianxia[31].username);
		GetStatic(216)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(1032)->SetImage(stResourceLocation("data\\interfaces3.gl",113,27));
	}





	//八强

	if(tianxia[0].saidian >=2)
	{
		GetStatic(301)->SetText(tianxia[0].username);
		GetStatic(301)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2001)->SetImage(stResourceLocation("data\\interfaces3.gl",113,16));
		
	}
	else if(tianxia[1].saidian >=2)
	{
		GetStatic(301)->SetText(tianxia[1].username);
		GetStatic(301)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2001)->SetImage(stResourceLocation("data\\interfaces3.gl",113,16));
	}

	else if(tianxia[4].saidian >=2)
	{
		GetStatic(301)->SetText(tianxia[4].username);
		GetStatic(301)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2002)->SetImage(stResourceLocation("data\\interfaces3.gl",113,17));
	}
	else if(tianxia[5].saidian >=2)
	{
		GetStatic(301)->SetText(tianxia[5].username);
		GetStatic(301)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2002)->SetImage(stResourceLocation("data\\interfaces3.gl",113,17));
	}


	if(tianxia[2].saidian >=2)
	{
		GetStatic(303)->SetText(tianxia[2].username);
		GetStatic(303)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2003)->SetImage(stResourceLocation("data\\interfaces3.gl",113,24));
		
	}
	else if(tianxia[3].saidian >=2)
	{
		GetStatic(303)->SetText(tianxia[3].username);
		GetStatic(303)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2003)->SetImage(stResourceLocation("data\\interfaces3.gl",113,24));
	}

	else if(tianxia[6].saidian >=2)
	{
		GetStatic(303)->SetText(tianxia[6].username);
		GetStatic(303)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2004)->SetImage(stResourceLocation("data\\interfaces3.gl",113,25));
	}
	else if(tianxia[7].saidian >=2)
	{
		GetStatic(303)->SetText(tianxia[7].username);
		GetStatic(303)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2004)->SetImage(stResourceLocation("data\\interfaces3.gl",113,25));
	}


	if(tianxia[8].saidian >=2)
	{
		GetStatic(302)->SetText(tianxia[8].username);
		GetStatic(302)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2005)->SetImage(stResourceLocation("data\\interfaces3.gl",113,16));
		
	}
	else if(tianxia[9].saidian >=2)
	{
		GetStatic(302)->SetText(tianxia[9].username);
		GetStatic(302)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2005)->SetImage(stResourceLocation("data\\interfaces3.gl",113,16));
	}

	else if(tianxia[12].saidian >=2)
	{
		GetStatic(302)->SetText(tianxia[12].username);
		GetStatic(302)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2006)->SetImage(stResourceLocation("data\\interfaces3.gl",113,17));
	}
	else if(tianxia[13].saidian >=2)
	{
		GetStatic(302)->SetText(tianxia[13].username);
		GetStatic(302)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2006)->SetImage(stResourceLocation("data\\interfaces3.gl",113,17));
	}

	if(tianxia[10].saidian >=2)
	{
		GetStatic(304)->SetText(tianxia[10].username);
		GetStatic(304)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2007)->SetImage(stResourceLocation("data\\interfaces3.gl",113,24));
		
	}
	else if(tianxia[11].saidian >=2)
	{
		GetStatic(304)->SetText(tianxia[11].username);
		GetStatic(304)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2007)->SetImage(stResourceLocation("data\\interfaces3.gl",113,24));
	}

	else if(tianxia[14].saidian >=2)
	{
		GetStatic(304)->SetText(tianxia[14].username);
		GetStatic(304)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2008)->SetImage(stResourceLocation("data\\interfaces3.gl",113,25));
	}
	else if(tianxia[15].saidian >=2)
	{
		GetStatic(304)->SetText(tianxia[15].username);
		GetStatic(304)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2008)->SetImage(stResourceLocation("data\\interfaces3.gl",113,25));
	}

	if(tianxia[16].saidian >=2)
	{
		GetStatic(305)->SetText(tianxia[16].username);
		GetStatic(305)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2009)->SetImage(stResourceLocation("data\\interfaces3.gl",113,16));
		
	}
	else if(tianxia[17].saidian >=2)
	{
		GetStatic(305)->SetText(tianxia[17].username);
		GetStatic(305)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2009)->SetImage(stResourceLocation("data\\interfaces3.gl",113,16));
	}

	else if(tianxia[20].saidian >=2)
	{
		GetStatic(305)->SetText(tianxia[20].username);
		GetStatic(305)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2010)->SetImage(stResourceLocation("data\\interfaces3.gl",113,17));
	}
	else if(tianxia[21].saidian >=2)
	{
		GetStatic(305)->SetText(tianxia[21].username);
		GetStatic(305)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2010)->SetImage(stResourceLocation("data\\interfaces3.gl",113,17));
	}

	if(tianxia[18].saidian >=2)
	{
		GetStatic(307)->SetText(tianxia[18].username);
		GetStatic(307)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2011)->SetImage(stResourceLocation("data\\interfaces3.gl",113,24));
		
	}
	else if(tianxia[19].saidian >=2)
	{
		GetStatic(307)->SetText(tianxia[19].username);
		GetStatic(307)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2011)->SetImage(stResourceLocation("data\\interfaces3.gl",113,24));
	}

	else if(tianxia[22].saidian >=2)
	{
		GetStatic(307)->SetText(tianxia[22].username);
		GetStatic(307)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2012)->SetImage(stResourceLocation("data\\interfaces3.gl",113,25));
	}
	else if(tianxia[23].saidian >=2)
	{
		GetStatic(307)->SetText(tianxia[23].username);
		GetStatic(307)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2012)->SetImage(stResourceLocation("data\\interfaces3.gl",113,25));
	}

	if(tianxia[24].saidian >=2)
	{
		GetStatic(306)->SetText(tianxia[24].username);
		GetStatic(306)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2013)->SetImage(stResourceLocation("data\\interfaces3.gl",113,16));
		
	}
	else if(tianxia[25].saidian >=2)
	{
		GetStatic(306)->SetText(tianxia[25].username);
		GetStatic(306)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2013)->SetImage(stResourceLocation("data\\interfaces3.gl",113,16));
	}

	else if(tianxia[28].saidian >=2)
	{
		GetStatic(306)->SetText(tianxia[28].username);
		GetStatic(306)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2014)->SetImage(stResourceLocation("data\\interfaces3.gl",113,17));
	}
	else if(tianxia[29].saidian >=2)
	{
		GetStatic(306)->SetText(tianxia[29].username);
		GetStatic(306)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2014)->SetImage(stResourceLocation("data\\interfaces3.gl",113,17));
	}

	if(tianxia[26].saidian >=2)
	{
		GetStatic(308)->SetText(tianxia[26].username);
		GetStatic(308)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2015)->SetImage(stResourceLocation("data\\interfaces3.gl",113,24));
		
	}
	else if(tianxia[27].saidian >=2)
	{
		GetStatic(308)->SetText(tianxia[27].username);
		GetStatic(308)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2015)->SetImage(stResourceLocation("data\\interfaces3.gl",113,24));
	}

	else if(tianxia[30].saidian >=2)
	{
		GetStatic(308)->SetText(tianxia[30].username);
		GetStatic(308)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2016)->SetImage(stResourceLocation("data\\interfaces3.gl",113,25));
	}
	else if(tianxia[31].saidian >=2)
	{
		GetStatic(308)->SetText(tianxia[31].username);
		GetStatic(308)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(2016)->SetImage(stResourceLocation("data\\interfaces3.gl",113,25));
	}



	//四强
	if(tianxia[0].saidian >=3)
	{
		GetStatic(401)->SetText(tianxia[0].username);
		GetStatic(401)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3001)->SetImage(stResourceLocation("data\\interfaces3.gl",113,14));
		
	}
	else if(tianxia[1].saidian >=3)
	{
		GetStatic(401)->SetText(tianxia[1].username);
		GetStatic(401)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3001)->SetImage(stResourceLocation("data\\interfaces3.gl",113,14));
	}

	else if(tianxia[4].saidian >=3)
	{
		GetStatic(401)->SetText(tianxia[4].username);
		GetStatic(401)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3001)->SetImage(stResourceLocation("data\\interfaces3.gl",113,14));
	}
	else if(tianxia[5].saidian >=3)
	{
		GetStatic(401)->SetText(tianxia[5].username);
		GetStatic(401)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3001)->SetImage(stResourceLocation("data\\interfaces3.gl",113,14));
	}
	else if(tianxia[8].saidian >=3)
	{
		GetStatic(401)->SetText(tianxia[8].username);
		GetStatic(401)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3002)->SetImage(stResourceLocation("data\\interfaces3.gl",113,15));
		
	}
	else if(tianxia[9].saidian >=3)
	{
		GetStatic(401)->SetText(tianxia[9].username);
		GetStatic(401)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3002)->SetImage(stResourceLocation("data\\interfaces3.gl",113,15));
	}

	else if(tianxia[13].saidian >=3)
	{
		GetStatic(401)->SetText(tianxia[13].username);
		GetStatic(401)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3002)->SetImage(stResourceLocation("data\\interfaces3.gl",113,15));
	}
	else if(tianxia[14].saidian >=3)
	{
		GetStatic(401)->SetText(tianxia[14].username);
		GetStatic(401)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3002)->SetImage(stResourceLocation("data\\interfaces3.gl",113,15));
	}



	if(tianxia[2].saidian >=3)
	{
		GetStatic(403)->SetText(tianxia[2].username);
		GetStatic(403)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3003)->SetImage(stResourceLocation("data\\interfaces3.gl",113,22));
		
	}
	else if(tianxia[3].saidian >=3)
	{
		GetStatic(403)->SetText(tianxia[3].username);
		GetStatic(403)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3003)->SetImage(stResourceLocation("data\\interfaces3.gl",113,22));
	}

	else if(tianxia[6].saidian >=3)
	{
		GetStatic(403)->SetText(tianxia[6].username);
		GetStatic(403)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3003)->SetImage(stResourceLocation("data\\interfaces3.gl",113,22));
	}
	else if(tianxia[7].saidian >=3)
	{
		GetStatic(403)->SetText(tianxia[7].username);
		GetStatic(403)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3003)->SetImage(stResourceLocation("data\\interfaces3.gl",113,22));
	}
	else if(tianxia[10].saidian >=3)
	{
		GetStatic(403)->SetText(tianxia[10].username);
		GetStatic(403)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3004)->SetImage(stResourceLocation("data\\interfaces3.gl",113,23));
		
	}
	else if(tianxia[11].saidian >=3)
	{
		GetStatic(403)->SetText(tianxia[11].username);
		GetStatic(403)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3004)->SetImage(stResourceLocation("data\\interfaces3.gl",113,23));
	}

	else if(tianxia[14].saidian >=3)
	{
		GetStatic(403)->SetText(tianxia[14].username);
		GetStatic(403)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3004)->SetImage(stResourceLocation("data\\interfaces3.gl",113,23));
	}
	else if(tianxia[15].saidian >=3)
	{
		GetStatic(403)->SetText(tianxia[15].username);
		GetStatic(403)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3004)->SetImage(stResourceLocation("data\\interfaces3.gl",113,23));
	}


	if(tianxia[16].saidian >=3)
	{
		GetStatic(402)->SetText(tianxia[16].username);
		GetStatic(402)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3005)->SetImage(stResourceLocation("data\\interfaces3.gl",113,14));
		
	}
	else if(tianxia[17].saidian >=3)
	{
		GetStatic(402)->SetText(tianxia[17].username);
		GetStatic(402)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3005)->SetImage(stResourceLocation("data\\interfaces3.gl",113,14));
	}

	else if(tianxia[20].saidian >=3)
	{
		GetStatic(402)->SetText(tianxia[20].username);
		GetStatic(402)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3005)->SetImage(stResourceLocation("data\\interfaces3.gl",113,14));
	}
	else if(tianxia[21].saidian >=3)
	{
		GetStatic(402)->SetText(tianxia[21].username);
		GetStatic(402)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3005)->SetImage(stResourceLocation("data\\interfaces3.gl",113,14));
	}
	else if(tianxia[24].saidian >=3)
	{
		GetStatic(402)->SetText(tianxia[24].username);
		GetStatic(402)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3006)->SetImage(stResourceLocation("data\\interfaces3.gl",113,15));
		
	}
	else if(tianxia[25].saidian >=3)
	{
		GetStatic(402)->SetText(tianxia[25].username);
		GetStatic(402)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3006)->SetImage(stResourceLocation("data\\interfaces3.gl",113,15));
	}

	else if(tianxia[28].saidian >=3)
	{
		GetStatic(402)->SetText(tianxia[28].username);
		GetStatic(402)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3006)->SetImage(stResourceLocation("data\\interfaces3.gl",113,15));
	}
	else if(tianxia[29].saidian >=3)
	{
		GetStatic(402)->SetText(tianxia[29].username);
		GetStatic(402)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3006)->SetImage(stResourceLocation("data\\interfaces3.gl",113,15));
	}
	
	if(tianxia[18].saidian >=3)
	{
		GetStatic(404)->SetText(tianxia[18].username);
		GetStatic(404)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3007)->SetImage(stResourceLocation("data\\interfaces3.gl",113,22));
		
	}
	else if(tianxia[19].saidian >=3)
	{
		GetStatic(404)->SetText(tianxia[19].username);
		GetStatic(404)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3007)->SetImage(stResourceLocation("data\\interfaces3.gl",113,22));
	}

	else if(tianxia[22].saidian >=3)
	{
		GetStatic(404)->SetText(tianxia[22].username);
		GetStatic(404)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3007)->SetImage(stResourceLocation("data\\interfaces3.gl",113,22));
	}
	else if(tianxia[23].saidian >=3)
	{
		GetStatic(404)->SetText(tianxia[23].username);
		GetStatic(404)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3007)->SetImage(stResourceLocation("data\\interfaces3.gl",113,22));
	}
	else if(tianxia[26].saidian >=3)
	{
		GetStatic(404)->SetText(tianxia[26].username);
		GetStatic(404)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3008)->SetImage(stResourceLocation("data\\interfaces3.gl",113,23));
		
	}
	else if(tianxia[27].saidian >=3)
	{
		GetStatic(404)->SetText(tianxia[27].username);
		GetStatic(404)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3008)->SetImage(stResourceLocation("data\\interfaces3.gl",113,23));
	}

	else if(tianxia[30].saidian >=3)
	{
		GetStatic(404)->SetText(tianxia[30].username);
		GetStatic(404)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3008)->SetImage(stResourceLocation("data\\interfaces3.gl",113,23));
	}
	else if(tianxia[31].saidian >=3)
	{
		GetStatic(404)->SetText(tianxia[31].username);
		GetStatic(404)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(3008)->SetImage(stResourceLocation("data\\interfaces3.gl",113,23));
	}

	
	if(tianxia[0].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[0].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4001)->SetImage(stResourceLocation("data\\interfaces3.gl",113,12));
		
	}
	else if(tianxia[1].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[1].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4001)->SetImage(stResourceLocation("data\\interfaces3.gl",113,12));
		
	}
	else if(tianxia[4].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[4].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4001)->SetImage(stResourceLocation("data\\interfaces3.gl",113,12));
		
	}
	else if(tianxia[5].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[5].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4001)->SetImage(stResourceLocation("data\\interfaces3.gl",113,12));
		
	}
	else if(tianxia[8].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[8].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4001)->SetImage(stResourceLocation("data\\interfaces3.gl",113,12));
		
	}
	else if(tianxia[9].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[9].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4001)->SetImage(stResourceLocation("data\\interfaces3.gl",113,12));
		
	}
	else if(tianxia[12].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[12].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4001)->SetImage(stResourceLocation("data\\interfaces3.gl",113,12));
		
	}
	else if(tianxia[13].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[13].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4001)->SetImage(stResourceLocation("data\\interfaces3.gl",113,12));
		
	}
	else if(tianxia[16].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[16].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4002)->SetImage(stResourceLocation("data\\interfaces3.gl",113,13));
		
	}
	else if(tianxia[17].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[17].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4002)->SetImage(stResourceLocation("data\\interfaces3.gl",113,13));
		
	}

	else if(tianxia[20].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[20].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4002)->SetImage(stResourceLocation("data\\interfaces3.gl",113,13));
		
	}
	else if(tianxia[21].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[21].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4002)->SetImage(stResourceLocation("data\\interfaces3.gl",113,13));
		
	}

	else if(tianxia[24].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[24].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4002)->SetImage(stResourceLocation("data\\interfaces3.gl",113,13));
		
	}
	else if(tianxia[25].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[25].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4002)->SetImage(stResourceLocation("data\\interfaces3.gl",113,13));
		
		
	}

	else if(tianxia[28].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[28].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4002)->SetImage(stResourceLocation("data\\interfaces3.gl",113,13));
		
	}
	else if(tianxia[29].saidian >=4)
	{
		GetStatic(501)->SetText(tianxia[29].username);
		GetStatic(501)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4002)->SetImage(stResourceLocation("data\\interfaces3.gl",113,13));
		
	}



	if(tianxia[2].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[2].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4003)->SetImage(stResourceLocation("data\\interfaces3.gl",113,20));
		
	}
	else if(tianxia[3].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[3].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4003)->SetImage(stResourceLocation("data\\interfaces3.gl",113,20));
		
	}
	else if(tianxia[6].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[6].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4003)->SetImage(stResourceLocation("data\\interfaces3.gl",113,20));
		
	}
	else if(tianxia[7].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[7].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4003)->SetImage(stResourceLocation("data\\interfaces3.gl",113,20));
		
	}
	else if(tianxia[10].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[10].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4003)->SetImage(stResourceLocation("data\\interfaces3.gl",113,20));
		
	}
	else if(tianxia[11].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[11].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4003)->SetImage(stResourceLocation("data\\interfaces3.gl",113,20));
		
	}
	else if(tianxia[14].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[14].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4003)->SetImage(stResourceLocation("data\\interfaces3.gl",113,20));
		
	}
	else if(tianxia[15].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[15].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4003)->SetImage(stResourceLocation("data\\interfaces3.gl",113,20));
		
	}
	else if(tianxia[18].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[18].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4004)->SetImage(stResourceLocation("data\\interfaces3.gl",113,21));
		
	}
	else if(tianxia[19].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[19].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4004)->SetImage(stResourceLocation("data\\interfaces3.gl",113,21));
		
	}

	else if(tianxia[22].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[22].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4004)->SetImage(stResourceLocation("data\\interfaces3.gl",113,21));
		
	}
	else if(tianxia[23].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[23].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4004)->SetImage(stResourceLocation("data\\interfaces3.gl",113,21));
		
	}

	else if(tianxia[26].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[26].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4004)->SetImage(stResourceLocation("data\\interfaces3.gl",113,21));
		
	}
	else if(tianxia[27].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[27].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4004)->SetImage(stResourceLocation("data\\interfaces3.gl",113,21));
		
		
	}

	else if(tianxia[30].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[30].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4004)->SetImage(stResourceLocation("data\\interfaces3.gl",113,21));
		
	}
	else if(tianxia[31].saidian >=4)
	{
		GetStatic(502)->SetText(tianxia[31].username);
		GetStatic(502)->SetTextColor(COLOR_ARGB(255,0,255,0));
		GetImage(4004)->SetImage(stResourceLocation("data\\interfaces3.gl",113,21));
		
	}

	for(int i=0;i<32;i++)
	{
		if(tianxia[i].userid != 0 && tianxia[i].saidian == 5)
		{
			GetStatic(601)->SetText(tianxia[i].username); //冠军
			GetStatic(601)->SetTextColor(COLOR_ARGB(255,255,0,0));
		}
	}

	FUNCTION_END;
}
