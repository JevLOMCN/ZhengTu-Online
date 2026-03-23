#include "Scene.h"
#include "SceneUser.h"
#include "SceneUserManager.h"
#include "Chat.h"
#include "Dice.h"

bool SceneUser::doMiniGameCmd(const Cmd::stMiniGameUserCmd *cmd,unsigned int cmdLen)
{
	using namespace Cmd;
	switch (cmd->byParam)
	{
		case INVITE_MINI_PARA:
			{
				stInviteMiniGame * rev = (stInviteMiniGame *)cmd;

				if (miniGame)
				{
					Channel::sendSys(this, INFO_TYPE_FAIL, "请先结束你现在进行的游戏");
					return true;
				}

				if (!packs.checkMoney(rev->money))
				{
					Channel::sendSys(this, INFO_TYPE_FAIL, "你没有那么多钱");
					return true;
				}

				if (rev->money<100 || rev->money>10000)
				{
					Channel::sendSys(this, INFO_TYPE_FAIL, "押金必须在1两到1锭之间");
					return true;
				}

				SceneUser * pUser = SceneUserManager::getMe().getUserByTempID(rev->tempid);

				if (!pUser || !scene->checkTwoPosIInNine(getPosI(), pUser->getPosI()))
				{
					Channel::sendSys(this, INFO_TYPE_FAIL, "对方不在附近，不能和你一起玩");
					return true;
				}

				if(!isset_state(pUser->sysSetting , Cmd::USER_SETTING_MINIGAME))
				{
					Channel::sendSys(this, INFO_TYPE_FAIL, "%s 不和任何人玩小游戏", pUser->name);
					return true;
				}

				if (pUser->miniGame)
				{
					Channel::sendSys(this, INFO_TYPE_FAIL, "%s 已经和别人玩开了", pUser->name);
					return true;
				}

				rev->tempid = tempid;
				pUser->sendCmdToMe(rev, cmdLen);
				return true;
			}
			break;
		case INVITE_RET_MINI_PARA:
			{
				stInviteRetMiniGame * rev = (stInviteRetMiniGame *)cmd;
				SceneUser * pUser = SceneUserManager::getMe().getUserByTempID(rev->tempid);
				if (!pUser || !scene->checkTwoPosIInNine(getPosI(), pUser->getPosI()))
				{
					Channel::sendSys(this, INFO_TYPE_FAIL, "对方已经走远了，不能和你一起玩");
					return true;
				}

				if (0==rev->ret)//不同意玩
				{
					Channel::sendSys(pUser, INFO_TYPE_FAIL, "%s 不同意和你玩游戏", name);
					return true;
				}

				if (!packs.checkMoney(rev->money))
				{
					Channel::sendSys(this, INFO_TYPE_FAIL, "你没有那么多钱");
					Channel::sendSys(pUser, INFO_TYPE_FAIL, "%s 没有那么多钱", name);
					return true;
				}

				if (miniGame)
				{
					Channel::sendSys(pUser, INFO_TYPE_FAIL, "%s 已经和别人玩开了", name);
					return true;
				}

				if (pUser->miniGame)
				{
					Channel::sendSys(this, INFO_TYPE_FAIL, "%s 已经和别人玩开了", pUser->name);
					return true;
				}

				//创建游戏
				miniGame = pUser->miniGame = new Dice(this, pUser, rev->money);
				if (!miniGame) return false;

				stStartMiniGame send;
				send.money = rev->money;

				send.tempid = pUser->tempid;
				sendCmdToMe(&send, sizeof(send));
				send.tempid = tempid;
				pUser->sendCmdToMe(&send, sizeof(send));

				return true;
			}
			break;
		case STOP_DICE_MINI_PARA:
			{
				if (miniGame && Dice::DICE_STATE_ROLLING==miniGame->getState())
					miniGame->rotate(tempid);
				return true;
			}
			break;
		case END_MINI_PARA:
			{
				if (miniGame)
				{
					Dice * temp = miniGame;
					miniGame->endGame(this);

					delete temp;
					miniGame = 0;
				}
				return true;
			}
			break;
		case CONTINUE_MINI_PARA:
			{
				if (miniGame && Dice::DICE_STATE_END==miniGame->getState())
				{
					Cmd::stContinueMiniGame * rev = (Cmd::stContinueMiniGame *)cmd;
					if (0==rev->ret || !miniGame->setReady(this))
					{
						Dice * temp = miniGame;
						miniGame->endGame();

						delete temp;
						miniGame = 0;

						return true;
					}
				}
			}
			break;
		default:
			break;
	}
	return false;
}
