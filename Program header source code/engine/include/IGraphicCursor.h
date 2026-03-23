#pragma once

class IGraphicCursor
{
public:
	IGraphicCursor(void);
	~IGraphicCursor(void);

	virtual bool loadCursor(const char* filename,POINT ptHot) = 0;
	virtual bool loadCursor(IBitmaps* pBmps,POINT ptHot) = 0;
	virtual void showCursor(bool bShow) = 0;
	virtual void run() = 0;
};
