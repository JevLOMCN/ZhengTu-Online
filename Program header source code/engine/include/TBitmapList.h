/**
 * \file      TBitmapList.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     图形包模板
 * 
 *	      图形包模板
 */

#pragma once

#include "cSafeQueue.h"

#include <list>
#include <vector>
#include <process.h>
#include <string>
#include "ibitmap.h"
#include "ibitmaplist.h"
#include "stack_alloc.h"
#include "Log.h"
#include "tBitmaps.h"
#include "../../Engine/include/Lock.h"
//inline HANDLE OpenFileForRead(const char* szFileName)
//{
//	return CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL);
//	//return fp->Create(szFileName,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING,/*FILE_FLAG_SEQUENTIAL_SCAN |*/ FILE_ATTRIBUTE_NORMAL);
//}

const DWORD c_nLongFrame = 60000;

typedef bool (WINAPI * fnVirtualLock)(LPVOID lpAddress,SIZE_T dwSize);
typedef bool (WINAPI * fnVirtualUnlock)(LPVOID lpAddress, SIZE_T dwSize);

const size_t MIN_MAP_SIZE = (1024 * 1024);
extern size_t gBitmapHeaderSize;
//template < class tBitmapList >
struct stLoadResT{
	/*
	tBitmapList * pBmpList;
	int group;
	int frame;
	int bmpIndex;
	//*/
	IBitmap* pBmp;
};

#define AniHdrDataMagic "XZAH"
#define AniHdrDataHdrVersion 3
struct stAniHdrDataHdr{
	DWORD magic;
	DWORD ver;
	DWORD glVer;
	DWORD glSize;
	FILETIME glTime; 
};

#define GLHdrDataMagic "XZGH"
#define GLHdrVersion 8
struct stGLHdrDataHdr{
	char magic[4];
	DWORD ver;
	DWORD glVer;
	DWORD glSize;
	FILETIME glTime;

	DWORD glHeaderSize;
	DWORD  nSaveBitmapData;
	DWORD nGroupNum;
};

struct stGLGroupInfo{
	DWORD	id;
	DWORD	offsetInFile;
	DWORD	size;
	//DWORD	offsetInPack;
	std::vector<stGrpAniInfo> * aniInfos;
};

struct stGLHdrInfo{
	size_t nHeaderSize;
	DWORD  nSaveBitmapData;
	std::vector<stGLGroupInfo> groups;
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param fileName : 描述
 * \param pTime : 描述
 * \param pSize : 描述
 * \return 返回值的描述
 */
inline bool GetFileTimeAndSize(const char* fileName,FILETIME* pTime,DWORD * pSize)
{
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((LPTSTR)fileName, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hFind);
	*pTime = findFileData.ftLastWriteTime;
	*pSize = findFileData.nFileSizeLow;
	return true;
}

struct stMapFileData{
	DWORD	dwFileOffset;
	DWORD	dwSize;
	DWORD	dwMarkTime;
	BYTE*	pAddress;
	std::set<IBitmap*> bitmapSet;
};

#define GLHEADER_HEADER_IDENT MAKEFOURCC('X','G','L','H')
#define GLHEADER_HEADER_VERSION 2

struct stGLHeaderInfoHeader{
	DWORD dwIdent;
	DWORD dwVersion;
	DWORD dwGroupCount;
	DWORD dwInfoOffset;
};

struct stGLGroupHeaderInfo{
	DWORD dwGrpFileCRC;
	DWORD dwGroupID;
	DWORD dwAniCount;
	DWORD dwOffset;
	DWORD dwAniHeaderOffset;
};

struct stGLAniHeaderInfo{
	stGrpAniInfo aniInfo;
	DWORD		 bitmapHeaderOffset;
};

class cGPackAttributes{
	stGLHeaderInfoHeader*			 mHeader;
	stGLGroupHeaderInfo*			 mGroups;
	std::vector<BYTE>				 mAttribute;
	File*							 mFile;
	int								 mCurGroup;
	int								 mCurAni;
	BYTE*							 mData;
	stGLAniHeaderInfo*				 mAniHeader;
public:

	cGPackAttributes(File* pPackFile,stFileInfo* attribute);
	int getGroupIndex(size_t groupID);
	bool setGroup(size_t groupID,DWORD dwCRC);
	void setAni(size_t aniIndex);
	const stGLHeader* getGLHeader();
	const stGrpAniInfo* getAniInfo();
	const BYTE* getBitmapInfo();
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
template < class tBitmap>
class TBitmapList : public IBitmapList
{
	friend class tBitmap;
public:
	typedef TBitmapList<tBitmap>	_Myt;
	typedef TBitmaps<tBitmap>		_MyBitmaps;

protected:

	struct stGroupDataMap{
		size_t	m_dataOffset;
		size_t	m_dataSize;
		BYTE*	m_data;
	};

	struct stGroup{
		DWORD	dwGroupID;
		_MyBitmaps*		m_aniBuf;
		_MyBitmaps* *	m_bitmaps;

		size_t		m_dataOffset;
		size_t		m_dataSize;
		stGroupDataMap* m_dataMap;
		//BYTE*		m_data;
		BYTE*	getData(){
			return m_dataMap->m_data + m_dataOffset - m_dataMap->m_dataOffset;
		}
		bool	isMaped(){
			return m_dataMap->m_data != NULL;
		}
		size_t		m_bitmapsCount;
		size_t	GetBitmapsCount(){ return m_bitmapsCount; }

		stGroup()
			: m_bitmapsCount(0)
			, dwGroupID(-1)
			, m_dataMap(NULL)
		{
		}

		~stGroup()
		{
			for(size_t i = 0; i< m_bitmapsCount;++i)
			{
				if(m_bitmaps[i])
				{
					destructInPlace(m_bitmaps[i]);
				}
			}
		}
	};

	//std::vector<stMapFileData *> m_mapFileDatas;
	//std::lookaside_allocator<stMapFileData,16> m_allocMapFileData;
	//CLock		m_lockOfMapFile;
public:
	typedef stLoadResT		stLoadRes;

	static DWORD		 g_dwAllocationGranularity;
	static size_t		 g_dwMemoryPageSize;
	static fnVirtualLock g_fnVirtualLock;
	static fnVirtualUnlock g_fnVirtualUnlock;
	static cSafeStack<stLoadRes > g_queue;
	//优先级不高的
	static cSafeQueue<stLoadRes > g_queueLower;
	static bool s_bQueueLowerEmpty;
	static bool	s_bExitThread;
	static bool s_bPauseThreadLoad;
	static size_t s_dwMemorySize;
	static _Myt* g_pRemove;
	static IBitmap* g_pRemoveBmp;
	static std::vector<stLoadRes>  g_loaded;// 已经装载的图片
	static CLock s_lockLoaded;		// g_loaded 数组的锁
	static CLock s_lockLoadThread;	// 锁定整个装载线程
	static CLock s_lockLoading;		// 装载过程的锁

	static HANDLE		g_hResThread;

	static DWORD GetAllocationGranularity();
	static void GetVirtualMemoryFnAddress();
	static void Load(stLoadRes& st);
	static bool remove_res_if(stLoadRes& st);
	static bool remove_res_if2(stLoadRes& st);
	static unsigned int __stdcall  ResLoadThread ( void * );
	static size_t ClearResMemory();
	static void DeleteLoadResource(_Myt* p);
	static void RemoveFromThread(IBitmap* pBmp);
	static void  LockResLoad();
	static void  UnlockResLoad();
	static int cmpGroupID(const void* p1,const void* p2);
	bool ThreadAddRes( IBitmap* pBmp,int nPri );
	bool ThreadAddRes(int group, int frame,int bmpIndex, int nPri);
	bool ThreadAddResGroup( int group, int exception,int nPri );
	bool ThreadAddResAni( int group, int frame,int exception,int nPri );
	bool ThreadAddResAni( IBitmaps* pBmps,int exception,int nPri );

	static bool InitResThread();
	static void DeleteResThread();
	static void ClearLoadQueue();

	void ThreadLoadAImage(size_t group,size_t frame);
	void ThreadLoadImage(DWORD* aImageID,size_t size);
	void ThreadLoadImage(IBitmaps** pBmps,size_t size);
	void ReleaseImage(DWORD* aImageID,size_t size);
	void ReleaseAImage(size_t group,size_t frame);
	
	/*void MarkGroupTime(DWORD dwGroupID,DWORD dwTime)
	{
		GetGroup(dwGroupID)->MarkTime(dwTime);
	}*/

	size_t ClearMemory();
	size_t ClearMemoryByTime(int group,int frame,int bmpIndex);
	static size_t ClearMemoryByTime(IBitmap* pBmp);
protected:
	std::vector<stGroupDataMap> m_groupDatas;
	stGroup*	m_groups;
	size_t		m_groupCount;

	File		m_file;
	std::string	m_strFileName;
	DWORD		m_flags;
	HANDLE		m_maphandle;
	DWORD		m_dwFileSize;

	//BYTE*		m_pMapData;
	File &		GetFile(){ return m_file; }
	HANDLE		GetMapHandle(){ return m_maphandle;}
	bool		MapFile();
	//bool UnMapFile();
	DWORD		m_dwID;


	stGroup * GetGroup(size_t nGroupID)
	{
		return (stGroup*)bsearch(&nGroupID,m_groups,m_groupCount,sizeof(stGroup),compareGroupID);
	}
public:
	bool IsMapView(){ return m_maphandle != NULL;}

	void BuildGroupDatas();
	
	const char* GetFileName() { return m_strFileName.c_str();}
	DWORD GetResourceID(){return m_dwID;}
	bool IsLoadByGroup(){ return (m_flags & flag_LoadByGroup) != 0;}
	bool IsFlag(DWORD flag) { return (m_flags & flag) != 0;}
	bool IsOpen() { return m_file.getStatus() != File::Closed;}
	bool Open();
	bool InitGroup(const char* groupName);

	struct stFileBlockInfo{
		DWORD dwOffset;
		DWORD dwSize;
	};
	typedef std::vector<stFileBlockInfo,std::stack_allocator<stFileBlockInfo> > t_aFileBlockInfo;

	static int __cdecl compareFileBlockInfo(const void *, const void *);
	static int __cdecl cmpGroupDataOffset(const void* p1,const void* p2);
	/*
	bool IsMapFileGroup(stGroup * pGroup)
	{
		return pGroup->m_mapFileDatas.size() != 0;
	}

	// 将一个组中的图片映射到内存中，此函数只能在主线程中执行！！！
	bool MapFileGroup(stGroup* pGroup)
	{
		//bool b1 = strstr(GetFileName(),"map1") != NULL;
		//if(b1) TRACE("GroupID:%d\n",pGroup->dwGroupID);
		SetFrameAllocator fa;
		t_aFileBlockInfo aFileBlockInfo;
		aFileBlockInfo.reserve(4096);

		// 将每个图片的文件偏移信息取出来，进行排序
		for( size_t i=0;i<pGroup->m_bitmapsCount;++i )
		{
			IBitmaps* pBitmaps = pGroup->m_bitmaps[i];
			if(pBitmaps)
			{
				stFileBlockInfo info;
				info.dwOffset = pBitmaps->GetDataOffset();
				info.dwSize = pBitmaps->GetDataSize();		

				//if(b1) TRACE("stFileBlockInfo:%d,%d,%d,%d\n",pBitmaps->GetGroup(),pBitmaps->GetFrame(),info.dwOffset,info.dwSize);
				aFileBlockInfo.push_back(info);
			}
		}

		qsort( &aFileBlockInfo[0], aFileBlockInfo.size(), sizeof(stFileBlockInfo), compareFileBlockInfo );

		// 将连续的区域整成一个块进行内存映射
		DWORD dwFileOffset = 0;
		DWORD dwBlockSize = 0;

		
		for(size_t i=0 ; i < aFileBlockInfo.size() ; ++i)
		{
			stFileBlockInfo* pBlock = &aFileBlockInfo[i];
			// 按内存分配颗粒度对齐
			DWORD dwOffset = pBlock->dwOffset / GetAllocationGranularity() * GetAllocationGranularity();
			DWORD dwSize = (pBlock->dwOffset + pBlock->dwSize - dwOffset + GetAllocationGranularity() - 1) / GetAllocationGranularity()
				* GetAllocationGranularity();
			if(i == 0)
			{
				dwFileOffset = dwOffset;
				dwBlockSize = dwSize;
			}
			else
			{
				if(dwFileOffset + dwBlockSize >= dwOffset)
				{
					dwBlockSize = dwOffset + dwSize - dwFileOffset;
				}
				else
				{
					stMapFileData mapFileData;
					
					//if(b1) TRACE("MapViewOfFile:%d,%d\n",dwFileOffset,dwBlockSize);
					mapFileData.pAddress = (BYTE*)MapViewOfFile(m_maphandle,FILE_MAP_READ,0,dwFileOffset,dwBlockSize);
					if(mapFileData.pAddress)
					{
						mapFileData.dwFileOffset = dwFileOffset;
						mapFileData.dwSize = dwBlockSize;
						pGroup->m_mapFileDatas.push_back(mapFileData);
						//TRACE("MapOfFile1: %s Group:%d [%x:%u]\n",GetFileName(),pGroup->dwGroupID,dwFileOffset,dwBlockSize);
					}
					else
					{
						DWORD dwError = GetLastError();
						AssertFatal(0,avar("MapViewOfFile function failed! code: %d",dwError));
						Engine_WriteLogF("MapViewOfFile failed! error code:%d\n",dwError);
						UnmapFileGroup(pGroup);
						return false;
					}

					dwFileOffset = dwOffset;
					dwBlockSize = dwSize;
				}
			}
		}
		if(dwBlockSize)
		{
			stMapFileData mapFileData;
			if(dwFileOffset + dwBlockSize > m_dwFileSize)
			{
				dwBlockSize = m_dwFileSize - dwFileOffset;
			}
			mapFileData.pAddress = (BYTE*)MapViewOfFile(m_maphandle,FILE_MAP_READ,0,dwFileOffset,dwBlockSize);
			if(mapFileData.pAddress)
			{
				mapFileData.dwFileOffset = dwFileOffset;
				mapFileData.dwSize = dwBlockSize;
				pGroup->m_mapFileDatas.push_back(mapFileData);
				//TRACE("MapOfFile2: %s Group:%d [%x:%u]\n",GetFileName(),pGroup->dwGroupID,dwFileOffset,dwBlockSize);
			}
			else
			{
				DWORD dwError = GetLastError();
				AssertFatal(0,avar("MapViewOfFile function failed! code: %d",dwError));
				Engine_WriteLogF("MapViewOfFile failed! error code:%d\n",dwError);
				UnmapFileGroup(pGroup);
				return false;
			}
		}
		return true;
	}
	
	void UnmapFileGroup(stGroup* pGroup)
	{
		for(size_t i=0;i<pGroup->m_mapFileDatas.size();++i)
		{
			if(!UnmapViewOfFile(pGroup->m_mapFileDatas[i].pAddress))
			{
				DWORD dwError = GetLastError();
				AssertFatal(0,avar("UnmapViewOfFile function failed! code: %d",dwError));
				Engine_WriteLogF("UnmapViewOfFile failed! error code:%d\n",dwError);
			}
		}
		pGroup->m_mapFileDatas.resize(0);
	}

	void UnmapFileGroup(DWORD dwGroupID)
	{
		UnmapFileGroup(GetGroup(dwGroupID));
	}
	static int __cdecl compareMapFileData( const void *, const void *);

	BYTE* GetFileDataAddress(stGroup* pGroup,DWORD dwFileOffset)
	{
		stMapFileData* p = (stMapFileData*)bsearch(&dwFileOffset,&pGroup->m_mapFileDatas[0],
			pGroup->m_mapFileDatas.size(),sizeof(stMapFileData),compareMapFileData);
		if(!p)
		{
			Assert(0);
			return NULL;
		}
		return p->pAddress + (dwFileOffset - p->dwFileOffset);
	}

	BYTE* GetFileDataAddress(DWORD dwGroupID,DWORD dwFileOffset)
	{
		stGroup * pGroup = GetGroup(dwGroupID);
		return GetFileDataAddress(pGroup,dwFileOffset);
	}
	//*/

	void UnmapFileGroupByTime(DWORD dwMaxTime);

	void UnmapAllBitmapData()
	{
		for(size_t i=0; i<m_groupCount;++i){
			stGroup& group = m_groups[i];
			for(size_t j=0; j<group.m_bitmapsCount;++j){
				TBitmaps<tBitmap> * pBitmaps = group.m_bitmaps[j];
				if(pBitmaps)
					pBitmaps->ReleaseMemory();
			}
		}
		UnmapFile();
	}

	void UnmapFile()
	{
		for(size_t i=0; i<m_groupDatas.size();++i){
			UnmapGroupData(&m_groupDatas[i]);
		}
	}

	/*bool	MapGroupData(stGroup * pGroup){
		if(pGroup->m_data == NULL){
			pGroup->m_data = (BYTE*)MapViewOfFile(m_maphandle,FILE_MAP_READ,0,pGroup->m_dataOffset,pGroup->m_dataSize);
			if(!pGroup->m_data){
				DWORD dwError = GetLastError();
				AssertFatal(0,avar("MapViewOfFile function failed! code: %d",dwError));
				return false;
			}
			return true;
		}
		return true;
	}

	void	UnmapGroupData(stGroup * pGroup){
		if(pGroup->m_data){
			UnmapViewOfFile(pGroup->m_data);
			pGroup ->m_data  = NULL;
		}
	}*/

	bool	MapGroupData(stGroup * pGroup){
		Assert(pGroup->m_dataMap);
		stGroupDataMap* pGroupData = pGroup->m_dataMap;

		if(pGroupData->m_data == NULL){
			pGroupData->m_data = (BYTE*)MapViewOfFile(m_maphandle,FILE_MAP_READ,0,pGroupData->m_dataOffset,pGroupData->m_dataSize);
			if(pGroupData->m_data == NULL){
				 DWORD dwError = GetLastError();

#if defined _DEBUG || defined RELEASE_TEST
				Engine_WriteLogF("MapViewOfFile function failed! offset:0x%x; size:%u, error code: %u\n",
					pGroupData->m_dataOffset,
					pGroupData->m_dataSize,
					dwError);
#endif
				if(dwError == ERROR_ACCESS_DENIED){
					// 没有足够的逻辑地址空间可用了，清空所有已经映射的地址，重试

					GetDevice()->UnmapAllBitmapData();
					if (m_groups != NULL) {
						free(m_groups);
						m_groups = NULL;
					}

					pGroupData->m_data = (BYTE*)MapViewOfFile(m_maphandle,FILE_MAP_READ,0,pGroupData->m_dataOffset,pGroupData->m_dataSize);

					if(pGroupData->m_data == NULL){
						DWORD dwError = GetLastError();
#if defined _DEBUG || defined RELEASE_TEST
						Engine_WriteLogF("MapViewOfFile function failed! offset:0x%x; size:%u, error code: %u\n",
							pGroupData->m_dataOffset,
							pGroupData->m_dataSize,
							dwError);
#endif
					}else{
						return true;
					}
				}
				///2025年1月17日03:43:09 记录这一刻 终于解决了
				//如果是因为映射时内存不足返回错误码8 则尝试清除所有已经映射的地址，重新映射 醉梦
				else if(dwError == ERROR_NOT_ENOUGH_MEMORY){
					GetDevice()->UnmapAllBitmapData();
					pGroupData->m_data = (BYTE*)MapViewOfFile(m_maphandle,FILE_MAP_READ,0,pGroupData->m_dataOffset,pGroupData->m_dataSize);

					if(pGroupData->m_data == NULL){
						DWORD dwError = GetLastError();
#if defined _DEBUG || defined RELEASE_TEST
						Engine_WriteLogF("MapViewOfFile function failed! offset:0x%x; size:%u, error code: %u\n",
							pGroupData->m_dataOffset,
							pGroupData->m_dataSize,
							dwError);
#endif
					}else{
						return true;
					}
				}
				//AssertFatal(0,avar("MapViewOfFile function failed! code: %d",dwError));
				return false;
			}
		}
		return true;
	}

	void	UnmapGroupData(stGroupDataMap * pGroupData){
		if(pGroupData->m_data){
			UnmapViewOfFile(pGroupData->m_data);
			pGroupData ->m_data  = NULL;
		}
	}

	/*
	stMapFileData* MapFileData(DWORD dwDataOffset,DWORD dwSize)
	{
		DWORD ChunkSize = ((1024 * 1024 * 1) / g_dwAllocationGranularity) * g_dwAllocationGranularity;
		DWORD begin = (dwDataOffset / ChunkSize) * ChunkSize;
		DWORD end = begin + ChunkSize;

		if(dwDataOffset + dwSize > end)
		{
			begin = (dwDataOffset / g_dwAllocationGranularity) * g_dwAllocationGranularity;
			end = ((dwDataOffset + dwSize + g_dwAllocationGranularity - 1)/g_dwAllocationGranularity) * g_dwAllocationGranularity;
		}
		DWORD dwMapViewSize = end - begin;
		if(end >= m_file.getSize())
		{
			//end = ((m_file.getSize() + g_dwAllocationGranularity -1) / g_dwAllocationGranularity) * g_dwAllocationGranularity;
			end = m_file.getSize();
			dwMapViewSize = end -begin;
			if(begin == 0) 
				dwMapViewSize = 0;
		}

		BYTE* pAddress = (BYTE*)MapViewOfFile(m_maphandle,FILE_MAP_READ,0,begin,dwMapViewSize);
		if(!pAddress)
		{
			DWORD dwError = GetLastError();
			AssertFatal(0,avar("MapViewOfFile function failed! code: %d",dwError));
			//Engine_WriteLogF("MapViewOfFile failed! error code:%d,%d:%d %s\n",dwError,begin,dwMapViewSize,GetFileName());
			return NULL;
		}

		//Engine_WriteLogF("MapViewOfFile ok! %d:%d %s\n",begin,dwMapViewSize,GetFileName());

		stMapFileData  * st = LOOKASIDE_ALLOC(m_allocMapFileData);

		st->pAddress = pAddress;
		st->dwFileOffset = begin;
		st->dwSize = end - begin;
		st->dwMarkTime = xtimeGetTime();

		m_mapFileDatas.resize(m_mapFileDatas.size() + 1);
		
		int i = m_mapFileDatas.size() - 2;
		while( i >= 0)
		{
			if( (m_mapFileDatas[i]->dwFileOffset > st->dwFileOffset) ||
				(m_mapFileDatas[i]->dwFileOffset == st->dwFileOffset && m_mapFileDatas[i]->dwSize > st->dwSize) )
			{
				m_mapFileDatas[i+1] = m_mapFileDatas[i];
				--i;
			}
			else break;
		}
		m_mapFileDatas[i+1] = st;
		return m_mapFileDatas[i+1];
	}
	//*/
private:
	struct stMapFileDataKey{
		DWORD dwDataOffset;
		DWORD dwDataSize;
	};

	static int  __cdecl compareMapFileData(const void * p1, const void * p2)
	{
		stMapFileDataKey * pKey = (stMapFileDataKey*)p1;
		stMapFileData ** pMapFileData = (stMapFileData**)p2;
		if( (*pMapFileData)->dwFileOffset <= pKey->dwDataOffset && 
			pKey->dwDataOffset + pKey->dwDataSize <= (*pMapFileData)->dwFileOffset + (*pMapFileData)->dwSize )
			return 0;
		if(pKey->dwDataOffset < (*pMapFileData)->dwFileOffset) return -1;
		return 1;
	}
	
	size_t _MapDataLoad(tBitmap* pBmp,bool bInThread)
	{
		stGroup * pGroup = GetGroup(pBmp->GetGroup());

		DWORD dwOffset = pBmp->GetDataOffset();
		DWORD dwSize = pBmp->GetDataSize();
		if(pGroup->m_dataOffset <= dwOffset && pGroup->m_dataSize + pGroup->m_dataOffset >= dwOffset + dwSize )
		{
			if(bInThread){
				if(!pGroup->isMaped()){
					Assert(0);
					return IBitmap::errorSize;
				}
			}else{
				if(!MapGroupData(pGroup)){
					Assert(0);
					return IBitmap::errorSize;
				}
			}

			BYTE* pAddress = pGroup->getData() + (dwOffset - pGroup->m_dataOffset);
			if(bInThread)
				return pBmp->_MapDataLoad(pAddress);
			else
				return pBmp->_MapData(pAddress);
		}else{
			Assert(0);
			return IBitmap::errorSize;
		}
	}

public:

	//*
	BYTE* GetFileDataAddress( DWORD dwDataOffset ,DWORD dwSize)
	{
		Assert(0);
		return NULL;
	}
	//*/

	/*
	size_t _MapDataLoad(stGroup* pGroup,int frame,int bmpIndex)
	{
		//TRACE("%s\t%d\t%d\t%d\n",GetFileName(),pGroup->dwGroupID,frame,bmpIndex);
		if(pGroup->m_bitmaps[frame])
		{
			_MyBitmaps & bitmaps = *pGroup->m_bitmaps[frame];
			tBitmap & bitmap = *(tBitmap*)bitmaps.GetBitmapByIndex(bmpIndex);

			BYTE* pAddress = GetFileDataAddress(pGroup,bitmap.GetDataOffset());
			if(!pAddress)
			{
				Assert(0);
				return IBitmap::errorSize;
			}

			size_t size = bitmap._MapDataLoad(pAddress);
			return size;
		}
		return 0;
	}
	//*/

	static size_t GetHeaderSizeGL(File& file,stGrpFileHeaderOld & hdr);
	static size_t GetHeaderSize(File& file,std::vector<stFileInfo> & aGroupNames,cGPackAttributes * attribute);

	IBitmap* LoadFrame(size_t group,size_t frame,size_t frameframe = 0);
	//bool LoadAGroup(int group,int pre_frame);
	bool LoadAFrame(size_t group,size_t frame,size_t bmpIndex);
	bool LoadAFrame(tBitmap* pBmp);
	//DWORD GetLastestTime(size_t iGroup, size_t iFrame);

	TBitmapList(LPCTSTR lpszFileName,DWORD flags,DWORD dwID);

	~TBitmapList(void);

	static int __cdecl compareGroupID( const void *, const void *); 


	
	size_t	 GetGroupID(size_t iGroup)
	{
		if(iGroup >= GetGroupCount()) return -1;
		return m_groups[iGroup].dwGroupID;
	}

	long	GetGroupIndex(long nGroupID)
	{
		stGroup* p = GetGroup(nGroupID);
		if(!p) return -1;
		return p - m_groups;
	}

	void	GetGroupRange(int & minGroup,int & maxGroup)
	{
		if(m_groupCount){
			minGroup = m_groups[0].dwGroupID;
			maxGroup = m_groups[m_groupCount-1].dwGroupID;
		}else{
			minGroup = -1;
			maxGroup = -1;
		}
	}

	size_t	  GetGroupCount() { return m_groupCount;}

	size_t	  GetFrameCount(size_t groupID)
	{ 
		stGroup * pGroup = GetGroup(groupID);
		if(pGroup) return pGroup->GetBitmapsCount(); 
		return 0;
	}

	size_t	  GetFrameFrameCount(size_t group,size_t frame)
	{
		_MyBitmaps* pBmps = GetBitmaps(group,frame);
		if(pBmps) return pBmps->GetFrameCount();
		return 0;
	}

	_MyBitmaps* GetBitmaps(size_t group,size_t frame)
	{
		stGroup * pGroup = GetGroup(group);
		if(pGroup)
		{
			if(frame < pGroup->GetBitmapsCount() && pGroup->m_bitmaps[frame])
				return pGroup->m_bitmaps[frame];
		}
		return NULL;
	}

	bool IsExist (size_t group,size_t frame)
	{
		IBitmap* p = GetBitmap(group,frame);
		if(!p) return false;
		return !p->IsEmpty();
	}

	bool	IsExistGroup(int groupID)
	{
		return GetGroup(groupID) != NULL;
	}

	static size_t GetUseMemorySize();
};

//*
template <class tBitmap>
TBitmapList<tBitmap> * TBitmapList<tBitmap>::g_pRemove = NULL;

template <class tBitmap>
IBitmap* TBitmapList<tBitmap>::g_pRemoveBmp = NULL;

template <class tBitmap>
DWORD TBitmapList<tBitmap>::g_dwAllocationGranularity = 0;

template <class tBitmap>
size_t TBitmapList<tBitmap>::s_dwMemorySize = 0;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
template <class tBitmap>
DWORD TBitmapList<tBitmap>::GetAllocationGranularity()
{
	return TBitmapList<tBitmap>::g_dwAllocationGranularity;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
template <class tBitmap>
size_t TBitmapList<tBitmap>::GetUseMemorySize()
{
	return TBitmapList<tBitmap>::s_dwMemorySize;
}

template <class tBitmap>
cSafeStack< stLoadResT > TBitmapList<tBitmap>::g_queue;

//优先级不高的
template <class tBitmap>
cSafeQueue< stLoadResT > TBitmapList<tBitmap>::g_queueLower;

//优先级不高的
template <class tBitmap>
bool TBitmapList<tBitmap>::s_bQueueLowerEmpty = true;

template <class tBitmap>
bool	TBitmapList<tBitmap>::s_bExitThread = false;

template <class tBitmap>
bool	TBitmapList<tBitmap>::s_bPauseThreadLoad = false;

template <class tBitmap>
std::vector<stLoadResT >  TBitmapList<tBitmap>::g_loaded;

template <class tBitmap>
CLock TBitmapList<tBitmap>::s_lockLoaded;

template <class tBitmap>
CLock TBitmapList<tBitmap>::s_lockLoadThread; 

template <class tBitmap>
CLock TBitmapList<tBitmap>::s_lockLoading;

template <class tBitmap>
HANDLE TBitmapList<tBitmap>::g_hResThread = NULL;

template <class tBitmap>
fnVirtualLock TBitmapList<tBitmap>::g_fnVirtualLock = NULL;

template <class tBitmap>
fnVirtualUnlock TBitmapList<tBitmap>::g_fnVirtualUnlock = NULL;

template <class tBitmap>
size_t TBitmapList<tBitmap>::g_dwMemoryPageSize = 4096;

//*/

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
template < class tBitmap>
TBitmapList<tBitmap>::TBitmapList(LPCTSTR lpszFileName,DWORD flags,DWORD dwID)
: m_strFileName(lpszFileName)
, m_flags(flags)
, m_maphandle(NULL)
//, m_pMapData(NULL)
, m_dwID(dwID)
, m_groupCount(0)
, m_groups(NULL)
, m_dwFileSize(0)
{
	static BOOL bWin98 = 2;
	if(bWin98==2)
	{
		DWORD dwVersion = GetVersion();
		DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
		DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
		if (dwVersion < 0x80000000) 
		{
			bWin98 = 0;
			//Engine_WriteLogF("WinNT System!\n");
		}
		else
		{
			bWin98 = 1;
			//Engine_WriteLogF("Win32 System!\n");
		}
	}
	if(bWin98)
	{
		m_flags &= (~flag_MapView); // Win98
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
template < class tBitmap>
TBitmapList<tBitmap>::~TBitmapList(void)
{
	if(m_maphandle)
	{
		UnmapFile();
		//UnmapViewOfFile(m_pMapData);
		CloseHandle(m_maphandle);
	}
	m_file.close();
#ifdef _DEBUG
	if(m_groups){
		gBitmapHeaderSize -= _msize(m_groups);

		TRACE("===gBitmapHeaderSize %d\n",gBitmapHeaderSize);
	}
#endif

	destructInPlaceCount(m_groups,GetGroupCount());
	free(m_groups);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param p1 : 描述
 * \param p2 : 描述
 * \return 返回值的描述
 */
/*
template < class tBitmap>
int  __cdecl TBitmapList<tBitmap>::compareMapFileData( const void * p1, const void * p2)
{
	DWORD dwFileOffset = *(DWORD*)p1;
	stMapFileData *pMapFileData = (stMapFileData*)p2;
	if(pMapFileData->dwFileOffset <= dwFileOffset && dwFileOffset <= pMapFileData->dwFileOffset + pMapFileData->dwSize)
		return 0;
	if(dwFileOffset < pMapFileData->dwFileOffset) return -1;
	return 1;
}
//*/

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param key : 描述
 * \param pGroup : 描述
 * \return 返回值的描述
 */
template < class tBitmap>
int __cdecl TBitmapList<tBitmap>::compareGroupID( const void *key, const void *pGroup)
{
	DWORD dwGroupID = *(DWORD*)key;
	stGroup * pTheGroup = (stGroup*)pGroup;
	return (int)dwGroupID - (int)pTheGroup->dwGroupID;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param p1 : 描述
 * \param p2 : 描述
 * \return 返回值的描述
 */
template < class tBitmap>
int __cdecl TBitmapList<tBitmap>::compareFileBlockInfo(const void *p1, const void *p2)
{
	stFileBlockInfo* pi1 = (stFileBlockInfo*)p1;
	stFileBlockInfo* pi2 = (stFileBlockInfo*)p2;
	return (long)pi1->dwOffset - (long)pi2->dwOffset;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param file : 描述
 * \param hdr : 描述
 * \return 返回值的描述
 */
//*
template < class tBitmap>
size_t TBitmapList<tBitmap>::GetHeaderSizeGL(File& file,stGrpFileHeaderOld & hdr)
{
	size_t nHeaderSize = 0;

	DWORD pos = file.getPosition();
	nHeaderSize = hdr.wdDirCount * sizeof( TBitmapList<tBitmap>::stGroup );

	for(size_t i=0;i<hdr.wdDirCount;++i)
	{
		// 遍历每个Group
		DWORD dwFileNum;
		DWORD dwGroupID;

		file.read(&dwGroupID,sizeof(DWORD));
		file.read(&dwFileNum,sizeof(DWORD));

		nHeaderSize += sizeof(IBitmaps*) * dwFileNum;
		for( size_t j=0 ; j<dwFileNum ; ++j )
		{
			// 遍历每个动画
			stGrpAniInfoOld aniHdr;
			file.read(&aniHdr,sizeof(aniHdr));
			if(aniHdr.location.dwFileSize)
				nHeaderSize += sizeof(TBitmaps<tBitmap>);
		}
	}

	file.setPosition(pos);
	return nHeaderSize;
}
//*/

template < class tBitmap>
size_t TBitmapList<tBitmap>::GetHeaderSize(File& file,std::vector<stFileInfo> & aGroupNames,cGPackAttributes * attribute)
{
	size_t nHeaderSize = 0;

	DWORD pos = file.getPosition();
	nHeaderSize = aGroupNames.size() * sizeof( TBitmapList<tBitmap>::stGroup );

	for(size_t i=0; i<aGroupNames.size(); ++i)
	{
		// 遍历每个Group
		stFileInfo & info = aGroupNames[i];

		if(attribute->setGroup(atol(info.szFileName),info.crc)){
			const stGLHeader * pHdr = attribute->getGLHeader();
			const stGLHeader &glHdr = *pHdr;

			if(strncmp((char*)&glHdr.dwMagic,GRP_FILE_MAGIC,4) || glHdr.dwVersion != c_GrpVersion)
				return -1;

			nHeaderSize += sizeof(IBitmaps*) * glHdr.dwPictureNum;
			for( size_t j=0 ; j<glHdr.dwPictureNum ; ++j )
			{
				// 遍历每个动画
				attribute->setAni(j);
				const stGrpAniInfo &aniHdr = * attribute->getAniInfo();
				if(aniHdr.location.dwFileSize){
					nHeaderSize += sizeof(TBitmaps<tBitmap>);
					nHeaderSize += TBitmaps<tBitmap>::GetAddinDataSize((stPackAniHeader*)attribute->getBitmapInfo());
				}
			}
		}else{
			
#if 0
			file.setPosition(info.offsetInPack);
			stGLHeader glHdr;
			file.read(&glHdr,sizeof(glHdr));
			if(strncmp((char*)&glHdr.dwMagic,GRP_FILE_MAGIC,4) || glHdr.dwVersion != c_GrpVersion)
				return -1;
			nHeaderSize += sizeof(IBitmaps*) * glHdr.dwPictureNum;
			file.setPosition(glHdr.dwHeaderDataOffset+info.offsetInPack);
			for( size_t j=0 ; j<glHdr.dwPictureNum ; ++j )
			{
				// 遍历每个动画
				stGrpAniInfo aniHdr;
				file.read(&aniHdr,sizeof(aniHdr));
				if(aniHdr.location.dwFileSize)
					nHeaderSize += sizeof(TBitmaps<tBitmap>);
			}
#endif

		}
	}

	file.setPosition(pos);
	return nHeaderSize;
}


inline bool __cdecl filterGLResFile(const char* filename){
	const char* p = strrchr(filename, '.');
	if(!p) return false;
	return stricmp(p,".grp") == 0;
}

template < class tBitmap>
bool TBitmapList<tBitmap>::InitGroup(const char* groupName)
{
}

template < class tBitmap>
int TBitmapList<tBitmap>::cmpGroupID(const void* p1,const void* p2)
{
	stGroup * g1 = (stGroup*)p1;
	stGroup * g2 = (stGroup*)p2;
	return (long)g1->dwGroupID - (long)g2->dwGroupID;
}

inline int cmpGroupID2(const void* p1,const void* p2)
{
	stGLGroupInfo * g1 = (stGLGroupInfo*)p1;
	stGLGroupInfo * g2 = (stGLGroupInfo*)p2;
	return (long)g1->id - (long)g2->id;
}

template < class tBitmap>
int __cdecl TBitmapList<tBitmap>::cmpGroupDataOffset(const void* p1,const void* p2)
{
	stGroup * g1 = *(stGroup**)p1;
	stGroup * g2 = *(stGroup**)p2;
	//return (long)(g1->m_dataOffset + g1->m_dataSize) - (long)(g2->m_dataOffset + g2->m_dataSize);
	return (long)g1->m_dataOffset - (long)g2->m_dataOffset;
}

template < class tBitmap>
void TBitmapList<tBitmap>::BuildGroupDatas()
{
	SetFrameAllocator fa;
	std::vector<stGroup*,std::stack_allocator<stGroup*> > aGroups;
	std::vector<size_t , std::stack_allocator<size_t> > aGroupDataIndex;
	aGroupDataIndex.resize(GetGroupCount());
	aGroups.resize(GetGroupCount());
	for(size_t i=0; i<aGroups.size(); ++i){
		aGroups[i] = &m_groups[i];
	}

	qsort(&aGroups[0],aGroups.size(),sizeof(stGroup*),cmpGroupDataOffset);

	size_t minMapSize = (MIN_MAP_SIZE / g_dwAllocationGranularity) * g_dwAllocationGranularity;

	for(size_t i=0; i<aGroups.size();){
		m_groupDatas.resize(m_groupDatas.size()+1);
		aGroupDataIndex[i] = m_groupDatas.size()-1;

		stGroup* pGroup = aGroups[i];
		size_t offset = (pGroup->m_dataOffset / g_dwAllocationGranularity) * g_dwAllocationGranularity;
		size_t size = (pGroup->m_dataSize + (pGroup->m_dataOffset - offset));
		++i;
		while(size < minMapSize && i < aGroups.size()){
			aGroupDataIndex[i] = m_groupDatas.size()-1;

			stGroup* pGroup = aGroups[i];
			size = pGroup->m_dataSize + (pGroup->m_dataOffset - offset);
			++i;
		}

		stGroupDataMap & groupData =  m_groupDatas.back();
		groupData.m_data = NULL;
		groupData.m_dataOffset = offset;
		groupData.m_dataSize = size;
	}

	for(size_t i=0; i<aGroups.size();++i){
		aGroups[i]->m_dataMap = &m_groupDatas[aGroupDataIndex[i]];
	}
}

template < class tBitmap>
bool TBitmapList<tBitmap>::Open()
{
	if(File::Ok != m_file.open( m_strFileName.c_str() , File::Read  ))
	{
		TRACE("OpenFile %s failed!\n",GetFileName());
		return false;
	}

	DWORD ullSize = m_file.getSize();
	m_dwFileSize = (DWORD)ullSize;

	const char* pszFileName = m_strFileName.c_str();
	const char * pszResFileName = pszFileName;
	
	{
		char szFileName[MAX_PATH];
		strcpy(szFileName,pszResFileName);
		char* p = strrchr(szFileName,'.');
		strcpy(p,".ghd");
		if(IsFileExist(szFileName)){
			DeleteFile(szFileName);
		}
	}

	stFileInfo attritueInfo;
	memset(&attritueInfo,0,sizeof(attritueInfo));
	std::vector<stFileInfo> aGroupNames;
	if(!searchFileInPack(pszResFileName,aGroupNames,&attritueInfo,filterGLResFile))
		return false;
	
	cGPackAttributes attribute(&m_file,&attritueInfo);

	if(attritueInfo.szFileName[0] == 0){
		TRACE("======> 图包 %s 没有 attributes 文件\n",m_strFileName.c_str());
	}

	size_t nHeaderSize = GetHeaderSize(m_file,aGroupNames,&attribute);
	if(nHeaderSize == -1){
		TRACE("OpenFile %s format failed!\n",m_strFileName.c_str());
		return false;
	}

	destructInPlaceCount(m_groups,GetGroupCount());

	free(m_groups);

	m_groupCount = 0;

#ifdef _DEBUG
	gBitmapHeaderSize += nHeaderSize;
	
	TRACE("===gBitmapHeaderSize %d\n",gBitmapHeaderSize);
#endif

	m_groups = (stGroup*)malloc(nHeaderSize);
	BYTE* pBuffer = (BYTE*)m_groups;
	m_groupCount = aGroupNames.size();

	constructInPlaceCount(m_groups,m_groupCount);

	pBuffer += sizeof(stGroup) * m_groupCount;
	Assert( (size_t)(pBuffer - (BYTE*)m_groups) <= (size_t)nHeaderSize );

	for(size_t i=0; i< aGroupNames.size(); ++i)
	{
		// 遍历每个Group
		stFileInfo & info = aGroupNames[i];

		DWORD dwGroupID = atol(info.szFileName);

		if(attribute.setGroup(dwGroupID,info.crc)){
			const stGLHeader &glHdr = *attribute.getGLHeader();

			DWORD dwFileNum = glHdr.dwPictureNum;

			stGroup & aBlock = m_groups[i];
			aBlock.dwGroupID = dwGroupID;
			aBlock.m_bitmaps = (TBitmaps<tBitmap>* *)pBuffer;
			aBlock.m_bitmapsCount = dwFileNum;

			pBuffer += sizeof( IBitmaps * ) * dwFileNum;
			Assert( (size_t)(pBuffer - (BYTE*)m_groups) <= (size_t)nHeaderSize );

			aBlock.m_aniBuf = (TBitmaps<tBitmap>*)pBuffer;
			TBitmaps<tBitmap> *  pAni = aBlock.m_aniBuf;

			{
				aBlock.m_dataOffset = info.offsetInPack;
				aBlock.m_dataSize = info.size;
				aBlock.m_dataMap = NULL;
			}

			for( size_t j=0 ; j < dwFileNum ; ++j )
			{
				// 遍历每个动画
				attribute.setAni(j);
				const stGrpAniInfo & aniHdr = *attribute.getAniInfo();

				if( aniHdr.location.dwFileSize ) {
					aBlock.m_bitmaps[j] = pAni;
					constructInPlace(pAni);
					pAni->Init(this,dwGroupID,j,aniHdr.location.dwFileOffset + sizeof(stGLHeader)+info.offsetInPack,aniHdr.location.dwFileSize);
					pBuffer += sizeof(TBitmaps<tBitmap>);
					pAni++;
					Assert( (size_t)(pBuffer - (BYTE*)m_groups) <= (size_t)nHeaderSize );
				}
				else
					aBlock.m_bitmaps[j] = NULL;
			}
			
			for( size_t j=0 ; j < dwFileNum ; ++j )
			{
				// 遍历每个动画
				attribute.setAni(j);
				const stGrpAniInfo & aniHdr = *attribute.getAniInfo();

				if( aniHdr.location.dwFileSize ) {
					aBlock.m_bitmaps[j]->InitBitmap(attribute.getBitmapInfo(),pBuffer);
					Assert( (size_t)(pBuffer - (BYTE*)m_groups) <= (size_t)nHeaderSize );
				}
			}

		}else{

			TRACE("======> 图包 %s 的 attributes 文件不完整!\n",m_strFileName.c_str());

			stGroup & aBlock = m_groups[i];
			aBlock.dwGroupID = dwGroupID;
			aBlock.m_bitmaps = NULL;
			aBlock.m_aniBuf = NULL;
			aBlock.m_bitmapsCount = 0;
			aBlock.m_dataOffset = 0;
			aBlock.m_dataSize = 0;
			aBlock.m_dataMap = NULL;

#if 0
			m_file.setPosition(info.offsetInPack);

			stGLHeader glHdr;
			m_file.read(&glHdr,sizeof(glHdr));

			DWORD dwFileNum = glHdr.dwPictureNum;

			stGroup & aBlock = m_groups[i];
			aBlock.dwGroupID = dwGroupID;
			aBlock.m_bitmaps = (TBitmaps<tBitmap>* *)pBuffer;
			aBlock.m_bitmapsCount = dwFileNum;

			pBuffer += sizeof( IBitmaps * ) * dwFileNum;
			Assert( (size_t)(pBuffer - (BYTE*)m_groups) <= (size_t)nHeaderSize );

			aBlock.m_aniBuf = (TBitmaps<tBitmap>*)pBuffer;
			TBitmaps<tBitmap> *  pAni = aBlock.m_aniBuf;

			{
				aBlock.m_dataOffset = info.offsetInPack;
				aBlock.m_dataSize = info.size;
				aBlock.m_dataMap = NULL;
			}

			m_file.setPosition(glHdr.dwHeaderDataOffset + info.offsetInPack);

			for( size_t j=0 ; j < dwFileNum ; ++j )
			{
				// 遍历每个动画
				stGrpAniInfo aniHdr;

				m_file.read(&aniHdr,sizeof(aniHdr));

				if( aniHdr.location.dwFileSize ) 
				{
					aBlock.m_bitmaps[j] = pAni;
					constructInPlace(pAni);
					pAni->Init(this,dwGroupID,j,aniHdr.location.dwFileOffset + sizeof(stGLHeader)+info.offsetInPack,aniHdr.location.dwFileSize,-1);
					pAni++;
					pBuffer += sizeof(TBitmaps<tBitmap>);
					Assert( (size_t)(pBuffer - (BYTE*)m_groups) <= (size_t)nHeaderSize );
				}
				else
				{
					aBlock.m_bitmaps[j] = NULL;
				}
			}
#endif
		}
	}
	Assert( (size_t)(pBuffer - (BYTE*)m_groups) == (size_t)nHeaderSize );
	qsort(&m_groups[0],m_groupCount,sizeof(stGroup),cmpGroupID);

	if(IsFlag(flag_MapView))
	{
		if(!MapFile()) 
		{
			m_file.close();
			return false;
		}
		BuildGroupDatas();
	}
	else if(!IsFlag(flag_AlwaysOpen))
	{
		m_file.close();
	}

	return true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
template < class tBitmap>
bool TBitmapList<tBitmap>::MapFile()
{
	char *szName = NULL;
	if(m_maphandle == NULL){
		m_maphandle = CreateFileMapping((HANDLE)(m_file.getHandle()), NULL, PAGE_READONLY, 0, 0, szName);
		if(m_maphandle == NULL) 
		{
			DWORD dwErr = GetLastError();
			AssertFatal(0,avar("CreateFileMapping  failed! file[%s] code:%u\n",GetFileName(),dwErr));
			appDump << "CreateFileMapping function failed! file[" << GetFileName() << "] code:" << dwErr << "\n";
			return false;
		}
	}

	/*
	m_pMapData = (BYTE*)::MapViewOfFile(m_maphandle, FILE_MAP_READ, 0, 0, 0);
	if(m_pMapData == NULL)
	{
		DWORD dwErr = GetLastError();
		TRACE("CreateFileMapping  failed! file[%s] code:%u\n",GetFileName(),dwErr);

		CloseHandle(m_maphandle);
		m_maphandle = NULL;
		return false;
	}
	//*/
	return true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
//template < class tBitmap>
//bool TBitmapList<tBitmap>::UnMapFile()
//{
//	if(m_pMapData)
//	{
//		UnmapViewOfFile(m_pMapData);
//		m_pMapData = NULL;
//	}
//	if(m_maphandle)
//	{
//		CloseHandle(m_maphandle);
//		m_maphandle = NULL;
//	}
//}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param group : 描述
 * \param frame : 描述
 * \param frameframe : 描述
 * \return 返回值的描述
 */
template < class tBitmap>
IBitmap* TBitmapList<tBitmap>::LoadFrame(size_t group,size_t frame,size_t frameframe )
{
	TBitmapList<tBitmap>::_MyBitmaps * pBitmaps = GetBitmaps(group,frame);
	if(!pBitmaps) return NULL;

	size_t bmpIndex = pBitmaps->GetFrameIndex(frameframe);
	if(bmpIndex == -1) return NULL;

	tBitmap* pBmp = (tBitmap*)pBitmaps->GetBitmapByIndex(bmpIndex);

	if(pBmp->IsNull())
	{
		return NULL;
	}

	if (pBmp->IsLoad())
		return pBmp;

	if( IsFlag(flag_MapView) )
	{
		if( m_maphandle == NULL )
		{
			if(!MapFile()) return NULL;
		}
		size_t size = _MapDataLoad(pBmp,false);
		if(size == IBitmap::errorSize)
			return NULL;
	}

	if( IsFlag( flag_LoadByThread ) )
	{
		// 在另一个线程中，加载到物理内存
		if( IsLoadByGroup() )
		{
			if(GetFrameFrameCount(group,frame) > 1)
				ThreadAddResAni(group,frame,bmpIndex,0);
			else
				ThreadAddResGroup(group,frame,0);
		}
		else
		{
			ThreadAddResAni(group,frame,bmpIndex,0);
			//ThreadAddRes(this,group,frame,bmpIndex,0);
		}

		if( !IsFlag(flag_MapView) )
		{
			stLoadRes load;
			load.pBmp = pBmp;
			Load(load);
			/*
			DWORD dwTime = xtimeGetTime();
			while( !pBmp->IsLoad() && xtimeGetTime() - dwTime < 500)
			{
				if(pBmp->IsLoadError())
				{
					return NULL;
				}
				Sleep(1);
			}
			//*/
			if(pBmp->IsLoadError())
			{
				return NULL;
			}
			//Assert(pBmp != NULL);
		}
		return pBmp;
	}

	if (!LoadAFrame(group,frame,bmpIndex))
		return NULL;

	return pBmp;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pBmp : 描述
 * \return 返回值的描述
 */
template < class tBitmap>
bool TBitmapList<tBitmap>::LoadAFrame(tBitmap* pBmp)
{
	if ( pBmp->IsLoad() )
		return false;

	if( IsFlag(flag_MapView) )
	{
		//加载到物理内存
		if(m_maphandle == NULL)
		{
			pBmp->SetLoad( Load_LoadError );
			return true;
		}

		size_t size = _MapDataLoad(pBmp,true);

		if( size != IBitmap::errorSize )
		{
			s_dwMemorySize += size;
			pBmp->SetLoad(Load_Loaded);
		}
		return true;
	}

	File* fp;
	File file;
	if(IsFlag(flag_AlwaysOpen)) 
		fp = &m_file;
	else
		fp = &file;

	if(fp->getStatus() == File::Closed)
	{
		if(fp->open(m_strFileName.c_str(),File::Read) != File::Ok)
			goto _error;
	}

	size_t size = pBmp->_LoadBitmap(*fp);
	if(size == IBitmap::errorSize)
		goto _error;

	s_dwMemorySize += size;

	pBmp->SetLoad( Load_Loaded );
	return true;
_error:
	pBmp->SetLoad( Load_LoadError );
	return false;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param iGroup : 描述
 * \param iFrame : 描述
 * \param bmpIndex : 描述
 * \return 返回值的描述
 */
template < class tBitmap>
bool TBitmapList<tBitmap>::LoadAFrame(size_t iGroup,size_t iFrame,size_t bmpIndex)
{	
	_MyBitmaps* pFrame = ( _MyBitmaps* )GetBitmaps(iGroup,iFrame);
	tBitmap* pBmp = (tBitmap*)pFrame->GetBitmapByIndex(bmpIndex);
	if ( pBmp->IsLoad() )
		return false;

	if( IsFlag(flag_MapView) )
	{
		//加载到物理内存
		if(m_maphandle == NULL){
			pBmp->SetLoad( Load_LoadError );
			return true;
		}

		size_t size = _MapDataLoad(pBmp,true);

		if( size != IBitmap::errorSize )
		{
			s_dwMemorySize += size;
			pBmp->SetLoad(Load_Loaded);
		}
		return true;
	}

	File* fp;
	File file;
	if(IsFlag(flag_AlwaysOpen)) 
		fp = &m_file;
	else
		fp = &file;

	if(fp->getStatus() == File::Closed)
	{
		if(fp->open(m_strFileName.c_str(),File::Read) != File::Ok)
			goto _error;
	}

	size_t size = pBmp->_LoadBitmap(*fp);
	if(size == IBitmap::errorSize)
		goto _error;

	s_dwMemorySize += size;

	pBmp->SetLoad( Load_Loaded );
	return true;
_error:
	pBmp->SetLoad( Load_LoadError );
	return false;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
template < class tBitmap>
size_t TBitmapList<tBitmap>::ClearMemory()
{
	int size = 0;
	for (size_t i=0; i < GetGroupCount(); i++)
	{
		stGroup& group = m_groups[i];
		for (size_t j=0; j < group.GetBitmapsCount(); j++)			
		{
			if(group.m_bitmaps[j])
			{
				_MyBitmaps& frame = *group.m_bitmaps[j];
				size += frame.ReleaseMemory();
			}
		}
	}
	return size;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pBmp : 描述
 * \return 返回值的描述
 */
template < class tBitmap>
size_t TBitmapList<tBitmap>::ClearMemoryByTime( IBitmap* pBmp)
{
	DWORD time = xtimeGetTime();
	size_t size = -1;

	if (pBmp->IsLoad() && time - pBmp->GetLastRenderTime() >= c_nLongFrame)
	{
		size = pBmp->ReleaseMemory();
		pBmp->SetLoad(Load_NotLoad);
	}

	return size;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nGroup : 描述
 * \param nFrame : 描述
 * \param bmpIndex : 描述
 * \return 返回值的描述
 */
template < class tBitmap>
size_t TBitmapList<tBitmap>::ClearMemoryByTime( int nGroup,int nFrame,int bmpIndex )
{
	
	stGroup* pGroup = GetGroup(nGroup);

	if(pGroup->m_bitmaps[nFrame])
	{
		_MyBitmaps& frame = *pGroup->m_bitmaps[nFrame];
		tBitmap* pBmp = (tBitmap*)frame.GetBitmapByIndex(bmpIndex);

		return ClearMemoryByTime(pBmp);	
	}
	return 0;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param iGroup : 描述
 * \param iFrame : 描述
 * \return 返回值的描述
 */
/*
template < class tBitmap>
DWORD TBitmapList<tBitmap>::GetLastestTime(size_t iGroup, size_t iFrame)
{
	stGroup* pGroup = GetGroup(iGroup);
	DWORD time = -1;
	if(pGroup)
	{
		time = pGroup->MarkTime();
		_MyBitmaps* pFrame = GetBitmaps(iGroup,iFrame);
		if (pFrame)
			time = pFrame->MarkTime();
	}
	return time;
}
//*/

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
#pragma optimize( "", off )
template<class tBitmap>
void TBitmapList<tBitmap>::GetVirtualMemoryFnAddress()
{
	SYSTEM_INFO sysInfo;
	memset(&sysInfo,0,sizeof(sysInfo));
	GetSystemInfo(&sysInfo);
	g_dwMemoryPageSize = sysInfo.dwPageSize;
	g_dwAllocationGranularity = sysInfo.dwAllocationGranularity;
	TRACE("AllocationGranularity : %u\n",g_dwAllocationGranularity);
	HMODULE hdll = GetModuleHandle("KERNEL32.DLL");
	if(hdll)
	{
		g_fnVirtualLock = (fnVirtualLock) ::GetProcAddress(hdll,"VirtualLock");
		g_fnVirtualUnlock = (fnVirtualUnlock) ::GetProcAddress(hdll,"VirtualUnlock");
	}
}
#pragma optimize( "", on )


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
template<class tBitmap>
bool TBitmapList<tBitmap>::InitResThread()
{
	GetVirtualMemoryFnAddress();
	g_hResThread = (HANDLE)_beginthreadex(NULL,0,ResLoadThread,NULL,0,NULL);
	return (g_hResThread != NULL);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
template<class tBitmap>
void TBitmapList<tBitmap>::DeleteResThread()
{
	if(s_bExitThread) return;
	s_bExitThread = true;

	if(g_hResThread)
	{
		WaitForSingleObject(g_hResThread,INFINITE);
		CloseHandle(g_hResThread);
		g_hResThread= NULL;
	}
}

//template<class tBitmap>
//bool TBitmapList<tBitmap>::ThreadAddRes2(TBitmapList<tBitmap>* p,int group, int frame, int nPri)
//{
//	DWORD time = p->GetLastestTime(group,frame);
//	if(time == (DWORD)-1) return false;
//
//	DWORD c_dwTime = 30000;
//	if(xtimeGetTime() - time < c_dwTime)
//		return false;
//
//	stLoadRes st;
//	st.pBmpList = p;
//	st.group = group;
//	st.frame = frame;
//	
//	p->MarkTime(xtimeGetTime(),group,frame);
//	if (nPri == 0)
//		g_queue.Push(st);
//	else
//		g_queueLower.Push(st);
//	
//	return true;
//}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param group : 描述
 * \param exception : 描述
 * \param nPri : 描述
 * \return 返回值的描述
 */
template<class tBitmap>
bool TBitmapList<tBitmap>::ThreadAddResGroup( int group, int exception,int nPri )
{
	TBitmapList<tBitmap>::stGroup * pGroup = GetGroup(group);
	Assert(pGroup);
	for(size_t i=0;i<pGroup->GetBitmapsCount();++i)
	{
		if( i != exception)
		{
			ThreadAddResAni(group,i,-1,nPri);
		}
	}
	if(exception != -1)
		ThreadAddResAni(group,exception,-1,nPri);
	return true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param group : 描述
 * \param frame : 描述
 * \param exception : 描述
 * \param nPri : 描述
 * \return 返回值的描述
 */
template<class tBitmap>
bool TBitmapList<tBitmap>::ThreadAddResAni( int group, int frame,int exception,int nPri )
{
	_MyBitmaps * pBmps = GetBitmaps( group,frame );
	if(!pBmps) return true;
	for(size_t i = 0;i<pBmps->GetBitmapCount();++i)
	{
		if(i != exception)
		{
			ThreadAddRes(group,frame,i,nPri);
		}
	}
	if(exception != -1)
		ThreadAddRes(group,frame,exception,nPri);
	return true;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param group : 描述
* \param frame : 描述
* \param exception : 描述
* \param nPri : 描述
* \return 返回值的描述
*/
template<class tBitmap>
bool TBitmapList<tBitmap>::ThreadAddResAni( IBitmaps* pBmps,int exception,int nPri )
{
	for(size_t i = 0;i<pBmps->GetBitmapCount();++i)
	{
		if(i != exception)
			ThreadAddRes(pBmps->GetBitmapByIndex(i),nPri);
	}
	
	if(exception != -1)
		ThreadAddRes(pBmps->GetBitmapByIndex(exception),nPri);

	return true;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param group : 描述
* \param frame : 描述
* \param bmpIndex : 描述
* \param nPri : 描述
* \return 返回值的描述
*/
template<class tBitmap>
bool TBitmapList<tBitmap>::ThreadAddRes( IBitmap* pBmp,int nPri )
{
	if(pBmp->IsLoad()) 
		return true;

	if(pBmp->IsPreload()) 
		return true;

	stLoadRes st;

	st.pBmp = pBmp;

	if(IsFlag(IBitmapList::flag_MapView))
		_MapDataLoad((tBitmap*)pBmp,false);

	if ( nPri == 0 )
		g_queue.Push(st);
	else
		g_queueLower.Push(st);

	pBmp->SetLoad(Load_PreLoad);

	return true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param group : 描述
 * \param frame : 描述
 * \param bmpIndex : 描述
 * \param nPri : 描述
 * \return 返回值的描述
 */
template<class tBitmap>
bool TBitmapList<tBitmap>::ThreadAddRes( int group, int frame,int bmpIndex, int nPri )
{
	_MyBitmaps * pBmps = GetBitmaps( group,frame );
	if(!pBmps) 
		return true;

	tBitmap * pBmp = (tBitmap*)pBmps->GetBitmapByIndex(bmpIndex);
	Assert(pBmp);

	return ThreadAddRes(pBmp,nPri);

}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param group : 描述
 * \param frame : 描述
 * \return 返回值的描述
 */
template<class tBitmap>
void TBitmapList<tBitmap>::ThreadLoadAImage(size_t group,size_t frame)
{
	TBitmapList<tBitmap>::stGroup* pGroup = GetGroup(group);
	if(!pGroup) return;

	if(pGroup->GetBitmapsCount() <= frame) 
		return;

	if( IsFlag(flag_MapView) )
	{
		if( m_maphandle == NULL )
		{
			if(!MapFile()) return;
		}
	}

	ThreadAddResAni(group,frame,-1,1);

	s_bQueueLowerEmpty  = false;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param aImageID : 描述
* \param size : 描述
* \return 返回值的描述
*/
template<class tBitmap>
void TBitmapList<tBitmap>::ThreadLoadImage(IBitmaps** pBmps,size_t size)
{
	if( IsFlag(flag_MapView) )
	{
		if( m_maphandle == NULL )
		{
			if(!MapFile()) 
				return ;
		}
	}

	for(size_t i=0;i<size;++i)
	{
		ThreadAddResAni(pBmps[i],-1,1);
	}

	if(size)
		s_bQueueLowerEmpty  = false;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param aImageID : 描述
 * \param size : 描述
 * \return 返回值的描述
 */
template<class tBitmap>
void TBitmapList<tBitmap>::ThreadLoadImage(DWORD* aImageID,size_t size)
{
	if( IsFlag(flag_MapView) )
	{
		if( m_maphandle == NULL )
		{
			if(!MapFile()) return ;
		}
	}
	for(size_t i=0;i<size;++i)
	{
		DWORD group,frame;
		group = (aImageID[i] >> 16);
		frame = (aImageID[i] & 0xffff);

		TBitmapList<tBitmap>::stGroup* pGroup = GetGroup(group);
		if(!pGroup) continue;
		if(pGroup->GetBitmapsCount() <= frame) 
			continue;

		ThreadAddResAni(group,frame,-1,1);
	}

	if(size)
		s_bQueueLowerEmpty  = false;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param aImageID : 描述
 * \param size : 描述
 * \return 返回值的描述
 */
template<class tBitmap>
void TBitmapList<tBitmap>::ReleaseImage(DWORD* aImageID,size_t size)
{
	s_lockLoadThread.Lock();
	for(size_t i=0;i<size;++i)
	{
		DWORD group,frame;
		group = (aImageID[i] >> 16);
		frame = (aImageID[i] & 0xffff);
		TBitmaps<tBitmap> * pBitmaps = GetBitmaps(group,frame);
		if(pBitmaps)
		{
			s_dwMemorySize -= pBitmaps->ReleaseMemory();
		}
	}
	s_lockLoadThread.Unlock();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param group : 描述
 * \param frame : 描述
 * \return 返回值的描述
 */
template<class tBitmap>
void TBitmapList<tBitmap>::ReleaseAImage(size_t group,size_t frame)
{
	s_lockLoadThread.Lock();

	TBitmaps<tBitmap> * pBitmaps = GetBitmaps(group,frame);
	if(pBitmaps)
	{
		s_dwMemorySize -= pBitmaps->ReleaseMemory();
	}

	s_lockLoadThread.Unlock();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param st : 描述
 * \return 返回值的描述
 */
template<class tBitmap>
void TBitmapList<tBitmap>::Load(stLoadResT & st)
{
	TBitmapList<tBitmap> * pBitmapList = (TBitmapList<tBitmap>*)st.pBmp->GetBitmapList();
	s_lockLoading.Lock();
	if(st.pBmp->IsLoad()){
		s_lockLoading.Unlock();
		return;
	}
	if( pBitmapList->LoadAFrame((tBitmap*)st.pBmp))
	{
		if(!pBitmapList->IsFlag(TBitmapList<tBitmap>::flag_NoRelease))
		{
			//if( 0 == (pBitmapList->m_flags & flag_MapView) )
			{
				s_lockLoaded.Lock();
				g_loaded.push_back(st);
				s_lockLoaded.Unlock();
			}
		}
	}
	s_lockLoading.Unlock();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param st : 描述
 * \return 返回值的描述
 */
template<class tBitmap>
bool TBitmapList<tBitmap>::remove_res_if(stLoadResT & st)
{
	return st.pBmp->GetBitmapList() == g_pRemove;
}

template<class tBitmap>
bool TBitmapList<tBitmap>::remove_res_if2(stLoadResT & st)
{
	return st.pBmp == g_pRemoveBmp;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param p : 描述
 * \return 返回值的描述
 */
template < class tBitmap>
void TBitmapList<tBitmap>::DeleteLoadResource(TBitmapList<tBitmap>* p)
{
	g_pRemove = p;
	s_lockLoadThread.Lock();
	
	g_queue.remove_if(remove_res_if);
	g_queueLower.remove_if(remove_res_if);

	for(size_t i= 0; i < g_loaded.size();)
	{
		if(g_loaded[i].pBmp->GetBitmapList() == p)
		{
			g_loaded[i] = g_loaded.back();
			g_loaded.resize(g_loaded.size()-1);
		}
		else ++i;
	}

	s_dwMemorySize -= p->ClearMemory();

	s_lockLoadThread.Unlock();
}

template < class tBitmap >
void TBitmapList<tBitmap>::RemoveFromThread(IBitmap* pBmp)
{
	if(pBmp->IsPreload()){

		g_pRemoveBmp = pBmp;
		s_lockLoadThread.Lock();

		g_queue.remove_if(remove_res_if2);
		g_queueLower.remove_if(remove_res_if2);

		s_lockLoadThread.Unlock();
	}
}

template < class tBitmap>
void TBitmapList<tBitmap>::ClearLoadQueue()
{
	s_bPauseThreadLoad = true;
	s_lockLoadThread.Lock();

	g_queue.Clear();
	g_queueLower.Clear();

	s_lockLoadThread.Unlock();
	s_bPauseThreadLoad = false;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwMaxTime : 描述
 * \return 返回值的描述
 */
template < class tBitmap>
void TBitmapList<tBitmap>::UnmapFileGroupByTime(DWORD dwMaxTime)
{
	Assert(0);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
template < class tBitmap>
size_t TBitmapList<tBitmap>::ClearResMemory()
{
	s_lockLoaded.Lock();

	DWORD dwTime = xtimeGetTime();
	ULONG size = 0;
	
	for(size_t i = 0; i < g_loaded.size(); )
	{
		stLoadResT& st = g_loaded[i];
		size_t size1 = TBitmapList<tBitmap>::ClearMemoryByTime( st.pBmp );
		if(size1 != -1)
		{
			g_loaded[i] = g_loaded.back();
			g_loaded.resize(g_loaded.size()-1);
			size += size1;
		}
		else ++i;
	}

#if defined _DEBUG || defined RELEALSE_TEST
	Engine_WriteLogF("=====> ClearResMemory time %u,size %u\n", xtimeGetTime() - dwTime,size);
#endif

	s_lockLoaded.Unlock();
	s_dwMemorySize -= size;

	return size;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
template < class tBitmap>
void  TBitmapList<tBitmap>::LockResLoad()
{
	s_lockLoadThread.Lock();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
template < class tBitmap>
void  TBitmapList<tBitmap>::UnlockResLoad()
{
	s_lockLoadThread.Unlock();
}

template < class tBitmap>
void TBitmaps<tBitmap>::ReleaseData()
{
	size_t size = 0;
	for(size_t i=0; i<GetBitmapCount();++i){
		TBitmapList<tBitmap>::RemoveFromThread(&m_bitmaps[i]);
		if(!m_bitmaps[i].IsNull())
		{
			size += m_bitmaps[i].ReleaseMemory();
			m_bitmaps[i].SetLoad(Load_NotLoad);
		}
	}
	TBitmapList<tBitmap>::s_dwMemorySize -= size;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param  : 描述
 * \return 返回值的描述
 */
template < class tBitmap>
unsigned int __stdcall TBitmapList<tBitmap>::ResLoadThread ( void * )
{
	DWORD time = xtimeGetTime();
	stLoadResT st;
	TRACE("---thread----start\n");
	
	while(!s_bExitThread)
	{
		s_lockLoadThread.Lock();
		
		bool bLoad = false;
		while( g_queue.Pop(st) )
		{
			if (s_bExitThread)
				break;
			if(s_bPauseThreadLoad)
				break;

			Load(st);
			bLoad = true;
		}

		if( g_queueLower.Pop(st) )
		{
			if(s_bPauseThreadLoad)
				break;

			Load(st);
			bLoad = true;
		}
		else
		{
			if(s_bQueueLowerEmpty == false)
				s_bQueueLowerEmpty = true;
		}

		s_lockLoadThread.Unlock();
		if(!bLoad) Sleep(10);
	}
	
	TRACE("---thread----exit\n");
	return 0;
}
