#include "LoadMap.h"
#include "Zebra.h"
#include "zMisc.h"

/**
 * \brief 读取地图文件
 * \param pszFileName: 地图文件名
 * \param aTiles: 地图块
 * \param width: 地图宽度
 * \param height: 地图高度
 * \return 如果成功返回true否则返回false
 */
bool LoadMap(const char* pszFileName,zTiles &aTiles,DWORD & width,DWORD & height)
{
	// aTiles : 二维数组,以行优先
	stMapFileHeader hdr;
	FILE* fp = fopen(pszFileName,"rb");
	if(!fp) return false;
	fread(&hdr,sizeof(hdr),1,fp);
	//if(hdr.magic != MAP_MAGIC || hdr.ver != MAP_VERSION)  
	//{
	//	Zebra::logger->debug("加载地图 %s 版本不正确", pszFileName);
	//	fclose(fp);
	//	return false;
	//}

	width = hdr.width;
	height = hdr.height;

	aTiles.resize(width * height);
	fread(&aTiles[0],aTiles.size() * sizeof(stSrvMapTile),1,fp);

	fclose(fp);
	return true;
}
