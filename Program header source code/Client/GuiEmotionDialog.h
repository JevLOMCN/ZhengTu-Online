#pragma once
#include "..\gui\include\guidialog.h"
#include "..\gui\include\GuiScrollBar.h"
#include <string>

#define    c_IconGL		    "data\\icons.gl"
#define	   EMOTION_GROUP	    7
#define    EMOTION_ICON_ID_BASE     1000
#define    EMOTION_BTN_ID_BASE      1000
#define    ACCESS_NUM               5

enum ICON_ACCESS_COME_FROM
{
	FROM_NONE,
	FROM_MAIN,
	FROM_FLOAT,
	FROM_EMOTION,
	FROM_KEYBOARD,
};

struct stDesc
{
	DWORD animation;
	std::string strMusic; // music
	std::string strDesc1; // icon string
	std::string strDesc2; // self to other 
	std::string strDesc3; // other to me 
};
typedef std::map<DWORD, stDesc> tMapEmotionDesc;
typedef std::map<DWORD, stDesc>::iterator _Iterator;
typedef std::map<DWORD, stDesc>::const_iterator _CIterator;
class CEmotionInfo
{
public:
     CEmotionInfo();
     ~CEmotionInfo();

   
     std::string  GetIconDesc1(DWORD dwIconID);
     std::string  GetIconDesc2(DWORD dwIconID);
     std::string  GetIconDesc3(DWORD dwIconID);
     std::string  GetCurIconDesc1();
     std::string  GetEmotionMusic(DWORD dwIconID);

     void SetCurIconInfo(DWORD IconID,ICON_ACCESS_COME_FROM _from)
     {
	     m_dwCurIconID = IconID;
	     m_IconComeFrom = _from;

     }

     DWORD GetCurIconID()
     {
	     return m_dwCurIconID;
     }

     bool GetCurAnimation(int & dwOutAnimation)
     {
	     return GetAnimation(dwOutAnimation,m_dwCurIconID);

     }

     bool GetAnimation(int & dwOutAimation,DWORD dwInIcon)
     {
	     if(m_MapEmotionDesc.end() == m_MapEmotionDesc.find(dwInIcon))
		     return false;

	     stDesc & desc = m_MapEmotionDesc[dwInIcon];
	     dwOutAimation = desc.animation;
	     return true;
     }

     ICON_ACCESS_COME_FROM GetComeFrom()
     {
	     return m_IconComeFrom;
     }

     DWORD   GetActiveEmotionIcon();
     void    ClearActiveEmotionIcon();

     void  GetCurIconRL();
     void  OnUseEmotionOnSome(CCharacter * pCharaceter);
     void  OnReceivedEmotion(stChannelChatUserCmd * pCmd);
     void  LoadFromFile();

     void  SetAccessKey(int index,DWORD dwKey);
     DWORD GetAccessKey(int index);
     void  RefreshAccessKey();

     void  OnPreUseEmotion(DWORD dwKey);

     bool GetResourceLocation(DWORD iIconID,stResourceLocation & rl);
     bool GetActiveResourceLocation(stResourceLocation & rl);

     CToolTips*  GetToolTips(DWORD dwIconID);
    
     const tMapEmotionDesc & GetEmotionMap(){return m_MapEmotionDesc;};
private:

     tMapEmotionDesc  m_MapEmotionDesc;

    
     DWORD m_dwCurIconID;
     DWORD m_AccessKey[ACCESS_NUM]; // IconID

     ICON_ACCESS_COME_FROM m_IconComeFrom;
     void  ResetAccessKey();
};


extern CEmotionInfo * GetEmotionInfo();


class CGuiEmotionDialog : public CGuiDialog
{
public:
	enum SetAccess_Icon_From
	{
		FROM_NONE = 0,
		FROM_ICON = 1,
		FROM_ACCESS = 2, //从已有快捷键上拖动
	};
	CGuiEmotionDialog(void);
	~CGuiEmotionDialog(void);

	struct stAccessKey{
		CGuiButton * pButton;
		DWORD   dwKey; // IconID 
		stRectI rcClick;
		stAccessKey()
		{
			pButton = NULL;
		}
	};
       
	stAccessKey m_AccessKeyBtn[ACCESS_NUM];
 

	struct stEmotionIcon{
		CGuiButton* pButton;
		stEmotionIcon()
		{
			pButton = NULL;
		}
	};
       
	void	OnClose();

	typedef std::map<DWORD, stEmotionIcon> tMapEmotionIcon;
	typedef std::vector<DWORD>             tVecEmotionIcon;
	tVecEmotionIcon m_vecEmotionIcon;   // 保存的就是map中的key,也就是真实图片frame + EMOTION_ICON_ID_BASE
	tMapEmotionIcon m_mapEmotionIcon;   // key =真实图片frame + EMOTION_ICON_ID_BASE,生成的按钮是真实图片frame+ EMOTION_BTN_ID_BASE

	void	OnMouseWheel();
	void	OnSetCloseLocation();
	
	void	OnResetScreenSize();
	bool	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	void    OnCreate();
	bool    OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void    InitCustomElements();
	HRESULT OnRender(float fElapsedTime);

	
	void    OnUseEmotion(CCharacter * pChar);

private:
	void    InitIcon();
	void    UpdateIcon();
	void    InitScrollBar();
	void	UpdateSrollRects();

	void DeleteAccessKey(DWORD iIconID);
	void SetAccessKey(DWORD iIconID,DWORD nBtnID);
	void __SetAccessKey(DWORD iIconID,DWORD nBtnID);
	void CreateAccessKey();
	void ReplaceAccessKey(DWORD iIconID,DWORD iOldIconID,int nBtnID);
	void SetCursorImage(const stResourceLocation & rl);
	void RestoreCursorImage();

	CGuiScrollBar*	      m_pScrollBar;
	SetAccess_Icon_From   m_IconFrom;
	DWORD	              m_dwCurIconID;
        DWORD                 m_dwCurAccessKeyBtn;
};

