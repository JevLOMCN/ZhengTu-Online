/**
*\file		GuiBallot.h
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    投票对话框
*
* 
*
*/

#pragma once
#include "..\gui\include\guidialog.h"

#define  FONTCOLOR COLOR_ARGB(255,246,201,3)
struct stBallot {
	char szBallot[MAX_NAMESIZE];
	char szName[MAX_NAMESIZE];
	int  nBallot;
	char szContext[MAX_PATH];
	stBallot(){
		szBallot[0]=0;
		szName[0]=0;
		nBallot=0;
		szContext[0]=0;
	}
};

/**
 * \brief 帮会,家族投票对话框
 * 
 * 帮会,家族投票对话框
 * 
 */
class CGuiBallot :
	public CGuiDialog
{
	bool                bMaster;
	///投票类型
	byte                 m_eState;
	///投票对象的说明
	CGuiMLEditBox*        m_pEditBox;
	///投票列表
	CGuiListBox*          m_pListBox;
	///当前选择的投票对象
	int                  m_nSelect;
	///玩家是否能进行投票操作
	bool                 m_bIsBallot;

public:
	CGuiBallot(byte  eState);
	void OnSetCloseLocation();

	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void RefreshListBox(char* name);
	void Ballot(char* name,int nSuccess);

	///投票列表结构
	std::vector<stBallot>  m_BallotList;
	
};
