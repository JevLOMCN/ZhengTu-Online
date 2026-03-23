/**
 * \file      GuiMLEditBox.h
 * \version   $Id$
 * \author    $$$03公司@###.com
 *           $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI多行编辑框
 * 
 *	      GUI多行编辑框
 */

#pragma once
#include "guicontrol.h"
#include "./guiScrollBar.h"

struct stGuiCtrlBorderInfo;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiMLEditBox :
	public CGuiControl
{
	std::string m_strTextA;
public:
	enum STYLE
	{
		AUTONEWLINE = 0x1,
		GUI_ES_MAXCOUNT = 0x2,
		GUI_ES_NOSKIN	= 0x8,
		GUI_ES_READONLY = 0x10,
		GUI_ES_MULTIMEDIA = 0x20,
		GUI_ES_TEXTBOX = 0x40,
		GUI_ES_SCROLLBAR_HAS_SKIN = 0x80,
	};

protected:
	class CUniBuffer{

		//bool   m_bAutoNewline;	// 自动换行
		WCHAR * m_pwszBuffer;
		DWORD * m_pData;
		int    m_nBufferSize;   // Size of the buffer allocated, in characters
		int    m_nTextSize;     // Size of the text in buffer excluding
		//int	   m_iFont;         // Font index of this buffer
		int	   m_nLineWidth;
		int	   m_nLineHeight;
		int	   m_nWidth;

		CGuiMLEditBox * m_pEditBox;
		struct stLine{
			size_t index;
			size_t count;
			int width;
		};

		std::vector<stLine> m_Lines;

		bool Grow( int nNewSize = -1);
		void CalcuWidth();
	public:
		void Init(int nInitialSize = 1);
		bool IsMultimedia();

		bool SetText(const WCHAR* pStr,const DWORD* pData = NULL);
		int GetFontIndex() { return m_pEditBox->GetFont();}

		//void SetFontIndex(int i) { m_iFont = i; }
		void SetWChar(int index,WCHAR ch,DWORD* data = NULL);
		void SetLineHeight(int nLineHeight){ m_nLineHeight = nLineHeight; }
		void SetLineWidth(int nLineWidth) { m_nLineWidth = nLineWidth; }

		// 删除字符
		bool RemoveChar(stPointI ptStart, stPointI ptEnd);

		int CharWidth(stPointI ptCP);

		bool InsertString(stPointI cp,const WCHAR* pStr,const DWORD* pData = NULL,int count = -1);
		bool RemoveChar(stPointI cp);

		bool InsertChar(stPointI cp,WCHAR nChar,DWORD *data = NULL);

		int GetBufferSize() { return m_nBufferSize; }

		WCHAR* GetBuffer(){ return m_pwszBuffer; }
		DWORD* GetData(){ return m_pData;}
		int GetTextSize(){ return m_nTextSize; }

		bool IsLineTail(stPointI ptCP);
		CUniBuffer(CGuiMLEditBox* pEditBox);
		~CUniBuffer();
		void Refresh(int nStartLine = 0);
		int GetLineCount(){ return m_Lines.size();}
		int GetLineWidth(int iLine) { return m_Lines[iLine].width;}
		int GetLineLength(int index){  return m_Lines[index].count; }
		void SetLineWidth();
		int GetWidth(){ return m_nWidth;}
		stPointI CPtoXY(stPointI cp,bool bTrail);
		stPointI XYtoCP(stPointI pt,int *pnTrail);
		void ForwardCP(stPointI & ptCP,bool bNewLineIncChar = false);
		void BackwardCP(stPointI & ptCP);
		int CPtoIndex(stPointI cp) { Assert((size_t)cp.y < m_Lines.size()); return m_Lines[cp.y].index + cp.x;}
		stPointI IndextoCP(int index);
		WCHAR* GetLine(int index){ Assert(index < GetLineCount()); return m_pwszBuffer + m_Lines[index].index;}
		DWORD* GetLineData(int index) { Assert( index < GetLineCount() ); return (m_pData ? m_pData + m_Lines[index].index : NULL);}
		
		stLine& GetLineStruct(int index) { Assert(index < GetLineCount()); return m_Lines[index];}
	};
	static inline void GetFirstLastPoint(stPointI pt1,stPointI pt2,stPointI& First,stPointI & Last)
	{
		if(pt1.y < pt2.y)
		{
			First = pt1;
			Last = pt2;
		}
		else if(pt1.y > pt2.y)
		{
			First = pt2;
			Last = pt1;
		}
		else
		{
			if(pt1.x < pt2.x)
			{
				First = pt1;
				Last = pt2;
			}
			else
			{
				First = pt2;
				Last = pt1;
			}
		}
	}
public:

	CGuiMLEditBox(CGuiDialog* pDialog,DWORD dwStyle = AUTONEWLINE);
	~CGuiMLEditBox(void);

	bool CanHaveInputFocus() { return CanHaveFocus(); }
	bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	static bool AdjustControlRect(RECT* pRect,DWORD dwStyle,int nSBWidth,stGuiCtrlBorderInfo* pBorderInfo);
protected:
	bool     m_bCaretOn;     // Flag to indicate whether caret is currently visible
    bool     m_bInsertMode;  // If true, control is in insert mode. Else, overwrite mode.
	bool	 m_bSBInit;
	bool	 m_bMouseDrag;

	int		 m_nLineHeight;
	int		 m_nMaxShowLineCount;
	int		 m_nBorder;
	int		 m_nSpacing;
	int		 m_nSBWidth;

	int		m_maxCharCount;

	double	 m_dfBlink;
	double	 m_dfLastBlink;

	CUniBuffer		m_Buffer;     // Buffer to hold text

	stPointI		m_ptCaret;       // Caret position, in characters
    stPointI		m_ptSelStart;    // Starting position of the selection. The caret marks the end.
    stPointI		m_ptFirstVisible;// First visible character in the edit control
	stRectI			m_rcRender[9];
	stRectI			m_rcText;

    //DWORD m_TextColor;    // Text color
    DWORD m_SelTextColor; // Selected text color
    DWORD m_SelBkColor;   // Selected background color
    DWORD m_CaretColor;   // Caret color

	CGuiScrollBar m_HScrollBar;	// 水平滚动条
	CGuiScrollBar m_VScrollBar; // 垂直滚动条

	void OnFocusIn();

	void Render( float fElapsedTime );

	HRESULT OnInit() ;

	// 设置光标位置
	void PlaceCaret(stPointI ptCP);
	
	void ResetCaretBlink();

	void SetText(const WCHAR* strText,DWORD* pData = NULL);
	const WCHAR* GetTextW(){ return m_Buffer.GetBuffer(); }
public:

	// 删除选择的文本
	void DeleteSelectionText(void);
	void SetBorderWidth( int nBorder ) { m_nBorder = nBorder; UpdateRects(); }  // Border of the window
    void SetSpacing( int nSpacing ) { m_nSpacing = nSpacing; UpdateRects(); }

	bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	void SetText(const char* strText,DWORD* pData = NULL);
	const char* GetText();
	void ScrollTo(POINT pt);
	void ScrollToLine(int line);
protected:
	void UpdateRects(void);
public:
	void CopyToClipboard();
	void PasteFromClipboard(void);
	void  GetTextByFM(char* pStr, const int& length = -1);

	DWORD GetCaretColor(){ return m_CaretColor;}
	void SetCaretColor(DWORD dw){ m_CaretColor = dw;}

	int GetLineCount();
	int GetLineLength(int line);
	void SetSelText(const char* pszText,bool bScroll = false,DWORD* pData = NULL);
	void SetCaretPos(stPointI ptCP);
	void SetLineHeight(int nLineHeight);
	int GetLineHeight(){ return m_nLineHeight;}
	void SelectAll();

	void OnTextChange();
	int GetMaxCharCount(){ return m_maxCharCount; }
	void SetMaxCharCount(int count) { m_maxCharCount = count; }

protected:
	// 刷新文本行
	void Refresh(int nStartLine);

};

