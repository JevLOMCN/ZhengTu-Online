// CJShell.h : header file
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
 * $Date: 10/26/99 10:46p $
 * $Revision: 13 $
 * $Archive: /CodeJock/Include/CJShell.h $
 *
 * $History: CJShell.h $
 * 
 * *****************  Version 13  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:46p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/Include
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 10:25a
 * Updated in $/CodeJock/Include
 * Added four new shell methods which deal with PIDL's including a new
 * version of an Explorer context menu called DoExplorerMenu(...) which
 * can be used in place of the existing DoTheMenuThing(...) method.
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 9/13/99    Time: 5:52a
 * Updated in $/CodeJockey/Include
 * Fixed build errors which occured with UNICODE builds.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:14p
 * Updated in $/CodeJockey/Include
 * Added Unicode compliance, thanks to Barry Burton for his help with
 * this.
 * 
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:13p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 6/11/99    Time: 10:06p
 * Updated in $/CodeJockey/Include
 * Renamed class from CCJShellFile to CCJShell.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 3:38p
 * Updated in $/CodeJockey/Include
 * Added version macro  _WIN32_IE, you will need to have at IE4 or greater
 * installed to use this class. This will ensure that the version of
 * SHELL32.DLL is compatible.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 3/19/99    Time: 11:01p
 * Created in $/CodeJockey/Include
 * Initial re-write and release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJSHELL_H__
#define __CJSHELL_H__

#if _WIN32_IE >= 0x0400 // Need to have at IE4 or greater installed.

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <shlobj.h>

#define WM_SHELL_NOTIFY		WM_USER + 1
#define NM_SH_SHELLMENU		1

// helper struct that holds list item data
typedef struct tagLVID
{
   LPSHELLFOLDER lpsfParent;
   LPITEMIDLIST  lpi;
   ULONG         ulAttribs;
} LVITEMDATA, *LPLVITEMDATA;

// helper struct that holds tree item data
typedef struct tagTVID
{
   LPSHELLFOLDER lpsfParent;
   LPITEMIDLIST  lpi;
   LPITEMIDLIST  lpifq;
} TVITEMDATA, *LPTVITEMDATA;

// CCJShell class is used to perform shell file operations
class  CCJShell
{

public:
	
	// Default constructor
	//
	CCJShell(LPCTSTR lpszFileName=NULL);

	// Virtual destructor
	//
	virtual ~CCJShell();

public:

    int			m_iImageIndex;	// index into system image list
    CString		m_strPath;		// full file path used with BrowseForFolder(...)
    CString		m_strInitDir;	// start directory used with BrowseForFolder(...)
    CString		m_strSelDir;	// selected directory used with BrowseForFolder(...)
    CString		m_strTitle;		// dialog title used with BrowseForFolder(...)

protected:

	CString		m_strFileName;			// contains the full path to a file or folder
	char		m_szDrive[_MAX_DRIVE];	// contains the drive letter
	char		m_szDir[_MAX_DIR];		// contains the directory string
	char		m_szFname[_MAX_FNAME];	// contains the file name minus path and ext
	char		m_szExt[_MAX_EXT];		// contains the file extension

public:
	virtual void SplitPath(CString strFileName, char* szDrive, char* szDir, char* szFile, char* szExt);

	// this member function is used to retrieve a key from the registry
	//
	virtual LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata);

	// this member funciont is used to perform execute command
	//
	virtual HINSTANCE ShellExec(LPCTSTR lpszExec, int nShow);

	// this member function returns the size of the file formatted in KB.
	//
	virtual CString GetFileSizeString(
		// a string containing the system file size
		LPCTSTR lpszFileSize);

	// this member function returns a formatted string for the last 
	// time file was written to.
	//
	virtual CString GetLastWriteTime(
		// points to a CTime object
		CTime* pTime);

	// retrieves the path only of the current filename. if the filename 
	// is "c:\incoming\hello.txt", this function returns "c:"
	//
	virtual CString GetFileDrive(
		// full path of file name to parse
		LPCTSTR lpszFileName=NULL);

	// this member function determines whether a file or directory exists.
	//
	virtual BOOL Exist(
		// full path of file or directory name
		LPCTSTR lpszFileName=NULL);

	// this member function returns the icon index of the file or folder
	//
	virtual int	GetIconIndex(
		// full path of file or directory name.
		LPCTSTR lpszFileName=NULL,
		// can be large or small icon
		UINT uFlags=SHGFI_SMALLICON);

	// this member function returns the system display name for the file.
	//
	virtual CString GetDisplayName(
		// full path of file or directory name.
		LPCTSTR lpszFileName=NULL);

	// this member function returns the system description for the file.
	//
	virtual CString GetFileType(
		// full path of file or directory name.
		LPCTSTR lpszFileName=NULL);

	// this member function retrieves the file extension. if the filename is 
	// "c:\incoming\hello.txt", this function returns "txt".
	//
	virtual CString GetFileExt(
		// full path of file or directory name.
		LPCTSTR lpszFileName=NULL);

	// this member function retrieves the title of the filename excluding 
	// the path and extension. if the filename is "c:\incoming\hello.txt",
	// this function returns "hello".
	//
	virtual CString GetFileTitle(
		// full path of file or directory name.
		LPCTSTR lpszFileName=NULL);

	// this member function retrieves the path only of the current filename. 
	// if the filename is "c:\incoming\hello.txt", this function returns "c:\incoming\".
	//
	virtual CString GetRoot(
		// full path of file or directory name.
		LPCTSTR lpszFileName=NULL);

	// this member function retrieves current filename minus the path if the 
	// filename is "c:\incoming\hello.txt", this function returns "hello.txt".
	//
	virtual CString GetFileName(
		// full path of file or directory name.
		LPCTSTR lpszFileName=NULL);

	// this member function returns the system image list for small and large icons.
	// if you use this method, you must call CImageList::Detach() for both
	// image lists
	//
	virtual void GetSystemImageList(
		// points to a CImageList object that contains small icons
		CImageList* pSmallList,
		// points to a CImageList object that contains large icons
		CImageList* pLargeList);

	// this member function displays browse for folder dialog.
	//
	virtual BOOL BrowseForFolder(
		// points to the parent window
		CWnd* pParentWnd);

	// callback function for the BrowseForFolder method.
	//
	static int CALLBACK BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

// FUNCTIONS THAT DEAL WITH PIDLs

	// this member function gets the next PIDL in the list
	//
	virtual LPITEMIDLIST Next(LPCITEMIDLIST pidl);

	// this member function gets the size of the PIDL
	//
	virtual UINT GetSize(LPCITEMIDLIST pidl);

	// this member function allocates a PIDL
	//
	virtual LPITEMIDLIST Create(UINT cbSize);

	// this member function concatenates two PIDLs
	//
	virtual LPITEMIDLIST ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);

	// this member function copies the ITEMID
	//
	virtual LPITEMIDLIST CopyITEMID(LPMALLOC lpMalloc, LPITEMIDLIST lpi);

	// this member function gets the friendly name for the folder
	//
	virtual BOOL GetName(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi, DWORD dwFlags, LPTSTR lpFriendlyName);

	// this member function gets the Fully qualified Pidls for the folder 
	//
	virtual LPITEMIDLIST GetFullyQualPidl(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi);

	// this member function Displays a popup context menu, given a parent shell folder,
	// relative item id and screen location. returns TRUE on success, FALSE on failure
	//
	virtual BOOL DoTheMenuThing(
		// parent window handle
		HWND hwnd,
		// pointer to parent shell folder
		LPSHELLFOLDER lpsfParent,
		// pointer to item id that is relative to lpsfParent
		LPITEMIDLIST lpi,
		// screen location of where to popup the menu
		LPPOINT lppt);

	// this member function gets the index for the current icon. Index is 
	// index into system image list. returns icon index for current item.
	//
	virtual int	GetIcon(
		// fully qualified item id list for current item.
		LPITEMIDLIST lpi,
		// flags for SHGetFileInfo()
		UINT uFlags);

	// this member function gets the index for the normal and 
	// selected icons for the current item.
	//
	virtual void GetNormalAndSelectedIcons(
		// fully qualified item id list for current item.
		LPITEMIDLIST lpifq,
		// pointer to treeview item we are about to add to the tree.
		LPTV_ITEM lptvitem);

	// Makes a copy of the next item in an item ID list. Returns pointer to 
	// an ITEMIDLIST containing the copied item ID.
	//
	virtual LPITEMIDLIST DuplicateItem (
		// Pointer to an IMalloc interface.
		LPMALLOC pMalloc,
		// Pointer to an item ID list.
		LPITEMIDLIST pidl);

	// Finds the next item in an item ID list. Returns pointer to the next item.
	//
	virtual LPITEMIDLIST GetNextItem (
		// Pointer to an item ID list.
		LPITEMIDLIST pidl);

	// Computes the number of item IDs in an item ID list. Returns number 
	// of item IDs in the list.
	virtual UINT GetItemCount (
		// Pointer to an item ID list.
		LPITEMIDLIST pidl);

	// Given a path name to a file or folder object, displays the shell's context 
	// menu for that object and executes the menu command (if any) selected by the user.
	//
	virtual BOOL DoExplorerMenu (
		// Handle of the window in which the menu will be displayed.
		HWND hwnd,
		// Pointer to an ANSI or Unicode string specifying the path to the object.
		LPCTSTR pszPath,
		// x and y coordinates of the point where the menu's upper left corner should be 
		// located, in client coordinates relative to hwnd.
		POINT point);
};

/////////////////////////////////////////////////////////////////////////////

#endif // _WIN32_IE >= 0x0400
#endif // __CJSHELL_H__

