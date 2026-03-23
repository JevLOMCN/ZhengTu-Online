/**
 * \file
 * \version	$Id: QuestAction.h $
 * \author	
 * \date		2005-03-31
 * \brief		脚本触发动作
 * 
 */

#ifndef __QUEST_ACTION_H__
#define __QUEST_ACTION_H__

#include <string>

#include "Quest.h"
#include "QuestParse.h"

#include "Zebra.h"
#include "zObject.h"
#include "zDatabase.h"
#include "SceneUser.h"
#include "Command.h"

namespace Op {
	/**
	 * \brief 增加变量值
	 *
	 * 增加一个变量的值
	 *
	 */
	template <typename T>
	class Add
	{
	public:
		typedef T value_type;

		/**     
		 * \brief 
		 *
		 * 增加变量的值         
		 *      
		 * \param value: 待改变的变量
		 * \param action: 要修改的值
		 * \return 无
		 */     		
		void operator() (T& value, T action, SceneUser *user) const
		{
			value += action;
		}

		//added for debug
		std::string name() const 
		{
			return "Add";
		}		
	};

	/**
	 * \brief 设定变量值
	 *
	 * 设定一个变量的值
	 *
	 */
	template <typename T>
	class Set
	{
	public:
		typedef T value_type;
		
		/**     
		 * \brief  
		 *
		 * 设定一个变量的值         
		 *      
		 * \param value: 待改变的变量
		 * \param action: 要设定的值
		 * \return 无
		 */     
		 void operator() (T& value, T action, SceneUser *user) const
		{
			value = action;
		}

		//added for debug
		std::string name() const 
		{
			return "Set";
		}		
		
	};

	/**
	 * \brief 减少变量值
	 *
	 * 减少一个变量的值
	 *
	 */	
	template <typename T>
	class Sub
	{
	public:
		typedef T value_type;
		
		/**     
		 * \brief 
		 *
		 * 增加变量的值         
		 *      
		 * \param value: 待改变的变量
		 * \param action: 要减少的值
		 * \return 无
		 */     
		void operator() (T& value, T action, SceneUser *user) const
		{
			value -= action;
		}

		//added for debug
		std::string name() const 
		{
			return "Sub";
		}		
	};

	/**
	 * \brief 乘一个变量值
	 *
	 * 乘一个变量的值
	 *
	 */
	template <typename T>
	class Mul
	{
	public:
		typedef T value_type;
		
		/**     
		 * \brief  
		 *
		 * 设定一个变量的值         
		 *      
		 * \param value: 待改变的变量
		 * \param action: 要设定的值
		 * \return 无
		 */     
		 void operator() (T& value, T action, SceneUser *user) const
		{
			value *= action;
		}

		//added for debug
		std::string name() const 
		{
			return "Mul";
		}		
	};

	/**
	 * \brief 变量值乘方
	 *
	 * 变量值乘方
	 *
	 */
	template <typename T>
	class Pow
	{
	public:
		typedef T value_type;
		
		/**     
		 * \brief  
		 *
		 * 设定一个变量的值         
		 *      
		 * \param value: 待改变的变量
		 * \param action: 要设定的值
		 * \return 无
		 */     
		 void operator() (T& value, T action, SceneUser *user) const
		{
			value *= value;
		}

		//added for debug
		std::string name() const 
		{
			return "Pow";
		}		
	};

	/**
	 * \brief 变量值乘方
	 *
	 * 变量值乘方
	 *
	 */
	template <typename T>
	class Div
	{
	public:
		typedef T value_type;
		
		/**     
		 * \brief  
		 *
		 * 设定一个变量的值         
		 *      
		 * \param value: 待改变的变量
		 * \param action: 要设定的值
		 * \return 无
		 */     
		 void operator() (T& value, T action, SceneUser *user) const
		{
			value /= action;
		}

		//added for debug
		std::string name() const 
		{
			return "Div";
		}		
	};

	/**
	 * \brief 变量值乘方
	 *
	 * 变量值乘方
	 *
	 */
	template <typename T>
	class GetP
	{
	public:
		typedef T value_type;
		
		/**     
		 * \brief  
		 *
		 * 设定一个变量的值         
		 *      
		 * \param value: 待改变的变量
		 * \param action: 要设定的值
		 * \return 无
		 */     
		 void operator() (T& value, T action, SceneUser *user) const
		{
			if (user)
			{
				switch(action)
				{
					case 1:
						value = user->charbase.level;
						break;
					case 2:
						value = time(NULL)%2000000000;
						break;
					case 3:
						break;
					default:
						break;
				}
			}
		}

		//added for debug
		std::string name() const 
		{
			return "GetP";
		}		
	};
}

class SceneUser;

/**
 * \brief 触发动作基类
 *
 * 该类定义了脚本触发动作的接口。
 *
 */	
class Action
{
public:	
	typedef Action Base;
	
	enum {
		SUCCESS = 0,
		FAILED = 1,
		DISABLE = 2,		
	};

	virtual int do_it (SceneUser* user, Vars* vars);
	

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~Action() { }

protected:
	virtual bool check_args(SceneUser* user, Vars* vars) const;
	
	/**     
	 * \brief  执行脚本定义的动作
	 *
	 * 虚函数,继承类需要重载此函数提供对各种触发动作的支持
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */     
	virtual int done(SceneUser* user, Vars* vars) = 0;
};

/**
 * \brief 组队触发动作基类
 *
 * 该类提供了对于组队任务的支持
 *
 */	
class TeamAction : public Action
{
public:	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	TeamAction(Parse& p)
	{
		p.value("team", _team);	
	}
		
	int do_it (SceneUser* user, Vars* vars);

protected:	
	/**     
	 * \brief  执行脚本定义的动作
	 *
	 * 虚函数,继承类需要重载此函数提供对各种触发动作的支持
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */     
	int done(SceneUser* user, Vars* vars) = 0;

private:
	
	int _team;
};

/**
 * \brief 变量
 *
 * 该类提供了对于任务脚本中有关变量的修改的支持
 *
 */	
template <typename Operation>
class VarAction : public TeamAction
{
public:
	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	VarAction(Parse& p) : TeamAction(p)
	{ 
		p.value("name", _name);
		p.value("value", _action);
		p.value("id", _id);
		p.value("tmp", _tmp);		
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~VarAction()
	{ }

	/**     
	 * \brief  执行变量修改
	 *
	 * 重载了done函数,实现对相关变量的修改
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */     	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;
//		Zebra::logger->debug("id:%d\tname:%s\taction:%d\toperation:%s", _id, _name.c_str(), _action, op.name().c_str());					
		if (_id) {
			Vars* v = user->quest_list.vars(_id);
			if (v) {
				v->set_value(op, _name, _action, _tmp, user);
			}
		}else {
			vars->set_value(op, _name, _action, _tmp, user);
		}
		return Action::SUCCESS;
	}
	
private:
	std::string _name;
	value_type _action;
	int _tmp;
	int _id;
};

/**
 * \brief 变量间动作
 *
 * 该类提供了对于任务脚本中变量之间的操作的支持
 *
 */	
template <typename Operation>
class VarsAction : public TeamAction
{
public:
	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	VarsAction(Parse& p) : TeamAction(p)
	{ 
		p.value("name1", _name1);
		p.value("id1", _id1);
		p.value("name2", _name2);
		p.value("id2", _id2);
		p.value("tmp", _tmp);		
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~VarsAction()
	{ }

	/**     
	 * \brief  执行变量修改
	 *
	 * 重载了done函数,实现对相关变量的修改
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */     	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;
		value_type _action;
//		Zebra::logger->debug("id:%d\tname:%s\taction:%d\toperation:%s", _id, _name.c_str(), _action, op.name().c_str());					
		if (_id1)
		{
			Vars* v = user->quest_list.vars(_id1);
			if (v)
			{
				if (_id2)
				{
					Vars* tmpvars= user->quest_list.vars(_id2);
					tmpvars->get_value(_name2, _action);
				}
				else
				{
					vars->get_value(_name2,_action);
				}
				v->set_value(op, _name1, _action, _tmp, user);

			}
		}
		else
		{
			if (_id2)
			{
				Vars* tmpvars= user->quest_list.vars(_id2);
				tmpvars->get_value(_name2, _action);
			}
			else
			{
				vars->get_value(_name2,_action);
			}
			vars->set_value(op, _name1, _action, _tmp, user);
		}
		return Action::SUCCESS;
	}
	
private:
	int _tmp;
	int _id1;
	std::string _name1;
	int _id2;
	std::string _name2;
};

/**
 * \brief 用户变量
 *
 * 该类提供了对于任务脚本中需要保持在用户身上变量的支持
 *
 */	
template <typename Operation>
class UserVarAction : public TeamAction
{
public:
	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	UserVarAction(Parse& p) : TeamAction(p)
	{ 
		p.value("name", _name);
		p.value("value", _action);
		p.value("id", _id);
		p.value("tmp", _tmp);		
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~UserVarAction()
	{ }

	/**     
	 * \brief  执行变量修改
	 *
	 * 重载了done函数,实现对相关变量的修改
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */     	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;
		
		if (!_id) _id = vars->quest_id();
				
		Vars* v = UserVar::instance().add(_id, ((QWORD)user->charbase.accid << 32) | user->charbase.id);
		v->set_value(op, _name, _action, _tmp, user);
		
		return Action::SUCCESS;
	}
	
private:
	std::string _name;
	value_type _action;
	int _tmp;
	int _id;
};

/**
 * \brief 系统信息
 *
 * 该类提供了对用户提供系统聊天信息的支持.
 *
 */	
class NotifyAction : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	NotifyAction(Parse& p) : TeamAction(p)
	{ 
		p.value("content", _info);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~NotifyAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _info;	
};

/**
 * \brief 日志信息
 *
 * 该类提供了对日志信息的支持.
 *
 */	
class LogAction : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	LogAction(Parse& p) : TeamAction(p)
	{ 
		p.value("content", _info);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~LogAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _info;	
};

/**
 * \brief 国家/世界公告
 *
 * 该类提供了对日志信息的支持.
 *
 */	
class SendAllInfoAction : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	SendAllInfoAction(Parse& p) : TeamAction(p)
	{ 
		p.value("content", _info);
		p.value("kind", _kind);
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~SendAllInfoAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _info;
	int _kind;
};

class BulletinAction : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	BulletinAction(Parse& p) : TeamAction(p)
	{ 
		p.value("content", _info);	
		p.value("kind", _kind);
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~BulletinAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _info;	
	int _kind; 
};

/**
 * \brief 系统信息
 *
 * 该类提供了对用户提供系统聊天信息的支持.
 *
 */	
class Notify1Action : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	Notify1Action(Parse& p) : TeamAction(p)
	{ 
		p.value("content", _info);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~Notify1Action() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _info;	
};

/**
 * \brief 系统信息
 *
 * 该类提供了对用户提供系统聊天信息的支持.
 *
 */	
//soke 淡黄色
class Notify2Action : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	Notify2Action(Parse& p) : TeamAction(p)
	{ 
		p.value("content", _info);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~Notify2Action() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _info;	
};

/**
 * \brief 系统信息
 *
 * 该类提供了对用户提供系统聊天信息的支持.
 *
 */	
//soke 绿色
class Notify3Action : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	Notify3Action(Parse& p) : TeamAction(p)
	{ 
		p.value("content", _info);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~Notify3Action() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _info;	
};

/**
 * \brief 系统信息
 *
 * 该类提供了对用户提供系统聊天信息的支持.
 *
 */	
//soke 紫色
class Notify4Action : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	Notify4Action(Parse& p) : TeamAction(p)
	{ 
		p.value("content", _info);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~Notify4Action() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _info;	
};

/**
 * \brief 系统信息
 *
 * 该类提供了对用户提供系统聊天信息的支持.
 *
 */	
//soke 橙色
class Notify5Action : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	Notify5Action(Parse& p) : TeamAction(p)
	{ 
		p.value("content", _info);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~Notify5Action() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _info;	
};

/**
 * \brief 系统信息
 *
 * 该类提供了对用户提供系统聊天信息的支持.
 *
 */	
//soke 红色
class Notify6Action : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	Notify6Action(Parse& p) : TeamAction(p)
	{ 
		p.value("content", _info);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~Notify6Action() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _info;	
};

/**
 * \brief 系统信息
 *
 * 该类提供了对用户提供系统聊天信息的支持.
 *
 */	
//soke 蓝色
class Notify7Action : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	Notify7Action(Parse& p) : TeamAction(p)
	{ 
		p.value("content", _info);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~Notify7Action() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _info;	
};

///////////////////////////////////////////////////////////////////////
/**
 * \brief 系统信息
 *
 * 该类提供了对用户提供系统聊天信息的支持.
 *
 */	
//桃子 增加定时BOSS提示
class Notify8Action : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	Notify8Action(Parse& p) : TeamAction(p)
	{ 
		p.value("content", _info);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~Notify8Action() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _info;	
};

///////////////////////////////////////////////////////////////////////
/**
 * \brief 系统信息
 *
 * 该类提供了对用户提供系统聊天信息的支持.
 *
 */	//soke 增加系统弹窗公告
class FlexmenuAction : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	FlexmenuAction(Parse& p) : TeamAction(p)
	{ 
		p.value("content", _info);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~FlexmenuAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _info;	
};
/////////////////////////////////////////////////////////////////

/**
 * \brief 菜单
 *
 * 该类提供了对客户端动态菜单的支持.
 *
 */	
class MenuAction : public Action
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param menu: 菜单内容
	 */     	
	MenuAction(const std::string& menu) : _menu(menu)
	{ }
	
	virtual ~MenuAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _menu;	
};

/**
 * \brief 菜单
 *
 * 该类提供了对客户端动态菜单的支持.
 *
 */	
class SubMenuAction : public Action
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param menu: 菜单内容
	 */     	
	SubMenuAction(const std::string& menu) : _menu(menu)
	{ }
	
	virtual ~SubMenuAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _menu;	
};

/**
 * \brief 菜单
 *
 * 该类提供了对客户端动态菜单的支持.
 *
 */	
class MainMenuAction : public Action
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param menu: 菜单内容
	 */     	
	MainMenuAction(const std::string& menu) : _menu(menu)
	{ }
	
	virtual ~MainMenuAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _menu;	
};


/**
 * \brief 变量刷新
 *
 * 该类实现了对任务变量的及时刷新
 *
 */	
class RefreshAction : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	RefreshAction(Parse& p) : TeamAction(p)
	{ 
		p.value("name", _name);	
		p.value("id", _id);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~RefreshAction() { }	
	
	int done (SceneUser* user, Vars* vars);
private:
	std::string _name;
	int _id;
};

/**
 * \brief 经验
 *
 * 该类提供了增加特定用户经验的接口
 *
 */
class ExpAction : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	ExpAction(Parse& p) : TeamAction(p)
	{ 
		p.value("value", _exp);	
		p.value("name", _name);
		p.value("id", _id);
		p.value("percent", _percent);
		
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~ExpAction() { }	
	
	int done (SceneUser* user, Vars* vars);
protected:
	int _exp;
	std::string _name;
	int _id;
	DWORD _percent;
};



class Exp1Action : public ExpAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	Exp1Action(Parse& p) : ExpAction(p)
	{ 
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~Exp1Action() { }	
	
	int done (SceneUser* user, Vars* vars);
};


/**
 * \brief 银子
 *
 * 该类提供了修改用户身上银子数量的接口
 *
 */	
template <typename Operation>
class GoldAction : public TeamAction
{
public:
	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	GoldAction(Parse& p)  : TeamAction(p)
	{ 
		p.value("value", _value);
		
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~GoldAction() { }	

	/**     
	 * \brief  添加银子
	 *
	 * 重载了done函数,增加用户身上携带的银子
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;

		
		if (op.name() == "Add") 
		{
			user->packs.addMoney(_value,"任务添加");
			return Action::SUCCESS;
		}

		if (op.name() == "Sub") 
		{
			if (!user->packs.removeMoney(_value,"任务减少")) 
			{
				Zebra::logger->fatal("任务银子计算错误: 用户(%s), 任务(%d)", user->name, vars->quest_id());
			}
			
			return Action::SUCCESS;
		}
		
		return Action::FAILED;
	}
	
private:
	value_type _value;
};

/////////////////////////////////////////////////////////////
//soke 增加金子加减脚本
/////////////////////////////////////////////////////////////
/**
 * \brief 金子
 *
 * 该类提供了修改用户身上金子数量的接口
 *
 */	
template <typename Operation>
class ZiseGoldAction : public TeamAction
{
public:
	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	ZiseGoldAction(Parse& p)  : TeamAction(p)
	{ 
		p.value("value", _value);
		
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~ZiseGoldAction() { }	

	/**     
	 * \brief  添加金子
	 *
	 * 重载了done函数,增加用户身上携带的金子
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;

		
		if (op.name() == "Add") 
		{
			user->packs.addGold(_value,"任务添加");
			return Action::SUCCESS;
		}

		if (op.name() == "Sub") 
		{
			if (!user->packs.removeGold(_value,"任务减少")) 
			{
				Zebra::logger->fatal("任务金子计算错误: 用户(%s), 任务(%d)", user->name, vars->quest_id());
			}
			
			return Action::SUCCESS;
		}
		
		return Action::FAILED;
	}
	
private:
	value_type _value;
};

////////////////////////////////////////////////////////////
//soke 增加积分点数加减脚本
/////////////////////////////////////////////////////////////
/**
 * \brief 积分
 *
 * 该类提供了修改用户身上积分数量的接口
 *
 */	
template <typename Operation>
class TicketAction : public TeamAction
{
public:
	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	TicketAction(Parse& p)  : TeamAction(p)
	{ 
		p.value("value", _value);
		
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~TicketAction() { }	

	/**     
	 * \brief  添加积分
	 *
	 * 重载了done函数,增加用户身上携带的积分
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;

		
		if (op.name() == "Add") 
		{
			user->packs.addTicket(_value,"任务添加");
			return Action::SUCCESS;
		}

		if (op.name() == "Sub") 
		{
			if (!user->packs.removeTicket(_value,"任务减少")) 
			{
				Zebra::logger->fatal("任务积分计算错误: 用户(%s), 任务(%d)", user->name, vars->quest_id());
			}
			
			return Action::SUCCESS;
		}
		
		return Action::FAILED;
	}
	
private:
	value_type _value;
};

/////////////////////////////////////////////////////////////
//soke 增加工资加减脚本
/////////////////////////////////////////////////////////////
/**
 * \brief 工资
 *
 * 该类提供了修改用户身上工资数量的接口
 *
 */	
template <typename Operation>
class SalaryAction : public TeamAction
{
public:
	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	SalaryAction(Parse& p)  : TeamAction(p)
	{ 
		p.value("value", _value);
		
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~SalaryAction() { }	

	/**     
	 * \brief  添加工资
	 *
	 * 重载了done函数,增加用户身上携带的工资
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;

		
		if (op.name() == "Add") 
		{
			user->packs.addSalary(_value,"任务添加");
			return Action::SUCCESS;
		}

		if (op.name() == "Sub") 
		{
			if (!user->packs.removeSalary(_value,"任务减少")) 
			{
				Zebra::logger->fatal("任务工资计算错误: 用户(%s), 任务(%d)", user->name, vars->quest_id());
			}
			
			return Action::SUCCESS;
		}
		
		return Action::FAILED;
	}
	
private:
	value_type _value;
};

/////////////////////////////////////////////////////////////
//soke 增加VIP点数加减脚本
/////////////////////////////////////////////////////////////
/**
 * \brief VIP点数
 *
 * 该类提供了修改用户VIP点数的接口
 *
 */	
template <typename Operation>
class CharvipAction : public TeamAction
{
public:
	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	CharvipAction(Parse& p)  : TeamAction(p)
	{ 
		p.value("value", _value);
		
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~CharvipAction() { }	

	/**     
	 * \brief  添加VIP点数
	 *
	 * 重载了done函数,增加用户VIP点数
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;

		
		if (op.name() == "Add") 
		{
			user->packs.addCharvip(_value,"任务添加");
			return Action::SUCCESS;
		}

		if (op.name() == "Sub") 
		{
			if (!user->packs.removeCharvip(_value,"任务减少")) 
			{
				Zebra::logger->fatal("VIP点数计算错误: 用户(%s), 任务(%d)", user->name, vars->quest_id());
			}
			
			return Action::SUCCESS;
		}
		
		return Action::FAILED;
	}
	
private:
	value_type _value;
};
/////////////////////////////////////////////////////////////
//soke 增加充值点数加减脚本
/////////////////////////////////////////////////////////////
/**
 * \brief 充值点数
 *
 * 该类提供了修改用户充值点数的接口
 *
 */	
template <typename Operation>
class ChongzhiAction : public TeamAction
{
public:
	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	ChongzhiAction(Parse& p)  : TeamAction(p)
	{ 
		p.value("value", _value);
		
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~ChongzhiAction() { }	

	/**     
	 * \brief  添加充值点数
	 *
	 * 重载了done函数,增加用户充值点数
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;

		
		if (op.name() == "Add") 
		{
			user->packs.addChongzhi(_value,"任务添加");
			return Action::SUCCESS;
		}

		if (op.name() == "Sub") 
		{
			if (!user->packs.removeChongzhi(_value,"任务减少")) 
			{
				Zebra::logger->fatal("充值点数计算错误: 用户(%s), 任务(%d)", user->name, vars->quest_id());
			}
			
			return Action::SUCCESS;
		}
		
		return Action::FAILED;
	}
	
private:
	value_type _value;
};
/////////////////////////////////////////////////////////////
//soke 增加职业点数加减脚本
/////////////////////////////////////////////////////////////
/**
 * \brief 职业点数
 *
 * 该类提供了修改用户职业点数的接口
 *
 */	
template <typename Operation>
class ChangejobAction : public TeamAction
{
public:
	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	ChangejobAction(Parse& p)  : TeamAction(p)
	{ 
		p.value("value", _value);
		
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~ChangejobAction() { }	

	/**     
	 * \brief  添加职业点数
	 *
	 * 重载了done函数,增加用户职业点数
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;

		
		if (op.name() == "Add") 
		{
			user->packs.addChangejob(_value,"任务添加");
			return Action::SUCCESS;
		}

		if (op.name() == "Sub") 
		{
			if (!user->packs.removeChangejob(_value,"任务减少")) 
			{
				Zebra::logger->fatal("职业点数计算错误: 用户(%s), 任务(%d)", user->name, vars->quest_id());
			}
			
			return Action::SUCCESS;
		}
		
		return Action::FAILED;
	}
	
private:
	value_type _value;
};

///////////////////////////////////////////////////////////////////////

class EnterSeptGuardAction : public Action
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	EnterSeptGuardAction()
	{ 
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~EnterSeptGuardAction() { }	
	
	int done (SceneUser* user, Vars* vars);
private:
};

class FinishSeptGuardAction : public Action
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	FinishSeptGuardAction()
	{ 
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~FinishSeptGuardAction() { }	
	
	int done (SceneUser* user, Vars* vars);
};

/**
 * \brief 评价
 *
 * 该类提供了增加特定用户评价的接口,尚未实现
 *
 */
template <typename Operation>
class ScoreAction : public TeamAction
{
public:

	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	ScoreAction(Parse& p) : TeamAction(p)
	{ 
		p.value("value", _value);	
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~ScoreAction() { }	
	
	int done (SceneUser* user, Vars* vars)
	{
		return Action::SUCCESS;
	}
	
private:
	value_type _value;
};

/**
 * \brief 技能
 *
 * 该类提供了增加特定用户技能等级的接口.
 *
 */
class SkillAction : public TeamAction
{
public:
	typedef int value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	SkillAction(Parse& p) : TeamAction(p)
	{ 
		p.value("id", _id);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~SkillAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	value_type _id;
};

/**
 * \brief 禁止
 *
 * 该类提供了禁止用户执行某项动作的方法,当前支持的类型有使用物品,拾取物品及上下马.
 *
 */
class DisableAction : public TeamAction
{
public:
	typedef int value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	DisableAction(Parse& p)	: TeamAction(p)
	{ 
	
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~DisableAction() { }
	
	int done (SceneUser* user, Vars* vars);
	
private:

};

/**
 * \brief 地图跳转
 *
 * 该类提供了用户在地图上跳转的接口
 *
 */
class GotoAction : public TeamAction
{
public:
	typedef int value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	GotoAction(Parse& p) : TeamAction(p)
	{ 
		p.value("name", _name);	
		p.value("pos", _pos);
		p.value("pos1", _pos1);
		p.value("pos2", _pos2);
		p.value("pos3", _pos3);
		p.value("pos4", _pos4);
		p.value("cpos", _cpos);
		p.value("rlen", _rlen);
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~GotoAction() { }
	
	int done (SceneUser* user, Vars* vars);
	
private:
	std::string _name;
	std::string _pos;
	std::string _pos1;
	std::string _pos2;
	std::string _pos3;
	std::string _pos4;
	std::string _cpos;
	std::string _rlen;
};

/**
 * \brief 下马
 *
 * 该类提供了使用户下马的接口
 *
 */
class RideDownAction : public TeamAction
{
public:
	typedef int value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	RideDownAction(Parse& p) : TeamAction(p)
	{ }
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~RideDownAction() { }
	
	int done (SceneUser* user, Vars* vars);
	
private:

};

//soke 添加掉落物品
class AddDropItemAction : public TeamAction
{
public:
	typedef int value_type;

	/**     
	* \brief  构造函数
	*
	* 初始化相关变量
	*      
	* \param p: 关键词列表
	*/     	
	AddDropItemAction(Parse& p) : TeamAction(p)
	{ 
		p.value("id", _id);
		p.value("value", _value);	
		p.value("odds", _odds);
	}

	/**     
	* \brief 析构函数
	*
	*/     
	virtual ~AddDropItemAction() { }

	int done (SceneUser* user, Vars* vars);

private:
	int _id;
	int _value;
	int _odds;
};

//soke 添加随机给予道具
/**
* \brief 添加物品
*
* 该类提供了给用户添加某个物品的接口
*
*/
class AddRandItemAction : public TeamAction
{
public:
	typedef int value_type;

	/**     
	* \brief  构造函数
	*
	* 初始化相关变量
	*      
	* \param p: 关键词列表
	*/     	
	AddRandItemAction(Parse& p) : TeamAction(p)
	{ 
		p.value("id", _id);
		p.value("value", _value);	
		p.value("odds", _odds);
		p.value("id2", _id2);
		p.value("value2", _value2);	
		p.value("odds2", _odds2);
		p.value("id3", _id3);
		p.value("value3", _value3);	
		p.value("odds3", _odds3);
		p.value("id4", _id4);
		p.value("value4", _value4);	
		p.value("odds4", _odds4);
		p.value("id5", _id5);
		p.value("value5", _value5);	
		p.value("odds5", _odds5);
		p.value("id6", _id6);
		p.value("value6", _value6);	
		p.value("odds6", _odds6);
		p.value("id7", _id7);
		p.value("value7", _value7);	
		p.value("odds7", _odds7);
		p.value("id8", _id8);
		p.value("value8", _value8);	
		p.value("odds8", _odds8);
		p.value("id9", _id9);
		p.value("value9", _value9);	
		p.value("odds9", _odds9);
		p.value("id10", _id10);
		p.value("value10", _value10);	
		p.value("odds10", _odds10);
		p.value("id11", _id11);
		p.value("value11", _value11);	
		p.value("odds11", _odds11);
		p.value("id12", _id12);
		p.value("value12", _value12);	
		p.value("odds12", _odds12);
		p.value("id13", _id13);
		p.value("value13", _value13);	
		p.value("odds13", _odds13);
		p.value("id14", _id14);
		p.value("value14", _value14);	
		p.value("odds14", _odds14);
		p.value("id15", _id15);
		p.value("value15", _value15);	
		p.value("odds15", _odds15);
		p.value("id16", _id16);
		p.value("value16", _value16);	
		p.value("odds16", _odds16);
		p.value("id17", _id17);
		p.value("value17", _value17);	
		p.value("odds17", _odds17);
		p.value("id18", _id18);
		p.value("value18", _value18);	
		p.value("odds18", _odds18);
		p.value("id19", _id19);
		p.value("value19", _value19);	
		p.value("odds19", _odds19);
		p.value("id20", _id20);
		p.value("value20", _value20);	
		p.value("odds20", _odds20);	
	}

	/**     
	* \brief 析构函数
	*
	*/     
	virtual ~AddRandItemAction() { }

	int done (SceneUser* user, Vars* vars);

private:
	int _id;
	int _value;
	int _odds;
	int _id2,_id3,_id4,_id5,_id6,_id7,_id8,_id9,_id10,_id11,_id12,_id13,_id14,_id15,_id16,_id17,_id18,_id19,_id20;
	int _odds2,_odds3,_odds4,_odds5,_odds6,_odds7,_odds8,_odds9,_odds10,_odds11,_odds12,_odds13,_odds14,_odds15,_odds16,_odds17,_odds18,_odds19,_odds20;
	int _value2,_value3,_value4,_value5,_value6,_value7,_value8,_value9,_value10,_value11,_value12,_value13,_value14,_value15,_value16,_value17,_value18,_value19,_value20;
};
/////////////////////////////////////////////////////////

//soke 添加随机给予绑定道具
/**
* \brief 添加物品
*
* 该类提供了给用户添加某个物品的接口
*
*/
class AddRandBindItemAction : public TeamAction
{
public:
	typedef int value_type;

	/**     
	* \brief  构造函数
	*
	* 初始化相关变量
	*      
	* \param p: 关键词列表
	*/     	
	AddRandBindItemAction(Parse& p) : TeamAction(p)
	{ 
		p.value("id", _id);
		p.value("value", _value);	
		p.value("odds", _odds);
		p.value("id2", _id2);
		p.value("value2", _value2);	
		p.value("odds2", _odds2);
		p.value("id3", _id3);
		p.value("value3", _value3);	
		p.value("odds3", _odds3);
		p.value("id4", _id4);
		p.value("value4", _value4);	
		p.value("odds4", _odds4);
		p.value("id5", _id5);
		p.value("value5", _value5);	
		p.value("odds5", _odds5);
		p.value("id6", _id6);
		p.value("value6", _value6);	
		p.value("odds6", _odds6);
		p.value("id7", _id7);
		p.value("value7", _value7);	
		p.value("odds7", _odds7);
		p.value("id8", _id8);
		p.value("value8", _value8);	
		p.value("odds8", _odds8);
		p.value("id9", _id9);
		p.value("value9", _value9);	
		p.value("odds9", _odds9);
		p.value("id10", _id10);
		p.value("value10", _value10);	
		p.value("odds10", _odds10);
		p.value("id11", _id11);
		p.value("value11", _value11);	
		p.value("odds11", _odds11);
		p.value("id12", _id12);
		p.value("value12", _value12);	
		p.value("odds12", _odds12);
		p.value("id13", _id13);
		p.value("value13", _value13);	
		p.value("odds13", _odds13);
		p.value("id14", _id14);
		p.value("value14", _value14);	
		p.value("odds14", _odds14);
		p.value("id15", _id15);
		p.value("value15", _value15);	
		p.value("odds15", _odds15);
		p.value("id16", _id16);
		p.value("value16", _value16);	
		p.value("odds16", _odds16);
		p.value("id17", _id17);
		p.value("value17", _value17);	
		p.value("odds17", _odds17);
		p.value("id18", _id18);
		p.value("value18", _value18);	
		p.value("odds18", _odds18);
		p.value("id19", _id19);
		p.value("value19", _value19);	
		p.value("odds19", _odds19);
		p.value("id20", _id20);
		p.value("value20", _value20);	
		p.value("odds20", _odds20);	
	}

	/**     
	* \brief 析构函数
	*
	*/     
	virtual ~AddRandBindItemAction() { }

	int done (SceneUser* user, Vars* vars);

private:
	int _id;
	int _value;
	int _odds;
	int _id2,_id3,_id4,_id5,_id6,_id7,_id8,_id9,_id10,_id11,_id12,_id13,_id14,_id15,_id16,_id17,_id18,_id19,_id20;
	int _odds2,_odds3,_odds4,_odds5,_odds6,_odds7,_odds8,_odds9,_odds10,_odds11,_odds12,_odds13,_odds14,_odds15,_odds16,_odds17,_odds18,_odds19,_odds20;
	int _value2,_value3,_value4,_value5,_value6,_value7,_value8,_value9,_value10,_value11,_value12,_value13,_value14,_value15,_value16,_value17,_value18,_value19,_value20;
};


/**
 * \brief 添加物品
 *
 * 该类提供了给用户添加某个物品的接口
 *
 */
class AddItemAction : public TeamAction
{
public:
	typedef int value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	AddItemAction(Parse& p) : TeamAction(p)
	{ 
		p.value("id", _id);
		p.value("value", _value);	
		p.value("odds", _odds);
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~AddItemAction() { }
	
	int done (SceneUser* user, Vars* vars);
	
private:
	int _id;
	int _value;
	int _odds;
};

/**
 * \brief 添加绑定物品
 *
 * 该类提供了给用户添加某个物品的接口
 *
 */
class AddBindItemAction : public TeamAction
{
public:
	typedef int value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	AddBindItemAction(Parse& p) : TeamAction(p)
	{ 
		p.value("id", _id);
		p.value("value", _value);	
		p.value("odds", _odds);
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~AddBindItemAction() { }
	
	int done (SceneUser* user, Vars* vars);
	
private:
	int _id;
	int _value;
	int _odds;
};

/**
 * \brief 添加等级物品
 *
 * 该类提供了给用户添加某个物品的接口
 *
 */
class AddGradeItemAction : public TeamAction
{
public:
	typedef int value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	AddGradeItemAction(Parse& p) : TeamAction(p)
	{ 
		p.value("id", _id);
		p.value("value", _value);	
		p.value("odds", _odds);
		p.value("level", _level);//soke 增加物品等级
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~AddGradeItemAction() { }
	
	int done (SceneUser* user, Vars* vars);
	
private:
	int _id;
	int _value;
	int _odds;
	int _level;//soke 增加物品等级
};

/**
 * \brief 添加蓝色绑定物品
 *
 * 该类提供了给用户添加某个物品的接口
 *
 */
class AddBlueBindItemAction : public TeamAction
{
public:
	typedef int value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	AddBlueBindItemAction(Parse& p) : TeamAction(p)
	{ 
		p.value("id", _id);
		p.value("value", _value);	
		p.value("odds", _odds);
		p.value("maker", _maker);
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~AddBlueBindItemAction() { }
	
	int done (SceneUser* user, Vars* vars);
	
private:
	int _id;
	int _value;
	int _odds;
	int _maker;
};

/**
 * \brief 添加黄色绑定物品
 *
 * 该类提供了给用户添加某个物品的接口
 *
 */
class AddGoldBindItemAction : public TeamAction
{
public:
	typedef int value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	AddGoldBindItemAction(Parse& p) : TeamAction(p)
	{ 
		p.value("id", _id);
		p.value("value", _value);	
		p.value("odds", _odds);
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~AddGoldBindItemAction() { }
	
	int done (SceneUser* user, Vars* vars);
	
private:
	int _id;
	int _value;
	int _odds;
};

/**
 * \brief 添加绿色绑定物品
 *
 * 该类提供了给用户添加某个物品的接口
 *
 */
class AddGreenBindItemAction : public TeamAction
{
public:
	typedef int value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	AddGreenBindItemAction(Parse& p) : TeamAction(p)
	{ 
		p.value("id", _id);
		p.value("value", _value);	
		p.value("level", _level);	
		p.value("odds", _odds);
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~AddGreenBindItemAction() { }
	
	int done (SceneUser* user, Vars* vars);
	
private:
	int _id;
	int _value;
	int _level;
	int _odds;
};

/**
 * \brief 添加紫色绑定物品
 *
 * 该类提供了给用户添加某个物品的接口
 *
 */
class AddPurpleBindItemAction : public TeamAction
{
public:
	typedef int value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	AddPurpleBindItemAction(Parse& p) : TeamAction(p)
	{ 
		p.value("id", _id);
		p.value("value", _value);	
		p.value("level", _level);
		p.value("odds", _odds);

	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~AddPurpleBindItemAction() { }
	
	int done (SceneUser* user, Vars* vars);
	
private:
	int _id;
	int _value;
	int _level;
	int _odds;

};

/**
 * \brief 删除物品
 *
 * 该类提供了删除用户身上某个物品的接口
 *
 */
class RemoveItemAction : public TeamAction
{
public:
	typedef int value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	RemoveItemAction(Parse& p) : TeamAction(p)
	{ 
		p.value("id", _id);
		p.value("value", _value);			
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~RemoveItemAction() { }
	
	int done (SceneUser* user, Vars* vars);
	
private:
	int _id;
	int _value;
};

/**
 * \brief 丢弃物品
 *
 * 该类提供了丢弃用户身上某个物品的接口
 *
 */
class DropItemAction : public TeamAction
{
public:
	typedef int value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	DropItemAction(Parse& p) : TeamAction(p)
	{ 
		p.value("id", _id);
		p.value("level", _level);
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~DropItemAction() { }
	
	int done (SceneUser* user, Vars* vars);
	
private:
	int _id;
	int _level;
};

/**
 * \brief 未实现
 *
 * 
 *
 */
class DropAction : public Action
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	DropAction(Parse& p)
	{
		p.value("id", _id);
		p.value("value", _value);	
		p.value("odds", _odds);
		p.value("guard", _guard);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~DropAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	int _id;
	int _value;
	int _odds;
	int _guard;
};

/**
 * \brief 定时器
 *
 * 该类提供了对需要时间限制的任务的支持
 *
 */
class TimeoutsAction : public Action
{
public:

	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	TimeoutsAction(Parse& p)
	{
		//p.value("value", _timeout);
		p.value("id", _id);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~TimeoutsAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	//int _timeout;
	int _id;
};

/**
 * \brief 设置状态
 *
 * 该类提供了设置用户身上某个状态的接口
 *
 */
class SetStateAction : public TeamAction
{
public:

	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	SetStateAction(Parse& p) : TeamAction(p)
	{
		p.value("value", _state);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~SetStateAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	int _state;
};

/**
 * \brief 清除状态
 *
 * 该类提供了清除用户身上某个状态的接口
 *
 */
class ClearStateAction : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	ClearStateAction(Parse& p) : TeamAction(p)
	{
		p.value("value", _state);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~ClearStateAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	int _state;
};

/**
 * \brief 马匹
 *
 * 该类提供了给予用户马匹的接口
 *
 */
class HorseAction : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	HorseAction(Parse& p) : TeamAction(p)
	{
		p.value("value", _id);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~HorseAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	int _id;
};


/**
 * \brief 全局变量
 *
 * 该类提供了对于任务脚本中需要对所有用户可见变量的支持
 *
 */	
template <typename Operation>
class GlobalAction : public Action
{
public:
	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	GlobalAction(Parse& p) : _id(0)
	{ 
		p.value("name", _name);
		p.value("value", _action);
		p.value("id", _id);
		p.value("tmp", _tmp);		
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~GlobalAction()
	{ }
	
	bool check_args(SceneUser* user, Vars* vars) const
	{
		if (!vars) return false;
		
		return true;		
	}

	/**     
	 * \brief  执行变量修改
	 *
	 * 重载了done函数,实现对相关变量的修改
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */     	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;
		if (!_id) _id = vars->quest_id();

//		Zebra::logger->debug("id:%d\tname:%s\taction:%d\toperation:%s", _id, _name.c_str(), _action, op.name().c_str());
				
		Vars* v = GlobalVar::instance().add(_id);
		v->set_value(op, _name, _action, _tmp, user);

		return Action::SUCCESS;
	}
	
protected:
	std::string _name;
	value_type _action;
	int _tmp;
	int _id;
};

template <typename Operation>
class TongVarAction : public GlobalAction<Operation>
{
public:
	typedef typename Operation::value_type value_type;
	
	TongVarAction(Parse& p) : GlobalAction<Operation>(p)
	{ 
	}

	virtual ~TongVarAction()
	{ }
	
	bool check_args(SceneUser* user, Vars* vars) const
	{
		if (!user) return false;
		
		return true;		
	}

	int done (SceneUser* user, Vars* vars)
	{
		Operation op;
				
		Vars* v = TongVar::instance().add(user->charbase.unionid);
		v->set_value(op, GlobalAction<Operation>::_name, GlobalAction<Operation>::_action, GlobalAction<Operation>::_tmp, user);

		return Action::SUCCESS;
	}	
};

#include "SessionClient.h"
template <typename Operation>
class FamilyLevelAction : public TongVarAction<Operation>
{
public:
	typedef typename Operation::value_type value_type;
	
	FamilyLevelAction(Parse& p) : TongVarAction<Operation>(p),_level(0)
	{ 
		p.value("value" ,_level);
		Zebra::logger->debug("初始化_level%d", _level);
	}

	virtual ~FamilyLevelAction()
	{ }

	int done (SceneUser* user, Vars* vars)
	{
		Cmd::Session::t_OpLevel_SceneSession uplevel;
		uplevel.dwSeptID=user->charbase.septid;
		uplevel.dwLevel=_level;
		Zebra::logger->debug("发送_level%d", _level);
		sessionClient->sendCmd(&uplevel,sizeof(uplevel));
		return Action::SUCCESS;
	}
	
private:
	value_type _level;
};

template <typename Operation>
class FamilyVarAction : public TongVarAction<Operation>
{
public:
	typedef typename Operation::value_type value_type;
	
	FamilyVarAction(Parse& p) : TongVarAction<Operation>(p)
	{ 
	}

	virtual ~FamilyVarAction()
	{ }
	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;
				
		Vars* v = FamilyVar::instance().add(user->charbase.septid);
		v->set_value(op, GlobalAction<Operation>::_name, GlobalAction<Operation>::_action, GlobalAction<Operation>::_tmp, user);

		return Action::SUCCESS;
	}	
};

template <typename Operation>
class UsersVarAction : public TongVarAction<Operation>
{
public:
	typedef typename Operation::value_type value_type;
	
	UsersVarAction(Parse& p) : TongVarAction<Operation>(p)
	{ 
	}

	virtual ~UsersVarAction()
	{ }
	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;
				
		Vars* v = UsersVar::instance().add(((QWORD)user->charbase.accid << 32) |user->charbase.id);
		v->set_value(op, GlobalAction<Operation>::_name, GlobalAction<Operation>::_action, GlobalAction<Operation>::_tmp, user);

		return Action::SUCCESS;
	}	
};

#include "zRegex.h"
#include "SceneNpc.h"
#include "SceneManager.h"

/**
 * \brief  NPC
 *
 * 该类提供了对场景上NPC访问的支持
 *
 */
class NpcAction : public Action
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	NpcAction(Parse& p)
	{ 
		p.value("id", _id);
		std::string map;
		p.value("map", map);
		_s = SceneManager::getInstance().getSceneByName(map.c_str());
		if (!_s) 
		{
		//soke 取消显示日志
		//	Zebra::logger->debug("NpcAction时请求的场景(%s)不存在", map.c_str());
		}
		
		std::string pos;		
		p.value("ltpos", pos);	
		zRegex regex;
		if((regex.compile("(.*)[ ,](.*)") && regex.match(pos.c_str()))) 
		{
			std::string s;
			regex.getSub(s, 1);
			_ltpos.x = atoi(s.c_str());
			regex.getSub(s, 2);
			_ltpos.y = atoi(s.c_str());
		}

		p.value("rbpos", pos);	
		if((regex.compile("(.*)[ ,](.*)") && regex.match(pos.c_str()))) {
			std::string s;
			regex.getSub(s, 1);
			_rbpos.x = atoi(s.c_str());
			regex.getSub(s, 2);
			_rbpos.y = atoi(s.c_str());
		}
		
		p.value("num", _num);
		if (!_num) _num = 1;
		if (_num>100) _num = 100;
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~NpcAction()
	{ }
	
	bool check_args(SceneUser* user, Vars* vars) const
	{
		return true;
	}
		
protected:
	Scene* _s;
	int _id;
	zPos _ltpos;
	zPos _rbpos;
	int _num;
	
};

/**
 * \brief 增加NPC
 *
 * 该类提供了在地图上增加一个NPC的接口
 *
 */
class AddNpcAction : public NpcAction
{
public:

	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	AddNpcAction(Parse& p) : NpcAction(p)
	{ }
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	~AddNpcAction()
	{ }
	
	int done(SceneUser* user, Vars* vars);	
	
};

/**
 * \brief 增加镖车
 *
 * 该类提供了对护镖任务的支持
 *
 */
class AddGuardAction : public NpcAction
{
public:

	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	AddGuardAction(Parse& p) : NpcAction(p)
	{ 
		p.value("id2", _id2);	
		p.value("id3", _id3);	
		p.value("odds1", _odds1);	
		p.value("odds2", _odds2);	
		p.value("odds3", _odds3);	
		p.value("gold", _gold);	
		p.value("path", _path);
		p.value("exp", _exp);
		p.value("script", _script);
		p.value("map", _map);

		if (_odds1+_odds2+_odds3==0)
			_odds1 = 100;

		std::string pos;		
		p.value("dest", pos);	
		zRegex regex;
		if((regex.compile("(.*)[ ,](.*)") && regex.match(pos.c_str()))) {
			std::string s;
			regex.getSub(s, 1);
			_dest.x = atoi(s.c_str());
			regex.getSub(s, 2);
			_dest.y = atoi(s.c_str());
			//Zebra::logger->debug("AddGuardAction::AddGuardAction(): _dest=%u,%u", _dest.x, _dest.y);
		}
		else
			Zebra::logger->trace("AddGuardAction::AddGuardAction(): _dest匹配失败 %s", pos.c_str());
		
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	~AddGuardAction()
	{ }
	
	int done(SceneUser* user, Vars* vars);	

	bool check_args(SceneUser* user, Vars* vars) const
	{
		if (user) return true;
		
		return false;
	}
	
	private:
		std::string _path;
		int _gold;
		int _exp;
		int _script;
		zPos _dest;
		std::string _map;
		DWORD _id2, _id3;
		DWORD _odds1, _odds2, _odds3;
};

//soke 镖车
class ChangeGuardAction : public NpcAction
{
public:

	/**     
	* \brief  构造函数
	*
	* 初始化相关变量
	*      
	* \param p: 关键词列表
	*/     	
	ChangeGuardAction(Parse& p) : NpcAction(p)
	{ 
		p.value("id2", _id2);	
		p.value("id3", _id3);	
		p.value("odds1", _odds1);	
		p.value("odds2", _odds2);	
		p.value("odds3", _odds3);	
		p.value("gold", _gold);	
		p.value("path", _path);
		p.value("exp", _exp);
		p.value("script", _script);
		p.value("map", _map);

		if (_odds1+_odds2+_odds3==0)
			_odds1 = 100;

		std::string pos;		
		p.value("dest", pos);	
		zRegex regex;
		if((regex.compile("(.*)[ ,](.*)") && regex.match(pos.c_str()))) {
			std::string s;
			regex.getSub(s, 1);
			_dest.x = atoi(s.c_str());
			regex.getSub(s, 2);
			_dest.y = atoi(s.c_str());
			//Zebra::logger->debug("AddGuardAction::AddGuardAction(): _dest=%u,%u", _dest.x, _dest.y);
		}
		else
			Zebra::logger->trace("ChangeGuardAction::ChangeGuardAction(): _dest匹配失败 %s", pos.c_str());

	}

	/**     
	* \brief 析构函数
	*
	*/     
	~ChangeGuardAction()
	{ }

	int done(SceneUser* user, Vars* vars);	

	bool check_args(SceneUser* user, Vars* vars) const
	{
		if (user) return true;

		return false;
	}

private:
	std::string _path;
	int _gold;
	int _exp;
	int _script;
	zPos _dest;
	std::string _map;
	DWORD _id2, _id3;
	DWORD _odds1, _odds2, _odds3;
};

/**
 * \brief 删除NPC
 *
 * 该类提供了在地图上删除一个NPC的接口
 *
 */
class RemoveNpcAction : public NpcAction
{
public:

	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	RemoveNpcAction(Parse& p) : NpcAction(p), _remove(_s, _id, _ltpos, _rbpos)
	{ }
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	~RemoveNpcAction()
	{ }
	
	int done(SceneUser* user, Vars* vars);		
private:

	class RemoveNpc : public removeEntry_Pred<SceneNpc>
	{
	public:		
		RemoveNpc(Scene* s, int id, const zPos& ltpos, const zPos& rbpos) : _s(s), _id(id), _ltpos(ltpos), _rbpos(rbpos)
		{
		}

		bool isIt(SceneNpc *npc)
		{
			if ((int)npc->id == _id && (npc->getPos() >= _ltpos && npc->getPos() <= _rbpos) ) {
				npc->setState(zSceneEntry::SceneEntry_Hide);
				Cmd::stAddMapNpcMapScreenUserCmd addNpc;
				npc->full_t_MapNpcData(addNpc.data);
				_s->sendCmdToNine(npc->getPosI(), &addNpc, sizeof(addNpc));
				return true;
			}

			return false;
		}
	private:
		Scene* _s;
		int _id;
		const zPos& _ltpos;
		const zPos& _rbpos;
	};
	
	RemoveNpc _remove;
};

/**
 * \brief 变量
 *
 * 该类提供了对于任务脚本中有关五行点数的修改的支持
 *
 */	
template <typename Operation>
class FiveLevelAction : public TeamAction
{
public:
	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	FiveLevelAction(Parse& p) : TeamAction(p)
	{ 
		p.value("value", _level);
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~FiveLevelAction()
	{ }

	/**     
	 * \brief  执行变量修改
	 *
	 * 重载了done函数,实现对相关变量的修改
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */     	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;
		op((int &)user->charbase.fivelevel, _level, user);

		Cmd::stMainUserDataUserCmd  userinfo;
		user->full_t_MainUserData(userinfo.data);
		user->sendCmdToMe(&userinfo,sizeof(userinfo));

		return Action::SUCCESS;
	}
	
private:
	value_type _level;
};

/**
 * \brief 设置五行类型
 *
 * 该类提供了设置五行类型的接口
 *
 */
class FiveTypeAction : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	FiveTypeAction(Parse& p) : TeamAction(p)
	{
		p.value("value", _type);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~FiveTypeAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	int _type;
};

template <typename Operation>
class HonorAction : public TeamAction
{
public:
	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	HonorAction(Parse& p) : TeamAction(p)
	{ 
		p.value("value", _honor);
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~HonorAction()
	{ }

	/**     
	 * \brief  执行变量修改
	 *
	 * 重载了done函数,实现对相关变量的修改
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */     	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;
		op((int &)user->charbase.honor, _honor, user);

		Cmd::stMainUserDataUserCmd  userinfo;
		user->full_t_MainUserData(userinfo.data);
		user->sendCmdToMe(&userinfo,sizeof(userinfo));

		return Action::SUCCESS;
	}
	
private:
	value_type _honor;
};

template <typename Operation>
class MaxHonorAction : public TeamAction
{
public:
	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	MaxHonorAction(Parse& p) : TeamAction(p)
	{ 
		p.value("value", _honor);
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~MaxHonorAction()
	{ }

	/**     
	 * \brief  执行变量修改
	 *
	 * 重载了done函数,实现对相关变量的修改
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */     	
	int done (SceneUser* user, Vars* vars)
	{
		Operation op;
		op((int &)user->charbase.maxhonor, _honor, user);

		Cmd::stMainUserDataUserCmd  userinfo;
		user->full_t_MainUserData(userinfo.data);
		user->sendCmdToMe(&userinfo,sizeof(userinfo));

		return Action::SUCCESS;
	}
	
private:
	value_type _honor;
};


template <typename Operation>
class ActionPointAction : public TeamAction
{
public:
	typedef typename Operation::value_type value_type;
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	ActionPointAction(Parse& p) : TeamAction(p)
	{ 
		p.value("value", _ap);
	}

	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~ActionPointAction()
	{ }

	/**     
	 * \brief  执行变量修改
	 *
	 * 重载了done函数,实现对相关变量的修改
	 *      
	 * \param user: 触发动作的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return SUCCESS表示成功，FAILED表示失败，DISABLE表示禁用某项功能
	 */     	
	int done (SceneUser* user, Vars* vars)
	{
		Cmd::Session::t_ChangeAP  cmd;
		cmd.id = user->charbase.unionid;
		cmd.point = _ap;
		sessionClient->sendCmd(&cmd, sizeof(cmd));
		
		return Action::SUCCESS;
	}
	
private:
	value_type _ap;
};

class UseSkillAction : public TeamAction
{
public:
	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	UseSkillAction(Parse& p) : TeamAction(p)
	{
		p.value("id", _id);
		p.value("level", _level);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	virtual ~UseSkillAction() { }	
	
	int done (SceneUser* user, Vars* vars);
	
private:
	int _id;
	int _level;
};



#endif
