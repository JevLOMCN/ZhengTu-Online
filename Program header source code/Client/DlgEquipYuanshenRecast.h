#pragma once

#include "..\gui\include\guidialog.h"
#include "GuiNpcMake.h"

enum EQUIPYUANSHENRECAST_PAGE
{

	EQUIPYUANSHENRECAST_COMPOSE106,   //图鉴进阶
	EQUIPYUANSHENRECAST_COMPOSE107,   //图鉴签名
	EQUIPYUANSHENRECAST_COMPOSE108,   //宝石镶嵌

};

enum RecastAction;
class CMyTimer;
struct stResponsePropertyUserCmd;
class CDlgEquipYuanshenRecast :
	public CGuiDialog
{
	enum
	{
		TOTAL_SMATERIAL_TABLE = 1,// 材料格子的总数
		TOTAL_PAGE_NUM = 3,	 // 装备改造总分页数
	};

public:
	CDlgEquipYuanshenRecast();
	~CDlgEquipYuanshenRecast();

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


	bool CanComposeEquip106();    //是否可以进行图鉴强化
	bool CanComposeEquip107();    //是否可以进行图鉴签名
	bool CanComposeEquip108();    //是否可以进行图鉴签名

	CRoleItem* GetFocusItem();
	CRoleItem* GetMaterialItem(int index); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
	bool IsEquipItem(enumItemType eType);  // 是否为装备类物品
	bool IsEquipItem2(enumItemType eType);  // 是否为装备类物品

	void SendEquipYuanshenRecastMsg(RecastAction eAction);
	void OnTimeUp();
	void SetBGImage();

	void RefreshPage();


	void RefreshEquipCompose106();    //刷新图鉴进阶页面信息
	void RefreshEquipCompose107();    //刷新图鉴进阶页面信息
	void RefreshEquipCompose108();    //刷新图鉴进阶页面信息



private:
	CGuiStatic*			m_pStaConsumeNum;
	CGuiStatic*			m_pStaSuccessRate;
	CGuiStatic*			m_pStaNeedMoney;
	CGuiStatic*			m_pStaResult;
	CGuiTab*			m_pTabEquipYuanshenRecast;
//	CGuiProcess*		m_pProcessTimer;
	CGuiButton*			m_pBtnPages[TOTAL_PAGE_NUM];
	CGuiTable*			m_pTableSMaterials[TOTAL_SMATERIAL_TABLE];
	EQUIPYUANSHENRECAST_PAGE	m_eCurPage;

	CMakeData	m_MakeData; // 打造数据类
	CMyTimer*	m_pTimerProcess;
	std::string m_strTitle;
	CAnimation	m_aniSuccess;
	bool		m_bPutFocusItem;
	UINT		m_unSoulMountNeedMoney;
};
