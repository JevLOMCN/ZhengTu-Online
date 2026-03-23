 #pragma once 

#include "..\gui\include\guidialog.h"
#include "../gui/include/ToolTips.h"
#define   GETSHOWCOLOR(total,base)  total==base?0xffffffff:(total>base?0xff00ff00:0xffff0000)
#define   GETSHOWCOLOR2(total,base)  total==base?0xffffffff:(total>base?0xff646464:0xffff0000)
#define   GETSHOWCOLOR3(total,base)  total==base?0xffffffff:(total>base?0xffff7100:0xffff0000)
extern void *pComboItem;
/**
* \brief 图鉴对话框
* 
*图鉴界面
* 
*/
 
class CDlgEquiphandbook : public CGuiDialog

{	

    struct stTitle
	{
		int      key;
		stRectI  rect;
		byte     index;
	};
   public:
	CDlgEquiphandbook(bool bMainUser = true);
	~CDlgEquiphandbook();

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	HRESULT OnRender(float fElapsedTime);
	
    void RenderBook(float fElapsedTime);
	
    void ShowUserInfoText1();
    void SetCharType(int type,void* pData = NULL);
	void TipsRender();
	void SetUserData(void* pData);


    void RefreshActiveItem();
	void InsertActiveItem(CRoleItem* pRoleItem);
	bool IsActiveItem(CRoleItem* pItem);
	char m_strSetMakerName[MAX_NAMESIZE];
	char* GetSetMakerName();

	void SetEquipByName(CRoleItem* pItem, bool bMainCharacter = true);
    void ShowEquipByName();
	void RefreshButtonState();
	


	CGuiTable* & GetEquipTable(int equip_type,int x = 0);
	//{
	//	int value = (equip_type << 16 | x);
	//	return m_tables[value];

	//}
	CRoleItem* GetEquipItem(int equip_type,int x = 0);

	

	struct xtUserData
	{
		::t_MapUserData*     eMapUserData;
		t_MainUserData*    eMainUserData;
		::t_MapUserData      rMapUserData;
		t_MainUserData     rMainUserData;	
	};

    std::map<int,CGuiTable*> m_tables;
	CGuiTab*  m_pTab;
	bool                   m_bMainUser;
	CToolTips          m_ToolTips;
	
	std::vector<stTitle>     m_titleList;
	std::vector<stTitle>     m_equipList;

    xtUserData             x_UserData;



	bool                  m_bEquipe ;
	bool                  m_bClothes ;
	std::vector<CRoleItem*> m_listActiveItem;

	std::lookaside_allocator<CRoleItem> m_allocRoleItem;
	std::vector<CRoleItem*> m_listItem;

	
	//t_Object* GetObject()
	//{
	//	return &m_object;
	//}

	//t_Object GetTObject()
	//{
	//	return m_object;
	//}

	//ObjectBase_t* GetObjectBase()
	//{
	//	return m_pObjectBase;
	//}





};
