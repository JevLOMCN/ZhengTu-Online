#pragma once
#include "guicontrol.h"
#include "./guiScrollBar.h"
#include "../../xml_parse/tinyxml.h"


extern float getLastTime(float time);
extern float getLinkShowTime();
extern int   getAlpha(float time);
extern int   g_GMFilter;
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CGuiMLTextBox :
	public CGuiControl
{
	class NodeBase
	{
	public:
		bool bSelected;
	public:
		NodeBase() : bSelected(false) {};
		virtual ~NodeBase(){};
		virtual bool DrawSelBkColor(POINT ptOffset,const RECT * prect,CGuiMLTextBox* pTextBox) {return false;}
		virtual bool Draw(POINT ptOffset,const RECT * prect,CGuiMLTextBox* pTextBox,DWORD dwBlend) = 0;
		virtual const char* GetTextContent() { return NULL; }
		virtual POINT GetTailOffset() = 0;
		virtual bool HandleMouse( UINT uMsg, POINT ptOffset, WPARAM wParam, CGuiMLTextBox* pTextBox ) = 0;
		virtual bool IsShow(POINT ptOffset,const RECT * prect) = 0;
		virtual bool IsInRect(POINT ptOffset) = 0;
		virtual bool OnClick(CGuiMLTextBox* pTextBox){ return false;}
		virtual void SetSelected(bool bSel) { bSelected = bSel; }
	};

	class NodeUnknown : public NodeBase{
	public:
		~NodeUnknown(){};
		stRectI rect;
		bool IsShow(POINT ptOffset,const RECT * prect);
		bool IsInRect(POINT ptOffset);
		POINT GetTailOffset() { return stPointI(rect.right,rect.bottom);}
		virtual bool DrawSelBkColor(POINT ptOffset,const RECT * prect,CGuiMLTextBox* pTextBox);
		bool HandleMouse( UINT uMsg, POINT ptOffset, WPARAM wParam, CGuiMLTextBox* pTextBox )
		{
			return false;
		}
	};

	class NodeText : public NodeUnknown{
	public:
		DWORD color;
		std::string text;
		std::vector<DWORD> data;

		NodeText():color(-1){}
		bool Draw(POINT ptOffset,const RECT * prect,CGuiMLTextBox* pTextBox,DWORD dwBlend);
		virtual const char* GetTextContent() { return text.c_str(); }
	};

	class NodeSubLink : public NodeText{
		DWORD  m_dwFormat;
	public:
		NodeSubLink() { m_dwFormat = DT_UNDERLINE | DT_SINGLELINE; color = 0xffffff00; }
		bool Draw(POINT ptOffset,const RECT * prect,DWORD color,CGuiMLTextBox* pTextBox,DWORD dwBlend);
		bool Draw(POINT ptOffset,const RECT * prect,CGuiMLTextBox* pTextBox,DWORD dwBlend)
		{
			return Draw(ptOffset,prect,color,pTextBox,dwBlend);
		}
		void SetFormat(DWORD format){
			m_dwFormat = format;
		}
	};

	class NodeHyperlinkBase : public NodeBase{
	public:
		std::vector<NodeSubLink * > subNodes;
		DWORD colorNormal;
		DWORD colorOver;
		DWORD colorDown;
		
		NodeHyperlinkBase();

		~NodeHyperlinkBase()
		{
			for(size_t i=0;i<subNodes.size();++i)
				delete subNodes[i];
			subNodes.clear();
		}

		virtual bool DrawSelBkColor(POINT ptOffset,const RECT * prect,CGuiMLTextBox* pTextBox);
		bool Draw( POINT ptOffset ,const RECT * prect,CGuiMLTextBox* pTextBox,DWORD dwBlend);
		bool HandleMouse( UINT uMsg, POINT ptOffset, WPARAM wParam, CGuiMLTextBox* pTextBox );
		POINT GetTailOffset()
		{
			if(!subNodes.empty())
			{
				return subNodes.back()->GetTailOffset();
			}
			return stPointI(0,0);
		}
		bool IsShow(POINT ptOffset,const RECT * prect);
		bool IsInRect(POINT ptOffset);
		void SetFormat(DWORD format){
			for(size_t i=0;i<subNodes.size();++i)
				subNodes[i]->SetFormat(format);
		}
		virtual bool OnRightClick(CGuiMLTextBox* pTextBox){ return false;}
		virtual void SetSelected(bool bSel)
		{
			NodeBase::SetSelected( bSel );
			for( size_t i=0;i<subNodes.size();++i )
			{
				if ( subNodes[i] )
					subNodes[i]->SetSelected( bSel );
			}
		}
		virtual const char* GetTextContent();
	};

	class NodeHyperlink : public NodeHyperlinkBase{
	public:
		std::string addr;
		bool OnClick( CGuiMLTextBox* pTextBox );
		~NodeHyperlink()
		{
		};
	};

	class NodeShell : public NodeHyperlinkBase{
	public:
		DWORD objid;
		bool OnClick( CGuiMLTextBox* pTextBox );
	};
	
	class NodeChatUser : public NodeHyperlinkBase{
	public:
		char strName[MAX_NAMESIZE];
		bool OnClick( CGuiMLTextBox* pTextBox );
		bool OnRightClick(CGuiMLTextBox* pTextBox );
		const char* GetTextContent() { return strName; }
	};

	class NodeImage : public NodeUnknown{
	public:
		IBitmaps * pImage;
		int group;
		bool check;
		bool Draw(POINT ptOffset,const RECT * prect,CGuiMLTextBox* pTextBox,DWORD dwBlend);
		bool HandleMouse( UINT uMsg, POINT ptOffset, WPARAM wParam, CGuiMLTextBox* pTextBox );
		bool OnClick(CGuiMLTextBox* pTextBox);
	};

	class NodeObject : public NodeImage{
	public:
		DWORD textColor;
		std::string strText;
		std::string scriptFunction;
		CToolTips toolTips;
		bool Draw(POINT ptOffset,const RECT * prect,CGuiMLTextBox* pTextBox,DWORD dwBlend);
		bool HandleMouse( UINT uMsg, POINT ptOffset, WPARAM wParam, CGuiMLTextBox* pTextBox );
		bool OnClick(CGuiMLTextBox* pTextBox);
		const char* GetTextContent() { return strText.c_str(); }
	};

	class NodeButton : public NodeImage{
	public:
		DWORD textColor;
		std::string strText;
		DWORD id;
		int sound;
		bool bDown;
		bool Draw(POINT ptOffset,const RECT * prect,CGuiMLTextBox* pTextBox,DWORD dwBlend);
		bool HandleMouse( UINT uMsg, POINT ptOffset, WPARAM wParam, CGuiMLTextBox* pTextBox );
		bool OnClick(CGuiMLTextBox* pTextBox);
		const char* GetTextContent() { return strText.c_str(); }
	};

	class stParagraph{
	public:
		std::vector<NodeBase*> nodes;
		void*		  data;
		float             time;
		bool		  bIsGmMsg;

		stParagraph(): bIsGmMsg(false)
		{
		}
		~stParagraph(){
			for(size_t i=0;i<nodes.size();++i)
				delete nodes[i];
			nodes.clear();
		}
		
		POINT GetTailOffset()
		{
			if(!nodes.empty())
				return nodes.back()->GetTailOffset();
			return stPointI(0,0);
		}
		
		void Draw( POINT ptOffset ,const RECT * prect,CGuiMLTextBox* pTextBox)
		{
			DWORD dwBlend = 0x0;
			if (pTextBox->GetStyle() & GUI_TS_AUTOFADEOUT)
			{
				time += Engine_GetDrawElapsedTime();
				float  fTime = getLastTime(time);
				int   alpha = 0;
				if (fTime>0.0f)
					alpha = getAlpha(time);
				dwBlend = COLOR_ARGB(alpha,255,255,255);
			}

			for(size_t i=0;i<nodes.size();++i)
			{
				if ( nodes[i]->bSelected )
					nodes[i]->DrawSelBkColor(ptOffset,prect,pTextBox);
				if ( nodes[i]->Draw(ptOffset,prect,pTextBox,dwBlend) )
					time = min(time,getLinkShowTime());
			} 
		}

		bool HandleMouse( UINT uMsg, POINT ptOffset, WPARAM wParam, CGuiMLTextBox* pTextBox )
		{
			for(size_t i=0;i<nodes.size();++i)
			{
				if(nodes[i]->HandleMouse(uMsg,ptOffset,wParam,pTextBox))
					return true;
			}
			return false;
		}

		bool IsShow(POINT ptOffset,const RECT * prect)
		{
			for(size_t i=0;i<nodes.size();++i)
			{
				if(nodes[i]->IsShow(ptOffset,prect))
					return true;
			}
			return false;
		}

		void RefreshSelNode(POINT ptOffset, RECT* prect)
		{
			for(size_t i=0;i<nodes.size();++i )
				nodes[i]->SetSelected( nodes[i]->IsShow(ptOffset, prect) );
		}

		bool IsInRect(POINT ptOffset)
		{
			for(size_t i=0;i<nodes.size();++i)
			{
				if( nodes[i]->IsInRect(ptOffset) )
					return true;
			}
			return false;
		}
	};

	std::vector<stParagraph* >	m_aParagraph;
	std::vector<NodeImage* >	m_aImage;

	stRectI m_rcText;
	stRectI m_rcRender[9];
	int		m_nLineHeight;
	int		m_nSBWidth;
	//int		m_nBorder;
	int		m_nSpacing;
	public:
	CGuiScrollBar m_VScrollBar; // ´¹Ö±¹ö¶¯Ìõ
	int    m_nSelect;
	TiXmlDocument m_xmlDoc;
	
	std::string m_strCurDir;
	std::string m_strNewDoc;

	bool        m_bFilter;

	//Following member is for selection
	bool		m_bMouseDrag;
	stPointI	m_ptStartSel;
	stPointI	m_ptEndSel;
	DWORD		m_SelBkColor;
	//End selection

	bool AppendImageNode(NodeImage * pNode,const char* pszText,const stResourceLocation * rl,stPointI & ptOffset,int iLineHeight,bool bSingleLine = false);
	void UncheckImageNode(int group);
	TiXmlElement& GetXmlBody();
public:

	enum STYLE
	{
		GUI_TS_NOSKIN	= 0x8,
		GUI_TS_MULTIMEDIA = 0x20,
		GUI_TS_LEFTSCROLL = 0x40,
		GUI_TS_AUTOFADEOUT = 0x80,
		GUI_TS_SCROLLBAR_HAS_SKIN = 0x100,
		GUI_TS_SELECT_ENABLE	= 0x200,
	};

	bool LoadDoc(const char* fileName);
public:

	template < class _TNode>
		void AppendText(const char* text,DWORD color,stPointI & ptOffset,std::vector<_TNode * > & aNode,int curLineHeight);
	POINT GetTailOffset();
	void RefreshText();
	void ClearParagraph();
	void ParseXml();
	void Render( float fElapsedTime );
	HRESULT OnInit();
	bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	void UpdateNodeLocation(std::vector<NodeUnknown*> & aNodes,int iLineHeight);
	bool CanHaveFocus();
public:
	CGuiMLTextBox(CGuiDialog* pDialog,DWORD dwStyle);
	~CGuiMLTextBox(void);

	
	void ScrollToTail();
	bool ParseAParagraph(TiXmlElement* page,void* pData);
	void _AppendParagraph(TiXmlElement* page,void* pData);
	void AppendParagraph(TiXmlElement* page,void* pData);
	void AddHeadParagraph( TiXmlElement* page,void* pData );
	void AppendParagraph(const char* text,DWORD color = -1,void* pData = NULL);
	// add by fancui
	void AppendParagraphButton(const char* text,DWORD color,DWORD objid,const char* icon,int soundindex,void* pData=NULL);
		//end of fancui
	void UpdateRects(void);
	int	 GetTextHeight() { return GetTailOffset().y ;}
	void SetText(const char* text);
	void SetXMLDoc(TiXmlDocument & doc);
	void Clear();
	void SetLineHeight(int nLineHeight);
	//void SetBorderWidth( int nBorder ) { m_nBorder = nBorder; UpdateRects(); }  // Border of the window
	void SetSpacing( int nSpacing ) { m_nSpacing = nSpacing; UpdateRects(); }
	int GetParagraphCount(){ return m_aParagraph.size();}
	void RemoveFirstParagraph();
	void Reset();
	int GetCurrItem(){ return m_nSelect;}
	void SetFilter(bool bFilter)
	{
		m_bFilter = bFilter;
	}

	void SetSelBkColor( DWORD clr ) { m_SelBkColor = clr; }
	DWORD GetSelBkColor() { return m_SelBkColor; }
	void CopyToClipboard();
};

