/**
 * \file
 * \version  $Id: SkillStatusManager.cpp  $
 * \author 
 * \date 
 * \brief 定义技能状态管理器头文件
 *
 */

#include "SkillBase.h"
#include "SkillStatusManager.h"
#include "SceneUser.h"
#include "ScenePk.h"
#include "SceneUserManager.h"
#include "Scene.h"
#include "Chat.h"

/**
 * \brief	状态0 空状态，不做任何操作
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_0(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 0;
	return SKILL_RETURN;
}

/**
 * \brief	状态1 对状态接受者伤害增加固定数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_1(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 1;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dvalue += sse.value;
					//pEntry->pkValue.dvalue += sse.value;
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.dvalue += sse.value;
				//pEntry->pkValue.dvalue += sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态2 对状态接受者伤害增加百分比，基准数值为角色的裸身与装备攻击力的总和
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_2(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 1;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dvaluep = sse.value;
					pEntry->pkValue.dvaluep = sse.value;
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.dvaluep = sse.value;
				pEntry->pkValue.dvaluep = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态3 技能的物理攻击力[此状态可能已经被废弃]
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_3(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 2;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
//					pEntry->skillValue.pdam = sse.value;
					pEntry->pkValue.pdamage = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
//				pEntry->skillValue.pdam += sse.value;
				pEntry->pkValue.pdamage = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态4 指定状态接受者的物理防御基础值[基本上不存在这种技能所以此状态可能也已经被废弃]
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_4(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 3;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
//					pEntry->skillValue.pdef = sse.value;
					pEntry->pkValue.pdefence = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->pkValue.pdefence = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态5 表示状态接受者裸身法术攻击力,装备法术攻击的力总和
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_5(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 10;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
//					pEntry->skillValue.mdam = sse.value;
					pEntry->pkValue.mdamage = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->pkValue.mdamage = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态6 表示状态接受者裸身法术防御力,装备法术防御力的总和
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_6(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 11;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					//pEntry->skillValue.mdef = sse.value;
					pEntry->pkValue.mdefence = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->pkValue.mdefence = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态7 消耗状态接受者的固定值法术值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_7(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 4;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(0.5f*pUser->charstate.wdMen);
		}
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->changeMP(0-sse.value-value);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->changeMP(0-sse.value-value);
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态8 消耗状态接受者的百分比法术值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_8(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 4;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					SDWORD value = (SDWORD)(pEntry->getMaxMP()*(sse.value/100.0f));
					pEntry->changeMP(value);
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				SDWORD value = (SDWORD)(pEntry->getMaxMP()*(sse.value/100.0f));
				pEntry->changeMP(value);
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态9 消耗状态接受者的固定值生命值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_9(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 5;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(1*pUser->charstate.wdMen);
		}
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					//pEntry->changeHP(0-sse.value);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					case zSceneEntry::SceneEntry_NPC:		// NPC
						{
							pAtt = SceneNpcManager::getMe().getNpcByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}

				if (pAtt)
				{
					pEntry->directDamage(pAtt, sse.value+value,true);
					pEntry->processDeath(pAtt);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态10 消耗状态接受者的百分比生命值，基准为最大生命值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_10(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 5;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					//SDWORD value = (SDWORD)(pEntry->getMaxHP()*(sse.value/100.0f));
					//pEntry->changeHP(0-value);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					case zSceneEntry::SceneEntry_NPC:		// NPC
						{
							pAtt = SceneNpcManager::getMe().getNpcByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}

				if (pAtt)
				{
					SDWORD value = (SDWORD)(pEntry->getMaxHP()*(sse.value/100.0f));
					pEntry->directDamage(pAtt, value, true);
					pEntry->processDeath(pAtt);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态11 消耗耗状态接受者的固定值体力值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_11(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 6;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->changeSP(0-sse.value);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->changeSP(0-sse.value);
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态12 消耗状态接受者的百分比体力值，基准是最大体力值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_12(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 6;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					SDWORD value = (SDWORD)(pEntry->getMaxSP()*(sse.value/100.0f));
					pEntry->changeSP(value);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				SDWORD value = (SDWORD)(pEntry->getMaxSP()*(sse.value/100.0f));
				pEntry->changeSP(value);
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态13 表示状态接受者的魔法防御力提升固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_13(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.umdef = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.umdef = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.umdef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态14 表示状态接受者的魔法防御力提升百分比(冰甲专用)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_14(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.ice_umdefp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.ice_umdefp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.ice_umdefp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态15 表示将状态接受者此次受到的攻击造成的伤害值归零（包括物理的和法术的）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_15(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 8;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->ignoreDam = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->ignoreDam = true;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->ignoreDam = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态16 表示增加状态接受者的移动速度
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_16(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 9;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(0.1f*pUser->charstate.wdMen);
		//	value= (WORD)(0.1f*pUser->charstate.movespeed); //soke 修改移动速度
		}
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->reSendOther = true;
					pEntry->skillValue.movespeed = sse.value+value;

					//xwl add
					if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
						((SceneNpc *)pEntry)->resetSpeedRate();
					//pEntry->reSendMyMapData();
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->reSendOther = true;
				pEntry->skillValue.movespeed = 0;

				//xwl add
				if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
					((SceneNpc *)pEntry)->resetSpeedRate();

				//pEntry->reSendMyMapData();
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态17 降低状态接受者再次施放此技能的冷却时间
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_17(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 12;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.mgspeed = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.mgspeed = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.mgspeed = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态18 减少状态接受者陷入冰冻状态的几率
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_18(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 13;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.coldp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.coldp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.coldp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态19 减少状态接受者陷入中毒状态几率
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_19(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 14;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.poisonp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.poisonp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.poisonp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态20 减少状态接受者陷入石化状态的几率
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_20(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 15;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.petrifyp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.petrifyp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.petrifyp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态21 减少状态接受者陷入失明状态几率
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_21(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 16;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.blindp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.blindp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态22 减少状态接受者陷入混乱状态几率
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_22(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 17;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.chaosp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.chaosp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.chaosp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态23 增加状态接受者物理攻击力固定值(一次性攻击，攻击时长不超过2秒)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_23(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.updam = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.updam = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.updam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态24 增加状态接受者物理攻击力百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_24(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 201;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.updamp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.updamp = sse.value;
				return SKILL_RETURN;	
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.updamp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态25 增加状态接受者法术攻击力固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_25(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 21;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.umdam = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.umdam = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.umdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态26 增加状态接受者法术攻击力百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_26(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 211;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.umdamp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.umdamp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.umdamp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态27 增加状态接受者生命值上限固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_27(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

//	pEntry->skillValue.maxhpup = sse.value;
	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				pEntry->skillValue.maxhp = (SWORD)(sse.value);
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.maxhp = 0;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态28 增加状态接受者生命值上限百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_28(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				pEntry->skillValue.maxhp = (SWORD)(pEntry->getBaseMaxHP()*(sse.value/100.0f));
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.maxhp = 0;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态29 增加状态接受者法术值上限固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_29(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 23;
	pEntry->reSendData = true;

//	pEntry->skillValue.maxmpup = sse.value;
	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				pEntry->skillValue.maxmp = (SWORD)(pEntry->getBaseMaxMP()+sse.value);
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.maxmp = 0;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态30 增加状态接受者法术值上限百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_30(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 23;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				pEntry->skillValue.maxmp = (SWORD)(pEntry->getBaseMaxMP()*(sse.value/100.0f));
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.maxmp =0;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态31 增加状态接受者体力值上限固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_31(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 24;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				pEntry->skillValue.maxsp = (SWORD)(pEntry->getMaxSP()+sse.value);
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.maxsp = 0;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态32 增加状态接受者体力值上限百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_32(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 24;
	pEntry->reSendData = true;

	//pEntry->skillValue.maxspupp = sse.value;
	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				pEntry->skillValue.maxsp = (SWORD)(pEntry->getMaxSP()*(sse.value/100.0f));
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.maxsp = 0;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态33 增加状态接受者的命中率
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_33(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 25;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.atrating = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.atrating = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.atrating = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态34 增加状态接受者生命值回复速度
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_34(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 26;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.hpspeedup = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.hpspeedup = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.hpspeedup = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态35 增加状态接受者法术值回复速度
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_35(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 27;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.mpspeedup = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.mpspeedup = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.mpspeedup = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态36 增加状态接受者体力值回复速度
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_36(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 28;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.spspeedup = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.spspeedup = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.spspeedup = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态37 增加状态接受者闪避率
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_37(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 29;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.akdodge = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.akdodge = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.akdodge = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态38 增加状态接受者生命值固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_38(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 30;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->changeHP(sse.value);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->changeHP(sse.value);
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态39 回春技能 桃子修改为不读取精神
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_39(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 30;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(0.000001f*pUser->charstate.wdMen);
		}
	}
	value +=sse.value;
	if (value>100) value =100;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->changeHP((SDWORD)(pEntry->getMaxHP()*(value/100.0f)));
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->changeHP((SDWORD)(pEntry->getMaxHP()*(value/100.0f)));
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态40 增加状态接受者法术值固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_40(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 31;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->changeMP(sse.value);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->changeMP(sse.value);
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态41 增加状态接受者法术值百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_41(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 31;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->changeMP((SDWORD)(pEntry->getMaxMP()*(sse.value/100.0f)));
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->changeMP((SDWORD)(pEntry->getMaxMP()*(sse.value/100.0f)));
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态42 增加状态接受者体力值固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_42(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 32;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->changeSP(sse.value);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->changeSP(sse.value);
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态43 增加状态接受者体力值百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_43(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 32;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->changeSP((SDWORD)(pEntry->getMaxSP()*(sse.value/100.0f)));
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->changeSP((SDWORD)(pEntry->getMaxSP()*(sse.value/100.0f)));
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态44 状态接受者被复活
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_44(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 33;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->relive();
					return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态45 状态接受者受到伤害时,反弹固定值伤害(不包括远程法术和远程攻击)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_45(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 8;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.reflect = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.reflect = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.reflect = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态46 状态接受者受到伤害时,反弹百分比伤害(不包括远程法术和远程攻击)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_46(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 8;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.reflectp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.reflectp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.reflectp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态47 状态接受者被击退3格
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_47(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 64;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->standBack(sse.dwTempID, 3);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->standBack(sse.dwTempID, 3);
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态48 消除目标或自身正在持续时间内的魔法效果(正面负面效果都全部消除)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_48(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 65;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillStatusM.clearActiveSkillStatusOnlyUseToStatus48();
					return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态49 消除角色或者目标玩家身上的所有不良状态(不良状态另表列明)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_49(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 66;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillStatusM.clearBadActiveSkillStatus();
					return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态50 当角色或目标受到伤害时,反弹百分比伤害(可以反弹近距技能攻击伤害)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_50(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 8;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.reflect2 = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.reflect2 = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.reflect2 = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态51 中毒状态
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_51(SceneEntryPk *pEntry, SkillStatusElement &sse) // 原来的timer方法
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 34;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent(((int)(sse.percent - pEntry->skillValue.poisonp-pEntry->skillValue.sevendownp)) > 0 ? (sse.percent - pEntry->skillValue.poisonp-pEntry->skillValue.sevendownp):0))
				{
					if (zMisc::selectByPercent(pEntry->getPoisondef()))
					{
						pEntry->sendMessageToMe("中毒状态被抵抗");
						return SKILL_RETURN;
					}
					else
					{
						return SKILL_ACTIVE;
					}
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					case zSceneEntry::SceneEntry_NPC:		// NPC
						{
							pAtt = SceneNpcManager::getMe().getNpcByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}

				if (pAtt)
				{
					pEntry->directDamage(pAtt, sse.value, true);
					pEntry->processDeath(pAtt);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态52 石化状态
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_52(SceneEntryPk *pEntry, SkillStatusElement &sse) // 原来的timer方法
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 35;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent(((int)(sse.percent - pEntry->skillValue.petrifyp-pEntry->skillValue.sevendownp)) > 0 ? (sse.percent - pEntry->skillValue.petrifyp-pEntry->skillValue.sevendownp):0))
				{
					if (zMisc::selectByPercent(pEntry->getPetrifydef()))
					{
						pEntry->sendMessageToMe("石化状态被抵抗");
						return SKILL_RETURN;
					}
					else
					{
						pEntry->attackAction = false;
						//pEntry->ignoreDam = true;
						pEntry->moveAction = false;
						pEntry->useRes = false;
						//pEntry->stone = true;
						return SKILL_RECOVERY;
					}
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->attackAction = false;
				//pEntry->ignoreDam = true;
				pEntry->moveAction = false;
				pEntry->useRes = false;
				//pEntry->stone = true;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->attackAction = true;
				//pEntry->ignoreDam = false;
				pEntry->moveAction = true;
				pEntry->useRes = true;
				//pEntry->stone = false;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}


/**
 * \brief	状态53 失明状态
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_53(SceneEntryPk *pEntry, SkillStatusElement &sse)  //原来的timer方法
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 36;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent(((int)(sse.percent - pEntry->skillValue.blindp-pEntry->skillValue.sevendownp)) > 0 ? (sse.percent - pEntry->skillValue.blindp-pEntry->skillValue.sevendownp):0))
				{
					if (zMisc::selectByPercent(pEntry->getBlinddef()))
					{
						pEntry->sendMessageToMe("失明状态被抵抗");
						return SKILL_RETURN;
					}
					else
					{
						pEntry->blind = true;
						return SKILL_ACTIVE;
					}
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->blind = true;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->blind = false;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}



/**
 * \brief	状态54 混乱状态
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_54(SceneEntryPk *pEntry, SkillStatusElement &sse) // 原来的timer方法
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 37;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent(((int)(sse.percent - pEntry->skillValue.chaosp-pEntry->skillValue.sevendownp)) > 0 ? (sse.percent - pEntry->skillValue.chaosp-pEntry->skillValue.sevendownp):0))
				{
					if (zMisc::selectByPercent(pEntry->getChaosdef()))
					{
						pEntry->sendMessageToMe("混乱状态被抵抗");
						return SKILL_RETURN;
					}
					else
					{
						pEntry->dread = true;
						return SKILL_ACTIVE;
					}
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
/*				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					case zSceneEntry::SceneEntry_NPC:		// NPC
						{
							pAtt = SceneNpcManager::getMe().getNpcByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}

				if (pAtt)
				{
					pEntry->directDamage(pAtt, sse.value, true);
					pEntry->processDeath(pAtt);
				}*/
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->dread = false;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}


/**
 * \brief	状态55 冰冻状态
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_55(SceneEntryPk *pEntry, SkillStatusElement &sse)  // 原来的time方法
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 38;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent(((int)(sse.percent - pEntry->skillValue.coldp-pEntry->skillValue.sevendownp)) > 0 ? (sse.percent - pEntry->skillValue.coldp-pEntry->skillValue.sevendownp):0))
				{
					if (zMisc::selectByPercent(pEntry->getColddef()))
					{
						pEntry->sendMessageToMe("冰冻状态被抵抗");
						return SKILL_RETURN;
					}
					else
					{
						pEntry->reSendOther = true;
						pEntry->useRes = false;
						pEntry->attackAction = false;
						pEntry->skillAction = false;
						pEntry->reSendOther = true;
						pEntry->skillValue.movespeed = -900;
	
						//xwl add
						if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
							((SceneNpc *)pEntry)->resetSpeedRate();
						return SKILL_ACTIVE;
					}
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->reSendOther = true;
				pEntry->useRes = false;
				pEntry->attackAction = false;
				pEntry->skillAction = false;
				if(pEntry->skillValue.movespeed != -900)
				{
					pEntry->reSendOther = true;
				}
				pEntry->skillValue.movespeed = -900;

				//xwl add
				if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
					((SceneNpc *)pEntry)->resetSpeedRate();
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->reSendOther = true;
				pEntry->useRes = true;
				pEntry->attackAction = true;
				pEntry->skillAction = true;
				pEntry->reSendOther = true;
				pEntry->skillValue.movespeed = 0;
	
				//xwl add
				if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
					((SceneNpc *)pEntry)->resetSpeedRate();
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态56 使角色或目标移动速度降低
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_56(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 39;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent(((int)(sse.percent-pEntry->skillValue.sevendownp)) > 0 ? (sse.percent-pEntry->skillValue.sevendownp):0))
				{
					if (zMisc::selectByPercent(pEntry->getSlowdef()))
					{
						pEntry->sendMessageToMe("减速状态被抵抗");
						return SKILL_RETURN;
					}
					else
					{
						pEntry->reSendOther = true;
						pEntry->skillValue.movespeed = -900;
						//xwl add
						if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
							((SceneNpc *)pEntry)->resetSpeedRate();
						//pEntry->reSendMyMapData();
						return SKILL_RECOVERY;
					}
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if(pEntry->skillValue.movespeed != -900)
				{
					pEntry->reSendOther = true;
				}
				pEntry->skillValue.movespeed = -900;

				//xwl add
				if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
					((SceneNpc *)pEntry)->resetSpeedRate();

				//pEntry->reSendMyMapData();
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->reSendOther = true;
				pEntry->skillValue.movespeed = 0;

				//xwl add
				if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
					((SceneNpc *)pEntry)->resetSpeedRate();

				//pEntry->reSendMyMapData();
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态57 降低目标物理防御固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_57(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 40;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(0.3f*pUser->charstate.wdMen);
		}
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dpdef = sse.value+value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.dpdef = sse.value+value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.dpdef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态58 降低目标物理防御百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_58(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 40;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(0.04f*pUser->charstate.wdMen);
		}
	}
	value+=sse.value;
	if (value >100) value=100;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dpdefp = value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.dpdefp = value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.dpdefp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态59 降低目标法术防御固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_59(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 41;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(0.3f*pUser->charstate.wdMen);
		}
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dmdef = sse.value+value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.dmdef = sse.value+value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.dmdef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态60 法术防御降低百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_60(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 41;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(0.04f*pUser->charstate.wdMen);
		}
	}
	value+=sse.value;
	if (value >100) value=100;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dmdefp = value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.dmdefp = value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.dmdefp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态61 使角色或目标生命值持续下降,下降速度比中毒低
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_61(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 5;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					//pEntry->changeHP(0-sse.value);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					case zSceneEntry::SceneEntry_NPC:		// NPC
						{
							pAtt = SceneNpcManager::getMe().getNpcByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}

				if (pAtt)
				{
					pEntry->directDamage(pAtt, sse.value, true);
					pEntry->processDeath(pAtt);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态62 使角色或目标法术值持续下降,速度与生命值持续降低相当
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_62(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 4;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->changeMP(0-sse.value);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->changeMP(0-sse.value);
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态63 使角色或目标体力值持续下降,速度与生命值持续降低相当
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_63(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 6;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->changeSP(0-sse.value);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->changeSP(0-sse.value);
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态64 降低目标全部命中率,包括近距物理攻击,近距法术攻击,远程法术攻击和远程物理攻击
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_64(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 42;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.reduce_atrating = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.reduce_atrating = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.reduce_atrating = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态65 将敌人的注意力吸引到施法者身上(转换攻击目标为施法者.例如圣骑士对正在攻击其他目标的怪物施放,使怪物将攻击目标转为圣骑士)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_65(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 43;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->changeAttackTarget(sse.dwTempID);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->changeAttackTarget(sse.dwTempID);
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态66 将目标物理防御变为0
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_66(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 44;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->pdeftozero = true;   // 物理防御变成0
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->pdeftozero = false;   // 物理防御变成0
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态67 将目标法术防御变为0
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_67(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 44;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->mdeftozero = true;   // 物理防御变成0
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->mdeftozero = false;   // 物理防御变成0
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态68 增加目标不良状态持续时间固定秒数
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_68(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 45;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillStatusM.addBadSkillStatusPersistTime(sse.value);
					return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态69 增加目标不良状态持续时间百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_69(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 45;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillStatusM.addBadSkillStatusPersistTimePercent(sse.value);
					return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态70 使目标不能攻击
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_70(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 46;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				pEntry->attackAction = false;
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->attackAction = false;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->attackAction = true;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态71 使目标不能移动
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_71(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 62;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent(((int)(sse.percent-pEntry->skillValue.sevendownp)) > 0 ? (sse.percent-pEntry->skillValue.sevendownp):0))
				{
					if (zMisc::selectByPercent(pEntry->getStabledef()))
					{
						pEntry->sendMessageToMe("定身状态被抵抗");
						return SKILL_RETURN;
					}
					else
					{
						pEntry->moveAction = false;
						return SKILL_RECOVERY;
					}
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->moveAction = false;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->moveAction = true;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态72 降低角色或目标生命值固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_72(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	return SkillStatus_9(pEntry, sse);
}

/**
 * \brief	状态73 降低角色或目标生命值百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_73(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	return SkillStatus_10(pEntry, sse);
}

/**
 * \brief	状态74 降低角色或目标法术值固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_74(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	return SkillStatus_7(pEntry, sse);
}

/**
 * \brief	状态75 降低角色或目标体力值固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_75(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	return SkillStatus_8(pEntry, sse);
}

/**
 * \brief	状态76 降低角色或目标体力值固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_76(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	return SkillStatus_11(pEntry, sse);
}

/**
 * \brief	状态77 降低角色或目标体力值百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_77(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	return SkillStatus_12(pEntry, sse);
}

/**
 * \brief	状态78 使角色或目标生命值变为0,死亡
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_78(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 33;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->toDie(sse.dwTempID);
					return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态79 使目标变为随机小动物
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_79(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 50;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.topet = sse.value;
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态80 增加自身或者目标攻击速度百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_80(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 51;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->reSendOther = true;
					pEntry->skillValue.uattackspeed = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if(pEntry->skillValue.uattackspeed != sse.value)
				{
					pEntry->reSendOther = true;
				}
				pEntry->skillValue.uattackspeed = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->reSendOther = true;
				pEntry->skillValue.uattackspeed = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态81 降低目标攻击速度百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_81(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 52;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->reSendOther = true;
					pEntry->skillValue.dattackspeed = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if(pEntry->skillValue.dattackspeed != sse.value)
				{
					pEntry->reSendOther = true;
				}
				pEntry->skillValue.dattackspeed = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->reSendOther = true;
				pEntry->skillValue.dattackspeed = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态82 降低自身或者目标陷入冰冻,中毒,石化,混乱,失明,麻痹和流血的几率
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_82(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 53;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.sevendownp = 0-sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.sevendownp = 0-sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.sevendownp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态83 对目标产生额外伤害
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_83(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 63;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.appenddam += sse.value;
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					case zSceneEntry::SceneEntry_NPC:		// NPC
						{
							pAtt = SceneNpcManager::getMe().getNpcByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}

				if (pAtt)
				{
					/*SWORD wdHP = */pEntry->directDamage(pAtt,pEntry->skillValue.appenddam, true);
					//ScenePk::attackRTCmdToNine(rev , pAtt , this , wdHP , 0);
					pEntry->processDeath(pAtt);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态84 角色或目标可以移动,但是不可使用技能攻击
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_84(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 47;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent(((int)(sse.percent-pEntry->skillValue.sevendownp)) > 0 ? (sse.percent-pEntry->skillValue.sevendownp):0))
				{
					if (zMisc::selectByPercent(pEntry->getLulldef()))
					{
						pEntry->sendMessageToMe("麻痹状态被抵抗");
						return SKILL_RETURN;
					}
					else
					{
						pEntry->skillAction = false;
						pEntry->attackAction = false;
						return SKILL_RECOVERY;
					}
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillAction = false;
				pEntry->attackAction = false;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillAction = true;
				pEntry->attackAction = true;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态85 角色或目标生命值持续下降
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_85(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 48;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent(((int)(sse.percent - pEntry->skillValue.sevendownp)) > 0 ? (sse.percent - pEntry->skillValue.sevendownp):0))
				{
					//pEntry->changeHP(0-sse.value);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					case zSceneEntry::SceneEntry_NPC:		// NPC
						{
							pAtt = SceneNpcManager::getMe().getNpcByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}

				if (pAtt)
				{
					pEntry->directDamage(pAtt, sse.value, true);
					pEntry->processDeath(pAtt);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态86 角色或目标的物理防御力上升固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_86(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 54;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.updef = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.updef = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.updef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态87 角色或目标的物理防御力上升百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_87(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 54;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{

					pEntry->skillValue.updefp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.updefp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.updefp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态88 将角色的攻击力的百分比转换成防御力加到角色防御力上,同时攻击力减少相应数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_88(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 55;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					DWORD temp = (DWORD)(pEntry->pkValue.mdamage*(sse.value/100.0f));
					pEntry->pkValue.mdamage-=temp;
					pEntry->pkValue.mdefence+=temp;
					temp = (DWORD) (pEntry->pkValue.pdamage*(sse.value/100.0f));
					pEntry->pkValue.pdamage-=temp;
					pEntry->pkValue.pdefence+=temp;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态89 将角色或目标的物理攻击力降低固定数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_89(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 56;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(1*pUser->charstate.wdMen);
		}
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dpdam = sse.value+value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.dpdam = sse.value+value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.dpdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态90 将角色或目标的物理攻击力降低百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_90(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 56;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(0.02f*pUser->charstate.wdMen);
		}
	}
	value+=sse.value;
	if (value >100) value=100;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dpdamp = value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.dpdamp = value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.dpdamp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态91 将角色或目标的法术攻击力降低固定数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_91(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 57;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(1*pUser->charstate.wdMen);
		}
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dmdam = sse.value+value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.dmdam = sse.value+value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.dmdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态92 将角色或目标的法术攻击力降低百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_92(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 57;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(0.02f*pUser->charstate.wdMen);
		}
	}
	value+=sse.value;
	if (value >100) value=100;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dmdamp = value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.dmdamp = value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.dmdamp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态93 处于此状态下的目标不攻击施法者,目标在状态持续时始终尝试与施法者保持3格以上的距离
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_93(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 58;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->standBack(sse.dwTempID, 3);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->standBack(sse.dwTempID, 3);
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态94 处于此状态下的目标不可使用技能
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_94(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 59;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillAction = false;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillAction = true;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态95 将角色或目标的法术防御力上升固定数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_95(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.umdef = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.umdef = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.umdef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态96 法术防御上升百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_96(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.umdefp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.umdefp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.umdefp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}




/**
 * \brief	状态97 将角色或目标受到的伤害固定值转换到其他目标上
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_97(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 61;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.tsfdam =sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.tsfdam =sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.tsfdam =0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态98 将角色或目标受到的伤害百分比转换到其他目标上
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_98(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 61;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.tsfdamp =sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.tsfdamp =sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.tsfdamp =0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态99 瞬间移动
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_99(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 67;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->goToRandomScreen();
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态100 破釜沉舟状态（魔物攻击力翻300，持续时间结束后获得复活虚弱状态)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_100(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 0;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.erupt = sse.value;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
			{
				pEntry->skillValue.erupt = 0;
				if (pEntry->getType() == zSceneEntry::SceneEntry_Player)
				{
					SceneUser *pUser = (SceneUser *)pEntry;
					if (pUser->charbase.reliveWeakTime >0)
					{
						pUser->charbase.reliveWeakTime = (pUser->charbase.reliveWeakTime + sse.dwTime)%10000;
					}
					else
					{
						pUser->charbase.reliveWeakTime = (SceneTimeTick::currentTime.sec() + sse.dwTime)%10000;
						// 调用预处理方法，进行重算
						pUser->setupCharBase();

						Cmd::stMainUserDataUserCmd  userinfo;
						pUser->full_t_MainUserData(userinfo.data);
						pUser->sendCmdToMe(&userinfo,sizeof(userinfo));

						pUser->showCurrentEffect(Cmd::USTATE_RELIVEWEAK, true);
						pUser->sendtoSelectedReliveWeakState();
					}
				}

			}
			break;
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.erupt = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态101 未知状态
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_101(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 0;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态102 远程攻击取上限
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_102(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 0;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->maxattack = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->maxattack = true;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->maxattack = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态110 自身攻击取上限
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_110(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 0;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->maxattack = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->maxattack = true;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->maxattack = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态111 减少状态接受者闪避率
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_111(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 29;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.akdodge = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.akdodge = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.akdodge = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态113 召唤召唤兽
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_113(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 0;

	pEntry->summonPet(sse.value, Cmd::PET_TYPE_SUMMON, sse.dwTime, 0, "", 0, pEntry->keepPos);
	return SKILL_RETURN;
}

/**
 * \brief	状态114 重新计算所有技能的冷却时间
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_114(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
  sse.byGoodnessType = SKILL_BAD;
  sse.byMutexType = 0;

  switch(sse.byStep)
  {
    case ACTION_STEP_DOPASS:
    case ACTION_STEP_START:
    case ACTION_STEP_RELOAD:
      {
        if (zMisc::selectByPercent((int)(sse.percent)))
        {
          pEntry->usm.clearAllUseTime(sse.dwSkillID);
          return SKILL_RETURN;
        }
        else
          return SKILL_RETURN;
      }
      break;
    case ACTION_STEP_TIMER:
      {
        return SKILL_RETURN;
      }
      break;
    case ACTION_STEP_STOP:
    case ACTION_STEP_CLEAR:
      {
      }
      break;
    default:
      break;
  }

  return SKILL_RETURN;
}

/**
 * \brief	状态115 狂乱攻击
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_115(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 0;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->frenzy = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->frenzy = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态116 解除召唤
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_116(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 0;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->killAllPets();
					return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态117 被动：额外伤害
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_117(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 63;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.passdam += sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态119 不能回复HP,MP,SP
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_119(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 0;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->isAutoRestitute = false;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->isAutoRestitute = true;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态120 不能使用物品
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_120(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 63;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->useRes = false;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->useRes = true;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态121 冲锋
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_121(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 0;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					//pEntry->assaultMe(sse.attacktype, sse.dwTempID);
					pEntry->reSendData = true;
					pEntry->assault = true;
					pEntry->reSendOther = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->assault = false;
				pEntry->reSendData = true;
				pEntry->reSendOther = true;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态122 眩晕
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_122(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 0;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent(((int)(sse.percent-pEntry->skillValue.sevendownp)) > 0 ? (sse.percent-pEntry->skillValue.sevendownp):0))
				{
					if (zMisc::selectByPercent(pEntry->getReeldef()))
					{
						pEntry->sendMessageToMe("眩晕状态被抵抗");
						return SKILL_RETURN;
					}
					else
					{
						pEntry->attackAction = false;
						pEntry->skillAction = false;
						pEntry->reSendOther = true;
						pEntry->skillValue.movespeed = -1900;

					//xwl add
						if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
							((SceneNpc *)pEntry)->resetSpeedRate();
						return SKILL_RECOVERY;
					}
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->attackAction = false;
				pEntry->skillAction = false;
				if(pEntry->skillValue.movespeed != -1900)
				{
					pEntry->reSendOther = true;
				}
				pEntry->skillValue.movespeed = -1900;

				//xwl add
				if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
					((SceneNpc *)pEntry)->resetSpeedRate();

				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->attackAction = true;
				pEntry->skillAction = true;
				pEntry->reSendOther = true;
				pEntry->skillValue.movespeed = 0;

				//xwl add
				if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
					((SceneNpc *)pEntry)->resetSpeedRate();

			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态123 猎手印记
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_123(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 0;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->huntermark = true;
					pEntry->skillValue.brappenddam = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->huntermark = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态124 增加状态接受者物理攻击力固定值(被动)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_124(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 120;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.pupdam = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.pupdam = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.pupdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态125 增加状态接受者法术攻击力固定值(被动)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_125(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 120;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.pumdam = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.pumdam = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.pumdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态126 增加状态接受者物理防御力固定值(被动)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_126(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 121;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.pupdef = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.pupdef = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.pupdef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态127 增加状态接受者法术防御固定值(被动)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_127(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 121;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->checkWeapon(108))
						pEntry->skillValue.pumdef = sse.value;
					else
						pEntry->skillValue.pumdef = 0;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->checkWeapon(108))
					pEntry->skillValue.pumdef = sse.value;
				else
					pEntry->skillValue.pumdef = 0;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.pumdef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态128 增强(召唤兽被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_128(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) 
				{
					pEntry->summon->boostupSummon = (int)sse.value;
					pEntry->summon->sendData();
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) pEntry->summon->boostupSummon = (int)sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->summon &&((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10)))
				{
					pEntry->summon->boostupSummon = 0;
					pEntry->summon->sendData();
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态129 增加角色全部属性点固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_129(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 121;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.upattribute = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.upattribute = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.upattribute = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态130 增加命中率（被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_130(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 25;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.patrating = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.patrating = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.patrating = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态131 增加攻击攻击速度（被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_131(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 52;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->reSendOther = true;
					pEntry->skillValue.pattackspeed = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if(pEntry->skillValue.pattackspeed != sse.value)
				{
					pEntry->reSendOther = true;
				}
				pEntry->skillValue.pattackspeed = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->reSendOther = true;
				pEntry->skillValue.pattackspeed = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态132 易容术
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_132(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	return SKILL_RECOVERY;
}

/**
 * \brief	状态133 取物理攻防
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_133(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	pEntry->isPhysics = true;
	return SKILL_RETURN;
}

/**
 * \brief	状态134 取法术攻防
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_134(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	pEntry->isPhysics = false;
	return SKILL_RETURN;
}

/**
 * \brief	状态135 弓箭类物理攻击力提升固定值(被动)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_135(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 120;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->checkWeapon(109)&&pEntry->checkWeapon(21)&&pEntry->checkWeapon(142))
						pEntry->skillValue.spupdam = sse.value;
					else
						pEntry->skillValue.spupdam = 0;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->checkWeapon(109)&&pEntry->checkWeapon(21)&&pEntry->checkWeapon(142))
					pEntry->skillValue.spupdam = sse.value;
				else
					pEntry->skillValue.spupdam = 0;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.spupdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态136 棍子类物理攻击力提升固定值(被动)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_136(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 120;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->checkWeapon(111))
						pEntry->skillValue.rpupdam = sse.value;
					else
						pEntry->skillValue.rpupdam = 0;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->checkWeapon(111))
					pEntry->skillValue.rpupdam = sse.value;
				else
					pEntry->skillValue.rpupdam = 0;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.rpupdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态137 恐吓
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_137(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 58;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->dread = true;
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->dread = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态138 弓箭类命中率上升
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_138(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 25;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->checkWeapon(109)&&pEntry->checkWeapon(21)&&pEntry->checkWeapon(142))
						pEntry->skillValue.satrating = sse.value;
					else
						pEntry->skillValue.satrating = 0;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->checkWeapon(109)&&pEntry->checkWeapon(21)&&pEntry->checkWeapon(142))
					pEntry->skillValue.satrating = sse.value;
				else
					pEntry->skillValue.satrating = 0;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.satrating = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态139 土灵加强
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_139(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 25;
	pEntry->skillValue.tuling = sse.value;

	return SKILL_RETURN;
}

/**
 * \brief	状态140 骷髅弓手加强
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_140(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 25;
	pEntry->skillValue.kulou = sse.value;

	return SKILL_RETURN;
}

/**
 * \brief	状态141 天兵加强
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_141(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 25;
	pEntry->skillValue.tianbing = sse.value;

	return SKILL_RETURN;
}

/**
 * \brief	状态142 召唤土灵
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_142(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 0;
	pEntry->summonPet(sse.value, Cmd::PET_TYPE_SUMMON, sse.dwTime, (DWORD)(pEntry->skillValue.tuling), "", 0, pEntry->keepPos);
	return SKILL_RETURN;
}

/**
 * \brief	状态143 召唤骷髅弓手
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_143(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 0;

	pEntry->summonPet(sse.value, Cmd::PET_TYPE_SUMMON, sse.dwTime, (DWORD)(pEntry->skillValue.kulou), "", 0, pEntry->keepPos);
	return SKILL_RETURN;
}

/**
 * \brief	状态144 召唤天兵
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_144(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 0;

	pEntry->summonPet(sse.value, Cmd::PET_TYPE_SUMMON, sse.dwTime, (DWORD)(pEntry->skillValue.tianbing), "", 0, pEntry->keepPos);
	return SKILL_RETURN;
}

/**
 * \brief	状态145 增加状态接受者物理攻击力固定值(只有在手上有箭的时候此状态才生效)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_145(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->checkReduce(BOW_ARROW_ITEM_TYPE,1)&&pEntry->checkReduce(BOW2_ARROW_ITEM_TYPE,1))
					{
						pEntry->skillValue.lupdam = sse.value;
					}
					else
					{
						pEntry->skillValue.lupdam = 0;
					}
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.lupdam = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->skillValue.lupdam >0)
					pEntry->reduce(BOW_ARROW_ITEM_TYPE,1);
				    pEntry->reduce(BOW2_ARROW_ITEM_TYPE,1);
				pEntry->skillValue.lupdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态146 增加状态接受者物理攻击力固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_146(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.lupdam = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.lupdam = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.lupdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态147 提升武器的物理攻击力百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_147(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 201;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.weaponupdamp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.weaponupdamp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.weaponupdamp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态148 提升武器的法术攻击力百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_148(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 201;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.weaponumdamp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.weaponumdamp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.weaponumdamp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态149 交换主人与召唤兽的位置
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_149(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 201;
	
	pEntry->exchangeMeAndSummonPet();

	return SKILL_RETURN;
}

/**
 * \brief	状态150 增加状态接受者生命值上限 增加部分为系数乘主人的法力值剩余点数（召唤兽专用状态）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_150(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

//	pEntry->skillValue.maxhpup = sse.value;
	switch(sse.byStep)
	{
		case ACTION_STEP_RELOAD:
			{
				pEntry->skillValue.maxhp=(SWORD)sse.value;
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_DOPASS:
			break;
		case ACTION_STEP_START:
			{
				//if (pEntry->skillValue.uppetdamage >0) break;
				pEntry->skillValue.maxhp = (DWORD)(pEntry->getMasterMana()*sse.value/1000.0f);
				pEntry->changeHP(pEntry->getMaxHP());
				sse.value = (WORD)pEntry->skillValue.maxhp;
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.maxhp = 0;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态151 按比例增加攻击力 增加值等于主人的法术值乘增加系数（召唤兽专用状态）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_151(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

//	pEntry->skillValue.maxhpup = sse.value;
	switch(sse.byStep)
	{
		case ACTION_STEP_RELOAD:
			{
				pEntry->skillValue.uppetdamage = (SWORD)sse.value;
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_DOPASS:
			break;
		case ACTION_STEP_START:
			{
				//if (pEntry->skillValue.uppetdamage >0) break;
				pEntry->skillValue.uppetdamage = (DWORD)(pEntry->getMasterMana()*(sse.value/1000.0f));
				sse.value = (WORD)pEntry->skillValue.uppetdamage;
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.uppetdamage = 0;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态152 增加状态接受者防御力 增加部分为系数乘主人的法力值剩余点数（召唤兽专用状态）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_152(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

//	pEntry->skillValue.maxhpup = sse.value;
	switch(sse.byStep)
	{
		case ACTION_STEP_RELOAD:
			{
				pEntry->skillValue.uppetdefence = (SWORD)sse.value;
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_DOPASS:
			break;
		case ACTION_STEP_START:
			{
				//if (pEntry->skillValue.uppetdefence >0) break;
				pEntry->skillValue.uppetdefence = (DWORD)(pEntry->getMasterMana()*(sse.value/1000.0f));
				sse.value = (WORD)pEntry->skillValue.uppetdefence;
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.uppetdefence = 0;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态153 增加状态接受者生命值上限 增加部分为系数乘主人的法力值剩余点数（召唤兽专用状态）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_153(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

	pEntry->clearMana();
	return SKILL_RETURN;
}

/**
 * \brief	状态154 弓箭类物理攻击力提升百分比(被动)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_154(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 120;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->checkWeapon(109)&&pEntry->checkWeapon(21)&&pEntry->checkWeapon(142))
						pEntry->skillValue.spupdamp = sse.value;
					else
						pEntry->skillValue.spupdamp = 0;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->checkWeapon(109)&&pEntry->checkWeapon(21)&&pEntry->checkWeapon(142))
					pEntry->skillValue.spupdamp = sse.value;
				else
					pEntry->skillValue.spupdamp = 0;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.spupdamp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态155 弓箭类物理攻击力提升固定值(被动)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_155(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 120;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->checkWeapon(109)&&pEntry->checkWeapon(21)&&pEntry->checkWeapon(142))
						pEntry->skillValue.supdam = sse.value;
					else
						pEntry->skillValue.supdam = 0;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->checkWeapon(109)&&pEntry->checkWeapon(21)&&pEntry->checkWeapon(142))
					pEntry->skillValue.supdam = sse.value;
				else
					pEntry->skillValue.supdam = 0;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.supdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态156 (被动)生命值上限提升
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_156(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				pEntry->skillValue.pmaxhp = (SWORD)(sse.value);
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.pmaxhp = 0;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态157 增加人物施放出重击的几率
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_157(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				pEntry->skillValue.bang = (SWORD)(sse.value);
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.bang = 0;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态158 仙术增加状态接受者物理攻击力固定值(长效)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_158(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(0.8f*pUser->pkpreValue.nofivewdmen);
		}
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.theurgy_updam = sse.value+value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.theurgy_updam = sse.value+value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.theurgy_updam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态159 仙术增加角色或目标的物理防御力上升固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_159(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 54;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(0.5f*pUser->pkpreValue.nofivewdmen);
		}
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.theurgy_updef = sse.value+value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.theurgy_updef = sse.value+value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.theurgy_updef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态160 仙术增加角色或目标的法术防御力上升固定数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_160(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(0.5f*pUser->pkpreValue.nofivewdmen);
		}
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.theurgy_umdef = sse.value+value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.theurgy_umdef = sse.value+value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.theurgy_umdef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态161 对目标产生额外伤害,伤害值取攻击者生命值的X%
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_161(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 63;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					SceneEntryPk *pAtt = NULL;
					switch(sse.attacktype)
					{
						case zSceneEntry::SceneEntry_Player:	// 玩家角色
							{
								pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
							}
							break;
						case zSceneEntry::SceneEntry_NPC:		// NPC
							{
								pAtt = SceneNpcManager::getMe().getNpcByTempID(sse.dwTempID);
							}
							break;
						default:
							break;
					}
					if (!pAtt) return SKILL_RETURN;
					pEntry->skillValue.appenddam += (SWORD)(pAtt->getMaxHP()*(sse.value/100.0f));
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					case zSceneEntry::SceneEntry_NPC:		// NPC
						{
							pAtt = SceneNpcManager::getMe().getNpcByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}

				if (pAtt)
				{
					/*SWORD wdHP = */pEntry->directDamage(pAtt,pEntry->skillValue.appenddam, true);
					//ScenePk::attackRTCmdToNine(rev , pAtt , this , wdHP , 0);
					pEntry->processDeath(pAtt);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态163 去掉玩家武器攻击力
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_163(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 60;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->dropweapon = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->dropweapon = true;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->dropweapon = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态165 被动-召唤兽受到的伤害降低
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_165(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->summon)
					{
						pEntry->summon->dwReduceDam = sse.value;
						return SKILL_RECOVERY;
					}
					else
						return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->summon)
				{
					pEntry->summon->dwReduceDam = sse.value;
					return SKILL_RECOVERY;
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
					if (pEntry->summon)
					{
						pEntry->summon->dwReduceDam = 0;
						return SKILL_RECOVERY;
					}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态168 召唤兽重生
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_168(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.summonrelive = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.summonrelive = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.summonrelive = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态169 仙术增加状态接受者物理攻击力固定值(长效)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_169(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(0.8f*pUser->pkpreValue.nofivewdmen);
		}
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.theurgy_umdam = sse.value+value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.theurgy_umdam = sse.value+value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.theurgy_umdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态170 (被动)生命值上限提升
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_170(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				pEntry->skillValue.pmaxhp = (SWORD)(sse.value);
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.pmaxhp = (SWORD)(sse.value);
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.pmaxhp = 0;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态172 侦察陷阱
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_172(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;

	SceneUser *pUser = NULL;
	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(1*pUser->charstate.wdMen);
		}
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->watchTrap = true;
					if (ACTION_STEP_START == sse.byStep) sse.dwTime += value;
					if (pUser) pUser->sendNineToMe();
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->watchTrap = true;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->watchTrap = false;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态173 召唤融和
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_173(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_RELOAD:
			{
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
			{
				if (pEntry->summon && ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10)))
				{
					pEntry->skillValue.introject_maxmdam = (DWORD)(pEntry->summon->getMaxMDamage()*sse.value/100.0f); // 召唤合体增加魔法攻击
					pEntry->skillValue.introject_maxpdam = (DWORD)(pEntry->summon->getMaxPDamage()*sse.value/100.0f); // 召唤合体增加物理攻击
					pEntry->skillValue.introject_mdam	 = (DWORD)(pEntry->summon->getMinMDamage()*sse.value/100.0f); // 召唤合体增加魔法攻击
					pEntry->skillValue.introject_pdam	 = (DWORD)(pEntry->summon->getMinPDamage()*sse.value/100.0f); // 召唤合体增加物理攻击
					pEntry->skillValue.introject_mdef	 = (DWORD)((pEntry->summon->getMaxMDefence()*sse.value/100.0f)*0.5f); // 召唤合体增加魔法防御
					pEntry->skillValue.introject_pdef	 = (DWORD)((pEntry->summon->getMaxPDefence()*sse.value/100.0f)*0.5f); // 召唤合体增加物理防御
					pEntry->skillValue.introject_maxhp	 = (DWORD)((pEntry->summon->getMaxHP()*sse.value/100.0f)); // 召唤合体增加物理防御
					pEntry->killSummon();
					if (pEntry->getType() == zSceneEntry::SceneEntry_Player)
					{
						SceneUser *pUser = (SceneUser *)pEntry;
						pUser->setupCharBase();
						pUser->changeMP(pUser->getMaxMP());
						pUser->changeHP(pUser->getMaxHP());						
					}
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.introject_maxmdam = 0;
				pEntry->skillValue.introject_maxpdam = 0;
				pEntry->skillValue.introject_mdam	 = 0;
				pEntry->skillValue.introject_pdam	 = 0;
				pEntry->skillValue.introject_mdef	 = 0;
				pEntry->skillValue.introject_pdef	 = 0;
				pEntry->skillValue.introject_maxhp   = 0;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}


/**
 * \brief	状态176 反弹一个灼热状态给攻击自己的玩家
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_176(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.reflect_ardor = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.reflect_ardor = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.reflect_ardor = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态177 训扑野兽
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_177(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}
				if (!pAtt) return SKILL_RETURN;

				int percent=0;
				percent = (pAtt->getLevel() - pEntry->getLevel() +1)*sse.percent;
				if (percent<0) percent =0;
				if (percent>100) percent =100;
				if(zMisc::selectByPercent(percent))
				{
					SceneUser *pUser = (SceneUser *)pAtt;
					if (pEntry->getType() == zSceneEntry::SceneEntry_NPC)
					{
						pUser->captureIt((SceneNpc *)pEntry, sse.value);
					}
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态178 复活宠物
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_178(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->relivePet();
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态179 吸引训练(宠物被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_179(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->pet) pEntry->pet->catchme = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->pet) pEntry->pet->catchme = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->pet) pEntry->pet->catchme = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态180 宠物增强(宠物被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_180(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->pet && (pEntry->pet->boostupPet==0) && (pEntry->pet->npc->bear_type==0))
				{
					pEntry->pet->boostupPet = (int)sse.value;
					pEntry->pet->sendData();
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->pet&&pEntry->pet->npc->bear_type==0) pEntry->pet->boostupPet = (int)sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->pet&&pEntry->pet->npc->bear_type==0)
				{
					pEntry->pet->boostupPet = 0;
					pEntry->pet->sendData();
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态182 胡言乱语，使玩家说话全为乱码
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_182(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 60;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(0.2f*pUser->charstate.wdMen);
		}
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->tosh = true;
					if (ACTION_STEP_START == sse.byStep) sse.dwTime+=value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->tosh = true;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->tosh = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态184 战士生命按级别增加（被动需要武器）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_184(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->needWeapon(sse.dwSkillID))
						pEntry->skillValue.hpupbylevel = sse.value;
					else
						pEntry->skillValue.hpupbylevel = 0;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->needWeapon(sse.dwSkillID))
					pEntry->skillValue.hpupbylevel = sse.value;
				else
					pEntry->skillValue.hpupbylevel = 0;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.hpupbylevel = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态185 反弹一个中毒状态给攻击自己的玩家
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_185(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.reflect_poison = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.reflect_poison = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.reflect_poison = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态186 增加状态接受者物理攻击力百分比(仙术某技能专用)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_186(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 201;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.theurgy_umdamp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.theurgy_umdamp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.theurgy_umdamp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态187 将角色物理攻击力转换成物理防御力
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_187(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 57;
	pEntry->reSendData = true;

    WORD cevalue = sse.value;
    //最大物理攻击力
    DWORD SKMaxPDamage = 0;

	SKMaxPDamage += pEntry->pkpreValue.nofivemaxdam;

    if(cevalue > 100)
	{
        cevalue = 100;
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.pdamtodef = (DWORD)(SKMaxPDamage * (sse.value/100.0f));
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.pdamtodef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态188 将角色扣除法术攻击力增强法术防御力
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_188(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 57;
	pEntry->reSendData = true;

    WORD cevalue = sse.value;
   //最大法术攻击力
    DWORD SKMaxMDamage = 0;

	SKMaxMDamage += pEntry->pkpreValue.nofivemaxmdam;

    if(cevalue > 100)
	{
        cevalue = 100;
	}


	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.mdamtodef = (DWORD)(SKMaxMDamage * (cevalue/100.0f));
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.mdamtodef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

//-----------

/**
 * \brief	状态189 将角色扣除物理防御力转成物理攻击力
 * \param pEntry 状态所有者
 * \param sse 技能状态189
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_189(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 57;
	pEntry->reSendData = true;

    WORD cevalue = sse.value;
    //物理防御力
    DWORD SKPDefence = 0;

	SKPDefence += pEntry->pkpreValue.nofivedef;

    if(cevalue > 100)
	{
        cevalue = 100;
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.pdeftodam = (DWORD)(SKPDefence *(cevalue/100.0f));
					//Zebra::logger->debug("SkillStatus_189状态对象角色[%s] 状态总数值: %d [获取当前物理防御:%d 百分比数值:%d]",pEntry->name,pEntry->skillValue.pdeftodam,SKPDefence,cevalue);
					return SKILL_RECOVERY;
				
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.pdeftodam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态190 将角色扣除法术防御力转成法术攻击力
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_190(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 57;
	pEntry->reSendData = true;

    WORD cevalue = sse.value;
    //魔法防御力
    DWORD SKMDefence = 0;

	SKMDefence += pEntry->pkpreValue.nofivemdef;
    if(cevalue > 100)
	{
        cevalue = 100;
	}
	
	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
	            if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.mdeftodam = (DWORD)(SKMDefence *(cevalue/100.0f));
					//Zebra::logger->debug("SkillStatus_190状态对象角色[%s] [1]状态总数值: %d [获取当前魔法防御:%d 百分比数值:%d]",pEntry->name,pEntry->skillValue.mdeftodam,SKMDefence,cevalue);
					return SKILL_RECOVERY;
					
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.mdeftodam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态191 增加宠物移动速度
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_191(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->pet)
						pEntry->pet->setSpeedRate(640.0f/((640-sse.value)<=0?1:(640-sse.value)));
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->pet)
					pEntry->pet->setSpeedRate(640.0f/((640-sse.value)<=0?1:(640-sse.value)));
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->pet)
					pEntry->pet->resetSpeedRate();
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态192 每秒释放一个技能伤害周围5*5的范围
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_192(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				Cmd::stAttackMagicUserCmd cmd;

				switch (pEntry->getType())
				{
					case zSceneEntry::SceneEntry_Player:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					case zSceneEntry::SceneEntry_NPC:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					default:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
				}
		
				cmd.dwDefenceTempID = 0;
				cmd.dwUserTempID = pEntry->tempid;
				cmd.wdMagicType = sse.value;
				cmd.byAction = Cmd::Ani_Null;//Ani_Num;
				cmd.byDirect = pEntry->getDir();

				zSkill *s = NULL;

				s = zSkill::createTempSkill(pEntry , sse.value , 1);
				if(s)
				{
					s->action(&cmd, sizeof(cmd));
					SAFE_DELETE(s);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态193 状态结束时伤害状态接受者极其周围
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_193(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				Cmd::stAttackMagicUserCmd cmd;

				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}
				if (!pAtt) return SKILL_RETURN;

				switch (pEntry->getType())
				{
					case zSceneEntry::SceneEntry_Player:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2U;
						}
						break;
					case zSceneEntry::SceneEntry_NPC:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_N2U; //路要求的
						}
						break;
					default:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2U;
						}
						break;
				}
		
				cmd.dwDefenceTempID = pEntry->tempid;
				cmd.dwUserTempID = pAtt->tempid;
				cmd.wdMagicType = 291;
				ScenePk::attackUserCmdToNine(&cmd , pEntry);
				if (pAtt)
				{
					pEntry->directDamage(pAtt, sse.value, true);
					pEntry->processDeath(pAtt);
				}
				return SKILL_RETURN;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态194 增强(召唤兽被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_194(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) 
				{
					pEntry->summon->boostupSummon = (int)sse.value;
					pEntry->summon->sendData();
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) pEntry->summon->boostupSummon = (int)sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->summon &&((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10)))
				{
					pEntry->summon->boostupSummon = 0;
					pEntry->summon->sendData();
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态195 增强(召唤兽被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_195(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) 
				{
					pEntry->summon->boostupSummon = (int)sse.value;
					pEntry->summon->sendData();
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) pEntry->summon->boostupSummon = (int)sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->summon &&((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10)))
				{
					pEntry->summon->boostupSummon = 0;
					pEntry->summon->sendData();
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态196 增强(召唤兽被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_196(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) 
				{
					pEntry->summon->boostupSummon = (int)sse.value;
					pEntry->summon->sendData();
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) pEntry->summon->boostupSummon = (int)sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->summon &&((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10)))
				{
					pEntry->summon->boostupSummon = 0;
					pEntry->summon->sendData();
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态197 增强(召唤兽被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_197(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) 
				{
					pEntry->summon->boostupSummon = (int)sse.value;
					pEntry->summon->sendData();
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) pEntry->summon->boostupSummon = (int)sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->summon &&((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10)))
				{
					pEntry->summon->boostupSummon = 0;
					pEntry->summon->sendData();
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态198 增强(召唤兽被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_198(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) 
				{
					pEntry->summon->boostupSummon = (int)sse.value;
					pEntry->summon->sendData();
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) pEntry->summon->boostupSummon = (int)sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->summon &&((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10)))
				{
					pEntry->summon->boostupSummon = 0;
					pEntry->summon->sendData();
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态199 增强(召唤兽被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_199(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) 
				{
					pEntry->summon->boostupSummon = (int)sse.value;
					pEntry->summon->sendData();
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) pEntry->summon->boostupSummon = (int)sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->summon &&((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10)))
				{
					pEntry->summon->boostupSummon = 0;
					pEntry->summon->sendData();
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态200 增强(召唤兽被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_200(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) 
				{
					pEntry->summon->boostupSummon = (int)sse.value;
					pEntry->summon->sendData();
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) pEntry->summon->boostupSummon = (int)sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->summon &&((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10)))
				{
					pEntry->summon->boostupSummon = 0;
					pEntry->summon->sendData();
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态201 增强(召唤兽被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_201(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) 
				{
					pEntry->summon->boostupSummon = (int)sse.value;
					pEntry->summon->sendData();
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) pEntry->summon->boostupSummon = (int)sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->summon &&((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10)))
				{
					pEntry->summon->boostupSummon = 0;
					pEntry->summon->sendData();
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态202 增强(召唤兽被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_202(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) 
				{
					pEntry->summon->boostupSummon = (int)sse.value;
					pEntry->summon->sendData();
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) pEntry->summon->boostupSummon = (int)sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->summon &&((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10)))
				{
					pEntry->summon->boostupSummon = 0;
					pEntry->summon->sendData();
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态203 仙术增加角色或目标的物理防御力上升百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_203(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 54;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(0.02f*pUser->charstate.wdMen);
		}
	}
	value+=sse.value;
	if (value >100) value=100;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.theurgy_updefp = value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.theurgy_updefp = value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.theurgy_updefp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态204 反弹一个麻痹状态给攻击自己的玩家
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_204(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.reflect_lull = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.reflect_lull = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.reflect_lull = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

//--家族--------------------------------------------

/**
 * \brief	状态205 增加状态接受者物理攻击力百分比（家族）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_205(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 201;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.sept_updamp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.sept_updamp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.sept_updamp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态206 增加状态接受者法术攻击力百分比（家族）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_206(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 211;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.sept_umdamp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.sept_umdamp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.sept_umdamp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态207 角色或目标的物理防御力上升百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_207(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 54;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{

					pEntry->skillValue.sept_updefp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.sept_updefp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.sept_updefp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态208 法术防御上升百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_208(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.sept_umdefp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.sept_umdefp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.sept_umdefp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态209 增加状态接受者生命值上限百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_209(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.sept_maxhp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.sept_maxhp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.sept_maxhp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态210 增加状态接受者法术值上限百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_210(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 23;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				pEntry->skillValue.sept_maxmp = (SWORD)(pEntry->getBaseMaxMP()*(sse.value/100.0f));
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.sept_maxmp =0;
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态216 召唤图腾
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_216(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 0;

	pEntry->summonPet(sse.value, Cmd::PET_TYPE_TOTEM, sse.dwTime, 0, "", 0, pEntry->keepPos, pEntry->keepDir);
	return SKILL_RETURN;
}

/**
 * \brief	状态217 每次攻击的时候附加法术攻击（两仪心法）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_217(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 120;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.magicattack = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.magicattack = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态218 转移施法者法术值百分比给状态接受者
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_218(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 31;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}
				if (!pAtt) return SKILL_RETURN;
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					SDWORD vchange = (SDWORD)(pAtt->getMP()*sse.value/100.0f);
					pAtt->changeMP(0-vchange);
					pEntry->changeMP(vchange);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态219 每秒释放一个技能让一屏范围内的友军防御提升
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_219(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				Cmd::stAttackMagicUserCmd cmd;

				switch (pEntry->getType())
				{
					case zSceneEntry::SceneEntry_Player:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					case zSceneEntry::SceneEntry_NPC:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					default:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
				}
		
				cmd.dwDefenceTempID = 0;
				cmd.dwUserTempID = pEntry->tempid;
				cmd.wdMagicType = sse.value;
				cmd.byAction = Cmd::Ani_Num;
				cmd.byDirect = pEntry->getDir();
				cmd.xDes = (WORD)pEntry->getPos().x;
				cmd.yDes = (WORD)pEntry->getPos().y;

				zSkill *s = NULL;

				s = zSkill::createTempSkill(pEntry , sse.value , 1);
				if(s)
				{
					s->action(&cmd, sizeof(cmd));
					SAFE_DELETE(s);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态220 瞬间移动到鼠标指定位置
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_220(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 67;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->jumpTo(pEntry->keepPos);
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态221 增加任务精神属性百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_221(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 67;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.addmenp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.addmenp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态222 献身状态，状态接受者所受到的伤害全部转移并翻倍施加到状态施放者身上
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_222(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 67;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->swapdamcharid = sse.dwTempID;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->swapdamcharid = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态223 提高法术之火焰系技能的伤害
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_223(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.pblazeappend = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.pblazeappend = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态224 被动，宠物攻击的时候有机会让目标眩晕
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_224(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->pet)
					{
						pEntry->pet->giddy = sse.value;
					}
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->pet)
				{
					pEntry->pet->giddy = 0;
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态225 反弹一个灼热状态给攻击自己的玩家
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_225(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.reflect_frost = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.reflect_frost = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.reflect_frost = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态226 反弹一个灼热状态给攻击自己的玩家
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_226(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->mhpd = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->mhpd = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态227 角色或目标的物理防御力降低固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_227(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 54;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.theurgy_dpdef = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.theurgy_dpdef = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.theurgy_dpdef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态228 角色或目标的法术防御力下降固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_228(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 54;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.theurgy_dmdef = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.theurgy_dmdef = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.theurgy_dmdef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态229 仙术的防御光环状态，物理和法术防御力提升固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_229(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.udef = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.udef = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.udef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态230 鹤翼阵每秒释放一个技能让组队队员享受状态加成
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_230(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->getType() == zSceneEntry::SceneEntry_Player)
					{
						SceneUser *pUser = (SceneUser *)pEntry;
						if (pUser->team.IsTeamed() && pUser->team.getLeader() == pUser->tempid)
							return SKILL_ACTIVE;
					}
					return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				Cmd::stAttackMagicUserCmd cmd;

				if (pEntry->getType() == zSceneEntry::SceneEntry_Player)
				{
					SceneUser *pUser = (SceneUser *)pEntry;
					if (!pUser->team.IsTeamed())
					{
						sse.dwTime = 0;
						return SKILL_RETURN;
					}
					else
					{
						if (!pUser->team.canPutSkill())
						{
							sse.dwTime = 0;
							return SKILL_RETURN;
						}
					}

					sse.dwTime = 100;

					cmd.byAttackType = Cmd::ATTACKTYPE_U2P;

					cmd.dwDefenceTempID = 0;
					cmd.dwUserTempID = pEntry->tempid;
					cmd.wdMagicType = sse.value;
					cmd.byAction = Cmd::Ani_Num;
					cmd.byDirect = pEntry->getDir();

					zSkill *s = NULL;

					s = zSkill::createTempSkill(pEntry , sse.value , 1);
					if(s)
					{
						s->action(&cmd, sizeof(cmd));
						SAFE_DELETE(s);
					}
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态231 鹤翼阵法专用状态提升双攻
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_231(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->getType() == zSceneEntry::SceneEntry_Player)
					{
						SceneUser *pUser = (SceneUser *)pEntry;
						int num = pUser->team.getSize();
						if (num>=2) num-=2;
						else num=0;
						pEntry->skillValue.array_udamp = sse.value+num*3;
						return SKILL_RECOVERY;
					}
					else
						return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.array_udamp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态232 鹤翼阵法专用状态降低移动速度
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_232(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 9;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.array_dmvspeed = sse.value;

					//xwl add
					if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
						((SceneNpc *)pEntry)->resetSpeedRate();
					//pEntry->reSendMyMapData();
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.array_dmvspeed = 0;

				//xwl add
				if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
					((SceneNpc *)pEntry)->resetSpeedRate();

				//pEntry->reSendMyMapData();
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态233 鹤翼阵法专用状态降低双防
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_233(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.array_ddefp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.array_ddefp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态234 人物体质增加数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_234(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.upcon = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.upcon = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态235 易容状态，
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_235(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->changeface = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->changeface = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态236 提升弓箭攻击力，必须是在持有弓的情况下
	由于是短效持续投送的技能状态所以采用这里检查武器在加成的地方直接加成的方式。
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_236(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->checkWeapon(109)&&pEntry->checkWeapon(21))
					{
						pEntry->skillValue.sword_udam = sse.value;
						return SKILL_RECOVERY;
					}
					return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.sword_udam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态237 复活
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_237(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->getType() == zSceneEntry::SceneEntry_Player)
					{
						SceneUser *pUser = (SceneUser *)pEntry;
						pUser->relive(Cmd::ReliveSkill, 0, sse.value);
					}
					return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态238 流阵每秒释放一个技能让组队队员享受状态加成
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_238(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->getType() == zSceneEntry::SceneEntry_Player)
					{
						SceneUser *pUser = (SceneUser *)pEntry;
						if (pUser->team.IsTeamed() && pUser->team.getLeader() == pUser->tempid && pUser->team.getSize()>=3)
							return SKILL_ACTIVE;
					}
					return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				Cmd::stAttackMagicUserCmd cmd;

				if (pEntry->getType() == zSceneEntry::SceneEntry_Player)
				{
					SceneUser *pUser = (SceneUser *)pEntry;
					if (!(pUser->team.IsTeamed() && pUser->team.getSize()>=3))
					{
						sse.dwTime = 0;
						return SKILL_RETURN;
					}
					else
					{
						pUser->changeMP(-7);
						if (!(pUser->team.canPutSkill() && pUser->getMP()>=7))
						{
							sse.dwTime = 0;
							return SKILL_RETURN;
						}
					}

					sse.dwTime = 100;

					cmd.byAttackType = Cmd::ATTACKTYPE_U2P;

					cmd.dwDefenceTempID = 0;
					cmd.dwUserTempID = pEntry->tempid;
					cmd.wdMagicType = sse.value;
					cmd.byAction = Cmd::Ani_Num;
					cmd.byDirect = pEntry->getDir();

					zSkill *s = NULL;

					s = zSkill::createTempSkill(pEntry , sse.value , 1);
					if(s)
					{
						s->action(&cmd, sizeof(cmd));
						SAFE_DELETE(s);
					}
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态239 流云阵法专用状态降低移动速度
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_239(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 9;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					SceneEntryPk *pAtt = NULL;
					switch(sse.attacktype)
					{
						case zSceneEntry::SceneEntry_Player:	// 玩家角色
							{
								pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
							}
							break;
						default:
							break;
					}
					if (pAtt)
					{
						SceneUser *pUser = (SceneUser *)pAtt;
						int num = pUser->team.getSize();
						if (num>=3) num-=3;
						else num=0;
						pEntry->skillValue.array_dmvspeed = sse.value+num*5;
	
						//xwl add
						if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
							((SceneNpc *)pEntry)->resetSpeedRate();
						//pEntry->reSendMyMapData();
						return SKILL_RECOVERY;
					}
					else
						return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.array_dmvspeed = 0;

				//xwl add
				if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
					((SceneNpc *)pEntry)->resetSpeedRate();

				//pEntry->reSendMyMapData();
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态240 狂乱诅咒
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_240(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态241 隐身状态
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_241(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->hideme = true;
					pEntry->skillAction = false;
					pEntry->attackAction = false;
					if (pEntry->getType() == zSceneEntry::SceneEntry_Player)
					{
						Cmd::stRemoveUserMapScreenUserCmd remove;
						remove.dwUserTempID=pEntry->tempid;
						((SceneUser *)pEntry)->scene->sendCmdToNineUnWatch(pEntry->getPosI(),&remove,sizeof(remove));
					}
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->hideme = false;
				pEntry->skillAction = true;
				pEntry->attackAction = true;
				if (pEntry->getType() == zSceneEntry::SceneEntry_Player)
				{
					//FunctionTimes times(39,__FUNCTION__);
					((SceneUser *)pEntry)->sendMeToNine();
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态242 有几率从对敌人的伤害上获取生命值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_242(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.attackaddhpnum = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.attackaddhpnum = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态243 有几率闪避敌人的攻击
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_243(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dodge = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.dodge = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态244 反弹一个麻痹状态给攻击自己的玩家
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_244(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.reflect_icelull = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.reflect_icelull = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.reflect_icelull = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态245 冰灵柜状态
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_245(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->icebox = true;
					pEntry->moveAction = false;
					pEntry->attackAction = false;
					pEntry->skillAction = false;
					pEntry->useRes = false;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->icebox = false;
				pEntry->moveAction = true;
				pEntry->attackAction = true;
				pEntry->skillAction = true;
				pEntry->useRes = true;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态246 （仙）将角色或目标的物理攻击力降低固定数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_246(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 56;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(1*pUser->charstate.wdMen);
		}
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.theurgy_dpdam = sse.value+value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.theurgy_dpdam = sse.value+value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.theurgy_dpdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态247 将角色或目标的法术攻击力降低固定数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_247(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 57;
	pEntry->reSendData = true;

	WORD value=0;
	if (sse.attacktype == zSceneEntry::SceneEntry_Player)
	{
		SceneUser *pUser = NULL;
		pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
		if (pUser)
		{
			value= (WORD)(1*pUser->charstate.wdMen);
		}
	}

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.theurgy_dmdam = sse.value+value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.theurgy_dmdam = sse.value+value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.theurgy_dmdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态248 液化状态
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_248(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->liquidState = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->liquidState = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态249 状态结束的时候投送技能,以状态投送者的身份投送技能。
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_249(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
			}
			break;
		case ACTION_STEP_STOP:
			{
				Cmd::stAttackMagicUserCmd cmd;

				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					case zSceneEntry::SceneEntry_NPC:		// NPC
						{
							pAtt = SceneNpcManager::getMe().getNpcByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}

				if (pAtt)
				{
					switch (pAtt->getType())
					{
						case zSceneEntry::SceneEntry_Player:
							{
								cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
							}
							break;
						case zSceneEntry::SceneEntry_NPC:
							{
								cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
							}
							break;
						default:
							{
								cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
							}
							break;
					}
		
					cmd.dwDefenceTempID = pEntry->tempid;
					cmd.dwUserTempID = pAtt->tempid;
					cmd.wdMagicType = sse.value;
					cmd.byAction = Cmd::Ani_Num;
					cmd.byDirect = pAtt->getDir();
					cmd.xDes = (WORD)pEntry->getPos().x;
					cmd.yDes = (WORD)pEntry->getPos().y;

					zSkill *s = NULL;

					s = zSkill::createTempSkill(pAtt , sse.value , 1);
					if(s)
					{
						s->action(&cmd, sizeof(cmd));
						SAFE_DELETE(s);
					}
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_CLEAR:
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态250 状态结束的时候投送技能,以状态投送者的身份投送技能。
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_250(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->bombskillId = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->bombskillId = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态251 人物智力增加数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_251(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.upint = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.upint = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态252 被动增加状态接受者法术攻击力百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_252(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 211;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.pumdamp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.pumdamp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.pumdamp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态253 扣除法力值固定值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_253(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 211;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					case zSceneEntry::SceneEntry_NPC:		// NPC
						{
							pAtt = SceneNpcManager::getMe().getNpcByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}
				if (pAtt)
				{
					SDWORD bkMp= pEntry->getMP();
					pEntry->changeMP(0-sse.value);
					bkMp-=pEntry->getMP();
					if (bkMp<0)bkMp=0;
					SDWORD dam = (SDWORD)(bkMp*(sse.percent/100.0f));
					pEntry->directDamage(pAtt, dam,true);
					pEntry->processDeath(pAtt);

					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态254 灵兽增强(灵兽被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_254(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->pet && (pEntry->pet->boostupPet==0) && (pEntry->pet->npc->bear_type==2))
				{
					pEntry->pet->boostupPet = (int)sse.value;
					pEntry->pet->sendData();
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->pet&&pEntry->pet->npc->bear_type==2) pEntry->pet->boostupPet = (int)sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->pet&&pEntry->pet->npc->bear_type==2)
				{
					pEntry->pet->boostupPet = 0;
					pEntry->pet->sendData();
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态255 献祭状态，杀死宠物将宠物生命值的百分之X加到自己身上
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_255(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->pet)
				{
					pEntry->changeHP((SDWORD)(pEntry->pet->hp*(sse.value/100.0f)));
					pEntry->pet->toDie(pEntry->tempid);
					pEntry->killOnePet(pEntry->pet);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态256 提高法术之雷电系技能的伤害
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_256(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.levinappend = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.levinappend = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态257 召唤召唤兽
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_257(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 0;

	zNpcB *base = npcbm.get(sse.value);
	if (base)
	{
		t_NpcDefine define;
		zPos pos = pEntry->getPos();
		define.id = base->id;
		strcpy(define.name, base->name);
		define.pos = pEntry->getPos();
		define.width = SCREEN_WIDTH;
		define.height = SCREEN_HEIGHT;
		define.num = 1;
		define.interval = 30;
		define.initstate = zSceneEntry::SceneEntry_Normal;
		//define.scriptID = atoi(prop["script"].c_str());

		pEntry->scene->zPosRevaluate(define.pos);
		define.pos -= zPosHalfScreen;
		pEntry->scene->zPosRevaluate(define.pos);
		pos -= zPosHalfScreen;
		pEntry->scene->zPosRevaluate(pos);
		pEntry->scene->initRegion(define.region, define.pos, define.width, define.height);
		pEntry->scene->summonNpc(define, pos, base);
	}
	return SKILL_RETURN;
}

/**
 * \brief	状态258 人物敏捷增加数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_258(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.updex = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.updex = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态259 有几率忽略一次伤害状态
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_259(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->resist = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->resist = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态260 吸魔剑法专用状态
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_260(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->dmpbyhp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->dmpbyhp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态261 宠物法术防御增强(宠物被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_261(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->pet && (pEntry->pet->boostupPetMDef==0))
				{
					pEntry->pet->boostupPetMDef = (int)sse.value;
					pEntry->pet->sendData();
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->pet) pEntry->pet->boostupPetMDef = (int)sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->pet)
				{
					pEntry->pet->boostupPetMDef = 0;
					pEntry->pet->sendData();
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态262 冰冻结界专用状态
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_262(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->icedam ==0) pEntry->icedam = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->icedam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态263 保护状态（宠物专用）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_263(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				pEntry->skillValue.protectdam = (WORD)(pEntry->getMaxHP()*(sse.percent/100.0f));
				pEntry->skillValue.protectUpAtt = sse.value;
				return SKILL_ACTIVE;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					case zSceneEntry::SceneEntry_NPC:		// NPC
						{
							pAtt = SceneNpcManager::getMe().getNpcByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}

				if (pAtt)
				{
					pEntry->directDamage(pAtt, pEntry->skillValue.protectdam,true);
					pEntry->processDeath(pAtt);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.protectdam = 0;
				pEntry->skillValue.protectUpAtt = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态264 一元阵每秒释放一个技能让组队队员加成状态
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_264(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->getType() == zSceneEntry::SceneEntry_Player)
					{
						SceneUser *pUser = (SceneUser *)pEntry;
						if (pUser->team.IsTeamed() && pUser->team.getLeader() == pUser->tempid && pUser->team.getSize()>=2)
						{
							zRTime ctv;
							pUser->unitaryBattleKeepTime = ctv.msecs()+sse.dwTime*1000;
							return SKILL_ACTIVE;
						}
					}
					return SKILL_RETURN;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				Cmd::stAttackMagicUserCmd cmd;

				if (pEntry->getType() == zSceneEntry::SceneEntry_Player)
				{
					SceneUser *pUser = (SceneUser *)pEntry;
					if (!(pUser->team.IsTeamed() && pUser->team.getSize()>=2 && (SceneTimeTick::currentTime.msecs() <pEntry->unitaryBattleKeepTime)))
					{
						sse.dwTime = 0;
						return SKILL_RETURN;
					}
//					else
//					{
//						if (!(pUser->team.canPutSkill()))
//						{
//							sse.dwTime = 0;
//							return SKILL_RETURN;
//						}
//					}

//					sse.dwTime = 100;

					cmd.byAttackType = Cmd::ATTACKTYPE_U2P;

					cmd.dwDefenceTempID = 0;
					cmd.dwUserTempID = pEntry->tempid;
					cmd.wdMagicType = sse.value;
					cmd.byAction = Cmd::Ani_Num;
					cmd.byDirect = pEntry->getDir();

					zSkill *s = NULL;

					s = zSkill::createTempSkill(pEntry , sse.value , 1);
					if(s)
					{
						s->action(&cmd, sizeof(cmd));
						SAFE_DELETE(s);
					}
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
//				pEntry->skillValue.unitarybattle = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态265 一元阵法投送到队长攻击的敌人身上，组队队员攻击的时候这个状态会带来而外的伤害
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_265(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				pEntry->skillValue.teamappend = sse.dwTempID;
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			{
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.teamappend = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态266 一元阵法投送一屏范围内的组队队员身上
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_266(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->getType() == zSceneEntry::SceneEntry_Player)
				{
					SceneUser *pUser = (SceneUser *)pEntry;
					if (pUser->team.IsTeamed() && pUser->team.getLeader() == pUser->tempid)
						pEntry->skillValue.unitarybattle = 696; //本人在攻击的时候会投送一个组队伤害状态给怪物。
				}
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			{
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.unitarybattle = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态268 将法术伤害转换成魔法值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_268(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				pEntry->mdmtompper = sse.percent;
				pEntry->mdmtomp = sse.value;
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->mdmtompper=0;
				pEntry->mdmtomp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态270 关爱状态，状态接受者所受到的伤害全部转移施加到状态施放者身上
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_270(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 67;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->swapdamcharid2 = sse.dwTempID;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->swapdamcharid2 = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态271 提高射术之陷阱系技能的伤害
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_271(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.iceappend = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.iceappend = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态272 提高射术之陷阱系技能的伤害
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_272(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.trapappend = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.trapappend = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态273 增加状态接受者物理攻击力百分比(某技能专用)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_273(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 201;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.nsc_updamp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.nsc_updamp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.nsc_updamp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态274 角色或目标的物理防御力上升固定值（铁骨之魂）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_274(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 54;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.tgzh_updef = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.tgzh_updef = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.tgzh_updef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态275 回光返照技能
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_275(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 201;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->ignoreDam = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->ignoreDam = true;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->ignoreDam = false;
				pEntry->toDie(pEntry->tempid);
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态276 角色或目标的物理防御力上升百分比（铁骨御体）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_276(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 54;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.tgyt_updefp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.tgyt_updefp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.tgyt_updefp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态277 增加状态接受者物理攻击力百分比(鲁莽技能专用)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_277(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 201;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.lm_updamp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.lm_updamp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.lm_updamp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态278 提高法术之雷电系技能的伤害
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_278(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.plevinappend = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.plevinappend = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态279 宠物自杀攻击
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_279(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->dietodam = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->dietodam=0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态280 提高法术之雷电系技能的伤害
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_280(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
//				if(zMisc::selectByPercent((int)(sse.percent)))
//				{
					pEntry->skillValue.relive = sse.percent;
					return SKILL_RECOVERY;
//				}
//				else
//					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
			{
				pEntry->skillValue.relive = 0;
			}
			break;
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态281 提高法术之雷电系技能的伤害
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_281(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态282 提高法术之火焰系技能的伤害
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_282(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.blazeappend = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.blazeappend = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态283 使角色或目标法术值持续下降,速度与生命值持续降低相当
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_283(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 4;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->ignorePhysicDam = true;
					pEntry->changeMP(0-sse.value);
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->getMP() >= sse.value)
				{
					pEntry->changeMP(0-sse.value);
				}
				else
				{
					pEntry->ignorePhysicDam = false;
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->ignorePhysicDam = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态284 增加状态接受者物理攻击力百分比(仙术某技能专用)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_284(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 201;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.theurgy_updamp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.theurgy_updamp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.theurgy_updamp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态285 法术防御上升百分比(仙术某技能专用)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_285(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.theurgy_umdefp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.theurgy_umdefp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.theurgy_umdefp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态286 神农护佑专用
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_286(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->useRes = false;
					pEntry->moveAction = false;
					pEntry->skillAction = false;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
					pEntry->useRes = true;
					pEntry->moveAction = true;
					pEntry->skillAction = true;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态287 PK值加速降低
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_287(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态288 眩晕（冲锋专用）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_288(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 0;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent(((int)(sse.percent-pEntry->skillValue.sevendownp)) > 0 ? (sse.percent-pEntry->skillValue.sevendownp):0))
				{
					pEntry->attackAction = false;
					pEntry->skillAction = false;
					pEntry->reSendOther = true;
					pEntry->skillValue.movespeed = -1900;

					//xwl add
					if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
						((SceneNpc *)pEntry)->resetSpeedRate();
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->attackAction = false;
				pEntry->skillAction = false;
				if(pEntry->skillValue.movespeed != -1900)
				{
					pEntry->reSendOther = true;
				}
				pEntry->skillValue.movespeed = -1900;

				//xwl add
				if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
					((SceneNpc *)pEntry)->resetSpeedRate();

				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->attackAction = true;
				pEntry->skillAction = true;
				pEntry->reSendOther = true;
				pEntry->skillValue.movespeed = 0;

				//xwl add
				if (pEntry->getType()==zSceneEntry::SceneEntry_NPC)
					((SceneNpc *)pEntry)->resetSpeedRate();

			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态289 提升力量属性百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_289(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.upstr = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.upstr = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态290 提升所有属性值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_290(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.upallattrib = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.upallattrib = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态292 死亡后立即复活
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_292(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->ignoreDam = false; //关闭回光状态
					pEntry->skillValue.nowrelive = 100;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.nowrelive = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态293 所有属性值下降数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_293(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dnallattrib = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.dnallattrib = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态294 所有属性值下降百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_294(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dpallattrib = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.dpallattrib = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态295 随机提升5个属性值中的一个
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_295(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					switch(zMisc::randBetween(1,5))
					{
						case 1:
							pEntry->skillValue.upattribstr = sse.value;
							break;
						case 2:
							pEntry->skillValue.upattribcon = sse.value;
							break;
						case 3:
							pEntry->skillValue.upattribmen = sse.value;
							break;
						case 4:
							pEntry->skillValue.upattribdex = sse.value;
							break;
						case 5:
							pEntry->skillValue.upattribint = sse.value;
							break;
					}
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.upattribstr = 0;
				pEntry->skillValue.upattribcon = 0;
				pEntry->skillValue.upattribmen = 0;
				pEntry->skillValue.upattribdex = 0;
				pEntry->skillValue.upattribint = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态296 攻击力取下限
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_296(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->attacklow = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->attacklow = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态298 攻击力取下限
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_298(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->attacklow = true;
					pEntry->skillValue.enervation = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->attacklow = false;
				pEntry->skillValue.enervation = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态299 物理攻击免疫
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_299(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->ignorePhysicDam = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->ignorePhysicDam = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态300 魔法攻击免疫
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_300(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->ignoreMagicDam = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->ignoreMagicDam = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态301 增强魔兽(召唤兽被动）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_301(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) 
				{
					pEntry->summon->boostupSummon = (int)sse.value;
					pEntry->summon->sendData();
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				if (pEntry->summon&& ((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10))) pEntry->summon->boostupSummon = (int)sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->summon &&((pEntry->summon->id >= (DWORD)sse.percent)&&(pEntry->summon->id < (DWORD)sse.percent+10)))
				{
					pEntry->summon->boostupSummon = 0;
					pEntry->summon->sendData();
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态302 劫火
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_302(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					sse.qwTime = sse.value*sse.dwTime;
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					case zSceneEntry::SceneEntry_NPC:		// NPC
						{
							pAtt = SceneNpcManager::getMe().getNpcByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}

				if (pAtt)
				{
					pEntry->directDamage(pAtt, sse.value, true);
					pEntry->processDeath(pAtt);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					case zSceneEntry::SceneEntry_NPC:		// NPC
						{
							pAtt = SceneNpcManager::getMe().getNpcByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}

				if (pAtt)
				{
					pEntry->directDamage(pAtt, sse.qwTime, true);
					pEntry->processDeath(pAtt);
				}

				Cmd::stAttackMagicUserCmd cmd;
				switch (pEntry->getType())
				{
					case zSceneEntry::SceneEntry_Player:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2U;
						}
						break;
					case zSceneEntry::SceneEntry_NPC:
					default:
						{
							return SKILL_RETURN;
						}
						break;
				}
				cmd.dwDefenceTempID = pEntry->tempid;
				cmd.dwUserTempID = pEntry->tempid;
				cmd.wdMagicType = 33;
				cmd.byAction = Cmd::Ani_Null;

				zSkill *s = NULL;

				s = zSkill::createTempSkill(pEntry , 33 , 1);
				if(s)
				{
					s->action(&cmd, sizeof(cmd));
					SAFE_DELETE(s);
				}
				return SKILL_RETURN;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态303 元素之门
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_303(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->summon)
				{
					switch(pEntry->summon->id)
					{
						case 42221 ... 42230://石灵
						case 42241 ... 42250://风灵
						case 45001 ... 45010://铁灵
						case 45041 ... 45050://焰魔君主
							{
								if ((pEntry->summon->getHp() == pEntry->summon->getMaxHP())&&(pEntry->summon->boostupHpMaxP==0))
								{
								   pEntry->summon->boostupHpMaxP = (int)sse.value;
								   pEntry->summon->changeHP(pEntry->summon->getMaxHP());
								   pEntry->summon->sendData();
								}
								else
								{
								   pEntry->summon->boostupHpMaxP = (int)sse.value;
								   pEntry->summon->sendData();
								}
								return SKILL_RECOVERY;
							}
							break;
					}
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->summon)
				{
					switch(pEntry->summon->id)
					{
						case 42221 ... 42230://石灵
						case 42241 ... 42250://风灵
						case 45001 ... 45010://铁灵
						case 45041 ... 45050://焰魔君主
							{
								pEntry->summon->boostupHpMaxP = 0;
								pEntry->summon->sendData();
								return SKILL_RETURN;
							}
							break;
					}
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态304 冥符之门（加亡灵类的生命值上限)
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_304(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->summon)
				{
					switch(pEntry->summon->id)
					{
						case 42061 ... 42070://骷髅战士
						case 42081 ... 42090://幽魂
						case 44061 ... 44070://骸骨守卫
						case 45081 ... 45090://六道鬼王
							{
								if ((pEntry->summon->getHp() == pEntry->summon->getMaxHP())&&(pEntry->summon->boostupHpMaxP==0))
								{
								   pEntry->summon->boostupHpMaxP = (int)sse.value;
								   pEntry->summon->changeHP(pEntry->summon->getMaxHP());
								   pEntry->summon->sendData();
								}
								else
								{
								   pEntry->summon->boostupHpMaxP = (int)sse.value;
								   pEntry->summon->sendData();
								}
								return SKILL_RECOVERY;
							}
							break;
					}
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->summon)
				{
					switch(pEntry->summon->id)
					{
						case 42061 ... 42070://骷髅战士
						case 42081 ... 42090://幽魂
						case 44061 ... 44070://骸骨守卫
						case 45081 ... 45090://六道鬼王
							{
								pEntry->summon->boostupHpMaxP = 0;
								pEntry->summon->sendData();
								return SKILL_RETURN;
							}
							break;
					}
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态305 天届之门加召唤生命上限（天仙系）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_305(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if (pEntry->summon)
				{
					switch(pEntry->summon->id)
					{
						case 42181 ... 42190://神兵
						case 42161 ... 42170://天将
						case 42121 ... 42130://天仙
						case 46021 ... 46030://三界天王
							{
								if ((pEntry->summon->getHp() == pEntry->summon->getMaxHP())&&(pEntry->summon->boostupHpMaxP==0))
								{
								   pEntry->summon->boostupHpMaxP = (int)sse.value;
								   pEntry->summon->changeHP(pEntry->summon->getMaxHP());
								   pEntry->summon->sendData();
								}
								else
								{
								   pEntry->summon->boostupHpMaxP = (int)sse.value;
								   pEntry->summon->sendData();
								}
								return SKILL_RECOVERY;
							}
							break;
					}
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->summon)
				{
					switch(pEntry->summon->id)
					{
						case 42181 ... 42190://神兵
						case 42161 ... 42170://天将
						case 42121 ... 42130://天仙
						case 46021 ... 46030://三界天王
							{
								pEntry->summon->boostupHpMaxP = 0;
								pEntry->summon->sendData();
								return SKILL_RETURN;
							}
							break;
					}
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态306 有几率使对敌人的伤害变成自己的生命值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_306(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				pEntry->damtohpper = sse.percent;
				pEntry->damtohpvalper = sse.value;
				return SKILL_ACTIVE;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->damtohpper = 0;
				pEntry->damtohpvalper = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态307 降低敌人的智力和精神百分比（屠魔陷阱专用）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_307(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dpintmen = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.dpintmen = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态308 降低敌人的力量和敏捷百分比（屠魔陷阱专用）
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_308(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dpstrdex = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.dpstrdex = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态309 一击必杀
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_309(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->killnow = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->killnow = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态311 地灭，随机抽取同屏的人来承担死亡效果。
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_311(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->switchdie = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->switchdie = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态312 血债血偿
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_312(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->diewithme = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->diewithme = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态315 浊水之舞
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_315(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}

				if (pAtt)
				{
					Cmd::stAttackMagicUserCmd cmd;

					cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
					cmd.dwDefenceTempID = pEntry->tempid;
					cmd.dwUserTempID = pAtt->tempid;
					cmd.wdMagicType = 176;
					cmd.byAction = Cmd::Ani_Num;
					cmd.byDirect = pEntry->getDir();
					cmd.xDes = (WORD)pEntry->getPos().x;
					cmd.yDes = (WORD)pEntry->getPos().y;

					zSkill *s = NULL;

					s = pAtt->usm.findSkill(176);
					if(s)
					{
						s->action(&cmd, sizeof(cmd));
					}
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态316 分裂
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_316(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 60;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					if (pEntry->summon)
					{
						switch(pEntry->summon->npc->id)
						{
							case 45041 ... 45050:
								{
									pEntry->summon->summonsplit = sse.value;
								}
								break;
						}
					}
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				if (pEntry->summon)
				{
					switch(pEntry->summon->npc->id)
					{
						case 45041 ... 45050:
							{
								pEntry->summon->summonsplit = 0;
							}
							break;
					}
				}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态317 每秒释放一个技能让一屏范围内的友军防御提升
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_317(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				Cmd::stAttackMagicUserCmd cmd;

				switch (pEntry->getType())
				{
					case zSceneEntry::SceneEntry_Player:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					case zSceneEntry::SceneEntry_NPC:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					default:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
				}
		
				cmd.dwDefenceTempID = 0;
				cmd.dwUserTempID = pEntry->tempid;
				cmd.wdMagicType = sse.value;
				cmd.byAction = Cmd::Ani_Num;
				cmd.byDirect = pEntry->getDir();
				cmd.xDes = (WORD)pEntry->getPos().x;
				cmd.yDes = (WORD)pEntry->getPos().y;

				zSkill *s = NULL;

				s = zSkill::createTempSkill(pEntry , sse.value , 1);
				if(s)
				{
					s->action(&cmd, sizeof(cmd));
					SAFE_DELETE(s);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态318 每秒释放一个技能让一屏范围内的友军防御提升
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_318(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				Cmd::stAttackMagicUserCmd cmd;

				switch (pEntry->getType())
				{
					case zSceneEntry::SceneEntry_Player:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					case zSceneEntry::SceneEntry_NPC:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					default:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
				}
		
				cmd.dwDefenceTempID = 0;
				cmd.dwUserTempID = pEntry->tempid;
				cmd.wdMagicType = sse.value;
				cmd.byAction = Cmd::Ani_Num;
				cmd.byDirect = pEntry->getDir();
				cmd.xDes = (WORD)pEntry->getPos().x;
				cmd.yDes = (WORD)pEntry->getPos().y;

				zSkill *s = NULL;

				s = zSkill::createTempSkill(pEntry , sse.value , 1);
				if(s)
				{
					s->action(&cmd, sizeof(cmd));
					SAFE_DELETE(s);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态319 每秒释放一个技能让一屏范围内的友军防御提升
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_319(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				Cmd::stAttackMagicUserCmd cmd;

				switch (pEntry->getType())
				{
					case zSceneEntry::SceneEntry_Player:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					case zSceneEntry::SceneEntry_NPC:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					default:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
				}
		
				cmd.dwDefenceTempID = 0;
				cmd.dwUserTempID = pEntry->tempid;
				cmd.wdMagicType = sse.value;
				cmd.byAction = Cmd::Ani_Num;
				cmd.byDirect = pEntry->getDir();
				cmd.xDes = (WORD)pEntry->getPos().x;
				cmd.yDes = (WORD)pEntry->getPos().y;

				zSkill *s = NULL;

				s = zSkill::createTempSkill(pEntry , sse.value , 1);
				if(s)
				{
					s->action(&cmd, sizeof(cmd));
					SAFE_DELETE(s);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态320 每秒释放一个技能让一屏范围内的友军防御提升
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_320(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				Cmd::stAttackMagicUserCmd cmd;

				switch (pEntry->getType())
				{
					case zSceneEntry::SceneEntry_Player:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					case zSceneEntry::SceneEntry_NPC:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					default:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
				}
		
				cmd.dwDefenceTempID = 0;
				cmd.dwUserTempID = pEntry->tempid;
				cmd.wdMagicType = sse.value;
				cmd.byAction = Cmd::Ani_Num;
				cmd.byDirect = pEntry->getDir();
				cmd.xDes = (WORD)pEntry->getPos().x;
				cmd.yDes = (WORD)pEntry->getPos().y;

				zSkill *s = NULL;

				s = zSkill::createTempSkill(pEntry , sse.value , 1);
				if(s)
				{
					s->action(&cmd, sizeof(cmd));
					SAFE_DELETE(s);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态321 每秒释放一个技能让一屏范围内的友军防御提升
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_321(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				Cmd::stAttackMagicUserCmd cmd;

				switch (pEntry->getType())
				{
					case zSceneEntry::SceneEntry_Player:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					case zSceneEntry::SceneEntry_NPC:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					default:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
				}
		
				cmd.dwDefenceTempID = 0;
				cmd.dwUserTempID = pEntry->tempid;
				cmd.wdMagicType = sse.value;
				cmd.byAction = Cmd::Ani_Num;
				cmd.byDirect = pEntry->getDir();
				cmd.xDes = (WORD)pEntry->getPos().x;
				cmd.yDes = (WORD)pEntry->getPos().y;

				zSkill *s = NULL;

				s = zSkill::createTempSkill(pEntry , sse.value , 1);
				if(s)
				{
					s->action(&cmd, sizeof(cmd));
					SAFE_DELETE(s);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态322 每秒释放一个技能让一屏范围内的友军防御提升
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_322(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				Cmd::stAttackMagicUserCmd cmd;

				switch (pEntry->getType())
				{
					case zSceneEntry::SceneEntry_Player:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					case zSceneEntry::SceneEntry_NPC:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					default:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
				}
		
				cmd.dwDefenceTempID = 0;
				cmd.dwUserTempID = pEntry->tempid;
				cmd.wdMagicType = sse.value;
				cmd.byAction = Cmd::Ani_Num;
				cmd.byDirect = pEntry->getDir();
				cmd.xDes = (WORD)pEntry->getPos().x;
				cmd.yDes = (WORD)pEntry->getPos().y;

				zSkill *s = NULL;

				s = zSkill::createTempSkill(pEntry , sse.value , 1);
				if(s)
				{
					s->action(&cmd, sizeof(cmd));
					SAFE_DELETE(s);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态323 每秒释放一个技能让一屏范围内的友军防御提升
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_323(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				Cmd::stAttackMagicUserCmd cmd;

				switch (pEntry->getType())
				{
					case zSceneEntry::SceneEntry_Player:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					case zSceneEntry::SceneEntry_NPC:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					default:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
				}
		
				cmd.dwDefenceTempID = 0;
				cmd.dwUserTempID = pEntry->tempid;
				cmd.wdMagicType = sse.value;
				cmd.byAction = Cmd::Ani_Num;
				cmd.byDirect = pEntry->getDir();
				cmd.xDes = (WORD)pEntry->getPos().x;
				cmd.yDes = (WORD)pEntry->getPos().y;

				zSkill *s = NULL;

				s = zSkill::createTempSkill(pEntry , sse.value , 1);
				if(s)
				{
					s->action(&cmd, sizeof(cmd));
					SAFE_DELETE(s);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态324 每秒释放一个技能让一屏范围内的友军防御提升
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_324(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				Cmd::stAttackMagicUserCmd cmd;

				switch (pEntry->getType())
				{
					case zSceneEntry::SceneEntry_Player:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					case zSceneEntry::SceneEntry_NPC:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
					default:
						{
							cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
						}
						break;
				}
		
				cmd.dwDefenceTempID = 0;
				cmd.dwUserTempID = pEntry->tempid;
				cmd.wdMagicType = sse.value;
				cmd.byAction = Cmd::Ani_Num;
				cmd.byDirect = pEntry->getDir();
				cmd.xDes = (WORD)pEntry->getPos().x;
				cmd.yDes = (WORD)pEntry->getPos().y;

				zSkill *s = NULL;

				s = zSkill::createTempSkill(pEntry , sse.value , 1);
				if(s)
				{
					s->action(&cmd, sizeof(cmd));
					SAFE_DELETE(s);
				}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态325 法术防御降低百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_325(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 41;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.dmdefp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.dmdefp = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.dmdefp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief 状态326 增加道具物理攻击力百分比
 * \author snip3r
 */
BYTE SkillStatus_326(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 211;
	pEntry->reSendData = true;
	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.physic_att = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.physic_att = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.physic_att = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief 状态327 增加道具物理防御力百分比
 * \author snip3r
 */
BYTE SkillStatus_327(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 211;
	pEntry->reSendData = true;
	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.physic_prt = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.physic_prt = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.physic_prt = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief 状态328 增加道具魔法攻击力百分比
 * \author snip3r
 */
BYTE SkillStatus_328(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 211;
	pEntry->reSendData = true;
	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.magic_att = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.magic_att = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.magic_att = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief 状态329 增加道具魔法防御力百分比
 * \author snip3r
 */
BYTE SkillStatus_329(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 211;
	pEntry->reSendData = true;
	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.magic_prt = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.magic_prt = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.magic_prt = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态330 冲锋技能对目标产生额外伤害
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 * \author snip3r
 */
BYTE SkillStatus_330(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_BAD;
	sse.byMutexType = 63;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.appenddam += sse.value;

					//if (sse.attacktype == zSceneEntry::SceneEntry_Player)
					//{
					//	SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
					//	if (pUser)
					//	{
					//		zPos tmppos = pEntry->getPos();
					//		const zPos &pos1 = pUser->getPos();
					//		if (pos1.x>tmppos.x)
					//		{
					//			tmppos.x++;
					//		}
					//		else if (tmppos.x>0)
					//		{
					//			tmppos.x--;
					//		}

					//		if (pos1.y>tmppos.y)
					//		{
					//			tmppos.y++;
					//		}
					//		else if (tmppos.y>0)
					//		{
					//			tmppos.y--;
					//		}
					//		pUser->goTo(tmppos);
					//	}
					//}
					return SKILL_ACTIVE;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				SceneEntryPk *pAtt = NULL;
				switch(sse.attacktype)
				{
					case zSceneEntry::SceneEntry_Player:	// 玩家角色
						{
							pAtt = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
						}
						break;
					case zSceneEntry::SceneEntry_NPC:		// NPC
						{
							pAtt = SceneNpcManager::getMe().getNpcByTempID(sse.dwTempID);
						}
						break;
					default:
						break;
				}

				if (pAtt)
				{
					/*SWORD wdHP = */pEntry->directDamage(pAtt,pEntry->skillValue.appenddam, true);
					//ScenePk::attackRTCmdToNine(rev , pAtt , this , wdHP , 0);
					pEntry->processDeath(pAtt);
				}


				//if (sse.attacktype == zSceneEntry::SceneEntry_Player)
				//{
				//	SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
				//	if (pUser)
				//	{
				//		zPos tmppos = pEntry->getPos();
				//		const zPos &pos1 = pUser->getPos();
				//		if (pos1.x>tmppos.x)
				//		{
				//			tmppos.x++;
				//		}
				//		else if (tmppos.x>0)
				//		{
				//			tmppos.x--;
				//		}

				//		if (pos1.y>tmppos.y)
				//		{
				//			tmppos.y++;
				//		}
				//		else if (tmppos.y>0)
				//		{
				//			tmppos.y--;
				//		}
				//		pUser->goTo(tmppos);
				//	}
				//}
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				//if (sse.attacktype == zSceneEntry::SceneEntry_Player)
				//{
				//	SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(sse.dwTempID);
				//	if (pUser)
				//	{
				//		zPos tmppos = pEntry->getPos();
				//		const zPos &pos1 = pUser->getPos();
				//		if (pos1.x>tmppos.x)
				//		{
				//			tmppos.x++;
				//		}
				//		else if (tmppos.x>0)
				//		{
				//			tmppos.x--;
				//		}

				//		if (pos1.y>tmppos.y)
				//		{
				//			tmppos.y++;
				//		}
				//		else if (tmppos.y>0)
				//		{
				//			tmppos.y--;
				//		}
				//		pUser->goTo(tmppos);
				//	}
				//}
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态331 人物力量增加数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 * \author snip3r
 */
BYTE SkillStatus_331(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x_upstr = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x_upstr = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief 状态332 人物物理攻击力增加数值，用来解决增加攻击力buf的冲突
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 * \author snip3r
 */
BYTE SkillStatus_332(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.physic_add = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.physic_add = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态333 人物魔法攻击力增加数值，用来解决增加攻击力buf的冲突
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 * \author snip3r
 */
BYTE SkillStatus_333(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.magic_add = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.magic_add = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
* \brief  状态334 新增 召唤坐骑
* \param pEntry 状态所有者
* \param sse 技能状态
* \return 技能状态返回值,参见头文件中的技能状态处理返回值枚举
*/
BYTE SkillStatus_334(SceneEntryPk *pEntry,SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 0;

	SceneUser * use = (SceneUser *)pEntry;

	if(use->horse.mount())
	{
		use->horse.mount(false);
		return SKILL_RETURN;
	}

	//soke 首先设置坐骑的类型
	use->horse.horse(sse.value);
	////soke 向客户端汇报坐骑的数据
	//use->horse.sendData();
	//soke 设置用户上马并设置用户的速度
	use->horse.mount(true, sse.dwTime);

	return SKILL_RETURN;
}

/**
 * \brief	状态335 人马合一
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_335(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	//return SKILL_RECOVERY;
	pEntry->reSendData = true;
	SceneUser * use = (SceneUser *)pEntry;
	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
			break;
		case ACTION_STEP_START:
			{
				use->horse.uesHorse(true);
				return SKILL_RECOVERY;
			}
			break;
		case ACTION_STEP_RELOAD:
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				use->horse.uesHorse(false);
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态336 人物精神增加数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 * \author snip3r
 */
BYTE SkillStatus_336(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x_upmen = sse.value; //精神固定值
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x_upmen = 0; //精神固定值
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态337 人物智力增加数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 * \author snip3r
 */
BYTE SkillStatus_337(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x_upint = sse.value; //智力固定值
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x_upint = 0; //智力固定值
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态338 人物体质增加数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 * \author snip3r
 */
BYTE SkillStatus_338(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x_upcon = sse.value; //体质固定值
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x_upcon = 0; //体质固定值
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态339 人物敏捷增加数值
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 * \author snip3r
 */
BYTE SkillStatus_339(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 7;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x_updex = sse.value; //敏捷固定值
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x_updex = 0; //敏捷固定值
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态340 伤害减少技能
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_340(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->x_mhpd = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->x_mhpd = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
 /**
 * \brief	状态341 (被动)生命值上限提升-新1
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_341(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

	switch(sse.byStep)
	  {
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x1_pmaxhp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x1_pmaxhp = sse.value;
				return SKILL_RETURN;	
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x1_pmaxhp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态342 (被动)生命值上限提升-新2
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_342(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

	switch(sse.byStep)
	  {
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x2_pmaxhp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x2_pmaxhp = sse.value;
				return SKILL_RETURN;	
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x2_pmaxhp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态343 (被动)生命值上限提升-新3
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_343(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

	switch(sse.byStep)
	  {
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x3_pmaxhp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x3_pmaxhp = sse.value;
				return SKILL_RETURN;	
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x3_pmaxhp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态344 (被动)生命值上限提升-新4
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_344(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

	switch(sse.byStep)
	  {
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x4_pmaxhp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x4_pmaxhp = sse.value;
				return SKILL_RETURN;	
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x4_pmaxhp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态345 (被动)生命值上限提升-新5
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_345(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 22;
	pEntry->reSendData = true;

	switch(sse.byStep)
	  {
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x5_pmaxhp = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x5_pmaxhp = sse.value;
				return SKILL_RETURN;	
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x5_pmaxhp = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态346 增加状态接受者物理攻击力固定值(被动)-新1
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_346(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 120;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x1_pupdam = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x1_pupdam = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x1_pupdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态347 增加状态接受者物理攻击力固定值(被动)-新2
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_347(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 120;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x2_pupdam = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x2_pupdam = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x2_pupdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态348 增加状态接受者物理攻击力固定值(被动)-新3
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_348(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 120;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x3_pupdam = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x3_pupdam = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x3_pupdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态349 增加状态接受者法术攻击力固定值(被动)-新1
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_349(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 120;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x1_pumdam = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x1_pumdam = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x1_pumdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态350 增加状态接受者法术攻击力固定值(被动)-新2
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_350(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 120;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x2_pumdam = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x2_pumdam = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x2_pumdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态351 增加状态接受者法术攻击力固定值(被动)-新3
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_351(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 120;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x3_pumdam = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x3_pumdam = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x3_pumdam = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态352 增加状态接受者物理防御力固定值(被动)-新1
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_352(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 121;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x1_pupdef = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x1_pupdef = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x1_pupdef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态353 增加状态接受者物理防御力固定值(被动)-新2
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_353(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 121;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x2_pupdef = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x2_pupdef = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x2_pupdef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态354 增加状态接受者物理防御力固定值(被动)-新3
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_354(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 121;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x3_pupdef = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x3_pupdef = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x3_pupdef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态355 增加状态接受者法术防御力固定值(被动)-新1
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_355(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 121;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x1_pumdef = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x1_pumdef = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x1_pumdef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态356 增加状态接受者法术防御力固定值(被动)-新2
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_356(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 121;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x2_pumdef = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x2_pumdef = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x2_pumdef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态357 增加状态接受者法术防御力固定值(被动)-新3
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_357(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 121;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.x3_pumdef = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.x3_pumdef = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.x3_pumdef = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态358 伤害减少技能
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_358(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->x1_mhpd = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->x1_mhpd = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态359 伤害减少技能
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_359(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->x2_mhpd = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->x2_mhpd = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态360 伤害减少技能
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_360(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->x3_mhpd = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->x3_mhpd = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态361 伤害减少技能
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_361(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->x4_mhpd = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->x4_mhpd = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态362 伤害减少技能
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_362(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->x5_mhpd = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->x5_mhpd = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}
/**
 * \brief	状态363 伤害减少技能
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_363(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 20;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->x6_mhpd = true;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->x6_mhpd = false;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}



/**
 * \brief	状态364 增加状态接受者物理攻击力百分比 刺客专用
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_364(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 0;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.cikewu = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.cikewu = sse.value;
				return SKILL_RETURN;	
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.cikewu = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}



/**
 * \brief	状态364 增加状态接受者法术攻击力百分比 刺客专用
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_365(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 0;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.cikemo = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.cikemo = sse.value;
				return SKILL_RETURN;	
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.cikemo = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


//轰天专用
/**
 * \brief	状态366 提升武器的物理攻击力百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_366(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 201;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.weaponupdamp2 = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.weaponupdamp2 = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.weaponupdamp2 = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}

/**
 * \brief	状态367 提升武器的法术攻击力百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_367(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 201;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.weaponumdamp2 = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.weaponumdamp2 = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.weaponumdamp2 = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}



/**
 * \brief	状态368 角色或目标的物理防御力上升百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_368(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 54;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{

					pEntry->skillValue.updefp2 = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.updefp2 = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.updefp2 = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态369 表示状态接受者的魔法防御力提升百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_369(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 777;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.ice_umdefp2 = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.ice_umdefp2 = sse.value;
				return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.ice_umdefp2 = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}



/**
 * \brief	状态370 增加状态接受者物理攻击力百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_370(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 778;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.shuanggong = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.shuanggong = sse.value;
				return SKILL_RETURN;	
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.shuanggong = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}


/**
 * \brief	状态371 增加状态接受者物理攻击力百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_371(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 779;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.shuangfang = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.shuangfang = sse.value;
				return SKILL_RETURN;	
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.shuangfang = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}



/**
 * \brief	状态372 增加状态接受者血量百分比
 * \param pEntry 状态所有者
 * \param sse 技能状态
 * \return 技能状态返回值，参见头文件中的技能状态处理返回值枚举
 */
BYTE SkillStatus_372(SceneEntryPk *pEntry, SkillStatusElement &sse)
{
	sse.byGoodnessType = SKILL_GOOD;
	sse.byMutexType = 790;
	pEntry->reSendData = true;

	switch(sse.byStep)
	{
		case ACTION_STEP_DOPASS:
		case ACTION_STEP_START:
		case ACTION_STEP_RELOAD:
			{
				if(zMisc::selectByPercent((int)(sse.percent)))
				{
					pEntry->skillValue.xueliang = sse.value;
					return SKILL_RECOVERY;
				}
				else
					return SKILL_RETURN;
			}
			break;
		case ACTION_STEP_TIMER:
			{
				pEntry->skillValue.xueliang = sse.value;
				return SKILL_RETURN;	
			}
			break;
		case ACTION_STEP_STOP:
		case ACTION_STEP_CLEAR:
			{
				pEntry->skillValue.xueliang = 0;
			}
			break;
		default:
			break;
	}

	return SKILL_RETURN;
}



//--------------------------------------------------

/**
 * \brief 技能状态管理器构造函数
 * \author fqnewman
 */
SkillStatusManager::SkillStatusManager()
{
	entry = NULL;
	bclearActiveSkillStatus = false;
}


/**
 * \brief 技能状态管理器析构函数
 * \author fqnewman
 */
SkillStatusManager::~SkillStatusManager()
{
	//if (entry->getType() == zSceneEntry::SceneEntry_Player)  saveSkillStatus();
}

/**
 * \brief 技能状态管理器析构函数
 * \param pEntry 传入技能管理器的使用对象
 * \author fqnewman
 */
void SkillStatusManager::initMe(SceneEntryPk *pEntry)
{
	entry = pEntry;
	funlist[0].func = SkillStatus_0;
	funlist[1].func = SkillStatus_1;
	funlist[2].func = SkillStatus_2;
	funlist[3].func = SkillStatus_3;
	funlist[4].func = SkillStatus_4;
	funlist[5].func = SkillStatus_5;
	funlist[6].func = SkillStatus_6;
	funlist[7].func = SkillStatus_7;
	funlist[8].func = SkillStatus_8;
	funlist[9].func = SkillStatus_9;
	funlist[10].func = SkillStatus_10;
	funlist[11].func = SkillStatus_11;
	funlist[12].func = SkillStatus_12;
	funlist[13].func = SkillStatus_13;
	funlist[14].func = SkillStatus_14;
	funlist[15].func = SkillStatus_15;
	funlist[16].func = SkillStatus_16;
	funlist[17].func = SkillStatus_17;
	funlist[18].func = SkillStatus_18;
	funlist[19].func = SkillStatus_19;
	funlist[20].func = SkillStatus_20;
	funlist[21].func = SkillStatus_21;
	funlist[22].func = SkillStatus_22;
	funlist[23].func = SkillStatus_23;
	funlist[24].func = SkillStatus_24;
	funlist[25].func = SkillStatus_25;
	funlist[26].func = SkillStatus_26;
	funlist[27].func = SkillStatus_27;
	funlist[28].func = SkillStatus_28;
	funlist[29].func = SkillStatus_29;
	funlist[30].func = SkillStatus_30;
	funlist[31].func = SkillStatus_31;
	funlist[32].func = SkillStatus_32;
	funlist[33].func = SkillStatus_33;
	funlist[34].func = SkillStatus_34;
	funlist[35].func = SkillStatus_35;
	funlist[36].func = SkillStatus_36;
	funlist[37].func = SkillStatus_37;
	funlist[38].func = SkillStatus_38;
	funlist[39].func = SkillStatus_39;
	funlist[40].func = SkillStatus_40;
	funlist[41].func = SkillStatus_41;
	funlist[42].func = SkillStatus_42;
	funlist[43].func = SkillStatus_43;
	funlist[44].func = SkillStatus_44;
	funlist[45].func = SkillStatus_45;
	funlist[46].func = SkillStatus_46;
	funlist[47].func = SkillStatus_47;
	funlist[48].func = SkillStatus_48;
	funlist[49].func = SkillStatus_49;
	funlist[50].func = SkillStatus_50;
	funlist[51].func = SkillStatus_51;
	funlist[52].func = SkillStatus_52;
	funlist[53].func = SkillStatus_53;
	funlist[54].func = SkillStatus_54;
	funlist[55].func = SkillStatus_55;
	funlist[56].func = SkillStatus_56;
	funlist[57].func = SkillStatus_57;
	funlist[58].func = SkillStatus_58;
	funlist[59].func = SkillStatus_59;
	funlist[60].func = SkillStatus_60;
	funlist[61].func = SkillStatus_61;
	funlist[62].func = SkillStatus_62;
	funlist[63].func = SkillStatus_63;
	funlist[64].func = SkillStatus_64;
	funlist[65].func = SkillStatus_65;
	funlist[66].func = SkillStatus_66;
	funlist[67].func = SkillStatus_67;
	funlist[68].func = SkillStatus_68;
	funlist[69].func = SkillStatus_69;
	funlist[70].func = SkillStatus_70;
	funlist[71].func = SkillStatus_71;
	funlist[72].func = SkillStatus_72;
	funlist[73].func = SkillStatus_73;
	funlist[74].func = SkillStatus_74;
	funlist[75].func = SkillStatus_75;
	funlist[76].func = SkillStatus_76;
	funlist[77].func = SkillStatus_77;
	funlist[78].func = SkillStatus_78;
	funlist[79].func = SkillStatus_79;
	funlist[80].func = SkillStatus_80;
	funlist[81].func = SkillStatus_81;
	funlist[82].func = SkillStatus_82;
	funlist[83].func = SkillStatus_83;
	funlist[84].func = SkillStatus_84;
	funlist[85].func = SkillStatus_85;
	funlist[86].func = SkillStatus_86;
	funlist[87].func = SkillStatus_87;
	funlist[88].func = SkillStatus_88;
	funlist[89].func = SkillStatus_89;
	funlist[90].func = SkillStatus_90;
	funlist[91].func = SkillStatus_91;
	funlist[92].func = SkillStatus_92;
	funlist[93].func = SkillStatus_93;
	funlist[94].func = SkillStatus_94;
	funlist[95].func = SkillStatus_95;
	funlist[96].func = SkillStatus_96;
	funlist[97].func = SkillStatus_97;
	funlist[98].func = SkillStatus_98;
	funlist[99].func = SkillStatus_99;
	funlist[100].func = SkillStatus_100;
	funlist[101].func = SkillStatus_101;
	funlist[102].func = SkillStatus_102;
	funlist[103].func = SkillStatus_101;
	funlist[104].func = SkillStatus_101;
	funlist[105].func = SkillStatus_101;
	funlist[106].func = SkillStatus_101;
	funlist[107].func = SkillStatus_101;
	funlist[108].func = SkillStatus_101;
	funlist[109].func = SkillStatus_101;
	funlist[110].func = SkillStatus_110;
	funlist[111].func = SkillStatus_111;
	funlist[112].func = SkillStatus_101;
	funlist[113].func = SkillStatus_113;
	funlist[114].func = SkillStatus_114;
	funlist[115].func = SkillStatus_115;
	funlist[116].func = SkillStatus_116;
	funlist[117].func = SkillStatus_117;
	funlist[118].func = SkillStatus_101;
	funlist[119].func = SkillStatus_119;
	funlist[120].func = SkillStatus_120;
	funlist[121].func = SkillStatus_121;
	funlist[122].func = SkillStatus_122;
	funlist[123].func = SkillStatus_123;
	funlist[124].func = SkillStatus_124;
	funlist[125].func = SkillStatus_125;
	funlist[126].func = SkillStatus_126;
	funlist[127].func = SkillStatus_127;
	funlist[128].func = SkillStatus_128;
	funlist[129].func = SkillStatus_129;
	funlist[130].func = SkillStatus_130;
	funlist[131].func = SkillStatus_131;
	funlist[132].func = SkillStatus_132;
	funlist[133].func = SkillStatus_133;
	funlist[134].func = SkillStatus_134;
	funlist[135].func = SkillStatus_135;
	funlist[136].func = SkillStatus_136;
	funlist[137].func = SkillStatus_137;
	funlist[138].func = SkillStatus_138;
	funlist[139].func = SkillStatus_139;
	funlist[140].func = SkillStatus_140;
	funlist[141].func = SkillStatus_141;
	funlist[142].func = SkillStatus_142;
	funlist[143].func = SkillStatus_143;
	funlist[144].func = SkillStatus_144;
	funlist[145].func = SkillStatus_145;
	funlist[146].func = SkillStatus_146;
	funlist[147].func = SkillStatus_147;
	funlist[148].func = SkillStatus_148;
	funlist[149].func = SkillStatus_149;
	funlist[150].func = SkillStatus_150;
	funlist[151].func = SkillStatus_151;
	funlist[152].func = SkillStatus_152;
	funlist[153].func = SkillStatus_153;
	funlist[154].func = SkillStatus_154;
	funlist[155].func = SkillStatus_155;
	funlist[156].func = SkillStatus_156;
	funlist[157].func = SkillStatus_157;
	funlist[158].func = SkillStatus_158;
	funlist[159].func = SkillStatus_159;
	funlist[160].func = SkillStatus_160;
	funlist[161].func = SkillStatus_161;
	funlist[162].func = SkillStatus_0;
	funlist[163].func = SkillStatus_163;
	funlist[164].func = SkillStatus_0;
	funlist[165].func = SkillStatus_165;
	funlist[166].func = SkillStatus_0;
	funlist[167].func = SkillStatus_0;
	funlist[168].func = SkillStatus_168;
	funlist[169].func = SkillStatus_169;
	funlist[170].func = SkillStatus_170;
	funlist[171].func = SkillStatus_0;
	funlist[172].func = SkillStatus_172;
	funlist[173].func = SkillStatus_173;
	funlist[174].func = SkillStatus_0;
	funlist[175].func = SkillStatus_0;
	funlist[176].func = SkillStatus_176;
	funlist[177].func = SkillStatus_177;
	funlist[178].func = SkillStatus_178;
	funlist[179].func = SkillStatus_179;
	funlist[180].func = SkillStatus_180;
	funlist[181].func = SkillStatus_0;
	funlist[182].func = SkillStatus_182;
	funlist[183].func = SkillStatus_0;
	funlist[184].func = SkillStatus_184;
	funlist[185].func = SkillStatus_185;
	funlist[186].func = SkillStatus_186;
	funlist[187].func = SkillStatus_187;
	funlist[188].func = SkillStatus_188;
	funlist[189].func = SkillStatus_189;
	funlist[190].func = SkillStatus_190;
	funlist[191].func = SkillStatus_191;
	funlist[192].func = SkillStatus_192;
	funlist[193].func = SkillStatus_193;
	funlist[194].func = SkillStatus_194;
	funlist[195].func = SkillStatus_195;
	funlist[196].func = SkillStatus_196;
	funlist[197].func = SkillStatus_197;
	funlist[198].func = SkillStatus_198;
	funlist[199].func = SkillStatus_199;
	funlist[200].func = SkillStatus_200;
	funlist[201].func = SkillStatus_201;
	funlist[202].func = SkillStatus_202;
	funlist[203].func = SkillStatus_203;
	funlist[204].func = SkillStatus_204;
	funlist[205].func = SkillStatus_205;
	funlist[206].func = SkillStatus_206;
	funlist[207].func = SkillStatus_207;
	funlist[208].func = SkillStatus_208;
	funlist[209].func = SkillStatus_209;
	funlist[210].func = SkillStatus_210;
	funlist[211].func = SkillStatus_0;
	funlist[212].func = SkillStatus_0;
	funlist[213].func = SkillStatus_0;
	funlist[214].func = SkillStatus_0;
	funlist[215].func = SkillStatus_0;
	funlist[216].func = SkillStatus_216;
	funlist[217].func = SkillStatus_217;
	funlist[218].func = SkillStatus_218;
	funlist[219].func = SkillStatus_219;
	funlist[220].func = SkillStatus_220;
	funlist[221].func = SkillStatus_221;
	funlist[222].func = SkillStatus_222;
	funlist[223].func = SkillStatus_223;
	funlist[224].func = SkillStatus_224;
	funlist[225].func = SkillStatus_225;
	funlist[226].func = SkillStatus_226;
	funlist[227].func = SkillStatus_227;
	funlist[228].func = SkillStatus_228;
	funlist[229].func = SkillStatus_229;
	funlist[230].func = SkillStatus_230;
	funlist[231].func = SkillStatus_231;
	funlist[232].func = SkillStatus_232;
	funlist[233].func = SkillStatus_233;
	funlist[234].func = SkillStatus_234;
	funlist[235].func = SkillStatus_235;
	funlist[236].func = SkillStatus_236;
	funlist[237].func = SkillStatus_237;
	funlist[238].func = SkillStatus_238;
	funlist[239].func = SkillStatus_239;
	funlist[240].func = SkillStatus_240;
	funlist[241].func = SkillStatus_241;
	funlist[242].func = SkillStatus_242;
	funlist[243].func = SkillStatus_243;
	funlist[244].func = SkillStatus_244;
	funlist[245].func = SkillStatus_245;
	funlist[246].func = SkillStatus_246;
	funlist[247].func = SkillStatus_247;
	funlist[248].func = SkillStatus_248;
	funlist[249].func = SkillStatus_249;
	funlist[250].func = SkillStatus_250;
	funlist[251].func = SkillStatus_251;
	funlist[252].func = SkillStatus_252;
	funlist[253].func = SkillStatus_253;
	funlist[254].func = SkillStatus_254;
	funlist[255].func = SkillStatus_255;
	funlist[256].func = SkillStatus_256;
	funlist[257].func = SkillStatus_257;
	funlist[258].func = SkillStatus_258;
	funlist[259].func = SkillStatus_259;
	funlist[260].func = SkillStatus_260;
	funlist[261].func = SkillStatus_261;
	funlist[262].func = SkillStatus_262;
	funlist[263].func = SkillStatus_263;
	funlist[264].func = SkillStatus_264;
	funlist[265].func = SkillStatus_265;
	funlist[266].func = SkillStatus_266;
	funlist[267].func = SkillStatus_0;
	funlist[268].func = SkillStatus_268;
	funlist[269].func = SkillStatus_0;
	funlist[270].func = SkillStatus_270;
	funlist[271].func = SkillStatus_271;
	funlist[272].func = SkillStatus_272;
	funlist[273].func = SkillStatus_273;
	funlist[274].func = SkillStatus_274;
	funlist[275].func = SkillStatus_275;
	funlist[276].func = SkillStatus_276;
	funlist[277].func = SkillStatus_277;
	funlist[278].func = SkillStatus_278;
	funlist[279].func = SkillStatus_279;
	funlist[280].func = SkillStatus_280;
	funlist[281].func = SkillStatus_281;
	funlist[282].func = SkillStatus_282;
	funlist[283].func = SkillStatus_283;
	funlist[284].func = SkillStatus_284;
	funlist[285].func = SkillStatus_285;
	funlist[286].func = SkillStatus_286;
	funlist[287].func = SkillStatus_287;
	funlist[288].func = SkillStatus_288;
	funlist[289].func = SkillStatus_289;
	funlist[290].func = SkillStatus_290;
	funlist[291].func = SkillStatus_0;
	funlist[292].func = SkillStatus_292;
	funlist[293].func = SkillStatus_293;
	funlist[294].func = SkillStatus_294;
	funlist[295].func = SkillStatus_295;
	funlist[296].func = SkillStatus_296;
	funlist[297].func = SkillStatus_0;
	funlist[298].func = SkillStatus_298;
	funlist[299].func = SkillStatus_299;
	funlist[300].func = SkillStatus_300;
	funlist[301].func = SkillStatus_301;
	funlist[302].func = SkillStatus_302;
	funlist[303].func = SkillStatus_303;
	funlist[304].func = SkillStatus_304;
	funlist[305].func = SkillStatus_305;
	funlist[306].func = SkillStatus_306;
	funlist[307].func = SkillStatus_307;
	funlist[308].func = SkillStatus_308;
	funlist[309].func = SkillStatus_309;
	funlist[310].func = SkillStatus_0;
	funlist[311].func = SkillStatus_0;
	funlist[312].func = SkillStatus_312;
	funlist[313].func = SkillStatus_0;
	funlist[314].func = SkillStatus_0;
	funlist[315].func = SkillStatus_315;
	funlist[316].func = SkillStatus_316;
	funlist[317].func = SkillStatus_0;
	funlist[318].func = SkillStatus_0;
	funlist[319].func = SkillStatus_0;
	funlist[320].func = SkillStatus_0;
	funlist[316].func = SkillStatus_0;
	funlist[317].func = SkillStatus_317;
	funlist[318].func = SkillStatus_318;
	funlist[319].func = SkillStatus_319;
	funlist[320].func = SkillStatus_320;
	funlist[321].func = SkillStatus_321;
	funlist[322].func = SkillStatus_322;
	funlist[323].func = SkillStatus_323;
	funlist[324].func = SkillStatus_324;
	funlist[325].func = SkillStatus_325;
	funlist[326].func = SkillStatus_326;
	funlist[327].func = SkillStatus_327;
	funlist[328].func = SkillStatus_328;
	funlist[329].func = SkillStatus_329;
	funlist[330].func = SkillStatus_330;
	funlist[331].func = SkillStatus_331;
	funlist[332].func = SkillStatus_332;
	funlist[333].func = SkillStatus_333;
	funlist[334].func = SkillStatus_334;
	funlist[335].func = SkillStatus_335;
	funlist[336].func = SkillStatus_336;
	funlist[337].func = SkillStatus_337;
	funlist[338].func = SkillStatus_338;
	funlist[339].func = SkillStatus_339;
    funlist[340].func = SkillStatus_340;
	funlist[341].func = SkillStatus_341;
	funlist[342].func = SkillStatus_342;
	funlist[343].func = SkillStatus_343;
	funlist[344].func = SkillStatus_344;
	funlist[345].func = SkillStatus_345;
	funlist[346].func = SkillStatus_346;
	funlist[347].func = SkillStatus_347;
	funlist[348].func = SkillStatus_348;
	funlist[349].func = SkillStatus_349;
	funlist[350].func = SkillStatus_350;
	funlist[351].func = SkillStatus_351;
	funlist[352].func = SkillStatus_352;
	funlist[353].func = SkillStatus_353;
	funlist[354].func = SkillStatus_354;
	funlist[355].func = SkillStatus_355;
	funlist[356].func = SkillStatus_356;
	funlist[357].func = SkillStatus_357;
	funlist[358].func = SkillStatus_358;
	funlist[359].func = SkillStatus_359;
	funlist[360].func = SkillStatus_360;
	funlist[361].func = SkillStatus_361;
	funlist[362].func = SkillStatus_362;
	funlist[363].func = SkillStatus_363;
	funlist[364].func = SkillStatus_364;
	funlist[365].func = SkillStatus_365;
	funlist[366].func = SkillStatus_366;
	funlist[367].func = SkillStatus_367;
	funlist[368].func = SkillStatus_368;
	funlist[369].func = SkillStatus_369;
	funlist[370].func = SkillStatus_370;
	funlist[371].func = SkillStatus_371;
	funlist[372].func = SkillStatus_372;

	


	//if (entry->getType() == zSceneEntry::SceneEntry_Player) loadSkillStatus();
}

/**
 * \brief 通知某个状态数值
 * \author 王海军
 */
void SkillStatusManager::sendSelectStates(SceneEntryPk *pThis , DWORD state , WORD value , WORD time)
{
	pThis->sendtoSelectedState(state , value , time);
	pThis->sendSevenStateToMe(state , value , time);
}

/**
 * \brief 得到当前7大状态的数值
 * \author 王海军
 */
void SkillStatusManager::getSelectStates(Cmd::stSelectReturnStatesPropertyUserCmd *buf , unsigned long maxSize)
{

	using namespace Cmd;
	std::map<DWORD, SkillStatusElement>::iterator tIterator;
	zRTime ctv;
	for(tIterator = _activeElement.begin() ; tIterator !=_activeElement.end(); tIterator++)
	{
		//Zebra::logger->debug("目前状态%u , %u , %u" , tIterator->first , tIterator->second.id , tIterator->second.state);
/*		switch(tIterator->second.state)
		{
			case USTATE_POISON:
			case USTATE_FREEZE:
			case USTATE_CONFUSION:
			case USTATE_BLED:
			case USTATE_PALSY:
			case USTATE_BLIND:
			case USTATE_ROCK:*/
		if (tIterator->second.state >0 && 
				(sizeof(stSelectReturnStatesPropertyUserCmd) + sizeof(buf->states[0]) * buf->size)<= maxSize)
		{
			buf->states[buf->size].state = tIterator->second.state;
			buf->states[buf->size].result = tIterator->second.value;
			buf->states[buf->size].time = tIterator->second.dwTime;
			buf->size ++;
		}
/*				break;
			default:
				break;
		}*/
	}

	for(tIterator = _recoveryElement.begin() ; tIterator !=_recoveryElement.end(); tIterator++)
	{

		if (tIterator->second.state >0 &&
				(sizeof(stSelectReturnStatesPropertyUserCmd) + sizeof(buf->states[0]) * buf->size)<= maxSize)
		{
			buf->states[buf->size].state = tIterator->second.state;
			buf->states[buf->size].result = tIterator->second.value;
			buf->states[buf->size].time = (tIterator->second.qwTime- ctv.msecs())/1000;
			buf->size ++;
		}
	}

}

/**
 * \brief 加载历史的技能状态，上次下线的时候技能还存留在身上的技能状态继续有效
 * \author fqnewman
 */
void SkillStatusManager::loadSkillStatus(char *buf, DWORD length)
{
	SkillStatusElement *value;
//	char buf[zSocket::MAX_DATASIZE];
	unsigned int count;
	DWORD statelen;
	bool notify;
	/*DWORD length;

	count = 0; 
	length=0;
	bzero(buf,zSocket::MAX_DATASIZE);*/

	SkillState *pState = (SkillState *)buf;
	statelen = sizeof(SkillState);
	memcpy(&(entry->skillValue), pState, statelen);
	length-=statelen;

#ifdef _DEBUGLOG
	Zebra::logger->debug("[为角色(%s)(%d)加载保存的技能状态]", entry->name,entry->id);
#endif
	//COfflineSkillStatus::getOfflineSkillStatus(entry->id,buf,length);
	count = length/sizeof(SkillStatusElement);
	notify = false;
#ifdef _DEBUGLOG
	Zebra::logger->debug("[有%d个技能状态需要加载]", count);
#endif
	value = (SkillStatusElement *)(buf+statelen);

	SceneEntryPk *pAtt = NULL;
	if (value->attacktype == zSceneEntry::SceneEntry_Player)
	{
		pAtt = SceneUserManager::getMe().getUserByID(value->dwAttackerID);
		if (pAtt) value->dwTempID = pAtt->tempid;
	}

	for(unsigned int i=0; i<count; i++)
	{
		value->byStep = ACTION_STEP_RELOAD;
		switch(runStatusElement(*value))
		{
			case SKILL_RECOVERY:
				{
#ifdef _DEBUGLOG
					Zebra::logger->debug("[%d号技能状态被加载到临时被动表内]", value->id);
#endif
					//value->refresh = 1;
					//value->qwTime = SceneTimeTick::currentTime.msecs()+value->dwTime *1000;
					_recoveryElement[value->id/*value->byMutexType*/]=*value;
					_recoveryElement[value->id].qwTime = SceneTimeTick::currentTime.msecs()+_recoveryElement[value->id].dwTime *1000;
					if (value->state >0)
					{
						sendSelectStates(entry, value->state, value->value, value->dwTime);
						//entry->setEffectStatus(value->state);
						entry->showCurrentEffect(value->state, true);
					}
					notify = true;
				}
				break;
			case SKILL_ACTIVE:
				{
#ifdef _DEBUGLOG
					Zebra::logger->debug("[%d号技能状态被加载到主动表内]", value->id);
#endif
					_activeElement[value->id/*value->byMutexType*/]=*value;
					_recoveryElement[value->id].qwTime = SceneTimeTick::currentTime.msecs()+_recoveryElement[value->id].dwTime *1000;
					if (value->state >0)
					{
						sendSelectStates(entry, value->state, value->value, value->dwTime);
						//entry->setEffectStatus(value->state);
						entry->showCurrentEffect(value->state, true);
					}
					notify = true;
				}
				break;
			default:
#ifdef _DEBUGLOG
					Zebra::logger->debug("[%d号技能状态无法被加到对应的表中]", value->id);
#endif
				break;
		}
		value++;
	}

	if (notify&&entry)
	{
		entry->changeAndRefreshHMS(true, true);
	}
}

/**
 * \brief 存储历史的技能状态，上次下线的时候技能还存留在身上的技能状态继续有效
 * \author fqnewman
 */
void SkillStatusManager::saveSkillStatus(char *buf , DWORD &size)
{
	std::map<DWORD, SkillStatusElement>::iterator tIterator;
	SkillStatusElement *value;
	unsigned int count;
	DWORD statelen;
	DWORD length;

	length = 0;
	count = 0;
	statelen = 0;

	SkillState *pState = (SkillState *)buf;
	statelen = sizeof(SkillState);
	memcpy(pState, &(entry->skillValue), statelen);

	value = (SkillStatusElement *)(buf+statelen);
	length = sizeof(SkillStatusElement);
	for(tIterator = _activeElement.begin() ; tIterator !=_activeElement.end();tIterator ++)
	{
		memcpy(value,&tIterator->second, length);
#ifdef _DEBUGLOG
		Zebra::logger->debug("[技能状态%d被存储]", tIterator->second.id);
#endif
		count++;
		value++;
		if (count>3000) break;
	}

	if (count<3000)
	{
		for(tIterator = _recoveryElement.begin() ; tIterator !=_recoveryElement.end();tIterator ++)
		{
			memcpy(value,&tIterator->second, length);
#ifdef _DEBUGLOG
		Zebra::logger->debug("[技能状态%d被存储]", tIterator->second.id);
#endif
			count++;
			value++;
			if (count>3000) break;
		}
	}
	//if (count >0) COfflineSkillStatus::writeOfflineSkillStatus(entry->id,buf,count*length);
	size = statelen + count*length;
#ifdef _DEBUGLOG
	Zebra::logger->debug("[有%d个技能状态需要存储]", count);
#endif
}

/**
 * \brief 将一个技能操作施加在我的身上
 * \param carrier 技能操作投送器，里面包含了技能状态
 * \param good 标志这个技能是不是一个增益的技能
 * \author fqnewman
 * \return true 为继续下一个操作，false为停止并返回。
 */
bool SkillStatusManager::putOperationToMe(const SkillStatusCarrier &carrier, const bool good, SWORD rangDamageBonus)
{
	std::vector<SkillElement>::const_iterator tIterator;
	bool sendData = false;

	entry->isPhysics = false;
	if (!entry->preAttackMe(carrier.attacker, &carrier.revCmd, false, good)) return true;

	for (tIterator = carrier.status->_StatusElementList.begin();
		tIterator != carrier.status->_StatusElementList.end();
		tIterator++)
	{
		SkillStatusElement element;
		element.byStep	=	ACTION_STEP_START;
		element.id		=	tIterator->id;
		element.percent =	tIterator->percent;
		element.value	=	tIterator->value;
		element.state	=	tIterator->state;

		element.dwSkillID = carrier.skillbase->skillid;//carrier.skillID;
		element.dwTime = tIterator->time;
		element.qwTime = SceneTimeTick::currentTime.msecs()+element.dwTime *1000;

		SceneEntryPk *pAtt = carrier.attacker->getTopMaster();
		element.dwTempID = pAtt->tempid;
		element.dwAttackerID = pAtt->id;
		element.attacktype = pAtt->getType();
		if (element.dwTime <=2)
		{
			element.refresh = 0;
		}
		else
		{
			element.refresh = 1;
			sendData = true;
		}

		entry->curMagicManType = element.attacktype;
		entry->curMagicManID = element.dwTempID;
		entry->keepPos.x = (DWORD)carrier.revCmd.xDes;
		entry->keepPos.y = (DWORD)carrier.revCmd.yDes;
		entry->keepDir = carrier.revCmd.byDirect;

		switch(runStatusElement(element))
		{
			case SKILL_RECOVERY:
				{
#ifdef _DEBUGLOG
					Zebra::logger->debug("[临时被动]第[%u]号状态被施加在身上持续时间为[%u]", element.id,element.dwTime);
					Channel::sendSys(entry->tempid, Cmd::INFO_TYPE_SYS, "[临时被动]第[%u]号状态被施加在身上持续时间为[%u]", element.id,element.dwTime);
#endif
					element.qwTime = SceneTimeTick::currentTime.msecs()+element.dwTime *1000;
					if (element.state >0)
					{
						clearMapElement(element.id/*element.byMutexType*/,_recoveryElement, element.id,false);
						_recoveryElement[element.id/*element.byMutexType*/]=element;
						sendSelectStates(entry, element.state, element.value, element.dwTime);
						//entry->setEffectStatus(element.state);
						entry->showCurrentEffect(element.state, true);
					}
					else
					{
						clearMapElement(element.id/*element.byMutexType*/,_recoveryElement, element.id);
						_recoveryElement[element.id/*element.byMutexType*/]=element;
					}
				}
				break;
			case SKILL_BREAK:
				{
					return false;
				}
				break;
			case SKILL_ACTIVE:
				{
#ifdef _DEBUGLOG
					Zebra::logger->debug("[伤害状态]第[%u]号状态被施加在身上持续时间为[%u]",element.id,element.dwTime);
					Channel::sendSys(entry->tempid, Cmd::INFO_TYPE_SYS, "[攻击状态]第[%u]号状态被施加在身上持续时间为[%u]",element.id,element.dwTime);
#endif
					if (element.dwTime==0) break;
					clearMapElement(element.id/*element.byMutexType*/,_activeElement, element.id);
					_activeElement[element.id/*element.byMutexType*/]=element;
					if (element.state >0)
					{
						sendSelectStates(entry, element.state, element.value, element.dwTime);
						//entry->setEffectStatus(element.state);
						entry->showCurrentEffect(element.state, true);
					}
				}
				break;
			case SKILL_RETURN:
			default:
				break;
		}
	}

	if (carrier.status->isInjure == 1)
	{
		entry->AttackMe(carrier.attacker, &carrier.revCmd, entry->isPhysics, rangDamageBonus);
		carrier.attacker->setPetsChaseTarget(entry);
#ifdef _DEBUGLOG
		Zebra::logger->debug("[走攻防计算公式]");
		Channel::sendSys(entry->tempid, Cmd::INFO_TYPE_SYS,"[伤害法术]");
#endif
	}
	else
	{
		if (entry->notifyHMS)
		{
			entry->attackRTHpAndMp();
		}
		SceneEntryPk *pAtt = carrier.attacker->getTopMaster();
		if (pAtt&&pAtt->isEnemy(entry)&&carrier.status->range!=22)
		{
			SceneEntryPk * aMaster = pAtt->getTopMaster();
			SceneEntryPk * pMaster = entry->getTopMaster();
			if (aMaster->getType()==zSceneEntry::SceneEntry_Player)
			{
				SceneUser * a = (SceneUser *)aMaster;
				if (pMaster->getType()==zSceneEntry::SceneEntry_Player)
				{
					SceneUser * p = (SceneUser *)pMaster;
					ScenePk::checkProtect(a, p);
				}
			}
		}
		//entry->processDeath(carrier.attacker);
#ifdef _DEBUGLOG
		Zebra::logger->debug("[不走攻防计算公式]");
		Channel::sendSys(entry->tempid, Cmd::INFO_TYPE_SYS,"[非伤血性法术]");
#endif
	}
	entry->isPhysics = false;
	if (entry->reSendData) entry->changeAndRefreshHMS(true, sendData);
	return true;
}

/**
 * \brief 定时器刷新
 * \author fqnewman
 */
void SkillStatusManager::timer()
{
	std::map<DWORD, SkillStatusElement>::iterator tIterator, delIterator;
	bool dirty=false;
	bool sendData =false;

	for(tIterator = _activeElement.begin() ; tIterator !=_activeElement.end() ; )
	{
#ifdef _DEBUGLOG
		Zebra::logger->debug("[计时.伤]施加在身上的第[%u]号状态剩下时间[%u]",tIterator->second.id,tIterator->second.dwTime);
		Channel::sendSys(entry->tempid, Cmd::INFO_TYPE_SYS,"[计时.伤]施加在身上的第[%u]号状态剩下时间[%u]",tIterator->second.id, tIterator->second.dwTime);
#endif
		if (tIterator->second.dwTime>0)
		{
			tIterator->second.dwTime --;
		}
		else 
		{
			tIterator->second.dwTime = 0;
		}

		tIterator->second.byStep = ACTION_STEP_TIMER;
		runStatusElement(tIterator->second);

		if(0 == tIterator->second.dwTime)
		{
			tIterator->second.byStep = ACTION_STEP_STOP;
			runStatusElement(tIterator->second);
			if (tIterator->second.refresh) sendData = true;
			if (tIterator->second.state >0)
			{
				//entry->clearEffectStatus(tIterator->second.state);
				entry->showCurrentEffect(tIterator->second.state, false);
			}
#ifdef _DEBUGLOG
		Zebra::logger->debug("[伤害状态]施加在身上的第[%u]号状态被删除",tIterator->second.id);
		Channel::sendSys(entry->tempid, Cmd::INFO_TYPE_SYS,"[伤害状态]施加在身上的第[%u]号状态被删除",tIterator->second.id);
#endif
			delIterator = tIterator;
			tIterator ++;
			_activeElement.erase(delIterator->first);
			dirty = true;
		}
		else
		{
			tIterator ++;
		}
	}

	for(tIterator = _recoveryElement.begin() ; tIterator !=_recoveryElement.end();)
	{
#ifdef _DEBUGLOG
		Zebra::logger->debug("[计时.临]施加在身上的第[%u]号状态剩余时间为[%u]",tIterator->second.id,tIterator->second.dwTime);
		time_t tempValue = (time_t)(tIterator->second.qwTime/1000);
		Channel::sendSys(entry->tempid, Cmd::INFO_TYPE_SYS,"[计时.临]施加在身上的第[%u]号状态剩余时间为[%s]",tIterator->second.id,ctime(&tempValue));
#endif
		QWORD curQtime = SceneTimeTick::currentTime.msecs();
		if(curQtime >= tIterator->second.qwTime)
		{
			tIterator->second.byStep = ACTION_STEP_STOP;
			runStatusElement(tIterator->second);
			if (tIterator->second.refresh) sendData = true;
			if (tIterator->second.state >0)
			{
				//entry->clearEffectStatus(tIterator->second.state);
				entry->showCurrentEffect(tIterator->second.state, false);
			}
#ifdef _DEBUGLOG
			Zebra::logger->debug("[临时被动]施加在身上的第[%u]号状态被删除",tIterator->second.id);
			Channel::sendSys(entry->tempid, Cmd::INFO_TYPE_SYS,"[临时被动]施加在身上的第[%u]号状态被删除",tIterator->second.id);
#endif
			delIterator = tIterator;
			tIterator ++;
			_recoveryElement.erase(delIterator->first);
			dirty = true;
		}
		else
		{
			tIterator->second.dwTime = (tIterator->second.qwTime - curQtime)/1000;
			tIterator ++;
		}
	}
	if (entry->notifyHMS)
	{
		entry->attackRTHpAndMp();
	}
	if (dirty||entry->reSendData)
	{
		entry->changeAndRefreshHMS(false, sendData);
	}
	//std::map<DWORD, SkillStatusElement>::iterator tIterator, delIterator;
	if (bclearActiveSkillStatus)
	{
		for(tIterator = _activeElement.begin() ; tIterator !=_activeElement.end() ; )
		{
			tIterator->second.byStep = ACTION_STEP_CLEAR;
			runStatusElement(tIterator->second);
			if (tIterator->second.state >0)
			{
				//entry->clearEffectStatus(tIterator->second.state);
				entry->showCurrentEffect(tIterator->second.state, false);
			}
			delIterator = tIterator;
			tIterator ++;
			_activeElement.erase(delIterator->first);
	
		}

		for(tIterator = _recoveryElement.begin() ; tIterator !=_recoveryElement.end();)
		{
			tIterator->second.byStep = ACTION_STEP_CLEAR;
			runStatusElement(tIterator->second);
			if (tIterator->second.state >0)
			{
				//entry->clearEffectStatus(tIterator->second.state);
				entry->showCurrentEffect(tIterator->second.state, false);
			}
			delIterator = tIterator;
			tIterator ++;
			_recoveryElement.erase(delIterator->first);
		}
		bclearActiveSkillStatus = false;
	}
}

/**
 * \brief 重新运行被动状态，包括永久被动和临时被动
 * \author fqnewman
 */
void SkillStatusManager::processPassiveness()
{
	std::map<DWORD, SkillStatusElement>::iterator tIterator;

	for(tIterator = _recoveryElement.begin() ; tIterator !=_recoveryElement.end() ; tIterator ++)
	{
		tIterator->second.byStep = ACTION_STEP_DOPASS;
		if (tIterator->second.percent<100) tIterator->second.percent=100;
		runStatusElement(tIterator->second);
	}

	for(tIterator = _passivenessElement.begin() ; tIterator !=_passivenessElement.end() ; tIterator ++)
	{
		tIterator->second.byStep = ACTION_STEP_DOPASS;
		if (tIterator->second.percent<100&&tIterator->second.id!=180) tIterator->second.percent=100;
		runStatusElement(tIterator->second);
	}
}

/**
 * \brief 执行一个具体的状态
 * \author 王海军，fqnewman
 * \return 状态返回值
			SKILL_ACTIVE		//	加到活动MAP中
			SKILL_RECOVERY		//	加到临时被动MAP中
			SKILL_PASSIVENESS	//	加到永久被动MAP中
			SKILL_RETURN		//	返回不做任何操作
			SKILL_DONOW			//	立即执行属性值扣除动作
 */
BYTE SkillStatusManager::runStatusElement(SkillStatusElement &element)
{
	return  funlist[element.id>SKILLSTATENUMBER?0:element.id].func(entry,element);
}

/**
 * \brief  清除人物身上的不良的非永久状态
 * \author fqnewman
 */
void SkillStatusManager::clearBadActiveSkillStatus()
{
	std::map<DWORD, SkillStatusElement>::iterator tIterator, delIterator;

	for(tIterator = _activeElement.begin() ; tIterator !=_activeElement.end() ; )
	{
		if (tIterator->second.byGoodnessType == SKILL_BAD)
		{
			tIterator->second.byStep = ACTION_STEP_CLEAR;
			runStatusElement(tIterator->second);
			if (tIterator->second.state >0)
			{
				//entry->clearEffectStatus(tIterator->second.state);
				entry->showCurrentEffect(tIterator->second.state, false);
			}
			delIterator = tIterator;
			tIterator ++;
			_activeElement.erase(delIterator->first);
		}
		else
		{
			tIterator ++;
		}
	}

	for(tIterator = _recoveryElement.begin() ; tIterator !=_recoveryElement.end();)
	{
		if (tIterator->second.byGoodnessType == SKILL_BAD)
		{
			tIterator->second.byStep = ACTION_STEP_CLEAR;
			runStatusElement(tIterator->second);
			if (tIterator->second.state >0)
			{
				//entry->clearEffectStatus(tIterator->second.state);
				entry->showCurrentEffect(tIterator->second.state, false);
			}
			delIterator = tIterator;
			tIterator ++;
			_recoveryElement.erase(delIterator->first);
		}
		else
		{
			tIterator ++;
		}
	}
}

/**
 * \brief  清除人物身上的非永久性状态
 * \author fqnewman
 */
void SkillStatusManager::clearActiveSkillStatus()
{
	bclearActiveSkillStatus = true;
}

/**
 * \brief  清除人物身上的非永久性状态
 * \author fqnewman
 */
void SkillStatusManager::clearActiveSkillStatusOnlyUseToStatus48()
{
	std::map<DWORD, SkillStatusElement>::iterator tIterator, delIterator;

	for(tIterator = _activeElement.begin() ; tIterator !=_activeElement.end() ; )
	{
		tIterator->second.byStep = ACTION_STEP_CLEAR;
		runStatusElement(tIterator->second);
		if (tIterator->second.state >0)
		{
			//entry->clearEffectStatus(tIterator->second.state);
			entry->showCurrentEffect(tIterator->second.state, false);
		}
		delIterator = tIterator;
		tIterator ++;
		_activeElement.erase(delIterator->first);

	}

	for(tIterator = _recoveryElement.begin() ; tIterator !=_recoveryElement.end();)
	{
		if (tIterator->second.id == 173)
		{ 
			tIterator++;
			continue; //不好意思特殊处理一下
		}
		tIterator->second.byStep = ACTION_STEP_CLEAR;
		runStatusElement(tIterator->second);
		if (tIterator->second.state >0)
		{
			//entry->clearEffectStatus(tIterator->second.state);
			entry->showCurrentEffect(tIterator->second.state, false);
		}
		delIterator = tIterator;
		tIterator ++;
		_recoveryElement.erase(delIterator->first);
	}
	this->entry->changeAndRefreshHMS();
}


/**
 * \brief  清除指定技能ID的技能状态
 * \author fqnewman
 */
void SkillStatusManager::clearSkill(DWORD dwSkillID)
{
	std::map<DWORD, SkillStatusElement>::iterator tIterator, delIterator;

	for(tIterator = _activeElement.begin() ; tIterator !=_activeElement.end() ; )
	{
		if (tIterator->second.dwSkillID == dwSkillID)
		{
			tIterator->second.byStep = ACTION_STEP_CLEAR;
			runStatusElement(tIterator->second);
			if (tIterator->second.state >0)
			{
				//entry->clearEffectStatus(tIterator->second.state);
				entry->showCurrentEffect(tIterator->second.state, false);
			}
			delIterator = tIterator;
			tIterator ++;
			_activeElement.erase(delIterator->first);
		}
		else
		{
			tIterator++;
		}

	}

	for(tIterator = _recoveryElement.begin() ; tIterator !=_recoveryElement.end();)
	{
		if (tIterator->second.dwSkillID == dwSkillID)
		{
			tIterator->second.byStep = ACTION_STEP_CLEAR;
			runStatusElement(tIterator->second);
			if (tIterator->second.state >0)
			{
				//entry->clearEffectStatus(tIterator->second.state);
				entry->showCurrentEffect(tIterator->second.state, false);
			}
			delIterator = tIterator;
			tIterator ++;
			_recoveryElement.erase(delIterator->first);
		}
		else
		{
			tIterator++;
		}
	}
	this->entry->changeAndRefreshHMS();
}


/**
 * \brief 按被动技能来处理这个操作
 * \param skillid 技能id
 * \param pSkillStatus 技能中的操作
 * \author fqnewman
 */
void SkillStatusManager::putPassivenessOperationToMe(const DWORD skillid, const SkillStatus *pSkillStatus)
{
	std::vector<SkillElement>::const_iterator tIterator;

	for (tIterator = pSkillStatus->_StatusElementList.begin();
		tIterator != pSkillStatus->_StatusElementList.end();
		tIterator++)
	{
		SkillStatusElement element;
		element.id		=	tIterator->id;
		element.percent =	tIterator->percent;
		element.value	=	tIterator->value;
		element.state	=	tIterator->state;
		element.dwSkillID = skillid;
		element.dwTime = tIterator->time;
		_passivenessElement[element.id]=element;

#ifdef _DEBUGLOG
			Zebra::logger->debug("[永久被动]之[%u]号状态被施加在身上",element.id);
			Channel::sendSys(entry->tempid, Cmd::INFO_TYPE_SYS,"[永久被动]之[%u]号状态被施加在身上",element.id);
#endif
	}
}

/**
 * \brief  增加不良状态的持续时间数值
 * \author fqnewman
 */
void SkillStatusManager::addBadSkillStatusPersistTime(const DWORD &value)
{
	std::map<DWORD, SkillStatusElement>::iterator tIterator;

	for(tIterator = _activeElement.begin() ; tIterator !=_activeElement.end() ; tIterator ++)
	{
		if (tIterator->second.byGoodnessType == SKILL_BAD)
		{
			tIterator->second.dwTime += value;
		}
	}

	for(tIterator = _recoveryElement.begin() ; tIterator !=_recoveryElement.end(); tIterator++)
	{
		if (tIterator->second.byGoodnessType == SKILL_BAD)
		{
			tIterator->second.dwTime += value;
			tIterator->second.qwTime += value;
		}
	}
}

/**
 * \brief  增加不良状态的持续时间百分比
 * \author fqnewman
 */
void SkillStatusManager::addBadSkillStatusPersistTimePercent(const DWORD &value)
{
	std::map<DWORD, SkillStatusElement>::iterator tIterator;

	for(tIterator = _activeElement.begin() ; tIterator !=_activeElement.end() ; tIterator ++)
	{
		if (tIterator->second.byGoodnessType == SKILL_BAD)
		{
			tIterator->second.dwTime = (DWORD)(tIterator->second.dwTime*(value/100.0f));
		}
	}

	for(tIterator = _recoveryElement.begin() ; tIterator !=_recoveryElement.end(); tIterator++)
	{
		if (tIterator->second.byGoodnessType == SKILL_BAD)
		{
			tIterator->second.dwTime = (DWORD)(tIterator->second.dwTime*(value/100.0f));
			tIterator->second.qwTime = (DWORD)(tIterator->second.qwTime*(value/100.0f));
		}
	}
}

/**
 * \brief  清除持续状态中的指定类别
 * \param byMutexType 技能大类
 * \param myMap 操作的状态map
 * \param dwID 过滤状态id
 * \author fqnewman
 */
void SkillStatusManager::clearMapElement(const BYTE &byMutexType, std::map<DWORD, SkillStatusElement> &myMap, DWORD dwID,bool notify)
{
/*	std::map<DWORD, SkillStatusElement>::iterator tIterator, delIterator;

	for(tIterator = myMap.begin() ; tIterator !=myMap.end();)
	{
		if (tIterator->second.byMutexType == byMutexType)
		{
			if (tIterator->second.id != dwID)
			{
				tIterator->second.byStep = ACTION_STEP_CLEAR;
				runStatusElement(tIterator->second);
			}
			if (tIterator->second.state >0)
			{
				//entry->clearEffectStatus(tIterator->second.state);
				entry->showCurrentEffect(tIterator->second.state, false);
			}
			delIterator = tIterator;
			tIterator ++;
			myMap.erase(delIterator->first);
		}
		else
		{
			tIterator ++;
		}
	}*/
	std::map<DWORD, SkillStatusElement>::iterator tIterator, delIterator;

	for(tIterator = myMap.begin() ; tIterator !=myMap.end();)
	{
		if (tIterator->second.id == byMutexType)
		{
			if (tIterator->second.id != dwID)
			{
				tIterator->second.byStep = ACTION_STEP_CLEAR;
				runStatusElement(tIterator->second);
			}
			if (tIterator->second.state >0)
			{
				//entry->clearEffectStatus(tIterator->second.state);
				entry->showCurrentEffect(tIterator->second.state, false,notify);
			}
			delIterator = tIterator;
			tIterator ++;
			myMap.erase(delIterator->first);
		}
		else
		{
			tIterator ++;
		}
	}

}

void SkillStatusManager::clearRecoveryElement(DWORD value)
{
	clearMapElement(value,_recoveryElement, 0);
}

void SkillStatusManager::clearActiveElement(DWORD value)
{
	clearMapElement(value,_activeElement, 0);
}

void SkillStatusManager::processDeath()
{
	if (entry->bombskillId >0)
	{
		Cmd::stAttackMagicUserCmd cmd;

		cmd.byAttackType = Cmd::ATTACKTYPE_U2P;
		cmd.dwDefenceTempID = 0;
		cmd.dwUserTempID = entry->tempid;
		cmd.wdMagicType = entry->bombskillId;
		cmd.byAction = Cmd::Ani_Num;
		cmd.byDirect = entry->getDir();
		cmd.xDes = (WORD)entry->getPos().x;
		cmd.yDes = (WORD)entry->getPos().y;

		zSkill *s = NULL;

		s = zSkill::createTempSkill(entry , entry->bombskillId , 1);
		if(s)
		{
			s->action(&cmd, sizeof(cmd));
			SAFE_DELETE(s);
		}
	}

	clearActiveSkillStatus();
}


/**
 * \brief  测试函数用来显示技能状态当前值
 * \author fqnewman
 */
void SkillStatusManager::showValue()
{
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "角色身上的[SkillValue]属性列表:---------------------------");
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "伤害值增加固定数值1=%ld",entry->skillValue.dvalue);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "伤害值增加x%2=%ld",entry->skillValue.dvaluep);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "物理防御数值变更 57，86=%ld",entry->skillValue.pdefence);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "物理防御变更百分比=%ld",entry->skillValue.pdefencep);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "变为随机小动物79=%ld",entry->skillValue.topet);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "产生额外伤害83=%ld",entry->skillValue.appenddam);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "移动速度变更百分比 16，56=%ld",entry->skillValue.movespeed);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "减少技能施放间隔17=%ld",entry->skillValue.mgspeed);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "减少陷入冰冻状态几率18=%ld",entry->skillValue.coldp);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "减少陷入中毒状态几率19=%ld",entry->skillValue.poisonp);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "减少陷入石化状态几率20=%ld",entry->skillValue.petrifyp);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "减少陷入失明状态几率21=%ld",entry->skillValue.blindp);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "减少陷入混乱状态几率22=%ld",entry->skillValue.chaosp);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "命中率增加33,64=%ld",entry->skillValue.atrating);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "命中率增加33,64=%ld",entry->skillValue.reduce_atrating);

	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "生命值恢复速度增加34=%ld",entry->skillValue.hpspeedup);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "法术值恢复速度增加35=%ld",entry->skillValue.mpspeedup);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "体力值恢复速度增加36=%ld",entry->skillValue.spspeedup);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "闪避率上升37=%ld",entry->skillValue.akdodge);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "反弹45=%ld",entry->skillValue.reflect);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "反弹x%46=%ld",entry->skillValue.reflectp);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "为反弹百分之几的敌人伤害50=%ld",entry->skillValue.reflect2);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "法术防御变更59，=%ld",entry->skillValue.mdefence);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "法术防御变更百分比x%=%ld",entry->skillValue.mdefencep);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "攻击速度变更80，81=%ld",entry->skillValue.uattackspeed);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "降低陷入七大状态几率82=%ld",entry->skillValue.sevendownp);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "伤害转移百分比=%ld",entry->skillValue.tsfdamp);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "被动:额外伤害=%ld",entry->skillValue.passdam);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "角色身上的[PkValue]属性列表:---------------------------");
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "物理攻击力=%ld",entry->pkValue.pdamage);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "物理防御力=%ld",entry->pkValue.pdefence);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "法术攻击力=%ld",entry->pkValue.mdamage);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "法术防御力=%ld",entry->pkValue.mdefence);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "消耗法术值=%ld",entry->pkValue.mcost);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "消耗生命值=%ld",entry->pkValue.hpcost);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "消耗体力值=%ld",entry->pkValue.spcost);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "经验消耗=%ld",entry->pkValue.exp);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "伤害值=%ld",entry->pkValue.dvalue);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "伤害值增加百分比=%ld",entry->pkValue.dvaluep);
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "列表结束:----------------------------------------------");
}

/**
 * \brief  打印技能状态当前值到log文件中
 * \author fqnewman
 */
void SkillStatusManager::showValueToLog()
{
	Zebra::logger->debug("角色身上的[SkillValue]属性列表:---------------------------");
	Zebra::logger->debug("伤害值增加固定数值1=%ld",entry->skillValue.dvalue);
	Zebra::logger->debug("伤害值增加x%2=%ld",entry->skillValue.dvaluep);
	Zebra::logger->debug("物理防御数值变更 57，86=%ld",entry->skillValue.pdefence);
	Zebra::logger->debug("物理防御变更百分比=%ld",entry->skillValue.pdefencep);
	Zebra::logger->debug("变为随机小动物79=%ld",entry->skillValue.topet);
	Zebra::logger->debug("产生额外伤害83=%ld",entry->skillValue.appenddam);
	Zebra::logger->debug("移动速度变更百分比 16，56=%ld",entry->skillValue.movespeed);
	Zebra::logger->debug("减少技能施放间隔17=%ld",entry->skillValue.mgspeed);
	Zebra::logger->debug("减少陷入冰冻状态几率18=%ld",entry->skillValue.coldp);
	Zebra::logger->debug("减少陷入中毒状态几率19=%ld",entry->skillValue.poisonp);
	Zebra::logger->debug("减少陷入石化状态几率20=%ld",entry->skillValue.petrifyp);
	Zebra::logger->debug("减少陷入失明状态几率21=%ld",entry->skillValue.blindp);
	Zebra::logger->debug("减少陷入混乱状态几率22=%ld",entry->skillValue.chaosp);
	Zebra::logger->debug("命中率增加33,64=%ld",entry->skillValue.atrating);
	Zebra::logger->debug("生命值恢复速度增加34=%ld",entry->skillValue.hpspeedup);
	Zebra::logger->debug("法术值恢复速度增加35=%ld",entry->skillValue.mpspeedup);
	Zebra::logger->debug("体力值恢复速度增加36=%ld",entry->skillValue.spspeedup);
	Zebra::logger->debug("闪避率上升37=%ld",entry->skillValue.akdodge);
	Zebra::logger->debug("反弹45=%ld",entry->skillValue.reflect);
	Zebra::logger->debug("反弹x%46=%ld",entry->skillValue.reflectp);
	Zebra::logger->debug("为反弹百分之几的敌人伤害50=%ld",entry->skillValue.reflect2);
	Zebra::logger->debug("法术防御变更59，=%ld",entry->skillValue.mdefence);
	Zebra::logger->debug("法术防御变更百分比x%=%ld",entry->skillValue.mdefencep);
	Zebra::logger->debug("攻击速度变更80，81=%ld",entry->skillValue.uattackspeed);
	Zebra::logger->debug("降低陷入七大状态几率82=%ld",entry->skillValue.sevendownp);
	Zebra::logger->debug("伤害转移百分比=%ld",entry->skillValue.tsfdamp);
	Zebra::logger->debug("被动:额外伤害=%ld",entry->skillValue.passdam);
	Zebra::logger->debug("角色身上的[PkValue]属性列表:---------------------------");
	Zebra::logger->debug("物理攻击力=%ld",entry->pkValue.pdamage);
	Zebra::logger->debug("物理防御力=%ld",entry->pkValue.pdefence);
	Zebra::logger->debug("法术攻击力=%ld",entry->pkValue.mdamage);
	Zebra::logger->debug("法术防御力=%ld",entry->pkValue.mdefence);
	Zebra::logger->debug("消耗法术值=%ld",entry->pkValue.mcost);
	Zebra::logger->debug("消耗生命值=%ld",entry->pkValue.hpcost);
	Zebra::logger->debug("消耗体力值=%ld",entry->pkValue.spcost);
	Zebra::logger->debug("经验消耗=%ld",entry->pkValue.exp);
	Zebra::logger->debug("伤害值=%ld",entry->pkValue.dvalue);
	Zebra::logger->debug("伤害值增加百分比=%ld",entry->pkValue.dvaluep);
	Zebra::logger->debug("列表结束:----------------------------------------------");
}

/**
 * \brief  显示角色身上的主动技能状态
 * \author fqnewman
 */
void SkillStatusManager::showActive()
{
	std::map<DWORD, SkillStatusElement>::iterator tIterator;
	SkillStatusElement *element;

	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "角色身上的主动状态列表:");
	for(tIterator = _activeElement.begin() ; tIterator !=_activeElement.end(); tIterator++)
	{
		std::string myname;
		char buf [45];
		element = &tIterator->second;
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(element->dwTempID);
		if (pUser)
			myname = pUser->name;
		else
		{
			sprintf(buf,"临时ID:%u", element->dwTempID);
			myname = buf;
		}
		Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "技能:%u 大类:%u 状态类别:%u %s 剩余时间:%u 攻击者:%s 几率:%u 总时间:%u 状态值%u 特效值:%u",element->dwSkillID,element->byMutexType,element->id, element->byGoodnessType==1?"伤害":"和平",element->dwTime,myname.c_str(), element->percent, element->dwTime,element->value, element->state);
	}
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "----------------------------------------------");
}

/**
 * \brief  显示角色身上的临时被动技能状态
 * \author fqnewman
 */
void SkillStatusManager::showRecovery()
{
	std::map<DWORD, SkillStatusElement>::iterator tIterator;
	SkillStatusElement *element;

	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "角色身上的临时被动状态列表:");
	for(tIterator = _recoveryElement.begin() ; tIterator !=_recoveryElement.end(); tIterator++)
	{
		std::string myname;
		char buf [45];
		element = &tIterator->second;
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(element->dwTempID);
		if (pUser)
			myname = pUser->name;
		else
		{
			sprintf(buf,"临时ID:%u", element->dwTempID);
			myname = buf;
		}
		Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "技能:%u 大类:%u 状态类别:%u %s 剩余时间:%u 攻击者:%s 几率:%u 总时间:%u 状态值%u 特效值:%u",element->dwSkillID,element->byMutexType,element->id, element->byGoodnessType==1?"伤害":"和平",element->dwTime,myname.c_str(), element->percent, element->dwTime,element->value, element->state);
	}
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "----------------------------------------------");
}

/**
 * \brief  显示角色身上的永久被动技能状态
 * \author fqnewman
 */
void SkillStatusManager::showPassiveness()
{
	std::map<DWORD, SkillStatusElement>::iterator tIterator;
	SkillStatusElement *element;

	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "角色身上的永久被动状态列表:");
	for(tIterator = _passivenessElement.begin() ; tIterator !=_passivenessElement.end(); tIterator++)
	{
		std::string myname;
		char buf [45];
		element = &tIterator->second;
		SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(element->dwTempID);
		if (pUser)
			myname = pUser->name;
		else
		{
			sprintf(buf,"临时ID:%u", element->dwTempID);
			myname = buf;
		}
		Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "技能:%u 大类:%u 状态类别:%u %s 剩余时间:%u 攻击者:%s 几率:%u 总时间:%u 状态值%u 特效值:%u",element->dwSkillID,element->byMutexType,element->id, element->byGoodnessType==1?"伤害":"和平",element->dwTime,myname.c_str(), element->percent, element->dwTime,element->value, element->state);
	}
	Channel::sendSys((SceneUser *)entry, Cmd::INFO_TYPE_SYS, "----------------------------------------------");
}

/**
 * \brief  获得当前角色所携带的技能状态数目
 */
WORD SkillStatusManager::getSaveStatusSize()
{
	WORD ret =0;

	ret = _recoveryElement.size()+_activeElement.size();
	return ret; 
}

/**
* \brief sky 系统为用户设置一个状态到临时被动状态管理器里
*/
void SkillStatusManager::SetStatuToRecovery(DWORD id, DWORD time, DWORD percent, DWORD value, DWORD state)
{
	SkillStatusElement element;
	element.byStep  =  ACTION_STEP_START;
	element.id    =  id;
	element.percent =  percent;
	element.value  =  value;
	element.state  =  state;

	element.dwSkillID = 0;
	element.dwTime = time;
	element.qwTime = SceneTimeTick::currentTime.msecs()+element.dwTime *1000;

	switch(runStatusElement(element))
	{
	case SKILL_RECOVERY:
		{
#ifdef _DEBUG
			Zebra::logger->debug("[临时被动]第[%u]号状态被施加在身上持续时间为[%u]",element.id,element.dwTime);
			Channel::sendSys(entry->tempid,Cmd::INFO_TYPE_SYS,"[临时被动]第[%u]号状态被施加在身上持续时间为[%u]",element.id,element.dwTime);
#endif
			element.qwTime = SceneTimeTick::currentTime.msecs()+element.dwTime *1000;
			if (element.state >0)
			{
				clearMapElement(element.id/*element.byMutexType*/,_recoveryElement,element.id,false);
				_recoveryElement[element.id/*element.byMutexType*/]=element;
				sendSelectStates(entry,element.state,element.value,element.dwTime);
				//entry->setEffectStatus(element.state);
				entry->showCurrentEffect(element.state,true);
			}
			else
			{
				clearMapElement(element.id/*element.byMutexType*/,_recoveryElement,element.id);
				_recoveryElement[element.id/*element.byMutexType*/]=element;
			}
		}
		break;
	case SKILL_BREAK:
		{
			return;
		}
		break;
	case SKILL_ACTIVE:
		{
#ifdef _DEBUG
			Zebra::logger->debug("[伤害状态]第[%u]号状态被施加在身上持续时间为[%u]",element.id,element.dwTime);
			Channel::sendSys(entry->tempid,Cmd::INFO_TYPE_SYS,"[攻击状态]第[%u]号状态被施加在身上持续时间为[%u]",element.id,element.dwTime);
#endif
			if (element.dwTime==0) break;
			clearMapElement(element.id/*element.byMutexType*/,_activeElement,element.id);
			_activeElement[element.id/*element.byMutexType*/]=element;
			if (element.state >0)
			{
				sendSelectStates(entry,element.state,element.value,element.dwTime);
				//entry->setEffectStatus(element.state);
				entry->showCurrentEffect(element.state,true);
			}
		}
		break;
	case SKILL_RETURN:
	default:
		break;
	}

	Channel::sendSys(entry->tempid,Cmd::INFO_TYPE_SYS,"[免疫类状态]之[%u]号状态被施加在身上",element.id);
}
