
#include "../engine/engine.h"

#include ".\cground.h"
#include "map.h"
#include "..\magic\iobj.h"
#include "..\magic\icharacter.h"
#include "..\engine\ini.h"
#include "cBuild.h"


using namespace std;


#define _FREE(X){ if( (X) != NULL ) { free(X); (X)=NULL; } }
#define GRIDSCREEN_WIDTH			13
#define GRIDSCREEN_HEIGHT			19
/// °ë¸ö¸ñµãÏñËØ¿í
#define	GRID_HALFWIDTH			32
/// °ë¸ö¸ñµãÏñËØ¸ß
#define GRID_HALFHEIGHT			16
#define TILE_WIDTH			128
#define TILE_HEIGHT			128
#define SMTILE_WIDTH		64
#define SMTILE_HEIGHT		64
const int	iScreenWidth	=	800;
const int	iScreenHeight	=	600;

#define FILE_TERRAIN_SMALL  "graphics\\smTiles.gl"
#define FILE_TERRAIN		"graphics\\Tiles.gl"
#define INI_FILE "Graphics\\gallery.ini"

#define TILE_GROUP 100

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
cGround::cGround(void)
{
	m_pMapTile = NULL;
	m_nMapTileWidth = 0;
	m_nMapTileHeight = 0;

	m_iScreenWidth = 0;
	m_iScreenHeight = 0;
	m_iOffsetPixelX = 0;
	m_iOffsetPixelY = 0;

	SetRect(&rcBig,0,0,0,0);
	SetRect(&rcSmall,0,0,0,0);
	SetRect(&rcObject,0,0,0,0);

	for (int i=0; i<MAX_OBJ; i++)
		m_aResObj[i] = NULL;
	m_pResLarge = NULL;
	m_pResSmall = NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
cGround::~cGround(void)
{
	ClearGraphicFileInfo();
	ReleaseTile();
	m_pResLarge = NULL;
	m_pResSmall = NULL;
	for (int i=0; i<MAX_OBJ; i++)
	{
		m_aResObj[i] = NULL;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cGround::ReleaseTile()
{
	InvalidateDeviceObjects();
	_FREE(m_pMapTile);
}
extern bool IsFileExist(const char* pszFileName);
//inline bool IsFileExist(const char* pszFileName)
//{
//	DWORD dwFA = GetFileAttributes(pszFileName);
//	return ( (dwFA != 0xffffffff) && (dwFA & FILE_ATTRIBUTE_ARCHIVE) );
//}
void cGround::LoadGraphicFileInfo()
{
	int i=0;
	m_aTileFile.resize(0);
	m_aTileFile.reserve(16);
	while(1)
	{
		char szFile[266];
		if(i == 0)
			sprintf(szFile,"Graphics\\TILES.gl");
		else
			sprintf(szFile,"Graphics\\TILES%d.gl",i);
		if(IsFileExist(szFile))
		{
			IBitmapList* pRes = GetDevice()->FindResource(szFile,c_nMagicResourceFlags,Usage_Terrain);
			if(pRes) m_aTileFile.push_back(pRes);
		}
		else
		{
			break;
		}
		i++;
	}

	i = 0;
	m_aSmTileFile.resize(0);
	m_aSmTileFile.reserve(16);
	while(1)
	{
		char szFile[266];
		if(i == 0)
			sprintf(szFile,"Graphics\\SMTILES.gl");
		else
			sprintf(szFile,"Graphics\\SMTILES%d.gl",i);
		if(IsFileExist(szFile))
		{
			IBitmapList* pRes = GetDevice()->FindResource(szFile,c_nMagicResourceFlags,Usage_TerrainSmall);
			m_aSmTileFile.push_back(pRes);
		}
		else
		{
			break;
		}
		i++;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cGround::ClearGraphicFileInfo()
{
	size_t i;
	for(i=0;i<m_aTileFile.size();i++)
	{
		m_aTileFile[i] = NULL;
	}
	m_aTileFile.resize(0);
	for(i=0;i<m_aSmTileFile.size();i++)
	{
		m_aSmTileFile[i] = NULL;
	}
	m_aSmTileFile.resize(0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nGroup : ÃèÊö
 * \param nFrame : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline WORD cGround::GetTileNum(UINT nGroup,UINT nFrame)
{
	for(int i=m_aTileFile.size()-1;i>=0;--i)
	{
		if(m_aTileFile[i]->IsExist(nGroup,nFrame))
		{
			return i;
		}
	}
	return -1;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nGroup : ÃèÊö
 * \param nFrame : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline WORD cGround::GetSmTileNum(UINT nGroup,UINT nFrame)
{
	for(int i=m_aSmTileFile.size()-1;i>=0;--i)
	{
		if(m_aSmTileFile[i]->IsExist(nGroup,nFrame))
		{
			return i;
		}
	}
	return -1;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cGround::RefTileInfo()
{
	UINT count = this->m_nMapTileWidth * this->m_nMapTileHeight;
	MapTile* p = m_pMapTile;
	for(UINT i=0;i<count;++i,++p)
	{
		if(p->smTile && p->smTile!= 0xffff)
		{
			UINT nGroup = p->smTile / 100;
			UINT nFrame = p->smTile % 100;
			p->smTileLib = GetSmTileNum(nGroup,nFrame);
		}
		if(p->tile && p->tile != 0xffff)
		{
			UINT nGroup = p->tile / 100;
			UINT nFrame = p->tile % 100;
			p->tileLib = GetTileNum(nGroup,nFrame);
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptPos : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cGround::MoveTo(POINT ptPos) 
{
	if (m_iOffsetPixelX == ptPos.x && m_iOffsetPixelY == ptPos.y)
		return;
	//remove the texture and resource too much far away...

	m_iOffsetPixelX = ptPos.x; 
	m_iOffsetPixelY = ptPos.y;
	if (m_iOffsetPixelX < 0)
		m_iOffsetPixelX = 0;
	if (m_iOffsetPixelY < 0)
		m_iOffsetPixelY = 0;
	if ( m_iOffsetPixelX + iScreenWidth >  (int)(m_nMapTileWidth * GRID_WIDTH) )
		m_iOffsetPixelX	=	( m_nMapTileWidth * GRID_WIDTH ) - iScreenWidth;
	if ( m_iOffsetPixelY + iScreenHeight >  (int)(m_nMapTileHeight * GRID_HEIGHT) )
		m_iOffsetPixelY	=	( m_nMapTileHeight * GRID_HEIGHT ) - iScreenHeight;

	//Ground_GetMiniGround()->OnGroundMoveTo(stPointI(m_iOffsetPixelX,m_iOffsetPixelY));
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param filename : ÃèÊö
 * \param e : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cGround::Load(LPCSTR filename,LoadEmum e)
{
	char szFileName[256];
	sprintf(szFileName,"map\\%s.nmp",filename);

	UINT nMapTileWidth;
	UINT nMapTileHeight;

	if(!LoadMap(szFileName,NULL,&nMapTileWidth,&nMapTileHeight))
		return false;
	ReleaseTile();

	UINT oldw = m_nMapTileWidth;
	UINT oldh = m_nMapTileHeight;

	m_pMapTile = (MapTileEx*)malloc(sizeof(MapTileEx)* nMapTileWidth * nMapTileHeight);

	m_nMapTileWidth  = nMapTileWidth;
	m_nMapTileHeight = nMapTileHeight;
	m_iScreenWidth = m_nMapTileWidth / GRIDSCREEN_WIDTH + (m_nMapTileWidth % GRIDSCREEN_WIDTH ? 1 : 0);
	m_iScreenHeight = m_nMapTileHeight / GRIDSCREEN_HEIGHT + (m_nMapTileHeight % GRIDSCREEN_HEIGHT ? 1 : 0);
	memset(m_pMapTile,0,sizeof(MapTileEx)* nMapTileWidth * nMapTileHeight);

	MapTile* pMapTileTemp = (MapTile*)malloc(sizeof(MapTile)* nMapTileWidth * nMapTileHeight);
	if(!LoadMap(szFileName,pMapTileTemp,NULL,NULL))
		return false;

	MapTileEx* pDes = m_pMapTile;
	MapTile* pSrc = pMapTileTemp;
	for (unsigned int i=0; i<m_nMapTileWidth*m_nMapTileHeight; i++)
	{
		memcpy(pDes++,pSrc++,sizeof(MapTile));
	}
	free(pMapTileTemp);

	//¼ÆËãcoverµÄĞÅÏ¢¡£
	ComputeCover();

	//GetMiniMap()->OpenMap(filename);
	if (e == Load_MultiThread)
		;
	else if (e == Load_All)
		LoadAllRes(filename);

	//Ground_GetMiniGround()->Load(filename,m_nMapTileWidth*GRID_WIDTH,m_nMapTileHeight*GRID_HEIGHT);

	RefTileInfo();
	return true;
}

struct stLoad
{
	TextureUsageType eUsage;
	char szTexture[128];
	map<long,int> mapFrame;
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param load : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cGround::LoadResMap(stLoad& load)
{
	if (load.mapFrame.empty())
		return;
	GetDevice()->FindResource(load.szTexture,c_nMagicResourceFlags,load.eUsage);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cGround::UnloadAllRes()
{
	m_mapLoad.clear();
}

#define ToMapTile(a) (((a/100)<<16) | (a%100))
#define ToMapObj(a) (((a/128)<<16) | (a%128))

#define GetTileGroupFrame(tile,group,frame) group = (tile)/TILE_GROUP;frame = (tile)%TILE_GROUP;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
LPCSTR cGround::GetObjName(int id)
{
	const char* aObject[] = 
	{
		"Graphics\\Objects0.gl",
		"Graphics\\Objects1.gl",
		"Graphics\\Objects2.gl",
		"Graphics\\Objects3.gl",
	};
	ATLASSERT(id>=0 && id <MAX_GROUNDOBJ);
	return aObject[id];
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param aMap : ÃèÊö
 * \param szFolder : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void SaveAllRes(std::vector<std::string>& aMap,LPCSTR szFolder)
{
	/*
	map<long,int> smallLoad;
	map<long,int> largeLoad;
	map<long,int> objLoad[MAX_GROUNDOBJ];
	MapTile* pMapTileTemp = NULL;
	for (int i=0; i<aMap.size(); i++)
	{
		UINT nMapTileWidth;
		UINT nMapTileHeight;
		if(!LoadMap(aMap[i].c_str(),NULL,&nMapTileWidth,&nMapTileHeight))
			continue;
		if (pMapTileTemp)
			free(pMapTileTemp);
		pMapTileTemp = (MapTile*)malloc(sizeof(MapTile)* nMapTileWidth * nMapTileHeight);
		if(!LoadMap(aMap[i].c_str(),pMapTileTemp,NULL,NULL))
			continue;
		MapTile* p = pMapTileTemp;
		for (unsigned int i=0; i<nMapTileWidth*nMapTileHeight; i++,p++)
		{	
			if (p->smTile)
				smallLoad[ToMapTile(p->smTile)] = true;
			if (p->tile)
				largeLoad[ToMapTile(p->tile)] = true;
			if (p->uObject)
			{
				int nLib = (p->uObject & 0xffff0000)>>16;
				int nPic = (p->uObject & 0xffff);
				ATLASSERT(nLib < MAX_GROUNDOBJ && nLib >= 0);
				objLoad[nLib][ToMapObj(nPic)] = true;
			}
		}
	}
	if (pMapTileTemp)
		free(pMapTileTemp); 
	PackFile(FILE_TERRAIN,largeLoad,szFolder);
	PackFile(FILE_TERRAIN_SMALL,smallLoad,szFolder);
	for (int i=0; i<MAX_GROUNDOBJ; i++)
	{
		if (!objLoad[i].empty())
		{
			CString s = cGround::GetObjName(i);
			PackFile(s,objLoad[i],szFolder);
		}
	}
	//*/
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szFolder : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cGround::SaveAllRes(LPCSTR szFolder)
{
	/*
	stLoad smallLoad;
	stLoad largeLoad;
	stLoad objLoad[MAX_GROUNDOBJ];
	MapTileEx* p = m_pMapTile;
	for (unsigned int i=0; i<m_nMapTileWidth*m_nMapTileHeight; i++,p++)
	{	
		if (p->smTile)
			smallLoad.mapFrame[ToMapTile(p->smTile)] = true;
		if (p->tile)
			largeLoad.mapFrame[ToMapTile(p->tile)] = true;
		if (p->uObject)
		{
			int nLib = (p->uObject & 0xffff0000)>>16;
			int nPic = (p->uObject & 0xffff);
			ATLASSERT(nLib < MAX_GROUNDOBJ && nLib >= 0);
			objLoad[nLib].mapFrame[ToMapObj(nPic)] = true;
		}
	}
	PackFile(FILE_TERRAIN,largeLoad.mapFrame,szFolder);
	PackFile(FILE_TERRAIN_SMALL,smallLoad.mapFrame,szFolder);
	for (int i=0; i<MAX_GROUNDOBJ; i++)
	{
		if (m_aResObj[i])
			PackFile(m_aResObj[i]->GetFileName(),objLoad[i].mapFrame,szFolder);
	}
	//*/
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param filename : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cGround::LoadAllRes(LPCSTR filename)
{
	if (m_mapLoad[filename] == true)
		return;
	m_mapLoad[filename] = true;
	//release should higher than ground...
	stLoad smallLoad;
	smallLoad.eUsage = Usage_TerrainSmall;
	strcpy(smallLoad.szTexture,FILE_TERRAIN_SMALL);
	stLoad largeLoad;
	largeLoad.eUsage = Usage_Terrain;
	strcpy(largeLoad.szTexture,FILE_TERRAIN);
	stLoad objLoad[MAX_GROUNDOBJ];
	for (int i=0; i<MAX_GROUNDOBJ; i++)
	{
		objLoad[i].eUsage = Usage_Building;
		strcpy(objLoad[i].szTexture,GetObjName(i));
	}

	MapTileEx* p = m_pMapTile;
	for (unsigned int i=0; i<m_nMapTileWidth*m_nMapTileHeight; i++,p++)
	{	
		if (p->smTile)
			smallLoad.mapFrame[ToMapTile(p->smTile)] = true;
		if (p->tile)
			largeLoad.mapFrame[ToMapTile(p->tile)] = true;
		if (p->uObject)
		{
			int nLib = (p->uObject & 0xffff0000)>>16;
			int nPic = (p->uObject & 0xffff);
			ATLASSERT(nLib < MAX_GROUNDOBJ && nLib >= 0);
			objLoad[nLib].mapFrame[ToMapObj(nPic)] = true;
		}
	}

	LoadResMap(smallLoad);
	LoadResMap(largeLoad);
	for (int i=0; i<MAX_GROUNDOBJ; i++)
	{
		LoadResMap(objLoad[i]);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param order : ÃèÊö
 * \param ptOffset : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cGround::RenderTerrain(int order,const POINT &ptOffset)
{
	int ix,iy;
	int _ix,_iy;

	int offx,offy;

	int ioffsetx;
	int ioffsety;

	MapTileEx * pTile1;
	MapTileEx * pTile;

	int nGroup,nFrame;

	if (order == 0)
	{
		// Render Big Tile!!
		stRectI rRealLarge = m_rReal;
		ioffsetx	=	m_rRenderRect.left & 0xfffffffe;
		ioffsety	=	m_rRenderRect.top  & 0xfffffffc;
		rRealLarge.left &= 0xfffffffe;
		rRealLarge.top &= 0xfffffffc;

		offy = ioffsety * GRID_HEIGHT - m_iOffsetPixelY;
		offx = ioffsetx * GRID_WIDTH  - m_iOffsetPixelX;

		_iy   = offy;
		pTile1 = m_pMapTile + ioffsety * m_nMapTileWidth + ioffsetx;

		rcBig.left = ioffsetx;
		rcBig.top = ioffsety;
		rcBig.bottom = m_rRenderRect.bottom;
		rcBig.right = m_rRenderRect.right;

		for( iy	  = ioffsety; iy < m_rRenderRect.bottom; iy += 4 , _iy += TILE_HEIGHT, pTile1 += (m_nMapTileWidth << 2))
		{
			pTile = pTile1;
			_ix = offx;
			for( ix = ioffsetx; ix < m_rRenderRect.right;  ix +=2 ,_ix += TILE_WIDTH, pTile += 2)
			{
				WORD Tile	 =	pTile->tile;
				if(!Tile)
					continue;
				IBitmap* m_pTexture = pTile->pTexLarge;
				if (m_pTexture == NULL)
				{
					WORD TileLib =  pTile->tileLib;
					if(TileLib != 0xffff)
					{
						GetTileGroupFrame(Tile,nGroup,nFrame);
						m_pTexture = m_aTileFile[TileLib]->LoadFrame(nGroup,nFrame);
						pTile->pTexLarge = m_pTexture;
					}
					
				}
				if (m_pTexture)
				{
					POINT pt;
					pt.x	=	_ix + ptOffset.x;
					pt.y	=	_iy + ptOffset.y;
					m_pTexture->Render(pt.x,pt.y);
				}
			}
		}
	}

	//if(0)
	if (order == 1)
	{
		// Render smTile!!
		stRectI rRealSmall = m_rReal;

		ioffsetx	=	m_rRenderRect.left & 0xffffffff;
		ioffsety	=	m_rRenderRect.top  & 0xfffffffe;
		rRealSmall.left &= 0xffffffff;
		rRealSmall.top &= 0xfffffffe;

		offy = ioffsety * GRID_HEIGHT - m_iOffsetPixelY;
		offx = ioffsetx * GRID_WIDTH  - m_iOffsetPixelX;

		_iy = offy;
		iy = ioffsety;
		pTile1 = m_pMapTile + ioffsety * m_nMapTileWidth + ioffsetx;

		rcSmall.left = ioffsetx;
		rcSmall.top = ioffsety;
		rcSmall.bottom = m_rRenderRect.bottom ;
		rcSmall.right = m_rRenderRect.right;

		for( iy = ioffsety; iy < m_rRenderRect.bottom; iy += 2 , _iy += SMTILE_HEIGHT,pTile1 += (m_nMapTileWidth <<1))
		{
			_ix = offx;
			pTile = pTile1;
			for( ix = ioffsetx; ix < m_rRenderRect.right;  ix +=1 ,_ix += SMTILE_WIDTH,++pTile)
			{
				WORD smTile	 =	pTile->smTile;
				if(!smTile)
					continue;
				IBitmap* m_pTexture = pTile->pTexSmall;
				if (m_pTexture == NULL)
				{
					WORD TileLib =  pTile->smTileLib;
					if(TileLib != 0xffff)
					{
						GetTileGroupFrame(smTile,nGroup,nFrame);
						m_pTexture = m_aSmTileFile[TileLib]->LoadFrame(nGroup,nFrame);
						pTile->pTexSmall = m_pTexture;
					}
				}
				if(m_pTexture)
				{
					POINT pt;
					pt.x	=	_ix + ptOffset.x;
					pt.y	=	_iy + ptOffset.y;
					m_pTexture->Render(pt.x,pt.y);
				}
			}
		}
	}
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param aObj : ÃèÊö
 * \param ptOffset : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//¸ÃËã·¨²»ÊÇºÜºÃ£¬±È½ÏºÃµÄÊÇ°Ñobj·ÅÔÚ¸ñ×ÓÀïÃæ
void cGround::DrawObject(std::vector<iObj*>& aObj,const POINT &ptOffset)
{
	
	stRectF rc0;
	rc0 = *GetDevice()->GetRenderRect();
	stRectI rcGround(rc0.left,rc0.top,rc0.right,rc0.bottom);
	rcGround.OffsetRect(GetPos().x,GetPos().y);
	stRectI rcLoad = rcGround;
	rcLoad.left -= 128;
	rcLoad.right += 128;
	rcLoad.top -= 64;
	rcLoad.bottom += 64;

	//32- 64|

	int left = m_rRenderRect.left - (m_rRenderRect.left & 0x1);
	if(left < 0) left = 0;
	int bottom = m_rRenderRect.bottom + (GRIDSCREEN_HEIGHT * 3);
	if(bottom > m_nMapTileHeight)
		bottom = m_nMapTileHeight;
	rcObject.left = left;
	rcObject.bottom = bottom;
	rcObject.top = m_rRenderRect.top;
	rcObject.right = m_rRenderRect.right;

	int iObjIndex = 0;

	std::vector<iObj*> aObject;

	MapTileEx* pTile1 = m_pMapTile + rcObject.top * m_nMapTileWidth + rcObject.left;
	MapTileEx* pTile;
	for (int y = rcObject.top; y < rcObject.bottom; y++,pTile1 += m_nMapTileWidth)
	{
		pTile = pTile1;
		for (int x = rcObject.left; x < rcObject.right; x++, pTile++)
		{
			if(!pTile->uObject)
				continue;

			while(iObjIndex < aObj.size() &&
				(aObj[iObjIndex]->GetSortPos().y < y*GRID_HEIGHT ||
				aObj[iObjIndex]->GetSortPos().y == y*GRID_HEIGHT && aObj[iObjIndex]->GetSortPos().x <= x*GRID_WIDTH)
				)
				aObject.push_back(aObj[iObjIndex++]);

			if (pTile->pBuild == NULL)
			{
				pTile->pBuild = new cBuild();
				pTile->pBuild->SetPic(pTile->uObject,x,y);
			}
			cBuild* pBuild = pTile->pBuild;
			
			stRectI rcBuild;
			IBitmapList* pRes = FindResource((pTile->uObject & 0xffff0000)>>16);
			ATLASSERT(pRes);
			if (!pRes)
				continue;
			if (!pBuild->GetBuildRect(rcBuild,pRes))
				continue;
			stRectI rDraw = rcBuild & rcGround;
			if (rDraw.IsRectEmpty())
			{
				stRectI rLoad = rcBuild & rcLoad;
				if (!rLoad.IsRectEmpty())
				{
					pBuild->PreLoad(pRes,pTile->uObject & 0xffff);
//					int nPic = (pTile->uObject & 0xffff);
//					if (pRes->PreLoad(nPic/100,nPic%100))
//					{
//					}
				}
				continue;
			}
			aObject.push_back(pBuild);
		}
	}

	while(iObjIndex < aObj.size())
		aObject.push_back(aObj[iObjIndex++]);
	ATLASSERT(iObjIndex == aObj.size());
	
	POINT pt = {ptOffset.x + GetPos().x,ptOffset.y + GetPos().y};

	for(size_t i=0;i<aObject.size();++i)
	{
		if(aObject[i]->IsEnableShake())
			aObject[i]->Draw(pt);
		else
			aObject[i]->Draw(GetPos());
	}

	DrawCoverCharacter(aObject,ptOffset);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cGround::CalcRenderRect(void)
{
	m_rRenderRect.left	=	m_iOffsetPixelX / GRID_WIDTH;
	m_rRenderRect.top	=	m_iOffsetPixelY / GRID_HEIGHT;

	int iPixelWidth  =	iScreenWidth - (m_rRenderRect.left * GRID_WIDTH - m_iOffsetPixelX);
	int iPixelHeight =  iScreenHeight -(m_rRenderRect.top * GRID_HEIGHT - m_iOffsetPixelY);

	int width  = (iPixelWidth / GRID_WIDTH) + (iPixelWidth  % GRID_WIDTH  ? 1 : 0);
	int height = (iPixelHeight / GRID_HEIGHT)+ (iPixelHeight % GRID_HEIGHT ? 1 : 0);

	if(m_rRenderRect.left + width > (int)m_nMapTileWidth)
		width = m_nMapTileWidth - m_rRenderRect.left;

	if(m_rRenderRect.top + height > (int)m_nMapTileHeight)
		height = m_nMapTileHeight - m_rRenderRect.top;

	m_rRenderRect.right = m_rRenderRect.left + width;
	m_rRenderRect.bottom = m_rRenderRect.top + height;

	stRectI rEnlarge;
	rEnlarge = m_rRenderRect;
	rEnlarge.left -= 2;
	rEnlarge.top -= 2;
	rEnlarge.right += 2;
	rEnlarge.bottom += 2;
	stRectI rc(0,0,m_nMapTileWidth-1,m_nMapTileHeight-1);
	rEnlarge &= rc;

	m_rReal = m_rRenderRect;
	m_rRenderRect = rEnlarge;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cGround::FinalRelease()
{
	ReleaseTile();
	//Ground_GetMiniGround()->FinalRelease();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szFileName : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cGround::LoadPicLibInfo(const char* szFileName)
{
	for (int i=0; i<4; i++)
	{
		char sTemp[256];
		sprintf(sTemp,"graphics\\Objects%d.gl",i);
		if (i)	m_aResObj[i] = GetDevice()->FindResource(sTemp,c_nMagicResourceFlags,Usage_Building);
	}
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cGround::Create()
{
	//Ground_GetMiniGround()->CreateMiniGround();
	m_pResLarge = GetDevice()->FindResource(FILE_TERRAIN,c_nMagicResourceFlags,Usage_Terrain);
	ATLASSERT(m_pResLarge);
	m_pResSmall = GetDevice()->FindResource(FILE_TERRAIN_SMALL,c_nMagicResourceFlags,Usage_TerrainSmall);
	ATLASSERT(m_pResSmall);
	if (!LoadPicLibInfo(INI_FILE))
		return false;
	LoadGraphicFileInfo();
	return true;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptFrom : ÃèÊö
 * \param ptTo : ÃèÊö
 * \param ptNext : ÃèÊö
 * \param speed : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//OBstacle
//ÕÏ°­ºÍ×èµ²
int cGround::NextPos(POINT ptFrom, POINT ptTo,POINT& ptNext,int& speed)
{
	//POINT p1 = WorldToGrid(ptFrom);
	//POINT p2 = WorldToGrid(ptTo);
	stGridPoint pt;
	int d = NextGrid(ptFrom,ptTo,pt,speed);
	if (d == -1)
		return -1;
	ptNext = pt;
	return d;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cGround::IsStaticObstacle(stGridPoint pt)
{
	if (pt.x < 0 || pt.x >= m_nMapTileWidth)
		return false;
	if (pt.y < 0 || pt.y >= m_nMapTileHeight)
		return false;
	return IsStaticObstacle(pt.x,pt.y);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param x : ÃèÊö
 * \param y : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cGround::IsStaticObstacle(int x,int y)
{
//	return false;
	return (m_pMapTile[y* m_nMapTileWidth + x].bBlock );
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptFrom : ÃèÊö
 * \param ptTo : ÃèÊö
 * \param width : ÃèÊö
 * \param height : ÃèÊö
 * \param ptNext : ÃèÊö
 * \param speed : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int cGround::NextGrid(stGridPoint ptFrom, stGridPoint ptTo,int width, int height,stGridPoint& ptNext,int& speed)
{
	static POINT s_aPoint[] = 
	{
		{0,-1},
		{1,-1},
		{1,0},
		{1,1},
		{0,1},
		{-1,1},
		{-1,0},
		{-1,-1},		
	};

	if (ptFrom.x < 0 || ptFrom.x >= width || ptFrom.y < 0 || ptFrom.y >= height)
		return -1;
	if (ptTo.x < 0 || ptTo.x >= width || ptTo.y < 0 || ptTo.y >= height)
		return -1;
	int dx = ptTo.x - ptFrom.x;
	int dy = ptTo.y - ptFrom.y;
	if (abs(dx) == 0 && abs(dy) == 0)
		return -1;
	if (abs(dx) <= 1 && abs(dy) <= 1)
	{
		bool bObs;
		bObs = IsStaticObstacle(ptTo);
		if (bObs)
			return -1;
		if (GetGroundCallBack())
		{
			bObs = GetGroundCallBack()->IsDynamicObstacle(ptTo);
			if (bObs)
				return -1;
		}
	}

	int dir = GetDir8(dx,dy);
	if (dir == -1)
		return -1;
	if (speed == 0)
	{
		ptNext = ptFrom;
		return dir;
	}
	for (int i=0; i<4; i++)
	{
		for(int j=0; j<2; j++)
		{
			int d = dir+i;
			if (j == 1)
				d = dir-i;
			if (d < 0)
				d+=8;
			else if (d >= 8)
				d-=8;

			stGridPoint ptN;
			bool bObs = false;

			int n = 2;
			if (speed == 1)
				n = 1;
			int aSpeed[2] = {1,speed};
			for (int t=0; t<n ; t++)
			{
				POINT pt = s_aPoint[d];

				ptN = ptFrom;
				for (int m = 0; m < aSpeed[t]; m++)
				{
					ptN.x += pt.x;
					ptN.y += pt.y;
					bObs = IsStaticObstacle(ptN);
					if (bObs)
						break;
					if (GetGroundCallBack())
					{
						bObs = GetGroundCallBack()->IsDynamicObstacle(ptN);
						if (bObs)
							break;
					}
				}
				if (bObs)
				{
					if (t == 0)
						break;
					else
					{
						ptN = ptFrom;
						speed = aSpeed[t-1];
						pt.x *= speed;
						pt.y *= speed;
						ptN.x += pt.x;
						ptN.y += pt.y;
						bObs = false;
						break;
					}
				}
			}
			if (bObs == false)
			{
				ptNext = ptN;
				return d;
			}
		}
	}
	return -1;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptFrom : ÃèÊö
 * \param ptTo : ÃèÊö
 * \param ptNext : ÃèÊö
 * \param speed : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int cGround::NextGrid(stGridPoint ptFrom, stGridPoint ptTo,stGridPoint& ptNext,int& speed)
{
	return NextGrid(ptFrom,ptTo,m_nMapTileWidth, m_nMapTileHeight, ptNext,speed);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT cGround::InvalidateDeviceObjects()
{
	if(m_pMapTile != NULL)
	{
		MapTileEx* p = m_pMapTile;
		for (unsigned int i=0; i<m_nMapTileWidth*m_nMapTileHeight; i++)
		{
			p->Release();
			p++;
		}
	}
	//Ground_GetMiniGround()->InvalidateDeviceObjects();
	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT cGround::RestoreDeviceObjects() 
{
	//Ground_GetMiniGround()->RestoreDeviceObjects();
	return S_OK;
}

iGroundCallBack* g_pGroundCallBack = NULL;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param p : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
EXPORT_API void Ground_SetCallBack(iGroundCallBack* p)
{
	g_pGroundCallBack = p;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
iGroundCallBack* GetGroundCallBack()
{
	return g_pGroundCallBack;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void MapTileEx::Release()
{
	pTexSmall = NULL;
	pTexLarge = NULL;
	SAFE_RELEASE(pBuild);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT cGround::InvalidateResource()
{
	m_mapLoad.clear();
	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT cGround::RestoreResource()
{
	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int cGround::GetMusicID(stGridPoint pt)
{
	if (pt.x < 0 || pt.x >= m_nMapTileWidth)
		return 0;
	if (pt.y < 0 || pt.y >= m_nMapTileHeight)
		return 0;
	return m_pMapTile[pt.y* m_nMapTileWidth + pt.x].music;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param aObject : ÃèÊö
 * \param ptOffset : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cGround::DrawCoverCharacter(std::vector<iObj*>& aObject,const POINT& ptOffset)
{
	POINT pt1 = GetPos() + ptOffset;
	for (std::vector<iObj*>::iterator it = aObject.begin(); it != aObject.end(); ++it)
	{
		iObj* p = *it;
		iCharacter* pChar = ( p->GetObjType() == iObj::eObjType_Character ? (iCharacter*)(p) : NULL);
		if (pChar)
		{
			stGridPoint pt = pChar->GetPos();
			if (IsCovered(pt))
			{
				BYTE alpha = pChar->GetAlpha();
				if (alpha > 128)
				{
					
					pChar->ChangeAlpha(128);
					if(pChar->IsEnableShake())
						pChar->Draw(pt1);
					else
						pChar->Draw(GetPos());
					pChar->ChangeAlpha(alpha);
				}
			}
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param rc : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cGround::AddCover(const stRectI& rc)
{
	stGridPoint p1 = rc.TopLeft();
	stGridPoint p2 = rc.BottomRight();
	for (int i=p1.x; i<p2.x; i++)
	for (int j=p1.y; j<p2.y; j++)
	{
		m_pMapTile[j* m_nMapTileWidth + i].bCover = true;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cGround::ComputeCover()
{
	MapTileEx* pTile = m_pMapTile;
	stRectI rcMap(0,0,m_nMapTileWidth*GRID_WIDTH, m_nMapTileHeight*GRID_HEIGHT);
	for (unsigned int y=0; y<m_nMapTileHeight; y++)
	for (unsigned int x=0; x<m_nMapTileWidth; x++,pTile++)
	{
		if (!pTile->uObject)
			continue;
		pTile->pBuild = new cBuild();
		pTile->pBuild->SetPic(pTile->uObject,x,y);
		stRectI rcBuild;
		IBitmapList* pRes = FindResource((pTile->uObject & 0xffff0000)>>16);
		ATLASSERT(pRes);
		if (!pRes)
			continue;
		if (!pTile->pBuild->GetBuildRect(rcBuild,pRes))
			continue;
		//¼ÆËãobjµ²×¡ÁËÊ²Ã´ÎïÌå£¿
		rcBuild&=rcMap;
		AddCover(rcBuild);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cGround::IsCovered(stGridPoint pt)
{
	if (pt.x < 0 || pt.x >= m_nMapTileWidth)
		return 0;
	if (pt.y < 0 || pt.y >= m_nMapTileHeight)
		return 0;
	return m_pMapTile[pt.y* m_nMapTileWidth + pt.x].bCover;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param n : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
IBitmapList* cGround::FindResource(int n)
{
	if (n >= 0 && n < MAX_OBJ)
		return m_aResObj[n];
	return NULL;
}