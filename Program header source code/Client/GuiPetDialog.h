/**
 *\file		GuiPetDialog.h
 *\version	$ID$
 *\author	$$$04公司
 *			$$$04公司@###.com
 *\date		2005-07-06 19:57:00
 *\brief	宠物对话框
 *
 * 宠物、马匹、保镖界面
 *
 */


#ifndef _GUI_PET_DIALOG_H
#define _GUI_PET_DIALOG_H

#include "..\gui\include\guidialog.h"

class CCartoonPet;
/**
* \brief 宠物对话框
* 
* 宠物界面
* 
*/
class CGuiPetDialog :public CGuiDialog
{
private:
	typedef std::map<int,CGuiTable*> EquipTables;
public:
	CGuiPetDialog();
	~CGuiPetDialog();
public:
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	void OnSetCloseLocation();

	void SetCharType(int type);
	CGuiTable* & GetEquipTable(int equip_type,int x = 0);

	CRoleItem* GetEquipItem(int equip_type,int x = 0);

	HRESULT OnRender(float fElapsedTime);

	void OnRemoveCartoonPet( DWORD dwCartoonID );
	void OnUpdateCartoonPet( DWORD dwCartoonID );

	void UpdateContents();
	void ClearEditText();

	static void ConsignAdoptCartoon(const char* pszName);	//指定pszName领养当前页面宠物
private:
	bool OnHorseFollowMaster();
	bool OnHorseRemove();
	bool OnPetRename();
	bool OnSummonRename();

	bool OnCartoonRename();
	bool OnCartoonAddCent();	//充值
	bool OnCartoonPutout();		//放出
	bool OnCartoonPutin();
	bool OnCartoonNext();
	bool OnCartoonPre();
	bool OnCartoonFeed();
	bool OnCartoonStartTraining();
	bool OnCartoonStopTraining();
	bool OnCartoonGetExp();
	bool OnStartAutoRepair();
	bool OnEndAutoRepair();

	bool OnAddZuoqi();//坐骑图鉴

	bool OnHorseTab();
	bool OnPetTab();
	bool OnSummonTab();
	bool OnCartoonTab();
private:
	void UpdateHorseTab();
	void UpdatePetTab();
	void UpdateSummonTab();
	void UpdateCartoonTab();

	bool RenderText(float fElapsedTime,petType type);
	bool RenderText1(float fElapsedTime,petType type);	
	void RenderHorse(float fElapsedTime);

	void RenderTab(float fElapsedTime,int cur);	

	inline void SetPetImage(int id,DWORD dwImage);
	inline void AfterOnUpdateTab(int id,petType type);
	inline void SetCartoonPetImage(int id,CCartoonPet* pCartoon);

	void RenderToolTip(const t_PetData* pData,int index);

	void RenderHorseToolTips(UINT nID);

	void SetCurCartoonPet( CCartoonPet* pCartoon );

	void RenderCartoonPetExp(CCartoonPet* pCartoon);

	CCartoonPet*		m_pCurCartoonPet;
	CHorse*				m_pCurHorse;
	
private:
	struct stToolTipRect
	{
		int		index;
		int		key;
		stRectI rect;
	};
	typedef std::vector<stToolTipRect>	ToolTipRectVec;
	typedef ToolTipRectVec::iterator	ToolTipRectIt;

	ToolTipRectVec						m_toolTipRects;
	CToolTips							m_ToolTips;

	IBitmaps*			m_pBmpExp;		//经验值

	///装备的格子
	//CGuiTable *			m_eTable[4];

	const static stRectI sNameRect;
	const static stRectI sLeveRect;
};




#endif