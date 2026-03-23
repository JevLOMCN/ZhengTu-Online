#pragma once
#include "..\gui\include\guifloatitem.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CGameGuiFloatItem :
	public CGuiFloatItem
{
public:
	CGameGuiFloatItem(void);
	~CGameGuiFloatItem(void);
	void OnCreate();
};
