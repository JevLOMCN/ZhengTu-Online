#pragma once
#include "..\magic\sceneobject.h"
#include "../Engine/include/Animation.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CItem :
	public CSceneObject
{
	enum enumState
	{
		State_Static,	//静止在地
		State_Up,		//向上
		State_Down,		//向下
	};

	enum enumStateOfItem
	{
		eState_Comm,		//正常状态
		eState_Start,		//放到地上的状态
		eState_End,			//从地上删除
	};

	enumStateOfItem	m_stateOfItem;
	DWORD	m_timeOfStartOnGround;
	enumState m_State;
	int		  m_y;
	IBitmaps*	m_image;
	IBitmaps*	m_star;
  
	IBitmaps*	m_imgStart;
	IBitmaps*	m_imgEnd;

	DWORD		m_nextShowTime;
	DWORD		m_endShowTime;
	bool		m_bShowStar;
	bool		m_bMouseInName;

	float		m_fSpeed;
	float		m_fAcceleration;

	ObjectBase_t* m_pObjectBase;
	DWORD		m_dwLevel;
	DWORD		m_dwCount;
	BYTE		m_upgrade;
	BYTE		m_kind;

	int			m_textHeight;
	void ResetImage();

	DWORD			m_dwOwner;
	DWORD			m_GuoOwner;//国家归属
public:
//#ifdef _AUTOPLAY
	DWORD	m_LastTryPickTime;
	DWORD	m_LastSendPickCmdTime;
//#endif
	DWORD	m_CreateTime;
	CItem(void);
	~CItem(void);
	void SetAttrib(t_MapObjectData* pData,bool bDrop);
	void Run(long time_elapsed);
	void Draw(POINT ptMapPixelOffset,DWORD dwColor);
	void DrawText(POINT ptMapPixelOffset);
	void DrawName(POINT ptMapPixelOffset);
	void DrawBack(POINT ptMapPixelOffset);
	int GetWidth(){ return ( m_image && m_image->GetFrameCount() ?  m_image->GetBitmap(0)->GetWidth() : 0 );}
	int GetHeight(){ return ( m_image && m_image->GetFrameCount() ?  m_image->GetBitmap(0)->GetHeight() : 0 );}
	bool InScreen();
	bool IsAtPixelPos(POINT pt, bool bJustInRect = false);
	POINT GetSortPos();
	bool OnSetCursor();
	void OnRemove();
	BYTE GetKind() { return m_kind; }
	DWORD GetItemObjectType();
	DWORD GetBaseID();
	DWORD GetOwner() {return m_dwOwner;};
	DWORD GetGuoOwner() {return m_GuoOwner;};//国家归属
	void  LoseOwner();
};
