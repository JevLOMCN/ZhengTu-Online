#pragma once

#include "../../engine/include/Animation.h"

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CCursor
{
public:

	enum enumCursorType{
		CursorType_Normal,	// ’˝≥£
		CursorType_Select,	// —°‘Ò
		CursorType_Wait,	// √§
		CursorType_Drag,	// Õœ∂Ø
	};

protected:
	stResourceLocation mImageResource;
	CAnimation m_image;
	enumCursorType m_eType;
	HCURSOR		m_hCursor;
	stPointI	m_ptHotPoint;
	stPointI	m_ptPosition;
	stPointI	m_ptPositionOnWnd;

	bool		m_bLButtonDown;
	bool		m_bRButtonDown;
	bool		m_bHardwareCursor;
public:
	CCursor(void);
	~CCursor(void);

	void SetImage(const stResourceLocation * rl,stPointI ptHotPoint = stPointI(0,0));

	POINT GetPosition(){ return m_ptPosition;}
	POINT GetPositionOnWnd(){ return m_ptPositionOnWnd;}
	virtual void Render(float fElapsedTime);
	virtual bool MsgProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual void Run(float fElapsedTime);

	virtual void OnSetHardwareCursor(const stResourceLocation* rl,stPointI ptHot)
	{
	}
	virtual HCURSOR GetCursorHandle(const stResourceLocation * rl)
	{
		return NULL;
	}
	virtual HCURSOR GetNormalCursorHandle(){
		return NULL;
	}

	bool	IsLButtonDown() { return m_bLButtonDown; }
	bool	IsRButtonDown() { return m_bRButtonDown; }
	bool	IsNormal() { return m_eType == CursorType_Normal; }
	bool	IsDrag() { return m_eType == CursorType_Drag;}
	void	UpdateCursorPosition();
	void	ResetCursor();
};	
