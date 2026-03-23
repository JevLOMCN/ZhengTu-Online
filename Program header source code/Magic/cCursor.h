#pragma once

#include "iCursor.h"

struct stCursorProperty
{
	CursorEnum eType;
	CursorEnum eNextType;
	int nTime;
	bool bLoop;
};

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class cCursor :
	public iCursor
{
public:
	cCursor(void);
	virtual ~cCursor(void);
	HWND	m_hWnd;
	ULONG m_ulCount;
	ULONG AddRef() {return ++m_ulCount;};
	ULONG Release()
	{
		int n = --m_ulCount;
		if (!m_ulCount) 
			delete this; 
		return n;
	};

	void FinalRelease();

    int Run(long time_eslasped) ;	//time µ±«∞ ±º‰
	void Draw(POINT ptScreen);
	
	CursorSelectEnum m_eSelectCursor;

	CursorEnum m_eCursor;
	long m_nTime;
	void ChangeSelectCursor(CursorSelectEnum e);
	void ChangeCursor(CursorEnum e);
	void SetCursor(CursorEnum e);

	bool m_bShow;
	void Show(bool bShow ){m_bShow = bShow;};

	HRESULT InvalidateDeviceObjects(){return S_OK;}
	HRESULT RestoreDeviceObjects() {return S_OK;}
};
