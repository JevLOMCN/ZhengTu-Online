/**
 * \file      winFileio.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     WIN32Æ½Ì¨ÉÏµÄÎÄ¼şIO ½Ó¿Ú
 * 
 *	      WIN32Æ½Ì¨ÉÏµÄÎÄ¼şIO ½Ó¿Ú
 */

// winFileio.cpp: implementation of the File class.
//
//////////////////////////////////////////////////////////////////////
#include "../include/engine.h"
#include "../include/File.h"
#include <vector>
#include <time.h>
#include <sys/utime.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param str : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-------------------------------------- Helper Functions
void forwardslash(char *str)
{
	while(*str)
	{
		if(*str == '\\')
			*str = '/';
		str++;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param str : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void backslash(char *str)
{
	while(*str)
	{
		if(*str == '/')
			*str = '\\';
		str++;
	}
}
///*
//
///**
// * \brief ¼ò¶ÌÃèÊö
// * 
// * ÏêÏ¸ÃèÊö
// * 
// * \param name : ÃèÊö
// * \return ·µ»ØÖµµÄÃèÊö
// */
////-----------------------------------------------------------------------------
//bool dFileDelete(const char * name)
//{
//	return(::DeleteFile(name));
//}
//
///**
// * \brief ¼ò¶ÌÃèÊö
// * 
// * ÏêÏ¸ÃèÊö
// * 
// * \param name : ÃèÊö
// * \return ·µ»ØÖµµÄÃèÊö
// */
//bool dFileTouch(const char * name)
//{
//	// change the modified time to the current time (0byte WriteFile fails!)
//	return(utime(name, 0) != -1);
//}
//
///**
// * \brief ¼ò¶ÌÃèÊö
// * 
// * ÏêÏ¸ÃèÊö
// * 
// * \param a : ÃèÊö
// * \param b : ÃèÊö
// * \return ·µ»ØÖµµÄÃèÊö
// */
//long compareFileTimes(const FileTime &a, const FileTime &b)
//{
//   if(a.v2 > b.v2)
//      return 1;
//   if(a.v2 < b.v2)
//      return -1;
//   if(a.v1 > b.v1)
//      return 1;
//   if(a.v1 < b.v1)
//      return -1;
//   return 0;
//}
//
///**
// * \brief ¼ò¶ÌÃèÊö
// * 
// * ÏêÏ¸ÃèÊö
// * 
// * \param path : ÃèÊö
// * \param pattern : ÃèÊö
// * \param fileVector : ÃèÊö
// * \return ·µ»ØÖµµÄÃèÊö
// */
//// ÕÒ³öÄ¿Â¼ÏÂµÄËùÓĞÓÉ pattern Ö¸¶¨ÀàĞÍµÄÎÄ¼ş
//bool recurseDumpPath(const char *path, const char *pattern, std::vector<FileInfo> &fileVector)
//{
//   String search;
//   String child;
//
//   WIN32_FIND_DATA findData;
//
//   search.Format("%s/%s", path, pattern);
//   
//   HANDLE handle = FindFirstFile(search, &findData);
//   if (handle == INVALID_HANDLE_VALUE)
//      return false;
//
//   do
//   {
//      if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//      {
//         // make sure it is a directory
//         if (findData.dwFileAttributes & (FILE_ATTRIBUTE_OFFLINE|FILE_ATTRIBUTE_SYSTEM) )                             
//            continue;
//
//         // skip . and .. directories
//         if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
//            continue;
//		
//		 child.Format("%s/%s",path,findData.cFileName);
//         recurseDumpPath(child, pattern, fileVector);
//      }      
//      else
//      {
//         // make sure it is the kind of file we're looking for
//         if (findData.dwFileAttributes & 
//             (FILE_ATTRIBUTE_DIRECTORY|                                      
//              FILE_ATTRIBUTE_OFFLINE|
//              FILE_ATTRIBUTE_SYSTEM|
//              FILE_ATTRIBUTE_TEMPORARY) )                             
//            continue;
//         
//         // add it to the list
//         fileVector.push_back(FileInfo());
//         FileInfo& rInfo = fileVector.back();
//			
//         rInfo.pFullPath = StringTable->insert(path);
//         rInfo.pFileName = StringTable->insert(findData.cFileName);
//         rInfo.fileSize  = findData.nFileSizeLow;
//		 //TRACE("----->file:%s\\%s\n",path_upr,findData.cFileName);
//      }
//
//   }while(FindNextFile(handle, &findData));
//
//   FindClose(handle);
//   return true;
//}   
//
//
////--------------------------------------
//
///**
// * \brief ¼ò¶ÌÃèÊö
// * 
// * ÏêÏ¸ÃèÊö
// * 
// * \param filePath : ÃèÊö
// * \param createTime : ÃèÊö
// * \param modifyTime : ÃèÊö
// * \return ·µ»ØÖµµÄÃèÊö
// */
//bool getFileTimes(const char *filePath, FileTime *createTime, FileTime *modifyTime)
//{
//   WIN32_FIND_DATA findData;
//   HANDLE h = FindFirstFile(filePath, &findData);
//   if(h == INVALID_HANDLE_VALUE)
//      return false;
//
//   if(createTime)
//   {   
//      createTime->v1 = findData.ftCreationTime.dwLowDateTime;
//      createTime->v2 = findData.ftCreationTime.dwHighDateTime;
//   }
//   if(modifyTime)
//   {
//      modifyTime->v1 = findData.ftLastWriteTime.dwLowDateTime;
//      modifyTime->v2 = findData.ftLastWriteTime.dwHighDateTime;
//   }
//   FindClose(h);
//   return true;
//}
//
///**
// * \brief ¼ò¶ÌÃèÊö
// * 
// * ÏêÏ¸ÃèÊö
// * 
// * \param file : ÃèÊö
// * \return ·µ»ØÖµµÄÃèÊö
// */
////--------------------------------------
//bool createPath(const char *file)
//{
//   char pathbuf[1024];
//   const char *dir;
//   pathbuf[0] = 0;
//   DWORD pathLen = 0;
//   
//   while((dir = strchr(file, '/')) != NULL)
//   {
//      strncpy(pathbuf + pathLen, file, dir - file);
//      pathbuf[pathLen + dir-file] = 0;
//      bool ret = CreateDirectory(pathbuf, NULL);
//      pathLen += dir - file;
//      pathbuf[pathLen++] = '/';
//      file = dir + 1;
//   }
//   return true;
//}
//
///**
// * \brief ¼ò¶ÌÃèÊö
// * 
// * ÏêÏ¸ÃèÊö
// * 
// * \param filePath : ÃèÊö
// * \param volumeName : ÃèÊö
// * \param serialNum : ÃèÊö
// * \return ·µ»ØÖµµÄÃèÊö
// */
//bool cdFileExists(const char *filePath, const char *volumeName, DWORD serialNum)
//{
//   if (!filePath || !filePath[0])
//      return true;
//
//   //first find the CD device...
//   char fileBuf[256];
//   char drivesBuf[256];
//   long length = GetLogicalDriveStrings(256, drivesBuf);
//   char *drivePtr = drivesBuf;
//   while (long(drivePtr - drivesBuf) < length)
//   {
//      char driveVolume[256], driveFileSystem[256];
//      DWORD driveSerial, driveFNLength, driveFlags;
//      if ((stricmp(drivePtr, "A:\\") != 0 && stricmp(drivePtr, "B:\\") != 0) &&
//          GetVolumeInformation((const char*)drivePtr, &driveVolume[0], (unsigned long)255,
//                               (unsigned long*)&driveSerial, (unsigned long*)&driveFNLength,
//                               (unsigned long*)&driveFlags, &driveFileSystem[0], (unsigned long)255))
//      {
//#if defined (_DEBUG) || defined (INTERNAL_RELEASE)
//         TRACELN("Found Drive: %s, vol: %s, serial: %d", drivePtr, driveVolume, driveSerial);
//#endif
//         //see if the volume and serial number match
//         if (!stricmp(volumeName, driveVolume) && (!serialNum || (serialNum == driveSerial)))
//         {
//            //see if the file exists on this volume
//            if(strlen(drivePtr) == 3 && drivePtr[2] == '\\' && filePath[0] == '\\')
//               _snprintf(fileBuf, sizeof(fileBuf), "%s%s", drivePtr, filePath + 1);
//            else
//               _snprintf(fileBuf, sizeof(fileBuf), "%s%s", drivePtr, filePath);
//#if defined (_DEBUG) || defined (INTERNAL_RELEASE)
//            TRACELN("Looking for file: %s on %s", fileBuf, driveVolume);
//#endif
//            WIN32_FIND_DATA findData;
//            HANDLE h = FindFirstFile(fileBuf, &findData);
//            if(h != INVALID_HANDLE_VALUE)
//            {
//               FindClose(h);
//               return true;
//            }
//            FindClose(h);
//         }
//      }
//
//      //check the next drive
//      drivePtr += strlen(drivePtr) + 1;
//   }
//
//   return false;
//}
//
///**
// * \brief ¼ò¶ÌÃèÊö
// * 
// * ÏêÏ¸ÃèÊö
// * 
// * \param path : ÃèÊö
// * \param fileVector : ÃèÊö
// * \return ·µ»ØÖµµÄÃèÊö
// */
////--------------------------------------
//bool dumpPath(const char *path, std::vector<FileInfo> &fileVector)
//{
//   return recurseDumpPath(path, "*", fileVector);
//}
//
//
///**
// * \brief ¼ò¶ÌÃèÊö
// * 
// * ÏêÏ¸ÃèÊö
// * 
// * \return ·µ»ØÖµµÄÃèÊö
// */
////--------------------------------------
//StringTableEntry getWorkingDirectory()
//{
//   static StringTableEntry cwd = NULL;
//   if (!cwd)
//   {
//      String cwd_buf;
//      GetCurrentDirectory(MAX_PATH, cwd_buf.GetBuffer(MAX_PATH));
//      forwardslash(cwd_buf.GetBuffer(0));
//	  cwd_buf.ReleaseBuffer();
//      cwd = StringTable->insert(cwd_buf);
//   }
//   return cwd;
//}
//
///**
// * \brief ¼ò¶ÌÃèÊö
// * 
// * ÏêÏ¸ÃèÊö
// * 
// * \return ·µ»ØÖµµÄÃèÊö
// */
//StringTableEntry getExeDirectory()
//{
//	static StringTableEntry cwd = NULL;
//	if (!cwd)
//	{
//		String cwd_buf;
//		GetModuleFileName(NULL,cwd_buf.GetBuffer(MAX_PATH),MAX_PATH);
//		cwd_buf.ReleaseBuffer();
//		long i = cwd_buf.ReverseFind('\\');
//		if(i != -1) cwd_buf.Delete(i,cwd_buf.GetLength()-i);
//		cwd = StringTable->insert(cwd_buf);
//	}
//	return cwd;
//}
//
///**
// * \brief ¼ò¶ÌÃèÊö
// * 
// * ÏêÏ¸ÃèÊö
// * 
// * \param pszFileName : ÃèÊö
// * \return ·µ»ØÖµµÄÃèÊö
// */
//const char* getAbsPath(const char* pszFileName)
//{
//	static String strPath;
//	strPath.Format("%s\\%s",getExeDirectory(),pszFileName);
//	return strPath;
//}
//
///**
// * \brief ¼ò¶ÌÃèÊö
// * 
// * ÏêÏ¸ÃèÊö
// * 
// * \param pFilePath : ÃèÊö
// * \return ·µ»ØÖµµÄÃèÊö
// */
////--------------------------------------
//bool isFile(const char *pFilePath)
//{
//   if (!pFilePath || !*pFilePath) 
//      return false;
//
//   // Get file info 
//   WIN32_FIND_DATA findData;
//   HANDLE handle = FindFirstFile(pFilePath, &findData);
//   FindClose(handle);
//
//   if(handle == INVALID_HANDLE_VALUE)
//      return false;
//
//   // if the file is a Directory, Offline, System or Temporary then false
//   if (findData.dwFileAttributes & 
//       (FILE_ATTRIBUTE_DIRECTORY|                                      
//        FILE_ATTRIBUTE_OFFLINE|
//        FILE_ATTRIBUTE_SYSTEM|
//        FILE_ATTRIBUTE_TEMPORARY) )                             
//      return false;
//
//   // must be a real file then
//   return true;
//}
//
//
///**
// * \brief ¼ò¶ÌÃèÊö
// * 
// * ÏêÏ¸ÃèÊö
// * 
// * \param pDirPath : ÃèÊö
// * \return ·µ»ØÖµµÄÃèÊö
// */
////--------------------------------------
//bool isDirectory(const char *pDirPath)
//{
//   if (!pDirPath || !*pDirPath) 
//      return false;
//
//   // Get file info 
//   WIN32_FIND_DATA findData;
//   HANDLE handle = FindFirstFile(pDirPath, &findData);
//   FindClose(handle);
//
//   if(handle == INVALID_HANDLE_VALUE)
//      return false;
//   
//   // if the file is a Directory, Offline, System or Temporary then false
//   if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//   {
//      // make sure it's a valid game directory
//      if (findData.dwFileAttributes & (FILE_ATTRIBUTE_OFFLINE|FILE_ATTRIBUTE_SYSTEM) )                             
//         return false;
//
//      // must be a directory
//      return true;      
//   }
//
//   return false;
//}
//
//
//
///**
// * \brief ¼ò¶ÌÃèÊö
// * 
// * ÏêÏ¸ÃèÊö
// * 
// * \param pParent : ÃèÊö
// * \param pDir : ÃèÊö
// * \return ·µ»ØÖµµÄÃèÊö
// */
////--------------------------------------
//bool isSubDirectory(const char *pParent, const char *pDir)
//{
//   if (!pParent || !*pDir) 
//      return false;
//
//   // this is somewhat of a brute force method but we need to be 100% sure
//   // that the user cannot enter things like ../dir or /dir etc,...
//   WIN32_FIND_DATA findData;
//   HANDLE handle = FindFirstFile(avar("%s/*", pParent), &findData);
//   if (handle == INVALID_HANDLE_VALUE)
//      return false;
//   do
//   {
//      // if it is a directory...
//      if (findData.dwFileAttributes & 
//          (FILE_ATTRIBUTE_DIRECTORY|                                      
//           FILE_ATTRIBUTE_OFFLINE|
//           FILE_ATTRIBUTE_SYSTEM|
//           FILE_ATTRIBUTE_TEMPORARY) )                             
//      {
//         // and the names match
//         if (strcmp(pDir, findData.cFileName ) == 0)
//         {
//            // then we have a real sub directory
//            FindClose(handle);
//            return true;
//         }
//      }
//   }while(FindNextFile(handle, &findData));
//
//   FindClose(handle);
//   return false;
//}
////*/

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
File::File()
: currentStatus(Closed), capability(0)
{
	handle = (void *)INVALID_HANDLE_VALUE;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
File::~File()
{
	close();
    handle = (void *)INVALID_HANDLE_VALUE;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
File::Status File::close()
{
    // check if it's already closed...
    if (Closed == currentStatus)
        return currentStatus;
	
    // it's not, so close it...
    if (INVALID_HANDLE_VALUE != (HANDLE)handle)
    {
        if (0 == CloseHandle((HANDLE)handle))
            return setStatus();                                    // unsuccessful
    }
    handle = (void *)INVALID_HANDLE_VALUE;
    return currentStatus = Closed;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param filename : ÃèÊö
 * \param openMode : ÃèÊö
 * \param randomAccess : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
File::Status File::open(const char *filename, const AccessMode openMode,bool randomAccess)
{
	char filebuf[MAX_PATH];
	strcpy(filebuf , filename);
	backslash(filebuf);
	filename = filebuf;
	
	
    // Close the file if it was already open...
    if (Closed != currentStatus)
        close();
	
    // create the appropriate type of file...
	DWORD dwFlags = (randomAccess ? 0 : FILE_FLAG_SEQUENTIAL_SCAN);
    switch (openMode)
    {
    case Read:
        handle = (void *)CreateFile(filename,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | dwFlags,
			NULL);
        break;
    case Write:
        handle = (void *)CreateFile(filename,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | dwFlags,
			NULL);
        break;
    case ReadWrite:
        handle = (void *)CreateFile(filename,
			GENERIC_WRITE | GENERIC_READ,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | dwFlags,
			NULL);
        break;
    case WriteAppend:
        handle = (void *)CreateFile(filename,
			GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | dwFlags,
			NULL);
        break;
		
    default:
        assert( 0 && "File::open: bad access mode");    // impossible
	}
    
    if (INVALID_HANDLE_VALUE == (HANDLE)handle)                // handle not created successfully
        return setStatus();
    else
    {
        // successfully created file, so set the file capabilities...
        switch (openMode)
        {
        case Read:
            capability = DWORD(FileRead);
            break;
        case Write:
        case WriteAppend:
            capability = DWORD(FileWrite);
            break;
        case ReadWrite:
            capability = DWORD(FileRead)  |
				DWORD(FileWrite);
            break;
        default:
            assert( 0 && "File::open: bad access mode");
        }
        return currentStatus = Ok;                                // success!
    }
}

//-----------------------------------------------------------------------------
// Self-explanatory.
//-----------------------------------------------------------------------------
File::Status File::getStatus() const
{
    return currentStatus;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-----------------------------------------------------------------------------
// Sets and returns the currentStatus when an error has been encountered.
//-----------------------------------------------------------------------------
File::Status File::setStatus()
{
    switch (GetLastError())
    {
    case ERROR_INVALID_HANDLE:
    case ERROR_INVALID_ACCESS:
    case ERROR_TOO_MANY_OPEN_FILES:
    case ERROR_FILE_NOT_FOUND:
    case ERROR_SHARING_VIOLATION:
    case ERROR_HANDLE_DISK_FULL:
		return currentStatus = IOError;
		
    default:
		return currentStatus = UnknownError;
    }
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param status : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-----------------------------------------------------------------------------
// Sets and returns the currentStatus to status.
//-----------------------------------------------------------------------------
File::Status File::setStatus(File::Status status)
{
    return currentStatus = status;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dst : ÃèÊö
 * \param size : ÃèÊö
 * \param bytesRead : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
File::Status File::read(void *dst,DWORD size,  DWORD *bytesRead)
{
    assert(Closed != currentStatus);
    assert(INVALID_HANDLE_VALUE != (HANDLE)handle);
    assert(NULL != dst);
    assert(hasCapability(FileRead));
    
    if (Ok != currentStatus || 0 == size)
        return currentStatus;
    else
    {
        DWORD lastBytes;
        DWORD *bytes = (NULL == bytesRead) ? &lastBytes : (DWORD *)bytesRead;
        if (0 != ReadFile((HANDLE)handle, dst, size, bytes, NULL))
        {
            if(*((DWORD *)bytes) != size)
                return currentStatus = EOS;                        // end of stream
        }
        else
            return setStatus();                                    // unsuccessful
    }
    return currentStatus = Ok;                                    // successfully read size bytes
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param src : ÃèÊö
 * \param size : ÃèÊö
 * \param bytesWritten : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
File::Status File::write(const void *src,DWORD size,  DWORD *bytesWritten)
{
    assert(Closed != currentStatus);
    assert(INVALID_HANDLE_VALUE != (HANDLE)handle);
    assert(NULL != src);
    assert(hasCapability(FileWrite));
    
    if ((Ok != currentStatus && EOS != currentStatus) || 0 == size)
        return currentStatus;
    else
    {
        DWORD lastBytes;
        DWORD *bytes = (NULL == bytesWritten) ? &lastBytes : (DWORD *)bytesWritten;
        if (0 != WriteFile((HANDLE)handle, src, size, bytes, NULL))
            return currentStatus = Ok;                            // success!
        else
            return setStatus();                                    // unsuccessful
    }
}

bool File::hasCapability(Capability cap) const
{
    return (0 != (DWORD(cap) & capability));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param position : ÃèÊö
 * \param absolutePos : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
File::Status File::setPosition(DWORD position, bool absolutePos)
{
    assert(Closed != currentStatus);
    assert(INVALID_HANDLE_VALUE != (HANDLE)handle);
    
    if (Ok != currentStatus && EOS != currentStatus)
        return currentStatus;
    
    DWORD finalPos;
    if(absolutePos)
    {
		// absolute position
        assert(0 <= position);
        
        // position beyond EOS is OK
        finalPos = SetFilePointer((HANDLE)handle,
			position,
			NULL,
			FILE_BEGIN);
	}
    else
	{
		// relative position
        assert((getPosition() >= (DWORD)abs((long)position) && 0 > position) || 0 <= position);
        
        // position beyond EOS is OK
        finalPos = SetFilePointer((HANDLE)handle,
			position,
			NULL,
			FILE_CURRENT);
    }
	
    if (0xffffffff == finalPos)
        return setStatus();                                        // unsuccessful
    else if (finalPos >= getSize())
        return currentStatus = EOS;                                // success, at end of file
    else
        return currentStatus = Ok;                                // success!
}

//-----------------------------------------------------------------------------
// Get the current position of the file pointer.
//-----------------------------------------------------------------------------
DWORD File::getPosition() const
{
    AssertFatal(Closed != currentStatus, "File::getPosition: file closed");
    AssertFatal(INVALID_HANDLE_VALUE != (HANDLE)handle, "File::getPosition: invalid file handle");
	
    return SetFilePointer((HANDLE)handle,
		0,                                    // how far to move
		NULL,                                    // pointer to high word
		FILE_CURRENT);                        // from what point
}

//-----------------------------------------------------------------------------
// Get the size of the file in bytes.
// It is an error to query the file size for a Closed file, or for one with an
// error status.
//-----------------------------------------------------------------------------
DWORD File::getSize() const
{
    assert(INVALID_HANDLE_VALUE != (HANDLE)handle);
    
    if (Ok == currentStatus || EOS == currentStatus)
    {
        DWORD high;
        return GetFileSize((HANDLE)handle, &high);                // success!
    }
    else
        return 0;                                                // unsuccessful
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-----------------------------------------------------------------------------
// Flush the file.
// It is an error to flush a read-only file.
// Returns the currentStatus of the file.
//-----------------------------------------------------------------------------
File::Status File::flush()
{
    assert(Closed != currentStatus);
    assert(INVALID_HANDLE_VALUE != (HANDLE)handle);
    assert(hasCapability(FileWrite));
	
    if (0 != FlushFileBuffers((HANDLE)handle))
        return setStatus();                                        // unsuccessful
    else
        return currentStatus = Ok;                                // success!
}


