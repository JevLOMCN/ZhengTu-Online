#pragma once

#include "..\gui\include\guidialog.h"
#include "GuiNpcMake.h"

enum EQUIPRECAST_PAGE
{
	//EQUIPRECAST_REBIND,			// 斗魂升级
	//EQUIPRECAST_EQUIPCOMPOSE,	// 装备祝福
	//EQUIPRECAST_BIND,			// 装备解绑
	//EQUIPRECAST_ADDSIGNATURE,	// 增加签名
	//EQUIPRECAST_MODIFYSIGNATURE,// 修改签名
	//EQUIPRECAST_ZBIND,			// 修改灵魂属性
	//EQUIPRECAST_ZIBIND,			// 保留灵魂链接改造

	EQUIPRECAST_COMPOSE11,   //装备祝福
	EQUIPRECAST_COMPOSE12,   //斗魂升级
	EQUIPRECAST_COMPOSE13,   //神石镶嵌
	EQUIPRECAST_COMPOSE14,   //装备栏激活
	EQUIPRECAST_COMPOSE15,   //装备栏进阶
	EQUIPRECAST_COMPOSE16,   //神器强化
	EQUIPRECAST_COMPOSE17,   //龙槽激活
	EQUIPRECAST_COMPOSE18,   //龙槽解封
	EQUIPRECAST_COMPOSE19,   //龙星升级
	EQUIPRECAST_COMPOSE20,   //魂魄清除
	EQUIPRECAST_COMPOSE21,   //装备升级
	EQUIPRECAST_COMPOSE22,   //马牌精炼
	EQUIPRECAST_COMPOSE23,   //装备神龙镶嵌	
	EQUIPRECAST_COMPOSE24,   //装备逆天进阶
	EQUIPRECAST_COMPOSE25,   //装备斗战圣佛进阶
	EQUIPRECAST_COMPOSE26,   //装备暗影进阶	
	EQUIPRECAST_COMPOSE27,   //龙凤吟	
	EQUIPRECAST_COMPOSE28,   //轰天宝石镶嵌	
	EQUIPRECAST_COMPOSE29,   //王者武器	

};

enum RecastAction;
class CMyTimer;
struct stResponsePropertyUserCmd;
class CDlgEquipRecast :
	public CGuiDialog
{
	enum
	{
		TOTAL_MATERIAL_TABLE = 1,// 材料格子的总数
		TOTAL_PAGE_NUM = 19,	 // 装备改造总分页数
	};

public:
	CDlgEquipRecast();
	~CDlgEquipRecast();

	void OnPutFocusItem(bool bPut);
	void OnResult(stResponsePropertyUserCmd* pCmd);
	
public:
	CGuiTable*	m_pTableFocusItem; // 待操作的item
	CGuiTable*	m_pTableMaterial1;

private:
	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnSetCloseLocation();
	HRESULT OnRender(float fElapsedTime);

private:


	bool CanComposeEquip1();     //是否可以进行装备祝福
	bool CanComposeEquip2();     //是否可以进行斗魂升级
	bool CanComposeEquip3();     //是否可以进行神石镶嵌
	bool CanComposeEquip4();     //是否可以进行装备栏激活
	bool CanComposeEquip5();     //是否可以进行装备栏进阶
	bool CanComposeEquip6();     //是否可以进行神器强化
	bool CanComposeEquip7();    //是否可以进行龙槽激活
	bool CanComposeEquip8();    //是否可以进行龙槽解封
	bool CanComposeEquip9();    //是否可以进行龙星升级
	bool CanComposeEquip10();    //是否可以进行魂魄清除
	bool CanComposeEquip11();    //是否可以进行装备升级
	bool CanComposeEquip12();    //是否可以进行马牌精炼
	bool CanComposeEquip13();    //是否可以进行装备神龙镶嵌	
	bool CanComposeEquip14();    //是否可以进行装备逆天进阶	
	bool CanComposeEquip15();    //是否可以进行装备斗战圣佛装备进阶	
	bool CanComposeEquip16();    //是否可以进行装备暗影石镶嵌	
	bool CanComposeEquip17();   //是否可以进行龙凤吟镶嵌		
	bool CanComposeEquip18();   //是否可以进行轰天宝石镶嵌
	bool CanComposeEquip19();    //是否可以进行王者升级						

	CRoleItem* GetFocusItem();
	CRoleItem* GetMaterialItem(int index); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
	bool IsEquipItem(enumItemType eType);  // 是否为装备类物品

	void SendEquipRecastMsg(RecastAction eAction);
	void OnTimeUp();
	void SetBGImage();

	void RefreshPage();


	void RefreshEquipCompose1();    //刷新装备祝福页面信息
	void RefreshEquipCompose2();    //刷新斗魂升级页面信息
	void RefreshEquipCompose3();    //刷新神石镶嵌页面信息
	void RefreshEquipCompose4();    //刷新装备栏激活页面信息
	void RefreshEquipCompose5();    //刷新装备栏进阶页面信息
	void RefreshEquipCompose6();    //刷新神器强化页面信息
	void RefreshEquipCompose7();   //刷新龙槽激活页面信息
	void RefreshEquipCompose8();   //刷新龙槽解封页面信息
	void RefreshEquipCompose9();   //刷新龙星升级页面信息
	void RefreshEquipCompose10();   //刷新魂魄清除页面信息
	void RefreshEquipCompose11();    //刷新装备升级页面信息
	void RefreshEquipCompose12();    //刷新马牌精炼页面信息
	void RefreshEquipCompose13();    //刷新装备神龙镶嵌页面信息	
	void RefreshEquipCompose14();    //刷新装备逆天进阶页面信息
	void RefreshEquipCompose15();    //刷新装备斗战圣佛进阶页面信息
	void RefreshEquipCompose16();    //刷新装备暗影宝石镶嵌页面信息	
	void RefreshEquipCompose17();   //刷新龙凤吟镶嵌页面信息
	void RefreshEquipCompose18();   //刷新轰天宝石镶嵌页面信息
	void RefreshEquipCompose19();    //刷新装备王者升级页面信息
					


private:
	CGuiStatic*			m_pStaConsumeNum;
	CGuiStatic*			m_pStaSuccessRate;
	CGuiStatic*			m_pStaNeedMoney;
	CGuiStatic*			m_pStaResult;
	CGuiTab*			m_pTabEquipRecast;
//	CGuiProcess*		m_pProcessTimer;
	CGuiButton*			m_pBtnPages[TOTAL_PAGE_NUM];
	CGuiTable*			m_pTableMaterials[TOTAL_MATERIAL_TABLE];
	EQUIPRECAST_PAGE	m_eCurPage;

	CMakeData	m_MakeData; // 打造数据类
	CMyTimer*	m_pTimerProcess;
	std::string m_strTitle;
	CAnimation	m_aniSuccess;
	bool		m_bPutFocusItem;
	UINT		m_unSoulMountNeedMoney;
};
