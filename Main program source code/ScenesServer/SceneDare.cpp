/**
 * \file
 * \version  $Id: SceneDare.cpp  $
 * \author  
 * \date
 * \brief 实现交战处理的类
 *
 * 
 */

#include "SceneUser.h"
#include "Scene.h"
#include "RecordClient.h"
#include "Chat.h"
#include "SessionCommand.h"
#include "SessionClient.h"
#include "Dare.h"

using namespace DareDef;

/**
 * \brief 比较交战物品
 *
 */
/*
class DareObjectCompare:public UserObjectCompare 
{
	public:
		DWORD  dwObjectID;

		bool isIt(zObject *object)
		{
			if (object->data.dwObjectID == dwObjectID) return true;
			return false;
		}
};
*/


/**
 * \brief 执行挑战指令
 *
 *
 * \param rev 挑战指令
 * \param cmdLen 消息长度
 * \return 是否成功
 */
bool SceneUser::doDareCmd(const Cmd::stDareUserCmd *rev,unsigned int cmdLen)
{
	switch (rev->byParam)
	{
		case Cmd::ACTIVE_UNION_CITY_DARE_PARA:
			{
				Cmd::Session::t_UnionCity_Dare_SceneSession send;
				Cmd::stActiveUnionCityDare* cmd = (Cmd::stActiveUnionCityDare*)rev;
				int need_money = 0;
				if(this->scene->getCountryID() == PUBLIC_COUNTRY)
				{
					need_money = CREATE_UNION_NEUTRAL_CITY_DARE_NEED_PRICE_MONEY;
				} 
				else	if ( this->scene->getRealMapID() == 139)
				{
					need_money = CREATE_UNION_KING_CITY_DARE_NEED_PRICE_MONEY;
				}
				else
				{
					need_money = CREATE_UNION_CITY_DARE_NEED_PRICE_MONEY;
				}
				
				if (this->charbase.unionid == 0)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "你还没有建立帮会，不能发起挑战");
					return true;
				}

				if (packs.checkMoney(need_money) 
					&& packs.removeMoney(need_money, "夺城战扣除挑战费用")) 
				{
					Zebra::logger->trace("[夺城战]:%s 扣除帮会夺城战挑战费用 %d", 
							this->name, need_money);	

					send.dwCountryID = this->charbase.country;

					if (cmd->toCountryID == 0 || cmd->toCountryID>20)
					{
						send.dwToCountryID = this->scene->getCountryID();
					}
					else
					{
						send.dwToCountryID = cmd->toCountryID;
					}

					send.dwCityID = this->scene->getRealMapID();
					send.dwFromUserID = this->id;
					send.dwFromUnionID = this->charbase.unionid; 
					sessionClient->sendCmd(&send, sizeof(send));
				}
				else
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "银两不够，不能发起挑战");
				}

				return true;
			}
			break;
		/*case Cmd::ACTIVE_DARE_PARA:
			{
				Cmd::stActiveDareCmd *pCmd = (Cmd::stActiveDareCmd *)rev;

				Cmd::Session::t_activeDare_SceneSession send;
				send.dwWarID = pCmd->dwWarID;
				Zebra::logger->debug("收接到会话转发过来的ACTIVE_DARE_PARA命令");

				if (packs.checkMoney(CREATE_DARE_NEED_PRICE_GOLD) && packs.removeMoney(CREATE_DARE_NEED_PRICE_GOLD)) {
					send.dwStatus = Cmd::Session::SCENE_ACTIVEDARE_SUCCESS;
				}
				else
				{
					//报告没有足够的钱
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "没有足够的钱来支付挑战费用！");
					send.dwStatus = Cmd::Session::SCENE_ACTIVEDARE_FAIL;
				}

				sessionClient->sendCmd(&send, sizeof(Cmd::Session::t_activeDare_SceneSession));



				return true;
			}
			break;*/
		default:
			break;
	}
	return false;
}

