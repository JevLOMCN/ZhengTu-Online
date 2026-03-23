/********************************************************************
	创建时间:	2004/10/31   15:14
	文件名: 	File.h
	文件类型:	h
	作者:		$$$03公司
	
	用途:	  文件IO 接口
*********************************************************************/

// File.h: interface for the File class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILE_H__AD2303E9_8172_4830_A4CF_B372F296AE94__INCLUDED_)
#define AFX_FILE_H__AD2303E9_8172_4830_A4CF_B372F296AE94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
//#include "stringtable.h"

void forwardslash(TCHAR *str);
void backslash(TCHAR *str);

/*
bool dFileDelete(const TCHAR * name);
bool dFileTouch(const TCHAR * name);
long compareFileTimes(const FileTime &a, const FileTime &b);
// 找出目录下的所有由 pattern 指定类型的文件
bool recurseDumpPath(const TCHAR *path, const TCHAR *pattern, std::vector<FileInfo> &fileVector);
bool getFileTimes(const TCHAR *filePath, FileTime *createTime, FileTime *modifyTime);
bool createPath(const TCHAR *file);
bool cdFileExists(const TCHAR *filePath, const TCHAR *volumeName, DWORD serialNum);
bool dumpPath(const TCHAR *path, std::vector<FileInfo> &fileVector);
StringTableEntry getWorkingDirectory();
bool isFile(const TCHAR *pFilePath);
bool isDirectory(const TCHAR *pDirPath);
bool isSubDirectory(const TCHAR *pParent, const TCHAR *pDir);
StringTableEntry getExeDirectory();
const TCHAR* getAbsPath(const TCHAR* pszFileName);
//*/

class ENGINE_API File  
{
public:
	/// What is the status of our file handle?
	enum Status
	{
		Ok = 0,           ///< Ok!
		IOError,          ///< Read or Write error
		EOS,              ///< End of Stream reached (mostly for reads)
		IllegalCall,      ///< An unsupported operation used.  Always accompanied by AssertWarn
		Closed,           ///< Tried to operate on a closed stream (or detached filter)
		UnknownError      ///< Catchall
	};
	
	/// How are we accessing the file?
	enum AccessMode
	{
		Read         = 0,  ///< Open for read only, starting at beginning of file.
		Write        = 1,  ///< Open for write only, starting at beginning of file; will blast old contents of file.
		ReadWrite    = 2,  ///< Open for read-write.
		WriteAppend  = 3   ///< Write-only, starting at end of file.
	};
	
	/// Flags used to indicate what we can do to the file.
	enum Capability
	{
		FileRead         = 1<<0,
		FileWrite        = 1<<1
	};
private:
	HANDLE handle;           ///< Pointer to the file handle.
	Status currentStatus;   ///< Current status of the file (Ok, IOError, etc.).
	DWORD capability;         ///< Keeps track of file capabilities.
protected:
	Status setStatus();                 ///< Called after error encountered.
	Status setStatus(Status status);    ///< Setter for the current status.
public:
	/// Gets the status of the file
	bool hasCapability(Capability cap) const;
	File::Status read(void *dst,DWORD size,  DWORD *bytesRead = NULL);
	Status write(const void *src,DWORD size,  DWORD *bytesWritten = NULL);
	Status getStatus() const;
	Status close();
	Status open(const TCHAR *filename, const AccessMode openMode,bool randomAccess = false);
	Status setPosition(DWORD position, bool absolutePos = true);
	DWORD   getPosition() const;
	DWORD   getSize() const;
	Status flush();
	File();
	~File();
	void* getHandle() { return handle; }
};

bool dFileDelete(const TCHAR * name);

bool dFileTouch(const TCHAR * name);

#endif // !defined(AFX_FILE_H__AD2303E9_8172_4830_A4CF_B372F296AE94__INCLUDED_)
