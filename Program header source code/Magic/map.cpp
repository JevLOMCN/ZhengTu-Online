#include "../engine/include/engine.h"
#include ".\map.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <iostream>

using namespace std;

#if 0
namespace oldmap{
  
static bool LoadMap(const unsigned char* pBuf,MapTile* pMapTiles);
#define OPEN_FILE(f,file_name) (f.Create(file_name,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NORMAL))
// 装载地图
// 当pMapTiles 为空时返回地图的大小
// 
bool LoadMap(const char* szFile ,MapTile* pMapTiles,unsigned int * piTileWidth,unsigned int * piTileHeight)
{
	CAtlFile file;
	OPEN_FILE(file,szFile);
	if(pMapTiles == NULL)
	{
		MapHeader hdr;
		file.Read(&hdr,sizeof(MapHeader));
		*piTileWidth = hdr.dwTileWidth;
		*piTileHeight = hdr.dwTileHeight;
		return true;
	}
	else
	{
		unsigned char* pBuf ;
		ULONGLONG ulLen;
		file.GetSize(ulLen);
		int len = ulLen;
		pBuf = (unsigned char*)malloc(len);
		file.Read(pBuf,len);
		bool b = LoadMap(pBuf,pMapTiles);
		free(pBuf);
		return b;
	}
	return true;
}

static bool LoadMap(const unsigned char* pBuf,MapTile* pMapTiles)
{

	MapHeader *hdr;
	hdr = (MapHeader*)pBuf;
	pBuf += sizeof(MapHeader);

	unsigned int i,j;
	unsigned char btemp;
	switch(hdr->dwVersion)
	{
	case 1:
		for(i=0;i<hdr->dwTileHeight;++i)
		{
			for(j=0;j<hdr->dwTileWidth;++j,++pMapTiles)
			{
				pMapTiles->smTile = *(unsigned short*)pBuf;		pBuf += 2;
				pMapTiles->tile   = *(unsigned short*)pBuf;		pBuf += 2;
				pMapTiles->objectPic = *(unsigned short*)pBuf;	pBuf += 2;
				pMapTiles->objectPicLib=*(unsigned short*)pBuf;	pBuf += 2;
				pMapTiles->tileLib = 0xffff;
				pMapTiles->smTileLib = 0xffff;
				btemp = *pBuf;  pBuf +=1;
				
				//pMapTiles->bUsed = ( (btemp & 0x01) > 0 );
				pMapTiles->bBlock = ( (btemp & 0x02) > 0 );
				
				pMapTiles->music = *pBuf; pBuf += 3;
			}
		}
		break;
	case 2:
		for(i=0;i<hdr->dwTileHeight;++i)
		{
			for(j=0;j<hdr->dwTileWidth;++j,++pMapTiles)
			{
				pMapTiles->tileLib = 0xffff;
				pMapTiles->smTileLib = 0xffff;
				btemp = *pBuf; pBuf++;
				//pMapTiles->bUsed = true;
				pMapTiles->bBlock = ( (btemp & 0x01) > 0 );
				if( btemp & 0x02 )
				{
					pMapTiles->smTile = *(unsigned short*)pBuf;
					pBuf += 2;
				}
                else
                {
                	pMapTiles->smTile = 0;
                }

				if( btemp & 0x04 )
				{
					pMapTiles->tile = *(unsigned short*)pBuf;
					pBuf += 2;
				}
                else
                {
                	pMapTiles->tile = 0;
                }

				if( btemp & 0x08 )
				{
					pMapTiles->objectPic = *(unsigned short*)pBuf;
					pBuf += 2;
					pMapTiles->objectPicLib = *(unsigned short*)pBuf;
					pBuf += 2;
				}
                else
                {
                	pMapTiles->objectPic = 0;
                    pMapTiles->objectPicLib = 0;
                }
			}
		}
		break;
	case 3:
		for(i=0;i<hdr->dwTileHeight;++i)
		{
			for(j=0;j<hdr->dwTileWidth;++j,++pMapTiles)
			{
				pMapTiles->tileLib = 0xffff;
				pMapTiles->smTileLib = 0xffff;
				btemp = *pBuf; pBuf++;
				//pMapTiles->bUsed = true;
				pMapTiles->bBlock = ( (btemp & 0x01) > 0 );

				if( btemp & 0x02 )
				{
					pMapTiles->smTile = *(unsigned short*)pBuf;
					pBuf += 2;
				}
                else
                {
                	pMapTiles->smTile = 0;
                }

				if( btemp & 0x04 )
				{
					pMapTiles->tile = *(unsigned short*)pBuf;
					pBuf += 2;
				}
                else
                {
                	pMapTiles->tile = 0;
                }

				if( btemp & 0x08 )
				{
					pMapTiles->objectPic = *(unsigned short*)pBuf;
					pBuf += 2;
					pMapTiles->objectPicLib = *(unsigned short*)pBuf;
					pBuf += 2;
				}
                else
                {
                	pMapTiles->objectPic = 0;
                    pMapTiles->objectPicLib = 0;
                }

				if( btemp & 0x10 )
				{
					pMapTiles->music = *pBuf;
					pBuf ++;
				}
                else
                {
                	pMapTiles->music = 0;
                }
                
				if( btemp & 0x20 )
				{
					pBuf ++;
				}
				if( btemp & 0x40 )
				{
					pBuf ++;
				}
				if( btemp & 0x80 )
				{
					pBuf ++;
				}
			}
		}
		break;
	default:
		assert(0);
		return false;
	}
	return true;
}

int SaveMap(unsigned char* pBuf,MapTile* pMapTiles,int MapWidth,int MapHeight,int iWidth,int iHeight,int ver)
{
	int len = 0;
	int i,j;
    unsigned char *btemp;
    MapTile* p;
	switch(ver)
    {
    case 1:
    	for(i=0;i<iHeight;i++)
        {
        	p = pMapTiles;
        	for(j=0;j<iWidth;j++,p++)
            {
            	if(j >= MapWidth || i >= MapHeight)
                {
                	memset(pBuf,0,12);
                    pBuf+=8;
                    *pBuf = 0x2;
                    pBuf += 4;
                    len+=12;
                }
                else
                {
                	*(unsigned short*)pBuf = p->smTile;			pBuf+=2;
                    *(unsigned short*)pBuf = p->tile;       	pBuf+=2;
                    *(unsigned short*)pBuf = p->objectPic;  	pBuf+=2;
                    *(unsigned short*)pBuf = p->objectPicLib;	pBuf+=2;

                    *pBuf = 0;

                    if(p->bBlock) *pBuf |= 0x2;
                    /*if(p->bUsed)*/  *pBuf |= 0x1;

                    pBuf++;

                    *pBuf = p->music;
                    pBuf+=3; 
                    len+=12;
                    
                }
            }
            pMapTiles += MapWidth;
        }
	    break;
    case 2:
    	for(i=0;i<iHeight;i++)
        {
        	p = pMapTiles;
        	for(j=0;j<iWidth;j++,p++)
            {
            	if(j >= MapWidth || i >= MapHeight)
                {
                	*pBuf++ = 0x1;
                    len++;
                }
                else
                {
                    btemp = pBuf++;
                    len++;
                    *btemp = 0;
                    if(p->bBlock)
                    {
                        *btemp |= 0x1;
                    }
                    if(p->smTile)
                    {
                        *btemp |= 0x2;
                        *(unsigned short*)pBuf = p->smTile;
                        pBuf+=2;
                        len+=2;
                    }
                    if(p->tile)
                    {
                        *btemp |= 0x4;
                        *(unsigned short*)pBuf = p->tile;
                        pBuf += 2;
                        len+=2;
                    }
                    if(p->objectPicLib &&  p->objectPic)
                    {
                        *btemp |= 0x8;

                        *(unsigned short*)pBuf = p->objectPic;
                        pBuf += 2;
                        *(unsigned short*)pBuf = p->objectPicLib;
                        pBuf += 2;

                        len+=4;
                    }
                }
            }
            pMapTiles += MapWidth;
        }
    	break;
    case 3:
        for(i=0;i<iHeight;i++)
        {
        	p = pMapTiles;
        	for(j=0;j<iWidth;j++,p++)
            {
            	if(j >= MapWidth || i >= MapHeight)
                {
                	*pBuf++ = 0x1;
                    len++;
                }
                else
                {
                    btemp = pBuf++;
                    len++;
                    *btemp = 0;
                    if(p->bBlock)
                    {
                        *btemp |= 0x1;
                    }
                    if(p->smTile)
                    {
                        *btemp |= 0x2;
                        *(unsigned short*)pBuf = p->smTile;
                        pBuf+=2;
                        len+=2;
                    }
                    if(p->tile)
                    {
                        *btemp |= 0x4;
                        *(unsigned short*)pBuf = p->tile;
                        pBuf += 2;
                        len+=2;
                    }
                    if(p->objectPicLib &&  p->objectPic)
                    {
                        *btemp |= 0x8;

                        *(unsigned short*)pBuf = p->objectPic;
                        pBuf += 2;
                        *(unsigned short*)pBuf = p->objectPicLib;
                        pBuf += 2;

                        len+=4;
                    }
                    if(p->music)
                    {
                        *btemp |= 0x10;
                        *pBuf = p->music;
                        pBuf ++;
                        len+=1;
                    }
                }
            }
            pMapTiles += MapWidth;
        }
    	break;
    }
    return len;
}

};
#endif
#define MAP_NODE_TERRAIN	MAKEFOURCC('T','E','R','R')
#define MAP_NODE_SMTERRAIN	MAKEFOURCC('S','T','E','R')
#define MAP_NODE_TILEITEM	MAKEFOURCC('T','I','L','E')
#define MAP_NODE_OTHER		MAKEFOURCC('O','T','H','E')
#define MAP_NODE_ADDIN		MAKEFOURCC('A','D','D','I')
#define MAP_NODE_END		MAKEFOURCC(' ','E','N','D')

#define MAP_MAGIC (MAKEFOURCC('X','M','A','P'))

struct stMapNodeHeader{
	DWORD type;
	DWORD size;
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param szFileName : 描述
 * \return 返回值的描述
 */
bool stMap::LoadFromFile(const char* szFileName)
{
	FileStream str(szFileName,FileStream::Read);
	return LoadFromStream(&str);
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
int __cdecl compareInt(const void * p1, const void * p2) 
{
	DWORD dw1 = ((stMapAddinData*)p1)->nodeNum;
	DWORD dw2 = ((stMapAddinData*)p2)->nodeNum;
	if(dw1 < dw2) return -1;
	if(dw1 == dw2) return 0;
	return 1;
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
int  __cdecl compareSInt ( const void * p1, const void * p2) 
{
	DWORD dw1 = *((DWORD*)p1);
	DWORD dw2 = ((stMapAddinData*)p2)->nodeNum;
	if(dw1 < dw2) return -1;
	if(dw1 == dw2) return 0;
	return 1;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
stMapAddinData* stMap::GetAddinData( POINT pt )
{
	DWORD dwNodeNum = ( (pt.y << 16) | pt.x );
	stMapAddinData* p = (stMapAddinData*)bsearch(&dwNodeNum,&m_addins[0],m_addins.size(),sizeof(stMapAddinData),compareSInt);
	return p;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pStream : 描述
 * \return 返回值的描述
 */
bool stMap::LoadFromStream(Stream* pStream)
{
	stMapFileHeader fhdr;
	
	if(!pStream->read(&fhdr,sizeof(fhdr)))
		return false;
	
	if(fhdr.magic != MAP_MAGIC)
	{
		AssertFatal(0,"地图文件格式不正确！");
		return false;
	}
	
	if(fhdr.ver != MAP_VERSION && fhdr.ver != MAP_VERSION_OLD && fhdr.ver != MAP_VERSION_OLD2 && fhdr.ver != MAP_VERSION_OLD3 && fhdr.ver != MAP_VERSION_NEW1)
	{
		AssertFatal(0,"地图文件版本不正确！");
		return false;
	}

	Reset(fhdr.width,fhdr.height);

	for( ; ; )
	{
		stMapNodeHeader hdr;
		if(!pStream->read(&hdr,sizeof(hdr)))
			return false;
		switch(hdr.type)
		{
		case MAP_NODE_ADDIN:	// 场景附加的数据
			m_addins.resize( (hdr.size + sizeof(stMapAddinData)-1) / sizeof(stMapAddinData) ) ;
			pStream->read(&m_addins[0],hdr.size);
			qsort(&m_addins[0],m_addins.size(),sizeof(stMapAddinData),compareInt);
			break;
		case MAP_NODE_OTHER:
			Engine_WriteLogF("现在在MAP_NODE_OTHER中");
            if(fhdr.ver == MAP_VERSION_NEW1)
			{
				m2_others.resize( (hdr.size + sizeof(stMapTileOther10)-1) / sizeof(stMapTileOther10) ) ;
				for(size_t i=0; i< m2_others.size(); i++)
				{
					stMapTileOther10 stMap;
					pStream->read(&stMap,sizeof(stMap));
					m2_others[i].aniID = stMap.aniID;
					m2_others[i].effect = (DWORD)stMap.effect;
					m2_others[i].effectAlpha = stMap.effectAlpha;
					m2_others[i].nodeNum = stMap.nodeNum;
					m2_others[i].object2 = stMap.object2;
					m2_others[i].objectLib2 = stMap.objectLib2;
					m2_others[i].x = stMap.x;
					m2_others[i].y = stMap.y;
				}

			}
			else
			{
				m_others.resize( (hdr.size + sizeof(stMapTileOther)-1) / sizeof(stMapTileOther) ) ;
				pStream->read(&m_others[0],hdr.size);
			}
			break;
		case MAP_NODE_TERRAIN:                                  //处理大地表 
			Engine_WriteLogF("现在在MAP_NODE_TERRAIN中\n");
			if(fhdr.ver == MAP_VERSION_OLD)
			{
				int num = CalculateTerrainCount(m_width,m_height);
				if(hdr.size != num * sizeof(stMapTileTerrainOld))
				{
					return false;
				} 
				for( size_t i=0; i< num; ++i )
				{
					stMapTileTerrainOld oldst;
					pStream->read(&oldst,sizeof(oldst));
					m_pTerrain[i].tile = oldst.tile;
				}
			}
			else if(fhdr.ver == MAP_VERSION_NEW1)
			{
				if(hdr.size != CalculateTerrainCount(m_width,m_height) * sizeof(stMapTileTerrain))
				{
					return false;
				}

				pStream->read(m_pTerrain,hdr.size);
			}
			else
			{
				Engine_WriteLogF("大地表hdr.size为:%d\n",hdr.size);
				Engine_WriteLogF("CalculateTerrainCount(m_width,m_height)为:%d\n",(CalculateTerrainCount(m_width,m_height) * sizeof(stMapTileTerrain10)));
				if(hdr.size != CalculateTerrainCount(m_width,m_height) * sizeof(stMapTileTerrain10))
				{
					return false;
				}
				int count = CalculateTerrainCount(m_width,m_height);

				for( size_t i=0; i< count; ++i )
				{
					stMapTileTerrain10 xin_ditu;
					pStream->read(&xin_ditu,sizeof(xin_ditu));
					m_pTerrain[i].tile = (DWORD)xin_ditu.tile;
				}

			}
			break;
		case MAP_NODE_SMTERRAIN:                                //处理小地表
			Engine_WriteLogF("现在在MAP_NODE_SMTERRAIN中");
			if(fhdr.ver == MAP_VERSION)
			{
				if(hdr.size != CalculateSmTerrainCount(m_width,m_height) * sizeof(stMapTileSmTerrain))
					return false;
				pStream->read(m_pSmTerrain,hdr.size);
			}
			else if(fhdr.ver == MAP_VERSION_NEW1)
			{
				if(hdr.size != CalculateSmTerrainCount(m_width,m_height) * sizeof(stMapTileSmTerrain))
					return false;
				pStream->read(m_pSmTerrain,hdr.size);
			}
			else
			{
				SetFrameAllocator fa;
				std::vector<stMapTileSmTerrainOld,std::stack_allocator<stMapTileSmTerrainOld> > aTerrainOld;
				aTerrainOld.resize(CalculateSmTerrainCount(m_width,m_height));
				if(hdr.size != aTerrainOld.size() * sizeof(stMapTileSmTerrainOld))
					return false;
				pStream->read(&aTerrainOld[0],hdr.size);
				for( size_t i=0; i< aTerrainOld.size(); ++i )
				{
					if(NULL_MAPIMGID == aTerrainOld[i].smTile)
						m_pSmTerrain[i].smTile = NULL_SMTERRID;
					else
						m_pSmTerrain[i].smTile = aTerrainOld[i].smTile;
				}
			}
			break;
		case MAP_NODE_TILEITEM:
			Engine_WriteLogF("现在在MAP_NODE_TILEITEM中\n");
			if(fhdr.ver == MAP_VERSION_OLD)
			{
				int num = CalculateTileCount(m_width,m_height);
				if(hdr.size != num)
					return false;
				for(size_t i=0;i<num;++i)
				{
					stMapTileItemOld oldst;
					pStream->read(&oldst,sizeof(oldst));
					m_pTiles[i].flags = oldst.flags;
					m_pTiles[i].type = oldst.type;
					m_pTiles[i].object = oldst.object;
					m_pTiles[i].objectLib = (BYTE)oldst.objectLib;
					m_pTiles[i].byBlock = 0;
					m_pTiles[i].pOther = NULL;
					m_pTiles[i].zone = -1;
				}
			}
			else if (fhdr.ver == MAP_VERSION_OLD2)
			{
				size_t num =  CalculateTileCount(m_width,m_height);
				if(hdr.size != num * sizeof(stMapTileItemOld2))
					return false;
				for(size_t i=0;i<num;++i)
				{
					stMapTileItemOld2 oldst;
					pStream->read(&oldst,sizeof(oldst));
					memcpy(&m_pTiles[i],&oldst,sizeof(stMapTileItemOld2));
					m_pTiles[i].pOther = NULL;
					m_pTiles[i].byBlock = 0;
					m_pTiles[i].zone = -1;
				}
			}
			else if(fhdr.ver == MAP_VERSION_NEW1)
			{
				size_t count = CalculateTileCount(m_width,m_height);
				if(hdr.size != count * sizeof(stMapTileItem))
					return false;
				Engine_WriteLogF("开始渲染循环！\n");
				for(size_t i=0; i<count ; ++i)
				{
					pStream->read(&m_pTiles[i],sizeof(stMapTileItem));
					m_pTiles[i].byBlock = 0;
					m_pTiles[i].p2Other = NULL;
				}
			}
			else
			{
				size_t count = CalculateTileCount(m_width,m_height);
				if(hdr.size != count * sizeof(stMapTileItem))
					return false;
				for(size_t i=0; i<count ; ++i)
				{
					pStream->read(&m_pTiles[i],sizeof(stMapTileItem));
					m_pTiles[i].byBlock = 0;
					m_pTiles[i].pOther = NULL;
				}
			}
			break;
		case MAP_NODE_END:
			Engine_WriteLogF("现在在MAP_NODE_END中\n");
			goto _out;
			break;
		default:
			Engine_WriteLogF("现在在default中\n");
			pStream->setPosition(pStream->getPosition() + hdr.size);
			break;
		}
	}
_out:
	{
		for( size_t i=0; i< m_others.size(); ++i )
		{
			stMapTileItemEx* p = GetTile(m_others[i].x,m_others[i].y);
			if(p)
			{
				p->pOther = &m_others[i];
			}
		}
		
		//sky
		for( size_t j=0; j< m2_others.size(); ++j )
		{
			stMapTileItemEx* p = GetTile(m2_others[j].x,m2_others[j].y);
			if(p)
			{
				p->p2Other = &m2_others[j];
			}
		}
	}
	return true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param szFileName : 描述
 * \return 返回值的描述
 */
bool stMap::SaveToFile(const char* szFileName)
{
	FileStream str(szFileName,FileStream::Write);
	return SaveToStream(&str);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pStream : 描述
 * \return 返回值的描述
 */
bool stMap::SaveToStream(Stream* pStream)
{
	Assert(0);
	return false;
}
