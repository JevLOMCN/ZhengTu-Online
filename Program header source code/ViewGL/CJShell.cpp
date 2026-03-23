// CJShell.cpp : implementation file
// Copyright ?1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Based upon the MFCENUM example. MFCENUM is a companion sample application 
// for Chapter 14 in "Programming the Windows 95 User Interface" by Nancy Cluts,
// available in the Microsoft(R) Development Library.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/24/99 12:01a $
 * $Revision: 10 $
 * $Archive: /CodeJock/CJLibrary/CJShell.cpp $
 *
 * $History: CJShell.cpp $
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/CJLibrary
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 10:25a
 * Updated in $/CodeJock/CJLibrary
 * Added four new shell methods which deal with PIDL's including a new
 * version of an Explorer context menu called DoExplorerMenu(...) which
 * can be used in place of the existing DoTheMenuThing(...) method.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 9/13/99    Time: 5:52a
 * Updated in $/CodeJockey/CJLibrary
 * Fixed build errors which occured with UNICODE builds.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 4:39a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/31/99    Time: 4:26p
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 6/11/99    Time: 10:06p
 * Updated in $/CodeJockey/CJ60Lib
 * Renamed class from CCJShellFile to CCJShell.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 3:38p
 * Updated in $/CodeJockey/CJ60Lib
 * Added version macro  _WIN32_IE, you will need to have at IE4 or greater
 * installed to use this class. This will ensure that the version of
 * SHELL32.DLL is compatible.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 3/19/99    Time: 11:01p
 * Created in $/CodeJockey/CJ60Lib
 * Initial re-write and release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CJShell.h"
#if _WIN32_IE >= 0x0400 // Need to have at IE4 or greater installed.

/////////////////////////////////////////////////////////////////////////////
// CCJShell
 
CCJShell::CCJShell(LPCTSTR lpszFileName)
{
	m_strFileName = lpszFileName;
	SplitPath(m_strFileName, m_szDrive, m_szDir, m_szFname, m_szExt);
}

CCJShell::~CCJShell()
{
}

int CALLBACK CCJShell::BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData)
{
	CCJShell* pCJShellInfo = (CCJShell*)lpData;
	
	if (pCJShellInfo->m_strSelDir.IsEmpty())
		return FALSE;

	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE,
			(LPARAM)(LPCTSTR)pCJShellInfo->m_strSelDir);
		break;
	
	case BFFM_SELCHANGED:
		break;
	}
	
	return FALSE;
}

BOOL CCJShell::BrowseForFolder(CWnd *pParentWnd)
{
	ASSERT(pParentWnd);

	LPMALLOC pMalloc;
	if (::SHGetMalloc(&pMalloc)!= NOERROR)
		return FALSE;
	
	BROWSEINFO bInfo;
	LPITEMIDLIST pidl;
	ZeroMemory ( (PVOID) &bInfo,sizeof (BROWSEINFO));
	
	if (!m_strInitDir.IsEmpty ())
	{
		OLECHAR       olePath[MAX_PATH];
		ULONG         chEaten;
		ULONG         dwAttributes;
		HRESULT       hr;
		LPSHELLFOLDER pDesktopFolder;
		// 
		// Get a pointer to the Desktop's IShellFolder interface. 
		//
		if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
		{
			//
			// IShellFolder::ParseDisplayName requires the file name be in Unicode.
			//
#if defined( _UNICODE )
			_tcsncpy( olePath, m_strInitDir, MAX_PATH );
#else
			MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, m_strInitDir,
								 -1, olePath, MAX_PATH);
#endif
			//
			// Convert the path to an ITEMIDLIST.
			//
			hr = pDesktopFolder->ParseDisplayName(NULL,
				NULL,
				olePath,
				&chEaten,
				&pidl,
				&dwAttributes);
			if (FAILED(hr))
			{
				pMalloc ->Free (pidl);
				pMalloc ->Release ();
				// fix potential memory leak - KStowell 10-22-99
				pDesktopFolder ->Release ();
				return FALSE;
			}
			bInfo.pidlRoot = pidl;
			// fix potential memory leak - KStowell 10-22-99
			pDesktopFolder ->Release ();
		}
	}
	
	bInfo.hwndOwner		 = pParentWnd->GetSafeHwnd();
	bInfo.pszDisplayName = m_strPath.GetBuffer (MAX_PATH);
	bInfo.lpszTitle		 = (m_strTitle.IsEmpty()) ? _T("Open"):m_strTitle;
	bInfo.ulFlags		 = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS;
	bInfo.lpfn			 = BrowseCtrlCallback;  // address of callback function
	bInfo.lParam		 = (LPARAM)this;		// pass address of object to callback function
	
	if ((pidl = ::SHBrowseForFolder(&bInfo)) == NULL)
	{
		return FALSE;
	}
	m_strPath.ReleaseBuffer();
	m_iImageIndex = bInfo.iImage;
	
	if (::SHGetPathFromIDList(pidl,m_strPath.GetBuffer(MAX_PATH)) == FALSE)
	{
		pMalloc ->Free(pidl);
		pMalloc ->Release();
		return FALSE;
	}
	
	m_strPath.ReleaseBuffer();
	
	pMalloc ->Free(pidl);
	pMalloc ->Release();
	
	return TRUE;
}

void CCJShell::GetSystemImageList(CImageList *pSmallList, CImageList *pLargeList)
{
	ASSERT(pSmallList);
	ASSERT(pLargeList);

	//image list setup
	SHFILEINFO  ssfi, lsfi;
    
	// Get a handle to the system small icon list 
	HIMAGELIST hSystemSmallImageList = (HIMAGELIST)
		::SHGetFileInfo((LPCTSTR)_T("C:\\"), 0, &ssfi,
		sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON); 

	// Attach it to the small image list 
	// DON'T FORGET TO PUT pSmallList->Detach(); in your destructor
 	pSmallList->Attach(hSystemSmallImageList);

	// Get a handle to the system large icon list 
	HIMAGELIST hSystemLargeImageList =	(HIMAGELIST)
		::SHGetFileInfo((LPCTSTR)_T("C:\\"), 0,	&lsfi,
		sizeof(SHFILEINFO),	SHGFI_SYSICONINDEX | SHGFI_ICON); 

	// Attach it to the large image list 
	// DON'T FORGET TO PUT pLargeList->Detach(); in your destructor
 	pLargeList->Attach(hSystemLargeImageList); 
}

CString CCJShell::GetFileName(LPCTSTR lpszFileName/*=NULL*/)
{
	// Parse the file path.
	if (lpszFileName != NULL) {
		SplitPath(lpszFileName, m_szDrive, m_szDir, m_szFname, m_szExt);
	}

	// Just return the file name + extension.
	CString str; str.Format(_T("%s%s"), m_szFname, m_szExt);
	return str;
}

CString CCJShell::GetFileDrive(LPCTSTR lpszFileName/*=NULL*/)
{
	// Parse the file path.
	if (lpszFileName != NULL) {
		SplitPath(lpszFileName, m_szDrive, m_szDir, m_szFname, m_szExt);
	}

	return m_szDrive;
}

CString CCJShell::GetRoot(LPCTSTR lpszFileName/*=NULL*/)
{
	// Parse the file path.
	if (lpszFileName != NULL) {
		SplitPath(lpszFileName, m_szDrive, m_szDir, m_szFname, m_szExt);
	}

	// Just return the file name + extension.
	CString str; str.Format(_T("%s%s"), m_szDrive, m_szDir);
	return str;
}

CString CCJShell::GetFileTitle(LPCTSTR lpszFileName/*=NULL*/)
{
	// Parse the file path.
	if (lpszFileName != NULL) {
		SplitPath(lpszFileName, m_szDrive, m_szDir, m_szFname, m_szExt);
	}

	return m_szFname;
}

CString CCJShell::GetFileExt(LPCTSTR lpszFileName/*=NULL*/)
{
	// Parse the file path.
	if (lpszFileName != NULL) {
		SplitPath(lpszFileName, m_szDrive, m_szDir, m_szFname, m_szExt);
	}

	return m_szExt;
}

CString CCJShell::GetFileType(LPCTSTR lpszFileName/*=NULL*/)
{
	SHFILEINFO	sfi;

	::SHGetFileInfo((lpszFileName==NULL)?m_strFileName:lpszFileName,
		0, &sfi, sizeof(SHFILEINFO), SHGFI_TYPENAME);

   return sfi.szTypeName;
}

CString CCJShell::GetDisplayName(LPCTSTR lpszFileName/*=NULL*/)
{
	SHFILEINFO sfi;

	::SHGetFileInfo((lpszFileName==NULL)?m_strFileName:lpszFileName,
		0, &sfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);

	return sfi.szDisplayName;
}

int CCJShell::GetIconIndex(LPCTSTR lpszFileName/*=NULL*/, UINT uFlags/*=SHGFI_SMALLICON*/)
{
	SHFILEINFO sfi;

	::SHGetFileInfo((lpszFileName==NULL)?m_strFileName:lpszFileName,
		0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | uFlags);

	return sfi.iIcon;
}

BOOL CCJShell::Exist(LPCTSTR lpszFileName/*=NULL*/)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile( (lpszFileName==NULL)?m_strFileName:lpszFileName, &fd );

	if ( hFind != INVALID_HANDLE_VALUE )
		FindClose( hFind );

	return (hFind != INVALID_HANDLE_VALUE);
}

CString CCJShell::GetLastWriteTime(CTime *pTime)
{
	if (!pTime)
		return _T("");

	CString strTime;

	// Format the date time string.
	strTime.Format( _T("%.2d/%.2d/%d %.2d:%.2d"), pTime->GetMonth(), pTime->GetDay(),
		pTime->GetYear(), pTime->GetHour(), pTime->GetSecond());

	return strTime;
}

CString CCJShell::GetFileSizeString(LPCTSTR lpszFileSize)
{
	CString strFileSize(lpszFileSize);

	UINT nSize = _ttoi(strFileSize);
	if (nSize == 0) return _T("0KB");

	nSize /= 1000;
	if (nSize == 0) return _T("1KB");
	
	strFileSize.Format(_T("%dKB"), nSize);
	return strFileSize;
}

// 
// FUNCTIONS THAT DEAL WITH PIDLs
//

LPITEMIDLIST CCJShell::Next(LPCITEMIDLIST pidl)
{
   LPSTR lpMem=(LPSTR)pidl;

   lpMem+=pidl->mkid.cb;

   return (LPITEMIDLIST)lpMem;
}

UINT CCJShell::GetSize(LPCITEMIDLIST pidl)
{
    UINT cbTotal = 0;
    if (pidl)
    {
        cbTotal += sizeof(pidl->mkid.cb);       // Null terminator
        while (pidl->mkid.cb)
        {
            cbTotal += pidl->mkid.cb;
            pidl = Next(pidl);
        }
    }

    return cbTotal;
}

LPITEMIDLIST CCJShell::Create(UINT cbSize)
{
    LPMALLOC lpMalloc;
    HRESULT  hr;
    LPITEMIDLIST pidl=NULL;

    hr=SHGetMalloc(&lpMalloc);

    if (FAILED(hr))
       return 0;

    pidl=(LPITEMIDLIST)lpMalloc->Alloc(cbSize);

    if (pidl)
        memset(pidl, 0, cbSize);      // zero-init for external task   alloc

    if (lpMalloc) lpMalloc->Release();

    return pidl;
}

LPITEMIDLIST CCJShell::ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
    LPITEMIDLIST pidlNew;
    UINT cb1;
    UINT cb2;

    if (pidl1)  //May be NULL
       cb1 = GetSize(pidl1) - sizeof(pidl1->mkid.cb);
    else
       cb1 = 0;

    cb2 = GetSize(pidl2);

    pidlNew = Create(cb1 + cb2);
    if (pidlNew)
    {
        if (pidl1)
           memcpy(pidlNew, pidl1, cb1);
        memcpy(((LPSTR)pidlNew) + cb1, pidl2, cb2);
    }
    return pidlNew;
}

LPITEMIDLIST CCJShell::CopyITEMID(LPMALLOC lpMalloc, LPITEMIDLIST lpi)
{
   LPITEMIDLIST lpiTemp;

   lpiTemp=(LPITEMIDLIST)lpMalloc->Alloc(lpi->mkid.cb+sizeof(lpi->mkid.cb));
   CopyMemory((PVOID)lpiTemp, (CONST VOID *)lpi, lpi->mkid.cb+sizeof(lpi->mkid.cb));

   return lpiTemp;
}

BOOL CCJShell::GetName( LPSHELLFOLDER lpsf, LPITEMIDLIST lpi, DWORD dwFlags, LPTSTR lpFriendlyName )
{
   BOOL   bSuccess=TRUE;
   STRRET str;

   if (NOERROR==lpsf->GetDisplayNameOf(lpi,dwFlags, &str))
   {
      switch (str.uType)
      {
         case STRRET_WSTR:
#if defined( _UNICODE )
			_tcscpy( lpFriendlyName, str.pOleStr );
#else
            WideCharToMultiByte(CP_ACP,                 // CodePage
                                0,		               // dwFlags
                                str.pOleStr,            // lpWideCharStr
                                -1,                     // cchWideChar
                                lpFriendlyName,         // lpMultiByteStr
								MAX_PATH,
                                //sizeof(lpFriendlyName), // cchMultiByte, wrong. sizeof on a pointer, psk, psk
                                NULL,                   // lpDefaultChar,
                                NULL);                  // lpUsedDefaultChar
#endif
             break;

         case STRRET_OFFSET:
#if defined( _UNICODE )
			MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, (LPSTR)lpi+str.uOffset,
				-1, lpFriendlyName, MAX_PATH);
#else
             _tcscpy(lpFriendlyName, (LPSTR)lpi+str.uOffset);
#endif
            break;

         case STRRET_CSTR:
#if defined( _UNICODE )
			MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, str.cStr,
				-1, lpFriendlyName, MAX_PATH);
#else
             _tcscpy(lpFriendlyName, (LPSTR)str.cStr);
#endif
             break;

         default:
             bSuccess = FALSE;
             break;
      }
   }
   else
      bSuccess = FALSE;

   return bSuccess;
}

LPITEMIDLIST CCJShell::GetFullyQualPidl(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi)
{
   TCHAR szBuff[MAX_PATH];
   OLECHAR szOleChar[MAX_PATH];
   LPSHELLFOLDER lpsfDeskTop;
   LPITEMIDLIST  lpifq;
   ULONG ulEaten, ulAttribs;
   HRESULT hr;

	if( GetName(lpsf, lpi, SHGDN_FORPARSING, szBuff) == FALSE ) {
		return NULL;
	}

   hr=SHGetDesktopFolder(&lpsfDeskTop);

   if (FAILED(hr))
      return NULL;

#if defined( _UNICODE )
   hr=lpsfDeskTop->ParseDisplayName(NULL, NULL, szOleChar, &ulEaten, 
		&lpifq, &ulAttribs );
#else
   MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szBuff,
		-1, szOleChar, sizeof(szOleChar));

   hr=lpsfDeskTop->ParseDisplayName(NULL, NULL, szOleChar, &ulEaten, 
		&lpifq, &ulAttribs );
#endif

   lpsfDeskTop->Release();

   if (FAILED(hr))
      return NULL;

   return lpifq;
}

BOOL CCJShell::DoTheMenuThing(HWND hwnd, LPSHELLFOLDER lpsfParent,
     LPITEMIDLIST  lpi, LPPOINT lppt)
{
    LPCONTEXTMENU lpcm;
    HRESULT       hr;
    TCHAR          szTemp[64];
    CMINVOKECOMMANDINFO cmi;
    int                 idCmd;
    HMENU               hMenu;
    BOOL                bSuccess=TRUE;

    hr=lpsfParent->GetUIObjectOf(hwnd,
        1,  //Number of objects to get attributes of
        (const struct _ITEMIDLIST **)&lpi,
        IID_IContextMenu,
        0,
        (LPVOID *)&lpcm);
    if (SUCCEEDED(hr))  
    {
       hMenu = CreatePopupMenu();

       if (hMenu)
       {
          hr=lpcm->QueryContextMenu(hMenu, 0, 1, 0x7fff, CMF_EXPLORE);
          if (SUCCEEDED(hr))
          {
             idCmd=TrackPopupMenu(hMenu, 
                TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, 
                lppt->x, lppt->y, 0, hwnd, NULL);

             if (idCmd)
             {
                cmi.cbSize		 = sizeof(CMINVOKECOMMANDINFO);
                cmi.fMask		 = 0;
                cmi.hwnd		 = hwnd;
                cmi.lpVerb		 = (LPCSTR)MAKEINTRESOURCE(idCmd-1);
                cmi.lpParameters = NULL;
      	        cmi.lpDirectory  = NULL;
                cmi.nShow        = SW_SHOWNORMAL;
                cmi.dwHotKey     = 0;
                cmi.hIcon        = NULL;
                
				hr=lpcm->InvokeCommand(&cmi);
                
				if (!SUCCEEDED(hr))
                {
                   wsprintf(szTemp, _T("InvokeCommand failed. hr=%lx"), hr);
                   AfxMessageBox(szTemp);
                }
				else
				{
					::SendMessage(hwnd, WM_SHELL_NOTIFY,
						NM_SH_SHELLMENU, (LPARAM)0);
				}
             }

          }
          else
             bSuccess = FALSE;

          DestroyMenu(hMenu);
       }
       else
          bSuccess = FALSE;

       lpcm->Release();
    } 
    else
    {
       wsprintf(szTemp, _T("GetUIObjectOf failed! hr=%lx"), hr);
       AfxMessageBox(szTemp );
       bSuccess = FALSE;
    }
    return bSuccess;
}

int CCJShell::GetIcon(LPITEMIDLIST lpi, UINT uFlags)
{
	SHFILEINFO    sfi;

	SHGetFileInfo((LPCTSTR)lpi, 0, &sfi, 
		sizeof(SHFILEINFO), uFlags);

	return sfi.iIcon;
}

void CCJShell::GetNormalAndSelectedIcons(LPITEMIDLIST lpifq, LPTV_ITEM lptvitem)
{
   //Note that we don't check the return value here because if GetIcon()
   //fails, then we're in big trouble...

   lptvitem->iImage = GetIcon(lpifq, SHGFI_PIDL | 
                              SHGFI_SYSICONINDEX | 
                              SHGFI_SMALLICON);
   
   lptvitem->iSelectedImage = GetIcon(lpifq, SHGFI_PIDL | 
                                      SHGFI_SYSICONINDEX | 
                                      SHGFI_SMALLICON |
                                      SHGFI_OPENICON);
   
   return;
}

LONG CCJShell::GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
    HKEY hkey;
    LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);

    if (retval == ERROR_SUCCESS) {
        long datasize = MAX_PATH;
        TCHAR data[MAX_PATH];
        RegQueryValue(hkey, NULL, data, &datasize);
        _tcscpy(retdata,data);
        RegCloseKey(hkey);
    }

    return retval;
}

HINSTANCE CCJShell::ShellExec(LPCTSTR lpszExec, int nShow)
{
    TCHAR key[MAX_PATH + MAX_PATH];

    // First try ShellExecute()
    HINSTANCE result = ShellExecute(NULL, _T("open"), lpszExec, NULL,NULL, nShow);

    // If it failed, get the .htm regkey and lookup the program
    if ((UINT)result <= HINSTANCE_ERROR) {

        if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS) {
            lstrcat(key, _T("\\shell\\open\\command"));

            if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS) {
                TCHAR *pos;
                pos = _tcsstr(key, _T("\"%1\""));
                if (pos == NULL) {                     // No quotes found
                    pos = _tcsstr(key, _T("%1"));       // Check for %1, without quotes 
                    if (pos == NULL)                   // No parameter at all...
                        pos = key+_tcslen(key)-1;
                    else
                        *pos = _T('\0');                   // Remove the parameter
                }
                else
                    *pos = _T('\0');                       // Remove the parameter

                lstrcat(pos, _T(" "));
                lstrcat(pos, lpszExec);
				PROCESS_INFORMATION pi;
		
				STARTUPINFO			si;

				::ZeroMemory( &si, sizeof( si )); 
		
				si.cb = sizeof( si ); 

				if( ::CreateProcess( NULL,
									 pos,
									 NULL,
									 NULL,
									 FALSE,
									 0,
									 NULL,
									 NULL,
									 &si,
									&pi ) == FALSE )
				{
					return NULL;
				}

                result = (HINSTANCE) pi.hProcess;
            }
        }
    }

    return result;
}

void CCJShell::SplitPath(CString strFileName, char* szDrive, char* szDir, char* szFile, char* szExt)
{
#if defined( _UNICODE )
	char szFileName[ MAX_PATH ];
	::WideCharToMultiByte( CP_ACP, 0, strFileName, -1,
		szFileName, MAX_PATH, NULL, NULL );
	_splitpath( szFileName, szDrive, szDir, szFile, szExt );
#else
	_splitpath( strFileName, szDrive, szDir, szFile, szExt );
#endif
}

/////////////////////////////////////////////////////////////////////////////
// New Context menu functions
/////////////////////////////////////////////////////////////////////////////

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  FUNCTION:       DuplicateItem
//
//  DESCRIPTION:    Makes a copy of the next item in an item ID list.
//
//  INPUT:          pMalloc = Pointer to an IMalloc interface.
//                  pidl    = Pointer to an item ID list.
//
//  RETURNS:        Pointer to an ITEMIDLIST containing the copied item ID.
//
//  NOTES:          It is the caller's responsibility to free the memory
//                  allocated by this function when the item ID is no longer
//                  needed. Example:
//
//                    pidlItem = DuplicateItem (pMalloc, pidl);
//                      .
//                      .
//                      .
//                    pMalloc->Free (pMalloc, pidlItem);
//
//                  Failure to free the ITEMIDLIST will result in memory
//                  leaks.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LPITEMIDLIST CCJShell::DuplicateItem (LPMALLOC pMalloc, LPITEMIDLIST pidl)
{
    USHORT nLen;
    LPITEMIDLIST pidlNew;
	
    nLen = pidl->mkid.cb;
    if (nLen == 0)
        return NULL;
	
    pidlNew = (LPITEMIDLIST) pMalloc->Alloc (nLen + sizeof (USHORT));
    if (pidlNew == NULL)
        return NULL;
	
    CopyMemory (pidlNew, pidl, nLen);
    *((USHORT*) (((LPBYTE) pidlNew) + nLen)) = 0;
	
    return pidlNew;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  FUNCTION:       GetNextItem
//
//  DESCRIPTION:    Finds the next item in an item ID list.
//
//  INPUT:          pidl = Pointer to an item ID list.
//
//  RETURNS:        Pointer to the next item.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LPITEMIDLIST CCJShell::GetNextItem (LPITEMIDLIST pidl)
{
    USHORT nLen;
	
    if ((nLen = pidl->mkid.cb) == 0)
        return NULL;
    
    return (LPITEMIDLIST) (((LPBYTE) pidl) + nLen);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  FUNCTION:       GetItemCount
//
//  DESCRIPTION:    Computes the number of item IDs in an item ID list.
//
//  INPUT:          pidl = Pointer to an item ID list.
//
//  RETURNS:        Number of item IDs in the list.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

UINT CCJShell::GetItemCount (LPITEMIDLIST pidl)
{
    USHORT nLen;
    UINT nCount;
	
    nCount = 0;
    while ((nLen = pidl->mkid.cb) != 0) {
        pidl = GetNextItem (pidl);
        nCount++;
    }
    return nCount;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  FUNCTION:       DoExplorerMenu
//
//  DESCRIPTION:    Given a path name to a file or folder object, displays
//                  the shell's context menu for that object and executes
//                  the menu command (if any) selected by the user.
//
//  INPUT:          hwnd    = Handle of the window in which the menu will be
//                            displayed.
//
//                  pszPath = Pointer to an ANSI or Unicode string
//                            specifying the path to the object.
//
//                  point   = x and y coordinates of the point where the
//                            menu's upper left corner should be located, in
//                            client coordinates relative to hwnd.
//  
//  RETURNS:        TRUE if successful, FALSE if not.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BOOL CCJShell::DoExplorerMenu (HWND hwnd, LPCTSTR pszPath, POINT point)
{
    LPMALLOC pMalloc;
    LPSHELLFOLDER psfFolder, psfNextFolder;
    LPITEMIDLIST pidlMain, pidlItem, pidlNextItem, *ppidl;
    LPCONTEXTMENU pContextMenu;
    CMINVOKECOMMANDINFO ici;
    ULONG ulCount, ulAttr;
    TCHAR tchPath[MAX_PATH];
    WCHAR wchPath[MAX_PATH];    
    UINT nCount, nCmd;
    BOOL bResult;
    HMENU hMenu;
	
    //
    // Make sure the file name is fully qualified and in Unicode format.
    //
    GetFullPathName (pszPath, sizeof (tchPath) / sizeof (TCHAR), tchPath, NULL);
	
#if defined( _UNICODE )
	_tcscpy( wchPath, tchPath );
#else
	MultiByteToWideChar (CP_ACP, 0, pszPath, -1, wchPath,
		sizeof (wchPath) / sizeof (WCHAR));
#endif
	
    //
    // Get pointers to the shell's IMalloc interface and the desktop's
    // IShellFolder interface.
    //
    bResult = FALSE;
	
    if (!SUCCEEDED (SHGetMalloc (&pMalloc)))
        return bResult;
	
    if (!SUCCEEDED (SHGetDesktopFolder (&psfFolder))) {
        pMalloc->Release();
        return bResult;
    }
	
    //
    // Convert the path name into a pointer to an item ID list (pidl).
    //
    if (SUCCEEDED (psfFolder->ParseDisplayName (hwnd,
        NULL, wchPath, &ulCount, &pidlMain, &ulAttr)) && (pidlMain != NULL))
	{
		nCount = GetItemCount (pidlMain);
		
        if (nCount) // nCount must be > 0
		{
            //
            // Initialize psfFolder with a pointer to the IShellFolder
            // interface of the folder that contains the item whose context
            // menu we're after, and initialize pidlItem with a pointer to
            // the item's item ID. If nCount > 1, this requires us to walk
            // the list of item IDs stored in pidlMain and bind to each
            // subfolder referenced in the list.
            //
            pidlItem = pidlMain;
			
            while (nCount)
			{
				--nCount;

                //
                // Create a 1-item item ID list for the next item in pidlMain.
                //
                pidlNextItem = DuplicateItem (pMalloc, pidlItem);
                if (pidlNextItem == NULL) {
                    pMalloc->Free (pidlMain);
                    psfFolder->Release();
                    pMalloc->Release();
                    return bResult;
                }
				
                //
                // Bind to the folder specified in the new item ID list.
                //
                if (!SUCCEEDED (psfFolder->BindToObject (
                    pidlNextItem, NULL, IID_IShellFolder, (LPVOID*)&psfNextFolder))) {
                    pMalloc->Free (pidlNextItem);
                    pMalloc->Free (pidlMain);
                    psfFolder->Release ();
                    pMalloc->Release ();
                    return bResult;
                }
				
                //
                // Release the IShellFolder pointer to the parent folder
                // and set psfFolder equal to the IShellFolder pointer for
                // the current folder.
                //
                psfFolder->Release ();
                psfFolder = psfNextFolder;
				
                //
                // Release the storage for the 1-item item ID list we created
                // just a moment ago and initialize pidlItem so that it points
                // to the next item in pidlMain.
                //
                pMalloc->Free (pidlNextItem);
                pidlItem = GetNextItem (pidlItem);
            }
			
            //
            // Get a pointer to the item's IContextMenu interface and call
            // IContextMenu::QueryContextMenu to initialize a context menu.
            //
            ppidl = &pidlItem;
            if (SUCCEEDED (psfFolder->GetUIObjectOf (
                hwnd, 1, (const struct _ITEMIDLIST **)ppidl, IID_IContextMenu, NULL, (LPVOID*)&pContextMenu))) {
				
                hMenu = CreatePopupMenu ();
                if (SUCCEEDED (pContextMenu->QueryContextMenu (
                    hMenu, 0, 1, 0x7FFF, CMF_EXPLORE))) {
					
                    //
                    // Display the context menu.
                    //
                    nCmd = TrackPopupMenu (hMenu, TPM_LEFTALIGN |
                        TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                        point.x, point.y, 0, hwnd, NULL);
					
                    //
                    // If a command was selected from the menu, execute it.
                    //
                    if (nCmd) {
                        ici.cbSize          = sizeof (CMINVOKECOMMANDINFO);
                        ici.fMask           = 0;
                        ici.hwnd            = hwnd;
                        ici.lpVerb          = (LPCSTR)MAKEINTRESOURCE (nCmd - 1);
                        ici.lpParameters    = NULL;
                        ici.lpDirectory     = NULL;
                        ici.nShow           = SW_SHOWNORMAL;
                        ici.dwHotKey        = 0;
                        ici.hIcon           = NULL;
						
                        if (SUCCEEDED (
                            pContextMenu->InvokeCommand (&ici)))
                            bResult = TRUE;
                    }
                }
                DestroyMenu (hMenu);
                pContextMenu->Release ();
            }
        }
        pMalloc->Free (pidlMain);
    }
		
    //
    // Clean up and return.
    //
    psfFolder->Release ();
    pMalloc->Release ();
	
    return bResult;
}

#endif // (_WIN32_IE >= 0x0400)
