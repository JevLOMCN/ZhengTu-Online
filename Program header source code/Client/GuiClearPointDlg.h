
#ifndef __GUI_CLEARPOINT_DLG__
#define __GUI_CLEARPOINT_DLG__



//--------------------------------------------------------------------
// 所有的洗点道具

#define ID_CLEARPOINT_MARROW_BIJOU     760  // 洗髓宝珠 (属性点)40级以上可以使用的（包括40级）
#define ID_CLEARPOINT_MARROW_BIJOU_NEW	756
#define ID_CLEARPOINT_MARROW_BOOK      752  // 洗髓经 40级以下可以使用的


#define ID_CLEARPOINT_TENDON_BIJOU     761  //易筋宝珠 (技能点)  40级以上可以使用的（包括40级）
#define ID_CLEARPOINT_TENDON_BIJOU_NEW		757
#define ID_CLEARPOINT_TENDON_BOOK      753  //易筋经 洗技能点    40级以下可以使用的

#define ID_CLEARPOINT_PROPERTY_JADE    755  // 精气玉 给玩家选择洗的属性点, 每次固定洗属性点5点
//--------------------------------------------------------------------
/**
 * \brief 洗属性点
 * 
 * 
 */
class CGuiClearPointDlg :
	public CGuiDialog
{	
public:
	CGuiClearPointDlg();
	~CGuiClearPointDlg();

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	void SetObjectThisID(DWORD dwThisID){ m_dwObjectThisID = dwThisID;};
private:
	DWORD m_dwObjectThisID;
	void InternalSendMsgToServer(BYTE byProperty);
};

extern bool OnUseClearPointItem(CRoleItem * pRoleItem);
extern void MSG_ClearPointOrSkill(DWORD dwObjectBaseID);
extern bool ClearPoint_CannotTrade(CRoleItem * pRoleItem);
extern bool ClearPoint_CannotMail(CRoleItem * pRoleItem);
extern bool ClearPoint_CannotTradeAndMail(DWORD dwObjectID);

#endif