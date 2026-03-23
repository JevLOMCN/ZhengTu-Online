#ifndef _LOADMAP_H_
#define _LOADMAP_H_

#include <vector>
#include "zType.h"

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
	((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |       \
	 ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */

#define MAP_MAGIC (MAKEFOURCC('X','M','P','S'))
#define MAP_VERSION 6 

struct stMapFileHeader
{
	DWORD magic;			/**< 文件标识  MAP_MAGIC */
	DWORD ver;				/**< 版本 MAP_VERSION */
	DWORD width;			/**< 宽度 */
	DWORD height;			/**< 高度 */
};

/**
 * \brief 格子定义
 */
#define TILE_BLOCK			0x01	// 阻挡点
#define TILE_MAGIC_BLOCK	0x02	// 魔法阻挡点
#define TILE_NOCREATE		0x04	// 不能建造点
#define TILE_DOOR			0x08	// 门
#define TILE_ENTRY_BLOCK	0x10	// 人物或者Npc阻挡
#define TILE_OBJECT_BLOCK	0x20	// 物品阻挡
#pragma  pack(1)
struct stSrvMapTile
{
	BYTE	flags;	// 格子属性
	BYTE	type;	// 格子类型
};
#pragma pack()

typedef std::vector<stSrvMapTile> zTiles;

bool LoadMap(const char* pszFileName,zTiles & aTiles,DWORD & width,DWORD & height);
#endif
