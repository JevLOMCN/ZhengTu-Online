#include "../engine/include/engine.h"
#include ".\mapscene.h"
#include "SceneObject.h"
#include "public.h"
#include "./iCharacter.h"
#include "./MapSurf.h"
#include "../xml_parse/XmlParseEx.h"
#include "./MapSoundManager.h"
#include "iMagic.h"
#include "../Client/execryptor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const POINT c_ptDir[8] = {
	{0,-1},
	{1,-1},
	{1,0},
	{1,1},

	{0,1},
	{-1,1},
	{-1,0},
	{-1,-1},
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
static CMapScene* s_pMapScene = NULL;

CMapScene* GetMapScene()
{
	return s_pMapScene;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CMapScene::CMapScene(void)
: m_pTerrainPack(NULL)
, m_pTerrainAlphaPack(NULL)
, m_pSmTerrainPack(NULL)
, m_bDoorBlock(false)
, m_rcFindPathRect(0,0,0,0)
, m_pMouseOverObject(NULL)
, m_pMouseOverTextObject(NULL)
, m_bLightMap(false)
, m_loaded(NULL)
, m_bInitConver(false)
, m_bInterior(false)
, m_converWidth(0)
, m_converHeight(0)
, m_bDrawMagicEffect(true)
{
	s_pMapScene = this;
	m_iPixelWidth	=	0;	//	屏幕的像素宽度
	m_iPixelHeight	=	0;	//	屏幕的像素高度
	ReleaseScene();
	Engine_SetCallBack(this);

	m_nLightRadius = 200;
	m_dwLightColor = 0xff808080;
	m_szFileName[0] = 0;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CMapScene::~CMapScene(void)
{
	Engine_SetCallBack(NULL);
	delete m_loaded;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMapScene::ReleaseScene()
{
	GetMapSurfManager()->Release();
	GetMapSoundManager()->Release();
	m_iScreenWidth	=	0;	//	以屏为单位的宽
	m_iScreenHeight	=	0;	//	以屏为单位的高

	m_ptPixelOffset = stPointI(0,0);	//	屏幕左上角的像素偏移
	m_ptAddinOffset = stPointI(0,0);
	m_rcDrawRect	= stRectI(0,0,0,0);
	
	m_map.Reset(1,1);
	m_dwBeginTime	=	0;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param szMapName : 描述
 * \return 返回值的描述
 */
bool CMapScene::LoadMap(const char* szMapName)
{

	ReleaseScene();

	SetScreenSize(GetDevice()->GetWidth(),GetDevice()->GetHeight());

	char szFileName[MAX_PATH];
	char pcan1[MAX_PATH];
	strcpy(pcan1,szMapName);
	char* pcan2=strtok(pcan1, "$");
	if(pcan2 == NULL)
		sprintf(szFileName,"map\\%s.map",szMapName);
	else
		sprintf(szFileName,"map\\%s.map",pcan2);
	Stream* pStream = OpenPackFileForRead("data\\map.pak",szFileName);		//"data\\map.pak"
	if(!pStream) 
	{
		Assert(0);
//		char szMsg[256];
		Engine_WriteLogF("不能打开地图文件 %s\n",szMapName);
		return false;
	}

	DWORD t1 = xtimeGetTime();
	bool bLoad = m_map.LoadFromStream(pStream);

	ClosePackFileForRead(pStream);
	m_iScreenWidth = (m_map.GetWidth() + SCREEN_GRID_WIDTH -1) / SCREEN_GRID_WIDTH;
	m_iScreenHeight = (m_map.GetHeight() + SCREEN_GRID_HEIGHT - 1) / SCREEN_GRID_HEIGHT;
	m_dwBeginTime	= xtimeGetTime();

	m_bInitConver = false;
	
	t1 = xtimeGetTime();
	InitCover();
	Engine_WriteLogF("InitCover time %d\n",xtimeGetTime() - t1);

	m_curConverZone.resize(0);
	m_curLoadResZone.resize(0);
	if( stricmp( szMapName,m_szFileName ) )
	{
		/*t1 = xtimeGetTime();
		LoadAllImage();
		Engine_WriteLogF("LoadAllImage time %d\n",xtimeGetTime() - t1);*/
		strcpy(m_szFileName,szMapName);
	}

	return true;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	描述: 		  初始化是地表的格子
*/
void CMapScene::InitTerrTile(stMapTileItemEx* pTile)
{
	bool bObject = false;
	if(pTile->objectLib < m_aObjectPack.size() && m_aObjectPack[pTile->objectLib])
	{
		stRectI rcPixel;
		size_t group,frame;
		GetTileGroupFrame(pTile->object,group,frame);
		IBitmap* pBmp = m_aObjectPack[pTile->objectLib]->GetBitmap(group,frame);
		if(pBmp && pBmp->GetHeight() <= 32 * 2)
		{
			bObject = true;
		}
	}
	else 
		bObject = true;

	bool bObject2 = false;
	stMapTileOther * pOther = pTile->pOther;

	if(pOther && pOther->objectLib2 < m_aObjectPack.size() && m_aObjectPack[pOther->objectLib2])
	{
		stRectI rcPixel;
		size_t group,frame;
		GetTileGroupFrame(pOther->object2,group,frame);
		IBitmap* pBmp = m_aObjectPack[pOther->objectLib2]->GetBitmap(group,frame);
		if(pBmp && pBmp->GetHeight() <= 32 * 2)
		{
			bObject2 = true;
		}
	}
	else
		bObject2 = true;

	//sky 兼容重置版地图
	bool bObject3 = false;
	st2MapTileOther * p2Other = pTile->p2Other;

	if (p2Other && p2Other->objectLib2 < m_aObjectPack.size() && m_aObjectPack[p2Other->objectLib2])
	{
		stRectI rcPixel;
		size_t group, frame;
		GetTileGroupFrame(p2Other->object2, group, frame);
		IBitmap* pBmp = m_aObjectPack[p2Other->objectLib2]->GetBitmap(group, frame);
		if (pBmp && pBmp->GetHeight() <= 32 * 2)
		{
			bObject3 = true;
		}
	}
	else
		bObject3 = true;

	if(bObject && bObject2 && bObject3)
	{
		pTile->flags |= TILE_TERR;
	}
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	描述: 		  初始化挡住人物的格子
*/
void CMapScene::InitCover()
{
	/*for(size_t i=0;i<m_map.GetHeight();++i)
	{
		for(size_t j=0;j<m_map.GetWidth();++j)
		{
			stMapTileItemEx* pTile = m_map.GetTile(j,i);
			InitTerrTile(pTile);
			if(pTile->flags & TILE_TERR) continue;
			if(pTile->objectLib < m_aObjectPack.size() && m_aObjectPack[pTile->objectLib])
			{
				stRectI rcPixel;
				size_t group,frame;
				GetTileGroupFrame(pTile->object,group,frame);
				IBitmap* pBmp = m_aObjectPack[pTile->objectLib]->GetBitmap(group,frame);
				if(pBmp)
				{
					POINT ptClip = pBmp->GetClipOffset();
					
					rcPixel.left = (long)j * GRID_WIDTH + ptClip.x;
					rcPixel.right = rcPixel.left + pBmp->GetClipWidth();
					rcPixel.top = ((long)i + 1)* GRID_HEIGHT - pBmp->GetHeight() + ptClip.y;
					rcPixel.bottom = rcPixel.top + pBmp->GetClipHeight();

					AddCover(&rcPixel);
				}
			}

			stMapTileOther * pOther = pTile->pOther;

			if(pOther && pOther->objectLib2 < m_aObjectPack.size() && m_aObjectPack[pOther->objectLib2])
			{
				stRectI rcPixel;
				size_t group,frame;
				GetTileGroupFrame(pOther->object2,group,frame);
				IBitmap* pBmp = m_aObjectPack[pOther->objectLib2]->GetBitmap(group,frame);
				if(pBmp)
				{
					POINT ptClip = pBmp->GetClipOffset();

					rcPixel.left = (long)j * GRID_WIDTH + ptClip.x;
					rcPixel.right = rcPixel.left + pBmp->GetClipWidth();
					rcPixel.top = ((long)i + 1)* GRID_HEIGHT - pBmp->GetHeight() + ptClip.y;
					rcPixel.bottom = rcPixel.top + pBmp->GetClipHeight();

					AddCover(&rcPixel);
				}
			}
		}
	}*/
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param prcPixel : 描述
 * \return 返回值的描述
 */
void CMapScene::AddCover(const RECT * prcPixel)
{

	/*stRectI rcGrid = *prcPixel;
	rcGrid.left += GRID_WIDTH/2;
	rcGrid.top += GRID_HEIGHT/2;
	rcGrid.right -= GRID_WIDTH/2;
	rcGrid.bottom -= GRID_HEIGHT/2;
	Map_PixelToGrid(&rcGrid);
	rcGrid.top = getMax(rcGrid.top,0L);
	rcGrid.left = getMax(rcGrid.left,0L);
	rcGrid.right = getMin(rcGrid.right,(long)m_map.GetWidth());
	rcGrid.bottom = getMin(rcGrid.bottom,(long)m_map.GetHeight());
	for(int i=rcGrid.top;i<rcGrid.bottom;++i)
	{
		for(int j=rcGrid.left;j<rcGrid.right;++j)
		{
			stMapTileItemEx* p = m_map.GetTile(j,i);
			MapTileItem_SetCoverNum(p,MapTileItem_GetCoverNum(p)+1);
		}
	}*/
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param prcPixel : 描述
 * \return 返回值的描述
 */
void CMapScene::RemoveCover(const RECT * prcPixel)
{
	/*stRectI rcGrid = *prcPixel;
	Map_PixelToGrid(&rcGrid);
	for(int i=rcGrid.top;i<rcGrid.bottom;++i)
	{
		for(int j=rcGrid.left;j<rcGrid.right;++j)
		{
			stMapTileItemEx* p = m_map.GetTile(j,i);
			MapTileItem_SetCoverNum(p,MapTileItem_GetCoverNum(p)-1);
		}
	}*/
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwColor : 描述
 * \return 返回值的描述
 */
void CMapScene::DrawTerrain(DWORD dwColor)
{
	if(!m_pTerrainPack) return;
	
	GetDevice()->BeginFlush(Flush_NoCover);

	stRectI terrainRect(m_rcDrawRect.left & (~0x1),m_rcDrawRect.top & (~0x3),
		(m_rcDrawRect.right + 1) & (~0x1), (m_rcDrawRect.bottom + 3) & (~0x3));
	
	if(terrainRect.right > m_map.GetWidth())
		terrainRect.right = m_map.GetWidth();

	if(terrainRect.bottom > m_map.GetHeight())
		terrainRect.bottom = m_map.GetHeight();

	stPointI pixelOffset;
	stPointI ptMapPixelOffset = GetPixelOffset();
	pixelOffset.x = terrainRect.left * GRID_WIDTH - (ptMapPixelOffset.x);
	pixelOffset.y = terrainRect.top * GRID_HEIGHT - (ptMapPixelOffset.y);

	stPointI ptMapPixelRB;
	ptMapPixelRB.x = terrainRect.right * GRID_WIDTH - (ptMapPixelOffset.x);
	ptMapPixelRB.y = terrainRect.bottom * GRID_HEIGHT - (ptMapPixelOffset.y);

	if(pixelOffset.x > 0 || pixelOffset.y > 0 || ptMapPixelRB.x < m_iPixelHeight || ptMapPixelRB.y < m_iPixelHeight)
		GetDevice()->Clear(0);

	DWORD dwTime = xtimeGetTime() - m_dwBeginTime;
	int x,y;
	y = pixelOffset.y;
	for(size_t i=terrainRect.top; i < terrainRect.bottom; i += 4)
	{
		x = pixelOffset.x;
		for(size_t j = terrainRect.left ; j < terrainRect.right; j += 2)
		{
			stMapTileTerrain* p = m_map.GetTerrain(j,i);
			bool bRender = false;
			if(p->tile != NULL_MAPIMGID)
			{
				stMapTileOther* pOther = m_map.GetTile(j,i)->pOther;
				if( pOther && pOther->effect != NULL_MAPIMGID && pOther->effectAlpha != NULL_MAPIMGID )
				{
					// 上一层表这一层完全挡住了
					IBitmaps* alphas;
					size_t group,frame;
					GetTileGroupFrame(pOther->effectAlpha,group,frame);
					alphas = m_pTerrainAlphaPack ? m_pTerrainAlphaPack->GetBitmaps(group,frame) : NULL;
					if( alphas && alphas->GetFrameCount() && alphas->GetBitmap(0)->IsAlphaAllWhite() )
						goto _step1;
				}

				//sky 兼容重置版地图
				st2MapTileOther* p2Other = m_map.GetTile(j, i)->p2Other;
				if (p2Other && p2Other->effect != NULL_MAPIMGID && p2Other->effectAlpha != NULL_MAPIMGID)
				{
					// 上一层表这一层完全挡住了
					IBitmaps* alphas;
					size_t group, frame;
					GetTileGroupFrame(p2Other->effectAlpha, group, frame);
					alphas = m_pTerrainAlphaPack ? m_pTerrainAlphaPack->GetBitmaps(group, frame) : NULL;
					if (alphas && alphas->GetFrameCount() && alphas->GetBitmap(0)->IsAlphaAllWhite())
						goto _step1;
				}

				size_t group,frame;
				GetTileGroupFrame(p->tile,group,frame);
				bRender = DrawMapTileAni(m_pTerrainPack,group,frame,x,y,dwTime,dwColor);
			}

			if(m_bInterior && !bRender)
			{// 地宫没有地表用一个黑块搞定
				stRectI rcBk(x,y,x+2*GRID_WIDTH,y+4*GRID_HEIGHT);
				GetDevice()->FillRect(&rcBk,0xff000000);
			}

_step1:			
			x += GRID_WIDTH * 2;
		}
		y += GRID_HEIGHT * 4;
	}
	GetDevice()->EndFlush();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwColor : 描述
 * \return 返回值的描述
 */
void CMapScene::DrawTerrainEffect(DWORD dwColor)
{
	if(!m_pTerrainPack || !m_pTerrainAlphaPack) return;

	GetDevice()->BeginFlush(Flush_NoCover);
	stRectI terrainRect(m_rcDrawRect.left & (~0x1),m_rcDrawRect.top & (~0x3),
		(m_rcDrawRect.right + 1) & (~0x1), (m_rcDrawRect.bottom + 3) & (~0x3));
	stPointI pixelOffset;
	stPointI ptMapPixelOffset = GetPixelOffset();
	pixelOffset.x = terrainRect.left * GRID_WIDTH - (ptMapPixelOffset.x);
	pixelOffset.y = terrainRect.top * GRID_HEIGHT - (ptMapPixelOffset.y);

	DWORD dwTime = xtimeGetTime() - m_dwBeginTime;
	int x,y;
	y = pixelOffset.y;
	for(size_t i=terrainRect.top; i < terrainRect.bottom; i += 4)
	{
		x = pixelOffset.x;
		for(size_t j = terrainRect.left ; j < terrainRect.right; j += 2)
		{
			stMapTileOther* pOther = m_map.GetTile(j,i)->pOther;
			if( pOther && pOther->effect != NULL_MAPIMGID && pOther->effectAlpha != NULL_MAPIMGID )
			{
				IBitmaps* bmps,*alphas;
				size_t group,frame;

				GetTileGroupFrame(pOther->effect,group,frame);
				bmps = m_pTerrainPack->GetBitmaps(group,frame);

				GetTileGroupFrame(pOther->effectAlpha,group,frame);
				alphas = m_pTerrainAlphaPack->GetBitmaps(group,frame);

				if(bmps && alphas)
					IBitmaps::RenderBlendAni(bmps,alphas,x,y,dwTime,dwColor);

			}

			//sky 兼容重置版地图
			st2MapTileOther* p2Other = m_map.GetTile(j, i)->p2Other;
			if (p2Other && p2Other->effect != NULL_MAPIMGID && p2Other->effectAlpha != NULL_MAPIMGID)
			{
				IBitmaps* bmps, *alphas;
				size_t group, frame;

				GetTileGroupFrame(p2Other->effect, group, frame);
				bmps = m_pTerrainPack->GetBitmaps(group, frame);

				GetTileGroupFrame(p2Other->effectAlpha, group, frame);
				alphas = m_pTerrainAlphaPack->GetBitmaps(group, frame);

				if(bmps && alphas)
					IBitmaps::RenderBlendAni(bmps,alphas,x,y,dwTime,dwColor);

			}
			x += GRID_WIDTH * 2;
		}
		y += GRID_HEIGHT * 4;
	}
	GetDevice()->EndFlush();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwColor : 描述
 * \return 返回值的描述
 */
void CMapScene::DrawSmTerrain(DWORD dwColor)
{
	if(!m_pSmTerrainPack) return;

	GetDevice()->BeginFlush(Flush_NoCover);
	
	stRectI terrainRect((m_rcDrawRect.left-4)  ,(m_rcDrawRect.top-8) & (~7),
		m_rcDrawRect.right, (m_rcDrawRect.bottom + 1) & (~1));

	if(terrainRect.left < 0)
		terrainRect.left = 0;
	if(terrainRect.top < 0)
		terrainRect.top = 0;

	if(terrainRect.right > m_map.GetWidth())
		terrainRect.right = m_map.GetWidth();

	if(terrainRect.bottom > m_map.GetHeight())
		terrainRect.bottom = m_map.GetHeight();

	stPointI pixelOffset;
	stPointI ptMapPixelOffset = GetPixelOffset();
	pixelOffset.x = terrainRect.left * GRID_WIDTH - (ptMapPixelOffset.x);
	pixelOffset.y = terrainRect.top * GRID_HEIGHT - (ptMapPixelOffset.y);

	DWORD dwTime = xtimeGetTime() - m_dwBeginTime;
	int x,y;
	y = pixelOffset.y;
	for(size_t i=terrainRect.top; i < terrainRect.bottom; i += 2)
	{
		x = pixelOffset.x;
		for(size_t j = terrainRect.left ; j < terrainRect.right; ++j)
		{
			stMapTileSmTerrain* p = m_map.GetSmTerrain(j,i);
			if(p->smTile != NULL_SMTERRID)
			{
				size_t group,frame;
				GetTileGroupFrame(p->smTile,group,frame);
				DrawMapTileAni(m_pSmTerrainPack,group,frame,x,y,dwTime,dwColor);
			}
			x += GRID_WIDTH;
		}
		y += GRID_HEIGHT * 2;
	}
	GetDevice()->EndFlush();
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwColor : 描述
 * \return 返回值的描述
 */
void CMapScene::DrawObstacle(DWORD dwColor,const stRectI *nrcDraw /* = NULL */)
{
	stRectI rcDraw(0,0,100,100);
	if (nrcDraw == NULL)
	{
#ifdef _DEBUG
		// 2009
		rcDraw.SetRect(m_rcDrawRect.left,m_rcDrawRect.top,m_rcDrawRect.right,m_rcDrawRect.bottom + SCREEN_GRID_HEIGHT * 3);
#else
		return;
#endif			
	}
	else
	{
		memcpy(&rcDraw,nrcDraw,sizeof(rcDraw));
	}

	if(rcDraw.bottom > m_map.GetHeight()) rcDraw.bottom = m_map.GetHeight();

	int x,y;
	DWORD dwTime = xtimeGetTime() - m_dwBeginTime;

	stPointI ptMapObject;
	stPointI ptMapPixelOffset = GetPixelOffset();
	stPointI ptOffsetPixel(rcDraw.left * GRID_WIDTH - ptMapPixelOffset.x,rcDraw.top * GRID_HEIGHT - ptMapPixelOffset.y);

	y = ptOffsetPixel.y;
	for(size_t i = rcDraw.top; i < rcDraw.bottom;++i)
	{
		ptMapObject.y = i ;
		x = ptOffsetPixel.x;
		for(size_t j = rcDraw.left; j < rcDraw.right;++j,x += GRID_WIDTH)
		{
			ptMapObject.x = j;
			DWORD dwColor = COLOR_ARGB(128,171,132,65);

#ifdef _DEBUG
			bool bS = IsStaticObstacle(ptMapObject);
			bool bM = IsMagicObstacle(ptMapObject);
			if(bS && bM)
				dwColor = COLOR_ARGB(128,255,0,255);
			else if(bS)
			{
				dwColor = COLOR_ARGB(128,255,0,0);
			}
			else if(bM)
			{
				dwColor = COLOR_ARGB(128,0,0,255);
			}
			if(IsDynamicObstacle(ptMapObject))
			{
				dwColor = COLOR_ARGB(128,0,255,0);
			}
			/*
			if(IsCover(ptMapObject))
			{
			if(dwColor)
			dwColor = ColorBlend(dwColor,COLOR_ARGB(128,255,255,0));
			else dwColor = COLOR_ARGB(128,255,255,0);
			}
			//*/
#endif
			if(dwColor)
			{
				RECT rc = {x,y,x+GRID_WIDTH,y+GRID_HEIGHT};
				GetDevice()->FillRect(&rc,dwColor);
			}
		}
		y += GRID_HEIGHT;
	}
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	描述: 		  有些物件是人所践踏的，先绘制
*/
void CMapScene::DrawMapLowestObjects(DWORD dwColor)
{
	// 这一段暂时不需要了
	/*
	GetDevice()->BeginFlush(Flush_NoCover);

	stRectI rcDraw(m_rcDrawRect.left,m_rcDrawRect.top,m_rcDrawRect.right,m_rcDrawRect.bottom);
	
	int x,y;

	DWORD dwTime = xtimeGetTime() - m_dwBeginTime;

	stPointI ptSceneObject,ptMapObject;
	stPointI ptMapPixelOffset = GetPixelOffset();
	stPointI ptOffsetPixel(rcDraw.left * GRID_WIDTH - ptMapPixelOffset.x,rcDraw.top * GRID_HEIGHT - ptMapPixelOffset.y);

	y = ptOffsetPixel.y + GRID_HEIGHT;
	for( size_t i = rcDraw.top; i < rcDraw.bottom;++i )
	{
		ptMapObject.y = i;
		x = ptOffsetPixel.x;
		for( size_t j = rcDraw.left; j < rcDraw.right;++j,x += GRID_WIDTH )
		{
			ptMapObject.x = j;

			stMapTileItemEx* pTile = m_map.GetTile(ptMapObject.x,ptMapObject.y);
			if( pTile->flags & TILE_TERR)
			{
				if( (size_t)pTile->objectLib < m_aObjectPack.size() && m_aObjectPack[pTile->objectLib] )
				{
					size_t group,frame;
					GetTileGroupFrame(pTile->object,group,frame);
					DrawMapObjectAni(m_aObjectPack[pTile->objectLib],group,frame,x,y,dwTime,dwColor);
				}

				stMapTileOther* pOther = pTile->pOther;
				if(pOther && (size_t)pOther->objectLib2 < m_aObjectPack.size() && m_aObjectPack[pOther->objectLib2])
				{
					size_t group,frame;
					GetTileGroupFrame(pOther->object2,group,frame);
					DrawMapObjectAni(m_aObjectPack[pOther->objectLib2],group,frame,x,y,dwTime,dwColor);
				}
			}
		}

		y += GRID_HEIGHT;
	}
	GetDevice()->EndFlush();
	//*/
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	描述: 		  绘制后景物体
*/
void CMapScene::DrawLowestObjects(DWORD dwColor)
{
	for(tListDrawObjects::iterator it = m_aDrawLowest.begin(); it != m_aDrawLowest.end(); ++it)
	{
		if ( !( !IsNeedDrawMagicEffect() && (*it)->IsSceneObjectType( OBJECT_MAGIC ) ) )
			(*it)->Draw(GetPixelOffset(),dwColor);
	}
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	描述: 		  绘制前景物体
*/
void CMapScene::DrawFrontestObjects(DWORD dwColor)
{
	for(tListDrawObjects::iterator it = m_aDrawFrontest.begin(); it != m_aDrawFrontest.end(); ++it)
	{
		if ( !( !IsNeedDrawMagicEffect() && (*it)->IsSceneObjectType( OBJECT_MAGIC ) ) )
			(*it)->Draw(GetPixelOffset(),dwColor);
	}
}

void CMapScene::DrawAboveAllObjects(DWORD dwColor)
{
	for(tListDrawObjects::iterator it = m_aDrawAboveAll.begin(); it != m_aDrawAboveAll.end(); ++it)
	{
		if ( !( !IsNeedDrawMagicEffect() && (*it)->IsSceneObjectType( OBJECT_MAGIC ) ) )
			(*it)->Draw(GetPixelOffset(),dwColor);
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ptSceneObject : 描述
 * \param ptMapObject : 描述
 * \return 返回值的描述
 */
inline int compareObject(POINT ptSceneObject,POINT ptMapObject)
{
	long v1 = (ptSceneObject.y << 16) | ptSceneObject.x;
	long v2 = (ptMapObject.y << 16) | ptMapObject.x;
	return v1 - v2;
	////ptSceneObject.y += 32;
	//if(ptSceneObject.y < ptMapObject.y) return -1;
	//if(ptSceneObject.y > ptMapObject.y) return 1;
	//if(ptSceneObject.x < ptMapObject.x) return -1;
	//if(ptSceneObject.x > ptMapObject.x) return 1;
	//return 0;
}

inline bool CMapScene::IsConverAtPixel(POINT pt)
{
	pt.x /= ConverPixelSize;
	pt.y /= ConverPixelSize;
	if(pt.x >= 0 && pt.x < m_converWidth &&
		pt.y >= 0 && pt.y < m_converHeight)
	{
		return ( m_convers[pt.y * m_converWidth + pt.x] == 0 ? false : true ) ;
	}
	return false;
}

inline void CMapScene::AddConverAtPixelRect(const RECT & rcConverPixel)
{
	int l,t,r,b;
	l = max(rcConverPixel.left/ConverPixelSize,0);
	t = max(rcConverPixel.top/ConverPixelSize,0);
	r = min(rcConverPixel.right/ConverPixelSize,m_converWidth-1);
	b = min(rcConverPixel.bottom/ConverPixelSize,m_converHeight-1);
	for(int i = t; i <=b; ++i){
		for(int j = l; j <= r; ++j){
			m_convers[i * m_converWidth + j] = 1;
		}
	}
}

inline bool DrawMapObject(IBitmapList* pBitmaps,int group,int frame,int x,int y,DWORD dwTime,DWORD dwColor,RECT& rcConverPixel)
{
	IBitmaps* pAni = pBitmaps->GetBitmaps(group,frame);
	if(!pAni) return false;
	IBitmap* pBmp = pAni->GetBitmapByTime(dwTime);
	if(!pBmp) return false;
	y -= pBmp->GetHeight();
	stRectI rcBmpClip = pBmp->GetClipRect();
	rcConverPixel.left = x + rcBmpClip.left;
	rcConverPixel.top = y + rcBmpClip.top;
	rcConverPixel.right = x + rcBmpClip.right;
	rcConverPixel.bottom = y + rcBmpClip.bottom;
	pBmp->Render(x,y,dwColor,Blend_Null);
	return true;
}
/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	描述: 		  绘制物体
*/
void CMapScene::DrawObjects(DWORD dwColor)
{
	memset(&m_convers[0],0,m_convers.size());

	stRectI rcDraw(m_rcDrawRect.left,m_rcDrawRect.top,m_rcDrawRect.right,m_rcDrawRect.bottom + SCREEN_GRID_HEIGHT * 3);
	if(rcDraw.bottom > m_map.GetHeight()) rcDraw.bottom = m_map.GetHeight();
	int indexObject = 0;
	int x,y;
	DWORD dwTime = xtimeGetTime() - m_dwBeginTime;

	stPointI ptSceneObject,ptMapObject;
	stPointI ptMapPixelOffset = GetPixelOffset();
	stPointI ptOffsetPixel(rcDraw.left * GRID_WIDTH - ptMapPixelOffset.x,rcDraw.top * GRID_HEIGHT - ptMapPixelOffset.y);

	y = ptOffsetPixel.y + GRID_HEIGHT;
	for( size_t i = rcDraw.top; i < rcDraw.bottom;++i )
	{
		ptMapObject.y = i;
		x = ptOffsetPixel.x;
		for( size_t j = rcDraw.left; j < rcDraw.right;++j,x += GRID_WIDTH )
		{
			ptMapObject.x = j;

			// 把排序在这个物件后面的物体绘制
			for(;;)
			{
				if(indexObject >= m_aDrawObjects.size()) break;
				ptSceneObject = m_aDrawObjects[indexObject]->GetSortPos();
				int nCmp = compareObject(stPointI(ptSceneObject.x / GRID_WIDTH , (ptSceneObject.y + 32) / GRID_HEIGHT) ,ptMapObject);
				if( nCmp <= 0 )
				{
					if ( !( !IsNeedDrawMagicEffect() && m_aDrawObjects[indexObject]->IsSceneObjectType( OBJECT_MAGIC ) ) )
					{
						m_aDrawObjects[indexObject]->Draw(ptMapPixelOffset,dwColor);
					}
					indexObject++ ;
				}
				else
					break;
			}

			stMapTileItemEx* pTile = m_map.GetTile(ptMapObject.x,ptMapObject.y);
			////if( 0 == (pTile->flags & TILE_TERR) )
			{
				if((size_t)pTile->objectLib < m_aObjectPack.size() && m_aObjectPack[pTile->objectLib])
				{
					size_t group,frame;
					GetTileGroupFrame(pTile->object,group,frame);
					stRectI rcConver;

					if(DrawMapObject(m_aObjectPack[pTile->objectLib],group,frame,x,y,dwTime,dwColor,rcConver))
						AddConverAtPixelRect(rcConver);
					//DrawMapObjectAni(m_aObjectPack[pTile->objectLib],group,frame,x,y,dwTime,dwColor);
				}

				stMapTileOther* pOther = pTile->pOther;
				if(pOther && (size_t)pOther->objectLib2 < m_aObjectPack.size() && m_aObjectPack[pOther->objectLib2])
				{
					size_t group,frame;
					GetTileGroupFrame(pOther->object2,group,frame);
					stRectI rcConver;
					if(DrawMapObject(m_aObjectPack[pOther->objectLib2],group,frame,x,y,dwTime,dwColor,rcConver))
						AddConverAtPixelRect(rcConver);
					//DrawMapObjectAni(m_aObjectPack[pOther->objectLib2],group,frame,x,y,dwTime,dwColor);
				}

				//sky 兼容重置版地图
				st2MapTileOther* p2Other = pTile->p2Other;
				if (p2Other && (size_t)p2Other->objectLib2 < m_aObjectPack.size() && m_aObjectPack[p2Other->objectLib2])
				{
					size_t group, frame;
					GetTileGroupFrame(p2Other->object2, group, frame);
					stRectI rcConver;
					if (DrawMapObject(m_aObjectPack[p2Other->objectLib2], group, frame, x, y, dwTime, dwColor, rcConver))
						AddConverAtPixelRect(rcConver);
					//DrawMapObjectAni(m_aObjectPack[pOther->objectLib2],group,frame,x,y,dwTime,dwColor);
				}
			}

		}
		y += GRID_HEIGHT;
	}

	for(;;)
	{
		if(indexObject >= m_aDrawObjects.size()) break;

		if ( !( !IsNeedDrawMagicEffect() && m_aDrawObjects[indexObject]->IsSceneObjectType( OBJECT_MAGIC ) ) )
		{
			m_aDrawObjects[indexObject]->Draw(ptMapPixelOffset,dwColor);
		}
		indexObject++ ;		
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwColor : 描述
 * \return 返回值的描述
 */
void CMapScene::DrawCoverObject(DWORD dwColor)
{
	// 物体在物件后面，再画一个半透明的
	DWORD  dwAlpha = COLOR_ARGB(128,255,255,255);
	dwColor = ColorBlend(dwColor,dwAlpha);
	stPointI ptMapPixelOffset = GetPixelOffset();
	iCharacter::s_bDrawCover = true;

	GetDevice()->BeginFlush(Flush_NoCover);
	for(size_t i = 0; i < m_aDrawLowest.size();++i)
	{
		if(m_aDrawLowest[i]->IsSceneObjectType(OBJECT_NPC))
		{
			POINT pt = m_aDrawLowest[i]->GetPos();
			pt -= ptMapPixelOffset;
			if(IsConverAtPixel(pt))
				m_aDrawLowest[i]->Draw(ptMapPixelOffset,dwColor);
		}
	}
	GetDevice()->EndFlush();

	for(size_t i = 0; i < m_aDrawObjects.size();++i)
	{
		if(m_aDrawObjects[i]->IsSceneObjectType(OBJECT_NPC))
		{
			POINT pt = m_aDrawObjects[i]->GetPos();
			pt -= ptMapPixelOffset;
			if(IsConverAtPixel(pt))
				m_aDrawObjects[i]->Draw(ptMapPixelOffset,dwColor);
		}
	}

	iCharacter::s_bDrawCover = false;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x : 描述
 * \param y : 描述
 * \return 返回值的描述
 */
void CMapScene::SetCenterPos(int x,int y)
{
	m_ptPixelOffset.x = x - m_iPixelWidth/2;
	m_ptPixelOffset.y = y - m_iPixelHeight/2;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMapScene::CalculateDrawRect()
{
	stPointI ptMapPixelOffset = GetPixelOffset();
	
	// 绘制区域
	m_rcDrawPixelRect.SetRect(ptMapPixelOffset.x,ptMapPixelOffset.y,ptMapPixelOffset.x + m_iPixelWidth,ptMapPixelOffset.y + m_iPixelHeight);

	// 以网格尺寸对齐的绘制区域
	m_rcDrawRect.left = getMax( 0L , ptMapPixelOffset.x / GRID_WIDTH );
	m_rcDrawRect.top = getMax( 0L , ptMapPixelOffset.y / GRID_HEIGHT );
	m_rcDrawRect.right = getMin( (long)m_map.GetWidth() , ((ptMapPixelOffset.x + m_iPixelWidth) + GRID_WIDTH - 1)/GRID_WIDTH );
	m_rcDrawRect.bottom = getMin( (long)m_map.GetHeight() , ((ptMapPixelOffset.y + m_iPixelHeight) + GRID_HEIGHT - 1)/GRID_HEIGHT );

}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param time_eslasped : 描述
 * \return 返回值的描述
 */
void CMapScene::Run(long time_eslasped)
{
	m_ptAddinOffset = stPointI(0,0);
	GetMapSurfManager()->Run(time_eslasped/1000.0f);
	GetMapSoundManager()->Run(time_eslasped/1000.0f);

	/*if(m_bInitConver == false && GetDevice()->IsLoadQueueLowerEmpty())
	{
	DWORD t1 = xtimeGetTime();
	InitCover();
	Engine_WriteLogF("InitCover time %d\n",xtimeGetTime() - t1);
	m_bInitConver = true;
	}*/
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwColor : 描述
 * \return 返回值的描述
 */
void CMapScene::Draw(DWORD dwColor)
{
	CalculateDrawRect();
	DrawTerrain(dwColor);
	DrawTerrainEffect(dwColor);
	DrawSmTerrain(dwColor);
	GetMapSurfManager()->Draw(GetPixelOffset());
	
	DrawObjects(dwColor);
	DrawCoverObject(dwColor);
}



/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
// 初始化图包
void CMapScene::InitGraphicPack()
{
	GetMapSurfManager()->Init();
	GetMapSoundManager()->Init();
	DWORD t1 = xtimeGetTime();

	TiXmlDocument xml;
	Stream* pStream = OpenPackFileForRead("data\\datas.pak","datas\\graphics.xml");
	if(!pStream)
	{
		Engine_WriteLog("装载 graphics.xml 失败！\n");
		return ;
	}

	if(!LoadXMLFromStream(pStream,xml))
	{
		ClosePackFileForRead(pStream);
		Engine_WriteLog("解析 graphics.xml 失败！\n");
		return ;
	}

	TiXmlElement * graphics = xml.FirstChildElement("graphics");
	if(!graphics) return;
	TiXmlElement * mapNode = graphics->FirstChildElement("map");
	if(!mapNode) return;
	const char* szTerrain = mapNode->Attribute("terrain");
	char szFileName[MAX_PATH];
	const char* dataPath = "data\\";
	if(szTerrain) 
	{
		sprintf(szFileName,"%s%s.gl",dataPath,szTerrain);
		m_pTerrainPack = GetDevice()->FindResource(szFileName,c_nTerrResourceFlags,Usage_Terrain);
	}

	const char* szAlpha = mapNode->Attribute("alpha");
	if(szAlpha)
	{
		sprintf(szFileName,"%s%s.gl",dataPath,szAlpha);
		m_pTerrainAlphaPack = GetDevice()->FindResource(szFileName,c_nTerrResourceFlags,Usage_BlendTerrain);
	}

	const char* szSmTerrain = mapNode->Attribute("smterrain");
	if(szSmTerrain)
	{
		sprintf(szFileName,"%s%s.gl",dataPath,szSmTerrain);
		m_pSmTerrainPack = GetDevice()->FindResource(szFileName,c_nMapResourceFlags,Usage_TerrainSmall);
	}

	TiXmlElement* object = mapNode->FirstChildElement("object");
	m_aObjectPack.reserve(16);
	m_aObjectPack.resize(0);
	while(object)
	{
		int id;
		const char * szObject;
		if( (object->QueryIntAttribute("id",&id) == TIXML_SUCCESS) && (szObject = object->Attribute("name")) )
		{
			sprintf(szFileName,"%s%s.gl",dataPath,szObject);
			IBitmapList* pBmpList = GetDevice()->FindResource(szFileName,c_nMapResourceFlags,Usage_Building);
			if(pBmpList)
			{
				while(m_aObjectPack.size() < id + 1)
					m_aObjectPack.push_back(NULL);
				m_aObjectPack[id] = pBmpList;
			}
		}
		object = object->NextSiblingElement("object");
	}

	Engine_WriteLogF("InitGraphicPack time %d\n",xtimeGetTime() - t1);

	struct stGrpPackInfo{
		char szFileName[MAX_PATH];
		TextureUsageType usage;
	};
	stGrpPackInfo aMagicPack[] = {
		{"data\\body.gl",Usage_Character},
		{"data\\hair.gl",Usage_Character},
		{"data\\npc.gl",Usage_Npc},
		{"data\\magic.gl",Usage_Magic},
		{"data\\weaponl.gl",Usage_Character},
		{"data\\weaponr.gl",Usage_Character},
		{"data\\pattern\\body1.gl",Usage_Character},
		{"data\\pattern\\body1.gl",Usage_Character},
		{"data\\pattern\\npc.gl",Usage_Npc},
	};

	for(size_t i=0; i< count_of(aMagicPack);++i)
	{
		GetDevice()->FindResource(aMagicPack[i].szFileName,c_nMagicResourceFlags,aMagicPack[i].usage);
	}

	ClosePackFileForRead(pStream);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x : 描述
 * \param y : 描述
 * \return 返回值的描述
 */
int CMapScene::GetMusicID(int x,int y)
{
	if((size_t)x < (size_t)m_map.GetWidth() && (size_t)y < (size_t)m_map.GetHeight())
		return m_map.GetTile(x,y)->type;
	return 0;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
int CMapScene::GetZoneID(POINT pt)
{
	if((size_t)pt.x < (size_t)m_map.GetWidth() && (size_t)pt.y < (size_t)m_map.GetHeight())
		return m_map.GetTile(pt.x,pt.y)->zone;
	return -1;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
bool CMapScene::IsMagicObstacle(POINT pt)
{
	if((size_t)pt.x < (size_t)m_map.GetWidth() && (size_t)pt.y < (size_t)m_map.GetHeight())
		return (m_map.GetTile(pt.x,pt.y)->flags & TILE_MAGIC_BLOCK)!=0;
	return true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
/*__declspec( naked )*/ bool CMapScene::IsDynamicObstacle(POINT pt)
{
	int retval;
	//CRYPT_START
	retval = 0;
	if((size_t)pt.x < (size_t)m_map.GetWidth() && (size_t)pt.y < (size_t)m_map.GetHeight())
	{
		//if( !m_rcFindPathRect.PtInRect(pt) ) return true;
		retval = MapTileItem_GetDBlockNum(m_map.GetTile(pt.x,pt.y)) != 0;
	}
	__asm mov eax , retval;
	//CRYPT_END
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pt : 描述
* \return 返回值的描述
*/
/*__declspec( naked )*/ bool CMapScene::IsStaticObstacle(POINT pt)
{
	int retval;
	//CRYPT_START
	retval = 0;
	//if( !m_rcFindPathRect.PtInRect(pt) ) return true;
	if((size_t)pt.x < (size_t)m_map.GetWidth() && (size_t)pt.y < (size_t)m_map.GetHeight())
	{
		stMapTileItemEx * st = m_map.GetTile(pt.x,pt.y);

		bool b = ((st->flags & TILE_BLOCK)!=0);
		if( b && (!m_bDoorBlock) && (m_map.GetTile(pt.x,pt.y)->flags & TILE_DOOR) )
		{
			retval = 0;
		}
		else
			retval = b;
	}
	__asm mov eax, retval
	//CRYPT_END
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
void CMapScene::AddDynamicObstacle(POINT pt)
{
	if((size_t)pt.x < (size_t)m_map.GetWidth() && (size_t)pt.y < (size_t)m_map.GetHeight())
	{
		stMapTileItemEx* p = m_map.GetTile(pt.x,pt.y);
		MapTileItem_SetDBlockNum(p,MapTileItem_GetDBlockNum(p) + 1);
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
void CMapScene::RemoveDynamicObstacle(POINT pt)
{
	if((size_t)pt.x < (size_t)m_map.GetWidth() && (size_t)pt.y < (size_t)m_map.GetHeight())
	{
		stMapTileItemEx* p = m_map.GetTile(pt.x,pt.y);
		MapTileItem_SetDBlockNum(p,MapTileItem_GetDBlockNum(p) - 1);
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ptStart : 描述
 * \param ptEnd : 描述
 * \param POINT : 描述
 * \param aPoints : 描述
 * \return 返回值的描述
 */
void CMapScene::GetALine(POINT ptStart,POINT ptEnd,std::vector< POINT ,std::stack_allocator<POINT> > &aPoints)
{
	int dx = (ptEnd.x-ptStart.x)*2;
	int dy = ptEnd.y-ptStart.y;
	int adx = abs(dx);
	int ady = abs(dy);

	if(adx == 0 && ady == 0)
		return;
	if(adx >= ady){
		int _dx = dx < 0 ? -1 : 1;
		for(int i=0; i<adx; ++i){
			POINT pt;
			pt.x = ptStart.x + i * _dx/2;
			pt.y = ptStart.y + i * dy/adx;
			if( i != 0 ){
				POINT & pt2 = aPoints.back();
				if( pt.x == pt2.x && pt.y == pt2.y)
					continue;
			}
			aPoints.push_back(pt);
		}
	}else{
		int _dy = dy < 0 ? -1 : 1;
		for(int i=0; i<ady; ++i){
			POINT pt;
			pt.x = ptStart.x + i * dx/ady/2;
			pt.y =  ptStart.y + i * _dy;
			if( i != 0 ){
				POINT & pt2 = aPoints.back();
				if( pt.x == pt2.x && pt.y == pt2.y)
					continue;
			}
			aPoints.push_back(pt);
		}
	}

	/*aPoints.resize(0);
	aPoints.reserve(128);
	int step;

	if(abs(ptStart.x - ptEnd.x) >= abs(ptStart.y - ptEnd.y))
		step = abs(ptEnd.x - ptStart.x);
	else step = abs(ptEnd.y - ptStart.y);

	float fDx,fDy;

	fDx = float(ptEnd.x - ptStart.x)/(float)step;
	fDy = float(ptEnd.y - ptStart.y)/(float)step;

	stPointI pt;
	float fX  = ptStart.x,fY = ptStart.y;

	for(int i=0;i<step;++i)
	{
		pt = stPointI(fX,fY);
		aPoints.push_back(pt);
		fX += fDx;
		fY += fDy;
	}*/
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ptStart : 描述
 * \param ptEnd : 描述
 * \param ptObs : 描述
 * \param dwTileFlags : 描述
 * \return 返回值的描述
 */
int CMapScene::GetTileFlagsNum(POINT ptStart,POINT ptEnd,POINT & ptObs,DWORD dwTileFlags)
{
	SetFrameAllocator fa;
	std::vector< POINT ,std::stack_allocator<POINT> > aPoints;
	GetALine(ptStart,ptEnd,aPoints);

	int count = 0;
	for( size_t i=0;i<aPoints.size();++i )
	{
		stMapTileItemEx* p = m_map.GetTile(aPoints[i].x,aPoints[i].y);
		if ( NULL == p )
		{
			break;
		}

		if((p->flags & dwTileFlags) == dwTileFlags )
		{
			if(count == 0) ptObs = aPoints[i];
			++count;
		}
	}

	return count;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMapScene::OnDisplayModeChange()
{
	m_iPixelWidth = GetDevice()->GetWidth();
	m_iPixelHeight = GetDevice()->GetHeight();
	m_converWidth = (m_iPixelWidth + ConverPixelSize - 1)/ConverPixelSize;
	m_converHeight = (m_iPixelHeight + ConverPixelSize - 1)/ConverPixelSize;
	m_convers.resize(m_converWidth * m_converHeight,0);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMapScene::LoadMapTileInfo()
{
	TiXmlDocument doc;
	if(!doc.LoadFile(".\\data\\tiletype.xml"))
		return;
	TiXmlElement* type = doc.FirstChildElement("type");
	TiXmlElement* item = type->FirstChildElement("item");
	while(item)
	{
		int id;
		if(item->QueryIntAttribute("id",&id) == TIXML_SUCCESS)
		{
			stGroundTypeInfo & info = m_aGroundTypeInfo[id];
			TiXmlElement* music = item->FirstChildElement("music");
			if(music)
			{
				const char* first;
				TiXmlElement* walk,*run,*ride_walk,*ride_run;
				if(walk = music->FirstChildElement("walk"))
				{
					if(first = walk->Attribute("first"))
						_parse_str_num(info.music.walk,first);
				}

				if(run = music->FirstChildElement("run"))
				{
					if(first = run->Attribute("first"))
						_parse_str_num(info.music.run,first);
				}

				if(ride_walk = music->FirstChildElement("ride_walk"))
				{
					if(first = ride_walk->Attribute("first"))
						_parse_str_num(info.music.ride_walk,first);
				}

				if(ride_run = music->FirstChildElement("ride_run"))
				{
					if(first = ride_run->Attribute("first"))
						_parse_str_num(info.music.ride_run,first);
				}
			}
		}
		item = item->NextSiblingElement("item");
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
int CMapScene::GetMapTileType(POINT pt)
{
	if((size_t)pt.x < (size_t)m_map.GetWidth() && (size_t)pt.y < (size_t)m_map.GetHeight())
		return m_map.GetTile(pt.x,pt.y)->type;
	return 0;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \param dir : 描述
 * \param mul : 描述
 * \return 返回值的描述
 */
POINT GetPosDirPos(POINT pt,int dir, int mul)
{
	stPointI ptOffset = stPointI(Scene_GetDirOffset(dir)) * mul;
	ptOffset.x *= GRID_WIDTH;
	ptOffset.y *= GRID_HEIGHT;
	pt.x += ptOffset.x;
	pt.y += ptOffset.y;
	return pt ;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param list : 描述
 * \param ptStart : 描述
 * \param dir : 描述
 * \param mul : 描述
 * \param pExcept : 描述
 * \param bGetNpc : 描述
 * \return 返回值的描述
 */
void CMapScene::GetCharacter(tStackVectorICharacter& list, POINT ptStart, size_t dir, int mul, iCharacter* pExcept,bool bGetNpc)
{
	int n = dir;
	if (n < 0)
		n+=8;
	else if (n > 7)
		n-=8;

	stPointI ptPos = GetPosDirPos(ptStart,n,mul);

	iCharacter* p = GetICharacterByPos(ptPos);
	if ( p && p != pExcept && (bGetNpc || !p->IsNpc()) )
		list.push_back(p);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param eRange : 描述
 * \param list : 描述
 * \param ptStart : 描述
 * \param dir : 描述
 * \param pExcept : 描述
 * \return 返回值的描述
 */
void CMapScene::GetCharacter(RangeEnum eRange, tStackVectorICharacter & list, POINT ptStart, size_t dir, iCharacter* pExcept)
{
	bool bGetNpc = true;

	if(pExcept && pExcept->GetObjType() == iObj::eObjType_Character)
	{
		if(pExcept->IsNpc())
			bGetNpc = false;
	}

	switch(eRange) 
	{
	case Range_FullScreenDir:
		GetFullScreenCharacter(list,pExcept,bGetNpc);
		break;
	case Range_FullScreen:
		GetFullScreenCharacter(list,pExcept,bGetNpc);
		break;
	case Range_Here:	// 原地
		GetCharacter(list,ptStart,dir,0,pExcept,true);
		break;
	case Range_Cross:	// 十字
		{
			int adir[][2] = {
				{0,0},
				{-2,1},
				{0,1},
				{2,1},
				{4,1},
			};

			for(size_t i=0;i<count_of(adir);++i)
				GetCharacter(list,ptStart,adir[i][0] + dir,adir[i][1],pExcept,true);
		}
		break;
	case Range_Upright3:	// 与该方向垂直的3格
		{
			int adir[][2] = {
				{0,0},
				{-2,1},
				{2,1},
			};

			for(size_t i=0;i<count_of(adir);++i)
				GetCharacter(list,ptStart,adir[i][0] + dir,adir[i][1],pExcept,true);
		}
		break;
	case Range_Upright5:	// 与该方向垂直的5格
		{
			int adir[][2] = {
				{0,0},
				{-2,1},
				{-2,2},
				{2,1},
				{2,2},
			};

			for(size_t i=0;i<count_of(adir);++i)
				GetCharacter(list,ptStart,adir[i][0] + dir,adir[i][1],pExcept,true);
		}
		break;
	case Range_Fan3:
		{
			int adir[][2] = {
				{0,1},
				{-1,1},
				{1,1},
			};

			for(size_t i=0;i<count_of(adir);++i)
				GetCharacter(list,ptStart,adir[i][0] + dir,adir[i][1],pExcept,true);
		}
		break;
	case Range_33:
	case Range_Dir5:
	case Range_Dir4:
	case Range_Dir3:
	case Range_Dir2:
	case Range_Dir1:
	case Range_Dir:
	case Range_Circle:
	case Range_HalfCircle:
	default:
		{
			int mul = 1;	// 方向上的格子数
			int start = 0;	// 起始方向，相对当前方向减去的（逆时针）方向数
			int end = 1;	// 结束方向，相对当前方向加上的（顺时针）方向数＋1

			if (eRange >= Range_Dir1 && eRange <= Range_Dir5)
			{
				// 当前方向上的一条直线
				mul = eRange - Range_Dir1 + 1;
			}

			if (eRange == Range_Dir)
				mul = 8; // 当前方向的所有对象（8格范围）
			else if (eRange == Range_HalfCircle)
			{
				// 半圆4格
				start = 2;
				end = 3;
			}
			else if (eRange == Range_Circle || eRange == Range_33)
			{
				// 周围8格，或者 3*3区域
				end = 8;
			}

			for (int j = 1; j<=mul; j++)
			{
				for (int i = (int)dir-start; i<(int)dir+end; i++)
				{
					GetCharacter(list,ptStart,i,j,pExcept,true);
				}
			}

			if (eRange == Range_33)	// 中心9格，3*3区域
			{
				GetCharacter(list,ptStart,dir,0,pExcept,true);
			}
			break;
		}
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMapScene::AddLight()
{
	std::for_each(m_aDrawObjects.begin(),m_aDrawObjects.end(),std::bind2nd(std::mem_fun1<void,CSceneObject,POINT>(&CSceneObject::AddLight),m_ptPixelOffset));
	std::for_each(m_aDrawLowest.begin(),m_aDrawLowest.end(),std::bind2nd(std::mem_fun1<void,CSceneObject,POINT>(&CSceneObject::AddLight),m_ptPixelOffset));
	std::for_each(m_aDrawFrontest.begin(),m_aDrawFrontest.end(),std::bind2nd(std::mem_fun1<void,CSceneObject,POINT>(&CSceneObject::AddLight),m_ptPixelOffset));
	std::for_each(m_aDrawAboveAll.begin(),m_aDrawAboveAll.end(),std::bind2nd(std::mem_fun1<void,CSceneObject,POINT>(&CSceneObject::AddLight),m_ptPixelOffset));
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \param szZone : 描述
 * \return 返回值的描述
 */
inline POINT GetZoneNum(POINT pt,SIZE szZone)
{
	return stPointI( pt.x / szZone.cx, pt.y / szZone.cy);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param vecPoint : 描述
 * \param rcZone : 描述
 * \return 返回值的描述
 */
template < class tVecPoint >
void CMapScene::GetZonePointList( tVecPoint & vecPoint , stRectI rcZone)
{
	stRectI rcMap(0,0,GetWidth(),GetHeight());
	rcZone &= rcMap;
	POINT pt;
	for( pt.y = rcZone.top ; pt.y < rcZone.bottom; ++pt.y )
	{
		for( pt.x = rcZone.left; pt.x < rcZone.right; ++pt.x )
		{
			vecPoint.push_back(pt);
		}
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ptZone : 描述
 * \param szZone : 描述
 * \return 返回值的描述
 */
RECT CMapScene::GetZoneRect(POINT ptZone,SIZE szZone)
{
	stRectI rect( ptZone.x * szZone.cx ,ptZone.y * szZone.cy, ptZone.x * szZone.cx + szZone.cx ,ptZone.y * szZone.cy + szZone.cy);
	stRectI rcMap(0,0,GetWidth(),GetHeight());
	rect &= rcMap;
	return rect;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ptCurZone : 描述
 * \param nearZone : 描述
 * \param szZone : 描述
 * \return 返回值的描述
 */
template < class tVecPoint >
void CMapScene::GetNearZone(POINT ptCurZone,tVecPoint & nearZone,SIZE szZone)
{
	stRectI rcZoneNum ( 0 , 0 , ( GetWidth() + szZone.cx-1 ) / szZone.cx, ( GetHeight() + szZone.cy - 1 )/szZone.cy );

	for(int i=-1; i < 2; ++i)
	{
		for( int j=-1; j < 2; ++j)
		{
			stPointI pt( ptCurZone.x + j, ptCurZone.y + i );
			if( rcZoneNum.PtInRect(pt) )
			{
				nearZone.push_back(pt);
			}
		}
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param curZoneList : 描述
 * \param ptCurZone : 描述
 * \param addZone : 描述
 * \param removeZone : 描述
 * \param szZone : 描述
 * \return 返回值的描述
 */
template < class tVecPoint >
void CMapScene::UpdateCurZone( std::vector<stPointI> & curZoneList,POINT ptCurZone,tVecPoint & addZone , tVecPoint & removeZone,SIZE szZone)
{
	std::vector< stPointI, std::stack_allocator< stPointI > > nearZone;
	GetNearZone(ptCurZone,nearZone,szZone);

	for(size_t i=0; i < nearZone.size(); ++i)
	{
		bool bFind = false;
		for(size_t j=0; j < curZoneList.size(); ++j)
		{
			if(curZoneList[j] == nearZone[i])
			{
				bFind = true;
				break;
			}
		}

		if(!bFind)
		{
			addZone.push_back(nearZone[i]);
		}
	}

	for(size_t i=0; i < curZoneList.size(); ++i)
	{
		bool bFind = false;
		for(size_t j=0; j < nearZone.size(); ++j)
		{
			if( nearZone[j] == curZoneList[i])
			{
				bFind = true;
				break;
			}
		}

		if(!bFind)
		{
			removeZone.push_back(curZoneList[i]);
		}
	}

	curZoneList.resize(nearZone.size());
	for(size_t i=0; i < nearZone.size();++i)
	{
		curZoneList[i] = nearZone[i];
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ptPrev : 描述
 * \param ptCenter : 描述
 * \return 返回值的描述
 */
void CMapScene::InitNearbyConver( POINT ptPrev,POINT ptCenter )
{
	return;
}


/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  装载附近的地图图片
*/
void CMapScene::LoadNearbyImage( POINT ptPrev,POINT ptCenter )
{
	//*
	DWORD t1 = xtimeGetTime();
	//SIZE szZone = { 20,24 };
	SIZE szZone = { GetDevice()->GetWidth()/64 + 1,GetDevice()->GetHeight()/32 + 1 };

	stPointI ptZonePrev = GetZoneNum(ptPrev,szZone);
	stPointI ptCurZone = GetZoneNum(ptCenter,szZone);

	if( ptZonePrev == ptCurZone) return;

	//GetDevice()->LockResLoad();
	SetFrameAllocator fa;
	std::vector< stPointI, std::stack_allocator<stPointI> > addZone;
	std::vector< stPointI, std::stack_allocator<stPointI> > removeZone;
	UpdateCurZone(m_curLoadResZone,ptCurZone,addZone,removeZone,szZone);

	for( size_t i=0; i<removeZone.size();++i )
	{
		stRectI rect = GetZoneRect( removeZone[i],szZone );
		POINT pt;
		for( pt.y = rect.top ; pt.y < rect.bottom; ++pt.y )
		{
			for( pt.x = rect.left; pt.x < rect.right; ++pt.x )
			{
				GetMapSurfManager()->DeleteObject(pt);
				GetMapSoundManager()->DeleteObject(pt);
			}
		}
	}

	DWORD dwCurTime = xtimeGetTime();

	stPointI ptCenterGrid = GetCenterGridPos();
	int mainDir = GetMainCharacterDir();
	stPointI ptDir = c_ptDir[mainDir];
	float len = sqrtf(ptDir.x * ptDir.x + ptDir.y * ptDir.y);
	for( size_t i=0; i<addZone.size();++i )
	{
		stRectI rect = GetZoneRect( addZone[i],szZone );
		POINT pt;
		for( pt.y = rect.top ; pt.y < rect.bottom; ++pt.y )
		{
			for( pt.x = rect.left; pt.x < rect.right; ++pt.x )
			{
				stMapAddinData* p = m_map.GetAddinData(pt);
				if( p )
				{
					if( p->surf.bySurfType == MapSurfType_Offing || p->surf.bySurfType == MapSurfType_HighSeas )
					{
						GetMapSurfManager()->AddObject( pt,&p->surf );
					}
					if( p->music != (BYTE)-1 )
					{
						DWORD dwID = p->music;
						GetMapSoundManager()->AddObject(pt,&dwID);
					}
				}

//*
				stPointI ptDirCur = pt - ptCenterGrid;
				bool bLoad = true;
				/*if(ptDirCur.x == 0 && ptDirCur.y == 0)
				{
					bLoad = true;
				}
				else
				{
					float fAngle = acos( ( ptDirCur.x * ptDir.x + ptDirCur.y * ptDir.y ) / sqrtf( ptDirCur.x * ptDirCur.x + ptDirCur.y * ptDirCur.y ) * len );
					if(fAngle <= angle2radian(90)) bLoad = true;
				}*/
				if(bLoad)
				{
					if( (pt.x & 1) == 0 && (pt.y & 3) == 0 )
					{
						stMapTileTerrain* p = m_map.GetTerrain(pt.x,pt.y);
						if(p->tile != NULL_MAPIMGID)
						{
							size_t group,frame;
							GetTileGroupFrame(p->tile,group,frame);
							m_pTerrainPack->ThreadLoadAImage(group,frame);
						}

						stMapTileOther* pOther = m_map.GetTile(pt.x,pt.y)->pOther;
						if( pOther && pOther->effect != NULL_MAPIMGID && pOther->effectAlpha != NULL_MAPIMGID )
						{
//							IBitmaps* bmps,*alphas;
							size_t group,frame;

							GetTileGroupFrame(pOther->effect,group,frame);
							if(m_pTerrainPack)
								m_pTerrainPack->ThreadLoadAImage(group,frame);

							GetTileGroupFrame(pOther->effectAlpha,group,frame);
							if(m_pTerrainAlphaPack)
								m_pTerrainAlphaPack->ThreadLoadAImage(group,frame);
						}

						//sky 兼容重置版地图
						st2MapTileOther* p2Other = m_map.GetTile(pt.x, pt.y)->p2Other;
						if (p2Other && p2Other->effect != NULL_MAPIMGID && p2Other->effectAlpha != NULL_MAPIMGID)
						{
							//							IBitmaps* bmps,*alphas;
							size_t group, frame;

							GetTileGroupFrame(p2Other->effect, group, frame);
							if (m_pTerrainPack)
								m_pTerrainPack->ThreadLoadAImage(group, frame);

							GetTileGroupFrame(p2Other->effectAlpha, group, frame);
							if (m_pTerrainAlphaPack)
								m_pTerrainAlphaPack->ThreadLoadAImage(group, frame);
						}
					}
					
					if( (pt.y & 1) == 0 )
					{
						stMapTileSmTerrain* p = m_map.GetSmTerrain(pt.x,pt.y);
						if(p->smTile != NULL_SMTERRID)
						{
							size_t group,frame;
							GetTileGroupFrame(p->smTile,group,frame);
							if(m_pSmTerrainPack)
								m_pSmTerrainPack->ThreadLoadAImage(group,frame);
						}
					}

					stMapTileItemEx* pTile = m_map.GetTile(pt.x,pt.y);
					if((size_t)pTile->objectLib < m_aObjectPack.size() && m_aObjectPack[pTile->objectLib])
					{
						size_t group,frame;
						GetTileGroupFrame(pTile->object,group,frame);
						m_aObjectPack[pTile->objectLib]->ThreadLoadAImage(group  , frame);

					}

					stMapTileOther* pOther = pTile->pOther;
					if(pOther && (size_t)pOther->objectLib2 < m_aObjectPack.size() && m_aObjectPack[pOther->objectLib2])
					{
						size_t group,frame;
						GetTileGroupFrame(pOther->object2,group,frame);
						m_aObjectPack[pOther->objectLib2]->ThreadLoadAImage(group  , frame);
					}

					//sky 兼容重置版地图
					st2MapTileOther* p2Other = pTile->p2Other;
					if (p2Other && (size_t)p2Other->objectLib2 < m_aObjectPack.size() && m_aObjectPack[p2Other->objectLib2])
					{
						size_t group, frame;
						GetTileGroupFrame(p2Other->object2, group, frame);
						m_aObjectPack[p2Other->objectLib2]->ThreadLoadAImage(group, frame);
					}
				}
//*/
			}
		}
	} 
	//GetDevice()->UnlockResLoad();
	//*/
#if defined _DEBUG || defined RELEASE_TEST
	//Engine_WriteLogF("LoadNearbyImage time %d\n",xtimeGetTime() - t1);
#endif
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param loaded : 描述
 * \return 返回值的描述
 */
void CMapScene::GetImageIDSet(CMapScene::stLoaded & loaded)
{
	loaded.clear();
	for(size_t i=0;i<m_map.GetHeight();++i)
	{
		for(size_t j = 0; j< m_map.GetWidth();++j)
		{
			if( (j & (~1)) && (i & (~3)) )
			{
				// 大地表
				stMapTileTerrain* p = m_map.GetTerrain(j,i);
				if(p->tile != NULL_MAPIMGID)
				{
					size_t group,frame;
					GetTileGroupFrame(p->tile,group,frame);
					loaded.loadedTerr.push_back(group << 16 | frame);
				}

				stMapTileOther* pOther = m_map.GetTile(j,i)->pOther;
				if( pOther && pOther->effect != NULL_MAPIMGID && pOther->effectAlpha != NULL_MAPIMGID )
				{
//					IBitmaps* bmps,*alphas;
					size_t group,frame;

					GetTileGroupFrame(pOther->effect,group,frame);
					loaded.loadedTerr.push_back(group << 16 | frame);

					GetTileGroupFrame(pOther->effectAlpha,group,frame);
					loaded.loadedTerrAlpha.push_back(group << 16 | frame);
				}

				//sky 兼容重置版地图
				st2MapTileOther* p2Other = m_map.GetTile(j, i)->p2Other;
				if (p2Other && p2Other->effect != NULL_MAPIMGID && p2Other->effectAlpha != NULL_MAPIMGID)
				{
					//					IBitmaps* bmps,*alphas;
					size_t group, frame;

					GetTileGroupFrame(p2Other->effect, group, frame);
					loaded.loadedTerr.push_back(group << 16 | frame);

					GetTileGroupFrame(p2Other->effectAlpha, group, frame);
					loaded.loadedTerrAlpha.push_back(group << 16 | frame);
				}
			}

			if( i & (~1))
			{
				//小地表
				stMapTileSmTerrain* p = m_map.GetSmTerrain(j,i);
				if(p->smTile != NULL_SMTERRID)
				{
					size_t group,frame;
					GetTileGroupFrame(p->smTile,group,frame);
					loaded.loadedSmTerr.push_back(group << 16 | frame);
				}
			}

			stMapTileItemEx* pTile = m_map.GetTile(j,i);
			if((size_t)pTile->objectLib < m_aObjectPack.size() && m_aObjectPack[pTile->objectLib])
			{
				size_t group,frame;
				GetTileGroupFrame(pTile->object,group,frame);
				loaded.loadedObjects[pTile->objectLib].push_back(group << 16 | frame);
			}

			stMapTileOther* pOther = pTile->pOther;
			if(pOther && (size_t)pOther->objectLib2 < m_aObjectPack.size() && m_aObjectPack[pOther->objectLib2])
			{
				size_t group,frame;
				GetTileGroupFrame(pOther->object2,group,frame);
				loaded.loadedObjects[pOther->objectLib2].push_back(group << 16 | frame);
			}
			
            //sky 兼容重置版地图
			st2MapTileOther* p2Other = pTile->p2Other;
			if (p2Other && (size_t)p2Other->objectLib2 < m_aObjectPack.size() && m_aObjectPack[p2Other->objectLib2])
			{
				size_t group, frame;
				GetTileGroupFrame(p2Other->object2, group, frame);
				loaded.loadedObjects[p2Other->objectLib2].push_back(group << 16 | frame);
			}
		}
	}
	loaded.sort();
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
int __cdecl CMapScene::compareNum( const void * p1, const void * p2 )
{
	if( *((DWORD*)p1) == *((DWORD*)p2) ) return 0;
	if( *((DWORD*)p1) < *((DWORD*)p2) ) return -1;
	return 1;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pBitmapList : 描述
 * \param loaded : 描述
 * \param cur : 描述
 * \return 返回值的描述
 */
inline void eraseImageID(IBitmapList* pBitmapList,CMapScene::tImageIDSet & loaded,CMapScene::tImageIDSet & cur)
{
	for(size_t i=0;i<loaded.size();++i)
	{
		DWORD* pFind = (DWORD*)bsearch(&loaded[i],&cur[0],cur.size(),sizeof(DWORD),CMapScene::compareNum);
		if( !pFind )
		{
			DWORD dwImageID = loaded[i];
			pBitmapList->ReleaseAImage( dwImageID >> 16 , dwImageID & 0xffff );
		}
	}
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  装载地图上的所有图片
*/
void CMapScene::LoadAllImage()
{
	stLoaded* loaded = new stLoaded;
	
	// 获取图片
	GetImageIDSet(*loaded);

	// 释放无用的图片
	if(m_loaded)
	{
		eraseImageID(m_pTerrainPack,m_loaded->loadedTerr,loaded->loadedTerr);
		eraseImageID(m_pTerrainAlphaPack,m_loaded->loadedTerrAlpha,loaded->loadedTerrAlpha);
		eraseImageID(m_pSmTerrainPack,m_loaded->loadedSmTerr,loaded->loadedSmTerr);

		for(size_t i= 0; i< m_aObjectPack.size();++i)
			eraseImageID(m_aObjectPack[i],m_loaded->loadedObjects[i],loaded->loadedObjects[i]);
	}

	delete m_loaded;

	m_loaded = loaded;

	GetDevice()->LockResLoad();

	// 装载图片
	if(m_loaded->loadedTerr.size())
		m_pTerrainPack->ThreadLoadImage(&m_loaded->loadedTerr[0],m_loaded->loadedTerr.size());

	if(m_loaded->loadedTerrAlpha.size())
		m_pTerrainAlphaPack->ThreadLoadImage(&m_loaded->loadedTerrAlpha[0],m_loaded->loadedTerrAlpha.size());

	if(m_loaded->loadedSmTerr.size())
		m_pSmTerrainPack->ThreadLoadImage(&m_loaded->loadedSmTerr[0],m_loaded->loadedSmTerr.size());

	for(size_t i= 0; i< m_aObjectPack.size();++i)
	{
		if(m_loaded->loadedObjects[i].size())
			m_aObjectPack[i]->ThreadLoadImage(&m_loaded->loadedObjects[i][0],m_loaded->loadedObjects[i].size());
	}
	
	Engine_WriteLogF("ThreadLoadNum : %d\n",m_loaded->loadedTerr.size() + m_loaded->loadedSmTerr.size() + m_loaded->loadedObjects[0].size() + m_loaded->loadedTerrAlpha.size());

	GetDevice()->UnlockResLoad();
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  卸载地图上的图片
*/
void CMapScene::ReleaseAllImage()
{
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	描述: 		  刷新海浪信息
*/
void CMapScene::RefreshSurf(POINT ptCenter)
{
	/*DWORD t1 = xtimeGetTime();
	SIZE szZone = { 24,28 };

	stPointI ptZonePrev = GetZoneNum(ptPrev,szZone);
	stPointI ptCurZone = GetZoneNum(ptCenter,szZone);

	if( ptZonePrev == ptCurZone) return;

	SetFrameAllocator fa;
	std::vector< stPointI, std::stack_allocator<stPointI> > addZone;
	std::vector< stPointI, std::stack_allocator<stPointI> > removeZone;
	UpdateCurZone(m_curLoadResZone,ptCurZone,addZone,removeZone,szZone);

	for( size_t i=0; i<removeZone.size();++i )
	{
		stRectI rect = GetZoneRect( removeZone[i],szZone );
		POINT pt;
		for( pt.y = rect.top ; pt.y < rect.bottom; ++pt.y )
		{
			for( pt.x = rect.left; pt.x < rect.right; ++pt.x )
			{
				GetMapSurfManager()->DeleteObject(pt);
			}
		}
	}
	
	for( size_t i=0; i<addZone.size();++i )
	{
		stRectI rect = GetZoneRect( addZone[i],szZone );
		POINT pt;
		for( pt.y = rect.top ; pt.y < rect.bottom; ++pt.y )
		{
			for( pt.x = rect.left; pt.x < rect.right; ++pt.x )
			{
				stMapAddinData* p = m_map.GetAddinData(pt);
				if( p )
				{
					if(p->surf.bySurfType == MapSurfType_Offing || p->surf.bySurfType == MapSurfType_HighSeas )
					{
						GetMapSurfManager()->AddObject(pt,&p->surf);
					}

					if(p->music != (BYTE)-1)
					{
						DWORD dwID = p->music;
						GetMapSoundManager()->AddObject(pt,&dwID);
					}
				}
			}
		}
	}

	stRectI rect(ptCenter.x - 12,ptCenter.y - 14,ptCenter.x + 12,ptCenter.y + 14);
	GetMapSurfManager()->DeleteObject(&rect);
	GetMapSoundManager()->DeleteObject(&rect);
	POINT pt;
	for( pt.y = rect.top ; pt.y < rect.bottom; ++pt.y)
	{
		for(pt.x = rect.left; pt.x < rect.right; ++pt.x)
		{
			stMapAddinData* p = m_map.GetAddinData(pt);
			if( p )
			{
				if(p->surf.bySurfType == MapSurfType_Offing || p->surf.bySurfType == MapSurfType_HighSeas )
				{
					GetMapSurfManager()->AddObject(pt,&p->surf);
				}
				if(p->music != (BYTE)-1)
				{
					DWORD dwID = p->music;
					GetMapSoundManager()->AddObject(pt,&dwID);
				}
			}
		}
	}*/
}