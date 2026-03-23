#pragma once

#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include "public.h"
#include "../property/iProperty.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class iObj
{
public:
	enum{
		eObjType_Obj,
		eObjType_Character,
		eObjType_Magic,
		eObjType_Light,
		eObjType_Build,
	};
	virtual void SetEnableShake(bool b) = 0;
	virtual bool IsEnableShake() = 0;

	virtual int GetObjType()=0;
	virtual ULONG AddRef() = 0;
	virtual ULONG Release() = 0;
	virtual void FinalRelease() = 0;

	virtual void SetPos(POINT pt) = 0;
	virtual POINT GetPos() = 0;
	virtual size_t GetDir() = 0;
	virtual POINT GetSortPos() = 0;

	//在地图偏移为ptMapOffset的地方绘制
	virtual bool Draw(POINT ptMapOffset) = 0;
	//在屏幕某处直接绘制
	virtual bool DrawAtScreen(POINT ptScreen) = 0;

	virtual int Run(long time_eslasped) = 0;	

	//data
	virtual void SetData(void* ldata) = 0;
	virtual void* GetData() = 0;

	virtual HRESULT InvalidateDeviceObjects() = 0;	//before call reset alt+tab 
	virtual HRESULT RestoreDeviceObjects() = 0;		//after alt+tab
};

//terrain
//地表
//grass
//地表上的物体，用来点缀
//tree
//静态物体，不会运动，但是可能有动画，并且按照点简单排序
//magic
//魔法
//character
//人物和npc
//wall
//静态物体，不会运动，按照线段复杂排序
//cloud
//高空物体
//snow
//雨雪，风沙
//light
//光源
