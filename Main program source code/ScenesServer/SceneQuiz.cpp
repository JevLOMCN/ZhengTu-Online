/**
 * \file
 * \version  $Id: SceneQuiz.cpp  $
 * \author  
 * \date 
 * \brief 实现竞赛处理的类
 *
 * 
 */
#include "SceneUser.h"
#include "Scene.h"
#include "RecordClient.h"
#include "Chat.h"
#include "SessionCommand.h"
#include "SessionClient.h"
#include "Quiz.h"

using namespace QuizDef;

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
bool SceneUser::doQuizCmd(const Cmd::stQuizUserCmd *rev,unsigned int cmdLen)
{
	//Zebra::logger->debug("doQuizCmd receive byParam:[%d]", rev->byParam);

	switch (rev->byParam)
	{
		case Cmd::QUERY_QUIZ_PARA:
			{
				Cmd::stQueryQuiz* cmd = (Cmd::stQueryQuiz*)rev;
				
				if (cmd->byMsgType == Cmd::QUIZ_YES)
				{
					this->setState(zSceneEntry::SceneEntry_Hide);

					//通知9屏删除该角色
					Cmd::stRemoveUserMapScreenUserCmd remove;
					remove.dwUserTempID = this->tempid;
					this->scene->sendCmdToNine(this->getPosI(),&remove,sizeof(remove));

					//移除block
					this->scene->clearBlock(this->getPos());	
					this->isQuiz = true;
				}
				
				return true;
			}
			break;
		case Cmd::CREATE_QUIZ_PARA:
			{//检查银两，并发送竞赛开始指令给Session
				Cmd::stCreateQuiz* cmd = (Cmd::stCreateQuiz*)rev;
				Cmd::Session::t_createQuiz_SceneSession send;
				DWORD needMoney = 0;
				DWORD subjects = 20;

				if (this->charbase.answerCount == 0)
				{
					this->charbase.answerCount = 1;
				}

				if (this->charbase.answerCount>5)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你今天已经回答5次了哦,该休息休息了");
					return true;

				}

//				needMoney = (DWORD)(0.05*this->charbase.level * (0.5+0.5*0.02*this->charbase.level));
//	金钱收取（两）=　int(角色等级/20 + 1) * （n + int(角色等级/30 + 1)）
				needMoney = (DWORD)((this->charbase.level/20+1) * ((this->charbase.answerCount)+(this->charbase.level/30+1)));
				needMoney = needMoney * 100;
/*
				if (this->charbase.answerCount>1)
				{
					needMoney = (DWORD)(needMoney * (this->charbase.answerCount));
				}
*/

				if (needMoney<100)
				{
					needMoney = 100;
				}
				
				if (cmd->byType == Cmd::CREATE_QUESTION)
				{
					if (this->packs.checkMoney(needMoney))
					{
						cmd->dwMoney = needMoney;
						this->sendCmdToMe(cmd, sizeof(Cmd::stCreateQuiz));
					}
					else
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, 
								"银两不足 %d 文，不能进行问答。", needMoney);
					}

					return true;
				}
				
				if (cmd->byType == Cmd::CREATE_YES)
				{
					if (needMoney>0)	
					{
						if (this->packs.checkMoney(needMoney) 
								&& this->packs.removeMoney(needMoney,"答题"))
						{
							Zebra::logger->trace("[个人答题]:%s 本次答题花费%d, 第%d次答题", 
									this->name, needMoney, this->charbase.answerCount);

							this->charbase.answerCount++;
							
							send.dwUserID = this->id;
							send.type = QuizDef::PERSONAL_QUIZ;
							send.dwSubjects = subjects; 
							sessionClient->sendCmd(&send, sizeof(send));
							// 进入隐身状态
							this->setState(zSceneEntry::SceneEntry_Hide);

							//通知9屏删除该角色
							Cmd::stRemoveUserMapScreenUserCmd remove;
							remove.dwUserTempID = this->tempid;
							this->scene->sendCmdToNine(this->getPosI(),&remove,sizeof(remove));

							//移除block
							this->scene->clearBlock(this->getPos());
							this->isQuiz = true;
						}
						else
						{
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, 
								"银两不足,不能进入智力问答");
						}
					}
					else
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "等级不足10级,不能进入智力问答");
					}	

					return true;
				}

				return true;
			}
			break;
		default:
			break;
	}
	
	return false;
}

