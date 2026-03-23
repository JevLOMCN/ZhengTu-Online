#include "./ibitmaps.h"
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */

template < class tBitmap >
class TBitmaps : public IBitmaps
{
	tBitmap *	m_bitmaps;
public:
	
	typedef TBitmaps<tBitmap> _Myt;

	TBitmaps()
		: m_bitmaps(NULL)
	{
	}

	virtual ~TBitmaps()
	{
		//delete [] m_bitmaps;
		destructInPlaceCount(m_bitmaps,GetBitmapCount());
	}

	void Init(IBitmapList* pBmpList,DWORD dwGroup,DWORD dwFrame,DWORD dwDataOffset,DWORD dwDataSize)
	{
		m_pBmpList = pBmpList;
		m_dwGroup = (WORD)dwGroup;
		m_dwFrame = (WORD)dwFrame;
		m_dwDataOffset = dwDataOffset;
		m_dwDataSize = dwDataSize;
		Assert( (m_dwDataOffset & 3) == 0 );
		Assert( (m_dwDataSize & 3) == 0 );
	}

	IBitmap* GetBitmap(size_t frame) 
	{ 
		size_t frameIndex = GetFrameIndex(frame);
		if(frameIndex == -1) return NULL;
		return &m_bitmaps[frameIndex];
	}

	IBitmap* GetBitmapByIndex(size_t index)
	{
		return &m_bitmaps[index];
	}

	void SetLoad(LoadEnum e)
	{
		eLoad = e;
		for(size_t i=0;i<m_aniInfo.wdBitmapCount;++i)
		{
			m_bitmaps[i].SetLoad(e);
		}
	}

	void SetLoadError()
	{
		eLoad = e;
		for(size_t i=0;i<m_aniInfo.wdBitmapCount;++i)
		{
			if(!m_bitmaps[i].IsLoadData())
				m_bitmaps[i].SetLoad(Load_LoadError);
		}
	}

	size_t _LoadBitmap(File& file)
	{
		size_t result = 0;
		for(size_t i=0;i<m_aniInfo.wdBitmapCount;++i)
		{
			if(!m_bitmaps[i].IsNull() && !m_bitmaps[i].IsLoadData())
			{
				size_t size = m_bitmaps[i]._LoadBitmap(file);
				if(size == IBitmap::errorSize)
					return IBitmap::errorSize;
				result += size;
			}
		}
		return result;
	}

	size_t ReleaseMemory()
	{
		size_t size = 0;
		for(size_t i=0;i<m_aniInfo.wdBitmapCount;++i)
		{
			if(!m_bitmaps[i].IsNull())
			{
				size += m_bitmaps[i].ReleaseMemory();
				m_bitmaps[i].SetLoad(Load_NotLoad);
			}
		}
		return size;
	}

	void InitBitmap( const BYTE* pData,BYTE* & pAddinData )
	{
		stPackAniHeader *pAniHdr = (stPackAniHeader*)pData;
		m_aniInfo = *pAniHdr;
		pData += sizeof(stPackAniHeader);
		Assert(m_bitmaps == NULL);
		m_bitmaps = (tBitmap*)pAddinData;
		constructInPlaceCount(m_bitmaps,GetBitmapCount());

		pAddinData += sizeof(tBitmap) * GetBitmapCount();

		DWORD dwDataOffset = GetDataOffset() + sizeof(stPackAniHeader);
		if(GetBitmapCount() != GetFrameCount())
		{
			m_pFrameIndex = (DWORD*)pAddinData;
			pAddinData += sizeof(DWORD) * GetFrameCount();

			memcpy(m_pFrameIndex,pData,sizeof(DWORD) * GetFrameCount());
			pData += sizeof(DWORD) * GetFrameCount();
			dwDataOffset += sizeof(DWORD) * GetFrameCount();
		}
		dwDataOffset += GetBitmapCount() * sizeof(stGrpBlock);
		for(size_t i=0;i<GetBitmapCount();++i)
		{
			memcpy(&m_bitmaps[i].m_block,pData,sizeof(stGrpBlock));
			m_bitmaps[i].m_block.dwDataOffset += dwDataOffset;
			m_bitmaps[i].Init(this,GetBitmapFrame(i));
			pData += sizeof(stGrpBlock);
		}
	}

	void SaveHdrData(Stream* file)
	{
		file->write(&m_aniInfo,sizeof(stPackAniHeader));
		DWORD dwDataOffset = GetDataOffset() + sizeof(stPackAniHeader);
		if(GetBitmapCount() != GetFrameCount())
		{
			file->write(m_pFrameIndex,sizeof(DWORD) * GetFrameCount());
			dwDataOffset += sizeof(DWORD) * GetFrameCount();
		}
		dwDataOffset += GetBitmapCount() * sizeof(stGrpBlock);
		for(size_t i=0;i<GetBitmapCount();++i)
		{
			stGrpBlock blk = m_bitmaps[i].m_block;
			blk.dwDataOffset -= dwDataOffset;
			file->write(&blk,sizeof(stGrpBlock));
		}
	}

	static size_t GetAddinDataSize(stPackAniHeader *pAniHdr)
	{
		size_t size;

		size = sizeof(tBitmap)* pAniHdr->wdBitmapCount;

		if(pAniHdr->wdBitmapCount != pAniHdr->wdFrameCount)
			size += sizeof(DWORD) * pAniHdr->wdFrameCount;

		return size;
	}

	void ReleaseData();
};