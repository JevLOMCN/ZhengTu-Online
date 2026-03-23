#pragma once
#include "objectbase.h"
#include "map.h"
#include <stdint.h>

// -------------------- 场景物体类型定义 ------------------//
enum enumSceneObjectType
{
	OBJECT_UNKNOW			=			(1<<0),
	OBJECT_NPC				=			(1<<1),		//	NPC
	OBJECT_CHARACTER		=			(1<<2),		//	人物
	OBJECT_MAINCHARACTER	=			(1<<3),		//	主人物
	OBJECT_BUILDING			=			(1<<4),		//	建筑
	OBJECT_MAGIC			=			(1<<5),		//	魔法效果
	OBJECT_RESOURCE			=			(1<<6),		//	资源
	OBJECT_ITEM				=			(1<<7),		//	道具
	OBJECT_IMAGE			=			(1<<8)
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CSceneObject :
	public CObjectBase
{
protected:
	stPointI	m_ptGrid;			// 网格位置
	stPointI	m_ptPixelOffset;	// 在地图上的像素偏移
	DWORD		m_dwSceneObjectType;

	DWORD		m_dwHP;
	DWORD		m_dwMaxHP;

	DWORD		m_dwMP;
	DWORD		m_dwMaxMP;
public:
	DWORD GetHP(){ return m_dwHP;}
	DWORD GetMaxHP(){ return m_dwMaxHP;}
	void  SetHP(DWORD dwHP) { m_dwHP = dwHP;}
	void  SetMaxHP(DWORD dwHP) { (m_dwMaxHP == 0) ? m_dwMaxHP = 1 : m_dwMaxHP = dwHP; }

	virtual size_t GetMP() { return m_dwMP; }
	virtual size_t GetMaxMP() { return m_dwMaxMP; }
	void SetMP(DWORD dwMP) { m_dwMP = dwMP; }
	void SetMaxMP(DWORD dwMP) { (m_dwMaxMP == 0) ? m_dwMaxMP = 1 : m_dwMaxMP = dwMP; }
	
	virtual void Die(){}
public:
	CSceneObject(void);
	~CSceneObject(void);
	virtual void Draw(POINT ptMapPixelOffset,DWORD dwColor){};
	POINT GetPos(){ return m_ptPixelOffset;}
	POINT GetGridPos(){ return m_ptGrid;}
	virtual POINT GetSortPos(){ return GetPos();}
	POINT GetCurGridPos()
	{ 
		POINT pt = {m_ptPixelOffset.x / GRID_WIDTH , m_ptPixelOffset.y / GRID_HEIGHT};
		return pt;
	}
	virtual void AddLight(POINT ptScreen){};
	
	//Add By David For 2006-3-9
	bool IsGridCenter( );
	int	 GetGridOffsetX( );			
	int	 GetGridOffsetY( );
	int	 GetScreenGrid( );
	
	bool IsSceneObjectType(DWORD dwSceneObjectType)
	{
		return (m_dwSceneObjectType & dwSceneObjectType) != NULL;
	}

	virtual bool UpdateGridPosition(POINT ptGird);

	virtual bool InScreen(){ return false;}

	virtual void DrawText(POINT ptMapPixelOffset) {} 
	virtual void DrawBack(POINT ptMapPixelOffset) {}

	bool UpdatePosition(DWORD dwScreenID,DWORD dwOffsetX,DWORD dwOffsetY);

	virtual bool IsAtPixelPos(POINT pt, bool bJustInRect = false){ return false; }
	bool IsAtScreenPixelPos(POINT pt, bool bJustInRect = false);
	bool IsAtGridPos(POINT pt){ return m_ptGrid == pt;}
	//virtual bool IsCanSelected(){ return true; }
	virtual bool OnSetCursor(){ return false; }
};
