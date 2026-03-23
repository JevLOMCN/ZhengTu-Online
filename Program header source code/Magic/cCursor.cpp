#include "../engine/include/engine.h"
#include ".\ccursor.h"


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
cCursor::cCursor(void)
{
	m_eCursor = Cursor_Normal;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
cCursor::~cCursor(void) 
{
}

static stCursorProperty s_aProperty[Cursor_Num] = 
{
	{Cursor_Normal,Cursor_Normal,0,1},
	{Cursor_LeftDown,Cursor_Normal,0,1},
	{Cursor_LeftUp,Cursor_Normal,0,0},
	{Cursor_LeftHold,Cursor_Normal,0,1},
	{Cursor_RightDown,Cursor_Normal,200,0},

	{Cursor_SelectNull,Cursor_Normal,200,0},
	{Cursor_SelectItem,Cursor_Normal,200,0},
	{Cursor_SelectNpc,Cursor_Normal,200,0},
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cCursor::FinalRelease()
{

}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param time_eslasped : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int cCursor::Run(long time_eslasped)
{
	if (s_aProperty[m_eCursor].bLoop)
		return 0;
	m_nTime += time_eslasped;
	if (m_nTime >= s_aProperty[m_eCursor].nTime)
		ChangeCursor(s_aProperty[m_eCursor].eNextType);
	return 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptScreen : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cCursor::Draw(POINT ptScreen)
{

}

static char* s_aWindowCursor[] = 
{
	IDC_ARROW,
	IDC_CROSS,
	IDC_CROSS,
	IDC_WAIT,
	IDC_IBEAM,

	IDC_UPARROW,
	IDC_UPARROW,
	IDC_UPARROW,
	IDC_SIZEALL,
	IDC_SIZENWSE,
	IDC_SIZENESW,
	IDC_SIZEWE,
	IDC_SIZENS,
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param e : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cCursor::ChangeSelectCursor(CursorSelectEnum e)
{
	if (m_eSelectCursor == e)
		return;
	m_eSelectCursor = e;
	if (e == Cursor_SelectNull)
		SetCursor(m_eCursor);
	else
		SetCursor(e);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param e : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cCursor::ChangeCursor(CursorEnum e)
{
	if (m_eCursor == e)
		return;
	m_eCursor = e;
	m_nTime = 0;
	SetCursor(e);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param e : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cCursor::SetCursor(CursorEnum e)
{
	HCURSOR hCursor;
	//hCursor = AfxGetApp()->LoadStandardCursor(s_aWindowCursor[e]);
	hCursor = LoadCursor(NULL,s_aWindowCursor[e]);
	//AfxGetMainWnd()->SetCursor(hCursor);

	SetClassLong(	m_hWnd,    // window handle 
					GCL_HCURSOR,      // change cursor 
					(LONG) hCursor);   // new cursor 
	::SetCursor(hCursor);
#ifdef _DEBUG
//	afxDump << "change to cursor: " << e << " [" << e <<  "]\n";
#endif
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
iCursor* Cursor_GetCursor()
{
	static cCursor s_cursor;
	return &s_cursor;
}
