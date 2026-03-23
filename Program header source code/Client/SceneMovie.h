#pragma once

class CSceneMovie
{
public:
	friend class CGameScene;
protected:
	IBitmaps*	m_pImage;
	DWORD		m_dwStartTime;
	DWORD		m_dwLastTime;
	POINT		m_ptLocation;
	DWORD		m_dwColor;
	bool		b_Stop;
public:
	CSceneMovie();
	bool Create( const char* pszFile, int nGroup, int nFrame, DWORD dwColor );
	void SetTimeLine( DWORD dwStart, DWORD dwLast );
	void SetLocation( LONG lX, LONG lY );
	bool Draw( long lLastTime );
	~CSceneMovie();
};