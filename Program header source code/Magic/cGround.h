
#pragma once
#include "iGround.h"
#include <vector>
#include <map>
#include <string>

struct MapTile;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class IBitmap;


struct PicLibInfo
{
	char szFileName[64];// 文件名
};

const int MAX_OBJ = 64;

struct stLoad;
class IBitmapList;


struct MapTileEx;

const int MAX_GROUNDOBJ = 4;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class cGround : public iGround
{
public:

	cGround(void);
	virtual ~cGround(void);
	void FinalRelease();

	std::vector<IBitmapList*> m_aTileFile;
	std::vector<IBitmapList*> m_aSmTileFile;

	void LoadGraphicFileInfo();
	void ClearGraphicFileInfo();
	WORD GetTileNum(UINT nGroup,UINT nFrame);
	WORD GetSmTileNum(UINT nGroup,UINT nFrame);
	void RefTileInfo();

	bool Create();
	
	static LPCSTR GetObjName(int id);
	bool LoadPicLibInfo(const char* szFileName);
	bool Load(LPCSTR szMap,LoadEmum e);

//base
	MapTileEx* m_pMapTile;		//	地图的数据
	void ReleaseTile();

	UINT	  m_nMapTileWidth;
	UINT	  m_nMapTileHeight;
	stRectI	  m_rRenderRect;	// 以格子为单位的显示范围，包括load的部分
	stRectI	  m_rReal;			// 实际绘制的矩形

	int		m_iScreenWidth;		//	以屏为单位的宽
	int		m_iScreenHeight;	//	以屏为单位的高

	int		m_iOffsetPixelX;
	int		m_iOffsetPixelY;
	void MoveTo(POINT ptPos);
	POINT GetPos() {POINT pt={m_iOffsetPixelX,m_iOffsetPixelY};return pt;};

//render
	RECT rcBig;
	RECT rcSmall;
	RECT rcObject;
	bool RenderTerrain(int order,const POINT &ptOffset);
	void DrawObject(std::vector<iObj*>& aObj,const POINT&);
	void CalcRenderRect(void);

//obstacle
	int NextPos(POINT ptFrom, POINT ptTo,POINT& ptNext,int& speed);
	int NextGrid(stGridPoint ptFrom, stGridPoint ptTo,stGridPoint& ptNext,int& speed);
	int NextGrid(stGridPoint ptFrom, stGridPoint ptTo,int width, int height,stGridPoint& ptNext,int& speed);
	bool IsStaticObstacle(int x, int y);
	//out...
	bool IsStaticObstacle(stGridPoint pt);
	//站在这个点上会被墙壁等等挡住。
	bool IsCovered(stGridPoint pt);
	void AddCover(const stRectI& rc);
	void ComputeCover();
	void DrawCoverCharacter(std::vector<iObj*>& aObject,const POINT&);

	HRESULT InvalidateDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateResource();
	HRESULT RestoreResource();

	std::map<std::string,bool> m_mapLoad;
	void LoadAllRes(LPCSTR filename);
	void SaveAllRes(LPCSTR szFolder);
	void UnloadAllRes();
	void LoadResMap(stLoad& load);

	int GetMusicID(stGridPoint ptGrid);
	
	IBitmapList* FindResource(int n);
	IBitmapList* m_pResLarge;
	IBitmapList* m_pResSmall;
	IBitmapList* m_aResObj[MAX_OBJ];
	IBitmapList* GetResLarge(){return m_pResLarge;};
	IBitmapList* GetResSmall(){return m_pResSmall;};
};
