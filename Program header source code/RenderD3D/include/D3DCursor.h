#pragma once
#include "..\..\engine\include\igraphiccursor.h"

class CD3DCursor :
	public IGraphicCursor
{
	int								mIndex;
	bool							mShow;
	std::vector<IDirect3DSurface8*> mCursorBitmap;
	IBitmaps*						mCurBmps;
	POINT			   mptHot;
public:
	CD3DCursor(void);
	~CD3DCursor(void);

	void init();
	void invalidateDevice();
	void release();

	bool loadCursor(const char* filename,POINT ptHot);
	bool loadCursor(IBitmaps* pBmps,POINT ptHot);
	void showCursor(bool bShow);
	void setCursor(int index);
	void run();
};
