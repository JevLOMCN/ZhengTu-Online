/**
 * \file
 * \version	$Id: QuestAction.cpp $
 * \author	
 * \date	
 * \brief	任务系统
 * 
 */

#include "QuestAction.h"

#include "SceneUser.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Chat.h"
#include "GuardNpc.h"
#include "SessionClient.h"
#include "ScenePk.h"

/**     
 * \brief  执行脚本定义的动作
 *
 * template method模式,进行参数检查，并执行脚本定义的动作
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */     
int Action::do_it (SceneUser* user, Vars* vars)
{
	if (!check_args(user, vars)) return Action::FAILED;	
	
	return done(user, vars);
}

/**     
 * \brief  参数检查
 *
 * 提供了一个默认的参数需求检测,继承类需要根据自己的要求重载此函数
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return true表示参数合法,false表示参数非法
 */     
bool Action::check_args(SceneUser* user, Vars* vars) const
{
	if (user && vars) return true;
	
	return false;
}

/**     
 * \brief  执行脚本定义的动作
 *
 * template method模式,进行参数检查，并对于队伍中的每一个成员执行脚本定义的动作
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */   
int TeamAction::do_it(SceneUser* user, Vars* vars)
{
	if (!check_args(user, vars) ) return false;
	
	if (_team) {
		DWORD leader_id = user->team.getLeader();
		SceneUser* leader = SceneUserManager::getMe().getUserByTempID(leader_id);
		int result = 0;
		if (leader) {
			Team& team = const_cast<Team&>(leader->team.getTeam());

			team.rwlock.rdlock();
			std::vector<TeamMember>::iterator it = team.member.begin();
			for(; it!=team.member.end(); ++it) {
				SceneUser* member = SceneUserManager::getMe().getUserByTempID(it->tempid);
				if (member) {
					Vars* v = member->quest_list.vars(vars->quest_id());

					if (v) result |= done(member, v);

				}
			}
			team.rwlock.unlock();
		}else {
			result = done(user, vars);		
		}
		
		return result;	
	}	

	return done(user, vars);
}

/**     
 * \brief  系统聊天
 *
 * 重载了done函数,发送系统聊天信息给特定用户
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */     	
int NotifyAction::done (SceneUser* user, Vars* vars)
{
	Channel::sendSys(user, Cmd::INFO_TYPE_GAME, _info.c_str());
	return Action::SUCCESS;
}

/**     
 * \brief  任务日志
 *
 * 重载了done函数,输出任务日志到Log中。
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */     	
int LogAction::done (SceneUser* user, Vars* vars)
{
	Zebra::logger->trace("任务日志: 角色%s, %s", user->name, _info.c_str());
	return Action::SUCCESS;
}

/**     
 * \brief  国家/世界公告
 *
 * 重载了done函数,输出任务日志到Log中。
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
//soke 任务脚本国家/世界公告
int SendAllInfoAction::done (SceneUser* user, Vars* vars)
{

//	Zebra::logger->trace("国家任务日志: 我国的「%s」%s", user->name, _info.c_str());

//	Zebra::logger->trace("世界任务日志: %s的「%s」%s", SceneManager::getInstance().getCountryNameByCountryID(user->charbase.country),user->name, _info.c_str());

	Cmd::Session::t_QuestBulletinUserCmd cmd;

	cmd.kind = _kind;

	if (_kind == 3) //soke 国家消息 (淡黄)
	{
		Channel::sendCountryInfo(user->charbase.country, Cmd::INFO_TYPE_EXP1, "我国的%s%s", user->name, _info.c_str());
    }
	if (_kind == 30) //soke 国家消息 (蓝色)
	{
		Channel::sendCountryInfo(user->charbase.country, Cmd::INFO_TYPE_EXP6, "%s%s", user->name, _info.c_str());
    }
	if (_kind == 31) //soke 国家消息 (黄色)
	{
		Channel::sendCountryInfo(user->charbase.country, Cmd::INFO_TYPE_EXP, "%s%s", user->name, _info.c_str());
    }
    if (_kind == 32) //soke 国家消息 (绿色)
	{
		Channel::sendCountryInfo(user->charbase.country, Cmd::INFO_TYPE_EXP2, "%s%s", user->name, _info.c_str());
    }
	if (_kind == 33) //soke 国家消息 (紫色)
	{
		Channel::sendCountryInfo(user->charbase.country, Cmd::INFO_TYPE_EXP3, "%s%s", user->name, _info.c_str());
    }

	if (_kind == 4) //soke 世界消息（黄色）
	{
		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP, " %s%s%s", SceneManager::getInstance().getCountryNameByCountryID(user->charbase.country),user->name, _info.c_str());
    }
	if (_kind == 40) //soke 世界消息（蓝色）
	{
		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP6, " %s%s%s", SceneManager::getInstance().getCountryNameByCountryID(user->charbase.country),user->name, _info.c_str());
    }
	if (_kind == 41) //soke 世界消息（绿色）
	{
		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, " %s的%s%s", SceneManager::getInstance().getCountryNameByCountryID(user->charbase.country),user->name, _info.c_str());
    }
	if (_kind == 42) //soke 世界消息（紫色）
	{
		Channel::sendAllInfo(Cmd::INFO_TYPE_EXP3, " %s的%s%s", SceneManager::getInstance().getCountryNameByCountryID(user->charbase.country),user->name, _info.c_str());
    }


	if (_kind == 66) //soke 滚动
	{
		Channel::sendAllInfo(Cmd::INFO_TYPE_YANSE1, " %s的%s%s", SceneManager::getInstance().getCountryNameByCountryID(user->charbase.country),user->name, _info.c_str());
    }
	
	return Action::SUCCESS;
}

int BulletinAction::done (SceneUser* user, Vars* vars)
{
	Cmd::Session::t_QuestBulletinUserCmd cmd;
	cmd.kind = _kind;
	strncpy(cmd.content, _info.c_str(), _info.length());
	if (_kind == 1) //soke 帮会消息
	{
		cmd.id = user->charbase.unionid;
	}
	if (_kind == 2) //soke 家族消息
	{
		cmd.id = user->charbase.septid;
	}
	
	sessionClient->sendCmd(&cmd, sizeof(cmd));
	return Action::SUCCESS;
}

/**     
 * \brief  系统聊天
 *
 * 重载了done函数,发送系统聊天信息给特定用户
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */     	
int Notify1Action::done (SceneUser* user, Vars* vars)
{
	Channel::sendSys(user, Cmd::INFO_TYPE_EXP, _info.c_str());
	return Action::SUCCESS;
}

/**     
 * \brief  系统聊天
 *
 * 重载了done函数,发送系统聊天信息给特定用户
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
//soke 淡黄色
int Notify2Action::done (SceneUser* user, Vars* vars)
{
	Channel::sendSys(user, Cmd::INFO_TYPE_EXP1, _info.c_str());
	return Action::SUCCESS;
}

/**     
 * \brief  系统聊天
 *
 * 重载了done函数,发送系统聊天信息给特定用户
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
//soke 绿色
int Notify3Action::done (SceneUser* user, Vars* vars)
{
	Channel::sendSys(user, Cmd::INFO_TYPE_EXP2, _info.c_str());
	return Action::SUCCESS;
}

/**     
 * \brief  系统聊天
 *
 * 重载了done函数,发送系统聊天信息给特定用户
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
//soke 紫色
int Notify4Action::done (SceneUser* user, Vars* vars)
{
	Channel::sendSys(user, Cmd::INFO_TYPE_EXP3, _info.c_str());
	return Action::SUCCESS;
}

/**     
 * \brief  系统聊天
 *
 * 重载了done函数,发送系统聊天信息给特定用户
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
//soke 橙色
int Notify5Action::done (SceneUser* user, Vars* vars)
{
	Channel::sendSys(user, Cmd::INFO_TYPE_EXP4, _info.c_str());
	return Action::SUCCESS;
}

/**     
 * \brief  系统聊天
 *
 * 重载了done函数,发送系统聊天信息给特定用户
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
//soke 红色
int Notify6Action::done (SceneUser* user, Vars* vars)
{
	Channel::sendSys(user, Cmd::INFO_TYPE_EXP5, _info.c_str());
	return Action::SUCCESS;
}

/**     
 * \brief  系统聊天
 *
 * 重载了done函数,发送系统聊天信息给特定用户
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
//soke 蓝色
int Notify7Action::done (SceneUser* user, Vars* vars)
{
	Channel::sendSys(user, Cmd::INFO_TYPE_EXP6, _info.c_str());
	return Action::SUCCESS;
}
/**     
 * \brief  系统聊天
 *
 * 重载了done函数,发送系统聊天信息给特定用户
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
//桃子 增加定时BOSS提示
int Notify8Action::done (SceneUser* user, Vars* vars)
{
	Channel::sendSys(user, Cmd::INFO_TYPE_GAME3, _info.c_str());
	return Action::SUCCESS;
}
/**     
 * \brief  系统聊天
 *
 * 重载了done函数,发送系统聊天信息给特定用户
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
//soke 增加一个弹窗的公告
int FlexmenuAction::done (SceneUser* user, Vars* vars)
{
	Channel::sendSys(user, Cmd::INFO_TYPE_MSG, _info.c_str());
	return Action::SUCCESS;
}

/**     
 * \brief  菜单
 *
 * 重载了done函数,设置用户访问的npc的任务菜单
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */  
   	
int MenuAction::done (SceneUser* user, Vars* vars)
{
	user->quest_list.set_menu(_menu);
	return Action::SUCCESS;
}

/**     
 * \brief  菜单
 *
 * 重载了done函数,设置用户访问的npc的任务菜单
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */  
   	
int SubMenuAction::done (SceneUser* user, Vars* vars)
{
	user->quest_list.add_menu(_menu);
	return Action::SUCCESS;
}

/**     
 * \brief  菜单
 *
 * 重载了done函数,设置用户访问的npc的任务菜单
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */     	
int MainMenuAction::done (SceneUser* user, Vars* vars)
{
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::stVisitNpcTradeUserCmd *cmd=(Cmd::stVisitNpcTradeUserCmd *)buf;
	bzero(buf, sizeof(buf));
	constructInPlace(cmd);

	strcpy(cmd->menuTxt, _menu.c_str());	
	cmd->byReturn = 1;
	user->sendCmdToMe(cmd, sizeof(Cmd::stVisitNpcTradeUserCmd) + strlen(cmd->menuTxt));

	return Action::SUCCESS;
}


/**     
 * \brief  刷新变量
 *
 * 重载了done函数,刷新任务变量值给该用户
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */    
int RefreshAction::done (SceneUser* user, Vars* vars)
{
	if (_id) {
		Vars* v = user->quest_list.vars(_id);
		if (v) {
			v->notify(*user, _name);
		}
	}else {
		vars->notify(*user, _name);
	}

	return Action::SUCCESS;
}

/**     
 * \brief  增加经验
 *
 * 重载了done函数,增加用户的经验
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */    
int ExpAction::done (SceneUser* user, Vars* vars)
{
	Vars* vs = vars;
	if (_id) vs = user->quest_list.vars(_id);

	if (!vs) 
	{
		user->addExp(_exp);
		return Action::SUCCESS;
	}

	int ratio;
	if (!vs->get_value(_name, ratio)) 
	{
		user->addExp(_exp);
		return Action::SUCCESS;
	}
	
	//add exp bonus	
	user->addExp(int(_exp*(ratio/100.0)));	
	return Action::SUCCESS;
}

int EnterSeptGuardAction::done(SceneUser* user, Vars* vars)
{
	user->enterSeptGuard();
	return Action::SUCCESS;
}

int FinishSeptGuardAction::done(SceneUser* user, Vars* vars)
{
	user->finishSeptGuard();
	return Action::SUCCESS;
}

int Exp1Action::done (SceneUser* user, Vars* vars)
{
	Vars* vs = vars;
	if (_id) vs = user->quest_list.vars(_id);

	if (!vs) 
	{
		return Action::SUCCESS;
	}

	int exp;
	if (!vs->get_value(_name, exp)) 
	{
		return Action::SUCCESS;
	}
	
	//add exp bonus	
	user->addExp(exp);	
	return Action::SUCCESS;
}

/**     
 * \brief  增加技能等级
 *
 * 重载了done函数,增加用户某一技能的等级
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int SkillAction::done (SceneUser* user, Vars* vars)
{
	user->upgradeSkill(_id, false);
	return Action::SUCCESS;
}

/**     
 * \brief  禁止动作
 *
 * 重载了done函数,禁止用户做特定的动作
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int DisableAction::done (SceneUser* user, Vars* vars)
{
	return Action::DISABLE;
}

/**     
 * \brief  跳转
 *
 * 重载了done函数,使用户跳转到特定地图位置
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int GotoAction::done (SceneUser* user, Vars* vars)
{
	
	if (_name.length()/*strcmp(user->scene->name, _name.c_str())*/)  {
		
		//Zebra::logger->debug("换地图：%s", _name.c_str());
		std::string tmpStr = "name=" + _name + " pos=" +  _pos;
		bool bret=false;
		switch(zMisc::randBetween(1,4))
		{
			case 1:
				if (_pos1.length() >3) {tmpStr = "name=" + _name + " pos=" +  _pos1;bret=true;}
				break;
			case 2:
				if (_pos2.length() >3) {tmpStr = "name=" + _name + " pos=" +  _pos2;bret=true;}
				break;
			case 3:
				if (_pos3.length() >3) {tmpStr = "name=" + _name + " pos=" +  _pos3;bret=true;}
				break;
			case 4:
				if (_pos4.length() >3) {tmpStr = "name=" + _name + " pos=" +  _pos4;bret=true;}
				break;
		}
		if(!bret)
		{
			if(_cpos.length() >4 )
			{
				if(_rlen.length( ) > 4)
					tmpStr = "name=" + _name + " cpos=" + _cpos + " rlen=" + _rlen;
				else
					tmpStr = "name=" + _name + " cpos=" + _cpos;
			}
		}
		// */
		if(user->isBike() || user->isZC() || user->isSpa())
		{
			Channel::sendSys(user, Cmd::INFO_TYPE_FAIL,"当前状态不允许传送");	
			return Action::FAILED;
		}

		//todo天下第一 添加禁止传送	
		if(user->scene->getRealMapID()>=10001 && user->scene->getRealMapID()<=10401)
		{

			Channel::sendSys(user, Cmd::INFO_TYPE_FAIL,"天下第一比武场不允许传送，如若遇到掉线无法传出，请联系GM");	
			return Action::FAILED;
		}
		
		Gm::gomap(user, tmpStr.c_str());
	}
	//Zebra::logger->debug("换坐标：%s", _pos.c_str());
	else
	{
		std::string tmpStr = _pos;

		switch(zMisc::randBetween(1,4))
		{
			case 1:
				if (_pos1.length() >3) tmpStr = _pos1;
				break;
			case 2:
				if (_pos2.length() >3) tmpStr = _pos2;
				break;
			case 3:
				if (_pos3.length() >3) tmpStr = _pos3;
				break;
			case 4:
				if (_pos4.length() >3) tmpStr = _pos4;
				break;
		}
		Gm::goTo(user, tmpStr.c_str());	
	}
	
	return Action::SUCCESS;
}


//soke 添加掉落物品
int AddDropItemAction::done (SceneUser* user, Vars* vars)
{	
	if (_id==695)
	{

	}
	else
	{

		if (!_odds || zMisc::selectByTenTh(_odds)) 
		{
			//user->addObjectNum(_id, _value);
			zObjectB *ob = objectbm.get(_id);
			if (ob)
			{
				user->scene->addObject(ob,_value,user->getPos(),user->id);
			}
			
		}
	}
	return Action::SUCCESS;
}

//soke 添加随机给予道具
/**        
* \param user: 触发动作的用户
* \param vars: 用户所带的该任务相关变量
* \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
*/
int AddRandItemAction::done (SceneUser* user, Vars* vars)
{	
	int id = 0;
	int value=0;
	int odds =_odds+_odds2+_odds3+_odds4+_odds5+_odds6+_odds7+_odds8+_odds9+_odds10+_odds11+_odds12+_odds13+_odds14+_odds15+_odds16+_odds17+_odds18+_odds19+_odds20;
	if (odds==0)
	{
		return 1;
	}
	int n = zMisc::randBetween(1, odds);

	if (_odds20)
	{
		if (n>_odds19+_odds18+_odds17+_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id20;
			value = _value20;
		}
	}

	if (_odds19)
	{
		if (n>_odds18+_odds17+_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds19+_odds18+_odds17+_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id19;
			value = _value19;
		}
	}

	if (_odds18)
	{
		if (n>_odds17+_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds18+_odds17+_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id18;
			value = _value18;
		}
	}

	if (_odds17)
	{
		if (n>_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds17+_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id17;
			value = _value17;
		}
	}

	if (_odds16)
	{
		if (n>_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id16;
			value = _value16;
		}
	}

    if (_odds15)
	{
		if (n>_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id15;
			value = _value15;
		}
	}

	if (_odds14)
	{
		if (n>_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id14;
			value = _value14;
		}
	}

	if (_odds13)
	{
		if (n>_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id13;
			value = _value13;
		}
	}

    if (_odds12)
	{
		if (n>_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id12;
			value = _value12;
		}
	}

	if (_odds11)
	{
		if (n>_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id11;
			value = _value11;
		}
	}

   if (_odds10)
	{
		if (n>_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id10;
			value = _value10;
		}
	}
	if (_odds9)
	{
		if (n>_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id9;
			value = _value9;
		}
	}
	if (_odds8)
	{
		if (n>_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id8;
			value = _value8;
		}
	}
	if (_odds7)
	{
		if (n>_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id7;
			value = _value7;
		}
	}
	if (_odds6)
	{
		if (n>_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id6;
			value = _value6;
		}
	}
	
	if (_odds5)
	{
		if (n>_odds4+_odds3+_odds2+_odds&&n<=_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id5;
			value = _value5;
		}
	}
	
	if (_odds4)
	{
		if (n>_odds3+_odds2+_odds&&n<=_odds4+_odds3+_odds2+_odds)
		{
			id=_id4;
			value = _value4;
		}
	}

	if (_odds3)
	{
		if (n>_odds2+_odds&&n<=_odds3+_odds2+_odds)
		{
			id=_id3;
			value = _value3;
		}
	}

	if (_odds2)
	{
		if (n>_odds&&n<=_odds2+_odds)
		{
			id=_id2;
			value = _value2;
		}
	}

	if (_odds)
	{
		if (n<=_odds)
		{
			id=_id;
			value = _value;
		}
	}

	if (id==0)
	{
		return 1;
	}
	if (id==695)
	{
		if (user->packs.store.days.size()<3)
		{
			user->packs.store.days.push_back(value);
			user->packs.store.notify(user);
		}

	}
	else
	{
		user->addObjectNum(id, value);
	}
	return Action::SUCCESS;
}

//soke 添加随机给予绑定道具
/**        
* \param user: 触发动作的用户
* \param vars: 用户所带的该任务相关变量
* \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
*/
int AddRandBindItemAction::done (SceneUser* user, Vars* vars)
{	
	int id = 0;
	int value=0;
	int odds =_odds+_odds2+_odds3+_odds4+_odds5+_odds6+_odds7+_odds8+_odds9+_odds10+_odds11+_odds12+_odds13+_odds14+_odds15+_odds16+_odds17+_odds18+_odds19+_odds20;
	if (odds==0)
	{
		return 1;
	}
	int n = zMisc::randBetween(1, odds);

	if (_odds20)
	{
		if (n>_odds19+_odds18+_odds17+_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id20;
			value = _value20;
		}
	}

	if (_odds19)
	{
		if (n>_odds18+_odds17+_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds19+_odds18+_odds17+_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id19;
			value = _value19;
		}
	}

	if (_odds18)
	{
		if (n>_odds17+_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds18+_odds17+_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id18;
			value = _value18;
		}
	}

	if (_odds17)
	{
		if (n>_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds17+_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id17;
			value = _value17;
		}
	}

	if (_odds16)
	{
		if (n>_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds16+_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id16;
			value = _value16;
		}
	}

    if (_odds15)
	{
		if (n>_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds15+_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id15;
			value = _value15;
		}
	}

	if (_odds14)
	{
		if (n>_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds14+_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id14;
			value = _value14;
		}
	}

	if (_odds13)
	{
		if (n>_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds13+_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id13;
			value = _value13;
		}
	}

    if (_odds12)
	{
		if (n>_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds12+_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id12;
			value = _value12;
		}
	}

	if (_odds11)
	{
		if (n>_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds11+_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id11;
			value = _value11;
		}
	}

   if (_odds10)
	{
		if (n>_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds10+_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id10;
			value = _value10;
		}
	}
	if (_odds9)
	{
		if (n>_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds9+_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id9;
			value = _value9;
		}
	}
	if (_odds8)
	{
		if (n>_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds8+_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id8;
			value = _value8;
		}
	}
	if (_odds7)
	{
		if (n>_odds6+_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds7+_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id7;
			value = _value7;
		}
	}
	if (_odds6)
	{
		if (n>_odds5+_odds4+_odds3+_odds2+_odds&&n<=_odds6+_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id6;
			value = _value6;
		}
	}
	
	if (_odds5)
	{
		if (n>_odds4+_odds3+_odds2+_odds&&n<=_odds5+_odds4+_odds3+_odds2+_odds)
		{
			id=_id5;
			value = _value5;
		}
	}
	
	if (_odds4)
	{
		if (n>_odds3+_odds2+_odds&&n<=_odds4+_odds3+_odds2+_odds)
		{
			id=_id4;
			value = _value4;
		}
	}

	if (_odds3)
	{
		if (n>_odds2+_odds&&n<=_odds3+_odds2+_odds)
		{
			id=_id3;
			value = _value3;
		}
	}

	if (_odds2)
	{
		if (n>_odds&&n<=_odds2+_odds)
		{
			id=_id2;
			value = _value2;
		}
	}

	if (_odds)
	{
		if (n<=_odds)
		{
			id=_id;
			value = _value;
		}
	}

	if (id==0)
	{
		return 1;
	}
	if (id==695)
	{
		if (user->packs.store.days.size()<3)
		{
			user->packs.store.days.push_back(value);
			user->packs.store.notify(user);
		}

	}
	else
	{
		user->addBindObjectNum(id, value);
	}
	return Action::SUCCESS;
}

/**     
 * \brief  增加物品
 *
 * 重载了done函数,在用户包裹中增加特定物品
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int AddItemAction::done (SceneUser* user, Vars* vars)
{	
	if (_id==695)
	{
		if (user->packs.store.days.size()<3)
		{
			user->packs.store.days.push_back(_value);
			user->packs.store.notify(user);
		}

	}
	else
	{

		if (!_odds || zMisc::selectByTenTh(_odds)) {
			user->addObjectNum(_id, _value);
		}
	}
	return Action::SUCCESS;
}


/**     
 * \brief  增加绑定物品
 *
 * 重载了done函数,在用户包裹中增加特定绑定物品
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
//soke 修复绑定物品到包裹不绑定的BUG
int AddBindItemAction::done (SceneUser* user, Vars* vars)
{
	if (!_odds || zMisc::selectByTenTh(_odds)) 
	{
		user->addBindObjectNum(_id, _value, 0, 0, true);
	}
	return Action::SUCCESS;
}

/**     
 * \brief  增加材料物品
 *
 * 重载了done函数,在用户包裹中增加特定物品
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int AddGradeItemAction::done (SceneUser* user, Vars* vars)
{
	if (!_odds || zMisc::selectByTenTh(_odds)) 
	{
		user->addGradeObjectNum(_id, _value, _level); //soke 增加物品等级
	}

	return Action::SUCCESS;
}

/**     
 * \brief  增加蓝色绑定物品
 *
 * 重载了done函数,在用户包裹中增加特定蓝色绑定物品
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int AddBlueBindItemAction::done (SceneUser* user, Vars* vars)
{
	if (!_odds || zMisc::selectByTenTh(_odds)) 
	{
		user->addBlueObjectNum(_id, _value, 0, _maker, 0 , true);
	}
	return Action::SUCCESS;
}

/**     
 * \brief  增加黄色绑定物品
 *
 * 重载了done函数,在用户包裹中增加特定黄色绑定物品
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int AddGoldBindItemAction::done (SceneUser* user, Vars* vars)
{
	if (!_odds || zMisc::selectByTenTh(_odds)) 
	{
		user->addGoldObjectNum(_id, _value, 0, 0, true);
	}
	return Action::SUCCESS;
}

/**     
 * \brief  增加绿色绑定物品
 *
 * 重载了done函数,在用户包裹中增加特定绿色绑定物品
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int AddGreenBindItemAction::done (SceneUser* user, Vars* vars)
{
	if (!_odds || zMisc::selectByTenTh(_odds)) 
	{
		user->addGreenObjectNum(_id, _value, _level, 0, true);
	}
	return Action::SUCCESS;
}

/**     
 * \brief  增加紫色绑定物品
 *
 * 重载了done函数,在用户包裹中增加特定紫色绑定物品
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int AddPurpleBindItemAction::done (SceneUser* user, Vars* vars)
{
	if (!_odds || zMisc::selectByTenTh(_odds)) 
	{
		user->addPurpleObjectNum(_id, _value, _level, 0, true);
	}
	return Action::SUCCESS;
}


/**     
 * \brief  删除物品
 *
 * 重载了done函数,在用户包裹中删除特定物品
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int RemoveItemAction::done (SceneUser* user, Vars* vars)
{
	user->reduceObjectNum(_id, _value);
	return Action::SUCCESS;
}

/**     
 * \brief  丢弃物品
 *
 * 重载了done函数,从用户包裹中丢弃特定物品
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int DropItemAction::done (SceneUser* user, Vars* vars)
{
	zObject* ob = user->packs.uom.getObjectByID(_id, _level, true);
	if (ob) user->packs.moveObjectToScene(ob, user->getPos());
	
	return Action::SUCCESS;
}

/**     
 * \brief 未实现
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int DropAction::done (SceneUser* user, Vars* vars)
{
/*
	zObject* ob = user->packs.main.getObjectByID(_id, _level, true);
	if (ob) user->packs.moveObjectToScene(ob, user->getPos());
*/	
	return Action::SUCCESS;
}

/**     
 * \brief  下马
 *
 * 重载了done函数,如果用户在马上则使用户下马
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int RideDownAction::done (SceneUser* user, Vars* vars)
{
//	Cmd::stRideMapScreenUserCmd ret;
//	ret.dwUserTempID = user->tempid;
//	ret.bySwitch = 0;
//	clear_state(user->getByState(), Cmd::USTATE_RIDE);
//	user->dwHorseID = 0;
//	user->sendMeToNine();
	user->horse.mount(false);
	
//	user->scene->sendCmdToNine(user->getPosI(), &ret, sizeof(ret));
	
	return Action::SUCCESS;
}

/**     
 * \brief  定时器
 *
 * 重载了done函数,增加用户完成该任务的时间限制
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int TimeoutsAction::done (SceneUser* user, Vars* vars)
{

	if (_id)
	{
		Vars* v = user->quest_list.vars(_id);
		if (v) 
		{
			v->set_timer();
		}
	}
	else 
	{
		vars->set_timer();	
	}

	return Action::SUCCESS;
}

/**     
 * \brief  设定状态
 *
 * 重载了done函数,设定用户的状态
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int SetStateAction::done (SceneUser* user, Vars* vars)
{
	user->setUState(_state);
	return Action::SUCCESS;
}

/**     
 * \brief  清除状态
 *
 * 重载了done函数,清除用户的状态
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int ClearStateAction::done (SceneUser* user, Vars* vars)
{
	user->clearUState(_state);
	return Action::SUCCESS;
}

/**     
 * \brief  马匹
 *
 * 重载了done函数,给予用户马匹
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int HorseAction::done (SceneUser* user, Vars* vars)
{
	user->horse.horse(_id);
	return Action::SUCCESS;
}


/**     
 * \brief  添加NPC
 *
 * 重载了done函数,在地图上增加一个NPC
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int AddNpcAction::done (SceneUser* user, Vars* vars)
{
	zNpcB *base = npcbm.get(_id);
	if (!base) return  Action::FAILED;

	t_NpcDefine define;
	define.id = base->id;
	strcpy(define.name, base->name);
	define.pos = _ltpos;
	define.width = _rbpos.x - _ltpos.x;
	define.height = _rbpos.y - _ltpos.y;
	define.num = _num;
	define.interval = 30;
	define.initstate = zSceneEntry::SceneEntry_Normal;

	if (!_s) {
		return  Action::FAILED;
	}

	_s->zPosRevaluate(define.pos);
	_s->initRegion(define.region, define.pos, define.width, define.height);

	_s->summonNpc(define, define.pos, base);

	//Zebra::logger->debug("增加npc成功");	
	
	return Action::SUCCESS;
}

/**     
 * \brief  删除NPC
 *
 * 重载了done函数,在地图上指定范围删除一个NPC
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int RemoveNpcAction::done (SceneUser* user, Vars* vars)
{	
	SceneNpcManager::getMe().removeNpc_if(_remove);
	
	return Action::SUCCESS;
}

/**     
 * \brief  添加镖车
 *
 * 重载了done函数,在地图上添加一辆镖车
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int AddGuardAction::done(SceneUser* user, Vars* vars)
{
	DWORD i=_id;

	DWORD o = zMisc::randBetween(1, 100);
	if (_id3 && (o>(_odds1+_odds2)*100/(_odds1+_odds2+_odds3)))
		i=_id3;
	else if (_id2 && (o>_odds1*100/(_odds1+_odds2+_odds3)))
	{
		i=_id2;
		Channel::sendSys(user, Cmd::INFO_TYPE_GAME, "恭喜！你接到了一辆无敌镖车！");
	}

	zNpcB *base = npcbm.get(i);
	if (!base) return  Action::FAILED;

	if (user->guard) {
		Channel::sendSys(user, Cmd::INFO_TYPE_FAIL, "你还在护镖中!");
		return  Action::FAILED;
	}

	t_NpcDefine define;
	define.id = base->id;
	//snprintf(define.name, MAX_NAMESIZE, "%s的%s", user->name, base->name );
	strcpy(define.name, base->name);
	define.pos = _ltpos;
	define.width = _rbpos.x - _ltpos.x;
	define.height = _rbpos.y - _ltpos.y;
	define.num = _num;
	define.interval = 30;
	define.initstate = zSceneEntry::SceneEntry_Normal;
	user->scene->initRegion(define.region, define.pos, define.width, define.height);

	user->scene->zPosRevaluate(define.pos);

	GuardNpc* npc = user->scene->summonOneNpc<GuardNpc>(define, define.pos, base);
	if (!npc)
	{
		Zebra::logger->error("召唤 %s 失败", define.name);
		return Action::FAILED;
	}
	npc->setMaster(user);
	npc->setPetType(Cmd::PET_TYPE_GUARDNPC);
	user->guard = npc;
	npc->gold(_gold);
	if (user->venterSeptGuard.size())//家族运镖
	{
		DWORD m=0;
		for (unsigned int i=0; i<user->venterSeptGuard.size(); i++)
			m += user->venterSeptGuard[i].money;
		npc->gold(m);
		npc->isSeptGuard = true;
		Zebra::logger->debug("%s(%u) 增加家族镖车 gold=%u", user->name, user->id, m);
	}
	npc->owner(user);
	npc->dest(_dest);
	npc->map(_map);
	npc->exp(base->exp);
	SceneNpcManager::getMe().addSpecialNpc(npc);

	return Action::SUCCESS;
}
//soke 镖车
int ChangeGuardAction::done(SceneUser* user, Vars* vars)
{
	DWORD i=_id;

	DWORD o = zMisc::randBetween(1, 100);
	if (_id3 && (o>(_odds1+_odds2)*100/(_odds1+_odds2+_odds3)))
		i=_id3;
	else if (_id2 && (o>_odds1*100/(_odds1+_odds2+_odds3)))
	{
		i=_id2;
		Channel::sendSys(user, Cmd::INFO_TYPE_GAME, "恭喜！你接到了一辆无敌镖车！");
	}

	zNpcB *base = npcbm.get(i);
	if (!base) return  Action::FAILED;

	if (!user->guard) 
	{
		Channel::sendSys(user, Cmd::INFO_TYPE_FAIL, "你没有护送的镖车!");
		return  Action::FAILED;
	}
	user->guard->setClearState();
	user->guard->reset();

	t_NpcDefine define;
	define.id = base->id;
	//snprintf(define.name, MAX_NAMESIZE, "%s的%s", user->name, base->name );
	strcpy(define.name, base->name);
	define.pos = _ltpos;
	define.width = _rbpos.x - _ltpos.x;
	define.height = _rbpos.y - _ltpos.y;
	define.num = _num;
	define.interval = 30;
	define.initstate = zSceneEntry::SceneEntry_Normal;
	//define.scriptID = _script;
	//define.setPath(_path.c_str());
	user->scene->initRegion(define.region, define.pos, define.width, define.height);

	//if (!_s) 
	//{
	//	return  Action::FAILED;
	//}

	user->scene->zPosRevaluate(define.pos);

	GuardNpc* npc = user->scene->summonOneNpc<GuardNpc>(define, define.pos, base);
	if (!npc)
	{
		Zebra::logger->error("召唤 %s 失败", define.name);
		return Action::FAILED;
	}
	npc->setMaster(user);
	npc->setPetType(Cmd::PET_TYPE_GUARDNPC);
	user->guard = npc;
	//t_NpcAIDefine	ai;
	//ai.type = NPC_AI_PATROL;
	//npc->setAI(ai);
	//npc->sendData();
	/*
	GuardNpc* npc = user->scene->summonPet(user, Cmd::PET_TYPE_GUARDNPC, define, define.pos, base);
	if (!npc) 
	{
	Zebra::logger->error("召唤 %s 失败", define.name);
	return Action::FAILED;
	}
	npc->setMaster(user);
	user->pets.push_back(npc);

	//set owner
	*/
	npc->gold(_gold);
	if (user->venterSeptGuard.size())//家族运镖
	{
		DWORD m=0;
		for (unsigned int i=0; i<user->venterSeptGuard.size(); i++)
			m += user->venterSeptGuard[i].money;
		npc->gold(m);
		npc->isSeptGuard = true;
		Zebra::logger->debug("%s(%u) 增加家族镖车 gold=%u", user->name, user->id, m);
	}
	npc->owner(user);
	npc->dest(_dest);
	npc->map(_map);
	npc->exp(base->exp);
	SceneNpcManager::getMe().addSpecialNpc(npc);

	return Action::SUCCESS;
}

/**     
 * \brief  设置五行类型
 *
 * 重载了done函数,设置用户五行类型
 *      
 * \param user: 触发动作的用户
 * \param vars: 用户所带的该任务相关变量
 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
 */
int FiveTypeAction::done (SceneUser* user, Vars* vars)
{
	user->charbase.fivetype = _type;
	Cmd::stMainUserDataUserCmd  userinfo;
	user->full_t_MainUserData(userinfo.data);
	user->sendCmdToMe(&userinfo,sizeof(userinfo));
	
	return Action::SUCCESS;
}

int UseSkillAction::done (SceneUser* user, Vars* vars)
{
	user->sendSkill(_id, _level);
	
	return Action::SUCCESS;
}

