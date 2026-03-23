/**
 * \file      D3DBitmap.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2005-07-05 12:29:19
 * \brief     图片类，用D3D设备绘制
 * 
 *	      图片类，用D3D设备绘制
 */

#pragma once
#include "../../engine/include/ibitmap.h"
#include <vector>
#include <list>

enum enumBlockRenderInfo{
	BlockRenderInfo_Error = 0,
	BlockRenderInfo_Render = 1,
	BlockRenderInfo_Null = 2,
};
struct stRenderInfo;
class CTextureCell;
class CD3DBitmapList;
class CD3DMBitmap;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CD3DBitmap :
	public IBitmap
{
	friend class CBmpTextureCell;
	friend class CD3DMBitmap;

	/*int m_iTailTexWidth;
	int m_iTailTexHeight;*/

	/*float m_fTailTexX;
	float m_fTailTexY;*/

	struct stBlockInfo{
		CBmpTextureCell* pTex;
	};

	stBlockInfo* m_textures;
	
	bool	m_bMapFile;
public:
	
	//static size_t GetAddinDataSize(stGrpBlock* pInfo);

	size_t _LoadBitmap(File& file);
	size_t _MapData(BYTE* pMapData);
	size_t _MapDataLoad(BYTE* pMapData);
	int GetAlpha(POINT pt);

	CD3DBitmapList* GetBitmapList(){ return (CD3DBitmapList*)IBitmap::GetBitmapList();}

	CD3DBitmap();
	~CD3DBitmap(void);

	size_t ReleaseMemory();
	void ReleaseTexture();

	bool UpdateTexture(IDirect3DTexture8* pTex,D3DFORMAT dstFormat,int blockX,int blockY,POINT ptTex);

	bool Render(int x,int y,DWORD color = 0xffffffff,BlendType blend = Blend_Null);

	bool Render(int x,int y,const RECT* rect,const stPointF * scale,DWORD color = 0xffffffff,BlendType blend = Blend_Null);

	bool RenderBlock(POINT block,stPointF  pt,const RECT * rect,const stPointF * scale,DWORD color,BlendType blend);

	enumBlockRenderInfo GetBlockRenderInfo(POINT  block,stPointF  pt,const RECT * rect,const stPointF * scale,DWORD color,BlendType blend,stRenderInfo & ri);

	void Init(IBitmaps * pBitmaps,DWORD dwFrameFrame);
	bool LoadBlockTexture(size_t xBlock, size_t yBlock);
	
	CBmpTextureCell* GetBlockTexture(POINT ptBlock);

	void GetBlockClipRect(int blockX,int blockY ,RECT* prect)
	{
		CalculateBlockClipRect(blockX,blockY,prect);
	}

	SIZE GetBlockSize(int blockX,int blockY)
	{
		return CalculateBlockClipSize(blockX,blockY);
	}

	bool GetBlockPos(int x,int y,POINT& ptBlock,POINT & ptBlockPos)
	{
		x -= (int)m_block.wdClipLeft;
		y -= (int)m_block.wdClipTop;

		if(x < 0 || x >= m_block.wdClipWidth || y < 0 || y >= m_block.wdClipHeight)
		{
			ptBlock.x = -1;
			ptBlock.y = -1;
			return false;
		}
		ptBlock.x = x / MAX_TEXTUREWIDTH;
		ptBlock.y = y / MAX_TEXTUREWIDTH;

		ptBlockPos.x = x % MAX_TEXTUREWIDTH;
		ptBlockPos.y = y % MAX_TEXTUREWIDTH;

		stRectI rcClip;
		GetBlockClipRect(ptBlock.x,ptBlock.y,&rcClip);
		if(!rcClip.PtInRect(ptBlockPos)) return false;

		ptBlockPos.x -= rcClip.left;
		ptBlockPos.y -= rcClip.top;

		return true;
	}

	void InitTexture();
	bool GetBlockBitmapData(int xBlock,int yBlock,void* pData,int dataPatch);
	bool GetBitmapData(void* pData,int dataPatch);
	//bool TryLoadBlockTexture(size_t xBlock, size_t yBlock);
	//bool TryLoadTexture();
};