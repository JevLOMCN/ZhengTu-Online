/**
 * \file
 * \version  $Id: SceneNpcDare.cpp $
 * \author  
 * \date 
 * \brief NPC争夺战处理模块
 *
 * 
 */

#include "SceneUser.h"
#include "Scene.h"
#include "RecordClient.h"
#include "Chat.h"
#include "SessionCommand.h"
#include "SessionClient.h"
#include "NpcDare.h"

using namespace NpcDareDef;

/**
 * \brief 比较交战物品
 *
 */

class NpcDareObjectCompare:public UserObjectCompare 
{
	public:
		DWORD  dwObjectID;

		bool isIt(zObject *object)
		{
			if (object->data.dwObjectID == dwObjectID) return true;
			return false;
		}
};



/**
 * \brief 执行挑战指令
 *
 *
 * \param rev 挑战指令
 * \param cmdLen 消息长度
 * \return 是否成功
 */
bool SceneUser::doNpcDareCmd(const Cmd::stDareUserCmd *rev,unsigned int cmdLen)
{
#ifdef _DEBUGLOG
       Zebra::logger->debug("doNpcDareCmd处理[%d]号消息", rev->byParam);
#endif 
	switch (rev->byParam)
	{
		case Cmd::QUESTION_NPCDARE_INFO_PARA:
			{
				Cmd::Session::t_questionNpcDare_SceneSession send;
				Cmd::stQuestionNpcDareInfo* ptCmd = (Cmd::stQuestionNpcDareInfo*)rev;
					
				send.dwUserID = this->id;
				send.dwCountryID = this->scene->getCountryID();
				send.dwMapID = this->scene->getRealMapID();
				send.dwNpcID = ptCmd->dwNpcID;
				send.byType = ptCmd->byType;
				sessionClient->sendCmd(&send, sizeof(send));
				return true;
			}
			break;
		case Cmd::NPCDARE_DARE_PARA:
			{
				Cmd::stDareNpcDare * pCmd = (Cmd::stDareNpcDare *) rev;
				SceneNpc *pNpc = SceneNpcManager::getMe().getNpcByTempID(pCmd->dwNpcID);
				if (pNpc)
				{
					if (scene->checkTwoPosIInNine(getPosI(),pNpc->getPosI()))
					{
						if (scene->getCountryID() == this->charbase.country)
						{
							if (this->charbase.septid !=0)
							{
								/*
								NpcDareObjectCompare found;
								found.dwObjectID = CREATE_NPCDARE_NEED_ITEM;
								zObject *itemobj = packs.uom.getObject(found);// 查找道具
								if (itemobj)
								{
									packs.removeObject(itemobj); //notify and delete
									Cmd::Session::t_NpcDare_Dare_SceneSession send;

									send.dwCountryID = scene->getCountryID();
									send.dwMapID = scene->getRealMapID();
									send.dwNpcID = pNpc->npc->id;
									send.dwUserID = this->id;
									sessionClient->sendCmd(&send, sizeof(send));
									return true;
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "缺少挑战发起道具！");
								}
								*/
								if (this->packs.checkMoney(4000))
								{
									Cmd::Session::t_NpcDare_Dare_SceneSession send;

									send.dwCountryID = scene->getCountryID();
									send.dwMapID = scene->getRealMapID();
									send.dwNpcID = pNpc->npc->id;
									send.dwUserID = this->id;
									sessionClient->sendCmd(&send, sizeof(send));
									return true;
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "缺少足够的钱来发起挑战！");
								}
							}
							else
							{
								Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你还没有家族不能发起挑战！");
							}
						}
						else
						{
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "不在自己国家无法发起挑战！");
						}
					}
					else
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "距离太远，无法对话！");
					}
				}
				return true;
			}
			break;
		case Cmd::NPCDARE_GETGOLD_PARA:
			{
				Cmd::stDareNpcDare * pCmd = (Cmd::stDareNpcDare *) rev;
				Cmd::Session::t_NpcDare_GetGold_SceneSession send;
				send.dwUserID = this->id;
				send.dwNpcID = pCmd->dwNpcID;
				send.dwCountryID = this->scene->getCountryID();
				send.dwMapID = this->scene->getRealMapID();
				sessionClient->sendCmd(&send, sizeof(send));

				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

