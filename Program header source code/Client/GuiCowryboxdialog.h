/**
*\file		GuiCowrybox.h
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2007-11-26  17:20:00 中国标准时间
*\brief	    新加宝盒对话框
*
* 
*
*/

#pragma once
#include "..\gui\include\guidialog.h"

#include <map>


//class CGuiEquip;
/**
 * \brief 宝盒对话框
 * 
 * 宝盒对话框
 * 
 */
class CGuiCowrybox :
	public CGuiDialog
{
public:

	//CGuiEquip* m_pEquip;
	///宝盒动画控制标志
	bool           m_TiquTy;  //用来检测提取触发时宝盒是否转动过
	int            m_Endid;
	int            m_tlbe;
	int            m_Zzhuans;  //整个箱子转动的次数
	int            m_Jzhuans;  //当前的转动位置
	DWORD          m_MyLTime;  //用于记录当前的系统时间
	int            m_sheep;    //用于控制动画的播放速度
	bool           m_TaleTy;
	bool           m_AniEnd;   //转动动画播放完毕标志
	DWORD			m_Keyid;
	//宝盒转动时候最终停止的格子ID
	int           m_TaleEnd;

	bool m_bShowEquip;

	CAnimation     m_aniBegin;

	bool           m_aniOk;


	CGuiCowrybox(void);
	void OnClose(void);
	void OnCreate();
	
	///宝盒网格
	CGuiTable*    m_pTable[17];
	CGuiItemCell*	m_arrayCell;


	//添加宝盒物品
	void AddItem(stGetCowBoxInitCmd* pCmd);
	void AddItem(CGuiTable*  pTable,DWORD id, WORD buffe);
	bool ComparisonTime(int sheep);   //为箱子动画的速度而添加的时间检测函数

	//箱子动画
	void DingTime();
	void DingTime1();
	void DingTime2();

	void SetCharName(const char* name);

	void SetCharType(int type);

	void OnInitScript();

	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	HRESULT OnRender(float fElapsedTime);

	void AllSetVisible(bool visible);
};
