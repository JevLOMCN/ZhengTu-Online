/**
 * \file      WaveModInfo.cpp
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-2-23 23:01:09
 * \brief     ？
 * 
 *	      
 */

#include "../engine/include/engine.h"
#include ".\wavemodinfo.h"

#undef MAKEFOURCC

#define MAKEFOURCC(ch0, ch1, ch2, ch3)\
	((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |\
	((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

#define mmioFOURCC(ch0, ch1, ch2, ch3)\
	MAKEFOURCC(ch0, ch1, ch2, ch3)

CWaveModInfo::CWaveModInfo(Stream* pStream)
: mOk(false)
, mStream(pStream)
{
	FRiffWaveHeader waveHeader;
	FRiffChunkOld riffChunk;

	bool bGetFmt = false,bGetData = false;
	pStream->read(&waveHeader,sizeof(waveHeader));
	if(waveHeader.wID != mmioFOURCC('W','A','V','E'))
		return ;

	BYTE* pData = NULL;
	DWORD dataSize = 0;
	while(1){
		if(Stream::EOS == pStream->getStatus())
			return ;

		pStream->read(&riffChunk,sizeof(riffChunk));
		size_t nextPos = pStream->getPosition() + riffChunk.ChunkLen;
		switch(riffChunk.ChunkID)
		{
		case mmioFOURCC('f','m','t',' '):
			pStream->read(&mFormat,sizeof(mFormat));
			bGetFmt = true;
			break;
		case mmioFOURCC('d','a','t','a'):
			mDataPos = pStream->getPosition();
			mDataSize = riffChunk.ChunkLen;
			bGetData = true;
			break;
		case mmioFOURCC('s','m','p','l'):
			break;
		}
		if(bGetFmt && bGetData)
			break;

		pStream->setPosition(nextPos);
	}
	mOk = bGetData && bGetFmt;
}

bool CWaveModInfo::readData(void* pData,size_t size)
{
	if(!mOk)
		return false;
	mStream->setPosition(mDataPos);
	mStream->read(pData,getMin(size,mDataSize));
	mDataPos = mStream->getPosition();
	return true;
}