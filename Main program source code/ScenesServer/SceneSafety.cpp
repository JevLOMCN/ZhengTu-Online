/**
 * \file
 * \version  $Id: SceneUnion.cpp  $
 * \author  
 * \date 
 * \brief 实现财产保护命令的处理
 *
 */
#include "SceneUser.h"
#include "Scene.h"
#include "RecordClient.h"
#include "Chat.h"
#include "SessionCommand.h"
#include "SessionClient.h"
#include "Command.h"

/**
  * \brief 处理用户财产保护命令
  *
  *
  * \param rev: 命令
  * \param cmdLen: 命令长度
  *
  * \return 命令被处理返回TRUE,否则为FALSE
  *
  *
  */
bool SceneUser::doSafetyCmd(const Cmd::stSafetyUserCmd *rev,unsigned int cmdLen)
{
	switch(rev->byParam)
	{
		case SET_SAFETY_PARA:
			{
				Cmd::stSetSafetyUserCmd* cmd = (Cmd::stSetSafetyUserCmd*)rev;

				if (cmd->state>0)
				{
					this->safety = 1;
					this->safety_setup = 0;
					Cmd::set_state((unsigned char*)&this->safety_setup, Cmd::SAFE_SPLIT_OBJECT);
					Cmd::set_state((unsigned char*)&this->safety_setup, Cmd::SAFE_THIRD_PACK);
					Cmd::set_state((unsigned char*)&this->safety_setup, Cmd::SAFE_GOLD_STOCK);

					Cmd::stNotifySafetyUserCmd send;
					send.byState = Cmd::SAFETY_OPEN;
					send.safe_setup = this->safety_setup;
					sendCmdToMe(&send, sizeof(send));
				}

				this->save(Cmd::Record::OPERATION_WRITEBACK);
				return true;
			}
			break;
		case SET_TEMP_UNSAFETY_PARA:
			{
				this->temp_unsafety_state = 1;

				Cmd::stNotifySafetyUserCmd send;
				send.byState = Cmd::SAFETY_TEMP_CLOSE;
				send.safe_setup = this->safety_setup;

				sendCmdToMe(&send, sizeof(send));
				return true;
			}
			break;
		case SET_SAFETY_DETAIL_PARA:
			{
				Cmd::stNotifySafetyUserCmd send;
				Cmd::stSetSafetyDetailUserCmd* cmd = (Cmd::stSetSafetyDetailUserCmd*)rev;

				if (this->safety==1)
				{
					send.byState = Cmd::SAFETY_OPEN;

					if (this->temp_unsafety_state == 1)
						send.byState = Cmd::SAFETY_TEMP_CLOSE;

					this->safety_setup = 0;

					if (Cmd::isset_state((unsigned char*)&cmd->safe_setup, Cmd::SAFE_SPLIT_OBJECT))
					{
						Cmd::set_state((unsigned char*)&this->safety_setup, Cmd::SAFE_SPLIT_OBJECT);
					}

					if (Cmd::isset_state((unsigned char*)&cmd->safe_setup, Cmd::SAFE_THIRD_PACK))
					{
						Cmd::set_state((unsigned char*)&this->safety_setup, Cmd::SAFE_THIRD_PACK);
					}

					if (Cmd::isset_state((unsigned char*)&cmd->safe_setup, Cmd::SAFE_GOLD_STOCK))
					{
						Cmd::set_state((unsigned char*)&this->safety_setup, Cmd::SAFE_GOLD_STOCK);
					}

					send.safe_setup = this->safety_setup;
					sendCmdToMe(&send, sizeof(send));
				}

				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

