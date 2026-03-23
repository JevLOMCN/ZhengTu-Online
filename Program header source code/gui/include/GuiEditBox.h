/**
 * \file      GuiEditBox.h
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUIµ¥ÐÐ±à¼­¿ò
 * 
 *	      GUIµ¥ÐÐ±à¼­¿ò
 */

#pragma once
#include "guicontrol.h"
//#include <Usp10.h>

//-----------------------------------------------------------------------------
// EditBox control
//-----------------------------------------------------------------------------
#define UNISCRIBE_DLLNAME "\\usp10.dll"
#define ITEM_SPECIAL_CHAR	0xfefe

#define GETPROCADDRESS( Module, APIName, Temp ) \
    Temp = GetProcAddress( Module, #APIName ); \
    if( Temp ) \
        *(FARPROC*)&_##APIName = Temp

#define PLACEHOLDERPROC( APIName ) \
    _##APIName = Dummy_##APIName


#ifndef MAX_NAMESIZE
#define MAX_NAMESIZE	32
#endif

#ifndef MAX_CHAT_HISTORY_NUM
#define MAX_CHAT_HISTORY_NUM	100
#endif

#ifndef MAX_CHATINFO
#define MAX_CHATINFO	256
#endif

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CGuiEditBox :
	public CGuiControl
{
	std::string m_strTextA;

	//media about
	struct MediaNameData
	{
		char	mName[MAX_NAMESIZE];
		DWORD	mNumber;		//for life about using

		MediaNameData( const char* pName )
		{
			strcpy( mName, pName );
		}
	};

	struct stChatInfo
	{
		char szChatStr[MAX_CHATINFO];
	};

	//Chat history about
	bool	m_bSaveHistory;
	std::list<stChatInfo>	m_strChatHistory;
    int		m_curChatHistory;
	int		m_maxChatHistoryNum;

public:
	enum USAGE
	{
		USAGE_CHAT,
		USAGE_NAME,
	};
private:
	USAGE m_uage;

	void SetText( LPCWSTR wszText, bool bSelected = false ,DWORD* pData = NULL);
	LPCWSTR GetTextW() { return m_Buffer.GetBuffer(); }
public:
	void SetUsage(USAGE use)
	{
		m_uage = use;
	}
	void SetSaveHistory( bool bNeedSave = false ) { m_bSaveHistory = bNeedSave; }
	bool IsNeedSaveHistoryInfo() { return m_bSaveHistory; }
	void ResetCurChatHistory() { m_curChatHistory = m_strChatHistory.size() - 1; }

	char* GetCurChatHistory();
	void InsertCurChatHistory();
	int GetChatHistoryNum();
	void ChangeCurChatHistory( bool bDown = true );

	int SetMaxChatHistory( int maxNum );
	int GetMaxChatHistoryNum() { return m_maxChatHistoryNum; }

	void SaveCurContent();
public:
	typedef std::map<DWORD, MediaNameData> mapMediaNameData;
	typedef std::pair<DWORD, MediaNameData>	pairMediaNameData;

	enum STYLE
	{
		GUI_ES_PASSWORD = 0x1,
		GUI_ES_NUMBER	= 0x2,
		GUI_ES_NOCHINESE = 0x4,
		GUI_ES_NOSKIN	= 0x8,
		GUI_ES_READONLY = 0x10,
		GUI_ES_MULTIMEDIA = 0x20,
		GUI_ES_TEXTBOX	= 0x40,
		GUI_ES_NOFILTER = 0x80, // Ã»ÓÐ¹ýÂË
		GUI_ES_MBPASSWORD = 0x90, // ºº×ÖÓ¢ÎÄ»ìºÏÃÜÂë
	};

	enum ACTIVE_MEDIA
	{
		ACTIVE_MEDIA_NONE = 0x0,
		ACTIVE_MEDIA_ITEM = 0x1,		
	};



    CGuiEditBox( CGuiDialog *pDialog = NULL );
    virtual ~CGuiEditBox();

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual void UpdateRects();
    virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual void RenderFrom( int& startPos, float fElapsedTime, int& mediaWidthBeforeCaret, int& mediaWidthBeforeStart );
    virtual void Render( float fElapsedTime );
    virtual void    OnFocusIn();
	bool CanHaveInputFocus() { return CanHaveFocus(); }

	void SetText( LPCTSTR wszText, bool bSelected = false ,DWORD* pData = NULL);
	const char* GetText();
    int GetTextLength() { return m_Buffer.GetTextSize(); }  // Returns text length in chars excluding NULL.
    HRESULT GetTextCopy( LPWSTR strDest, UINT bufferCount );
    void ClearText();
    virtual void SetTextColor( DWORD Color ) { m_TextColor = Color; }  // Text color
    void SetSelectedTextColor( DWORD Color ) { m_SelTextColor = Color; }  // Selected text color
    void SetSelectedBackColor( DWORD Color ) { m_SelBkColor = Color; }  // Selected background color
    void SetCaretColor( DWORD Color ) { m_CaretColor = Color; }  // Caret color
	DWORD GetCaretColor(){ return m_CaretColor; }
    void ParseFloatArray( float *pNumbers, int nCount );
    void SetTextFloatArray( const float *pNumbers, int nCount );
	DWORD GetActiveMediaType() const { return m_activeMediaType; }
	int GetMediaNameCount( int pos );
	void UpdateMediaListBeforeSend();
	bool AddOneMedia(const DWORD& id, const char* pName, const int& pos = -1);
	mapMediaNameData GetMediaList() { return m_mediaList; }
	void ClearMediaList();
	int GetFontIndex() { return m_Buffer.GetFontIndex(); }
	const WCHAR* GetWBuffer() { return m_Buffer.GetBuffer(); }
	bool CanInput( const int& posInsert );
	int  GetCurrPos() { return m_nCaret ;}

public:
    //-----------------------------------------------------------------------------
    // CUniBuffer class for the edit control
    //-----------------------------------------------------------------------------
    class CUniBuffer
    {

        // static member

		/*
        // Empty implementation of the Uniscribe API
        static HRESULT WINAPI Dummy_ScriptApplyDigitSubstitution( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* ) { return E_NOTIMPL; }
        static HRESULT WINAPI Dummy_ScriptStringAnalyse( HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* ) { return E_NOTIMPL; }
        static HRESULT WINAPI Dummy_ScriptStringCPtoX( SCRIPT_STRING_ANALYSIS, int, BOOL, int* ) { return E_NOTIMPL; }
        static HRESULT WINAPI Dummy_ScriptStringXtoCP( SCRIPT_STRING_ANALYSIS, int, int*, int* ) { return E_NOTIMPL; }
        static HRESULT WINAPI Dummy_ScriptStringFree( SCRIPT_STRING_ANALYSIS* ) { return E_NOTIMPL; }
        static const SCRIPT_LOGATTR* WINAPI Dummy_ScriptString_pLogAttr( SCRIPT_STRING_ANALYSIS ) { return NULL; }
        static const int* WINAPI Dummy_ScriptString_pcOutChars( SCRIPT_STRING_ANALYSIS ) { return NULL; }

        // Function pointers
        static HRESULT (WINAPI *_ScriptApplyDigitSubstitution)( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* );
        static HRESULT (WINAPI *_ScriptStringAnalyse)( HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* );
        static HRESULT (WINAPI *_ScriptStringCPtoX)( SCRIPT_STRING_ANALYSIS, int, BOOL, int* );
        static HRESULT (WINAPI *_ScriptStringXtoCP)( SCRIPT_STRING_ANALYSIS, int, int*, int* );
        static HRESULT (WINAPI *_ScriptStringFree)( SCRIPT_STRING_ANALYSIS* );
        static const SCRIPT_LOGATTR* (WINAPI *_ScriptString_pLogAttr)( SCRIPT_STRING_ANALYSIS );
        static const int* (WINAPI *_ScriptString_pcOutChars)( SCRIPT_STRING_ANALYSIS );
		
        static void InitializeUniscribe();
        static void UninitializeUniscribe();

        static HINSTANCE s_hDll;  // Uniscribe DLL handle
		
		//*/
    // Instance data
    private:
        WCHAR *m_pwszBuffer;    // Buffer to hold text
		DWORD* m_pData;
        int    m_nBufferSize;   // Size of the buffer allocated, in characters
        int    m_nTextSize;     // Size of the text in buffer excluding
                                //   NULL terminator, in characters
        // Uniscribe-specific
        //int  m_iFont;           // Font index of this buffer
        //bool m_bAnalyseRequired;// True if the string has changed since last analysis.
        //SCRIPT_STRING_ANALYSIS m_Analysis;  // Analysis for the current string

		CGuiEditBox* m_pEditBox;
    private:
        bool Grow( int nNewSize = -1 );

        // Uniscribe -- Analyse() analyses the string in the buffer
        //HRESULT Analyse();

    public:
		bool IsMultimedia();
        CUniBuffer( CGuiEditBox* pEditBox);
        ~CUniBuffer();
		void Init(int nInitialSize = 1);
        int GetBufferSize() { return m_nBufferSize; }
        bool SetBufferSize( int nSize );
        int GetTextSize() { return m_nTextSize; }
        const WCHAR *GetBuffer() { return m_pwszBuffer; }
		DWORD* GetData(){ return m_pData;}
        const WCHAR &operator[]( int n ) const { return m_pwszBuffer[n]; }
        WCHAR &operator[]( int n );
        int GetFontIndex() { return m_pEditBox->GetFont(); }
       // void SetFontIndex( int iFont ) { m_iFont = iFont; }
        void Clear();

        // Inserts the char at specified index.
        // If nIndex == -1, insert to the end.
        bool InsertChar( int nIndex, WCHAR wChar ,DWORD* data = NULL);

        // Removes the char at specified index.
        // If nIndex == -1, remove the last char.
        bool RemoveChar( int nIndex );

        // Inserts the first nCount characters of the string pStr at specified index.
        // If nCount == -1, the entire string is inserted.
        // If nIndex == -1, insert to the end.
        bool InsertString( int nIndex, const WCHAR *pStr, const DWORD* pData = NULL,int nCount = -1 );

        bool SetText( LPCWSTR wszText ,DWORD* pData = NULL);

        // Uniscribe
		int CharWidth(int nCP);
        HRESULT CPtoX( int nCP, BOOL bTrail, int *pX );
        HRESULT XtoCP( int nX, int *pCP, int *pnTrail );
        void GetPriorItemPos( int nCP, int *pPrior );
        void GetNextItemPos( int nCP, int *pPrior );
    };

protected:
	
    void PlaceCaret( int nCP );
    void ResetCaretBlink();

    CUniBuffer m_Buffer;     // Buffer to hold text
    int      m_nBorder;      // Border of the window
    int      m_nSpacing;     // Spacing between the text and the edge of border
    stRectI     m_rcText;       // Bounding rectangle for the text
    stRectI     m_rcRender[9];  // Convenient rectangles for rendering elements
    double   m_dfBlink;      // Caret blink time in milliseconds
    double   m_dfLastBlink;  // Last timestamp of caret blink
    bool     m_bCaretOn;     // Flag to indicate whether caret is currently visible
    bool     m_bInsertMode;  // If true, control is in insert mode. Else, overwrite mode.
	
    // Mouse-specific
    bool m_bMouseDrag;       // True to indicate drag in progress

	int      m_nCaret;       // Caret position, in characters
    int      m_nSelStart;    // Starting position of the selection. The caret marks the end.
    int      m_nFirstVisible;// First visible character in the edit control
    //DWORD m_TextColor;    // Text color
    DWORD m_SelTextColor; // Selected text color
    DWORD m_SelBkColor;   // Selected background color
    DWORD m_CaretColor;   // Caret color
	HRESULT OnInit();
	int		m_maxCharCount;
	int		m_minNumber;
	int		m_maxNumber;

	//media about
	DWORD	m_activeMediaType;
	mapMediaNameData	m_mediaList;

	static CGuiEditBox*	m_pLastFocusEditBox;
public:
	void OnTextChange();
	void DeleteSelectionText();
	bool PasteFromClipboard();
	void CopyToClipboard();
	void SelectAll();
	void SetSelText(const char* pText,DWORD* pData = NULL);
	int GetMaxCharCount(){ return m_maxCharCount; }
	int GetMinNumber() { return m_minNumber; }
	int GetMaxNumber() { return m_maxNumber; }

	void SetMaxCharCount(int count) { m_maxCharCount = count; }
	void SetMinNumber(int number) { m_minNumber = number; }
	void SetMaxNumber(int number) { m_maxNumber = number; }

	DWORD* GetData();
	
	static CGuiEditBox* GetLastFocusEditBox();
};
