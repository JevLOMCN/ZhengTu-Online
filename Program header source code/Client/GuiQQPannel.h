#pragma once

#include "..\gui\include\guidialog.h"
#include "../xml_parse/XmlParseEx.h"
#include "Object.h"

typedef std::vector<enumItemType>		tListItemType;
typedef tListItemType::iterator			tListItemTypeIt;

enum enumPannelType
{
	ePannel_None,			//none
	ePannel_Browse,			//ä¯ÀÀ
	ePannel_Roup,			//¾ºÅÄ
};

struct stPannelItem
{
	CGuiDialog*		m_pDialog;
	CGuiButton*		m_pButton;
	tListItemType	m_listType;
	DWORD			m_type;

	stPannelItem()
	{
		m_pDialog = NULL;
	}
	int	 GetID() { return m_pButton->GetID(); }
	void SetVisible(bool bShow) { m_pButton->SetVisible(bShow); }
	void SetLocation(int x,int y) { m_pButton->SetLocation(x,y); }
	void SetBitmap(stResourceLocation& rl,int frameNum){m_pButton->SetBitmap(rl,frameNum);}	
	void SetText(const char* szText) { m_pButton->SetText(szText); }
	void SetTextColor(DWORD dwColor) { m_pButton->SetTextColor(dwColor); }
	void Parse(TiXmlElement* Item);
	bool bIsItemTypeIn(enumItemType type);
	bool OnClick(enumPannelType type = ePannel_None);
};

typedef std::vector<stPannelItem*>		tListPannelItem;
typedef tListPannelItem::iterator		tListPannelItemIt;
class CPannelItem
{
public:
	CPannelItem();
	~CPannelItem();
	CPannelItem(CGuiDialog* pDialog,CGuiButton* pMasterBtn = NULL);
	stPannelItem* AddPannelItem(int id,const char* pszText);
	void SetButtonLocation(int id,const POINT& pt);
	void SetButtonLocation(int id,int x,int y);
	void SetAllButtonVisible(bool bVisible = true);
	void SetButtonBitmap(stResourceLocation& rl,int frameNum,int id = -1);
	void SetLocation(int x,int y);
	void Release();
	int	 GetID();
	void SetBitmap(stResourceLocation& rl,int frameNum);
	void SetChecked( bool bChecked );
	bool GetChecked();
	POINT GetLocation();
	void AddToTab(CGuiTab* &pTab,int index);

	void Parse(TiXmlElement* pannel);
	tListPannelItemIt GetButtonById(int id);
	void SetVisible(bool bShow);
	
	CGuiButton*			m_pMasterBtn;
	tListPannelItem		m_list;
	CGuiDialog*			m_pDialog;
};

typedef std::vector<CPannelItem*>	tListPannel;
typedef tListPannel::iterator		tListPannelIt;

class CGuiQQPannel
{
public:
	CGuiQQPannel();
	CGuiQQPannel(CGuiDialog* pDialog);
	~CGuiQQPannel();
	CPannelItem* AddPannel(int id,const char* szText);
	void AddChildItem(int pannelid,int id,const char* szText);
	void SetPannelBitmap(stResourceLocation& rl,int frameNum,int id = -1);
	void Release();
	void SetDialog(CGuiDialog* pDialog) { m_pDialog = pDialog;}
	void SetRect(const RECT& rc) { m_BorderRect = rc; UpdateBorderRects(); }
	void SetBorderBitmap(const stResourceLocation& rl);
	void UpdateBorderRects();
	void UpdatePannelLocation();
	RECT GetRect() { return m_BorderRect; }

	void ShowPannelItem(int index,bool bShow = true);
	void SetPannelItemChecked(int id);
	void SetCurrentPannel(int index);
	void AddToTab(CGuiTab* &pTab,int index);
	tListPannelIt GetCheckedIt();
	tListPannelIt GetPannelById(int id);
	CPannelItem* GetPannelByIndex(int index);
	void OnRender(float fElapsedTime);
	bool OnButtonClick(int id);
	void SetVisible(bool bShow);

	bool Load(const char* pszFileName);
	void SetType( enumPannelType type ){ m_type = type; }
private:
	enumPannelType	m_type;
	tListPannel		m_listPannel;
	CGuiDialog*		m_pDialog;

	stRectI m_rcRender[9];	//»æÖÆµÄÐ¡¾ØÐÎ×é
	stRectI m_BorderRect;		//±ß¿ò
	stRectI m_rcBitmapClip[9];	//±ß¿òÍ¼Æ¬²Ã¼ô×é
	IBitmaps* m_pBitmapBorder;	//±ß¿òÍ¼Æ¬
};
