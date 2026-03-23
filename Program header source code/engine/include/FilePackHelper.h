/**
 * \file      FilePackHelper.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     提供Zip文件包及自定义Pak文件包的透明Stream接口支持，是线程安全的
 * 
 *	      提供Zip文件包及自定义Pak文件包的透明Stream接口支持，是线程安全的
 */

#ifndef _FILEPACKHELPER_H
#define _FILEPACKHELPER_H
 
class Stream;

class Stream;


Stream * OpenResFile(const TCHAR* pszFileName);
bool	IsExistResFile(const TCHAR* pszFileName);
void CloseResFile(Stream* pStream);
bool GetResFileOffset(const TCHAR* pszFileName,size_t & offsetInPack/* -1 表示不在包中*/,size_t size);

bool InitFilePackHelper();
void UninitFilePackHelper();

struct stFileInfo{
	char szFileName[MAX_PATH];
	size_t offsetInPack;
	size_t size;
	DWORD  crc;
	bool operator < (const stFileInfo & fi){
		return stricmp(szFileName,fi.szFileName) < 0;
	}
};
bool searchFileInPack(const char* packName,std::vector<stFileInfo> & files,stFileInfo * attributes,bool (__cdecl * filterFunc)(const char* filename) = NULL);
void searchFileInPack(const char* packName,std::vector<String> & files,bool (__cdecl * filterFunc)(const char* filename) = NULL);
void searchFileInPack(const char* packName,std::set<String> & files,bool (__cdecl * filterFunc)(const char* filename) = NULL);


inline Stream * OpenPackFileForRead2(const char* pszFileName){
	
	if(strnicmp(pszFileName,".\\",strlen(".\\")) == 0)
		pszFileName += strlen(".\\");
	else if(strnicmp(pszFileName,"./",strlen("./")) == 0)
		pszFileName += strlen("./");

	if(strnicmp(pszFileName,"data\\",strlen("data\\")) == 0)
		pszFileName += strlen("data\\");
	else if(strnicmp(pszFileName,"data/",strlen("data/")) == 0)
		pszFileName += strlen("data/");

	return OpenResFile(pszFileName);
}

inline Stream * OpenPackFileForRead(const char* pszZipPack,const char* pszFileName){
	return OpenResFile(pszFileName);
}

inline void ClosePackFileForRead(Stream* pStream){
	CloseResFile(pStream);
}

#endif