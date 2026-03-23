
#pragma once
#include "iobj.h"

class IBitmap;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class cBuild :public iObj
{
public:
	void SetEnableShake(bool b) {}
	bool IsEnableShake() { return true;}

	virtual int GetObjType(){ return eObjType_Build;}
	cBuild(void);
	virtual ~cBuild(void);

	ULONG m_ulCount;
	ULONG AddRef() {return ++m_ulCount;};
	ULONG Release()
	{
		int n = --m_ulCount;
		if (!m_ulCount) 
			delete this; 
		return n;
	};

	//data
	void* m_lData;
	void SetData(void* ldata) { m_lData = ldata;};
	void* GetData() {return m_lData;};

	void FinalRelease();

	POINT m_ptPos;
	POINT GetPos(){return m_ptPos;}
	void SetPos(POINT pt){m_ptPos = pt;}

	size_t GetDir(){return 0;}
	POINT GetSortPos(){return GetPos();};

	//在地图偏移为ptMapOffset的地方绘制
	bool Draw(POINT ptMapOffset) ;
	IBitmap* m_pTexture;
	IBitmap* GetTexture(){return m_pTexture;}
	//在屏幕某处直接绘制
	bool DrawAtScreen(POINT ptScreen) ;

	int Run(DWORD time_eslasped) {return true;};

	HRESULT InvalidateDeviceObjects(){FinalRelease(); return S_OK;}
	HRESULT RestoreDeviceObjects() ;

	UINT	m_nPicLib;
	UINT	m_nPic;
	DWORD	m_dwX;
	DWORD	m_dwY;
	void SetPic(DWORD dwObject,int xGrid,int yGrid);

	bool GetBuildRect(stRectI& rcBuild,IBitmapList* pRes);
	IBitmapList* FindResource();

	static void PreLoad(IBitmapList* p, int nPic);
};
