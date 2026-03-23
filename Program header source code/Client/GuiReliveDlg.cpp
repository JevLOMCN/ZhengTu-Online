/**
 *\file		GuiReliveDlg.cpp
 *\version	$ID$
 *\author	$$$04公司
 *			$$$04公司@###.com
 *\date		2005-07-04 15:35:00
 *\brief	重生对话框
 *
 *主角死亡后通过本对话框选择重生方式
 *
 */
#include "public.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "GuiReliveDlg.h"
#include ".\gamestate.h"
#include "RoleItem.h"
#include "./GuiMessageBox.h"
#include "../gui/include/GuiManager.h"
#include "GameScene.h"
#include "MainCharacter.h"

///回城复活按钮id
static const int csBackCityReliveID = 2;

///银子复活按钮id
static const int csMoneyReliveID = 3;

///技能复活按钮id
//static const int csSkillReliveID = 4;

///soke 原地健康银子复活按钮id
static const int csMaxeyReliveID = 8;

/**
 * \brief 构造函数
 * 
 */
CGuiReliveDlg::CGuiReliveDlg()
{
	FUNCTION_BEGIN;

	m_dwUserTempID = 0;
	m_dwDeadType = 0;
	m_bSmDialog = true;

	FUNCTION_END;
}

/**
 * \brief 构造函数
 * 
 * \param dwID : 主角临时id
 */
CGuiReliveDlg::CGuiReliveDlg(DWORD dwID,DWORD dwDeadType)
{
	FUNCTION_BEGIN;

	m_dwUserTempID = dwID;
	m_bSmDialog = true;
	m_dwDeadType = dwDeadType;

	FUNCTION_END;
}

/**
 * \brief 析构函数
 * 
 */
CGuiReliveDlg::~CGuiReliveDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
 * \brief "创建消息"响应函数
 * 
 * 暂时disable 技能复活和银子复活
 * 
 * \return void
 */
void CGuiReliveDlg::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	//暂时disable 技能复活和银子复活
	CGuiButton *pMoneyReliveBtn = GetButton(csMoneyReliveID);
	if(pMoneyReliveBtn ) pMoneyReliveBtn->SetEnabled(false);

	//soke 增加原地健康复活
	CGuiButton *pMaxeyReliveBtn = GetButton(csMaxeyReliveID);
	if(pMaxeyReliveBtn ) pMaxeyReliveBtn->SetEnabled(false);

	CMainCharacter *pMainChar = GetScene()->GetMainCharacter();
	int money = GetReliveMoney();
	int gold = GetReliveMaxey();

	if(pMainChar && money && gold != -1)
	{
		DWORD dwMoney = pMainChar->GetMoney();
		if(dwMoney >= money)
		{
			if(pMoneyReliveBtn ) pMoneyReliveBtn->SetEnabled(true);

			char buf[MAX_PATH];
			sprintf(buf,"原地复活需要银子 %s \n并带来 %d 秒的\"虚弱\"状态 ",Money2String(money),pMainChar->GetLevel() * 3); 
			pMoneyReliveBtn->SetToolTips(buf);
		}

		if(dwMoney >= gold)
		{
			if(pMaxeyReliveBtn ) pMaxeyReliveBtn->SetEnabled(true);

			char buf[MAX_PATH];
			sprintf(buf,"原地健康康复需要银子 %s \n没有任何属性值降低的效果",Money2String(gold) ); 
		//	sprintf(buf,"原地健康康复需要银子 %s \n没有任何属性值降低的效果 \n这是你的第 %d 次原地复活",Money2String(gold) ); 
			pMaxeyReliveBtn->SetToolTips(buf);
		}

		if( m_dwDeadType == 0 )
		{
			GetButton(4)->SetEnabled(false);
			GetButton(6)->SetEnabled(false);
			GetButton(7)->SetEnabled(false);
		}
		else
		{
			bool bRelive1 = (dwMoney >= money);
			bool bRelive5 = (dwMoney >= 5*money);
			bool bRelive10 = (dwMoney >= 10*money);

			char buf[MAX_PATH];

			//
			_snprintf( buf,MAX_PATH,"抓捕一分钟需要银子 %s ",Money2String(money) );
			buf[MAX_PATH - 1] = 0;
			if( bRelive1 )
			{
				GetButton(4)->SetToolTips( buf );
			}

			_snprintf( buf,MAX_PATH,"抓捕五分钟需要银子 %s ",Money2String(money*5) );
			buf[MAX_PATH - 1] = 0;
			if( bRelive5 )
			{
				GetButton(6)->SetToolTips( buf );
			}

			_snprintf( buf,MAX_PATH,"抓捕十分钟需要银子 %s ",Money2String(money*10) );
			buf[MAX_PATH - 1] = 0;
			if( bRelive10 )
			{
				GetButton(7)->SetToolTips( buf );
			}

			GetButton(4)->SetEnabled(bRelive1);
			GetButton(6)->SetEnabled(bRelive5);
			GetButton(7)->SetEnabled(bRelive10);
		}
	}


	//if( GetDevice()->GetHeight() < 768 )
	{
		m_x = GetDevice()->GetWidth() / 2 + 30;
	}

	FUNCTION_END;
}

int	 CGuiReliveDlg::GetReliveMoney()
{
	CMainCharacter *pMainChar = GetScene()->GetMainCharacter();
	if(pMainChar)
	{
		DWORD dwMoney = pMainChar->GetMoney();

		int level = pMainChar->GetLevel();
		float money = level * level / 2.0f;
		if( level <= 40 )
		{
		//soke 最大的倍数x0.4 得到小于40级的倍数
		//	return (int)(money * 0.4f + 0.5f);
			return (int)(money * 16.0f + 0.5f); //soke 服务端的价格改了 客户端也得改
		}
	//	return (int)(money+0.5f);
		return (int)(money * 40.0f +0.5f); //soke 新的算法是服务端的2倍
	}
	return -1;
}

int	 CGuiReliveDlg::GetReliveMaxey()
{//soke 新原地满状态复活
	CMainCharacter *pMainChar = GetScene()->GetMainCharacter();
	if(pMainChar)
	{
		DWORD dwMoney = pMainChar->GetMoney();

		int level = pMainChar->GetLevel();

		float gold = level * level / 2.0f;
		if( level <= 40 )
		{
		//soke 最大的倍数x0.4 得到小于40级的倍数
		//	return (int)(money * 0.4f + 0.5f);
			return (int)(gold * 24.0f + 0.5f); //soke 服务端的价格改了 客户端也得改
		}
	//	return (int)(money+0.5f);
		return (int)(gold * 60.0f +0.5f); //soke 新的算法是服务端的2倍
	}
	return -1;
}

/**
 * \brief GUI消息处理函数
 * 
 * 处理重生方式选择按钮点击消息，发送重生消息
 * 
 * \param nEvent : 消息句柄
 * \param nID : 控件id
 * \param pControl : 控件指针
 * \return true if succeed else false
 */
bool CGuiReliveDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
		if( pMainRole && !pMainRole->IsDie() )
		{
			Close();
			return true;
		}

#define SEND_USER_OK_RELIVED_CMD(t)	{stOKReliveUserCmd cmd;cmd.dwUserTempID = m_dwUserTempID;cmd.byType = t;SEND_USER_CMD(cmd);Close();}
#define RELIVE_CASE_BTN_CLICK(id,t)	{case id:SEND_USER_OK_RELIVED_CMD(t);break;}	
		switch(nID)
		{
			RELIVE_CASE_BTN_CLICK(2,ReliveHome);		//回城复活
			RELIVE_CASE_BTN_CLICK(3,ReliveMoney);		//银子复活
			RELIVE_CASE_BTN_CLICK(4,Relive_1_min);		//追捕凶犯1分钟
			RELIVE_CASE_BTN_CLICK(6,Relive_5_min);		//追捕凶犯5分钟
			RELIVE_CASE_BTN_CLICK(7,Relive_10_min);		//追捕凶犯10分钟
			RELIVE_CASE_BTN_CLICK(8,ReliveMaxey);		//原地满状态银子复活
		default:break;
		}
		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}


/**
* \brief 对话框关闭消息处理
* 
* 退出对话框、更新管理器对应的指针
*
* \return void
*/
void CGuiReliveDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameState()->SetStateItem(eGameStateItem_Normal);
	GetGameGuiManager()->m_guiReliveDlg = NULL;

	FUNCTION_END;
}
