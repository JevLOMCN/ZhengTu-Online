/**
 * \file
 * \version  $Id: SceneSchool.cpp $
 * \author  
 * \date
 * \brief 实现师门命令的处理
 *
 */
#include "SceneUser.h"
#include "Scene.h"
#include "RecordClient.h"
#include "School.h"
#include "Chat.h"
#include "SessionCommand.h"
#include "SessionClient.h"

/**
  * \brief 一个比较器
  *
  *  用于查找建立师门所需要的道具是否存在
  *  
  *
  */
class SchoolObjectCompare:public UserObjectCompare 
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
  * \brief 处理用户师门命令
  *
  * 处理的关系命令如下:
  *
  * Cmd::ADD_MEMBER_TO_SCHOOL_PARA
  *
  * Cmd::SCHOOL_STATUS_CHECK_PARA:
  *
  *	Cmd::CREATE_SCHOOL_PARA:
  *
  * \param rev: 师门命令
  * \param cmdLen: 命令长度
  *
  * \return 命令被处理返回TRUE,否则为FALSE
  *
  *
  */
bool SceneUser::doSchoolCmd(const Cmd::stSchoolUserCmd *rev,unsigned int cmdLen)
{
	switch(rev->byParam)
	{
		case Cmd::ADD_MEMBER_TO_SCHOOL_PARA:
			{
				Cmd::stAddMemberToSchoolCmd *ptCmd=(Cmd::stAddMemberToSchoolCmd *)rev;
				switch(ptCmd->byState)
				{
					case Cmd::TEACHER_QUESTION:
						{
							if (charbase.level <TEACHER_LEVEL)
							{
								Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你必须达到%d级才能招收徒弟",TEACHER_LEVEL);
								return true;
							}
							//SceneUser *pUser=scene->getUserByName(ptCmd->memberName);
							SceneUser *pUser=SceneUserManager::getMe().getUserByName(ptCmd->memberName);
							if (pUser)
							{
								if (pUser != this)
								{
									if (pUser->charbase.level >10)
									{
										/*
										if (!isset_state(pUser->sysSetting , Cmd::USER_SETTING_SCHOOL))
										{
											Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "玩家 %s 加入师门未开启", pUser->name);
											return true;
										}
										*/
										if (scene->checkTwoPosIInNine(getPosI(),pUser->getPosI()))
										{
											forwardSession(rev,cmdLen);
										}
										else
										{
											Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "距离太远无法邀请");
										}
									}
									else
									{
										Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对方未满10级，不能收为徒弟！");
									}
								}
								else
								{
									Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "难道你想收自己为徒吗?");
								}
							}
							else
							{
								Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "玩家 %s 不在同一地图，无法回应邀请", ptCmd->memberName);
							}
							return true;
						}
						break;
					case Cmd::TEACHER_ANSWER_YES:
						{
							SceneUser *pUser=scene->getUserByTempID(ptCmd->memberID);
							if (pUser)
							{
								forwardSession(rev, cmdLen);
							}
							else
							{
								Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对方已经离开，他放弃了这次邀请");
							}
							return true;
						}
						break;
					case Cmd::TEACHER_ANSWER_NO:
						{
							SceneUser *pUser=scene->getUserByTempID(ptCmd->memberID);
							if (pUser)
							{
								Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "%s不愿意做你的徒弟, 拒绝了你的邀请",name);
							}
							return true;
						}
						break;
					default:
						break;
				}
			}
			break;
		case Cmd::SCHOOL_STATUS_CHECK_PARA:
		case Cmd::CREATE_SCHOOL_PARA:
			{
				if (charbase.level >= CREATE_SCHOOL_REQUEST_LEVEL)
				{
					SchoolObjectCompare found;
					found.dwObjectID = CREATE_SCHOOL_REQUEST_ITEM_ID;
					zObject *itemobj = packs.uom.getObject(found);// 查找道具
					if (itemobj)
					{
/*
						zObject *gold=packs.getGold();
						if (gold&&gold->data.dwNum >= CREATE_SCHOOL_REQUEST_PRICE_GOLD)
						{
							forwardSession(rev,cmdLen);
						}
*/
						if (packs.checkMoney(CREATE_SCHOOL_REQUEST_PRICE_GOLD) /*&& packs.removeMoney(CREATE_SCHOOL_REQUEST_PRICE_GOLD)*/) {
							forwardSession(rev,cmdLen);
						}
						else
						{
							//报告没有足够的钱
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "没有足够的钱来创建门派!");
						}
					}
					else 
					{
						//报告没有道具
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "缺少任务道具无法创建门派!");
					}
				}
				else
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的级别不够不能创建门派，再回去好好修炼吧!");
				}
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

