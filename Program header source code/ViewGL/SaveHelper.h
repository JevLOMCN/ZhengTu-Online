#pragma once


#include "../engine/include/engine.h"


enum ITEMDATA_INFO
{
   ITEM_ROOT = 1,          // 代表整个gl文件
   ITEM_GROUP = 2,	   // 代表gl文件中的某个组
   ITEM_FRAME = 3,	   // 代表gl文件中的某个frame(如果是个文件夹，则为一个动画)
   ITEM_FRAME_FRAME = 4,   // 代表动画文件里的某一帧
};

struct SAVE_INFO
{
	IBitmapList * pBitmapList;   // 整个图包接口
	ITEMDATA_INFO itemdata;		// 指出要保存哪部分
	size_t group;			// 要保存的组
	size_t frame;			// 要保存的帧(可能为动画)
	size_t frameframe;	     // 动画里的某一帧

};


class CSaveHelper
{
public:
	CSaveHelper(void);
	~CSaveHelper(void);

public:
	BOOL SaveToTga(const char* pszPath,const SAVE_INFO & saveInfo,bool bObject = false);
	BOOL SaveIBitmap(IBitmap * pBmp,const char * pszName);
	char m_strFileName[MAX_PATH];

private:
	char m_szSavePath[MAX_PATH];
};
