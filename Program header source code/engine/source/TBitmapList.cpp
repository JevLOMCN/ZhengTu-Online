/**
 * \file      TBitmapList.cpp
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     图形包模板
 * 
 *	      图形包模板
 */

#include "../include/engine.h"
#include "../include/IBitmap.h"
#include "../include/tbitmaplist.h"

size_t gBitmapHeaderSize = 0;

cGPackAttributes::cGPackAttributes(File* pPackFile,stFileInfo* attribute)
: mFile(pPackFile),mCurGroup(-1),mCurAni(-1),mAniHeader(NULL)
{
	mAttribute.resize(attribute->size);
	if(!mAttribute.empty()){
		pPackFile->setPosition(attribute->offsetInPack);
		pPackFile->read(&mAttribute[0],attribute->size);

		mHeader = (stGLHeaderInfoHeader*)&mAttribute[0];
		if(mHeader->dwVersion != GLHEADER_HEADER_VERSION || mHeader->dwIdent != GLHEADER_HEADER_IDENT){
			mAttribute.resize(0);
			return;
		}

		mGroups = (stGLGroupHeaderInfo*)&mAttribute[mHeader->dwInfoOffset];
		for(size_t i=0; i<mHeader->dwGroupCount;++i){
			//TRACE("groupID %d\n",mGroups[i].dwGroupID);
		}
	}
}

static  int __cdecl cmpGroup( const void *p1, const void *p2)
{
	DWORD dwGroupID = (DWORD)p1;
	stGLGroupHeaderInfo* pGroup = (stGLGroupHeaderInfo*)p2;
	return (long)dwGroupID - (long)pGroup->dwGroupID;
}

int cGPackAttributes::getGroupIndex(size_t groupID)
{
	stGLGroupHeaderInfo* pGroup = (stGLGroupHeaderInfo*)bsearch((void*)groupID,&mGroups[0],mHeader->dwGroupCount,sizeof(stGLGroupHeaderInfo),cmpGroup);
	if(pGroup == 0) return -1;
	return pGroup - &mGroups[0];
}

bool cGPackAttributes::setGroup(size_t groupID,DWORD dwCRC)
{
	if(mAttribute.empty())
		return false;

	int index = getGroupIndex(groupID);
	if(index == -1) 
		return false;

	if(mGroups[index].dwGrpFileCRC == dwCRC){
		mCurGroup = index;
		mCurAni = -1;
		mData = &mAttribute[mGroups[mCurGroup].dwOffset];
		mAniHeader = (stGLAniHeaderInfo*)&mAttribute[mGroups[mCurGroup].dwAniHeaderOffset];
		return true;
	}else{
		mCurGroup = -1;
		mCurAni = -1;
		mAniHeader = NULL;
		mData = NULL;
		return false;
	}
}

void cGPackAttributes::setAni(size_t aniIndex)
{
	mCurAni = aniIndex;
}

const stGLHeader* cGPackAttributes::getGLHeader()
{
	return (stGLHeader*)mData;
}

const stGrpAniInfo* cGPackAttributes::getAniInfo()
{
	return &mAniHeader[mCurAni].aniInfo;
}

const BYTE* cGPackAttributes::getBitmapInfo()
{
	return &mAttribute[mAniHeader[mCurAni].bitmapHeaderOffset];
}