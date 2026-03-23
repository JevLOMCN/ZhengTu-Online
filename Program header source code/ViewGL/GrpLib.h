#pragma once

#include "../MakeGL/MakeGrp.h"
#include <vector>

#include <gdiplus.h>
#include <Gdiplusheaders.h>
#include <Gdiplusimaging.h>

class CGrpLib;
class CGrpBlock
{
	friend class CGrpLib;
	stGrpBlockEx m_st;

	Gdiplus::Bitmap * m_bmp;
public:
	CGrpBlock();
	~CGrpBlock();
	BOOL LoadFromFile(const char* pszFileName,bool bTestLoad = false);
	Gdiplus::Bitmap* GetBitmap();
	//HBITMAP GetBitmap(COLORREF bkColor);

	DWORD GetCRC(){ return m_st.dwCRC; }
	int GetWidth(){return m_st.wdWidth;}
	int GetHeight(){ return m_st.wdHeight;}
	void Release();
	bool SaveToTga(const char* szFileName , bool bRLE);
	const stGrpBlockEx* GetBlkStruct(){ return &m_st;}
};

struct stAnimation{
	stPackAniInfo aniInfo;
	std::vector<CGrpBlock> bmps;
};

struct stGrpGroup
{
	DWORD dwGroupID;
	std::vector<stAnimation> Anis;
};

class CGrpLib
{
	stGrpFileHeader				m_hdr;
	std::vector<stGrpGroup>		m_aGroup;
	CGrpBlock * m_pCurBlock;
	std::string m_strFileName;
public:
	int GetGroupIndex(DWORD nGroupID);
	CGrpLib(void);
	~CGrpLib(void);
	BOOL Open(const char* pszFileName); 
	void SetCurBlock(size_t group,size_t frame,size_t frame0);
	CGrpBlock* GetCurBlock(){ return m_pCurBlock;}
	size_t GetGroupCount(){ return m_aGroup.size();}
	
	size_t GetGroupID(size_t nGroupIndex)
	{
		if(nGroupIndex >= m_aGroup.size())
		{
			return -1;
		}
		return m_aGroup[nGroupIndex].dwGroupID;
	}

	bool CheckGroupID();

	size_t GetFrameCount(size_t groupID) { 
		long index = GetGroupIndex(groupID);
		if(index == -1) return 0;
		return m_aGroup[index].Anis.size();
	}

	stAnimation& GetAnimation(size_t groupID,size_t frame)
	{
		long index = GetGroupIndex(groupID);
		ASSERT(index != -1);
		return m_aGroup[index].Anis[frame];
	}
	
	const stGrpBlockEx* GetBlkStruct(size_t groupID,size_t frame,size_t frame0)
	{
		long index = GetGroupIndex(groupID);
		ASSERT(index != -1);
		return m_aGroup[index].Anis[frame].bmps[frame0].GetBlkStruct();
	}

	bool SaveToTga(const char* szPath,bool bObject);
	DWORD TestTime(void);
};