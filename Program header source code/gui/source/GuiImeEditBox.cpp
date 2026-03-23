#include "../../engine/include/engine.h"
#include "./guiTypes.h"
#include "./guiDialog.h"
#include ".\guiimeeditbox.h"

//--------------------------------------------------------------------------------------
// CGuiImeEditBox class
//--------------------------------------------------------------------------------------
// IME constants
#define CHT_IMEFILENAME1    "TINTLGNT.IME" // New Phonetic
#define CHT_IMEFILENAME2    "CINTLGNT.IME" // New Chang Jie
#define CHT_IMEFILENAME3    "MSTCIPHA.IME" // Phonetic 5.1
#define CHS_IMEFILENAME1    "PINTLGNT.IME" // MSPY1.5/2/3
#define CHS_IMEFILENAME2    "MSSCIPYA.IME" // MSPY3 for OfficeXP

#define LANG_CHT            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL)
#define LANG_CHS            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)
#define _CHT_HKL            ( (HKL)(INT_PTR)0xE0080404 ) // New Phonetic
#define _CHT_HKL2           ( (HKL)(INT_PTR)0xE0090404 ) // New Chang Jie
#define _CHS_HKL            ( (HKL)(INT_PTR)0xE00E0804 ) // MSPY
#define MAKEIMEVERSION( major, minor ) \
    ( (DWORD)( ( (BYTE)( major ) << 24 ) | ( (BYTE)( minor ) << 16 ) ) )

#define IMEID_CHT_VER42 ( LANG_CHT | MAKEIMEVERSION( 4, 2 ) )	// New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
#define IMEID_CHT_VER43 ( LANG_CHT | MAKEIMEVERSION( 4, 3 ) )	// New(Phonetic/ChanJie)IME98a : 4.3.x.x // Win2k
#define IMEID_CHT_VER44 ( LANG_CHT | MAKEIMEVERSION( 4, 4 ) )	// New ChanJie IME98b          : 4.4.x.x // WinXP
#define IMEID_CHT_VER50 ( LANG_CHT | MAKEIMEVERSION( 5, 0 ) )	// New(Phonetic/ChanJie)IME5.0 : 5.0.x.x // WinME
#define IMEID_CHT_VER51 ( LANG_CHT | MAKEIMEVERSION( 5, 1 ) )	// New(Phonetic/ChanJie)IME5.1 : 5.1.x.x // IME2002(w/OfficeXP)
#define IMEID_CHT_VER52 ( LANG_CHT | MAKEIMEVERSION( 5, 2 ) )	// New(Phonetic/ChanJie)IME5.2 : 5.2.x.x // IME2002a(w/Whistler)
#define IMEID_CHT_VER60 ( LANG_CHT | MAKEIMEVERSION( 6, 0 ) )	// New(Phonetic/ChanJie)IME6.0 : 6.0.x.x // IME XP(w/WinXP SP1)
#define IMEID_CHS_VER41	( LANG_CHS | MAKEIMEVERSION( 4, 1 ) )	// MSPY1.5	// SCIME97 or MSPY1.5 (w/Win98, Office97)
#define IMEID_CHS_VER42	( LANG_CHS | MAKEIMEVERSION( 4, 2 ) )	// MSPY2	// Win2k/WinME
#define IMEID_CHS_VER53	( LANG_CHS | MAKEIMEVERSION( 5, 3 ) )	// MSPY3	// WinXP

// Function pointers
INPUTCONTEXT* (WINAPI * CGuiImeEditBox::_ImmLockIMC)( HIMC )
    = CGuiImeEditBox::Dummy_ImmLockIMC;
BOOL (WINAPI * CGuiImeEditBox::_ImmUnlockIMC)( HIMC )
    = CGuiImeEditBox::Dummy_ImmUnlockIMC;
LPVOID (WINAPI * CGuiImeEditBox::_ImmLockIMCC)( HIMCC )
    = CGuiImeEditBox::Dummy_ImmLockIMCC;
BOOL (WINAPI * CGuiImeEditBox::_ImmUnlockIMCC)( HIMCC )
    = CGuiImeEditBox::Dummy_ImmUnlockIMCC;
BOOL (WINAPI * CGuiImeEditBox::_ImmDisableTextFrameService)( DWORD )
    = CGuiImeEditBox::Dummy_ImmDisableTextFrameService;
LONG (WINAPI * CGuiImeEditBox::_ImmGetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD )
    = CGuiImeEditBox::Dummy_ImmGetCompositionStringW;
DWORD (WINAPI * CGuiImeEditBox::_ImmGetCandidateListW)( HIMC, DWORD, LPCANDIDATELIST, DWORD )
    = CGuiImeEditBox::Dummy_ImmGetCandidateListW;
HIMC (WINAPI * CGuiImeEditBox::_ImmGetContext)( HWND )
    = CGuiImeEditBox::Dummy_ImmGetContext;
BOOL (WINAPI * CGuiImeEditBox::_ImmReleaseContext)( HWND, HIMC )
    = CGuiImeEditBox::Dummy_ImmReleaseContext;
HIMC (WINAPI * CGuiImeEditBox::_ImmAssociateContext)( HWND, HIMC )
    = CGuiImeEditBox::Dummy_ImmAssociateContext;
BOOL (WINAPI * CGuiImeEditBox::_ImmGetOpenStatus)( HIMC )
    = CGuiImeEditBox::Dummy_ImmGetOpenStatus;
BOOL (WINAPI * CGuiImeEditBox::_ImmSetOpenStatus)( HIMC, BOOL )
    = CGuiImeEditBox::Dummy_ImmSetOpenStatus;
BOOL (WINAPI * CGuiImeEditBox::_ImmGetConversionStatus)( HIMC, LPDWORD, LPDWORD )
    = CGuiImeEditBox::Dummy_ImmGetConversionStatus;
HWND (WINAPI * CGuiImeEditBox::_ImmGetDefaultIMEWnd)( HWND )
    = CGuiImeEditBox::Dummy_ImmGetDefaultIMEWnd;
UINT (WINAPI * CGuiImeEditBox::_ImmGetIMEFileNameA)( HKL, LPSTR, UINT )
    = CGuiImeEditBox::Dummy_ImmGetIMEFileNameA;
UINT (WINAPI * CGuiImeEditBox::_ImmGetVirtualKey)( HWND )
    = CGuiImeEditBox::Dummy_ImmGetVirtualKey;
BOOL (WINAPI * CGuiImeEditBox::_ImmNotifyIME)( HIMC, DWORD, DWORD, DWORD )
    = CGuiImeEditBox::Dummy_ImmNotifyIME;
BOOL (WINAPI * CGuiImeEditBox::_ImmSetConversionStatus)( HIMC, DWORD, DWORD )
    = CGuiImeEditBox::Dummy_ImmSetConversionStatus;
BOOL (WINAPI * CGuiImeEditBox::_ImmSimulateHotKey)( HWND, DWORD )
    = CGuiImeEditBox::Dummy_ImmSimulateHotKey;
BOOL (WINAPI * CGuiImeEditBox::_ImmIsIME)( HKL )
    = CGuiImeEditBox::Dummy_ImmIsIME;
// Traditional Chinese IME
UINT (WINAPI * CGuiImeEditBox::_GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT )
    = CGuiImeEditBox::Dummy_GetReadingString;
BOOL (WINAPI * CGuiImeEditBox::_ShowReadingWindow)( HIMC, BOOL )
    = CGuiImeEditBox::Dummy_ShowReadingWindow;

BOOL (APIENTRY * CGuiImeEditBox::_VerQueryValueA)( const LPVOID, LPSTR, LPVOID *, PUINT )
    = CGuiImeEditBox::Dummy_VerQueryValueA;
BOOL (APIENTRY * CGuiImeEditBox::_GetFileVersionInfoA)( LPSTR, DWORD, DWORD, LPVOID )
    = CGuiImeEditBox::Dummy_GetFileVersionInfoA;
DWORD (APIENTRY * CGuiImeEditBox::_GetFileVersionInfoSizeA)( LPSTR, LPDWORD )
    = CGuiImeEditBox::Dummy_GetFileVersionInfoSizeA;

HINSTANCE CGuiImeEditBox::s_hDllImm32;      // IMM32 DLL handle
HINSTANCE CGuiImeEditBox::s_hDllVer;        // Version DLL handle
HKL       CGuiImeEditBox::s_hklCurrent;     // Current keyboard layout of the process
bool      CGuiImeEditBox::s_bVerticalCand;  // Indicates that the candidates are listed vertically
WCHAR     CGuiImeEditBox::s_aszIndicator[5][3] = // String to draw to indicate current input locale
            {
                L"En",
                L"\x7B80",
                L"\x7E41",
                L"\xAC00",
                L"\x3042",
            };
LPWSTR    CGuiImeEditBox::s_wszCurrIndicator   // Points to an indicator string that corresponds to current input locale
            = CGuiImeEditBox::s_aszIndicator[0];
bool      CGuiImeEditBox::s_bInsertOnType;     // Insert the character as soon as a key is pressed (Korean behavior)
HINSTANCE CGuiImeEditBox::s_hDllIme;           // Instance handle of the current IME module
HIMC      CGuiImeEditBox::s_hImcDef;           // Default input context
CGuiImeEditBox::IMESTATE  CGuiImeEditBox::s_ImeState = IMEUI_STATE_OFF;
bool      CGuiImeEditBox::s_bEnableImeSystem;  // Whether the IME system is active
POINT     CGuiImeEditBox::s_ptCompString;      // Composition string position. Updated every frame.
int       CGuiImeEditBox::s_nCompCaret;
int       CGuiImeEditBox::s_nFirstTargetConv;  // Index of the first target converted char in comp string.  If none, -1.
CGuiEditBox::CUniBuffer CGuiImeEditBox::s_CompString = CGuiEditBox::CUniBuffer( MAX_COMPSTRING_SIZE );
BYTE      CGuiImeEditBox::s_abCompStringAttr[MAX_COMPSTRING_SIZE];
DWORD     CGuiImeEditBox::s_adwCompStringClause[MAX_COMPSTRING_SIZE];
WCHAR     CGuiImeEditBox::s_wszReadingString[32];
CGuiImeEditBox::CCandList CGuiImeEditBox::s_CandList;       // Data relevant to the candidate list
bool      CGuiImeEditBox::s_bShowReadingWindow; // Indicates whether reading window is visible
bool      CGuiImeEditBox::s_bHorizontalReading; // Indicates whether the reading window is vertical or horizontal
bool      CGuiImeEditBox::s_bChineseIME;
CGrowableArray< CGuiImeEditBox::CInputLocale > CGuiImeEditBox::s_Locale; // Array of loaded keyboard layout on system


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pDialog : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
CGuiImeEditBox::CGuiImeEditBox( CGuiDialog *pDialog )
{
    m_Type = GUI_CONTROL_IMEEDITBOX;
    m_pDialog = pDialog;

    s_bEnableImeSystem = true;
    m_nIndicatorWidth = 0;
    m_ReadingColor = D3DCOLOR_ARGB( 188, 255, 255, 255 );
    m_ReadingWinColor = D3DCOLOR_ARGB( 128, 0, 0, 0 );
    m_ReadingSelColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );
    m_ReadingSelBkColor = D3DCOLOR_ARGB( 128, 80, 80, 80 );
    m_CandidateColor = D3DCOLOR_ARGB( 255, 200, 200, 200 );
    m_CandidateWinColor = D3DCOLOR_ARGB( 128, 0, 0, 0 );
    m_CandidateSelColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
    m_CandidateSelBkColor = D3DCOLOR_ARGB( 128, 158, 158, 158 );
    m_CompColor = D3DCOLOR_ARGB( 255, 200, 200, 255 );
    m_CompWinColor = D3DCOLOR_ARGB( 198, 0, 0, 0 );
    m_CompCaretColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
    m_CompTargetColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
    m_CompTargetBkColor = D3DCOLOR_ARGB( 255, 150, 150, 150 );
    m_CompTargetNonColor = D3DCOLOR_ARGB( 255, 255, 255, 0 );
    m_CompTargetNonBkColor = D3DCOLOR_ARGB( 255, 150, 150, 150 );
    m_IndicatorImeColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
    m_IndicatorEngColor = D3DCOLOR_ARGB( 255, 0, 0, 0 );
    m_IndicatorBkColor = D3DCOLOR_ARGB( 255, 128, 128, 128 );
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
CGuiImeEditBox::~CGuiImeEditBox()
{
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param nVirtKey : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiImeEditBox::SendKey( BYTE nVirtKey )
{
    keybd_event( nVirtKey, 0, 0,               0 );
    keybd_event( nVirtKey, 0, KEYEVENTF_KEYUP, 0 );
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
// Called by CGuiResourceCache's OnCreateDevice.  This gives the class a
// chance to initialize its default input context associated with the app window.
HRESULT CGuiImeEditBox::StaticOnCreateDevice()
{
    // Save the default input context
    s_hImcDef = _ImmGetContext( Engine_GetHWnd() );
    _ImmReleaseContext( Engine_GetHWnd(), s_hImcDef );

    return S_OK;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiImeEditBox::UpdateRects()
{
    // Temporary adjust m_width so that CGuiEditBox can compute
    // the correct rects for its rendering since we need to make space
    // for the indicator button
    int nWidth = m_width;
    m_width -= m_nIndicatorWidth + m_nBorder * 2; // Make room for the indicator button
    CGuiEditBox::UpdateRects();
    m_width = nWidth;  // Restore

    // Compute the indicator button rectangle
    SetRect( &m_rcIndicator, m_rcBoundingBox.right, m_rcBoundingBox.top, m_x + m_width, m_rcBoundingBox.bottom );
//    InflateRect( &m_rcIndicator, -m_nBorder, -m_nBorder );
    m_rcBoundingBox.right = m_rcBoundingBox.left + m_width;
}


//--------------------------------------------------------------------------------------
//	GetImeId( UINT uIndex )
//		returns 
//	returned value:
//	0: In the following cases
//		- Non Chinese IME input locale
//		- Older Chinese IME
//		- Other error cases
//
//	Othewise:
//      When uIndex is 0 (default)
//			bit 31-24:	Major version
//			bit 23-16:	Minor version
//			bit 15-0:	Language ID
//		When uIndex is 1
//			pVerFixedInfo->dwFileVersionLS
//
//	Use IMEID_VER and IMEID_LANG macro to extract version and language information.
//	

// We define the locale-invariant ID ourselves since it doesn't exist prior to WinXP
// For more information, see the CompareString() reference.
#define LCID_INVARIANT MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT)

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param uIndex : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
DWORD CGuiImeEditBox::GetImeId( UINT uIndex )
{
    static HKL hklPrev = 0;
    static DWORD dwID[2] = { 0, 0 };  // Cache the result
    
    DWORD   dwVerSize;
    DWORD   dwVerHandle;
    LPVOID  lpVerBuffer;
    LPVOID  lpVerData;
    UINT    cbVerData;
    char    szTmp[1024];

    if( uIndex >= sizeof( dwID ) / sizeof( dwID[0] ) )
        return 0;

    if( hklPrev == s_hklCurrent )
        return dwID[uIndex];

    hklPrev = s_hklCurrent;  // Save for the next invocation

    // Check if we are using an older Chinese IME
    if( !( ( s_hklCurrent == _CHT_HKL ) || ( s_hklCurrent == _CHT_HKL2 ) || ( s_hklCurrent == _CHS_HKL ) ) )
    {
        dwID[0] = dwID[1] = 0;
        return dwID[uIndex];
    }

    // Obtain the IME file name
    if ( !_ImmGetIMEFileNameA( s_hklCurrent, szTmp, ( sizeof(szTmp) / sizeof(szTmp[0]) ) - 1 ) )
    {
        dwID[0] = dwID[1] = 0;
        return dwID[uIndex];
    }

    // Check for IME that doesn't implement reading string API
    if ( !_GetReadingString )
    {
        if( ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME2, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME3, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME2, -1 ) != CSTR_EQUAL ) )
        {
            dwID[0] = dwID[1] = 0;
            return dwID[uIndex];
        }
    }

    dwVerSize = _GetFileVersionInfoSizeA( szTmp, &dwVerHandle );
    if( dwVerSize )
    {
        lpVerBuffer = HeapAlloc( GetProcessHeap(), 0, dwVerSize );
        if( lpVerBuffer )
        {
            if( _GetFileVersionInfoA( szTmp, dwVerHandle, dwVerSize, lpVerBuffer ) )
            {
                if( _VerQueryValueA( lpVerBuffer, "\\", &lpVerData, &cbVerData ) )
                {
                    DWORD dwVer = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionMS;
                    dwVer = ( dwVer & 0x00ff0000 ) << 8 | ( dwVer & 0x000000ff ) << 16;
                    if( _GetReadingString
                        ||
                        ( GetLanguage() == LANG_CHT &&
                          ( dwVer == MAKEIMEVERSION(4, 2) || 
                            dwVer == MAKEIMEVERSION(4, 3) || 
                            dwVer == MAKEIMEVERSION(4, 4) || 
                            dwVer == MAKEIMEVERSION(5, 0) ||
                            dwVer == MAKEIMEVERSION(5, 1) ||
                            dwVer == MAKEIMEVERSION(5, 2) ||
                            dwVer == MAKEIMEVERSION(6, 0) ) )
                        ||
                        ( GetLanguage() == LANG_CHS &&
                          ( dwVer == MAKEIMEVERSION(4, 1) ||
                            dwVer == MAKEIMEVERSION(4, 2) ||
                            dwVer == MAKEIMEVERSION(5, 3) ) )
                      )
                    {
                        dwID[0] = dwVer | GetLanguage();
                        dwID[1] = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionLS;
                    }
                }
            }
            HeapFree( GetProcessHeap(), 0, lpVerBuffer );
        }
    }

    return dwID[uIndex];
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiImeEditBox::CheckInputLocale()
{
    static HKL hklPrev = 0;
    s_hklCurrent = GetKeyboardLayout( 0 );
    if ( hklPrev == s_hklCurrent )
        return;

    hklPrev = s_hklCurrent;
    switch ( GetPrimaryLanguage() )
    {
        // Simplified Chinese
        case LANG_CHINESE:
            s_bVerticalCand = true;
            switch ( GetSubLanguage() )
            {
                case SUBLANG_CHINESE_SIMPLIFIED:
                    s_wszCurrIndicator = s_aszIndicator[INDICATOR_CHS];
                    s_bVerticalCand = GetImeId() == 0;
                    break;
                case SUBLANG_CHINESE_TRADITIONAL:
                    s_wszCurrIndicator = s_aszIndicator[INDICATOR_CHT];
                    break;
                default:    // unsupported sub-language
                    s_wszCurrIndicator = s_aszIndicator[INDICATOR_NON_IME];
                    break;
            }
            break;
        // Korean
        case LANG_KOREAN:
            s_wszCurrIndicator = s_aszIndicator[INDICATOR_KOREAN];
            s_bVerticalCand = false;
            break;
        // Japanese
        case LANG_JAPANESE:
            s_wszCurrIndicator = s_aszIndicator[INDICATOR_JAPANESE];
            s_bVerticalCand = true;
            break;
        default:
            // A non-IME language.  Obtain the language abbreviation
            // and store it for rendering the indicator later.
            s_wszCurrIndicator = s_aszIndicator[INDICATOR_NON_IME];
    }

    // If non-IME, use the language abbreviation.
    if( s_wszCurrIndicator == s_aszIndicator[INDICATOR_NON_IME] )
    {
        WCHAR wszLang[5];
        GetLocaleInfoW( MAKELCID( LOWORD( s_hklCurrent ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, wszLang, 5 );
        s_wszCurrIndicator[0] = wszLang[0];
        s_wszCurrIndicator[1] = towlower( wszLang[1] );
    }
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiImeEditBox::CheckToggleState()
{
    CheckInputLocale();
    bool bIme = _ImmIsIME( s_hklCurrent ) != 0;
    s_bChineseIME = ( GetPrimaryLanguage() == LANG_CHINESE ) && bIme;

    HIMC hImc;
    if( NULL != ( hImc = _ImmGetContext( Engine_GetHWnd() ) ) )
    {
        if( s_bChineseIME )
        {
            DWORD dwConvMode, dwSentMode;
            _ImmGetConversionStatus( hImc, &dwConvMode, &dwSentMode );
            s_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;
        }
        else
        {
            s_ImeState = ( bIme && _ImmGetOpenStatus( hImc ) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
        }
        _ImmReleaseContext( Engine_GetHWnd(), hImc );
    }
    else
        s_ImeState = IMEUI_STATE_OFF;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param bEnable : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
// Enable/disable the entire IME system.  When disabled, the default IME handling
// kicks in.
void CGuiImeEditBox::EnableImeSystem( bool bEnable )
{
    s_bEnableImeSystem = bEnable;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
// Sets up IME-specific APIs for the IME edit controls.  This is called every time
// the input locale changes.
void CGuiImeEditBox::SetupImeApi()
{
    char szImeFile[MAX_PATH + 1];

    _GetReadingString = NULL;
    _ShowReadingWindow = NULL;
    if( _ImmGetIMEFileNameA( s_hklCurrent, szImeFile, sizeof(szImeFile)/sizeof(szImeFile[0]) - 1 ) == 0 )
        return;

    if( s_hDllIme ) FreeLibrary( s_hDllIme );
    s_hDllIme = LoadLibraryA( szImeFile );
    if ( !s_hDllIme )
        return;
    _GetReadingString = (UINT (WINAPI*)(HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT))
        ( GetProcAddress( s_hDllIme, "GetReadingString" ) );
    _ShowReadingWindow =(BOOL (WINAPI*)(HIMC, BOOL))
        ( GetProcAddress( s_hDllIme, "ShowReadingWindow" ) );
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
// Resets the composition string.
void CGuiImeEditBox::ResetCompositionString()
{
    s_nCompCaret = 0;
    s_CompString.SetText( L"" );
    ZeroMemory( s_abCompStringAttr, sizeof(s_abCompStringAttr) );
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param bUseBackSpace : √Ë ˆ
 * \param iNewStrLen : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
// Truncate composition string by sending keystrokes to the window.
void CGuiImeEditBox::TruncateCompString( bool bUseBackSpace, int iNewStrLen )
{
    if( !s_bInsertOnType )
        return;

    int cc = (int) wcslen( s_CompString.GetBuffer() );
    // Send right arrow keystrokes to move the caret
    //   to the end of the composition string.
    for (int i = 0; i < cc - s_nCompCaret; ++i )
        SendMessage( Engine_GetHWnd(), WM_KEYDOWN, VK_RIGHT, 0 );
    SendMessage( Engine_GetHWnd(), WM_KEYUP, VK_RIGHT, 0 );

    if( bUseBackSpace || m_bInsertMode )
        iNewStrLen = 0;

    // The caller sets bUseBackSpace to false if there's possibility of sending
    // new composition string to the app right after this function call.
    // 
    // If the app is in overwriting mode and new comp string is 
    // shorter than current one, delete previous comp string 
    // till it's same long as the new one. Then move caret to the beginning of comp string.
    // New comp string will overwrite old one.
    if( iNewStrLen < cc )
    {
        for( int i = 0; i < cc - iNewStrLen; ++i )
        {
            SendMessage( Engine_GetHWnd(), WM_KEYDOWN, VK_BACK, 0 );  // Backspace character
            SendMessageW( Engine_GetHWnd(), WM_CHAR, VK_BACK, 0 );
        }
        SendMessage( Engine_GetHWnd(), WM_KEYUP, VK_BACK, 0 );
    }
    else
        iNewStrLen = cc;

    // Move the caret to the beginning by sending left keystrokes
    for (int i = 0; i < iNewStrLen; ++i )
        SendMessage( Engine_GetHWnd(), WM_KEYDOWN, VK_LEFT, 0 );
    SendMessage( Engine_GetHWnd(), WM_KEYUP, VK_LEFT, 0 );
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
// Sends the current composition string to the application by sending keystroke
// messages.
void CGuiImeEditBox::SendCompString()
{
    for( int i = 0; i < wcslen(s_CompString.GetBuffer() ); ++i )
        MsgProc( WM_CHAR, (WPARAM)s_CompString[i], 0 );
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param bSend : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
// Outputs current composition string then cleans up the composition task.
void CGuiImeEditBox::FinalizeString( bool bSend )
{
    HIMC hImc;
    if( NULL == ( hImc = _ImmGetContext( Engine_GetHWnd() ) ) )
        return;

    static bool bProcessing = false;
    if( bProcessing )    // avoid infinite recursion
    {
        TRACE( "CGuiImeEditBox::FinalizeString: Reentrant detected!\n" );
        _ImmReleaseContext( Engine_GetHWnd(), hImc );
        return;
    }
    bProcessing = true;

    if( !s_bInsertOnType && bSend )
    {
        // Send composition string to app.
        LONG lLength = wcslen( s_CompString.GetBuffer() );
        // In case of CHT IME, don't send the trailing double byte space, if it exists.
        if( GetLanguage() == LANG_CHT
            && s_CompString[lLength - 1] == 0x3000 )
        {
            --lLength;
        }
        SendCompString();
    }

    ResetCompositionString();
    // Clear composition string in IME
    _ImmNotifyIME( hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
    // the following line is necessary as Korean IME doesn't close cand list
    // when comp string is cancelled.
    _ImmNotifyIME( hImc, NI_CLOSECANDIDATE, 0, 0 ); 
    _ImmReleaseContext( Engine_GetHWnd(), hImc );
    bProcessing = false;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param dwId : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
// Determine whether the reading window should be vertical or horizontal.
void CGuiImeEditBox::GetReadingWindowOrientation( DWORD dwId )
{
    s_bHorizontalReading = ( s_hklCurrent == _CHS_HKL ) || ( s_hklCurrent == _CHT_HKL2 ) || ( dwId == 0 );
    if( !s_bHorizontalReading && ( dwId & 0x0000FFFF ) == LANG_CHT )
    {
        char szRegPath[MAX_PATH];
        HKEY hKey;
        DWORD dwVer = dwId & 0xFFFF0000;
        strcpy( szRegPath, "software\\microsoft\\windows\\currentversion\\" );
        strcat( szRegPath, ( dwVer >= MAKEIMEVERSION( 5, 1 ) ) ? "MSTCIPH" : "TINTLGNT" );
        LONG lRc = RegOpenKeyEx( HKEY_CURRENT_USER, szRegPath, 0, KEY_READ, &hKey );
        if (lRc == ERROR_SUCCESS)
        {
            DWORD dwSize = sizeof(DWORD), dwMapping, dwType;
            lRc = RegQueryValueEx( hKey, "Keyboard Mapping", NULL, &dwType, (PBYTE)&dwMapping, &dwSize );
            if (lRc == ERROR_SUCCESS)
            {
                if ( ( dwVer <= MAKEIMEVERSION( 5, 0 ) && 
                       ( (BYTE)dwMapping == 0x22 || (BYTE)dwMapping == 0x23 ) )
                     ||
                     ( ( dwVer == MAKEIMEVERSION( 5, 1 ) || dwVer == MAKEIMEVERSION( 5, 2 ) ) &&
                       (BYTE)dwMapping >= 0x22 && (BYTE)dwMapping <= 0x24 )
                   )
                {
                    s_bHorizontalReading = true;
                }
            }
            RegCloseKey( hKey );
        }
    }
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
// Obtain the reading string upon WM_IME_NOTIFY/INM_PRIVATE notification.
void CGuiImeEditBox::GetPrivateReadingString()
{
    DWORD dwId = GetImeId();
    if( !dwId )
    {
        s_bShowReadingWindow = false;
        return;
    }

    HIMC hImc;
    hImc = _ImmGetContext( Engine_GetHWnd() );
    if( !hImc )
    {
        s_bShowReadingWindow = false;
        return;
    }

    DWORD dwReadingStrLen = 0;
    DWORD dwErr = 0;
    WCHAR *pwszReadingStringBuffer = NULL;  // Buffer for when the IME supports GetReadingString()
    WCHAR *wstr = 0;
    bool bUnicodeIme = false;  // Whether the IME context component is Unicode.
    INPUTCONTEXT *lpIC = NULL;

    if( _GetReadingString )
    {
        UINT uMaxUiLen;
        BOOL bVertical;
        // Obtain the reading string size
        dwReadingStrLen = _GetReadingString( hImc, 0, NULL, (PINT)&dwErr, &bVertical, &uMaxUiLen );
        if( dwReadingStrLen )
        {
            wstr = pwszReadingStringBuffer = (LPWSTR)HeapAlloc( GetProcessHeap(), 0, sizeof(WCHAR) * dwReadingStrLen );
            if( !pwszReadingStringBuffer )
            {
                // Out of memory. Exit.
                _ImmReleaseContext( Engine_GetHWnd(), hImc );
                return;
            }

            // Obtain the reading string
            dwReadingStrLen = _GetReadingString( hImc, dwReadingStrLen, wstr, (PINT)&dwErr, &bVertical, &uMaxUiLen );
        }

        s_bHorizontalReading = !bVertical;
        bUnicodeIme = true;
    }
    else
    {
        // IMEs that doesn't implement Reading String API

        lpIC = _ImmLockIMC( hImc );
        
        LPBYTE p = 0;
        switch( dwId )
        {
            case IMEID_CHT_VER42: // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
            case IMEID_CHT_VER43: // New(Phonetic/ChanJie)IME98a : 4.3.x.x // WinMe, Win2k
            case IMEID_CHT_VER44: // New ChanJie IME98b          : 4.4.x.x // WinXP
                p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 24 );
                if( !p ) break;
                dwReadingStrLen = *(DWORD *)( p + 7 * 4 + 32 * 4 );
                dwErr = *(DWORD *)( p + 8 * 4 + 32 * 4 );
                wstr = (WCHAR *)( p + 56 );
                bUnicodeIme = true;
                break;

            case IMEID_CHT_VER50: // 5.0.x.x // WinME
                p = *(LPBYTE *)( (LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 3 * 4 );
                if( !p ) break;
                p = *(LPBYTE *)( (LPBYTE)p + 1*4 + 5*4 + 4*2 );
                if( !p ) break;
                dwReadingStrLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16);
                dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 + 1*4);
                wstr = (WCHAR *)(p + 1*4 + (16*2+2*4) + 5*4);
                bUnicodeIme = false;
                break;

            case IMEID_CHT_VER51: // 5.1.x.x // IME2002(w/OfficeXP)
            case IMEID_CHT_VER52: // 5.2.x.x // (w/whistler)
            case IMEID_CHS_VER53: // 5.3.x.x // SCIME2k or MSPY3 (w/OfficeXP and Whistler)
                p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 4);
                if( !p ) break;
                p = *(LPBYTE *)((LPBYTE)p + 1*4 + 5*4);
                if( !p ) break;
                dwReadingStrLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2);
                dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2 + 1*4);
                wstr  = (WCHAR *) (p + 1*4 + (16*2+2*4) + 5*4);
                bUnicodeIme = true;
                break;

            // the code tested only with Win 98 SE (MSPY 1.5/ ver 4.1.0.21)
            case IMEID_CHS_VER41:
            {
                int nOffset;
                nOffset = ( GetImeId( 1 ) >= 0x00000002 ) ? 8 : 7;

                p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + nOffset * 4);
                if( !p ) break;
                dwReadingStrLen = *(DWORD *)(p + 7*4 + 16*2*4);
                dwErr = *(DWORD *)(p + 8*4 + 16*2*4);
                dwErr = min( dwErr, dwReadingStrLen );
                wstr = (WCHAR *)(p + 6*4 + 16*2*1);
                bUnicodeIme = true;
                break;
            }

            case IMEID_CHS_VER42: // 4.2.x.x // SCIME98 or MSPY2 (w/Office2k, Win2k, WinME, etc)
            {
	            OSVERSIONINFOW osi;
                osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
	            GetVersionExW( &osi );

                int nTcharSize = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? sizeof(WCHAR) : sizeof(char);
                p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 1*4 + 1*4 + 6*4);
                if( !p ) break;
                dwReadingStrLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize);
                dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize + 1*4);
                wstr  = (WCHAR *) (p + 1*4 + (16*2+2*4) + 5*4);
                bUnicodeIme = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? true : false;
            }
        }   // switch
    }

    // Copy the reading string to the candidate list first
    s_CandList.awszCandidate[0][0] = 0;
    s_CandList.awszCandidate[1][0] = 0;
    s_CandList.awszCandidate[2][0] = 0;
    s_CandList.awszCandidate[3][0] = 0;
    s_CandList.dwCount = dwReadingStrLen;
    s_CandList.dwSelection = (DWORD)-1; // do not select any char
    if( bUnicodeIme )
    {
        UINT i;
        for( i = 0; i < dwReadingStrLen; ++i ) // dwlen > 0, if known IME
        {
            if( dwErr <= i && s_CandList.dwSelection == (DWORD)-1 )
            {
                // select error char
                s_CandList.dwSelection = i;
            }

            s_CandList.awszCandidate[i][0] = wstr[i];
            s_CandList.awszCandidate[i][1] = 0;
        }
        s_CandList.awszCandidate[i][0] = 0;
    }
    else
    {
        char *p = (char *)wstr;
        DWORD i, j;
        for( i = 0, j = 0; i < dwReadingStrLen; ++i, ++j ) // dwlen > 0, if known IME
        {
            if( dwErr <= i && s_CandList.dwSelection == (DWORD)-1 )
            {
                s_CandList.dwSelection = j;
            }
            // Obtain the current code page
	        WCHAR wszCodePage[8];
            UINT uCodePage = CP_ACP;  // Default code page
            if( GetLocaleInfoW( MAKELCID( GetLanguage(), SORT_DEFAULT ),
                                LOCALE_IDEFAULTANSICODEPAGE,
                                wszCodePage,
                                sizeof(wszCodePage)/sizeof(wszCodePage[0]) ) )
            {
                uCodePage = wcstoul( wszCodePage, NULL, 0 );
            }
            MultiByteToWideChar( uCodePage, 0, p + i, IsDBCSLeadByteEx( uCodePage, p[i] ) ? 2 : 1,
                                 s_CandList.awszCandidate[j], 1 );
            if( IsDBCSLeadByteEx( uCodePage, p[i] ) )
                ++i;
        }
        s_CandList.awszCandidate[j][0] = 0;
        s_CandList.dwCount = j;
    }
    if( !_GetReadingString )
    {
        _ImmUnlockIMCC( lpIC->hPrivate );
        _ImmUnlockIMC( hImc );
        GetReadingWindowOrientation( dwId );
    }
    _ImmReleaseContext( Engine_GetHWnd(), hImc );

    if( pwszReadingStringBuffer )
        HeapFree( GetProcessHeap(), 0, pwszReadingStringBuffer );

    // Copy the string to the reading string buffer
    if( s_CandList.dwCount > 0 )
        s_bShowReadingWindow = true;
    else
        s_bShowReadingWindow = false;
    if( s_bHorizontalReading )
    {
        s_CandList.nReadingError = -1;
        s_wszReadingString[0] = 0;
        for( UINT i = 0; i < s_CandList.dwCount; ++i )
        {
            if( s_CandList.dwSelection == i )
                s_CandList.nReadingError = wcslen( s_wszReadingString );
            wcsncat( s_wszReadingString, s_CandList.awszCandidate[i], 32 - lstrlenW( s_wszReadingString ) - 1 );
        }
    }

    s_CandList.dwPageSize = MAX_CANDLIST;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
// This function is used only briefly in CHT IME handling,
// so accelerator isn't processed.
void CGuiImeEditBox::PumpMessage()
{
    MSG msg;

    while( PeekMessageW( &msg, NULL, 0, 0, PM_NOREMOVE ) )
    {
        if( !GetMessageW( &msg, NULL, 0, 0 ) )
        {
            PostQuitMessage( (int)msg.wParam );
            return;
        }
        TranslateMessage( &msg );
        DispatchMessageA( &msg );
    }
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiImeEditBox::OnFocusIn()
{
    CGuiEditBox::OnFocusIn();

    if( s_bEnableImeSystem )
    {
        _ImmAssociateContext( Engine_GetHWnd(), s_hImcDef );
        CheckToggleState();
    } else
        _ImmAssociateContext( Engine_GetHWnd(), NULL );

    //
    // Set up the IME global state according to the current instance state
    //
    HIMC hImc;
    if( NULL != ( hImc = _ImmGetContext( Engine_GetHWnd() ) ) ) 
    {
        if( !s_bEnableImeSystem )
            s_ImeState = IMEUI_STATE_OFF;

        _ImmReleaseContext( Engine_GetHWnd(), hImc );
        CheckToggleState();
    }
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiImeEditBox::OnFocusOut()
{
    CGuiEditBox::OnFocusOut();

    FinalizeString( false );  // Don't send the comp string as to match RichEdit behavior

    _ImmAssociateContext( Engine_GetHWnd(), NULL );
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param uMsg : √Ë ˆ
 * \param wParam : √Ë ˆ
 * \param lParam : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
bool CGuiImeEditBox::StaticMsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    HIMC hImc;

    if( !s_bEnableImeSystem )
        return false;

    switch( uMsg )
    {
			/*
        case WM_ACTIVATEAPP:
            if( wParam )
            {
                // Populate s_Locale with the list of keyboard layouts.
                UINT cKL = GetKeyboardLayoutList( 0, NULL );
                s_Locale.RemoveAll();
                HKL *phKL = new HKL[cKL];
                if( phKL )
                {
                    GetKeyboardLayoutList( cKL, phKL );
                    for( UINT i = 0; i < cKL; ++i )
                    {
                        CInputLocale Locale;

                        // Filter out East Asian languages that are not IME.
                        if( ( PRIMARYLANGID( LOWORD( phKL[i] ) ) == LANG_CHINESE ||
                              PRIMARYLANGID( LOWORD( phKL[i] ) ) == LANG_JAPANESE ||
                              PRIMARYLANGID( LOWORD( phKL[i] ) ) == LANG_KOREAN ) &&
                              !_ImmIsIME( phKL[i] ) )
                              continue;

                        // If this language is already in the list, don't add it again.
                        bool bBreak = false;
                        for( int e = 0; e < s_Locale.GetSize(); ++e )
                            if( LOWORD( s_Locale.GetAt( e ).m_hKL ) ==
                                LOWORD( phKL[i] ) )
                            {
                                bBreak = true;
                                break;
                            }
                        if( bBreak )
                            break;

                        Locale.m_hKL = phKL[i];
                        WCHAR wszDesc[128] = L"";
                        switch( PRIMARYLANGID( LOWORD( phKL[i] ) ) )
                        {
                            // Simplified Chinese
                            case LANG_CHINESE:
                                switch( SUBLANGID( LOWORD( phKL[i] ) ) )
                                {
                                    case SUBLANG_CHINESE_SIMPLIFIED:
                                        lstrcpy( Locale.m_wszLangAbb, s_aszIndicator[INDICATOR_CHS] );
                                        break;
                                    case SUBLANG_CHINESE_TRADITIONAL:
                                        lstrcpy( Locale.m_wszLangAbb, s_aszIndicator[INDICATOR_CHT] );
                                        break;
                                    default:    // unsupported sub-language
                                        GetLocaleInfoW( MAKELCID( LOWORD( phKL[i] ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, wszDesc, 128 );
                                        Locale.m_wszLangAbb[0] = wszDesc[0];
                                        Locale.m_wszLangAbb[1] = towlower( wszDesc[1] );
                                        Locale.m_wszLangAbb[2] = L'\0';
                                        break;
                                }
                                break;
                            // Korean
                            case LANG_KOREAN:
                                lstrcpy( Locale.m_wszLangAbb, s_aszIndicator[INDICATOR_KOREAN] );
                                break;
                            // Japanese
                            case LANG_JAPANESE:
                                lstrcpy( Locale.m_wszLangAbb, s_aszIndicator[INDICATOR_JAPANESE] );
                                break;         
                            default:
                                // A non-IME language.  Obtain the language abbreviation
                                // and store it for rendering the indicator later.
                                GetLocaleInfoW( MAKELCID( LOWORD( phKL[i] ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, wszDesc, 128 );
                                Locale.m_wszLangAbb[0] = wszDesc[0];
                                Locale.m_wszLangAbb[1] = towlower( wszDesc[1] );
                                Locale.m_wszLangAbb[2] = L'\0';
                                break;
                        }

                        GetLocaleInfoW( MAKELCID( LOWORD( phKL[i] ), SORT_DEFAULT ), LOCALE_SLANGUAGE, wszDesc, 128 );
                        wcsncpy( Locale.m_wszLang, wszDesc, sizeof(Locale.m_wszLang) / sizeof(Locale.m_wszLang[0]) );
                        Locale.m_wszLang[sizeof(Locale.m_wszLang) / sizeof(Locale.m_wszLang[0]) - 1] = L'\0';

                        s_Locale.Add( Locale );
                    }
                    delete[] phKL;
                }
            }
            break;
        case WM_INPUTLANGCHANGE:
            DXUTTRACE( L"WM_INPUTLANGCHANGE\n" );
            {
                UINT uLang = GetPrimaryLanguage();
                CheckToggleState();
                if ( uLang != GetPrimaryLanguage() )
                {
                    // Korean IME always inserts on keystroke.  Other IMEs do not.
                    s_bInsertOnType = ( GetPrimaryLanguage() == LANG_KOREAN );
                }

                // IME changed.  Setup the new IME.
                SetupImeApi();
                if( _ShowReadingWindow )
                {
                    if ( NULL != ( hImc = _ImmGetContext( Engine_GetHWnd() ) ) )
                    {
                        _ShowReadingWindow( hImc, false );
                        _ImmReleaseContext( Engine_GetHWnd(), hImc );
                    }
                }
            }
            return true;

        case WM_IME_SETCONTEXT:
            DXUTTRACE( L"WM_IME_SETCONTEXT\n" );
            //
            // We don't want anything to display, so we have to clear this
            //
            lParam = 0;
            return false;

        // Handle WM_IME_STARTCOMPOSITION here since
        // we do not want the default IME handler to see
        // this when our fullscreen app is running.
        case WM_IME_STARTCOMPOSITION:
            DXUTTRACE( L"WM_IME_STARTCOMPOSITION\n" );
            ResetCompositionString();
            // Since the composition string has its own caret, we don't render
            // the edit control's own caret to avoid double carets on screen.
            s_bHideCaret = true;
            return true;

        case WM_IME_COMPOSITION:
            DXUTTRACE( L"WM_IME_COMPOSITION\n" );
            return true;
			//*/
    }

    return false;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param uMsg : √Ë ˆ
 * \param pt : √Ë ˆ
 * \param wParam : √Ë ˆ
 * \param lParam : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
bool CGuiImeEditBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch( uMsg )
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
            DXUTFontNode* pFont = m_pDialog->GetFont( m_Elements.GetAt( 9 )->iFont );

            // Check if this click is on top of the composition string
            int nCompStrWidth;
            s_CompString.CPtoX( s_CompString.GetTextSize(), FALSE, &nCompStrWidth );

            if( s_ptCompString.x <= pt.x &&
                s_ptCompString.y <= pt.y &&
                s_ptCompString.x + nCompStrWidth > pt.x &&
                s_ptCompString.y + pFont->nHeight > pt.y )
            {
                int nCharBodyHit, nCharHit;
                int nTrail;

                // Determine the character clicked on.
                s_CompString.XtoCP( pt.x - s_ptCompString.x, &nCharBodyHit, &nTrail );
                if( nTrail && nCharBodyHit < s_CompString.GetTextSize() )
                    nCharHit = nCharBodyHit + 1;
                else
                    nCharHit = nCharBodyHit;

                // Now generate keypress events to move the comp string cursor
                // to the click point.  First, if the candidate window is displayed,
                // send Esc to close it.
                HIMC hImc = _ImmGetContext( Engine_GetHWnd() );
                if( !hImc )
                    return true;

                _ImmNotifyIME( hImc, NI_CLOSECANDIDATE, 0, 0 );
                _ImmReleaseContext( Engine_GetHWnd(), hImc );

                switch( GetPrimaryLanguage() )
                {
                    case LANG_JAPANESE:
                        // For Japanese, there are two cases.  If s_nFirstTargetConv is
                        // -1, the comp string hasn't been converted yet, and we use
                        // s_nCompCaret.  For any other value of s_nFirstTargetConv,
                        // the string has been converted, so we use clause information.

                        if( s_nFirstTargetConv != -1 )
                        {
                            int nClauseClicked = 0;
                            while( (int)s_adwCompStringClause[nClauseClicked + 1] <= nCharBodyHit )
                                ++nClauseClicked;

                            int nClauseSelected = 0;
                            while( (int)s_adwCompStringClause[nClauseSelected + 1] <= s_nFirstTargetConv )
                                ++nClauseSelected;

                            BYTE nVirtKey = nClauseClicked > nClauseSelected ? VK_RIGHT : VK_LEFT;
                            int nSendCount = abs( nClauseClicked - nClauseSelected );
                            while( nSendCount-- > 0 )
                                SendKey( nVirtKey );

                            return true;
                        }

                        // Not converted case. Fall thru to Chinese case.

                    case LANG_CHINESE:
                    {
                        // For Chinese, use s_nCompCaret.
                        BYTE nVirtKey = nCharHit > s_nCompCaret ? VK_RIGHT : VK_LEFT;
                        int nSendCount = abs( nCharHit - s_nCompCaret );
                        while( nSendCount-- > 0 )
                            SendKey( nVirtKey );
                        break;
                    }
                }

                return true;
            }

            // Check if the click is on top of the candidate window
            if( s_CandList.bShowWindow && PtInRect( &s_CandList.rcCandidate, pt ) )
            {
                if( s_bVerticalCand )
                {
                    // Vertical candidate window

                    // Compute the row the click is on
                    int nRow = ( pt.y - s_CandList.rcCandidate.top ) / pFont->nHeight;

                    if( nRow < (int)s_CandList.dwCount )
                    {
                        // nRow is a valid entry.
                        // Now emulate keystrokes to select the candidate at this row.
                        switch( GetPrimaryLanguage() )
                        {
                            case LANG_CHINESE:
                            case LANG_KOREAN:
                                // For Chinese and Korean, simply send the number keystroke.
                                SendKey( '0' + nRow + 1 );
                                break;

                            case LANG_JAPANESE:
                                // For Japanese, move the selection to the target row,
                                // then send Right, then send Left.

                                BYTE nVirtKey;
                                if( nRow > (int)s_CandList.dwSelection )
                                    nVirtKey = VK_DOWN;
                                else
                                    nVirtKey = VK_UP;
                                int nNumToHit = abs( int( nRow - s_CandList.dwSelection ) );
                                for( int nStrike = 0; nStrike < nNumToHit; ++nStrike )
                                    SendKey( nVirtKey );

                                // Do this to close the candidate window without ending composition.
                                SendKey( VK_RIGHT );
                                SendKey( VK_LEFT );

                                break;
                        }
                    }
                } else
                {
                    // Horizontal candidate window

                    // Determine which the character the click has hit.
                    int nCharHit;
                    int nTrail;
                    s_CandList.HoriCand.XtoCP( pt.x - s_CandList.rcCandidate.left, &nCharHit, &nTrail );

                    // Determine which candidate string the character belongs to.
                    int nCandidate = s_CandList.dwCount - 1;

                    int nEntryStart = 0;
                    for( UINT i = 0; i < s_CandList.dwCount; ++i )
                    {
                        if( nCharHit >= nEntryStart )
                        {
                            // Haven't found it.
                            nEntryStart += lstrlenW( s_CandList.awszCandidate[i] ) + 1;  // plus space separator
                        } else
                        {
                            // Found it.  This entry starts at the right side of the click point,
                            // so the char belongs to the previous entry.
                            nCandidate = i - 1;
                            break;
                        }
                    }

                    // Now emulate keystrokes to select the candidate entry.
                    switch( GetPrimaryLanguage() )
                    {
                        case LANG_CHINESE:
                        case LANG_KOREAN:
                            // For Chinese and Korean, simply send the number keystroke.
                            SendKey( '0' + nCandidate + 1 );
                            break;
                    }
                }

                return true;
            }
        }
    }

    // If we didn't care for the msg, let the parent process it.
    return CGuiEditBox::HandleMouse( uMsg, pt, wParam, lParam );
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param uMsg : √Ë ˆ
 * \param wParam : √Ë ˆ
 * \param lParam : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
bool CGuiImeEditBox::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    // TEMPORARY!!! TEMPORARY!!! TEMPORARY!!!
    // See how I can get rid of trapped later.
    bool trappedData;
    bool *trapped = &trappedData;

    HIMC hImc;
    static LPARAM lAlt = 0x80000000, lCtrl = 0x80000000, lShift = 0x80000000;

    *trapped = false;
    if( !s_bEnableImeSystem )
        return CGuiEditBox::MsgProc( uMsg, wParam, lParam );

    switch( uMsg )
    {
        //
        //  IME Handling
        //
        case WM_IME_COMPOSITION:
            DXUTTRACE( L"WM_IME_COMPOSITION\n" );
            {
                LONG lRet;  // Returned count in CHARACTERS
                WCHAR wszCompStr[MAX_COMPSTRING_SIZE];

                *trapped = true;
                if( NULL == ( hImc = _ImmGetContext( Engine_GetHWnd() ) ) )
                {
                    break;
                }

                // ResultStr must be processed before composition string.
                if ( lParam & GCS_RESULTSTR )
                {
                    DXUTTRACE( L"  GCS_RESULTSTR\n" );
                    lRet = _ImmGetCompositionStringW( hImc, GCS_RESULTSTR, wszCompStr, sizeof( wszCompStr ) );
                    if( lRet > 0 )
                    {
                        lRet /= sizeof(WCHAR);
                        wszCompStr[lRet] = 0;  // Force terminate
                        TruncateCompString( false, (int)wcslen( wszCompStr ) );
                        s_CompString.SetText( wszCompStr );
                        SendCompString();
                        ResetCompositionString();
                    }
                }

                //
                // Reads in the composition string.
                //
                if ( lParam & GCS_COMPSTR )
                {
                    DXUTTRACE( L"  GCS_COMPSTR\n" );
                    //////////////////////////////////////////////////////
                    // Retrieve the latest user-selected IME candidates
                    lRet = _ImmGetCompositionStringW( hImc, GCS_COMPSTR, wszCompStr, sizeof( wszCompStr ) );
                    if( lRet > 0 )
                    {
                        lRet /= sizeof(WCHAR);  // Convert size in byte to size in char
                        wszCompStr[lRet] = 0;  // Force terminate
                        //
                        // Remove the whole of the string
                        //
                        TruncateCompString( false, (int)wcslen( wszCompStr ) );

                        s_CompString.SetText( wszCompStr );
                        lRet = _ImmGetCompositionStringW( hImc, GCS_COMPATTR, s_abCompStringAttr, sizeof( s_abCompStringAttr ) );
                        if( lRet > 0 )
                            s_abCompStringAttr[lRet] = 0;  // ??? Is this needed for attributes?

                        // Older CHT IME uses composition string for reading string
                        if ( GetLanguage() == LANG_CHT && !GetImeId() )
                        {
                            if( wcslen( s_CompString.GetBuffer() ) )
                            {
                                s_CandList.dwCount = 4;             // Maximum possible length for reading string is 4
                                s_CandList.dwSelection = (DWORD)-1; // don't select any candidate

                                // Copy the reading string to the candidate list
                                for( int i = 3; i >= 0; --i )
                                {
                                    if( i > wcslen( s_CompString.GetBuffer() ) - 1 )
                                        s_CandList.awszCandidate[i][0] = 0;  // Doesn't exist
                                    else
                                    {
                                        s_CandList.awszCandidate[i][0] = s_CompString[i];
                                        s_CandList.awszCandidate[i][1] = 0;
                                    }
                                }
                                s_CandList.dwPageSize = MAX_CANDLIST;
                                // Clear comp string after we are done copying
                                ZeroMemory( (LPVOID)s_CompString.GetBuffer(), 4 * sizeof(WCHAR) );
                                s_bShowReadingWindow = true;
                                GetReadingWindowOrientation( 0 );
                                if( s_bHorizontalReading )
                                {
                                    s_CandList.nReadingError = -1;  // Clear error

                                    // Create a string that consists of the current
                                    // reading string.  Since horizontal reading window
                                    // is used, we take advantage of this by rendering
                                    // one string instead of several.
                                    //
                                    // Copy the reading string from the candidate list
                                    // to the reading string buffer.
                                    s_wszReadingString[0] = 0;
                                    for( UINT i = 0; i < s_CandList.dwCount; ++i )
                                    {
                                        if( s_CandList.dwSelection == i )
                                            s_CandList.nReadingError = wcslen( s_wszReadingString );
                                        wcsncat( s_wszReadingString, s_CandList.awszCandidate[i], 32 - lstrlenW( s_wszReadingString ) - 1 );
                                    }
                                }
                            }
                            else
                            {
                                s_CandList.dwCount = 0;
                                s_bShowReadingWindow = false;
                            }
                        }

                        // Get the caret position in composition string
                        s_nCompCaret = _ImmGetCompositionStringW( hImc, GCS_CURSORPOS, NULL, 0 );
                        if( s_nCompCaret < 0 )
                            s_nCompCaret = 0; // On error, set caret to pos 0.

                        if( s_bInsertOnType )
                        {
                            // Send composition string to the edit control via WM_CHAR
                            SendCompString();
                            // Restore the caret to the correct location.
                            // It's at the end right now, so compute the number
                            // of times left arrow should be pressed to
                            // send it to the original position.
                            int nCount = wcslen( s_CompString.GetBuffer() + s_nCompCaret );
                            // Send left keystrokes
                            for( int i = 0; i < nCount; ++i )
                                SendMessage( Engine_GetHWnd(), WM_KEYDOWN, VK_LEFT, 0 );
                            SendMessage( Engine_GetHWnd(), WM_KEYUP, VK_LEFT, 0 );
                        }
                    }

                    ResetCaretBlink();
                }

                // Retrieve clause information
                if( lParam & GCS_COMPCLAUSE )
                {
                    lRet = _ImmGetCompositionStringW(hImc, GCS_COMPCLAUSE, s_adwCompStringClause, sizeof( s_adwCompStringClause ) );
                    s_adwCompStringClause[lRet / sizeof(DWORD)] = 0;  // Terminate
                }

                _ImmReleaseContext( Engine_GetHWnd(), hImc );
            }
            break;

        case WM_IME_ENDCOMPOSITION:
            DXUTTRACE( L"WM_IME_ENDCOMPOSITION\n" );
            TruncateCompString();
            ResetCompositionString();
            // We can show the edit control's caret again.
            s_bHideCaret = false;
            // Hide reading window
            s_bShowReadingWindow = false;
            break;

        case WM_IME_NOTIFY:
            DXUTTRACE( L"WM_IME_NOTIFY %u\n", wParam );
            switch( wParam )
            {
                case IMN_SETCONVERSIONMODE:
                    DXUTTRACE( L"  IMN_SETCONVERSIONMODE\n" );
                case IMN_SETOPENSTATUS:
                    DXUTTRACE( L"  IMN_SETOPENSTATUS\n" );
                    CheckToggleState();
                    break;

                case IMN_OPENCANDIDATE:
                case IMN_CHANGECANDIDATE:
                {
                    DXUTTRACE( wParam == IMN_CHANGECANDIDATE ? L"  IMN_CHANGECANDIDATE\n" : L"  IMN_OPENCANDIDATE\n" );

                    s_CandList.bShowWindow = true;
                    *trapped = true;
                    if( NULL == ( hImc = _ImmGetContext( Engine_GetHWnd() ) ) )
                        break;

                    LPCANDIDATELIST lpCandList = NULL;
                    DWORD dwLenRequired;

                    s_bShowReadingWindow = false;
                    // Retrieve the candidate list
                    dwLenRequired = _ImmGetCandidateListW( hImc, 0, NULL, 0 );
                    if( dwLenRequired )
                    {
                        lpCandList = (LPCANDIDATELIST)HeapAlloc( GetProcessHeap(), 0, dwLenRequired );
                        dwLenRequired = _ImmGetCandidateListW( hImc, 0, lpCandList, dwLenRequired );
                    }

                    if( lpCandList )
                    {
                        // Update candidate list data
                        s_CandList.dwSelection = lpCandList->dwSelection;
                        s_CandList.dwCount = lpCandList->dwCount;

                        int nPageTopIndex = 0;
                        s_CandList.dwPageSize = min( lpCandList->dwPageSize, MAX_CANDLIST );
                        if( GetPrimaryLanguage() == LANG_JAPANESE )
                        {
                            // Japanese IME organizes its candidate list a little
                            // differently from the other IMEs.
                            nPageTopIndex = ( s_CandList.dwSelection / s_CandList.dwPageSize ) * s_CandList.dwPageSize;
                        }
                        else
                            nPageTopIndex = lpCandList->dwPageStart;

                        // Make selection index relative to first entry of page
                        s_CandList.dwSelection = ( GetLanguage() == LANG_CHS && !GetImeId() ) ? (DWORD)-1
                                                 : s_CandList.dwSelection - nPageTopIndex;

                        ZeroMemory( s_CandList.awszCandidate, sizeof(s_CandList.awszCandidate) );
                        for( UINT i = nPageTopIndex, j = 0;
                            (DWORD)i < lpCandList->dwCount && j < s_CandList.dwPageSize;
                            i++, j++ )
                        {
                            // Initialize the candidate list strings
                            LPWSTR pwsz = s_CandList.awszCandidate[j];
                            // For every candidate string entry,
                            // write [index] + Space + [String] if vertical,
                            // write [index] + [String] + Space if horizontal.
	                        *pwsz++ = (WCHAR)( L'0' + ( (j + 1) % 10 ) );  // Index displayed is 1 based
	                        if( s_bVerticalCand )
		                        *pwsz++ = L' ';
                            WCHAR *pwszNewCand = (LPWSTR)( (LPBYTE)lpCandList + lpCandList->dwOffset[i] );
	                        while ( *pwszNewCand )
		                        *pwsz++ = *pwszNewCand++;
	                        if( !s_bVerticalCand )
		                        *pwsz++ = L' ';
	                        *pwsz = 0;  // Terminate
                        }

                        // Make dwCount in s_CandList be number of valid entries in the page.
                        s_CandList.dwCount = lpCandList->dwCount - lpCandList->dwPageStart;
                        if( s_CandList.dwCount > lpCandList->dwPageSize )
                            s_CandList.dwCount = lpCandList->dwPageSize;

                        HeapFree( GetProcessHeap(), 0, lpCandList );
                        _ImmReleaseContext( Engine_GetHWnd(), hImc );

                        // Korean and old Chinese IME can't have selection.
                        // User must use the number hotkey or Enter to select
                        // a candidate.
                        if( GetPrimaryLanguage() == LANG_KOREAN ||
                            GetLanguage() == LANG_CHT && !GetImeId() )
                        {
                            s_CandList.dwSelection = (DWORD)-1;
                        }

                        // Initialize s_CandList.HoriCand if we have a
                        // horizontal candidate window.
                        if( !s_bVerticalCand )
                        {
                            WCHAR wszCand[256] = L"";

                            s_CandList.nFirstSelected = 0;
                            s_CandList.nHoriSelectedLen = 0;
                            for( UINT i = 0; i < MAX_CANDLIST; ++i )
                            {
                                if( s_CandList.awszCandidate[i][0] == L'\0' )
                                    break;

                                WCHAR wszEntry[32];
                                swprintf( wszEntry, L"%s ", s_CandList.awszCandidate[i] );
                                // If this is the selected entry, mark its char position.
                                if( s_CandList.dwSelection == i )
                                {
                                    s_CandList.nFirstSelected = wcslen( wszCand );
                                    s_CandList.nHoriSelectedLen = wcslen( wszEntry ) - 1;  // Minus space
                                }
                                lstrcat( wszCand, wszEntry );
                            }
                            wszCand[wcslen(wszCand) - 1] = L'\0';  // Remove the last space
                            s_CandList.HoriCand.SetText( wszCand );
                        }
                    }
                    break;
                }
                
                case IMN_CLOSECANDIDATE:
                {
                    DXUTTRACE( L"  IMN_CLOSECANDIDATE\n" );
                    s_CandList.bShowWindow = false;
                    if( !s_bShowReadingWindow )
                    {
                        s_CandList.dwCount = 0;
                        ZeroMemory( s_CandList.awszCandidate, sizeof(s_CandList.awszCandidate) );
                    }
                    *trapped = true;
                    break;
                }

                case IMN_PRIVATE:
                    DXUTTRACE( L"  IMN_PRIVATE\n" );
                    {
                        if( !s_CandList.bShowWindow )
                            GetPrivateReadingString();

                        // Trap some messages to hide reading window
                        DWORD dwId = GetImeId();
                        switch( dwId )
                        {
                            case IMEID_CHT_VER42:
                            case IMEID_CHT_VER43:
                            case IMEID_CHT_VER44:
                            case IMEID_CHS_VER41:
                            case IMEID_CHS_VER42:
                                if( ( lParam == 1 ) || ( lParam == 2 ) )
                                {
                                    *trapped = true;
                                }
                                break;

                            case IMEID_CHT_VER50:
                            case IMEID_CHT_VER51:
                            case IMEID_CHT_VER52:
                            case IMEID_CHT_VER60:
                            case IMEID_CHS_VER53:
                                if( (lParam == 16) || (lParam == 17) || (lParam == 26) || (lParam == 27) || (lParam == 28) )
                                {
                                    *trapped = true;
                                }
                                break;
                        }
                    }
                    break;

                default:
                    *trapped = true;
                    break;
            }
            break;

        // fix for #15386 - When Text Service Framework is installed in Win2K, Alt+Shift and Ctrl+Shift combination (to switch 
        // input locale / keyboard layout) doesn't send WM_KEYUP message for the key that is released first. We need to check
        // if these keys are actually up whenever we receive key up message for other keys.
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if ( !( lAlt & 0x80000000 ) && wParam != VK_MENU && ( GetAsyncKeyState( VK_MENU ) & 0x8000 ) == 0 )
            {
                PostMessageW( GetFocus(), WM_KEYUP, (WPARAM)VK_MENU, ( lAlt & 0x01ff0000 ) | 0xC0000001 );
            }   
            else if ( !( lCtrl & 0x80000000 ) && wParam != VK_CONTROL && ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) == 0 )
            {
                PostMessageW( GetFocus(), WM_KEYUP, (WPARAM)VK_CONTROL, ( lCtrl & 0x01ff0000 ) | 0xC0000001 );
            }
            else if ( !( lShift & 0x80000000 ) && wParam != VK_SHIFT && ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) == 0 )
            {
                PostMessageW( GetFocus(), WM_KEYUP, (WPARAM)VK_SHIFT, ( lShift & 0x01ff0000 ) | 0xC0000001 );
            }
            // fall through WM_KEYDOWN / WM_SYSKEYDOWN
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            switch ( wParam )
            {
                case VK_MENU:
                    lAlt = lParam;
                    break;
                case VK_SHIFT:
                    lShift = lParam;
                    break;
                case VK_CONTROL:
                    lCtrl = lParam;
                    break;
            }
            //break;
            // Fall through to default case
            // so we invoke the parent.

        default:
            // Let the parent handle the message that we
            // don't handle.
            return CGuiEditBox::MsgProc( uMsg, wParam, lParam );

    }  // switch

    return *trapped;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param fElapsedTime : √Ë ˆ
 * \param bReading : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiImeEditBox::RenderCandidateReadingWindow(float fElapsedTime, bool bReading )
{
    RECT rc;
    UINT nNumEntries = bReading ? 4 : MAX_CANDLIST;
    DWORD TextColor, TextBkColor, SelTextColor, SelBkColor;
    int nX, nXFirst, nXComp;
    m_Buffer.CPtoX( m_nCaret, FALSE, &nX );
    m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );

    if( bReading )
    {
        TextColor = m_ReadingColor;
        TextBkColor = m_ReadingWinColor;
        SelTextColor = m_ReadingSelColor;
        SelBkColor = m_ReadingSelBkColor;
    } else
    {
        TextColor = m_CandidateColor;
        TextBkColor = m_CandidateWinColor;
        SelTextColor = m_CandidateSelColor;
        SelBkColor = m_CandidateSelBkColor;
    }

    // For Japanese IME, align the window with the first target converted character.
    // For all other IMEs, align with the caret.  This is because the caret
    // does not move for Japanese IME.
    if ( GetLanguage() == LANG_CHT && !GetImeId() )
        nXComp = 0;
    else
    if( GetPrimaryLanguage() == LANG_JAPANESE )
        s_CompString.CPtoX( s_nFirstTargetConv, FALSE, &nXComp );
    else
        s_CompString.CPtoX( s_nCompCaret, FALSE, &nXComp );

    // Compute the size of the candidate window
    int nWidthRequired = 0;
    int nHeightRequired = 0;
    int nSingleLineHeight = 0;

    if( ( s_bVerticalCand && !bReading ) ||
        ( !s_bHorizontalReading && bReading ) )
    {
        // Vertical window
        for( UINT i = 0; i < nNumEntries; ++i )
        {
            if( s_CandList.awszCandidate[i][0] == L'\0' )
                break;
            SetRect( &rc, 0, 0, 0, 0 );
            m_pDialog->CalcTextRect( s_CandList.awszCandidate[i], m_Elements.GetAt( 1 ), &rc );
            nWidthRequired = max( nWidthRequired, rc.right - rc.left );
            nSingleLineHeight = max( nSingleLineHeight, rc.bottom - rc.top );
        }
        nHeightRequired = nSingleLineHeight * nNumEntries;
    } else
    {
        // Horizontal window
        SetRect( &rc, 0, 0, 0, 0 );
        if( bReading )
            m_pDialog->CalcTextRect( s_wszReadingString, m_Elements.GetAt( 1 ), &rc );
        else
            m_pDialog->CalcTextRect( s_CandList.HoriCand.GetBuffer(), m_Elements.GetAt( 1 ), &rc );
        nWidthRequired = rc.right - rc.left;
        nSingleLineHeight = nHeightRequired = rc.bottom - rc.top;
    }

    // Now that we have the dimension, calculate the location for the candidate window.
    // We attempt to fit the window in this order:
    // bottom, top, right, left.

    bool bHasPosition = false;

    // Bottom
    SetRect( &rc, s_ptCompString.x + nXComp, s_ptCompString.y + m_rcText.bottom - m_rcText.top,
                  s_ptCompString.x + nXComp + nWidthRequired, s_ptCompString.y + m_rcText.bottom - m_rcText.top + nHeightRequired );
    // if the right edge is cut off, move it left.
    if( rc.right > m_pDialog->GetWidth() )
    {
        rc.left -= rc.right - m_pDialog->GetWidth();
        rc.right = m_pDialog->GetWidth();
    }
    if( rc.bottom <= m_pDialog->GetHeight() )
        bHasPosition = true;

    // Top
    if( !bHasPosition )
    {
        SetRect( &rc, s_ptCompString.x + nXComp, s_ptCompString.y - nHeightRequired,
                      s_ptCompString.x + nXComp + nWidthRequired, s_ptCompString.y );
        // if the right edge is cut off, move it left.
        if( rc.right > m_pDialog->GetWidth() )
        {
            rc.left -= rc.right - m_pDialog->GetWidth();
            rc.right = m_pDialog->GetWidth();
        }
        if( rc.top >= 0 )
            bHasPosition = true;
    }

    // Right
    if( !bHasPosition )
    {
        int nXCompTrail;
        s_CompString.CPtoX( s_nCompCaret, TRUE, &nXCompTrail );
        SetRect( &rc, s_ptCompString.x + nXCompTrail, 0,
                      s_ptCompString.x + nXCompTrail + nWidthRequired, nHeightRequired );
        if( rc.right <= m_pDialog->GetWidth() )
            bHasPosition = true;
    }

    // Left
    if( !bHasPosition )
    {
        SetRect( &rc, s_ptCompString.x + nXComp - nWidthRequired, 0,
                      s_ptCompString.x + nXComp, nHeightRequired );
        if( rc.right >= 0 )
            bHasPosition = true;
    }

    if( !bHasPosition )
    {
        // The dialog is too small for the candidate window.
        // Fall back to render at 0, 0.  Some part of the window
        // will be cut off.
        rc.left = 0;
        rc.right = nWidthRequired;
    }

    // If we are rendering the candidate window, save the position
    // so that mouse clicks are checked properly.
    if( !bReading )
        s_CandList.rcCandidate = rc;

    // Render the elements
    m_pDialog->DrawRect( &rc, TextBkColor );
    if( ( s_bVerticalCand && !bReading ) ||
        ( !s_bHorizontalReading && bReading ) )
    {
        // Vertical candidate window
        for( UINT i = 0; i < nNumEntries; ++i )
        {
            // Here we are rendering one line at a time
            rc.bottom = rc.top + nSingleLineHeight;
            // Use a different color for the selected string
            if( s_CandList.dwSelection == i )
            {
                m_pDialog->DrawRect( &rc, SelBkColor );
                m_Elements.GetAt( 1 )->FontColor.Current = SelTextColor;
            } else
                m_Elements.GetAt( 1 )->FontColor.Current = TextColor;

            m_pDialog->DrawText( s_CandList.awszCandidate[i], m_Elements.GetAt( 1 ), &rc );

            rc.top += nSingleLineHeight;
        }
    } else
    {
        // Horizontal candidate window
        m_Elements.GetAt( 1 )->FontColor.Current = TextColor;
        if( bReading )
            m_pDialog->DrawText( s_wszReadingString, m_Elements.GetAt( 1 ), &rc );
        else
            m_pDialog->DrawText( s_CandList.HoriCand.GetBuffer(), m_Elements.GetAt( 1 ), &rc );

        // Render the selected entry differently
        if( !bReading )
        {
            int nXLeft, nXRight;
            s_CandList.HoriCand.CPtoX( s_CandList.nFirstSelected, FALSE, &nXLeft );
            s_CandList.HoriCand.CPtoX( s_CandList.nFirstSelected + s_CandList.nHoriSelectedLen, FALSE, &nXRight );

            rc.right = rc.left + nXRight;
            rc.left += nXLeft;
            m_pDialog->DrawRect( &rc, SelBkColor );
            m_Elements.GetAt( 1 )->FontColor.Current = SelTextColor;
            m_pDialog->DrawText( s_CandList.HoriCand.GetBuffer() + s_CandList.nFirstSelected,
                                m_Elements.GetAt( 1 ), &rc, false, s_CandList.nHoriSelectedLen );
        }
    }
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param fElapsedTime : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiImeEditBox::RenderComposition( float fElapsedTime )
{
    RECT rcCaret = { 0, 0, 0, 0 };
    int nX, nXFirst;
    m_Buffer.CPtoX( m_nCaret, FALSE, &nX );
    m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );
    CGuiElement *pElement = m_Elements.GetAt( 1 );

    // Get the required width
    RECT rc = { m_rcText.left + nX - nXFirst, m_rcText.top,
                m_rcText.left + nX - nXFirst, m_rcText.bottom };
    m_pDialog->CalcTextRect( s_CompString.GetBuffer(), pElement, &rc );

    // If the composition string is too long to fit within
    // the text area, move it to below the current line.
    // This matches the behavior of the default IME.
    if( rc.right > m_rcText.right )
        OffsetRect( &rc, m_rcText.left - rc.left, rc.bottom - rc.top );

    // Save the rectangle position for processing highlighted text.
    RECT rcFirst = rc;

    // Update s_ptCompString for RenderCandidateReadingWindow().
    s_ptCompString.x = rc.left; s_ptCompString.y = rc.top;

    DWORD TextColor = m_CompColor;
    // Render the window and string.
    // If the string is too long, we must wrap the line.
    pElement->FontColor.Current = TextColor;
    const WCHAR *pwszComp = s_CompString.GetBuffer();
    int nCharLeft = s_CompString.GetTextSize();
    for( ; ; )
    {
        // Find the last character that can be drawn on the same line.
        int nLastInLine;
        int bTrail;
        s_CompString.XtoCP( m_rcText.right - rc.left, &nLastInLine, &bTrail );
        int nNumCharToDraw = min( nCharLeft, nLastInLine );
        m_pDialog->CalcTextRect( pwszComp, pElement, &rc, nNumCharToDraw );

        // Draw the background
        // For Korean IME, blink the composition window background as if it
        // is a cursor.
        if( GetPrimaryLanguage() == LANG_KOREAN )
        {
            if( m_bCaretOn )
            {
                m_pDialog->DrawRect( &rc, m_CompWinColor );
            }
            else
            {
                // Not drawing composition string background. We
                // use the editbox's text color for composition
                // string text.
                TextColor = m_Elements.GetAt(0)->FontColor.States[GUI_STATE_NORMAL];
            }
        } else
        {
            // Non-Korean IME. Always draw composition background.
            m_pDialog->DrawRect( &rc, m_CompWinColor );
        }

        // Draw the text
        pElement->FontColor.Current = TextColor;
        m_pDialog->DrawText( pwszComp, pElement, &rc, false, nNumCharToDraw );

        // Advance pointer and counter
        nCharLeft -= nNumCharToDraw;
        pwszComp += nNumCharToDraw;
        if( nCharLeft <= 0 )
            break;

        // Advance rectangle coordinates to beginning of next line
        OffsetRect( &rc, m_rcText.left - rc.left, rc.bottom - rc.top );
    }

    // Load the rect for the first line again.
    rc = rcFirst;

    // Inspect each character in the comp string.
    // For target-converted and target-non-converted characters,
    // we display a different background color so they appear highlighted.
    int nCharFirst = 0;
    nXFirst = 0;
    s_nFirstTargetConv = -1;
    BYTE *pAttr;
    const WCHAR *pcComp;
    for( pcComp = s_CompString.GetBuffer(), pAttr = s_abCompStringAttr;
          *pcComp != L'\0'; ++pcComp, ++pAttr )
    {
        DWORD bkColor;

        // Render a different background for this character
        int nXLeft, nXRight;
        s_CompString.CPtoX( int(pcComp - s_CompString.GetBuffer()), FALSE, &nXLeft );
        s_CompString.CPtoX( int(pcComp - s_CompString.GetBuffer()), TRUE, &nXRight );

        // Check if this character is off the right edge and should
        // be wrapped to the next line.
        if( nXRight - nXFirst > m_rcText.right - rc.left )
        {
            // Advance rectangle coordinates to beginning of next line
            OffsetRect( &rc, m_rcText.left - rc.left, rc.bottom - rc.top );

            // Update the line's first character information
            nCharFirst = int(pcComp - s_CompString.GetBuffer());
            s_CompString.CPtoX( nCharFirst, FALSE, &nXFirst );
        }

        // If the caret is on this character, save the coordinates
        // for drawing the caret later.
        if( s_nCompCaret == int(pcComp - s_CompString.GetBuffer()) )
        {
            rcCaret = rc;
            rcCaret.left += nXLeft - nXFirst - 1;
            rcCaret.right = rcCaret.left + 2;
        }

        // Set up color based on the character attribute
        if( *pAttr == ATTR_TARGET_CONVERTED )
        {
            pElement->FontColor.Current = m_CompTargetColor;
            bkColor = m_CompTargetBkColor;
        }
        else
        if( *pAttr == ATTR_TARGET_NOTCONVERTED )
        {
            pElement->FontColor.Current = m_CompTargetNonColor;
            bkColor = m_CompTargetNonBkColor;
        }
        else
        {
            continue;
        }

        RECT rcTarget = { rc.left + nXLeft - nXFirst, rc.top, rc.left + nXRight - nXFirst, rc.bottom };
        m_pDialog->DrawRect( &rcTarget, bkColor );
        m_pDialog->DrawText( pcComp, pElement, &rcTarget, false, 1 );

        // Record the first target converted character's index
        if( -1 == s_nFirstTargetConv )
            s_nFirstTargetConv = int(pAttr - s_abCompStringAttr);
    }

    // Render the composition caret
    if( m_bCaretOn )
    {
        // If the caret is at the very end, its position would not have
        // been computed in the above loop.  We compute it here.
        if( s_nCompCaret == s_CompString.GetTextSize() )
        {
            s_CompString.CPtoX( s_nCompCaret, FALSE, &nX );
            rcCaret = rc;
            rcCaret.left += nX - nXFirst - 1;
            rcCaret.right = rcCaret.left + 2;
        }

        m_pDialog->DrawRect( &rcCaret, m_CompCaretColor );
    }
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param fElapsedTime : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiImeEditBox::RenderIndicator( float fElapsedTime )
{
    CGuiElement *pElement = m_Elements.GetAt( 9 );
    pElement->TextureColor.Blend( GUI_STATE_NORMAL, fElapsedTime );

    m_pDialog->DrawSprite( pElement, &m_rcIndicator );
    RECT rc = m_rcIndicator;
    InflateRect( &rc, -m_nSpacing, -m_nSpacing );
    pElement->FontColor.Current = s_ImeState == IMEUI_STATE_ON && s_bEnableImeSystem ? m_IndicatorImeColor : m_IndicatorEngColor;
    RECT rcCalc = { 0, 0, 0, 0 };
    // If IME system is off, draw English indicator.
    WCHAR *pwszIndicator = s_bEnableImeSystem ? s_wszCurrIndicator : s_aszIndicator[0];

    m_pDialog->CalcTextRect( pwszIndicator, pElement, &rcCalc );
    m_pDialog->DrawText( pwszIndicator, pElement, &rc );
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param fElapsedTime : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiImeEditBox::Render( float fElapsedTime )
{
    if( m_bVisible == false )
        return;

    // If we have not computed the indicator symbol width,
    // do it.
    if( !m_nIndicatorWidth )
    {
        for( int i = 0; i < 5; ++i )
        {
            RECT rc = { 0, 0, 0, 0 };
            m_pDialog->CalcTextRect( s_aszIndicator[i], m_Elements.GetAt( 9 ), &rc );
            m_nIndicatorWidth = max( m_nIndicatorWidth, rc.right - rc.left );
        }
        // Update the rectangles now that we have the indicator's width
        UpdateRects();
    }

    // Let the parent render first (edit control)
    CGuiEditBox::Render( pd3dDevice, fElapsedTime );

    CGuiElement* pElement = GetElement( 1 );
    if( pElement )
    {
        s_CompString.SetFontIndex( pElement->iFont );
        s_CandList.HoriCand.SetFontIndex( pElement->iFont );
    }

    //
    // Now render the IME elements
    //
    if( m_bHasFocus )
    {
        // Render the input locale indicator
        RenderIndicator( fElapsedTime );

        // Display the composition string.
        // This method should also update s_ptCompString
        // for RenderCandidateReadingWindow.
        RenderComposition( fElapsedTime );

        // Display the reading/candidate window. RenderCandidateReadingWindow()
        // uses s_ptCompString to position itself.  s_ptCompString must have
        // been filled in by RenderComposition().
        if( s_bShowReadingWindow )
            // Reading window
            RenderCandidateReadingWindow( fElapsedTime, true );
        else
        if( s_CandList.bShowWindow )
            // Candidate list window
            RenderCandidateReadingWindow( fElapsedTime, false );
    }
}



/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiImeEditBox::InitializeImm()
{
    FARPROC Temp;

    WCHAR wszPath[MAX_PATH+1];
    if( !::GetSystemDirectory( wszPath, MAX_PATH+1 ) )
        return;
    lstrcatW( wszPath, IMM32_DLLNAME );
    s_hDllImm32 = LoadLibrary( wszPath );
    if( s_hDllImm32 )
    {
        GETPROCADDRESS( s_hDllImm32, ImmLockIMC, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmUnlockIMC, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmLockIMCC, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmUnlockIMCC, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmDisableTextFrameService, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetCompositionStringW, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetCandidateListW, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetContext, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmReleaseContext, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmAssociateContext, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetOpenStatus, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmSetOpenStatus, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetConversionStatus, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetDefaultIMEWnd, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetIMEFileNameA, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmGetVirtualKey, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmNotifyIME, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmSetConversionStatus, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmSimulateHotKey, Temp );
        GETPROCADDRESS( s_hDllImm32, ImmIsIME, Temp );
    }

    if( !::GetSystemDirectory( wszPath, MAX_PATH+1 ) )
        return;
    lstrcatW( wszPath, VER_DLLNAME );
    s_hDllVer = LoadLibrary( wszPath );
    if( s_hDllVer )
    {
        GETPROCADDRESS( s_hDllVer, VerQueryValueA, Temp );
        GETPROCADDRESS( s_hDllVer, GetFileVersionInfoA, Temp );
        GETPROCADDRESS( s_hDllVer, GetFileVersionInfoSizeA, Temp );
    }
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiImeEditBox::UninitializeImm()
{
    if( s_hDllImm32 )
    {
        PLACEHOLDERPROC( ImmLockIMC );
        PLACEHOLDERPROC( ImmUnlockIMC );
        PLACEHOLDERPROC( ImmLockIMCC );
        PLACEHOLDERPROC( ImmUnlockIMCC );
        PLACEHOLDERPROC( ImmDisableTextFrameService );
        PLACEHOLDERPROC( ImmGetCompositionStringW );
        PLACEHOLDERPROC( ImmGetCandidateListW );
        PLACEHOLDERPROC( ImmGetContext );
        PLACEHOLDERPROC( ImmReleaseContext );
        PLACEHOLDERPROC( ImmAssociateContext );
        PLACEHOLDERPROC( ImmGetOpenStatus );
        PLACEHOLDERPROC( ImmSetOpenStatus );
        PLACEHOLDERPROC( ImmGetConversionStatus );
        PLACEHOLDERPROC( ImmGetDefaultIMEWnd );
        PLACEHOLDERPROC( ImmGetIMEFileNameA );
        PLACEHOLDERPROC( ImmGetVirtualKey );
        PLACEHOLDERPROC( ImmNotifyIME );
        PLACEHOLDERPROC( ImmSetConversionStatus );
        PLACEHOLDERPROC( ImmSimulateHotKey );
        PLACEHOLDERPROC( ImmIsIME );

        FreeLibrary( s_hDllImm32 );
        s_hDllImm32 = NULL;
    }
    if( s_hDllIme )
    {
        PLACEHOLDERPROC( GetReadingString );
        PLACEHOLDERPROC( ShowReadingWindow );

        FreeLibrary( s_hDllIme );
        s_hDllIme = NULL;
    }
    if( s_hDllVer )
    {
        PLACEHOLDERPROC( VerQueryValueA );
        PLACEHOLDERPROC( GetFileVersionInfoA );
        PLACEHOLDERPROC( GetFileVersionInfoSizeA );

        FreeLibrary( s_hDllVer );
        s_hDllVer = NULL;
    }
}


