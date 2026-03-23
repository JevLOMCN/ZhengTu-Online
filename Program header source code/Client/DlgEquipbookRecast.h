#pragma once

#include "..\gui\include\guidialog.h"
#include "GuiNpcMake.h"

enum EQUIPBOOKRECAST_PAGE
{

	EQUIPBOOKRECAST_COMPOSE101,   //图鉴进阶
	EQUIPBOOKRECAST_COMPOSE102,   //图鉴签名

};

enum RecastAction;
class CMyTimer;
struct stResponsePropertyUserCmd;
class CDlgEquipbookRecast :
	public CGuiDialog
{
	enum
	{
		TOTAL_SMATERIAL_TABLE = 1,// 材料格子的总数
		TOTAL_PAGE_NUM = 2,	 // 装备改造总分页数
	};

public:
	CDlgEquipbookRecast();
	~CDlgEquipbookRecast();

	void OnPutFocusItembook(bool bPut);
	void OnResult(stResponsePropertyUserCmd* pCmd);
	
public:
	CGuiTable*	m_pTableSFocusItem; // 待操作的item
	CGuiTable*	m_pTableSMaterial1;

private:
	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnSetCloseLocation();
	HRESULT OnRender(float fElapsedTime);

private:


	bool CanComposeEquip101();    //是否可以进行图鉴强化
	bool CanComposeEquip102();    //是否可以进行图鉴签名

	CRoleItem* GetFocusItem();
	CRoleItem* GetMaterialItem(int index); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
	bool IsEquipItem(enumItemType eType);  // 是否为装备类物品

	void SendEquipbookRecastMsg(RecastAction eAction);
	void OnTimeUp();
	void SetBGImage();

	void RefreshPage();


	void RefreshEquipCompose101();    //刷新图鉴进阶页面信息
	void RefreshEquipCompose102();    //刷新图鉴进阶页面信息



private:
	CGuiStatic*			m_pStaConsumeNum;
	CGuiStatic*			m_pStaSuccessRate;
	CGuiStatic*			m_pStaNeedMoney;
	CGuiStatic*			m_pStaResult;
	CGuiTab*			m_pTabEquipbookRecast;
//	CGuiProcess*		m_pProcessTimer;
	CGuiButton*			m_pBtnPages[TOTAL_PAGE_NUM];
	CGuiTable*			m_pTableSMaterials[TOTAL_SMATERIAL_TABLE];
	EQUIPBOOKRECAST_PAGE	m_eCurPage;

	CMakeData	m_MakeData; // 打造数据类
	CMyTimer*	m_pTimerProcess;
	std::string m_strTitle;
	CAnimation	m_aniSuccess;
	bool		m_bPutFocusItem;
	UINT		m_unSoulMountNeedMoney;
};
