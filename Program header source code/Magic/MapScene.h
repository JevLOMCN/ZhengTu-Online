#pragma once

#include "map.h"
//#include "iMagic.h"
#include <vector>
class CSceneObject;
enum RangeEnum;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dir : 描述
 * \return 返回值的描述
 */
extern const POINT c_ptDir[8];
inline POINT  Scene_GetDirOffset(int dir)
{
	if(dir >= 0 && dir < 8) return c_ptDir[dir];
	return stPointI(0,0);
}
inline int Scene_GetDis(POINT pt1,POINT pt2)
{
	return getMax(abs(pt2.x-pt1.x) , abs(pt2.y-pt1.y));
}
inline int Scene_GetDisPixel(POINT pt1, POINT pt2)
{
	return getMax( abs(pt2.x-pt1.x)/GRID_WIDTH, abs(pt2.y-pt1.y)/GRID_HEIGHT );
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dx : 描述
 * \param dy : 描述
 * \return 返回值的描述
 */
inline int Scene_GetDir8(int dx,int dy)
{
	int dir;
	const float tg225 = (float)tan(22.5f*PI/180.f);
	const float tg675 = (float)tan(67.5f*PI/180.f);
	dy = -dy;

	if (dx == 0 && dy == 0)
		return -1;
	if (dx == 0)
	{
		if (dy > 0)
			return 0;
		else
			return 4;
	}

	float tga = (float)dy / (float)dx;
	static float af[4] = {-tg675,-tg225,tg225,tg675};
	if (tga < af[0])
	{
		dir = 4;
	}
	else if (tga >= af[0] && tga < af[1])
	{
		dir = 3;
	}
	else if (tga >= af[1] && tga < af[2])
	{
		dir = 2;
	}
	else if (tga >= af[2] && tga < af[3])
	{
		dir = 1;
	}
	else if (tga >= af[3])
	{
		dir = 0;
	}
	if (dx < 0)
		dir += 4;
	if (dir == 8)
		dir = 0;
	assert(dir >= 0 && dir < 8);
	return dir;
}

#include "../Engine/include/stack_alloc.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class iCharacter;
typedef std::vector<iCharacter*, std::stack_allocator<iCharacter*> > tStackVectorICharacter;

typedef std::vector<int> t_aGroundMusicLocation;
struct stGroundMusic{
	t_aGroundMusicLocation walk;
	t_aGroundMusicLocation run;
	t_aGroundMusicLocation ride_walk;
	t_aGroundMusicLocation ride_run;
};

struct stGroundTypeInfo{
	stGroundMusic music;
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CMapScene : public IEngineCallBack
{
protected:
	enum{
		ConverPixelSize = 32,
	};
	bool		m_bInitConver;
	bool        m_bInterior;
	std::vector<BYTE> m_convers;
	std::vector<BYTE> m_terrGrid;

	bool		m_bDrawMagicEffect;

	int			m_converWidth;
	int			m_converHeight;
	stMap		m_map;
	int			m_iScreenWidth;		//	以屏为单位的宽
	int			m_iScreenHeight;	//	以屏为单位的高

	stPointI	m_ptPixelOffset;		//	屏幕左上角的像素偏移
	stPointI	m_ptAddinOffset;		//	附加的偏移
	int			m_iPixelWidth;			//	屏幕的像素宽度
	int			m_iPixelHeight;			//	屏幕的像素高度
	stRectI		m_rcDrawRect;			//  以格子为单位的显示范围，包括load的部分
	stRectI		m_rcDrawPixelRect;		// 
	typedef std::vector<CSceneObject*> tListDrawObjects;	
	IBitmapList*				m_pTerrainPack;		// 大地表图包
	IBitmapList*				m_pTerrainAlphaPack;// 大地表Alpha图包
	IBitmapList*				m_pSmTerrainPack;	// 小地表图包
	std::vector<IBitmapList*>	m_aObjectPack;		// 物件图包
	tListDrawObjects			m_aDrawObjects;		// 要渲染的对象
	tListDrawObjects			m_aDrawLowest;		// 要渲染的后景对象
	tListDrawObjects			m_aDrawFrontest;	// 要渲染的前景对象
	DWORD						m_dwBeginTime;		// 自地图加载的时间
	tListDrawObjects			m_aDrawAboveAll;	// 要渲染的前景对象

	stRectI		m_rcFindPathRect;

	char	m_szFileName[MAX_PATH];
	void PlayUISound(DWORD dwSoundID){}
	bool GetObjectImageLocation(int id,stResourceLocation * rl){ return false;};
	void SetObjectTooltips(int id,CToolTips& toolTips){};
	virtual std::string GetTaskVariable(DWORD taskID,const char* taskVarName){ return "";}
	virtual int GetMainCharacterDir(){ return -1;}
public:
	
	std::map<int,stGroundTypeInfo> m_aGroundTypeInfo;

	const RECT& GetDrawPixelRect() { return m_rcDrawPixelRect;}

	POINT GetCenterPixelPos(){ POINT pt = GetPixelOffset(); pt.x += m_iPixelWidth/2; pt.y += m_iPixelHeight/2; return pt;}
	
	virtual POINT GetCenterGridPos(){ return stPointI(0,0);}
	void AddDrawObjects(CSceneObject* p) { m_aDrawObjects.push_back(p);}
	void AddLowestObjects(CSceneObject* p) { m_aDrawLowest.push_back(p);}
	void AddFrontestObjects(CSceneObject* p) { m_aDrawFrontest.push_back(p);}
	void AddAboveAllObjects(CSceneObject* p) { m_aDrawAboveAll.push_back(p);}

	void DrawLowestObjects(DWORD dwColor);
	void DrawMapLowestObjects(DWORD dwColor);
	void DrawFrontestObjects(DWORD dwColor);
	void DrawAboveAllObjects(DWORD dwColor);

	void OnOffMagicEffect(bool bDraw) { m_bDrawMagicEffect = bDraw; }
	bool IsNeedDrawMagicEffect() { return m_bDrawMagicEffect; }

	stGroundTypeInfo * GetMapTileInfo( int type )
	{
		std::map<int,stGroundTypeInfo>::iterator it = m_aGroundTypeInfo.find(type);
		if(it != m_aGroundTypeInfo.end())
			return &it->second;
		return NULL;
	}
	
	stGroundMusic* GetMapTileMusic( int type)
	{
		stGroundTypeInfo* info = GetMapTileInfo(type);
		if(info)
		{
			return &info->music;
		}
		return NULL;
	}

	void LoadMapTileInfo();

	CSceneObject*	m_pMouseOverTextObject;	//鼠标在哪个对象的头顶文字区域
	CSceneObject*	m_pMouseOverObject;
	bool			m_bDoorBlock;			// 门是否是阻挡
	CMapScene(void);
	~CMapScene(void);
	void InitGraphicPack();							// 初始化图包
	bool LoadMap(const char* szMapName);
	void InitCover();
	void AddCover(const RECT * prcPixel);
	void RemoveCover(const RECT * prcPixel);
	//bool IsCover(int x,int y) { return 0 != MapTileItem_GetCoverNum(m_map.GetTile(x,y)) ;}
	//bool IsCover(POINT pt) { return IsCover(pt.x,pt.y);}
	void OnDisplayModeChange();
	POINT GetPixelOffset()		// 获得地图像素偏移
	{
		POINT pt = {m_ptPixelOffset.x + m_ptAddinOffset.x,m_ptPixelOffset.y + m_ptAddinOffset.y};
		return pt;
	}

	POINT GetAddinOffsetPoint() { return m_ptAddinOffset;}
	void SetAddinOffsetPoint(POINT pt){ m_ptAddinOffset = pt;}

	virtual void MessageBox(const char* szText){};

	virtual void ReleaseScene();
	virtual void Run(long time_eslasped);
	virtual void Draw(DWORD dwColor);

	void DrawTerrain(DWORD dwColor);
	void DrawTerrainEffect(DWORD dwColor);
	void DrawSmTerrain(DWORD dwColor);
	void DrawObjects(DWORD dwColor);
	void DrawObstacle(DWORD dwColor,const stRectI *nrcDraw = NULL);
	void DrawCoverObject(DWORD dwColor);
	void SetCenterPos(int x,int y);
	void SetCenterPos(POINT pt){ SetCenterPos(pt.x,pt.y);}
	
	virtual void CalculateDrawRect();
	int GetMusicID(int x,int y);
	int GetMusicID(POINT pt){ return GetMusicID(pt.x,pt.y);}
	
	/*bool IsMagicObstacle(int x,int y);
	bool IsStaticObstacle(int x,int y);
	bool IsDynamicObstacle(int x,int y);*/
	bool IsMagicObstacle(POINT ptGrid);
	bool IsStaticObstacle(POINT ptGrid);
	bool IsDynamicObstacle(POINT ptGrid);

	bool IsMoveObstacle(POINT ptGrid)
	{
		if((size_t)ptGrid.x < (size_t)m_map.GetWidth() && (size_t)ptGrid.y < (size_t)m_map.GetHeight())
		{
			stMapTileItemEx * st = m_map.GetTile(ptGrid.x,ptGrid.y);
			if(MapTileItem_GetDBlockNum(st))
				return true;
			if( st->flags & TILE_BLOCK )
				return true;
			if( m_bDoorBlock && (st->flags & TILE_DOOR) )
				return true;
			return false;
		}else{
			return true;
		}
	}
	/*
	void AddDynamicObstacle(int x,int y);
	void RemoveDynamicObstacle(int x,int y);*/
	void AddDynamicObstacle(POINT ptGrid);
	void RemoveDynamicObstacle(POINT ptGrid);

	int GetScreenPixelWidth(){ return m_iPixelWidth;}
	int GetScreenPixelHeight(){return m_iPixelHeight;}

	void SetScreenSize(int cx,int cy){ m_iPixelWidth = cx;m_iPixelHeight = cy;}
	int GetScreenWidth(){ return m_iScreenWidth;}
	int GetScreenHeight(){ return m_iScreenHeight;}
	
	int GridPosToScreenID( POINT ptGrid )
	{
		int iScreenX,iScreenY;
		iScreenX = ptGrid.x / SCREEN_GRID_WIDTH;
		iScreenY = ptGrid.y / SCREEN_GRID_HEIGHT;
		return iScreenY * m_iScreenWidth + iScreenX;
	}

	POINT GridPosToScreenOffset( POINT ptGrid )
	{
		POINT pt;
		pt.x = ptGrid.x % SCREEN_GRID_WIDTH;
		pt.y = ptGrid.y % SCREEN_GRID_HEIGHT;
		return pt;
	}

	POINT ScreenIDToGridPos(int iScreenID,int iOffsetX,int iOffsetY)
	{
		int iScreenX,iScreenY;
		iScreenX = iScreenID % m_iScreenWidth;
		iScreenY = iScreenID / m_iScreenWidth;
		return stPointI(iScreenX * SCREEN_GRID_WIDTH + iOffsetX,iScreenY * SCREEN_GRID_HEIGHT + iOffsetY);
	}

	POINT GridCenterToPixelPos(POINT ptGrid)
	{
		return stPointI(ptGrid.x * GRID_WIDTH + GRID_WIDTH/2,ptGrid.y * GRID_HEIGHT + GRID_HEIGHT/2);
	}

	POINT PixelPosToGridPos(POINT ptPixel)
	{
		return stPointI(ptPixel.x/GRID_WIDTH,ptPixel.y/GRID_HEIGHT);
	}

	POINT ScreenIDToPixelPos(int iScreenID,int iOffsetX,int iOffsetY)
	{
		return GridCenterToPixelPos(ScreenIDToGridPos(iScreenID,iOffsetX,iOffsetY));
	}

	POINT ScreenPixelPosToPixelPos(POINT pt)
	{
		pt += m_ptPixelOffset;
		return pt;
	}

	POINT ScreenPixelPosToGridPos(POINT pt)
	{
		return PixelPosToGridPos(ScreenPixelPosToPixelPos(pt));
	}

	RECT GetScreenPixelRect()
	{
		return stRectI(m_ptPixelOffset.x ,m_ptPixelOffset.y,m_ptPixelOffset.x + m_iPixelWidth,m_ptPixelOffset.y + m_iPixelHeight);
	}

	int m_nLightRadius;
	DWORD m_dwLightColor;
	bool m_bLightMap;

	DWORD GetLightColor()
	{
		return m_dwLightColor;
	}
	int GetLightRadius()
	{ 
		return m_nLightRadius;
	}
	void AddLight();


	// 获得从 ptStart 到 ptEnd 的最短路径的点列表
	void GetALine(POINT ptStart,POINT ptEnd,std::vector< POINT ,std::stack_allocator<POINT> > &aPoints);
	// 获得从 ptStart 到 ptEnd 之间有dwTileFlags属性的格子数量
	int GetTileFlagsNum(POINT ptStart,POINT ptEnd,POINT & ptObs,DWORD dwTileFlags);

	int GetMapTileType(POINT pt);
	int GetZoneID(POINT pt);
	int GetWidth(){ return m_map.GetWidth();}
	int GetHeight(){ return m_map.GetHeight();}

	virtual iCharacter* GetICharacterByPos(POINT ptGrid) {return NULL;};
	void GetCharacter(RangeEnum eRange, tStackVectorICharacter & list, POINT ptStart, size_t dir, iCharacter* pExcept);
	void GetCharacter(tStackVectorICharacter& list, POINT ptStart, size_t dir, int mul, iCharacter* pExcept,bool bGetNpc);
	virtual void GetFullScreenCharacter(tStackVectorICharacter & list,iCharacter* pExcept, bool bGetNpc){};
	void LoadNearbyImage(POINT ptPrev,POINT ptCenter);

	static int __cdecl compareNum(const void *, const void *);
	typedef std::vector<DWORD> tImageIDSet;
	struct stLoaded{
		std::vector<DWORD> loadedTerr;
		std::vector<DWORD> loadedTerrAlpha;
		std::vector<DWORD> loadedSmTerr;
		std::vector<DWORD> loadedObjects[16];
		void clear()
		{
			loadedTerr.resize(0);
			loadedTerrAlpha.resize(0);
			loadedSmTerr.resize(0);
			for(size_t i=0;i<16;++i)
				loadedObjects[i].resize(0);
		}
		void sort()
		{
			qsort(&loadedTerr[0],loadedTerr.size(),sizeof(DWORD),compareNum);
			qsort(&loadedTerrAlpha[0],loadedTerrAlpha.size(),sizeof(DWORD),compareNum);
			qsort(&loadedSmTerr[0],loadedSmTerr.size(),sizeof(DWORD),compareNum);
			for(size_t i=0;i<16;++i)
				qsort(&loadedObjects[i][0],loadedObjects[i].size(),sizeof(DWORD),compareNum);
		}
	};

	stLoaded * m_loaded;
	void LoadAllImage();
	void GetImageIDSet(stLoaded & loaded);

	void ReleaseAllImage();

	void RefreshSurf(POINT ptCenter);
	void InitNearbyConver( POINT ptPrev,POINT ptCenter );

	std::vector<stPointI> m_curConverZone;
	std::vector<stPointI> m_curLoadResZone;

	template < class tVecPoint >
	void GetZonePointList( tVecPoint & vecPoint , stRectI rcZone);
	template < class tVecPoint >
	void UpdateCurZone(std::vector<stPointI> & curZoneList, POINT ptCurZone,tVecPoint & addZone , tVecPoint & removeZone,SIZE szZone);
	template < class tVecPoint >
	void GetNearZone(POINT ptCurZone,tVecPoint & nearZone,SIZE szZone);

	RECT GetZoneRect(POINT ptZone,SIZE szZone);
	void InitTerrTile(stMapTileItemEx* pTile);

	virtual DWORD GetGameHourTime() {return 0;};

	void SetInteriorMap(bool bInterior)
	{
		m_bInterior = bInterior;
	}

	void AddConverAtPixelRect(const RECT & rcConverPixel);
	bool IsConverAtPixel(POINT pt);
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pBitmaps : 描述
 * \param group : 描述
 * \param frame : 描述
 * \param x : 描述
 * \param y : 描述
 * \param dwTime : 描述
 * \param -1 : 描述
 * \param Blend_Null : 描述
 * \return 返回值的描述
 */
inline bool DrawMapTileAni(IBitmapList* pBitmaps,int group,int frame,int x,int y,DWORD dwTime,DWORD dwColor = -1,BlendType blend = Blend_Null)
{
	IBitmaps* pAni = pBitmaps->GetBitmaps(group,frame);
	if(!pAni) 
		return false;
	return pAni->RenderAni(x,y,dwTime,dwColor,blend);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pBitmaps : 描述
 * \param group : 描述
 * \param frame : 描述
 * \param x : 描述
 * \param y : 描述
 * \param dwTime : 描述
 * \param -1 : 描述
 * \param Blend_Null : 描述
 * \return 返回值的描述
 */
inline void DrawMapObjectAni(IBitmapList* pBitmaps,int group,int frame,int x,int y,DWORD dwTime,DWORD dwColor = -1,BlendType blend = Blend_Null)
{
	IBitmaps* pAni = pBitmaps->GetBitmaps(group,frame);
	if(!pAni) return;
	//pAni->TryLoadTexture();
	pAni->RenderAni2(x,y,dwTime,dwColor,blend);
}

extern CMapScene* GetMapScene();