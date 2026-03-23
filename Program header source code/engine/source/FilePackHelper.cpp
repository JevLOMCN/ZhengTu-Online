/**
 * \file      FilePackHelper.cpp
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     提供Zip文件包及自定义文件包的Stream接口支持，支持多线程
 * 
 *	      提供Zip文件包及自定义文件包的Stream接口支持，支持多线程
 */
#include "../include/engine.h"
#include <algorithm>
#include <functional>

//#include "../../EncDec/include/EncDec.h"
#include "../include/crc.h"

#define XSHARELIB_API 

//#define _USE_MEMVIEWOFFILE
#ifdef _USE_MEMVIEWOFFILE
typedef MemMapFileStream tFileStream;
#else
typedef FileStream	tFileStream;
#endif

const char* sResBasePath = "data\\";

struct stZipFileInfo
{
	DWORD dwFileIndex;
	size_t nFileOffset;
	size_t nFileSize;
	size_t nCompressedFileSize;
	DWORD  dwCompressedCRC;
	DWORD  packFlags;
	String name;
	bool bGetDetailInfo;
};

class stZipPackInfo
{
public:
	enum enumPackType{
		eZipPack,
		eMyPack,
	};

	typedef std::map<DWORD,stZipFileInfo > tMapZipPackInfo;
	typedef std::map<DWORD,tFileStream*> tThreadPacks;

	tMapZipPackInfo mapZipPackInfo;
	tThreadPacks threadPacks;
	DWORD dwVersion;
	DWORD dwFlags;
	enumPackType packType;
	BYTE des_key[3][8];
	String strPackFileName;
	bool	bFullMap;

	stZipPackInfo()
	{
	}

	~stZipPackInfo()
	{
		for(tThreadPacks::iterator it = threadPacks.begin(); it != threadPacks.end(); ++it)
		{
			delete it->second;
		}
		threadPacks.clear();
	}

};

typedef stdext::hash_map<String,stZipPackInfo*> tMapPacks;
static tMapPacks sPacks;

static CLock g_FilePackLock;

static bool isFullBuffer(const char* packName)
{
	return true;
	/*const char* fullBuferPackName[]={
		"model",
		"terrain",
		"texture",
	};
	for(size_t i=0; i<count_of(fullBuferPackName);++i)
	{
		if(stricmp(packName,fullBuferPackName[i]) == 0)
			return true;
	}
	return false;*/
}
static bool InitZipPackInfo(const TCHAR* pszZipPack,stZipPackInfo * & pZipPackInfo)
{
	bool bFullMap = false;//isFullMap(pszZipPack);
	DWORD dwThreadId = GetCurrentThreadId();

	pZipPackInfo = new stZipPackInfo;
	pZipPackInfo->dwFlags = 0;
	pZipPackInfo->strPackFileName = pszZipPack;
	pZipPackInfo->bFullMap = bFullMap;

	PackAggregate packAggregate;
	ZipAggregate zipAggregate;

	PasswordToDesKey( PackAggregate::GetDefaultPassword(),pZipPackInfo->des_key[0],pZipPackInfo->des_key[1],pZipPackInfo->des_key[2] );
	packAggregate.SetDesKey((const BYTE*)pZipPackInfo->des_key);

	if( packAggregate.openAggregate(pszZipPack) )
	{
		pZipPackInfo->packType = stZipPackInfo::eMyPack;
		pZipPackInfo->dwFlags = packAggregate.getFlags();
		pZipPackInfo->dwVersion = packAggregate.getVersion();
		if(pZipPackInfo->dwFlags & PackAggregate::flagEncrypt || pZipPackInfo->dwFlags & PackAggregate::flagEncryptHeaderOnly)
		{
			//Assert(pszPassword);
		}

		PackAggregate::iterator itr;
		for(itr = packAggregate.begin();itr != packAggregate.end();++itr)
		{
			const PackAggregate::stFileBaseInfo & fileInfo = *itr;
			TCHAR szFileName[MAX_PATH];
			sprintf(szFileName,"%s",fileInfo.szName);
			strlwr(szFileName);
			replaceFrontlashPath(szFileName);
			//TRACE("发现资源文件: %s\n",szFileName);

			stZipFileInfo& ZipFileInfo = pZipPackInfo->mapZipPackInfo[calculateCRC(szFileName,strlen(szFileName))];
			ZipFileInfo.dwFileIndex = itr - packAggregate.begin();
			ZipFileInfo.bGetDetailInfo = false;
			ZipFileInfo.nFileOffset = fileInfo.fileOffset;
			ZipFileInfo.nCompressedFileSize = fileInfo.size;
			ZipFileInfo.dwCompressedCRC = fileInfo.dwCRC;

			ZipFileInfo.name = szFileName;

		}
		packAggregate.closeAggregate();
	}
	else if(zipAggregate.openAggregate(pszZipPack))
	{
		pZipPackInfo->packType = stZipPackInfo::eZipPack;

		ZipAggregate::iterator itr;
		String strZipPackFileName = GetFileNameTitle(pszZipPack);
		for (itr = zipAggregate.begin (); itr != zipAggregate.end (); itr++)
		{
			const ZipAggregate::FileEntry & rEntry = * itr;
			TCHAR szFileName[MAX_PATH];

			sprintf(szFileName,"%s",rEntry.pFileName);
			strlwr(szFileName);
			replaceFrontlashPath(szFileName);
			//TRACE("发现资源文件: %s\n",szFileName);

			stZipFileInfo& ZipFileInfo = pZipPackInfo->mapZipPackInfo[calculateCRC(szFileName,strlen(szFileName))];
			ZipFileInfo.nFileSize = rEntry.fileSize;
			ZipFileInfo.nFileOffset = rEntry.fileOffset;
			Assert(ZipFileInfo.nFileOffset != 4272471662);
			ZipFileInfo.nCompressedFileSize = rEntry.compressedFileSize;
			ZipFileInfo.name = szFileName;
			/*if(s_bFillFileNameBase)
			{
			char szFileNameBase[MAX_PATH];
			strcpy(szFileNameBase,szFileName);
			char* p = strrchr(szFileNameBase,'.');
			if(p)
			{
			*p = 0;
			stResFileNames * & pF = (*s_mapFileNameBase)[szFileNameBase];
			if( pF == NULL ) 
			{
			pF = new stResFileNames;
			pF->bFullMap = bFullMap;
			}
			pF->array.push_back(szFileName);
			}
			}*/

		}
		zipAggregate.closeAggregate();
	}
	else
	{
		Assert(0);
		return false;
	}
	return true;
}

//static void SearchDirFileList(const TCHAR* szDir,stThreadZipPacks * pThreadPacks)
//{
//	WIN32_FIND_DATA findData;
//	HANDLE hFind;
//	TCHAR szFind[MAX_PATH];
//	if(szDir[0])
//		sprintf(szFind,"%s\\*.*",szDir);
//	else
//		sprintf(szFind,".\\*.*");
//	hFind = FindFirstFile(szFind,&findData);
//	BOOL bFind = (hFind != NULL);
//	bool bFullMap = isFullMap(szDir);
//
//	while(bFind)
//	{
//		if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//		{
//			if(strcmp(findData.cFileName,".") && strcmp(findData.cFileName,".."))
//			{
//				TCHAR szPackName1[MAX_PATH];
//				TCHAR szPackName2[MAX_PATH];
//				if(szDir[0])
//				{
//					sprintf(szPackName1,"%s\\%s.pak",szDir,findData.cFileName);
//					sprintf(szPackName2,"%s\\%s.zip",szDir,findData.cFileName);
//				}
//				else
//				{
//					sprintf(szPackName1,"%s.pak",findData.cFileName);
//					sprintf(szPackName2,"%s.zip",findData.cFileName);
//				}
//
//				if(!IsFileExist(szPackName1) && !IsFileExist(szPackName2))
//				{
//					TCHAR szDirNew[MAX_PATH];
//					
//					if(szDir[0])
//						sprintf(szDirNew,"%s\\%s",szDir,findData.cFileName);
//					else
//						sprintf(szDirNew,"%s",findData.cFileName);
//
//					SearchDirFileList(szDirNew,pThreadPacks);
//				}
//			}
//		}
//		else
//		{
//			TCHAR szFileName[MAX_PATH];
//			if(szDir[0])
//				sprintf(szFileName,"%s\\%s",szDir,findData.cFileName);
//			else
//				strcpy(szFileName,findData.cFileName);
//
//			strlwr(szFileName);
//			String strExtName = GetFileNameExt(findData.cFileName);
//
//			if(stricmp("pak",strExtName.c_str()) == 0 || stricmp("zip",strExtName.c_str()) == 0)
//			{
//				// 搜索包中的文件
//				//replaceBacklashPath(szFileName);
//				InitZipPackInfo(szFileName,pThreadPacks);
//			}
//			else
//			{
//				replaceBacklashPath(szFileName);
//				pThreadPacks->mapZipPackInfo[szFileName].pPackInfo = NULL;
//				//if(s_bFillFileNameBase)
//				//{
//				//	char szFileNameBase[MAX_PATH];
//				//	strcpy(szFileNameBase,szFileName);
//				//	char* p = strrchr(szFileNameBase,'.');
//				//	if(p)
//				//	{
//				//		*p = 0;
//				//		stResFileNames * & pF = (*s_mapFileNameBase)[szFileNameBase];
//				//		//TRACE("resource %s\n",szFileNameBase);
//				//		if( pF == NULL ) 
//				//		{
//				//			pF = new stResFileNames;
//				//			pF->bFullMap = bFullMap;
//				//		}
//				//		pF->array.push_back(szFileName);
//				//	}
//				//}
//				//TRACE("发现资源文件: %s\n",szFileName);
//			}
//		}
//		bFind = FindNextFile(hFind,&findData);
//	}
//	FindClose(hFind);
//}
static bool isSearchInPack(const char* packName)
{

	const char* noSearch[] = {
		"alpha",
		"magic",
		"body",
		"npc",
		"terrain",
		"terraina",
		"weaponl",
		"weaponr",
		"interfaces",
		"icons",
		"hair",
		"cursor",
		"items",
		"login",
		"logo",
		"minimap",
		"other",
	};
	char szPackName[MAX_PATH];
	char* p1,*p2;
	p1 = strrchr(packName,'\\');
	if(!p1) p1 = (char*)packName;
	p2 = strrchr(packName,'.');
	if(!p2) p2 = (char*)packName + strlen(packName)-1;

	while(p2 >= p1 && *p2 >= '0' && *p2 <= '9')
		--p2;
	memcpy(szPackName,p1,p2-p1+1);
	szPackName[p2-p1+1] = 0;
	strlwr(szPackName);

	for(size_t i=0; i < count_of(noSearch); ++i){
		if(strcmp(szPackName,noSearch[i]) == 0){
			return false;
		}
	}

	if(strcmp(szPackName,"map") == 0){
		if(*(p2 + 1) >= '0' && *(p2+1) <= '9'){
			return false;
		}
	}
	return true;
}

static void SearchDirFileList()
{
	WIN32_FIND_DATA findData;
	HANDLE hFind;
	char szFind[MAX_PATH];
	sprintf(szFind,"%s*.pak",sResBasePath);
	hFind = FindFirstFile(szFind,&findData);
	BOOL bFind = (hFind != INVALID_HANDLE_VALUE && hFind != NULL);
	while(bFind)
	{
		TCHAR szFileName[MAX_PATH];
		sprintf(szFileName,"%s%s",sResBasePath,findData.cFileName);
		TCHAR szName[MAX_PATH];
		strcpy(szName,findData.cFileName);
		*strchr(szName,'.') = 0;
		strlwr(szName);
		if(isSearchInPack(szName)){
			Assert(sPacks.find(szName) == sPacks.end());
			stZipPackInfo*& pp = sPacks[szName];
			InitZipPackInfo(szFileName,pp);
		}
		bFind = FindNextFile(hFind,&findData);
	}
	FindClose(hFind);

	sprintf(szFind,"%s*.zip",sResBasePath);
	hFind = FindFirstFile(szFind,&findData);
	bFind = (hFind != INVALID_HANDLE_VALUE && hFind != NULL);
	while(bFind)
	{
		TCHAR szFileName[MAX_PATH];
		sprintf(szFileName,"%s%s",sResBasePath,findData.cFileName);
		TCHAR szName[MAX_PATH];
		strcpy(szName,findData.cFileName);
		*strchr(szName,'.') = 0;
		strlwr(szName);
		if(isSearchInPack(szName)){
			Assert(sPacks.find(szName) == sPacks.end());
			stZipPackInfo*& pp = sPacks[szName];
			InitZipPackInfo(szFileName,pp);
		}
		bFind = FindNextFile(hFind,&findData);
	}
	FindClose(hFind);
}

static int __cdecl compareFileName(const void * p1, const void * p2)
{
	String * pstr1 = (String*)p1;
	String * pstr2 = (String*)p2;
	char* pext1 = strrchr(pstr1->c_str(),'.');
	char* pext2 = strrchr(pstr2->c_str(),'.');

	if(strcmp(pext1,".xpc"))
	{
		return -1;
	}
	if(strcmp(pext2,".xpc"))
	{
		return 1;
	}
	if(strcmp(pext1,".bmp"))
	{
		return -1;
	}
	if(strcmp(pext2,".bmp"))
	{
		return 1;
	}

	if(strcmp(pext1,".tga"))
	{
		return -1;
	}
	if(strcmp(pext2,".tga"))
	{
		return 1;
	}
	return 0;
}

static void SortFileNameBase()
{
	/*for(tMapFileNameBase::iterator it = s_mapFileNameBase->begin(); it != s_mapFileNameBase->end(); ++it)
	{
	tFileNameArray & array = it->second->array;
	qsort(&array[0],array.size(),sizeof(String),compareFileName);
	}*/
}

static Stream* OpenZipFileForRead(stZipPackInfo * pZipPackInfo,stZipFileInfo * pFile,bool bFullBuffer)
{
	stZipFileInfo& ZipFileInfo = *pFile;
	g_FilePackLock.Lock();
	tFileStream * & ppStream = pZipPackInfo->threadPacks[GetCurrentThreadId()];
	if(ppStream == NULL)
		ppStream = new tFileStream;
	tFileStream* pStream = ppStream;
	g_FilePackLock.Unlock();

	if(pStream->getStatus() == Stream::Closed)
	{
#ifdef _USE_MEMVIEWOFFILE
		if(!pStream->open(pZipPackInfo->strPackFileName.c_str(),pZipPackInfo->bFullMap))
#else
		if(!pStream->open(pZipPackInfo->strPackFileName.c_str(),FileStream::Read,0))
#endif
			return NULL;
	}
	if(pStream->getStatus() != Stream::Ok)
	{
		pStream->close ();
		return NULL;
	}
		
	pStream->setPosition(ZipFileInfo.nFileOffset);

	if(pZipPackInfo->packType == stZipPackInfo::eZipPack)
	{
		ZipLocalFileHeader zlfHeader;
		if (zlfHeader.readFromStream (*pStream) == false)
		{
			pStream->close ();
			return NULL;
		}

		if (zlfHeader.m_header.compressionMethod == ZipLocalFileHeader::Stored
			|| ZipFileInfo.nFileSize == 0)
		{
			// 不压缩的
			ResizeFilterStream *strm = new ResizeFilterStream;
			strm->attachStream (pStream);
			strm->setStreamOffset (pStream->getPosition (), ZipFileInfo.nFileSize);
			return strm;
		}
		else
		{
			if (zlfHeader.m_header.compressionMethod ==
				ZipLocalFileHeader::Deflated)
			{
				// 压缩的
				ZipSubRStream *zipStream = new ZipSubRStream(bFullBuffer);
				zipStream->setUncompressedSize (ZipFileInfo.nFileSize);
				zipStream->attachStream (pStream);
				return zipStream;
			}
			else
			{
				// 出错了
				Assert(0);
				pStream->close ();
				return NULL;
			}
		}
	}
	else if(pZipPackInfo->packType == stZipPackInfo::eMyPack)
	{
		if(ZipFileInfo.bGetDetailInfo == false){
			PackAggregate pack;
			pack.SetDesKey(pZipPackInfo->des_key[0]);
			PackAggregate::stFileDetailInfo info;
			pack.getFileInfo(ZipFileInfo.dwFileIndex,pStream,pZipPackInfo->dwVersion,pZipPackInfo->dwFlags,ZipFileInfo.nFileOffset,&info);
			ZipFileInfo.bGetDetailInfo = true;
			ZipFileInfo.nFileOffset = info.fileOffset;
			ZipFileInfo.nFileSize = info.uncompressedSize;
			ZipFileInfo.packFlags = info.flags;
		}

		if(ZipFileInfo.packFlags == 0)
		{
			// 不压缩，不加密
			ResizeFilterStream *strm = new ResizeFilterStream;
			strm->attachStream (pStream);
			strm->setStreamOffset (pStream->getPosition (), ZipFileInfo.nFileSize);
			return strm;
		}
		else
		{
			PackSubRStream * packStream = new PackSubRStream(ZipFileInfo.packFlags,bFullBuffer);

			packStream->SetDesKey((const BYTE*)pZipPackInfo->des_key);
			packStream->setUncompressedSize(ZipFileInfo.nFileSize);
			packStream->attachStream(pStream);
			return packStream;
		}
	}
	else
	{
		Assert(0);
		return NULL;
	}
}

static bool getPackFileInfo(const char* filename,stZipPackInfo *& pPack,stZipFileInfo *& pFile,String & packName)
{
	if( (filename[0] != '\\' || filename[1] != '\\') && (filename[0] == 0 || filename[1] != ':') )
	{
		if(filename[0] == 'd' && filename[1] == 'a' && filename[2] == 't' && filename[3] == 'a')
		{
			char* p1 = strchr(filename,'\\');
			if(p1 == NULL) return false;
			char* p2 = strchr(p1+1,'\\');
			if(p2 == NULL) return false;

			packName.resize(p2-p1-1);
			char* szName = &packName[0];
			memcpy(szName,p1+1,p2-p1-1);
			szName[p2-p1-1] = 0;

			tMapPacks::iterator it = sPacks.find(packName);
			if(it == sPacks.end()) 
				return false;

			pPack = it->second;
			stZipPackInfo::tMapZipPackInfo::iterator it1 = pPack->mapZipPackInfo.find(calculateCRC(p1+1,strlen(p1+1)));
			if(it1 == pPack->mapZipPackInfo.end())
				return false;
			pFile = &it1->second;
			return true;
		}
	}
	return false;
}

static String fixResFileName(const char* pszFileName)
{
	if(pszFileName[0] == '.') 
		pszFileName++;

	if( (pszFileName[0] == '\\' && pszFileName[1] == '\\') 
		|| (pszFileName[0] && pszFileName[1] == ':'))
	{
		return pszFileName;
	}

	String strFileName = sResBasePath;
	strFileName += pszFileName;
	//replaceBacklashPath(&strFileName[0]);
	strlwr(&strFileName[0]);
	return strFileName;
}

template < class _Tlist>
void insertToList(_Tlist & list,const char* t)
{
	list.push_back(t);
}

template <>
void insertToList(std::set<String> & list,const char* t)
{
	list.insert(t);
}

template < class _TList>
void _searchDirFiles(const char* path,int basePathLen,_TList & files,bool (__cdecl * filterFunc)(const char* filename))
{
	WIN32_FIND_DATA findData;
	HANDLE hFind;
	char szFind[MAX_PATH];
	sprintf(szFind,"%s\\*.*",path);
	hFind = FindFirstFile(szFind,&findData);
	BOOL bFind = (hFind != INVALID_HANDLE_VALUE && hFind != NULL);
	while(bFind)
	{
		if(strcmp(findData.cFileName,"..") && strcmp(findData.cFileName,"."))
		{
			char szFileName[MAX_PATH];
			sprintf(szFileName,"%s\\%s",path,findData.cFileName);
			if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				_searchDirFiles(szFileName,basePathLen,files,filterFunc);
			}
			else
			{
				//replaceBacklashPath(szFileName);
				if(filterFunc == NULL || filterFunc(szFileName + basePathLen))
					insertToList(files,(szFileName + basePathLen));
			}
		}

		bFind = FindNextFile(hFind,&findData);
	}
	FindClose(hFind);
}

template < class _TList>
void _searchFileInPack(const char* packName,_TList & files,bool (__cdecl * filterFunc)(const char* filename))
{
	char szPackName[MAX_PATH];
	strcpy(szPackName,packName);
	strlwr(szPackName);
	tMapPacks::iterator it = sPacks.find(szPackName);
	if(it == sPacks.end())
	{
		char szPath[MAX_PATH];
		sprintf(szPath,"%s%s",sResBasePath,packName);
		_searchDirFiles(szPath,strlen(sResBasePath) + strlen(packName) + 1,files,filterFunc);
	}
	else
	{
		stZipPackInfo* p = it->second;
		for(stZipPackInfo::tMapZipPackInfo::iterator it1 = p->mapZipPackInfo.begin(); it1 != p->mapZipPackInfo.end(); ++it1)
		{
			const char* pszName = (const char*)(it1->second.name.c_str()) + strlen(packName) + 1;
			if(filterFunc == NULL || filterFunc(pszName))
				insertToList(files,pszName);
		}
	}
}

static int __cdecl compareFileInfo(const void *p1, const void *p2) 
{
	stFileInfo * f1 = (stFileInfo*)p1;
	stFileInfo * f2 = (stFileInfo*)p2;
	return stricmp(f1->szFileName,f2->szFileName);
}

// 注意：此函数只支持未压缩且未加密的文件包
bool searchFileInPack(const char* packName,std::vector<stFileInfo> & files,stFileInfo * attributes,bool (__cdecl * filterFunc)(const char* filename) )
{
	char szPackName[MAX_PATH];
	strcpy(szPackName,packName);
	strlwr(szPackName);
	tMapPacks::iterator it = sPacks.find(szPackName);
	stZipPackInfo * pack = NULL;
	if(it == sPacks.end())
	{
		if(strrchr(packName,'.') == 0)
		{
			char szPackName[MAX_PATH];
			strcpy(szPackName,packName);
			strcat(szPackName,".pak");
			packName = szPackName;
		}
		if(!InitZipPackInfo(packName,pack))
		{
			delete pack;
			return false;
		}
	}
	else
	{
		pack = it->second;
	}

	std::string strTile = GetFileNameTitle(packName);

	stZipPackInfo* p = pack;
	files.reserve(p->mapZipPackInfo.size());
	for(stZipPackInfo::tMapZipPackInfo::iterator it1 = p->mapZipPackInfo.begin(); it1 != p->mapZipPackInfo.end(); ++it1)
	{
		const char* pszName = (const char*)(it1->second.name.c_str()) + strTile.length() + 1;
		if(filterFunc == NULL || filterFunc(pszName))
		{
			stFileInfo info;
			strcpy(info.szFileName,pszName);
			info.offsetInPack = it1->second.nFileOffset + sizeof(PackAggregate::stFileSizeInfo);
			info.size = it1->second.nCompressedFileSize - sizeof(PackAggregate::stFileSizeInfo);
			info.crc = it1->second.dwCompressedCRC;

			if(stricmp(pszName,"attributes.ghd") == 0){
				*attributes = info;
			}else{
				files.push_back(info);
			}
		}else{
			if(stricmp(pszName,"attributes.ghd") == 0){
				strcpy(attributes->szFileName,pszName);
				attributes->offsetInPack = it1->second.nFileOffset + sizeof(PackAggregate::stFileSizeInfo);
				attributes->size = it1->second.nCompressedFileSize - sizeof(PackAggregate::stFileSizeInfo);
				attributes->crc = it1->second.dwCompressedCRC;
			}
		}
	}

	//qsort(&files[0],files.size(),sizeof(stFileInfo),compareFileInfo);
	std::sort(files.begin(),files.end());

	if(it == sPacks.end())
	{
		delete pack;
	}

	return true;
}

XSHARELIB_API void searchFileInPack(const char* packName,std::vector<String> & files,bool (__cdecl * filterFunc)(const char* filename))
{
	_searchFileInPack(packName,files,filterFunc);
}

XSHARELIB_API void searchFileInPack(const char* packName,std::set<String> & files,bool (__cdecl * filterFunc)(const char* filename))
{
	_searchFileInPack(packName,files,filterFunc);
}

XSHARELIB_API bool	IsExistResFile(const TCHAR* pszFileName)
{
	stZipPackInfo* pPack;
	stZipFileInfo* pFile;
	String filename = fixResFileName(pszFileName);
	String packName;
	if(!getPackFileInfo(filename.c_str(),pPack,pFile,packName))
	{
		replaceFrontlashPath(&filename[0]);
		return IsFileExist(filename.c_str());
	}
	return true;	
}

XSHARELIB_API Stream * OpenResFile(const TCHAR* pszFileName)
{
	stZipPackInfo* pPack;
	stZipFileInfo* pFile;
	String filename = fixResFileName(pszFileName);
	String packName;
	if(!getPackFileInfo(filename.c_str(),pPack,pFile,packName))
	{
		// 不是包中的文件

		bool bFullMap = false;//isFullMap(szFileName);
		replaceFrontlashPath(&filename[0]);
		if(IsFileExist(filename.c_str()))
		{
#ifdef _USE_MEMVIEWOFFILE
			MemMapFileStream * pStream = new MemMapFileStream(filename,bFullMap);
#else
			FileStream * pStream = new FileStream(filename.c_str(),FileStream::Read);
#endif
			return pStream;
		}
		TRACE("未找到资源文件 %s\n",pszFileName);
		return NULL;
	}
	else
	{
		return OpenZipFileForRead(pPack,pFile,isFullBuffer(packName.c_str()));
	}
}

XSHARELIB_API void CloseResFile(Stream* pStream)
{
	if(!pStream) return;

	pStream->close();
	delete pStream;
}

bool GetResFileOffset(const TCHAR* pszFileName,size_t & offsetInPack/* -1 表示不在包中*/,size_t size)
{
	stZipPackInfo* pPack;
	stZipFileInfo* pFile;
	String filename = fixResFileName(pszFileName);
	String packName;
	if(!getPackFileInfo(filename.c_str(),pPack,pFile,packName))
	{
		// 不是包中的文件
		return false;
	}
	else
	{
		offsetInPack = pFile->nFileOffset;
		size = pFile->nFileSize;
		return true;
	}
}

XSHARELIB_API bool InitFilePackHelper()
{
	//initMemMapFileManager();
	SearchDirFileList();
	return true;
}

XSHARELIB_API void UninitFilePackHelper()
{
	for(tMapPacks::iterator it = sPacks.begin(); it != sPacks.end(); ++it)
	{
		stZipPackInfo * pInfo = it->second;
		delete pInfo;
	}
	sPacks.clear();
	//shutdownMemMapFileManager();
}
//bool DecompositionPackFile(const TCHAR *pszZipPack,const TCHAR *newPath,const TCHAR *pszPassword)
//{
//	FILE *pf = fopen("D:\\e.txt","wt");
//	if(!pf) return 0;
//		
//	TCHAR szZipPack[MAX_PATH];
//	
//	stThreadZipPacks* pThreadZipPacks = GetCurrentThreadZipPacks();
//	Assert(pThreadZipPacks);
//	stZipPackInfo* pZipPackInfo = GetZipPackInfo(pszZipPack,pThreadZipPacks,pszPassword);
//	Assert(pZipPackInfo);
//
//	fprintf(pf,"%d\n",pZipPackInfo->mapZipFileInfo.size);	
//	stZipPackInfo::tMapZipFileInfo::iterator it = pZipPackInfo->mapZipFileInfo.begin();
//	BYTE btBuf[1024];
//	while(it != pZipPackInfo->mapZipFileInfo.end())
//	{
//		String str = it->first;
//	
//		Stream *pFs = OpenPackFileForRead(pszZipPack,str.c_str(),pszPassword);
//		memset(szZipPack,0,MAX_PATH);
//		strcpy(szZipPack,newPath);
//		strlwr(szZipPack);
//		strcat(szZipPack,str.c_str());
//
//		fprintf(pf,"%s\n",szZipPack);	
//
//		replaceFrontlashPath(szZipPack);
//
//		fprintf(pf,"%s\n",szZipPack);	
//
//		TRACE(szZipPack);
//		CreateFile_Ex(newPath,szZipPack);
//
//		fprintf(pf,"%s\n",str);	
//		FILE *pSubFile = fopen(szZipPack,"wb");
//		if(!pSubFile) 
//		{
//			++it;
//			continue;
//		}
//		while(pFs->getStatus() != Stream::EOS)
//		{			
//			BYTE bt = 0;
//			bool b = pFs->read(bt);
//			fwrite(&bt,sizeof(BYTE),1,pSubFile);
//			fclose(pSubFile);
//		}		
//		++it;
//	}
//	fclose(pf);
//	return false;
//}

