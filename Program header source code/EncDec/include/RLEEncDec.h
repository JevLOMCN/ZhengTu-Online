/**
 * \file	RLEEncDec.h
 * \version $Id$
 * \author  $$$03公司
 *          $$$03公司@###.com
 * \date   
 * \brief   RLE压缩解压缩
 *
 *	RLE压缩解压缩
 *
 */

#define RLE_ERROR_OK  0		// 成功
#define RLE_ERROR_SIZE 1	// 缓冲区大小不够
#define RLE_ERROR_DATA 2	// 数据错误

/**
 * \brief  RLE 解压缩
 *
 *
 *
 * \param source : 源数据
 * \param sourceSize : 源数据大小
 * \param dest : 目标数据
 * \param destSize : 目标数据大小(in , out)
 *
 * \return  错误码
 *
 */
inline DWORD RLE_DecodeBytes( const BYTE * source,size_t sourceSize,BYTE * dest,size_t & destSize )
{
	const size_t c1 = sizeof(BYTE)*8 - 1;
	const size_t max_line = (1<<c1);
	const size_t mask_rle = (1<<c1);

	const BYTE* end = source + sourceSize;

	BYTE* pDest = dest;
	BYTE* pDestEnd = dest + destSize;

	for( ; source < end ; )
	{
		size_t flag= *source++;
		size_t len = (flag & (~mask_rle))+1;
		
		if( source  == end ) return RLE_ERROR_DATA;
		if( pDest + len > pDestEnd ) return RLE_ERROR_SIZE;

		if( flag & mask_rle )
		{
			BYTE value = *source++;
			memset(pDest,value,len);
		}
		else
		{
			if( source + len > end ) return RLE_ERROR_DATA;

			memcpy(pDest,source,len);
			source += len;
		}

		pDest += len;
	}
	
	destSize = pDest - dest;

	return RLE_ERROR_OK;
}

/**
 * \brief  RLE 压缩
 *
 *	RLE 压缩
 *
 * \param pData : 源数据
 * \param dataSize : 源数据大小
 * \param pRetData : 目标数据缓冲区，为了安全应提供源数据大小的1.5倍的空间
 * \param nRetLen : 返回目标数据大小
 * \return  无
 *
 */
inline void RLE_EncodeBytes(BYTE* pData,size_t dataSize, BYTE* pRetData, size_t& nRetLen )
{
	size_t flag;
	int i1,i2;

	const size_t c1 = sizeof(BYTE)*8 - 1;
	const size_t max_line = (1<<c1);
	const size_t mask_rle = (1<<c1);

	i1=0;
	i2=-1;
	nRetLen=0;

	for(size_t i = 0 ; i < dataSize ; i++)
	{
		int rl=1;
		while( i+1 < dataSize && rl < max_line &&  pData[i] == pData[i+1] )
		{
			rl++;
			i++;
		}

		if(rl>1)
		{
			if(i1 <= i2)
			{
				flag=i2 - i1;

				pRetData[nRetLen++]=(BYTE)flag;
				memcpy(&pRetData[nRetLen],&pData[i1],(flag+1) * sizeof(BYTE));
				nRetLen+=(flag + 1);
			}

			flag= mask_rle | (rl-1);

			pRetData[nRetLen++]=(BYTE)flag;
			pRetData[nRetLen++]=pData[i];
			i1=(int)(i+1);
		}
		else
		{
			i2=i;
			if(i2-i1 + 1 == (int)max_line)
			{
				flag=i2-i1;
				pRetData[nRetLen++]=flag;
				memcpy( &pRetData[nRetLen],&pData[i1],(flag+1) * sizeof(BYTE) );
				nRetLen += (flag+1);
				i1=(int)(i+1);
			}
		}
	}

	if(i2>=i1)
	{
		flag=i2-i1;
		pRetData[nRetLen++]=flag;
		memcpy(&pRetData[nRetLen],&pData[i1],(flag+1) * sizeof(BYTE) );
		nRetLen+=(flag+1);
	}

}