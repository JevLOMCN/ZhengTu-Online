/**
 *\file		GameAccelKeyManager.h
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:23:00 中国标准时间
 *\brief	快捷键管理器
 *
 * 
 *
*/

#pragma once

#include "./command.h"
#include "../gui/include/AccelKey.h"

enum enumPlaceAccessKey
{
	PLACE_ACCESSKEY_NONE,
	PLACE_ACCESSKEY_MAIN,
	PLACE_ACCESSKEY_FLOAT,
	PLACE_ACCESSKEY_OTHER,
};

struct stEditAccesskeyInfo{
	bool bMainAccessKey;
	DWORD placeAccessKe;
	stResourceLocation rl;
	int keyNo;
	int group;
	DWORD	keyID;
	enumGameAccelKeyType keyType;
	std::string m_strText;
	int m_iFont;

	void Init()
	{
		keyNo = -1;
		group = -1;
		keyType = GameAccelKeyType_Num;
		placeAccessKe = PLACE_ACCESSKEY_NONE;
	}

	bool IsValid()
	{
		if ( ( (keyType != GameAccelKeyType_Item) 
			&& (keyType != GameAccelKeyType_Skill) 
			&& (keyType != GameAccelKeyType_Equipment) 
			&& (keyType != GameAccelKeyType_Face)
			&& (keyType != GameAccelKeyType_TaoZhuang)
			&& (keyType != GameAccelKeyType_AttPak)
			)
			|| (group == -1)
			|| (keyNo == -1) )
		{
			return false;
		}
		else
			return true;
	}
};
/**
 * \brief 游戏快捷键管理器
 * 
 * 用于游戏快捷键的增加、删除等管理
 * 
 */
class CGameAccelKeyManager
{
	struct stAccessKey
	{
		DWORD	mKeyID;
		CAccelKey	mAcceKey;
	};
	typedef std::vector<stAccessKey>	t_vecKeys;
	///快捷键列表
	t_vecKeys m_aKey[GameAccelKeyType_Num];	
public:
	stEditAccesskeyInfo m_curEditAccessKey;
	stGameAccelKey		m_preDeleteAccessKey;
public:
	CGameAccelKeyManager(void);
	~CGameAccelKeyManager(void);
	void SetAttrib(stAccekKeyPropertyUserCmd* pCmd);
	void Save(BYTE activeGroup = 0);
	void Release();
	void SetKey(stGameAccelKey & key);
	bool GetCurAccelKeyID(enumGameAccelKeyType & type,DWORD & dwID, BYTE& group, BYTE key[4]=0);
	CAccelKey* GetKey(enumGameAccelKeyType type,DWORD dwID);
	int GetKeyCount()
	{
		int count = 0;
		for(size_t i=0;i<GameAccelKeyType_Num;++i)
		{
			count += m_aKey[i].size();
		}
		return count;
	}
	bool FindCurAccelKeyIt(BYTE key[4], BYTE group, t_vecKeys::iterator & it,enumGameAccelKeyType& type);
	bool GetAccessKeyIdAndType(BYTE key[4], BYTE group, enumGameAccelKeyType& type, DWORD& id);
	void Init();
	
	//Delete one access key
	void DeleteAccessKeyByType(enumGameAccelKeyType type);
	bool DeleteOneAccelKey(BYTE key[4], BYTE group);
	bool DeleteAccelKey(enumGameAccelKeyType& type, DWORD& id, BYTE key[4], BYTE& group);
	bool ReplaceAccelKey(enumGameAccelKeyType& type, const DWORD& id, const DWORD& newID);

	void DeleteCurEditAccess();
};

inline CGameAccelKeyManager* GetGameAccelKeyManager()
{
	static CGameAccelKeyManager sm;
	return &sm;
}