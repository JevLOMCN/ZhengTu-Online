#pragma once

#include "..\gui\include\guidialog.h"
#include "GuiNpcMake.h"

enum EQUIPANNEXPACK_PAGE
{
	EQUIPANNEXPACK_COMPOSE1,   //勋章升级
	EQUIPANNEXPACK_COMPOSE2,   //寒冰升级
	EQUIPANNEXPACK_COMPOSE3,   //翅膀升级
	EQUIPANNEXPACK_COMPOSE4,   //神兵升级
	EQUIPANNEXPACK_COMPOSE5,   //神兽升级
	EQUIPANNEXPACK_COMPOSE6,   //护身符璀璨镶嵌
	EQUIPANNEXPACK_COMPOSE7,   //护身符神兽宝石镶嵌
	EQUIPANNEXPACK_COMPOSE8,   //尚方宝剑升级
	EQUIPANNEXPACK_COMPOSE9,   //紫金龙袍升级
	EQUIPANNEXPACK_COMPOSE10,   //十二星宿升级
	EQUIPANNEXPACK_COMPOSE11,   //神斧
	EQUIPANNEXPACK_COMPOSE12,   //黑曜
	EQUIPANNEXPACK_COMPOSE13,   //精金
	EQUIPANNEXPACK_COMPOSE14,   //金箍咒强化
	EQUIPANNEXPACK_COMPOSE15,   //金箍咒字纹
	EQUIPANNEXPACK_COMPOSE16,   //金箍咒淬炼
	EQUIPANNEXPACK_COMPOSE17,   //寒冰神炼
	EQUIPANNEXPACK_COMPOSE18,   //寒冰字纹
	EQUIPANNEXPACK_COMPOSE19,   //寒冰纹章

};

enum RecastAction;
class CMyTimer;
struct stResponsePropertyUserCmd;
class CDlgEquipAnnexpack :
	public CGuiDialog
{
	enum
	{
		TOTAL_MATERIAL_TABLE = 1,// 材料格子的总数
		TOTAL_PAGE_NUM = 19,	 // 装备改造总分页数
	};

public:
	CDlgEquipAnnexpack();
	~CDlgEquipAnnexpack();

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
	bool CanComposeEquip1();    //是否可以进行勋章升级
	bool CanComposeEquip2();    //是否可以进行寒冰升级
	bool CanComposeEquip3();   //是否可以进行翅膀升级
	bool CanComposeEquip4();   //是否可以进行神兵升级
	bool CanComposeEquip5();   //是否可以进行神兽升级
	bool CanComposeEquip6();    //是否可以进行护身符璀璨镶嵌
	bool CanComposeEquip7();    //是否可以进行护身符神兽宝石镶嵌	
 	bool CanComposeEquip8();    //是否可以进行尚方宝剑升级
	bool CanComposeEquip9();    //是否可以进行紫金龙袍升级	   	
	bool CanComposeEquip10();    //是否可以进行十二星宿升级	 
	bool CanComposeEquip11();    //是否可以进行神斧升级	 
	bool CanComposeEquip12();    //是否可以进行黑曜 升级	 
	bool CanComposeEquip13();    //是否可以进行精金升级	   	
	bool CanComposeEquip14();   //刷新金箍咒强化页面信息
	bool CanComposeEquip15();   //刷新金箍咒字纹页面信息
	bool CanComposeEquip16();   //刷新金箍咒淬炼页面信息
	bool CanComposeEquip17();   //刷新寒冰神炼页面信息
	bool CanComposeEquip18();   //刷新寒冰字纹页面信息
	bool CanComposeEquip19();   //刷新寒冰纹章页面信息

	CRoleItem* GetFocusItem();
	CRoleItem* GetMaterialItem(int index); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
	bool IsEquipItem(enumItemType eType);  // 是否为装备类物品

	void SendEquipAnnexpackMsg(RecastAction eAction);
	void OnTimeUp();	
	void SetBGImage();

	void RefreshPage();


	void RefreshEquipCompose1();    //刷新勋章升级页面信息
	void RefreshEquipCompose2();    //刷新寒冰升级页面信息
	void RefreshEquipCompose3();    //刷新翅膀升级页面信息
	void RefreshEquipCompose4();   //刷新神兵升级页面信息
	void RefreshEquipCompose5();   //刷新神兽升级页面信息
	void RefreshEquipCompose6();    //刷新护身符璀璨镶嵌页面信息
	void RefreshEquipCompose7();    //刷新护身符神兽宝石镶嵌页面信息
 	void RefreshEquipCompose8();    //刷新尚方宝剑升级页面信息
	void RefreshEquipCompose9();    //刷新紫金龙袍升级页面信息   
	void RefreshEquipCompose10();    //刷新十二星宿升级页面信息 
	void RefreshEquipCompose11();    //刷新神斧升级页面信息 
	void RefreshEquipCompose12();    //刷新黑曜升级页面信息 
	void RefreshEquipCompose13();    //刷新精金升级页面信息 
	void RefreshEquipCompose14();   //刷新金箍咒强化页面信息
	void RefreshEquipCompose15();   //刷新金箍咒字纹页面信息
	void RefreshEquipCompose16();   //刷新金箍咒淬炼页面信息
	void RefreshEquipCompose17();   //刷新寒冰神炼页面信息
	void RefreshEquipCompose18();   //刷新寒冰字纹页面信息
	void RefreshEquipCompose19();   //刷新寒冰纹章页面信息
private:
	CGuiStatic*			m_pStaConsumeNum;
	CGuiStatic*			m_pStaSuccessRate;
	CGuiStatic*			m_pStaNeedMoney;
	CGuiStatic*			m_pStaResult;
	CGuiTab*			m_pTabEquipAnnexpack;
//	CGuiProcess*		m_pProcessTimer;
	CGuiButton*			m_pBtnPages[TOTAL_PAGE_NUM];
	CGuiTable*			m_pTableMaterials[TOTAL_MATERIAL_TABLE];
	EQUIPANNEXPACK_PAGE	m_eCurPage;

	CMakeData	m_MakeData; // 打造数据类
	CMyTimer*	m_pTimerProcess;
	std::string m_strTitle;
	CAnimation	m_aniSuccess;
	bool		m_bPutFocusItem;
	UINT		m_unSoulMountNeedMoney;
};
