#ifndef _X_MAP_H
#define _X_MAP_H

#ifdef _PIC_CLEAN_TOOL

#include <assert.h>

#ifdef	Assert
	#undef	Assert
	#define Assert	assert
#else
	#define Assert	assert
#endif

#endif

class IBitmap;
class IBitmaps;
class IBitmapList;
class cBuild;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class Stream;

namespace oldmap{

struct MapHeader{
	unsigned long dwSize;				// 文件头长度
	unsigned long dwVersion;			// 版本
	unsigned long dwTileWidth;			// 以砖块为单位的场景宽度
	unsigned long dwTileHeight;			// 以砖块为单位的场景高度
	unsigned char  none[32 - 4 * 4];	// 其它数据
};

struct MapTile
{
	unsigned char bBlock;			// 是否阻挡
	//unsigned char bUsed;			// 是否使用
	unsigned char music;			// 音乐编号
	unsigned short smTile;			// 小 Tile 编号
	unsigned short tile;			// 大 Tile 编号
	union
	{
		struct {
			unsigned short objectPic;		// 建筑图片编号
			unsigned short objectPicLib;	// 建筑图库编号
		};
		unsigned long uObject;
	};
	unsigned short smTileLib;		// 小 Tile 图库
	unsigned short tileLib;			// 大 Tile 图库
};


struct MapTileEx : public MapTile
{
	IBitmap* pTexLarge;
	IBitmap* pTexSmall;
	cBuild* pBuild;
	bool bCover;
	void Release();
	void LoadRes();
};

bool LoadMap(const char* pszFileName,MapTile* pMapTiles,unsigned int * iTileWidth,unsigned int * iTileHeight);
int SaveMap(unsigned char* pBuf,MapTile* pMapTiles,int MapWidth,int MapHeight,int iWidth,int iHeight,int ver);

};

#define GRID_WIDTH  64
#define GRID_HEIGHT 32

#define SCREEN_GRID_WIDTH  13
#define SCREEN_GRID_HEIGHT 19

#define SCREEN_WIDTH (SCREEN_GRID_WIDTH * GRID_WIDTH)
#define SCREEN_HEIGHT (SCREEN_GRID_HEIGHT * GRID_HEIGHT)

#define MAP_VERSION_OLD 3
#define MAP_VERSION_OLD2 5
#define MAP_VERSION_OLD3 6
#define MAP_VERSION		7
#define MAP_VERSION_NEW1 10

#define NULL_MAPIMGID ((WORD)-1)
#define NULL_SMTERRID ((DWORD)-1)
#define NULL_BYMAPIMGID ((BYTE)-1)

#define MAP_MAGIC (MAKEFOURCC('X','M','A','P'))
#define MPS_MAGIC (MAKEFOURCC('X','M','P','S'))

struct stMapFileHeader{
	DWORD magic;
	DWORD ver;
	DWORD width;
	DWORD height;
};

#define TILE_BLOCK			0x1	// 阻档点
#define TILE_MAGIC_BLOCK	0x2	// 魔法不能过的点
#define TILE_CREATE			0x4	// 不能建的点
#define TILE_DOOR			0x8 // 门
#define TILE_TERR			0x80	// 这个格子是地面（人一定会踩在上面）
#pragma  pack(1)

enum enumSurfType{
	MapSurfType_None = -1, // 没有海浪
	MapSurfType_Offing,	   // 近海
	MapSurfType_HighSeas,  // 远海
};

// 海浪结构
struct stSurfData{
	BYTE	bySurfType;	// 海浪类型
	BYTE	byFrequency;// 产生频率(0,1,2,3,随机)
	WORD	byAngle;	// 海浪角度
};

// 场景附加数据
struct stMapAddinData{
	union{
		struct{
			WORD x;
			WORD y;
		};
		DWORD nodeNum;
	};
	BYTE  music;		// 场景音效
	stSurfData surf;	// 海浪
};



//新加数据
struct stMapTileOther10
{
	union
	{
		struct
		{
			WORD x;
			WORD y;
		};
		DWORD nodeNum;
	};

	BYTE  objectLib2;		// 建筑图库ID2
	WORD  object2;			// 建筑图片ID2
	DWORD effect;
	WORD  effectAlpha;
	WORD  aniID;			// 动画ID（同一ID是一组动画）
};

// 场景其它(稀疏的)数据
struct st2MapTileOther : public stMapTileOther10
{
	stPointI offset2;       // 建筑图片偏移2
};

//原格式
struct stMapTileOther
{
	union
	{
		struct
		{
			WORD x;
			WORD y;
		};
		DWORD nodeNum;
	};

	BYTE  objectLib2;		// 建筑图库ID2
	WORD  object2;			// 建筑图片ID2
	WORD  effect;
	WORD  effectAlpha;
	WORD  aniID;			// 动画ID（同一ID是一组动画）
};

struct stMapTile
{
	BYTE  flags;			// 属性
	BYTE  type;				// 地表类型
	//BYTE  music;			// 场景音效
	BYTE  objectLib;		// 建筑图库ID
	BYTE  objectLib2;		// 建筑图库ID2
	WORD  object;			// 建筑图片ID
	WORD  object2;			// 建筑图片ID2
	WORD  tile;				// 大地表图片ID
	DWORD  smTile;			// 小地表图片ID
	WORD  effect;
	WORD  effectAlpha;
	WORD  aniID;			// 动画ID（同一ID是一组动画）
	BYTE  zone;				// 在地图上的区域ID 
};

struct stMapTileTerrainOld
{
	WORD  tile;			// 大地表图片
	WORD  effect;
	WORD  effectAlpha;
};

struct stMapTileTerrain
{
	DWORD  tile;	       // 大地表图片 
};

struct stMapTileTerrain10
{
	WORD  tile;	       // 大地表图片
};

struct stMapTileSmTerrainOld{
	WORD	smTile;			// 小地表图片
};

struct stMapTileSmTerrain{
	DWORD	smTile;			// 小地表图片
};

struct stMapTileItemOld{
	union{
		struct{
			WORD  object;	// 建筑图片ID
			WORD  objectLib;// 建筑图库ID
		};
		DWORD dwObject;
	};
	BYTE  flags;			// 属性
	BYTE  type;				// 地表类型
	WORD  aniID;			// 动画ID（相同的ID是一组动画）
};

struct stMapTileItemOld2
{
	WORD  object;			// 建筑图片ID
	BYTE  objectLib;		// 建筑图库ID
	BYTE  flags;			// 属性
	BYTE  type;				// 地表类型
};
struct stMapTileItem10
{
	WORD  object;			// 建筑图片ID
	BYTE  objectLib;		// 建筑图库ID
	BYTE  flags;			// 属性
	BYTE  type;				// 地表类型
	BYTE  zone;				// 在地图上的区域ID
};

struct stMapTileItem
{
	WORD  object;			// 建筑图片ID
	BYTE  objectLib;		// 建筑图库ID
	BYTE  flags;			// 属性
	BYTE  type;				// 地表类型
	BYTE  zone;				// 在地图上的区域ID
};
struct stMapTileItemEx : public stMapTileItem
{
	//BYTE byDBlockAndCover;	// 动态阻挡的数量(低4位), 物件遮挡的数量(高4位)
	BYTE			byBlock;	// 物件遮挡的数量
	stMapTileOther* pOther;		// 其它数据
	st2MapTileOther* p2Other;		// 其它数据
};

#pragma pack()
inline int MapTileItem_GetDBlockNum(stMapTileItemEx * p)
{
	//return (p->byDBlockAndCover & 0xf);
	return p->byBlock;
}

inline void MapTileItem_SetDBlockNum(stMapTileItemEx * p,int num){
	//assert(num <= 0xf);
	//p->byDBlockAndCover = (BYTE)( (p->byDBlockAndCover & 0xf0) | (num & 0xf) );
	assert(num <= 0xff);
	p->byBlock = num;
}

struct stMap
{
	std::vector<stMapTileOther> m_others;
	std::vector<st2MapTileOther> m2_others;
	std::vector<BYTE>			m_buffer;
	std::vector<stMapAddinData> m_addins;
	int m_width;
	int m_height;
	stMapTileTerrain*	m_pTerrain;
	stMapTileSmTerrain* m_pSmTerrain;
	stMapTileItemEx*	m_pTiles;
	
	stMap()	: m_width(0), m_height(0), m_pTerrain(NULL), m_pSmTerrain(NULL)
	{
	}

	int GetWidth(){ return m_width;}
	int GetHeight(){ return m_height;}
	
	stMapTileTerrain*	GetTerrain(int x,int y)
	{
		assert(x < m_width && y < m_height);
		return &m_pTerrain[(y/4) * ((m_width+1)/2) + (x/2)];
	}
	stMapTileSmTerrain*	GetSmTerrain(int x,int y)
	{
		assert(x < m_width && y < m_height);
		return &m_pSmTerrain[(y/2) * m_width + x];
	}
	stMapTileItemEx*		GetTile(int x,int y)
	{
		assert(x < m_width && y < m_height);
		return &m_pTiles[y * m_width + x];
	}
	
	size_t CalculateTerrainCount(int width,int height)
	{
		return ((width + 1)/2) * ((height + 3)/4);
	}
	
	size_t CalculateSmTerrainCount(int width,int height)
	{
		return width * ((height + 1)/2);
	}

	size_t CalculateTileCount(int width,int height)
	{
		return width * height;
	}

	size_t CalculateBufferSize(int width,int height)
	{
		return CalculateTerrainCount(width,height) * sizeof(stMapTileTerrain) + 
			   CalculateSmTerrainCount(width,height) * sizeof(stMapTileSmTerrain) +
			   CalculateTileCount(width,height) * sizeof(stMapTileItemEx);
	}

	void Reset(int width,int height)
	{
		m_others.resize(0);
		m2_others.resize(0);
		m_addins.resize(0);
		m_buffer.resize(CalculateBufferSize(width,height));
		memset(&m_buffer[0],0,m_buffer.size());
		m_width = width;
		m_height = height;
		m_pTerrain = (stMapTileTerrain*)&m_buffer[0];
		m_pSmTerrain = (stMapTileSmTerrain*)&m_buffer[CalculateTerrainCount(width,height) * sizeof(stMapTileTerrain)];
		m_pTiles = (stMapTileItemEx*)&m_buffer[CalculateTerrainCount(width,height) * sizeof(stMapTileTerrain) + CalculateSmTerrainCount(width,height) * sizeof(stMapTileSmTerrain)];
	}

	bool LoadFromFile(const char* szFileName);
	bool LoadFromStream(Stream* pStream);
	bool SaveToFile(const char* szFileName);
	bool SaveToStream(Stream* pStream);

	stMapAddinData* GetAddinData(POINT pt);
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param group : 描述
 * \param frame : 描述
 * \return 返回值的描述
 */
inline DWORD GetTileID(size_t group,size_t frame)
{
	return group * 100 + frame;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param tile : 描述
 * \param group : 描述
 * \param frame : 描述
 * \return 返回值的描述
 */
inline void GetTileGroupFrame(DWORD tile,size_t & group,size_t & frame)
{
	group = tile / 100;
	frame = tile % 100;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pOldMap : 描述
 * \param pNewMap : 描述
 * \param num : 描述
 * \return 返回值的描述
 */
/*
inline void OldMapToNewMap(oldmap::MapTile* pOldMap,stMapTile* pNewMap,size_t num)
{
	for(size_t i=0;i<num;++i)
	{
		pNewMap[i].aniID = -1;
		pNewMap[i].dwObject = pOldMap[i].uObject;
		pNewMap[i].tile = pOldMap[i].tile;
		pNewMap[i].smTile = pOldMap[i].smTile;
		pNewMap[i].type = pOldMap[i].music;
		pNewMap[i].flags = 0;
		if(pOldMap[i].bBlock)
		{
			pNewMap[i].flags |= TILE_BLOCK;
		}
	}
}
//*/

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
inline void Map_GridCenterToPixel(POINT* pt)
{
	pt->x = pt->x * GRID_WIDTH + GRID_WIDTH/2;
	pt->y = pt->y * GRID_HEIGHT + GRID_HEIGHT/2;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
inline void Map_PixelToGrid(POINT* pt)
{
	pt->x /= GRID_WIDTH;
	pt->y /= GRID_HEIGHT;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param rc : 描述
 * \return 返回值的描述
 */
inline void Map_GridCenterToPixel(RECT* rc)
{
	Map_GridCenterToPixel((POINT*)rc);
	rc->right = (rc->right) * GRID_WIDTH - GRID_WIDTH/2;
	rc->bottom = (rc->bottom ) *GRID_WIDTH - GRID_HEIGHT/2;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param rc : 描述
 * \return 返回值的描述
 */
inline void Map_PixelToGrid(RECT * rc)
{
	Map_PixelToGrid((POINT*)rc);
	rc->right = (rc->right + GRID_WIDTH - 1)/GRID_WIDTH;
	rc->bottom = (rc->bottom + GRID_HEIGHT -1)/GRID_HEIGHT;
}

#endif

