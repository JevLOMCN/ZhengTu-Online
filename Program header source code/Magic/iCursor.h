#pragma once

enum CursorEnum
{
	Cursor_Normal,
	Cursor_LeftDown,
	Cursor_LeftUp,
	Cursor_LeftHold,
	Cursor_RightDown,

	Cursor_SelectNull,
	Cursor_SelectItem,
	Cursor_SelectNpc,
	Cursor_Num,
};

typedef CursorEnum CursorSelectEnum;

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class iCursor
{
public:
	virtual ULONG AddRef() = 0;
	virtual ULONG Release() = 0;
	virtual void FinalRelease() = 0;

	virtual void ChangeSelectCursor(CursorSelectEnum e) = 0;
	virtual void ChangeCursor(CursorEnum e) = 0;

	virtual int Run(long time_eslasped) = 0;	

	virtual void Draw(POINT ptScreen) = 0;
	virtual void Show(bool bShow = true) = 0;
	virtual HRESULT InvalidateDeviceObjects() = 0;	//before call reset alt+tab 
	virtual HRESULT RestoreDeviceObjects() = 0;		//after alt+tab
};

iCursor* Cursor_GetCursor();

