#pragma once
#include "..\magic\sceneobject.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CSceneImage :
	public CSceneObject
{
	IBitmaps*	m_image;

	DWORD	m_startTime;
public:
	CSceneImage(void);
	~CSceneImage(void);
	
	int GetWidth(){ return ( m_image && m_image->GetFrameCount() ?  m_image->GetBitmap(0)->GetWidth() : 0 ); }
	int GetHeight(){ return ( m_image && m_image->GetFrameCount() ?  m_image->GetBitmap(0)->GetHeight() : 0 ); }

	void Draw(POINT ptMapPixelOffset,DWORD dwColor);
	POINT GetSortPos();

	void Play();
};
