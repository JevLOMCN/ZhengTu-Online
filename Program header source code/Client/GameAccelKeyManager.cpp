/**
 *\file		GameAccelKeyManager.cpp
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:23:00 中国标准时间
 *\brief	快捷键管理器
 *
 * 
 *
*/

#include "public.h"
#include "./command.h"
#include ".\gameaccelkeymanager.h"
#include "./GameGuiManager.h"
#include "./GuiSkill.h"
#include "./GameScene.h"
#include "./GuiMain.h"
#include "./GuiFloatAccessKey.h"
#include "./GuiOtherAccessKey.h"
#include "./Game.h"
#include "./GuiEmotionDialog.h"
#include "./GuiAutoAttackDlg.h" //////////star100515

/**
 * \brief 返回游戏快捷键管理器
 * 
 * 
 * 
 * \return 游戏快捷键管理器指针
 */


/**
 * \brief 构造函数
 * 
 * 
 * 
 * \return 
 */
CGameAccelKeyManager::CGameAccelKeyManager(void)
{
	FUNCTION_BEGIN;
	
	m_curEditAccessKey.Init();
	
	//Init pre delete access key
	m_preDeleteAccessKey.dwID = -1;
	for( int i=0;i<4;i++ )
		m_preDeleteAccessKey.szAccelKey[i] = 0;
	m_preDeleteAccessKey.type = GameAccelKeyType_Num;

	FUNCTION_END;
}

/**
 * \brief 析构函数
 * 
 * 
 * 
 * \return 
 */
CGameAccelKeyManager::~CGameAccelKeyManager(void)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
 * \brief 快捷键管理器初始化
 * 
 * 仅在单机运行时需要调用
 * 
 * \return
 */
void CGameAccelKeyManager::Init()
{
	FUNCTION_BEGIN;

	if(g_bSinglePlayer)
	{
		stGameAccelKey key;
		memset(&key,0,sizeof(key));
		key.mGroup = 0;
		key.szAccelKey[0] = VK_F1;
		key.type = GameAccelKeyType_Skill;
		key.dwID = 275;
		SetKey(key);
		key.szAccelKey[0] = VK_F2;
		key.dwID = 251;
		SetKey(key);
		key.szAccelKey[0] = VK_F3;
		key.dwID = 254;
		SetKey(key);
		key.szAccelKey[0] = VK_F4;
		key.dwID = 2;
		SetKey(key);

		key.szAccelKey[0] = VK_F5;
		key.dwID = 270;
		SetKey(key);		
	}

	FUNCTION_END;
}

/**
 * \brief 释放所有快捷键
 * 
 * 
 * 
 * \return 
 */
void CGameAccelKeyManager::Release()
{
	FUNCTION_BEGIN;

	for(size_t i = 0; i< count_of(m_aKey);++i)
	{
		m_aKey[i].clear();
	}

	FUNCTION_END;
}

/**
 * \brief 设置一个快捷键
 * 
 * 
 * 
 * \param key : 一个快捷键，参见结构本身
 * \return 
 */
void CGameAccelKeyManager::SetKey(stGameAccelKey & key)
{
	FUNCTION_BEGIN;

	if(key.type >= GameAccelKeyType_Num) 
		return;
	if ( (key.mGroup >= MAIN_ACCESS_KEY_GROUP) && (key.mGroup != FLOAT_ACCESS_KEY_GROUP) && (key.mGroup != OTHER_ACCESS_KEY_GROUP)  && (key.mGroup != EMOTION_ACCESS_KEY_GROUP) )
		return;

	t_vecKeys::iterator itFind;
	enumGameAccelKeyType typeFind;
	
	if(FindCurAccelKeyIt(key.szAccelKey, key.mGroup, itFind,typeFind))
	{
		if(itFind->mKeyID == key.dwID && typeFind == key.type) return;
		m_aKey[typeFind].erase(itFind);
	}

	t_vecKeys & vecKey = m_aKey[key.type];
	stAccessKey gameKey;
	gameKey.mKeyID = key.dwID;

	CAccelKey accelKey;
	accelKey.Clear();

	for(size_t j = 0; j < count_of(key.szAccelKey); ++j)
	{
		if(key.szAccelKey[j] == 0)
			break;
		accelKey.InsertKey(key.szAccelKey[j]);
	}

    accelKey.SetGroup( key.mGroup );

	gameKey.mAcceKey = accelKey;

	//push to 
	vecKey.push_back( gameKey );

	t_vecKeys::iterator itFind1;
	enumGameAccelKeyType typeFind1;

	Engine_WriteLogF("是否成功:%d\n",FindCurAccelKeyIt(key.szAccelKey, key.mGroup, itFind1,typeFind1));

	FUNCTION_END;
}

/**
 * \brief 接受从服务器传来的快捷键列表
 * 
 * 
 * 
 * \param pCmd : 协议
 * \return 
 */
void CGameAccelKeyManager::SetAttrib(stAccekKeyPropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	Release();	
	for(size_t i=0;i<pCmd->accelNum;++i)
	{
		stGameAccelKey& key = pCmd->accelKeys[i];
		
		if(key.type >= GameAccelKeyType_Num) 
			continue;

		SetKey(key);
	}
	if(GetGameGuiManager()->m_guiSkill)
		GetGameGuiManager()->m_guiSkill->RefreshState();
	if ( GetScene()->GetMainCharacter() )
		GetScene()->GetMainCharacter()->SetPreferAccessGroup( pCmd->activeGroup );

	if ( GetGameGuiManager()->m_guiMain )
		GetGameGuiManager()->m_guiMain->SetActiveAccessKeyGroup( pCmd->activeGroup );
	if ( GetGameGuiManager()->m_guiFloatAcessKey )
		GetGameGuiManager()->m_guiFloatAcessKey->RefreshKeyGroupState();
	if ( GetGameGuiManager()->m_guiOtherAccessKey )
		GetGameGuiManager()->m_guiOtherAccessKey->RefreshKeyGroupState();
	 
	GetEmotionInfo()->RefreshAccessKey();
	//TRACE( "Get cur key group=%d\n", pCmd->activeGroup );

	FUNCTION_END;
}

/**
 * \brief 删除一个快捷键
 * 
 * 
 * 
 * \param key[4] : 键
 * \param group : 组
 * \return 成功返回true 否则返回false
 */
bool CGameAccelKeyManager::DeleteOneAccelKey( BYTE key[4], BYTE group )
{
	FUNCTION_BEGIN;

	for( size_t i=0;i<count_of(m_aKey);i++ )
	{
		t_vecKeys & vecKey = m_aKey[i];
		for( t_vecKeys::iterator it = vecKey.begin(); it != vecKey.end(); ++it )
		{
			if( it->mAcceKey.IsKey(key,4) && it->mAcceKey.IsSameGroup(group) )
				break;
		}

		if ( it != vecKey.end() )
		{
			vecKey.erase( it );
			return true;
		}
	}

	return false;

	FUNCTION_END;
}

/**
 * \brief 删除快捷键
 * 
 * 
 * 
 * \param type : 快捷键类型	[in]
 * \param id : 快捷键id		[in]
 * \param key[4] : 键           [out]
 * \param group : 组            [out]
 * \return 成功返回true 否则返回false
 */
bool CGameAccelKeyManager::DeleteAccelKey( enumGameAccelKeyType& type, DWORD& id, BYTE key[4], BYTE& group )
{
	FUNCTION_BEGIN;

	if ( type >= GameAccelKeyType_Num )
		return false;
	
	t_vecKeys::iterator it;
	for( it = m_aKey[type].begin(); it != m_aKey[type].end(); ++it )
	{
		if ( it->mKeyID == id )
			break;
	}

	if ( it != m_aKey[type].end() )
	{
		size_t num = it->mAcceKey.GetKey( key, 4 );
		group = it->mAcceKey.GetGroup();

		//Save cur delete access key
		m_preDeleteAccessKey.dwID = id;
		m_preDeleteAccessKey.mGroup = group;

		for( int i=0;i<4;i++ )
			m_preDeleteAccessKey.szAccelKey[0] = 0;
		m_preDeleteAccessKey.szAccelKey[0] = key[0];

		m_preDeleteAccessKey.type = type;

		//delete it
		m_aKey[type].erase( it );
		return true;
	}

	return false;

	FUNCTION_END;
}

bool CGameAccelKeyManager::ReplaceAccelKey( enumGameAccelKeyType& type, const DWORD& id, const DWORD& newID )
{
	FUNCTION_BEGIN;

	if ( type >= GameAccelKeyType_Num )
		return false;

	t_vecKeys::iterator it;
	for( it = m_aKey[type].begin(); it != m_aKey[type].end(); ++it )
	{
		if ( it->mKeyID == id )
			break;
	}

	if ( it != m_aKey[type].end() )
	{
		it->mKeyID = newID;		
		return true;
	}

	return false;

	FUNCTION_END;
}

void CGameAccelKeyManager::DeleteAccessKeyByType( enumGameAccelKeyType type )
{
	if ( type < GameAccelKeyType_Num )
		m_aKey[type].clear();
}

/**
 * \brief 查找一个快捷键
 * 
 * 
 * 
 * \param key[4] : 键
 * \param group : 组
 * \param itResult : iterator
 * \param type : 类型
 * \return 找到返回true 否则返回false
 */
bool CGameAccelKeyManager::FindCurAccelKeyIt(BYTE key[4], BYTE group, CGameAccelKeyManager::t_vecKeys::iterator & itResult,enumGameAccelKeyType& type)
{
	FUNCTION_BEGIN;

	for( size_t i = 0; i< count_of(m_aKey);++i )
	{
		t_vecKeys & vecKey = m_aKey[i];
		for( t_vecKeys::iterator it = vecKey.begin(); it != vecKey.end(); ++it )
		{
			if( it->mAcceKey.IsKey(key,4) && it->mAcceKey.IsSameGroup(group) )
			{
				type = (enumGameAccelKeyType)i;
				itResult = it;
				return true;
			}
		}
	}
	return false;

	FUNCTION_END;
}

/**
 * \brief 查找一个快捷键
 * 
 * 
 * 
 * \param key[4] : 键
 * \param group : 组
 * \param type : 类型
 * \param id : id
 * \return 找到返回true 否则返回false
 */
bool CGameAccelKeyManager::GetAccessKeyIdAndType(BYTE key[4], BYTE group, enumGameAccelKeyType& type, DWORD& id)
{
	FUNCTION_BEGIN;

	if ( (group >= MAIN_ACCESS_KEY_GROUP) && (group != FLOAT_ACCESS_KEY_GROUP) && (group != OTHER_ACCESS_KEY_GROUP) && (group != EMOTION_ACCESS_KEY_GROUP) )
		return false;

	t_vecKeys::iterator itFind;
	if ( FindCurAccelKeyIt( key, group, itFind, type ) )
	{
		id = itFind->mKeyID;
		return true;
	}
	return false;

	FUNCTION_END;
}

/**
 * \brief 得到某个快捷键id
 * 
 * 
 * 
 * \param type : 类型
 * \param dwID : id
 * \param group : 组
 * \param key[4] : 键
 * \return 找到返回true 否则返回false
 */
bool CGameAccelKeyManager::GetCurAccelKeyID(enumGameAccelKeyType & type,DWORD & dwID, BYTE& group, BYTE key[4])
{
	FUNCTION_BEGIN;
	
	for( size_t i = 0; i< count_of(m_aKey);++i )
	{
		t_vecKeys & vecKey = m_aKey[i];
		for( t_vecKeys::iterator it = vecKey.begin(); it != vecKey.end(); ++it )
		{
			bool keyDown = false;
			if ( key )		//Btn pressed
				keyDown = it->mAcceKey.IsKey( key, 4 );
			else			//Use access key
				keyDown = it->mAcceKey.IsKeyDown();

			if( keyDown == true)
				int ii = 0;
			if( keyDown && (it->mAcceKey.IsSameGroup(group) || it->mAcceKey.IsNumKey() || it->mAcceKey.IsAltDown())  )
			{
				type = (enumGameAccelKeyType)i;
				dwID = it->mKeyID;
				return true;
			}
		}
	}

	return false;

	FUNCTION_END;
}

/**
 * \brief 保存快捷键
 * 
 * 
 * 
 * \param activeGroup : 当前组
 * \return 
 */
void CGameAccelKeyManager::Save( BYTE activeGroup )
{
	FUNCTION_BEGIN;

	SetFrameAllocator fa;
	
	int count = GetKeyCount();
	size_t cmdSize = sizeof(stAccekKeyPropertyUserCmd) + sizeof(stGameAccelKey) * count;
	stAccekKeyPropertyUserCmd* cmd = 
		(stAccekKeyPropertyUserCmd*)FrameAllocator::alloc(cmdSize);

	constructInPlace(cmd);
	cmd->activeGroup = activeGroup;
	cmd->accelNum = count;
	int ix = 0;
	for(size_t i=0 ; i < GameAccelKeyType_Num ; ++i)
	{
		for(t_vecKeys::iterator it = m_aKey[i].begin();it != m_aKey[i].end();++it)
		{
			stGameAccelKey& st = cmd->accelKeys[ix++];
			st.type = (enumGameAccelKeyType)i;
			st.dwID = it->mKeyID;
			int num = it->mAcceKey.GetKey(st.szAccelKey,3);
			st.szAccelKey[num] = 0;
			st.mGroup = it->mAcceKey.GetGroup();
		}
	}

	Client_SendCommand(cmd,cmdSize,false);

	FUNCTION_END;
}

/**
 * \brief 得到一个快捷键
 * 
 * 
 * 
 * \param type : 类型
 * \param dwID : id
 * \return 快捷键指针
 */
CAccelKey* CGameAccelKeyManager::GetKey(enumGameAccelKeyType type,DWORD dwID)
{
	FUNCTION_BEGIN;

	t_vecKeys::iterator it;
	for( it=m_aKey[type].begin();it!=m_aKey[type].end();it++ )
	{
		if ( it->mKeyID == dwID )
			break;
	}
	if(it == m_aKey[type].end())
		return NULL;
	return &it->mAcceKey;

	FUNCTION_END;
}

void CGameAccelKeyManager::DeleteCurEditAccess()
{
	FUNCTION_BEGIN;
	Engine_WriteLogF("扔掉快捷方式\n");
	switch (m_curEditAccessKey.placeAccessKe)
	{
	case PLACE_ACCESSKEY_OTHER:
		{
			BYTE key[4];
			key[0] = VK_CONTROL;
			key[1] = VK_F1 + m_curEditAccessKey.keyNo;
			key[2] = 0;
			if ( GetGameGuiManager()->m_guiOtherAccessKey )
			{
				if ( DeleteOneAccelKey( key, (BYTE)OTHER_ACCESS_KEY_GROUP ) )
					GetGameGuiManager()->m_guiOtherAccessKey->RemoveAccessKeyBtnInfo( (BYTE)m_curEditAccessKey.keyNo );
			}
		}
		break;
	case PLACE_ACCESSKEY_MAIN:
		{
			BYTE key[4];
			key[0] = VK_F1 + m_curEditAccessKey.keyNo;
			key[1] = 0;

			if ( GetGameGuiManager()->m_guiMain )
			{
				if ( DeleteOneAccelKey( key, (BYTE)GetGameGuiManager()->m_guiMain->GetCurAccessKeyGroup() ) )
					GetGameGuiManager()->m_guiMain->RemoveAccessKeyBtnInfo( (BYTE)m_curEditAccessKey.keyNo );
			}
		}
		break;
	case PLACE_ACCESSKEY_FLOAT:
		{
			BYTE key[4];
			key[0] = '0' + m_curEditAccessKey.keyNo;
			key[1] = 0;

			if ( GetGameGuiManager()->m_guiFloatAcessKey )
			{
				if ( DeleteOneAccelKey( key, (BYTE)FLOAT_ACCESS_KEY_GROUP ) )
					GetGameGuiManager()->m_guiFloatAcessKey->RemoveAccessKeyBtnInfo( (BYTE)m_curEditAccessKey.keyNo );
			}
		}
		break;
	}
	/* if ( m_curEditAccessKey.bMainAccessKey )
	{
		//Really has a key to delete
		BYTE key[4];
		key[0] = VK_F1 + m_curEditAccessKey.keyNo;
		key[1] = 0;

		if ( GetGameGuiManager()->m_guiMain )
		{
			if ( DeleteOneAccelKey( key, (BYTE)GetGameGuiManager()->m_guiMain->GetCurAccessKeyGroup() ) )
				GetGameGuiManager()->m_guiMain->RemoveAccessKeyBtnInfo( (BYTE)m_curEditAccessKey.keyNo );
		}
	}
	else
	{
		//Really has a key to delete
		BYTE key[4];
		key[0] = '0' + m_curEditAccessKey.keyNo;
		key[1] = 0;

		if ( GetGameGuiManager()->m_guiFloatAcessKey )
		{
			if ( DeleteOneAccelKey( key, (BYTE)FLOAT_ACCESS_KEY_GROUP ) )
				GetGameGuiManager()->m_guiFloatAcessKey->RemoveAccessKeyBtnInfo( (BYTE)m_curEditAccessKey.keyNo );
		}

	} */
	FUNCTION_END;
}