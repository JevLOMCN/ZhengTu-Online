#pragma once

/// 格点像素宽
#define	GRID_WIDTH				64
/// 格点像素高
#define GRID_HEIGHT				32

struct stGridPoint
{
	long x,y;
	stGridPoint(){}
	stGridPoint(const POINT& pt)
	{
		x = pt.x / GRID_WIDTH;
		y = pt.y / GRID_HEIGHT;
	}
	POINT UpLeft()
	{
		POINT pt = {x*GRID_WIDTH,y*GRID_HEIGHT};
		return pt;
	}
	operator POINT()
	{
		POINT pt = {x,y};
		pt.x *= GRID_WIDTH;
		pt.y *= GRID_HEIGHT;
		pt.x += GRID_WIDTH/2;
		pt.y += GRID_HEIGHT/2;
		return pt;
	}
protected:
	stGridPoint(long _x, long _y)
	{
		x = _x;
		y = _y;
	}
	friend stGridPoint operator+(stGridPoint p1, stGridPoint p2);
	friend stGridPoint operator-(stGridPoint p1, stGridPoint p2);
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param p1 : 描述
 * \param p2 : 描述
 * \return 返回值的描述
 */
inline stGridPoint operator+(stGridPoint p1, stGridPoint p2)
{
	stGridPoint p(p1.x + p2.x,p1.y + p2.y);
	return p;
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
inline stGridPoint operator-(stGridPoint p1, stGridPoint p2)
{
	stGridPoint p(p1.x - p2.x,p1.y - p2.y);
	return p;
}


class iObj;
class iGround;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class iMiniGround
{
public:
	enum PointSizeEnum{
		PointSize_Small,
		PointSize_Normal,
		PointSize_Large,
		PointSize_Num,
	};
public:
	//draw
	virtual void Draw() = 0;
	virtual void AddObject(PointSizeEnum size,const stDrawPoint& st) = 0;
	virtual void ClearObject() = 0;

	virtual BOOL MoveTo(POINT ptScreen) = 0;
	virtual BOOL SetSize(SIZE sz) = 0;
	virtual void ToggleHide() = 0;
protected:
	//call by ground..
	//左上角的坐标
	virtual void FinalRelease() = 0;
	virtual BOOL CreateMiniGround() = 0;
	virtual BOOL Load(LPCSTR szMap,int width,int height) = 0;
	virtual void OnGroundMoveTo(POINT ptPos) = 0;
	virtual HRESULT InvalidateDeviceObjects() = 0;	//before call reset alt+tab 
	virtual HRESULT RestoreDeviceObjects() = 0;		//after alt+tab
	friend class cGround;
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class iGround
{
public:
	virtual bool Create() = 0;
	virtual bool Load(LPCSTR szMap,LoadEmum e = Load_Normal) = 0;
	virtual void FinalRelease() = 0;

	virtual void LoadAllRes(LPCSTR szMap) = 0;
	//virtual void SaveAllRes(LPCSTR szFolder) = 0;
	virtual void UnloadAllRes() = 0;

//左上角的坐标
	virtual void MoveTo(POINT ptPos) = 0;
	virtual POINT GetPos() = 0;
//draw
	virtual void CalcRenderRect(void) = 0;
	virtual bool RenderTerrain(int order,const POINT&) = 0;
	virtual void DrawObject(std::vector<iObj*>& aObj,const POINT&) = 0;
//find path
	//return dir
	virtual bool IsStaticObstacle(stGridPoint pt) = 0;
	//站在这个点上会被墙壁等等挡住。
	virtual bool IsCovered(stGridPoint pt) = 0;
	virtual int NextPos(POINT ptFrom, POINT ptTo,POINT& ptNext,int& speed) = 0;
	virtual int NextGrid(stGridPoint ptFrom, stGridPoint ptTo,stGridPoint& ptNext,int& speed) = 0;
	virtual int GetMusicID(stGridPoint ptGrid) = 0;

	virtual HRESULT InvalidateDeviceObjects() = 0;	//before call reset alt+tab 
	virtual HRESULT RestoreDeviceObjects() = 0;		//after alt+tab

	//创建预读的线程
	//virtual BOOL CreateLoadThread() = 0;
	virtual HRESULT InvalidateResource() = 0;
	virtual HRESULT RestoreResource() = 0;
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class iGroundCallBack
{
public:
	//该点是否为动态阻挡
	virtual BOOL IsDynamicObstacle(stGridPoint pt) = 0;
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
iGround* Ground_GetGround();
iMiniGround* Ground_GetMiniGround();
void Ground_SetCallBack(iGroundCallBack* p);
iGroundCallBack* GetGroundCallBack();
/*
inline POINT GridToWorld(POINT pt)
{
	pt.x *= GRID_WIDTH;
	pt.y *= GRID_HEIGHT;
	return pt;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
inline POINT WorldToGrid(POINT pt)
{
	pt.x /= GRID_WIDTH;
	pt.y /= GRID_HEIGHT;
	return pt;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
inline POINT GridCenterToWorld(POINT pt)
{
	pt.x *= GRID_WIDTH;
	pt.y *= GRID_HEIGHT;
	pt.x += GRID_WIDTH/2;
	pt.y += GRID_HEIGHT/2;
	return pt;
}
*/

void SaveAllRes(std::vector<std::string>& aMap,LPCSTR szFolder);
